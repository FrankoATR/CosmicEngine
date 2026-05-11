#include "ResourceManager.hpp"
#include "MasterShaders.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

namespace WandEngine
{

    ResourceManager::ResourceManager()
    {
        Init();

    }

    void ResourceManager::Init()
    {
        //DefaultVAOTexture
        std::vector<std::vector<float>> vertices = { 
            {0.0f, 1.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f}, 
            {0.0f, 1.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 1.0f, 0.0f}
        };

        LoadVAO("Sprite", vertices);

        LoadShaderFromCode("Shape", shape_vs, shape_fs);
        LoadShaderFromCode("Sprite", sprite_vs, sprite_fs);
        LoadShaderFromCode("SpriteSheet", spriteSheet_vs, spriteSheet_fs);
    }


    void ResourceManager::Render2DSprite(
        const std::string &textureKey,
        glm::vec2 position,
        glm::vec2 size,
        float rotation,
        glm::vec3 color,
        float alpha)
    {

        GameTexture2D *texture = GetTexture(textureKey);

        if (!texture)
        {
            return;
        }

        Shader *spriteShader = GetShader("Sprite");

        spriteShader->Use();
        spriteShader->SetModel("model", glm::vec3(position, 0.0f), glm::vec3(size, 0.0f), glm::vec3(0.0f, 0.0f, rotation));
        spriteShader->SetProjection("projection", ViewType::Ortho);
        spriteShader->SetVec3("spriteColor", color);

        unsigned int textureIndex = 0;
        texture->Bind(textureIndex);
        spriteShader->SetInt("image", textureIndex);

        glBindVertexArray(GetVAO("Sprite"));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        texture->Unbind();
        spriteShader->EndUse();
    }

    void ResourceManager::Render2DSprite(
        const std::string &vaoKey,
        const std::string &shaderKey,
        const std::string &textureKey,
        glm::vec2 position,
        glm::vec2 size,
        float rotation,
        glm::vec3 color,
        float alpha)
    {
        unsigned int vao = GetVAO(vaoKey);
        GameTexture2D *texture = GetTexture(textureKey);
        Shader *shader = GetShader(shaderKey);

        if ((vao == 0) || !texture || !shader)
        {
            return;
        }

        shader->Use();
        shader->SetModel("model", glm::vec3(position, 0.0f), glm::vec3(size, 0.0f), glm::vec3(0.0f, 0.0f, rotation));
        shader->SetProjection("projection", ViewType::Ortho);
        shader->SetVec3("spriteColor", color);

        unsigned int textureIndex = 0;
        texture->Bind(textureIndex);
        shader->SetInt("image", textureIndex);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        texture->Unbind();
        shader->EndUse();
    }

    void ResourceManager::Render2DSpriteFromTextureSheet(
        const std::string &textureKey,
        int row,
        int column,
        glm::vec2 position,
        glm::vec2 size,
        float rotation,
        glm::vec3 color,
        float alpha)
    {

        Texture_Sheet *textureSheet = GetTextureSheet(textureKey);

        if (!textureSheet)
        {
            return;
        }

        Shader *spriteSheetShader = GetShader("SpriteSheet");

        spriteSheetShader->Use();
        spriteSheetShader->SetModel("model", glm::vec3(position, 0.0f), glm::vec3(size, 0.0f), glm::vec3(0.0f, 0.0f, rotation));
        spriteSheetShader->SetProjection("projection", ViewType::Ortho);

        spriteSheetShader->SetVec3("spriteColor", color);


        int spriteWidth = ((textureSheet->texture->GetWidth() - (textureSheet->padding * textureSheet->columns)) / textureSheet->columns);
        int spriteHeight = ((textureSheet->texture->GetHeight() - (textureSheet->padding * textureSheet->rows)) / textureSheet->rows);

        int x = column * (spriteWidth + textureSheet->padding);
        int y = row * (spriteHeight + textureSheet->padding);

        float u_min = static_cast<float>(x) / textureSheet->texture->GetWidth();
        float v_min = static_cast<float>(y) / textureSheet->texture->GetHeight();
        float u_max = static_cast<float>(x + spriteWidth) / textureSheet->texture->GetWidth();
        float v_max = static_cast<float>(y + spriteHeight) / textureSheet->texture->GetHeight();

        spriteSheetShader->SetVec2("uvMin", glm::vec2(u_min, v_min));
        spriteSheetShader->SetVec2("uvMax", glm::vec2(u_max, v_max));

        unsigned int textureIndex = 0;
        textureSheet->texture->Bind(textureIndex);
        spriteSheetShader->SetInt("image", textureIndex);

        glBindVertexArray(GetVAO("Sprite"));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);

        spriteSheetShader->EndUse();
    }

    void ResourceManager::Render2DSpriteFromTextureSheet( // normal, specular, etc...
        const std::string &vaoKey,
        const std::string &shaderKey,
        const std::vector<std::pair<std::string, std::string>> &shaderVar_textureKey,
        int row,
        int column,
        glm::vec2 position,
        glm::vec2 size,
        float rotation,
        glm::vec3 color,
        float alpha)
    {
        unsigned int vao = GetVAO(vaoKey);
        Shader *shader = GetShader(shaderKey);

        if ((vao == 0) || !shader)
        {
            return;
        }

        shader->Use();
        shader->SetModel("model", glm::vec3(position, 0.0f), glm::vec3(size, 0.0f), glm::vec3(0.0f, 0.0f, rotation));
        shader->SetProjection("projection", ViewType::Ortho);

        shader->SetVec3("spriteColor", color);

        unsigned int textureIndex = 0;
        for (auto pair : shaderVar_textureKey)
        {
            Texture_Sheet *textureSheet = GetTextureSheet(pair.second);

            if (!textureSheet)
            {
                return;
            }

            int spriteWidth = ((textureSheet->texture->GetWidth() - (textureSheet->padding * textureSheet->columns)) / textureSheet->columns);
            int spriteHeight = ((textureSheet->texture->GetHeight() - (textureSheet->padding * textureSheet->rows)) / textureSheet->rows);

            int x = column * (spriteWidth + textureSheet->padding);
            int y = row * (spriteHeight + textureSheet->padding);

            float u_min = static_cast<float>(x) / textureSheet->texture->GetWidth();
            float v_min = static_cast<float>(y) / textureSheet->texture->GetHeight();
            float u_max = static_cast<float>(x + spriteWidth) / textureSheet->texture->GetWidth();
            float v_max = static_cast<float>(y + spriteHeight) / textureSheet->texture->GetHeight();

            shader->SetVec2("uvMin", glm::vec2(u_min, v_min));
            shader->SetVec2("uvMax", glm::vec2(u_max, v_max));

            textureSheet->texture->Bind(textureIndex);
            shader->SetInt(pair.first, textureIndex);
            textureIndex++;
        }

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);

        shader->EndUse();
    }

    void ResourceManager::InitShapeResources(ShapeResources &shape, const std::vector<glm::vec3> &vertices)
    {
        if (!shape.initialized)
        {
            glGenVertexArrays(1, &shape.VAO);
            glGenBuffers(1, &shape.VBO);

            glBindVertexArray(shape.VAO);
            glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW); // Reservar espacio

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            shape.initialized = true;
            shape.vertexCount = vertices.size();
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    void ResourceManager::RenderShape(
        ShapeResources &shape,
        const std::vector<glm::vec3> &vertices,
        glm::vec3 pivot,
        glm::vec3 rotation,
        glm::vec3 color,
        float alpha,
        float lineWidth,
        GLenum drawMode)
    {
        InitShapeResources(shape, vertices);

        Shader *shapeShader = GetShader("Shape");
        shapeShader->Use();

        glm::mat4 model(1.0f);
        model = glm::translate(model, pivot);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, -pivot);

        shapeShader->SetMatrix4("model", model);
        shapeShader->SetProjection("projection", ViewType::Ortho);
        shapeShader->SetVec4("LineColor", glm::vec4(color, alpha));

        glLineWidth(lineWidth);

        glBindVertexArray(shape.VAO);
        glDrawArrays(drawMode, 0, shape.vertexCount);
        shapeShader->EndUse();
    }

    void ResourceManager::RenderLine(glm::vec3 point_1, glm::vec3 point_2, glm::vec3 pivot, glm::vec3 rotation, glm::vec3 color, float alpha, float lineWidth)
    {
        RenderShape(lineResources, {point_1, point_2}, pivot, rotation, color, alpha, lineWidth, GL_LINES);
    }

    void ResourceManager::RenderTriangle(glm::vec3 point_1, glm::vec3 point_2, glm::vec3 point_3, glm::vec3 pivot, glm::vec3 rotation, glm::vec3 color, float alpha, float lineWidth, bool filled)
    {
        RenderShape(triangleResources, {point_1, point_2, point_3}, pivot, rotation, color, alpha, lineWidth, GL_LINE_LOOP);
    }

    void ResourceManager::RenderRectangle(glm::vec3 point_1, glm::vec3 point_2, glm::vec3 pivot, glm::vec3 rotation, glm::vec3 color, float alpha, float lineWidth, bool filled)
    {
        RenderShape(rectangleResources, {point_1, glm::vec3(point_2.x, point_1.y, 0.0f), point_2, glm::vec3(point_1.x, point_2.y, 0.0f)}, pivot, rotation, color, alpha, lineWidth, GL_LINE_LOOP);
    }

    bool ResourceManager::LoadVAO(const std::string &key, const std::vector<std::vector<float>> &vertices)
    {
        if (vao_resources.find(key) != vao_resources.end())
        {
            return false;
        }

        if (vertices.empty())
        {
            std::cerr << "Error: Empty VAO data" << std::endl;
            return false;
        }

        std::vector<float> flatVertices;
        for (const auto &row : vertices)
        {
            flatVertices.insert(flatVertices.end(), row.begin(), row.end());
        }

        unsigned int VBO, VAO;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, flatVertices.size() * sizeof(float), flatVertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, vertices[0].size(), GL_FLOAT, GL_FALSE, vertices[0].size() * sizeof(float), (void *)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glDeleteBuffers(1, &VBO);

        vao_resources[key] = VAO;

        return true;
    }

    bool ResourceManager::LoadShaderFromPath(const std::string &key, const std::string &vs_path, const std::string &fs_path, const std::string &gs_path)
    {
        if (shader_resources.find(key) != shader_resources.end())
        {
            return false;
        }

        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        try
        {
            std::ifstream vertexShaderFile(vs_path);
            std::ifstream fragmentShaderFile(fs_path);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vertexShaderFile.rdbuf();
            fShaderStream << fragmentShaderFile.rdbuf();

            vertexShaderFile.close();
            fragmentShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();

            if (!gs_path.empty())
            {
                std::ifstream geometryShaderFile(gs_path);
                std::stringstream gShaderStream;
                gShaderStream << geometryShaderFile.rdbuf();
                geometryShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::exception e)
        {
            std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
        }

        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        const char *gShaderCode = geometryCode.c_str();

        Shader *shader = new Shader(vShaderCode, fShaderCode, !geometryCode.empty() ? gShaderCode : nullptr);

        shader_resources[key] = shader;

        return true;
    }

    bool ResourceManager::LoadShaderFromCode(const std::string &key, const char* vertexSource, const char* fragmentSource, const char* geometrySource)
    {
        if (shader_resources.find(key) != shader_resources.end())
        {
            return false;
        }

        Shader *shader = new Shader(vertexSource, fragmentSource, geometrySource);

        shader_resources[key] = shader;

        return true;
    }

    bool ResourceManager::LoadTexture(const std::string &key, const std::string &path, bool alpha)
    {
        if (texture_resources.find(key) != texture_resources.end())
        {
            return false;
        }

        GameTexture2D *texture = new GameTexture2D(path.c_str(), alpha ? GL_RGBA : GL_RGB, alpha ? GL_RGBA : GL_RGB);

        texture_resources[key] = texture;
        return true;
    }

    bool ResourceManager::LoadTextureSheet(const std::string &key, const std::string &path, bool alpha, int rows, int columns, int padding)
    {
        if (textures_sheet_resources.find(key) != textures_sheet_resources.end())
        {
            return false;
        }

        GameTexture2D *texture = new GameTexture2D(path.c_str(), alpha ? GL_RGBA : GL_RGB, alpha ? GL_RGBA : GL_RGB);

        Texture_Sheet *textures_sheet = new Texture_Sheet(texture, rows, columns, padding);

        textures_sheet_resources[key] = textures_sheet;
        return true;
    }

    bool ResourceManager::LoadFont(const std::string &key, const std::string &path, int size)
    {
        if (font_resources.find(key) != font_resources.end())
        {
            return false;
        }

        return true;
    }

    unsigned int ResourceManager::GetVAO(const std::string &key) const
    {
        auto it = vao_resources.find(key);
        if (it == vao_resources.end())
        {
            return 0;
        }
        return it->second;
    }

    Shader *ResourceManager::GetShader(const std::string &key) const
    {
        auto it = shader_resources.find(key);
        if (it == shader_resources.end())
        {
            return nullptr;
        }
        return it->second;
    }

    GameTexture2D *ResourceManager::GetTexture(const std::string &key) const
    {
        auto it = texture_resources.find(key);
        if (it == texture_resources.end())
        {
            return nullptr;
        }
        return it->second;
    }

    ResourceManager::Texture_Sheet *ResourceManager::GetTextureSheet(const std::string &key) const
    {
        auto it = textures_sheet_resources.find(key);
        if (it == textures_sheet_resources.end())
        {
            return nullptr;
        }
        return it->second;
    }

    int *ResourceManager::GetFont(const std::string &key) const
    {
        auto it = font_resources.find(key);
        if (it == font_resources.end())
        {
            return nullptr;
        }
        return it->second;
    }

    void ResourceManager::Clear()
    {
        if (lineResources.initialized)
        {
            glDeleteVertexArrays(1, &lineResources.VAO);
            glDeleteBuffers(1, &lineResources.VBO);
            lineResources.initialized = false;
        }

        if (triangleResources.initialized)
        {
            glDeleteVertexArrays(1, &triangleResources.VAO);
            glDeleteBuffers(1, &triangleResources.VBO);
            triangleResources.initialized = false;
        }

        if (rectangleResources.initialized)
        {
            glDeleteVertexArrays(1, &rectangleResources.VAO);
            glDeleteBuffers(1, &rectangleResources.VBO);
            rectangleResources.initialized = false;
        }



        auto it_vao_r = vao_resources.begin();
        while(it_vao_r != vao_resources.end())
        {
            if((*it_vao_r).second != 0)
            {
                glDeleteVertexArrays(1, &(*it_vao_r).second);
                (*it_vao_r).second = 0;
                //glDeleteBuffers(1, ------); DELETE VBO

                it_vao_r++;
            }
        }
        vao_resources.clear();


        auto it_shader_r = shader_resources.begin();
        while(it_shader_r != shader_resources.end())
        {
            if(
                (*it_shader_r).second != nullptr
                //&& 
                //!( (*it_shader_r).first == "Shape" || (*it_shader_r).first == "Sprite" || (*it_shader_r).first == "SpriteSheet" ) // Change to INIT RESOURCES
                )
            {
                delete (*it_shader_r).second;
                (*it_shader_r).second = nullptr;
                it_shader_r++;
            }
        }
        shader_resources.clear();


        auto it_texture_r = texture_resources.begin();
        while(it_texture_r != texture_resources.end())
        {
            if((*it_texture_r).second != nullptr)
            {
                delete (*it_texture_r).second;
                (*it_texture_r).second = nullptr;
                it_texture_r++;
            }
        }
        texture_resources.clear();


        auto it_textureSheet_r = textures_sheet_resources.begin();
        while(it_textureSheet_r != textures_sheet_resources.end())
        {
            if((*it_textureSheet_r).second != nullptr)
            {
                if((*it_textureSheet_r).second->texture != nullptr)
                {
                    delete (*it_textureSheet_r).second->texture;
                    (*it_textureSheet_r).second->texture = nullptr;
                }
                delete (*it_textureSheet_r).second;
                (*it_textureSheet_r).second = nullptr;
                it_textureSheet_r++;
            }
        }
        textures_sheet_resources.clear();


        for (auto &pair : font_resources)
        {

        }
        font_resources.clear();


        #ifndef NDEBUG
                std::cout << "Resource manager cleared" << std::endl;
        #endif
    }

    ResourceManager::~ResourceManager()
    {
        #ifndef NDEBUG
                std::cout << "Resource manager destroyed" << std::endl;
        #endif
    }
}
