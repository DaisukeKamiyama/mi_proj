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

AWindow_HSeparatorOverlay

*/

#include "stdafx.h"

#include "AWindow_HSeparatorOverlay.h"

const AControlID	kHSperatorViewControlID = 101;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_HSeparatorOverlay
#pragma mark ===========================
//HSperator�\���I�[�o�[���C�E�C���h�E

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_HSeparatorOverlay::AWindow_HSeparatorOverlay():AWindow()
{
}
/**
�f�X�g���N�^
*/
AWindow_HSeparatorOverlay::~AWindow_HSeparatorOverlay()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_HSeparatorOverlay::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

/**
�E�C���h�E����
*/
void	AWindow_HSeparatorOverlay::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
}

/**
�V�K�^�u����
@param inTabIndex ��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_HSeparatorOverlay::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {0,0};
	NVI_CreateHSeparatorView(kHSperatorViewControlID,kSeparatorLinePosition_Middle,pt,10,10);
	UpdateViewBounds();
}

/**
View�ʒu�T�C�Y�ݒ�
*/
void	AWindow_HSeparatorOverlay::UpdateViewBounds()
{
	//
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	//
	AWindowPoint	pt = {0,0};
	NVI_SetControlPosition(kHSperatorViewControlID,pt);
	NVI_SetControlSize(kHSperatorViewControlID,windowBounds.right-windowBounds.left, windowBounds.bottom-windowBounds.top);
}

/**
Target�E�C���h�E�ݒ�
*/
void	AWindow_HSeparatorOverlay::SPI_SetTargetWindowID( const AWindowID inTargetWindowID )
{
	NVI_GetHSeparatorViewByControlID(kHSperatorViewControlID).SPI_SetTargetWindowID(inTargetWindowID);
}

/**
Separator�ʒu�ݒ�
*/
void	AWindow_HSeparatorOverlay::SPI_SetLinePosition( const ASeparatorLinePosition inLinePosition )
{
	NVI_GetHSeparatorViewByControlID(kHSperatorViewControlID).SPI_SetLinePosition(inLinePosition);
}

//#634
/**
�w�i���ߗ��ݒ�
*/
/*#688
void	AWindow_HSeparatorOverlay::SPI_SetTransparency( const AFloatNumber inTransparency )
{
	NVI_GetHSeparatorViewByControlID(kHSperatorViewControlID).SPI_SetTransparency(inTransparency);
}
*/
