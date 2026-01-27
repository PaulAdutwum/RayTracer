#pragma once

#include <cmath>

struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    constexpr Vec3() = default;
    constexpr Vec3(float x_value, float y_value, float z_value)
        : x(x_value), y(y_value), z(z_value) {}

    constexpr Vec3 operator+(const Vec3 &other) const
    {
        return Vec3{x + other.x, y + other.y, z + other.z};
    }

    constexpr Vec3 operator-(const Vec3 &other) const
    {
        return Vec3{x - other.x, y - other.y, z - other.z};
    }

    constexpr Vec3 operator-() const
    {
        return Vec3{-x, -y, -z};
    }

    constexpr Vec3 operator*(float scalar) const
    {
        return Vec3{x * scalar, y * scalar, z * scalar};
    }

    constexpr Vec3 operator/(float scalar) const
    {
        return Vec3{x / scalar, y / scalar, z / scalar};
    }

    constexpr float operator[](int index) const
    {
        return (index == 0) ? x : (index == 1 ? y : z);
    }

    Vec3 &operator+=(const Vec3 &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3 &operator-=(const Vec3 &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
};

inline Vec3 operator*(float scalar, const Vec3 &v)
{
    return Vec3{v.x * scalar, v.y * scalar, v.z * scalar};
}

inline Vec3 operator*(const Vec3 &a, const Vec3 &b)
{
    return Vec3{a.x * b.x, a.y * b.y, a.z * b.z};
}

inline float Dot(const Vec3 &a, const Vec3 &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 Cross(const Vec3 &a, const Vec3 &b)
{
    return Vec3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

inline float Length(const Vec3 &v)
{
    return std::sqrt(Dot(v, v));
}

inline Vec3 Normalize(const Vec3 &v)
{
    float len = Length(v);
    return (len > 0.0f) ? (v / len) : Vec3{};
}
