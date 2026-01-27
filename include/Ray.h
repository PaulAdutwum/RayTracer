#pragma once

#include "Vec3.h"

struct Ray3
{
    Vec3 origin;
    Vec3 direction;

    constexpr Ray3() = default;
    constexpr Ray3(const Vec3 &origin_value, const Vec3 &direction_value)
        : origin(origin_value), direction(direction_value) {}

    constexpr Vec3 At(float t) const
    {
        return origin + direction * t;
    }
};
