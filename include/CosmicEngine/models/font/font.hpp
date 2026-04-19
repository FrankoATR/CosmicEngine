#ifndef COSMIC_FONT_HPP
#define COSMIC_FONT_HPP

/**
 * @file font.hpp
 * @brief Declares the font resource used by the engine text renderer.
 */

#include <iostream>
#include <map>
#include <glm/glm.hpp>

namespace CosmicEngine
{
    /**
     * @brief Stores glyph rendering data for a single character.
     */
    struct TextCharacter
    {
        /** @brief Glyph texture identifier. */
        unsigned int TextureID;
        /** @brief Glyph size in pixels. */
        glm::ivec2 Size;
        /** @brief Glyph bearing relative to the baseline. */
        glm::ivec2 Bearing;
        /** @brief Horizontal cursor advance after rendering the glyph. */
        unsigned int Advance;
    };

    /**
     * @brief Stores preloaded glyphs and GPU buffers used to render text.
     *
     * Font uses FreeType to load a TrueType font file and rasterizes the first 128
     * ASCII glyphs into individual textures.  Fonts are loaded through the
     * ResourceManager and referenced by key when creating UI elements or calling
     * RS_MN.RenderText().
     *
     * @par Example — loading and using a font
     * @code
     * // Load once in Scene::loadResources():
     * RS_MN.LoadFont("test_font", "assets/fonts/ThaleahFat.ttf", 32);
     *
     * // Render text in draw():
     * RS_MN.RenderText("Hello!", "test_font",
     *     {100.0f, 200.0f, 0.0f}, {0.55f, 0.55f, 1.0f});
     *
     * // Reference the font for UI elements:
     * auto* label = new CosmicEngine::UIText("Score: 0", "test_font",
     *     {0, 24}, {800, 36}, true);
     * @endcode
     */
    class Font
    {
    private:
        std::map<char, TextCharacter> characters_resources;
        unsigned int VAO, VBO;

    public:
        /**
         * @brief Loads a font and prepares glyph resources for rendering.
         * @param fontPath Source font file path.
         * @param fontSize Requested glyph size.
         */
        Font(const std::string &fontPath, unsigned int fontSize);
        /** @brief Releases glyph textures and rendering buffers. */
        ~Font();
        
        /** @brief Returns the loaded glyph map. */
        const std::map<char, TextCharacter>& GetCharacters() const;
        /** @brief Returns the VAO used for text rendering. */
        unsigned int GetVAO() const;
        /** @brief Returns the VBO used for text rendering. */
        unsigned int GetVBO() const;
    };

}

#endif // COSMIC_FONT_HPP