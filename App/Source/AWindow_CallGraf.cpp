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

AWindow_CallGraf
#723

*/

#include "stdafx.h"

#include "AWindow_CallGraf.h"
#include "AView_CallGraf.h"
#include "AView_CallGrafHeader.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowStatusBar.h"
#include "AView_SubWindowSizeBox.h"

//�R�[���O���t
const AControlID	kControlID_CallGrafView = 101;

//�X�N���[���o�[
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kWidth_VScroll = 11;
//const AControlID	kControlID_HScrollBar = 106;
//const ANumber		kHeight_HScroll = 11;

//CallGraf�w�b�_
const AControlID	kControlID_CallGrafHeaderView = 110;
const ANumber		kHeight_CallGrafHeader = AView_CallGrafHeader::kLineHeight*2;

//�e�{�^��
const AControlID	kControlID_ReturnHome = 105;
const ANumber		kHeight_HomeButton = 17;
const ANumber		kWidth_LevelButton = 72;
const ANumber		kHeight_HeaderButtons = 18;

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
//const AControlID	kControlID_ImportFileProgressStatusBar = 803;

//Find�v���O���X�\��
const AControlID	kControlID_FindProgressIndicator = 811;

//Progress indicator�T�C�Y
const ANumber	kHeight_ProgressIndicator = 16;
const ANumber	kWidth_ProgressIndicator = 16;


#pragma mark ===========================
#pragma mark [�N���X]AWindow_CallGraf
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_CallGraf::AWindow_CallGraf():AWindow(), mAllFilesSearched(false), mShowImportFileProgress(false)
{
}
/**
�f�X�g���N�^
*/
AWindow_CallGraf::~AWindow_CallGraf()
{
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_CallGraf::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + kHeight_CallGrafHeader + 10;
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

/**
TextView�擾
*/
AView_CallGraf&	AWindow_CallGraf::SPI_GetCallGrafView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_CallGraf,kViewType_CallGraf,kControlID_CallGrafView);
}

/**
CallGraf�r���[�擾
*/
AView_CallGrafHeader&	AWindow_CallGraf::SPI_GetCallGrafHeaderView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_CallGrafHeader,kViewType_CallGrafHeader,kControlID_CallGrafHeaderView);
}

//#725
/**
SubWindow�p�^�C�g���o�[�擾
*/
AView_SubWindowTitlebar&	AWindow_CallGraf::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
���j���[���ڑI��������
*/
ABool	AWindow_CallGraf::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
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
���j���[�X�V
*/
void	AWindow_CallGraf::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
�R���g���[���̃N���b�N���̃R�[���o�b�N
*/
ABool	AWindow_CallGraf::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_ReturnHome:
		{
			SPI_GetCallGrafView().SPI_ReturnHome();
			break;
		}
	  case kControlID_LeftLevelButton:
		{
			SPI_GetCallGrafView().SPI_GoLeftLevel();
			break;
		}
	  case kControlID_RightLevelButton:
		{
			SPI_GetCallGrafView().SPI_GoRightLevel();
			break;
		}
	}
	return result;
}

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_CallGraf::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

/**
�E�C���h�E�̕���{�^��
*/
void	AWindow_CallGraf::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}


#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_CallGraf::NVIDO_Create( const ADocumentID inDocumentID )
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
		text.SetLocalizedText("SubWindowTitle_CallGraf");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwTree);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//==================CallGraf view����==================
	
	//CallGraf view����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_CallGraf>	callGrafViewFactory(GetObjectID(),kControlID_CallGrafView);
		NVM_CreateView(kControlID_CallGrafView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,callGrafViewFactory);
		NVI_SetControlBindings(kControlID_CallGrafView,true,true,true,true,false,false);
	}
	
	//==================Import File Progress����==================
	
	/*
	//Import File Progress�e�L�X�g�p�l������
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowStatusBar>	viewFactory(GetObjectID(),kControlID_ImportFileProgressStatusBar);
		NVM_CreateView(kControlID_ImportFileProgressStatusBar,pt,10,10,kControlID_CallGrafView,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_ImportFileProgressStatusBar,true,true,true,false,false,true);
		//�w�i�F���ݒ�
		NVI_GetViewByControlID(kControlID_ImportFileProgressStatusBar).NVI_SetBackgroundColor(
					GetApp().SPI_GetSubWindowBackgroundColor(true),
					GetApp().SPI_GetSubWindowBackgroundColor(false));
	}
	*/
	
	//Import File Progress�e�L�X�g����
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateButtonView(kControlID_ImportFileProgressText,pt,10,kHeight_ProgressIndicator,
							 kControlID_Background);//kControlID_ImportFileProgressStatusBar);
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).SPI_SetButtonViewType(kButtonViewType_NoFrame);
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).
				SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,kTextStyle_Normal);//#1316
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
	
	//==================Find Progress����==================
	
	//Find Progress Indicator����
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateProgressIndicator(kControlID_FindProgressIndicator,pt,kWidth_ProgressIndicator,kHeight_ProgressIndicator);
		NVI_EmbedControl(kControlID_CallGrafView,kControlID_FindProgressIndicator);
	}
	
	//==================�Z�p���[�^����==================
	
	//SubWindow�p�Z�p���[�^����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_CallGrafView,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//==================CallGraf�w�b�_����==================
	
	//CallGrafHeader view����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_CallGrafHeader>	viewFactory(GetObjectID(),kControlID_CallGrafHeaderView);
		NVM_CreateView(kControlID_CallGrafHeaderView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_CallGrafHeaderView,true,true,true,false,false,true);
	}
	
	//==================����{�^������==================
	
	//�z�[���{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_ReturnHome,pt,10,kHeight_HomeButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_ReturnHome,false,true,false,false,false,true);
		NVI_GetButtonViewByControlID(kControlID_ReturnHome).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		NVI_GetButtonViewByControlID(kControlID_ReturnHome).
				SPI_SetOvalHoverColor(GetApp().SPI_GetSubWindowTitlebarButtonHoverColor());
		NVI_GetButtonViewByControlID(kControlID_ReturnHome).SPI_SetIcon(kImageID_iconSwHome);
		AText	text;
		text.SetLocalizedText("CallGrafHome");
		NVI_GetButtonViewByControlID(kControlID_ReturnHome).NVI_SetText(text);
	}
	
	/*
	//�����x���{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_LeftLevelButton,pt,10,10,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_LeftLevelButton,true,true,false,false,true,true);
	}
	
	//�E���x���{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_RightLevelButton,pt,10,10,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_RightLevelButton,false,true,true,false,true,true);
	}
	*/
	
	//==================�E�C���h�E����R���g���[������==================
	
	//V�X�N���[���o�[����
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateScrollBar(kControlID_VScrollBar,pt,kWidth_VScroll,kWidth_VScroll*2);
		SPI_GetCallGrafView().NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
		NVI_SetShowControl(kControlID_VScrollBar,true);
		NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	}
	
	/*
	//H�X�N���[���o�[����
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateScrollBar(kControlID_HScrollBar,pt,kHeight_HScroll*2,kHeight_HScroll);
		NVI_SetMinMax(kControlID_HScrollBar,0,100);
		NVI_SetShowControl(kControlID_HScrollBar,true);
		NVI_SetControlBindings(kControlID_HScrollBar,true,false,true,true,false,true);
	}
	*/
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
void	AWindow_CallGraf::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	UpdateViewBounds();
}

/**
Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
*/
void	AWindow_CallGraf::NVIDO_Hide()
{
}

/**
*/
void	AWindow_CallGraf::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//�����v���O���X�\���E��\������
	NVI_SetShowControl(kControlID_FindProgressIndicator,SPI_GetCallGrafView().SPI_IsFinding());
	NVI_SetControlBool(kControlID_FindProgressIndicator,true);
	
	//�z�[���{�^���F�ݒ�
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);
    //#1316 AColor	color = GetApp().SPI_GetSubWindowTitlebarTextColor();
	NVI_GetButtonViewByControlID(kControlID_ReturnHome).
			SPI_SetTextProperty(fontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	
	//view bounds�X�V
	UpdateViewBounds();
	//�`��X�V
	NVI_RefreshWindow();
}

/**
view bounds�X�V
*/
void	AWindow_CallGraf::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//�^�C���o�[�A�Z�p���[�^�����擾
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	
	//�T�C�h�o�[�Ō�̍��ڂ��ǂ������擾
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
	//
	AWindowPoint	headerViewPoint = {leftMargin,0};
	ANumber	headerViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
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
			listViewPoint.y += height_Titlebar /*+ kHeight_HeaderButtons*/ + kHeight_CallGrafHeader;
			listViewHeight -= height_Titlebar /*+ kHeight_HeaderButtons*/ + kHeight_CallGrafHeader;// + height_Separator;
			//
			headerViewPoint.y += height_Titlebar;// + kHeight_HeaderButtons;
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
			listViewPoint.y += height_Titlebar /*+ kHeight_HeaderButtons*/ + kHeight_CallGrafHeader;
			listViewHeight -= height_Titlebar /*+ kHeight_HeaderButtons*/ + kHeight_CallGrafHeader;
			//
			headerViewPoint.y += height_Titlebar;// + kHeight_HeaderButtons;
			//
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
	
	//�v���O���X�o�[�z�u�v�Z
	ABool	showProgress = mShowImportFileProgress;
	AWindowPoint	progressPoint = {0};
	if( showProgress == true )
	{
		//list view�������v���O���X�o�[�����炷
		listViewHeight -= kHeight_ProgressArea;
		//�v���O���X�o�[�ʒu�v�Z
		progressPoint.x = leftMargin;
		progressPoint.y = listViewPoint.y + listViewHeight;
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
	
	//CallGrafHeader�z�u
	{
		NVI_SetControlPosition(kControlID_CallGrafHeaderView,headerViewPoint);
		NVI_SetControlSize(kControlID_CallGrafHeaderView,headerViewWidth,kHeight_CallGrafHeader);
	}
	
	//==================CallGraf�z�u==================
	{
		NVI_SetControlPosition(kControlID_CallGrafView,listViewPoint);
		NVI_SetControlSize(kControlID_CallGrafView,listViewWidth - vscrollbarWidth,listViewHeight);
	}
	
	//==================Import File Progress�z�u==================
	/*
	//Import File Progress�e�L�X�g�p�l���z�u�E�\��
	{
		NVI_SetControlPosition(kControlID_ImportFileProgressStatusBar,progressPoint);
		NVI_SetControlSize(kControlID_ImportFileProgressStatusBar,
		windowBounds.right - windowBounds.left - vscrollbarWidth,kHeight_ProgressArea);
		NVI_SetShowControl(kControlID_ImportFileProgressStatusBar,showProgress);
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
	//==================�����v���O���X�z�u==================
	
	//Find Progress Indicator�z�u
	{
		AWindowPoint	pt = {0};
		pt.x = listViewPoint.x + 5;
		pt.y = listViewPoint.y + 2;
		NVI_SetControlPosition(kControlID_FindProgressIndicator,pt);
		NVI_SetShowControl(kControlID_FindProgressIndicator,showProgress);
		//
		NVI_SetShowControl(kControlID_FindProgressIndicator,SPI_GetCallGrafView().SPI_IsFinding());
		NVI_SetControlBool(kControlID_FindProgressIndicator,true);
	}
	
	//==================����{�^���z�u==================
	
	//�z�[���{�^���z�u
	{
		NVI_GetButtonViewByControlID(kControlID_ReturnHome).SPI_SetWidthToFitTextWidth();
		ANumber	w = NVI_GetButtonViewByControlID(kControlID_ReturnHome).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right - windowBounds.left - w - 5,1};
		NVI_SetControlPosition(kControlID_ReturnHome,pt);
	}
	
	/*
	//�����x���{�^���z�u
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar};
		NVI_SetControlPosition(kControlID_LeftLevelButton,pt);
		NVI_SetControlSize(kControlID_LeftLevelButton,kWidth_LevelButton-leftMargin,kHeight_HeaderButtons);
	}
	
	//�E���x���{�^���z�u
	{
		AWindowPoint	pt = {windowBounds.right - windowBounds.left - kWidth_LevelButton,height_Titlebar};
		NVI_SetControlPosition(kControlID_RightLevelButton,pt);
		NVI_SetControlSize(kControlID_RightLevelButton,kWidth_LevelButton-rightMargin,kHeight_HeaderButtons);
	}
	*/
	
	//==================�E�C���h�E����R���g���[���z�u==================
	
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
	/*
	//H�X�N���[���o�[�z�u
	{
		AWindowPoint	pt = {0,listViewPoint.y + listViewHeight};
		NVI_SetControlPosition(kControlID_HScrollBar,pt);
		NVI_SetControlSize(kControlID_HScrollBar,listViewWidth,kHeight_HScroll);
		NVI_SetShowControl(kControlID_HScrollBar,true);
	}
	*/
	
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
Import File Progress�\���E��\��
*/
void	AWindow_CallGraf::SPI_ShowHideImportFileProgress( const ABool inShow )
{
	//�\���E��\���ύX�Ȃ��Ȃ牽�����Ȃ�
	if( mShowImportFileProgress == inShow )
	{
		return;
	}
	
	//�\���E��\��
	mShowImportFileProgress = inShow;
	UpdateViewBounds();
}



