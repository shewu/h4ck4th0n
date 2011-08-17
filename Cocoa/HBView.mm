//
//  HBView.m
//
//  Created by Sherry Wu on 6/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "HBView.h"

@implementation HBView

#pragma mark -
#pragma mark Drawing

- (void)initGL {
	NSLog(@"%s: balls pointer is %p", __PRETTY_FUNCTION__, balls);
	// do other things
	NSLog(@"initialized HBView");
	if (!_context) {
		NSOpenGLPixelFormatAttribute attrs[] = {
			NSOpenGLPFADoubleBuffer,
			NSOpenGLPFADepthSize, 32,
			0
		};
		
		NSOpenGLPixelFormat* format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];	
		_context = [[NSOpenGLContext alloc] initWithFormat:format 
											  shareContext:nil];
	}

	// do context stuff
	[_context clearDrawable];
	[_context setView:self]; // causes "invalid drawable" error -- fixed by moving NSOpenGLContext allocation to viewDidMoveToWindow
	
    [_context makeCurrentContext];
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ((float) balls.width) / ((float) balls.height), 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	_quad = gluNewQuadric();
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
	
	GLfloat light_ambient[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat light_diffuse[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_specular[] = { 0.8, 0.8, 0.8, 0.0 };
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 200.0);
	
	glEnable(GL_LIGHTING); // enable lighting 
	glEnable(GL_LIGHT0); // enable light 0
	
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	// clean up context stuff
	[_context flushBuffer];
	[NSOpenGLContext clearCurrentContext];
	NSLog(@"initGL done");
	_initGLDone = YES;
	[self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect {
	if (!_initGLDone) {
		return;
	}
	[_context clearDrawable];
	[_context setView:self];
	
	[_context makeCurrentContext];
	
	// seems like this needs a nop to draw, otherwise it's just all blue?!
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_MULTISAMPLE_ARB);
	
	glLoadIdentity();
	
	float focusx = balls.world->objects[balls.myId].p.x, focusy = balls.world->objects[balls.myId].p.y;
	if (focusx < balls.world->minX+6) {
		focusx = balls.world->minX+6;
	}
	if (focusx > balls.world->maxX-6) {
		focusx = balls.world->maxX-6;
	}
	if (focusy < balls.world->minY+6) {
		focusy = balls.world->minY+6;
	}
	if (focusy > balls.world->maxY-6) {
		focusy = balls.world->maxY-6;
	}
	
	gluLookAt(focusx-6*cos(balls.angle), focusy-6*sin(balls.angle), 3, focusx, focusy, 0.0, 0.0, 0.0, 1.0);
	float matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	
	LOCK(); { /* begin draw obstacles */
		glUseProgram(0);
		glBegin(GL_QUADS);
		for (std::vector<Obstacle>::const_iterator i = balls.world->obstacles.begin(); i != balls.world->obstacles.end(); i++) {
			glColor3f(i->color.r/255.0, i->color.g/255.0, i->color.b/255.0);
			glVertex3f(i->p1.x, i->p1.y, 0);
			glVertex3f(i->p2.x, i->p2.y, 0);
			glVertex3f(i->p2.x, i->p2.y, 1);
			glVertex3f(i->p1.x, i->p1.y, 1);
		}
		glEnd();
	} UNLOCK(); /* end draw obstacles */
	
	glUseProgram(0);
	glUniform3f(glGetUniformLocation(0, "lightv"), 5*matrix[8]+matrix[12], 5*matrix[9]+matrix[13], 5*matrix[10]+matrix[14]);
	
	NSAssert(_quad, @"quad is null!");
	
	glPushMatrix();
	glScalef(1, 1, -1);
	LOCK(); { /* begin draw objects */
		glEnable(GL_NORMALIZE);
		for (std::map<int, Object>::const_iterator i = balls.world->objects.begin(); i != balls.world->objects.end(); ++i) {
			glPushMatrix();
			glTranslatef(i->second.p.x, i->second.p.y, 0);
			glScalef(i->second.rad, i->second.rad, i->second.hrat*i->second.rad);
			glColor3f(i->second.color.r/255.0, i->second.color.g/255.0, i->second.color.b/255.0);
			gluSphere(_quad, 1.0, 50, 50);
			glPopMatrix();
		}
		glDisable(GL_NORMALIZE);
	} UNLOCK(); /* end draw objects */
	glPopMatrix();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	LOCK(); { /* begin draw floor */
		// checkerboard
		unsigned int GridSizeX = balls.world->maxX/3;
		unsigned int GridSizeY = balls.world->maxY/3;
		unsigned int SizeX = 6;
		unsigned int SizeY = 6;
		const float alpha = 0.82;
		
		glEnable(GL_NORMALIZE);
		glBegin(GL_QUADS);
		for (int x = 0; x < GridSizeX; ++x) {
			for (int y = 0; y < GridSizeY; ++y) {
				if (abs(x+y) & 1) { // modulo 2
					glColor4f(1.0f,1.0f,1.0f, alpha);
				} else {
					glColor4f(0.0f,0.0f,0.0f, alpha);
				}
				
				glNormal3f(                              0,                              0, 1);
				glVertex3f(    x*SizeX + balls.world->minX,    y*SizeY + balls.world->minY, 0);
				glVertex3f((x+1)*SizeX + balls.world->minX,    y*SizeY + balls.world->minY, 0);
				glVertex3f((x+1)*SizeX + balls.world->minX,(y+1)*SizeY + balls.world->minY, 0);
				glVertex3f(    x*SizeX + balls.world->minX,(y+1)*SizeY + balls.world->minY, 0);
				
			}
		}
		glEnd();
		glDisable(GL_NORMALIZE);
	} UNLOCK(); /* end draw floor */

	glDisable(GL_BLEND);
	
	LOCK(); { /* begin draw objects */
		glEnable(GL_NORMALIZE);
		for (std::map<int, Object>::const_iterator i = balls.world->objects.begin(); i != balls.world->objects.end(); ++i) {
			glPushMatrix();
			glTranslatef(i->second.p.x, i->second.p.y, 0);
			glScalef(i->second.rad, i->second.rad, i->second.hrat*i->second.rad);
			glColor3f(i->second.color.r/255.0, i->second.color.g/255.0, i->second.color.b/255.0);
			gluSphere(_quad, 1.0, 50, 50);
			glPopMatrix();
		}
		glDisable(GL_NORMALIZE);
	} UNLOCK(); /* end draw objects */
	
	glDisable(GL_MULTISAMPLE_ARB);
	
	glFlush();
	
	[_context flushBuffer];
	[NSOpenGLContext clearCurrentContext];
}

- (BOOL)isOpaque {
	return YES;
}

#pragma mark -
#pragma mark Memory management

- (id)initWithFrame:(NSRect)frameRect {
	if (self = [super initWithFrame:frameRect]) {
		_initGLDone = NO;
	}
	return self;	
}

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)pixelFormat {
	if (self = [super initWithFrame:frameRect]) {
		_context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat 
											  shareContext:nil];
	}
	return self;
}

- (void)dealloc {
	[_context release];
	[super dealloc];
}

#pragma mark -
#pragma mark Keyboard/Mouse Handling

- (void)viewDidMoveToWindow {
	[[self window] setAcceptsMouseMovedEvents:YES];
	_trackingTag = [self addTrackingRect:[self bounds] 
								  owner:self 
							   userData:NULL 
						   assumeInside:YES];
	[[self window] makeFirstResponder:self];
}

- (void)mouseEntered:(NSEvent *)theEvent {
    _wasAcceptingMouseEvents = [[self window] acceptsMouseMovedEvents];
    [[self window] setAcceptsMouseMovedEvents:YES];
    [[self window] makeFirstResponder:self];
    [self setNeedsDisplayInRect:[self bounds]];
    [self setNeedsDisplay:YES];
	_mouseInArea = YES;
}

- (void)mouseMoved:(NSEvent *)theEvent {
	if(_mouseInArea) {
		_xCurLoc = [NSEvent mouseLocation].x;
		if(_xPrevLoc == -1) {
			_xPrevLoc = [NSEvent mouseLocation].x;
		} else {
			float del = _xCurLoc - _xPrevLoc;
			balls.angle -= del/100.0;
			
			while(balls.angle >= 2*M_PI) {
				balls.angle -= 2*M_PI;
			}
			while(balls.angle < 0) {
				balls.angle += 2*M_PI;
			}
			_xPrevLoc = _xCurLoc; // lolol how did i forget this
		}
		[self setNeedsDisplay:YES];
	}
}

- (void)mouseExited:(NSEvent *)theEvent {
	_mouseInArea = NO;
    [[self window] setAcceptsMouseMovedEvents:_wasAcceptingMouseEvents];
	[self setNeedsDisplay:YES];
}

- (void)keyDown:(NSEvent *)theEvent {
	if(!_keysPressed) {
		_keysPressed = [[NSMutableSet alloc] init];
	}
	NSNumber* k = [NSNumber numberWithInt:[theEvent keyCode]];
	switch([theEvent keyCode]) {
		case kS:
		case kW:
		case kA:
		case kD:
			[_keysPressed addObject:k];
			break;
		default:
			break;
	}
	[self setNeedsDisplay:YES];
}

- (void)keyUp:(NSEvent *)theEvent {
	if(!_keysPressed) {
		_keysPressed = [[NSMutableSet alloc] init];
	}
	NSNumber* k = [NSNumber numberWithInt:[theEvent keyCode]];
	switch ([theEvent keyCode]) {
		case kW:
		case kA:
		case kS:
		case kD:
			[_keysPressed removeObject:k];
			break;
		case kESC:
			exit(0);
			break;
		default:
			break;
	}
}

- (BOOL)acceptsFirstResponder {
	return TRUE;
}

#pragma mark -
#pragma mark Misc

- (NSSet *)getKeypresses {
	return _keysPressed;
}

@end
