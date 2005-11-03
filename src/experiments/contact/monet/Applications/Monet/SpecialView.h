#import "TransitionView.h"

/*===========================================================================

	Author: Craig-Richard Taube-Schock
		Copyright (c) 1994, Trillium Sound Research Incorporated.
		All Rights Reserved.

=============================================================================
*/

@interface SpecialView : TransitionView
{
}

- (id)initWithFrame:(NSRect)frameRect;
- (void)dealloc;

// Drawing
- (void)drawGrid;
- (void)updateDisplayPoints;
- (void)highlightSelectedPoints;

// Event handling
//- (void)mouseDown:(NSEvent *)mouseEvent;

// Selection
- (void)selectGraphPointsBetweenPoint:(NSPoint)point1 andPoint:(NSPoint)point2;

@end
