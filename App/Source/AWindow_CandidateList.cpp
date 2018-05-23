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

AWindow_CandidateList

*/

#include "stdafx.h"

#include "AWindow_CandidateList.h"
#include "AView_CandidateList.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowStatusBar.h"
#include "AView_SubWindowSizeBox.h"

//#725 const AFloatNumber	kFontSize = 9.0;

//#725
//List view, scrollbar
const AControlID	kControlID_ListView = 101;
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kVScrollBarWidth = 11;

//����StatusBar
const AControlID	kControlID_StatusBar = 103;

//�������[�h�I�����j���[�{�^��
const AControlID	kControlID_SelectSearchMode = 104;
const ANumber		kHeight_SelectSearchModeButton = 15;

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

//list view�̍��������]��
const AControlID	kCandidateListHeightMargin = 4;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_CandidateList
#pragma mark ===========================
//���E�C���h�E�̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_CandidateList::AWindow_CandidateList():AWindow(/*#175NULL*/), mCurrentModeIndexForAbbrevKeyText(kIndex_Invalid)
{
	//#138 �f�t�H���g�͈�(10000�`)���g������ݒ�s�v NVM_SetDynamicControlIDRange(kDynamicControlIDStart,kDynamicControlIDEnd);
}
//�f�X�g���N�^
AWindow_CandidateList::~AWindow_CandidateList()
{
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_CandidateList::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + kCandidateListHeightMargin;
}

//#725
/**
SubWindow�p�^�C�g���o�[�擾
*/
AView_SubWindowTitlebar&	AWindow_CandidateList::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//TextView�擾
AView_CandidateList&	AWindow_CandidateList::GetCandidateListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_CandidateList,kViewType_CandidateList,kControlID_ListView);
	/*#199 
	if( NVI_GetViewByControlID(kControlID_ListView).NVI_GetViewType() != kViewType_CandidateList )   _ACThrow("",this);
	return dynamic_cast<AView_CandidateList&>(NVI_GetViewByControlID(kControlID_ListView));
	*/
}
const AView_CandidateList&	AWindow_CandidateList::GetCandidateListViewConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_CandidateList,kViewType_CandidateList,kControlID_ListView);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�R���g���[���̃N���b�N���̃R�[���o�b�N
ABool	AWindow_CandidateList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	return result;
}

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_CandidateList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

/**
�E�C���h�E�̕���{�^��
*/
void	AWindow_CandidateList::EVTDO_DoCloseButton()
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
ABool	AWindow_CandidateList::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//�N���[�Y�{�^��
	  case kMenuItemID_Close:
		{
			GetApp().SPI_CloseSubWindow(GetObjectID());
			result = true;
			break;
		}
		//�������[�h �O����v
	  case kMenuItemID_CandidateSearchMode_Normal:
		{
			GetApp().SPI_SetCandidatePartialSearchMode(false);
			NVI_UpdateProperty();
			break;
		}
		//�������[�h ������v
	  case kMenuItemID_CandidateSearchMode_Partial:
		{
			GetApp().SPI_SetCandidatePartialSearchMode(true);
			NVI_UpdateProperty();
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
void	AWindow_CandidateList::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

//�E�C���h�E����
void	AWindow_CandidateList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//�w�b�_�����t�H���g
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	
	//�E�C���h�E����
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//�|�b�v�A�b�v
	  case kSubWindowLocationType_Popup:
		{
			//�E�C���h�E����
			//���V���h�E��false�B�V���h�E������ƁA�w�i�F�A���t�@�������ꍇ�ɁA�����ɃV���h�E�����ĉ����Ȃ�B
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
			//�T�C�Y�ݒ�
			NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kCandidateWindowWidth));
			NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kCandidateWindowHeight));
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
		text.SetLocalizedText("SubWindowTitle_CandidateList");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwList);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//�����e�L�X�g�p�l������
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowStatusBar>	viewFactory(GetObjectID(),kControlID_StatusBar);
		NVM_CreateView(kControlID_StatusBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_StatusBar,true,false,true,true,false,true);
	}
	
	//�������[�h�ؑփ{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_SelectSearchMode,pt,10,kHeight_SelectSearchModeButton,kControlID_StatusBar);
		NVI_SetControlBindings(kControlID_SelectSearchMode,false,false,true,true,true,true);
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).SPI_SetEllipsisMode(kEllipsisMode_FixedFirstCharacters,10);
		//
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).SPI_SetMenu(kContextMenuID_CandidateSearchMode,kMenuItemID_Invalid);
	}
	
	//SubWindow�p�Z�p���[�^����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_StatusBar,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//SizeBox����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_StatusBar,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
}

//�V�K�^�u����
//�����F��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
void	AWindow_CandidateList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//listview����
	{
		AWindowPoint	pt = {0,0}; 
		AViewDefaultFactory<AView_CandidateList>	candidateListViewFactory(GetObjectID(),kControlID_ListView);//#199
		NVM_CreateView(kControlID_ListView,pt,500,500,kControlID_Invalid,kControlID_Invalid,false,candidateListViewFactory);
		NVI_SetControlBindings(kControlID_ListView,true,true,true,true,false,false);
	}
	
	//V�X�N���[���o�[����
	{
		AWindowPoint	pt = {0,0}; 
		NVI_CreateScrollBar(kControlID_VScrollBar,pt,kVScrollBarWidth,kVScrollBarWidth*2);
		GetCandidateListView().NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
		NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	}
	//
	UpdateViewBounds();
	//windows�p
	NVI_RegisterToFocusGroup(kControlID_ListView);
	NVI_SwitchFocusTo(kControlID_ListView);
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_CandidateList::NVIDO_Hide()
{
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Popup )
	{
		//�E�C���h�E���̕ۑ�
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kCandidateWindowWidth,NVI_GetWindowWidth());
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kCandidateWindowHeight,NVI_GetWindowHeight());
	}
}

//
void	AWindow_CandidateList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//#725 NVI_SetWindowTransparency(/*(static_cast<AFloatNumber>(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIdInfoWindowAlphaPercent)))/100*/
	//#725		0.9);
	
	//�T�u�E�C���h�E�p�w�i�F
	AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(true);
	//�����e�L�X�g�p�l���F���ݒ�
	NVI_GetViewByControlID(kControlID_StatusBar).NVI_SetBackgroundColor(
				GetApp().SPI_GetSubWindowBackgroundColor(true),
				GetApp().SPI_GetSubWindowBackgroundColor(false));
	NVI_GetViewByControlID(kControlID_StatusBar).NVI_SetTextProperty(kColor_Black,kTextStyle_Normal,kJustification_Left);
	//�T�C�Y�{�b�N�X�F�ݒ�
	NVI_GetViewByControlID(kControlID_SizeBox).NVI_SetColor(backgroundColor);
	//list view�Ɖ����e�L�X�g�p�l���Ƀ��l�ݒ�
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		GetCandidateListView().SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
		NVI_GetViewByControlID(kControlID_StatusBar).NVI_SetBackgroundTransparency(GetApp().SPI_GetFloatingWindowAlpha());
	}
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Popup )
	{
		GetCandidateListView().SPI_SetTransparency(GetApp().SPI_GetPopupWindowAlpha());
		NVI_GetViewByControlID(kControlID_StatusBar).NVI_SetBackgroundTransparency(GetApp().SPI_GetPopupWindowAlpha());
	}
	//�����e�L�X�g�p�l���̃e�L�X�g�ݒ�
	SPI_UpdateStatusBar();
	//�������[�h���j���[�{�^���̃e�L�X�g�ݒ�
	{
		AText	text;
		if( GetApp().SPI_IsCandidatePartialSearchMode() == false )
		{
			text.SetLocalizedText("CandidateList_SearchMode_Normal");
		}
		else
		{
			text.SetLocalizedText("CandidateList_SearchMode_Partial");
		}
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).NVI_SetText(text);
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).SPI_SetWidthToFitTextWidth();
		
		//�T�u�E�C���h�E�p�t�H���g�擾
		AText	fontname;
		AFloatNumber	fontsize = 9.0;
		GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
		//�`��F�ݒ�
		AColor	letterColor = kColor_Black;
		AColor	dropShadowColor = kColor_White;
		AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
		GetApp().SPI_GetSubWindowBoxColor(GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).SPI_SetTextProperty(fontname,9.0,kJustification_Left,
																					  kTextStyle_Normal,letterColor,letterColor);
	}
	//view bounds�X�V
	UpdateViewBounds();
	//�`��X�V
	NVI_RefreshWindow();
}

/**
�����X�e�[�^�X�o�[�̕\���X�V
*/
void	AWindow_CandidateList::SPI_UpdateStatusBar()
{
	//��␔
	AText	text;
	text.SetLocalizedText("CandidateList_Count");
	text.ReplaceParamText('0',GetCandidateListView().SPI_GetCandidateCount());
	if( GetCandidateListView().SPI_GetCandidateCount() >= kLimit_CandidateListItemCount )
	{
		AText	t;
		t.SetLocalizedText("CandidateList_MaxItems");
		text.AddText(t);
	}
	text.AddCstring("   ");
	//�⊮�L�[
	if( GetCandidateListView().SPI_GetIndex() == kIndex_Invalid )
	{
		//------------------�⊮���̓L�[���͑O------------------
		AText	t;
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAbbrevInputByArrowKeyAfterPopup) == true )
		{
			//------------------�㉺�L�[�ŕ⊮���͉\�̏ꍇ------------------
			//�u^1�L�[�܂��́����L�[�ŕ⊮���́v
			t.SetLocalizedText("CandidateList_Key_BeforeAbbrevMode_ArrowKeyAvailable");
		}
		else
		{
			//------------------�⊮�L�[�݂̂ŕ⊮���͉\�̏ꍇ------------------
			//�u^1�L�[�ŕ⊮���́v
			t.SetLocalizedText("CandidateList_Key_BeforeAbbrevMode");
		}
		t.ReplaceParamText('1',mAbbrevKeyText);
		text.AddText(t);
	}
	else
	{
		//------------------�⊮���̓L�[���͌�------------------
		//�u"�����L�[�Ō��I���Areturn �L�[�Ŋm��Aesc �L�[�ŃL�����Z���v
		AText	t;
		t.SetLocalizedText("CandidateList_Key_AfterAbbrevMode");
		text.AddText(t);
	}
	NVI_GetViewByControlID(kControlID_StatusBar).NVI_SetText(text);
}

//
void	AWindow_CandidateList::SPI_SetInfo( const AModeIndex inModeIndex,
		const AWindowID inTextWindowID, const AControlID inTextViewControlID, 
		//#717 const AGlobalPoint inPoint, const ANumber inLineHeight, 
		const ATextArray& inIdTextArray, const ATextArray& inInfoTextArray,
		const AArray<AIndex>& inCategoryIndexArray, const AArray<AScopeType>& inScopeTypeArray, //#717 const AObjectArray<AFileAcc>& inFileArray )
		const ATextArray& inFilePathArray, const AArray<ACandidatePriority>& inPriorityArray,
		const ATextArray& inParentKeywordArray,
		const AArray<AItemCount>& inMatchedCountArray )
{
	/*#717 �E�C���h�E�ʒu�́A�R�[�����̂ق��Őݒ肷��
	ANumber	h = NVI_GetWindowHeight();
	AGlobalRect	screenbounds;
	AWindow::NVI_GetMainDeviceScreenBounds(screenbounds);
	APoint	pt;
	pt.x = inPoint.x;
	pt.y = inPoint.y + inLineHeight + 32;//�^�C�g���o�[����������
	if( pt.y + h > screenbounds.bottom )
	{
		pt.y = inPoint.y - 16- h;
	}
	NVI_SetWindowPosition(pt);
	*/
	GetCandidateListView().SPI_SetInfo(inModeIndex,inTextWindowID,inTextViewControlID,
				inIdTextArray,inInfoTextArray,inCategoryIndexArray,inScopeTypeArray,inFilePathArray,inPriorityArray,
				inParentKeywordArray,
				inMatchedCountArray);//#717 inFileArray);
	//
	NVI_UpdateProperty();
	/*#1160
	//���L�[�̃L�[�o�C���h�e�L�X�g�擾
	switch(GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kAbbrevNextKey))
	{
	  case kAbbrevKeyType_ShiftSpace:
		{
			mAbbrevKeyText.SetLocalizedText("AbbrevKeyType1");
			break;
		}
	  case kAbbrevKeyType_ShiftTab:
		{
			mAbbrevKeyText.SetLocalizedText("AbbrevKeyType2");
			break;
		}
	  case kAbbrevKeyType_Tab:
		{
			mAbbrevKeyText.SetLocalizedText("AbbrevKeyType3");
			break;
		}
	  case kAbbrevKeyType_Escape:
		{
			mAbbrevKeyText.SetLocalizedText("AbbrevKeyType4");
			break;
		}
	  case kAbbrevKeyType_ShiftEscape:
		{
			mAbbrevKeyText.SetLocalizedText("AbbrevKeyType5");
			break;
		}
	  case kAbbrevKeyType_KeyBind:
		{
			//���[�h���ϊ������ꍇ�̂݁A�⊮�L�[�擾�iGetKeyTextFromKeybindAction()�͂����������Ԃ�������j
			if( mCurrentModeIndexForAbbrevKeyText != inModeIndex && inModeIndex != kIndex_Invalid )
			{
				GetApp().SPI_GetModePrefDB(inModeIndex).GetKeyTextFromKeybindAction(keyAction_abbrevnext,mAbbrevKeyText);
				//�L�[�o�C���h�����������Ƃ���?�ƕ\���i�������𖾊m�ɂ��邽�߁j
				if( mAbbrevKeyText.GetItemCount() == 0 )
				{
					mAbbrevKeyText.SetCstring("?");
				}
			}
			break;
		}
		mCurrentModeIndexForAbbrevKeyText = inModeIndex;
	}
	*/
}

//
void	AWindow_CandidateList::SPI_SetIndex( const AIndex inIndex )
{
	GetCandidateListView().SPI_SetIndex(inIndex);
}

/*:
�E�C���h�E�̍ő卂���擾�i�|�b�v�A�b�v���̃E�C���h�E���������̂��߁j
*/
ANumber	AWindow_CandidateList::SPI_GetMaxWindowHeight() const
{
	return NVI_GetViewConstByControlID(kControlID_ListView).NVI_GetImageHeight() + kHeight_CandidateListStatusBar + kCandidateListHeightMargin;
}

//
void	AWindow_CandidateList::UpdateViewBounds()
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
			topMargin = 1;
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
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin -topMargin - kHeight_CandidateListStatusBar;
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
			listViewPoint.y += 1;//#1079 3;
			listViewPoint.x += 1;
			listViewWidth -= 1;//#1079 2;
			listViewHeight -= 1;//#1079 2;
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
		NVI_SetControlPosition(kControlID_ListView,listViewPoint);
		NVI_SetControlSize(kControlID_ListView,listViewWidth - vscrollbarWidth,listViewHeight-3);
		NVI_SetShowControl(kControlID_ListView,true);
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
	
	//�e�L�X�g�p�l���z�u�E�\��
	{
		NVI_SetControlPosition(kControlID_StatusBar,footerPoint);
		NVI_SetControlSize(kControlID_StatusBar,footerWidth,kHeight_CandidateListStatusBar);
		NVI_SetShowControl(kControlID_StatusBar,true);
	}
	
	//�������[�h�{�^��
	{
		AWindowPoint	pt = {0};
		pt.x = windowBounds.right-windowBounds.left - NVI_GetControlWidth(kControlID_SelectSearchMode) - kWidth_SizeBox - 3;
		pt.y = windowBounds.bottom-windowBounds.top - kHeight_SelectSearchModeButton - 1;
		NVI_SetControlPosition(kControlID_SelectSearchMode,pt);
		NVI_SetShowControl(kControlID_SelectSearchMode,true);
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
	GetCandidateListView().SPI_UpdateDrawProperty();
}

