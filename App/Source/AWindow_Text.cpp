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

AWindow_Text

*/
/*
AWindow_Text�݌v�|���V�[

�EADocument_Text����̒��ڂ̊֘A�͂Ȃ��B(110323)
�EAApp����ADocumentID�������Ƃ��ăR�[������邱�Ƃ͂���B���̏ꍇ�A�Y������DocumentID������ꍇ�̂ݏ������s���B(110323)




*/

#include "stdafx.h"

#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AView_Text.h"
#include "AView_LineNumber.h"
#include "AView_TabSelector.h"
//#include "AView_Separator.h"
#include "AView_Toolbar.h"
#include "AView_Ruler.h"
#include "AApp.h"
#include "ASCMFolderManager.h"
#include "ADocument_IndexWindow.h"
#include "AView_Index.h"
#include "AView_FindResultSeparator.h"
#include "AWindow_DiffInfo.h"
#include "AView_IdInfo.h"
#include "AView_CandidateList.h"
#include "AView_Footer.h"
#include "AWindow_SubTextShadow.h"
#include "AWindow_Notification.h"
#include "AView_Notification.h"
#include "AView_TextViewHSeparator.h"
#include "Cocoa.h"

//===================�R���g���[��===================

//
const AControlID	kControlID_LineNumberButton 	= 101;
const AControlID	kControlID_PositionButton 		= 102;
//#725 const AControlID	kControlID_HScrollBar 			= 103;
const AControlID	kControlID_TabSelector			= 104;
//#246 const AControlID	kControlID_ToolbarPlacard		= 105;
//#725 const AControlID	kControlID_InfoHeader			= 106;
//#246 const AControlID	kControlID_Ruler				= 107;
const AControlID	kControlID_SizeBox				= 108;//win 080711
const AControlID	kSubPaneModeButtonControlID		= 110;//#212
const AControlID	kSubPaneFileNameButtonControlID	= 111;//#212
const AControlID	kSubPaneSyncButtonControlID		= 112;//#212
const AControlID	kSubPaneSwapButtonControlID		= 113;//#212
const AControlID	kSubPaneDiffButtonControlID		= 114;//#379
//#725 const AControlID	kControlID_FindResultInfoPaneBackground	= 119;//#634
const AControlID	kControlID_LineNumberButton_SubText 	= 122;//#725
const AControlID	kControlID_PositionButton_SubText	 	= 123;//#725

//�E�T�C�h�o�[�̐���{�^��
const AControlID	kControlID_RightSideBar_SeparatorButton= 116;//#634
const AControlID	kControlID_RightSideBar_PrefButton		= 117;//#634
const AControlID	kControlID_RightSideBar_ShowHideButton	= 118;//#634
//���T�C�h�o�[�̐���{�^��
const AControlID	kControlID_LeftSideBar_SeparatorButton	= 124;//#725
const AControlID	kControlID_LeftSideBar_PrefButton		= 125;//#725
const AControlID	kControlID_LeftSideBar_ShowHideButton	= 126;//#725

//�T�C�h�o�[����{�^���T�C�Y
//const ANumber	kWidth_LeftSideBarSeparatorButton = 32;//#725
const ANumber	kWidth_RightSideBar_PrefButton = 50;//#725
const ANumber	kWidth_RightSideBar_ShowHideButton = 32;//#725
const ANumber	kWidth_LeftSideBar_PrefButton = 50;//#725
const ANumber	kWidth_LeftSideBar_ShowHideButton = 32;//#725

//�t�H���g�p�l���ł̕ύX����virtual control ID
const AControlID	kFontPanelVirtualControlID = 127;//#688

//virtual contorl ID
const AControlID	kVirtualControlID_AskSaveChangesReply 	= 200;
const AControlID	kVirtualControlID_SaveWindowReply 		= 201;
const AControlID	kVirtualControlID_ToolRemoveOK 			= 202;
const AControlID	kVirtualControlID_FindResultCloseButon	= 203;//win
const AControlID	kVirtualControlID_DeleteMacrosAfterThis	= 204;

//
//�w�b�_�t�@�C���Œ�` const AControlID	kVirtualControlID_CompareFileChoosen 	= 210;

//���E�T�C�h�o�[�w�i
const AControlID	kControlID_RightSideBarBackground		= 301;
const AControlID	kControlID_LeftSideBarBackground		= 302;
//�E�C���h�E�S�̔w�i
const AControlID	kControlID_WindowBackground				= 303;

//�t�b�^�[
const AControlID	kControlID_Footer = 351;
const ANumber		kHeight_Footer = 20;
/*
const AControlID	kControlID_FooterSeparator_MainColumn = 352;
const AControlID	kControlID_FooterSeparator_MainColumn2 = 353;
const AControlID	kControlID_FooterSeparator_SubTextColumn = 354;
const AControlID	kControlID_FooterSeparator_SubTextColumn2 = 355;
const ANumber		kWidth_FooterSeparator = 5;
*/

//#725
//�T�C�Y�{�b�N�X
const ANumber		kWidth_SizeBox = 24;

//#725
//�v���O���X�\��
const AControlID	kControlID_MainColumnProgressIndicator = 402;
const ANumber	kHeight_ProgressIndicator = 16;
const ANumber	kWidth_ProgressIndicator = 16;
const ANumber	kTopMargin_ProgressIndicator = 2;
const ANumber	kRightMargin_ProgressText = 4;
const AControlID	kControlID_MainColumnProgressText = 403;
const ANumber	kImportProgressRightMargin = 20;

//#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kButtonFontSize = 10.0;
const AFloatNumber	kSearchBoxFontSize = 10.0;
/*
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AFloatNumber	kButtonFontSize = 8.0;
const AFloatNumber	kSearchBoxFontSize = 8.0;
#endif
*/

//���C�A�E�g�p�����[�^
static const ANumber	kHScrollBarHeight = 15;
static const ANumber	kVScrollBarWidth = 15;
static const ANumber	kLineNumberButtonWidth = 120;
static const ANumber	kPositionButtonWidth = 250;
//#450 static const ANumber	kLeftMarginWidthWithNoEachLineNumber = 8;
//#450 static const ANumber	kLeftMarginWidthWidthEachLineNumber = 48;
static const ANumber	kToolbarHeight = 38;//32;
//#725 static const ANumber	kInfoHeaderHeight = 18;
static const ANumber	kRulerHeight = 20;//15;
static const ANumber	kSplitButtonHeight = 15;
static const ANumber	kSeparatorHeight = 7;
static const ANumber	kSeparatorWidth = 6;//#212 #688 
static const ANumber	kInfoPaneSeparatorHeight = 3;//#219
static const ANumber	kSubPaneSeparatorHeight = 3;//#212
/*#724
static const ANumber	kSmallToolButtonWidth = 16;
static const ANumber	kSmallToolButtonHeight = 16;
static const ANumber	kToolButtonWidth = 24;
static const ANumber	kToolButtonHeight = 24;
static const ANumber	kToolButtonMarginWidth = 2;
static const ANumber	kToolButtonUpMargin = 4;
*/
static const ANumber	kMacroBar_LeftMargin = 10;//#725
static const ANumber	kMacroBar_RightMargin = 6;//#725
static const ANumber	kMacroBar_TopMargin = 0;//#725
static const ANumber	kMacroBar_BottomMargin = 2;//#725
static const ANumber	kMarginHeight_MacroBarItem = 1;//#724

static const ANumber	kInfoPaneColumnControlButtonWidth = 32;//#725

static const ANumber	k_wLeftSideBarVisibleArea = 0;//#725
static const ANumber	k_wRightSideBarVisibleArea = 0;//#725

//�E�T�C�h�o�[����
const ANumber	kHeight_RightSidebarControlButtons = 20;

//#724
const AItemCount	kFixMacroItemCount = 2;

//#725
//�T�u�e�L�X�g�p�V���h�E�\��
const ANumber	kWidth_SubTextShadow = 22;

//�T�u�e�L�X�g�㕔�{�^���T�C�Y
static const ANumber				kSubPaneModeButtonHeight = 20;//18;
static const ANumber				kSubPaneSyncButtonWidth = 70;
static const ANumber				kSubPaneSwapButtonWidth = 70;
static const ANumber				kSubPaneDiffButtonWidth = 90;//#379

//===================���E�����ŏ��l===================

//�T�C�h�o�[�ŏ���
const ANumber	kMinWidth_SideBar = AApp::kSubWindowMinWidth;

//�T�u�e�L�X�g�ŏ���
const ANumber	kMinWidth_SubTextColumn = kSubPaneSyncButtonWidth*4;

//���C���J�����ŏ����E����
const ANumber	kMinWidth_MainColumn =120;
const ANumber	kMinHeight_MainColumn = 50;


//�ҏW���]��ON�̂Ƃ��́A�C���[�W�T�C�Y�̗]��
const ANumber	kImageYBottomMarginForNormal = 32;
const ANumber	kImageYBottomMarginFor25PercentMargin = 1000;//������

#pragma mark ===========================
#pragma mark [�N���X]AWindow_Text
#pragma mark ===========================
//�e�L�X�g�E�C���h�E�̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_Text::AWindow_Text(/*#175 AObjectArrayItem* inParent */)
: AWindow(/*#175inParent*/), 
		/*#199 mJumpListWindow(*this), */mAskingSaveChanges(false), /*#199 mMoveLineWindow(*this), mFontSizeWindow(*this),*///, mInfoWindow(this),
		/*#199 mWrapLetterCountWindow(*this), mTextEncodingCorrectWindow(*this), mPrintOptionWindow(*this), mDocPropertyWindow(*this),
		mAddToolButtonWindow(*this),*//* mQuitting(false), mAllClosing(false), *//* #199 mDropWarningWindow(*this), mEditByOtherAppWindow(*this),*/
		mJumpMenuItemIndex(kIndex_Invalid)
		//#138, mExternalCommentWindow(*this)//RC3
//#725		,mFindResultDocumentID(kObjectID_Invalid)//#92
//#725		,mFindResultViewID(kObjectID_Invalid)//#92
//#725		,mFindResultVScrollBarControlID(kControlID_Invalid)//#92
		//win ,mFindResultCloseButtonControlID(kControlID_Invalid)//#92
//#725		,mFindResultCloseButtonWindowID(kObjectID_Invalid)//win
//#725		,mFindResultDisplayPosition(kDisplayPosition_MiddleBottom), mFindResultHeight(100)//#92
//#899 ,mSubPaneMode(kSubPaneMode_PreviousDocument)//#212
		,mLeftSideBarDisplayed(false), mLeftSideBarWidth(500), mSubTextCurrentDocumentID(kObjectID_Invalid), mPrevLeftSideBarWidth(-1)//#212
//#725		,mPaneDisplayMode_LeftWideHeader(true)//#212
//#725		,mPaneDisplayMode_RightWideHeader(true)//#291
//#634,mPaneDisplayMode_OverlayInfoPane(true)//#291
		,mRightSideBarDisplayed(false), mRightSideBarWidth(300), mPrevRightSideBarWidth(-1)//#291
//#850		,mTabModeMainWindow(false)//#209
		,mRightSideBarVSeparatorWindowID(kObjectID_Invalid)//#291
		,mLeftSideBarVSeparatorWindowID(kObjectID_Invalid)//#291
		,mSubTextColumnVSeparatorWindowID(kObjectID_Invalid)//#725
		,mDiffDisplayMode(false)//#379
		,mDiffInfoWindowID(kObjectID_Invalid)//#379
,mLuaConsoleMode(false)//#567
,mSuppressUpdateViewBoundsCounter(0)//#675
,mFloatingJumpListWindowID(kObjectID_Invalid)//#725
,mLayoutMode(kLayoutMode_4Pane_Header11111111)//kLayoutMode_4Pane_Header11110110)//#725
,mSubTextColumnWidth(500),mSubTextColumnDisplayed(true)//#725
,mPopupIdInfoWindowID(kObjectID_Invalid)//#853
,mPopupCandidateWindowID(kObjectID_Invalid)//#717
,mPopupKeyToolListWindowID(kObjectID_Invalid)//#725
//drop,mIsMainSubDiffMode(false),mMainSubDiff_SubTextControlID(kControlID_Invalid),mMainSubDiff_MainTextControlID(kControlID_Invalid)//#737
//drop,mMainSubDiff_DiffRangeMainStartParagraphIndex(kIndex_Invalid),mMainSubDiff_DiffRangeMainEndParagraphIndex(kIndex_Invalid)//#737
//drop,mMainSubDiff_DiffRangeSubStartParagraphIndex(kIndex_Invalid),mMainSubDiff_DiffRangeSubEndParagraphIndex(kIndex_Invalid)//#737
,mFooterProgressRightPosition(0)//#725
,mDoingSetWindowBoundsAnimate(false)//#725
,mSubTextShadowWindowID(kObjectID_Invalid)//#725
,mDraggingMacroBarCurrentItemDisplayIndex(kIndex_Invalid)//#724
,mCurrentPopupLayoutPattern(kTextWindowPopupLayoutPatternA)//#725
,mFooterProgress_ShowImportFileProgress(false),mFotterProgress_ImportFileProgressRemainCount(0)//#725
,mRightSideBarCreated(false),mLeftSideBarCreated(false)//#725
,mLastNormalSubTextDocumentID(kObjectID_Invalid)//#725
,mLatentTextViewContolID(kControlID_Invalid)
//#1091-test ,mDefferedFocusTick(0),mDefferedFocus(kControlID_Invalid)
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AWindow_Text::AWindow_Text() started.",this);
	//#138 �f�t�H���g�͈�(10000�`)���g������ݒ�s�v NVM_SetDynamicControlIDRange(kDynamicControlIDStart,kDynamicControlIDEnd);
	//#175
	NVI_AddToRotateArray();
	//
	NVI_AddToTimerActionWindowArray();
	//#199
	NVI_AddToAllCloseTargetWindowArray();
	//#986
	NVI_SetCloseByEscapeKey(false);
	
	/*#725
	//JumpListWindow #199
	AChildWindowFactory<AWindow_JumpList>	jumpListWindowFactory(GetObjectID());
	mJumpListWindowID = GetApp().NVI_CreateWindow(jumpListWindowFactory);
	*/
	
	//MoveLineWindow #199
	AChildWindowFactory<AWindow_TextSheet_MoveLine>	moveLineWindowFactory(GetObjectID());
	mMoveLineWindowID = GetApp().NVI_CreateWindow(moveLineWindowFactory);
	NVI_RegisterChildWindow(mMoveLineWindowID);//#694
	
	/*#844
	//FontSizeWindow #199
	AChildWindowFactory<AWindow_TextSheet_FontSize>	fontSizeWindowFactory(GetObjectID());
	mFontSizeWindowID = GetApp().NVI_CreateWindow(fontSizeWindowFactory);
	NVI_RegisterChildWindow(mFontSizeWindowID);//#694
	*/
	//WrapLetterCountWindow #199
	AChildWindowFactory<AWindow_TextSheet_WrapLetterCount>	wrapLetterCountWindowFactory(GetObjectID());
	mWrapLetterCountWindowID = GetApp().NVI_CreateWindow(wrapLetterCountWindowFactory);
	NVI_RegisterChildWindow(mWrapLetterCountWindowID);//#694
	
	//TextEncodingCorrectWindow #199
	AChildWindowFactory<AWindow_TextSheet_CorrectEncoding>	textEncodingCorrectWindowFactory(GetObjectID());
	mTextEncodingCorrectWindowID = GetApp().NVI_CreateWindow(textEncodingCorrectWindowFactory);
	NVI_RegisterChildWindow(mTextEncodingCorrectWindowID);//#694
	
	/*#844
	//PrintOptionWindow #199
	AChildWindowFactory<AWindow_TextSheet_PrintOption>	printOptionWindowFactory(GetObjectID());
	mPrintOptionWindowID = GetApp().NVI_CreateWindow(printOptionWindowFactory);
	NVI_RegisterChildWindow(mPrintOptionWindowID);//#694
	
	//PropertyWindow #199
	AChildWindowFactory<AWindow_TextSheet_Property>	propertyWindowFactory(GetObjectID());
	mPropertyWindowID = GetApp().NVI_CreateWindow(propertyWindowFactory);
	NVI_RegisterChildWindow(mPropertyWindowID);//#694
	*/
	//DropWarningWindow #199
	AChildWindowFactory<AWindow_TextSheet_DropWarning>	dropWarningWindowFactory(GetObjectID());
	mDropWarningWindowID = GetApp().NVI_CreateWindow(dropWarningWindowFactory);
	NVI_RegisterChildWindow(mDropWarningWindowID);//#694
	
	//AddToolButtonWindow #199
	AChildWindowFactory<AWindow_AddToolButton>	addToolButtonFactory(GetObjectID());
	mAddToolButtonWindowID = GetApp().NVI_CreateWindow(addToolButtonFactory);
	NVI_RegisterChildWindow(mAddToolButtonWindowID);//#694
	
	//EditByOtherAppWindow #199
	AChildWindowFactory<AWindow_TextSheet_EditByOtherApp>	editByOtherAppWindowFactory(GetObjectID());
	mEditByOtherAppWindowID = GetApp().NVI_CreateWindow(editByOtherAppWindowFactory);
	NVI_RegisterChildWindow(mEditByOtherAppWindowID);//#694
	
	//SCMCommitWindow #455
	AChildWindowFactory<AWindow_TextSheet_SCMCommit>	scmCommitWindowFactory(GetObjectID());
	mSCMCommitWindowID = GetApp().NVI_CreateWindow(scmCommitWindowFactory);
	NVI_RegisterChildWindow(mSCMCommitWindowID);//#694
	
	if( GetApp().SPI_IsSupportPaneMode() == true )
	{
		//#291 �E�T�C�h�o�[VSeparator
		AWindowDefaultFactory<AWindow_VSeparatorOverlay>	infoPaneVSeparatorFactory;
		mRightSideBarVSeparatorWindowID = GetApp().NVI_CreateWindow(infoPaneVSeparatorFactory);
		
		//#291 �T�u�y�C��VSeparator
		AWindowDefaultFactory<AWindow_VSeparatorOverlay>	subPaneVSeparatorFactory;
		mLeftSideBarVSeparatorWindowID = GetApp().NVI_CreateWindow(subPaneVSeparatorFactory);
		
		//#725 �T�u�e�L�X�g�J����VSeparator
		AWindowDefaultFactory<AWindow_VSeparatorOverlay>	subTextPaneColumnVSeparatorFactory;
		mSubTextColumnVSeparatorWindowID = GetApp().NVI_CreateWindow(subTextPaneColumnVSeparatorFactory);
		
		//#212 �T�u�y�C����ԓǂݍ���
		mLeftSideBarDisplayed = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kLeftSideBarDisplayed);
		mLeftSideBarWidth = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kLeftSideBarWidth);
		//#899 mSubPaneMode = static_cast<ASubPaneMode>(GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kSubPaneMode));
		//#668 mLeftSideBarDisplayed��NVIDO_Create()�Őݒ肷��
		
		//#291 �E�T�C�h�o�[��ԓǂݍ���
		mRightSideBarDisplayed = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kRightSideBarDisplayed);
		mRightSideBarWidth = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kRightSideBarWidth);
	}
	//�T�u�e�L�X�g�J����
	mSubTextColumnDisplayed = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSubTextColumnDisplayed);
	mSubTextColumnWidth = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kSubTextColumnWidth);
}

//�f�X�g���N�^
AWindow_Text::~AWindow_Text()
{
	// #92 NVIDO_DoDeleted�ֈړ�
}

//#92
/**
*/
void	AWindow_Text::NVIDO_DoDeleted()
{
	//�|�b�v�A�b�v�E�C���h�E����� #717
	if( mPopupIdInfoWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_DeleteSubWindow(mPopupIdInfoWindowID);
	}
	if( mPopupCandidateWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_DeleteSubWindow(mPopupCandidateWindowID);
	}
	if( mPopupKeyToolListWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_DeleteSubWindow(mPopupKeyToolListWindowID);
	}
	
	//#199mJumpListWindow.NVI_Close();
	//�t���[�e�B���O�W�����v���X�g�폜
	//#725 SPI_GetJumpListWindow().NVI_Close();
	/*#725 ExecuteClose()�Ŏ��s
	if( mFloatingJumpListWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_DeleteSubWindow(mFloatingJumpListWindowID);
		mFloatingJumpListWindowID = kObjectID_Invalid;
	}
	*/
	//mInfoWindow.NVI_Close();
	//#199
	//#725 GetApp().NVI_DeleteWindow(mJumpListWindowID);
	GetApp().NVI_DeleteWindow(mMoveLineWindowID);
	NVI_UnregisterChildWindow(mMoveLineWindowID);//#694
	//#844 GetApp().NVI_DeleteWindow(mFontSizeWindowID);
	//#844 NVI_UnregisterChildWindow(mFontSizeWindowID);//#694
	GetApp().NVI_DeleteWindow(mWrapLetterCountWindowID);
	NVI_UnregisterChildWindow(mWrapLetterCountWindowID);//#694
	GetApp().NVI_DeleteWindow(mTextEncodingCorrectWindowID);
	NVI_UnregisterChildWindow(mTextEncodingCorrectWindowID);//#694
	//#844 GetApp().NVI_DeleteWindow(mPrintOptionWindowID);
	//#844 NVI_UnregisterChildWindow(mPrintOptionWindowID);//#694
	//#844 GetApp().NVI_DeleteWindow(mPropertyWindowID);
	//#844 NVI_UnregisterChildWindow(mPropertyWindowID);//#694
	GetApp().NVI_DeleteWindow(mDropWarningWindowID);
	NVI_UnregisterChildWindow(mDropWarningWindowID);//#694
	GetApp().NVI_DeleteWindow(mAddToolButtonWindowID);
	NVI_UnregisterChildWindow(mAddToolButtonWindowID);//#694
	GetApp().NVI_DeleteWindow(mEditByOtherAppWindowID);
	NVI_UnregisterChildWindow(mEditByOtherAppWindowID);//#694
	GetApp().NVI_DeleteWindow(mSCMCommitWindowID);
	NVI_UnregisterChildWindow(mSCMCommitWindowID);//#694
	GetApp().NVI_DeleteWindow(mDiffInfoWindowID);//#379
	GetApp().NVI_DeleteWindow(mSubTextShadowWindowID);//#725
	//#291
	//�e�������E�C���h�E�폜
	if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteWindow(mRightSideBarVSeparatorWindowID);
	}
	if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteWindow(mLeftSideBarVSeparatorWindowID);
	}
	//#725
	if( mSubTextColumnVSeparatorWindowID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteWindow(mSubTextColumnVSeparatorWindowID);
	}
	/*#725 ExecuteClose()����DeleteRightSideBar(),DeleteLeftSideBar()�o�R�ō폜�����
	DeleteInfoPaneArray();//#291
	DeleteSubPaneArray();//#212
	*/
	
#if IMPLEMENTATION_FOR_WINDOWS
	//Windows�̏ꍇ�A�Ō�̂P��TextWindow���폜�����Ƃ��ɃA�v�����I�����邽�߂̓����C�x���g�𑗐M
	//���ڏI������ƁAApp�I�u�W�F�N�g�̍폜��Window������d�ɂ���Ă��܂����肷���肪����
	if( GetApp().NVI_GetWindowCount(kWindowType_Text) == 1 )
	{
		ABase::PostToMainInternalEventQueue(kInternalEvent_Quit,GetObjectID(),0,GetEmptyText(),GetEmptyObjectIDArray());
	}
#endif
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//TextDocument�擾�i���݂�Tab�j
ADocument_Text&	AWindow_Text::SPI_GetCurrentTabTextDocument()
{
	return GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID());
}
const ADocument_Text&	AWindow_Text::SPI_GetCurrentTabTextDocumentConst() const
{
	return GetApp().SPI_GetTextDocumentConstByID(NVI_GetCurrentDocumentID());
}

//TextDocument�擾�i�w��TextView��ControlID����j#212
ADocument_Text&	AWindow_Text::SPI_GetTextDocument( const AControlID inTextViewControlID )
{
	return GetTextViewByControlID(inTextViewControlID).SPI_GetTextDocument();
}
const ADocument_Text&	AWindow_Text::SPI_GetTextDocumentConst( const AControlID inTextViewControlID ) const
{
	return GetTextViewConstByControlID(inTextViewControlID).SPI_GetTextDocumentConst();
}

//TextDocument�擾�i���݂�Focus�j
ADocument_Text&	AWindow_Text::SPI_GetCurrentFocusTextDocument()
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		return GetTextViewByControlID(focus).SPI_GetTextDocument();
	}
	else
	{
		return SPI_GetCurrentTabTextDocument();
	}
}

//ModePrefDB�擾
AModePrefDB&	AWindow_Text::GetCurrentTabModePrefDB()
{
	return GetApp().SPI_GetModePrefDB(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex());
}

//TextView�擾
AView_Text&	AWindow_Text::GetTextViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Text,kViewType_Text,inControlID);
	/*#199 
	if( NVI_GetViewByControlID(inControlID).NVI_GetViewType() != kViewType_Text )   _ACThrow("not text view",this);
	return dynamic_cast<AView_Text&>(NVI_GetViewByControlID(inControlID));
	*/
}
const AView_Text&	AWindow_Text::GetTextViewConstByControlID( const AControlID inControlID ) const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_Text,kViewType_Text,inControlID);
	/*#199 
	if( NVI_GetViewConstByControlID(inControlID).NVI_GetViewType() != kViewType_Text )   _ACThrow("not text view",this);
	return dynamic_cast<const AView_Text&>(NVI_GetViewConstByControlID(inControlID));
	*/
}

//LineNumberView�擾
AView_LineNumber&	AWindow_Text::GetLineNumberViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_LineNumber,kViewType_LineNumber,inControlID);
}
const AView_LineNumber&	AWindow_Text::GetLineNumberViewConstByControlID( const AControlID inControlID ) const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_LineNumber,kViewType_LineNumber,inControlID);
}

//TabSelector�擾
AView_TabSelector&	AWindow_Text::GetTabSelector()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_TabSelector,kViewType_TabSelector,kControlID_TabSelector);
	/*#199
	return dynamic_cast<AView_TabSelector&>(NVI_GetViewByControlID(kControlID_TabSelector));
	*/
}
const AView_TabSelector&	AWindow_Text::GetTabSelectorConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_TabSelector,kViewType_TabSelector,kControlID_TabSelector);
}

//#379
/**
���݂̃^�u�̏c�����r���[�̐����擾
*/
AItemCount	AWindow_Text::SPI_GetCurrentViewCount() const
{
	return mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewCount();
}

//#379
/**
���݂̃^�u��LineNumber�r���[���擾
*/
AView_LineNumber&	AWindow_Text::SPI_GetCurrentLineNumberView( const AIndex inViewIndex )
{
	AControlID	controlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLineNumberViewControlID(inViewIndex);
	return GetLineNumberViewByControlID(controlID);
}

/**
�w��^�u��LineNumber�r���[���擾
*/
const AView_LineNumber&	AWindow_Text::SPI_GetLineNumberViewConst( const AIndex inTabIndex, const AIndex inViewIndex ) const
{
	AControlID	controlID = mTabDataArray.GetObjectConst(inTabIndex).GetLineNumberViewControlID(inViewIndex);
	return GetLineNumberViewConstByControlID(controlID);
}

//#853
/**
���݂̃t�H�[�J�X��text view���擾
@note �T�u�e�L�X�g�y�C�����܂ށB
*/
AView_Text&	AWindow_Text::SPI_GetCurrentFocusTextView()
{
	//�i�T�u�e�L�X�g���܂߁j���݂�focus��text view���擾
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus == kControlID_Invalid )
	{
		focus = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0);
	}
	return GetTextViewByControlID(focus);
}

/**
�w��document ID�ɑΉ�����A���C���J�����̍ŏ��̕����e�L�X�g�r���[���擾
*/
AView_Text& AWindow_Text::SPI_GetMainTextViewForDocumentID( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex == kIndex_Invalid ) {_ACThrow("",NULL);};
	
	return GetTextViewByControlID(mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(0));
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//
ABool	AWindow_Text::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
#if 0
	//#92
	if( inID == /*win mFindResultCloseButtonControlID*/kVirtualControlID_FindResultCloseButon )
	{
		//�}���`�t�@�C���������Ȃ�Close�s��
		if( GetApp().SPI_IsMultiFileFindWorking() == true )   return true;
		//
		DeleteFindResultDocument();
		return true;
	}
#endif
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	/*#724
	//�����{�b�N�X�����{�^�� #137
	if( inID == mTabDataArray.GetObject(tabIndex).GetSearchButtonControlID() )
	{
		if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == false )//#268 Command�������Ȃ���̏ꍇ�͑O�����ɂ���
		{
			FindNextWithSearchBox();
		}
		else
		{
			FindPreviousWithSearchBox();
		}
	}
	*/
	//�r���[�����{�^��
	AIndex	splitButtonIndex = mTabDataArray.GetObject(tabIndex).FindFromSplitButtonControlID(inID);
	if( splitButtonIndex != kIndex_Invalid )
	{
		SPI_SplitView(tabIndex,splitButtonIndex,false);
		return true;
	}
	//�r���[�����{�^��
	AIndex	concatButtonIndex = mTabDataArray.GetObject(tabIndex).FindFromConcatButtonControlID(inID);
	if( concatButtonIndex != kIndex_Invalid )
	{
		SPI_ConcatSplitView(concatButtonIndex);
		return true;
	}
	//�c�[���{�^��
	AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(inID);
	if( toolButtonIndex != kIndex_Invalid )
	{
		ABool	result = true;
		switch( mTabDataArray.GetObjectConst(tabIndex).GetToolButtonType(toolButtonIndex) )
		{
		  case kToolbarItemType_File:
			{
				AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
				if( focus != kControlID_Invalid )
				{
					GetTextViewByControlID(focus).SPI_DoTool(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonFile(toolButtonIndex),inModifierKeys,false);
				}
				break;
			}
			/*#232
		  case kToolbarItemType_Folder:
			{
				AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
				if( focus != kControlID_Invalid )
				{
					if( NVI_GetControlNumber(inID) != kIndex_Invalid )
					{
						AIndex	toolButtonDBIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(toolButtonIndex);
						AText	filename;
						GetApp().SPI_GetModePrefDB(GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetModeIndex()).
							GetToolbarItemFilenameArray(toolButtonDBIndex).Get(NVI_GetControlNumber(inID),filename);
						AFileAcc	file;
						file.SpecifyChild(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonFile(toolButtonIndex),filename);
						GetTextViewByControlID(focus).SPI_DoTool(file,inModifierKeys,false);
					}
				}
				break;
			}
			*/
		  case kToolbarItemType_ModePref:
			{
				GetApp().SPI_ShowModePrefWindow(GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetModeIndex());
				break;
			}
		  case kToolbarItemType_AddToolButton:
			{
				SPI_ShowAddToolButtonWindow();
				break;
			}
			/*#232 EVT_DoMenuItemSelected()�Ŏ��s�����
		  case kToolbarItemType_TextEncoding:
			{
				AText	tecname;
				NVI_GetControlText(inID,tecname);
				//B0343
				if( ATextEncodingWrapper::CheckTextEncodingAvailability(tecname) == false )
				{
					tecname.SetCstring("UTF-8");
					NVI_SetControlText(inID,tecname);
				}
				GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetTextEncoding(tecname);
				break;
			}
		  case kToolbarItemType_ReturnCode:
			{
				GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetReturnCode(static_cast<AReturnCode>(NVI_GetControlNumber(inID)));
				break;
			}
		  case kToolbarItemType_WrapMode:
			{
				AWrapMode	wrapMode = static_cast<AWrapMode>(NVI_GetControlNumber(inID));
				switch(wrapMode)
				{
				  case kWrapMode_NoWrap:
					{
						GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetWrapMode(wrapMode,0);
						break;
					}
				  case kWrapMode_WordWrap:
					{
						GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetWrapMode(wrapMode,0);
						break;
					}
				  case kWrapMode_WordWrapByLetterCount:
					{
						SPI_ShowWrapLetterCountWindow();
						break;
					}
				}
				break;
			}
		  case kToolbarItemType_Mode:
			{
				AText	modename;
				NVI_GetControlText(inID,modename);
				GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetMode(modename);
				break;
			}
			*/
		  default:
			{
				result = false;
				break;
			}
		}
		return result;
	}
	/*#164
	//R0228
	if( inID == GetTabSelector().SPI_GetTabCloseButtonControlID() )
	{
		AIndex	tabIndex = GetTabSelector().SPI_GetCurrentCloseButtonIndex();
		if( tabIndex != kIndex_Invalid )
		{
			TryClose(tabIndex);
		}
		return true;
	}
	*/
	//
	ABool	result = true;
	switch(inID)
	{
		//#724
		//�c�[���o�[���ځ@�e�L�X�g�G���R�[�f�B���O�I��������
	  case kControlID_Toolbar_TextEncoding:
		{
			AText	tecname;
			NVI_GetControlText(kControlID_Toolbar_TextEncoding,tecname);
			GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetTextEncoding(tecname);
			break;
		}
		//�c�[���o�[���ځ@���s�R�[�h�I��������
	  case kControlID_Toolbar_ReturnCode:
		{
			GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).
					SPI_SetReturnCode((AReturnCode)NVI_GetControlNumber(kControlID_Toolbar_ReturnCode));
			break;
		}
		//�c�[���o�[���ځ@�܂�Ԃ��I��������
	  case kControlID_Toolbar_WrapMode:
		{
			AWrapMode	wrapMode = (AWrapMode)NVI_GetControlNumber(kControlID_Toolbar_WrapMode);
			switch(wrapMode)
			{
			  case kWrapMode_NoWrap:
			  case kWrapMode_WordWrap:
			  case kWrapMode_LetterWrap://#1113
				{
					GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetWrapMode(wrapMode,0);
					break;
				}
			  case kWrapMode_WordWrapByLetterCount:
			  case kWrapMode_LetterWrapByLetterCount://#1113
				{
					SPI_ShowWrapLetterCountWindow(wrapMode);//#1113
					break;
				}
			}
			break;
		}
		//�c�[���o�[���ځ@���[�h�I������
	  case kControlID_Toolbar_Mode:
		{
			AText	modename;
			NVI_GetControlText(kControlID_Toolbar_Mode,modename);
			GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetModeByRawName(modename);
			break;
		}
		//�c�[���o�[���ځ@�V�K�h�L�������g�I��������
	  case kControlID_Toolbar_NewDocument:
		{
			AText	modename;
			NVI_GetControlText(kControlID_Toolbar_NewDocument,modename);
			AModeIndex modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modename);
			if( modeIndex == kIndex_Invalid )   {_ACError("",this); break;}
			GetApp().SPNVI_CreateNewTextDocument(modeIndex);
			//�`�F�b�N�}�[�N����
			NVI_SetControlNumber(kControlID_Toolbar_NewDocument,kIndex_Invalid);
			break;
		}
		//�c�[���o�[���ځ@�ݒ�
	  case kControlID_Toolbar_Pref:
		{
			AText	modename;
			NVI_GetControlText(kControlID_Toolbar_Pref,modename);
			AModeIndex modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modename);
			if( modeIndex == kIndex_Invalid )
			{
				GetApp().SPI_ShowAppPrefWindow();
			}
			else
			{
				GetApp().SPI_ShowModePrefWindow(modeIndex);
			}
			break;
		}
		//�c�[���o�[���ځ@�e�L�X�g�}�[�J�[�I��������
	  case kControlID_Toolbar_FindHighlight:
		{
			AIndex	textMarkerIndex = NVI_GetControlNumber(kControlID_Toolbar_FindHighlight);
			if( textMarkerIndex >= GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kTextMarkerArray_Title) )
			{
				//�ݒ��ʂ��J��
				GetApp().SPI_SelectOrCreateTextMarkerWindow();
			}
			else
			{
				//�O���[�v��I��
				GetApp().SPI_SetCurrentTextMarkerGroupIndex(textMarkerIndex);
			}
			break;
		}
		//�i�r�{�^����
	  case kControlID_Toolbar_GoPrevious:
		{
			GetApp().SPI_NavigatePrev();
			break;
		}
		//�i�r�{�^���E
	  case kControlID_Toolbar_GoNext:
		{
			GetApp().SPI_NavigateNext();
			break;
		}
		//�c�[���o�[���ځ@�L�[�L�^�J�n
	  case kControlID_Toolbar_KeyRecordStart:
		{
			//�L�[�L�^�J�n
			GetApp().SPI_StartRecord();
			break;
		}
		//�c�[���o�[���ځ@�L�[�L�^��~
	  case kControlID_Toolbar_KeyRecordStop:
		{
			//�L�[�L�^��~
			GetApp().SPI_StopRecord();
			//�L�[�L�^��~�{�^���̃g�O��ON������
			NVI_SetControlBool(kControlID_Toolbar_KeyRecordStop,false);
			break;
		}
		//�c�[���o�[���ځ@�L�[�Đ�
	  case kControlID_Toolbar_KeyRecordPlay:
		{
			//�L�[�Đ�
			SPI_GetCurrentFocusTextView().SPI_PlayKeyRecord();
			//�L�[�Đ��{�^���̃g�O��ON������
			NVI_SetControlBool(kControlID_Toolbar_KeyRecordPlay,false);
			break;
		}
		//�c�[���o�[���ځ@�L�[�L�^���}�N���ɒǉ�
	  case kControlID_Toolbar_AddMacroBar:
		{
			//�L�[���}�N���ɒǉ�
			SPI_ShowAddToolButtonWindowFromKeyRecord();
			//�}�N���ǉ��{�^���̃g�O��ON������
			NVI_SetControlBool(kControlID_Toolbar_AddMacroBar,false);
			break;
		}
		//�c�[���o�[���ځ@�|�b�v�A�b�v�֎~
	  case kControlID_Toolbar_ProhibitPopup:
		{
			//�|�b�v�A�b�v�֎~��Ԏ擾�A�ݒ�
			GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kProhibitPopup,
													 !GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup));
			//�֎~�ɂ����Ƃ��́A���ݕ\�����̃|�b�v�A�b�v��S�ĕ���
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup) == true )
			{
				SPI_HideAllPopupWindows();
			}
			//�S�ẴE�C���h�E�̃c�[���o�[�l�X�V
			GetApp().SPI_UpdateToolbarItemValueAll();
			break;
		}
		/*
		//�c�[���o�[���ځ@���[�h�ݒ�
	  case kControlID_Toolbar_ModePref:
		{
			GetApp().SPI_ShowModePrefWindow(GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetModeIndex());
			break;
		}
		//�c�[���o�[���ځ@���ݒ�
	  case kControlID_Toolbar_AppPref:
		{
			GetApp().SPI_ShowAppPrefWindow();
			break;
		}
		*/
		//�c�[���o�[���ځ@���T�C�h�o�[�\����\��
	  case kControlID_Toolbar_LeftSideBar:
		{
			/*test
			SPI_GetCurrentTabTextDocument().TestGuessTextEncoding();
			break;
			*/
			ShowHideLeftSideBar(!mLeftSideBarDisplayed);
			break;
		}
		//�c�[���o�[���ځ@�E�T�C�h�o�[�\����\��
	  case kControlID_Toolbar_RightSideBar:
		{
			ShowHideRightSideBar(!mRightSideBarDisplayed);
			break;
		}
		//�c�[���o�[���ځ@�T�u�e�L�X�g�\����\��
	  case kControlID_Toolbar_2Screens:
		{
			ShowHideSubTextColumn(!mSubTextColumnDisplayed);
			break;
		}
		//�c�[���o�[���ځ@�r���[�c����
	  case kControlID_Toolbar_SplitView:
		{
			SPI_SplitView(false);
			break;
		}
		//�c�[���o�[���ځ@�R�~�b�g
	  case kControlID_Toolbar_Commit:
		{
			SPI_ShowCommitWindow(NVI_GetCurrentDocumentID());
			break;
		}
		//�c�[���o�[���ځ@��������
	  case kControlID_Toolbar_FindNext:
		{
			SPI_GetCurrentFocusTextView().SPI_FindSelectedNext();
			break;
		}
		//�c�[���o�[���ځ@�O������
	  case kControlID_Toolbar_FindPrevious:
		{
			SPI_GetCurrentFocusTextView().SPI_FindSelectedPrevious();
			break;
		}
		//�c�[���o�[���ځ@�}���`�t�@�C������
	  case kControlID_Toolbar_MultiFileFind:
		{
			AText	text;
			SPI_GetCurrentFocusTextView().SPI_GetSelectedText(text);
			GetApp().SPI_ShowMultiFileFindWindow(text);
			break;
		}
		//�c�[���o�[���ځ@�I��͈͂��}���`�t�@�C������
	  case kControlID_Toolbar_MultiFileFindExecute:
		{
			AText	text;
			SPI_GetCurrentFocusTextView().SPI_GetSelectedText(text);
			//��������
			break;
		}
		//�c�[���o�[���ځ@�t���[�e�B���O�E�C���h�E�\����\��
	  case kControlID_Toolbar_ShowHideFloatingWindows:
		{
			GetApp().SPI_ShowHideFloatingSubWindowsTemporary();
			//�S�ẴE�C���h�E�̃c�[���o�[�l�X�V
			GetApp().SPI_UpdateToolbarItemValueAll();
			break;
		}
		//�s�ԍ��{�^��
	  case kControlID_LineNumberButton:
		{
			SPI_ShowMoveLineWindow();
			break;
		}
		//�s�ԍ��{�^���i�T�u�e�L�X�g�j
	  case kControlID_LineNumberButton_SubText:
		{
			SPI_ShowMoveLineWindow();
			break;
		}
		//�ꏊ�{�^��
	  case kControlID_PositionButton:
		{
			SPI_ShowHideJumpListWindow(true);
			break;
		}
		//�ꏊ�{�^���i�T�u�e�L�X�g�j
	  case kControlID_PositionButton_SubText:
		{
			SPI_ShowHideJumpListWindow(true);
			break;
		}
		//�c�[���o�[���ځ@�����t�B�[���h
	  case kControlID_Toolbar_SearchField:
		{
			FindNextWithSearchBox();
			break;
		}
		//#232
		//�c�[���{�^���폜
	  case kVirtualControlID_ToolRemoveOK:
		{
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(NVI_GetLastClickedButtonControlID());
			if( toolButtonIndex != kIndex_Invalid )
			{
				AIndex	toolButtonDBIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(toolButtonIndex);
				GetApp().SPI_GetModePrefDB(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex()).DeleteToolbarItem(toolButtonDBIndex);
			}
			break;
		}
		//�R���e�L�X�g���j���[�I�������}�N���o�[���ڈȍ~��S�č폜
	  case kVirtualControlID_DeleteMacrosAfterThis:
		{
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			AIndex	clickedMacroDisplayIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(NVI_GetLastClickedButtonControlID());
			if( clickedMacroDisplayIndex != kIndex_Invalid )
			{
				//��납��폜���Ă���
				AItemCount	displayedMacroCount = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonCount();
				for( AIndex displayIndex = displayedMacroCount-1; displayIndex >= clickedMacroDisplayIndex; displayIndex-- )
				{
					AIndex	dbIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(displayIndex);
					GetCurrentTabModePrefDB().DeleteToolbarItem(dbIndex);
				}
			}
			break;
		}
		/*#899
		//#212 �T�u�y�C�����[�h�ύX
	  case kSubPaneModeButtonControlID:
		{
			switch(mSubPaneMode)
			{
			  case kSubPaneMode_PreviousDocument:
				{
					SetSubPaneMode(kSubPaneMode_SameDocument);
					break;
				}
			  case kSubPaneMode_SameDocument:
				{
					SetSubPaneMode(kSubPaneMode_Manual);
					break;
				}
			  case kSubPaneMode_Manual:
				{
					SetSubPaneMode(kSubPaneMode_PreviousDocument);
					break;
				}
			}
			break;
		}
		*/
		//�T�u�y�C���\�������C���y�C���ɓ���������
	  case kSubPaneSyncButtonControlID:
		{
			//Diff�\������ #379
			SPI_SetDiffDisplayMode(false);
			//
			SPI_DisplayTabInSubText(true);
			//#899 SetSubPaneMode(kSubPaneMode_Manual);
			
			//
			if( false )
			{
				//���݂�view�̐܂肽���ݏ�Ԃ��擾
				ANumberArray	collapsedLinesArray;
				GetTextViewByControlID(mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetTextViewControlID(0)).
						SPI_GetFoldingCollapsedLines(collapsedLinesArray);
				//
				GetTextViewByControlID(mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSubTextControlID()).
						SPI_CollapseLines(collapsedLinesArray,true,true);
			}
			break;
		}
		//�T�u�y�C���������C���y�C������
	  case kSubPaneSwapButtonControlID:
		{
			if( mSubTextCurrentDocumentID != kObjectID_Invalid )
			{
				AIndex	subPaneTabIndex = NVI_GetTabIndexByDocumentID(mSubTextCurrentDocumentID);
				AControlID	subPaneTextControlID = mTabDataArray.GetObjectConst(subPaneTabIndex).GetSubTextControlID();
				
				//���݂�view�̐܂肽���ݏ�Ԃ��擾
				ANumberArray	mainCollapsedLinesArray;
				ANumberArray	subCollapsedLinesArray;
				if( false )
				{
					GetTextViewByControlID(mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetTextViewControlID(0)).
							SPI_GetFoldingCollapsedLines(mainCollapsedLinesArray);
					GetTextViewByControlID(subPaneTextControlID).SPI_GetFoldingCollapsedLines(subCollapsedLinesArray);
				}
				
				//�T�u�e�L�X�g�̃X�N���[���J�n�ʒu�p���O���t���擾 #450
				AIndex	subTextOriginParagraph = GetTextViewByControlID(subPaneTextControlID).SPI_GetOriginParagraph();
				//
				ABool	oldSubPaneFocused = GetTextViewByControlID(subPaneTextControlID).NVI_IsFocusActive();//#600
				AIndex	oldMainTabIndex = NVI_GetCurrentTabIndex();//#600
				
				//�T�u�y�C���̃^�u�I���{�X�N���[������
				SPI_DisplayTabInSubText(true);
				//#899 SetSubPaneMode(kSubPaneMode_Manual);
				
				//���C���y�C���̃^�u�I��
				NVI_SelectTab(subPaneTabIndex);
				//���C���y�C���̃t�H�[�J�X���������Ă���r���[���擾
				//�^�u���̃��C���e�L�X�g�̍ŏI�t�H�[�J�XTextView���擾�i�����ŏI�t�H�[�J�X�����ݒ�Ȃ�A���C���y�C�����̍ŏ��̕����r���[�ɂ���j
				AControlID	mainTextViewControlID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).
							GetLatentMainTextViewControlID();
				if( IsTextViewControlID(mainTextViewControlID) == false )
				{
					mainTextViewControlID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0);
				}
				
				//
				if( false )
				{
					GetTextViewByControlID(mTabDataArray.GetObject(oldMainTabIndex).GetSubTextControlID()).
							SPI_CollapseLines(mainCollapsedLinesArray,true,true);
					GetTextViewByControlID(mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetTextViewControlID(0)).
							SPI_CollapseLines(subCollapsedLinesArray,true,true);
				}
				
				//�V�K�Ƀt�H�[�J�X�𓖂Ă�ׂ�ControlID #600
				//���X�̃t�H�[�J�X�����C���y�C���Ȃ�A���̃T�u�y�C���Ƀt�H�[�J�X
				AControlID	newFocusControlID = mTabDataArray.GetObjectConst(oldMainTabIndex).GetSubTextControlID();
				//���C���y�C���̃t�H�[�J�X���������Ă���r���[�̃X�N���[������
				if( IsTextViewControlID(mainTextViewControlID) == true )
				{
					//#450 GetTextViewByControlID(mainTextViewControlID).NVI_ScrollTo(subPaneOriginPoint);
					//��Ŏ擾�����X�N���[���J�n�ʒu�p���O���t��scroll #450
					GetTextViewByControlID(mainTextViewControlID).SPI_ScrollToParagraph(subTextOriginParagraph,0);
					//���X�̃t�H�[�J�X���T�u�y�C���Ȃ�A���̃��C���y�C���Ƀt�H�[�J�X #600
					if( oldSubPaneFocused == true )
					{
						newFocusControlID = mainTextViewControlID;
					}
				}
				//�t�H�[�J�X�ݒ� #600
				NVI_SwitchFocusTo(newFocusControlID);
			}
			break;
		}
		//�T�u�y�C����r���[�h�؂�ւ� #379
	  case kSubPaneDiffButtonControlID:
		{
			/*#81
			if( NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_GetToggleOn() == true &&
						GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )
			{
				//��r�Ώۃh�L�������g���ݒ�Ȃ�Diff�Ώېݒ�E�C���h�E�\��
				GetApp().SPI_GetDiffWindow().SPNVI_Show(NVI_GetCurrentDocumentID());
			}
			else
			*/
			{
				SPI_SetDiffDisplayMode(NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_GetToggleOn());
			}
			break;
		}
		//�T�C�h�o�[���ݒ� #634
	  case kControlID_RightSideBar_PrefButton:
		{
			/*
			GetApp().SPI_GetAppPrefWindow().NVI_CreateAndShow();
			GetApp().SPI_GetAppPrefWindow().NVI_SelectTabControl(2);
			GetApp().SPI_GetAppPrefWindow().NVI_RefreshWindow();
			GetApp().SPI_GetAppPrefWindow().NVI_SwitchFocusToFirst();
			*/
			//��������
			break;
		}
		//�T�C�h�o�[showhide #634
	  case kControlID_RightSideBar_ShowHideButton:
		{
			ShowHideRightSideBar(!mRightSideBarDisplayed);
			break;
		}
		//�T�u�y�C��showhide #634
	  case kControlID_LeftSideBar_ShowHideButton:
		{
			ShowHideLeftSideBar(!mLeftSideBarDisplayed);
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

/**
�}�N���o�[���ڂ����s����
*/
void	AWindow_Text::SPI_DoMacroButton( const AIndex inToolButtonIndex )
{
	EVT_Clicked(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetToolButtonControlID(2+inToolButtonIndex),0);
}

//�R���g���[���l�ύX�ʒm���̃R�[���o�b�N�i�������s������true��Ԃ��j
ABool	AWindow_Text::EVTDO_ValueChanged( const AControlID inID )
{
	//#688
	//�t�H���g�p�l���ύX������
	if( inID == kFontPanelVirtualControlID )
	{
		//�t�H���g�p�l������t�H���g�E�T�C�Y�擾
		AText	fontname;
		AFloatNumber	fontsize = 9.0;
		NVI_GetControlFont(kFontPanelVirtualControlID,fontname,fontsize);
		//�h�L�������g�Ƀt�H���g�E�T�C�Y�ݒ�
		SPI_GetCurrentFocusTextDocument().SPI_SetFontNameAndSize(fontname,fontsize);
		return true;
	}
	//�X�N���[���o�[�̒l���ς�����ꍇ�̏���
	AIndex	viewIndex = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).FindFromVScrollBarControlID(inID);
	if( viewIndex != kIndex_Invalid )
	{
		AControlID	textViewControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetTextViewControlID(viewIndex);
		AImagePoint	pt;
		GetTextViewByControlID(textViewControlID).NVI_GetOriginOfLocalFrame(pt);
		pt.y = NVI_GetControlNumber(inID);
		GetTextViewByControlID(textViewControlID).NVI_ScrollTo(pt);
		return true;
	}
	return false;
}

//WindowActivated���̃R�[���o�b�N
void	AWindow_Text::EVTDO_WindowActivated()
{
	//==================�W�����v���X�g��\������==================
	//#725
	//�W�����v���X�g�̕\���A�z�u
	UpdateFloatingJumpListDisplay();
	
	//�������E�C���h�E#1255
	SPI_UpdateWindowAlpha();
	
	/*#724
	//#137 �����{�b�N�X�̃o�b�N�X���b�V�����[�h��ݒ�
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )//#688 if�ǉ�
	{
		AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
		if( searchBoxControlID != kControlID_Invalid )
		{
			NVI_GetEditBoxView(searchBoxControlID).NVI_SetInputBackslashByYenKeyMode(
						GetApp().SPI_GetModePrefDB(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputBackslashByYenKey));
		}
	}
	*/
	
	//==================�T�C�h�o�[�̕\�����X�V==================
	//#725
	//�T�C�h�o�[�\���X�V
	//#1275 RefreshSideBar();
}

//WindowDeactivated���̃R�[���o�b�N
void	AWindow_Text::EVTDO_WindowDeactivated()
{
	//==================�W�����v���X�g���\���ɂ���==================
	//win 080703 �őO�ʂ̃e�L�X�g�E�C���h�E�ł͂Ȃ��Ȃ�Ƃ��̂݃W�����v���X�g���B���悤�ɕύX
	//Windows�ł̓t���[�e�B���O�E�C���h�E�������̂ŁA�W�����v���X�g�N���b�N���ɁA�e�L�X�g�E�C���h�E��deactivate���W�����v���X�g��Hide�����삵�Ă��܂�
	if( GetApp().NVI_GetMostFrontWindowID(kWindowType_Text) != GetObjectID() )
	{
		if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
		{
			/*#725
			if( mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetJumpListVisible() == true )
			{
				SPI_GetJumpListWindow().NVI_HideIfNotOverlayMode();//#291
			}
			*/
			if( mFloatingJumpListWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Hide();
			}
		}
	}
	
	//�������E�C���h�E#1255
	SPI_UpdateWindowAlpha();
	
	//==================�T�C�h�o�[�̕\�����X�V==================
	//#725
	//�T�C�h�o�[�\���X�V
	//#1275 RefreshSideBar();
}

//#688
/**
�E�C���h�E�t�H�[�J�X�A�N�e�B�x�[�g������
*/
void	AWindow_Text::EVTDO_WindowFocusActivated()
{
	//�����s�ꏊ���� �L�[���[�h���E�C���h�E���N���b�N�����́Akeywindow�ړ����Ȃ����߁B
	//#725
	//�t���[�e�B���O�E�C���h�E��z-order�𒲐�
	GetApp().SPI_AdjustAllFlotingWindowsZOrder();
}

//#688
/**
�E�C���h�E�t�H�[�J�X�f�B�A�N�e�B�x�[�g������
*/
void	AWindow_Text::EVTDO_WindowFocusDeactivated()
{
}

//#725
#if 0
//B0442
//�t���[�e�B���O�E�C���h�E���B��
void	AWindow_Text::SPI_HideFloatingWindow()
{
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		/*#725
		if( mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetJumpListVisible() == true )
		{
			SPI_GetJumpListWindow().NVI_HideIfNotOverlayMode();//#291
		}
		*/
		if( mFloatingJumpListWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Hide();
		}
	}
}
#endif

//#1255
void	AWindow_Text::SPI_UpdateWindowAlpha()
{
	//B0333
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kAlphaWindowModeV3) == true )
	{
		//�s�������擾
		AFloatNumber	alpha = 1.0;
		//�őO�ʃe�L�X�g�E�C���h�E���ǂ����ł��ꂼ��̕s�������擾
		if( GetApp().NVI_GetMostFrontWindowID(kWindowType_Text) == GetObjectID() )
		{
			alpha = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kAlphaWindowPercent1V3);
			alpha /= 100.0;
		}
		else
		{
			alpha = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kAlphaWindowPercent2V3);
			alpha /= 100.0;
		}
		//���C���E�C���h�E
		NVI_SetWindowTransparency(alpha);
		//�E�T�C�h�o�[
		for( AIndex index = 0; index < mRightSideBarArray_OverlayWindowID.GetItemCount(); index++ )
		{
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(index);
			GetApp().NVI_GetWindowByID(winID).NVI_SetWindowTransparency(alpha);
		}
		//���T�C�h�o�[
		for( AIndex index = 0; index < mLeftSideBarArray_OverlayWindowID.GetItemCount(); index++ )
		{
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(index);
			GetApp().NVI_GetWindowByID(winID).NVI_SetWindowTransparency(alpha);
		}
	}
	else
	{
		NVI_SetWindowTransparency(1.0);
		//�E�T�C�h�o�[
		for( AIndex index = 0; index < mRightSideBarArray_OverlayWindowID.GetItemCount(); index++ )
		{
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(index);
			GetApp().NVI_GetWindowByID(winID).NVI_SetWindowTransparency(1.0);
		}
		//���T�C�h�o�[
		for( AIndex index = 0; index < mLeftSideBarArray_OverlayWindowID.GetItemCount(); index++ )
		{
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(index);
			GetApp().NVI_GetWindowByID(winID).NVI_SetWindowTransparency(1.0);
		}
	}
}

//WindowCollapsed���̃R�[���o�b�N
void	AWindow_Text::EVTDO_WindowCollapsed()
{
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		/*#725
		if( mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetJumpListVisible() == true )
		{
			SPI_GetJumpListWindow().NVI_HideIfNotOverlayMode();//#291
		}
		*/
		if( mFloatingJumpListWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Hide();
		}
	}
}

/**
�E�C���h�E�ʒu�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_Text::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//�E�C���h�Ebounds�ɕω���������Ή������Ȃ�
	if( inPrevBounds.left == inCurrentBounds.left && inPrevBounds.top == inCurrentBounds.top &&
				inPrevBounds.right == inCurrentBounds.right && inPrevBounds.bottom == inCurrentBounds.bottom )
	{
		return;
	}
	/*#725
	if( mRightSideBarDisplayed == false )//#291
	{
		SPI_GetJumpListWindow().SPI_UpdatePosition();
	}
	*/
	//B0407 ���I���T�C�Y���A���̃��\�b�h���R�[�������B���T�C�Y�ɏ]���āA�R���g���[�����i�̔z�uUpdate���s���B�i�s�܂�Ԃ�����Update���Ȃ��j
	//if( ((inPrevBounds.right-inPrevBounds.left) != (inCurrentBounds.right-inCurrentBounds.left)) ||
	//			((inPrevBounds.bottom-inPrevBounds.top) != (inCurrentBounds.bottom-inCurrentBounds.top)) )
	{
		UpdateViewBounds(NVI_GetCurrentTabIndex(),kUpdateViewBoundsTriggerType_WindowBoudnsChanged);
	}
	//�t���[�e�B���O�W�����v���X�g�̈ʒu�X�V
	UpdateFloatingJumpListDisplay();
}

//
ABool	AWindow_Text::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//�s�ړ�
	  case kMenuItemID_MoveLine:
		{
			SPI_ShowMoveLineWindow();
			break;
		}
		//�t�H���g�T�C�Y  #966
		//���j���[���ڂ̕�����̍ŏ��̐����ɏ]���ăt�H���g�T�C�Y�ݒ�
	  case kMenuItemID_FontSize:
	  case kMenuItemID_FontSize60:
	  case kMenuItemID_FontSize80:
	  case kMenuItemID_FontSize90:
	  case kMenuItemID_FontSize95:
	  case kMenuItemID_FontSize100:
	  case kMenuItemID_FontSize105:
	  case kMenuItemID_FontSize110:
	  case kMenuItemID_FontSize120:
	  case kMenuItemID_FontSize140:
	  case kMenuItemID_FontSize160:
	  case kMenuItemID_FontSize180:
	  case kMenuItemID_FontSize240:
		{
			AFloatNumber	num = inDynamicMenuActionText.GetFloatNumber();
			SPI_GetCurrentTabTextDocument().SPI_SetTemporaryFontSize(num);//#966
			break;
		}
	  case kMenuItemID_RevertFontSize:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetTemporaryFontSize(0);
			break;
		}
			/*#844
		//�t�H���g�T�C�Y�i���̑��j
	  case kMenuItemID_FontSizeOther:
		{
			SPI_GetFontSizeWindow().NVI_Create(NVI_GetCurrentDocumentID());
			SPI_GetFontSizeWindow().NVI_Show();
			SPI_GetFontSizeWindow().NVI_SwitchFocusToFirst();
			break;
		}*/
		//�t�H���g
	  case kMenuItemID_Font:
		{
			//win AFont	font = inDynamicMenuActionText.GetNumber();
			/*win AFont	font;
			AFontWrapper::GetFontByName(inDynamicMenuActionText,font);
			SPI_GetCurrentTextDocument().SPI_SetFont(font);*/
			SPI_GetCurrentTabTextDocument().SPI_SetFontName(inDynamicMenuActionText);
			break;
		}
		//TextEncoding
	  case kMenuItemID_SetTextEncoding:
		{
			//B0343
			if( ATextEncodingWrapper::CheckTextEncodingAvailability(inDynamicMenuActionText) == false )
			{
				break;
			}
			SPI_GetCurrentTabTextDocument().SPI_SetTextEncoding(inDynamicMenuActionText);
			break;
		}
		//���s�R�[�h
	  case kMenuItemID_ReturnCode_CR:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetReturnCode(returnCode_CR);
			break;
		}
	  case kMenuItemID_ReturnCode_CRLF:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetReturnCode(returnCode_CRLF);
			break;
		}
	  case kMenuItemID_ReturnCode_LF:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetReturnCode(returnCode_LF);
			break;
		}
		//�s����
	  case kMenuItemID_WrapMode_NoWrap:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetWrapMode(kWrapMode_NoWrap,0);
			break;
		}
	  case kMenuItemID_WrapMode_WordWrap:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetWrapMode(kWrapMode_WordWrap,0);
			break;
		}
	  case kMenuItemID_WrapMode_WordWrapByLetterCount:
		{
			SPI_ShowWrapLetterCountWindow(kWrapMode_WordWrapByLetterCount);//#1113
			break;
		}
		//#1113
	  case kMenuItemID_WrapMode_LetterWrap:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetWrapMode(kWrapMode_LetterWrap,0);
			break;
		}
	  case kMenuItemID_WrapMode_LetterWrapByLetterCount:
		{
			SPI_ShowWrapLetterCountWindow(kWrapMode_LetterWrapByLetterCount);
			break;
		}
		//�t�H���g #868
		//���[�h�̃t�H���g���g��
	  case kMenuItemID_UseModeFont:
		{
			//�h�L�������g�ɐݒ�
			SPI_GetCurrentTabTextDocument().SPI_SetUseModeFont(true);
			//���[�h�ݒ��ʂ��J���i�������A�u�W�����[�h�Ɠ����v�ݒ�ɂ��Ă���ꍇ�́A�W�����[�h���J���j
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kSameAsNormalMode_Font) == true )
			{
				modeIndex = kStandardModeIndex;
			}
			GetApp().SPI_ShowModePrefWindow(modeIndex);
			GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SelectTabControl(0);
			GetApp().SPI_GetModePrefWindow(modeIndex).NVI_ShowControlFontPanel(AWindow_ModePref::kFontControl_DefaultFont);
			break;
		}
		//�h�L�������g�̃t�H���g���g��
	  case kMenuItemID_UseDocumentFont:
		{
			//�h�L�������g�ɐݒ�
			SPI_GetCurrentTabTextDocument().SPI_SetUseModeFont(false);
			//�t�H���g�p�l����\��
			AText	fontname;
			SPI_GetCurrentTabTextDocument().SPI_GetFontName(fontname);
			AFloatNumber	fontsize = SPI_GetCurrentTabTextDocument().SPI_GetFontSize();
			NVI_ShowFontPanel(kFontPanelVirtualControlID,fontname,fontsize);
			break;
		}
		//�W�����v���X�g�\���^��\��
	  case kMenuItemID_ShowJumpList:
		{
			SPI_ShowHideJumpListWindow(true);
			break;
		}
	  case kMenuItemID_Close:
		{
			//#513 win �E�C���h�E��������ʃA�v���I���̉\��������̂ŁA�����ŕۑ�
			GetApp().SPI_SaveReopenFile();
			//
			if( AKeyWrapper::IsOptionKeyPressed() == true )
			{
				GetApp().NVI_CloseAllWindow();
			}
			else
			{
				TryClose(NVI_GetCurrentTabIndex());
			}
			result = true;
			break;
		}
		//���[�h
	  case kMenuItemID_SetMode:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetModeByRawName(inDynamicMenuActionText);
			break;
		}
		//�y�[�W�ݒ�
	  case kMenuItemID_PageSetup:
		{
			SPI_GetCurrentTabTextDocument().NVI_PrintPageSetup(GetObjectID());
			//#902 SPI_GetCurrentTabTextDocument().NVI_SetDirty(true);
			break;
		}
		/*#524
		//���
	  case kMenuItemID_Print:
		{
			SPI_Print();
			break;
		}
		*/
		/*#844 �v�����g�I�v�V�����͊��ݒ�݂̂ɂ���i�h�L�������g���Ƃ̐ݒ�Ȃ��j 
		//����I�v�V����
	  case kMenuItemID_ShowPrintOptionWindow:
		{
			SPI_ShowPrintOptionWindow();
			break;
		}
		*/
		/*#844
		//�v���p�e�B�E�C���h�E
	  case kMenuItemID_ShowDocPropertyWindow:
		{
			SPI_ShowDocPropertyWindow();
			break;
		}
		*/
		//FTP�G�C���A�X�ۑ�
	  case kMenuItemID_SaveFTPAlias:
		{
			SaveFTPAlias();
			break;
		}
		//
	  case kMenuItemID_CorrectEncoding:
		{
			if( SPI_GetCurrentTabTextDocument().NVI_IsDirty() == true )   break;
			ATextEncodingFallbackType	fallback = kTextEncodingFallbackType_None;//#473
			SPI_ShowTextEncodingCorrectWindow(true,fallback);
			break;
		}
		//�v���p�e�B�E�C���h�E
	  case kMenuItemID_ShowHideCrossCaret:
		{
			/*#232
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(tabIndex).GetViewCount(); viewIndex++ )
			{
				GetTextViewByControlID(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(viewIndex)).SPI_ShowHideCrossCaret();
			}
			*/
			SPI_ShowHideCrossCaret();
			break;
		}
		//
	  case kMenuItemID_FindEncodingProblem:
		{
			SPI_GetCurrentTabTextDocument().SPI_ReportTextEncodingErrors();
			break;
		}
		//R0199
	  case kMenuItemID_FindMisspell:
		{
			SPI_GetCurrentTabTextDocument().SPI_ReportSpellCheck();
			break;
		}
		//
	  case kMenuItemID_WindowToTab:
		{
			SPI_WindowToTab();//#389
			break;
		}
		//
	  case kMenuItemID_TabToWindow:
		{
			SPI_DetachTab(NVI_GetCurrentTabIndex());//#389
			break;
		}
		//
	  case kMenuItemID_JumpExportToText:
		{
			AText	text;
			const ATextArray&	textArray = GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetJumpMenuTextArray();
			for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
			{
				AText	t;
				textArray.Get(i,t);
				text.AddText(t);
				text.Add(kUChar_LineEnd);
			}
			ADocumentID docID = GetApp().SPNVI_CreateNewTextDocument(GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetModeIndex());
			if( docID == kObjectID_Invalid )   break;//win
			GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
			break;
		}
		//
	  case kMenuItemID_SplitView:
		{
			SPI_SplitView(false);
			break;
		}
		//
	  case kMenuItemID_ConcatView:
		{
			SPI_ConcatSplitView();
			break;
		}
		//#212 �T�u�y�C���\���^��\���ύX
	  case kMenuItemID_DisplaySubPane:
		{
			ShowHideLeftSideBar(!mLeftSideBarDisplayed);
			break;
		}
		//#291 �E�T�C�h�o�[�\���^��\���ύX
	  case kMenuItemID_DisplayInfoPane:
		{
			ShowHideRightSideBar(!mRightSideBarDisplayed);
			break;
		}
		//#725 �T�u�e�L�X�g�J�����\���E��\���ύX
	  case kMenuItemID_DisplaySubTextPane:
		{
			ShowHideSubTextColumn(!mSubTextColumnDisplayed);
			break;
		}
		//#212
	  case kMenuItemID_SubTextPaneMenu:
		{
			AIndex	tabIndex;
			tabIndex = inDynamicMenuActionText.GetNumber();
			SPI_DisplayInSubText(tabIndex,false,kIndex_Invalid,false,false);
			break;
		}
		//#232
		//�c�[���{�^���i�t�H���_�j�̓��e���N���b�N����
	  case kMenuItemID_ToolBar_Folder:
		{
			AControlID	focus = GetTopMostFocusTextViewControlID();
			if( focus != kControlID_Invalid )
			{
				AFileAcc	file;
				file.Specify(inDynamicMenuActionText);
				GetTextViewByControlID(focus).SPI_DoTool(file,inModifierKeys,false);
			}
			break;
		}
		//�c�[���{�^����ҏW����
	  case kMenuItemID_ToolBar_EditButton:
		{
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(NVI_GetLastClickedButtonControlID());
			if( toolButtonIndex != kIndex_Invalid )
			{
				switch( mTabDataArray.GetObjectConst(tabIndex).GetToolButtonType(toolButtonIndex) )
				{
				  case kToolbarItemType_File:
					{
						GetApp().GetAppPref().LaunchAppWithFile(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonFile(toolButtonIndex),0);
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
			}
			break;
		}
		//�\���X�V
	  case kMenuItemID_ToolBar_UpdateDisplay:
		{
			GetApp().SPI_RemakeToolButtonsAll(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex());
			break;
		}
		//�c�[���{�^�����\���ɂ���
	  case kMenuItemID_ToolBar_InvalidButton:
		{
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(NVI_GetLastClickedButtonControlID());
			if( toolButtonIndex != kIndex_Invalid )
			{
				GetCurrentTabModePrefDB().SetEnableToolbarItem(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(toolButtonIndex),false);
			}
			break;
		}
		//�c�[���{�^����S�ĕ\������
	  case kMenuItemID_ToolBar_DisplayAllButton:
		{
			GetCurrentTabModePrefDB().SetEnableToolbarItemAll();
			break;
		}
		//�c�[���{�^�����폜
	  case kMenuItemID_ToolBar_DeleteButton:
		{
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("TextWindow_DeleteToolButton1");
			mes2.SetLocalizedText("TextWindow_DeleteToolButton2");
			mes3.SetLocalizedText("TextWindow_DeleteToolButton3");
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kVirtualControlID_ToolRemoveOK);
			break;
		}
		
	  case kMenuItemID_CM_DetachTab_Left:
		{
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			SPI_DetachTab_Left(tabIndex);//#389
			break;
		}
	  case kMenuItemID_CM_DetachTab_Right:
		{
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			SPI_DetachTab_Right(tabIndex);//#389
			break;
		}
	  /* case kMenuItemID_CM_DetachTab_Bottom:
		{
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			ATextWindowFactory	factory(&(GetApp()));
			AObjectID	winID = GetApp().NVI_CreateWindow(factory);
			SPI_CopyTabToWindow(tabIndex,winID);
			SPI_CloseTab(tabIndex);
			//
			ARect	rect;
			GetApp().NVI_GetWindowByID(winID).NVI_GetWindowBounds(rect);
			rect.top += (rect.bottom-rect.top)/2;
			GetApp().NVI_GetWindowByID(winID).NVI_SetWindowBounds(rect);
			//
			NVI_GetWindowBounds(rect);
			rect.bottom -= (rect.bottom-rect.top)/2;
			NVI_SetWindowBounds(rect);
			break;
		}*/
	  case kMenuItemID_CM_CloseTab:
		{
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			SPI_CloseTab(tabIndex);
			break;
		}
	  case kMenuItemID_CM_DisplayInSubTextPane:
		{
			//#725 �T�u�e�L�X�g�̓T�C�h�o�[�ɕ\�����Ȃ��̂ŁA�T�C�h�o�[�\�������͂��Ȃ��B ShowHideLeftSideBar(true);
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			SPI_DisplayTabInSubText(tabIndex,true);
			break;
		}
#if IMPLEMENTATION_FOR_WINDOWS
		//�J��
	  case kMenuItemID_Open:
		{
			AText	filter;
			GetApp().SPI_GetDefaultFileFilter(filter);
			NVI_ShowFileOpenWindow(filter,false);
			result = true;
			break;
		}
	  case kMenuItemID_OpenInvisible:
		{
			AText	filter;
			GetApp().SPI_GetDefaultFileFilter(filter);
			NVI_ShowFileOpenWindow(filter,true);
			result = true;
			break;
		}
#endif
		/*#725
		//�C���f�b�N�X�E�C���h�E�E��
	  case kMenuItemID_IndexNext:
		{
			SPI_IndexWindowNext();
			break;
		}
		//�C���f�b�N�X�E�C���h�E�E�O
	  case kMenuItemID_IndexPrevious:
		{
			SPI_IndexWindowPrev();
			break;
		}*/
		//�����{�b�N�X�ւ̃t�H�[�J�X�ړ� #137
	  case kMenuItemID_Find:
		{
			//���ݒ�Ō����{�b�N�X�ւ̃t�H�[�J�X�ړ���ON�A���A�����{�b�N�X�\�����A���A���݂̃t�H�[�J�X��text view�̏ꍇ�A�����{�b�N�X�փt�H�[�J�X�ړ�
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFindShortcutIsFocusSearchBox) == true &&
			   mToolbarExistArray.Find(kControlID_Toolbar_SearchField) != kIndex_Invalid && 
			   NVI_IsControlVisible(kControlID_Toolbar_SearchField) &&
			   IsTextViewControlID(NVI_GetCurrentFocus()) == true )
			{
				NVI_SwitchFocusTo(kControlID_Toolbar_SearchField);
				result = true;
				break;
			}
			/*#724
			AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
			if( NVI_GetCurrentFocus() != searchBoxControlID && searchBoxControlID != kControlID_Invalid &&
						GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplaySearchBox) == true &&
						GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFindShortcutIsFocusSearchBox) == true )
			{
				if( NVI_GetEditBoxView(searchBoxControlID).NVI_IsVisible() == true )//#383
				{
					NVI_GetEditBoxView(searchBoxControlID).NVI_SetSelectAll();
					NVI_SwitchFocusTo(searchBoxControlID);
					result = true;
				}
				else
				{
					result = false;
				}
			}
			else
			*/
			{
				//AApp�ŏ����i�����E�C���h�E�\���j
				result = false;
			}
			break;
		}
		//�������� #137
	  case kMenuItemID_FindNext:
		{
			//�őO�ʂ�text view�Ŏ�������
			AControlID	focus = GetTopMostFocusTextViewControlID();
			if( focus != kControlID_Invalid )
			{
				GetTextViewByControlID(focus).SPI_FindNext();
				result = true;
			}
			/*#724
			AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
			//�t�H�[�J�X�������{�b�N�X�ɂ���AlastFocusedTextView���L���ȏꍇ�̂ݎ��s
			if( NVI_GetCurrentFocus() == searchBoxControlID && searchBoxControlID != kControlID_Invalid &&
						lastFocusedTextView != kControlID_Invalid )
			{
				AFindParameter	param;
				GetApp().SPI_GetFindParam(param);
				GetTextViewByControlID(lastFocusedTextView).SPI_FindNext(param);
				SPI_GetTextDocument(lastFocusedTextView).SPI_SetFindHighlight(param);
				GetApp().SPI_GetFindWindow().SPI_AddRecentlyUsedFindText(param);
				result = true;
			}
			*/
			break;
		}
		//�O������ #137
	  case kMenuItemID_FindPrevious:
		{
			//�őO�ʂ�text view�őO������
			AControlID	focus = GetTopMostFocusTextViewControlID();
			if( focus != kControlID_Invalid )
			{
				GetTextViewByControlID(focus).SPI_FindPrev();
				result = true;
			}
			/*#724
			AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
			AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
			//�t�H�[�J�X�������{�b�N�X�ɂ���AlastFocusedTextView���L���ȏꍇ�̂ݎ��s
			if( NVI_GetCurrentFocus() == searchBoxControlID && searchBoxControlID != kControlID_Invalid &&
						lastFocusedTextView != kControlID_Invalid )
			{
				AFindParameter	param;
				GetApp().SPI_GetFindParam(param);
				GetTextViewByControlID(lastFocusedTextView).SPI_FindPrevious(param);
				SPI_GetTextDocument(lastFocusedTextView).SPI_SetFindHighlight(param);
				GetApp().SPI_GetFindWindow().SPI_AddRecentlyUsedFindText(param);
				result = true;
			}
			*/
			break;
		}
		//�ŏ����猟�� #137
	  case kMenuItemID_FindFromFirst:
		{
			//�őO�ʂ�text view�ōŏ����猟��
			AControlID	focus = GetTopMostFocusTextViewControlID();
			if( focus != kControlID_Invalid )
			{
				GetTextViewByControlID(focus).SPI_FindFromFirst();
				result = true;
			}
			/*#724
			AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
			AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
			//�t�H�[�J�X�������{�b�N�X�ɂ���AlastFocusedTextView���L���ȏꍇ�̂ݎ��s
			if( NVI_GetCurrentFocus() == searchBoxControlID && searchBoxControlID != kControlID_Invalid &&
						lastFocusedTextView != kControlID_Invalid )
			{
				AFindParameter	param;
				GetApp().SPI_GetFindParam(param);
				GetTextViewByControlID(lastFocusedTextView).SPI_FindFromFirst(param);
				SPI_GetTextDocument(lastFocusedTextView).SPI_SetFindHighlight(param);
				GetApp().SPI_GetFindWindow().SPI_AddRecentlyUsedFindText(param);
				result = true;
			}
			*/
			break;
		}
		//�����n�C���C�g���� #137
	  case kMenuItemID_EraseFindHighlight:
		{
			SPI_GetCurrentFocusTextDocument().SPI_ClearFindHighlight();
			result = true;
			break;
		}
		//#357
	  case kMenuItemID_SwitchToNextTab:
		{
			SPI_SwitchTabNext();
			break;
		}
		//#357
	  case kMenuItemID_SwitchToPrevTab:
		{
			SPI_SwitchTabPrev();
			break;
		}
		//#390
	  case kMenuItemID_KeyRecordRecentlyPlay:
		{
			GetApp().SPI_SetKeyRecordedRawText(inDynamicMenuActionText);
			break;
		}
		/*#476
		//#404
	  case kMenuItemID_FullScreenMode:
		{
			SPI_SwitchFullScreenMode();
			break;
		}
		*/
		//#455
	  case kMenuItemID_SCMCommit:
		{
			SPI_ShowCommitWindow(NVI_GetCurrentDocumentID());
			break;
		}
		/*#725
		//#465
	  case kMenuItemID_CM_IndexView:
		{
			if( mFindResultViewID != kObjectID_Invalid )
			{
				AIndex	rowIndex = AView_Index::GetIndexViewByViewID(mFindResultViewID).SPI_GetContextMenuSelectedRowDBIndex();
				AView_Index::GetIndexViewByViewID(mFindResultViewID).SPI_ExportToNewDocument(rowIndex);
			}
			break;
		}
		*/
		//#454
	  case kMenuItemID_JumpListHistory:
		{
			//docpath+���s+idtext�̌`��actiontext�ɓ����Ă���̂Ŏ��o��
			AText	docpath, headertext;
			AIndex	pos = 0;
			for( ; pos < inDynamicMenuActionText.GetItemCount(); pos++ )
			{
				if( inDynamicMenuActionText.Get(pos) == kUChar_LineEnd )   break;
			}
			inDynamicMenuActionText.GetText(0,pos,docpath);
			inDynamicMenuActionText.GetText(pos+1,inDynamicMenuActionText.GetItemCount()-(pos+1),headertext);
			//�h�L�������g��������
			AFileAcc	file;
			file.Specify(docpath);
			ADocumentID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file);
			if( docID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_RevealFromJumpMenuText(headertext);
			}
			break;
		}
		/*#899
		//#379
	  case kMenuItemID_SubPaneMode_Manual:
		{
			SetSubPaneMode(kSubPaneMode_Manual);
			break;
		}
	  case kMenuItemID_SubPaneMode_PrevTab:
		{
			SetSubPaneMode(kSubPaneMode_PreviousDocument);
			break;
		}
	  case kMenuItemID_SubPaneMode_SameTab:
		{
			SetSubPaneMode(kSubPaneMode_SameDocument);
			break;
		}
		*/
		//#619
	  case kMenuItemID_AddToolbarButton:
		{
			SPI_ShowAddToolButtonWindow();
			break;
		}
		/*#844
	  case kMenuItemID_HideAllModeCreatorToolbarButton:
		{
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			GetApp().SPI_GetModePrefDB(modeIndex).SetEnableToolbarItemAll(false,true,false);
			break;
		}
		*/
	  case kMenuItemID_HideAllUserToolbarButton:
		{
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			GetApp().SPI_GetModePrefDB(modeIndex).SetEnableToolbarItemAll(false);//#844 ,false,true);
			break;
		}
		//�}�N����Finder��ɕ\��
	  case kMenuItemID_RevealMacroWithFinder:
		{
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			AFileAcc	folder;
			GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarFolder(folder);
			ALaunchWrapper::Reveal(folder);
			break;
		}
		//Finder�ł̃}�N���\���ύX�𔽉f
	  case kMenuItemID_ReconfigMacro:
		{
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			GetApp().SPI_GetModePrefDB(modeIndex).LoadToolbar();
			break;
		}
		//�}�N���@�E�N���b�N�������ڈȍ~��S�Ĕ�\���ɂ���
	  case kMenuItemID_HideMacrosAfterThis:
		{
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			AIndex	clickedMacroDisplayIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(NVI_GetLastClickedButtonControlID());
			if( clickedMacroDisplayIndex != kIndex_Invalid )
			{
				//��납���\���ɂ��Ă����iSetEnableToolbarItem()����SPI_RemakeToolButtonsAll()���R�[�������̂ŁA
				//SetEnableToolbarItem()���R�[���������_��mTabDataArray����f�[�^�������Ȃ邽�߁B�j
				AItemCount	displayedMacroCount = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonCount();
				for( AIndex displayIndex = displayedMacroCount-1; displayIndex >= clickedMacroDisplayIndex; displayIndex-- )
				{
					AIndex	dbIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(displayIndex);
					GetCurrentTabModePrefDB().SetEnableToolbarItem(dbIndex,false);
				}
			}
			break;
		}
		//�}�N���@�E�N���b�N�������ڈȍ~��S�č폜�i�m�F�_�C�A���O�\���j
	  case kMenuItemID_DeleteMacrosAfterThis:
		{
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("TextWindow_DeleteToolButton1");
			mes2.SetLocalizedText("TextWindow_DeleteToolButton2");
			mes3.SetLocalizedText("TextWindow_DeleteToolButton3");
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kVirtualControlID_DeleteMacrosAfterThis);
			break;
		}
		//�^�u�@�E�N���b�N�������ڈȍ~�����
	  case kMenuItemID_CloseTabsAfterThis:
		{
			AArray<ADocumentID>	docIDArray;
			//�N���b�N�����^�u�擾
			AIndex	clickedTabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			AIndex	clickedTabDisplayIndex = GetTabSelectorConst().SPI_GetDisplayTabIndex(clickedTabIndex);
			//����ׂ��^�u��document id���擾
			AItemCount	tabDisplayCount = GetTabSelectorConst().SPI_GetDisplayTabCount();
			for( AIndex tabDisplayIndex = tabDisplayCount-1 ; tabDisplayIndex >= clickedTabDisplayIndex; tabDisplayIndex-- )
			{
				AIndex	tabIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(tabDisplayIndex);
				docIDArray.Add(NVI_GetDocumentIDByTabIndex(tabIndex));
			}
			//document�ɑΉ�����^�u�����idirty�łȂ����̂̂݁j
			for( AIndex i = 0; i < docIDArray.GetItemCount(); i++ )
			{
				ADocumentID	docID = docIDArray.Get(i);
				if( GetApp().SPI_GetTextDocumentByID(docID).NVI_IsDirty() == false )
				{
					TryClose(NVI_GetTabIndexByDocumentID(docID));
				}
			}
			break;
		}
		//���݂̃h�L�������g�̃��[�h�ݒ�
	  case kMenuItemID_CurrentModePref:
		{
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			GetApp().SPI_ShowModePrefWindow(modeIndex);
			break;
		}
		//�t���O�̂Ȃ��^�u��S�ĕ���
	  case kMenuItemID_CloseAllTabWithoutFlag:
		{
			//�S�Ă�document id���擾�iz-order�̌�둤���珇�Ɂj
			AArray<ADocumentID>	docIDArray;
			for( AIndex zorder = NVI_GetSelectableTabCount()-1; zorder >= 0; zorder-- )
			{
				//�^�u�̕\��index�擾
				AIndex	tabIndex = NVI_GetTabIndexByZOrderIndex(zorder);
				//doc id�擾
				docIDArray.Add(NVI_GetDocumentIDByTabIndex(tabIndex));
			}
			//��Ŏ擾�������Ƀ^�u�����idirty�łȂ����́A���A�t���O�̖������̂̂݁j
			for( AIndex i = 0; i < docIDArray.GetItemCount(); i++ )
			{
				ADocumentID	docID = docIDArray.Get(i);
				if( GetApp().SPI_GetTextDocumentByID(docID).NVI_IsDirty() == false && 
							GetApp().SPI_GetTextDocumentByID(docID).SPI_GetDocumentFlag() == false )
				{
					TryClose(NVI_GetTabIndexByDocumentID(docID));
				}
			}
			break;
		}
		//�^�u�Ƀt���O�ݒ�^�ݒ����
	  case kMenuItemID_SetDocumentFlag:
		{
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
			GetApp().SPI_GetTextDocumentByID(docID).
					SPI_SetDocumentFlag(!GetApp().SPI_GetTextDocumentByID(docID).SPI_GetDocumentFlag());
			break;
		}
		/*drop
		//#737
	  case kMenuItemID_CompareLeftRightSelected:
		{
			SPI_CompareWithSubText(true);
			break;
		}
		*/
		//�t���[�e�B���O�E�C���h�E��S�Ĉꎞ�I�ɔ�\��
	  case kMenuItemID_ShowHideAllFloatingWindow:
		{
			GetApp().SPI_ShowHideFloatingSubWindowsTemporary();
			//�S�ẴE�C���h�E�̃c�[���o�[�l�X�V
			GetApp().SPI_UpdateToolbarItemValueAll();
			break;
		}
		//�|�b�v�A�b�v�֎~
	  case kMenuItemID_InhibitPopup:
		{
			//�|�b�v�A�b�v�֎~��Ԏ擾�A�ݒ�
			GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kProhibitPopup,
													 !GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup));
			//�֎~�ɂ����Ƃ��́A���ݕ\�����̃|�b�v�A�b�v��S�ĕ���
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup) == true )
			{
				SPI_HideAllPopupWindows();
			}
			//�S�ẴE�C���h�E�̃c�[���o�[�l�X�V
			GetApp().SPI_UpdateToolbarItemValueAll();
			break;
		}
		//#725
		//�E�T�C�h�o�[���ڒǉ�
	  case kMenuItemID_AddRecentlyOpenFileListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_FileList,0,200);
			break;
		}
	  case kMenuItemID_AddSameFolderFileListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_FileList,1,200);
			break;
		}
	  case kMenuItemID_AddSameProjectFileListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_FileList,2,200);
			break;
		}
	  case kMenuItemID_AddRemoteFileListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_FileList,3,200);
			break;
		}
	  case kMenuItemID_AddJumpListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_JumpList,0,300);
			break;
		}
	  case kMenuItemID_AddDocInfoInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_DocInfo,0,80);
			break;
		}
	  case kMenuItemID_AddTextMarkerInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_TextMarker,0,120);
			break;
		}
	  case kMenuItemID_AddCandidateListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_CandidateList,0,150);
			break;
		}
	  case kMenuItemID_AddToolListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_KeyToolList,0,150);
			break;
		}
	  case kMenuItemID_AddIdInfoListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_IdInfo,0,250);
			break;
		}
	  case kMenuItemID_AddCallGrefInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_CallGraf,0,300);
			break;
		}
	  case kMenuItemID_AddPreviewInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_Previewer,0,400);
			break;
		}
	  case kMenuItemID_AddWordsListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_WordsList,0,300);
			break;
		}
	  case kMenuItemID_AddKeyRecordInRightSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_KeyRecord,0,150);
			break;
		}
		//#725
		//���T�C�h�o�[���ڒǉ�
	  case kMenuItemID_AddRecentlyOpenFileListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_FileList,0,200);
			break;
		}
	  case kMenuItemID_AddSameFolderFileListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_FileList,1,200);
			break;
		}
	  case kMenuItemID_AddSameProjectFileListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_FileList,2,200);
			break;
		}
	  case kMenuItemID_AddRemoteFileListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_FileList,3,200);
			break;
		}
	  case kMenuItemID_AddJumpListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_JumpList,0,300);
			break;
		}
	  case kMenuItemID_AddDocInfoInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_DocInfo,0,80);
			break;
		}
	  case kMenuItemID_AddTextMarkerInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_TextMarker,0,120);
			break;
		}
	  case kMenuItemID_AddCandidateListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_CandidateList,0,150);
			break;
		}
	  case kMenuItemID_AddToolListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_KeyToolList,0,150);
			break;
		}
	  case kMenuItemID_AddIdInfoListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_IdInfo,0,250);
			break;
		}
	  case kMenuItemID_AddCallGrefInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_CallGraf,0,300);
			break;
		}
	  case kMenuItemID_AddPreviewInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_Previewer,0,400);
			break;
		}
	  case kMenuItemID_AddWordsListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_WordsList,0,300);
			break;
		}
	  case kMenuItemID_AddKeyRecordInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_KeyRecord,0,150);
			break;
		}
		//#688
		//�c�[���o�[�\����\��
	  case kMenuItemID_ShowHideToolbar:
		{
			NVI_ShowHideToolbar();
			break;
		}
		//�c�[���o�[�J�X�^�}�C�Y
	  case kMenuItemID_CustomizeToolbar:
		{
			NVI_ShowToolbarCustomize();
			break;
		}
		//#793
		//���̃t�@�C���ƉE�̃t�@�C�����r
	  case kMenuItemID_CompareLeftFileAndRightFile:
		{
			ADocumentID	mainDocID = NVI_GetCurrentDocumentID();
			ADocumentID	subDocID = SPI_GetCurrentSubTextPaneDocumentID();
			if( mainDocID != kObjectID_Invalid && subDocID != kObjectID_Invalid && mainDocID != subDocID )
			{
				AFileAcc	subFile;
				GetApp().SPI_GetTextDocumentByID(subDocID).NVI_GetFile(subFile);
				if( subFile.IsSpecified() == true )
				{
					//���C��text view�̃h�L�������g�ɂāA�T�u�e�L�X�g�̃t�@�C���Ƃ̔�r�����s
					GetApp().SPI_GetTextDocumentByID(mainDocID).SPI_SetDiffMode_File(subFile,kCompareMode_File);
					SPI_SetDiffDisplayMode(true);
				}
			}
			break;
		}
		//#688
	  case kMenuItemID_Zoom:
		{
			NVI_Zoom();
			break;
		}
		//#1062
		//�^�u���A���t�@�x�b�g���Ƀ\�[�g����
	  case kMenuItemID_SortTabs:
		{
			SortTabs();
			break;
		}
		//���܂� #1194
	  case kMenuItemID_Minimize:
		{
			NVI_CollapseWindow(true);
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
void	AWindow_Text::EVTDO_UpdateMenu()
{
	if( NVI_GetTabCount() == 0 )   return;//#291
	/*B0411 B0410
	if( SPI_GetAskingSaveChanges() == true ) 
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,false);
		return;
	}
	if( NVI_IsPrintMode() == true )   return;
	*/
	//fprintf(stderr,"UpdateMenu() ");
	/*B0411
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(NVI_ChildWindowVisible()==false));
	*/
	//��
	/*#725
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowJumpList,//#291 true);
				(SPI_GetJumpListWindow().NVI_GetOverlayMode()!=true));
		AText	text;
		//#291 if( SPI_GetJumpListWindow().NVI_IsWindowVisible() == true )
		if( mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetJumpListVisible() == true )//#291
		{
			text.SetLocalizedText("JumpList_Hide");
		}
		else
		{
			text.SetLocalizedText("JumpList_Show");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_ShowJumpList,text);
	}
	*/
	//�s�ړ�
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_MoveLine,true);
	//���܂� #1194
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Minimize,true);
	//�t�H���g�T�C�Y
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSizeOther,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize60,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize80,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize90,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize95,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize100,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize105,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize110,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize120,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize140,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize160,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize180,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize240,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_RevertFontSize,true);
	AFloatNumber	fontsize = SPI_GetCurrentTabTextDocument().SPI_GetFontSize();
	//win GetApp().NVI_GetMenuManager().CheckSameNumberMenuItem(kMenuItemID_FontSize,fontsize);
	AArray<AMenuItemID>	itemIDArray;
	itemIDArray.Add(kMenuItemID_FontSize);
	itemIDArray.Add(kMenuItemID_FontSize60);
	itemIDArray.Add(kMenuItemID_FontSize80);
	itemIDArray.Add(kMenuItemID_FontSize90);
	itemIDArray.Add(kMenuItemID_FontSize95);
	itemIDArray.Add(kMenuItemID_FontSize100);
	itemIDArray.Add(kMenuItemID_FontSize105);
	itemIDArray.Add(kMenuItemID_FontSize110);
	itemIDArray.Add(kMenuItemID_FontSize120);
	itemIDArray.Add(kMenuItemID_FontSize140);
	itemIDArray.Add(kMenuItemID_FontSize160);
	itemIDArray.Add(kMenuItemID_FontSize180);
	itemIDArray.Add(kMenuItemID_FontSize240);
	GetApp().NVI_GetMenuManager().CheckSameNumberMenuItem(itemIDArray,fontsize);
	//�t�H���g
	//#688 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Font,true);
	//#688 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontRootMenu,true);
	//#688 AText	font;
	//win font.SetFormattedCstring("%d",SPI_GetCurrentTextDocument().SPI_GetFont());
	//#688 SPI_GetCurrentTabTextDocument().SPI_GetFontName(font);//win
	//#688 GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_Font,font);
	//TextEncoding
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetTextEncoding,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetTextEncodingRootMenu,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	AText	tecname;
	SPI_GetCurrentTabTextDocument().SPI_GetTextEncoding(tecname);
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_SetTextEncoding,tecname);
	//���s�R�[�h
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReturnCode_CR,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReturnCode_CRLF,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReturnCode_LF,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReturnCode_RootMenu,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_ReturnCode_CR,(SPI_GetCurrentTabTextDocument().SPI_GetReturnCode()==returnCode_CR));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_ReturnCode_CRLF,(SPI_GetCurrentTabTextDocument().SPI_GetReturnCode()==returnCode_CRLF));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_ReturnCode_LF,(SPI_GetCurrentTabTextDocument().SPI_GetReturnCode()==returnCode_LF));
	//�s����
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_NoWrap,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_WordWrap,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_WordWrapByLetterCount,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_LetterWrap,true);//#1113
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_LetterWrapByLetterCount,true);//#1113
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_RootMenu,true);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_WrapMode_NoWrap,(SPI_GetCurrentTabTextDocument().SPI_GetWrapMode()==kWrapMode_NoWrap));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_WrapMode_WordWrap,(SPI_GetCurrentTabTextDocument().SPI_GetWrapMode()==kWrapMode_WordWrap));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_WrapMode_WordWrapByLetterCount,(SPI_GetCurrentTabTextDocument().SPI_GetWrapMode()==kWrapMode_WordWrapByLetterCount));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_WrapMode_LetterWrap,(SPI_GetCurrentTabTextDocument().SPI_GetWrapMode()==kWrapMode_LetterWrap));//#1113
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_WrapMode_LetterWrapByLetterCount,(SPI_GetCurrentTabTextDocument().SPI_GetWrapMode()==kWrapMode_LetterWrapByLetterCount));//#1113
	//�t�H���g #868
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_UseModeFont,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_UseDocumentFont,true);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_UseModeFont,(SPI_GetCurrentTabTextDocument().SPI_GetUseModeFont()==true));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_UseDocumentFont,(SPI_GetCurrentTabTextDocument().SPI_GetUseModeFont()==false));
	//���[�h
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetMode,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetModeRootMenu,true);
	AText	modename;
	SPI_GetCurrentTabTextDocument().SPI_GetModeRawName(modename);
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_SetMode,modename);
	//�y�[�W�ݒ�
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_PageSetup,true);
	//���
	//#524 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Print,true);
	//����I�v�V����
	//#844 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowPrintOptionWindow,true);
	//�v���p�e�B�E�C���h�E
	//#844 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowDocPropertyWindow,true);
	//FTP�G�C���A�X�ۑ�
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SaveFTPAlias,(SPI_GetCurrentTabTextDocument().SPI_IsRemoteFileMode()==true));
	//�e�L�X�g�G���R�[�f�B���O�C��
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CorrectEncoding,(SPI_GetCurrentTabTextDocument().NVI_IsDirty()==false));
	//CrossCaret
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowHideCrossCaret,true);
		AText	text;
		if( IsCrossCaretMode() == true )
		{
			text.SetLocalizedText("CrossCaret_Hide");
		}
		else
		{
			text.SetLocalizedText("CrossCaret_Show");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_ShowHideCrossCaret,text);
	}
	//
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindEncodingProblem,true);
	//R0199
#if IMPLEMENTATION_FOR_WINDOWS
	//�X�y���`�F�b�N���Ή�
#else
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindMisspell,true);
#endif
	//
	//B0411 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WindowToTab,GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow));
	//B0411 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_TabToWindow,GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow));
	//
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_JumpExportToText,true);
	//
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_MoveToDefinitionRootMenu,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_MoveToDefinition,true);
	//
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SplitView,true);
	//#212
	AText	text;
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_DisplaySubPane,GetApp().SPI_IsSupportPaneMode());
		if( mLeftSideBarDisplayed == true )
		{
			text.SetLocalizedText("Menu_SubPane_Hide");
		}
		else
		{
			text.SetLocalizedText("Menu_SubPane_Show");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_DisplaySubPane,text);
	}
	//#291
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_DisplayInfoPane,GetApp().SPI_IsSupportPaneMode());
	{
		if( mRightSideBarDisplayed == true )
		{
			text.SetLocalizedText("Menu_InfoPane_Hide");
		}
		else
		{
			text.SetLocalizedText("Menu_InfoPane_Show");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_DisplayInfoPane,text);
	}
	//
	ABool	b = false;
	/*#137 ���ł�B0187�̕ύX�ňӖ����������Ȃ̂ŃR�����g�A�E�g AControlID	focus = NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
	*B0187 AIndex	viewIndex =* mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).FindFromTextViewControlID(focus);*/
	if( /*B0187 viewIndex > 0*/ mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetViewCount() >= 2 )
	{
		b = true;
	}
	//#137}
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ConcatView,b);
	
#if IMPLEMENTATION_FOR_WINDOWS
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Open,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_OpenInvisible,true);
#endif
	
	//�E�C���h�E���^�u���i�ł��^�u���������E�C���h�E�փ^�u������j
	b = true;
	/*#850
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == false )   b = false;
	else
	*/
	{
		//#850 if( SPI_IsTabModeMainWindow() == true )   b = false;
		if( GetApp().SPI_GetMostNumberTabsTextWindowID() == GetObjectID() )
		{
			//�������u�ł��^�u���������E�C���h�E�v�̏ꍇ�̓��j���[disable
			b = false;
		}
		else
		{
			if( NVI_IsChildWindowVisible() == true )   b = false;//B0411
			//B0411 �^�u�������̃E�C���h�E���q�E�C���h�E�\�����̓��j���[disable
			AWindowID	mainWindowID = GetApp().SPI_GetMostNumberTabsTextWindowID();//#850 SPI_GetTabModeMainTextWindowID();
			if( mainWindowID != kObjectID_Invalid )
			{
				if( GetApp().SPI_GetTextWindowByID(mainWindowID).NVI_IsChildWindowVisible() == true )   b = false;
			}
		}
	}
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WindowToTab,b);
	//�^�u���E�C���h�E��
	b = true;
	/*#850 
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == false )   b = false;
	else
	*/
	{
		//#850 if( SPI_IsTabModeMainWindow() == false )   b = false;
		if( NVI_IsChildWindowVisible() == true )   b = false;//B0411
		if( NVI_GetSelectableTabCount() < 2 )   b = false;//#850
	}
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_TabToWindow,b);
	/*
	//#92
	if( (mFindResultDocumentID!=kObjectID_Invalid) == true )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_IndexNext,true);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_IndexPrevious,true);
	}
	*/
	/*#724
	//#137
	AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( NVI_GetCurrentFocus() == searchBoxControlID && searchBoxControlID != kControlID_Invalid )
	*/
	if( GetTopMostFocusTextViewControlID() != kControlID_Invalid )
	{
		AFindParameter	param;
		GetApp().SPI_GetFindParam(param);
		b = (param.findText.GetItemCount()>0);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindNext,b);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindPrevious,b);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindFromFirst,b);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_EraseFindHighlight,SPI_GetCurrentFocusTextDocument().SPI_IsFindHighlightSet());
	}
	//#357
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchToNextTab,(NVI_GetSelectableTabCount()>=2));//#379
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchToPrevTab,(NVI_GetSelectableTabCount()>=2));//#379
	//#404
	//#476 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FullScreenMode,SPI_IsTabModeMainWindow());
	//#455
	ABool	canCommit = GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_CanCommit();
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SCMCommit,canCommit);
	//#379
	/*#899
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SubPaneMode_Manual,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SubPaneMode_PrevTab,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SubPaneMode_SameTab,true);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SubPaneMode_Manual,(mSubPaneMode==kSubPaneMode_Manual));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SubPaneMode_PrevTab,(mSubPaneMode==kSubPaneMode_PreviousDocument));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SubPaneMode_SameTab,(mSubPaneMode==kSubPaneMode_SameDocument));
	*/
	//#725
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_DisplaySubTextPane,true);
	//#688
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowHideToolbar,true);
	if( NVI_IsToolBarShown() == false )
	{
		text.SetLocalizedText("ShowToolBar");
	}
	else
	{
		text.SetLocalizedText("HideToolBar");
	}
	GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_ShowHideToolbar,text);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CustomizeToolbar,true);
	//#793
	//���E�t�@�C����r
	ADocumentID	mainDocID = NVI_GetCurrentDocumentID();
	ADocumentID	subDocID = SPI_GetCurrentSubTextPaneDocumentID();
	if( mainDocID != kObjectID_Invalid && subDocID != kObjectID_Invalid && mainDocID != subDocID )
	{
		//���C���E�T�u�����s�v�Z�������ǂ������擾
		ABool	wrapCalculateCompleted = ((GetApp().SPI_GetTextDocumentByID(mainDocID).SPI_IsWrapCalculating()==false)&&
										  (GetApp().SPI_GetTextDocumentByID(subDocID).SPI_IsWrapCalculating()==false));
		//�����s�v�Z�����Ȃ獶�E��renable
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareLeftFileAndRightFile,wrapCalculateCompleted);
	}
	
	//#725 �e�T�C�h�o�[
	for( AMenuItemID i = kMenuItemID_AddRecentlyOpenFileListInRightSideBar; i <= kMenuItemID_AddKeyRecordInRightSideBar; i++ )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(i,true);
	}
	for( AMenuItemID i = kMenuItemID_AddRecentlyOpenFileListInLeftSideBar; i <= kMenuItemID_AddKeyRecordInLeftSideBar; i++ )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(i,true);
	}
	
	//�g��^�k��
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Zoom,true);
	//�t���[�e�B���O�E�C���h�E�ꎞ�I��\���A�|�b�v�A�b�v�֎~
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowHideAllFloatingWindow,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_InhibitPopup,true);
	
	//==================�c�[���o�[enable/disable==================
	//�L�[�L�^�E�Đ�
	if( mToolbarExistArray.Find(kControlID_Toolbar_KeyRecordStart) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_KeyRecordStart,(GetApp().SPI_IsKeyRecording()==false));
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_KeyRecordStop) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_KeyRecordStop,(GetApp().SPI_IsKeyRecording()==true));
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_KeyRecordPlay) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_KeyRecordPlay,((GetApp().SPI_IsKeyRecording()==false)&&
						(GetApp().SPI_ExistRecordedText()==true)));
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_AddMacroBar) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_AddMacroBar,((GetApp().SPI_IsKeyRecording()==false)&&
						(GetApp().SPI_ExistRecordedText()==true)));
	}
	//�e�L�X�g�}�[�J�[
	if( mToolbarExistArray.Find(kControlID_Toolbar_FindHighlight) != kIndex_Invalid )
	{
		NVI_SetControlNumber(kControlID_Toolbar_FindHighlight,GetApp().SPI_GetCurrentTextMarkerGroupIndex());
	}
	//Nav�{�^��
	if( mToolbarExistArray.Find(kControlID_Toolbar_GoPrevious) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_GoPrevious,GetApp().SPI_GetCanNavigatePrev());
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_GoNext) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_GoNext,GetApp().SPI_GetCanNavigateNext());
	}
	//commit
	if( mToolbarExistArray.Find(kControlID_Toolbar_Commit) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_Commit,
							 ((GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).NVI_IsDirty()==false)&&canCommit));
	}
	//#1062
	//�^�u���A���t�@�x�b�g���Ƀ\�[�g����
	if( GetTabSelectorConst().SPI_GetDisplayTabCount() >= 2 )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SortTabs,true);
	}
}

//�E�C���h�E�̕���{�^��
void	AWindow_Text::EVTDO_DoCloseButton()
{
	//#513 win  �E�C���h�E��������ʃA�v���I���̉\��������̂ŁA�����ŕۑ�
	GetApp().SPI_SaveReopenFile();
	//
	/*
	if( AKeyWrapper::IsOptionKeyPressed() == true 
#if IMPLEMENTATION_FOR_WINDOWS
				|| AKeyWrapper::IsControlKeyPressed() == true 
#endif
				)
	{
		GetApp().NVI_CloseAllWindow();
	}
	else
	*/
	{
		//#510
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCloseAllTabsWhenCloseWindow) == true ||
			(AKeyWrapper::IsOptionKeyPressed() == true || AKeyWrapper::IsControlKeyPressed() == true) )
		{
			//�N���[�Y�{�^���őS�Ẵ^�u�����ݒ�
			//NVI_TryCloseAllTabs(false,false);//#667 closingAllWindows=false�Ƃ��邱�ƂŁA�ۑ����Ȃ��ŕ���ꍇ�ɑ��̃E�C���h�E�����Ă�������΍􂷂�B
			if( NVI_AskSaveForAllTabs() == true )
			{
				//�S�Ẵ^�u�ň�x���L�����Z������Ȃ������ꍇ�̂݁A�N���[�Y���s�B�i�L�����Z�����ꂽ��P�����Ȃ��j
				NVI_ExecuteClose();
			}
		}
		else
		{
			//�N���[�Y�{�^���Ń^�u�P�P�����ݒ�
			TryClose(NVI_GetCurrentTabIndex());
		}
	}
}

/**
AskSaveChanges�E�C���h�E�̉�����M����
�ʃN���[�Y�̏ꍇ�i�����[�_���ł͂Ȃ��ꍇ�j�̂݃R�[�������
*/
void	AWindow_Text::EVTDO_AskSaveChangesReply( const AObjectID inDocumentID, const AAskSaveChangesResult inAskSaveChangesReply )
{
	switch(inAskSaveChangesReply)
	{
		//�L�����Z��
	  case kAskSaveChangesResult_Cancel:
		{
			//��������
			break;
		}
		//�ۑ����Ȃ�
	  case kAskSaveChangesResult_DontSave:
		{
			ExecuteClose(inDocumentID);
			break;
		}
		//�ۑ�
	  case kAskSaveChangesResult_Save:
		{
			//B0325
			if( GetApp().SPI_GetTextDocumentByID(inDocumentID).NVI_IsFileSpecified() == false && 
						GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_IsRemoteFileMode() == false )
			{
				GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_Save(false);
			}
			else
			{
				GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_Save(false);
				ExecuteClose(inDocumentID);
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	mAskingSaveChanges = false;
}

//�ۑ��_�C�A���O������M����
void	AWindow_Text::EVTDO_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText )
{
	if( inRefText.Compare("FTPAlias") == true )
	{
		AText	text;
		GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_GetRemoteFileURL(text);
		inFile.WriteFile(text);
		AFileAttribute	attr;
		//win attr.creator = 'MMKE';
		//win attr.type = 'ftpa';
		//win AFileWrapper::SetFileAttribute(inFile,attr);
		GetApp().SPI_GetFTPAliasFileAttribute(attr);
		inFile.SetFileAttribute(attr);
	}
	//�^�u�Z�b�g #1050
	else if( inRefText.Compare("tabset") == true )
	{
		//�^�u���Ƃ̃t�@�C���p�X�����s��؂�Ŏ擾
		AText	text;
		AItemCount	tabDisplayCount = GetTabSelectorConst().SPI_GetDisplayTabCount();
		for( AIndex tabDisplayIndex = 0; tabDisplayIndex < tabDisplayCount; tabDisplayIndex++ )
		{
			AIndex	tabIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(tabDisplayIndex);
			ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
			AText	path;
			if( GetApp().SPI_GetTextDocumentByID(docID).SPI_IsRemoteFileMode() == true )
			{
				//�����[�g�t�@�C���Ȃ�URL�擾
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetRemoteFileURL(path);
			}
			else if( GetApp().SPI_GetTextDocumentByID(docID).NVI_IsFileSpecified() == true )
			{
				//���[�J���t�@�C���ispecify�ς݁j�Ȃ�t�@�C���p�X�擾
				GetApp().SPI_GetTextDocumentByID(docID).NVI_GetFilePath(path);
			}
			if( path.GetItemCount() > 0 )
			{
				text.AddText(path);
				text.Add(kUChar_LF);
			}
		}
		//�t�@�C���ۑ�
		inFile.WriteFile(text);
	}
	//�ʏ�h�L�������g
	else
	{
		GetApp().SPI_GetTextDocumentByID(inDocumentID).NVI_SpecifyFile(inFile);
		GetApp().SPI_AddToRecentlyOpenedFile(inFile);//#376
		GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_Save(false);
		//#376 GetApp().SPI_AddToRecentlyOpenedFile(inFile);
	}
}

/**
���T�C�Y����������
*/
void	AWindow_Text::EVTDO_WindowResizeCompleted( const ABool inResized )
{
	//#688
	//���ۂɃT�C�Y�ɕύX���Ȃ���΁A���������I���i���Ƃ��΃T�C�Y�{�b�N�X���N���b�N���������̏ꍇ�Ȃǂ́AinResized��false�ɂȂ�j
	if( inResized == false )
	{
		return;
	}
	//#506
	NVI_SetFullScreenMode(false);
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kFullScreenMode,false);
	//
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{      
		//�e�^�u��document�̌��݂̃��C��text view�����擾
		ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
		ANumber	oldTextViewWidth = SPI_GetTextViewWidth(docID);
		//#724 UpdateViewBounds()���ŁA�}�N���o�[�̈ʒu���Ĕz�u���邽�߂ɁA�ʒu�f�[�^���폜����
		mTabDataArray.GetObject(tabIndex).GetToolButtonPositionArray().DeleteAll();
		//View�̈ʒu�A�T�C�Y�X�V
		UpdateViewBounds(tabIndex);
		//�s�܂�Ԃ��Čv�Z
		//if( oldTextViewWidth != SPI_GetTextViewWidth(docID) )
		{
			//�s���Čv�Z�i�E�C���h�E���ς��̂Łj
			//Diff�Ώۃh�L�������g�͍s���Čv�Z���Ȃ��i�����A�T�u�y�C���̕��͌��܂��Ă��邪�A�T�u�y�C�����ɏ]��Ȃ��ݒ�̎��ɁA���̔���͏�����肪����j
			if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_IsDiffTargetDocument() == false )//#379
			{
				//�s�܂�Ԃ�����̏ꍇ�ɍČv�Z
				if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetWrapMode() != kWrapMode_NoWrap )
				{
					GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_DoWrapRelatedDataChanged(false);
				}
			}
		}
		//
		UpdateTextDrawProperty(tabIndex);
		//#212 tab���ɂ���K�v�͂Ȃ��̂ŁA���ֈړ�NVI_RefreshWindow();
	}
	NVI_RefreshWindow();//#212
	//B0415�ɂ�NVI_SetWindowBounds()��EVT_WindowBoundsChanged()���R�[�������悤�ɕύX�������߁A
	//kMenuItemID_CM_DetachTab_Left���s���ɁANVI_SetWindowBounds()���R�[�����ꂽ�Ƃ��A�����ɗ���悤�ɂȂ�A
	//�E�C���h�E��active�łȂ��Ƃ��ɁANVI_SelectTab���R�[������悤�ɂȂ��Ă��܂��������̌��ʁA�W�����v���j���[���\������Ă��܂��B
	//���������A������NVI_SelectTab���R�[������K�v�Ȃ��Ǝv����̂ŃR�����g�A�E�g
	//B0415 NVI_SelectTab(NVI_GetCurrentTabIndex(),true);
}

//#240
/**
�E�C���h�ESelect���s���O�ɃR�[�������
*/
void	AWindow_Text::EVTDO_DoWindowBeforeSelected()
{
	//�I�������Window�ɑΉ�����JumpList�ȊO��Hide�ɂ���
	//Acitive�ɂȂ����Ƃ��ɑ���Window��JumpList����u�����܂��\�����ꂽ��ԂɂȂ��Ă����Ƃ���������
	//#725 GetApp().SPI_HideOtherJumpList(SPI_GetJumpListWindow().GetObjectID());
	/*
	if( mFloatingJumpListWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_HideOtherFloatingJumpListWindows(mFloatingJumpListWindowID);
	}
	*/
	UpdateFloatingJumpListDisplay();
}

void	AWindow_Text::SPI_ShowMoveLineWindow()
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		ATextPoint	spt, ept;
		GetTextViewByControlID(focus).SPI_GetSelect(spt,ept);
		ABool	paraMode = true;//��ɒi�����[�h�ɂ���B�܂�Ԃ��s�̍s�ԍ��w��ňړ����邱�Ƃ͂Ȃ����߁B(SPI_GetTextDocument(focus).SPI_GetWrapMode() != kWrapMode_NoWrap);
		AIndex	lineIndex = spt.y;
		AItemCount	lineCount = SPI_GetTextDocument(focus).SPI_GetLineCount();
		if( paraMode == true )
		{
			lineIndex = SPI_GetTextDocument(focus).SPI_GetParagraphIndexByLineIndex(spt.y);
			lineCount = SPI_GetTextDocument(focus).SPI_GetParagraphCount();
		}
		/*#199 mMoveLineWindow*/SPI_GetMoveLieWindow().NVI_Create(kObjectID_Invalid);
		/*#199 mMoveLineWindow*/SPI_GetMoveLieWindow().SPNVI_Show(lineIndex,lineCount,paraMode);
		/*#199 mMoveLineWindow*/SPI_GetMoveLieWindow().NVI_SwitchFocusToFirst();
	}
}

ABool	AWindow_Text::IsCrossCaretMode() const
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		return GetTextViewConstByControlID(focus).SPI_IsCrossCaretMode();
	}
	return false;
}

#if IMPLEMENTATION_FOR_MACOSX
//B0370
ABool	AWindow_Text::EVTDO_DoServiceGetTypes( AArray<AScrapType>& outCopyTypes, AArray<AScrapType>& outPasteTypes )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		if( GetTextViewConstByControlID(focus).SPI_IsCaretMode() == false )
		{
			outCopyTypes.Add(kScrapType_UnicodeText);
			//#688 outCopyTypes.Add(kScrapType_Text);
		}
		if( SPI_GetTextDocument(focus).NVI_IsReadOnly() == false )
		{
			outPasteTypes.Add(kScrapType_UnicodeText);
			//#688 outPasteTypes.Add(kScrapType_Text);
		}
	}
	return true;
}

//B0370
ABool	AWindow_Text::EVTDO_DoServiceCopy( const AScrapRef inScrapRef )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		//�e�L�X�g���傫������ƁA���j���[�I�������ɃX���[�I�y���[�V�����ɂȂ�̂ŁA���̍s���ȏ�̏ꍇ�́A��̃e�L�X�g��Ԃ��B#695
		ATextPoint	spt = {0,0}, ept = {0,0};//#695
		GetTextViewConstByControlID(focus).SPI_GetSelect(spt,ept);//#695
		if( ept.y - spt.y > kLimit_ServiceCopy_MaxLineCount )   return true;//#695 10���s��葽���ꍇ�̓��j���[�I���ŃX���[�ɂȂ�̂�h������service copy�ł��Ȃ��悤�ɂ���
		//���ݑI�𒆂̃e�L�X�g��Ԃ�
		AText	text;
		GetTextViewConstByControlID(focus).SPI_GetSelectedText(text);
		AScrapWrapper::SetTextScrap(inScrapRef,text,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp));//#688 ,SPI_GetTextDocument(focus).SPI_GetPreferLegacyTextEncoding());//win
	}
	return true;
}

//B0370
ABool	AWindow_Text::EVTDO_DoServicePaste( const AScrapRef inScrapRef )
{
	//#212 ����if( SPI_GetCurrentTextDocument().NVI_IsReadOnly() == true )   return false;
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		if( SPI_GetTextDocument(focus).NVI_IsReadOnly() == true )   return false;
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		GetTextViewByControlID(focus).SPI_ServicePaste(inScrapRef);
	}
	return true;
}
#endif

//#137
/**
EditBox��Return�L�[���������Ƃ��̏���
*/
ABool	AWindow_Text::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )//#135
{
	//��
	/*#724
	//�����{�b�N�X
	AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( inControlID == searchBoxControlID )
	{
		FindNextWithSearchBox();
	}
	*/
	return true;//#135
}

//#137
/**
EditBox��Tab�L�[���������Ƃ��̏���
*/
ABool	AWindow_Text::NVIDO_ViewTabKeyPressed( const AControlID inControlID )//#135
{
	//��
	/*#724
	//�����{�b�N�X
	AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( inControlID == searchBoxControlID )
	{
		//TextView�փt�H�[�J�X�ړ�
		AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
		if( lastFocusedTextView != kControlID_Invalid )
		{
			NVI_SwitchFocusTo(lastFocusedTextView);
		}
	}
	*/
	return true;//#135
}

//#137
/**
EditBox��ESC�L�[���������Ƃ��̏���
*/
ABool	AWindow_Text::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )//#135
{
	//��
	/*#724
	//�����{�b�N�X
	AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( inControlID == searchBoxControlID )
	{
		//TextView�փt�H�[�J�X�ړ�
		AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
		if( lastFocusedTextView != kControlID_Invalid )
		{
			NVI_SwitchFocusTo(lastFocusedTextView);
		}
	}
	*/
	return true;//#135
}

//#137
/**
�����{�b�N�X�ɂ�錟��
*/
void	AWindow_Text::FindNextWithSearchBox()
{
	//
	AControlID	searchBoxControlID = kControlID_Toolbar_SearchField;//#724 mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( searchBoxControlID == kControlID_Invalid )   return;
	//�����p�����[�^�擾
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	AText	text;
	NVI_GetControlText(searchBoxControlID,text);
	if( text.GetItemCount() == 0 )//#1114   return;//#724
	{
		//#1114
		//�����t�B�[���h����A���A�C�x���g�̃e�L�X�g��ESC�̏ꍇ�A�e�L�X�g�r���[�փt�H�[�J�X��߂�
		//�iESC�L�[�����������AOS�ɂ���Č����t�B�[���h����ɂȂ��Ă���B�j
		AText	eventText;
		NSString*	str = [[NSApp currentEvent] charactersIgnoringModifiers];
		if( str != nil )
		{
			eventText.SetFromNSString(str);
		}
		if( eventText.GetItemCount() == 1 )
		{
			if( eventText.Get(0) == 0x1B )
			{
				if( mLatentTextViewContolID != kControlID_Invalid )
				{
					NVI_SwitchFocusTo(mLatentTextViewContolID);
				}
			}
		}
		return;
	}
	param.findText.SetText(text);
	//�����E�C���h�E�ɂ�����������Ƃ��ēo�^
	GetApp().SPI_SetFindText(text);
	//�Ō�̃t�H�[�J�XTextView�Ō������s
	//AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
	if( mLatentTextViewContolID != kControlID_Invalid )
	{
		if( IsTextViewControlID(mLatentTextViewContolID) == true )
		{
			GetTextViewByControlID(mLatentTextViewContolID).SPI_FindNext(param);
		}
	}
}

//#268
/**
�����{�b�N�X�ɂ�錟���i�O�j
*/
void	AWindow_Text::FindPreviousWithSearchBox()
{
	AControlID	searchBoxControlID = kControlID_Toolbar_SearchField;//#724 mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( searchBoxControlID == kControlID_Invalid )   return;
	//�����p�����[�^�擾
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	AText	text;
	NVI_GetControlText(searchBoxControlID,text);
	if( text.GetItemCount() == 0 )   return;//#724
	param.findText.SetText(text);
	//�����E�C���h�E�ɂ�����������Ƃ��ēo�^
	GetApp().SPI_SetFindText(text);
	//�Ō�̃t�H�[�J�XTextView�Ō������s
	//AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
	if( mLatentTextViewContolID != kControlID_Invalid )
	{
		if( IsTextViewControlID(mLatentTextViewContolID) == true )
		{
			GetTextViewByControlID(mLatentTextViewContolID).SPI_FindPrevious(param);
		}
	}
}

//#137
/**
�����{�b�N�X�Ɍ��������������
*/
void	AWindow_Text::SPI_SetSearchBoxText( const AText& inText )
{
	//#725
	AControlID	searchBoxControlID = kControlID_Toolbar_SearchField;//mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( searchBoxControlID == kControlID_Invalid )   return;
	NVI_SetControlText(searchBoxControlID,inText);
}

//#232
/**
CrossCaret�\���E��\���؂�ւ�
*/
void	AWindow_Text::SPI_ShowHideCrossCaret()
{
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(tabIndex).GetViewCount(); viewIndex++ )
	{
		GetTextViewByControlID(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(viewIndex)).SPI_ShowHideCrossCaret();
	}
}

//#291
/**
�Z�p���[�^�ړ�
@return ���ۂɈړ�����delta
*/
ANumber	AWindow_Text::NVIDO_SeparatorMoved( const AWindowID inSeparatorWindowID, const AControlID inID, 
		const ANumber inDelta, const ABool inCompleted )//#409
{
	//
	if( inCompleted == false )
	{
		NVI_SetVirtualLiveResizing(true);
	}
	else
	{
		NVI_SetVirtualLiveResizing(false);
	}
	//
	if( inSeparatorWindowID == mRightSideBarVSeparatorWindowID )
	{
		return ChangeRightSideBarWidth(inDelta,inCompleted);//#409
	}
	else if( inSeparatorWindowID == mLeftSideBarVSeparatorWindowID )
	{
		return ChangeLeftSideBarWidth(inDelta,inCompleted);//#409
	}
	//#725 �T�u�e�L�X�g�J����
	else if( inSeparatorWindowID == mSubTextColumnVSeparatorWindowID )
	{
		return ChangeSubTextPaneColumnWidth(inDelta,inCompleted);
	}
	//#634 �T�C�h�o�[VSeparator�{�^���ɂ��ړ�
	else if( inSeparatorWindowID == GetObjectID() && inID == kControlID_RightSideBar_SeparatorButton )
	{
		return ChangeRightSideBarWidth(inDelta,inCompleted);
	}
	//#725
	else if( inSeparatorWindowID == GetObjectID() && inID == kControlID_LeftSideBar_SeparatorButton )
	{
		return ChangeLeftSideBarWidth(inDelta,inCompleted);
	}
	//
	else
	{
		for( AIndex i = 0; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			if( mRightSideBarArray_OverlayWindowID.Get(i) == inSeparatorWindowID )
			{
				return ChangeRightSideBarHeight(i,inDelta);
			}
		}
		for( AIndex i = 0; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			if( mLeftSideBarArray_OverlayWindowID.Get(i) == inSeparatorWindowID )
			{
				return ChangeLeftSideBarHeight(i,inDelta);
			}
		}
		//#866
		//�e�L�X�g�r���[HSeparator���ړ�
		//�Y��HSeparator�������A��v������text view�����ύX
		for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
		{
			AIndex	viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromSeparatorControlID(inID);
			if( viewIndex != kIndex_Invalid )
			{
				return ChangeTextViewHeight(tabIndex,viewIndex,inDelta,inCompleted);
			}
		}
	}
	return inDelta;
}

//#291
/**
�Z�p���[�^�_�u���N���b�N
*/
void	AWindow_Text::NVIDO_SeparatorDoubleClicked( const AWindowID inSeparatorWindowID, const AControlID inID )
{
	if( inSeparatorWindowID == mRightSideBarVSeparatorWindowID )
	{
		ShowHideRightSideBar(!mRightSideBarDisplayed);
	}
	else if( inSeparatorWindowID == mLeftSideBarVSeparatorWindowID )
	{
		ShowHideLeftSideBar(!mLeftSideBarDisplayed);
	}
}

//#602
/**
�w���v�^�O���擾�R�[���o�b�N
*/
ABool	AWindow_Text::EVTDO_DoGetHelpTag( const AControlID inID, const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	outTagSide = kHelpTagSide_Default;//#643
	ABool	result = false;
	switch(inID)
	{
	  case kSubPaneDiffButtonControlID:
		{
			AText	text;
			if( mDiffDisplayMode == false )
			{
				text.SetLocalizedText("HelpTag_ShowDiffSubPaneText");
			}
			else
			{
				text.SetLocalizedText("HelpTag_HideDiffSubPaneText");
			}
			outText1.SetText(text);
			result = true;
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return result;
}


//#389 #859
/**
�^�u���E�C���h�E��
*/
void	AWindow_Text::SPI_TabToWindow( const AIndex inTabIndex, const ABool inWindowPositionDirected, const APoint inPoint )
{
	AWindowDefaultFactory<AWindow_Text>	factory;//#175 (&GetApp()); #199
	AObjectID	winID = GetApp().NVI_CreateWindow(factory);
	//#859 AIndex	tabIndex = NVI_GetCurrentTabIndex();
	SPI_CopyTabToWindow(inTabIndex,winID);
	if( inWindowPositionDirected == true )
	{
		GetApp().SPI_GetTextWindowByID(winID).NVI_SetWindowPosition(inPoint);
	}
	SPI_CloseTab(inTabIndex);
	GetApp().SPI_GetTextWindowByID(winID).SPI_UpdateViewBounds();//#291
	GetApp().NVI_GetWindowByID(winID).NVI_SelectWindow();
}

//#389
/**
�ł��^�u���̑����E�C���h�E�ցA�S�Ă̑I���\�^�u���ړ�
*/
void	AWindow_Text::SPI_WindowToTab()
{
	//�ł��^�u���̑����E�C���h�E���擾
	AWindowID	tabWindowID = GetApp().SPI_GetMostNumberTabsTextWindowID();
	if( tabWindowID == kObjectID_Invalid )   return;
	
	//�E�C���h�E�̑S�Ẵ^�u�����C���E�C���h�E�փR�s�[���āA�^�u��S�ĕ���i���E�C���h�E�����j
	{
		//���E�C���h�E�Ƃ��A�u���b�N���ł�update view bounds��}��
		AStSuppressTextWindowUpdateViewBounds	s1(tabWindowID);
		AStSuppressTextWindowUpdateViewBounds	s2(GetObjectID());
		//�I���\�ȃ^�u��S�āA�Ŕw�ʂ̃^�u���珇�ɁA�R�s�[
		while( NVI_GetSelectableTabCount() > 0 )
		{
			AIndex	mostBackTabIndex = NVI_GetMostBackTabIndex();//z-order�Ō�̃^�u
			GetApp().SPI_GetTextWindowByID(tabWindowID).SPNVI_CopyCreateTab(GetObjectID(), mostBackTabIndex);//�^�u���R�s�[�������A���̃^�u���폜
		}
	}
	//view bounds�X�V
	GetApp().SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();//#291
}

//#389
/**
�^�u���E�C���h�E���i���ֈړ��j
*/
void	AWindow_Text::SPI_DetachTab_Left( const AIndex tabIndex )
{
	ABool	active = false;
	if( tabIndex == NVI_GetCurrentTabIndex() )   active = true;
	AWindowDefaultFactory<AWindow_Text>	factory;//#175 (&(GetApp())); #199
	AObjectID	winID = GetApp().NVI_CreateWindow(factory);
	SPI_CopyTabToWindow(tabIndex,winID);
	SPI_CloseTab(tabIndex);
	ANumber	offset = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kTabToWindowPositionOffset);
	/*#668
	ARect	rect;
	GetApp().NVI_GetWindowByID(winID).NVI_GetWindowBounds(rect);
	rect.left -= offset;
	rect.right -= offset;
	GetApp().NVI_GetWindowByID(winID).NVI_SetWindowBounds(rect);
	*/
	//SPI_CopyTabToWindow()�ł̓E�C���h�E�ʒu�E�T�C�Y�͂��ꂼ��̃h�L�������g�ݒ�ɏ]���悤�ɂ����̂ŁA
	//�ʒu�������^�u�E�C���h�E����̃I�t�Z�b�g�ʒu�ɐݒ肷�� #668
	APoint	pt = {0,0};
	NVI_GetWindowPosition(pt);
	pt.x -= offset;
	GetApp().NVI_GetWindowByID(winID).NVI_SetWindowPosition(pt);
	//
	//#919 ���݂̃E�C���h�E�̔w�i�ɉB���Ƃǂꂾ���킩��Ȃ��Ȃ�̂ŁA��ɍőO�ʂɂ���Bif( active == true )
	{
		GetApp().NVI_GetWindowByID(winID).NVI_SelectWindow();
	}
	//#370 �ʒu�␳
	GetApp().SPI_GetTextWindowByID(winID).NVI_ConstrainWindowPosition(false);//�����I�ɉB���ꍇ���ʒu�␳
}

//#389
/**
�^�u���E�C���h�E���i�E�ֈړ��j
*/
void	AWindow_Text::SPI_DetachTab_Right( const AIndex tabIndex )
{
	ABool	active = false;
	if( tabIndex == NVI_GetCurrentTabIndex() )   active = true;
	AWindowDefaultFactory<AWindow_Text>	factory;//#175 (&(GetApp())); #199
	AObjectID	winID = GetApp().NVI_CreateWindow(factory);
	SPI_CopyTabToWindow(tabIndex,winID);
	SPI_CloseTab(tabIndex);
	ANumber	offset = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kTabToWindowPositionOffset);
	/*#668
	ARect	rect;
	GetApp().NVI_GetWindowByID(winID).NVI_GetWindowBounds(rect);
	rect.left += offset;
	rect.right += offset;
	GetApp().NVI_GetWindowByID(winID).NVI_SetWindowBounds(rect);
	*/
	//SPI_CopyTabToWindow()�ł̓E�C���h�E�ʒu�E�T�C�Y�͂��ꂼ��̃h�L�������g�ݒ�ɏ]���悤�ɂ����̂ŁA
	//�ʒu�������^�u�E�C���h�E����̃I�t�Z�b�g�ʒu�ɐݒ肷�� #668
	APoint	pt = {0,0};
	NVI_GetWindowPosition(pt);
	pt.x += offset;
	GetApp().NVI_GetWindowByID(winID).NVI_SetWindowPosition(pt);
	//
	//#919 ���݂̃E�C���h�E�̔w�i�ɉB���Ƃǂꂾ���킩��Ȃ��Ȃ�̂ŁA��ɍőO�ʂɂ���Bif( active == true )
	{
		GetApp().NVI_GetWindowByID(winID).NVI_SelectWindow();
	}
	//#370 �ʒu�␳
	GetApp().SPI_GetTextWindowByID(winID).NVI_ConstrainWindowPosition(false);//�����I�ɉB���ꍇ���ʒu�␳
}

//#389
/**
�^�u���E�C���h�E��
*/
void	AWindow_Text::SPI_DetachTab( const AIndex tabIndex )
{
	ARect	bounds;
	NVI_GetWindowBounds(bounds);
	AGlobalRect	screenBounds;
	//#688 CWindowImp::GetAvailableWindowPositioningBounds(screenBounds);
	NVI_GetAvailableWindowBoundsInSameScreen(screenBounds);//#688
	if( (bounds.left+bounds.right)/2 < (screenBounds.left+screenBounds.right)/2 )
	{
		SPI_DetachTab_Right(tabIndex);
	}
	else
	{
		SPI_DetachTab_Left(tabIndex);
	}
}

//win
/**
����_�C�A���O�\��
*/
/*#524
void	AWindow_Text::SPI_Print()
{
	AText	title;
	NVI_GetCurrentTitle(title);
	SPI_GetCurrentTabTextDocument().NVI_Print(GetObjectID(),title);
}
*/

//#438
/**
�C���f�b�N�X�E�C���h�E�E��
*/
/*#725
void	AWindow_Text::SPI_IndexWindowNext()
{
	if( mFindResultViewID != kObjectID_Invalid )
	{
		AView_Index::GetIndexViewByViewID(mFindResultViewID).SPI_SelectNextRow();
	}
	else
	{
		GetApp().SPI_GoNextIndexWindowItem();
	}
}
*/

//#438
/**
�C���f�b�N�X�E�C���h�E�E�O
*/
/*#725
void	AWindow_Text::SPI_IndexWindowPrev()
{
	if( mFindResultViewID != kObjectID_Invalid )
	{
		AView_Index::GetIndexViewByViewID(mFindResultViewID).SPI_SelectPreviousRow();
	}
	else
	{
		GetApp().SPI_GoPrevIndexWindowItem();
	}
}
*/

//#112
/**
�t�@�C���I��������
*/
void	AWindow_Text::EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile )
{
	switch(inControlID)
	{
	  case kVirtualControlID_CompareFileChoosen:
		{
			//��r�t�@�C���I��������
			SPI_GetCurrentFocusTextDocument().SPI_SetDiffMode_File(inFile,kCompareMode_File);
			SPI_SetDiffDisplayMode(true);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

/*#1091-test
void	AWindow_Text::EVTDO_DoTickTimerAction()
{
	if( mDefferedFocusTick > 0 )
	{
		mDefferedFocusTick--;
		if( mDefferedFocusTick == 0 )
		{
			NVI_SwitchFocusTo(mDefferedFocus);
			fprintf(stderr,"mDefferedFocusTick(%d)  ",mDefferedFocus);
		}
	}
}
*/

#pragma mark ===========================

#pragma mark ---�ʃR�}���h����

//���j���[����u����v�I���A�������́A�E�C���h�E�̕���{�^���N���b�N���AClose��Try����B�iDirty�Ȃ�AskSaveChanges�E�C���h�E���J���j
void	AWindow_Text::TryClose( const AIndex inTabIndex )//win 080709 ABool��void�i�Ԃ�l���g�p�̂��߁j
{
	//AskSaveChanges�E�C���h�E�\�����Ȃ牽���������^�[��
	if( mAskingSaveChanges == true )    return;//win 080709 false;
	
	//���݂̃h�L�������g��Dirty���E�C���h�E�P���ǂ������f
	ADocumentID	closingDocID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	if( GetApp().SPI_GetTextDocumentByID(closingDocID).NVI_IsDirty() == true && 
				//#379 GetApp().SPI_GetTextDocumentByID(closingDocID).SPI_GetWindowCount() <= 1 )
				GetApp().SPI_GetTextDocumentByID(closingDocID).SPI_ExistAnotherWindow(GetObjectID()) == false )//#379
	{
		//AskSaveChanges�E�C���h�E���J���āA�ۑ����邩�ǂ�����q�˂�
		AText	filename;
		GetApp().SPI_GetTextDocumentByID(closingDocID).NVI_GetTitle(filename);
		mAskingSaveChanges = true;//win 080709
		NVI_ShowAskSaveChangesWindow(filename,closingDocID);
		//win 080709��ֈړ� mAskingSaveChanges = true;
		NVI_SelectWindow();
		//return false;
	}
	else
	{
		//Close���s
		ExecuteClose(closingDocID);
		//return true;
	}
}

//Close���s
void	AWindow_Text::ExecuteClose( const AObjectID inClosingDocumentID )
{
	//�W�����v���j���[���̉�����
	GetApp().NVI_GetMenuManager().UnrealizeDynamicMenu(GetApp().SPI_GetTextDocumentByID(inClosingDocumentID).SPI_GetJumpMenuObjectID());
	//�c�[�����j���[�X�V
	if( NVI_GetCurrentDocumentID() == inClosingDocumentID )//#988
	{
		GetApp().SPI_UpdateToolMenu(kIndex_Invalid);
	}
	
	//globals->mHookManager->CloseHook();
	
	//B0000 �e��V�[�g�E�C���h�E�������N���[�Y�i���j���[����̓N���[�Y�ł��Ȃ����AAppleScript����̃N���[�Y�͖����ł��Ȃ��j
	/*OSX10.4�ŃN���b�V������̂ł�߂�BNavDialogDispose()��NG�H
	if( mAskingSaveChanges == true )
	{
		NVM_GetImp().CloseAskSaveChanges();
		mAskingSaveChanges = false;
	}*/
	if( /*#199 mMoveLineWindow*/SPI_GetMoveLieWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mMoveLineWindow*/SPI_GetMoveLieWindow().NVI_Close();
	}
	//#844 if( /*#199 mFontSizeWindow*/SPI_GetFontSizeWindow().NVI_IsWindowVisible() == true )
	//#844 {
	//#844 	/*#199 mFontSizeWindow*/SPI_GetFontSizeWindow().NVI_Close();
	//#844 }
	if( /*#199 mWrapLetterCountWindow*/SPI_GetWrapLetterCountWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mWrapLetterCountWindow*/SPI_GetWrapLetterCountWindow().NVI_Close();
	}
	if( /*#199 mTextEncodingCorrectWindow*/SPI_GetCorrectEncodingWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mTextEncodingCorrectWindow*/SPI_GetCorrectEncodingWindow().NVI_Close();
	}
	//#844 if( /*#199 mPrintOptionWindow*/SPI_GetPrintOptionWindow().NVI_IsWindowVisible() == true )
	//#844 {
	//#844 	/*#199 mPrintOptionWindow*/SPI_GetPrintOptionWindow().NVI_Close();
	//#844 }
	//#844 if( /*#199 mDocPropertyWindow*/SPI_GetPropertyWindow().NVI_IsWindowVisible() == true )
	//#844 {
	//#844 	/*#199 mDocPropertyWindow*/SPI_GetPropertyWindow().NVI_Close();
	//#844 }
	if( /*#199 mDropWarningWindow*/SPI_GetDropWarningWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mDropWarningWindow*/SPI_GetDropWarningWindow().NVI_Close();
	}
	if( /*#199 mAddToolButtonWindow*/SPI_GetAddToolButtonWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mAddToolButtonWindow*/SPI_GetAddToolButtonWindow().NVI_Close();
	}
	if( /*#199 mEditByOtherAppWindow*/SPI_GetEditByOtherAppWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mEditByOtherAppWindow*/SPI_GetEditByOtherAppWindow().NVI_Close();
	}
	//#455
	if( SPI_GetSCMCommitWindow().NVI_IsWindowVisible() == true )
	{
		SPI_GetSCMCommitWindow().NVI_Close();
	}
	/* #138 if( mExternalCommentWindow.NVI_IsWindowVisible() == true )//RC3
	{
		mExternalCommentWindow.NVI_Close();
	}*/
	if( NVI_IsChildWindowVisible() == true )
	{
		NVI_CloseChildWindow();
	}
	
	//#379 ���Ă���Œ���Diff�`��h�~�̂���Diff���[�h���������Ă���
	//Tab<->Window�؂�ւ����ɔ�r�{�^��disable�ɂȂ��Ă��܂��̂ŃR�����g�A�E�gGetApp().SPI_GetTextDocumentByID(inClosingDocumentID).SPI_SetDiffMode(kDiffTargetMode_None);
	//#379 ���悤�Ƃ��Ă���h�L�������g�����L����h�L�������g�̃^�u���N���[�Y
	AArray<ADocumentID>	docIDArray;
	GetApp().SPI_GetTextDocumentByID(inClosingDocumentID).SPI_GetOwnDocIDArray(docIDArray);
	for( AIndex i = 0; i < docIDArray.GetItemCount(); i++ )
	{
		//���悤�Ƃ��Ă���h�L�������g�����L����h�L�������g�����ۂɂ��̃E�C���h�E��ɕ\������Ă��邩�ǂ����𔻒f�B
		//�i1�h�L�������g2�E�C���h�E�Ή�(#856)�������ƂŁA�K���������L�h�L�������g�����̃E�C���h�E��ɕ\������Ă���Ƃ͌���Ȃ��Ȃ������߁j
		ADocumentID	docID = docIDArray.Get(i);
		if( NVI_GetTabIndexByDocumentID(docID) != kIndex_Invalid )
		{
			//���L�h�L�������g�ɂ���close���s
			ExecuteClose(docID);
		}
	}
	
	//doc pref��ۑ�
	GetApp().SPI_GetTextDocumentByID(inClosingDocumentID).SPI_SaveDocPrefIfLoaded();
	
	//#212
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inClosingDocumentID);
	DeleteSubTextInEachTab(tabIndex);
	
	//#212
	UpdateSubTextFileNameButton();
	
	//�W�����v���X�g�̊Y���^�u�폜 #291
	//#725 SPI_GetJumpListWindow().NVI_DeleteTabAndView(tabIndex);
	GetApp().SPI_CloseJumpListWindowsTab(GetObjectID(),inClosingDocumentID);//#725
	
	//#0 Delete�����ƃ����o�[�f�[�^�ւ̃A�N�Z�X���ۏ؂ł��Ȃ��Ȃ�̂Ő�ɕۑ����Ă���
	AObjectID	winID = GetObjectID();
	
	/*#725 tab�ɑ�����overlay window�́ANVI_DeleteTabAndView()�ō폜����B
	//ViewCloseOverlay�E�C���h�E��S�č폜���� win
	//SPI_DeleteSplitView()���R�[�����邱�Ƃ������������A����A�ق��̃f�[�^��NVI_DeleteTabAndView()�ō폜������肾���A
	//NVI_DeleteTabAndView()����UpdateViewBounds()�����R�[�����Ă���̂ŁA�ʂɍ폜����B
	for( AIndex v = 0; v < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); v++ )
	{
		GetApp().NVI_DeleteWindow(mTabDataArray.GetObjectConst(tabIndex).GetCloseViewButtonWindowID(v));
	}
	*/
	
	//�^�u�{����r���[�폜�i������NVIDO_DeleteTab()���Ă΂��j
	NVI_DeleteTabAndView(NVI_GetTabIndexByDocumentID(inClosingDocumentID));
	
	//�^�u��0�ɂȂ�����E�C���h�E�폜
	if( NVI_GetTabCount() == 0 )
	{
		/*#725
		//FindViewResultCloseBUtton���폜 win
		if( mFindResultCloseButtonWindowID != kObjectID_Invalid )
		{
			GetApp().NVI_DeleteWindow(mFindResultCloseButtonWindowID);
			mFindResultCloseButtonWindowID = kObjectID_Invalid;
		}
		*/
		
		//�t���[�e�B���O�̃W�����v���X�g�E�C���h�E�����ioverlay��jumplist�͉��ŉE�T�C�h�o�[�J�����A�T�u�y�C���J�����폜���ɍ폜�����B�j
		//#725 SPI_GetJumpListWindow().NVI_Close();
		if( mFloatingJumpListWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_DeleteSubWindow(mFloatingJumpListWindowID);
			mFloatingJumpListWindowID = kObjectID_Invalid;
		}
		
		//#291 �eInfo�E�C���h�E�̃I�[�o�[���C���[�h������
		//�������E�C���h�E���B���B�폜�����̂́ADoDeleted()�̃^�C�~���O�i�C�x���g�g�����U�N�V�����I�����j�B#291 #212
		if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
		{
			GetApp().NVI_GetWindowByID(mLeftSideBarVSeparatorWindowID).NVI_Hide();
		}
		if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )
		{
			GetApp().NVI_GetWindowByID(mRightSideBarVSeparatorWindowID).NVI_Hide();
		}
		//#725 �T�u�e�L�X�g�J������������\��
		if( mSubTextColumnVSeparatorWindowID != kObjectID_Invalid )
		{
			GetApp().NVI_GetWindowByID(mSubTextColumnVSeparatorWindowID).NVI_Hide();
		}
		/*#725 �e�T�u�E�C���h�E�͉��ō폜����B
		if( SPI_IsTabModeMainWindow() == true )//#377
		{
			//�I�[�o�[���C�E�C���h�E�͕��邱�ƂɂȂ�̂ŁA�����ɉB�� #338
			if( GetApp().SPI_GetIdInfoWindow().NVI_GetOverlayMode() == true )
			{
				GetApp().SPI_GetIdInfoWindow().NVI_Hide();
			}displa
			if( GetApp().SPI_GetFileListWindow().NVI_GetOverlayMode() == true )
			{
				GetApp().SPI_GetFileListWindow().NVI_Hide();
			}
		}
		*/
		
		//#725
		//�E�T�C�h�o�[�A�T�u�y�C���폜
		DeleteRightSideBar();
		DeleteLeftSideBar();
		
		//#338 ��Ƀe�L�X�g�E�C���h�E�����S�ɍ폜���Ă���A�I�[�o�[���C�E�C���h�E���t���[�e�B���O�ɖ߂��B
		NVI_Close();
		GetApp().NVI_DeleteWindow(GetObjectID());
		//�E�C���h�E���j���[�X�V
		//#922 GetApp().SPI_UpdateWindowMenu();
		/*#725 IdInfoWindow, FileListWindow���t���[�e�B���O�ɖ߂����Ƃ͂����Ȃ��B�i���InfoPane, SubPane�폜�j
		if( SPI_IsTabModeMainWindow() == true )//#377
		{
			//���ʎq���E�C���h�E���t���[�e�B���O�ɖ߂�
			RevertFromOverlay_IdInfoWindow();
			//�t�@�C�����X�g�E�C���h�E���t���[�e�B���O�ɖ߂�
			RevertFromOverlay_FileListWindow();
		}
		*/
		//#338 ��Ɉړ��B
		//#338 ���L�����̑O��RevertFromOverlay_IdInfoWindow()�����s����ƁA�t�@�C�����X�g�i�E�C���h�E���X�g�j���č쐬����Ƃ��A
		//#338 �e�L�X�g�E�C���h�E�����r���[�ɑ��݂����ԂȂ̂ŁA�E�C���h�E�^�C�g���擾���ɃC���f�b�N�X�G���[����������B
		//#338 �Ȃ��A���̕ύX������ƁA�I�[�o�[���C�̐e�E�C���h�E���ɍ폜���邱�ƂɂȂ邽�߁ACWindowImp�ɂ��Ή���ǉ������B
		//NVI_Close();
		//GetApp().NVI_DeleteWindow(GetObjectID());
	}
	//�^�u���܂��c���Ă���ꍇ
	else//#212 �t�H�[�J�X��V��������Tab�̍ŏ���TextView�ɐݒ�
	{
		//������switch focus�����NSView<NSTextInputClient>��removeFromSuperview����Ƃ��Ɏ��Ԃ�������B
		//�i�A�N�e�B�r�e�B���j�^�̃T���v�����O�ł́ATSM��activate�Ɏ��Ԃ��������Ă���悤�Ɍ�����B�j
		//AWindow::NVI_DeleteTabAndView()�ɂāA�A�N�e�B�u�ȃ^�u���폜�����Ƃ��́A�V���ȃ^�u��I���������Ă���A
		//NVI_SelectTab()���Ń^�u���t�H�[�J�X���ݒ肳���̂ŁA�����ł�switch focus�͕K�v�Ȃ����߁A�폜�B
		//fprintf(stderr,"SWITCH(%d,%d)  ",NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0));
		//#688 NVI_SwitchFocusTo(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0));
		
		//win �^�u������Ƃ��ɃT�u�y�C���ɕ\���������Ȃ�����A���݂̃^�u�̓��e���T�u�y�C���ɕ\������
		if( /*#725 mLeftSideBarDisplayed == true &&*/ mSubTextCurrentDocumentID == kObjectID_Invalid )
		{
			SPI_DisplayInSubText(NVI_GetCurrentTabIndex(),false,kIndex_Invalid,false,false);
		}
	}
	
	//�h�L�������g�փE�C���h�E�i�^�u�j������ꂽ���Ƃ�ʒm����
	//#379 GetApp().SPI_GetTextDocumentByID(inClosingDocumentID).OWICB_DoWindowClosed(winID);//#0 GetObjectID());
	
	//�E�C���h�E���j���[�X�V
	//#922 GetApp().SPI_UpdateWindowMenu();
	//�t�@�C�����X�g�E�C���h�E�X�V
	//#725 GetApp().SPI_UpdateFileListWindow_DocumentOpened(inClosingDocumentID);
	GetApp().SPI_UpdateFileListWindows(kFileListUpdateType_DocumentClosed,inClosingDocumentID);//#725
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void	AWindow_Text::NVIDO_Create( const ADocumentID inDocumentID )
{
	const ANumber	kWindowPositionOffset = 20;//B0394
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AWindow_Text::NVIDO_Create() started.",this);
	NVM_CreateWindow(kWindowClass_Document);
	//�c�[���o�[����
	NVI_CreateToolbar(0);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("Window Imp created.",this);
	
	/*#725 �R���X�g���N�^�ֈړ�
	//#668 �T�u�y�C���\����Ԃ̐ݒ�
	if( SPI_IsTabModeMainWindow() == true )
	{
		//���C���^�u�E�C���h�E�̏ꍇ��apppref�����Ԏ擾
		mLeftSideBarDisplayed = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kLeftSideBarDisplayed);
	}
	else
	{
		//���C���^�u�E�C���h�E�ȊO�́A������Ԃ̓T�u�y�C����\��
		mLeftSideBarDisplayed = false;
	}
	*/
	//==================�E�C���h�E�T�C�Y�A�ʒu�ݒ� ==================
	//@note ���C���J�����̕��̈ʒu�A�T�C�Y
	ARect	bounds = {0,0,300,300};//B0394
	/*#850
	if( SPI_IsTabModeMainWindow() == true )//#668 GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true )
	{
		//�^�u���C���E�C���h�E�̏ꍇ
		
		//�^�u���[�h�̃E�C���h�E�T�C�Y��AppPref����擾
		GetApp().GetAppPref().GetData_Rect(AAppPrefDB::kSingleWindowBounds,bounds);
	}
	else
	{
	//�^�u���[�h�łȂ��ꍇ
	*/
	
	/*B0344 if( GetApp().SPI_GetModePrefDB(GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_GetModeIndex()).
	GetData_Bool(AModePrefDB::kSaveWindowPosition) == true )
	{*/
	if( mInitialWindowBoundsIsSet == true )//#850
	{
		//------------Initial Bounds�ݒ�L��̏ꍇ�i��reopen�̏ꍇ�j------------
		
		bounds = mInitialWindowBounds;
	}
	else
	{
		//------------Initial Bounds�ݒ薳���̏ꍇ�i��reopen�ȊO�̏ꍇ�j------------
		
		//DocPref����ʒu�f�[�^�ǂݍ���
		APoint	pt = {0,0};
		GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Point(ADocPrefDB::kWindowPosition,pt);
		//DocPref����T�C�Y�f�[�^�ǂݍ��� #1238
		ANumber	width = GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowWidth);
		ANumber	height = GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowHeight);
		//DocPref�ւ̈ʒu�f�[�^�ݒ�L�肩�ǂ����𔻒�
		if( GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().IsStillDefault(ADocPrefDB::kWindowPosition) == true ||
					(pt.x==-1&&pt.y==-1) )
		{
			//------------DocPref�̐ݒ薳���A�܂��́A�^�u�E�C���h�E���ɕۑ����ꂽ(��-1,-1�ɂȂ��Ă���)�ꍇ------------
			
			//�őO�ʃE�C���h�E����̑��Έʒu�A�������́A�X�N���[���̍���
			AWindowID	frontWindowID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( frontWindowID != kObjectID_Invalid )
			{
				//------------DocPref�����E�őO�ʃe�L�X�g�E�C���h�E�L��̏ꍇ------------
				
				//�őO�ʃE�C���h�E����̑��Έʒu���擾
				ARect	frontWindowBounds;
				GetApp().SPI_GetTextWindowByID(frontWindowID).SPI_GetWindowMainColumnBounds(frontWindowBounds);//#850
				pt.x 	= frontWindowBounds.left + kWindowPositionOffset;
				pt.y	= frontWindowBounds.top + kWindowPositionOffset;
			}
			else
			{
				//------------DocPref�����E�őO�ʃe�L�X�g�E�C���h�E�����̏ꍇ------------
				
				//���j���[�o�[�Edock���������X�N���[��bounds�̍���ʒu���擾
				AGlobalRect	screenbounds = {0};
				AWindow::NVI_GetAvailableWindowBoundsInMainScreen(screenbounds);
				pt.x 	= screenbounds.left +16;
				pt.y	= screenbounds.top;
				
				//�u�h�L�������g�̓^�u�ɊJ���v��ON�̏ꍇ�A�E�C���h�E�͌Œ�ʒu�ɊJ���i�O��������̃E�C���h�E�ʒu�j
				if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true )
				{
					GetApp().GetAppPref().GetData_Point(AAppPrefDB::kNewDocumentWindowPoint,pt);
				}
			}
			
			//DocPref�̐ݒ�Ȃ��̏ꍇ�́A�T�C�Y�f�[�^��kSingleWindowBounds����擾 #1238
			ARect	b = {0};
			GetApp().GetAppPref().GetData_Rect(AAppPrefDB::kSingleWindowBounds,b);
			width = b.right - b.left;
			height = b.bottom - b.top;
		}
		//�V�K�E�C���h�Ebounds�擾
		bounds.left 	= pt.x;
		bounds.top 		= pt.y;
		bounds.right 	= bounds.left + width;//GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowWidth);
		bounds.bottom 	= bounds.top + height;//GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowHeight);
	}
	/*}
	else
	{
	ARect	screenRect;
	CWindowImp::GetAvailableWindowPositioningBounds(screenRect);
	ANumber	offset = GetApp().NVI_GetDocumentCount(kDocumentType_Text)*kNewWindowPositionOffset;
	bounds.left 	= screenRect.left+offset;
	bounds.top 		= screenRect.top+offset+kWindowTitleBarHeight;
	bounds.right 	= bounds.left + GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowWidth);
	bounds.bottom 	= bounds.top +  GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowHeight);
	}*/
	//#850 }
	//#567 LuaConsole
	if( GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_GetLuaConsoleMode() == true )
	{
		//LuaConsole���[�h�ݒ�
		mLuaConsoleMode = true;
		
		//�T�u�y�C���E�E�T�C�h�o�[��\��
		mLeftSideBarDisplayed = false;
		mRightSideBarDisplayed = false;
		
		//LuaConsole�pbounds��ݒ�
		APoint	pt = {0,0};
		GetApp().NVI_GetAppPrefDB().GetData_Point(AAppPrefDB::kLuaConsoleWindowPosition,pt);
		bounds.left		= pt.x;
		bounds.top		= pt.y;
		bounds.right	= bounds.left +
						GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kLuaConsoleWindowWidth);
		bounds.bottom	= bounds.top +
						GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kLuaConsoleWindowHeight);
	}
	
	//#476 �E�C���h�E���ُ�ɏ������Ȃ����Ƃ��ُ̈�n�΍�
	if( bounds.right - bounds.left < 100 )   bounds.right = bounds.left + 100;
	if( bounds.bottom - bounds.top < 100 )   bounds.bottom = bounds.top + 100;
	//�E�C���h�E�ʒu�E�T�C�Y�ݒ�
	SPI_SetWindowBoundsByMainColumnPosition(bounds);//#850
	//win �ő剻��ԕ���
#if IMPLEMENTATION_FOR_WINDOWS
//Mac��NVI_Zoom()�ɖ��Ή��Ȃ̂łƂ肠����#if�Ő؂��Ă���
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kSingleWindowBounds_Zoomed) == true )
	{
		GetApp().GetAppPref().GetData_Rect(AAppPrefDB::kSingleWindowBounds_UnzoomedBounds,bounds);
		NVI_SetWindowBounds(bounds);
		NVI_Zoom();
	}
#endif
	
	//#385 �E�C���h�E�T�C�Y�␳�i�T�C�Y�{�b�N�X�������Ȃ��Ƒ���ɖ����l��������������̂Łj
	NVI_ConstrainWindowPosition(true);//�����I�ɉB��Ă��Ă�OK
	NVI_ConstrainWindowSize();
	
	//==================�Œ�View����==================
	
	//window bounds�擾
	ARect	windowbounds = {0};
	NVI_GetWindowBounds(windowbounds);
	
	//�w�i�F�`��pview����
	if( true )
	{
		//�E���̃X�N���[���o�[������镔���̂ݔw�i�`�悷��
		AWindowPoint	pt = {-10000,-10000};//#1090
		NVI_CreatePlainView(kControlID_WindowBackground,pt,kVScrollBarWidth+8,kHScrollBarHeight+8);
		NVI_SetShowControl(kControlID_WindowBackground,true);
		NVI_SetControlBindings(kControlID_WindowBackground,false,false,true,true,true,true);
		NVI_SetPlainViewColor(kControlID_WindowBackground,kColor_White,kColor_White,false,false,false,false,kColor_White);
	}
	else
	{
		//�E�C���h�E�S�̂�w�i�`��
		AWindowPoint	pt = {-10000,-10000};//#1090
		NVI_CreatePlainView(kControlID_WindowBackground,pt,windowbounds.right-windowbounds.left,windowbounds.bottom-windowbounds.top);
		NVI_SetShowControl(kControlID_WindowBackground,true);
		NVI_SetControlBindings(kControlID_WindowBackground,true,true,true,true,false,false);
		NVI_SetPlainViewColor(kControlID_WindowBackground,kColor_White,kColor_White,false,false,false,false,kColor_White);
	}
	//�E�T�C�h�o�[�w�i
	{
		AWindowPoint	pt = {-10000,-10000};//#1090
		NVI_CreatePlainView(kControlID_RightSideBarBackground,pt,10,10);
		NVI_SetShowControl(kControlID_RightSideBarBackground,false);
		NVI_SetControlBindings(kControlID_RightSideBarBackground,false,true,true,true,true,false);
	}
	//���T�C�h�o�[�w�i
	{
		AWindowPoint	pt = {-10000,-10000};//#1090
		NVI_CreatePlainView(kControlID_LeftSideBarBackground,pt,10,10);
		NVI_SetShowControl(kControlID_LeftSideBarBackground,false);
		NVI_SetControlBindings(kControlID_LeftSideBarBackground,true,true,false,true,true,false);
	}
	
	//�i�������T�C�Y�A�ʒu�́ASPI_UpdateViewBounds()�Őݒ肳���B�j
	AWindowPoint	pt = {-10000,-10000};//#1090
	//H�X�N���[���o�[����
	//#725 NVI_CreateScrollBar(kControlID_HScrollBar,pt,20,10);
	
	//win
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);//#375
	//�s�ԍ��{�^������ #232 �{�^����AView_Button��
	NVI_CreateButtonView(kControlID_LineNumberButton,pt,kLineNumberButtonWidth,kHeight_Footer-2,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Left,
				kTextStyle_DropShadow,kColor_Black,kColor_Black);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton).SPI_SetDropShadowColor(kColor_Gray80Percent);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,3);
	//�ꏊ�{�^������ #232 �{�^����AView_Button��
	NVI_CreateButtonView(kControlID_PositionButton,pt,kPositionButtonWidth,kHeight_Footer-2,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_PositionButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_PositionButton).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Left,
				kTextStyle_DropShadow,kColor_Black,kColor_Black);
	NVI_GetButtonViewByControlID(kControlID_PositionButton).SPI_SetDropShadowColor(kColor_Gray80Percent);
	NVI_GetButtonViewByControlID(kControlID_PositionButton).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,5);
	NVI_GetButtonViewByControlID(kControlID_PositionButton).SPI_SetIcon(kImageID_iconSwList,0,2,18);
	
	//#725
	//�T�u�e�L�X�g�̍s�ԍ��{�^������
	NVI_CreateButtonView(kControlID_LineNumberButton_SubText,pt,kLineNumberButtonWidth,kHeight_Footer-2,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton_SubText).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton_SubText).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Left,
				kTextStyle_DropShadow,kColor_Black,kColor_Black);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton_SubText).SPI_SetDropShadowColor(kColor_Gray80Percent);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton_SubText).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,3);
	//�T�u�e�L�X�g�̏ꏊ�{�^������
	NVI_CreateButtonView(kControlID_PositionButton_SubText,pt,kPositionButtonWidth,kHeight_Footer-2,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Left,
				kTextStyle_DropShadow,kColor_Black,kColor_Black);
	NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).SPI_SetDropShadowColor(kColor_Gray80Percent);
	NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,5);
	NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).SPI_SetIcon(kImageID_iconSwList,0,2,18);
	
	//�^�u�Z���N�^
	AViewDefaultFactory<AView_TabSelector>	tabSelectorFactory(GetObjectID(),kControlID_TabSelector);//#199
	NVM_CreateView(kControlID_TabSelector,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,tabSelectorFactory);
	
	//#725
	//�t�b�^�[
	{
		//
		AViewDefaultFactory<AView_Footer>	factory(GetObjectID(),kControlID_Footer);
		NVM_CreateView(kControlID_Footer,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,factory);
	}
	/*
	{
		//
		AViewDefaultFactory<AView_FooterSeparator>	factory(GetObjectID(),kControlID_FooterSeparator_MainColumn);
		NVM_CreateView(kControlID_FooterSeparator_MainColumn,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,factory);
	}
	{
		//
		AViewDefaultFactory<AView_FooterSeparator>	factory(GetObjectID(),kControlID_FooterSeparator_MainColumn2);
		NVM_CreateView(kControlID_FooterSeparator_MainColumn2,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,factory);
	}
	{
		//
		AViewDefaultFactory<AView_FooterSeparator>	factory(GetObjectID(),kControlID_FooterSeparator_SubTextColumn);
		NVM_CreateView(kControlID_FooterSeparator_SubTextColumn,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,factory);
	}
	{
		//
		AViewDefaultFactory<AView_FooterSeparator>	factory(GetObjectID(),kControlID_FooterSeparator_SubTextColumn2);
		NVM_CreateView(kControlID_FooterSeparator_SubTextColumn2,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,factory);
	}
	*/
	
	//#725
	//�v���O���Xindicator
	{
		//progress indicator����
		NVI_CreateProgressIndicator(kControlID_MainColumnProgressIndicator,pt,kWidth_ProgressIndicator,kHeight_ProgressIndicator);
		//progress text����
		AWindowPoint	pt = {-10000,-10000};//#1090
		NVI_CreateButtonView(kControlID_MainColumnProgressText,pt,10,kHeight_ProgressIndicator,kControlID_Invalid);
		NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).SPI_SetButtonViewType(kButtonViewType_NoFrame);
		NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).SPI_SetTextProperty(defaultfontname,9.0,kJustification_Left,
					kTextStyle_DropShadow,kColor_Black,kColor_Black);
		NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).SPI_SetDropShadowColor(kColor_Gray80Percent);
		
	}
	
	/*#246
	//Toolbar
	AViewDefaultFactory<AView_Toolbar>	toolbarFactory(GetObjectID(),kControlID_ToolbarPlacard);//#199
	NVM_CreateView(kControlID_ToolbarPlacard,pt,10,10,false,toolbarFactory);
	*/
	/*#725 ���w�b�_�p�~
	//InfoHeader
	AViewDefaultFactory<AView_InfoHeader>	infoHeaderFactory(GetObjectID(),kControlID_InfoHeader);//#199
	NVM_CreateView(kControlID_InfoHeader,pt,10,10,kControlID_Invalid,false,infoHeaderFactory);
	*/
	/*#246
	//Ruler
	AViewDefaultFactory<AView_Ruler>	rulerFactory(GetObjectID(),kControlID_Ruler);//#199
	NVM_CreateView(kControlID_Ruler,pt,10,10,false,rulerFactory);
	GetRulerView().SPI_UpdateImageSize();
	*/
	//======================�E�T�C�h�o�[����{�^��======================
	//�T�C�h�o�[VSeparator�{�^��
	NVI_CreateButtonView(kControlID_RightSideBar_SeparatorButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_SeparatorButton).SPI_SetButtonViewType(kButtonViewType_NoFrame);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_SeparatorButton).SPI_SetIcon(kImageID_iconPnHandle,8,3);//#725
	//#725 NVI_GetButtonViewByControlID(kControlID_RightSideBar_SeparatorButton).SPI_SetIcon(kIconID_Mi_VSeparator,1,1);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_SeparatorButton).SPI_SetButtonBehaviorType(kButtonBehaviorType_VSeparator);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_SeparatorButton).SPI_SetButtonValidXRange(0,32);
	//�T�C�h�o�[���ݒ�{�^��
	NVI_CreateButtonView(kControlID_RightSideBar_PrefButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_PrefButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_PrefButton).SPI_SetIcon(kImageID_iconPnOption,10,5,10);//#725
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_PrefButton).SPI_SetMenu(kContextMenuID_RightSideBarPref,kMenuItemID_Invalid);//#725
	//�E�T�C�h�o�[�\����\���{�^��
	NVI_CreateButtonView(kControlID_RightSideBar_ShowHideButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_ShowHideButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_ShowHideButton).SPI_SetIcon(kImageID_iconPnRight,8,3);//#725
	
	//======================���T�C�h�o�[����{�^��======================
	//�T�C�h�o�[VSeparator�{�^��
	NVI_CreateButtonView(kControlID_LeftSideBar_SeparatorButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_SeparatorButton).SPI_SetButtonViewType(kButtonViewType_NoFrame);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_SeparatorButton).SPI_SetIcon(kImageID_iconPnHandle,-20,3);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_SeparatorButton).SPI_SetButtonBehaviorType(kButtonBehaviorType_VSeparator);
	//�T�C�h�o�[���ݒ�{�^��
	NVI_CreateButtonView(kControlID_LeftSideBar_PrefButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_PrefButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_PrefButton).SPI_SetIcon(kImageID_iconPnOption,10,5,10);//#725
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_PrefButton).SPI_SetMenu(kContextMenuID_LeftSideBarPref,kMenuItemID_Invalid);//#725
	//�E�T�C�h�o�[�\����\���{�^��
	NVI_CreateButtonView(kControlID_LeftSideBar_ShowHideButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_ShowHideButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_ShowHideButton).SPI_SetIcon(kImageID_iconPnLeft,8,3);//#725
	
#if IMPLEMENTATION_FOR_WINDOWS
	//sizebox
	NVI_CreateSizeBox(kControlID_SizeBox,pt,kVScrollBarWidth,kHScrollBarHeight);
#endif
	
	AText	text;
	//==================�T�u�e�L�X�g�֘A #212==================
	/*#379
	//�T�u�e�L�X�g���[�h�{�^��
	NVI_CreateButtonView(kSubPaneModeButtonControlID,pt,kSubPaneModeButtonWidth,kSubPaneModeButtonHeight);
	NVI_GetButtonViewByControlID(kSubPaneModeButtonControlID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);
	NVI_SetShowControl(kSubPaneModeButtonControlID,false);
	SetSubPaneMode(mSubPaneMode);//�{�^���e�L�X�g�X�V�̂���
	*/
	//�T�u�e�L�X�g�t�@�C�����{�^��
	NVI_CreateButtonView(kSubPaneFileNameButtonControlID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetButtonViewType(kButtonViewType_Rect20);
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center,kTextStyle_Normal);
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetIcon(kImageID_iconSwDocument,5,3);
	NVI_SetShowControl(kSubPaneFileNameButtonControlID,false);
	//�T�u�e�L�X�g�����{�^��
	NVI_CreateButtonView(kSubPaneSyncButtonControlID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).SPI_SetButtonViewType(kButtonViewType_Rect20);
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center,kTextStyle_Normal);
	NVI_SetShowControl(kSubPaneSyncButtonControlID,false);
	text.SetLocalizedText("SubPaneSyncButton");
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).NVI_SetText(text);
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).SPI_SetIcon(kImageID_iconSwShow,5,2);
	//�T�u�e�L�X�g���փ{�^��
	NVI_CreateButtonView(kSubPaneSwapButtonControlID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).SPI_SetButtonViewType(kButtonViewType_Rect20);
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center,kTextStyle_Normal);
	NVI_SetShowControl(kSubPaneSwapButtonControlID,false);
	text.SetLocalizedText("SubPaneSwapButton");
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).NVI_SetText(text);
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).SPI_SetIcon(kImageID_iconSwReplace,5,2);
	//�T�u�e�L�X�gDiff�{�^�� #379
	NVI_CreateButtonView(kSubPaneDiffButtonControlID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetButtonViewType(kButtonViewType_Rect20);
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center,kTextStyle_Normal);
	NVI_SetShowControl(kSubPaneDiffButtonControlID,false);
	text.SetLocalizedText("SubPaneDiffButton");
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).NVI_SetText(text);
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetIcon(kImageID_iconSwCompare,5,2);
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetToggleMode(true);
	NVI_EnableHelpTagCallback(kSubPaneDiffButtonControlID);
	
	//==================�J�����Z�p���[�^==================
	//�T�u�e�L�X�gVSeparator Overlay�E�C���h�E���� #219
	if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_ChangeToOverlay(GetObjectID(),true);//win
		NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_Create(kObjectID_Invalid);
		NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).SPI_SetTargetWindowID(GetObjectID());
		NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).SPI_SetLinePosition(kSeparatorLinePosition_None);
		NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_Hide();
		//#725 NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_SetWindowTransparency(1.0);//#634 (0.5);
		//#725 NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).SPI_SetTransparency(0.1);//#634
	}
	//�E�T�C�h�o�[VSeparator Overlay�E�C���h�E���� #212
	if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_ChangeToOverlay(GetObjectID(),true);//win
		NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_Create(kObjectID_Invalid);
		NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).SPI_SetTargetWindowID(GetObjectID());
		NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).SPI_SetLinePosition(kSeparatorLinePosition_None);
		NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_Hide();
		//#725 NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_SetWindowTransparency(1.0);//#634 0.5);
		//#725 NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).SPI_SetTransparency(0.1);//#634
	}
	//#725 �T�u�e�L�X�g�J����VSeparator overlay�E�C���h�E����
	if( mSubTextColumnVSeparatorWindowID != kObjectID_Invalid )
	{
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_ChangeToOverlay(GetObjectID(),true);//win
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_Create(kObjectID_Invalid);
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).SPI_SetTargetWindowID(GetObjectID());
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).SPI_SetLinePosition(kSeparatorLinePosition_Right);
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_Hide();
		//#725 NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_SetWindowTransparency(1.0);
		//#725 NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).SPI_SetTransparency(0.1);//#634
	}
	
	//==================�w���v�^�O==================
	{
		//#602
		AText	text;
		text.SetLocalizedText("HelpTag_SwapSubPaneText");
		NVI_SetHelpTag(kSubPaneSwapButtonControlID,text);
		text.SetLocalizedText("HelpTag_DisplayToSubPaneText");
		NVI_SetHelpTag(kSubPaneSyncButtonControlID,text);
		//#634
		text.SetLocalizedText("HelpTag_SideBarAppPrefButton");
		NVI_SetHelpTag(kControlID_RightSideBar_PrefButton,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_SideBarShowHideInfoPaneButton");
		NVI_SetHelpTag(kControlID_RightSideBar_ShowHideButton,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_SideBarShowHideLeftSideBarButton");
		NVI_SetHelpTag(kControlID_LeftSideBar_ShowHideButton,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_SideBarInfoPaneVSeparatorButton");
		NVI_SetHelpTag(kControlID_RightSideBar_SeparatorButton,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_LineButton");
		NVI_SetHelpTag(kControlID_LineNumberButton,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_JumpButton");
		NVI_SetHelpTag(kControlID_PositionButton,text,kHelpTagSide_Top);
		
		//#725
		text.SetLocalizedText("HelpTag_LeftSideBar_VSeparatorButton");
		NVI_SetHelpTag(kControlID_LeftSideBar_SeparatorButton,text,kHelpTagSide_Top);
		
		//#725
		text.SetLocalizedText("HelpTag_LineButton");
		NVI_SetHelpTag(kControlID_LineNumberButton_SubText,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_JumpButton");
		NVI_SetHelpTag(kControlID_PositionButton_SubText,text,kHelpTagSide_Top);
	}
	
	//==================�T�C�h�o�[����==================
	
	//#725 SPI_UpdateSubPaneArray();
	//���T�C�h�o�[
	if( mLeftSideBarDisplayed == true )
	{
		CreateLeftSideBar();
		mLeftSideBarCreated = true;
	}
	
	//#725 SPI_UpdateInfoPaneArray();
	//�E�T�C�h�o�[
	if( mRightSideBarDisplayed == true )
	{
		CreateRightSideBar();
		mRightSideBarCreated = true;
	}
	
	//==================diff info�I�[�o�[���C�E�C���h�E==================
	//#379 Diff���\���E�C���h�E���� 
	AWindowDefaultFactory<AWindow_DiffInfo>	diffInfoWindowFactory;
	mDiffInfoWindowID = GetApp().NVI_CreateWindow(diffInfoWindowFactory);
#if IMPLEMENTATION_FOR_WINDOWS
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_SetWindowTransparency(0.1);
#endif
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_ChangeToOverlay(GetObjectID(),true);
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Create(kObjectID_Invalid);
	/*#1255
#if IMPLEMENTATION_FOR_MACOSX
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_SetWindowTransparency(0.1);
#endif
*/
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Hide();
	(dynamic_cast<AWindow_DiffInfo&>(GetApp().NVI_GetWindowByID(mDiffInfoWindowID))).SPI_SetTextWindowID(GetObjectID());
	
	//==================�T�u�e�L�X�g�V���h�E==================
	//#725
	{
		AWindowDefaultFactory<AWindow_SubTextShadow>	factory;
		mSubTextShadowWindowID = GetApp().NVI_CreateWindow(factory);
#if IMPLEMENTATION_FOR_WINDOWS
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_SetWindowTransparency(0.1);
#endif
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_ChangeToOverlay(GetObjectID(),true);
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_Create(kObjectID_Invalid);
		/*#1255
#if IMPLEMENTATION_FOR_MACOSX
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_SetWindowTransparency(0.1);
#endif
*/
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_Hide();
	}
	
	//==================�c�[���o�[==================
	//#724
	//toolbar���ڑ��݃e�[�u���X�V
	UpdateToolbarExistArray();
	//�c�[���o�[�̃��j���[��ݒ�
	if( mToolbarExistArray.Find(kControlID_Toolbar_TextEncoding) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_TextEncoding,kTextArrayMenuID_TextEncoding);
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_ReturnCode) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_ReturnCode,kTextArrayMenuID_ReturnCode);
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_WrapMode) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_WrapMode,kTextArrayMenuID_WrapMode);
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_Mode) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_Mode,kTextArrayMenuID_EnabledMode);
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_SubWindows) != kIndex_Invalid )
	{
		NVI_SetControlMenu(kControlID_Toolbar_SubWindows,GetApp().NVI_GetMenuManager().GetContextMenu(kContextMenuID_SubWindows));
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_NewDocument) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_NewDocument,kTextArrayMenuID_EnabledMode);
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_Pref) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_Pref,kTextArrayMenuID_Pref);
	}
	/*#0 ���g�p�̂��߃R�����g�A�E�g
	if( mToolbarExistArray.Find(kControlID_Toolbar_Diff) != kIndex_Invalid )
	{
		NVI_SetControlMenu(kControlID_Toolbar_Diff,GetApp().NVI_GetMenuManager().GetContextMenu(kContextMenuID_Diff));
	}
	*/
	if( mToolbarExistArray.Find(kControlID_Toolbar_FindHighlight) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_FindHighlight,kTextArrayMenuID_TextMarker);
	}
	
	//==================�W�����v���X�g==================
	//���ݒ�ŕ\�����ON�i���ŏI�I�ɕ\��������ԁj�Ȃ�A�f�t�H���g�Ńt���[�e�B���O�W�����v���X�g�\��
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowFloatingJumpList) == true )
	{
		SPI_ShowHideJumpListWindow(true);
	}
	
	//==================���̑�==================
	
	//
	///*#199 mJumpListWindow*/SPI_GetJumpListWindow().NVI_Create(kObjectID_Invalid);
	
	//
	//mInfoWindow.NVI_Create(kObjectID_Invalid);
	//mInfoWindow.NVI_Show();
	
	//R0240
	NVI_EnableHelpTagCallback(kControlID_TabSelector);
	
	//#404 �t���X�N���[�����[�h�K�p
	//#476 if( mTabModeMainWindow == true )
	{
		ABool	fullScreenMode = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFullScreenMode);
		NVI_SetFullScreenMode(fullScreenMode);
	}
	
	//#602
	UpdateVSeparatorHelpTag();
	
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AWindow_Text::NVIDO_Create() ended.",this);
}

//#602
/**
�J������؂���̃w���v�^�O�X�V
*/
void	AWindow_Text::UpdateVSeparatorHelpTag()
{
	AText	text;
	if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		if( mLeftSideBarDisplayed == true )
		{
			text.SetLocalizedText("HelpTag_SubPane_Hide");
		}
		else
		{
			text.SetLocalizedText("HelpTag_SubPane_Show");
		}
		GetApp().NVI_GetWindowByID(mLeftSideBarVSeparatorWindowID).NVI_SetHelpTag(
					GetApp().NVI_GetWindowByID(mLeftSideBarVSeparatorWindowID).NVI_GetMainControlID(),text);
	}
	if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		if( mRightSideBarDisplayed == true )
		{
			text.SetLocalizedText("HelpTag_InfoPane_Hide");
		}
		else
		{
			text.SetLocalizedText("HelpTag_InfoPane_Show");
		}
		GetApp().NVI_GetWindowByID(mRightSideBarVSeparatorWindowID).NVI_SetHelpTag(
					GetApp().NVI_GetWindowByID(mRightSideBarVSeparatorWindowID).NVI_GetMainControlID(),text);
	}
}

/* B0402 NVMDO_DoTabActivated()��UpdateViewBounds()���R�[�������̂�����w��̃^�u�ɂ���UpdateViewBounds()����K�v�͂Ȃ�
void	AWindow_Text::SPI_UpdateViewBoundsAll( const AModeIndex inModeIndex )
{
	GetTabSelector().SPI_UpdateProperty();//R0177
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			UpdateViewBounds(tabIndex);
		}
	}
}
*/

//#379
/**
ViewBounds�X�V�iinDocumentID�ɑΉ�����^�u�݂̂��X�V�j
*/
void	AWindow_Text::SPI_UpdateViewBounds( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex == kIndex_Invalid )   return;
	
	GetTabSelector().SPI_UpdateProperty();
	UpdateViewBounds(tabIndex);
}

void	AWindow_Text::SPI_UpdateViewBounds()
{
	GetTabSelector().SPI_UpdateProperty();//R0177 B0402 SPI_UpdateViewBoundsAll()����ړ�
	UpdateViewBounds(NVI_GetCurrentTabIndex());
}

/**
�eControl�̈ʒu�E�T�C�Y�EShow/Hide����

#246 ��ʃ��T�C�Y�����ɁA�S�^�u�ɑ΂��ď���UpdateViewBounds()���s�����Ƃ�����̂ŁA
�^�u����Control��Control����̓^�u���A�N�e�B�u�̏ꍇ�ȊO�͂���Ă͂����Ȃ�
�܂��A�^�u����Control�ł͂Ȃ��Ă��AShow/Hide�̐���̓^�u���A�N�e�B�u�̏ꍇ�ȊO�͂���Ă͂����Ȃ�
*/
void	AWindow_Text::UpdateViewBounds( const AIndex inTabIndex, const AUpdateViewBoundsTriggerType inTriggerType )
{
	//
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	return UpdateViewBounds(inTabIndex,windowBounds,inTriggerType);//#341 #645
}
void	AWindow_Text::UpdateViewBounds( const AIndex inTabIndex, const ARect& windowBounds, 
		const AUpdateViewBoundsTriggerType inTriggerType )//#341 windowBounds���w��ł���悤�ɕύX
{
	//�E�C���h�E���E�����擾
	ANumber	windowWidth = windowBounds.right - windowBounds.left;
	ANumber	windowHeight = windowBounds.bottom - windowBounds.top;
	
	//#675 UpdateViewBounds�}����Ԓ��͉������Ȃ�
	if( mSuppressUpdateViewBoundsCounter > 0 )   return;
	
	//�`��X�V���~�i�������j
    ACocoa::DisableScreenUpdates();
	
	/*#725 NVM_RegisterOverlayWindow()�ɂĎq�I�[�o�[���C�E�C���h�E�o�^���邱�ƂŁAAWindow�Ŏ����I��show/hide���邱�Ƃɂ������߁A�폜
	//ViewClose�{�^��Overlay��S��Hide����B#675 DoTabActivated()����ړ�
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
		{
			GetApp().NVI_GetWindowByID(mTabDataArray.GetObject(tabIndex).GetCloseViewButtonWindowID(v)).NVI_Hide();
		}
	}
	*/
	
	//#634
	ABool	panemodesupported = GetApp().SPI_IsSupportPaneMode();
	//���݂̃^�u�̃��[�h�擾
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	
	//�t���O��
	ABool	subTextPaneDisplayed = false;//#320
	
	//=============================================
	//�@�@�T�u�E�C���h�EShow/Hide�X�V
	//=============================================
	//�T�u�E�C���h�E��Show/Hide�X�V #725
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		UpdateVisibleSubWindows();
	}
	
	//=============================================
	//�@�@���C�A�E�g�v�Z
	//=============================================
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(inTabIndex,layout);
	
	//=============================================
	//�E�C���h�E�w�iview�̔z�u
	//=============================================
	if( true )
	{
		//�X�N���[���o�[��������Ă��镔�������\��
		if( layout.h_MainHScrollbar > 0 || mRightSideBarDisplayed == false )
		{
			AWindowPoint	pt = {0,0};
			pt.x = layout.pt_VScrollbarArea.x -8;
			pt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight - layout.h_Footer -8;
			NVI_SetPlainViewColor(kControlID_WindowBackground,kColor_White,kColor_White,false,!mRightSideBarDisplayed,false,false,kColor_White);
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_WindowBackground,pt);
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_WindowBackground,true);
		}
		else
		{
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_WindowBackground,false);
		}
	}
	else
	{
		//�E�C���h�E�S�̕\��
		AWindowPoint	pt = {0,0};
		pt.x = 0;
		pt.y = layout.h_MacroBar + layout.h_TabSelector;
		NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_WindowBackground,pt);
		NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_WindowBackground,windowWidth,windowHeight
									   - (layout.h_MacroBar + layout.h_TabSelector + layout.h_Footer));
	}
	
	//=============================================
	//�@�@�}�N���o�[�z�u
	//=============================================
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		UpdateLayout_MacroBar(inTabIndex,layout.pt_MacroBar,layout.w_MacroBar,layout.h_MacroBar);
	}
	
	//=============================================
	//�@�@�T�u�e�L�X�g�J�����z�u
	//=============================================
	//�T�u�e�L�X�g�J�����E�������z�u�i�T�u�e�L�X�g�J���������݂��Ă���ꍇ�̂݁j #725
	subTextPaneDisplayed = UpdateLayout_SubTextColumn(inTabIndex,layout);
	
	//=============================================
	//�@�@�^�u�Z���N�^�z�u
	//=============================================
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		if( layout.h_TabSelector > 0 )
		{
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_TabSelector,layout.pt_TabSelector);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_TabSelector,layout.w_TabSelector,layout.h_TabSelector);
		}
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_TabSelector,(layout.h_TabSelector>0));
	}
	
	//=============================================
	//�@�@���[���[�z�u
	//=============================================
	if( true )
	{
		AControlID	controlID = mTabDataArray.GetObjectConst(inTabIndex).GetRulerControlID();
		if( layout.h_Ruler > 0 && inTabIndex == NVI_GetCurrentTabIndex() )
		{
			/*
			//���[���[�z�u
			GetApp().NVI_GetWindowByID(rulerWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
			layout.pt_Ruler,layout.w_Ruler,layout.h_Ruler);
			//���[���[�\��
			NVI_SetOverlayWindowVisibility(rulerWindowID,true);
			*/
			//
			NVI_SetControlPositionIfCurrentTab(inTabIndex,controlID,layout.pt_Ruler);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,controlID,layout.w_Ruler,layout.h_Ruler);
			NVI_SetShowControl(controlID,true);
			//���[���[�X�V
			UpdateRulerProperty(inTabIndex);
		}
		else
		{
			//���[���[��\��
			//NVI_SetOverlayWindowVisibility(rulerWindowID,false);
			NVI_SetShowControl(controlID,false);
		}
	}
	
	/*
	//�eTextView�̍��������߂�B�i�]���̕�������A���݂̃E�C���h�E�T�C�Y�ɍ��킹�ēK�p����B�j
	ANumber	newheight = layout.h_MainColumn;//#725 windowBounds.bottom - windowBounds.top - kHScrollBarHeight - y;
	//#92 ��������View
	if( mFindResultDocumentID != kObjectID_Invalid && mFindResultDisplayPosition == kDisplayPosition_MiddleBottom )
	{
		//View�pHeight������������
		newheight -= SPI_GetFindResultHeight() + kSeparatorHeight;//win+1;//#309
		layout.h_MainColumn -= SPI_GetFindResultHeight() + kSeparatorHeight+1;//���b��
#if IMPLEMENTATION_FOR_MACOSX
		newheight--; //���ꂪ�Ȃ��ƌ������ʕ\�����ɃX�N���[�����d���Ȃ�(#309)
#endif
		//#291 ��������View��
		ANumber	findResultViewWidth = x2-x0;
		ANumber	findResultSeparatorWidth = x2-x0+kVScrollBarWidth;//#634
		if( mRightSideBarDisplayed == true && sideBarMode == false )//#634
		{
			findResultViewWidth -= mRightSideBarWidth;
			findResultSeparatorWidth -= mRightSideBarWidth+kVScrollBarWidth;
		}
		//FindResultSeparator��z�u
		AWindowPoint	sepapt;
		sepapt.x = x0;
		sepapt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight - SPI_GetFindResultHeight() - kSeparatorHeight;
		GetApp().NVI_GetViewByID(mFindResultSeparatorViewID).NVI_SetPosition(sepapt);
		GetApp().NVI_GetViewByID(mFindResultSeparatorViewID).NVI_SetSize(findResultSeparatorWidth,kSeparatorHeight);//#634
		//�������ʗpIndexView��z�u
		AWindowPoint	frpt;
		frpt.x = x0;
		frpt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight - SPI_GetFindResultHeight();
		GetApp().NVI_GetViewByID(mFindResultViewID).NVI_SetPosition(frpt);
		GetApp().NVI_GetViewByID(mFindResultViewID).NVI_SetSize(findResultViewWidth,SPI_GetFindResultHeight());
		//VScrollBar��z�u
		AWindowPoint	scpt;
		scpt.x = windowBounds.right - windowBounds.left - kVScrollBarWidth +1;
		scpt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight - SPI_GetFindResultHeight() +1;
		if( mRightSideBarDisplayed == true && sideBarMode == true )//#634
		{
			scpt.x = frpt.x + findResultViewWidth;
		}
		NVI_SetControlPosition(mFindResultVScrollBarControlID,scpt);
		NVI_SetControlSize(mFindResultVScrollBarControlID,kVScrollBarWidth,SPI_GetFindResultHeight());
		//����{�^��
		AWindowPoint	cbpt;
		cbpt.x = x0+ 2;
		cbpt.y = scpt.y + 1;
		GetApp().NVI_GetWindowByID(mFindResultCloseButtonWindowID).NVI_SetOffsetOfOverlayWindowAndSize(
				GetObjectID(),cbpt,kVScrollBarWidth,kSplitButtonHeight);
		GetApp().NVI_GetWindowByID(mFindResultCloseButtonWindowID).NVI_Show(false);
		//��������ƁA�X�N���[�����Ɂ~�{�^�����X�N���[������Ă��܂�NVI_EmbedControl(GetApp().NVI_GetViewByID(mFindResultViewID).NVI_GetControlID(),mFindResultCloseButtonControlID);//win
	}
	*/
	
	//=============================================
	//�@�@TextView�z�u
	//=============================================
	if( inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneHSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_SubPaneHSeparator )//#645
	{
		//���݂�tab index�̏ꍇ�̂�
		if( NVI_GetCurrentTabIndex() == inTabIndex )
		{
			UpdateLayout_TextView(inTabIndex,layout.pt_MainColumn,layout.w_MainColumn,layout.h_MainColumn,
								  layout.w_MainTextView,layout.w_LineNumber,layout.w_MainTextMargin,layout.pt_VScrollbarArea);
		}
	}
	
	//=============================================
	//�@�@���T�C�h�o�[�z�u
	//=============================================
	//#212 �T�u�y�C���̕\���^��\���A�T�C�Y�A�ʒu�ݒ�
	//#645 �T�u�y�C���̈ʒu�E�T�C�Y�ݒ�(NVI_SetOffsetOfOverlayWindowAndSize()��)���K�v���ǂ����̔���
	/*
	if( NVI_IsWindowVisible() == true && inTabIndex == NVI_GetCurrentTabIndex() && panemodesupported == true &&
				inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneHSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneVSeparator &&
	inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneVSeparator_Completed )
	*/
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		if( NVI_IsWindowVisible() == true && inTabIndex == NVI_GetCurrentTabIndex() && panemodesupported == true )
		{
			if( UpdateLayout_LeftSideBar(inTabIndex,layout) == true )
			{
				subTextPaneDisplayed = true;
			}
		}
	}
	
	//#320
	//�T�u�e�L�X�g�y�C����\�����Ȃ������ꍇ�A�T�u�e�L�X�g�y�C���ƃ{�^�����\���ɂ���
	if( subTextPaneDisplayed == false )
	{
		//�T�u�y�C�����[�h�{�^����\��
		//#379 NVI_SetShowControl(kSubPaneModeButtonControlID,false);
		//�T�u�y�C���t�@�C�����{�^����\��
		NVI_SetShowControl(kSubPaneFileNameButtonControlID,false);
		//�T�u�y�C�����փ{�^��
		NVI_SetShowControl(kSubPaneSwapButtonControlID,false);
		//�T�u�y�C�������{�^��
		NVI_SetShowControl(kSubPaneSyncButtonControlID,false);
		//�T�u�y�C����r�{�^�� #379
		NVI_SetShowControl(kSubPaneDiffButtonControlID,false);
		//SubTextPane��\��
		AControlID	textViewControlID = mTabDataArray.GetObject(inTabIndex).GetSubTextControlID();
		if( textViewControlID != kControlID_Invalid )
		{
			//�T�u�e�L�X�g�̃e�L�X�g�r���[�A�s�ԍ��r���[���\��
			NVI_SetShowControl(textViewControlID,false);
			AControlID	lineNumberControlID = mTabDataArray.GetObject(inTabIndex).GetSubTextLineNumberControlID();
			NVI_SetShowControl(lineNumberControlID,false);
			//#725
			//�T�u�e�L�X�g�̐����X�N���[���o�[���\��
			AControlID	hScrollbarControlID = mTabDataArray.GetObject(inTabIndex).GetSubTextHScrollbarControlID();
			NVI_SetShowControl(hScrollbarControlID,false);
		}
	}
	
	//=============================================
	//�@�@�E�T�C�h�o�[�z�u
	//=============================================
	//#291 �E�T�C�h�o�[
	//#645 �E�T�C�h�o�[�̈ʒu�E�T�C�Y�ݒ�(NVI_SetOffsetOfOverlayWindowAndSize()��)���K�v���ǂ����̔���
	/*
	if( NVI_IsWindowVisible() == true && inTabIndex == NVI_GetCurrentTabIndex() && panemodesupported == true &&
				inTriggerType != kUpdateViewBoundsTriggerType_SubPaneHSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_ShowLeftSideBar &&
	inTriggerType != kUpdateViewBoundsTriggerType_HideLeftSideBar )
	*/
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		if( NVI_IsWindowVisible() == true && inTabIndex == NVI_GetCurrentTabIndex() && panemodesupported == true )
		{
			UpdateLayout_RightSideBar(inTabIndex,layout);
		}
	}
	
	//=============================================
	//�@�@�t�b�^�[�z�u
	//=============================================
	
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		//�X�e�[�^�X�o�[�\������
		ABool	showStatusBar = ( layout.h_Footer > 0 );
		
		//-------------�t�b�^�[�i��؂���j�z�u------------
		if( showStatusBar == true )
		{
			//�X�e�[�^�X�o�[�\����
			//�t�b�^�[�\������
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_Footer,layout.pt_Footer);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_Footer,layout.w_Footer,1);//layout.h_Footer);
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_Footer,true);
		}
		else
		{
			//�X�e�[�^�X�o�[��\����
			//�t�b�^�[�\�����Ȃ�
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_Footer,false);
		}
		
		//�t�b�^�[���e�{�^����y�ʒu�I�t�Z�b�g
		const ANumber	kFooterButtonsYOffset = 2;
		
		/*#�t�b�^�[���̋�؂��
		//
		{
			AWindowPoint	pt = layout.pt_Footer;
			pt.x = layout.pt_MainColumn.x;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn,pt);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn,kWidth_FooterSeparator,layout.h_Footer);
			if( mLeftSideBarDisplayed == true || mSubTextColumnDisplayed == true )
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn,false);
			}
			else
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn,false);
			}
		}
		//
		{
			AWindowPoint	pt = layout.pt_Footer;
			pt.x = layout.pt_MainColumn.x + kWidth_FooterSeparator+ kLineNumberButtonWidth;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn2,pt);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn2,kWidth_FooterSeparator,layout.h_Footer);
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn2,false);
		}
		//
		{
			AWindowPoint	pt = layout.pt_Footer;
			pt.x = layout.pt_SubTextPaneColumn.x;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn,pt);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn,kWidth_FooterSeparator,layout.h_Footer);
			if( mSubTextColumnDisplayed == true && mLeftSideBarDisplayed == true )
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn,false);
			}
			else
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn,false);
			}
		}
		//
		{
			AWindowPoint	pt = layout.pt_Footer;
			pt.x = layout.pt_SubTextPaneColumn.x + kWidth_FooterSeparator + kLineNumberButtonWidth;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn2,pt);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn2,kWidth_FooterSeparator,layout.h_Footer);
			if( mSubTextColumnDisplayed == true )
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn2,false);
			}
			else
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn2,false);
			}
		}
		*/
		
		if( showStatusBar == true )
		{
			//------------------�X�e�[�^�X�o�[�\����------------------
			
			//-------------�E�T�C�h�o�[����{�^���z�u-------------
			
			if( mRightSideBarDisplayed == true )
			{
				//-------------�E�T�C�h�o�[�\����-------------
				
				//�E�T�C�h�o�[��؂�{�^��
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_RightSideBar.x;// + kWidth_FooterSeparator;
					pt.y = layout.pt_Footer.y +kFooterButtonsYOffset;
					ANumber	w = 64;//layout.w_RightSideBar - kWidth_SizeBox - kWidth_RightSideBar_PrefButton - kWidth_RightSideBar_ShowHideButton;
					NVI_SetControlPosition(kControlID_RightSideBar_SeparatorButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_SeparatorButton,w,layout.h_Footer);
					NVI_SetShowControl(kControlID_RightSideBar_SeparatorButton,true);
				}
				//�E�T�C�h�o�[�\���E��\���{�^��
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_RightSideBar.x + layout.w_RightSideBar - kWidth_SizeBox - kWidth_RightSideBar_PrefButton - kWidth_RightSideBar_ShowHideButton;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_RightSideBar_ShowHideButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_ShowHideButton,kWidth_RightSideBar_ShowHideButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_RightSideBar_ShowHideButton,true);
				}
				//�E�T�C�h�o�[���ݒ�{�^��
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_RightSideBar.x + layout.w_RightSideBar - kWidth_SizeBox - kWidth_RightSideBar_PrefButton;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_RightSideBar_PrefButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_PrefButton,kWidth_RightSideBar_PrefButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_RightSideBar_PrefButton,true);
				}
				//
				mFooterProgressRightPosition = layout.pt_MainColumn.x + layout.w_MainColumn - kWidth_SizeBox;
			}
			else
			{
				//-------------�E�T�C�h�o�[��\����-------------
				
				//�E�T�C�h�o�[��؂�{�^����\��
				NVI_SetShowControl(kControlID_RightSideBar_SeparatorButton,false);
				//�E�T�C�h�o�[�\���E��\���{�^��
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_MainColumn.x + layout.w_MainColumn - kWidth_SizeBox - kWidth_RightSideBar_ShowHideButton;
					pt.y = layout.pt_Footer.y +kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_RightSideBar_ShowHideButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_ShowHideButton,kWidth_RightSideBar_ShowHideButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_RightSideBar_ShowHideButton,true);
				}
				//�E�T�C�h�o�[���ݒ�{�^��
				{
					NVI_SetShowControl(kControlID_RightSideBar_PrefButton,false);
					/*
					AWindowPoint	pt = {0};
					pt.x = inLayout.pt_MainColumn.x + inLayout.w_MainColumn - kWidth_SizeBox - kWidth_RightSideBar_PrefButton;
					pt.y = inLayout.pt_Footer.y +kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_RightSideBar_PrefButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_PrefButton,kWidth_RightSideBar_PrefButton,layout.h_Footer);
					NVI_SetShowControl(kControlID_RightSideBar_PrefButton,true);
					*/
				}
				//
				mFooterProgressRightPosition = layout.pt_MainColumn.x + layout.w_MainColumn
						- kWidth_SizeBox - kWidth_RightSideBar_ShowHideButton;
			}
			
			//-------------���T�C�h�o�[����{�^���z�u-------------
			
			if( mLeftSideBarDisplayed == true )
			{
				//-------------���T�C�h�o�[�\����-------------
				
				//���T�C�h�o�[��؂�{�^��
				{
					ANumber	w = layout.w_LeftSideBar - kWidth_LeftSideBar_PrefButton - kWidth_LeftSideBar_ShowHideButton;
					AWindowPoint	pt = {0};
					pt.x = layout.pt_LeftSideBar.x + kWidth_LeftSideBar_PrefButton + kWidth_LeftSideBar_ShowHideButton;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_SeparatorButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_SeparatorButton,w,layout.h_Footer);
					NVI_SetShowControl(kControlID_LeftSideBar_SeparatorButton,true);
					NVI_GetButtonViewByControlID(kControlID_LeftSideBar_SeparatorButton).SPI_SetButtonValidXRange(w-40,w);
				}
				/*#1184
				//���T�C�h�o�[�\���E��\���{�^��
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_LeftSideBar.x + kWidth_LeftSideBar_PrefButton;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_ShowHideButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_ShowHideButton,kWidth_LeftSideBar_ShowHideButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_LeftSideBar_ShowHideButton,true);
				}
				*/
				//���T�C�h�o�[���ݒ�{�^��
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_LeftSideBar.x;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_PrefButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_PrefButton,kWidth_LeftSideBar_PrefButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_LeftSideBar_PrefButton,true);
				}
			}
			else
			{
				//-------------���T�C�h�o�[��\����-------------
				
				//���T�C�h�o�[��؂�{�^����\��
				NVI_SetShowControl(kControlID_LeftSideBar_SeparatorButton,false);
				/*#1184
				//���T�C�h�o�[�\���E��\���{�^��
				if( mSubTextColumnDisplayed == true )
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_SubTextPaneColumn.x;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_ShowHideButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_ShowHideButton,kWidth_LeftSideBar_ShowHideButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_LeftSideBar_ShowHideButton,true);
				}
				else
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_MainColumn.x;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_ShowHideButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_ShowHideButton,kWidth_LeftSideBar_ShowHideButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_LeftSideBar_ShowHideButton,true);
				}
				*/
				//���T�C�h�o�[���ݒ�{�^��
				{
					NVI_SetShowControl(kControlID_LeftSideBar_PrefButton,false);
					/*
					AWindowPoint	pt = {0};
					pt.x = inLayout.pt_MainColumn.x + inLayout.w_MainColumn - kWidth_SizeBox - kWidth_LeftSideBar_PrefButton;
					pt.y = inLayout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_PrefButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_PrefButton,kWidth_LeftSideBar_PrefButton,layout.h_Footer);
					NVI_SetShowControl(kControlID_LeftSideBar_PrefButton,true);
					*/
				}
			}
			//#1184
			//���T�C�h�o�[�\���E��\���{�^��
			if( mSubTextColumnDisplayed == true )
			{
				AWindowPoint	pt = {0};
				pt.x = layout.pt_SubTextPaneColumn.x;
				pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
				NVI_SetControlPosition(kControlID_LeftSideBar_ShowHideButton,pt);
				NVI_SetControlSize(kControlID_LeftSideBar_ShowHideButton,kWidth_LeftSideBar_ShowHideButton,layout.h_Footer-2);
				NVI_SetShowControl(kControlID_LeftSideBar_ShowHideButton,true);
			}
			else
			{
				AWindowPoint	pt = {0};
				pt.x = layout.pt_MainColumn.x;
				pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
				NVI_SetControlPosition(kControlID_LeftSideBar_ShowHideButton,pt);
				NVI_SetControlSize(kControlID_LeftSideBar_ShowHideButton,kWidth_LeftSideBar_ShowHideButton,layout.h_Footer-2);
				NVI_SetShowControl(kControlID_LeftSideBar_ShowHideButton,true);
			}
		}
		else
		{
			//------------------�X�e�[�^�X�o�[��\����------------------
			
			if( mRightSideBarDisplayed == true )
			{
				//�E�T�C�h�o�[��؂�{�^��
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_RightSideBar.x;
					pt.y = windowHeight -20;
					NVI_SetControlPosition(kControlID_RightSideBar_SeparatorButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_SeparatorButton,64,20);
					NVI_SetShowControl(kControlID_RightSideBar_SeparatorButton,true);
				}
			}
			else
			{
				NVI_SetShowControl(kControlID_RightSideBar_SeparatorButton,false);
			}
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_RightSideBar_ShowHideButton,false);
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_RightSideBar_PrefButton,false);
			//
			if( mLeftSideBarDisplayed == true )
			{
				//���T�C�h�o�[��؂�{�^��
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_LeftSideBar.x + layout.w_LeftSideBar -65;
					pt.y = windowHeight -20;
					NVI_SetControlPosition(kControlID_LeftSideBar_SeparatorButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_SeparatorButton,64,20);
					NVI_SetShowControl(kControlID_LeftSideBar_SeparatorButton,true);
				}
			}
			else
			{
				NVI_SetShowControl(kControlID_LeftSideBar_SeparatorButton,false);
			}
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LeftSideBar_ShowHideButton,false);
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LeftSideBar_PrefButton,false);
		}
		
		//=============================================
		//�@�@�����e�R���g���[���z�u
		//=============================================
		
		//�e�s�ԍ��E�ʒu�{�^���z�u
		UpdateLayout_LineNumberAndPositionButton(inTabIndex,layout);
		
		//�t�b�^�[���v���O���X�\���X�V
		UpdateFooterProgress();
		
		//���C��H�X�N���[���o�[�z�u
		AControlID	hscrollbarControlID = mTabDataArray.GetObjectConst(inTabIndex).GetHScrollbarControlID();
		if( layout.h_MainHScrollbar > 0 )
		{
			AWindowPoint	pt = {0};
			pt.x = layout.pt_MainColumn.x;
			pt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight - layout.h_Footer;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,hscrollbarControlID,pt);
			ANumber	scrollwidth = windowWidth - layout.w_RightSideBar - layout.pt_MainColumn.x - kVScrollBarWidth;
			NVI_SetControlSizeIfCurrentTab(inTabIndex,hscrollbarControlID,scrollwidth,kHScrollBarHeight);
			NVI_SetShowControlIfCurrentTab(inTabIndex,hscrollbarControlID,true);
		}
		else
		{
			NVI_SetShowControlIfCurrentTab(inTabIndex,hscrollbarControlID,false);
		}
		
		//�T�C�Y�{�b�N�X�z�u
#if IMPLEMENTATION_FOR_WINDOWS
		{
			AWindowPoint	pt = {0};
			pt.x = windowWidth - kControlID_SizeBox;
			pt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_SizeBox,pt);
		}
#endif
	}
	
	//�^�u�\���X�V
	if( inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneHSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneVSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneVSeparator_Completed &&
				inTriggerType != kUpdateViewBoundsTriggerType_SubPaneHSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_ShowLeftSideBar &&
				inTriggerType != kUpdateViewBoundsTriggerType_HideLeftSideBar &&//#645
				inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		//R0228
		GetTabSelector().SPI_UpdateProperty();
	}
	
	//#505
	//�E�C���h�E�ŏ��T�C�Y�ݒ�
	//�ŏ���
	ANumber	minWidth = kMinWidth_MainColumn;
	if( mRightSideBarDisplayed == true )
	{
		minWidth += mRightSideBarWidth;
	}
	if( mLeftSideBarDisplayed == true )
	{
		minWidth += mLeftSideBarWidth;
	}
	if( mSubTextColumnDisplayed == true )
	{
		minWidth += mSubTextColumnWidth;
	}
	//�ŏ�����
	ANumber	minHeight = kMinHeight_MainColumn;
	minHeight += layout.h_MacroBar + layout.h_TabSelector + layout.h_Ruler + layout.h_Footer;
	//���T�C�h�o�[�̍ŏ������̂ق����傫����΁A��������̗p
	if( mLeftSideBarDisplayed == true )
	{
		ANumber	leftSideBarMinHeight = layout.h_MacroBar + layout.h_TabSelector + layout.h_Footer + 32;
		for( AIndex i = 0; i < mLeftSideBarArray_Type.GetItemCount(); i++ )
		{
			leftSideBarMinHeight += GetApp().SPI_GetSubWindowMinHeight(mLeftSideBarArray_OverlayWindowID.Get(i));
		}
		if( leftSideBarMinHeight > minHeight )
		{
			minHeight = leftSideBarMinHeight;
		}
	}
	//�E�T�C�h�o�[�̍ŏ������̂ق����傫����΁A��������̗p
	if( mRightSideBarDisplayed == true )
	{
		ANumber	rightSideBarMinHeight = layout.h_MacroBar + layout.h_TabSelector + layout.h_Footer + 32;
		for( AIndex i = 0; i < mRightSideBarArray_Type.GetItemCount(); i++ )
		{
			rightSideBarMinHeight += GetApp().SPI_GetSubWindowMinHeight(mRightSideBarArray_OverlayWindowID.Get(i));
		}
		if( rightSideBarMinHeight > minHeight )
		{
			minHeight = rightSideBarMinHeight;
		}
	}
	//�ŏ��ݒ�
	NVI_SetWindowMinimumSize(minWidth,minHeight);
	
	//#519 H�X�N���[���o�[�̍ő�ŏ��E���ݒl�X�V
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		GetTextViewByControlID(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).
					GetTextViewControlID(0)).NVI_UpdateScrollBar();
	}
	
	//�`��X�V��~����
	ACocoa::EnableScreenUpdates();
}

//#725
/**
4pane���[�h���ǂ������擾
*/
ABool	AWindow_Text::Is4PaneMode() const
{
	ABool	fourpaneMode = false;
	switch(mLayoutMode)
	{
	  case kLayoutMode_4Pane_Header11110110:
	  case kLayoutMode_4Pane_Header11110111:
	  case kLayoutMode_4Pane_Header01110111:
	  case kLayoutMode_4Pane_Header11111111:
		{
			fourpaneMode = true;
			break;
		}
	  default:
		{
			//��������
			//�i3pane���[�h�j
			break;
		}
	}
	return fourpaneMode;
}

//#725
/**
���C�A�E�g�v�Z
*/
void	AWindow_Text::CalculateLayout( const AIndex inTabIndex, ATextWindowLayoutData& outLayout ) 
{
	//�c�������[�h�擾 #558
	ABool	verticalMode = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetVerticalMode();
	
	//
	ABool	panemodesupported = GetApp().SPI_IsSupportPaneMode();
	//mode index�擾
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	
	//�E�C���h�E���E�����擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	ANumber	windowWidth = windowBounds.right - windowBounds.left;
	ANumber	windowHeight = windowBounds.bottom - windowBounds.top;
	
	//�t�b�^�[�T�C�Y�i�\���L���j�擾
	outLayout.h_Footer = kHeight_Footer;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
	{
		outLayout.h_Footer = 0;
	}
	
	//============= ���T�C�h�o�[ �J���� �v�Z(1) =============
	outLayout.w_LeftSideBar = 0;
	outLayout.h_LeftSideBar = 0;
	outLayout.pt_LeftSideBar.x = 0;
	outLayout.pt_LeftSideBar.y = 0;
	//���T�C�h�o�[�\���^��\������
	if( mLeftSideBarDisplayed == true )
	{
		//w_LeftSideBar�ɂ͋�؂�������̕����܂߂�
		//���̂��߁A�T�u�E�C���h�E�̕���w_LeftSideBar����k_wLeftSideBarVisibleArea���������l��ݒ肷��
		outLayout.w_LeftSideBar = mLeftSideBarWidth;
		outLayout.h_LeftSideBarWithMargin = windowHeight - outLayout.h_Footer;
		if( outLayout.h_Footer == 0 )
		{
			outLayout.h_LeftSideBar = outLayout.h_LeftSideBarWithMargin -22;
		}
		else
		{
			outLayout.h_LeftSideBar = outLayout.h_LeftSideBarWithMargin;
		}
		//�}�N���o�[�A�^�u�̌v�Z��ɁA���T�C�h�o�[�J�����ēx�v�Z
	}
	
	//============= �}�N���o�[�v�Z =============
	outLayout.w_MacroBar = 0;
	outLayout.h_MacroBar = 0;
	outLayout.pt_MacroBar.x = 0;
	outLayout.pt_MacroBar.y = 0;
	//�}�N���o�[�\���^��\������
	{
		//�}�N���o�[��
		outLayout.w_MacroBar = windowWidth;
		//���C�A�E�g���[�h�ɂ���ă}�N���o�[���ݒ�
		switch(mLayoutMode)
		{
		  case kLayoutMode_4Pane_Header01110111:
		  case kLayoutMode_3Pane_Header01110111:
			{
				//�}�N���o�[�͍��T�C�h�o�[�̉E�[�`�E�C���h�E�̉E�[
				outLayout.pt_MacroBar.x		+= outLayout.w_LeftSideBar;
				outLayout.w_MacroBar		-= outLayout.w_LeftSideBar;
				break;
			}
		  default:
			{
				//��������
				//�i�}�N���o�[�̓E�C���h�E�������ς��j
				break;
			}
		}
	}
	//�}�N���o�[�����ݒ�
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowMacroBar) == true &&
				NVI_IsToolBarShown() == true )
	{
		outLayout.h_MacroBar = CalculateMacroBarHeight(inTabIndex,outLayout.pt_MacroBar,outLayout.w_MacroBar);
	}
	else
	{
		//��������\�����邽�߁A����1�̃}�N���o�[��\������i
		outLayout.h_MacroBar = 1;
	}
	//============= �^�uselector�v�Z =============
	outLayout.w_TabSelector = 0;
	outLayout.h_TabSelector = 0;
	outLayout.pt_TabSelector.x = 0;
	outLayout.pt_TabSelector.y = 0;
	//�^�u����2�ȏ�Ȃ�^�uselector��\��
	if( NVI_GetSelectableTabCount() >= 2 )
	{
		//�^�uselector�ʒu�E��
		outLayout.pt_TabSelector.x = 0;
		outLayout.pt_TabSelector.y = outLayout.h_MacroBar;
		outLayout.w_TabSelector = windowWidth;
		//���C�A�E�g���[�h�ɂ���ă^�uselector�ʒu�E���ݒ�
		switch(mLayoutMode)
		{
		  case kLayoutMode_4Pane_Header11110110:
		  case kLayoutMode_3Pane_Header11110110:
			{
				//�^�u�͍��T�C�h�o�[�̉E�[�`�E�T�C�h�o�[�̍��[
				outLayout.pt_TabSelector.x	+= outLayout.w_LeftSideBar;
				outLayout.w_TabSelector		-= outLayout.w_LeftSideBar + mRightSideBarWidth;
				break;
			}
		  case kLayoutMode_4Pane_Header11110111:
		  case kLayoutMode_3Pane_Header11110111:
		  case kLayoutMode_4Pane_Header01110111:
		  case kLayoutMode_3Pane_Header01110111:
			{
				//�^�u�͍��T�C�h�o�[�̉E�[�`�E�C���h�E�̉E�[
				outLayout.pt_TabSelector.x	+= outLayout.w_LeftSideBar;
				outLayout.w_TabSelector		-= outLayout.w_LeftSideBar;
				break;
			}
		  default:
			{
				//��������
				//�i�^�u�̓E�C���h�E�������ς��j
				break;
			}
		}
		//�^�uselector�����ݒ�
		outLayout.h_TabSelector = GetTabSelectorConst().SPI_GetHeight(outLayout.w_TabSelector);
	}
	//============= �w�b�_�[���������v�Z =============
	//���T�C�h�o�[�̃w�b�_�[�����v�Z
	ANumber	leftSideBar_headerHeight = 0;
	switch(mLayoutMode)
	{
		//1-1
	  case kLayoutMode_4Pane_Header11111111:
	  case kLayoutMode_3Pane_Header11111111:
		{
			leftSideBar_headerHeight += outLayout.h_MacroBar + outLayout.h_TabSelector;
			break;
		}
		//1-0
	  case kLayoutMode_4Pane_Header11110110:
	  case kLayoutMode_3Pane_Header11110110:
	  case kLayoutMode_4Pane_Header11110111:
	  case kLayoutMode_3Pane_Header11110111:
		{
			leftSideBar_headerHeight += outLayout.h_MacroBar;
			break;
		}
		//0-0
	  default:
		{
			//��������
			break;
		}
	}
	//���C�������̃w�b�_�[�����v�Z
	ANumber	main_headerHeight = outLayout.h_MacroBar + outLayout.h_TabSelector;
	//�E�T�C�h�o�[�̃w�b�_�[�����v�Z
	ANumber	rightSideBar_headerHeight = 0;
	switch(mLayoutMode)
	{
		//1-1
	  case kLayoutMode_4Pane_Header11110111:
	  case kLayoutMode_3Pane_Header11110111:
	  case kLayoutMode_4Pane_Header01110111:
	  case kLayoutMode_3Pane_Header01110111:
	  case kLayoutMode_4Pane_Header11111111:
	  case kLayoutMode_3Pane_Header11111111:
		{
			rightSideBar_headerHeight += outLayout.h_MacroBar + outLayout.h_TabSelector;
			break;
		}
		//1-0
	  case kLayoutMode_4Pane_Header11110110:
	  case kLayoutMode_3Pane_Header11110110:
		{
			rightSideBar_headerHeight += outLayout.h_MacroBar;
			break;
		}
		//0-0
	  default:
		{
			//��������
			break;
		}
	}
	
	//============= ���T�C�h�o�[ �J���� �v�Z(2) =============
	//���T�C�h�o�[�Ƀw�b�_���������v�Z���ʔ��f
	if( outLayout.w_LeftSideBar > 0 )
	{
		outLayout.pt_LeftSideBar.y 	+= leftSideBar_headerHeight;
		outLayout.h_LeftSideBar		-= leftSideBar_headerHeight;
	}
	//���T�C�h�o�[������ �v�Z
	outLayout.pt_VSeparator_LeftSideBar.x = outLayout.pt_LeftSideBar.x + outLayout.w_LeftSideBar;
	outLayout.pt_VSeparator_LeftSideBar.y = outLayout.pt_LeftSideBar.y;
	if( outLayout.w_LeftSideBar > 0 )
	{
		outLayout.pt_VSeparator_LeftSideBar.x -= kSeparatorWidth;
	}
	
	//============= �T�u�e�L�X�g �J���� =============
	//�����X�N���[���o�[�����擾 #457
	outLayout.h_subHScrollbar = kHScrollBarHeight;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowHScrollBar) == false )
	{
		outLayout.h_subHScrollbar = 0;
	}
	//4Pane�̏ꍇ�A�J�����Ƃ��ăT�u�e�L�X�g�y�C����\������B
	outLayout.w_SubTextPaneColumn = 0;
	outLayout.h_SubTextPaneColumn = 0;
	outLayout.pt_SubTextPaneColumn.x = 0;
	outLayout.pt_SubTextPaneColumn.y = 0;
	if( Is4PaneMode() == true && mSubTextColumnDisplayed == true )
	{
		outLayout.w_SubTextPaneColumn = mSubTextColumnWidth;
		outLayout.h_SubTextPaneColumn = windowHeight - main_headerHeight - outLayout.h_subHScrollbar - outLayout.h_Footer;
		outLayout.pt_SubTextPaneColumn.x = outLayout.w_LeftSideBar;
		outLayout.pt_SubTextPaneColumn.y = main_headerHeight;
	}
	
	//============= ���[���[ =============
	outLayout.pt_Ruler.x = 0;
	outLayout.pt_Ruler.y = 0;
	outLayout.w_Ruler = 0;
	outLayout.h_Ruler = 0;
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kDisplayRuler) == true && verticalMode == false )//#558 �c�������[�h���̓��[���[�\�����Ȃ�
	{
		outLayout.pt_Ruler.x = outLayout.w_LeftSideBar + outLayout.w_SubTextPaneColumn;
		outLayout.pt_Ruler.y = main_headerHeight;
		outLayout.w_Ruler = windowWidth - outLayout.pt_Ruler.x;
		outLayout.h_Ruler = kRulerHeight;
	}
	
	//============= ���C�� �J���� =============
	//���C���J����
	outLayout.pt_MainColumn.x = outLayout.w_LeftSideBar + outLayout.w_SubTextPaneColumn;
	outLayout.pt_MainColumn.y = main_headerHeight + outLayout.h_Ruler;
	outLayout.w_MainColumn = windowWidth - outLayout.pt_MainColumn.x - kVScrollBarWidth;
	outLayout.h_MainColumn = windowHeight - main_headerHeight - outLayout.h_Ruler - kHScrollBarHeight - outLayout.h_Footer;
	//���C���r���[�pVScrollbar�̈�ʒu�ݒ�
	outLayout.pt_VScrollbarArea.x = windowWidth - kVScrollBarWidth;
	outLayout.pt_VScrollbarArea.y = main_headerHeight;
	//�s�ԍ��r���[���ݒ�
	outLayout.w_LineNumber = GetLineNumberWidth(inTabIndex);//#450
	
	//============= �E�T�C�h�o�[ �J���� =============
	//�E�T�C�h�o�[�J����
	outLayout.w_RightSideBar = 0;
	outLayout.h_RightSideBar_WithBottomMargin = windowHeight - rightSideBar_headerHeight - outLayout.h_Footer;// - kHeight_RightSidebarControlButtons;
	if( outLayout.h_Footer == 0 )
	{
		outLayout.h_RightSideBar = outLayout.h_RightSideBar_WithBottomMargin - 22;
	}
	else
	{
		outLayout.h_RightSideBar = outLayout.h_RightSideBar_WithBottomMargin;
	}
	outLayout.pt_RightSideBar.x = 0;
	outLayout.pt_RightSideBar.y = 0;
	outLayout.pt_VSeparator_RightSideBar.x = 0;
	outLayout.pt_VSeparator_RightSideBar.y = 0;
	if( panemodesupported == true )
	{
		if( mRightSideBarDisplayed == true )
		{
			//�E�T�C�h�o�[�E������
			outLayout.w_RightSideBar = mRightSideBarWidth;
			outLayout.pt_RightSideBar.x = windowWidth - outLayout.w_RightSideBar;
			outLayout.pt_RightSideBar.y = rightSideBar_headerHeight;
			outLayout.pt_VSeparator_RightSideBar.x = outLayout.pt_RightSideBar.x;
			outLayout.pt_VSeparator_RightSideBar.y = outLayout.pt_RightSideBar.y;
		}
		else
		{
			//�E�T�C�h�o�[������
			outLayout.pt_VSeparator_RightSideBar.x = windowWidth - kSeparatorWidth;
			outLayout.pt_VSeparator_RightSideBar.y = rightSideBar_headerHeight;
		}
		//���C���r���[���̕��␳
		outLayout.w_Ruler -= outLayout.w_RightSideBar;
		outLayout.w_MainColumn -= outLayout.w_RightSideBar;
		outLayout.pt_VScrollbarArea.x -= outLayout.w_RightSideBar;
	}
	//���C��text view�̕��ݒ�
	outLayout.w_MainTextView = outLayout.w_MainColumn - outLayout.w_LineNumber;
	//���C��h scrollbar�̍����ݒ�
	outLayout.h_MainHScrollbar = kHScrollBarHeight;
	if( mTabDataArray.GetObjectConst(inTabIndex).GetViewCount() > 0 && verticalMode == false )//#558 �c�������[�h�͏��H�X�N���[���o�[�\��
	{
		//text view�̃C���[�W�T�C�Y����text view�̕��ȉ��Ȃ�Ah�X�N���[���o�[�͕\�����Ȃ�
		if( GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0)).NVM_GetImageWidth() <=
					outLayout.w_MainTextView )
		{
			outLayout.h_MainColumn += outLayout.h_MainHScrollbar;
			outLayout.h_MainHScrollbar = 0;
		}
	}
	//�����X�N���[���o�[��\���ݒ�̏ꍇ�̏��� #457
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowHScrollBar) == false )
	{
		outLayout.h_MainColumn += outLayout.h_MainHScrollbar;
		outLayout.h_MainHScrollbar = 0;
	}
	//�]���v�Z #1186
	outLayout.w_MainTextDisplayWidth = outLayout.w_MainTextView;
	outLayout.w_MainTextMargin = 0;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayLeftRightMargin) == true )
	{
		outLayout.w_MainTextDisplayWidth = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kTextWidthForLeftRightMargin);
		ANumber	a = outLayout.w_LineNumber + outLayout.w_MainTextDisplayWidth;
		if( verticalMode == false )
		{
			outLayout.w_MainTextMargin = (outLayout.w_MainColumn - a)/2;
			if( outLayout.w_MainTextMargin > 0 )
			{
				outLayout.w_LineNumber += outLayout.w_MainTextMargin;
				outLayout.w_MainTextView -= outLayout.w_MainTextMargin;
			}
			else
			{
				outLayout.w_MainTextMargin = 0;
			}
		}
		else
		{
			outLayout.w_MainTextMargin = (outLayout.h_MainColumn - a)/2;
			if( outLayout.w_MainTextMargin > 0 )
			{
				outLayout.w_LineNumber += outLayout.w_MainTextMargin;
				outLayout.w_MainTextView -= outLayout.w_MainTextMargin;
			}
			else
			{
				outLayout.w_MainTextMargin = 0;
			}
		}
	}
	
	//============= �t�b�^�[ =============
	outLayout.pt_Footer.x = 0;
	outLayout.pt_Footer.y = windowHeight - outLayout.h_Footer;
	outLayout.w_Footer = windowWidth;
}

//#450
/**
�s�ԍ�view���擾
*/
ANumber	AWindow_Text::GetLineNumberWidth( const AIndex inTabIndex ) const
{
	AItemCount	lineCount = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetLineCount();
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	ANumber	width = AView_LineNumber::kSeparatorOffset + AView_LineNumber::kRightMargin;
	if( mTabDataArray.GetObjectConst(inTabIndex).GetViewCount() > 0 )
	{
		//1 digit�̕��擾
		ANumber	digitWidth = SPI_GetLineNumberViewConst(inTabIndex,0).SPI_Get1DigitWidth();
		//�s���ɂ���āA���v�Z
		if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber) == true )
		{
			ANumber	numwidth = digitWidth*5 + 5;
			if( lineCount > 1000000 )
			{
				numwidth += digitWidth*2;
			}
			else if( lineCount > 100000 )
			{
				numwidth += digitWidth*1;
			}
			width += AView_LineNumber::kFoldingMarkLeftMargin + AView_LineNumber::kFoldingMarkSize + numwidth;
		}
	}
	return width;
}

//#725
/**
���C��text view�z�u
*/
void	AWindow_Text::UpdateLayout_TextView( const AIndex inTabIndex, 
		const AWindowPoint pt_MainColumn_origin, const ANumber w_MainColumn, const ANumber h_MainColumn,
											 const ANumber w_TextView, const ANumber w_LineNumber, 
											 const ANumber w_MainTextMargin, //#1186
											 AWindowPoint pt_VScrollbarArea_origin )
{
	//�c�������[�h�擾 #558
	ABool	verticalMode = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetVerticalMode();
	
	//#675 UpdateViewBounds�}����Ԓ��͉������Ȃ�
	//#1090 UpdateViewBounds�}����Ԓ��͉�ʊO�ɔz�u����
	AWindowPoint pt_MainColumn = pt_MainColumn_origin;
	AWindowPoint	pt_VScrollbarArea = pt_VScrollbarArea_origin;
	if( mSuppressUpdateViewBoundsCounter > 0 )   //return;
	{
		pt_MainColumn.x = -10000;
		pt_MainColumn.y = -10000;
		pt_VScrollbarArea.x = -10000;
		pt_VScrollbarArea.y = -10000;
	}
	
	//
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	//�etext view�̍����␳
	ANumber	viewsheight = 0;
	for( AIndex v = 0; v < mTabDataArray.GetObject(inTabIndex).GetViewCount(); v++ )
	{
		viewsheight += mTabDataArray.GetObject(inTabIndex).GetViewHeight(v);
	}
	ANumber	newviewsheight = 0;
	for( AIndex v = 0; v < mTabDataArray.GetObject(inTabIndex).GetViewCount()-1; v++ )
	{
		AFloatNumber	h = mTabDataArray.GetObject(inTabIndex).GetViewHeight(v);
		h /= viewsheight;
		h *= h_MainColumn;
		mTabDataArray.GetObject(inTabIndex).SetViewHeight(v,static_cast<ANumber>(h));
		newviewsheight += static_cast<ANumber>(h);
	}
	mTabDataArray.GetObject(inTabIndex).SetViewHeight(mTabDataArray.GetObject(inTabIndex).GetViewCount()-1, h_MainColumn - newviewsheight);
	
	//y�ʒu
	ANumber	y = pt_MainColumn.y;
	//�e�c�����r���[�����[�v
	AItemCount	viewcount = mTabDataArray.GetObject(inTabIndex).GetViewCount();
	for( AIndex v = 0; v < viewcount; v++ )
	{
		//viewheight�擾�i�����������r���[���̍������܂�ł���j
		ANumber	viewheight = mTabDataArray.GetObject(inTabIndex).GetViewHeight(v);
		
		//�����������r���[�̔z�u
		AControlID	separatorControlID = mTabDataArray.GetObject(inTabIndex).GetSeparatorControlID(v);
		if( v == 0 )
		{
			//�ŏ��̐����������͕\�����Ȃ�
			NVI_SetControlPosition(separatorControlID,kWindowPoint_00);
			NVI_SetControlSize(separatorControlID,0,0);
		}
		else
		{
			AWindowPoint	pt = {pt_MainColumn.x,y};
			NVI_SetControlPosition(separatorControlID,pt);
			NVI_SetControlSize(separatorControlID,w_MainColumn,kSeparatorHeight);
			y += kSeparatorHeight;
			viewheight -= kSeparatorHeight;
			/*#725
			//�������̔w�i�F��text view�w�i�F�Ɠ����ɂ���
			AColor	backgroundColor = kColor_White;
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kBackgroundColor,backgroundColor);
			NVI_GetHSeparatorViewByControlID(separatorControlID).SPI_SetBackgroundColor(backgroundColor);
			*/
		}
		//====================TextView�̔z�u====================
		//TextView�c�������[�h�ݒ� #558
		GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_SetVerticalMode(verticalMode);
		if( verticalMode == false )
		{
			//�ʏ�z�u
			AWindowPoint	pt = {pt_MainColumn.x + w_LineNumber,y};
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v),pt);
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v),w_TextView,viewheight);
//������			GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).SPI_UpdateImageSize();
			//TextView�ɁA�w�i�摜��Offset(=LineNumberView�̕�)��ݒ肷��
			GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).SPI_SetBackgroundImageOffset(w_LineNumber);
			//�C���[�W�T�C�Y�O�ɃX�N���[������Ă�����␳����
			//�i�h�L�������g���J��������ASPI_DoWrapCalculated()�ŁAview�T�C�Y���ݒ�Ȃ̂ŃT�C�Y���X�N���[���ʒu���C���[�W�T�C�Y�O�ɂȂ邱�Ƃ����邽�߁j
			/*
			ANumber	svImageYMaxMargin = GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_GetImageYMaxMargin();
			ANumber	svImageYMinMargin = GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_GetImageYMinMargin();
			GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_SetImageYMargin(0,0);
			GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_ConstrainScrollToImageSize(false);
			GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_SetImageYMargin(svImageYMinMargin,svImageYMaxMargin);
			*/
		}
		else
		{
			//�c�������[�h���z�u #558
			AWindowPoint	pt = {pt_MainColumn.x,y+w_LineNumber};
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v),pt);
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v),w_TextView+w_LineNumber,viewheight-w_LineNumber);
		}
		//TextView�ɑΉ�����X�N���[���o�[�̃R���g���[��ID�ݒ� #558
		GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).
				NVI_SetScrollBarControlID(mTabDataArray.GetObjectConst(inTabIndex).GetHScrollbarControlID(),
										  mTabDataArray.GetObject(inTabIndex).GetVScrollBarControlID(v));
		//==================LineNumberView�̔z�u==================
		//LineNumberView�c�������[�h�ݒ� #558
		GetLineNumberViewByControlID(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v)).NVI_SetVerticalMode(verticalMode);
		if( verticalMode == false )
		{
			//�ʏ�z�u
			AWindowPoint	pt = {pt_MainColumn.x,y};
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v),pt);
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v),w_LineNumber,viewheight);
			//#379
			//������r�r���[�z�u
			if( v == 0 && inTabIndex == NVI_GetCurrentTabIndex() )
			{
				GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
																								  pt,w_LineNumber,viewheight);
				if( NVI_IsWindowVisible() == true )//#1090 CreateTab()����ŏ��ɃR�[�����ꂽ���ɁA�E�C���h�E���܂�show����Ă��Ȃ��̂ŁA���̏ꍇ�́Ashow()���Ȃ��悤�ɂ���i�q�E�C���h�E��show����Ɛe��show����Ă��܂��̂Łj
				{
					GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Show(false);
				}
			}
			//
			GetLineNumberViewByControlID(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v)).SPI_SetLeftMargin(w_MainTextMargin);
		}
		else
		{
			//�c�������[�h���z�u #558
			AWindowPoint	pt = {pt_MainColumn.x,y};
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v),pt);
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v),w_TextView+w_LineNumber,w_LineNumber);
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Hide();
		}
		//==================�����{�^��==================
		ANumber	h = 0;
		//SplitButton�̔z�u
		{
			AWindowPoint	pt = {pt_VScrollbarArea.x,y};
			if( v > 0 )
			{
				//2�߈ȍ~�̃r���[split�{�^����y�ʒu��4pt��ɂ���
				pt.y -= 4;
			}
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetSplitButtonControlID(v),pt);
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetSplitButtonControlID(v),kVScrollBarWidth,kSplitButtonHeight);
			h += kSplitButtonHeight;
		}
		//==================����{�^��==================
		//#844 AControlID	concatControlID = mTabDataArray.GetObject(inTabIndex).GetConcatButtonControlID(v);
		AWindowID	closeViewButtonWindowID = mTabDataArray.GetObject(inTabIndex).GetCloseViewButtonWindowID(v);//win
		if( mTabDataArray.GetObject(inTabIndex).GetViewCount() < 2 )
		{
			//�r���[���P�݂̂̏ꍇ
			//Show/Hide���� 
			//#844 NVI_SetShowControl(concatControlID,false);
			//#725 GetApp().NVI_GetWindowByID(closeViewButtonWindowID).NVI_Hide();
			NVI_SetOverlayWindowVisibility(closeViewButtonWindowID,false);//#725
		}
		else
		{
			//�ʒu�E�T�C�Y�ݒ�
			AWindowPoint	pt = {pt_MainColumn.x,y};
			if( v > 0 )
			{
				//2�߈ȍ~�̃r���[�N���[�Y�{�^����y�ʒu��4pt��ɂ���
				pt.y -= 3;
			}
			GetApp().NVI_GetWindowByID(closeViewButtonWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
						pt,19,19);
			//Show/Hide���� 
			/*#725
			if( NVI_GetCurrentTabIndex() == inTabIndex )
			{
				NVI_SetShowControl(concatControlID,false);
				GetApp().NVI_GetWindowByID(closeViewButtonWindowID).NVI_Show(false);
			}
			*/
			NVI_SetOverlayWindowVisibility(closeViewButtonWindowID,true);//#725
		}
		//==================V�X�N���[���o�[�̔z�u==================
		{
			AWindowPoint	pt = {pt_VScrollbarArea.x, y + h};
			if( v > 0 )
			{
				//2�߈ȍ~��V�X�N���[���o�[�{�^����y�ʒu��4pt��ɂ���
				pt.y -= 4;
			}
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetVScrollBarControlID(v),pt);
			ANumber	vscrollheight = viewheight-h;
			if( v > 0 )
			{
				vscrollheight += 4;
			}
			if( v != viewcount-1 )
			{
				vscrollheight += 3;
			}
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetVScrollBarControlID(v),kVScrollBarWidth,vscrollheight);
		}
		//����y�ʒu
		y += viewheight;
	}
}

/**
�T�u�e�L�X�g�z�u
*/
ABool	AWindow_Text::UpdateLayout_SubTextColumn( const AIndex inTabIndex, const ATextWindowLayoutData& layout )
{
	ABool	subTextPaneDisplayed = false;
	if( layout.w_SubTextPaneColumn > 0 )
	{
		//�T�u�e�L�X�g�J�����z�u
		UpdateLayout_SubTextPane(inTabIndex,layout.pt_SubTextPaneColumn,layout.w_SubTextPaneColumn,layout.h_SubTextPaneColumn,
								 layout.h_subHScrollbar);
		//�T�u�e�L�X�g�y�C���\���ς݃t���O��ON
		subTextPaneDisplayed = true;
		
		//�T�u�e�L�X�g�J�����������z�u
		if( mSubTextColumnVSeparatorWindowID != kObjectID_Invalid )
		{
			AWindowPoint	pt = {0};
			pt.x = layout.pt_SubTextPaneColumn.x + layout.w_SubTextPaneColumn - kSeparatorWidth;
			pt.y = layout.pt_SubTextPaneColumn.y;
			NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
						pt,kSeparatorWidth,layout.h_SubTextPaneColumn + layout.h_subHScrollbar);
			NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_Show(false);
			NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).SPI_SetLinePosition(kSeparatorLinePosition_Right);
		}
		
		//�T�u�e�L�X�g�V���h�E�z�u
		{
			AWindowPoint	pt = {0};
			pt.x = layout.pt_SubTextPaneColumn.x + layout.w_SubTextPaneColumn - kWidth_SubTextShadow;
			pt.y = layout.pt_SubTextPaneColumn.y + kSubPaneModeButtonHeight;
			GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
						pt, kWidth_SubTextShadow, layout.h_SubTextPaneColumn - kSubPaneModeButtonHeight);
			GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_Show(false);
		}
	}
	else
	{
		//�T�u�e�L�X�g�J������������\��
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_Hide();
		//�T�u�e�L�X�g�V���h�E��\��
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_Hide();
	}
	return subTextPaneDisplayed;
}

//#725
/**
�T�u�e�L�X�g�y�C���z�u
*/
void	AWindow_Text::UpdateLayout_SubTextPane( const AIndex inTabIndex, const AWindowPoint pt_SubTextPane_origin, 
		const ANumber w_SubTextPane, const ANumber h_SubTextPane, const ANumber h_SubHScrollbar )
{
	//#675 UpdateViewBounds�}����Ԓ��͉������Ȃ�
	//#1090 UpdateViewBounds�}����Ԓ��͉�ʊO�ɔz�u����
	AWindowPoint pt_SubTextPane = pt_SubTextPane_origin;
	if( mSuppressUpdateViewBoundsCounter > 0 )   //return;
	{
		pt_SubTextPane.x = -10000;
		pt_SubTextPane.y = -10000;
	}
	//�^�u�����[�v
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )//win �T�u�y�C���͌��݂̃^�u�Ƃ͈Ⴄ�^�u�̓��e��\���\�Ȃ̂�
	{
		//mode index�擾
		AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex();
		//
		AControlID	textViewControlID = mTabDataArray.GetObject(tabIndex).GetSubTextControlID();
		if( textViewControlID != kControlID_Invalid )
		{
			//�s�ԍ����擾
			ANumber	lineNumberWidth = GetLineNumberWidth(inTabIndex);//#450
			AWindowPoint	pt2 = pt_SubTextPane;
			//�㕔�{�^���z�u
			if( mDiffDisplayMode == false )
			{
				//(Diff�\���ł͂Ȃ�)�ʏ�\���̏ꍇ�̃{�^���z�u
				
				//�T�u�y�C���t�@�C�����{�^��
				ANumber	w2 = w_SubTextPane-kSubPaneSyncButtonWidth-kSubPaneSwapButtonWidth-kSubPaneDiffButtonWidth;//#379
				NVI_SetControlPosition(kSubPaneFileNameButtonControlID,pt_SubTextPane);
				NVI_SetControlSize(kSubPaneFileNameButtonControlID,w2,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneFileNameButtonControlID,true);
				//�T�u�y�C��Diff�{�^�� #379
				pt2.x += w2;//#379 kSubPaneModeButtonWidth;
				NVI_SetControlPosition(kSubPaneDiffButtonControlID,pt2);
				NVI_SetControlSize(kSubPaneDiffButtonControlID,kSubPaneDiffButtonWidth,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneDiffButtonControlID,true);
				//��r�h�L�������g�L���Ń{�^����Enable����
				if( NVI_GetCurrentDocumentID() != kObjectID_Invalid )
				{
					if( GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )
					{
						NVI_SetControlEnable(kSubPaneDiffButtonControlID,false);
					}
					else
					{
						NVI_SetControlEnable(kSubPaneDiffButtonControlID,true);
					}
				}
				//�T�u�y�C�����փ{�^��
				pt2.x += kSubPaneDiffButtonWidth;
				NVI_SetControlPosition(kSubPaneSwapButtonControlID,pt2);
				NVI_SetControlSize(kSubPaneSwapButtonControlID,kSubPaneSwapButtonWidth,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneSwapButtonControlID,true);
				//�T�u�y�C�������{�^��
				pt2.x += kSubPaneSwapButtonWidth;
				NVI_SetControlPosition(kSubPaneSyncButtonControlID,pt2);
				NVI_SetControlSize(kSubPaneSyncButtonControlID,kSubPaneSyncButtonWidth,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneSyncButtonControlID,true);
			}
			else//#379
			{
				//Diff�\���̏ꍇ�̃{�^���z�u
				
				//
				NVI_SetShowControl(kSubPaneFileNameButtonControlID,false);
				//#379 NVI_SetShowControl(kSubPaneModeButtonControlID,false);
				NVI_SetShowControl(kSubPaneSwapButtonControlID,false);
				//�T�u�y�C��Diff�{�^��
				ANumber	w2 = w_SubTextPane-kSubPaneSyncButtonWidth;
				NVI_SetControlPosition(kSubPaneDiffButtonControlID,pt2);
				NVI_SetControlSize(kSubPaneDiffButtonControlID,w2+1,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneDiffButtonControlID,true);
				//�T�u�y�C�������{�^��
				pt2.x += w2;
				NVI_SetControlPosition(kSubPaneSyncButtonControlID,pt2);
				NVI_SetControlSize(kSubPaneSyncButtonControlID,kSubPaneSyncButtonWidth,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneSyncButtonControlID,true);
			}
			//�c�������[�h�擾 #558
			ABool	verticalMode = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetVerticalMode();
			//�e�s�s�ԍ��z�u
			AControlID	lineNumberControlID = mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID();
			pt2 = pt_SubTextPane;
			pt2.y += kSubPaneModeButtonHeight;
			//LineNumberView�c�������[�h�ݒ� #558
			GetLineNumberViewByControlID(lineNumberControlID).NVI_SetVerticalMode(verticalMode);
			if( verticalMode == false )
			{
				//�ʏ�z�u
				NVI_SetControlPosition(lineNumberControlID,pt2);
				NVI_SetControlSize(lineNumberControlID,lineNumberWidth,h_SubTextPane-kSubPaneModeButtonHeight);
			}
			else
			{
				//�c�����p�z�u #558
				NVI_SetControlPosition(lineNumberControlID,pt2);
				NVI_SetControlSize(lineNumberControlID,w_SubTextPane,lineNumberWidth);
			}
			//#725
			//�T�u�e�L�X�gH�X�N���[���o�[�z�u
			{
				AControlID	hScrollbarControlID = mTabDataArray.GetObject(tabIndex).GetSubTextHScrollbarControlID();
				if( h_SubHScrollbar > 0 )
				{
					AWindowPoint	pt = {0};
					pt.x = pt_SubTextPane.x;
					pt.y = pt_SubTextPane.y + h_SubTextPane;
					NVI_SetControlPosition(hScrollbarControlID,pt);
					NVI_SetControlSize(hScrollbarControlID,w_SubTextPane,kHScrollBarHeight);
					NVI_SetShowControl(hScrollbarControlID,true);
				}
				else
				{
					//H�X�N���[���o�[��\��
					NVI_SetShowControl(hScrollbarControlID,false);
				}
				//TextView�ɁA�Ή�����X�N���[���o�[�̃R���g���[��ID�ݒ� #558
				GetTextViewByControlID(textViewControlID).NVI_SetScrollBarControlID(hScrollbarControlID,kControlID_Invalid);
			}
			//TextView�c�������[�h�ݒ� #558
			GetTextViewByControlID(textViewControlID).NVI_SetVerticalMode(verticalMode);
			//TextView�z�u
			if( verticalMode == false )
			{
				//�ʏ�z�u
				pt2.x += lineNumberWidth;
				NVI_SetControlPosition(textViewControlID,pt2);
				NVI_SetControlSize(textViewControlID,w_SubTextPane-lineNumberWidth,h_SubTextPane-kSubPaneModeButtonHeight);
			}
			else
			{
				//�c�����p�z�u #558
				pt2.y += lineNumberWidth;
				NVI_SetControlPosition(textViewControlID,pt2);
				NVI_SetControlSize(textViewControlID,w_SubTextPane,h_SubTextPane-kSubPaneModeButtonHeight-lineNumberWidth);
			}
			//TextView�ɁA�w�i�摜��Offset(=LineNumberView�̕�)��ݒ肷��
			GetTextViewByControlID(textViewControlID).SPI_SetBackgroundImageOffset(lineNumberWidth);
			//ImageSize�X�V
			GetTextViewByControlID(textViewControlID).SPI_UpdateImageSize();
		}
	}
}

/**
���T�C�h�o�[�e�����␳
*/
void	AWindow_Text::UpdateLayout_AdjustLeftSideBarHeight( const ANumber inLeftSideBarHeight, const AIndex inAdjustStartIndex )
{
	//==================inAdjustStartIndex�ȍ~�̃T�C�h�o�[���ڂ̍����f�[�^�̘a���v�Z==================
	AFloatNumber	totalSubPaneHeight = 0;
	for( AIndex index = inAdjustStartIndex; index < mLeftSideBarArray_Height.GetItemCount(); index++ )
	{
		//4�y�C�����[�h�̏ꍇ�́A�T�u�e�L�X�g�y�C���͕\�����Ȃ��B
		if( Is4PaneMode() == true )
		{
			if( mLeftSideBarArray_Type.Get(index) == kSubWindowType_SubTextPane )
			{
				continue;
			}
		}
		//�S�̍���
		if( mLeftSideBarArray_Collapsed.Get(index) == true )
		{
			//�܂肽���ݎ����������Z
			totalSubPaneHeight += GetApp().SPI_GetSubWindowCollapsedHeight();
		}
		else
		{
			//���݂̍��������Z
			totalSubPaneHeight += mLeftSideBarArray_Height.Get(index);
		}
	}
	//==================inAdjustStartIndex�ȍ~�̃T�C�h�o�[�\���G���A�̎��ۂ̍������擾==================
	ANumber	remainSideBarHeight = inLeftSideBarHeight;
	for( AIndex index = 0; index < inAdjustStartIndex; index++ )
	{
		if( mLeftSideBarArray_Collapsed.Get(index) == true )
		{
			remainSideBarHeight -= GetApp().SPI_GetSubWindowCollapsedHeight();
		}
		else
		{
			remainSideBarHeight -= mLeftSideBarArray_Height.Get(index);
		}
	}
	//==================�����␳==================
	//���ۂ̕\���G���A�ƁA��L�Ōv�Z�����S�̍������Ⴄ�ꍇ�́A�����␳����B
	if( totalSubPaneHeight != remainSideBarHeight && remainSideBarHeight > 0 )//win �ŏ����������ɃT�C�Y�ُ�ɂȂ���΍�
	{
		//���݂�mLeftSideBarArray_Height�f�[�^�̍����䗦�Őݒ肷��
		AFloatNumber	n = 0;
		for( AIndex index = inAdjustStartIndex; index < mLeftSideBarArray_Height.GetItemCount(); index++ )
		{
			if( mLeftSideBarArray_Collapsed.Get(index) == true )
			{
				//�܂肽���ݎ�
				n += GetApp().SPI_GetSubWindowCollapsedHeight();
			}
			else
			{
				//4�y�C�����[�h�̏ꍇ�́A�T�u�e�L�X�g�y�C���͕\�����Ȃ��B
				if( Is4PaneMode() == true )
				{
					if( mLeftSideBarArray_Type.Get(index) == kSubWindowType_SubTextPane )
					{
						continue;
					}
				}
				//�����v�Z�i�Ō�̂݁A�䗦�v�Z�ł͂Ȃ��A�c�荂����ݒ�j
				AFloatNumber	heightRatio = static_cast<AFloatNumber>(mLeftSideBarArray_Height.Get(index));
				heightRatio = heightRatio/totalSubPaneHeight;
				AFloatNumber	h = (remainSideBarHeight*heightRatio);
				if( index != mLeftSideBarArray_Height.GetItemCount()-1 )
				{
					mLeftSideBarArray_Height.Set(index,h);
					n += h;
				}
				else
				{
					mLeftSideBarArray_Height.Set(index,remainSideBarHeight-n);
				}
			}
		}
	}
	//DB�ɃT�C�h�o�[�f�[�^�ۑ�
	SetDataSideBar();
}

/**
�E�T�C�h�o�[�e�����␳
*/
void	AWindow_Text::UpdateLayout_AdjustRightSideBarHeight( const ANumber inRightSideBarHeight, const AIndex inAdjustStartIndex )
{
	//==================inAdjustStartIndex�ȍ~�̃T�C�h�o�[���ڂ̍����f�[�^�̘a���v�Z==================
	AFloatNumber	totalSubPaneHeight = 0;
	for( AIndex index = inAdjustStartIndex; index < mRightSideBarArray_Height.GetItemCount(); index++ )
	{
		//4�y�C�����[�h�̏ꍇ�́A�T�u�e�L�X�g�y�C���͕\�����Ȃ��B
		if( Is4PaneMode() == true )
		{
			if( mRightSideBarArray_Type.Get(index) == kSubWindowType_SubTextPane )
			{
				continue;
			}
		}
		//�S�̍���
		if( mRightSideBarArray_Collapsed.Get(index) == true )
		{
			//�܂肽���ݎ����������Z
			totalSubPaneHeight += GetApp().SPI_GetSubWindowCollapsedHeight();
		}
		else
		{
			//���݂̍��������Z
			totalSubPaneHeight += mRightSideBarArray_Height.Get(index);
		}
	}
	//
	ANumber	remainSideBarHeight = inRightSideBarHeight;
	for( AIndex index = 0; index < inAdjustStartIndex; index++ )
	{
		if( mRightSideBarArray_Collapsed.Get(index) == true )
		{
			remainSideBarHeight -= GetApp().SPI_GetSubWindowCollapsedHeight();
		}
		else
		{
			remainSideBarHeight -= mRightSideBarArray_Height.Get(index);
		}
	}
	//==================�����␳==================
	//���ۂ̕\���G���A�ƁA��L�Ōv�Z�����S�̍������Ⴄ�ꍇ�́A�����␳����B
	if( totalSubPaneHeight != remainSideBarHeight && remainSideBarHeight > 0 )//win �ŏ����������ɃT�C�Y�ُ�ɂȂ���΍�
	{
		//���݂�mRightSideBarArray_Height�f�[�^�̍����䗦�Őݒ肷��
		AFloatNumber	n = 0;
		for( AIndex index = inAdjustStartIndex; index < mRightSideBarArray_Height.GetItemCount(); index++ )
		{
			if( mRightSideBarArray_Collapsed.Get(index) == true )
			{
				//�܂肽���ݎ�
				n += GetApp().SPI_GetSubWindowCollapsedHeight();
			}
			else
			{
				//4�y�C�����[�h�̏ꍇ�́A�T�u�e�L�X�g�y�C���͕\�����Ȃ��B
				if( Is4PaneMode() == true )
				{
					if( mRightSideBarArray_Type.Get(index) == kSubWindowType_SubTextPane )
					{
						continue;
					}
				}
				//�����v�Z�i�Ō�̂݁A�䗦�v�Z�ł͂Ȃ��A�c�荂����ݒ�j
				AFloatNumber	heightRatio = static_cast<AFloatNumber>(mRightSideBarArray_Height.Get(index));
				heightRatio = heightRatio/totalSubPaneHeight;
				AFloatNumber	h = (remainSideBarHeight*heightRatio);
				if( index != mRightSideBarArray_Height.GetItemCount()-1 )
				{
					mRightSideBarArray_Height.Set(index,h);
					n += h;
				}
				else
				{
					mRightSideBarArray_Height.Set(index,remainSideBarHeight-n);
				}
			}
		}
	}
	//DB�ɃT�C�h�o�[�f�[�^�ۑ�
	SetDataSideBar();
}

/**
�E�T�C�h�o�[�z�u
*/
void	AWindow_Text::UpdateLayout_RightSideBar( const AIndex inTabIndex, const ATextWindowLayoutData& inLayout, const ABool inAnimate )
{
	//#675 UpdateViewBounds�}����Ԓ��͉������Ȃ�
	if( mSuppressUpdateViewBoundsCounter > 0 )   return;
	
	//
	if( mRightSideBarDisplayed == true )
	{
		//========== �E�T�C�h�o�[��\������ꍇ ==========
		
		//�T�C�h�o�[�w�i
		NVI_SetControlPosition(kControlID_RightSideBarBackground,inLayout.pt_RightSideBar);
		NVI_SetControlSize(kControlID_RightSideBarBackground,inLayout.w_RightSideBar,inLayout.h_RightSideBar_WithBottomMargin);
		NVI_SetShowControl(kControlID_RightSideBarBackground,true);
		
		//�E�T�C�h�o�[�����ڂ������ꍇ����
		if( mRightSideBarArray_Type.GetItemCount() == 0 )
		{
			AWindowRect	eraserect = {0};
			eraserect.left		= inLayout.pt_RightSideBar.x;
			eraserect.top		= inLayout.pt_RightSideBar.y;
			eraserect.right		= inLayout.pt_RightSideBar.x + inLayout.w_RightSideBar;
			eraserect.bottom	= inLayout.pt_RightSideBar.y + inLayout.h_RightSideBar;
			NVI_PaintRect(eraserect,kColor_White);
		}
		
		//�E�T�C�h�o�[������
		if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )//#319
		{
			NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
						inLayout.pt_VSeparator_RightSideBar,kSeparatorWidth,inLayout.h_RightSideBar);
			NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_Show(false);
			//
			ASeparatorLinePosition	separatorLinePosition = kSeparatorLinePosition_None;
			NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).SPI_SetLinePosition(separatorLinePosition);
		}
		
		//�T�u�E�C���h�E�e�����␳
		UpdateLayout_AdjustRightSideBarHeight(inLayout.h_RightSideBar);
		
		//�J�ny�ʒu�擾
		AFloatNumber	infopaney = inLayout.pt_RightSideBar.y;
		//�eInfo Window
		for( AIndex infoPaneIndex = 0; infoPaneIndex < mRightSideBarArray_Type.GetItemCount(); infoPaneIndex++ )
		{
			//�T�C�h�o�[���ڍ����擾
			AFloatNumber	infoPaneHeight = mRightSideBarArray_Height.Get(infoPaneIndex);
			if( mRightSideBarArray_Collapsed.Get(infoPaneIndex) == true )
			{
				//�܂肽���ݎ�
				infoPaneHeight = GetApp().SPI_GetSubWindowCollapsedHeight();
			}
			//���̍���y�ʒu
			AFloatNumber	nexty = infopaney + infoPaneHeight;
			ANumber	intNextY = nexty;
			//�T�C�h�o�[���ڃf�[�^�擾
			ASubWindowType	type = mRightSideBarArray_Type.Get(infoPaneIndex);
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(infoPaneIndex);
			ABool	dragging = mRightSideBarArray_Dragging.Get(infoPaneIndex);
			if( winID != kObjectID_Invalid )
			{
				AWindowPoint	pt = inLayout.pt_RightSideBar;
				pt.x += k_wRightSideBarVisibleArea;
				pt.y = infopaney;
				//�h���b�O���ȊO�̏ꍇ�A�I�[�o�[���C�E�C���h�E�z�u
				//�i�h���b�O���̃I�[�o�[���C�E�C���h�E�̏ꍇ�́A�z�u�����Ȃ��B�j
				if( dragging == false )
				{
					//�T�u�E�C���h�E�z�u
					GetApp().SPI_UpdateLayoutOverlaySubWindow(GetObjectID(),type,winID,
															  pt,inLayout.w_RightSideBar - k_wRightSideBarVisibleArea,intNextY-pt.y,inAnimate);
					//�W�����v���X�g�̏ꍇ�A�^�uselect
					switch(type)
					{
					  case kSubWindowType_JumpList:
						{
							if( NVI_GetCurrentTabIndex() == inTabIndex )
							{
								GetApp().SPI_GetJumpListWindow(winID).NVI_SelectTab(inTabIndex);
							}
							break;
						}
					  default:
						{
							//�����Ȃ�
							break;
						}
					}
				}
			}
			//����y��
			infopaney = nexty;
		}
	}
	else
	{
		//========== �E�T�C�h�o�[��\�����Ȃ��ꍇ ==========
		
		//�T�C�h�o�[�w�i
		NVI_SetShowControl(kControlID_RightSideBarBackground,false);
		
		//�E�T�C�h�o�[��������\��
		if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )
		{
			NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_Hide();
		}
	}
}

/**
���T�C�h�o�[�z�u
*/
ABool	AWindow_Text::UpdateLayout_LeftSideBar( const AIndex inTabIndex, const ATextWindowLayoutData& inLayout )
{
	//#675 UpdateViewBounds�}����Ԓ��͉������Ȃ�
	if( mSuppressUpdateViewBoundsCounter > 0 )   return false;
	
	ABool	subTextPaneDisplayed = false;
	if( inLayout.w_LeftSideBar > 0 )
	{
		//========== ���T�C�h�o�[��\������ꍇ ==========
		
		//�T�C�h�o�[�w�i
		NVI_SetControlPosition(kControlID_LeftSideBarBackground,inLayout.pt_LeftSideBar);
		NVI_SetControlSize(kControlID_LeftSideBarBackground,inLayout.w_LeftSideBar,inLayout.h_LeftSideBarWithMargin);
		NVI_SetShowControl(kControlID_LeftSideBarBackground,true);
		
		//���T�C�h�o�[�����ڂ������ꍇ����
		if( mLeftSideBarArray_Type.GetItemCount() == 0 )
		{
			AWindowRect	eraserect = {0};
			eraserect.left		= inLayout.pt_LeftSideBar.x;
			eraserect.top		= inLayout.pt_LeftSideBar.y;
			eraserect.right		= inLayout.pt_LeftSideBar.x + inLayout.w_LeftSideBar;
			eraserect.bottom	= inLayout.pt_LeftSideBar.y + inLayout.h_LeftSideBar;
			NVI_PaintRect(eraserect,kColor_White);
		}
		
		//���T�C�h�o�[������
		if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
		{
			AWindowPoint	pt = {0};
			NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
											inLayout.pt_VSeparator_LeftSideBar,kSeparatorWidth,inLayout.h_LeftSideBar);
			NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_Show(false);
			NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).SPI_SetLinePosition(kSeparatorLinePosition_None);
		}
		
		//�T�u�E�C���h�E�e�����␳
		UpdateLayout_AdjustLeftSideBarHeight(inLayout.h_LeftSideBar);
		
		//�J�ny�ʒu�擾
		AFloatNumber	subpaney = inLayout.pt_LeftSideBar.y;
		//�e�T�u�E�C���h�E�����[�v
		for( AIndex subPaneIndex = 0; subPaneIndex < mLeftSideBarArray_Type.GetItemCount(); subPaneIndex++ )
		{
			//�T�C�h�o�[���ڍ����擾
			AFloatNumber	subPaneHeight = mLeftSideBarArray_Height.Get(subPaneIndex);
			if( mLeftSideBarArray_Collapsed.Get(subPaneIndex) == true )
			{
				//�܂肽���ݎ�
				subPaneHeight = GetApp().SPI_GetSubWindowCollapsedHeight();
			}
			//���̍���y�ʒu
			AFloatNumber	nexty = subpaney + subPaneHeight;
			ANumber	intNextY = nexty;
			//�T�C�h�o�[���ڃf�[�^�擾
			ASubWindowType	type = mLeftSideBarArray_Type.Get(subPaneIndex);
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(subPaneIndex);
			ABool	dragging = mLeftSideBarArray_Dragging.Get(subPaneIndex);
			if( winID != kObjectID_Invalid )
			{
				AWindowPoint	pt = inLayout.pt_LeftSideBar;
				pt.y = subpaney;
				//�h���b�O���ȊO�̏ꍇ�A�I�[�o�[���C�E�C���h�E�z�u
				//�i�h���b�O���̃I�[�o�[���C�E�C���h�E�̏ꍇ�́A�z�u�����Ȃ��B�j
				if( dragging == false )
				{
					//�T�u�E�C���h�E�z�u
					GetApp().NVI_GetWindowByID(winID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),pt,
								inLayout.w_LeftSideBar - k_wLeftSideBarVisibleArea,intNextY-pt.y);
					switch(type)
					{
					  case kSubWindowType_SubTextPane:
						{
							//�T�u�e�L�X�g�y�C���z�u
							UpdateLayout_SubTextPane(inTabIndex,pt,
													 inLayout.w_LeftSideBar - k_wLeftSideBarVisibleArea,subPaneHeight,
													 inLayout.h_subHScrollbar);
							//�T�u�e�L�X�g�y�C���\���t���OON
							subTextPaneDisplayed = true;
							break;
						}
						//�W�����v���X�g�̏ꍇ�A�^�uselect
					  case kSubWindowType_JumpList:
						{
							if( NVI_GetCurrentTabIndex() == inTabIndex )
							{
								GetApp().SPI_GetJumpListWindow(winID).NVI_SelectTab(inTabIndex);
							}
							break;
						}
					  default:
						{
							//�����Ȃ�
							break;
						}
					}
				}
			}
			//����y��
			subpaney = nexty;
		}
	}
	else
	{
		//========== ���T�C�h�o�[��\�����Ȃ��ꍇ ==========
		
		//�T�C�h�o�[�w�i
		NVI_SetShowControl(kControlID_LeftSideBarBackground,false);
		
		//�T�u�y�C���J������������\��
		if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
		{
			NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_Hide();
		}
	}
	return subTextPaneDisplayed;
}

/**
�s�ԍ��E�ꏊ�{�^���z�u
*/
void	AWindow_Text::UpdateLayout_LineNumberAndPositionButton( const AIndex inTabIndex, const ATextWindowLayoutData& layout )
{
	//�E�C���h�E���E�����擾
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//�X�e�[�^�X�o�[��\�����邩�ǂ������擾
	ABool	showStatusBar = (layout.h_Footer > 0 );
	
	//�t�b�^�[�̊e�{�^����y�ʒu�I�t�Z�b�g
	const ANumber	kFooterButtonsYOffset = 2;
	
	//�s�ԍ��{�^��
	if( showStatusBar == true )
	{
		AWindowPoint	pt = {0};
		pt.x = layout.pt_MainColumn.x;// + kWidth_FooterSeparator;
		//#1184 if( mSubTextColumnDisplayed == false && mLeftSideBarDisplayed == false )
		{
			pt.x += kWidth_LeftSideBar_ShowHideButton;
		}
		pt.y = windowBounds.bottom - windowBounds.top - layout.h_Footer + kFooterButtonsYOffset;
		NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_LineNumberButton,pt);
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LineNumberButton,true);
	}
	else
	{
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LineNumberButton,false);
	}
	//�ꏊ�{�^��
	if( showStatusBar == true )
	{
		AWindowPoint	pt = {0};
		pt.x = layout.pt_MainColumn.x + NVI_GetControlWidth(kControlID_LineNumberButton);// + kWidth_FooterSeparator*2;
		//#1184 if( mSubTextColumnDisplayed == false && mLeftSideBarDisplayed == false )
		{
			pt.x += kWidth_LeftSideBar_ShowHideButton;
		}
		pt.y = windowBounds.bottom - windowBounds.top - layout.h_Footer + kFooterButtonsYOffset;
		NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_PositionButton,pt);
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_PositionButton,true);
		NVI_GetButtonViewByControlID(kControlID_PositionButton).
				SPI_SetWidthToFitTextWidth(layout.pt_MainColumn.x + layout.w_MainColumn - pt.x - 32);
	}
	else
	{
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_PositionButton,false);
	}
	
	//#725
	//�T�u�e�L�X�g�s�ԍ��{�^��
	if( mSubTextColumnDisplayed == true && showStatusBar == true )
	{
		AWindowPoint	pt = {0};
		pt.x = layout.pt_SubTextPaneColumn.x;// + kWidth_FooterSeparator;
		//#1184 if( mLeftSideBarDisplayed == false )
		{
			pt.x += kWidth_LeftSideBar_ShowHideButton;
		}
		pt.y = windowBounds.bottom - windowBounds.top - layout.h_Footer + kFooterButtonsYOffset;
		NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_LineNumberButton_SubText,pt);
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LineNumberButton_SubText,true);
	}
	else
	{
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LineNumberButton_SubText,false);
	}
	//�T�u�e�L�X�g�̏ꏊ�{�^��
	if( mSubTextColumnDisplayed == true && showStatusBar == true )
	{
		AWindowPoint	pt = {0};
		pt.x = layout.pt_SubTextPaneColumn.x + NVI_GetControlWidth(kControlID_LineNumberButton_SubText);// + kWidth_FooterSeparator*2;
		//#1184 if( mLeftSideBarDisplayed == false )
		{
			pt.x += kWidth_LeftSideBar_ShowHideButton;
		}
		pt.y = windowBounds.bottom - windowBounds.top - layout.h_Footer + kFooterButtonsYOffset;
		NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_PositionButton_SubText,pt);
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_PositionButton_SubText,true);
		NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).
				SPI_SetWidthToFitTextWidth(layout.pt_SubTextPaneColumn.x + layout.w_SubTextPaneColumn - pt.x - 32);
	}
	else
	{
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_PositionButton_SubText,false);
	}
}

/**
�T�u�E�C���h�E�\���E��\���X�V
*/
void	AWindow_Text::UpdateVisibleSubWindows()
{
	//���T�C�h�o�[�̕\���E��\���ɏ]���āA�e�T�u�E�C���h�E�\���E��\��
	for( AIndex subPaneIndex = 0; subPaneIndex < mLeftSideBarArray_Type.GetItemCount(); subPaneIndex++ )
	{
		AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(subPaneIndex);
		if( winID != kObjectID_Invalid )
		{
			if( mLeftSideBarDisplayed == true )
			{
				GetApp().NVI_GetWindowByID(winID).NVI_Show(false);
			}
			else
			{
				GetApp().NVI_GetWindowByID(winID).NVI_Hide();
			}
		}
	}
	//�E�T�C�h�o�[�̕\���E��\���ɏ]���āA�e�T�u�E�C���h�E�\���E��\��
	for( AIndex infoPaneIndex = 0; infoPaneIndex < mRightSideBarArray_OverlayWindowID.GetItemCount(); infoPaneIndex++ )
	{
		AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(infoPaneIndex);
		if( winID != kObjectID_Invalid )
		{
			if( mRightSideBarDisplayed == true )
			{
				GetApp().NVI_GetWindowByID(winID).NVI_Show(false);
			}
			else
			{
				GetApp().NVI_GetWindowByID(winID).NVI_Hide();
			}
		}
	}
}

/**
�eview bindings�X�V
*/
void	AWindow_Text::UpdateViewBindings( const ATextWindowViewBindings inBindings )
{
	//�E�C���h�E�w�i�r���[
	if( true )
	{
		//�X�N���[���o�[����镔���̂ݔw�i�`��̏ꍇ
		NVI_SetControlBindings(kControlID_WindowBackground,false,false,true,true,true,true);
	}
	else
	{
		//�S�̂ɔw�i�`��̏ꍇ
		NVI_SetControlBindings(kControlID_WindowBackground,true,true,true,true,false,false);
	}
	//�e�^�u���Ƃ̃��[�v
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		//�c�������[�h�擾 #558
		ABool	verticalMode = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetVerticalMode();
		switch(inBindings)
		{
		  case kTextWindowViewBindings_Normal:
			{
				//�esplit view
				for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
				{
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(v),
										   true,true,true,true,false,false);//�㉺���E�o�C���h
					if( verticalMode == false )
					{
						//�ʏ�p�o�C���h
						NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(v),
											   true,true,false,true,true,false);//���o�C���h�A���Œ�A�㉺�o�C���h
					}
					else
					{
						//�c�����p�o�C���h #558
						NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(v),
											   true,true,true,false,false,true);//��o�C���h�A���E�o�C���h�A�����Œ�
					}
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetVScrollBarControlID(v),
										   false,true,true,true,true,false);//�E�o�C���h�A���Œ�A�㉺�o�C���h
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSplitButtonControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSeparatorControlID(v),
										   true,true,true,false,false,true);//��o�C���h�A���E�o�C���h�A�����Œ�
				}
				//���[���[
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetRulerControlID(),
									   true,true,true,false,false,true);//��o�C���h�A���E�o�C���h�A�����Œ�
				//�^�u�Z���N�^
				NVI_SetControlBindings(kControlID_TabSelector,
									   true,true,true,false,false,true);//��o�C���h�A���E�o�C���h�A�����Œ�
				//H�X�N���[���o�[
				NVI_SetControlBindings(mTabDataArray.GetObjectConst(tabIndex).GetHScrollbarControlID(),
									   true,false,true,true,false,true);//���o�C���h�A���E�o�C���h�A�����Œ�
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextHScrollbarControlID(),
									   true,false,true,true,false,true);//���o�C���h�A���o�C���h�A�T�C�Y�Œ�
				//�T�u�e�L�X�g view
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextControlID(),
									   true,true,false,true,true,false);//���o�C���h�A���Œ�A�㉺�o�C���h
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID(),
									   true,true,false,true,true,false);//���o�C���h�A���Œ�A�㉺�o�C���h
				//�T�u�e�L�X�g����{�^��
				NVI_SetControlBindings(kSubPaneFileNameButtonControlID,
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneSyncButtonControlID,
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneSwapButtonControlID,
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneDiffButtonControlID,
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				break;
			}
		  case kTextWindowViewBindings_ShowHideLeftSidebar:
			{
				//�esplit view
				for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
				{
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetVScrollBarControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSplitButtonControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSeparatorControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				}
				//���[���[
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetRulerControlID(),
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				//�^�u�Z���N�^
				NVI_SetControlBindings(kControlID_TabSelector,
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				//H�X�N���[���o�[
				NVI_SetControlBindings(mTabDataArray.GetObjectConst(tabIndex).GetHScrollbarControlID(),
									   true,false,true,true,false,true);//�E��o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextHScrollbarControlID(),
									   true,false,true,true,false,true);//�E��o�C���h�A�T�C�Y�Œ�
				//�T�u�e�L�X�g view
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextControlID(),
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID(),
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				//�T�u�e�L�X�g����{�^��
				NVI_SetControlBindings(kSubPaneFileNameButtonControlID,
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneSyncButtonControlID,
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneSwapButtonControlID,
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneDiffButtonControlID,
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				break;
			}
		  case kTextWindowViewBindings_ShowRightSidebar:
		  case kTextWindowViewBindings_HideRightSidebar:
			{
				//�esplit view
				for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
				{
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(v),
										   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(v),
										   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetVScrollBarControlID(v),
										   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSplitButtonControlID(v),
										   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSeparatorControlID(v),
										   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				}
				//���[���[
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetRulerControlID(),
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				//�^�u�Z���N�^
				NVI_SetControlBindings(kControlID_TabSelector,
									   true,true,true,false,false,true);//��o�C���h�A���E�o�C���h�A�����Œ�
				//H�X�N���[���o�[
				NVI_SetControlBindings(mTabDataArray.GetObjectConst(tabIndex).GetHScrollbarControlID(),
									   true,false,true,true,false,true);//����o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextHScrollbarControlID(),
									   true,false,true,true,false,true);//����o�C���h�A�T�C�Y�Œ�
				//�T�u�e�L�X�g view
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextControlID(),
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID(),
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				//�T�u�e�L�X�g����{�^��
				NVI_SetControlBindings(kSubPaneFileNameButtonControlID,
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneSyncButtonControlID,
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneSwapButtonControlID,
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneDiffButtonControlID,
									   true,true,false,false,true,true);//����o�C���h�A�T�C�Y�Œ�
				break;
			}
		  case kTextWindowViewBindings_ShowSubTextColumn:
		  case kTextWindowViewBindings_HideSubTextColumn:
			{
				//�esplit view
				for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
				{
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetVScrollBarControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSplitButtonControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSeparatorControlID(v),
										   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				}
				//���[���[
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetRulerControlID(),
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				//�^�u�Z���N�^
				NVI_SetControlBindings(kControlID_TabSelector,
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				//H�X�N���[���o�[
				NVI_SetControlBindings(mTabDataArray.GetObjectConst(tabIndex).GetHScrollbarControlID(),
									   true,false,true,true,false,true);//�E��o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextHScrollbarControlID(),
									   true,false,true,true,false,true);//�E��o�C���h�A�T�C�Y�Œ�
				//�T�u�e�L�X�g view
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextControlID(),
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID(),
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				//�T�u�e�L�X�g����{�^��
				NVI_SetControlBindings(kSubPaneFileNameButtonControlID,
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneSyncButtonControlID,
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneSwapButtonControlID,
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				NVI_SetControlBindings(kSubPaneDiffButtonControlID,
									   false,true,true,false,true,true);//�E��o�C���h�A�T�C�Y�Œ�
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
}

//
void	AWindow_Text::NVIDO_Show()
{
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_Text::NVIDO_Hide()
{
	ARect	bounds;
	//#1238 kSingleWindowBounds�ɂ̓T�C�h�o�[�Ȃ��̃T�C�Y��ۑ�����悤�ɂ���B NVI_GetWindowBounds(bounds);
	SPI_GetWindowMainColumnBounds(bounds);
	//#404 �t���X�N���[�����[�h�ݒ�ۑ�
	//#476 if( mTabModeMainWindow == true )
	{
		ABool	fullScreenMode = NVI_GetFullScreenMode();
		//#476 GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kFullScreenMode,fullScreenMode);
		if( fullScreenMode == true )
		{
			//�t���X�N���[���������̃E�C���h�E�̈�̂ق���ۑ�����
			NVI_GetWindowBoundsInStandardState(bounds);
		}
	}
	//
	if( /*#850 GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true &&
		SPI_IsTabModeMainWindow() == true &&//#569 �^�u���C���E�C���h�E�ȊO�ł�singleWindowBounds�L�����Ȃ�*/
				mLuaConsoleMode == false )//#567 LuaConsole�ł�singleWindowBounds�L�����Ȃ�
	{
		GetApp().GetAppPref().SetData_Rect(AAppPrefDB::kSingleWindowBounds,bounds);
		//�ő剻��� win
		NVI_GetWindowBoundsInStandardState(bounds);
		GetApp().GetAppPref().SetData_Rect(AAppPrefDB::kSingleWindowBounds_UnzoomedBounds,bounds);
		GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kSingleWindowBounds_Zoomed,NVI_IsZoomed());
		
		//�u�h�L�������g�̓^�u�ɊJ���v��ON�̂Ƃ��́A�E�C���h�E�ʒu���L������
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true )
		{
			ARect	maincolumnrect = {0};
			SPI_GetWindowMainColumnBounds(maincolumnrect);
			APoint	pt = {maincolumnrect.left,maincolumnrect.top};
			GetApp().GetAppPref().SetData_Point(AAppPrefDB::kNewDocumentWindowPoint,pt);
		}
	}
	//#567 LuaConsole���[�h�Ȃ�kLuaConsoleWindowPosition�����L��
	//#850 if( mLuaConsoleMode == true )
	else
	{
		APoint	pt = {0,0};
		pt.x = bounds.left;
		pt.y = bounds.top;
		GetApp().NVI_GetAppPrefDB().SetData_Point(AAppPrefDB::kLuaConsoleWindowPosition,pt);
		GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kLuaConsoleWindowWidth,bounds.right-bounds.left);
		GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kLuaConsoleWindowHeight,bounds.bottom-bounds.top);
	}
}

//�E�C���h�E�S�̂̕\���X�V
void	AWindow_Text::NVIDO_UpdateProperty()
{
	//View�̈ʒu�A�T�C�Y�X�V
	SPI_UpdateViewBounds();
	
	//
	/*�h�L�������g��NVI_UpdateProperty()�Ŏ��s�����for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		SPI_UpdateTextDrawProperty(NVI_GetDocumentIDByTabIndex(tabIndex));
	}*/
	
	//
	//#725 SPI_GetJumpListWindow().NVI_UpdateProperty();
	
	//�T�u�e�L�X�g����{�^���F�X�V
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetLetterColor(GetApp().SPI_GetSubTextColumnButtonLetterColor(),
																					 GetApp().SPI_GetSubTextColumnButtonLetterColorDeactivate());
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetDropShadowColor(GetApp().SPI_GetSubTextColumnButtonDropShadowColor());
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).SPI_SetLetterColor(GetApp().SPI_GetSubTextColumnButtonLetterColor(),
																				 GetApp().SPI_GetSubTextColumnButtonLetterColorDeactivate());
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).SPI_SetDropShadowColor(GetApp().SPI_GetSubTextColumnButtonDropShadowColor());
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).SPI_SetLetterColor(GetApp().SPI_GetSubTextColumnButtonLetterColor(),
																				 GetApp().SPI_GetSubTextColumnButtonLetterColorDeactivate());
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).SPI_SetDropShadowColor(GetApp().SPI_GetSubTextColumnButtonDropShadowColor());
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetLetterColor(GetApp().SPI_GetSubTextColumnButtonLetterColor(),
																				 GetApp().SPI_GetSubTextColumnButtonLetterColorDeactivate());
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetDropShadowColor(GetApp().SPI_GetSubTextColumnButtonDropShadowColor());
	
	//�T�C�h�o�[�w�i�F�X�V
	AColor	color = GetApp().SPI_GetSubWindowBackgroundColor(true);
	AColor	separatorColor = GetApp().SPI_GetSideBarFrameColor();
	NVI_SetPlainViewColor(kControlID_RightSideBarBackground,color,color,false,true,false,true,separatorColor);
	NVI_SetPlainViewColor(kControlID_LeftSideBarBackground,color,color,true,false,true,false,separatorColor);
	
}

ABool	AWindow_Text::SPI_IsDocumentVisible( const ADocumentID inDocumentID ) const
{
	return (NVI_GetCurrentDocumentID() == inDocumentID);
}

//B0411
ABool	AWindow_Text::NVIDO_IsChildWindowVisible() const
{
	if( SPI_GetAskingSaveChanges() == true )   return true;
	/*#694
	if( SPI_GetMoveLieWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetFontSizeWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetWrapLetterCountWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetCorrectEncodingWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetPrintOptionWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetPropertyWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetDropWarningWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetAddToolButtonWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetEditByOtherAppWindowConst().NVI_IsWindowVisible() == true )   return true;//R0232
	if( SPI_GetSCMCommitWindowConst().NVI_IsWindowVisible() == true )   return true;//#455
	*/
	return false;
}

//#404
/**
�t���X�N���[�����[�h�ؑ�
*/
/*#476
void	AWindow_Text::SPI_SwitchFullScreenMode()
{
	if( SPI_IsTabModeMainWindow() == false )   return;
	NVI_SetFullScreenMode(!NVI_GetFullScreenMode());
}
*/

//#675
/**
�I�[�o�[���C�E�C���h�E���܂߂đS��Refresh����
*/
void	AWindow_Text::SPI_RefreshWindowAll()
{
	NVI_RefreshWindow();
	//#725 SPI_GetJumpListWindow().NVI_RefreshWindow();
	//#725 GetApp().SPI_GetIdInfoWindow().NVI_RefreshWindow();
	//#725 GetApp().SPI_GetFileListWindow().NVI_RefreshWindow();
}

//#850
/**
���C���J������bounds�ɂ���āA�E�C���h�E�ʒu�E�T�C�Y��ݒ�
*/
void	AWindow_Text::SPI_SetWindowBoundsByMainColumnPosition( const ARect& inRect )
{
	//�T�C�h�o�[�E�T�u�e�L�X�g�\�����Ȃ�Abounds�����̕��g������
	ARect	bounds = inRect;
	if( mLeftSideBarDisplayed == true )
	{
		bounds.left -= mLeftSideBarWidth;
	}
	if( mSubTextColumnDisplayed == true )
	{
		bounds.left -= mSubTextColumnWidth;
	}
	if( mRightSideBarDisplayed == true )
	{
		bounds.right += mRightSideBarWidth;
	}
	//�E�C���h�Ebounds�ݒ�
	NVI_SetWindowBounds(bounds);
}

//#850
/**
���C���J������bounds���擾
*/
void	AWindow_Text::SPI_GetWindowMainColumnBounds( ARect& outRect ) const
{
	//�E�C���h�Ebounds�擾
	NVI_GetWindowBounds(outRect);
	//�T�C�h�o�[�E�T�u�e�L�X�g�\�����Ȃ�Abounds�����̕��k�߂�
	if( mLeftSideBarDisplayed == true )
	{
		outRect.left += mLeftSideBarWidth;
	}
	if( mSubTextColumnDisplayed == true )
	{
		outRect.left += mSubTextColumnWidth;
	}
	if( mRightSideBarDisplayed == true )
	{
		outRect.right -= mRightSideBarWidth;
	}
}

#pragma mark ===========================

#pragma mark ---�^�u����

//�V�K�e�L�X�g�h�L�������g�^�u����
void	AWindow_Text::SPNVI_CreateTextDocumentTab( const ADocumentID inTextDocumentID, 
		const ABool inSelect, const ABool inSelectableTab )//#212 #379
{
	NVI_CreateTab(inTextDocumentID,inSelectableTab);//#379
	//B0411 �q�E�C���h�E�\�����̓^�u�ؑւ��Ȃ�
	if( NVI_IsChildWindowVisible() == false && inSelect == true ) //#212
	{
		NVI_SelectTab(NVI_GetTabCount()-1);
	}
	NVI_DrawControl(kControlID_TabSelector);
}

/**
�V�K�^�u����
@return outInitialFocusControlID ��������^�u�̃C���f�b�N�X�@�������ɗ������A�܂��ANVI_GetCurrentTabIndex()�͐�������^�u�̃C���f�b�N�X�ł͂Ȃ�
*/
void	AWindow_Text::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//TabData��V�K�ǉ�
	mTabDataArray.InsertNew1Object(inTabIndex);
	//�L�����b�g�^�I����DocPref����擾�i�C���[�K����TextPoint�������ꍇ�͕␳����B�j
	//DocPref�̃L�����b�g�^�I���ʒu��y�͍s�ԍ��ł͂Ȃ��Ēi���ԍ��ł���B�iAView_Text::SPI_SavePreProcess()�j
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	ABool	caretMode = GetApp().SPI_GetTextDocumentConstByID(docID).GetDocPrefDBConst().GetData_Bool(ADocPrefDB::kCaretMode);
	ATextPoint	caretTextPoint = {0,0}, selectTextPoint = {0,0};
	caretTextPoint.x	= GetApp().SPI_GetTextDocumentConstByID(docID).GetDocPrefDBConst().GetData_Number(ADocPrefDB::kCaretPointX);
	caretTextPoint.y	= GetApp().SPI_GetTextDocumentConstByID(docID).GetDocPrefDBConst().GetData_Number(ADocPrefDB::kCaretPointY);
	//#699 �h�L�������g��������͍s�܂�Ԃ��v�Z�������Ă��Ȃ��̂ŁA�����ł�SPI_CorrectTextPoint()�͎��s���Ȃ��B
	//�s�܂�Ԃ����������́A�L�����b�g�E�I��ݒ�͂��ꂸ�AAView_Text::SPI_DoWrapCalculated()�ŃL�����b�g�E�I��ݒ肳���̂ŁA
	//�s�܂�Ԃ��������AAView_Text::SPI_DoWrapCalculated()�ɂ�SPI_CorrectTextPoint()�����s����B
	//#699 GetApp().SPI_GetTextDocumentConstByID(docID).SPI_CorrectTextPoint(caretTextPoint);
	//
	selectTextPoint.x	= GetApp().SPI_GetTextDocumentConstByID(docID).GetDocPrefDBConst().GetData_Number(ADocPrefDB::kSelectPointX);
	selectTextPoint.y	= GetApp().SPI_GetTextDocumentConstByID(docID).GetDocPrefDBConst().GetData_Number(ADocPrefDB::kSelectPointY);
	//#699 GetApp().SPI_GetTextDocumentConstByID(docID).SPI_CorrectTextPoint(selectTextPoint);
	//#699 CreateSplitView()����caretMode�������������̂ŁAcaret mode�Ȃ�selectTextPoint�̒l��caretTextPoint�Ɠ����ɂ���B
	if( caretMode == true )
	{
		selectTextPoint = caretTextPoint;
	}
	
	//#246
	AWindowPoint	pt = {0,0};
	
	//�c�[���o�[����
	AControlID	toolbarControlID = NVM_AssignDynamicControlID();
	AViewDefaultFactory<AView_Toolbar>	toolbarFactory(GetObjectID(),toolbarControlID);
	NVM_CreateView(toolbarControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,toolbarFactory);
	NVI_SetControlBindings(toolbarControlID,true,true,true,false,false,true);//#725
	NVM_RegisterViewInTab(inTabIndex,toolbarControlID);
	mTabDataArray.GetObject(inTabIndex).SetToolbarControlID(toolbarControlID);
	
	//���[���[����
	///*#725
	AControlID	rulerControlID = NVM_AssignDynamicControlID();
	AViewDefaultFactory<AView_Ruler>	rulerFactory(GetObjectID(),rulerControlID);
	NVM_CreateView(rulerControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,rulerFactory);
	NVM_RegisterViewInTab(inTabIndex,rulerControlID);
	mTabDataArray.GetObject(inTabIndex).SetRulerControlID(rulerControlID);
	
	//���C��HScrollbar����
	{
		AWindowPoint	pt = {0};
		AControlID	hscrollbarControlID = NVM_AssignDynamicControlID();
		NVI_CreateScrollBar(hscrollbarControlID,pt,20,10);
		NVM_RegisterViewInTab(inTabIndex,hscrollbarControlID);
		mTabDataArray.GetObject(inTabIndex).SetHScrollbarControlID(hscrollbarControlID);
	}
	
	//*/
	/*
	AWindowDefaultFactory<AWindow_Ruler>	rulerWindowFactory;
	AWindowID	rulerWindowID = GetApp().NVI_CreateWindow(rulerWindowFactory);
	GetApp().NVI_GetWindowByID(rulerWindowID).NVI_ChangeToOverlay(GetObjectID(),false);
	GetApp().NVI_GetWindowByID(rulerWindowID).NVI_Create(kObjectID_Invalid);
	NVM_RegisterOverlayWindow(rulerWindowID,inTabIndex,false);
	mTabDataArray.GetObject(inTabIndex).SetRulerWindowID(rulerWindowID);
	GetRulerView(inTabIndex).SPI_UpdateImageSize();
	*/
	//�W�����v���X�g
	//#725 SPI_GetJumpListWindow().NVI_CreateTab(docID);//#291
	//�W�����v���X�g�Ƀ^�u����
	GetApp().SPI_CreateJumpListWindowsTab(GetObjectID(),docID);//#725
	/*#725
	//�W�����v���X�g�\���t���O�ݒ�
	mTabDataArray.GetObject(inTabIndex).SetJumpListVisible(GetApp().SPI_GetModePrefDB(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex()).
				GetData_Bool(AModePrefDB::kDisplayJumpList));
	*/
	
	//�c�[���{�^������
	CreateToolButtons(inTabIndex);
	
	//�V�K�r���[����
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	ANumberArray	collapsedLinesArray;
	CreateSplitView(inTabIndex,0,windowBounds.bottom-windowBounds.top-kHScrollBarHeight,/*#699 caretMode,*/
					caretTextPoint,selectTextPoint,kImagePoint_00,collapsedLinesArray);
	//#1090
	//�ŏ��̃^�u�ɂ��ẮA������SPI_DoViewActivating()�i���h�L�������g�ǂݍ��݁j���s��
	//�i����SPI_DisplayInSubText()���ł�SPI_DoViewActivating()���s���A�T�u�e�L�X�g�\���������s��Ȃ��Ȃ����̂ŁA
	//�����Ŏ��s����悤�ɂ����j
	if( inTabIndex == 0 )
	{
		GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_DoViewActivating();
	}
	//#699
	//�����I���ʒu�ݒ�
	AControlID	textViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(0);
	GetTextViewByControlID(textViewControlID).SPI_SetSelectWithParagraphIndex(
				caretTextPoint.y,caretTextPoint.x,selectTextPoint.y,selectTextPoint.x);
	
	//#212 �T�u�e�L�X�g�y�C������
	//#725 �������̂͏�ɐ�������悤�ɂ���B�����폜�Bif( mLeftSideBarDisplayed == true )
	//{
	//
	//#699 ATextPoint	spt = {0,0}, ept = {0,0};
	CreateSubTextInEachTab(inTabIndex,/*#699 spt,ept,*/kImagePoint_00);
	//}
	
	//�c�[���{�^������
	//win090927 win CreateSplitView()��UpdateViewBounds()���Ă�ł���̂ŁA���toolButton������Ă����K�v������ CreateToolButtons(inTabIndex);
	//�����^�u���t�H�[�J�X�擾
	//#699 ��ֈړ� AControlID	textViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(0);
	outInitialFocusControlID = textViewControlID;
	
	/* CreateSplitView()����text view�̔z�u��ݒ肷��B
	//B0411 CreateSplitView()�Ő�������tab�ɂ���UpdateViewBounds()���Ă��܂��̂ŁA�ēx�A���݂�tab�ɂ���UpdateViewBounds()����B
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		UpdateViewBounds(NVI_GetCurrentTabIndex());
	}
	*/
	//#212
	UpdateSubTextFileNameButton();
	//#212 LastFocusedTextView��ݒ肵�Ă����B��Open�����Ƃ��Ɂu���ցv�{�^�������������삷��悤�ɁB
	//mTabDataArray.GetObject(inTabIndex).SetLastFocusedTextViewControlID(textViewControlID);
	mTabDataArray.GetObject(inTabIndex).SetLatentMainTextViewControlID(textViewControlID);
	mTabDataArray.GetObject(inTabIndex).SetLatentTextViewControlID(textViewControlID);
	
	//win �ŏ��ɊJ�����Ƃ��ɃT�u�y�C���ɂ��ŏ��̃^�u�̓��e��\������
	if( /*#725 mLeftSideBarDisplayed == true &&*/ mSubTextCurrentDocumentID == kObjectID_Invalid )
	{
		//#728
		//SPI_DisplayInSubText()������SPI_DoViewActivated()���Ă�ł��܂��̂ŁA
		//��Ƀ��C���r���[��SPI_DoViewActivated()���Ă�ł����A���C���r���[��ADocument_Text::InitViewData()���s���B
		//�i�T�u�e�L�X�gview�́A���CreateSubTextInEachTab()�Ŋ��ɐ����ς݁j
		//GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_DoViewActivating();
		//		GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0)).
		//		GetObjectID());
		
		//�T�u�e�L�X�g�ɍŏ��̃^�u�̓��e��\������
		SPI_DisplayInSubText(inTabIndex,false,kIndex_Invalid,false,false);
	}
	//#379
	GetTabSelector().SPI_DoTabCreated(inTabIndex);
	
	//#725
	UpdateViewBindings(kTextWindowViewBindings_Normal);
	
	//#688
	//�ŏ��ɐ��������ꍇ�A�c�[���o�[�̃��j���[�̃^�C�g����ݒ肷��B�i�N�����ɃA�v����deactivate���ƁAtab activated�����Ȃ��̂ŁA�󔒂̂܂܂ɂȂ��Ă��܂����߁j
	if( mTabDataArray.GetItemCount() == 1 )
	{
		UpdateToolbarItemValue(inTabIndex);
	}
}

/**
�^�u�폜
*/
void	AWindow_Text::NVIDO_DeleteTab( const AIndex inTabIndex )
{
	//#725
	//Tab�ɑ�����View, Overlay window��NVI_DeleteTabAndView()�ō폜�����
	//
	mTabDataArray.Delete1Object(inTabIndex);
	//#379
	GetTabSelector().SPI_DoTabDeleted(inTabIndex);
}

//#663
/**
�^�u�폜����������
*/
void	AWindow_Text::NVIDO_TabDeleted()
{
	//�^�u���������tabselector�̍������ς��\��������̂ŁAUpdateViewBounds()����
	UpdateViewBounds(NVI_GetCurrentTabIndex());
}

//#725
/**
�^�uselect������
��note �������䂳��Ȃ�view��show/hide�Ȃǂ����s
*/
void	AWindow_Text::NVIDO_SelectTab( const AIndex inTabIndex )
{
	//=====================�T�u�e�L�X�g�\���X�V=====================
	//�T�u�e�L�X�g�̓^�u��view�Ȃ̂ŁA�����I�Ƀ^�u�ɏ]���Ĕ�\���E�\������Ă��܂����߁A
	//�����Ō��݂̃T�u�e�L�X�g��\���X�V����
	AIndex	diffDocTabIndex = GetDiffDocumentTabIndexFromWorkingTabIndex(inTabIndex);//#379
	if( mDiffDisplayMode == true )//#379
	{
		//#379 �T�u�y�C����Diff�\��
		if( diffDocTabIndex != kIndex_Invalid )
		{
			SPI_UpdateDiffDisplay(NVI_GetDocumentIDByTabIndex(inTabIndex));
		}
		else
		{
			//diff�Ώۃh�L�������g�������ꍇ�̓T�u�y�C���ɂȂɂ��\�����Ȃ�
			
			//DiffInfo�E�C���h�E�͍X�V�i��r���h�L�������g����A��r�Ȃ��h�L�������g�ɐ؂�ւ����Ƃ��ɔ�r�����������߁j#611
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
		}
	}
	else
	{
		/*#844
		//#212 �T�u�y�C���\���X�V�i���[�h�ɉ����Ċetab��SubPaneText�̕\���E��\����؂�ւ��j
		if( mSubPaneMode != kSubPaneMode_Manual )
		{
			UpdateSubPaneAutomatic(mSubPaneMode,inTabSwitched);
		}
		else
		 */
		{
			AIndex	tabIndex = NVI_GetTabIndexByDocumentID(mSubTextCurrentDocumentID);
			if( tabIndex != kIndex_Invalid )
			{
				SPI_DisplayInSubText(tabIndex,false,kIndex_Invalid,false,false);
			}
		}
	}
}

//win
/**
Tab Activate��������
*/
void	AWindow_Text::NVMDO_DoTabActivating( const AIndex inTabIndex )
{
	//#379 �h�L�������g��ViewActivating��ʒm
	GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_DoViewActivating();
	//			GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0)).
	//			GetObjectID());
}

//Tab Activated���̏���

//inTabSwitched: true: �^�u���؂�ւ����  false: �^�u�͐؂�ւ�炸�E�C���h�E��activate���ꂽ
void	AWindow_Text::NVMDO_DoTabActivated( const AIndex inTabIndex, const ABool inTabSwitched )//win 080712
{
	if( NVI_IsWindowVisible() == false )   return;
	
	//#688
	if( NVI_GetCurrentTabIndex() == kIndex_Invalid )   return;
	
	/*#675 UpdateViewBounds()�ֈړ�
	//ViewClose�{�^��Overlay��S��Hide����BUpdateViewBounds()�Ŏ��^�u�����\�� win
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
		{
			GetApp().NVI_GetWindowByID(mTabDataArray.GetObject(tabIndex).GetCloseViewButtonWindowID(v)).NVI_Hide();
		}
	}
	*/
	//B0442
	//#725 GetApp().SPI_UpdateFloatingWindowHideAll();
	//
	if( inTabSwitched == true )//#700 Spaces�ł̕�space�ł̃t�@�C���I�[�v������space�J��Ԃ��ړ��h�~
	{
		UpdateViewBounds(inTabIndex);
	}
	//�^�u�Z���N�^�\���X�V
	NVI_DrawControl(kControlID_TabSelector);
	
	//
	AImagePoint	originOfLocalFrame;
	GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0)).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
	//#558 NVI_SetControlNumber(mTabDataArray.GetObjectConst(inTabIndex).GetHScrollbarControlID(),originOfLocalFrame.x);
	GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0)).NVI_UpdateScrollBar();//#558 �X�N���[���o�[�l�X�V��AView�ōs���i�c�����l���j
	//���[���[�X�V
	UpdateRulerProperty(inTabIndex);
	//#848 ���ֈړ����Afocus�̃h�L�������g�ɑ΂��ď�������悤�ύX�B GetApp().NVI_GetMenuManager().RealizeDynamicMenu(GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetJumpMenuObjectID());
	//�W�����v���X�g�X�V
	//focus�ɂ͌��݂�focus�̃e�L�X�g�r���[������B�i�T�u�e�L�X�g�y�C����focus�Ȃ炻�ꂪ����j
	AControlID	focus = GetTopMostFocusTextViewControlID();//#212 (inTabIndex);//#137 NVI_GetFocusInTab(inTabIndex);
	if( focus != kControlID_Invalid )
	{
		//#1091�΍�e�X�g
		NVI_SetShowControl(focus,true);
		//
		NVI_SwitchFocusTo(focus);//#212 �T�u�y�C���Ƀt�H�[�J�X���������ꍇ�́ANVI_SelectTab()�ŃT�u�y�C����hide����āAfocus�ݒ�ł��Ă��Ȃ��̂ł����Őݒ肷��K�v������
		//
		//#1091-test SPI_SetDefferedFocus(focus);
		//
		AIndex	tabIndex = NVI_GetTabIndexByDocumentID(SPI_GetTextDocument(focus).GetObjectID());//#212
		ATextPoint	spt, ept;
		GetTextViewByControlID(focus).SPI_GetSelect(spt,ept);
		//B0308
		AIndex	jumpMenuItemIndex = SPI_GetTextDocument(focus).SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
		if( jumpMenuItemIndex >= kLimit_MaxJumpMenuItemCount )//#695
		{
			jumpMenuItemIndex = kIndex_Invalid;
		}
		//#857 GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenu(kMenuItemID_Jump,jumpMenuItemIndex);
		GetApp().SPI_UpdateJumpMenu(SPI_GetTextDocument(focus).SPI_GetJumpMenuObjectID(),jumpMenuItemIndex);//#857
		/*#725
		//�t���[�e�B���O�W�����v���X�g�\������
		if( mFloatingJumpListWindowID != kObjectID_Invalid )
		{
			if( mTabDataArray.GetObjectConst(tabIndex).GetJumpListVisible() == true )
			{
				GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Show(false);//win
				GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_SelectTab(tabIndex);
			}
			else
			{
				//�W�����v���X�g���\��
				GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Hide();
			}
		}
		*/
		//�t���[�e�B���O�W�����v���X�g�\���E�z�u�X�V
		UpdateFloatingJumpListDisplay();
		//�W�����v���X�g�̃^�u�I�� #725
		GetApp().SPI_SelectJumpListWindowsTab(GetObjectID(),SPI_GetTextDocument(focus).GetObjectID());
		
		//
		//���[���[�̃L�����b�g�X�V
		if( tabIndex == inTabIndex && GetRulerView(inTabIndex).NVI_IsVisible() == true )//#212 #1090 ���[���[��\�����̓��[���[�ɃL�����b�g�ʒu�ݒ肵�Ȃ�
		{
			AImagePoint	spt, ept;
			GetTextViewByControlID(focus).SPI_GetImageSelect(spt,ept);
			GetRulerView(inTabIndex).SPI_UpdateCaretImageX(spt.x,ept.x);
			//
			AImagePoint	originOfLocalFrame;
			GetTextViewByControlID(focus).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
			originOfLocalFrame.y = 0;
			GetRulerView(inTabIndex).NVI_ScrollTo(originOfLocalFrame);
		}
	}
	//�u�����t�H���_�v���j���[�X�V
	AFileAcc	file;
	SPI_GetCurrentTabTextDocument().NVI_GetFile(file);
	AFileAcc	parent;
	parent.SpecifyParent(file);
	GetApp().SPI_UpdateSameFolderMenu(parent);
	//�u�����v���W�F�N�g�v�X�V
	AFileAcc	projectFolder;
	SPI_GetCurrentTabTextDocument().SPI_GetProjectFolder(projectFolder);
	GetApp().SPI_UpdateSameProjectMenu(projectFolder,SPI_GetCurrentTabTextDocument().SPI_GetModeIndex());
	//�֘A�t�@�C�����j���[�X�V
	GetApp().SPI_UpdateImportFileMenu(SPI_GetCurrentTabTextDocument().SPI_GetImportFileArray());
	//#81
	//�����o�b�N�A�b�v���j���[�X�V
	SPI_GetCurrentTabTextDocument().SPI_UpdateComopareWithAutoBackupMenu();
	//�t�@�C�����X�g�X�V
	//#725 GetApp().SPI_UpdateFileListWindow_DocumentActivated(SPI_GetCurrentTabTextDocument().GetObjectID());
	GetApp().SPI_UpdateFileListWindows(kFileListUpdateType_DocumentActivated,SPI_GetCurrentTabTextDocument().GetObjectID());//#725
	//�tⳎ����X�g�E�C���h�E�X�V #138
	//#858 GetApp().SPI_SetFusenListActiveDocument(/*#298 file*/SPI_GetCurrentTabTextDocument().SPI_GetProjectObjectID(),SPI_GetCurrentTabTextDocument().SPI_GetProjectRelativePath());
	//�c�[�����j���[�X�V
	GetApp().SPI_UpdateToolMenu(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex());
	//InfoHeader�\���X�V
	//#725 InfoHeader�p�~ SetInfoHeaderTextDefault();
	//�s�ԍ��{�^���̍X�V
	SPI_UpdateLineNumberButton(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0));
	//�ꏊ�{�^���̍X�V
	UpdatePositionButton(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0));
	//R0000
	SPI_GetCurrentTabTextDocument().SPI_UpdateFileAttribute();
	//#379 ��r�{�^���X�V
	/*if( mDiffDisplayMode == true &&
				GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )
	{
		SPI_SetDiffDisplayMode(false);
	}*/
	SPI_UpdateDiffButtonText();
	//#725
	//#725 UpdateViewBounds(inTabIndex);
	/*
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(inTabIndex,layout);
	UpdateLayout_LeftSideBar(inTabIndex,layout);
	UpdateLayout_RightSideBar(inTabIndex,layout);
	*/
	//#724
	//�c�[���o�[�l�X�V
	UpdateToolbarItemValue(inTabIndex);
	//#142
	//doc info�E�C���h�E�X�V
	SPI_GetCurrentTabTextDocument().SPI_UpdateDocInfoWindows();
	//���[�h���j���[�̃V���[�g�J�b�g�i���݂̃h�L�������g�̃��[�h�j�ݒ�
	GetApp().SPI_UpdateModeMenuShortcut();
}

//Tab Deactivated
void	AWindow_Text::NVMDO_DoTabDeactivated( const AIndex inTabIndex )
{
	if( NVI_IsWindowVisible() == false )   return;
	//�W�����v���j���[���̉�����
	GetApp().NVI_GetMenuManager().UnrealizeDynamicMenu(GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetJumpMenuObjectID());
	/*#291
	//�W�����v���X�g�X�N���[���ʒu�ۑ�
	APoint	pt;
	SPI_GetJumpListWindow().SPI_GetScrollPosition(pt);
	mTabDataArray.GetObject(inTabIndex).SetJumpListScrollPosition(pt);
	*/
	//��`�E�錾���j���[���폜
	GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_MoveToDefinition);
	//RC2 �ϊ����E�C���h�E����
	//#717 GetApp().SPI_GetCandidateListWindow().NVI_Hide();
	//�e��|�b�v�A�b�v��\��
	GetPopupIdInfoWindow().NVI_Hide();
	SPI_GetPopupCandidateWindow().NVI_Hide();
	//#81
	//���ۑ��f�[�^�ۑ��g���K�[
	SPI_GetCurrentFocusTextDocument().SPI_AutoSave_Unsaved();
}

//
void	AWindow_Text::SPI_CloseTab( const AIndex inTabIndex )//win 080709 ABool��void�i�Ԃ�l���g�p�̂��߁j
{
	TryClose(inTabIndex);
}

//
void	AWindow_Text::SPI_TryCloseDocument( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex == kIndex_Invalid )   return;//#909    {_ACThrow("",this);}
	TryClose(tabIndex);
}

//
void	AWindow_Text::SPI_ExecuteCloseDocument( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex == kIndex_Invalid )   return;//#909    {_ACThrow("",this);}
	ExecuteClose(inDocumentID);
}

//#357
void	AWindow_Text::SPI_SwitchTabNext()
{
	/*#559
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	tabIndex++;
	for( ; tabIndex != NVI_GetCurrentTabIndex(); tabIndex++ )//#379
	{
		if( tabIndex >= NVI_GetTabCount() )
		{
			tabIndex = 0;
		}
		if( NVI_GetTabSelectable(tabIndex) == true )   break;//#379
	}
	NVI_SelectTab(tabIndex);
	*/
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	if( NVI_GetNextDisplayTab(tabIndex,true) == true )
	{
		NVI_SelectTab(tabIndex);
	}
}

//#357
void	AWindow_Text::SPI_SwitchTabPrev()
{
	/*#559
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	tabIndex--;
	for( ; tabIndex != NVI_GetCurrentTabIndex(); tabIndex-- )//#379
	{
		if( tabIndex < 0 )
		{
			tabIndex = NVI_GetTabCount()-1;
		}
		if( NVI_GetTabSelectable(tabIndex) == true )   break;//#379
	}
	NVI_SelectTab(tabIndex);
	*/
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	if( NVI_GetPrevDisplayTab(tabIndex,true) == true )
	{
		NVI_SelectTab(tabIndex);
	}
}

//#513
/**
�^�uIndex����\��Index���擾
*/
AIndex	AWindow_Text::SPI_GetDisplayTabIndex( const AIndex inTabIndex ) const
{
	return GetTabSelectorConst().SPI_GetDisplayTabIndex(inTabIndex);
}

//#559
/**
���̕\���^�u���擾
*/
ABool	AWindow_Text::NVIDO_GetNextDisplayTab( AIndex& ioIndex, const ABool inLoop ) const
{
	//�\��index���擾
	AIndex	displayIndex = GetTabSelectorConst().SPI_GetDisplayTabIndex(ioIndex);
	//���̕\��index�ɑΉ�����tabIndex���擾
	displayIndex++;
	AIndex	tabIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(displayIndex);
	if( tabIndex == kIndex_Invalid )
	{
		//�������݂��Ȃ��i�Ō�j�̏ꍇ
		if( inLoop == true )
		{
			ioIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(0);
			return true;
		}
		else
		{
			return false;
		}
	}
	ioIndex = tabIndex;
	return true;
}

//#559
/**
�O�̕\���^�u���擾
*/
ABool	AWindow_Text::NVIDO_GetPrevDisplayTab( AIndex& ioIndex, const ABool inLoop ) const
{
	//�\��index���擾
	AIndex	displayIndex = GetTabSelectorConst().SPI_GetDisplayTabIndex(ioIndex);
	//�O�̕\��index�ɑΉ�����tabIndex���擾
	displayIndex--;
	AIndex	tabIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(displayIndex);
	if( tabIndex == kIndex_Invalid )
	{
		//�O�����݂��Ȃ��i�ŏ��j�̏ꍇ
		if( inLoop == true )
		{
			ioIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(GetTabSelectorConst().SPI_GetDisplayTabCount()-1);
			return true;
		}
		else
		{
			return false;
		}
	}
	ioIndex = tabIndex;
	return true;
}

//#850
/**
�^�u�̕\��index�擾
*/
AIndex	AWindow_Text::SPI_GetDisplayTabIndexByTabIndex( const AIndex inTabIndex ) const
{
	return GetTabSelectorConst().SPI_GetDisplayTabIndex(inTabIndex);
}

//#850
/**
�^�u�̕\��index�擾
*/
void	AWindow_Text::SPI_SetDisplayTabIndexByDocumentID( const ADocumentID inDocumentID, const AIndex inTabDisplayIndex )
{
	GetTabSelector().SPI_SetDisplayTabIndex(NVI_GetTabIndexByDocumentID(inDocumentID),inTabDisplayIndex);
}

//#1050
/**
�^�u�Z�b�g�ۑ�
*/
void	AWindow_Text::SPI_SaveTabSet()
{
	//�ۑ��_�C�A���O�\��
	AText	name;
	name.SetLocalizedText("TabSetFileDefaultName");
	AFileAttribute	attr;
	GetApp().SPI_GetExportFileAttribute(attr);
	AText	kind;
	kind.SetCstring("tabset");
	AText	filter;
	GetApp().SPI_GetDefaultFileFilter(filter);
	AFileAcc	defaultFolder;
	AFileWrapper::GetDesktopFolder(defaultFolder);
	NVI_ShowSaveWindow(name,kObjectID_Invalid,attr,filter,kind,defaultFolder);
}

//#1062
/**
�^�u���A���t�@�x�b�g���Ƀ\�[�g����
*/
void	AWindow_Text::SortTabs()
{
	//�^�u�^�C�g�����i�[���ă\�[�g���邽�߂̔z��
	ATextArray	titleArray;
	//�\���^�u���Ƃ̃��[�v
	AItemCount	tabDisplayCount = GetTabSelectorConst().SPI_GetDisplayTabCount();
	for( AIndex tabDisplayIndex = 0; tabDisplayIndex < tabDisplayCount; tabDisplayIndex++ )
	{
		//�^�u�^�C�g���擾
		AIndex	tabIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(tabDisplayIndex);
		ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
		AText	title;
		NVI_GetTitle(tabIndex,title);
		//�^�C�g�����̃^�u�R�[�h���X�y�[�X�ɕϊ��i�����Ǝv�����A�ꉞ�j
		title.ReplaceChar(kUChar_Tab,kUChar_Space);
		//�^�u�R�[�h�̌�ɁA�^�uindex��ǉ�
		title.AddCstring("\t");
		title.AddNumber(tabIndex);
		//�^�u�^�C�g���z��ɒǉ�
		titleArray.Add(title);
	}
	//�^�u�^�C�g�����\�[�g
	titleArray.Sort(true);
	
	//�\�[�g��̏��ԂɃ\�[�g���ꂽ�\�����z����i�[���邽�߂̔z��
	AArray<AIndex>	tabIndexArray;
	//�\�[�g��̔z��v�f���Ƃ̃��[�v
	for( AIndex tabDisplayIndex = 0; tabDisplayIndex < tabDisplayCount; tabDisplayIndex++ )
	{
		//�^�uindex�擾�p�ϐ�
		AIndex	tabIndex = kIndex_Invalid;
		//�\�[�g�z��̃e�L�X�g�擾
		AText	text;
		titleArray.Get(tabDisplayIndex,text);
		//�^�u�R�[�h��ɒǉ����ꂽ�A�^�uindex���擾�i�擾�ł��Ȃ��ꍇ�͉��������I���j
		AIndex	pos = 0;
		if( text.FindCstring(0,"\t",pos) == true )
		{
			pos++;
			text.ParseIntegerNumber(pos,tabIndex);
		}
		else
		{
			return;
		}
		if( tabIndex < 0 || tabIndex >= NVI_GetTabCount() )
		{
			return;
		}
		//�\�����z��ɒǉ�
		tabIndexArray.Add(tabIndex);
	}
	//�^�u�\�����X�V
	GetTabSelector().SPI_SetDisplayOrderArray(tabIndexArray);
}

#pragma mark ===========================

#pragma mark ---�^�u�؂藣���E����

//�w��^�u��ʂ̐V�K�E�C���h�E�փR�s�[�i�؂藣���j
void	AWindow_Text::SPI_CopyTabToWindow( const AIndex inTabIndex, const AWindowID inNewWindowID )
{
	if( NVI_GetTabSelectable(inTabIndex) == false )   return;//#379
	//#668 �E�C���h�Ebounds��DocPref�̃f�[�^�ɏ]���悤�ɂ���isubpane�̍l�����l����Ƃ�����̕����ǂ��j ARect	bounds;
	//#668 �E�C���h�Ebounds��DocPref�̃f�[�^�ɏ]���悤�ɂ��� NVI_GetWindowBounds(bounds);
	AWindow_Text&	newWindow = GetApp().SPI_GetTextWindowByID(inNewWindowID);
	newWindow.SPI_SetInitialSideBarDisplay(false,false,false);//#725 sidebar, subtext�\�����Ȃ�
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	newWindow.NVI_Create(docID);
	newWindow.NVI_SendBehind(GetObjectID());
	//#668 �E�C���h�Ebounds��DocPref�̃f�[�^�ɏ]���悤�ɂ��� newWindow.NVI_SetWindowBounds(bounds);
	//#379 diff�Ώۃh�L�������g������΁A�V�K�E�C���h�E��Diff�h�L�������g�̃^�u�i�I��s�^�u�j�𐶐�
	ADocumentID	diffTargetDocumentID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetDiffTargetDocumentID();
	if( diffTargetDocumentID != kObjectID_Invalid )
	{
		GetApp().SPI_CreateWindowOrTabForNewTextDocument(diffTargetDocumentID,
				kIndex_Invalid,kIndex_Invalid,false,false,inNewWindowID);
	}
	//#379 GetApp().SPI_GetTextDocumentByID(docID).SPI_RegisterWindow(inNewWindowID);
	SPI_CopyViewDataTo(inTabIndex,inNewWindowID,0);
	newWindow.NVI_Show(false);
}

//�w��^�u��ViewData���R�s�[����i�R�s�[���̃E�C���h�E�ŃR�[������j
void	AWindow_Text::SPI_CopyViewDataTo( const AIndex inSrcTabIndex, const AWindowID inDstWindowID, const AIndex inDstTabIndex )
{
	//View�̏����擾
	AArray<ANumber>	viewHeightArray;
	AArray<ATextPoint>	viewCaretTextPointArray;
	AArray<ATextPoint>	viewSelectTextPointArray;
	AArray<AImagePoint>	viewOriginOfLocalFrameArray;
	for( AIndex i = 0; i < mTabDataArray.GetObject(inSrcTabIndex).GetViewCount(); i++ )
	{
		viewHeightArray.Add(mTabDataArray.GetObjectConst(inSrcTabIndex).GetViewHeight(i));
		AControlID	textViewControlID = mTabDataArray.GetObjectConst(inSrcTabIndex).GetTextViewControlID(i);
		ATextPoint	spt, ept;
		GetTextViewByControlID(textViewControlID).SPI_GetSelect(spt,ept);
		viewCaretTextPointArray.Add(spt);
		viewSelectTextPointArray.Add(ept);
		AImagePoint	originOfLocalFrame;
		GetTextViewByControlID(textViewControlID).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
		viewOriginOfLocalFrameArray.Add(originOfLocalFrame);
	}
	//
	GetApp().SPI_GetTextWindowByID(inDstWindowID).
	CopyViewData(inDstTabIndex,viewHeightArray,viewCaretTextPointArray,viewSelectTextPointArray,viewOriginOfLocalFrameArray);
}
//�R�s�[��̃E�C���h�E�Ŏ��s���郁�\�b�h
void	AWindow_Text::CopyViewData( const AIndex inTabIndex, const AArray<ANumber>& inViewHeightArray, 
		const AArray<ATextPoint>& inViewCaretTextPointArray, const AArray<ATextPoint>& inViewSelectTextPointArray,
		const AArray<AImagePoint>& inViewOriginOfLocalFrameArray )
{
	//view0
	{
		mTabDataArray.GetObject(inTabIndex).SetViewHeight(0,inViewHeightArray.Get(0));
		AControlID	textViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(0);
		GetTextViewByControlID(textViewControlID).SPI_SetSelect(inViewCaretTextPointArray.Get(0),inViewSelectTextPointArray.Get(0));
		GetTextViewByControlID(textViewControlID).NVI_ScrollTo(inViewOriginOfLocalFrameArray.Get(0));
	}
	AItemCount	viewCount = inViewHeightArray.GetItemCount();
	for( AIndex i = 1; i < viewCount; i++ )
	{
		//�V�K�r���[����
		//#699 ABool	caretMode = false;
		ATextPoint	spt = inViewCaretTextPointArray.Get(i);
		ATextPoint	ept = inViewSelectTextPointArray.Get(i);
		ANumberArray	collapsedLinesArray;
		//#699 if( spt.x == ept.x && spt.y == ept.y )   caretMode = true;
		CreateSplitView(inTabIndex,i,inViewHeightArray.Get(i),/*#699 caretMode,*/spt,ept,inViewOriginOfLocalFrameArray.Get(i),collapsedLinesArray);
	}
}

//�E�C���h�E��V�K�^�u�ɃR�s�[�i�����j
void	AWindow_Text::SPNVI_CopyCreateTab( const AWindowID inSrcWindowID, const AIndex inSrcTabIndex )
{
	//diff�\�������Ɛ��䂪��₱�����Ȃ�̂ŁAdiff�\���͉�������
	SPI_SetDiffDisplayMode(false);
	//
	AWindow_Text&	srcWindow = GetApp().SPI_GetTextWindowByID(inSrcWindowID);
	if( srcWindow.NVI_GetTabSelectable(inSrcTabIndex) == false )   return;//#379
	ADocumentID	docID = srcWindow.NVI_GetDocumentIDByTabIndex(inSrcTabIndex);
	SPNVI_CreateTextDocumentTab(docID);
	//#379 GetApp().SPI_GetTextDocumentByID(docID).SPI_RegisterWindow(GetObjectID());
	srcWindow.SPI_CopyViewDataTo(inSrcTabIndex,GetObjectID(),NVI_GetTabCount()-1);
	//#379 diff�Ώۃh�L�������g������΁A�����P�V�K�^�u�i�I��s�j��������
	ADocumentID	diffTargetDocumentID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetDiffTargetDocumentID();
	if( diffTargetDocumentID != kObjectID_Invalid )
	{
		GetApp().SPI_CreateWindowOrTabForNewTextDocument(diffTargetDocumentID,
				kIndex_Invalid,kIndex_Invalid,false,false,GetObjectID());
	}
	//B0000 �s���Čv�Z����Ƃ��ɈȑO�̃E�C���h�E������ƁA������̃E�C���h�E���Ōv�Z���Ă��܂��̂ŁA��ɃN���[�Y
	GetApp().SPI_GetTextWindowByID(inSrcWindowID).SPI_CloseTab(inSrcTabIndex);
	//�s���Čv�Z�i�E�C���h�E���ς��̂Łj
	GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(NVI_GetTabCount()-1)).SPI_DoWrapRelatedDataChanged(false);
}

/*#850
//
void	AWindow_Text::SPI_SetTabModeMainWindow( const ABool inTabModeMainWindow )
{
	mTabModeMainWindow = inTabModeMainWindow;
	//
	if( NVI_IsWindowCreated() == true )
	{
		//#725 SPI_UpdateInfoPaneArray();
		//#725 SPI_UpdateSubPaneArray();
		SPI_UpdateViewBounds();
	}
}
*/
#pragma mark ===========================

#pragma mark ---�r���[����

//
void	AWindow_Text::CreateSplitView( const AIndex inTabIndex, const AIndex inViewIndex, const ANumber inViewHeight,
		//#699 const ABool inCaretMode, 
									  const ATextPoint& inCaretTextPoint, const ATextPoint& inSelectTextPoint, 
									  const AImagePoint& inOriginOfLocalFrame, const ANumberArray& inCollapsedLinesArray )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::CreateSplitView() started.",this);
	//�����r���[��View����
	
	//�i�������T�C�Y�A�ʒu�́ANVICB_Update()�Őݒ肳���B�j
	AWindowPoint	pt = {0,0};
	//TextView����
	//#844 ABool	crosscaretmode = false;
	/*#844
	//R0185
	if( mTabDataArray.GetObject(inTabIndex).GetViewCount() == 0 )
	{
		ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
		AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentConstByID(docID).SPI_GetModeIndex();
        //#844 crosscaretmode = GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kDefaultDisplayCrossCaret);
	}
	else
	{
        //#844 crosscaretmode = IsCrossCaretMode();
	}
	*/
	
	//ImageY�]���擾
	ANumber	imageYBottomMargin = kImageYBottomMarginForNormal;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kBottomScrollAt25Percent) == true )
	{
		imageYBottomMargin = kImageYBottomMarginFor25PercentMargin;
	}
	
	//�e�L�X�g�r���[�����i����0�ɐݒ肷��BAView_Text::SPI_DoWrapCalculated()�ɂĐ�����adjust scroll���邽�߁j
	AControlID	textViewControlID = NVM_AssignDynamicControlID();
	ATextViewFactory	textViewFactory(/*#199 this,*this*/GetObjectID(),textViewControlID,
						/*#695 NVI_GetDocumentIDByTabIndex(inTabIndex)*/
						GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)),//#695
						GetObjectID(),//#379
						/*#699 inCaretMode,*///#699 inCaretTextPoint,inSelectTextPoint,
                        inOriginOfLocalFrame,false);//#844 ,crosscaretmode);//R0185 IsCrossCaretMode());//B0345
	AViewID	textViewID = NVM_CreateView(textViewControlID,pt,0,10,kControlID_Invalid,kControlID_Invalid,true,textViewFactory);
	GetTextViewByControlID(textViewControlID).SPI_SetTextWindowID(GetObjectID());
	NVI_SetControlBindings(textViewControlID,false,true,true,true,true,false);
	GetTextViewByControlID(textViewControlID).NVI_SetImageYMargin(0,imageYBottomMargin);//#621
	//#450
	GetTextViewByControlID(textViewControlID).SPI_CollapseLines(inCollapsedLinesArray,true);
	GetTextViewByControlID(textViewControlID).NVI_ScrollTo(inOriginOfLocalFrame);
	//#699 �����I���ʒu�ݒ�B
	GetTextViewByControlID(textViewControlID).SPI_SetSelect(inCaretTextPoint,inSelectTextPoint);
	//LineNumberView����
	AImagePoint	originOfLocalFrame = inOriginOfLocalFrame;
	originOfLocalFrame.x = 0;
	AControlID	lineNumberControlID = NVM_AssignDynamicControlID();
	ALineNumberViewFactory	lineNumberViewFactory(/*#199 this,*this*/GetObjectID(),lineNumberControlID,/*#199 textViewControlID*/textViewID,NVI_GetDocumentIDByTabIndex(inTabIndex),originOfLocalFrame);
	AViewID	lineNumberViewID = NVM_CreateView(lineNumberControlID,pt,0,10,kControlID_Invalid,kControlID_Invalid,false,lineNumberViewFactory);
	GetLineNumberViewByControlID(lineNumberControlID).SPI_SetDiffDrawEnable();//#379
	GetLineNumberViewByControlID(lineNumberControlID).NVI_SetImageYMargin(0,imageYBottomMargin);//#621
	NVI_SetControlBindings(lineNumberControlID,false,true,true,true,true,false);
	GetTextViewByControlID(textViewControlID).SPI_SetLineNumberView(lineNumberViewID);//#450
	//V�X�N���[���o�[����
	AControlID	vScrollBarID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(vScrollBarID,pt,10,20);
	//�r���[�����{�^������
	AControlID	splitButtonID = NVM_AssignDynamicControlID();
	AText	text;
	NVI_CreateButtonView(splitButtonID,pt,10,10,kControlID_Invalid);
	//#725 NVI_GetButtonViewByControlID(splitButtonID).SPI_SetIcon(kIconID_Mi_SplitView,0,0,16,16);
	NVI_GetButtonViewByControlID(splitButtonID).SPI_SetButtonViewType(kButtonViewType_ScrollBar);
	NVI_GetButtonViewByControlID(splitButtonID).SPI_SetIcon(kImageID_iconPnSeparateVertical,0,1);//#725
	text.SetLocalizedText("HelpTag_SplitView");//#661
	NVI_GetButtonViewByControlID(splitButtonID).SPI_SetHelpTag(text,text);//#661
	/*
	NVI_CreateRoundedRectButton(splitButtonID,pt,10,10);
	*/
	//�r���[�����{�^������
	//#844 �r���[�����{�^���͎g�p���Ȃ����ƂƂ���B�i��Ɂu����v�{�^���̕����g�p�j�B�����A�����폜�̍H���Z�k�̂��߁A�T�C�Y0,0�Ŏc��
	AControlID	concatButtonID = NVM_AssignDynamicControlID();
	NVI_CreateButtonView(concatButtonID,pt,0,0,kControlID_Invalid);
	NVI_GetButtonViewByControlID(concatButtonID).SPI_SetButtonViewType(kButtonViewType_NoFrame);
	//#688 NVI_GetButtonViewByControlID(concatButtonID).SPI_SetIcon(kIconID_Mi_Close,0,0,16,16);
	text.SetLocalizedText("HelpTag_CloseSplitView");//#661
	NVI_GetButtonViewByControlID(concatButtonID).SPI_SetHelpTag(text,text);//#661
	//�r���[�����{�^����OverlayWindow�� win
	AWindowDefaultFactory<AWindow_ButtonOverlay>	closeViewButtonWindowFactory;
	AWindowID	closeViewButtonWindowID = GetApp().NVI_CreateWindow(closeViewButtonWindowFactory);
	NVI_GetButtonWindow(closeViewButtonWindowID).NVI_ChangeToOverlay(GetObjectID(),false);//win
	NVI_GetButtonWindow(closeViewButtonWindowID).NVI_Create(kObjectID_Invalid);
	NVM_RegisterOverlayWindow(closeViewButtonWindowID,inTabIndex,false);//#725 �e�E�C���h�E�ɓo�^�B�itab����ɂ�鎩��show/hide/delete���̂��߁j
	//#725 NVI_GetButtonWindow(closeViewButtonWindowID).NVI_Show(false);//�A�N�e�B�u�ɂ��Ȃ�
	NVI_GetButtonWindow(closeViewButtonWindowID).SPI_SetTargetWindowID(GetObjectID(),concatButtonID);
//	NVI_GetButtonWindow(closeViewButtonWindowID).SPI_SetIcon(kImageID_iconPnSeparateVertical,1,1);//#725
	NVI_GetButtonWindow(closeViewButtonWindowID).SPI_SetButtonViewType(kButtonViewType_NoFrame);
	NVI_GetButtonWindow(closeViewButtonWindowID).SPI_SetIcon(kImageID_btnEditorClose,0,0,19,19,kImageID_btnEditorClose_h);
	NVI_GetButtonWindow(closeViewButtonWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),pt,16,16);
	text.SetLocalizedText("HelpTag_CloseSplitView");//#661
	NVI_GetButtonWindow(closeViewButtonWindowID).SPI_SetHelpTag(text,text);//#661
	//�������r���[����
	AControlID	separatorControlID = NVM_AssignDynamicControlID();
	AViewDefaultFactory<AView_TextViewHSeparator>	separatorFactory(GetObjectID(),separatorControlID);//#199 #866
	NVM_CreateView(separatorControlID,pt,0,10,kControlID_Invalid,kControlID_Invalid,false,separatorFactory);//#866
	//#866 NVI_CreateHSeparatorView(separatorControlID,kSeparatorLinePosition_Middle,pt,0,10);
	
	//Tab��View�̃R���g���[��ID��o�^
	NVM_RegisterViewInTab(inTabIndex,textViewControlID);
	NVM_RegisterViewInTab(inTabIndex,lineNumberControlID);
	NVM_RegisterViewInTab(inTabIndex,vScrollBarID);
	NVM_RegisterViewInTab(inTabIndex,splitButtonID);
	NVM_RegisterViewInTab(inTabIndex,separatorControlID);
	NVM_RegisterViewInTab(inTabIndex,concatButtonID);
	
	//TabData�ɂP��ViewData�ǉ�
	mTabDataArray.GetObject(inTabIndex).InsertViewData(inViewIndex,
				textViewControlID,lineNumberControlID,vScrollBarID,splitButtonID,separatorControlID,concatButtonID,
			closeViewButtonWindowID,//win
			inViewHeight);
	
	//TextView�ɑΉ�����X�N���[���o�[�̃R���g���[��ID�ݒ�
	GetTextViewByControlID(textViewControlID).NVI_SetScrollBarControlID(mTabDataArray.GetObjectConst(inTabIndex).GetHScrollbarControlID(),vScrollBarID);
	
	//View�̈ʒu�A�T�C�Y�ݒ�
	//UpdateViewBounds(inTabIndex);
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(inTabIndex,layout);
	UpdateLayout_TextView(inTabIndex,layout.pt_MainColumn,layout.w_MainColumn,layout.h_MainColumn,
						  layout.w_MainTextView,layout.w_LineNumber,layout.w_MainTextMargin,layout.pt_VScrollbarArea);
	
	//TextView��Image�T�C�Y�ݒ�
	GetTextViewByControlID(textViewControlID).SPI_UpdateImageSize();
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::CreateSplitView() ended.",this);
}

//
void	AWindow_Text::SPI_ConcatSplitView()
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		AIndex	viewIndex = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).FindFromTextViewControlID(focus);
		//R0187 if( viewIndex > 0 )
		if( viewIndex != kIndex_Invalid )//#212
		{
			SPI_ConcatSplitView(viewIndex);
		}
	}
}
//
void	AWindow_Text::SPI_ConcatSplitView( const AIndex inViewIndex )
{
	if( inViewIndex > 0 )//R0187
	{
		SPI_SetViewHeight(inViewIndex-1,SPI_GetViewHeight(inViewIndex-1)+SPI_GetViewHeight(inViewIndex));
		SPI_DeleteSplitView(inViewIndex);
	}
	else if( mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetViewCount() >= 2 )//R0187
	{
		SPI_SetViewHeight(1,SPI_GetViewHeight(0)+SPI_GetViewHeight(1));
		SPI_DeleteSplitView(0);
	}
}
//
void	AWindow_Text::SPI_DeleteSplitView( const AIndex inViewIndex )
{
	//���݂̃t�H�[�J�X�̂���View���폜���悤�Ƃ��Ă���Ƃ��̓t�H�[�J�X�ړ�
	if( NVI_GetFocusInTab(NVI_GetCurrentTabIndex()) == mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(inViewIndex) )
	{
		AIndex	focusViewIndex = inViewIndex+1;
		if( focusViewIndex >= mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewCount() )
		{
			focusViewIndex = inViewIndex-1;
		}
		NVI_SwitchFocusTo(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(focusViewIndex));
	}
	//View�폜
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(inViewIndex));
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetLineNumberViewControlID(inViewIndex));
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetVScrollBarControlID(inViewIndex));
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetSplitButtonControlID(inViewIndex));
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetSeparatorControlID(inViewIndex));
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetConcatButtonControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetLineNumberViewControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetVScrollBarControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetSplitButtonControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetSeparatorControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetConcatButtonControlID(inViewIndex));
	//CloseView�{�^��OverlayWindow���폜 win #725
	AWindowID	closeViewButtonOverlayWindowID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetCloseViewButtonWindowID(inViewIndex);
	NVM_UnregisterOverlayWindow(closeViewButtonOverlayWindowID);//�q�I�[�o�[���C�E�C���h�E�o�^����
	GetApp().NVI_DeleteWindow(closeViewButtonOverlayWindowID);
	//ViewData�̍폜
	mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).DeleteViewData(inViewIndex);
	//View�̈ʒu�A�T�C�Y�ݒ�
	SPI_UpdateViewBounds();
}

/*#864
//�w��ControlID�������镪���r���[��Index���擾����
AIndex	AWindow_Text::SPI_GetViewIndexByControlID( const AControlID inControlID, AIndex& outTabIndex ) const//#864
{
	outTabIndex = kIndex_Invalid;
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		AIndex	viewIndex;
		viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromTextViewControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			outTabIndex = tabIndex;//#864
			return viewIndex;
		}
		viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromLineNumberViewControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			outTabIndex = tabIndex;//#864
			return viewIndex;
		}
		viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromVScrollBarControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			outTabIndex = tabIndex;//#864
			return viewIndex;
		}
		viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromSplitButtonControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			outTabIndex = tabIndex;//#864
			return viewIndex;
		}
		viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromSeparatorControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			outTabIndex = tabIndex;//#864
			return viewIndex;
		}
	}
	return kIndex_Invalid;
}
*/
//�w��Index�̕����r���[�̍������擾����
ANumber	AWindow_Text::SPI_GetViewHeight( const AIndex inViewIndex ) const
{
	return mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewHeight(inViewIndex);
}

//�w��Index�̕����r���[�̍�����ݒ肷��i�ݒ��ANVI_Update()�K�v�j
void	AWindow_Text::SPI_SetViewHeight( const AIndex inViewIndex, const ANumber inHeight )
{
	mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).SetViewHeight(inViewIndex,inHeight);
}

//#866
//
const ANumber	kMinHeight_TextView = 32;

//#866
/**
�e�L�X�g�r���[�c���������ݒ�
*/
ANumber	AWindow_Text::ChangeTextViewHeight( const AIndex inTabIndex, const AIndex inViewIndex, const ANumber inDelta,
										  const ABool inCompleted )
{
	//���X�̍������L��
	ANumber	origHeight = mTabDataArray.GetObjectConst(inTabIndex).GetViewHeight(inViewIndex-1);
	//�������ړ���̊e�������擾
	ANumber	aboveHeight = mTabDataArray.GetObjectConst(inTabIndex).GetViewHeight(inViewIndex-1) + inDelta;
	ANumber	height = mTabDataArray.GetObjectConst(inTabIndex).GetViewHeight(inViewIndex) - inDelta;
	//�����␳�i���view�̍ŏ��l�␳�j
	if( aboveHeight < kMinHeight_TextView )
	{
		ANumber	adjust = kMinHeight_TextView - aboveHeight;
		aboveHeight = kMinHeight_TextView;
		height += adjust;
	}
	//�����␳�i����view�̍ŏ��l�␳�j
	if( height < kMinHeight_TextView )
	{
		ANumber	adjust = kMinHeight_TextView - height;
		height = kMinHeight_TextView;
		aboveHeight += adjust;
	}
	//�����ݒ�
	mTabDataArray.GetObject(inTabIndex).SetViewHeight(inViewIndex-1,aboveHeight);
	mTabDataArray.GetObject(inTabIndex).SetViewHeight(inViewIndex,height);
	//text view�z�u
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(inTabIndex,layout);
	UpdateLayout_TextView(inTabIndex,layout.pt_MainColumn,layout.w_MainColumn,layout.h_MainColumn,
						  layout.w_MainTextView,layout.w_LineNumber,layout.w_MainTextMargin,layout.pt_VScrollbarArea);
	if( inCompleted == true )
	{
		//�`��X�V
		NVI_RefreshWindow();
	}
	//���ۂ�delta�v�Z
	return mTabDataArray.GetObjectConst(inTabIndex).GetViewHeight(inViewIndex-1) - origHeight;
}

//#92
/**
TextView�\���G���A�S�̂̍������擾�iseparator�܂ށj
*/
ANumber	AWindow_Text::SPI_GetTextViewAreaHeight() const
{
	AItemCount	viewcount = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewCount();
	ANumber	result = kSeparatorHeight*(viewcount-1);
	for( AIndex i = 0; i < viewcount; i++ )
	{
		result += mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewHeight(i);
	}
	return result;
}

/*#199
//�e�L�X�g�\���X�V�iADocument_Text����w�������j
void	AWindow_Text::SPI_UpdateText( const AObjectID inTextDocumentID, 
		const AIndex inStartLineIndex, const AIndex inEndLineIndex, const AItemCount inInsertedLineCount, 
		const ABool inParagraphCountChanged )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inTextDocumentID);
	if( tabIndex != kObjectID_Invalid )
	{
		for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(tabIndex).GetViewCount(); viewIndex++ )
		{
			//�e�L�X�g�r���[�X�V
			GetTextViewByControlID(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(viewIndex)).
					SPI_UpdateText(inStartLineIndex,inEndLineIndex,inInsertedLineCount);
			//�s�ԍ��r���[�X�V
			GetLineNumberViewByControlID(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(viewIndex)).
					SPI_UpdateText(inStartLineIndex,inEndLineIndex,inInsertedLineCount,inParagraphCountChanged);
		}
	}
}
*/

//�t�H���g����Text�`��v���p�e�B���ύX���ꂽ�ꍇ�̏���
void	AWindow_Text::SPI_UpdateTextDrawProperty( const AObjectID inTextDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inTextDocumentID);
	if( tabIndex != /*#216 kObjectID_Invalid*/kIndex_Invalid )
	{
		UpdateTextDrawProperty(tabIndex);
	}
}

void	AWindow_Text::SPI_UpdateTextDrawPropertyAll( const AModeIndex inModeIndex )
{
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			UpdateTextDrawProperty(tabIndex);
		}
	}
}

void	AWindow_Text::UpdateTextDrawProperty( const AIndex inTabIndex )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(inTabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	textViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(textViewControlID).SPI_UpdateTextDrawProperty();
		GetTextViewByControlID(textViewControlID).NVI_Refresh();
		if( textViewControlID == focus )
		{
			GetTextViewByControlID(textViewControlID).SPI_AdjustScroll_Center();
		}
		AControlID	lineNumberViewControlID = mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(viewIndex);
		GetLineNumberViewByControlID(lineNumberViewControlID).SPI_UpdateTextDrawProperty();
		GetLineNumberViewByControlID(lineNumberViewControlID).NVI_Refresh();
	}
	if( mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonModeIndex() != 
				GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex() )
	/*#725 ���T�C�Y�����AWrap���ύX���ɂ́A�c�[���o�[�č쐬�͕s�v�Ǝv����A���A�č쐬��̍Ĕz�u���K�v�Ȃ̂ŁA�R�����g�A�E�g
	���������ōč쐬����悤�ɖ߂��Ȃ�A�Ĕz�u���K�v�B���邢��SPI_RemakeToolButtonsAll()�̂ق����ǂ���������Ȃ��B
	{
		//�c�[���o�[�č쐬
		CreateToolButtons(inTabIndex);
	}
	*/
	//#725 InfoHeader�p�~ SetInfoHeaderTextDefault();
	UpdateRulerProperty(inTabIndex);
	//#342
	AControlID	subTextPane = mTabDataArray.GetObject(inTabIndex).GetSubTextControlID();
	if( subTextPane != kControlID_Invalid )
	{
		GetTextViewByControlID(subTextPane).SPI_UpdateTextDrawProperty();
		GetTextViewByControlID(subTextPane).NVI_Refresh();
	}
	AControlID	subLineNumber = mTabDataArray.GetObject(inTabIndex).GetSubTextLineNumberControlID();
	if( subLineNumber != kControlID_Invalid )
	{
		GetLineNumberViewByControlID(subLineNumber).SPI_UpdateTextDrawProperty();
		GetLineNumberViewByControlID(subLineNumber).NVI_Refresh();
	}
	//�c�[�����j���[�X�V
	GetApp().SPI_UpdateToolMenu(GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex());
}

//
void	AWindow_Text::SPI_UpdateViewImageSize( const AObjectID inTextDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inTextDocumentID);
	if( tabIndex != /*#216 kObjectID_Invalid*/kIndex_Invalid )
	{
		for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(tabIndex).GetViewCount(); viewIndex++ )
		{
			GetTextViewByControlID(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(viewIndex)).SPI_UpdateImageSize();
			GetLineNumberViewByControlID(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(viewIndex)).SPI_UpdateImageSize();
			GetRulerView(tabIndex).SPI_UpdateImageSize();
		}
		//#212
		AControlID	subTextPane = mTabDataArray.GetObject(tabIndex).GetSubTextControlID();
		if( subTextPane != kControlID_Invalid )
		{
			GetTextViewByControlID(subTextPane).SPI_UpdateImageSize();
		}
		AControlID	subLineNumber = mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID();
		if( subLineNumber != kControlID_Invalid )
		{
			GetLineNumberViewByControlID(subLineNumber).SPI_UpdateImageSize();
		}
	}
}

//#450
/**
*/
void	AWindow_Text::SPI_UpdateLineImageInfo( const AModeIndex inModeIndex )
{
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex() == inModeIndex 
					|| inModeIndex == kIndex_Invalid )
		{
			for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
			{
				GetTextViewByControlID(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(v)).SPI_InitLineImageInfo();
				AControlID	subpaneControlID = mTabDataArray.GetObject(tabIndex).GetSubTextControlID();
				if( subpaneControlID != kControlID_Invalid )
				{
					GetTextViewByControlID(subpaneControlID).SPI_InitLineImageInfo();
				}
			}
		}
	}
	NVI_RefreshWindow();
}

/*#199
//win 080727
//�w��Document��TextView���ĕ`�悷��
void	AWindow_Text::SPI_RefreshTextViews( const AObjectID inTextDocumentID ) const
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inTextDocumentID);
	if( tabIndex != kObjectID_Invalid )
	{
		for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); viewIndex++ )
		{
			GetTextViewConstByControlID(mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex)).NVI_Refresh();
		}
	}
}
*/

//#199
/**
TextViewID�擾

@param inTabIndex TabIndex
@param inViewIndex View��Index
*/
/*
AViewID	AWindow_Text::GetTextViewID( const AIndex inTabIndex, const AIndex inViewIndex )
{
	return mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(inViewIndex);
}
*/

/**
�w��documentID, spos, epos���E�C���h�E���ɕ\������i�h�L�������g�̓E�C���h�E���ɐ����ς݂̂��Ɓj
*/
void	AWindow_Text::SPI_RevealTextViewByDocumentID( const ADocumentID inDocumentID, const AIndex inSpos, const AIndex inEpos,
		const ABool inInAnotherView, const AAdjustScrollType inAdjustScrollType )
{
	//�E�C���h�E�I��
	NVI_SelectWindow();
	//�w��h�L�������g��tabIndex�擾
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	AControlID	textViewControlID = kControlID_Invalid;
	//���݂̃t�H�[�J�X�̃e�L�X�g�r���[�擾
	AControlID	currentFocus = GetTopMostFocusTextViewControlID();
	if( SPI_IsSubTextView(currentFocus) == true )
	{
		//------------------���݂̃t�H�[�J�X���T�u�e�L�X�g�J�����̏ꍇ------------------
		if( inInAnotherView == true )
		{
			//�ʃr���[�\�������C���J�����ɕ\������
			
			//�^�u��I��
			NVI_SelectTab(tabIndex);
			//control ID�擾
			textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(0);
		}
		else
		{
			//���݃r���[�\�����T�u�e�L�X�g�J�����ɕ\������
			
			//�T�u�e�L�X�g�ɕ\��
			SPI_DisplayInSubText(tabIndex,false,kIndex_Invalid,true,false);
			//control ID�擾
			textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID();
		}
	}
	else
	{
		//------------------���݂̃t�H�[�J�X�����C���e�L�X�g�r���[�̏ꍇ------------------
		if( inInAnotherView == true )
		{
			//�ʃr���[�\�����T�u�e�L�X�g�J�����ɕ\������
			
			//�T�u�e�L�X�g�J�����\��
			ShowHideSubTextColumn(true);
			//�T�u�e�L�X�g�ɕ\��
			SPI_DisplayInSubText(tabIndex,false,kIndex_Invalid,true,false);
			//control ID�擾
			textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID();
		}
		else
		{
			//���݃r���[�\�������C���e�L�X�g�r���[�ɕ\������
			
			if( NVI_GetCurrentTabIndex() == tabIndex )
			{
				//�^�u�������ꍇ�́A�����r���[�̃t�H�[�J�X���ړ������A���݂̃t�H�[�J�X��ɕ\������
				textViewControlID = currentFocus;
			}
			else
			{
				//�^�u��I��
				NVI_SelectTab(tabIndex);
				//control ID�擾
				textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(0);
			}
		}
	}
	if( textViewControlID != kControlID_Invalid )
	{
		//�t�H�[�J�X
		NVI_SwitchFocusTo(textViewControlID);
		//�w��spos,epos��I��
		if( inSpos != kIndex_Invalid && inEpos != kIndex_Invalid )
		{
			GetTextViewByControlID(textViewControlID).SPI_SetSelect(inSpos,inEpos,true);
		}
		//�X�N���[������
		GetTextViewByControlID(textViewControlID).SPI_AdjustScroll(inAdjustScrollType);
	}
}

//#844
/**
�L�����b�g�\���X�V
*/
void	AWindow_Text::SPI_RefreshCaret()
{
	//�e���C���e�L�X�g�r���[�̃L�����b�g�\���X�V
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(tabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(textViewControlID).SPI_RefreshCaret();
	}
	//�T�u�e�L�X�g�r���[�̃L�����b�g�\���X�V
	if( mSubTextCurrentDocumentID != kObjectID_Invalid )
	{
		AIndex	subTextTabIndex = NVI_GetTabIndexByDocumentID(mSubTextCurrentDocumentID);
		AControlID	textViewControlID = mTabDataArray.GetObjectConst(subTextTabIndex).GetSubTextControlID();
		GetTextViewByControlID(textViewControlID).SPI_RefreshCaret();
	}
}

/**
�E�C���h�E����ԉ��̃r���[���ǂ������擾
*/
ABool	AWindow_Text::SPI_GetIsBottomTextView( const AControlID inControlID ) const
{
	if( NVI_ExistView(inControlID) == false )   return false;//#212 View�����ۂɑ��݂��Ă��邩�ǂ����̃`�F�b�N
	if( inControlID == kControlID_Invalid )   return false;//#212
	
	//�^�u���Ƃ̃��[�v
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		//���C���J�������̍Ō�̃r���[�Ɣ�r
		AItemCount	viewCount = mTabDataArray.GetObjectConst(tabIndex).GetViewCount();
		if( mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewCount-1) == inControlID )
		{
			return true;
		}
		//�T�u�y�C����TextView�Ɣ�r
		if( mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID() == inControlID )
		{
			return true;
		}
	}
	return false;
	
}

#pragma mark ===========================

#pragma mark ---�T�u�E�C���h�E
//#725

/**
�T�u�E�C���h�E�ʒu�ړ�
*/
void	AWindow_Text::SPI_MoveOverlaySubWindow( const ASubWindowLocationType inSrcType, const AIndex inSrcIndex,
		const ASubWindowLocationType inDstType, const AIndex inDstIndex )
{
	ASubWindowType	type = kSubWindowType_None;
	AFloatNumber	height = 0;
	AWindowID	winID = kObjectID_Invalid;
	//�ړ����f�[�^�擾���A�ړ�������폜
	switch(inSrcType)
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			//�ړ����f�[�^�擾
			type = mLeftSideBarArray_Type.Get(inSrcIndex);
			height = mLeftSideBarArray_Height.Get(inSrcIndex);
			winID = mLeftSideBarArray_OverlayWindowID.Get(inSrcIndex);
			//�ړ����f�[�^�폜
			mLeftSideBarArray_Type.Delete1(inSrcIndex);
			mLeftSideBarArray_Height.Delete1(inSrcIndex);
			mLeftSideBarArray_OverlayWindowID.Delete1(inSrcIndex);
			mLeftSideBarArray_Dragging.Delete1(inSrcIndex);
			mLeftSideBarArray_Collapsed.Delete1(inSrcIndex);
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			//�ړ����f�[�^�擾
			type = mRightSideBarArray_Type.Get(inSrcIndex);
			height = mRightSideBarArray_Height.Get(inSrcIndex);
			winID = mRightSideBarArray_OverlayWindowID.Get(inSrcIndex);
			//�ړ����f�[�^�폜
			mRightSideBarArray_Type.Delete1(inSrcIndex);
			mRightSideBarArray_Height.Delete1(inSrcIndex);
			mRightSideBarArray_OverlayWindowID.Delete1(inSrcIndex);
			mRightSideBarArray_Dragging.Delete1(inSrcIndex);
			mRightSideBarArray_Collapsed.Delete1(inSrcIndex);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//�ړ���Ƀf�[�^�ǉ�
	switch(inDstType)
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			mLeftSideBarArray_Type.Insert1(inDstIndex,type);
			mLeftSideBarArray_Height.Insert1(inDstIndex,height);
			mLeftSideBarArray_OverlayWindowID.Insert1(inDstIndex,winID);
			mLeftSideBarArray_Dragging.Insert1(inDstIndex,false);
			mLeftSideBarArray_Collapsed.Insert1(inDstIndex,false);
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			mRightSideBarArray_Type.Insert1(inDstIndex,type);
			mRightSideBarArray_Height.Insert1(inDstIndex,height);
			mRightSideBarArray_OverlayWindowID.Insert1(inDstIndex,winID);
			mRightSideBarArray_Dragging.Insert1(inDstIndex,false);
			mRightSideBarArray_Collapsed.Insert1(inDstIndex,false);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//view bounds�X�V
	SPI_UpdateViewBounds();
	//�T�C�h�o�[�f�[�^�ݒ�
	SetDataSideBar();
}

/**
�T�u�E�C���h�E�ǉ�
*/
void	AWindow_Text::SPI_AddOverlaySubWindow( const ASubWindowLocationType inLocationType, const AIndex inIndex,
		const ASubWindowType inSubWindowType )
{
	switch(inLocationType)
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			AWindowID	winID = GetApp().SPI_CreateSubWindow(inSubWindowType,0,GetObjectID(),
						kSubWindowLocationType_LeftSideBar,inIndex,false);
			if( winID != kObjectID_Invalid )
			{
				mLeftSideBarArray_Type.Insert1(inIndex,inSubWindowType);
				mLeftSideBarArray_Height.Insert1(inIndex,100);
				mLeftSideBarArray_OverlayWindowID.Insert1(inIndex,winID);
				mLeftSideBarArray_Dragging.Insert1(inIndex,false);
				mLeftSideBarArray_Collapsed.Insert1(inIndex,false);
			}
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			AWindowID	winID = GetApp().SPI_CreateSubWindow(inSubWindowType,0,GetObjectID(),
						kSubWindowLocationType_RightSideBar,inIndex,false);
			if( winID != kObjectID_Invalid )
			{
				mRightSideBarArray_Type.Insert1(inIndex,inSubWindowType);
				mRightSideBarArray_Height.Insert1(inIndex,100);
				mRightSideBarArray_OverlayWindowID.Insert1(inIndex,winID);
				mRightSideBarArray_Dragging.Insert1(inIndex,false);
				mRightSideBarArray_Collapsed.Insert1(inIndex,false);
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//view bounds�X�V
	SPI_UpdateViewBounds();
	//�T�C�h�o�[�f�[�^�ݒ�
	SetDataSideBar();
}

/**
�T�u�E�C���h�Edrag
*/
ABool	AWindow_Text::SPI_DragSubWindow( const AWindowID inSubWindowID, const AGlobalPoint inMouseGlobalPoint )
{
	//���C�A�E�g�擾
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	
	//===============�}�E�X�ʒu�擾===============
	//�E�C���h�E���̃}�E�X�ʒu�擾
	APoint	textWindowPosition = {0};
	NVI_GetWindowPosition(textWindowPosition);
	AWindowPoint	mouseWindowPoint = {0};
	mouseWindowPoint.x = inMouseGlobalPoint.x - textWindowPosition.x;
	mouseWindowPoint.y = inMouseGlobalPoint.y - textWindowPosition.y;
	
	//�E�T�C�h�o�[�����ǂ����̔���
	if( mouseWindowPoint.x >= layout.pt_RightSideBar.x && mouseWindowPoint.x <= layout.pt_RightSideBar.x + layout.w_RightSideBar &&
				mouseWindowPoint.y >= layout.pt_RightSideBar.y && mouseWindowPoint.y <= layout.pt_RightSideBar.y + layout.h_RightSideBar )
	{
		//===============�}�E�X���E�T�C�h�o�[���ɂ���ꍇ===============
		
		//�}�E�X�ʒu�ɑΉ�����T�C�h�o�[���ڂ�index����
		AIndex	currentIndex = mRightSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( currentIndex == kIndex_Invalid )
		{
			//------------------�E�T�C�h�o�[���ɖ��z�u�Ȃ�A�E�T�C�h�o�[���ɁA�idragging��ԂŁj�V�K�z�u����B------------------
			
			//�T�C�h�o�[���ڔz�u�f�[�^�v�Z
			AFloatNumber	subWindowHeight = GetApp().NVI_GetWindowByID(inSubWindowID).NVI_GetWindowHeight();
			AFloatNumber	sideBarHeight = layout.h_RightSideBar;
			AFloatNumber	height = subWindowHeight*sideBarHeight/(sideBarHeight-subWindowHeight);
			//�T�C�h�o�[�f�[�^�ǉ�
			currentIndex = mRightSideBarArray_Type.Add(GetApp().SPI_GetSubWindowType(inSubWindowID));
			mRightSideBarArray_Height.Add(height);
			mRightSideBarArray_OverlayWindowID.Add(inSubWindowID);
			mRightSideBarArray_Dragging.Add(true);
			mRightSideBarArray_Collapsed.Add(false);
			//�T�u�E�C���h�E�̃v���p�e�B���X�V
			GetApp().SPI_SetSubWindowProperty(inSubWindowID,kSubWindowLocationType_RightSideBar,currentIndex,GetObjectID());
			//�T�C�h�o�[���ڂ�ǉ������̂ŁA�e�T�C�h�o�[��index���X�V����B
			SPI_UpdateOverlayWindowLocationProperty();
			//view bounds�X�V
			SPI_UpdateViewBounds();
		}
		//�ǉ��������ڂ�dragging�t���O��ON
		mRightSideBarArray_Dragging.Set(currentIndex,true);
		//------------------�}�E�X�ʒu�ɉ����č��ڈړ�------------------
		//�}�E�X�ʒu�ɑΉ�����T�C�h�o�[���ڌ���
		ANumber	y = layout.pt_RightSideBar.y;
		for( AIndex i = 0; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//����y�ʒu�擾
			ANumber	nexty = y + mRightSideBarArray_Height.Get(i);
			//�T�C�h�o�[���ڌ���
			AIndex	dstIndex = kIndex_Invalid;
			if( i == currentIndex-1 && y + mRightSideBarArray_Height.Get(currentIndex) < nexty )
			{
				if( mouseWindowPoint.y >= y && mouseWindowPoint.y <= y + mRightSideBarArray_Height.Get(currentIndex) )
				{
					dstIndex = i;
				}
				else if( mouseWindowPoint.y >= y + mRightSideBarArray_Height.Get(currentIndex) && mouseWindowPoint.y <= nexty )
				{
					dstIndex = i+1;
				}
			}
			else
			{
				if( mouseWindowPoint.y >= y && mouseWindowPoint.y <= nexty )
				{
					dstIndex = i;
				}
			}
			//�}�E�X�ʒu�ɑΉ�����T�C�h�o�[���ڈʒu����������A���ڂ��ړ����ďI��
			if( dstIndex != kIndex_Invalid )
			{
				if( dstIndex < mRightSideBarArray_OverlayWindowID.GetItemCount() && dstIndex != currentIndex )
				{
					mRightSideBarArray_Type.Move(currentIndex,dstIndex);
					mRightSideBarArray_Height.Move(currentIndex,dstIndex);
					mRightSideBarArray_OverlayWindowID.Move(currentIndex,dstIndex);
					mRightSideBarArray_Dragging.Move(currentIndex,dstIndex);
					mRightSideBarArray_Collapsed.Move(currentIndex,dstIndex);
					//view bounds�X�V
					SPI_UpdateViewBounds();
					//�e�T�C�h�o�[��index���X�V����B
					SPI_UpdateOverlayWindowLocationProperty();
				}
				//�T�C�h�o�[�f�[�^�ݒ�
				SetDataSideBar();
				return true;
			}
			//����y��
			y = nexty;
		}
	}
	
	//���T�C�h�o�[�����ǂ����̔���
	if( mouseWindowPoint.x >= layout.pt_LeftSideBar.x && mouseWindowPoint.x <= layout.pt_LeftSideBar.x + layout.w_LeftSideBar &&
				mouseWindowPoint.y >= layout.pt_LeftSideBar.y && mouseWindowPoint.y <= layout.pt_LeftSideBar.y + layout.h_LeftSideBar )
	{
		//===============�}�E�X�����T�C�h�o�[���ɂ���ꍇ===============
		
		//�}�E�X�ʒu�ɑΉ�����T�C�h�o�[���ڂ�index����
		AIndex	currentIndex = mLeftSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( currentIndex == kIndex_Invalid )
		{
			//------------------���T�C�h�o�[���ɖ��z�u�Ȃ�A���T�C�h�o�[���ɁA�idragging��ԂŁj�V�K�z�u����B------------------
			
			//�T�C�h�o�[���ڔz�u�f�[�^�v�Z
			AFloatNumber	subWindowHeight = GetApp().NVI_GetWindowByID(inSubWindowID).NVI_GetWindowHeight();
			AFloatNumber	sideBarHeight = layout.h_LeftSideBar;
			AFloatNumber	height = subWindowHeight*sideBarHeight/(sideBarHeight-subWindowHeight);
			//�T�C�h�o�[�f�[�^�ǉ�
			currentIndex = mLeftSideBarArray_Type.Add(GetApp().SPI_GetSubWindowType(inSubWindowID));
			mLeftSideBarArray_Height.Add(height);
			mLeftSideBarArray_OverlayWindowID.Add(inSubWindowID);
			mLeftSideBarArray_Dragging.Add(true);
			mLeftSideBarArray_Collapsed.Add(false);
			//�T�u�E�C���h�E�̃v���p�e�B���X�V
			GetApp().SPI_SetSubWindowProperty(inSubWindowID,kSubWindowLocationType_LeftSideBar,currentIndex,GetObjectID());
			//�T�C�h�o�[���ڂ�ǉ������̂ŁA�e�T�C�h�o�[��index���X�V����B
			SPI_UpdateOverlayWindowLocationProperty();
			//view bounds�X�V
			SPI_UpdateViewBounds();
		}
		//�ǉ��������ڂ�dragging�t���O��ON
		mLeftSideBarArray_Dragging.Set(currentIndex,true);
		//------------------�}�E�X�ʒu�ɉ����č��ڈړ�------------------
		//�}�E�X�ʒu�ɑΉ�����T�C�h�o�[���ڌ���
		ANumber	y = layout.pt_LeftSideBar.y;
		for( AIndex i = 0; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//����y�ʒu�擾
			ANumber	nexty = y + mLeftSideBarArray_Height.Get(i);
			//�T�C�h�o�[���ڌ���
			AIndex	dstIndex = kIndex_Invalid;
			if( i == currentIndex-1 && y + mLeftSideBarArray_Height.Get(currentIndex) < nexty )
			{
				if( mouseWindowPoint.y >= y && mouseWindowPoint.y <= y + mLeftSideBarArray_Height.Get(currentIndex) )
				{
					dstIndex = i;
				}
				else if( mouseWindowPoint.y >= y + mLeftSideBarArray_Height.Get(currentIndex) && mouseWindowPoint.y <= nexty )
				{
					dstIndex = i+1;
				}
			}
			else
			{
				if( mouseWindowPoint.y >= y && mouseWindowPoint.y <= nexty )
				{
					dstIndex = i;
				}
			}
			//�}�E�X�ʒu�ɑΉ�����T�C�h�o�[���ڈʒu����������A���ڂ��ړ����ďI��
			if( dstIndex != kIndex_Invalid )
			{
				if( dstIndex < mLeftSideBarArray_OverlayWindowID.GetItemCount() && dstIndex != currentIndex )
				{
					mLeftSideBarArray_Type.Move(currentIndex,dstIndex);
					mLeftSideBarArray_Height.Move(currentIndex,dstIndex);
					mLeftSideBarArray_OverlayWindowID.Move(currentIndex,dstIndex);
					mLeftSideBarArray_Dragging.Move(currentIndex,dstIndex);
					mLeftSideBarArray_Collapsed.Move(currentIndex,dstIndex);
					//view bounds�X�V
					SPI_UpdateViewBounds();
					//�e�T�C�h�o�[��index���X�V����B
					SPI_UpdateOverlayWindowLocationProperty();
				}
				//�T�C�h�o�[�f�[�^�ݒ�
				SetDataSideBar();
				return true;
			}
			//����y��
			y = nexty;
		}
	}
	
	//===============�}�E�X���E�T�C�h�o�[���A���T�C�h�o�[���ǂ���ɂ������ꍇ===============
	
	//�T�C�h�o�[���ɂ���΁A�t���[�e�B���O�ɕύX����B
	{
		//�E�T�C�h�o�[���ڂ��猟��
		AIndex	currentIndex = mRightSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( currentIndex != kIndex_Invalid )
		{
			//�E�T�C�h�o�[�f�[�^����폜
			mRightSideBarArray_Type.Delete1(currentIndex);
			mRightSideBarArray_Height.Delete1(currentIndex);
			mRightSideBarArray_OverlayWindowID.Delete1(currentIndex);
			mRightSideBarArray_Dragging.Delete1(currentIndex);
			mRightSideBarArray_Collapsed.Delete1(currentIndex);
			//�e�T�C�h�o�[��index���X�V����B
			SPI_UpdateOverlayWindowLocationProperty();
			//view bounds�X�V
			SPI_UpdateViewBounds();
			//�T�u�E�C���h�E�̃v���p�e�B���X�V
			GetApp().SPI_SetSubWindowProperty(inSubWindowID,kSubWindowLocationType_Floating,kIndex_Invalid,GetObjectID());
		}
		
	}
	{
		//���T�C�h�o�[���ڂ��猟��
		AIndex	currentIndex = mLeftSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( currentIndex != kIndex_Invalid )
		{
			//���T�C�h�o�[�f�[�^����폜
			mLeftSideBarArray_Type.Delete1(currentIndex);
			mLeftSideBarArray_Height.Delete1(currentIndex);
			mLeftSideBarArray_OverlayWindowID.Delete1(currentIndex);
			mLeftSideBarArray_Dragging.Delete1(currentIndex);
			mLeftSideBarArray_Collapsed.Delete1(currentIndex);
			//�e�T�C�h�o�[��index���X�V����B
			SPI_UpdateOverlayWindowLocationProperty();
			//view bounds�X�V
			SPI_UpdateViewBounds();
			//�T�u�E�C���h�E�̃v���p�e�B���X�V
			GetApp().SPI_SetSubWindowProperty(inSubWindowID,kSubWindowLocationType_Floating,kIndex_Invalid,GetObjectID());
		}
		
	}
	//�T�C�h�o�[�f�[�^�ݒ�
	SetDataSideBar();
	return false;
}

/**
�T�u�E�C���h�E�h���b�O�I��
*/
void	AWindow_Text::SPI_EndDragSubWindow( const AWindowID inSubWindowID )
{
	//dragging�t���O����
	for( AIndex i = 0; i < mRightSideBarArray_Dragging.GetItemCount(); i++ )
	{
		mRightSideBarArray_Dragging.Set(i,false);
	}
	for( AIndex i = 0; i < mLeftSideBarArray_Dragging.GetItemCount(); i++ )
	{
		mLeftSideBarArray_Dragging.Set(i,false);
	}
	//�W�����v���X�g�̏ꍇ�̏����i�t���[�e�B���O�W�����v���X�g�j
	if( GetApp().SPI_GetSubWindowType(inSubWindowID) == kSubWindowType_JumpList )
	{
		ASubWindowLocationType	locationType = GetApp().SPI_GetSubWindowLocationType(inSubWindowID);
		if( locationType == kSubWindowLocationType_Floating )
		{
			//�t���[�e�B���O�W�����v���X�g���P�݂̂ɂ��邽�߁A�]���̃W�����v���X�g�͍폜����B
			if( inSubWindowID != mFloatingJumpListWindowID && mFloatingJumpListWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_DeleteSubWindow(mFloatingJumpListWindowID);
			}
			mFloatingJumpListWindowID = inSubWindowID;
			//�t���[�e�B���O�W�����v���X�g�\���E��\���̏�Ԃ�app pref db�ɕۑ�
			GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kShowFloatingJumpList,true);
		}
		else if( locationType == kSubWindowLocationType_LeftSideBar ||
					locationType == kSubWindowLocationType_RightSideBar )
		{
			//����T�C�h�o�[�ɂȂ����̂��A�]���̃t���[�e�B���O�E�C���h�E�ł���΁A
			//mFloatingJumpListWindowID�̒l���N���A����B
			if( inSubWindowID == mFloatingJumpListWindowID )
			{
				mFloatingJumpListWindowID = kObjectID_Invalid;
			}
			//�t���[�e�B���O�W�����v���X�g�\���E��\���̏�Ԃ�app pref db�ɕۑ�
			GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kShowFloatingJumpList,false);
		}
	}
	//view bounds�X�V
	SPI_UpdateViewBounds();
	//�T�C�h�o�[�f�[�^�ݒ�
	SetDataSideBar();
}

/**
�T�C�h�o�[�f�[�^�X�V�iindex�����ɕ��ׂ�j
*/
void	AWindow_Text::SPI_UpdateOverlayWindowLocationProperty()
{
	for( AIndex i = 0; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
	{
		//
		AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(i);
		GetApp().SPI_SetSubWindowProperty(winID,kSubWindowLocationType_RightSideBar,i,GetObjectID());
	}
	for( AIndex i = 0; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
	{
		//
		AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(i);
		GetApp().SPI_SetSubWindowProperty(winID,kSubWindowLocationType_LeftSideBar,i,GetObjectID());
	}
}

/**
�T�C�h�o�[�̐܂肽����
*/
void	AWindow_Text::SPI_ExpandCollapseSubWindow( const AWindowID inSubWindowID )
{
	//�E�T�C�h�o�[
	{
		//����
		AIndex	index = mRightSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( index != kIndex_Invalid )
		{
			//==================�܂肽���ݏ�ԕύX==================
			ABool	collapse = !(mRightSideBarArray_Collapsed.Get(index));
			mRightSideBarArray_Collapsed.Set(index,collapse);
			//==================�Ώۍ��ڂ̃T�C�Y�ݒ�==================
			if( collapse == true )
			{
				//�܂肽���ݎ��̃T�C�Y�ɐݒ�
				GetApp().NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(AApp::kSubWindowMinWidth,
							GetApp().SPI_GetSubWindowCollapsedHeight());
			}
			else
			{
				//�܂肽���݉���
				//�ŏ��T�C�Y�擾
				AFloatNumber	minHeight = GetApp().SPI_GetSubWindowMinHeight(inSubWindowID);
				GetApp().NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(AApp::kSubWindowMinWidth,minHeight);
				//�ŏ��T�C�Y�����Ȃ�ŏ��T�C�Y�ݒ�
				if( mRightSideBarArray_Height.Get(index) < minHeight )
				{
					mRightSideBarArray_Height.Set(index,minHeight);
				}
			}
			//==================�e���ڂ̍����␳==================
			//�Ώۍ��ڂ���ō����␳���s���B
			AIndex	adjustStartIndex = index+1;
			//�Ώۍ��ڂ��オ�S�Đ܂肽���ݍ��ڂȂ�A�S�Ă̍��ڂō����␳����B
			ABool	adjustable = false;
			for( AIndex i = adjustStartIndex; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
			{
				if( mRightSideBarArray_Collapsed.Get(i) == false )
				{
					adjustable = true;
				}
			}
			if( adjustable == false )
			{
				adjustStartIndex = 0;
			}
			//�����␳���s
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetCurrentTabIndex(),layout);
			UpdateLayout_AdjustRightSideBarHeight(layout.h_RightSideBar,adjustStartIndex);
		}
		//view bounds�X�V
		SPI_UpdateViewBounds();
	}
	//���T�C�h�o�[
	{
		//����
		AIndex	index = mLeftSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( index != kIndex_Invalid )
		{
			//==================�܂肽���ݏ�ԕύX==================
			ABool	collapse = !(mLeftSideBarArray_Collapsed.Get(index));
			mLeftSideBarArray_Collapsed.Set(index,collapse);
			//==================�Ώۍ��ڂ̃T�C�Y�ݒ�==================
			if( collapse == true )
			{
				//�܂肽���ݎ��̃T�C�Y�ɐݒ�
				GetApp().NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(AApp::kSubWindowMinWidth,
							GetApp().SPI_GetSubWindowCollapsedHeight());
			}
			else
			{
				//�܂肽���݉���
				//�ŏ��T�C�Y�擾
				AFloatNumber	minHeight = GetApp().SPI_GetSubWindowMinHeight(inSubWindowID);
				GetApp().NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(AApp::kSubWindowMinWidth,minHeight);
				//�ŏ��T�C�Y�����Ȃ�ŏ��T�C�Y�ݒ�
				if( mLeftSideBarArray_Height.Get(index) < minHeight )
				{
					mLeftSideBarArray_Height.Set(index,minHeight);
				}
			}
			//==================�e���ڂ̍����␳==================
			//�Ώۍ��ڂ���ō����␳���s���B
			AIndex	adjustStartIndex = index+1;
			//�Ώۍ��ڂ��オ�S�Đ܂肽���ݍ��ڂȂ�A�S�Ă̍��ڂō����␳����B
			ABool	adjustable = false;
			for( AIndex i = adjustStartIndex; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
			{
				if( mLeftSideBarArray_Collapsed.Get(i) == false )
				{
					adjustable = true;
				}
			}
			if( adjustable == false )
			{
				adjustStartIndex = 0;
			}
			//�����␳���s
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetCurrentTabIndex(),layout);
			UpdateLayout_AdjustLeftSideBarHeight(layout.h_LeftSideBar,adjustStartIndex);
		}
		//view bounds�X�V
		SPI_UpdateViewBounds();
	}
	//�T�C�h�o�[�f�[�^�ݒ�
	SetDataSideBar();
}

/**
�T�u�E�C���h�E���܂肽���܂�Ă��邩�ǂ������擾
*/
ABool	AWindow_Text::SPI_IsCollapsedSubWindow( const AWindowID inSubWindowID )
{
	{
		AIndex	index = mRightSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( index != kIndex_Invalid )
		{
			return mRightSideBarArray_Collapsed.Get(index);
		}
	}
	{
		AIndex	index = mLeftSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( index != kIndex_Invalid )
		{
			return mLeftSideBarArray_Collapsed.Get(index);
		}
	}
	return false;
}

/**
�T�C�h�o�[�̕\���X�V
*/
void	AWindow_Text::RefreshSideBar()
{
	if( mRightSideBarDisplayed == true )
	{
		for( AIndex i = 0; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(i);
			GetApp().NVI_GetWindowByID(winID).NVI_RefreshWindow();
		}
	}
	if( mLeftSideBarDisplayed == true )
	{
		for( AIndex i = 0; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(i);
			GetApp().NVI_GetWindowByID(winID).NVI_RefreshWindow();
		}
	}
}

//#798
/**
�T�C�h�o�[���T�u�E�C���h�E�N���[�Y������
@note AApp���ŃN���[�Y���ꂽ�Ƃ��̓�������
*/
void	AWindow_Text::SPI_DoSubWindowClosed( const AWindowID inSubWindowID )
{
	switch(GetApp().SPI_GetSubWindowLocationType(inSubWindowID))
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			AIndex	index = mLeftSideBarArray_OverlayWindowID.Find(inSubWindowID);
			//
			mLeftSideBarArray_Type.Delete1(index);
			mLeftSideBarArray_Height.Delete1(index);
			mLeftSideBarArray_OverlayWindowID.Delete1(index);
			mLeftSideBarArray_Dragging.Delete1(index);
			mLeftSideBarArray_Collapsed.Delete1(index);
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			AIndex	index = mRightSideBarArray_OverlayWindowID.Find(inSubWindowID);
			//
			mRightSideBarArray_Type.Delete1(index);
			mRightSideBarArray_Height.Delete1(index);
			mRightSideBarArray_OverlayWindowID.Delete1(index);
			mRightSideBarArray_Dragging.Delete1(index);
			mRightSideBarArray_Collapsed.Delete1(index);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//view bounds�X�V
	SPI_UpdateViewBounds();
	//�T�C�h�o�[�f�[�^�ݒ�
	SetDataSideBar();
}

//#798
/**
�u���o���������v
*/
void	AWindow_Text::SPI_SearchInHeaderList()
{
	//�P�D�t���[�e�B���O�E�C���h�E���\�����Ȃ�A�t���[�e�B���O�E�C���h�E���Ō���
	if( mFloatingJumpListWindowID != kObjectID_Invalid )
	{
		if( GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_IsWindowVisible() == true )
		{
			GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//�Q�D�\�����̃T�C�h�o�[���ɃE�C���h�E������΁A���̃E�C���h�E���Ō���
	if( mRightSideBarDisplayed == true )
	{
		for( AIndex i = 0; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(i);
			if( GetApp().SPI_GetSubWindowType(winID) == kSubWindowType_JumpList )
			{
				GetApp().SPI_GetJumpListWindow(winID).SPI_SwitchFocusToSearchBox();
				return;
			}
		}
	}
	if( mLeftSideBarDisplayed == true )
	{
		for( AIndex i = 0; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(i);
			if( GetApp().SPI_GetSubWindowType(winID) == kSubWindowType_JumpList )
			{
				GetApp().SPI_GetJumpListWindow(winID).SPI_SwitchFocusToSearchBox();
				return;
			}
		}
	}
	//�R�D���o���E�C���h�E���\���Ȃ�A�V�K�ɐ������āA���̃E�C���h�E���Ō���
	SPI_ShowHideJumpListWindow(true);
	if( mFloatingJumpListWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).SPI_SwitchFocusToSearchBox();
	}
}

//#725
/**
�T�C�h�o�[�f�[�^�ݒ�
*/
void	AWindow_Text::SetDataSideBar()
{
	if( mLeftSideBarCreated == true )
	{
		//DB���T�C�h�o�[�f�[�^�S�폜
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kLeftSideBarArray_Type);
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kLeftSideBarArray_Height);
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kLeftSideBarArray_Parameter);
		GetApp().NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kLeftSideBarArray_Collapsed);
		GetApp().NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kLeftSideBarArray_CurrentPath);
		//���݂̃T�C�h�o�[���ږ��̃��[�v
		AItemCount	itemCount = mLeftSideBarArray_Type.GetItemCount();
		if( itemCount > 0 )
		{
			for( AIndex index = 0; index < itemCount; index++ )
			{
				//�f�[�^�擾
				ASubWindowType	type = mLeftSideBarArray_Type.Get(index);
				ANumber	height = mLeftSideBarArray_Height.Get(index)*10000.0;
				ANumber	parameter = 0;
				ABool	collapsed = mLeftSideBarArray_Collapsed.Get(index);
				switch(type)
				{
				  case kSubWindowType_FileList:
					{
						parameter = GetApp().SPI_GetFileListWindow(mLeftSideBarArray_OverlayWindowID.Get(index)).SPI_GetCurrentMode();
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
				//���݃p�X���擾
				AText	currentPath;
				switch(type)
				{
				  case kSubWindowType_FileList:
					{
						GetApp().SPI_GetFileListWindow(mLeftSideBarArray_OverlayWindowID.Get(index)).SPI_GetCurrentPath(currentPath);
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
				//DB�ɐݒ�
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Type,type);
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Height,height);
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Parameter,parameter);
				GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kLeftSideBarArray_Collapsed,collapsed);
				GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kLeftSideBarArray_CurrentPath,currentPath);
			}
		}
		else
		{
			//���ڐ�0�̂Ƃ��́A�_�~�[���ڂ�ǉ����Ă����i���ڐ�0�ŋN�������Ƃ��A�N�����Ƀf�t�H���g�f�[�^�ݒ肷�邽�߁j
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Type,kSubWindowType_None);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Height,0);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Parameter,0);
			GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kLeftSideBarArray_Collapsed,false);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kLeftSideBarArray_CurrentPath,GetEmptyText());
		}
	}
	if( mRightSideBarCreated == true )
	{
		//DB���T�C�h�o�[�f�[�^�S�폜
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kRightSideBarArray_Type);
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kRightSideBarArray_Height);
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kRightSideBarArray_Parameter);
		GetApp().NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kRightSideBarArray_Collapsed);
		GetApp().NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kRightSideBarArray_CurrentPath);
		//���݂̃T�C�h�o�[���ږ��̃��[�v
		AItemCount	itemCount = mRightSideBarArray_Type.GetItemCount();
		if( itemCount > 0 )
		{
			for( AIndex index = 0; index < itemCount; index++ )
			{
				//�f�[�^�擾
				ASubWindowType	type = mRightSideBarArray_Type.Get(index);
				ANumber	height = mRightSideBarArray_Height.Get(index)*10000.0;
				ANumber	parameter = 0;
				ABool	collapsed = mRightSideBarArray_Collapsed.Get(index);
				switch(type)
				{
				  case kSubWindowType_FileList:
					{
						parameter = GetApp().SPI_GetFileListWindow(mRightSideBarArray_OverlayWindowID.Get(index)).SPI_GetCurrentMode();
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
				//���݃p�X���擾
				AText	currentPath;
				switch(type)
				{
				  case kSubWindowType_FileList:
					{
						GetApp().SPI_GetFileListWindow(mRightSideBarArray_OverlayWindowID.Get(index)).SPI_GetCurrentPath(currentPath);
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
				//DB�ɐݒ�
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Type,type);
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Height,height);
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Parameter,parameter);
				GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kRightSideBarArray_Collapsed,collapsed);
				GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kRightSideBarArray_CurrentPath,currentPath);
			}
		}
		else
		{
			//���ڐ�0�̂Ƃ��́A�_�~�[���ڂ�ǉ����Ă����i���ڐ�0�ŋN�������Ƃ��A�N�����Ƀf�t�H���g�f�[�^�ݒ肷�邽�߁j
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Type,kSubWindowType_None);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Height,0);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Parameter,0);
			GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kRightSideBarArray_Collapsed,false);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kRightSideBarArray_CurrentPath,GetEmptyText());
		}
	}
}

#pragma mark ===========================

#pragma mark ---�T�u�e�L�X�g�J����

/**
�T�u�e�L�X�g�J�����\���E��\���؂�ւ��A����сA����ɂ��T�u�e�L�X�g�\���Ediff�\�����[�h�ؑ�
*/
void	AWindow_Text::ShowHideSubTextColumn( const ABool inShow )
{
	//�T�u�e�L�X�g�J�����\���E��\��
	ShowHideSubTextColumnCore(inShow);
	
	if( inShow == true )
	{
		//Diff�\�����[�h���ǂ����ɂ�镪��
		if( mDiffDisplayMode == false )
		{
			//���݂̃^�u�̓��e���T�u�e�L�X�g�ɕ\������
			SPI_DisplayTabInSubText(true);
		}
		else
		{
			//Diff���T�u�e�L�X�g�ɕ\������
			SPI_UpdateDiffDisplay(NVI_GetCurrentDocumentID());
		}
	}
	else
	{
		//Diff�\�����[�h����
		SPI_SetDiffDisplayMode(false);
	}
}

/**
�T�u�e�L�X�g�J�����\���E��\���؂�ւ�
*/
void	AWindow_Text::ShowHideSubTextColumnCore( const ABool inShow )
{
	//�\���E��\���ɕύX���Ȃ���Ή������Ȃ�
	if( inShow == mSubTextColumnDisplayed )
	{
		return;
	}
	//�T�u�e�L�X�g�J�����\����Ԑݒ�
	mSubTextColumnDisplayed = inShow;
	
	//bounds�擾
	ARect	bounds = {0};
	NVI_GetWindowBounds(bounds);
	if( inShow == true )
	{
		// ===================== �\�� =====================
		
		if( NVI_IsFullScreen() == false )
		{
			//�T�u�e�L�X�g���Abounds�g��
			bounds.left -= mSubTextColumnWidth;
			
			//�E�C���h�E�T�C�Y�ύX
			UpdateViewBindings(kTextWindowViewBindings_ShowSubTextColumn);
			mDoingSetWindowBoundsAnimate = true;
			NVI_SetWindowBounds(bounds,false);
			mDoingSetWindowBoundsAnimate = false;
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
	}
	else
	{
		// ===================== ��\�� =====================
		
		if( NVI_IsFullScreen() == false )
		{
			//�T�u�e�L�X�g���Abounds�k�߂�
			bounds.left += mSubTextColumnWidth;
			
			//�E�C���h�E�T�C�Y�ύX
			UpdateViewBindings(kTextWindowViewBindings_HideSubTextColumn);
			mDoingSetWindowBoundsAnimate = true;
			NVI_SetWindowBounds(bounds,false);
			mDoingSetWindowBoundsAnimate = false;
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
	}
	//view bounds�X�V
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		UpdateViewBounds(tabIndex);
	}
	//�T�u�e�L�X�g�J�����\����Ԃ�DB�ɋL��
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kSubTextColumnDisplayed,mSubTextColumnDisplayed);
}

/**
�T�u�e�L�X�g�J�������ύX
*/
ANumber	AWindow_Text::ChangeSubTextPaneColumnWidth( const ANumber inDelta, const ABool inCompleted )
{
	//���X�̕����擾 #725
	ANumber	origWidth = mSubTextColumnWidth;
	
	//���ύX
	mSubTextColumnWidth += inDelta;
	
	//�T�u�y�C�����␳ #725
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	//�ŏ��l�␳
	if( mSubTextColumnWidth < kMinWidth_SubTextColumn )
	{
		mSubTextColumnWidth = kMinWidth_SubTextColumn;
	}
	//�ő�l�␳
	if( layout.w_MainColumn - inDelta < kMinWidth_MainColumn )
	{
		mSubTextColumnWidth = layout.w_MainColumn + layout.w_SubTextPaneColumn - kMinWidth_MainColumn;
	}
	
	//�ύX����������
	if( inCompleted == true )
	{
		for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
		{
			//�e�^�u��document�̌��݂̃��C��text view�����擾
			ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
			//View�̈ʒu�A�T�C�Y�X�V
			UpdateViewBounds(tabIndex);
			{
				//�s���Čv�Z�i�E�C���h�E���ς��̂Łj
				//Diff�Ώۃh�L�������g�͍s���Čv�Z���Ȃ��i�����A�T�u�y�C���̕��͌��܂��Ă��邪�A�T�u�y�C�����ɏ]��Ȃ��ݒ�̎��ɁA���̔���͏�����肪����j
				if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_IsDiffTargetDocument() == false )//#379
				{
					//�s�܂�Ԃ�����̏ꍇ�ɍČv�Z
					if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetWrapMode() != kWrapMode_NoWrap )
					{
						GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_DoWrapRelatedDataChanged(false);
					}
				}
			}
			//
			UpdateTextDrawProperty(tabIndex);
		}
		//�`��X�V
		NVI_RefreshWindow();
	}
	//�ύX������
	else
	{
		UpdateViewBounds(NVI_GetCurrentTabIndex(),kUpdateViewBoundsTriggerType_SubTextColumnVSeparator);
		//NVI_RefreshWindow();
		//NVI_UpdateWindow();
		/*
		AIndex	tabIndex = NVI_GetCurrentTabIndex();
		ATextWindowLayoutData	layout = {0};
		CalculateLayout(tabIndex,layout);
		UpdateLayout_TextView(tabIndex,layout.pt_MainColumn,layout.w_MainColumn,layout.h_MainColumn,
					layout.w_MainTextView,layout.w_LineNumber,layout.pt_VScrollbarArea);
		UpdateLayout_SubTextColumn(tabIndex,layout);
		*/
	}
	//�T�u�e�L�X�g����DB�ɋL��
	GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kSubTextColumnWidth,mSubTextColumnWidth);
	//���ۂ�delta�v�Z #725
	return mSubTextColumnWidth - origWidth;
}

#pragma mark ===========================

#pragma mark ---�T�u�e�L�X�g

/**
�w��controlID���T�u�e�L�X�g���ǂ������擾
*/
ABool	AWindow_Text::SPI_IsSubTextView( const AControlID inTextViewControlID ) const
{
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID() == inTextViewControlID )
		{
			return true;
		}
	}
	return false;
}

/**
���݂̃T�u�e�L�X�g�ɕ\������Ă���text view��control ID���擾
*/
AControlID	AWindow_Text::SPI_GetCurrentSubTextControlID() const
{
	ADocumentID	docID = SPI_GetCurrentSubTextPaneDocumentID();
	if( docID == kObjectID_Invalid )   return kControlID_Invalid;
	
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(docID);
	return mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID();
}

#pragma mark ===========================

#pragma mark ---���T�C�h�o�[
//#212

/**
�T�u�y�C���\���E��\���؂�ւ�
*/
void	AWindow_Text::ShowHideLeftSideBar( const ABool inShow )
{
	if( inShow == true )
	{
		//���T�C�h�o�[�������Ȃ琶��
		if( mLeftSideBarCreated == false )
		{
			CreateLeftSideBar();
			mLeftSideBarCreated = true;
		}
		
		// ===================== �\�� =====================
		
		if( mLeftSideBarDisplayed == true )   return;
		
		//#0 Pane�T�|�[�g�`�F�b�N
		if( GetApp().SPI_IsSupportPaneMode() == false )   return;
		
		//�T�u�y�C����\������
		
		//SubPane�\����Ԑݒ�
		ABool	oldLeftSideBarDisplayed = mLeftSideBarDisplayed;
		mLeftSideBarDisplayed = true;
		
		if( NVI_IsFullScreen() == false )
		{
			//#341
			ARect	newBounds;
			GetChangedWindowBoundsForSubPane(oldLeftSideBarDisplayed,newBounds);
			
			//#725
			//�T�C�h�o�[�A�j���[�V�����pbindings�ݒ�
			UpdateViewBindings(kTextWindowViewBindings_ShowHideLeftSidebar);
			
			//�T�C�h�o�[�w�i�\��
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetCurrentTabIndex(),layout);
			NVI_SetControlPosition(kControlID_LeftSideBarBackground,layout.pt_LeftSideBar);
			NVI_SetControlSize(kControlID_LeftSideBarBackground,layout.w_LeftSideBar,layout.h_LeftSideBarWithMargin);
			NVI_SetShowControl(kControlID_LeftSideBarBackground,true);
			
			{
				//window bounds�X�V�i�A�j���[�V�����j
				AStSuppressTextWindowUpdateViewBounds	s(GetObjectID());
				//
				mDoingSetWindowBoundsAnimate = true;
				NVI_SetWindowBounds(newBounds,true);//#341 #688
				mDoingSetWindowBoundsAnimate = false;
			}
			
			//#725
			//bindings��ʏ�ɖ߂�
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
		
		/* #725 �T�u�e�L�X�g�y�C���͏�ɐ�������悤�ɕύX�B
		//�S�^�u��SubTextPane�𐶐�
		for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
		{
			//���C��TextView�̃X�N���[���ʒu���擾
			AImagePoint	originPoint;
			AControlID	mainTextViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(0);
			GetTextViewByControlID(mainTextViewControlID).NVI_GetOriginOfLocalFrame(originPoint);
			//SubTextPane����
			//#699 ATextPoint	spt = {0,0}, ept = {0,0};
			CreateSubTextInEachTab(tabIndex,originPoint);
			//View�̈ʒu�A�T�C�Y�ݒ�
			UpdateViewBounds(tabIndex,newBounds,kUpdateViewBoundsTriggerType_ShowLeftSideBar);//#341 #645
		}
		*/
		
		//�E�C���h�E�T�C�Y����
		//#341 AdjustWindowWidthForSubPane(oldLeftSideBarDisplayed);
		//#688 NVI_SetWindowBounds(newBounds);//#341
		
		//View�̈ʒu�A�T�C�Y�ݒ�
		for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
		{
			UpdateViewBounds(tabIndex,kUpdateViewBoundsTriggerType_ShowLeftSideBar);//#645
		}
		
		/*
		//�\��
		//�T�u�y�C���\���X�V
		AIndex	diffTabIndex = GetDiffDocumentTabIndexFromWorkingTabIndex(NVI_GetCurrentTabIndex());//#379
		if( mDiffDisplayMode == true && diffTabIndex != kIndex_Invalid )//#379
		{
			//#379 �T�u�y�C����Diff�\��
			SPI_UpdateDiffDisplay(NVI_GetCurrentDocumentID());
		}
		else
		{
			if( mSubPaneMode != kSubPaneMode_Manual )
			{
				UpdateSubPaneAutomatic(mSubPaneMode,true);
			}
			else
			{
				SPI_DisplayTabInSubText(true);
			}
		}
		*/
	}
	else
	{
		// ===================== ��\�� =====================
		
		if( mLeftSideBarDisplayed == false )   return;
		
		//�T�u�y�C�����\���ɂ���
		
		/*
		//#379 �T�u�y�C����\���ɂ�����Diff���[�h������
		SPI_SetDiffDisplayMode(false);
		*/
		
		//SubPane�\����Ԑݒ�
		ABool	oldLeftSideBarDisplayed = mLeftSideBarDisplayed;
		mLeftSideBarDisplayed = false;
		
		if( NVI_IsFullScreen() == false )
		{
			//#725
			//��ɃT�u�E�C���h�E��Hide����B
			//�A�j���[�V�������{���ɁA�T�u�E�C���h�E��\�����Ȃ��悤�ɂ��邽�߁B
			UpdateVisibleSubWindows();
			
			//#341
			ARect	newBounds;
			GetChangedWindowBoundsForSubPane(oldLeftSideBarDisplayed,newBounds);
			
			/* #725 �T�u�e�L�X�g�y�C���͏�ɐ�������悤�ɕύX�B
		//�S�^�u��SubTextPane���폜
		for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
		{
			DeleteSubTextInEachTab(tabIndex);
			//View�̈ʒu�A�T�C�Y�ݒ�
			UpdateViewBounds(tabIndex,kUpdateViewBoundsTriggerType_HideLeftSideBar);//#645
		}
		*/
			
			//#725
			//�T�C�h�o�[�A�j���[�V�����pbindings�ݒ�
			UpdateViewBindings(kTextWindowViewBindings_ShowHideLeftSidebar);
			
			{
				//window bounds�X�V�i�A�j���[�V�����j
				AStSuppressTextWindowUpdateViewBounds	s(GetObjectID());
				//
				mDoingSetWindowBoundsAnimate = true;
				NVI_SetWindowBounds(newBounds,true);//#341 #688
				mDoingSetWindowBoundsAnimate = false;
			}
			
			//#725
			//bindings��ʏ�ɖ߂�
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
		
		//View�̈ʒu�A�T�C�Y�ݒ� #725
		for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
		{
			UpdateViewBounds(tabIndex,kUpdateViewBoundsTriggerType_HideLeftSideBar);
		}
		
	}
	
	//�ݒ�ۑ�
	//#850 /*#667 NVIDO_Hide()��AAppPrefDB::kCreateTabInsteadOfCreateWindow�ۑ��ƈꏏ�ɕۑ�����悤�ɕύX
	//#569 �ۑ����ׂ������f
	ABool	shouldSave = true;
	/*#850 
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true &&
				SPI_IsTabModeMainWindow() == false )
	{
		//�^�u�E�C���h�E���[�h�̏ꍇ�A���C���E�C���h�E�ȊO�ł͕ۑ����Ȃ�
		shouldSave = false;
	}
	*/
	if( mLuaConsoleMode == true )
	{
		//Lua Console�ł͕ۑ����Ȃ�
		shouldSave = false;
	}
	//�ۑ�
	if( shouldSave == true )
	{
		GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kLeftSideBarDisplayed,mLeftSideBarDisplayed);
	}
	//#602
	UpdateVSeparatorHelpTag();
	
	//doc info�E�C���h�E�X�V #1053
	SPI_GetCurrentTabTextDocument().SPI_UpdateDocInfoWindows();
}

/**
�w��Tab���ɃT�u�e�L�X�g�y�C������
*/
void	AWindow_Text::CreateSubTextInEachTab( const AIndex inTabIndex, 
		/*#699 const ATextPoint& inCaretTextPoint, const ATextPoint& inSelectTextPoint, */
		const AImagePoint& inOriginOfLocalFrame )
{
	//�����ς݂Ȃ炷���Ƀ��^�[��
	if( mTabDataArray.GetObject(inTabIndex).GetSubTextControlID() != kControlID_Invalid )   return;
	
	//�i�������T�C�Y�A�ʒu�́ANVICB_Update()�Őݒ肳���B�j
	AWindowPoint	pt = {0,0};
	//CrossCaret���[�h�擾
    //#844 ABool	crosscaretmode = false;
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentConstByID(docID).SPI_GetModeIndex();
    //#844 crosscaretmode = GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kDefaultDisplayCrossCaret);
	//TextView�����i����0�ɐݒ肷��BAView_Text::SPI_DoWrapCalculated()�ɂĐ�����adjust scroll���邽�߁j
	AControlID	textViewControlID = NVM_AssignDynamicControlID();
	ATextViewFactory	textViewFactory(GetObjectID(),textViewControlID,
						/*#695NVI_GetDocumentIDByTabIndex(inTabIndex)*/
						GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)),//#695
						GetObjectID(),//#379
						/*#699 ComparePoint(inCaretTextPoint,inSelectTextPoint),*///#699inCaretTextPoint,inSelectTextPoint,
                        inOriginOfLocalFrame,false);//#844 ,crosscaretmode);
	AViewID	textViewID = NVM_CreateView(textViewControlID,pt,0,10,kControlID_Invalid,kControlID_Invalid,true,textViewFactory);
	GetTextViewByControlID(textViewControlID).SPI_SetTextWindowID(GetObjectID());
	NVI_SetControlBindings(textViewControlID,false,true,true,true,true,false);
	//LineNumberView����
	AImagePoint	originOfLocalFrame = inOriginOfLocalFrame;
	originOfLocalFrame.x = 0;
	AControlID	lineNumberControlID = NVM_AssignDynamicControlID();
	ALineNumberViewFactory	lineNumberViewFactory(GetObjectID(),lineNumberControlID,textViewID,NVI_GetDocumentIDByTabIndex(inTabIndex),originOfLocalFrame);
	AViewID	lineNumberViewID = NVM_CreateView(lineNumberControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,lineNumberViewFactory);
	GetTextViewByControlID(textViewControlID).SPI_SetLineNumberView(lineNumberViewID);//#450
	NVI_SetControlBindings(lineNumberControlID,false,true,true,true,true,false);
	
	//#725
	//�T�u�e�L�X�gH�X�N���[���o�[����
	AControlID	hScrollbarControlID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(hScrollbarControlID,pt,20,10);
	
	//Tab��View�̃R���g���[��ID��o�^
	NVM_RegisterViewInTab(inTabIndex,textViewControlID);
	NVM_RegisterViewInTab(inTabIndex,lineNumberControlID);
	NVM_RegisterViewInTab(inTabIndex,hScrollbarControlID);//#725
	
	//��{�I��Hide���
	NVI_SetShowControl(textViewControlID,false);
	NVI_SetShowControl(lineNumberControlID,false);
	NVI_SetShowControl(hScrollbarControlID,false);//#725
	//SelectTab()�Ŏ����\������Ȃ��悤�ɂ���i�\�������TabActivated���ɁA�T�u�y�C�����[�h�ɏ]���ĕ\������j
	NVI_SetLatentVisible(inTabIndex,textViewControlID,false);
	NVI_SetLatentVisible(inTabIndex,lineNumberControlID,false);
	NVI_SetLatentVisible(inTabIndex,hScrollbarControlID,false);//#725
	
	//TabData�ɐݒ�
	mTabDataArray.GetObject(inTabIndex).SetSubTextControlID(textViewControlID,lineNumberControlID,hScrollbarControlID);
	
	//TextView��Image�T�C�Y�ݒ�
	GetTextViewByControlID(textViewControlID).SPI_UpdateImageSize();
	
	//TextView�ɁA�Ή�����X�N���[���o�[�̃R���g���[��ID�ݒ�
	GetTextViewByControlID(textViewControlID).NVI_SetScrollBarControlID(hScrollbarControlID,kControlID_Invalid);
	
}

/**
����������
*/
void	AWindow_Text::DeleteSubTextInEachTab( const AIndex inTabIndex )
{
	//�������Ȃ炷���Ƀ��^�[��
	if( mTabDataArray.GetObject(inTabIndex).GetSubTextControlID() == kControlID_Invalid )   return;
	
	//mSubTextCurrentDocumentID�̐ݒ����
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	if( docID == mSubTextCurrentDocumentID )
	{
		mSubTextCurrentDocumentID = kObjectID_Invalid;
	}
	
	//View�폜
	NVI_DeleteControl(mTabDataArray.GetObject(inTabIndex).GetSubTextControlID());//win NVI_DeleteView()��mTabDataArray_ControlIDs����̍폜�����s��Ȃ��̂�NG
	NVI_DeleteControl(mTabDataArray.GetObject(inTabIndex).GetSubTextLineNumberControlID());//win NVI_DeleteView()��mTabDataArray_ControlIDs����̍폜�����s��Ȃ��̂�NG
	NVI_DeleteControl(mTabDataArray.GetObject(inTabIndex).GetSubTextHScrollbarControlID());//#725
	
	//TabData�ɐݒ�
	mTabDataArray.GetObject(inTabIndex).SetSubTextControlID(kControlID_Invalid,kControlID_Invalid,kControlID_Invalid);
}

/**
SubPane�\����Ԃɉ����ăE�C���h�E�T�C�Y�ύX��̃E�C���h�Ebounds�擾 #341
*/
void	AWindow_Text::GetChangedWindowBoundsForSubPane( const ABool inOldSubTextPaneDisplayed, ARect& outBounds )//#341
{
	NVI_GetWindowBounds(outBounds);//#341
	if( inOldSubTextPaneDisplayed == false )
	{
		//SubTextPane�\�����łȂ���΃E�C���h�E�����ɍL����
		if( mLeftSideBarDisplayed == true )
		{
			/*#341
			ARect	bounds;
			NVI_GetWindowBounds(bounds);
			*/
			outBounds.left -= mLeftSideBarWidth;
			//�E�C���h�E�̍��[��������悤�ɒ���
			AGlobalRect	availableBounds;
			//#688 CWindowImp::GetAvailableWindowPositioningBounds(availableBounds);
			NVI_GetAvailableWindowBoundsInSameScreen(availableBounds);//#688
			if( outBounds.left < availableBounds.left )
			{
				ANumber	delta = availableBounds.left - outBounds.left;
				outBounds.left += delta;
				outBounds.right += delta;
			}
			//�ݒ�
			//#341 NVI_SetWindowBounds(bounds);
		}
	}
	else
	{
		//SubTextPane�\�����Ȃ�E�C���h�E�����΂߂�
		if( mLeftSideBarDisplayed == false )
		{
			/*#341
			ARect	bounds;
			NVI_GetWindowBounds(bounds);
			*/
			outBounds.left += mLeftSideBarWidth;
			//�O�̂���
			if( outBounds.right-outBounds.left < 100 )   outBounds.right = outBounds.left + 100;
			//�ݒ�
			//#341 NVI_SetWindowBounds(bounds);
		}
	}
}

/**
�T�u�y�C�����ړ�
*/
ANumber	AWindow_Text::ChangeLeftSideBarWidth( const ANumber inDelta, const ABool inCompleted )//#409
{
	//�E�C���h�E�T�C�Y�擾
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	//���C�A�E�g�f�[�^�擾
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	
	//���X�̕����L�� #725
	ANumber	origWidth = mLeftSideBarWidth;
	
	//�T�u�y�C�����ύX
	mLeftSideBarWidth += inDelta;
	
	//�T�u�y�C�����␳
	if( mLeftSideBarWidth < kMinWidth_SideBar )
	{
		mLeftSideBarWidth = kMinWidth_SideBar;
	}
	if( mLeftSideBarWidth > layout.w_MainColumn + layout.w_LeftSideBar - kMinWidth_MainColumn )
	{
		mLeftSideBarWidth = layout.w_MainColumn + layout.w_LeftSideBar - kMinWidth_MainColumn;
	}
	//
	if( inCompleted == false )//#409
	{
		UpdateViewBounds(NVI_GetCurrentTabIndex());
		NVI_UpdateWindow();
	}
	else
	{
		//==================�T�C�Y�ύX������==================
		
		//�T�C�Y���O��̊������ƕς���Ă���ꍇ�̂݁A�s���Čv�Z���̏������s���i�Ԉ���ăN���b�N�����i�h���b�O�Ȃ��j�����Ƃ��ɏ������Ȃ��悤�ɂ��邽�߁j
		if( mLeftSideBarWidth != mPrevLeftSideBarWidth )
		{
			//
			mPrevLeftSideBarWidth = mLeftSideBarWidth;
			
			//�S�^�u��View�̈ʒu�A�T�C�Y�ݒ�
			for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
			{
				UpdateViewBounds(tabIndex);
				//�s���Čv�Z�i�L���r���[���ς��̂Łj #324
				GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_DoWrapRelatedDataChanged(false);
			}
			//#324
			NVI_RefreshWindow();
			
			//DB�ɕۑ�
			GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kLeftSideBarWidth,mLeftSideBarWidth);
		}
	}
	//���ۂ�delta�v�Z #725
	return mLeftSideBarWidth - origWidth;
}

/**
���T�C�h�o�[���ڍ����ύX
*/
ANumber	AWindow_Text::ChangeLeftSideBarHeight( const AIndex inItemIndex, const ANumber inDelta )
{
	//���X�̍����L��
	AFloatNumber	origHeight = mLeftSideBarArray_Height.Get(inItemIndex);
	if( inDelta < 0 )
	{
		//==================����������������==================
		
		//�w��T�C�h�o�[���ڂ̍ŏ������A���݂̍������擾
		AWindowID	winID0 = mLeftSideBarArray_OverlayWindowID.Get(inItemIndex);
		AFloatNumber	minHeight0 = GetApp().SPI_GetSubWindowMinHeight(winID0);
		AFloatNumber	height0 = mLeftSideBarArray_Height.Get(inItemIndex);
		//�w��T�C�h�o�[���ڂ̎��ȍ~��collapsed�ł͂Ȃ����ڂ������A�������擾
		AFloatNumber	height1 = 0;
		AIndex	changeIndex = kIndex_Invalid;
		AItemCount	itemCount = mLeftSideBarArray_Height.GetItemCount();
		for( changeIndex = inItemIndex+1; changeIndex < itemCount; changeIndex++ )
		{
			if( mLeftSideBarArray_Collapsed.Get(changeIndex) == false )
			{
				height1 = mLeftSideBarArray_Height.Get(changeIndex);
				break;
			}
		}
		if( height1 != 0 )
		{
			//�����ύX�i�w�荀�ڂ̍��������������A���i�ȍ~�̂����ŏ���collapsed�łȂ����́j�̍��ڂ̍��������̕��傫������j
			if( height0 + inDelta >= minHeight0 )
			{
				//�ύX��A�ŏ��������͑傫���ꍇ
				height0 += inDelta;
				height1 -= inDelta;
			}
			else
			{
				//�ύX��A�ŏ������ɂȂ�ꍇ
				height1 += height0 - minHeight0;
				height0 = minHeight0;
			}
			//�����ݒ�
			mLeftSideBarArray_Height.Set(inItemIndex,height0);
			mLeftSideBarArray_Height.Set(changeIndex,height1);
		}
	}
	else
	{
		//==================������傫������==================
		
		//�ύX���ׂ��T�C�Y�̎c���remainDelta�Ɋi�[����B
		//���ȍ~�̍��ڂ̃T�C�Y���o������菬�������Ă����āAinDelta�̒l���m�ۂł���܂ő�����
		ANumber	remainDelta = inDelta;
		//���ȍ~�̍��ڂŃ��[�v
		AItemCount	itemCount = mLeftSideBarArray_Height.GetItemCount();
		for( AIndex i = inItemIndex+1; i < itemCount; i++ )
		{
			//collapsed�ȍ��ڂ͏������ł��Ȃ��̂ŁA�������Ȃ�
			if( mLeftSideBarArray_Collapsed.Get(i) == true )
			{
				continue;
			}
			//���ڂ̍ŏ������A�������擾
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(i);
			AFloatNumber	minHeight = GetApp().SPI_GetSubWindowMinHeight(winID);
			AFloatNumber	height = mLeftSideBarArray_Height.Get(i);
			//�o������菬�������āA�������������̒l��remainDelta�������
			if( height - remainDelta >= minHeight )
			{
				height = height - remainDelta;
				remainDelta = 0;
			}
			else
			{
				remainDelta -= height-minHeight;
				height = minHeight;
			}
			//�����X�V
			mLeftSideBarArray_Height.Set(i,height);
			//remainDelta��0�ɂȂ�����I��
			if( remainDelta <= 0 )
			{
				break;
			}
		}
		//���ȍ~�̍��ڂ��������ł����������A�w�荀�ڂ�傫������
		AFloatNumber	height = mLeftSideBarArray_Height.Get(inItemIndex);
		height += inDelta-remainDelta;
		mLeftSideBarArray_Height.Set(inItemIndex,height);
	}
	
	//�T�C�h�o�[�Ĕz�u
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	UpdateLayout_LeftSideBar(NVI_GetCurrentTabIndex(),layout);
	UpdateViewBounds(NVI_GetCurrentTabIndex());
	//�T�C�h�o�[�f�[�^�ݒ�
	SetDataSideBar();
	//���ۂ�delta�v�Z
	return mLeftSideBarArray_Height.Get(inItemIndex) - origHeight;
}

/**
�T�u�y�C���Ŏw��tab�̃e�L�X�g��\������
*/
void	AWindow_Text::SPI_DisplayInSubText( const AIndex inTabIndex, 
		const ABool inScroll, const AIndex inScrollToParagraphIndex, //#450
		const ABool inSwitchFocus, const ABool inDiffDisplay )//#379
{
	//#379 �T�u�y�C�����\���Ȃ牽���������^�[��
	if( mTabDataArray.GetObjectConst(inTabIndex).GetSubTextControlID() == kControlID_Invalid )   return;
	
	//#1090
	//�T�u�e�L�X�g�J�����\�����̏ꍇ�̂݁A�\���������s���B
	if( mSubTextColumnDisplayed == false )   return;
	
	//#379 �h�L�������g��ViewActivated��ʒm
	GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_DoViewActivating();
	//			GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetSubTextControlID()).
	//			GetObjectID());
	
	//Diff�\���ȊO�Ȃ�ADiff�\�����[�h����������
	if( inDiffDisplay == false && mDiffDisplayMode == true )
	{
		SPI_SetDiffDisplayMode(false);
	}
	//
	ABool	currentFocusIsInSubText = SPI_IsSubTextView(NVI_GetCurrentFocus());
	//
	mSubTextCurrentDocumentID = kObjectID_Invalid;
	//�w��^�u��SubPane������\������i���݂̕\��Tab�𖳎����ĕ\���E��\������j
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		AControlID	textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID();
		AControlID	lineNumberControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextLineNumberControlID();
		AControlID	hScrollbarControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextHScrollbarControlID();//#725
		if( textViewControlID != kControlID_Invalid )
		{
			if( inTabIndex == tabIndex )
			{
				NVI_SetShowControl(textViewControlID,true);
				NVI_SetShowControl(lineNumberControlID,true);
				//#457 NVI_SetShowControl(hScrollbarControlID,true);//#725
				//H�X�N���[���o�[��minmax�ݒ�
				GetTextViewByControlID(textViewControlID).NVI_UpdateScrollBar();
				//H�X�N���[���o�[�̌��݈ʒu�ݒ�
				AImagePoint	originOfLocalFrame = {0};
				GetTextViewByControlID(textViewControlID).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
				//#558 NVI_SetControlNumber(hScrollbarControlID,originOfLocalFrame.x);
				GetTextViewByControlID(textViewControlID).NVI_UpdateScrollBar();//#558 �X�N���[���o�[�l�X�V��AView�ōs���i�c�����l���j
				//�X�N���[��
				if( inScroll == true )
				{
					GetTextViewByControlID(textViewControlID).SPI_ScrollToParagraph(inScrollToParagraphIndex,0);//#450 NVI_ScrollTo(inOriginPoint);
				}
				//�T�u�y�C���t�@�C�����{�^���X�V
				mSubTextCurrentDocumentID = NVI_GetDocumentIDByTabIndex(tabIndex);
				UpdateSubTextFileNameButton();
				//�T�u�e�L�X�g�ɍŌ�ɕ\������Ă����i��r�ȊO�́jdocument���L������i��r�������ɂ����\�����邽�߁j
				if( GetApp().SPI_GetTextDocumentByID(mSubTextCurrentDocumentID).SPI_IsDiffTargetDocument() == false )
				{
					mLastNormalSubTextDocumentID = mSubTextCurrentDocumentID;
				}
				//Focus�ړ�
				if( inSwitchFocus == true || currentFocusIsInSubText == true )
				{
					NVI_SwitchFocusTo(textViewControlID);
				}
				//�\������^�C�~���O�ɂȂ�Ȃ���ViewBounds�ݒ�ł��Ȃ��̂ŁA������UpdateViewBounds()���Ȃ��ƁA
				//Open��Tab��Select���Ă��Ȃ������Ƃ��ɃT�u�y�C���\���ł��Ȃ�
				UpdateViewBounds(tabIndex);
			}
			else
			{
				NVI_SetShowControl(textViewControlID,false);
				NVI_SetShowControl(lineNumberControlID,false);
				//#457 NVI_SetShowControl(hScrollbarControlID,false);//#725
			}
		}
	}
	//
	AControlID	subTextControlID = SPI_GetCurrentSubTextControlID();
	SPI_UpdateLineNumberButton(subTextControlID);
	UpdatePositionButton(subTextControlID);
}

/**
�T�u�y�C���Ō��݂�tab�̃e�L�X�g��\������i���C��text view�Ɠ����X�N���[���ʒu��\���j
*/
void	AWindow_Text::SPI_DisplayTabInSubText( const ABool inAdjustScroll )
{
	SPI_DisplayTabInSubText(NVI_GetCurrentTabIndex(),inAdjustScroll);
	/*#450 �������ʉ�
	//#450 AImagePoint	originPoint = kImagePoint_00;
	//�^�u���̍ŏI�t�H�[�J�XTextView���擾
	AControlID	mainTextViewControlID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
	//�����ŏI�t�H�[�J�X�����ݒ�A�܂��́ASubPaneText�Ȃ�A���C���y�C�����̍ŏ��̕����r���[�ɂ���
	if( mainTextViewControlID == kControlID_Invalid ||
				mainTextViewControlID == mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetSubTextControlID() )
	{
		mainTextViewControlID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0);
	}
	//���C��TextView�̃X�N���[���ʒu���擾
	AIndex	originParagraph = kIndex_Invalid;//#450 
	if( IsTextViewControlID(mainTextViewControlID) == true )
	{
		originParagraph = GetTextViewByControlID(mainTextViewControlID).SPI_GetOriginParagraph();//#450 NVI_GetOriginOfLocalFrame(originPoint);
	}
	//�T�u�y�C���ɓ����X�N���[���ʒu�ŕ\��
	SPI_DisplayInSubText(NVI_GetCurrentTabIndex(),inAdjustScroll,originParagraph);//#450 originPoint);
	*/
}

//#333
/**
�T�u�y�C���Ɏw��tab�̃e�L�X�g��\������i���C��text view�Ɠ����X�N���[���ʒu��\���j
*/
void	AWindow_Text::SPI_DisplayTabInSubText( const AIndex inTabIndex, const ABool inAdjustScroll )
{
	//#450 AImagePoint	originPoint = kImagePoint_00;
	//�^�u���̃��C���e�L�X�g�̍ŏI�t�H�[�J�XTextView���擾
	AControlID	mainTextViewControlID = mTabDataArray.GetObjectConst(inTabIndex).GetLatentMainTextViewControlID();
	//�����ŏI�t�H�[�J�X�����ݒ�Ȃ�A���C���y�C�����̍ŏ��̕����r���[�ɂ���
	if( IsTextViewControlID(mainTextViewControlID) == false )
	{
		mainTextViewControlID = mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0);
	}
	//���C��TextView�̃X�N���[���ʒu���擾
	AIndex	originParagraph = kIndex_Invalid;//#450 
	if( IsTextViewControlID(mainTextViewControlID) == true )
	{
		originParagraph = GetTextViewByControlID(mainTextViewControlID).SPI_GetOriginParagraph();//#450 NVI_GetOriginOfLocalFrame(originPoint);
	}
	//�T�u�y�C���ɓ����X�N���[���ʒu�ŕ\��
	SPI_DisplayInSubText(inTabIndex,inAdjustScroll,originParagraph,false,false);//#450 originPoint);
}

/**
�T�u�y�C�����[�h�{�^���e�L�X�g�X�V
*/
/*#899
void	AWindow_Text::SetSubPaneMode( const ASubPaneMode inSubPaneMode )
{
	//���[�h�X�V
	mSubPaneMode = inSubPaneMode;
	*#379
	//�{�^���X�V
	AText	text;
	switch(mSubPaneMode)
	{
	  case kSubPaneMode_PreviousDocument:
		{
			text.SetLocalizedText("SubPaneMode_PreviousDocument");
			break;
		}
	  case kSubPaneMode_SameDocument:
		{
			text.SetLocalizedText("SubPaneMode_SameDocument");
			break;
		}
	  case kSubPaneMode_Manual:
		{
			text.SetLocalizedText("SubPaneMode_Manual");
			break;
		}
	}
	NVI_GetButtonViewByControlID(kSubPaneModeButtonControlID).NVI_SetText(text);
	*
	//DB�X�V
	GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kSubPaneMode,static_cast<ANumber>(mSubPaneMode));
}
*/
/**
�T�u�y�C���\���X�V�i�w��T�u�y�C�����[�h�ɏ]���Ċe�^�u�̃T�u�y�C���̕\���^��\�����s���j
*/
/*#899
void	AWindow_Text::UpdateSubPaneAutomatic( const ASubPaneMode inSubPaneMode, const ABool inAdjustScroll )
{
	switch(inSubPaneMode)
	{
	  case kSubPaneMode_PreviousDocument:
		{
			if( NVI_GetSelectableTabCount() > 1 )
			{
				AIndex	previousTabIndex = NVI_GetTabIndexByZOrderIndex(1);
				if( previousTabIndex != kIndex_Invalid )
				{
					//���C��TextView�̃X�N���[���ʒu���擾
					AImagePoint	originPoint = kImagePoint_00;
					AControlID	mainTextViewControlID = mTabDataArray.GetObjectConst(previousTabIndex).GetTextViewControlID(0);
					//#450 GetTextViewByControlID(mainTextViewControlID).NVI_GetOriginOfLocalFrame(originPoint);
					AIndex	paragraphIndex = GetTextViewByControlID(mainTextViewControlID).SPI_GetOriginParagraph();
					//�T�u�y�C���ɓ����X�N���[���ʒu�ŕ\��
					SPI_DisplayInSubText(previousTabIndex,inAdjustScroll,paragraphIndex,false,false);//#450 originPoint);
				}
			}
			break;
		}
	  case kSubPaneMode_SameDocument:
		{
			SPI_DisplayTabInSubText(inAdjustScroll);
			break;
		}
	}
}
*/

//#725
/**
���T�C�h�o�[�����i�N�������ɃR�[�������BDB�f�[�^�Ɋ�Â��Đ����j
*/
void	AWindow_Text::CreateLeftSideBar()
{
	if( GetApp().SPI_IsSupportPaneMode() == false )   return;
	
	//���T�C�h�o�[�T�u�E�C���h�E���擾
	AItemCount	itemCount = GetApp().NVI_GetAppPrefDB().GetItemCount_Array(AAppPrefDB::kLeftSideBarArray_Type);
	for( AIndex index = 0; index < itemCount; index++ )
	{
		CreateLeftSideBarItem(index);
	}
}

//#725
/**
���T�C�h�o�[���ڐ����i�N�������ɃR�[�������BDB�f�[�^�Ɋ�Â��Đ����j
*/
void	AWindow_Text::CreateLeftSideBarItem( const AIndex inIndex )
{
	//�T�u�E�C���h�E�^�C�v�E�����擾�A�����o�ϐ��ւ̐ݒ�
	ASubWindowType	type = (ASubWindowType)(GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kLeftSideBarArray_Type,inIndex));
	AFloatNumber	height = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kLeftSideBarArray_Height,inIndex);
	ANumber	parameter = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kLeftSideBarArray_Parameter,inIndex);
	ABool	collapsed =  GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kLeftSideBarArray_Collapsed,inIndex);
	if( type != kSubWindowType_None )
	{
		//�T�u�E�C���h�E����
		AWindowID	winID = GetApp().SPI_CreateSubWindow(type,parameter,GetObjectID(),
					kSubWindowLocationType_LeftSideBar,inIndex,collapsed);
		if( winID != kObjectID_Invalid )
		{
			mLeftSideBarArray_Type.Add(type);
			mLeftSideBarArray_Height.Add(height/10000.0);
			mLeftSideBarArray_Dragging.Add(false);//#725
			mLeftSideBarArray_Collapsed.Add(collapsed);//#725
			mLeftSideBarArray_OverlayWindowID.Add(winID);
		}
		//���݃p�X��ݒ�
		AText	currentPath;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kLeftSideBarArray_CurrentPath,inIndex,currentPath);
		switch(type)
		{
		  case kSubWindowType_FileList:
			{
				GetApp().SPI_GetFileListWindow(winID).SPI_SetCurrentPath(currentPath);
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
}

//#725
/**
���T�C�h�o�[���S�T�u�E�C���h�E�폜
*/
void	AWindow_Text::DeleteLeftSideBar()
{
	for( AIndex index = 0; index < mLeftSideBarArray_Type.GetItemCount(); index++ )
	{
		AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(index);
		if( winID != kObjectID_Invalid )
		{
			GetApp().SPI_DeleteSubWindow(winID);
		}
	}
	//�f�[�^�폜
	mLeftSideBarArray_Type.DeleteAll();
	mLeftSideBarArray_Height.DeleteAll();
	mLeftSideBarArray_OverlayWindowID.DeleteAll();
	mLeftSideBarArray_Dragging.DeleteAll();//#725
	mLeftSideBarArray_Collapsed.DeleteAll();//#725
}

//#725
/**
���T�C�h�o�[���ڒǉ�
*/
void	AWindow_Text::AddNewSubWindowInLeftSideBar( const ASubWindowType inSubWindowType, const ANumber inParameter,
		const ANumber inHeight )
{
	//DB�ɍ��ڃf�[�^�ǉ�
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Type,inSubWindowType);
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Height,inHeight*10000);
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Parameter,inParameter);
	GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kLeftSideBarArray_Collapsed,false);
	GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kLeftSideBarArray_CurrentPath,GetEmptyText());
	//���T�C�h�o�[���ڐ���
	CreateLeftSideBarItem(GetApp().NVI_GetAppPrefDB().GetItemCount_NumberArray(AAppPrefDB::kLeftSideBarArray_Type)-1);
	//view bounds�X�V
	UpdateViewBounds(NVI_GetCurrentTabIndex());
	//�T�C�h�o�[�f�[�^�ݒ�
	SetDataSideBar();
}

//#725
/**
���T�C�h�o�[���ڂ�S�ăf�t�H���g�ɖ߂�
*/
void	AWindow_Text::SetDefaultSubWindowInLeftSideBar()
{
	//���T�C�h�o�[�f�[�^�폜
	DeleteLeftSideBar();
	//DB�f�[�^���f�t�H���g�ɖ߂�
	GetApp().GetAppPref().SetDefaultLeftSideBarArray();
	//���T�C�h�o�[����
	CreateLeftSideBar();
}

/*#725
//
void	AWindow_Text::DeleteSubPaneArray()
{
	for( AIndex i = 0; i < mLeftSideBarArray_Type.GetItemCount(); i++ )
	{
		GetApp().NVI_DeleteWindow(mLeftSideBarArray_HSeparatorWindowID.Get(i));
	}
	mLeftSideBarArray_Type.DeleteAll();
	mLeftSideBarArray_Height.DeleteAll();
	mLeftSideBarArray_HSeparatorWindowID.DeleteAll();
}
*/

/**
�T�u�y�C���t�@�C�����{�^���X�V
*/
void	AWindow_Text::UpdateSubTextFileNameButton()
{
	//�T�u�e�L�X�g�y�C���̃t�@�C�����{�^���̃��j���[��ݒ�
	ATextArray	titleArray, actionTextArray;
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		//#379 Diff�Ώۃh�L�������g�̓��j���[�ɒǉ����Ȃ��i�f�o�b�O���A�������R�����g�A�E�g�����Diff�Ώۃh�L�������g�����X�g�ɒǉ��ł���j
		if( NVI_GetTabSelectable(tabIndex) == false )   continue;
		//�^�C�g��
		AText	title;
		NVI_GetTitle(tabIndex,title);
		titleArray.Add(title);
		//DocID��ActionText�Ƃ��Đݒ�
		AText	actionText;
		actionText.AddNumber(tabIndex);
		actionTextArray.Add(actionText);
	}
	//���j���[�ݒ�
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetMenuTextArray(titleArray,actionTextArray,kMenuItemID_SubTextPaneMenu);
	
	//�T�u�e�L�X�g�t�@�C�����{�^���e�L�X�g�X�V
	UpdateSubTextFileNameButtonText();
}

/**
�T�u�e�L�X�g�t�@�C�����{�^���e�L�X�g�X�V
*/
void	AWindow_Text::UpdateSubTextFileNameButtonText()
{
	//�{�^���e�L�X�g�X�V
	AText	name;
	if( mSubTextCurrentDocumentID != kObjectID_Invalid )
	{
		GetApp().NVI_GetDocumentByID(mSubTextCurrentDocumentID).NVI_GetTitle(name);
		//�i����
		AText	paraText;
		paraText.SetLocalizedText("TitleText_Paragraph");
		paraText.ReplaceParamText('0',GetApp().SPI_GetTextDocumentByID(mSubTextCurrentDocumentID).SPI_GetParagraphCount());
		name.AddText(paraText);
	}
	//�e�L�X�g�ݒ�
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).NVI_SetText(name);
}

//#668
/**
�T�u�y�C���J�������܂܂Ȃ�window bounds���擾
*/
/*#725
void	AWindow_Text::SPI_GetWindowBoundsWithoutSubPane( ARect& outRect ) const
{
	NVI_GetWindowBounds(outRect);
	if( mLeftSideBarDisplayed == true )
	{
		outRect.left += mLeftSideBarWidth;
	}
}
*/

#pragma mark ===========================

#pragma mark ---�E�T�C�h�o�[
//#291

/**
�E�T�C�h�o�[�̕\���E��\���ݒ�
*/
void	AWindow_Text::ShowHideRightSideBar( const ABool inShow )
{
	//
	if( inShow == true )
	{
		//�E�T�C�h�o�[�������Ȃ琶��
		if( mRightSideBarCreated == false )
		{
			CreateRightSideBar();
			mRightSideBarCreated = true;
		}
		
		//================ �\�� ================
		
		if( mRightSideBarDisplayed == true )   return;
		
		//#0 Pane�T�|�[�g�`�F�b�N
		if( GetApp().SPI_IsSupportPaneMode() == false )   return;
		
		//�E�T�C�h�o�[�\��
		
		//InfoPane�\����Ԑݒ�
		ABool	oldInfoPaneDisplayed = mRightSideBarDisplayed;
		mRightSideBarDisplayed = true;
		
		if( NVI_IsFullScreen() == false )
		{
			//�E�T�C�h�o�[�A�j���[�V�����pbindings�ݒ�
			UpdateViewBindings(kTextWindowViewBindings_ShowRightSidebar);
			
			//�T�C�h�o�[�w�i�\��
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetCurrentTabIndex(),layout);
			NVI_SetControlPosition(kControlID_RightSideBarBackground,layout.pt_RightSideBar);
			NVI_SetControlSize(kControlID_RightSideBarBackground,layout.w_RightSideBar,layout.h_RightSideBar_WithBottomMargin);
			NVI_SetShowControl(kControlID_RightSideBarBackground,true);
			
			//window bounds�ݒ�
			ARect	bounds = {0};
			NVI_GetWindowBounds(bounds);
			bounds.right += mRightSideBarWidth;
			//
			{
				//window bounds�X�V�i�A�j���[�V�����j
				AStSuppressTextWindowUpdateViewBounds	s(GetObjectID());
				//
				mDoingSetWindowBoundsAnimate = true;
				NVI_SetWindowBounds(bounds,true);
				mDoingSetWindowBoundsAnimate = false;
			}
			
			//�ʏ�bindings�ݒ�
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
	}
	else
	{
		//================ ��\�� ================
		
		if( mRightSideBarDisplayed == false )   return;
		
		//�E�T�C�h�o�[��\��
		
		//InfoPane�\����Ԑݒ�
		ABool	oldInfoPaneDisplayed = mRightSideBarDisplayed;
		mRightSideBarDisplayed = false;
		
		if( NVI_IsFullScreen() == false )
		{
			//��ɃT�u�E�C���h�E��Hide����B
			//�A�j���[�V�������{���ɁA�T�u�E�C���h�E��\�����Ȃ��悤�ɂ��邽�߁B
			UpdateVisibleSubWindows();
			
			//�E�T�C�h�o�[�A�j���[�V�����pbindings�ݒ�
			UpdateViewBindings(kTextWindowViewBindings_HideRightSidebar);
			
			//window bounds�ݒ�
			ARect	bounds = {0};
			NVI_GetWindowBounds(bounds);
			bounds.right -= mRightSideBarWidth;
			//
			{
				//window bounds�X�V�i�A�j���[�V�����j
				AStSuppressTextWindowUpdateViewBounds	s(GetObjectID());
				//
				mDoingSetWindowBoundsAnimate = true;
				NVI_SetWindowBounds(bounds,true);
				mDoingSetWindowBoundsAnimate = false;
			}
			
			//�ʏ�bindings�ݒ�
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
	}
	
	//
	//#326 UpdateViewBounds(NVI_GetCurrentTabIndex());
	//#326
	//�S�^�u��View�̈ʒu�A�T�C�Y�ݒ�
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		UpdateViewBounds(tabIndex);
		//�s���Čv�Z�i�L���r���[���ς��̂Łj
		//#725 �T�C�h�o�[���ɂ��L���r���[���ς��Ȃ� GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_DoWrapRelatedDataChanged(false);
	}
	NVI_RefreshWindow();
	
	//�ݒ�ۑ�
	//#569 �ۑ����ׂ������f
	ABool	shouldSave = true;
	/*#850
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true &&
				SPI_IsTabModeMainWindow() == false )
	{
		//�^�u�E�C���h�E���[�h�̏ꍇ�A���C���E�C���h�E�ȊO�ł͕ۑ����Ȃ�
		shouldSave = false;
	}
	*/
	if( mLuaConsoleMode == true )
	{
		//Lua Console�ł͕ۑ����Ȃ�
		shouldSave = false;
	}
	//�ۑ�
	if( shouldSave == true )
	{
		GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kRightSideBarDisplayed,mRightSideBarDisplayed);
	}
	//#602
	UpdateVSeparatorHelpTag();
	
	//doc info�E�C���h�E�X�V #1053
	SPI_GetCurrentTabTextDocument().SPI_UpdateDocInfoWindows();
}

/**
�E�T�C�h�o�[���ړ�
*/
ANumber	AWindow_Text::ChangeRightSideBarWidth( const ANumber inDelta, const ABool inCompleted )//#409
{
	//�E�C���h�E�T�C�Y�擾
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	//���C�A�E�g�f�[�^�擾
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	
	//���X�̕����L�� #725
	ANumber	origWidth = mRightSideBarWidth;
	
	//�E�T�C�h�o�[���ύX
	mRightSideBarWidth -= inDelta;
	
	//�E�T�C�h�o�[���␳
	if( mRightSideBarWidth < kMinWidth_SideBar )
	{
		mRightSideBarWidth = kMinWidth_SideBar;
	}
	if( mRightSideBarWidth > layout.w_MainColumn + layout.w_RightSideBar - kMinWidth_MainColumn )
	{
		mRightSideBarWidth = layout.w_MainColumn + layout.w_RightSideBar - kMinWidth_MainColumn;
	}
	//
	if( inCompleted == false )//#409
	{
		UpdateViewBounds(NVI_GetCurrentTabIndex(),kUpdateViewBoundsTriggerType_InfoPaneVSeparator);//#645
		NVI_UpdateWindow();
	}
	else
	{
		//==================�T�C�Y�ύX������==================
		
		//�T�C�Y���O��̊������ƕς���Ă���ꍇ�̂݁A�s���Čv�Z���̏������s���i�Ԉ���ăN���b�N�����i�h���b�O�Ȃ��j�����Ƃ��ɏ������Ȃ��悤�ɂ��邽�߁j
		if( mRightSideBarWidth != mPrevRightSideBarWidth )
		{
			//
			mPrevRightSideBarWidth = mRightSideBarWidth;
			
			//�S�^�u��View�̈ʒu�A�T�C�Y�ݒ�
			for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
			{
				UpdateViewBounds(tabIndex,kUpdateViewBoundsTriggerType_InfoPaneVSeparator_Completed);//#645
				//�s���Čv�Z�i�L���r���[���ς��̂Łj
				GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_DoWrapRelatedDataChanged(false);
			}
			//#324
			NVI_RefreshWindow();
			
			//DB�ɕۑ�
			GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kRightSideBarWidth,mRightSideBarWidth);
		}
	}
	//���ۂ�delta�v�Z #725
	return origWidth - mRightSideBarWidth;
}

/**
�E�T�C�h�o�[�����ύX
*/
ANumber	AWindow_Text::ChangeRightSideBarHeight( const AIndex inItemIndex, const ANumber inDelta )
{
	//���X�̍����L��
	AFloatNumber	origHeight = mRightSideBarArray_Height.Get(inItemIndex);
	if( inDelta < 0 )
	{
		//==================����������������==================
		
		//�w��T�C�h�o�[���ڂ̍ŏ������A���݂̍������擾
		AWindowID	winID0 = mRightSideBarArray_OverlayWindowID.Get(inItemIndex);
		AFloatNumber	minHeight0 = GetApp().SPI_GetSubWindowMinHeight(winID0);
		AFloatNumber	height0 = mRightSideBarArray_Height.Get(inItemIndex);
		//�w��T�C�h�o�[���ڂ̎��ȍ~��collapsed�ł͂Ȃ����ڂ������A�������擾
		AFloatNumber	height1 = 0;
		AIndex	changeIndex = kIndex_Invalid;
		AItemCount	itemCount = mRightSideBarArray_Height.GetItemCount();
		for( changeIndex = inItemIndex+1; changeIndex < itemCount; changeIndex++ )
		{
			if( mRightSideBarArray_Collapsed.Get(changeIndex) == false )
			{
				height1 = mRightSideBarArray_Height.Get(changeIndex);
				break;
			}
		}
		if( height1 != 0 )
		{
			//�����ύX�i�w�荀�ڂ̍��������������A���i�ȍ~�̂����ŏ���collapsed�łȂ����́j�̍��ڂ̍��������̕��傫������j
			if( height0 + inDelta >= minHeight0 )
			{
				//�ύX��A�ŏ��������͑傫���ꍇ
				height0 += inDelta;
				height1 -= inDelta;
			}
			else
			{
				//�ύX��A�ŏ������ɂȂ�ꍇ
				height1 += height0 - minHeight0;
				height0 = minHeight0;
			}
			//�����ݒ�
			mRightSideBarArray_Height.Set(inItemIndex,height0);
			mRightSideBarArray_Height.Set(changeIndex,height1);
		}
	}
	else
	{
		//==================������傫������==================
		
		//�ύX���ׂ��T�C�Y�̎c���remainDelta�Ɋi�[����B
		//���ȍ~�̍��ڂ̃T�C�Y���o������菬�������Ă����āAinDelta�̒l���m�ۂł���܂ő�����
		ANumber	remainDelta = inDelta;
		//���ȍ~�̍��ڂŃ��[�v
		AItemCount	itemCount = mRightSideBarArray_Height.GetItemCount();
		for( AIndex i = inItemIndex+1; i < itemCount; i++ )
		{
			//collapsed�ȍ��ڂ͏������ł��Ȃ��̂ŁA�������Ȃ�
			if( mRightSideBarArray_Collapsed.Get(i) == true )
			{
				continue;
			}
			//���ڂ̍ŏ������A�������擾
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(i);
			AFloatNumber	minHeight = GetApp().SPI_GetSubWindowMinHeight(winID);
			AFloatNumber	height = mRightSideBarArray_Height.Get(i);
			//�o������菬�������āA�������������̒l��remainDelta�������
			if( height - remainDelta >= minHeight )
			{
				height = height - remainDelta;
				remainDelta = 0;
			}
			else
			{
				remainDelta -= height-minHeight;
				height = minHeight;
			}
			//�����X�V
			mRightSideBarArray_Height.Set(i,height);
			//remainDelta��0�ɂȂ�����I��
			if( remainDelta <= 0 )
			{
				break;
			}
		}
		//���ȍ~�̍��ڂ��������ł����������A�w�荀�ڂ�傫������
		AFloatNumber	height = mRightSideBarArray_Height.Get(inItemIndex);
		height += inDelta-remainDelta;
		mRightSideBarArray_Height.Set(inItemIndex,height);
	}
	
	//�T�C�h�o�[�Ĕz�u
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	UpdateLayout_RightSideBar(NVI_GetCurrentTabIndex(),layout);
	UpdateViewBounds(NVI_GetCurrentTabIndex());
	//�T�C�h�o�[�f�[�^�ݒ�
	SetDataSideBar();
	//���ۂ�delta�v�Z
	return mRightSideBarArray_Height.Get(inItemIndex) - origHeight;
}

/**
�E�T�C�h�o�[�T�u�E�C���h�E����
*/
void	AWindow_Text::CreateRightSideBar()
{
	if( GetApp().SPI_IsSupportPaneMode() == false )   return;
	
	//�E�T�C�h�o�[�T�u�E�C���h�E���擾
	AItemCount	itemCount = GetApp().NVI_GetAppPrefDB().GetItemCount_Array(AAppPrefDB::kRightSideBarArray_Type);
	for( AIndex index = 0; index < itemCount; index++ )
	{
		CreateRightSideBarItem(index);
	}
}

/**
�E�T�C�h�o�[���ڐ����i�N�������ɃR�[�������BDB�f�[�^�Ɋ�Â��Đ����j
*/
void	AWindow_Text::CreateRightSideBarItem( const AIndex inIndex )
{
	//�T�u�E�C���h�E�^�C�v�E�����擾�A�����o�ϐ��ւ̐ݒ�
	ASubWindowType	type = (ASubWindowType)(GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kRightSideBarArray_Type,inIndex));
	AFloatNumber	height = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kRightSideBarArray_Height,inIndex);
	ANumber	parameter = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kRightSideBarArray_Parameter,inIndex);
	ABool	collapsed =  GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kRightSideBarArray_Collapsed,inIndex);
	if( type != kSubWindowType_None )
	{
		//�T�u�E�C���h�E����
		AWindowID	winID = GetApp().SPI_CreateSubWindow(type,parameter,GetObjectID(),
					kSubWindowLocationType_RightSideBar,inIndex,collapsed);
		if( winID != kObjectID_Invalid )
		{
			mRightSideBarArray_Type.Add(type);
			mRightSideBarArray_Height.Add(height/10000.0);
			mRightSideBarArray_Dragging.Add(false);
			mRightSideBarArray_Collapsed.Add(collapsed);
			mRightSideBarArray_OverlayWindowID.Add(winID);
		}
		//���݃p�X��ݒ�
		AText	currentPath;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kRightSideBarArray_CurrentPath,inIndex,currentPath);
		switch(type)
		{
		  case kSubWindowType_FileList:
			{
				GetApp().SPI_GetFileListWindow(winID).SPI_SetCurrentPath(currentPath);
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
}

//#725
/**
�E�T�C�h�o�[�T�u�E�C���h�E�폜
*/
void	AWindow_Text::DeleteRightSideBar()
{
	//�T�u�E�C���h�E�폜
	for( AIndex index = 0; index < mRightSideBarArray_Type.GetItemCount(); index++ )
	{
		AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(index);
		if( winID != kObjectID_Invalid )
		{
			GetApp().SPI_DeleteSubWindow(winID);
		}
	}
	//�f�[�^�폜
	mRightSideBarArray_Type.DeleteAll();
	mRightSideBarArray_Height.DeleteAll();
	mRightSideBarArray_OverlayWindowID.DeleteAll();
	mRightSideBarArray_Dragging.DeleteAll();
	mRightSideBarArray_Collapsed.DeleteAll();
}

//#725
/**
�E�T�C�h�o�[���ڒǉ�
*/
void	AWindow_Text::AddNewSubWindowInRightSideBar( const ASubWindowType inSubWindowType, const ANumber inParameter,
		const ANumber inHeight )
{
	//DB�ɍ��ڃf�[�^�ǉ�
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Type,inSubWindowType);
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Height,inHeight*10000);
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Parameter,inParameter);
	GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kRightSideBarArray_Collapsed,false);
	GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kRightSideBarArray_CurrentPath,GetEmptyText());
	//�E�T�C�h�o�[���ڐ���
	CreateRightSideBarItem(GetApp().NVI_GetAppPrefDB().GetItemCount_NumberArray(AAppPrefDB::kRightSideBarArray_Type)-1);
	//view bounds�X�V
	UpdateViewBounds(NVI_GetCurrentTabIndex());
	//�T�C�h�o�[�f�[�^�ݒ�
	SetDataSideBar();
}

//#725
/**
�E�T�C�h�o�[���ڂ�S�ăf�t�H���g�ɖ߂�
*/
void	AWindow_Text::SetDefaultSubWindowInRightSideBar()
{
	//�E�T�C�h�o�[�f�[�^�폜
	DeleteRightSideBar();
	//DB�f�[�^���f�t�H���g�ɖ߂�
	GetApp().GetAppPref().SetDefaultRightSideBarArray();
	//�E�T�C�h�o�[����
	CreateRightSideBar();
}

/*#725
//
void	AWindow_Text::DeleteInfoPaneArray()
{
	for( AIndex i = 0; i < mRightSideBarArray_Type.GetItemCount(); i++ )
	{
		GetApp().NVI_DeleteWindow(mRightSideBarArray_HSeparatorWindowID.Get(i));
	}
	mRightSideBarArray_Type.DeleteAll();
	mRightSideBarArray_Height.DeleteAll();
	mRightSideBarArray_HSeparatorWindowID.DeleteAll();
}
*/

/*#725 
//
void	AWindow_Text::RevertFromOverlay_IdInfoWindow()
{
	GetApp().SPI_GetIdInfoWindow().NVI_ChangeToFloating();
	//Show/Hide����
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayIdInfoWindow) == true )
	{
		GetApp().SPI_GetIdInfoWindow().NVI_Show(false);
	}
	else
	{
		GetApp().SPI_GetIdInfoWindow().NVI_Hide();
	}
}

//
void	AWindow_Text::RevertFromOverlay_FileListWindow()
{
	GetApp().SPI_GetFileListWindow().NVI_ChangeToFloating();
	//Show/Hide����
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayWindowList) == true )
	{
		GetApp().SPI_GetFileListWindow().NVI_Show(false);
	}
	else
	{
		GetApp().SPI_GetFileListWindow().NVI_Hide();
	}
}
*/

/*#725
void	AWindow_Text::SPI_UpdateOverlayWindowAlpha()
{
	SPI_GetJumpListWindow().SPI_UpdateOverlayWindowAlpha();
}
*/

#pragma mark ===========================

#pragma mark ---�|�b�v�A�b�v�E�C���h�E

//�e�|�b�v�A�b�v�E�C���h�E�T�C�Y
const ANumber	kPopupCandidateWindowHeight = 150;
const ANumber	kPopupIdInfoWindowHeight = 200;
const ANumber	kPopupIdInfoWindowWidth = 300;
const ANumber	kPopupAdjustThreasholdHeight = 30;

/**
�L�[���[�h���|�b�v�A�b�v�E�C���h�E��\���^��\��
*/
void	AWindow_Text::SPI_ShowIdInfoPopupWindow( const ABool inShow )
{
	//text view�S�̂�global rect�擾
	AGlobalRect	viewGlobalRect = {0};
	SPI_GetCurrentFocusTextView().NVI_GetGlobalViewRect(viewGlobalRect);
	
	if( inShow == true && GetPopupIdInfoWindow().SPI_GetIdInfoView().SPI_GetIdInfoItemCount() > 0 && //mProhibitPopup == false &&
	   GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup) == false &&
	   GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPopupIdInfo) == true )
	{
		//���C�A�E�g�v�Z
		ATextWindowPopupLayoutData	layout = {0};
		mCurrentPopupLayoutPattern = CalcTextWindowPopupLayout(layout);
		//�z�u�E�\��
		GetPopupIdInfoWindow().NVI_SetWindowBounds(layout.idInfoWindowRect);
		GetPopupIdInfoWindow().NVI_Show(false);
	}
	else
	{
		//��\��
		GetPopupIdInfoWindow().NVI_Hide();
	}
}

/**
�|�b�v�A�b�vIdInfo�E�C���h�E�擾
*/
AWindow_IdInfo&	AWindow_Text::GetPopupIdInfoWindow()
{
	//�������Ȃ琶��
	if( mPopupIdInfoWindowID == kObjectID_Invalid )
	{
		mPopupIdInfoWindowID = GetApp().SPI_CreateSubWindow(kSubWindowType_IdInfo,0,GetObjectID(),
					kSubWindowLocationType_Popup,kIndex_Invalid,false);
		GetPopupIdInfoWindow().NVI_Create(kObjectID_Invalid);
	}
	//IdInfo�E�C���h�E�擾
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_IdInfo,kWindowType_IdInfo,mPopupIdInfoWindowID);
}

/**
���|�b�v�A�b�v
*/
void	AWindow_Text::SPI_ShowCandidatePopupWindow( const ABool inShow, const ABool inAbbrevInputMode )
{
	//�|�b�v�A�b�v�ʒu�ݒ�擾
	ABool	showBelowInputText = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPopupCandidateBelowInputText);
	ABool	showNearInputText = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPopupCandidateNearInputText);
	//
	if( inShow == true && (showBelowInputText==true||showNearInputText==true) &&// mProhibitPopup == false )
	   GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup) == false )
	{
		//���C�A�E�g�v�Z
		ATextWindowPopupLayoutData	layout = {0};
		mCurrentPopupLayoutPattern = CalcTextWindowPopupLayout(layout);
		//�z�u�E�\��
		SPI_GetPopupCandidateWindow().NVI_SetWindowBounds(layout.candidateWindowRect);
		SPI_GetPopupCandidateWindow().NVI_Show(false);
	}
	else
	{
		//��\��
		SPI_GetPopupCandidateWindow().NVI_Hide();
	}
}

/**
�|�b�v�A�b�v�̌�⃊�X�g�E�C���h�E�擾
*/
AWindow_CandidateList&	AWindow_Text::SPI_GetPopupCandidateWindow()
{
	//�������Ȃ琶��
	if( mPopupCandidateWindowID == kObjectID_Invalid )
	{
		mPopupCandidateWindowID = GetApp().SPI_CreateSubWindow(kSubWindowType_CandidateList,0,GetObjectID(),
					kSubWindowLocationType_Popup,kIndex_Invalid,false);
		SPI_GetPopupCandidateWindow().NVI_Create(kObjectID_Invalid);
	}
	//�|�b�v�A�b�v�E�C���h�E����
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_CandidateList,kWindowType_CandidateList,mPopupCandidateWindowID);
}

/**
�|�b�v�A�b�v�⊮���E�C���h�E�\�������ǂ������擾
*/
ABool	AWindow_Text::SPI_IsPopupCandidateWindowVisible()
{ 
	if( mPopupCandidateWindowID == kObjectID_Invalid )   return false;
	return SPI_GetPopupCandidateWindow().NVI_IsWindowVisible();
}

/**
�|�b�v�A�b�v�E�C���h�E�̈ʒu�����݂̃L�����b�g�ʒu�ɍ��킹�Ē���
*/
void	AWindow_Text::SPI_AdjustPopupWindowsPosition()
{
	if( GetPopupIdInfoWindow().NVI_IsWindowVisible() == true )
	{
		//���݂�focus��text view�̑I��͈�global rect���擾
		AGlobalRect	textViewSelectionGlobalRect = {0};
		SPI_GetCurrentFocusTextView().SPI_GetSelectGlobalRect(textViewSelectionGlobalRect);
		//�L�[���[�h���|�b�v�A�b�v�E�C���h�E��bounds�擾
		ARect	winrect = {0};
		GetPopupIdInfoWindow().NVI_GetWindowBounds(winrect);
		if( textViewSelectionGlobalRect.top > winrect.top - kPopupAdjustThreasholdHeight && 
					textViewSelectionGlobalRect.top < winrect.bottom + kPopupAdjustThreasholdHeight )
		{
			//���݂̃��C�A�E�g�p�^�[��
			ATextWindowPopupLayoutPattern	oldPattern = mCurrentPopupLayoutPattern;
			//���C�A�E�g�v�Z
			ATextWindowPopupLayoutData	layout = {0};
			mCurrentPopupLayoutPattern = CalcTextWindowPopupLayout(layout);
			
			//���C�A�E�g�p�^�[��A��C�̏ꍇ�́A�|�b�v�A�b�v�����
			ABool	shouldShow = true;
			if( (oldPattern == kTextWindowPopupLayoutPatternA && mCurrentPopupLayoutPattern == kTextWindowPopupLayoutPatternC) )
			{
				shouldShow = false;
			}
			
			//�L�[���[�h���̍��ڂ�0�Ȃ�|�b�v�A�b�v�����
			if( GetPopupIdInfoWindow().SPI_GetIdInfoView().SPI_GetIdInfoItemCount() == 0 )
			{
				shouldShow = false;
			}
			
			//�\���E��\��
			SPI_ShowIdInfoPopupWindow(shouldShow);
		}
	}
	
}

/**
�c�[�����X�g�|�b�v�A�b�v�E�C���h�E�擾
*/
AWindow_KeyToolList&	AWindow_Text::SPI_GetPopupKeyToolListWindow()
{
	//�������Ȃ琶��
	if( mPopupKeyToolListWindowID == kObjectID_Invalid )
	{
		mPopupKeyToolListWindowID = GetApp().SPI_CreateSubWindow(kSubWindowType_KeyToolList,0,GetObjectID(),
					kSubWindowLocationType_Popup,kIndex_Invalid,false);
		SPI_GetPopupKeyToolListWindow().NVI_Create(kObjectID_Invalid);
	}
	//�|�b�v�A�b�v�E�C���h�E����
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_KeyToolList,kWindowType_KeyToolList,mPopupKeyToolListWindowID);
}

/**
�|�b�v�A�b�v�E�C���h�E�̃��C�A�E�g�v�Z
*/
ATextWindowPopupLayoutPattern	AWindow_Text::CalcTextWindowPopupLayout( ATextWindowPopupLayoutData& outLayout ) 
{
	//text view�̌��݂̑I��͈͂�globa rect���擾
	AGlobalRect	textViewCurrentWordGlobalRect = {0};
	SPI_GetCurrentFocusTextView().SPI_GetCurrentWordGlobalRect(textViewCurrentWordGlobalRect);
	
	//text view�S�̂�global rect�擾
	AGlobalRect	viewGlobalRect = {0};
	SPI_GetCurrentFocusTextView().NVI_GetGlobalViewRect(viewGlobalRect);
	
	//==================���E�C���h�E�z�u==================
	
	//
	ABool	candidateWindowDisplayedBelowSelection = true;
	
	//���E�C���h�E�̍ő卂���iview�̃C���[�W�����{�e�L�X�g�p�l�������j���擾
	ANumber	candidateWindowMaxHeight = SPI_GetPopupCandidateWindow().SPI_GetMaxWindowHeight();
	//�ő卂�����͍����Ȃ�Ȃ��悤�ɂ���
	ANumber	candidateWindowHeight = kPopupCandidateWindowHeight;
	if( candidateWindowHeight > candidateWindowMaxHeight )
	{
		candidateWindowHeight = candidateWindowMaxHeight;
	}
	
	//
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPopupCandidateNearInputText) == true )
	{
		//------------------�e�L�X�g���͈ʒu�́h�߂��h�Ƀ|�b�v�A�b�v------------------
		
		//
		const ANumber	kCandidateWindowYOffset = 80;
		const ANumber	kCandidateWindowRightMargin = 5;
		
		//
		outLayout.candidateWindowRect.right 	= viewGlobalRect.right - kCandidateWindowRightMargin;
		outLayout.candidateWindowRect.left 		= outLayout.candidateWindowRect.right - SPI_GetPopupCandidateWindow().NVI_GetWindowWidth();
		
		//�ォ�����𔻒�
		//�i���݂̑I��͈͉��̕\���\�̈�̍��������E�C���h�E�̍����{�I�t�Z�b�g�Ȃ牺�ɕ\���j
		if( viewGlobalRect.bottom - textViewCurrentWordGlobalRect.bottom > kCandidateWindowYOffset + /*candidateWindowHeight*/kPopupCandidateWindowHeight )
		{
			//���ɕ\���\�Ȃ牺�ɕ\������
			
			//
			outLayout.candidateWindowRect.top		= textViewCurrentWordGlobalRect.bottom + kCandidateWindowYOffset;
			outLayout.candidateWindowRect.bottom	= outLayout.candidateWindowRect.top + candidateWindowHeight;
		}
		else
		{
			//���ɕ\���s�\�Ȃ��ɕ\������
			
			//
			outLayout.candidateWindowRect.bottom	= textViewCurrentWordGlobalRect.top - kCandidateWindowYOffset;
			outLayout.candidateWindowRect.top		= outLayout.candidateWindowRect.bottom - candidateWindowHeight;
			
			//
			candidateWindowDisplayedBelowSelection = false;
		}
	}
	else
	{
		//------------------�e�L�X�g���͈ʒu�̂������i�܂��͏�j�Ƀ|�b�v�A�b�v------------------
		
		//
		const ANumber	kCandidateWindowYOffset = 16;
		
		//
		ANumber	keywordStartOffset = SPI_GetPopupCandidateWindow().SPI_GetCandidateListViewConst().SPI_GetKeywordStartX();
		AWindowPoint	listViewPoint = {0};
		SPI_GetPopupCandidateWindow().NVI_GetControlPosition(
			SPI_GetPopupCandidateWindow().SPI_GetCandidateListViewConst().NVI_GetControlID(),listViewPoint);
		
		//
		outLayout.candidateWindowRect.left		= textViewCurrentWordGlobalRect.left - keywordStartOffset - listViewPoint.x;
		outLayout.candidateWindowRect.right		= outLayout.candidateWindowRect.left + SPI_GetPopupCandidateWindow().NVI_GetWindowWidth();
		
		//�ォ�����𔻒�
		//�i���݂̑I��͈͉��̕\���\�̈�̍��������E�C���h�E�̍����{�I�t�Z�b�g�Ȃ牺�ɕ\���j
		if( viewGlobalRect.bottom - textViewCurrentWordGlobalRect.bottom > kCandidateWindowYOffset + /*candidateWindowHeight*/kPopupCandidateWindowHeight )
		{
			//���ɕ\���\�Ȃ牺�ɕ\������
			
			//
			outLayout.candidateWindowRect.top		= textViewCurrentWordGlobalRect.bottom + kCandidateWindowYOffset;
			outLayout.candidateWindowRect.bottom	= outLayout.candidateWindowRect.top + candidateWindowHeight;
		}
		else
		{
			//���ɕ\���s�\�Ȃ��ɕ\������
			
			//
			outLayout.candidateWindowRect.bottom	= textViewCurrentWordGlobalRect.top - kCandidateWindowYOffset;
			outLayout.candidateWindowRect.top		= outLayout.candidateWindowRect.bottom - candidateWindowHeight;
			
			//
			candidateWindowDisplayedBelowSelection = false;
		}
	}
	
	//==================�L�[���[�h���E�C���h�E�z�u==================
	
	const ANumber	kIdInfoWindowYOffset1 = 80;
	const ANumber	kIdInfoWindowYOffset2 = 20;
	
	//�L�[���[�h���E�C���h�E�̍ő卂�����擾
	ANumber	idInfoWindowMaxHeight = GetPopupIdInfoWindow().SPI_GetMaxWindowHeight();
	//�L�[���[�h���E�C���h�E���ڂ̍ő卂�����擾
	ANumber	idInfoBoxMaxHeight = GetPopupIdInfoWindow().SPI_GetIdInfoView().SPI_GetMaxItemHeight();
	//�ő卂�����͍����Ȃ�Ȃ��悤�ɂ��A���ڍő卂�����͒Ⴍ�Ȃ�Ȃ��悤�ɂ���
	ANumber	idInfoWindowHeight = kPopupIdInfoWindowHeight;
	if( idInfoWindowHeight < idInfoBoxMaxHeight )
	{
		idInfoWindowHeight = idInfoBoxMaxHeight;
	}
	if( idInfoWindowHeight > idInfoWindowMaxHeight )
	{
		idInfoWindowHeight = idInfoWindowMaxHeight;
	}
	
	//
	outLayout.idInfoWindowRect.right = viewGlobalRect.right - 2;
	outLayout.idInfoWindowRect.left = outLayout.idInfoWindowRect.right - kPopupIdInfoWindowWidth;
	
	//�㕔�̕\���\�̈���v�Z
	ANumber	spaceAboveSelection = textViewCurrentWordGlobalRect.top - viewGlobalRect.top;
	if( candidateWindowDisplayedBelowSelection == false )
	{
		spaceAboveSelection = outLayout.candidateWindowRect.top - viewGlobalRect.top;
	}
	
	//
	ATextWindowPopupLayoutPattern	layoutPattern = kTextWindowPopupLayoutPatternA;
	
	//��ɕ\���\���ǂ����𔻒�
	if( spaceAboveSelection > kIdInfoWindowYOffset1 + idInfoWindowHeight )
	{
		//��ɕ\���\�Ȃ�A��ɕ\��
		
		//
		if( candidateWindowDisplayedBelowSelection == false )
		{
			//���E�C���h�E����ɕ\������ꍇ�́A���E�C���h�E�̏�ɕ\������B�i�p�^�[��B�j
			outLayout.idInfoWindowRect.bottom = outLayout.candidateWindowRect.top - kIdInfoWindowYOffset2;
			outLayout.idInfoWindowRect.top = outLayout.idInfoWindowRect.bottom - idInfoWindowHeight;
			layoutPattern = kTextWindowPopupLayoutPatternB;
		}
		else
		{
			//���E�C���h�E�����ɕ\������ꍇ�́A�I��͈͂̏�ɕ\������B�i�p�^�[��A�j
			outLayout.idInfoWindowRect.bottom = textViewCurrentWordGlobalRect.top - kIdInfoWindowYOffset1;
			outLayout.idInfoWindowRect.top = outLayout.idInfoWindowRect.bottom - idInfoWindowHeight;
			layoutPattern = kTextWindowPopupLayoutPatternA;
		}
	}
	else
	{
		//��ɕ\���s�\�Ȃ�A���E�C���h�E�̉��ɕ\���i�p�^�[��C�j
		
		outLayout.idInfoWindowRect.top = outLayout.candidateWindowRect.top + kIdInfoWindowYOffset2;
		outLayout.idInfoWindowRect.bottom = outLayout.idInfoWindowRect.top + idInfoWindowHeight;
		layoutPattern = kTextWindowPopupLayoutPatternC;
	}
	return layoutPattern;
}

/**
�S�Ẵ|�b�v�A�b�v�E�C���h�E�����
*/
void	AWindow_Text::SPI_HideAllPopupWindows()
{
	GetPopupIdInfoWindow().NVI_Hide();
	SPI_GetPopupCandidateWindow().NVI_Hide();
}

#pragma mark ===========================

#pragma mark ---Diff�\��
//#379

/**
Diff�\�����X�V�idiff�f�[�^�X�V���j
*/
void	AWindow_Text::SPI_UpdateDiffDisplay( const ADocumentID inDocumentID )
{
	//���ݕ\�����̃h�L�������g���Ώۃh�L�������g�̏ꍇ�̂ݍX�V
	if( inDocumentID != NVI_GetCurrentDocumentID() )   return;
	
	//Working�̂ق��̃^�u���擾
	AIndex	workingTabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	//diff�Ώۂ̂ق��̃^�u���擾
	AIndex	diffDocTabIndex = GetDiffDocumentTabIndexFromWorkingTabIndex(workingTabIndex);
	if( mDiffDisplayMode == true && diffDocTabIndex != kIndex_Invalid )
	{
		//Repository���T�u�y�C���ɕ\��
		//#450 AImagePoint	origin = {0,0};
		SPI_DisplayInSubText(diffDocTabIndex,false,kIndex_Invalid,false,true);
		//�X�N���[���ʒu�����킹��
		AControlID	view0 = mTabDataArray.GetObjectConst(workingTabIndex).GetTextViewControlID(0);
		SPI_AdjustDiffDisplayScroll(inDocumentID,view0,kIndex_Invalid);
	}
	//Diff�\���X�V
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
	//Diff�{�^���\���X�V
	SPI_UpdateDiffButtonText();
}

/**
Diff�\���̃X�N���[�����킹�i�ƍs�ԍ�View����Diff�Ή��\���X�V�j���s��
*/
void	AWindow_Text::SPI_AdjustDiffDisplayScroll( const ADocumentID inWorkingDocID, const AControlID inWorkingViewControlID, const AIndex inCurrentDiffIndex )
{
	//dirty���̓X�N���[�����킹���Ȃ�
	if( GetApp().SPI_GetTextDocumentByID(inWorkingDocID).NVI_IsDirty() == true )
	{
		return;
	}
	
	//�����r���[�̂Q�Ԗڈȍ~�̏ꍇ�̓X�N���[�����@�����Ȃ�
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inWorkingDocID);
	if( mTabDataArray.GetObjectConst(tabIndex).FindFromTextViewControlID(inWorkingViewControlID) > 0 )   return;
	
#if 0
	if( mIsMainSubDiffMode == true )
	{
		//�X�N���[�������h�L�������g��main text�łȂ��ꍇ�́A�s�ԍ�View�̕\���X�V�������ďI��
		if( inWorkingViewControlID != mMainSubDiff_MainTextControlID )
		{
			//Diff�\���X�V #379
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
#if IMPLEMENTATION_FOR_WINDOWS
			//�� Mac�ł͂��ꂪ�Ȃ��ق���������Ȃ��BWindows�ł͂��ꂪ����ق���������Ȃ��B
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_UpdateWindow();
#endif
			return;
		}
		
		//Y��������
		AImagePoint	rightOrigin = {0};
		GetTextViewByControlID(mMainSubDiff_MainTextControlID).NVI_GetOriginOfLocalFrame(rightOrigin);
		//Main Text�̃t���[���J�n�i��index�擾
		AIndex	viewStartParagraphIndex = GetTextViewByControlID(mMainSubDiff_MainTextControlID).SPI_GetViewStartParagraphIndex();
		AIndex	viewEndParagraphIndex = GetTextViewByControlID(mMainSubDiff_MainTextControlID).SPI_GetViewEndParagraphIndex();
		//�E���ŏ���DiffIndex���擾���A�E���ŏ���Diff������΁A�����̑Ή�����Diff�����傤�Ǔ��������ɕ\�������悤�ɃX�N���[��
		for( AIndex paraIndex = viewStartParagraphIndex; paraIndex < viewEndParagraphIndex; paraIndex++ )
		{
			AIndex	rightFirstDiffIndex = SPI_MainSubDiff_GetDiffIndexByStartParagraphIndex(paraIndex);
			if( rightFirstDiffIndex != kIndex_Invalid )
			{
				AIndex	rightFirstDiffLineIndex = SPI_GetTextDocumentConst(mMainSubDiff_MainTextControlID).
						SPI_GetLineIndexByParagraphIndex(mMainSubDiff_StartParagraphIndexArray.Get(rightFirstDiffIndex));
				AIndex	leftFirstDiffLineIndex = SPI_GetTextDocumentConst(mMainSubDiff_SubTextControlID).
						SPI_GetLineIndexByParagraphIndex(mMainSubDiff_OldStartParagraphIndexArray.Get(rightFirstDiffIndex));
				//
				ANumber	offset = GetTextViewByControlID(mMainSubDiff_MainTextControlID).GetImageYByLineIndex(rightFirstDiffLineIndex)
						- rightOrigin.y;
				AImagePoint	leftOrigin = {0};
				GetTextViewByControlID(mMainSubDiff_SubTextControlID).NVI_GetOriginOfLocalFrame(leftOrigin);
				leftOrigin.y = GetTextViewByControlID(mMainSubDiff_SubTextControlID).GetImageYByLineIndex(leftFirstDiffLineIndex)
						- offset;
				GetTextViewByControlID(mMainSubDiff_SubTextControlID).NVI_ScrollTo(leftOrigin);
				break;
			}
		}
		//X��������
		AImagePoint	leftOrigin = {0};
		GetTextViewByControlID(mMainSubDiff_SubTextControlID).NVI_GetOriginOfLocalFrame(leftOrigin);
		leftOrigin.x = rightOrigin.x;
		//�X�N���[�����s
		GetTextViewByControlID(mMainSubDiff_SubTextControlID).NVI_ScrollTo(leftOrigin);
		//Diff�\���X�V
		GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
#if IMPLEMENTATION_FOR_WINDOWS
		//�� Mac�ł͂��ꂪ�Ȃ��ق���������Ȃ��BWindows�ł͂��ꂪ����ق���������Ȃ��B
		GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_UpdateWindow();
#endif
		//#611 diffInfoWindow�̕\���ƁA�{�E�C���h�E�̕\���̃^�C�~���O�������ɂ������̑΍�
		NVI_UpdateWindow();
	}
	//Diff�\�����[�h����
	else 
#endif
	if( mDiffDisplayMode == true )
	{
		//�X�N���[�������h�L�������g(inWorkingDocID)��Diff�Ώۃh�L�������g�̏ꍇ�́A�s�ԍ�View�̕\���X�V�������ďI��
		ADocumentID	workingDocID = GetApp().SPI_GetTextDocumentByID(inWorkingDocID).SPI_GetDiffWorkingDocumentID();
		if( workingDocID != kObjectID_Invalid )
		{
			//Diff�\���X�V #379
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
#if IMPLEMENTATION_FOR_WINDOWS
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_UpdateWindow();
#endif
			NVI_UpdateWindow();
			return;
		}
		
		//==================Working���̃X�N���[���ʒu�ɍ��킹��Diff���̃X�N���[���ʒu�𒲐�����==================
		//�i�����ȍ~�́AinWorkingDocID�͏�ɁAworking���̃h�L�������g�ł���B�j
		
		//Diff�Ώۃh�L�������g�擾
		ADocumentID	diffTargetDocID = GetApp().SPI_GetTextDocumentByID(inWorkingDocID).SPI_GetDiffTargetDocumentID();
		if( diffTargetDocID == kObjectID_Invalid )   return;
		
		//Working�h�L�������g�̃t���[���J�n�i��index�擾
		AIndex	viewStartParagraphIndex = GetTextViewByControlID(inWorkingViewControlID).SPI_GetViewStartParagraphIndex();
		AIndex	viewEndParagraphIndex = GetTextViewByControlID(inWorkingViewControlID).SPI_GetViewEndParagraphIndex();
		//�Ή�����Repository�h�L�������g�ł̒i��index�擾
		AIndex	diffViewStartParaIndex = GetApp().SPI_GetTextDocumentByID(inWorkingDocID).
				SPI_GetDiffTargetParagraphIndexFromThisDocumentParagraphIndex(viewStartParagraphIndex);
		AIndex	diffViewTabIndex = NVI_GetTabIndexByDocumentID(diffTargetDocID);
		if( diffViewTabIndex == kIndex_Invalid )   return;
		AControlID	diffTextViewControlID = mTabDataArray.GetObjectConst(diffViewTabIndex).GetSubTextControlID();
		if( diffTextViewControlID != kControlID_Invalid )
		{
			if( GetTextViewByControlID(diffTextViewControlID).NVI_IsVisible() == true )
			{
				//Working�h�L�������g��origin�擾
				AImagePoint	workingViewOrigin = {0,0};
				GetTextViewByControlID(inWorkingViewControlID).NVI_GetOriginOfLocalFrame(workingViewOrigin);
				//
				ATextPoint	selSpt = {0}, selEpt = {0};
				GetTextViewByControlID(inWorkingViewControlID).SPI_GetSelect(selSpt,selEpt);
				AImagePoint	selImageSpt = {0}, selImageEpt = {0}; 
				GetTextViewByControlID(inWorkingViewControlID).SPI_GetImageSelect(selImageSpt,selImageEpt);
				AImageRect	workingImageFrame = {0};
				GetTextViewByControlID(inWorkingViewControlID).NVI_GetImageViewRect(workingImageFrame);
				if( selImageSpt.y >= workingImageFrame.top && selImageSpt.y <= workingImageFrame.bottom )
				{
					//------------------working���I���J�n�ʒu�����݂̕\���t���[�����̏ꍇ------------------
					
					ANumber	offset = selImageSpt.y - workingImageFrame.top;
					//diff���̑Ή�����text point�擾
					ATextPoint	diffSelSpt = {0};
					GetApp().SPI_GetTextDocumentByID(diffTargetDocID).
							SPI_GetCorrespondingTextPointByDiffTargetTextPoint(selSpt,diffSelSpt);
					//
					AImagePoint	diffSelIpt = {0};
					GetTextViewByControlID(diffTextViewControlID).SPI_GetImagePointFromTextPoint(diffSelSpt,diffSelIpt);
					//
					AImagePoint	diffOrigin = {0};
					GetTextViewByControlID(diffTextViewControlID).NVI_GetOriginOfLocalFrame(diffOrigin);
					//
					AWindowPoint	workingViewWpt = {0};
					NVI_GetControlPosition(inWorkingViewControlID,workingViewWpt);
					AWindowPoint	diffViewWpt = {0};
					NVI_GetControlPosition(diffTextViewControlID,diffViewWpt);
					ANumber	viewPositionOffset = diffViewWpt.y - workingViewWpt.y;
					//
					diffOrigin.y = diffSelIpt.y - offset + viewPositionOffset;
					GetTextViewByControlID(diffTextViewControlID).NVI_ScrollTo(diffOrigin,true,false);
				}
				//Diff Index�w�胂�[�h����
				else if( inCurrentDiffIndex != kIndex_Invalid )
				{
					//3.���݂̃L�����b�g�ʒu�ɑΉ�����Diff�p�[�g��\���ł���悤��Diff���̃X�N���[������
					AIndex	diffStartLineIndex = GetApp().SPI_GetTextDocumentByID(diffTargetDocID).
							SPI_GetDiffStartLineIndexByDiffIndex(inCurrentDiffIndex);
					AIndex	diffEndLineIndex = GetApp().SPI_GetTextDocumentByID(diffTargetDocID).
							SPI_GetDiffEndLineIndexByDiffIndex(inCurrentDiffIndex);
					GetTextViewByControlID(diffTextViewControlID).
							SPI_AdjustScroll_Center(diffStartLineIndex,diffEndLineIndex,workingViewOrigin.x);
				}
				else 
				{
					//Y��������
					if( diffViewStartParaIndex != kIndex_Invalid )
					{
						//1. ���E�����s���Ή�����悤��originImagePt���v�Z
						
						AImagePoint	originImagePt1 = {0,0};
						//�s�ԍ��擾
						AIndex	diffViewStartLineIndex = GetApp().SPI_GetTextDocumentByID(diffTargetDocID).
								SPI_GetLineIndexByParagraphIndex(diffViewStartParaIndex);
						//
						ALocalPoint	lpt = {0,0};
						AWindowPoint	wpt = {0,0};
						NVI_GetWindowPointFromControlLocalPoint(inWorkingViewControlID,lpt,wpt);
						//
						ALocalPoint	difflpt = {0,0};
						AWindowPoint	diffwpt = {0,0};
						NVI_GetWindowPointFromControlLocalPoint(diffTextViewControlID,difflpt,diffwpt);
						//
						originImagePt1.y = GetTextViewByControlID(diffTextViewControlID).
								GetImageYByLineIndex(diffViewStartLineIndex) + diffwpt.y-wpt.y;
						if( originImagePt1.y < 0 )   originImagePt1.y =0;
						
						//2. �E���ŏ���Diff�ɑΉ����鍶���s��top�ɕ\�������悤��originImagePt���v�Z
						
						//�E���ŏ���DiffIndex���擾���A�E���ŏ���Diff������΁A�����̍s�ԍ����擾
						AIndex	rightFirstDiffIndex = kIndex_Invalid;
						AIndex	leftFirstDiffLineIndex = kIndex_Invalid;
						for( AIndex paraIndex = viewStartParagraphIndex; paraIndex < viewEndParagraphIndex; paraIndex++ )
						{
							rightFirstDiffIndex = GetApp().SPI_GetTextDocumentByID(inWorkingDocID).
									SPI_GetDiffIndexByStartParagraphIndex(paraIndex);
							if( rightFirstDiffIndex != kIndex_Invalid )
							{
								leftFirstDiffLineIndex = GetApp().SPI_GetTextDocumentByID(diffTargetDocID).
										SPI_GetDiffStartLineIndexByDiffIndex(rightFirstDiffIndex);
								break;
							}
						}
						//
						AImagePoint	originImagePt2 = {0,0};
						if( leftFirstDiffLineIndex != kIndex_Invalid )
						{
							leftFirstDiffLineIndex -= 3;
							if( leftFirstDiffLineIndex < 0 )   leftFirstDiffLineIndex = 0;
							originImagePt2.y = GetTextViewByControlID(diffTextViewControlID).
									GetImageYByLineIndex(leftFirstDiffLineIndex);
						}
						
						//2<1�ɂȂ�����A2���̗p�B����ȊO��1���̗p�B
						if( leftFirstDiffLineIndex != kIndex_Invalid && originImagePt2.y < originImagePt1.y )
						{
							GetTextViewByControlID(diffTextViewControlID).NVI_ScrollTo(originImagePt2,true,false);
						}
						else
						{
							GetTextViewByControlID(diffTextViewControlID).NVI_ScrollTo(originImagePt1,true,false);
						}
					}
					//X��������
					AImagePoint	diffViewOrigin = {0,0};
					GetTextViewByControlID(diffTextViewControlID).NVI_GetOriginOfLocalFrame(diffViewOrigin);
					diffViewOrigin.x = workingViewOrigin.x;
					//�X�N���[�����s
					GetTextViewByControlID(diffTextViewControlID).NVI_ScrollTo(diffViewOrigin,true,false);
				}
			}
			//Diff�\���X�V
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
#if IMPLEMENTATION_FOR_WINDOWS
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_UpdateWindow();
#endif
		}
		//#611 diffInfoWindow�̕\���ƁA�{�E�C���h�E�̕\���̃^�C�~���O�������ɂ������̑΍�
		NVI_UpdateWindow();
	}
}

/**
�iDiff���j�i��index����AWindowPoint���擾
*/
void	AWindow_Text::SPI_GetDiffViewWindowPointByParagraph( const ADocumentID inDocID, const AIndex inDiffParagraphStart,
		AWindowPoint& outWindowPoint ) const
{
	outWindowPoint.x = 0;
	outWindowPoint.y = 0;
	//Diff�Ώۃh�L�������g�E�^�u�E�r���[�擾
	ADocumentID	diffTargetDocID = GetApp().SPI_GetTextDocumentByID(inDocID).SPI_GetDiffTargetDocumentID();
	AIndex	diffTargetDocTabIndex = NVI_GetTabIndexByDocumentID(diffTargetDocID);
	AControlID	diffTextViewControlID = mTabDataArray.GetObjectConst(diffTargetDocTabIndex).GetSubTextControlID();
	if( diffTextViewControlID != kControlID_Invalid )
	{
		if( GetTextViewConstByControlID(diffTextViewControlID).NVI_IsVisible() == true )
		{
			//�iDiff���j�i��index����AWindowPoint���擾
			AIndex	lineIndex = GetApp().SPI_GetTextDocumentConstByID(diffTargetDocID).SPI_GetLineIndexByParagraphIndex(inDiffParagraphStart);
			ALocalPoint	lpt = {0,0};
			lpt.y = GetTextViewConstByControlID(diffTextViewControlID).SPI_GetLocalYByLineIndex(lineIndex);
			NVI_GetWindowPointFromControlLocalPoint(diffTextViewControlID,lpt,outWindowPoint);
		}
	}
}

/**
Working�h�L�������g��TabIndex����Repository�h�L�������g��TabIndex���擾
*/
AIndex	AWindow_Text::GetDiffDocumentTabIndexFromWorkingTabIndex( const AIndex inTabIndex )
{
	ADocumentID	workDocID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	ADocumentID	diffTargetDocID = GetApp().SPI_GetTextDocumentByID(workDocID).SPI_GetDiffTargetDocumentID();
	if( diffTargetDocID == kObjectID_Invalid )   return kIndex_Invalid;
	return NVI_GetTabIndexByDocumentID(diffTargetDocID);
}

/**
Diff�\�����[�h�ݒ�
*/
void	AWindow_Text::SPI_SetDiffDisplayMode( const ABool inDiffDisplayMode )
{
	if( GetApp().SPI_IsSupportPaneMode() == false )   return;//���T�|�[�g�Ȃ烊�^�[��
	
	if( mDiffDisplayMode != inDiffDisplayMode )
	{
		mDiffDisplayMode = inDiffDisplayMode;
		NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetToogleOn(inDiffDisplayMode);
		//
		//#899 SetSubPaneMode(kSubPaneMode_Manual);
		if( inDiffDisplayMode ==true )
		{
			//==================�\��==================
			
			//�T�u�e�L�X�g�\��
			ShowHideSubTextColumnCore(true);
			//Diff���T�u�e�L�X�g�ɕ\������
			SPI_UpdateDiffDisplay(NVI_GetCurrentDocumentID());
		}
		else
		{
			//==================��\��==================
			
			//�T�u�e�L�X�g�\�����̏ꍇ�́A�����ɒʏ�T�u�y�C����\������
			if( mSubTextColumnDisplayed == true )
			{
				//�Ō�ɕ\������Ă����h�L�������g���T�u�e�L�X�g�ɕ\������
				AIndex	tabIndex = NVI_GetTabIndexByDocumentID(mLastNormalSubTextDocumentID);
				if( tabIndex == kIndex_Invalid )
				{
					tabIndex = NVI_GetCurrentTabIndex();
				}
				//�ʏ�T�u�y�C���\��
				SPI_DisplayTabInSubText(tabIndex,false);
			}
		}
	}
	//Diff�{�^���e�L�X�g�X�V
	SPI_UpdateDiffButtonText();
	//Diff�\���X�V
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
	//
	NVI_RefreshWindow();
}

/**
Diff�{�^���e�L�X�g�X�V
*/
void	AWindow_Text::SPI_UpdateDiffButtonText()
{
	if( mDiffDisplayMode == false )
	{
		AText	t;
		t.SetLocalizedText("SubPaneDiffButton");
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(NVI_GetCurrentTabIndex())).NVI_IsDirty() == false )
		{
			//�������e�L�X�g��ǉ�
			t.AddCstring(" (");
			t.AddNumber(GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(NVI_GetCurrentTabIndex())).SPI_GetDiffCount());
			t.AddCstring(")");
		}
		//
		NVI_SetControlText(kSubPaneDiffButtonControlID,t);
	}
	else
	{
		AText	t;
		GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(NVI_GetCurrentTabIndex())).
				SPI_GetDiffStatusText(t);
		NVI_SetControlText(kSubPaneDiffButtonControlID,t);
	}
	//��r�h�L�������g�L���Ń{�^����Enable����
	if( NVI_GetCurrentDocumentID() != kObjectID_Invalid )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )
		{
			NVI_SetControlEnable(kSubPaneDiffButtonControlID,false);
		}
		else
		{
			NVI_SetControlEnable(kSubPaneDiffButtonControlID,true);
		}
	}
}

#pragma mark ===========================

#pragma mark ---�t�H�[�J�X����

//#137
/**
���݂�Focus��TextView���擾
�iFocus��TextView�ł͂Ȃ��Ƃ���Invalid��Ԃ��j
@note �D�揇�ʁF���݂̃t�H�[�J�Xtext view���Ō�̃t�H�[�J�Xtext view
*/
AControlID	AWindow_Text::GetTopMostFocusTextViewControlID() const
{
	AControlID	focus = NVI_GetCurrentFocus();
	if( IsTextViewControlID(focus) == true )
	{
		return focus;
	}
	else
	{
		if( NVI_GetCurrentTabIndex() == kIndex_Invalid )   return kControlID_Invalid;//#379
		//AControlID	lastFocusedTextView = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
		if( IsTextViewControlID(mLatentTextViewContolID) == true )//#212 ���݂��L�����ǂ����𔻒�
		{
			return mLatentTextViewContolID;
		}
		else //#212
		{
			return kControlID_Invalid;
		}
	}
}

//#137
/**
�w��TabIndex�̌��݂�Focus��TextView���擾
�iFocus��TextView�ł͂Ȃ��Ƃ���Invalid��Ԃ��j
@note �T�u�e�L�X�g�y�C�����܂߂āA���݂�focus��text view���擾�Btext view�ȊO��focus�Ȃ�invalid��Ԃ��B
*/
AControlID	AWindow_Text::GetTopMostFocusTextViewControlID( const AIndex inTabIndex ) const
{
	AControlID	focus = NVI_GetFocusInTab(inTabIndex);
	if( IsTextViewControlID(focus) == true )
	{
		return focus;
	}
	else
	{
		AControlID	lastFocusedTextView = mTabDataArray.GetObjectConst(inTabIndex).GetLatentTextViewControlID();
		if( IsTextViewControlID(lastFocusedTextView) == true )//#212 ���݂��L�����ǂ����𔻒�
		{
			return lastFocusedTextView;
		}
		else //#212
		{
			return kControlID_Invalid;
		}
	}
}

//#137
/**
*/
ABool	AWindow_Text::IsTextViewControlID( const AControlID inControlID ) const
{
	if( NVI_ExistView(inControlID) == false )   return false;//#212 View�����ۂɑ��݂��Ă��邩�ǂ����̃`�F�b�N
	if( inControlID == kControlID_Invalid )   return false;//#212
	
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		if( mTabDataArray.GetObjectConst(tabIndex).FindFromTextViewControlID(inControlID) != kIndex_Invalid )
		{
			return true;
		}
		//#212 �T�u�y�C����TextView�Ɣ�r
		if( mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID() == inControlID )
		{
			return true;
		}
	}
	return false;
}

//#137
/**
�t�H�[�J�X�ύX���ɃR�[�������

�����Ώ�TextView���L�����邽�߂Ɏg�p����
*/
void	AWindow_Text::EVTDO_DoViewFocusActivated( const AControlID inFocusControlID )
{
	//#688 ���݃^�u�ݒ�ς݃`�F�b�N
	if( NVI_GetCurrentTabIndex() == kIndex_Invalid )   return;
	//
	if( IsTextViewControlID(inFocusControlID) == true )
	{
		//�h�L�������g�A�^�u�擾
		ADocumentID	docID = GetTextViewByControlID(inFocusControlID).SPI_GetTextDocument().GetObjectID();
		AIndex	tabIndex = NVI_GetTabIndexByDocumentID(docID);
		//�^�u���Ƃ̃��C���e�L�X�g�̍Ō�̃t�H�[�J�X�Ƃ��ċL��
		if( SPI_IsSubTextView(inFocusControlID) == false )
		{
			mTabDataArray.GetObject(tabIndex).SetLatentMainTextViewControlID(inFocusControlID);
		}
		//�^�u���Ƃ̍Ō�̃t�H�[�J�X�Ƃ��ċL��
		mTabDataArray.GetObject(tabIndex).SetLatentTextViewControlID(inFocusControlID);
		//���̃E�C���h�E�̍Ō�̃t�H�[�J�X�Ƃ��ċL��
		mLatentTextViewContolID = inFocusControlID;
	}
}

#pragma mark ===========================

#pragma mark ---�R���g���[������

/**
�s�ԍ��{�^���̍X�V
*/
void	AWindow_Text::SPI_UpdateLineNumberButton( const AControlID inTextViewControlID )
{
	//�X�e�[�^�X�o�[��\���Ȃ牽�������I��
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
	{
		return;
	}
	
	//�w��control ID���e�L�X�g�r���[�łȂ���Ή��������I��
	if( IsTextViewControlID(inTextViewControlID) == false )
	{
		return;
	}
	
	if( SPI_IsSubTextView(inTextViewControlID) == false )
	{
		//==================���C���e�L�X�g�̏ꍇ==================
		ATextPoint	spt, ept;
		GetTextViewByControlID(inTextViewControlID).SPI_GetSelect(spt,ept);
		AText	text;
		if( GetApp().SPI_GetModePrefDB(SPI_GetTextDocument(inTextViewControlID).SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber_AsParagraph/*#844kDisplayLineNumberButton_AsParagraph*/) == true )
		{
			//text.SetLocalizedText("LineNumberButton_Paragraph");
			AText	numbertext;
			numbertext.SetFormattedCstring("%d/%d",
						SPI_GetTextDocument(inTextViewControlID).SPI_GetParagraphIndexByLineIndex(spt.y)+1,SPI_GetTextDocument(inTextViewControlID).SPI_GetParagraphCount());
			text.AddText(numbertext);
		}
		else
		{
			//text.SetLocalizedText("LineNumberButton_Line");
			AText	numbertext;
			numbertext.SetFormattedCstring("%d/%d",spt.y+1,SPI_GetTextDocument(inTextViewControlID).SPI_GetLineCount());
			text.AddText(numbertext);
		}
		//
		if( mCacheLineNumberButtonText.Compare(text) == false )
		{
			NVI_GetButtonViewByControlID(kControlID_LineNumberButton).NVI_SetText(text);
			mCacheLineNumberButtonText.SetText(text);
		}
	}
	//#725
	else if( mSubTextColumnDisplayed == true )
	{
		//==================�T�u�e�L�X�g�̏ꍇ==================
		//
		ATextPoint	spt, ept;
		GetTextViewByControlID(inTextViewControlID).SPI_GetSelect(spt,ept);
		AText	text;
		if( GetApp().SPI_GetModePrefDB(SPI_GetTextDocument(inTextViewControlID).SPI_GetModeIndex()).
					GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber_AsParagraph) == true )
		{
			//text.SetLocalizedText("LineNumberButton_Paragraph");
			AText	numbertext;
			numbertext.SetFormattedCstring("%d/%d",
						SPI_GetTextDocument(inTextViewControlID).SPI_GetParagraphIndexByLineIndex(spt.y)+1,
						SPI_GetTextDocument(inTextViewControlID).SPI_GetParagraphCount());
			text.AddText(numbertext);
		}
		else
		{
			//text.SetLocalizedText("LineNumberButton_Line");
			AText	numbertext;
			numbertext.SetFormattedCstring("%d/%d",spt.y+1,SPI_GetTextDocument(inTextViewControlID).SPI_GetLineCount());
			text.AddText(numbertext);
		}
		//
		if( mCacheLineNumberButtonText_SubText.Compare(text) == false )
		{
			NVI_GetButtonViewByControlID(kControlID_LineNumberButton_SubText).NVI_SetText(text);
			mCacheLineNumberButtonText_SubText.SetText(text);
		}
	}
}

//�ꏊ�{�^���̍X�V
void	AWindow_Text::UpdatePositionButton( const AControlID inTextViewControlID )
{
	//�X�e�[�^�X�o�[��\���Ȃ牽�������I��
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
	{
		return;
	}
	
	//�w��control ID���e�L�X�g�r���[�łȂ���Ή��������I��
	if( IsTextViewControlID(inTextViewControlID) == false )
	{
		return;
	}
	
	//�\���e�L�X�g��ύX�������ǂ����̃t���O
	ABool	textchanged = false;
	//
	if( SPI_IsSubTextView(inTextViewControlID) == false )
	{
		//==================���C���e�L�X�g�̏ꍇ==================
		//
		ATextPoint	spt, ept;
		GetTextViewByControlID(inTextViewControlID).SPI_GetSelect(spt,ept);
		AIndex	menuItemIndex = SPI_GetTextDocument(inTextViewControlID).SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
		AText	text;
		if( menuItemIndex != kIndex_Invalid )
		{
			SPI_GetTextDocument(inTextViewControlID).SPI_GetJumpMenuTextByMenuItemIndex(menuItemIndex,text);
		}
		/*
		AText	t;
		t.SetLocalizedText("PositionButtonTitle");
		text.InsertText(0,t);
		*/
		//
		if( mCachePositionButtonText.Compare(text) == false )
		{
			NVI_GetButtonViewByControlID(kControlID_PositionButton).NVI_SetText(text);
			mCachePositionButtonText.SetText(text);
			//
			textchanged = true;
		}
	}
	//#725
	else if( mSubTextColumnDisplayed == true )
	{
		//==================�T�u�e�L�X�g�̏ꍇ==================
		//
		ATextPoint	spt, ept;
		GetTextViewByControlID(inTextViewControlID).SPI_GetSelect(spt,ept);
		AIndex	menuItemIndex = SPI_GetTextDocument(inTextViewControlID).SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
		AText	text;
		if( menuItemIndex != kIndex_Invalid )
		{
			SPI_GetTextDocument(inTextViewControlID).SPI_GetJumpMenuTextByMenuItemIndex(menuItemIndex,text);
		}
		/*
		AText	t;
		t.SetLocalizedText("PositionButtonTitle");
		text.InsertText(0,t);
		*/
		//
		if( mCachePositionButtonText_SubText.Compare(text) == false )
		{
			NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).NVI_SetText(text);
			mCachePositionButtonText_SubText.SetText(text);
			//
			textchanged = true;
		}
	}
	//�s�ԍ��E�ꏊ�{�^�����C�A�E�g�X�V
	if( textchanged == true && NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		ATextWindowLayoutData	layout = {0};
		CalculateLayout(NVI_GetCurrentTabIndex(),layout);
		UpdateLayout_LineNumberAndPositionButton(NVI_GetCurrentTabIndex(),layout);
	}
}

#pragma mark ===========================

#pragma mark ---�E�C���h�E�^�C�g���ݒ�^�擾

//
void	AWindow_Text::NVIDO_UpdateTitleBar()
{
	if( NVI_GetCurrentTabIndex() == kIndex_Invalid )   return;//#379
	
	//�^�C�g���e�L�X�g�X�V
	SPI_UpdateWindowTitleBarText();
	
	//�h�L�������gID�擾
	ADocumentID	docID = NVI_GetCurrentDocumentID();
	if( docID == kObjectID_Invalid )   return;
		
	//�t�@�C���ݒ�
	//�t�@�C���w�肳��A���A�X�N���[�j���O������̏ꍇ�̂݁A�E�C���h�E�t�@�C���ݒ肷��
	if( AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_IsFileSpecifiedAndScreened() == true )
	{
		AFileAcc	file;
		AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_GetFile(file);
		GetImp().SetWindowFile(file);
	}
	else
	{
		GetImp().ClearWindowFile();
	}
	NVI_SetModified(AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_IsDirty());
	
	//�^�C�g���o�[��Update����^�C�~���O�Ń^�u�Z���N�^��Update����B
	NVI_DrawControl(kControlID_TabSelector);
}

/**
*/
void	AWindow_Text::SPI_UpdateWindowTitleBarText()
{
	//�^�C�g���擾
	AText	title;
	//#688 �^�C�g����GetTitleText()�Ŏ擾 NVI_GetTitle(NVI_GetCurrentTabIndex(),title);
	
	//�h�L�������gID�擾
	ADocumentID	docID = NVI_GetCurrentDocumentID();
	if( docID == kObjectID_Invalid )   return;
	
	//�^�C�g���擾
	SPI_GetTitleText(docID,title);
	
	//�^�C�g���ݒ�
	GetImp().SetTitle(title);
	
	//�T�u�e�L�X�g�t�@�C�����{�^���e�L�X�g�X�V
	UpdateSubTextFileNameButtonText();
}

/**
�^�C�g���擾
*/
void	AWindow_Text::SPI_GetTitleText( const ADocumentID inDocumentID, AText& outText )
{
	//
	ADocument_Text&	textdoc = GetApp().SPI_GetTextDocumentByID(inDocumentID);
	//
#if IMPLEMENTATION_FOR_MACOSX
	if( textdoc.SPI_IsODBMode() == true )
	{
		//ODB���[�h�̏ꍇ�AODB�p�X���擾
		AText	odbcustompath;
		textdoc.SPI_GetODBCustomPath(odbcustompath);
		outText.AddText(odbcustompath);
	}
	else
#endif
	{
		//�t�@�C��specified�Ȃ�t�@�C���p�X���擾
		if( textdoc.NVI_IsFileSpecified() == true )
		{
			AFileAcc	file;
			textdoc.NVI_GetFile(file);
			AText	filepath;
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayFullPathTitleBar) == true )//#1182
			{
				GetApp().GetAppPref().GetFilePathForDisplay(file,filepath);//B0389
			}
			//#1182
			else
			{
				textdoc.NVI_GetTitle(filepath);
			}
			outText.AddText(filepath);
			
		}
		//�����[�g�t�@�C���̏ꍇ��URL #1247
		else if( textdoc.SPI_IsRemoteFileMode() == true )
		{
			textdoc.SPI_GetURL(outText);
		}
		//�t�@�C��not specified�Ȃ�f�t�H���g�^�C�g�����擾
		else
		{
			AText	title;
			textdoc.NVI_GetTitle(title);
			outText.AddText(title);
		}
	}
	
	if( mToolbarExistArray.Find(kControlID_Toolbar_Mode) == kIndex_Invalid )
	{
		//"-"��ǉ�
		outText.AddCstring(" - ");
		//���[�h�����擾
		AText	modename;
		textdoc.SPI_GetModeDisplayName(modename);
		outText.AddText(modename);
	}
	outText.AddCstring("  ");
	
	//�t�@�C���w�肳��A���A�X�N���[�j���O������̏ꍇ�̂݁A�t�@�C�����擾�A�\������
	if( textdoc.NVI_IsFileSpecifiedAndScreened() == true )
	{
		//�h�L�������g��read only���A�s�܂�Ԃ��X���b�h�v�Z���ɂ��read only�łȂ��ꍇ�A"�������ݕs��"�e�L�X�g�ǉ�
		if( textdoc.NVI_IsReadOnly() == true &&
		   textdoc.SPI_GetWrapCalculatorProgressPercent() == kIndex_Invalid )//#699
		{
			AText	t;
			t.SetLocalizedText("ReadOnly");
			outText.AddText(t);
		}
		
		//�ŏI�ύX����
		//"("
		outText.AddCstring(" (");
		//�ŏI�ύX����
		AText	datetime;
		ADateTimeWrapper::GetDateTimeText(textdoc.SPI_GetLastModifiedDateTime(),datetime);
		outText.AddText(datetime);
		//")"
		outText.AddCstring(") ");
		
		//SCM���
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
		{
			AFileAcc	file;
			textdoc.NVI_GetFile(file);
			AText	t;
			GetApp().SPI_GetSCMFolderManager().GetFileStateText(file,t);
			if( t.GetItemCount() > 0 )
			{
				outText.AddCstring("  ");
				outText.AddText(t);
			}
		}
	}
	
	//�s�܂�Ԃ��i���擾
	ANumber	wrapCalcPercent = textdoc.SPI_GetWrapCalculatorProgressPercent();
	if( wrapCalcPercent != kIndex_Invalid )//�s�܂�Ԃ��X���b�h�v�Z����kIndex_Invalid�ȊO�ɂȂ�
	{
		if( wrapCalcPercent != 0 )//�h�L�������g���J��������ɁA�u�܂�Ԃ��v�Z���v�u�������݋֎~�v�ǂ�����\�������Ȃ�����
		{
			AText	t;
			t.SetNumber(wrapCalcPercent);
			AText	wraptext;
			wraptext.SetLocalizedText("TitleText_WrapCalculating");
			wraptext.ReplaceParamText('0',t);
			//�^�C�g���e�L�X�g�ɒǉ�
			outText.InsertText(0,wraptext);
		}
	}
	
	//�i����
	if( textdoc.SPI_IsWrapCalculating() == false )
	{
		AText	paraText;
		paraText.SetLocalizedText("TitleText_Paragraph");
		paraText.ReplaceParamText('0',textdoc.SPI_GetParagraphCount());
		outText.AddText(paraText);
	}
}

#pragma mark ===========================

#pragma mark ---�W�����v���X�g

/**
�W�����v���X�g�\���E��\��
inShow��false�ł���΁A�t���[�e�B���O�W�����v���X�g�͍폜����B
�\����ԁ��t���[�e�B���O�W�����v���X�g�E�C���h�E�����ςݏ��
��\����ԁ��t���[�e�B���O�W�����v���X�g�E�C���h�E���������
*/
void	AWindow_Text::SPI_ShowHideJumpListWindow( const ABool inShow )
{
	//
	if( inShow == false )
	{
		//==================�\������ꍇ�F�t���[�e�B���O�W�����v���X�g���폜����==================
		if( mFloatingJumpListWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_DeleteSubWindow(mFloatingJumpListWindowID);
			mFloatingJumpListWindowID = kObjectID_Invalid;
		}
	}
	else
	{
		//==================��\���̏ꍇ�F�t���[�e�B���O�W�����v���X�g���������ł���΁A�E�C���h�E��������B==================
		if( mFloatingJumpListWindowID == kObjectID_Invalid )
		{
			mFloatingJumpListWindowID = GetApp().SPI_CreateSubWindow(kSubWindowType_JumpList,0,GetObjectID(),
																	 kSubWindowLocationType_Floating,kIndex_Invalid,false);
		}
	}
	
	//�W�����v���X�g�\���E�z�u�X�V
	UpdateFloatingJumpListDisplay();
	
	//�W�����v���X�g�̃^�u�I�� #1109
	GetApp().SPI_SelectJumpListWindowsTab(GetObjectID(),NVI_GetCurrentDocumentID());
	
	//�\���E��\���̏�Ԃ�app pref db�ɕۑ�
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kShowFloatingJumpList,inShow);
	
	/*#725
	//
	mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).SetJumpListVisible(
				GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_IsWindowVisible());
	*/
	//
	/*#725
	//Overlay�Ȃ牽�����Ȃ�
	if( SPI_GetJumpListWindow().NVI_GetOverlayMode() == true )   return;
	
	//
	if( SPI_GetJumpListWindow().NVI_IsWindowVisible() == true )
	{
		SPI_GetJumpListWindow().NVI_Hide();
	}
	else
	{
		SPI_GetJumpListWindow().NVI_Show();
		SPI_GetJumpListWindow().NVI_SelectTab(NVI_GetCurrentTabIndex());
	}
	//
	mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).SetJumpListVisible(SPI_GetJumpListWindow().NVI_IsWindowVisible());
	*/
}

/**
�W�����v���X�g�\���E�z�u�X�V
�W�����v���X�g�����ς݂ł���Ε\�����A�������I�t�Z�b�g�ʒu�ɕ\������B
*/
void	AWindow_Text::UpdateFloatingJumpListDisplay()
{
	if( mFloatingJumpListWindowID != kObjectID_Invalid )
	{
		//���̃t���[�e�B���O�W�����v���X�g�ȊO�̃W�����v���X�g���\���ɂ���
		GetApp().SPI_HideOtherFloatingJumpListWindows(mFloatingJumpListWindowID);
		//�W�����v���X�g�\��
		if( GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_IsWindowVisible() == false )
		{
			GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Show(false);
		}
		//�W�����v���X�g�ʒu�X�V
		GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).SPI_UpdatePosition();
		/*
		//�W�����v���X�g�^�u�I��
		if( NVI_GetCurrentDocumentID() != kObjectID_Invalid )
		{
			ADocumentID	docID = SPI_GetCurrentFocusTextDocument().GetObjectID();
			GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_SelectTabByDocumentID(docID);
		}
		*/
	}
}

//
void	AWindow_Text::SPI_DoJumpListSelected( const ADocumentID inDocumentID,
											 const AIndex inIndex, const AModifierKeys inModifierKeys )
{
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true )
	{
		SPI_SplitView(false);
	}
	
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_DoJumpListSelected(inIndex);
	}
}

/*#725
//
void	AWindow_Text::SPI_UpdatePropertyJumpList()
{
	//
	SPI_GetJumpListWindow().NVI_UpdateProperty();
}
*/

#pragma mark ===========================

#pragma mark ---�c�[���o�[

//�}�N���o�[���ڃT�C�Y
const ANumber	kWidth_AddMacroButton = 30;
const ANumber	kHeight_AddMacroButton = 20;
const ANumber	kMarginWidth_AddMacroButton = 3;

//
void	AWindow_Text::SPI_UpdateToolbarItemValue( const AObjectID inDocumentID )
{
	if( inDocumentID == NVI_GetCurrentDocumentID() )
	{
		UpdateToolbarItemValue(NVI_GetCurrentTabIndex());
	}
}

/**
�c�[���o�[�̒l���X�V
*/
void	AWindow_Text::UpdateToolbarItemValue( const AIndex inTabIndex )
{
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	//�e�L�X�g�G���R�[�f�B���O���j���[�X�V
	if( mToolbarExistArray.Find(kControlID_Toolbar_TextEncoding) != kIndex_Invalid )
	{
		AText	tecname;
		GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextEncoding(tecname);
		NVI_SetControlText(kControlID_Toolbar_TextEncoding,tecname);
	}
	//���s�R�[�h���j���[�X�V
	if( mToolbarExistArray.Find(kControlID_Toolbar_ReturnCode) != kIndex_Invalid )
	{
		NVI_SetControlNumber(kControlID_Toolbar_ReturnCode,GetApp().SPI_GetTextDocumentByID(docID).SPI_GetReturnCode());
	}
	//�s�܂�Ԃ����j���[�X�V
	if( mToolbarExistArray.Find(kControlID_Toolbar_WrapMode) != kIndex_Invalid )
	{
		ANumber	wrapMode = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetWrapMode();
		NVI_SetControlNumber(kControlID_Toolbar_WrapMode,wrapMode);//#1113
		/*
		switch(wrapMode)
		{
		  case 0:
		  case 1:
			{
				NVI_SetControlNumber(kControlID_Toolbar_WrapMode,wrapMode);
				break;
			}
		  case 2:
			{
				NVI_SetControlNumber(kControlID_Toolbar_WrapMode,wrapMode);
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		*/
	}
	//���[�h���j���[�X�V
	if( mToolbarExistArray.Find(kControlID_Toolbar_Mode) != kIndex_Invalid )
	{
		AText	modename;
		GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeRawName(modename);
		NVI_SetControlText(kControlID_Toolbar_Mode,modename);
	}
	//�ݒ�
	if( mToolbarExistArray.Find(kControlID_Toolbar_Pref) != kIndex_Invalid )
	{
		AText	modename;
		GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeRawName(modename);
		NVI_SetControlText(kControlID_Toolbar_Pref,modename);
	}
	//�|�b�v�A�b�v�֎~�g�O�����
	if( mToolbarExistArray.Find(kControlID_Toolbar_ProhibitPopup) != kIndex_Invalid )
	{
		NVI_SetControlBool(kControlID_Toolbar_ProhibitPopup,
						   GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup));
	}
	//�t���[�e�B���O�\���ƃO�����
	if( mToolbarExistArray.Find(kControlID_Toolbar_ShowHideFloatingWindows) != kIndex_Invalid )
	{
		NVI_SetControlBool(kControlID_Toolbar_ShowHideFloatingWindows,
						   !GetApp().SPI_GetProhibitFloatingSubWindow());
	}
	
	/*#724
	//�c�[���{�^���l�X�V
	AItemCount	toolButtonCount = mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonCount();
	for( AIndex i = 0; i < toolButtonCount; i++ )
	{
		AToolbarItemType	type = mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonType(i);
		switch(type)
		{
		  case kToolbarItemType_TextEncoding:
			{
				AText	tecname;
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextEncoding(tecname);
				//#232 NVI_SetControlText(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i),tecname);
				NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).NVI_SetText(tecname);
				break;
			}
		  case kToolbarItemType_ReturnCode:
			{
				NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).
						SPI_SetTextFromMenuIndex(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetReturnCode());
				break;
			}
		  case kToolbarItemType_WrapMode:
			{
				ANumber	wrapMode = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetWrapMode();
				//#232 NVI_SetControlNumber(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i),wrapMode);
				NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).SPI_SetTextFromMenuIndex(wrapMode);
				if( wrapMode == 1 )
				{
					AText	text;
					text.SetLocalizedText("BuiltinToolButtonTitle_WordWrap");
					//#232 NVI_SetControlText(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i),text,true);
					NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).NVI_SetText(text);
				}
				else if( wrapMode == 2 )
				{
					AText	text;
					text.SetLocalizedText("BuiltinToolButtonTitle_WordWrapWithLetterCount");
					AText	numtext;
					numtext.SetFormattedCstring("%d",GetApp().SPI_GetTextDocumentByID(docID).SPI_GetWrapLetterCount());
					text.ReplaceParamText('0',numtext);
					//#232 NVI_SetControlText(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i),text,true);
					NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).NVI_SetText(text);
				}
				break;
			}
		  case kToolbarItemType_Mode:
			{
				AText	modename;
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeName(modename);
				//#232 NVI_SetControlText(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i),modename);
				NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).NVI_SetText(modename);//#232
				break;
			}
		}
	}
	*/
}

//#688
/**
�c�[���o�[���ڒǉ��^�폜�������i�폜����inID=kControlID_Invalid)
*/
void	AWindow_Text::EVTDO_ToolbarItemAdded( const AControlID inID )
{
	switch(inID)
	{
	  case kControlID_Toolbar_TextEncoding:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_TextEncoding,kTextArrayMenuID_TextEncoding);
			break;
		}
	  case kControlID_Toolbar_ReturnCode:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_ReturnCode,kTextArrayMenuID_ReturnCode);
			break;
		}
	  case kControlID_Toolbar_WrapMode:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_WrapMode,kTextArrayMenuID_WrapMode);
			break;
		}
	  case kControlID_Toolbar_Mode:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_Mode,kTextArrayMenuID_EnabledMode);
			break;
		}
	  case kControlID_Toolbar_SubWindows:
		{
			NVI_SetControlMenu(kControlID_Toolbar_SubWindows,GetApp().NVI_GetMenuManager().GetContextMenu(kContextMenuID_SubWindows));
			break;
		}
	  case kControlID_Toolbar_NewDocument:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_NewDocument,kTextArrayMenuID_EnabledMode);
			break;
		}
	  case kControlID_Toolbar_Pref:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_Pref,kTextArrayMenuID_Pref);
			break;
		}
		/* #0 ���g�p�̂��߃R�����g�A�E�g
	  case kControlID_Toolbar_Diff:
		{
			NVI_SetControlMenu(kControlID_Toolbar_Diff,GetApp().NVI_GetMenuManager().GetContextMenu(kContextMenuID_Diff));
			break;
		}
		*/
	  case kControlID_Toolbar_FindHighlight:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_FindHighlight,kTextArrayMenuID_TextMarker);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//toolbar���ڑ��݃e�[�u���X�V
	UpdateToolbarExistArray();
	//�c�[���o�[�l�X�V
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		UpdateToolbarItemValue(NVI_GetCurrentTabIndex());
	}
}

/**
toolbar���ڑ��݃e�[�u���X�V
*/
void	AWindow_Text::UpdateToolbarExistArray()
{
	//�c�[���o�[���݂��邩�ǂ����������o�ϐ��ɋL���iNVI_ExistControl()�͎��Ԃ�������̂Łj
	mToolbarExistArray.DeleteAll();
	if( NVI_ExistControl(kControlID_Toolbar_TextEncoding) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_TextEncoding);
	}
	if( NVI_ExistControl(kControlID_Toolbar_ReturnCode) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_ReturnCode);
	}
	if( NVI_ExistControl(kControlID_Toolbar_WrapMode) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_WrapMode);
	}
	if( NVI_ExistControl(kControlID_Toolbar_Mode) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_Mode);
	}
	/*#0 ���g�p�̂��߃R�����g�A�E�g
	if( NVI_ExistControl(kControlID_Toolbar_Diff) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_Diff);
	}
	*/
	if( NVI_ExistControl(kControlID_Toolbar_SearchField) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_SearchField);
	}
	for( AControlID controlID = kControlID_Toolbar_ButtonStart; controlID <= kControlID_Toolbar_ButtonEnd; controlID++ )
	{
		if( NVI_ExistControl(controlID) == true )
		{
			mToolbarExistArray.Add(controlID);
		}
	}
}

//
void	AWindow_Text::SPI_RemakeToolButtonsAll( const AModeIndex inModeIndex )
{
	//B0312 �c�[���{�^���S�폜
	//�c�[���č\�����A�c�[���{�^���̃A�C�R����S�ēo�^�������Ă��炱���ɗ���BCreateToolButtons()���^�u���ƂɎ��s����ƁA
	//����SetIcon()�����s����̂����A���̂Ȃ���DrawControl()�����s�����Ƃ��ɁA�i�����炭�j�d�Ȃ��Ă���ʂ̃^�u�̃A�C�R����redraw�����悤�Ƃ��Ă��܂��B
	//���̂Ƃ��A�܂�CreateToolButtons()�𖢎��s�̃^�u�̃c�[���{�^���̃A�C�R���͂��łɓo�^�����Ȃ̂ŁA�����I�����������Ă����B
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			for( AIndex i = 0; i < mTabDataArray.GetObjectConst(tabIndex).GetToolButtonCount(); i++ )
			{
				NVM_UnregisterViewInTab(tabIndex,mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(i));
				NVI_DeleteControl(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(i));
			}
			mTabDataArray.GetObject(tabIndex).DeleteAllToolButtonData();
		}
	}
	//
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			CreateToolButtons(tabIndex);
			//#275
			UpdateViewBounds(tabIndex);
		}
	}
}

//�c�[���{�^������
void	AWindow_Text::CreateToolButtons( const AIndex inTabIndex )
{
	//�c�[���{�^���S�폜
	for( AIndex i = 0; i < mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonCount(); i++ )
	{
		NVM_UnregisterViewInTab(inTabIndex,mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i));
		NVI_DeleteControl(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i));
	}
	mTabDataArray.GetObject(inTabIndex).DeleteAllToolButtonData();
	//�����{�b�N�X�E�{�^���폜 #243
	/*#724
	if( mTabDataArray.GetObject(inTabIndex).GetSearchButtonControlID() != kControlID_Invalid )
	{
		NVI_DeleteControl(mTabDataArray.GetObject(inTabIndex).GetSearchButtonControlID());
		mTabDataArray.GetObject(inTabIndex).SetSearchButtonControlID(kControlID_Invalid);
	}
	if( mTabDataArray.GetObject(inTabIndex).GetSearchBoxControlID() != kControlID_Invalid )
	{
		NVI_DeleteControl(mTabDataArray.GetObject(inTabIndex).GetSearchBoxControlID());
		mTabDataArray.GetObject(inTabIndex).SetSearchBoxControlID(kControlID_Invalid);
	}
	*/
	//
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	//#232 �{�^���̈ʒu���L���iToolBar�ł�Drag&Drop�p�j
	mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().DeleteAll();
	//
	ANumber	toolButtonX = 4;
	AWindowPoint	pt;
	AControlID	toolbarControlID = mTabDataArray.GetObjectConst(inTabIndex).GetToolbarControlID();//#246 
	NVI_GetControlPosition(/*#246 kControlID_ToolbarPlacard*/toolbarControlID,pt);
	ANumber	y = pt.y;
	//�u�}�N���ǉ��v�{�^��
	{
		AControlID	toolButtonControlID = NVM_AssignDynamicControlID();
		AWindowPoint	pt = {0,0};
		AFileAcc	file;
		mTabDataArray.GetObject(inTabIndex).AddToolButton(toolButtonControlID,kToolbarItemType_AddToolButton,file,kIndex_Invalid);
		NVI_CreateButtonView(toolButtonControlID,pt,kWidth_AddMacroButton,kHeight_MacroBarItem,toolbarControlID);
		NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	title;
		title.SetLocalizedText("AddMacro");
		NVI_GetButtonViewByControlID(toolButtonControlID).NVI_SetText(title);
		NVM_RegisterViewInTab(inTabIndex,toolButtonControlID);
	}
	//�u�}�N���F�v�{�^��
	{
		AControlID	toolButtonControlID = NVM_AssignDynamicControlID();
		AWindowPoint	pt = {0,0};
		AFileAcc	file;
		mTabDataArray.GetObject(inTabIndex).AddToolButton(toolButtonControlID,kToolbarItemType_MacroTitle,file,kIndex_Invalid);
		NVI_CreateButtonView(toolButtonControlID,pt,0,kHeight_MacroBarItem,toolbarControlID);
		NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetMenu(kContextMenuID_ToolBar,kMenuItemID_Invalid);
		NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	title;
		title.SetLocalizedText("MacroBarTitle");
		NVI_GetButtonViewByControlID(toolButtonControlID).NVI_SetText(title);
		NVM_RegisterViewInTab(inTabIndex,toolButtonControlID);
	}
	
	//========================�e�}�N���o�[���ږ��̃��[�v========================
	AItemCount	toolButtonCount = GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItemCount();
	for( AIndex toolButtonDBIndex = 0; toolButtonDBIndex < toolButtonCount; toolButtonDBIndex++ )
	{
		//------------------�c�[���o�[�f�[�^�̎擾------------------
		AToolbarItemType	type = kToolbarItemType_File;
		AFileAcc	file;
		AText	name;
		ABool	enabled = false;
		GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItem(toolButtonDBIndex,type,file,name,enabled);
		//enabled�łȂ��A�܂��́A�t�@�C���^�t�H���_���ڈȊO�̏ꍇ�́A�������Ȃ�
		if( enabled == false || (type != kToolbarItemType_File && type != kToolbarItemType_Folder) )
		{
			continue;
		}
		//
		AControlID	toolButtonControlID = NVM_AssignDynamicControlID();
		//�{�^���f�[�^�ǉ�
		mTabDataArray.GetObject(inTabIndex).AddToolButton(toolButtonControlID,type,file,toolButtonDBIndex);
		//�{�^������
		AWindowPoint	pt = {0};
		NVI_CreateButtonView(toolButtonControlID,pt,0,kHeight_MacroBarItem,toolbarControlID);
		//#724 �}�N���o�[��
		NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		//
		NVM_RegisterViewInTab(inTabIndex,toolButtonControlID);//#135 �ォ��ړ�
		NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetEnableDrag(true);//#232
		NVI_SetShowControl(toolButtonControlID,false);
	}
	//�}�N���{�^���e���ڃe�L�X�g�ݒ�
	UpdateMacroBarItemContent(inTabIndex);
}

//#724
/**
�}�N���{�^���e���ڃe�L�X�g�ݒ�
*/
void	AWindow_Text::UpdateMacroBarItemContent( const AIndex inTabIndex )
{
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	//
	AIndex	toolDisplayIndex = 2;
	AItemCount	toolButtonCount = GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItemCount();
	for( AIndex toolButtonDBIndex = 0; toolButtonDBIndex < toolButtonCount; toolButtonDBIndex++ )
	{
		//------------------�c�[���o�[�f�[�^�̎擾------------------
		AToolbarItemType	type = kToolbarItemType_File;
		AFileAcc	file;
		AText	name;
		ABool	enabled = false;
		GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItem(toolButtonDBIndex,type,file,name,enabled);
		//enabled�łȂ��A�܂��́A�t�@�C���^�t�H���_���ڈȊO�̏ꍇ�́A�������Ȃ�
		if( enabled == false || (type != kToolbarItemType_File && type != kToolbarItemType_Folder) )
		{
			continue;
		}
		//�}�N���o�[��control ID�擾
		AControlID	toolButtonControlID = mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(toolDisplayIndex);
		toolDisplayIndex++;
		switch( type )
		{
		  case kToolbarItemType_File:
			{
				//�E�N���b�N���j���[�ݒ�
				NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetContextMenu(kContextMenuID_ToolButton);
				NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetContextMenuEnableItem(0,true);
				//�^�C�g���擾
				AText	title;
				file.GetFilename(title);
				//�V���[�g�J�b�g�L�[�e�L�X�g�ǉ�
				if( false )//�����Ԃ�������̂łƂ肠�����V���[�g�J�b�g�L�[�\���͂��Ȃ�
				{
					if( toolButtonDBIndex >= 0 && toolButtonDBIndex < 9 )
					{
						AKeyBindAction	action = keyAction_macro1 + toolButtonDBIndex;
						AText	keyText;
						if( GetApp().SPI_GetModePrefDB(modeIndex).GetKeyTextFromKeybindAction(action,keyText) == true )
						{
							title.AddCstring(" (");
							title.AddText(keyText);
							title.AddCstring(")");
						}
					}
				}
				//�^�C�g���ݒ�
				NVI_GetButtonViewByControlID(toolButtonControlID).NVI_SetText(title);
				break;
			}
		  case kToolbarItemType_Folder:
			{
				//���j���[text��actiontext�𐶐����āAButton�ɐݒ�
				const ATextArray&	filenamearray = GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItemFilenameArray(toolButtonDBIndex);
				ATextArray	actiontextarray;
				for( AIndex j = 0; j < filenamearray.GetItemCount(); j++ )
				{
					AFileAcc	f;
					f.SpecifyChild(file,filenamearray.GetTextConst(j));
					AText	path;
					f.GetPath(path);
					actiontextarray.Add(path);
				}
				NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetMenuTextArray(filenamearray,actiontextarray,kMenuItemID_ToolBar_Folder);
				//�E�N���b�N���j���[
				NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetContextMenu(kContextMenuID_ToolButton);
				NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetContextMenuEnableItem(0,false);
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
}

/**
�}�N���{�^���e���ڃe�L�X�g�ݒ�
*/
void	AWindow_Text::SPI_UpdateMacroBarItemContent()
{
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		UpdateMacroBarItemContent(tabIndex);
	}
}

//#725
/**
�}�N���o�[�����v�Z
*/
ANumber	AWindow_Text::CalculateMacroBarHeight( const AIndex inTabIndex,
		const AWindowPoint pt_MacroBar, const ANumber w_MacroBar ) 
{
	//�e�}�N���o�[�̈ʒu���v�Z����tab�f�[�^�Ɋi�[
	UpdateMacroBarItemPositions(inTabIndex,w_MacroBar);
	//
	ANumber	y = kMacroBar_TopMargin;
	AItemCount	itemCount = mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().GetItemCount();
	//�ő��y�ʒu���擾����
	for( AIndex i = 0; i < itemCount; i++ )
	{
		ANumber	cy = mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().Get(i).y;
		if( cy > y )   y = cy;
	}
	return y+kHeight_MacroBarItem+kMacroBar_BottomMargin;
}

//#725
/**
�}�N���o�[�̔z�u
*/
void	AWindow_Text::UpdateLayout_MacroBar( const AIndex inTabIndex, 
		const AWindowPoint pt_MacroBar, const ANumber w_MacroBar, const ANumber h_MacroBar )
{
	//�e�}�N���o�[�̈ʒu���v�Z����tab�f�[�^�Ɋi�[
	UpdateMacroBarItemPositions(inTabIndex,w_MacroBar);
	
	//#675 UpdateViewBounds�}����Ԓ��͉������Ȃ�
	if( mSuppressUpdateViewBoundsCounter > 0 )   return;
	//���݃^�u�łȂ���Ή������Ȃ�
	if( NVI_GetCurrentTabIndex() != inTabIndex )
	{
		return;
	}
	//���[�hindex�擾
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	//�}�N���o�[�z�u
	AControlID	toolbarControlID = mTabDataArray.GetObjectConst(inTabIndex).GetToolbarControlID();
	if(  GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowMacroBar) == true &&
	   NVI_IsToolBarShown() == true )
	{
		//================�}�N���o�[�\����================
		
		//�}�N���o�[
		NVI_SetControlPosition(toolbarControlID,pt_MacroBar);
		NVI_SetControlSize(toolbarControlID,w_MacroBar,h_MacroBar);
		NVI_SetShowControlIfCurrentTab(inTabIndex,toolbarControlID,true);//#246
		//�e�}�N���z�u
		AItemCount	count = mTabDataArray.GetObject(inTabIndex).GetToolButtonCount();
		for( AIndex i = 0; i < count; i++ )
		{
			AIndex	index = i;
			//dragging����index���擾
			if( mDraggingMacroBarDisplayIndexArray.GetItemCount() > 0 )
			{
				index = mDraggingMacroBarDisplayIndexArray.Get(i);
			}
			//
			ALocalPoint	macroBarItemPoint = mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().Get(i);
			//�R���g���[��ID�擾
			AControlID	controlID = mTabDataArray.GetObject(inTabIndex).GetToolButtonControlID(index);
			//�ʒu�v�Z
			AWindowPoint	pt = { pt_MacroBar.x + macroBarItemPoint.x,
							pt_MacroBar.y + macroBarItemPoint.y };
			//�z�u
			NVI_SetControlPosition(controlID,pt);
			//�\��
			NVI_SetShowControlIfCurrentTab(inTabIndex,controlID,true);
		}
	}
	else
	{
		//================�}�N���o�[��\����================
		
		//�}�N���o�[�\���i����1�̃}�N���o�[��\������B�������\���̂��߁B�isetShowsBaselineSeparator���g���ƁA�Ȃ����}�E�X�z�o�[�ł��Ȃ��Ȃ邽�߁j�j
		NVI_SetControlPosition(toolbarControlID,pt_MacroBar);
		NVI_SetControlSize(toolbarControlID,w_MacroBar,h_MacroBar);
		NVI_SetShowControlIfCurrentTab(inTabIndex,toolbarControlID,true);//#246
		//�e�}�N����\��
		AItemCount	count = mTabDataArray.GetObject(inTabIndex).GetToolButtonCount();
		for( AIndex i = 0; i < count; i++ )
		{
			AControlID	controlID = mTabDataArray.GetObject(inTabIndex).GetToolButtonControlID(i);
			NVI_SetShowControlIfCurrentTab(inTabIndex,controlID,false);
		}
	}
}

//#724
/**
�e�}�N���o�[���ڂ̈ʒu���v�Z�Etab�f�[�^�ɕۑ�
*/
void	AWindow_Text::UpdateMacroBarItemPositions( const AIndex inTabIndex, const ANumber w_MacroBar )
{
	//���łɌv�Z�ς݂Ȃ牽�����Ȃ�
	if( mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().GetItemCount() > 0 )
	{
		return;
	}
	
	//�t�H���g�擾
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);
	//���[�hindex�擾
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	//�}�N���o�[�e���ڂ��Ƃ̃��[�v
	AControlID	toolbarControlID = mTabDataArray.GetObjectConst(inTabIndex).GetToolbarControlID();
	if(  GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowMacroBar) == true &&
		   NVI_IsToolBarShown() == true )
	{
		//=============�u�}�N���ǉ��v�{�^��=============
		ANumber	macroItemsWidth = w_MacroBar-kWidth_AddMacroButton-kMarginWidth_AddMacroButton*2;
		{
			ALocalPoint	toolButtonPt = {macroItemsWidth+kMarginWidth_AddMacroButton,kMacroBar_TopMargin};
			//
			mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().Add(toolButtonPt);
			//�R���g���[��ID�擾
			AControlID	controlID = mTabDataArray.GetObject(inTabIndex).GetToolButtonControlID(0);
			//�{�^���̃v���p�e�B�ݒ�
			AText	fontname;
			AFontWrapper::GetDialogDefaultFontName(fontname);
			NVI_GetButtonViewByControlID(controlID).SPI_SetTextProperty(fontname,10.0,kJustification_Center,
																		0/*#1079 kTextStyle_Bold|kTextStyle_DropShadow*/,
																		kColor_Black,kColor_Gray60Percent);//#1079
			NVI_GetButtonViewByControlID(controlID).SPI_SetDropShadowColor(kColor_Gray80Percent);
		}
		//
		AItemCount	count = mTabDataArray.GetObject(inTabIndex).GetToolButtonCount();
		//=============�e�}�N���{�^���z�u=============
		ALocalPoint	toolButtonPt = {kMacroBar_LeftMargin,kMacroBar_TopMargin};
		for( AIndex i = 1; i < count; i++ )
		{
			AIndex	index = i;
			//dragging����index���擾
			if( mDraggingMacroBarDisplayIndexArray.GetItemCount() > 0 )
			{
				index = mDraggingMacroBarDisplayIndexArray.Get(i);
			}
			//�R���g���[��ID�擾
			AControlID	controlID = mTabDataArray.GetObject(inTabIndex).GetToolButtonControlID(index);
			//�{�^���̃v���p�e�B�ݒ�
			AText	fontname;
			AFontWrapper::GetDialogDefaultFontName(fontname);
			NVI_GetButtonViewByControlID(controlID).SPI_SetTextProperty(fontname,10.0,kJustification_Center,
																		0/*#1079 kTextStyle_Bold|kTextStyle_DropShadow*/,
																		kColor_Black,kColor_Gray60Percent);//#1079
			NVI_GetButtonViewByControlID(controlID).SPI_SetDropShadowColor(kColor_Gray80Percent);
			//dragging���Ŕ�\���̏ꍇ�́Atemporary�Ŕ�\���ɂ���
			NVI_GetButtonViewByControlID(controlID).SPI_SetTemporaryInvisible(i==mDraggingMacroBarCurrentItemDisplayIndex);
			//�\�����擾
			ANumber	width = NVI_GetControlWidth(controlID);
			//�\��pt�擾
			if( toolButtonPt.x + width + kMacroBar_RightMargin >= macroItemsWidth )
			{
				toolButtonPt.x = kMacroBar_LeftMargin;
				toolButtonPt.y += kHeight_MacroBarItem + kMarginHeight_MacroBarItem;
			}
			//tab�f�[�^�ɒǉ�
			mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().Add(toolButtonPt);
			//����X
			toolButtonPt.x += width;
		}
		//#232 ���݂̃��[�hIndex����ToolBar�ɐݒ�
		GetToolbarView(inTabIndex).SPI_SetInfo(modeIndex);
	}
}

//#724
/**
�}�N���o�[���ڂ�inPoint���猟��
*/
AIndex	AWindow_Text::FindMacroBarIndex( const AWindowPoint& inPoint ) const
{
	//�������r���[
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	//�}�N���o�[���擾
	AItemCount	count = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonCount();
	//==================drag��==================
	if( mDraggingMacroBarCurrentItemDisplayIndex != kIndex_Invalid )
	{
		//drag�����ڂ�index���擾
		AIndex	currentIndex = mDraggingMacroBarDisplayIndexArray.Get(mDraggingMacroBarCurrentItemDisplayIndex);
		ANumber	w_current = NVI_GetControlWidth(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(currentIndex));
		//
		AIndex	prevDisplayIndex = mDraggingMacroBarCurrentItemDisplayIndex-1;
		if( prevDisplayIndex >= 0 && prevDisplayIndex < count )
		{
			AIndex	previndex = prevDisplayIndex;
			if( mDraggingMacroBarDisplayIndexArray.GetItemCount() > 0 )
			{
				previndex = mDraggingMacroBarDisplayIndexArray.Get(prevDisplayIndex);
			}
			//
			AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(previndex);
			//
			AWindowPoint	controlPoint = {0};
			NVI_GetControlPosition(controlID,controlPoint);
			ANumber	w = NVI_GetControlWidth(controlID);
			ANumber	h = NVI_GetControlHeight(controlID);
			if( w_current < w )
			{
				if( inPoint.x >= controlPoint.x + w_current && inPoint.x <= controlPoint.x + w &&
							inPoint.y >= controlPoint.y && inPoint.y <= controlPoint.y + h )
				{
					return mDraggingMacroBarCurrentItemDisplayIndex;
				}
			}
		}
		//
		AIndex	nextDisplayIndex = mDraggingMacroBarCurrentItemDisplayIndex+1;
		if( nextDisplayIndex >= 0 && nextDisplayIndex < count )
		{
			AIndex	nextindex = nextDisplayIndex;
			if( mDraggingMacroBarDisplayIndexArray.GetItemCount() > 0 )
			{
				nextindex = mDraggingMacroBarDisplayIndexArray.Get(nextDisplayIndex);
			}
			//
			AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(nextindex);
			//
			AWindowPoint	controlPoint = {0};
			NVI_GetControlPosition(controlID,controlPoint);
			ANumber	w = NVI_GetControlWidth(controlID);
			ANumber	h = NVI_GetControlHeight(controlID);
			if( w_current < w )
			{
				if( inPoint.x >= controlPoint.x + w_current && inPoint.x <= controlPoint.x + w &&
							inPoint.y >= controlPoint.y && inPoint.y <= controlPoint.y + h )
				{
					return mDraggingMacroBarCurrentItemDisplayIndex;
				}
			}
		}
	}
	//
	for( AIndex i = kFixMacroItemCount; i < count; i++ )
	{
		//
		AIndex	index = i;
		if( mDraggingMacroBarDisplayIndexArray.GetItemCount() > 0 )
		{
			index = mDraggingMacroBarDisplayIndexArray.Get(i);
		}
		//�R���g���[��ID�擾
		AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(index);
		//
		AWindowPoint	controlPoint = {0};
		NVI_GetControlPosition(controlID,controlPoint);
		ANumber	w = NVI_GetControlWidth(controlID);
		ANumber	h = NVI_GetControlHeight(controlID);
		if( inPoint.x >= controlPoint.x && inPoint.x <= controlPoint.x + w &&
					inPoint.y >= controlPoint.y && inPoint.y <= controlPoint.y + h )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

//#724
/**
*/
void	AWindow_Text::SPI_MacroBarItemDragging( const AViewID inButtonViewID, const AWindowPoint& inDropPoint )
{
	//�������r���[
	
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	AControlID	controlID = GetApp().NVI_GetViewByID(inButtonViewID).NVI_GetControlID();
	AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(controlID);
	if( toolButtonIndex == kIndex_Invalid )   return;
	
	AIndex	newIndex = FindMacroBarIndex(inDropPoint);
	if( newIndex == kIndex_Invalid )
	{
		return;
	}
	//
	if( mDraggingMacroBarCurrentItemDisplayIndex == kIndex_Invalid )
	{
		//Drag�ŏ��̃R�[����
		mDraggingMacroBarCurrentItemDisplayIndex = toolButtonIndex;
		//
		mDraggingMacroBarDisplayIndexArray.DeleteAll();
		AItemCount	count = mTabDataArray.GetObject(tabIndex).GetToolButtonCount();
		for( AIndex i = 0; i < count; i++ )
		{
			mDraggingMacroBarDisplayIndexArray.Add(i);
		}
	}
	else
	{
		//Drag2��ڈȍ~�̃R�[����
		
		//
		mDraggingMacroBarDisplayIndexArray.Move(mDraggingMacroBarCurrentItemDisplayIndex,newIndex);
		//
		mDraggingMacroBarCurrentItemDisplayIndex = newIndex;
		//
		mTabDataArray.GetObject(tabIndex).GetToolButtonPositionArray().DeleteAll();
		//
		ATextWindowLayoutData	layout = {0};
		CalculateLayout(tabIndex,layout);
		UpdateMacroBarItemPositions(tabIndex,layout.w_MacroBar);
		UpdateLayout_MacroBar(tabIndex,layout.pt_MacroBar,layout.w_MacroBar,layout.h_MacroBar);
	}
}

//#724
/**
*/
void	AWindow_Text::SPI_MacroBarItemStopDragging()
{
	//�������r���[
	
	mDraggingMacroBarCurrentItemDisplayIndex = kIndex_Invalid;
	mDraggingMacroBarDisplayIndexArray.DeleteAll();
	//
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	//
	mTabDataArray.GetObject(tabIndex).GetToolButtonPositionArray().DeleteAll();
	//
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(tabIndex,layout);
	UpdateMacroBarItemPositions(tabIndex,layout.w_MacroBar);
	UpdateLayout_MacroBar(tabIndex,layout.pt_MacroBar,layout.w_MacroBar,layout.h_MacroBar);
}

//#232
/**
�c�[���{�^��Drop�ɂ��{�^�������ύX
*/
void	AWindow_Text::SPI_MacroBarItemDrop( const AViewID inButtonViewID, const AWindowPoint& inDropPoint )
{
	//�������r���[
	
	AIndex	newIndex = FindMacroBarIndex(inDropPoint);
	//
	SPI_MacroBarItemStopDragging();
	//
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	AControlID	controlID = GetApp().NVI_GetViewByID(inButtonViewID).NVI_GetControlID();
	AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(controlID);
	if( toolButtonIndex == kIndex_Invalid )   return;
	
	if( newIndex == kIndex_Invalid )
	{
		return;
	}
	//
	if( newIndex == toolButtonIndex )   return;
	//
	AIndex	oldDBIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(toolButtonIndex);//#277 #724
	AIndex	newDBIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(newIndex);//#277 #724
	GetApp().SPI_GetModePrefDB(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex()).MoveToolbarItem(oldDBIndex,newDBIndex);//#277
}

AView_Toolbar&		AWindow_Text::GetToolbarView( const AIndex inTabIndex )
{
	AControlID	toolbarControlID = mTabDataArray.GetObjectConst(inTabIndex).GetToolbarControlID();//#246
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Toolbar,kViewType_Toolbar,toolbarControlID);//#246 kControlID_ToolbarPlacard);
}

#if 0
#pragma mark ===========================

#pragma mark ---�������ʕ\���y�C��

/**
*/
ADocumentID	AWindow_Text::SPI_GetOrCreateFindResultDocument()
{
	if( mFindResultDocumentID == kObjectID_Invalid )
	{
		CreateFindResultDocument();
		SPI_UpdateViewBounds();
	}
	return mFindResultDocumentID;
}

/**
*/
void	AWindow_Text::CreateFindResultDocument()
{
	//Height�ݒ�
	mFindResultHeight = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFindResultViewHeight);
	
	//Document����
	AIndexWindowDocumentFactory	docfactory(this);
	mFindResultDocumentID = GetApp().NVI_CreateDocument(docfactory);
	
	//View����
	AControlID	findResultViewControlID = NVM_AssignDynamicControlID();//#271
	AIndexViewFactory	indexViewFactory(GetObjectID(),/*#271kControlID_Invalid*/findResultViewControlID,mFindResultDocumentID);
	AWindowPoint	pt = {0,0};
	//#271 mFindResultViewID = NVI_CreateView(indexViewFactory,pt,10,10,true);
	mFindResultViewID = NVI_CreateView(findResultViewControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false/*312 true*/,indexViewFactory);
	GetApp().NVI_GetViewByID(mFindResultViewID).NVI_SetBorrowerView(true);
	NVI_EnableHelpTagCallback(findResultViewControlID);//#541
	
	//FindResultSeparator����
	AControlID	findResultSeparatorControlID = NVM_AssignDynamicControlID();//#271
	AViewDefaultFactory<AView_FindResultSeparator>	separatorfactory(GetObjectID(),/*#271kControlID_Invalid*/findResultSeparatorControlID);
	//#271 mFindResultSeparatorViewID = NVI_CreateView(separatorfactory,pt,10,10,true);
	mFindResultSeparatorViewID = NVI_CreateView(findResultSeparatorControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,true,separatorfactory);
	
	//VScrollBar����
	mFindResultVScrollBarControlID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(mFindResultVScrollBarControlID,pt,10,20);
	
	//����{�^��
	/*win
	NVI_CreateButtonView(mFindResultCloseButtonControlID,pt,10,10);
	NVI_GetButtonViewByControlID(mFindResultCloseButtonControlID).SPI_SetIcon(kIconID_Close,0,0,16,16);
	*/
	/*#725
	AWindowDefaultFactory<AWindow_ButtonOverlay>	findResultCloseButtonWindowFactory;
	mFindResultCloseButtonWindowID = GetApp().NVI_CreateWindow(findResultCloseButtonWindowFactory);
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).NVI_ChangeToOverlay(GetObjectID(),false);//win
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).NVI_Create(kObjectID_Invalid);
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).NVI_Show(false);
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).SPI_SetTargetWindowID(GetObjectID(),kVirtualControlID_FindResultCloseButon);
	//#725 NVI_GetButtonWindow(mFindResultCloseButtonWindowID).SPI_SetIcon(kIconID_Mi_Close,0,0,16,16);
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),pt,16,16);
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).SPI_SetButtonViewType(kButtonViewType_Rect16);
	*/
	//ScrollBar��View�Ƀ����N
	GetApp().NVI_GetViewByID(mFindResultViewID).NVI_SetScrollBarControlID(kControlID_Invalid,mFindResultVScrollBarControlID);
}

/**

*/
void	AWindow_Text::DeleteFindResultDocument()
{
	//����{�^���폜
	/*win
	NVI_DeleteControl(mFindResultCloseButtonControlID);
	mFindResultCloseButtonControlID = kControlID_Invalid;
	*/
	/*#725
	GetApp().NVI_DeleteWindow(mFindResultCloseButtonWindowID);//win
	mFindResultCloseButtonWindowID = kObjectID_Invalid;//win
	*/
	//VScrollBar�폜
	NVI_DeleteControl(mFindResultVScrollBarControlID);
	mFindResultVScrollBarControlID = kControlID_Invalid;
	//FindResultSeparator�폜
	//win NVI_DeleteViewByViewID(mFindResultSeparatorViewID);
	NVI_DeleteControl(GetApp().NVI_GetViewByID(mFindResultSeparatorViewID).NVI_GetControlID());//win
	mFindResultSeparatorViewID = kObjectID_Invalid;
	//View�폜
	//win NVI_DeleteViewByViewID(mFindResultViewID);
	NVI_DeleteControl(GetApp().NVI_GetViewByID(mFindResultViewID).NVI_GetControlID());//win
	mFindResultViewID = kObjectID_Invalid;
	/*#379
	//Document�폜
	GetApp().NVI_DeleteDocument(mFindResultDocumentID);
	*/
	//#725 mFindResultDocumentID = kObjectID_Invalid;
	
	//
	SPI_UpdateViewBounds();
}

/**

*/
ANumber	AWindow_Text::SPI_GetFindResultHeight() const
{
	return mFindResultHeight;
}

/**
��������View�̍�������
*/
void	AWindow_Text::SPI_SetFindResultHeight( const ANumber inHeight )
{
	if( inHeight <= 0 )
	{
		DeleteFindResultDocument();
	}
	else
	{
		mFindResultHeight = inHeight;
		GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kFindResultViewHeight,inHeight);
	}
	SPI_UpdateViewBounds();
}
#endif

#pragma mark ===========================

#pragma mark <���L�N���X(AWindow_AddToolButton)>

#pragma mark ===========================

/**
�}�N���o�[���ڒǉ��E�C���h�E�\���i�I��͈̓e�L�X�g���}�N���ɒǉ��j
*/
void	AWindow_Text::SPI_ShowAddToolButtonWindow()
{
	//�^�C�g���擾
	AText	title, text;
	AIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
	title.SetLocalizedText("MacroDefaultName");
	AText	t;
	ADateTimeWrapper::Get6LettersDateText(t);
	title.AddText(t);
	GetApp().SPI_GetModePrefDB(modeIndex).GetUniqToolbarName(title);
	//�I��͈̓e�L�X�g�ǉ�
	SPI_GetCurrentFocusTextView().SPI_GetSelectedText(text);
	//�}�N���o�[���ڒǉ��E�C���h�E�\��
	SPI_ShowAddToolButtonWindow(title,text,true);
}

/**
�}�N���o�[���ڒǉ��E�C���h�E�\��
*/
void	AWindow_Text::SPI_ShowAddToolButtonWindow( const AText& inTitle, const AText& inToolText, const ABool inEditWithTextWindow )
{
	//
	SPI_GetAddToolButtonWindow().NVI_CreateAndShow();
	SPI_GetAddToolButtonWindow().SPI_SetMode(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex(),inEditWithTextWindow,inTitle,inToolText);
	SPI_GetAddToolButtonWindow().NVI_SwitchFocusToFirst();
}

/**
�L�[�L�^����}�N���o�[���ڒǉ�
*/
void	AWindow_Text::SPI_ShowAddToolButtonWindowFromKeyRecord()
{
	//�^�C�g���擾
	AText	title, text;
	AIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
	title.SetLocalizedText("MacroDefaultName_KeyRecord");
	AText	t;
	ADateTimeWrapper::Get6LettersDateText(t);
	title.AddText(t);
	GetApp().SPI_GetModePrefDB(modeIndex).GetUniqToolbarName(title);
	//�L�[�L�^�擾
	GetApp().SPI_GetRecordedText(text);
	//�}�N���o�[���ڒǉ��E�C���h�E�\��
	SPI_ShowAddToolButtonWindow(title,text,false);
}

#pragma mark ===========================

#pragma mark ---

/*#199
void	AWindow_Text::SPI_EditPreProcess( const AObjectID inDocumentID, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(controlID).SPI_EditPreProcess(inTextIndex,inInsertedCount);
	}
}

void	AWindow_Text::SPI_EditPostProcess( const AObjectID inDocumentID, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(controlID).SPI_EditPostProcess(inTextIndex,inInsertedCount);
	}
}
*/

void	AWindow_Text::SPI_SplitView( const ABool inFocusToUpperView )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		AIndex	viewIndex = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).FindFromTextViewControlID(focus);
		if( viewIndex != kIndex_Invalid )//#212
		{
			SPI_SplitView(NVI_GetCurrentTabIndex(),viewIndex,inFocusToUpperView);
		}
	}
}

void	AWindow_Text::SPI_SplitView( const AIndex inTabIndex, const AIndex inViewIndex, const ABool inFocusToUpperView )
{
	//ViewHeight���擾����
	ANumber	originalViewHeight = mTabDataArray.GetObject(inTabIndex).GetViewHeight(inViewIndex);
	ANumber	newViewHeight = originalViewHeight/2;
	//�N���b�N�����{�^���ɑΉ�����View��Height��ݒ肷��
	mTabDataArray.GetObject(inTabIndex).SetViewHeight(inViewIndex,originalViewHeight-newViewHeight);
	//�N���b�N�����{�^���ɑΉ�����View�̃L�����b�g�A�I��͈͂��擾����
	ATextPoint	spt, ept;
	/*#699 ABool	caretMode = */
	GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(inViewIndex)).SPI_GetSelect(spt,ept);
	//�N���b�N�����{�^���ɑΉ�����View��LocalFrame�̃I�t�Z�b�g���擾����
	AImagePoint	originOfLocalFrame;
	GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(inViewIndex)).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
	//#450
	//���݂�view�̐܂肽���ݏ�Ԃ��擾
	ANumberArray	collapsedLinesArray;
	GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(inViewIndex)).
			SPI_GetFoldingCollapsedLines(collapsedLinesArray);
	//�V�K�r���[����
	CreateSplitView(inTabIndex,inViewIndex,newViewHeight,/*#699 caretMode,*/spt,ept,originOfLocalFrame,collapsedLinesArray);//#450
	//
	AControlID	upperTextViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(inViewIndex);
	AControlID	downerTextViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(inViewIndex+1);
	//�t�H�[�J�X��ݒ肷��
	if( inFocusToUpperView == true )
	{
		NVI_SwitchFocusTo(upperTextViewControlID);
	}
	else
	{
		NVI_SwitchFocusTo(downerTextViewControlID);
	}
	//AdjustScroll
	//GetTextViewByControlID(upperTextViewControlID).SPI_AdjustScroll_Center();
	GetTextViewByControlID(downerTextViewControlID).SPI_AdjustScroll_Center();
}

void	AWindow_Text::SPI_SwitchFocus_SplitView( const ABool inNext )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		AIndex	viewIndex = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).FindFromTextViewControlID(focus);
		if( viewIndex != kIndex_Invalid )
		{
			if( inNext == true )
			{
				viewIndex++;
			}
			else
			{
				viewIndex--;
			}
			if( viewIndex < 0 )   viewIndex = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewCount()-1;
			if( viewIndex >= mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewCount() )   viewIndex = 0;
			focus = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(viewIndex);
			NVI_SwitchFocusTo(focus);
		}
	}
}

ABool	AWindow_Text::SPI_IsCaretMode() const
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		return GetTextViewConstByControlID(focus).SPI_IsCaretMode();
	}
	return false;
}

//�s�ړ�
void	AWindow_Text::SPI_MoveToLine( const AIndex inLineIndex, const ABool inParagraphMode )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_MoveToLine(inLineIndex,inParagraphMode);
	}
}

void	AWindow_Text::SPI_ShowSaveWindow()
{
	//
	AText	filename;
	SPI_GetCurrentFocusTextDocument().NVI_GetTitle(filename);//#933 �Ώۂ͌��݂̃t�H�[�J�X�̃h�L�������g�ɕύX�i�ȉ����̊֐����e�ӏ��ɓK�p�j
	
	//B0000 �V�K�h�L�������g�̏ꍇ�͊g���q������
	if( SPI_GetCurrentFocusTextDocument().NVI_IsFileSpecified() == false )
	{
		AModeIndex	modeIndex = SPI_GetCurrentFocusTextDocument().SPI_GetModeIndex();
		/*#207
		if( GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kSuffix) > 0 )
		{
			AText	suffix;
			GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kSuffix,0,suffix);
			filename.AddText(suffix);
		}
		*/
		//�g���q�ݒ�̍ŏ��̍��ڂ��擾
		//#844 if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kUseDefaultSuffixForSave) == true )
		if( GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_TextArray(AModePrefDB::kSuffix) > 0 )//#844
		{
			AText	suffix;
			//#844 GetApp().SPI_GetModePrefDB(modeIndex).GetData_Text(AModePrefDB::kDefaultSuffixForSaveText,suffix);
			GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kSuffix,0,suffix);//#844
			filename.AddText(suffix);
		}
	}
	
	AFileAttribute	attr;
	SPI_GetCurrentFocusTextDocument().SPI_GetFileAttribute(attr);
	//�f�t�H���g�t�H���_�̐ݒ�
	AFileAcc	defaultfolder;
	//#844 if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kSaveAsDefaultFolderIsOriginalFileFolder) == true )
	{
		if( SPI_GetCurrentFocusTextDocument().NVI_IsFileSpecified() == true )
		{
			//------------------�����t�@�C�����R�s�[�ۑ��̏ꍇ------------------
			//���݂̃t�@�C���̐e�t�H���_
			AFileAcc	file;
			SPI_GetCurrentFocusTextDocument().NVI_GetFile(file);
			defaultfolder.SpecifyParent(file);
		}
		else
		{
			//------------------�V�K�h�L�������g��ۑ��̏ꍇ------------------
			//�Ō�ɊJ�����t�@�C���̐e�t�H���_
			defaultfolder.SpecifyParent(GetApp().SPI_GetLastOpenedTextFile());
		}
	}
	/*#844
	else//#429
	{
		//�Ō�ɊJ�����t�@�C���̐e�t�H���_
		defaultfolder.SpecifyParent(GetApp().SPI_GetLastOpenedTextFile());
	}
	*/
	AText	filter;//win 080709
	GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
	NVI_ShowSaveWindow(filename,SPI_GetCurrentFocusTextDocument().GetObjectID(),attr,filter,GetEmptyText(),defaultfolder);
}

//�h�L�������g��Save�O����(ADocument_Text��SPI_Save()����R�[�������)
//�E�C���h�E�ʒu��A�L�����b�g�ʒu���AWindow, View�I�u�W�F�N�g���ێ�����f�[�^���A�h�L�������g��PrefDB�֊i�[���鏈����
void	AWindow_Text::SPI_SavePreProcess( const AObjectID inDocumentID )
{
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_SavePreProcess();
	}
	ARect	bounds;
	//#850 NVI_GetWindowBounds(bounds);
	SPI_GetWindowMainColumnBounds(bounds);//#850
	APoint	pt = {-1,-1};
	if( NVI_GetSelectableTabCount() == 1 )//#850 �^�u�\���ɂȂ��Ă��Ȃ��Ƃ��̂݁A���݂̈ʒu��ۑ�����
	{
		pt.x = bounds.left;
		pt.y = bounds.top;
	}
	/*#850
	//#668 �T�u�y�C���̕����̓E�C���h�E�ʒu�E�T�C�Y�ɂ͊܂߂Ȃ�
	ANumber	width = bounds.right-bounds.left;
	if( mLeftSideBarDisplayed == true )
	{
		pt.x += mLeftSideBarWidth;
		width -= mLeftSideBarWidth;
	}
	*/
	//
	GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().SetData_Point(ADocPrefDB::kWindowPosition,pt);
	GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().
			SetData_Number(ADocPrefDB::kWindowWidth,bounds.right-bounds.left);//#850 width);//#668 bounds.right-bounds.left);
	GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().
			SetData_Number(ADocPrefDB::kWindowHeight,bounds.bottom-bounds.top);
	/*#850
	//#668
	if( SPI_IsTabModeMainWindow() == false && 
				GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSaveSubPaneModeIntoDocument) == true )
	{
		GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().
				SetData_Bool(ADocPrefDB::kDisplaySubPane,mLeftSideBarDisplayed);
		GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().
				SetData_Number(ADocPrefDB::kSubPaneWidth,mLeftSideBarWidth);
	}
	*/
}

//
void	AWindow_Text::SaveFTPAlias()
{
	AText	title;
	SPI_GetCurrentTabTextDocument().NVI_GetTitle(title);
	AText	filename;
	title.GetFilename(filename);
	filename.AddCstring(".miremote");//#1073
	AFileAttribute	attr;
	//win attr.creator = 'MMKE';
	//win attr.type = 'ftpa';
	GetApp().SPI_GetFTPAliasFileAttribute(attr);
	AText	refText;
	refText.SetCstring("FTPAlias");
	AFileAcc	defaultfolder;
	if( SPI_GetCurrentTabTextDocument().NVI_IsFileSpecified() == true )
	{
		AFileAcc	file;
		SPI_GetCurrentTabTextDocument().NVI_GetFile(file);
		defaultfolder.SpecifyParent(file);
	}
	else
	{
		defaultfolder.SpecifyParent(GetApp().SPI_GetLastOpenedTextFile());
	}
	AText	filter;//win 080709
	GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
	NVI_ShowSaveWindow(filename,NVI_GetCurrentDocumentID(),attr,filter,refText,defaultfolder);
}

/*#199
//
void	AWindow_Text::SPI_SelectText( const AObjectID inDocumentID, const AIndex inStartIndex, const AItemCount inLength )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	NVI_SelectTab(tabIndex);
	NVI_SelectWindow();
	AControlID	focus = NVI_GetFocusInTab(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_SelectText(inStartIndex,inLength);
	}
}
*/

//
void	AWindow_Text::SPI_GetSelectedText( const AObjectID inDocumentID, AText& outText ) const
{
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewConstByControlID(focus).SPI_GetSelectedText(outText);
	}
}

void	AWindow_Text::SPI_GetSelect( const AObjectID inDocumentID, AArray<ATextIndex>& outStart, AArray<ATextIndex>& outEnd ) const
{
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewConstByControlID(focus).SPI_GetSelect(outStart,outEnd);
	}
}

//B0000
void	AWindow_Text::SPI_GetSelect( const AObjectID inDocumentID, AIndex& outStartIndex, AIndex& outEndIndex ) const
{
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewConstByControlID(focus).SPI_GetSelect(outStartIndex,outEndIndex);
	}
}

void	AWindow_Text::SPI_SetSelect( const AObjectID inDocumentID, const AIndex inStart, const AIndex inEnd )
{
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_SetSelect(inStart,inEnd);
		//B0329
		GetTextViewByControlID(focus).SPI_AdjustScroll_Center();
	}
}

//Scroll�ʒu����
void	AWindow_Text::SPI_AdjustScroll_Center( const ADocumentID inDocumentID )
{
	//
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_AdjustScroll_Center();
	}
}

/**
TextView�̕��擾
@note �r���[�̎��ۂ�width�ݒ�O�ł�������View width��Ԃ��iUpdateViewBounds()�Ɠ������W�b�N�j
*/
ANumber	AWindow_Text::SPI_GetTextViewWidth( const ADocumentID inDocumentID ) 
{
	//�c�������[�h�擾 #558
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	ABool	verticalMode = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetVerticalMode();
	
	//�Ώۃh�L�������g��DiffTargetDocument���ǂ�������
	if( GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_IsDiffTargetDocument() == false )
	{
		if( verticalMode == false )
		{
			//�ʏ펞�@���C�A�E�g�v�Z�����text view���擾
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetTabIndexByDocumentID(inDocumentID),layout);
			return layout.w_MainTextView - layout.w_MainTextMargin;
		}
		else
		{
			//�c�������[�h�̏ꍇ #558
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetTabIndexByDocumentID(inDocumentID),layout);
			return layout.h_MainColumn-layout.w_LineNumber -10 - layout.w_MainTextMargin;
		}
	}
	else
	{
		//ATextWindowLayoutData	layout = {0};
		//CalculateLayout(NVI_GetTabIndexByDocumentID(inDocumentID),layout);
		//mode index�擾
		AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_GetModeIndex();
		//�s�ԍ����擾
		ANumber	lineNumberWidth = GetLineNumberWidth(NVI_GetTabIndexByDocumentID(inDocumentID));//#450
		//
		if( Is4PaneMode() == true )
		{
			//4pane���[�h�̏ꍇ�A�T�u�e�L�X�g�́A�T�u�e�L�X�g�J�����ɕ\�������B
			return mSubTextColumnWidth - lineNumberWidth -1;
		}
		else
		{
			//3 pane���[�h�̏ꍇ�A�T�u�e�L�X�g�́A���T�C�h�o�[�ɕ\�������B
			return mLeftSideBarWidth - lineNumberWidth -1;
		}
	}
}

/*#199
//
void	AWindow_Text::SPI_DoLineInfoChangeStart( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(controlID).SPI_DoLineInfoChangeStart();
	}
}

//
void	AWindow_Text::SPI_DoLineInfoChangeEnd( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(controlID).SPI_DoLineInfoChangeEnd();
	}
}
*/

void	AWindow_Text::SPI_ShowWrapLetterCountWindow( const AWrapMode inWrapMode )//#1113
{
	/*#199 mWrapLetterCountWindow*/SPI_GetWrapLetterCountWindow().NVI_Create(NVI_GetCurrentDocumentID());
	SPI_GetWrapLetterCountWindow().SPI_SetWrapMode(inWrapMode);//#1113
	/*#199 mWrapLetterCountWindow*/SPI_GetWrapLetterCountWindow().NVI_Show();
	/*#199 mWrapLetterCountWindow*/SPI_GetWrapLetterCountWindow().NVI_SwitchFocusToFirst();
}

void	AWindow_Text::SPI_ShowTextEncodingCorrectWindow( const ABool inModify, 
		const ATextEncodingFallbackType inFallbackType )//#473
{
	SPI_GetCorrectEncodingWindow().SPNVI_CreateAndShow(NVI_GetCurrentDocumentID(),(inModify==false),inFallbackType);//#473
}

#if 0
void	AWindow_Text::SPI_ShowPrintOptionWindow()
{
	/*#199 mPrintOptionWindow*/SPI_GetPrintOptionWindow().NVI_Create(NVI_GetCurrentDocumentID());
	/*#199 mPrintOptionWindow*/SPI_GetPrintOptionWindow().NVI_Show();
	/*#199 mPrintOptionWindow*/SPI_GetPrintOptionWindow().NVI_SwitchFocusToFirst();
}
#endif

#if 0
void	AWindow_Text::SPI_ShowDocPropertyWindow()
{
	/*#199 mDocPropertyWindow*/SPI_GetPropertyWindow().NVI_Create(NVI_GetCurrentDocumentID());
	/*#199 mDocPropertyWindow*/SPI_GetPropertyWindow().NVI_Show();
	/*#199 mDocPropertyWindow*/SPI_GetPropertyWindow().NVI_SwitchFocusToFirst();
}
#endif

void	AWindow_Text::SPI_ShowDropWarningWindow( const AText& inText )
{
	/*#199 mDropWarningWindow*/SPI_GetDropWarningWindow().NVI_Create(NVI_GetCurrentDocumentID());
	/*#199 mDropWarningWindow*/SPI_GetDropWarningWindow().NVI_Show();
	/*#199 mDropWarningWindow*/SPI_GetDropWarningWindow().SPI_SetToolText(inText);
}

/*B0000 080810 void	AWindow_Text::SPI_ClearCursorRow()
{
	mJumpListWindow.SPI_ClearCursorRow();
}*/

void	AWindow_Text::SPI_ShowChildErrorWindow( const ADocumentID inDocumentID, const AText& inMessage1, const AText& inMessage2,
											   const ABool inLongDialog )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex == kIndex_Invalid )   return;
	NVI_SelectTab(tabIndex);
	NVI_SelectWindow();
	NVI_ShowChildWindow_OK(inMessage1,inMessage2,inLongDialog);
}

void	AWindow_Text::SPI_DoTool( const AFileAcc& inFile, const AModifierKeys inModifierKeys, const ABool inDontShowDropWarning )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_DoTool(inFile,inModifierKeys,inDontShowDropWarning);
	}
}

void	AWindow_Text::SPI_DoTool_Text( const ADocumentID inDocumentID, const AText& inText, const ABool inDontShowDropWarning )
{
	if( inDocumentID == NVI_GetCurrentDocumentID() )
	{
		AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
		if( focus != kControlID_Invalid )
		{
			GetTextViewByControlID(focus).SPI_DoTool_Text(inText,inDontShowDropWarning);
		}
	}
}

//RC2
//�ȗ����͂̌���Index��TextView�̊O���i�Ⴆ�Ό��E�C���h�E�j���猈�߂�i�e�L�X�g�����̌�╶���ɓ���ւ��j
void	AWindow_Text::SPI_SetCandidateIndex( const AControlID inTextViewControlID, const AIndex inIndex )
{
	//�w��e�L�X�g�r���[��control ID�łȂ���Ή������Ȃ�
	if( IsTextViewControlID(inTextViewControlID) == false )
	{
		return;
	}
	//���index�ݒ�
	GetTextViewByControlID(inTextViewControlID).SPI_SetCandidateIndex(inIndex);
}

//R0073
void	AWindow_Text::SPI_KeyTool( const AControlID inTextViewControlID, const AUChar inKeyChar )
{
	GetTextViewByControlID(inTextViewControlID).SPI_KeyTool(inKeyChar);
}

//R0232
//�t�@�C�������̃A�v���ŕҏW���ꂽ���Ƃ������_�C�A���O��\��
void	AWindow_Text::SPI_ShowAskApplyEditByOtherApp( const ADocumentID inDocumentID )
{
	/*#199 mEditByOtherAppWindow*/SPI_GetEditByOtherAppWindow().NVI_Create(kObjectID_Invalid);
	/*#199 mEditByOtherAppWindow*/SPI_GetEditByOtherAppWindow().SPI_AddDocument(inDocumentID);
	/*#199 mEditByOtherAppWindow*/SPI_GetEditByOtherAppWindow().NVI_Show();
}

//RC3
//�O���R�����g�_�C�A���O�\��
/* #138
void	AWindow_Text::SPI_ShowExternalCommentWindow( const AText& inModuleName, const AIndex inOffset )
{
	//�_�C�A���O�\��
	mExternalCommentWindow.NVI_Create(kObjectID_Invalid);
	mExternalCommentWindow.SPNVI_Show(NVI_GetCurrentDocumentID(),inModuleName,inOffset);
}
*/
#pragma mark ===========================

#pragma mark ---Ruler

AView_Ruler&	AWindow_Text::GetRulerView( const AIndex inTabIndex )//#246
{
	/*
	AWindowID	winID = mTabDataArray.GetObjectConst(inTabIndex).GetRulerWindowID();
	return (dynamic_cast<AWindow_Ruler&>(GetApp().NVI_GetWindowByID(winID))).SPI_GetRulerView();
	*/
	///*#725
	AControlID	rulerControlID = mTabDataArray.GetObjectConst(inTabIndex).GetRulerControlID();//#246
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Ruler,kViewType_Ruler,rulerControlID);
	//*/
	/*#199
	return dynamic_cast<AView_Ruler&>(NVI_GetViewByControlID(kControlID_Ruler));
	*/
}

//�ڐ��蓙�X�V
void	AWindow_Text::UpdateRulerProperty( const AIndex inTabIndex )
{
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kDisplayRuler) == true )
	{
		//#1186 ANumber	x1 = GetLineNumberWidth(inTabIndex);//#450
		//#1186
		ATextWindowLayoutData	layout = {0};
		CalculateLayout(inTabIndex,layout);
		ANumber	x1 = layout.w_LineNumber;
		//
		AItemCount	rulerScaleCount = 5;
		if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kRulerScaleWithTabLetter) == true )
		{
			rulerScaleCount = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetTabWidth();
		}
		GetRulerView(inTabIndex).SPI_UpdateProperty(NVI_GetDocumentIDByTabIndex(inTabIndex),x1,rulerScaleCount);
	}
}

#pragma mark ===========================

#pragma mark ---����

ABool	AWindow_Text::SPI_FindNext( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		return GetTextViewByControlID(focus).SPI_FindNext(inParam);
	}
	return false;
}

ABool	AWindow_Text::SPI_FindPrevious( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		return GetTextViewByControlID(focus).SPI_FindPrevious(inParam);
	}
	return false;
}

ABool	AWindow_Text::SPI_FindFromFirst( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		return GetTextViewByControlID(focus).SPI_FindFromFirst(inParam);
	}
	return false;
}

void	AWindow_Text::SPI_FindList( const AFindParameter& inParam, const ABool inExtractMatchedText )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		GetTextViewByControlID(focus).SPI_FindList(inParam,inExtractMatchedText);
	}
}

ABool	AWindow_Text::SPI_ReplaceNext( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		return GetTextViewByControlID(focus).SPI_ReplaceNext(inParam);
	}
	return false;
}

AItemCount	AWindow_Text::SPI_ReplaceAfterCaret( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		return GetTextViewByControlID(focus).SPI_ReplaceAfterCaret(inParam);
	}
	return 0;
}

AItemCount	AWindow_Text::SPI_ReplaceInSelection( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		return GetTextViewByControlID(focus).SPI_ReplaceInSelection(inParam);
	}
	return 0;
}

AItemCount	AWindow_Text::SPI_ReplaceAll( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		return GetTextViewByControlID(focus).SPI_ReplaceAll(inParam);
	}
	return 0;
}

ABool	AWindow_Text::SPI_ReplaceAndFind( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		return GetTextViewByControlID(focus).SPI_ReplaceAndFind(inParam);
	}
	return false;
}

ABool	AWindow_Text::SPI_ReplaceSelectedTextOnly( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		return GetTextViewByControlID(focus).SPI_ReplaceSelectedTextOnly(inParam);
	}
	return false;
}

//#935
/**
�ꊇ�u��
*/
void	AWindow_Text::SPI_BatchReplace( const AFindParameter& inParam, const AText& inBatchText )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 �����{�b�N�X�Ƀt�H�[�J�X������ꍇ��TextView�Ƀt�H�[�J�X���ڂ�
		GetTextViewByControlID(focus).SPI_BatchReplace(inParam,inBatchText);
	}
}

#pragma mark ===========================

#pragma mark ---�E�C���h�E�폜

/**

�ۑ��^�ۑ����Ȃ��^�L�����Z���m�F�_�C�A���O��\��
�_�C�A���O�̓��[�_��
�ۑ��̏ꍇ�F�ۑ����s�B�Ԃ�l��true
�ۑ����Ȃ��ꍇ�F�������Ȃ��B�Ԃ�l��true
�L�����Z���̏ꍇ�F�������Ȃ��B�Ԃ�l��false
*/
ABool	AWindow_Text::NVIDO_AskSaveForAllTabs()
{
	//AskSaveChanges�E�C���h�E�\�����Ȃ牽���������^�[��
	if( mAskingSaveChanges == true )    return false;
	
	//�^�u���Ƃ̃��[�v�iz-order��O����j
	AItemCount	displayTabCount = NVI_GetSelectableTabCount();
	for( AIndex zorder = 0; zorder < displayTabCount; zorder++ )
	{
		//�^�u�A�h�L�������g�擾
		AIndex	tabIndex = NVI_GetTabIndexByZOrderIndex(zorder);
		ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
		//dirty�Ȃ�ۑ��_�C�A���O�\��
		if( GetApp().SPI_GetTextDocumentByID(docID).NVI_IsDirty() == true )
		{
			//�E�C���h�E�A�^�u�I��
			NVI_SelectWindow();
			NVI_SelectTab(tabIndex);
			//���[�_��AskSaveChanges�E�C���h�E���J���āA�ۑ����邩�ǂ�����q�˂�
			AText	filename;
			GetApp().SPI_GetTextDocumentByID(docID).NVI_GetTitle(filename);
			mAskingSaveChanges = true;
			AAskSaveChangesResult	result = NVI_ShowAskSaveChangesWindow(filename,docID,true);//modal
			mAskingSaveChanges = false;
			//���ʂɉ���������
			ABool	canceled = false;
			switch(result)
			{
			  case kAskSaveChangesResult_Save:
				{
					//�ۑ����s
					//SaveAs�_�C�A���O���J���ꂽ�ꍇ�ȂǁA�ۑ��o���Ȃ������ꍇ�́Acanceled�t���O��ON�ɂ���B�����[�v���f���āA�Ԃ�lfalse�ŕԂ��B
					canceled = (GetApp().SPI_GetTextDocumentByID(docID).SPI_Save(false)==false);
					break;
				}
			  case kAskSaveChangesResult_DontSave:
				{
					//��������
					break;
				}
			  case kAskSaveChangesResult_Cancel:
				{
					canceled = true;
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			if( canceled == true )
			{
				return false;
			}
		}
	}
	return true;
}

/**
����
*/
void	AWindow_Text::NVIDO_ExecuteClose()
{
	//AskSaveChanges�E�C���h�E�\�����Ȃ牽���������^�[��
	if( mAskingSaveChanges == true )    return;
	
	//view bounds�X�V�̗}��
	AStSuppressTextWindowUpdateViewBounds	s(GetObjectID());
	
	//�^�u���ꂼ��ɂ��āA���鏈�����s��
	while( NVI_GetTabCount() > 0 )
	{
		//�Ŕw�ʃ^�u�i�\���^�u�̂����̍Ŕw�ʁj�̃h�L�������g���擾
		AIndex	closingTabIndex = NVI_GetMostBackTabIndex();//#579
		ADocumentID	closingDocID = NVI_GetDocumentIDByTabIndex(closingTabIndex);//#579 0);
		//Close���s�i�����N���[�Y�j
		ExecuteClose(closingDocID);
	}
}

#pragma mark ===========================

#pragma mark ---DiffInfoWindow

//#379
/**
Diff���\���E�C���h�E�\���X�V
*/
void	AWindow_Text::SPI_RefreshDiffInfoWindow() const
{
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
}

/**
diff info �E�C���h�E�\��
*/
void	AWindow_Text::SPI_ShowDiffWindow()
{
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Show(false);
}

/**
diff info �E�C���h�E��\��
*/
void	AWindow_Text::SPI_HideDiffWindow()
{
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Hide();
}


#pragma mark ===========================

#pragma mark ---���E��r
//#737

//drop
#if 0
/**
*/
void	AWindow_Text::SPI_CompareWithSubText( const ABool inCompareWithSelectionInSubText )
{
	AControlID	mainTextControlID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0);
	AIndex	subPaneTabIndex = NVI_GetTabIndexByDocumentID(mSubTextCurrentDocumentID);
	AControlID	subTextControlID = mTabDataArray.GetObjectConst(subPaneTabIndex).GetSubTextControlID();
	//
	AText	mainText;
	GetTextViewByControlID(mainTextControlID).SPI_GetSelectedText(mainText);
	ATextPoint	mainTextSpt = {0}, mainTextEpt = {0};
	GetTextViewByControlID(mainTextControlID).SPI_GetSelect(mainTextSpt,mainTextEpt);
	mMainSubDiff_DiffRangeMainStartParagraphIndex = 
			SPI_GetTextDocumentConst(mainTextControlID).SPI_GetParagraphIndexByLineIndex(mainTextSpt.y);
	mMainSubDiff_DiffRangeMainEndParagraphIndex = 
			SPI_GetTextDocumentConst(mainTextControlID).SPI_GetParagraphIndexByLineIndex(mainTextEpt.y);
	//
	AText	subText;
	ATextPoint	subTextSpt = {0}, subTextEpt = {0};
	if( inCompareWithSelectionInSubText == true )
	{
		GetTextViewByControlID(subTextControlID).SPI_GetSelectedText(subText);
		GetTextViewByControlID(subTextControlID).SPI_GetSelect(subTextSpt,subTextEpt);
	}
	else
	{
		SPI_GetTextDocumentConst(subTextControlID).SPI_GetText(subText);
		subTextSpt.y = 0;
		subTextEpt.y = SPI_GetTextDocumentConst(subTextControlID).SPI_GetLineCount()-1;
	}
	mMainSubDiff_DiffRangeSubStartParagraphIndex = 
			SPI_GetTextDocumentConst(subTextControlID).SPI_GetParagraphIndexByLineIndex(subTextSpt.y);
	mMainSubDiff_DiffRangeSubEndParagraphIndex = 
			SPI_GetTextDocumentConst(subTextControlID).SPI_GetParagraphIndexByLineIndex(subTextEpt.y);
	//
	mMainSubDiff_StartParagraphIndexArray.DeleteAll();
	mMainSubDiff_EndParagraphIndexArray.DeleteAll();
	mMainSubDiff_OldStartParagraphIndexArray.DeleteAll();
	mMainSubDiff_OldEndParagraphIndexArray.DeleteAll();
	//
	AFileAcc	diffTempFolder;
	GetApp().SPI_GetUniqTempFolder(diffTempFolder);
	AArray<AIndex>	startParagraphIndexArray, endParagraphIndexArray;
	ADocument_Text::GetDiffDataFor2Texts(diffTempFolder,
				subText,mainText,mMainSubDiff_DiffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
				mMainSubDiff_OldStartParagraphIndexArray,mMainSubDiff_OldEndParagraphIndexArray,mMainSubDiff_DeletedTextArray);
	diffTempFolder.DeleteFileOrFolderRecursive();
	//
	AItemCount	itemCount = mMainSubDiff_DiffTypeArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		mMainSubDiff_StartParagraphIndexArray.Add(startParagraphIndexArray.Get(i)+mMainSubDiff_DiffRangeMainStartParagraphIndex);
		mMainSubDiff_EndParagraphIndexArray.Add(endParagraphIndexArray.Get(i)+mMainSubDiff_DiffRangeMainStartParagraphIndex);
		mMainSubDiff_OldStartParagraphIndexArray.Set(i,mMainSubDiff_OldStartParagraphIndexArray.Get(i)+mMainSubDiff_DiffRangeSubStartParagraphIndex);
		mMainSubDiff_OldEndParagraphIndexArray.Set(i,mMainSubDiff_OldEndParagraphIndexArray.Get(i)+mMainSubDiff_DiffRangeSubStartParagraphIndex);
	}
	//
	mIsMainSubDiffMode = true;
	mMainSubDiff_MainTextControlID = mainTextControlID;
	mMainSubDiff_SubTextControlID = subTextControlID;
	//paragraph�Ώƕ\���쐬
	mMainSubDiff_ParagraphIndexArray.DeleteAll();
	ADocument_Text::SPI_GetDiffParagraphIndexArray(SPI_GetTextDocumentConst(mainTextControlID).SPI_GetParagraphCount(),
			mMainSubDiff_DiffTypeArray,
			mMainSubDiff_StartParagraphIndexArray,mMainSubDiff_EndParagraphIndexArray,
			mMainSubDiff_OldStartParagraphIndexArray,mMainSubDiff_OldEndParagraphIndexArray,
			mMainSubDiff_ParagraphIndexArray);
	//
	NVI_RefreshWindow();
}

/**
*/
ABool	AWindow_Text::SPI_IsMainSubDiffMode( const AControlID inTextControlID ) const
{
	if( mIsMainSubDiffMode == true )
	{
		if( inTextControlID == mMainSubDiff_MainTextControlID || inTextControlID == mMainSubDiff_SubTextControlID )
		{
			return true;
		}
	}
	return false;
}

/**
�sIndex����DiffPart�C���f�b�N�X���擾
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffIndexByIncludingLineIndex(  const AControlID inTextControlID , 
		const AIndex inLineIndex, ADiffType& outDiffType ) const
{
	//
	AIndex	diffIndex = kIndex_Invalid;
	outDiffType = kDiffType_None;
	//
	AIndex	paraIndex = SPI_GetTextDocumentConst(inTextControlID).SPI_GetParagraphIndexByLineIndex(inLineIndex);
	//
	if( inTextControlID == mMainSubDiff_MainTextControlID )
	{
		//
		if( paraIndex < mMainSubDiff_DiffRangeMainStartParagraphIndex || paraIndex >= mMainSubDiff_DiffRangeMainEndParagraphIndex )
		{
			outDiffType = kDiffType_OutOfDiffRange;
			return diffIndex;
		}
		//
		AItemCount	itemcount = mMainSubDiff_StartParagraphIndexArray.GetItemCount();
		for( AIndex i = 0; i < itemcount; i++ )
		{
			if( paraIndex >= mMainSubDiff_StartParagraphIndexArray.Get(i) &&
						paraIndex < mMainSubDiff_EndParagraphIndexArray.Get(i) )
			{
				diffIndex = i;
				outDiffType = mMainSubDiff_DiffTypeArray.Get(diffIndex);
				return diffIndex;
			}
		}
	}
	else if( inTextControlID == mMainSubDiff_SubTextControlID )
	{
		//
		if( paraIndex < mMainSubDiff_DiffRangeSubStartParagraphIndex || paraIndex >= mMainSubDiff_DiffRangeSubEndParagraphIndex )
		{
			outDiffType = kDiffType_OutOfDiffRange;
			return kIndex_Invalid;
		}
		//
		AItemCount	itemcount = mMainSubDiff_OldStartParagraphIndexArray.GetItemCount();
		for( AIndex i = 0; i < itemcount; i++ )
		{
			if( paraIndex >= mMainSubDiff_OldStartParagraphIndexArray.Get(i) &&
				paraIndex < mMainSubDiff_OldEndParagraphIndexArray.Get(i) )
			{
				diffIndex = i;
				outDiffType = mMainSubDiff_DiffTypeArray.Get(diffIndex);
				switch(outDiffType)
				{
				  case kDiffType_Added:
					{
						outDiffType = kDiffType_Deleted;
						break;
					}
				  case kDiffType_Deleted:
					{
						outDiffType = kDiffType_Added;
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
				return diffIndex;
			}
		}
	}
	return kIndex_Invalid;
}

//#379
/**
DiffPart�̃C���f�b�N�X����DiffType���擾
*/
ADiffType	AWindow_Text::SPI_MainSubDiff_GetDiffTypeByDiffIndex( const AIndex inDiffIndex ) const
{
	return mMainSubDiff_DiffTypeArray.Get(inDiffIndex);
}

/**
DiffPart�J�n�s����DiffPart�C���f�b�N�X���擾
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffIndexByStartLineIndex( const AIndex inLineIndex ) const
{
	const ADocument_Text&	doc = SPI_GetTextDocumentConst(mMainSubDiff_MainTextControlID);
	//�i���J�n�s�ȊO�Ȃ猟�����Ȃ�
	if( inLineIndex > 0 )
	{
		if( doc.SPI_GetLineExistLineEnd(inLineIndex-1) == false )   return kIndex_Invalid;
	}
	//�w��s����n�܂�DiffIndex������
	AIndex	paraIndex = doc.SPI_GetParagraphIndexByLineIndex(inLineIndex);
	return mMainSubDiff_StartParagraphIndexArray.Find(paraIndex);
}

/**
DiffPart�I���s����DiffPart�C���f�b�N�X���擾
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffIndexByEndLineIndex( const AIndex inLineIndex ) const
{
	const ADocument_Text&	doc = SPI_GetTextDocumentConst(mMainSubDiff_MainTextControlID);
	//�i���J�n�s�ȊO�Ȃ猟�����Ȃ�
	if( inLineIndex > 0 )
	{
		if( doc.SPI_GetLineExistLineEnd(inLineIndex-1) == false )   return kIndex_Invalid;
	}
	//�w��s�ŏI������DiffIndex������
	AIndex	paraIndex = doc.SPI_GetParagraphIndexByLineIndex(inLineIndex);
	return mMainSubDiff_EndParagraphIndexArray.Find(paraIndex);
}

/**
DiffPart�̃C���f�b�N�X���炻��Part�̊J�n�s���擾
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffStartLineIndexByDiffIndex( const AIndex inDiffIndex ) const
{
	const ADocument_Text&	doc = SPI_GetTextDocumentConst(mMainSubDiff_MainTextControlID);
	AIndex	paraIndex = mMainSubDiff_StartParagraphIndexArray.Get(inDiffIndex);
	return doc.SPI_GetLineIndexByParagraphIndex(paraIndex);
}

/**
DiffPart�̃C���f�b�N�X���炻��Part�̏I���s���擾
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffEndLineIndexByDiffIndex( const AIndex inDiffIndex ) const
{
	const ADocument_Text&	doc = SPI_GetTextDocumentConst(mMainSubDiff_MainTextControlID);
	AIndex	paraIndex = mMainSubDiff_EndParagraphIndexArray.Get(inDiffIndex);
	return doc.SPI_GetLineIndexByParagraphIndex(paraIndex);
}

/**
���̃h�L�������g�̒i��index�ɑΉ�����ADiff�Ώۃh�L�������g�̒i��index���擾
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffTargetParagraphIndexFromMainTextParagraphIndex( const AIndex inParagraphIndex ) const
{
	//
	if( inParagraphIndex >= mMainSubDiff_ParagraphIndexArray.GetItemCount() )
	{
		return kIndex_Invalid;
	}
	//mDiffParagraphIndexArray�̎w��index�̒l��Ԃ�
	return mMainSubDiff_ParagraphIndexArray.Get(inParagraphIndex);
}

/**
DiffPart�J�n�i������DiffPart�C���f�b�N�X���擾
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffIndexByStartParagraphIndex( const AIndex inParagraphIndex ) const
{
	return mMainSubDiff_StartParagraphIndexArray.Find(inParagraphIndex);
}

/**
DiffPart�I���s����DiffPart�C���f�b�N�X���擾
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffIndexByEndParagraphIndex( const AIndex inParagraphIndex ) const
{
	return mMainSubDiff_EndParagraphIndexArray.Find(inParagraphIndex);
}

/**
�iDiff���j�i��index����AWindowPoint���擾
*/
void	AWindow_Text::SPI_MainSubDiff_GetSubTextWindowPointByParagraph( const AIndex inDiffParagraphStart,
		AWindowPoint& outWindowPoint ) const
{
	outWindowPoint.x = 0;
	outWindowPoint.y = 0;
	//Diff�Ώۃh�L�������g�E�^�u�E�r���[�擾
	if( mMainSubDiff_SubTextControlID != kControlID_Invalid )
	{
		if( GetTextViewConstByControlID(mMainSubDiff_SubTextControlID).NVI_IsVisible() == true )
		{
			//�iDiff���j�i��index����AWindowPoint���擾
			AIndex	lineIndex = SPI_GetTextDocumentConst(mMainSubDiff_SubTextControlID).SPI_GetLineIndexByParagraphIndex(inDiffParagraphStart);
			ALocalPoint	lpt = {0,0};
			lpt.y = GetTextViewConstByControlID(mMainSubDiff_SubTextControlID).SPI_GetLocalYByLineIndex(lineIndex);
			NVI_GetWindowPointFromControlLocalPoint(mMainSubDiff_SubTextControlID,lpt,outWindowPoint);
		}
	}
}
#endif

#pragma mark ===========================

#pragma mark ---SCM

//#455
/**
�R�~�b�g�E�C���h�E�\��
*/
void	AWindow_Text::SPI_ShowCommitWindow( const ADocumentID inDocID )
{
	if( GetApp().SPI_GetTextDocumentByID(inDocID).SPI_CanCommit() == false )   return;
	
	SPI_GetSCMCommitWindow().NVI_Create(inDocID);
	SPI_GetSCMCommitWindow().NVI_Show();
}

//#455
/**
���̕ύX�ӏ�
*/
void	AWindow_Text::SPI_NextDiff()
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_NextDiff();
	}
}

//#455
/**
���̕ύX�ӏ�
*/
void	AWindow_Text::SPI_PrevDiff()
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_PrevDiff();
	}
}

#pragma mark ===========================

#pragma mark ---Progress�\��

/**
�C���|�[�g�t�@�C���v���O���X�\��
*/
void	AWindow_Text::SPI_ShowImportFileProgress( const ANumber inCount )
{
	mFooterProgress_ShowImportFileProgress = true;
	mFotterProgress_ImportFileProgressRemainCount = inCount;
	UpdateFooterProgress();
}

/**
�C���|�[�g�t�@�C���v���O���X��\��
*/
void	AWindow_Text::SPI_HideImportFileProgress()
{
	mFooterProgress_ShowImportFileProgress = false;
	mFotterProgress_ImportFileProgressRemainCount = 0;
	UpdateFooterProgress();
}

/**
�t�b�^�[���v���O���X�X�V
*/
void	AWindow_Text::UpdateFooterProgress()
{
	//���C�A�E�g�擾
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(0,layout);
	//
	ABool	showProgressIndicator = false;
	ABool	showProgressText = false;
	AText	progressText;
	if( mFooterProgress_ShowImportFileProgress == true )//&& layout.h_Footer > 0  )
	{
		//�v���O���X�e�L�X�g�擾
		progressText.SetLocalizedText("Progress_ImportFile");
		/*
		//�c��t�@�C�����e�L�X�g
		AText	t;
		t.SetFormattedCstring(" (%d)",mFotterProgress_ImportFileProgressRemainCount);
		progressText.AddText(t);
		*/
		//�\���t���OON
		showProgressIndicator = true;
		showProgressText = true;
	}
	//
	if( showProgressText == true )
	{
		//�v���O���X�e�L�X�g�ݒ�A�\��
		NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).NVI_SetText(progressText);
		NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).SPI_SetWidthToFitTextWidth();
		ANumber	textWidth = NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).NVI_GetWidth();
		AWindowPoint	pt = {0};
		pt.x = layout.pt_MacroBar.x + layout.w_MacroBar - textWidth - kImportProgressRightMargin;//mFooterProgressRightPosition - kRightMargin_ProgressText - textWidth;
		pt.y = layout.pt_MacroBar.y + 2;//layout.pt_Footer.y + kTopMargin_ProgressIndicator +1;
		NVI_SetControlPosition(kControlID_MainColumnProgressText,pt);
		NVI_SetShowControl(kControlID_MainColumnProgressText,true);
		//�v���O���Xindicator�\���E��\��
		if( showProgressIndicator == true )
		{
			pt.x -= kWidth_ProgressIndicator;
			pt.y = layout.pt_MacroBar.y;
			//pt.y = layout.pt_Footer.y + kTopMargin_ProgressIndicator;
			NVI_SetControlPosition(kControlID_MainColumnProgressIndicator,pt);
			NVI_SetShowControl(kControlID_MainColumnProgressIndicator,true);
			//
			NVI_SetControlBool(kControlID_MainColumnProgressIndicator,true);
		}
		else
		{
			NVI_SetControlBool(kControlID_MainColumnProgressIndicator,false);
		}
	}
	else
	{
		//�v���O���X�e�L�X�g��\��
		NVI_SetShowControl(kControlID_MainColumnProgressText,false);
		NVI_SetShowControl(kControlID_MainColumnProgressIndicator,false);
	}
}


#pragma mark ===========================

#pragma mark <���L�N���X(View)�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�����ʒm�R�[���o�b�N

//View���X�N���[�����ꂽ�Ƃ��A���̌��ʂ𑼂�View�ɔ��f�����鏈���i�X�N���[���o�[��TextView���Ŕ��f�ς݁j
void	AWindow_Text::SPI_ViewScrolled( const AControlID inControlID, const ANumber inDeltaX, const ANumber inDeltaY,
										 const ABool inRedraw, const ABool inConstrainToImageSize )
{
	//==================���C��text view�X�N���[��������==================
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )//win �A�N�e�B�u�^�u�ȊO�ł��X�N���[������悤�ɂ���i�h�L�������g���J���Ƃ��̕`��j
	{
		AIndex	viewIndex = mTabDataArray.GetObject(tabIndex).FindFromTextViewControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			AImagePoint	originOfLocalFrame;
			GetTextViewByControlID(inControlID).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
			//����TextView��H�����X�N���[���ʒu����
			/*#��������ƁADrag�X�N���[���̏ꍇ�͂��܂������̂����A�X�N���[���o�[�̏ꍇ�ɂ��܂������Ȃ��B�i�X�N���[���o�[�͌��XAWindow�œ����X�N���[�����Ă���̂ŁA���삪�d������j
			AItemCount	viewCount = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetViewCount();
			for( AIndex i = 0; i < viewCount; i++ )
			{
			AControlID	otherViewControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetTextViewControlID(i);
			AImagePoint	otherViewOrigin;
			GetTextViewByControlID(otherViewControlID).NVI_GetOriginOfLocalFrame(otherViewOrigin);
			if( otherViewOrigin.x != originOfLocalFrame.x )
			{
			otherViewOrigin.x = originOfLocalFrame.x;
			GetTextViewByControlID(otherViewControlID).NVI_ScrollTo(otherViewOrigin);
			}
			}*/
			//�s�ԍ�View�̃X�N���[���ʒu����
			AImagePoint	pt = originOfLocalFrame;
			pt.x = 0;
			GetLineNumberViewByControlID(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(viewIndex)).
					NVI_ScrollTo(pt,inRedraw,inConstrainToImageSize);
			if( inDeltaX != 0 )
			{
				AModeIndex	modeIndex = GetTextViewByControlID(inControlID).SPI_GetTextDocument().SPI_GetModeIndex();
				if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kDisplayBackgroundImage) == true )
				{
					GetLineNumberViewByControlID(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(viewIndex)).NVI_Refresh();
				}
			}
			//���[���[�̃X�N���[���ʒu����
			if( GetRulerView(tabIndex).NVI_IsVisible() == true )//B0393 
			{
				pt = originOfLocalFrame;
				pt.y = 0;
				GetRulerView(tabIndex).NVI_ScrollTo(pt,inRedraw);
			}
		}
	}
	//==================�T�utext view�X�N���[��������==================
	//#291 �T�u�y�C���̍s�ԍ��\���G���A�X�V
	//#725 if( mLeftSideBarDisplayed == true )
	{
		//�e�T�u�y�C����TextView�Ɣ�r
		for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
		{
			if( mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID() == inControlID )
			{
				AImagePoint	originOfLocalFrame;
				GetTextViewByControlID(inControlID).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
				//�T�u�y�C����TextView��ControlID�ƈ�v������Ή�����s�ԍ�View���X�N���[������
				AControlID	subPaneLineNumberControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextLineNumberControlID();
				AImagePoint	pt = originOfLocalFrame;
				pt.x = 0;
				GetLineNumberViewByControlID(subPaneLineNumberControlID).NVI_ScrollTo(pt,inRedraw,inConstrainToImageSize);
				//X�����̃X�N���[���̏ꍇ�A�w�i�ĕ`�悷��
				if( inDeltaX != 0 )
				{
					AModeIndex	modeIndex = GetTextViewByControlID(inControlID).SPI_GetTextDocument().SPI_GetModeIndex();
					if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kDisplayBackgroundImage) == true )
					{
						GetLineNumberViewByControlID(subPaneLineNumberControlID).NVI_Refresh();
					}
				}
			}
		}
	}
}

//
void	AWindow_Text::NVIDO_SelectionChanged( const AControlID inControlID )
{
	//#852
	//�r���[��\���Ȃ牽�����Ȃ�
	if( NVI_GetViewByControlID(inControlID).NVI_IsVisible() == false )
	{
		return;
	}
	//
	if( NVI_GetViewByControlID(inControlID).NVI_GetViewType() == kViewType_Text )
	{
		//focus�ɂ͌��݂�focus�̃e�L�X�g�r���[������B�i�T�u�e�L�X�g�y�C����focus�Ȃ炻�ꂪ����j
		AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
		if( focus != kControlID_Invalid )
		{
			ATextPoint	spt, ept;
			GetTextViewByControlID(focus).SPI_GetSelect(spt,ept);
			//�W�����v���X�g�̃^�u�I�� #725
			GetApp().SPI_SelectJumpListWindowsTab(GetObjectID(),SPI_GetTextDocument(focus).GetObjectID());
			//�W�����v���X�g�̑I���X�V
			//#291 SPI_UpdateJumpListSelection(spt.y);
			SPI_GetTextDocument(focus).SPI_UpdateJumpListSelection(spt.y,true);//#291
			//�s�ԍ��{�^���̍X�V
			SPI_UpdateLineNumberButton(focus);
			//�ꏊ�{�^���̍X�V
			UpdatePositionButton(focus);
			if( mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).FindFromTextViewControlID(focus) != kIndex_Invalid )//#212
			{
				//���[���[�̃L�����b�g�X�V
				{
					AImagePoint	spt, ept;
					GetTextViewByControlID(focus).SPI_GetImageSelect(spt,ept);
					if( spt.x == ept.x )//�I��͈̓h���b�O�Ɏ��Ԃ��������Ă��܂��̂�h�����߂ɁA�I����Ԏ��̓��[���[�L�����b�g�X�V���Ȃ�
					{
						GetRulerView(NVI_GetCurrentTabIndex()).SPI_UpdateCaretImageX(spt.x,ept.x);
					}
				}
			}
			//B0308
			if( NVI_IsWindowActive() == true )
			{
				//
				AIndex	jumpMenuItemIndex = SPI_GetTextDocument(focus).SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
				//#857 if( jumpMenuItemIndex != mJumpMenuItemIndex )//B0000 ������
				//#857 {
				//
				if( jumpMenuItemIndex >= kLimit_MaxJumpMenuItemCount )//#695
				{
					jumpMenuItemIndex = kIndex_Invalid;
				}
				//#857 GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenu(kMenuItemID_Jump,jumpMenuItemIndex);
				GetApp().SPI_UpdateJumpMenu(SPI_GetTextDocument(focus).SPI_GetJumpMenuObjectID(),jumpMenuItemIndex);//#857
				//
				//#857 mJumpMenuItemIndex = jumpMenuItemIndex;//B0000
				//#857 }
			}
			//}
			//#723 CallGraf�X�V
			//���݈ʒu��function�����擾
			SPI_GetTextDocument(focus).SPI_SetCallGrafCurrentFunction(GetObjectID(),spt.y,false);
			
			if( mDiffDisplayMode == true && SPI_GetTextDocumentConst(focus).NVI_IsDirty() == false )
			{
				if( SPI_GetTextDocumentConst(focus).SPI_IsDiffTargetDocument() == false )
				{
					//���݂̃t�H�[�J�X��Diff���h�L�������g�łȂ��ꍇ�ADiff���h�L�������g�̑I���ʒu���A�Ή�����ӏ��ɐݒ肷��
					
					ADocumentID	diffDocID = SPI_GetTextDocument(focus).SPI_GetDiffTargetDocumentID();
					ADocumentID	workingDocID = SPI_GetTextDocument(focus).GetObjectID();
					if( diffDocID != kObjectID_Invalid )
					{
						AIndex	tabIndex = NVI_GetTabIndexByDocumentID(diffDocID);
						AControlID	subTextControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID();
						GetTextViewByControlID(subTextControlID).SPI_SetSelectByDiffTargetDocumentSelection(spt,ept);
						//�X�N���[������
						SPI_AdjustDiffDisplayScroll(workingDocID,focus,kIndex_Invalid);
					}
				}
			}
			//#734
			//�v���r���[�X�V
			if( SPI_IsSubTextView(focus) == false )//���C���r���[�̏ꍇ�̂݃v���r���[�X�V
			{
				if( GetApp().SPI_PreviewWindowExist(GetObjectID()) == true )
				{
					SPI_GetTextDocument(focus).SPI_UpdatePreview(spt.y);
				}
			}
		}
	}
}

/**
�{�^���h���b�O�L�����Z��������
*/
void	AWindow_Text::NVIDO_ButtonViewDragCanceled( const AControlID inControlID )
{
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(inControlID);
	if( toolButtonIndex != kIndex_Invalid )
	{
		//�}�N�����ڂ��h���b�O�L�����Z���������\���ɂ���
		GetCurrentTabModePrefDB().SetEnableToolbarItem(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(toolButtonIndex),false);
	}
}

/**
�W�����v���X�g�I�����X�V
*/
void	AWindow_Text::SPI_UpdateJumpListSelection( const ABool inAdjustScroll )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		ATextPoint	spt, ept;
		GetTextViewByControlID(focus).SPI_GetSelect(spt,ept);
		//�W�����v���X�g�̑I���X�V
		SPI_GetTextDocument(focus).SPI_UpdateJumpListSelection(spt.y,inAdjustScroll);
	}
}

/**
�W�����v���X�g�z�o�[�X�V������
*/
void	AWindow_Text::SPI_DoListViewHoverUpdated( const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_DoListViewHoverUpdated(inHoverStartDBIndex,inHoverEndDBIndex);
	}
}

/**
view bounds�X�V�}���N���X�@�R���X�g���N�^
*/
AStSuppressTextWindowUpdateViewBounds::AStSuppressTextWindowUpdateViewBounds( const AWindowID inTextWindowID ) : mTextWindowID(inTextWindowID)
{
	GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IncrementSuppressUpdateViewBoundsCounter();
}

/**
view bounds�X�V�}���N���X�@�f�X�g���N�^
*/
AStSuppressTextWindowUpdateViewBounds::~AStSuppressTextWindowUpdateViewBounds()
{
	if( GetApp().NVI_ExistWindow(mTextWindowID) == true )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DecrementSuppressUpdateViewBoundsCounter();
	}
}


