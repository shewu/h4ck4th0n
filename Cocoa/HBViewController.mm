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
		[NSThread detachNewThreadSelector:@selector(loop:) toTarget:self withObject:nil];
	}
	NSLog(@"%s", __PRETTY_FUNCTION__);
	return self;
}
/*
- (void)awakeFromNib { // this is called
	NSLog(@"%s", __PRETTY_FUNCTION__);
	NSLog(@"width = %f", [[self view] bounds].size.width);
	NSLog(@"height = %f", [[self view] bounds].size.height);
	
	notificationCenter = [NSNotificationCenter defaultCenter];
	
	// spawn the main thread here - causes _sc corruption
}
*/
- (void)loop:(id)args {
	NSAutoreleasePool* outerPool = [[NSAutoreleasePool alloc] init];
	
	// get width and height
	balls.width = [[self view] bounds].size.width;
	balls.height = [[self view] bounds].size.height;
	NSLog(@"%s: width = %d, height = %d", __PRETTY_FUNCTION__, balls.width, balls.height);
	
	// assume this is the same balls as in the view is valid!
	NSLog(@"%s: balls pointer is %p", __PRETTY_FUNCTION__, balls);
	NSLog(@"ip address is %@", _ipaddy);

	srand(time(NULL));
	
	addrinfo hints, *res;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	getaddrinfo([_ipaddy cStringUsingEncoding:NSASCIIStringEncoding], "55555", &hints, &res);
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	printf("sockfd = %d\n", sockfd);
	Socket sock(sockfd);
	_sc = sock.connect(*res->ai_addr, res->ai_addrlen);
	NSLog(@"socketconnection = %p", _sc);
	
	int u[2];
	bool done = false;
	int tries = 10;
	for (int i = 0; i < tries && !done; i++) {
		_sc->send();
		[NSThread sleepForTimeInterval:0.2];
		int n;
		while (!done && (n = _sc->receive((char*)&u, 8)) != -1) {
			done |= (n == 4);
		}
	}
	_sc->packetnum = 10;
	if (!done) {
		NSLog(@"Failed to Connect");
		// TODO: post a notification and go back to the ip address view
		exit(-1);
	}
	NSLog(@"%d: here", __LINE__);
	
	u[0] = ntohl(u[0]);
	
	NSLog(@"%d: here", __LINE__);
	
	[(HBView *)[self view] initGL];
	
	NSLog(@"%d: here", __LINE__);
	
	balls.myId = -1;
	balls.angle = *reinterpret_cast<float*>(u);
	
	NSLog(@"connections size = %lu", _sc->sock->connections.size());
	
	[outerPool drain];
	
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
