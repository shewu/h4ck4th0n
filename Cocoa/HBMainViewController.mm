//
//  HBMainViewController.m
//  Holy Balls
//
//  Created by Cherry Su on 7/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "HBMainViewController.h"

@implementation HBMainViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
    }
    
    return self;
}

- (void)awakeFromNib {
	
	notificationCenter = [NSNotificationCenter defaultCenter];
	serverConnectController = [[HBServerConnectViewController alloc] initWithNibName:@"HBServerConnectViewController" bundle:nil];
	[[serverConnectController view] setFrame:[[self view] bounds]];
	[[self view] addSubview:[serverConnectController view]];

	[notificationCenter addObserver:self selector:@selector(receivedIP:) name:@"HBReceivedIPNotification" object:nil];
	[notificationCenter addObserver:self selector:@selector(connectToServer:) name:@"HBServerUnavailable" object:nil];
}

- (void)receivedIP:(NSNotification *)notification {
	NSAssert([[notification name] isEqualToString:@"HBReceivedIPNotification"], @"received wrong notification!");
	NSDictionary* userInfo = [notification userInfo];
	NSString* ip = [userInfo objectForKey:@"ip"];
	NSAssert(ip, @"received IP is null!");
	[self showGameViewWithIP:ip];
}

- (void)connectToServer:(NSNotification *)notification {
	NSLog(@"%s", __PRETTY_FUNCTION__);
	NSAssert([[notification name] isEqualToString:@"HBServerUnavailable"], @"received wrong notification!");
	serverConnectController = [[HBServerConnectViewController alloc] initWithNibName:@"HBServerConnectViewController" bundle:nil];
	[[serverConnectController view] setFrame:[[self view] bounds]];
	[[self view] replaceSubview:[gameViewController view] with:[serverConnectController view]];
}

// [FIXED] bug: the nib is being loaded twice, so there are two controllers and two views!
- (void)showGameViewWithIP:(NSString *)ip {
	gameViewController = [[HBViewController alloc] initWithNibName:@"HBViewController" bundle:nil IP:ip];
	NSAssert(gameViewController, @"bad gameViewController");
	[gameViewController.view setFrame:[self.view bounds]];
//	[serverConnectController.view removeFromSuperview];
//	[self.view addSubview:gameViewController.view];
	[[self view] replaceSubview:[serverConnectController view] with:[gameViewController view]];
	NSLog(@"%s finished", __PRETTY_FUNCTION__);
}

- (void)dealloc
{
	[notificationCenter removeObserver:self];
	[super dealloc];
}

@end
