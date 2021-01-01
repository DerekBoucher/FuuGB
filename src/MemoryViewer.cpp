#include "MemoryViewer.h"

MemoryViewer::MemoryViewer(wxWindow* parent, Memory* memory) : wxWindow(parent, wxID_ANY, wxPoint(0, 0), wxDefaultSize) {
    wxStaticText* memoryPaneLabel = new wxStaticText(this, wxID_ANY, wxT("Memory"), wxPoint(10, 10), wxDefaultSize);
    wxPanel* memoryPane = new wxPanel(this, wxID_ANY, wxPoint(10, 30), wxSize(515, 400), wxBORDER_SIMPLE);
    wxTextCtrl* textBox = new wxTextCtrl(memoryPane, -1, "", wxPoint(0, 0), wxSize(515, 400), wxTE_READONLY | wxTE_MULTILINE);
    wxFont font(wxFontInfo(9).FaceName("Courier New"));
    textBox->SetFont(font);

    wxString CharString = "";
    for (int i = 0; i < 0x10000; i++) {
        if (i == 0) {
            CharString += "0000  ";
        }
        else if (i % 0x10 == 0) {
            CharString += "  ";
            for (int j = 0; j < 0x10; j++) {
                char* buffer = new char[2];
                snprintf(buffer, 2, "%c", memory->Read(i - (0x10 - j), true));
                CharString += buffer;
                delete buffer;
            }
            char* buffer = new char[6];
            sprintf(buffer, "\n%04X  ", i);
            CharString += buffer;
            delete buffer;
        }
        char* buffer = new char[2];
        sprintf(buffer, "%02X ", memory->Read(i, true));
        CharString += buffer;
        delete buffer;

        if (i == 0xFFFF) {
            CharString += "  ";
            for (int j = 0; j < 0x10; j++) {
                char* buffer = new char;
                *buffer = memory->Read(i - (0x10 - j), true);
                if (*buffer < 0x7F && *buffer != 0x00 && *buffer != 0x0A) {
                    int val = *buffer;
                    sprintf(buffer, "%c", val);
                    CharString += buffer;
                }
                else {
                    CharString += ".";
                }
                delete buffer;
            }
        }
    }

    textBox->AppendText(CharString);
    textBox->SetScrollPos(wxVERTICAL, 0, true);
    memoryPane->Refresh();
}

MemoryViewer::~MemoryViewer() {

}