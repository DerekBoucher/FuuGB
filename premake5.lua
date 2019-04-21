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

		includedirs
		{
			"FuuGBcore/headers",
			"FuuGBcore/external/spdlog/include",
			"FuuGBcore/external/pch"
        }
        
        files
        {
            "%{prj.name}/headers/**.h",
            "%{prj.name}/source/**.cpp",
            "%{prj.name}/*.h",
			"%{prj.name}/external/pch/*.h",
			"%{prj.name}/external/pch/*.cpp"
        }

        filter "system:macosx"
            pchheader "external/pch/Fuupch.h"
            defines
            {
                "FUUGB_SYSTEM_MACOS"
            }
            links
            {
                "SDL2.framework"
            }
            buildoptions 
            {
                "-F %{prj.location}/external/SDL2/MacOS"
            }
            linkoptions 
            {
                "-F %{prj.location}/external/SDL2/MacOS"
            }

        filter "system:windows"
            systemversion "latest"
			staticruntime "On"
            pchheader "Fuupch.h"
            pchsource "FuuGBcore/external/pch/Fuupch.cpp"
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