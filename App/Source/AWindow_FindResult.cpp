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

AWindow_FindResult
#725

*/

#include "stdafx.h"

#include "AWindow_FindResult.h"
#include "ADocument_IndexWindow.h"
#include "AView_Index.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowSizeBox.h"

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

//�X�N���[���o�[
const ANumber		kWidth_VScroll = 11;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_FindResult
#pragma mark ===========================
//�C���f�b�N�X�E�C���h�E�̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_FindResult::AWindow_FindResult( const ADocumentID inDocumentID )
	:AWindow(), mDocumentID(inDocumentID), mQuitting(false), mAllClosing(false)//drop , mAskingSaveChanges(false)
	,mIndexViewID(kObjectID_Invalid)//#92
{
}
//�f�X�g���N�^
AWindow_FindResult::~AWindow_FindResult()
{
}

/**
�E�C���h�E�̒ʏ펞�icollapse���Ă��Ȃ����j�̍ŏ������擾
*/
ANumber	AWindow_FindResult::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 10;
}

ADocument_IndexWindow&	AWindow_FindResult::GetDocument()
{
	return GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID);
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//#725
/**
SubWindow�p�^�C�g���o�[�擾
*/
AView_SubWindowTitlebar&	AWindow_FindResult::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//
ABool	AWindow_FindResult::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			NVI_TryClose();//#92
			result = true;
			break;
		}
		/*drop
		//Save
	  case kMenuItemID_Save:
		{
			GetDocument().SPI_Save(false);
			break;
		}
		//SaveAs
	  case kMenuItemID_SaveAs:
		{
			GetDocument().SPI_Save(true);
			break;
		}
		*/
		//#465
	  case kMenuItemID_CM_IndexView:
		{
			AIndex	rowIndex = AView_Index::GetIndexViewByViewID(mIndexViewID).SPI_GetContextMenuSelectedRowDBIndex();
			AView_Index::GetIndexViewByViewID(mIndexViewID).SPI_ExportToNewDocument(rowIndex);
			break;
		}
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

//
void	AWindow_FindResult::EVTDO_UpdateMenu()
{
	/*drop
	//Save
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Save,true);
	//SaveAs
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SaveAs,true);
	*/
}

//�E�C���h�E�̕���{�^��
void	AWindow_FindResult::EVTDO_DoCloseButton()
{
	//#92 TryClose();
	NVI_TryClose();//#92
}

//
void	AWindow_FindResult::EVTDO_WindowResizeCompleted( const ABool inResized )
{
	//View�̈ʒu�A�T�C�Y�X�V
	UpdateViewBounds();
	NVI_RefreshWindow();
}

/*drop
//AskSaveChanges�E�C���h�E�̉�����M����
void	AWindow_FindResult::EVTDO_AskSaveChangesReply( const AObjectID inDocumentID, const AAskSaveChanges inAskSaveChangesReply )
{
	switch(inAskSaveChangesReply)
	{
		//�L�����Z��
	  case kAskSaveChanges_Cancel:
		{
			mQuitting = false;
			mAllClosing = false;
			break;
		}
		//�ۑ����Ȃ�
	  case kAskSaveChanges_DontSave:
		{
			//#92 ExecuteClose();
			NVI_ExecuteClose();//#92
			break;
		}
		//�ۑ�
	  case kAskSaveChanges_Save:
		{
			GetDocument().SPI_Save(false);
			//#92 ExecuteClose();
			NVI_ExecuteClose();//#92
			break;
		}
	}
	mAskingSaveChanges = false;
	if( mQuitting == true )
	{
		mQuitting = false;
		mAllClosing = false;
		GetApp().NVI_Quit(true);//#433
	}
	else if( mAllClosing == true )
	{
		mQuitting = false;
		mAllClosing = false;
		GetApp().NVI_CloseAllWindow(false);
	}
}

//�ۑ��_�C�A���O������M����
void	AWindow_FindResult::EVTDO_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText )
{
	AText	filename;
	inFile.GetFilename(filename);
	GetDocument().SPI_SetTitle(filename);
	GetDocument().NVI_SpecifyFile(inFile);
	GetDocument().SPI_Save(false);
}
*/
//���j���[����u����v�I���A�������́A�E�C���h�E�̕���{�^���N���b�N���AClose��Try����B�iDirty�Ȃ�AskSaveChanges�E�C���h�E���J���j
//#92 void	AWindow_FindResult::TryClose()
void	AWindow_FindResult::NVIDO_TryClose( const AIndex inTabIndex )
{
	//#92 NVI_ExecuteClose()���Ŏ��s�ς� if( mInhibitClose == true )   return;
	//AskSaveChanges�E�C���h�E�\�����Ȃ牽���������^�[��
	//drop if( mAskingSaveChanges == true )    return;
	
	//���݂̃h�L�������g��Dirty���ǂ������f
	/*drop
	if( GetDocument().NVI_IsDirty() == true )
	{
		//AskSaveChanges�E�C���h�E���J���āA�ۑ����邩�ǂ�����q�˂�
		AText	filename;
		GetDocument().NVI_GetTitle(filename);
		NVI_ShowAskSaveChangesWindow(filename,GetDocument().GetObjectID());
		mAskingSaveChanges = true;
		return;
	}
	else
	*/
	{
		//Close���s
		//#92 ExecuteClose();
		//#1336 NVI_ExecuteClose();//#92
		//#1336 ������hide����
		NVI_Hide();
	}
}

//Close���s
//#92 void	AWindow_FindResult::ExecuteClose()
void	AWindow_FindResult::NVIDO_ExecuteClose()
{
	//#92 NVI_ExecuteClose()���Ŏ��s�ς� if( mInhibitClose == true )   return;
	//�^�u�{����r���[�폜
	NVI_DeleteTabAndView(0);
	//�^�u��0�ɂȂ�����E�C���h�E�폜
	if( NVI_GetTabCount() == 0 )
	{
		NVI_Close();
		GetApp().SPI_CloseSubWindow(GetObjectID());
	}
	
	//�h�L�������g�փE�C���h�E�i�^�u�j������ꂽ���Ƃ�ʒm����
	//#379 AView_Index::NVI_UnregisterView()�ɂ��h�L�������g������邽�߂����͕s�v�B GetDocument().OWICB_DoWindowClosed(GetObjectID());
	
	//�E�C���h�E���j���[�X�V
	//#922 GetApp().SPI_UpdateWindowMenu();
	
}

//�E�C���h�E�T�C�Y�ύX�ʒm���̃R�[���o�b�N
void	AWindow_FindResult::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_FindResult::NVIDO_Create( const ADocumentID inDocumentID )
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
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,false,false,false,false);
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
		text.SetLocalizedText("SubWindowTitle_FindResult");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwSearchList);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//�T�u�E�C���h�E�p�t�H���g�擾
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
	//SizeBox����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
	
	//IndexView����
	AControlID	indexViewControlID = kIndex_Invalid;
	{
		AWindowPoint	pt = {0,0};
		indexViewControlID = NVM_AssignDynamicControlID();//#271
		AIndexViewFactory	indexViewFactory(GetObjectID(),indexViewControlID,mDocumentID);//#92
		mIndexViewID = NVI_CreateView(indexViewControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,true,indexViewFactory);//#92
		NVI_SetControlBindings(indexViewControlID,true,true,true,true,false,false);
	}
	
	//V�X�N���[���o�[����
	{
		AWindowPoint	pt = {0,0};
		mVScrollBarControlID = NVM_AssignDynamicControlID();
		NVI_CreateScrollBar(mVScrollBarControlID,pt,kWidth_VScroll,kWidth_VScroll*2);
		NVI_SetControlBindings(mVScrollBarControlID,false,true,true,true,true,false);
	}
	
	//SubWindow�p�Z�p���[�^����
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,indexViewControlID,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//
	AView_Index::GetIndexViewByViewID(mIndexViewID).NVI_SetScrollBarControlID(kControlID_Invalid,mVScrollBarControlID);
}

void	AWindow_FindResult::NVIDO_Hide()
{
	//�t���[�e�B���O�̏ꍇ�͈ʒu��db�ɐݒ�
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		APoint	pt = {0};
		NVI_GetWindowPosition(pt);
		GetApp().GetAppPref().SetData_Point(AAppPrefDB::kIndexWindowPosition,pt);
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowWidth,NVI_GetWindowWidth());
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowHeight,NVI_GetWindowHeight());
	}
}

//�E�C���h�E�S�̂̕\���X�V
void	AWindow_FindResult::NVIDO_UpdateProperty()
{
	//
	AView_Index::GetIndexViewByViewID(mIndexViewID).SPI_UpdateImageSize();
	AView_Index::GetIndexViewByViewID(mIndexViewID).SPI_UpdateTextDrawProperty();
	//view bounds�X�V
	UpdateViewBounds();
	//
	NVI_RefreshWindow();
}

//�R���g���[����ԁiEnable/Disable���j���X�V
//NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()����R�[�������
//�N���b�N��A�N�e�B�x�[�g�ŃR�[�������̂ŁA���܂�d��������NVMDO_UpdateControlStatus()�ɂ͓���Ȃ����ƁB
void	AWindow_FindResult::NVMDO_UpdateControlStatus()
{
}

//�V�K�^�u����
//�����F��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
void	AWindow_FindResult::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//view bounds�X�V
	UpdateViewBounds();
	//�h�L�������gID���^�u�f�[�^�ɐݒ�iNVI_GetDocumentIDByTabIndex()����mDocumentID���擾�ł���悤�ɂ��邽�߁j#688
	NVM_SetTabDocumentID(inTabIndex,mDocumentID);
	//
	outInitialFocusControlID = AView_Index::GetIndexViewByViewID(mIndexViewID).NVI_GetControlID();
}

void	AWindow_FindResult::UpdateViewBounds()
{
	//�E�C���h�Ebounds�擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//�^�C���o�[�A�Z�p���[�^�����擾
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	
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
	ABool	showBackground = false;
	ABool	showSizeBox = false;
	//list view�T�C�Y
	AWindowPoint	listViewPoint = {leftMargin,0};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;
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
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;// + height_Separator;
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
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;
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
	
	//ListView�z�u�E�\��
	{
		AView_Index::GetIndexViewByViewID(mIndexViewID).NVI_SetPosition(listViewPoint);
		AView_Index::GetIndexViewByViewID(mIndexViewID).NVI_SetSize(listViewWidth - vscrollbarWidth,listViewHeight);
		AView_Index::GetIndexViewByViewID(mIndexViewID).SPI_UpdateImageSize();
	}
	
	//V�X�N���[���o�[�z�u
	if( vscrollbarWidth > 0 /*&& NVI_IsControlEnable(mVScrollBarControlID) == true*/)
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
}

#pragma mark ===========================

#pragma mark ---

void	AWindow_FindResult::SPI_ShowSaveWindow()
{
/*drop
	AText	filename;
	GetDocument().NVI_GetTitle(filename);
	AFileAttribute	attr;
	//win attr.creator = 'MMKE';
	//win attr.type = 'INDX';
	GetApp().SPI_GetIndexFileAttribute(attr);
	AFileAcc	defaultfolder;
	if( GetDocument().NVI_IsFileSpecified() == true )
	{
		AFileAcc	file;
		GetDocument().NVI_GetFile(file);
		defaultfolder.SpecifyParent(file);
	}
	else
	{
		defaultfolder.SpecifyParent(GetApp().SPI_GetLastOpenedTextFile());
	}
	AText	filter;//win 080709
	GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
	NVI_ShowSaveWindow(filename,GetDocument().GetObjectID(),attr,filter,GetEmptyText(),defaultfolder);
*/
}
