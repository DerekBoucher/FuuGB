#ifdef FUUGB_SYSTEM_WINDOWS
#ifdef FUUGB_DEBUG


#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <guisan.hpp>
#include <guisan/sdl.hpp>
#include "Gameboy.h"

#define DEBUG_WINX (160 * SCALE_FACTOR)
#define DEBUG_WINY 0
#define DEBUG_WINW (160 * SCALE_FACTOR)
#define DEBUG_WINH (144 * SCALE_FACTOR)
#define REG_VIEW_SIZE_X 60
#define REG_VIEW_SIZE_Y 20
#define REG_ANCHOR_X 240
#define REG_ANCHOR_Y 20
#define MEM_VIEW_SIZE_X 200
#define MEM_VIEW_SIZE_Y 400
#define MEM_VIEW_ANCHOR_X 20
#define MEM_VIEW_ANCHOR_Y 40
#define MEM_LABEL_X 20
#define MEM_LABEL_Y 20
#define CART_LABEL_ANCHOR_X 320
#define CART_LABEL_ANCHOR_Y 20
#define CART_VIEW_SIZE_X 60
#define CART_VIEW_SIZE_Y 20
#define CART_VIEW_ANCHOR_X 320
#define CART_VIEW_ANCHOR_Y 40

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
        gcn::Label* memoryLabel;
        gcn::Label* pcLabel;
        gcn::Label* spLabel;
        gcn::Label* afLabel;
        gcn::Label* bcLabel;
        gcn::Label* deLabel;
        gcn::Label* hlLabel;
        gcn::Label* cartLabel;
        gcn::Button* buttn;
        gcn::CheckBox* checkbox;
        gcn::TextBox* memoryViewer;
        gcn::TextBox* pcViewer;
        gcn::TextBox* spViewer;
        gcn::TextBox* afViewer;
        gcn::TextBox* bcViewer;
        gcn::TextBox* deViewer;
        gcn::TextBox* hlViewer;
        gcn::TextBox* cartViewer;
        
        // Debugee
        Gameboy* gb;
    };

}

#endif
#endif
#endif