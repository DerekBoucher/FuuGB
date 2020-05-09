#ifdef FUUGB_DEBUG

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "Fuupch.h"
#include "Core.h"
#include "Gameboy.h"
#include <guisan.hpp>
#include <guisan/sdl.hpp>

#define DEBUG_WINX (160 * SCALE_FACTOR)
#define DEBUG_WINY 0
#define DEBUG_WINW (160 * SCALE_FACTOR)
#define DEBUG_WINH (144 * SCALE_FACTOR)

namespace FuuGB {

    class Debugger {

    public:
        Debugger();
        Debugger(SDL_Window*, Gameboy*);
        ~Debugger();

        void ProcessEvents(SDL_Event);
        void PerformLogic();
        void RenderGui();

    private:
        // Core Attributes
        SDL_Surface* screen;
        SDL_Window* win;
        gcn::SDLImageLoader* imgLoader;
        gcn::SDLGraphics* graphics;
        gcn::SDLInput* input;
        gcn::Gui* gui;
        gcn::ImageFont* font;

        // Widgets
        gcn::Container* top;
        gcn::Label* label;
        gcn::Button* buttn;
        gcn::CheckBox* checkbox;
        gcn::TextBox* memoryViewer;

        // Debugee
        Gameboy* gb;
    };

}

#endif

#endif