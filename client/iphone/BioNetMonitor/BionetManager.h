//
//  BionetManager.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Hab.h"

@interface BionetManager : NSObject {
	bool isRunning;
	
	id habTypeController;
	id habController;
	id nodeController;
	id resourceController;
	
	NSMutableDictionary *rootDictionary;
	NSMutableDictionary *resourceDictionary;
	
	int bionet_fd;
	int hab_fd;
}

@property (retain, readwrite) id habTypeController;
@property (retain, readwrite) id habController;
@property (retain, readwrite) id nodeController;
@property (retain, readwrite) id resourceController;

@property (nonatomic, copy, readwrite) NSMutableDictionary *rootDictionary;
@property (nonatomic, copy, readwrite) NSMutableDictionary *resourceDictionary;


- (NSMutableArray*) genNodeListWithNodeFilter: (NSString*)node_id_filter habTypeFilter:(NSString*)hab_type_filter habIdFilter:(NSString*)node_id_filter;
- (NSMutableArray*) genHabListWithTypeFilter: (NSString*)hab_type_filter;
- (void) shutdown;
+ (BionetManager *)getInstance;
+ (NSMutableArray*) sortBionetObjects:(NSArray*)unsortedArray;

@end

