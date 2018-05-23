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

AWindow_ToolTip

*/

#include "stdafx.h"

#include "AWindow_ToolTip.h"

const AControlID	kControlID_Text = 101;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_ToolTip
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_ToolTip::AWindow_ToolTip():AWindow()
{
}
/**
�f�X�g���N�^
*/
AWindow_ToolTip::~AWindow_ToolTip()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_ToolTip::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//view bounds�X�V
	UpdateViewBounds();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

/**
�E�C���h�E����
*/
void	AWindow_ToolTip::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_Top,false,false,false,false,false,true,false);
	
	//�e�L�X�g�{�b�N�X����
	{
		AText	headerfontname;
		AFontWrapper::GetDialogDefaultFontName(headerfontname);
		AColor	backgrondColor = kColor_Gray97Percent;//#1079 AColorWrapper::GetColorByHTMLFormatColor("fcfac7");
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_Text,pt,10,10,kControlID_Invalid,kIndex_Invalid,false,false,false,true,kEditBoxType_ThinFrame);
		NVI_GetEditBoxView(kControlID_Text).SPI_SetBackgroundColor(
					backgrondColor,backgrondColor,backgrondColor);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetTextFont(headerfontname,10.0);
		NVI_GetEditBoxView(kControlID_Text).SPI_SetTransparency(0.98);//#1079 0.8����ύX
		NVI_GetEditBoxView(kControlID_Text).SPI_SetEnableFocusRing(false);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetAutomaticSelectBySwitchFocus(false);
		NVI_GetEditBoxView(kControlID_Text).SPI_SetReadOnly();
		NVI_SetControlBindings(kControlID_Text,true,true,true,true,false,false);
	}
	
}

/**
�V�K�^�u����
@param inTabIndex ��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_ToolTip::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
}

/**
View�ʒu�T�C�Y�ݒ�
*/
void	AWindow_ToolTip::UpdateViewBounds()
{
	//bounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	//�e�L�X�gview�z�u
	AWindowPoint	pt = {0,0};
	NVI_SetControlPosition(kControlID_Text,pt);
	NVI_SetControlSize(kControlID_Text,windowBounds.right-windowBounds.left, windowBounds.bottom-windowBounds.top);
}

/**
�c�[���`�b�v�e�L�X�g�ݒ�
*/
void	AWindow_ToolTip::SPI_SetToolTipText( const AText& inToolTipText )
{
	//�e�L�X�g�ݒ�
	NVI_GetEditBoxView(kControlID_Text).NVI_SetText(inToolTipText);
	//�E�C���h�E�T�C�Y����
	AdjustWindowSize();
	//�e�L�X�g�X�N���[���ʒu
	AImagePoint	pt = {0,0};
	NVI_GetEditBoxView(kControlID_Text).NVI_ScrollTo(pt);
}

/**
�E�C���h�E�T�C�Y����
*/
void	AWindow_ToolTip::AdjustWindowSize()
{
	ANumber	w = NVI_GetEditBoxView(kControlID_Text).SPI_GetMaxDisplayWidth();
	ANumber	h = NVI_GetEditBoxView(kControlID_Text).NVM_GetImageHeight();
	w += 16;
	h += 8;
	NVI_SetWindowSize(w,h);
}



