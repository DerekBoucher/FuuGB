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
        memoryLabel             = new gcn::Label("Memory");
        pcLabel                 = new gcn::Label("PC");
        spLabel                 = new gcn::Label("SP");
        afLabel                 = new gcn::Label("AF");
        bcLabel                 = new gcn::Label("BC");
        deLabel                 = new gcn::Label("DE");
        hlLabel                 = new gcn::Label("HL");
        cartLabel               = new gcn::Label("Cartridge");

        // Buttons
        buttn           = new gcn::Button("Click me!");
        checkbox        = new gcn::CheckBox("button", false);

        // Text Fields
        memoryViewer    = new gcn::TextBox();
        pcViewer    = new gcn::TextBox();
        spViewer    = new gcn::TextBox();
        afViewer    = new gcn::TextBox();
        bcViewer    = new gcn::TextBox();
        deViewer    = new gcn::TextBox();
        hlViewer    = new gcn::TextBox();
        cartViewer  = new gcn::TextBox();

        // Set the Global Font
        gcn::Widget::setGlobalFont(font);

        // Configure Core Attributes
        graphics->setTarget(screen);
        top->setDimension(gcn::Rectangle(DEBUG_WINX, DEBUG_WINY, 160 * SCALE_FACTOR, 144 * SCALE_FACTOR));
        gui->setGraphics(graphics);
        gui->setInput(input);
        gui->setTop(top);

        // Configure Widgets   
        memoryLabel->adjustSize();
        memoryLabel->setPosition(MEM_LABEL_X, MEM_LABEL_Y);
        pcLabel->adjustSize();
        pcLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 0);
        spLabel->adjustSize();
        spLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 60);
        afLabel->adjustSize();
        afLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 120);
        bcLabel->adjustSize();
        bcLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 180);
        deLabel->adjustSize();
        deLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 240);
        hlLabel->adjustSize();
        hlLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 300);
        cartLabel->adjustSize();
        cartLabel->setPosition(CART_LABEL_ANCHOR_X, CART_LABEL_ANCHOR_Y);


        memoryViewer->setSize(MEM_VIEW_SIZE_X, MEM_VIEW_SIZE_Y);
        memoryViewer->setPosition(MEM_VIEW_ANCHOR_X, MEM_VIEW_ANCHOR_Y);
        pcViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        pcViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 20);
        spViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        spViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 80);
        afViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        afViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 140);
        bcViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        bcViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 200);
        deViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        deViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 260);
        hlViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        hlViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 320);
        cartViewer->setSize(CART_VIEW_SIZE_X, CART_VIEW_SIZE_Y);
        cartViewer->setPosition(CART_VIEW_ANCHOR_X, CART_VIEW_ANCHOR_Y);

        // Add Widgets to top container
        top->add(memoryLabel);
        top->add(pcLabel);
        top->add(spLabel);
        top->add(afLabel);
        top->add(bcLabel);
        top->add(deLabel);
        top->add(hlLabel);
        top->add(cartLabel);
        top->add(memoryViewer);
        top->add(pcViewer);
        top->add(spViewer);
        top->add(afViewer);
        top->add(bcViewer);
        top->add(deViewer);
        top->add(hlViewer);
        top->add(cartViewer);
    }

    Debugger::~Debugger() {
        delete hlViewer;
        delete deViewer;
        delete bcViewer;
        delete afViewer;
        delete spViewer;
        delete pcViewer;
        delete memoryViewer;
        delete hlLabel;
        delete deLabel;
        delete bcLabel;
        delete afLabel;
        delete spLabel;
        delete pcLabel;
        delete memoryLabel;
    }

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