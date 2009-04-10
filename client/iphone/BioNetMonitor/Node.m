//
//  Node.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Node.h"
#import "BionetManager.h"


@implementation Node

@synthesize c_node;

-(Node*) init {
	if(self = [super init]){
		c_node = NULL;
	}
	return self;
}

- (Node*) initWithPtr: (bionet_node_t *)ptr {
	if(self = [super init]){
		c_node = ptr;
	}
	return self;	
}

+ (Node*) createWrapper: (bionet_node_t *)ptr {
	Node * obj = (Node*)bionet_node_get_user_data(ptr);
	if(obj){
		[obj retain];
	}else{
		obj = [[Node alloc] initWithPtr: ptr];
	}
	return obj;
}

-(void) dealloc {
	if(c_node){
		id obj = bionet_node_get_user_data(c_node);
		if(obj == self){
			bionet_node_set_user_data(c_node, NULL);
		}
		c_node = NULL;
	}
	[ident release];
	[name release];
	
    [super dealloc];	
}

- (void) wentAway {
	if(c_node){
		id obj = bionet_node_get_user_data(c_node);
		if(obj == self){
			bionet_node_set_user_data(c_node, NULL);
		} else {
			Debugger;
		}
	}
	c_node = NULL;
}

- (NSString*) ident {
	if(nil == ident && c_node){
		const char * c_id = bionet_node_get_id(c_node);
		ident = [[NSString alloc] initWithCString:c_id];
	}
	return ident;
}

- (NSString*) name {
	if(nil == name && c_node){
		const char * c_id = bionet_node_get_name(c_node);
		name = [[NSString alloc] initWithCString:c_id];
	}
	return name;
}

- (NSComparisonResult)compare:(Node *)toNode {
	if(self == toNode){
		return NSOrderedSame;
	}
	return [self.name localizedCompare:toNode.name];
}

- (NSMutableArray*) contentsAsArrayOfSections {
	NSMutableArray * sectionList = [[NSMutableArray alloc] init];
	
	unsigned int i;

	@synchronized([BionetManager getInstance]){
		if(c_node){
			for (i = 0; i < bionet_node_get_num_resources(c_node); i++) {
				bionet_resource_t *resource = bionet_node_get_resource_by_index(c_node, i);
				bionet_datapoint_t *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
				
				NSString * resourceName = [[NSString alloc] initWithCString: bionet_resource_get_name(resource)]; 
				NSString * dataPointStr = nil;
				if (datapoint == NULL) {
					dataPointStr = @"";
					
					/*g_message(
							  "        %s %s %s (no known value)", 
							  bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
							  bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
							  bionet_resource_get_id(resource)
							  );*/
				} else {
					char * value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));
					dataPointStr = [[NSString alloc] initWithCString: value_str];
					
					/*g_message(
							  "        %s %s %s = %s @ %s", 
							  bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
							  bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
							  bionet_resource_get_id(resource),
							  value_str,
							  bionet_datapoint_timestamp_to_string(datapoint)
							  );*/
					
					free(value_str);
				}
				NSMutableDictionary * sectionDict = [[NSMutableDictionary alloc] initWithObjectsAndKeys:resourceName, @"title", [NSMutableArray arrayWithObject: dataPointStr], @"cells", nil]; 
				[sectionList addObject:sectionDict];
				[sectionDict release];
			}
			
			if (bionet_node_get_num_streams(c_node)) {
				g_message("    Streams:");
				
				for (i = 0; i < bionet_node_get_num_streams(c_node); i++) {
					bionet_stream_t *stream = bionet_node_get_stream_by_index(c_node, i);
					g_message(
							  "        %s %s %s", 
							  bionet_stream_get_id(stream),
							  bionet_stream_get_type(stream),
							  bionet_stream_direction_to_string(bionet_stream_get_direction(stream))
							  );
				}
			}
		}
	}
	
	return [sectionList autorelease];
}

@end
