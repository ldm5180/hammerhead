//
//  Hab.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Hab.h"


@implementation Hab

@synthesize c_hab;

-(Hab*) init {
	if(self = [super init]){
		c_hab = NULL;
	}
	return self;
}

- (Hab*) initWithPtr: (bionet_hab_t *)ptr {
	if(self = [super init]){
		c_hab = ptr;
		bionet_hab_set_user_data(c_hab, self);
	}
	return self;	
}

+ (Hab*) createWrapper: (bionet_hab_t *)ptr {
	Hab * obj = nil;
	obj = (Hab*)bionet_hab_get_user_data(ptr);
	if(obj){
		[obj retain];
	}else{
		obj = [[Hab alloc] initWithPtr: ptr];
	}
	return obj;
}
		
- (void)dealloc {
	if(c_hab){
		id obj = bionet_hab_get_user_data(c_hab);
		if(obj == self){
			bionet_hab_set_user_data(c_hab, NULL);
		}
		c_hab = NULL;
	}
	[ident release];
	[name release];
	
    [super dealloc];
}

- (void) wentAway {
	if(c_hab){
		id obj = bionet_hab_get_user_data(c_hab);
		if(obj == self){
			bionet_hab_set_user_data(c_hab, NULL);
		}
		c_hab = NULL;
	}
	c_hab = NULL;
}

- (NSComparisonResult)compare:(Hab *)toHab {
	if(self == toHab){
		return NSOrderedSame;
	}
	return [self.name localizedCompare:toHab.name];
}

- (NSString*) ident {
	if(ident == nil){
		const char * c_id = bionet_hab_get_id(c_hab);
		ident = [[NSString alloc] initWithCString:c_id];
	}
	return ident;
}

- (NSString*) name {
	if(name == nil){
		const char * c_id = bionet_hab_get_id(c_hab);
		name = [[NSString alloc] initWithCString:c_id];
	}
	return name;
}

- (BOOL) isSecure {
	if(c_hab) {
	return bionet_hab_is_secure(c_hab);
	}
	return NO;
}
@end
