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
    * @par Example - loading resources in a scene
     * @code
     * RS_MN.LoadFont("test_font", "assets/fonts/ThaleahFat.ttf", 32);
     * RS_MN.LoadTexture("test_texture", "assets/textures/test.png");
     * RS_MN.LoadTextureSheet("player_sheet", "assets/textures/player2.png", 4, 4, 0);
     * @endcode
     *
    * @par Example - rendering a 2D sprite and text
     * @code
    * RS_MN.Render2DSpriteUnlit("test_texture", position, size);
     * RS_MN.RenderText("Hello!", "test_font",
     *     {100.0f, 200.0f, 0.0f}, {0.55f, 0.55f, 1.0f});
     * @endcode
     *
    * @par Example - rendering from a texture sheet (sprite animation)
     * @code
    * RS_MN.Render2DSpriteFromTextureSheetUnlit(
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



        /**
         * @brief Wraps a Texture2D with sprite-sheet metadata.
         *
         * Used by the resource manager to keep ownership of texture sheets and the
         * subdivision parameters (rows, columns, padding between cells) needed to
         * compute UV rectangles for individual frames.
         */
        struct Texture_Sheet
        {
            /**
             * @brief Builds a sheet descriptor with the given grid layout.
             * @param texture Owned texture instance.
             * @param rows Number of frame rows in the sheet.
             * @param columns Number of frame columns in the sheet.
             * @param padding Pixel padding between frames.
             */
            Texture_Sheet(Texture2D *texture, int rows, int columns, int padding)
                : texture(texture), rows(rows), columns(columns), padding(padding) {}
            Texture2D *texture; ///< Owned texture data uploaded to the GPU.
            int rows;           ///< Frame rows in the sheet.
            int columns;        ///< Frame columns in the sheet.
            int padding;        ///< Pixel padding between adjacent frames.
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

        void Render2DSpriteWithShader(
            const std::string &vaoKey,
            const std::string &shaderKey,
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation,
            glm::vec3 color,
            float alpha,
            ViewType viewType
        );

        void Render2DSpriteFromTextureSheetWithShader(
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
        );

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

        /** @brief Initializes built-in resource state required by the renderer.
         *  @return True when all mandatory built-in resources were created successfully.
         *  GameManager treats a false result as a fatal startup error because rendering cannot continue safely without these assets.
         */
        bool init();

        /** @brief Clears scene-loaded assets while preserving built-in COSMIC renderer resources. */
        void ClearSceneResources();

        /**
         * @brief Renders a textured 2D sprite using the built-in unlit shader.
         * @param textureKey Texture resource key.
         * @param position Sprite position.
         * @param size Sprite size.
         * @param rotation Sprite rotation.
         * @param color Color tint multiplier.
         * @param alpha Alpha multiplier.
         * @param viewType Projection mode used for rendering.
         */
        void Render2DSpriteUnlit(
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho
        );

        /**
         * @brief Renders a textured 2D sprite using the built-in lit shader.
         * @param textureKey Texture resource key.
         * @param position Sprite position.
         * @param size Sprite size.
         * @param rotation Sprite rotation.
         * @param color Color tint multiplier.
         * @param alpha Alpha multiplier.
         * @param viewType Projection mode used for rendering.
         */
        void Render2DSpriteLit(
            const std::string &textureKey,
            glm::vec2 position,
            glm::vec2 size,
            float rotation = 0.0f,
            glm::vec3 color = glm::vec3(1.0f),
            float alpha = 1.0f,
            ViewType viewType = ViewType::Ortho
        );

        /**
         * @brief Renders a tile from a texture sheet using the built-in unlit shader.
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
        void Render2DSpriteFromTextureSheetUnlit(
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
         * @brief Renders a lit tile from a texture sheet using the built-in lit shader.
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
        void Render2DSpriteFromTextureSheetLit(
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

            /**
             * @brief Renders a textured parallelepiped using the built-in lit shader.
             * @param textureKey Texture resource key.
             * @param position Parallelepiped position.
             * @param size Parallelepiped scale.
             * @param pivot Rotation pivot.
             * @param rotation Euler rotation in degrees.
             * @param color Base color fallback when textures are missing.
             * @param alpha Alpha multiplier.
             * @param viewType Projection mode used for rendering.
             */
            void RenderParallelepipedTextureLit(
                std::string textureKey,
                glm::vec3 position,
                glm::vec3 size = glm::vec3(1.0f),
                glm::vec3 pivot = glm::vec3(0.0f),
                glm::vec3 rotation = glm::vec3(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            /**
             * @brief Renders a textured parallelepiped using the built-in unlit shader.
             * @param textureKey Texture resource key.
             * @param position Parallelepiped position.
             * @param size Parallelepiped scale.
             * @param pivot Rotation pivot.
             * @param rotation Euler rotation in degrees.
             * @param color Base color fallback when textures are missing.
             * @param alpha Alpha multiplier.
             * @param viewType Projection mode used for rendering.
             */
            void RenderParallelepipedTextureUnlit(
                std::string textureKey,
                glm::vec3 position,
                glm::vec3 size = glm::vec3(1.0f),
                glm::vec3 pivot = glm::vec3(0.0f),
                glm::vec3 rotation = glm::vec3(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            /**
             * @brief Renders a textured cube using a cross-layout atlas and the built-in lit shader.
             * @param textureKey Texture atlas resource key.
             * @param position Cube position.
             * @param size Cube scale.
             * @param pivot Rotation pivot.
             * @param rotation Euler rotation in degrees.
             * @param color Base color fallback when textures are missing.
             * @param alpha Alpha multiplier.
             * @param viewType Projection mode used for rendering.
             */
            void RenderParallelepipedTextureAtlasLit(
                std::string textureKey,
                glm::vec3 position,
                glm::vec3 size = glm::vec3(1.0f),
                glm::vec3 pivot = glm::vec3(0.0f),
                glm::vec3 rotation = glm::vec3(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            /**
             * @brief Renders a textured cube using a cross-layout atlas and the built-in unlit shader.
             * @param textureKey Texture atlas resource key.
             * @param position Cube position.
             * @param size Cube scale.
             * @param pivot Rotation pivot.
             * @param rotation Euler rotation in degrees.
             * @param color Base color fallback when textures are missing.
             * @param alpha Alpha multiplier.
             * @param viewType Projection mode used for rendering.
             */
            void RenderParallelepipedTextureAtlasUnlit(
                std::string textureKey,
                glm::vec3 position,
                glm::vec3 size = glm::vec3(1.0f),
                glm::vec3 pivot = glm::vec3(0.0f),
                glm::vec3 rotation = glm::vec3(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            /**
             * @brief Renders a 3D model using the built-in lit shader.
             * @param modelKey Model resource key.
             * @param position Model position.
             * @param size Model scale.
             * @param rotation Model rotation.
             * @param color Base color fallback used when the model has no diffuse texture.
             * @param alpha Alpha multiplier.
             * @param viewType Projection mode used for rendering.
             */
            void Render3DModelLit(
                const std::string &modelKey,
                glm::vec3 position,
                glm::vec3 size,
                glm::vec3 rotation = glm::vec3(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            /**
             * @brief Renders a 3D model using the built-in unlit shader.
             * @param modelKey Model resource key.
             * @param position Model position.
             * @param size Model scale.
             * @param rotation Model rotation.
             * @param color Base color fallback used when the model has no diffuse texture.
             * @param alpha Alpha multiplier.
             * @param viewType Projection mode used for rendering.
             */
            void Render3DModelUnlit(
                const std::string &modelKey,
                glm::vec3 position,
                glm::vec3 size,
                glm::vec3 rotation = glm::vec3(0.0f),
                glm::vec3 color = glm::vec3(1.0f),
                float alpha = 1.0f,
                ViewType viewType = ViewType::Ortho
            );

            /**
             * @brief Loads a 3D model from disk and registers it under @p key.
             *
             * Supported formats are those handled by Assimp (.obj, .fbx, .gltf, ...).
             * The created Model is owned by the manager and released on shutdown.
             *
             * @param key Lookup identifier for later retrieval through GetModel().
             * @param path Filesystem path to the model file.
             * @return True when the model was loaded and registered successfully.
             */
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
        /** @brief Registers an already loaded shader with the light manager by key. */
        bool RegisterShaderForLighting(const std::string &key);
        /** @brief Loads a texture resource from disk. */
        bool LoadTexture(const std::string &key, const std::string &path);
        /** @brief Loads a texture sheet resource from disk. */
        bool LoadTextureSheet(const std::string &key, const std::string &path, int rows, int columns, int padding);

        /** @brief Loads a font resource with the requested glyph size. */
        bool LoadFont(const std::string &key, const std::string &path, unsigned int fontSize);


        /** @brief Releases every resource currently managed by the resource manager. */
        void Clear();
    };

}

#endif // COSMIC_RESOURCEMANAGER_HPP
