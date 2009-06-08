//
//  BioNetMonitorAppDelegate.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/6/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "BioNetMonitorAppDelegate.h"
#import "RootViewController.h"
#import "GraphViewController.h"
#import "BionetManager.h"


@implementation BioNetMonitorAppDelegate

@synthesize window;
@synthesize navigationController;
@synthesize rootViewController;
@synthesize graphViewController;
@synthesize bionetManager;

void powerCallback(void *refCon, io_service_t service, natural_t messageType, void *messageArgument)
{       
	[(BioNetMonitorAppDelegate *)refCon powerMessageReceived: messageType withArgument: messageArgument];
}



- (void)awakeFromNib {
    IONotificationPortRef notificationPort;
    root_port = IORegisterForSystemPower(self, &notificationPort, powerCallback, &notifier);
    NSAssert(root_port != IO_OBJECT_NULL, @"IORegisterForSystemPower failed");
    CFRunLoopAddSource(CFRunLoopGetCurrent(), IONotificationPortGetRunLoopSource(notificationPort), kCFRunLoopDefaultMode);
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	
	// Configure and show the window
	[window addSubview:[navigationController view]];
	[window makeKeyAndVisible];
}


- (void)applicationWillTerminate:(UIApplication *)application {
	[bionetManager shutdown];
	// Save data if appropriate
}


- (void)powerMessageReceived:(natural_t)messageType withArgument:(void *) messageArgument
{
    switch (messageType)
    {
        case kIOMessageSystemWillSleep:
			/* The system WILL go to sleep. If you do not call IOAllowPowerChange or
			 IOCancelPowerChange to acknowledge this message, sleep will be
			 delayed by 30 seconds.
			 
			 NOTE: If you call IOCancelPowerChange to deny sleep it returns kIOReturnSuccess,
			 however the system WILL still go to sleep.
			 */
			
            // we cannot deny forced sleep
			NSLog(@"powerMessageReceived kIOMessageSystemWillSleep");
            IOAllowPowerChange(root_port, (long)messageArgument);  
            break;
        case kIOMessageCanSystemSleep:
			/*
			 Idle sleep is about to kick in.
			 Applications have a chance to prevent sleep by calling IOCancelPowerChange.
			 Most applications should not prevent idle sleep.
			 
			 Power Management waits up to 30 seconds for you to either allow or deny idle sleep.
			 If you don't acknowledge this power change by calling either IOAllowPowerChange
			 or IOCancelPowerChange, the system will wait 30 seconds then go to sleep.
			 */
			
			NSLog(@"powerMessageReceived kIOMessageCanSystemSleep");
			
			//cancel the change to prevent sleep
			IOCancelPowerChange(root_port, (long)messageArgument);
			//IOAllowPowerChange(root_port, (long)messageArgument); 
			
            break; 
        case kIOMessageSystemHasPoweredOn:
            NSLog(@"powerMessageReceived kIOMessageSystemHasPoweredOn");
            break;
    }
}



- (void)dealloc {
    [navigationController release];
	[rootViewController release];
    [window release];
	[bionetManager release];
	[super dealloc];
}

@end
