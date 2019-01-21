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

AWindow_TextMarker
#750

*/

#include "stdafx.h"

#include "AWindow_TextMarker.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowHeader.h"
#include "AView_SubWindowSizeBox.h"

//�e�L�X�g�{�b�N�X
const AControlID	kControlID_Text = 101;

//�X�N���[���o�[
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kWidth_VScroll = 11;

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

//�w�b�_view
const AControlID	kControlID_Header = 906;
const ANumber	kHeight_Header = 40;
const ANumber	kHeight_Header1Line = 20;

//�w�b�_���e��R���g���[��
const AControlID	kControlID_SelectGroupButton = 105;
const AControlID	kControlID_AddGroupButton = 106;
const AControlID	kControlID_DeleteGroupButton = 107;
const AControlID	kControlID_ChangeGroupNameButton = 108;
const AControlID	kControlID_DeleteGroupButtonOK = 109;
const AControlID	kControlID_NameEditBox = 110;
const ANumber		kHeight_HeaderButton = 15;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_TextMarker
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_TextMarker::AWindow_TextMarker():AWindow(),mGroupNameEditMode(false)
{
	//#725 �^�C�}�[�ΏۃE�C���h�E�ɂ���
	NVI_AddToTimerActionWindowArray();
}
/**
�f�X�g���N�^
*/
AWindow_TextMarker::~AWindow_TextMarker()
{
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_TextMarker::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + kHeight_Header + 10;
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//#725
/**
SubWindow�p�^�C�g���o�[�擾
*/
AView_SubWindowTitlebar&	AWindow_TextMarker::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
���j���[�I��������
*/
ABool	AWindow_TextMarker::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//�N���[�Y�{�^��
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
		//�O���[�v�I��
	  case kMenuItemID_TextMarkerSelectGroup:
		{
			//���݂̃e�L�X�g���e��K�p
			ApplyMarkerTextToDocument();
			//�O���[�v�ύX
			AIndex	index = inDynamicMenuActionText.GetNumber();
			GetApp().SPI_SetCurrentTextMarkerGroupIndex(index);
			result = true;
			//�`��v���p�e�B�X�V
			NVI_UpdateProperty();
			//view bouns�X�V
			UpdateViewBounds();
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
void	AWindow_TextMarker::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
�R���g���[���̃N���b�N���̃R�[���o�b�N
*/
ABool	AWindow_TextMarker::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	//
	if( inID != kControlID_NameEditBox )
	{
		ClearGroupNameEditMode(true);
	}
	//
	switch(inID)
	{
		//�O���[�v�ǉ��{�^��
	  case kControlID_AddGroupButton:
		{
			//���݂̃e�L�X�g���e��K�p
			ApplyMarkerTextToDocument();
			//�V�K�e�L�X�g�}�[�J�[�O���[�v�ǉ�
			GetApp().SPI_CreateNewTextMarkerGroup();
			//�`��v���p�e�B�X�V
			NVI_UpdateProperty();
			//view bounds�X�V
			UpdateViewBounds();
			//
			result = true;
			break;
		}
		//�O���[�v�폜�{�^��
	  case kControlID_DeleteGroupButton:
		{
			//if( GetApp().SPI_GetCurrentTextMarkerGroupIndex() != 0 )
			if( GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kTextMarkerArray_Text) > 1 )
			{
				//�폜�m�F�_�C�A���O�\��
				AText	mes1, mes2, mes3;
				mes1.SetLocalizedText("TextMarker_DeleteButton1");
				mes2.SetLocalizedText("TextMarker_DeleteButton2");
				mes3.SetLocalizedText("TextMarker_DeleteButton3");
				NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kControlID_DeleteGroupButtonOK);
			}
			//
			result = true;
			break;
		}
		//�폜�m�F�_�C�A���O��OK�N���b�N
	  case kControlID_DeleteGroupButtonOK:
		{
			//���݂̃e�L�X�g�}�[�J�[�O���[�v�폜
			GetApp().SPI_DeleteCurrentTextMarkerGroup();
			//�`��v���p�e�B�X�V
			NVI_UpdateProperty();
			//view bounds�X�V
			UpdateViewBounds();
			//
			result = true;
			break;
		}
		//�O���[�v���ύX�{�^��
	  case kControlID_ChangeGroupNameButton:
		{
			//���݂̃e�L�X�g���e��K�p
			ApplyMarkerTextToDocument();
			//�O���[�v���ύX���[�hON
			mGroupNameEditMode = true;
			//�`��v���p�e�B�X�V
			NVI_UpdateProperty();
			//view bounds�X�V
			UpdateViewBounds();
			//�O���[�v�^�C�g���擾�Aedit box�ɐݒ�
			AText	text, title;
			GetApp().SPI_GetCurrentTextMarkerData(title,text);
			NVI_GetEditBoxView(kControlID_NameEditBox).NVI_SetText(title);
			//edit box�Ƀt�H�[�J�X�ړ�
			NVI_SwitchFocusTo(kControlID_NameEditBox);
			//
			result = true;
			break;
		}
	}
	return result;
}

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_TextMarker::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}


/**
�e�L�X�g���͎�����
*/
void	AWindow_TextMarker::EVTDO_TextInputted( const AControlID inID )
{
}

/**
�R���g���[���̒l�ύX������
*/
ABool	AWindow_TextMarker::EVTDO_ValueChanged( const AControlID inID )
{
	switch(inID)
	{
		//�}�[�J�[�e�L�X�g�ύX������
	  case kControlID_Text:
		{
			//�e�h�L�������g�Ƀ}�[�J�[�K�p
			ApplyMarkerTextToDocument();
			break;
		}
	}
	return true;
}

/**
�R���g���[���Ń��^�[���L�[����������
*/
ABool	AWindow_TextMarker::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )
{
	//�t�B���^�œ��͎�
	switch(inControlID)
	{
	  case kControlID_Text:
		{
			//list view�ł̃N���b�N���Ɠ������������s
			ApplyMarkerTextToDocument();
			break;
		}
	  case kControlID_NameEditBox:
		{
			//�O���[�v���ҏW���[�h����
			ClearGroupNameEditMode(true);
			break;
		}
	}
	return true;
}

/**
�R���g���[����ESC�L�[����������
*/
ABool	AWindow_TextMarker::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )
{
	//�t�B���^�œ��͎�
	switch(inControlID)
	{
	  case kControlID_NameEditBox:
		{
			ClearGroupNameEditMode(false);
			break;
		}
	}
	return true;
}

/**
�O���[�v���ҏW���[�h����
*/
void	AWindow_TextMarker::ClearGroupNameEditMode( const ABool inApply )
{
	if( mGroupNameEditMode == true )
	{
		if( inApply == true )
		{
			//�O���[�v���ύX�K�p
			AText	title;
			NVI_GetEditBoxView(kControlID_NameEditBox).NVI_GetText(title);
			GetApp().SPI_SetCurrentTextMarkerData_Title(title);
		}
		//�O���[�v���ҏW���[�h����
		mGroupNameEditMode = false;
		//�`��v���p�e�B�X�V
		NVI_UpdateProperty();
		//view bounds�X�V
		UpdateViewBounds();
	}
	//���݂̃e�L�X�g���e��K�p
	ApplyMarkerTextToDocument();
}

/**
Marker���h�L�������g�ɓK�p����
*/
void	AWindow_TextMarker::ApplyMarkerTextToDocument()
{
	//�e�L�X�g�{�b�N�X����e�L�X�g�擾
	AText	text;
	NVI_GetControlText(kControlID_Text,text);
	//�}�[�J�[�e�L�X�g�K�p
	GetApp().SPI_SetCurrentTextMarkerData_Text(text);
}

/**
�E�C���h�E�̕���{�^��
*/
void	AWindow_TextMarker::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_TextMarker::NVIDO_Create( const ADocumentID inDocumentID )
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
	
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontName;
	AFloatNumber	fontSize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontName,fontSize);
	
	//�I���{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_SelectGroupButton,pt,11,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_SelectGroupButton,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
	}
	
	//�ǉ��{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_AddGroupButton,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_AddGroupButton,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_AddGroupButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("TextMarkerAdd");
		NVI_GetButtonViewByControlID(kControlID_AddGroupButton).NVI_SetText(text);
	}
	//�ǉ��{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_DeleteGroupButton,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_DeleteGroupButton,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_DeleteGroupButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("TextMarkerRemove");
		NVI_GetButtonViewByControlID(kControlID_DeleteGroupButton).NVI_SetText(text);
	}
	//�O���[�v���ύX�{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_ChangeGroupNameButton,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_ChangeGroupNameButton,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_ChangeGroupNameButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("TextMarkerEdit");
		NVI_GetButtonViewByControlID(kControlID_ChangeGroupNameButton).NVI_SetText(text);
	}
	
	//SubWindow�p�^�C�g���o�[����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_TextMarker");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwMarkerPen);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//�e�L�X�g�{�b�N�X����
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_Text,pt,10,10,kControlID_Invalid,kIndex_Invalid,false,false,false,true,
							  kEditBoxType_Normal);//#1316 kEditBoxType_NoFrameDraw);
		AText	filtertext;
		filtertext.SetLocalizedText("TextMarkerEmptyText");
		NVI_GetEditBoxView(kControlID_Text).SPI_SetTextForEmptyState(filtertext,GetEmptyText());
		NVI_GetEditBoxView(kControlID_Text).SPI_SetEnableFocusRing(false);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetCatchReturn(true);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetAutomaticSelectBySwitchFocus(false);
		NVI_SetControlBindings(kControlID_Text,true,true,true,true,false,false);
	}
	
	//�O���[�v���ύX�e�L�X�g�{�b�N�X����
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_NameEditBox,pt,10,10,kControlID_Invalid,kIndex_Invalid,false,false,false,true,kEditBoxType_Normal);
		NVI_GetEditBoxView(kControlID_NameEditBox).NVI_SetTextFont(fontName,fontSize);
		/*#1316 �F��EditBox�N���X���Őݒ肷��
		NVI_GetEditBoxView(kControlID_NameEditBox).SPI_SetBackgroundColor(
					kColor_White,kColor_Gray70Percent,kColor_Gray70Percent);
					*/
		NVI_GetEditBoxView(kControlID_NameEditBox).NVI_SetAutomaticSelectBySwitchFocus(false);
	}
	
	//SubWindow�p�Z�p���[�^����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_Text,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//V�X�N���[���o�[����
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateScrollBar(kControlID_VScrollBar,pt,kWidth_VScroll,kWidth_VScroll*2);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
		NVI_SetShowControl(kControlID_VScrollBar,true);
		NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	}
	
	//SizeBox����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
}

/**
�V�K�^�u����
�����F��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_TextMarker::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	NVI_UpdateProperty();
	//
	UpdateViewBounds();
}

/**
Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
*/
void	AWindow_TextMarker::NVIDO_Hide()
{
}

/**
*/
void	AWindow_TextMarker::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//view bounds�X�V
	UpdateViewBounds();
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontName;
	AFloatNumber	fontSize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontName,fontSize);
	//�}�[�J�[�e�L�X�gedit box�Ƀ��l�K�p
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		NVI_GetEditBoxView(kControlID_Text).SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
	}
	//���݂̃O���[�v�̃}�[�J�[�e�L�X�g�擾���Aedit box�ɐݒ�
	AText	title, text;
	GetApp().SPI_GetCurrentTextMarkerData(title,text);
	NVI_GetEditBoxView(kControlID_Text).NVI_SetText(text);
	//edit box�Ƀt�H���g�ݒ�
	NVI_GetEditBoxView(kControlID_Text).NVI_SetTextFont(fontName,fontSize);
	
	//�}�[�J�[�O���[�v�I���{�^���̐ݒ�
	ATextArray	groupTextArray;
	groupTextArray.SetFromTextArray(GetApp().NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kTextMarkerArray_Title));
	ATextArray	actionTextArray;
	for( AIndex i = 0;i < groupTextArray.GetItemCount(); i++ )
	{
		AText	actionText;
		actionText.SetNumber(i);
		actionTextArray.Add(actionText);
	}
	NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).
			SPI_SetMenuTextArray(groupTextArray,actionTextArray,kMenuItemID_TextMarkerSelectGroup);
	NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).NVI_SetText(title);
	
	//�폜�{�^����enable/disable�i�ŏ��̃O���[�v�͍폜�s�j
	//if( GetApp().SPI_GetCurrentTextMarkerGroupIndex() == 0 )
	if( GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kTextMarkerArray_Text) <= 1 )
	{
		NVI_SetControlEnable(kControlID_DeleteGroupButton,false);
	}
	else
	{
		NVI_SetControlEnable(kControlID_DeleteGroupButton,true);
	}
	
	//�w�b�_�F�擾
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	//#1316 �F�̓{�^���N���X���Őݒ肷�� AColor	headerlettercolor = GetApp().SPI_GetSubWindowHeaderLetterColor();
	
	//����{�^���F�ݒ�
	NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_AddGroupButton).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_DeleteGroupButton).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_ChangeGroupNameButton).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	//edit box�F�ݒ�
	NVI_GetEditBoxView(kControlID_Text).NVI_SetTextFont(fontName,fontSize);
	
	/*#1316 �F��EditBox�N���X���Őݒ肷��
	NVI_GetEditBoxView(kControlID_Text).SPI_SetTextColor(GetApp().SPI_GetSubWindowLetterColor(),GetApp().SPI_GetSubWindowLetterColor());
	NVI_GetEditBoxView(kControlID_Text).SPI_SetBackgroundColor(GetApp().SPI_GetSubWindowBackgroundColor(true),
															   kColor_Gray70Percent,kColor_Gray70Percent);
	NVI_GetEditBoxView(kControlID_Text).SPI_SetBackgroundColorForEmptyState(GetApp().SPI_GetSubWindowBackgroundColor(true),
																			GetApp().SPI_GetSubWindowBackgroundColor(false));
																			*/
	
	//�`��X�V
	NVI_RefreshWindow();
}

/**
view bounds�X�V
*/
void	AWindow_TextMarker::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//�^�C���o�[�A�Z�p���[�^�����擾
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	ANumber	height_Header = kHeight_Header;
	
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
			leftMargin = 3;//#1316 0;
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
	ANumber	vscrollbarWidth = kWidth_VScroll;
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
			listViewPoint.y += height_Titlebar + height_Header;
			listViewHeight -= height_Titlebar + height_Header;// + height_Separator;
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
			listViewPoint.y += height_Titlebar + height_Header;
			listViewHeight -= height_Titlebar + height_Header;
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
	
	//�O���[�v�I���{�^���z�u
	{
		NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).SPI_SetWidthToFitTextWidth();
		//ANumber	w = NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).NVI_GetViewWidth();
		AWindowPoint	pt = {2,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_SelectGroupButton,pt);
	}
	
	//�O���[�v���ύXtext box�z�u
	if( mGroupNameEditMode == true )
	{
		AWindowPoint	pt = {2,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_NameEditBox,pt);
		NVI_SetControlSize(kControlID_NameEditBox,listViewWidth - 32,24);
		//
		NVI_SetShowControl(kControlID_NameEditBox,true);
	}
	else
	{
		//
		NVI_SetShowControl(kControlID_NameEditBox,false);
	}
	
	//�O���[�v���ύX�{�^���z�u
	ANumber	wgroupname = 0;
	{
		NVI_GetButtonViewByControlID(kControlID_ChangeGroupNameButton).SPI_SetWidthToFitTextWidth();
		wgroupname = NVI_GetButtonViewByControlID(kControlID_ChangeGroupNameButton).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - wgroupname - 3,height_Titlebar + kHeight_Header1Line + 3};
		NVI_SetControlPosition(kControlID_ChangeGroupNameButton,pt);
	}
	
	//�폜�{�^���z�u
	ANumber	wminus = 0;
	{
		NVI_GetButtonViewByControlID(kControlID_DeleteGroupButton).SPI_SetWidthToFitTextWidth();
		wminus = NVI_GetButtonViewByControlID(kControlID_DeleteGroupButton).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - wminus - wgroupname - 3,height_Titlebar + kHeight_Header1Line + 3};
		NVI_SetControlPosition(kControlID_DeleteGroupButton,pt);
	}
	
	//�ǉ��{�^���z�u
	ANumber	wplus = 0;
	{
		NVI_GetButtonViewByControlID(kControlID_AddGroupButton).SPI_SetWidthToFitTextWidth();
		wplus = NVI_GetButtonViewByControlID(kControlID_AddGroupButton).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - wplus - wminus - wgroupname - 3,height_Titlebar + kHeight_Header1Line + 3};
		NVI_SetControlPosition(kControlID_AddGroupButton,pt);
	}
	
	//text box�z�u
	{
		NVI_SetControlPosition(kControlID_Text,listViewPoint);
		NVI_SetControlSize(kControlID_Text,listViewWidth - vscrollbarWidth,listViewHeight-5);
		NVI_GetEditBoxView(kControlID_Text).SPI_AdjustScroll();
	}
	
	//V�X�N���[���o�[�z�u
	if( vscrollbarWidth > 0 /*&& NVI_IsControlEnable(kControlID_VScrollBar) == true*/ )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(kControlID_VScrollBar,pt);
		NVI_SetControlSize(kControlID_VScrollBar,vscrollbarWidth,listViewHeight - sizeboxHeight);
		NVI_SetShowControl(kControlID_VScrollBar,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_VScrollBar,false);
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

