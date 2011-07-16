//
//  HBView.h
//
//  Created by Sherry Wu on 6/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <GLUT/glut.h>

#import "HBModel.h"

#define UP 125
#define DOWN 126
#define LEFT 123
#define RIGHT 124

#define kW 13
#define kA 0
#define kS 1
#define kD 2

#define kESC 53

#define HB_PORT 55555

#define LOCK() \
	[balls lock];

#define UNLOCK() \
	[balls unlock];

@interface HBView : NSView {
    IBOutlet HBModel *balls;

	NSOpenGLContext* _context;
	BOOL _mouseInArea;
	BOOL _wasAcceptingMouseEvents;
	int _xCurLoc, _xPrevLoc;
	NSTrackingRectTag _trackingTag;
	GLUquadric* _quad;
	NSMutableSet* _keysPressed;
	BOOL _initGLDone;
}

- (id)initWithFrame:(NSRect)frameRect;
- (id)initWithFrame:(NSRect)frameRect 
		pixelFormat:(NSOpenGLPixelFormat *)pixelFormat;
- (void)dealloc;

- (void)initGL;
- (void)drawRect:(NSRect)dirtyRect;
- (BOOL)isOpaque;

- (NSSet *)getKeypresses;

@end
