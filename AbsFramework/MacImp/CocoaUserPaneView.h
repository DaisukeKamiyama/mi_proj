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

CocoaUserPaneView

*/

#include "CUserPane.h"
#import <Cocoa/Cocoa.h>

/**
UserPaneView�N���X
*/
@interface CocoaUserPaneView : NSControl//<NSToolTipOwner>
{
	AControlID	mControlID;
	CWindowImp*	mWindowImp;
	
	//�t�H�[�J�X��
	BOOL	mAcceptFocus;
	
	//deactivate�E�C���h�E���N���b�N�����Ƃ��̍ŏ��̃}�E�X�N���b�N���������邩�ǂ���
	BOOL	mAcceptsFirstMouse;
	
	//
	BOOL	mNeedsPanelToBecomeKey;
	
	//�s�������ǂ��� #1090
	BOOL	mIsOpaque;
	
	//�c�������[�h #558
	BOOL	mVerticalMode;
	
	//�T�[�r�X���j���[�� #1309
	BOOL	mServiceMenuAvailable;
	
	//mouse tracking rect
	//AArray<ALocalRect>	mAdditionalTrackingRectArray;
	AArray<NSTrackingRectTag>	mTrackingRectTagArray;
	
	//�L�[�����ς݃t���O
	//#1015 ABool	mKeyHandled;
	
	//InlineInput�p�_�~�[�e�L�X�g
	NSMutableString*	mCurrentInlineInputString;
	
	//�ϊ��e�L�X�g��UTF16/UTF8�ϊ��f�[�^
	AArray<AIndex>	mCurrentInlineInputUTF16PosIndexArray;
	AArray<AIndex>	mCurrentInlineInputUTF8PosIndexArray;
	
	//resetSelectedRangeForTextInput�ɂ�鏈����}������t���O
	ABool	mSuppressResetSelectedRange;
	
	//�ϊ��e�L�X�grange
	NSRange	mMarkedRange;
	//�_�~�[�e�L�X�g���I��range
	NSRange	mSelectedRange;
}

- (id)initWithFrame:(NSRect)inFrame windowImp:(CWindowImp*)inWindowImp controlID:(AControlID)inControlID;
- (void)setAcceptFocus:(ABool)inAcceptFocus;
- (void)setAcceptsFirstMouse:(ABool)inAcceptsFirstMouse;
- (void)setNeedsPanelToBecomeKey:(ABool)inNeedsPanelToBecomeKey;
- (void)setOpaque:(ABool)inOpqque;
- (void)setVerticalMode:(ABool)inVertical;//#558
- (void)setServiceMenuAvailable:(ABool)inAvailable;//#1309
- (void)resetSelectedRangeForTextInput;
//- (void)setMouseTrackingRect:(const AArray<ALocalRect>&)inLocalRectArray;
- (void)fixInlineInput;

//
- (void)moveToBeginningOfParagraphAndModifySelection:(id)sender;
//
- (void)indenttab:(id)sender;

@end

/**
UserPaneView�N���X + NSTextInputClient(Mac OS X 10.5�ȍ~)
*/
@interface CocoaUserPaneTextInputClientView : CocoaUserPaneView<NSTextInputClient>
{
}
@end

/**
UserPaneView�N���X + NSTextInput(Mac OS X 10.4�ȑO)
*/
/*#0 10.4�ȉ��̓T�|�[�g�O�ɂ���
@interface CocoaUserPaneTextInputView : CocoaUserPaneView<NSTextInput>
{
}
@end
*/

