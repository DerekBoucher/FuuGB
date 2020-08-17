#define SDL_MAIN_HANDLED
#include "Sandbox.h"

int main(int argc, char **argv)
{
    Sandbox *sandbox = new Sandbox();
    sandbox->Run(argc, argv);
    delete sandbox;
}
