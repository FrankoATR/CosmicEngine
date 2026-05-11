#ifndef TEXTFONT_HPP
#define TEXTFONT_HPP

#include <iostream>
#include <map>
#include <glm/glm.hpp>

namespace WandEngine
{
    struct TextCharacter
    {
        unsigned int TextureID;
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        unsigned int Advance;
    };

    class TextFont
    {
    private:
        std::map<char, TextCharacter> characters_resources;
        unsigned int VAO, VBO;

    public:
        TextFont(const std::string &fontPath, unsigned int fontSize);
        ~TextFont();
        
        std::map<char, TextCharacter> GetCharacters() const;
        unsigned int GetVAO() const;
        unsigned int GetVBO() const;
    };

}

#endif // TEXTFONT_HPP