#pragma once

#include "Hittable.h"

struct Sphere : public Hittable
{
    Vec3 center;
    float radius = 1.0f;

    bool Hit(const Ray3 &ray, float t_min, float t_max, HitRecord &out_hit) const override
    {
        Vec3 oc = ray.origin - center;
        float a = Dot(ray.direction, ray.direction);
        float half_b = Dot(oc, ray.direction);
        float c = Dot(oc, oc) - radius * radius;
        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0.0f)
        {
            return false;
        }

        float sqrt_disc = std::sqrt(discriminant);
        float root = (-half_b - sqrt_disc) / a;
        if (root < t_min || root > t_max)
        {
            root = (-half_b + sqrt_disc) / a;
            if (root < t_min || root > t_max)
            {
                return false;
            }
        }

        out_hit.t = root;
        out_hit.point = ray.At(root);
        out_hit.normal = Normalize(out_hit.point - center);
        return true;
    }

    AABB Bounds() const override
    {
        Vec3 r{radius, radius, radius};
        return AABB{center - r, center + r};
    }

    Vec3 Centroid() const override
    {
        return center;
    }
};
