//
//  RootViewController.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/6/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "BionetManager.h"

@interface RootViewController : UITableViewController {
	NSMutableArray *list;
	BionetManager *bionetManager;
	int depth;
	NSString *subTitle;
	NSString *hab_type_filter;
	NSString *hab_id_filter;
	NSString *node_id_filter;
}

- (void)reloadTable:(id)sender;
- (void)newItem: (id)theItem;
- (void)delItem: (id)theItem;
- (void) setDepth: (int)theDepth;

@property (nonatomic, retain) NSMutableArray *list; 
@property (nonatomic, retain) BionetManager *bionetManager;
@property (nonatomic, retain) NSString *subTitle;
@property (nonatomic, retain) NSString *hab_type_filter;
@property (nonatomic, retain) NSString *hab_id_filter;
@property (nonatomic, retain) NSString *node_id_filter;


- (unsigned)countOfList;
- (id)objectInListAtIndex:(unsigned)theIndex;

@end
