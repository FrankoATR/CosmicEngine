#ifndef COSMIC_FONT_HPP
#define COSMIC_FONT_HPP

#include <iostream>
#include <map>
#include <glm/glm.hpp>

namespace CosmicEngine
{
    struct TextCharacter
    {
        unsigned int TextureID;
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        unsigned int Advance;
    };

    class Font
    {
    private:
        std::map<char, TextCharacter> characters_resources;
        unsigned int VAO, VBO;

    public:
        Font(const std::string &fontPath, unsigned int fontSize);
        ~Font();
        
        const std::map<char, TextCharacter>& GetCharacters() const;
        unsigned int GetVAO() const;
        unsigned int GetVBO() const;
    };

}

#endif // COSMIC_FONT_HPP