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

CocoaWindowDelegate

*/

#import "CocoaWindowDelegate.h"
#import "CocoaTextField.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"
#import "CWindowImpCocoaObjects.h"

@implementation CocoaWindowDelegate

#pragma mark ===========================
#pragma mark [クラス]CocoaWindowDelegate
#pragma mark ===========================

/**
初期設定
*/
- (id)initWithWindowImp:(CWindowImp*)inWindowImp
{
	//継承処理実行
	self = [super init];
	//初期設定
	if( self != nil )
	{
		mWindowImp = inWindowImp;
	}
	return self;
}

/**
ウインドウを閉じようとするときの処理
*/
- (BOOL)windowShouldClose:(id)sender
{
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,NO);
	
	//AWindow::EVT_DoCloseButton()実行
	mWindowImp->APICB_CocoaDoWindowShouldClose();
	
	OS_CALLBACKBLOCK_END;
	
	//OSにウインドウクローズ処理させないために、NOを返す
	return NO;
}

/**
ウインドウ移動時処理
*/
- (void)windowDidMove:(NSNotification *)notification
{
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowBoundsChanged()実行
	NSWindow*	win = mWindowImp->GetCocoaObjects().GetWindow();
	AGlobalRect	newFrameGlobalRect = {0};
	NSRect	contentFrame = [win contentRectForFrameRect:[win frame]];
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	newFrameGlobalRect.left		= contentFrame.origin.x;
	newFrameGlobalRect.top		= firstScreenFrame.size.height - (contentFrame.origin.y + contentFrame.size.height);
	newFrameGlobalRect.right	= contentFrame.origin.x + contentFrame.size.width;
	newFrameGlobalRect.bottom	= firstScreenFrame.size.height - contentFrame.origin.y;
	mWindowImp->APICB_CocoaWindowBoundsChanged(newFrameGlobalRect);
	
	OS_CALLBACKBLOCK_END;
}

@end

