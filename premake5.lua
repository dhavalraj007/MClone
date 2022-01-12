workspace "MClone"
    startproject "MClone"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

tdir = "bin/%{cfg.buildcfg}/%{prj.name}"
odir = "bin-obj/%{cfg.buildcfg}/%{prj.name}"
-- External Dependencies
externals = {}
externals["sdl2"] = "external/sdl2"
externals["spdlog"] = "external/spdlog"
externals["glad"] = "external/glad"
externals["glm"] = "external/glm"
externals["stb"] = "external/stb"

-- process glad before anything else
include "external/glad"

project "MClone"
    location "MClone"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    flags
    {
        "FatalWarnings"
    }

    targetdir(tdir)
    objdir(odir)
    
    sysincludedirs
    {
        "%{prj.name}/include",
        "%{externals.sdl2}/include",
        "%{externals.spdlog}/include",
        "%{externals.glad}/include",
        "%{externals.glm}/include",
        "%{externals.stb}/include"
    }

    libdirs
    {
        "%{externals.sdl2}/lib",
    }

    links
    {
        "glad",
        "SDL2.lib"
    }

    files
    {
        "%{prj.name}/include/**.h",    
        "%{prj.name}/include/**.cpp",   
        "%{prj.name}/include/**.hpp",   
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.glsl",
        "%{prj.name}/**.natvis"
    }
    
    defines
    {
        "GLFW_INCLUDE_NONE" -- ensure glad doesn't include glfw
    }
    systemversion "latest"
    
    filter {"configurations:Debug"}
        
        defines
        {
            "MCLONE_CONFIG_DEBUG"
        }
        runtime "Debug"
        symbols "on"
    
    filter {"configurations:Release"}
        
        defines
        {
            "MCLONE_CONFIG_RELEASE"
        }
        runtime "Release"
        symbols "off"
        optimize "on"
