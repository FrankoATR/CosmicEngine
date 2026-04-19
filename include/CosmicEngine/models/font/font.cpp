/**
 * @file font.cpp
 * @brief Implements the font resource used by the engine text renderer.
 */

#include "font.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdexcept>

#include "../../utils/log.hpp"

namespace CosmicEngine
{

    Font::Font(const std::string &fontPath, unsigned int fontSize)
    {
        this->VAO = 0;
        this->VBO = 0;
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glBindVertexArray(this->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        characters_resources.clear();
        FT_Library ft;
        FT_Face face = nullptr;
        if (FT_Init_FreeType(&ft))
        {
            if (this->VBO != 0)
            {
                glDeleteBuffers(1, &this->VBO);
                this->VBO = 0;
            }
            if (this->VAO != 0)
            {
                glDeleteVertexArrays(1, &this->VAO);
                this->VAO = 0;
            }
            throw std::runtime_error("[Font] Could not initialize FreeType library.");
        }

        if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
        {
            FT_Done_FreeType(ft);
            if (this->VBO != 0)
            {
                glDeleteBuffers(1, &this->VBO);
                this->VBO = 0;
            }
            if (this->VAO != 0)
            {
                glDeleteVertexArrays(1, &this->VAO);
                this->VAO = 0;
            }
            throw std::runtime_error(std::string("[Font] Failed to load font: ") + fontPath);
        }

        FT_Set_Pixel_Sizes(face, 0, fontSize);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (GLubyte c = 0; c < 128; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
				RUNTIME_WARNING("[Font] Failed to load glyph: " << static_cast<int>(c));
                continue;
            }
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

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            TextCharacter character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };

            characters_resources.insert(std::pair<char, TextCharacter>(c, character));
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    Font::~Font()
    {
        for (auto& [c, character] : characters_resources)
        {
            if (character.TextureID != 0)
            {
                glDeleteTextures(1, &character.TextureID);
                character.TextureID = 0;
            }
        }
    
        characters_resources.clear();

            if (VBO != 0)
            {
                glDeleteBuffers(1, &VBO);
                VBO = 0;
            }

            if (VAO != 0)
            {
                glDeleteVertexArrays(1, &VAO);
                VAO = 0;
            }
    }

    const std::map<char, TextCharacter>& Font::GetCharacters() const
    {
        return characters_resources;
    }

    unsigned int Font::GetVAO() const
    {
        return VAO;
    }

    unsigned int Font::GetVBO() const
    {
        return VBO;
    }

}