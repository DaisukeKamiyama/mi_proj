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

AWindow_WordsList
#723

*/

#include "stdafx.h"

#include "AWindow_WordsList.h"
#include "AView_WordsList.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowHeader.h"
#include "AView_SubWindowStatusBar.h"
#include "AView_SubWindowSizeBox.h"

//���[�h���X�g�r���[
const AControlID	kControlID_WordsListView = 101;

//�X�N���[���o�[
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kWidth_VScroll = 11;
const AControlID	kControlID_HScrollBar = 106;
const ANumber		kHeight_HScroll = 11;

//���b�N�{�^��
const AControlID	kControlID_LockDisplay = 107;
const ANumber		kHeight_LockDisplay = 17;

//#725
//SubWindow�p�^�C�g���o�[view
const AControlID	kControlID_TitleBar = 901;

//#725
//SubWindow�p�Z�p���[�^view
const AControlID	kControlID_Separator = 902;

//#725
//�T�C�Y�{�b�N�X
const AControlID	kControlID_SizeBox = 903;
const ANumber	kWidth_SizeBox = 15;
const ANumber	kHeight_SizeBox = 15;

//#725
//�w�i�F�`��pview
const AControlID	kControlID_Background = 905;

//Import File�v���O���X�\��
const AControlID	kControlID_ImportFileProgressIndicator = 801;
const AControlID	kControlID_ImportFileProgressText = 802;
const ANumber	kHeight_ProgressArea = 20;
//const AControlID	kControlID_ImportFileProgressTextPanel = 803;

//Find�v���O���X�\��
const AControlID	kControlID_FindProgressIndicator = 811;

//�v���O���X�\���̃T�C�Y
const ANumber	kHeight_ProgressIndicator = 16;
const ANumber	kWidth_ProgressIndicator = 16;

/*
const AControlID	kControlID_ProjectFolderIsNotSetText = 812;
const AControlID	kControlID_ProjectFolderIsNotSetPanel = 813;
const ANumber	kHeight_ProjectFolderIsNotSetPanel = 40;
*/

//�w�b�_view
const AControlID	kControlID_Header = 906;
const ANumber	kHeight_Header = 36;

//�w�b�_��̃e�L�X�g
const AControlID	kControlID_FindTitle = 701;
const AControlID	kControlID_FindPath = 702;
const ANumber		kHeight_HeaderButton = 15;
const ANumber		kLeftMargin_HeaderButton = 5;
const ANumber		kRightMargin_HeaderButton = 5;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_WordsList
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_WordsList::AWindow_WordsList():AWindow(),  mShowImportFileProgress(false)
{
}
/**
�f�X�g���N�^
*/
AWindow_WordsList::~AWindow_WordsList()
{
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_WordsList::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + kHeight_Header + 10;
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

/**
���[�h���X�g�r���[�擾
*/
AView_WordsList&	AWindow_WordsList::SPI_GetWordsListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_WordsList,kViewType_WordsList,kControlID_WordsListView);
}

//#725
/**
SubWindow�p�^�C�g���o�[�擾
*/
AView_SubWindowTitlebar&	AWindow_WordsList::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
���j���[���ڑI��������
*/
ABool	AWindow_WordsList::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			//�t���[�e�B���O�E�C���h�E�̏ꍇ�̂݃E�C���h�E�N���[�Y����
			if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
			{
				GetApp().SPI_CloseSubWindow(GetObjectID());
			}
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
���j���[����update
*/
void	AWindow_WordsList::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
�R���g���[���̃N���b�N���̃R�[���o�b�N
*/
ABool	AWindow_WordsList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_LockDisplay:
		{
			SPI_GetWordsListView().SPI_SetLockedMode(NVI_GetControlBool(kControlID_LockDisplay));
			break;
		}
	}
	return result;
}

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_WordsList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
	//
	if( (inCurrentBounds.right-inCurrentBounds.left) != (inPrevBounds.right-inPrevBounds.left) ||
		(inCurrentBounds.bottom-inCurrentBounds.top) != (inPrevBounds.bottom-inPrevBounds.top) )
	{
		SPI_GetWordsListView().SPI_UpdateDrawProperty();
	}
}

/**
�E�C���h�E�̕���{�^��
*/
void	AWindow_WordsList::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}


#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_WordsList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
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
	
	//�w�b�_view����
	{
		AViewDefaultFactory<AView_SubWindowHeader>	viewFactory(GetObjectID(),kControlID_Header);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Header,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Header,true);
		NVI_SetControlBindings(kControlID_Header,true,true,true,false,false,true);
	}
	
	//�����^�C�g���{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_FindTitle,pt,10,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_FindTitle,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_FindTitle).SPI_SetButtonViewType(kButtonViewType_TextWithNoHoverWithFixedWidth);
		NVI_GetButtonViewByControlID(kControlID_FindTitle).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,10);
	}
	
	//�����p�X�{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_FindPath,pt,10,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_FindPath,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_FindPath).SPI_SetButtonViewType(kButtonViewType_TextWithNoHoverWithFixedWidth);
		NVI_GetButtonViewByControlID(kControlID_FindPath).SPI_SetEllipsisMode(kEllipsisMode_FixedFirstCharacters,20);
	}
	
	//SubWindow�p�^�C�g���o�[����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_WordsList");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwWordReferrer);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//WordsList view����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_WordsList>	viewFactory(GetObjectID(),kControlID_WordsListView);
		NVM_CreateView(kControlID_WordsListView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_WordsListView,true,true,true,true,false,false);
	}
	
	/*
	//Import File Progress�e�L�X�g�p�l������
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowStatusBar>	viewFactory(GetObjectID(),kControlID_ImportFileProgressTextPanel);
		NVM_CreateView(kControlID_ImportFileProgressTextPanel,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_ImportFileProgressTextPanel,true,true,true,false,false,true);
		//�w�i�F���ݒ�
		NVI_GetViewByControlID(kControlID_ImportFileProgressTextPanel).NVI_SetBackgroundColor(
					GetApp().SPI_GetSubWindowBackgroundColor(true),
					GetApp().SPI_GetSubWindowBackgroundColor(false));
	}
	*/
	
	//�w�b�_�����t�H���g
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	//Import File Progress�e�L�X�g����
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateButtonView(kControlID_ImportFileProgressText,pt,10,kHeight_ProgressIndicator,
							 kControlID_Background);//kControlID_ImportFileProgressTextPanel);
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).SPI_SetButtonViewType(kButtonViewType_NoFrame);
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,kTextStyle_Normal);//#1316
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).SPI_SetDropShadowColor(kColor_Gray80Percent);
		//
		AText	text;
		text.SetLocalizedText("Progress_ImportFile");
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).NVI_SetText(text);
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).SPI_SetWidthToFitTextWidth();
	}
	
	//Import File Progress Indicator����
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateProgressIndicator(kControlID_ImportFileProgressIndicator,pt,kWidth_ProgressIndicator,kHeight_ProgressIndicator);
	}
	
	//Find Progress Indicator����
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateProgressIndicator(kControlID_FindProgressIndicator,pt,kWidth_ProgressIndicator,kHeight_ProgressIndicator);
		NVI_EmbedControl(kControlID_WordsListView,kControlID_FindProgressIndicator);
	}
	
	/*
	
	//Project Folder���ݒ�e�L�X�g�p�l������
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_TextPanel>	viewFactory(GetObjectID(),kControlID_ProjectFolderIsNotSetPanel);
		NVM_CreateView(kControlID_ProjectFolderIsNotSetPanel,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_ProjectFolderIsNotSetPanel,true,true,true,false,false,true);
		//�w�i�F���ݒ�
		NVI_GetViewByControlID(kControlID_ProjectFolderIsNotSetPanel).NVI_SetBackgroundColor(
					GetApp().SPI_GetSubWindowBackgroundColor(true),
					GetApp().SPI_GetSubWindowBackgroundColor(false));
	}
	
	//Import File Progress�e�L�X�g����
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateButtonView(kControlID_ProjectFolderIsNotSetText,pt,10,kHeight_ProgressIndicator,kControlID_ProjectFolderIsNotSetPanel);
		NVI_GetButtonViewByControlID(kControlID_ProjectFolderIsNotSetText).SPI_SetButtonViewType(kButtonViewType_NoFrame);
		NVI_GetButtonViewByControlID(kControlID_ProjectFolderIsNotSetText).SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,
					kTextStyle_DropShadow,kColor_Black,kColor_Black);
		NVI_GetButtonViewByControlID(kControlID_ProjectFolderIsNotSetText).SPI_SetDropShadowColor(kColor_Gray80Percent);
		//
		AText	text;
		text.SetLocalizedText("Progress_ImportFile");
		NVI_GetButtonViewByControlID(kControlID_ProjectFolderIsNotSetText).NVI_SetText(text);
		NVI_GetButtonViewByControlID(kControlID_ProjectFolderIsNotSetText).SPI_SetWidthToFitTextWidth();
	}
	*/
	//==================
	
	//SubWindow�p�Z�p���[�^����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_WordsListView,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//V�X�N���[���o�[����
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateScrollBar(kControlID_VScrollBar,pt,kWidth_VScroll,kWidth_VScroll*2);
		SPI_GetWordsListView().NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
		NVI_SetShowControl(kControlID_VScrollBar,true);
		NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	}
	
	//Lock�{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_LockDisplay,pt,10,kHeight_LockDisplay,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_LockDisplay,false,true,false,false,false,true);
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).
				SPI_SetOvalHoverColor(GetApp().SPI_GetSubWindowTitlebarButtonHoverColor());
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).SPI_SetIconImageID(kImageID_iconSwUnlock,
																				kImageID_Invalid,kImageID_iconSwLock);
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).SPI_SetToggleMode(true);
		AText	text;
		text.SetLocalizedText("WordsListLock");
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).NVI_SetText(text);
	}
	
	//SizeBox����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
	
	//�w�b�_�e�L�X�g�����e�L�X�g�ݒ�
	SPI_SetHeaderText(GetEmptyText(),GetEmptyText(),kIndex_Invalid);
}

/**
�V�K�^�u����
�����F��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_WordsList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	UpdateViewBounds();
}

/**
Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
*/
void	AWindow_WordsList::NVIDO_Hide()
{
}

/**
�{�^�����v���p�e�B�X�V
*/
void	AWindow_WordsList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//�������v���O���X�X�V
	NVI_SetShowControl(kControlID_FindProgressIndicator,SPI_GetWordsListView().SPI_IsFinding());
	NVI_SetControlBool(kControlID_FindProgressIndicator,true);
	
	//�w�b�_�����t�H���g�A�F
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
    //#1316 AColor	headerlettercolor = GetApp().SPI_GetSubWindowHeaderLetterColor();
	NVI_GetButtonViewByControlID(kControlID_FindTitle).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_FindPath).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,kTextStyle_Bold);//#1316
	
	//���b�N�{�^���F�ݒ�
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);
    //#1316 AColor	color = GetApp().SPI_GetSubWindowTitlebarTextColor();
	NVI_GetButtonViewByControlID(kControlID_LockDisplay).
			SPI_SetTextProperty(fontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	
	//view bounds�X�V
	UpdateViewBounds();
	NVI_RefreshWindow();
}

/**
view bounds�X�V
*/
void	AWindow_WordsList::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//�^�C���o�[�A�Z�p���[�^�����擾
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	ANumber	height_Header = kHeight_Header;
	
	//�T�C�h�o�[�̈�ԉ��̍��ڂ��ǂ������擾
	ABool	isSideBarBottom = GetApp().SPI_IsSubWindowSideBarBottom(GetObjectID());
	
	//�w�i�F�`��view����
	ANumber	leftMargin = 1;
	ANumber	rightMargin = 1;
	ANumber	bottomMargin = 1;
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
	  default:
		{
			leftMargin = 0;
			rightMargin = 0;
			bottomMargin = 0;
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
	//list view�T�C�Y
	AWindowPoint	listViewPoint = {leftMargin,0};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - listViewPoint.x - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;// - kHeight_HScroll;
	//�w�b�_�[�����\���ʒu�E�T�C�Y
	AWindowPoint	headerViewPoint = {leftMargin,0};
	//ANumber	headerViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	//�X�N���[���o�[��
	ANumber	vscrollbarWidth = kWidth_VScroll;
	//�E�T�C�h�o�[�̍ŏI���ڂ̏ꍇ�A���A�X�e�[�^�X�o�[���\�����ȊO�̓T�C�Y�{�b�N�X�̑Ώۂ͎��E�C���h�E
	NVI_SetSizeBoxTargetWindowID(GetObjectID());
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
			listViewPoint.y += height_Titlebar /*+ kHeight_HeaderButtons*/ + height_Header;
			listViewHeight -= height_Titlebar /*+ kHeight_HeaderButtons*/ + height_Header;// + height_Separator;
			//�w�b�_�[�����\���ʒu
			headerViewPoint.y += height_Titlebar;// + kHeight_HeaderButtons;
			/*
			//�E�T�C�h�o�[�̍ŏI���ڂ̏ꍇ�A��������\��
			if( isSideBarBottom == true )
			{
				//�Z�p���[�^��\��
				showSeparator = false;
				//�E�T�C�h�o�[�̍ŏI���ڂ̏ꍇ�A���A�X�e�[�^�X�o�[���\�����̓T�C�Y�{�b�N�X��\������
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
			//list view�T�C�Y
			listViewPoint.y += height_Titlebar /*+ kHeight_HeaderButtons*/ + height_Header;
			listViewHeight -= height_Titlebar /*+ kHeight_HeaderButtons*/ + height_Header;
			//�w�b�_�[�����\���ʒu
			headerViewPoint.y += height_Titlebar;// + kHeight_HeaderButtons;
			//�T�C�Y�{�b�N�X�\��
			showSizeBox = true;
			break;
		}
	}
	
	//V�X�N���[���o�[�ݒ�
	ANumber	vscrollbarHeight = listViewHeight;
	if( showSizeBox == true )
	{
		vscrollbarHeight -= kHeight_SizeBox;
	}
	
	/*
	//�v���W�F�N�g�t�H���_���ݒ�\�� �z�u�v�Z
	ABool	showProjectFolderIsNotSet = false;
	ANumber	showProjectFolderIsNotSetHeight = 0;
	AWindowPoint	projectFolderIsNotSetPoint = {0};
	if( SPI_GetWordsListView().SPI_GetSearchInProjectFolder() == false )
	{
		//
		showProjectFolderIsNotSet = true;
		showProjectFolderIsNotSetHeight = kHeight_ProjectFolderIsNotSetPanel;
		//
		//list view�������v���O���X�o�[�����炷
		listViewHeight -= showProjectFolderIsNotSetHeight;
		//�v���O���X�o�[�ʒu�v�Z
		projectFolderIsNotSetPoint.x = listViewPoint.x;
		projectFolderIsNotSetPoint.y = listViewPoint.y + listViewHeight;
	}
	*/
	
	//�v���O���X�o�[�z�u�v�Z
	ABool	showProgress = mShowImportFileProgress;
	if( listViewHeight < kHeight_ProgressArea )   showProgress = false;
	AWindowPoint	progressPoint = {0};
	if( showProgress == true )
	{
		//list view�������v���O���X�o�[�����炷
		listViewHeight -= kHeight_ProgressArea;
		//�v���O���X�o�[�ʒu�v�Z
		progressPoint.x = leftMargin;
		progressPoint.y = listViewPoint.y + listViewHeight;// + showProjectFolderIsNotSetHeight;
		//
		//projectFolderIsNotSetPoint.y -= kHeight_ProgressArea;
	}
	
	//�^�C�g���o�[�z�u
	{
		//�^�C�g���o�[�z�u
		AWindowPoint	pt = {-1+leftMargin,0};
		NVI_SetControlPosition(kControlID_TitleBar,pt);
		NVI_SetControlSize(kControlID_TitleBar,windowBounds.right - windowBounds.left +2-leftMargin-rightMargin,height_Titlebar);
		//�^�C�g���o�[�\��
		NVI_SetShowControl(kControlID_TitleBar,true);
	}
	
	//�w�i�`��view�z�u
	{
		//�w�i�`��view�z�u
		NVI_SetControlPosition(kControlID_Background,backgroundPt);
		NVI_SetControlSize(kControlID_Background,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top - backgroundPt.y);
		//�w�i�`��view�\��
		NVI_SetShowControl(kControlID_Background,true);
	}
	
	//==================Words List view�z�u==================
	{
		NVI_SetControlPosition(kControlID_WordsListView,listViewPoint);
		NVI_SetControlSize(kControlID_WordsListView,listViewWidth - vscrollbarWidth,listViewHeight);
	}
	
	/*
	//�v���W�F�N�g�t�H���_���ݒ�e�L�X�g�p�l���z�u
	{
		NVI_SetControlPosition(kControlID_ProjectFolderIsNotSetPanel,projectFolderIsNotSetPoint);
		NVI_SetControlSize(kControlID_ProjectFolderIsNotSetPanel,windowBounds.right - windowBounds.left - vscrollbarWidth,kHeight_ProjectFolderIsNotSetPanel);
		NVI_SetShowControl(kControlID_ProjectFolderIsNotSetPanel,showProjectFolderIsNotSet);
	}
	*/
	//==================ImportFileProgress�z�u==================
	
	/*
	//Import File Progress�e�L�X�g�p�l���z�u�E�\��
	{
		NVI_SetControlPosition(kControlID_ImportFileProgressTextPanel,progressPoint);
		NVI_SetControlSize(kControlID_ImportFileProgressTextPanel,windowBounds.right - windowBounds.left - vscrollbarWidth,kHeight_ProgressArea);
		NVI_SetShowControl(kControlID_ImportFileProgressTextPanel,showProgress);
	}
	*/
	//Import File Progress Indicator�z�u
	{
		AWindowPoint	pt = {0};
		pt.x = progressPoint.x + 5;
		pt.y = progressPoint.y + 2;
		NVI_SetControlPosition(kControlID_ImportFileProgressIndicator,pt);
		NVI_SetShowControl(kControlID_ImportFileProgressIndicator,showProgress);
		//
		NVI_SetControlBool(kControlID_ImportFileProgressIndicator,true);
	}
	//Import File Progress Text�z�u
	{
		AWindowPoint	pt = {0};
		pt.x = progressPoint.x + 23;
		pt.y = progressPoint.y + 3;
		NVI_SetControlPosition(kControlID_ImportFileProgressText,pt);
		NVI_SetShowControl(kControlID_ImportFileProgressText,showProgress);
	}
	
	//Find Progress Indicator�z�u
	{
		AWindowPoint	pt = {0};
		pt.x = listViewPoint.x + 5;
		pt.y = listViewPoint.y + 2;
		NVI_SetControlPosition(kControlID_FindProgressIndicator,pt);
		NVI_SetShowControl(kControlID_FindProgressIndicator,showProgress);
		//
		NVI_SetShowControl(kControlID_FindProgressIndicator,SPI_GetWordsListView().SPI_IsFinding());
		NVI_SetControlBool(kControlID_FindProgressIndicator,true);
	}
	//V�X�N���[���o�[�z�u
	if( vscrollbarWidth > 0 && NVI_IsControlEnable(kControlID_VScrollBar) == true )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(kControlID_VScrollBar,pt);
		NVI_SetControlSize(kControlID_VScrollBar,vscrollbarWidth,vscrollbarHeight);
		NVI_SetShowControl(kControlID_VScrollBar,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_VScrollBar,false);
	}
	
	//�w�b�_�z�u
	if( height_Header > 0 )
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar};
		NVI_SetControlPosition(kControlID_Header,pt);
		NVI_SetControlSize(kControlID_Header,windowBounds.right-windowBounds.left - leftMargin - rightMargin,height_Header);
		NVI_SetShowControl(kControlID_Header,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_Header,false);
	}
	
	//�����^�C�g���{�^���z�u
	{
		//NVI_GetButtonViewByControlID(kControlID_FindTitle).SPI_SetWidthToFitTextWidth();
		//ANumber	w = NVI_GetButtonViewByControlID(kControlID_FindTitle).NVI_GetViewWidth();
		AWindowPoint	pt = {kLeftMargin_HeaderButton,height_Titlebar + 2};
		NVI_SetControlPosition(kControlID_FindTitle,pt);
		NVI_SetControlSize(kControlID_FindTitle,windowBounds.right-windowBounds.left-kLeftMargin_HeaderButton-kRightMargin_HeaderButton,kHeight_HeaderButton);
	}
	
	//�����p�X�{�^���z�u
	{
		//NVI_GetButtonViewByControlID(kControlID_FindPath).SPI_SetWidthToFitTextWidth();
		//ANumber	w = NVI_GetButtonViewByControlID(kControlID_FindPath).NVI_GetViewWidth();
		AWindowPoint	pt = {kLeftMargin_HeaderButton,height_Titlebar + 19};
		NVI_SetControlPosition(kControlID_FindPath,pt);
		NVI_SetControlSize(kControlID_FindPath,windowBounds.right-windowBounds.left-kLeftMargin_HeaderButton-kRightMargin_HeaderButton,kHeight_HeaderButton);
	}
	
	//Lock�{�^���z�u
	{
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).SPI_SetWidthToFitTextWidth();
		ANumber	w = NVI_GetButtonViewByControlID(kControlID_LockDisplay).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right - windowBounds.left - w - 5,1};
		NVI_SetControlPosition(kControlID_LockDisplay,pt);
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
}

/**
Import File Progress�̕\���^��\��
*/
void	AWindow_WordsList::SPI_ShowHideImportFileProgress( const ABool inShow )
{
	if( mShowImportFileProgress == inShow )
	{
		return;
	}
	
	//�\���^��\���ύX
	mShowImportFileProgress = inShow;
	UpdateViewBounds();
}

/**
�w�b�_�����̃e�L�X�g�X�V
*/
void	AWindow_WordsList::SPI_SetHeaderText( const AText& inWord, const AText& inProjectPath, const AModeIndex inModeIndex )
{
	//
	AText	headertext;
	headertext.SetLocalizedText("WordsListHeader");
	headertext.ReplaceParamText('0',inWord);
	AText	modename;
	if( inModeIndex != kIndex_Invalid )
	{
		GetApp().SPI_GetModePrefDB(inModeIndex).GetModeDisplayName(modename);
	}
	//
	AText	headertext2;
	headertext2.SetLocalizedText("WordsListHeader2");
	headertext2.ReplaceParamText('0',inProjectPath);
	headertext2.ReplaceParamText('1',modename);
	//
	NVI_GetViewByControlID(kControlID_FindTitle).NVI_SetText(headertext);
	NVI_GetViewByControlID(kControlID_FindPath).NVI_SetText(headertext2);
	//UpdateViewBounds();
}

/**
���b�N���[�h�ݒ�
*/
void	AWindow_WordsList::SPI_SetLockedMode( const ABool inLocked )
{
	NVI_SetControlBool(kControlID_LockDisplay,inLocked);
	SPI_GetWordsListView().SPI_SetLockedMode(inLocked);
}

