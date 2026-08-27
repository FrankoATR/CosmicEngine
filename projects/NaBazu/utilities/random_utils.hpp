#ifndef NABAZU_RANDOM_UTILS_HPP
#define NABAZU_RANDOM_UTILS_HPP

#include <random>

namespace NaBazu
{
    inline float RandomFloat01()
    {
        static std::mt19937 generator(std::random_device{}());
        static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
        return distribution(generator);
    }

    inline float RandomRange(float minValue, float maxValue)
    {
        static std::mt19937 generator(std::random_device{}());
        std::uniform_real_distribution<float> distribution(minValue, maxValue);
        return distribution(generator);
    }
}

#endif
