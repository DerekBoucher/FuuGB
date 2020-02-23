#include "FuuGB.h"
#include "Core.h"

extern "C"
{
    void FUUGB_API execute()
    {
        FUUGB_APP app;
        app.run();
    }
}