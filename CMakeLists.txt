cmake_minimum_required(VERSION 3.31)
project(CPP_Engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BGFX_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/bgfx.cmake")
set(BGFX_DIR "${BGFX_CMAKE_DIR}/bgfx")
set(BGFX_BUILD_EXAMPLES OFF)

set(BGFX_CONFIG_SHADER_FOLDER_PATH "${CMAKE_SOURCE_DIR}/src/shaders")
option(USES_GLFW "Use GLFW for windowing and input" ON)

add_subdirectory("${BGFX_CMAKE_DIR}")
add_subdirectory(external/entt)

option(BUILD_FOR_X11 "Build for X11" OFF)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install")

bgfx_compile_shaders(
        TYPE VERTEX
        SHADERS src/shaders/cube_vert.sc
        VARYING_DEF ${CMAKE_SOURCE_DIR}/src/shaders/varying.def.sc
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/shaders
        INCLUDE_DIRS "${BGFX_DIR}/src"
)

bgfx_compile_shaders(
        TYPE FRAGMENT
        SHADERS src/shaders/cube_frag.sc
        VARYING_DEF ${CMAKE_SOURCE_DIR}/src/shaders/varying.def.sc
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/shaders
        INCLUDE_DIRS "${BGFX_DIR}/src"
)

add_executable(CPP_Engine
        # shaders
        src/shaders/cube_vert.sc
        src/shaders/cube_frag.sc

        src/gameobject.cpp
        src/gameobject.h
        src/components/component.h
        src/components/mesh_renderer.h
        src/misc/singleton.h
        src/application.h
        src/scene.h
        src/scene.cpp
        src/components/mesh_renderer.cpp
        src/components/transform.h
        src/types.h
        src/misc/unique_handle.h
        src/types.cpp
        src/application.cpp
        src/misc/utils.h
        src/misc/utils.cpp
        src/components/camera.h
        src/components/camera.cpp
        src/misc/service_locator.h
        src/input/mouse_keyboard_input.h
        src/input/mouse_keyboard_input.cpp

        src/commands/move_command.h
        src/commands/move_command.cpp
        src/components/entity.h
        src/components/entity.cpp
        src/components/transform.cpp
        src/components/player.h
        src/components/player.cpp
        src/commands/cam_adjust_command.h
        src/commands/cam_adjust_command.cpp

        src/include/math/vec.h
        src/api_internal/math/vec_utils.h
        src/api_internal/math/vec_utils.cpp
        src/misc/dirty.h
        src/include/math/quaternion.h
        src/api_internal/math/quaternion.cpp
        src/api_internal/math/quaternion_utils.h
        src/game.h
        src/engine.h
        src/engine.cpp
        src/game/game_main.cpp
        src/game/audio_rt_game.h
        src/game/audio_rt_game.cpp
        src/graphics/texture.h
        src/graphics/texture.cpp
        external/stb_image/stb_image.cpp
        src/graphics/mesh.h
        src/graphics/mesh.cpp
        src/scene_loaders/gltf_loader.h
        src/scene_loaders/gltf_loader.cpp
        src/texture_store.h
        src/texture_store.cpp
        src/include/math/matrix.h
        src/misc/spaced_span.h
        src/presentation/game_host.h
        src/presentation/game_bootstrap_info.h
        src/presentation/window.h
        src/misc/observer.h
)

if (USES_GLFW)
    if (BUILD_FOR_X11)
        target_compile_definitions(CPP_Engine PRIVATE BUILD_FOR_X11)
    endif ()
endif ()


if (EMSCRIPTEN)
  if (NOT DEFINED ENV{EMSDK})
    message(fatal_error "EMSDK is not in environment")
  endif()

  set(CXX_AND_LINKER_FLAGS "-fwasm-exceptions")
  target_compile_options(${PROJECT_NAME} PRIVATE ${CXX_AND_LINKER_FLAGS})
  target_link_options(${PROJECT_NAME} PRIVATE ${CXX_AND_LINKER_FLAGS} -sEXIT_RUNTIME=1)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(${PROJECT_NAME} PRIVATE "SHELL:-s SAFE_HEAP=1 -s ASSERTIONS=2 --profiling-funcs -gsource-map -s EXCEPTION_STACK_TRACES=1 -fsanitize=undefined")
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
  endif()
  target_link_options(${PROJECT_NAME} PRIVATE -sALLOW_MEMORY_GROWTH=1 --shell-file ${CMAKE_SOURCE_DIR}/src/platform_specific/host/emscripten/shell.html "SHELL:--embed-file \"${CMAKE_BINARY_DIR}/assets/crytech_sponza/@assets/crytech_sponza\" --embed-file \"${CMAKE_BINARY_DIR}/shaders@shaders\"")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set(CMAKE_TOOLCHAIN_FILE "$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" CACHE STRING "Emscripten toolchain file")
  target_include_directories(host_emscripten PUBLIC "$ENV{EMSDK}/upstream/emscripten/cache/sysroot/include")
elseif (USES_GLFW)
    add_subdirectory(external/glfw)
    target_sources(${PROJECT_NAME} PRIVATE
            src/platform_specific/host/glfw/glfw_window.cpp
            src/platform_specific/host/glfw/glfw_host.cpp
            src/platform_specific/host/glfw/glfw_input.h
            src/platform_specific/host/glfw/glfw_input.cpp
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE USES_GLFW)
    target_include_directories(${PROJECT_NAME} PRIVATE external/glfw/include)
    target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
endif ()

include(FetchContent)
FetchContent_Declare(
        fastgltf
        GIT_REPOSITORY https://github.com/spnda/fastgltf
        GIT_TAG v0.9.0
)
FetchContent_MakeAvailable(fastgltf)
add_dependencies(${PROJECT_NAME} fastgltf)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.9.1
)

FetchContent_MakeAvailable(Catch2)
target_link_libraries(${PROJECT_NAME} PRIVATE bx EnTT::EnTT bgfx fastgltf::fastgltf)

add_executable(tests
        src/tests/spaced_span.test.cpp
        src/tests/matrix.test.cpp
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE src src/include)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include/generated/shaders "${BGFX_DIR}/install/include" external/stb_image src src/include external/magic_enum)

set(ASSETS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ASSETS_DST "${CMAKE_CURRENT_BINARY_DIR}/assets")
set(HASH_FILE "${CMAKE_CURRENT_BINARY_DIR}/assets.sha256")
function(compute_assets_hash outvar)
    file(GLOB_RECURSE asset_files RELATIVE "${ASSETS_SRC}" "${ASSETS_SRC}/*")
    list(SORT asset_files) # Ensure deterministic ordering

    set(hash_list_file "${CMAKE_CURRENT_BINARY_DIR}/assets_hashes.txt")

    file(WRITE "${hash_list_file}" "") # Start fresh

    foreach (asset_file IN LISTS asset_files)
        file(SHA256 "${ASSETS_SRC}/${asset_file}" file_hash)
        file(APPEND "${hash_list_file}" "${file_hash}  ${asset_file}\n")
    endforeach ()

    # Hash the list to give a global hash
    file(SHA256 "${hash_list_file}" dir_hash)
    set(${outvar} "${dir_hash}" PARENT_SCOPE)
endfunction()

compute_assets_hash(CURRENT_HASH)
file(WRITE "${HASH_FILE}.new" "${CURRENT_HASH}")

add_custom_command(
        OUTPUT "${HASH_FILE}"
        COMMAND ${CMAKE_COMMAND}
        -DASSETS_SRC=${ASSETS_SRC}
        -DASSETS_DST=${ASSETS_DST}
        -DHASH_FILE=${HASH_FILE}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy-assets.cmake"
        DEPENDS "${HASH_FILE}.new"
        COMMENT "Copying assets if contents changed"
        VERBATIM
)
add_custom_target(assets ALL DEPENDS "${HASH_FILE}")

add_dependencies(${PROJECT_NAME} assets)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
install(TARGETS ${PROJECT_NAME} RUNTIME COMPONENT Runtime DESTINATION package)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders COMPONENT Runtime DESTINATION package)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets COMPONENT Runtime DESTINATION package)
