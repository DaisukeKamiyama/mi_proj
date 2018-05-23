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

AWindow_DiffInfo
#379

*/

#include "stdafx.h"

#include "AWindow_DiffInfo.h"
#include "AView_DiffInfo.h"
#include "AApp.h"

//
const AControlID	kInfoViewControlID = 1;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_DiffInfo
#pragma mark ===========================
//Diff���E�C���h�E�̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_DiffInfo::AWindow_DiffInfo():AWindow()
{
}
//�f�X�g���N�^
AWindow_DiffInfo::~AWindow_DiffInfo()
{
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

/**
�E�C���h�E����
*/
void	AWindow_DiffInfo::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�E�C���h�E���̐���
	NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_None,false,false,false,false,true,false,false);
}

/**
�V�K�^�u����
@param inTabIndex ��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_DiffInfo::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {0,0};
	//IdInfoView����
	AViewDefaultFactory<AView_DiffInfo>	infoViewFactory(GetObjectID(),kInfoViewControlID);
	NVM_CreateView(kInfoViewControlID,pt,500,500,kControlID_Invalid,kControlID_Invalid,false,infoViewFactory);
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(kInfoViewControlID,true,true,true,true,false,false);
}

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_DiffInfo::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//view bounds�ݒ�
	ARect	rect = {0};
	NVI_GetWindowBounds(rect);
	GetDiffInfoView().NVI_SetSize(rect.right-rect.left,rect.bottom-rect.top);
	AWindowPoint	pt = {0,0};
	GetDiffInfoView().NVI_SetPosition(pt);
}

/**
*/
void	AWindow_DiffInfo::SPI_SetTextWindowID( const AWindowID inTextWindowID )
{
	GetDiffInfoView().SPI_SetTextWindowID(inTextWindowID);
}

/**
*/
AView_DiffInfo&	AWindow_DiffInfo::GetDiffInfoView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_DiffInfo,kViewType_DiffInfo,kInfoViewControlID);
}

