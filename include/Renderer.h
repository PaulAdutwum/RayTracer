#pragma once

#include <algorithm>
#include <cmath>
#include <random>
#include <thread>
#include <vector>

#include "raylib.h"

#include "BVH.h"
#include "Camera.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Vec3.h"

struct RenderParams
{
    Sphere sphere;
    Vec3 light_position;
    float light_radius = 0.8f;
    float light_intensity = 3.0f;
    int shadow_samples = 8;
    bool debug_normals = false;
    Vec3 albedo = Vec3{0.9f, 0.35f, 0.25f};
    float roughness = 0.35f;
    float metallic = 0.05f;
};

inline Vec3 Clamp01(const Vec3 &color)
{
    return Vec3{
        std::clamp(color.x, 0.0f, 1.0f),
        std::clamp(color.y, 0.0f, 1.0f),
        std::clamp(color.z, 0.0f, 1.0f),
    };
}

inline unsigned char ToByte(float v)
{
    float clamped = std::clamp(v, 0.0f, 1.0f);
    return static_cast<unsigned char>(clamped * 255.0f + 0.5f);
}

inline Vec3 FresnelSchlick(float cos_theta, const Vec3 &f0)
{
    float t = std::pow(1.0f - std::clamp(cos_theta, 0.0f, 1.0f), 5.0f);
    return f0 + (Vec3{1.0f, 1.0f, 1.0f} - f0) * t;
}

inline Vec3 ShadeHit(const HitRecord &hit,
                     const Vec3 &view_dir,
                     const RenderParams &params,
                     const Hittable &scene,
                     std::mt19937 &rng)
{
    if (params.debug_normals)
    {
        return 0.5f * (hit.normal + Vec3{1.0f, 1.0f, 1.0f});
    }

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    Vec3 f0 = Vec3{0.04f, 0.04f, 0.04f} * (1.0f - params.metallic) + params.albedo * params.metallic;
    float ambient = 0.12f;
    Vec3 color = params.albedo * ambient;

    int samples = std::max(1, params.shadow_samples);
    float shadow_accum = 0.0f;
    Vec3 light_accum{};
    for (int i = 0; i < samples; ++i)
    {
        Vec3 rand_dir = Normalize(Vec3{dist(rng) * 2.0f - 1.0f,
                                       dist(rng) * 2.0f - 1.0f,
                                       dist(rng) * 2.0f - 1.0f});
        Vec3 light_pos = params.light_position + rand_dir * params.light_radius;
        Vec3 to_light = light_pos - hit.point;
        float light_dist = Length(to_light);
        Vec3 light_dir = to_light / std::max(1e-4f, light_dist);

        Ray3 shadow_ray{hit.point + hit.normal * 0.001f, light_dir};
        HitRecord shadow_hit;
        bool occluded = scene.Hit(shadow_ray, 0.001f, light_dist - 0.002f, shadow_hit);
        if (!occluded)
        {
            float ndotl = std::max(0.0f, Dot(hit.normal, light_dir));
            Vec3 half_vec = Normalize(light_dir + view_dir);
            float ndoth = std::max(0.0f, Dot(hit.normal, half_vec));
            float spec_power = 2.0f + std::pow(1.0f - params.roughness, 4.0f) * 128.0f;
            float spec = std::pow(ndoth, spec_power);

            Vec3 fresnel = FresnelSchlick(std::max(0.0f, Dot(view_dir, half_vec)), f0);
            Vec3 diffuse = params.albedo * (1.0f - params.metallic);
            Vec3 light_color = params.light_intensity * Vec3{1.0f, 0.98f, 0.92f};
            light_accum += (diffuse * ndotl + fresnel * spec) * light_color;
            shadow_accum += 1.0f;
        }
    }

    if (shadow_accum > 0.0f)
    {
        color += light_accum / shadow_accum;
    }

    return Clamp01(color);
}

inline Vec3 BackgroundColor(float v)
{
    float t = 0.5f * (v + 1.0f);
    return Clamp01(Vec3{0.12f, 0.14f, 0.18f} * (1.0f - t) +
                   Vec3{0.02f, 0.04f, 0.08f} * t);
}

inline void RenderScene(std::vector<Color> &pixels,
                        int width,
                        int height,
                        const OrbitCamera &camera,
                        const RenderParams &params,
                        unsigned int thread_count)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    pixels.resize(static_cast<size_t>(width * height));

    thread_count = std::max(1u, thread_count);
    thread_count = std::min(thread_count, static_cast<unsigned int>(height));
    int rows_per_thread = std::max(1, height / static_cast<int>(thread_count));

    std::vector<HittablePtr> objects;
    objects.reserve(2);
    objects.push_back(std::make_shared<Sphere>(params.sphere));
    Triangle tri;
    tri.v0 = Vec3{-2.0f, -1.0f, -2.0f};
    tri.v1 = Vec3{2.0f, -1.0f, -2.0f};
    tri.v2 = Vec3{0.0f, 1.5f, -3.0f};
    objects.push_back(std::make_shared<Triangle>(tri));

    HittablePtr bvh_root = BuildBVH(objects, 0, objects.size());

    auto render_rows = [&](int y_start, int y_end)
    {
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        std::mt19937 rng(static_cast<unsigned int>(y_start * 73856093u + width * 19349663u));
        for (int y = y_start; y < y_end; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                float u = (2.0f * (static_cast<float>(x) + 0.5f) / static_cast<float>(width) - 1.0f);
                float v = (1.0f - 2.0f * (static_cast<float>(y) + 0.5f) / static_cast<float>(height));

                Ray3 ray = camera.GetRay(u, v, aspect);
                Vec3 color = Vec3{0.08f, 0.09f, 0.12f};

                HitRecord hit;
                if (bvh_root->Hit(ray, 0.001f, 1000.0f, hit))
                {
                    Vec3 view_dir = Normalize(-ray.direction);
                    color = ShadeHit(hit, view_dir, params, *bvh_root, rng);
                }
                else
                {
                    color = BackgroundColor(v);
                }

                size_t index = static_cast<size_t>(y * width + x);
                pixels[index] = Color{ToByte(color.x), ToByte(color.y), ToByte(color.z), 255};
            }
        }
    };

    std::vector<std::thread> workers;
    workers.reserve(thread_count);
    int y_start = 0;
    for (unsigned int i = 0; i < thread_count; ++i)
    {
        int y_end = (i == thread_count - 1) ? height : (y_start + rows_per_thread);
        workers.emplace_back(render_rows, y_start, y_end);
        y_start = y_end;
    }
    for (auto &worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}
