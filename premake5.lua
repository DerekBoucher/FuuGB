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

        libdirs {}

        links {}

		includedirs
		{
			"FuuGBcore/headers"
		}

        filter "system:macosx"
            pchheader "headers/Fuupch.h"
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
            systemversion "latest"
            pchheader "Fuupch.h"
            pchsource "FuuGBcore/source/Fuupch.cpp"
            files
            {
                "%{prj.name}/**.h",
                "%{prj.name}/**.cpp"
            }
			defines
            {
                "FUUGB_SYSTEM_WINDOWS",
				"FUUGB_BUILD_DLL"
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

		filter "system:windows"
			systemversion "latest"
			defines
            {
                "FUUGB_SYSTEM_WINDOWS"
            }
			postbuildcommands
			{
				("copy ..\\bin\\" .. outputdir .. "\\FuuGBcore\\FuuGBcore.dll ..\\bin\\" .. outputdir .. "\\FuuSandbox\\FuuGBcore.dll")
			}