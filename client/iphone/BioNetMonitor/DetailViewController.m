/*

File: DetailViewController.m
Abstract: Creates a grouped table view to act as an inspector.

Version: 2.6

Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc.
("Apple") in consideration of your agreement to the following terms, and your
use, installation, modification or redistribution of this Apple software
constitutes acceptance of these terms.  If you do not agree with these terms,
please do not use, install, modify or redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and subject
to these terms, Apple grants you a personal, non-exclusive license, under
Apple's copyrights in this original Apple software (the "Apple Software"), to
use, reproduce, modify and redistribute the Apple Software, with or without
modifications, in source and/or binary forms; provided that if you redistribute
the Apple Software in its entirety and without modifications, you must retain
this notice and the following text and disclaimers in all such redistributions
of the Apple Software.
Neither the name, trademarks, service marks or logos of Apple Inc. may be used
to endorse or promote products derived from the Apple Software without specific
prior written permission from Apple.  Except as expressly stated in this notice,
no other rights or licenses, express or implied, are granted by Apple herein,
including but not limited to any patent rights that may be infringed by your
derivative works or by other works in which the Apple Software may be
incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR
DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF
CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Copyright (C) 2008 Apple Inc. All Rights Reserved.

*/

#import "DetailViewController.h"
#import "GraphViewController.h"
#import "GraphView.h"
#import "ResourceDetailCell.h"
#import "BionetManager.h"


@implementation DetailViewController

@synthesize resList, subTitle, hab_type_filter, hab_id_filter, node_id_filter, resource_id_filter;

- (id)initWithStyle:(UITableViewStyle)style {
	if (self = [super initWithStyle:style]) {
		self.title = NSLocalizedString(@"Node Resources", @"Resources");
		self.tableView.rowHeight = kResourceDetailRowHeight;
		
		dateFormater = [[NSDateFormatter alloc] init];
		[dateFormater setDateStyle:NSDateFormatterShortStyle];
		[dateFormater setTimeStyle:NSDateFormatterMediumStyle];
	}
	return self;
}

- (void)viewWillAppear:(BOOL)animated {
    // Update the view with current data before it is displayed
    [super viewWillAppear:animated];
    
    // Scroll the table view to the top before it appears
    [self.tableView reloadData];
	
	self.resource_id_filter = @"*";

	graphViewController = nil;
}

// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait 
			|| interfaceOrientation == UIInterfaceOrientationLandscapeLeft
			|| interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

- (void)updateItem: (Resource*)theItem {
	if(graphViewController){
		[graphViewController.graphView updateItem:theItem];
		return;
	}
	int i;
	for(i=0;i<resList.count; i++){
		Resource * res = (Resource*)[resList objectAtIndex:i];
		if(res == theItem){
			[self.tableView reloadData];
			//UITableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPaths];
			//[cell setSelected:YES animated:YES];
			break;
		}
	}
}


#pragma mark Table view methods
// Standard table view data source and delegate methods
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	return 1;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    return subTitle;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {    
	return [resList count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"ResCell";
    ResourceDetailCell *cell = (ResourceDetailCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
		CGRect startingRect = CGRectMake(0.0, 0.0, 320.0, kResourceDetailRowHeight);
        cell = [[[ResourceDetailCell alloc] initWithFrame:startingRect reuseIdentifier:CellIdentifier] autorelease];
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
    }

	Resource  *res = nil;
	if(indexPath.row < resList.count){
		res = (Resource*)[resList objectAtIndex:indexPath.row];
	}

	cell.res = res;
	cell.habName.text = res.habName;
	cell.localName.text = res.ident;
	cell.dataType.text = res.dataType;
	cell.writable = (res.flavor >= BIONET_RESOURCE_FLAVOR_ACTUATOR);
	
	cell.timestamp.text = [dateFormater stringFromDate:res.timestamp]; 

	cell.value.text = res.valueStr;
	cell.value.delegate = self;
	if(res.isNumeric){
		cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
	}
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	Resource * resource = nil;
	if(indexPath.row < resList.count){
		resource = (Resource*)[resList objectAtIndex:indexPath.row];
	}	

	if(resource && resource.isNumeric){
		graphViewController = [[GraphViewController alloc] init];
		graphViewController.title = [resource name];

		NSArray*    topLevelObjs = nil;
		topLevelObjs = [[NSBundle mainBundle] loadNibNamed:@"GraphViewController" owner:graphViewController	options:nil];
			
		[graphViewController.graphView setResource: resource];
		
		[[self navigationController] pushViewController:graphViewController animated:YES];
	}
}

#pragma mark Edit control

- (BOOL) textFieldShouldReturn: (UITextField *)textField {
	ResourceDetailCell * cell = (ResourceDetailCell*)textField.superview.superview;
	Resource * res = cell.res;
	if(res && cell.writable){
		if([res commandValue:textField.text]){
			[textField resignFirstResponder];	
		}
		return NO;
	}
	// Should never happen, but go away in case
	[textField resignFirstResponder];
	return NO;
}

- (void)textFieldDidBeginEditing:(UITextField *)textField {
	ResourceDetailCell * cell = (ResourceDetailCell*)textField.superview.superview;	
	self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem: UIBarButtonSystemItemCancel target: cell action: @selector( cancelEditing )];
}

- (void)textFieldDidEndEditing:(UITextField *)textField {
	self.navigationItem.rightBarButtonItem = nil;
}

- (BOOL) textFieldShouldEndEditing: (UITextField *)textField {
	//ResourceDetailCell * cell = (ResourceDetailCell*)textField.superview.superview;	
	return YES;
}

- (BOOL) textFieldShouldBeginEditing: (UITextField *)textField {
	ResourceDetailCell * cell = (ResourceDetailCell*)textField.superview.superview;	
	if(cell.writable){
		return YES;
	} else {
		return NO;
	}
}

@end
