# RayTracer Visualizer (C++20)

An interactive, multithreaded ray tracer for macOS with BVH acceleration, PBR-style shading, and a real-time ImGui control panel.

## Highlights
- Ray–sphere and ray–triangle intersections (Möller–Trumbore)
- BVH acceleration with axis-aligned bounding boxes (AABB)
- PBR-style shading (roughness/metallic + Schlick Fresnel)
- Soft shadows using area-light sampling
- Real-time UI controls via rlImGui
- Multithreaded CPU rendering for responsive iteration

## Tech Stack
- C++20, CMake
- Raylib (windowing + texture display)
- Dear ImGui (docking branch) + rlImGui

## Dependencies (macOS)
Install Raylib and CMake with Homebrew:
```
brew install cmake raylib
```

ImGui is fetched automatically (docking branch) unless you vendor it in `external/imgui`.

rlImGui is expected at `external/rlImGui` or fetched by CMake.

## Build & Run
```
cmake -S . -B build
cmake --build build
./build/raytracer
```

## Controls
- Orbit: right mouse button drag
- Zoom: mouse wheel
- UI panel: tweak light, material, and debug view

## Project Structure
- `src/main.cpp` — app loop + UI
- `include/` — math, ray objects, BVH, renderer
- `external/` — rlImGui (and optional ImGui)

## Notes
- If you vendor ImGui, it must be the **docking** branch.
- Use a lower shadow sample count if performance dips.
