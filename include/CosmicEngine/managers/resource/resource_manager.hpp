#ifndef COSMIC_RESOURCEMANAGER_HPP
#define COSMIC_RESOURCEMANAGER_HPP

#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "../../models/texture/texture_2d.hpp"
#include "../../models/shader/shader.hpp"
#include "../../models/font/font.hpp"
#include "../../models/model/model.hpp"

namespace CosmicEngine
{

    class ResourceManager
    {
    private:
        ResourceManager();
        ~ResourceManager();
        ResourceManager(const ResourceManager &) = delete;
        ResourceManager &operator=(const ResourceManager &) = delete;


    #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        void RenderShape(
            const std::string &key, 
            const std::vector<glm::vec2> &vertices, 
            glm::vec2 pivot, 
            glm::vec2 rotation,
            glm::vec3 color,
            float alpha, 
            float lineWidth = 1.0f, 
            GLenum drawMode = GL_LINE_LOOP,
            ViewType viewType = ViewType::Ortho
        );
    #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        void RenderShape(
            const std::string &key, 
            const std::vector<glm::vec3> &vertices, 
            glm::vec3 pivot, 
            glm::vec3 rotation,
            glm::vec3 color,
            float alpha, 
            float lineWidth = 1.0f, 
            GLenum drawMode = GL_LINE_LOOP,
            ViewType viewType = ViewType::Ortho
        );
    #else
        #error "[ResourceManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif   



        struct Texture_Sheet
        {
            Texture_Sheet(Texture2D *texture, int rows, int columns, int padding)
                : texture(texture), rows(rows), columns(columns), padding(padding) {}
            Texture2D *texture;
            int rows, columns, padding;
        };

        std::map<std::string, unsigned int> static_vao_resources;
        std::map<std::string, std::pair<unsigned int, unsigned int>> dynamic_vao_vbo_resources;
        std::map<std::string, Texture2D *> texture_resources;
        std::map<std::string, Texture_Sheet *> textures_sheet_resources;
        std::map<std::string, Shader *> shader_resources;
        std::map<std::string, Font *> text_font_resources;
        std::map<std::string, Model *> model_resources;

        unsigned int Get_Static_VAO(const std::string &key) const;
        std::pair<unsigned int, unsigned int> Get_Dynamic_VAO_VBO(const std::string &key) const;
        Shader *GetShader(const std::string &key) const;
        Texture2D *GetTexture(const std::string &key) const;
        Texture_Sheet *GetTextureSheet(const std::string &key) const;
        Font *GetFont(const std::string &key) const;

        Model *GetModel(const std::string &key) const;

        template <typename V, typename Callback>
        void ClearMap(std::map<std::string, V> &map, Callback callback) {
            for (auto& [key, value] : map) {
                callback(value);
            }
            map.clear();
        }

    public:
        static ResourceManager &GetInstance();

        void init();

        void Render2DSprite(
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho
        );

        void Render2DSprite_UI(
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho // NOT NECESSARY ¿? Verify for each
        );

        void Render2DSprite(
            const std::string &vaoKey,
            const std::string &shaderKey,
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho
        );

        void Render2DSpriteFromTextureSheet(
            const std::string &textureKey,
            int row, int column,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho
        );

        void Render2DSpriteFromTextureSheet(
            const std::string &vaoKey, const std::string &shaderKey,
            const std::vector<std::pair<std::string, std::string>> &shaderVar_textureKey,
            int row, int column,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho
        );


        glm::vec2 MeasureText(
            const std::string& text,
            const std::string& fontKey,
            glm::vec3 scale
        );

        void RenderText(
            const std::string &text,
            const std::string &fontKey,
            glm::vec3 position,
            glm::vec3 scale = glm::vec3(1.0f),
            glm::vec3 pivot = glm::vec3(0.0f),
            glm::vec3 rotation = glm::vec3(0.0f),
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho
        );


        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION

            void RenderPoint(
                glm::vec2 coordinates,
                glm::vec3 color,
                float alpha = 1.0f,
                float width = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            void RenderCircle(
                glm::vec2 center,
                float radius,
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                float lineWidth = 1.0f,
                bool filled = false,
                ViewType viewType = ViewType::Ortho
            );

            void RenderLine(
                glm::vec2 point_1,
                glm::vec2 point_2,
                glm::vec2 pivot = glm::vec2(0.0f),
                glm::vec2 rotation = glm::vec2(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0, float lineWidth = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            void RenderTriangle(
                glm::vec2 point_1,
                glm::vec2 point_2,
                glm::vec2 point_3,
                glm::vec2 pivot = glm::vec2(0.0f),
                glm::vec2 rotation = glm::vec2(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                float lineWidth = 1.0f,
                bool filled = false,
                ViewType viewType = ViewType::Ortho
            );

            void RenderRectangle(
                glm::vec2 point_1,
                glm::vec2 point_2,
                glm::vec2 pivot = glm::vec2(0.0f),
                glm::vec2 rotation = glm::vec2(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                float lineWidth = 1.0f,
                bool filled = false,
                ViewType viewType = ViewType::Ortho
            );

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

            void RenderPoint(
                glm::vec3 coordinates,
                glm::vec3 color,
                float alpha = 1.0f,
                float width = 1.0f,
                ViewType viewType = ViewType::Ortho
                float lineWidth = 1.0f,
                bool filled = false,
                ViewType viewType = ViewType::Ortho
            );

            void RenderParallelepipedTexture(
                std::string textureKey,
                glm::vec3 position,
                glm::vec3 size = glm::vec3(1.0f),
                glm::vec3 pivot = glm::vec3(0.0f),
                glm::vec3 rotation = glm::vec3(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            void Render3DModel(
                const std::string &modelKey,
                glm::vec3 position,
                glm::vec3 size,
                glm::vec3 rotation = glm::vec3(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            bool Load3DModel(const std::string &key, const std::string &path);

        #else
            #error "[ResourceManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif   


        bool Load_Static_VAO(const std::string &key, const std::vector<std::vector<float>> &vertices, const std::vector<int> &attributeSizes);
        bool Load_Dynamic_VAO_VBO(const std::string &key, size_t vertexCount);
        bool LoadShaderFromPath(const std::string &key, const std::string &vs_path, const std::string &fs_path, const std::string &gs_path = "");
        bool LoadShaderFromCode(const std::string &key, const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
        bool LoadTexture(const std::string &key, const std::string &path);
        bool LoadTextureSheet(const std::string &key, const std::string &path, int rows, int columns, int padding);

        bool LoadFont(const std::string &key, const std::string &path, unsigned int fontSize);


        void Clear();
    };


    inline ResourceManager* RS_MNX() { return &ResourceManager::GetInstance(); }  // DEFINITIONS OR INLINES?

}

#endif // COSMIC_RESOURCEMANAGER_HPP