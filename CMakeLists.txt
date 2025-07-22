cmake_minimum_required(VERSION 3.31)
project(CPP_Engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/bgfx)
add_subdirectory(external/glfw)
add_subdirectory(external/entt)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install")

bgfx_compile_shaders(
        TYPE VERTEX
        SHADERS src/shaders/cube_vert.sc
        VARYING_DEF ${CMAKE_SOURCE_DIR}/src/shaders/varying.def.sc
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/shaders
        INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/bgfx/bgfx/src
)

bgfx_compile_shaders(
        TYPE FRAGMENT
        SHADERS src/shaders/cube_frag.sc
        VARYING_DEF ${CMAKE_SOURCE_DIR}/src/shaders/varying.def.sc
        OUTPUT_DIR ${CMAKE_BINARY_DIR}//shaders
        INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/bgfx/bgfx/src
)

add_executable(CPP_Engine src/main.cpp external/backward.cpp
        src/gameobject.cpp
        src/gameobject.h
        src/components/component.h
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
        src/shaders/cube_vert.sc
        src/shaders/cube_frag.sc
        src/misc/utils.h
        src/misc/utils.cpp
        src/components/camera.h
        src/components/camera.cpp
        src/commands/command.h
        src/misc/service_locator.h
        src/input/keyboard_input.h
        src/input/keyboard_input.cpp
        src/input/glfw_input.h
        src/input/glfw_input.cpp

        src/game/commands/move_command.h
        src/game/commands/move_command.cpp
        src/game/components/entity.h
        src/game/components/entity.cpp
        src/components/transform.cpp
        src/game/components/player.h
        src/game/components/player.cpp
        src/game/setup.cpp
        src/game/setup.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include/generated/shaders external/bgfx/install/include)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE bgfx bx glfw EnTT::EnTT)
