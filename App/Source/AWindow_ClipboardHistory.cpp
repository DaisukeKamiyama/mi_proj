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

AWindow_ClipboardHistory
#152

*/

#include "stdafx.h"

#include "AWindow_ClipboardHistory.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"

//�N���b�v�{�[�h����list view
const AControlID	kControlID_ListView = 101;

//�N���b�v�{�[�h����list view ��
const ADataID			kColumn_Text				= 0;

//�X�N���[���o�[
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kWidth_VScroll = 12;

//#725
//SubWindow�p�^�C�g���o�[view
const AControlID	kControlID_TitleBar = 901;

//#725
//SubWindow�p�Z�p���[�^view
const AControlID	kControlID_Separator = 902;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_ClipboardHistory
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_ClipboardHistory::AWindow_ClipboardHistory():AWindow()
{
}
/**
�f�X�g���N�^
*/
AWindow_ClipboardHistory::~AWindow_ClipboardHistory()
{
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_ClipboardHistory::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 10;
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//#725
/**
SubWindow�p�^�C�g���o�[�擾
*/
AView_SubWindowTitlebar&	AWindow_ClipboardHistory::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�R���g���[���̃N���b�N���̃R�[���o�b�N
*/
ABool	AWindow_ClipboardHistory::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	return result;
}

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_ClipboardHistory::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}


#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_ClipboardHistory::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�E�C���h�E����
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//�T�C�h�o�[
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Bottom,false,false,false,false,false,false,false);
			break;
		}
		//�t���[�e�B���O
	  default:
		{
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,true,false,true,false);
			break;
		}
	}
	
	//SubWindow�p�^�C�g���o�[����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_ClipboardHistory");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwList);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//SubWindow�p�Z�p���[�^����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//���X�gview����
	{
		AWindowPoint	pt = {0,0};
		AListViewFactory	listViewFactory(GetObjectID(),kControlID_ListView);
		NVM_CreateView(kControlID_ListView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,listViewFactory);
		AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(true);
		NVI_GetListView(kControlID_ListView).SPI_SetBackgroundColor(
					GetApp().SPI_GetSubWindowBackgroundColor(true),
					GetApp().SPI_GetSubWindowBackgroundColor(false));
		NVI_GetListView(kControlID_ListView).SPI_RegisterColumn_Text(kColumn_Text,0,"",false);
		NVI_GetListView(kControlID_ListView).SPI_SetEnableCursorRow();
		AText	fontname;
		AFloatNumber	fontsize = 9.0;
		GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
		NVI_GetListView(kControlID_ListView).SPI_SetTextDrawProperty(fontname,fontsize,kColor_Black);
		NVI_GetListView(kControlID_ListView).SPI_SetWheelScrollPercent(
					GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
					GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));
		NVI_SetControlBindings(kControlID_ListView,true,true,true,true,false,false);
	}
	
	//V�X�N���[���o�[����
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateScrollBar(kControlID_VScrollBar,pt,kWidth_VScroll,kWidth_VScroll*2);
		NVI_GetListView(kControlID_ListView).NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
		NVI_SetShowControl(kControlID_VScrollBar,true);
		NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	}
}

/**
�V�K�^�u����
�����F��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_ClipboardHistory::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	UpdateViewBounds();
}

/**
Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
*/
void	AWindow_ClipboardHistory::NVIDO_Hide()
{
}

/**
*/
void	AWindow_ClipboardHistory::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	NVI_RefreshWindow();
}

/**
�N���b�v�{�[�h�������X�gview�X�V
*/
void	AWindow_ClipboardHistory::SPI_UpdateTable()
{
	NVI_GetListView(kControlID_ListView).SPI_BeginSetTable();
	NVI_GetListView(kControlID_ListView).SPI_SetColumn_Text(kColumn_Text,GetApp().SPI_GetClipboardHistory());
	NVI_GetListView(kControlID_ListView).SPI_EndSetTable();
	NVI_RefreshWindow();
}

/**
View�z�u�X�V
*/
void	AWindow_ClipboardHistory::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//�^�C���o�[�A�Z�p���[�^�����擾
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	
	//�^�C�g���o�[�A�Z�p���[�^�\���t���O
	ABool	showTitleBar = false;
	ABool	showSeparator = false;
	//list view�T�C�Y
	AWindowPoint	listViewPoint = {0,0};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top;
	//�X�N���[���o�[��
	ANumber	vscrollbarWidth = kWidth_VScroll;
	//���C�A�E�g�v�Z
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//�T�C�h�o�[�̏ꍇ
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//�^�C�g���o�[�A�Z�p���[�^�\���t���O
			showTitleBar = true;
			showSeparator = true;
			//list view�T�C�Y
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar + height_Separator;
			break;
		}
		//�t���[�e�B���O�̏ꍇ
	  default:
		{
			//�^�C�g���o�[�A�Z�p���[�^�\���t���O
			showTitleBar = true;
			//list view�T�C�Y
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;
			break;
		}
	}
	
	//�^�C�g���o�[�z�u
	if( showTitleBar == true )
	{
		//�^�C�g���o�[�z�u
		AWindowPoint	pt = {0,0};
		NVI_SetControlPosition(kControlID_TitleBar,pt);
		NVI_SetControlSize(kControlID_TitleBar,windowBounds.right - windowBounds.left,height_Titlebar);
		//�^�C�g���o�[�\��
		NVI_SetShowControl(kControlID_TitleBar,true);
	}
	else
	{
		//�^�C�g���o�[��\��
		NVI_SetShowControl(kControlID_TitleBar,false);
	}
	//�Z�p���[�^�z�u
	if( showSeparator == true )
	{
		//�Z�p���[�^�z�u
		AWindowPoint	pt = {0,windowBounds.bottom-windowBounds.top-height_Separator};
		NVI_SetControlPosition(kControlID_Separator,pt);
		NVI_SetControlSize(kControlID_Separator,windowBounds.right-windowBounds.left,height_Separator);
		//�Z�p���[�^�\��
		NVI_SetShowControl(kControlID_Separator,true);
	}
	else
	{
		//�Z�p���[�^��\��
		NVI_SetShowControl(kControlID_Separator,false);
	}
	
	//list view�z�u
	{
		NVI_GetListView(kControlID_ListView).NVI_SetPosition(listViewPoint);
		NVI_GetListView(kControlID_ListView).NVI_SetSize(listViewWidth - vscrollbarWidth,listViewHeight);
	}
	
	
	//V�X�N���[���o�[�z�u
	if( vscrollbarWidth > 0 )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(kControlID_VScrollBar,pt);
		NVI_SetControlSize(kControlID_VScrollBar,vscrollbarWidth,listViewHeight);
		NVI_SetShowControl(kControlID_VScrollBar,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_VScrollBar,false);
	}
}

