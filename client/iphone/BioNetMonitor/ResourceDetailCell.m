//
//  ResourceDetailCell.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 5/8/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "ResourceDetailCell.h"
#import "Resource.h"

@implementation ResourceDetailCell

@synthesize res, habName, localName, value, timestamp, dataType, writable;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier {
    if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier]) {
		// Initialize the labels, their fonts, colors, alignment, and background color.
        localName = [[UILabel alloc] initWithFrame:CGRectZero];
        localName.font = [UIFont boldSystemFontOfSize:20];
        localName.backgroundColor = [UIColor clearColor];
		
        habName = [[UILabel alloc] initWithFrame:CGRectZero];
        habName.font = [UIFont boldSystemFontOfSize:12];
        habName.textColor = [UIColor grayColor];
        habName.backgroundColor = [UIColor clearColor];
				
        dataType = [[UILabel alloc] initWithFrame:CGRectZero];
        dataType.font = [UIFont boldSystemFontOfSize:12];
        dataType.textColor = [UIColor darkGrayColor];
        dataType.backgroundColor = [UIColor clearColor];
		dataType.textAlignment = UITextAlignmentRight;

        value = [[UITextField alloc] initWithFrame:CGRectZero];
        value.font = [UIFont boldSystemFontOfSize:14];
        value.textColor = [UIColor blackColor];
        value.backgroundColor = [UIColor clearColor];
		//value.borderStyle = UITextBorderStyleRoundedRect;		
		//value.keyboardAppearance = UIKeyboardAppearanceAlert;
		value.returnKeyType = UIReturnKeySend;
		value.clearButtonMode = UITextFieldViewModeWhileEditing;
		value.keyboardType = UIKeyboardTypeDefault;
		
        timestamp = [[UILabel alloc] initWithFrame:CGRectZero];
        timestamp.font = [UIFont boldSystemFontOfSize:14];
        timestamp.textColor = [UIColor blackColor];
        timestamp.backgroundColor = [UIColor clearColor];
		//timestamp.textAlignment = UITextAlignmentRight;

		
		// Add the labels to the content view of the cell.
        // Important: although UITableViewCell inherits from UIView, you should add subviews to its content view
        // rather than directly to the cell so that they will be positioned appropriately as the cell transitions 
        // into and out of editing mode.        
        [self.contentView addSubview:localName];
        //[self.contentView addSubview:habName];
        [self.contentView addSubview:dataType];
        [self.contentView addSubview:timestamp];
        [self.contentView addSubview:value];
		self.autoresizesSubviews = NO;
    }
    return self;
}


- (void)layoutSubviews {
    [super layoutSubviews];
    // Start with a rect that is inset from the content view by 10 pixels on all sides.
    CGRect baseRect = CGRectInset(self.contentView.bounds, 10, 10);
    CGRect rect = baseRect;
	
    // Position each label with a modified version of the base rect.
	rect.size.height = kNameHeight;
	rect.size.width = baseRect.size.width - 70;
	localName.frame = rect;

	/*
    rect.origin.y += kNameHeight;
	rect.size.height = kSubHeight;
    rect.size.width = baseRect.size.width - 70;
    habName.frame = rect;
	 */
	
    rect.origin.x += rect.size.width + 10;
    rect.size.width = baseRect.size.width - rect.origin.x;
    dataType.frame = rect;

    rect.origin.y += kNameHeight;
	rect.size.height = kValueHeight;
	rect.origin.x = baseRect.origin.x + 20;
    rect.size.width = ((baseRect.size.width - baseRect.origin.x) * 0.5) - 10;
    timestamp.frame = rect;

	rect.origin.x = rect.size.width + rect.origin.x + 10;
    rect.size.width = baseRect.size.width - rect.origin.x;
    value.frame = rect;
	
	
	if(writable){
		value.borderStyle = UITextBorderStyleRoundedRect;		
	} else {
		value.borderStyle = UITextBorderStyleNone;		
	}

}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {

    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void) cancelEditing {
	self.value.text = res.valueStr;
	[self.value resignFirstResponder];
}

- (void)dealloc {
	[habName release];
	[localName release];
	[value release];
	[dataType release];
	[res release];
    [super dealloc];
}


@end
