#pragma once

#include <cmath>

#include "Hittable.h"
#include "Vec3.h"

struct Triangle : public Hittable {
  Vec3 v0;
  Vec3 v1;
  Vec3 v2;

  bool Hit(const Ray3& ray, float t_min, float t_max, HitRecord& out_hit) const override {
    const float kEpsilon = 1e-6f;
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 pvec = Cross(ray.direction, edge2);
    float det = Dot(edge1, pvec);
    if (std::abs(det) < kEpsilon) {
      return false;
    }
    float inv_det = 1.0f / det;
    Vec3 tvec = ray.origin - v0;
    float u = Dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) {
      return false;
    }
    Vec3 qvec = Cross(tvec, edge1);
    float v = Dot(ray.direction, qvec) * inv_det;
    if (v < 0.0f || (u + v) > 1.0f) {
      return false;
    }
    float t = Dot(edge2, qvec) * inv_det;
    if (t < t_min || t > t_max) {
      return false;
    }

    out_hit.t = t;
    out_hit.point = ray.At(t);
    out_hit.normal = Normalize(Cross(edge1, edge2));
    return true;
  }

  AABB Bounds() const override {
    Vec3 min_v{std::min(v0.x, std::min(v1.x, v2.x)),
               std::min(v0.y, std::min(v1.y, v2.y)),
               std::min(v0.z, std::min(v1.z, v2.z))};
    Vec3 max_v{std::max(v0.x, std::max(v1.x, v2.x)),
               std::max(v0.y, std::max(v1.y, v2.y)),
               std::max(v0.z, std::max(v1.z, v2.z))};
    return AABB{min_v, max_v};
  }

  Vec3 Centroid() const override {
    return (v0 + v1 + v2) / 3.0f;
  }
};
