#ifdef FUUGB_SYSTEM_LINUX

#ifndef LinuxApplication_h
#define LinuxApplication_h

#include "Application.h"
#include "Fuupch.h"
#include "System.h"
#include "Gameboy.h"
#include "FuuEvent.h"

namespace FuuGB
{
	class FUUGB_API LinuxApplication : public Application
	{
	public:
		void run();
	};
}

#endif /* LinuxApplication_h */

#endif