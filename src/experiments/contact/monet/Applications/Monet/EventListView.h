#import <AppKit/NSView.h>
#import <AppKit/NSNibDeclarations.h> // For IBAction, IBOutlet
#import <AppKit/NSTextField.h>
#import <Foundation/NSNotification.h>

@class NSTextFieldCell;
@class EventList;
@class AppController;

/*===========================================================================

	Author: Craig-Richard Taube-Schock
		Copyright (c) 1994, Trillium Sound Research Incorporated.
		All Rights Reserved.

=============================================================================
*/

@interface EventListView : NSView
{
    NSFont *timesFont;
    NSFont *timesFontSmall;

    EventList *eventList;

    IBOutlet NSTextField *mouseTimeField;
    IBOutlet NSTextField *mouseValueField;

    int startingIndex;
    float timeScale;
    BOOL mouseBeingDragged;
    NSTrackingRectTag trackTag;

    NSTextFieldCell *ruleCell;
    NSTextFieldCell *minMaxCell;
    NSTextFieldCell *parameterNameCell;

    NSArray *displayParameters;
}

- (id)initWithFrame:(NSRect)frameRect;
- (void)dealloc;

- (void)awakeFromNib;

- (NSArray *)displayParameters;
- (void)setDisplayParameters:(NSArray *)newDisplayParameters;

- (BOOL)acceptsFirstResponder;

- (void)setEventList:(EventList *)newEventList;

- (BOOL)isOpaque;
- (void)drawRect:(NSRect)rects;

- (void)clearView;
- (void)drawGrid;
- (void)drawRules;

- (void)mouseDown:(NSEvent *)theEvent;
- (void)mouseEntered:(NSEvent *)theEvent;
- (void)mouseExited:(NSEvent *)theEvent;
- (void)mouseMoved:(NSEvent *)theEvent;

- (void)updateScale:(float)column;

- (void)frameDidChange:(NSNotification *)aNotification;
- (void)resetTrackingRect;

- (float)scaledX:(float)x;
- (float)scaledWidth:(float)width;

@end
