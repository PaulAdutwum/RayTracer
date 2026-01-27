#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "AABB.h"
#include "Hittable.h"

struct BVHNode : public Hittable {
  HittablePtr left;
  HittablePtr right;
  AABB bounds;

  BVHNode(HittablePtr left_node, HittablePtr right_node)
      : left(std::move(left_node)), right(std::move(right_node)) {
    bounds = SurroundingBox(left->Bounds(), right->Bounds());
  }

  bool Hit(const Ray3& ray, float t_min, float t_max, HitRecord& out_hit) const override {
    if (!bounds.Hit(ray, t_min, t_max)) {
      return false;
    }

    HitRecord left_hit;
    HitRecord right_hit;
    bool hit_left = left->Hit(ray, t_min, t_max, left_hit);
    bool hit_right = right->Hit(ray, t_min, t_max, right_hit);

    if (hit_left && hit_right) {
      if (left_hit.t <= right_hit.t) {
        out_hit = left_hit;
      } else {
        out_hit = right_hit;
      }
      return true;
    }

    if (hit_left) {
      out_hit = left_hit;
      return true;
    }

    if (hit_right) {
      out_hit = right_hit;
      return true;
    }

    return false;
  }

  AABB Bounds() const override {
    return bounds;
  }

  Vec3 Centroid() const override {
    return (bounds.min + bounds.max) * 0.5f;
  }
};

inline int LongestAxis(const AABB& box) {
  Vec3 extent = box.max - box.min;
  if (extent.x > extent.y && extent.x > extent.z) {
    return 0;
  }
  if (extent.y > extent.z) {
    return 1;
  }
  return 2;
}

inline HittablePtr BuildBVH(std::vector<HittablePtr>& objects, size_t start, size_t end) {
  size_t count = end - start;
  if (count == 1) {
    return objects[start];
  }
  if (count == 2) {
    return std::make_shared<BVHNode>(objects[start], objects[start + 1]);
  }

  AABB centroid_bounds = objects[start]->Bounds();
  Vec3 c0 = objects[start]->Centroid();
  centroid_bounds.min = c0;
  centroid_bounds.max = c0;
  for (size_t i = start + 1; i < end; ++i) {
    Vec3 c = objects[i]->Centroid();
    centroid_bounds.min.x = std::min(centroid_bounds.min.x, c.x);
    centroid_bounds.min.y = std::min(centroid_bounds.min.y, c.y);
    centroid_bounds.min.z = std::min(centroid_bounds.min.z, c.z);
    centroid_bounds.max.x = std::max(centroid_bounds.max.x, c.x);
    centroid_bounds.max.y = std::max(centroid_bounds.max.y, c.y);
    centroid_bounds.max.z = std::max(centroid_bounds.max.z, c.z);
  }

  int axis = LongestAxis(centroid_bounds);
  auto begin_it = objects.begin() + static_cast<long>(start);
  auto end_it = objects.begin() + static_cast<long>(end);
  std::sort(begin_it, end_it, [axis](const HittablePtr& a, const HittablePtr& b) {
    return a->Centroid()[axis] < b->Centroid()[axis];
  });

  size_t mid = start + count / 2;
  HittablePtr left = BuildBVH(objects, start, mid);
  HittablePtr right = BuildBVH(objects, mid, end);
  return std::make_shared<BVHNode>(left, right);
}
