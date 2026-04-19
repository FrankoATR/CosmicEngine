#ifndef COSMIC_MESH_HPP
#define COSMIC_MESH_HPP

/**
 * @file mesh.hpp
 * @brief Declares the mesh type used by the engine model renderer.
 */

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../shader/shader.hpp"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

namespace CosmicEngine
{

    /**
     * @brief Describes a renderable mesh vertex, including optional skeletal animation data.
     */
    struct Vertex
    {
        /** @brief Vertex position. */
        glm::vec3 Position;
        /** @brief Vertex normal. */
        glm::vec3 Normal;
        /** @brief Texture coordinates. */
        glm::vec2 TexCoords;
        /** @brief Tangent vector used for advanced shading. */
        glm::vec3 Tangent;
        /** @brief Bitangent vector used for advanced shading. */
        glm::vec3 Bitangent;
        /** @brief Bone indices affecting the vertex. */
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        /** @brief Bone weights affecting the vertex. */
        float m_Weights[MAX_BONE_INFLUENCE];
    };

    /**
     * @brief Describes a texture referenced by a mesh material.
     */
    struct Texture
    {
        /** @brief OpenGL texture identifier. */
        unsigned int id;
        /** @brief Logical texture type, such as diffuse or specular. */
        string type;
        /** @brief Original source path of the texture. */
        string path;
    };

    /**
     * @brief Represents a GPU-ready mesh with vertices, indices, and material textures.
     *
     * Mesh is the lowest-level renderable unit.  A Model (loaded via Assimp)
     * is made up of one or more Mesh instances.  Each mesh uploads its vertex
     * and index data to the GPU on construction and provides a Draw() method
     * used by the Model class.  Game code does not normally create Mesh objects
     * directly; they are produced by the Model loader.
     */
    class Mesh
    {
    public:
        /** @brief Vertex buffer data owned by the mesh. */
        vector<Vertex> vertices;
        /** @brief Index buffer data owned by the mesh. */
        vector<unsigned int> indices;
        /** @brief Material textures referenced by the mesh. */
        vector<Texture> textures;
        /** @brief Vertex array object used to render the mesh. */
        unsigned int VAO;

        /**
         * @brief Creates a mesh and uploads its geometry to the GPU.
         * @param vertices Vertex data.
         * @param indices Index data.
         * @param textures Material texture bindings.
         */
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

        /**
         * @brief Draws the mesh using the provided shader.
         * @param shader Shader used for rendering.
         */
        void draw(Shader &shader);

    private:
        unsigned int VBO, EBO;

        void setupMesh();
    };
}

#endif // COSMIC_MESH_HPP