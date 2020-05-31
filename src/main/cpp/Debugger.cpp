#ifdef FUUGB_DEBUG

#include "Debugger.h"

namespace FuuGB {

    Debugger::Debugger() {}

    Debugger::Debugger(SDL_Window* winRef, Gameboy* gbRef) : gcn::ActionListener(){

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
        pcLabel         = new gcn::Label("PC");
        spLabel         = new gcn::Label("SP");
        afLabel         = new gcn::Label("AF");
        bcLabel         = new gcn::Label("BC");
        deLabel         = new gcn::Label("DE");
        hlLabel         = new gcn::Label("HL");
        zLabel          = new gcn::Label("Z");
        nLabel          = new gcn::Label("N");
        hLabel          = new gcn::Label("H");
        cLabel          = new gcn::Label("C");
        memoryLabel     = new gcn::Label("Memory");
        cartLabel       = new gcn::Label("Cartridge");
        flagLabel       = new gcn::Label("Flags");

        // Buttons
        breakButton     = new gcn::Button("Break Execution");
        stepButton      = new gcn::Button("Step Once");
        leftPageButton  = new gcn::Button("L");
        rightPageButton = new gcn::Button("R");

        // Checkboxes
        breakBootRom    = new gcn::CheckBox("Break when boot rom exits");

        // Text Fields
        pcViewer        = new gcn::TextBox();
        spViewer        = new gcn::TextBox();
        afViewer        = new gcn::TextBox();
        bcViewer        = new gcn::TextBox();
        deViewer        = new gcn::TextBox();
        hlViewer        = new gcn::TextBox();
        zViewer         = new gcn::TextBox();
        nViewer         = new gcn::TextBox();
        hViewer         = new gcn::TextBox();
        cViewer         = new gcn::TextBox();
        memoryViewerTop = new gcn::TextBox();
        memoryViewer    = new gcn::TextBox();
        cartViewer      = new gcn::TextBox();
        pageViewer      = new gcn::TextBox();

        // Attach to debugger action listener
        leftPageButton->addActionListener(this);
        rightPageButton->addActionListener(this);
        breakButton->addActionListener(this);
        stepButton->addActionListener(this);
        breakBootRom->addActionListener(this);

        // Set Action Event ID's
        leftPageButton->setActionEventId("0");
        rightPageButton->setActionEventId("1");
        breakButton->setActionEventId("2");
        stepButton->setActionEventId("3");
        breakBootRom->setActionEventId("4");

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
        pcLabel->adjustSize();
        spLabel->adjustSize();
        afLabel->adjustSize();
        bcLabel->adjustSize();
        deLabel->adjustSize();
        hlLabel->adjustSize();
        zLabel->adjustSize();
        nLabel->adjustSize();
        hLabel->adjustSize();
        cLabel->adjustSize();
        memoryLabel->adjustSize();
        cartLabel->adjustSize();
        flagLabel->adjustSize();
        breakBootRom->adjustSize();

        pcLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 0);
        spLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 60);
        afLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 120);
        bcLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 180);
        deLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 240);
        hlLabel->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 300);
        zLabel->setPosition(FLAG_LABEL_X, FLAG_LABEL_Y + 20);
        nLabel->setPosition(FLAG_LABEL_X + 120, FLAG_LABEL_Y + 20);
        hLabel->setPosition(FLAG_LABEL_X, FLAG_LABEL_Y + 60);
        cLabel->setPosition(FLAG_LABEL_X + 120, FLAG_LABEL_Y + 60);
        memoryLabel->setPosition(MEM_LABEL_X, MEM_LABEL_Y);
        cartLabel->setPosition(CART_LABEL_ANCHOR_X, CART_LABEL_ANCHOR_Y);
        flagLabel->setPosition(FLAG_LABEL_X, FLAG_LABEL_Y);
        
        // Format the memory viewer
        currentPage = 1;

        pcViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        spViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        afViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        bcViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        deViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        hlViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        zViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        nViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        hViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        cViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);
        memoryViewerTop->setSize(1000, 47);
        memoryViewer->setSize(326, 366);
        cartViewer->setSize(CART_VIEW_SIZE_X, CART_VIEW_SIZE_Y);
        pageViewer->setSize(REG_VIEW_SIZE_X, REG_VIEW_SIZE_Y);

        pcViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 20);
        spViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 80);
        afViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 140);
        bcViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 200);
        deViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 260);
        hlViewer->setPosition(REG_ANCHOR_X, REG_ANCHOR_Y + 320);
        zViewer->setPosition(FLAG_LABEL_X + 20, FLAG_LABEL_Y + 25);
        nViewer->setPosition(FLAG_LABEL_X + 140, FLAG_LABEL_Y + 25);
        hViewer->setPosition(FLAG_LABEL_X + 20, FLAG_LABEL_Y + 65);
        cViewer->setPosition(FLAG_LABEL_X + 140, FLAG_LABEL_Y + 65);
        memoryViewerTop->setPosition(20, 46);
        memoryViewer->setPosition(MEM_VIEW_ANCHOR_X, MEM_VIEW_ANCHOR_Y);
        cartViewer->setPosition(CART_VIEW_ANCHOR_X, CART_VIEW_ANCHOR_Y);
        pageViewer->setPosition(180, 440);

        pcViewer->setTextRow(0,"0x0000");
        spViewer->setTextRow(0,"0x0000");
        afViewer->setTextRow(0,"0x0000");
        bcViewer->setTextRow(0,"0x0000");
        deViewer->setTextRow(0,"0x0000");
        hlViewer->setTextRow(0,"0x0000");
        zViewer->setTextRow(0,"0");
        nViewer->setTextRow(0,"0");
        hViewer->setTextRow(0,"0");
        cViewer->setTextRow(0,"0");
        memoryViewerTop->setTextRow(0, "0x     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
        memoryViewer->setTextRow(0, "0x0000 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
        cartViewer->setTextRow(0,"null");
        char* buffer = new char[256];

        pageViewer->setTextRow(0, itoa(currentPage, buffer, 10));
        for (auto i = 1; i < 28; i++) {
            sprintf(buffer, "0x%04X 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", (i*currentPage*0x10));
            memoryViewer->addRow(buffer);
        }

        breakButton->setSize(140, 60);
        stepButton->setSize(140, 60);
        leftPageButton->setSize(20,20);
        rightPageButton->setSize(20,20);

        breakButton->setPosition(460, 80);
        stepButton->setPosition(460, 160);
        leftPageButton->setPosition(133, 436);
        rightPageButton->setPosition(213, 436);
        breakBootRom->setPosition(460, 240);

        stepButton->setEnabled(false);
        leftPageButton->setEnabled(false);
        rightPageButton->setEnabled(false);

        stepButton->setBaseColor(gcn::Color(255, 255, 255, 100));
        leftPageButton->setBaseColor(gcn::Color(255, 255, 255, 100));
        rightPageButton->setBaseColor(gcn::Color(255, 255, 255, 100));

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
        top->add(memoryViewer);
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
        top->add(leftPageButton);
        top->add(rightPageButton);
        top->add(pageViewer);
        top->add(breakBootRom);
    }

    Debugger::~Debugger() {

        top->clear();

        delete hlViewer;
        delete deViewer;
        delete bcViewer;
        delete afViewer;
        delete spViewer;
        delete pcViewer;
        delete memoryViewer;
        delete memoryViewerTop;
        delete pageViewer;
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
        delete leftPageButton;
        delete rightPageButton;
        delete breakBootRom;

        delete top;

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

    void Debugger::UpdatePage() {
        unsigned char* Buffer = new unsigned char[0x10];
        for(int i = 0; i < 28; i++) {
            int BaseAddress = (i*0x10)+((currentPage-1)*(0x10*28));
            if (gb != NULL) {
                for (int j = 0x0; j < 0x10; j++) {
                    Buffer[j] = (gb->GetMemory()->DMA_read((BaseAddress)+j));
                }
            } else {
                for (int j = 0x0; j < 0x10; j++) {
                    Buffer[j] = 0x00;
                }
            }
            char* rowString = new char[256];
            if (BaseAddress > 0xFFF0) {
                delete rowString;
                memoryViewer->setTextRow(i, "");
                continue;
            }
            sprintf(rowString, "0x%04X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", BaseAddress,
                Buffer[0x0],
                Buffer[0x1],
                Buffer[0x2],
                Buffer[0x3],
                Buffer[0x4],
                Buffer[0x5],
                Buffer[0x6],
                Buffer[0x7],
                Buffer[0x8],
                Buffer[0x9],
                Buffer[0xA],
                Buffer[0xB],
                Buffer[0xC],
                Buffer[0xD],
                Buffer[0xE],
                Buffer[0xF]
            );
            memoryViewer->setTextRow(i, rowString);
            delete rowString;
        }
        delete Buffer;
    }

    void Debugger::UpdatePcHighlight() {
    }

    void Debugger::action(const gcn::ActionEvent &event) {
        switch(std::stoi(event.getId(), NULL, 10)) {
            case 0: OnClickLeftPageButton(); break;
            case 1: OnClickRightPageButton(); break;
            default: break;
        }
    }

    void Debugger::OnClickLeftPageButton() {
        if (currentPage == 1) {
            currentPage = 147;
        } else {
            currentPage--;
        }

        char Buffer[256];

        pageViewer->setTextRow(0, itoa(currentPage, Buffer, 10));
        UpdatePage();

    }

    void Debugger::OnClickRightPageButton() {
        if(currentPage == 147) {
            currentPage = 1;
        } else {
            currentPage++;
        }

        char Buffer[256];

        pageViewer->setTextRow(0, itoa(currentPage, Buffer, 10));
        UpdatePage();
    }
}

#endif