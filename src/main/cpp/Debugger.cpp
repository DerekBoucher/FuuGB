#ifdef FUUGB_SYSTEM_WINDOWS
#ifdef FUUGB_DEBUG

#include "Debugger.h"

namespace FuuGB {

    Debugger::Debugger() {}

    Debugger::Debugger(SDL_Window* winRef, Gameboy* gbRef) {

        // Set Global Image Loader
        imgLoader   = new gcn::SDLImageLoader();
        gcn::Image::setImageLoader(imgLoader);

        // Instantiate Debugger Core Attributes
        win         = winRef;
        gb          = gbRef;
        screen      = SDL_GetWindowSurface(winRef);
        graphics    = new gcn::SDLGraphics();
        input       = new gcn::SDLInput();
        gui         = new gcn::Gui();
        font        = new gcn::ImageFont("fixedfont.bmp", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");


        // Instantiate Widgets
        top             = new gcn::Container();

        // Labels
        label           = new gcn::Label("Debugger");

        // Buttons
        buttn           = new gcn::Button("Click me!");
        checkbox        = new gcn::CheckBox("button", false);

        // Text Fields
        memoryViewer    = new gcn::TextBox();

        // Set the Global Font
        gcn::Widget::setGlobalFont(font);

        // Configure Core Attributes
        graphics->setTarget(screen);
        top->setDimension(gcn::Rectangle(DEBUG_WINX, DEBUG_WINY, 160 * SCALE_FACTOR, 144 * SCALE_FACTOR));
        gui->setGraphics(graphics);
        gui->setInput(input);
        gui->setTop(top);

        // Configure Widgets   
        label->adjustSize();
        label->setPosition(20, 20);
        memoryViewer->setPosition(20,50);

        // Add Widgets to top container
        top->add(label);
        top->add(memoryViewer);
    }

    Debugger::~Debugger() {}

    void Debugger::ProcessEvents(SDL_Event e) {
        input->pushInput(e);
    }

    void Debugger::PerformLogic() {
        gui->logic();
    }

    void Debugger::RenderGui() {
        gui->draw();
        SDL_UpdateWindowSurface(win);
    }
}

#endif
#endif