//
//  HBViewController.m
//
//  Created by Sherry Wu on 6/26/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "HBViewController.h"

@implementation HBViewController

#pragma mark -
#pragma mark Initialization

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil IP:(NSString *)ip {
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
		_ipaddy = ip;
		NSLog(@"ip address is %@", _ipaddy);
		
		balls.width = [self.view bounds].size.width;
		balls.height = [self.view bounds].size.height;
		
		srand(time(NULL));
		
		addrinfo hints, *res;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_DGRAM;
		
		getaddrinfo([_ipaddy cStringUsingEncoding:NSASCIIStringEncoding], "55555", &hints, &res);
		int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		_sock = new Socket(sockfd);
		_sc = _sock->connect(*res->ai_addr, res->ai_addrlen);
		
		int u[2];
		bool done = false;
		int tries = 10;
		for (int i = 0; i < tries && !done; ++i) {
			_sc->send();
			[NSThread sleepForTimeInterval:0.02];
			int n;
			while (!done && (n = _sc->receive((char *)&u, 8)) != -1) {
				done |= (n == 4);
			}
		}
		_sc->packetnum = 10;
		if (!done) {
			NSLog(@"Failed to connect");
			// TODO: post notification to go back to ip address view
			exit(-1);
		}
		u[0] = ntohl(u[0]);
		[(HBView *)self.view initGL];
		
		balls.myId = -1;
		balls.angle = *reinterpret_cast<float*>(u);
		
		[NSThread detachNewThreadSelector:@selector(loop:) toTarget:self withObject:nil];
	}
	NSLog(@"%s", __PRETTY_FUNCTION__);
	return self;
}

- (void)loop:(id)args {
	while (true) {
		int status;
		[balls lock];
		int ID = balls.myId;
		while ((status = balls.world->receiveObjects(_sc, ID)) != -1);
		balls.myId = ID;
		[balls unlock];
		
		// le speed limiter -- 0.02 seems to be the sweet spot
		[NSThread sleepForTimeInterval:0.02];
		
		NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		[self sendKeypressData:[(HBView *)[self view] getKeypresses]];		
		[[self view] setNeedsDisplay:YES];
		[pool drain];
	}
}

- (void)sendKeypressData:(NSSet *)keysPressed {
	const int bufsize = 5;
	char buf[bufsize];
	float angle = balls.angle;
	*((int *)buf) = htonl(*reinterpret_cast<int*> (&angle));
	buf[bufsize - 1] = 0;
	if ([keysPressed containsObject:[NSNumber numberWithInt:kA]]) {
		buf[bufsize - 1] ^= 1;
	}
	if ([keysPressed containsObject:[NSNumber numberWithInt:kD]]) {
		buf[bufsize - 1] ^= 2; 
	}
	if ([keysPressed containsObject:[NSNumber numberWithInt:kW]]) {
		buf[bufsize - 1] ^= 4; 
	}
	if ([keysPressed containsObject:[NSNumber numberWithInt:kS]]) {
		buf[bufsize - 1] ^= 8;
	}
	if (_sc) {
		 _sc->add(buf, bufsize);
		 _sc->send();
	} else {
		 NSLog(@"socket connection sc is NULL!");
	}
}

- (void)dealloc {
	delete _sc;
	[_ipaddy release];
	[super dealloc];
}

@end
