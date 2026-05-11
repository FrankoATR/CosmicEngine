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

    struct Vec2
    {
        float x, y;
        Vec2() = delete;
        Vec2(float x, float y) : x(x), y(y) {}
    };

    struct Size
    {
        float width, height;
        Size() : width(0.0f), height(0.0f) {}
        Size(float width, float height) : width(width), height(height) {}
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

    enum KeyEventType
    {
        KeyDown,
        KeyUp,
        KeyRelease
    };

}

#endif // DEFINITIONS_HPP