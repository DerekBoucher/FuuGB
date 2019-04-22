//
//  CocoaWindow.m
//  GBemu
//
//  Created by Derek Boucher on 2019-02-04.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#import "CocoaWindow.h"

@implementation CocoaWindow

- (id)init
{
    self = [super init];
    MacEvent = [[MacEventHandler alloc] init];
    return self;
    
}

- (void)configureWindow:(SDL_SysWMinfo*)Handle
{
    NSMenuItem* FILE_menuitem = [[NSMenuItem alloc] initWithTitle:@"File" action:NULL keyEquivalent:@"'"];
    NSMenu* FILE_menu = [[NSMenu alloc] initWithTitle:@"File"];
    
    [FILE_menuitem setSubmenu:FILE_menu];
    [FILE_menu setAutoenablesItems:true];
    
    NSMenuItem* LOADROM_menuitem = [[NSMenuItem alloc] initWithTitle:@"Load ROM" action:NULL keyEquivalent:@"l"];
    [LOADROM_menuitem setTarget:MacEvent];
    [LOADROM_menuitem setAction:@selector(OnClick_LOADROM:)];
    
    [FILE_menu addItem:LOADROM_menuitem];
    [Handle->info.cocoa.window.menu insertItem:FILE_menuitem atIndex:1];
}
@end
