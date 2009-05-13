//
//  DataPoint.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 5/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#include "bionet.h"
#include "bionet-util.h"

@interface DataPoint : NSObject {
	double timestamp; // Converted from timestamp
	double value;
}


- (DataPoint*) initWithPtr: (bionet_datapoint_t*) ptr;

@property (assign, readonly) double timestamp;
@property (assign, readonly) double value;


@end
