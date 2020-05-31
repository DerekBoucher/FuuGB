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
#define REG_VIEW_SIZE_Y 15
#define REG_ANCHOR_X 380
#define REG_ANCHOR_Y 20
#define MEM_VIEW_SIZE_X 327
#define MEM_VIEW_SIZE_Y 53240
#define MEM_VIEW_ANCHOR_X 20
#define MEM_VIEW_ANCHOR_Y 60
#define MEM_LABEL_X 20
#define MEM_LABEL_Y 20
#define CART_LABEL_ANCHOR_X 460
#define CART_LABEL_ANCHOR_Y 20
#define CART_VIEW_SIZE_X 300
#define CART_VIEW_SIZE_Y 15
#define CART_VIEW_ANCHOR_X 460
#define CART_VIEW_ANCHOR_Y 40
#define FLAG_LABEL_X 20
#define FLAG_LABEL_Y 460

namespace FuuGB {

    class Debugger : public gcn::ActionListener {

    public:
        Debugger();
        Debugger(SDL_Window*, Gameboy*);
        ~Debugger();

        void action(const gcn::ActionEvent&);

        void ProcessEvents(SDL_Event);
        void PerformLogic();
        void RenderGui();
        void ResetWindowPosition(int x, int y);
        void MinimizeWindow();
        void MaximizeWindow();
        void SetCartridgeName();
        void SetGbRef(Gameboy*);
        void UpdatePage();

        void OnClickLeftPageButton();
        void OnClickRightPageButton();

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
        gcn::Label* flagLabel;
        gcn::Label* zLabel;
        gcn::Label* nLabel;
        gcn::Label* hLabel;
        gcn::Label* cLabel;

        gcn::TextBox* memoryViewerTop;
        gcn::TextBox* memoryViewer;
        gcn::TextBox* pcViewer;
        gcn::TextBox* spViewer;
        gcn::TextBox* afViewer;
        gcn::TextBox* bcViewer;
        gcn::TextBox* deViewer;
        gcn::TextBox* hlViewer;
        gcn::TextBox* cartViewer;
        gcn::TextBox* zViewer;
        gcn::TextBox* nViewer;
        gcn::TextBox* hViewer;
        gcn::TextBox* cViewer;
        gcn::TextBox* pageViewer;

        gcn::Button* breakButton;
        gcn::Button* stepButton;
        gcn::Button* leftPageButton;
        gcn::Button* rightPageButton;

        unsigned int currentPage;
        
        // Debugee
        Gameboy* gb;
    };

}

#endif
#endif