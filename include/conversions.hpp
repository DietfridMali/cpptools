#pragma once

#define _USE_MATH_DEFINES // for C++

#include <cmath>
#include <limits>

#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif


namespace Conversions
{
    static float DegToRad(float deg) {
        return static_cast<float>((static_cast<double>(deg) / 180.0 * M_PI));
    }

    static float RadToDeg(float rad) {
        return static_cast<float>((static_cast<double>(rad) * 180.0 / M_PI));
    }

    static float DotToRad(float dot) {
        return static_cast<float>(acos(dot));
    }

    static float DotToDeg(float dot) {
        return RadToDeg(DotToRad(dot));
    }

    template <typename T, size_t N>
    constexpr size_t ArrayLength(const T(&)[N]) noexcept { return N; }

    template<typename T>
    constexpr int Sign(T val) {
        return (T(0) < val) - (val < T(0));
    }

    constexpr float NumericTolerance = 0.000001f;

    template<typename T>
    struct Interval {
        T min = std::numeric_limits<T>::lowest();
        T max = std::numeric_limits<T>::max();
        bool Contains(T v) const { return v >= this->min and v <= this->max; }
    };

    using FloatInterval = Interval<float>;
};
