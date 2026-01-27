#include "raylib.h"

#include "imgui.h"
#include "rlImGui.h"

#include <algorithm>
#include <thread>
#include <vector>

#include "Camera.h"
#include "Renderer.h"
#include "Sphere.h"
#include "Vec3.h"

int main()
{
    int screen_width = 1280;
    int screen_height = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "Ray Tracer Visualizer");
    SetTargetFPS(60);

    rlImGuiSetup(true);
    ImGui::StyleColorsDark();

    RenderTexture2D render_target = LoadRenderTexture(screen_width, screen_height);
    Image cpu_image = GenImageColor(screen_width, screen_height, BLACK);
    Texture2D cpu_texture = LoadTextureFromImage(cpu_image);
    UnloadImage(cpu_image);

    std::vector<Color> pixels;
    pixels.resize(static_cast<size_t>(screen_width * screen_height));

    OrbitCamera camera;
    camera.yaw = camera.yaw_target = 0.6f;
    camera.pitch = camera.pitch_target = 0.2f;
    camera.distance = camera.distance_target = 6.0f;

    RenderParams params;
    params.sphere.center = Vec3{0.0f, 0.0f, 0.0f};
    params.sphere.radius = 1.5f;
    params.light_position = Vec3{3.5f, 4.0f, 2.0f};
    params.light_radius = 1.0f;
    params.light_intensity = 3.0f;
    params.shadow_samples = 8;
    params.albedo = Vec3{0.9f, 0.35f, 0.25f};
    params.roughness = 0.35f;
    params.metallic = 0.05f;
    params.debug_normals = false;

    unsigned int thread_count = std::max(1u, std::thread::hardware_concurrency());

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 delta = GetMouseDelta();
            camera.yaw_target += delta.x * 0.01f;
            camera.pitch_target -= delta.y * 0.01f;
        }
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f)
        {
            camera.distance_target -= wheel * 0.6f;
        }
        camera.ClampTargets();
        camera.SmoothStep(dt);

        if (IsWindowResized())
        {
            screen_width = GetScreenWidth();
            screen_height = GetScreenHeight();
            UnloadTexture(cpu_texture);
            UnloadRenderTexture(render_target);
            render_target = LoadRenderTexture(screen_width, screen_height);
            cpu_image = GenImageColor(screen_width, screen_height, BLACK);
            cpu_texture = LoadTextureFromImage(cpu_image);
            UnloadImage(cpu_image);
            pixels.resize(static_cast<size_t>(screen_width * screen_height));
        }

        RenderScene(pixels, screen_width, screen_height, camera, params, thread_count);
        UpdateTexture(cpu_texture, pixels.data());

        BeginTextureMode(render_target);
        ClearBackground(BLACK);
        DrawTexture(cpu_texture, 0, 0, WHITE);
        EndTextureMode();

        BeginDrawing();
        ClearBackground(Color{18, 18, 18, 255});

        Rectangle src{0.0f, 0.0f, static_cast<float>(screen_width),
                      -static_cast<float>(screen_height)};
        DrawTextureRec(render_target.texture, src, Vector2{0.0f, 0.0f}, WHITE);

        rlImGuiBegin();
        ImGui::Begin("Ray Tracer Controls");
        ImGui::Text("Resolution: %dx%d", screen_width, screen_height);
        ImGui::Text("Threads: %u", thread_count);
        ImGui::Separator();
        ImGui::Text("Sphere");
        ImGui::SliderFloat3("Position", &params.sphere.center.x, -4.0f, 4.0f);
        ImGui::SliderFloat("Radius", &params.sphere.radius, 0.5f, 3.0f);
        ImGui::Separator();
        ImGui::Text("Light");
        ImGui::SliderFloat3("Light Pos", &params.light_position.x, -8.0f, 8.0f);
        ImGui::SliderFloat("Light Radius", &params.light_radius, 0.0f, 3.0f);
        ImGui::SliderFloat("Intensity", &params.light_intensity, 0.2f, 8.0f);
        ImGui::SliderInt("Shadow Samples", &params.shadow_samples, 1, 32);
        ImGui::Separator();
        ImGui::Text("Material");
        ImGui::ColorEdit3("Albedo", &params.albedo.x);
        ImGui::SliderFloat("Roughness", &params.roughness, 0.0f, 1.0f);
        ImGui::SliderFloat("Metallic", &params.metallic, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::Checkbox("Debug Normals", &params.debug_normals);
        ImGui::Text("Orbit: RMB drag, Zoom: mouse wheel");
        ImGui::Text("FPS: %.0f", 1.0f / std::max(0.0001f, dt));
        ImGui::End();
        rlImGuiEnd();

        EndDrawing();
    }

    UnloadTexture(cpu_texture);
    UnloadRenderTexture(render_target);
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
