//
//  Node.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Node.h"
#import "Resource.h"
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
		bionet_node_set_user_data(ptr, (void*)obj);
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

- (NSMutableArray*) contentsAsArrayOfResources {
	NSMutableArray * resList = [[NSMutableArray alloc] init];
	
	unsigned int i;

	@synchronized([BionetManager getInstance]){
		if(c_node){
			for (i = 0; i < bionet_node_get_num_resources(c_node); i++) {
				bionet_resource_t *c_resource = bionet_node_get_resource_by_index(c_node, i);
				Resource * resource = [Resource createWrapper:c_resource];
				[resList addObject:resource];
			}
		}
	}
	
	return [resList autorelease];
}

@end
