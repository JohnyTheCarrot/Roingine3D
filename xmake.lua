-- doesn't work at this stage bc xmake's packages don't work

add_rules("mode.debug", "mode.release")
add_requires("bgfx 8928", "entt v3.15.0")

-- glfw package doesn't work for wayland
package("glfw-local")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "external/glfw"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

add_requires("glfw-local")

target("engine")
    includes("shaders.lua")
    add_rules("shaders")
	set_default(true)
	set_kind(binary)
	add_files("src/*.cpp", "src/**/*.cpp")
	add_packages("glfw-local", "bgfx", "entt")
	set_languages("c++23")
    add_includedirs("src")

    add_files("src/shaders/*_frag.sc", {type = "fragment", output_dir = "shaders/spirv/"})
    add_files("src/shaders/*_vert.sc", {type = "vertex", output_dir = "shaders/spirv/"})
