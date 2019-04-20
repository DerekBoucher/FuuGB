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
			"FuuGBcore/headers",
			"FuuGBcore/external/spdlog/include"
        }
        
        files
        {
            "%{prj.name}/headers/**.h",
            "%{prj.name}/source/**.cpp",
            "%{prj.name}/*.h"
        }

        filter "system:macosx"
            pchheader "headers/Fuupch.h"
            files
            {
                "%{prj.name}/source/**.m",
                "%{prj.name}/source/**.mm",
            }
            defines
            {
                "FUUGB_SYSTEM_MACOS"
            }

        filter "system:windows"
            systemversion "latest"
			staticruntime "On"
            pchheader "Fuupch.h"
            pchsource "FuuGBcore/source/Fuupch.cpp"
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
            "FuuGBcore",
			"FuuGBcore/external/spdlog/include"
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
            filter "configurations:Debug"
                postbuildcommands
                {
                    "{COPY} bin/Debug-macosx-x86/FuuGBcore/libFuuGBcore.dylib /usr/local/lib/libFuuGBcore.dylib"
                }
                filter "configurations:Release"
                postbuildcommands
                {
                    "{COPY} bin/Release-macosx-x86/FuuGBcore/libFuuGBcore.dylib /usr/local/lib/libFuuGBcore.dylib"
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