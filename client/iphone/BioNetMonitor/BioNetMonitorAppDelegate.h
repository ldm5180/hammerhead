//
//  BioNetMonitorAppDelegate.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/6/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@class DataController;
@class RootViewController;
@class BionetManager;

@interface BioNetMonitorAppDelegate : NSObject <UIApplicationDelegate> {
    
    IBOutlet UIWindow *window;
    IBOutlet UINavigationController *navigationController;
	IBOutlet RootViewController *rootViewController;
	
    DataController *dataController;
	BionetManager *bionetManager;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UINavigationController *navigationController;
@property (nonatomic, retain) RootViewController *rootViewController;

@property (nonatomic, retain) DataController *dataController;
@property (nonatomic, retain) BionetManager *bionetManager;

@end

