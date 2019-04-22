# FuuGBemu
Cross platform C/C++ GameBoy and GameBoy Color emulator using SDL2 framework.

Early Alpha Development

Build Script System provided by Premake5 : https://github.com/premake/premake-core

#How to build:

    1.  Clone Repository into arbitrary directory.
    2.  Open the command line or Terminal from the DIRECTORY of the CLONED repository.
    
    MAC OS X
    **Only supports building with Xcode.
        3_1. From the Terminal, enter "premake5 xcode4".
        3_2. Open the generated .xcodeproj file.
        3_3. Build the project in Xcode.

    WINDOWS:
    **Only supports building with VS 2013, 2015, 2017 and 2019.
        3_4. From the Command Line, enter "premake5 vs20xx". (where xx = 13, 15, 17 or 19)
        3_5. Open the generated .sln file.
        3_6. Build the project in Visual Studio.
        
        **NOTE:  Clean solution prior to rebuilding, or else some copied dll's will not be updated to the new build version. (Only applicable for Windows)
