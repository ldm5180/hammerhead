//
//  Resource.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 5/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#include "bionet.h"
#include "bionet-util.h"
//#import "DataPoint.h"

@interface Resource : NSObject {	
	NSString *ident;
	NSString *name;
	NSString *valueStr;
	NSDate *timestamp;
	NSString *habName;
	NSString *localName;
	NSString *dataType;
	bool isNumeric;
	bionet_resource_flavor_t flavor;
	NSMutableArray * dataPoints;
	double minValue;
	double maxValue;
	double minTime;
	double maxTime;
}	

@property (assign, readonly) NSString* ident;
@property (assign, readonly) NSString* name;
@property (assign, readonly) NSString* localName;
@property (assign, readonly) NSString* habName;
@property (assign) NSString* valueStr;
@property (copy) NSDate* timestamp;
@property (assign, readonly) NSString* dataType;
@property (assign, readonly) bool isNumeric;
@property (assign, readonly) bionet_resource_flavor_t flavor;
@property (assign, readonly) NSMutableArray * dataPoints;
@property (assign, readonly) double maxValue;
@property (assign, readonly) double minValue;
@property (assign, readonly) double maxTime;
@property (assign, readonly) double minTime;

+ (Resource*) createWrapper: (bionet_resource_t *)ptr;

- (Resource*) initWithPtr: (bionet_resource_t*)ptr;
- (void) pushDataPoint: (bionet_datapoint_t*) c_datapoint;
- (BOOL) commandValue: (NSString*)newVal;

@end
