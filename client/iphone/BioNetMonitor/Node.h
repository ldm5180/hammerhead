//
//  Node.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#include "bionet.h"
#include "bionet-util.h"

@interface Node : NSObject {
	bionet_node_t * c_node;
	NSString *ident;
	NSString *name;
}

+ (Node*) createWrapper: (bionet_node_t *)ptr;

- (Node*) initWithPtr: (bionet_node_t*)ptr;
- (NSMutableArray*) contentsAsArrayOfResources;
- (NSComparisonResult)compare:(Node *)indexPath;
- (void)wentAway;

@property (assign, readonly) bionet_node_t* c_node;
@property (assign, readonly) NSString* ident;
@property (assign, readonly) NSString* name;

@end
