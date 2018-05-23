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

AWindow_Previewer
#734

*/

#include "stdafx.h"

#include "AWindow_Previewer.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowSizeBox.h"

//WebView controlID
const AControlID	kControlID_WebView = 101;

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

//�e�{�^��
const AControlID	kControlID_LetterSize = 105;
const ANumber		kHeight_LetterSize = 15;


#pragma mark ===========================
#pragma mark [�N���X]AWindow_Previewer
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AWindow_Previewer::AWindow_Previewer():AWindow()
{
}
/**
�f�X�g���N�^
*/
AWindow_Previewer::~AWindow_Previewer()
{
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_Previewer::SPI_GetSubWindowMinHeight() const
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
AView_SubWindowTitlebar&	AWindow_Previewer::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
���j���[�I��������
*/
ABool	AWindow_Previewer::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
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
		//�����T�C�Y�p�[�Z���g�擾
	  case kMenuItemID_Percent:
		{
			ANumber	percent = inDynamicMenuActionText.GetNumber();
			SPI_SetLetterSize(percent);
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
void	AWindow_Previewer::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
�R���g���[���̃N���b�N���̃R�[���o�b�N
*/
ABool	AWindow_Previewer::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	return result;
}

/**
�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_Previewer::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

/**
�E�C���h�E�̕���{�^��
*/
void	AWindow_Previewer::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}


#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_Previewer::NVIDO_Create( const ADocumentID inDocumentID )
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
	
	//SubWindow�p�^�C�g���o�[����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_Previewer");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwPreview);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//Web view����
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateWebView(kControlID_WebView,pt,100,600);//���T�C�Y��10,10�ɂ���ƕ\������Ȃ��B�����K�v
		NVI_SetShowControl(kControlID_WebView,true);
		NVI_SetControlBindings(kControlID_WebView,true,true,true,true,false,false);
	}
	
	//�����T�C�Y�{�^������
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_LetterSize,pt,10,kHeight_LetterSize,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_LetterSize,false,true,false,false,false,true);
		NVI_GetButtonViewByControlID(kControlID_LetterSize).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	fontname;
		AFontWrapper::GetDialogDefaultFontName(fontname);
		AColor	color = GetApp().SPI_GetSubWindowTitlebarTextColor();
		NVI_GetButtonViewByControlID(kControlID_LetterSize).SPI_SetTextProperty(fontname,9.0,kJustification_Center,
					kTextStyle_Bold|kTextStyle_DropShadow,color,color);
		//
		NVI_GetButtonViewByControlID(kControlID_LetterSize).SPI_SetMenu(kContextMenuID_Percent,kMenuItemID_Invalid);
	}
	
	//SubWindow�p�Z�p���[�^����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_WebView,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
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
void	AWindow_Previewer::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	NVI_UpdateProperty();
	//
	UpdateViewBounds();
}

/**
Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
*/
void	AWindow_Previewer::NVIDO_Hide()
{
}

/**
*/
void	AWindow_Previewer::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//�����T�C�Y�g�嗦�ݒ�
	AFloatNumber	mul = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kPreviewerMultiplier);
	mul /= 100.0;
	NVI_SetWebViewFontMultiplier(kControlID_WebView,mul);
	//�����T�C�Y�{�^���Ɂu�����T�C�Y�F�`%�v��ݒ�
	AText	text;
	text.SetLocalizedText("PreviewWindow_LetterSizeButton");
	text.ReplaceParamText('0',GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kPreviewerMultiplier));
	NVI_GetButtonViewByControlID(kControlID_LetterSize).NVI_SetText(text);
	//�`��X�V
	NVI_RefreshWindow();
}

/**
WebView�ւ�URL�\���v��
*/
void	AWindow_Previewer::SPI_RequestLoadURL( const AText& inURL )
{
	//�O��v������URL�ƍ���v��URL�������ꍇ�A�����������^�[��
	if( inURL.Compare(mRequestedURL) == true )
	{
		return;
	}
	
	//�\���v��
	//AText	test("file:///Users/daisuke/kadai.pdf#page=3");
	NVI_LoadURLToWebView(kControlID_WebView,inURL);
	
	//�v��URL��ۑ�
	mRequestedURL.SetText(inURL);
	//���ݕ\������URL��ۑ�
	NVI_GetCurrentWebViewURL(kControlID_WebView,mDisplayedURLWhenLoaded);
}

/**
�Ō�ɗv�����ꂽURL�������[�h����
@note �t�@�C���ۑ��̔��f�p
*/
void	AWindow_Previewer::SPI_RequestReload()
{
	//���ݕ\������URL�擾
	AText	currentURL;
	NVI_GetCurrentWebViewURL(kControlID_WebView,currentURL);
	
	if( currentURL.Compare(mDisplayedURLWhenLoaded) == true )
	{
		//���ݕ\������URL���A�v������URL�Ɠ����ꍇ�A�����[�h����
		NVI_ReloadWebView(kControlID_WebView);
	}
	else
	{
		//���ݕ\������URL���A�v������URL�ƈႤ�ꍇ�i��web view��̃��[�U�[����ɂ��y�[�W�ړ������ꍇ�j�A�v������URL���ēx�v������
		NVI_LoadURLToWebView(kControlID_WebView,mRequestedURL);
		//���ݕ\������URL��ۑ�
		NVI_GetCurrentWebViewURL(kControlID_WebView,mDisplayedURLWhenLoaded);
	}
}

/**
Web view���Ŏw��JavaScript�����s����
*/
void	AWindow_Previewer::SPI_ExecuteJavaScript( const AText& inText )
{
	NVI_ExecuteJavaScriptInWebView(kControlID_WebView,inText);
}

/**
�����T�C�Y�ݒ�
*/
void	AWindow_Previewer::SPI_SetLetterSize( const ANumber inPercent )
{
	GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kPreviewerMultiplier,inPercent);
	NVI_UpdateProperty();
	UpdateViewBounds();
}

/**
View�z�u�X�V
*/
void	AWindow_Previewer::UpdateViewBounds()
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
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;// + height_Separator;
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
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;
			//
			showSizeBox = true;
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
	
	//WebView�z�u
	{
		NVI_SetControlPosition(kControlID_WebView,listViewPoint);
		NVI_SetControlSize(kControlID_WebView,listViewWidth,listViewHeight);
		NVI_SetShowControl(kControlID_WebView,true);
	}
	
	//�����T�C�Y�{�^���z�u
	{
		NVI_GetButtonViewByControlID(kControlID_LetterSize).SPI_SetWidthToFitTextWidth();
		ANumber	w = NVI_GetButtonViewByControlID(kControlID_LetterSize).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right - windowBounds.left - w - 5,3};
		NVI_SetControlPosition(kControlID_LetterSize,pt);
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

