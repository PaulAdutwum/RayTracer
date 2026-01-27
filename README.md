# RayTracer Visualizer (C++20)

Interactive, multithreaded ray tracer with BVH acceleration, PBR-style shading, and an ImGui control panel.

## Features
- Ray–sphere and ray–triangle intersection
- BVH acceleration with AABBs
- PBR-style shading (roughness/metallic + Fresnel)
- Soft shadows via area light sampling
- Real-time UI controls (rlImGui)
- Multithreaded CPU rendering

## Dependencies (macOS)
- CMake
- Raylib (Homebrew)
- ImGui (docking branch, fetched automatically unless vendored)
- rlImGui (local clone in `external/rlImGui`)

Install Raylib:
```
brew install cmake raylib
```

## Build & Run
```
cmake -S . -B build
cmake --build build
./build/raytracer
```

## Notes
- If you vendored ImGui, it must be the **docking** branch.
- `external/rlImGui` is expected to exist locally or be fetched by CMake.
