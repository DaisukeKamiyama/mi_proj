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

AWindow_SubTextShadow
#725

*/

#include "stdafx.h"

#include "AWindow_SubTextShadow.h"
#include "AView_SubTextShadow.h"
#include "AApp.h"

//
const AControlID	kShadowViewControlID = 1;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_SubTextShadow
#pragma mark ===========================
//Diff���E�C���h�E�̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_SubTextShadow::AWindow_SubTextShadow():AWindow()
{
}
//�f�X�g���N�^
AWindow_SubTextShadow::~AWindow_SubTextShadow()
{
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

/**
�E�C���h�E����
*/
void	AWindow_SubTextShadow::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�E�C���h�E���̐���
	NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_None,false,false,false,false,true,false,false);
}

/**
�V�K�^�u����
@param inTabIndex ��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_SubTextShadow::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {0,0};
	//View����
	AViewDefaultFactory<AView_SubTextShadow>	factory(GetObjectID(),kShadowViewControlID);
	NVM_CreateView(kShadowViewControlID,pt,500,500,kControlID_Invalid,kControlID_Invalid,false,factory);
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(kShadowViewControlID,true,true,true,true,false,false);
}

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_SubTextShadow::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//view bounds�ݒ�
	ARect	rect = {0};
	NVI_GetWindowBounds(rect);
	NVI_GetViewByControlID(kShadowViewControlID).NVI_SetSize(rect.right-rect.left,rect.bottom-rect.top);
	AWindowPoint	pt = {0,0};
	NVI_GetViewByControlID(kShadowViewControlID).NVI_SetPosition(pt);
}

