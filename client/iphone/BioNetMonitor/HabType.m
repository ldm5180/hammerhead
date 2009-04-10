//
//  HabType.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "HabType.h"


@implementation HabType

@synthesize ident, habs;

- (HabType*) init {
	if(self = [super init]){
		habs = [[NSMutableDictionary alloc] init];
	}
	return self;
}

- (NSComparisonResult)compare:(HabType *)toType {
	if(toType == self){
		return NSOrderedSame;
	}
	return [ident localizedCompare:toType.ident];
}

@end
