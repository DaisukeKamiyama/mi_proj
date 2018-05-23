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

AWindow_ButtonOverlay

*/

#include "stdafx.h"

#include "AWindow_ButtonOverlay.h"

const AControlID	kButtonViewControlID = 101;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_ButtonOverlay
#pragma mark ===========================
//HSperator�\���I�[�o�[���C�E�C���h�E

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_ButtonOverlay::AWindow_ButtonOverlay():AWindow()
,mClickEventTargetWindowID(kObjectID_Invalid),mClickEventVirtualControlID(kControlID_Invalid)
{
}
/**
�f�X�g���N�^
*/
AWindow_ButtonOverlay::~AWindow_ButtonOverlay()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�N���b�N���̏����i���_�C���N�g�j
*/
ABool	AWindow_ButtonOverlay::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	return AApplication::GetApplication().NVI_GetWindowByID(mClickEventTargetWindowID).EVT_Clicked(mClickEventVirtualControlID,inModifierKeys);;
}

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_ButtonOverlay::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//�ʒu�X�V #688
	AWindowPoint	pt = {0,0};
	NVI_GetButtonViewByControlID(kButtonViewControlID).NVI_SetPosition(pt);
	//�T�C�Y�X�V
	NVI_GetButtonViewByControlID(kButtonViewControlID).
			NVI_SetSize(inCurrentBounds.right-inCurrentBounds.left,inCurrentBounds.bottom-inCurrentBounds.top);
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

/**
�E�C���h�E����
*/
void	AWindow_ButtonOverlay::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
}

/**
�V�K�^�u����
@param inTabIndex ��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_ButtonOverlay::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//�{�^������
	AWindowPoint	pt = {0,0};
	NVI_CreateButtonView(kButtonViewControlID,pt,NVI_GetWindowWidth(),NVI_GetWindowHeight(),kControlID_Invalid);
	NVI_GetButtonViewByControlID(kButtonViewControlID).SPI_SetAlwaysActiveColors(true);
}

/**
Target�E�C���h�E�ݒ�
*/
void	AWindow_ButtonOverlay::SPI_SetTargetWindowID( const AWindowID inTargetWindowID, const AControlID inVirtualControlID )
{
	mClickEventTargetWindowID = inTargetWindowID;
	mClickEventVirtualControlID = inVirtualControlID;
}

/**
�{�^���A�C�R���ݒ�
*/
void	AWindow_ButtonOverlay::SPI_SetIcon( const AImageID inIconID, const ANumber inLeftOffset, const ANumber inTopOffset,
		const ANumber inWidth, const ANumber inHeight, const AImageID inHoverIconImageID, const AImageID inToggleOnImageID )
{
	NVI_GetButtonViewByControlID(kButtonViewControlID).SPI_SetIcon(inIconID,inLeftOffset,inTopOffset,inWidth,inHeight,false,inHoverIconImageID,inToggleOnImageID);
}

/**
�{�^���^�C�v�ݒ�
*/
void	AWindow_ButtonOverlay::SPI_SetButtonViewType( const AButtonViewType inType )
{
	NVI_GetButtonViewByControlID(kButtonViewControlID).SPI_SetButtonViewType(inType);
}

//#661
/**
�w���v�^�O�ݒ�
*/
void	AWindow_ButtonOverlay::SPI_SetHelpTag( const AText& inText1, const AText& inText2, const AHelpTagSide inTagSide )
{
	NVI_GetButtonViewByControlID(kButtonViewControlID).SPI_SetHelpTag(inText1,inText2,inTagSide);
}

