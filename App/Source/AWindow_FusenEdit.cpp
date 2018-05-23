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

AWindow_FusenEdit

*/

#include "stdafx.h"

#include "AWindow_FusenEdit.h"
#include "AApp.h"
#include "AView_Text.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_FusenEdit
#pragma mark ===========================
//�C���f�b�N�X�E�C���h�E�̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

�tⳎ��ҏW�E�C���h�E�́AApp�I�u�W�F�N�g����ɂP�������L����B
�K�v�ɉ����ē��e�E�ʒu�E�T�C�Y��ݒ肵�ĕ\������B
*/
AWindow_FusenEdit::AWindow_FusenEdit() :AWindow(), mProjectObjectID(kObjectID_Invalid), mTextViewID(kObjectID_Invalid), mEditBoxViewControlID(kControlID_Invalid)
{
	//�L�����b�g�u�����N�̂��߃^�C�}�[�C�x���g��L���ɂ���
	NVI_AddToTimerActionWindowArray();
}
//�f�X�g���N�^
AWindow_FusenEdit::~AWindow_FusenEdit()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

//�tⳂ�\��t����TextView���擾
AView_Text&	AWindow_FusenEdit::GetTextView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_Text,kViewType_Text,mTextViewID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_FusenEdit::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	UpdateViewBounds();
}

//WindowDeactivated���̃R�[���o�b�N
void	AWindow_FusenEdit::EVTDO_WindowDeactivated()
{
	NVI_Hide();
}

//�E�C���h�E�̕���{�^��
void	AWindow_FusenEdit::EVTDO_DoCloseButton()
{
	NVI_Hide();
}

//���j���[�I�����̃R�[���o�b�N�i�������s������true��Ԃ��j
ABool	AWindow_FusenEdit::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			NVI_Hide();
			return true;
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E�����iAWindow::Show()����R�[�������j
void	AWindow_FusenEdit::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�E�C���h�E���̐���
	NVM_CreateWindow("main/FusenEdit");
	NVI_SetWindowTransparency(0.8);
	
	AWindowPoint	pt = {0,0};
	//EditBox����
	mEditBoxViewControlID = NVM_AssignDynamicControlID();
	NVI_CreateEditBoxView(mEditBoxViewControlID,pt,10,10,kControlID_Invalid,kIndex_Invalid,true);
	//�t�H���g�E�w�i�F�F���ݒ�
	AText	defaultfontname;
	AFontWrapper::GetAppDefaultFontName(defaultfontname);//#375
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetTextDrawProperty(defaultfontname,9.0,kColor_Gray20Percent,kColor_Gray20Percent);
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetBackgroundColor(kColor_LightYellow,kColor_Bisque,kColor_Bisque);
	NVI_SetCatchReturn(mEditBoxViewControlID,true);//#135
	NVI_SetCatchTab(mEditBoxViewControlID,true);//#135
}

void	AWindow_FusenEdit::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	
	//EditBox bounds�ݒ�
	AWindowPoint	pt;
	pt.x = 0;
	pt.y = 0;
	ANumber	viewwidth = windowBounds.right - windowBounds.left;
	ANumber	viewheight = windowBounds.bottom - windowBounds.top;
	NVI_GetViewByControlID(mEditBoxViewControlID).NVI_SetPosition(pt);
	NVI_GetViewByControlID(mEditBoxViewControlID).NVI_SetSize(viewwidth,viewheight);
}

/**
�E�C���h�E��Hide�����Ƃ��ɌĂ΂��
*/
void	AWindow_FusenEdit::NVIDO_Hide()
{
	ApplyData();
	mProjectObjectID = kObjectID_Invalid;
	mTextViewID = kObjectID_Invalid;
}

/**
�\��

@param inProjectObjectID �ҏW�Ώەtⳃf�[�^��L�[�i�v���W�F�N�g��ObjectID�j
@param inRelativePath �ҏW�Ώەtⳃf�[�^��L�[�i���΃p�X�j
@param inModuleName �ҏW�Ώەtⳃf�[�^��L�[�i���W���[�����j
@param inOffset �ҏW�Ώەtⳃf�[�^��L�[�i�I�t�Z�b�g�j
@param inBounds �\��Rect
*/
void	AWindow_FusenEdit::SPNVI_Show( const AObjectID inProjectObjectID, const AText& inRelativePath, 
		const AText& inModuleName, const ANumber inOffset,
		const ARect& inDisplayBounds, 
		const AViewID inTextViewID )
{
	//���ɊJ���Ă���ꍇ�́A��U�f�[�^�𔽉f����B
	if( NVI_IsWindowVisible() == true )
	{
		ApplyData();
	}
	
	//�Ή�TextView�ݒ�
	mTextViewID = inTextViewID;
	
	//�ҏW�Ώەtⳃf�[�^��L�[�ۑ�
	mProjectObjectID = inProjectObjectID;
	mRelativePath.SetText(inRelativePath);
	mModuleName.SetText(inModuleName);
	mOffset = inOffset;
	
	//���݂̃R�����g�e�L�X�g�擾�AEditBox�֐ݒ�
	AText	text;
	GetApp().SPI_GetExternalCommentFromProjectDB(mProjectObjectID,mRelativePath,mModuleName,mOffset,text);
	NVI_SetControlText(mEditBoxViewControlID,text);
	
	//�E�C���h�Ebounds�ݒ�
	NVI_SetWindowBounds(inDisplayBounds);
	UpdateViewBounds();
	
	//�E�C���h�E�\��
	NVI_Show();
	
	//�t�H�[�J�X�ړ�
	NVI_SwitchFocusTo(mEditBoxViewControlID);
	//�e�L�X�g�̍Ō��I��
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetSelect(text.GetItemCount(),text.GetItemCount());
}

/**
�f�[�^��ProjectDB�֕ۑ�����

�tⳎ��ҏW�E�C���h�E��Hide�����ꍇ�ȂǂɌĂ΂��
*/
void	AWindow_FusenEdit::ApplyData()
{
	//�R�����g�e�L�X�g��EditBox����擾���AProjectDB�֐ݒ�
	AText	text;
	NVI_GetControlText(mEditBoxViewControlID,text);
	GetApp().SPI_SetExternalCommentToProjectDB(mProjectObjectID,mRelativePath,mModuleName,mOffset,text);
	//�E�C���h�E�ʒu���擾���ATextView�Ƃ̑��Έʒu����AModuleName, Offset��ύX����
	if( mTextViewID != kObjectID_Invalid && GetApp().NVI_ExistView(mTextViewID) == true )
	{
		//�ʒu�����݂̃E�C���h�E�ʒu�ƁATextView����擾���AProjectDB�֐ݒ�
		APoint	pt;
		NVI_GetWindowPosition(pt);
		AText	newModuleName;
		ANumber	newOffset;
		GetTextView().SPI_GetFusenModuleAndOffset(pt,newModuleName,newOffset);
		GetApp().SPI_UpdateExternalCommentPosition(mProjectObjectID,mRelativePath,mModuleName,mOffset,newModuleName,newOffset);
		//�����f�[�^�X�V�i���̌�Hide����Ƃ͌���Ȃ���������Ȃ��̂ŁA�O�̂��ߎ�L�[�X�V���Ă����j
		mModuleName.SetText(newModuleName);
		mOffset = newOffset;
	}
}

