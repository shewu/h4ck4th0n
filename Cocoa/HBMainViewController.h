//
//  HBMainViewController.h
//  Holy Balls
//
//  Created by Cherry Su on 7/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "HBServerConnectViewController.h"
#import "HBViewController.h"

@class HBServerConnectViewController;

@interface HBMainViewController : NSViewController {
	HBServerConnectViewController* serverConnectController;
	HBViewController* gameViewController;
}

- (void)awakeFromNib;
- (void)showGameViewWithIP:(NSString *)ip;
- (void)dealloc;

@end
