#ifndef DEBUGGER_H
#define DEBUUGER_H

#include "Defines.h"
#include "Memory.h"
#include "Cartridge.h"

#include <wx/wx.h>

class Debugger : public wxFrame {

public:
    Debugger(Memory*, Cartridge*);
    ~Debugger();

private:
};

#endif