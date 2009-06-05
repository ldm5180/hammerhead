//
//  Resource.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 5/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Resource.h"
#import "DataPoint.h"
#include "bionet.h"
#include "bionet-resource.h"

@implementation Resource

@synthesize ident, name, valueStr, timestamp, dataType, localName, habName, isNumeric, flavor, dataPoints, maxValue, minValue, maxTime, minTime;

- (Resource*) initWithPtr: (bionet_resource_t *) ptr {
	if(self = [super init]){
		bionet_resource_t * c_resource = ptr;
		
		bionet_resource_data_type_t type = bionet_resource_get_data_type(c_resource);
		
		name = [[NSString alloc] initWithCString: bionet_resource_get_name(c_resource)]; 
		ident = [[NSString alloc] initWithCString:  bionet_resource_get_id(c_resource)];
		localName = [[NSString alloc] initWithCString:  bionet_resource_get_local_name(c_resource)];
		habName = name;
		[habName retain];
		dataType = [[NSString alloc] initWithCString:  bionet_resource_data_type_to_string(bionet_resource_get_data_type(c_resource))];
		flavor = bionet_resource_get_flavor(c_resource);
		dataPoints = [[NSMutableArray alloc] init];
		
		switch(type){
			case BIONET_RESOURCE_DATA_TYPE_UINT8:
			case BIONET_RESOURCE_DATA_TYPE_INT8:
			case BIONET_RESOURCE_DATA_TYPE_UINT16:
			case BIONET_RESOURCE_DATA_TYPE_INT16:
			case BIONET_RESOURCE_DATA_TYPE_UINT32:
			case BIONET_RESOURCE_DATA_TYPE_INT32:
			case BIONET_RESOURCE_DATA_TYPE_FLOAT:
			case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
				isNumeric = TRUE;
				break;
	
			default:
				isNumeric = FALSE;
				break;
		}
		bionet_datapoint_t *c_datapoint = bionet_resource_get_datapoint_by_index(c_resource, 0);
		if(c_datapoint){
			[self pushDataPoint:c_datapoint];
		}
	}	
	return self;
}				

+ (Resource*) createWrapper: (bionet_resource_t *)ptr {
	Resource * obj = (Resource*)bionet_resource_get_user_data(ptr);
	if(obj){
		[obj retain];
	}else{
		obj = [[Resource alloc] initWithPtr: ptr];
		bionet_resource_set_user_data(ptr, (void*) obj);
	}
	return obj;
}


- (void) pushDataPoint: (bionet_datapoint_t*) ptr {
	self.valueStr = [[NSString alloc] initWithUTF8String: bionet_value_to_str(bionet_datapoint_get_value(ptr))];	
	DataPoint* datapoint = [[DataPoint alloc] initWithPtr: (bionet_datapoint_t*)ptr];
	self.timestamp = [NSDate dateWithTimeIntervalSince1970: datapoint.timestamp];
	[dataPoints addObject:datapoint];
	
	if(dataPoints.count == 1){
		maxValue = datapoint.value;
		minValue = datapoint.value;	
		maxTime = datapoint.timestamp;
		minTime = datapoint.timestamp;
	} else {
		if(datapoint.value > maxValue){
			maxValue = datapoint.value;
		}
		if(datapoint.value < minValue){
			minValue = datapoint.value;
		}
		
		if(datapoint.timestamp > maxTime){
			maxTime = datapoint.timestamp;
		}
		if(datapoint.timestamp < minTime){
			minTime = datapoint.timestamp;
		}
	}
}

- (BOOL) commandValue: (NSString*)newVal {

	const char * valStr = [newVal UTF8String];
	BOOL ok = NO;
	
	if(valStr){
		if(0 == bionet_set_resource_by_name([name UTF8String], valStr)){
			ok = YES;
		}
	}

	return ok;
	
#if 0
	// Do our own type checking...
	BOOL typeOk = NO;	
	BOOL boundsOk = NO;
	NSScanner * scanner = [NSScanner localizedScannerWithString:newVal];
		
	union {
		long long ll;
		double d;
		char * s;
	} val;
	
	/* Parse the string */
	switch(type){
		case BIONET_RESOURCE_DATA_TYPE_UINT8:
		case BIONET_RESOURCE_DATA_TYPE_UINT16:
		case BIONET_RESOURCE_DATA_TYPE_UINT32:
		{
			long long val;
			typeOk = [scanner scanLongLong:&val.ll];
			if(val < 0){
				typeOk = NO;
			}
			break;
		}
		case BIONET_RESOURCE_DATA_TYPE_INT8:
		case BIONET_RESOURCE_DATA_TYPE_INT16:
		case BIONET_RESOURCE_DATA_TYPE_INT32:
		{
			typeOk = [scanner scanLongLong:&val.ll];
			break;
		}
		case BIONET_RESOURCE_DATA_TYPE_FLOAT:
		case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
		{
			typeOk = [scanner scanDouble:&val.d];
			break;
		}
			
		case BIONET_RESOURCE_DATA_TYPE_STRING:
		{
			val.s = [newVal cString];
			break;
		}
			
		default:
			break;
	}
	
	if(typeOk){
		/* Type was close to correct type, check bounds */
		switch(type){
			case BIONET_RESOURCE_DATA_TYPE_UINT8:
				if(val.ll <= UCHAR_MAX){
					bionet_resource_set_uint8(c_resource, val.ll, ts);
					boundsOk = YES;
				}
				break;
				
			case BIONET_RESOURCE_DATA_TYPE_UINT16:
				if(val.ll <= USHRT_MAX){
					bionet_resource_set_uint16(c_resource, val.ll, ts);
					boundsOk = YES;
				}
				break;
				
			case BIONET_RESOURCE_DATA_TYPE_UINT32:
				if(val.ll <= UINT_MAX){
					bionet_resource_set_uint32(c_resource, val.ll, ts);
					boundsOk = YES;
				}
				break;
				
			case BIONET_RESOURCE_DATA_TYPE_INT8:
				if(val.ll <= CHAR_MAX && val.ll >= CHAR_MIN ){
					bionet_resource_set_int8(c_resource, val.ll, ts);
					boundsOk = YES;
				}
				break;
				
			case BIONET_RESOURCE_DATA_TYPE_INT16:
				if(val.ll <= SHRT_MAX && val.ll >= SHRT_MIN ){
					bionet_resource_set_int16(c_resource, val.ll, ts);
					boundsOk = YES;
				}
				break;
				
			case BIONET_RESOURCE_DATA_TYPE_INT32:
				if(val.ll <= INT_MAX && val.ll >= INT_MIN ){
					bionet_resource_set_int32(c_resource, val.ll, ts);
					boundsOk = YES;
				}
				break;
				
			case BIONET_RESOURCE_DATA_TYPE_FLOAT:
				bionet_resource_set_float(c_resource, val.d, ts);
				boundsOk = YES;
				break;
				
			case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
				bionet_resource_set_double(c_resource, val.d, ts);
				boundsOk = YES;
				break;				
				
			case BIONET_RESOURCE_DATA_TYPE_STRING:
				bionet_resource_set_str(c_resource, val.s, ts);
				boundsOk = YES;
				break;
				
			default:
				break;
		}
	
		if(boundsOk){
			
		} else {
			// Display Alert: "Value out of bounds"
	} else {
		// Display Alert: "Value malformed or wrong type"
	}
#endif
	
}

- (void) dealloc {
	[ident release];
	[name release];
	[localName release];
	[habName release];
	[valueStr release];
	[dataType release];
	[dataPoints release];
	
	[super dealloc];
}


@end
