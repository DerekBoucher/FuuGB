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
        int x, y;
        SDL_GetWindowPosition(winRef, &x, &y);
        win = SDL_CreateWindow("Debugger",
            x + DEBUG_WINX,
            y + DEBUG_WINY - 20,
            DEBUG_WINW,
            DEBUG_WINH,
            SDL_WINDOW_BORDERLESS
        );
        SDL_SetWindowResizable(win, SDL_FALSE);
        gb              = gbRef;
        screen          = SDL_GetWindowSurface(win);
        graphics        = new gcn::SDLGraphics();
        input           = new gcn::SDLInput();
        gui             = new gcn::Gui();
        font            = new gcn::ImageFont(
            "fixedfont.bmp",
            " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        );

        // Instantiate Widgets
        top             = new gcn::Container();

        // Labels
        memoryLabel = new gcn::Label("Memory");
        pcLabel     = new gcn::Label("PC");
        spLabel     = new gcn::Label("SP");
        afLabel     = new gcn::Label("AF");
        bcLabel     = new gcn::Label("BC");
        deLabel     = new gcn::Label("DE");
        hlLabel     = new gcn::Label("HL");
        cartLabel   = new gcn::Label("Cartridge");
        flagLabel   = new gcn::Label("Flags");
        zLabel      = new gcn::Label("Z");
        nLabel      = new gcn::Label("N");
        hLabel      = new gcn::Label("H");
        cLabel      = new gcn::Label("C");

        // Buttons
        breakButton     = new gcn::Button("Break Execution");
        stepButton      = new gcn::Button("Step Once");
        checkbox        = new gcn::CheckBox("button", false);

        // Text Fields
        memoryViewer    = new gcn::TextBox();
        memoryViewerTop = new gcn::TextBox();
        scrollMemView   = new gcn::ScrollArea(memoryViewer);
        pcViewer        = new gcn::TextBox();
        spViewer        = new gcn::TextBox();
        afViewer        = new gcn::TextBox();
        bcViewer        = new gcn::TextBox();
        deViewer        = new gcn::TextBox();
        hlViewer        = new gcn::TextBox();
        cartViewer      = new gcn::TextBox();
        zViewer         = new gcn::TextBox();
        nViewer         = new gcn::TextBox();
        hViewer         = new gcn::TextBox();
        cViewer         = new gcn::TextBox();

        // Set the Global Font
        gcn::Widget::setGlobalFont(font);

        // Configure Core Attributes
        graphics->setTarget(screen);
        top->setDimension(gcn::Rectangle(
            0,
            0, 
            160 * SCALE_FACTOR,
            144 * SCALE_FACTOR
        ));
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
        flagLabel->adjustSize();
        flagLabel->setPosition(FLAG_LABEL_X, FLAG_LABEL_Y);
        zLabel->adjustSize();
        zLabel->setPosition(FLAG_LABEL_X, FLAG_LABEL_Y + 20);
        nLabel->adjustSize();
        nLabel->setPosition(FLAG_LABEL_X + 120, FLAG_LABEL_Y + 20);
        hLabel->adjustSize();
        hLabel->setPosition(FLAG_LABEL_X, FLAG_LABEL_Y + 60);
        cLabel->adjustSize();
        cLabel->setPosition(FLAG_LABEL_X + 120, FLAG_LABEL_Y + 60);
        
        // Format the memory viewer
        memoryViewerTop->setSize(1000, 47);
        memoryViewerTop->setPosition(21, 46);
        memoryViewerTop->setTextRow(0, "0x     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
        memoryViewer->setTextRow(0, "0x0000 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
        char* buffer = new char[256];
        for (auto i = 0x10; i < 0x10000; i+=0x10) {
            sprintf(buffer, "0x%04X 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", i);
            memoryViewer->addRow(buffer);
        }
        delete buffer;
        memoryViewer->setSize(MEM_VIEW_SIZE_X, MEM_VIEW_SIZE_Y);
        memoryViewer->setPosition(MEM_VIEW_ANCHOR_X, MEM_VIEW_ANCHOR_Y);
        scrollMemView->setPosition(MEM_VIEW_ANCHOR_X, MEM_VIEW_ANCHOR_Y);
        scrollMemView->setSize(MEM_VIEW_SIZE_X, 380);
        scrollMemView->setScrollPolicy(gcn::ScrollArea::SHOW_NEVER, gcn::ScrollArea::SHOW_AUTO);

        pcViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        pcViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 20);
        pcViewer->setTextRow(0,"0x0000");
        spViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        spViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 80);
        spViewer->setTextRow(0,"0x0000");
        afViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        afViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 140);
        afViewer->setTextRow(0,"0x0000");
        bcViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        bcViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 200);
        bcViewer->setTextRow(0,"0x0000");
        deViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        deViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 260);
        deViewer->setTextRow(0,"0x0000");
        hlViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        hlViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 320);
        hlViewer->setTextRow(0,"0x0000");
        cartViewer->setSize(CART_VIEW_SIZE_X, CART_VIEW_SIZE_Y);
        cartViewer->setPosition(CART_VIEW_ANCHOR_X, CART_VIEW_ANCHOR_Y);
        cartViewer->setTextRow(0,"null");
        zViewer->setTextRow(0,"0");
        zViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        zViewer->setPosition(FLAG_LABEL_X + 20, FLAG_LABEL_Y + 25);
        nViewer->setTextRow(0,"0");
        nViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        nViewer->setPosition(FLAG_LABEL_X + 140, FLAG_LABEL_Y + 25);
        hViewer->setTextRow(0,"0");
        hViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        hViewer->setPosition(FLAG_LABEL_X + 20, FLAG_LABEL_Y + 65);
        cViewer->setTextRow(0,"0");
        cViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        cViewer->setPosition(FLAG_LABEL_X + 140, FLAG_LABEL_Y + 65);

        breakButton->setSize(140, 60);
        breakButton->setPosition(460, 80);
        stepButton->setSize(140, 60);
        stepButton->setPosition(460, 160);
        stepButton->setEnabled(false);
        stepButton->setBaseColor(gcn::Color(255, 255, 255, 100));

        // Add Widgets to top container
        top->add(memoryLabel);
        top->add(pcLabel);
        top->add(spLabel);
        top->add(afLabel);
        top->add(bcLabel);
        top->add(deLabel);
        top->add(hlLabel);
        top->add(cartLabel);
        top->add(flagLabel);
        top->add(zLabel);
        top->add(nLabel);
        top->add(hLabel);
        top->add(cLabel);
        top->add(memoryViewerTop);
        top->add(scrollMemView);
        top->add(pcViewer);
        top->add(spViewer);
        top->add(afViewer);
        top->add(bcViewer);
        top->add(deViewer);
        top->add(hlViewer);
        top->add(cartViewer);
        top->add(zViewer);
        top->add(nViewer);
        top->add(hViewer);
        top->add(cViewer);
        top->add(breakButton);
        top->add(stepButton);
    }

    Debugger::~Debugger() {
        delete hlViewer;
        delete deViewer;
        delete bcViewer;
        delete afViewer;
        delete spViewer;
        delete pcViewer;
        delete memoryViewer;
        delete zViewer;
        delete nViewer;
        delete hViewer;
        delete cViewer;
        delete hlLabel;
        delete deLabel;
        delete bcLabel;
        delete afLabel;
        delete spLabel;
        delete pcLabel;
        delete memoryLabel;
        delete flagLabel;
        delete zLabel;
        delete nLabel;
        delete hLabel;
        delete cLabel;
        delete breakButton;
        delete stepButton;
        delete scrollMemView;
        delete memoryViewerTop;
        SDL_FreeSurface(screen);
        SDL_DestroyWindow(win);
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

    void Debugger::ResetWindowPosition(int x, int y) {
        SDL_SetWindowPosition(win, x + DEBUG_WINX, y + DEBUG_WINY - 20);
    }

    void Debugger::MinimizeWindow() {
        SDL_MinimizeWindow(win);
    }

    void Debugger::MaximizeWindow() {
        SDL_RestoreWindow(win);
    }

    void Debugger::SetCartridgeName() {
        char* Buffer = new char[0x10];
        for (int i = 0x0; i < 0x10; i++) {
            Buffer[i] = gb->GetMemory()->DMA_read(0x134+i);
        }
        cartViewer->setTextRow(0, Buffer);
        delete Buffer;
    }

    void Debugger::SetGbRef(Gameboy* ref) {
        gb = ref;
    }
}

#endif
#endif