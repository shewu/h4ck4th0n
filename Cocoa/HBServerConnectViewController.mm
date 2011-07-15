//
//  HBServerConnectViewController.m
//  Holy Balls
//
//  Created by Cherry Su on 7/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "HBServerConnectViewController.h"

@implementation HBServerConnectViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (IBAction)connectToServer:(id)sender
{
	NSDictionary* userInfo = [NSDictionary dictionaryWithObject:[ipField stringValue] forKey:@"ip"];
	[[NSNotificationCenter defaultCenter] postNotification:[NSNotification notificationWithName:@"HBReceivedIPNotification" object:self userInfo:userInfo]];
}

@end
