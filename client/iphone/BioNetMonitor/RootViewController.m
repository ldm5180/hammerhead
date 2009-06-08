//
//  RootViewController.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 3/6/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "RootViewController.h"
#import "BioNetMonitorAppDelegate.h"
#import "DetailViewController.h"

#include "Node.h"


#define addFakeItems 0
@implementation RootViewController

@synthesize list, bionetManager, subTitle, hab_type_filter, hab_id_filter, node_id_filter, resource_id_filter, hab_secure;


- (void)viewDidLoad {
    [super viewDidLoad];
	
	// Obtain refrence to singleton bionet manager
	self.bionetManager = [BionetManager getInstance];
	
	
	switch(depth){
		case 0:
			self.title = NSLocalizedString(@"HAB Types", @"Hab Types");
			subTitle = nil;
			bionetManager.habTypeController = self;

			// Create the list
			NSMutableArray *theList = [[NSMutableArray alloc] init];
			self.list = theList;
			[theList release];
			
			break;
		case 1:
			self.title = NSLocalizedString(@"HAB List", @"Hab List");
			bionetManager.habController = self;
			self.list = [bionetManager genHabListWithTypeFilter:hab_type_filter];
			[self.list retain];
			//[bionetManager populateHabs];
			break;
		case 2:
			self.title = NSLocalizedString(@"Node List", @"Node List");
			bionetManager.nodeController = self;
			//[bionetManager populateNodes: ];
			self.list = [bionetManager genNodeListWithNodeFilter:@"*"	
												   habTypeFilter:hab_type_filter 
													 habIdFilter:hab_id_filter];
			[self.list retain];
			break;
		default:
			break;
	}
	
}

/*
- (void)viewDidAppear: (BOOL)animated {
    [super viewDidAppear: animated];
	
	switch(depth){
		case 0:
				break;
		case 1:
			[bionetManager populateHabs];
			break;
		case 2:
			[bionetManager populateNodesWithTypeFilter: hab_type_filter habIdFilter: hab_id_filter nodeIdFilter: node_id_filter ];
			break;
		default:
			break;
	}
}
*/

- (void) setDepth: (int)theDepth {
	depth = theDepth;
}

/*
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}
*/
/*
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}
*/
/*
- (void)viewWillDisappear:(BOOL)animated {
	[super viewWillDisappear:animated];
}
*/
/*
- (void)viewDidDisappear:(BOOL)animated {
	[super viewDidDisappear:animated];
}
*/


// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait 
			|| interfaceOrientation == UIInterfaceOrientationLandscapeLeft
			|| interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void) newItem: (id) theItem {
	int i;
	UITableView * tableView = [self valueForKey:@"tableView"];
	
	for(i=0;i<list.count; i++){
		NSString * title = [[list objectAtIndex:i] ident];
		NSComparisonResult cmpResult = [[theItem ident] localizedCompare:title];
		if(NSOrderedSame== cmpResult){
			return;
		} else if (NSOrderedAscending == cmpResult) {
			// Goes before this item. Insert here
			[tableView beginUpdates];

			[list insertObject:theItem atIndex:i];
			NSArray * indexPaths = [NSArray arrayWithObject:[NSIndexPath indexPathForRow:i+addFakeItems inSection: 0]];	
			[tableView insertRowsAtIndexPaths: indexPaths withRowAnimation:UITableViewRowAnimationLeft];
			
			[tableView endUpdates];
			//[self reloadTable:self];
			return;
		}
	}
	
	// Goes at end of list
	[tableView beginUpdates];

	[list addObject:theItem];	
	NSArray * indexPaths = [NSArray arrayWithObject:[NSIndexPath indexPathForRow:i+addFakeItems inSection: 0]];	
	[tableView insertRowsAtIndexPaths: indexPaths withRowAnimation:UITableViewRowAnimationFade];
	
	[tableView endUpdates];
	//[self reloadTable:self];
	return;
}

- (void) delItem: (id) theItem {
	int i;
	UITableView * tableView = [self valueForKey:@"tableView"];
	for(i=0; i<list.count; i++){
		if(NSOrderedSame == [theItem compare:[list objectAtIndex: i]]){
		//if(theItem == [list objectAtIndex:i]){
			[tableView beginUpdates];

			[list removeObjectAtIndex:i];
			NSArray * indexPaths = [NSArray arrayWithObject:[NSIndexPath indexPathForRow:i+addFakeItems inSection: 0]];	
			[tableView deleteRowsAtIndexPaths: indexPaths withRowAnimation:UITableViewRowAnimationFade];			
			[tableView endUpdates];
			break;
		}
	}
}			
/*
// Custom set accessor to ensure the new list is mutable
- (void)setList:(NSMutableArray *)newList {
    if (list != newList) {
        [list release];
        list = [newList mutableCopy];
    }
}
 */

- (unsigned)countOfList {
    return [list count];
}

- (id)objectInListAtIndex:(unsigned)theIndex {
	if(theIndex < list.count){
		return [list objectAtIndex:theIndex];
	} else {
		return nil;
	}
}

			/*
- (void)tableView:(UITableView *)tableView observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	[tableView reloadData];
}
			 */

- (void) reloadTable:(id)sender {
	UITableView * tableView = [self valueForKey:@"tableView"];
	[tableView reloadData];
}

#pragma mark Table view methods



- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	return 1;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    return subTitle;
}

// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	return [list count] + addFakeItems;
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {

	
	NSString * CellIdentifier;
	if(depth >= 1){
		CellIdentifier = @"rvcLock";
	}else{
		CellIdentifier = @"rvcOther";
	}
		

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
		cell.font = [UIFont boldSystemFontOfSize:18];
		if(depth >= 1){
			// Add sub-views
			UILabel *label;

			CGRect rectCell = CGRectInset(cell.bounds, 10, 10);

			// Image
			CGRect rect = rectCell;			
			rect.size.width = rect.size.height;
			UIImageView *image = [[UIImageView alloc] initWithFrame:rect];
			image.tag = 0;
			[cell.contentView addSubview:image];
			[image release];
			
			// Name
			rect = rectCell;			
			rect.size.width -= (rect.size.height + 10);
			rect.origin.x += (rect.size.height + 10);

			label = [[UILabel alloc] initWithFrame:rect];
			label.tag = 1;
			label.font = [UIFont boldSystemFontOfSize:18];
			label.adjustsFontSizeToFitWidth = YES;
			[cell.contentView addSubview:label];
			[label release];
		}
    }
	
    
	// Get the object to display and set the value in the cell
	id itemAtIndex = nil;
	if(indexPath.row < list.count) {
#if addFakeItems
		if(indexPath.row == 0){
			cell.text = @"All";
		} else 
#endif
		{
			int i = indexPath.row;
			itemAtIndex = [list objectAtIndex:i - addFakeItems];
			if( depth >= 1){
				UILabel * label = (UILabel *)[cell viewWithTag:1];
				label.text = [itemAtIndex ident];

				UIImageView *image = (UIImageView *)[cell viewWithTag:0];
				if(hab_secure || (depth == 1 && [itemAtIndex isSecure])){
					image.image = [UIImage imageNamed:@"lock.png"];
				} else {
					image.image = [UIImage imageNamed:@"unlock.png"];
				}
				
			} else {
				cell.text = [itemAtIndex ident];
			}

			cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
			
		}
	}

    return cell;
}


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	/*
     Create the detail view controller and set its inspected item to the currently-selected item
     */
	UITableViewController *nextViewController;
	NSString *myTitle;
	id itemAtIndex = nil;
#if addFakeItems
	if(indexPath.row == 0){
		myTitle = @"*" ;
	} else 
#endif
	{
		itemAtIndex = [list objectAtIndex:indexPath.row - addFakeItems];
		myTitle = [itemAtIndex ident];
	}	
	if(depth < 2){
		RootViewController * rootViewController = [[RootViewController alloc] initWithStyle:UITableViewStylePlain];
		if(depth == 0){
			bionetManager.habController = rootViewController;
			rootViewController.hab_type_filter = myTitle;
			rootViewController.subTitle = myTitle;
		} else if(depth == 1){
			bionetManager.nodeController = rootViewController;
			rootViewController.hab_type_filter = hab_type_filter;
			rootViewController.hab_id_filter = myTitle;
			rootViewController.node_id_filter = @"*";
			rootViewController.subTitle = [NSString localizedStringWithFormat:@"%@.%@", hab_type_filter, myTitle];
			rootViewController.hab_secure = [itemAtIndex isSecure];
		}
		[rootViewController setDepth:depth+1];
		nextViewController = rootViewController;
		//nextViewController populate:
    } else {
		Node * node = itemAtIndex;
		DetailViewController *detailViewController = [[DetailViewController alloc] initWithStyle:UITableViewStylePlain];
		bionetManager.resourceController = detailViewController;
		detailViewController.resList = [node contentsAsArrayOfResources];
		//detailViewController.subTitle = node.ident;
		detailViewController.subTitle = [NSString localizedStringWithFormat:@"%@.%@.%@", hab_type_filter, hab_id_filter, node.ident];
		detailViewController.hab_type_filter = hab_type_filter;
		detailViewController.hab_id_filter = hab_id_filter;
		detailViewController.node_id_filter = myTitle;
		
		nextViewController = detailViewController;
	}
    
    // Push the detail view controller
    [[self navigationController] pushViewController:nextViewController animated:YES];
    //[nextViewController release];
}


/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/


/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:YES];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/


/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/


/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/



- (void)dealloc {
	switch(depth){
		case 0:
			bionetManager.habTypeController = nil;
			break;
		case 1:
			bionetManager.habController = nil;
			break;
		case 2:
			bionetManager.nodeController = nil;
			break;
		default:
			break;
	}
	[list release];
    [super dealloc];
}


@end

