//  This file is part of __APPNAME__, __SHORT_DESCRIPTION__.
//  Copyright (C) 2004 __OWNER__.  All rights reserved.

#import "MWindowController.h"

@class WebView;

@interface MReleaseNotesController : MWindowController
{
#ifndef GNUSTEP
    IBOutlet WebView *webView;
#endif
}

- (id)init;

- (void)windowDidLoad;

@end
