/**
 * @file resource_manager.cpp
 * @brief Implements loading, ownership, and rendering helpers for engine resources.
 */

#include "resource_manager.hpp"
#include "master_shaders.hpp"

#include "../camera/camera_manager.hpp"
#include "../light/light_manager.hpp"
#include "../../models/light/light.hpp"
#include "../../utils/log.hpp"


#include <sstream>
#include <fstream>
#include <functional>
#include <memory>

#include <glm/gtc/matrix_transform.hpp>

namespace CosmicEngine
{

    ResourceManager &ResourceManager::GetInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    ResourceManager::ResourceManager()
    {
        RUNTIME_LIFECYCLE("Resource manager", "created");
    }

    ResourceManager::~ResourceManager()
    {
        RUNTIME_LIFECYCLE("Resource manager", "destroyed");
    }


    
    bool ResourceManager::Load_Static_VAO(const std::string &key, const std::vector<std::vector<float>> &vertices, const std::vector<int> &attributeSizes)
    {
		// Static VAOs flatten row-oriented authoring data into a single GPU buffer while preserving attribute layout.
        if (static_vao_resources.find(key) != static_vao_resources.end())
            return false;
    
        if (vertices.empty() || attributeSizes.empty())
        {
            RUNTIME_WARNING("[ResourceManager] Empty VAO data or attribute sizes.");
            return false;
        }
    
        size_t vertexStride = 0;
        for (int size : attributeSizes)
            vertexStride += size;
    

        for(auto current_size : vertices)
        {
            if (current_size.size() != vertexStride)
            {
                RUNTIME_WARNING("[ResourceManager] VAO data size and attribute sizes do not match.");
                return false;
            }
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
    
        size_t offset = 0;
        for (GLuint i = 0; i < attributeSizes.size(); ++i)
        {
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(
                i,
                attributeSizes[i],
                GL_FLOAT,
                GL_FALSE,
                vertexStride * sizeof(float),
                (void*)(offset * sizeof(float))
            );
            offset += attributeSizes[i];
        }
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glDeleteBuffers(1, &VBO);
    
        static_vao_resources[key] = VAO;
    
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
            if (!vertexShaderFile.is_open() || !fragmentShaderFile.is_open())
            {
                RUNTIME_WARNING("[ResourceManager] Failed to open shader files: " << vs_path << " or " << fs_path);
                return false;
            }
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
                if (!geometryShaderFile.is_open())
                {
                    RUNTIME_WARNING("[ResourceManager] Failed to open geometry shader file: " << gs_path);
                    return false;
                }
                std::stringstream gShaderStream;
                gShaderStream << geometryShaderFile.rdbuf();
                geometryShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (const std::exception &e)
        {
            RUNTIME_WARNING("[ResourceManager] Failed to read shader files: " << e.what());
            return false;
        }

        try
		{
			const char *vShaderCode = vertexCode.c_str();
			const char *fShaderCode = fragmentCode.c_str();
			const char *gShaderCode = geometryCode.c_str();
			std::unique_ptr<Shader> shader = std::make_unique<Shader>(vShaderCode, fShaderCode, !geometryCode.empty() ? gShaderCode : nullptr);
			shader_resources[key] = shader.release();
			return true;
		}
		catch (const std::exception &e)
		{
			RUNTIME_WARNING("[ResourceManager] Failed to create shader '" << key << "': " << e.what());
			return false;
		}
    }

    bool ResourceManager::LoadShaderFromCode(const std::string &key, const char* vertexSource, const char* fragmentSource, const char* geometrySource)
    {
        if (shader_resources.find(key) != shader_resources.end())
        {
            return false;
        }

        try
        {
            std::unique_ptr<Shader> shader = std::make_unique<Shader>(vertexSource, fragmentSource, geometrySource);
            shader_resources[key] = shader.release();
            return true;
        }
        catch (const std::exception &e)
        {
            RUNTIME_WARNING("[ResourceManager] Failed to create shader '" << key << "': " << e.what());
            return false;
        }
    }

    bool ResourceManager::RegisterShaderForLighting(const std::string &key)
    {
        Shader *shader = GetShader(key);
        if (!shader)
        {
            return false;
        }

        LightManager::GetInstance().RegisterShader(shader);
        return true;
    }

    bool ResourceManager::LoadTexture(const std::string &key, const std::string &path)
    {
        if (texture_resources.find(key) != texture_resources.end())
        {
            RUNTIME_WARNING("[ResourceManager] Texture already exists: " << key);
            return false;
        }

        Texture2D *texture = new Texture2D(path.c_str());

        if (texture->GetWidth() == 0 || texture->GetHeight() == 0)
        {
            RUNTIME_WARNING("[ResourceManager] Failed to load texture: " << key << " from path: " << path);
            delete texture;
            return false;
        }

        texture_resources[key] = texture;
        RUNTIME_INFO("[ResourceManager] Loaded texture: " << key << " (" << texture->GetWidth() << "x" << texture->GetHeight() << ")");
        return true;
    }

    bool ResourceManager::LoadTextureSheet(const std::string &key, const std::string &path, int rows, int columns, int padding)
    {
        if (textures_sheet_resources.find(key) != textures_sheet_resources.end())
        {
            return false;
        }

        std::unique_ptr<Texture2D> texture = std::make_unique<Texture2D>(path.c_str());

        if (texture->GetWidth() == 0 || texture->GetHeight() == 0)
        {
            RUNTIME_WARNING("[ResourceManager] Failed to load texture sheet: " << key << " from path: " << path);
            return false;
        }

        textures_sheet_resources[key] = new Texture_Sheet(texture.release(), rows, columns, padding);
        return true;
    }

    bool ResourceManager::LoadFont(const std::string &key, const std::string &path, unsigned int fontSize)
    {
        if (text_font_resources.find(key) != text_font_resources.end())
        {
            return false;
        }

        try
        {
            std::unique_ptr<Font> text = std::make_unique<Font>(path, fontSize);
            text_font_resources[key] = text.release();
            return true;
        }
        catch (const std::exception &e)
        {
            RUNTIME_WARNING("[ResourceManager] Failed to load font '" << key << "': " << e.what());
            return false;
        }
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

    Texture2D *ResourceManager::GetTexture(const std::string &key) const
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

    Font *ResourceManager::GetFont(const std::string &key) const
    {
        auto it = text_font_resources.find(key);
        if (it == text_font_resources.end())
        {
            return nullptr;
        }
        return it->second;
    }

    Model *ResourceManager::GetModel(const std::string &key) const
    {
        auto it = model_resources.find(key);
        if (it == model_resources.end())
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

        ClearMap<Texture2D*>(texture_resources, [](Texture2D*& resource){
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


        ClearMap<Font*>(text_font_resources, [](Font*& resource){
            if(resource) delete resource;
            resource = nullptr;
        });

        ClearMap<Model*>(model_resources, [](Model*& resource){
            if(resource) delete resource;
            resource = nullptr;
        });


        RUNTIME_LIFECYCLE("Resource manager", "cleared");
    }



    bool ResourceManager::init()
    {
        Clear();

        auto require = [](bool result, const std::string &message)
        {
            if (!result)
            {
                RUNTIME_WARNING(message);
            }
            return result;
        };

		// Register engine-owned built-in geometry and shaders before scenes start requesting resources.
        //DefaultVAOTexture
        std::vector<std::vector<float>> vertices = { 
            {0.0f, 1.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f}, 
            {0.0f, 1.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 1.0f, 0.0f}
        };

        

        //Load_Static_VAO("COSMIC_Sprite", vertices, {2, 2});  ??
        
        if (!require(Load_Static_VAO("COSMIC_Sprite", vertices, {4}), "[ResourceManager] Failed to create built-in VAO COSMIC_Sprite."))
            return false;

        if (!require(Load_Dynamic_VAO_VBO("COSMIC_Point", 1), "[ResourceManager] Failed to create built-in VAO/VBO COSMIC_Point."))
            return false;
        if (!require(Load_Dynamic_VAO_VBO("COSMIC_Line", 2), "[ResourceManager] Failed to create built-in VAO/VBO COSMIC_Line."))
            return false;
        if (!require(Load_Dynamic_VAO_VBO("COSMIC_Triangle", 3), "[ResourceManager] Failed to create built-in VAO/VBO COSMIC_Triangle."))
            return false;
        if (!require(Load_Dynamic_VAO_VBO("COSMIC_Rectangle", 4), "[ResourceManager] Failed to create built-in VAO/VBO COSMIC_Rectangle."))
            return false;

        if (!require(LoadShaderFromCode("COSMIC_Shape_2D", shape_2D_vs, shape_2D_fs), "[ResourceManager] Failed to create built-in shader COSMIC_Shape_2D."))
            return false;
        if (!require(LoadShaderFromCode("COSMIC_SpriteSheet_Unlit", spriteSheet_unlit_vs, spriteSheet_unlit_fs), "[ResourceManager] Failed to create built-in shader COSMIC_SpriteSheet_Unlit."))
            return false;
        if (!require(LoadShaderFromCode("COSMIC_Text", text_vs, text_fs), "[ResourceManager] Failed to create built-in shader COSMIC_Text."))
            return false;

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        if (!require(LoadShaderFromCode("COSMIC_SpriteSheet_Lit", spriteSheet_lit_vs, spriteSheet_lit_fs), "[ResourceManager] Failed to create built-in shader COSMIC_SpriteSheet_Lit."))
            return false;
        if (!require(RegisterShaderForLighting("COSMIC_SpriteSheet_Lit"), "[ResourceManager] Failed to register built-in shader COSMIC_SpriteSheet_Lit for lighting."))
            return false;
        
        
        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

        const float atlasU0 = 0.0f;
        const float atlasU1 = 0.25f;
        const float atlasU2 = 0.50f;
        const float atlasU3 = 0.75f;
        const float atlasU4 = 1.00f;
        const float atlasV0 = 0.0f;
        const float atlasV1 = 1.0f / 3.0f;
        const float atlasV2 = 2.0f / 3.0f;
        const float atlasV3 = 1.0f;

        std::vector<std::vector<float>> Parallelepiped_texture_normal_vertices = {
                {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f},
                {0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f},
                {0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f},
                {0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f},
                {-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f},
                {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f},
        
                {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f},
                {0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f},
                {0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f},
                {0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f},
                {-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f},
                {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f},
        
                {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
                {-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
                {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
                {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
                {-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
                {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
        
                {0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
                {0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
                {0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
                {0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
                {0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
                {0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
        
                {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f},
                {0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f},
                {0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f},
                {0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f},
                {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f},
                {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f},
        
                {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f},
                {0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f},
                {0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f},
                {0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f},
                {-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f},
                {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f}
            };

            std::vector<std::vector<float>> Parallelepiped_cube_atlas_vertices = {
                {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  atlasU4, atlasV1},
                { 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  atlasU3, atlasV1},
                { 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  atlasU3, atlasV2},
                { 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  atlasU3, atlasV2},
                {-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  atlasU4, atlasV2},
                {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  atlasU4, atlasV1},

                {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  atlasU1, atlasV1},
                { 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  atlasU2, atlasV1},
                { 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  atlasU2, atlasV2},
                { 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  atlasU2, atlasV2},
                {-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  atlasU1, atlasV2},
                {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  atlasU1, atlasV1},

                {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  atlasU1, atlasV1},
                {-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  atlasU1, atlasV2},
                {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  atlasU0, atlasV2},
                {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  atlasU0, atlasV2},
                {-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  atlasU0, atlasV1},
                {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  atlasU1, atlasV1},

                { 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  atlasU2, atlasV1},
                { 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  atlasU2, atlasV2},
                { 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  atlasU3, atlasV2},
                { 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  atlasU3, atlasV2},
                { 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  atlasU3, atlasV1},
                { 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  atlasU2, atlasV1},

                {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  atlasU1, atlasV1},
                { 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  atlasU2, atlasV1},
                { 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  atlasU2, atlasV0},
                { 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  atlasU2, atlasV0},
                {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  atlasU1, atlasV0},
                {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  atlasU1, atlasV1},

                {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  atlasU1, atlasV3},
                { 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  atlasU2, atlasV3},
                { 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  atlasU2, atlasV2},
                { 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  atlasU2, atlasV2},
                {-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  atlasU1, atlasV2},
                {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  atlasU1, atlasV3}
            };
        
            std::vector<std::vector<float>> ParallelepipedLinesVertices = {
                { -0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f, -0.5f },
                {  0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f,  0.5f },
                {  0.5f, -0.5f,  0.5f }, { -0.5f, -0.5f,  0.5f },
                { -0.5f, -0.5f,  0.5f }, { -0.5f, -0.5f, -0.5f },
            
                { -0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f, -0.5f },
                {  0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f,  0.5f },
                {  0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f },
                { -0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f, -0.5f },
            
                { -0.5f, -0.5f, -0.5f }, { -0.5f,  0.5f, -0.5f },
                {  0.5f, -0.5f, -0.5f }, {  0.5f,  0.5f, -0.5f },
                {  0.5f, -0.5f,  0.5f }, {  0.5f,  0.5f,  0.5f },
                { -0.5f, -0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f }
            };
        

            if (!require(LoadShaderFromCode("COSMIC_Shape_3D", shape_3D_vs, shape_3D_fs), "[ResourceManager] Failed to create built-in shader COSMIC_Shape_3D."))
                return false;
            if (!require(Load_Static_VAO("COSMIC_Parallelepiped_Lines", ParallelepipedLinesVertices, {3}), "[ResourceManager] Failed to create built-in VAO COSMIC_Parallelepiped_Lines."))
                return false;
            if (!require(Load_Static_VAO("COSMIC_Parallelepiped", Parallelepiped_texture_normal_vertices, {3,3,2}), "[ResourceManager] Failed to create built-in VAO COSMIC_Parallelepiped."))
                return false;
            if (!require(Load_Static_VAO("COSMIC_Parallelepiped_CubeAtlas", Parallelepiped_cube_atlas_vertices, {3,3,2}), "[ResourceManager] Failed to create built-in VAO COSMIC_Parallelepiped_CubeAtlas."))
                return false;
            if (!require(LoadShaderFromCode("COSMIC_3DModel_Lit", model_3d_lit_vs, model_3d_lit_fs), "[ResourceManager] Failed to create built-in shader COSMIC_3DModel_Lit."))
                return false;
            if (!require(LoadShaderFromCode("COSMIC_3DModel_Unlit", model_3d_lit_vs, model_3d_unlit_fs), "[ResourceManager] Failed to create built-in shader COSMIC_3DModel_Unlit."))
                return false;

            if (Shader *modelShader = GetShader("COSMIC_3DModel_Lit"))
            {
                LightManager::GetInstance().RegisterShader(modelShader); // MOVE TO ...?
            }
            else
            {
                RUNTIME_WARNING("[ResourceManager] Built-in shader COSMIC_3DModel_Lit is missing after initialization.");
                return false;
            }

        #else
            #error "[ObjectManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
    
        RUNTIME_LIFECYCLE("Resource manager", "initialized");
		return true;
    }
    void ResourceManager::Render2DSpriteUnlit(
        const std::string &textureKey,
        glm::vec2 position,
        glm::vec2 size,
        float rotation,
        glm::vec3 color,
        float alpha,
        ViewType viewType
    )
    {

        Texture2D *texture = GetTexture(textureKey);

        if (!texture)
        {
            RUNTIME_WARNING("[ResourceManager] Texture not found: " << textureKey);
            return;
        }

        if (texture->GetWidth() == 0 || texture->GetHeight() == 0)
        {
            RUNTIME_WARNING("[ResourceManager] Texture has invalid dimensions: " << textureKey
                      << " (" << texture->GetWidth() << "x" << texture->GetHeight() << ")");
            return;
        }

        Shader *spriteShader = GetShader("COSMIC_SpriteSheet_Unlit");

        if (!spriteShader)
        {
            RUNTIME_WARNING("[ResourceManager] Shader not found: COSMIC_SpriteSheet_Unlit");
            return;
        }

        unsigned int vao = Get_Static_VAO("COSMIC_Sprite");
        if (vao == 0)
        {
            RUNTIME_WARNING("[ResourceManager] VAO not found: COSMIC_Sprite");
            return;
        }

        //std::cout << "[Render2DSprite] Rendering sprite " << textureKey << " at (" << position.x << ", " << position.y 
        //          << ") size (" << size.x << ", " << size.y << ")" << std::endl;

        spriteShader->Use();
        spriteShader->SetModel("model", glm::vec3(position, 0.0f), glm::vec3(size, 0.0f), glm::vec3(0.0f, 0.0f, rotation));
        spriteShader->SetProjection("projection", viewType);
        spriteShader->SetVec3("spriteColor", color);
        spriteShader->SetVec2("uvMin", glm::vec2(0.0f, 0.0f));
        spriteShader->SetVec2("uvMax", glm::vec2(1.0f, 1.0f));

        unsigned int textureIndex = 0;
        texture->Bind(textureIndex);
        spriteShader->SetInt("image", textureIndex);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        texture->Unbind();
        spriteShader->EndUse();
    }

    void ResourceManager::Render2DSpriteLit(
        const std::string &textureKey,
        glm::vec2 position,
        glm::vec2 size,
        float rotation,
        glm::vec3 color,
        float alpha,
        ViewType viewType
    )
    {
        Render2DSpriteWithShader(
            "COSMIC_Sprite",
            "COSMIC_SpriteSheet_Lit",
            textureKey,
            position,
            size,
            rotation,
            color,
            alpha,
            viewType
        );
    }

    void ResourceManager::Render2DSpriteWithShader(
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
        Texture2D *texture = GetTexture(textureKey);
        Shader *shader = GetShader(shaderKey);

        if ((vao == 0) || !texture || !shader)
        {
            return;
        }

        shader->Use();
        shader->SetModel("model", glm::vec3(position, 0.0f), glm::vec3(size, 0.0f), glm::vec3(0.0f, 0.0f, rotation));
        shader->SetProjection("projection", viewType);
        shader->SetVec3("spriteColor", color);
        shader->SetVec2("uvMin", glm::vec2(0.0f, 0.0f));
        shader->SetVec2("uvMax", glm::vec2(1.0f, 1.0f));

        unsigned int textureIndex = 0;
        texture->Bind(textureIndex);
        shader->SetInt("image", textureIndex);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        texture->Unbind();
        shader->EndUse();
    }

    void ResourceManager::Render2DSpriteFromTextureSheetUnlit(
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

        Shader *spriteSheetShader = GetShader("COSMIC_SpriteSheet_Unlit");

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

        glBindVertexArray(Get_Static_VAO("COSMIC_Sprite"));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        spriteSheetShader->EndUse();
    }

    void ResourceManager::Render2DSpriteFromTextureSheetLit(
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
        Render2DSpriteFromTextureSheetWithShader(
            "COSMIC_Sprite",
            "COSMIC_SpriteSheet_Lit",
            {{"image", textureKey}},
            row,
            column,
            position,
            size,
            rotation,
            color,
            alpha,
            viewType
        );
    }

    void ResourceManager::Render2DSpriteFromTextureSheetWithShader(
        const std::string &vaoKey,
        const std::string &shaderKey,
        const std::vector<std::pair<std::string, std::string>> &shaderVar_textureKey,  //"name on shader uniform - name on resource manager"
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
        glBindVertexArray(0);

        shader->EndUse();
    }


    glm::vec2 ResourceManager::MeasureText(
        const std::string& text,
        const std::string& fontKey,
        glm::vec3 scale
    ) {
        Font* fontResource = GetFont(fontKey);
        if (!fontResource) return glm::vec2(0.0f);

        const auto& characters = fontResource->GetCharacters();
    
        float width = 0.0f;
        float maxHeight = 0.0f;
    
        for (char c : text) {
            auto it = characters.find(c);
            if (it == characters.end())
            {
                continue;
            }

            const TextCharacter& ch = it->second;
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
        Font *fontResource = GetFont(fontKey);
        Shader* textShader = GetShader("COSMIC_Text");

        if (!(fontResource && textShader)) return;

        const auto& characters = fontResource->GetCharacters();
        auto baselineIt = characters.find('H');
        const float baselineBearingY = baselineIt != characters.end() ? static_cast<float>(baselineIt->second.Bearing.y) : 0.0f;

        textShader->Use();
        textShader->SetProjection("projection", viewType);
        textShader->SetInt("text", 0);
        textShader->SetVec3("textColor", color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(fontResource->GetVAO());
        glBindBuffer(GL_ARRAY_BUFFER, fontResource->GetVBO());

        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            auto it = characters.find(*c);
            if (it == characters.end())
            {
                continue;
            }

            const TextCharacter& ch = it->second;
    
            float xpos = position.x + ch.Bearing.x * scale.x;
            float ypos = position.y + (baselineBearingY - ch.Bearing.y) * scale.y;
    
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
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph
            position.x += (ch.Advance >> 6) * scale.x; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION


            void ResourceManager::RenderPoint(glm::vec2 coordinates, glm::vec3 color, float alpha, float width, ViewType viewType)
        {
            glPointSize(width);
                RenderShape("COSMIC_Point", {coordinates}, coordinates, glm::vec2(0.0f), color, alpha, width, GL_POINTS, viewType);
            glPointSize(1.0f);
        }

        void ResourceManager::RenderLine(glm::vec2 point_1, glm::vec2 point_2, glm::vec2 pivot, glm::vec2 rotation, glm::vec3 color, float alpha, float lineWidth, ViewType viewType)
        {
            RenderShape("COSMIC_Line", {point_1, point_2}, pivot, rotation, color, alpha, lineWidth, GL_LINES, viewType);
        }

        void ResourceManager::RenderTriangle(glm::vec2 point_1, glm::vec2 point_2, glm::vec2 point_3, glm::vec2 pivot, glm::vec2 rotation, glm::vec3 color, float alpha, float lineWidth, bool filled, ViewType viewType)
        {
            RenderShape("COSMIC_Triangle", {point_1, point_2, point_3}, pivot, rotation, color, alpha, lineWidth, GL_LINE_LOOP, viewType);
        }

        void ResourceManager::RenderRectangle(glm::vec2 point_1, glm::vec2 point_2, glm::vec2 pivot, glm::vec2 rotation, glm::vec3 color, float alpha, float lineWidth, bool filled, ViewType viewType)
        {
            RenderShape("COSMIC_Rectangle", {point_1, glm::vec2(point_2.x, point_1.y), point_2, glm::vec2(point_1.x, point_2.y)}, pivot, rotation, color, alpha, lineWidth, GL_LINE_LOOP, viewType);
        }


    void ResourceManager::RenderShape(
        const std::string &key,
        const std::vector<glm::vec2> &vertices,
        glm::vec2 pivot,
        glm::vec2 rotation,
        glm::vec3 color,
        float alpha,
        float lineWidth,
        GLenum drawMode,
        ViewType viewType
    )
    {
        auto dynamic_temp = Get_Dynamic_VAO_VBO(key);

        glBindBuffer(GL_ARRAY_BUFFER, dynamic_temp.second);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec2), vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        Shader *shapeShader = GetShader("COSMIC_Shape_2D");
        shapeShader->Use();

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(pivot, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -glm::vec3(pivot, 0.0f));

        shapeShader->SetMatrix4("model", model);
        shapeShader->SetProjection("projection", viewType);
        shapeShader->SetVec4("LineColor", glm::vec4(color, alpha));


        glLineWidth(lineWidth);

        glBindVertexArray(dynamic_temp.first);
        glDrawArrays(drawMode, 0, vertices.size());

        glBindVertexArray(0);

        shapeShader->EndUse();
    }

    bool ResourceManager::Load_Dynamic_VAO_VBO(const std::string &key, size_t vertexCount)
    {
        if (dynamic_vao_vbo_resources.find(key) != dynamic_vao_vbo_resources.end())
        {
            return false;
        }

        if (vertexCount <= 0)
        {
			RUNTIME_WARNING("[ResourceManager] Vertex count must be greater than 0.");
            return false;
        }


        std::pair<unsigned int, unsigned int> tmp_dynamic;

        glGenVertexArrays(1, &tmp_dynamic.first);
        glGenBuffers(1, &tmp_dynamic.second);

        glBindVertexArray(tmp_dynamic.first);
        glBindBuffer(GL_ARRAY_BUFFER, tmp_dynamic.second);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        dynamic_vao_vbo_resources[key] = tmp_dynamic;

        return true;
    }



    #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

        void ResourceManager::RenderRectangle(glm::vec2 point_1, glm::vec2 point_2, glm::vec2 pivot, glm::vec2 rotation, glm::vec3 color, float alpha, float lineWidth, bool filled, ViewType viewType)
        {
            (void)filled;

            auto dynamic_temp = Get_Dynamic_VAO_VBO("COSMIC_Rectangle");
            std::vector<glm::vec3> vertices = {
                glm::vec3(point_1, 0.0f),
                glm::vec3(point_2.x, point_1.y, 0.0f),
                glm::vec3(point_2, 0.0f),
                glm::vec3(point_1.x, point_2.y, 0.0f)
            };

            glBindBuffer(GL_ARRAY_BUFFER, dynamic_temp.second);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            Shader *shapeShader = GetShader("COSMIC_Shape_2D");
            shapeShader->Use();

            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(pivot, 0.0f));
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, -glm::vec3(pivot, 0.0f));

            shapeShader->SetMatrix4("model", model);
            shapeShader->SetProjection("projection", viewType);
            shapeShader->SetVec4("LineColor", glm::vec4(color, alpha));

            glLineWidth(lineWidth);
            glBindVertexArray(dynamic_temp.first);
            glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(vertices.size()));
            glBindVertexArray(0);

            shapeShader->EndUse();
        }

        void ResourceManager::RenderPoint(glm::vec3 coordinates, glm::vec3 color, float alpha, float width, ViewType viewType)
        {
            glPointSize(width);
            RenderShape("COSMIC_Point", {coordinates}, coordinates, glm::vec3(0.0f), color, alpha, width, GL_POINTS, viewType);
            glPointSize(1.0f);
        }

        void ResourceManager::RenderLine(glm::vec3 point_1, glm::vec3 point_2, glm::vec3 pivot, glm::vec3 rotation, glm::vec3 color, float alpha, float lineWidth, ViewType viewType)
        {
            RenderShape("COSMIC_Line", {point_1, point_2}, pivot, rotation, color, alpha, lineWidth, GL_LINES, viewType);
        }

        void ResourceManager::RenderTriangle(glm::vec3 point_1, glm::vec3 point_2, glm::vec3 point_3, glm::vec3 pivot, glm::vec3 rotation, glm::vec3 color, float alpha, float lineWidth, bool filled, ViewType viewType)
        {
            RenderShape("COSMIC_Triangle", {point_1, point_2, point_3}, pivot, rotation, color, alpha, lineWidth, GL_LINE_LOOP, viewType);
        }

        void ResourceManager::RenderRectangle(glm::vec3 point_1, glm::vec3 point_2, glm::vec3 pivot, glm::vec3 rotation, glm::vec3 color, float alpha, float lineWidth, bool filled, ViewType viewType)
        {
            //RenderShape("COSMIC_Rectangle", {point_1, glm::vec3(point_2.x, point_1.y, 0.0f), point_2, glm::vec3(point_1.x, point_2.y, 0.0f)}, pivot, rotation, color, alpha, lineWidth, GL_LINE_LOOP, viewType);
            RenderShape("COSMIC_Rectangle", {point_1, glm::vec3(point_2.x, point_1.y, point_1.z), point_2, glm::vec3(point_1.x, point_2.y, point_2.z)}, pivot, rotation, color, alpha, lineWidth, GL_LINE_LOOP, viewType);
            //TODO ADAP VERSION VEC2 AND VEC3 SEPARETLY
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

        Shader *shapeShader = GetShader("COSMIC_Shape_3D");

        shapeShader->Use();

        glm::mat4 model(1.0f);
        model = glm::translate(model, pivot);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, -pivot);

        shapeShader->SetMatrix4("model", model);
        
        shapeShader->SetMatrix4("view", CameraManager::GetInstance().GetViewMatrix());  // TODO OPTIMIZE FOR 3D AND FIX
        shapeShader->SetMatrix4("projection", CameraManager::GetInstance().GetProjectionMatrix());  // TODO OPTIMIZE FOR 3D AND FIX

        //shapeShader->SetProjection("projection", viewType);
        
        shapeShader->SetVec4("LineColor", glm::vec4(color, alpha));


        glLineWidth(lineWidth);

        glBindVertexArray(dynamic_temp.first);
        glDrawArrays(drawMode, 0, vertices.size());

        glBindVertexArray(0);

        shapeShader->EndUse();
    }

            
    void ResourceManager::RenderParallelepipedLines(
        glm::vec3 position,
        glm::vec3 size,
        glm::vec3 pivot,
        glm::vec3 rotation,
        glm::vec3 color,
        float alpha,
        float lineWidth,
        bool filled,
        ViewType viewType
    )
    {
        auto vao = Get_Static_VAO("COSMIC_Parallelepiped_Lines");

        Shader *shapeShader = GetShader("COSMIC_Shape_3D");
        shapeShader->Use();
        shapeShader->SetModel("model", position, size, rotation, position);

        shapeShader->SetMatrix4("view", CameraManager::GetInstance().GetViewMatrix());  // TODO OPTIMIZE FOR 3D AND FIX
        shapeShader->SetMatrix4("projection", CameraManager::GetInstance().GetProjectionMatrix());  // TODO OPTIMIZE FOR 3D AND FIX
        
        shapeShader->SetVec4("LineColor", glm::vec4(color, alpha));

        glLineWidth(lineWidth);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, 24);
        glBindVertexArray(0);

        shapeShader->EndUse();
    }


    void ResourceManager::RenderParallelepipedTextureLit(
        std::string textureKey,
        glm::vec3 position,
        glm::vec3 size,
        glm::vec3 pivot,
        glm::vec3 rotation,
        glm::vec3 color,
        float alpha,
        ViewType viewType
    )
    {
        auto vao = Get_Static_VAO("COSMIC_Parallelepiped");

        Shader *shader = GetShader("COSMIC_3DModel_Lit");
        Texture2D *texture = GetTexture(textureKey);
        Texture2D *texture2 = GetTexture("cobblestone_specular");

        if ((vao == 0) || !texture || !shader)
        {
            return;
        }

        shader->Use();
        shader->SetVec3("viewPos", CameraManager::GetInstance().GetPosition()); 
        shader->SetVec3("baseColor", color);
        shader->SetBool("hasDiffuseTexture", true);
        shader->SetBool("hasSpecularTexture", texture2 != nullptr);
        shader->SetModel("model", position, size, rotation, position);
        shader->SetMatrix4("view", CameraManager::GetInstance().GetViewMatrix());  // TODO OPTIMIZE FOR 3D AND FIX
        shader->SetMatrix4("projection", CameraManager::GetInstance().GetProjectionMatrix());  // TODO OPTIMIZE FOR 3D AND FIX
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetID());
        shader->SetInt("material.diffuse", 0);

        if (texture2)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2->GetID());
            shader->SetInt("material.specular", 1);
        }

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        shader->EndUse();
    }


    void ResourceManager::RenderParallelepipedTextureUnlit(
        std::string textureKey,
        glm::vec3 position,
        glm::vec3 size,
        glm::vec3 pivot,
        glm::vec3 rotation,
        glm::vec3 color,
        float alpha,
        ViewType viewType
    )
    {
        auto vao = Get_Static_VAO("COSMIC_Parallelepiped");

        Shader *shader = GetShader("COSMIC_3DModel_Unlit");
        Texture2D *texture = GetTexture(textureKey);

        if ((vao == 0) || !texture || !shader)
        {
            return;
        }

        shader->Use();
        shader->SetVec3("baseColor", color);
        shader->SetBool("hasDiffuseTexture", true);
        shader->SetBool("hasSpecularTexture", false);
        shader->SetModel("model", position, size, rotation, position);
        shader->SetMatrix4("view", CameraManager::GetInstance().GetViewMatrix());
        shader->SetMatrix4("projection", CameraManager::GetInstance().GetProjectionMatrix());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetID());
        shader->SetInt("material.diffuse", 0);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        shader->EndUse();
    }


    void ResourceManager::RenderParallelepipedTextureAtlasLit(
        std::string textureKey,
        glm::vec3 position,
        glm::vec3 size,
        glm::vec3 pivot,
        glm::vec3 rotation,
        glm::vec3 color,
        float alpha,
        ViewType viewType
    )
    {
        auto vao = Get_Static_VAO("COSMIC_Parallelepiped_CubeAtlas");

        Shader *shader = GetShader("COSMIC_3DModel_Lit");
        Texture2D *texture = GetTexture(textureKey);
        Texture2D *texture2 = GetTexture("cobblestone_specular");

        if ((vao == 0) || !texture || !shader)
        {
            return;
        }

        shader->Use();
        shader->SetVec3("viewPos", CameraManager::GetInstance().GetPosition());
        shader->SetVec3("baseColor", color);
        shader->SetBool("hasDiffuseTexture", true);
        shader->SetBool("hasSpecularTexture", texture2 != nullptr);
        shader->SetModel("model", position, size, rotation, position);
        shader->SetMatrix4("view", CameraManager::GetInstance().GetViewMatrix());
        shader->SetMatrix4("projection", CameraManager::GetInstance().GetProjectionMatrix());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetID());
        shader->SetInt("material.diffuse", 0);

        if (texture2)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2->GetID());
            shader->SetInt("material.specular", 1);
        }

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        shader->EndUse();
    }


    void ResourceManager::RenderParallelepipedTextureAtlasUnlit(
        std::string textureKey,
        glm::vec3 position,
        glm::vec3 size,
        glm::vec3 pivot,
        glm::vec3 rotation,
        glm::vec3 color,
        float alpha,
        ViewType viewType
    )
    {
        auto vao = Get_Static_VAO("COSMIC_Parallelepiped_CubeAtlas");

        Shader *shader = GetShader("COSMIC_3DModel_Unlit");
        Texture2D *texture = GetTexture(textureKey);

        if ((vao == 0) || !texture || !shader)
        {
            return;
        }

        shader->Use();
        shader->SetVec3("baseColor", color);
        shader->SetBool("hasDiffuseTexture", true);
        shader->SetBool("hasSpecularTexture", false);
        shader->SetModel("model", position, size, rotation, position);
        shader->SetMatrix4("view", CameraManager::GetInstance().GetViewMatrix());
        shader->SetMatrix4("projection", CameraManager::GetInstance().GetProjectionMatrix());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetID());
        shader->SetInt("material.diffuse", 0);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        shader->EndUse();
    }


    void ResourceManager::Render3DModelLit(
        const std::string &modelKey,
        glm::vec3 position,
        glm::vec3 size,
        glm::vec3 rotation,
        glm::vec3 color,
        float alpha,
        ViewType viewType
    )
    {

        Model *model = GetModel(modelKey);

        if (!model)
        {
            return;
        }

        Shader *modelShader = GetShader("COSMIC_3DModel_Lit");

        modelShader->Use();
        modelShader->SetVec3("viewPos", CameraManager::GetInstance().GetPosition()); 
        modelShader->SetVec3("baseColor", color);
        modelShader->SetModel("model", position, size, rotation, position);
        modelShader->SetMatrix4("view", CameraManager::GetInstance().GetViewMatrix());  // TODO OPTIMIZE FOR 3D AND FIX
        modelShader->SetProjection("projection", viewType);

        model->draw(*modelShader);

        modelShader->EndUse();

    }

    void ResourceManager::Render3DModelUnlit(
        const std::string &modelKey,
        glm::vec3 position,
        glm::vec3 size,
        glm::vec3 rotation,
        glm::vec3 color,
        float alpha,
        ViewType viewType
    )
    {

        Model *model = GetModel(modelKey);

        if (!model)
        {
            return;
        }

        Shader *modelShader = GetShader("COSMIC_3DModel_Unlit");

        modelShader->Use();
        modelShader->SetVec3("baseColor", color);
        modelShader->SetModel("model", position, size, rotation, position);
        modelShader->SetMatrix4("view", CameraManager::GetInstance().GetViewMatrix());
        modelShader->SetProjection("projection", viewType);

        model->draw(*modelShader);

        modelShader->EndUse();

    }

    bool ResourceManager::Load3DModel(const std::string &key, const std::string &path)
    {
        if (model_resources.find(key) != model_resources.end())
        {
            return false;
        }

        Model *model = new Model(path);

        model_resources[key] = model;

        return true;
    }


    bool ResourceManager::Load_Dynamic_VAO_VBO(const std::string &key, size_t vertexCount)
    {
        if (dynamic_vao_vbo_resources.find(key) != dynamic_vao_vbo_resources.end())
        {
            return false;
        }

        if (vertexCount <= 0)
        {
			RUNTIME_WARNING("[ResourceManager] Vertex count must be greater than 0.");
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


    #else
        #error "[ResourceManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif



}
