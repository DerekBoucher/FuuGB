//
//  LinuxApplication.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-20.
//

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