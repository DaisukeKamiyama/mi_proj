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

AWindow_IdInfo

*/

#include "stdafx.h"

#include "AWindow_IdInfo.h"
#include "AView_IdInfo.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowSizeBox.h"

//const AControlID	kDisplayCommentButtonID = 100;
//#725 const AControlID	kFixDisplayButtonID = 101;
//#725 const AControlID	kHistoryButtonID = 102;//#238

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

//#725
//�����{�b�N�X
const AControlID	kControlID_SearchBox = 101;
const ANumber		kHeight_SearchBoxMargin			= 8;

//#725
//�|�b�v�A�b�v�ݒ�{�^��
//const AControlID	kControlID_PopupSetupButton = 102;
//const ANumber		kWidth_PopupSetupButton = 80;
//�|�b�v�A�b�v�N���[�Y�{�^��
const AControlID	kControlID_PopupCloseButton = 103;
const ANumber		kWidth_PopupCloseButton = 80;
//�|�b�v�A�b�v�N���[�Y�{�^������
const ANumber		kHeight_PopupButtons = 20;

/*#725
//const ANumber	kDisplayCommentButtonWidth = 80;
const ANumber	kDisplayCommentButtonHeight = 19;
//#238
const ANumber	kFixDisplayButtonWidth = 80;

#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kButtonFontSize = 9.0;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AFloatNumber	kButtonFontSize = 8.0;
#endif
*/

#pragma mark ===========================
#pragma mark [�N���X]AWindow_IdInfo
#pragma mark ===========================
//���E�C���h�E�̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_IdInfo::AWindow_IdInfo():AWindow(/*#175NULL*/)//#238, mFixDisplay(false)
,mInfoViewControlID(kControlID_Invalid)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kIdInfoWindowPosition);
	//#138 �f�t�H���g�͈�(10000�`)���g������ݒ�s�v NVM_SetDynamicControlIDRange(kDynamicControlIDStart,kDynamicControlIDEnd);
	//#725 �^�C�}�[�ΏۃE�C���h�E�ɂ���
	NVI_AddToTimerActionWindowArray();
}
//�f�X�g���N�^
AWindow_IdInfo::~AWindow_IdInfo()
{
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_IdInfo::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 20 + kHeight_SearchBoxMargin + 10;
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//TextView�擾
AView_IdInfo&	AWindow_IdInfo::GetIdInfoView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_IdInfo,kViewType_IdInfo,mInfoViewControlID);
	/*#199
	if( NVI_GetViewByControlID(mInfoViewControlID).NVI_GetViewType() != kViewType_IdInfo )   _ACThrow("",this);
	return dynamic_cast<AView_IdInfo&>(NVI_GetViewByControlID(mInfoViewControlID));
	*/
}
const AView_IdInfo&	AWindow_IdInfo::GetIdInfoViewConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_IdInfo,kViewType_IdInfo,mInfoViewControlID);
	/*#199
	if( NVI_GetViewConstByControlID(mInfoViewControlID).NVI_GetViewType() != kViewType_IdInfo )   _ACThrow("",this);
	return dynamic_cast<const AView_IdInfo&>(NVI_GetViewConstByControlID(mInfoViewControlID));
	*/
}

//#238
/**
IdInfoView�擾
*/
AView_IdInfo&	AWindow_IdInfo::SPI_GetIdInfoView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_IdInfo,kViewType_IdInfo,mInfoViewControlID);
}

//#725
/**
SubWindow�p�^�C�g���o�[�擾
*/
AView_SubWindowTitlebar&	AWindow_IdInfo::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
���j���[�I��������
*/
ABool	AWindow_IdInfo::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
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
		//�T�u�E�C���h�E��܂肽���� #1380
	  case kMenuItemID_CollapseThisSubwindow:
		{
			AWindowID	textWindowID = NVI_GetOverlayParentWindowID();
			if( textWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(textWindowID).SPI_ExpandCollapseSubWindow(GetObjectID());
			}
			break;
		}
		//�T�u�E�C���h�E����� #1380
	  case kMenuItemID_CloseThisSubwindow:
		{
			AWindowID	textWindowID = NVI_GetOverlayParentWindowID();
			if( textWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(textWindowID).SPI_CloseOverlaySubWindow(GetObjectID());
			}
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
void	AWindow_IdInfo::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

//�R���g���[���̃N���b�N���̃R�[���o�b�N
ABool	AWindow_IdInfo::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_PopupCloseButton:
		{
			NVI_Hide();
			break;
		}
		/*#725 �\���Œ�{�^���p�~
	  case kFixDisplayButtonID:
		{
			//#238 mFixDisplay = NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_GetToggleOn();//#232NVI_GetControlBool(kFixDisplayButtonID);
			SPI_GetIdInfoView().SPI_SetFixDisplay(!SPI_GetIdInfoView().SPI_GetFixDisplay());//#238
			result = true;
			break;
		}
		*/
	}
	return result;
}

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_IdInfo::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

/*#725
//#291
void	AWindow_IdInfo::EVTDO_DoCloseButton()
{
	GetApp().SPI_ShowHideIdInfoWindow(false);
}
*/

/**
�R���g���[����text���͎�����
*/
void	AWindow_IdInfo::EVTDO_TextInputted( const AControlID inID )
{
	//�T�[�`�{�b�N�X�œ��͎�
	if( inID == kControlID_SearchBox )
	{
		//�T�[�`�{�b�N�X�̃e�L�X�g�擾
		AText	text;
		NVI_GetControlText(kControlID_SearchBox,text);
		
		//�őO�ʂ̃e�L�X�g�h�L�������g���擾
		ADocumentID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
		//IdInfo�����X���b�h�ɗv���ݒ�
		GetApp().SPI_GetIdInfoFinderThread().SetRequestData(docID,
															GetObjectID(),kObjectID_Invalid,kObjectID_Invalid,
															text,GetEmptyTextArray(),kIndex_Invalid,GetEmptyText(),
															kIndex_Invalid);
		//�X���b�hpause����
		GetApp().SPI_GetIdInfoFinderThread().NVI_UnpauseIfPaused();
	}
}

//#853
/**
�R���g���[���Ń��^�[���L�[����������
*/
ABool	AWindow_IdInfo::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )
{
	//�T�[�`�{�b�N�X�Ń��^�[���L�[�������A���ڂ̒�`�ӏ����J��
	if( inControlID == kControlID_SearchBox )
	{
		GetIdInfoView().SPI_OpenItem(GetIdInfoView().SPI_GetSelectedIndex(),kModifierKeysMask_None);
	}
	return true;
}

//#798
/**
�R���g���[����ESC�L�[����������
*/
ABool	AWindow_IdInfo::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )
{
	//�t�B���^�œ��͎�
	if( inControlID == kControlID_SearchBox )
	{
		//�t�B���^�e�L�X�g����ɂ���
		NVI_SetControlText(kControlID_SearchBox,GetEmptyText());
		
		//�e�L�X�g�E�C���h�E�փt�H�[�J�X�ړ�
		if( NVM_GetOverlayMode() == true )
		{
			GetApp().NVI_GetWindowByID(NVI_GetOverlayParentWindowID()).NVI_SelectWindow();
		}
	}
	return true;
}

//#853
/**
�R���g���[���ŏ�L�[����������
*/
ABool	AWindow_IdInfo::NVIDO_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�T�[�`�{�b�N�X�ŏ�L�[�������A�O�̍��ڂ�I��
	if( inControlID == kControlID_SearchBox )
	{
		GetIdInfoView().SPI_SelectPrev();
	}
	return true;
}

//#853
/**
�R���g���[���ŉ��L�[����������
*/
ABool	AWindow_IdInfo::NVIDO_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�T�[�`�{�b�N�X�ŏ�L�[�������A���̍��ڂ�I��
	if( inControlID == kControlID_SearchBox )
	{
		GetIdInfoView().SPI_SelectNext();
	}
	return true;
}

/**
�E�C���h�E�̕���{�^��
*/
void	AWindow_IdInfo::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}

/**
tick�^�C�}�[����
*/
void	AWindow_IdInfo::EVTDO_DoTickTimerAction()
{
	if( NVI_IsWindowVisible() == true )
	{
		GetIdInfoView().SPI_DoTickTimer();
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_IdInfo::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�E�C���h�E����
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//�|�b�v�A�b�v
	  case kSubWindowLocationType_Popup:
		{
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
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
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,false,false,false,false);
			break;
		}
	}
	
	//���T�C�Y�ݒ�K�v�H
	/*
	if( NVM_GetOverlayMode() == false )//#291
	{
		//�E�C���h�E���̐ݒ�
		NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIdInfoWindowWidth));
		NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIdInfoWindowHeight));
	}
	*/
	
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
		text.SetLocalizedText("SubWindowTitle_IdInfo");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwKeyWord);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
	//�����{�b�N�X�𐶐�
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_SearchBox,pt,10,10,kControlID_Invalid,kIndex_Invalid,true,false,false,true,kEditBoxType_FilterBox);
		NVI_SetControlBindings(kControlID_SearchBox,true,true,true,false,false,true);
		//�t�H���g�ݒ�
		NVI_GetEditBoxView(kControlID_SearchBox).NVI_SetTextFont(fontname,fontsize);
		/*#1316 �F��EditBox���Őݒ�
		NVI_GetEditBoxView(kControlID_SearchBox).SPI_SetBackgroundColor(
					kColor_White,kColor_Gray70Percent,kColor_Gray70Percent);
		NVI_GetEditBoxView(kControlID_SearchBox).SPI_SetTextColor(
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"),
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"));
					*/
		NVI_SetAutomaticSelectBySwitchFocus(kControlID_SearchBox,true);
		NVI_GetEditBoxView(kControlID_SearchBox).SPI_SetEnableFocusRing(false);
	}
	
	/*
	//�|�b�v�A�b�v�ݒ�{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_PopupSetupButton,pt,10,10,kControlID_Invalid);
		NVI_GetButtonViewByControlID(kControlID_PopupSetupButton).SPI_SetTextProperty(fontname,fontsize,kJustification_Center,kTextStyle_Normal);
		NVI_GetButtonViewByControlID(kControlID_PopupSetupButton).SPI_SetButtonViewType(kButtonViewType_NoFrame);
		NVI_GetButtonViewByControlID(kControlID_PopupSetupButton).SPI_SetIcon(kImageID_iconPnOption,kWidth_PopupSetupButton-16,5);
		AText	text("config");
		//NVI_GetButtonViewByControlID(kControlID_PopupSetupButton).NVI_SetText(text);
	}
	*/
	//�|�b�v�A�b�v�N���[�Y�{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_PopupCloseButton,pt,10,10,kControlID_Invalid);
		NVI_GetButtonViewByControlID(kControlID_PopupCloseButton).SPI_SetTextProperty(fontname,14.0,kJustification_Left,kTextStyle_Normal);
		NVI_GetButtonViewByControlID(kControlID_PopupCloseButton).SPI_SetButtonViewType(kButtonViewType_NoFrameWithTextHover);
		AText	text;
		text.SetLocalizedText("PopupClose");
		NVI_GetButtonViewByControlID(kControlID_PopupCloseButton).NVI_SetText(text);
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
void	AWindow_IdInfo::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontName;
	AFloatNumber	fontSize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontName,fontSize);
	
	AWindowPoint	pt = {0,0};
	//IdInfoView����
	mInfoViewControlID = NVM_AssignDynamicControlID();
	AViewDefaultFactory<AView_IdInfo>	infoViewFactory(GetObjectID(),mInfoViewControlID);//#199
	NVM_CreateView(mInfoViewControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,infoViewFactory);
	/*#725
	if( NVM_GetOverlayMode() == true )//#291
	{
		GetIdInfoView().SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
	}
	*/
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(mInfoViewControlID,true,true,true,true,false,false);
	//#442
	//#1380 GetIdInfoView().SPI_SetContextMenuItemID(kContextMenuID_GeneralSubWindow);//#442 #1380
	//#507
	//#1370 NVI_EnableHelpTagCallback(mInfoViewControlID,false);
	
	//SubWindow�p�Z�p���[�^����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,mInfoViewControlID,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//V�X�N���[���o�[����
	mVScrollBarControlID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(mVScrollBarControlID,pt,kVScrollBarWidth,kVScrollBarWidth*2);
	GetIdInfoView().NVI_SetScrollBarControlID(kControlID_Invalid,mVScrollBarControlID);
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(mVScrollBarControlID,false,true,true,true,true,false);
	
	
	/*#725
	//�\���Œ�{�^������
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);//#375
	NVI_CreateButtonView(kFixDisplayButtonID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);
	AText	t;
	t.SetLocalizedText("IdInfo_FixDisplayButton");
	NVI_GetButtonViewByControlID(kFixDisplayButtonID).NVI_SetText(t);
	NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetToggleMode(true);
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetAlwaysActiveColors(true);
	}
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(kFixDisplayButtonID,false,true,true,false,true,true);
	
	//#238
	NVI_CreateButtonView(kHistoryButtonID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);//#446
	NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetMenu(kContextMenuID_IdInfoHistory,kMenuItemID_IdInfoHistory);
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetAlwaysActiveColors(true);
	}
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(kHistoryButtonID,true,true,true,false,false,true);
	//#446
	t.SetLocalizedText("IdInfo");
	NVI_GetButtonViewByControlID(kHistoryButtonID).NVI_SetText(t);
	
	//#661
	{
		AText	text;
		text.SetLocalizedText("HelpTag_IdInfo");
		NVI_SetHelpTag(kHistoryButtonID,text);
		text.SetLocalizedText("HelpTag_IdInfoFixDisplay");
		NVI_SetHelpTag(kFixDisplayButtonID,text);
	}
	//#634 �w�i�F�ݒ�
	SPI_SetSideBarMode(false,kColor_Gray97Percent);
	*/
	//
	UpdateViewBounds();
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_IdInfo::NVIDO_Hide()
{
	//������
	/*
	if( NVM_GetOverlayMode() == false )//#291 Overlay���[�h���ɂ͕ۑ����Ȃ�
	{
		//�E�C���h�E���̕ۑ�
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIdInfoWindowWidth,NVI_GetWindowWidth());
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIdInfoWindowHeight,NVI_GetWindowHeight());
	}
	*/
}

//
void	AWindow_IdInfo::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	/*#725
	if( NVM_GetOverlayMode() == false )//#291
	{
		NVI_SetWindowTransparency((static_cast<AFloatNumber>(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIdInfoWindowAlphaPercent)))/100);
	}
	else
	{
		//Overlay���͕s�����i�������AMacOSX�ł͎��ۂɂ͉������Ȃ��BView���œh��Ԃ����ɓ����x�K�p�B�j
		NVI_SetWindowTransparency(0.5);
	}
	*/
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//�e�L�X�g�����͎��e�L�X�g�擾
	AText	filtertext;
	filtertext.SetLocalizedText("SearchKeyword");
	//�L�[���[�h��񌟍��p�̃V���[�g�J�b�g�L�[���擾
	AText	shortcuttext;
	GetApp().NVI_GetMenuManager().GetMenuItemShortcutDisplayText(kMenuItemID_SearchInKeywordList,shortcuttext);
	//�e�L�X�g�����͎��e�L�X�g�ݒ�
	NVI_GetEditBoxView(kControlID_SearchBox).SPI_SetTextForEmptyState(filtertext,shortcuttext);
	//�����{�b�N�X�Ƀt�H���g�ݒ�
	NVI_GetEditBoxView(kControlID_SearchBox).NVI_SetTextFont(fontname,fontsize);
	/*#1316 �F��EditBox���Őݒ�
	NVI_GetEditBoxView(kControlID_SearchBox).SPI_SetBackgroundColorForEmptyState(
																				 GetApp().SPI_GetSubWindowBackgroundColor(true),
																				 GetApp().SPI_GetSubWindowBackgroundColor(false));
																				 */
	//#1380
	//�E�C���h�E�^�C�v�ɉ����ăR���e�L�X�g���j���[�ݒ�
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
	  case kSubWindowLocationType_RightSideBar:
		{
			NVI_GetViewByControlID(mInfoViewControlID).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow_RightSideBar);
			NVI_GetViewByControlID(kControlID_TitleBar).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow_RightSideBar);
			break;
		}
	  case kSubWindowLocationType_LeftSideBar:
		{
			NVI_GetViewByControlID(mInfoViewControlID).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow_LeftSideBar);
			NVI_GetViewByControlID(kControlID_TitleBar).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow_LeftSideBar);
			break;
		}
	  default:
		{
			NVI_GetViewByControlID(mInfoViewControlID).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow);
			NVI_GetViewByControlID(kControlID_TitleBar).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow);
			break;
		}
	}
	//view bounds�X�V
	UpdateViewBounds();
	//
	GetIdInfoView().SPI_UpdateDisplayComment();
	NVI_RefreshWindow();
}

/*#238
//
void	AWindow_IdInfo::SPI_SetInfo( const AText& inIdText, const AModeIndex inModeIndex, 
		const ATextArray& inInfoTextArray, const AObjectArray<AFileAcc>& inFileArray, 
		const ATextArray& inCommentTextArray, const AArray<AIndex>& inCategoryIndexArray,
		const AArray<AScopeType>& inScopeTypeArray,
		const AArray<AIndex>& inStartIndexArray, const AArray<AIndex>& inEndIndexArray,
		const ATextArray& inParentKeywordArray )
{
	if( mFixDisplay == true )   return;
	
	GetIdInfoView().SPI_SetInfo(inIdText,inModeIndex,inInfoTextArray,inFileArray,inCommentTextArray,inCategoryIndexArray,inScopeTypeArray,
			inStartIndexArray,inEndIndexArray,inParentKeywordArray);
}
*/

//#798
/**
�����{�b�N�X�փt�H�[�J�X�ړ�
*/
void	AWindow_IdInfo::SPI_SwitchFocusToSearchBox()
{
	NVI_SelectWindow();
	NVI_SwitchFocusTo(kControlID_SearchBox);
	NVI_GetEditBoxView(kControlID_SearchBox).NVI_SetSelectAll();
}

/*:
�E�C���h�E�̍ő卂���擾�i�|�b�v�A�b�v���̃E�C���h�E���������̂��߁j
*/
ANumber	AWindow_IdInfo::SPI_GetMaxWindowHeight() const
{
	return GetIdInfoViewConst().NVI_GetImageHeight() + kHeight_PopupButtons;
}

/**
view bounds�X�V
*/
void	AWindow_IdInfo::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//�^�C���o�[�A�Z�p���[�^�����擾
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	ANumber	height_Filter = NVI_GetEditBoxView(kControlID_SearchBox).SPI_GetLineHeightWithMargin() + kHeight_SearchBoxMargin;
	
	//�T�C�h�o�[�̍Ō�̍��ڂ��ǂ������擾
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
	ABool	showBackground = false;
	ABool	showSizeBox = false;
	//list view�T�C�Y
	AWindowPoint	listViewPoint = {leftMargin,0};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - listViewPoint.x - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;
	//�X�N���[���o�[��
	ANumber	vscrollbarWidth = kVScrollBarWidth;
	//�E�T�C�h�o�[�̍ŏI���ڂ̏ꍇ�A���A�X�e�[�^�X�o�[���\�����ȊO�̓T�C�Y�{�b�N�X�̑Ώۂ͎��E�C���h�E
	NVI_SetSizeBoxTargetWindowID(GetObjectID());
	//���C�A�E�g�v�Z
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//�|�b�v�A�b�v�̏ꍇ
	  case kSubWindowLocationType_Popup:
		{
			listViewPoint.y += kHeight_PopupButtons;
			listViewHeight -= kHeight_PopupButtons;
			vscrollbarWidth = 0;
			break;
		}
		//�T�C�h�o�[�̏ꍇ
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//�^�C�g���o�[�A�Z�p���[�^�\���t���O
			showTitleBar = true;
			showSeparator = true;
			//list view�T�C�Y
			listViewPoint.y += height_Titlebar + height_Filter;
			listViewHeight -= height_Titlebar + height_Filter;// + height_Separator;
			//
			showBackground = true;
			/*
			if( isSideBarBottom == true )
			{
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
			listViewPoint.y += height_Titlebar + height_Filter;
			listViewHeight -= height_Titlebar + height_Filter;
			//
			showBackground = true;
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
		//�^�C�g���o�[��\��
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
	
	//�����{�b�N�X�z�u
	if( showTitleBar == true )
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar};
		NVI_SetControlPosition(kControlID_SearchBox,pt);
		NVI_SetControlSize(kControlID_SearchBox,windowBounds.right-windowBounds.left- leftMargin - rightMargin,height_Filter);
		NVI_SetShowControl(kControlID_SearchBox,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_SearchBox,false);
	}
	
	//ListView�z�u�E�\��
	{
		NVI_SetControlPosition(mInfoViewControlID,listViewPoint);
		NVI_SetControlSize(mInfoViewControlID,listViewWidth - vscrollbarWidth,listViewHeight);
		NVI_SetShowControl(mInfoViewControlID,true);
	}
	
	//V�X�N���[���o�[�z�u
	if( vscrollbarWidth > 0 )//#1370 && NVI_IsControlEnable(mVScrollBarControlID) == true )
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
	
	/*
	//�|�b�v�A�b�v�ݒ�{�^���z�u
	{
		AWindowPoint	pt = {0};
		pt.x = windowBounds.right - windowBounds.left - kWidth_PopupSetupButton;
		pt.y = 0;
		NVI_SetControlPosition(kControlID_PopupSetupButton,pt);
		NVI_SetControlSize(kControlID_PopupSetupButton,kWidth_PopupSetupButton,kHeight_PopupButtons);
		NVI_SetShowControl(kControlID_PopupSetupButton,false);
	}
	*/
	
	//�|�b�v�A�b�v�N���[�Y�{�^���z�u
	{
		AWindowPoint	pt = {0};
		pt.x = 0;
		pt.y = 0;
		NVI_SetControlPosition(kControlID_PopupCloseButton,pt);
		NVI_SetControlSize(kControlID_PopupCloseButton,kWidth_PopupCloseButton,kHeight_PopupButtons);
		NVI_SetShowControl(kControlID_PopupCloseButton,false);
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
	
	//�`��f�[�^�X�V
	GetIdInfoView().SPI_UpdateDrawProperty();
}

/*#725
//#238
void	AWindow_IdInfo::SPI_UpdateHistoryButton()
{
	NVI_GetButtonViewByControlID(kHistoryButtonID).NVI_SetText(SPI_GetIdInfoView().SPI_GetCurrentIdText());
}
*/

/*#238
//
void	AWindow_IdInfo::SPI_SetArgIndex( const AIndex inIndex )
{
	if( mFixDisplay == true )   return;
	
	GetIdInfoView().SPI_SetArgIndex(inIndex);
}
*/

/*#725
//�\���Œ�
void	AWindow_IdInfo::SPI_FixDisplay()
{
	//#238 mFixDisplay = !mFixDisplay;
	SPI_GetIdInfoView().SPI_SetFixDisplay(!SPI_GetIdInfoView().SPI_GetFixDisplay());//#238
	//#232 NVI_SetControlBool(kFixDisplayButtonID,mFixDisplay);
	NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetToogleOn(SPI_GetIdInfoView().SPI_GetFixDisplay());
}
*/

/*#238
//���݂̎��ʎq�e�L�X�g���擾
const AText&	AWindow_IdInfo::SPI_GetIdText() const
{
	return GetIdInfoViewConst().SPI_GetIdText();
}
*/

#if 0
//#725

//#291
/**
�I�[�o�[���C���[�h���̓����x�ݒ�
*/
void	AWindow_IdInfo::SPI_UpdateOverlayWindowAlpha()
{
	if( NVI_GetOverlayMode() == true )
	{
		GetIdInfoView().SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		//#725 NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		//#725 NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
	}
}

//#634
/**
�T�C�h�o�[���[�h�̓��ߗ��E�w�i�F�ɐ؂�ւ�
*/
void	AWindow_IdInfo::SPI_SetSideBarMode( const ABool inSideBarMode, const AColor inSideBarColor )
{
	if( NVI_GetOverlayMode() == true )
	{
		if( inSideBarMode == true )
		{
			//�ł�����蕶�������ꂢ�ɕ\�������邽�߂ɕs���ߗ��͂ł��邾�����߂ɐݒ肷��B�i�w�i�̃O���f�[�V�������������瓧�߂�����x�j
			GetIdInfoView().SPI_SetBackgroundColor(inSideBarColor,kColor_Gray97Percent);
			GetIdInfoView().SPI_SetTransparency(0.8);
			NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTransparency(0.8);
			NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTransparency(0.8);
		}
		else
		{
			GetIdInfoView().SPI_SetBackgroundColor(kColor_White,kColor_White);
			GetIdInfoView().SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
			NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTransparency(1);
			NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTransparency(1);
		}
	}
	else
	{
		//�t���[�e�B���O�E�C���h�E���̔w�i�F���ݒ�
		GetIdInfoView().SPI_SetBackgroundColor(kColor_Gray97Percent,kColor_Gray97Percent);
		GetIdInfoView().SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTransparency(1);
		NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTransparency(1);
	}
}
#endif

/**
�|�b�v�A�b�v�E�C���h�E�̃}�E�Xenter/leave������
*/
ABool	AWindow_IdInfo::EVTDO_DoMouseMoved( const AWindowPoint& inWindowPoint, const AModifierKeys inModifierKeys )
{
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Popup )
	{
		//NVI_SetShowControl(kControlID_PopupSetupButton,true);
		NVI_SetShowControl(kControlID_PopupCloseButton,true);
		//
		GetIdInfoView().SPI_SetDisableAutoHide();
	}
	/*
	fprintf(stderr,"moved ");
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Popup )
	{
		GetApp().SPI_SetSubWindowProperty(GetObjectID(),kSubWindowLocationType_Floating,kIndex_Invalid);
		//
		UpdateViewBounds();
		NVI_RefreshWindow();
	}
	*/
	return true;
}


