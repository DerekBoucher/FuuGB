#ifdef FUUGB_SYSTEM_MACOS

#ifndef MACEVENTHANDLER_H
#define MACEVENTHANDLER_H

#include "Gameboy.h"

@interface MacEventHandler : NSObject
{
@public
    NSURL *filePath;
@public
    FILE *inputBuffer;
@public
    FuuGB::Gameboy *gb_ref;
}

- (IBAction)OnClick_LOADROM:(id)sender;

@end

#endif

#endif