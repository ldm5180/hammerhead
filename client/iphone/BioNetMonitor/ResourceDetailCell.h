//
//  ResourceDetailCell.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 5/8/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ResourceDetailView;
@class Resource;

#define kNameHeight 30
//#define kSubHeight 20
#define kSubHeight 0
#define kValueHeight 24
#define kResourceDetailRowHeight (kNameHeight + kSubHeight + kValueHeight + 15)

@interface ResourceDetailCell : UITableViewCell {
	Resource * res;
	UITextField * habName;
	UITextField * localName;
	UITextField	* dataType;
	UITextField	* value;
	UITextField	* timestamp;
	bool writable;
}

@property (nonatomic, retain) Resource * res;
@property (nonatomic, retain) UITextField * habName;
@property (nonatomic, retain) UITextField * localName;
@property (nonatomic, retain) UITextField * dataType;
@property (nonatomic, retain) UITextField * value;
@property (nonatomic, retain) UITextField * timestamp;
@property (nonatomic) bool writable;

- (void) cancelEditing;

@end
