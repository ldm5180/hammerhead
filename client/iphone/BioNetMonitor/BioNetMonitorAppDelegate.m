//
//  BioNetMonitorAppDelegate.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/6/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "BioNetMonitorAppDelegate.h"
#import "RootViewController.h"
#import "DataController.h"
#import "BionetManager.h"


@implementation BioNetMonitorAppDelegate

@synthesize window;
@synthesize navigationController;
@synthesize rootViewController;
@synthesize dataController;
@synthesize bionetManager;


- (void)applicationDidFinishLaunching:(UIApplication *)application {
	
	// Configure and show the window
	[window addSubview:[navigationController view]];
	[window makeKeyAndVisible];
}


- (void)applicationWillTerminate:(UIApplication *)application {
	[bionetManager shutdown];
	// Save data if appropriate
}


- (void)dealloc {
    [navigationController release];
	[rootViewController release];
    [window release];
    [dataController release];
	[bionetManager release];
	[super dealloc];
}

@end
