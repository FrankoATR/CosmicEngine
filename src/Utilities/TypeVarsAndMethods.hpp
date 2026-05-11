#ifndef TYPEVARSANDMETHODS_HPP
#define TYPEVARSANDMETHODS_HPP

namespace TypeVarsAndMethods
{

    struct Vec3
    {
        float x, y, z;
        Vec3() = delete;
        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    };



}

#endif // TYPEVARSANDMETHODS_HPP