//
//  Holy_BallsAppDelegate.m
//  Holy Balls
//
//  Created by Sherry Wu on 6/24/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "Holy_BallsAppDelegate.h"

@implementation Holy_BallsAppDelegate

#pragma mark -
#pragma mark Behaviors

- (IBAction)setFullScreen:(id)sender {
	/*
    NSRect mainDisplayRect = [[NSScreen mainScreen] frame];
	
	NSWindow *fullScreenWindow = [[NSWindow alloc] initWithContentRect:mainDisplayRect 
															 styleMask:NSBorderlessWindowMask 
															   backing:NSBackingStoreBuffered 
																 defer:YES];
	[fullScreenWindow setLevel:NSMainMenuWindowLevel+1];
	
	[fullScreenWindow setOpaque:YES];
	[fullScreenWindow setHidesOnDeactivate:YES];
	
	NSOpenGLPixelFormatAttribute attrs[] = {
		NSOpenGLPFADoubleBuffer,
		0
	};	
	NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	NSRect viewRect = NSMakeRect(0.0, 0.0, mainDisplayRect.size.width, mainDisplayRect.size.height);
	HBView* fullScreenView = [[HBView alloc] initWithFrame:viewRect 
											   pixelFormat:pixelFormat];
	[fullScreenWindow setContentView:fullScreenView];
	
	[fullScreenWindow makeKeyAndOrderFront:self];
	
	[view setNeedsDisplay:YES];
	*/
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
	return YES;
}

@synthesize window;

#pragma mark -
#pragma mark Initialization

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application 
}

@end
