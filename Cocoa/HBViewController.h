//
//  HBViewController.h
//
//  Created by Sherry Wu on 6/26/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <sys/types.h>
#import <sys/socket.h>
#import <netdb.h>
#import <cmath>
#import <cstdlib>

#import "socket.h"
#import "HBView.h"
#import "HBModel.h"

@interface HBViewController : NSViewController {
    IBOutlet HBModel *balls;
	
	SocketConnection* _sc;
	NSString* _ipaddy;
	NSNotificationCenter* notificationCenter;
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil IP:(NSString *)ip;
//- (void)awakeFromNib;
- (void)sendKeypressData:(NSSet *)keysPressed;
- (void)loop:(id)args;
- (void)dealloc;

@end
