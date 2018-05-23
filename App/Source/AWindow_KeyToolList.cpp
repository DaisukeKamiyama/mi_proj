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

AWindow_KeyToolList
R0073
*/

#include "stdafx.h"

#include "AWindow_KeyToolList.h"
#include "AView_KeyToolList.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowSizeBox.h"

const AFloatNumber	kFontSize = 9.0;

//#725
//List view, scrollbar
const AControlID	kControlID_ListView = 101;
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kVScrollBarWidth = 11;

//#725
//�T�C�Y�{�b�N�X
const AControlID	kControlID_SizeBox = 903;
const ANumber	kWidth_SizeBox = 15;
const ANumber	kHeight_SizeBox = 15;

//#725
//SubWindow�p�^�C�g���o�[view
const AControlID	kControlID_TitleBar = 901;

//#725
//SubWindow�p�Z�p���[�^view
const AControlID	kControlID_Separator = 902;

//#725
//�w�i�F�`��pview
const AControlID	kControlID_Background = 905;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_KeyToolList
#pragma mark ===========================
//���E�C���h�E�̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_KeyToolList::AWindow_KeyToolList():AWindow(/*#175NULL*/)
,mPreferBelow(true)
{
	//#138 �f�t�H���g�͈�(10000�`)���g������ݒ�s�v NVM_SetDynamicControlIDRange(kDynamicControlIDStart,kDynamicControlIDEnd);
}
//�f�X�g���N�^
AWindow_KeyToolList::~AWindow_KeyToolList()
{
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_KeyToolList::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 10;
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//TextView�擾
AView_KeyToolList&	AWindow_KeyToolList::GetKeyToolListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_KeyToolList,kViewType_KeyToolList,mKeyToolListViewControlID);
	/*#199
	if( NVI_GetViewByControlID(mKeyToolListViewControlID).NVI_GetViewType() != kViewType_KeyToolList )   _ACThrow("",this);
	return dynamic_cast<AView_KeyToolList&>(NVI_GetViewByControlID(mKeyToolListViewControlID));
	*/
}

//#725
/**
SubWindow�p�^�C�g���o�[�擾
*/
AView_SubWindowTitlebar&	AWindow_KeyToolList::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�R���g���[���̃N���b�N���̃R�[���o�b�N
ABool	AWindow_KeyToolList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	return result;
}

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_KeyToolList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

/**
�E�C���h�E�̕���{�^��
*/
void	AWindow_KeyToolList::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}


#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

/**
���j���[�I��������
*/
ABool	AWindow_KeyToolList::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			GetApp().SPI_CloseSubWindow(GetObjectID());
			result = true;
			break;
		}
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
void	AWindow_KeyToolList::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

//�E�C���h�E����
void	AWindow_KeyToolList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�E�C���h�E����
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//�|�b�v�A�b�v
	  case kSubWindowLocationType_Popup:
		{
			//
			//���V���h�E��false�B�V���h�E������ƁA�w�i�F�A���t�@�������ꍇ�ɁA�����ɃV���h�E�����ĉ����Ȃ�B
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
			//�T�C�Y�ݒ�
			NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kKeyToolListWindowWidth));
			NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kKeyToolListWindowHeight));
			break;
		}
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
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,false,false,true,false);
			break;
		}
	}
	
	//�w�i�F�`��pview����
	{
		AViewDefaultFactory<AView_SubWindowBackground>	viewFactory(GetObjectID(),kControlID_Background);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Background,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Background,true);
		NVI_SetControlBindings(kControlID_Background,true,true,true,true,false,false);
	}
	
	//SubWindow�p�^�C�g���o�[����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_ToolList");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwTool);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//SizeBox����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
}

//�V�K�^�u����
//�����F��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
void	AWindow_KeyToolList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//listview����
	{
		AWindowPoint	pt = {0,0}; 
		mKeyToolListViewControlID = NVM_AssignDynamicControlID();
		AViewDefaultFactory<AView_KeyToolList>	keyToolListViewFactory(GetObjectID(),mKeyToolListViewControlID);//#199
		NVM_CreateView(mKeyToolListViewControlID,pt,500,500,kControlID_Invalid,kControlID_Invalid,false,keyToolListViewFactory);
		NVI_SetControlBindings(mKeyToolListViewControlID,true,true,true,true,false,false);
	}
	//V�X�N���[���o�[����
	{
		AWindowPoint	pt = {0,0}; 
		mVScrollBarControlID = NVM_AssignDynamicControlID();
		NVI_CreateScrollBar(mVScrollBarControlID,pt,kVScrollBarWidth,kVScrollBarWidth*2);
		GetKeyToolListView().NVI_SetScrollBarControlID(kControlID_Invalid,mVScrollBarControlID);
		NVI_SetControlBindings(mVScrollBarControlID,false,true,true,true,true,false);
	}
	//SubWindow�p�Z�p���[�^����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,mKeyToolListViewControlID,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	//
	UpdateViewBounds();
	//windows�p
	NVI_RegisterToFocusGroup(mKeyToolListViewControlID);
	NVI_SwitchFocusTo(mKeyToolListViewControlID);
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_KeyToolList::NVIDO_Hide()
{
	//�E�C���h�E���̕ۑ�
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kKeyToolListWindowWidth,NVI_GetWindowWidth());
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kKeyToolListWindowHeight,NVI_GetWindowHeight());
	//����\�����ɉ����\���D��ɂ���
	mPreferBelow = true;
}

//
void	AWindow_KeyToolList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	NVI_SetWindowTransparency(0.9);
	UpdateViewBounds();
	NVI_RefreshWindow();
}

//
void	AWindow_KeyToolList::SPI_SetInfo( const AWindowID inTextWindowID, const AControlID inTextViewControlID,
										 const AGlobalPoint inPoint, const ANumber inLineHeight, 
										 const ATextArray& inTextArray, const ABoolArray& inIsUserToolArray )
{
	//���X�g�ݒ�
	GetKeyToolListView().SPI_SetInfo(inTextWindowID,inTextViewControlID,inTextArray,inIsUserToolArray);
	//�|�b�v�A�b�v���́A�E�C���h�E�T�C�Y�ɍ��킹�āi���SPI_SetInfo()�ō��ڐ��ɍ��킹�ăE�C���h�E�T�C�Y�ύX����Ă���j�A
	//�E�C���h�E�ʒu�𒲐�
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Popup )
	{
		ANumber	h = NVI_GetWindowHeight();
		AGlobalRect	screenbounds;
		NVI_GetAvailableWindowBoundsInSameScreen(screenbounds);
		APoint	pt;
		if( mPreferBelow == true )
		{
			//==================�����\���D��̏ꍇ==================
			//�����Ɉʒu�ݒ�
			pt.x = inPoint.x;
			pt.y = inPoint.y + inLineHeight + 32;//�^�C�g���o�[����������
			if( pt.y + h > screenbounds.bottom )
			{
				//�E�C���h�Ebottom���X�N���[���O�ɂȂ�Ƃ��́A��ɕ\������
				pt.y = inPoint.y - 16- h;
				//����\���D��ɂ���
				mPreferBelow = false;
			}
		}
		else
		{
			//==================����\���D��̏ꍇ==================
			//����Ɉʒu�ݒ�
			pt.x = inPoint.x;
			pt.y = inPoint.y - 16- h;
			if( pt.y < screenbounds.top )
			{
				//�E�C���h�Etop���X�N���[���O�ɂȂ�Ƃ��́A���ɕ\������
				pt.y = inPoint.y + inLineHeight + 32;//�^�C�g���o�[����������
				//�����\���D��ɂ���
				mPreferBelow = true;
			}
		}
		NVI_SetWindowPosition(pt);
	}
}

//
void	AWindow_KeyToolList::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//�^�C���o�[�A�Z�p���[�^�����擾
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	
	//�T�C�h�o�[�̍Ō�̍��ڂ��ǂ������擾
	ABool	isSideBarBottom = GetApp().SPI_IsSubWindowSideBarBottom(GetObjectID());
	
	//�w�i�F�`��view����
	ANumber	leftMargin = 1;
	ANumber	rightMargin = 1;
	ANumber	bottomMargin = 1;
	ANumber	topMargin = 0;
	AWindowPoint	backgroundPt = {0,height_Titlebar};
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			leftMargin = 0;
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			rightMargin = 0;
			break;
		}
	  case kSubWindowLocationType_Floating:
		{
			leftMargin = 0;
			rightMargin = 0;
			bottomMargin = 0;
			break;
		}
	  default:
		{
			//�|�b�v�A�b�v�̏ꍇ
			leftMargin = 0;
			rightMargin = 0;
			bottomMargin = 0;
			backgroundPt.y = 0;
			topMargin = 5;
			break;
		}
	}
	if( isSideBarBottom == true )
	{
		bottomMargin = 0;
	}
	
	//�^�C�g���o�[�A�Z�p���[�^�\���t���O
	ABool	showTitleBar = false;
	ABool	showSeparator = false;
	ABool	showSizeBox = false;
	ABool	showBackground = false;
	//list view�T�C�Y
	AWindowPoint	listViewPoint = {leftMargin,topMargin};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;
	//vscrollbar��
	ANumber	vscrollbarWidth = kVScrollBarWidth;
	//�E�T�C�h�o�[�̍ŏI���ڂ̏ꍇ�A���A�X�e�[�^�X�o�[���\�����ȊO�̓T�C�Y�{�b�N�X�̑Ώۂ͎��E�C���h�E
	NVI_SetSizeBoxTargetWindowID(GetObjectID());
	//���C�A�E�g�v�Z
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//�|�b�v�A�b�v�̏ꍇ
	  case kSubWindowLocationType_Popup:
		{
			//background view��rounded rect��\������̂ŁA���̓����ɂ����܂�悤�ɁAlist view����������������
			showBackground = true;
			listViewPoint.y += 4;
			listViewPoint.x += 5;
			listViewWidth -= 10;
			listViewHeight -= 8;
			//scroll bar�͕\�����Ȃ�
			vscrollbarWidth = 0;
			//�T�C�Y�{�b�N�X�͕\������
			showSizeBox = true;
			break;
		}
		//�T�C�h�o�[�̏ꍇ
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//�^�C�g���o�[�A�Z�p���[�^�\���t���O
			showTitleBar = true;
			showSeparator = true;
			showBackground = true;
			//list view�T�C�Y
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;// + height_Separator;
			/*
			if( isSideBarBottom == true )
			{
				showSeparator = false;
				//�E�T�C�h�o�[�̍ŏI���ڂ̏ꍇ�A���A�X�e�[�^�X�o�[���\�����̓e�L�X�g�E�C���h�E�T�C�Y�ύX�p�̃T�C�Y�{�b�N�X��\������
				if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_RightSideBar &&
							GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
				{
					showSizeBox = true;
					//�T�C�Y�{�b�N�X�Ώۂ��e�L�X�g�E�C���h�E�ɂ���
					NVI_SetSizeBoxTargetWindowID(NVI_GetOverlayParentWindowID());
				}
			}
			*/
			break;
		}
		//�t���[�e�B���O�̏ꍇ
	  default:
		{
			//�^�C�g���o�[�A�Z�p���[�^�\���t���O
			showTitleBar = true;
			showBackground = true;
			//list view�T�C�Y
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;
			//
			showSizeBox = true;
			break;
		}
	}
	//
	if( listViewHeight < 0 )   listViewHeight = 0;
	AWindowPoint	footerPoint = {leftMargin,listViewPoint.y+listViewHeight};
	ANumber	footerWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	
	//V�X�N���[���o�[�ݒ�
	ANumber	vscrollbarHeight = listViewHeight;
	if( showSizeBox == true )
	{
		vscrollbarHeight -= kHeight_SizeBox;
	}
	
	//�^�C�g���o�[�z�u
	if( showTitleBar == true )
	{
		//�^�C�g���o�[�z�u
		AWindowPoint	pt = {-1+leftMargin,0};
		NVI_SetControlPosition(kControlID_TitleBar,pt);
		NVI_SetControlSize(kControlID_TitleBar,windowBounds.right - windowBounds.left +2-leftMargin-rightMargin,height_Titlebar);
		//�^�C�g���o�[�\��
		NVI_SetShowControl(kControlID_TitleBar,true);
	}
	else
	{
		//�^�C�g���o�[��\��
		NVI_SetShowControl(kControlID_TitleBar,false);
	}
	
	//�w�i�`��view�z�u
	if( showBackground == true )
	{
		//�w�i�`��view�z�u
		NVI_SetControlPosition(kControlID_Background,backgroundPt);
		NVI_SetControlSize(kControlID_Background,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top - backgroundPt.y);
		//�w�i�`��view�\��
		NVI_SetShowControl(kControlID_Background,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_Background,false);
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
	
	//ListView�z�u�E�\��
	{
		NVI_SetControlPosition(mKeyToolListViewControlID,listViewPoint);
		NVI_SetControlSize(mKeyToolListViewControlID,listViewWidth - vscrollbarWidth,listViewHeight);
		NVI_SetShowControl(mKeyToolListViewControlID,true);
	}
	
	//V�X�N���[���o�[�z�u
	if( vscrollbarWidth > 0 && NVI_IsControlEnable(mVScrollBarControlID) == true  )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(mVScrollBarControlID,pt);
		NVI_SetControlSize(mVScrollBarControlID,vscrollbarWidth,vscrollbarHeight);
		NVI_SetShowControl(mVScrollBarControlID,true);
	}
	else
	{
		NVI_SetShowControl(mVScrollBarControlID,false);
	}
	
	//�T�C�Y�{�b�N�X�z�u
	if( showSizeBox == true )
	{
		AWindowPoint	pt = {windowBounds.right-windowBounds.left-kWidth_SizeBox - rightMargin,
		windowBounds.bottom-windowBounds.top-kHeight_SizeBox - bottomMargin};
		NVI_SetControlPosition(kControlID_SizeBox,pt);
		NVI_SetControlSize(kControlID_SizeBox,kWidth_SizeBox,kHeight_SizeBox);
		NVI_SetShowControl(kControlID_SizeBox,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_SizeBox,false);
	}
	
	//
	GetKeyToolListView().SPI_UpdateDrawProperty();
}

