#ifndef GUI_H
#define GUI_H

#include "Defines.h"
#include "Gameboy.h"

#include <wx/frame.h>

class GUI : public wxFrame {

public:
    GUI();
    ~GUI();

    wxDECLARE_EVENT_TABLE();

private:
    wxWindow* gameboyScreen = nullptr;
    Gameboy* gameboy = nullptr;
    void OnClose(wxCloseEvent&);
    void OnClickOpen(wxCommandEvent&);
    void OnClickExit(wxCommandEvent&);
};

#endif