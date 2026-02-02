#pragma once

#include <vector>

#include "raylib.h"

#include "Triangle.h"

inline bool LoadObjAsTriangles(const char *path,
                               const Vec3 &offset,
                               float scale,
                               std::vector<HittablePtr> &out_objects)
{
    if (!FileExists(path))
    {
        return false;
    }

    Model model = LoadModel(path);
    if (model.meshCount <= 0)
    {
        UnloadModel(model);
        return false;
    }

    for (int mesh_index = 0; mesh_index < model.meshCount; ++mesh_index)
    {
        const Mesh &mesh = model.meshes[mesh_index];
        if (mesh.vertexCount <= 0)
        {
            continue;
        }

        const float *vertices = mesh.vertices;
        const unsigned short *indices = mesh.indices;
        int tri_count = mesh.triangleCount;
        if (tri_count <= 0)
        {
            tri_count = mesh.vertexCount / 3;
        }

        for (int tri = 0; tri < tri_count; ++tri)
        {
            int i0 = indices ? indices[tri * 3 + 0] : (tri * 3 + 0);
            int i1 = indices ? indices[tri * 3 + 1] : (tri * 3 + 1);
            int i2 = indices ? indices[tri * 3 + 2] : (tri * 3 + 2);

            Vec3 v0{vertices[i0 * 3 + 0], vertices[i0 * 3 + 1], vertices[i0 * 3 + 2]};
            Vec3 v1{vertices[i1 * 3 + 0], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]};
            Vec3 v2{vertices[i2 * 3 + 0], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]};

            v0 = v0 * scale + offset;
            v1 = v1 * scale + offset;
            v2 = v2 * scale + offset;

            auto tri_obj = std::make_shared<Triangle>();
            tri_obj->v0 = v0;
            tri_obj->v1 = v1;
            tri_obj->v2 = v2;
            out_objects.push_back(tri_obj);
        }
    }

    UnloadModel(model);
    return true;
}
