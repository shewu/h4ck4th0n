//
//  HBServerConnectViewController.h
//  Holy Balls
//
//  Created by Cherry Su on 7/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "HBMainViewController.h"

@class HBMainViewController;

@interface HBServerConnectViewController : NSViewController {
	IBOutlet NSTextField* ipField;
}

- (IBAction)connectToServer:(id)sender;

@end
