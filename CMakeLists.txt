cmake_minimum_required(VERSION 3.31)
project(CPP_Engine)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(external/bgfx)
add_subdirectory(external/glfw)

add_executable(CPP_Engine src/main.cpp external/backward.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE bgfx glfw)
