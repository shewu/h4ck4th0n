//
//  HBModel.h
//
//  Created by Sherry Wu on 6/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "constants.h"
#import "hack.h"

@interface HBModel : NSObject {
@public
	World* world;
	float angle;
	int myId;
	
	int width, height;
	
	NSLock* _lock;
}

// note: see http://stackoverflow.com/questions/588866/atomic-vs-nonatomic-properties \
	regarding atomic vs. nonatomic \
	TL;DR: nonatomic is considerably faster, but atomic is thread-safe

@property World* world;
@property float angle;
@property int myId;

@property int width;
@property int height;

- (id)init;
- (void)dealloc;
- (void)lock;
- (void)unlock;

@end
