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

CocoaCollapsableView, CocoaDisclosureButton
�J�\�ȃr���[�ƁA�J�p�O�p�{�^��

*/

#import "CocoaCollapsableView.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

@implementation CocoaCollapsableView

/**
�����ݒ�
*/
- (id)initWithFrame:(NSRect)inFrame
{
	//�p���������s
	self = [super initWithFrame:inFrame];
	if( self != nil )
	{
		//�J�����Ƃ��̍������L��
		expandedHeight = [self frame].size.height;
	}
	return self;
}

/**
�J�p�g���C�A���O���{�^���N���b�N������
*/
- (IBAction) doDisclosureButton:(id)sender
{
	//�����Ƃ��̍���
	const ANumber	kCollapsedViewHeight = 20;
	
	if( [sender isKindOfClass:[NSButton class]] == YES )
	{
		NSButton*	button = static_cast<NSButton*>(sender);
		//�g���C�A���O���{�^����ON/OFF�ɂ���āAview�̃T�C�Y�ݒ�
		if( [button state] == NSOffState )
		{
			//��frame���擾
			NSRect	oldFrame = [self frame];
			//top�ʒu��ۂ����܂܁A�����Ƃ��̍����ɂ���
			NSRect	newFrame = oldFrame;
			newFrame.size.height = kCollapsedViewHeight;
			newFrame.origin.y = oldFrame.origin.y + oldFrame.size.height -kCollapsedViewHeight;
			[self setFrame:newFrame];
		}
		else
		{
			//��frame���擾
			NSRect	oldFrame = [self frame];
			//top�ʒu��ۂ����܂܁A�J�����Ƃ��̍����ɂ���
			NSRect	newFrame = oldFrame;
			newFrame.size.height = expandedHeight;
			newFrame.origin.y = oldFrame.origin.y + oldFrame.size.height -expandedHeight;
			[self setFrame:newFrame];
		}
	}
	//�o�^����Ă��鎟view�̈ʒu�����炷
	[self updateNextViewFrame];
}

/**
�o�^����Ă��鎟view�̈ʒu�����炷
*/
- (void) updateNextViewFrame
{
	if( nextView != nil )
	{
		//���r���[��top�ʒu���A���r���[�̐^���ɂ���
		NSRect	nextViewFrame = [nextView frame];
		nextViewFrame.origin.y = [self frame].origin.y - nextViewFrame.size.height;
		[nextView setFrame:nextViewFrame];
		//���r���[��CocoaCollapsableView�Ȃ瓯���֐����Ă�ŁA�A�Ȃ��Ă���r���[�̈ʒu�����ׂĂ��炷
		if( [nextView isKindOfClass:[CocoaCollapsableView class]] == YES )
		{
			[nextView updateNextViewFrame];
		}
	}
}

@end

@implementation CocoaDisclosureButton
- (void) updateCollapsableView
{
	//collapsable view�ŊJ�p�g���C�A���O���{�^���N���b�N�����������s
	if( collapsableView != nil )
	{
		[collapsableView doDisclosureButton:self];
	}
}
@end

