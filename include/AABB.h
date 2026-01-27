#pragma once

#include <algorithm>

#include "Ray.h"
#include "Vec3.h"

struct AABB {
  Vec3 min;
  Vec3 max;

  bool Hit(const Ray3& ray, float t_min, float t_max) const {
    for (int axis = 0; axis < 3; ++axis) {
      float origin = ray.origin[axis];
      float dir = ray.direction[axis];
      if (dir == 0.0f) {
        if (origin < min[axis] || origin > max[axis]) {
          return false;
        }
        continue;
      }

      float inv_d = 1.0f / dir;
      float t0 = (min[axis] - origin) * inv_d;
      float t1 = (max[axis] - origin) * inv_d;
      if (inv_d < 0.0f) {
        std::swap(t0, t1);
      }
      t_min = std::max(t_min, t0);
      t_max = std::min(t_max, t1);
      if (t_max <= t_min) {
        return false;
      }
    }
    return true;
  }
};

inline AABB SurroundingBox(const AABB& a, const AABB& b) {
  return AABB{
      Vec3{std::min(a.min.x, b.min.x),
           std::min(a.min.y, b.min.y),
           std::min(a.min.z, b.min.z)},
      Vec3{std::max(a.max.x, b.max.x),
           std::max(a.max.y, b.max.y),
           std::max(a.max.z, b.max.z)},
  };
}
