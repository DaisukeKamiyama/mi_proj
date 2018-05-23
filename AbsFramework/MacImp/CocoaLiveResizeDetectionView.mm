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

CocoaLiveResizeDetectionView

*/

#import "CocoaLiveResizeDetectionView.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

@implementation CocoaLiveResizeDetectionView

#pragma mark ===========================
#pragma mark [�N���X]CocoaLiveResizeDetectionView
#pragma mark ===========================

/*
�E�C���h�E��LiveResize���������m���邽�߂̃_�~�[view�B
NSWindow��LiveResize���m���\�b�h��10.3���ŃT�|�[�g����Ă��Ȃ����߁A�iResize�r���͌��m�ł��邪�A�r���Ȃ̂������Ȃ̂�������ł��Ȃ��j
����view����ɓo�^���Ă������ƂŁALiveResize���������m����B
*/

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
		//��ɔ�\���ɂ���
		[self setHidden:YES];
	}
	return self;
}

/**
KeyView��
*/
- (BOOL)canBecomeKeyView
{
	return NO;
}

/**
FirstResponder��
*/
- (BOOL)acceptsFirstResponder
{
	return NO;
}


/**
LiveResize����������
*/
- (void)viewDidEndLiveResize
{
	//�p���������s
	[super viewDidEndLiveResize];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowResizeCompleted()���s
	mWindowImp->APICB_CocoaWindowResizeCompleted();
	
	OS_CALLBACKBLOCK_END;
}

@end

