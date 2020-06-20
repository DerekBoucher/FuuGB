#ifdef FUUGB_SYSTEM_MACOS
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
    if(gb_ref != NULL)
        gb_ref->Pause();
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
    if(gb_ref != NULL)
        gb_ref->Resume();
}

@end
#endif
