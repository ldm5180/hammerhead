//
//  GraphViewController.m
//  BioNetMonitor
//
//  Created by Brian Marshall on 5/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "GraphViewController.h"


@implementation GraphViewController

@synthesize scrollView, viewToScroll, graphView;


/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
	
	[scrollView setContentSize:CGSizeMake(viewToScroll.frame.size.width, viewToScroll.frame.size.height)];
}

- (UIView *)viewForZoomingInScrollView: (UIScrollView *)theScrollView {
	if (theScrollView == scrollView) {
		return viewToScroll;
	}
	return nil;
}

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


- (void)dealloc {
	self.scrollView = nil;
	self.graphView = nil;
	self.viewToScroll = nil;
	
    [super dealloc];
}


@end
