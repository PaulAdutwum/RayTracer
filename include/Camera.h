#pragma once

#include <algorithm>
#include <cmath>

#include "Ray.h"
#include "Vec3.h"

struct OrbitCamera
{
    Vec3 target{0.0f, 0.0f, 0.0f};
    float distance = 6.0f;
    float yaw = 0.0f;
    float pitch = 0.2f;
    float fov_degrees = 45.0f;

    float yaw_target = 0.0f;
    float pitch_target = 0.2f;
    float distance_target = 6.0f;

    void ClampTargets()
    {
        const float pitch_limit = 1.4f;
        pitch_target = std::clamp(pitch_target, -pitch_limit, pitch_limit);
        distance_target = std::clamp(distance_target, 2.0f, 20.0f);
    }

    void SmoothStep(float dt)
    {
        const float smooth = 8.0f;
        float t = 1.0f - std::exp(-smooth * dt);
        yaw += (yaw_target - yaw) * t;
        pitch += (pitch_target - pitch) * t;
        distance += (distance_target - distance) * t;
    }

    Vec3 Position() const
    {
        float cos_pitch = std::cos(pitch);
        return Vec3{
            target.x + distance * cos_pitch * std::sin(yaw),
            target.y + distance * std::sin(pitch),
            target.z + distance * cos_pitch * std::cos(yaw),
        };
    }

    Ray3 GetRay(float u, float v, float aspect) const
    {
        Vec3 position = Position();
        Vec3 forward = Normalize(target - position);
        Vec3 right = Normalize(Cross(forward, Vec3{0.0f, 1.0f, 0.0f}));
        Vec3 up = Cross(right, forward);

        float fov_rad = fov_degrees * 0.017453292f;
        float half_height = std::tan(0.5f * fov_rad);
        float half_width = aspect * half_height;

        Vec3 dir = Normalize(forward + right * (u * half_width) + up * (v * half_height));
        return Ray3{position, dir};
    }
};
