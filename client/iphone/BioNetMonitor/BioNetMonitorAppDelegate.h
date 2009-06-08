//
//  BioNetMonitorAppDelegate.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/6/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <IOKit/pwr_mgt/IOPMLib.h>
#import <IOKit/IOMessage.h>


@class RootViewController;
@class BionetManager;
@class GraphViewController;

@interface BioNetMonitorAppDelegate : NSObject <UIApplicationDelegate> {
    
    IBOutlet UIWindow *window;
    IBOutlet UINavigationController *navigationController;
	IBOutlet RootViewController *rootViewController;
	IBOutlet GraphViewController *graphViewController;
	
	BionetManager *bionetManager;
	
	
	io_connect_t root_port;
	io_object_t notifier;
	
}

- (void)powerMessageReceived:(natural_t)messageType withArgument:(void *) messageArgument;

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UINavigationController *navigationController;
@property (nonatomic, retain) GraphViewController *graphViewController;
@property (nonatomic, retain) RootViewController *rootViewController;

@property (nonatomic, retain) BionetManager *bionetManager;

@end

