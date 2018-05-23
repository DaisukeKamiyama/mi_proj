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

AWindow_Notification
#725

*/

#include "stdafx.h"

#include "AWindow_Notification.h"
#include "AView_Notification.h"
#include "AApp.h"

//Notification view��control id
const AControlID	kControlID_ListView = 101;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_Notification
#pragma mark ===========================
/*
�e�탁�b�Z�[�W�\���p�|�b�v�A�b�v
�etext view����AWindow_Notification�|�b�v�A�b�v�E�C���h�E�������A���̃E�C���h�E����AView_Notification������
*/

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_Notification::AWindow_Notification():AWindow()
{
	//�^�C�}�[enable
	NVI_AddToTimerActionWindowArray();
}
/**
�f�X�g���N�^
*/
AWindow_Notification::~AWindow_Notification()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

/**
ListView(Notification view)�擾
*/
AView_Notification&	AWindow_Notification::GetListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Notification,kViewType_Notification,kControlID_ListView);
}
const AView_Notification&	AWindow_Notification::GetListViewConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_Notification,kViewType_Notification,kControlID_ListView);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
���j���[�I��������
*/
ABool	AWindow_Notification::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//���̑���menu item ID�̏ꍇ�A���̃N���X�ŏ��������A���̃R�}���h�Ώۂŏ�������
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

/**
���j���[�X�V
*/
void	AWindow_Notification::EVTDO_UpdateMenu()
{
}

//�R���g���[���̃N���b�N���̃R�[���o�b�N
ABool	AWindow_Notification::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	}
	return result;
}

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_Notification::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	UpdateViewBounds();
}

/**
�E�C���h�E�̕���{�^��
*/
void	AWindow_Notification::EVTDO_DoCloseButton()
{
}

/**
tick�^�C�}�[����
*/
void	AWindow_Notification::EVTDO_DoTickTimerAction()
{
	GetListView().SPI_DoTickTimer();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_Notification::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
	
	//Notification view����
	AWindowPoint	pt = {0,0};
	AViewDefaultFactory<AView_Notification>	infoViewFactory(GetObjectID(),kControlID_ListView);
	NVM_CreateView(kControlID_ListView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,infoViewFactory);
}

//�V�K�^�u����
//�����F��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
void	AWindow_Notification::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_Notification::NVIDO_Hide()
{
	GetListView().SPI_DoWindowHide();
}

//
void	AWindow_Notification::NVIDO_UpdateProperty()
{
}

/**
view bounds�X�V
*/
void	AWindow_Notification::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	//�w�i�`��view�z�u
	AWindowPoint	pt = {0};
	NVI_SetControlPosition(kControlID_ListView,pt);
	NVI_SetControlSize(kControlID_ListView,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top);
	//�w�i�`��view�\��
	NVI_SetShowControl(kControlID_ListView,true);
}


