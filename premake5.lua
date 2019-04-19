-- Build Script for FuuGBemu

workspace "FuuGBemu"
    configurations { "Debug", "Release" }
    architecture "x86"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    project "FuuGBcore"
        location "FuuGBcore"
        kind "SharedLib"
        language "C++"
        cppdialect "C++17"
        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
        pchheader "headers/Fuupch.h"
        pchsource "source/Fuupch.cpp"

        libdirs {}

        links {}

        filter "system:macosx"
            files
            {
                "%{prj.name}/**.h",
                "%{prj.name}/**.cpp",
                "%{prj.name}/**.m",
                "%{prj.name}/**.mm",
            }

            defines
            {
                "FUUGB_SYSTEM_MACOS"
            }
        
        filter "system:windows"
            files
            {
                "%{prj.name}/**.h",
                "%{prj.name}/**.cpp"
            }
    
    project "FuuSandbox"
        location "FuuSandBox"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files
        {
            "%{prj.name}/**.h",
            "%{prj.name}/**.cpp"
        }

        includedirs
        {
            "FuuGBcore"
        }

        links
        {
            "FuuGBcore"
        }

        filter "system:macosx"
            defines
            {
                "FUUGB_SYSTEM_MACOS"
            }