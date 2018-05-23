/*

Copyright (C) 2018  Daisuke Kamiyama (https://www.mimikaki.net)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
/*

CocoaWindow

*/

#include "CWindowImp.h"
#import <Cocoa/Cocoa.h>

/**
AppleScript用ウインドウオブジェクト
*/
@interface CASWindow : NSObject
{
	AWindowID	mWindowID;
}
- (id)init:(AWindowID)windowID;
@end

/**
ウインドウクラス
*/
@interface CocoaWindow : NSWindow
{
	IBOutlet NSProgressIndicator*	progressIndicator;
	IBOutlet NSTabView*	tabView;
	
	AArray<NSTabView*>	mTabViewArray;
	
	CWindowImp*	mWindowImp;
	
	BOOL	mCanBecomeMainWindow;
	BOOL	mCanBecomeKeyWindow;
	//BOOL	mIgnoresMouseEvents;
	
	AOverlayWindowLayer	mOverlayWindowLayer;
	
	NSToolbarItem*	mAddingToolbarItem;
	
	NSFont*	mCurrentFontPanelFont;
	
	AArray<AControlID>			mSegmentedControlArray_ControlID;
	AArray<NSSegmentedControl*>	mSegmentedControlArray_SegmentedControl;
	AArray<NSInteger>			mSegmentedControlArray_Segment;
	
	AIndex	mToolbarType;
}

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)windowStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferCreation;
- (void)setup:(CWindowImp*)inWindowImp;
- (void)setOverlayWindowLayer:(AOverlayWindowLayer)inOverlayWindowLayer;
- (AOverlayWindowLayer)getOverlayWindowLayer;
- (void)setCanBecomeMainWindow:(BOOL)inVal;
- (void)setCanBecomeKeyWindow:(BOOL)inVal;
//- (void)setIgnoresMouseEvents:(BOOL)inVal;
- (NSProgressIndicator*)getProgressIndicator;
- (NSTabView*)getTabView;
- (NSView*)findViewByControlID:(AControlID)inControlID;
- (AWindowID)windowID;
- (void)doMouseMovedForThisWindow:(NSPoint)inGloalPoint modifierKeys:(AModifierKeys)modifier;
- (NSToolbarItem*)getAddingToolbarItem;
- (void)setToolbarType:(AIndex)inType;

- (void)reorderOverlayChildWindows;

- (IBAction) doButtonAction:(id)sender;
- (IBAction) doPopupMenuAction:(id)sender;
- (IBAction) doScrollbarAction:(id)sender;

- (BOOL)isSegmentControl:(AControlID)inControlID;
- (BOOL)setBoolForSegmentControl:(AControlID)inControlID select:(BOOL)inSelect;
- (BOOL)setEnableForSegmentControl:(AControlID)inControlID enable:(BOOL)inEnable;
- (BOOL)getEnableForSegmentControl:(AControlID)inControlID;

- (void)showFontPanel:(NSFont*)font;
- (NSFont*)getFontPanelFont;

//#1034
- (void)showChooseFolderWindow:(NSString*)dirPath message:(NSString*)message isSheet:(ABool)isSheet;
- (void)showChooseFileWindow:(NSString*)dirPath message:(NSString*)message isSheet:(ABool)isSheet onlyApp:(ABool)onlyApp;
- (void)showSaveWindow:(NSString*)dirPath filename:(NSString*)filename;

- (void)setWindowOrder:(SInt32)orderNumber;

@end

