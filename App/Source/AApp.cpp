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

AApp

*/

#include "stdafx.h"

#include "AApp.h"
#include "ADocument_IndexWindow.h"
//#858 #include "AView_List_FusenList.h"
#include "AView_Index.h"
#include "AView_IdInfo.h"
#include "AProjectFolderLoader.h"
#include "AWindow_DevTools.h"
#if IMPLEMENTATION_FOR_WINDOWS
#include <Shlobj.h>
#endif
#include "AView_CallGraf.h"
#include "AView_CandidateList.h"
#include "AWindow_Notification.h"
#include "AView_Notification.h"
#include "AWindow_FindResult.h"
#include "AView_Text.h"
#include "ASameFolderLoader.h"
#include "ADocument_Text.h"
#if IMPLEMENTATION_FOR_MACOSX
#import "AppDelegate.h"
#endif
#include "../../AbsFramework/AbsBase/Cocoa.h"
#include "AWindow_JavaScriptModalAlert.h"
#include "AWindow_JavaScriptModalCancelAlert.h"

const AItemCount	kJumpListHistoryMax = 100;//#454

const AItemCount	kModeUpdateURLCount = 12;//#427

const AItemCount	kFileFilterPresetCount = 3;//#617

#pragma mark ===========================
#pragma mark [�N���X]AApp
#pragma mark ===========================
//�A�v���P�[�V�����I�u�W�F�N�g

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AApp::AApp() : mUntitledDocumentNumber(0), mAppElapsedTick(0), mCurrentToolMenuModeIndex(kIndex_Invalid),
		mCurrentSameProjectIndex(kIndex_Invalid), mCurrentSameFolder(kIndex_Invalid),
/*#1034
#if IMPLEMENTATION_FOR_MACOSX
		mModelDirector(&mASApp), 
#endif
*/
	mSDFDebugMode(false)
	,mNoCheckModeExecutable(false)//R0137
	,mNavigateIndex(0)//#146
//#725	,mKeyRecordWindowID(kObjectID_Invalid)//#390
,mProjectPrefDBArray(NULL)//#693,true)//#417
	,mToolCommandWaitOpenFileFlag(false)
//#379	,mDiffWindowID(kObjectID_Invalid)//#379
	,mCurrentSameProjectModeIndex(kIndex_Invalid)//#533
	,mEnabledDefinesWindowID(kObjectID_Invalid)//#467
	,mHandToolMode(false)//#606
,mAutoIndentMode(true)//#638
,mAppPrefWindowID(kObjectID_Invalid)//#693
//#868 ,mModePrefWindowID(kObjectID_Invalid)//#693
,mFindWindowID(kObjectID_Invalid)//#693
,mAboutWindowID(kObjectID_Invalid)//#693
,mMultiFileFindWindowID(kObjectID_Invalid)//#693
//#922 ,mFTPFolderWindowID(kObjectID_Invalid)//#693
,mMultiFileFindProgressWindowID(kObjectID_Invalid)//#693
,mFTPProgressWindowID(kObjectID_Invalid)//#693
//#922 ,mFolderListWindowID(kObjectID_Invalid)//#693
//#725 ,mKeyToolListWindowID(kObjectID_Invalid)//#693
,mFTPLogWindowID(kObjectID_Invalid)//#693
//#695 ,mBigFileAlertWindowID(kObjectID_Invalid)//#693
,mTextCountWindowID(kObjectID_Invalid)//#693
,mNewFTPFileWindowID(kObjectID_Invalid)//#693
//#858 ,mFusenListWindowID(kObjectID_Invalid)//#693
//#858 ,mFusenEditWindowID(kObjectID_Invalid)//#693
//#725 ,mCandidateListWindowID(kObjectID_Invalid)//#693
//#725 ,mIdInfoWindowID(kObjectID_Invalid)//#693
//#725 ,mFileListWindowID(kObjectID_Invalid)//#693
,mSetProjectFolderWindowID(kObjectID_Invalid)//#693
//#688 ,mDevToolsWindowID(kObjectID_Invalid)//#693
//#858 ,mProjectFolderAlertWindowID(kObjectID_Invalid)//#693
,mCompareFolderWindowID(kObjectID_Invalid)//#693
,mCandidateFinderThreadID(kObjectID_Invalid)//#717
//,mPopupCandidateWindowID(kObjectID_Invalid)//#717
//,mPopupIdInfoWindowID(kObjectID_Invalid)//#725
,mIdInfoFinderThreadID(kObjectID_Invalid)//#853
,mCurrentJumpMenu_DynamicMenuObjectID(kObjectID_Invalid),mCurrentJumpMenu_CheckItemIndex(kIndex_Invalid)//#857
,mReopenProcessCompleted(false)
,mFIFOClipboardMode(false)//#878
,mKeyActionListWindowID(kObjectID_Invalid)//#738
,mEditProgressWindowID(kObjectID_Invalid), mEditProgressStartDateTime(0), mEditProgressType(kEditProgressType_None)//#846
,mEditProgressModalSessionStartedInActual(false), mEditProgressCurrentCount(0), mEditProgressCurrentProgressPercent(0)//#846
,mEditProgressModalSessionStarted(false),mEditProgressContinueCheckDateTime(0)//#846
,mCurrentTextMarkerGroupIndex(0)//#750
,mIsCandidatePartialSearchMode(false)//#717
,mJavaScriptExecuterThreadID(kObjectID_Invalid)
,mExecutingJavaScript(false)
,mAddNewModeWindowID(kObjectID_Invalid)//#844
,mActiveSubWindowForItemSelector(kObjectID_Invalid)
,mKillBuffer_DocumentID(kObjectID_Invalid)//#913
,mKillBuffer_TextPoint(kTextPoint_00)//#913
,mModeExecutableAlertWindowID(kObjectID_Invalid)
,mKeyRecording(false), mStoppedFlag(true)
,mSuppressAddToRecentlyOpenedFile(false)//#923
,mShowFloatingSubWindow(true)
,mEditProgressEnableCancelButton(false)
,mCurrentModeIndexForPluginLoad(kIndex_Invalid)//#994
,mPrependOpenUntitledFileEvent(false)//#1056
,mJavaScriptModalAlertWindowID(kObjectID_Invalid)//#1217
,mJavaScriptModalCancelAlertWindowID(kObjectID_Invalid)//#1217
,mUserIDRegistrationWindowID(kObjectID_Invalid)//#1384
{
	/*#1316
	//�F������
	mSkinColor_TabLetter = AColorWrapper::GetColorByHTMLFormatColor("333333");
	mSkinColor_TabLetterTopmost = AColorWrapper::GetColorByHTMLFormatColor("000000");
	mSkinColor_TabLetterDeactive = AColorWrapper::GetColorByHTMLFormatColor("444444");
	mSkinColor_TabLetterDropShadow = AColorWrapper::GetColorByHTMLFormatColor("CCCCCC");
	mSkinColor_TabLetterTopmostDropShadow = AColorWrapper::GetColorByHTMLFormatColor("CCCCCC");
	mSkinColor_SubTextColumnButtonLetterColor = AColorWrapper::GetColorByHTMLFormatColor("4C4C4C");
	mSkinColor_SubTextColumnButtonLetterColorDeactive = AColorWrapper::GetColorByHTMLFormatColor("999999");
	mSkinColor_SubTextColumnButtonDropShadowColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
	mSkinColor_SubWindowLetterColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
	mSkinColor_SubWindowTitlebarTextColor = AColorWrapper::GetColorByHTMLFormatColor("2F3031");
	mSkinColor_SubWindowTitlebarTextDropShadowColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
	mSkinColor_SubWindowBackgroundColor = AColorWrapper::GetColorByHTMLFormatColor("E9EEF4");
	mSkinColor_SubWindowBackgroundColorDeactive = AColorWrapper::GetColorByHTMLFormatColor("F3F3F3");
	mSkinColor_SubWindowHeaderLetterColor = AColorWrapper::GetColorByHTMLFormatColor("2C4665");
	mSkinColor_SubWindowHeaderBackgroundColor = AColorWrapper::GetColorByHTMLFormatColor("D3DBE4");
	mSkinColor_SubWindowBoxLetterColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
	mSkinColor_SubWindowBoxLetterDropShadowColor = AColorWrapper::GetColorByHTMLFormatColor("CCCCCC");
	mSkinColor_SubWindowBoxBackgroundColor = AColorWrapper::GetColorByHTMLFormatColor("EEEEEE");
	mSkinColor_SubWindowBoxHeaderColor = AColorWrapper::GetColorByHTMLFormatColor("0000FF");
	mSkinColor_SubWindowBoxFunctionNameColor = AColorWrapper::GetColorByHTMLFormatColor("007500");
	mSkinColor_SubWindowBoxMatchedColor = AColorWrapper::GetColorByHTMLFormatColor("FF0000");
	mSkinColor_FileListDiffColor = AColorWrapper::GetColorByHTMLFormatColor("0000C3");
	mSkinColor_JumpListDiffColor = AColorWrapper::GetColorByHTMLFormatColor("FF0000");
	*/
	//mModePrefDBArray�̃��X�g�̃��X�g�r���ւ̒ǉ��E�폜�͍s��Ȃ��̂ŁA���X�g�̃X���b�hmutex�͕s�v�iSPI_GetModePrefDB()�͑����R�[�������̂ŁA�d���Ȃ��Ă��܂��j
	mModePrefDBArray.EnableThreadSafe(false);
	
	//Lua�p�֐��o�^ #567 #1170
	lua_register(NVI_Lua_GetVM(),"print",AApp::Lua_Output);
	lua_register(NVI_Lua_GetVM(),"GetMiLuaScriptingVersion",AApp::Lua_GetMiLuaScriptingVersion);
	lua_register(NVI_Lua_GetVM(),"InsertText",AApp::Lua_InsertText);
	lua_register(NVI_Lua_GetVM(),"DeleteText",AApp::Lua_DeleteText);
	lua_register(NVI_Lua_GetVM(),"GetNextCharTextIndex",AApp::Lua_GetNextCharTextIndex);
	lua_register(NVI_Lua_GetVM(),"GetPrevCharTextIndex",AApp::Lua_GetPrevCharTextIndex);
	lua_register(NVI_Lua_GetVM(),"GetParagraphIndexFromTextIndex",AApp::Lua_GetParagraphIndexFromTextIndex);
	lua_register(NVI_Lua_GetVM(),"GetParagraphStartTextIndex",AApp::Lua_GetParagraphStartTextIndex);
	lua_register(NVI_Lua_GetVM(),"GetParagraphCount",AApp::Lua_GetParagraphCount);
	lua_register(NVI_Lua_GetVM(),"GetActiveDocumentID",AApp::Lua_GetActiveDocumentID);
	lua_register(NVI_Lua_GetVM(),"GetActiveViewID",AApp::Lua_GetActiveViewID);
	lua_register(NVI_Lua_GetVM(),"GetDocumentIDFromViewID",AApp::Lua_GetDocumentIDFromViewID);
	lua_register(NVI_Lua_GetVM(),"CreateNewDocument",AApp::Lua_CreateNewDocument);
	lua_register(NVI_Lua_GetVM(),"RevealDocument",AApp::Lua_RevealDocument);
	lua_register(NVI_Lua_GetVM(),"GetText",AApp::Lua_GetText);
	lua_register(NVI_Lua_GetVM(),"GetSelectedRange",AApp::Lua_GetSelectedRange);
	lua_register(NVI_Lua_GetVM(),"SetSelectedRange",AApp::Lua_SetSelectedRange);
	lua_register(NVI_Lua_GetVM(),"autotest",AApp::Lua_AutoTest);
	//lua�t�@�C���ɂ��֐��ǉ�
	AFileAcc	miluaFile;
	AFileWrapper::GetResourceFile("mi.lua",miluaFile);
	AText	errorText;
	NVI_Lua_dofile(miluaFile,errorText);
    
	//EditBoxView�̃f�t�H���g�t�H���g�E�T�C�Y��ݒ� win
#if IMPLEMENTATION_FOR_MACOSX
	AText	defaultFontName;
	AFontWrapper::GetDialogDefaultFontName(defaultFontName);//#375
	AView_EditBox::SPI_SetDefaultFont(defaultFontName,14.0);
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	AText	defaultFontName;
	AFontWrapper::GetDialogDefaultFontName(defaultFontName);//#375
	AView_EditBox::SPI_SetDefaultFont(defaultFontName,10.0);
#endif
	
	//�N���V�[�P���X�g���[�X�p
	AFileAcc	app;
	AFileWrapper::GetAppFile(app);
	AText	appname;
	app.GetFilename(appname);
	if( appname.Compare("mi(debug).app") == true )
	{
		NVI_SetEnableAPICBTraceMode(true);
		NVI_SetEnableDebugTraceMode(true);
		ATrace::SetTraceAlwaysOutputToStderr();
		NVI_SetEnableDebugMenu(true);
		NVI_SetEnableTraceLog(true);
		mMiscDebugMode = true;
		/*#199 mTraceLogWindow*/SPI_GetTraceLogWindow().NVI_CreateAndShow();
	}
	_AInfo("AApp::App() started.",this);
	
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
#if IMPLEMENTATION_FOR_MACOSX
	//#460 �ݒ�t�@�C���ʒu�ړ�
	//~/Library/Application Support/mi3�����݂��Ă��Ȃ���΁A��Preferences�ʒu����R�s�[����
	if( appPrefFolder.Exist() == false )
	{
		//==================�o�[�W����2.1.11/2.1.12�̃t�H���_����R�s�[================== #941
		//�o�[�W����2.1.11/2.1.12�̃t�H���_�F~/Library/Application Support/mi
		AFileAcc	appPrefFolderV2;
		OSStatus	err = noErr;
		FSRef	ref;
		err = ::FSFindFolder(kUserDomain,kCurrentUserFolderType,true,&ref);
		if( err != noErr )   _ACErrorNum("FSFindFolder() returned error: ",err,NULL);
		AFileAcc	homeFolder;
		homeFolder.SpecifyByFSRef(ref);
		AFileAcc	prefroot;
		prefroot.SpecifyChild(homeFolder,"Library/Application Support");
		appPrefFolderV2.SpecifyChild(prefroot,"mi");
		//
		if( appPrefFolderV2.Exist() == true )
		{
			//------------------�o�[�W����2.1.11/2.1.12�̃t�H���_�����݂���ꍇ------------------
			
			//���[�_���Z�b�V�����J�n
			AStEditProgressModalSession	modalSession(kEditProgressType_CopyV2AppPrefFolder,false,true,true);
			//�v���O���XIntermediate�\��
			SPI_CheckContinueingEditProgressModalSession(kEditProgressType_CopyV2AppPrefFolder,0,true,-1,1);
			
			//���t�H���_����V�t�H���_�փR�s�[
			appPrefFolderV2.CopyFolderTo(appPrefFolder,true,true);
		}
		else
		{
			//------------------�o�[�W����2.1.11/2.1.12�̃t�H���_�����݂��Ȃ��ꍇ------------------
			
			//==================�o�[�W����2.1.10�ȑO�̃t�H���_����R�s�[==================
			//��appPref�t�H���_�ʒu����
			OSStatus	err = noErr;
			FSRef	ref;
			err = ::FSFindFolder(kUserDomain,kPreferencesFolderType,true,&ref);
			if( err != noErr )   _ACErrorNum("FSFindFolder() returned error: ",err,NULL);
			AFileAcc	prefroot;
			prefroot.SpecifyByFSRef(ref);
			AFileAcc	oldAppPrefFolder;
			oldAppPrefFolder.SpecifyChild(prefroot,"mi");
			if( oldAppPrefFolder.Exist() == true )
			{
				//���[�_���Z�b�V����
				AStEditProgressModalSession	modalSession(kEditProgressType_CopyV2AppPrefFolder,false,true,true);
				//�v���O���XIntermediate�\��
				SPI_CheckContinueingEditProgressModalSession(kEditProgressType_CopyV2AppPrefFolder,0,true,-1,1);
				
				//���t�H���_����V�t�H���_�փR�s�[
				oldAppPrefFolder.CopyFolderTo(appPrefFolder,true,true);
			}
		}
	}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	//win MyDocument����%appdata%�ֈړ�(3.0.0a20)
	if( appPrefFolder.Exist() == false )
	{
		wchar_t	buf[MAX_PATH];
		::SHGetFolderPathW(NULL,CSIDL_PERSONAL,NULL,0,buf);
		AText	path;
		path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
		AFileAcc	mydocfolder;
		mydocfolder.Specify(path);
		AFileAcc	oldAppPrefFolder;
		oldAppPrefFolder.SpecifyChild(mydocfolder,"mi Preferences");
		{
			//���t�H���_����V�t�H���_�փR�s�[
			oldAppPrefFolder.CopyFolderTo(appPrefFolder,true,true);
		}
	}
#endif
	//
	if( appPrefFolder.Exist() == false )   appPrefFolder.CreateFolder();
	AFileAcc	debugFile;
	debugFile.SpecifyChild(appPrefFolder,"debug");
	if( debugFile.Exist() == true )
	{
		NVI_SetEnableDebugMenu(true);
		NVI_SetEnableDebugTraceMode(true);
		NVI_SetEnableTraceLog(true);
		mMiscDebugMode = true;
	}
	/*
	#185
	ATOK2008�̏ꍇ�A���ߋ�؂�ύX���ɉ��L��0��3�̐F�������ɂȂ�̂ŁA��ʂ��t���Ȃ��B
	�y�΍�z
	�O���[���[�h�F
	0: �����D�F
	1: ���D�F���D�F
	2: �D�F�����D�F�i�ϊ����ɍ��ƊD�F�ł͌������t���ɂ����j�����D�F(LightGray)�ł͔w�i���F�Ƃ̋�ʂ��t���ɂ����̂�60%�D�F�ɕύX
	�J���[���[�h�F
	0: �ԐF���D�F
	
	�������D��x�F
	2��3
	0,1��3
	
	ATOK2008
	0: �ŏ��̏�ԁi�܂��ϊ��L�[��S�������Ă��Ȃ��j�A���ߋ�؂�ύX���̎��̕���
	1: (���g�p�H)
	2: �ϊ��Ώۂł͂Ȃ�����
	3: �ϊ��Ώە���
	4: (���g�p�H)
	5: (���g�p�H)
	
	ATOK2007
	0: (���g�p�H)
	1: �ŏ��̏�ԁi�܂��ϊ��L�[��S�������Ă��Ȃ��j�A���ߋ�؂�ύX���̎��̕���
	2: �ϊ��Ώۂł͂Ȃ�����
	3: �ϊ��Ώە���
	4: (���g�p�H)
	5: (���g�p�H)
	
	���Ƃ���
	0: �ŏ��̏�ԁi�܂��ϊ��L�[��S�������Ă��Ȃ��j�i���ߋ�؂�ύX���́A�����I�ɕ��ߋ�؂�ς݂ɂȂ�̂ŁA0�ɂ͂Ȃ�Ȃ��j
	1: (���g�p�H)
	2: �ϊ��Ώۂł͂Ȃ�����
	3: �ϊ��Ώە���
	4: (���g�p�H)
	5: (���g�p�H)
	
	*/
#if IMPLEMENTATION_FOR_MACOSX
	mInlineInputHiliteColorArray_Gray.Add(kColor_Gray);//#185
	mInlineInputHiliteColorArray_Gray.Add(kColor_Gray);//#185
	mInlineInputHiliteColorArray_Gray.Add(kColor_Gray60Percent);//#185
	mInlineInputHiliteColorArray_Gray.Add(kColor_Black);
	mInlineInputHiliteColorArray_Gray.Add(kColor_Black);
	mInlineInputHiliteColorArray_Gray.Add(kColor_Black);
#else
	mInlineInputHiliteColorArray_Gray.Add(kColor_Black);//converted target
	mInlineInputHiliteColorArray_Gray.Add(kColor_Gray);//input
	mInlineInputHiliteColorArray_Gray.Add(kColor_Gray80Percent);//converted non-target
	mInlineInputHiliteColorArray_Gray.Add(kColor_Black);
	mInlineInputHiliteColorArray_Gray.Add(kColor_Black);
	mInlineInputHiliteColorArray_Gray.Add(kColor_Black);
#endif
	
	mInlineInputHiliteColorArray_Color.Add(kColor_Gray);//#185
	mInlineInputHiliteColorArray_Color.Add(kColor_DarkGreen);
	mInlineInputHiliteColorArray_Color.Add(kColor_Blue);
	mInlineInputHiliteColorArray_Color.Add(kColor_Red);
	mInlineInputHiliteColorArray_Color.Add(kColor_Yellow);
	mInlineInputHiliteColorArray_Color.Add(kColor_Black);
	
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	
	//�}���`�t�@�C�������X���b�h����
	{
		AMultiFileFinderFactory	factory(this);
		mMultiFileFinderObjectID = NVI_CreateThread(factory);
	}
	_AInfo("MultiFileFinder thread created.",this);
	
	//
	{
		AImportFileRecognizerFactory	factory(this);//#272
		mImportFileRecognizerObjectID = NVI_CreateThread(factory);
		SPI_GetImportFileRecognizer().NVI_Run();
	}
	_AInfo("ImportFilerRecognizer thread created.",this);
	
	//
	{
		AFTPConnectionFactory	factory(this);
		mFTPConnectionObjectID = NVI_CreateThread(factory);
	}
	_AInfo("FTPConnection thread created.",this);
	
#if IMPLEMENTATION_FOR_MACOSX
	//
	{
		AAppleScriptExecuterFactory	factory(this);
		mAppleScriptExecuterObjectID = NVI_CreateThread(factory);
	}
	_AInfo("AppleScriptExecuter thread created.",this);
#endif
	
	//�c�[���R�}���h�o�^
	RegisterToolCommand("CARET",tc_CARET);
	RegisterToolCommand("MOVECARET-HERE",tc_MOVECARET_HERE);
	RegisterToolCommand("SELECTED",tc_SELECTED);
	RegisterToolCommand("PASTE",tc_PASTE,keyAction_Paste);
	RegisterToolCommand("FILENAME",tc_FILENAME);
	RegisterToolCommand("PATH",tc_PATH);
	RegisterToolCommand("FULL-PATH",tc_FULL_PATH);
	RegisterToolCommand("DATE",tc_DATE);
	RegisterToolCommand("DATE-SHORT",tc_DATE_SHORT);
	RegisterToolCommand("DATE-LONG",tc_DATE_LONG);
	RegisterToolCommand("DATE-6",tc_DATE_6);
	RegisterToolCommand("WAIT",tc_WAIT);
	RegisterToolCommand("WAIT-ACTIVATE",tc_WAIT_ACTIVATE);
	RegisterToolCommand("ASCOMPILE",tc_ASCOMPILE);
	RegisterToolCommand("COMPILE-APPLESCRIPT",tc_COMPILE_APPLESCRIPT);
	RegisterToolCommand("ASRUN",tc_ASRUN);
	RegisterToolCommand("RUN-APPLESCRIPT",tc_RUN_APPLESCRIPT);
	RegisterToolCommand("TOOL",tc_TOOL);
	RegisterToolCommand("AUTOTAB",tc_AUTOTAB);
	RegisterToolCommand("LAUNCH",tc_LAUNCH);
	RegisterToolCommand("DROP",tc_DROP);
	RegisterToolCommand("SENDSELECTED",tc_SENDSELECTED);
	RegisterToolCommand("SEND-SELECTED",tc_SEND_SELECTED);
	RegisterToolCommand("SENDSELECTEDR",tc_SENDSELECTEDR);
	RegisterToolCommand("SEND-SELECTED-REPLACE",tc_SEND_SELECTED_REPLACE);
	RegisterToolCommand("SENDSELECTEDN",tc_SENDSELECTEDN);
	RegisterToolCommand("SEND-SELECTED-NEWDOCUMENT",tc_SEND_SELECTED_NEWDOCUMENT);
	RegisterToolCommand("TIME",tc_TIME);
	RegisterToolCommand("CLOSEDROP",tc_CLOSEDROP);
	RegisterToolCommand("CLOSE-AND-DROP",tc_CLOSE_AND_DROP);
	RegisterToolCommand("IMAGEWIDTH",tc_IMAGEWIDTH);
	RegisterToolCommand("IMAGEHEIGHT",tc_IMAGEHEIGHT);
	RegisterToolCommand("IMAGEHALFWIDTH",tc_IMAGEHALFWIDTH);//#1173
	RegisterToolCommand("IMAGEHALFHEIGHT",tc_IMAGEHALFHEIGHT);//#1173
	RegisterToolCommand("SAVE",tc_SAVE);
	RegisterToolCommand("SAVEAS",tc_SAVEAS);
	RegisterToolCommand("NEWDOCUMENT",tc_NEWDOCUMENT);
	RegisterToolCommand("OPEN-DIALOG",tc_OPEN_DIALOG);
	RegisterToolCommand("CLOSE",tc_CLOSE);
	RegisterToolCommand("CHANGEMODE",tc_CHANGEMODE);
	RegisterToolCommand("CHANGEFONT",tc_CHANGEFONT);
	RegisterToolCommand("CHANGESIZE",tc_CHANGESIZE);
	RegisterToolCommand("CHANGETO-SHIFTJIS",tc_CHANGETO_SHIFTJIS);
	RegisterToolCommand("CHANGETO-JIS",tc_CHANGETO_JIS);
	RegisterToolCommand("CHANGETO-EUC",tc_CHANGETO_EUC);
	RegisterToolCommand("CHANGETO-UCS2",tc_CHANGETO_UCS2);
	RegisterToolCommand("CHANGETO-UTF8",tc_CHANGETO_UTF8);
	RegisterToolCommand("CHANGETO-ISO88591",tc_CHANGETO_ISO88591);
	RegisterToolCommand("CHANGETO-CR",tc_CHANGETO_CR);
	RegisterToolCommand("CHANGETO-LF",tc_CHANGETO_LF);
	RegisterToolCommand("CHANGETO-CRLF",tc_CHANGETO_CRLF);
	RegisterToolCommand("CHANGETO-NOWRAP",tc_CHANGETO_NOWRAP);
	RegisterToolCommand("CHANGETO-WORDWRAP",tc_CHANGETO_WORDWRAP);
	RegisterToolCommand("CHANGETO-LETTERWRAP",tc_CHANGETO_LETTERWRAP);
	RegisterToolCommand("CHANGETAB",tc_CHANGETAB);
	RegisterToolCommand("CHANGECREATOR",tc_CHANGECREATOR);
	RegisterToolCommand("CHANGETYPE",tc_CHANGETYPE);
	RegisterToolCommand("FIND-REGEXP-NEXT",tc_FIND_REGEXP_NEXT);
	RegisterToolCommand("MULTIFILEFIND-REGEXP",tc_MULTIFILEFIND_REGEXP);
	RegisterToolCommand("REPLACE-REGEXP-NEXT",tc_REPLACE_REGEXP_NEXT);
	RegisterToolCommand("REPLACE-REGEXP-SELECTED",tc_REPLACE_REGEXP_SELECTED);
	RegisterToolCommand("REPLACE-REGEXP-AFTERCARET",tc_REPLACE_REGEXP_AFTERCARET);
	RegisterToolCommand("REPLACE-REGEXP-ALL",tc_REPLACE_REGEXP_ALL);
	RegisterToolCommand("INDENT",tc_INDENT,keyAction_indent);//#249
	RegisterToolCommand("SHIFT-LEFT",tc_SHIFT_LEFT,keyAction_shiftleft);
	RegisterToolCommand("SHIFT-RIGHT",tc_SHIFT_RIGHT,keyAction_shiftright);
	RegisterToolCommand("ABBREV-NEXT",tc_ABBREV_NEXT,keyAction_abbrevnext);
	RegisterToolCommand("ABBREV-PREV",tc_ABBREV_PREV,keyAction_abbrevprev);
	RegisterToolCommand("DELETE-NEXT",tc_DELETE_NEXT,keyAction_deletenext);
	RegisterToolCommand("DELETE-PREV",tc_DELETE_PREV,keyAction_deleteprev);
	RegisterToolCommand("DELETE-ENDOFLINE",tc_DELETE_ENDOFLINE,keyAction_deletelineend);
	RegisterToolCommand("DELETE-SELECTED",tc_DELETE_SELECTED);
	RegisterToolCommand("MOVECARET-UP",tc_MOVECARET_UP,keyAction_caretup);
	RegisterToolCommand("MOVECARET-DOWN",tc_MOVECARET_DOWN,keyAction_caretdown);
	RegisterToolCommand("MOVECARET-LEFT",tc_MOVECARET_LEFT,keyAction_caretleft);
	RegisterToolCommand("MOVECARET-RIGHT",tc_MOVECARET_RIGHT,keyAction_caretright);
	RegisterToolCommand("MOVECARET-HOME",tc_MOVECARET_HOME,keyAction_carethome);
	RegisterToolCommand("MOVECARET-END",tc_MOVECARET_END,keyAction_caretend);
	RegisterToolCommand("MOVECARET-ENDOFLINE",tc_MOVECARET_ENDOFLINE,keyAction_caretlineend);
	RegisterToolCommand("MOVECARET-STARTOFLINE",tc_MOVECARET_STARTOFLINE,keyAction_caretlinestart);
	RegisterToolCommand("MOVECARET-PREVWORD",tc_MOVECARET_PREVWORD,keyAction_previousword);
	RegisterToolCommand("MOVECARET-NEXTWORD",tc_MOVECARET_NEXTWORD,keyAction_nextword);
	RegisterToolCommand("MOVECARET-LINE",tc_MOVECARET_LINE);
	RegisterToolCommand("MOVECARET-PARAGRAPH",tc_MOVECARET_PARAGRAPH);
	RegisterToolCommand("MOVECARET-COLUMN",tc_MOVECARET_COLUMN);
	RegisterToolCommand("SELECT-UP",tc_SELECT_UP,keyAction_selectup);
	RegisterToolCommand("SELECT-DOWN",tc_SELECT_DOWN,keyAction_selectdown);
	RegisterToolCommand("SELECT-LEFT",tc_SELECT_LEFT,keyAction_selectleft);
	RegisterToolCommand("SELECT-RIGHT",tc_SELECT_RIGHT,keyAction_selectright);
	RegisterToolCommand("SELECT-HOME",tc_SELECT_HOME,keyAction_selecthome);
	RegisterToolCommand("SELECT-END",tc_SELECT_END,keyAction_selectend);
	RegisterToolCommand("SELECT-ENDOFLINE",tc_SELECT_ENDOFLINE,keyAction_selectlineend);
	RegisterToolCommand("SELECT-STARTOFLINE",tc_SELECT_STARTOFLINE,keyAction_selectlinestart);
	RegisterToolCommand("SELECT-PREVWORD",tc_SELECT_PREVWORD,keyAction_selectprevword);
	RegisterToolCommand("SELECT-NEXTWORD",tc_SELECT_NEXTWORD,keyAction_selectnextword);
	RegisterToolCommand("SELECT-ALL",tc_SELECT_ALL,keyAction_selectAll);
	RegisterToolCommand("SCROLL-UP",tc_SCROLL_UP,keyAction_scrollup);
	RegisterToolCommand("SCROLL-DOWN",tc_SCROLL_DOWN,keyAction_scrolldown);
	RegisterToolCommand("SCROLL-LEFT",tc_SCROLL_LEFT,keyAction_scrollleft);
	RegisterToolCommand("SCROLL-RIGHT",tc_SCROLL_RIGHT,keyAction_scrollright);
	RegisterToolCommand("SCROLL-HOME",tc_SCROLL_HOME,keyAction_home);
	RegisterToolCommand("SCROLL-END",tc_SCROLL_END,keyAction_end);
	RegisterToolCommand("SCROLL-PREVPAGE",tc_SCROLL_PREVPAGE,keyAction_pageup);
	RegisterToolCommand("SCROLL-NEXTPAGE",tc_SCROLL_NEXTPAGE,keyAction_pagedown);
	RegisterToolCommand("MOVECARET-PREVPAGE",tc_SCROLL_MOVECARET_PREVPAGE,keyAction_pageupwithcaret);
	RegisterToolCommand("MOVECARET-NEXTPAGE",tc_SCROLL_MOVECARET_NEXTPAGE,keyAction_pagedownwithcaret);
	RegisterToolCommand("UNDO",tc_UNDO,keyAction_Undo);
	RegisterToolCommand("CUT",tc_CUT,keyAction_Cut);
	RegisterToolCommand("COPY",tc_COPY,keyAction_Copy);
	RegisterToolCommand("CUT-ENDOFLINE",tc_CUT_ENDOFLINE,keyAction_cutlineend);
	RegisterToolCommand("KEYTOOL",tc_KEYTOOL);
	RegisterToolCommand("RETURN",tc_RETURN,keyAction_return);
	RegisterToolCommand("INDENT-RETURN",tc_INDENT_RETURN,keyAction_indentreturn);
	RegisterToolCommand("TAB",tc_TAB,keyAction_tab);
	RegisterToolCommand("INDENT-TAB",tc_INDENT_TAB,keyAction_indenttab);
	RegisterToolCommand("SPACE",tc_SPACE,keyAction_space);
	RegisterToolCommand("URL",tc_URL);
	RegisterToolCommand("TITLE",tc_TITLE);
	RegisterToolCommand("REDO",tc_REDO,keyAction_Redo);
	RegisterToolCommand("MULTIFILEFIND-DIALOG",tc_MULTIFILEFIND_DIALOG);
	RegisterToolCommand("FIND-DIALOG",tc_FIND_DIALOG);
	RegisterToolCommand("FIND-REGEXP-PREV",tc_FIND_REGEXP_PREV);
	RegisterToolCommand("FIND-SELECTED-NEXT",tc_FIND_SELECTED_NEXT);
	RegisterToolCommand("FIND-SELECTED-PREV",tc_FIND_SELECTED_PREV);
	RegisterToolCommand("MULTIFILEFIND-SELECTED",tc_MULTIFILEFIND_SELECTED);
	RegisterToolCommand("OPEN-SELECTED",tc_OPEN_SELECTED);
	RegisterToolCommand("COMMAND",tc_COMMAND);
	RegisterToolCommand("KEY",tc_KEY);
	RegisterToolCommand("OPEN",tc_OPEN);
	RegisterToolCommand("YEAR",tc_YEAR);
	RegisterToolCommand("YEAR-2",tc_YEAR_2);
	RegisterToolCommand("YEAR-4",tc_YEAR_4);
	RegisterToolCommand("MONTH",tc_MONTH);
	RegisterToolCommand("MONTH-2",tc_MONTH_2);
	RegisterToolCommand("DAY",tc_DAY);
	RegisterToolCommand("DAY-2",tc_DAY_2);
	RegisterToolCommand("SELECT-WHOLE-PARAGRAPH",tc_SELECT_WHOLE_PARAGRAPH,keyAction_selectParagraph);//#913
	RegisterToolCommand("SELECT-WHOLE-LINE",tc_SELECT_WHOLE_LINE,keyAction_selectLine);//#913
	RegisterToolCommand("CORRESPONDENCE",tc_CORRESPONDENCE,keyAction_correspondence);
	RegisterToolCommand("RECENTER",tc_RECENTER,keyAction_recenter);
	RegisterToolCommand("OPEN-URL",tc_OPEN_URL);
	RegisterToolCommand("OPEN-FILE",tc_OPEN_FILE);
	RegisterToolCommand("SELECTMODE",tc_SELECTMODE,keyAction_selectmode);
	RegisterToolCommand("CUT-STARTOFLINE",tc_CUT_STARTOFLINE,keyAction_cutlinestart);
	RegisterToolCommand("SELECT-BRACE",tc_SELECT_BRACE,keyAction_selectbrace);
	RegisterToolCommand("SAMELETTER-UP",tc_SAMELETTER_UP,keyAction_sameletterup);
	RegisterToolCommand("DEBUG1",tc_DEBUG1);
	RegisterToolCommand("BROWSE",tc_BROWSE);
	RegisterToolCommand("DEBUGINFO",tc_DEBUGINFO);
	RegisterToolCommand("DEBUG2",tc_DEBUG2);
	RegisterToolCommand("DUPLICATE-LINE",tc_DUPLICATE_LINE,keyAction_duplicateline);
	RegisterToolCommand("INSERT-LINE",tc_INSERT_LINE,keyAction_insertline);
	RegisterToolCommand("COMPILE-TEX",tc_COMPILE_TEX);
	RegisterToolCommand("PREVIEW-TEX",tc_PREVIEW_TEX);
	RegisterToolCommand("COMPILEPREVIEW-TEX",tc_COMPILEPREVIEW_TEX);
	RegisterToolCommand("DIRECTORY",tc_DIRECTORY);
	RegisterToolCommand("FILENAME-NOSUFFIX",tc_FILENAME_NOSUFFIX);
	RegisterToolCommand("SWITCH-WINDOW",tc_SWITCH_WINDOW);
	RegisterToolCommand("SWITCH-NEXTWINDOW",tc_SWITCH_WINDOW);//#552
	RegisterToolCommand("SWITCH-PREVWINDOW",tc_SWITCH_PREVWINDOW);//#552
	RegisterToolCommand("MOVECARET-ENDOFPARAGRAPH",tc_MOVECARET_ENDOFPARAGRAPH,keyAction_caretparagraphend);
	RegisterToolCommand("MOVECARET-STARTOFPARAGRAPH",tc_MOVECARET_STARTOFPARAGRAPH,keyAction_caretparagraphstart);
	RegisterToolCommand("DAYOFTHEWEEK",tc_DAYOFTHEWEEK);
	RegisterToolCommand("GT3",tc_GT3);
	RegisterToolCommand("LT3",tc_LT3);
	RegisterToolCommand("GT1",tc_GT1);
	RegisterToolCommand("LT1",tc_LT1);
	RegisterToolCommand("MOVECARET-PREVWORD-LINESTOP",tc_MOVECARET_PREVWORD_LINESTOP,keyAction_previousword_linestop);
	RegisterToolCommand("MOVECARET-NEXTWORD-LINESTOP",tc_MOVECARET_NEXTWORD_LINESTOP,keyAction_nextword_linestop);
	RegisterToolCommand("DIRLIST",tc_DIR_LIST);
	RegisterToolCommand("SELECT-PREVWORD-LINESTOP",tc_SELECT_PREVWORD_LINESTOP,keyAction_selectpreviousword_linestop);
	RegisterToolCommand("SELECT-NEXTWORD-LINESTOP",tc_SELECT_NEXTWORD_LINESTOP,keyAction_selectnextword_linestop);
	RegisterToolCommand("FIND-REGEXP-FROMFIRST",tc_FIND_REGEXP_FROMFIRST);
	RegisterToolCommand("CHANGETO",tc_CHANGETO);
	RegisterToolCommand("MODIFYTO",tc_MODIFYTO);
	RegisterToolCommand("SWAPLINE",tc_SWAPLINE);
	RegisterToolCommand("SWAPLETTER",tc_SWAPLETTER,keyAction_swapLetter);//#1154
	RegisterToolCommand("UNITTEST",tc_UNITTEST);
	RegisterToolCommand("MULTIFILEFIND-SELECTED-INPROJECT",tc_MULTIFILEFIND_SELECTED_INPROJECT);
	RegisterToolCommand("SWITCH-NEXTSPLITVIEW",tc_SWITCH_NEXTSPLITVIEW);
	RegisterToolCommand("SWITCH-PREVSPLITVIEW",tc_SWITCH_PREVSPLITVIEW);
	RegisterToolCommand("SPLITVIEW",tc_SPLITVIEW);
	RegisterToolCommand("CLOSE-SPLITVIEW",tc_CLOSE_SPLITVIEW);
	RegisterToolCommand("SHOW-DEFINITION",tc_SHOW_DEFINITION);
	RegisterToolCommand("COPY-WITHHTMLCOLOR",tc_COPY_WITHHTMLCOLOR);//R0015
	RegisterToolCommand("NUMBER-REGEXP-SELECTED",tc_NUMBER_REGEXP_SELECTED);//R0189
	RegisterToolCommand("TRANSLITERATE-SELECTED",tc_TRANSLITERATE_SELECTED);//R0017
	RegisterToolCommand("TRANSLITERATE-REGEXP-SELECTED",tc_TRANSLITERATE_REGEXP_SELECTED);//R0017
	RegisterToolCommand("DUMP-UNICODE-CHARCODE",tc_DUMP_UNICODE_CHARCODE);//For Debug
	RegisterToolCommand("DELETE-ENDOFPARAGRAPH",tc_DELETE_ENDOFPARAGRAPH,keyAction_deleteparagraphend);//R0213
	RegisterToolCommand("CUT-ENDOFPARAGRAPH",tc_CUT_ENDOFPARAGRAPH,keyAction_cutparagraphend);//R0213
	RegisterToolCommand("ADDITIONAL-CUT-ENDOFPARAGRAPH",tc_ADDITIONAL_CUT_ENDOFPARAGRAPH,keyAction_additionalcutparagraphend);//R0213
	RegisterToolCommand("ADDITIONAL-CUT",tc_ADDITIONAL_CUT,keyAction_additionalcut);//R0213
	RegisterToolCommand("ADDITIONAL-COPY",tc_ADDITIONAL_COPY,keyAction_additionalcopy);//R0213
	RegisterToolCommand("TOOLINDENT",tc_TOOLINDENT);//R0000
	RegisterToolCommand("CONCATALL-INPROJECT",tc_CONCATALL_INPROJECT);//R0000
	RegisterToolCommand("SCM-COMPARE-WITH-LATEST",tc_COMPARE_WITH_LATEST);//R0006
	RegisterToolCommand("MOVECARET-NEXTDIFF",tc_MOVECARET_NEXTDIFF,keyAction_nextdiff);//R0006
	RegisterToolCommand("MOVECARET-PREVDIFF",tc_MOVECARET_PREVDIFF,keyAction_prevdiff);//R0006
	RegisterToolCommand("ARROWTODEF",tc_ARROWTODEF);//RC1
	RegisterToolCommand("IDINFO",tc_IDINFO,keyAction_idinfo);//RC2
	RegisterToolCommand("SHOWHIDE-CROSSCARET",tc_SHOWHIDE_CROSSCARET);//R0185
	RegisterToolCommand("HOUR",tc_HOUR);//R0116
	RegisterToolCommand("HOUR-2",tc_HOUR_2);//R0116
	RegisterToolCommand("HOUR12",tc_HOUR12);//R0116
	RegisterToolCommand("HOUR12-2",tc_HOUR12_2);//R0116
	RegisterToolCommand("AMPM",tc_AMPM);//R0116
	RegisterToolCommand("MINUTE",tc_MINUTE);//R0116
	RegisterToolCommand("MINUTE-2",tc_MINUTE_2);//R0116
	RegisterToolCommand("SECOND",tc_SECOND);//R0116
	RegisterToolCommand("SECOND-2",tc_SECOND_2);//R0116
	RegisterToolCommand("DICT",tc_DICT);//R0231
	RegisterToolCommand("DICT-POPUP",tc_DICT_POPUP);//R0231
	RegisterToolCommand("FIX-IDINFO",tc_FIX_IDINFO);//RC2
	RegisterToolCommand("EDIT-MYCOMMENT",tc_EDIT_MYCOMMENT);//RC3
	RegisterToolCommand("NAV-PREV",tc_NAV_PREV);//#146
	RegisterToolCommand("NAV-NEXT",tc_NAV_NEXT);//#146
	RegisterToolCommand("TOOLBARICON",tc_TOOLBARICON);//#232
	RegisterToolCommand("HIGHLIGHT-SELECTED",tc_HIGHLIGHT_SELECTED);//#232
	RegisterToolCommand("HIGHLIGHT-CLEAR",tc_HIGHLIGHT_CLEAR);//#232
	RegisterToolCommand("INDEXWINDOW-NEXT",tc_INDEXWINDOW_NEXT);//#232
	RegisterToolCommand("INDEXWINDOW-PREV",tc_INDEXWINDOW_PREV);//#232
	RegisterToolCommand("SELECT-WHOLE-PARAGRAPH-NOLINEEND",tc_SELECT_WHOLE_PARAGRAPH_NOLINEEND);//#223
	RegisterToolCommand("DELETE-WHOLE-PARAGRAPH",tc_DELETE_WHOLE_PARAGRAPH,keyAction_deletewholeparagraph);//#124
	RegisterToolCommand("NEWDOCUMENT-MOVETARGET",tc_NEWDOCUMENT_MOVETARGET);//#56
	RegisterToolCommand("NEWDOCUMENT-SAMEMODE",tc_NEWDOCUMENT_SAMEMODE);//#56
	RegisterToolCommand("NEWDOCUMENT-SAMEMODE-MOVETARGET",tc_NEWDOCUMENT_SAMEMODE_MOVETARGET);//#56
	RegisterToolCommand("SELECT-NEXTHEADER",tc_SELECT_NEXTHEADER);//#150
	RegisterToolCommand("SELECT-PREVHEADER",tc_SELECT_PREVHEADER);//#150
	RegisterToolCommand("SELECTED-PLACEHOLDER",tc_SELECTED_PLACEHOLDER);//#110
	RegisterToolCommand("SETVAR-SELECTED",tc_SETVAR_SELECTED);//#50
	RegisterToolCommand("GETVAR",tc_GETVAR);//#50
	RegisterToolCommand("TAB-BYSPACE",tc_TAB_BYSPACE,keyAction_tab_byspace);//#249
	RegisterToolCommand("INDENT-BYSPACE-TAB",tc_INDENT_BYSPACE_TAB,keyAction_indent_byspace_tab);//#249
	RegisterToolCommand("INDENT-BYSPACE-TAB-BYSPACE",tc_INDENT_BYSPACE_TAB_BYSPACE,keyAction_indent_byspace_tab_byspace);//#249
	RegisterToolCommand("INDENT-BYSPACE",tc_INDENT_BYSPACE,keyAction_indent_byspace);//#249
	RegisterToolCommand("INDENT-BYSPACE-RETURN",tc_INDENT_BYSPACE_RETURN);//#249
	RegisterToolCommand("INDENT-ONLYNEWLINE-RETURN",tc_INDENT_ONLYNEWLINE_RETURN);//#249
	RegisterToolCommand("INDENT-ONLYNEWLINE-BYSPACE-RETURN",tc_INDENT_ONLYNEWLINE_BYSPACE_RETURN);//#249
	RegisterToolCommand("CHANGEINDENT",tc_CHANGEINDENT);//#251
	RegisterToolCommand("SHOWHIDE-SUBPANECOLUMN",tc_SHOWHIDE_SUBPANECOLUMN);//#212
	RegisterToolCommand("SHOWHIDE-INFOPANECOLUMN",tc_SHOWHIDE_INFOPANECOLUMN);//#291
	RegisterToolCommand("CHANGEFONT-MODEDEFAULT",tc_CHANGEFONT_MODEDEFAULT);//#354
	RegisterToolCommand("REVERTFONT",tc_REVERTFONT);//#354
	RegisterToolCommand("SWITCH-NEXTTAB",tc_SWITCH_NEXTTAB,keyAction_switch_nexttab);//#357
	RegisterToolCommand("SWITCH-PREVTAB",tc_SWITCH_PREVTAB,keyAction_switch_prevtab);//#357
	RegisterToolCommand("URL-NOURLENCODE",tc_URL_NOURLENCODE);//#363
	RegisterToolCommand("KEYRECORD-START",tc_KEYRECORD_START);//#390
	RegisterToolCommand("KEYRECORD-START-WITHWINDOW",tc_KEYRECORD_START_WITHWINDOW);//#390
	RegisterToolCommand("KEYRECORD-STOP",tc_KEYRECORD_STOP);//#390
	RegisterToolCommand("KEYRECORD-PLAY",tc_KEYRECORD_PLAY);//#390
	RegisterToolCommand("TABTOWINDOW",tc_TABTOWINDOW);//#389
	RegisterToolCommand("WINDOWTOTAB",tc_WINDOWTOTAB);//#389
	RegisterToolCommand("SELECT-CORRESPONDENCE",tc_SELECT_CORRESPONDENCE);//#359
	RegisterToolCommand("ADDITIONAL-COPY-TEXT",tc_ADDITIONAL_COPY_TEXT);//#425
	RegisterToolCommand("FIND-NEXT",tc_FIND_NEXT, keyAction_findnext);//win #1397
	RegisterToolCommand("FIND-PREV",tc_FIND_PREV, keyAction_findprev);//win #1397
	RegisterToolCommand("SETFINDTEXT-SELECTED",tc_SETFINDTEXT_SELECTED);//win
	RegisterToolCommand("SHOW-PRINTDIALOG",tc_SHOW_PRINTDIALOG);//win
	RegisterToolCommand("SHOW-FINDDIALOG",tc_SHOW_FINDDIALOG);//win
	RegisterToolCommand("SHOW-MULTIFILEFINDDIALOG",tc_SHOW_MULTIFINDDIALOG);//win
	RegisterToolCommand("FULLSCREEN",tc_FULLSCREEN);//#404
	RegisterToolCommand("SCM-COMMIT",tc_SCM_COMMIT);//#455
	RegisterToolCommand("DIFF-REPOSITORY",tc_DIFF_REPOSITORY);//#379
	RegisterToolCommand("DAYOFWEEK",tc_DAYOFWEEK);//#537
	RegisterToolCommand("DAYOFWEEK-LONG",tc_DAYOFWEEK_LONG);//#537
	RegisterToolCommand("HANDTOOL",tc_HANDTOOL);//#606
	RegisterToolCommand("SHOW-LINEDIALOG",tc_SHOW_LINEDIALOG,keyAction_linedialog);//#594
	RegisterToolCommand("AUTOINDENTMODE",tc_AUTOINDENTMODE,keyAction_autoindentmode);//#638
	RegisterToolCommand("DEBUG_CHECKLINEINFO",tc_DEBUG_CHECKLINEINFO);//#699
	RegisterToolCommand("FIND-TEXT-NEXT",tc_FIND_TEXT_NEXT);//#887
	RegisterToolCommand("FIND-TEXT-PREV",tc_FIND_TEXT_PREV);//#887
	RegisterToolCommand("FIND-TEXT-FROMFIRST",tc_FIND_TEXT_FROMFIRST);//#887
	RegisterToolCommand("REPLACE-TEXT-NEXT",tc_REPLACE_TEXT_NEXT);//#887
	RegisterToolCommand("REPLACE-TEXT-INSELECTION",tc_REPLACE_TEXT_INSELECTION);//#887
	RegisterToolCommand("REPLACE-TEXT-AFTERCARET",tc_REPLACE_TEXT_AFTERCARET);//#887
	RegisterToolCommand("REPLACE-TEXT-ALL",tc_REPLACE_TEXT_ALL);//#887
	RegisterToolCommand("REPLACE-TEXT-SELECTEDIFMATCHED",tc_REPLACE_TEXT_SELECTEDTEXTONLY);//#887
	RegisterToolCommand("PREVIEW-EXECJS-SELECTED",tc_PREVIEW_EXECJS_SELECTED);//#734
	RegisterToolCommand("DELETE-STARTOFLINE",tc_DELETE_STARTOFLINE,keyAction_deletelinestart);//#913
	RegisterToolCommand("DELETE-STARTOFPARAGRAPH",tc_DELETE_STARTOFPARAGRAPH,keyAction_deleteparagraphstart);//#913
	RegisterToolCommand("SELECT-STARTOFPARAGRAPH",tc_SELECT_STARTOFPARAGRAPH,keyAction_selectparagraphstart);//#913
	RegisterToolCommand("SELECT-ENDOFPARAGRAPH",tc_SELECT_ENDOFPARAGRAPH,keyAction_selectparagraphend);//#913
	RegisterToolCommand("SETMARK",tc_SETMARK,keyAction_setMark);//#913
	RegisterToolCommand("SWAPMARK",tc_SWAPMARK,keyAction_swapWithMark);//#913
	RegisterToolCommand("SELECT-MARK",tc_SELECT_MARK,keyAction_selectToMark);//#913
	RegisterToolCommand("DELETE-MARK",tc_DELETE_MARK,keyAction_deleteToMark);//#913
	RegisterToolCommand("DELETE-PREV-DECOMP",tc_DELETE_PREV_DECOMP,keyAction_deleteBackwardByDecomposingPreviousCharacter);//#913
	RegisterToolCommand("YANK",tc_YANK,keyAction_yank);//#913
	RegisterToolCommand("SELECT-WORD",tc_SELECT_WORD,keyAction_selectWord);//#913
	RegisterToolCommand("MACRO1",tc_MACRO1,keyAction_macro1);//#724
	RegisterToolCommand("HINT",tc_HINT);
	RegisterToolCommand("FIND-EXTRACT",tc_FIND_EXTRACT);
	RegisterToolCommand("BROWSE_PANDOC",tc_BROWSE_PANDOC);//#1286
	RegisterToolCommand("MOVECARET-PREVSTARTOFPARAGRAPH",tc_MOVECARET_PREVSTARTOFPARAGRAPH,keyAction_caretprevparagraphstart);//#1399
	RegisterToolCommand("MOVECARET-NEXTENDOFPARAGRAPH",tc_MOVECARET_NEXTENDOFPARAGRAPH,keyAction_caretnextparagraphend);//#1399
	RegisterToolCommand("SELECT-PREVSTARTOFPARAGRAPH",tc_SELECT_PREVSTARTOFPARAGRAPH,keyAction_selectprevparagraphstart);//#1399
	RegisterToolCommand("SELECT-NEXTENDOFPARAGRAPH",tc_SELECT_NEXTENDOFPARAGRAPH,keyAction_selectnextparagraphend);//#1399
	//#724 LoadToolbarIcon();
	_AInfo("AApp::AApp() done.",this);
}

//�f�X�g���N�^
AApp::~AApp()
{
	ABaseFunction::DoObjectArrayItemTrashDelete();
	/* #92 NVIDO_Quit()�ֈړ�
	Quit_Phase_UI();
	Quit_Phase_Data();
	*/
}

/**
�C���[�W���[�h
*/
void	AApp::SPI_LoadImageFiles( const ABool inRemoveOld )
{
	/*#1316
	ABool	useCustom = false;
	AFileAcc	customskinfolder;
	//�J�X�^��skin���擾
	AText	skinName;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kSkinName,skinName);
	//�J�X�^��skin�t�H���_�擾
	if( skinName.GetItemCount() > 0 )
	{
		AFileAcc	skinroot;
		SPI_GetUserSkinFolder(skinroot);
		customskinfolder.SpecifyChild(skinroot,skinName);
		//�J�X�^��skin�t�H���_���݂��Ă�����A�J�X�^���̕����g�p����t���OON
		if( customskinfolder.Exist() == true && customskinfolder.IsFolder() == true )
		{
			useCustom = true;
		}
	}
	*/
	
	//Image�ǂݍ���
	
	//�F���[�h
	//#1316 LoadColors(useCustom,customskinfolder);
	
	//�C���[�W�t�@�C���ǂݍ���
	//#130
	LoadImageFile("iconSwCancel.png",kImageID_iconSwCancel,inRemoveOld);
	LoadImageFile("iconSwCancel_h.png",kImageID_iconSwCancel_h,inRemoveOld);
	LoadImageFile("iconSwCancel_p.png",kImageID_iconSwCancel_p,inRemoveOld);
	LoadImageFile("iconSwSearch.png",kImageID_iconSwSearch,inRemoveOld);
	
	/*#1316
	//Rect20�{�^���C���[�W
	LoadImageFile("btn20Rect_1.png",kImageID_btn20Rect_1,inRemoveOld);
	LoadImageFile("btn20Rect_2.png",kImageID_btn20Rect_2,inRemoveOld);
	LoadImageFile("btn20Rect_3.png",kImageID_btn20Rect_3,inRemoveOld);
	LoadImageFile("btn20Rect_h_1.png",kImageID_btn20Rect_h_1,inRemoveOld);
	LoadImageFile("btn20Rect_h_2.png",kImageID_btn20Rect_h_2,inRemoveOld);
	LoadImageFile("btn20Rect_h_3.png",kImageID_btn20Rect_h_3,inRemoveOld);
	LoadImageFile("btn20Rect_p_1.png",kImageID_btn20Rect_p_1,inRemoveOld);
	LoadImageFile("btn20Rect_p_2.png",kImageID_btn20Rect_p_2,inRemoveOld);
	LoadImageFile("btn20Rect_p_3.png",kImageID_btn20Rect_p_3,inRemoveOld);
	
	//RoundedRect20�{�^���C���[�W
	LoadImageFile("btn20RoundedRect_1.png",kImageID_btn20RoundedRect_1,inRemoveOld);
	LoadImageFile("btn20RoundedRect_2.png",kImageID_btn20RoundedRect_2,inRemoveOld);
	LoadImageFile("btn20RoundedRect_3.png",kImageID_btn20RoundedRect_3,inRemoveOld);
	LoadImageFile("btn20RoundedRect_h_1.png",kImageID_btn20RoundedRect_h_1,inRemoveOld);
	LoadImageFile("btn20RoundedRect_h_2.png",kImageID_btn20RoundedRect_h_2,inRemoveOld);
	LoadImageFile("btn20RoundedRect_h_3.png",kImageID_btn20RoundedRect_h_3,inRemoveOld);
	LoadImageFile("btn20RoundedRect_p_1.png",kImageID_btn20RoundedRect_p_1,inRemoveOld);
	LoadImageFile("btn20RoundedRect_p_2.png",kImageID_btn20RoundedRect_p_2,inRemoveOld);
	LoadImageFile("btn20RoundedRect_p_3.png",kImageID_btn20RoundedRect_p_3,inRemoveOld);
	
	//Rect16�{�^���C���[�W
	LoadImageFile("btn16Rect_1.png",kImageID_btn16Rect_1,inRemoveOld);
	LoadImageFile("btn16Rect_2.png",kImageID_btn16Rect_2,inRemoveOld);
	LoadImageFile("btn16Rect_3.png",kImageID_btn16Rect_3,inRemoveOld);
	LoadImageFile("btn16Rect_h_1.png",kImageID_btn16Rect_h_1,inRemoveOld);
	LoadImageFile("btn16Rect_h_2.png",kImageID_btn16Rect_h_2,inRemoveOld);
	LoadImageFile("btn16Rect_h_3.png",kImageID_btn16Rect_h_3,inRemoveOld);
	LoadImageFile("btn16Rect_p_1.png",kImageID_btn16Rect_p_1,inRemoveOld);
	LoadImageFile("btn16Rect_p_2.png",kImageID_btn16Rect_p_2,inRemoveOld);
	LoadImageFile("btn16Rect_p_3.png",kImageID_btn16Rect_p_3,inRemoveOld);
	
	//Rect16Footer�{�^���C���[�W
	LoadImageFile("panel16Footer_1.png",kImageID_panel16Footer_1,inRemoveOld);
	LoadImageFile("panel16Footer_2.png",kImageID_panel16Footer_2,inRemoveOld);
	LoadImageFile("panel16Footer_3.png",kImageID_panel16Footer_3,inRemoveOld);
	*/
	//�|�b�v�A�b�v���j���[�p�g���C�A���O��
	LoadImageFile("iconSwTriangles1.png",kImageID_iconSwTriangles,inRemoveOld);
	
	//�܂肽���ݎO�p�`
	LoadImageFile("barSwCursorRight.png",kImageID_barSwCursorRight,inRemoveOld);
	LoadImageFile("barSwCursorRight_h.png",kImageID_barSwCursorRight_h,inRemoveOld);
	LoadImageFile("barSwCursorRight_d.png",kImageID_barSwCursorRight_p,inRemoveOld);
	LoadImageFile("barSwCursorDown.png",kImageID_barSwCursorDown,inRemoveOld);
	LoadImageFile("barSwCursorDown_h.png",kImageID_barSwCursorDown_h,inRemoveOld);
	LoadImageFile("barSwCursorDown_d.png",kImageID_barSwCursorDown_p,inRemoveOld);
	
	/*#1316
	//�t�@�C���C���[�W
	LoadImageFile("iconSwDocumentOutline.png",kImageID_iconSwFile,inRemoveOld);
	
	//�X�N���[���o�[���{�^��
	LoadImageFile("frameScrlbarTop_106.png",kImageID_frameScrlbarTop_106,inRemoveOld);
	LoadImageFile("framescrlbartop_107.png",kImageID_frameScrlbarTop_107,inRemoveOld);
	*/
	
	//�T�C�h�o�[�^�C�g���A�C�R��
	LoadImageFile("iconSwBox_ProjectFolder.png", kImageID_iconSwBox_ProjectFolder,inRemoveOld);
	LoadImageFile("iconSwBox_RecentlyOpened.png", kImageID_iconSwBox_RecentlyOpened,inRemoveOld);
	LoadImageFile("iconSwBox_SameFolder.png", kImageID_iconSwBox_SameFolder,inRemoveOld);
	LoadImageFile("iconSwBox_Remote.png", kImageID_iconSwBox_Remote,inRemoveOld);//#1316
	LoadImageFile("iconSwTree.png",kImageID_iconSwTree,inRemoveOld);
	LoadImageFile("iconSwList.png",kImageID_iconSwList,inRemoveOld);
	LoadImageFile("iconSwMarkerPen.png",kImageID_iconSwMarkerPen,inRemoveOld);
	LoadImageFile("iconSwPreview.png",kImageID_iconSwPreview,inRemoveOld);
	LoadImageFile("iconSwKeyWord.png",kImageID_iconSwKeyWord,inRemoveOld);
	LoadImageFile("iconSwTextCount.png",kImageID_iconSwTextCount,inRemoveOld);
	
	//���C���E�C���h�E���e�A�C�R��
	LoadImageFile("iconSwCompare.png",kImageID_iconSwCompare,inRemoveOld);
	LoadImageFile("iconSwReplace.png",kImageID_iconSwReplace,inRemoveOld);
	LoadImageFile("iconSwShow.png",kImageID_iconSwShow,inRemoveOld);
	LoadImageFile("iconSwDocument.png",kImageID_iconSwDocument,inRemoveOld);
	
	/*#1316
	//�^�u�w�i
	LoadImageFile("bgTabBar_1.png",kImageID_Tab0,inRemoveOld);
	LoadImageFile("bgTabBar_2.png",kImageID_Tab1,inRemoveOld);
	LoadImageFile("bgTabBar_3.png",kImageID_Tab2,inRemoveOld);
	LoadImageFile("bgTabBar_4.png",kImageID_Tab3,inRemoveOld);
	LoadImageFile("bgTabBar_5.png",kImageID_Tab4,inRemoveOld);
	LoadImageFile("bgTabBar_6.png",kImageID_Tab5,inRemoveOld);
	LoadImageFile("bgTabBar_7.png",kImageID_Tab6,inRemoveOld);
	LoadImageFile("bgTabBar_8.png",kImageID_Tab7,inRemoveOld);
	LoadImageFile("bgTabBar_9.png",kImageID_Tab8,inRemoveOld);
	
	//�^�u�{�^���A�N�e�B�u
	LoadImageFile("tabFront_1.png",kImageID_tabFront_1,inRemoveOld);
	LoadImageFile("tabFront_2.png",kImageID_tabFront_2,inRemoveOld);
	LoadImageFile("tabFront_3.png",kImageID_tabFront_3,inRemoveOld);
	LoadImageFile("tabFront_h_1.png",kImageID_tabFront_h_1,inRemoveOld);
	LoadImageFile("tabFront_h_2.png",kImageID_tabFront_h_2,inRemoveOld);
	LoadImageFile("tabFront_h_3.png",kImageID_tabFront_h_3,inRemoveOld);
	//�^�u�{�^��2nd�A�N�e�B�u
	LoadImageFile("tabNext_1.png",kImageID_tabNext_1,inRemoveOld);
	LoadImageFile("tabNext_2.png",kImageID_tabNext_2,inRemoveOld);
	LoadImageFile("tabNext_3.png",kImageID_tabNext_3,inRemoveOld);
	LoadImageFile("tabNext_h_1.png",kImageID_tabNext_h_1,inRemoveOld);
	LoadImageFile("tabNext_h_2.png",kImageID_tabNext_h_2,inRemoveOld);
	LoadImageFile("tabNext_h_3.png",kImageID_tabNext_h_3,inRemoveOld);
	//�^�u�{�^����A�N�e�B�u
	LoadImageFile("tabOther_h_1.png",kImageID_tabOther_h_1,inRemoveOld);
	LoadImageFile("tabOther_h_2.png",kImageID_tabOther_h_2,inRemoveOld);
	LoadImageFile("tabOther_h_3.png",kImageID_tabOther_h_3,inRemoveOld);
	*/
	//�^�udirty�}�[�N
	LoadImageFile("tabDot.png",kImageID_tabDot,inRemoveOld);
	
	//�t�b�^�[���̊e����{�^���p�A�C�R��
	LoadImageFile("iconScrlbarSeparateVertical.png",kImageID_iconPnSeparateVertical,inRemoveOld);
	LoadImageFile("iconPnLeft.png",kImageID_iconPnLeft,inRemoveOld);
	LoadImageFile("iconPnRight.png",kImageID_iconPnRight,inRemoveOld);
	LoadImageFile("iconPnOption.png",kImageID_iconPnOption,inRemoveOld);
	LoadImageFile("iconPnHandle.png",kImageID_iconPnHandle,inRemoveOld);
	
	//�t���O�}�[�N��
	LoadImageFile("iconSwlFlag.png",kImageID_iconSwlFlag,inRemoveOld);
	//#1316 LoadImageFile("iconSwlPin.png",kImageID_iconSwlPin,inRemoveOld);
	LoadImageFile("iconSwlParentPath.png",kImageID_iconSwlParentPath,inRemoveOld);
	
	/*#1316
	//�T�u�E�C���h�E�^�C�g���o�[�i�t���[�e�B���O�E�C��
	LoadImageFile("barSwTitleBarWithBorder_1.png",kImageID_SubWindowTitlebar_Floating0,inRemoveOld);
	LoadImageFile("barSwTitleBarWithBorder_2.png",kImageID_SubWindowTitlebar_Floating1,inRemoveOld);
	LoadImageFile("barSwTitleBarWithBorder_3.png",kImageID_SubWindowTitlebar_Floating2,inRemoveOld);
	
	//�T�u�E�C���h�E�^�C�g���o�[�i�I�[�o�[���C�E�C���h
	LoadImageFile("ui_SubWindowTitlebar_Overlay_0.png",kImageID_SubWindowTitlebar_Overlay0,inRemoveOld);
	LoadImageFile("ui_SubWindowTitlebar_Overlay_1.png",kImageID_SubWindowTitlebar_Overlay1,inRemoveOld);
	LoadImageFile("ui_SubWindowTitlebar_Overlay_2.png",kImageID_SubWindowTitlebar_Overlay2,inRemoveOld);
	*/
	
	//�T�u�E�C���h�E�N���[�Y�{�^��
	LoadImageFile("barSwCloseButton_1.png",kImageID_barSwCloseButton_1,inRemoveOld);
	LoadImageFile("barSwCloseButton_h.png",kImageID_barSwCloseButton_h,inRemoveOld);
	LoadImageFile("barSwCloseButton_p.png",kImageID_barSwCloseButton_p,inRemoveOld);
	
	//�܂肽���݃}�[�N�A�C�R��
	LoadImageFile("btnFldngArrowRight.png",kImageID_btnFldngArrowRight,inRemoveOld);
	LoadImageFile("btnFldngArrowRight_h.png",kImageID_btnFldngArrowRight_h,inRemoveOld);
	LoadImageFile("btnFldngArrowDown.png",kImageID_btnFldngArrowDown,inRemoveOld);
	LoadImageFile("btnFldngArrowDown_h.png",kImageID_btnFldngArrowDown_h,inRemoveOld);
	LoadImageFile("btnFldngArrowEnd.png",kImageID_btnFldngArrowEnd,inRemoveOld);
	LoadImageFile("btnFldngArrowEnd_h.png",kImageID_btnFldngArrowEnd_h,inRemoveOld);
	
	//���X�g���ڗp�A�C�R��
	LoadImageFile("iconSwlDocument.png",kImageID_iconSwlDocument,inRemoveOld);
	LoadImageFile("iconSwlFolder.png",kImageID_iconSwlFolder,inRemoveOld);
	
	//�r���[�����{�^���A�C�R��
	LoadImageFile("btnEditorClose.png",kImageID_btnEditorClose,inRemoveOld);
	LoadImageFile("btnEditorClose_h.png",kImageID_btnEditorClose_h,inRemoveOld);
	
	//�R�[���O���t�p�A�C�R��
	LoadImageFile("btnNaviRight.png",kImageID_btnNaviRight,inRemoveOld);
	LoadImageFile("btnNaviLeft.png",kImageID_btnNaviLeft,inRemoveOld);
	
	//�z�[���A�C�R��
	LoadImageFile("iconSwHome.png",kImageID_iconSwHome,inRemoveOld);
	
	//�}�N���{�^��
	LoadImageFile("iconTbCircleCursorButton.png",kImageID_iconTbCircleCursorButton,inRemoveOld);
	
	//���b�N�{�^��
	LoadImageFile("iconSwLock.png",kImageID_iconSwLock,inRemoveOld);
	LoadImageFile("iconSwUnlock.png",kImageID_iconSwUnlock,inRemoveOld);
	
	//�T�u�E�C���h�E�^�C�g���p�A�C�R��
	LoadImageFile("iconSwSearchList.png",kImageID_iconSwSearchList,inRemoveOld);
	LoadImageFile("iconSwWordReferrer.png",kImageID_iconSwWordReferrer,inRemoveOld);
	LoadImageFile("iconSwTool.png",kImageID_iconSwTool,inRemoveOld);
	
	LoadImageFile("iconTbSeparateVertical.Template.pdf",kImageID_iconTbSeparateVertical,inRemoveOld);
	LoadImageFile("iconSwKeyRecord.png",kImageID_iconTbKeyRecord,inRemoveOld);
	
	//#1368
	LoadImageFile("swSwitchFunction.png",kImageID_swSwitchFunction,inRemoveOld);
	
	//#1421
	LoadImageFile("iconTabPosition.png",kImageID_iconTabPosition,inRemoveOld);
}

/**
�C���[�W���[�h
*/
void	AApp::LoadImageFile( /*#1316 const ABool inUseCustom ,const AFileAcc& inCustomFolder, */
							const AConstCharPtr inFilename, const AImageID inImageID,
							const ABool inRemoveOld )
{
	//inRemoveOld��true�Ȃ狌�f�[�^��o�^����
	if( inRemoveOld == true )
	{
		CUserPane::UnregisterImage(inImageID);
	}
	/*#1316
	//�J�X�^���g�p�t���OON�Ȃ炻���炩��ǂݍ��݂�����
	if( inUseCustom == true )
	{
		//�J�X�^��skin�g�p
		
		//�C���[�W�t�@�C���ǂݍ���
		AFileAcc	file;
		file.SpecifyChild(inCustomFolder,inFilename);
		if( file.Exist() == true )
		{
			CUserPane::RegisterImageByFile(inImageID,file);
			return;
		}
	}
	*/
	
	//�W��skin�g�p
	
	//���O����C���[�W���[�h
	AText	name(inFilename);
	CUserPane::RegisterImageByName(inImageID,name);
}

/**
�F���[�h
*/
/*#1316
void	AApp::LoadColors( const ABool inUseCustom ,const AFileAcc& inCustomFolder )
{
	AFileAcc	colorsFile;
	//inUseCustom��true�Ȃ�J�X�^��colors.txt���g��
	if( inUseCustom == true )
	{
		colorsFile.SpecifyChild(inCustomFolder,"colors.txt");
	}
	//inUseCustom��false�܂��̓J�X�^��colors.txt�����݂��Ă��Ȃ���ΕW��colors.txt���g��
	if( inUseCustom == false || colorsFile.Exist() == false )
	{
		AFileWrapper::GetResourceFile("colors.txt",colorsFile);
	}
	//���[�h
	AText	colorsText;
	colorsFile.ReadTo(colorsText);
	//1�s���Ƃɓǂݍ���
	AIndex	index = 0;
	for( AIndex pos = 0; pos < colorsText.GetItemCount(); )
	{
		AText	line;
		colorsText.GetLine(pos,line);
		AText	token;
		AIndex	p = 0;
		line.GetToken(p,token);
		switch(index)
		{
		  case 0:
			{
				mSkinColor_TabLetter = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 1:
			{
				mSkinColor_TabLetterTopmost = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 2:
			{
				mSkinColor_TabLetterDeactive = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 3:
			{
				mSkinColor_TabLetterDropShadow = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 4:
			{
				mSkinColor_TabLetterTopmostDropShadow = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 5:
			{
				mSkinColor_SubTextColumnButtonLetterColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 6:
			{
				mSkinColor_SubTextColumnButtonLetterColorDeactive = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 7:
			{
				mSkinColor_SubTextColumnButtonDropShadowColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 8:
			{
				mSkinColor_SubWindowLetterColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 9:
			{
				mSkinColor_SubWindowTitlebarTextColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 10:
			{
				mSkinColor_SubWindowTitlebarTextDropShadowColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 11:
			{
				mSkinColor_SubWindowBackgroundColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 12:
			{
				mSkinColor_SubWindowBackgroundColorDeactive = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 13:
			{
				mSkinColor_SubWindowHeaderLetterColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 14:
			{
				mSkinColor_SubWindowHeaderBackgroundColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 15:
			{
				mSkinColor_SubWindowBoxLetterColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 16:
			{
				mSkinColor_SubWindowBoxLetterDropShadowColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 17:
			{
				mSkinColor_SubWindowBoxBackgroundColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 18:
			{
				mSkinColor_SubWindowBoxHeaderColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 19:
			{
				mSkinColor_SubWindowBoxFunctionNameColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 20:
			{
				mSkinColor_SubWindowBoxMatchedColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 21:
			{
				mSkinColor_FileListDiffColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  case 22:
			{
				mSkinColor_JumpListDiffColor = AColorWrapper::GetColorByHTMLFormatColor(token);
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		index++;
	}
}
*/

#pragma mark ===========================

#pragma mark ---�N���^�I������

/**
*/
ABool	AApp::NVIDO_Run()//#182 
{
	//�f�[�^�I�u�W�F�N�g������
	Startup_Phase_Data();
	//UI�I�u�W�F�N�g������
	Startup_Phase_UI();
	
	//UI��Update�i�f�[�^�̔��f�j
	Startup_Phase_UIUpdate();
	
	//#904
	//JavaScript�pmi��{�������s
#if BUILD_WITH_JAVASCRIPTCORE
	AFileAcc	mijsFile;
	AFileWrapper::GetResourceFile("data/JavaScript/mi.js",mijsFile);
	AText	mijsText;
	mijsFile.ReadTo(mijsText);
	AFileAcc	mijsFolder;
	mijsFolder.SpecifyParent(mijsFile);
	NVI_DoJavaScript(mijsText,mijsFolder);
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_Save,kIconID_Tool_Save);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_MutiFileFind,kIconID_Tool_FindInFolder);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_Find,kIconID_Tool_Find);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_ModePref,kIconID_Tool_Gear);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_NavigatePrev,kIconID_Tool_NavPrev);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_NavigateNext,kIconID_Tool_NavNext);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_CompareFiles,kIconID_Tool_Diff);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_SCMCommit,kIconID_Tool_Commit);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_FullScreenMode,kIconID_Tool_FullScreen);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_DisplaySubPane,kIconID_Tool_SubPane);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_Indent,kIconID_Tool_Indent);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_Undo,kIconID_Tool_Undo);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_Redo,kIconID_Tool_Redo);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_CorrectEncoding,kIconID_Tool_Kanji);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_SetProjectFolder,kIconID_Tool_ProjectFolderStamp);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_SetProjectFolder,kIconID_Tool_OpenProjectFolder,-1);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_Print,kIconID_Tool_Printer);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_TabWindowMode,kIconID_Tool_Tab);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_KeyRecordStart,kIconID_Record);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_KeyRecordStop,kIconID_Stop);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_KeyRecordPlay,kIconID_Play);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_AppPref,kIconID_Tool_Checkboxes);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_SCMNextDiff,kIconID_Tool_Down);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_SCMPrevDiff,kIconID_Tool_Up);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_ShowDocPropertyWindow,kIconID_Tool_Property);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_KeyToolMode,kIconID_Tool_DropdownList);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_TextCount,kIconID_Tool_Report);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_CorrespondText,kIconID_Tool_Balance);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_SplitView,kIconID_Tool_SplitView);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_KukeiSelectionMode,kIconID_Tool_Selection);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_Help,kIconID_Tool_Question);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_ShowHideCrossCaret,kIconID_Tool_Guide);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_FindNext,kIconID_Tool_FindNext);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_FindPrevious,kIconID_Tool_FindPrev);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_IndexNext,kIconID_Tool_TreeNext);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_IndexPrevious,kIconID_Tool_TreePrev);
	NVI_GetMenuManager().SetMenuItemIcon(kMenuItemID_MoveToBookmark,kIconID_Tool_NavNext2,1);
#endif
	
	/*#263
#if IMPLEMENTATION_FOR_WINDOWS
	//�R�}���h���C���擾�A�L���ȃt�@�C���p�X������ΊJ��
	AText	cmdline;
	CAppImp::GetCmdLine(cmdline);
	if( NVI_OpenFromCmdLine(cmdline) == false )
	{
		//�ŏ��ɐV�K�h�L�������g���P�쐬
		SPNVI_CreateNewTextDocument(0);
	}
	
#endif
	*/
	ABool	fileexist = false;
	//�����擾
	ATextArray	argTextArray;
	CAppImp::GetCmdLineArgTextArray(argTextArray);
	/*#263
	for( AIndex i = 0; i < argTextArray.GetItemCount(); i++ )
	{
		AFileAcc	file;
		file.Specify(argTextArray.GetTextConst(i));
		if( file.Exist() == true )
		{
			SPNVI_CreateDocumentFromLocalFile(file);
			fileexist = true;
		}
	}
	*/
	//�������s #263
	fileexist = NVI_ExecCommandLineArgs(argTextArray);
	
	//#513 �I�����J���Ă����t�@�C�����ēx�J��
	//#975 kInternalEvent_ReopenFile�͏�ɑ��M���邱�Ƃɂ���BEVTDO_DoInternalEvent()���Ŕ���B if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kReopenFile) == true )
	{
		//#682 ReopenFile();
		//#682 �N�����Ƀt���[�e�B���O�E�C���h�E���\������Ȃ�������̂��߁A��U�N�����������������Ă���A
		//Inernal Event�o�R��Reopen�����邱�Ƃɂ���B
		AObjectIDArray	objectIDArray;
		ABase::PostToMainInternalEventQueue(kInternalEvent_ReopenFile,GetObjectID(),0,GetEmptyText(),objectIDArray);
	}
	
#if IMPLEMENTATION_FOR_WINDOWS
	if( fileexist == false && NVI_GetMostFrontWindowID(kWindowType_Text) == kObjectID_Invalid )
	{
		//�ŏ��ɐV�K�h�L�������g���P�쐬
		AText	name;
		GetAppPref().GetData_Text(AAppPrefDB::kCmdNModeName,name);
		AModeIndex	modeIndex = SPI_FindModeIndexByModeRawName(name);
		if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
		ADocumentID	docID = SPNVI_CreateNewTextDocument(modeIndex);//B0441
		if( docID == kObjectID_Invalid )   {_ACError("",this);};//win
		SPI_GetTextDocumentByID(docID).SPI_SetCreatedAutomatically(true);//B0441
		::Sleep(250);//�A�j���[�V�������ɁAOverlay�E�C���h�E����ɕ\������Ȃ��悤�A�A�j���[�V���������҂�
	}
#endif
	
	//�t�@�C�����X�g�E�C���h�E�\�� Startup_Phase_UIUpdate()����ړ� win
	/*#725
	if( GetAppPref().GetData_Bool(AAppPrefDB::kDisplayWindowList) == true )
	{
		SPI_GetFileListWindow().NVI_Show(false);
	}
	_AInfo("FileList Show done.",this);
	*/
	/*#182 ���ʏ�����NVI_Run()�̒��Ŏ��s����悤�ɕύX
	//�A�v�����s��Ԃֈڍs
	NVM_SetWorking(true);
	
	//win 080728
	NVI_UpdateMenu();
	
	//B0000
	ATrace::StartPostInternalEvent();
	
	_AInfo("Application event loop starts.",this);
	
	//B0372
	//SPI_CreateNewWindowAutomatic();//AppleEvent AppOpen����M���ɃR�[������悤�C���i���ꂾ�ƁA���ރ_�u���N���b�N�ƂƂ��ɋN�����鎞�ɖ��j
	
	//Run()�̓A�v���I���܂ŕԂ��Ă��Ȃ�
	int result = NVM_GetImp().Run();
	_AInfo("Application event loop ended.",this);
	return result;
	*/
	
	//start up�����C�x���g���M
	{
		AObjectIDArray	objectIDArray;
		ABase::PostToMainInternalEventQueue(kInternalEvent_StartUp,GetObjectID(),0,GetEmptyText(),objectIDArray);
	}
	
	return true;//#182
}

/**
�I�����j���[�I�����̏���
@param inContinuing 1�̃g�����U�N�V�������̑����̏����ł��邱�Ƃ������i�ۑ��m�F�Łu�ۑ����Ȃ��v��I�񂾏ꍇ�Ȃǁj
*/
ABool	AApp::NVIDO_Quit(/*#1102 const ABool inContinuing */)//win 080702 #433
{
	/*win �E�C���h�E������Ƃ��ɂ��ۑ��K�v�i����O�ɕۑ��K�v�j�Ȃ̂Ŋe�C�x���g��M����֏������ړ�
	//#513 �I�����J���Ă����t�@�C���̃p�X���L��
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kReopenFile) == true && inContinuing == false )
	{
		SPI_SaveReopenFile();
	}
	*/
	
	//���[�h�ݒ�_�C�A���O�ɖ����f�̃f�[�^������Β��~
	//#868if( mModePrefWindowID != kObjectID_Invalid )//#693
	//#868{
	//#868if( /*#199 mModePrefWindow*/SPI_GetModePrefWindow().AttemptQuit() == false )
	for( AIndex modeIndex = 0; modeIndex < mModePrefWindowIDArray.GetItemCount(); modeIndex++ )
	{
		if( mModePrefWindowIDArray.Get(modeIndex) != kObjectID_Invalid )
		{
			if( SPI_GetModePrefWindow(modeIndex).AttemptQuit() == false )
			{
#if IMPLEMENTATION_FOR_WINDOWS
				//�V�K�h�L�������g���P�쐬
				AText	name;
				GetAppPref().GetData_Text(AAppPrefDB::kCmdNModeName,name);
				AModeIndex	modeIndex = SPI_FindModeIndexByModeRawName(name);
				if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
				ADocumentID	docID = SPNVI_CreateNewTextDocument(modeIndex);
				if( docID == kObjectID_Invalid )   {_ACError("",this);};
				SPI_GetTextDocumentByID(docID).SPI_SetCreatedAutomatically(true);
#endif
				return false;//#182
			}
		}
	}
	
	//�ł��^�u�������E�C���h�E�ɂăT�C�h�o�[�̃f�[�^��ۑ��i���݂̃����[�g�t�@�C���̃t�H���_��ۑ����邽�߁j#361
	AWindowID	mostTabWindowID = SPI_GetMostNumberTabsTextWindowID();
	if( mostTabWindowID != kObjectID_Invalid )
	{
		SPI_GetTextWindowByID(mostTabWindowID).SPI_SaveSideBarData();
	}
	
	//#513
	//reopen�ۑ� #1102 Quit���j���[��������ړ�
	SPI_SaveReopenFile();
	
	//�E�C���h�E�S�N���[�Y
	if( NVI_CloseAllWindow() == false )
	{
		//��ŕۑ�����reopen�f�[�^���N���A #1102
		SPI_ClearReopenData();
		//�N���[�Y���̕ۑ��m�F�_�C�A���O�L�����Z�����͏I���������^�[��
		return false;//#182
	}
	
	//�I����Ԑݒ� #688
	NVM_SetWorking(false);
	
	//#92 Document�S�폜�����O�ɂ��Ȃ��ƁADocument�g�p��Window���폜����DocumentID�Q�ƃG���[��������
	Quit_Phase_UI();
	Quit_Phase_Data();
	
	/*#688 AApplication::NVI_Quit()�ֈړ�
	//�h�L�������g�S�폜
	NVI_DeleteAllDocument();
	*/
	
	//�X���b�h�I��
	//�}���`�t�@�C�������X���b�h�I��
	if( mMultiFileFinderObjectID != kObjectID_Invalid )
	{
		NVI_DeleteThread(mMultiFileFinderObjectID);
	}
	//�C���|�[�g�t�@�C���F���X���b�h�I��
	if( mImportFileRecognizerObjectID != kObjectID_Invalid )
	{
		NVI_DeleteThread(mImportFileRecognizerObjectID);
	}
	//FTP�ڑ��X���b�h�I��
	if( mFTPConnectionObjectID != kObjectID_Invalid )
	{
		NVI_DeleteThread(mFTPConnectionObjectID);
	}
	//AppleScript���s�X���b�h�I��
	if( mAppleScriptExecuterObjectID != kObjectID_Invalid )
	{
		NVI_DeleteThread(mAppleScriptExecuterObjectID);
	}
	
	//Imp�����I���i����ɂ��A�{�C�x���g�g�����U�N�V�����I����ARun()���I������B�j
	//#182 NVM_GetImp().Quit();
	
	return true;//#182
}

void	AApp::Startup_Phase_Data()
{
	try
	{
		//�A�v���P�[�V�����̍X�V�������擾�i���[�h�ݒ��default����̎����X�V����Ɏg�p����j
		AFileAcc	appFile;
		AFileWrapper::GetAppFile(appFile);
		ADateTime	datetime;
		appFile.GetLastModifiedDataTime(datetime);
		ADateTimeWrapper::GetDateTimeText(datetime,mAppModifiedDataTimeText);
		
		//�o�[�W����2.1�ł̕W���Y�t�c�[���o�[���\���ɂ��邽�߂́A�W���Y�t�c�[���o�[���̃��X�g���擾
		AText	v2ToolbarItemNameToHideText;
		AFileAcc	v2ToolbarItemNameToHideFile;
		AFileWrapper::GetResourceFile("data/v2ToolbarItemNameToHide.txt",v2ToolbarItemNameToHideFile);
		v2ToolbarItemNameToHideFile.ReadTo(v2ToolbarItemNameToHideText);
		mV2ToolbarItemNameToHideArray.ExplodeLineFromText(v2ToolbarItemNameToHideText);
		
		//AppPref
		try
		{
			mAppPref.Setup();
		}
		catch(...)
		{
			_ACError("AApp::Startup_Phase_Data() caught exception.",NULL);//#199
		}
		_AInfo("AppPref Setup() done.",this);
		
		//�C���[�W�t�@�C���ǂݍ���
		SPI_LoadImageFiles(false);
		
		//win ������Ή�
#if IMPLEMENTATION_FOR_WINDOWS
		CAppImp::SetupLanguageSateliteDLL((ALanguage)(mAppPref.GetData_Number(AAppPrefDB::kLaunguage)));
#endif
		
		//win ������Ή�
		//AApp�R���X�g���N�^����ړ�
		//���j���[�֘A������
		NVI_GetMenuManager().InitMenuBarMenu();
		_AInfo("InitMenuBarMenu() done.",this);
		//�R���e�N�X�g���j���[�ݒ�
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_TextSelected",kContextMenuID_TextSelected);
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_TextNotSelected",kContextMenuID_TextUnselected);
		//#688 NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_IndexWindow",kContextMenuID_IndexWindow);
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_Tab",kContextMenuID_Tab);
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_RecentOpen",kContextMenuID_RecentOpen);//R0186
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_RecentOpen_LeftSideBar",kContextMenuID_RecentOpen_LeftSideBar);//#1380
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_RecentOpen_RightSideBar",kContextMenuID_RecentOpen_RightSideBar);//#1380
		//#1380 NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_FileList",kContextMenuID_FileList);//#442
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_GeneralSubWindow",kContextMenuID_GeneralSubWindow);//#1380
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_GeneralSubWindow_LeftSideBar",kContextMenuID_GeneralSubWindow_LeftSideBar);//#1380
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_GeneralSubWindow_RightSideBar",kContextMenuID_GeneralSubWindow_RightSideBar);//#1380
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_JumpList",kContextMenuID_JumpList);//#442
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_JumpList_LeftSideBar",kContextMenuID_JumpList_LeftSideBar);//#1380
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_JumpList_RightSideBar",kContextMenuID_JumpList_RightSideBar);//#1380
		//#1380 NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_IdInfo",kContextMenuID_IdInfo);//#442
		NVI_GetMenuManager().RegisterContextMenu(kContextMenuID_TextEncoding);//#232
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_ReturnCode",kContextMenuID_ReturnCode);//#232
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_WrapMode",kContextMenuID_WrapMode);//#232
		NVI_GetMenuManager().RegisterContextMenu(kContextMenuID_Mode);//#232
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_ToolButton",kContextMenuID_ToolButton);//#232
		NVI_GetMenuManager().RegisterContextMenu(kContextMenuID_IdInfoHistory);//#238
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_IndexView",kContextMenuID_IndexView);//#465
		NVI_GetMenuManager().RegisterContextMenu(kContextMenuID_JumpListHistory);//#454
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_Toolbar",kContextMenuID_ToolBar);//#619
		//#1367 NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_RightSideBarPref",kContextMenuID_RightSideBarPref);
		//#1367 NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_LeftSideBarPref",kContextMenuID_LeftSideBarPref);
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_SameProjectHeader",kContextMenuID_SameProjectHeader);
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_CandidateSearchMode",kContextMenuID_CandidateSearchMode);
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_Percent",kContextMenuID_Percent);
		NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_SubWindows",kContextMenuID_SubWindows);
		//#0 ���g�p�̂��߃R�����g�A�E�g NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_Diff",kContextMenuID_Diff);
		//win
		NVI_GetMenuManager().SetContextMenuItemIcon(kContextMenuID_TextUnselected,0,kIconID_Tool_NavNext2);
		NVI_GetMenuManager().SetContextMenuItemIcon(kContextMenuID_TextUnselected,1,kIconID_Tool_Info);
		NVI_GetMenuManager().SetContextMenuItemIcon(kContextMenuID_TextUnselected,13,kIconID_Tool_DocumentTree);
		NVI_GetMenuManager().SetContextMenuItemIcon(kContextMenuID_TextSelected,0,kIconID_Tool_NavNext2);
		NVI_GetMenuManager().SetContextMenuItemIcon(kContextMenuID_TextSelected,1,kIconID_Tool_Info);
		NVI_GetMenuManager().SetContextMenuItemIcon(kContextMenuID_TextSelected,13,kIconID_Tool_FindInFolder);
		NVI_GetMenuManager().SetContextMenuItemIcon(kContextMenuID_TextSelected,24,kIconID_Tool_Printer);
		NVI_GetMenuManager().SetContextMenuItemIcon(kContextMenuID_TextSelected,22,kIconID_Tool_DocumentTree);
		NVI_GetMenuManager().SetContextMenuItemIcon(kContextMenuID_Tab,3,kIconID_Tool_DockLeft);
		//���I���j���[�ݒ�
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_Open_RecentlyOpenedFile);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_NewTextDocument);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_NewWithTemplate);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_SameFolder);
		//#922 NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_SameProject);
		//#922 NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_FolderList) ;
		//#922 NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_FTPFolder);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_SetTextEncoding);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_SetMode);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_ModePref);//#868
		//#688 NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_Font);
		//#922 NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_WindowMenu);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_OpenImportFile);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_Transliterate);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_MoveToDefinition);
		NVI_GetMenuManager().ReserveDynamicMenuItem(kMenuItemID_Tool);
		NVI_GetMenuManager().ReserveDynamicMenuItem(kMenuItemID_Tool_StandardMode);
		//NVI_GetMenuManager().ReserveDynamicMenuItem(kMenuItemID_Tool_StandardMode);
		mCorrectSpellMenu = NVI_GetMenuManager().CreateDynamicMenu(kMenuItemID_CorrectSpell,false);//R0199
		//#81
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_CompareWithAutoBackup_Monthly);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_CompareWithAutoBackup_Daily);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_CompareWithAutoBackup_Hourly);
		NVI_GetMenuManager().RegisterDynamicMenuItem(kMenuItemID_CompareWithAutoBackup_Minutely);
		//TextArray���j���[�o�^
		NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_TextEncoding);//#232
		NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_EnabledMode);//#232
		NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_AllMode);//#232
		NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_DefaultMode);//#463
		//#844 NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_ModeUpdateURL);//#427
		NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_FileFilterPreset);//#617
		NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_RegExpList);//#801
		NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_TextMarker);//#750
		NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_Pref);
		{
			ATextArray	textArray;
			AText	text;
			NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_ReturnCode);//#232
			text.SetLocalizedText("ReturnCodeMenu_CR");
			textArray.Add(text);
			text.SetLocalizedText("ReturnCodeMenu_CRLF");
			textArray.Add(text);
			text.SetLocalizedText("ReturnCodeMenu_LF");
			textArray.Add(text);
			NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_ReturnCode,textArray);//#232
		}
		{
			ATextArray	textArray;
			AText	text;
			NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_WrapMode);//#232
			text.SetLocalizedText("WrapModeMenu_NoWrap");
			textArray.Add(text);
			text.SetLocalizedText("WrapModeMenu_WordWrapByWindowWidth");
			textArray.Add(text);
			text.SetLocalizedText("WrapModeMenu_WordWrapByLetterCount");
			textArray.Add(text);
			//#1113
			text.SetLocalizedText("WrapModeMenu_LetterWrapByWindowWidth");
			textArray.Add(text);
			text.SetLocalizedText("WrapModeMenu_LetterWrapByLetterCount");
			textArray.Add(text);
			NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_WrapMode,textArray);//#232
		}
		/*#844
		//#427
		{
			ATextArray	textArray;
			mModeUpdateURLArray.DeleteAll();
			for( AIndex i = 0; i < kModeUpdateURLCount; i++ )
			{
				AText	text;
				text.SetLocalizedText("ModeUpdateURLTitle",i);
				textArray.Add(text);
				text.SetLocalizedText("ModeUpdateURL",i);
				mModeUpdateURLArray.Add(text);
			}
			NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_ModeUpdateURL,textArray);
		}
		*/
		//#617
		{
			ATextArray	textArray;
			mFileFilterPresetArray.DeleteAll();
			for( AIndex i = 0; i < kFileFilterPresetCount; i++ )
			{
				AText	text;
				text.SetLocalizedText("FileFilterPresetTitle",i);
				textArray.Add(text);
				text.SetLocalizedText("FileFilterPreset",i);
				mFileFilterPresetArray.Add(text);
			}
			NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_FileFilterPreset,textArray);
		}
		//#801
		//���K�\���T���v�����j���[�쐬
		{
			//"data/�i�e����j"�t�H���_���擾
			AFileAcc	folder;
			SPI_GetLocalizedDataFolder(folder);
			//���j���[�쐬
			AFileAcc	regExpList_Menu_File;
			regExpList_Menu_File.SpecifyChild(folder,"RegExpList_Menu.txt");
			AText	menutext;
			regExpList_Menu_File.ReadTo(menutext);
			ATextArray	menutextarray;
			menutextarray.ExplodeFromText(menutext,kUChar_LF);
			NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_RegExpList,menutextarray);
			//���j���[�ɑΉ����鐳�K�\�����擾
			AFileAcc	regExpList_RegExp_File;
			regExpList_RegExp_File.SpecifyChild(folder,"RegExpList_RegExp.txt");
			AText	regexptext;
			regExpList_RegExp_File.ReadTo(regexptext);
			mRegExpListArray.ExplodeFromText(regexptext,kUChar_LF);
		}
		
		//#889
		//�J���[�X�L�[�����j���[�쐬
		{
			//���j���[�o�^
			NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_ColorScheme);
			NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_AppColorScheme);//#1316
			//���j���[�X�V
			SPI_UpdateColorSchemeMenu();
		}
		
		//#193
		{
			//���j���[�o�^
			NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_AccessPlugIn);
			//���j���[�X�V
			SPI_UpdateAccessPlugInMenu();
		}
		
		/*#1316
		//skin���j���[�쐬
		{
			//���j���[�o�^
			NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_Skin);
			//���j���[�X�V
			SPI_UpdateSkinMenu();
		}
		*/
		
		//default���[�h�̃��[�h�����j���[�����i�����X�V�����[�h�j
		{
			//���j���[�e�L�X�g
			ATextArray	menutextarray;
			ATextArray	actionTextArray;
			//�ŏ��̍��ڂ́u�������[�h���̃��[�h�v�ɂ���
			AText	text;
			text.SetLocalizedText("ModeUpdate_SourceIsSameNameMode");
			menutextarray.Add(text);
			actionTextArray.Add(GetEmptyText());
			//default�t�H���_�̊e�t�H���_�����擾���āA���j���[�ɒǉ�
			AFileAcc	defaultModeFolder;
			GetApp().SPI_GetModeDefaultFolder(defaultModeFolder);
			AObjectArray<AFileAcc>	modeFolderArray;
			defaultModeFolder.GetChildren(modeFolderArray);
			for( AIndex i = 0; i < modeFolderArray.GetItemCount(); i++ )
			{
				AText	filename;
				modeFolderArray.GetObjectConst(i).GetFilename(filename);
				menutextarray.Add(filename);
				actionTextArray.Add(filename);
			}
			//���j���[�ݒ�
			AApplication::GetApplication().NVI_GetTextArrayMenuManager().
					UpdateTextArrayMenu(kTextArrayMenuID_DefaultMode,menutextarray,actionTextArray);
		}
		
		//ContextMenu��TextArrayMenu��o�^
		NVI_GetMenuManager().SetContextMenuTextArrayMenu(kContextMenuID_TextEncoding,kTextArrayMenuID_TextEncoding);//#232
		NVI_GetMenuManager().SetContextMenuTextArrayMenu(kContextMenuID_Mode,kTextArrayMenuID_EnabledMode);//#914
		//Font���j���[�o�^
		//#688 NVI_GetMenuManager().SetDynamicMenuAsFontMenu(kMenuItemID_Font,true);//#375
		//#688 _AInfo("Menu Register done.",this);
		
		//R0000
		MakeBuiltinSDFList();
		_AInfo("MakeBuiltinSDFList() done.",this);
		
		//ModePref
		try
		{
			InitModePref();
		}
		catch(...)
		{
			_ACError("AApp::Startup_Phase_Data() caught exception.",NULL);//#199
		}
		_AInfo("InitModePref() done.",this);
		
		//#844 mAppPref.SetupAfterModeInit();
		
		InitTransliterate();
		_AInfo("InitTransliterate() done.",this);
		
		InitFuzzySearch();//#166
		
		//#693
		//�����p�����[�^�������i���݂̌����p�����[�^�f�[�^��AApp�ŊǗ��j
		InitFindParam();
		
		//#454
		mJumpListHistory.SetFromTextArray(NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kJumpListHistory));
		mJumpListHistory_Path.SetFromTextArray(NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kJumpListHistory_Path));
		//���j���[�X�V
		GetApp().NVI_GetMenuManager().SetContextMenuTextArray(kContextMenuID_JumpListHistory,mJumpListHistory,mJumpListHistory_Path);
		
		//#717
		//�⊮���X�g�����擾
		mRecentCompletionWordArray.SetFromTextArray(NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kRecentCompletionWordArray));
		
		//�e�L�X�g�}�[�J�[index�擾
		mCurrentTextMarkerGroupIndex = NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kTextMarkerCurrentGroupIndex);
	}
	catch(...)
	{
		_ACError("AApp::Startup_Phase_Data() caught exception.",NULL);//#199
	}
	/*#369 ���L�ŕϊ��t�@�C�����쐬�������A�T���Q�[�g�y�A�������܂܂�邽�߁A�ϊ��e�[�u�������͂�߂邱�Ƃɂ���
	ATextEncoding	macjapanese;
	ATextEncodingWrapper::GetTextEncodingFromName("x-mac-japanese",macjapanese);
	AText	sjis0213unicode;
	AText	gaijiunicode;
	//��P�o�C�g�F81-9F, E0-FC
	for( unsigned char ch1 = 0xF0; ch1 <= 0xFC; ch1++ )
	{
		//��Q�o�C�g�F40-7E, 80-FC
		for( unsigned char ch2 = 0x40; ch2 <= 0xFC; ch2++ )
		{
			if( ch2 == 0x7F )   continue;
			AText	text;
			text.Add(ch1);
			text.Add(ch2);
			AText	t;
			ABool	fallbackused = false;
			if( ATextEncodingWrapper::TECConvertToUTF16(text,t,ATextEncodingWrapper::GetSJISX0213TextEncoding(),
							false,fallbackused) == false )
			{
				_ACError("",this);
			}
			sjis0213unicode.AddText(t);
			if( ATextEncodingWrapper::TECConvertToUTF16(text,t,macjapanese,
							false,fallbackused) == false )
			{
				_ACError("",this);
			}
			gaijiunicode.AddText(t);
		}
	}
	AFileAcc	desktopfolder;
	AFileWrapper::GetDesktopFolder(desktopfolder);
	AFileAcc	sjis0213unicodeFile;
	sjis0213unicodeFile.SpecifyChild(desktopfolder,"sjis0213unicode.txt");
	sjis0213unicodeFile.CreateFile();
	sjis0213unicodeFile.WriteFile(sjis0213unicode);
	AFileAcc	gaijiunicodeFile;
	gaijiunicodeFile.SpecifyChild(desktopfolder,"gaijiunicode.txt");
	gaijiunicodeFile.CreateFile();
	gaijiunicodeFile.WriteFile(gaijiunicode);
	*/
	/*
	AText	text;
	unsigned char	ch1 = 0xF0;
	//��Q�o�C�g�F40-7E, 80-FC
	for( unsigned char ch2 = 0x40; ch2 <= 0xFC; ch2++ )
	{
		if( ch2 == 0x7F )   continue;
		text.Add(ch1);
		text.Add(ch2);
	}
	AFileAcc	desktopfolder;
	AFileWrapper::GetDesktopFolder(desktopfolder);
	AFileAcc	file;
	file.SpecifyChild(desktopfolder,"SJIS0xF0.txt");
	file.CreateFile();
	file.WriteFile(text);
	*/
	/*���L�Œ��ׂ��Ƃ���tec==1�̂�F040->E000�ɂȂ�
	for( ATextEncoding tec = 0; tec < 65536; tec++ )
	{
		AText	src;
		src.Add(0xF0);
		src.Add(0x40);
		AText	dst;
		ABool	fallbackused = false;
		if( ATextEncodingWrapper::TECConvertToUTF16(src,dst,tec,false,fallbackused) == false )
		{
		}
		else
		{
			if( dst.Get(0) == 0x00 && dst.Get(1) == 0xE0 )
			{
				AText	t;
				t.SetNumber(tec);
				t.OutputToStderr();
			}
		}
	}
	*/
}

void	AApp::Startup_Phase_UI()
{
	try
	{
		AText	path;
		AFileAcc	folder;
		//�u�����v���W�F�N�g�v����
		GetAppPref().GetData_Text(AAppPrefDB::kSameProjectFolderPath,path);
		if( path.GetItemCount() > 0 )
		{
			folder.Specify(path);
			AModeIndex	modeIndex = GetAppPref().GetData_Number(AAppPrefDB::kSameProjectFolderModeIndex);
			//#932 if( folder.Exist() == true )
			{
				AFileAcc	file;//#298
				AText	miproj("miproj");//#298
				file.SpecifyChild(folder,miproj);//#298
				//#932 if( file.Exist() == true )//#298
				{
					SPI_UpdateSameProjectMenu(folder,modeIndex);
				}
			}
		}
		_AInfo("UpdateSameProjectMenu done.",this);
		//�u�����t�H���_�v����
		GetAppPref().GetData_Text(AAppPrefDB::kSameFolderPath,path);
		folder.Specify(path);
		//#932 if( folder.Exist() == true )
		{
			SPI_UpdateSameFolderMenu(folder);
		}
		_AInfo("UpdateSameFolderMenu done.",this);
		
		SPI_UpdateModeMenu();
		_AInfo("UpdateModeMenu done.",this);
		
		//TraceLogWindow #199
		AWindowDefaultFactory<AWindow_TraceLog>	traceLogWindow;
		mTraceLogWindowID = NVI_CreateWindow(traceLogWindow);
		//#1275 �������̂��߃E�C���h�E�\�����삷��܂ł͐������Ȃ� /*#199 mTraceLogWindow*/SPI_GetTraceLogWindow().NVI_Create(kObjectID_Invalid);
		
		//#1217
		AWindowDefaultFactory<AWindow_JavaScriptModalAlert>	javaScriptModalAlertWindowFactory;
		mJavaScriptModalAlertWindowID = NVI_CreateWindow(javaScriptModalAlertWindowFactory);
		AWindowDefaultFactory<AWindow_JavaScriptModalCancelAlert>	javaScriptModalCancelAlertWindowFactory;
		mJavaScriptModalCancelAlertWindowID = NVI_CreateWindow(javaScriptModalCancelAlertWindowFactory);
		
		_AInfo("Windows create done.",this);
	}
	catch(...)
	{
		_ACError("AApp::Startup_Phase_UI() caught exception.",NULL);//#199
	}
}

void	AApp::Startup_Phase_UIUpdate()
{
	/*NVIDO_Run()���ֈړ� win
	//�t�@�C�����X�g�E�C���h�E�\��
	if( GetAppPref().GetData_Bool(AAppPrefDB::kDisplayWindowList) == true )
	{
		SPI_GetFileListWindow().NVI_Show();
	}
	_AInfo("FileList Show done.",this);
	*/
	/*#725
	//RC2
	if( GetAppPref().GetData_Bool(AAppPrefDB::kDisplayIdInfoWindow) == true )
	{
		SPI_GetIdInfoWindow().NVI_Show();
	}
	//#138
	if( GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFusenList) == true )
	{
		SPI_GetFusenListWindow().NVI_Show();
	}
	_AInfo("IdInfo Show done.",this);
	*/
	/*#725
	//#679 �L�[�L�^�Đ��E�C���h�E
	if( GetAppPref().GetData_Bool(AAppPrefDB::kDisplayKeyRecordWindow) == true )
	{
		SPI_GetKeyRecordWindow().NVI_Show();
	}
	*/
	
	SPI_UpdateNewMenu();
	_AInfo("SPI_UpdateNewMenu() done.",this);
	//#922 MakeFolderListMenu();
	//#922 _AInfo("MakeFolderListMenu() done.",this);
	//#922 MakeFTPFolderMenu();
	//#922 _AInfo("MakeFTPFolderMenu() done.",this);
	SPI_UpdateTextEncodingMenu();
	_AInfo("SPI_UpdateTextEncodingMenu() done.",this);
	
	//�ŋߊJ�����t�@�C���̐ݒ�
	SPI_UpdateRecentlyOpenedFile(false);
	//#932 SPI_DeleteRecentlyOpenedFileNotFound();//not found���ڂ��폜 #921
	_AInfo("SPI_UpdateRecentlyOpenedFile() done.",this);
	
	//#844 SPI_UpdateFixedMenuShortcut();
	//#844 _AInfo("SPI_UpdateFixedMenuShortcut() done.",this);
	
	//#750
	//�e�L�X�g�}�[�J�[���j���[�X�V
	UpdateTextMarkerMenu();
	
	//#725
	//�t���[�e�B���O�T�u�E�C���h�E��reopen
	try
	{
		ReopenFloatingSubWindows();
	}
	catch(...)
	{
	}
	//#81
	//���ۑ��f�[�^��\��
	OpenUnsavedData();
}


void	AApp::Quit_Phase_UI()
{
	_AInfo("AApp::Quit_Phase_UI() started.",this);
	//AApp�ێ��̊e��E�C���h�E�̏I������
	//#291 GetAppPref().SetData_Bool(AAppPrefDB::kDisplayWindowList,/*#199 mFileListWindow*/SPI_GetFileListWindow().NVI_IsVisibleMode());//B0404
	//#291 GetAppPref().SetData_Bool(AAppPrefDB::kDisplayIdInfoWindow,/*#199 mIdInfoWindow*/SPI_GetIdInfoWindow().NVI_IsVisibleMode());//RC2
	
	
	//#725
	//�t���[�e�B���O�T�u�E�C���h�E�̈ʒu�E�T�C�Y��ۑ�
	SaveFloatingSubWindows();
	
	//#725
	//�T�u�E�C���h�E�폜
	while( mSubWindowArray_WindowID.GetItemCount() > 0 )
	{
		SPI_DeleteSubWindow(mSubWindowArray_WindowID.Get(0));
	}
	/*#858
	ABool	fusenListWindowVisible = false;
	if( mFusenListWindowID != kObjectID_Invalid )
	{
		fusenListWindowVisible = SPI_GetFusenListWindow().NVI_IsVisibleMode();
	}
	GetAppPref().SetData_Bool(AAppPrefDB::kDisplayFusenList,fusenListWindowVisible);//#138
	*/
	/*#725
	ABool	keyRecordWindowVisible = false;
	if( mKeyRecordWindowID != kObjectID_Invalid )
	{
		keyRecordWindowVisible = SPI_GetKeyRecordWindow().NVI_IsVisibleMode();
	}
	GetAppPref().SetData_Bool(AAppPrefDB::kDisplayKeyRecordWindow,keyRecordWindowVisible);//#679
	*/
	//#693 /*#199 mAppPrefWindow*/SPI_GetAppPrefWindow().NVI_Close();
	//#693 /*#199 mModePrefWindow*/SPI_GetModePrefWindow().NVI_Close();
	//#693 /*#199 mFindWindow*/SPI_GetFindWindow().NVI_Close();
	//#693 /*#199 mMultiFileFindWindow*/SPI_GetMultiFileFindWindow().NVI_Close();
	//#693 /*#199 mMultiFileFindProgress*/SPI_GetMultiFileFindProgress().NVI_Close();
	//#693 /*#199 mFTPFolderWindow*/SPI_GetFTPFolderWindow().NVI_Close();
	//#693 /*#199 mFolderListWindow*/SPI_GetFolderListWindow().NVI_Close();
	//#693 /*#199 mFTPProgress*/SPI_GetFTPProgress().NVI_Close();
	//#693 /*#199 mFTPLogWindow*/SPI_GetFTPLogWindow().NVI_Close();
	//#695 /*#199 mTraceLogWindow*/SPI_GetTraceLogWindow().NVI_Close();
	//#693 /*#199 mFileListWindow*/SPI_GetFileListWindow().NVI_Close();
	//#693 /*#199 mTextCountWindow*/SPI_GetTextCountWindow().NVI_Close();
	//#693 /*#199 mIdInfoWindow*/SPI_GetIdInfoWindow().NVI_Close();//RC2
	//#693 /*#199 mCandidateListWindow*/SPI_GetCandidateListWindow().NVI_Close();//RC2
	//#693 /*#199 mKeyToolListWindow*/SPI_GetKeyToolListWindow().NVI_Close();//R0073
	
	//#738
	if( mKeyActionListWindowID != kObjectID_Invalid )
	{
		NVI_DeleteWindow(mKeyActionListWindowID);
	}
	//#846
	if( mEditProgressWindowID != kObjectID_Invalid )
	{
		NVI_DeleteWindow(mEditProgressWindowID);
	}
	//#199 �E�C���h�E�I�u�W�F�N�g�폜
	if( mAboutWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetAboutWindow().NVI_Close();//#693
		NVI_DeleteWindow(mAboutWindowID);
	}
	//#1384
	if( mUserIDRegistrationWindowID != kObjectID_Invalid )
	{
		SPI_GetUserIDRegistrationWindow().NVI_Close();
		NVI_DeleteWindow(mUserIDRegistrationWindowID);
	}
	//
	if( mEnabledDefinesWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetEnabledDefinesWindow().NVI_Close();//#693
		NVI_DeleteWindow(mEnabledDefinesWindowID);//#467
	}
	/*#725
	if( mKeyRecordWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetKeyRecordWindow().NVI_Close();//#693
		NVI_DeleteWindow(mKeyRecordWindowID);//#390
	}
	*/
	if( mCompareFolderWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetCompareFolderWindow().NVI_Close();//#693
		NVI_DeleteWindow(mCompareFolderWindowID);
	}
	/*#688
	if( mDevToolsWindowID != kObjectID_Invalid )//#693
	{
		NVI_DeleteWindow(mDevToolsWindowID);//#436
	}
	*/
	SPI_GetTraceLogWindow().NVI_Close();
	NVI_DeleteWindow(mTraceLogWindowID);
	if( mFindWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFindWindow().NVI_Close();
		NVI_DeleteWindow(mFindWindowID);
	}
	//#1217
	NVI_DeleteWindow(mJavaScriptModalAlertWindowID);
	mJavaScriptModalAlertWindowID = kObjectID_Invalid;
	NVI_DeleteWindow(mJavaScriptModalCancelAlertWindowID);
	mJavaScriptModalCancelAlertWindowID = kObjectID_Invalid;
	/*#379
	if( mDiffWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetDiffWindow().NVI_Close();
		NVI_DeleteWindow(mDiffWindowID);
	}
	*/
	if( mMultiFileFindWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetMultiFileFindWindow().NVI_Close();
		NVI_DeleteWindow(mMultiFileFindWindowID);
	}
	if( mAppPrefWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetAppPrefWindow().NVI_Close();
		NVI_DeleteWindow(mAppPrefWindowID);
	}
	/*#922
	if( mFTPFolderWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFTPFolderWindow().NVI_Close();
		NVI_DeleteWindow(mFTPFolderWindowID);
	}
	*/
	if( mMultiFileFindProgressWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetMultiFileFindProgress().NVI_Close();
		NVI_DeleteWindow(mMultiFileFindProgressWindowID);
	}
	if( mFTPProgressWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFTPProgress().NVI_Close();
		NVI_DeleteWindow(mFTPProgressWindowID);
	}
	//#844
	if( mAddNewModeWindowID != kObjectID_Invalid )
	{
		SPI_GetAddNewModeWindow().NVI_Close();
		NVI_DeleteWindow(mAddNewModeWindowID);
		mAddNewModeWindowID = kObjectID_Invalid;
	}
	//���s�\�t�@�C���C���|�[�g�x���E�C���h�E�폜
	if( mModeExecutableAlertWindowID != kObjectID_Invalid )
	{
		SPI_GetModeExecutableAlertWindow().NVI_Close();
		NVI_DeleteWindow(mModeExecutableAlertWindowID);
		mModeExecutableAlertWindowID = kObjectID_Invalid;
	}
	/*#868
	if( mModePrefWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetModePrefWindow().NVI_Close();
		NVI_DeleteWindow(mModePrefWindowID);
	}
	*/
	//�e���[�h�ݒ�E�C���h�E���Ƃ̃��[�v
	for( AIndex modeIndex = 0; modeIndex < mModePrefWindowIDArray.GetItemCount(); modeIndex++ )
	{
		AWindowID	modePrefWindowID = mModePrefWindowIDArray.Get(modeIndex);
		if( modePrefWindowID != kObjectID_Invalid )
		{
			//���[�h�ݒ�E�C���h�E���폜
			SPI_GetModePrefWindow(modeIndex).NVI_Close();
			NVI_DeleteWindow(modePrefWindowID);
		}
	}
	/*#922
	if( mFolderListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFolderListWindow().NVI_Close();
		NVI_DeleteWindow(mFolderListWindowID);
	}
	*/
	if( mFTPLogWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFTPLogWindow().NVI_Close();
		NVI_DeleteWindow(mFTPLogWindowID);
	}
	/*#695
	if( mBigFileAlertWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetBigFileAlertWindow().NVI_Close();
		NVI_DeleteWindow(mBigFileAlertWindowID);
	}
	*/
	/*#725
	if( mCandidateListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetCandidateListWindow().NVI_Close();
		NVI_DeleteWindow(mCandidateListWindowID);
	}
	if( mKeyToolListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetKeyToolListWindow().NVI_Close();//#693
		NVI_DeleteWindow(mKeyToolListWindowID);
	}
	*/
	if( mTextCountWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetTextCountWindow().NVI_Close();
		NVI_DeleteWindow(mTextCountWindowID);
	}
	if( mNewFTPFileWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetNewFTPFileWindow().NVI_Close();
		NVI_DeleteWindow(mNewFTPFileWindowID);
	}
	if( mSetProjectFolderWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetSetProjectFolderWindow().NVI_Close();
		NVI_DeleteWindow(mSetProjectFolderWindowID);
	}
	/*#858
	if( mProjectFolderAlertWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetProjectFolderAlertWindow().NVI_Close();
		NVI_DeleteWindow(mProjectFolderAlertWindowID);
	}
	*/
	/*#858
	if( mFusenListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFusenListWindow().NVI_Close();
		NVI_DeleteWindow(mFusenListWindowID);
	}
	if( mFusenEditWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFusenEditWindow().NVI_Close();
		NVI_DeleteWindow(mFusenEditWindowID);
	}
	*/
	/*#725 
	if( mIdInfoWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetIdInfoWindow().NVI_Close();
		NVI_DeleteWindow(mIdInfoWindowID);
	}
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().NVI_Close();
		NVI_DeleteWindow(mFileListWindowID);
	}
	*/
	_AInfo("AApp::Quit_Phase_UI() ended.",this);
}

void	AApp::Quit_Phase_Data()
{
	_AInfo("AApp::Quit_Phase_Data() started.",this);
	
	//�������L�����Ȃ��ݒ�̏ꍇ�́A�e�������폜
	if( NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )
	{
		//�ŋߊJ�����t�@�C���f�[�^�폜
		SPI_DeleteAllRecentlyOpenedFile();
		//���������폜
		NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFindString);
		NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kFindStringCache);
		NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kReplaceStringCache);
		NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder);
	}
	
	//�e�L�X�g�}�[�J�[index��DB�ɕۑ�
	NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kTextMarkerCurrentGroupIndex,mCurrentTextMarkerGroupIndex);
	
	//#717
	//�⊮��◚����DB�ɕۑ�
	NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kRecentCompletionWordArray,mRecentCompletionWordArray);
	
	//�u�����v���W�F�N�g�v�ۑ�
	if( mCurrentSameProjectIndex != kIndex_Invalid )
	{
		//#723
		AStMutexLocker	locker(mSameProjectArrayMutex);
		
		AText	path;
		mSameProject_ProjectFolder.GetObjectConst(mCurrentSameProjectIndex).GetPath(path);
		GetAppPref().SetData_Text(AAppPrefDB::kSameProjectFolderPath,path);
		//#533 GetAppPref().SetData_Number(AAppPrefDB::kSameProjectFolderModeIndex,mSameProject_ModeIndex.Get(mCurrentSameProjectIndex));
		GetAppPref().SetData_Number(AAppPrefDB::kSameProjectFolderModeIndex,mCurrentSameProjectModeIndex);//#533
	}
	//�u�����t�H���_�v�ۑ�
	if( mCurrentSameFolder != kIndex_Invalid )
	{
		//#932
		AStMutexLocker	locker(mSameFolderMutex);
		
		AText	path;
		mSameFolder_Folder.GetObjectConst(mCurrentSameFolder).GetPath(path);
		GetAppPref().SetData_Text(AAppPrefDB::kSameFolderPath,path);
	}
	//#454
	NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kJumpListHistory,mJumpListHistory);
	NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kJumpListHistory_Path,mJumpListHistory_Path);
	//#892
	//�t�@�C�����X�g�́u�����v���W�F�N�g�v�̐܂肽���ݏ�Ԃ��L��
	{
		//#723
		//�r������
		AStMutexLocker	locker(mSameProjectArrayMutex);
		
		//collapsedata��collapse���ׂ��t�H���_�̃p�X���i�[���Ă����ADB�ɕۑ�����i�Ⴄ�v���W�F�N�g�ł��������X�g���ɕۑ�����j
		ATextArray	collapsedata;
		//�v���W�F�N�g���̃��[�v
		AItemCount	projectCount = mSameProject_ProjectFolder.GetItemCount();
		for( AIndex i = 0; i < projectCount; i++ )
		{
			//�v���W�F�N�g�t�H���_�p�X���擾
			AText	projectfolderpath;
			mSameProject_ProjectFolder.GetObjectConst(i).GetPath(projectfolderpath);
			projectfolderpath.RemoveLastPathDelimiter();
			//�v���W�F�N�g���̐܂肽���܂ꂽ�t�H���_�̑��΃p�X���擾
			const ATextArray&	c = mSameProjectArray_CollapseData.GetObjectConst(i);
			//�܂肽���܂ꂽ�t�H���_�f�[�^���̃��[�v
			for( AIndex j = 0; j < c.GetItemCount(); j++ )
			{
				//�t�H���_�p�X���擾�i�v���W�F�N�g�̃p�X�{�܂肽���܂ꂽ�t�H���_�̑��΃p�X�j
				AText	t;
				c.Get(j,t);
				t.InsertText(0,projectfolderpath);
				//�t�H���_�p�X��ǉ�
				collapsedata.Add(t);
			}
		}
		//DB�ɕۑ�
		mAppPref.SetData_TextArray(AAppPrefDB::kFileListCollapseFolder,collapsedata);
	}
	
	//app pref DB���t�@�C���ɕۑ�
	mAppPref.SaveToFile();
	
	//���[�h�ς݂̃��[�h�ݒ��ۑ�����
	for( AIndex modeIndex = 0; modeIndex < mModePrefDBArray.GetItemCount(); modeIndex++ )
	{
		if( mModePrefDBArray.GetObject(modeIndex).IsLoaded() == true )
		{
			mModePrefDBArray.GetObject(modeIndex).SaveToFile();
		}
	}
	
	
	//B0446
	DeleteTemporaryTextFile();
	
	/*#427
	//#379 Diff�p���|�W�g���e�L�X�g�t�@�C���폜
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	AFileAcc	tmpFolder;
	tmpFolder.SpecifyChild(appPrefFolder,"temp");
	AObjectArray<AFileAcc>	children;
	tmpFolder.GetChildren(children);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		children.GetObject(i).DeleteFile();
	}
	*/
	/*#693 AApplication::~AApplication()�ֈړ�
	//temp�t�H���_�S�̂��폜�i�t�@�C���E�t�H���_���ׂāj
	AFileAcc	tempFolder;
	SPI_GetTempFolder(tempFolder);
	tempFolder.DeleteFileOrFolderRecursive();
	*/
	
	_AInfo("AApp::Quit_Phase_Data() ended.",this);
}

//#263
/**
�R�}���h���C���������s
*/
ABool	AApp::NVIDO_ExecCommandLineArgs( const ATextArray& inArgs )
{
	ABool	fileexist = false;
	ADocumentID	lastDocID = kObjectID_Invalid;
	for( AIndex i = 0; i < inArgs.GetItemCount(); i++ )
	{
		//
		AText	arg;
		inArgs.Get(i,arg);
		//�t�@�C���ȊO�̃R�}���h
		if( arg.GetItemCount() > 0 )
		{
			AUChar	ch = arg.Get(0);
			//�s�ԍ��w��
			if( ch == '+' && lastDocID != kObjectID_Invalid )
			{
				AIndex	paragraph = kIndex_Invalid;
				AIndex	p = 1;
				arg.ParseIntegerNumber(p,paragraph);
				paragraph--;
				if( paragraph < 0 )
				{
					paragraph = 0;
				}
				if( paragraph >= SPI_GetTextDocumentByID(lastDocID).SPI_GetParagraphCount() )
				{
					paragraph = SPI_GetTextDocumentByID(lastDocID).SPI_GetParagraphCount() - 1;
				}
				ATextPoint	textpt;
				textpt.x = 0;
				textpt.y = SPI_GetTextDocumentByID(lastDocID).SPI_GetLineIndexByParagraphIndex(paragraph);
				AIndex	textIndex = SPI_GetTextDocumentByID(lastDocID).SPI_GetTextIndexFromTextPoint(textpt);
				SPI_GetTextDocumentByID(lastDocID).SPI_SelectText(textIndex,0);
				continue;
			}
			//�J�����g�f�B���N�g���w�� #1111
			//�i�v���Z�X�d���N���ɂ��AAppleEvent�ŃR�}���h���C����M�����ꍇ�A���M���̃v���Z�X�̌��݃f�B���N�g����':'�̌�ɑ��M�����B�j
			if( ch == ':' )
			{
				arg.Delete1(0);
				AStCreateNSStringFromAText	curdirstr(arg);
				[[NSFileManager defaultManager] changeCurrentDirectoryPath:curdirstr.GetNSString()];
				continue;
			}
		}
		//�t�@�C�����J��
		AFileAcc	file;
		file.Specify(arg);
		if( file.Exist() == true )
		{
			lastDocID = SPNVI_CreateDocumentFromLocalFile(file);
			fileexist = true;
		}
	}
	return fileexist;
}

//#513
/**
�I�����J���Ă���t�@�C�����L��
*/
void	AApp::SPI_SaveReopenFile()
{
	//win �ݒ�m�F���R�[��������ړ�
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kReopenFile) == false )   return;
	
	//�f�[�^���܂��폜
	SPI_ClearReopenData();//#1102 �֐���
	//�E�C���h�Eindex�ireopen�f�[�^�̃E�C���h�E����ʂ��邽�߂�index�j
	AIndex	windowIndex = 0;//#850
	//�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			//�e�L�X�g�E�C���h�E�ɂ��ď���
			const AWindow_Text&	textWindow = SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID());
			//�T�C�h�o�[�A�T�u�e�L�X�g�\����Ԏ擾
			ABool	leftSideBarDisplayed = textWindow.SPI_IsLeftSideBarDisplayed();
			ABool	rightSideBarDisplayed = textWindow.SPI_IsRightSideBarDisplayed();
			ABool	subTextDisplayed = textWindow.SPI_IsSubTextColumnDisplayed();
			//���C���J������bounds���擾
			ARect	bounds = {0};
			textWindow.SPI_GetWindowMainColumnBounds(bounds);
			//�^�u���̃��[�v(z-order��)
			ABool	reopenTabZOrder = 0;
			for( AIndex zorder = 0; zorder < textWindow.NVI_GetSelectableTabCount(); zorder++ )
			{
				//�^�u�̕\��index�擾
				AIndex	tabIndex = textWindow.NVI_GetTabIndexByZOrderIndex(zorder);
				AIndex	tabDisplayIndex = textWindow.SPI_GetDisplayTabIndexByTabIndex(tabIndex);
				//�h�L�������g�擾
				const ADocument_Text&	document = GetApp().SPI_GetTextDocumentByID(textWindow.NVI_GetDocumentIDByTabIndex(tabIndex));
				//#630 ODB�h�L�������g�̓��I�[�v�����Ȃ��悤�ɂ���
				if( document.SPI_IsODBMode() == true )
				{
					continue;
				}
				//���I�[�v���p�t�@�C���p�X�擾
				AText	path;
				ABool	isRemoteFile = false;
				document.SPI_GetReopenFilePath(path,isRemoteFile);
				//�p�X�e�L�X�g����Ȃ�f�[�^�ǉ����Ȃ�
				if( path.GetItemCount() == 0 )
				{
					continue;
				}
				//���݃T�u�e�L�X�g�ɕ\������Ă��邩�ǂ������擾
				ABool	shownInSubText = false;
				if( textWindow.SPI_GetCurrentSubTextPaneDocumentID() == document.GetObjectID() )
				{
					shownInSubText = true;
				}
				//reopen�f�[�^�ǉ�
				NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kReopenFile_Path,path);
				NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kReopenFile_Selected,
							NVI_GetMostFrontDocumentID(kDocumentType_Text) == document.GetObjectID());
				NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kReopenFile_IsMainWindow,false);
				NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kReopenFile_WindowIndex,windowIndex);
				NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kReopenFile_TabZOrder,reopenTabZOrder);
				NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kReopenFile_TabDisplayIndex,tabDisplayIndex);
				NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kReopenFile_IsRemoteFile,isRemoteFile);
				NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kReopenFile_WindowWidth,bounds.right-bounds.left);
				NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kReopenFile_WindowHeight,bounds.bottom-bounds.top);
				NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kReopenFile_WindowX,bounds.left);
				NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kReopenFile_WindowY,bounds.top);
				NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kReopenFile_DisplayLeftSideBar,leftSideBarDisplayed);
				NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kReopenFile_DisplayRightSideBar,rightSideBarDisplayed);
				NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kReopenFile_DisplaySubText,subTextDisplayed);
				NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kReopenFile_ShownInSubText,shownInSubText);
				//�^�uz-order���C���N�������g
				reopenTabZOrder++;
			}
			//�E�C���h�Eindex�C���N�������g
			windowIndex++;
		}
	}
}

//#1102
/**
reopen�f�[�^���N���A
*/
void	AApp::SPI_ClearReopenData()
{
	NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kReopenFile_Path);
	NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kReopenFile_Selected);
	NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kReopenFile_IsMainWindow);
	NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kReopenFile_WindowIndex);//#850
	NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kReopenFile_TabZOrder);//#850
	NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kReopenFile_TabDisplayIndex);//#850
	NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kReopenFile_IsRemoteFile);//#850
	NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kReopenFile_WindowWidth);//#850
	NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kReopenFile_WindowHeight);//#850
	NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kReopenFile_WindowX);//#850
	NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kReopenFile_WindowY);//#850
	NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kReopenFile_DisplayLeftSideBar);//#850
	NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kReopenFile_DisplayRightSideBar);//#850
	NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kReopenFile_DisplaySubText);//#850
	NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kReopenFile_ShownInSubText);
}

//#513
/**
�I�����ɊJ���Ă����t�@�C�����A�N�����ɍēx�J��
*/
void	AApp::ReopenFile()
{
	//#573 �܂���U���[�J���ϐ��ɓǂݍ���ŁADB�̂ق��̓f�[�^�������ĕۑ����Ă����B
	//�t�@�C�����J���̂Ɉُ�Ɏ��Ԃ�������ꍇ(#571��)�A���xmi���N�����Ă������ɂȂ�̂�h�~���邽��
	ATextArray	pathArray;
	ABoolArray	selectedArray;
	ANumberArray	tabZOrderArray;
	ANumberArray	tabDisplayIndexArray;
	ABoolArray	isRemoteFileArray;
	ANumberArray	windowWidthArray, windowHeightArray, windowXArray, windowYArray;
	ABoolArray	displayLeftSideBarArray, displayRightSideBarArray, displaySubTextArray, shownInSubTextArray;
	//�f�[�^�擾
	pathArray.SetFromTextArray(
				NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kReopenFile_Path));
	selectedArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_BoolArrayRef(AAppPrefDB::kReopenFile_Selected));
	tabZOrderArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_NumberArrayRef(AAppPrefDB::kReopenFile_TabZOrder));
	tabDisplayIndexArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_NumberArrayRef(AAppPrefDB::kReopenFile_TabDisplayIndex));
	isRemoteFileArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_BoolArrayRef(AAppPrefDB::kReopenFile_IsRemoteFile));
	windowWidthArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_NumberArrayRef(AAppPrefDB::kReopenFile_WindowWidth));
	windowHeightArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_NumberArrayRef(AAppPrefDB::kReopenFile_WindowHeight));
	windowXArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_NumberArrayRef(AAppPrefDB::kReopenFile_WindowX));
	windowYArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_NumberArrayRef(AAppPrefDB::kReopenFile_WindowY));
	displayLeftSideBarArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_BoolArrayRef(AAppPrefDB::kReopenFile_DisplayLeftSideBar));
	displayRightSideBarArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_BoolArrayRef(AAppPrefDB::kReopenFile_DisplayRightSideBar));
	displaySubTextArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_BoolArrayRef(AAppPrefDB::kReopenFile_DisplaySubText));
	shownInSubTextArray.SetFromObject(
				NVI_GetAppPrefDB().GetData_BoolArrayRef(AAppPrefDB::kReopenFile_ShownInSubText));
	/* mi�N������reopen�f�[�^�폜�͂��Ȃ����Ƃɂ���i�r���I���Ƃ��Ń^�u��������̂��_���[�W�傫���̂ŁB�j #1239
	//reopen�f�[�^�폜
	SPI_ClearReopenData();//#1102
	NVI_GetAppPrefDB().SaveToFile();
	*/
	
	//#923
	//�ŋߊJ�����t�@�C���ւ̒ǉ���}��
	mSuppressAddToRecentlyOpenedFile = true;
	//
	AHashArray<AObjectID>				createdWinIDArray;
	AObjectArray<AHashNumberArray>		createdWinIDArray_TabDisplayIndexInDataArray;
	AObjectArray< AArray<AObjectID> >	createdWinIDArray_DocumentIDArray;
	AArray<ANumber>						createdWinIDArray_MaxTabDisplayIndex;
	//�t�@�C�����J��
	AWindowID	winID = kObjectID_Invalid;//#852
	ADocumentID	selectDocumentID = kObjectID_Invalid;
	AItemCount	itemCount = pathArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//==================bounds�擾==================
		ARect	bounds = {0};
		bounds.left		= windowXArray.Get(i);
		bounds.top		= windowYArray.Get(i);
		bounds.right	= bounds.left + windowWidthArray.Get(i);
		bounds.bottom	= bounds.top  + windowHeightArray.Get(i);
		//==================�E�C���h�E��������==================
		//�ŏ��̍��ځA�܂��́A��ԏ�̃^�u�̏ꍇ�A�E�C���h�E�����t���O��ON�ɂ���
		ABool	createWindow = false;
		if( i == 0 || tabZOrderArray.Get(i) == 0 )
		{
			createWindow = true;
		}
		//==================�h�L�������g����==================
		ADocumentID	docID = kObjectID_Invalid;
		if( isRemoteFileArray.Get(i) == false )
		{
			//------------------���[�J���t�@�C���̏ꍇ------------------
			//�p�X�擾
			AFileAcc	file;
			file.Specify(pathArray.GetTextConst(i));
			//�h�L�������g����
			//#932 ���݃`�F�b�N�����Ƀh�L�������g�^�E�C���h�E�������� if( file.Exist() == true )
			{
				//�t�@�C�������݂��Ă�����A�h�L�������g�𐶐��i�E�C���h�E�������邩�ǂ����́A��ł̔���ɏ]���j
				docID = SPNVI_CreateDocumentFromLocalFile(file,GetEmptyText(),kObjectID_Invalid,true,
							kIndex_Invalid,kIndex_Invalid,(i==0),winID,createWindow,
							bounds,true,displayLeftSideBarArray.Get(i),displayRightSideBarArray.Get(i),displaySubTextArray.Get(i));
			}
			/*#932
			else
			{
				//�t�@�C�������݂��Ă��Ȃ��ꍇ�͉������Ȃ�
				continue;
			}
			 */
		}
		else
		{
			//------------------�����[�g�t�@�C���̏ꍇ------------------
			//���L���ƃ^�u�\���ʒu���ς���Ă��܂��̂ŁA�h�L�������g������Ƀ����[�g�t�@�C���ǂݍ��݂���Ή����K�v�B 
			//�^�u�ʒu�͕ς���Ă��܂����A�Ƃ肠�����Ή� #1000
			AText	remoteFileURL;
			pathArray.Get(i,remoteFileURL);
			SPI_RequestOpenFileToRemoteConnection(remoteFileURL,false);
			continue;
		}
		//�A�N�e�B�u�������Ȃ�I��ΏۃE�C���h�E�ɐݒ�i���internal event�ɂ��E�C���h�Eselect�j
		if( selectedArray.Get(i) == true )
		{
			selectDocumentID = docID;
		}
		//==================�����E�C���h�E���X�g==================
		//�h�L�������g�̃E�C���h�E���擾
		winID = SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
		//�����E�C���h�E���X�g����E�C���h�E������
		AIndex	createdWinIDIndex = createdWinIDArray.Find(winID);
		if( createdWinIDIndex == kIndex_Invalid )
		{
			//���������E�C���h�E�ɂ��āAupdate view bounds��}��
			SPI_GetTextWindowByID(winID).SPI_IncrementSuppressUpdateViewBoundsCounter();
			//�����E�C���h�E���X�g�ɍ��ڒǉ�
			createdWinIDIndex = createdWinIDArray.Add(winID);
			createdWinIDArray_TabDisplayIndexInDataArray.AddNewObject();
			createdWinIDArray_DocumentIDArray.AddNewObject();
			createdWinIDArray_MaxTabDisplayIndex.Add(-1);
		}
		//�����E�C���h�E���X�g�Ɋe�^�u�̃f�[�^��ۑ�
		AIndex	tabDisplayIndex = tabDisplayIndexArray.Get(i);
		createdWinIDArray_TabDisplayIndexInDataArray.GetObject(createdWinIDIndex).Add(tabDisplayIndex);
		createdWinIDArray_DocumentIDArray.GetObject(createdWinIDIndex).Add(docID);
		if( tabDisplayIndex > createdWinIDArray_MaxTabDisplayIndex.Get(createdWinIDIndex) )
		{
			createdWinIDArray_MaxTabDisplayIndex.Set(createdWinIDIndex,tabDisplayIndex);
		}
		//==================�T�u�e�L�X�g�\������==================
		//�T�u�e�L�X�g�ɕ\������Ă������ǂ����̃t���O��ON���A�T�u�e�L�X�g�J�����\������Ă���Ȃ�A�T�u�e�L�X�g�ɕ\������
		if( SPI_GetTextWindowByID(winID).SPI_IsSubTextColumnDisplayed() == true )
		{
			if( shownInSubTextArray.Get(i) == true )
			{
				SPI_GetTextWindowByID(winID).SPI_DisplayTabInSubText(SPI_GetTextWindowByID(winID).NVI_GetTabIndexByDocumentID(docID),true);
			}
		}
	}
	//==================�����E�C���h�E���X�g���̃��[�v==================
	for( AIndex i = 0; i < createdWinIDArray.GetItemCount();i++ )
	{
		AWindowID	winID = createdWinIDArray.Get(i);
		//------------------�^�u�\�����̎Ⴂ�����珇�Ƀ^�u�\������ݒ肵�Ă���------------------
		AIndex	displayTabIndex = 0;
		AIndex	displayTabIndexMax = createdWinIDArray_MaxTabDisplayIndex.Get(i);
		for( AIndex j = 0; j <= displayTabIndexMax; j++ )
		{
			//�^�u�\������j�Ԗڂ�index������
			AIndex	index = createdWinIDArray_TabDisplayIndexInDataArray.GetObjectConst(i).Find(j);
			if( index != kIndex_Invalid )
			{
				//����index�i�^�u�\������j�Ԗڂ̍��ځj�̃^�u�ɕ\������ݒ�
				ADocumentID	docID = createdWinIDArray_DocumentIDArray.GetObjectConst(i).Get(index);
				SPI_GetTextWindowByID(winID).SPI_SetDisplayTabIndexByDocumentID(docID,displayTabIndex);
				//�^�u�\��index�C���N�������g�idisplayTabIndex��j�ł��ǂ���������Ȃ��H�j
				displayTabIndex++;
			}
		}
		//update view bounds�̗}��������
		SPI_GetTextWindowByID(winID).SPI_DecrementSuppressUpdateViewBoundsCounter();
		//update view bounds
		SPI_GetTextWindowByID(winID).SPI_UpdateViewBounds();
	}
	//�E�C���h�E�I��
	if( selectDocumentID != kObjectID_Invalid )
	{
		//#678 SPI_GetTextDocumentByID(selectDocumentID).NVI_RevealDocumentWithAView();
		//#678 deactivate/activate�̃C�x���g����R�������ĕs�����������Ă�����ۂ��̂ŁA
		//��U��L�ɂ��deactivate/activate����������AInternal Event�ŃE�C���h�Eselect���邱�Ƃɂ���
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(selectDocumentID);
		ABase::PostToMainInternalEventQueue(kInternalEvent_RevealDocument,GetObjectID(),0,GetEmptyText(),objectIDArray);
	}
	//#688
	//Reopen�����I���t���O�ݒ�iSPI_CreateNewWindowAutomatic()�ł̐V�K�t�@�C�������֎~�����j
	mReopenProcessCompleted = true;
	//�����V�K�t�@�C������
	//�i�N������EVT_OpenUntitledFile()�o�R��SPI_CreateNewWindowAutomatic()���R�[������邪�A
	//�@mReopenProcessCompleted�t���O��false�̊Ԃ͎��s�����Aprepend�t���O��true�ɂ���݂̂Ƃ��Ă���B
	//�@prepend���ꂽ�V�K�t�@�C���������������������Ŏ��s����B #1056�j
	DoPrependedOpenUntitledFileEvent();
	//#923
	//�ŋߊJ�����t�@�C���ւ̒ǉ���}��������
	mSuppressAddToRecentlyOpenedFile = false;
}

//#81
/**
���ۑ��f�[�^�̕\��
*/
void	AApp::OpenUnsavedData()
{
	//UnsavedData�t�H���_�擾
	AFileAcc	unsavedDataRootFolder;
	GetApp().SPI_GetUnsavedDataFolder(unsavedDataRootFolder);
	//UnsavedData�t�H���_���̃t�H���_���X�g���擾
	AObjectArray<AFileAcc>	unsavedDataFolderArray;
	unsavedDataRootFolder.GetChildren(unsavedDataFolderArray);
	//UnsavedData�t�H���_���̊e�t�H���_���Ƃ̃��[�v
	AItemCount	itemCount = unsavedDataFolderArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//UnsavedData�t�H���_���̊e�t�H���_���擾
		AFileAcc	folder;
		folder.CopyFrom(unsavedDataFolderArray.GetObjectConst(i));
		//�t�H���_����AutoSave�`���̖��O�̃t�@�C�����X�g���擾
		AArray<ANumber64bit>	fileNameArray;
		ADocument_Text::GetAutoBackupNumberArray(folder,fileNameArray);
		if( fileNameArray.GetItemCount() > 0 )
		{
			//AutoSave�`���̖��O�̃t�@�C�����X�g�̍Ō�̍��ڂ��擾
			AText	filename;
			filename.SetNumber64bit(fileNameArray.Get(fileNameArray.GetItemCount()-1));
			
			//==================���ۑ��f�[�^���ۑ����ꂽ�t�@�C�����J��==================
			//�t�@�C�����擾
			AFileAcc	unsavedfile;
			unsavedfile.SpecifyChild(folder,filename);
			//�h�L�������g�̃t�@�C���p�X��ǂݍ���
			AFileAcc	filepathfile;
			filepathfile.SpecifyChild(folder,"filepath");
			AText	filepath;
			filepathfile.ReadTo(filepath);
			//�h�L�������g�t�@�C�����擾
			AFileAcc	docfile;
			docfile.Specify(filepath);
			//�h�L�������g����
			ADocumentID	docID = kObjectID_Invalid;
			if( docfile.Exist() == true )
			{
				//�h�L�������g�t�@�C�������݂��Ă���΁A���̃t�@�C������h�L�������g����
				docID = SPNVI_CreateDocumentFromLocalFile(docfile);
			}
			else
			{
				//�h�L�������g�t�@�C�������݂��Ă��Ȃ���΁A�V�K�h�L�������g����
				docID = SPNVI_CreateNewTextDocument();
			}
			//
			//�����̂܂܏I���΍��K�vSPI_GetTextDocumentByID(docID).NVI_SetDirty(true);
			//�������[�g�t�@�C���Ή�
			
			//==================���ۑ��f�[�^�t�@�C�����r�Ώۂɂ���==================
			//���������h�L�������g�ɔ�r���[�h�ݒ�i���ۑ��f�[�^�Ƃ̔�r�j
			SPI_GetTextDocumentByID(docID).SPI_SetDiffMode_File(unsavedfile,kCompareMode_UnsavedData);
			//��r�\��
			AWindowID	winID = SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			SPI_GetTextWindowByID(winID).SPI_SetDiffDisplayMode(true);
			//notification�\��
			SPI_GetTextWindowByID(winID).SPI_GetMainTextViewForDocumentID(docID).SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_SetNotification_UnsavedFile();
			SPI_GetTextWindowByID(winID).SPI_GetMainTextViewForDocumentID(docID).SPI_ShowNotificationPopupWindow(true);
		}
		//�t�H���_�폜
		folder.DeleteFileOrFolderRecursive();
	}
}

/**
�J���[�X�L�[�����j���[�X�V
*/
void	AApp::SPI_UpdateColorSchemeMenu()
{
	ATextArray	textArray;
	AText	text;
	//#1316
	ATextArray	textArray2;
	text.SetLocalizedText("ModeColors");
	textArray2.Add(text);
	//------------------�A�v�����J���[�X�L�[��------------------
	{
		//�J���[�X�L�[���t�H���_�擾
		AFileAcc	folder;
		SPI_GetAppColorSchemeFolder(folder);
		//�J���[�X�L�[���t�H���_���t�@�C�����Ƃ̃��[�v
		AObjectArray<AFileAcc>	childArray;;
		folder.GetChildren(childArray);
		AItemCount	itemCount = childArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//�t�@�C�����擾
			AText	filename;
			childArray.GetObjectConst(i).GetFilename(filename);
			//���O�t�@�C��
			if( filename.Compare(".DS_Store") == true )   continue;
			//�g���q�擾
			AText	suffix;
			filename.GetSuffix(suffix);
			if( suffix.Compare(".csv") == true )
			{
				//".csv"�����폜
				filename.DeleteAfter(filename.GetItemCount()-4);
			}
			//���j���[�\�����X�g�Ƀe�L�X�g�ǉ�
			textArray.Add(filename);
			textArray2.Add(filename);//#1316
		}
	}
	//------------------���[�U�[��`�J���[�X�L�[��------------------
	{
		//�J���[�X�L�[���t�H���_�擾
		AFileAcc	folder;
		SPI_GetUserColorSchemeFolder(folder);
		//�J���[�X�L�[���t�H���_���t�@�C�����Ƃ̃��[�v
		AObjectArray<AFileAcc>	childArray;;
		folder.GetChildren(childArray);
		AItemCount	itemCount = childArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//�t�@�C�����擾
			AText	filename;
			childArray.GetObjectConst(i).GetFilename(filename);
			//���O�t�@�C��
			if( filename.Compare(".DS_Store") == true )   continue;
			//�g���q�擾
			AText	suffix;
			filename.GetSuffix(suffix);
			if( suffix.Compare(".csv") == true || suffix.Compare(".epf") == true )
			{
				//".csv"�����폜
				filename.DeleteAfter(filename.GetItemCount()-4);
			}
			//
			if( filename.Compare("LastOverwrittenColors") == true )
			{
				filename.SetLocalizedText("LastOverwrittenColors");
			}
			else
			{
				//#1316
				textArray2.Add(filename);
			}
			//���j���[�\�����X�g�Ƀe�L�X�g�ǉ�
			textArray.Add(filename);
		}
	}
	
	//------------------���j���[�X�V------------------
	NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_ColorScheme,textArray);
	
	//#1316
	NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_AppColorScheme,textArray2);
}

//#193
/**
�A�N�Z�X�v���O�C�����j���[�X�V
*/
void	AApp::SPI_UpdateAccessPlugInMenu()
{
	mAccessPluginMenuTextArray.DeleteAll();
	//
	AText	text;
	//------------------�A�v�����A�N�Z�X�v���O�C���t�H���_------------------
	{
		//�A�N�Z�X�v���O�C���t�H���_�擾
		AFileAcc	folder;
		SPI_GetAppAccessPluginFolder(folder);
		//�A�N�Z�X�v���O�C���t�H���_���t�@�C�����Ƃ̃��[�v
		AObjectArray<AFileAcc>	childArray;;
		folder.GetChildren(childArray);
		AItemCount	itemCount = childArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//�t�@�C���擾
			AFileAcc	file = childArray.GetObjectConst(i);
			//�t�@�C�����擾
			AText	filename;
			file.GetFilename(filename);
			//
			if( file.IsFolder() == false && file.IsInvisible() == false )
			{
				//���j���[�\�����X�g�Ƀe�L�X�g�ǉ�
				mAccessPluginMenuTextArray.Add(filename);
			}
		}
	}
	//#1231 ���j���[���ԓ���ւ��iFTP�͂����Z�L�����e�B�㐄���ł��Ȃ��̂Łj
	text.SetCstring("FTP");
	mAccessPluginMenuTextArray.Add(text);
	/*�����Ή�
	//------------------���[�U�[��`�A�N�Z�X�v���O�C���t�H���_------------------
	{
		//�A�N�Z�X�v���O�C���t�H���_�擾
		AFileAcc	folder;
		SPI_GetUserAccessPluginFolder(folder);
		//�A�N�Z�X�v���O�C���t�H���_���t�@�C�����Ƃ̃��[�v
		AObjectArray<AFileAcc>	childArray;;
		folder.GetChildren(childArray);
		AItemCount	itemCount = childArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//�t�@�C���擾
			AFileAcc	file = childArray.GetObjectConst(i);
			//�t�@�C�����擾
			AText	filename;
			file.GetFilename(filename);
			//
			if( file.IsFolder() == false && file.IsInvisible() == false )
			{
				//���j���[�\�����X�g�Ƀe�L�X�g�ǉ�
				mAccessPluginMenuTextArray.Add(filename);
			}
		}
	}
	*/
	//------------------���j���[�X�V------------------
	NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_AccessPlugIn,mAccessPluginMenuTextArray);
}

/**
�o�[�W����2.1�ł̕W���Y�t�c�[���o�[���́i�o�[�W����3�ڍs���ɔ�\���ɂ��ׂ����́j���ǂ������擾
*/
ABool	AApp::SPI_IsV2ToolbarItemNameToHide( const AText& inName ) const
{
	return (mV2ToolbarItemNameToHideArray.Find(inName)!=kIndex_Invalid);
}

/**
skin���j���[�X�V
*/
/*#1316
void	AApp::SPI_UpdateSkinMenu()
{
	ATextArray	textArray;
	AText	text;
	text.SetLocalizedText("DefaultSkinName");
	textArray.Add(text);
	//------------------skin�t�H���_�̃t�H���_�����j���[�ɒǉ�------------------
	//skin�t�H���_�擾
	AFileAcc	folder;
	SPI_GetUserSkinFolder(folder);
	//skin�t�H���_���t�@�C�����Ƃ̃��[�v
	AObjectArray<AFileAcc>	childArray;;
	folder.GetChildren(childArray);
	AItemCount	itemCount = childArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AFileAcc	file;
		file = childArray.GetObjectConst(i);
		//�t�@�C�����擾
		AText	filename;
		file.GetFilename(filename);
		//
		if( file.IsFolder() == true )
		{
			//���j���[�\�����X�g�Ƀe�L�X�g�ǉ�
			textArray.Add(filename);
		}
	}
	//------------------���j���[�X�V------------------
	NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_Skin,textArray);
}
*/

/**
skin���[�g�t�H���_�擾
*/
/*#1316
void	AApp::SPI_GetUserSkinFolder( AFileAcc& outFolder )
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChild(appPrefFolder,"Skin");
	outFolder.CreateFolderRecursive();
}
*/

//#1003
/**
URL�X�L�[�����s
*/
void	AApp::NVIDO_ExecuteURIScheme( const AText& inURI )
{
	ATextArray	textArray;
	textArray.ExplodeFromText(inURI,'/');
	if( textArray.GetItemCount() >= 3 )
	{
		if( textArray.GetTextConst(0).Compare("net.mimikaki.mi:") == true )
		{
			if( textArray.GetTextConst(2).Compare("modepref") == true )
			{
				if( textArray.GetItemCount() >= 5 )
				{
					AText	modeName;
					textArray.Get(3,modeName);
					AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
					if( modeIndex != kIndex_Invalid )
					{
						GetApp().SPI_ShowModePrefWindow(modeIndex);
						ANumber	modePrefWindowTabIndex = textArray.GetTextConst(4).GetNumber();
						GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SelectTabControl(modePrefWindowTabIndex);
						GetApp().SPI_GetModePrefWindow(modeIndex).NVI_RefreshWindow();
						GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SwitchFocusToFirst();
					}
				}
			}
		}
	}
}

//#1050
/**
�^�u�Z�b�g����J��
*/
void	AApp::OpenTabSet( const AFileAcc& inTabSetFile )
{
	ABool	first = true;
	AWindowID	winID = kObjectID_Invalid;
	//�^�u�Z�b�g�t�@�C���ǂݍ���
	AText	text;
	inTabSetFile.ReadTo(text);
	for( AIndex pos = 0; pos < text.GetItemCount(); )
	{
		//�s���Ƃɓǂݍ��݁A�t�@�C������
		AText	path;
		text.GetLine(pos,path);
		if( path.Contains("://") == true )
		{
			//�����[�g�t�@�C���I�[�v��
			SPI_RequestOpenFileToRemoteConnection(path,false);
		}
		else
		{
			//�h�L�������g����
			AFileAcc	file;
			file.Specify(path);
			ADocumentID	docID = SPNVI_CreateDocumentFromLocalFile(file,GetEmptyText(),kObjectID_Invalid,true,kIndex_Invalid,kIndex_Invalid,false,
																  winID,first);
			//�E�C���h�E�擾�i����ȍ~�ɓ����E�C���h�E�Ƀ^�u���J���j
			winID = SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			first = false;
		}
	}
}

//#1034
/**
�E�C���h�E���E�C���h�Eorder���Ɏ擾
�iAppleScript����g�p�����B�j
�h���N���X�ɂ��E�C���h�E�ǉ�
*/
void	AApp::NVIDO_GetOrderedWindowIDArray( AArray<AWindowID>& outArray, const AWindowType inWindowType )
{
	AWindowID	winID = SPI_GetCurrentFindResultWindowID();
	if( winID != kObjectID_Invalid )
	{
		outArray.Insert1(0,winID);
	}
}

#pragma mark ===========================

#pragma mark ---���[�h������

/**
ModePref������
*/
void	AApp::InitModePref()
{
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_InitMode,false,false,true);
	
	//==================���[�g�̃��[�h�t�H���_�i"mode"�j�擾�i�������Ȃ琶���j==================
	//mode�t�H���_�쐬�^����
	AFileAcc	rootModeFolder;
	SPI_GetModeRootFolder(rootModeFolder);//�A�v���P�[�V����(.app)�Ɠ����_mode�t�H���_�i�D��j���APref�t�H���_��mode�t�H���_
	if( rootModeFolder.Exist() == false )
	{
		rootModeFolder.CreateFolderRecursive();
	}
	
	//==================�W�����[�h�t�H���_�擾�i���u�W���v�t�H���_�̖��̕ϊ��j==================
	
	//�W�����[�h�t�@�C���i�t�H���_�j�� #305
	//�o�[�W����2.1.10�ȑO�F�u�W���v
	//�o�[�W����2.1.11�Ȍ�F�uNormal�v
	//�u�W���v�t�H���_�����݂��āA�uNormal�v�t�H���_�����݂��Ȃ��ꍇ�A�u�W���v�t�H���_�ƒ��̃��[�h�t�@�C�����uNormal�v�ɖ��̕ύX����
	//�i��������s���Ă��o�[�W����2.1.11�ȍ~�Ŏg�p�������A�]���́u�W���v�t�H���_���W���t�H���_�Ƃ��Ĉ�����B
	//�܂��A�����ꖼ�̂̓��{��ɂ͕K���u�W���v���Z�b�g�����̂ŁA�o�[�W����2.1.11�ȍ~�ł��\�����́u�W���v�ɂȂ�B
	//�W�����[�h���E�t�H���_�擾
	AText	normalModeName("Normal");
	AFileAcc	normalModeFolder;
	normalModeFolder.SpecifyChild(rootModeFolder,normalModeName);
	//���W�����[�h���E�t�H���_�擾
	AText	oldNormalModeName;
	oldNormalModeName.SetLocalizedText("StandardModeFileNameOld");
	AFileAcc	oldNormalModeFolder;
	oldNormalModeFolder.SpecifyChild(rootModeFolder,oldNormalModeName);
	//�u�W���v�t�H���_�����݂��āA�uNormal�v�t�H���_�����݂��Ȃ��ꍇ�A�t�H���_���ƃ��[�h�t�@�C������Normal�ɖ��̕ύX����
	if( oldNormalModeFolder.Exist() == true && normalModeFolder.Exist() == false  )
	{
		//���[�h�t�H���_�𖼏̕ύX
		oldNormalModeFolder.Rename(normalModeName);
		//���[�h�t�@�C���𖼏̕ύX�i�o�[�W����3�ł͎g�p���Ȃ����A�o�[�W����2.1.11�ȍ~�ŋN�����鎞�ɕK�v�B�j
		AFileAcc	oldNormalModeFile;
		oldNormalModeFile.SpecifyChild(oldNormalModeFolder,oldNormalModeName);
		oldNormalModeFile.Rename(normalModeName);
	}
	
	//==================�A�v�������[�h�f�t�H���g�t�H���_�ɂ����āA������ɂȂ����[�h���R�s�[����==================
	//#1374 data/ModePreferences.mi�̂݃R�s�[����B����ȊO�̓A�v�������Q�Ƃ���悤�ɂ����̂ŁA�R�s�[�s�v�B
	
	//�f�t�H���g�t�H���_�擾
	AFileAcc	defaultFolder;
	SPI_GetModeDefaultFolder(defaultFolder);
	
	//�f�t�H���g�t�H���_�̎q���擾
	AObjectArray<AFileAcc>	defaultFolderArray;
	defaultFolder.GetChildren(defaultFolderArray);
	//�f�t�H���g�t�H���_�̎q���Ƃ̃��[�v
	for( AIndex i = 0; i < defaultFolderArray.GetItemCount(); i++ )
	{
		//�q�t�@�C���^�t�H���_���擾
		AFileAcc	srcModeFolder = defaultFolderArray.GetObjectConst(i);
		//�t�H���_�ȊO�͉������Ȃ��Ń��[�v�p��
		if( srcModeFolder.IsFolder() == false )
		{
			continue;
		}
		//�f�t�H���g�t�H���_�̃��[�h�̃��[�h���擾
		AText	srcModeName;
		srcModeFolder.GetFilename(srcModeName);
		//�������O�̃��[�h�t�H���_���擾
		AFileAcc	dstModeFolder;
		dstModeFolder.SpecifyChild(rootModeFolder,srcModeName);
		//�������O�̃��[�h�t�H���_���Ȃ���΁A�f�t�H���g�t�H���_�̃��[�h���R�s�[����
		if( dstModeFolder.Exist() == false )
		{
			//�Z�b�V�����v���O���X�X�V
			SPI_CheckContinueingEditProgressModalSession(kEditProgressType_InitMode,0,true,i/2,defaultFolderArray.GetItemCount(),true);//#1374
			
			/*#1374 ModePreferences.mi�t�@�C�������R�s�[����悤�ɕύX�B
			//�f�t�H���g�t�H���_�̃��[�h�𓯂����O�ŃR�s�[����
			srcModeFolder.CopyFolderTo(dstModeFolder,true,true);
			*/
			
			//ModePreferences.mi�t�@�C���̂݁A�f�t�H���g�t�H���_�i�A�v�����t�H���_�j����A���[�U�[mode�t�H���_�փR�s�[����
			AFileAcc	dstFolder;
			dstFolder.SpecifyChild(dstModeFolder,"data");
			dstFolder.CreateFolderRecursive();
			AFileAcc	srcFile, dstFile;
			srcFile.SpecifyChild(srcModeFolder,"data/ModePreferences.mi");
			dstFile.SpecifyChild(dstModeFolder,"data/ModePreferences.mi");
			srcFile.CopyFileTo(dstFile,true);
			
			//�V�K�ɐ����������[�h���̃��X�g�ɒǉ�
			mNewlyCreatedModeNameArray.Add(srcModeName);
		}
	}
	
	//==================�W�����[�h��AModePrefDB�쐬,Load==================
	//�V�K�����������ǂ����擾
	ABool	normalModeCreatedNewly = (mNewlyCreatedModeNameArray.Find(normalModeName)!=kIndex_Invalid);
	//���[�hpref db�I�u�W�F�N�g����
	AIndex	modeIndex = AddMode(normalModeFolder,normalModeCreatedNewly,false);//#427
	//���[�h���[�h
	mModePrefDBArray.GetObject(modeIndex).LoadOrWaitLoadComplete(true);
	//�W�����[�h�̕\�����͏�ɌŒ薼�̂ɂ���B���{�ꖼ�̂́u�W���v
	mModePrefDBArray.GetObject(modeIndex).SetNormalModeNames();
	//
	NVI_GetMenuManager().RealizeDynamicMenu(mModePrefDBArray.GetObject(modeIndex).GetToolMenuRootObjectID());
	
	//==================�e���[�h��AModePrefDB����==================
	//���[�h�t�H���_�̃��X�g���擾
	AObjectArray<AFileAcc>	modeFolderArray;
	rootModeFolder.GetChildren(modeFolderArray);
	
	//modeFolderArray����A�W�����[�h�ȊO��AModePrefDB���쐬
	for( AIndex i = 0; i < modeFolderArray.GetItemCount(); i++ )
	{
		//�Z�b�V�����v���O���X�X�V
		if( mNewlyCreatedModeNameArray.GetItemCount() > 0 )
		{
			SPI_CheckContinueingEditProgressModalSession(kEditProgressType_InitMode,0,true,modeFolderArray.GetItemCount()/2+i/2,modeFolderArray.GetItemCount());
		}
		
		//�t�@�C���^�t�H���_�擾
		AFileAcc	modeFolder;
		modeFolder = modeFolderArray.GetObjectConst(i);
		
		//�t�H���_�ȊO�͑ΏۊO�Ȃ̂ŉ����������[�v�p��
		if( modeFolder.IsFolder() == false )
		{
			continue;
		}
		
		//�t�H���_���擾
		AText	modeName;
		modeFolder.GetFilename(modeName);
		
		//�W�����[�h�͂��łɍ쐬�ς݂Ȃ̂ŉ����������[�v�p��
		if( modeName.Compare(normalModeName) == true )
		{
			continue;
		}
		
		//------------------���[�h�쐬------------------
		//�V�K�����������ǂ����擾
		ABool	modeCreatedNewly = (mNewlyCreatedModeNameArray.Find(modeName)!=kIndex_Invalid);
		//���[�hpref db�I�u�W�F�N�g����
		AddMode(modeFolderArray.GetObjectConst(i),modeCreatedNewly,false);//#427
	}
	
	/*�����X�V�͂Ƃ肠�����ۗ��B�܂��X�V�{�^���N���b�N�ł̍X�V�̂ݑΉ�����B
	//#427 AutoUpdate
	//Revision�`�F�b�N
	for( AIndex i = 0; i < mModePrefDBArray.GetItemCount(); i++ )
	{
		SPI_ModeUpdate(i,false);
	}
	*/
	
	//�W�����[�h�𐶐������Ƃ��i���ŏ��̋N�����j�́A�͂��߂ɂ��ǂ݂���������\������ #1351 #1333
	if( normalModeCreatedNewly == true )
	{
		//#1374 SPI_ShowReadMeFirst();
		AObjectIDArray	objectIDArray;
		ABase::PostToMainInternalEventQueue(kInternalEvent_ShowReadMe,GetObjectID(),0,GetEmptyText(),objectIDArray);
	}
}

//#920
#if 0
//#402
/**
�N�����o�b�N�A�b�v�ۑ�
*/
void	AApp::SPI_CopyToLaunchBackup()
{
	//�o�b�N�A�b�v�t�H���_�擾
	AFileAcc	apppreffolder;
	AFileWrapper::GetAppPrefFolder(apppreffolder);
	AFileAcc	backupfolder;
	backupfolder.SpecifyChild(apppreffolder,"_lmb");
	backupfolder.CreateFolder();
	//���[�h���ƂɎ��s
	for( AIndex i = 0; i < mModeFileArray.GetItemCount(); i++ )
	{
		AFileAcc	modeFile;
		modeFile.CopyFrom(mModeFileArray.GetObjectConst(i));
		AText	modename;
		modeFile.GetFilename(modename);
		
		//mode�t�@�C�����o�b�N�A�b�v
		AFileAcc	backupfile;
		backupfile.SpecifyChildFile(backupfolder,modename);
		//#844 modeFile.CopyFileTo(backupfile,true);
		
		AText	backupfilepath;
		backupfile.GetPath(backupfilepath);
		
		//ModePreferences.mi���o�b�N�A�b�v
		AFileAcc	srcPrefFile;
		srcPrefFile.SpecifySister(modeFile,"data/ModePreferences.mi");
		AFileAcc	dstPrefFile;
		AText	backupfilepath_pref(backupfilepath);
		backupfilepath_pref.AddCstring("_modepref");
		dstPrefFile.Specify(backupfilepath_pref);
		srcPrefFile.CopyFileTo(dstPrefFile,true);
		/*#844
		//keywords.txt���o�b�N�A�b�v
		AFileAcc	srcKeywordFile;
		srcKeywordFile.SpecifySister(modeFile,"data/keywords.txt");
		AFileAcc	dstKeywordFile;
		AText	backupfilepath_keyword(backupfilepath);
		backupfilepath_keyword.AddCstring("_keywords");
		dstKeywordFile.Specify(backupfilepath_keyword);
		srcKeywordFile.CopyFileTo(dstKeywordFile,true);
		*/
	}
}
#endif

//�S�Ẵ��[�h�̃��[�g�t�H���_�擾
void	AApp::SPI_GetModeRootFolder( AFileAcc& outFileAcc ) const
{
	AFileAcc	app;
	AFileWrapper::GetAppFile(app);
	outFileAcc.SpecifySister(app,"_mode");
	if( outFileAcc.Exist() )   return;
	
	AFileAcc	preffolder;
	AFileWrapper::GetAppPrefFolder(preffolder);
	outFileAcc.SpecifyChild(preffolder,"mode");
}

//�f�t�H���gmode�t�H���_�擾
void	AApp::SPI_GetModeDefaultFolder( AFileAcc& outFileAcc ) const
{
	/*#461
	AFileAcc	app;
	AFileWrapper::GetAppFile(app);
	outFileAcc.SpecifySister(app,"default");
	*/
	AFileWrapper::GetResourceFile("default",outFileAcc);
}

#pragma mark ===========================

#pragma mark --- ModeUpdate
//#427

/**
���[�h�����X�V���s
*/
void	AApp::SPI_ModeUpdate( const AModeIndex inModeIndex, const ABool inForce )
{
	//inForce(revision�ɂ�����炸��ɍX�V)�̒l���L��
	SPI_GetModePrefDB(inModeIndex,false).SetForceAutoUpdate(inForce);
	
	//�Q��URL�擾
	AText	updateURL;
	SPI_GetModePrefDB(inModeIndex,false).GetData_Text(AModePrefDB::kModeUpdateURL,updateURL);
	if( updateURL.GetItemCount() > 0 )
	{
		//Revision�t�@�C����URL���擾
		AText	revisionFileURL;
		revisionFileURL.SetChildURL(updateURL,"update.txt");
		//�񓯊������Ԏ󂯓n���f�[�^
		ATextArray	infoTextArray;
		AText	t;
		t.SetNumber(inModeIndex);
		infoTextArray.Add(t);
		//�ǂݍ��݃X���b�h�N��
		//�ǂݍ��݊����������FModeUpdate_Result_Revision()
		ACurlThreadFactory	factory(this);
		AObjectID	threadObjectID = NVI_CreateThread(factory);
		(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
				SPNVI_Run(kCurlRequestType_HTTP,revisionFileURL,
					kCurlTransactionType_AutoUpdateRevision,infoTextArray);
	}
	else
	{
		//�Q��URL���ݒ�
		SPI_GetModePrefWindow(inModeIndex).SPI_ShowAutoUpdateError();//#858
		return;
	}
}

/**
Revision�f�[�^�ǂݍ��݊���������
*/
void	AApp::ModeUpdate_Result_Revision( const AText& inText, const ATextArray& inInfoTextArray )
{
	AIndex	modeindex = inInfoTextArray.GetTextConst(0).GetNumber();
	
	//�����X�V�t���O�i�蓮�X�V�j�ǂݍ��݁E�t���O����
	ABool	forceUpdate = SPI_GetModePrefDB(modeindex,false).GetForceAutoUpdate();
	SPI_GetModePrefDB(modeindex,false).SetForceAutoUpdate(false);
	
	//Web���revision�擾
	ANumber64bit	webRevision = -1;
	AText	webModename;
	if( ParseUpdateTxt(inText,webModename,webRevision) == false )
	{
		webRevision = -1;
	}
	if( webRevision < 0 )
	{
		//�t�@�C���Ȃ��A�܂��́Aupdate.txt�̃t�H�[�}�b�g�ԈႢ
		SPI_GetModePrefWindow(modeindex).SPI_ShowAutoUpdateError();//#858
		return;
	}
	//�ǂݍ��ݒ�revision�ԍ��ݒ�
	SPI_GetModePrefDB(modeindex,false).AutoUpdate_SetLoadingInfo(webRevision,webModename);
	
	//revision��r
	AText	revisionText;
	SPI_GetModePrefDB(modeindex,false).GetData_Text(AModePrefDB::kAutoUpdateRevisionText,revisionText);
	ANumber64bit	localRevision = revisionText.GetNumber64bit();
	if( localRevision < webRevision || forceUpdate == true )
	{
		//zip�t�@�C�����擾
		AText	zipfilename;
		zipfilename.AddText(webModename);
		zipfilename.AddNumber64bit(webRevision);
		zipfilename.AddCstring(".zip");
		
		//���[�h�X�V�T�[�o�[URL�擾
		AText	updateURL;
		SPI_GetModePrefDB(modeindex,false).GetData_Text(AModePrefDB::kModeUpdateURL,updateURL);
		if( updateURL.GetItemCount() > 0 )
		{
			//zip�t�@�C����URL���擾
			AText	zipFileURL;
			zipFileURL.SetChildURL(updateURL,zipfilename);
			
			//zip�ǂ݂��݃X���b�h�N��
			//�ǂݍ��݊����������FModeUpdate_Result_ZipFile()
			ACurlThreadFactory	factory(this);
			AObjectID	threadObjectID = NVI_CreateThread(factory);
			(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
					SPNVI_Run(kCurlRequestType_HTTP,zipFileURL,
						kCurlTransactionType_AutoUpdateZip,inInfoTextArray);
		}
	}
}

/**
���[�hZip�t�@�C���ǂݍ��݊���������
*/
void	AApp::ModeUpdate_Result_ZipFile( const AText& inZipText, const ATextArray& inInfoTextArray )
{
	AIndex	modeindex = inInfoTextArray.GetTextConst(0).GetNumber();
	//���[�J��zip�t�@�C������
	AFileAcc	tempFolder;
	SPI_GetTempFolder(tempFolder);
	AFileAcc	zipFileAcc;
	zipFileAcc.SpecifyUniqChildFile(tempFolder,"modeupdatezip");
	zipFileAcc.CreateFile();
	zipFileAcc.WriteFile(inZipText);
	//zip�𓀐�
	AFileAcc	unzipFolder;
	unzipFolder.SpecifyUniqChildFile(tempFolder,"modeupdate");
	unzipFolder.CreateFolder();
	//��
	AZipFile	zipFile(zipFileAcc);
	if( zipFile.Unzip(unzipFolder) == false )
	{
		//Unzip���s
		SPI_GetModePrefWindow(modeindex).SPI_ShowAutoUpdateError();//#858
		return;
	}
	
	//update.txt�ɋL�ڂ��ꂽ�t�H���_���̃t�H���_�擾
	AText	modename;
	SPI_GetModePrefDB(modeindex,false).AutoUpdate_GetLoadingModeName(modename);
	AFileAcc	modefolder;
	modefolder.SpecifyChild(unzipFolder,modename);
	//update.txt�ɋL�ڂ��ꂽ�t�H���_���̃t�H���_�����݂��Ă��邩�`�F�b�N
	if( modefolder.Exist() == false )
	{
		//�t�H���_�񑶍�
		SPI_GetModePrefWindow(modeindex).SPI_ShowAutoUpdateError();//#858
		return;
	}
	
	//AutoUpdate���s
	SPI_GetModePrefDB(modeindex,true).AutoUpdate(modefolder);
	
}

#pragma mark ===========================

#pragma mark --- Web���烂�[�h�ǉ�
//#427

/**
Web���烂�[�h�ǉ�
*/
void	AApp::SPI_AddNewModeFromWeb( const AText& inModeName, const AText& inURL )
{
	//Revision�t�@�C����URL�擾
	AText	revisionURL;
	revisionURL.SetChildURL(inURL,"update.txt");
	//�񓯊������Ԏ󂯓n���f�[�^
	//0:���[�h���i���[�U�[�w�胂�[�h���j
	//1:�ǂݍ��݌�URL
	ATextArray	infoTextArray;
	infoTextArray.Add(inModeName);
	infoTextArray.Add(inURL);
	//�ǂݍ��݃X���b�h�N��
	//�����������FAddNewModeFromWeb_Result_Revision()
	ACurlThreadFactory	factory(this);
	AObjectID	threadObjectID = NVI_CreateThread(factory);
	(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
			SPNVI_Run(kCurlRequestType_HTTP,revisionURL,
				kCurlTransactionType_AddModeFromWeb_Revision,infoTextArray);
}

/**
Web���烂�[�h�ǉ��ERevision�t�@�C���ǂݍ��݊���������
*/
void	AApp::AddNewModeFromWeb_Result_Revision( const AText& inText, const ATextArray& inInfoTextArray )
{
	//Web���revision�擾
	ANumber64bit	webRevision = -1;
	AText	webModename;
	if( ParseUpdateTxt(inText,webModename,webRevision) == false )
	{
		webRevision = -1;
	}
	if( webRevision < 0 )
	{
		//�t�@�C���Ȃ��A�܂��́Aupdate.txt�̃t�H�[�}�b�g�ԈႢ
		return;
	}
	
	//zip�t�@�C�����擾
	AText	zipfilename;
	zipfilename.AddText(webModename);
	zipfilename.AddNumber64bit(webRevision);
	zipfilename.AddCstring(".zip");
	//zip�t�@�C����URL���擾
	AText	zipFileURL;
	zipFileURL.SetChildURL(inInfoTextArray.GetTextConst(1),zipfilename);
	//�񓯊������Ԏ󂯓n���f�[�^�i���[�h���ǉ��j
	//0:���[�h���i���[�U�[�w�胂�[�h���j
	//1:�ǂݍ��݌�URL
	//2:���[�h���iupdate.txt�ɋL�q���ꂽ���[�h���j
	ATextArray	infoTextArray;
	infoTextArray.SetFromTextArray(inInfoTextArray);
	infoTextArray.Add(webModename);
	//zip�ǂ݂��݃X���b�h�J�n
	ACurlThreadFactory	factory(this);
	AObjectID	threadObjectID = NVI_CreateThread(factory);
	(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
			SPNVI_Run(kCurlRequestType_HTTP,zipFileURL,
				kCurlTransactionType_AddModeFromWeb_Zip,infoTextArray);
}

/**
Web���烂�[�h�ǉ��Ezip�t�@�C���ǂݍ��݊���������
@param inInfoTextArray �񓯊������Ԏ󂯓n���f�[�^
0:���[�h���i���[�U�[�w�胂�[�h���j
1:�ǂݍ��݌�URL
2:���[�h���iupdate.txt�ɋL�q���ꂽ���[�h���j
*/
void	AApp::AddNewModeFromWeb_Result_ZipFile( const AText& inZipText, const ATextArray& inInfoTextArray )
{
	//���[�J��zip�t�@�C������
	AFileAcc	tempFolder;
	SPI_GetTempFolder(tempFolder);
	AFileAcc	zipFileAcc;
	zipFileAcc.SpecifyUniqChildFile(tempFolder,"modeupdatezip");
	zipFileAcc.CreateFile();
	zipFileAcc.WriteFile(inZipText);
	//zip�𓀐�
	AFileAcc	unzipFolder;
	unzipFolder.SpecifyUniqChildFile(tempFolder,"modeupdate");
	unzipFolder.CreateFolder();
	//��
	AZipFile	zipFile(zipFileAcc);
	if( zipFile.Unzip(unzipFolder) == false )
	{
		//Unzip���s
		return;
	}
	
	//update.txt�ɋL�ڂ��ꂽ�t�H���_���̃t�H���_�擾
	AText	modename;
	modename.SetText(inInfoTextArray.GetTextConst(2));
	AFileAcc	modefolder;
	modefolder.SpecifyChild(unzipFolder,modename);
	//update.txt�ɋL�ڂ��ꂽ�t�H���_���̃t�H���_�����݂��Ă��邩�`�F�b�N
	if( modefolder.Exist() == false )
	{
		//�t�H���_�񑶍�
		return;
	}
	
	//�W�J����modefolder���烂�[�h�C���|�[�g
	AModeIndex	modeIndex = SPI_AddNewModeImportFromFolder(inInfoTextArray.GetTextConst(0),modefolder,false,true);
	if( modeIndex != kIndex_Invalid )
	{
		//#844 SPI_GetModePrefWindow(modeIndex).SPI_SelectMode(modeIndex);
		SPI_ShowModePrefWindow(modeIndex);
	}
}

/**
update.txt���
*/
ABool	AApp::ParseUpdateTxt( const AText& inText, AText& outModeName, ANumber64bit& outRevision )
{
	AIndex	pos = 0;
	AText	token;
	AText	line;
	//���[�h���i�t�H���_���j�ǂݍ���
	inText.GetLine(pos,line);
	{
		AIndex	p = 0;
		line.GetToken(p,token);
		if( token.Compare("name") == false )   return false;
		line.GetToken(p,token);
		if( token.Compare(":") == false )   return false;
		line.GetToken(p,token);
		if( token.Compare("\"") == false )   return false;
		AIndex	sp = p;
		if( inText.FindCstring(sp,"\"",p) == true )
		{
			line.GetText(sp,p-sp,outModeName);
		}
		else
		{
			return false;
		}
		if( outModeName.GetItemCount() == 0 )   return false;
	}
	//Revision�ǂݍ���
	inText.GetLine(pos,line);
	{
		AIndex	p = 0;
		line.GetToken(p,token);
		if( token.Compare("revision") == false )   return false;
		line.GetToken(p,token);
		if( token.Compare(":") == false )   return false;
		line.GetToken(p,token);
		outRevision = token.GetNumber64bit();
	}
	return true;
}

//#427
/**
���[�h�����X�V�_�E�����[�h��URL�i�v���Z�b�gURL�j�擾
*/
void	AApp::SPI_GetModeUpdateURL( const AIndex inIndex, AText& outURL ) const
{
	mModeUpdateURLArray.Get(inIndex,outURL);
}

#pragma mark ===========================

#pragma mark ---�}���`�t�@�C�������t�@�C���t�B���^�[�v���Z�b�g

//#617
/**
�t�@�C���t�B���^�[�v���Z�b�g���擾
*/
void	AApp::SPI_GetFileFilterPreset( const AIndex inIndex, AText& outFileFilterPreset ) const
{
	mFileFilterPresetArray.Get(inIndex,outFileFilterPreset);
}

#pragma mark ===========================

#pragma mark ---SDF

//R0000
void	AApp::MakeBuiltinSDFList()
{
	/*#166
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	AFileAcc	sdfFolder;
#if IMPLEMENTATION_FOR_MACOSX
	sdfFolder.SpecifyChild(appFile,"Contents/Resources/sdf");
#else
	sdfFolder.SpecifySister(appFile,"sdf");
#endif
	*/
	AFileAcc	sdfFolder;//#166
	AFileWrapper::GetResourceFile("sdf",sdfFolder);//#166
	sdfFolder.GetChildren(mBuiltinSDFList_File);
	for( AIndex i = 0; i < mBuiltinSDFList_File.GetItemCount(); i++ )
	{
		/*#402 ���O��ǂނ��߂�����Parse����̂͂��������Ȃ��̂ŁA���O�ǂݍ��ݐ�p�̏����ɕύX
		ASyntaxDefinition	syntaxDef;
		syntaxDef.Parse(mBuiltinSDFList_File.GetObjectConst(i));
		AText	name;
		syntaxDef.GetSDFName(name);
		*/
		AText	text;
		mBuiltinSDFList_File.GetObjectConst(i).ReadTo(text);
		AText	name;
		ASyntaxDefinition::ParseSDFNameOnly(text,name);
		mBuiltinSDFList_Name.Add(name);
	}
}

//R0000
void	AApp::SPI_GetBuiltinSDFFileByName( const AText& inSDFName, AFileAcc& outFile ) const
{
	AIndex	index = mBuiltinSDFList_Name.Find(inSDFName);
	if( index == kIndex_Invalid )
	{
		_ACError("cannot find the SDF name",this);
		index = 0;
	}
	outFile.CopyFrom(mBuiltinSDFList_File.GetObjectConst(index));
}

//R0000
const ATextArray&	AApp::SPI_GetBuiltinSDFNameArray() const
{
	return mBuiltinSDFList_Name;
}

#pragma mark ===========================

#pragma mark ---���K�\���T���v�����X�g

/**
���K�\���T���v�����X�g���ڂ��擾
*/
void	AApp::SPI_GetRegExpListItem( const AIndex inIndex, AText& outItem ) const
{
	mRegExpListArray.Get(inIndex,outItem);
}

#pragma mark ===========================

#pragma mark ---Transliterate������

void	AApp::InitTransliterate()
{
	/*B0408 AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	AFileAcc	transliterateFolder;
	transliterateFolder.SpecifyChild(appPrefFolder,"Transliterate");
	//Pref�t�H���_�̉���Transliterate�t�H���_���Ȃ���΁A�t�H���_���쐬���A�A�v���t�H���_��Transliterate�̒��g���R�s�[����
	if( transliterateFolder.Exist() == false )
	{
		AFileAcc	appFile;
		AFileWrapper::GetAppFile(appFile);
		AFileAcc	defaultTransliterateFolder;
		defaultTransliterateFolder.SpecifySister(appFile,"Transliterate");
		defaultTransliterateFolder.CopyFolderTo(transliterateFolder);
	}*/
	/*B0408 2.1.6�Ƃ̈Ⴂ�܂Ƃ߁i2.1.8b4���_�j
	���p�J�i���S�p�J�i�F
	2.1.6: �`���Ώ�
	2.1.8: �`�͑ΏۊO�i~�͖{���`���_�}�[�N�Ȃ̂őΏۊO�Ƃ���ق����x�^�[�j
	�S�p�J�i�����p�J�i�F
	2.1.6: �������͏����ɂ���đΏۊO�A�����E�������������ɂ���đΏۊO
	2.1.8: �������E�����E����������ɑΏہi2.1.8��Transliterate�V�X�e����2.1.6�̂悤�ɏ����ɂ���đΉ�����͍̂H����j
	*/
	//B0408
	AFileAcc	transliterateFolder;
	AFileWrapper::GetLocalizedResourceFile("Transliterate",transliterateFolder);
	if( transliterateFolder.Exist() == false )   return;
	
	//
	AObjectArray<AFileAcc>	children;
	transliterateFolder.GetChildren(children);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		AFileAcc	file = children.GetObject(i);
		AText	filename;
		file.GetFilename(filename);
		//�ΏۊO�t�@�C��
		if( filename.GetLength() == 0 )   continue;
		if( filename.Get(0) == '.' )   continue;
		if( filename.Compare("order") == true )   continue;
		if( filename.Compare("Icon\r") == true )   continue;
		//
		AText	text;
		file.ReadTo(text);//win SPI_LoadTextFromFile()���g����Windows��SJIS�ƌ�F������邽��ReadTo()���g��
		AItemCount	count = text.GetItemCount();
		AIndex index = mTransliterate_Source.AddNewObject();
		mTransliterate_Destination.AddNewObject();
		AText	title;
		for( AIndex pos = 0; pos < count; )
		{
			AText	srcline;
			text.GetLine(pos,srcline);
			if( pos >= count )//B0000   break;
			{
				//B0000 svn�T�[�o�[�ɒu�����߂ɁA�t�@�C�������A�X�L�[�����ɂ���B
				//���̂����A�t�@�C���̍ŏI�s�Ƀ^�C�g�����L�q
				title.SetText(srcline);
				break;
			}
			AText	dstline;
			text.GetLine(pos,dstline);
			mTransliterate_Source.GetObject(index).Add(srcline);
			mTransliterate_Destination.GetObject(index).Add(dstline);
		}
		//B0000 svn�T�[�o�[�ɒu�����߂ɁA�t�@�C�������A�X�L�[�����ɂ���BmTransliterate_Title.Add(filename);
		mTransliterate_Title.Add(title);
	}
	//B0408
	AFileAcc	orderfile;
	orderfile.SpecifyChild(transliterateFolder,"order");
	AText	ordertext;
	orderfile.ReadTo(ordertext);
	AIndex	n = 0;
	for( AIndex pos = 0; pos < ordertext.GetItemCount();  )
	{
		AText	line;
		ordertext.GetLine(pos,line);
		AIndex	index = mTransliterate_Title.Find(line);
		if( index != kIndex_Invalid && index != n )
		{
			mTransliterate_Source.MoveObject(index,n);
			mTransliterate_Destination.MoveObject(index,n);
			mTransliterate_Title.MoveObject(index,n);
		}
		n++;
	}
	//
	NVI_GetMenuManager().SetDynamicMenuItemByMenuItemID(kMenuItemID_Transliterate,mTransliterate_Title,mTransliterate_Title);
}

#pragma mark ===========================

#pragma mark ---FuzzySearch�f�[�^

//#166
/**
�B������������
*/
void	AApp::InitFuzzySearch()
{
	AFileAcc	fuzzySearchFolder;
	AFileWrapper::GetResourceFile("FuzzySearch",fuzzySearchFolder);
	if( fuzzySearchFolder.Exist() == false )   {_ACError("",this);return;}
	
	//���Ȃ�炬
	AFileAcc	kanaYuragiFile;
	kanaYuragiFile.SpecifyChild(fuzzySearchFolder,"KanaYuragi.txt");
	if( kanaYuragiFile.Exist() == false )   {_ACError("",this);return;}
	AText	text;
	kanaYuragiFile.ReadTo(text);
	for( AIndex pos = 0; pos < text.GetItemCount(); )
	{
		//���K����
		AText	srcline;
		text.GetLine(pos,srcline);
		AUCS4Char	uc = 0;
		srcline.Convert1CharToUCS4(0,uc);
		mFuzzySearchNormalizeArray_from.Add(uc);
		//���K����
		AIndex	kanaYuragiIndex = mFuzzySearchNormalizeArray_to.AddNewObject();
		AText	dstline;
		text.GetLine(pos,dstline);
		for( AIndex p = 0; p < dstline.GetItemCount(); )
		{
			p += dstline.Convert1CharToUCS4(p,uc);
			mFuzzySearchNormalizeArray_to.GetObject(kanaYuragiIndex).Add(uc);
		}
	}
}

//#724
#if 0
#pragma mark ===========================

#pragma mark ---Toolbar�A�C�R��

void	AApp::LoadToolbarIcon()
{
#if IMPLEMENTATION_FOR_MACOSX
	//�A�v���P�[�V�������\�[�X�̃A�C�R��
	for( short i = 999; i < 1100; i++ )
	{
		AIconID	iconID = CWindowImp::RegisterIconFromResouce(i);
		if( iconID == kIndex_Invalid )   break;
		mToolbarIconArray.Add(iconID);
		mToolbarIconArray_ResID.Add(i);
		mToolbarIconArray_Path.Add(GetEmptyText());
		mToolbarIconArray_CustomIcon.Add(false);
	}
	
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	AFileAcc	icnFolder;
	icnFolder.SpecifyChild(appPrefFolder,"icn");
	icnFolder.CreateFolder();//#356
	AObjectArray<AFileAcc>	children;
	icnFolder.GetChildren(children);
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		AFileAcc	file;
		file.CopyFrom(children.GetObject(index));
		AText	path;
		file.GetPath(path);
		if( file.IsFolder() == false )
		{
			//icns�t�@�C��
			/* AText	suffix;
			path.GetSuffix(suffix);
			if( suffix.Compare(".icns") == true )
			{
				AIconID	iconID = CWindowImp::RegisterDynamicIconFromFile(file);
				if( iconID == kIconID_NoIcon )   break;
				mToolbarIconArray.Add(iconID);
				mToolbarIconArray_ResID.Add(0);
				mToolbarIconArray_Path.Add(path);
				mToolbarIconArray_CustomIcon.Add(false);
			}*/
			//
			FSRef	ref;
			file.GetFSRef(ref);
			short	rRefNum = FSOpenResFile(&ref,fsRdPerm);
			if( rRefNum != -1 ) 
			{
				for( short i = 1000; i < 1100; i++ )
				{
					AIconID	iconID = CWindowImp::RegisterIconFromResouce(i);
					if( iconID == kIndex_Invalid )   break;
					mToolbarIconArray.Add(iconID);
					mToolbarIconArray_ResID.Add(i);
					mToolbarIconArray_Path.Add(path);
					mToolbarIconArray_CustomIcon.Add(false);
				}
			}
			//R0217 test
			/*AIconID	iconID = CWindowImp::RegisterDynamicIconFromImageFile(file);
			if( iconID != kIconID_NoIcon )
			{
				mToolbarIconArray.Add(iconID);
				mToolbarIconArray_ResID.Add(-16455);
				mToolbarIconArray_Path.Add(path);
				mToolbarIconArray_CustomIcon.Add(true);
			}*/
		}
		else
		{
			//B0307 �t�H���_�̃J�X�^���A�C�R��
			LoadToolbarIconRecursive(file);
		}
	}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	mToolbarIconArray.Add(kIconID_Tool_Doc);
	mToolbarIconArray.Add(kIconID_Tool_Info);
#endif
}

//B0307
//�t�H���_�̃J�X�^���A�C�R������o�^
void	AApp::LoadToolbarIconRecursive( const AFileAcc& inFolder )
{
#if IMPLEMENTATION_FOR_MACOSX
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		AFileAcc	file;
		file.CopyFrom(children.GetObject(index));
		AText	path;
		file.GetPath(path);
		if( file.IsFolder() == false )
		{
			AText	filename;
			file.GetFilename(filename);
			if( filename.Compare("Icon\r") == true )
			{
				AIconID	iconID = CWindowImp::RegisterDynamicIconFromFile(inFolder);
				if( iconID == kIconID_NoIcon )   break;
				mToolbarIconArray.Add(iconID);
				mToolbarIconArray_ResID.Add(-16455);
				mToolbarIconArray_Path.Add(path);
				mToolbarIconArray_CustomIcon.Add(true);
			}
		}
		else
		{
			if( file.IsLink() == false )//#0 �V���{���b�N�����N�E�G�C���A�X�ɂ�閳�����[�v�h�~
			{
				LoadToolbarIconRecursive(file);
			}
		}
	}
#endif
}

void	AApp::SPI_CopyToolbarIconResource( const AIconID inIconID, const AFileAcc& inDestFile )
{
#if IMPLEMENTATION_FOR_MACOSX
	AIndex	index = mToolbarIconArray.Find(inIconID);
	if( index == kIndex_Invalid )   return;
	AIndex	resID = mToolbarIconArray_ResID.Get(index);
	AText	path;
	mToolbarIconArray_Path.Get(index,path);
	//B0307
	FSSpec	dstfsspec;
	inDestFile.GetFSSpec(dstfsspec);
	//B0307�܂��֘A���\�[�X��S�č폜�i�A�C�R���ύX���͂���K�v�j
	{
		short	rRefNum = ::FSpOpenResFile(&dstfsspec,fsWrPerm);
		if( rRefNum != -1 ) 
		{
			Handle	res = ::Get1Resource('cicn',128);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			res = ::Get1Resource('icns',-16455);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			res = ::Get1Resource('icl4',-16455);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			res = ::Get1Resource('icl8',-16455);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			res = ::Get1Resource('ICN#',-16455);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			res = ::Get1Resource('ics4',-16455);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			res = ::Get1Resource('ics8',-16455);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			res = ::Get1Resource('ics#',-16455);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			::CloseResFile(rRefNum);
		}
	}
	//
	if( mToolbarIconArray_CustomIcon.Get(index) == false )//B0307
	{
		//cicn#128�ɒǉ�
		Handle	h = NULL;
		if( path.GetItemCount() == 0 )
		{
			h = ::GetResource('cicn',resID);
			::DetachResource(h);
		}
		else
		{
			AFileAcc	file;
			file.Specify(path);
			FSRef	ref;
			file.GetFSRef(ref);
			short	rRefNum = ::FSOpenResFile(&ref,fsWrPerm);
			if( rRefNum != -1 )
			{
				h = ::GetResource('cicn',resID);
				::DetachResource(h);
				::CloseResFile(rRefNum);
			}
		}
		
		//
		FSRef	ref;
		inDestFile.GetFSRef(ref);
		{
			HFSUniStr255	forkName;
			::FSGetResourceForkName(&forkName);
			::FSCreateResourceFork(&ref,forkName.length,forkName.unicode,0);
		}
		short	rRefNum = ::FSOpenResFile(&ref,fsWrPerm);
		if( rRefNum != -1 )
		{
			Handle	res = ::Get1Resource('cicn',128);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			
			AddResource(h,'cicn',128,"\p");
			WriteResource(h);
			ReleaseResource(h);
			CloseResFile(rRefNum);
		}
		else
		{
			::DisposeHandle(h);
		}
		//B0307 �J�X�^���A�C�R���t���O����
		FInfo fi;
		::FSpGetFInfo(&dstfsspec,&fi);
		fi.fdFlags &= ~kHasCustomIcon;
		::FSpSetFInfo(&dstfsspec,&fi);
		IconRef	iconRef;
		SInt16	label;
		if( ::GetIconRefFromFile(&dstfsspec,&iconRef,&label) == noErr )
		{
			::UpdateIconRef(iconRef);
			::ReleaseIconRef(iconRef);
		}
	}
	else//B0307
	{
		//�t�H���_�[�J�X�^���A�C�R������̃R�s�[
		AFileAcc	file;
		file.Specify(path);
		AText	buffer;
		file.ReadTo(buffer,true);//RF
		inDestFile.WriteResourceFork(buffer);
		//�J�X�^���A�C�R���t���O�ݒ�
		FInfo fi;
		::FSpGetFInfo(&dstfsspec,&fi);
		fi.fdFlags |= kHasCustomIcon;
		::FSpSetFInfo(&dstfsspec,&fi);
		IconRef	iconRef;
		SInt16	label;
		if( ::GetIconRefFromFile(&dstfsspec,&iconRef,&label) == noErr )
		{
			::UpdateIconRef(iconRef);
			::ReleaseIconRef(iconRef);
		}
	}
#endif
}
#endif

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�����C�x���g����
void	AApp::EVTDO_DoInternalEvent( ABool& outUpdateMenu )
{
	//#698
	outUpdateMenu = false;
	
	//���[�_���\�����͓����C�x���g�������Ȃ��B�i���[�_�����͑��̃C�x���g�����荞�ނ��Ƃ��l������Ă��Ȃ��̂ŁB�j(#946)
	if( AWindow::NVI_IsInModal() == true )
	{
		return;
	}
	
	//�ҏW�v���O���X���[�_���Z�b�V��������������A�����C�x���g�������Ȃ��i���R�H�j
	if( SPI_InEditProgressModalSession() == true )
	{
		return;
	}
	
	//fprintf(stderr,"EVT_DoInternalEvent() ");
	//�����ɗ������_�ŃL���[�ɂ��܂��Ă���C�x���g����������������B
	//����ɂ��A�i�����C�x���g�����݂��Ă�����胋�[�v������ꍇ�ƈ���āj
	//�}�E�X�C�x���g��\���X�V�C�x���g��OS�C�x���g�L���[�ɂ��߂��܂�EVT_DoInternalEvent()���ł��邮���葱���邱�Ƃ͂Ȃ��A
	//queueCount�����I��������AOS�֏�����Ԃ����Ƃ��ł���B
	AItemCount	queueCount = ABase::GetMainInternalEventQueueCount();
	
	AInternalEventType	type;
	AObjectID	senderObjectID = kObjectID_Invalid;
	ANumber	num;
	AText	text;
	AObjectIDArray	objectIDArray;
	for( AIndex i = 0; i < queueCount; i++ )
	{
		if( ABase::DequeueFromMainInternalEventQueue(type,senderObjectID,num,text,objectIDArray) == true )
		{
			try//#982
			{
				if( NVI_DoInternalEvent_FrameCommon(type,senderObjectID,num,text,objectIDArray) == false )//win
				switch(type)
				{
				  case kInternalEvent_FTPProgress_Hide:
					{
						/*#199 mFTPProgress*/SPI_GetFTPProgress().NVI_Hide();
						break;
					}
				  case kInternalEvent_FTPProgress_SetProgress:
					{
						/*#199 mFTPProgress*/SPI_GetFTPProgress().NVI_Create(kObjectID_Invalid);
						/*#199 mFTPProgress*/SPI_GetFTPProgress().SPI_SetProgress(num);
						break;
					}
				  case kInternalEvent_FTPProgress_SetProgressText:
					{
						/*#199 mFTPProgress*/SPI_GetFTPProgress().NVI_Create(kObjectID_Invalid);
						/*#199 mFTPProgress*/SPI_GetFTPProgress().SPI_SetProgressText(text);
						break;
					}
				  case kInternalEvent_FTPProgress_Error:
					{
						/*#199 mFTPProgress*/SPI_GetFTPProgress().NVI_Create(kObjectID_Invalid);
						/*#199 mFTPProgress*/SPI_GetFTPProgress().SPI_SetError(text);
						break;
					}
				  case kInternalEvent_FTPLog:
					{
						/*#199 mFTPLogWindow*/SPI_GetFTPLogWindow().NVI_Create(kObjectID_Invalid);
						/*#199 mFTPLogWindow*/SPI_GetFTPLogWindow().SPI_AddLog(text);
						break;
					}
				  case kInternalEvent_FTPOpen:
					{
						/*#361
					AIndex	index = mThreadDataReceiver_Text.GetIndexByID(objectIDArray.Get(0));
					if( index != kIndex_Invalid )
					{
						AText	doctext;
						doctext.SetText(mThreadDataReceiver_Text.GetObjectConst(index));
						mThreadDataReceiver_Text.Delete1Object(index);
						SPNVI_CreateTextDocumentFTP(text,doctext);
					}
					*/
						AObjectID	accessPluginConnectionObjectID = objectIDArray.Get(0);
						SPI_DoAccessPluginResponseReceived(accessPluginConnectionObjectID,text,GetEmptyText());
						break;
					}
					//#361
				  case kInternalEvent_FTPDirectoryResult:
					{
						AObjectID	accessPluginConnectionObjectID = objectIDArray.Get(0);
						SPI_DoAccessPluginResponseReceived(accessPluginConnectionObjectID,text,GetEmptyText());
						break;
					}
					//�}���`�t�@�C�������v���O���X
				  case kInternalEvent_MultiFileFindProgress_SetProgress:
					{
						if( SPI_GetMultiFileFindWindow().SPI_ExecutingFindForMultiFileReplace() == true )
						{
							//�}���`�t�@�C���u���̃v���O���X
							SPI_GetMultiFileFindWindow().SPI_MultiFileReplace_Progress(static_cast<short>(num));
						}
						else
						{
							//�ʏ�}���`�t�@�C�������̃v���O���X
							SPI_GetMultiFileFindProgress().NVI_Create(kObjectID_Invalid);
							SPI_GetMultiFileFindProgress().SPI_SetProgress(static_cast<short>(num));
						}
						break;
					}
				  case kInternalEvent_MultiFileFindProgress_SetProgressText:
					{
						if( SPI_GetMultiFileFindWindow().SPI_ExecutingFindForMultiFileReplace() == true )
						{
							//�}���`�t�@�C���u���̃v���O���X
							SPI_GetMultiFileFindWindow().SPI_MultiFileReplace_SetProgressText(text);
						}
						else
						{
							//�ʏ�}���`�t�@�C�������̃v���O���X
							SPI_GetMultiFileFindProgress().NVI_Create(kObjectID_Invalid);
							SPI_GetMultiFileFindProgress().SPI_SetProgressText(text);
						}
						break;
					}
					//�}���`�t�@�C�����������ʒm
				  case kInternalEvent_MultiFileFind_Completed:
					{
						//
						GetApp().SPI_GetIndexWindowDocumentByID(objectIDArray.Get(0)).SPI_InhibitClose(false);
						//�}���`�t�@�C�������E�C���h�E�̌����{�^����Enable�ɂ��邽��
						SPI_UpdateMultiFileFindWindow();
						//ImportFileRecognizer�ĊJ
						GetApp().SPI_GetImportFileRecognizer().NVI_SetSleepFlag(false);
						GetApp().SPI_GetImportFileRecognizer().NVI_WakeTrigger();
						//
						if( SPI_GetMultiFileFindWindow().SPI_ExecutingFindForMultiFileReplace() == true )
						{
							//�}���`�t�@�C���u���̏ꍇ
							//�}���`�t�@�C�������E�C���h�E�̊�������
							SPI_GetMultiFileFindWindow().SPI_MultiFileReplace_FindCompleted();
						}
						else
						{
							//�}���`�t�@�C�������̏ꍇ
							//�v���O���X�E�C���h�E����
							SPI_GetMultiFileFindProgress().NVI_Hide();
						}
						//------------------��v�ӏ���V�K�h�L�������g�ɒ��o------------------
						if( text.GetItemCount() > 0 )
						{
							ADocumentID docID = SPNVI_CreateNewTextDocument(kStandardModeIndex);
							if( docID != kObjectID_Invalid )
							{
								SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
							}
						}
						//------------------�}���`�t�@�C�������E�C���h�E�\���X�V------------------
						SPI_GetMultiFileFindWindow().NVI_UpdateProperty();
						//
						//#698
						outUpdateMenu = true;
						break;
					}
					//�}���`�t�@�C����������
				  case kInternalEvent_MultiFileFindResult:
					{
						AObjectID	receiverID = objectIDArray.Get(0);
						AMultiFileFindResult&	result = SPI_GetThreadDataReceiver_MultiFileFindResult(receiverID);
						//�}���`�t�@�C���u���̏ꍇ�A�}���`�t�@�C�������E�C���h�E�ɒʒm����
						//�i����ɂ��u���Ώۃt�@�C�����X�g�̍X�V���s���B�u���E�C���h�E���̌��ʃ��X�g�͂��̉��̏����ōX�V����B�j
						if( SPI_GetMultiFileFindWindow().SPI_ExecutingFindForMultiFileReplace() == true )
						{
							SPI_GetMultiFileFindWindow().SPI_MultiFileReplace_FindResult(result.GetFilePathArray(),text);
						}
						//
						AObjectID	indexWindowDocumentID = result.GetIndexWindowDocumentID();
						SPI_GetIndexWindowDocumentByID(indexWindowDocumentID).
								SPI_AddItems_ToFirstGroup_TextPosition(result.GetFilePathArray(),
																	   result.GetHitTextArray(),result.GetPreHitTextArray(),result.GetPostHitTextArray(),
																	   result.GetPositionTextArray(),
																	   result.GetParagraphTextArray(),//#465
																	   result.GetStartPositionArray(),result.GetLengthArray(),result.GetParagraphArray(),GetEmptyText());
						SPI_Delete1ThreadDataReceiver_MultiFileFindResult(receiverID);
						break;
					}
				  case kInternalEvent_TraceLog:
					{
						//#1275 �������̂��߃E�C���h�E�\�����삷��܂ł͐������Ȃ� /*#199 mTraceLogWindow*/SPI_GetTraceLogWindow().NVI_Create(kObjectID_Invalid);
						/*#199 mTraceLogWindow*/SPI_GetTraceLogWindow().SPI_AddLog(text);
						break;
					}
				  case kInternalEvent_ShowTraceLog:
					{
						if( NVI_GetEnableTraceLog() == true )
						{
							/*#199 mTraceLogWindow*/SPI_GetTraceLogWindow().NVI_CreateAndShow(false);
						}
						break;
					}
					/*#193
				//FTP�t�H���_�ǂݍ��݊���
			  case kInternalEvent_FTPFolder:
				{
					AObjectID	dynamicMenuObjectID = objectIDArray.Get(0);
					for( AIndex index = 0; index < GetAppPref().GetItemCount_Array(AAppPrefDB::kFTPFolder_Server); index++ )
					{
						AText	menuActionText;
						NVI_GetMenuManager().GetDynamicMenuItemActionTextByMenuItemID(kMenuItemID_FTPFolder,index,menuActionText);
						if( text.Compare(menuActionText) == true )
						{
							NVI_GetMenuManager().SetDynamicMenuSubMenu(kMenuItemID_FTPFolder,index,dynamicMenuObjectID);
							break;
						}
					}
					break;
				}
				*/
					//�C���|�[�g�t�@�C����͊���
				  case kInternalEvent_ImportFileRecognizeCompleted:
					{
						//------------------�C���|�[�g�t�@�C����͊�������------------------
						//�C���|�[�g�t�@�C��manager�ł̃C���|�[�g�t�@�C����͊�������
						//�idocument��mode pref�ł̊����������s���j
						mImportFileManager.DoImportFileRecognized(true);
						//------------------�C���|�[�g�t�@�C����͒��v���O���X�\������------------------
						//�e�L�X�g�E�C���h�E�̃v���O���X�\��������
						for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
						{
							SPI_GetTextWindowByID(winID).SPI_HideImportFileProgress();
						}
						//�R�[���O���t�̃v���O���X�\��������
						SPI_ShowHideImportFileProgressInCallGrafWindow(false);
						//���[�h���X�g�̃v���O���X�\��������
						SPI_ShowHideImportFileProgressInWordsListWindow(false);
						break;
					}
					/*
				//�C���|�[�g�t�@�C����͈ꕔ����
			  case kInternalEvent_ImportFileRecognizePartialCompleted:
				{
					mImportFileManager.DoImportFileRecognized(false);
					break;
				}
				*/
					//#725
					//�C���|�[�g�t�@�C���v���O���X�\��
				  case kInternalEvent_ImportFileRecognizeProgress:
					{
						//�őO�ʂ̃e�L�X�g�E�C���h�E�̃C���|�[�g�t�@�C���v���O���X�\���X�V
						AWindowID	frontWinID = NVI_GetMostFrontWindowID(kWindowType_Text);
						if( frontWinID != kObjectID_Invalid )
						{
							for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
							{
								if( winID == frontWinID )
								{
									SPI_GetTextWindowByID(winID).SPI_ShowImportFileProgress(num);
								}
								else
								{
									SPI_GetTextWindowByID(winID).SPI_HideImportFileProgress();
								}
							}
						}
						//�R�[���O���t�E�C���h�E�̃C���|�[�g�t�@�C���v���O���X�\��
						SPI_ShowHideImportFileProgressInCallGrafWindow(true);
						//���[�h���X�g�E�C���h�E�̃C���|�[�g�t�@�C���v���O���X�\��
						SPI_ShowHideImportFileProgressInWordsListWindow(true);
						break;
					}
					//
				  case kInternalEvent_ChildProcessCompleted:
					{
						mChildProcessManager.DeletePipe(objectIDArray.Get(0),text);
						break;
					}
					//#402 �v���W�F�N�g�t�H���_���[�h����
				  case kInternalEvent_ProjectFolderLoaderResult:
					{
						//�v���f�[�^�擾
						AObjectID	threadObjectID = objectIDArray.Get(0);
						ATextArray	requestArray_ProjectFolder;
						(dynamic_cast<AProjectFolderLoader&>(NVI_GetThreadByID(threadObjectID))).
								SPI_GetRequestedData(requestArray_ProjectFolder);
						
						//mSameProject_ProjectFolder�����pHashTextArray����
						//�imSameProject_ProjectFolder��folderPathArrayForSearch�ɃR�s�[�j
						AHashTextArray	folderPathArrayForSearch;
						for( AIndex i = 0; i < mSameProject_ProjectFolder.GetItemCount(); i++ )
						{
							AText	path;
							mSameProject_ProjectFolder.GetObjectConst(i).GetPath(path);
							folderPathArrayForSearch.Add(path);
						}
						
						//���ʎ擾
						//��x�ɕ����̃v���W�F�N�g�ɂ��Ă̏������s���ꍇ������̂ŁA�e�v���W�F�N�g���̃��[�v
						for( AIndex requestArrayIndex = 0; requestArrayIndex < requestArray_ProjectFolder.GetItemCount(); requestArrayIndex++ )
						{
							//�v���W�F�N�gindex�擾
							AIndex	sameProjectArrayIndex = 
									folderPathArrayForSearch.Find(requestArray_ProjectFolder.GetTextConst(requestArrayIndex));
							if( sameProjectArrayIndex != kIndex_Invalid )
							{
								//#723
								AStMutexLocker	locker(mSameProjectArrayMutex);
								//#520
								mSameProjectArray_Pathname.GetObject(sameProjectArrayIndex).DeleteAll();
								mSameProjectArray_ModeIndex.GetObject(sameProjectArrayIndex).DeleteAll();//#533
								mSameProjectArray_IsFolder.GetObject(sameProjectArrayIndex).DeleteAll();
								//���ʎ擾
								ATextArray	resultTextArray;
								(dynamic_cast<AProjectFolderLoader&>(NVI_GetThreadByID(threadObjectID))).
										SPI_GetResult(requestArrayIndex,resultTextArray);
								//
								for( AIndex i = 0; i < resultTextArray.GetItemCount(); i++ )
								{
									//�t�@�C���擾
									AText	path;
									resultTextArray.Get(i,path);
									AFileAcc	file;
									file.Specify(path);
									//�t�@�C���p�X�f�[�^�ǉ�
									mSameProjectArray_Pathname.GetObject(sameProjectArrayIndex).Add(path);
									//�i�t�H���_�ȊO�̏ꍇ�j���[�hindex���擾�A�f�[�^�ǉ��i�t�H���_�̏ꍇ�̓��[�hindex��kIndex_Invalid�Ƃ��Ēǉ��j
									AModeIndex	modeIndex = kIndex_Invalid;
									ABool	isFolder = (path.GetLastChar()==AFileAcc::GetPathDelimiter());//file.IsFolder();
									if( isFolder == false )
									{
										modeIndex = SPI_FindModeIDByTextFilePath(path);
									}
									mSameProjectArray_ModeIndex.GetObject(sameProjectArrayIndex).Add(modeIndex);
									mSameProjectArray_IsFolder.GetObject(sameProjectArrayIndex).Add(isFolder);
								}
							}
						}
						//���j���[�X�V
						UpdateSameProjectMenu();
						
						//�ǂݍ��񂾃t�H���_���X�g�擾
						AObjectArray<AFileAcc>	folders;
						(dynamic_cast<AProjectFolderLoader&>(NVI_GetThreadByID(threadObjectID))).
								SPI_GetLoadedFolders(folders);
						//SCM�o�^
						if( GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
						{
							mSCMFolderManager.RegisterFolders(folders,0);
						}
						
						//�t�@�C�����X�g�\���X�V�iSCM��ԓǂݍ��ݏI������Ă΂�邪�ASCM�͕K���������s���Ȃ��j
						//#725 SPI_GetFileListWindow().SPI_UpdateTable();
						SPI_UpdateFileListWindows(kFileListUpdateType_ProjectFolderLoaded);//#725
						
						//�X���b�h�I�u�W�F�N�g�폜
						NVI_DeleteThread(threadObjectID);
						break;
					}
					//#427
				  case kInternalEvent_Curl:
					{
						//�v���f�[�^�擾
						AObjectID	threadObjectID = objectIDArray.Get(0);
						
						//�ǂݍ��݃f�[�^�擾
						AText	resultText;
						AText	url;
						ECurlRequestType	requestType = kCurlRequestType_None;
						ATextArray	infoTextArray;
						ACurlTransactionType	transactionType = kCurlTransactionType_None;
						(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
								GetResult(resultText,requestType,url,transactionType,infoTextArray);
						
						//Transaction���Ƃ̏����؂�ւ�
						switch(transactionType)
						{
						  case kCurlTransactionType_AutoUpdateRevision:
							{
								ModeUpdate_Result_Revision(resultText,infoTextArray);
								break;
							}
						  case kCurlTransactionType_AutoUpdateZip:
							{
								ModeUpdate_Result_ZipFile(resultText,infoTextArray);
								break;
							}
						  case kCurlTransactionType_AddModeFromWeb_Revision:
							{
								AddNewModeFromWeb_Result_Revision(resultText,infoTextArray);
								break;
							}
						  case kCurlTransactionType_AddModeFromWeb_Zip:
							{
								AddNewModeFromWeb_Result_ZipFile(resultText,infoTextArray);
								break;
							}
							//�L�[���[�h���pCSV�t�@�C��Http�o�R�ǂݍ���
						  case kCurlTransactionType_LoadCSVForKeyword:
							{
								LoadCSVForKeyword_Result(resultText,infoTextArray);
								break;
							}
						}
						
						//�X���b�h�I�u�W�F�N�g�폜
						NVI_DeleteThread(threadObjectID);
						break;
					}
					//#682 ���I�[�v��
				  case kInternalEvent_ReopenFile:
					{
						if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kReopenFile) == true )
						{
							ReopenFile();
						}
						//#975 ���I�[�v���ݒ肪OFF�̏ꍇ�A������mReopenProcessCompleted��true�ɐݒ肵�āA�V�K�t�@�C�������������s�B
						else
						{
							mReopenProcessCompleted = true;
							//�����V�K�t�@�C������
							//�i�N������EVT_OpenUntitledFile()�o�R��SPI_CreateNewWindowAutomatic()���R�[������邪�A
							//�@mReopenProcessCompleted�t���O��false�̊Ԃ͎��s�����Aprepend�t���O��true�ɂ���݂̂Ƃ��Ă���B
							//�@prepend���ꂽ�V�K�t�@�C���������������������Ŏ��s����B #1056�j
							DoPrependedOpenUntitledFileEvent();
						}
						//#698
						outUpdateMenu = true;
						break;
					}
					//#678 Document Reveal�i���I�[�v�����Ɏg�p���Ă���j
				  case kInternalEvent_RevealDocument:
					{
						ADocumentID	docID = objectIDArray.Get(0);
						if( NVI_IsDocumentValid(docID) == true )
						{
							SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();
						}
						break;
					}
					//#698 ���@�F���X���b�h�����ꎞ����
				  case kInternalEvent_SyntaxRecognizerPaused:
					{
						ADocumentID	docID = objectIDArray.Get(0);
						if( NVI_IsDocumentValid(docID) == true )
						{
							SPI_GetTextDocumentByID(docID).SPI_DoSyntaxRecognizerPaused();
						}
						break;
					}
					//#699 �s�v�Z�X���b�h�����ꎞ����
				  case kInternalEvent_WrapCalculatorPaused:
					{
						ADocumentID	docID = objectIDArray.Get(0);
						if( NVI_IsDocumentValid(docID) == true )
						{
							if( SPI_GetTextDocumentByID(docID).SPI_DoWrapCalculatorPaused() == true )
							{
								//�܂�Ԃ��v�Z�������̓��j���[�X�V�i�������݉ɕς��̂Łj
								outUpdateMenu = true;
							}
						}
						break;
					}
					//#733 
					//�⊮��⌟������
				  case kInternalEvent_CandidateFinderPaused:
					{
						AViewID	viewID = objectIDArray.Get(0);
						if( NVI_ExistView(viewID) == true )
						{
							AView_Text::GetTextViewByViewID(viewID).SPI_DoCandidateFinderPaused();
						}
						break;
					}
					//#723
					//���[�h���X�g��������
				  case kInternalEvent_WordsListFinderPausedForCallGraf:
					{
						AWindowID	winID = objectIDArray.Get(0);
						if( NVI_ExistWindow(winID) == true )
						{
							SPI_GetCallGrafWindow(winID).SPI_GetCallGrafView().SPI_DoWordsListFinderPaused();
						}
						break;
					}
				  case kInternalEvent_WordsListFinderPausedForWordsList:
					{
						AWindowID	winID = objectIDArray.Get(0);
						if( NVI_ExistWindow(winID) == true )
						{
							SPI_GetWordsListWindow(winID).SPI_GetWordsListView().SPI_DoWordsListFinderPaused();
						}
						break;
					}
					/*#193
				//#361
				//�A�N�Z�X�v���O�C���f�[�^��M
			  case kInternalEvent_RemoteConnectionResponseReceived:
				{
					AObjectID	accessPluginConnectionObjectID = objectIDArray.Get(0);
					DoAccessPluginResponseReceived(accessPluginConnectionObjectID,text);
					break;
				}
				*/
					//#853
					//�L�[���[�h��񌟍�����
				  case kInternalEvent_IdInfoFinderPaused:
					{
						ADocumentID	docID = objectIDArray.Get(0);
						AWindowID	idInfoWinID = objectIDArray.Get(1);
						AWindowID	textWinID = objectIDArray.Get(2);
						DoIdInfoFinderPaused(docID,idInfoWinID,textWinID);
						break;
					}
					//startup ����
				  case kInternalEvent_StartUp:
					{
						//���[�h��default����V�K���������Ƃ��̓_�C�A���O��\������
						if( mNewlyCreatedModeNameArray.GetItemCount() > 0 )
						{
							//�W�����[�h��V�K���������Ƃ��i���V�K�C���X�g�[�����j�̓_�C�A���O�\�����Ȃ��i���ꂪ�ŏ��ɕ\������Ă������ł͈Ӗ����킩��Ȃ��j #1374
							AText	normalModeName("Normal");
							if( mNewlyCreatedModeNameArray.Find(normalModeName) == kIndex_Invalid )
							{
								//
								AText	message1, message2;
								message1.SetLocalizedText("ModeAddedMessage");
								for( AIndex i = 0; i < mNewlyCreatedModeNameArray.GetItemCount(); i++ )
								{
									AText	modeName;
									mNewlyCreatedModeNameArray.Get(i,modeName);
									//���[�h�̕\�������擾
									AIndex	modeIndex = SPI_FindModeIndexByModeRawName(modeName);
									AText	modeDisplayName;
									SPI_GetModePrefDB(modeIndex,false).GetModeDisplayName(modeDisplayName);
									//���[�h�\���������b�Z�[�W�ɒǉ�
									message2.AddText(modeDisplayName);
									if( i < mNewlyCreatedModeNameArray.GetItemCount()-1 )
									{
										message2.AddCstring(", ");
									}
								}
								NVI_ShowModalAlertWindow(message1,message2);
							}
						}
						break;
					}
					//#1374
				  case kInternalEvent_ShowReadMe:
					{
						SPI_ShowReadMeFirst();
						break;
					}
					//#926
					//�ۑ����_�C�A���O�@����UTF8�ϊ������ꍇ�̃_�C�A���O
				  case kInternalEvent_SaveResultDialog_ForcedToUTF8:
					{
						if( NVI_ExistWindow(objectIDArray.Get(0)) == true )
						{
							AText	text1, text2;
							text1.SetLocalizedText("FileSave_ConvertedToUTF8_1");
							text2.SetLocalizedText("FileSave_ConvertedToUTF8_2");
							SPI_GetTextWindowByID(objectIDArray.Get(0)).SPI_ShowChildErrorWindow(senderObjectID,text1,text2);
						}
						break;
					}
					//�ۑ����_�C�A���O�@ charset�G���[�_�C�A���O
				  case kInternalEvent_SaveResultDialog_CharsetError:
					{
						if( NVI_ExistWindow(objectIDArray.Get(0)) == true )
						{
							AText	text1, text2;
							text1.SetLocalizedText("CharsetCheck_Error1");
							text2.SetLocalizedText("CharsetCheck_Error2");
							SPI_GetTextWindowByID(objectIDArray.Get(0)).SPI_ShowChildErrorWindow(senderObjectID,text1,text2);
						}
						break;
					}
					//CSV�L�[���[�h���[�_�[����������
				  case kInternalEvent_CSVKeywordLoaderCompleted:
					{
						//�X���b�h�I�u�W�F�N�g�폜
						NVI_DeleteThread(senderObjectID);
						break;
					}
					//�����t�H���_���[�_�[����������
				  case kInternalEvent_SameFolderLoaderCompleted:
					{
						AFileAcc	folder;
						folder.Specify(text);
						//���݂̓����t�H���_���j���[�̃t�H���_�Ȃ�u�����t�H���_�v���j���[�X�V
						if( mCurrentSameFolder == FindSameFolderIndex(folder) )
						{
							//�u�����t�H���_�v���j���[�X�V
							UpdateSameFolderMenu();
							//�t�@�C�����X�g�\���X�V�iSCM��ԓǂݍ��ݏI������Ă΂�邪�ASCM�͕K���������s���Ȃ����߁B�j
							SPI_UpdateFileListWindows(kFileListUpdateType_SameFolderDataUpdated);//#725
						}
						//SCM��ԓǂݍ���
						SCMRegisterFolder(folder);//SCM��ԓǂݍ��ݏI����AInternalEvent�o�R�Ńt�@�C�����X�g��SPI_UpdateTable()�������s�����
						//�X���b�h�I�u�W�F�N�g�폜
						NVI_DeleteThread(senderObjectID);
						break;
					}
					//�h�L�������g�t�@�C���̃X�N���[�j���O����������
				  case kInternalEvent_DocumentFileScreenCompleted:
					{
						//�h�L�������g�擾
						ADocumentID	docID = objectIDArray.Get(0);
						//��ԑJ�ڎ��s
						if( NVI_IsDocumentValid(docID) == true )
						{
							if( SPI_GetTextDocumentByID(docID).SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
							{
								//FileScreened��Ԃ̂܂܂ł���΃��[�h�^�܂�Ԃ��v�Z�^��ԑ@�ۂ��s���B
								//�i�����ʒm�̑O�ɁA�r���[active�ɂȂ����Ƃ��́A�����瑤����SPI_TransitInitState_ViewActivated()���R�[�������B�j
								SPI_GetTextDocumentByID(docID).SPI_TransitInitState_ViewActivated();
							}
						}
						break;
					}
					//#962
					//�h�L�������g�t�@�C��screen�G���[�������i�G���[�_�C�A���O�\�����A�h�L�������g�����j
				  case kInternalEvent_DocumentFileScreenError:
					{
						//�h�L�������g�擾
						ADocumentID	docID = objectIDArray.Get(0);
						//��ԑJ�ڎ��s
						if( NVI_IsDocumentValid(docID) == true )
						{
							SPI_GetTextDocumentByID(docID).SPI_ShowScreenErrorAndClose();
						}
						break;
					}
					//#974
					//�A�v���P�[�V����activated���̏����i�̂����A���̃C�x���g���s��Ɏ��s���ׂ������j���s
				  case kInternalEvent_AppActivated:
					{
						//�V�K�E�C���h�E����
						//#1056 EVT_OpenUntitledFile()�o�R�Ŏ��s����悤�ɕύX SPI_CreateNewWindowAutomatic();
						break;
					}
					//#1006
					//�_�u���N���b�N�ɂ��t�@�C���I�[�v�����ATextDocument��initWithContentsOfURL�o�R�ł��̃C�x���g����������
				  case kInternalEvent_AppleEventOpenDoc:
					{
						//�f�[�^�擾�iNULL������؂�j
						ATextArray	textArray;
						textArray.ExplodeFromText(text,0);
						AText	filepath, tecname, odbServer, odbSenderToken, odbCustomPath;
						textArray.Get(0,filepath);
						textArray.Get(1,tecname);
						ABool	odbMode = false;
						if( textArray.GetItemCount() >= 3 )
						{
							textArray.Get(2,odbServer);
							textArray.Get(3,odbSenderToken);
							textArray.Get(4,odbCustomPath);
							odbMode = true;
						}
						//�t�@�C���擾
						AFileAcc	file;
						file.Specify(filepath);
						//CDocImp�I�u�W�F�N�g�擾 #1078
						AObjectID	docImpID = objectIDArray.Get(0);
						//�h�L�������g����
						SPNVI_CreateDocumentFromLocalFile(file,tecname,docImpID);//#1078
						ADocumentID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
						if( odbMode == true )
						{
							SPI_GetTextDocumentByID(docID).SPI_SetODBMode(odbServer.GetOSTypeFromText(),odbSenderToken,odbCustomPath);
						}
						break;
					}
					//#1422
					//File Presenter�t�@�C���ύX�ʒm��M��
				  case kInternalEvent_FileChangedNotification:
					{
						AFileAcc	file;
						file.Specify(text);
						ADocumentID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
						if( docID != kObjectID_Invalid )
						{
							SPI_GetTextDocumentByID(docID).SPI_CheckEditByOtherApp();
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
			catch(...)
			{
			}
		}
	}
	
	//���̃��W���[���ɂ��Ă̋��R�����g
	//�����C�x���g���L���[����ɂȂ�܂Ŏ��s����B
	//���̊ԁA�����C�x���g�L���[��mutex�̓��b�N����B
	//����ɂ��A�C�x���g�L���[��Post����X���b�h�́A���̎��_�Ńu���b�N�����B
	//����́A���C���X���b�h�̏����i��ɕ\�����f�����̂͂��j��D�悵�ď������������߁B
	//�����C�x���g�P���Ƃ�mutex���b�N��������ƁA���̎��_�ŃX���b�h�������o���\���������A�����C�x���g�����܂������X���b�h����������܂ŕ\������Ȃ��B
	//����߂��@Mutex�����̂悤�ȖړI�Ɏg���ׂ��ł͂Ȃ��B�i�������ԃ��b�N������ׂ��ł͂Ȃ��Bmutex�̃_�u�����b�N�ŁA�A�v�������b�N���邱�Ƃ�����B�j
	//�{���W���[���̃R�����g�ɂ���ʂ�A�u�L���[�ɂ��܂��Ă���C�x���g����������������v���@�ɕύX�B
	
}

//�R�}���h����
ABool	AApp::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//�J��
	  case kMenuItemID_Open:
		{
			NVI_ShowFileOpenWindow(false);
			result = true;
			break;
		}
	  case kMenuItemID_OpenInvisible:
		{
			NVI_ShowFileOpenWindow(true);
			result = true;
			break;
		}
		//�u�V�K�v���j���[
	  case kMenuItemID_NewTextDocument:
		{
			AModeIndex modeIndex = SPI_FindModeIndexByModeRawName(inDynamicMenuActionText);
			if( modeIndex == kIndex_Invalid )   {_ACError("",this); break;}
			SPNVI_CreateNewTextDocument(modeIndex);
			result = true;
			break;
		}
		//�I��
	  case kMenuItemID_Quit:
		{
			NVI_TryQuit();//#1034 #1102
			result = true;
			break;
		}
		//���̃A�v���P�[�V�������B��
	  case kMenuItemID_HideThisApplication:
		{
			GetImp().HideThisApplication();
			result = true;
			break;
		}
		//�ق��̃A�v���P�[�V�������B��
	  case kMenuItemID_HideOtherApplications:
		{
			GetImp().HideOtherApplications();
			result = true;
			break;
		}
		//���ׂẴA�v���P�[�V������\��
	  case kMenuItemID_ShowAllApplications:
		{
			GetImp().ShowAllApplications();
			result = true;
			break;
		}
		//�S�Ă�O�ʂ�
	  case kMenuItemID_BringAllToFront:
		{
			GetImp().BringAllToFront();
			result = true;
			break;
		}
		//���ݒ�E�C���h�E
	  case kMenuItemID_AppPref:
		{
			SPI_ShowAppPrefWindow();
			result = true;
			break;
		}
	  case kMenuItemID_About:
		{
			/*#199 mAboutWindow*/SPI_GetAboutWindow().NVI_CreateAndShow();
			result = true;
			break;
		}
		//���[�U�[ID�o�^ #1384
	  case kMenuItemID_RegisterUserID:
		{
			SPI_GetUserIDRegistrationWindow().NVI_CreateAndShow();
			result = true;
			break;
		}
		//
	  case kMenuItemID_ModePref:
		{
			AModeIndex modeIndex = SPI_FindModeIndexByModeRawName(inDynamicMenuActionText);//#868
			if( modeIndex == kIndex_Invalid )   {_ACError("",this); break;}//#868
			SPI_ShowModePrefWindow(modeIndex);//B0303 #868
			result = true;
			break;
		}
		//#1102
		//Sparkle�A�b�v�f�[�g���m�F
	  case kMenuItemID_CheckForUpdates:
		{
#ifndef Target_Mac_OS_X_10_6
#ifdef __LP64__
			//�A�b�v�f�[�g�m�F
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
			{
				[[SUUpdater sharedUpdater] checkForUpdates:nil];
			}
#endif
#endif
			break;
		}
		//�E�C���h�E���j���[
		/*#922
	  case kMenuItemID_WindowMenu:
		{
			AIndex	index = inDynamicMenuActionText.GetNumber();
			SPI_SelectFromWindowMenuByIndex(index);
			break;
		}
		*/
	  case kMenuItemID_SwitchWindow:
		{
			SPI_SwitchWindow(true);//#552
			break;
		}
		//#552
	  case kMenuItemID_SwitchWindowPrev:
		{
			SPI_SwitchWindow(false);
			break;
		}
		//�e���v���[�g���j���[
	  case kMenuItemID_NewWithTemplate:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			SPNVI_CreateDocumentFromTemplateFile(file);
			result = true;
			break;
		}
		//�ŋߊJ�����t�@�C��
	  case kMenuItemID_Open_RecentlyOpenedFile:
		{
			//#235
			AIndex	p = 0;
			if( inDynamicMenuActionText.FindCstring(0,"://",p) == true )
			{
				//#361 GetApp().SPI_StartOpenFTPDocument(inDynamicMenuActionText,false);
				SPI_RequestOpenFileToRemoteConnection(inDynamicMenuActionText,false);//#361
			}
			else
			{
				//B0389 OK SPNVI_CreateDocumentByPathForDisplay(inDynamicMenuActionText);
				AFileAcc	file;
				file.Specify(inDynamicMenuActionText);//#910 ,kFilePathType_1);//B0389 OK �ŋߊJ�����t�@�C���̃f�[�^�͋��`���ɂ���
				SPNVI_CreateDocumentFromLocalFile(file);
			}
			result = true;
			break;
		}
	  case kMenuItemID_Delete_RecentlyOpenedFile:
		{
			SPI_DeleteAllRecentlyOpenedFile();//win 080710
			result = true;
			break;
		}
		//#480
	  case kMenuItemID_Delete_NotFoundRecentlyOpenedFile:
		{
			SPI_DeleteRecentlyOpenedFileNotFound();
			result = true;
			break;
		}
		//�����t�H���_�[
	  case kMenuItemID_SameFolder:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			//B0421 SPNVI_CreateDocumentFromLocalFile(file);
			GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
			result = true;
			break;
		}
		//�����v���W�F�N�g
	  case kMenuItemID_SameProject:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			//B0421 SPNVI_CreateDocumentFromLocalFile(file);
			GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
			result = true;
			break;
		}
		//�v���W�F�N�g�t�H���_�\���X�V #331
	  case kMenuItemID_RefreshProjectFolders:
		{
			SPI_UpdateAllSameProjectData();
			break;
		}
		//�֘A�t�@�C�����J��
	  case kMenuItemID_OpenImportFile:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			SPNVI_CreateDocumentFromLocalFile(file);
			result = true;
			break;
		}
		//
			/*#361
		//FTP�t�H���_�[���j���[
	  case kMenuItemID_FTPFolder:
		{
			DoFTPFolderMenuSelected(inDynamicMenuActionText);
			break;
		}
	  case kMenuItemID_FTPFolderSetup:
		{
			SPI_GetFTPFolderWindow().NVI_CreateAndShow();
			result = true;
			break;
		}
		//�t�H���_�[���X�g���j���[
	  case kMenuItemID_FolderListSetup:
		{
			SPI_GetFolderListWindow().NVI_CreateAndShow();
			result = true;
			break;
		}
			 */
	  case kMenuItemID_FolderList:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			//B0421 SPNVI_CreateDocumentFromLocalFile(file);
			GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
			break;
		}
		/*#725
		//#390
	  case kMenuItemID_KeyRecordWindow:
		{
			if( SPI_GetKeyRecordWindow().NVI_IsVisibleMode() == true )
			{
				SPI_GetKeyRecordWindow().NVI_Hide();
			}
			else
			{
				SPI_GetKeyRecordWindow().NVI_Show(false);
			}
			result = true;
			break;
		}
		*/
		
	  case kMenuItemID_FTPLog:
		{
			//B0000
			/*#199 mFTPLogWindow*/SPI_GetFTPLogWindow().NVI_Create(kObjectID_Invalid);
			if( /*#199 mFTPLogWindow*/SPI_GetFTPLogWindow().NVI_IsVisibleMode() == true )
			{
				/*#199 mFTPLogWindow*/SPI_GetFTPLogWindow().NVI_Hide();
			}
			else
			{
				/*#199 mFTPLogWindow*/SPI_GetFTPLogWindow().NVI_Show();
			}
			result = true;
			break;
		}
	  case kMenuItemID_TraceLog:
		{
			/*#199 mTraceLogWindow*/SPI_GetTraceLogWindow().NVI_CreateAndShow();
			result = true;
			break;
		}
		//����
	  case kMenuItemID_Find:
	  case kMenuItemID_FindDialog:
		{
			/*#199 mFindWindow*/SPI_GetFindWindow().NVI_CreateAndShow();//#701
			/*#199 mFindWindow*/SPI_GetFindWindow().NVI_SwitchFocusToFirst();
			result = true;
			break;
		}
		//�}���`�t�@�C������
	  case kMenuItemID_MutiFileFind:
		{
			SPI_GetMultiFileFindWindow().SPI_OpenCloseReplaceScreen(false);//#65
			/*#199 mMultiFileFindWindow*/SPI_GetMultiFileFindWindow().NVI_Show();
			/*#199 mMultiFileFindWindow*/SPI_GetMultiFileFindWindow().NVI_SwitchFocusToFirst();
			result = true;
			break;
		}
		//#65
		//�}���`�t�@�C���u��
	  case kMenuItemID_MultiFileReplace:
		{
			SPI_GetMultiFileFindWindow().SPI_OpenCloseReplaceScreen(true);
			SPI_GetMultiFileFindWindow().NVI_Show();
			SPI_GetMultiFileFindWindow().NVI_SwitchFocusToFirst();
			result = true;
			break;
		}
		//
	  case kMenuItemID_OpenModePrefFolder:
		{
			AFileAcc	folder;
			SPI_GetModePrefDB(kStandardModeIndex).GetModeFolder(folder);
			ALaunchWrapper::Reveal(folder);
			result = true;
			break;
		}
		/*#725
	  case kMenuItemID_ShowFileList:
		{
			SPI_ShowHideFileListWindow(!GetAppPref().GetData_Bool(AAppPrefDB::kDisplayWindowList));//#291
			result = true;
			break;
		}
		*/
		/*#858
	  case kMenuItemID_ShowFusenList://#138
		{
			if( SPI_GetFusenListWindow().NVI_IsVisibleMode() == true )
			{
				SPI_GetFusenListWindow().NVI_Hide();
				mAppPref.SetData_Bool(AAppPrefDB::kDisplayFusenList,false);
			}
			else
			{
				SPI_GetFusenListWindow().NVI_Show();
				mAppPref.SetData_Bool(AAppPrefDB::kDisplayFusenList,true);
			}
			result = true;
			break;
		}
		*/
		/*#850 
	  case kMenuItemID_TabWindowMode:
		{
			ABool	tabMode = GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow);
			mAppPref.SetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow,(tabMode==false));
			if( tabMode == true )
			{
				//�^�u�E�C���h�E���[�h��OFF�ɂ���
				AWindowID	tabWindowID = SPI_GetTabModeMainTextWindowID();
				if( tabWindowID != kObjectID_Invalid )
				{
					//#675 �^�u���[�h�ؑ֒���UpdateViewBounds�}��
					{
						AStSuppressTextWindowUpdateViewBounds	s(tabWindowID);
						//�^�u��1�ɂȂ�܂ŁA�V�K�E�C���h�E�������A�^�u���e���R�s�[���A�^�u�����
						//#668 rect���g�p�̂��ߍ폜 ARect	tabWindowRect;
						//#668 rect���g�p�̂��ߍ폜 SPI_GetTextWindowByID(tabWindowID).NVI_GetWindowBounds(tabWindowRect);
						//#668 rect���g�p�̂��ߍ폜 ARect	rect = tabWindowRect;
						AWindowID	prevWinID = tabWindowID;
						while( SPI_GetTextWindowByID(tabWindowID).NVI_GetSelectableTabCount() > 1 )//#379
						{
							AWindowDefaultFactory<AWindow_Text>	factory;//#175(this); #199
							AObjectID	winID = NVI_CreateWindow(factory);
							AIndex	mostBackTabIndex = SPI_GetTextWindowByID(tabWindowID).NVI_GetMostBackTabIndex();
							SPI_GetTextWindowByID(tabWindowID).SPI_CopyTabToWindow(mostBackTabIndex,winID);
							SPI_GetTextWindowByID(tabWindowID).SPI_CloseTab(mostBackTabIndex);//DiffTarget�������ɕ�����
							//#668 rect���g�p�̂��ߍ폜 rect.left -= 16;
							//SPI_GetTextWindowByID(winID).NVI_SetWindowBounds(rect);
							//SPI_GetTextWindowByID(winID).NVI_SendBehind(prevWinID);
							SPI_GetTextWindowByID(winID).SPI_UpdateViewBounds();//#291
							prevWinID = winID;
						}
						//#850 SPI_GetTextWindowByID(tabWindowID).SPI_SetTabModeMainWindow(false);
						SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();
					}
					//#675 UpdateViewBounds���s
					SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();
				}
			}
			else
			{
				//�^�u�E�C���h�E���[�h��ON�ɂ���
				//�E�C���h�E�̑S�Ẵ^�u�����C���E�C���h�E�փR�s�[���āA�^�u��S�ĕ���i���E�C���h�E�����j
				AWindowID	tabWindowID = NVI_GetFirstWindowID(kWindowType_Text);
				if( tabWindowID != kObjectID_Invalid )
				{
					//#850 SPI_GetTextWindowByID(tabWindowID).SPI_SetTabModeMainWindow(true);
					SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();
					//#675 �^�u���[�h�ؑ֒���UpdateViewBounds�}��
					{
						AStSuppressTextWindowUpdateViewBounds	s(tabWindowID);
						//
						for( AWindowID winID = NVI_GetNextWindowID(tabWindowID); winID != kObjectID_Invalid;  )
						{
							//#379 AItemCount	tabCount = SPI_GetTextWindowByID(winID).NVI_GetTabCount();
							AWindowID	nextWindowID = NVI_GetNextWindowID(winID);
							//#379 for( AIndex tabIndex = 0; tabIndex < tabCount; tabIndex++ )
							while( SPI_GetTextWindowByID(winID).NVI_GetSelectableTabCount() > 0 ) //#379 #0 NVI_GetTabCount()����C��
							{
								AIndex	mostBackTabIndex = SPI_GetTextWindowByID(winID).NVI_GetMostBackTabIndex();//#379 #0 tabWindowID����C��
								SPI_GetTextWindowByID(tabWindowID).SPNVI_CopyCreateTab(winID, mostBackTabIndex);//#379 tabIndex);
								//B0000 ���̏�����SPNVI_CopyCreateTab()�ֈړ� SPI_GetTextWindowByID(winID).SPI_CloseTab(tabIndex);
								if( mWindowArray.ExistObject(winID) == false )   break;//#379 �^�u�N���[�Y�̌��ʃE�C���h�E���Ȃ��Ȃ��Ă�����break
							}
							winID = nextWindowID;
						}
					}
					//
					SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();//#291
					SPI_GetTextWindowByID(tabWindowID).SPI_RefreshWindowAll();//#675 �W�����v���X�g���܂߂�refresh
				}
			}
			
			result = true;
			break;
		}
	  case kMenuItemID_AllWindowToTab:
		{
			if( GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == false )   break;
			AWindowID	tabWindowID = SPI_GetTabModeMainTextWindowID();
			if( tabWindowID == kObjectID_Invalid )
			{
				tabWindowID = NVI_GetFirstWindowID(kWindowType_Text);
				SPI_GetTextWindowByID(tabWindowID).SPI_SetTabModeMainWindow(true);
				SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();
			}
			for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid;  )
			{
				AWindowID	nextWindowID = NVI_GetNextWindowID(winID);
				if( winID != tabWindowID )
				{
					//�E�C���h�E�̑S�Ẵ^�u�����C���E�C���h�E�փR�s�[���āA�^�u��S�ĕ���i���E�C���h�E�����j
					AItemCount	tabCount = SPI_GetTextWindowByID(winID).NVI_GetTabCount();
					for( AIndex tabIndex = 0; tabIndex < tabCount; tabIndex++ )
					{
						SPI_GetTextWindowByID(tabWindowID).SPNVI_CopyCreateTab(winID,tabIndex);
						//B0000 ���̏�����SPNVI_CopyCreateTab()�ֈړ� SPI_GetTextWindowByID(winID).SPI_CloseTab(tabIndex);
					}
				}
				winID = nextWindowID;
			}
			SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();//#291
			break;
		}
		*/
		//�P�̃E�C���h�E�ɂ܂Ƃ߂�
	  case kMenuItemID_CollectAllWindowsToTabs:
		{
			//�ł��^�u���̑����e�L�X�g�E�C���h�E���擾
			AWindowID	tabWindowID = SPI_GetMostNumberTabsTextWindowID();
			if( tabWindowID == kObjectID_Invalid )   break;
			
			for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid;  )
			{
				//���̃E�C���h�E���擾
				AWindowID	nextWindowID = NVI_GetNextWindowID(winID);
				//���̃E�C���h�E���u�ł��^�u���̑����e�L�X�g�E�C���h�E�v�ł͏ꍇ�A
				//�ł��^�u���̑����e�L�X�g�E�C���h�E�ցA�S�Ẵ^�u���ړ�
				if( winID != tabWindowID )
				{
					SPI_GetTextWindowByID(winID).SPI_WindowToTab();
				}
				//���̃E�C���h�E��
				winID = nextWindowID;
			}
			break;
		}
		//�C���f�b�N�X�E�C���h�E�E��
	  case kMenuItemID_IndexNext:
		{
			//#725 SPI_GoNextIndexWindowItem();
			SPI_SelectNextItemInSubWindow();
			break;
		}
		//�C���f�b�N�X�E�C���h�E�E�O
	  case kMenuItemID_IndexPrevious:
		{
			//#725 SPI_GoPrevIndexWindowItem();
			SPI_SelectPreviousItemInSubWindow();
			break;
		}
		//
	  case kMenuItemID_TextCount:
		{
			/*#199 mTextCountWindow*/SPI_GetTextCountWindow().NVI_CreateAndShow();
			/*#199 mTextCountWindow*/SPI_GetTextCountWindow().NVI_SwitchFocusToFirst();
			/*#199 mTextCountWindow*/SPI_GetTextCountWindow().SPI_Update();
			break;
		}
		//
	  /*case kMenuItemID_IconManager:
		{
			mIconManagerWindow.NVI_CreateAndShow();
			break;
		}*/
		//
	  case kMenuItemID_SetProjectFolder:
		{
			//RC3 mSetProjectFolderWindow.NVI_CreateAndShow();
			AFileAcc	defaultfolder;
			//#551 �f�t�H���g�t�H���_�Ή�
			//ShowChooseFolderWindow()�Ƀf�t�H���g�t�H���_�����Ή�������R�����g�����i�f�t�H���g�t�H���_�̕��@��DoNavChooseFileCB()���Q�Ɓj
			AObjectID	docID = NVI_GetMostFrontDocumentID(kDocumentType_Text);
			if( docID != kObjectID_Invalid )
			{
				SPI_GetTextDocumentByID(docID).SPI_GetParentFolder(defaultfolder);
			}
			//
			SPI_ShowSetProjectFolderWindow(defaultfolder);
			break;
		}
		
		//R0186
		//�t�@�C�����X�g�̃R���e�L�X�g���j���[
		//#480
	  case kMenuItemID_CM_ClearAllItemsFromRecentOpen:
		{
			SPI_DeleteAllRecentlyOpenedFile();
			break;
		}
		//R0006
	  case kMenuItemID_SCMUpdateStatusAll:
		{
			UpdateAllSCMData();
			break;
		}
		//#146
	  case kMenuItemID_NavigatePrev:
		{
			SPI_NavigatePrev();
			break;
		}
	  case kMenuItemID_NavigateNext:
		{
			SPI_NavigateNext();
			break;
		}
		/*#725
		//#238
	  case kMenuItemID_IdInfoHistory:
		{
			//docpath+���s+idtext�̌`��actiontext�ɓ����Ă���̂Ŏ��o��
			AText	docpath, idtext;
			AIndex	pos = 0;
			for( ; pos < inDynamicMenuActionText.GetItemCount(); pos++ )
			{
				if( inDynamicMenuActionText.Get(pos) == kUChar_LineEnd )   break;
			}
			inDynamicMenuActionText.GetText(0,pos,docpath);
			inDynamicMenuActionText.GetText(pos+1,inDynamicMenuActionText.GetItemCount()-(pos+1),idtext);
			//�h�L�������g��������
			AFileAcc	file;
			file.Specify(docpath);
			ADocumentID	docID = NVI_GetDocumentIDByFile(kDocumentType_Text,file);
			if( docID != kObjectID_Invalid )
			{
				SPI_GetIdInfoView().SPI_SetIdInfo(idtext,docID);
			}
			break;
		}
		*/
		
		//RC2
	  case kMenuItemID_ShowIdInfo:
		{
			//�� #725 SPI_ShowHideIdInfoWindow(!GetAppPref().GetData_Bool(AAppPrefDB::kDisplayIdInfoWindow));//#291
			break;
		}
		//#442
	  case kMenuItemID_DisplayInfoPane:
		{
			AWindowID	winID = NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				SPI_GetTextWindowByID(winID).SPI_ShowHideRightSideBarColumn();
			}
			break;
		}
		//#442
	  case kMenuItemID_DisplaySubPane:
		{
			AWindowID	winID = NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				SPI_GetTextWindowByID(winID).SPI_ShowHideLeftSideBarColumn();
			}
			break;
		}
		//#566
	  case kMenuItemID_SetupPane:
		{
			SPI_GetAppPrefWindow().NVI_CreateAndShow();
			SPI_GetAppPrefWindow().NVI_SelectTabControl(1);
			SPI_GetAppPrefWindow().NVI_RefreshWindow();
			SPI_GetAppPrefWindow().NVI_SwitchFocusToFirst();
			break;
		}
		//#476
	  case kMenuItemID_FullScreenMode:
		{
			SPI_SwitchFullScreenMode();
			break;
		}
		//#467
	  case kMenuItemID_EnableDefines:
		{
			SPI_GetEnabledDefinesWindow().NVI_SwitchFocusToFirst();
			SPI_GetEnabledDefinesWindow().NVI_Show();
			break;
		}
		//#567 #1170
	  case kMenuItemID_OpenLuaConsole:
		{
			ADocumentID	docID = SPI_CreateOrGetLuaConsoleDocumentID();
			SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();
			break;
		}
		//#606
	  case kMenuItemID_SwitchHandToolMode:
		{
			SPI_SwitchHandToolMode();
			break;
		}
		//#638
	  case kMenuItemID_SwitchAutoIndentMode:
		{
			mAutoIndentMode = !mAutoIndentMode;
			break;
		}
		//#798
		//���o�����X�g������
	  case kMenuItemID_SearchInHeaderList:
		{
			AWindowID	winID = NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				SPI_GetTextWindowByID(winID).SPI_SearchInHeaderList();
			}
			break;
		}
		//�v���W�F�N�g�t�H���_������
	  case kMenuItemID_SearchInProjectFolder:
		{
			SPI_SearchInProjectFolder();
			break;
		}
		//�����t�H���_������
	  case kMenuItemID_SearchInSameFolder:
		{
			SPI_SearchInSameFolder();
			break;
		}
		//�ŋߊJ�����t�@�C��������
	  case kMenuItemID_SearchInRecentlyOpenFiles:
		{
			SPI_SearchInRecentlyOpenFiles();
			break;
		}
		//�L�[���[�h���X�g������
	  case kMenuItemID_SearchInKeywordList:
		{
			SPI_SearchInKeywordList();
			break;
		}
		//�S�ẴT�u�E�C���h�E�̃��b�N��Ԃ�����
	  case kMenuItemID_ClearAllLock:
		{
			SPI_ClearAllLockOfSubWindows();
			break;
		}
		//#850
		//�h�L�������g���^�u�ŊJ�����ǂ�����ݒ�
	  case kMenuItemID_OpenDocumentUsingTab:
		{
			NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow,
		  !(NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow)));
			SPI_GetAppPrefWindow().NVI_UpdateProperty();
			break;
		}
		//#725
		//�e�t���[�e�B���O�E�C���h�E�𐶐�
	  case kMenuItemID_AddRecentlyOpenFileListInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_FileList,0,250,250);
			break;
		}
	  case kMenuItemID_AddSameFolderFileListInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_FileList,1,250,250);
			break;
		}
	  case kMenuItemID_AddSameProjectFileListInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_FileList,2,250,250);
			break;
		}
	  case kMenuItemID_AddRemoteFileListInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_FileList,3,250,250);
			break;
		}
	  case kMenuItemID_AddJumpListInFloating:
		{
			AWindowID	textWinID = NVI_GetMostFrontWindowID(kWindowType_Text);
			if( textWinID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(textWinID).SPI_ShowHideJumpListWindow(true);
			}
			break;
		}
	  case kMenuItemID_AddDocInfoInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_DocInfo,0,250,100);
			break;
		}
	  case kMenuItemID_AddTextMarkerInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_TextMarker,0,300,180);//#1316 ���ύX
			break;
		}
	  case kMenuItemID_AddCandidateListInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_CandidateList,0,400,200);
			break;
		}
	  case kMenuItemID_AddToolListInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_KeyToolList,0,250,250);
			break;
		}
	  case kMenuItemID_AddIdInfoListInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_IdInfo,0,250,250);
			break;
		}
	  case kMenuItemID_AddCallGrefInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_CallGraf,0,300,300);
			break;
		}
	  case kMenuItemID_AddPreviewInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_Previewer,0,420,400);
			break;
		}
	  case kMenuItemID_AddWordsListInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_WordsList,0,300,300);
			break;
		}
	  case kMenuItemID_AddKeyRecordInFloating:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_KeyRecord,0,300,150);//#1316 ���ύX
			break;
		}
	  case kMenuItemID_AddKeyRecordInFloating_AndStart:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_KeyRecord,0,300,150);//#1316 ���ύX
			SPI_StartRecord();
			break;
		}
		//�������ʃT�u�E�C���h�E�ǉ� #1382
	  case kMenuItemID_AddFindResultInFloating:
		{
			GetApp().SPI_GetOrCreateFindResultWindowDocument();
			break;
		}
		//#844
		//�V�K���[�h�ǉ��E�C���h�E��\��
	  case kMenuItemID_AddNewMode:
		{
			SPI_GetAddNewModeWindow().NVI_CreateAndShow();
			SPI_GetAddNewModeWindow().NVI_SwitchFocusToFirst();
			break;
		}
		//#1050
		//�^�u�Z�b�g�ۑ�
	  case kMenuItemID_SaveTabSet:
		{
			AWindowID	frontTextWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);
			SPI_GetTextWindowByID(frontTextWindowID).SPI_SaveTabSet();
			break;
		}
		
		//win
		//#539#if IMPLEMENTATION_FOR_WINDOWS
	  case kMenuItemID_Help:
		{
			/*#539
			AFileAcc	app;
			AFileWrapper::GetAppFile(app);
			AFileAcc	help;
			AText	childpath;
			CAppImp::GetLangText(childpath);
			childpath.SetCstring("jp");//
			childpath.AddCstring("/help/index.htm");
			help.SpecifySister(app,childpath);
			*/
			AFileAcc	helpFolder;
			AFileWrapper::GetHelpFolder("ja",helpFolder);
			AFileAcc	help;
			help.SpecifyChild(helpFolder,"index.htm");
			AText	url;
			help.GetURL(url);
			ALaunchWrapper::OpenURLWithDefaultBrowser(url);
			break;
		}
		//�͂��߂ɂ��ǂ݂������� #1351
	  case kMenuItemID_ReadMeFirst:
		{
			SPI_ShowReadMeFirst();
			break;
		}
		//#539 #endif
		//�f�o�b�O
	  case kMenuItemID_HashStatics://#271
		{
			ABaseFunction::HashStatics();
			break;
		}
	  case kMenuItemID_DumpObjectID:
		{
			AFileAcc	folder;
			AFileWrapper::GetDesktopFolder(folder);
			AFileAcc	file;
			file.SpecifyUniqChildFile(folder,"mi_objectID_dump");
			file.CreateFile();
			AObjectArrayItem::DumpObjectID(file);
			break;
		}
		//���j�b�g�e�X�g
	  case kMenuItemID_UnitTest:
		{
			UnitTest();
			break;
		}
	  case kMenuItemID_UnitTest2:
		{
			UnitTest2();
			break;
		}
	  case kMenuItemID_LowMemoryTest:
		{
			ABaseFunction::LowMemoryTest();
			break;
		}
	  case kMenuItemID_CompareFolder:
		{
			/*#199 mCompareFolderWindow*/SPI_GetCompareFolderWindow().NVI_CreateAndShow();
			break;
		}
	  case kMenuItemID_RegExpTest:
		{
			/*#199 mTraceLogWindow*/SPI_GetTraceLogWindow().NVI_CreateAndShow();
			if( ABaseFunction::TestRegExp() == false )  _ACError("reg exp error",NULL);
			else _AInfo("regexp ok",NULL);
			break;
		}
	  case kMenuItemID_SDFDebugMode:
		{
			mSDFDebugMode = true;
			break;
		}
		/*#688
	  case 'DevT'://#436
		{
			AWindowDefaultFactory<AWindow_DevTools>	devToolsWindow;
			mDevToolsWindowID = NVI_CreateWindow(devToolsWindow);
			NVI_GetWindowByID(mDevToolsWindowID).NVI_Create(kObjectID_Invalid);
			NVI_GetWindowByID(mDevToolsWindowID).NVI_CreateAndShow();
			break;
		}
		*/
	  case 'ZZZZ':
		{
			//break;
			ASocket	listensocket;
			listensocket.BindListen(8080,64);
			AText	hostlabel("Host: ");
			AItemCount	hostlabellen = hostlabel.GetItemCount();
			while(true)
			{
				try
				{
					//�N���C�A���g�����M���鑤�̃\�P�b�g
					ASocket	srvsocket;
					fprintf(stderr,"WAIT_ACCEPT\n");
					srvsocket.Accept(listensocket);
					fprintf(stderr,"WAIT_RECV\n");
					
					//�N���C�A���g����f�[�^��M
					AText	rcvtext;
					while(true)
					{
						//��s(CRLFCRLF)����M����܂�Receive���J��Ԃ�
						AText	text;
						ABool	disconnected = false;
						srvsocket.Receive(text,disconnected);
						if( disconnected == true )   _AThrow("",this);
						rcvtext.AddText(text);
						AIndex	foundpos;
						if( rcvtext.FindCstring(0,"\r\n\r\n",foundpos) == true )
						{
							break;
						}
					}
					rcvtext.OutputToStderr();
					//�s���ɕ���
					ATextArray	rcvTextLineArray;
					rcvTextLineArray.ExplodeLineFromText(rcvtext);
					if( rcvTextLineArray.GetItemCount() > 0 )
					{
						//�R�}���h�s�擾
						AText	commandLine;
						rcvTextLineArray.Get(0,commandLine);
						//GET�̏ꍇ
						if( commandLine.CompareMin("GET ") == true )
						{
							//"Host: "���x������z�X�g�T�[�o�[�����擾
							AText	httpservername;
							for( AIndex i = 0; i < rcvTextLineArray.GetItemCount(); i++ )
							{
								AText	text;
								rcvTextLineArray.Get(i,text);
								if( text.GetItemCount() > hostlabellen )
								{
									if( text.CompareMin(hostlabel) == true )
									{
										text.GetText(hostlabellen,text.GetItemCount()-hostlabellen,httpservername);
										break;
									}
								}
							}
							//�T�[�o�[�֐ڑ�����\�P�b�g�쐬
							ASocket	clisocket;
							clisocket.Connect(httpservername,80);
							clisocket.Send(rcvtext);
							//�T�[�o�[�ڑ��\�P�b�g����ؒf�����܂Ńf�[�^��S�Ď�M
							clisocket.ReceiveAll(rcvtext);
							//�f�[�^���N���C�A���g���֑��M
							srvsocket.Send(rcvtext);
							rcvtext.OutputToStderr();
							//�T�[�o�[�ڑ��\�P�b�g��close
							clisocket.Close();
						}
					}
					//
					srvsocket.Close();
				}
				catch(...)
				{
					_ACError("AApp::EVTDO_DoMenuItemSelected() caught exception.",NULL);//#199
				}
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			result = false;
			break;
		}
	}
	return result;
}

//���j���[�X�V
void	AApp::EVTDO_UpdateMenu()
{
	//fprintf(stderr,"EVTDO_UpdateMenu() ");
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Open,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_NewTextDocument,true);
	//#922 NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SameProject,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_RefreshProjectFolders,true);//#331
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_OpenImportFile,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Open_RecentlyOpenedFile,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_OpenInvisible,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_AppPref,true);
	//NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchWindow,(mWindowMenuDocumentIDArray.GetItemCount()>=2));
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SameFolder,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_About,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_RegisterUserID,true);//#1384
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ModePref,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_MutiFileFind,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_MultiFileReplace,true);
	/*#922
#if IMPLEMENTATION_FOR_WINDOWS
	//FTP�@�\���Ή�
#else
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FTPFolder,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FTPFolderSetup,true);
#endif
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FolderList,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FolderListSetup,true);
	*/
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_NewWithTemplate,true);
	//FTP���O
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FTPLog,true);
		AText	text;
		text.SetLocalizedText("FTPLog_Show");
		if( mFTPLogWindowID != kObjectID_Invalid )
		{
			if( SPI_GetFTPLogWindow().NVI_IsWindowVisible() == true )
			{
				text.SetLocalizedText("FTPLog_Hide");
			}
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_FTPLog,text);
	}
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_TraceLog,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Delete_RecentlyOpenedFile,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Delete_NotFoundRecentlyOpenedFile,true);//#480
	/*#725
	//#390
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KeyRecordWindow,true);
		AText	text;
		text.SetLocalizedText("KeyRecordWindow_Show");
		if( mKeyRecordWindowID != kObjectID_Invalid )
		{
			if( SPI_GetKeyRecordWindow().NVI_IsWindowVisible() == true )
			{
				text.SetLocalizedText("KeyRecordWindow_Hide");
			}
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_KeyRecordWindow,text);
	}
	*/
	//#850
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CollectAllWindowsToTabs,true);
	//B0411
	ABool	anyChildWindow = false;
	for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
	{
		if( SPI_GetTextWindowByID(winID).NVI_IsChildWindowVisible() == true )   anyChildWindow = true;
	}
	//�^�u���[�h
	/*#850
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_TabWindowMode,(anyChildWindow==false));//B0411
		AText	text;
		if( GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true )
		{
			text.SetLocalizedText("TabMode_Off");
		}
		else
		{
			text.SetLocalizedText("TabMode_On");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_TabWindowMode,text);
	}
	*/
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_OpenModePrefFolder,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Find,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindDialog,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Delete_RecentlyOpenedFile,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetProjectFolder,true);
	{
		//��
		/*#725
		AText	text;
		text.SetLocalizedText("FileList_Show");
		if( mFileListWindowID != kObjectID_Invalid )
		{
			if( GetAppPref().GetData_Bool(AAppPrefDB::kDisplayWindowList) == true )//#291
			{
				text.SetLocalizedText("FileList_Hide");
			}
			NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowFileList,//#291 true);
						(SPI_GetFileListWindow().NVI_GetOverlayMode()!=true));//#291
		}
		NVI_GetMenuManager().SetMenuItemText(kMenuItemID_ShowFileList,text);
		*/
	}
	/*#858
	//#138 �tⳎ����X�g
	{
		NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowFusenList,true);
		AText	text;
		text.SetLocalizedText("FusenList_Show");
		if( mFusenListWindowID != kObjectID_Invalid )
		{
			if( SPI_GetFusenListWindow().NVI_IsVisibleMode() )
			{
				text.SetLocalizedText("FusenList_Hide");
			}
		}
		NVI_GetMenuManager().SetMenuItemText(kMenuItemID_ShowFusenList,text);
	}
	*/
	
	//
	//#725 if( (GetApp().NVI_GetMostFrontWindowID(kWindowType_Index) != kObjectID_Invalid) == true )//#92 true�̏ꍇ�̂ݐݒ肷��悤�ɕύX
	if( SPI_GetActiveSubWindowForItemSelector() != kObjectID_Invalid )
	{
		NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_IndexNext,true);
		NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_IndexPrevious,true);
	}
	//�E�C���h�E���j���[
	{
		NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchWindow,true);//#175 (mWindowMenu_WindowIDArray.GetItemCount()>1));
		NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchWindowPrev,true);//#552
		/*#922 
		NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WindowMenu,true);
		if( CWindowImp::GetMostFrontWindow() != NULL )
		{
			AWindowID	winID = NVI_GetWindowIDByWindowRef(CWindowImp::GetMostFrontWindow());
			if( winID != kObjectID_Invalid )
			{
				AIndex	index = FindWindowMenuIndex(winID,NVI_GetWindowByID(winID).NVI_GetCurrentTabIndex());
				if( index != kIndex_Invalid )
				{
					AText	actiontext;
					actiontext.SetFormattedCstring("%d",index);
					NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_WindowMenu,actiontext);
				}
			}
		}
		*/
	}
	//�S�ẴE�C���h�E���^�u��
	//ABool	b = true;
	/*#850
	if( GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == false )   b = false;
	else
	{
		AWindowID	winID = NVI_GetMostFrontWindowID(kWindowType_Text);
		if( winID == kObjectID_Invalid )
		{
			b = false;
		}
		else
		{
			//�E�C���h�E���P�������^�u���C���E�C���h�E�Ȃ�S�ẴE�C���h�E���^�u����s�ɂ���
			if( NVI_GetWindowCount(kWindowType_Text) == 1 && SPI_GetTextWindowByID(winID).SPI_IsTabModeMainWindow() == true )
			{
				b = false;
			}
		}
		if( anyChildWindow == true )   b = false;//B0411
	}
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_AllWindowToTab,b);
	*/
	//
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_TextCount,true);
	//
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_IconManager,true);
	//
	/* if( NVI_GetWindowCount(kWindowType_Text) == 0 )
	{
		NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Tool_StandardMode,false);
	}*/
	//
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_NewIndexWindow,true);
	//R0006
	if( GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
	{
		NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SCMUpdateStatusAll,true);
	}
	//win 080702
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Quit,true);
	//#688
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_HideThisApplication,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_HideOtherApplications,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowAllApplications,true);
	//#688
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_BringAllToFront,true);
	/*#725
	//RC2
	{
		AText	text;
		text.SetLocalizedText("IdInfo_Show");
		if( mIdInfoWindowID != kObjectID_Invalid )
		{
			if( GetAppPref().GetData_Bool(AAppPrefDB::kDisplayIdInfoWindow) == true )
			{
				text.SetLocalizedText("IdInfo_Hide");
			}
			NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowIdInfo,//#291 true
						(SPI_GetIdInfoWindow().NVI_GetOverlayMode()!=true));//#291
		}
		NVI_GetMenuManager().SetMenuItemText(kMenuItemID_ShowIdInfo,text);
	}
	*/
	//#539#if IMPLEMENTATION_FOR_WINDOWS
	//win
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Help,true);
	//#1351
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReadMeFirst,true);
	//#539#endif
	//#476
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FullScreenMode,true);
	//#467
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_EnableDefines,true);
	//#567 #1170
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_OpenLuaConsole,true);
	//#606
	AText	text;
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchHandToolMode,true);
		if( SPI_GetHandToolMode() == true )
		{
			text.SetLocalizedText("Menu_HandTool_Hide");
		}
		else
		{
			text.SetLocalizedText("Menu_HandTool_Show");
		}
		NVI_GetMenuManager().SetMenuItemText(kMenuItemID_SwitchHandToolMode,text);
	}
	//#844
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_AddNewMode,true);
	//�f�o�b�O
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_HashStatics,true);//#271
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_DumpObjectID,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_UnitTest,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_UnitTest2,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_LowMemoryTest,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_RegExpTest,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SDFDebugMode,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareFolder,true);
	NVI_GetMenuManager().SetEnableMenuItem('ZZZZ',true);
	NVI_GetMenuManager().SetEnableMenuItem('DevT',true);//#436
	NVI_GetMenuManager().SetEnableMenuItem('DEBG',true);
	//#146
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_NavigatePrev,SPI_GetCanNavigatePrev());
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_NavigateNext,SPI_GetCanNavigateNext());
	//#638
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchAutoIndentMode,true);
		if( mAutoIndentMode == true )
		{
			text.SetLocalizedText("Menu_AutoIndent_Off");
		}
		else
		{
			text.SetLocalizedText("Menu_AutoIndent_On");
		}
		NVI_GetMenuManager().SetMenuItemText(kMenuItemID_SwitchAutoIndentMode,text);
	}
	
	//#798
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SearchInHeaderList,(NVI_GetMostFrontWindowID(kWindowType_Text)!=kObjectID_Invalid));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SearchInProjectFolder,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SearchInSameFolder,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SearchInRecentlyOpenFiles,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SearchInKeywordList,true);
	//#850
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_OpenDocumentUsingTab,true);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_OpenDocumentUsingTab,
											   NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow));
	//#725 �e�t���[�e�B���O�T�u�E�C���h�E
	for( AMenuItemID i = kMenuItemID_AddRecentlyOpenFileListInFloating; i <= /*#1382 kMenuItemID_AddKeyRecordInFloating*/kMenuItemID_AddFindResultInFloating; i++ )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(i,true);
	}
	//�T�u�E�C���h�E���b�N����
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ClearAllLock,true);
	
	//#1050
	//�^�u�Z�b�g�ۑ�
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SaveTabSet,true);
	
	//#1102
	//Sparkle�A�b�v�f�[�g���m�F
#ifndef Target_Mac_OS_X_10_6
#ifdef __LP64__
	//10.7�ȏ�A64bit�̏ꍇ�̂ݗL��
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CheckForUpdates,true);
	}
#endif
#endif
}

/*#688
//
ABool	AApp::EVTDO_DoMouseMoved()
{
	*B0000 080810 mFileListWindow.SPI_ClearCursorRow();
	AWindowID	frontWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontWindowID != kObjectID_Invalid )
	{
		SPI_GetTextWindowByID(frontWindowID).SPI_ClearCursorRow();
	}*
	return false;
}
*/
//Tick�^�C�}�[
void	AApp::EVTDO_DoTickTimerAction()
{
	mAppElapsedTick++;
	/* #138 AApplication�ɓ����
	AWindowID	frontWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontWindowID != kObjectID_Invalid )
	{
		SPI_GetTextWindowByID(frontWindowID).EVT_DoTickTimer();
	}
	mModePrefWindow.EVT_DoTickTimer();//win 080618
	mAppPrefWindow.EVT_DoTickTimer();//win 080618
	*/
	
	//#699
	//�^�C�}�[�ɂ�鏈�����s
	for( AIndex i = 0; i < mReservedTimedExecutionArray_Type.GetItemCount(); )
	{
		//�^�C�}�[�l�擾
		ANumber	timer = mReservedTimedExecutionArray_Timer.Get(i);
		timer--;
		//�^�C�}�[�l����
		if( timer <= 0 )
		{
			//�^�C�}�[�����Ȃ珈�����s
			switch(mReservedTimedExecutionArray_Type.Get(i))
			{
			  case kTimedExecutionType_TransitNextDocumentToWrapCalculatingState:
				{
					//GetApp().SPI_TransitNextDocumentToWrapCalculatingState();
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			//�^�C�}�[�폜
			mReservedTimedExecutionArray_Type.Delete1(i);
			mReservedTimedExecutionArray_Timer.Delete1(i);
		}
		else
		{
			//�^�C�}�[�������Ȃ�f�N�������g�����^�C�}�[�l��ݒ�
			mReservedTimedExecutionArray_Timer.Set(i,timer);
			i++;
		}
	}
}

//#699
/**
�^�C�}�[�������s�\��
*/
void	AApp::SPI_ReserveTimedExecution( const ATimedExecutionType inType, const ANumber inFireSecond )
{
	AIndex	index = mReservedTimedExecutionArray_Type.Find(inType);
	if( index == kIndex_Invalid )
	{
		//�����^�C�v��������΃^�C�}�[�ǉ�
		mReservedTimedExecutionArray_Type.Add(inType);
		mReservedTimedExecutionArray_Timer.Add(inFireSecond/60);
	}
	else
	{
		//�����^�C�v������΃^�C�}�[�l�Đݒ�
		mReservedTimedExecutionArray_Timer.Set(index,inFireSecond/60);
	}
}

void	AApp::EVTDO_DoAppActivated()
{
	/*B0372 if( GetAppPref().GetData_Bool(AAppPrefDB::kCreateNewWindowAutomatic) == true )
	{
		if( NVI_GetMostFrontWindowID(kWindowType_Text) == kObjectID_Invalid )
		{
			AText	name;
			GetAppPref().GetData_Text(AAppPrefDB::kCmdNModeName,name);
			AModeIndex	modeIndex = SPI_FindModeIndexByModeName(name);
			if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
			SPNVI_CreateNewTextDocument(modeIndex);
		}
	}*/
	//
	if( mToolCommandWaitOpenFileFlag == true )
	{
		SPNVI_CreateDocumentFromLocalFile(mToolCommandWaitOpenFile);
		AObjectID	docID = NVI_GetDocumentIDByFile(kDocumentType_Text,mToolCommandWaitOpenFile);
		AWindowID	winID = SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
		SPI_GetTextWindowByID(winID).SPI_DoTool_Text(docID,mToolCommandWaitText,false);
		mToolCommandWaitText.DeleteAll();
		mToolCommandWaitOpenFileFlag = false;
	}
	//
	else if( mToolCommandWaitText.GetItemCount() > 0 )
	{
		AWindowID	winID = SPI_GetTextDocumentByID(mToolCommandWaitTextDestinationDocumentID).SPI_GetFirstWindowID();
		SPI_GetTextWindowByID(winID).SPI_DoTool_Text(mToolCommandWaitTextDestinationDocumentID,mToolCommandWaitText,false);
		mToolCommandWaitText.DeleteAll();
	}
	//B0420 �����t�H���_�A�����v���W�F�N�g�̃f�[�^���X�V�iFinder���ŕύX�����ꍇ���l���j
	SPI_UpdateAllSameFolderData();
	//#331 SPI_UpdateAllSameProjectData();
	//R0232 ���̃A�v���Ńt�@�C���ҏW���������ꍇ�Ƀ_�C�A���O��\��
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		SPI_GetTextDocumentByID(docID).SPI_CheckEditByOtherApp();
	}
#if IMPLEMENTATION_FOR_MACOSX
	//#422
	AOSVersion	ver = AEnvWrapper::GetOSVersion();
	if( ver >= kOSVersion_MacOSX_10_5 && ver < kOSVersion_MacOSX_10_6 )
	{
		AWindowID	mostFrontWinID = NVI_GetMostFrontWindowID(kWindowType_Text);
		if( mostFrontWinID != kObjectID_Invalid )
		{
			if( NVI_GetWindowByID(mostFrontWinID).NVI_IsWindowActive() == true )
			{
				NVI_GetWindowByID(mostFrontWinID).NVI_SelectWindowForSpacesProblemCountermeasure();
			}
		}
	}
#endif
	/*#974 �����Ŏ��s����ƁA�_�u���N���b�N�ŊJ�����t�@�C����open�C�x���g�̑O�ɐV�K�E�C���h�E��������Ă��܂��̂ŁAEVTDO_DoInternalEvent()��ɂ܂킷
	//#688
	//�V�K�E�C���h�E����
	SPI_CreateNewWindowAutomatic();
	*/
	AObjectIDArray	objectIDArray;
	ABase::PostToMainInternalEventQueue(kInternalEvent_AppActivated,GetObjectID(),0,GetEmptyText(),objectIDArray);
}

//B0442
void	AApp::EVTDO_DoAppDeactivated()
{
	//��������
}

//
void	AApp::SPI_Register_ToolCommandWaitText( const AText& inText, const ADocumentID inDocumentID )
{
	mToolCommandWaitText.SetText(inText);
	mToolCommandWaitTextDestinationDocumentID = inDocumentID;
}

//
void	AApp::SPI_Register_ToolCommandWaitOpen( const AFileAcc& inFile, const AText& inText )
{
	mToolCommandWaitOpenFileFlag = true;
	mToolCommandWaitOpenFile.CopyFrom(inFile);
	mToolCommandWaitText.SetText(inText);
}

//#232
/**
�C���f�b�N�X�E�C���h�E�E����
*/
/*#725
void	AApp::SPI_GoNextIndexWindowItem()
{
	AWindowID	findResultWindowID = GetApp().SPI_GetCurrentFindResultWindowID();
	if( findResultWindowID != kObjectID_Invalid )
	{
		AView_Index::GetIndexViewByViewID(SPI_GetFindResultWindow(findResultWindowID).SPI_GetIndexViewID()).SPI_SelectNextRow();//#92
	}
}
*/

//#232
/**
�C���f�b�N�X�E�C���h�E�E����
*/
/*#725
void	AApp::SPI_GoPrevIndexWindowItem()
{
	AWindowID	findResultWindowID = GetApp().SPI_GetCurrentFindResultWindowID();
	if( findResultWindowID != kObjectID_Invalid )
	{
		AView_Index::GetIndexViewByViewID(SPI_GetFindResultWindow(findResultWindowID).SPI_GetIndexViewID()).SPI_SelectPreviousRow();//#92
	}
}
*/

//#476
/**
�t���X�N���[�����[�h�ؑ�
*/
void	AApp::SPI_SwitchFullScreenMode()
{
	ABool	fullscreenmode = !(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFullScreenMode));
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kFullScreenMode,fullscreenmode);
	//
	for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
	{
		NVI_GetWindowByID(winID).NVI_SetFullScreenMode(fullscreenmode);
	}
}

//#606
/**
��̂Ђ�c�[�����[�h�ؑ�
*/
void	AApp::SPI_SwitchHandToolMode()
{
	mHandToolMode = !mHandToolMode;
}

//#724
/**
���ݒ�E�C���h�E�\��
*/
void	AApp::SPI_ShowAppPrefWindow()
{
	SPI_GetAppPrefWindow().NVI_CreateAndShow();
	SPI_GetAppPrefWindow().NVI_RefreshWindow();
	SPI_GetAppPrefWindow().NVI_SwitchFocusToFirst();
}

//#959
/**
�A�v���P�[�V�������ʂ�Window�A�N�e�B�x�[�g������
*/
void	AApp::EVTDO_WindowActivated()
{
	//�e�t���[�e�B���O�E�C���h�E�̕\���^��\�����X�V
	SPI_UpdateFloatingSubWindowsVisibility();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================


#pragma mark ---Update

void	AApp::NVIDO_UpdateAll()
{
	if( mMultiFileFindWindowID != kObjectID_Invalid )
	{
		/*#199 mMultiFileFindWindow*/SPI_GetMultiFileFindWindow().NVI_UpdateProperty();
	}
	//#844 SPI_UpdateFixedMenuShortcut();
	SPI_UpdateRecentlyOpenedFile(false);
	SPI_UpdateTextEncodingMenu();
	SPI_UpdateModeMenu();
	/*#725
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().NVI_UpdateProperty();
	}
	*/
	if( mFindWindowID != kObjectID_Invalid )//#693
	{
		/*#199 mFindWindow*/SPI_GetFindWindow().NVI_UpdateProperty();
	}
}

//R0198
void	AApp::NVIDO_OpenFile( const AFileAcc& inFile, const AFileOpenDialogRef inRef )
{
#if IMPLEMENTATION_FOR_MACOSX
	/*#1034
	ControlRef	controlRef = NULL;
	HIViewID	id;
	id.signature = 0;
	id.id = 1000;
	::HIViewFindByID(::HIViewGetRoot(inFileOpenWindowRef),id,&controlRef);
	AText	tecname;
	if( controlRef != NULL )
	{
		MenuRef	menuref = NULL;
		::GetControlData(controlRef,0,kControlPopupButtonMenuRefTag,sizeof(menuref),&menuref,NULL);
		ACarbonMenuWrapper::GetMenuItemText(menuref,::GetControl32BitValue(controlRef)-1,tecname);
		AText	automatic;
		automatic.SetLocalizedText("OpenCustom_TextEncodingAutomatic");
		if( tecname.Compare(automatic) == true )   tecname.DeleteAll();
	}
	*/
	NSOpenPanel*	panel = inRef;
	//�|�b�v�A�b�v���j���[�̑I��index����A�I�����ꂽ�e�L�X�g�G���R�[�f�B���O���擾
	NSPopUpButton*	popup = [[panel contentView] viewWithTag:1000];
	AIndex	selItemIndex = [popup indexOfSelectedItem];
	AIndex	tecindex = selItemIndex-2;
	AText	tecname;
	if( tecindex >= 0 )
	{
		GetAppPref().GetTextEncodingNameArray().Get(tecindex,tecname);
	}
	//�h�L�������g����
	SPNVI_CreateDocumentFromLocalFile(inFile,tecname);
#else
	AText	tecname;
	SPNVI_CreateDocumentFromLocalFile(inFile,tecname);
#endif
}

//NibRef
IBNibRef	AApp::sOpenCustomNibRef = NULL;

//R0198
void	AApp::NVIDO_MakeCustomControlInFileOpenWindow( const AFileOpenDialogRef inRef )
{
#if IMPLEMENTATION_FOR_MACOSX
	NSOpenPanel*	panel = inRef;
	
	//OpenCustoView�擾�i�������Ȃ烍�[�h�j
	NSView*	view = [((CocoaApp*)NSApp) getOpenCustomView];
	if( view == nil )
	{
		//���[�h
		[NSBundle loadNibNamed:@"OpenCustom" owner:NSApp];
		view = [((CocoaApp*)NSApp) getOpenCustomView];
		if( view == nil )
		{
			return;
		}
	}
	
	[view retain];
	[panel setAccessoryView:view];
	
	//�e�L�X�g�G���R�[�f�B���O�̃��X�g�擾
	AText	text;
	text.SetLocalizedText("OpenCustom_TextEncodingAutomatic");
	ATextArray	textArray;
	textArray.Add(text);
	text.SetCstring("-");
	textArray.Add(text);
	textArray.AddFromTextArray(GetAppPref().GetTextEncodingDisplayNameArray());
	//�|�b�v�A�b�v���j���[�ɐݒ�
	NSPopUpButton*	popup = [view viewWithTag:1000];
	AMenuWrapper::DeleteAllMenuItems([popup menu]);
	for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
	{
		AText	text;
		textArray.Get(i,text);
		//���j���[���ڒǉ�
		//���ڑ}��
		AMenuWrapper::InsertMenuItem([popup menu],i,text,kMenuItemID_Invalid,0,0);
		//enable/disable�ݒ�
		AMenuWrapper::SetEnableMenuItem([popup menu],i,true);
	}
	/*#1034
	if( sOpenCustomNibRef == NULL )
	{
		OSStatus err = ::CreateNibReference(CFSTR("OpenCustom"),&sOpenCustomNibRef);
		if( err != noErr )   _ACErrorNum("CreateNibReference() returned error: ",err,NULL);
	}
	WindowRef	customWinRef = NULL;
	::CreateWindowFromNib(sOpenCustomNibRef,CFSTR("OpenCustom"),&customWinRef);
	ControlID	userPaneID = {'usrp',100};
	HIViewRef	userPane;
	::HIViewFindByID(::HIViewGetRoot(customWinRef),userPaneID,&userPane);
	//10.3���O�̃o�[�W�����ɑΉ�����ɂ�EmbedControl�̏����K�v
	::NavCustomControl(inRef,kNavCtlAddControl,userPane);
	::DisposeWindow(customWinRef);
	//
	ControlRef	controlRef = NULL;
	HIViewID	id;
	id.signature = 0;
	id.id = 1000;
	::HIViewFindByID(::HIViewGetRoot(inWindowRef),id,&controlRef);
	if( controlRef != NULL )
	{
		AText	text;
		text.SetLocalizedText("OpenCustom_TextEncodingAutomatic");
		ATextArray	textArray;
		textArray.Add(text);
		text.SetCstring("-");
		textArray.Add(text);
		//�e�L�X�g�G���R�[�f�B���O�̃��X�g
		textArray.AddFromTextArray(GetAppPref().GetTextEncodingNameArray());
		//
		MenuRef	menuref = NULL;
		::GetControlData(controlRef,0,kControlPopupButtonMenuRefTag,sizeof(menuref),&menuref,NULL);
		ACarbonMenuWrapper::SetMenuItemsFromTextArray(menuref,textArray);
		::SetControl32BitMaximum(controlRef,ACarbonMenuWrapper::GetMenuItemCount(menuref));
	}
	*/
#endif
}

//R0198
/*#1034
void	AApp::NVIDO_GetCustomControlRectInFileOpenWindow( ARect& ioRect )
{
#if IMPLEMENTATION_FOR_MACOSX
	if( ioRect.right == 0 && ioRect.bottom == 0 )
	{
		if( sOpenCustomNibRef == NULL )
		{
			OSStatus err = ::CreateNibReference(CFSTR("OpenCustom"),&sOpenCustomNibRef);
			if( err != noErr )   _ACErrorNum("CreateNibReference() returned error: ",err,NULL);
		}
		WindowRef	customWinRef = NULL;
		::CreateWindowFromNib(sOpenCustomNibRef,CFSTR("OpenCustom"),&customWinRef);
		Rect	bounds;
		::GetWindowPortBounds(customWinRef,&bounds);
		ioRect.right = ioRect.left + (bounds.right - bounds.left);
		ioRect.bottom = ioRect.top + (bounds.bottom - bounds.top);
		::DisposeWindow(customWinRef);
	}
#endif
}
*/

/*#844
void	AApp::SPI_UpdateFixedMenuShortcut()
{
#if IMPLEMENTATION_FOR_MACOSX
	if( GetAppPref().GetData_Bool(AAppPrefDB::kCommandMIsMultiFileFind) == true )
	{
		NVI_GetMenuManager().SetMenuItemShortcut(kMenuItemID_MutiFileFind,'M',kModifierKeysMask_None);//#688 0);//AKeyWrapper::MakeModifierKeys(false,false,false,false));
		NVI_GetMenuManager().SetMenuItemShortcut(kMenuItemID_Minimize,'M',kModifierKeysMask_Shift);//#688 1);//AKeyWrapper::MakeModifierKeys(false,false,false,true));
	}
	else
	{
		NVI_GetMenuManager().SetMenuItemShortcut(kMenuItemID_Minimize,'M',kModifierKeysMask_None);//#688 0);//AKeyWrapper::MakeModifierKeys(false,false,false,false));
		NVI_GetMenuManager().SetMenuItemShortcut(kMenuItemID_MutiFileFind,'M',kModifierKeysMask_Shift);//#688 1);//AKeyWrapper::MakeModifierKeys(false,false,false,true));
	}
#endif
	//Mac�ȊO��Ctrl+M�Œ�
}
*/

#pragma mark ===========================

#pragma mark ---���擾

/*#305
ALanguage	AApp::SPI_GetCurrentLanguage()
{
	//�b��
	return kLanguage_Japanese;
}
*/

//win
void	AApp::SPI_GetDefaultTextFileAttribute( AFileAttribute& outFileAttribute ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	outFileAttribute.creator = 'MMKE';
	outFileAttribute.type = 'TEXT';
#endif
}

//win
void	AApp::SPI_GetModeFileAttribute( AFileAttribute& outFileAttribute ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	outFileAttribute.creator = 'MMKE';
	outFileAttribute.type = 'MODE';
#endif
}

//win
void	AApp::SPI_GetExportFileAttribute( AFileAttribute& outFileAttribute ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	outFileAttribute.creator = 'MMKE';
	outFileAttribute.type = 'TEXT';
#endif
}

//win
void	AApp::SPI_GetToolFileAttribute( AFileAttribute& outFileAttribute ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	outFileAttribute.creator = 'MMKE';
	outFileAttribute.type = 'TEXT';
#endif
}

//win
void	AApp::SPI_GetIndexFileAttribute( AFileAttribute& outFileAttribute ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	outFileAttribute.creator = 'MMKE';
	outFileAttribute.type = 'INDX';
#endif
}

//win
void	AApp::SPI_GetFTPAliasFileAttribute( AFileAttribute& outFileAttribute ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	outFileAttribute.creator = 'MMKE';
	outFileAttribute.type = 'ftpa';
#endif
}

//win 080709
//�f�t�H���g�̃t�@�C���t�B���^�擾
void	AApp::SPI_GetDefaultFileFilter( AText& outText ) const
{
	outText.SetCstring("All Files (*.*)");
	outText.Add(0);
	outText.AddCstring("*.*");
	outText.Add(0);
	outText.Add(0);
}

#pragma mark ===========================

#pragma mark ---�e��t�@�C���^�t�H���_�擾

void	AApp::SPI_GetTemplateFolder( AFileAcc& outFolder ) const
{
	AText	templateFolderName;
	templateFolderName.SetLocalizedText("FolderName_Template");
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChildFile(appPrefFolder,templateFolderName);
}

/*win 
#pragma mark ===========================

#pragma mark ---���j���[�X�V

//
void	AApp::NVIDO_UpdateMenu()
{
	//�܂��A�S�Ẵ��j���[��Disable�ɂ���
	AApplication::GetApplication().NVI_GetMenuManager().DisableAllMenuBarMenuItems();
	//MenuUpdate�C�x���g�����g�Ɍ������ē�����i���j���[�R�}���h�����Ɠ������[�g��ʂ��j
	GetApp().SPI_PostCommand(kCommandID_UpdateCommandStatus);
}
*/

#pragma mark ===========================

#pragma mark ---�E�C���h�E���j���[�Ǘ�

/*#922
void	AApp::SPI_UpdateWindowMenu()
{
	//�V���[�g�J�b�g�󂫒���
	ABool	shortcutUsed[9];
	for( AIndex i = 0; i < 9; i++ )   shortcutUsed[i] = false;
	for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
	{
		for( AIndex tabIndex = 0; tabIndex < NVI_GetWindowByID(winID).NVI_GetTabCount(); tabIndex++ )
		{
			AIndex	shortcutIndex = NVI_GetWindowByID(winID).NVI_GetWindowMenuShortcut(tabIndex);
			if( shortcutIndex >= 0 && shortcutIndex < 9 )
			{
				shortcutUsed[shortcutIndex] = true;
			}
		}
	}
	//�V���[�g�J�b�g�󂫕����Ƀe�L�X�g�E�C���h�E������
	for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
	{
		for( AIndex tabIndex = 0; tabIndex < NVI_GetWindowByID(winID).NVI_GetTabCount(); tabIndex++ )
		{
			AIndex	shortcutIndex = NVI_GetWindowByID(winID).NVI_GetWindowMenuShortcut(tabIndex);
			if( shortcutIndex == kIndex_Invalid )
			{
				for( shortcutIndex = 0; shortcutIndex < 9; shortcutIndex++ )
				{
					if( shortcutUsed[shortcutIndex] == false )
					{
						NVI_GetWindowByID(winID).NVI_SetWindowMenuShortcut(tabIndex,shortcutIndex);
						shortcutUsed[shortcutIndex] = true;
						break;
					}
				}
				if( shortcutIndex == 9 )
				{
					//�󂫂Ȃ�
					break;
				}
			}
		}
	}
	
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_WindowMenu);
	mWindowMenu_WindowIDArray.DeleteAll();
	mWindowMenu_TabIndexArray.DeleteAll();
	//�e�L�X�g�E�C���h�E���j���[�쐬
	for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
	{
		for( AIndex tabIndex = 0; tabIndex < NVI_GetWindowByID(winID).NVI_GetTabCount(); tabIndex++ )
		{
			if( NVI_GetWindowByID(winID).NVI_GetTabSelectable(tabIndex) == false )   continue;//#379
			AText	title;
			NVI_GetWindowByID(winID).NVI_GetTitle(tabIndex,title);
			AIndex	shortcutIndex = NVI_GetWindowByID(winID).NVI_GetWindowMenuShortcut(tabIndex);
			ANumber	shortcutLetter = NULL;
			AMenuShortcutModifierKeys	modifiers = 0;//win
#if IMPLEMENTATION_FOR_WINDOWS
			modifiers = kMenuShoftcutModifierKeysMask_Control;
#endif
			*#844
			if( GetAppPref().GetData_Bool(AAppPrefDB::kWindowShortcut) == true )
			{
				if( shortcutIndex >= 0 && shortcutIndex < 9 )
				{
					shortcutLetter = '1'+shortcutIndex;
				}
			}
			*
			AText	actiontext;
			actiontext.SetFormattedCstring("%d",mWindowMenu_WindowIDArray.GetItemCount());
			NVI_GetMenuManager().AddDynamicMenuItemByMenuItemID(kMenuItemID_WindowMenu,title,actiontext,
					kObjectID_Invalid,shortcutLetter,modifiers,false);
			mWindowMenu_WindowIDArray.Add(winID);
			mWindowMenu_TabIndexArray.Add(tabIndex);
		}
	}
}

//
AIndex	AApp::FindWindowMenuIndex( const AWindowID inWindowID, const AIndex inTabIndex ) const
{
	for( AIndex index = 0; index < mWindowMenu_WindowIDArray.GetItemCount(); index++ )
	{
		if( inWindowID == mWindowMenu_WindowIDArray.Get(index) && inTabIndex == mWindowMenu_TabIndexArray.Get(index) )
		{
			return index;
		}
	}
	return kIndex_Invalid;
}
*/
void	AApp::SPI_SwitchWindow( const ABool inNext )//#552
{
	//#175
	GetApp().NVI_RotateWindow(inNext);//#552
	/*#175
	if( mWindowMenu_WindowIDArray.GetItemCount() <= 1 )   return;
	//�őO�ʂ̃h�L�������g�E�C���h�E������
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; windowRef = AWindowWrapper::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )
		{
			//mWindowMenuWindowIDArray�ł́A���̃E�C���h�E��ID���擾
			AIndex	index = FindWindowMenuIndex(winID,NVI_GetWindowByID(winID).NVI_GetCurrentTabIndex());
			if( index != kIndex_Invalid )
			{
				index++;
				if( index >= mWindowMenu_WindowIDArray.GetItemCount() )
				{
					index = 0;
				}
				//�I��
				NVI_GetWindowByID(mWindowMenu_WindowIDArray.Get(index)).NVI_SelectTab(mWindowMenu_TabIndexArray.Get(index));
				NVI_GetWindowByID(mWindowMenu_WindowIDArray.Get(index)).NVI_SelectWindow();
				return;
			}
		}
	}
	*/
}

/*#922
void	AApp::SPI_GetWindowMenuTextArray( ATextArray& outTextArray ) const
{
	for( AIndex index = 0; index < mWindowMenu_WindowIDArray.GetItemCount(); index++ )
	{
		AText	title;
		NVI_GetWindowConstByID(mWindowMenu_WindowIDArray.Get(index)).NVI_GetTitle(mWindowMenu_TabIndexArray.Get(index),title);
		outTextArray.Add(title);
	}
}

void	AApp::SPI_SelectFromWindowMenuByIndex( const AIndex inIndex )
{
	NVI_GetWindowByID(mWindowMenu_WindowIDArray.Get(inIndex)).NVI_SelectTab(mWindowMenu_TabIndexArray.Get(inIndex));
	NVI_GetWindowByID(mWindowMenu_WindowIDArray.Get(inIndex)).NVI_SelectWindow();
}
*/

#pragma mark ===========================

#pragma mark ---�u�ŋߊJ�����t�@�C���v���j���[�Ǘ�

//�u�ŋߊJ�����t�@�C���v�ւ̍��ڒǉ�
void	AApp::SPI_AddToRecentlyOpenedFile( const AFileAcc& inFileAcc )
{
	AText	text;
	inFileAcc.GetPath(text);//#910 ,kFilePathType_1);//B0389 OK �ŋߊJ�����t�@�C���̃f�[�^�͋��`���ɂ���
	SPI_AddToRecentlyOpenedFile(text);
}

void	AApp::SPI_AddToRecentlyOpenedFile( const AText& inPath )//#235
{
	//#923
	//�ŋߊJ�����t�@�C���ւ̒ǉ��}�����Ȃ牽���������^�[��
	if( mSuppressAddToRecentlyOpenedFile == true )   return;
	
	//R0193
	if( GetAppPref().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )   return;
	
	//#411
	//�e���|�����t�@�C���͑ΏۊO�iODB�o�R�t�@�C���Ȃǁj
	AIndex	pos = 0;
	if( inPath.FindCstring(0,"/private/var/",pos) == true )   return;
	
	//==================���f�[�^�폜==================
	//AddRecentlyUsed_TextArray()���g���ƁAmRecentlyOpenedFileDisplayText�𓯎��ɕύX���邱�Ƃ��ł��Ȃ��̂ŁA���L�ɕύX
	AIndex	rowIndex = GetAppPref().Find_TextArray(AAppPrefDB::kRecentlyOpenedFile,inPath);
	if( rowIndex != kIndex_Invalid )
	{
		//�����e�L�X�g���݂Ȃ�ړ�����B�h�L�������g�ݒ�t�@�C���͍폜���Ȃ��̂ŁADeleteRecentlyOpenedFileItem()�̓R�[�����Ȃ��B
		GetAppPref().DeleteRow_Table(AAppPrefDB::kRecentlyOpenedFile,rowIndex);//#394
		mRecentlyOpenedFileDisplayText.Delete1(rowIndex);
		mRecentlyOpenedFileNameWithComment.Delete1(rowIndex);//B0413
		//���j���[����폜
		NVI_GetMenuManager().DeleteDynamicMenuItemByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,rowIndex);
	}
	//==================�ǉ�==================
	//�ǉ��ʒu����
	AIndex	insertIndex = 0;
	/*#394 �s��������
	{
		AItemCount	itemCount = NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kRecentlyOpenedFile_Pin);
		for( ; insertIndex < itemCount; insertIndex++ )
		{
			if( NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kRecentlyOpenedFile_Pin,insertIndex) == true )
			{
				break;
			}
		}
	}
	*/
	//doc pref�f�[�^�ǉ�
	GetAppPref().Insert1_TextArray(AAppPrefDB::kRecentlyOpenedFile,insertIndex,inPath);
	GetAppPref().Insert1_BoolArray(AAppPrefDB::kRecentlyOpenedFile_Pin,insertIndex,false);//#394
	//���X�g�ւ̃f�[�^�ǉ�
	mRecentlyOpenedFileDisplayText.Insert1(insertIndex,inPath);
	mRecentlyOpenedFileNameWithComment.Insert1(insertIndex,inPath);//B0413
	//���j���[�֒ǉ�
	NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,insertIndex,inPath,inPath,
				kObjectID_Invalid,0,kModifierKeysMask_None,false);
	//���������������������폜
	if( GetAppPref().GetItemCount_Array(AAppPrefDB::kRecentlyOpenedFile) > kRecentlyOpendFileMax )
	{
		DeleteRecentlyOpenedFileItem(mRecentlyOpenedFileNameWithComment.GetItemCount()-1);
	}
	//���j���[�^���X�g�̍X�V�i�ŏ��̃f�[�^�Ɋւ��Ă̂ݍX�V�j
	SPI_UpdateRecentlyOpenedFile(true);
}

//R0186
//�u�ŋߊJ�����t�@�C���v���獀�ڍ폜
void	AApp::SPI_DeleteRecentlyOpenedFileItem( const AIndex inIndex )
{
	DeleteRecentlyOpenedFileItem(inIndex);
	//���j���[�^���X�g�̍X�V�i�S���X�V�j
	SPI_UpdateRecentlyOpenedFile(false);
}

//#241
/**
�ŋߊJ�����t�@�C���̍��ڍ폜�i�����R�[���p�E���j���[�X�V�����j
*/
void	AApp::DeleteRecentlyOpenedFileItem( const AIndex inIndex )
{
	//�ݒ���폜
	GetAppPref().DeleteRow_Table(AAppPrefDB::kRecentlyOpenedFile,inIndex);//#394
	mRecentlyOpenedFileDisplayText.Delete1(inIndex);
	mRecentlyOpenedFileNameWithComment.Delete1(inIndex);
	//���j���[����폜
	NVI_GetMenuManager().DeleteDynamicMenuItemByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,inIndex);
}

//R0186
//�u�ŋߊJ�����t�@�C���v����A�t�@�C�����݂���Ȃ����ڂ����ׂč폜
void	AApp::SPI_DeleteRecentlyOpenedFileNotFound()
{
	for( AIndex i = 0; i < GetAppPref().GetItemCount_Array(AAppPrefDB::kRecentlyOpenedFile); )
	{
		AText	text;
		GetAppPref().GetData_TextArray(AAppPrefDB::kRecentlyOpenedFile,i,text);
		//#235 ftp://�Ŏn�܂鍀�ڂ͂Ƃ΂�
		//#0 sftp���̏ꍇ��sftp://���ƂȂ�̂ŁA://���܂ނ��ǂ����̔���ɏC���B if( text.CompareMin("ftp://") == true )
		if( text.Contains("://") == true )//#0
		{
			i++;//#0 �s��C��
			continue;
		}
		AFileAcc	file;
		file.Specify(text);//#910 ,kFilePathType_1);//B0389 OK �����̓f�[�^�݊����̂��ߋ��^�C�v�̂܂܁B
		if( file.Exist() == true )
		{
			i++;
		}
		else
		{
			//�f�[�^�폜�i�N�����ɃR�[�����ꂽ���́A�܂�mRecentlyOpenedFileDisplayText�͏o���Ă��Ȃ��̂ŁADeleteRecentlyOpenedFileItem()�R�[���͂��Ȃ��B����SPI_UpdateRecentlyOpenedFile()�ōč\�������̂ŁAmRecentlyOpenedFileDisplayText�����폜���Ȃ��Ă���薳���j
			GetAppPref().DeleteRow_Table(AAppPrefDB::kRecentlyOpenedFile,i);
		}
	}
	//���j���[�^���X�g�̍X�V�i�S���X�V�j
	SPI_UpdateRecentlyOpenedFile(false);
}

//win 080710
//�u�ŋߊJ�����t�@�C���v�̏���S�č폜
void	AApp::SPI_DeleteAllRecentlyOpenedFile()
{
	while( GetAppPref().GetItemCount_Array(AAppPrefDB::kRecentlyOpenedFile) > 0 )
	{
		DeleteRecentlyOpenedFileItem(0);
	}
	//���j���[�^���X�g�̍X�V�i�S���X�V�j
	SPI_UpdateRecentlyOpenedFile(false);
}

/**
�u�ŋߊJ�����t�@�C���v���j���[�^���X�g�\���X�V�i�ŋߊJ�����t�@�C���ǉ��E�폜�����ɃR�[�������j
@param inUpdateOnlyFirstItem: true: �ŏ��̍��ڂ̂ݍX�V  false: �S�čX�V
*/
void	AApp::SPI_UpdateRecentlyOpenedFile( const ABool inUpdateOnlyFirstItem )
{
	//==================���j���[�̍X�V�{mRecentlyOpenedFileDisplayText�̍X�V==================
	UpdateRecentlyOpenedFileDisplayText(inUpdateOnlyFirstItem);
	
	//==================�t�@�C�����X�g�̍X�V==================
	SPI_UpdateFileListWindows(kFileListUpdateType_RecentlyOpenedFileUpdated);//#725
}

//�u�ŋߊJ�����t�@�C���v�̕\���p�e�L�X�g�z����쐬����
//inUpdateOnlyFirstItem: �ŏ��̍��ڂɊւ��Ă̂�Update����ꍇtrue B0000������
void	AApp::UpdateRecentlyOpenedFileDisplayText( const ABool inUpdateOnlyFirstItem )
{
	AText	notfound;
	notfound.SetLocalizedText("RecentlyOpenedFile_FileNotFound");
	AItemCount	fileCount = GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFile).GetItemCount();
	AItemCount	commentCount = GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFileCommentPath).GetItemCount();
	//B0000������
	AItemCount	targetEnd = fileCount;
	if( inUpdateOnlyFirstItem == true )
	{
		targetEnd = 1;
	}
	//
	AHashTextArray	textArray;
	//�t�@�C�����{�R�����g��z��ɒǉ����Ă���
	for( AIndex i = 0; i < targetEnd; i++ )//B0000 fileCount->targetEnd
	{
		//�t�@�C���p�X�A�t�@�C�����擾
		AText	path;
		GetAppPref().GetData_TextArrayRef(AAppPrefDB::kRecentlyOpenedFile).Get(i,path);
		//#235 ftp�̏ꍇ�͂��̂܂ܕ\��
		//#0 sftp���̏ꍇ��sftp://���ƂȂ�̂ŁA://���܂ނ��ǂ����̔���ɏC���B if( path.CompareMin("ftp://") == true )
		if( path.Contains("://") == true )//#0
		{
			textArray.Add(path);
			continue;
		}
		//
		AFileAcc	fileacc;
		fileacc.Specify(path);//#910 ,kFilePathType_1);//B0389 �ŋߊJ�����t�@�C���̃f�[�^�͋��`���ɂ���
		AText	text;
		fileacc.GetFilename(text);
		/*#910
		//B0389 �e�t�@�C���p�X�`���ł̃t�@�C���p�X�擾
		AText	p0, p2;
		fileacc.GetPath(p0,kFilePathType_Default);
		fileacc.GetPath(p2,kFilePathType_2);
		if( p2.GetItemCount() > 0 )   p2.Delete(0,1);
		*/
		//���߃p�X�Ƃ̈�v������
		for( AIndex j = 0; j < commentCount; j++ )
		{
			const AText&	t = GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFileCommentPath).GetTextConst(j);
			//#910 if( t.CompareMin(p0) == true || t.CompareMin(path) == true || t.CompareMin(p2) == true )
			if( t.CompareMin(path) == true  )//#910 
			{
				text.AddCstring("  ");
				text.AddText(GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFileComment).GetTextConst(j));
				break;
			}
		}
		//#1301 /*#932 ���݃`�F�b�N�͍s��Ȃ��B
		if( fileacc.Exist() == false )
		{
			text.SetText(path);
			text.InsertText(0,notfound);
		}
		//#1301 */
		textArray.Add(text);
	}
	//B0000 inUpdateOnlyFirstItem��true�̂Ƃ��́AtargetEnd�ȍ~�̃f�[�^��mRecentlyOpenedFileDisplayText����\�����遨B0413 mRecentlyOpenedFileNameWithComment����\������
	if( inUpdateOnlyFirstItem == true )
	{
		//�ŏ��̍��ڂ�����Update����ꍇ:
		//mRecentlyOpenedFileNameWithComment��2�Ԗڈȍ~�̍��ڂ�textArray�֒ǉ�����
		for( AIndex i = targetEnd; i < /*B0413 mRecentlyOpenedFileDisplayText*/mRecentlyOpenedFileNameWithComment.GetItemCount(); i++ )
		{
			textArray.Add(mRecentlyOpenedFileNameWithComment.GetTextConst(i));
		}
		//mRecentlyOpenedFileNameWithComment��mRecentlyOpenedFileDisplayText�ɁAtextArray�̓��e���R�s�[���� B0413
		for( AIndex i = 0; i < targetEnd; i++ )
		{
			//��ō쐬�����\���e�L�X�g���擾
			AText	text;
			textArray.Get(i,text);
			//���X�g�X�V
			mRecentlyOpenedFileNameWithComment.Set(i,text);
			mRecentlyOpenedFileDisplayText.Set(i,text);
			//���j���[�e�L�X�g�X�V
			NVI_GetMenuManager().SetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,i,text);
			//���j���[action�e�L�X�g�X�V
			AText	actiontext;
			GetAppPref().GetData_TextArray(AAppPrefDB::kRecentlyOpenedFile,i,actiontext);
			NVI_GetMenuManager().SetDynamicMenuItemActionTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,i,actiontext);
		}
	}
	//B0413
	else
	{
		//�S�Ă̍��ڂ�Update����ꍇ:
		//mRecentlyOpenedFileNameWithComment��mRecentlyOpenedFileDisplayText�ɁAtextArray�̓��e���R�s�[����
		mRecentlyOpenedFileNameWithComment.DeleteAll();
		mRecentlyOpenedFileDisplayText.DeleteAll();
		for( AIndex i = 0; i < targetEnd; i++ )
		{
			mRecentlyOpenedFileNameWithComment.Add(textArray.GetTextConst(i));
			mRecentlyOpenedFileDisplayText.Add(textArray.GetTextConst(i));
		}
		//���j���[�X�V
		NVI_GetMenuManager().SetDynamicMenuItemByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,mRecentlyOpenedFileDisplayText,
					GetAppPref().GetData_TextArrayRef(AAppPrefDB::kRecentlyOpenedFile));
	}
	//B0413 ���̒i�K�ŁAinUpdateOnlyFirstItem�Ɋւ�炸 textArray�ɂ̓t�@�C�����{���߂������Ă��邱�ƂƂȂ�
	//mRecentlyOpenedFileNameWithComment�ɂ́A�t�@�C�����{����
	//mRecentlyOpenedFileDisplayText�ɂ́A
	//inUpdateOnlyFirstItem��true�̏ꍇ�F�ŏ��̍��ڂ́u�t�@�C�����{���߁v�A����ȊO�́u�t�@�C�����{���߁{(�p�X)�v
	//inUpdateOnlyFirstItem��false�̏ꍇ�F�u�t�@�C�����{���߁v
	
	//R0153 �����t�@�C�����i�{�R�����g�j�����鎞�́A
	for( AIndex i = 0; i < targetEnd; i++ )//B0000 fileCount->targetEnd
	{
		AText	targetfilename;
		textArray.Get(i,targetfilename);
		if( targetfilename.CompareMin(notfound) )   continue;
		//�������g�ɂЂ�������Ȃ��悤�ɂ��邽�߁AtextArray�̂ق��ɂ́A�Ō�ɋ󔒃X�y�[�X������
		AText	text_;
		text_.SetText(targetfilename);
		text_.AddCstring(" ");
		textArray.Set(i,text_);
		ABool	first = true;
		//textArray����Atargetfilename�Ɠ����t�@�C���������邩�ǂ�������
		for( AIndex loop = 0; loop < fileCount; loop++ )//�ő�fileCount�������̂�����
		{
			AIndex	samenum = textArray.Find(targetfilename);
			if( samenum == kIndex_Invalid )   break;
			if( first == true )
			{
				//��r���̂ق��Ƀp�X���t�����āAmRecentlyOpenedFileDisplayText�ɐݒ� B0406�̕ύX�ɂ��s�v�����H
				AText	path;
				GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFile).Get(i,path);
				
				AFileAcc	file;
				file.Specify(path);//#910 ,kFilePathType_1);
				AFileAcc	parent;
				parent.SpecifyParent(file);
				//B0389 parent.GetPath(path);
				GetApp().GetAppPref().GetFilePathForDisplay(parent,path);//B0389
				
				AText	newtext;
				newtext.SetText(targetfilename);
				newtext.AddCstring("  (");
				newtext.AddText(path);
				newtext.AddCstring(")");
				textArray.Set(i,newtext);
				mRecentlyOpenedFileDisplayText.Set(i,newtext);//B0403
				first = false;
				//���j���[�ݒ�
				NVI_GetMenuManager().SetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,i,newtext);
			}
			//��r��̂ق��Ƀp�X���t�����āAmRecentlyOpenedFileDisplayText�ɐݒ�
			AText	sametext;
			textArray.Get(samenum,sametext);
			AText	path;
			GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFile).Get(samenum,path);
			
			AFileAcc	file;
			file.Specify(path);//#910 ,kFilePathType_1);
			AFileAcc	parent;
			parent.SpecifyParent(file);
			//B0389 parent.GetPath(path);
			GetApp().GetAppPref().GetFilePathForDisplay(parent,path);//B0389
			
			sametext.AddCstring("  (");
			sametext.AddText(path);
			sametext.AddCstring(")");
			textArray.Set(samenum,sametext);
			mRecentlyOpenedFileDisplayText.Set(samenum,sametext);//B0403
			//���j���[�ݒ�
			NVI_GetMenuManager().SetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,samenum,sametext);
		}
	}
	//�Ō��mRecentlyOpenedFileDisplayText�փR�s�[
	/*B0413
	mRecentlyOpenedFileDisplayText.DeleteAll();
	for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
	{
		mRecentlyOpenedFileDisplayText.Add(textArray.GetTextConst(i));
	}*/
}

//
const ATextArray&	AApp::SPI_GetRecentlyOpenedFileDisplayTextArray()
{
	return mRecentlyOpenedFileDisplayText;
}

/*�s��������
void	AApp::SPI_SetPinForRecentlyOpenedFileItem( const AIndex inIndex, const ABool inEnablePin )
{
	if( inEnablePin == true )
	{
		//==================�s���ݒ�==================
		
		//�w�荀�ڂ�index=0�ɒǉ�
		AText	path;
		GetAppPref().GetData_TextArray(AAppPrefDB::kRecentlyOpenedFile,inIndex,path);
		SPI_AddToRecentlyOpenedFile(path);
		//index=0�Ƀs���ݒ�
		NVI_GetAppPrefDB().SetData_BoolArray(AAppPrefDB::kRecentlyOpenedFile,0,true);
	}
	else
	{
		//==================
		
		//�w�荀�ڂ̃s������
		
	}
}
*/

#pragma mark ===========================

#pragma mark ---�t�H���_�[���x��

/**
�t�H���_�[���x��������
*/
void	AApp::SPI_FindFolderLabel( const AText& inDocFilePath, ATextArray& outFolderPathArray, ATextArray& outLabelTextArray ) const
{
	//�t�H���_���x���ݒ�̊e���ڂ��Ƃ̃��[�v
	AItemCount	commentCount = NVI_GetAppPrefDBConst().GetItemCount_TextArray(AAppPrefDB::kRecentlyOpenedFileCommentPath);
	for( AIndex i = 0; i < commentCount; i++ )
	{
		//�p�X�擾
		AText	folderpath, label;
		NVI_GetAppPrefDBConst().GetData_TextArray(AAppPrefDB::kRecentlyOpenedFileCommentPath,i,folderpath);
		if( folderpath.CompareMin(inDocFilePath) == true )
		{
			//�h�L�������g�t�@�C�����ݒ�p�X�̔z���̏ꍇ�A���ʂɒǉ�
			NVI_GetAppPrefDBConst().GetData_TextArray(AAppPrefDB::kRecentlyOpenedFileComment,i,label);
			outFolderPathArray.Add(folderpath);
			outLabelTextArray.Add(label);
		}
	}
}

#pragma mark ===========================

#pragma mark ---�L���o�b�t�@�[
//#913

/**
�L���o�b�t�@�ɒǉ�
*/
void	AApp::SPI_AddToKillBuffer( const ADocumentID inTextDocID, const ATextPoint& inTextPoint, const AText& inText )
{
	if( inTextDocID == mKillBuffer_DocumentID && 
	   inTextPoint.x == mKillBuffer_TextPoint.x && inTextPoint.y == mKillBuffer_TextPoint.y )
	{
		//�A���폜�̏ꍇ�́A�L���o�b�t�@�ɒǉ�
		mKillBuffer_Text.AddText(inText);
	}
	else
	{
		//�A���폜�łȂ��ꍇ�́A�L���o�b�t�@�ɐV�K�ݒ�
		mKillBuffer_Text.SetText(inText);
	}
	//����̃h�L�������g��text point���L��
	mKillBuffer_DocumentID = inTextDocID;
	mKillBuffer_TextPoint = inTextPoint;
}

/**
�L���o�b�t�@�f�[�^���擾
*/
void	AApp::SPI_GetKillBuffer( AText& outText )
{
	outText.SetText(mKillBuffer_Text);
	mKillBuffer_TextPoint.x = mKillBuffer_TextPoint.y = -1;
}

#pragma mark ===========================

#pragma mark ---�u�V�K�v���j���[�Ǘ�

void	AApp::SPI_UpdateNewMenu()
{
	//���[�h���Ƃ̐V�K���j���[
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_NewTextDocument);
	AText	newShortcutModeName;
	GetAppPref().GetData_Text(AAppPrefDB::kCmdNModeName,newShortcutModeName);
	for( AIndex index = 0; index < mModePrefDBArray.GetItemCount(); index++ )
	{
		//disable�ȃ��[�h�͐V�K���j���[�ɒǉ����Ȃ�
		if( mModePrefDBArray.GetObjectConst(index).IsModeEnabled() == false )
		{
			continue;
		}
		//
		AText	modeDisplayName, modeRawName;
		mModePrefDBArray.GetObjectConst(index).GetModeDisplayName(modeDisplayName);
		mModePrefDBArray.GetObjectConst(index).GetModeRawName(modeRawName);
		ANumber	shortcut = NULL;
		AMenuShortcutModifierKeys	modifiers = 0;//win
#if IMPLEMENTATION_FOR_WINDOWS
		modifiers = kMenuShoftcutModifierKeysMask_Control;
#endif
		if( modeRawName.Compare(newShortcutModeName) == true || modeDisplayName.Compare(newShortcutModeName) == true )//#1356 �o�[�W����2�ł͕\���p���[�h�����i�[����Ă���̂ŕ\���p���[�h���ƈ�v�������ł��R�}���hN��ݒ肷��
		{
			shortcut = 'N';
		}
		//#305 AAppPrefDB::kCmdNModeName�����ݒ�̏ꍇ�́A�ŏ��̍��ڂɐݒ�
		if( newShortcutModeName.GetItemCount() == 0 && index == 0 )
		{
			shortcut = 'N';
		}
		//
		NVI_GetMenuManager().AddDynamicMenuItemByMenuItemID(kMenuItemID_NewTextDocument,modeDisplayName,modeRawName,
				kObjectID_Invalid,shortcut,modifiers,false);//win
	}
	//�e���v���[�g���j���[
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_NewWithTemplate);
	AFileAcc	templateFolder;
	SPI_GetTemplateFolder(templateFolder);
	templateFolder.GetChildren(mTemplateFile);
	for( AIndex index = 0; index < mTemplateFile.GetItemCount(); index++ )
	{
		AText	menutext, actiontext;
		mTemplateFile.GetObject(index).GetFilename(menutext);
		mTemplateFile.GetObject(index).GetPath(actiontext);
		NVI_GetMenuManager().AddDynamicMenuItemByMenuItemID(kMenuItemID_NewWithTemplate,menutext,actiontext,kObjectID_Invalid,NULL,0,false);
	}
}

#pragma mark ===========================

#pragma mark ---�u�����t�H���_����J���v���j���[�Ǘ�

//�őO�ʃh�L�������g���ς�������ɃR�[�������
//�u�����t�H���_�v���j���[�̓��e���X�V����B
void	AApp::SPI_UpdateSameFolderMenu( const AFileAcc& inFolder )
{
	AIndex	sameFolderIndex = FindSameFolderIndex(inFolder);
	
	//�t�H���_���o�^�Ȃ�V�K����
	if( sameFolderIndex == kIndex_Invalid )
	{
		//#932
		AStMutexLocker	locker(mSameFolderMutex);
		
		sameFolderIndex = mSameFolder_Folder.AddNewObject();
		mSameFolder_Folder.GetObject(sameFolderIndex).CopyFrom(inFolder);
		mSameFolder_Filename.AddNewObject();
		//#932
		ASameFolderLoaderFactory	factory(NULL);
		AObjectID	threadObjectID = GetApp().NVI_CreateThread(factory);
		(dynamic_cast<ASameFolderLoader&>(GetApp().NVI_GetThreadByID(threadObjectID))).SPNVI_Run(sameFolderIndex,inFolder);
		/*#932
		MakeFilenameListSameFolder(sameFolderIndex,inFolder);
		
		//R0006
		SCMRegisterFolder(inFolder);//SCM��ԓǂݍ��ݏI����AInternalEvent�o�R�Ńt�@�C�����X�g��SPI_UpdateTable()�������s�����
		*/
	}
	
	//���݂̃��j���[�ɓo�^���ꂽFolder�Ȃ牽���������^�[��
	if( mCurrentSameFolder == sameFolderIndex )   return;
	
	mCurrentSameFolder = sameFolderIndex;
	
	//�u�����t�H���_����J���v���j���[�X�V
	UpdateSameFolderMenu();//B0420
	
	//SPI_GetFileListWindow().SPI_UpdateTable_SameFolder(inActiveFile);
}

/**
�u�����t�H���_����J���v���j���[�f�[�^��index���擾
*/
AIndex	AApp::FindSameFolderIndex( const AFileAcc& inFolder )
{
	AIndex	sameFolderIndex = kIndex_Invalid;
	//#932
	AStMutexLocker	locker(mSameFolderMutex);
	
	//����SameFoler�z�񂩂�t�H���_����
	for( AIndex i = 0; i < mSameFolder_Folder.GetItemCount(); i++ )
	{
		if( mSameFolder_Folder.GetObject(i).Compare(inFolder) == true )
		{
			sameFolderIndex = i;
			break;
		}
	}
	return sameFolderIndex;
}

//B0420
//�u�����t�H���_����J���v���j���[�X�V
void	AApp::UpdateSameFolderMenu()
{
	//���݂̃��j���[���ڂ��폜
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_SameFolder);
	if( mCurrentSameFolder == kIndex_Invalid )   return;
	
	//#982
	AStMutexLocker	locker(mSameFolderMutex);
	
	//���j���[�ɂP���ڂ��o�^
	AItemCount	menuItemCount = mSameFolder_Filename.GetObject(mCurrentSameFolder).GetItemCount();
	if( menuItemCount > kLimit_SameFolderMenuItemCount )
	{
		menuItemCount = kLimit_SameFolderMenuItemCount;
	}
	for( AIndex menuItem = 0; menuItem < menuItemCount; menuItem++ )
	{
		AText	menutext;
		mSameFolder_Filename.GetObject(mCurrentSameFolder).Get(menuItem,menutext);
		AFileAcc	file;
		{
			//#932
			//#982 AStMutexLocker	locker(mSameFolderMutex);
			
			file.SpecifyChildFile(mSameFolder_Folder.GetObject(mCurrentSameFolder),menutext);
		}
		AText	actiontext;
		file.GetPath(actiontext);
		NVI_GetMenuManager().AddDynamicMenuItemByMenuItemID(kMenuItemID_SameFolder,menutext,actiontext,kObjectID_Invalid,NULL,0,false);
	}
	
}

//�e�t�H���_�̃t�@�C�����X�g���(mSameFolder_Filename)���쐬����i�t�H���_���̃t�@�C������������mSameFolder_Filename�֒ǉ����Ă����j
//mSameFolder_Filename�̎w��Index�̔z��͂��̊֐����̍ŏ��őS�폜�����
void	AApp::SPI_MakeFilenameListSameFolder( const AIndex inIndex, const AFileAcc& inFolder )
{
	AStMutexLocker	locker(mSameFolderMutex);//#982
	
	mSameFolder_Filename.GetObject(inIndex).DeleteAll();
	//�t�H���_���̃t�@�C�����擾
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		if( children.GetObject(index).IsFolder() == true )   continue;
		
		//win 080702 �s���t�@�C����\�����Ȃ��ݒ�Ȃ�mSameFolder_Filename�ɒǉ����Ȃ�
		if( GetAppPref().GetData_Bool(AAppPrefDB::kFileListDontDisplayInvisibleFile) == true )
		{
			if( children.GetObject(index).IsInvisible() == true )   continue;
		}
		//mSameFolder_Filename�փt�@�C������ǉ�
		AText	filename;
		children.GetObject(index).GetFilename(filename);
		filename.ConvertToCanonicalComp();//#1087
		if( filename.Compare(".DS_Store") == true )   continue;
		mSameFolder_Filename.GetObject(inIndex).Add(filename);
	}
}

//�t�@�C�����X�g�E�C���h�E�p�f�[�^�擾���\�b�h

//�t�H���_����SameFolder��Index���擾����
AIndex	AApp::SPI_GetSameFolderArrayIndex( const AFileAcc& inFolder ) const
{
	//#932
	AStMutexLocker	locker(mSameFolderMutex);
	
	for( AIndex index = 0; index < mSameFolder_Folder.GetItemCount(); index++ )
	{
		if( mSameFolder_Folder.GetObjectConst(index).Compare(inFolder) == true )
		{
			return index;
		}
	}
	return kIndex_Invalid;
}

//B0420
//mi�ւ̃A�v���ؑ֎���SameFolder�̑S�f�[�^���X�V����
void	AApp::SPI_UpdateAllSameFolderData()
{
	{
		//#932
		AStMutexLocker	locker(mSameFolderMutex);
		
		//mSameFolder_Folder�ɓo�^���ꂽ�f�[�^��S�čX�V����
		for( AIndex i = 0; i < mSameFolder_Folder.GetItemCount(); i++ )
		{
			//#932 MakeFilenameListSameFolder(i,mSameFolder_Folder.GetObject(i));
			//#932
			ASameFolderLoaderFactory	factory(NULL);
			AObjectID	threadObjectID = GetApp().NVI_CreateThread(factory);
			(dynamic_cast<ASameFolderLoader&>(GetApp().NVI_GetThreadByID(threadObjectID))).SPNVI_Run(i,mSameFolder_Folder.GetObject(i));
		}
	}
	//�t�@�C�����X�g�̃e�[�u���X�V
	/*#725
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().SPI_UpdateTable();
	}
	*/
	/*#932
	SPI_UpdateFileListWindows(kFileListUpdateType_SameFolderDataUpdated);//#725
	//�u�����t�H���_����J���v���j���[�X�V
	UpdateSameFolderMenu();
	*/
}

void	AApp::SPI_GetSameFolderFilenameTextArray( ATextArray& outFilenameTextArray ) const
{
	AStMutexLocker	locker(mSameFolderMutex);//#982
	
	outFilenameTextArray.DeleteAll();
	if( mCurrentSameFolder == kIndex_Invalid )   return;
	outFilenameTextArray.SetFromTextArray(mSameFolder_Filename.GetObjectConst(mCurrentSameFolder));
}

//R0006
void	AApp::SPI_GetSameFolderSCMStateArray( AArray<ASCMFileState>& outSCMStateArray ) const
{
	outSCMStateArray.DeleteAll();
	if( mCurrentSameFolder == kIndex_Invalid )   return;
	if( mAppPref.GetData_Bool(AAppPrefDB::kUseSCM) == true )
	{
		//#932
		AStMutexLocker	locker(mSameFolderMutex);
		
		mSCMFolderManager.GetFileStateArray(mSameFolder_Folder.GetObjectConst(mCurrentSameFolder),
				mSameFolder_Filename.GetObjectConst(mCurrentSameFolder),outSCMStateArray);
	}
}

void	AApp::SPI_GetCurrentFolderPathForDisplay( AText& outText ) const
{
	outText.DeleteAll();
	if( mCurrentSameFolder == kIndex_Invalid )   return;
	GetApp().GetAppPref().GetFilePathForDisplay(mSameFolder_Folder.GetObjectConst(mCurrentSameFolder),outText);//B0389
}

void	AApp::SPI_GetSameFolderFile( const AIndex inIndex, AFileAcc& outFile ) const
{
	if( mCurrentSameFolder == kIndex_Invalid )   return;
	//#932
	AStMutexLocker	locker(mSameFolderMutex);
	
	AText	filename;
	mSameFolder_Filename.GetObjectConst(mCurrentSameFolder).Get(inIndex,filename);
	outFile.SpecifyChildFile(mSameFolder_Folder.GetObjectConst(mCurrentSameFolder),filename);
}

#pragma mark ===========================

#pragma mark ---�u�����v���W�F�N�g�v

/**
�v���W�F�N�g�t�H���_�o�^
*/
AIndex	AApp::SPI_RegisterProjectFolder( const AFileAcc& inProjectFolder )
{
	//#723
	AStMutexLocker	locker(mSameProjectArrayMutex);
	
	//#958
	//�v���W�F�N�g�t�H���_���ݒ�imiproj�t�@�C�������j�̏ꍇ�́AkIndex_Invalid��Ԃ�
	if( inProjectFolder.IsSpecified() == false )
	{
		return kIndex_Invalid;
	}
	//���łɓ����v���W�F�N�g�t�H���_���o�^�ς݂Ȃ炻��index��Ԃ�
	for( AIndex i = 0; i < mSameProject_ProjectFolder.GetItemCount(); i++ )
	{
		if( mSameProject_ProjectFolder.GetObjectConst(i).Compare(inProjectFolder) == true )
		{
			return i;
		}
	}
	
	//
	AIndex	sameProjectArrayIndex = mSameProjectArray_Pathname.AddNewObject();
	mSameProjectArray_ModeIndex.AddNewObject();//#533
	mSameProjectArray_CollapseData.AddNewObject();//#892
	mSameProjectArray_IsFolder.AddNewObject();
	//#402 MakeSameProjectArrayRecursive(sameProjectArrayIndex,inProjectFolder,inModeIndex);
	mSameProject_ProjectFolder.GetObject(mSameProject_ProjectFolder.AddNewObject()).CopyFrom(inProjectFolder);
	//#533 mSameProject_ModeIndex.Add(inModeIndex);
	//#402 �t�H���_�ǂݍ��݃X���b�h�N��
	AProjectFolderLoaderFactory	factory(this);
	AObjectID	threadObjectID = NVI_CreateThread(factory);
	(dynamic_cast<AProjectFolderLoader&>(NVI_GetThreadByID(threadObjectID))).SPNVI_Run(
																					   mSameProject_ProjectFolder.GetObjectConst(sameProjectArrayIndex),
																					   NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFileListDontDisplayInvisibleFile));
	
	//==================�܂肽���ݏ�Ԃ𕜌�==================
	//#892
	//�v���W�F�N�g�p�X�擾
	AText	projectpath;
	inProjectFolder.GetPath(projectpath);
	projectpath.RemoveLastPathDelimiter();
	//�܂肽���܂ꂽ�t�H���_�̐ݒ荀�ږ��̃��[�v
	AItemCount	itemcount = mAppPref.GetItemCount_TextArray(AAppPrefDB::kFileListCollapseFolder);
	for( AIndex i = 0; i < itemcount; i++ )
	{
		//�܂肽���܂ꂽ�t�H���_�p�X�ݒ���擾
		AText	path;
		mAppPref.GetData_TextArray(AAppPrefDB::kFileListCollapseFolder,i,path);
		//�܂肽���܂ꂽ�t�H���_�p�X�ݒ肪�A�v���W�F�N�g�z���Ȃ�΁AmSameProjectArray_CollapseData�Ɋi�[
		if( path.CompareMin(projectpath) == true && path.GetItemCount() > projectpath.GetItemCount() )
		{
			//�܂肽���܂ꂽ�t�H���_�p�X�ݒ肩�瑊�΃p�X���폜���āAmSameProjectArray_CollapseData�Ɋi�[
			path.Delete(0,projectpath.GetItemCount());
			mSameProjectArray_CollapseData.GetObject(sameProjectArrayIndex).Add(path);
		}
	}
	
	return sameProjectArrayIndex;
}

//ProjectFolder���j���[�X�V
void	AApp::SPI_UpdateSameProjectMenu( const AFileAcc& inProjectFolder, const AModeIndex inModeIndex )
{
	//#932 ���݃`�F�b�N���Ȃ��B�񑶍݂ł����[�_�[�Ŏ擾�ł��Ȃ������Ȃ̂Ŗ��͏��Ȃ��B if( inProjectFolder.Exist() == false )   return;

	//����SameProject�z�񂩂�v���W�F�N�g�t�H���_����
	AIndex	sameProjectArrayIndex = kIndex_Invalid;
	for( AIndex i = 0; i < mSameProject_ProjectFolder.GetItemCount(); i++ )
	{
		if( mSameProject_ProjectFolder.GetObjectConst(i).Compare(inProjectFolder) == true 
					/*#533 && mSameProject_ModeIndex.Get(i) == inModeIndex*/ )
		{
			sameProjectArrayIndex = i;
			break;
		}
	}
	//�v���W�F�N�g�t�H���_���o�^�Ȃ�V�K����
	if( sameProjectArrayIndex == kIndex_Invalid )
	{
		sameProjectArrayIndex = SPI_RegisterProjectFolder(inProjectFolder);
	}
	//inProjectFolder���v���W�F�N�g�t�H���_�ł͂Ȃ��ꍇ�imiproj�����j�́A���݂̃��j���[��invalid�ɂ��ă��^�[��
	if( sameProjectArrayIndex == kIndex_Invalid )
	{
		mCurrentSameProjectIndex = kIndex_Invalid;
		mCurrentSameProjectModeIndex = kIndex_Invalid;
		return;
	}
	
	//���݂̃��j���[�ɓo�^���ꂽPrjectFolder�Ȃ牽���������^�[��
	if( mCurrentSameProjectIndex == sameProjectArrayIndex && 
				mCurrentSameProjectModeIndex == inModeIndex )   return;//#533
	
	//
	mCurrentSameProjectIndex = sameProjectArrayIndex;
	mCurrentSameProjectModeIndex = inModeIndex;//#533
	//�u�����v���W�F�N�g����J���v���j���[�X�V
	UpdateSameProjectMenu();//B0420 #533
	
}

//B0420
//�u�����v���W�F�N�g����J���v���j���[�X�V
void	AApp::UpdateSameProjectMenu()//#533
{
	/*#922
	//���݂̃��j���[���ڂ��폜
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_SameProject);
	//
	if( mCurrentSameProjectIndex == kIndex_Invalid )   return;
	AText	projectFolderPath;
	mSameProject_ProjectFolder.GetObjectConst(mCurrentSameProjectIndex).GetPath(projectFolderPath);
	AItemCount	projectFolderPathLen = projectFolderPath.GetItemCount();
	//���j���[�ɂP���ڂ��o�^
	{
		//#723
		AStMutexLocker	locker(mSameProjectArrayMutex);
		
		for( AIndex menuItem = 0; menuItem < mSameProjectArray_Pathname.GetObject(mCurrentSameProjectIndex).GetItemCount(); menuItem++ )
		{
			//#533
			AModeIndex	modeIndex = mSameProjectArray_ModeIndex.GetObject(mCurrentSameProjectIndex).Get(menuItem);
			//�t�H���_���ځA�܂��́A���݂̃��[�h�Ɠ������[�h�̃t�@�C���݂̂�ǉ�����
			if( modeIndex != kIndex_Invalid && modeIndex != mCurrentSameProjectModeIndex )   continue;
			//
			AText	actiontext;
			mSameProjectArray_Pathname.GetObject(mCurrentSameProjectIndex).Get(menuItem,actiontext);
			AText	menutext;
			actiontext.GetText(projectFolderPathLen,actiontext.GetItemCount()-projectFolderPathLen,menutext);
			NVI_GetMenuManager().AddDynamicMenuItemByMenuItemID(kMenuItemID_SameProject,menutext,actiontext,kObjectID_Invalid,NULL,0,false);
		}
	}
	*/
}

/*#402 �X���b�h�����ɕύX
//mSameProjectArray_Pathname�̓��e�쐬�i�v���W�F�N�g�t�H���_���ċA�I�Ɍ������đS�t�@�C���o�^�j
//mSameProjectArray_Pathname�̎w��Index�̔z��͂��̊֐����ł͍폜����Ȃ��i�폜�����ɒǉ����Ă����j�̂ŁA�֐��R�[�����ō폜�K�v
void	AApp::MakeSameProjectArrayRecursive( const AIndex inSameProjectArrayIndex, const AFileAcc& inFolder, const AModeIndex inModeIndex )
{
	//R0006
	SCMRegisterFolder(inFolder);//SCM��ԓǂݍ��ݏI����AInternalEvent�o�R�Ńt�@�C�����X�g��SPI_UpdateTable()�������s�����
	//
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		AFileAcc	child;
		child.CopyFrom(children.GetObjectConst(index));
		//win 080702 �s���t�@�C����\�����Ȃ��ݒ�Ȃ�mSameProjectArray_Pathname�ɒǉ����Ȃ�
		if( GetAppPref().GetData_Bool(AAppPrefDB::kFileListDontDisplayInvisibleFile) == true )
		{
			if( child.IsInvisible() == true )   continue;
		}
		
		if( child.IsFolder() == true )
		{
			MakeSameProjectArrayRecursive(inSameProjectArrayIndex,child,inModeIndex);
		}
		else
		{
			AText	path;
			child.GetPath(path);
			if( SPI_FindModeIDByTextFilePath(path) == inModeIndex )//R0210
			{
				mSameProjectArray_Pathname.GetObject(inSameProjectArrayIndex).Add(path);
			}
		}
	}
}
*/

//B0420
//mi�ւ̃A�v���ؑ֎���SameProject�̑S�f�[�^���X�V����
void	AApp::SPI_UpdateAllSameProjectData()
{
	//#402
	AObjectArray<AFileAcc>	projectFolerArray;
	//
	{
		//#723
		AStMutexLocker	locker(mSameProjectArrayMutex);
		
		//
		for( AIndex i = 0; i < mSameProject_ProjectFolder.GetItemCount(); i++ )
		{
			//mSameProjectArray_Pathname�ɓo�^���ꂽ�f�[�^��S�čX�V����
			mSameProjectArray_Pathname.GetObject(i).DeleteAll();
			mSameProjectArray_ModeIndex.GetObject(i).DeleteAll();//#533
			mSameProjectArray_IsFolder.GetObject(i).DeleteAll();
			//#402 MakeSameProjectArrayRecursive(i,mSameProject_ProjectFolder.GetObjectConst(i),mSameProject_ModeIndex.Get(i));
			projectFolerArray.GetObject(projectFolerArray.AddNewObject()).CopyFrom(mSameProject_ProjectFolder.GetObjectConst(i));//#402
		}
	}
	
	//#402 �t�H���_�ǂݍ��݃X���b�h�N��
	AProjectFolderLoaderFactory	factory(this);
	AObjectID	threadObjectID = NVI_CreateThread(factory);
	(dynamic_cast<AProjectFolderLoader&>(NVI_GetThreadByID(threadObjectID))).SPNVI_Run(projectFolerArray,
			NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFileListDontDisplayInvisibleFile));
	
	//
	//�t�@�C�����X�g�̃e�[�u���X�V
	/*#725
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().SPI_UpdateTable();
	}
	*/
	/*#932 ���[�_�[�������ɏ���
	SPI_UpdateFileListWindows(kFileListUpdateType_SameProjectDataUpdated);//#725
	//�u�����v���W�F�N�g����J���v���j���[�X�V
	UpdateSameProjectMenu();
	*/
}

//�t�@�C�����X�g�E�C���h�E�p�f�[�^�擾���\�b�h

//�t�H���_����SameProject��Index���擾����
AIndex	AApp::SPI_GetSameProjectArrayIndex( const AFileAcc& inFolder ) const
{
	for( AIndex index = 0; index < mSameProject_ProjectFolder.GetItemCount(); index++ )
	{
		if( mSameProject_ProjectFolder.GetObjectConst(index).Compare(inFolder) == true )
		{
			return index;
		}
	}
	return kIndex_Invalid;
}

/**
�����v���W�F�N�g�̃f�[�^�擾�i�t�@�C�����X�g����R�[�������j
*/
void	AApp::SPI_GetCurrentSameProjectData( AText& outProjectFolderPath,
											ATextArray& outFullPathArray, AArray<AModeIndex>& outModeArray,
											ABoolArray& outIsFolderArray ) const
{
	//#533 if( mCurrentSameProjectIndex == kIndex_Invalid )   return;
	//���ʏ�����
	outProjectFolderPath.DeleteAll();
	outFullPathArray.DeleteAll();
	outModeArray.DeleteAll();
	outIsFolderArray.DeleteAll();
	//���݂̃v���W�F�N�g��������΃��^�[��
	if( mCurrentSameProjectIndex == kIndex_Invalid )   return;//#0
	//
	{
		AStMutexLocker	locker(mSameProjectArrayMutex);
		
		//�v���W�F�N�g�t�H���_�̃p�X���擾�i��ɍŌ�̃p�X�f���~�^�L��ɂ���i�t�@�C�����X�g�ő��΃p�X�𐶐����邽�߂Ɏg���Ă��邽�߁j�j
		mSameProject_ProjectFolder.GetObjectConst(mCurrentSameProjectIndex).GetPath(outProjectFolderPath);
		outProjectFolderPath.AddPathDelimiter(AFileAcc::GetPathDelimiter());
		//�e�f�[�^�擾
		outFullPathArray.SetFromTextArray(mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex));
		outModeArray.SetFromObject(mSameProjectArray_ModeIndex.GetObjectConst(mCurrentSameProjectIndex));
		outIsFolderArray.SetFromObject(mSameProjectArray_IsFolder.GetObjectConst(mCurrentSameProjectIndex));
	}
}

//#892
/**
�w��v���W�F�N�g�́A�܂肽���܂ꂽ�t�H���_�̃p�X�̃��X�g���擾
*/
void	AApp::SPI_GetProjectFolderCollapseData( const AIndex inProjectIndex, ATextArray& outCollapseData ) const
{
	//#723
	AStMutexLocker	locker(mSameProjectArrayMutex);
	
	//�܂肽���܂ꂽ�t�H���_�̃p�X�擾
	outCollapseData.SetFromTextArray(mSameProjectArray_CollapseData.GetObjectConst(inProjectIndex));
}

//#892
/**
�w��v���W�F�N�g�́A�܂肽���܂ꂽ�t�H���_�̃p�X�̃��X�g��ݒ�
*/
void	AApp::SPI_SaveProjectFolderCollapseData( const AIndex inProjectIndex, const ATextArray& inCollapseData )
{
	//#723
	AStMutexLocker	locker(mSameProjectArrayMutex);
	
	//�܂肽���܂ꂽ�t�H���_�̃p�X�ݒ�
	mSameProjectArray_CollapseData.GetObject(inProjectIndex).SetFromTextArray(inCollapseData);
}

/*#892
//R0006
void	AApp::SPI_GetProjectSCMStateArray( AArray<ASCMFileState>& outSCMStateArray ) const
{
	outSCMStateArray.DeleteAll();
	if( mCurrentSameProjectIndex == kIndex_Invalid )   return;
	if( mAppPref.GetData_Bool(AAppPrefDB::kUseSCM) == true )
	{
		if( mSCMFolderManager.IsSCMFolder(mSameProject_ProjectFolder.GetObjectConst(mCurrentSameProjectIndex)) == false )   return;
		
		{
			//#723
			AStMutexLocker	locker(mSameProjectArrayMutex);
			
			//
			for( AIndex menuItem = 0; menuItem < mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex).GetItemCount(); menuItem++ )
			{
				AText	actiontext;
				mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex).Get(menuItem,actiontext);
				AFileAcc	file;
				file.Specify(actiontext);
				outSCMStateArray.Add(mSCMFolderManager.GetFileState(file));
			}
		}
	}
}
*/

//
void	AApp::SPI_GetCurrentProjectPathForDisplay( AText& outText ) const
{
	if( mCurrentSameProjectIndex == kIndex_Invalid )   return;
	//B0389 OK mSameProject_ProjectFolder.GetObjectConst(mCurrentSameProjectIndex).GetPath(outText,kFilePathType_1);
	GetApp().GetAppPref().GetFilePathForDisplay(mSameProject_ProjectFolder.GetObjectConst(mCurrentSameProjectIndex),outText);//B0389
}

void	AApp::SPI_GetSameProjectFile( const AIndex inIndex, AFileAcc& outFile ) const
{
	if( mCurrentSameProjectIndex == kIndex_Invalid )   return;
	AText	path;
	{
		//#723
		AStMutexLocker	locker(mSameProjectArrayMutex);
		
		//�p�X�擾
		mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex).Get(inIndex,path);
	}
	outFile.Specify(path);
}

//#892
/**
�v���W�F�N�g�t�H���_�擾
*/
void	AApp::SPI_GetSameProjectFolder( AFileAcc& outFolder ) const
{
	if( mCurrentSameProjectIndex == kIndex_Invalid )   return;
	outFolder.CopyFrom(mSameProject_ProjectFolder.GetObjectConst(mCurrentSameProjectIndex));
}

//R0000
const ATextArray&	AApp::SPI_GetSameProjectPathnameArray() const
{
	if( mCurrentSameProjectIndex == kIndex_Invalid )   return GetEmptyTextArray();
	//#723
	AStMutexLocker	locker(mSameProjectArrayMutex);
	
	//
	return mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex);
}

//win 080722
//�u�����v���W�F�N�g�v�̃p�X�z��擾�iindex�w��j
const ATextArray&	AApp::SPI_GetSameProjectPathnameArray( const AIndex inProjectIndex ) const
{
	//#723
	AStMutexLocker	locker(mSameProjectArrayMutex);
	
	//
	return mSameProjectArray_Pathname.GetObjectConst(inProjectIndex);
}

/**
�u�����v���W�F�N�g�v�̃p�X���X�g�̂����A�w�胂�[�h�Ɉ�v����p�X�̃��X�g���擾
*/
void	AApp::SPI_GetSameProjectPathnameArrayMatchMode( const AModeIndex inModeIndex, ATextArray& outPathnameArray) const
{
	//���ʏ�����
	outPathnameArray.DeleteAll();
	
	AStMutexLocker	locker(mSameProjectArrayMutex);
	//���݂̃v���W�F�N�g�̊e�t�@�C���p�X���̃��[�v
	AItemCount	itemCount = mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex).GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//���[�h����v�����猋�ʂɒǉ�
		if( mSameProjectArray_ModeIndex.GetObjectConst(mCurrentSameProjectIndex).Get(i) == inModeIndex )
		{
			AText	path;
			mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex).Get(i,path);
			outPathnameArray.Add(path);
		}
	}
}

#pragma mark ===========================

#pragma mark ---�L�[���[�hCSV�t�@�C�����[�h
//#796

/**
�L�[���[�hCSV�t�@�C��(http�o�R)�ǂݍ��ݗv��
*/
void	AApp::SPI_LoadCSVForKeyword( const AText& inModeName, const AText& inURL, const AIndex inCategoryIndex )
{
	//���[�h���A�J�e�S�����L���iLoadCSVForKeyword_Result()�Ŏg���j
	ATextArray	infoTextArray;
	infoTextArray.Add(inModeName);
	AText	categoryIndexText;
	categoryIndexText.SetNumber(inCategoryIndex);
	infoTextArray.Add(categoryIndexText);
	//CSV�t�@�C���ǂ݂��݃X���b�h�J�n
	ACurlThreadFactory	factory(this);
	AObjectID	threadObjectID = NVI_CreateThread(factory);
	(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
			SPNVI_Run(kCurlRequestType_HTTP,inURL,
				kCurlTransactionType_LoadCSVForKeyword,infoTextArray);
}

/**
�L�[���[�hCSV�t�@�C��(http�o�R)�ǂݍ��݊���������
*/
void	AApp::LoadCSVForKeyword_Result( const AText& inResultText, const ATextArray& inInfoTextArray )
{
	//�e�L�X�g�G���R�[�f�B���O�ϊ�
	AText	resultText;
	resultText.SetText(inResultText);
	ATextEncodingFallbackType	fallbackResult = kTextEncodingFallbackType_None;
	ATextEncoding	resultTec = ATextEncodingWrapper::GetUTF8TextEncoding();
	AReturnCode	resultReturnCode = returnCode_CR;
	ADocPrefDB	docPrefDB;
	ABool	notMixed = true;//#995
	SPI_ConvertToUTF8CRUsingDocPref(kLoadTextPurposeType_KeywordCSVFile,resultText,docPrefDB,GetEmptyText(),
									resultTec,resultReturnCode,fallbackResult,notMixed);
	//���[�h�擾
	AText	modeName;
	inInfoTextArray.Get(0,modeName);
	AIndex	modeIndex = SPI_FindModeIndexByModeRawName(modeName);
	if( modeIndex == kIndex_Invalid )   return;
	//�J�e�S���擾
	AText	categoryIndexText;
	inInfoTextArray.Get(1,categoryIndexText);
	//CSV�t�@�C������L�[���[�h�ɒǉ�
	SPI_GetModePrefDB(modeIndex).StartAddCSVKeywordThread(resultText,categoryIndexText.GetNumber());
}

#pragma mark ===========================

#pragma mark ---�֘A�t�@�C�����j���[

void	AApp::SPI_UpdateImportFileMenu( const AObjectArray<AFileAcc>& inFileArray )
{
	//���݂̃��j���[���ڂ��폜
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_OpenImportFile);
	
	//���j���[�ɂP���ڂ��o�^
	AItemCount	menuItemCount = inFileArray.GetItemCount();
	if( menuItemCount > kLimit_ImportFileMenuItemCount )
	{
		menuItemCount = kLimit_ImportFileMenuItemCount;
	}
	for( AIndex index = 0; index < menuItemCount ; index++ )
	{
		AText	path;
		inFileArray.GetObjectConst(index).GetPath(path);
		AText	menutext;
		path.GetFilename(menutext);
		NVI_GetMenuManager().AddDynamicMenuItemByMenuItemID(kMenuItemID_OpenImportFile,menutext,path,kObjectID_Invalid,NULL,0,false);
	}
}

#pragma mark ===========================

#pragma mark ---���[�h���j���[�Ǘ�

void	AApp::SPI_UpdateModeMenu()
{
	//#914 enable/all
	
	//TextArray�擾
	ATextArray	enabledModeDisplayNameArray, allModeDisplayNameArray;
	ATextArray	enabledModeRawNameArray, allModeRawNameArray;
	for( AModeIndex index = 0; index < mModePrefDBArray.GetItemCount(); index++ )
	{
		AText	modeDisplayName, modeRawName;
		mModePrefDBArray.GetObjectConst(index).GetModeDisplayName(modeDisplayName);
		mModePrefDBArray.GetObjectConst(index).GetModeRawName(modeRawName);
		if( mModePrefDBArray.GetObjectConst(index).IsModeEnabled() == true )
		{
			//enabled�ȃ��[�h�̃��X�g�ɒǉ�
			enabledModeDisplayNameArray.Add(modeDisplayName);
			enabledModeRawNameArray.Add(modeRawName);
		}
		else
		{
			modeDisplayName.AddCstring("(disabled)");
		}
		//�S�Ẵ��[�h�̃��X�g�ɒǉ�
		allModeDisplayNameArray.Add(modeDisplayName);
		allModeRawNameArray.Add(modeRawName);
	}
	//TextArray���j���[�i�R���g���[����̃��j���[�j�̍X�V
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_EnabledMode,enabledModeDisplayNameArray,enabledModeRawNameArray);//#232
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_AllMode,allModeDisplayNameArray,allModeRawNameArray);//#232
	//���j���[�o�[�̃��[�h���j���[�̍X�V
	NVI_GetMenuManager().SetDynamicMenuItemByMenuItemID(kMenuItemID_SetMode,enabledModeDisplayNameArray,enabledModeRawNameArray);
	NVI_GetMenuManager().SetDynamicMenuItemByMenuItemID(kMenuItemID_ModePref,allModeDisplayNameArray,allModeRawNameArray);
	//�c�[���o�[�̐ݒ�{�^�����j���[���X�V
	AText	appPrefDisplayName;
	appPrefDisplayName.SetLocalizedText("Toolbar_AppPref");
	AText	separantor("-");
	AText	modePrefDisplayName;
	modePrefDisplayName.SetLocalizedText("Toolbar_ModePref");
	modePrefDisplayName.Insert1(0,0x00);//disable����
	ATextArray	prefDisplayNameArray, prefRawNameArray;
	prefDisplayNameArray.Add(appPrefDisplayName);
	prefRawNameArray.Add(GetEmptyText());
	prefDisplayNameArray.Add(separantor);
	prefRawNameArray.Add(GetEmptyText());
	prefDisplayNameArray.Add(modePrefDisplayName);
	prefRawNameArray.Add(GetEmptyText());
	prefDisplayNameArray.AddFromTextArray(allModeDisplayNameArray);
	prefRawNameArray.AddFromTextArray(allModeRawNameArray);
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_Pref,prefDisplayNameArray,prefRawNameArray);//#232
	
	//�u�V�K�v���j���[�X�V
	SPI_UpdateNewMenu();
}

/**
���[�h���j���[�V���[�g�J�b�g�i���݂̃��[�h�j�̍X�V
*/
void	AApp::SPI_UpdateModeMenuShortcut()
{
	//�őO�ʃh�L�������g�擾
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		//���݂̃��[�h���擾
		AIndex	currentModeIndex = SPI_GetTextDocumentByID(docID).SPI_GetModeIndex();
		//���[�h���j���[�e���ڂ��Ƃ̃��[�v
		AItemCount	count = NVI_GetMenuManager().GetDynamicMenuItemCountByMenuItemID(kMenuItemID_ModePref);
		for( AIndex index = 0; index < count; index++ )
		{
			//�e���ڂɃV���[�g�J�b�g�ݒ�
			ANumber	shortcut = 0;
			AMenuShortcutModifierKeys	modifier = 0;
			if( index == currentModeIndex )
			{
				shortcut = ',';
				modifier = kModifierKeysMask_Command | kModifierKeysMask_Shift;
			}
			NVI_GetMenuManager().SetDynamicMenuItemShortcutByMenuItemID(kMenuItemID_ModePref,index,shortcut,modifier);
		}
	}
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�G���R�[�f�B���O���j���[�Ǘ�

void	AApp::SPI_UpdateTextEncodingMenu()
{
	//TextArray�擾
	/*#844
	ATextArray	textArray;
	textArray.SetFromTextArray(GetAppPref().GetTextEncodingMenuFixedTextArrayRef());
	textArray.AddFromTextArray(GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kTextEncodingMenu));
	*/
	//TextArray���j���[�i�R���g���[����̃��j���[�j�̍X�V
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_TextEncoding,//#232
																					 GetAppPref().GetTextEncodingDisplayNameArray(),
																					 GetAppPref().GetTextEncodingNameArray());
	//���j���[�o�[�̃e�L�X�g�G���R�[�f�B���O���j���[�̍X�V
	NVI_GetMenuManager().SetDynamicMenuItemByMenuItemID(kMenuItemID_SetTextEncoding,
														GetAppPref().GetTextEncodingDisplayNameArray(),
														GetAppPref().GetTextEncodingNameArray());
}

/*#193
#pragma mark ===========================

#pragma mark ---FTP�t�H���_���j���[�Ǘ�

//FTP�t�H���_���j���[���쐬����i�ݒ�ɏ]���ă��[�g�̑S�Ẵ��j���[���ڂ��쐬����j
void	AApp::MakeFTPFolderMenu()
{
	for( AIndex index = 0; index < GetAppPref().GetItemCount_Array(AAppPrefDB::kFTPFolder_Server); index++ )
	{
		SPI_UpdateFTPFolderMenu(kModificationType_RowAdded,index);
	}
}

//FTP�t�H���_���j���[�X�V
void	AApp::SPI_UpdateFTPFolderMenu( const AModificationType inModificationType, const AIndex inIndex )
{
	switch(inModificationType)
	{
		//�l�ύX
	  case kModificationType_ValueChanged:
		{
			AText	currenttext;
			NVI_GetMenuManager().GetDynamicMenuItemActionTextByMenuItemID(kMenuItemID_FTPFolder,inIndex,currenttext);
			AText	newtext;
			GetFTPFolderFTPURL(inIndex,newtext);
			ANumber	currentdepth = 0;
			if( currenttext.GetItemCount() >= 2 )
			{
				currentdepth = currenttext.Get(1)-'0';
				currenttext.Delete(0,2);
			}
			if( newtext.Compare(currenttext) == false || GetAppPref().GetData_NumberArray(AAppPrefDB::kFTPFolder_Depth,inIndex) != currentdepth )
			{
				//���ڍ폜
				SPI_UpdateFTPFolderMenu(kModificationType_RowDeleted,inIndex);
				//���ڒǉ�
				SPI_UpdateFTPFolderMenu(kModificationType_RowAdded,inIndex);
			}
			break;
		}
		//���ڒǉ�
	  case kModificationType_RowAdded:
		{
			//���ڒǉ�
			//�K�w�擾
			ANumber	depth = GetAppPref().GetData_NumberArray(AAppPrefDB::kFTPFolder_Depth,inIndex);
			//���j���[���ڕ\��������쐬
			AText	menutext;
			GetFTPFolderFTPURL(inIndex,menutext);
			AText	numtext;
			numtext.SetFormattedCstring("%d",depth);
			AText	depthtext;
			depthtext.SetLocalizedText("FTPFolderDepth");
			depthtext.ReplaceParamText('0',numtext);
			menutext.AddText(depthtext);
			//���j���[�A�N�V����������쐬
			AText	actiontext;
			GetFTPFolderFTPURL(inIndex,actiontext);
			actiontext.Insert1(0,'0'+depth);
			actiontext.Insert1(0,'R');
			//���j���[�ݒ�
			NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_FTPFolder,inIndex,menutext,actiontext,kObjectID_Invalid,0,0,false);
			break;
		}
		//���ڍ폜
	  case kModificationType_RowDeleted:
		{
			//���ڍ폜
			NVI_GetMenuManager().DeleteDynamicMenuItemByMenuItemID(kMenuItemID_FTPFolder,inIndex);
			break;
		}
	}
}

//
void	AApp::GetFTPFolderFTPURL( const AIndex inIndex, AText& outText ) const
{
	AText	server, user, path;
	GetAppPrefConst().GetData_TextArray(AAppPrefDB::kFTPFolder_Server,inIndex,server);
	GetAppPrefConst().GetData_TextArray(AAppPrefDB::kFTPFolder_User,inIndex,user);
	GetAppPrefConst().GetData_TextArray(AAppPrefDB::kFTPFolder_Path,inIndex,path);
	outText.SetFTPURL(server,user,path);
}
*/
/*#361
//
void	AApp::DoFTPFolderMenuSelected( const AText& inActionText )
{
	//FTP�ڑ��X���b�h�g�p���͉������Ȃ�
	if( SPI_GetFTPConnection().NVI_IsThreadWorking() == true )   return;
	
	if( inActionText.GetItemCount() == 0 )   return;
	
	if( inActionText.Get(0) == 'R' && inActionText.GetItemCount() >= 2 )
	{
		//���j���[�̃��[�g��I���iFTP�t�H���_�ǂݍ��݁j
		AText	text;
		text.SetText(inActionText);
		ANumber	depth = text.Get(1)-'0';
		text.Delete(0,2);
		SPI_GetFTPConnection().SPNVI_Run_GetDirectoryForFTPFolder(text,depth,inActionText);
	}
	else if( inActionText.Get(0) == 'N' && inActionText.GetItemCount() >= 2 )
	{
		//
		AText	text;
		text.SetText(inActionText);
		text.Delete(0,1);
		SPI_GetNewFTPFileWindow().NVI_CreateAndShow();
		SPI_GetNewFTPFileWindow().SPI_SetFolder(text);
		SPI_GetNewFTPFileWindow().NVI_SwitchFocusToFirst();
	}
	else if( inActionText.Get(0) == 'U' && inActionText.GetItemCount() >= 2 )
	{
		AText	text;
		text.SetText(inActionText);
		text.Set(0,'R');
		//�폜���Ȃ������烁�������[�N�H
		DoFTPFolderMenuSelected(text);
	}
	else
	{
		//���[�g�ł͂Ȃ����ځi�t�@�C���j��I�������ꍇ�A����url�̃t�@�C�����J��
		//#361 SPI_StartOpenFTPDocument(inActionText,false);
		SPI_RequestOpenFileToRemoteConnection(inActionText,false);
	}
}
*/
#pragma mark ===========================

#pragma mark ---�t�H���_���X�g���j���[�Ǘ�

//
void	AApp::MakeFolderListMenu()
{
	for( AIndex index = 0; index < GetAppPref().GetItemCount_Array(AAppPrefDB::kFolderListMenu); index++ )
	{
		SPI_UpdateFolderListMenu(kModificationType_RowAdded,index);
	}
}

//
AObjectID	AApp::MakeFolderListMenu_SubMenu( const AFileAcc& inFolder, const ANumber inDepth )
{
	//
	AObjectID	dynamicMenuObjectID = NVI_GetMenuManager().CreateDynamicMenu(kMenuItemID_FolderList,false);
	
	//
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	
	//
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		AText	menutext;
		children.GetObject(index).GetFilename(menutext);
		//�ΏۊO�t�@�C��
		if( menutext.GetLength() == 0 )   continue;
		if( children.GetObject(index).IsFolder() == true )
		{
			if( inDepth < 3 )
			{
				AObjectID	subMenuObjectID = MakeFolderListMenu_SubMenu(children.GetObject(index),inDepth+1);
				NVI_GetMenuManager().AddDynamicMenuItemByObjectID(dynamicMenuObjectID,menutext,menutext,subMenuObjectID,0,0,false);
			}
		}
		else
		{
			AText	filepath;
			children.GetObject(index).GetPath(filepath);
			NVI_GetMenuManager().AddDynamicMenuItemByObjectID(dynamicMenuObjectID,menutext,filepath,kObjectID_Invalid,0,0,false);
		}
	}
	return dynamicMenuObjectID;
}

void	AApp::SPI_UpdateFolderListMenu( const AModificationType inModificationType, const AIndex inIndex )
{
	switch(inModificationType)
	{
	  case kModificationType_ValueChanged:
		{
			//���ڍ폜
			SPI_UpdateFolderListMenu(kModificationType_RowDeleted,inIndex);
			//���ڒǉ�
			SPI_UpdateFolderListMenu(kModificationType_RowAdded,inIndex);
			break;
		}
	  case kModificationType_RowAdded:
		{
			//���ڒǉ�
			AText	folderPath;
			GetAppPref().GetData_TextArray(AAppPrefDB::kFolderListMenu,inIndex,folderPath);
			AFileAcc	folder;
			folder.SpecifyWithAnyFilePathType(folderPath);//B0389 OK ,kFilePathType_1);
			//B0389
			AText	actiontext;
			folder.GetPath(actiontext);//#910 ,kFilePathType_1);
			
			AObjectID	subMenuObjectID = MakeFolderListMenu_SubMenu(folder,1);
			NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_FolderList,inIndex,folderPath,actiontext,subMenuObjectID,0,0,false);
			break;
		}
	  case kModificationType_RowDeleted:
		{
			//���ڍ폜
			NVI_GetMenuManager().DeleteDynamicMenuItemByMenuItemID(kMenuItemID_FolderList,inIndex);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark ---�c�[�����j���[

void	AApp::SPI_UpdateToolMenu( const AModeIndex inModeIndex )
{
	if( mCurrentToolMenuModeIndex != inModeIndex )
	{
		if( mCurrentToolMenuModeIndex != kIndex_Invalid )
		{
			SPI_GetModePrefDB(mCurrentToolMenuModeIndex).DetachToolMenu();
		}
		if( inModeIndex != kIndex_Invalid )
		{
			SPI_GetModePrefDB(inModeIndex).AttachToolMenu();
		}
		mCurrentToolMenuModeIndex = inModeIndex;
	}
}

#pragma mark ===========================

#pragma mark ---Transliterate

//R0017
ABool	AApp::SPI_Transliterate( const AText& inTitle, const AText& inText, AText& outText ) const
{
	outText.DeleteAll();//B0000
	AIndex	transliterateIndex = mTransliterate_Title.Find(inTitle);
	if( transliterateIndex == kIndex_Invalid )   return false;
	AItemCount	count = inText.GetItemCount();
	for( AIndex pos = 0; pos < count;  )
	{
		AItemCount	count2 = mTransliterate_Source.GetObjectConst(transliterateIndex).GetItemCount();
		ABool	found = false;
		for( AIndex i = 0; i < count2; i++ )
		{
			AText	src;
			mTransliterate_Source.GetObjectConst(transliterateIndex).Get(i,src);
			if( pos + src.GetItemCount() > inText.GetItemCount() )   continue;
			if( src.Compare(inText,pos,src.GetItemCount()) == true )
			{
				AText	text;
				mTransliterate_Destination.GetObjectConst(transliterateIndex).Get(i,text);
				outText.AddText(text);
				pos += src.GetItemCount();
				found = true;
				break;
			}
		}
		if( found == false )
		{
			AText	ch;
			inText.Get1UTF8Char(pos,ch);
			outText.AddText(ch);
		}
	}
	return true;
}

#pragma mark ===========================

#pragma mark ---�ݒ�֘A���擾

//InlineInput�n�C���C�g�����F���擾
void	AApp::SPI_GetInlineInputHiliteColor( const AIndex inIndex, AColor& outColor ) const
{
	AIndex	index = inIndex;
	//#844if( mAppPref.GetData_Bool(AAppPrefDB::kInlineInputColorLine) == false )
	{
		if( index >= mInlineInputHiliteColorArray_Gray.GetItemCount() )   index = mInlineInputHiliteColorArray_Gray.GetItemCount()-1;
		outColor = mInlineInputHiliteColorArray_Gray.Get(inIndex);
	}
	/*#844
	else
	{
		if( index >= mInlineInputHiliteColorArray_Color.GetItemCount() )   index = mInlineInputHiliteColorArray_Gray.GetItemCount()-1;
		outColor = mInlineInputHiliteColorArray_Color.Get(inIndex);
	}
	*/
}

#pragma mark ===========================

#pragma mark --- �e�L�X�g�t�@�C���ǂݍ���

/**
�t�@�C���A�������́A�h�L�������g����e�L�X�g��ǂݍ��݁AUTF8�֕ϊ�����B
@note thread-safe
*/
void	AApp::SPI_LoadTextFromFileOrDocument( const ALoadTextPurposeType inLoadTextPurposeType,
											 const AFileAcc& inFile, AText& outText, AModeIndex& outModeIndex,
											 AText& outTextEncoding )//B0313
{
	//#898 ABool	utf8ok = true;//B0313
	//�G�C���A�X����
	AFileAcc	file;
	file.ResolveAnyLink(inFile);
	//Text�ǂݍ���
	ABool	read = false;//#831 �ǂݍ��ݍς݃t���O������
	AObjectID	openedDocID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
	if( openedDocID != kObjectID_Invalid )
	{
		//�t�@�C���ɑΉ�����h�L�������g�����݂���ꍇ
		
		//document�I�u�W�F�N�g���擾���Aretain����i�X���b�h������R�[������邽�߁Aretain����document�I�u�W�F�N�g�������Ȃ��悤�ɂ���j
		ADocument*	doc = GetApp().NVI_GetDocumentAndRetain(openedDocID);
		if( doc != NULL )
		{
			//document�I�u�W�F�N�g��retain��K��������邽�߂̃X�^�b�N�N���X
			AStDecrementRetainCounter	releaser(doc);
			
			ADocument_Text&	textdoc = reinterpret_cast<ADocument_Text&>(*doc);
			//�h�L�������g���t�@�C���ǂݍ��ݍς݂Ȃ�h�L�������g����R�s�[���� #831�i�h�L�������g�Ƀt�@�C�����ǂݍ��݂̏ꍇ�����肦��悤�ɂȂ������ߏ����ǉ��j
			if( textdoc.SPI_IsLoaded() == true )
			{
				textdoc.SPI_GetText(outText);
				outModeIndex = textdoc.SPI_GetModeIndex();
				textdoc.SPI_GetTextEncoding(outTextEncoding);
				read = true;//#831 �ǂݍ��ݍς݃t���OON
			}
		}
	}
	//else
	//#831 �ǂݍ��ݍς݃t���OOFF�̂܂܂Ȃ�A�t�@�C������ǂݍ���
	if( read == false )
	{
		SPI_LoadTextFromFile(inLoadTextPurposeType,file,outText,outModeIndex,outTextEncoding);
	}
	//#898 return utf8ok;//B0313
}

//#723 SPI_GetTextFromFileOrDocument()���番���BDocument����ǂޕK�v�������ꍇ�A��������g�p����B�i�X���b�h�����Document�Q�Ƃ��Ȃ�ׂ����Ȃ��悤�ɂ���j
/**
�t�@�C������e�L�X�g��ǂݍ��݁AUTF8�֕ϊ�����B
@note thread-safe
*/
void	AApp::SPI_LoadTextFromFile( const ALoadTextPurposeType inLoadTextPurposeType, 
								   const AFileAcc& inFile, AText& outText )
{
	AModeIndex	modeIndex = kIndex_Invalid;
	AText	tecname;
	SPI_LoadTextFromFile(inLoadTextPurposeType,inFile,outText,modeIndex,tecname);
}
void	AApp::SPI_LoadTextFromFile( const ALoadTextPurposeType inLoadTextPurposeType, 
								   const AFileAcc& inFile, AText& outText, AModeIndex& outModeIndex, AText& outTextEncodingName )
{
	//==================�����N����==================
	AFileAcc	file;
	file.ResolveAnyLink(inFile);
	
	//==================�t�@�C������e�L�X�g�ǂݍ���==================
	file.ReadTo(outText);
	
	//==================DocPref���t�@�C������ǂݍ���==================
	ADocPrefDB	docPrefDB;
	AText	textfilepath;//#898
	file.GetPath(textfilepath);//#898
	//doc pref DB����ǂݍ��݁A���[�hindex�擾
	ABool	loadedFromFile = false;//#1429
	outModeIndex = docPrefDB.LoadPref(textfilepath, loadedFromFile);//#898 #1429
	
	//==================UTF8CR�֕ϊ�==================
	//doc pref���g����UTF8/CR�֕ϊ�
	ATextEncodingFallbackType	fallbackResult = kTextEncodingFallbackType_None;
	ATextEncoding	resultTec = ATextEncodingWrapper::GetUTF8TextEncoding();
	AReturnCode	resultReturnCode = returnCode_CR;
	ABool	notMixed = true;//#995
	SPI_ConvertToUTF8CRUsingDocPref(inLoadTextPurposeType,outText,docPrefDB,textfilepath,resultTec,resultReturnCode,fallbackResult,notMixed);
	ATextEncodingWrapper::GetTextEncodingName(resultTec,outTextEncodingName);
	
	
#if 0
	//�h�L�������g�I�[�v�����łȂ���΁A�t�@�C������ǂݍ���
	//�܂��ADocPrefDB����TextEncoding��ǂݍ���
	AText	tecname;
	docPrefDB.GetData_Text(ADocPrefDB::kTextEncodingName,tecname);
	ATextEncoding	tec;
	ATextEncodingWrapper::GetTextEncodingFromName(tecname,tec);
	//docPref�Ŗ��w��Ȃ玩���F��
	/*#898 #764 if( docPrefDB.IsStillDefault(ADocPrefDB::kTextEncodingName) == true )
	if( 
	{
		tec = ATextEncodingWrapper::GetUTF8TextEncoding();//B0390
		if( outText.GuessTextEncoding(true,true,true,false,tec) == false )//B0390
		{
			tec = ATextEncodingWrapper::GetSJISTextEncoding();
		}
	}
	*/
	if( docPrefDB.IsLoadedFromPrefFile() == false )
	{
		//DocPref���t�@�C������ǂݍ���ł��Ȃ��ꍇ�i��doc pref�t�@�C�������������ꍇ��V�K�t�@�C���j�A�������s��
		//�����m�肵�Ȃ��ꍇ�́ADocPref�ɐݒ肵���l�i��ModePref�ɐݒ肳��Ă���f�t�H���g�l�j���g�p����
		if( outText.GuessTextEncoding(true,true,true,false,tec) == false )
		{
		}
	}
	//���e�X�g #829
	//tec = ATextEncodingWrapper::GetJISTextEncoding();
	//UTF8�֕ϊ�
	if( outText.GetItemCount() < 1024*1024*16 )//B0313 16MB�ȏ�̃t�@�C���̓e�L�X�g�łȂ��\���������̂Ŗ��ϊ��i�ϊ�����ƕ��׏d���Ȃ肷���j
	{
		//#307 ATextEncoding	resulttec;
		//B0400 utf8ok = AUtil::ConvertToUTF8CRAnyway(outText,tec,resulttec);
		utf8ok = NVI_ConvertToUTF8CRAnyway(outText,tec,true);//B0400 #182 #307
	}
	//
	ATextEncodingWrapper::GetTextEncodingName(tec,outTextEncodingName);
	/*#898
	//���[�h����
	AText	filepath;//R0210
	file.GetPath(filepath);//R0210
	outModeIndex = GetApp().SPI_FindModeIDByTextFilePath(filepath);//R0210
	if( docPrefDB.IsStillDefault(ADocPrefDB::kModeName) == false )
	{
		AText	modename;
		docPrefDB.GetData_Text(ADocPrefDB::kModeName,modename);
		outModeIndex = GetApp().SPI_FindModeIndexByModeName(modename);
		if( outModeIndex == kIndex_Invalid )   outModeIndex = kStandardModeIndex;
	}
	*/
	return utf8ok;
#endif
}

/**
doc pref���g����UTF8/CR�֕ϊ�
@param inFile �����[�g�t�@�C�����̏ꍇ�A��specify�ł��ǂ��Bspecify����Ă����EA���g����TEC�����Ɏg�p����B
@note thread-safe
*/
ABool	AApp::SPI_ConvertToUTF8CRUsingDocPref( const ALoadTextPurposeType inLoadTextPurposeType,
											  AText& ioText, const ADocPrefDB& inDocPrefDB, const AText& inFilePath,
											  ATextEncoding& outResultEncoding,
											  AReturnCode& outResultReturnCode,
											   ATextEncodingFallbackType& outResultFallbackType,
											   ABool& outNotMixed ) const
{
	//==================�t�H�[���o�b�N�ASJIS���X���X�t�H�[���o�b�N�̎g�p�۔���==================
	ABool	useFallback = NVI_GetAppPrefDBConst().GetData_Bool(AAppPrefDB::kUseFallbackForTextEncodingConversion);//#830
	ABool	useLosslessFallback = false;//#844 SJIS���X���X�t�H�[���o�b�N��drop NVI_GetAppPrefDBConst().GetData_Bool(AAppPrefDB::kUseSJISLosslessFallback);
	//
	switch( inLoadTextPurposeType )
	{
	  case kLoadTextPurposeType_MultiFileFinder_Replace:
	  case kLoadTextPurposeType_LoadDocument_ForMultiFileReplace:
		{
			useFallback = false;
			useLosslessFallback = false;
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	
	//DocPref���t�@�C������ǂݍ���ł��Ȃ��ꍇ�Fmode pref��tec�̒l���i�[����Ă���
	//DocPref���t�@�C������ǂݍ��񂾏ꍇ�F�t�@�C���ɋL�����ꂽtec�̒l���i�[����Ă���
	//==================(II) DocPref�w��iModePref�ɂ��f�t�H���g�l�ł͂Ȃ��j�ɂ��e�L�X�g�G���R�[�f�B���O==================
	//Cocoa�p�ݒ肪�ݒ肳��Ă���΂������D��B�����łȂ���΁ACarbon�p�ݒ��ǂݍ��ށB #1040
	AText	tecname;
	inDocPrefDB.GetData_Text(ADocPrefDB::kTextEncodingNameCocoa,tecname);
	if( tecname.GetItemCount() == 0 ) 
	{
		inDocPrefDB.GetData_Text(ADocPrefDB::kTextEncodingName,tecname);
	}
	ATextEncoding	tec = ATextEncodingWrapper::GetTextEncodingFromName(tecname);
	//Doc Pref�̃e�L�X�g�G���R�[�f�B���O�̒l���i���[�h�ݒ肩��ǂݍ��񂾒l�ł͂Ȃ��j�t�@�C������ǂݍ��񂾒l�Ȃǂł���ꍇ�́A
	//doc pref�̐ݒ�l���g�p����B
	if( inDocPrefDB.IsTextEncodingFixed() == true )
	{
		//�G���[������������(20%�ȏ�)�Ƃ��́Adoc pref�̊ԈႢ�̉\���������̂ŁADoc pref�͎g�킸�A(III)�A(IV)��K�p����
		AFloatNumber	errorRate = 0.0;
		if( tec == ATextEncodingWrapper::GetUTF8TextEncoding() )
		{
			ioText.CalcUTF8ErrorRate(1024*1024,errorRate);
		}
		if( tec == ATextEncodingWrapper::GetEUCTextEncoding() )
		{
			ioText.CalcEUCErrorRate(1024*1024,errorRate);
		}
		if( tec == ATextEncodingWrapper::GetSJISTextEncoding() )
		{
			ioText.CalcSJISErrorRate(1024*1024,errorRate);
		}
		if( errorRate < 0.2 )
		{
			//------------------�e�L�X�g�G���R�[�f�B���O�ϊ����s�i�ϊ��G���[�łȂ���΁A�����ŏI���j------------------
			if( SPI_ConvertToUTF8CR(ioText,tec,true,
									useFallback,useLosslessFallback,
									(AReturnCode)(inDocPrefDB.GetData_Number(ADocPrefDB::kReturnCode)),
									outResultEncoding,outResultReturnCode,outResultFallbackType,outNotMixed) == true )
			{
				return true;
			}
		}
	}
	
	//==================(III)�����ɂ��e�L�X�g�G���R�[�f�B���O==================
	if( NVI_GetAppPrefDBConst().GetData_Bool(AAppPrefDB::kUseGuessTextEncoding) == true )//#764
	{
		if( GuessTextEncoding(ioText,inFilePath,tec) == true )
		{
			//------------------�e�L�X�g�G���R�[�f�B���O�ϊ����s�i�ϊ��G���[�łȂ���΁A�����ŏI���j------------------
			if( SPI_ConvertToUTF8CR(ioText,tec,true,
									useFallback,useLosslessFallback,//��Ƀt�H�[���o�b�N���g�p���Ȃ��Ă����������A��������ƁA�G���[����������΂����ɕK���������s���Ă��܂��B�i�G���[��臒l�����������Ӗ����Ȃ��Ȃ�j
									(AReturnCode)(inDocPrefDB.GetData_Number(ADocPrefDB::kReturnCode)),
									outResultEncoding,outResultReturnCode,outResultFallbackType,outNotMixed) == true )
			{
				return true;
			}
		}
	}
	
	//==================(IV)���[�h�ݒ�ɂ��e�L�X�g�G���R�[�f�B���O==================
	AText	modename;
	inDocPrefDB.GetData_Text(ADocPrefDB::kModeName,modename);
	AModeIndex	modeIndex = SPI_FindModeIndexByModeRawName(modename);
	if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_Text(AModePrefDB::kDefaultTextEncoding,tecname);
	tec = ATextEncodingWrapper::GetTextEncodingFromName(tecname);
	return SPI_ConvertToUTF8CR(ioText,tec,true,
							   useFallback,useLosslessFallback,
							   (AReturnCode)(inDocPrefDB.GetData_Number(ADocPrefDB::kReturnCode)),
							   outResultEncoding,outResultReturnCode,outResultFallbackType,outNotMixed);
}

/**
�e�L�X�g�G���R�[�f�B���O�̐���
@note thread-safe
*/
ABool	AApp::GuessTextEncoding( const AText& inText, const AText& inFilePath, ATextEncoding& ioTextEncoding ) const
{
	/*�ڂɌ�����������Â炢�̂łЂƂ܂��g�p���Ȃ����Ƃɂ���B�Ή�����Ȃ�A�ŗD��ł͂Ȃ��A�Ō�ɂ��ׂ����H
#if IMPLEMENTATION_FOR_MACOSX
	//Mac OS X �g�������Ƀe�L�X�g�G���R�[�f�B���O���ݒ肳��Ă���΁A����𐄑����ʂƂ��čŗD��Ŏg�p����B
	if( inFilePath.GetItemCount() > 0 )
	{
		//EA
		//R0230
		AFileAcc	file;
		file.Specify(inFilePath);
		AText	xattrtecname;
		if( file.GetTextEncodingByXattr(xattrtecname) == true )
		{
			if( xattrtecname.GetItemCount() > 0 )
			{
				ioTextEncoding = ATextEncodingWrapper::GetTextEncodingFromName(xattrtecname);
				return true;
			}
		}
	}
#endif
	*/
	//
	ABool	result = inText.GuessTextEncoding(ioTextEncoding);
	return result;
}

//#898
#if 0
//
void	AApp::SPI_GetTextFromFile( const AFileAcc& inFile, AText& outText ) const
{
	//DocPref���t�@�C������ǂݍ���
	ADocPrefDB	docPrefDB;
	AText	textfilepath;//#898
	inFile.GetPath(textfilepath);//#898
	docPrefDB.LoadPref(textfilepath);//#898
	//�܂��ADocPrefDB����TextEncoding��ǂݍ���
	AText	tecname;
	docPrefDB.GetData_Text(ADocPrefDB::kTextEncodingName,tecname);
#if IMPLEMENTATION_FOR_MACOSX
	//R0230 �g����������ǂݍ��ށi�g�������D��j
	AText	xattrtecname;
	if( inFile.GetTextEncodingByXattr(xattrtecname) == true )
	{
		tecname.SetText(xattrtecname);
		docPrefDB.SetData_Text(ADocPrefDB::kTextEncodingName,tecname);
	}
#endif
	//
	ATextEncoding	tec;
	ATextEncodingWrapper::GetTextEncodingFromName(tecname,tec);
	//�t�@�C������e�L�X�g�ǂݍ���
	inFile.ReadTo(outText);
	//docPref�Ŗ��w��Ȃ玩���F��
	if( docPrefDB.IsStillDefault(ADocPrefDB::kTextEncodingName) == true )
	{
		tec = ATextEncodingWrapper::GetUTF8TextEncoding();//B0390
		if( outText.GuessTextEncoding(true,true,true,false,tec) == false )//B0390
		{
			tec = ATextEncodingWrapper::GetSJISTextEncoding();
		}
	}
	//UTF8�֕ϊ�
	//#307 ATextEncoding	resulttec;
	//B0400 AUtil::ConvertToUTF8CRAnyway(outText,tec,resulttec);
	NVI_ConvertToUTF8CRAnyway(outText,tec,true);//B0400 #182 #307
}
#endif

//B0310
/**
�e�L�X�g�t�@�C���iUTF8/CR�j��������
*/
void	AApp::SPI_WriteTextFile( const AText& inText, const AFileAcc& inFile )
{
	//�t�@�C����������
	inFile.WriteFile(inText);
	//doc pref�X�V�i�e�L�X�g�G���R�[�f�B���O��UTF8�ɐݒ�j
	ADocPrefDB	docPrefDB;
	AText	textfilepath;//#898
	inFile.GetPath(textfilepath);//#898
	ABool	loadedFromFile = false;//#1429
	docPrefDB.LoadPref(textfilepath, loadedFromFile);//#1429
	AText	tecname("UTF-8");
	//#1040 docPrefDB.SetData_Text(ADocPrefDB::kTextEncodingName,tecname);
	docPrefDB.SetTextEncoding(tecname);//#1040
	/*#898
	if( docPrefDB.IsStillDefault(ADocPrefDB::kModeName) == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(kStandardModeIndex).GetModeName(text);
		docPrefDB.SetData_Text(ADocPrefDB::kModeName,text);
	}
	*/
	docPrefDB.SavePref(textfilepath);//B0406
}

//B0400
/**
�����e�L�X�g(UTF8,CR)�֕ϊ�����
@return �ϊ��o���Ȃ������Ƃ���false�ƂȂ�Bfallback���g���Ă��ϊ��ł����Ƃ���true�ƂȂ�B
*/
ABool	AApp::SPI_ConvertToUTF8CR( AText& ioText, const ATextEncoding& inTextEncoding, const ABool inFromExternalData,
								   const ABool inUseFallback, const ABool inUseLosslessFallback,
								   const AReturnCode inDefaultReturnCode,//�e�L�X�g���ɉ��s�R�[�h�����������ꍇ�̃f�t�H���g���s�R�[�h
								   ATextEncoding& outResultEncoding,
								   AReturnCode& outResultReturnCode,
								   ATextEncodingFallbackType& outResultFallbackType,
								   ABool& outNotMixed ) const //#182 #307 #473 #764
{
	outResultEncoding = inTextEncoding;
	outResultReturnCode = returnCode_CR;
	outResultFallbackType = kTextEncodingFallbackType_None;//#473
	
	AText	origText;
	origText.SetText(ioText);
	
	ABool	fallback = false;//B0168
	
	/*#844 SJIS���X���X�t�H�[���o�b�N��drop�i�ʏ�t�H�[���o�b�N�ł��J�����Ƃ͂ł���̂Łj
	//==================�P�DSJIS���X���X�t�H�[���o�b�N�ϊ�==================
	//#473
	//���X���X�t�H�[���o�b�N�ϊ������݂�
	if( inTextEncoding == ATextEncodingWrapper::GetSJISTextEncoding() && inUseLosslessFallback == true )
	{
		if( ConvertFromSJISToUTF8UsingLosslessFallback(ioText,inFromExternalData,inUseFallback) == true )
		{
			//�ُ핶�������݂��A���A�t�H�[���o�b�N�ϊ�����
			outResultEncoding = inTextEncoding;//#473
			outResultReturnCode = ioText.ConvertReturnCodeToCR(inDefaultReturnCode);//#307
			outResultFallbackType = kTextEncodingFallbackType_Lossless;//#473
			return true;
		}
	}
	*/
	
	//==================�Q�D�ʏ�ϊ�==================
	//�ʏ�ϊ������݂�
	if( ioText.ConvertToUTF8(inTextEncoding,inUseFallback,fallback,inFromExternalData) == true )//B0400
	{
		outResultEncoding = inTextEncoding;//#473
		outResultReturnCode = ioText.ConvertReturnCodeToCR(inDefaultReturnCode,outNotMixed);//B0400 #307 #995
		//#473
		if( fallback == true )
		{
			outResultFallbackType = kTextEncodingFallbackType_Normal;
			return true;
		}
		else
		{
			return true;
		}
	}
	
	//============�R�D����text�̂܂�============
	ioText.SetText(origText);
	outResultEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
	outResultReturnCode = ioText.ConvertReturnCodeToCR(inDefaultReturnCode,outNotMixed);//#995
	return false;
}

//#844 SJIS���X���X�t�H�[���o�b�N��drop
#if 0
//#473
/**
SJIS���X���X�t�H�[���o�b�N�ϊ�
@return �ُ핶�������݂��A���A�t�H�[���o�b�N�ϊ�����������true�ifalse�̏ꍇ��ioText�ɕω������j
*/
ABool	AApp::ConvertFromSJISToUTF8UsingLosslessFallback( AText& ioText, const ABool inFromExternalData, const ABool inUseFallback ) const
{
	AText	text;
	//SJIS�e�L�X�g���ُ̈핶������
	ABool	abnormalCharExist = false;
	AArray<AUChar>	escape01Array;
	AItemCount	len = ioText.GetItemCount();
	for( AIndex pos = 0; pos < len; pos++ )
	{
		//1�o�C�g�ڎ擾
		AUChar	ch = ioText.Get(pos);
		if( ch == 0x01 )
		{
			//0x01�i�[
			escape01Array.Add(0x01);
			text.Add(0x01);
		}
		else if( ((ch>=0x81&&ch<=0x9F)||(ch>=0xE0&&ch<=0xFC)) )
		{
			//2�o�C�g�ڎ擾
			AUChar	ch2 = 0;
			if( pos+1 < len )
			{
				ch2 = ioText.Get(pos+1);
			}
			if( ((ch2>=0x40&&ch2<=0x7E)||(ch2>=0x80&&ch2<=0xFC)) )
			{
				//����SJIS�Q�o�C�g����
				text.Add(ch);
				text.Add(ch2);
				pos++;
				continue;
			}
			else
			{
				//�ُ�i2�o�C�g�ڂ��K�i�O�j
				escape01Array.Add(ch);
				text.Add(0x01);//0x01�i�[
				abnormalCharExist = true;
				continue;
			}
		}
		else
		{
			//�P�o�C�g����
			text.Add(ch);
			continue;
		}
	}
	//�ُ핶�������݂��Ȃ����return
	if( abnormalCharExist == false )   return false;
	
	//UTF8�֕ϊ�
	ABool	fallback = false;
	if( text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding(),inUseFallback,fallback,inFromExternalData) == true )
	{
		//#764 if( text.CheckUTF8Text() == true )
		{
			//ioText�S�폜
			ioText.DeleteAll();
			//�ϊ����UTF8�e�L�X�g��0x01���������āAescape01Array�ɏ]���āA�����ϊ�����
			AIndex	escape01pos = 0;
			AItemCount	textlen = text.GetItemCount();
			for( AIndex pos = 0; pos < textlen; pos++ )
			{
				AUChar	ch = text.Get(pos);
				if( ch == 0x01 )
				{
					AUChar	escapeCh = escape01Array.Get(escape01pos);
					if( escapeCh == 0x01 )
					{
						//0x01�̂܂ܒǉ�
						ioText.Add(0x01);
					}
					else
					{
						//���p�� U+105CXX�ɕϊ����Ēǉ�
						ioText.Add(0xF4);
						ioText.Add(0x85);
						ioText.Add(0xB0+((escapeCh&0xC0)>>6));
						ioText.Add(0x80+((escapeCh&0x3F)));
					}
				}
				else
				{
					//�ϊ������ǉ�
					ioText.Add(ch);
				}
			}
			return true;
		}
	}
	return false;
}
#endif

//#898
/**
ModeIndex���t�@�C������擾����iDocPref������΂����D�悷��j
*/
AModeIndex	AApp::SPI_GetModeIndexFromFile( const AFileAcc& inFile ) const
{
	//DocPref���t�@�C������ǂݍ���
	ADocPrefDB	docPrefDB;
	AText	textfilepath;
	inFile.GetPath(textfilepath);
	ABool	loadedFromFile = false;//#1429
	return docPrefDB.LoadPref(textfilepath, loadedFromFile);//#1429
}

#pragma mark ===========================

#pragma mark --- SCM
//R0006

//SCMManager�Ƀt�H���_���o�^�Ȃ�o�^����
void	AApp::SCMRegisterFolder( const AFileAcc& inFolder )
{
	if( GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
	{
		mSCMFolderManager.RegisterFolder(inFolder);
	}
}
void	AApp::SCMRegisterFolderByFile( const AFileAcc& inFile )
{
	if( GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
	{
		AFileAcc	folder;
		folder.SpecifyParent(inFile);
		mSCMFolderManager.RegisterFolder(folder);
	}
}

void	AApp::SCMUpdateFolderStatusByFile( const AFileAcc& inFile )
{
	if( GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
	{
		AFileAcc	folder;
		folder.SpecifyParent(inFile);
		mSCMFolderManager.UpdateFolderStatus(folder);
	}
}

//SCMFolderData��update���ꂽ�Ƃ��ɃR�[�������
//�t�@�C�����X�g���̕\���X�V
void	AApp::SPI_SCMFolderDataUpdated( const AFileAcc& inFolder )
{
	SPI_UpdateSCMDisplayAll();
}

void	AApp::SPI_UpdateSCMDisplayAll()
{
	/*#725 InfoHeader�p�~
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObjectConst(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_SetInfoHeaderText(GetEmptyText());
		}
	}
	*/
	//�^�C�g���o�[��SVN��ԍX�V #725
	NVI_UpdateTitleBar();
	/*#725
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().SPI_UpdateTable();
	}
	*/
	SPI_UpdateFileListWindows(kFileListUpdateType_SCMDataUpdated);//#725
}

void	AApp::UpdateAllSCMData()
{
	mSCMFolderManager.UpdateFolderStatusAll();
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		SPI_GetTextDocumentByID(docID).SPI_DoRepositoryUpdated();
	}
	NVI_RefreshAllWindow();
}

#pragma mark ===========================

#pragma mark --- �e���|�����e�L�X�g�ۑ�

//B0446
//
void	AApp::SPI_SaveTemporaryText( const AText& inTempText )
{
	AFileAcc	apppreffolder;
	AFileWrapper::GetAppPrefFolder(apppreffolder);
	AFileAcc	tempfile;
	tempfile.SpecifyChild(apppreffolder,"miLastSaveOldText");
	
	tempfile.CreateFile();
	SPI_WriteTextFile(inTempText,tempfile);
}

//B0446
void	AApp::DeleteTemporaryTextFile()
{
	AFileAcc	apppreffolder;
	AFileWrapper::GetAppPrefFolder(apppreffolder);
	AFileAcc	tempfile;
	tempfile.SpecifyChild(apppreffolder,"miLastSaveOldText");
	if( tempfile.Exist() == true )
	{
		tempfile.DeleteFile();
	}
}

#pragma mark ===========================

#pragma mark --- �X�V���f

//#291
/**
JumpList�ݒ�f�[�^�X�V
*/
/*#725
void	AApp::SPI_UpdatePropertyJumpListAll()
{
	//JumpList�E�C���h�E�̓����x�X�V
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_JumpList )
		{
			mWindowArray.GetObject(index).NVI_UpdateProperty();
		}
	}
	//JumpList View�̃t�H���g�X�V
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObject(index).NVI_GetDocumentType() == kDocumentType_Text )
		{
			SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_UpdateJumpListProperty();
		}
	}
}
*/

//#240
/**
�w��WindowID�ȊO��JumpList��S��Hide�ɂ���
*/
/*#725 SPI_HideOtherFloatingJumpListWindows()�ɕύX
void	AApp::SPI_HideOtherJumpList( const AWindowID inWindowID )
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_JumpList )
		{
			if( mWindowArray.GetObject(index).GetObjectID() != inWindowID )
			{
				mWindowArray.GetObject(index).NVI_HideIfNotOverlayMode();
			}
		}
	}
}
*/

#pragma mark ===========================

#pragma mark --- �W�����v���X�g����
//#454

/**
�W�����v���X�g�����X�V
*/
void	AApp::SPI_UpdateJumpListHistory( const AText& inSelectionText, const AFileAcc& inDocFile )
{
	//����ǉ��֎~�Ȃ牽�����Ȃ�
	if( GetAppPref().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )   return;
	
	//
	mJumpListHistory.Insert1(0,inSelectionText);
	AText	path;
	inDocFile.GetPath(path);
	path.Add(kUChar_LineEnd);
	path.AddText(inSelectionText);
	mJumpListHistory_Path.Insert1(0,path);
	//�������ڂ���������폜
	for( AIndex i = 1; i < mJumpListHistory_Path.GetItemCount(); i++ )
	{
		if( mJumpListHistory_Path.GetTextConst(i).Compare(path) == true )
		{
			mJumpListHistory.Delete1(i);
			mJumpListHistory_Path.Delete1(i);
			break;
		}
	}
	//����max��葽����Έ�ԍŌ�̂��폜
	if( mJumpListHistory.GetItemCount() >= kJumpListHistoryMax )
	{
		mJumpListHistory.Delete1(mJumpListHistory.GetItemCount()-1);
		mJumpListHistory_Path.Delete1(mJumpListHistory_Path.GetItemCount()-1);
	}
	//���j���[�X�V
	GetApp().NVI_GetMenuManager().SetContextMenuTextArray(kContextMenuID_JumpListHistory,mJumpListHistory,mJumpListHistory_Path);
}

#pragma mark ===========================

#pragma mark --- �����p�����[�^
//#693

/**
�����p�����[�^�擾
*/
void	AApp::SPI_GetFindParam( AFindParameter& outParam ) const
{
	outParam.Set(mFindParam);
}

/**
�����p�����[�^�ݒ�
*/
void	AApp::SPI_SetFindParam( const AFindParameter& inFindParam )
{
	//�����p�����[�^�ݒ�
	mFindParam.Set(inFindParam);
	//���������񒷐���
	mFindParam.findText.LimitLength(0,kLimit_FindTextLength);
	//�E�C���h�E�֔��f
	if( mFindWindowID != kObjectID_Invalid )
	{
		SPI_GetFindWindow().SPI_UpdateUI_CurrentFindParam();
	}
}

/**
�����p�����[�^�i����������̂݁j�ݒ�
*/
void	AApp::SPI_SetFindText( const AText& inText )
{
	//#928
	//�R�}���h�{E���ɐ��K�\���������邱�Ƃ͂Ȃ��͂��Ȃ̂ŁA���K�\����������
	mFindParam.regExp = false;
	//����������ݒ�
	mFindParam.findText.SetText(inText);
	//���������񒷐���
	mFindParam.findText.LimitLength(0,kLimit_FindTextLength);
	//�E�C���h�E�֔��f
	if( mFindWindowID != kObjectID_Invalid )
	{
		SPI_GetFindWindow().SPI_UpdateUI_CurrentFindParam();
	}
}

/**
�����p�����[�^�i�u��������̂݁j�ݒ�
*/
void	AApp::SPI_SetReplaceText( const AText& inText )
{
	//�u��������ݒ�
	mFindParam.replaceText.SetText(inText);
	//�E�C���h�E�֔��f
	if( mFindWindowID != kObjectID_Invalid )
	{
		SPI_GetFindWindow().SPI_UpdateUI_CurrentFindParam();
	}
}

/**
�����p�����[�^������
*/
void	AApp::InitFindParam()
{
	mFindParam.ignoreCase		= GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreCase);
	mFindParam.wholeWord		= GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultWholeWord);
	mFindParam.ignoreSpaces		= GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreSpaces);
	mFindParam.ignoreBackslashYen = GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreBackslashYen);
	mFindParam.regExp			= GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultRegExp);
	mFindParam.loop				= GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultLoop);
	mFindParam.fuzzy			= GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultAimai);
	//
	mFindParam.recursive		=  GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultRecursive);
	mFindParam.displayPosition	=  GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultDisplayPosition);
	mFindParam.onlyVisibleFile	=  GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultOnlyVisibleFile);
	mFindParam.onlyTextFile		=  GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultOnlyTextFile);
}

/**
�w��find parameter�ɑΉ����錟���c�[���R�}���h���擾����
*/
void	AApp::SPI_GetFindParamToolCommand( const AFindParameter& inParam, AText& outText, const char* inCommand ) const
{
	//���݂̌����p�����[�^���c�[���R�}���h�����I�v�V�����ɕϊ�����
	AText	optiontext;
	GetToolCommandFindOptionFromFindParameter(inParam,false,optiontext);
	
	//�c�[���R�}���h����
	
	//�R�}���h�s
	outText.SetCstring("<<<");
	outText.AddCstring(inCommand);
	outText.AddCstring("(");
	outText.AddText(optiontext);
	outText.AddCstring(")\r");
	
	//����������̍s
	AText	findtext;
	findtext.SetText(inParam.findText);
	//�����e�L�X�g�́A���s�R�[�h�ƃo�b�N�X���b�V�����G�X�P�[�v����B
	findtext.ConvertToEscapedText(true,false);
	//����������ǉ�
	outText.AddText(findtext);
	
	//�ŏI�s
	outText.AddCstring("\r>>>");
}

/**
�w��find parameter�ɑΉ�����u���c�[���R�}���h���擾����
*/
void	AApp::SPI_GetReplaceParamToolCommand( const AFindParameter& inParam, const AReplaceRangeType inReplaceRange, AText& outText ) const
{
	//���݂̌����p�����[�^���c�[���R�}���h�����I�v�V�����ɕϊ�����
	AText	optiontext;
	AApp::GetToolCommandFindOptionFromFindParameter(inParam,false,optiontext);
	
	//�c�[���R�}���h����
	
	//�R�}���h�s
	switch(inReplaceRange)
	{
	  case kReplaceRangeType_Next:
		{
			outText.SetCstring("<<<REPLACE-TEXT-NEXT(");
			break;
		}
	  case kReplaceRangeType_AfterCaret:
		{
			outText.SetCstring("<<<REPLACE-TEXT-AFTERCARET(");
			break;
		}
	  case kReplaceRangeType_InSelection:
		{
			outText.SetCstring("<<<REPLACE-TEXT-INSELECTION(");
			break;
		}
	  case kReplaceRangeType_All:
		{
			outText.SetCstring("<<<REPLACE-TEXT-ALL(");
			break;
		}
	  case kReplaceRangeType_ReplaceSelectedTextOnly:
		{
			outText.SetCstring("<<<REPLACE-TEXT-SELECTEDIFMATCHED(");
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	outText.AddText(optiontext);
	outText.AddCstring(")\r");
	
	//����������̍s
	AText	findtext;
	findtext.SetText(inParam.findText);
	//�����e�L�X�g�́A���s�R�[�h�ƃo�b�N�X���b�V�����G�X�P�[�v����B
	findtext.ConvertToEscapedText(true,false);
	//����������ǉ�
	outText.AddText(findtext);
	outText.AddCstring("\r");
	
	//�u��������̍s
	AText	replacetext;
	replacetext.SetText(inParam.replaceText);
	//�����e�L�X�g�́A���s�R�[�h�ƃo�b�N�X���b�V�����G�X�P�[�v����B
	replacetext.ConvertToEscapedText(true,false);
	//�u��������ǉ�
	outText.AddText(replacetext);
	
	//�ŏI�s
	outText.AddCstring("\r>>>");
}

//#887
/**
FindParameter����A�c�[���R�}���h�̌����I�v�V�����p�e�L�X�g�𐶐�����
@note static
*/
void	AApp::GetToolCommandFindOptionFromFindParameter( const AFindParameter& inParameter, const ABool inMultiFileFind, AText& outText )
{
	if( inParameter.onlyVisibleFile == true )
	{
		outText.AddCstring("ONLYVISIBLEFILE ");
	}
	if( inParameter.onlyTextFile == true )
	{
		outText.AddCstring("ONLYTEXTFILE ");
	}
	if( inParameter.ignoreCase == false )
	{
		outText.AddCstring("CASESENSITIVE ");
	}
	if( inParameter.wholeWord == true )
	{
		outText.AddCstring("WHOLEWORD ");
	}
	if( inParameter.fuzzy == false )
	{
		outText.AddCstring("NOFUZZY ");
	}
	if( inParameter.loop == true )
	{
		outText.AddCstring("LOOP ");
	}
	if( inParameter.ignoreBackslashYen == false )
	{
		outText.AddCstring("BACKSLASHYENSENSITIVE ");
	}
	if( inParameter.ignoreSpaces == false )
	{
		outText.AddCstring("SPACESSENSITIVE ");
	}
	if( inParameter.regExp == true )
	{
		outText.AddCstring("REGEXP ");
	}
	if( inMultiFileFind == true )
	{
		if( inParameter.recursive == false )
		{
			outText.AddCstring("NORECURSIVE ");
		}
		if( inParameter.displayPosition == false )
		{
			outText.AddCstring("NODISPLAYPOSITION ");
		}
		if( inParameter.filterText.Compare(".+") == false )
		{
			AText	filtertext;
			filtertext.SetText(inParameter.filterText);
			//
			filtertext.ConvertToEscapedText(false,true);
			//
			outText.AddCstring("FILTER\"");
			outText.AddText(filtertext);
			outText.AddCstring("\" ");
		}
	}
}

/**
�ŋߎg��������������ɒǉ�
*/
void	AApp::SPI_AddRecentlyUsedFindText( const AFindParameter& inParam )
{
	SPI_GetFindWindow().SPI_AddRecentlyUsedFindText(inParam);
}

/**
�ŋߎg�����u��������ɒǉ�
*/
void	AApp::SPI_AddRecentlyUsedReplaceText( const AFindParameter& inParam )
{
	SPI_GetFindWindow().SPI_AddRecentlyUsedReplaceText(inParam);
}

#pragma mark ===========================

#pragma mark --- �����[�g�ڑ�
//#361

/**
�����[�g�ڑ��I�u�W�F�N�g����
pparam inType �^�C�v�F�t�@�C�����X�g�����LIST�v�� or �t�@�C���I�[�v���v��
@param inRequestorObjectID �t�@�C�����X�g�����LIST�v���̏ꍇ�F�t�@�C�����X�g�E�C���h�Eobject�@�t�@�C���I�[�v���v���Finvalid
*/
AObjectID	AApp::SPI_CreateRemoteConnection( const ARemoteConnectionType inType, const AObjectID inRequestorObjectID )
{
	ARemoteConnectionFactory	factory(inType,inRequestorObjectID);
	AIndex	index = mRemoteConnectionArray.AddNewObject(factory);
	return mRemoteConnectionArray.GetObjectConst(index).GetObjectID();
}

/**
�����[�g�ڑ��I�u�W�F�N�g�폜
*/
void	AApp::SPI_DeleteRemoteConnection( const AObjectID inRemoteConnectionObjectID )
{
	AIndex	index = mRemoteConnectionArray.GetIndexByID(inRemoteConnectionObjectID);
	mRemoteConnectionArray.Delete1Object(index);
}

/**
�����[�g�t�@�C��URL���J���Ă���h�L�������g������
*/
AObjectID	AApp::SPI_GetTextDocumentIDByRemoteFileURL( const AText& inRemoteFileURL ) const
{
	if( inRemoteFileURL.GetItemCount() == 0 )   return kObjectID_Invalid;
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		AText	remoteFileURL;
		SPI_GetTextDocumentConstByID(docID).SPI_GetRemoteFileURL(remoteFileURL);
		if( inRemoteFileURL.Compare(remoteFileURL) == true )
		{
			return docID;
		}
	}
	return kObjectID_Invalid;
}

//#1228
/**
URL���烊���[�g�ڑ��I�u�W�F�N�g����������i�t�@�C���I�[�v������TextDocument���������Ȃ̂�SPI_GetTextDocumentIDByRemoteFileURL()�Ō�����Ȃ��̂ŁA������Ō�������K�v������B�j
*/
AObjectID	AApp::SPI_FindRemoteConnectionByRemoteURL( const AText& inRemoteURL ) const
{
	for( AIndex index = 0; index < mRemoteConnectionArray.GetItemCount(); index++ )
	{
		if( inRemoteURL.Compare(mRemoteConnectionArray.GetObjectConst(index).GetRemoteFilePath()) == true )
		{
			return mRemoteConnectionArray.GetObjectConst(index).GetObjectID();
		}
	}
	return kObjectID_Invalid;
}

//#994
/**
URL����g�p����A�N�Z�X�v���O�C������������
*/
/*#1231
AIndex	AApp::FindAccessPluginIndexFromURL( const AText& inURL )
{
	//�A�J�E���g���Ƃ̃��[�v
	AItemCount	itemCount = GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kFTPAccount_ConnectionType);
	for( AIndex index = 0; index < itemCount; index++ )
	{
		//�R�l�N�V�����^�C�v����v���g�R���擾
		AText	connectionType;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_ConnectionType,index,connectionType);
		AIndex	foundpos = 0;
		if( connectionType.FindCstring(0,"(",foundpos) == true )
		{
			connectionType.DeleteAfter(foundpos);
		}
		connectionType.ChangeCaseLower();
		//�A�J�E���g�́A�T�[�o�[�A���[�U�[�A�p�X�f�[�^�擾
		AText	server, user, path;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_Server,index,server);
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_User,index,user);
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_RootPath,index,path);
		//URL�擾
		AText	url;
		url.SetFTPURL(connectionType,server,user,path,0);
		//�v���g�R���������r���āA�����Ȃ�index��Ԃ�
		if( inURL.CompareMin(url) == true )
		{
			return index;
		}
	}
	return kIndex_Invalid;
}
*/

/**
�����[�g�ڑ��փt�@�C���I�[�v���v���i�����[�g�ڑ��I�u�W�F�N�g�o�R�ŃX���b�h��GET�v���j
*/
void	AApp::SPI_RequestOpenFileToRemoteConnection( const AText& inRemoteFileURL, const ABool inAllowNoFile )
{
	/* #1231
	AText	connectionType;
	//URL����Ή�����R�l�N�V�����^�C�v���擾
	AIndex	index = FindAccessPluginIndexFromURL(inRemoteFileURL);
	if( index != kIndex_Invalid )
	{
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_ConnectionType,index,connectionType);
	}
	//�����[�g�t�@�C�����J��
	SPI_RequestOpenFileToRemoteConnection(connectionType,inRemoteFileURL,inAllowNoFile);
}
void	AApp::SPI_RequestOpenFileToRemoteConnection( const AText& inConnectionType, const AText& inRemoteFileURL, const ABool inAllowNoFile )
{
*/
	//�����[�g�t�@�C��URL�ɑΉ�����h�L�������g������
	ADocumentID	docID = SPI_GetTextDocumentIDByRemoteFileURL(inRemoteFileURL);
	if( docID != kObjectID_Invalid )
	{
		//�����[�g�t�@�C�����h�L�������g�ɊJ���Ă���΁A���̃h�L�������g��reveal����
		SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();
	}
	else
	{
		//#1228
		//URL���烊���[�g�ڑ��I�u�W�F�N�g���������āA���݂��Ă���Ή��������I���B
		//�t�@�C���I�[�v������TextDocument���������Ȃ̂�SPI_GetTextDocumentIDByRemoteFileURL()�Ō�����Ȃ��̂ŁB�j
		if( SPI_FindRemoteConnectionByRemoteURL(inRemoteFileURL) != kObjectID_Invalid )
		{
			return;
		}
		
		//�����[�g�ڑ��I�u�W�F�N�g����
		AObjectID	remoteConnectionObjectID = SPI_CreateRemoteConnection(kRemoteConnectionType_TextDocument,kObjectID_Invalid);
		//�ڑ��AGET�v��
		//#1231 SPI_GetRemoteConnection(remoteConnectionObjectID).SetConnectionType(inConnectionType);
		SPI_GetRemoteConnection(remoteConnectionObjectID).ExecuteGET(kRemoteConnectionRequestType_GETforOpenDocument,
																	 inRemoteFileURL,inAllowNoFile);
	}
}

/**
�����[�g�ڑ��I�u�W�F�N�g����̉�����M������
*/
void	AApp::SPI_DoAccessPluginResponseReceived( const AObjectID inRemoteConnectionObjectID, const AText& inText, const AText& inStderrText )
{
	if( mRemoteConnectionArray.ExistObject(inRemoteConnectionObjectID) == true )
	{
		SPI_GetRemoteConnection(inRemoteConnectionObjectID).DoResponseReceived(inText,inStderrText);//#1231
	}
}

/**
�����[�g�t�@�C���h�L�������g�����iGET������M���ɁA�����[�g�ڑ��I�u�W�F�N�g����R�[�������j
*/
void	AApp::SPNVI_CreateDocumentFromRemoteFile( const AObjectID inRemoteConnectionObjectID, 
		const AText& inRemoteFileURL, const AText& inDocumentText )
{
#if IMPLEMENTATION_FOR_WINDOWS
	if( CAppImp::SystemResourceCheck_GDIObject() == false )   return;
#endif
	
	//DocImp���� #1034
	AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
	//�h�L�������g�����i�����[�g�ڑ��I�u�W�F�N�g���h�L�������g�ɓn���j
	ATextDocumentFactory	factory(this,docImpObjectID);//#1034
	AObjectID	docID = NVI_CreateDocument(factory);
	SPI_GetTextDocumentByID(docID).SPI_Init_RemoteFile(inRemoteConnectionObjectID,inRemoteFileURL,inDocumentText);
	//#379 �E�C���h�E����
	SPI_CreateWindowOrTabForNewTextDocument(docID,kIndex_Invalid,kIndex_Invalid,true);
	//�ŋߊJ�����t�@�C���ɒǉ�
	SPI_AddToRecentlyOpenedFile(inRemoteFileURL);//#235
}

/**
�����[�g�ڑ��p�v���O�C���p�X�擾�i"sftp"���̃^�C�v������A�v���O�C���̃t�@�C���p�X���擾�j
*/
ABool	AApp::SPI_GetPluginPath( const AText& inConnectionTypeName, AText& outPluginPath )
{
	outPluginPath.DeleteAll();
	if( inConnectionTypeName.GetItemCount() == 0 )
	{
		return true;//�i���K�V�[FTP�j
	}
	/*�����Ή�
	//�v���O�C���̃t�@�C���p�X���擾
	{
		AFileAcc	folder;
		//���[�U�[��`�A�N�Z�X�v���O�C���t�H���_
		SPI_GetUserAccessPluginFolder(folder);
		AFileAcc	pluginFolder;
		pluginFolder.SpecifyChild(folder,inConnectionTypeName);
		if( pluginFolder.Exist() == true && pluginFolder.IsFolder() == false )
		{
			pluginFolder.GetPath(outPluginPath);
			return true;
		}
	}
	*/
	//�A�v�����A�N�Z�X�v���O�C���t�H���_
	{
		AFileAcc	folder;
		SPI_GetAppAccessPluginFolder(folder);
		AFileAcc	pluginFolder;
		pluginFolder.SpecifyChild(folder,inConnectionTypeName);
		if( pluginFolder.Exist() == true && pluginFolder.IsFolder() == false )
		{
			pluginFolder.GetPath(outPluginPath);
			return true;
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark <���L�N���X(ADocument_Text)�����^�폜�^�擾>

#pragma mark ===========================

//�V�K�e�L�X�g�h�L�������g����
AObjectID	AApp::SPNVI_CreateNewTextDocument( const AModeIndex inModeIndex, const AText& inTextEncodingName, 
			const AText& inDocumentText, const ABool inSelect,
			const AWindowID inCreateTabInWindowID, const ABool inAlwaysCreateWindow,
			const ARect inInitialWindowBounds )
{
#if IMPLEMENTATION_FOR_WINDOWS
	if( CAppImp::SystemResourceCheck_GDIObject() == false )   return kObjectID_Invalid;
#endif
	
	//DocImp���� #1034
	AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
	//�h�L�������g����
	ATextDocumentFactory	factory(this,docImpObjectID);//#1034
	ADocumentID	docID = NVI_CreateDocument(factory);
	SPI_GetTextDocumentByID(docID).SPI_Init_NewFile(inModeIndex,inTextEncodingName,inDocumentText);
	//#379 �E�C���h�E�����i�h�L�������g���ł̓E�C���h�E�E�^�u�͐������Ȃ��B�h�L�������g�͂����܂Ńf�[�^�݂̂Ƃ���B�j
	SPI_CreateWindowOrTabForNewTextDocument(docID,kIndex_Invalid,kIndex_Invalid,inSelect,true,inCreateTabInWindowID,inAlwaysCreateWindow,inInitialWindowBounds);
	return docID;
}

//#567 #1170
/**
LuaConsole�h�L�������g����
*/
AObjectID	AApp::SPNVI_CreateLuaConsoleDocument()
{
#if IMPLEMENTATION_FOR_WINDOWS
	if( CAppImp::SystemResourceCheck_GDIObject() == false )   return kObjectID_Invalid;
#endif
	
    //DocImp���� #1034
    AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
    //
	ATextDocumentFactory	factory(this,docImpObjectID);
	ADocumentID	docID = NVI_CreateDocument(factory);
	AText	utf8("UTF-8");
	SPI_GetTextDocumentByID(docID).SPI_Init_NewFile(kStandardModeIndex,utf8,GetEmptyText());
	AText	title("Lua Console");
	SPI_GetTextDocumentByID(docID).SPI_SetTemporaryTitle(title);
	SPI_GetTextDocumentByID(docID).NVI_SetAlwaysNotDirty();
	SPI_GetTextDocumentByID(docID).SPI_SetLuaConsoleMode();
	//#379 �E�C���h�E�����i�h�L�������g���ł̓E�C���h�E�E�^�u�͐������Ȃ��B�h�L�������g�͂����܂Ńf�[�^�݂̂Ƃ���B�j
	SPI_CreateWindowOrTabForNewTextDocument(docID,kIndex_Invalid,kIndex_Invalid,false,true,kObjectID_Invalid,true);
	SPI_GetTextDocumentByID(docID).SPI_Lua_Prompt();
	return docID;
}

//#379
/**
�ǂݍ��ݐ�p�h�L�������g�����iDiff�h�L�������g�ȂǗp�j
*/
ADocumentID	AApp::SPNVI_CreateDiffTargetDocument( const AText& inTitle, const AText& inDocumentText,
			const AModeIndex inModeIndex, const AText& inTextEncodingName,
			const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd, 
			const ABool inSelect, const ABool inSelectableTab,
			const AWindowID inCreateTabInWindowID )
{
#if IMPLEMENTATION_FOR_WINDOWS
	if( CAppImp::SystemResourceCheck_GDIObject() == false )   return kObjectID_Invalid;
#endif
	
	//DocImp���� #1034
	AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
	//�h�L�������g����
	ATextDocumentFactory	factory(this,docImpObjectID);//#1034
	ADocumentID	docID = NVI_CreateDocument(factory);
	SPI_GetTextDocumentByID(docID).SPI_Init_DiffTargetDocument(inTitle,inDocumentText,inModeIndex,inTextEncodingName);
	//#379 �E�C���h�E�����i�h�L�������g���ł̓E�C���h�E�E�^�u�͐������Ȃ��B�h�L�������g�͂����܂Ńf�[�^�݂̂Ƃ���B�j
	SPI_CreateWindowOrTabForNewTextDocument(docID,inInitialSelectStart,inInitialSelectEnd,inSelect,inSelectableTab,inCreateTabInWindowID);
	return docID;
}
ADocumentID	AApp::SPNVI_CreateDiffTargetDocument( const AText& inTitle, const AFileAcc& inFile,
												 const AModeIndex inModeIndex, const AText& inTextEncodingName,
												 const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd, 
												 const ABool inSelect, const ABool inSelectableTab,
												 const AWindowID inCreateTabInWindowID )
{
#if IMPLEMENTATION_FOR_WINDOWS
	if( CAppImp::SystemResourceCheck_GDIObject() == false )   return kObjectID_Invalid;
#endif
	
	//DocImp���� #1034
	AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
	//�h�L�������g����
	ATextDocumentFactory	factory(this,docImpObjectID);//#1034
	ADocumentID	docID = NVI_CreateDocument(factory);
	SPI_GetTextDocumentByID(docID).SPI_Init_DiffTargetDocument(inTitle,inFile,inModeIndex,inTextEncodingName);
	//#379 �E�C���h�E�����i�h�L�������g���ł̓E�C���h�E�E�^�u�͐������Ȃ��B�h�L�������g�͂����܂Ńf�[�^�݂̂Ƃ���B�j
	SPI_CreateWindowOrTabForNewTextDocument(docID,inInitialSelectStart,inInitialSelectEnd,inSelect,inSelectableTab,inCreateTabInWindowID);
	return docID;
}

//#856
/**
���ɐ����ς݂̃h�L�������g�ɑ΂��A�V���ȃE�C���h�E�𐶐�
*/
void	AApp::SPI_CreateNewWindowForExistingTextDocument( const ADocumentID inTextDocumentID,
			const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd )
{
	SPI_CreateWindowOrTabForNewTextDocument(inTextDocumentID,inInitialSelectStart,inInitialSelectEnd,true,
							true,kObjectID_Invalid,true);
}

//���[�J���t�@�C������h�L�������g����
AObjectID	AApp::SPNVI_CreateDocumentFromLocalFile( const AFileAcc& inFile, const AText& inTextEncodingName, 
													 const AObjectID inDocImpID,//#1078
													 const ABool inJudgeBigFile /*B0383, const ABool inTemplate*/,
			const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd, const ABool inSelect, //win 080715
			const AWindowID inCreateTabInWindowID, const ABool inAlwaysCreateWindow,
			const ARect inInitialWindowBounds,
			const ABool inSetInitialSideBarDisplay, const ABool inLeftSideBar, const ABool inRightSideBar, const ABool inSubText )
{
#if IMPLEMENTATION_FOR_WINDOWS
	if( CAppImp::SystemResourceCheck_GDIObject() == false )   return kObjectID_Invalid;
#endif
	
	AObjectID	result = kObjectID_Invalid;
	AFileAcc	file;
	file = inFile;
	/*#932 �G�C���A�X�����A�t�@�C�����݃`�F�b�N�A�t�H���_�`�F�b�N�́A�h�L�������g����������ɃX���b�h�ōs��
	file.ResolveAnyLink(inFile);
	if( file.Exist() == false )   return result;
	if( file.IsFolder() == true )   return result;//win
	*/
	switch(GetDocumentTypeForFile(file))
	{
	  case kDocumentType_Text:
		{
			//win
			ADocumentID	firstDocID = NVI_GetFirstDocumentID(kDocumentType_Text);;
			
			//�^�u���[�h�ł͂Ȃ��ꍇ�͐�Ɏ����V�K�����h�L�������g����� #1056
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == false )
			{
				CloseAutomaticallyCreatedDocument(firstDocID);
			}
			
			AObjectID	docID = NVI_GetDocumentIDByFile(kDocumentType_Text,file);
			if( docID == kObjectID_Invalid )
			{
				/*#932 �t�@�C���T�C�Y�`�F�b�N�́A�h�L�������g����������ɃX���b�h�ōs���B
				//�t�@�C���T�C�Y��100MB�𒴂���t�@�C���̏ꍇ�́A���[�_���G���[�\�����āA�t�@�C�����J���Ȃ�
				AByteCount	filesize = file.GetFileSize();
				if( filesize > kLimit_OpenTextFileSize )
				{
					AText	message1, message2;
					message1.SetLocalizedText("CantOpen_Title");
					message2.SetLocalizedText("CantOpen_FileSize");
					NVI_ShowModalAlertWindow(message1,message2);
					break;
				}
				//�g�[�^���̃t�@�C���T�C�Y��200MB�𒴂���ꍇ�́A���[�_���G���[�\�����āA�t�@�C�����J���Ȃ�
				if( SPI_GetTotalTextDocumentMemoryByteCount() + filesize > kLimit_TotalOpenTextFileSize )
				{
					AText	message1, message2;
					message1.SetLocalizedText("CantOpen_Title");
					message2.SetLocalizedText("CantOpen_TotalFileSize");
					NVI_ShowModalAlertWindow(message1,message2);
					break;
				}
				*/
				/*#699
				//R0208
				if( false && file.GetFileSize() >= 1024*1024 && inJudgeBigFile == true && 
							GetAppPref().GetData_Bool(AAppPrefDB::kOpenAlertWhenBigFile) == true )
				{
					SPI_GetBigFileAlertWindow().NVI_Create(kObjectID_Invalid);//B0419
					SPI_GetBigFileAlertWindow().SPI_SetFile(file);
					SPI_GetBigFileAlertWindow().NVI_Show();//B0419
					break;
				}
				*/
				//DocImp���� #1034
				AObjectID	docImpObjectID = inDocImpID;
				if( docImpObjectID == kObjectID_Invalid )
				{
					docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
				}
				//���[�J���t�@�C������e�L�X�g�h�L�������g����
				ATextDocumentFactory	factory(this,docImpObjectID);//#1034
				docID = NVI_CreateDocument(factory);
				SPI_GetTextDocumentByID(docID).SPI_Init_LocalFile(file,inTextEncodingName);//win 080715 #212
				//#379 �E�C���h�E����
				SPI_CreateWindowOrTabForNewTextDocument(docID,inInitialSelectStart,inInitialSelectEnd,inSelect,true,
							inCreateTabInWindowID,inAlwaysCreateWindow,
							inInitialWindowBounds,inSetInitialSideBarDisplay,inLeftSideBar,inRightSideBar,inSubText);
				//�ŋߊJ�����t�@�C���֒ǉ�
				SPI_AddToRecentlyOpenedFile(inFile);
				//R0006
				//#932 �����t�H���_�̃f�[�^�X�V���i�����t�H���_���[�_�X���b�h�I����j�ɓo�^����̂ł����͕s�v SCMRegisterFolderByFile(inFile);
			}
			else
			{
				//�t�@�C���ɑΉ�����h�L�������g�����ɑ��݂��Ă���ꍇ�͂��̃E�C���h�E��I������B
				SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();//#92 SPI_SelectFirstWindow();
			}
			mLastOpenedTextFile.CopyFrom(inFile);
			result = docID;
			
			//�^�u���[�h�̏ꍇ�͌�Ŏ����V�K�����h�L�������g�����
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true )
			{
				CloseAutomaticallyCreatedDocument(firstDocID);
			}
			break;
		}
	  case kDocumentType_IndexWindow:
		{
			AObjectID	docID = NVI_GetDocumentIDByFile(kDocumentType_IndexWindow,file);
			if( docID == kObjectID_Invalid )
			{
				/*#725
				AIndexWindowDocumentFactory	docfactory(this);
				docID = NVI_CreateDocument(docfactory);
				//�t�@�C���ǂݍ���
				SPI_GetIndexWindowDocumentByID(docID).Load(file);//#92 Init(file);
				
				//Window���� #92 Document���ł�Window�𐶐����Ȃ��悤�ɕύX
				AIndexWindowFactory	winfactory(docID);
				AWindowID	winID = GetApp().NVI_CreateWindow(winfactory);
				NVI_GetWindowByID(winID).NVI_Create(docID);
				NVI_GetWindowByID(winID).NVI_Show();
				
				//�E�C���h�E���j���[�X�V
				SPI_UpdateWindowMenu();
				*/
				ADocumentID	docID = SPI_GetOrCreateFindResultWindowDocument();
				//�t�@�C���ǂݍ���
				SPI_GetIndexWindowDocumentByID(docID).Load(file);//#92 Init(file);
			}
			else
			{
				SPI_GetIndexWindowDocumentByID(docID).NVI_RevealDocumentWithAView();//#92 SPI_SelectFirstWindow();
			}
			SPI_AddToRecentlyOpenedFile(inFile);
			result = docID;
			break;
		}
	  case kDocumentType_FTPAlias:
		{
			AText	text;
			file.ReadTo(text);
			SPI_RequestOpenFileToRemoteConnection(text,false);//#361
			break;
		}
		/*#858 ���[�h�ݒ�t�@�C���_�u���N���b�N�ł̃I�[�v���͖��Ή��ɂ���
	  case kDocumentType_ModePref:
		{
			if( SPI_GetModePrefWindow().NVI_IsWindowVisible() == false )
			{
				//B0303 mModePrefWindow.NVI_Create(kObjectID_Invalid);
				//B0303 mModePrefWindow.SPNVI_Show(kStandardModeIndex);
				SPI_GetModePrefWindow().NVI_CreateAndShow();//#402 Show->CreateAndShow
				SPI_GetModePrefWindow().NVI_SwitchFocusToFirst();
			}
			else
			{
				SPI_GetModePrefWindow().NVI_SelectWindow();
			}
			SPI_GetModePrefWindow().SPI_ShowAddNewModeImportWindow(file);
			break;
		}
		*/
		//#1050
		//�^�u�Z�b�g
	  case kDocumentType_TabSet:
		{
			OpenTabSet(file);
			//�^�u�Z�b�g���̂̓h�L�������g�ł͂Ȃ��̂ŁACDocImp�I�u�W�F�N�g������΁A�폜����B #1078
			if( inDocImpID != kObjectID_Invalid )
			{
				AApplication::GetApplication().GetImp().DeleteDocImp(inDocImpID);
			}
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

/*B0389 
void	AApp::SPNVI_CreateDocumentByPathForDisplay( const AText& inFilePath )
{
	AFileAcc	fileacc;
	fileacc.Specify(inFilePath,kFilePathType_1);
	SPNVI_CreateDocumentFromLocalFile(fileacc);
}
*/
/*#361
void	AApp::SPNVI_CreateTextDocumentFTP( const AText& inFTPURL, const AText& inDocumentText )
{
#if IMPLEMENTATION_FOR_WINDOWS
	if( CAppImp::SystemResourceCheck_GDIObject() == false )   return;
#endif
	
	ATextDocumentFactory	factory(this);
	AObjectID	docID = NVI_CreateDocument(factory);
	SPI_GetTextDocumentByID(docID).SPI_Init_FTP(inFTPURL,inDocumentText);
	//#379 �E�C���h�E����
	SPI_CreateWindowOrTabForNewTextDocument(docID,kIndex_Invalid,kIndex_Invalid,true);
	//
	SPI_AddToRecentlyOpenedFile(inFTPURL);//#235
}
*/
void	AApp::SPNVI_CreateDocumentFromTemplateFile( const AFileAcc& inFile )
{
	//B0383 SPNVI_CreateDocumentFromLocalFile(inFile,GetEmptyText(),true);
	AText	text;
	AIndex	modeIndex = kStandardModeIndex;
	AText	tecname;
	SPI_LoadTextFromFile(kLoadTextPurposeType_CreateDocumentFromTemplateFile,inFile,text,modeIndex,tecname);//#723 �h�L�������g����e�L�X�g�擾����K�v�������̂�SPI_GetTextFromFileOrDocument()����ύX
	AObjectID	docID = SPNVI_CreateNewTextDocument(modeIndex,tecname);
	if( docID == kObjectID_Invalid )   return;//win
	SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
	SPI_GetTextDocumentByID(docID).NVI_SetDirty(false);
}

//#1034
/**
DocImp����
*/
AObjectID	AApp::SPI_CreateDocImp( const ADocumentType inDocType )
{
	AObjectID	docImpObjectID = kObjectID_Invalid;
	switch(inDocType)
	{
	  case kDocumentType_Text:
		{
			docImpObjectID = GetImp().CreateDocImp();
			TextDocument*	doc = [[TextDocument alloc] init];
			GetImp().GetDocImpByID(docImpObjectID).SetNSDocument(doc);
			break;
		}
	  case kDocumentType_IndexWindow:
		{
			docImpObjectID = GetImp().CreateDocImp();
			IndexWindowDocument*	doc = [[IndexWindowDocument alloc] init];
			GetImp().GetDocImpByID(docImpObjectID).SetNSDocument(doc);
			break;
		}
	}
	return docImpObjectID;
}

//#725
/**
�h�L�������g�̎w��ʒu���J���A�܂��́Areveal����
*/
ADocumentID	AApp::SPI_OpenOrRevealTextDocument( const AText& inFilePath, const AIndex inSpos, const AIndex inEpos, 
			const AWindowID inTextWindowID, const ABool inInAnotherView,
			const AAdjustScrollType inAdjustScrollType )
{
	//�t�@�C���p�X����̂Ƃ��͉������Ȃ�
	if( inFilePath.GetItemCount() == 0 )
	{
		return kObjectID_Invalid;
	}
	//http://�Ŏn�܂鎞�̓u���E�U��URL���J��
	if( inFilePath.CompareMin("http://") == true )
	{
		ALaunchWrapper::OpenURLWithDefaultBrowser(inFilePath);
		return kObjectID_Invalid;
	}
	
	//------------------�܂������[�g�t�@�C�����猟��------------------
	ADocumentID	docID = SPI_GetTextDocumentIDByRemoteFileURL(inFilePath);
	if( docID == kObjectID_Invalid )
	{
		//------------------���[�J���t�@�C�����猟��------------------
		//�t�@�C���擾
		AFileAcc	file;
		file.Specify(inFilePath);
		//�t�@�C������h�L�������g���擾
		docID = NVI_GetDocumentIDByFile(kDocumentType_Text,file);
		//------------------�����݂Ȃ�A�h�L�������g�𐶐�------------------
		//�h�L�������g�������Ȃ�A�h�L�������g�𐶐��B�i�^�u�𐶐�����e�L�X�g�E�C���h�E���w��j
		if( docID == kObjectID_Invalid )
		{
			docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,GetEmptyText(),kObjectID_Invalid,true,inSpos,inEpos,false,inTextWindowID);//#1090�̂��� inSelect=true�֕ύX ��false�ɖ߂��B����SPI_RevealTextViewByDocumentID()�ɂ��Areveal�����Btrue���ƁA�R�}���h�{info�N���b�N�ŁA�T�u�e�L�X�g�ƃ��C���e�L�X�g�����ɕ\������Ă��܂�
		}
	}
	//------------------�h�L�������g��̊Y���ӏ���\��------------------
	if( docID != kObjectID_Invalid )
	{
		//�\����e�L�X�g�E�C���h�E�擾�i���w��Ȃ�h�L�������g�̍ŏ��̃E�C���h�E�ɂ���j
		AWindowID	textWindowID = inTextWindowID;
		if( textWindowID == kObjectID_Invalid )
		{
			textWindowID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
		}
		//�e�L�X�g�E�C���h�E��ɕ\��
		GetApp().SPI_GetTextWindowByID(textWindowID).SPI_RevealTextViewByDocumentID(docID,inSpos,inEpos,inInAnotherView,inAdjustScrollType);
	}
	/*#�V�K�E�C���h�E�ŊJ���@�\��drop
	else
	{
		//�E�C���h�E����ɐ�������ꍇ
		
		//�h�L�������g�����ς݂Ȃ�A�����ς݃h�L�������g�ɑ΂��V�K�E�C���h�E����
		if( docID != kObjectID_Invalid )
		{
			SPI_CreateNewWindowForExistingTextDocument(docID,inSpos,inEpos);
		}
		//�h�L�������g�������Ȃ�A�h�L�������g��V�K�E�C���h�E�ɐ����B
		else
		{
			docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,GetEmptyText(),true,inSpos,inEpos,false,kObjectID_Invalid,true);
		}
	}
	*/
	return docID;
}

//#379
/**
�e�L�X�g�E�C���h�E�E�^�u�𐶐�
*/
AObjectID	AApp::SPI_CreateWindowOrTabForNewTextDocument( const ADocumentID inDocumentID, 
			const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd,
			const ABool inSelect, const ABool inSelectableTab,
			const AWindowID inCreateTabInWindowID, 
			const ABool inAlwaysCreateWindow, //#567
			const ARect inInitialWindowBounds,
			const ABool inSetInitialSideBarDisplay, const ABool inLeftSideBar, const ABool inRightSideBar, const ABool inSubText )
{
	////==============�E�C���h�E���� or �^�u�����̔���==============
	AWindowID	winID = kObjectID_Invalid;
	//#850 ABool	tabMode = NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow);
	ABool	createWindow = true;
	AWindowID	frontTextWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);//#850
	if( NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true )
	{
		//�^�u�����̏ꍇ�A���A�őO�ʃe�L�X�g�E�C���h�E�����݂��Ă���ꍇ�A�E�C���h�E�𐶐������^�u�𐶐�����
		//#850 if( SPI_GetTabModeMainTextWindowID() != kObjectID_Invalid )
		if( frontTextWindowID != kObjectID_Invalid )//#850
		{ 
			createWindow = false;
		}
	}
	//
	if( (createWindow == true && inCreateTabInWindowID == kObjectID_Invalid) || 
				inAlwaysCreateWindow == true )//����inAlwaysCreateWindow��true�Ȃ��L�̌��ʂɊւ�炸�E�C���h�E�������� #567
	{
		//==============�E�C���h�E�����̏ꍇ==============
		AWindowDefaultFactory<AWindow_Text>	factory;
		winID = NVI_CreateWindow(factory);
		//�E�C���h�E�E�^�u��������UpdateViewBounds()��}������ #1090
		SPI_GetTextWindowByID(winID).SPI_IncrementSuppressUpdateViewBoundsCounter();
		/*#850
		if( tabMode == true )
		{
			SPI_GetTextWindowByID(winID).SPI_SetTabModeMainWindow(true);
		}
		*/
		
		//#850
		//inInitialWindowBounds����łȂ���΁A���������E�C���h�E�̏����E�C���h�Ebounds�Ƃ��Đݒ�
		if( inInitialWindowBounds.bottom - inInitialWindowBounds.top > 0 && inInitialWindowBounds.right - inInitialWindowBounds.left > 0 )
		{
			NVI_GetWindowByID(winID).NVI_SetInitialWindowBounds(inInitialWindowBounds);
		}
		//�T�C�h�o�[�^�T�u�e�L�X�g�̏�����Ԑݒ�
		if( inSetInitialSideBarDisplay == true )
		{
			SPI_GetTextWindowByID(winID).SPI_SetInitialSideBarDisplay(inLeftSideBar,inRightSideBar,inSubText);
		}
		
		//�E�C���h�E����
		NVI_GetWindowByID(winID).NVI_Create(inDocumentID);
		//NVI_GetWindowByID(winID).NVI_SendBehind(kObjectID_Invalid);//#567
	}
	else
	{
		//==============�^�u�����̏ꍇ==============
		if( inCreateTabInWindowID == kObjectID_Invalid )
		{
			//�����ŁA�^�u������E�C���h�E�̎w�肪�Ȃ���΁A�őO�ʂ̃e�L�X�g�E�C���h�E�Ƀ^�u�𐶐�����
			winID = frontTextWindowID;//#850 SPI_GetTabModeMainTextWindowID();
		}
		else
		{
			//�����ŁA�^�u������E�C���h�E�̎w�肪����΁A���̃E�C���h�E���Ƀ^�u�𐶐�����B
			winID = inCreateTabInWindowID;
		}
		//�E�C���h�E�E�^�u��������UpdateViewBounds()��}������ #1090
		SPI_GetTextWindowByID(winID).SPI_IncrementSuppressUpdateViewBoundsCounter();
		//�^�u�����i��Q������false�ɂ���B���Ƃ�inSelect��true�Ȃ�NVI_SelectTabByDocumentID()���s���j
		SPI_GetTextWindowByID(winID).SPNVI_CreateTextDocumentTab(inDocumentID,false,inSelectableTab);//#212 #379
	}
	
	//�E�C���h�E���j���[�X�V
	//#922 SPI_UpdateWindowMenu();
	
	//�t�@�C�����X�g�E�C���h�E�X�V
	//#725 SPI_UpdateFileListWindow_DocumentOpened(inDocumentID);
	SPI_UpdateFileListWindows(kFileListUpdateType_DocumentOpened,inDocumentID);//#725
	
	//�ŏ��̑I��͈͐ݒ� win 080715
	if( inInitialSelectStart != kIndex_Invalid && inInitialSelectEnd != kIndex_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(winID).SPI_SetSelect(inDocumentID,inInitialSelectStart,inInitialSelectEnd);
	}
	//
	GetApp().SPI_GetTextWindowByID(winID).SPI_AdjustScroll_Center(inDocumentID);
	//B0000 �^�u���[�h�̏ꍇ�͊��ɃE�C���h�E�\���ς݂Ȃ̂�NVI_Show()�ŕ`�悳��Ȃ��B���̂��߁A��L�s�܂�Ԃ��v�Z�������𔽉f�����ĕ`�悪����Ȃ��B
	//�����������邽�߁A�E�C���h�E�\���ς݂̏ꍇ�̓E�C���h�E�ĕ`�悷��
	/*win ���̎��_�ł͍s�܂�Ԃ��v�Z�͖����s�ɂȂ����iTabActivate����SPI_DoViewActivating()�Ōv�Z�����)�̂ŃR�����g�A�E�g�B
	if( GetApp().NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
	{
		GetApp().NVI_GetWindowByID(winID).NVI_RefreshWindow();
	}*/
	
	//
	SPI_GetTextDocumentByID(inDocumentID).SPI_UpdateJumpList();//#291
	
	//�E�C���h�E�\��
	NVI_GetWindowByID(winID).NVI_Show(inSelect);//#567
	//win �ォ��ړ�
	if( inSelect == true )
	{
		if( NVI_GetWindowByID(winID).NVI_IsChildWindowVisible() == false )
		{
			NVI_GetWindowByID(winID).NVI_SelectTabByDocumentID(inDocumentID);
		}
	}
	//UpdateViewBounds()�̗}������ #1090
	SPI_GetTextWindowByID(winID).SPI_DecrementSuppressUpdateViewBoundsCounter();
	//
	SPI_GetTextWindowByID(winID).SPI_UpdateViewBounds();//#410
	//#725 ��Ŏ��s�ς݂Ȃ̂ō폜 SPI_GetTextDocumentByID(inDocumentID).SPI_UpdateJumpList();//#291
	
	//�e�L�X�g�G���R�[�f�B���O�C���E�C���h�E�\��
	ATextEncodingFallbackType	fallbackType = kTextEncodingFallbackType_None;//#473
	if( SPI_GetTextDocumentByID(inDocumentID).SPI_ShouldShowCorrectEncodingWindow(fallbackType) == true )//#473
	{
		SPI_GetTextWindowByID(winID).SPI_ShowTextEncodingCorrectWindow(false,fallbackType);//#473
	}
	return winID;
}

ADocumentType	AApp::GetDocumentTypeForFile( const AFileAcc& inFile )
{
	if( AKeyWrapper::IsOptionKeyPressed() == true )
	{
		return kDocumentType_Text;
	}
	/*#932 �^�C�v�ɂ��ǂݕ����͖����ɂ���B�C���f�b�N�X�E�C���h�E�h�L�������g���g���Ă��Ȃ����낤���B
#if IMPLEMENTATION_FOR_MACOSX
	OSType	type = AFileWrapper::GetType(inFile);
	if( type == 'ftpa' )   return kDocumentType_FTPAlias;
	if( type == 'INDX' )   return kDocumentType_IndexWindow;
	if( type == 'MODE' )   return kDocumentType_ModePref;
#endif
	*/
	//#1050
	//�t�@�C���g���q�擾
	AText	path;
	inFile.GetPath(path);
	AText	suffix;
	path.GetSuffix(suffix);
	//mitabset�Ȃ�^�u�Z�b�g
	if( suffix.Compare(".mitabset") == true )
	{
		return kDocumentType_TabSet;
	}
	//miremote�Ȃ烊���[�g�t�@�C���G�C���A�X #1073
	AFileAttribute	attribute = {0};//#1425
	inFile.GetFileAttribute(attribute);//#1425
	if( suffix.Compare(".miremote") == true || /*#1425 AFileWrapper::GetType(inFile)*/attribute.type == 'ftpa' )
	{
		return kDocumentType_FTPAlias;
	}
	//
	return kDocumentType_Text;
}

ADocument_Text&	AApp::SPI_GetTextDocumentByID( const AObjectID inDocID )
{
	MACRO_RETURN_DOCUMENT_DYNAMIC_CAST(ADocument_Text,kDocumentType_Text,inDocID);//#199
	/*#199 if( NVI_GetDocumentTypeByID(inDocID) != kDocumentType_Text )
	{
		_ACThrow("document type different",this);
	}
	return dynamic_cast<ADocument_Text&>(NVI_GetDocumentByID(inDocID));*/
}
const ADocument_Text&	AApp::SPI_GetTextDocumentConstByID( const AObjectID inDocID ) const
{
	MACRO_RETURN_CONSTDOCUMENT_DYNAMIC_CAST(ADocument_Text,kDocumentType_Text,inDocID);//#199
	/*#199 if( NVI_GetDocumentTypeByID(inDocID) != kDocumentType_Text )
	{
		_ACThrow("document type different",this);
	}
	return dynamic_cast<ADocument_Text&>(NVI_GetDocumentConstByID(inDocID));*/
}


/*#725 SPI_OpenOrRevealTextDocument()�֓���
void	AApp::SPI_SelectTextDocument( const AFileAcc& inFile, const ATextIndex inStartIndex, const AItemCount inLength )
{
	//win 080715 Create�����ꍇ�́A�ŏ��̑I��͈͂��w�肷��悤�ɐݒ�i�O��ۑ����̃L�����b�g�ʒu���\�������̌�A�����Ɉړ��ƂȂ��Ă��܂��̂�h���j
	AObjectID	docID = NVI_GetDocumentIDByFile(kDocumentType_Text,inFile);
	if( docID != kObjectID_Invalid )
	{
		SPI_GetTextDocumentByID(docID).SPI_SelectText(inStartIndex,inLength);
	}
	else
	{
		SPNVI_CreateDocumentFromLocalFile(inFile,GetEmptyText(),true,inStartIndex,inStartIndex+inLength);
	}
}
*/

//���[�h���b�v�̃h�L�������g�̍s�����Čv�Z
void	AApp::SPI_RecalcWordWrapAll( const AModeIndex inModeIndex )
{
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid  )
		{
			AWrapMode	wrapMode = SPI_GetTextDocumentByID(docID).SPI_GetWrapMode();
			if( wrapMode == kWrapMode_WordWrap || wrapMode == kWrapMode_WordWrapByLetterCount ||
				wrapMode == kWrapMode_LetterWrap || wrapMode == kWrapMode_LetterWrapByLetterCount )//#1113
			{
				SPI_GetTextDocumentByID(docID).SPI_DoWrapRelatedDataChanged(true);
			}
		}
	}
	GetApp().SPI_UpdateTextDrawPropertyAll(inModeIndex);
}

//�s�����Čv�Z
void	AApp::SPI_RecalcAll( const AModeIndex inModeIndex )
{
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			SPI_GetTextDocumentByID(docID).SPI_DoWrapRelatedDataChanged(true);
		}
	}
	//�s��񂪕ς��̂ŁA�C���[�W�T�C�Y�̔��f���K�v
	GetApp().SPI_UpdateTextDrawPropertyAll(inModeIndex);
}

//#349
/**
�e�h�L�������g�̃��[�h���Đݒ肷��
*/
void	AApp::SPI_ReSetModeAll( const AModeIndex inModeIndex )
{
	//#844
	//�܂��ŏ��ɂ��̃��[�h�̃h�L�������g�̕��@�f�[�^��S�č폜����
	//�iSPI_SetMode()�̓r���ŁA�ʂ̃h�L�������g�̕��@�f�[�^���Q�Ƃ���ƁA���݂��Ȃ�state�ɃA�N�Z�X�����肷���肪���邽�߁B�j
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			SPI_GetTextDocumentByID(docID).SPI_ClearSyntaxDataForAllLines();
		}
	}
	//
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			SPI_GetTextDocumentByID(docID).SPI_SetMode(inModeIndex);
		}
	}
	//�s��񂪕ς��̂ŁA�C���[�W�T�C�Y�̔��f���K�v
	GetApp().SPI_UpdateTextDrawPropertyAll(inModeIndex);
}

/**
�w�胂�[�h�̃h�L�������g��S�ĕ��@�ĔF������
*/
void	AApp::SPI_ReRecognizeSyntaxAll( const AModeIndex inModeIndex )
{
	//�e�h�L�������g���[�v
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid  )
		{
			//���@�ĔF���g���K�[
			SPI_GetTextDocumentByID(docID).SPI_ReRecognizeSyntax();
			//#683 ����A�s�Čv�Z�͕K�v�Ȃ��B�����A�s�܂�Ԃ����肩���o���t�H���g�ύX�@�\�Ή����A�����͕ύX�K�v�B
		}
	}
	//#450
	//�s���X�V
	SPI_UpdateLineImageInfo(inModeIndex);
	//�ĕ`��
	GetApp().NVI_RefreshAllWindow();
}

//#890
/**
�w�胂�[�h�̃h�L�������g�̕��@�F���X���b�h��S�Ē�~����
*/
void	AApp::SPI_StopSyntaxRecognizerThreadAll( const AModeIndex inModeIndex )
{
	//�e�h�L�������g���[�v
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid  )
		{
			SPI_GetTextDocumentByID(docID).SPI_StopSyntaxRecognizerThread();
		}
	}
}

//
/**
�w�胂�[�h�̑S�Ẵh�L�������g����RegExp�I�u�W�F�N�g���X�V����
*/
void	AApp::SPI_UpdateRegExpForAllDocuments( const AModeIndex inModeIndex )//#683
{
	//�e�h�L�������g���[�v
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid  )
		{
			//#683 �������x���P SPI_GetTextDocumentByID(docID).SPI_SetMode(inModeIndex);//IndentRegExp�̍X�V����эs�Čv�Z
			//TextInfo��RegExp�̍X�V���s�� #683
			SPI_GetTextDocumentByID(docID).SPI_UpdateTextInfoRegExp();
		}
	}
}

//#699
/**
���̍s�܂�Ԃ��v�Z���J�n�h�L�������g���������A�܂�Ԃ��v�Z���J�n����
//AApp::EVTDO_DoTickTimerAction()����R�[������A���^�u��ǂݍ��ނ��߂̏��������A����A�g�p���Ă��Ȃ��B
//���^�u��ǂݍ��ނƁA��������H�����A�N���サ�΂炭�X���b�h�������ďd���Ȃ�f�����b�g�����邽�߁B
*/
void	AApp::SPI_TransitNextDocumentToWrapCalculatingState()
{
	/*
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); 
				docID != kObjectID_Invalid; 
				docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetDocumentInitState() == kDocumentInitState_Initial )
		{
			SPI_GetTextDocumentByID(docID).SPI_TransitInitState_ViewActivated();
			return;
		}
	}
	*/
}

//#81
/**
�����o�b�N�A�b�v�t�H���_�ύX��S�Ẵh�L�������g�ɔ��f
*/
void	AApp::SPI_DoAutoBackupFolderChangedAll()
{
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); 
				docID != kObjectID_Invalid; 
				docID = NVI_GetNextDocumentID(docID) )
	{
		SPI_GetTextDocumentByID(docID).SPI_UpdateAutoBackupArray();
	}
}

//#868
/**
���[�h�ݒ�̃e�L�X�g�t�H���g���ύX���ꂽ�Ƃ��̊e�h�L�������g�ւ̔��f����
*/
void	AApp::SPI_TextFontSetInModeIsUpdatedAll( const AIndex inUpdatedModeIndex )
{
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); 
				docID != kObjectID_Invalid; 
				docID = NVI_GetNextDocumentID(docID) )
	{
		SPI_GetTextDocumentByID(docID).SPI_TextFontSetInModeIsUpdated(inUpdatedModeIndex);
	}
}

#pragma mark �e�L�X�g�h�L�������g����̒ʒm

void	AApp::SPI_DoTextDocumentSaved( const AFileAcc& inFile )
{
	mImportFileManager.DoFileSaved(inFile);
	//�}���`�t�@�C���u�����ȊO�Ȃ�SCM��update�R�}���h�𔭍s����
	//�i�}���`�t�@�C���u�����͑�ʂ�update���s���āA�I�����̌��ʎ󂯎�菈�����d���Ȃ�j
	if( SPI_GetMultiFileFindWindow().SPI_ExecutingMultiFileReplace() == false )
	{
		SCMUpdateFolderStatusByFile(inFile);//R0006
	}
}

//#896
/**
�e�L�X�g�h�L�������g��������̏���
*/
void	AApp::SPI_DoTextDocumentClosed( const AFileAcc& inFile, const AFileAcc& inProjectFolder )
{
	//�v���W�F�N�g�t�H���_�̃p�X�擾
	AText	projectFolderPath;
	inProjectFolder.GetPath(projectFolderPath);
	//�t�@�C���p�X�擾
	AText	closingDocumentFilePath;
	inFile.GetPath(closingDocumentFilePath);
	//�v���W�F�N�g������
	AIndex	projectIndex = SPI_GetSameProjectArrayIndex(inProjectFolder);
	if( projectIndex != kIndex_Invalid )
	{
		//==================�v���W�F�N�g���̃t�@�C�����P���J����Ă��Ȃ���ԂɂȂ����烁�����팸�̂���purge����==================
		//�v���W�F�N�g�����������ꍇ�́A���̃v���W�F�N�g�ɑ����鑼�̃h�L�������g�����݂��邩�ǂ����𒲂ׁA
		//�h�L�������g�����݂��Ă��Ȃ��ꍇ�́A���̃v���W�F�N�g�ɑ�����t�@�C����Import File Data��S��purge����
		
		//�v���W�F�N�g���̃t�@�C�����܂����݂��Ă��邩�ǂ����̃t���O
		ABool	oneOrMoreFileStillOpenedInProject = false;
		//�e�h�L�������g�̃��[�v
		for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
		{
			//�h�L�������g�̃t�@�C�����A�v���W�F�N�g�̃t�H���_�̔z�����ǂ����𒲂ׂ�B
			ADocumentID	docID = mDocumentArray.GetObjectConst(index).GetObjectID();
			if( NVI_GetDocumentByID(docID).NVI_GetDocumentType() == kDocumentType_Text && 
						NVI_GetDocumentByID(docID).NVI_IsFileSpecified() == true )
			{
				AText	filePath;
				NVI_GetDocumentByID(docID).NVI_GetFilePath(filePath);
				if( filePath.CompareMin(projectFolderPath) == true && filePath.Compare(closingDocumentFilePath) == false )
				{
					//�h�L�������g�̃t�@�C�����A�v���W�F�N�g�̃t�H���_�̔z���Ȃ̂Ńt���OON�ɂ���
					oneOrMoreFileStillOpenedInProject = true;
				}
			}
		}
		
		//�v���W�F�N�g���̃t�@�C��������J����Ă��Ȃ��ꍇ�A�v���W�F�N�g���̃t�@�C����import file data��S��purge����
		if( oneOrMoreFileStillOpenedInProject == false )
		{
			//���[�h���X�g�E�C���h�E�����X���b�h��~
			SPI_AbortCurrentWordsListFinderRequestInWordsListWindow();
			//�R�[���O���t�����X���b�h��~
			SPI_AbortCurrentWordsListFinderRequestInCallGrafWindow();
			//�v���W�F�N�g�ɑ�����t�@�C����Import File Data��S��purge����
			mImportFileManager.PurgeImportFileData(mSameProjectArray_Pathname.GetObjectConst(projectIndex));
		}
	}
}

/**
�h�L�������g�ҏW������
*/
void	AApp::SPI_DoTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	//Index window�֒ʒm
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_IndexWindow); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		SPI_GetIndexWindowDocumentByID(docID).SPI_Update_TextDocumentEdited(inFile,inTextIndex,inInsertedCount);
	}
	//CallGraf�փh�L�������g�ҏW��ʒm
	SPI_NotifyToCallGrafByTextDocumentEdited(inFile,inTextIndex,inInsertedCount);
	//WordsList�փh�L�������g�ҏW��ʒm
	SPI_NotifyToWordsListByTextDocumentEdited(inFile,inTextIndex,inInsertedCount);
}

//R0199
//�w��modeIndex�̃X�y���`�F�b�N�L���b�V�����폜
void	AApp::SPI_DeleteSpellCheckCache( const AModeIndex inModeIndex )
{
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObject(index).NVI_GetDocumentType() == kDocumentType_Text )
		{
			if( SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_GetModeIndex() == inModeIndex )
			{
				SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_ClearSpellCheckCache();
			}
		}
	}
}

//RC3
//�e�h�L�������g�̃v���W�F�N�g�t�H���_�ݒ���X�V
void	AApp::SPI_UpdateProjectFolder()
{
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObject(index).NVI_GetDocumentType() == kDocumentType_Text )
		{
			SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_UpdateProjectFolder();
		}
	}
}

//#379
/**
�e�h�L�������g��Diff�t�@�C���Ώۂ��X�V
*/
/*#81
ABool	AApp::SPI_UpdateDiffFileTargetForAllDocuments()
{
	ABool	result = false;
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObject(index).NVI_GetDocumentType() == kDocumentType_Text )
		{
			if( SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_UpdateDiffFileTarget() == true )
			{
				result = true;
			}
		}
	}
	return true;
}
*/

//#379
/**
Diff�Ώۃh�L�������g�̍s�܂�Ԃ��ݒ�i��ɃT�u�y�C�����ɏ]���čs�܂�Ԃ�����ݒ�j�̍X�V����
*/
/*#drop diff info�̕\���X�V�����ʓ|�Ȃ̂Ŋ��ɊJ���Ă���h�L�������g�ɂ͓K�p���Ȃ����Ƃɂ���B
void	AApp::SPI_UpdateDiffTargetDocumentWrap()
{
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObject(index).NVI_GetDocumentType() == kDocumentType_Text )
		{
			//Diff�Ώۃh�L�������g���ǂ����̔���
			if( SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_IsDiffTargetDocument() == true )
			{
				//�ݒ蔻��
				if( NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowDiffTargetWithWordWrap) == true )
				{
					//��ɍs�܂�Ԃ�����
					SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_SetWrapMode(kWrapMode_WordWrap,80);
				}
				else
				{
					//Working�h�L�������g�Ɠ����ݒ��K�p����
					ADocumentID	workingDocID = SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).
								SPI_GetDiffWorkingDocumentID();
					SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).
							SPI_SetWrapMode(SPI_GetTextDocumentByID(workingDocID).SPI_GetWrapMode(),
							SPI_GetTextDocumentByID(workingDocID).SPI_GetWrapLetterCount());
				}
			}
		}
	}
}
*/

//#379
/**
SCM��status update�R�}���h���s������ɃR�[�������i�h�L�������g��Diff���[�h�X�V�̂��߁j
*/
void	AApp::SPI_DoSCMStatusUpdated( const AFileAcc& inFolder )
{
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObject(index).NVI_GetDocumentType() == kDocumentType_Text )
		{
			SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_DoSCMStatusUpdated(inFolder);
		}
	}
}

/**
�őO�ʂ̃h�L�������g�̐e�t�H���_�擾
*/
ABool	AApp::SPI_GetMostFrontDocumentParentFolder( AFileAcc& outFolder ) const
{
	//�f�t�H���g�̓��[�U�[�h�L�������g�t�H���_
	AFileWrapper::GetDocumentsFolder(outFolder);
	//�őO�ʃh�L�������g�擾
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		if( SPI_GetTextDocumentConstByID(docID).NVI_IsFileSpecified() == true )
		{
			//�őO�ʃh�L�������g�̐e�t�H���_�擾
			AFileAcc	file;
			SPI_GetTextDocumentConstByID(docID).NVI_GetFile(file);
			outFolder.SpecifyParent(file);
			return true;
		}
	}
	return false;
}

/**
���݊J���Ă���t�@�C���̃g�[�^���̃������T�C�Y���擾
*/
AByteCount	AApp::SPI_GetTotalTextDocumentMemoryByteCount() const
{
	AByteCount	total = 0;
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); 
		docID != kObjectID_Invalid; 
		docID = NVI_GetNextDocumentID(docID) )
	{
		total += SPI_GetTextDocumentConstByID(docID).SPI_GetTextDocumentMemoryByteCount();
	}
	return total;
}

#pragma mark ===========================

#pragma mark <���L�N���X(ADocument_IndexWindow)�����^�폜�^�擾>

#pragma mark ===========================

ADocument_IndexWindow&	AApp::SPI_GetIndexWindowDocumentByID( const AObjectID inDocID )
{
	MACRO_RETURN_DOCUMENT_DYNAMIC_CAST(ADocument_IndexWindow,kDocumentType_IndexWindow,inDocID);//#199
}
const ADocument_IndexWindow&	AApp::SPI_GetIndexWindowDocumentConstByID( const AObjectID inDocID ) const
{
	MACRO_RETURN_CONSTDOCUMENT_DYNAMIC_CAST(ADocument_IndexWindow,kDocumentType_IndexWindow,inDocID);//#199
}

#pragma mark ===========================

#pragma mark <���L�N���X(AWindow_Text)�����^�폜�^�擾>

#pragma mark ===========================

/**
�e�L�X�g�E�C���h�E�擾
*/
AWindow_Text&	AApp::SPI_GetTextWindowByID( const AObjectID inWindowID )
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,inWindowID);
	//#199 return dynamic_cast<AWindow_Text&>(NVI_GetWindowByID(inWindowID));
}

//#850
/**
�e�L�X�g�E�C���h�E�擾
*/
const AWindow_Text&	AApp::SPI_GetTextWindowConstByID( const AObjectID inWindowID ) const
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,inWindowID);
}

/*#850
AWindowID	AApp::SPI_GetTabModeMainTextWindowID()
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			AWindowID	winID = mWindowArray.GetObject(index).GetObjectID();
			if( SPI_GetTextWindowByID(winID).SPI_IsTabModeMainWindow() == true )
			{
				return winID;
			}
		}
	}
	return kObjectID_Invalid;
}
*/

//#850
/**
�^�u������ԑ����e�L�X�g�E�C���h�E���擾
*/
AWindowID	AApp::SPI_GetMostNumberTabsTextWindowID() const
{
	AItemCount	maxTabCount = 0;
	AWindowID	maxTabWindowID = kObjectID_Invalid;
	//�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObjectConst(index).NVI_GetWindowType() == kWindowType_Text )
		{
			AWindowID	winID = mWindowArray.GetObjectConst(index).GetObjectID();
			AItemCount	tabCount = SPI_GetTextWindowConstByID(winID).NVI_GetSelectableTabCount();
			if( tabCount > maxTabCount )
			{
				//�^�u���ő�̃E�C���h�E
				maxTabCount = tabCount;
				maxTabWindowID = winID;
			}
		}
	}
	//�S�ẴE�C���h�E�̃^�u����1�ȉ��Ȃ�A�őO�ʂ̃e�L�X�g�E�C���h�E��Ԃ�
	if( maxTabCount <= 1 )
	{
		maxTabWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);
	}
	//
	return maxTabWindowID;
}


void	AApp::SPI_UpdateTextWindowViewBoundsAll()
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateViewBounds();//B0402 All(inModeIndex);
		}
	}
}

void	AApp::SPI_UpdateTextDrawPropertyAll( const AModeIndex inModeIndex )
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateTextDrawPropertyAll(inModeIndex);
		}
	}
}

//#1350
/**
���ׂẴE�C���h�E�̉E�T�C�h�o�[��\���^��\������
*/
void	AApp::SPI_ShowHideRightSidebarAll()
{
	ABool	show = NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kRightSideBarDisplayed);
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_ShowHideRightSideBar(show,true);
		}
	}
}

//#1350
/**
���ׂẴE�C���h�E�̍��T�C�h�o�[��\���^��\������
*/
void	AApp::SPI_ShowHideLeftSidebarAll()
{
	ABool	show = NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kLeftSideBarDisplayed);
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_ShowHideLeftSideBar(show,true);
		}
	}
}

//#450
/**
*/
void	AApp::SPI_UpdateLineImageInfo( const AModeIndex inModeIndex )
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateLineImageInfo(inModeIndex);
		}
	}
}

/*#291
//R0006
void	AApp::SPI_UpdateJumpListWithColor()
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateJumpListWithColor();
		}
	}
}

void	AApp::SPI_UpdatePropertyJumpListAll()
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdatePropertyJumpList();
		}
	}
}
*/

//#1255
/**
�S�e�L�X�g�E�C���h�E�����x�X�V
*/
void	AApp::SPI_UpdateWindowAlphaAll()
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateWindowAlpha();
		}
	}
}

void	AApp::SPI_RemakeToolButtonsAll( const AModeIndex inModeIndex )
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_RemakeToolButtonsAll(inModeIndex);
		}
	}
}

//B0389 OK
void	AApp::SPI_UpdateFilePathDisplayAll()
{
	/*#725 InfoHeader�p�~
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_SetInfoHeaderText(GetEmptyText());
		}
	}
	 */
	/*#725
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().NVI_UpdateProperty();
	}
	*/
	SPI_UpdateFileListWindowsProperty();//#725
}

/*#725
//B0442
//�A�N�e�B�u�ł͂Ȃ��A�t���[�e�B���O�E�C���h�E�̕\�����B��
void	AApp::SPI_UpdateFloatingWindowHideAll()
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			//
			if( SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).NVI_IsWindowActive() == false )
			{
				//
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_HideFloatingWindow();
			}
		}
	}
}
*/

//#844
/**
�S�ẴE�C���h�E�̃L�����b�g�`��X�V
*/
void	AApp::SPI_RefreshCaretAll()
{
	//�e�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			//�L�����b�g�`��X�V
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_RefreshCaret();
		}
	}
}

//#724
/**
�S�ẴE�C���h�E�̃}�N���o�[���ڃe�L�X�g���X�V�i�V���[�g�J�b�g�X�V�����ɃR�[�������j
*/
void	AApp::SPI_UpdateMacroBarItemContentAll()
{
	//�e�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			//�}�N���o�[���ڂ̃e�L�X�g���X�V
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateMacroBarItemContent();
		}
	}
}

/*#725
//#291
void	AApp::SPI_UpdatePaneLayout()
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
		SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateInfoPaneArray();
		SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateSubPaneArray();
		SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateViewBounds();
		}
	}
}
*/

//#379
/**
�e�E�C���h�E�̃c�[���o�[���ڂ̒l�̍X�V�i�e�L�X�g�G���R�[�f�B���O���j���[�Ȃǁj
*/
void	AApp::SPI_UpdateToolbarItemValue( const ADocumentID inDocumentID )
{
	//���ׂĂ�TextWindow�ɑ΂��ăc�[���o�[���ڂ̒l�X�V��ʒm����
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				//AWindow_Text::SPI_UpdateToolbarItemValue()��documentID�Ɉ�v����^�u�����X�V����
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateToolbarItemValue(inDocumentID);
			}
		}
	}
}

/**
�S�Ẵe�L�X�g�E�C���h�E�̃c�[���o�[�̒l�\�����X�V
*/
void	AApp::SPI_UpdateToolbarItemValueAll()
{
	//���ׂĂ�TextWindow�ɑ΂��ăc�[���o�[���ڂ̒l�X�V��ʒm����
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			AWindowID	winID = mWindowArray.GetObject(index).GetObjectID();
			SPI_GetTextWindowByID(winID).SPI_UpdateToolbarItemValue(SPI_GetTextWindowByID(winID).NVI_GetCurrentDocumentID());
		}
	}
}

//#379
/**
�e�E�C���h�E��TextDrawProperty�X�V
*/
void	AApp::SPI_UpdateTextDrawProperty( const ADocumentID inDocumentID )
{
	//���ׂĂ�TextWindow�ɑ΂���TextDrawProperty�X�V��ʒm����
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				//AWindow_Text::SPI_UpdateTextDrawProperty()��documentID�Ɉ�v����^�u�����X�V����
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateTextDrawProperty(inDocumentID);
			}
		}
	}
}

//#379
/**
�e�E�C���h�E��ViewBounds�X�V
*/
void	AApp::SPI_UpdateViewBounds( const ADocumentID inDocumentID )
{
	//���ׂĂ�TextWindow�ɑ΂���TextDrawProperty�X�V��ʒm����
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				//AWindow_Text::SPI_UpdateViewBounds()��documentID�Ɉ�v����^�u�����X�V����
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateViewBounds(inDocumentID);
			}
		}
	}
}

//#379
/**
�e�E�C���h�E��InfoHeader�X�V
*/
/*#725
void	AApp::SPI_UpdateInfoHeader( const ADocumentID inDocumentID )
{
	//���ׂĂ�TextWindow�ɑ΂���InfoHeader�X�V��ʒm����
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				//AWindow_Text::SPI_UpdateInfoHeader()��documentID�Ɉ�v����^�u�����X�V����
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateInfoHeader(inDocumentID);
			}
		}
	}
}
*/

//#379
/**
�h�L�������g�ɑΉ�����Window/Tab��TryClose
*/
void	AApp::SPI_TryCloseTextDocument( const ADocumentID inDocumentID )
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_TryCloseDocument(inDocumentID);
			}
		}
	}
}

//#379
/**
�h�L�������g�ɑΉ�����Window/Tab��TryClose
*/
void	AApp::SPI_ExecuteCloseTextDocument( const ADocumentID inDocumentID )
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_ExecuteCloseDocument(inDocumentID);
			}
		}
	}
}

//#379
/**
�e�E�C���h�E��Diff�\���X�V
*/
void	AApp::SPI_UpdateDiffDisplay( const ADocumentID inDocumentID )
{
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateDiffDisplay(inDocumentID);
			}
		}
	}
}

#pragma mark ===========================

#pragma mark <���L�N���X(AModePrefDB)�����^�폜�^�擾>

#pragma mark ===========================

//ModePrefDB�擾
AModePrefDB&	AApp::SPI_GetModePrefDB( const AModeIndex inModeIndex, const ABool inLoadIfNotLoaded )//#253
{
	if( inModeIndex >= mModePrefDBArray.GetItemCount() )
	{
		//���̃`�F�b�N�K�v���ǂ����킩��Ȃ����A���o�[�W�������`�F�b�N���Ă���̂ŁE�E�E
		_ACError("modeIndex error",this);
		return SPI_GetModePrefDB(kStandardModeIndex);
	}
	//�����[�h�Ȃ烍�[�h����
	if( mModePrefDBArray.GetObject(inModeIndex).IsLoaded() == false && inLoadIfNotLoaded == true )
	{
		mModePrefDBArray.GetObject(inModeIndex).LoadOrWaitLoadComplete(true);
	}
	return mModePrefDBArray.GetObject(inModeIndex);
}

/**
���[�h��(raw)���烂�[�hIndex�擾
@note thread-safe
*/
AModeIndex	AApp::SPI_FindModeIndexByModeRawName( const AText& inModeName ) const
{
	AStMutexLocker	locker(mModeNameArrayMutex);
	return mModeRawNameArray.Find(inModeName);
}

/**
���[�h��(�\����)���烂�[�hIndex�擾
@note thread-safe
@note �\�����͏d�����Ă���\��������̂Ŏg�p�͔񐄏��BAppleScript�̌݊����̂��߂Ɏc���Ă���B
*/
AModeIndex	AApp::SPI_FindModeIndexByModeDisplayName( const AText& inModeName ) const
{
	AStMutexLocker	locker(mModeNameArrayMutex);
	return mModeDisplayNameArray.Find(inModeName);
}

//#427
/**
���[�h��(raw)�̏d���`�F�b�N�i�啶�������������j
*/
ABool	AApp::SPI_CheckModeRawNameUnique( const AText& inName ) const
{
	AStMutexLocker	locker(mModeNameArrayMutex);
	for( AIndex i = 0; i < mModeRawNameArray.GetItemCount(); i++ )
	{
		if( inName.CompareIgnoreCase(mModeRawNameArray.GetTextConst(i)) == true )
		{
			return true;
		}
	}
	return false;
}

//#427
/**
���[�h���ύX������
*/
void	AApp::SPI_ModeNameUpdated( const AModeIndex inModeIndex )
{
	AStMutexLocker	locker(mModeNameArrayMutex);
	//���[�h���z��X�V
	AText	modeRawName;
	SPI_GetModePrefDB(inModeIndex).GetModeRawName(modeRawName);
	mModeRawNameArray.Set(inModeIndex,modeRawName);
	AText	modeDisplayName;
	SPI_GetModePrefDB(inModeIndex).GetModeDisplayName(modeDisplayName);
	mModeRawNameArray.Set(inModeIndex,modeDisplayName);
	//���[�h���j���[�X�V
	SPI_UpdateModeMenu();
	//���[�h�ݒ�E�C���h�E�X�V
	GetApp().SPI_GetModePrefWindow(inModeIndex).NVI_UpdateProperty();//#858
	GetApp().SPI_GetModePrefWindow(inModeIndex).NVI_RefreshWindow();//#858
}

/**
�e�L�X�g�t�@�C���̃t�@�C�������烂�[�hIndex���擾����
@note thread-safe
*/
AModeIndex	AApp::SPI_FindModeIDByTextFilePath( const AText& inFilePath ) const//R0210
{
	//==================�g���q�ݒ肪��v���郂�[�h������==================
	//�g���q�ݒ肪��v���郂�[�h��S�Ď擾����
	AArray<AIndex>	modeIndexArray;//R0210
	SPI_FindModeIndexFromSuffix(inFilePath,modeIndexArray);
	/*
	for( AIndex modeIndex = 0; modeIndex < mModePrefDBArray.GetItemCount(); modeIndex++ )
	{
		if( mModePrefDBArray.GetObjectConst(modeIndex).IsThisModeSuffix(inFilePath) == true )//R0210
		{
			//R0210 return modeIndex;
			modeIndexArray.Add(modeIndex);
		}
	}
	*/
	//R0210
	//�g���q����v���郂�[�h���Ȃ��ꍇ�́A�W�����[�h��Ԃ�
	if( modeIndexArray.GetItemCount() == 0 )
	{
		return kStandardModeIndex;
	}
	//�g���q����v���郂�[�h���P�݂̂̏ꍇ�́A���̃��[�h�Ŋm��B
	if( modeIndexArray.GetItemCount() == 1 )
	{
		return modeIndexArray.Get(0);
	}
	
	//#868
	//==================�����̃��[�h�̊g���q����v�����ꍇ�A���[�h�t�@�C���p�X�Ɉ�v���郂�[�h������==================
	//���L�̗D�揇��
	//1. ���[�h�t�@�C���p�X����v���郂�[�h
	//2. ���[�h�t�@�C���p�X�ݒ肪�������[�h�i��̃��[�h�D��j
	//3. �ŏ��̃��[�h
	//���[�h�t�@�C���p�X�ݒ肪�������[�h���L�����邽�߂̕ϐ�
	AIndex	modeIndexWithoutFilePathSetting = kIndex_Invalid;
	//��Ō��������g���q����v���郂�[�h���̃��[�v
	for( AIndex i = 0; i < modeIndexArray.GetItemCount(); i++ )
	{
		//���[�h�t�@�C���p�X�Ɉ�v�����炻�̃��[�hindex��Ԃ�l�Ƃ��ďI��
		if( mModePrefDBArray.GetObjectConst(modeIndexArray.Get(i)).IsThisModeSelectFilePath(inFilePath) == true )
		{
			return modeIndexArray.Get(i);
		}
		//���[�h�t�@�C���p�X�ݒ肪�����ꍇ�́A���̃��[�h�i�ŏ��̂��́j���L������
		if( modeIndexWithoutFilePathSetting == kIndex_Invalid )
		{
			if( mModePrefDBArray.GetObjectConst(modeIndexArray.Get(i)).GetItemCount_TextArray(AModePrefDB::kModeSelectFilePath) == 0 )
			{
				modeIndexWithoutFilePathSetting = modeIndexArray.Get(i);
			}
		}
	}
	//------------------���[�h�t�@�C���p�X�ݒ�Ɉ����v���Ȃ������ꍇ------------------
	if( modeIndexWithoutFilePathSetting == kIndex_Invalid )
	{
		//���[�h�t�@�C���p�X�ݒ肪�������[�h�����������ꍇ�́A�ŏ��̃��[�h��Ԃ�
		return modeIndexArray.Get(0);
	}
	else
	{
		//���[�h�t�@�C���p�X�ݒ肪�������[�h������΁A���̃��[�h
		return modeIndexWithoutFilePathSetting;
	}
}

//#427
/**

���L�̉ӏ�����R�[�������B
1. AApp::InitModePref()�́A�W�����[�h�쐬�ӏ��B
�W�����[�h��V�K�쐬�����Ƃ��i��mi�V�K�C���X�g�[�����j��inConvertToAutoUpdate=true�ƂȂ�B
2. AApp::InitModePref()�́A�W���ȊO�̃��[�h�쐬�ӏ��B
���[�h��V�K�쐬�����Ƃ��i��mi�V�K�C���X�g�[�����A����сA�g�ݍ��݃��[�h�ǉ����j��inConvertToAutoUpdate=true�ƂȂ�B
3. AApp::SPI_AddNewMode()�V�K���[�h�iinConvertToAutoUpdate��false�j
4. SPI_AddNewModeImportFromFolder()
�C���|�[�g�̏ꍇ��inConvertToAutoUpdate��false�B
Web����擾�̏ꍇ��inConvertToAutoUpdate��true�B

�܂�A�g�ݍ��݃��[�h�C���X�g�[�����̂�inConvertToAutoUpdate=true�A���[�h�V�K�쐬�i�C���|�[�g�܂ށj��inConvertToAutoUpdate=false�ƂȂ�B

inConvertToAutoUpdate=true�̂Ƃ��́A���L�������s���B#1374
�E���[�U�[�ҏW���ڂ̃L�[���[�h�E���o�����폜
*/
AModeIndex	AApp::AddMode( const AFileAcc inModeFolder, const ABool inConvertToAutoUpdate, const ABool inRemoveMultiLanguageModeName )
{
	//���[�h�t�@�C���擾
	AText	filename;
	inModeFolder.GetFilename(filename);
	
	//==================�����X�V���[�h�f�[�^�ւ̕ϊ����s���@�i�L�[���[�h�ƃc�[���������X�V�t�H���_�ֈړ��j==================
	
	//�����X�V���[�h�f�[�^�ւ̕ϊ��́AModePreferences.mi�����݂���ꍇ�̂ݑΉ�����
	//ModePreferences.mi�������ꍇ�Aautoupdate�t�H���_�z�������MMKE���\�[�X������ǂݍ��ޕK�v�����邪�A
	//�������ώG�ɂȂ邽�߁B
	//�i���o�[�W�����Ŕz�z����Ă���f�[�^�́A�����X�V�Ŕz�z����邱�Ƃ������Ǝv���邽�߁A
	//���o�[�W�����f�[�^�ɂ��āA�����X�V�f�[�^�ւ̕ϊ�������K�v�͂Ȃ��j
	ABool	convertToAutoUpdate = inConvertToAutoUpdate;
	AFileAcc 	modePrefFile;
	modePrefFile.SpecifyChild(inModeFolder,"data/ModePreferences.mi");
	if( modePrefFile.Exist() == false )
	{
		convertToAutoUpdate = false;
	}
	
	/*#1374
	�g�ݍ��݃��[�h�C���X�g�[�����A���[�U�[�f�[�^�փR�s�[����̂�data/ModePreferences.mi�݂̂ɂ����̂ŁAautoupdate�t�H���_�ֈڂ���Ƃ͕s�v�ƂȂ����B
	//�����X�V���[�h�f�[�^���\��
	if( convertToAutoUpdate == true )
	{
		//autoupdate�t�H���_����
		AFileAcc	autoUpdateFolder;
		autoUpdateFolder.SpecifyChild(inModeFolder,"autoupdate");
		autoUpdateFolder.CreateFolder();
		
		//���[�h�t�H���_���̂�autoupdate, autoupdate_userdata�ȊO���Aautoupdate�t�H���_�փR�s�[
		AObjectArray<AFileAcc>	children;
		inModeFolder.GetChildren(children);
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			//�t�@�C���^�t�H���_���擾
			AText	filename;
			children.GetObjectConst(i).GetFilename(filename);
			//autoupdate, autoupdate_userdata�t�H���_�̓R�s�[�ΏۊO
			if( filename.Compare("autoupdate") == true )   continue;
			if( filename.Compare("autoupdate_userdata") == true )   continue;
			//�t�@�C���܂��̓t�H���_���R�s�[
			AFileAcc	dst;
			dst.SpecifyChild(autoUpdateFolder,filename);
			children.GetObject(i).CopyFileOrFolderTo(dst,false,false);
		}
		
		//���[�U�[�c�[���t�H���_���폜
		AFileAcc	userToolFolder;
		userToolFolder.SpecifyChild(inModeFolder,"tool");
		userToolFolder.DeleteFileOrFolderRecursive();
		
		//�L�[���[�h�����t�@�C�����폜
		AFileAcc	keywordExplanationFile;
		keywordExplanationFile.SpecifyChild(inModeFolder,"data/keywords_explanation.txt");
		keywordExplanationFile.DeleteFileOrFolderRecursive();
		
		//���[�U�[�L�[���[�h�t�H���_���g���폜 #954
		AFileAcc	userKeywordFolder;
		userKeywordFolder.SpecifyChild(inModeFolder,"keyword");
		userKeywordFolder.DeleteFileOrFolderRecursive();
	}
	*/
	
	//#954
	//==================���[�U�[tool, keyword�t�H���_����==================
	{
		AFileAcc	userToolFolder;
		userToolFolder.SpecifyChild(inModeFolder,"tool");
		userToolFolder.CreateFolderRecursive();
		
		AFileAcc	userKeywordFolder;
		userKeywordFolder.SpecifyChild(inModeFolder,"keyword");
		userKeywordFolder.CreateFolderRecursive();
	}
	
	//==================���[�hDB�I�u�W�F�N�g����==================
	//�I�u�W�F�N�g����
	AModeIndex	modeIndex = mModePrefDBArray.AddNewObject();
	//mode pref db������
	mModePrefDBArray.GetObject(modeIndex).InitMode(modeIndex,inModeFolder);
	//�����X�V���[�h�ւ̕ϊ��A�܂��́A�����ꃂ�[�h���̍폜���s�����́A�����̏������s�����߂ɁA������mode pref�̃��[�h���s��
	if( inConvertToAutoUpdate == true || inRemoveMultiLanguageModeName == true )
	{
		SPI_GetModePrefDB(modeIndex);
	}
	
	//#868
	//==================���[�h�E�C���h�E����==================
	mModePrefWindowIDArray.Add(kObjectID_Invalid);
	
	
	//==================�����ꃂ�[�h���̍폜==================
	//���[�h�����w�肵�ăC���|�[�g�����Ƃ��́A�����ꃂ�[�h���͍폜����
	if( inRemoveMultiLanguageModeName == true )
	{
		mModePrefDBArray.GetObject(modeIndex).RemoveMultiLanguageModeNames();
	}
	
	//==================���[�h���f�[�^�擾==================
	//mModeNameArray��ݒ�
	{
		AStMutexLocker	locker(mModeNameArrayMutex);
		AText	modeRawName;
		mModePrefDBArray.GetObjectConst(modeIndex).GetModeRawName(modeRawName);
		mModeRawNameArray.Add(modeRawName);
		AText	modeDisplayName;
		mModePrefDBArray.GetObjectConst(modeIndex).GetModeDisplayName(modeDisplayName);
		mModeDisplayNameArray.Add(modeDisplayName);
	}
	
	//==================�����X�V���[�h�f�[�^�ւ̕ϊ����s���A�i���[�U�[�f�[�^����g�ݍ��݃L�[���[�h���폜�j==================
	//#427 
	if( convertToAutoUpdate == true )
	{
		//���[�U�[�L�[���[�h�J�e�S���[�^���o�����폜����
		//�L�[���[�h�J�e�S���[
		for( AIndex index = mModePrefDBArray.GetObjectConst(modeIndex).GetItemCount_Array(AModePrefDB::kAdditionalCategory_Name)-1;
					index >= 0;
					index-- )
		{
			if( mModePrefDBArray.GetObjectConst(modeIndex).GetData_BoolArray(AModePrefDB::kAdditionalCategory_AutoUpdate,index) == false )
			{
				mModePrefDBArray.GetObject(modeIndex).DeleteRow_Table(AModePrefDB::kAdditionalCategory_Name,index);
			}
		}
		/*#844
		//���o��
		for( AIndex index = mModePrefDBArray.GetObjectConst(modeIndex).GetItemCount_Array(AModePrefDB::kJumpSetup_Name)-1;
					index >= 0;
					index-- )
		{
			if( mModePrefDBArray.GetObjectConst(modeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_AutoUpdate,index) == false )
			{
				mModePrefDBArray.GetObject(modeIndex).DeleteRow_Table(AModePrefDB::kJumpSetup_Name,index);
			}
		}
		*/
	}
	//
	return modeIndex;
}

/**
���[�h�g���q�����n�b�V�����X�V
*/
void	AApp::SPI_UpdateModeSuffixArray( const AModeIndex inModeIndex )
{
	//�r������
	AStMutexLocker	locker(mModeSuffixArrayMutex);
	
	//�w�胂�[�h�̃f�[�^���폜
	{
		for( AIndex i = 0; i < mModeSuffixArray_Suffix.GetItemCount(); )
		{
			//���[�h����v���Ă�����폜
			if( mModeSuffixArray_ModeIndex.Get(i) == inModeIndex )
			{
				mModeSuffixArray_Suffix.Delete1(i);
				mModeSuffixArray_ModeIndex.Delete1(i);
			}
			else
			{
				i++;
			}
		}
	}
	
	//���[�h��disable�Ȃ�f�[�^�ǉ��������^�[��
	if( SPI_GetModePrefDB(inModeIndex,false).GetData_Bool(AModePrefDB::kEnableMode) == false )
	{
		return;
	}
	
	//���[�h�ݒ�ɐݒ肳�ꂽsuffix��ǉ�
	{
		AItemCount	itemCount = SPI_GetModePrefDB(inModeIndex,false).GetItemCount_Array(AModePrefDB::kSuffix);
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//suffix�擾
			AText	suffix;
			SPI_GetModePrefDB(inModeIndex,false).GetData_TextArray(AModePrefDB::kSuffix,i,suffix);
			//�f�[�^�ǉ�
			mModeSuffixArray_Suffix.Add(suffix);
			mModeSuffixArray_ModeIndex.Add(inModeIndex);
		}
	}
}

/**
�g���q�ɑΉ����郂�[�h������
*/
void	AApp::SPI_FindModeIndexFromSuffix( const AText& inFilePath, AArray<AIndex>& outModeIndexArray ) const
{
	//���ʏ�����
	outModeIndexArray.DeleteAll();
	//suffix�J�n�ʒu���擾
	AIndex	suffixspos = inFilePath.GetSuffixStartPos();
	//#1428 �g���q�Ȃ�����v����悤�ɂ��� if( suffixspos == inFilePath.GetItemCount() )   return;
	//�r������
	AStMutexLocker	locker(mModeSuffixArrayMutex);
	//�g���q�n�b�V������inFilePath�̊g���q�ɑΉ����鍀�ڂ�index������
	AArray<AIndex>	indexArray;
	mModeSuffixArray_Suffix.FindAll(inFilePath,suffixspos,inFilePath.GetItemCount()-suffixspos,indexArray);
	//���[�hindex���擾
	AItemCount	itemCount = indexArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		outModeIndexArray.Add(mModeSuffixArray_ModeIndex.Get(indexArray.Get(i)));
	}
}

//
AModeIndex	AApp::SPI_AddNewMode( const AText& inModeName )
{
	//���ɑ��݂��郂�[�h���Ȃ�I��
	AModeIndex	modeIndex = SPI_FindModeIndexByModeRawName(inModeName);
	if( modeIndex != kIndex_Invalid )   return kIndex_Invalid;
	
	//���[�h�t�H���_�̍쐬
	AFileAcc	rootModeFolder;
	SPI_GetModeRootFolder(rootModeFolder);//�A�v���P�[�V����(.app)�Ɠ����_mode�t�H���_�i�D��j���APref�t�H���_��mode�t�H���_
	AFileAcc	modeFolder;
	modeFolder.SpecifyChild(rootModeFolder,inModeName);
	modeFolder.CreateFolder();
	
	//�c�[���t�H���_�̐���
	AFileAcc	toolFolder;
	toolFolder.SpecifyChild(modeFolder,"tool");
	
	//���[�h�쐬
	modeIndex = AddMode(modeFolder,false,true);
	
	//���[�h�t�@�C���ۑ�
	SPI_GetModePrefDB(modeIndex).SaveToFile();
	
	SPI_UpdateModeMenu();
	
	return modeIndex;
}

/**
�����[�h�t�H���_����C���|�[�g���āA���[�h��V�K�ǉ�
*/
AModeIndex	AApp::SPI_AddNewModeImportFromFolder( const AText& inModeName, const AFileAcc& inImportFolder, 
			const ABool inSecurityScreened, const ABool inConvertToAutoUpdate )//R0000 #427
{
	//���ɑ��݂��郂�[�h���Ȃ�I��
	AModeIndex	modeIndex = SPI_FindModeIndexByModeRawName(inModeName);
	if( modeIndex != kIndex_Invalid )   return kIndex_Invalid;
	
	//R0000 security ���ǂ������邮�炢�����Ȃ�
	if( inSecurityScreened == false )
	{
		AText	text;
		AText	report;
		if( SPI_ScreenModeExecutable(inImportFolder,text,report) == true )
		{
			SPI_ShowModeExecutableAlertWindow(inModeName,inImportFolder,text,report,inConvertToAutoUpdate);//#427 #858
			return kIndex_Invalid;
		}
	}
	
	//�R�s�[�惂�[�h�t�H���_�̎擾
	AFileAcc	rootModeFolder;
	SPI_GetModeRootFolder(rootModeFolder);//�A�v���P�[�V����(.app)�Ɠ����_mode�t�H���_�i�D��j���APref�t�H���_��mode�t�H���_
	AFileAcc	modeFolder;
	modeFolder.SpecifyChild(rootModeFolder,inModeName);
	
	//���[�h�t�H���_���R�s�[
	inImportFolder.CopyFolderTo(modeFolder,true,true);
	
	//���[�h�쐬
	modeIndex = AddMode(modeFolder,inConvertToAutoUpdate,true);//#427 ���[�h�ǉ����Aautoupdate�t�H���_�\��
	
	//�C���|�[�g���̃��[�h��disable�ɂȂ��Ă���\��������̂ŁA�����ŃR�s�[�惂�[�h��enable�ɂ���
	SPI_GetModePrefDB(modeIndex).SetData_Bool(AModePrefDB::kEnableMode,true);
	
	//���[�h���j���[�̍X�V
	SPI_UpdateModeMenu();
	
	//�u�����v���W�F�N�g�v�̃f�[�^���X�V
	SPI_UpdateAllSameProjectData();
	
	return modeIndex;
}

//R0000 security
ABool	AApp::SPI_ScreenModeExecutable( const AFileAcc& inItem, AText& outText, AText& outReport ) 
{
	/*R0137 outText.DeleteAll();
	outReport.DeleteAll();
	return false;//�b��
	*/
	if( mNoCheckModeExecutable == true )   return false;
	return ScreenModeExecutableItem(inItem,outText,outReport);
}
ABool	AApp::ScreenModeExecutableFolder( const AFileAcc& inFolder, AText& outText, AText& outReport )
{
	ABool	result = false;
	AObjectArray<AFileAcc>	folderArray;
	inFolder.GetChildren(folderArray);
	for( AIndex i = 0; i < folderArray.GetItemCount(); i++ )
	{
		if( ScreenModeExecutableItem(folderArray.GetObject(i),outText,outReport) == true )
		{
			result = true;
		}
	}
	return result;
}
ABool	AApp::ScreenModeExecutableItem( const AFileAcc& inItem, AText& outText, AText& outReport )
{
	ABool	result = false;
#if IMPLEMENTATION_FOR_MACOSX
	AFileAcc	file;
	file.CopyFrom(inItem);
	//�p�X�̓G�C���A�X�����O�̃p�X��\������
	AText	path;
	file.GetNormalizedPath(path);
	path.ConvertToCanonicalComp();
	if( file.IsFolder() == true )
	{
		//�t�H���_�̓c�[�����j���[�̎��s�ΏۂɂȂ�Ȃ����A�����̂��߁A.app�̓`�F�b�N����
		AText	filename;
		file.GetFilename(filename);
		AText	suffix;
		filename.GetSuffix(suffix);
		if( suffix.Compare(".app") == true )
		{
			//�A�v���P�[�V����
			outText.AddText(path);
			outText.Add(kUChar_LineEnd);
			outReport.AddCstring("==========================================================================================\r");
			AText	t;
			t.SetLocalizedText("ModeExecutableReport_Application");
			outReport.AddText(t);
			outReport.Add(kUChar_LineEnd);
			outReport.AddText(path);
			outReport.Add(kUChar_LineEnd);
			outReport.AddCstring("==========================================================================================\r");
			outReport.Add(kUChar_LineEnd);
			outReport.Add(kUChar_LineEnd);
			outReport.Add(kUChar_LineEnd);
			result = true;
		}
		
		if( ScreenModeExecutableFolder(file,outText,outReport) == true )
		{
			result = true;
		}
	}
	else
	{
		//�G�C���A�X����
		file.ResolveAlias();
		
		AText	filename;
		file.GetFilename(filename);
		AText	suffix;
		filename.GetSuffix(suffix);
		AFileAttribute	attr;
		file.GetFileAttribute(attr);
		AText	text;
		GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_ScreenModeExecutableItem,file,text);
		//ABool	executable = file.IsExecutable();
		if( /*B0000 attr.type == 'osas'*/file.IsAppleScriptFile() == true )
		{
			//AppleScript
			outText.AddText(path);
			outText.Add(kUChar_LineEnd);
			AText	src;
			SPI_GetAppleScriptSource(file,src);
			outReport.AddCstring("==========================================================================================\r");
			AText	t;
			t.SetLocalizedText("ModeExecutableReport_AppleScript");
			outReport.AddText(t);
			outReport.Add(kUChar_LineEnd);
			outReport.AddText(path);
			outReport.Add(kUChar_LineEnd);
			outReport.AddCstring("==========================================================================================\r");
			t.SetLocalizedText("ModeExecutableReport_Content");
			outReport.AddText(t);
			outReport.Add(kUChar_LineEnd);
			outReport.AddCstring("==========================================================================================\r");
			outReport.AddText(src);
			outReport.Add(kUChar_LineEnd);
			outReport.AddCstring("==========================================================================================\r");
			outReport.Add(kUChar_LineEnd);
			outReport.Add(kUChar_LineEnd);
			outReport.Add(kUChar_LineEnd);
			result = true;
		}
		else if( attr.type == 'APPL' || suffix.Compare(".app") == true )
		{
			//�A�v���P�[�V����
			outText.AddText(path);
			outText.Add(kUChar_LineEnd);
			outReport.AddCstring("==========================================================================================\r");
			AText	t;
			t.SetLocalizedText("ModeExecutableReport_Application");
			outReport.AddText(t);
			outReport.Add(kUChar_LineEnd);
			outReport.AddText(path);
			outReport.Add(kUChar_LineEnd);
			outReport.AddCstring("==========================================================================================\r");
			outReport.Add(kUChar_LineEnd);
			outReport.Add(kUChar_LineEnd);
			outReport.Add(kUChar_LineEnd);
			result = true;
		}
		/*else if( executable )
		{
			//���s�\�t�@�C��
			outText.AddText(path);
			outText.Add(kUChar_LineEnd);
			outReport.AddCstring("==========================================================================================\r");
			AText	t;
			t.SetLocalizedText("ModeExecutableReport_Executable");
			outReport.AddText(t);
			outReport.Add(kUChar_LineEnd);
			outReport.AddText(path);
			outReport.Add(kUChar_LineEnd);
			outReport.AddCstring("==========================================================================================\r");
			outReport.Add(kUChar_LineEnd);
			outReport.Add(kUChar_LineEnd);
			outReport.Add(kUChar_LineEnd);
			result = true;
		}*/
		else if( text.GetItemCount() > 2 )
		{
			if( text.Get(0) == '#' && text.Get(1) == '!' )
			{
				//�V�F���X�N���v�g
				outText.AddText(path);
				outText.Add(kUChar_LineEnd);
				outReport.AddCstring("==========================================================================================\r");
				AText	t;
				t.SetLocalizedText("ModeExecutableReport_ShellScript");
				outReport.AddText(t);
				outReport.Add(kUChar_LineEnd);
				outReport.AddText(path);
				outReport.Add(kUChar_LineEnd);
				outReport.AddCstring("==========================================================================================\r");
				t.SetLocalizedText("ModeExecutableReport_Content");
				outReport.AddText(t);
				outReport.Add(kUChar_LineEnd);
				outReport.AddCstring("==========================================================================================\r");
				outReport.AddText(text);
				outReport.Add(kUChar_LineEnd);
				outReport.AddCstring("==========================================================================================\r");
				outReport.Add(kUChar_LineEnd);
				outReport.Add(kUChar_LineEnd);
				outReport.Add(kUChar_LineEnd);
				result = true;
			}
		}
	}
#endif
	return result;
}

/**
���s�\�t�@�C���x���\���i���[�h�ǉ����j
*/
void	AApp::SPI_ShowModeExecutableAlertWindow( const AText& inModeName, 
												const AFileAcc& inImportFile, const AText& inFileList, const AText& inReport, const ABool inConvertToAutoUpdate )//#427
{
	SPI_GetModeExecutableAlertWindow().NVI_CreateAndShow();
	SPI_GetModeExecutableAlertWindow().SPI_Set(inModeName,inImportFile,inFileList,inReport,inConvertToAutoUpdate);//#427
	SPI_GetModeExecutableAlertWindow().NVI_SwitchFocusToFirst();
	//���[�_���J�n
	SPI_GetModeExecutableAlertWindow().NVI_RunModalWindow();
	//�E�C���h�E�����
	SPI_GetModeExecutableAlertWindow().NVI_Close();
}

/**
���s�\�t�@�C���x���\���i�c�[���ǉ����j
*/
void	AApp::SPI_ShowModeExecutableAlertWindow_Tool( const AModeIndex inModeIndex, const AObjectID inToolMenuObjectID, const AIndex inRowIndex, 
													 const AFileAcc& inImportFile, const AFileAcc& inDstFile, const AText& inFileList, const AText& inReport )
{
	SPI_GetModeExecutableAlertWindow().NVI_CreateAndShow();
	SPI_GetModeExecutableAlertWindow().SPI_Set_Tool(inModeIndex,inToolMenuObjectID,inRowIndex,inImportFile,inDstFile,inFileList,inReport);
	SPI_GetModeExecutableAlertWindow().NVI_SwitchFocusToFirst();
	//���[�_���J�n
	SPI_GetModeExecutableAlertWindow().NVI_RunModalWindow();
	//�E�C���h�E�����
	SPI_GetModeExecutableAlertWindow().NVI_Close();
}

#pragma mark ===========================

#pragma mark <���L�N���X(AMultiFileFinder)>

#pragma mark ===========================

//�}���`�t�@�C�������X���b�h�擾  
AMultiFileFinder&	AApp::GetMultiFileFinder()
{
	return dynamic_cast<AMultiFileFinder&>(NVI_GetThreadByID(mMultiFileFinderObjectID));
}

//�}���`�t�@�C���������s�����ǂ�����Ԃ� 
ABool	AApp::SPI_IsMultiFileFindWorking()
{
	//#1378 �����I���C�x���g��M���ɂ܂��X���b�h�����삵�Ă���̂Ńt���O�Q�Ƃɕς���B return GetMultiFileFinder().NVI_IsThreadWorking();
	return GetMultiFileFinder().SPI_IsWorking();
}

//�}���`�t�@�C�������J�n 
void	AApp::SPI_StartMultiFileFind( const AFindParameter& inFindParam, const ADocumentID inIndexDocumentID,
									 const ABool inForMultiFileReplace, const ABool inExtractMatchedText )
{
	//�����O�ɑ��݂���O���[�v�̍ŐV��܂肽���� #0
	SPI_GetIndexWindowDocumentByID(inIndexDocumentID).SPI_CollapseFirstGroup();
	//�������s
	GetMultiFileFinder().SPNVI_Run(inFindParam,inIndexDocumentID,inForMultiFileReplace,inExtractMatchedText);
}

//�}���`�t�@�C���������f 
void	AApp::SPI_AbortMultiFileFind()
{
	GetMultiFileFinder().NVI_AbortThread();
}

//
void	AApp::SPI_SleepForAWhileMultiFileFind()
{
	GetMultiFileFinder().SPI_SleepForAWhile();
}

#pragma mark ===========================

#pragma mark <���L�N���X(AImportFileRecognizer)>

#pragma mark ===========================

//
AImportFileRecognizer&	AApp::SPI_GetImportFileRecognizer()
{
	return dynamic_cast<AImportFileRecognizer&>(NVI_GetThreadByID(mImportFileRecognizerObjectID));
}

#pragma mark ===========================

#pragma mark <���L�N���X(AFTPConnection)>

#pragma mark ===========================

//
AFTPConnection&	AApp::SPI_GetFTPConnection()
{
	return dynamic_cast<AFTPConnection&>(NVI_GetThreadByID(mFTPConnectionObjectID));
}

#if IMPLEMENTATION_FOR_MACOSX

#pragma mark ===========================

#pragma mark <���L�N���X(AAppleScriptExecuter)>

#pragma mark ===========================

void	AApp::SPI_ExecuteAppleScript( const AFileAcc& inFile )
{
	dynamic_cast<AAppleScriptExecuter&>(NVI_GetThreadByID(mAppleScriptExecuterObjectID)).SPI_Execute(inFile);
}

void	AApp::SPI_GetAppleScriptSource( const AFileAcc& inFile, AText& outText )
{
	dynamic_cast<AAppleScriptExecuter&>(NVI_GetThreadByID(mAppleScriptExecuterObjectID)).SPI_GetSource(inFile,outText);
}
#endif

#pragma mark ===========================

#pragma mark <���L�N���X(AWindow_MultiFileFind)>

#pragma mark ===========================

//�}���`�t�@�C�������E�C���h�E�\��
void	AApp::SPI_ShowMultiFileFindWindow( const AText& inFindText )
{
	SPI_GetMultiFileFindWindow().NVI_Show();
	if( inFindText.GetItemCount() > 0 )//win
	{
		SPI_GetMultiFileFindWindow().SPI_SetFindText(inFindText);
	}
	SPI_GetMultiFileFindWindow().NVI_SwitchFocusToFirst();
}

//�u�ŋߎg�����`�v�ɒǉ�
void	AApp::SPI_AddRecentlyUsed( const AText& inFindText, const AFileAcc& inFolder )
{
	SPI_GetMultiFileFindWindow().SPI_AddRecentlyUsedFindText(inFindText);
	SPI_GetMultiFileFindWindow().SPI_AddRecentlyUsedPath(inFolder);
}

//�}���`�t�@�C�������E�C���h�E�\���X�V
void	AApp::SPI_UpdateMultiFileFindWindow()
{
	SPI_GetMultiFileFindWindow().NVI_UpdateProperty();
}

#pragma mark ===========================

#pragma mark <���L�N���X(AWindow_Find)>

#pragma mark ===========================

//�����E�C���h�E�\��
void	AApp::SPI_ShowFindWindow( const AText& inFindText )
{
	SPI_GetFindWindow().NVI_Show();
	if( inFindText.GetItemCount() > 0 )//win
	{
		SPI_SetFindText(inFindText);
	}
	SPI_GetFindWindow().NVI_SwitchFocusToFirst();
}

//�����E�C���h�E�\���X�V
void	AApp::SPI_UpdateFindWindow()
{
	SPI_GetFindWindow().NVI_UpdateProperty();
}

#pragma mark ===========================

#pragma mark <���L�N���X(AWindow_ModePref)>

#pragma mark ===========================

//
void	AApp::SPI_ShowModePrefWindow( const AModeIndex inModeIndex )
{
	if( SPI_GetModePrefWindow(inModeIndex).NVI_IsWindowVisible() == false )//#858
	{
		//�E�C���h�E����
		SPI_GetModePrefWindow(inModeIndex).NVI_CreateAndShow();
		//�����E�C���h�E�����J�E���g�i�E�C���h�E�ʒu�I�t�Z�b�g�Ɏg�p����j
		AItemCount	createdWindowCount = 0;
		for( AIndex i = 0; i < mModePrefWindowIDArray.GetItemCount(); i++ )
		{
			if( mModePrefWindowIDArray.Get(i) != kObjectID_Invalid )
			{
				if( NVI_GetWindowByID(mModePrefWindowIDArray.Get(i)).NVI_IsWindowVisible() == true )
				{
					createdWindowCount++;
				}
			}
		}
		//�E�C���h�E�ʒu�ݒ�
		APoint	windowPosition = {0};
		SPI_GetModePrefWindow(inModeIndex).NVI_GetWindowPosition(windowPosition);
		windowPosition.x += 8*(createdWindowCount-1);
		windowPosition.y += 8*(createdWindowCount-1);
		SPI_GetModePrefWindow(inModeIndex).NVI_SetWindowPosition(windowPosition);
		//�ŏ��̃t�H�[�J�X�Ɉړ�
		SPI_GetModePrefWindow(inModeIndex).NVI_UpdateProperty();//#858
		SPI_GetModePrefWindow(inModeIndex).NVI_SwitchFocusToFirst();//#858
	}
	else
	{
		//�E�C���h�E�I��
		SPI_GetModePrefWindow(inModeIndex).NVI_SelectWindow();//#858
		/*if( inModeIndex != kIndex_Invalid )
		{
			SPI_GetModePrefWindow(inModeIndex).SPI_SelectMode(inModeIndex);//#858
		}*/
	}
}

//#868
/**
���[�h�ݒ�E�C���h�E�擾
*/
AWindow_ModePref&	AApp::SPI_GetModePrefWindow( const AIndex inModeIndex )
{
	//
	if( mModePrefWindowIDArray.Get(inModeIndex) == kObjectID_Invalid )
	{
		//�E�C���h�E����
		AWindowDefaultFactory<AWindow_ModePref>	modePrefWindowFactory;
		mModePrefWindowIDArray.Set(inModeIndex,NVI_CreateWindow(modePrefWindowFactory));
		SPI_GetModePrefWindow(inModeIndex).SPI_SetModeIndex(inModeIndex);
	}
	//
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ModePref,kWindowType_ModePref,mModePrefWindowIDArray.Get(inModeIndex));
}

/**
���[�h�����E�C���h�E�����ς݂��ǂ������擾
*/
ABool	AApp::SPI_IsModePrefWindowCreated( const AIndex inModeIndex ) const
{
	return (mModePrefWindowIDArray.Get(inModeIndex) != kObjectID_Invalid);
}

#pragma mark ===========================

#pragma mark --- �V�K���[�h�ǉ��E�C���h�E


#pragma mark ===========================

#pragma mark --- �T�u�E�C���h�E
//#725

/**
�T�u�E�C���h�E����
*/
AWindowID	AApp::SPI_CreateSubWindow( const ASubWindowType inType, const ANumber inParameter, const AWindowID inTextWindowID,
			const ASubWindowLocationType inLocationType, const AIndex inLocationIndex, const ABool inCollapsed )
{
	AWindowID	winID = kObjectID_Invalid;
	switch(inType)
	{
	  case kSubWindowType_CallGraf:
		{
			AWindowDefaultFactory<AWindow_CallGraf>	callGrafWindowFactory;
			winID = NVI_CreateWindow(callGrafWindowFactory);
			break;
		}
	  case kSubWindowType_Previewer:
		{
			AWindowDefaultFactory<AWindow_Previewer>	previewerWindowFactory;
			winID = NVI_CreateWindow(previewerWindowFactory);
			break;
		}
	  case kSubWindowType_ClipboardHistory:
		{
			AWindowDefaultFactory<AWindow_ClipboardHistory>	clipboardHistoryWindowFactory;
			winID = NVI_CreateWindow(clipboardHistoryWindowFactory);
			break;
		}
	  case kSubWindowType_TextMarker:
		{
			AWindowDefaultFactory<AWindow_TextMarker>	textMarkerWindowFactory;
			winID = NVI_CreateWindow(textMarkerWindowFactory);
			break;
		}
	  case kSubWindowType_KeyRecord:
		{
			AWindowDefaultFactory<AWindow_KeyRecord>	keyRecordWindowFactory;
			winID = NVI_CreateWindow(keyRecordWindowFactory);
			break;
		}
	  case kSubWindowType_DocInfo:
		{
			AWindowDefaultFactory<AWindow_DocInfo>	DocInfoWindowFactory;
			winID = NVI_CreateWindow(DocInfoWindowFactory);
			break;
		}
	  case kSubWindowType_FileList:
		{
			AWindowDefaultFactory<AWindow_FileList>	fileListWindow;
			winID = NVI_CreateWindow(fileListWindow);
			break;
		}
	  case kSubWindowType_IdInfo:
		{
			AWindowDefaultFactory<AWindow_IdInfo>	idInfoWindow;
			winID = NVI_CreateWindow(idInfoWindow);
			break;
		}
	  case kSubWindowType_JumpList:
		{
			AChildWindowFactory<AWindow_JumpList>	jumpListWindow(inTextWindowID);
			winID = NVI_CreateWindow(jumpListWindow);
			break;
		}
	  case kSubWindowType_CandidateList:
		{
			AWindowDefaultFactory<AWindow_CandidateList>	candidateListWindow;
			winID = NVI_CreateWindow(candidateListWindow);
			break;
		}
	  case kSubWindowType_KeyToolList:
		{
			AWindowDefaultFactory<AWindow_KeyToolList>	factory;
			winID = NVI_CreateWindow(factory);
			break;
		}
	  case kSubWindowType_WordsList:
		{
			AWindowDefaultFactory<AWindow_WordsList>	factory;
			winID = NVI_CreateWindow(factory);
			break;
		}
	  case kSubWindowType_FindResult:
		{
			//DocImp���� #1034
			AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_IndexWindow);
			//Index Document����
			AIndexWindowDocumentFactory	docfactory(this,docImpObjectID);//#1034
			ADocumentID	docID = GetApp().NVI_CreateDocument(docfactory);
			//�������ʃE�C���h�E����
			AFindResultWindowFactory	factory(docID);
			winID = NVI_CreateWindow(factory);
			break;
		}
	  default:
		{
			_ACError("",NULL);
			return kObjectID_Invalid;
			break;
		}
	}
	//�f�[�^�ǉ�
	mSubWindowArray_WindowID.Add(winID);
	mSubWindowArray_SubWindowType.Add(inType);
	mSubWindowArray_TextWindowID.Add(inTextWindowID);
	mSubWindowArray_SubWindowLocationType.Add(inLocationType);
	mSubWindowArray_SubWindowLocationIndex.Add(inLocationIndex);
	//�T�C�h�o�[�\���Ȃ�I�[�o�[���C���[�h�ɐݒ�
	if( inLocationType == kSubWindowLocationType_LeftSideBar || inLocationType == kSubWindowLocationType_RightSideBar ||
				inLocationType == kSubWindowLocationType_Popup )
	{
		NVI_GetWindowByID(winID).NVI_ChangeToOverlay(inTextWindowID,false);
	}
	//�ŏ��T�C�Y�ݒ�
	if( inCollapsed == false )
	{
		//�܂肽���݂ł͂Ȃ��ꍇ�̍ŏ��T�C�Y�ݒ�
		NVI_GetWindowByID(winID).NVI_SetWindowMinimumSize(kSubWindowMinWidth,SPI_GetSubWindowMinHeight(winID));
	}
	else
	{
		//�܂肽���ݎ��̍ŏ��T�C�Y�ݒ�
		NVI_GetWindowByID(winID).NVI_SetWindowMinimumSize(kSubWindowMinWidth,SPI_GetSubWindowCollapsedHeight());
	}
	//�E�C���h�E����
	if( inType == kSubWindowType_JumpList )
	{
		//------------------�W�����v���X�g�̏ꍇ------------------
		//�e�L�X�g�E�C���h�E�ɑ��݂���^�u�������Ő���
		AWindow_Text&	textWindow = GetApp().SPI_GetTextWindowByID(inTextWindowID);
		for( AIndex tabIndex = 0; tabIndex < textWindow.NVI_GetTabCount(); tabIndex++ )
		{
			GetApp().SPI_GetJumpListWindow(winID).NVI_CreateTab(textWindow.NVI_GetDocumentIDByTabIndex(tabIndex));
		}
	}
	else
	{
		//------------------����ȊO�̃E�C���h�E�̏ꍇ------------------
		NVI_GetWindowByID(winID).NVI_Create(kObjectID_Invalid);
	}
	//�p�����[�^�ݒ�
	switch( inType )
	{
	  case kSubWindowType_FileList:
		{
			SPI_GetFileListWindow(winID).SPI_ChangeMode(inParameter);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return winID;
}

/**
�T�u�E�C���h�E�폜
*/
void	AApp::SPI_DeleteSubWindow( const AWindowID inSubWindowID )
{
	//�E�C���h�E�폜
	NVI_DeleteWindow(inSubWindowID);
	
	//�f�[�^�폜
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return;}
	mSubWindowArray_WindowID.Delete1(index);
	mSubWindowArray_SubWindowType.Delete1(index);
	mSubWindowArray_TextWindowID.Delete1(index);
	mSubWindowArray_SubWindowLocationType.Delete1(index);
	mSubWindowArray_SubWindowLocationIndex.Delete1(index);
	
	//���ڑI���A�N�e�B�u�T�u�E�C���h�E���폜�����ꍇ�́A�A�N�e�B�u������
	if( inSubWindowID == mActiveSubWindowForItemSelector )
	{
		mActiveSubWindowForItemSelector = kObjectID_Invalid;
	}
}

/**
�T�u�E�C���h�E�����i�E�C���h�E�̃N���[�Y�{�^���N���b�N���Ȃǁj
*/
void	AApp::SPI_CloseSubWindow( const AWindowID inSubWindowID )
{
	//�T�u�E�C���h�Eindex����
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return;}
	
	//�e�L�X�g�E�C���h�E�ɕ������Ƃ�ʒm
	AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
	if( textWindowID != kObjectID_Invalid )
	{
		SPI_GetTextWindowByID(textWindowID).SPI_DoSubWindowClosed(inSubWindowID);
	}
	
	//�E�C���h�E�폜
	SPI_DeleteSubWindow(inSubWindowID);
}

/**
�T�u�E�C���h�E�v���p�e�B�i�\���ʒu�j�擾
*/
void	AApp::SPI_GetSubWindowProperty( const AWindowID inSubWindowID,
		ASubWindowLocationType& outLocationType, AIndex& outLocationIndex ) const
{
	//�T�u�E�C���h�Eindex����
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return;}
	//�v���p�e�B�擾
	outLocationType = mSubWindowArray_SubWindowLocationType.Get(index);
	outLocationIndex = mSubWindowArray_SubWindowLocationIndex.Get(index);
}

/**
�T�u�E�C���h�E�v���p�e�B�i�\���ʒu�j�ݒ�
*/
void	AApp::SPI_SetSubWindowProperty( const AWindowID inSubWindowID,
		const ASubWindowLocationType inLocationType, const AIndex inLocationIndex, const AWindowID inTextWindowID )
{
	//�T�u�E�C���h�Eindex����
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return;}
	//�v���p�e�B�ݒ�
	mSubWindowArray_SubWindowLocationType.Set(index,inLocationType);
	mSubWindowArray_SubWindowLocationIndex.Set(index,inLocationIndex);
	mSubWindowArray_TextWindowID.Set(index,inTextWindowID);
	
	if( inLocationType == kSubWindowLocationType_LeftSideBar || inLocationType == kSubWindowLocationType_RightSideBar ||
				inLocationType == kSubWindowLocationType_Popup )
	{
		//------------------�T�C�h�o�[�A�|�b�v�A�b�v�̏ꍇ------------------
		if( NVI_GetWindowByID(inSubWindowID).NVI_GetOverlayMode() == false )
		{
			//�^�C�v�ύX�����Ƃ��ɂ͍ŏ��T�C�Y��ʏ�ɖ߂��icollapse���̍ŏ��T�C�Y���������邽�߁j
			NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(kSubWindowMinWidth,SPI_GetSubWindowMinHeight(inSubWindowID));
			//�I�[�o�[���C�E�C���h�E�ɐݒ�
			NVI_GetWindowByID(inSubWindowID).NVI_ChangeToOverlay(inTextWindowID,false);
			//�\���X�V
			NVI_GetWindowByID(inSubWindowID).NVI_UpdateProperty();
			NVI_GetWindowByID(inSubWindowID).NVI_RefreshWindow();
		}
	}
	else
	{
		//------------------�t���[�e�B���O�̏ꍇ------------------
		if( NVI_GetWindowByID(inSubWindowID).NVI_GetOverlayMode() == true )
		{
			//�^�C�v�ύX�����Ƃ��ɂ͍ŏ��T�C�Y��ʏ�ɖ߂��icollapse���̍ŏ��T�C�Y���������邽�߁j
			NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(kSubWindowMinWidth,SPI_GetSubWindowMinHeight(inSubWindowID));
			//�t���[�e�B���O�E�C���h�E�ɐݒ�
			NVI_GetWindowByID(inSubWindowID).NVI_ChangeToFloating();
			//�\���X�V
			NVI_GetWindowByID(inSubWindowID).NVI_UpdateProperty();
			NVI_GetWindowByID(inSubWindowID).NVI_RefreshWindow();
		}
	}
}

/**
�T�C�h�o�[�̍ŏI���ڂ��ǂ������擾
*/
ABool	AApp::SPI_IsSubWindowSideBarBottom( const AWindowID inSubWindowID ) const
{
	//�T�u�E�C���h�Eindex����
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return false;}
	//�v���p�e�B�擾
	AWindowID	winID = mSubWindowArray_WindowID.Get(index);
	AWindowID	textWinID = mSubWindowArray_TextWindowID.Get(index);
	if( textWinID == kObjectID_Invalid )
	{
		return false;
	}
	//�ŏI���ڂ��ǂ������擾
	switch(mSubWindowArray_SubWindowLocationType.Get(index))
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			AItemCount	itemCount = GetApp().SPI_GetTextWindowByID(textWinID).SPI_GetLeftSideBarItemCount();
			if( mSubWindowArray_SubWindowLocationIndex.Get(index) == itemCount-1 )
			{
				return true;
			}
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			AItemCount	itemCount = GetApp().SPI_GetTextWindowByID(textWinID).SPI_GetRightSideBarItemCount();
			if( mSubWindowArray_SubWindowLocationIndex.Get(index) == itemCount-1 )
			{
				return true;
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return false;
}

/**
�T�u�E�C���h�E�\���ʒu�^�C�v�擾
*/
ASubWindowLocationType	AApp::SPI_GetSubWindowLocationType( const AWindowID inSubWindowID ) const
{
	//�T�u�E�C���h�Eindex����
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return kSubWindowLocationType_None;}
	//�\���ʒu�^�C�v�擾
	return mSubWindowArray_SubWindowLocationType.Get(index);
}


/**
�T�u�E�C���h�E�^�C�v�擾
*/
ASubWindowType	AApp::SPI_GetSubWindowType( const AWindowID inSubWindowID ) const
{
	//�T�u�E�C���h�Eindex����
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return kSubWindowType_None;}
	//�^�C�v�擾
	return mSubWindowArray_SubWindowType.Get(index);
}

/**
�T�u�E�C���h�E�p�����[�^�擾
*/
ANumber	AApp::SPI_GetSubWindowParameter( const AWindowID inSubWindowID ) const
{
	//�T�u�E�C���h�Eindex����
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return 0;}
	//�p�����[�^�擾
	ANumber	parameter = 0;
	AWindowID	winID = mSubWindowArray_WindowID.Get(index);
	switch(mSubWindowArray_SubWindowType.Get(index))
	{
	  case kSubWindowType_FileList:
		{
			parameter = SPI_GetFileListWindowConst(winID).SPI_GetCurrentMode();
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return parameter;
}

/**
�I�[�o�[���C�T�u�E�C���h�E�z�u
*/
void	AApp::SPI_UpdateLayoutOverlaySubWindow( const AWindowID inTextWindowID, const ASubWindowType inType, const AWindowID inSubWindowID,
		const AWindowPoint inPoint, const ANumber inWidth, const ANumber inHeight, const ABool inAnimate )
{
	NVI_GetWindowByID(inSubWindowID).NVI_Show(false);
	NVI_GetWindowByID(inSubWindowID).NVI_SetOffsetOfOverlayWindowAndSize(inTextWindowID,inPoint,inWidth,inHeight,inAnimate);
}

/**
�I�[�o�[���C�T�u�E�C���h�E���ڈړ�
*/
void	AApp::SPI_MoveOverlaySubWindow( const ASubWindowLocationType inSrcType, const AIndex inSrcIndex,
		const ASubWindowLocationType inDstType, const AIndex inDstIndex )
{
	//�S�Ẵe�L�X�g�E�C���h�E�ɓK�p����
	for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
	{
		//�I�[�o�[���C�T�u�E�C���h�E�ړ�
		SPI_GetTextWindowByID(winID).SPI_MoveOverlaySubWindow(inSrcType,inSrcIndex,inDstType,inDstIndex);
	}
}

/**
�I�[�o�[���C�T�u�E�C���h�E�ǉ�
*/
void	AApp::SPI_AddOverlaySubWindow( const ASubWindowLocationType inLocationType, const AIndex inIndex,
		const ASubWindowType inSubWindowType )
{
	//�S�Ẵe�L�X�g�E�C���h�E�ɓK�p����
	for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
	{
		//�I�[�o�[���C�T�u�E�C���h�E�ǉ�
		SPI_GetTextWindowByID(winID).SPI_AddOverlaySubWindow(inLocationType,inIndex,inSubWindowType);
	}
}

/**
�T�u�E�C���h�E�t�H���g�擾
*/
void	AApp::SPI_GetSubWindowsFont( AText& outFontName, AFloatNumber& outFontSize ) const
{
	NVI_GetAppPrefDBConst().GetData_Text(AAppPrefDB::kSubWindowsFontName,outFontName);
	outFontSize = NVI_GetAppPrefDBConst().GetData_FloatNumber(AAppPrefDB::kSubWindowsFontSize);
}

/**
�T�u�E�C���h�E�p�w�i�F�擾
*/
AColor	AApp::SPI_GetSubWindowBackgroundColor( const ABool inActive ) const
{
	/*#1316
	if( inActive == true )
	{
		return mSkinColor_SubWindowBackgroundColor;
	}
	else
	{
		return mSkinColor_SubWindowBackgroundColor;
		//#1275 return mSkinColor_SubWindowBackgroundColorDeactive;
	}
	*/
	//
	if( GetApp().NVI_IsDarkMode() == false )
	{
		return AColorWrapper::GetColorByHTMLFormatColor("F2F2F2");
	}
	else
	{
		return AColorWrapper::GetColorByHTMLFormatColor("303030");
	}
}

/**
�T�u�E�C���h�E���{�b�N�X�F�擾
���|�b�v�A�b�v�E�C���h�E�̐F�Ɏg�p����B�i�e�L�X�g�G���A�̐F�Ɏ����F��Ԃ��B�j
*/
void	AApp::SPI_GetSubWindowBoxColor( const AWindowID inSubWindowID, 
										AColor& outLetterColor, //#1316 AColor& outDropShadowColor,
									   AColor& outBoxColor1, AColor& outBoxColor2, AColor& outBoxColor3 ) const
{
	//�|�b�v�A�b�v�E�C���h�E�ȊO�̏ꍇ�́A�����F�F���A�{�b�N�X�F��
	outLetterColor = AColorWrapper::GetColorByHTMLFormatColor("303030");//#1316 mSkinColor_SubWindowBoxLetterColor;
	//#1316 outDropShadowColor = mSkinColor_SubWindowBoxLetterDropShadowColor;
	AColor	boxBaseColor = AColorWrapper::GetColorByHTMLFormatColor("F2F2F2");//#1316 mSkinColor_SubWindowBoxBackgroundColor;
	//
	if( GetApp().NVI_IsDarkMode() == true )
	{
		outLetterColor = AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
		boxBaseColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
	}
	//�|�b�v�A�b�v�E�C���h�E�̏ꍇ�́A���[�h�ݒ�̕����F�A�w�i�F�ɏ]��
	if( GetApp().SPI_GetSubWindowLocationType(inSubWindowID) == kSubWindowLocationType_Popup )
	{
		AWindowID	textWindowID = GetApp().SPI_GetSubWindowParentTextWindow(inSubWindowID);
		if( textWindowID != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetTextWindowByID(textWindowID).NVI_GetCurrentDocumentID() != kObjectID_Invalid )
			{
				AModeIndex	modeIndex = GetApp().SPI_GetTextWindowByID(textWindowID).SPI_GetCurrentFocusTextDocument().SPI_GetModeIndex();
				//�w�i�F
				GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kBackgroundColor,boxBaseColor);
				//�����F
				GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kLetterColor,outLetterColor);
			}
		}
	}
	outBoxColor1 = AColorWrapper::ChangeHSV(boxBaseColor,0,1.0,0.99);//#1316 1.2);
	outBoxColor2 = AColorWrapper::ChangeHSV(boxBaseColor,0,1.0,0.99);//#1316 0.95);
	outBoxColor3 = AColorWrapper::ChangeHSV(boxBaseColor,0,1.0,0.99);//#1316 0.93);
}

/**
�T�u�E�C���h�Erounded rect�I�����͂ݐF�擾
*/
AColor	AApp::SPI_GetSubWindowRoundedRectBoxSelectionColor( const AWindowID inSubWindowID ) const
{
	if( inSubWindowID == mActiveSubWindowForItemSelector )
	{
		return kColor_Blue;
	}
	else
	{
		return kColor_Gray20Percent;
	}
}

/**
�T�u�E�C���h�Erounded rect�z�o�[���͂ݐF�擾
*/
AColor	AApp::SPI_GetSubWindowRoundedRectBoxHoverColor() const
{
	return kColor_Blue;
}

/**
�T�u�E�C���h�E�W�������F�擾
*/
AColor	AApp::SPI_GetSubWindowLetterColor() const
{
	//#1316 return mSkinColor_SubWindowLetterColor;
	if( GetApp().NVI_IsDarkMode() == false )
	{
		return AColorWrapper::GetColorByHTMLFormatColor("303030");
	}
	else
	{
		return AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
	}
}

/**
�T�C�h�o�[�t���[���F�擾
*/
AColor	AApp::SPI_GetSideBarFrameColor() const
{
	//#1316 return AColorWrapper::GetColorByHTMLFormatColor("b4b4b4");//"686b6e");
	if( GetApp().NVI_IsDarkMode() == false )
	{
		return AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
	}
	else
	{
		return AColorWrapper::GetColorByHTMLFormatColor("000000");
	}
}

/**
�T�C�h�o�[�X�y�[�X�w�i�F�擾
*/
/*
AColor	AApp::SPI_GetSideBarEmptySpaceColor() const
{
	//����ς�Â���ۂ��󂯂�̂ł�߂�return AColorWrapper::GetColorByHTMLFormatColor("a8acb1");
	return AColorWrapper::GetColorByHTMLFormatColor("e9eef4");
}
*/

/**
�T�u�E�C���h�E�w�b�_�[�����w�i�F�擾
*/
AColor	AApp::SPI_GetSubWindowHeaderBackgroundColor() const
{
	//#1316 return mSkinColor_SubWindowHeaderBackgroundColor;
	if( GetApp().NVI_IsDarkMode() == false )
	{
		return AColorWrapper::GetColorByHTMLFormatColor("F2F2F2");
	}
	else
	{
		return AColorWrapper::GetColorByHTMLFormatColor("303030");
	}
}

/**
�T�u�E�C���h�E�w�b�_�[���������F�擾
*/
/*#1316
AColor	AApp::SPI_GetSubWindowHeaderLetterColor() const
{
	return mSkinColor_SubWindowHeaderLetterColor;
}
*/

/**
�T�u�E�C���h�E�p�^�C�g���o�[�����F�擾
*/
AColor	AApp::SPI_GetSubWindowTitlebarTextColor() const
{
	//#1316 return mSkinColor_SubWindowTitlebarTextColor;
	if( GetApp().NVI_IsDarkMode() == false )
	{
		return AColorWrapper::GetColorByHTMLFormatColor("303030");
	}
	else
	{
		return AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
	}
}

/**
�T�u�E�C���h�E�p�^�C�g���o�[�����h���b�v�V���h�E�F�擾
*/
/*#1316
AColor	AApp::SPI_GetSubWindowTitlebarTextDropShadowColor() const
{
	return mSkinColor_SubWindowTitlebarTextDropShadowColor;
}
*/

/**
�T�u�E�C���h�E�^�C�g���̃{�^���z�o�[�F�擾
*/
AColor	AApp::SPI_GetSubWindowTitlebarButtonHoverColor() const
{
	return AColorWrapper::GetColorByHTMLFormatColor("778c9c");
}

/**
�|�b�v�A�b�v�T�u�E�C���h�E���擾
*/
AFloatNumber	AApp::SPI_GetPopupWindowAlpha() const
{
	AFloatNumber	alpha = NVI_GetAppPrefDBConst().GetData_Number(AAppPrefDB::kSubWindowsAlpha_Popup);
	return alpha/100;
}

/**
�t���[�e�B���O�T�u�E�C���h�E���擾
*/
AFloatNumber	AApp::SPI_GetFloatingWindowAlpha() const
{
	AFloatNumber	alpha = NVI_GetAppPrefDBConst().GetData_Number(AAppPrefDB::kSubWindowsAlpha_Floating);
	return alpha/100;
}

/**
�T�u�E�C���h�E�^�C�g���o�[�����擾
*/
ANumber	AApp::SPI_GetSubWindowTitleBarHeight() const
{
	return 19;
}

/**
�T�u�E�C���h�E�����������擾
*/
ANumber	AApp::SPI_GetSubWindowSeparatorHeight() const
{
	return 11;
}

/**
�T�u�E�C���h�E�ŏ������擾
*/
ANumber	AApp::SPI_GetSubWindowMinHeight( const AWindowID inSubWindowID ) 
{
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return 30;}
	//
	ANumber	minHeight = 30;
	switch(mSubWindowArray_SubWindowType.Get(index))
	{
	  case kSubWindowType_CallGraf:
		{
			minHeight = SPI_GetCallGrafWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_Previewer:
		{
			minHeight = SPI_GetPreviewerWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_ClipboardHistory:
		{
			minHeight = SPI_GetClipboardHistoryWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_TextMarker:
		{
			minHeight = SPI_GetTextMarkerWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_KeyRecord:
		{
			minHeight = SPI_GetKeyRecordWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_DocInfo:
		{
			minHeight = SPI_GetDocInfoWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_FileList:
		{
			minHeight = SPI_GetFileListWindowConst(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_IdInfo:
		{
			minHeight = SPI_GetIdInfoWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_JumpList:
		{
			minHeight = SPI_GetJumpListWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_CandidateList:
		{
			minHeight = SPI_GetCandidateListWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_KeyToolList:
		{
			minHeight = SPI_GetKeyToolListWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_WordsList:
		{
			minHeight = SPI_GetWordsListWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  case kSubWindowType_FindResult:
		{
			minHeight = SPI_GetFindResultWindow(inSubWindowID).SPI_GetSubWindowMinHeight();
			break;
		}
	  default:
		{
			_ACError("",NULL);
			break;
		}
	}
	return minHeight;
}

/**
�S�ẴT�u�E�C���h�E�\���X�V
*/
void	AApp::SPI_UpdateSubWindowsProperty()
{
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		NVI_GetWindowByID(winID).NVI_UpdateProperty();
		NVI_GetWindowByID(winID).NVI_RefreshWindow();
	}
}

/**
�ΏۃT�u�E�C���h�E���ǂ����𔻒肷��
�|�b�v�A�b�v�E�t���[�e�B���O�E�C���h�E�͏�ɑΏہB�I�[�o�[���C�E�C���h�E�͎w��text window�ɕ\������Ă���ꍇ�̂ݑΏہB
*/
ABool	AApp::JudgeIfTargetSubWindowByTextWindowID( const AWindowID inSubWindowID, const AWindowID inTextWindowID ) 
{
	//�T�u�E�C���h�E����
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return false;}
	
	//�|�b�v�A�b�v�E�t���[�e�B���O�E�C���h�E�͏�ɑΏہB�I�[�o�[���C�E�C���h�E�͎w��text window�ɕ\������Ă���ꍇ�̂ݑΏہB
	switch(mSubWindowArray_SubWindowLocationType.Get(index))
	{
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//�T�u�E�C���h�E�̑�����e�L�X�g�E�C���h�E���A�w��E�C���h�E�ƈ�v���Ă�����ΏۂƂ���B
			if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
	  default:
		{
			//��ɑΏہB
			return true;
			break;
		}
	}
}

/**
�ΏۃT�u�E�C���h�E���ǂ����𔻒肷��
�|�b�v�A�b�v�E�t���[�e�B���O�E�C���h�E�͏�ɑΏہB�I�[�o�[���C�E�C���h�E�͎w��text document���\������Ă���ꍇ�̂ݑΏہB
*/
ABool	AApp::JudgeIfTargetSubWindowByTextDocumentID( const AWindowID inSubWindowID, const ADocumentID inTextDocumentID ) 
{
	//�T�u�E�C���h�E����
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return false;}
	
	//�|�b�v�A�b�v�E�t���[�e�B���O�E�C���h�E�͏�ɑΏہB�I�[�o�[���C�E�C���h�E�͎w��text document���\������Ă���ꍇ�̂ݑΏہB
	switch(mSubWindowArray_SubWindowLocationType.Get(index))
	{
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//�T�u�E�C���h�E�̑�����e�L�X�g�E�C���h�E�̌��݂̃h�L�������g���A�w��h�L�������g�ƈ�v���Ă�����ΏۂƂ���B
			AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
			if( SPI_GetTextWindowByID(textWindowID).NVI_GetCurrentDocumentID() == inTextDocumentID )
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		}
	  default:
		{
			//��ɑΏہB
			return true;
			break;
		}
	}
}

/**
�e�T�u�E�C���h�E�̃��[�h�֘A�f�[�^���폜����
*/
void	AApp::SPI_ClearSubWindowsModeRelatedData( const AIndex inModeIndex )
{
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		switch( mSubWindowArray_SubWindowType.Get(index) )
		{
		  case kSubWindowType_CallGraf:
			{
				SPI_GetCallGrafWindow(winID).SPI_GetCallGrafView().SPI_DeleteAllData();
				break;
			}
		  case kSubWindowType_IdInfo:
			{
				SPI_GetIdInfoWindow(winID).SPI_GetIdInfoView().SPI_DeleteAllIdInfo();
				break;
			}
		  case kSubWindowType_CandidateList:
			{
				SPI_GetCandidateListWindow(winID).SPI_GetCandidateListView().SPI_DeleteAllData();
				break;
			}
		  case kSubWindowType_KeyToolList:
			{
				//��
				break;
			}
		  case kSubWindowType_WordsList:
			{
				SPI_GetWordsListWindow(winID).SPI_GetWordsListView().SPI_DeleteAllData();
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
�t���[�e�B���O�T�u�E�C���h�E����
*/
AWindowID	AApp::SPI_CreateFloatingSubWindow( const ASubWindowType inSubWindowType, const ANumber inParameter, 
			const ANumber inWidth, const ANumber inHeight )
{
	//�T�u�E�C���h�E����
	AWindowID	winID = SPI_CreateSubWindow(inSubWindowType,inParameter,kObjectID_Invalid,kSubWindowLocationType_Floating,kIndex_Invalid,false);
	if( winID == kObjectID_Invalid )
	{
		return kObjectID_Invalid;
	}
	//�E�C���h�E�z�u
	AGlobalRect	screenRect = {0};
	AWindow::NVI_GetAvailableWindowBoundsInMainScreen(screenRect);
	ARect	rect = {0};
	rect.left		= screenRect.left + 16;
	rect.top		= screenRect.top + 16;
	rect.right		= rect.left + inWidth;
	rect.bottom		= rect.top + inHeight;
	NVI_GetWindowByID(winID).NVI_SetWindowBounds(rect);
	//�E�C���h�E�����E�\��
	NVI_GetWindowByID(winID).NVI_CreateAndShow(false);
	//
	return winID;
}

/**
�t���[�e�B���O�T�u�E�C���h�E�ꊇ�\���E��\���ؑ�
*/
void	AApp::SPI_ShowHideFloatingSubWindowsTemporary()
{
	//�\���E��\���ؑ�
	mShowFloatingSubWindow = !mShowFloatingSubWindow;
	//�t���[�e�B���O�E�C���h�E�\����ԍX�V #959
	SPI_UpdateFloatingSubWindowsVisibility();
	//�c�[���o�[�̕\���X�V
	SPI_UpdateToolbarItemValueAll();
}

//#959
/**
�t���[�e�B���O�E�C���h�E�\����ԍX�V
*/
void	AApp::SPI_UpdateFloatingSubWindowsVisibility()
{
	ABool	shouldShow = mShowFloatingSubWindow;
	//�őO�ʂ̃E�C���h�E�̃^�C�v���e�L�X�g�E�C���h�E�ȊO�̂Ƃ��̓t���[�e�B���O�E�C���h�E�͕\�����Ȃ��B
	AWindowID	winID = NVI_GetMostFrontWindowID(kWindowType_Invalid,false);
	if( winID != kObjectID_Invalid )
	{
		if( NVI_GetWindowByID(winID).NVI_GetWindowType() != kWindowType_Text )
		{
			shouldShow = false;
		}
	}
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Floating &&
			mSubWindowArray_SubWindowType.Get(index) != kSubWindowType_FindResult )//#1336 �������ʃE�C���h�E�͕\����ԍX�V�ΏۊO�Ƃ���
		{
			//�\���E��\���ݒ�
			if( shouldShow == true )
			{
				NVI_GetWindowByID(winID).NVI_Show(false);
			}
			else
			{
				NVI_GetWindowByID(winID).NVI_Hide();
			}
		}
	}
}

/**
�t���[�e�B���O�T�u�E�C���h�E��app pref�ɏ]���ĊJ��
*/
void	AApp::ReopenFloatingSubWindows()
{
	//�e�ݒ薈�̃��[�v
	AItemCount	itemCount = NVI_GetAppPrefDB().GetItemCount_Array(AAppPrefDB::kFloatingSubWindowArray_Type);
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//�E�C���h�E����
		ASubWindowType	type = (ASubWindowType)(NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Type,i));
		ANumber	param = NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Parameter,i);
		AWindowID	winID = SPI_CreateSubWindow(type,param,kObjectID_Invalid,kSubWindowLocationType_Floating,kIndex_Invalid,false);
		if( winID != kObjectID_Invalid )
		{
			//�\���ʒu�擾�A�ݒ�
			ARect	rect = {0};
			rect.left 	= NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_X,i);
			rect.top 	= NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Y,i);
			rect.right	= rect.left + NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Width,i);
			rect.bottom	= rect.top  + NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Height,i);
			NVI_GetWindowByID(winID).NVI_SetWindowBounds(rect);
			//�ʒu�␳
			NVI_GetWindowByID(winID).NVI_ConstrainWindowPosition(false);
			//�\��
			NVI_GetWindowByID(winID).NVI_Show(false);
		}
		//���݂̃p�X��ݒ�
		AText	currentPath;
		NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFloatingSubWindowArray_CurrentPath,i,currentPath);
		switch(type)
		{
		  case kSubWindowType_FileList:
			{
				SPI_GetFileListWindow(winID).SPI_SetCurrentPath(currentPath);
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
�t���[�e�B���O�T�u�E�C���h�E��app pref�ɕۑ�
*/
void	AApp::SaveFloatingSubWindows()
{
	//�f�[�^�폜
	NVI_GetAppPrefDB().DeleteAllRows_Table(AAppPrefDB::kFloatingSubWindowArray_Type);
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		if( mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Floating &&
					mSubWindowArray_SubWindowType.Get(index) != kSubWindowType_JumpList &&
					mSubWindowArray_SubWindowType.Get(index) != kSubWindowType_FindResult )
		{
			//�E�C���h�E�̃f�[�^�擾
			AWindowID	winID = mSubWindowArray_WindowID.Get(index);
			ANumber	param = SPI_GetSubWindowParameter(winID);
			ANumber	type = mSubWindowArray_SubWindowType.Get(index);
			ARect	rect = {0};
			NVI_GetWindowByID(winID).NVI_GetWindowBounds(rect);
			//���݂̃p�X���擾
			AText	currentPath;
			switch(type)
			{
			  case kSubWindowType_FileList:
				{
					SPI_GetFileListWindow(winID).SPI_GetCurrentPath(currentPath);
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
			//DB�ɐݒ�
			NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Type,type);
			NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Parameter,param);
			NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Width,rect.right-rect.left);
			NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Height,rect.bottom-rect.top);
			NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kFloatingSubWindowArray_X,rect.left);
			NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Y,rect.top);
			NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kFloatingSubWindowArray_CurrentPath,currentPath);
		}
	}
}

/**
�t���[�e�B���O�T�u�E�C���h�E�̋z��
*/
void	AApp::SPI_DockSubWindowsToSubWindows( ARect& ioWindowRect ) const
{
	//
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		if( mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Floating )
		{
			AWindowID	winID = mSubWindowArray_WindowID.Get(index);
			ARect	rect = {0};
			NVI_GetWindowConstByID(winID).NVI_GetWindowBounds(rect);
			//�㉺�z��
			if( (ioWindowRect.left >= rect.left && ioWindowRect.left <= rect.right) ||
						(ioWindowRect.right >= rect.left && ioWindowRect.right <= rect.right) )
			{
				ABool	docked = false;
				//��z��
				if( abs(ioWindowRect.bottom - rect.top) < 10 )
				{
					ioWindowRect.top = rect.top - (ioWindowRect.bottom-ioWindowRect.top);
					ioWindowRect.bottom = rect.top;
					//
					docked = true;
				}
				//���z��
				if( abs(ioWindowRect.top - rect.bottom) < 10 )
				{
					ioWindowRect.bottom = rect.bottom + (ioWindowRect.bottom-ioWindowRect.top);
					ioWindowRect.top = rect.bottom;
					//
					docked = true;
				}
				//���E�[�z��
				if( docked == true )
				{
					//�E�[�z��
					if( abs(ioWindowRect.right - rect.right) < 10 )
					{
						ioWindowRect.left = rect.right - (ioWindowRect.right-ioWindowRect.left);
						ioWindowRect.right = rect.right;
					}
					//���[�z��
					if( abs(ioWindowRect.left - rect.left) < 10 )
					{
						ioWindowRect.right = rect.left + (ioWindowRect.right-ioWindowRect.left);
						ioWindowRect.left = rect.left;
					}
				}
			}
			//���E�z��
			if( (ioWindowRect.top >= rect.top && ioWindowRect.top <= rect.bottom) ||
						(ioWindowRect.bottom >= rect.top && ioWindowRect.bottom <= rect.bottom) )
			{
				ABool	docked = false;
				//���z��
				if( abs(ioWindowRect.right - rect.left) < 10 )
				{
					ioWindowRect.left = rect.left - (ioWindowRect.right-ioWindowRect.left);
					ioWindowRect.right = rect.left;
					//
					docked = true;
				}
				//��z��
				if( abs(ioWindowRect.left - rect.right) < 10 )
				{
					ioWindowRect.right = rect.right + (ioWindowRect.right-ioWindowRect.left);
					ioWindowRect.left = rect.right;
					//
					docked = true;
				}
				//�㉺�[�z��
				if( docked == true )
				{
					//���[�z��
					if( abs(ioWindowRect.bottom - rect.bottom) < 10 )
					{
						ioWindowRect.top = rect.bottom - (ioWindowRect.bottom-ioWindowRect.top);
						ioWindowRect.bottom = rect.bottom;
					}
					//��[�z��
					if( abs(ioWindowRect.top - rect.top) < 10 )
					{
						ioWindowRect.bottom = rect.top + (ioWindowRect.bottom-ioWindowRect.top);
						ioWindowRect.top = rect.top;
					}
				}
			}
		}
	}
}

/**
�t���[�e�B���O�T�u�E�C���h�E��z-order�̕␳
���̂ق��̃E�C���h�E�قǑO�ʂɗ���悤�ɂ���
*/
void	AApp::SPI_AdjustAllFlotingWindowsZOrder()
{
	//z-order���ɂ��̕ϐ��Ɋi�[���Ă���
	AArray<AWindowID>	floatingWindowOrderArray;
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		if( mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Floating )
		{
			AWindowID	winID = mSubWindowArray_WindowID.Get(index);
			//�T�u�E�C���h�E���t���[�e�B���O�A���A�\�����Ȃ珈�����s��
			if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
			{
				//bounds�擾
				ARect	rect = {0};
				NVI_GetWindowConstByID(winID).NVI_GetWindowBounds(rect);
				
				//floatingWindowOrderArray�̒��ōŏ���y�ʒu���傫���Ȃ鍀�ڂ̈ʒu�ɒǉ�
				//�iy�ʒu�����������ɕ��ׂ�j
				AItemCount	orderItemCount = floatingWindowOrderArray.GetItemCount();
				AIndex	insertIndex = orderItemCount;
				for( AIndex j = 0; j < orderItemCount; j++ )
				{
					AWindowID	w = floatingWindowOrderArray.Get(j);
					ARect	r = {0};
					NVI_GetWindowConstByID(w).NVI_GetWindowBounds(r);
					//�ŏ���y�ʒu���傫���Ȃ鍀��index���擾
					if( r.top > rect.top )
					{
						insertIndex = j;
						break;
					}
				}
				floatingWindowOrderArray.Insert1(insertIndex,winID);
			}
		}
	}
	//��Ŏ擾����z-order�ɏ]���āA���ɍőO�ʂɂ���
	for( AIndex index = 0; index < floatingWindowOrderArray.GetItemCount(); index++ )
	{
		NVI_GetWindowByID(floatingWindowOrderArray.Get(index)).NVI_SendAbove(kObjectID_Invalid);
	}
}

/**
���ڑI���A�N�e�B�u�T�u�E�C���h�E��ݒ�icmd+option+�����̑ΏۃE�C���h�E��ݒ�j
*/
void	AApp::SPI_SetActiveSubWindowForItemSelector( const AWindowID inWindowID )
{
	//���ڑI���A�N�e�B�u�T�u�E�C���h�E��ݒ�
	mActiveSubWindowForItemSelector = inWindowID;
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		switch(mSubWindowArray_SubWindowType.Get(index) )
		{
		  case kSubWindowType_WordsList:
		  case kSubWindowType_CallGraf:
		  case kSubWindowType_FindResult:
			{
				//�E�C���h�E�\�����Ȃ�`��X�V�i�I��F��ς��邽�߁j
				AWindowID	winID = mSubWindowArray_WindowID.Get(index);
				if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
				{
					NVI_GetWindowByID(winID).NVI_RefreshWindow();
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

/**
���ڑI���A�N�e�B�u�E�C���h�E�ɂāA���̍��ڂֈړ�
*/
void	AApp::SPI_SelectNextItemInSubWindow()
{
	AIndex	index = mSubWindowArray_WindowID.Find(mActiveSubWindowForItemSelector);
	if( index != kIndex_Invalid )
	{
		switch(mSubWindowArray_SubWindowType.Get(index) )
		{
		  case kSubWindowType_WordsList:
			{
				SPI_GetWordsListWindow(mActiveSubWindowForItemSelector).SPI_GetWordsListView().SPI_SelectNextItem();
				break;
			}
		  case kSubWindowType_CallGraf:
			{
				SPI_GetCallGrafWindow(mActiveSubWindowForItemSelector).SPI_GetCallGrafView().SPI_SelectNextItem();
				break;
			}
		  case kSubWindowType_FindResult:
			{
				AViewID	findResultViewID = SPI_GetFindResultWindow(mActiveSubWindowForItemSelector).SPI_GetIndexViewID();
				AView_Index::GetIndexViewByViewID(findResultViewID).SPI_SelectNextRow();
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
���ڑI���A�N�e�B�u�E�C���h�E�ɂāA�O�̍��ڂֈړ�
*/
void	AApp::SPI_SelectPreviousItemInSubWindow()
{
	AIndex	index = mSubWindowArray_WindowID.Find(mActiveSubWindowForItemSelector);
	if( index != kIndex_Invalid )
	{
		switch(mSubWindowArray_SubWindowType.Get(index) )
		{
		  case kSubWindowType_WordsList:
			{
				SPI_GetWordsListWindow(mActiveSubWindowForItemSelector).SPI_GetWordsListView().SPI_SelectPreviousItem();
				break;
			}
		  case kSubWindowType_CallGraf:
			{
				SPI_GetCallGrafWindow(mActiveSubWindowForItemSelector).SPI_GetCallGrafView().SPI_SelectPreviousItem();
				break;
			}
		  case kSubWindowType_FindResult:
			{
				AViewID	findResultViewID = SPI_GetFindResultWindow(mActiveSubWindowForItemSelector).SPI_GetIndexViewID();
				AView_Index::GetIndexViewByViewID(findResultViewID).SPI_SelectPreviousRow();
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
�e�T�u�E�C���h�E���b�N����
*/
void	AApp::SPI_ClearAllLockOfSubWindows()
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		switch(mSubWindowArray_SubWindowType.Get(index) )
		{
		  case kSubWindowType_WordsList:
			{
				SPI_GetWordsListWindow(winID).SPI_SetLockedMode(false);
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

#pragma mark ===========================

#pragma mark --- AWindow_CallGraf

/**
CallGraf�E�C���h�E�Ɍ���function��ݒ�
*/
void	AApp::SPI_SetCallGrafCurrentFunction( const AWindowID inTextWindowID, const AFileAcc& inProjectFolder,
		const AIndex inModeIndex, const AIndex inCategoryIndex, 
		const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
		const AIndex inStartIndex, const AIndex inEndIndex, const ABool inByEdit )
{
	//�eCallGraf�E�C���h�E�̂����ATextWindowID���ʒu������́i�I�[�o�[���C�j�A����сATextWindowID�����ݒ�i�t���[�e�B���O�j�̂��̂ɓK�p
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CallGraf )
			{
				AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
				if( textWindowID == inTextWindowID || textWindowID == kObjectID_Invalid )
				{
					//����function��ݒ�
					SPI_GetCallGrafWindow(winID).SPI_GetCallGrafView().SPI_SetCurrentFunction(inProjectFolder,
								inModeIndex,inCategoryIndex,
								inFilePath,inClassName,inFunctionName,inStartIndex,inEndIndex,inByEdit);
				}
			}
		}
	}
}

/**
CallGraf�E�C���h�E�Ɍ���function��ݒ�
*/
void	AApp::SPI_SetCallGrafEditFlag( const AWindowID inTextWindowID, 
									  const AText& inFilePath, const AText& inClassName, const AText& inFunctionName )
{
	//�eCallGraf�E�C���h�E�̂����ATextWindowID���ʒu������́i�I�[�o�[���C�j�A����сATextWindowID�����ݒ�i�t���[�e�B���O�j�̂��̂ɓK�p
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CallGraf )
			{
				AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
				if( textWindowID == inTextWindowID || textWindowID == kObjectID_Invalid )
				{
					//����function��ݒ�
					SPI_GetCallGrafWindow(winID).SPI_GetCallGrafView().SPI_SetEditFlag(inFilePath,inClassName,inFunctionName);
				}
			}
		}
	}
}

/**
IdInfo�ɂ��W�����v��CallGraf�ɒʒm
*/
void	AApp::SPI_NotifyToCallGrafByIdInfoJump( const AText& inCallerFuncIdText, 
		const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
		const AIndex inStartIndex, const AIndex inEndIndex )
{
	//�eCallGraf�E�C���h�E�ɒʒm
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CallGraf )
			{
				SPI_GetCallGrafWindow(winID).SPI_GetCallGrafView().SPI_NotifyToCallGrafByIdInfoJump(inCallerFuncIdText,
							inFilePath,inClassName,inFunctionName,inStartIndex,inEndIndex);
			}
		}
	}
}

/**
�h�L�������g�ҏW���eCallGraf�ɒʒm
*/
void	AApp::SPI_NotifyToCallGrafByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	//�eCallGraf�E�C���h�E�ɒʒm
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CallGraf )
			{
				SPI_GetCallGrafWindow(winID).SPI_GetCallGrafView().SPI_NotifyToCallGrafByTextDocumentEdited(inFile,inTextIndex,inInsertedCount);
			}
		}
	}
}

/**
CallGraf�E�C���h�E�ɃC���|�[�g�t�@�C��progress�\���E��\��
*/
void	AApp::SPI_ShowHideImportFileProgressInCallGrafWindow( const ABool inShow )
{
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CallGraf )
			{
				SPI_GetCallGrafWindow(winID).SPI_ShowHideImportFileProgress(inShow);
			}
		}
	}
}

/**
CallGraf�E�C���h�E�̌��݂̌����𒆎~����
*/
void	AApp::SPI_AbortCurrentWordsListFinderRequestInCallGrafWindow()
{
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CallGraf )
			{
				SPI_GetCallGrafWindow(winID).SPI_GetCallGrafView().SPI_AbortCurrentWordsListFinderRequest();
			}
		}
	}
}

#pragma mark ===========================

#pragma mark --- AWindow_WordsList

/**
���[�Y���X�g�E�C���h�E�ɒP�ꌟ����v��
@note ���[�Y���X�g�E�C���h�E���Ȃ���Ή������Ȃ�
*/
void	AApp::SPI_RequestWordsList( const AWindowID inTextWindowID, const AFileAcc& inProjectFolder, const AIndex inModeIndex,
		const AText& inWord )
{
	//�e���[�Y���X�g�E�C���h�E�̂����ATextWindowID���ʒu������́i�I�[�o�[���C�j�A����сATextWindowID�����ݒ�i�t���[�e�B���O�j�̂��̂ɓK�p
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_WordsList )
			{
				AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
				if( textWindowID == inTextWindowID || textWindowID == kObjectID_Invalid )
				{
					//�P�ꌟ����v��
					SPI_GetWordsListWindow(winID).SPI_GetWordsListView().SPI_RequestWordsList(inProjectFolder,inModeIndex,inWord);
				}
			}
		}
	}
}

/**
���[�Y���X�g�E�C���h�E��import file�v���O���X��\���E��\��
*/
void	AApp::SPI_ShowHideImportFileProgressInWordsListWindow( const ABool inShow )
{
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_WordsList )
			{
				SPI_GetWordsListWindow(winID).SPI_ShowHideImportFileProgress(inShow);
			}
		}
	}
}

/**
�h�L�������g�ҏW���e���[�Y���X�g�E�C���h�E�ɒʒm
*/
void	AApp::SPI_NotifyToWordsListByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	//�ewordslist�E�C���h�E�ɒʒm
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_WordsList )
			{
				SPI_GetWordsListWindow(winID).SPI_GetWordsListView().SPI_NotifyToWordsListByTextDocumentEdited(inFile,inTextIndex,inInsertedCount);
			}
		}
	}
}

/**
���[�Y���X�g�E�C���h�E�̌��݂̌����𒆎~
*/
void	AApp::SPI_AbortCurrentWordsListFinderRequestInWordsListWindow()
{
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_WordsList )
			{
				SPI_GetWordsListWindow(winID).SPI_GetWordsListView().SPI_AbortCurrentWordsListFinderRequest();
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---���[�Y���X�g
//#723

/**
�w��v���W�F�N�g�A���[�h�ɑ�����S�Ẵt�@�C����ImportFileData���擾�iImportFileData�I�u�W�F�N�g�����݂Ȃ琶���j
@note thread-safe
*/
void	AApp::SPI_GetImportFileDataIDArrayForWordsList( const AFileAcc& inProjectFolder, const AIndex inModeIndex, 
			AArray<AObjectID>& outImportFileDataIDArray, const ABool& inAbort ) 
{
	//���ʏ�����
	outImportFileDataIDArray.DeleteAll();
	ATextArray	filePathArray;
	//�v���W�F�N�g�t�H���_����v���W�F�N�g�擾
	//�iproject array�̗v�f���폜����邱�Ƃ͂Ȃ��̂ŁA������mutex�s�v�B
	AIndex	projectIndex = SPI_GetSameProjectArrayIndex(inProjectFolder);
	if( projectIndex == kIndex_Invalid )
	{
		//------------------�v���W�F�N�g�����̏ꍇ------------------
		//�v���W�F�N�g�����̏ꍇ�A�����t�H���_���̃t�@�C�����ċA�I�Ɏ擾 #908
		AObjectArray<AFileAcc>	children;
		inProjectFolder.GetChildren(children);
		//inProjectFolder�t�H���_���̊e�t�@�C�����Ƃ̃��[�v
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			AFileAcc	file = children.GetObjectConst(i);
			if( file.IsFolder() == false )
			{
				AText	path;
				file.GetPath(path);
				//�t�@�C���p�X���擾
				filePathArray.Add(path);
			}
		}
	}
	else
	{
		//------------------�v���W�F�N�g�L��̏ꍇ------------------
		//�v���W�F�N�g���̃t�@�C�����̃��[�v
		{
			//#723
			AStMutexLocker	locker(mSameProjectArrayMutex);
			
			//�v���W�F�N�g���t�@�C�����̃��[�v
			for( AIndex i = 0; i < mSameProjectArray_Pathname.GetObjectConst(projectIndex).GetItemCount(); i++ )
			{
				//�t�@�C���̃��[�h����v����ꍇ�̂݁AImportFileData�I�u�W�F�N�g�擾
				AIndex	modeIndex = mSameProjectArray_ModeIndex.GetObjectConst(projectIndex).Get(i);
				if( inModeIndex == modeIndex )
				{
					//�t�@�C���p�X�擾
					AText	path;
					mSameProjectArray_Pathname.GetObjectConst(projectIndex).Get(i,path);
					filePathArray.Add(path);
				}
			}
		}
	}
	//��Ŏ擾�����t�@�C���p�X���X�g���ږ��̃��[�v
	for( AIndex i = 0; i < filePathArray.GetItemCount(); i++ )
	{
		//abort���ꂽ��break
		if( inAbort == true )
		{
			break;
		}
		//�t�@�C���擾
		AFileAcc	file;
		file.Specify(filePathArray.GetTextConst(i));
		//�v���W�F�N�g���̑S�Ẵt�@�C���ɂ��āAImportFileData�I�u�W�F�N�g�𐶐�����B
		//�������A���̎��_�Ő�������Ă��Ȃ����̂ɂ��ẮATextInfo���̃��[�h���X�g�͋�ɂȂ�
		AObjectID	importFileDataID = SPI_GetImportFileManager().FindOrLoadImportFileDataByFile(file,false);
		if( importFileDataID != kObjectID_Invalid )
		{
			outImportFileDataIDArray.Add(importFileDataID);
		}
	}
}

//#723
/**
�v���W�F�N�g���̂����t�@�C������v���W�F�N�g������
*/
/*#723
AIndex	AApp::SPI_FindProjectIndex( const AFileAcc& inFile ) const
{
	//�p�X�擾
	AText	path;
	inFile.GetPath(path);
	//�v���W�F�N�g���e�t�@�C���Ɣ�r
	for( AIndex i = 0; i < mSameProject_ProjectFolder.GetItemCount(); i++ )
	{
		AText	folderPath;
		mSameProject_ProjectFolder.GetObjectConst(i).GetPath(folderPath);
		if( folderPath.CompareMin(path) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}
*/

#pragma mark ===========================

#pragma mark --- AWindow_Previewer

/**
�v���r���[����URL���[�h�v��
*/
void	AApp::SPI_RequestLoadToPreviewerWindow( const AWindowID inTextWindowID, const AText& inURL )
{
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_Previewer )
			{
				AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
				if( textWindowID == inTextWindowID || textWindowID == kObjectID_Invalid )
				{
					//�v���r���[���[�Ƀ��[�h�v��
					SPI_GetPreviewerWindow(winID).SPI_RequestLoadURL(inURL);
				}
			}
		}
	}
}

/**
�v���r���[����URL�����[�h�v���i�Ō�̗v��URL�������[�h�j
*/
void	AApp::SPI_RequestReloadToPreviewerWindow( const AWindowID inTextWindowID )
{
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_Previewer )
			{
				AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
				if( textWindowID == inTextWindowID || textWindowID == kObjectID_Invalid )
				{
					//�v���r���[���[�Ƀ����[�h�v��
					SPI_GetPreviewerWindow(winID).SPI_RequestReload();
				}
			}
		}
	}
}

#if 0
/**
�v���r���[���[��JavaScript���s
*/
void	AApp::SPI_ExecuteJavaScriptInPreviewerWindow( const AWindowID inTextWindowID, const AText& inText )
{
	//�ePreviewer�E�C���h�E�̂����ATextWindowID���ʒu������́i�I�[�o�[���C�j�A����сATextWindowID�����ݒ�i�t���[�e�B���O�j�̂��̂ɓK�p
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_Previewer )
			{
				AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
				if( textWindowID == inTextWindowID || textWindowID == kObjectID_Invalid )
				{
					SPI_GetPreviewerWindow(winID).SPI_ExecuteJavaScript(inText);
				}
			}
		}
	}
}
#endif

/**
�v���r���[���[�E�C���h�E�����݂��Ă��邩�ǂ������擾
*/
ABool	AApp::SPI_PreviewWindowExist( const AWindowID inTextWindowID ) const
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_Previewer )
			{
				AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
				if( textWindowID == inTextWindowID || textWindowID == kObjectID_Invalid )
				{
					return true;
				}
			}
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark --- AWindow_ClipboardHistory
//#152

/**
�N���b�v�{�[�h�����ɒǉ�
*/
void	AApp::SPI_AddClipboardHistory( const AText& inText )
{
	//#861
	//�������L�����Ȃ����[�h�Ȃ牽�����Ȃ�
	if( NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )
	{
		return;
	}
	//�N���b�v�{�[�h�����L��
	mClipboardHistory.Insert1(0,inText);
	//�e�T�u�E�C���h�E�ɔ��f
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_ClipboardHistory )
			{
				SPI_GetClipboardHistoryWindow(winID).SPI_UpdateTable();
			}
		}
	}
}

#pragma mark ===========================

#pragma mark --- AWindow_TextMarker
//#750

/**
�e�L�X�g�}�[�J�[�@���݃O���[�v��ݒ�
*/
void	AApp::SPI_SetCurrentTextMarkerGroupIndex( const AIndex inIndex )
{
	//���݃O���[�v��ݒ�
	mCurrentTextMarkerGroupIndex = inIndex;
	//�e�L�X�g�}�[�J�[��S�Ẵh�L�������g�ɓK�p
	SPI_ApplyTextMarkerForAllDocuments();
	//���ׂẴe�L�X�g�}�[�J�[�ݒ�E�C���h�E�ɓK�p
	UpdateTextMarkerWindows();
}

/**
�e�L�X�g�}�[�J�[�@���݃O���[�v����f�[�^�擾
*/
void	AApp::SPI_GetCurrentTextMarkerData( AText& outTitle, AText& outText ) 
{
	//�f�[�^�擾
	NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kTextMarkerArray_Title,mCurrentTextMarkerGroupIndex,outTitle);
	NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kTextMarkerArray_Text,mCurrentTextMarkerGroupIndex,outText);
}

/**
�e�L�X�g�}�[�J�[�@���݃O���[�v�Ƀe�L�X�g�f�[�^�ݒ�
*/
void	AApp::SPI_SetCurrentTextMarkerData_Text( const AText& inText )
{
	//���݂̃f�[�^�擾���A�����Ȃ牽�����Ȃ�
	AText	text;
	NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kTextMarkerArray_Text,mCurrentTextMarkerGroupIndex,text);
	if( inText.Compare(text) == true )
	{
		return;
	}
	//���݃O���[�v�Ƀe�L�X�g�f�[�^�ݒ�
	NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kTextMarkerArray_Text,mCurrentTextMarkerGroupIndex,inText);
	//�e�L�X�g�}�[�J�[��S�Ẵh�L�������g�ɓK�p
	SPI_ApplyTextMarkerForAllDocuments();
	//���ׂẴe�L�X�g�}�[�J�[�ݒ�E�C���h�E�ɔ��f
	UpdateTextMarkerWindows();
}

/**
�e�L�X�g�}�[�J�[�@���݃O���[�v�Ƀ^�C�g���ݒ�
*/
void	AApp::SPI_SetCurrentTextMarkerData_Title( const AText& inTitle )
{
	//���݂̃f�[�^�擾���A�����Ȃ牽�����Ȃ�
	AText	text;
	NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kTextMarkerArray_Title,mCurrentTextMarkerGroupIndex,text);
	if( inTitle.Compare(text) == true )
	{
		return;
	}
	//���݃O���[�v�Ƀ^�C�g���ݒ�
	NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kTextMarkerArray_Title,mCurrentTextMarkerGroupIndex,inTitle);
	//�e�L�X�g�}�[�J�[�O���[�v���j���[�X�V
	UpdateTextMarkerMenu();
	//���ׂẴe�L�X�g�}�[�J�[�ݒ�E�C���h�E�ɔ��f
	UpdateTextMarkerWindows();
}

/**
�e�L�X�g�}�[�J�[��S�Ẵh�L�������g�ɓK�p
*/
void	AApp::SPI_ApplyTextMarkerForAllDocuments()
{
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObject(index).NVI_GetDocumentType() == kDocumentType_Text )
		{
			SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_ApplyTextMarker();
		}
	}
}

/**
�e�L�X�g�}�[�J�[�O���[�v��V�K����
*/
void	AApp::SPI_CreateNewTextMarkerGroup()
{
	//------------------�O���[�v�V�K����------------------
	AText	title, title0;
	title.SetLocalizedText("TextMarkerNewGroup");
	title0.SetText(title);
	//------------------���j�[�N�ȃO���[�v�����擾------------------
	//�����^�C�g���̃O���[�v�����邩�ǂ����𔻒�
	if( NVI_GetAppPrefDB().Find_TextArray(AAppPrefDB::kTextMarkerArray_Title,title0) != kIndex_Invalid )
	{
		//�����^�C�g���̃O���[�v�����ɑ��݂��Ă�����A"-2"���珇�ɁA�����^�C�g�������݂��Ȃ��Ȃ�悤�Ȑ����������ǉ�����
		for( ANumber num = 2; num < 9999; num++ )
		{
			//�n�C�t�H���{������ǉ�
			title.SetText(title0);
			title.AddCstring("-");
			title.AddNumber(num);
			//�����^�C�g���̃O���[�v�����݂��Ă��Ȃ���΃��[�v�I��
			if( NVI_GetAppPrefDB().Find_TextArray(AAppPrefDB::kTextMarkerArray_Title,title) == kIndex_Invalid )
			{
				break;
			}
		}
	}
	//�O���[�v�ǉ�
	mCurrentTextMarkerGroupIndex = NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kTextMarkerArray_Title,title);
	NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kTextMarkerArray_Text,GetEmptyText());
	//�e�L�X�g�}�[�J�[��S�Ẵh�L�������g�ɓK�p
	SPI_ApplyTextMarkerForAllDocuments();
	//�e�L�X�g�}�[�J�[�O���[�v���j���[�X�V
	UpdateTextMarkerMenu();
	//���ׂẴe�L�X�g�}�[�J�[�ݒ�E�C���h�E�ɔ��f
	UpdateTextMarkerWindows();
}

/**
�e�L�X�g�}�[�J�[�O���[�v���폜
*/
void	AApp::SPI_DeleteCurrentTextMarkerGroup()
{
	//�Ō�P�̃n�C���C�g�O���[�v�͍폜�s��
	//if( mCurrentTextMarkerGroupIndex == 0 )
	if( GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kTextMarkerArray_Text) <= 1 )
	{
		return;
	}
	
	//���݂̃O���[�v�폜
	NVI_GetAppPrefDB().Delete1_TextArray(AAppPrefDB::kTextMarkerArray_Title,mCurrentTextMarkerGroupIndex);
	NVI_GetAppPrefDB().Delete1_TextArray(AAppPrefDB::kTextMarkerArray_Text,mCurrentTextMarkerGroupIndex);
	mCurrentTextMarkerGroupIndex = 0;
	//�e�L�X�g�}�[�J�[��S�Ẵh�L�������g�ɓK�p
	SPI_ApplyTextMarkerForAllDocuments();
	//�e�L�X�g�}�[�J�[�O���[�v���j���[�X�V
	UpdateTextMarkerMenu();
	//���ׂẴe�L�X�g�}�[�J�[�ݒ�E�C���h�E�ɔ��f
	UpdateTextMarkerWindows();
}

/**
���ׂẴe�L�X�g�}�[�J�[�ݒ�E�C���h�E�ɔ��f
*/
void	AApp::UpdateTextMarkerWindows()
{
	//�e�T�u�E�C���h�E�ɔ��f
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_TextMarker )
			{
				SPI_GetTextMarkerWindow(winID).NVI_UpdateProperty();
			}
		}
	}
}

/**
�e�L�X�g�}�[�J�[�ݒ�E�C���h�E�I���i�Ȃ���ΐ����j
*/
void	AApp::SPI_SelectOrCreateTextMarkerWindow()
{
	//�T�u�E�C���h�E����
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_TextMarker )
			{
				SPI_GetTextMarkerWindow(winID).NVI_SelectWindow();
				return;
			}
		}
	}
	//��L�Ō�����Ȃ���΁A����
	SPI_CreateFloatingSubWindow(kSubWindowType_TextMarker,0,300,180);//#1316 ���ύX
}

/**
�e�L�X�g�}�[�J�[���j���[���X�V
*/
void	AApp::UpdateTextMarkerMenu()
{
	//�e�L�X�g�}�[�J�[�^�C�g���f�[�^�擾
	ATextArray	textArray;
	textArray.SetFromTextArray(NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kTextMarkerArray_Title));
	//�Z�p���[�^
	AText	text;
	text.SetCstring("-");
	textArray.Add(text);
	//�u�ݒ�v���j���[���ڒǉ�
	text.SetLocalizedText("TextMarker_Setup");
	textArray.Add(text);
	//TextArray���j���[�i�R���g���[����̃��j���[�j�̍X�V
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_TextMarker,textArray);
}

#pragma mark ===========================

#pragma mark --- AWindow_DocInfo
//#142

/**
DocInfo�i�e�L�X�g�J�E���g�j�S�������E�P�ꐔ�X�V
*/
void	AApp::SPI_UpdateDocInfoWindows_TotalCount( const ADocumentID inDocumentID, 
		const AItemCount inTotalCharCount, const AItemCount inTotalWordCount )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#1312 if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_DocInfo )
			{
				//�ΏۃT�u�E�C���h�E���ǂ�������B�i�t���[�e�B���O�͏�ɑΏہB�I�[�o�[���C�͑�����e�L�X�g�E�C���h�E�̌��݃h�L�������g����v���Ă���ΑΏہB�j
				if( JudgeIfTargetSubWindowByTextDocumentID(winID,inDocumentID) == true )
				{
					//�S�������E�P�ꐔ���f
					SPI_GetDocInfoWindow(winID).SPI_UpdateDocInfo_TotalCount(inTotalCharCount,inTotalWordCount,
								SPI_GetTextDocumentByID(inDocumentID).SPI_GetParagraphCount());
				}
			}
		}
	}
}

/**
DocInfo�i�e�L�X�g�J�E���g�j�I�𕶎����E�P�ꐔ�X�V
*/
void	AApp::SPI_UpdateDocInfoWindows_SelectedCount( const AWindowID inTextWindowID, 
													 const AItemCount inSelectedCharCount, const AItemCount inSelectedWordCount, 
													 const AItemCount inSelectedParagraphCount,
													 const AText& inSelectedText )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#1312 if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_DocInfo )
			{
				//�ΏۃT�u�E�C���h�E���ǂ�������B�i�t���[�e�B���O�͏�ɑΏہB�I�[�o�[���C�͑�����e�L�X�g�E�C���h�E�̌��݃h�L�������g����v���Ă���ΑΏہB�j
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					//�I�𕶎����E�P�ꐔ���f
					SPI_GetDocInfoWindow(winID).
							SPI_UpdateDocInfo_SelectedCount(inSelectedCharCount,inSelectedWordCount,inSelectedParagraphCount,
															inSelectedText);
				}
			}
		}
	}
}

/**
doc info�E�C���h�E���ݕ����X�V
*/
void	AApp::SPI_UpdateDocInfoWindows_CurrentChar( const AWindowID inTextWindowID, 
		const AUCS4Char inChar, const AUCS4Char inCanonicalDecomp, 
		const ATextArray& inHeaderTextArray, const AArray<AIndex>& inHeaderParagraphIndexArray,
		const AText& inDebugText )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#1312 if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_DocInfo )
			{
				//�ΏۃT�u�E�C���h�E���ǂ�������B�i�t���[�e�B���O�͏�ɑΏہB�I�[�o�[���C�͑�����e�L�X�g�E�C���h�E�̌��݃h�L�������g����v���Ă���ΑΏہB�j
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					//���ݕ����X�V
					SPI_GetDocInfoWindow(winID).SPI_UpdateDocInfo_CurrentChar(inChar,inCanonicalDecomp,inHeaderTextArray,inHeaderParagraphIndexArray,inDebugText);
				}
			}
		}
	}
}

/**
doc info�E�C���h�E�@�v���O�C������e�L�X�g�ݒ�
*/
void	AApp::SPI_UpdateDocInfoWindows_PluginText( const AWindowID inTextWindowID, const AText& inPluginText )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#1312 if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_DocInfo )
			{
				//�ΏۃT�u�E�C���h�E���ǂ�������B�i�t���[�e�B���O�͏�ɑΏہB�I�[�o�[���C�͑�����e�L�X�g�E�C���h�E�̌��݃h�L�������g����v���Ă���ΑΏہB�j
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					//�v���O�C���p�e�L�X�g�X�V
					SPI_GetDocInfoWindow(winID).SPI_UpdateDocInfo_PluginText(inPluginText);
				}
			}
		}
	}
}

/**
doc info�E�C���h�E�����݂��Ă��邩�ǂ������擾
*/
ABool	AApp::SPI_DocInfoWindowExist( const AWindowID inTextWindowID )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#1312 if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_DocInfo )
			{
				//�ΏۃT�u�E�C���h�E���ǂ�������B�i�t���[�e�B���O�͏�ɑΏہB�I�[�o�[���C�͑�����e�L�X�g�E�C���h�E�̌��݃h�L�������g����v���Ă���ΑΏہB�j
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					//
					return true;
				}
			}
		}
	}
	return false;
}

//#1312
/**
���݊J���Ă���h�L�������g���ׂĂɂ���doc info�X�V�������s��
�idoc inf�E�C���h�E��document������ɊJ�����ꍇ�ɂ��̏������s���j
*/
void	AApp::SPI_UpdateDocInfoWindowsForAllDocuments()
{
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		SPI_GetTextDocumentByID(docID).SPI_UpdateDocInfoWindows();
	}
}

#pragma mark ===========================

#pragma mark --- AWindow_FileList
//#725

/**
�t�@�C�����X�g���X�V
*/
void	AApp::SPI_UpdateFileListWindows( const AFileListUpdateType inType, const ADocumentID inDocumentID )
{
	//�e�t�@�C�����X�g�E�C���h�E�����[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FileList )
		{
			if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
			{
				//�t�@�C�����X�g�X�V
				SPI_GetFileListWindow(winID).SPI_UpdateFileList(inType,inDocumentID);
			}
		}
	}
}

/**
�t�@�C�����X�g�̕`��v���p�e�B�X�V
*/
void	AApp::SPI_UpdateFileListWindowsProperty()
{
	//�e�t�@�C�����X�g�E�C���h�E�����[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FileList )
		{
			if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
			{
				//�`��v���p�e�B�X�V
				SPI_GetFileListWindow(winID).NVI_UpdateProperty();
			}
		}
	}
}

//#361
/**
�t�@�C�����X�g�Ƀ����[�g�t�@�C���\�����A�����[�g�t�H���_���ēǍ�����
*/
void	AApp::SPI_UpdateFileListRemoteFolder( const AText& inFolderURL )
{
	//�e�t�@�C�����X�g�E�C���h�E�����[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FileList )
		{
			if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
			{
				//�����[�g�t�H���_���ēǍ�����
				SPI_GetFileListWindow(winID).SPI_UpdateRemoteFolder(inFolderURL);
			}
		}
	}
}

//#798
/**
�w�胂�[�h��\�����Ă���t�@�C�����X�g������
*/
AWindowID	AApp::FindFileListWindow( const AWindowID inTextWindowID, const ANumber inMode ) const
{
	//�e�t�@�C�����X�g�E�C���h�E�����[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FileList )
		{
			//���[�h��v�`�F�b�N
			if( SPI_GetFileListWindowConst(winID).SPI_GetCurrentMode() == inMode )
			{
				//TextWindowID��v�`�F�b�N
				if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
				{
					//�E�C���h�E�\�������`�F�b�N
					if( SPI_GetFileListWindowConst(winID).NVI_IsWindowVisible() == true )
					{
						return winID;
					}
				}
			}
		}
	}
	return kObjectID_Invalid;
}

//#798
/**
�u�����v���W�F�N�g�̃t�@�C���������v�i�t�@�C�����X�g�̃t�B���^�Ƀt�H�[�J�X�ړ��j
*/
void	AApp::SPI_SearchInProjectFolder()
{
	//�P�D�őP�ʂ̃e�L�X�g�E�C���h�E�̃I�[�o�[���C�E�C���h�E
	AWindowID	frontTextWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontTextWindowID != kObjectID_Invalid )
	{
		AWindowID	winID = FindFileListWindow(frontTextWindowID,kFileListMode_SameProject);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//�Q�D�e�L�X�g�E�C���h�E�w�肪�Ȃ����́i���t���[�e�B���O�E�C���h�E�j���猟��
	{
		AWindowID	winID = FindFileListWindow(kObjectID_Invalid,kFileListMode_SameProject);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//�R�D��L�ŃE�C���h�E��������΁A�V�K��������B
	{
		AWindowID	winID = GetApp().SPI_CreateFloatingSubWindow(kSubWindowType_FileList,kFileListMode_SameProject,250,250);
		SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
		return;
	}
}

//#798
/**
�u�����t�H���_�̃t�@�C���������v�i�t�@�C�����X�g�̃t�B���^�Ƀt�H�[�J�X�ړ��j
*/
void	AApp::SPI_SearchInSameFolder()
{
	//�P�D�őP�ʂ̃e�L�X�g�E�C���h�E�̃I�[�o�[���C�E�C���h�E
	AWindowID	frontTextWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontTextWindowID != kObjectID_Invalid )
	{
		AWindowID	winID = FindFileListWindow(frontTextWindowID,kFileListMode_SameFolder);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//�Q�D�e�L�X�g�E�C���h�E�w�肪�Ȃ����́i���t���[�e�B���O�E�C���h�E�j���猟��
	{
		AWindowID	winID = FindFileListWindow(kObjectID_Invalid,kFileListMode_SameFolder);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//�R�D��L�ŃE�C���h�E��������΁A�V�K��������B
	{
		AWindowID	winID = GetApp().SPI_CreateFloatingSubWindow(kSubWindowType_FileList,kFileListMode_SameFolder,250,250);
		SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
		return;
	}
}

//#798
/**
�u�ŋߊJ�����t�@�C���������v�i�t�@�C�����X�g�̃t�B���^�Ƀt�H�[�J�X�ړ��j
*/
void	AApp::SPI_SearchInRecentlyOpenFiles()
{
	//�P�D�őP�ʂ̃e�L�X�g�E�C���h�E�̃I�[�o�[���C�E�C���h�E
	AWindowID	frontTextWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontTextWindowID != kObjectID_Invalid )
	{
		AWindowID	winID = FindFileListWindow(frontTextWindowID,kFileListMode_RecentlyOpened);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//�Q�D�e�L�X�g�E�C���h�E�w�肪�Ȃ����́i���t���[�e�B���O�E�C���h�E�j���猟��
	{
		AWindowID	winID = FindFileListWindow(kObjectID_Invalid,kFileListMode_RecentlyOpened);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//�R�D��L�ŃE�C���h�E��������΁A�V�K��������B
	{
		AWindowID	winID = GetApp().SPI_CreateFloatingSubWindow(kSubWindowType_FileList,kFileListMode_RecentlyOpened,250,250);
		SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
		return;
	}
}

#pragma mark ===========================

#pragma mark --- AWindow_IdInfo
//#725

/**
�|�b�v�A�b�vIdInfo�E�C���h�E�擾
*/
/*
AWindow_IdInfo&	AApp::SPI_GetPopupIdInfoWindow()
{
	//�������Ȃ琶��
	if( mPopupIdInfoWindowID == kObjectID_Invalid )
	{
		mPopupIdInfoWindowID = SPI_CreateSubWindow(kSubWindowType_IdInfo,0,kObjectID_Invalid,
					kSubWindowLocationType_Popup,kIndex_Invalid);
		SPI_GetPopupIdInfoWindow().NVI_Create(kObjectID_Invalid);
	}
	//IdInfo�E�C���h�E�擾
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_IdInfo,kWindowType_IdInfo,mPopupIdInfoWindowID);
}
*/

//#853

/**
IdInfo�����X���b�h�擾
*/
AThread_IdInfoFinder&	AApp::SPI_GetIdInfoFinderThread()
{
	//�������Ȃ琶��
	if( mIdInfoFinderThreadID == kObjectID_Invalid )
	{
		AThread_IdInfoFinderFactory	factory(this);
		mIdInfoFinderThreadID = NVI_CreateThread(factory);
		NVI_GetThreadByID(mIdInfoFinderThreadID).NVI_Run(true);
	}
	//�X���b�h�擾
	return (dynamic_cast<AThread_IdInfoFinder&>(NVI_GetThreadByID(mIdInfoFinderThreadID)));
}

/**
IdInfo�����X���b�h����
*/
void	AApp::DoIdInfoFinderPaused( const ADocumentID inTextDocumentID,
		const AWindowID inIdInfoWindowID, const AWindowID inTextWindowID )
{
	//�������s��word, parentword���擾
	AText	word;
	ATextArray	parentWord;
	SPI_GetIdInfoFinderThread().GetWordForResult(word,parentWord);
	//�������ʎ擾
	ATextArray	keywordTextArray;
	ATextArray	parentKeywordTextArray;
	ATextArray	typeTextArray;
	ATextArray	infoTextArray;
	ATextArray	commentTextArray;
	AArray<AIndex>	categoryIndexArray;
	AArray<AScopeType>	scopeArray;
	AArray<AIndex>	startIndexArray;
	AArray<AIndex>	endIndexArray;
	ATextArray	filePathArray;
	AArray<AIndex>	colorSlotIndexArray;
	AIndex	argIndex = kIndex_Invalid;
	AText	callerFuncIdText;
	SPI_GetIdInfoFinderThread().GetResult(
				keywordTextArray,parentKeywordTextArray,typeTextArray,
				infoTextArray,commentTextArray,categoryIndexArray,colorSlotIndexArray,scopeArray,
				startIndexArray,endIndexArray,filePathArray,argIndex,callerFuncIdText);
	//IdInfo�E�C���h�E�w��̏ꍇ�i��IdInfo�E�C���h�E�̌����{�b�N�X�Ō��������ꍇ�j
	if( inIdInfoWindowID != kObjectID_Invalid )
	{
		if( NVI_ExistWindow(inIdInfoWindowID) == true )
		{
			//�w��IdInfo�E�C���h�E�֔��f
			SPI_GetIdInfoWindow(inIdInfoWindowID).SPI_GetIdInfoView().SPI_SetIdInfo(word,parentWord,inTextDocumentID,
						keywordTextArray,parentKeywordTextArray,
						typeTextArray,infoTextArray,commentTextArray,categoryIndexArray,colorSlotIndexArray,scopeArray,
						startIndexArray,endIndexArray,filePathArray,argIndex,callerFuncIdText);
		}
	}
	//TextWindow�w��̏ꍇ�i��Text�E�C���h�E���ID�N���b�N�������ꍇ�j
	else if( inTextWindowID != kObjectID_Invalid )
	{
		if( NVI_ExistWindow(inTextWindowID) == true )
		{
			//�e�E�C���h�E���̃��[�v
			for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
			{
				AWindowID	winID = mSubWindowArray_WindowID.Get(index);
				if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_IdInfo )
				{
					//�w��TextWindow�Ɉ�v�����ꍇ�A�܂��́A�t���[�e�B���O�E�|�b�v�A�b�v�̏ꍇ�A���̃E�C���h�E�ɔ��f
					AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
					if( inTextWindowID == textWindowID || textWindowID == kObjectID_Invalid )
					{
						//���f
						SPI_GetIdInfoWindow(winID).SPI_GetIdInfoView().SPI_SetIdInfo(word,parentWord,inTextDocumentID,
									keywordTextArray,parentKeywordTextArray,
									typeTextArray,infoTextArray,commentTextArray,categoryIndexArray,colorSlotIndexArray,scopeArray,
									startIndexArray,endIndexArray,filePathArray,argIndex,callerFuncIdText);
					}
				}
			}
			//�|�b�v�A�b�v�E�C���h�E�\���iSPI_ShowIdInfoPopupWindow()���ŁA���ڂ����݂��Ă��邩�ǂ����̔��f���s���A���݂��Ă���ꍇ�̂ݕ\�����Ă���j
			SPI_GetTextWindowByID(inTextWindowID).SPI_ShowIdInfoPopupWindow(true);
			//==================�e����v����L�[���[�h��⊮�\��==================
			if( parentKeywordTextArray.GetItemCount() > 0 )
			{
				SPI_GetTextWindowByID(inTextWindowID).SPI_GetCurrentFocusTextView().SPI_RequestCandidateFromParentKeyword(parentKeywordTextArray);
			}
		}
	}
}

/**
IdInfo�E�C���h�E�̎w�����index���n�C���C�g
*/
ABool	AApp::SPI_HighlightIdInfoArgIndex( const AWindowID inTextWindowID ,const AText& inKeyword, const AIndex inArgIndex )
{
	ABool	argHighlighted = false;
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_IdInfo )
			{
				//�w��TextWindow�Ɉ�v�����ꍇ�A�܂��́A�t���[�e�B���O�E�|�b�v�A�b�v�̏ꍇ�A���̃E�C���h�E�ɔ��f
				AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
				if( inTextWindowID == textWindowID || textWindowID == kObjectID_Invalid )
				{
					if( SPI_GetIdInfoWindow(winID).SPI_GetIdInfoView().SPI_HighlightIdInfoArgIndex(inKeyword,inArgIndex) == true )
					{
						argHighlighted = true;
					}
				}
			}
		}
	} 
	return argHighlighted;
}

//#1336
/**
�|�b�v�A�b�v�̃L�[���[�h���T�u�E�C���h�E�����
*/
void AApp::SPI_HideFloatingIdInfoWindow()
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_IdInfo )
			{
				if( SPI_GetSubWindowLocationType(winID) == kSubWindowLocationType_Popup )
				{
					SPI_GetIdInfoWindow(winID).NVI_Hide();
				}
			}
		}
	}
}

//#798
/**
�L�[���[�h�����E�C���h�E������
*/
AWindowID	AApp::SPI_FindIdInfoWindow( const AWindowID inTextWindowID ) const
{
	//�e�t�@�C�����X�g�E�C���h�E�����[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_IdInfo )
			{
				//TextWindowID��v�`�F�b�N
				if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
				{
					//�E�C���h�E�\�������`�F�b�N
					if( SPI_GetIdInfoWindowConst(winID).NVI_IsWindowVisible() == true )
					{
						return winID;
					}
				}
			}
		}
	}
	return kObjectID_Invalid;
}

//#798
/**
�u�L�[���[�h�������v�i�t�B���^�Ƀt�H�[�J�X�ړ��j
*/
void	AApp::SPI_SearchInKeywordList()
{
	//�P�D�őP�ʂ̃e�L�X�g�E�C���h�E�̃I�[�o�[���C�E�C���h�E
	AWindowID	frontTextWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontTextWindowID != kObjectID_Invalid )
	{
		AWindowID	winID = SPI_FindIdInfoWindow(frontTextWindowID);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetIdInfoWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//�Q�D�e�L�X�g�E�C���h�E�w�肪�Ȃ����́i���t���[�e�B���O�E�C���h�E�j���猟��
	{
		AWindowID	winID = SPI_FindIdInfoWindow(kObjectID_Invalid);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetIdInfoWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//�R�D��L�ŃE�C���h�E��������΁A�V�K��������B
	{
		AWindowID	winID = GetApp().SPI_CreateFloatingSubWindow(kSubWindowType_IdInfo,0,250,250);
		SPI_GetIdInfoWindow(winID).SPI_SwitchFocusToSearchBox();
		return;
	}
}

#pragma mark ===========================

#pragma mark --- AWindow_JumpList
//#725

/**
�w��WindowID�ȊO�̃t���[�e�B���OJumpList��S��Hide�ɂ���
*/
void	AApp::SPI_HideOtherFloatingJumpListWindows( const AWindowID inWindowID )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_JumpList &&
						mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Floating &&
						winID != inWindowID )
			{
				{
					SPI_GetJumpListWindow(winID).NVI_Hide();
				}
			}
		}
	}
}

/**
�W�����v���X�g�E�C���h�E��tab�𐶐�
*/
void	AApp::SPI_CreateJumpListWindowsTab( const AWindowID inTextWindowID, const ADocumentID inDocumentID )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_JumpList )
		{
			//�w��text window�ɑ�����W�����v���X�g�E�C���h�E�Ȃ�^�u����
			if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
			{
				SPI_GetJumpListWindow(winID).NVI_CreateTab(inDocumentID);
			}
		}
	}
}

/**
�W�����v���X�g�E�C���h�E��tab�����
*/
void	AApp::SPI_CloseJumpListWindowsTab( const AWindowID inTextWindowID, const ADocumentID inDocumentID )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_JumpList )
		{
			//�w��text window�ɑ�����W�����v���X�g�E�C���h�E�Ȃ�A�^�u�폜
			if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
			{
				AIndex	tabIndex = SPI_GetJumpListWindow(winID).NVI_GetTabIndexByDocumentID(inDocumentID);
				SPI_GetJumpListWindow(winID).NVI_DeleteTabAndView(tabIndex);
			}
		}
	}
}

/**
�W�����v���X�g�E�C���h�E��tab��I��
*/
void	AApp::SPI_SelectJumpListWindowsTab( const AWindowID inTextWindowID, const ADocumentID inDocumentID )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_JumpList )
			{
				//�w��text window�ɑ�����W�����v���X�g�E�C���h�E�Ȃ�A�^�u�I��
				if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
				{
					//�^�u�I��
					AIndex	tabIndex = SPI_GetJumpListWindow(winID).NVI_GetTabIndexByDocumentID(inDocumentID);
					if( tabIndex != SPI_GetJumpListWindow(winID).NVI_GetCurrentTabIndex() )
					{
						SPI_GetJumpListWindow(winID).NVI_SelectTab(tabIndex);
						//�W�����v���X�g�E�C���h�E�̃^�C�g���o�[�X�V�i�t�@�C������\���j
						AText	text;
						text.SetLocalizedText("SubWindowTitle_JumpList");
						AText	title;
						NVI_GetDocumentByID(inDocumentID).NVI_GetTitle(title);
						text.AddCstring("  (");
						text.AddText(title);
						text.AddCstring(")");
						SPI_GetJumpListWindow(winID).SPI_SetSubWindowTitle(text);
					}
				}
			}
		}
	}
}

/**
�W�����v���X�g�E�C���h�E�̃^�C�g����ݒ�
*/
void	AApp::SPI_SetJumpListWindowTitle( const AWindowID inTextWindowID, const AText& inTitle )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_JumpList )
			{
				//�w��text window�ɑ�����W�����v���X�g�E�C���h�E�Ȃ�A�^�C�g���ݒ�
				if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
				{
					SPI_GetJumpListWindow(winID).SPI_SetSubWindowTitle(inTitle);
				}
			}
		}
	}
}

#pragma mark ===========================

#pragma mark --- AWindow_CandidateList
//#725 #717

/**
�|�b�v�A�b�v�̌�⃊�X�g�E�C���h�E�擾
*/
/*
AWindow_CandidateList&	AApp::SPI_GetPopupCandidateWindow()
{
	//�������Ȃ琶��
	if( mPopupCandidateWindowID == kObjectID_Invalid )
	{
		mPopupCandidateWindowID = SPI_CreateSubWindow(kSubWindowType_CandidateList,0,kObjectID_Invalid,
					kSubWindowLocationType_Popup,kIndex_Invalid);
		SPI_GetPopupCandidateWindow().NVI_Create(kObjectID_Invalid);
	}
	//�|�b�v�A�b�v�E�C���h�E����
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_CandidateList,kWindowType_CandidateList,mPopupCandidateWindowID);
}
*/

/**
��⃊�X�g�E�C���h�E�֌�⌟�����ʔ��f
*/
void	AApp::SPI_UpdateCandidateListWindows( const AModeIndex inModeIndex,
		const AWindowID inTextWindowID, const AControlID inTextViewControlID,
		const ATextArray& inIdTextArray, const ATextArray& inInfoTextArray,
		const AArray<AIndex>& inCategoryIndexArray, const AArray<AScopeType>& inScopeTypeArray, 
		const ATextArray& inFilePathArray, const AArray<ACandidatePriority>& inPriorityArray,
		const ATextArray& inParentKeywordArray,
		const AArray<AItemCount>& inMatchedCountArray )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true ||
					mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Popup )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CandidateList )
			{
				//�ΏۃE�C���h�E�Ȃ甽�f
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					SPI_GetCandidateListWindow(winID).SPI_SetInfo(inModeIndex,
								inTextWindowID,inTextViewControlID,
								inIdTextArray,inInfoTextArray,inCategoryIndexArray,inScopeTypeArray,inFilePathArray,
								inPriorityArray,inParentKeywordArray,inMatchedCountArray);
				}
			}
		}
	}
}

/**
��⃊�X�g�E�C���h�E�̍��ڑI��
*/
void	AApp::SPI_SetCandidateListWindowsSelectedIndex( const AWindowID inTextWindowID, const AIndex inSelectedIndex )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CandidateList )
			{
				//�ΏۃE�C���h�E�Ȃ獀�ڑI��
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					SPI_GetCandidateListWindow(winID).SPI_SetIndex(inSelectedIndex);
				}
			}
		}
	}
}

/**
��⃊�X�g�E�C���h�E�̍��ڃN���b�N�𖳌���
*/
void	AApp::SPI_InvalidateCandidateListWindowClick( const AWindowID inTextWindowID )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CandidateList )
			{
				//�ΏۃE�C���h�E�Ȃ獀�ڑI��
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					SPI_GetCandidateListWindow(winID).SPI_GetCandidateListView().SPI_InvalidateClick();
				}
			}
		}
	}
}

/**
��⌟���X���b�h�擾
*/
AThread_CandidateFinder&	AApp::SPI_GetCandidateFinderThread()
{
	//�������Ȃ琶��
	if( mCandidateFinderThreadID == kObjectID_Invalid )
	{
		AThread_CandidateFinderFactory	factory(this);
		mCandidateFinderThreadID = NVI_CreateThread(factory);
		NVI_GetThreadByID(mCandidateFinderThreadID).NVI_Run(true);
	}
	//�擾
	return (dynamic_cast<AThread_CandidateFinder&>(NVI_GetThreadByID(mCandidateFinderThreadID)));
}

#pragma mark ===========================

#pragma mark --- AWindow_KeyToolList
//#725

/**
�c�[�����X�g�Ƀf�[�^�ݒ�
*/
void	AApp::SPI_SetKeyToolListInfo( const AWindowID inTextWindowID, const AControlID inTextViewControlID,
									 const AGlobalPoint inPoint, const ANumber inLineHeight, 
									 const ATextArray& inTextArray, const ABoolArray& inIsUserToolArray )
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true ||
					mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Popup )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_KeyToolList )
			{
				//�ΏۃE�C���h�E�Ȃ獀�ڑI��
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					SPI_GetKeyToolListWindow(winID).SPI_SetInfo(inTextWindowID,inTextViewControlID,inPoint,inLineHeight,
																inTextArray,inIsUserToolArray);
				}
			}
		}
	}
}

#pragma mark ===========================

#pragma mark --- AWindow_FindResult
//#725

/**
�������ʃE�C���h�E�擾�i�Ȃ���ΐ����j
*/
ADocumentID	AApp::SPI_GetOrCreateFindResultWindowDocument()
{
	//�������ʃE�C���h�E������
	AWindowID	winID = SPI_GetCurrentFindResultWindowID();
	if( winID == kObjectID_Invalid )
	{
		//�������ʃE�C���h�E��������ΐ�������
		winID = SPI_CreateSubWindow(kSubWindowType_FindResult,0,kObjectID_Invalid,kSubWindowLocationType_Floating,kIndex_Invalid,false);
		//�z�u�E�\���iapp pref db�ɋL�����ꂽ�ʒu�ɕ\���j
		APoint	pt = {0};
		GetApp().GetAppPref().GetData_Point(AAppPrefDB::kIndexWindowPosition,pt);
		ANumber	w = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIndexWindowWidth);
		ANumber	h = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIndexWindowHeight);
		ARect	bounds = {0};
		bounds.left		= pt.x;
		bounds.top		= pt.y;
		bounds.right	= pt.x + w;
		bounds.bottom	= pt.y + h;
		NVI_GetWindowByID(winID).NVI_SetWindowBounds(bounds);
		NVI_GetWindowByID(winID).NVI_ConstrainWindowPosition(true);
		NVI_GetWindowByID(winID).NVI_Show();
	}
	//#959
	//�������ʃt���[�e�B���O�E�C���h�E����\����������\������i�����E�C���h�E��\�������Ƃ��Ƀt���[�e�B���O�E�C���h�E���\���ɂ������߁j
	if( SPI_GetFindResultWindow(winID).NVI_IsWindowVisible() == false )
	{
		SPI_GetFindResultWindow(winID).NVI_Show(false);
	}
	return SPI_GetFindResultWindow(winID).SPI_GetDocumentID();
}

/**
�������ʃE�C���h�E����
*/
AWindowID	AApp::SPI_GetCurrentFindResultWindowID()
{
	//------------------�܂����݂̍őO�ʃe�L�X�g�E�C���h�E�̃T�C�h�o�[����T��------------------
	AWindowID	frontTextWindowID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontTextWindowID != kObjectID_Invalid )
	{
		//�e�T�u�E�C���h�E���̃��[�v
		for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
		{
			AWindowID	winID = mSubWindowArray_WindowID.Get(index);
			//#959 if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
			{
				if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FindResult )
				{
					switch(mSubWindowArray_SubWindowLocationType.Get(index))
					{
					  case kSubWindowLocationType_LeftSideBar:
					  case kSubWindowLocationType_RightSideBar:
						{
							if( mSubWindowArray_TextWindowID.Get(index) == frontTextWindowID &&
										NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
							{
								//�T�C�h�o�[��ł��肽���܂�Ă�����A�܂肽���݂���������
								if( SPI_GetTextWindowByID(frontTextWindowID).SPI_IsCollapsedSubWindow(winID) == true )
								{
									SPI_GetTextWindowByID(frontTextWindowID).SPI_ExpandCollapseSubWindow(winID);
								}
								//
								return winID;
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
		}
	}
	//------------------�T�C�h�o�[�ɖ�����΁A�t���[�e�B���O�E�C���h�E��T��------------------
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#959 if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FindResult )
			{
				switch(mSubWindowArray_SubWindowLocationType.Get(index))
				{
				  case kSubWindowLocationType_Floating:
					{
						//#959 if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
						{
							return winID;
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
	}
	return kObjectID_Invalid;
}

//#1336
/**
�������ʃE�C���h�E�i�t���[�e�B���O�̂݁j����
*/
AWindowID	AApp::SPI_GetFloatingFindResultWindowID()
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#959 if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FindResult )
			{
				switch(mSubWindowArray_SubWindowLocationType.Get(index))
				{
				  case kSubWindowLocationType_Floating:
					{
						//#959 if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
						{
							return winID;
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
	}
	return kObjectID_Invalid;
}

#pragma mark ===========================

#pragma mark --- �L�[�L�^
//#725

/**
�L�[�L�^�E�C���h�E���������Ȃ琶��
*/
void	AApp::SPI_ShowKeyRecordWindow()
{
	if( SPI_FindKeyRecordWindow() == kObjectID_Invalid )
	{
		SPI_CreateFloatingSubWindow(kSubWindowType_KeyRecord,0,300,150);//#1316 ���ύX
	}
}

/**
�L�[�L�^�E�C���h�E������
*/
AWindowID	AApp::SPI_FindKeyRecordWindow() const
{
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_KeyRecord )
			{
				return winID;
			}
		}
	}
	return kObjectID_Invalid;
}

/**
�L�[�L�^
*/
void	AApp::AddRecordedText( const AText& inText, const ABool inEscape )
{
	//�L�[�L�^
	AText	text;
	if( inEscape == true )
	{
		//�G�X�P�[�v�L��̏ꍇ�i���ʏ�e�L�X�g�̒ǉ��j
		for( AIndex pos = 0; pos < inText.GetItemCount(); pos++ )
		{
			AUChar	ch = inText.Get(pos);
			switch(ch)
			{
			  case '<':
				{
					text.AddCstring("<<<LT1>>>");
					break;
				}
			  case '>':
				{
					text.AddCstring("<<<GT1>>>");
					break;
				}
			  default:
				{
					text.Add(ch);
					break;
				}
			}
		}
	}
	else
	{
		//�G�X�P�[�v�����̏ꍇ�i���c�[���R�}���h�e�L�X�g�̒ǉ��j
		text.SetText(inText);
	}
	mKeyRecordedText.AddText(text);
	
	//�L�[�L�^�E�C���h�E�X�V
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_KeyRecord )
			{
				SPI_GetKeyRecordWindow(winID).SPI_AddRecordedText(text);
			}
		}
	}
}

/**
�L�[�L�^�i�e�L�X�g�j
*/
void	AApp::SPI_RecordKeyText( const AText& inText )
{
	//�L�[�L�^���łȂ���Ή��������I��
	if( mKeyRecording == false )
	{
		return;
	}
	
	//�L�[�L�^
	AddRecordedText(inText,true);
}

/**
�L�[�L�^�i�L�[�A�N�V�����j
*/
void	AApp::SPI_RecordKeybindAction( const AKeyBindAction inAction, const AText& inText )
{
	//�L�[�L�^���łȂ���Ή��������I��
	if( mKeyRecording == false )
	{
		return;
	}
	
	//
	switch(inAction)
	{
	  case keyAction_string:
		{
			AddRecordedText(inText,true);
			break;
		}
		//#481
	  case keyAction_InsertText:
		{
			AddRecordedText(inText,true);
			break;
		}
		//
	  default:
		{
			AText	text;
			SPI_GetToolCommandTextFromKeyBindAction(inAction,text);
			text.InsertCstring(0,"<<<");
			text.AddCstring("\r>>>");
			AddRecordedText(text,false);
			break;
		}
	}
}

/**
�����̋L�^
*/
void	AApp::SPI_RecordFindNext( const AFindParameter& inParam )
{
	//�L�[�L�^���łȂ���Ή��������I��
	if( mKeyRecording == false )
	{
		return;
	}
	
	//�����̃c�[���R�}���h�擾
	AText	text;
	SPI_GetFindParamToolCommand(inParam,text,"FIND-TEXT-NEXT");
	
	//�L�[�L�^
	AddRecordedText(text,false);
}

/**
�����̋L�^
*/
void	AApp::SPI_RecordFindPrevious( const AFindParameter& inParam )
{
	//�L�[�L�^���łȂ���Ή��������I��
	if( mKeyRecording == false )
	{
		return;
	}
	
	//�����̃c�[���R�}���h�擾
	AText	text;
	SPI_GetFindParamToolCommand(inParam,text,"FIND-TEXT-PREV");
	
	//�L�[�L�^
	AddRecordedText(text,false);
}

/**
�����̋L�^
*/
void	AApp::SPI_RecordFindFromFirst( const AFindParameter& inParam )
{
	//�L�[�L�^���łȂ���Ή��������I��
	if( mKeyRecording == false )
	{
		return;
	}
	
	//�����̃c�[���R�}���h�擾
	AText	text;
	SPI_GetFindParamToolCommand(inParam,text,"FIND-TEXT-FROMFIRST");
	
	//�L�[�L�^
	AddRecordedText(text,false);
}

/**
�u���̋L�^
*/
void	AApp::SPI_RecordReplace( const AFindParameter& inParam, const AReplaceRangeType inReplaceRange )
{
	//�L�[�L�^���łȂ���Ή��������I��
	if( mKeyRecording == false )
	{
		return;
	}
	
	//�u���̃c�[���R�}���h�擾
	AText	text;
	SPI_GetReplaceParamToolCommand(inParam,inReplaceRange,text);
	
	//�L�[�L�^
	AddRecordedText(text,false);
}

/**
�L�^�J�n
*/
void	AApp::SPI_StartRecord()
{
	//��~���Ȃ�L�^�e�L�X�g��S�폜���A��~�t���O����
	if( mStoppedFlag == true )
	{
		mKeyRecordedText.DeleteAll();
		mStoppedFlag = false;
	}
	//�L�[�L�^���t���OON
	mKeyRecording = true;
	
	//�L�[�L�^�E�C���h�E�X�V
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_KeyRecord )
			{
				SPI_GetKeyRecordWindow(winID).SPI_SetRecordedText(mKeyRecordedText);
				SPI_GetKeyRecordWindow(winID).NVI_UpdateProperty();
			}
		}
	}
}

/**
�|�[�Y
*/
void	AApp::SPI_PauseRecord()
{
	//�L�[�L�^���t���OOFF
	mKeyRecording = false;
	//��~���t���OOFF�iSPI_StartRecord()���R�[������ƁA��������j
	mStoppedFlag = false;
	
	//�L�[�L�^�E�C���h�E�X�V
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_KeyRecord )
			{
				SPI_GetKeyRecordWindow(winID).NVI_UpdateProperty();
			}
		}
	}
}

/**
��~
*/
void	AApp::SPI_StopRecord()
{
	//�L�[�L�^���t���OOFF
	mKeyRecording = false;
	//��~���t���OON
	mStoppedFlag = true;
	
	//�L�[�L�^�E�C���h�E�X�V
	//�e�T�u�E�C���h�E���̃��[�v
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_KeyRecord )
			{
				SPI_GetKeyRecordWindow(winID).NVI_UpdateProperty();
			}
		}
	}
}

/**
�L�^�e�L�X�g�擾
*/
void	AApp::SPI_GetRecordedText( AText& outText ) const
{
	outText.SetText(mKeyRecordedText);
}

/**
*/
void	AApp::SPI_SetKeyRecordedRawText( const AText& inText )
{
	mKeyRecordedText.SetText(inText);
}

#pragma mark ===========================

#pragma mark --- EditProgress���[�_���Z�b�V����
//#846

/**
�ҏW�v���O���X�@���[�_���Z�b�V�����J�n
*/
ABool	AApp::SPI_StartEditProgressModalSession( const AEditProgressType inType, const ABool inEnableCancelButton,
		const ABool inShowProgressWindowNow )
{
	//���ɊJ�n�ς݂Ȃ牽�����Ȃ�
	if( mEditProgressModalSessionStarted == true )
	{
		return false;
	}
	//�p�����[�^�ݒ�
	mEditProgressType = inType;
	mEditProgressStartDateTime = ADateTimeWrapper::GetCurrentTime();
	mEditProgressContinueCheckDateTime = 0;
	mEditProgressModalSessionStarted = true;
	mEditProgressModalSessionStartedInActual = false;
	mEditProgressCurrentProgressPercent = 0;
	mEditProgressCurrentCount = 0;
	mEditProgressEnableCancelButton = inEnableCancelButton;
	
	if( inShowProgressWindowNow == true )
	{
		ShowProgressWindowAndStartModalSessionInActual();
	}
	
	return true;
}

/**
�ҏW�v���O���X�@���[�_���Z�b�V�����I��
*/
void	AApp::SPI_EndEditProgressModalSession()
{
	if( mEditProgressModalSessionStartedInActual == true )
	{
		//OS���[�_���Z�b�V�����I��
		SPI_GetEditProgressWindow().NVI_EndModalSession();
		//
		/*hide���Ƃ��΂炭����notification����������H
		SPI_GetEditProgressWindow().NVI_Hide();
		*/
		//�v���O���X�E�C���h�E�폜
		SPI_GetEditProgressWindow().NVI_Close();
		NVI_DeleteWindow(mEditProgressWindowID);
		mEditProgressWindowID = kObjectID_Invalid;
	}
	//�p�����[�^������
	mEditProgressType = kEditProgressType_None;
	mEditProgressStartDateTime = 0;
	mEditProgressContinueCheckDateTime = 0;
	mEditProgressModalSessionStartedInActual = false;
	mEditProgressModalSessionStarted = false;
	//internal event���s�i���[�_���Z�b�V��������internal event���s��}�����Ă���̂Łj
	//������internal event���s����ƁA�������s���Ԃ��������Ȃ��ƂɂȂ�\��������B�idocument��view activated���s�r���ŁA�ēxview activated���R�[�������Ȃǁj EVT_DoInternalEvent();
}

/**
���[�_���Z�b�V�����p������
*/
ABool	AApp::SPI_CheckContinueingEditProgressModalSession( const AEditProgressType inType, const AItemCount inCount,
															const ABool inUpdateProgress, const AItemCount inCurrentProgress, const AItemCount inTotalProgress, const ABool inForceShowDialog  )//#1374
{
	//���ݎ����擾
	ADateTime	currentDateTime = ADateTimeWrapper::GetCurrentTime();
	//OS���[�_���Z�b�V�������J�n�̏ꍇ�A�J�n����5�b�o���Ă��Ȃ���΁A�������Ōp���Ɣ��肵�A�����ŏI��
	if( mEditProgressModalSessionStartedInActual == false )
	{
		if( ADateTimeWrapper::GetDateTimeDiff(currentDateTime,mEditProgressStartDateTime) <= 5 && inForceShowDialog == false )//#1374
		{
			return true;
		}
	}
	
	//OS���[�_���Z�b�V�������J�n�̏ꍇ�A�J�n����
	if( mEditProgressModalSessionStartedInActual == false )
	{
		ShowProgressWindowAndStartModalSessionInActual();
	}
	//�v���O���X�E�C���h�E�X�V
	ABool	progressChanged = false;
	if( inUpdateProgress == true && inTotalProgress != 0 && inType == mEditProgressType)
	{
		ANumber	progress = inCurrentProgress*100/inTotalProgress;
		if( mEditProgressCurrentProgressPercent != progress )
		{
			progressChanged = true;
			SPI_GetEditProgressWindow().SPI_SetProgress(progress);
			mEditProgressCurrentProgressPercent = progress;
		}
	}
	//
	/*
	//�i16000���u�� �u�������\���L��F66s�A�\�������F38s�j
	if( mEditProgressCurrentCount != inCount )
	{
	mEditProgressCurrentCount = inCount;
	//
	if( (inCount%10) == 0 )
	{
	AText	progresstext;
	switch(mEditProgressType)
	{
	case kEditProgressType_Replace:
	{
	progresstext.SetLocalizedText("EditProgress_Progress_Replace");
	progresstext.ReplaceParamText('0',inCount);
	break;
	}
	}
	SPI_GetEditProgressWindow().SPI_SetProgressText(progresstext);
	}
	}
	*/
	
	//1�b�����Ɍp���`�F�b�N����B
	if( ADateTimeWrapper::GetDateTimeDiff(currentDateTime,mEditProgressContinueCheckDateTime) > 1 )
	{
		mEditProgressContinueCheckDateTime = currentDateTime;
		return SPI_GetEditProgressWindow().NVI_CheckContinueingModalSession();
	}
	else
	{
		return true;
	}
}

/**
���[�_���Z�b�V���������ǂ������擾�iOS��Ŏ��ۂɃ��[�_���Z�b�V�������ǂ����Ɋւ�炸�AAApp�N���X��ł̏�Ԃ�Ԃ��j
*/
ABool	AApp::SPI_InEditProgressModalSession() const
{
	return (mEditProgressType != kEditProgressType_None);
}

/**
���[�_���Z�b�V�����E�C���h�E��\�����AOS��̃��[�_���Z�b�V�������J�n����
*/
void	AApp::ShowProgressWindowAndStartModalSessionInActual()
{
	AWindow_EditProgress&	win = SPI_GetEditProgressWindow();
	//�v���O���X�E�C���h�E�ݒ�
	win.SPI_SetEnableCancelButton(mEditProgressEnableCancelButton);
	win.SPI_SetProgress(0);
	AText	title, message;
	switch(mEditProgressType)
	{
	  case kEditProgressType_Find:
		{
			title.SetLocalizedText("EditProgress_Title_Find");
			message.SetLocalizedText("EditProgress_Message_Find");
			break;
		}
	  case kEditProgressType_Replace:
		{
			title.SetLocalizedText("EditProgress_Title_Replace");
			message.SetLocalizedText("EditProgress_Message_Replace");
			message.ReplaceParamText('0',0);
			break;
		}
	  case kEditProgressType_Undo:
		{
			title.SetLocalizedText("EditProgress_Title_Undo");
			message.SetLocalizedText("EditProgress_Message_Undo");
			break;
		}
	  case kEditProgressType_Redo:
		{
			title.SetLocalizedText("EditProgress_Title_Redo");
			message.SetLocalizedText("EditProgress_Message_Redo");
			break;
		}
	  case kEditProgressType_MultiFileReplace:
		{
			title.SetLocalizedText("EditProgress_Title_MultiFileReplace");
			message.SetLocalizedText("EditProgress_Message_MultiFileReplace");
			break;
		}
	  case kEditProgressType_FindList:
		{
			title.SetLocalizedText("EditProgress_Title_FindList");
			message.SetLocalizedText("EditProgress_Message_FindList");
			break;
		}
	  case kEditProgressType_JavaScript:
		{
			title.SetLocalizedText("EditProgress_Title_JavaScript");
			message.SetLocalizedText("EditProgress_Message_JavaScript");
			break;
		}
	  case kEditProgressType_Indent:
		{
			title.SetLocalizedText("EditProgress_Title_Indent");
			message.SetLocalizedText("EditProgress_Message_Indent");
			break;
		}
	  case kEditProgressType_Correspond:
		{
			title.SetLocalizedText("EditProgress_Title_Correspond");
			message.SetLocalizedText("EditProgress_Message_Correspond");
			break;
		}
	  case kEditProgressType_BatchReplace:
		{
			title.SetLocalizedText("EditProgress_Title_BatchReplace");
			message.SetLocalizedText("EditProgress_Message_BatchReplace");
			break;
		}
	  case kEditProgressType_CalcLineInfo:
		{
			title.SetLocalizedText("EditProgress_Title_CalcLineInfo");
			message.SetLocalizedText("EditProgress_Message_CalcLineInfo");
			break;
		}
	  case kEditProgressType_InitMode:
		{
			title.SetLocalizedText("EditProgress_Title_InitMode");
			message.SetLocalizedText("EditProgress_Message_InitMode");
			break;
		}
	  case kEditProgressType_CopyV2AppPrefFolder:
		{
			title.SetLocalizedText("EditProgress_Title_CopyV2AppPrefFolder");
			message.SetLocalizedText("EditProgress_Message_CopyV2AppPrefFolder");
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	win.SPI_SetText(title,message);
	//�v���O���X�E�C���h�E�\��
	win.NVI_Show(true);
	//OS���[�_���Z�b�V�����J�n
	win.NVI_StartModalSession();
	mEditProgressModalSessionStartedInActual = true;
}

#pragma mark ===========================

#pragma mark --- JavaScript���s�X���b�h
//

/**
JavaScript���s�X���b�h�擾
*/
AThread_JavaScriptExecuter&	AApp::SPI_GetJavaScriptExecuterThread()
{
	//�������Ȃ琶��
	if( mJavaScriptExecuterThreadID == kObjectID_Invalid )
	{
		AThread_JavaScriptExecuterFactory	factory(this);
		mJavaScriptExecuterThreadID = NVI_CreateThread(factory);
	}
	//�擾
	return (dynamic_cast<AThread_JavaScriptExecuter&>(NVI_GetThreadByID(mJavaScriptExecuterThreadID)));
}

/**
JavaScript���s
*/
void	AApp::SPI_ExecuteJavaScript( const AText& inScriptText, const AFileAcc& inFolder )
{
	//���s���t���OON
	mExecutingJavaScript = true;
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_JavaScript,true,false,true);
	try
	{
		//JavaScript���s #1222 JavaScript�̓��C���X���b�h�Ŏ��s���邱�Ƃɂ���B
		NVI_DoJavaScript(inScriptText,inFolder);
		/*#1222 JavaScript�̓��C���X���b�h�Ŏ��s���邱�Ƃɂ���B
		//�X���b�h��JavaScript���s
		SPI_GetJavaScriptExecuterThread().SPNVI_Run_ExecuteJavaScript(inScriptText,inFolder);
		while( SPI_GetJavaScriptExecuterThread().SPI_IsCompleted() == false )
		{
			if( SPI_CheckContinueingEditProgressModalSession(kEditProgressType_JavaScript,0,true,-1,0) == false )
			{
				SPI_GetJavaScriptExecuterThread().NVI_Kill();
				//�K������kill�ł���Ƃ͌���Ȃ��̂ŁA���[�v�𔲂���B
				break;
			}
			NVI_SleepWithTimer(1);
		}
		*/
	}
	catch(...)
	{
		_ACError("",this);
	}
	//���s���t���OOFF
	mExecutingJavaScript = false;
}

//#1217
/**
JavaScript�_�C�A���O�\��
*/
void	AApp::SPI_ShowJavaScriptModalAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, ABool& outAborted )
{
	NVI_GetWindowByID(mJavaScriptModalAlertWindowID).NVI_Create(kObjectID_Invalid);
	(dynamic_cast<AWindow_JavaScriptModalAlert&>(NVI_GetWindowByID(mJavaScriptModalAlertWindowID))).SPNVI_Show(inMessage1,inMessage2,inOKButtonText,outAborted);
}

//#1217
/**
JavaScript�_�C�A���O�\��
*/
ABool	AApp::SPI_ShowJavaScriptModalCancelAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, const AText& inCancelButtonText, ABool& outAborted )
{
	NVI_GetWindowByID(mJavaScriptModalCancelAlertWindowID).NVI_Create(kObjectID_Invalid);
	return (dynamic_cast<AWindow_JavaScriptModalCancelAlert&>(NVI_GetWindowByID(mJavaScriptModalCancelAlertWindowID))).SPNVI_Show(inMessage1,inMessage2,inOKButtonText,inCancelButtonText,outAborted);
}

#pragma mark ===========================

#pragma mark --- JavaScript�v���O�C��

//#994
/**
�v���O�C���̃��[�h�iAModePrefDB����R�[�������j
*/
void	AApp::SPI_LoadPlugin( const AIndex inModeIndex, const AText& inPluginScript, const AFileAcc& inPluginFolder )
{
	mCurrentModeIndexForPluginLoad = inModeIndex;
	SPI_ExecuteJavaScript(inPluginScript,inPluginFolder);
	mCurrentModeIndexForPluginLoad = kIndex_Invalid;
}

/*#994
ABool	AApp::SPI_AddJavaScriptPlugin( const AText& inEventName, const AText& inScript, const AText& inGuid )
{
	if( inEventName.Compare("onSelectionChanged") == true )
	{
		//
		AIndex	index = mJavaScriptPluginArray_OnSelectionChanged_Guid.Find(inGuid);
		if( index != kIndex_Invalid )
		{
			mJavaScriptPluginArray_OnSelectionChanged_Guid.Delete1(index);
			mJavaScriptPluginArray_OnSelectionChanged_Script.Delete1(index);
		}
		//
		mJavaScriptPluginArray_OnSelectionChanged_Guid.Add(inGuid);
		mJavaScriptPluginArray_OnSelectionChanged_Script.Add(inScript);
		return true;
	}
	else
	{
		return false;
	}
}

void	AApp::SPI_DoJavaScriptPlugin_OnSelectionChanged()
{
	AItemCount	itemCount = mJavaScriptPluginArray_OnSelectionChanged_Script.GetItemCount();
	for( AIndex index = 0; index < itemCount; index++ )
	{
		AText	script;
		mJavaScriptPluginArray_OnSelectionChanged_Script.Get(index,script);
		AFileAcc	folder;
		GetApp().NVI_DoJavaScript(script,folder);
	}
}
*/

#pragma mark ===========================

#pragma mark <���L�N���X(AProjectPrefDB)>

#pragma mark ===========================

//�t�@�C���ɑΉ�����v���W�F�N�g�t�H���_����������B
void	AApp::SPI_FindProjectFolder( const AFileAcc& inFile, AFileAcc& outProjectFolder, AObjectID& outProjectObjectID, AText& outRelativePath )
{
	//RC3
	outProjectObjectID = kObjectID_Invalid;
	outRelativePath.DeleteAll();
	//
	AFileAcc	parent;
	parent.SpecifyParent(inFile);
	//�v���W�F�N�g�t�H���_
	AFileAcc	projectFolder;
	ABool	found = false;
	//miproj����
	AText	miproj("miproj");
	if( FindProjectFolderByFilenameRecursive(parent,miproj,projectFolder) == true )
	{
		found = true;
	}
	else
	{
		//#1284
		//������git�t�H���_�Ή��\��
		
		//��
		//svn�̃��[�g�t�H���_���v���W�F�N�g�t�H���_�ɂ��������ł��A����Ƀv���W�F�N�g�t�H���_�ɂ���ƁA��K�͂Ȃ΂����Ƀ������������������������
	}
	/* if( found == false )
	{
		//xcodeproj����
		AText	xcodeprojSuffix(".xcodeproj");
		if( FindProjectFolderBySuffixRecursive(parent,xcodeprojSuffix,projectFolder) == true )
		{
			found = true;
		}
	}*/
	if( found == true )
	{
		outProjectFolder.CopyFrom(projectFolder);
		//RC3
		//�v���W�F�N�gDB�I�u�W�F�N�g�擾�A�܂��́A����
		AText	projectfolderpath;
		projectFolder.GetPath(projectfolderpath);
		for( AIndex i = 0; i < mProjectPrefDBArray.GetItemCount(); i++ )
		{
			//#138 if( mProjectPrefDBArray.GetObjectConst(i).GetPath().Compare(projectpath) == true )
			//#138 projectpath���v���W�F�N�g�t�H���_�̃p�X�Ƃ݂Ȃ�����Amiproj�t�@�C���̃p�X�Ƃ݂Ȃ����肵�Ă����̂ŏC���B
			AText	p;
			mProjectPrefDBArray.GetObjectConst(i).GetProjectFolderPath(p);
			if( p.Compare(projectfolderpath) == true )
			{
				outProjectObjectID = mProjectPrefDBArray.GetObjectConst(i).GetObjectID();
			}
		}
		if( outProjectObjectID == kObjectID_Invalid )
		{
			AFileAcc	projfile;
			projfile.SpecifyChild(projectFolder,"miproj");
			AText	projfilepath;
			projfile.GetPath(projfilepath);
			//
			AProjectPrefDBFactory	factory(this,projfilepath);
			AIndex	index = mProjectPrefDBArray.AddNewObject(factory);
			outProjectObjectID = mProjectPrefDBArray.GetObjectConst(index).GetObjectID();
			mProjectPrefDBArray.GetObject(index).Load();
		}
		inFile.GetPartialPath(outProjectFolder,outRelativePath);
	}
}

//inFolder����t�H���_�������̂ڂ��āAinFilename�̖��O�����t�@�C�������݂���t�H���_����������
ABool	AApp::FindProjectFolderByFilenameRecursive( const AFileAcc& inFolder, const AText& inFilename, AFileAcc& outProjectFolder ) const
{
	/*#298 ����������
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		AFileAcc	child;
		child.CopyFrom(children.GetObjectConst(index));
		if( child.CompareFilename(inFilename) == true )
		{
			outProjectFolder.CopyFrom(inFolder);
			return true;
		}
	}
	*/
	//inFolder����inFilename�̖��O�̃t�@�C�������݂��邩�ǂ����𒲂ׂ�
	AFileAcc	file;
	file.SpecifyChild(inFolder,inFilename);
	if( file.Exist() == true )
	{
		outProjectFolder.CopyFrom(inFolder);
		return true;
	}
	//���݂��Ă��Ȃ���ΐe�t�H���_�Ō����i�ċA�j
	AFileAcc	parent;
	parent.SpecifyParent(inFolder);
	if( parent.IsRoot() == true )
	{
		return false;
	}
	return FindProjectFolderByFilenameRecursive(parent,inFilename,outProjectFolder);
}

//inFolder����t�H���_�������̂ڂ��āAinProjectSuffix�̊g���q�����t�@�C�������݂���t�H���_����������
ABool	AApp::FindProjectFolderBySuffixRecursive( const AFileAcc& inFolder, const AText& inProjectSuffix, AFileAcc& outProjectFolder ) const
{
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		AFileAcc	child;
		child.CopyFrom(children.GetObjectConst(index));
		if( child.CompareSuffix(inProjectSuffix) == true )
		{
			outProjectFolder.CopyFrom(inFolder);
			return true;
		}
	}
	AFileAcc	parent;
	parent.SpecifyParent(inFolder);
	if( parent.IsRoot() == true )
	{
		return false;
	}
	return FindProjectFolderBySuffixRecursive(parent,inProjectSuffix,outProjectFolder);
}

#if 0
//RC3
//�O���R�����g�擾
ABool	AApp::SPI_GetExternalCommentFromProjectDB( const AObjectID inProjectDBObjectID, const AText& inRelativePath, const AText& inModuleName,
		const AIndex inOffset, AText& outComment ) const
{
	outComment.DeleteAll();
	if( inProjectDBObjectID == kObjectID_Invalid )   return false;
	//
	AText	text;
	text.SetText(inRelativePath);
	text.Add(kUChar_CR);
	text.AddText(inModuleName);
	text.Add(kUChar_CR);
	text.AddNumber(inOffset);
	AIndex	index = mProjectPrefDBArray.GetObjectConstByID(inProjectDBObjectID).Find_TextArray(AProjectPrefDB::kExternalComment_PathAndModuleName,text);
	if( index != kIndex_Invalid )
	{
		mProjectPrefDBArray.GetObjectConstByID(inProjectDBObjectID).GetData_TextArray(AProjectPrefDB::kExternalComment_Comment,index,outComment);
		return true;
	}
	else
	{
		return false;
	}
}

//RC3
//�O���R�����g�ݒ�
//�ʒu�͈ړ����Ȃ�
void	AApp::SPI_SetExternalCommentToProjectDB( const AObjectID inProjectDBObjectID, const AText& inRelativePath, const AText& inModuleName,
		const AIndex inOffset, const AText& inComment ) 
{
	//
	if( inProjectDBObjectID == kObjectID_Invalid )   return;
	//
	AText	text;
	text.SetText(inRelativePath);
	text.Add(kUChar_CR);
	text.AddText(inModuleName);
	text.Add(kUChar_CR);
	text.AddNumber(inOffset);
	AIndex	index = mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Find_TextArray(AProjectPrefDB::kExternalComment_PathAndModuleName,text);
	if( index != kIndex_Invalid )
	{
		//�����̃f�[�^������ꍇ�͏㏑��
		mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).SetData_TextArray(AProjectPrefDB::kExternalComment_Comment,index,inComment);
	}
	else
	{
		//�����̃f�[�^���Ȃ��ꍇ�͐V�K����
		mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Add_TextArray(AProjectPrefDB::kExternalComment_PathAndModuleName,text);
		mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Add_TextArray(AProjectPrefDB::kExternalComment_Comment,inComment);
	}
	mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Save();
	//�h�L�������g�֍X�V�ʒm
	NotifyExternalCommentUpdatedToDocument(inProjectDBObjectID,inRelativePath);
}

//#138
/**
�O���R�����g�ʒu�X�V

�O���R�����g�̈ʒu�i���W���[�����{�I�t�Z�b�g�j���X�V����B
@param inProjectDBObjectID ProjectDB��ObjectID
@param inRelativePath �v���W�F�N�g�̃��[�g�t�H���_����̑��΃p�X
@param inModuleName �����W���[����
@param inOffset ���I�t�Z�b�g
@param inNewModuleName �V���W���[����
@param inNewOffset �V�I�t�Z�b�g
*/
void	AApp::SPI_UpdateExternalCommentPosition( const AObjectID inProjectDBObjectID, const AText& inRelativePath, 
		const AText& inModuleName, const AIndex inOffset, const AText& inNewModuleName, const AIndex inNewOffset )
{
	//�v���W�F�N�gDBObjectID�s���Ȃ牽�������ɏI��
	if( inProjectDBObjectID == kObjectID_Invalid )   return;
	//���ʒu
	AText	text;
	text.SetText(inRelativePath);
	text.Add(kUChar_CR);
	text.AddText(inModuleName);
	text.Add(kUChar_CR);
	text.AddNumber(inOffset);
	//�V�K�ʒu
	AText	newtext;
	newtext.SetText(inRelativePath);
	newtext.Add(kUChar_CR);
	newtext.AddText(inNewModuleName);
	newtext.Add(kUChar_CR);
	newtext.AddNumber(inNewOffset);
	//�V�K�ʒu�Ɋ��ɕtⳎ������݂���ꍇ�͉��������ɏI��
	if( mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Find_TextArray(AProjectPrefDB::kExternalComment_PathAndModuleName,newtext) != kIndex_Invalid )   return;
	//�ʒu�i���W���[�����{�I�t�Z�b�g�j�X�V
	AIndex	index = mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Find_TextArray(AProjectPrefDB::kExternalComment_PathAndModuleName,text);
	if( index != kIndex_Invalid )
	{
		mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).SetData_TextArray(AProjectPrefDB::kExternalComment_PathAndModuleName,index,newtext);
	}
	mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Save();
	//�h�L�������g�֍X�V�ʒm
	NotifyExternalCommentUpdatedToDocument(inProjectDBObjectID,inRelativePath);
}

//#138
/**
�O���R�����g�擾�i�h�L�������g�w��A�z��֎擾�j

�h�L�������g���̑S���R�[�h�ipath�Ɉ�v���郌�R�[�h�j��S�Ď擾
*/
void	AApp::SPI_GetExternalCommentArrayFromProjectDB( const AObjectID inProjectDBObjectID, const AText& inRelativePath, ATextArray& outModuleNameArray, ANumberArray& outOffsetArray, ATextArray& outCommentArray ) const
{
	//������
	outModuleNameArray.DeleteAll();
	outOffsetArray.DeleteAll();
	outCommentArray.DeleteAll();
	//DBObjectID���s���Ȃ牽�������I��
	if( inProjectDBObjectID == kObjectID_Invalid )   return;
	//���΃p�X����v���郌�R�[�h������
	AProjectPrefDB&	projectPrefDB = mProjectPrefDBArray.GetObjectConstByID(inProjectDBObjectID);
	for( AIndex i = 0; i < projectPrefDB.GetItemCount_Array(AProjectPrefDB::kExternalComment_PathAndModuleName); i++ )
	{
		const AText&	pathAndModuleName = projectPrefDB.GetData_TextArrayRef(AProjectPrefDB::kExternalComment_PathAndModuleName).GetTextConst(i);
		if( pathAndModuleName.CompareMin(inRelativePath) == true )
		{
			//���΃p�X����v������Amodulename��offset�����o��
			AText	modulename, offsettext;
			AIndex	spos = inRelativePath.GetItemCount()+1;
			AIndex	epos = spos;
			for( ; epos < pathAndModuleName.GetItemCount(); epos++ )
			{
				if( pathAndModuleName.Get(epos) == kUChar_CR )   break;
			}
			//modulename
			pathAndModuleName.GetText(spos,epos-spos,modulename);
			//offset
			pathAndModuleName.GetText(epos+1,pathAndModuleName.GetItemCount()-(epos+1),offsettext);
			ANumber	offset = offsettext.GetNumber();
			//�o�͔z��ɒǉ�
			outModuleNameArray.Add(modulename);
			outOffsetArray.Add(offset);
			outCommentArray.Add(projectPrefDB.GetData_TextArrayRef(AProjectPrefDB::kExternalComment_Comment).GetTextConst(i));
		}
	}
}
#endif

//RC3
void	AApp::SPI_ShowSetProjectFolderWindow( const AFileAcc& inDefaultFolder )
{
	/*#199 mSetProjectFolderWindow*/SPI_GetSetProjectFolderWindow().NVI_CreateAndShow();
	/*#199 mSetProjectFolderWindow*/SPI_GetSetProjectFolderWindow().SPI_SetFolder(inDefaultFolder);
}

/*#858
//RC3
void	AApp::SPI_ShowProjectFolderAlertWindow( const AFileAcc& inDefaultFolder )
{
	SPI_GetProjectFolderAlertWindow().NVI_CreateAndShow();
	SPI_GetProjectFolderAlertWindow().SPI_SetFolder(inDefaultFolder);
}
*/

//#858
#if 0
//#138
void	AApp::SPI_RevealExternalComment( const AObjectID inProjectDBObjectID, const AText& inRelativePath, const AText& inModuleName, const ANumber inOffset )
{
	ADocumentID docID = SPI_GetDocumentIDByProjectObjectIDAndPath(inProjectDBObjectID,inRelativePath,true);
	if( docID != kObjectID_Invalid )
	{
		SPI_GetTextDocumentByID(docID).SPI_SelectTextByModuleNameAndOffset(inModuleName,inOffset);
	}
}

//#138
/**
�O���R�����g�X�V���h�L�������g�֒ʒm����
*/
void	AApp::NotifyExternalCommentUpdatedToDocument( const AObjectID inProjectDBObjectID, const AText& inRelativePath )
{
	ADocumentID docID = SPI_GetDocumentIDByProjectObjectIDAndPath(inProjectDBObjectID,inRelativePath,false);
	if( docID == kObjectID_Invalid )   return;
	
	//�h�L�������g�̊O���R�����g�z����X�V�i�tⳎ��E�C���h�E�����̃��\�b�h����X�V�����j
	SPI_GetTextDocumentByID(docID).SPI_UpdateExternalCommentArray();
}
#endif

//#138
/**
ProjectObjectID�ƃp�X����h�L�������g��ID���擾
*/
ADocumentID	AApp::SPI_GetDocumentIDByProjectObjectIDAndPath( const AObjectID inProjectDBObjectID, const AText& inRelativePath,
			const ABool inCreateDocument )
{
	//�h�L�������g�̃p�X���擾
	AFileAcc	projectFile;
	projectFile.Specify(mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).GetProjectFilePath());
	AFileAcc	projectFolder;
	projectFolder.SpecifyParent(projectFile);
	AFileAcc	docfile;
	docfile.SpecifyChild(projectFolder,inRelativePath);
	//
	AObjectID	docID = NVI_GetDocumentIDByFile(kDocumentType_Text,docfile);
	if( docID == kObjectID_Invalid && inCreateDocument == true )
	{
		docID = SPNVI_CreateDocumentFromLocalFile(docfile);
	}
	return docID;
}

//#858
#if 0
#pragma mark ===========================

#pragma mark <���L�N���X(AWindow_FusenList)>

#pragma mark ===========================
//#138

/**
�tⳎ����X�g�E�C���h�E�̃A�N�e�B�u�h�L�������g��ݒ肷��
�A�N�e�B�u�E�C���h�E�i�^�u�j�̕ύX���ɃR�[�������B
@param inDocumentFile �A�N�e�B�u�h�L�������g�̃t�@�C��
*/
void	AApp::SPI_SetFusenListActiveDocument( /*#298 const AFileAcc& inDocumentFile*/ const AObjectID inProjectObjectID, const AText& inRelativePath )
{
	//�tⳎ����X�g�E�C���h�E�������Ȃ�I��
	if( SPI_GetFusenListWindow().NVI_IsWindowCreated() == false )   return;
	
	/*#298
	//ProjectObjectID�Ƒ��΃p�X���擾
	AFileAcc	projectFolder;
	AObjectID	projectObjectID;
	AText	relativePath;
	GetApp().SPI_FindProjectFolder(inDocumentFile,projectFolder,projectObjectID,relativePath);
	if( projectObjectID == kObjectID_Invalid )   return;
	*/
	if( inProjectObjectID == kObjectID_Invalid )   return;
	
	//View�ɑ΂��ăA�N�e�B�u�h�L�������g�X�V���w��
	SPI_GetFusenListWindow().SPI_GetListView().SPI_ChangeActiveDocument(inProjectObjectID,inRelativePath);
}
#endif

#pragma mark ===========================

#pragma mark ---�W�����v���j���[
//#857

/**
�W�����v���j���[�X�V(realize�{�`�F�b�N�}�[�N)�i�����W�����v���j���[�̏ꍇ�͍X�V���Ȃ��j
*/
void	AApp::SPI_UpdateJumpMenu( const AObjectID inDynamicMenuObjectID, const AIndex inCheckItemIndex )
{
	ABool	shouldRealize = (inDynamicMenuObjectID!=mCurrentJumpMenu_DynamicMenuObjectID);
	//�W�����v���j���[���̉�
	if( shouldRealize == true )
	{
		GetApp().NVI_GetMenuManager().RealizeDynamicMenu(inDynamicMenuObjectID);
		mCurrentJumpMenu_DynamicMenuObjectID = inDynamicMenuObjectID;
	}
	//������̉������A�������́A�`�F�b�N���ڂ��قȂ�ꍇ�́A�`�F�b�N�X�V
	if( shouldRealize == true || inCheckItemIndex != mCurrentJumpMenu_CheckItemIndex )
	{
		GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenu(kMenuItemID_Jump,inCheckItemIndex);
		mCurrentJumpMenu_CheckItemIndex = inCheckItemIndex;
	}
}

#pragma mark ===========================

#pragma mark ---�e��t���[�e�B���O�E�C���h�E

#pragma mark ===========================
//#291

/**
���ʎq���E�C���h�EShow/Hide
*/
/*#725
void	AApp::SPI_ShowHideIdInfoWindow( const ABool inShow )
{
	if( SPI_GetIdInfoWindow().NVI_GetOverlayMode() == true )   return;
	
	if( inShow == true )
	{
		SPI_GetIdInfoWindow().NVI_CreateAndShow();
	}
	else
	{
		SPI_GetIdInfoWindow().NVI_Hide();
	}
	//DB�ɕۑ�
	GetAppPref().SetData_Bool(AAppPrefDB::kDisplayIdInfoWindow,inShow);
}
 */

/**
�t�@�C�����X�g�E�C���h�EShow/Hide
*/
/*#725
void	AApp::SPI_ShowHideFileListWindow( const ABool inShow )
{
	if( SPI_GetFileListWindow().NVI_GetOverlayMode() == true )   return;
	
	if( inShow == true )
	{
		SPI_GetFileListWindow().NVI_CreateAndShow();
	}
	else
	{
		SPI_GetFileListWindow().NVI_Hide();
	}
	//DB�ɕۑ�
	GetAppPref().SetData_Bool(AAppPrefDB::kDisplayWindowList,inShow);
}
 */

#pragma mark ===========================

#pragma mark ---Navigate�{�^��

#pragma mark ===========================
//#146

/**
Nagivate�ʒu�o�^

���݂̍őO�ʂ̃h�L�������g�̑I���ʒu��Navigate�����ɕۑ�����
*/
void	AApp::SPI_RegisterNavigationPosition()
{
	//�őO�ʃE�C���h�E�̌��݃h�L�������g�̑I���ʒu���擾
	AWindowID	winID = NVI_GetMostFrontWindowID(kWindowType_Text);
	if( winID != kObjectID_Invalid )
	{
		ADocumentID	docID = SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextDocument().GetObjectID();
		//�t�@�C�����擾�ispecified�łȂ���Γo�^���Ȃ��j
		AFileAcc	file;
		if( SPI_GetTextDocumentByID(docID).NVI_IsFileSpecified() == true )
		{
			SPI_GetTextDocumentByID(docID).NVI_GetFile(file);
			//�I���ʒu�擾
			ATextIndex	start,end;
			SPI_GetTextWindowByID(winID).SPI_GetSelect(docID,start,end);
			//���݂�Index�ȍ~�̗v�f���폜
			while( mNavigate_File.GetItemCount() > mNavigateIndex )
			{
				mNavigate_File.Delete1Object(mNavigateIndex);
				mNavigate_StartIndex.Delete1(mNavigateIndex);
				mNavigate_Length.Delete1(mNavigateIndex);
			}
			//�ǉ�
			mNavigate_File.GetObject(mNavigate_File.AddNewObject()).CopyFrom(file);
			mNavigate_StartIndex.Add(start);
			mNavigate_Length.Add(end-start);
			//index��ItemCount���Ɠ����ɂ���
			mNavigateIndex++;
		}
	}
}

/**
����Navigate
*/
void	AApp::SPI_NavigateNext()
{
	if( SPI_GetCanNavigateNext() == false )   return;
	
	//index++
	mNavigateIndex++;
	//�e�L�X�g�I��
	if( mNavigateIndex == mNavigate_File.GetItemCount() )
	{
		//�����̍Ō�̏ꍇ��Last��I������i������ItemCount�̈ʒu�ɂ́A�v�f���Ȃ��B�j
		if( mNavigate_Last_DocumentID != kObjectID_Invalid )
		{
			if( NVI_IsDocumentValid(mNavigate_Last_DocumentID) == true )
			{
				if( NVI_GetDocumentTypeByID(mNavigate_Last_DocumentID) == kDocumentType_Text )
				{
					SPI_GetTextDocumentByID(mNavigate_Last_DocumentID).SPI_SelectText(mNavigate_Last_StartIndex,mNavigate_Last_Length);
				}
			}
		}
	}
	else
	{
		/*#725
		SPI_SelectTextDocument(mNavigate_File.GetObjectConst(mNavigateIndex),
		mNavigate_StartIndex.Get(mNavigateIndex),mNavigate_Length.Get(mNavigateIndex));
		*/
		AIndex	spos = mNavigate_StartIndex.Get(mNavigateIndex);
		AItemCount	len = mNavigate_Length.Get(mNavigateIndex);
		AText	path;
		mNavigate_File.GetObjectConst(mNavigateIndex).GetPath(path);
		SPI_OpenOrRevealTextDocument(path,spos,spos+len,kObjectID_Invalid,false,kAdjustScrollType_Center);
	}
}

/**
�O��Navigate
*/
void	AApp::SPI_NavigatePrev() 
{
	if( SPI_GetCanNavigatePrev() == false )   return;
	
	//�Ōォ��O�ɖ߂�ꍇ��Last���L������
	if( mNavigateIndex == mNavigate_File.GetItemCount() )
	{
		//�őO�ʃE�C���h�E�̌��݃h�L�������g�̑I���ʒu��ۑ�
		AWindowID	winID = NVI_GetMostFrontWindowID(kWindowType_Text);
		if( winID != kObjectID_Invalid )
		{
			ADocumentID	docID = SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextDocument().GetObjectID();
			ATextIndex	start,end;
			SPI_GetTextWindowByID(winID).SPI_GetSelect(docID,start,end);
			mNavigate_Last_DocumentID = docID;
			mNavigate_Last_StartIndex = start;
			mNavigate_Last_Length = end-start;
		}
		else
		{
			mNavigate_Last_DocumentID = kObjectID_Invalid;
			mNavigate_Last_StartIndex = kIndex_Invalid;
			mNavigate_Last_Length = 0;
		}
	}
	//index--
	mNavigateIndex--;
	//�e�L�X�g�I��
	/*#725
	SPI_SelectTextDocument(mNavigate_File.GetObjectConst(mNavigateIndex),
	mNavigate_StartIndex.Get(mNavigateIndex),mNavigate_Length.Get(mNavigateIndex));
	*/
	AIndex	spos = mNavigate_StartIndex.Get(mNavigateIndex);
	AItemCount	len = mNavigate_Length.Get(mNavigateIndex);
	AText	path;
	mNavigate_File.GetObjectConst(mNavigateIndex).GetPath(path);
	SPI_OpenOrRevealTextDocument(path,spos,spos+len,kObjectID_Invalid,false,kAdjustScrollType_Center);
}

/**
�O�ւ̈ړ��ۂ�Ԃ��B
*/
ABool	AApp::SPI_GetCanNavigatePrev() const
{
	return (mNavigateIndex > 0);
}

/**
���ւ̈ړ��ۂ�Ԃ�
*/
ABool	AApp::SPI_GetCanNavigateNext() const
{
	return (mNavigateIndex < mNavigate_File.GetItemCount());
}

#pragma mark ===========================

/*#725 SPI_UpdateFileListWindows()�ɕύX
void	AApp::SPI_UpdateFileListWindow_DocumentActivated( const ADocumentID inTextDocumentID )
{
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().SPI_Update_DocumentActivated(inTextDocumentID);
	}
}

void	AApp::SPI_UpdateFileListWindow_DocumentOpened( const ADocumentID inTextDocumentID )
{
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().SPI_Update_DocumentOpened(inTextDocumentID);
	}
}

void	AApp::SPI_UpdateFileListProperty()
{
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().NVI_UpdateProperty();
	}
}
*/

//#1056
/**
prepend���ꂽ�V�K�t�@�C�������������������s
�i�N������EVT_OpenUntitledFile()�o�R��SPI_CreateNewWindowAutomatic()���R�[������邪�A
�@mReopenProcessCompleted�t���O��false�̊Ԃ͎��s�����Aprepend�t���O��true�ɂ���݂̂Ƃ��Ă���B
�@prepend���ꂽ�V�K�t�@�C���������������������Ŏ��s����B�j
*/
void	AApp::DoPrependedOpenUntitledFileEvent()
{
	if( mPrependOpenUntitledFileEvent == true )
	{
		SPI_CreateNewWindowAutomatic();
		mPrependOpenUntitledFileEvent = false;
	}
}

//B0372
void	AApp::SPI_CreateNewWindowAutomatic()
{
	//#688
	//reopen�������͐V�K�����������Ȃ��Breopen�̍Ō�ɂ������Ă΂�A���̂Ƃ��ɂ͎��s�����B
	if( mReopenProcessCompleted == false )
	{
		mPrependOpenUntitledFileEvent = true;//#1056
		return;
	}
	
	//B0395 Dock�ɂ��܂�ꂽ�E�C���h�E��CWindowImp::GetMostFrontWindow()�Ŏ擾�ł��Ȃ��悤�Ȃ̂ŁA
	//�e�L�X�g�h�L�������g��0�̏ꍇ�̂ݐV�K�h�L�������g��������悤�ɕύX
	//if( NVI_GetMostFrontWindowID(kWindowType_Text) == kObjectID_Invalid )
	if( NVI_GetDocumentCount(kDocumentType_Text) == 0 )
	{
		//#844 if( GetAppPref().GetData_Bool(AAppPrefDB::kCreateNewWindowAutomatic) == true )//B0000 ���̐ݒ�OFF�ł�collapse�����͍s���悤�ɂ���
		{
			AText	name;
			GetAppPref().GetData_Text(AAppPrefDB::kCmdNModeName,name);
			AModeIndex	modeIndex = SPI_FindModeIndexByModeRawName(name);
			if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
			ADocumentID	docID = SPNVI_CreateNewTextDocument(modeIndex);//B0441
			if( docID == kObjectID_Invalid )   return;//win
			SPI_GetTextDocumentByID(docID).SPI_SetCreatedAutomatically(true);//B0441
		}
	}
	//B0395 �S�Ẵe�L�X�g�E�C���h�E��collapsed�Ȃ�A�ŏ��̃E�C���h�E��collapse��������
	else
	{
		AWindowID	firstWinID = NVI_GetFirstWindowID(kWindowType_Text);
		for( AWindowID winID = firstWinID; winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
		{
			if( SPI_GetTextWindowByID(winID).NVI_IsWindowCollapsed() == false )
			{
				firstWinID = kObjectID_Invalid;
				break;
			}
		}
		if( firstWinID != kObjectID_Invalid )
		{
			SPI_GetTextWindowByID(firstWinID).NVI_CollapseWindow(false);
			SPI_GetTextWindowByID(firstWinID).NVI_SelectWindow();
		}
	}
}

//#1056
/**
�����V�K�����h�L�������g�����
*/
void	AApp::CloseAutomaticallyCreatedDocument( const ADocumentID inDocID )
{
	if( inDocID != kObjectID_Invalid )
	{
		if( SPI_GetTextDocumentByID(inDocID).SPI_IsNewAndUneditted() == true &&
			SPI_GetTextDocumentByID(inDocID).SPI_GetCreatedAutomatically() == true )//B0441
		{
			//#379 SPI_GetTextDocumentByID(firstDocID).SPI_TryClose();
			SPI_TryCloseTextDocument(inDocID);//#379
		}
	}
}

#if 0
//#725
//#291
/**
*/
AFloatNumber	AApp::SPI_GetOverlayWindowsAlpha() const
{
	return NVI_GetAppPrefDBConst().GetData_Number(AAppPrefDB::kOverlayWindowsAlpha)/100.0;
}

//#291
/**
*/
void	AApp::SPI_UpdateOverlayWindowsAlpha()
{
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().SPI_UpdateOverlayWindowAlpha();
	}
	if( mIdInfoWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetIdInfoWindow().SPI_UpdateOverlayWindowAlpha();
	}
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateOverlayWindowAlpha();
		}
	}
	NVI_RefreshAllWindow();
	}
#endif

//#291
/**
*/
ABool	AApp::SPI_IsSupportPaneMode() const
{
	/*#688
#if IMPLEMENTATION_FOR_MACOSX
	return (AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4);
#else
	return true;
	#endif
	*/
	return true;
}

//#427
/**
�e���|�����t�H���_�擾�i�Ȃ���ΐ����j
*/
void	AApp::SPI_GetTempFolder( AFileAcc& outTempFolder )
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outTempFolder.SpecifyChild(appPrefFolder,"temp");
	outTempFolder.CreateFolder();
}

//#862
/**
�P���j�[�N�ȃe���|�����t�H���_�𐶐����擾
*/
void	AApp::SPI_GetUniqTempFolder( AFileAcc& outUniqTempFolder )
{
	AFileAcc	tempFolder;
	SPI_GetTempFolder(tempFolder);
	outUniqTempFolder.SpecifyUniqChildFile(tempFolder,"temp");
	outUniqTempFolder.CreateFolder();
}

//#801
/**
�i���ꂲ��data�t�H���_�擾
*/
void	AApp::SPI_GetLocalizedDataFolder( AFileAcc& outFolder ) const
{
	switch(NVI_GetLanguage())
	{
	  case kLanguage_Japanese:
		{
			AFileWrapper::GetResourceFile("data/jp",outFolder);
			break;
		}
	  default:
		{
			AFileWrapper::GetResourceFile("data/en",outFolder);
			break;
		}
	}
}

//#65
/**
�}���`�t�@�C���u���o�b�N�A�b�v�t�H���_�f�t�H���g�擾
*/
void	AApp::SPI_GetDefaultMultiFileReplaceBackupFolder( AFileAcc& outFolder )
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChild(appPrefFolder,"MultiFileReplaceBackup");
	outFolder.CreateFolder();
	//����"MultiFileReplaceBackup"���̂́A�t�H���_�łȂ����́i�t�@�C���j�����݂��Ă����ꍇ�́Aapp pref folder�ɂ���
	//���x���\��
	if( outFolder.IsFolder() == false )
	{
		outFolder.CopyFrom(appPrefFolder);
	}
}

//#81
/**
�����o�b�N�A�b�v�t�H���_�擾
*/
void	AApp::SPI_GetAutoBackupRootFolder( AFileAcc& outFolder )
{
	//==================�����o�b�N�A�b�v�ۑ���t�H���_�擾==================
	if( NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kEnableAutoBackup) == true )
	{
		//app pref�̎����o�b�N�A�b�v�ۑ���ݒ�f�[�^�擾
		AText	autoBackupRootFolderPath;
		NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kAutoBackupRootFolder,autoBackupRootFolderPath);
		outFolder.Specify(autoBackupRootFolderPath);
		//�t�H���_�����݂��Ă��Ȃ���΃t�H���_����
		if( outFolder.Exist() == false )
		{
			outFolder.CreateFolderRecursive();
		}
		//�t�H���_�����ł��Ȃ��ꍇ�́A�f�t�H���g�̃o�b�N�A�b�v��Ƀo�b�N�A�b�v����
		if( outFolder.Exist() == false || outFolder.IsFolder() == false )
		{
			SPI_GetDefaultAutoBackupRootFolder(outFolder);
			//���x��
		}
	}
	else
	{
		//�����o�b�N�A�b�v��enable�Ŗ����ꍇ�́A�f�t�H���gauto backup�t�H���_���擾
		//�h�L�������g�f�[�^�ۑ���t�H���_�Ƃ��Ă̂ݎg�p����B
		SPI_GetDefaultAutoBackupRootFolder(outFolder);
	}
}

//#81
/**
�����o�b�N�A�b�v�i�h�L�������g�f�[�^�ۑ��j�f�t�H���g�t�H���_�擾
*/
void	AApp::SPI_GetDefaultAutoBackupRootFolder( AFileAcc& outFolder )
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChild(appPrefFolder,"AutoBackup");
	outFolder.CreateFolderRecursive();
	//����"AutoBackup"���̂́A�t�H���_�łȂ����́i�t�@�C���j�����݂��Ă����ꍇ�́Aapp pref folder�ɂ���
	//���x���\��
	if( outFolder.IsFolder() == false )
	{
		outFolder.CopyFrom(appPrefFolder);
	}
}

//#81
/**
���ۑ��f�[�^�ۑ��t�H���_�擾
*/
void	AApp::SPI_GetUnsavedDataFolder( AFileAcc& outFolder )
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChild(appPrefFolder,"UnsavedData");
	outFolder.CreateFolder();
	//����"UnsavedData"���̂̃t�H���_�łȂ����́i�t�@�C���j�����݂��Ă����ꍇ�́Aapp pref folder�ɂ���
	//���x���\��
	if( outFolder.IsFolder() == false )
	{
		outFolder.CopyFrom(appPrefFolder);
	}
}

//#898
/**
�h�L�������g�f�[�^�ۑ��t�H���_�擾
*/
void	AApp::SPI_GetDocPrefRootFolder( AFileAcc& outFolder )
{
	outFolder.SpecifyChild(AFileWrapper::GetAppPrefFolder(),"DocumentData");
}

//#889
/**
�J���[�X�L�[���t�H���_�i�A�v����data/ColorScheme�j�擾
*/
void	AApp::SPI_GetAppColorSchemeFolder( AFileAcc& outFolder )
{
	AFileWrapper::GetResourceFile("data/ColorScheme",outFolder);
}

/**
���[�U�[�p�J���[�X�L�[���t�H���_�imi�A�v���ݒ�t�H���_��ColorScheme/�j�擾
*/
void	AApp::SPI_GetUserColorSchemeFolder( AFileAcc& outFolder ) const
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChild(appPrefFolder,"ColorScheme");
	outFolder.CreateFolderRecursive();
}

//#193
/**
�A�N�Z�X�v���O�C���t�H���_�i�A�v����AccessPlugin�j�擾
*/
void	AApp::SPI_GetAppAccessPluginFolder( AFileAcc& outFolder )
{
	AFileWrapper::GetResourceFile("AccessPlugin",outFolder);
}

/**
���[�U�[�p�A�N�Z�X�v���O�C���t�H���_�imi�A�v���ݒ�t�H���_��ColorScheme/�j�擾
*/
void	AApp::SPI_GetUserAccessPluginFolder( AFileAcc& outFolder ) const
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChild(appPrefFolder,"AccessPlugin");
	outFolder.CreateFolderRecursive();
}

/**
�e�L�X�g�G���R�[�f�B���O�t�@�C���i�A�v����data/TextEncoding/TextEncoding.csv�j�擾
*/
void	AApp::SPI_GetAppTextEncodingFile( AFileAcc& outFile ) const
{
	AFileWrapper::GetResourceFile("data/TextEncoding/TextEncoding.csv",outFile);
}

/**
�e�L�X�g�G���R�[�f�B���O�t�@�C���imi�A�v���ݒ�t�H���_��TextEncoding/TextEncoding.csv�j�擾
*/
void	AApp::SPI_GetUserTextEncodingFile( AFileAcc& outFile ) const
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	AFileAcc	folder;
	folder.SpecifyChild(appPrefFolder,"TextEncoding");
	folder.CreateFolderRecursive();
	outFile.SpecifyChild(folder,"TextEncoding.csv");
	outFile.CreateFile();
}

#pragma mark ===========================

#pragma mark --- �c�[���R�}���h�Ǘ�

//
void	AApp::RegisterToolCommand( AConstCharPtr inToolCommandText, const AToolCommandID inToolCommandID, const AKeyBindAction inKeyAction )
{
	AText	text;
	text.SetCstring(inToolCommandText);
	mToolCommand_Text.Add(text);
	mToolCommand_ID.Add(inToolCommandID);
	mToolCommand_KeyBindAction.Add(inKeyAction);
}

//
AToolCommandID	AApp::SPI_FindToolCommandID( const AText& inText ) const
{
	AIndex	index = mToolCommand_Text.Find(inText);
	if( index != kIndex_Invalid )
	{
		return mToolCommand_ID.Get(index);
	}
	return tc_NOP;
}

//
AKeyBindAction	AApp::SPI_GetKeyBindActionFromToolCommandText( const AText& inText ) const
{
	AIndex	index = mToolCommand_Text.Find(inText);
	if( index == kIndex_Invalid )
	{
		return keyAction_NOP;
	}
	return mToolCommand_KeyBindAction.Get(index);
}

//
void	AApp::SPI_GetToolCommandTextFromKeyBindAction( const AKeyBindAction inKeyAction, AText& outText ) const
{
	outText.DeleteAll();
	if( inKeyAction == keyAction_NOP )   return;
	AIndex	index = mToolCommand_KeyBindAction.Find(inKeyAction);
	if( index == kIndex_Invalid )   return;
	mToolCommand_Text.Get(index,outText);
}

#pragma mark ===========================

#pragma mark --- ��◚��
//#717

/**
��◚��ǉ�
*/
void	AApp::SPI_AddToRecentCompletionWord( const AText& inWord )
{
	AStMutexLocker	locker(mRecentCompletionWordArrayMutex);
	
	//��◚�����猟��
	AIndex	index = mRecentCompletionWordArray.Find(inWord);
	if( index != kIndex_Invalid )
	{
		//���������炻����폜
		mRecentCompletionWordArray.Delete1(index);
	}
	//��◚���ɒǉ�
	mRecentCompletionWordArray.Insert1(0,inWord);
	//��◚��������
	if( mRecentCompletionWordArray.GetItemCount() > kLimit_MaxRecentCompletionWordArray )
	{
		mRecentCompletionWordArray.Delete1(kLimit_MaxRecentCompletionWordArray);
	}
}

/**
��◚���擾
*/
void	AApp::SPI_GetRecentCompletionWordArray( AHashTextArray& outWordArray ) const
{
	AStMutexLocker	locker(mRecentCompletionWordArrayMutex);
	
	outWordArray.DeleteAll();
	for( AIndex i = 0; i < mRecentCompletionWordArray.GetItemCount(); i++ )
	{
		AText	word;
		mRecentCompletionWordArray.Get(i,word);
		outWordArray.Add(word);
	}
}

#pragma mark ===========================

#pragma mark ---Lua

#pragma mark ===========================
//#567

/**
LuaConsole��DocumentID�擾�i�������Ȃ�Invalid��Ԃ��j
*/
ADocumentID	AApp::SPI_GetLuaConsoleDocumentID()
{
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetLuaConsoleMode() == true )
		{
			return docID;
		}
	}
	return kObjectID_Invalid;
}

/**
LuaConsole��DocumentID�擾�i�������Ȃ琶�����Ď擾�j
*/
ADocumentID	AApp::SPI_CreateOrGetLuaConsoleDocumentID()
{
	ADocumentID	docID = SPI_GetLuaConsoleDocumentID();
	if( docID == kObjectID_Invalid )
	{
		docID = SPNVI_CreateLuaConsoleDocument();
	}
	return docID;
}

/**
�A�N�e�B�u��TextWindow���擾
*/
AWindowID	AApp::Lua_GetActiveTextWindowID()
{
	//LuaConsole��WindowID���擾
	ADocumentID	luaConsoleDocID = GetApp().SPI_GetLuaConsoleDocumentID();
	AWindowID	luaConsoleWinID = kObjectID_Invalid;
	if( luaConsoleDocID != kObjectID_Invalid )
	{
		luaConsoleWinID = GetApp().SPI_GetTextDocumentByID(luaConsoleDocID).SPI_GetFirstWindowID();
	}
	//LuaConsole�ȊO�̍őO��WindowID���擾
	return GetApp().NVI_GetMostFrontWindowID(kWindowType_Text,luaConsoleWinID);
}

/**
�A�N�e�B�u��TextDocument���擾
*/
AUniqID	AApp::Lua_GetActiveTextDocumentUniqID()//#693
{
	AWindowID	winID = Lua_GetActiveTextWindowID();
	if( winID == kObjectID_Invalid )   return kUniqID_Invalid;
	return GetApp().NVI_GetDocumentUniqID(GetApp().SPI_GetTextWindowByID(winID).NVI_GetCurrentDocumentID());//#693
}

/**
�A�N�e�B�u��TextView���擾
*/
AUniqID	AApp::Lua_GetActiveTextViewUniqID()//#693
{
	AWindowID	winID = Lua_GetActiveTextWindowID();
	if( winID != kObjectID_Invalid )
	{
		return GetApp().NVI_GetViewUniqID(GetApp().NVI_GetWindowConstByID(winID).NVI_GetCurrentFocusViewID());//#693
	}
	return kUniqID_Invalid;
}

#pragma mark ===========================

#pragma mark ---�h�L�������g����

/**
InsertText(int docID,int textIndex,string text)
@return void
*/
int	AApp::Lua_InsertText( lua_State* L )
{
	try
	{
		//�����擾
		AText	text;
		Lua_PopArgumentText(L,text);
		AIndex	textIndex = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�u�h�L�������g�ɑ΂��Ď��s
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqID����ADocumentID���擾
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//�h�L�������g���݃`�F�b�N
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndex�`�F�b�N
		if( textIndex < 0 || textIndex > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range");
		//TextIndex UTF-8�ʒu�`�F�b�N
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetCurrentCharTextIndex(textIndex) != textIndex )   Lua_Error(L,"(MI002) text index is not at first byte of UTF-8");
		
		//InsertText���s
		GetApp().SPI_GetTextDocumentByID(docID).SPI_RecordUndoActionTag(undoTag_Tool);
		GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(textIndex,text);
		
		//�Ԃ�l�ݒ�
		return 0;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
DeleteText(int docID,int spos,int epos)
@return void
*/
int	AApp::Lua_DeleteText( lua_State* L )
{
	try
	{
		//�����擾
		AIndex	epos = Lua_PopArgumentInteger(L);
		AIndex	spos = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�u�h�L�������g�ɑ΂��Ď��s
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqID����ADocumentID���擾
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//�h�L�������g���݃`�F�b�N
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndex�`�F�b�N
		if( spos < 0 || spos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (start index)");
		if( epos < 0 || epos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (end index)");
		//TextIndex UTF-8�ʒu�`�F�b�N
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetCurrentCharTextIndex(spos) != spos )   Lua_Error(L,"(MI002) text index is not at first byte of UTF-8 (start index)");
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetCurrentCharTextIndex(epos) != epos )   Lua_Error(L,"(MI002) text index is not at first byte of UTF-8 (end index)");
		
		//DeleteText���s
		GetApp().SPI_GetTextDocumentByID(docID).SPI_RecordUndoActionTag(undoTag_Tool);
		GetApp().SPI_GetTextDocumentByID(docID).SPI_DeleteText(spos,epos);
		
		//�Ԃ�l�ݒ�
		return 0;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
GetNextCharTextIndex(int docID,int pos)
@return int
*/
int	AApp::Lua_GetNextCharTextIndex( lua_State* L )
{
	try
	{
		//�����擾
		AIndex	pos = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�u�h�L�������g�ɑ΂��Ď��s
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqID����ADocumentID���擾
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//�h�L�������g���݃`�F�b�N
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndex�`�F�b�N
		if( pos < 0 || pos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range");
		
		//GetNextCharPos���s
		pos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetNextCharTextIndex(pos);
		
		//�Ԃ�l�ݒ�
		Lua_PushInteger(L,pos);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
GetPrevCharTextIndex(int docID,int pos)
@return int
*/
int	AApp::Lua_GetPrevCharTextIndex( lua_State* L )
{
	try
	{
		//�����擾
		AIndex	pos = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�u�h�L�������g�ɑ΂��Ď��s
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqID����ADocumentID���擾
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//�h�L�������g���݃`�F�b�N
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndex�`�F�b�N
		if( pos < 0 || pos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range");
		
		//GetPrevCharPos���s
		pos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetPrevCharTextIndex(pos);
		
		//�Ԃ�l�ݒ�
		Lua_PushInteger(L,pos);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
GetParagraphIndexFromTextIndex(int docID,int pos)
@return int
*/
int	AApp::Lua_GetParagraphIndexFromTextIndex( lua_State* L )
{
	try
	{
		//�����擾
		AIndex	pos = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�u�h�L�������g�ɑ΂��Ď��s
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqID����ADocumentID���擾
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//�h�L�������g���݃`�F�b�N
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndex�`�F�b�N
		if( pos < 0 || pos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range");
		
		//GetParagraphIndex���s
		ATextPoint	textpt = {0,0};
		GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextPointFromTextIndex(pos,textpt);
		AIndex	paraIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphIndexByLineIndex(textpt.y);
		
		//�Ԃ�l�ݒ�
		Lua_PushInteger(L,paraIndex);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
GetParagraphStartTextIndex(int docID,int paraIndex)
@return int
*/
int	AApp::Lua_GetParagraphStartTextIndex( lua_State* L )
{
	try
	{
		//�����擾
		AIndex	paraIndex = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�u�h�L�������g�ɑ΂��Ď��s
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqID����ADocumentID���擾
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//�h�L�������g���݃`�F�b�N
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//paraIndex�`�F�b�N
		if( paraIndex < 0 || paraIndex > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphCount() )   Lua_Error(L,"(MI003) paragraph index out of range");
		
		//GetParagraphStart���s
		AIndex	pos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphStartTextIndex(paraIndex);
		
		//�Ԃ�l�ݒ�
		Lua_PushInteger(L,pos);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
GetParagraphCount(int docID)
@return int
*/
int	AApp::Lua_GetParagraphCount( lua_State* L )
{
	try
	{
		//�����擾
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�u�h�L�������g�ɑ΂��Ď��s
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqID����ADocumentID���擾
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//�h�L�������g���݃`�F�b�N
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		
		//GetParagraphCount���s
		AItemCount	paraCount = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphCount();
		
		//�Ԃ�l�ݒ�
		Lua_PushInteger(L,paraCount);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
GetText(int docID,int spos,int epos)
@return void
*/
int	AApp::Lua_GetText( lua_State* L )
{
	try
	{
		//�����擾
		AIndex	epos = Lua_PopArgumentInteger(L);
		AIndex	spos = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�u�h�L�������g�ɑ΂��Ď��s
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqID����ADocumentID���擾
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//�h�L�������g���݃`�F�b�N
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndex�`�F�b�N
		if( spos < 0 || spos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (start index)");
		if( epos < 0 || epos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (end index)");
		
		//GetText���s
		AText	text;
		GetApp().SPI_GetTextDocumentByID(docID).SPI_GetText(spos,epos,text);
		
		//�Ԃ�l�ݒ�
		Lua_PushText(L,text);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

#pragma mark ===========================

#pragma mark ---View����

/**
GetSelectedRange(int viewID)
@return {int s,int e}
*/
int	AApp::Lua_GetSelectedRange( lua_State* L )
{
	try
	{
		//�����擾
		AUniqID	viewUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�uView�ɑ΂��Ď��s
		if( viewUniqID == kUniqID_Invalid )
		{
			viewUniqID = Lua_GetActiveTextViewUniqID();
		}
		//UniqID����AViewID���擾
		AViewID	viewID = GetApp().NVI_GetViewIDByUniqID(viewUniqID);//#693
		//View���݃`�F�b�N
		GetApp().Lua_CheckExistView(L,viewID,kViewType_Text);
		
		//�őO�ʂ̃h�L�������g�ɑ΂���GetSelectedText���s
		AIndex	startIndex = kIndex_Invalid, endIndex = kIndex_Invalid;
		AView_Text::GetTextViewByViewID(viewID).SPI_GetSelect(startIndex,endIndex);
		
		//�Ԃ�l�ݒ�
		Lua_PushNewTable(L);
		Lua_SetTableFieldInteger(L,"s",startIndex);
		Lua_SetTableFieldInteger(L,"e",endIndex);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
SetSelectedRange(int viewID,int spos,int epos)
@return void
*/
int	AApp::Lua_SetSelectedRange( lua_State* L )
{
	try
	{
		//�����擾
		AIndex	epos = Lua_PopArgumentInteger(L);
		AIndex	spos = Lua_PopArgumentInteger(L);
		AUniqID	viewUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�uView�ɑ΂��Ď��s
		if( viewUniqID == kUniqID_Invalid )
		{
			viewUniqID = Lua_GetActiveTextViewUniqID();
		}
		//UniqID����AViewID���擾
		AViewID	viewID = GetApp().NVI_GetViewIDByUniqID(viewUniqID);//#693
		//View���݃`�F�b�N
		GetApp().Lua_CheckExistView(L,viewID,kViewType_Text);
		//TextIndex�`�F�b�N
		if( spos < 0 || spos > AView_Text::GetTextViewByViewID(viewID).SPI_GetTextDocument().SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (start index)");
		if( epos < 0 || epos > AView_Text::GetTextViewByViewID(viewID).SPI_GetTextDocument().SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (end index)");
		
		//�őO�ʂ̃h�L�������g�ɑ΂���GetSelectedText���s
		AView_Text::GetTextViewByViewID(viewID).SPI_SetSelect(spos,epos);
		
		//�Ԃ�l�ݒ�
		return 0;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

#pragma mark ===========================

#pragma mark ---ObjectID�擾

/**
GetActiveDocumentID()
@return int
*/
int	AApp::Lua_GetActiveDocumentID( lua_State* L )
{
	try
	{
		//�����擾
		Lua_CheckArgumentEnd(L);
		
		//�A�N�e�B�u�h�L�������g�擾
		AUniqID	docID = Lua_GetActiveTextDocumentUniqID();//#693
		
		//�Ԃ�l�ݒ�
		Lua_PushInteger(L,docID.val);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
GetActiveViewID()
@return int
*/
int	AApp::Lua_GetActiveViewID( lua_State* L )
{
	try
	{
		//�����擾
		Lua_CheckArgumentEnd(L);
		
		//�A�N�e�B�u�r���[�擾
		AUniqID	viewID = Lua_GetActiveTextViewUniqID();//#693
		
		//�Ԃ�l�ݒ�
		Lua_PushInteger(L,viewID.val);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
GetDocumentIDFromViewID(int viewID)
@return int
*/
int	AApp::Lua_GetDocumentIDFromViewID( lua_State* L )
{
	try
	{
		//�����擾
		AUniqID	viewUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�uView�ɑ΂��Ď��s
		if( viewUniqID == kUniqID_Invalid )
		{
			viewUniqID = Lua_GetActiveTextViewUniqID();
		}
		//UniqID����AViewID���擾
		AViewID	viewID = GetApp().NVI_GetViewIDByUniqID(viewUniqID);//#693
		//View���݃`�F�b�N
		GetApp().Lua_CheckExistView(L,viewID,kViewType_Text);
		
		//
		AUniqID	docID = GetApp().NVI_GetDocumentUniqID(AView_Text::GetTextViewByViewID(viewID).SPI_GetTextDocument().GetObjectID());//#693
		
		//�Ԃ�l�ݒ�
		Lua_PushInteger(L,docID.val);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

#pragma mark ===========================

#pragma mark ---�h�L�������g�I�u�W�F�N�g

/**
CreateNewDocument(string modeName)
@return int
*/
int	AApp::Lua_CreateNewDocument( lua_State* L )
{
	try
	{
		//�����擾
		AText	modeName;
		Lua_PopArgumentText(L,modeName);
		Lua_CheckArgumentEnd(L);
		
		//CreateNewDocument���s
		AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
		if( modeIndex == kIndex_Invalid )
		{
			modeIndex = kStandardModeIndex;
		}
		AText	utf8("UTF-8");
		AUniqID	docID = GetApp().NVI_GetDocumentUniqID(GetApp().SPNVI_CreateNewTextDocument(modeIndex,utf8));//#693
		
		//�Ԃ�l�ݒ�
		Lua_PushInteger(L,docID.val);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
RevealDocument(int docID)
@return void
*/
int	AApp::Lua_RevealDocument( lua_State* L )
{
	try
	{
		//�����擾
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"�̏ꍇ�A�A�N�e�B�u�h�L�������g�ɑ΂��Ď��s
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqID����ADocumentID���擾
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//�h�L�������g���݃`�F�b�N
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		
		//Reveal���s
		GetApp().SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();
		
		//�Ԃ�l�ݒ�
		return 0;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

#pragma mark ===========================

#pragma mark ---���̑�

/**
print(string text)
@return void
*/
int	AApp::Lua_Output( lua_State* L )
{
	try
	{
		//�����擾
		AText	text;
		Lua_PopArgumentText(L,text);
		Lua_CheckArgumentEnd(L);
		
		//LuaConsole��output
		ADocumentID	docID = GetApp().SPI_CreateOrGetLuaConsoleDocumentID();
		GetApp().SPI_GetTextDocumentByID(docID).SPI_Lua_Output(text);
		
		//�Ԃ�l�ݒ�
		return 0;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
GetMiLuaScriptingVersion()
@return number
*/
int	AApp::Lua_GetMiLuaScriptingVersion( lua_State* L )
{
	try
	{
		//�����擾
		Lua_CheckArgumentEnd(L);
		
		//�Ԃ�l�ݒ�
		Lua_PushNumber(L,0.01);
		return 1;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

/**
Debug_AutoTest(string text)
@return void
*/
int	AApp::Lua_AutoTest( lua_State* L )
{
	try
	{
		//�����擾
		Lua_CheckArgumentEnd(L);
		
		//LuaConsole��output
		ADocumentID	docID = GetApp().SPI_CreateOrGetLuaConsoleDocumentID();
		
		//
		ABool	result = true;
		GetApp().SPI_GetTraceLogWindow().NVI_CreateAndShow();
		
		//
		AText	text;
		text.SetCstring("====================================\r");
		text.AddCstring("      UnitTest (without wrap test)  \r");
		text.AddCstring("====================================\r");
		text.AddCstring("       Base Component UnitTest      \r");
		text.AddCstring("------------------------------------\r");
		GetApp().SPI_GetTextDocumentByID(docID).SPI_Lua_Output(text);
		if( ABaseFunction::BaseComponentUnitTest() == false )   result = false;
		//
		if( result == true )
		{
			text.SetCstring("====================================\r");
			text.AddCstring("     Wrapper Component UnitTest     \r");
			text.AddCstring("------------------------------------\r");
			GetApp().SPI_GetTextDocumentByID(docID).SPI_Lua_Output(text);
	//		if( WrapperComponentUnitTest() == false )   result = false;
		}
		/*
		if( result == true )
		{
			_AInfo("====================================",this);
			_AInfo("       Abs Component UnitTest       ",this);
			_AInfo("------------------------------------",this);
			if( AbsTest(true) == false )   result = false;
		}
		*/
		if( result == true )
		{
			text.SetCstring("====================================\r");
			text.AddCstring("        UnitTest completed.         \r");
			text.AddCstring("     All tests are OK!     \r");
			text.AddCstring("====================================\r");
			GetApp().SPI_GetTextDocumentByID(docID).SPI_Lua_Output(text);
		}
		else
		{
			text.SetCstring("************************************\r");
			text.AddCstring(" NG!NG!NG!NG!NG!NG!NG!NG!NG!NG! \r");
			text.AddCstring(" At least one test is NG!!!\r");
			text.AddCstring(" NG!NG!NG!NG!NG!NG!NG!NG!NG!NG! \r");
			text.AddCstring("************************************\r");
			GetApp().SPI_GetTextDocumentByID(docID).SPI_Lua_Output(text);
		}
		//�Ԃ�l�ݒ�
		return 0;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

#pragma mark ===========================

//#1351
/**
�͂��߂ɂ��ǂ݂���������\������
*/
void	AApp::SPI_ShowReadMeFirst()
{
	AFileAcc	helpFolder;
	AFileWrapper::GetHelpFolder("ja",helpFolder);
	AFileAcc	readme;
	readme.SpecifyChild(helpFolder,"README");
	ADocumentID	docID = SPNVI_CreateDocumentFromLocalFile(readme);
	AText	modename("headers");
	AModeIndex modeIndex = SPI_FindModeIndexByModeRawName(modename);
	SPI_GetTextDocumentByID(docID).SPI_SetMode(modeIndex);
	SPI_GetTextDocumentByID(docID).NVI_SetReadOnly(true);
	SPI_GetTextDocumentByID(docID).SPI_SetReadMeFile();
}

#pragma mark ===========================

#pragma mark <�������W���[��>

#pragma mark ===========================

/*win void	AApp::SPI_PostCommand( const AMenuItemID inCommandID )
{
#if IMPLEMENTATION_FOR_MACOSX
	HICommand	command;
	command.attributes = NULL;
	command.commandID = inCommandID;
	command.menu.menuRef = NULL;
	command.menu.menuItemIndex = 0;
	::ProcessHICommand(&command);
#else
	_ACError("not implemented",this);
#endif
}
*/
/*#193
void	AApp::SPI_ShowFTPFolderWindow()
{
	//#361 SPI_GetFTPFolderWindow().NVI_CreateAndShow();
}
*/

void	AApp::SPI_TexPreview( const AFileAcc& inDocFile )
{
	//
	AText	filename;
	inDocFile.GetFilename(filename);
	AText	origsuffix;
	filename.GetSuffix(origsuffix);
	AText	filenamewithoutsuffix;
	filenamewithoutsuffix.SetText(filename);
	filenamewithoutsuffix.Delete(filename.GetItemCount()-origsuffix.GetItemCount(),origsuffix.GetItemCount());
	AText	dvifilename;
	dvifilename.SetText(filenamewithoutsuffix);
	dvifilename.AddCstring(".dvi");
	AFileAcc	dvifile;
	dvifile.SpecifySister(inDocFile,dvifilename);
	AFileAcc	folder;
	folder.SpecifyParent(dvifile);
	AText	folderpath;
	folder.GetPath(folderpath);
	/*
	AText	previewerpath;
	GetAppPref().GetData_Text(AAppPrefDB::kTexPreviewerPath,previewerpath);
	AFileAcc	previewer;
	previewer.Specify(previewerpath);
	AAppAcc	app(previewer);
	ALaunchWrapper::Drop(app,dvifile);
	*/
	//�f�B���N�g���ݒ�
	AStCreateCstringFromAText	dirPath(folderpath);
	chdir(dirPath.GetPtr());
	//dvi to pdf�R�}���h�p�X�擾
	AText	command;
	NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kTexDviToPdfPath,command);
	command.AddCstring(" \"");
	command.AddText(dvifilename);
	command.AddCstring("\"");
	//dvi to pdf�R�}���h���s
	AStCreateCstringFromAText	cstr(command);
	system(cstr.GetPtr());
	
	//pdf�t�@�C���擾
	AText	pdffilename;
	pdffilename.SetText(filenamewithoutsuffix);
	pdffilename.AddCstring(".pdf");
	AFileAcc	pdffile;
	pdffile.SpecifyChild(folder,pdffilename);
	//pdf�t�@�C�����J��
	ALaunchWrapper::Open(pdffile);
}


//�v���I�G���[�������̃f�[�^�o�b�N�A�b�v
//ABaseFunction::CriticalError������R�[�������
void	AApp::NVIDO_BackupDataForCriticalError()//#182
{
	//�Ή�����
	//�e�h�L�������g�̓��e���t�@�C���ɏ����o��
}

/*
#182 AApplication.cpp�Ɉړ�
void	BackupDataForCriticalError()
{
	try
	{
		GetApp().BackupDataForCriticalError();
	}
	catch(...)
	{
	}
}
*/

//�A�v���P�[�V������
void	GetApplicationName( AText& outName )
{
	outName.SetCstring("mi");
}

/*
#182 AApplication.cpp�Ɉړ�
void	EventTransactionPostProcess( const ABool inPeriodical )
{
	try
	{
		if( inPeriodical == false )
		{
			//���̂Ƃ��돈������
		}
	}
	catch(...)
	{
	}
}
*/

extern ABool	AbsTest( const ABool inWrapTest );

void	AApp::UnitTest()
{
#if IMPLEMENTATION_FOR_MACOSX

	ABool	result = true;
	SPI_GetTraceLogWindow().NVI_CreateAndShow();
	_AInfo("====================================",this);
	_AInfo("             UnitTest               ",this);
	_AInfo("====================================",this);
	_AInfo("       Base Component UnitTest      ",this);
	_AInfo("------------------------------------",this);
	if( ABaseFunction::BaseComponentUnitTest() == false )   result = false;
	if( result == true )
	{
		_AInfo("====================================",this);
		_AInfo("     Wrapper Component UnitTest     ",this);
		_AInfo("------------------------------------",this);
		if( WrapperComponentUnitTest() == false )   result = false;
	}
	if( result == true )
	{
		_AInfo("====================================",this);
		_AInfo("       Abs Component UnitTest       ",this);
		_AInfo("------------------------------------",this);
		if( AbsTest(true) == false )   result = false;
	}
	if( result == true )
	{
		_AInfo("====================================",this);
		_AInfo("        UnitTest completed.         ",this);
		_AInfo("     All tests are OK!     ",this);
		_AInfo("====================================",this);
	}
	else
	{
		_AInfo("************************************",this);
		_AInfo(" NG!NG!NG!NG!NG!NG!NG!NG!NG!NG! ",this);
		_AInfo(" At least one test is NG!!!",this);
		_AInfo(" NG!NG!NG!NG!NG!NG!NG!NG!NG!NG! ",this);
		_AInfo("************************************",this);
	}
#endif
}
void	AApp::UnitTest2()
{
#if IMPLEMENTATION_FOR_MACOSX

	ABool	result = true;
	SPI_GetTraceLogWindow().NVI_CreateAndShow();
	_AInfo("====================================",this);
	_AInfo("      UnitTest (without wrap test)  ",this);
	_AInfo("====================================",this);
	_AInfo("       Base Component UnitTest      ",this);
	_AInfo("------------------------------------",this);
	if( ABaseFunction::BaseComponentUnitTest() == false )   result = false;
	if( result == true )
	{
		_AInfo("====================================",this);
		_AInfo("     Wrapper Component UnitTest     ",this);
		_AInfo("------------------------------------",this);
		if( WrapperComponentUnitTest() == false )   result = false;
	}
	if( result == true )
	{
		_AInfo("====================================",this);
		_AInfo("       Abs Component UnitTest       ",this);
		_AInfo("------------------------------------",this);
		if( AbsTest(false) == false )   result = false;
	}
	if( result == true )
	{
		_AInfo("====================================",this);
		_AInfo("        UnitTest completed.         ",this);
		_AInfo("     All tests are OK!     ",this);
		_AInfo("====================================",this);
	}
	else
	{
		_AInfo("************************************",this);
		_AInfo(" NG!NG!NG!NG!NG!NG!NG!NG!NG!NG! ",this);
		_AInfo(" At least one test is NG!!!",this);
		_AInfo(" NG!NG!NG!NG!NG!NG!NG!NG!NG!NG! ",this);
		_AInfo("************************************",this);
	}
#endif
}

/*�o������
�����e�X�g���s�̂��߂̊�
�E���ݒ�t�H���_��"debug"�t�@�C�����쐬
�E�{�����[���̃��[�g��mitest�t�H���_�{2�t�@�C����ۑ�
�E�A�v���P�[�V�����t�H���_��mi(a7)�t�H���_�{���g��ۑ�
*/


#pragma mark ===========================
#pragma mark [�N���X]AStEditProgressModalSession
#pragma mark ===========================

/**
���[�_���Z�b�V�����J�n�E�I���p�X�^�b�N�N���X�i�R���X�g���N�^�j
*/
AStEditProgressModalSession::AStEditProgressModalSession( const AEditProgressType inType, 
														 const ABool inEnableCancelButton,
														 const ABool inShowProgressWindowNow,
														const ABool inEnableModalSession ) : mModalSessionStarted(false)
{
	//���[�_���Z�b�V�����J�n
	if( inEnableModalSession == true )
	{
		mModalSessionStarted = GetApp().SPI_StartEditProgressModalSession(inType,inEnableCancelButton,inShowProgressWindowNow);
	}
}

/**
���[�_���Z�b�V�����J�n�E�I���p�X�^�b�N�N���X�i�f�X�g���N�^�j
*/
AStEditProgressModalSession::~AStEditProgressModalSession()
{
	//���[�_���Z�b�V�����I��
	if( mModalSessionStarted == true )
	{
		GetApp().SPI_EndEditProgressModalSession();
	}
}



