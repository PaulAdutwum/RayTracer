cmake_minimum_required(VERSION 3.25)

project(RayTracerVisualizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer Apple Silicon on macOS.
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
  set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)
endif()

include(FetchContent)
include(CheckCXXCompilerFlag)

# Raylib (Homebrew: raylib)
find_package(raylib CONFIG QUIET)
if(NOT raylib_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(RAYLIB REQUIRED raylib)
endif()

# ImGui (prefer installed package, otherwise use local or fetch from source)
find_package(imgui CONFIG QUIET)
if(NOT imgui_FOUND)
  set(IMGUI_LOCAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
  if(EXISTS ${IMGUI_LOCAL_DIR}/imgui.cpp)
    add_library(imgui STATIC
      ${IMGUI_LOCAL_DIR}/imgui.cpp
      ${IMGUI_LOCAL_DIR}/imgui_draw.cpp
      ${IMGUI_LOCAL_DIR}/imgui_tables.cpp
      ${IMGUI_LOCAL_DIR}/imgui_widgets.cpp
    )
    target_include_directories(imgui PUBLIC ${IMGUI_LOCAL_DIR})
    file(READ ${IMGUI_LOCAL_DIR}/imgui.h IMGUI_HEADER)
    string(FIND "${IMGUI_HEADER}" "GetPlatformIO" IMGUI_HAS_PLATFORM_IO)
    string(FIND "${IMGUI_HEADER}" "ImTextureData" IMGUI_HAS_TEXTURE_DATA)
    if(IMGUI_HAS_PLATFORM_IO EQUAL -1 OR IMGUI_HAS_TEXTURE_DATA EQUAL -1)
      message(FATAL_ERROR "rlImGui requires Dear ImGui docking branch (with GetPlatformIO/ImTextureData). Please checkout the docking branch in external/imgui.")
    endif()
  else()
    FetchContent_Declare(
      imgui
      GIT_REPOSITORY https://github.com/ocornut/imgui.git
      GIT_TAG docking
    )
    FetchContent_GetProperties(imgui)
    if(NOT imgui_POPULATED)
      FetchContent_Populate(imgui)
      add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
      )
      target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
      file(READ ${imgui_SOURCE_DIR}/imgui.h IMGUI_HEADER)
      string(FIND "${IMGUI_HEADER}" "GetPlatformIO" IMGUI_HAS_PLATFORM_IO)
      string(FIND "${IMGUI_HEADER}" "ImTextureData" IMGUI_HAS_TEXTURE_DATA)
      if(IMGUI_HAS_PLATFORM_IO EQUAL -1 OR IMGUI_HAS_TEXTURE_DATA EQUAL -1)
        message(FATAL_ERROR "Fetched ImGui missing docking features required by rlImGui. Try clearing build/ and reconfigure.")
      endif()
    endif()
  endif()
endif()

if(imgui_FOUND)
  set(IMGUI_TARGET imgui::imgui)
else()
  set(IMGUI_TARGET imgui)
endif()

# rlImGui bridge (prefer local checkout, otherwise fetch)
set(RLIMGUI_LOCAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/rlImGui)
if(EXISTS ${RLIMGUI_LOCAL_DIR}/rlImGui.cpp)
  add_library(rlimgui STATIC
    ${RLIMGUI_LOCAL_DIR}/rlImGui.cpp
  )
  target_include_directories(rlimgui PUBLIC ${RLIMGUI_LOCAL_DIR})
else()
  FetchContent_Declare(
    rlimgui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG v5.0
  )
  FetchContent_GetProperties(rlimgui)
  if(NOT rlimgui_POPULATED)
    FetchContent_Populate(rlimgui)
    add_library(rlimgui STATIC
      ${rlimgui_SOURCE_DIR}/rlImGui.cpp
    )
    target_include_directories(rlimgui PUBLIC ${rlimgui_SOURCE_DIR})
  endif()
endif()

if(raylib_FOUND)
  target_link_libraries(rlimgui PRIVATE raylib ${IMGUI_TARGET})
else()
  target_link_libraries(rlimgui PRIVATE ${RAYLIB_LIBRARIES} ${IMGUI_TARGET})
  target_include_directories(rlimgui PRIVATE ${RAYLIB_INCLUDE_DIRS})
endif()

add_executable(raytracer
  src/main.cpp
)

target_include_directories(raytracer
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(raylib_FOUND)
  target_link_libraries(raytracer PRIVATE raylib)
else()
  target_include_directories(raytracer PRIVATE ${RAYLIB_INCLUDE_DIRS})
  target_link_libraries(raytracer PRIVATE ${RAYLIB_LIBRARIES})
endif()

target_link_libraries(raytracer PRIVATE ${IMGUI_TARGET})

target_link_libraries(raytracer PRIVATE rlimgui)

if(APPLE)
  target_link_libraries(raytracer PRIVATE "-framework OpenGL" "-framework Cocoa" "-framework IOKit")
endif()
