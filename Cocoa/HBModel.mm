//
//  HBModel.m
//
//  Created by Sherry Wu on 6/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "HBModel.h"

@implementation HBModel

@synthesize world;
@synthesize angle;
@synthesize myId;

@synthesize width;
@synthesize height;

#pragma mark -
#pragma mark Memory management

- (id)init {
	if (self = [super init]) {
		NSLog(@"%s: HBModel init", __PRETTY_FUNCTION__);
		myId = -1;
		world = new World();
		width = height = 0;
		if (!world) {
			NSLog(@"%s: world is null!", __PRETTY_FUNCTION__);
		}
		_lock = [[NSLock alloc] init];
	}
	return self;
}

- (void)dealloc {
	[super dealloc];
}

- (void)lock {
	[_lock lock];
}

- (void)unlock {
	[_lock unlock];
}

@end
