#ifndef TYPEVARSANDMETHODS_HPP
#define TYPEVARSANDMETHODS_HPP

namespace TypeVarsAndMethods
{

    struct Vec2
    {
        float x, y;
        Vec2() = delete;
        Vec2(float x, float y) : x(x), y(y) {}
    };

    struct Vec3
    {
        float x, y, z;
        Vec3() = delete;
        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    };

}

#endif // TYPEVARSANDMETHODS_HPP