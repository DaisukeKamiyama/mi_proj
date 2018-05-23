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

AWindow_FusenList
#138 AWindow_JumpList���x�[�X�Ƃ��ĕtⳎ����X�g�p�ɐV�K����

*/

#include "stdafx.h"

#include "AWindow_FusenList.h"
#include "AApp.h"
#include "AView_List_FusenList.h"

#pragma mark ===========================
#pragma mark [�N���X]AWindow_FusenList
#pragma mark ===========================
//�tⳎ����X�g�E�C���h�E�̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_FusenList::AWindow_FusenList()
	:AWindow(), mListViewControlID(kControlID_Invalid), mVScrollBarControlID(kControlID_Invalid)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kFusenListWindowPosition);
}
//�f�X�g���N�^
AWindow_FusenList::~AWindow_FusenList()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

/**
ListView(�tⳎ����X�g)�I�u�W�F�N�g�擾
*/
AView_List_FusenList&	AWindow_FusenList::SPI_GetListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_List_FusenList,kViewType_List_FusenList,mListViewControlID);
}
const AView_List_FusenList&	AWindow_FusenList::SPI_GetListViewConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_List_FusenList,kViewType_List_FusenList,mListViewControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_FusenList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	//�R���g���[���̈ʒu�E�T�C�Y�X�V
	UpdateViewBounds(kIndex_Invalid);
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_FusenList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�E�C���h�E���̐���
	NVM_CreateWindow("main/FusenList");
	
	//�T�C�Y�ݒ�
	NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFusenListWindowWidth));
	NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFusenListWindowHeight));
	
	/*win
	//�t���[�e�B���O�ݒ�
	NVI_SetFloating();
	*/
}

//�V�K�^�u����
//�����F��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
void	AWindow_FusenList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {0,0};
	
	//ListView����
	mListViewControlID = NVM_AssignDynamicControlID();
	AViewDefaultFactory<AView_List_FusenList>	fusenListViewFactory(GetObjectID(),mListViewControlID);
	NVM_CreateView(mListViewControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,fusenListViewFactory);
	
	//V�X�N���[���o�[�����AListView�Ƀ����N��ݒ�
	mVScrollBarControlID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(mVScrollBarControlID,pt,kVScrollBarWidth,kVScrollBarWidth*2);
	SPI_GetListView().NVI_SetScrollBarControlID(kControlID_Invalid,mVScrollBarControlID);
	
	//�R���g���[���̈ʒu�E�T�C�Y�X�V
	UpdateViewBounds(inTabIndex);
	
	//
	outInitialFocusControlID = mListViewControlID;
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_FusenList::NVIDO_Hide()
{
	//�E�C���h�E���̕ۑ�
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kFusenListWindowWidth,NVI_GetWindowWidth());
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kFusenListWindowHeight,NVI_GetWindowHeight());
}

/**
�v���p�e�B�f�[�^�𔽉f����
*/
void	AWindow_FusenList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	NVI_SetWindowTransparency((static_cast<AFloatNumber>(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kSubWindowsAlpha_Floating)))/100);
}

#pragma mark ===========================

#pragma mark ---

/**
�R���g���[���̈ʒu�E�T�C�Y�X�V
*/
void	AWindow_FusenList::UpdateViewBounds( const AIndex inTabIndex )
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	
	//FileListView bounds�ݒ�
	AWindowPoint	pt;
	pt.x = 0;
	pt.y = kButtonHeight;
	ANumber	viewwidth = windowBounds.right - windowBounds.left -kVScrollBarWidth +1;
	ANumber	viewheight = windowBounds.bottom - windowBounds.top;
	NVI_SetControlPosition(mListViewControlID,pt);
	NVI_SetControlSize(mListViewControlID,viewwidth,viewheight-kButtonHeight);
	//�X�N���[���o�[bounds�ݒ�
	pt.x = windowBounds.right - windowBounds.left -kVScrollBarWidth +1;
	pt.y = kButtonHeight;
	viewwidth = kVScrollBarWidth;
	viewheight = windowBounds.bottom - windowBounds.top - kDragBoxHeight +1;
	NVI_SetControlPosition(mVScrollBarControlID,pt);
	NVI_SetControlSize(mVScrollBarControlID,viewwidth,viewheight-kButtonHeight);
}


