//
//  BionetManager.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "BionetManager.h"
#import "RootViewController.h"
#import "Hab.h"
#import "HabType.h"
#import "Node.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>
#include "bionet.h"
#include "bionet-util.h"

// Class variable to allow c callbacks access to the data modle object
static BionetManager * instanceRef = NULL;

void cb_lost_node(bionet_node_t *node);

void cb_datapoint(bionet_datapoint_t *datapoint) {
	BionetManager * sself = instanceRef;
	
    bionet_value_t * value = bionet_datapoint_get_value(datapoint);
	bionet_resource_t * resource = bionet_value_get_resource(value);
	
	if(sself.resourceController
	   && bionet_resource_matches_habtype_habid_nodeid_resourceid(resource, 
												   [[sself.resourceController hab_type_filter] UTF8String],
												   [[sself.resourceController hab_id_filter] UTF8String],
												   [[sself.resourceController node_id_filter] UTF8String],
												   "*"))
	{


		NSString * resourceName = [[NSString alloc] initWithCString: bionet_resource_get_name(resource)]; 
		NSString * dataPointStr = nil;
		char * value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));
		dataPointStr = [[NSString alloc] initWithCString: value_str];
		NSDictionary * sectionDict = [[NSDictionary alloc] initWithObjectsAndKeys:resourceName, @"title", [NSArray arrayWithObject: dataPointStr], @"cells", nil]; 

		
		[[sself resourceController] performSelectorOnMainThread:@selector(updateItem:) withObject:sectionDict waitUntilDone: YES];
	}
	
	
    char * value_str = bionet_value_to_str(value);
	
    g_message(
			  "%s = %s %s %s @ %s",
			  bionet_resource_get_name(resource),
			  bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
			  bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
			  value_str,
			  bionet_datapoint_timestamp_to_string(datapoint)
			  );
	
    free(value_str);
}

void cb_new_hab(bionet_hab_t *c_hab) {

	BionetManager * sself = instanceRef;
	
	const char * habTypeConst = bionet_hab_get_type(c_hab);
	const char * habIdConst   = bionet_hab_get_id(c_hab);
	NSString * habTypeId = [[NSString alloc] initWithCString:habTypeConst];
	NSString * habId = [[NSString alloc] initWithCString:habIdConst];
	
	// Update hab and habType dictionaries
	HabType *habType = [[sself rootDictionary] objectForKey:habTypeId];
	
	if(habType == nil){
		habType = [[HabType alloc] init];
		habType.ident = habTypeId;
		NSMutableDictionary *habsDict = habType.habs;

		Hab *hab = [Hab createWrapper:c_hab];
		[habsDict setObject:hab forKey:habId];
		[[sself rootDictionary] setObject:habType forKey:habTypeId];
		
		[[sself habTypeController] performSelectorOnMainThread:@selector(newItem:) withObject:habType waitUntilDone: YES];
		//[[sself habTypeController] newItem:habTypeId]; // There is no actual habType c type, so we'll clean up when the last node of a type goes away
		[[sself habController] performSelectorOnMainThread:@selector(newItem:) withObject:hab waitUntilDone: YES];
		//id newItem = [[sself habController] newItem:habId];
		//if(newItem){
		//	bionet_hab_set_user_data(c_hab, newItem);
		//}
		[habType release];
		[hab release];
	} else {
		NSMutableDictionary *habsDict = habType.habs;
		Hab * hab = [habsDict objectForKey:habId];
		if(NULL == hab){
			hab = [Hab createWrapper:c_hab];
			[habsDict setObject:hab forKey: habId]; 
			
			[[sself habController] performSelectorOnMainThread:@selector(newItem:) withObject:hab waitUntilDone: YES];
			//id newItem = [[sself habController] newItem:habId];
			//if(newItem){
			//	bionet_hab_set_user_data(c_hab, newItem);
			//}		
			[hab release];
		}
	}
	
	[habId release];
	[habTypeId release];
	

	g_message("new hab: %s", bionet_hab_get_name(c_hab));
	
}

void cb_lost_hab(bionet_hab_t *c_hab) {
	BionetManager * sself = instanceRef;
	
	// Trigger cb_lost_nodes for each active node...
	unsigned int i;
	for(i=0;i<bionet_hab_get_num_nodes(c_hab); i++){
		bionet_node_t *c_node =  bionet_hab_get_node_by_index(c_hab, i);
		if(c_node){
			cb_lost_node(c_node);
		}
	}
	
	const char * habTypeConst = bionet_hab_get_type(c_hab);
	const char * habIdConst   = bionet_hab_get_id(c_hab);
	NSString * habTypeId = [[NSString alloc] initWithCString:habTypeConst];
	NSString * habId = [[NSString alloc] initWithCString:habIdConst];
	
	
	
	// Update habType dictionaries
	HabType * habType = [[sself rootDictionary] objectForKey:habTypeId];
	if(habType){
		NSMutableDictionary* habsDict = habType.habs;
		Hab * hab = [habsDict objectForKey:habId];
		if(hab){
			// Remove hab from View
			[[sself habController] performSelectorOnMainThread:@selector(delItem:) withObject:hab waitUntilDone: YES];
			[hab wentAway];
			[habsDict removeObjectForKey:habId];
		}			

		if(0 == [habsDict count]){
			// Remove type from View
			HabType * habType = [[sself rootDictionary] objectForKey:habTypeId];
			[[sself habTypeController] performSelectorOnMainThread:@selector(delItem:) withObject:habType waitUntilDone: YES];
			[habsDict removeObjectForKey:habId];
		}
	}
	[habTypeId release];
	[habId release];
	
    g_message("lost hab: %s", bionet_hab_get_name(c_hab));
}



void cb_new_node(bionet_node_t *c_node) {
	BionetManager * sself = instanceRef;
	
	//const char * nodeIdConst   = bionet_node_get_id(c_node);
	//NSString * nodeId = [[NSString alloc] initWithCString: nodeIdConst];
	
	if(sself.nodeController
	&& bionet_node_matches_habtype_habid_nodeid(c_node, 
												[[sself.nodeController hab_type_filter] UTF8String],
												[[sself.nodeController hab_id_filter] UTF8String],
												[[sself.nodeController node_id_filter] UTF8String]))
	{
		Node *node = (Node*)[Node createWrapper:c_node];
				
		[[sself nodeController] performSelectorOnMainThread:@selector(newItem:) withObject:node waitUntilDone: YES];
	}
	
	/*
    g_message("new node: %s", bionet_node_get_name(node));
	
    if (bionet_node_get_num_resources(node)) {
        g_message("    Resources:");
		
        for (i = 0; i < bionet_node_get_num_resources(node); i++) {
            bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
            bionet_datapoint_t *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
			
            if (datapoint == NULL) {
                g_message(
						  "        %s %s %s (no known value)", 
						  bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
						  bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
						  bionet_resource_get_id(resource)
						  );
            } else {
                char * value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));
				
                g_message(
						  "        %s %s %s = %s @ %s", 
						  bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
						  bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
						  bionet_resource_get_id(resource),
						  value_str,
						  bionet_datapoint_timestamp_to_string(datapoint)
						  );
				
				free(value_str);
            }
			
        }
    }
	
    if (bionet_node_get_num_streams(node)) {
        g_message("    Streams:");
		
        for (i = 0; i < bionet_node_get_num_streams(node); i++) {
            bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
            g_message(
					  "        %s %s %s", 
					  bionet_stream_get_id(stream),
					  bionet_stream_get_type(stream),
					  bionet_stream_direction_to_string(bionet_stream_get_direction(stream))
					  );
        }
    }
	 */
	 
}

void cb_lost_node(bionet_node_t *c_node) {
    g_message("lost node: %s", bionet_node_get_name(c_node));
	BionetManager * sself = instanceRef;
	
	//const char * nodeIdConst   = bionet_node_get_id(c_node);
	//NSString * nodeId = [[NSString alloc] initWithCString: nodeIdConst];
	
	//if(sself.nodeController
	//   && bionet_node_matches_habtype_habid_nodeid(c_node, 
	//											   [[sself.nodeController hab_type_filter] UTF8String],
	//											   [[sself.nodeController hab_id_filter] UTF8String],
	//											   [[sself.nodeController node_id_filter] UTF8String]))
	{
		Node *node = (Node*)[Node createWrapper:c_node];
		
		[[sself nodeController] performSelectorOnMainThread:@selector(delItem:) withObject:node waitUntilDone: YES];
		[node release];
	}
}





@implementation BionetManager

@synthesize rootDictionary, habTypeController, habController, nodeController, resourceController;

- (id)init {
    if (self = [super init]) {
		
		habTypeController = nil;
		habController = nil;
		nodeController = nil;

		
		//hab_type_filter = NULL;
		//hab_filter = NULL;
		//node_filter = NULL;
		
		//tableViewToUpdate = nil;
		
		rootDictionary = [[NSMutableDictionary alloc]init];

		NSString *secpath = [[NSBundle mainBundle] pathForResource:@"sec-dir" ofType:nil];
		if(0 == bionet_init_security([secpath UTF8String], 1)){
			fprintf(stderr, "Security enabled");
		}
		
		// this must happen before anything else
		bionet_fd = bionet_connect();
		if (bionet_fd < 0) {
			fprintf(stderr, "error connecting to Bionet");
			exit(1);
		}
		
		
		bionet_register_callback_new_hab(cb_new_hab);
		bionet_register_callback_lost_hab(cb_lost_hab);
		
		bionet_register_callback_new_node(cb_new_node);
		bionet_register_callback_lost_node(cb_lost_node);
		
		bionet_register_callback_datapoint(cb_datapoint);
		
		bionet_subscribe_hab_list_by_name("*.*");
		bionet_subscribe_node_list_by_name("*.*.*");
		bionet_subscribe_datapoints_by_name("*.*.*:*");
		
		if(instanceRef == NULL){
			instanceRef = self;
		}

		
		isRunning = YES;
		[self performSelectorInBackground:@selector(bionetEventLoop) withObject:nil];
		
	}
    return self;
}

- (NSMutableArray*) genNodeListWithNodeFilter: (NSString*)node_id_filter habTypeFilter:(NSString*)hab_type_filter habIdFilter:(NSString*)hab_id_filter 
{
	unsigned int i;
	NSMutableArray* list = [[NSMutableArray alloc]init];
	
	@synchronized(self){
		for(i=0; i<bionet_cache_get_num_habs(); i++){
			bionet_hab_t* c_hab = bionet_cache_get_hab_by_index(i);
			if(NULL == c_hab){
				break;
			}
			if(bionet_hab_matches_type_and_id(c_hab, 
											  [hab_type_filter UTF8String],
											  "*"))
			{
				unsigned int j;
				for(j=0; j<bionet_hab_get_num_nodes(c_hab); j++){
					bionet_node_t* c_node = bionet_hab_get_node_by_index(c_hab, j);
					if(NULL == c_node){
						break;
					}
					if(bionet_node_matches_habtype_habid_nodeid(c_node, 
																[hab_type_filter UTF8String], 
																[hab_id_filter UTF8String], 
																[node_id_filter UTF8String]))
					{
						Node *node = [Node createWrapper: c_node];
						[list addObject:node];
						[node release];
					}
				}
			}
		}
	}
	
	NSMutableArray *sortedArray = [BionetManager sortBionetObjects:list];
	[list release];
	return sortedArray;
}

- (NSMutableArray*) genHabListWithTypeFilter: (NSString*)hab_type_filter {
	unsigned int i;
	NSMutableArray* list = [[NSMutableArray alloc]init];
	@synchronized(self){
		for(i=0; i<bionet_cache_get_num_habs(); i++){
			bionet_hab_t* c_hab = bionet_cache_get_hab_by_index(i);
			if(NULL == c_hab){
				break;
			}
			if(bionet_hab_matches_type_and_id(c_hab, 
											  [hab_type_filter UTF8String],
											  "*"))
			{
				Hab * hab = [Hab createWrapper:c_hab];				
				[list addObject: hab];
				[hab release];
			}
		}
	}
	
	NSMutableArray *sortedArray = [BionetManager sortBionetObjects:list];
	[list release];
	return sortedArray;
}

+ (NSMutableArray*) sortBionetObjects:(NSArray*)unsortedArray {
	NSArray* sortDescriptors = [NSArray arrayWithObject:
								[[[NSSortDescriptor alloc] initWithKey:@"ident"	
															 ascending:YES 
															  selector:@selector(localizedCompare:)] autorelease]];
	NSArray* sortedArray = [unsortedArray sortedArrayUsingDescriptors:sortDescriptors];
	NSMutableArray*sortedMutableArray = [sortedArray mutableCopy];
	return [sortedMutableArray autorelease];
}

+ (BionetManager *) getInstance {
	if(NULL == instanceRef){
		instanceRef = [[BionetManager alloc] init];
	}
	return instanceRef;
}


- (void)shutdown {
	isRunning = NO;
}

- (void)bionetEventLoop {
	while(isRunning) {
		int r;
		fd_set readers;
		
		FD_ZERO(&readers);
		FD_SET(bionet_fd, &readers);
		
		r = select(bionet_fd + 1, &readers, NULL, NULL, NULL);
		
		if ((r < 0) && (errno != EINTR)) {
			fprintf(stderr, "error from select: %s", strerror(errno));
			g_usleep(1000*1000);
			break;
		}
		
		@synchronized(self) {
			bionet_read();
		}
		
		/*
		 NSMutableArray *habList = [[NSMutableArray alloc] init];
		 
		 unsigned int numHabs = bionet_cache_get_num_habs();
		 if(numHabs > 0){
		 unsigned int i;
		 for(i=0;i<numHabs; i++) {
		 bionet_hab_t * hab = bionet_cache_get_hab_by_index(i);
		 if(hab) {
		 NSMutableDictionary *dictionary;
		 NSMutableDictionary *characters;
		 NSDateComponents *dateComponents = [[NSDateComponents alloc] init];
		 NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
		 NSDate *date;
		 
		 characters = [[NSArray alloc] initWithObjects:@"Antony", @"Artemidorus", @"Brutus", @"Caesar", @"Calpurnia", @"Casca", @"Cassius", @"Cicero", @"Cinna", @"Cinna the Poet", @"Citizens", @"Claudius", @"Clitus", @"Dardanius", @"Decius Brutus", @"First Citizen", @"First Commoner", @"First Soldier", @"Flavius", @"Fourth Citizen", @"Lepidus", @"Ligarius", @"Lucilius", @"Lucius", @"Marullus", @"Messala", @"Messenger", @"Metellus Cimber", @"Octavius", @"Pindarus", @"Poet", @"Popilius", @"Portia", @"Publius", @"Second Citizen", @"Second Commoner", @"Second Soldier", @"Servant", @"Soothsayer", @"Strato", @"Third Citizen", @"Third Soldier", @"Tintinius", @"Trebonius", @"Varro", @"Volumnius", @"Young Cato", nil];
		 [dateComponents setYear:1599];
		 date = [calendar dateFromComponents:dateComponents];
		 
		 const char * habNameConst = bionet_hab_get_name(hab);
		 NSString *habName = [[NSString alloc] initWithCString:habNameConst];
		 dictionary = [[NSMutableDictionary alloc] initWithObjectsAndKeys:habName, @"title", characters, @"mainCharacters", date, @"date", @"Tragedy", @"genre", nil];
		 [habList addObject:dictionary];
		 [habName release];
		 [dictionary release];
		 [characters release];
		 }
		 }					
		 
		 //[self.nodeList release];
		 self.nodeList = habList;
		 [habList release];
		 }	
		 */
		
	}

}


- (void)dealloc {
	
	self.habTypeController = nil;
	self.habController = nil;
	self.nodeController = nil;
	[rootDictionary release];
	
    [super dealloc];
}

@end
