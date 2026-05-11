#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "../../Models/Texture/GameTexture2D.hpp"
#include "../../Models/Shader/Shader.hpp"

namespace WandEngine
{

    class ResourceManager
    {
    private:
        struct ShapeResources
        {
            unsigned int VAO = 0;
            unsigned int VBO = 0;
            bool initialized = false;
            int vertexCount = 0;
        };

        ShapeResources lineResources, triangleResources, rectangleResources;

        void InitShapeResources(ShapeResources &shape, const std::vector<glm::vec3> &vertices);
        void RenderShape(ShapeResources &shape, const std::vector<glm::vec3> &vertices, glm::vec3 pivot, glm::vec3 rotation,
                         glm::vec3 color, float alpha, float lineWidth = 1.0f, GLenum drawMode = GL_LINE_LOOP);

        struct Texture_Sheet
        {
            Texture_Sheet(GameTexture2D *texture, int rows, int columns, int padding)
                : texture(texture), rows(rows), columns(columns), padding(padding) {}
            GameTexture2D *texture;
            int rows, columns, padding;
        };

        std::map<std::string, unsigned int> vao_resources;
        std::map<std::string, GameTexture2D *> texture_resources;
        std::map<std::string, Texture_Sheet *> textures_sheet_resources;
        std::map<std::string, Shader *> shader_resources;
        std::map<std::string, int *> font_resources;

        ResourceManager();
        ~ResourceManager();
        ResourceManager(const ResourceManager &) = delete;
        ResourceManager &operator=(const ResourceManager &) = delete;

        unsigned int GetVAO(const std::string &key) const;
        GameTexture2D *GetTexture(const std::string &key) const;
        Texture_Sheet *GetTextureSheet(const std::string &key) const;
        Shader *GetShader(const std::string &key) const;

        int *GetFont(const std::string &key) const;

    public:
        static ResourceManager &GetInstance()
        {
            static ResourceManager instance;
            return instance;
        }

        void Init();

        void Render2DSprite(
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f);

        void Render2DSprite(
            const std::string &vaoKey,
            const std::string &shaderKey,
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f);

        void Render2DSpriteFromTextureSheet(
            const std::string &textureKey,
            int row, int column,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f);

        void Render2DSpriteFromTextureSheet(
            const std::string &vaoKey, const std::string &shaderKey,
            const std::vector<std::pair<std::string, std::string>> &shaderVar_textureKey,
            int row, int column,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f);



        void RenderPoint(
            glm::vec3 coordinates,
            glm::vec3 color,
            float alpha = 1.0f,
            float width = 3.0f
        );

        void RenderCircle(
            glm::vec3 center,
            float radius,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            float lineWidth = 1.0f,
            bool filled = false);

        void RenderLine(
            glm::vec3 point_1,
            glm::vec3 point_2,
            glm::vec3 pivot = glm::vec3(0.0f),
            glm::vec3 rotation = glm::vec3(0.0f),
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0, float lineWidth = 1.0f);

        void RenderTriangle(
            glm::vec3 point_1,
            glm::vec3 point_2,
            glm::vec3 point_3,
            glm::vec3 pivot = glm::vec3(0.0f),
            glm::vec3 rotation = glm::vec3(0.0f),
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            float lineWidth = 1.0f,
            bool filled = false);

        void RenderRectangle(
            glm::vec3 point_1,
            glm::vec3 point_2,
            glm::vec3 pivot = glm::vec3(0.0f),
            glm::vec3 rotation = glm::vec3(0.0f),
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            float lineWidth = 1.0f,
            bool filled = false);

        bool LoadVAO(const std::string &key, const std::vector<std::vector<float>> &vertices);
        bool LoadShaderFromPath(const std::string &key, const std::string &vs_path, const std::string &fs_path, const std::string &gs_path = "");
        bool LoadShaderFromCode(const std::string &key, const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
        bool LoadTexture(const std::string &key, const std::string &path, bool alpha);
        bool LoadTextureSheet(const std::string &key, const std::string &path, bool alpha, int rows, int columns, int padding);


        bool LoadFont(const std::string &key, const std::string &path, int size);

        void Clear();
    };

}

#endif // RESOURCEMANAGER_HPP