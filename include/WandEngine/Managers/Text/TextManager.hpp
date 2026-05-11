#ifndef TEXTMANAGER_HPP
#define TEXTMANAGER_HPP

#include <string>
#include <map>
#include "../../Models/Text/Text.hpp"

namespace WandEngine
{

    class TextManager
    {
    private:
        std::map<std::string, Text *> text_resources;
        unsigned int VAO, VBO;

    public:
        TextManager();
        ~TextManager();

        void Render(unsigned int width, unsigned int height);

        void Load(std::string font, unsigned int fontSize);

        void Clear();
    };

}

#endif // TEXTMANAGER_HPP