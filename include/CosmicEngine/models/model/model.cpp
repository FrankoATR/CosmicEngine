/**
 * @file model.cpp
 * @brief Implements the Assimp-based 3D model loader used by the engine.
 */

#include "model.hpp"

#include "../../utils/log.hpp"

#include <algorithm>
#include <glm/gtc/matrix_inverse.hpp>
#include <limits>

namespace CosmicEngine
{
    namespace
    {
        glm::mat4 AssimpMatrixToGlm(const aiMatrix4x4 &matrix)
        {
            glm::mat4 result;
            result[0][0] = matrix.a1;
            result[1][0] = matrix.a2;
            result[2][0] = matrix.a3;
            result[3][0] = matrix.a4;
            result[0][1] = matrix.b1;
            result[1][1] = matrix.b2;
            result[2][1] = matrix.b3;
            result[3][1] = matrix.b4;
            result[0][2] = matrix.c1;
            result[1][2] = matrix.c2;
            result[2][2] = matrix.c3;
            result[3][2] = matrix.c4;
            result[0][3] = matrix.d1;
            result[1][3] = matrix.d2;
            result[2][3] = matrix.d3;
            result[3][3] = matrix.d4;
            return result;
        }

        void ExpandSceneBounds(
            aiNode *node,
            const aiScene *scene,
            const glm::mat4 &parentTransform,
            glm::vec3 &minimum,
            glm::vec3 &maximum)
        {
            const glm::mat4 nodeTransform = parentTransform * AssimpMatrixToGlm(node->mTransformation);

            for (unsigned int meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[meshIndex]];

                for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
                {
                    const aiVector3D &vertex = mesh->mVertices[vertexIndex];
                    const glm::vec3 transformedVertex = glm::vec3(
                        nodeTransform * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f));

                    minimum = glm::min(minimum, transformedVertex);
                    maximum = glm::max(maximum, transformedVertex);
                }
            }

            for (unsigned int childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
            {
                ExpandSceneBounds(node->mChildren[childIndex], scene, nodeTransform, minimum, maximum);
            }
        }
    }

    Model::Model(string const &path, bool gamma) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    void Model::draw(Shader &shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].draw(shader);
    }

    void Model::loadModel(string const &path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
			RUNTIME_WARNING("[Model] Assimp error: " << importer.GetErrorString());
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        glm::vec3 minimum(std::numeric_limits<float>::max());
        glm::vec3 maximum(std::numeric_limits<float>::lowest());
        ExpandSceneBounds(scene->mRootNode, scene, glm::mat4(1.0f), minimum, maximum);

        if (minimum.x <= maximum.x && minimum.y <= maximum.y && minimum.z <= maximum.z)
        {
            normalizationCenter = (minimum + maximum) * 0.5f;
            const glm::vec3 size = maximum - minimum;
            const float maxExtent = std::max(size.x, std::max(size.y, size.z));
            normalizationScale = maxExtent > 0.0001f ? 1.0f / maxExtent : 1.0f;
        }

        processNode(scene->mRootNode, scene, glm::mat4(1.0f));
    }

    void Model::processNode(aiNode *node, const aiScene *scene, const glm::mat4 &parentTransform)
    {
        const glm::mat4 nodeTransform = parentTransform * AssimpMatrixToGlm(node->mTransformation);

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene, nodeTransform));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, nodeTransform);
        }
    }

    Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene, const glm::mat4 &nodeTransform)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        const glm::mat3 normalTransform = glm::transpose(glm::inverse(glm::mat3(nodeTransform)));

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex{};
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = (glm::vec3(nodeTransform * glm::vec4(vector, 1.0f)) - normalizationCenter) * normalizationScale;
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = glm::normalize(normalTransform * vector);
            }
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = glm::normalize(normalTransform * vector);
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = glm::normalize(normalTransform * vector);
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        vector<Texture> diffuseMaps = loadMaterialTextures(material, scene, aiTextureType_DIFFUSE, "texture_diffuse");
        if (diffuseMaps.empty())
        {
            diffuseMaps = loadMaterialTextures(material, scene, aiTextureType_BASE_COLOR, "texture_diffuse");
        }
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        vector<Texture> specularMaps = loadMaterialTextures(material, scene, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = loadMaterialTextures(material, scene, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> heightMaps = loadMaterialTextures(material, scene, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures);
    }

    vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, const aiScene *scene, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                Texture texture;
                const aiTexture *embeddedTexture = scene ? scene->GetEmbeddedTexture(str.C_Str()) : nullptr;
                texture.id = embeddedTexture
                    ? TextureFromEmbedded(embeddedTexture, str.C_Str())
                    : TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    unsigned int Model::TextureFromEmbedded(const aiTexture *textureData, const string &textureName, bool gamma)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width = 0;
        int height = 0;
        int nrComponents = 0;
        unsigned char *decodedData = nullptr;
        GLenum internalFormat = GL_RGB;
        GLenum dataFormat = GL_RGB;
        const void *pixelData = nullptr;

        if (textureData->mHeight == 0)
        {
            decodedData = stbi_load_from_memory(
                reinterpret_cast<const stbi_uc *>(textureData->pcData),
                static_cast<int>(textureData->mWidth),
                &width,
                &height,
                &nrComponents,
                0);

            if (!decodedData)
            {
                RUNTIME_WARNING("[Model] Embedded texture failed to decode: " << textureName);
                return textureID;
            }

            if (nrComponents == 1)
            {
                internalFormat = GL_RED;
                dataFormat = GL_RED;
            }
            else if (nrComponents == 4)
            {
                internalFormat = GL_RGBA;
                dataFormat = GL_RGBA;
            }

            pixelData = decodedData;
        }
        else
        {
            width = static_cast<int>(textureData->mWidth);
            height = static_cast<int>(textureData->mHeight);
            internalFormat = GL_RGBA;
            dataFormat = GL_BGRA;
            pixelData = textureData->pcData;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, pixelData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (decodedData)
        {
            stbi_image_free(decodedData);
        }

        return textureID;
    }

    unsigned int Model::TextureFromFile(const char *path, const string &directory, bool gamma)
    {
        string filename = string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
			RUNTIME_WARNING("[Model] Texture failed to load at path: " << path);
            stbi_image_free(data);
        }

        return textureID;
    }
}