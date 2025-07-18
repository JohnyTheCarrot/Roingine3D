cmake_minimum_required(VERSION 3.31)
project(CPP_Engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/bgfx)
add_subdirectory(external/glfw)
add_subdirectory(external/entt)

add_executable(CPP_Engine src/main.cpp external/backward.cpp
        src/gameobject.cpp
        src/gameobject.h
        src/components/component.h
        src/setup.cpp
        src/setup.h
        src/components/mesh_renderer.h
        src/misc/singleton.h
        src/game.h
        src/scene.h
        src/scene.cpp
        src/components/mesh_renderer.cpp
        src/components/transform.h
        src/types.h
        src/misc/unique_handle.h
        src/types.cpp
        src/game.cpp
)

bgfx_compile_shaders(
        TYPE VERTEX
        SHADERS src/shaders/cube.vert.sc
        VARYING_DEF src/shaders/varying.def.sc
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
)

bgfx_compile_shaders(
        TYPE FRAGMENT
        SHADERS src/shaders/cube.frag.sc
        VARYING_DEF src/shaders/varying.def.sc
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE bgfx glfw EnTT::EnTT)
