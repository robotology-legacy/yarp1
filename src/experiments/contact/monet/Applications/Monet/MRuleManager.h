//  This file is part of __APPNAME__, __SHORT_DESCRIPTION__.
//  Copyright (C) 2004 __OWNER__.  All rights reserved.

#import "MWindowController.h"
#import <AppKit/NSNibDeclarations.h> // For IBAction, IBOutlet
#import <AppKit/NSDragging.h>
#import <AppKit/NSTableView.h>
#import <AppKit/NSOutlineView.h>
#import <AppKit/NSTextField.h>

@class MMBooleanNode, MMBooleanParser, MModel, MMRule, MonetList;
@class NSBrowser, NSPasteboard, NSForm;

@interface MRuleManager : MWindowController
{
    IBOutlet NSTableView *ruleTableView;

    IBOutlet NSBrowser *match1Browser;
    IBOutlet NSBrowser *match2Browser;
    IBOutlet NSBrowser *match3Browser;
    IBOutlet NSBrowser *match4Browser;

    IBOutlet NSForm *expressionForm;
    IBOutlet NSTextField *errorTextField;
    IBOutlet NSTextField *possibleCombinationsTextField;

    IBOutlet NSTableView *symbolTableView;
    IBOutlet NSOutlineView *symbolEquationOutlineView;

    IBOutlet NSTableView *parameterTableView;
    IBOutlet NSOutlineView *parameterTransitionOutlineView;

    IBOutlet NSTableView *specialParameterTableView;
    IBOutlet NSOutlineView *specialParameterTransitionOutlineView;

    IBOutlet NSTableView *metaParameterTableView;
    IBOutlet NSOutlineView *metaParameterTransitionOutlineView;

    IBOutlet NSTextView *ruleCommentTextView;

    MModel *model;

    NSMutableArray *matchLists; // Of arrays of postures/categories?
    MMBooleanNode *expressions[4];

    NSFont *regularControlFont;
    NSFont *boldControlFont;

    MMBooleanParser *boolParser;
}

- (id)initWithModel:(MModel *)aModel;
- (void)dealloc;

- (MModel *)model;
- (void)setModel:(MModel *)newModel;

- (NSUndoManager *)undoManager;
- (MMRule *)selectedRule;

- (void)windowDidLoad;
- (void)updateViews;
- (void)expandOutlines;
- (void)_updateSelectedRuleDetails;
- (void)_updateRuleComment;
- (void)_updateSelectedSymbolDetails;
- (void)_updateSelectedParameterDetails;
- (void)_updateSelectedSpecialParameterDetails;
- (void)_updateSelectedMetaParameterDetails;

- (void)setExpression:(MMBooleanNode *)anExpression atIndex:(int)index;
- (void)evaluateMatchLists;
- (void)updateCombinations;

// NSTableView data source
- (int)numberOfRowsInTableView:(NSTableView *)tableView;
- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(int)row;

// NSTableView delegate
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification;
- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(int)row;

// NSTableView dragging
- (BOOL)tableView:(NSTableView *)tableView writeRows:(NSArray *)rows toPasteboard:(NSPasteboard *)pboard;
- (NSDragOperation)tableView:(NSTableView *)tableView validateDrop:(id <NSDraggingInfo>)info proposedRow:(int)row proposedDropOperation:(NSTableViewDropOperation)op;
- (BOOL)tableView:(NSTableView *)tableView acceptDrop:(id <NSDraggingInfo>)info row:(int)row dropOperation:(NSTableViewDropOperation)op;

// MExtendedTableView delegate
- (BOOL)control:(NSControl *)aControl shouldProcessCharacters:(NSString *)characters;

// Browser delegate methods
- (int)browser:(NSBrowser *)sender numberOfRowsInColumn:(int)column;
- (void)browser:(NSBrowser *)sender willDisplayCell:(id)cell atRow:(int)row column:(int)column;

// NSOutlineView data source
- (int)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item;
- (id)outlineView:(NSOutlineView *)outlineView child:(int)index ofItem:(id)item;
- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item;
- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item;

// NSOutlineView delegate
- (BOOL)outlineView:(NSOutlineView *)outlineView shouldSelectItem:(id)item;
- (BOOL)outlineView:(NSOutlineView *)outlineView shouldCollapseItem:(id)item;
- (void)outlineViewSelectionDidChange:(NSNotification *)aNotification;

// NSTextView delegate
- (void)textDidEndEditing:(NSNotification *)aNotification;

// Actions
- (IBAction)setExpression:(id)sender;
- (IBAction)addRule:(id)sender;
- (IBAction)updateRule:(id)sender;
- (IBAction)removeRule:(id)sender;

@end
