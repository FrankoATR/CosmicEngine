#ifndef COSMIC_RESOURCEMANAGER_HPP
#define COSMIC_RESOURCEMANAGER_HPP

/**
 * @file resource_manager.hpp
 * @brief Declares the resource manager used to load and render engine assets.
 */

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
    /**
     * @brief Stores and renders engine resources such as textures, shaders, fonts, models, and primitive geometry.
     *
     * ResourceManager is the main asset hub.  It loads textures, texture sheets,
     * TrueType fonts, and 3D models into GPU-ready state, and provides a full set
     * of rendering helpers for 2D sprites, text, lines, rectangles, and 3D shapes.
     *
     * @par Example — loading resources in a scene
     * @code
     * RS_MN.LoadFont("test_font", "assets/fonts/ThaleahFat.ttf", 32);
     * RS_MN.LoadTexture("test_texture", "assets/textures/test.png");
     * RS_MN.LoadTextureSheet("player_sheet", "assets/textures/player2.png", 4, 4, 0);
     * @endcode
     *
     * @par Example — rendering a 2D sprite and text
     * @code
     * RS_MN.Render2DSprite("test_texture", position, size);
     * RS_MN.RenderText("Hello!", "test_font",
     *     {100.0f, 200.0f, 0.0f}, {0.55f, 0.55f, 1.0f});
     * @endcode
     *
     * @par Example — rendering from a texture sheet (sprite animation)
     * @code
     * RS_MN.Render2DSpriteFromTextureSheet(
     *     "player_sheet", row, col,
     *     position, size, rotation,
     *     color, alpha, CosmicEngine::ViewType::Ortho);
     * @endcode
     */
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
        /** @brief Returns the singleton instance of the resource manager. */
        static ResourceManager &GetInstance();

        /** @brief Initializes built-in resource state required by the renderer. */
        void init();

        /**
         * @brief Renders a textured 2D sprite using a previously loaded texture.
         * @param textureKey Texture resource key.
         * @param position Sprite position.
         * @param size Sprite size.
         * @param rotation Sprite rotation.
         * @param color Color tint multiplier.
         * @param alpha Alpha multiplier.
         * @param viewType Projection mode used for rendering.
         */
        void Render2DSprite(
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho
        );

        /**
         * @brief Renders a textured 2D sprite in UI space.
         * @param textureKey Texture resource key.
         * @param position Sprite position.
         * @param size Sprite size.
         * @param rotation Sprite rotation.
         * @param color Color tint multiplier.
         * @param alpha Alpha multiplier.
         * @param viewType Projection mode used for rendering.
         */
        void Render2DSprite_UI(
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho // NOT NECESSARY ¿? Verify for each
        );

        /**
         * @brief Renders a textured 2D sprite using explicit VAO and shader resources.
         * @param vaoKey Static VAO resource key.
         * @param shaderKey Shader resource key.
         * @param textureKey Texture resource key.
         * @param position Sprite position.
         * @param size Sprite size.
         * @param rotation Sprite rotation.
         * @param color Color tint multiplier.
         * @param alpha Alpha multiplier.
         * @param viewType Projection mode used for rendering.
         */
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

        /**
         * @brief Renders a tile from a texture sheet.
         * @param textureKey Texture-sheet resource key.
         * @param row Tile row.
         * @param column Tile column.
         * @param position Sprite position.
         * @param size Sprite size.
         * @param rotation Sprite rotation.
         * @param color Color tint multiplier.
         * @param alpha Alpha multiplier.
         * @param viewType Projection mode used for rendering.
         */
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

        /**
         * @brief Renders a tile from a texture sheet using explicit VAO, shader, and bound texture variables.
         * @param vaoKey Static VAO resource key.
         * @param shaderKey Shader resource key.
         * @param shaderVar_textureKey Texture bindings by shader variable name.
         * @param row Tile row.
         * @param column Tile column.
         * @param position Sprite position.
         * @param size Sprite size.
         * @param rotation Sprite rotation.
         * @param color Color tint multiplier.
         * @param alpha Alpha multiplier.
         * @param viewType Projection mode used for rendering.
         */
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

    /**
     * @brief Measures the size of a string rendered with the given font and scale.
     * @param text Text content to measure.
     * @param fontKey Font resource key.
     * @param scale Scale applied to glyph metrics.
     * @return Measured text size.
     */
        glm::vec2 MeasureText(
            const std::string& text,
            const std::string& fontKey,
            glm::vec3 scale
        );

        /**
         * @brief Renders a text string using a loaded font.
         * @param text Text content.
         * @param fontKey Font resource key.
         * @param position Text origin.
         * @param scale Text scale.
         * @param pivot Rotation and scaling pivot.
         * @param rotation Text rotation.
         * @param color Text color.
         * @param alpha Alpha multiplier.
         * @param viewType Projection mode used for rendering.
         */
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

            void RenderPoint(
                glm::vec3 coordinates,
                glm::vec3 color,
                float alpha = 1.0f,
                float width = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            void RenderLine(
                glm::vec3 point_1,
                glm::vec3 point_2,
                glm::vec3 pivot = glm::vec3(0.0f),
                glm::vec3 rotation = glm::vec3(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                float lineWidth = 1.0f,
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

            void RenderParallelepipedLines(
                glm::vec3 position,
                glm::vec3 size = glm::vec3(1.0f),
                glm::vec3 pivot = glm::vec3(0.0f),
                glm::vec3 rotation = glm::vec3(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
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


        /** @brief Loads a static VAO resource from raw vertex and attribute data. */
        bool Load_Static_VAO(const std::string &key, const std::vector<std::vector<float>> &vertices, const std::vector<int> &attributeSizes);
        /** @brief Loads a dynamic VAO/VBO pair sized for the requested vertex count. */
        bool Load_Dynamic_VAO_VBO(const std::string &key, size_t vertexCount);
        /** @brief Loads a shader program from source files. */
        bool LoadShaderFromPath(const std::string &key, const std::string &vs_path, const std::string &fs_path, const std::string &gs_path = "");
        /** @brief Loads a shader program from in-memory source strings. */
        bool LoadShaderFromCode(const std::string &key, const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
        /** @brief Loads a texture resource from disk. */
        bool LoadTexture(const std::string &key, const std::string &path);
        /** @brief Loads a texture sheet resource from disk. */
        bool LoadTextureSheet(const std::string &key, const std::string &path, int rows, int columns, int padding);

        /** @brief Loads a font resource with the requested glyph size. */
        bool LoadFont(const std::string &key, const std::string &path, unsigned int fontSize);


        /** @brief Releases every resource currently managed by the resource manager. */
        void Clear();
    };


    /** @brief Returns a convenient pointer to the singleton resource manager instance. */
    inline ResourceManager* RS_MNX() { return &ResourceManager::GetInstance(); }

}

#endif // COSMIC_RESOURCEMANAGER_HPP