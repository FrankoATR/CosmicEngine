#ifndef TEXT_HPP
#define TEXT_HPP

#include <map>
#include <glm/glm.hpp>

namespace WandEngine
{

    class Text
    {
    private:
        struct Character
        {
            unsigned int TextureID;
            glm::ivec2 Size;
            glm::ivec2 Bearing;
            unsigned int Advance;
        };
        std::map<char, Character> Characters;

        unsigned int VAO, VBO;

    public:
        Text();
        
        ~Text();
        
    };

}

#endif // TEXT_HPP