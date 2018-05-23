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

AWindow_Fusen

*/

#include "stdafx.h"

#include "AWindow_Fusen.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_Fusen
#pragma mark ===========================
//�tⳎ��\���E�C���h�E

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

@param inTextView �Ή�����TextView��ViewID
@param inProjectObjectID �\���tⳃf�[�^��L�[�i�v���W�F�N�g��ObjectID�j
@param inRelativePath �\���tⳃf�[�^��L�[�i���΃p�X�j
@param inModuleName �\���tⳃf�[�^��L�[�i���W���[�����j
@param inOffset �\���tⳃf�[�^��L�[�i�I�t�Z�b�g�j
*/
AWindow_Fusen::AWindow_Fusen( const AViewID inTextViewID,
			const AObjectID inProjectObjectID, const AText& inRelativePath, 
			const AText& inModuleName, const ANumber inOffset ) 
			:AWindow(), mTextViewID(inTextViewID), mEditBoxViewControlID(kControlID_Invalid)
{
	//�ҏW�Ώەtⳃf�[�^��L�[�ۑ�
	mProjectObjectID = inProjectObjectID;
	mRelativePath.SetText(inRelativePath);
	mModuleName.SetText(inModuleName);
	mOffset = inOffset;
}
/**
�f�X�g���N�^
*/
AWindow_Fusen::~AWindow_Fusen()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

//�tⳂɑΉ�����TextView���擾
AView_Text&	AWindow_Fusen::GetTextView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_Text,kViewType_Text,mTextViewID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�R���g���[���̃N���b�N���̃R�[���o�b�N
*/
ABool	AWindow_Fusen::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	if( inID == mEditBoxViewControlID )
	{
		SPI_EditFusen();
		result = true;
	}
	return result;
}

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_Fusen::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	//�E�C���h�E���̊eView�̈ʒu�E�T�C�Y���X�V
	UpdateViewBounds();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E�����iAWindow::Show()����R�[�������j
void	AWindow_Fusen::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�E�C���h�E���̐���
	NVM_CreateWindow(kWindowClass_Toolbar);
	NVI_SetWindowTransparency(0.8);
	
	AWindowPoint	pt = {0,0};
	//EditBox����
	mEditBoxViewControlID = NVM_AssignDynamicControlID();
	NVI_CreateEditBoxView(mEditBoxViewControlID,pt,10,10,kControlID_Invalid,kIndex_Invalid,true);
	//EditBox�ݒ�
	AText	defaultfontname;
	AFontWrapper::GetAppDefaultFontName(defaultfontname);//#375
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetTextDrawProperty(defaultfontname,9.0,kColor_Gray20Percent,kColor_Gray20Percent);
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetBackgroundColor(kColor_LightYellow,kColor_Bisque,kColor_Bisque);
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetEnableEdiit(false);
	//���݂̃R�����g�e�L�X�g�擾�AEditBox�֐ݒ�
	AText	text;
	GetApp().SPI_GetExternalCommentFromProjectDB(mProjectObjectID,mRelativePath,mModuleName,mOffset,text);
	NVI_SetControlText(mEditBoxViewControlID,text);
	//�E�C���h�E�̃T�C�Y�ύX
	ANumber	width = NVI_GetEditBoxView(mEditBoxViewControlID).SPI_GetMaxDisplayWidth() 
			+ NVI_GetEditBoxView(mEditBoxViewControlID).NVI_GetFrameLeftMargin()
			+ NVI_GetEditBoxView(mEditBoxViewControlID).NVI_GetFrameRightMargin();
	if( width < kMinWidth )   width = kMinWidth;
	ANumber	height = NVI_GetEditBoxView(mEditBoxViewControlID).NVI_GetImageHeight()
			+ NVI_GetEditBoxView(mEditBoxViewControlID).NVI_GetFrameTopMargin()
			+ NVI_GetEditBoxView(mEditBoxViewControlID).NVI_GetFrameBottomMargin();
	if( height < kMinHeight )   height = kMinHeight;
	NVI_SetWindowSize(width,height);
	UpdateViewBounds();
	//MouseWheelRedirect
	NVI_GetEditBoxView(mEditBoxViewControlID).NVI_SetMouseWheelEventRedirectViewID(mTextViewID);
	/*win
	//�t���[�e�B���O�ݒ�
	NVI_SetFloating();
	*/
}

/**
�E�C���h�E���̊eView�̈ʒu�E�T�C�Y���E�C���h�E�T�C�Y�ɍ��킹�čX�V����
*/
void	AWindow_Fusen::UpdateViewBounds()
{
	//�E�C���h�E�ʒu�E�T�C�Y�擾
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	
	//FileListView bounds�ݒ�
	AWindowPoint	pt;
	pt.x = 0;
	pt.y = 0;
	ANumber	viewwidth = windowBounds.right - windowBounds.left;
	ANumber	viewheight = windowBounds.bottom - windowBounds.top;
	//EditBox bounds�ݒ�
	NVI_GetViewByControlID(mEditBoxViewControlID).NVI_SetPosition(pt);
	NVI_GetViewByControlID(mEditBoxViewControlID).NVI_SetSize(viewwidth,viewheight);
}

/**
�tⳎ��ҏW�E�C���h�E��\�����A�\���E�C���h�E��Hide
*/
void	AWindow_Fusen::SPI_EditFusen()
{
	//���E�C���h�E����������Ƒ傫�߂�bounds�擾
	ARect	bounds;
	NVI_GetWindowBounds(bounds);
	ARect	rect = bounds;
	rect.right += kEditWindowWidthDelta;
	rect.bottom += kEditWindowHeightDelta;
	//�tⳎ��ҏW�E�C���h�E�\��
	GetApp().SPI_GetFusenEditWindow().SPNVI_Show(mProjectObjectID,mRelativePath,mModuleName,mOffset,rect,mTextViewID);
	//���E�C���h�E�͉B���iTextView��̊Ǘ��f�[�^�͎c�����܂܂ɂȂ�B�����TextView����̍X�V���i�ĕ`�擙�j�ɁA�ĕ\�������B�j
	NVI_Hide();
}
