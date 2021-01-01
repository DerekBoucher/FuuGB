#include "Debugger.h"

wxBEGIN_EVENT_TABLE(Debugger, wxFrame)
EVT_CLOSE(Debugger::OnClose)
wxEND_EVENT_TABLE();

Debugger::Debugger(wxFrame* parent, Gameboy* g, Memory* m, Cartridge* c) : wxFrame(parent, wxID_ANY, wxT("Debugger"), wxDefaultPosition, wxSize(NATIVE_SIZE_X* SCALE_FACTOR + 50, NATIVE_SIZE_Y* SCALE_FACTOR), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER) {
    gameboyRef = g;

    wxNotebook* tabbedWindow = new wxNotebook(this, wxID_ANY, wxPoint(0, 0), wxDefaultSize, wxNB_LEFT);

    MemoryViewer* memoryPage = new MemoryViewer(tabbedWindow, m);
    CartridgeViewer* cartPage = new CartridgeViewer(tabbedWindow, c);
    VramViewer* vramPage = new VramViewer(tabbedWindow, m);

    tabbedWindow->AddPage(memoryPage, wxT("Memory Viewer"), true, wxID_ANY);
    tabbedWindow->AddPage(cartPage, wxT("Cartridge Viewer"), false, wxID_ANY);
    tabbedWindow->AddPage(vramPage, wxT("VRAM Viewer"), false, wxID_ANY);
}

Debugger::~Debugger() {

}

void Debugger::OnClose(wxCloseEvent& e) {
    GetParent()->Enable();
    gameboyRef->Resume();
    Destroy();
    e.Skip();
}