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
#pragma mark [�N���X]CocoaWindowDelegate
#pragma mark ===========================

/**
�����ݒ�
*/
- (id)initWithWindowImp:(CWindowImp*)inWindowImp
{
	//�p���������s
	self = [super init];
	//�����ݒ�
	if( self != nil )
	{
		mWindowImp = inWindowImp;
	}
	return self;
}

/**
�E�C���h�E����悤�Ƃ���Ƃ��̏���
*/
- (BOOL)windowShouldClose:(id)sender
{
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,NO);
	
	//AWindow::EVT_DoCloseButton()���s
	mWindowImp->APICB_CocoaDoWindowShouldClose();
	
	OS_CALLBACKBLOCK_END;
	
	//OS�ɃE�C���h�E�N���[�Y���������Ȃ����߂ɁANO��Ԃ�
	return NO;
}

/**
�E�C���h�E�ړ�������
*/
- (void)windowDidMove:(NSNotification *)notification
{
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowBoundsChanged()���s
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

