#ifndef MEMORYVIEWER_H
#define MEMORYVIEWER_H

#include "Memory.h"

#include <wx/wx.h>
#include <wx/vscroll.h>
#include <wx/grid.h>

class MemoryViewer : public wxWindow {

public:
    MemoryViewer(wxWindow*, Memory*);
    ~MemoryViewer();

private:

};

#endif