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
#import "Resource.h"
#import "DataPoint.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>
#include "bionet.h"
#include "hardware-abstractor.h"
#include "bionet-util.h"

#define SIZEOF(x) (sizeof(x)/sizeof(x[0]))

// Class variable to allow c callbacks access to the data modle object
static BionetManager * instanceRef = NULL;

void cb_lost_node(bionet_node_t *node);

void cb_datapoint(bionet_datapoint_t *c_datapoint) {
	NSAutoreleasePool *cbPool = [[NSAutoreleasePool alloc] init];
	BionetManager * sself = instanceRef;
	
    bionet_value_t * value = bionet_datapoint_get_value(c_datapoint);
	bionet_resource_t * c_resource = bionet_value_get_resource(value);
	
	if(sself.resourceController
	   && bionet_resource_matches_habtype_habid_nodeid_resourceid(c_resource, 
												   [[sself.resourceController hab_type_filter] UTF8String],
												   [[sself.resourceController hab_id_filter] UTF8String],
												   [[sself.resourceController node_id_filter] UTF8String],
												   [[sself.resourceController resource_id_filter] UTF8String]))
	{

		NSString * resourceId = [[NSString alloc] initWithCString:bionet_resource_get_id(c_resource)];

		Resource * resource = [sself.resourceDictionary  objectForKey:resourceId];
		if(nil == resource){
			resource = [Resource createWrapper: c_resource];
		}
		
		[resource pushDataPoint:c_datapoint];
		
		[[sself resourceController] performSelectorOnMainThread:@selector(updateItem:) withObject:resource waitUntilDone: YES];
	}
	
	[cbPool drain];
}

void cb_new_hab(bionet_hab_t *c_hab) {
	NSAutoreleasePool *cbPool = [[NSAutoreleasePool alloc] init];

	BionetManager * sself = instanceRef;
	
	const char * habTypeConst = bionet_hab_get_type(c_hab);
	const char * habIdConst   = bionet_hab_get_id(c_hab);
	NSString * habTypeId = [[NSString alloc] initWithCString:habTypeConst];
	NSString * habId = [[NSString alloc] initWithCString:habIdConst];
	NSMutableDictionary *habsDict = nil;
	Hab *hab = nil;
	
	// Update hab and habType dictionaries
	HabType *habType = [[sself rootDictionary] objectForKey:habTypeId];
	
	if(habType == nil){
		habType = [[HabType alloc] init];
		habType.ident = habTypeId;
		habsDict = habType.habs;

		[[sself rootDictionary] setObject:habType forKey:habTypeId];
		[[sself habTypeController] performSelectorOnMainThread:@selector(newItem:) withObject:habType waitUntilDone: YES];
		
		[habType release];
	} else {
		habsDict = habType.habs;
		hab = [habsDict objectForKey:habId];
	}

	if(nil == hab){
		hab = [Hab createWrapper:c_hab];
		[habsDict setObject:hab forKey: habId]; 
		
		if(sself.habController
		   && bionet_hab_matches_type_and_id(c_hab, [[sself.habController hab_type_filter] UTF8String], "*"))
		{
			[[sself habController] performSelectorOnMainThread:@selector(newItem:) withObject:hab waitUntilDone: YES];
		}
		
		//[[sself habController] performSelectorOnMainThread:@selector(newItem:) withObject:hab waitUntilDone: YES];
		[hab release];
	}
	
	[habId release];
	[habTypeId release];
	

	g_message("new hab: %s", bionet_hab_get_name(c_hab));
	[cbPool drain];

}

void cb_lost_hab(bionet_hab_t *c_hab) {
	NSAutoreleasePool *cbPool = [[NSAutoreleasePool alloc] init];

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
			[[sself rootDictionary] removeObjectForKey:habTypeId];
		}
	}
	[habTypeId release];
	[habId release];
	
    g_message("lost hab: %s", bionet_hab_get_name(c_hab));
	[cbPool drain];

}



void cb_new_node(bionet_node_t *c_node) {
	NSAutoreleasePool *cbPool = [[NSAutoreleasePool alloc] init];

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
	[cbPool drain];

}

void cb_lost_node(bionet_node_t *c_node) {
	NSAutoreleasePool *cbPool = [[NSAutoreleasePool alloc] init];

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
	[cbPool drain];

}





@implementation BionetManager

@synthesize rootDictionary, habTypeController, habController, nodeController, resourceController, resourceDictionary;

static void cleanUTF8String(char * str) {
	char *p = str;
	char *w = str;
	while(*p != '\0'){
		if(0x80 & *p){
			// Drop multi-byte chars
			p++;
			continue;
		}
		*w = tolower(*p);
		if(!isalnum(*w)) {
			*w = '-';
		}
		p++;
		w++;
	}
	*w = '\0';
}

- (id)init {
    if (self = [super init]) {
		
		habTypeController = nil;
		habController = nil;
		nodeController = nil;

		bool hab_enabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"hab_enabled"];
		//bool hab_enabled = TRUE;
				
		rootDictionary = [[NSMutableDictionary alloc]init];
		resourceDictionary = [[NSMutableDictionary alloc]init];

		NSString *secpath = [[NSBundle mainBundle] pathForResource:@"sec-dir" ofType:nil];
		
		bool sec_required = [[NSUserDefaults standardUserDefaults] boolForKey:@"ssl_required"];
		bool hab_secure = [[NSUserDefaults standardUserDefaults] boolForKey:@"hab_secure"];
		//bool hab_secure = FALSE;

		NSString * status_msg = [[NSUserDefaults standardUserDefaults] stringForKey:@"status_msg"];

		
		bionet_init_security([secpath UTF8String], sec_required);
		
		if(hab_enabled){			
			UIDevice *dev = [UIDevice currentDevice];
			char * c_habtype = strdup([dev.model UTF8String]);
			char * c_hostname = strdup([dev.name UTF8String]);
			cleanUTF8String(c_habtype);
			cleanUTF8String(c_hostname);
			
			bionet_hab_t * c_hab = bionet_hab_new(c_habtype, c_hostname);
			if(NULL == c_hab) {
				fprintf(stderr, "error creating hab");
				exit(1);
			}
			
			if(hab_secure) {
				hab_init_security([secpath UTF8String], sec_required);
			}
			
			hab_fd = hab_connect(c_hab);
			if(hab_fd < 0) {
				fprintf(stderr, "error connecting to hab");
				exit(1);
			}
			
			hab_read();
			
			bionet_node_t * c_node = bionet_node_new(c_hab, "info");
			if (c_node == NULL) {
				fprintf(stderr, "Error creating node\n");
				exit(1);
			}
			
			const char * c_name = [dev.name UTF8String]; 			
			const char * c_sys_vers = [[NSString stringWithFormat:@"%@ %@", dev.systemName, dev.systemVersion] UTF8String];  
			const char * c_uuid = [[UIDevice currentDevice].uniqueIdentifier UTF8String];  			
			const char * c_status = [status_msg UTF8String];  			

			
			const char * vals[] = {
				c_name,
				c_sys_vers,
				c_uuid,
				c_status
			};
			
			const char * names[] = {
				"name",
				"system",
				"dev-id",
				"status"
			};
			
			int i;
			for(i=0;i<SIZEOF(names);i++){
				bionet_resource_t * c_resource = bionet_resource_new(c_node, 
											   BIONET_RESOURCE_DATA_TYPE_STRING, 
											   BIONET_RESOURCE_FLAVOR_SENSOR, names[i]);
				if (c_resource == NULL) {
					fprintf(stderr, "Error creating Resource\n");
					exit(1);
				}
				
				int r = bionet_node_add_resource(c_node, c_resource);
				if (r != 0) {
					fprintf(stderr, "Error adding Resource\n");
				}
				

				bionet_value_t * c_value;
				if(vals[i]){
					c_value =  bionet_value_new_str(c_resource, strdup(vals[i]));
				} else {
					c_value =  bionet_value_new_str(c_resource, "");
				}
				
				bionet_datapoint_t * c_datapoint = bionet_datapoint_new(c_resource, c_value, NULL);
				bionet_resource_add_datapoint(c_resource, c_datapoint);
			}
			
			if (bionet_hab_add_node(c_hab, c_node) != 0) {
				fprintf(stderr, "HAB failed to add Node\n");
				exit(1);
			}
			
			if (hab_report_new_node(c_node) != 0) {
				// printf("error reporting Node to the NAG: %s\n", hab_get_nag_error());
			}
			
			
			
			hab_read();
			
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
	int fdsel = bionet_fd>hab_fd?bionet_fd:hab_fd;
	
	while(isRunning) {
		NSAutoreleasePool *loopPool = [[NSAutoreleasePool alloc] init];
		int r;
		fd_set readers;
		
		FD_ZERO(&readers);
		FD_SET(bionet_fd, &readers);
		FD_SET(hab_fd, &readers);
		
		//struct timeval timeout = { 1, 0 } ;
		
		r = select(fdsel + 1, &readers, NULL, NULL, NULL);
		
		if ((r < 0) && (errno != EINTR)) {
			fprintf(stderr, "error from select: %s", strerror(errno));
			g_usleep(1000*1000);
			break;
		}
		
		@synchronized(self) {
			bionet_read();
			hab_read();
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
		[loopPool drain];
		
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
