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



@implementation DetailViewController

@synthesize sectionList, subTitle, hab_type_filter, hab_id_filter, node_id_filter;


- (void)viewWillAppear:(BOOL)animated {
    // Update the view with current data before it is displayed
    [super viewWillAppear:animated];
    
    // Scroll the table view to the top before it appears
    [self.tableView reloadData];
    [self.tableView setContentOffset:CGPointZero animated:NO];
	self.title = NSLocalizedString(@"Node Resources", @"Resources");
}

// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait 
			|| interfaceOrientation == UIInterfaceOrientationLandscapeLeft
			|| interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

// Standard table view data source and delegate methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    NSInteger sections;
	sections = [sectionList count];
	if(sections < 1){
		sections = 1;
	}
    return sections;
}


- (void)updateItem: (NSDictionary*)theItem {
	int i;
	NSString * newTitle = [theItem objectForKey:@"title"];
	for(i=0;i<sectionList.count; i++){
		NSMutableDictionary *sectionDict = [sectionList objectAtIndex:i];
		NSString *title = [sectionDict objectForKey:@"title"];
		if([newTitle isEqualToString:title]){
			[sectionDict setObject:[theItem objectForKey:@"cells"] forKey:@"cells"];
			NSArray * indexPaths = [NSArray arrayWithObject:[NSIndexPath indexPathForRow:i+1 inSection: 0]];
			[self.tableView reloadData];
			//UITableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPaths];
			//[cell setSelected:YES animated:YES];
			break;
		}
	}
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    
    NSInteger rows = 0;
	if(section < sectionList.count){
		rows = [[[sectionList objectAtIndex:section] objectForKey:@"cells"] count];
	}
    return rows;
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"tvc";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
    }

    NSString *cellText = nil;
	if(indexPath.section < sectionList.count){
		NSDictionary* sectionDict = [sectionList objectAtIndex:indexPath.section];
		NSArray * cellList = [sectionDict objectForKey:@"cells"];
		
		// Set the text in the cell for the section/row
		if(indexPath.row < cellList.count){
			cellText = [cellList objectAtIndex:indexPath.row];
		}
	}	
	cell.text = cellText;
    return cell;
}


/*
 Provide section titles
 HIG note: In this case, since the content of each section is obvious, there's probably no need to provide a title, but the code is useful for illustration.
 */
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
 
	NSString * title = nil;
	if(section < sectionList.count){
		NSDictionary* sectionDict = [sectionList objectAtIndex:section];
		title = [sectionDict objectForKey:@"title"];
	}
    return title;
}


@end
