//
//  Application.cpp
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-19.
//

#include "Fuupch.h"
#include "Application.h"

namespace FuuGB
{
    Application::Application()
    {
        
    }
    
    Application::~Application()
    {
        
    }
    
    void Application::run()
    {
		Logger::init();
		FUUGB_CPU_LOG("Initialized Log!");
		FUUGB_MEM_LOG("Mem Log!");
		FUUGB_PPU_LOG("PPU Log!");
		FUUGB_SUBSYSTEM_LOG("Subsystem Log!");
        while(true);
    }
}
