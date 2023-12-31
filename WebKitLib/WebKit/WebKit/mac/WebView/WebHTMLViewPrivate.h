/*
 * Copyright (C) 2005 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <WebKit/WebHTMLView.h>

@class DOMDocumentFragment;
@class DOMElement;
@class DOMNode;
@class DOMRange;
@class WebArchive;
@class WebFrameBridge;
@class WebView;
@class WebFrame;
@class WebPluginController;

@protocol WebHTMLHighlighter
- (NSRect)highlightRectForLine:(NSRect)lineRect representedNode:(DOMNode *)node;
- (void)paintHighlightForBox:(NSRect)boxRect onLine:(NSRect)lineRect behindText:(BOOL)text entireLine:(BOOL)line representedNode:(DOMNode *)node;

// the following methods are deprecated and will be removed once Mail switches to the new methods <rdar://problem/5050528>
- (NSRect)highlightRectForLine:(NSRect)lineRect;
- (void)paintHighlightForBox:(NSRect)boxRect onLine:(NSRect)lineRect behindText:(BOOL)text entireLine:(BOOL)line;
@end

@interface WebHTMLView (WebPrivate)

+ (NSArray *)supportedMIMETypes;
+ (NSArray *)supportedImageMIMETypes;
+ (NSArray *)supportedNonImageMIMETypes;
+ (NSArray *)unsupportedTextMIMETypes;

- (void)close;

// Modifier (flagsChanged) tracking SPI
+ (void)_postFlagsChangedEvent:(NSEvent *)flagsChangedEvent;
- (void)_updateMouseoverWithFakeEvent;

- (void)_setAsideSubviews;
- (void)_restoreSubviews;

- (BOOL)_insideAnotherHTMLView;
- (void)_clearLastHitViewIfSelf;
- (void)_updateMouseoverWithEvent:(NSEvent *)event;

+ (NSArray *)_insertablePasteboardTypes;
+ (NSArray *)_selectionPasteboardTypes;
- (void)_writeSelectionToPasteboard:(NSPasteboard *)pasteboard;

- (void)_frameOrBoundsChanged;

- (NSImage *)_dragImageForLinkElement:(NSDictionary *)element;
- (NSImage *)_dragImageForURL:(NSString*)linkURL withLabel:(NSString*)label;
- (void)_handleAutoscrollForMouseDragged:(NSEvent *)event;
- (WebPluginController *)_pluginController;

// FIXME: _selectionRect is deprecated in favor of selectionRect, which is in protocol WebDocumentSelection.
// We can't remove this yet because it's still in use by Mail.
- (NSRect)_selectionRect;

- (void)_startAutoscrollTimer:(NSEvent *)event;
- (void)_stopAutoscrollTimer;

- (BOOL)_canEdit;
- (BOOL)_canEditRichly;
- (BOOL)_canAlterCurrentSelection;
- (BOOL)_hasSelection;
- (BOOL)_hasSelectionOrInsertionPoint;
- (BOOL)_isEditable;

- (BOOL)_transparentBackground;
- (void)_setTransparentBackground:(BOOL)f;

- (void)_setToolTip:(NSString *)string;

// SPI's for Mail.
- (NSImage *)_selectionDraggingImage;
- (NSRect)_selectionDraggingRect;
- (DOMNode *)_insertOrderedList;
- (DOMNode *)_insertUnorderedList;
- (BOOL)_canIncreaseSelectionListLevel;
- (BOOL)_canDecreaseSelectionListLevel;
- (DOMNode *)_increaseSelectionListLevel;
- (DOMNode *)_increaseSelectionListLevelOrdered;
- (DOMNode *)_increaseSelectionListLevelUnordered;
- (void)_decreaseSelectionListLevel;
- (void)_setHighlighter:(id<WebHTMLHighlighter>)highlighter ofType:(NSString*)type;
- (void)_removeHighlighterOfType:(NSString*)type;
- (DOMDocumentFragment *)_documentFragmentFromPasteboard:(NSPasteboard *)pasteboard forType:(NSString *)pboardType inContext:(DOMRange *)context subresources:(NSArray **)subresources;

// SPI for DumpRenderTree
- (void)_updateFocusedAndActiveState;

// SPI for printing (should be converted to API someday). When the WebHTMLView isn't being printed
// directly, this method must be called before paginating, or the computed height might be incorrect.
// Typically this would be called from inside an override of -[NSView knowsPageRange:].
- (void)_layoutForPrinting;

- (BOOL)_canSmartReplaceWithPasteboard:(NSPasteboard *)pasteboard;


@end
