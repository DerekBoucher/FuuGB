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
            xcodebuildsettings { ["GCC_INPUT_FILETYPE"] = "sourcecode.cpp.objcpp" }
            pchheader "external/pch/Fuupch.h"
            defines
            {
                "FUUGB_SYSTEM_MACOS"
            }
            includedirs
            {
                "%{prj.name}/MacOS"
            }
            files
            {
                "%{prj.name}/MacOS/*.h",
                "%{prj.name}/MacOS/*.m",
                "%{prj.name}/MacOS/*.mm"

            }
            links
            {
                "SDL2.framework",
                "Cocoa.framework",
                "Foundation.framework",
                "Appkit.framework"
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
			files
			{
				"FuuGBcore/Windows/*.h",
				"FuuGBcore/Windows/*.cpp"
			}
			includedirs
			{
				"FuuGBcore/external/SDL2/Windows/SDL2-2.0.8/include"
			}
            links
			{
				"SDL2",
				"SDL2main"
			}
			libdirs
			{
				"FuuGBcore/external/SDL2/Windows/SDL2-2.0.8/lib/x86"
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
            "FuuGBcore/headers",
            "FuuGBcore/external/spdlog/include",
            "FuuGBcore/external/pch",
        }
        links
        {
            "FuuGBcore"
        }

        filter { "system:macosx" , "configurations:Debug" }
            xcodebuildsettings { ["GCC_INPUT_FILETYPE"] = "sourcecode.cpp.objcpp" }
            defines
            {
                "FUUGB_SYSTEM_MACOS"
            }
            includedirs
            {
                "FuuGBcore/MacOS"
            }
            buildoptions 
            {
				"-F ../FuuGBcore/external/SDL2/MacOS"
            }
            linkoptions 
            {
                "-F ../FuuGBcore/external/SDL2/MacOS"
            }
            links
            {
                "SDL2.framework"
            }
            postbuildcommands
            {
                "{COPY} ../bin/Debug-macosx-x86/FuuGBcore/libFuuGBcore.dylib /usr/local/lib/libFuuGBcore.dylib",
                "{COPY} ../FuuGBcore/external/SDL2/MacOS/SDL2.framework ../bin/Debug-macosx-x86/FuuSandbox/SDL2.framework",
                "{COPY} ../FuuGBcore/external/boot/DMG_ROM.bin ../bin/Debug-macosx-x86/FuuSandbox/DMG_ROM.bin"
            }

        filter { "system:macosx" , "configurations:Release" }
			xcodebuildsettings { ["GCC_INPUT_FILETYPE"] = "sourcecode.cpp.objcpp" }
            defines
            {
                "FUUGB_SYSTEM_MACOS"
            }
            includedirs
            {
                "FuuGBcore/MacOS"
            }
            buildoptions 
            {
				"-F ../FuuGBcore/external/SDL2/MacOS"
            }
            linkoptions 
            {
                "-F ../FuuGBcore/external/SDL2/MacOS"
            }
            links
            {
                "SDL2.framework"
            }
            postbuildcommands
            {
                "{COPY} bin/Release-macosx-x86/FuuGBcore/libFuuGBcore.dylib /usr/local/lib/libFuuGBcore.dylib"
            }
            postbuildcommands
            {
                "{COPY} bin/Debug-macosx-x86/FuuGBcore/libFuuGBcore.dylib /usr/local/lib/libFuuGBcore.dylib",
                "{COPY} FuuGBcore/external/SDL2/MacOS/SDL2.framework bin/Release-macos-x86/FuuSandbox/SDL2.framework"
            }

		filter "system:windows"
			systemversion "latest"
			defines
            {
                "FUUGB_SYSTEM_WINDOWS",
                "SDL_MAIN_HANDLED"
            }
			postbuildcommands
			{
				("copy ..\\bin\\" .. outputdir .. "\\FuuGBcore\\FuuGBcore.dll ..\\bin\\" .. outputdir .. "\\FuuSandbox\\FuuGBcore.dll"),
				("copy ..\\FuuGBcore\\external\\SDL2\\Windows\\SDL2-2.0.8\\lib\\x86\\SDL2.dll ..\\bin\\" .. outputdir .. "\\FuuSandbox\\SDL2.dll")
			}
			includedirs
			{
				"FuuGBcore/external/SDL2/Windows/SDL2-2.0.8/include",
				"FuuGBcore/Windows"
			}
			cleanextensions
			{
				".dll"
			}