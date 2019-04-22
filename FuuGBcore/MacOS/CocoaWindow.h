//
//  CocoaWindow.h
//  GBemu
//
//  Created by Derek Boucher on 2019-02-04.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#import "MacEventHandler.h"

NS_ASSUME_NONNULL_BEGIN

@interface CocoaWindow : NSObject
{
@public MacEventHandler* MacEvent;
}

- (void)configureWindow:(SDL_SysWMinfo*)Handle;

@end

NS_ASSUME_NONNULL_END
