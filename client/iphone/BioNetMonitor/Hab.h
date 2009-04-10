//
//  Hab.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


#include "bionet.h"
#include "bionet-util.h"

@interface Hab : NSObject {
	bionet_hab_t * c_hab;
	NSString *ident;
	NSString *name;
}

+ (Hab*) createWrapper: (bionet_hab_t *)ptr;

- (Hab*) initWithPtr: (bionet_hab_t*)c_hab;
- (NSComparisonResult)compare:(Hab *)toHab;
- (void)wentAway;

@property (assign,readonly) bionet_hab_t* c_hab;
@property (assign, readonly) NSString* ident;
@property (assign, readonly) NSString* name;


@end
