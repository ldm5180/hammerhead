//
//  DataPoint.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 5/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "DataPoint.h"


@implementation DataPoint

@synthesize value, timestamp;

- (DataPoint*) initWithPtr: (bionet_datapoint_t *) c_datapoint {
	if(self = [super init]){
			
		bionet_value_t * c_value = bionet_datapoint_get_value(c_datapoint);
		bionet_resource_t * c_resource = bionet_value_get_resource(c_value);
		bionet_resource_data_type_t type = bionet_resource_get_data_type(c_resource);
		
		struct timeval * tv = bionet_datapoint_get_timestamp(c_datapoint);
		timestamp = (double)tv->tv_sec + ((double)tv->tv_usec * 1.0e-6);
						
		value = 0;
		switch(type){
			case BIONET_RESOURCE_DATA_TYPE_UINT8:
			{
				uint8_t val;
				if( 0 == bionet_value_get_uint8(c_value, &val)){
					value = val;
				}
				break;
			}
				
			case BIONET_RESOURCE_DATA_TYPE_INT8:
			{
				int8_t val;
				if( 0 == bionet_value_get_int8(c_value, &val)){
					value = val;
				}
				break;
			}
				
			case BIONET_RESOURCE_DATA_TYPE_UINT16:
			{
				uint16_t val;
				if( 0 == bionet_value_get_uint16(c_value, &val)){
					value = val;
				}
				break;
			}
				
			case BIONET_RESOURCE_DATA_TYPE_INT16:
			{
				int16_t val;
				if( 0 == bionet_value_get_int16(c_value, &val)){
					value = val;
				}
				break;
			}
				
			case BIONET_RESOURCE_DATA_TYPE_UINT32:
			{
				uint32_t val;
				if( 0 == bionet_value_get_uint32(c_value, &val)){
					value = val;
				}
				break;
			}
				
			case BIONET_RESOURCE_DATA_TYPE_INT32:
			{
				int32_t val;
				if( 0 == bionet_value_get_int32(c_value, &val)){
					value = val;
				}
				break;
			}
				
			case BIONET_RESOURCE_DATA_TYPE_FLOAT:
			{
				float val;
				if( 0 == bionet_value_get_float(c_value, &val)){
					value = val;
				}
				break;
			}
				
			case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
			{
				double val;
				if( 0 == bionet_value_get_double(c_value, &val)){
					value = val;
				}
				break;
			}
			default:
				break;
		}
	}
		
	return self;	
}


@end
