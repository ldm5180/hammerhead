//
//  HabType.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface HabType : NSObject {
	NSString * ident;
	NSMutableDictionary *habs;
}

@property (retain, readwrite) NSString * ident;
@property (retain, readwrite) NSMutableDictionary *habs;

- (NSComparisonResult)compare:(HabType *)toType;

@end
