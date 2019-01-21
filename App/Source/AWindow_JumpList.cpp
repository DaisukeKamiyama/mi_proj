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

AWindow_JumpList

#291 �^�u����ListView,vScrollBar�쐬����悤�ɕύX

*/

#include "stdafx.h"

#include "AWindow_JumpList.h"
#include "AApp.h"
#include "AView_Text.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowSizeBox.h"

/*#725
#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kButtonFontSize = 9.0;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AFloatNumber	kButtonFontSize = 8.0;
#endif
*/

//#798
//�t�B���^
const AControlID	kControlID_Filter				= 201;
const ANumber		kHeight_FilterMargin			= 8;

//#725
//SubWindow�p�^�C�g���o�[view
const AControlID	kControlID_TitleBar = 901;

//#725
//�T�C�Y�{�b�N�X
const AControlID	kControlID_SizeBox = 903;
const ANumber	kWidth_SizeBox = 15;
const ANumber	kHeight_SizeBox = 15;

//#725
//�w�i�F�`��pview
const AControlID	kControlID_Background = 905;

//#725
//SubWindow�p�Z�p���[�^view
//const AControlID	kControlID_Separator = 902;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_JumpList
#pragma mark ===========================
//�W�����v���X�g�E�C���h�E�̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_JumpList::AWindow_JumpList( const AWindowID inTextWindowID )
:AWindow(), mTextWindowID(inTextWindowID)
{
	//#725 �^�C�}�[�Ώۂɂ���
	NVI_AddToTimerActionWindowArray();
}
//�f�X�g���N�^
AWindow_JumpList::~AWindow_JumpList()
{
}

//TextWindow�擾
AWindow_Text&	AWindow_JumpList::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_JumpList::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 20 + kHeight_FilterMargin + 10;
}

//#725
/**
SubWindow�p�^�C�g���o�[�擾
*/
AView_SubWindowTitlebar&	AWindow_JumpList::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
���j���[�I��������
*/
ABool	AWindow_JumpList::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			//�t���[�e�B���O�̏ꍇ�A�e�L�X�g�E�C���h�E��show/hide����
			if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
			{
				GetTextWindow().SPI_ShowHideJumpListWindow(false);
			}
			//�t���[�e�B���O�ȊO�̏ꍇ�́A���������B�i�N���[�Y�{�^���Ȃ��j
			result = true;
			break;
		}
	  case kMenuItemID_JumpList_CollapseAllForLevel:
		{
			//�R���e�L�X�g���j���[�N���b�N�������X�g���ڂ�index���擾
			AIndex	contextMenuDBIndex = GetListView(NVI_GetCurrentTabIndex()).SPI_GetContextMenuSelectedRowDBIndex();
			if( contextMenuDBIndex != kIndex_Invalid )
			{
				AIndex	displayRowIndex = GetListView(NVI_GetCurrentTabIndex()).SPI_GetDisplayRowIndexFromDBIndex(contextMenuDBIndex);
				if( displayRowIndex != kIndex_Invalid )
				{
					//�������x���̍��ڂ�S�Đ܂肽���݁E�W�J
					GetListView(NVI_GetCurrentTabIndex()).SPI_ExpandCollapseAllForSameLevel(displayRowIndex);
				}
			}
			break;
		}
	  case kMenuItemID_JumpList_ExpandAll:
		{
			GetListView(NVI_GetCurrentTabIndex()).SPI_ExpandAll();
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
void	AWindow_JumpList::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
�R���g���[���̃N���b�N���̃R�[���o�b�N
*/
ABool	AWindow_JumpList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	//ListView�N���b�N
	AIndex	listViewTabIndex = mListViewControlIDArray.Find(inID);
	if( listViewTabIndex != kIndex_Invalid )
	{
		AIndex	index = GetListView(listViewTabIndex).SPI_GetSelectedDBIndex();
		if( index != kIndex_Invalid )
		{
			GetTextWindow().SPI_DoJumpListSelected(NVI_GetDocumentIDByTabIndex(listViewTabIndex),index,inModifierKeys);
			result = true;
		}
		return result;
	}
	//�\�[�g�{�^���N���b�N
	AIndex	sortButtonTabIndex = mSortButtonControlIDArray.Find(inID);
	if( sortButtonTabIndex != kIndex_Invalid )
	{
		GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).
				SPI_SetJumpListSortMode(mListViewIDArray.Get(sortButtonTabIndex),
				NVI_GetButtonViewByControlID(mSortButtonControlIDArray.Get(sortButtonTabIndex)).SPI_GetToggleOn());
		result = true;
		return result;
	}
	return result;
}

//TableView�̃R�������̕ύX��Sort���̕ύX�Ȃǂ̏�ԕύX���̃R�[���o�b�N
void	AWindow_JumpList::EVTDO_TableViewStateChanged( const AControlID inID )
{
	if( NVI_IsWindowCreated() == false )   return;
}


//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_JumpList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//ViewBounds�X�V
	UpdateViewBounds();
	
	//�E�C���h�E���Έʒu�E�T�C�Y��DB�ɕۑ�
	SaveWindowPosition();
}


//�E�C���h�E�̕���{�^��
void	AWindow_JumpList::EVTDO_DoCloseButton()
{
	//�t���[�e�B���O�̏ꍇ�A�e�L�X�g�E�C���h�E��show/hide����
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		GetTextWindow().SPI_ShowHideJumpListWindow(false);
	}
	//�t���[�e�B���O�ȊO�̏ꍇ�́A���������B�i�N���[�Y�{�^���Ȃ��j
}

//#798
/**
�R���g���[����text���͎�����
*/
void	AWindow_JumpList::EVTDO_TextInputted( const AControlID inID )
{
	//�t�B���^�œ��͎�
	if( inID == kControlID_Filter )
	{
		//�t�B���^�e�L�X�g�擾
		AText	filter;
		NVI_GetControlText(kControlID_Filter,filter);
		filter.ChangeCaseLowerFast();
		
		//list view�Ƀt�B���^�ݒ�
		GetListView(NVI_GetCurrentTabIndex()).SPI_SetFilterText(0,filter);
		
		//�I�����X�V
		if( filter.GetItemCount() == 0 )
		{
			GetTextWindow().SPI_UpdateJumpListSelection(true);
		}
	}
}

//#798
/**
�R���g���[���Ń��^�[���L�[����������
*/
ABool	AWindow_JumpList::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )
{
	//�t�B���^�œ��͎�
	if( inControlID == kControlID_Filter )
	{
		//list view�ł̃N���b�N���Ɠ������������s
		EVTDO_Clicked(mListViewControlIDArray.Get(NVI_GetCurrentTabIndex()),kModifierKeysMask_None);
		
		//�t�B���^�e�L�X�g����ɂ���
		NVI_SetControlText(kControlID_Filter,GetEmptyText());
		
		//list view�Ƀt�B���^�ݒ�
		GetListView(NVI_GetCurrentTabIndex()).SPI_SetFilterText(0,GetEmptyText());
		
		//�I�����X�V
		GetTextWindow().SPI_UpdateJumpListSelection(true);
		
	}
	return true;
}

//#798
/**
ESC�L�[����������
*/
ABool	AWindow_JumpList::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )
{
	//�t�B���^�œ��͎�
	if( inControlID == kControlID_Filter )
	{
		//�t�B���^�e�L�X�g����ɂ���
		NVI_SetControlText(kControlID_Filter,GetEmptyText());
		
		//list view�Ƀt�B���^�ݒ�
		GetListView(NVI_GetCurrentTabIndex()).SPI_SetFilterText(0,GetEmptyText());
		
		//�I�����X�V
		GetTextWindow().SPI_UpdateJumpListSelection(true);
		
		//�e�L�X�g�E�C���h�E�փt�H�[�J�X�ړ�
		GetTextWindow().NVI_SelectWindow();
	}
	return true;
}

//#798
/**
�R���g���[���ŏ�L�[����������
*/
ABool	AWindow_JumpList::NVIDO_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�t�B���^�œ��͎�
	if( inControlID == kControlID_Filter )
	{
		//list view�őO��I��
		GetListView(NVI_GetCurrentTabIndex()).SPI_SetSelectPreviousDisplayRow(inModifers);
	}
	return true;
}

//#798
/**
�R���g���[���ŉ��L�[����������
*/
ABool	AWindow_JumpList::NVIDO_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//�t�B���^�œ��͎�
	if( inControlID == kControlID_Filter )
	{
		//list view�Ŏ���I��
		GetListView(NVI_GetCurrentTabIndex()).SPI_SetSelectNextDisplayRow(inModifers);
	}
	return true;
}

//#798
/**
�^�u�L�[����������
*/
ABool	AWindow_JumpList::NVIDO_ViewTabKeyPressed( const AControlID inControlID )
{
	//�^�u�L�[�ɂ��t�H�[�J�X�ړ�
	return true;
}

//#892
/**
���X�g�܂肽���݁^�W�J������
*/
void	AWindow_JumpList::NVIDO_ListViewExpandedCollapsed( const AControlID inControlID )
{
	GetTextWindow().NVI_SelectWindow();
}

/**
���X�g�̃z�o�[�X�V������
*/
void	AWindow_JumpList::NVIDO_ListViewHoverUpdated( const AControlID inControlID, 
													 const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex )
{
	GetTextWindow().SPI_DoListViewHoverUpdated(inHoverStartDBIndex,inHoverEndDBIndex);
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_JumpList::NVIDO_Create( const ADocumentID inDocumentID )
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
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,false,false,true,false);
			break;
		}
	}
	
	//�E�C���h�E�T�C�Y�̐ݒ�
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListSizeX));
		NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListSizeY));
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
		text.SetLocalizedText("SubWindowTitle_JumpList");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwList);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontName;
	AFloatNumber	fontSize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontName,fontSize);
	
	//�����{�b�N�X�𐶐� #798
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_Filter,pt,10,10,kControlID_Background,kIndex_Invalid,true,false,false,true,kEditBoxType_FilterBox);
		NVI_GetEditBoxView(kControlID_Filter).NVI_SetTextFont(fontName,fontSize);
		/*#1316
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetBackgroundColor(
					kColor_White,kColor_Gray70Percent,kColor_Gray70Percent);
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetTextColor(
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"),
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"));
					*/
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetEnableFocusRing(false);
		NVI_SetAutomaticSelectBySwitchFocus(kControlID_Filter,true);
		NVI_SetControlBindings(kControlID_Filter,true,true,true,false,false,true);
	}
	
	//SizeBox����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
	
	//ViewBounds�ݒ�
	UpdateViewBounds();
}

/**
�V�K�^�u����
@return outInitialFocusControlID ��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_JumpList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {-10000,0};//win �`��͈͂ɐV�K�R���g���[���𐶐�����ƁA�������Ă��������_�ŉ�ʑS�̃N���A���������邽��0��-10000
	
	//�T�u�E�C���h�E�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
	//ListView����
	AControlID	listViewControlID = NVM_AssignDynamicControlID();
	AListViewFactory	listViewFactory(GetObjectID(),listViewControlID);
	AViewID	listViewViewID = NVM_CreateView(listViewControlID,pt,10,10,kControlID_Background,kControlID_Invalid,false,listViewFactory);
	mListViewControlIDArray.Insert1(inTabIndex,listViewControlID);
	NVM_RegisterViewInTab(inTabIndex,listViewControlID);
	mListViewIDArray.Insert1(inTabIndex,listViewViewID);
	/*#725
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetListView(listViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetListView(listViewControlID).SPI_SetAlwaysActiveColors(true);
	}
	*/
	NVI_GetListView(listViewControlID).SPI_SetAlwaysActiveColors(true);
	NVI_GetListView(listViewControlID).SPI_SetBackgroundColor(
				GetApp().SPI_GetSubWindowBackgroundColor(true),
				GetApp().SPI_GetSubWindowBackgroundColor(false));
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(listViewControlID,true,true,true,true,false,false);
	//#328 HelpTag
	NVI_EnableHelpTagCallback(listViewControlID);
	//#442
	NVI_GetListView(listViewControlID).SPI_SetEnableContextMenu(true,kContextMenuID_JumpList);//#442
	NVI_GetListView(listViewControlID).SPI_SetTextDrawProperty(fontname,fontsize,GetApp().SPI_GetSubWindowLetterColor());//#725
	//#725
	//NVI_EmbedControl(kControlID_PaneFrame,listViewControlID);
	
	//V�X�N���[���o�[����
	AControlID	vScrollBarControlID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(vScrollBarControlID,pt,kVScrollBarWidth,kVScrollBarWidth*2);
	mVScrollBarControlIDArray.Insert1(inTabIndex,vScrollBarControlID);
	NVM_RegisterViewInTab(inTabIndex,vScrollBarControlID);
	//�X�N���[���o�[�֘A�Â�
	NVI_GetViewByControlID(listViewControlID).NVI_SetScrollBarControlID(kControlID_Invalid,vScrollBarControlID);
	//#291 �T�C�Ybinding�ݒ�i��؂���ύX���̂�����h�~�j
	NVI_SetControlBindings(vScrollBarControlID,false,true,true,true,true,false);
	
	//SubWindow�p�Z�p���[�^����
	{
		AControlID	separatorControlID = NVM_AssignDynamicControlID();
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),separatorControlID);
		NVM_CreateView(separatorControlID,pt,10,10,listViewControlID,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(separatorControlID,true);
		NVI_SetControlBindings(separatorControlID,true,false,true,true,false,true);
		mSeparatorControlIDArray.Insert1(inTabIndex,separatorControlID);
	}
	
	//
	NVI_UpdateProperty();
	
	//ViewBounds�ݒ�
	UpdateViewBounds();
	
	//Document��ListView��o�^
	GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_RegisterJumpListView(listViewViewID);
	
	//#661
	/*
	{
		AText	text;
		text.SetLocalizedText("HelpTag_JumpList");
		NVI_SetHelpTag(jumpListButtonControlID,text);
	}
	 */
	//
	outInitialFocusControlID = listViewControlID;
}

/**
�^�u�폜���ɃR�[�������

Tab��View��AWindow���ō폜�����
*/
void	AWindow_JumpList::NVIDO_DeleteTab( const AIndex inTabIndex )
{
	//Document����ListView��o�^�폜
	GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_UnregisterJumpListView(mListViewIDArray.Get(inTabIndex));
	//�f�[�^�폜
	mListViewControlIDArray.Delete1(inTabIndex);
	mListViewIDArray.Delete1(inTabIndex);
	mVScrollBarControlIDArray.Delete1(inTabIndex);
	mSeparatorControlIDArray.Delete1(inTabIndex);//#725
	/*
	mSortButtonControlIDArray.Delete1(inTabIndex);
	mJumpListButtonControlIDArray.Delete1(inTabIndex);//#446
	*/
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_JumpList::NVIDO_Hide()
{
	SaveWindowPosition();
}

//#798
/**
*/
void	AWindow_JumpList::NVIDO_Show()
{
}

/**
�ݒ�f�[�^�X�V������
*/
void	AWindow_JumpList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	
	/*#725
	//�����x�ݒ�
	if( NVM_GetOverlayMode() == false )//#291
	{
		NVI_SetWindowTransparency((static_cast<AFloatNumber>(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListAlphaPercent)))/100);
	}
	else
	{
		//Overlay���͏��y�C��Overlay�����x��K�p�i�������AMacOSX�ł͎��ۂɂ͉������Ȃ��BView���œh��Ԃ����ɓ����x�K�p�B�j
		NVI_SetWindowTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
	}
	*/
	
	//�T�u�E�C���h�E�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//�t�B���^�Ƀt�H���g�ݒ�
	NVI_GetEditBoxView(kControlID_Filter).NVI_SetTextFont(fontname,fontsize);
	//
	AText	filtertext;
	filtertext.SetLocalizedText("Filter");
	//
	AText	shortcuttext;
	GetApp().NVI_GetMenuManager().GetMenuItemShortcutDisplayText(kMenuItemID_SearchInHeaderList,shortcuttext);
	//
	NVI_GetEditBoxView(kControlID_Filter).SPI_SetTextForEmptyState(filtertext,shortcuttext);
	//���X�gview�Ƀt�H���g�ݒ�
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		GetListView(tabIndex).SPI_SetTextDrawProperty(fontname,fontsize,GetApp().SPI_GetSubWindowLetterColor());
		//�z�C�[���X�N���[�����ݒ�
		GetListView(tabIndex).SPI_SetWheelScrollPercent(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
					GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));
		//�t���[�e�B���O�̏ꍇ�̔w�i���ߗ��ݒ�
		if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
		{
			GetListView(tabIndex).SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
			NVI_GetEditBoxView(kControlID_Filter).SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
		}
		else
		{
			GetListView(tabIndex).SPI_SetTransparency(1.0);
			NVI_GetEditBoxView(kControlID_Filter).SPI_SetTransparency(1.0);
		}
	}
	/*#1316
	//�t�B���^�̐F�ݒ�
	NVI_GetEditBoxView(kControlID_Filter).SPI_SetBackgroundColorForEmptyState(
																			  GetApp().SPI_GetSubWindowBackgroundColor(true),
																			  GetApp().SPI_GetSubWindowBackgroundColor(false));
																			  */
	//view bounds�X�V
	UpdateViewBounds();
	//�E�C���h�E�ʒu�X�V
	SPI_UpdatePosition();
}

/*#291
void	AWindow_JumpList::SPI_SetScrollPosition( const APoint& inPoint )
{
	if( NVI_IsWindowCreated() == false )   return;
	AImagePoint	pt;
	pt.x = inPoint.x;
	pt.y = inPoint.y;
	GetListView().NVI_ScrollTo(pt);
}

void	AWindow_JumpList::SPI_GetScrollPosition( APoint& outPoint ) const
{
	if( NVI_IsWindowCreated() == false )   return;
	AImagePoint	pt;
	GetListViewConst().NVI_GetOriginOfLocalFrame(pt);
	outPoint.x = pt.x;
	outPoint.y = pt.y;
}
*/

//#798
/**
�����{�b�N�X���t�H�[�J�X
*/
void	AWindow_JumpList::SPI_SwitchFocusToSearchBox()
{
	NVI_SelectWindow();
	NVI_SwitchFocusTo(kControlID_Filter);
	NVI_GetEditBoxView(kControlID_Filter).NVI_SetSelectAll();
}

/**
�E�C���h�E�^�C�g���ݒ�
*/
void	AWindow_JumpList::SPI_SetSubWindowTitle( const AText& inTitle )
{
	GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(inTitle,kImageID_iconSwList);
}

#pragma mark ===========================

#pragma mark ---�\���X�V

/**
�E�C���h�E�ʒu�X�V
*/
void	AWindow_JumpList::SPI_UpdatePosition()
{
	if( NVI_IsWindowCreated() == false )   return;
	
	if( NVM_GetOverlayMode() == true )   return;//#291
	
	//
	ARect	textDocumentBounds = {0};
	GetTextWindow().NVI_GetWindowBounds(textDocumentBounds);
	
	//
	APoint	pt = {0};
	NVI_GetWindowPosition(pt);
	if( GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListPosition) == /*AAppPrefDB::kJumpListPosition_Left*/0 )
	{
		pt.x = textDocumentBounds.left + GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListOffsetX);
	}
	else
	{
		pt.x = textDocumentBounds.right + GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListOffsetX);
	}
	pt.y = textDocumentBounds.top + GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListOffsetY);
	NVI_SetWindowPosition(pt);
	//#680 ���ꂪ����ƃe�L�X�g�E�C���h�E�ړ����Ɍ��o���E�C���h�E�̈ʒu������Ă��܂� NVI_ConstrainWindowPosition(true);
}

/**
�E�C���h�E���Έʒu�E�T�C�Y��DB�ɕۑ�
*/
void	AWindow_JumpList::SaveWindowPosition()
{
	//�ʒu�E�T�C�Y�ۑ�
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		//
		ARect	textDocumentBounds = {0};
		GetTextWindow().NVI_GetWindowBounds(textDocumentBounds);
		
		//
		APoint	pt = {0};
		NVI_GetWindowPosition(pt);
		if( pt.x < (textDocumentBounds.left+textDocumentBounds.right)/2 )
		{
			//�e�L�X�g�E�C���h�E�̍��㑊�Έʒu��ۑ�
			GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListPosition,/*AAppPrefDB::kJumpListPosition_Left*/0);
			GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListOffsetX,pt.x-textDocumentBounds.left);
		}
		else
		{
			//�e�L�X�g�E�C���h�E�̉E�㑊�Έʒu��ۑ�
			GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListPosition,/*AAppPrefDB::kJumpListPosition_Right*/1);
			GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListOffsetX,pt.x-textDocumentBounds.right);
		}
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListOffsetY,pt.y-textDocumentBounds.top);
		//�E�C���h�E�T�C�Y�̕ۑ�
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListSizeX,NVI_GetWindowWidth());
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListSizeY,NVI_GetWindowHeight());
	}
}

/**
ViewBounds�X�V
*/
void	AWindow_JumpList::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//�^�C���o�[�A�Z�p���[�^�����擾
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	ANumber	height_Filter = NVI_GetEditBoxView(kControlID_Filter).SPI_GetLineHeightWithMargin() + kHeight_FilterMargin;
	ANumber	height_DropBox = 0;
	
	//
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
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;
	//�X�N���[���o�[��
	ANumber	vscrollbarWidth = kVScrollBarWidth;
	//
	ANumber	sizeboxHeight = 0;
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
			listViewPoint.y += height_Titlebar + height_Filter;
			listViewHeight -= height_Titlebar + height_Filter;// + height_Separator;
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
			height_DropBox = kDragBoxHeight;
			//
			showSizeBox = true;
			sizeboxHeight = kHeight_SizeBox;
			break;
		}
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
	{
		//�w�i�`��view�z�u
		NVI_SetControlPosition(kControlID_Background,backgroundPt);
		NVI_SetControlSize(kControlID_Background,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top - backgroundPt.y);
		//�w�i�`��view�\��
		NVI_SetShowControl(kControlID_Background,true);
	}
	
	//�t�B���^�z�u
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar};
		NVI_SetControlPosition(kControlID_Filter,pt);
		NVI_SetControlSize(kControlID_Filter,windowBounds.right-windowBounds.left- leftMargin - rightMargin,height_Filter);
	}
	
	
	//�^�u����
	for( AIndex tabIndex = 0; tabIndex < mListViewControlIDArray.GetItemCount(); tabIndex++ )
	{
		AControlID	vscrollControlID = mVScrollBarControlIDArray.Get(tabIndex);
		//ListView bounds�ݒ�
		{
			AControlID	controlID = mListViewControlIDArray.Get(tabIndex);
			NVI_SetControlPosition(controlID,listViewPoint);
			//V�X�N���[���o�[�\���L���ɍ��킹��list view�̃T�C�Y��ς���i�I��`�悪�E�[�܂ŕ\������Ȃ��Ƃ�������邢���߁j
			ANumber	w = listViewWidth;
			if( vscrollbarWidth > 0 && NVI_IsControlEnable(vscrollControlID) == true )
			{
				w -= vscrollbarWidth;
			}
			NVI_SetControlSize(controlID,w,listViewHeight);
		}
		//V�X�N���[���o�[�z�u
		if( vscrollbarWidth > 0 && NVI_IsControlEnable(vscrollControlID) == true && tabIndex == NVI_GetCurrentTabIndex() )
		{
			AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
			NVI_SetControlPosition(vscrollControlID,pt);
			NVI_SetControlSize(vscrollControlID,vscrollbarWidth,listViewHeight - sizeboxHeight);
			NVI_SetShowControl(vscrollControlID,true);
		}
		else
		{
			NVI_SetShowControl(vscrollControlID,false);
		}
		//�Z�p���[�^�z�u
		AControlID	separatorID = mSeparatorControlIDArray.Get(tabIndex);
		if( showSeparator == true && tabIndex == NVI_GetCurrentTabIndex() )
		{
			//�Z�p���[�^�z�u
			AWindowPoint	pt = {0,windowBounds.bottom-windowBounds.top-height_Separator};
			NVI_SetControlPosition(separatorID,pt);
			NVI_SetControlSize(separatorID,windowBounds.right-windowBounds.left,height_Separator);
			//�Z�p���[�^�\��
			NVI_SetShowControl(separatorID,true);
		}
		else
		{
			//�Z�p���[�^��\��
			NVI_SetShowControl(separatorID,false);
		}
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

#if 0
//#725
//#291
/**
�I�[�o�[���C���[�h���̓����x�ݒ�
*/
void	AWindow_JumpList::SPI_UpdateOverlayWindowAlpha()
{
	if( NVI_GetOverlayMode() == true )
	{
		for( AIndex i = 0; i < mListViewControlIDArray.GetItemCount(); i++ )
		{
			AControlID listViewControlID = mListViewControlIDArray.Get(i);
			NVI_GetListView(listViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
			AControlID	sortButtonControlID = mSortButtonControlIDArray.Get(i);
			NVI_GetButtonViewByControlID(sortButtonControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
			//#446
			AControlID	jumpListButtonControlID = mJumpListButtonControlIDArray.Get(i);
			NVI_GetButtonViewByControlID(jumpListButtonControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		}
	}
}

//#634
/**
�T�C�h�o�[���[�h�̓��ߗ��E�w�i�F�ɐ؂�ւ�
*/
void	AWindow_JumpList::SPI_SetSideBarMode( const ABool inSideBarMode, const AColor inSideBarColor )
{
	if( NVI_GetOverlayMode() == true )
	{
		for( AIndex i = 0; i < mListViewControlIDArray.GetItemCount(); i++ )
		{
			AControlID listViewControlID = mListViewControlIDArray.Get(i);
			AControlID	sortButtonControlID = mSortButtonControlIDArray.Get(i);
			AControlID	jumpListButtonControlID = mJumpListButtonControlIDArray.Get(i);
			if( inSideBarMode == true )
			{
				//�ł�����蕶�������ꂢ�ɕ\�������邽�߂ɕs���ߗ��͂ł��邾�����߂ɐݒ肷��B�i�w�i�̃O���f�[�V�������������瓧�߂�����x�j
				NVI_GetListView(listViewControlID).SPI_SetBackgroundColor(inSideBarColor,kColor_Gray97Percent);
				NVI_GetListView(listViewControlID).SPI_SetTransparency(0.8);
				NVI_GetButtonViewByControlID(sortButtonControlID).SPI_SetTransparency(0.8);
				NVI_GetButtonViewByControlID(jumpListButtonControlID).SPI_SetTransparency(0.8);
			}
			else
			{
				NVI_GetListView(listViewControlID).SPI_SetBackgroundColor(kColor_White,kColor_White);
				NVI_GetListView(listViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
				NVI_GetButtonViewByControlID(sortButtonControlID).SPI_SetTransparency(1);
				NVI_GetButtonViewByControlID(jumpListButtonControlID).SPI_SetTransparency(1);
			}
		}
	}
}
#endif

//#695
/**
�\�[�g�{�^���̕\����Ԃ��X�V����
*/
void	AWindow_JumpList::SPI_UpdateSortButtonStatus( const ADocumentID inDocumentID, const ABool inSortOn )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	NVI_GetButtonViewByControlID(mSortButtonControlIDArray.Get(tabIndex)).SPI_SetToogleOn(inSortOn);
}

#pragma mark ===========================

#pragma mark --- ListView�擾

/**
ListView�擾
@param inTabIndex TabIndex
*/
AView_List&	AWindow_JumpList::GetListView( const AIndex inTabIndex )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,mListViewControlIDArray.Get(inTabIndex));
}
/**
ListView�擾
@param inTabIndex TabIndex
*/
const AView_List&	AWindow_JumpList::GetListViewConst( const AIndex inTabIndex ) const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,mListViewControlIDArray.Get(inTabIndex));
}

