#include "ResourceManager.hpp"
#include "MasterShaders.hpp"

#include "../Camera/CameraManager.hpp"


#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>

#include <glm/gtc/matrix_transform.hpp>

namespace WandEngine
{

    ResourceManager &ResourceManager::GetInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    ResourceManager::ResourceManager()
    {
        std::cout << "Resource manager created" << std::endl;
    }

    ResourceManager::~ResourceManager()
    {
        std::cout << "Resource manager destroyed" << std::endl;
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

        Load_Static_VAO("WAND_Sprite", vertices);
        Load_Dynamic_VAO_VBO("WAND_Point", 1);
        Load_Dynamic_VAO_VBO("WAND_Line", 2);
        Load_Dynamic_VAO_VBO("WAND_Triangle", 3);
        Load_Dynamic_VAO_VBO("WAND_Rectangle", 4);

        LoadShaderFromCode("WAND_Shape", shape_vs, shape_fs);
        LoadShaderFromCode("WAND_Sprite", sprite_vs, sprite_fs);
        LoadShaderFromCode("WAND_SpriteSheet", spriteSheet_vs, spriteSheet_fs);
        LoadShaderFromCode("WAND_Text", text_vs, text_fs);

        std::cout << "Resource manager initialized" << std::endl;
    }


    void ResourceManager::Render2DSprite(
        const std::string &textureKey,
        glm::vec2 position,
        glm::vec2 size,
        float rotation,
        glm::vec3 color,
        float alpha,
        ViewType viewType
    )
    {

        GameTexture2D *texture = GetTexture(textureKey);

        if (!texture)
        {
            return;
        }

        Shader *spriteShader = GetShader("WAND_Sprite");

        spriteShader->Use();
        spriteShader->SetModel("model", glm::vec3(position, 0.0f), glm::vec3(size, 0.0f), glm::vec3(0.0f, 0.0f, rotation));
        spriteShader->SetProjection("projection", viewType);
        spriteShader->SetVec3("spriteColor", color);

        unsigned int textureIndex = 0;
        texture->Bind(textureIndex);
        spriteShader->SetInt("image", textureIndex);

        glBindVertexArray(Get_Static_VAO("WAND_Sprite"));
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
        float alpha,
        ViewType viewType
    )
    {
        unsigned int vao = Get_Static_VAO(vaoKey);
        GameTexture2D *texture = GetTexture(textureKey);
        Shader *shader = GetShader(shaderKey);

        if ((vao == 0) || !texture || !shader)
        {
            return;
        }

        shader->Use();
        shader->SetModel("model", glm::vec3(position, 0.0f), glm::vec3(size, 0.0f), glm::vec3(0.0f, 0.0f, rotation));
        shader->SetProjection("projection", viewType);
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
        float alpha,
        ViewType viewType
    )
    {

        Texture_Sheet *textureSheet = GetTextureSheet(textureKey);

        if (!textureSheet)
        {
            return;
        }

        Shader *spriteSheetShader = GetShader("WAND_SpriteSheet");

        spriteSheetShader->Use();
        spriteSheetShader->SetModel("model", glm::vec3(position, 0.0f), glm::vec3(size, 0.0f), glm::vec3(0.0f, 0.0f, rotation));
        spriteSheetShader->SetProjection("projection", viewType);

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

        glBindVertexArray(Get_Static_VAO("WAND_Sprite"));
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
        float alpha,
        ViewType viewType
    )
    {
        unsigned int vao = Get_Static_VAO(vaoKey);
        Shader *shader = GetShader(shaderKey);

        if ((vao == 0) || !shader)
        {
            return;
        }

        shader->Use();
        shader->SetModel("model", glm::vec3(position, 0.0f), glm::vec3(size, 0.0f), glm::vec3(0.0f, 0.0f, rotation));
        shader->SetProjection("projection", viewType);

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


    void ResourceManager::RenderShape(
        const std::string &key,
        const std::vector<glm::vec3> &vertices,
        glm::vec3 pivot,
        glm::vec3 rotation,
        glm::vec3 color,
        float alpha,
        float lineWidth,
        GLenum drawMode,
        ViewType viewType
    )
    {
        auto dynamic_temp = Get_Dynamic_VAO_VBO(key);

        glBindBuffer(GL_ARRAY_BUFFER, dynamic_temp.second);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        Shader *shapeShader = GetShader("WAND_Shape");
        shapeShader->Use();

        glm::mat4 model(1.0f);
        model = glm::translate(model, pivot);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, -pivot);

        shapeShader->SetMatrix4("model", model);
        
        shapeShader->SetMatrix4("view", CameraManager::GetInstance().GetViewMatrix());  // TODO OPTIMIZE FOR 3D AND FIX

        shapeShader->SetProjection("projection", viewType);
        shapeShader->SetVec4("LineColor", glm::vec4(color, alpha));


        glLineWidth(lineWidth);

        glBindVertexArray(dynamic_temp.first);
        glDrawArrays(drawMode, 0, vertices.size());
        shapeShader->EndUse();
    }

    void ResourceManager::RenderPoint(glm::vec3 coordinates, glm::vec3 color, float alpha, float width, ViewType viewType)
    {
        glPointSize(width);
        RenderShape("WAND_Point", {coordinates}, coordinates, glm::vec3(0.0f), color, alpha, width, GL_POINTS, viewType);
        glPointSize(1.0f);
    }

    void ResourceManager::RenderLine(glm::vec3 point_1, glm::vec3 point_2, glm::vec3 pivot, glm::vec3 rotation, glm::vec3 color, float alpha, float lineWidth, ViewType viewType)
    {
        RenderShape("WAND_Line", {point_1, point_2}, pivot, rotation, color, alpha, lineWidth, GL_LINES, viewType);
    }

    void ResourceManager::RenderTriangle(glm::vec3 point_1, glm::vec3 point_2, glm::vec3 point_3, glm::vec3 pivot, glm::vec3 rotation, glm::vec3 color, float alpha, float lineWidth, bool filled, ViewType viewType)
    {
        RenderShape("WAND_Triangle", {point_1, point_2, point_3}, pivot, rotation, color, alpha, lineWidth, GL_LINE_LOOP, viewType);
    }

    void ResourceManager::RenderRectangle(glm::vec3 point_1, glm::vec3 point_2, glm::vec3 pivot, glm::vec3 rotation, glm::vec3 color, float alpha, float lineWidth, bool filled, ViewType viewType)
    {
        RenderShape("WAND_Rectangle", {point_1, glm::vec3(point_2.x, point_1.y, 0.0f), point_2, glm::vec3(point_1.x, point_2.y, 0.0f)}, pivot, rotation, color, alpha, lineWidth, GL_LINE_LOOP, viewType);
    }

    glm::vec2 ResourceManager::MeasureText(
        const std::string& text,
        const std::string& fontKey,
        glm::vec3 scale
    ) {
        TextFont* textFont = GetTextFont(fontKey);
        if (!textFont) return glm::vec2(0.0f);
    
        float width = 0.0f;
        float maxHeight = 0.0f;
    
        for (char c : text) {
            const TextCharacter& ch = textFont->GetCharacters().at(c);
            width += (ch.Advance >> 6) * scale.x;
            float height = ch.Size.y * scale.y;
            if (height > maxHeight)
                maxHeight = height;
        }
    
        return glm::vec2(width, maxHeight);
    }

    void ResourceManager::RenderText(
        const std::string &text,
        const std::string &fontKey,
        glm::vec3 position,
        glm::vec3 scale,
        glm::vec3 pivot,
        glm::vec3 rotation,
        glm::vec3 color,
        float alpha,
        ViewType viewType
    )
    {
        TextFont * textFont = GetTextFont(fontKey);
        Shader* textShader = GetShader("WAND_Text");

        if (!(textFont && textShader)) return;

        textShader->Use();
        textShader->SetProjection("projection", viewType);
        textShader->SetInt("WAND_text", 0);
        textShader->SetVec3("textColor", color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(textFont->GetVAO());

        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            TextCharacter ch = textFont->GetCharacters()[*c];
    
            float xpos = position.x + ch.Bearing.x * scale.x;
            float ypos = position.y + (textFont->GetCharacters()['H'].Bearing.y - ch.Bearing.y) * scale.y;
    
            float w = ch.Size.x * scale.x;
            float h = ch.Size.y * scale.y;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 0.0f },
                { xpos,     ypos,       0.0f, 0.0f },
    
                { xpos,     ypos + h,   0.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, textFont->GetVBO());
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph
            position.x += (ch.Advance >> 6) * scale.x; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    bool ResourceManager::Load_Static_VAO(const std::string &key, const std::vector<std::vector<float>> &vertices)
    {
        if (static_vao_resources.find(key) != static_vao_resources.end())
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

        static_vao_resources[key] = VAO;

        return true;
    }


    bool ResourceManager::Load_Dynamic_VAO_VBO(const std::string &key, size_t vertexCount)
    {
        if (static_vao_resources.find(key) != static_vao_resources.end())
        {
            return false;
        }

        if (vertexCount <= 0)
        {
            std::cerr << "Vertex must be greater than 0" << std::endl;
            return false;
        }


        std::pair<unsigned int, unsigned int> tmp_dynamic;

        glGenVertexArrays(1, &tmp_dynamic.first);
        glGenBuffers(1, &tmp_dynamic.second);

        glBindVertexArray(tmp_dynamic.first);
        glBindBuffer(GL_ARRAY_BUFFER, tmp_dynamic.second);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        dynamic_vao_vbo_resources[key] = tmp_dynamic;

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

    bool ResourceManager::LoadTextFont(const std::string &key, const std::string &path, unsigned int fontSize)
    {
        if (text_font_resources.find(key) != text_font_resources.end())
        {
            return false;
        }

        TextFont *text = new TextFont(path, fontSize);

        text_font_resources[key] = text;

        return true;
    }

    unsigned int ResourceManager::Get_Static_VAO(const std::string &key) const
    {
        auto it = static_vao_resources.find(key);
        if (it == static_vao_resources.end())
        {
            return 0;
        }
        return it->second;
    }

    std::pair<unsigned int, unsigned int> ResourceManager::Get_Dynamic_VAO_VBO(const std::string &key) const
    {
        auto it = dynamic_vao_vbo_resources.find(key);
        if (it == dynamic_vao_vbo_resources.end())
        {
            return std::pair(0,0);
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

    TextFont *ResourceManager::GetTextFont(const std::string &key) const
    {
        auto it = text_font_resources.find(key);
        if (it == text_font_resources.end())
        {
            return nullptr;
        }
        return it->second;
    }

    void ResourceManager::Clear()
    {

        ClearMap<unsigned int>(static_vao_resources, [](unsigned int& resource){
            if(resource != 0) glDeleteVertexArrays(1, &resource);
            resource = 0;
        });

        ClearMap<std::pair<unsigned int, unsigned int>>(dynamic_vao_vbo_resources, [](std::pair<unsigned int, unsigned int>& resource){
            if(resource.first != 0) glDeleteVertexArrays(1, &resource.first);
            resource.first = 0;
            if(resource.second != 0) glDeleteBuffers(1, &resource.second);
            resource.second = 0;            
        });


        ClearMap<Shader*>(shader_resources, [](Shader*& resource){
            if(resource) delete resource;
            resource = nullptr;
        });

        ClearMap<GameTexture2D*>(texture_resources, [](GameTexture2D*& resource){
            if(resource) delete resource;
            resource = nullptr;
        });

        
        ClearMap<Texture_Sheet*>(textures_sheet_resources, [](Texture_Sheet*& resource){
            if(resource)
            {
                if(resource->texture) delete resource->texture;
                resource->texture = nullptr;
                delete resource;
            }
            resource = nullptr;
        });


        ClearMap<TextFont*>(text_font_resources, [](TextFont*& resource){
            if(resource) delete resource;
            resource = nullptr;
        });
        ClearMap<TextFont*>(text_font_resources, [](TextFont*& resource){
            if(resource) delete resource;
            resource = nullptr;
        });


        std::cout << "Resource manager cleared" << std::endl;
    }


}
