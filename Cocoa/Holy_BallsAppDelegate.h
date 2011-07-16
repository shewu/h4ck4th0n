//
//  Holy_BallsAppDelegate.h
//  Holy Balls
//
//  Created by Sherry Wu on 6/24/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface Holy_BallsAppDelegate : NSObject <NSApplicationDelegate> {
    IBOutlet NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

- (IBAction)setFullScreen:(id)sender;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication;

@end
