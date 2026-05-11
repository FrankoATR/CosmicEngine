#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "../../Models/Texture/GameTexture2D.hpp"
#include "../../Models/Shader/Shader.hpp"
#include "../../Models/TextFont/TextFont.hpp"

namespace WandEngine
{

    class ResourceManager
    {
    private:
        ResourceManager();
        ~ResourceManager();
        ResourceManager(const ResourceManager &) = delete;
        ResourceManager &operator=(const ResourceManager &) = delete;

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

        struct Texture_Sheet
        {
            Texture_Sheet(GameTexture2D *texture, int rows, int columns, int padding)
                : texture(texture), rows(rows), columns(columns), padding(padding) {}
            GameTexture2D *texture;
            int rows, columns, padding;
        };

        std::map<std::string, unsigned int> static_vao_resources;
        std::map<std::string, std::pair<unsigned int, unsigned int>> dynamic_vao_vbo_resources;
        std::map<std::string, GameTexture2D *> texture_resources;
        std::map<std::string, Texture_Sheet *> textures_sheet_resources;
        std::map<std::string, Shader *> shader_resources;
        std::map<std::string, TextFont *> text_font_resources;


        unsigned int Get_Static_VAO(const std::string &key) const;
        std::pair<unsigned int, unsigned int> Get_Dynamic_VAO_VBO(const std::string &key) const;
        GameTexture2D *GetTexture(const std::string &key) const;
        Texture_Sheet *GetTextureSheet(const std::string &key) const;
        Shader *GetShader(const std::string &key) const;
        TextFont *GetTextFont(const std::string &key) const;


        template <typename V, typename Callback>
        void ClearMap(std::map<std::string, V> &map, Callback callback) {
            for (auto& [key, value] : map) {
                callback(value);
            }
            map.clear();
        }

    public:
        static ResourceManager &GetInstance();

        void Init();

        void Render2DSprite(
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho
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



        void RenderPoint(
            glm::vec3 coordinates,
            glm::vec3 color,
            float alpha = 1.0f,
            float width = 1.0f,
            ViewType viewType = ViewType::Ortho
        );

        void RenderCircle(
            glm::vec3 center,
            float radius,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            float lineWidth = 1.0f,
            bool filled = false,
            ViewType viewType = ViewType::Ortho
        );

        void RenderLine(
            glm::vec3 point_1,
            glm::vec3 point_2,
            glm::vec3 pivot = glm::vec3(0.0f),
            glm::vec3 rotation = glm::vec3(0.0f),
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0, float lineWidth = 1.0f,
            ViewType viewType = ViewType::Ortho
        );

        void RenderTriangle(
            glm::vec3 point_1,
            glm::vec3 point_2,
            glm::vec3 point_3,
            glm::vec3 pivot = glm::vec3(0.0f),
            glm::vec3 rotation = glm::vec3(0.0f),
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            float lineWidth = 1.0f,
            bool filled = false,
            ViewType viewType = ViewType::Ortho
        );

        void RenderRectangle(
            glm::vec3 point_1,
            glm::vec3 point_2,
            glm::vec3 pivot = glm::vec3(0.0f),
            glm::vec3 rotation = glm::vec3(0.0f),
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            float lineWidth = 1.0f,
            bool filled = false,
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


        bool Load_Static_VAO(const std::string &key, const std::vector<std::vector<float>> &vertices);
        bool Load_Dynamic_VAO_VBO(const std::string &key, size_t vertexCount);
        bool LoadShaderFromPath(const std::string &key, const std::string &vs_path, const std::string &fs_path, const std::string &gs_path = "");
        bool LoadShaderFromCode(const std::string &key, const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
        bool LoadTexture(const std::string &key, const std::string &path, bool alpha);
        bool LoadTextureSheet(const std::string &key, const std::string &path, bool alpha, int rows, int columns, int padding);

        bool LoadTextFont(const std::string &key, const std::string &path, unsigned int fontSize);

        void Clear();
    };


    inline ResourceManager* RS_MNX() { return &ResourceManager::GetInstance(); }  // DEFINITIONS OR INLINES?

}

#endif // RESOURCEMANAGER_HPP