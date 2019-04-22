//
//  MacEventHandler.m
//  GBemu
//
//  Created by Derek Boucher on 2019-02-04.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#import "MacEventHandler.h"

@implementation MacEventHandler

- (id)init
{
    self = [super init];
    filePath = [[NSURL alloc]initWithString:@"."];
    inputBuffer = NULL;
    return self;
}

- (IBAction)OnClick_LOADROM:(id)sender
{
    NSOpenPanel* OpenFileDialog = [NSOpenPanel openPanel];
    [OpenFileDialog setCanChooseFiles:true];
    [OpenFileDialog setAllowsMultipleSelection:false];
    [OpenFileDialog setCanCreateDirectories:false];
    [OpenFileDialog setMessage:@"Select a .gb ROM file to open"];
    if([OpenFileDialog runModal] == NSModalResponseOK)
    {
        self->filePath = [OpenFileDialog URL];
        self->inputBuffer = fopen([[self->filePath path]UTF8String], "r");
    }
    [OpenFileDialog close];
}

@end
