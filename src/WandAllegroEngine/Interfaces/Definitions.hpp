#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

namespace WandEngine
{

    struct WAND_COLOR
    {
        float r, g, b, a;
        WAND_COLOR() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
        WAND_COLOR(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    };

}

#endif // DEFINITIONS_HPP