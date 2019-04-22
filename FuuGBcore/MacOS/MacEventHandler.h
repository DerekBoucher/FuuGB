//
//  MacEventHandler.h
//  GBemu
//
//  Created by Derek Boucher on 2019-02-04.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

NS_ASSUME_NONNULL_BEGIN

@interface MacEventHandler : NSObject
{
    @public NSURL* filePath;
    @public FILE* inputBuffer;
}

-(IBAction)OnClick_LOADROM:(id)sender;

@end

NS_ASSUME_NONNULL_END
