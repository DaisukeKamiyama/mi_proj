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

AWindow_KeyRecord
#725

*/

#include "stdafx.h"

#include "AWindow_KeyRecord.h"
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
const ANumber	kHeight_Header = 20;

//�w�b�_���e��R���g���[��
const AControlID	kControlID_RecordStart = 106;
const AControlID	kControlID_RecordStop = 107;
const AControlID	kControlID_Play = 108;
const AControlID	kControlID_AddMacro = 109;
const ANumber		kHeight_HeaderButton = 15;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_KeyRecord
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_KeyRecord::AWindow_KeyRecord():AWindow()
{
	//#725 �^�C�}�[�ΏۃE�C���h�E�ɂ���
	NVI_AddToTimerActionWindowArray();
}
/**
�f�X�g���N�^
*/
AWindow_KeyRecord::~AWindow_KeyRecord()
{
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_KeyRecord::SPI_GetSubWindowMinHeight() const
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
AView_SubWindowTitlebar&	AWindow_KeyRecord::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
���j���[�I��������
*/
ABool	AWindow_KeyRecord::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
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
void	AWindow_KeyRecord::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
�R���g���[���̃N���b�N���̃R�[���o�b�N
*/
ABool	AWindow_KeyRecord::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	//�G�f�B�b�g�{�b�N�X�̃e�L�X�g���f�[�^�ɔ��f
	ApplyTextToKeyRecord();
	//
	ABool	result = false;
	//
	switch(inID)
	{
		//
	  case kControlID_RecordStart:
		{
			//�L�[�L�^�J�n
			GetApp().SPI_StartRecord();
			//
			result = true;
			break;
		}
		//
	  case kControlID_RecordStop:
		{
			//�L�[�L�^��~
			GetApp().SPI_StopRecord();
			//
			result = true;
			break;
		}
		//
	  case kControlID_Play:
		{
			//�L�[�Đ�
			AObjectID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextView().SPI_PlayKeyRecord();
			}
			//
			result = true;
			break;
		}
		//
	  case kControlID_AddMacro:
		{
			//�}�N���ɒǉ�
			AObjectID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(winID).SPI_ShowAddToolButtonWindowFromKeyRecord();
			}
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
void	AWindow_KeyRecord::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}


/**
�e�L�X�g���͎�����
*/
void	AWindow_KeyRecord::EVTDO_TextInputted( const AControlID inID )
{
}

/**
�R���g���[���̒l�ύX������
*/
ABool	AWindow_KeyRecord::EVTDO_ValueChanged( const AControlID inID )
{
	switch(inID)
	{
		//�G�f�B�b�g�{�b�N�X�̃e�L�X�g���f�[�^�ɔ��f
	  case kControlID_Text:
		{
			ApplyTextToKeyRecord();
			break;
		}
	}
	return true;
}

/**
�R���g���[���Ń��^�[���L�[����������
*/
ABool	AWindow_KeyRecord::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )
{
	//�t�B���^�œ��͎�
	switch(inControlID)
	{
	}
	return true;
}

/**
�R���g���[����ESC�L�[����������
*/
ABool	AWindow_KeyRecord::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )
{
	//�t�B���^�œ��͎�
	switch(inControlID)
	{
	}
	return true;
}

/**
�E�C���h�E�̕���{�^��
*/
void	AWindow_KeyRecord::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}

/**
���݂̃G�f�B�b�g�{�b�N�X�̃e�L�X�g���L�[�L�^�f�[�^�ɔ��f
*/
void	AWindow_KeyRecord::ApplyTextToKeyRecord()
{
	AText	text;
	NVI_GetEditBoxView(kControlID_Text).NVI_GetText(text);
	GetApp().SPI_SetKeyRecordedRawText(text);
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_KeyRecord::NVIDO_Create( const ADocumentID inDocumentID )
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
	
	//�L�^�J�n�{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_RecordStart,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_RecordStart,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_RecordStart).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("KeyRecordStart");
		NVI_GetButtonViewByControlID(kControlID_RecordStart).NVI_SetText(text);
	}
	//��~�{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_RecordStop,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_RecordStop,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_RecordStop).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("KeyRecordStop");
		NVI_GetButtonViewByControlID(kControlID_RecordStop).NVI_SetText(text);
	}
	//�Đ��{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_Play,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_Play,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_Play).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("KeyRecordPlay");
		NVI_GetButtonViewByControlID(kControlID_Play).NVI_SetText(text);
	}
	//�}�N���ɒǉ��{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_AddMacro,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_AddMacro,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_AddMacro).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("KeyRecordAddMacro");
		NVI_GetButtonViewByControlID(kControlID_AddMacro).NVI_SetText(text);
	}
	
	//SubWindow�p�^�C�g���o�[����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_KeyRecord");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconTbKeyRecord);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//�e�L�X�g�{�b�N�X����
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_Text,pt,10,10,kControlID_Invalid,kIndex_Invalid,false,false,false,true,kEditBoxType_Normal);//#1316 kEditBoxType_NoFrameDraw);
		NVI_GetEditBoxView(kControlID_Text).SPI_SetTextForEmptyState(GetEmptyText(),GetEmptyText());
		NVI_GetEditBoxView(kControlID_Text).SPI_SetEnableFocusRing(false);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetCatchReturn(true);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetAutomaticSelectBySwitchFocus(false);
		//NVI_GetEditBoxView(kControlID_Text).SPI_SetReadOnly();
		NVI_SetControlBindings(kControlID_Text,true,true,true,true,false,false);
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
void	AWindow_KeyRecord::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	NVI_UpdateProperty();
	//
	UpdateViewBounds();
}

/**
Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
*/
void	AWindow_KeyRecord::NVIDO_Hide()
{
}

/**
*/
void	AWindow_KeyRecord::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontName;
	AFloatNumber	fontSize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontName,fontSize);
	//�w�b�_�p�t�H���g�擾
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
    //#1316 �F�̓{�^���N���X���Őݒ肷�� AColor	headerlettercolor = GetApp().SPI_GetSubWindowHeaderLetterColor();
	//����{�^���F�ݒ�
	NVI_GetButtonViewByControlID(kControlID_RecordStart).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_RecordStop).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_Play).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_AddMacro).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	//edit box�F�ݒ�
	NVI_GetEditBoxView(kControlID_Text).NVI_SetTextFont(fontName,fontSize);
	NVI_GetEditBoxView(kControlID_Text).SPI_SetTextColor(GetApp().SPI_GetSubWindowLetterColor(),GetApp().SPI_GetSubWindowLetterColor());
	/*#1316 �F��EditBox�N���X���Őݒ肷��
	NVI_GetEditBoxView(kControlID_Text).SPI_SetBackgroundColor(GetApp().SPI_GetSubWindowBackgroundColor(true),
															   kColor_Gray70Percent,kColor_Gray70Percent);
	NVI_GetEditBoxView(kControlID_Text).SPI_SetBackgroundColorForEmptyState(GetApp().SPI_GetSubWindowBackgroundColor(true),
																			GetApp().SPI_GetSubWindowBackgroundColor(false));
																			*/
	
	//view bounds�X�V
	UpdateViewBounds();
}

/**
�R���g���[����ԁiEnable/Disable���j���X�V
NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()����R�[�������
�N���b�N��A�N�e�B�x�[�g�ŃR�[�������̂ŁA���܂�d��������NVMDO_UpdateControlStatus()�ɂ͓���Ȃ����ƁB
*/
void	AWindow_KeyRecord::NVMDO_UpdateControlStatus()
{
	NVI_SetControlEnable(kControlID_RecordStart,(GetApp().SPI_IsKeyRecording()==false));
	NVI_SetControlEnable(kControlID_RecordStop,(GetApp().SPI_IsKeyRecording()==true));
	NVI_SetControlEnable(kControlID_Play,((GetApp().SPI_IsKeyRecording()==false)&&(GetApp().SPI_ExistRecordedText()==true)));
	NVI_SetControlEnable(kControlID_AddMacro,((GetApp().SPI_IsKeyRecording()==false)&&(GetApp().SPI_ExistRecordedText()==true)));
}

/**
view bounds�X�V
*/
void	AWindow_KeyRecord::UpdateViewBounds()
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
	
	//�}�N���ɒǉ��{�^���z�u
	ANumber	w_buttons = 0;
	{
		NVI_GetButtonViewByControlID(kControlID_AddMacro).SPI_SetWidthToFitTextWidth();
		w_buttons += NVI_GetButtonViewByControlID(kControlID_AddMacro).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - w_buttons - 3,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_AddMacro,pt);
	}
	
	//�O���[�v���ύX�{�^���z�u
	{
		NVI_GetButtonViewByControlID(kControlID_Play).SPI_SetWidthToFitTextWidth();
		w_buttons += NVI_GetButtonViewByControlID(kControlID_Play).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - w_buttons - 3,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_Play,pt);
	}
	
	//�폜�{�^���z�u
	{
		NVI_GetButtonViewByControlID(kControlID_RecordStop).SPI_SetWidthToFitTextWidth();
		w_buttons += NVI_GetButtonViewByControlID(kControlID_RecordStop).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - w_buttons - 3,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_RecordStop,pt);
	}
	
	//�ǉ��{�^���z�u
	{
		NVI_GetButtonViewByControlID(kControlID_RecordStart).SPI_SetWidthToFitTextWidth();
		w_buttons += NVI_GetButtonViewByControlID(kControlID_RecordStart).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - w_buttons - 3,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_RecordStart,pt);
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

/**
UI�ɃL�[�L�^�e�L�X�g��ǉ�
*/
void	AWindow_KeyRecord::SPI_AddRecordedText( const AText& inText )
{
	NVI_GetEditBoxView(kControlID_Text).NVI_AddText(inText);
	NVI_GetEditBoxView(kControlID_Text).SPI_ArrowKeyEdge(kUChar_Down,false);
}

/**
UI�ɃL�[�L�^�e�L�X�g��ݒ�
*/
void	AWindow_KeyRecord::SPI_SetRecordedText( const AText& inText )
{
	NVI_GetEditBoxView(kControlID_Text).NVI_SetText(inText);
}


