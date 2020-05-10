#ifndef FuuGB_h
#define FuuGB_h

#ifdef FUUGB_SYSTEM_MACOS
    #include "MacApplication.h"
    #define FUUGB_APP FuuGB::MacApplication
#endif

#ifdef FUUGB_SYSTEM_WINDOWS
    #include "WinApplication.h"
    #define FUUGB_APP FuuGB::WinApplication
#endif

#ifdef FUUGB_SYSTEM_LINUX
    #include "LinuxApplication.h"
    #define FUUGB_APP FuuGB::LinuxApplication
#endif

#endif
