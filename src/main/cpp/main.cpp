#ifndef FUUGB_UNIT_TEST
#define SDL_MAIN_HANDLED
#include <stdio.h>

#include "Sandbox.h"

int main(int argc, char** argv)
{
    Sandbox* sandbox = new Sandbox();
    sandbox->run();
    delete sandbox;
}

#endif