#include "TextFont.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace WandEngine
{

    TextFont::TextFont(const std::string &fontPath, unsigned int fontSize)
    {
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glBindVertexArray(this->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // first clear the previously loaded Characters
        characters_resources.clear();
        // then initialize and load the FreeType library
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) // all functions return a value different than 0 whenever an error occurred
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        // load font as face
        FT_Face face;
        if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, fontSize);
        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // then for the first 128 ASCII characters, pre-load/compile their characters and store them
        for (GLubyte c = 0; c < 128; c++) // lol see what I did there
        {
            // load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer);
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // now store character for later use
            TextCharacter character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x};
            characters_resources.insert(std::pair<char, TextCharacter>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    TextFont::~TextFont()
    {

    }

    std::map<char, TextCharacter> TextFont::GetCharacters() const
    {
        return characters_resources;
    }

    unsigned int TextFont::GetVAO() const
    {
        return VAO;
    }

    unsigned int TextFont::GetVBO() const
    {
        return VBO;
    }

}