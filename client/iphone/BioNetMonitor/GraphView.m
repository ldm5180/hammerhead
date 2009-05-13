/*

File: GraphView.m
Abstract: This class is responsible for updating and drawing the accelerometer
history of values. The history is a circular buffer implementation, with a
pointer moving repeatedly through the buffer, resetting to zero each time it
reaches the end.

Version: 1.7

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

#import "GraphView.h"
#import "Resource.h"
#import "DataPoint.h"

// Constant for maximum acceleration.
#define kMaxAcceleration 3.0
// Constant for the high-pass filter.
#define kFilteringFactor 0.1
// Constant for view padding
#define kPadding 10.0

// Modes for graph updates
#define kSquashMode 0
#define kScrollMode 1
#define kFreezeMode 2

@interface GraphView (Hidden)
// Hidden methods
- (void) maybeUpdateDisplay;

// MultiTouch tracking
- (CGFloat)distanceBetweenTwoPoints:(CGPoint)fromPoint toPoint:(CGPoint)toPoint;

@end

// GraphView class implementation.
@implementation GraphView

// Instruct the compiler to generate accessors for the property, and use the internal variable _filter for storage.
@synthesize resource;

- (void) setResource: (Resource *)new_resource {
	if(resource == nil){
		struct timeval tv;
		if(0 == gettimeofday(&tv, NULL)){
			rightEdgeX = (double)tv.tv_sec + ((double)tv.tv_usec * 1.0e-6);
			leftEdgeX = rightEdgeX - 1.0;
		}			
		[self setNeedsDisplay];
	}
	if(new_resource != resource){
		Resource * old = resource;
		resource = new_resource;
		[resource retain];
		[old release];
		[self setNeedsDisplay];
	}
}

- (void) scaleRange: (float) amount inBounds:(CGRect)bounds {
	float timeAdjust = (amount  / (bounds.size.width)) * (rightEdgeX - leftEdgeX); 
	// Split the difference ober left and right to keep centered
	leftEdgeX -= timeAdjust / 2.0;
	rightEdgeX += timeAdjust / 2.0;
	
	if(leftEdgeX < resource.minTime){
		leftEdgeX = resource.minTime;
	}
	if(rightEdgeX > resource.maxTime){
		rightEdgeX = resource.maxTime;
		update_mode = kScrollMode;
	} else {
		update_mode = kFreezeMode;
	}
	[self setNeedsDisplay];
}

- (void) scrollRange: (float) amount inBounds:(CGRect)bounds {
	float timeAdjust = -1.0 * (amount  / (bounds.size.width)) * (rightEdgeX - leftEdgeX); 
	
	if(timeAdjust >=0){
		// Scrolling to the future
		if(rightEdgeX + timeAdjust > resource.maxTime){
			leftEdgeX += (resource.maxTime - rightEdgeX);
			rightEdgeX = resource.maxTime;
			update_mode = kScrollMode;
		} else{
			leftEdgeX += timeAdjust;
			rightEdgeX += timeAdjust;
			update_mode = kFreezeMode;

		}
	} else {
		// Scrolling to the past
		if(leftEdgeX + timeAdjust < resource.minTime){
			rightEdgeX += (resource.minTime - leftEdgeX);
			leftEdgeX = resource.minTime;
			update_mode = kFreezeMode;
		} else{
			leftEdgeX += timeAdjust;
			rightEdgeX += timeAdjust;
			update_mode = kFreezeMode;

		}
	}

	[self setNeedsDisplay];
}

- (void)updateItem: (Resource*)new_resource {
	if(new_resource == resource){
		[self maybeUpdateDisplay];
	}
}

- (float) yCoordinateForDataPoint: (DataPoint*) dpoint bounds:(CGRect)bounds {
	// NOTE: We need to draw upside-down as UIView referential has the Y axis going down
	float yScalar = (dpoint.value - resource.minValue)  / (resource.maxValue - resource.minValue); //
	float y = bounds.size.height + bounds.origin.y - (yScalar * bounds.size.height);	
	return y;
}

- (float) xCoordinateForDataPoint: (DataPoint*) dpoint bounds:(CGRect)bounds {
	float xScalar = (dpoint.timestamp - leftEdgeX)  / (rightEdgeX - leftEdgeX);
	float x = bounds.origin.x + (xScalar * bounds.size.width);	
	return x;	
}

- (float) timeAtX: (float) x bounds:(CGRect)bounds {
	float xScalar = (x - bounds.origin.x)  / (bounds.size.width);
	float time = leftEdgeX + (xScalar * (rightEdgeX - leftEdgeX));
	return time;	
}

- (void) maybeUpdateDisplay {
	bool bUpdate = NO;
	switch (update_mode) {
		case kSquashMode:
			// Squash in new points
			if(resource.maxTime > rightEdgeX){
				rightEdgeX= resource.maxTime;
				bUpdate = YES;
			}
			if(resource.minTime < leftEdgeX){
				leftEdgeX = resource.minTime;
				bUpdate = YES;
			}
			break;
			
		case kScrollMode:
			// Scroll new points with current scale
			if(resource.maxTime > maxX){
				double interval = resource.maxTime - maxX;
				leftEdgeX += interval;
				rightEdgeX += interval;
				bUpdate = YES;
			}
			break;
		case kFreezeMode:
		default:
			// Leave view alone
			break;
			
	}
	maxX = resource.maxTime;
	if(bUpdate){
		[self setNeedsDisplay];
	}
}

- (void)drawAxisInContext:(CGContextRef)context bounds:(CGRect)bounds {
    UIFont *font = [UIFont systemFontOfSize:12];
	float value;
    
    // Draw the background
    CGContextSetGrayFillColor(context, 0.6, 1.0);
    CGContextFillRect(context, bounds);
    
    // Draw the intermediate lines
    CGContextSetGrayStrokeColor(context, 0.5, 1.0);
    CGContextBeginPath(context);
	double increment = (resource.maxValue - resource.minValue) / 10.0;
	if( 0 == increment ){
		increment = 0.1;
	}
	
	for (value = resource.minValue; value <= resource.maxValue; value += increment) {
		float y = roundf(bounds.origin.y + (value - resource.minValue ) / (resource.maxValue - resource.minValue) * bounds.size.height);
		CGContextMoveToPoint(context, bounds.origin.x, y);
		CGContextAddLineToPoint(context, bounds.origin.x + bounds.size.width, y);
	}
    CGContextStrokePath(context);
	
	// Draw the labels
    CGContextSetGrayFillColor(context, 1.0, 1.0);
    CGContextSetAllowsAntialiasing(context, true);
	if(increment){
		for (value = resource.minValue; value <= resource.maxValue; value += increment) {
			float y = roundf(bounds.origin.y + (resource.maxValue - value) / (resource.maxValue - resource.minValue) * bounds.size.height);
			// NOTE: We need to draw upside-down as UIView referential has the Y axis going down
			[[NSString stringWithFormat:@"%.2f", value] 
			 drawAtPoint:CGPointMake(bounds.origin.x + 4, y) withFont:font]; 
		}
	}

	/* Add x-axis 
    temp = roundf(bounds.origin.y + bounds.size.height / 2);
    CGPoint sPoint = CGPointMake(bounds.origin.x + bounds.size.width - 40, temp - 16);
    [[NSString stringWithFormat:@"%c Axis", 'X' + axis] drawAtPoint:sPoint withFont:font];
    CGContextSetAllowsAntialiasing(context, false);
	 */
}


- (void)drawPlotInContext:(CGContextRef)context bounds:(CGRect)bounds {
    //UIFont *font = [UIFont systemFontOfSize:12];
    unsigned i;
	//double padding = 0.1 * bounds.size.height; // Add 10% padding to top and bottom

	BOOL overscanLeft = NO;
	BOOL overscanRight = NO;
	bool firstPoint = YES;
	CGPoint * aPoints = NULL;
	int numPoints = 0;
	float x, y;
	
	// Create an array of points
	for (i = 0; i < resource.dataPoints.count; ++i) {
		DataPoint * dpoint = (DataPoint*)[resource.dataPoints objectAtIndex:i];
		if(dpoint.timestamp >= leftEdgeX && dpoint.timestamp <= rightEdgeX){
			if (firstPoint) {
				firstPoint = NO;
				if(i>0){
					overscanLeft = YES;
					DataPoint * overscanPoint = (DataPoint*)[resource.dataPoints objectAtIndex:i-1];
					y = [self yCoordinateForDataPoint:overscanPoint bounds:bounds]; 
					x = [self xCoordinateForDataPoint:overscanPoint bounds:bounds];
					aPoints = realloc(aPoints, sizeof(CGPoint) * ++numPoints);
					aPoints[numPoints-1].x = x;
					aPoints[numPoints-1].y = y;
				}
			}
			y = [self yCoordinateForDataPoint:dpoint bounds:bounds]; 
			x = [self xCoordinateForDataPoint:dpoint bounds:bounds];
			aPoints = realloc(aPoints, sizeof(CGPoint) * ++numPoints);
			aPoints[numPoints-1].x = x;
			aPoints[numPoints-1].y = y;
		}
		if(dpoint.timestamp > rightEdgeX){
			y = [self yCoordinateForDataPoint:dpoint bounds:bounds]; 
			x = [self xCoordinateForDataPoint:dpoint bounds:bounds];
			aPoints = realloc(aPoints, sizeof(CGPoint) * ++numPoints);
			aPoints[numPoints-1].x = x;
			aPoints[numPoints-1].y = y;
			overscanRight = YES;
			break;
		}
    }

	// Draw the lines
    CGContextSetRGBStrokeColor(context, 1.0, 0.0, 0.0, 1.0);
	CGContextSetRGBFillColor(context, 0.0, 0.0, 0.0, 1.0);
    CGContextBeginPath(context);
	
	
	CGContextAddLines(context, aPoints, numPoints);
	CGContextStrokePath(context);
	
    for (i = 0; i < numPoints; ++i) {
		if(0 == i && overscanLeft){
			continue;
		}
		if(overscanRight && i == numPoints-1){
			break;
		}

		//CGContextMoveToPoint(context, aPoints[i].x, aPoints[i].y);
		CGContextBeginPath(context);
		CGContextAddArc(context, aPoints[i].x, aPoints[i].y, 3, 0, M_PI * 2.0, YES);
		CGContextFillPath(context);

    }
	
	CGContextSetLineWidth(context, 1.0);
}

- (void)drawRect:(CGRect)clip {
    CGSize size = [self bounds].size;
    //CGSize bounds = [self bounds].size;
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    // Draw the X, Y & Z graphs with anti-aliasing turned off
    CGContextSetAllowsAntialiasing(context, false);
    //CGFloat hOver3 = size.height / 3;
	//CGFloat hOver4 = size.height / 4;
	CGRect hBounds = CGRectMake(0 , 0, size.width, size.height);
	[self drawAxisInContext:context bounds:hBounds];
	[self drawPlotInContext:context bounds:hBounds];
    CGContextSetAllowsAntialiasing(context, true);
}

#pragma mark Touch Event Handling

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch* touch = [touches anyObject];
	NSUInteger numTaps = [touch tapCount];
	if (numTaps < 2) {
		[self.nextResponder touchesBegan:touches withEvent:event];
	} else {
		if(update_mode == kSquashMode){
			update_mode = kScrollMode;
		} else {
			update_mode = kSquashMode;
		}
		[self maybeUpdateDisplay];
	}
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {

	
	NSSet *allTouches = [event allTouches];
	
	switch ([allTouches count]) {
		case 1:  //Single touch
		{
			UITouch *touch1 = [[allTouches allObjects] objectAtIndex:0];
			lastTouchTime = [touch1 timestamp];
			break;
		}
			
		case 2:  //Double Touch
		{
			//Track the initial distance between two fingers.
			UITouch *touch1 = [[allTouches allObjects] objectAtIndex:0];
			UITouch *touch2 = [[allTouches allObjects] objectAtIndex:1];
			
			initialDistance = [self distanceBetweenTwoPoints:[touch1 locationInView:self]
													 toPoint:[touch2 locationInView:self]];
			break;
		}
		default:
			break;
	}
	
}

/*
 - (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
 {
 UITouch *touch = [touches anyObject];
 CGPoint currentTouchPosition = [touch locationInView:self];
 
 // If the swipe tracks correctly.
 if (fabsf(startTouchPosition.x - currentTouchPosition.x) >= HORIZ_SWIPE_DRAG_MIN &&
 fabsf(startTouchPosition.y - currentTouchPosition.y) <= VERT_SWIPE_DRAG_MAX)
 {
 // It appears to be a swipe.
 if (startTouchPosition.x < currentTouchPosition.x)
 [self myProcessRightSwipe:touches withEvent:event];
 else
 [self myProcessLeftSwipe:touches withEvent:event];
 }
 else
 {
 // Process a non-swipe event.
 }
 }
 */

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	
	if([timer isValid])
		[timer invalidate];
	
	NSSet *allTouches = [event allTouches];
	
	switch ([allTouches count])
	{
		case 1: 
		{
			// Scroll the graph
			UITouch *touch = [[allTouches allObjects] objectAtIndex:0];
			float old = [touch previousLocationInView:self].x; 
			float new = [touch locationInView:self].x;
			[self scrollRange: new-old inBounds:[self bounds]];
		}	
		break;

		case 2: 
		{
			//Adjust graph x-axis scale
			
			UITouch *touch1 = [[allTouches allObjects] objectAtIndex:0];
			UITouch *touch2 = [[allTouches allObjects] objectAtIndex:1];
			
			//Calculate the distance between the two fingers.
			CGFloat finalDistance = [self distanceBetweenTwoPoints:[touch1 locationInView:self]
														   toPoint:[touch2 locationInView:self]];
			
			[self scaleRange: finalDistance - initialDistance inBounds:[self bounds]];
			
		} 
		break;
	}
	
}


- (CGFloat)distanceBetweenTwoPoints:(CGPoint)fromPoint toPoint:(CGPoint)toPoint {
	
	float x = toPoint.x - fromPoint.x;
	float y = toPoint.y - fromPoint.y;
	
	return sqrt(x * x + y * y);
}

- (void) clearTouches {
	initialDistance=-1;
	lastTouchTime = -1;
}

@end
