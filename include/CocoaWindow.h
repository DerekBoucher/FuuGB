#ifdef FUUGB_SYSTEM_MACOS

#ifndef COCOAWINDOW_H
#define COCOAWINDOW_H

#import "MacEventHandler.h"
#include "Gameboy.h"

NS_ASSUME_NONNULL_BEGIN

@interface CocoaWindow : NSObject
{
@public
    MacEventHandler *MacEvent;
}

- (void)configureWindow:(SDL_SysWMinfo *)Handle;

@end

NS_ASSUME_NONNULL_END

#endif
#endif