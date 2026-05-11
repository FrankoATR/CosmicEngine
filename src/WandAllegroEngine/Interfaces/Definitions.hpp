#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

namespace WandEngine
{

    struct WAND_COLOR
    {
        float r, g, b, a;
        WAND_COLOR() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
        WAND_COLOR(float r, float g, float b) : r(r), g(g), b(b), a(0.0f) {}
        WAND_COLOR(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    };

    struct WAND_VEC2
    {
        float x, y;
        WAND_VEC2() : x(0.0f), y(0.0f) {}
        WAND_VEC2(float x, float y) : x(x), y(y) {}
    };

    struct WAND_SIZE
    {
        float width, height;
        WAND_SIZE() : width(0.0f), height(0.0f) {}
        WAND_SIZE(float width, float height) : width(width), height(height) {}
    };

    enum class GameBodyObjectType
    {
        Body_Rectangle,
        Body_Circle,
        Body_Triangle
    };

    enum class Object
    {
        DynamicEntity,
        StaticEntity
    };

    enum class KeyEventType
    {
        KeyDown,
        KeyUp,
        KeyRelease
    };

    enum class UIElementType
    {
        Button,
        Label,
        Image
    };

}

#endif // DEFINITIONS_HPP