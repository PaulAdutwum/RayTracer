#pragma once

#include <memory>

#include "AABB.h"
#include "Ray.h"
#include "Vec3.h"

struct HitRecord {
  float t = 0.0f;
  Vec3 point;
  Vec3 normal;
};

struct Hittable {
  virtual ~Hittable() = default;
  virtual bool Hit(const Ray3& ray, float t_min, float t_max, HitRecord& out_hit) const = 0;
  virtual AABB Bounds() const = 0;
  virtual Vec3 Centroid() const = 0;
};

using HittablePtr = std::shared_ptr<Hittable>;
