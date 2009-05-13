//
//  GraphViewController.h
//  BioNetMonitor
//
//  Created by Brian Marshall on 5/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GraphView.h";


@interface GraphViewController : UIViewController {
	IBOutlet UIScrollView *scrollView;
	IBOutlet UIView * viewToScroll;
	IBOutlet GraphView * graphView;
}

- (void)viewDidLoad;

@property (nonatomic, retain) UIScrollView *scrollView;
@property (nonatomic, retain) GraphView *graphView;
@property (nonatomic, retain) UIView *viewToScroll;

@end
