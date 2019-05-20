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
#pragma mark [クラス]AApp
#pragma mark ===========================
//アプリケーションオブジェクト

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
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
	//色初期化
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
	//mModePrefDBArrayのリストのリスト途中への追加・削除は行わないので、リストのスレッドmutexは不要（SPI_GetModePrefDB()は多くコールされるので、重くなってしまう）
	mModePrefDBArray.EnableThreadSafe(false);
	
	//Lua用関数登録 #567 #1170
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
	//luaファイルによる関数追加
	AFileAcc	miluaFile;
	AFileWrapper::GetResourceFile("mi.lua",miluaFile);
	AText	errorText;
	NVI_Lua_dofile(miluaFile,errorText);
    
	//EditBoxViewのデフォルトフォント・サイズを設定 win
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
	
	//起動シーケンストレース用
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
	//#460 設定ファイル位置移動
	//~/Library/Application Support/mi3が存在していなければ、旧Preferences位置からコピーする
	if( appPrefFolder.Exist() == false )
	{
		//==================バージョン2.1.11/2.1.12のフォルダからコピー================== #941
		//バージョン2.1.11/2.1.12のフォルダ：~/Library/Application Support/mi
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
			//------------------バージョン2.1.11/2.1.12のフォルダが存在する場合------------------
			
			//モーダルセッション開始
			AStEditProgressModalSession	modalSession(kEditProgressType_CopyV2AppPrefFolder,false,true,true);
			//プログレスIntermediate表示
			SPI_CheckContinueingEditProgressModalSession(kEditProgressType_CopyV2AppPrefFolder,0,true,-1,1);
			
			//旧フォルダから新フォルダへコピー
			appPrefFolderV2.CopyFolderTo(appPrefFolder,true,true);
		}
		else
		{
			//------------------バージョン2.1.11/2.1.12のフォルダが存在しない場合------------------
			
			//==================バージョン2.1.10以前のフォルダからコピー==================
			//旧appPrefフォルダ位置特定
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
				//モーダルセッション
				AStEditProgressModalSession	modalSession(kEditProgressType_CopyV2AppPrefFolder,false,true,true);
				//プログレスIntermediate表示
				SPI_CheckContinueingEditProgressModalSession(kEditProgressType_CopyV2AppPrefFolder,0,true,-1,1);
				
				//旧フォルダから新フォルダへコピー
				oldAppPrefFolder.CopyFolderTo(appPrefFolder,true,true);
			}
		}
	}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	//win MyDocumentから%appdata%へ移動(3.0.0a20)
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
			//旧フォルダから新フォルダへコピー
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
	ATOK2008の場合、文節区切り変更時に下記の0と3の色が同じになるので、区別が付かない。
	【対策】
	グレーモード：
	0: 黒→灰色
	1: 薄灰色→灰色
	2: 灰色→薄灰色（変換中に黒と灰色では見分け付きにくい）→薄灰色(LightGray)では背景白色との区別が付きにくいので60%灰色に変更
	カラーモード：
	0: 赤色→灰色
	
	見分け優先度：
	2と3
	0,1と3
	
	ATOK2008
	0: 最初の状態（まだ変換キーを全く押していない）、文節区切り変更時の次の文節
	1: (未使用？)
	2: 変換対象ではない文節
	3: 変換対象文節
	4: (未使用？)
	5: (未使用？)
	
	ATOK2007
	0: (未使用？)
	1: 最初の状態（まだ変換キーを全く押していない）、文節区切り変更時の次の文節
	2: 変換対象ではない文節
	3: 変換対象文節
	4: (未使用？)
	5: (未使用？)
	
	ことえり
	0: 最初の状態（まだ変換キーを全く押していない）（文節区切り変更時は、自動的に文節区切り済みになるので、0にはならない）
	1: (未使用？)
	2: 変換対象ではない文節
	3: 変換対象文節
	4: (未使用？)
	5: (未使用？)
	
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
	
	//マルチファイル検索スレッド生成
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
	
	//ツールコマンド登録
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

//デストラクタ
AApp::~AApp()
{
	ABaseFunction::DoObjectArrayItemTrashDelete();
	/* #92 NVIDO_Quit()へ移動
	Quit_Phase_UI();
	Quit_Phase_Data();
	*/
}

/**
イメージロード
*/
void	AApp::SPI_LoadImageFiles( const ABool inRemoveOld )
{
	/*#1316
	ABool	useCustom = false;
	AFileAcc	customskinfolder;
	//カスタムskin名取得
	AText	skinName;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kSkinName,skinName);
	//カスタムskinフォルダ取得
	if( skinName.GetItemCount() > 0 )
	{
		AFileAcc	skinroot;
		SPI_GetUserSkinFolder(skinroot);
		customskinfolder.SpecifyChild(skinroot,skinName);
		//カスタムskinフォルダ存在していたら、カスタムの方を使用するフラグON
		if( customskinfolder.Exist() == true && customskinfolder.IsFolder() == true )
		{
			useCustom = true;
		}
	}
	*/
	
	//Image読み込み
	
	//色ロード
	//#1316 LoadColors(useCustom,customskinfolder);
	
	//イメージファイル読み込み
	//#130
	LoadImageFile("iconSwCancel.png",kImageID_iconSwCancel,inRemoveOld);
	LoadImageFile("iconSwCancel_h.png",kImageID_iconSwCancel_h,inRemoveOld);
	LoadImageFile("iconSwCancel_p.png",kImageID_iconSwCancel_p,inRemoveOld);
	LoadImageFile("iconSwSearch.png",kImageID_iconSwSearch,inRemoveOld);
	
	/*#1316
	//Rect20ボタンイメージ
	LoadImageFile("btn20Rect_1.png",kImageID_btn20Rect_1,inRemoveOld);
	LoadImageFile("btn20Rect_2.png",kImageID_btn20Rect_2,inRemoveOld);
	LoadImageFile("btn20Rect_3.png",kImageID_btn20Rect_3,inRemoveOld);
	LoadImageFile("btn20Rect_h_1.png",kImageID_btn20Rect_h_1,inRemoveOld);
	LoadImageFile("btn20Rect_h_2.png",kImageID_btn20Rect_h_2,inRemoveOld);
	LoadImageFile("btn20Rect_h_3.png",kImageID_btn20Rect_h_3,inRemoveOld);
	LoadImageFile("btn20Rect_p_1.png",kImageID_btn20Rect_p_1,inRemoveOld);
	LoadImageFile("btn20Rect_p_2.png",kImageID_btn20Rect_p_2,inRemoveOld);
	LoadImageFile("btn20Rect_p_3.png",kImageID_btn20Rect_p_3,inRemoveOld);
	
	//RoundedRect20ボタンイメージ
	LoadImageFile("btn20RoundedRect_1.png",kImageID_btn20RoundedRect_1,inRemoveOld);
	LoadImageFile("btn20RoundedRect_2.png",kImageID_btn20RoundedRect_2,inRemoveOld);
	LoadImageFile("btn20RoundedRect_3.png",kImageID_btn20RoundedRect_3,inRemoveOld);
	LoadImageFile("btn20RoundedRect_h_1.png",kImageID_btn20RoundedRect_h_1,inRemoveOld);
	LoadImageFile("btn20RoundedRect_h_2.png",kImageID_btn20RoundedRect_h_2,inRemoveOld);
	LoadImageFile("btn20RoundedRect_h_3.png",kImageID_btn20RoundedRect_h_3,inRemoveOld);
	LoadImageFile("btn20RoundedRect_p_1.png",kImageID_btn20RoundedRect_p_1,inRemoveOld);
	LoadImageFile("btn20RoundedRect_p_2.png",kImageID_btn20RoundedRect_p_2,inRemoveOld);
	LoadImageFile("btn20RoundedRect_p_3.png",kImageID_btn20RoundedRect_p_3,inRemoveOld);
	
	//Rect16ボタンイメージ
	LoadImageFile("btn16Rect_1.png",kImageID_btn16Rect_1,inRemoveOld);
	LoadImageFile("btn16Rect_2.png",kImageID_btn16Rect_2,inRemoveOld);
	LoadImageFile("btn16Rect_3.png",kImageID_btn16Rect_3,inRemoveOld);
	LoadImageFile("btn16Rect_h_1.png",kImageID_btn16Rect_h_1,inRemoveOld);
	LoadImageFile("btn16Rect_h_2.png",kImageID_btn16Rect_h_2,inRemoveOld);
	LoadImageFile("btn16Rect_h_3.png",kImageID_btn16Rect_h_3,inRemoveOld);
	LoadImageFile("btn16Rect_p_1.png",kImageID_btn16Rect_p_1,inRemoveOld);
	LoadImageFile("btn16Rect_p_2.png",kImageID_btn16Rect_p_2,inRemoveOld);
	LoadImageFile("btn16Rect_p_3.png",kImageID_btn16Rect_p_3,inRemoveOld);
	
	//Rect16Footerボタンイメージ
	LoadImageFile("panel16Footer_1.png",kImageID_panel16Footer_1,inRemoveOld);
	LoadImageFile("panel16Footer_2.png",kImageID_panel16Footer_2,inRemoveOld);
	LoadImageFile("panel16Footer_3.png",kImageID_panel16Footer_3,inRemoveOld);
	*/
	//ポップアップメニュー用トライアングル
	LoadImageFile("iconSwTriangles1.png",kImageID_iconSwTriangles,inRemoveOld);
	
	//折りたたみ三角形
	LoadImageFile("barSwCursorRight.png",kImageID_barSwCursorRight,inRemoveOld);
	LoadImageFile("barSwCursorRight_h.png",kImageID_barSwCursorRight_h,inRemoveOld);
	LoadImageFile("barSwCursorRight_d.png",kImageID_barSwCursorRight_p,inRemoveOld);
	LoadImageFile("barSwCursorDown.png",kImageID_barSwCursorDown,inRemoveOld);
	LoadImageFile("barSwCursorDown_h.png",kImageID_barSwCursorDown_h,inRemoveOld);
	LoadImageFile("barSwCursorDown_d.png",kImageID_barSwCursorDown_p,inRemoveOld);
	
	/*#1316
	//ファイルイメージ
	LoadImageFile("iconSwDocumentOutline.png",kImageID_iconSwFile,inRemoveOld);
	
	//スクロールバー内ボタン
	LoadImageFile("frameScrlbarTop_106.png",kImageID_frameScrlbarTop_106,inRemoveOld);
	LoadImageFile("framescrlbartop_107.png",kImageID_frameScrlbarTop_107,inRemoveOld);
	*/
	
	//サイドバータイトルアイコン
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
	
	//メインウインドウ内各アイコン
	LoadImageFile("iconSwCompare.png",kImageID_iconSwCompare,inRemoveOld);
	LoadImageFile("iconSwReplace.png",kImageID_iconSwReplace,inRemoveOld);
	LoadImageFile("iconSwShow.png",kImageID_iconSwShow,inRemoveOld);
	LoadImageFile("iconSwDocument.png",kImageID_iconSwDocument,inRemoveOld);
	
	/*#1316
	//タブ背景
	LoadImageFile("bgTabBar_1.png",kImageID_Tab0,inRemoveOld);
	LoadImageFile("bgTabBar_2.png",kImageID_Tab1,inRemoveOld);
	LoadImageFile("bgTabBar_3.png",kImageID_Tab2,inRemoveOld);
	LoadImageFile("bgTabBar_4.png",kImageID_Tab3,inRemoveOld);
	LoadImageFile("bgTabBar_5.png",kImageID_Tab4,inRemoveOld);
	LoadImageFile("bgTabBar_6.png",kImageID_Tab5,inRemoveOld);
	LoadImageFile("bgTabBar_7.png",kImageID_Tab6,inRemoveOld);
	LoadImageFile("bgTabBar_8.png",kImageID_Tab7,inRemoveOld);
	LoadImageFile("bgTabBar_9.png",kImageID_Tab8,inRemoveOld);
	
	//タブボタンアクティブ
	LoadImageFile("tabFront_1.png",kImageID_tabFront_1,inRemoveOld);
	LoadImageFile("tabFront_2.png",kImageID_tabFront_2,inRemoveOld);
	LoadImageFile("tabFront_3.png",kImageID_tabFront_3,inRemoveOld);
	LoadImageFile("tabFront_h_1.png",kImageID_tabFront_h_1,inRemoveOld);
	LoadImageFile("tabFront_h_2.png",kImageID_tabFront_h_2,inRemoveOld);
	LoadImageFile("tabFront_h_3.png",kImageID_tabFront_h_3,inRemoveOld);
	//タブボタン2ndアクティブ
	LoadImageFile("tabNext_1.png",kImageID_tabNext_1,inRemoveOld);
	LoadImageFile("tabNext_2.png",kImageID_tabNext_2,inRemoveOld);
	LoadImageFile("tabNext_3.png",kImageID_tabNext_3,inRemoveOld);
	LoadImageFile("tabNext_h_1.png",kImageID_tabNext_h_1,inRemoveOld);
	LoadImageFile("tabNext_h_2.png",kImageID_tabNext_h_2,inRemoveOld);
	LoadImageFile("tabNext_h_3.png",kImageID_tabNext_h_3,inRemoveOld);
	//タブボタン非アクティブ
	LoadImageFile("tabOther_h_1.png",kImageID_tabOther_h_1,inRemoveOld);
	LoadImageFile("tabOther_h_2.png",kImageID_tabOther_h_2,inRemoveOld);
	LoadImageFile("tabOther_h_3.png",kImageID_tabOther_h_3,inRemoveOld);
	*/
	//タブdirtyマーク
	LoadImageFile("tabDot.png",kImageID_tabDot,inRemoveOld);
	
	//フッター内の各制御ボタン用アイコン
	LoadImageFile("iconScrlbarSeparateVertical.png",kImageID_iconPnSeparateVertical,inRemoveOld);
	LoadImageFile("iconPnLeft.png",kImageID_iconPnLeft,inRemoveOld);
	LoadImageFile("iconPnRight.png",kImageID_iconPnRight,inRemoveOld);
	LoadImageFile("iconPnOption.png",kImageID_iconPnOption,inRemoveOld);
	LoadImageFile("iconPnHandle.png",kImageID_iconPnHandle,inRemoveOld);
	
	//フラグマーク等
	LoadImageFile("iconSwlFlag.png",kImageID_iconSwlFlag,inRemoveOld);
	//#1316 LoadImageFile("iconSwlPin.png",kImageID_iconSwlPin,inRemoveOld);
	LoadImageFile("iconSwlParentPath.png",kImageID_iconSwlParentPath,inRemoveOld);
	
	/*#1316
	//サブウインドウタイトルバー（フローティングウイン
	LoadImageFile("barSwTitleBarWithBorder_1.png",kImageID_SubWindowTitlebar_Floating0,inRemoveOld);
	LoadImageFile("barSwTitleBarWithBorder_2.png",kImageID_SubWindowTitlebar_Floating1,inRemoveOld);
	LoadImageFile("barSwTitleBarWithBorder_3.png",kImageID_SubWindowTitlebar_Floating2,inRemoveOld);
	
	//サブウインドウタイトルバー（オーバーレイウインド
	LoadImageFile("ui_SubWindowTitlebar_Overlay_0.png",kImageID_SubWindowTitlebar_Overlay0,inRemoveOld);
	LoadImageFile("ui_SubWindowTitlebar_Overlay_1.png",kImageID_SubWindowTitlebar_Overlay1,inRemoveOld);
	LoadImageFile("ui_SubWindowTitlebar_Overlay_2.png",kImageID_SubWindowTitlebar_Overlay2,inRemoveOld);
	*/
	
	//サブウインドウクローズボタン
	LoadImageFile("barSwCloseButton_1.png",kImageID_barSwCloseButton_1,inRemoveOld);
	LoadImageFile("barSwCloseButton_h.png",kImageID_barSwCloseButton_h,inRemoveOld);
	LoadImageFile("barSwCloseButton_p.png",kImageID_barSwCloseButton_p,inRemoveOld);
	
	//折りたたみマークアイコン
	LoadImageFile("btnFldngArrowRight.png",kImageID_btnFldngArrowRight,inRemoveOld);
	LoadImageFile("btnFldngArrowRight_h.png",kImageID_btnFldngArrowRight_h,inRemoveOld);
	LoadImageFile("btnFldngArrowDown.png",kImageID_btnFldngArrowDown,inRemoveOld);
	LoadImageFile("btnFldngArrowDown_h.png",kImageID_btnFldngArrowDown_h,inRemoveOld);
	LoadImageFile("btnFldngArrowEnd.png",kImageID_btnFldngArrowEnd,inRemoveOld);
	LoadImageFile("btnFldngArrowEnd_h.png",kImageID_btnFldngArrowEnd_h,inRemoveOld);
	
	//リスト項目用アイコン
	LoadImageFile("iconSwlDocument.png",kImageID_iconSwlDocument,inRemoveOld);
	LoadImageFile("iconSwlFolder.png",kImageID_iconSwlFolder,inRemoveOld);
	
	//ビューを閉じるボタンアイコン
	LoadImageFile("btnEditorClose.png",kImageID_btnEditorClose,inRemoveOld);
	LoadImageFile("btnEditorClose_h.png",kImageID_btnEditorClose_h,inRemoveOld);
	
	//コールグラフ用アイコン
	LoadImageFile("btnNaviRight.png",kImageID_btnNaviRight,inRemoveOld);
	LoadImageFile("btnNaviLeft.png",kImageID_btnNaviLeft,inRemoveOld);
	
	//ホームアイコン
	LoadImageFile("iconSwHome.png",kImageID_iconSwHome,inRemoveOld);
	
	//マクロボタン
	LoadImageFile("iconTbCircleCursorButton.png",kImageID_iconTbCircleCursorButton,inRemoveOld);
	
	//ロックボタン
	LoadImageFile("iconSwLock.png",kImageID_iconSwLock,inRemoveOld);
	LoadImageFile("iconSwUnlock.png",kImageID_iconSwUnlock,inRemoveOld);
	
	//サブウインドウタイトル用アイコン
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
イメージロード
*/
void	AApp::LoadImageFile( /*#1316 const ABool inUseCustom ,const AFileAcc& inCustomFolder, */
							const AConstCharPtr inFilename, const AImageID inImageID,
							const ABool inRemoveOld )
{
	//inRemoveOldがtrueなら旧データを登録解除
	if( inRemoveOld == true )
	{
		CUserPane::UnregisterImage(inImageID);
	}
	/*#1316
	//カスタム使用フラグONならそちらから読み込みを試す
	if( inUseCustom == true )
	{
		//カスタムskin使用
		
		//イメージファイル読み込み
		AFileAcc	file;
		file.SpecifyChild(inCustomFolder,inFilename);
		if( file.Exist() == true )
		{
			CUserPane::RegisterImageByFile(inImageID,file);
			return;
		}
	}
	*/
	
	//標準skin使用
	
	//名前からイメージロード
	AText	name(inFilename);
	CUserPane::RegisterImageByName(inImageID,name);
}

/**
色ロード
*/
/*#1316
void	AApp::LoadColors( const ABool inUseCustom ,const AFileAcc& inCustomFolder )
{
	AFileAcc	colorsFile;
	//inUseCustomがtrueならカスタムcolors.txtを使う
	if( inUseCustom == true )
	{
		colorsFile.SpecifyChild(inCustomFolder,"colors.txt");
	}
	//inUseCustomがfalseまたはカスタムcolors.txtが存在していなければ標準colors.txtを使う
	if( inUseCustom == false || colorsFile.Exist() == false )
	{
		AFileWrapper::GetResourceFile("colors.txt",colorsFile);
	}
	//ロード
	AText	colorsText;
	colorsFile.ReadTo(colorsText);
	//1行ごとに読み込む
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
				//処理なし
				break;
			}
		}
		index++;
	}
}
*/

#pragma mark ===========================

#pragma mark ---起動／終了処理

/**
*/
ABool	AApp::NVIDO_Run()//#182 
{
	//データオブジェクト初期化
	Startup_Phase_Data();
	//UIオブジェクト初期化
	Startup_Phase_UI();
	
	//UIのUpdate（データの反映）
	Startup_Phase_UIUpdate();
	
	//#904
	//JavaScript用mi基本処理実行
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
	//コマンドライン取得、有効なファイルパスがあれば開く
	AText	cmdline;
	CAppImp::GetCmdLine(cmdline);
	if( NVI_OpenFromCmdLine(cmdline) == false )
	{
		//最初に新規ドキュメントを１つ作成
		SPNVI_CreateNewTextDocument(0);
	}
	
#endif
	*/
	ABool	fileexist = false;
	//引数取得
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
	//引数実行 #263
	fileexist = NVI_ExecCommandLineArgs(argTextArray);
	
	//#513 終了時開いていたファイルを再度開く
	//#975 kInternalEvent_ReopenFileは常に送信することにする。EVTDO_DoInternalEvent()内で判定。 if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kReopenFile) == true )
	{
		//#682 ReopenFile();
		//#682 起動時にフローティングウインドウが表示されない問題回避のため、一旦起動処理を完了させてから、
		//Inernal Event経由でReopenさせることにする。
		AObjectIDArray	objectIDArray;
		ABase::PostToMainInternalEventQueue(kInternalEvent_ReopenFile,GetObjectID(),0,GetEmptyText(),objectIDArray);
	}
	
#if IMPLEMENTATION_FOR_WINDOWS
	if( fileexist == false && NVI_GetMostFrontWindowID(kWindowType_Text) == kObjectID_Invalid )
	{
		//最初に新規ドキュメントを１つ作成
		AText	name;
		GetAppPref().GetData_Text(AAppPrefDB::kCmdNModeName,name);
		AModeIndex	modeIndex = SPI_FindModeIndexByModeRawName(name);
		if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
		ADocumentID	docID = SPNVI_CreateNewTextDocument(modeIndex);//B0441
		if( docID == kObjectID_Invalid )   {_ACError("",this);};//win
		SPI_GetTextDocumentByID(docID).SPI_SetCreatedAutomatically(true);//B0441
		::Sleep(250);//アニメーション中に、Overlayウインドウが先に表示されないよう、アニメーション完了待ち
	}
#endif
	
	//ファイルリストウインドウ表示 Startup_Phase_UIUpdate()から移動 win
	/*#725
	if( GetAppPref().GetData_Bool(AAppPrefDB::kDisplayWindowList) == true )
	{
		SPI_GetFileListWindow().NVI_Show(false);
	}
	_AInfo("FileList Show done.",this);
	*/
	/*#182 共通処理をNVI_Run()の中で実行するように変更
	//アプリ実行状態へ移行
	NVM_SetWorking(true);
	
	//win 080728
	NVI_UpdateMenu();
	
	//B0000
	ATrace::StartPostInternalEvent();
	
	_AInfo("Application event loop starts.",this);
	
	//B0372
	//SPI_CreateNewWindowAutomatic();//AppleEvent AppOpenを受信時にコールするよう修正（これだと、書類ダブルクリックとともに起動する時に問題）
	
	//Run()はアプリ終了まで返ってこない
	int result = NVM_GetImp().Run();
	_AInfo("Application event loop ended.",this);
	return result;
	*/
	
	//start up内部イベント送信
	{
		AObjectIDArray	objectIDArray;
		ABase::PostToMainInternalEventQueue(kInternalEvent_StartUp,GetObjectID(),0,GetEmptyText(),objectIDArray);
	}
	
	return true;//#182
}

/**
終了メニュー選択時の処理
@param inContinuing 1つのトランザクション内の続きの処理であることを示す（保存確認で「保存しない」を選んだ場合など）
*/
ABool	AApp::NVIDO_Quit(/*#1102 const ABool inContinuing */)//win 080702 #433
{
	/*win ウインドウを閉じたときにも保存必要（閉じる前に保存必要）なので各イベント受信直後へ処理を移動
	//#513 終了時開いていたファイルのパスを記憶
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kReopenFile) == true && inContinuing == false )
	{
		SPI_SaveReopenFile();
	}
	*/
	
	//モード設定ダイアログに未反映のデータがあれば中止
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
				//新規ドキュメントを１つ作成
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
	
	//最もタブが多いウインドウにてサイドバーのデータを保存（現在のリモートファイルのフォルダを保存するため）#361
	AWindowID	mostTabWindowID = SPI_GetMostNumberTabsTextWindowID();
	if( mostTabWindowID != kObjectID_Invalid )
	{
		SPI_GetTextWindowByID(mostTabWindowID).SPI_SaveSideBarData();
	}
	
	//#513
	//reopen保存 #1102 Quitメニュー処理から移動
	SPI_SaveReopenFile();
	
	//ウインドウ全クローズ
	if( NVI_CloseAllWindow() == false )
	{
		//上で保存したreopenデータをクリア #1102
		SPI_ClearReopenData();
		//クローズ時の保存確認ダイアログキャンセル時は終了せずリターン
		return false;//#182
	}
	
	//終了状態設定 #688
	NVM_SetWorking(false);
	
	//#92 Document全削除よりも前にしないと、Document使用のWindowを削除時にDocumentID参照エラー発生する
	Quit_Phase_UI();
	Quit_Phase_Data();
	
	/*#688 AApplication::NVI_Quit()へ移動
	//ドキュメント全削除
	NVI_DeleteAllDocument();
	*/
	
	//スレッド終了
	//マルチファイル検索スレッド終了
	if( mMultiFileFinderObjectID != kObjectID_Invalid )
	{
		NVI_DeleteThread(mMultiFileFinderObjectID);
	}
	//インポートファイル認識スレッド終了
	if( mImportFileRecognizerObjectID != kObjectID_Invalid )
	{
		NVI_DeleteThread(mImportFileRecognizerObjectID);
	}
	//FTP接続スレッド終了
	if( mFTPConnectionObjectID != kObjectID_Invalid )
	{
		NVI_DeleteThread(mFTPConnectionObjectID);
	}
	//AppleScript実行スレッド終了
	if( mAppleScriptExecuterObjectID != kObjectID_Invalid )
	{
		NVI_DeleteThread(mAppleScriptExecuterObjectID);
	}
	
	//Imp側を終了（これにより、本イベントトランザクション終了後、Run()が終了する。）
	//#182 NVM_GetImp().Quit();
	
	return true;//#182
}

void	AApp::Startup_Phase_Data()
{
	try
	{
		//アプリケーションの更新時刻を取得（モード設定のdefaultからの自動更新判定に使用する）
		AFileAcc	appFile;
		AFileWrapper::GetAppFile(appFile);
		ADateTime	datetime;
		appFile.GetLastModifiedDataTime(datetime);
		ADateTimeWrapper::GetDateTimeText(datetime,mAppModifiedDataTimeText);
		
		//バージョン2.1での標準添付ツールバーを非表示にするための、標準添付ツールバー名称リストを取得
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
		
		//イメージファイル読み込み
		SPI_LoadImageFiles(false);
		
		//win 多言語対応
#if IMPLEMENTATION_FOR_WINDOWS
		CAppImp::SetupLanguageSateliteDLL((ALanguage)(mAppPref.GetData_Number(AAppPrefDB::kLaunguage)));
#endif
		
		//win 多言語対応
		//AAppコンストラクタから移動
		//メニュー関連初期化
		NVI_GetMenuManager().InitMenuBarMenu();
		_AInfo("InitMenuBarMenu() done.",this);
		//コンテクストメニュー設定
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
		//#0 未使用のためコメントアウト NVI_GetMenuManager().RegisterContextMenu("menu/ContextMenu_Diff",kContextMenuID_Diff);
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
		//動的メニュー設定
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
		//TextArrayメニュー登録
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
		//正規表現サンプルメニュー作成
		{
			//"data/（各言語）"フォルダを取得
			AFileAcc	folder;
			SPI_GetLocalizedDataFolder(folder);
			//メニュー作成
			AFileAcc	regExpList_Menu_File;
			regExpList_Menu_File.SpecifyChild(folder,"RegExpList_Menu.txt");
			AText	menutext;
			regExpList_Menu_File.ReadTo(menutext);
			ATextArray	menutextarray;
			menutextarray.ExplodeFromText(menutext,kUChar_LF);
			NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_RegExpList,menutextarray);
			//メニューに対応する正規表現を取得
			AFileAcc	regExpList_RegExp_File;
			regExpList_RegExp_File.SpecifyChild(folder,"RegExpList_RegExp.txt");
			AText	regexptext;
			regExpList_RegExp_File.ReadTo(regexptext);
			mRegExpListArray.ExplodeFromText(regexptext,kUChar_LF);
		}
		
		//#889
		//カラースキームメニュー作成
		{
			//メニュー登録
			NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_ColorScheme);
			NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_AppColorScheme);//#1316
			//メニュー更新
			SPI_UpdateColorSchemeMenu();
		}
		
		//#193
		{
			//メニュー登録
			NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_AccessPlugIn);
			//メニュー更新
			SPI_UpdateAccessPlugInMenu();
		}
		
		/*#1316
		//skinメニュー作成
		{
			//メニュー登録
			NVI_GetTextArrayMenuManager().RegisterTextArrayMenuID(kTextArrayMenuID_Skin);
			//メニュー更新
			SPI_UpdateSkinMenu();
		}
		*/
		
		//defaultモードのモード名メニュー生成（自動更新元モード）
		{
			//メニューテキスト
			ATextArray	menutextarray;
			ATextArray	actionTextArray;
			//最初の項目は「同じモード名のモード」にする
			AText	text;
			text.SetLocalizedText("ModeUpdate_SourceIsSameNameMode");
			menutextarray.Add(text);
			actionTextArray.Add(GetEmptyText());
			//defaultフォルダの各フォルダ名を取得して、メニューに追加
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
			//メニュー設定
			AApplication::GetApplication().NVI_GetTextArrayMenuManager().
					UpdateTextArrayMenu(kTextArrayMenuID_DefaultMode,menutextarray,actionTextArray);
		}
		
		//ContextMenuにTextArrayMenuを登録
		NVI_GetMenuManager().SetContextMenuTextArrayMenu(kContextMenuID_TextEncoding,kTextArrayMenuID_TextEncoding);//#232
		NVI_GetMenuManager().SetContextMenuTextArrayMenu(kContextMenuID_Mode,kTextArrayMenuID_EnabledMode);//#914
		//Fontメニュー登録
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
		//検索パラメータ初期化（現在の検索パラメータデータはAAppで管理）
		InitFindParam();
		
		//#454
		mJumpListHistory.SetFromTextArray(NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kJumpListHistory));
		mJumpListHistory_Path.SetFromTextArray(NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kJumpListHistory_Path));
		//メニュー更新
		GetApp().NVI_GetMenuManager().SetContextMenuTextArray(kContextMenuID_JumpListHistory,mJumpListHistory,mJumpListHistory_Path);
		
		//#717
		//補完リスト履歴取得
		mRecentCompletionWordArray.SetFromTextArray(NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kRecentCompletionWordArray));
		
		//テキストマーカーindex取得
		mCurrentTextMarkerGroupIndex = NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kTextMarkerCurrentGroupIndex);
	}
	catch(...)
	{
		_ACError("AApp::Startup_Phase_Data() caught exception.",NULL);//#199
	}
	/*#369 下記で変換ファイルを作成したが、サロゲートペア文字も含まれるため、変換テーブル方式はやめることにした
	ATextEncoding	macjapanese;
	ATextEncodingWrapper::GetTextEncodingFromName("x-mac-japanese",macjapanese);
	AText	sjis0213unicode;
	AText	gaijiunicode;
	//第１バイト：81-9F, E0-FC
	for( unsigned char ch1 = 0xF0; ch1 <= 0xFC; ch1++ )
	{
		//第２バイト：40-7E, 80-FC
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
	//第２バイト：40-7E, 80-FC
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
	/*下記で調べたところtec==1のみF040->E000になる
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
		//「同じプロジェクト」復元
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
		//「同じフォルダ」復元
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
		//#1275 高速化のためウインドウ表示操作するまでは生成しない /*#199 mTraceLogWindow*/SPI_GetTraceLogWindow().NVI_Create(kObjectID_Invalid);
		
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
	/*NVIDO_Run()内へ移動 win
	//ファイルリストウインドウ表示
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
	//#679 キー記録再生ウインドウ
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
	
	//最近開いたファイルの設定
	SPI_UpdateRecentlyOpenedFile(false);
	//#932 SPI_DeleteRecentlyOpenedFileNotFound();//not found項目を削除 #921
	_AInfo("SPI_UpdateRecentlyOpenedFile() done.",this);
	
	//#844 SPI_UpdateFixedMenuShortcut();
	//#844 _AInfo("SPI_UpdateFixedMenuShortcut() done.",this);
	
	//#750
	//テキストマーカーメニュー更新
	UpdateTextMarkerMenu();
	
	//#725
	//フローティングサブウインドウをreopen
	try
	{
		ReopenFloatingSubWindows();
	}
	catch(...)
	{
	}
	//#81
	//未保存データを表示
	OpenUnsavedData();
}


void	AApp::Quit_Phase_UI()
{
	_AInfo("AApp::Quit_Phase_UI() started.",this);
	//AApp保持の各種ウインドウの終了処理
	//#291 GetAppPref().SetData_Bool(AAppPrefDB::kDisplayWindowList,/*#199 mFileListWindow*/SPI_GetFileListWindow().NVI_IsVisibleMode());//B0404
	//#291 GetAppPref().SetData_Bool(AAppPrefDB::kDisplayIdInfoWindow,/*#199 mIdInfoWindow*/SPI_GetIdInfoWindow().NVI_IsVisibleMode());//RC2
	
	
	//#725
	//フローティングサブウインドウの位置・サイズを保存
	SaveFloatingSubWindows();
	
	//#725
	//サブウインドウ削除
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
	//#199 ウインドウオブジェクト削除
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
	//実行可能ファイルインポート警告ウインドウ削除
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
	//各モード設定ウインドウごとのループ
	for( AIndex modeIndex = 0; modeIndex < mModePrefWindowIDArray.GetItemCount(); modeIndex++ )
	{
		AWindowID	modePrefWindowID = mModePrefWindowIDArray.Get(modeIndex);
		if( modePrefWindowID != kObjectID_Invalid )
		{
			//モード設定ウインドウを削除
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
	
	//履歴を記憶しない設定の場合は、各履歴を削除
	if( NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )
	{
		//最近開いたファイルデータ削除
		SPI_DeleteAllRecentlyOpenedFile();
		//検索履歴削除
		NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFindString);
		NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kFindStringCache);
		NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kReplaceStringCache);
		NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder);
	}
	
	//テキストマーカーindexをDBに保存
	NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kTextMarkerCurrentGroupIndex,mCurrentTextMarkerGroupIndex);
	
	//#717
	//補完候補履歴をDBに保存
	NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kRecentCompletionWordArray,mRecentCompletionWordArray);
	
	//「同じプロジェクト」保存
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
	//「同じフォルダ」保存
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
	//ファイルリストの「同じプロジェクト」の折りたたみ状態を記憶
	{
		//#723
		//排他制御
		AStMutexLocker	locker(mSameProjectArrayMutex);
		
		//collapsedataにcollapseすべきフォルダのパスを格納していき、DBに保存する（違うプロジェクトでも同じリスト内に保存する）
		ATextArray	collapsedata;
		//プロジェクト毎のループ
		AItemCount	projectCount = mSameProject_ProjectFolder.GetItemCount();
		for( AIndex i = 0; i < projectCount; i++ )
		{
			//プロジェクトフォルダパスを取得
			AText	projectfolderpath;
			mSameProject_ProjectFolder.GetObjectConst(i).GetPath(projectfolderpath);
			projectfolderpath.RemoveLastPathDelimiter();
			//プロジェクト内の折りたたまれたフォルダの相対パスを取得
			const ATextArray&	c = mSameProjectArray_CollapseData.GetObjectConst(i);
			//折りたたまれたフォルダデータ毎のループ
			for( AIndex j = 0; j < c.GetItemCount(); j++ )
			{
				//フォルダパスを取得（プロジェクトのパス＋折りたたまれたフォルダの相対パス）
				AText	t;
				c.Get(j,t);
				t.InsertText(0,projectfolderpath);
				//フォルダパスを追加
				collapsedata.Add(t);
			}
		}
		//DBに保存
		mAppPref.SetData_TextArray(AAppPrefDB::kFileListCollapseFolder,collapsedata);
	}
	
	//app pref DBをファイルに保存
	mAppPref.SaveToFile();
	
	//ロード済みのモード設定を保存する
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
	//#379 Diff用レポジトリテキストファイル削除
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
	/*#693 AApplication::~AApplication()へ移動
	//tempフォルダ全体を削除（ファイル・フォルダすべて）
	AFileAcc	tempFolder;
	SPI_GetTempFolder(tempFolder);
	tempFolder.DeleteFileOrFolderRecursive();
	*/
	
	_AInfo("AApp::Quit_Phase_Data() ended.",this);
}

//#263
/**
コマンドライン引数実行
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
		//ファイル以外のコマンド
		if( arg.GetItemCount() > 0 )
		{
			AUChar	ch = arg.Get(0);
			//行番号指定
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
			//カレントディレクトリ指定 #1111
			//（プロセス重複起動により、AppleEventでコマンドライン受信した場合、送信元のプロセスの現在ディレクトリが':'の後に送信される。）
			if( ch == ':' )
			{
				arg.Delete1(0);
				AStCreateNSStringFromAText	curdirstr(arg);
				[[NSFileManager defaultManager] changeCurrentDirectoryPath:curdirstr.GetNSString()];
				continue;
			}
		}
		//ファイルを開く
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
終了時開いているファイルを記憶
*/
void	AApp::SPI_SaveReopenFile()
{
	//win 設定確認をコール元から移動
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kReopenFile) == false )   return;
	
	//データをまず削除
	SPI_ClearReopenData();//#1102 関数化
	//ウインドウindex（reopenデータのウインドウを区別するためのindex）
	AIndex	windowIndex = 0;//#850
	//ウインドウ毎のループ
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			//テキストウインドウについて処理
			const AWindow_Text&	textWindow = SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID());
			//サイドバー、サブテキスト表示状態取得
			ABool	leftSideBarDisplayed = textWindow.SPI_IsLeftSideBarDisplayed();
			ABool	rightSideBarDisplayed = textWindow.SPI_IsRightSideBarDisplayed();
			ABool	subTextDisplayed = textWindow.SPI_IsSubTextColumnDisplayed();
			//メインカラムのboundsを取得
			ARect	bounds = {0};
			textWindow.SPI_GetWindowMainColumnBounds(bounds);
			//タブ毎のループ(z-order順)
			ABool	reopenTabZOrder = 0;
			for( AIndex zorder = 0; zorder < textWindow.NVI_GetSelectableTabCount(); zorder++ )
			{
				//タブの表示index取得
				AIndex	tabIndex = textWindow.NVI_GetTabIndexByZOrderIndex(zorder);
				AIndex	tabDisplayIndex = textWindow.SPI_GetDisplayTabIndexByTabIndex(tabIndex);
				//ドキュメント取得
				const ADocument_Text&	document = GetApp().SPI_GetTextDocumentByID(textWindow.NVI_GetDocumentIDByTabIndex(tabIndex));
				//#630 ODBドキュメントはリオープンしないようにする
				if( document.SPI_IsODBMode() == true )
				{
					continue;
				}
				//リオープン用ファイルパス取得
				AText	path;
				ABool	isRemoteFile = false;
				document.SPI_GetReopenFilePath(path,isRemoteFile);
				//パステキストが空ならデータ追加しない
				if( path.GetItemCount() == 0 )
				{
					continue;
				}
				//現在サブテキストに表示されているかどうかを取得
				ABool	shownInSubText = false;
				if( textWindow.SPI_GetCurrentSubTextPaneDocumentID() == document.GetObjectID() )
				{
					shownInSubText = true;
				}
				//reopenデータ追加
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
				//タブz-order順インクリメント
				reopenTabZOrder++;
			}
			//ウインドウindexインクリメント
			windowIndex++;
		}
	}
}

//#1102
/**
reopenデータをクリア
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
終了時に開いていたファイルを、起動時に再度開く
*/
void	AApp::ReopenFile()
{
	//#573 まず一旦ローカル変数に読み込んで、DBのほうはデータ消去して保存しておく。
	//ファイルを開くのに異常に時間がかかる場合(#571等)、何度miを起動しても同じになるのを防止するため
	ATextArray	pathArray;
	ABoolArray	selectedArray;
	ANumberArray	tabZOrderArray;
	ANumberArray	tabDisplayIndexArray;
	ABoolArray	isRemoteFileArray;
	ANumberArray	windowWidthArray, windowHeightArray, windowXArray, windowYArray;
	ABoolArray	displayLeftSideBarArray, displayRightSideBarArray, displaySubTextArray, shownInSubTextArray;
	//データ取得
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
	/* mi起動中のreopenデータ削除はしないことにする（途中終了とかでタブが消えるのもダメージ大きいので。） #1239
	//reopenデータ削除
	SPI_ClearReopenData();//#1102
	NVI_GetAppPrefDB().SaveToFile();
	*/
	
	//#923
	//最近開いたファイルへの追加を抑制
	mSuppressAddToRecentlyOpenedFile = true;
	//
	AHashArray<AObjectID>				createdWinIDArray;
	AObjectArray<AHashNumberArray>		createdWinIDArray_TabDisplayIndexInDataArray;
	AObjectArray< AArray<AObjectID> >	createdWinIDArray_DocumentIDArray;
	AArray<ANumber>						createdWinIDArray_MaxTabDisplayIndex;
	//ファイルを開く
	AWindowID	winID = kObjectID_Invalid;//#852
	ADocumentID	selectDocumentID = kObjectID_Invalid;
	AItemCount	itemCount = pathArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//==================bounds取得==================
		ARect	bounds = {0};
		bounds.left		= windowXArray.Get(i);
		bounds.top		= windowYArray.Get(i);
		bounds.right	= bounds.left + windowWidthArray.Get(i);
		bounds.bottom	= bounds.top  + windowHeightArray.Get(i);
		//==================ウインドウ生成判定==================
		//最初の項目、または、一番上のタブの場合、ウインドウ生成フラグをONにする
		ABool	createWindow = false;
		if( i == 0 || tabZOrderArray.Get(i) == 0 )
		{
			createWindow = true;
		}
		//==================ドキュメント生成==================
		ADocumentID	docID = kObjectID_Invalid;
		if( isRemoteFileArray.Get(i) == false )
		{
			//------------------ローカルファイルの場合------------------
			//パス取得
			AFileAcc	file;
			file.Specify(pathArray.GetTextConst(i));
			//ドキュメント生成
			//#932 存在チェックせずにドキュメント／ウインドウ生成する if( file.Exist() == true )
			{
				//ファイルが存在していたら、ドキュメントを生成（ウインドウ生成するかどうかは、上での判定に従う）
				docID = SPNVI_CreateDocumentFromLocalFile(file,GetEmptyText(),kObjectID_Invalid,true,
							kIndex_Invalid,kIndex_Invalid,(i==0),winID,createWindow,
							bounds,true,displayLeftSideBarArray.Get(i),displayRightSideBarArray.Get(i),displaySubTextArray.Get(i));
			}
			/*#932
			else
			{
				//ファイルが存在していない場合は何もしない
				continue;
			}
			 */
		}
		else
		{
			//------------------リモートファイルの場合------------------
			//下記だとタブ表示位置が変わってしまうので、ドキュメント生成後にリモートファイル読み込みする対応が必要。 
			//タブ位置は変わってしまうが、とりあえず対応 #1000
			AText	remoteFileURL;
			pathArray.Get(i,remoteFileURL);
			SPI_RequestOpenFileToRemoteConnection(remoteFileURL,false);
			continue;
		}
		//アクティブだったなら選択対象ウインドウに設定（後でinternal eventによりウインドウselect）
		if( selectedArray.Get(i) == true )
		{
			selectDocumentID = docID;
		}
		//==================生成ウインドウリスト==================
		//ドキュメントのウインドウを取得
		winID = SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
		//生成ウインドウリストからウインドウを検索
		AIndex	createdWinIDIndex = createdWinIDArray.Find(winID);
		if( createdWinIDIndex == kIndex_Invalid )
		{
			//生成したウインドウについて、update view boundsを抑制
			SPI_GetTextWindowByID(winID).SPI_IncrementSuppressUpdateViewBoundsCounter();
			//生成ウインドウリストに項目追加
			createdWinIDIndex = createdWinIDArray.Add(winID);
			createdWinIDArray_TabDisplayIndexInDataArray.AddNewObject();
			createdWinIDArray_DocumentIDArray.AddNewObject();
			createdWinIDArray_MaxTabDisplayIndex.Add(-1);
		}
		//生成ウインドウリストに各タブのデータを保存
		AIndex	tabDisplayIndex = tabDisplayIndexArray.Get(i);
		createdWinIDArray_TabDisplayIndexInDataArray.GetObject(createdWinIDIndex).Add(tabDisplayIndex);
		createdWinIDArray_DocumentIDArray.GetObject(createdWinIDIndex).Add(docID);
		if( tabDisplayIndex > createdWinIDArray_MaxTabDisplayIndex.Get(createdWinIDIndex) )
		{
			createdWinIDArray_MaxTabDisplayIndex.Set(createdWinIDIndex,tabDisplayIndex);
		}
		//==================サブテキスト表示復元==================
		//サブテキストに表示されていたかどうかのフラグがONかつ、サブテキストカラム表示されているなら、サブテキストに表示する
		if( SPI_GetTextWindowByID(winID).SPI_IsSubTextColumnDisplayed() == true )
		{
			if( shownInSubTextArray.Get(i) == true )
			{
				SPI_GetTextWindowByID(winID).SPI_DisplayTabInSubText(SPI_GetTextWindowByID(winID).NVI_GetTabIndexByDocumentID(docID),true);
			}
		}
	}
	//==================生成ウインドウリスト毎のループ==================
	for( AIndex i = 0; i < createdWinIDArray.GetItemCount();i++ )
	{
		AWindowID	winID = createdWinIDArray.Get(i);
		//------------------タブ表示順の若い方から順にタブ表示順を設定していく------------------
		AIndex	displayTabIndex = 0;
		AIndex	displayTabIndexMax = createdWinIDArray_MaxTabDisplayIndex.Get(i);
		for( AIndex j = 0; j <= displayTabIndexMax; j++ )
		{
			//タブ表示順がj番目のindexを検索
			AIndex	index = createdWinIDArray_TabDisplayIndexInDataArray.GetObjectConst(i).Find(j);
			if( index != kIndex_Invalid )
			{
				//項目index（タブ表示順がj番目の項目）のタブに表示順を設定
				ADocumentID	docID = createdWinIDArray_DocumentIDArray.GetObjectConst(i).Get(index);
				SPI_GetTextWindowByID(winID).SPI_SetDisplayTabIndexByDocumentID(docID,displayTabIndex);
				//タブ表示indexインクリメント（displayTabIndexはjでも良いかもしれない？）
				displayTabIndex++;
			}
		}
		//update view boundsの抑制を解除
		SPI_GetTextWindowByID(winID).SPI_DecrementSuppressUpdateViewBoundsCounter();
		//update view bounds
		SPI_GetTextWindowByID(winID).SPI_UpdateViewBounds();
	}
	//ウインドウ選択
	if( selectDocumentID != kObjectID_Invalid )
	{
		//#678 SPI_GetTextDocumentByID(selectDocumentID).NVI_RevealDocumentWithAView();
		//#678 deactivate/activateのイベントが沢山発生して不整合発生しているっぽいので、
		//一旦上記によるdeactivate/activate処理完了後、Internal Eventでウインドウselectすることにする
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(selectDocumentID);
		ABase::PostToMainInternalEventQueue(kInternalEvent_RevealDocument,GetObjectID(),0,GetEmptyText(),objectIDArray);
	}
	//#688
	//Reopen処理終了フラグ設定（SPI_CreateNewWindowAutomatic()での新規ファイル生成禁止解除）
	mReopenProcessCompleted = true;
	//自動新規ファイル生成
	//（起動時にEVT_OpenUntitledFile()経由でSPI_CreateNewWindowAutomatic()がコールされるが、
	//　mReopenProcessCompletedフラグがfalseの間は実行せず、prependフラグをtrueにするのみとしている。
	//　prependされた新規ファイル自動生成処理をここで実行する。 #1056）
	DoPrependedOpenUntitledFileEvent();
	//#923
	//最近開いたファイルへの追加を抑制を解除
	mSuppressAddToRecentlyOpenedFile = false;
}

//#81
/**
未保存データの表示
*/
void	AApp::OpenUnsavedData()
{
	//UnsavedDataフォルダ取得
	AFileAcc	unsavedDataRootFolder;
	GetApp().SPI_GetUnsavedDataFolder(unsavedDataRootFolder);
	//UnsavedDataフォルダ内のフォルダリストを取得
	AObjectArray<AFileAcc>	unsavedDataFolderArray;
	unsavedDataRootFolder.GetChildren(unsavedDataFolderArray);
	//UnsavedDataフォルダ内の各フォルダごとのループ
	AItemCount	itemCount = unsavedDataFolderArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//UnsavedDataフォルダ内の各フォルダを取得
		AFileAcc	folder;
		folder.CopyFrom(unsavedDataFolderArray.GetObjectConst(i));
		//フォルダ内のAutoSave形式の名前のファイルリストを取得
		AArray<ANumber64bit>	fileNameArray;
		ADocument_Text::GetAutoBackupNumberArray(folder,fileNameArray);
		if( fileNameArray.GetItemCount() > 0 )
		{
			//AutoSave形式の名前のファイルリストの最後の項目を取得
			AText	filename;
			filename.SetNumber64bit(fileNameArray.Get(fileNameArray.GetItemCount()-1));
			
			//==================未保存データが保存されたファイルを開く==================
			//ファイルを取得
			AFileAcc	unsavedfile;
			unsavedfile.SpecifyChild(folder,filename);
			//ドキュメントのファイルパスを読み込み
			AFileAcc	filepathfile;
			filepathfile.SpecifyChild(folder,"filepath");
			AText	filepath;
			filepathfile.ReadTo(filepath);
			//ドキュメントファイルを取得
			AFileAcc	docfile;
			docfile.Specify(filepath);
			//ドキュメント生成
			ADocumentID	docID = kObjectID_Invalid;
			if( docfile.Exist() == true )
			{
				//ドキュメントファイルが存在していれば、そのファイルからドキュメント生成
				docID = SPNVI_CreateDocumentFromLocalFile(docfile);
			}
			else
			{
				//ドキュメントファイルが存在していなければ、新規ドキュメント生成
				docID = SPNVI_CreateNewTextDocument();
			}
			//
			//★そのまま終了対策必要SPI_GetTextDocumentByID(docID).NVI_SetDirty(true);
			//★リモートファイル対応
			
			//==================未保存データファイルを比較対象にする==================
			//生成したドキュメントに比較モード設定（未保存データとの比較）
			SPI_GetTextDocumentByID(docID).SPI_SetDiffMode_File(unsavedfile,kCompareMode_UnsavedData);
			//比較表示
			AWindowID	winID = SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			SPI_GetTextWindowByID(winID).SPI_SetDiffDisplayMode(true);
			//notification表示
			SPI_GetTextWindowByID(winID).SPI_GetMainTextViewForDocumentID(docID).SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_SetNotification_UnsavedFile();
			SPI_GetTextWindowByID(winID).SPI_GetMainTextViewForDocumentID(docID).SPI_ShowNotificationPopupWindow(true);
		}
		//フォルダ削除
		folder.DeleteFileOrFolderRecursive();
	}
}

/**
カラースキームメニュー更新
*/
void	AApp::SPI_UpdateColorSchemeMenu()
{
	ATextArray	textArray;
	AText	text;
	//#1316
	ATextArray	textArray2;
	text.SetLocalizedText("ModeColors");
	textArray2.Add(text);
	//------------------アプリ内カラースキーム------------------
	{
		//カラースキームフォルダ取得
		AFileAcc	folder;
		SPI_GetAppColorSchemeFolder(folder);
		//カラースキームフォルダ内ファイルごとのループ
		AObjectArray<AFileAcc>	childArray;;
		folder.GetChildren(childArray);
		AItemCount	itemCount = childArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//ファイル名取得
			AText	filename;
			childArray.GetObjectConst(i).GetFilename(filename);
			//除外ファイル
			if( filename.Compare(".DS_Store") == true )   continue;
			//拡張子取得
			AText	suffix;
			filename.GetSuffix(suffix);
			if( suffix.Compare(".csv") == true )
			{
				//".csv"部分削除
				filename.DeleteAfter(filename.GetItemCount()-4);
			}
			//メニュー表示リストにテキスト追加
			textArray.Add(filename);
			textArray2.Add(filename);//#1316
		}
	}
	//------------------ユーザー定義カラースキーム------------------
	{
		//カラースキームフォルダ取得
		AFileAcc	folder;
		SPI_GetUserColorSchemeFolder(folder);
		//カラースキームフォルダ内ファイルごとのループ
		AObjectArray<AFileAcc>	childArray;;
		folder.GetChildren(childArray);
		AItemCount	itemCount = childArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//ファイル名取得
			AText	filename;
			childArray.GetObjectConst(i).GetFilename(filename);
			//除外ファイル
			if( filename.Compare(".DS_Store") == true )   continue;
			//拡張子取得
			AText	suffix;
			filename.GetSuffix(suffix);
			if( suffix.Compare(".csv") == true || suffix.Compare(".epf") == true )
			{
				//".csv"部分削除
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
			//メニュー表示リストにテキスト追加
			textArray.Add(filename);
		}
	}
	
	//------------------メニュー更新------------------
	NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_ColorScheme,textArray);
	
	//#1316
	NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_AppColorScheme,textArray2);
}

//#193
/**
アクセスプラグインメニュー更新
*/
void	AApp::SPI_UpdateAccessPlugInMenu()
{
	mAccessPluginMenuTextArray.DeleteAll();
	//
	AText	text;
	//------------------アプリ内アクセスプラグインフォルダ------------------
	{
		//アクセスプラグインフォルダ取得
		AFileAcc	folder;
		SPI_GetAppAccessPluginFolder(folder);
		//アクセスプラグインフォルダ内ファイルごとのループ
		AObjectArray<AFileAcc>	childArray;;
		folder.GetChildren(childArray);
		AItemCount	itemCount = childArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//ファイル取得
			AFileAcc	file = childArray.GetObjectConst(i);
			//ファイル名取得
			AText	filename;
			file.GetFilename(filename);
			//
			if( file.IsFolder() == false && file.IsInvisible() == false )
			{
				//メニュー表示リストにテキスト追加
				mAccessPluginMenuTextArray.Add(filename);
			}
		}
	}
	//#1231 メニュー順番入れ替え（FTPはもうセキュリティ上推奨できないので）
	text.SetCstring("FTP");
	mAccessPluginMenuTextArray.Add(text);
	/*★未対応
	//------------------ユーザー定義アクセスプラグインフォルダ------------------
	{
		//アクセスプラグインフォルダ取得
		AFileAcc	folder;
		SPI_GetUserAccessPluginFolder(folder);
		//アクセスプラグインフォルダ内ファイルごとのループ
		AObjectArray<AFileAcc>	childArray;;
		folder.GetChildren(childArray);
		AItemCount	itemCount = childArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//ファイル取得
			AFileAcc	file = childArray.GetObjectConst(i);
			//ファイル名取得
			AText	filename;
			file.GetFilename(filename);
			//
			if( file.IsFolder() == false && file.IsInvisible() == false )
			{
				//メニュー表示リストにテキスト追加
				mAccessPluginMenuTextArray.Add(filename);
			}
		}
	}
	*/
	//------------------メニュー更新------------------
	NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_AccessPlugIn,mAccessPluginMenuTextArray);
}

/**
バージョン2.1での標準添付ツールバー名称（バージョン3移行時に非表示にすべきもの）かどうかを取得
*/
ABool	AApp::SPI_IsV2ToolbarItemNameToHide( const AText& inName ) const
{
	return (mV2ToolbarItemNameToHideArray.Find(inName)!=kIndex_Invalid);
}

/**
skinメニュー更新
*/
/*#1316
void	AApp::SPI_UpdateSkinMenu()
{
	ATextArray	textArray;
	AText	text;
	text.SetLocalizedText("DefaultSkinName");
	textArray.Add(text);
	//------------------skinフォルダのフォルダをメニューに追加------------------
	//skinフォルダ取得
	AFileAcc	folder;
	SPI_GetUserSkinFolder(folder);
	//skinフォルダ内ファイルごとのループ
	AObjectArray<AFileAcc>	childArray;;
	folder.GetChildren(childArray);
	AItemCount	itemCount = childArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AFileAcc	file;
		file = childArray.GetObjectConst(i);
		//ファイル名取得
		AText	filename;
		file.GetFilename(filename);
		//
		if( file.IsFolder() == true )
		{
			//メニュー表示リストにテキスト追加
			textArray.Add(filename);
		}
	}
	//------------------メニュー更新------------------
	NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_Skin,textArray);
}
*/

/**
skinルートフォルダ取得
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
URLスキーム実行
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
タブセットから開く
*/
void	AApp::OpenTabSet( const AFileAcc& inTabSetFile )
{
	ABool	first = true;
	AWindowID	winID = kObjectID_Invalid;
	//タブセットファイル読み込み
	AText	text;
	inTabSetFile.ReadTo(text);
	for( AIndex pos = 0; pos < text.GetItemCount(); )
	{
		//行ごとに読み込み、ファイル特定
		AText	path;
		text.GetLine(pos,path);
		if( path.Contains("://") == true )
		{
			//リモートファイルオープン
			SPI_RequestOpenFileToRemoteConnection(path,false);
		}
		else
		{
			//ドキュメント生成
			AFileAcc	file;
			file.Specify(path);
			ADocumentID	docID = SPNVI_CreateDocumentFromLocalFile(file,GetEmptyText(),kObjectID_Invalid,true,kIndex_Invalid,kIndex_Invalid,false,
																  winID,first);
			//ウインドウ取得（次回以降に同じウインドウにタブを開く）
			winID = SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			first = false;
		}
	}
}

//#1034
/**
ウインドウをウインドウorder順に取得
（AppleScriptから使用される。）
派生クラスによるウインドウ追加
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

#pragma mark ---モード初期化

/**
ModePref初期化
*/
void	AApp::InitModePref()
{
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_InitMode,false,false,true);
	
	//==================ルートのモードフォルダ（"mode"）取得（未生成なら生成）==================
	//modeフォルダ作成／特定
	AFileAcc	rootModeFolder;
	SPI_GetModeRootFolder(rootModeFolder);//アプリケーション(.app)と同列の_modeフォルダ（優先）か、Prefフォルダのmodeフォルダ
	if( rootModeFolder.Exist() == false )
	{
		rootModeFolder.CreateFolderRecursive();
	}
	
	//==================標準モードフォルダ取得（旧「標準」フォルダの名称変換）==================
	
	//標準モードファイル（フォルダ）名 #305
	//バージョン2.1.10以前：「標準」
	//バージョン2.1.11以後：「Normal」
	//「標準」フォルダが存在して、「Normal」フォルダが存在しない場合、「標準」フォルダと中のモードファイルを「Normal」に名称変更する
	//（→これを行ってもバージョン2.1.11以降で使用する限り、従来の「標準」フォルダが標準フォルダとして扱われる。
	//また、多言語名称の日本語には必ず「標準」がセットされるので、バージョン2.1.11以降でも表示名は「標準」になる。
	//標準モード名・フォルダ取得
	AText	normalModeName("Normal");
	AFileAcc	normalModeFolder;
	normalModeFolder.SpecifyChild(rootModeFolder,normalModeName);
	//旧標準モード名・フォルダ取得
	AText	oldNormalModeName;
	oldNormalModeName.SetLocalizedText("StandardModeFileNameOld");
	AFileAcc	oldNormalModeFolder;
	oldNormalModeFolder.SpecifyChild(rootModeFolder,oldNormalModeName);
	//「標準」フォルダが存在して、「Normal」フォルダが存在しない場合、フォルダ名とモードファイル名をNormalに名称変更する
	if( oldNormalModeFolder.Exist() == true && normalModeFolder.Exist() == false  )
	{
		//モードフォルダを名称変更
		oldNormalModeFolder.Rename(normalModeName);
		//モードファイルを名称変更（バージョン3では使用しないが、バージョン2.1.11以降で起動する時に必要。）
		AFileAcc	oldNormalModeFile;
		oldNormalModeFile.SpecifyChild(oldNormalModeFolder,oldNormalModeName);
		oldNormalModeFile.Rename(normalModeName);
	}
	
	//==================アプリ内モードデフォルトフォルダにあって、こちらにないモードをコピーする==================
	//#1374 data/ModePreferences.miのみコピーする。それ以外はアプリ内を参照するようにしたので、コピー不要。
	
	//デフォルトフォルダ取得
	AFileAcc	defaultFolder;
	SPI_GetModeDefaultFolder(defaultFolder);
	
	//デフォルトフォルダの子を取得
	AObjectArray<AFileAcc>	defaultFolderArray;
	defaultFolder.GetChildren(defaultFolderArray);
	//デフォルトフォルダの子ごとのループ
	for( AIndex i = 0; i < defaultFolderArray.GetItemCount(); i++ )
	{
		//子ファイル／フォルダを取得
		AFileAcc	srcModeFolder = defaultFolderArray.GetObjectConst(i);
		//フォルダ以外は何もしないでループ継続
		if( srcModeFolder.IsFolder() == false )
		{
			continue;
		}
		//デフォルトフォルダのモードのモード名取得
		AText	srcModeName;
		srcModeFolder.GetFilename(srcModeName);
		//同じ名前のモードフォルダを取得
		AFileAcc	dstModeFolder;
		dstModeFolder.SpecifyChild(rootModeFolder,srcModeName);
		//同じ名前のモードフォルダがなければ、デフォルトフォルダのモードをコピーする
		if( dstModeFolder.Exist() == false )
		{
			//セッションプログレス更新
			SPI_CheckContinueingEditProgressModalSession(kEditProgressType_InitMode,0,true,i/2,defaultFolderArray.GetItemCount(),true);//#1374
			
			/*#1374 ModePreferences.miファイルだけコピーするように変更。
			//デフォルトフォルダのモードを同じ名前でコピーする
			srcModeFolder.CopyFolderTo(dstModeFolder,true,true);
			*/
			
			//ModePreferences.miファイルのみ、デフォルトフォルダ（アプリ内フォルダ）から、ユーザーmodeフォルダへコピーする
			AFileAcc	dstFolder;
			dstFolder.SpecifyChild(dstModeFolder,"data");
			dstFolder.CreateFolderRecursive();
			AFileAcc	srcFile, dstFile;
			srcFile.SpecifyChild(srcModeFolder,"data/ModePreferences.mi");
			dstFile.SpecifyChild(dstModeFolder,"data/ModePreferences.mi");
			srcFile.CopyFileTo(dstFile,true);
			
			//新規に生成したモード名のリストに追加
			mNewlyCreatedModeNameArray.Add(srcModeName);
		}
	}
	
	//==================標準モードのAModePrefDB作成,Load==================
	//新規生成したかどうか取得
	ABool	normalModeCreatedNewly = (mNewlyCreatedModeNameArray.Find(normalModeName)!=kIndex_Invalid);
	//モードpref dbオブジェクト生成
	AIndex	modeIndex = AddMode(normalModeFolder,normalModeCreatedNewly,false);//#427
	//モードロード
	mModePrefDBArray.GetObject(modeIndex).LoadOrWaitLoadComplete(true);
	//標準モードの表示名は常に固定名称にする。日本語名称は「標準」
	mModePrefDBArray.GetObject(modeIndex).SetNormalModeNames();
	//
	NVI_GetMenuManager().RealizeDynamicMenu(mModePrefDBArray.GetObject(modeIndex).GetToolMenuRootObjectID());
	
	//==================各モードのAModePrefDB生成==================
	//モードフォルダのリストを取得
	AObjectArray<AFileAcc>	modeFolderArray;
	rootModeFolder.GetChildren(modeFolderArray);
	
	//modeFolderArrayから、標準モード以外のAModePrefDBを作成
	for( AIndex i = 0; i < modeFolderArray.GetItemCount(); i++ )
	{
		//セッションプログレス更新
		if( mNewlyCreatedModeNameArray.GetItemCount() > 0 )
		{
			SPI_CheckContinueingEditProgressModalSession(kEditProgressType_InitMode,0,true,modeFolderArray.GetItemCount()/2+i/2,modeFolderArray.GetItemCount());
		}
		
		//ファイル／フォルダ取得
		AFileAcc	modeFolder;
		modeFolder = modeFolderArray.GetObjectConst(i);
		
		//フォルダ以外は対象外なので何もせずループ継続
		if( modeFolder.IsFolder() == false )
		{
			continue;
		}
		
		//フォルダ名取得
		AText	modeName;
		modeFolder.GetFilename(modeName);
		
		//標準モードはすでに作成済みなので何もせずループ継続
		if( modeName.Compare(normalModeName) == true )
		{
			continue;
		}
		
		//------------------モード作成------------------
		//新規生成したかどうか取得
		ABool	modeCreatedNewly = (mNewlyCreatedModeNameArray.Find(modeName)!=kIndex_Invalid);
		//モードpref dbオブジェクト生成
		AddMode(modeFolderArray.GetObjectConst(i),modeCreatedNewly,false);//#427
	}
	
	/*自動更新はとりあえず保留。まず更新ボタンクリックでの更新のみ対応する。
	//#427 AutoUpdate
	//Revisionチェック
	for( AIndex i = 0; i < mModePrefDBArray.GetItemCount(); i++ )
	{
		SPI_ModeUpdate(i,false);
	}
	*/
	
	//標準モードを生成したとき（＝最初の起動時）は、はじめにお読みくださいを表示する #1351 #1333
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
起動時バックアップ保存
*/
void	AApp::SPI_CopyToLaunchBackup()
{
	//バックアップフォルダ取得
	AFileAcc	apppreffolder;
	AFileWrapper::GetAppPrefFolder(apppreffolder);
	AFileAcc	backupfolder;
	backupfolder.SpecifyChild(apppreffolder,"_lmb");
	backupfolder.CreateFolder();
	//モードごとに実行
	for( AIndex i = 0; i < mModeFileArray.GetItemCount(); i++ )
	{
		AFileAcc	modeFile;
		modeFile.CopyFrom(mModeFileArray.GetObjectConst(i));
		AText	modename;
		modeFile.GetFilename(modename);
		
		//modeファイルをバックアップ
		AFileAcc	backupfile;
		backupfile.SpecifyChildFile(backupfolder,modename);
		//#844 modeFile.CopyFileTo(backupfile,true);
		
		AText	backupfilepath;
		backupfile.GetPath(backupfilepath);
		
		//ModePreferences.miをバックアップ
		AFileAcc	srcPrefFile;
		srcPrefFile.SpecifySister(modeFile,"data/ModePreferences.mi");
		AFileAcc	dstPrefFile;
		AText	backupfilepath_pref(backupfilepath);
		backupfilepath_pref.AddCstring("_modepref");
		dstPrefFile.Specify(backupfilepath_pref);
		srcPrefFile.CopyFileTo(dstPrefFile,true);
		/*#844
		//keywords.txtをバックアップ
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

//全てのモードのルートフォルダ取得
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

//デフォルトmodeフォルダ取得
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
モード自動更新実行
*/
void	AApp::SPI_ModeUpdate( const AModeIndex inModeIndex, const ABool inForce )
{
	//inForce(revisionにかかわらず常に更新)の値を記憶
	SPI_GetModePrefDB(inModeIndex,false).SetForceAutoUpdate(inForce);
	
	//参照URL取得
	AText	updateURL;
	SPI_GetModePrefDB(inModeIndex,false).GetData_Text(AModePrefDB::kModeUpdateURL,updateURL);
	if( updateURL.GetItemCount() > 0 )
	{
		//RevisionファイルのURLを取得
		AText	revisionFileURL;
		revisionFileURL.SetChildURL(updateURL,"update.txt");
		//非同期処理間受け渡しデータ
		ATextArray	infoTextArray;
		AText	t;
		t.SetNumber(inModeIndex);
		infoTextArray.Add(t);
		//読み込みスレッド起動
		//読み込み完了時処理：ModeUpdate_Result_Revision()
		ACurlThreadFactory	factory(this);
		AObjectID	threadObjectID = NVI_CreateThread(factory);
		(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
				SPNVI_Run(kCurlRequestType_HTTP,revisionFileURL,
					kCurlTransactionType_AutoUpdateRevision,infoTextArray);
	}
	else
	{
		//参照URL未設定
		SPI_GetModePrefWindow(inModeIndex).SPI_ShowAutoUpdateError();//#858
		return;
	}
}

/**
Revisionデータ読み込み完了時処理
*/
void	AApp::ModeUpdate_Result_Revision( const AText& inText, const ATextArray& inInfoTextArray )
{
	AIndex	modeindex = inInfoTextArray.GetTextConst(0).GetNumber();
	
	//強制更新フラグ（手動更新）読み込み・フラグ解除
	ABool	forceUpdate = SPI_GetModePrefDB(modeindex,false).GetForceAutoUpdate();
	SPI_GetModePrefDB(modeindex,false).SetForceAutoUpdate(false);
	
	//Web上のrevision取得
	ANumber64bit	webRevision = -1;
	AText	webModename;
	if( ParseUpdateTxt(inText,webModename,webRevision) == false )
	{
		webRevision = -1;
	}
	if( webRevision < 0 )
	{
		//ファイルなし、または、update.txtのフォーマット間違い
		SPI_GetModePrefWindow(modeindex).SPI_ShowAutoUpdateError();//#858
		return;
	}
	//読み込み中revision番号設定
	SPI_GetModePrefDB(modeindex,false).AutoUpdate_SetLoadingInfo(webRevision,webModename);
	
	//revision比較
	AText	revisionText;
	SPI_GetModePrefDB(modeindex,false).GetData_Text(AModePrefDB::kAutoUpdateRevisionText,revisionText);
	ANumber64bit	localRevision = revisionText.GetNumber64bit();
	if( localRevision < webRevision || forceUpdate == true )
	{
		//zipファイル名取得
		AText	zipfilename;
		zipfilename.AddText(webModename);
		zipfilename.AddNumber64bit(webRevision);
		zipfilename.AddCstring(".zip");
		
		//モード更新サーバーURL取得
		AText	updateURL;
		SPI_GetModePrefDB(modeindex,false).GetData_Text(AModePrefDB::kModeUpdateURL,updateURL);
		if( updateURL.GetItemCount() > 0 )
		{
			//zipファイルのURLを取得
			AText	zipFileURL;
			zipFileURL.SetChildURL(updateURL,zipfilename);
			
			//zip読みこみスレッド起動
			//読み込み完了時処理：ModeUpdate_Result_ZipFile()
			ACurlThreadFactory	factory(this);
			AObjectID	threadObjectID = NVI_CreateThread(factory);
			(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
					SPNVI_Run(kCurlRequestType_HTTP,zipFileURL,
						kCurlTransactionType_AutoUpdateZip,inInfoTextArray);
		}
	}
}

/**
モードZipファイル読み込み完了時処理
*/
void	AApp::ModeUpdate_Result_ZipFile( const AText& inZipText, const ATextArray& inInfoTextArray )
{
	AIndex	modeindex = inInfoTextArray.GetTextConst(0).GetNumber();
	//ローカルzipファイル生成
	AFileAcc	tempFolder;
	SPI_GetTempFolder(tempFolder);
	AFileAcc	zipFileAcc;
	zipFileAcc.SpecifyUniqChildFile(tempFolder,"modeupdatezip");
	zipFileAcc.CreateFile();
	zipFileAcc.WriteFile(inZipText);
	//zip解凍先
	AFileAcc	unzipFolder;
	unzipFolder.SpecifyUniqChildFile(tempFolder,"modeupdate");
	unzipFolder.CreateFolder();
	//解凍
	AZipFile	zipFile(zipFileAcc);
	if( zipFile.Unzip(unzipFolder) == false )
	{
		//Unzip失敗
		SPI_GetModePrefWindow(modeindex).SPI_ShowAutoUpdateError();//#858
		return;
	}
	
	//update.txtに記載されたフォルダ名のフォルダ取得
	AText	modename;
	SPI_GetModePrefDB(modeindex,false).AutoUpdate_GetLoadingModeName(modename);
	AFileAcc	modefolder;
	modefolder.SpecifyChild(unzipFolder,modename);
	//update.txtに記載されたフォルダ名のフォルダが存在しているかチェック
	if( modefolder.Exist() == false )
	{
		//フォルダ非存在
		SPI_GetModePrefWindow(modeindex).SPI_ShowAutoUpdateError();//#858
		return;
	}
	
	//AutoUpdate実行
	SPI_GetModePrefDB(modeindex,true).AutoUpdate(modefolder);
	
}

#pragma mark ===========================

#pragma mark --- Webからモード追加
//#427

/**
Webからモード追加
*/
void	AApp::SPI_AddNewModeFromWeb( const AText& inModeName, const AText& inURL )
{
	//RevisionファイルのURL取得
	AText	revisionURL;
	revisionURL.SetChildURL(inURL,"update.txt");
	//非同期処理間受け渡しデータ
	//0:モード名（ユーザー指定モード名）
	//1:読み込み元URL
	ATextArray	infoTextArray;
	infoTextArray.Add(inModeName);
	infoTextArray.Add(inURL);
	//読み込みスレッド起動
	//完了時処理：AddNewModeFromWeb_Result_Revision()
	ACurlThreadFactory	factory(this);
	AObjectID	threadObjectID = NVI_CreateThread(factory);
	(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
			SPNVI_Run(kCurlRequestType_HTTP,revisionURL,
				kCurlTransactionType_AddModeFromWeb_Revision,infoTextArray);
}

/**
Webからモード追加・Revisionファイル読み込み完了時処理
*/
void	AApp::AddNewModeFromWeb_Result_Revision( const AText& inText, const ATextArray& inInfoTextArray )
{
	//Web上のrevision取得
	ANumber64bit	webRevision = -1;
	AText	webModename;
	if( ParseUpdateTxt(inText,webModename,webRevision) == false )
	{
		webRevision = -1;
	}
	if( webRevision < 0 )
	{
		//ファイルなし、または、update.txtのフォーマット間違い
		return;
	}
	
	//zipファイル名取得
	AText	zipfilename;
	zipfilename.AddText(webModename);
	zipfilename.AddNumber64bit(webRevision);
	zipfilename.AddCstring(".zip");
	//zipファイルのURLを取得
	AText	zipFileURL;
	zipFileURL.SetChildURL(inInfoTextArray.GetTextConst(1),zipfilename);
	//非同期処理間受け渡しデータ（モード名追加）
	//0:モード名（ユーザー指定モード名）
	//1:読み込み元URL
	//2:モード名（update.txtに記述されたモード名）
	ATextArray	infoTextArray;
	infoTextArray.SetFromTextArray(inInfoTextArray);
	infoTextArray.Add(webModename);
	//zip読みこみスレッド開始
	ACurlThreadFactory	factory(this);
	AObjectID	threadObjectID = NVI_CreateThread(factory);
	(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
			SPNVI_Run(kCurlRequestType_HTTP,zipFileURL,
				kCurlTransactionType_AddModeFromWeb_Zip,infoTextArray);
}

/**
Webからモード追加・zipファイル読み込み完了時処理
@param inInfoTextArray 非同期処理間受け渡しデータ
0:モード名（ユーザー指定モード名）
1:読み込み元URL
2:モード名（update.txtに記述されたモード名）
*/
void	AApp::AddNewModeFromWeb_Result_ZipFile( const AText& inZipText, const ATextArray& inInfoTextArray )
{
	//ローカルzipファイル生成
	AFileAcc	tempFolder;
	SPI_GetTempFolder(tempFolder);
	AFileAcc	zipFileAcc;
	zipFileAcc.SpecifyUniqChildFile(tempFolder,"modeupdatezip");
	zipFileAcc.CreateFile();
	zipFileAcc.WriteFile(inZipText);
	//zip解凍先
	AFileAcc	unzipFolder;
	unzipFolder.SpecifyUniqChildFile(tempFolder,"modeupdate");
	unzipFolder.CreateFolder();
	//解凍
	AZipFile	zipFile(zipFileAcc);
	if( zipFile.Unzip(unzipFolder) == false )
	{
		//Unzip失敗
		return;
	}
	
	//update.txtに記載されたフォルダ名のフォルダ取得
	AText	modename;
	modename.SetText(inInfoTextArray.GetTextConst(2));
	AFileAcc	modefolder;
	modefolder.SpecifyChild(unzipFolder,modename);
	//update.txtに記載されたフォルダ名のフォルダが存在しているかチェック
	if( modefolder.Exist() == false )
	{
		//フォルダ非存在
		return;
	}
	
	//展開したmodefolderからモードインポート
	AModeIndex	modeIndex = SPI_AddNewModeImportFromFolder(inInfoTextArray.GetTextConst(0),modefolder,false,true);
	if( modeIndex != kIndex_Invalid )
	{
		//#844 SPI_GetModePrefWindow(modeIndex).SPI_SelectMode(modeIndex);
		SPI_ShowModePrefWindow(modeIndex);
	}
}

/**
update.txt解析
*/
ABool	AApp::ParseUpdateTxt( const AText& inText, AText& outModeName, ANumber64bit& outRevision )
{
	AIndex	pos = 0;
	AText	token;
	AText	line;
	//モード名（フォルダ名）読み込み
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
	//Revision読み込み
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
モード自動更新ダウンロード元URL（プリセットURL）取得
*/
void	AApp::SPI_GetModeUpdateURL( const AIndex inIndex, AText& outURL ) const
{
	mModeUpdateURLArray.Get(inIndex,outURL);
}

#pragma mark ===========================

#pragma mark ---マルチファイル検索ファイルフィルタープリセット

//#617
/**
ファイルフィルタープリセットを取得
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
		/*#402 名前を読むためだけにParseするのはもったいないので、名前読み込み専用の処理に変更
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

#pragma mark ---正規表現サンプルリスト

/**
正規表現サンプルリスト項目を取得
*/
void	AApp::SPI_GetRegExpListItem( const AIndex inIndex, AText& outItem ) const
{
	mRegExpListArray.Get(inIndex,outItem);
}

#pragma mark ===========================

#pragma mark ---Transliterate初期化

void	AApp::InitTransliterate()
{
	/*B0408 AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	AFileAcc	transliterateFolder;
	transliterateFolder.SpecifyChild(appPrefFolder,"Transliterate");
	//Prefフォルダの下にTransliterateフォルダがなければ、フォルダを作成し、アプリフォルダのTransliterateの中身をコピーする
	if( transliterateFolder.Exist() == false )
	{
		AFileAcc	appFile;
		AFileWrapper::GetAppFile(appFile);
		AFileAcc	defaultTransliterateFolder;
		defaultTransliterateFolder.SpecifySister(appFile,"Transliterate");
		defaultTransliterateFolder.CopyFolderTo(transliterateFolder);
	}*/
	/*B0408 2.1.6との違いまとめ（2.1.8b4時点）
	半角カナ→全角カナ：
	2.1.6: ～も対象
	2.1.8: ～は対象外（~は本来チルダマークなので対象外とするほうがベター）
	全角カナ→半角カナ：
	2.1.6: 音引きは条件によって対象外、濁音・半濁音も条件によって対象外
	2.1.8: 音引き・濁音・半濁音も常に対象（2.1.8のTransliterateシステムで2.1.6のように条件によって対応するのは工数大）
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
		//対象外ファイル
		if( filename.GetLength() == 0 )   continue;
		if( filename.Get(0) == '.' )   continue;
		if( filename.Compare("order") == true )   continue;
		if( filename.Compare("Icon\r") == true )   continue;
		//
		AText	text;
		file.ReadTo(text);//win SPI_LoadTextFromFile()を使うとWindowsでSJISと誤認識されるためReadTo()を使う
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
				//B0000 svnサーバーに置くために、ファイル名をアスキー文字にする。
				//そのかわり、ファイルの最終行にタイトルを記述
				title.SetText(srcline);
				break;
			}
			AText	dstline;
			text.GetLine(pos,dstline);
			mTransliterate_Source.GetObject(index).Add(srcline);
			mTransliterate_Destination.GetObject(index).Add(dstline);
		}
		//B0000 svnサーバーに置くために、ファイル名をアスキー文字にする。mTransliterate_Title.Add(filename);
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

#pragma mark ---FuzzySearchデータ

//#166
/**
曖昧検索初期化
*/
void	AApp::InitFuzzySearch()
{
	AFileAcc	fuzzySearchFolder;
	AFileWrapper::GetResourceFile("FuzzySearch",fuzzySearchFolder);
	if( fuzzySearchFolder.Exist() == false )   {_ACError("",this);return;}
	
	//かなゆらぎ
	AFileAcc	kanaYuragiFile;
	kanaYuragiFile.SpecifyChild(fuzzySearchFolder,"KanaYuragi.txt");
	if( kanaYuragiFile.Exist() == false )   {_ACError("",this);return;}
	AText	text;
	kanaYuragiFile.ReadTo(text);
	for( AIndex pos = 0; pos < text.GetItemCount(); )
	{
		//正規化元
		AText	srcline;
		text.GetLine(pos,srcline);
		AUCS4Char	uc = 0;
		srcline.Convert1CharToUCS4(0,uc);
		mFuzzySearchNormalizeArray_from.Add(uc);
		//正規化先
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

#pragma mark ---Toolbarアイコン

void	AApp::LoadToolbarIcon()
{
#if IMPLEMENTATION_FOR_MACOSX
	//アプリケーションリソースのアイコン
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
			//icnsファイル
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
			//B0307 フォルダのカスタムアイコン
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
//フォルダのカスタムアイコンから登録
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
			if( file.IsLink() == false )//#0 シンボリックリンク・エイリアスによる無限ループ防止
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
	//B0307まず関連リソースを全て削除（アイコン変更時はこれ必要）
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
		//cicn#128に追加
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
		//B0307 カスタムアイコンフラグ解除
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
		//フォルダーカスタムアイコンからのコピー
		AFileAcc	file;
		file.Specify(path);
		AText	buffer;
		file.ReadTo(buffer,true);//RF
		inDestFile.WriteResourceFork(buffer);
		//カスタムアイコンフラグ設定
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

#pragma mark <イベント処理>

#pragma mark ===========================

//内部イベント処理
void	AApp::EVTDO_DoInternalEvent( ABool& outUpdateMenu )
{
	//#698
	outUpdateMenu = false;
	
	//モーダル表示中は内部イベント処理しない。（モーダル中は他のイベントが割り込むことが考慮されていないので。）(#946)
	if( AWindow::NVI_IsInModal() == true )
	{
		return;
	}
	
	//編集プログレスモーダルセッション中だったら、内部イベント処理しない（理由？）
	if( SPI_InEditProgressModalSession() == true )
	{
		return;
	}
	
	//fprintf(stderr,"EVT_DoInternalEvent() ");
	//ここに来た時点でキューにたまっているイベント数だけを処理する。
	//これにより、（内部イベントが存在している限りループさせる場合と違って）
	//マウスイベントや表示更新イベントをOSイベントキューにためたままEVT_DoInternalEvent()内でぐるぐる回り続けることはなく、
	//queueCount分が終了したら、OSへ処理を返すことができる。
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
					//マルチファイル検索プログレス
				  case kInternalEvent_MultiFileFindProgress_SetProgress:
					{
						if( SPI_GetMultiFileFindWindow().SPI_ExecutingFindForMultiFileReplace() == true )
						{
							//マルチファイル置換のプログレス
							SPI_GetMultiFileFindWindow().SPI_MultiFileReplace_Progress(static_cast<short>(num));
						}
						else
						{
							//通常マルチファイル検索のプログレス
							SPI_GetMultiFileFindProgress().NVI_Create(kObjectID_Invalid);
							SPI_GetMultiFileFindProgress().SPI_SetProgress(static_cast<short>(num));
						}
						break;
					}
				  case kInternalEvent_MultiFileFindProgress_SetProgressText:
					{
						if( SPI_GetMultiFileFindWindow().SPI_ExecutingFindForMultiFileReplace() == true )
						{
							//マルチファイル置換のプログレス
							SPI_GetMultiFileFindWindow().SPI_MultiFileReplace_SetProgressText(text);
						}
						else
						{
							//通常マルチファイル検索のプログレス
							SPI_GetMultiFileFindProgress().NVI_Create(kObjectID_Invalid);
							SPI_GetMultiFileFindProgress().SPI_SetProgressText(text);
						}
						break;
					}
					//マルチファイル検索完了通知
				  case kInternalEvent_MultiFileFind_Completed:
					{
						//
						GetApp().SPI_GetIndexWindowDocumentByID(objectIDArray.Get(0)).SPI_InhibitClose(false);
						//マルチファイル検索ウインドウの検索ボタンをEnableにするため
						SPI_UpdateMultiFileFindWindow();
						//ImportFileRecognizer再開
						GetApp().SPI_GetImportFileRecognizer().NVI_SetSleepFlag(false);
						GetApp().SPI_GetImportFileRecognizer().NVI_WakeTrigger();
						//
						if( SPI_GetMultiFileFindWindow().SPI_ExecutingFindForMultiFileReplace() == true )
						{
							//マルチファイル置換の場合
							//マルチファイル検索ウインドウの完了処理
							SPI_GetMultiFileFindWindow().SPI_MultiFileReplace_FindCompleted();
						}
						else
						{
							//マルチファイル検索の場合
							//プログレスウインドウ閉じる
							SPI_GetMultiFileFindProgress().NVI_Hide();
						}
						//------------------一致箇所を新規ドキュメントに抽出------------------
						if( text.GetItemCount() > 0 )
						{
							ADocumentID docID = SPNVI_CreateNewTextDocument(kStandardModeIndex);
							if( docID != kObjectID_Invalid )
							{
								SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
							}
						}
						//------------------マルチファイル検索ウインドウ表示更新------------------
						SPI_GetMultiFileFindWindow().NVI_UpdateProperty();
						//
						//#698
						outUpdateMenu = true;
						break;
					}
					//マルチファイル検索結果
				  case kInternalEvent_MultiFileFindResult:
					{
						AObjectID	receiverID = objectIDArray.Get(0);
						AMultiFileFindResult&	result = SPI_GetThreadDataReceiver_MultiFileFindResult(receiverID);
						//マルチファイル置換の場合、マルチファイル検索ウインドウに通知する
						//（これにより置換対象ファイルリストの更新を行う。置換ウインドウ内の結果リストはこの下の処理で更新する。）
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
						//#1275 高速化のためウインドウ表示操作するまでは生成しない /*#199 mTraceLogWindow*/SPI_GetTraceLogWindow().NVI_Create(kObjectID_Invalid);
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
				//FTPフォルダ読み込み完了
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
					//インポートファイル解析完了
				  case kInternalEvent_ImportFileRecognizeCompleted:
					{
						//------------------インポートファイル解析完了処理------------------
						//インポートファイルmanagerでのインポートファイル解析完了処理
						//（documentやmode prefでの完了処理を行う）
						mImportFileManager.DoImportFileRecognized(true);
						//------------------インポートファイル解析中プログレス表示消去------------------
						//テキストウインドウのプログレス表示を消去
						for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
						{
							SPI_GetTextWindowByID(winID).SPI_HideImportFileProgress();
						}
						//コールグラフのプログレス表示を消去
						SPI_ShowHideImportFileProgressInCallGrafWindow(false);
						//ワードリストのプログレス表示を消去
						SPI_ShowHideImportFileProgressInWordsListWindow(false);
						break;
					}
					/*
				//インポートファイル解析一部完了
			  case kInternalEvent_ImportFileRecognizePartialCompleted:
				{
					mImportFileManager.DoImportFileRecognized(false);
					break;
				}
				*/
					//#725
					//インポートファイルプログレス表示
				  case kInternalEvent_ImportFileRecognizeProgress:
					{
						//最前面のテキストウインドウのインポートファイルプログレス表示更新
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
						//コールグラフウインドウのインポートファイルプログレス表示
						SPI_ShowHideImportFileProgressInCallGrafWindow(true);
						//ワードリストウインドウのインポートファイルプログレス表示
						SPI_ShowHideImportFileProgressInWordsListWindow(true);
						break;
					}
					//
				  case kInternalEvent_ChildProcessCompleted:
					{
						mChildProcessManager.DeletePipe(objectIDArray.Get(0),text);
						break;
					}
					//#402 プロジェクトフォルダロード完了
				  case kInternalEvent_ProjectFolderLoaderResult:
					{
						//要求データ取得
						AObjectID	threadObjectID = objectIDArray.Get(0);
						ATextArray	requestArray_ProjectFolder;
						(dynamic_cast<AProjectFolderLoader&>(NVI_GetThreadByID(threadObjectID))).
								SPI_GetRequestedData(requestArray_ProjectFolder);
						
						//mSameProject_ProjectFolder検索用HashTextArray生成
						//（mSameProject_ProjectFolderをfolderPathArrayForSearchにコピー）
						AHashTextArray	folderPathArrayForSearch;
						for( AIndex i = 0; i < mSameProject_ProjectFolder.GetItemCount(); i++ )
						{
							AText	path;
							mSameProject_ProjectFolder.GetObjectConst(i).GetPath(path);
							folderPathArrayForSearch.Add(path);
						}
						
						//結果取得
						//一度に複数のプロジェクトについての処理を行う場合もあるので、各プロジェクト毎のループ
						for( AIndex requestArrayIndex = 0; requestArrayIndex < requestArray_ProjectFolder.GetItemCount(); requestArrayIndex++ )
						{
							//プロジェクトindex取得
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
								//結果取得
								ATextArray	resultTextArray;
								(dynamic_cast<AProjectFolderLoader&>(NVI_GetThreadByID(threadObjectID))).
										SPI_GetResult(requestArrayIndex,resultTextArray);
								//
								for( AIndex i = 0; i < resultTextArray.GetItemCount(); i++ )
								{
									//ファイル取得
									AText	path;
									resultTextArray.Get(i,path);
									AFileAcc	file;
									file.Specify(path);
									//ファイルパスデータ追加
									mSameProjectArray_Pathname.GetObject(sameProjectArrayIndex).Add(path);
									//（フォルダ以外の場合）モードindexを取得、データ追加（フォルダの場合はモードindexをkIndex_Invalidとして追加）
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
						//メニュー更新
						UpdateSameProjectMenu();
						
						//読み込んだフォルダリスト取得
						AObjectArray<AFileAcc>	folders;
						(dynamic_cast<AProjectFolderLoader&>(NVI_GetThreadByID(threadObjectID))).
								SPI_GetLoadedFolders(folders);
						//SCM登録
						if( GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
						{
							mSCMFolderManager.RegisterFolders(folders,0);
						}
						
						//ファイルリスト表示更新（SCM状態読み込み終了後も呼ばれるが、SCMは必ずしも実行しない）
						//#725 SPI_GetFileListWindow().SPI_UpdateTable();
						SPI_UpdateFileListWindows(kFileListUpdateType_ProjectFolderLoaded);//#725
						
						//スレッドオブジェクト削除
						NVI_DeleteThread(threadObjectID);
						break;
					}
					//#427
				  case kInternalEvent_Curl:
					{
						//要求データ取得
						AObjectID	threadObjectID = objectIDArray.Get(0);
						
						//読み込みデータ取得
						AText	resultText;
						AText	url;
						ECurlRequestType	requestType = kCurlRequestType_None;
						ATextArray	infoTextArray;
						ACurlTransactionType	transactionType = kCurlTransactionType_None;
						(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
								GetResult(resultText,requestType,url,transactionType,infoTextArray);
						
						//Transactionごとの処理切り替え
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
							//キーワード情報用CSVファイルHttp経由読み込み
						  case kCurlTransactionType_LoadCSVForKeyword:
							{
								LoadCSVForKeyword_Result(resultText,infoTextArray);
								break;
							}
						}
						
						//スレッドオブジェクト削除
						NVI_DeleteThread(threadObjectID);
						break;
					}
					//#682 リオープン
				  case kInternalEvent_ReopenFile:
					{
						if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kReopenFile) == true )
						{
							ReopenFile();
						}
						//#975 リオープン設定がOFFの場合、ここでmReopenProcessCompletedをtrueに設定して、新規ファイル生成処理実行。
						else
						{
							mReopenProcessCompleted = true;
							//自動新規ファイル生成
							//（起動時にEVT_OpenUntitledFile()経由でSPI_CreateNewWindowAutomatic()がコールされるが、
							//　mReopenProcessCompletedフラグがfalseの間は実行せず、prependフラグをtrueにするのみとしている。
							//　prependされた新規ファイル自動生成処理をここで実行する。 #1056）
							DoPrependedOpenUntitledFileEvent();
						}
						//#698
						outUpdateMenu = true;
						break;
					}
					//#678 Document Reveal（リオープン時に使用している）
				  case kInternalEvent_RevealDocument:
					{
						ADocumentID	docID = objectIDArray.Get(0);
						if( NVI_IsDocumentValid(docID) == true )
						{
							SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();
						}
						break;
					}
					//#698 文法認識スレッド処理一時完了
				  case kInternalEvent_SyntaxRecognizerPaused:
					{
						ADocumentID	docID = objectIDArray.Get(0);
						if( NVI_IsDocumentValid(docID) == true )
						{
							SPI_GetTextDocumentByID(docID).SPI_DoSyntaxRecognizerPaused();
						}
						break;
					}
					//#699 行計算スレッド処理一時完了
				  case kInternalEvent_WrapCalculatorPaused:
					{
						ADocumentID	docID = objectIDArray.Get(0);
						if( NVI_IsDocumentValid(docID) == true )
						{
							if( SPI_GetTextDocumentByID(docID).SPI_DoWrapCalculatorPaused() == true )
							{
								//折り返し計算完了時はメニュー更新（書き込み可に変わるので）
								outUpdateMenu = true;
							}
						}
						break;
					}
					//#733 
					//補完候補検索結果
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
					//ワードリスト検索結果
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
				//アクセスプラグインデータ受信
			  case kInternalEvent_RemoteConnectionResponseReceived:
				{
					AObjectID	accessPluginConnectionObjectID = objectIDArray.Get(0);
					DoAccessPluginResponseReceived(accessPluginConnectionObjectID,text);
					break;
				}
				*/
					//#853
					//キーワード情報検索結果
				  case kInternalEvent_IdInfoFinderPaused:
					{
						ADocumentID	docID = objectIDArray.Get(0);
						AWindowID	idInfoWinID = objectIDArray.Get(1);
						AWindowID	textWinID = objectIDArray.Get(2);
						DoIdInfoFinderPaused(docID,idInfoWinID,textWinID);
						break;
					}
					//startup 処理
				  case kInternalEvent_StartUp:
					{
						//モードをdefaultから新規生成したときはダイアログを表示する
						if( mNewlyCreatedModeNameArray.GetItemCount() > 0 )
						{
							//標準モードを新規生成したとき（＝新規インストール時）はダイアログ表示しない（これが最初に表示されても初見では意味がわからない） #1374
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
									//モードの表示名を取得
									AIndex	modeIndex = SPI_FindModeIndexByModeRawName(modeName);
									AText	modeDisplayName;
									SPI_GetModePrefDB(modeIndex,false).GetModeDisplayName(modeDisplayName);
									//モード表示名をメッセージに追加
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
					//保存時ダイアログ　強制UTF8変換した場合のダイアログ
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
					//保存時ダイアログ　 charsetエラーダイアログ
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
					//CSVキーワードローダー完了時処理
				  case kInternalEvent_CSVKeywordLoaderCompleted:
					{
						//スレッドオブジェクト削除
						NVI_DeleteThread(senderObjectID);
						break;
					}
					//同じフォルダローダー完了時処理
				  case kInternalEvent_SameFolderLoaderCompleted:
					{
						AFileAcc	folder;
						folder.Specify(text);
						//現在の同じフォルダメニューのフォルダなら「同じフォルダ」メニュー更新
						if( mCurrentSameFolder == FindSameFolderIndex(folder) )
						{
							//「同じフォルダ」メニュー更新
							UpdateSameFolderMenu();
							//ファイルリスト表示更新（SCM状態読み込み終了後も呼ばれるが、SCMは必ずしも実行しないため。）
							SPI_UpdateFileListWindows(kFileListUpdateType_SameFolderDataUpdated);//#725
						}
						//SCM状態読み込み
						SCMRegisterFolder(folder);//SCM状態読み込み終了後、InternalEvent経由でファイルリストのSPI_UpdateTable()等が実行される
						//スレッドオブジェクト削除
						NVI_DeleteThread(senderObjectID);
						break;
					}
					//ドキュメントファイルのスクリーニング完了時処理
				  case kInternalEvent_DocumentFileScreenCompleted:
					{
						//ドキュメント取得
						ADocumentID	docID = objectIDArray.Get(0);
						//状態遷移実行
						if( NVI_IsDocumentValid(docID) == true )
						{
							if( SPI_GetTextDocumentByID(docID).SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
							{
								//FileScreened状態のままであればロード／折り返し計算／状態繊維を行う。
								//（完了通知の前に、ビューactiveになったときは、そちら側からSPI_TransitInitState_ViewActivated()がコールされる。）
								SPI_GetTextDocumentByID(docID).SPI_TransitInitState_ViewActivated();
							}
						}
						break;
					}
					//#962
					//ドキュメントファイルscreenエラー時処理（エラーダイアログ表示し、ドキュメントを閉じる）
				  case kInternalEvent_DocumentFileScreenError:
					{
						//ドキュメント取得
						ADocumentID	docID = objectIDArray.Get(0);
						//状態遷移実行
						if( NVI_IsDocumentValid(docID) == true )
						{
							SPI_GetTextDocumentByID(docID).SPI_ShowScreenErrorAndClose();
						}
						break;
					}
					//#974
					//アプリケーションactivated時の処理（のうち、他のイベント実行後に実行すべき処理）実行
				  case kInternalEvent_AppActivated:
					{
						//新規ウインドウ生成
						//#1056 EVT_OpenUntitledFile()経由で実行するように変更 SPI_CreateNewWindowAutomatic();
						break;
					}
					//#1006
					//ダブルクリックによるファイルオープン時、TextDocumentのinitWithContentsOfURL経由でこのイベントが発生する
				  case kInternalEvent_AppleEventOpenDoc:
					{
						//データ取得（NULL文字区切り）
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
						//ファイル取得
						AFileAcc	file;
						file.Specify(filepath);
						//CDocImpオブジェクト取得 #1078
						AObjectID	docImpID = objectIDArray.Get(0);
						//ドキュメント生成
						SPNVI_CreateDocumentFromLocalFile(file,tecname,docImpID);//#1078
						ADocumentID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
						if( odbMode == true )
						{
							SPI_GetTextDocumentByID(docID).SPI_SetODBMode(odbServer.GetOSTypeFromText(),odbSenderToken,odbCustomPath);
						}
						break;
					}
					//#1422
					//File Presenterファイル変更通知受信時
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
						//処理なし
						break;
					}
				}
			}
			catch(...)
			{
			}
		}
	}
	
	//このモジュールについての旧コメント
	//内部イベントをキューが空になるまで実行する。
	//その間、内部イベントキューのmutexはロックする。
	//これにより、イベントキューへPostするスレッドは、その時点でブロックされる。
	//これは、メインスレッドの処理（主に表示反映処理のはず）を優先して処理したいため。
	//内部イベント１つごとにmutexロック解除すると、その時点でスレッドが動き出す可能性が高く、内部イベントがたまる一方＝スレッドが完了するまで表示されない。
	//→やめた　Mutexをこのような目的に使うべきではない。（長い期間ロックさせるべきではない。mutexのダブルロックで、アプリがロックすることがある。）
	//本モジュールのコメントにある通り、「キューにたまっているイベント数だけを処理する」方法に変更。
	
}

//コマンド処理
ABool	AApp::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//開く
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
		//「新規」メニュー
	  case kMenuItemID_NewTextDocument:
		{
			AModeIndex modeIndex = SPI_FindModeIndexByModeRawName(inDynamicMenuActionText);
			if( modeIndex == kIndex_Invalid )   {_ACError("",this); break;}
			SPNVI_CreateNewTextDocument(modeIndex);
			result = true;
			break;
		}
		//終了
	  case kMenuItemID_Quit:
		{
			NVI_TryQuit();//#1034 #1102
			result = true;
			break;
		}
		//このアプリケーションを隠す
	  case kMenuItemID_HideThisApplication:
		{
			GetImp().HideThisApplication();
			result = true;
			break;
		}
		//ほかのアプリケーションを隠す
	  case kMenuItemID_HideOtherApplications:
		{
			GetImp().HideOtherApplications();
			result = true;
			break;
		}
		//すべてのアプリケーションを表示
	  case kMenuItemID_ShowAllApplications:
		{
			GetImp().ShowAllApplications();
			result = true;
			break;
		}
		//全てを前面に
	  case kMenuItemID_BringAllToFront:
		{
			GetImp().BringAllToFront();
			result = true;
			break;
		}
		//環境設定ウインドウ
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
		//ユーザーID登録 #1384
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
		//Sparkleアップデートを確認
	  case kMenuItemID_CheckForUpdates:
		{
#ifndef Target_Mac_OS_X_10_6
#ifdef __LP64__
			//アップデート確認
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
			{
				[[SUUpdater sharedUpdater] checkForUpdates:nil];
			}
#endif
#endif
			break;
		}
		//ウインドウメニュー
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
		//テンプレートメニュー
	  case kMenuItemID_NewWithTemplate:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			SPNVI_CreateDocumentFromTemplateFile(file);
			result = true;
			break;
		}
		//最近開いたファイル
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
				file.Specify(inDynamicMenuActionText);//#910 ,kFilePathType_1);//B0389 OK 最近開いたファイルのデータは旧形式にする
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
		//同じフォルダー
	  case kMenuItemID_SameFolder:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			//B0421 SPNVI_CreateDocumentFromLocalFile(file);
			GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
			result = true;
			break;
		}
		//同じプロジェクト
	  case kMenuItemID_SameProject:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			//B0421 SPNVI_CreateDocumentFromLocalFile(file);
			GetAppPref().LaunchAppWithFile(file,inModifierKeys);//B0421
			result = true;
			break;
		}
		//プロジェクトフォルダ構成更新 #331
	  case kMenuItemID_RefreshProjectFolders:
		{
			SPI_UpdateAllSameProjectData();
			break;
		}
		//関連ファイルを開く
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
		//FTPフォルダーメニュー
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
		//フォルダーリストメニュー
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
		//検索
	  case kMenuItemID_Find:
	  case kMenuItemID_FindDialog:
		{
			/*#199 mFindWindow*/SPI_GetFindWindow().NVI_CreateAndShow();//#701
			/*#199 mFindWindow*/SPI_GetFindWindow().NVI_SwitchFocusToFirst();
			result = true;
			break;
		}
		//マルチファイル検索
	  case kMenuItemID_MutiFileFind:
		{
			SPI_GetMultiFileFindWindow().SPI_OpenCloseReplaceScreen(false);//#65
			/*#199 mMultiFileFindWindow*/SPI_GetMultiFileFindWindow().NVI_Show();
			/*#199 mMultiFileFindWindow*/SPI_GetMultiFileFindWindow().NVI_SwitchFocusToFirst();
			result = true;
			break;
		}
		//#65
		//マルチファイル置換
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
				//タブウインドウモードをOFFにする
				AWindowID	tabWindowID = SPI_GetTabModeMainTextWindowID();
				if( tabWindowID != kObjectID_Invalid )
				{
					//#675 タブモード切替中のUpdateViewBounds抑制
					{
						AStSuppressTextWindowUpdateViewBounds	s(tabWindowID);
						//タブが1つになるまで、新規ウインドウ生成し、タブ内容をコピーし、タブを閉じる
						//#668 rect未使用のため削除 ARect	tabWindowRect;
						//#668 rect未使用のため削除 SPI_GetTextWindowByID(tabWindowID).NVI_GetWindowBounds(tabWindowRect);
						//#668 rect未使用のため削除 ARect	rect = tabWindowRect;
						AWindowID	prevWinID = tabWindowID;
						while( SPI_GetTextWindowByID(tabWindowID).NVI_GetSelectableTabCount() > 1 )//#379
						{
							AWindowDefaultFactory<AWindow_Text>	factory;//#175(this); #199
							AObjectID	winID = NVI_CreateWindow(factory);
							AIndex	mostBackTabIndex = SPI_GetTextWindowByID(tabWindowID).NVI_GetMostBackTabIndex();
							SPI_GetTextWindowByID(tabWindowID).SPI_CopyTabToWindow(mostBackTabIndex,winID);
							SPI_GetTextWindowByID(tabWindowID).SPI_CloseTab(mostBackTabIndex);//DiffTargetも同時に閉じられる
							//#668 rect未使用のため削除 rect.left -= 16;
							//SPI_GetTextWindowByID(winID).NVI_SetWindowBounds(rect);
							//SPI_GetTextWindowByID(winID).NVI_SendBehind(prevWinID);
							SPI_GetTextWindowByID(winID).SPI_UpdateViewBounds();//#291
							prevWinID = winID;
						}
						//#850 SPI_GetTextWindowByID(tabWindowID).SPI_SetTabModeMainWindow(false);
						SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();
					}
					//#675 UpdateViewBounds実行
					SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();
				}
			}
			else
			{
				//タブウインドウモードをONにする
				//ウインドウの全てのタブをメインウインドウへコピーして、タブを全て閉じる（＝ウインドウを閉じる）
				AWindowID	tabWindowID = NVI_GetFirstWindowID(kWindowType_Text);
				if( tabWindowID != kObjectID_Invalid )
				{
					//#850 SPI_GetTextWindowByID(tabWindowID).SPI_SetTabModeMainWindow(true);
					SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();
					//#675 タブモード切替中のUpdateViewBounds抑制
					{
						AStSuppressTextWindowUpdateViewBounds	s(tabWindowID);
						//
						for( AWindowID winID = NVI_GetNextWindowID(tabWindowID); winID != kObjectID_Invalid;  )
						{
							//#379 AItemCount	tabCount = SPI_GetTextWindowByID(winID).NVI_GetTabCount();
							AWindowID	nextWindowID = NVI_GetNextWindowID(winID);
							//#379 for( AIndex tabIndex = 0; tabIndex < tabCount; tabIndex++ )
							while( SPI_GetTextWindowByID(winID).NVI_GetSelectableTabCount() > 0 ) //#379 #0 NVI_GetTabCount()から修正
							{
								AIndex	mostBackTabIndex = SPI_GetTextWindowByID(winID).NVI_GetMostBackTabIndex();//#379 #0 tabWindowIDから修正
								SPI_GetTextWindowByID(tabWindowID).SPNVI_CopyCreateTab(winID, mostBackTabIndex);//#379 tabIndex);
								//B0000 この処理はSPNVI_CopyCreateTab()へ移動 SPI_GetTextWindowByID(winID).SPI_CloseTab(tabIndex);
								if( mWindowArray.ExistObject(winID) == false )   break;//#379 タブクローズの結果ウインドウがなくなっていたらbreak
							}
							winID = nextWindowID;
						}
					}
					//
					SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();//#291
					SPI_GetTextWindowByID(tabWindowID).SPI_RefreshWindowAll();//#675 ジャンプリストも含めてrefresh
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
					//ウインドウの全てのタブをメインウインドウへコピーして、タブを全て閉じる（＝ウインドウを閉じる）
					AItemCount	tabCount = SPI_GetTextWindowByID(winID).NVI_GetTabCount();
					for( AIndex tabIndex = 0; tabIndex < tabCount; tabIndex++ )
					{
						SPI_GetTextWindowByID(tabWindowID).SPNVI_CopyCreateTab(winID,tabIndex);
						//B0000 この処理はSPNVI_CopyCreateTab()へ移動 SPI_GetTextWindowByID(winID).SPI_CloseTab(tabIndex);
					}
				}
				winID = nextWindowID;
			}
			SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();//#291
			break;
		}
		*/
		//１つのウインドウにまとめる
	  case kMenuItemID_CollectAllWindowsToTabs:
		{
			//最もタブ数の多いテキストウインドウを取得
			AWindowID	tabWindowID = SPI_GetMostNumberTabsTextWindowID();
			if( tabWindowID == kObjectID_Invalid )   break;
			
			for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid;  )
			{
				//次のウインドウを取得
				AWindowID	nextWindowID = NVI_GetNextWindowID(winID);
				//このウインドウが「最もタブ数の多いテキストウインドウ」では場合、
				//最もタブ数の多いテキストウインドウへ、全てのタブを移動
				if( winID != tabWindowID )
				{
					SPI_GetTextWindowByID(winID).SPI_WindowToTab();
				}
				//次のウインドウへ
				winID = nextWindowID;
			}
			break;
		}
		//インデックスウインドウ・次
	  case kMenuItemID_IndexNext:
		{
			//#725 SPI_GoNextIndexWindowItem();
			SPI_SelectNextItemInSubWindow();
			break;
		}
		//インデックスウインドウ・前
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
			//#551 デフォルトフォルダ対応
			//ShowChooseFolderWindow()にデフォルトフォルダ引数対応したらコメント解除（デフォルトフォルダの方法はDoNavChooseFileCB()を参照）
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
		//ファイルリストのコンテキストメニュー
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
			//docpath+改行+idtextの形でactiontextに入っているので取り出す
			AText	docpath, idtext;
			AIndex	pos = 0;
			for( ; pos < inDynamicMenuActionText.GetItemCount(); pos++ )
			{
				if( inDynamicMenuActionText.Get(pos) == kUChar_LineEnd )   break;
			}
			inDynamicMenuActionText.GetText(0,pos,docpath);
			inDynamicMenuActionText.GetText(pos+1,inDynamicMenuActionText.GetItemCount()-(pos+1),idtext);
			//ドキュメントを見つける
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
			//★ #725 SPI_ShowHideIdInfoWindow(!GetAppPref().GetData_Bool(AAppPrefDB::kDisplayIdInfoWindow));//#291
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
		//見出しリストを検索
	  case kMenuItemID_SearchInHeaderList:
		{
			AWindowID	winID = NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				SPI_GetTextWindowByID(winID).SPI_SearchInHeaderList();
			}
			break;
		}
		//プロジェクトフォルダを検索
	  case kMenuItemID_SearchInProjectFolder:
		{
			SPI_SearchInProjectFolder();
			break;
		}
		//同じフォルダを検索
	  case kMenuItemID_SearchInSameFolder:
		{
			SPI_SearchInSameFolder();
			break;
		}
		//最近開いたファイルを検索
	  case kMenuItemID_SearchInRecentlyOpenFiles:
		{
			SPI_SearchInRecentlyOpenFiles();
			break;
		}
		//キーワードリストを検索
	  case kMenuItemID_SearchInKeywordList:
		{
			SPI_SearchInKeywordList();
			break;
		}
		//全てのサブウインドウのロック状態を解除
	  case kMenuItemID_ClearAllLock:
		{
			SPI_ClearAllLockOfSubWindows();
			break;
		}
		//#850
		//ドキュメントをタブで開くかどうかを設定
	  case kMenuItemID_OpenDocumentUsingTab:
		{
			NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow,
		  !(NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow)));
			SPI_GetAppPrefWindow().NVI_UpdateProperty();
			break;
		}
		//#725
		//各フローティングウインドウを生成
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
			SPI_CreateFloatingSubWindow(kSubWindowType_TextMarker,0,300,180);//#1316 幅変更
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
			SPI_CreateFloatingSubWindow(kSubWindowType_KeyRecord,0,300,150);//#1316 幅変更
			break;
		}
	  case kMenuItemID_AddKeyRecordInFloating_AndStart:
		{
			SPI_CreateFloatingSubWindow(kSubWindowType_KeyRecord,0,300,150);//#1316 幅変更
			SPI_StartRecord();
			break;
		}
		//検索結果サブウインドウ追加 #1382
	  case kMenuItemID_AddFindResultInFloating:
		{
			GetApp().SPI_GetOrCreateFindResultWindowDocument();
			break;
		}
		//#844
		//新規モード追加ウインドウを表示
	  case kMenuItemID_AddNewMode:
		{
			SPI_GetAddNewModeWindow().NVI_CreateAndShow();
			SPI_GetAddNewModeWindow().NVI_SwitchFocusToFirst();
			break;
		}
		//#1050
		//タブセット保存
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
		//はじめにお読みください #1351
	  case kMenuItemID_ReadMeFirst:
		{
			SPI_ShowReadMeFirst();
			break;
		}
		//#539 #endif
		//デバッグ
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
		//ユニットテスト
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
					//クライアントから受信する側のソケット
					ASocket	srvsocket;
					fprintf(stderr,"WAIT_ACCEPT\n");
					srvsocket.Accept(listensocket);
					fprintf(stderr,"WAIT_RECV\n");
					
					//クライアントからデータ受信
					AText	rcvtext;
					while(true)
					{
						//空行(CRLFCRLF)を受信するまでReceiveを繰り返す
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
					//行毎に分割
					ATextArray	rcvTextLineArray;
					rcvTextLineArray.ExplodeLineFromText(rcvtext);
					if( rcvTextLineArray.GetItemCount() > 0 )
					{
						//コマンド行取得
						AText	commandLine;
						rcvTextLineArray.Get(0,commandLine);
						//GETの場合
						if( commandLine.CompareMin("GET ") == true )
						{
							//"Host: "ラベルからホストサーバー名を取得
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
							//サーバーへ接続するソケット作成
							ASocket	clisocket;
							clisocket.Connect(httpservername,80);
							clisocket.Send(rcvtext);
							//サーバー接続ソケットから切断されるまでデータを全て受信
							clisocket.ReceiveAll(rcvtext);
							//データをクライアント側へ送信
							srvsocket.Send(rcvtext);
							rcvtext.OutputToStderr();
							//サーバー接続ソケットをclose
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
			//処理なし
			result = false;
			break;
		}
	}
	return result;
}

//メニュー更新
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
	//FTP機能未対応
#else
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FTPFolder,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FTPFolderSetup,true);
#endif
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FolderList,true);
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FolderListSetup,true);
	*/
	NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_NewWithTemplate,true);
	//FTPログ
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
	//タブモード
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
		//★
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
	//#138 付箋紙リスト
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
	//#725 if( (GetApp().NVI_GetMostFrontWindowID(kWindowType_Index) != kObjectID_Invalid) == true )//#92 trueの場合のみ設定するように変更
	if( SPI_GetActiveSubWindowForItemSelector() != kObjectID_Invalid )
	{
		NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_IndexNext,true);
		NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_IndexPrevious,true);
	}
	//ウインドウメニュー
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
	//全てのウインドウをタブ化
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
			//ウインドウが１つだけかつタブメインウインドウなら全てのウインドウをタブ化を不可にする
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
	//デバッグ
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
	//#725 各フローティングサブウインドウ
	for( AMenuItemID i = kMenuItemID_AddRecentlyOpenFileListInFloating; i <= /*#1382 kMenuItemID_AddKeyRecordInFloating*/kMenuItemID_AddFindResultInFloating; i++ )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(i,true);
	}
	//サブウインドウロック解除
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ClearAllLock,true);
	
	//#1050
	//タブセット保存
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SaveTabSet,true);
	
	//#1102
	//Sparkleアップデートを確認
#ifndef Target_Mac_OS_X_10_6
#ifdef __LP64__
	//10.7以上、64bitの場合のみ有効
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
//Tickタイマー
void	AApp::EVTDO_DoTickTimerAction()
{
	mAppElapsedTick++;
	/* #138 AApplicationに入れる
	AWindowID	frontWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontWindowID != kObjectID_Invalid )
	{
		SPI_GetTextWindowByID(frontWindowID).EVT_DoTickTimer();
	}
	mModePrefWindow.EVT_DoTickTimer();//win 080618
	mAppPrefWindow.EVT_DoTickTimer();//win 080618
	*/
	
	//#699
	//タイマーによる処理実行
	for( AIndex i = 0; i < mReservedTimedExecutionArray_Type.GetItemCount(); )
	{
		//タイマー値取得
		ANumber	timer = mReservedTimedExecutionArray_Timer.Get(i);
		timer--;
		//タイマー値判定
		if( timer <= 0 )
		{
			//タイマー満了なら処理実行
			switch(mReservedTimedExecutionArray_Type.Get(i))
			{
			  case kTimedExecutionType_TransitNextDocumentToWrapCalculatingState:
				{
					//GetApp().SPI_TransitNextDocumentToWrapCalculatingState();
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			//タイマー削除
			mReservedTimedExecutionArray_Type.Delete1(i);
			mReservedTimedExecutionArray_Timer.Delete1(i);
		}
		else
		{
			//タイマー未満了ならデクリメントしたタイマー値を設定
			mReservedTimedExecutionArray_Timer.Set(i,timer);
			i++;
		}
	}
}

//#699
/**
タイマー処理実行予約
*/
void	AApp::SPI_ReserveTimedExecution( const ATimedExecutionType inType, const ANumber inFireSecond )
{
	AIndex	index = mReservedTimedExecutionArray_Type.Find(inType);
	if( index == kIndex_Invalid )
	{
		//同じタイプが無ければタイマー追加
		mReservedTimedExecutionArray_Type.Add(inType);
		mReservedTimedExecutionArray_Timer.Add(inFireSecond/60);
	}
	else
	{
		//同じタイプがあればタイマー値再設定
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
	//B0420 同じフォルダ、同じプロジェクトのデータを更新（Finder等で変更した場合を考慮）
	SPI_UpdateAllSameFolderData();
	//#331 SPI_UpdateAllSameProjectData();
	//R0232 他のアプリでファイル編集があった場合にダイアログを表示
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
	/*#974 ここで実行すると、ダブルクリックで開いたファイルのopenイベントの前に新規ウインドウ生成されてしまうので、EVTDO_DoInternalEvent()後にまわす
	//#688
	//新規ウインドウ生成
	SPI_CreateNewWindowAutomatic();
	*/
	AObjectIDArray	objectIDArray;
	ABase::PostToMainInternalEventQueue(kInternalEvent_AppActivated,GetObjectID(),0,GetEmptyText(),objectIDArray);
}

//B0442
void	AApp::EVTDO_DoAppDeactivated()
{
	//処理無し
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
インデックスウインドウ・次へ
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
インデックスウインドウ・次へ
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
フルスクリーンモード切替
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
手のひらツールモード切替
*/
void	AApp::SPI_SwitchHandToolMode()
{
	mHandToolMode = !mHandToolMode;
}

//#724
/**
環境設定ウインドウ表示
*/
void	AApp::SPI_ShowAppPrefWindow()
{
	SPI_GetAppPrefWindow().NVI_CreateAndShow();
	SPI_GetAppPrefWindow().NVI_RefreshWindow();
	SPI_GetAppPrefWindow().NVI_SwitchFocusToFirst();
}

//#959
/**
アプリケーション共通のWindowアクティベート時処理
*/
void	AApp::EVTDO_WindowActivated()
{
	//各フローティングウインドウの表示／非表示を更新
	SPI_UpdateFloatingSubWindowsVisibility();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

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
	//ポップアップメニューの選択indexから、選択されたテキストエンコーディングを取得
	NSPopUpButton*	popup = [[panel contentView] viewWithTag:1000];
	AIndex	selItemIndex = [popup indexOfSelectedItem];
	AIndex	tecindex = selItemIndex-2;
	AText	tecname;
	if( tecindex >= 0 )
	{
		GetAppPref().GetTextEncodingNameArray().Get(tecindex,tecname);
	}
	//ドキュメント生成
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
	
	//OpenCustoView取得（未生成ならロード）
	NSView*	view = [((CocoaApp*)NSApp) getOpenCustomView];
	if( view == nil )
	{
		//ロード
		[NSBundle loadNibNamed:@"OpenCustom" owner:NSApp];
		view = [((CocoaApp*)NSApp) getOpenCustomView];
		if( view == nil )
		{
			return;
		}
	}
	
	[view retain];
	[panel setAccessoryView:view];
	
	//テキストエンコーディングのリスト取得
	AText	text;
	text.SetLocalizedText("OpenCustom_TextEncodingAutomatic");
	ATextArray	textArray;
	textArray.Add(text);
	text.SetCstring("-");
	textArray.Add(text);
	textArray.AddFromTextArray(GetAppPref().GetTextEncodingDisplayNameArray());
	//ポップアップメニューに設定
	NSPopUpButton*	popup = [view viewWithTag:1000];
	AMenuWrapper::DeleteAllMenuItems([popup menu]);
	for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
	{
		AText	text;
		textArray.Get(i,text);
		//メニュー項目追加
		//項目挿入
		AMenuWrapper::InsertMenuItem([popup menu],i,text,kMenuItemID_Invalid,0,0);
		//enable/disable設定
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
	//10.3より前のバージョンに対応するにはEmbedControlの処理必要
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
		//テキストエンコーディングのリスト
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
	//Mac以外はCtrl+M固定
}
*/

#pragma mark ===========================

#pragma mark ---情報取得

/*#305
ALanguage	AApp::SPI_GetCurrentLanguage()
{
	//暫定
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
//デフォルトのファイルフィルタ取得
void	AApp::SPI_GetDefaultFileFilter( AText& outText ) const
{
	outText.SetCstring("All Files (*.*)");
	outText.Add(0);
	outText.AddCstring("*.*");
	outText.Add(0);
	outText.Add(0);
}

#pragma mark ===========================

#pragma mark ---各種ファイル／フォルダ取得

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

#pragma mark ---メニュー更新

//
void	AApp::NVIDO_UpdateMenu()
{
	//まず、全てのメニューをDisableにする
	AApplication::GetApplication().NVI_GetMenuManager().DisableAllMenuBarMenuItems();
	//MenuUpdateイベントを自身に向かって投げる（メニューコマンド処理と同じルートを通す）
	GetApp().SPI_PostCommand(kCommandID_UpdateCommandStatus);
}
*/

#pragma mark ===========================

#pragma mark ---ウインドウメニュー管理

/*#922
void	AApp::SPI_UpdateWindowMenu()
{
	//ショートカット空き調査
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
	//ショートカット空き部分にテキストウインドウを割当
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
					//空きなし
					break;
				}
			}
		}
	}
	
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_WindowMenu);
	mWindowMenu_WindowIDArray.DeleteAll();
	mWindowMenu_TabIndexArray.DeleteAll();
	//テキストウインドウメニュー作成
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
	//最前面のドキュメントウインドウを検索
	for( AWindowRef windowRef = CWindowImp::GetMostFrontWindow(); windowRef != NULL; windowRef = AWindowWrapper::GetNextOrderWindow(windowRef) )
	{
		AWindowID	winID = NVI_GetWindowIDByWindowRef(windowRef);
		if( winID != kObjectID_Invalid )
		{
			//mWindowMenuWindowIDArrayでの、次のウインドウのIDを取得
			AIndex	index = FindWindowMenuIndex(winID,NVI_GetWindowByID(winID).NVI_GetCurrentTabIndex());
			if( index != kIndex_Invalid )
			{
				index++;
				if( index >= mWindowMenu_WindowIDArray.GetItemCount() )
				{
					index = 0;
				}
				//選択
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

#pragma mark ---「最近開いたファイル」メニュー管理

//「最近開いたファイル」への項目追加
void	AApp::SPI_AddToRecentlyOpenedFile( const AFileAcc& inFileAcc )
{
	AText	text;
	inFileAcc.GetPath(text);//#910 ,kFilePathType_1);//B0389 OK 最近開いたファイルのデータは旧形式にする
	SPI_AddToRecentlyOpenedFile(text);
}

void	AApp::SPI_AddToRecentlyOpenedFile( const AText& inPath )//#235
{
	//#923
	//最近開いたファイルへの追加抑制中なら何もせずリターン
	if( mSuppressAddToRecentlyOpenedFile == true )   return;
	
	//R0193
	if( GetAppPref().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )   return;
	
	//#411
	//テンポラリファイルは対象外（ODB経由ファイルなど）
	AIndex	pos = 0;
	if( inPath.FindCstring(0,"/private/var/",pos) == true )   return;
	
	//==================旧データ削除==================
	//AddRecentlyUsed_TextArray()を使うと、mRecentlyOpenedFileDisplayTextを同時に変更することができないので、下記に変更
	AIndex	rowIndex = GetAppPref().Find_TextArray(AAppPrefDB::kRecentlyOpenedFile,inPath);
	if( rowIndex != kIndex_Invalid )
	{
		//同じテキスト存在なら移動する。ドキュメント設定ファイルは削除しないので、DeleteRecentlyOpenedFileItem()はコールしない。
		GetAppPref().DeleteRow_Table(AAppPrefDB::kRecentlyOpenedFile,rowIndex);//#394
		mRecentlyOpenedFileDisplayText.Delete1(rowIndex);
		mRecentlyOpenedFileNameWithComment.Delete1(rowIndex);//B0413
		//メニューから削除
		NVI_GetMenuManager().DeleteDynamicMenuItemByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,rowIndex);
	}
	//==================追加==================
	//追加位置決定
	AIndex	insertIndex = 0;
	/*#394 ピン未実装
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
	//doc prefデータ追加
	GetAppPref().Insert1_TextArray(AAppPrefDB::kRecentlyOpenedFile,insertIndex,inPath);
	GetAppPref().Insert1_BoolArray(AAppPrefDB::kRecentlyOpenedFile_Pin,insertIndex,false);//#394
	//リストへのデータ追加
	mRecentlyOpenedFileDisplayText.Insert1(insertIndex,inPath);
	mRecentlyOpenedFileNameWithComment.Insert1(insertIndex,inPath);//B0413
	//メニューへ追加
	NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,insertIndex,inPath,inPath,
				kObjectID_Invalid,0,kModifierKeysMask_None,false);
	//制限数よりも多い部分を削除
	if( GetAppPref().GetItemCount_Array(AAppPrefDB::kRecentlyOpenedFile) > kRecentlyOpendFileMax )
	{
		DeleteRecentlyOpenedFileItem(mRecentlyOpenedFileNameWithComment.GetItemCount()-1);
	}
	//メニュー／リストの更新（最初のデータに関してのみ更新）
	SPI_UpdateRecentlyOpenedFile(true);
}

//R0186
//「最近開いたファイル」から項目削除
void	AApp::SPI_DeleteRecentlyOpenedFileItem( const AIndex inIndex )
{
	DeleteRecentlyOpenedFileItem(inIndex);
	//メニュー／リストの更新（全部更新）
	SPI_UpdateRecentlyOpenedFile(false);
}

//#241
/**
最近開いたファイルの項目削除（内部コール用・メニュー更新無し）
*/
void	AApp::DeleteRecentlyOpenedFileItem( const AIndex inIndex )
{
	//設定を削除
	GetAppPref().DeleteRow_Table(AAppPrefDB::kRecentlyOpenedFile,inIndex);//#394
	mRecentlyOpenedFileDisplayText.Delete1(inIndex);
	mRecentlyOpenedFileNameWithComment.Delete1(inIndex);
	//メニューから削除
	NVI_GetMenuManager().DeleteDynamicMenuItemByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,inIndex);
}

//R0186
//「最近開いたファイル」から、ファイルがみつからない項目をすべて削除
void	AApp::SPI_DeleteRecentlyOpenedFileNotFound()
{
	for( AIndex i = 0; i < GetAppPref().GetItemCount_Array(AAppPrefDB::kRecentlyOpenedFile); )
	{
		AText	text;
		GetAppPref().GetData_TextArray(AAppPrefDB::kRecentlyOpenedFile,i,text);
		//#235 ftp://で始まる項目はとばす
		//#0 sftp等の場合はsftp://等となるので、://を含むかどうかの判定に修正。 if( text.CompareMin("ftp://") == true )
		if( text.Contains("://") == true )//#0
		{
			i++;//#0 不具合修正
			continue;
		}
		AFileAcc	file;
		file.Specify(text);//#910 ,kFilePathType_1);//B0389 OK ここはデータ互換性のため旧タイプのまま。
		if( file.Exist() == true )
		{
			i++;
		}
		else
		{
			//データ削除（起動時にコールされた時は、まだmRecentlyOpenedFileDisplayTextは出来ていないので、DeleteRecentlyOpenedFileItem()コールはしない。下のSPI_UpdateRecentlyOpenedFile()で再構成されるので、mRecentlyOpenedFileDisplayText等を削除しなくても問題無い）
			GetAppPref().DeleteRow_Table(AAppPrefDB::kRecentlyOpenedFile,i);
		}
	}
	//メニュー／リストの更新（全部更新）
	SPI_UpdateRecentlyOpenedFile(false);
}

//win 080710
//「最近開いたファイル」の情報を全て削除
void	AApp::SPI_DeleteAllRecentlyOpenedFile()
{
	while( GetAppPref().GetItemCount_Array(AAppPrefDB::kRecentlyOpenedFile) > 0 )
	{
		DeleteRecentlyOpenedFileItem(0);
	}
	//メニュー／リストの更新（全部更新）
	SPI_UpdateRecentlyOpenedFile(false);
}

/**
「最近開いたファイル」メニュー／リスト表示更新（最近開いたファイル追加・削除時等にコールされる）
@param inUpdateOnlyFirstItem: true: 最初の項目のみ更新  false: 全て更新
*/
void	AApp::SPI_UpdateRecentlyOpenedFile( const ABool inUpdateOnlyFirstItem )
{
	//==================メニューの更新＋mRecentlyOpenedFileDisplayTextの更新==================
	UpdateRecentlyOpenedFileDisplayText(inUpdateOnlyFirstItem);
	
	//==================ファイルリストの更新==================
	SPI_UpdateFileListWindows(kFileListUpdateType_RecentlyOpenedFileUpdated);//#725
}

//「最近開いたファイル」の表示用テキスト配列を作成する
//inUpdateOnlyFirstItem: 最初の項目に関してのみUpdateする場合true B0000高速化
void	AApp::UpdateRecentlyOpenedFileDisplayText( const ABool inUpdateOnlyFirstItem )
{
	AText	notfound;
	notfound.SetLocalizedText("RecentlyOpenedFile_FileNotFound");
	AItemCount	fileCount = GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFile).GetItemCount();
	AItemCount	commentCount = GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFileCommentPath).GetItemCount();
	//B0000高速化
	AItemCount	targetEnd = fileCount;
	if( inUpdateOnlyFirstItem == true )
	{
		targetEnd = 1;
	}
	//
	AHashTextArray	textArray;
	//ファイル名＋コメントを配列に追加していく
	for( AIndex i = 0; i < targetEnd; i++ )//B0000 fileCount->targetEnd
	{
		//ファイルパス、ファイル名取得
		AText	path;
		GetAppPref().GetData_TextArrayRef(AAppPrefDB::kRecentlyOpenedFile).Get(i,path);
		//#235 ftpの場合はそのまま表示
		//#0 sftp等の場合はsftp://等となるので、://を含むかどうかの判定に修正。 if( path.CompareMin("ftp://") == true )
		if( path.Contains("://") == true )//#0
		{
			textArray.Add(path);
			continue;
		}
		//
		AFileAcc	fileacc;
		fileacc.Specify(path);//#910 ,kFilePathType_1);//B0389 最近開いたファイルのデータは旧形式にする
		AText	text;
		fileacc.GetFilename(text);
		/*#910
		//B0389 各ファイルパス形式でのファイルパス取得
		AText	p0, p2;
		fileacc.GetPath(p0,kFilePathType_Default);
		fileacc.GetPath(p2,kFilePathType_2);
		if( p2.GetItemCount() > 0 )   p2.Delete(0,1);
		*/
		//注釈パスとの一致を検索
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
		//#1301 /*#932 存在チェックは行わない。
		if( fileacc.Exist() == false )
		{
			text.SetText(path);
			text.InsertText(0,notfound);
		}
		//#1301 */
		textArray.Add(text);
	}
	//B0000 inUpdateOnlyFirstItemがtrueのときは、targetEnd以降のデータをmRecentlyOpenedFileDisplayTextから構成する→B0413 mRecentlyOpenedFileNameWithCommentから構成する
	if( inUpdateOnlyFirstItem == true )
	{
		//最初の項目だけをUpdateする場合:
		//mRecentlyOpenedFileNameWithCommentの2番目以降の項目をtextArrayへ追加する
		for( AIndex i = targetEnd; i < /*B0413 mRecentlyOpenedFileDisplayText*/mRecentlyOpenedFileNameWithComment.GetItemCount(); i++ )
		{
			textArray.Add(mRecentlyOpenedFileNameWithComment.GetTextConst(i));
		}
		//mRecentlyOpenedFileNameWithCommentとmRecentlyOpenedFileDisplayTextに、textArrayの内容をコピーする B0413
		for( AIndex i = 0; i < targetEnd; i++ )
		{
			//上で作成した表示テキストを取得
			AText	text;
			textArray.Get(i,text);
			//リスト更新
			mRecentlyOpenedFileNameWithComment.Set(i,text);
			mRecentlyOpenedFileDisplayText.Set(i,text);
			//メニューテキスト更新
			NVI_GetMenuManager().SetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,i,text);
			//メニューactionテキスト更新
			AText	actiontext;
			GetAppPref().GetData_TextArray(AAppPrefDB::kRecentlyOpenedFile,i,actiontext);
			NVI_GetMenuManager().SetDynamicMenuItemActionTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,i,actiontext);
		}
	}
	//B0413
	else
	{
		//全ての項目をUpdateする場合:
		//mRecentlyOpenedFileNameWithCommentとmRecentlyOpenedFileDisplayTextに、textArrayの内容をコピーする
		mRecentlyOpenedFileNameWithComment.DeleteAll();
		mRecentlyOpenedFileDisplayText.DeleteAll();
		for( AIndex i = 0; i < targetEnd; i++ )
		{
			mRecentlyOpenedFileNameWithComment.Add(textArray.GetTextConst(i));
			mRecentlyOpenedFileDisplayText.Add(textArray.GetTextConst(i));
		}
		//メニュー更新
		NVI_GetMenuManager().SetDynamicMenuItemByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,mRecentlyOpenedFileDisplayText,
					GetAppPref().GetData_TextArrayRef(AAppPrefDB::kRecentlyOpenedFile));
	}
	//B0413 この段階で、inUpdateOnlyFirstItemに関わらず textArrayにはファイル名＋注釈が入っていることとなる
	//mRecentlyOpenedFileNameWithCommentには、ファイル名＋注釈
	//mRecentlyOpenedFileDisplayTextには、
	//inUpdateOnlyFirstItemがtrueの場合：最初の項目は「ファイル名＋注釈」、それ以外は「ファイル名＋注釈＋(パス)」
	//inUpdateOnlyFirstItemがfalseの場合：「ファイル名＋注釈」
	
	//R0153 同じファイル名（＋コメント）がある時は、
	for( AIndex i = 0; i < targetEnd; i++ )//B0000 fileCount->targetEnd
	{
		AText	targetfilename;
		textArray.Get(i,targetfilename);
		if( targetfilename.CompareMin(notfound) )   continue;
		//自分自身にひっかからないようにするため、textArrayのほうには、最後に空白スペースをつける
		AText	text_;
		text_.SetText(targetfilename);
		text_.AddCstring(" ");
		textArray.Set(i,text_);
		ABool	first = true;
		//textArrayから、targetfilenameと同じファイル名があるかどうか検索
		for( AIndex loop = 0; loop < fileCount; loop++ )//最大fileCount個同じものがある
		{
			AIndex	samenum = textArray.Find(targetfilename);
			if( samenum == kIndex_Invalid )   break;
			if( first == true )
			{
				//比較元のほうにパス名付加して、mRecentlyOpenedFileDisplayTextに設定 B0406の変更により不要かも？
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
				//メニュー設定
				NVI_GetMenuManager().SetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,i,newtext);
			}
			//比較先のほうにパス名付加して、mRecentlyOpenedFileDisplayTextに設定
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
			//メニュー設定
			NVI_GetMenuManager().SetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,samenum,sametext);
		}
	}
	//最後にmRecentlyOpenedFileDisplayTextへコピー
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

/*ピン未実装
void	AApp::SPI_SetPinForRecentlyOpenedFileItem( const AIndex inIndex, const ABool inEnablePin )
{
	if( inEnablePin == true )
	{
		//==================ピン設定==================
		
		//指定項目をindex=0に追加
		AText	path;
		GetAppPref().GetData_TextArray(AAppPrefDB::kRecentlyOpenedFile,inIndex,path);
		SPI_AddToRecentlyOpenedFile(path);
		//index=0にピン設定
		NVI_GetAppPrefDB().SetData_BoolArray(AAppPrefDB::kRecentlyOpenedFile,0,true);
	}
	else
	{
		//==================
		
		//指定項目のピン解除
		
	}
}
*/

#pragma mark ===========================

#pragma mark ---フォルダーラベル

/**
フォルダーラベルを検索
*/
void	AApp::SPI_FindFolderLabel( const AText& inDocFilePath, ATextArray& outFolderPathArray, ATextArray& outLabelTextArray ) const
{
	//フォルダラベル設定の各項目ごとのループ
	AItemCount	commentCount = NVI_GetAppPrefDBConst().GetItemCount_TextArray(AAppPrefDB::kRecentlyOpenedFileCommentPath);
	for( AIndex i = 0; i < commentCount; i++ )
	{
		//パス取得
		AText	folderpath, label;
		NVI_GetAppPrefDBConst().GetData_TextArray(AAppPrefDB::kRecentlyOpenedFileCommentPath,i,folderpath);
		if( folderpath.CompareMin(inDocFilePath) == true )
		{
			//ドキュメントファイルが設定パスの配下の場合、結果に追加
			NVI_GetAppPrefDBConst().GetData_TextArray(AAppPrefDB::kRecentlyOpenedFileComment,i,label);
			outFolderPathArray.Add(folderpath);
			outLabelTextArray.Add(label);
		}
	}
}

#pragma mark ===========================

#pragma mark ---キルバッファー
//#913

/**
キルバッファに追加
*/
void	AApp::SPI_AddToKillBuffer( const ADocumentID inTextDocID, const ATextPoint& inTextPoint, const AText& inText )
{
	if( inTextDocID == mKillBuffer_DocumentID && 
	   inTextPoint.x == mKillBuffer_TextPoint.x && inTextPoint.y == mKillBuffer_TextPoint.y )
	{
		//連続削除の場合は、キルバッファに追加
		mKillBuffer_Text.AddText(inText);
	}
	else
	{
		//連続削除でない場合は、キルバッファに新規設定
		mKillBuffer_Text.SetText(inText);
	}
	//今回のドキュメントとtext pointを記憶
	mKillBuffer_DocumentID = inTextDocID;
	mKillBuffer_TextPoint = inTextPoint;
}

/**
キルバッファデータを取得
*/
void	AApp::SPI_GetKillBuffer( AText& outText )
{
	outText.SetText(mKillBuffer_Text);
	mKillBuffer_TextPoint.x = mKillBuffer_TextPoint.y = -1;
}

#pragma mark ===========================

#pragma mark ---「新規」メニュー管理

void	AApp::SPI_UpdateNewMenu()
{
	//モードごとの新規メニュー
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_NewTextDocument);
	AText	newShortcutModeName;
	GetAppPref().GetData_Text(AAppPrefDB::kCmdNModeName,newShortcutModeName);
	for( AIndex index = 0; index < mModePrefDBArray.GetItemCount(); index++ )
	{
		//disableなモードは新規メニューに追加しない
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
		if( modeRawName.Compare(newShortcutModeName) == true || modeDisplayName.Compare(newShortcutModeName) == true )//#1356 バージョン2では表示用モード名が格納されているので表示用モード名と一致した時でもコマンドNを設定する
		{
			shortcut = 'N';
		}
		//#305 AAppPrefDB::kCmdNModeNameが未設定の場合は、最初の項目に設定
		if( newShortcutModeName.GetItemCount() == 0 && index == 0 )
		{
			shortcut = 'N';
		}
		//
		NVI_GetMenuManager().AddDynamicMenuItemByMenuItemID(kMenuItemID_NewTextDocument,modeDisplayName,modeRawName,
				kObjectID_Invalid,shortcut,modifiers,false);//win
	}
	//テンプレートメニュー
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

#pragma mark ---「同じフォルダから開く」メニュー管理

//最前面ドキュメントが変わった時にコールされる
//「同じフォルダ」メニューの内容を更新する。
void	AApp::SPI_UpdateSameFolderMenu( const AFileAcc& inFolder )
{
	AIndex	sameFolderIndex = FindSameFolderIndex(inFolder);
	
	//フォルダ未登録なら新規生成
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
		SCMRegisterFolder(inFolder);//SCM状態読み込み終了後、InternalEvent経由でファイルリストのSPI_UpdateTable()等が実行される
		*/
	}
	
	//現在のメニューに登録されたFolderなら何もせずリターン
	if( mCurrentSameFolder == sameFolderIndex )   return;
	
	mCurrentSameFolder = sameFolderIndex;
	
	//「同じフォルダから開く」メニュー更新
	UpdateSameFolderMenu();//B0420
	
	//SPI_GetFileListWindow().SPI_UpdateTable_SameFolder(inActiveFile);
}

/**
「同じフォルダから開く」メニューデータのindexを取得
*/
AIndex	AApp::FindSameFolderIndex( const AFileAcc& inFolder )
{
	AIndex	sameFolderIndex = kIndex_Invalid;
	//#932
	AStMutexLocker	locker(mSameFolderMutex);
	
	//既存SameFoler配列からフォルダ検索
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
//「同じフォルダから開く」メニュー更新
void	AApp::UpdateSameFolderMenu()
{
	//現在のメニュー項目を削除
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_SameFolder);
	if( mCurrentSameFolder == kIndex_Invalid )   return;
	
	//#982
	AStMutexLocker	locker(mSameFolderMutex);
	
	//メニューに１項目ずつ登録
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

//各フォルダのファイルリスト情報(mSameFolder_Filename)を作成する（フォルダ内のファイルを検索してmSameFolder_Filenameへ追加していく）
//mSameFolder_Filenameの指定Indexの配列はこの関数内の最初で全削除される
void	AApp::SPI_MakeFilenameListSameFolder( const AIndex inIndex, const AFileAcc& inFolder )
{
	AStMutexLocker	locker(mSameFolderMutex);//#982
	
	mSameFolder_Filename.GetObject(inIndex).DeleteAll();
	//フォルダ内のファイルを取得
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		if( children.GetObject(index).IsFolder() == true )   continue;
		
		//win 080702 不可視ファイルを表示しない設定ならmSameFolder_Filenameに追加しない
		if( GetAppPref().GetData_Bool(AAppPrefDB::kFileListDontDisplayInvisibleFile) == true )
		{
			if( children.GetObject(index).IsInvisible() == true )   continue;
		}
		//mSameFolder_Filenameへファイル名を追加
		AText	filename;
		children.GetObject(index).GetFilename(filename);
		filename.ConvertToCanonicalComp();//#1087
		if( filename.Compare(".DS_Store") == true )   continue;
		mSameFolder_Filename.GetObject(inIndex).Add(filename);
	}
}

//ファイルリストウインドウ用データ取得メソッド

//フォルダからSameFolderのIndexを取得する
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
//miへのアプリ切替時にSameFolderの全データを更新する
void	AApp::SPI_UpdateAllSameFolderData()
{
	{
		//#932
		AStMutexLocker	locker(mSameFolderMutex);
		
		//mSameFolder_Folderに登録されたデータを全て更新する
		for( AIndex i = 0; i < mSameFolder_Folder.GetItemCount(); i++ )
		{
			//#932 MakeFilenameListSameFolder(i,mSameFolder_Folder.GetObject(i));
			//#932
			ASameFolderLoaderFactory	factory(NULL);
			AObjectID	threadObjectID = GetApp().NVI_CreateThread(factory);
			(dynamic_cast<ASameFolderLoader&>(GetApp().NVI_GetThreadByID(threadObjectID))).SPNVI_Run(i,mSameFolder_Folder.GetObject(i));
		}
	}
	//ファイルリストのテーブル更新
	/*#725
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().SPI_UpdateTable();
	}
	*/
	/*#932
	SPI_UpdateFileListWindows(kFileListUpdateType_SameFolderDataUpdated);//#725
	//「同じフォルダから開く」メニュー更新
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

#pragma mark ---「同じプロジェクト」

/**
プロジェクトフォルダ登録
*/
AIndex	AApp::SPI_RegisterProjectFolder( const AFileAcc& inProjectFolder )
{
	//#723
	AStMutexLocker	locker(mSameProjectArrayMutex);
	
	//#958
	//プロジェクトフォルダ未設定（miprojファイル無し）の場合は、kIndex_Invalidを返す
	if( inProjectFolder.IsSpecified() == false )
	{
		return kIndex_Invalid;
	}
	//すでに同じプロジェクトフォルダが登録済みならそのindexを返す
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
	//#402 フォルダ読み込みスレッド起動
	AProjectFolderLoaderFactory	factory(this);
	AObjectID	threadObjectID = NVI_CreateThread(factory);
	(dynamic_cast<AProjectFolderLoader&>(NVI_GetThreadByID(threadObjectID))).SPNVI_Run(
																					   mSameProject_ProjectFolder.GetObjectConst(sameProjectArrayIndex),
																					   NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFileListDontDisplayInvisibleFile));
	
	//==================折りたたみ状態を復元==================
	//#892
	//プロジェクトパス取得
	AText	projectpath;
	inProjectFolder.GetPath(projectpath);
	projectpath.RemoveLastPathDelimiter();
	//折りたたまれたフォルダの設定項目毎のループ
	AItemCount	itemcount = mAppPref.GetItemCount_TextArray(AAppPrefDB::kFileListCollapseFolder);
	for( AIndex i = 0; i < itemcount; i++ )
	{
		//折りたたまれたフォルダパス設定を取得
		AText	path;
		mAppPref.GetData_TextArray(AAppPrefDB::kFileListCollapseFolder,i,path);
		//折りたたまれたフォルダパス設定が、プロジェクト配下ならば、mSameProjectArray_CollapseDataに格納
		if( path.CompareMin(projectpath) == true && path.GetItemCount() > projectpath.GetItemCount() )
		{
			//折りたたまれたフォルダパス設定から相対パスを削除して、mSameProjectArray_CollapseDataに格納
			path.Delete(0,projectpath.GetItemCount());
			mSameProjectArray_CollapseData.GetObject(sameProjectArrayIndex).Add(path);
		}
	}
	
	return sameProjectArrayIndex;
}

//ProjectFolderメニュー更新
void	AApp::SPI_UpdateSameProjectMenu( const AFileAcc& inProjectFolder, const AModeIndex inModeIndex )
{
	//#932 存在チェックしない。非存在でもローダーで取得できないだけなので問題は少ない。 if( inProjectFolder.Exist() == false )   return;

	//既存SameProject配列からプロジェクトフォルダ検索
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
	//プロジェクトフォルダ未登録なら新規生成
	if( sameProjectArrayIndex == kIndex_Invalid )
	{
		sameProjectArrayIndex = SPI_RegisterProjectFolder(inProjectFolder);
	}
	//inProjectFolderがプロジェクトフォルダではない場合（miproj無し）は、現在のメニューをinvalidにしてリターン
	if( sameProjectArrayIndex == kIndex_Invalid )
	{
		mCurrentSameProjectIndex = kIndex_Invalid;
		mCurrentSameProjectModeIndex = kIndex_Invalid;
		return;
	}
	
	//現在のメニューに登録されたPrjectFolderなら何もせずリターン
	if( mCurrentSameProjectIndex == sameProjectArrayIndex && 
				mCurrentSameProjectModeIndex == inModeIndex )   return;//#533
	
	//
	mCurrentSameProjectIndex = sameProjectArrayIndex;
	mCurrentSameProjectModeIndex = inModeIndex;//#533
	//「同じプロジェクトから開く」メニュー更新
	UpdateSameProjectMenu();//B0420 #533
	
}

//B0420
//「同じプロジェクトから開く」メニュー更新
void	AApp::UpdateSameProjectMenu()//#533
{
	/*#922
	//現在のメニュー項目を削除
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_SameProject);
	//
	if( mCurrentSameProjectIndex == kIndex_Invalid )   return;
	AText	projectFolderPath;
	mSameProject_ProjectFolder.GetObjectConst(mCurrentSameProjectIndex).GetPath(projectFolderPath);
	AItemCount	projectFolderPathLen = projectFolderPath.GetItemCount();
	//メニューに１項目ずつ登録
	{
		//#723
		AStMutexLocker	locker(mSameProjectArrayMutex);
		
		for( AIndex menuItem = 0; menuItem < mSameProjectArray_Pathname.GetObject(mCurrentSameProjectIndex).GetItemCount(); menuItem++ )
		{
			//#533
			AModeIndex	modeIndex = mSameProjectArray_ModeIndex.GetObject(mCurrentSameProjectIndex).Get(menuItem);
			//フォルダ項目、または、現在のモードと同じモードのファイルのみを追加する
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

/*#402 スレッド処理に変更
//mSameProjectArray_Pathnameの内容作成（プロジェクトフォルダを再帰的に検索して全ファイル登録）
//mSameProjectArray_Pathnameの指定Indexの配列はこの関数内では削除されない（削除せずに追加していく）ので、関数コール元で削除必要
void	AApp::MakeSameProjectArrayRecursive( const AIndex inSameProjectArrayIndex, const AFileAcc& inFolder, const AModeIndex inModeIndex )
{
	//R0006
	SCMRegisterFolder(inFolder);//SCM状態読み込み終了後、InternalEvent経由でファイルリストのSPI_UpdateTable()等が実行される
	//
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		AFileAcc	child;
		child.CopyFrom(children.GetObjectConst(index));
		//win 080702 不可視ファイルを表示しない設定ならmSameProjectArray_Pathnameに追加しない
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
//miへのアプリ切替時にSameProjectの全データを更新する
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
			//mSameProjectArray_Pathnameに登録されたデータを全て更新する
			mSameProjectArray_Pathname.GetObject(i).DeleteAll();
			mSameProjectArray_ModeIndex.GetObject(i).DeleteAll();//#533
			mSameProjectArray_IsFolder.GetObject(i).DeleteAll();
			//#402 MakeSameProjectArrayRecursive(i,mSameProject_ProjectFolder.GetObjectConst(i),mSameProject_ModeIndex.Get(i));
			projectFolerArray.GetObject(projectFolerArray.AddNewObject()).CopyFrom(mSameProject_ProjectFolder.GetObjectConst(i));//#402
		}
	}
	
	//#402 フォルダ読み込みスレッド起動
	AProjectFolderLoaderFactory	factory(this);
	AObjectID	threadObjectID = NVI_CreateThread(factory);
	(dynamic_cast<AProjectFolderLoader&>(NVI_GetThreadByID(threadObjectID))).SPNVI_Run(projectFolerArray,
			NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFileListDontDisplayInvisibleFile));
	
	//
	//ファイルリストのテーブル更新
	/*#725
	if( mFileListWindowID != kObjectID_Invalid )//#693
	{
		SPI_GetFileListWindow().SPI_UpdateTable();
	}
	*/
	/*#932 ローダー完了時に処理
	SPI_UpdateFileListWindows(kFileListUpdateType_SameProjectDataUpdated);//#725
	//「同じプロジェクトから開く」メニュー更新
	UpdateSameProjectMenu();
	*/
}

//ファイルリストウインドウ用データ取得メソッド

//フォルダからSameProjectのIndexを取得する
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
同じプロジェクトのデータ取得（ファイルリストからコールされる）
*/
void	AApp::SPI_GetCurrentSameProjectData( AText& outProjectFolderPath,
											ATextArray& outFullPathArray, AArray<AModeIndex>& outModeArray,
											ABoolArray& outIsFolderArray ) const
{
	//#533 if( mCurrentSameProjectIndex == kIndex_Invalid )   return;
	//結果初期化
	outProjectFolderPath.DeleteAll();
	outFullPathArray.DeleteAll();
	outModeArray.DeleteAll();
	outIsFolderArray.DeleteAll();
	//現在のプロジェクトが無ければリターン
	if( mCurrentSameProjectIndex == kIndex_Invalid )   return;//#0
	//
	{
		AStMutexLocker	locker(mSameProjectArrayMutex);
		
		//プロジェクトフォルダのパスを取得（常に最後のパスデリミタ有りにする（ファイルリストで相対パスを生成するために使っているため））
		mSameProject_ProjectFolder.GetObjectConst(mCurrentSameProjectIndex).GetPath(outProjectFolderPath);
		outProjectFolderPath.AddPathDelimiter(AFileAcc::GetPathDelimiter());
		//各データ取得
		outFullPathArray.SetFromTextArray(mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex));
		outModeArray.SetFromObject(mSameProjectArray_ModeIndex.GetObjectConst(mCurrentSameProjectIndex));
		outIsFolderArray.SetFromObject(mSameProjectArray_IsFolder.GetObjectConst(mCurrentSameProjectIndex));
	}
}

//#892
/**
指定プロジェクトの、折りたたまれたフォルダのパスのリストを取得
*/
void	AApp::SPI_GetProjectFolderCollapseData( const AIndex inProjectIndex, ATextArray& outCollapseData ) const
{
	//#723
	AStMutexLocker	locker(mSameProjectArrayMutex);
	
	//折りたたまれたフォルダのパス取得
	outCollapseData.SetFromTextArray(mSameProjectArray_CollapseData.GetObjectConst(inProjectIndex));
}

//#892
/**
指定プロジェクトの、折りたたまれたフォルダのパスのリストを設定
*/
void	AApp::SPI_SaveProjectFolderCollapseData( const AIndex inProjectIndex, const ATextArray& inCollapseData )
{
	//#723
	AStMutexLocker	locker(mSameProjectArrayMutex);
	
	//折りたたまれたフォルダのパス設定
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
		
		//パス取得
		mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex).Get(inIndex,path);
	}
	outFile.Specify(path);
}

//#892
/**
プロジェクトフォルダ取得
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
//「同じプロジェクト」のパス配列取得（index指定）
const ATextArray&	AApp::SPI_GetSameProjectPathnameArray( const AIndex inProjectIndex ) const
{
	//#723
	AStMutexLocker	locker(mSameProjectArrayMutex);
	
	//
	return mSameProjectArray_Pathname.GetObjectConst(inProjectIndex);
}

/**
「同じプロジェクト」のパスリストのうち、指定モードに一致するパスのリストを取得
*/
void	AApp::SPI_GetSameProjectPathnameArrayMatchMode( const AModeIndex inModeIndex, ATextArray& outPathnameArray) const
{
	//結果初期化
	outPathnameArray.DeleteAll();
	
	AStMutexLocker	locker(mSameProjectArrayMutex);
	//現在のプロジェクトの各ファイルパス毎のループ
	AItemCount	itemCount = mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex).GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//モードが一致したら結果に追加
		if( mSameProjectArray_ModeIndex.GetObjectConst(mCurrentSameProjectIndex).Get(i) == inModeIndex )
		{
			AText	path;
			mSameProjectArray_Pathname.GetObjectConst(mCurrentSameProjectIndex).Get(i,path);
			outPathnameArray.Add(path);
		}
	}
}

#pragma mark ===========================

#pragma mark ---キーワードCSVファイルロード
//#796

/**
キーワードCSVファイル(http経由)読み込み要求
*/
void	AApp::SPI_LoadCSVForKeyword( const AText& inModeName, const AText& inURL, const AIndex inCategoryIndex )
{
	//モード名、カテゴリ名記憶（LoadCSVForKeyword_Result()で使う）
	ATextArray	infoTextArray;
	infoTextArray.Add(inModeName);
	AText	categoryIndexText;
	categoryIndexText.SetNumber(inCategoryIndex);
	infoTextArray.Add(categoryIndexText);
	//CSVファイル読みこみスレッド開始
	ACurlThreadFactory	factory(this);
	AObjectID	threadObjectID = NVI_CreateThread(factory);
	(dynamic_cast<ACurlThread&>(NVI_GetThreadByID(threadObjectID))).
			SPNVI_Run(kCurlRequestType_HTTP,inURL,
				kCurlTransactionType_LoadCSVForKeyword,infoTextArray);
}

/**
キーワードCSVファイル(http経由)読み込み完了時処理
*/
void	AApp::LoadCSVForKeyword_Result( const AText& inResultText, const ATextArray& inInfoTextArray )
{
	//テキストエンコーディング変換
	AText	resultText;
	resultText.SetText(inResultText);
	ATextEncodingFallbackType	fallbackResult = kTextEncodingFallbackType_None;
	ATextEncoding	resultTec = ATextEncodingWrapper::GetUTF8TextEncoding();
	AReturnCode	resultReturnCode = returnCode_CR;
	ADocPrefDB	docPrefDB;
	ABool	notMixed = true;//#995
	SPI_ConvertToUTF8CRUsingDocPref(kLoadTextPurposeType_KeywordCSVFile,resultText,docPrefDB,GetEmptyText(),
									resultTec,resultReturnCode,fallbackResult,notMixed);
	//モード取得
	AText	modeName;
	inInfoTextArray.Get(0,modeName);
	AIndex	modeIndex = SPI_FindModeIndexByModeRawName(modeName);
	if( modeIndex == kIndex_Invalid )   return;
	//カテゴリ取得
	AText	categoryIndexText;
	inInfoTextArray.Get(1,categoryIndexText);
	//CSVファイルからキーワードに追加
	SPI_GetModePrefDB(modeIndex).StartAddCSVKeywordThread(resultText,categoryIndexText.GetNumber());
}

#pragma mark ===========================

#pragma mark ---関連ファイルメニュー

void	AApp::SPI_UpdateImportFileMenu( const AObjectArray<AFileAcc>& inFileArray )
{
	//現在のメニュー項目を削除
	NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_OpenImportFile);
	
	//メニューに１項目ずつ登録
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

#pragma mark ---モードメニュー管理

void	AApp::SPI_UpdateModeMenu()
{
	//#914 enable/all
	
	//TextArray取得
	ATextArray	enabledModeDisplayNameArray, allModeDisplayNameArray;
	ATextArray	enabledModeRawNameArray, allModeRawNameArray;
	for( AModeIndex index = 0; index < mModePrefDBArray.GetItemCount(); index++ )
	{
		AText	modeDisplayName, modeRawName;
		mModePrefDBArray.GetObjectConst(index).GetModeDisplayName(modeDisplayName);
		mModePrefDBArray.GetObjectConst(index).GetModeRawName(modeRawName);
		if( mModePrefDBArray.GetObjectConst(index).IsModeEnabled() == true )
		{
			//enabledなモードのリストに追加
			enabledModeDisplayNameArray.Add(modeDisplayName);
			enabledModeRawNameArray.Add(modeRawName);
		}
		else
		{
			modeDisplayName.AddCstring("(disabled)");
		}
		//全てのモードのリストに追加
		allModeDisplayNameArray.Add(modeDisplayName);
		allModeRawNameArray.Add(modeRawName);
	}
	//TextArrayメニュー（コントロール上のメニュー）の更新
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_EnabledMode,enabledModeDisplayNameArray,enabledModeRawNameArray);//#232
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_AllMode,allModeDisplayNameArray,allModeRawNameArray);//#232
	//メニューバーのモードメニューの更新
	NVI_GetMenuManager().SetDynamicMenuItemByMenuItemID(kMenuItemID_SetMode,enabledModeDisplayNameArray,enabledModeRawNameArray);
	NVI_GetMenuManager().SetDynamicMenuItemByMenuItemID(kMenuItemID_ModePref,allModeDisplayNameArray,allModeRawNameArray);
	//ツールバーの設定ボタンメニューを更新
	AText	appPrefDisplayName;
	appPrefDisplayName.SetLocalizedText("Toolbar_AppPref");
	AText	separantor("-");
	AText	modePrefDisplayName;
	modePrefDisplayName.SetLocalizedText("Toolbar_ModePref");
	modePrefDisplayName.Insert1(0,0x00);//disable項目
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
	
	//「新規」メニュー更新
	SPI_UpdateNewMenu();
}

/**
モードメニューショートカット（現在のモード）の更新
*/
void	AApp::SPI_UpdateModeMenuShortcut()
{
	//最前面ドキュメント取得
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		//現在のモードを取得
		AIndex	currentModeIndex = SPI_GetTextDocumentByID(docID).SPI_GetModeIndex();
		//モードメニュー各項目ごとのループ
		AItemCount	count = NVI_GetMenuManager().GetDynamicMenuItemCountByMenuItemID(kMenuItemID_ModePref);
		for( AIndex index = 0; index < count; index++ )
		{
			//各項目にショートカット設定
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

#pragma mark ---テキストエンコーディングメニュー管理

void	AApp::SPI_UpdateTextEncodingMenu()
{
	//TextArray取得
	/*#844
	ATextArray	textArray;
	textArray.SetFromTextArray(GetAppPref().GetTextEncodingMenuFixedTextArrayRef());
	textArray.AddFromTextArray(GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kTextEncodingMenu));
	*/
	//TextArrayメニュー（コントロール上のメニュー）の更新
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_TextEncoding,//#232
																					 GetAppPref().GetTextEncodingDisplayNameArray(),
																					 GetAppPref().GetTextEncodingNameArray());
	//メニューバーのテキストエンコーディングメニューの更新
	NVI_GetMenuManager().SetDynamicMenuItemByMenuItemID(kMenuItemID_SetTextEncoding,
														GetAppPref().GetTextEncodingDisplayNameArray(),
														GetAppPref().GetTextEncodingNameArray());
}

/*#193
#pragma mark ===========================

#pragma mark ---FTPフォルダメニュー管理

//FTPフォルダメニューを作成する（設定に従ってルートの全てのメニュー項目を作成する）
void	AApp::MakeFTPFolderMenu()
{
	for( AIndex index = 0; index < GetAppPref().GetItemCount_Array(AAppPrefDB::kFTPFolder_Server); index++ )
	{
		SPI_UpdateFTPFolderMenu(kModificationType_RowAdded,index);
	}
}

//FTPフォルダメニュー更新
void	AApp::SPI_UpdateFTPFolderMenu( const AModificationType inModificationType, const AIndex inIndex )
{
	switch(inModificationType)
	{
		//値変更
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
				//項目削除
				SPI_UpdateFTPFolderMenu(kModificationType_RowDeleted,inIndex);
				//項目追加
				SPI_UpdateFTPFolderMenu(kModificationType_RowAdded,inIndex);
			}
			break;
		}
		//項目追加
	  case kModificationType_RowAdded:
		{
			//項目追加
			//階層取得
			ANumber	depth = GetAppPref().GetData_NumberArray(AAppPrefDB::kFTPFolder_Depth,inIndex);
			//メニュー項目表示文字列作成
			AText	menutext;
			GetFTPFolderFTPURL(inIndex,menutext);
			AText	numtext;
			numtext.SetFormattedCstring("%d",depth);
			AText	depthtext;
			depthtext.SetLocalizedText("FTPFolderDepth");
			depthtext.ReplaceParamText('0',numtext);
			menutext.AddText(depthtext);
			//メニューアクション文字列作成
			AText	actiontext;
			GetFTPFolderFTPURL(inIndex,actiontext);
			actiontext.Insert1(0,'0'+depth);
			actiontext.Insert1(0,'R');
			//メニュー設定
			NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_FTPFolder,inIndex,menutext,actiontext,kObjectID_Invalid,0,0,false);
			break;
		}
		//項目削除
	  case kModificationType_RowDeleted:
		{
			//項目削除
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
	//FTP接続スレッド使用中は何もしない
	if( SPI_GetFTPConnection().NVI_IsThreadWorking() == true )   return;
	
	if( inActionText.GetItemCount() == 0 )   return;
	
	if( inActionText.Get(0) == 'R' && inActionText.GetItemCount() >= 2 )
	{
		//メニューのルートを選択（FTPフォルダ読み込み）
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
		//削除しなかったらメモリリーク？
		DoFTPFolderMenuSelected(text);
	}
	else
	{
		//ルートではない項目（ファイル）を選択した場合、そのurlのファイルを開く
		//#361 SPI_StartOpenFTPDocument(inActionText,false);
		SPI_RequestOpenFileToRemoteConnection(inActionText,false);
	}
}
*/
#pragma mark ===========================

#pragma mark ---フォルダリストメニュー管理

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
		//対象外ファイル
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
			//項目削除
			SPI_UpdateFolderListMenu(kModificationType_RowDeleted,inIndex);
			//項目追加
			SPI_UpdateFolderListMenu(kModificationType_RowAdded,inIndex);
			break;
		}
	  case kModificationType_RowAdded:
		{
			//項目追加
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
			//項目削除
			NVI_GetMenuManager().DeleteDynamicMenuItemByMenuItemID(kMenuItemID_FolderList,inIndex);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark ---ツールメニュー

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

#pragma mark ---設定関連情報取得

//InlineInputハイライト下線色情報取得
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

#pragma mark --- テキストファイル読み込み

/**
ファイル、もしくは、ドキュメントからテキストを読み込み、UTF8へ変換する。
@note thread-safe
*/
void	AApp::SPI_LoadTextFromFileOrDocument( const ALoadTextPurposeType inLoadTextPurposeType,
											 const AFileAcc& inFile, AText& outText, AModeIndex& outModeIndex,
											 AText& outTextEncoding )//B0313
{
	//#898 ABool	utf8ok = true;//B0313
	//エイリアス解決
	AFileAcc	file;
	file.ResolveAnyLink(inFile);
	//Text読み込み
	ABool	read = false;//#831 読み込み済みフラグ初期化
	AObjectID	openedDocID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
	if( openedDocID != kObjectID_Invalid )
	{
		//ファイルに対応するドキュメントが存在する場合
		
		//documentオブジェクトを取得し、retainする（スレッドからもコールされるため、retain中にdocumentオブジェクト解放されないようにする）
		ADocument*	doc = GetApp().NVI_GetDocumentAndRetain(openedDocID);
		if( doc != NULL )
		{
			//documentオブジェクトのretainを必ず解放するためのスタッククラス
			AStDecrementRetainCounter	releaser(doc);
			
			ADocument_Text&	textdoc = reinterpret_cast<ADocument_Text&>(*doc);
			//ドキュメントがファイル読み込み済みならドキュメントからコピーする #831（ドキュメントにファイル未読み込みの場合がありえるようになったため条件追加）
			if( textdoc.SPI_IsLoaded() == true )
			{
				textdoc.SPI_GetText(outText);
				outModeIndex = textdoc.SPI_GetModeIndex();
				textdoc.SPI_GetTextEncoding(outTextEncoding);
				read = true;//#831 読み込み済みフラグON
			}
		}
	}
	//else
	//#831 読み込み済みフラグOFFのままなら、ファイルから読み込み
	if( read == false )
	{
		SPI_LoadTextFromFile(inLoadTextPurposeType,file,outText,outModeIndex,outTextEncoding);
	}
	//#898 return utf8ok;//B0313
}

//#723 SPI_GetTextFromFileOrDocument()から分離。Documentから読む必要が無い場合、こちらを使用する。（スレッドからのDocument参照をなるべくしないようにする）
/**
ファイルからテキストを読み込み、UTF8へ変換する。
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
	//==================リンク解決==================
	AFileAcc	file;
	file.ResolveAnyLink(inFile);
	
	//==================ファイルからテキスト読み込み==================
	file.ReadTo(outText);
	
	//==================DocPrefをファイルから読み込み==================
	ADocPrefDB	docPrefDB;
	AText	textfilepath;//#898
	file.GetPath(textfilepath);//#898
	//doc pref DBから読み込み、モードindex取得
	ABool	loadedFromFile = false;//#1429
	outModeIndex = docPrefDB.LoadPref(textfilepath, loadedFromFile);//#898 #1429
	
	//==================UTF8CRへ変換==================
	//doc prefを使ってUTF8/CRへ変換
	ATextEncodingFallbackType	fallbackResult = kTextEncodingFallbackType_None;
	ATextEncoding	resultTec = ATextEncodingWrapper::GetUTF8TextEncoding();
	AReturnCode	resultReturnCode = returnCode_CR;
	ABool	notMixed = true;//#995
	SPI_ConvertToUTF8CRUsingDocPref(inLoadTextPurposeType,outText,docPrefDB,textfilepath,resultTec,resultReturnCode,fallbackResult,notMixed);
	ATextEncodingWrapper::GetTextEncodingName(resultTec,outTextEncodingName);
	
	
#if 0
	//ドキュメントオープン中でなければ、ファイルから読み込み
	//まず、DocPrefDBからTextEncodingを読み込み
	AText	tecname;
	docPrefDB.GetData_Text(ADocPrefDB::kTextEncodingName,tecname);
	ATextEncoding	tec;
	ATextEncodingWrapper::GetTextEncodingFromName(tecname,tec);
	//docPrefで未指定なら自動認識
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
		//DocPrefをファイルから読み込んでいない場合（＝doc prefファイルが無かった場合や新規ファイル）、推測を行う
		//推測確定しない場合は、DocPrefに設定した値（＝ModePrefに設定されているデフォルト値）を使用する
		if( outText.GuessTextEncoding(true,true,true,false,tec) == false )
		{
		}
	}
	//★テスト #829
	//tec = ATextEncodingWrapper::GetJISTextEncoding();
	//UTF8へ変換
	if( outText.GetItemCount() < 1024*1024*16 )//B0313 16MB以上のファイルはテキストでない可能性が高いので無変換（変換すると負荷重くなりすぎ）
	{
		//#307 ATextEncoding	resulttec;
		//B0400 utf8ok = AUtil::ConvertToUTF8CRAnyway(outText,tec,resulttec);
		utf8ok = NVI_ConvertToUTF8CRAnyway(outText,tec,true);//B0400 #182 #307
	}
	//
	ATextEncodingWrapper::GetTextEncodingName(tec,outTextEncodingName);
	/*#898
	//モード決定
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
doc prefを使ってUTF8/CRへ変換
@param inFile リモートファイル等の場合、未specifyでも良い。specifyされていればEAを使ったTEC推測に使用する。
@note thread-safe
*/
ABool	AApp::SPI_ConvertToUTF8CRUsingDocPref( const ALoadTextPurposeType inLoadTextPurposeType,
											  AText& ioText, const ADocPrefDB& inDocPrefDB, const AText& inFilePath,
											  ATextEncoding& outResultEncoding,
											  AReturnCode& outResultReturnCode,
											   ATextEncodingFallbackType& outResultFallbackType,
											   ABool& outNotMixed ) const
{
	//==================フォールバック、SJISロスレスフォールバックの使用可否判定==================
	ABool	useFallback = NVI_GetAppPrefDBConst().GetData_Bool(AAppPrefDB::kUseFallbackForTextEncodingConversion);//#830
	ABool	useLosslessFallback = false;//#844 SJISロスレスフォールバックはdrop NVI_GetAppPrefDBConst().GetData_Bool(AAppPrefDB::kUseSJISLosslessFallback);
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
			//処理なし
			break;
		}
	}
	
	//DocPrefをファイルから読み込んでいない場合：mode prefのtecの値が格納されている
	//DocPrefをファイルから読み込んだ場合：ファイルに記憶されたtecの値が格納されている
	//==================(II) DocPref指定（ModePrefによるデフォルト値ではない）によるテキストエンコーディング==================
	//Cocoa用設定が設定されていればそちらを優先。そうでなければ、Carbon用設定を読み込む。 #1040
	AText	tecname;
	inDocPrefDB.GetData_Text(ADocPrefDB::kTextEncodingNameCocoa,tecname);
	if( tecname.GetItemCount() == 0 ) 
	{
		inDocPrefDB.GetData_Text(ADocPrefDB::kTextEncodingName,tecname);
	}
	ATextEncoding	tec = ATextEncodingWrapper::GetTextEncodingFromName(tecname);
	//Doc Prefのテキストエンコーディングの値が（モード設定から読み込んだ値ではなく）ファイルから読み込んだ値などである場合は、
	//doc prefの設定値を使用する。
	if( inDocPrefDB.IsTextEncodingFixed() == true )
	{
		//エラー率が高すぎる(20%以上)ときは、doc prefの間違いの可能性が高いので、Doc prefは使わず、(III)、(IV)を適用する
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
			//------------------テキストエンコーディング変換実行（変換エラーでなければ、ここで終了）------------------
			if( SPI_ConvertToUTF8CR(ioText,tec,true,
									useFallback,useLosslessFallback,
									(AReturnCode)(inDocPrefDB.GetData_Number(ADocPrefDB::kReturnCode)),
									outResultEncoding,outResultReturnCode,outResultFallbackType,outNotMixed) == true )
			{
				return true;
			}
		}
	}
	
	//==================(III)推測によるテキストエンコーディング==================
	if( NVI_GetAppPrefDBConst().GetData_Bool(AAppPrefDB::kUseGuessTextEncoding) == true )//#764
	{
		if( GuessTextEncoding(ioText,inFilePath,tec) == true )
		{
			//------------------テキストエンコーディング変換実行（変換エラーでなければ、ここで終了）------------------
			if( SPI_ConvertToUTF8CR(ioText,tec,true,
									useFallback,useLosslessFallback,//常にフォールバックを使用しない案もあったが、そうすると、エラー文字があるばあいに必ず推測失敗してしまう。（エラー率閾値をもうけた意味がなくなる）
									(AReturnCode)(inDocPrefDB.GetData_Number(ADocPrefDB::kReturnCode)),
									outResultEncoding,outResultReturnCode,outResultFallbackType,outNotMixed) == true )
			{
				return true;
			}
		}
	}
	
	//==================(IV)モード設定によるテキストエンコーディング==================
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
テキストエンコーディングの推測
@note thread-safe
*/
ABool	AApp::GuessTextEncoding( const AText& inText, const AText& inFilePath, ATextEncoding& ioTextEncoding ) const
{
	/*目に見えず分かりづらいのでひとまず使用しないことにする。対応するなら、最優先ではなく、最後にすべきか？
#if IMPLEMENTATION_FOR_MACOSX
	//Mac OS X 拡張属性にテキストエンコーディングが設定されていれば、それを推測結果として最優先で使用する。
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
	//DocPrefをファイルから読み込み
	ADocPrefDB	docPrefDB;
	AText	textfilepath;//#898
	inFile.GetPath(textfilepath);//#898
	docPrefDB.LoadPref(textfilepath);//#898
	//まず、DocPrefDBからTextEncodingを読み込み
	AText	tecname;
	docPrefDB.GetData_Text(ADocPrefDB::kTextEncodingName,tecname);
#if IMPLEMENTATION_FOR_MACOSX
	//R0230 拡張属性から読み込む（拡張属性優先）
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
	//ファイルからテキスト読み込み
	inFile.ReadTo(outText);
	//docPrefで未指定なら自動認識
	if( docPrefDB.IsStillDefault(ADocPrefDB::kTextEncodingName) == true )
	{
		tec = ATextEncodingWrapper::GetUTF8TextEncoding();//B0390
		if( outText.GuessTextEncoding(true,true,true,false,tec) == false )//B0390
		{
			tec = ATextEncodingWrapper::GetSJISTextEncoding();
		}
	}
	//UTF8へ変換
	//#307 ATextEncoding	resulttec;
	//B0400 AUtil::ConvertToUTF8CRAnyway(outText,tec,resulttec);
	NVI_ConvertToUTF8CRAnyway(outText,tec,true);//B0400 #182 #307
}
#endif

//B0310
/**
テキストファイル（UTF8/CR）書き込み
*/
void	AApp::SPI_WriteTextFile( const AText& inText, const AFileAcc& inFile )
{
	//ファイル書き込み
	inFile.WriteFile(inText);
	//doc pref更新（テキストエンコーディングをUTF8に設定）
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
内部テキスト(UTF8,CR)へ変換する
@return 変換出来なかったときはfalseとなる。fallbackを使っても変換できたときはtrueとなる。
*/
ABool	AApp::SPI_ConvertToUTF8CR( AText& ioText, const ATextEncoding& inTextEncoding, const ABool inFromExternalData,
								   const ABool inUseFallback, const ABool inUseLosslessFallback,
								   const AReturnCode inDefaultReturnCode,//テキスト内に改行コードが無かった場合のデフォルト改行コード
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
	
	/*#844 SJISロスレスフォールバックはdrop（通常フォールバックでも開くことはできるので）
	//==================１．SJISロスレスフォールバック変換==================
	//#473
	//ロスレスフォールバック変換を試みる
	if( inTextEncoding == ATextEncodingWrapper::GetSJISTextEncoding() && inUseLosslessFallback == true )
	{
		if( ConvertFromSJISToUTF8UsingLosslessFallback(ioText,inFromExternalData,inUseFallback) == true )
		{
			//異常文字が存在し、かつ、フォールバック変換成功
			outResultEncoding = inTextEncoding;//#473
			outResultReturnCode = ioText.ConvertReturnCodeToCR(inDefaultReturnCode);//#307
			outResultFallbackType = kTextEncodingFallbackType_Lossless;//#473
			return true;
		}
	}
	*/
	
	//==================２．通常変換==================
	//通常変換を試みる
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
	
	//============３．元のtextのまま============
	ioText.SetText(origText);
	outResultEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
	outResultReturnCode = ioText.ConvertReturnCodeToCR(inDefaultReturnCode,outNotMixed);//#995
	return false;
}

//#844 SJISロスレスフォールバックはdrop
#if 0
//#473
/**
SJISロスレスフォールバック変換
@return 異常文字が存在し、かつ、フォールバック変換成功したらtrue（falseの場合はioTextに変化無し）
*/
ABool	AApp::ConvertFromSJISToUTF8UsingLosslessFallback( AText& ioText, const ABool inFromExternalData, const ABool inUseFallback ) const
{
	AText	text;
	//SJISテキスト内の異常文字検索
	ABool	abnormalCharExist = false;
	AArray<AUChar>	escape01Array;
	AItemCount	len = ioText.GetItemCount();
	for( AIndex pos = 0; pos < len; pos++ )
	{
		//1バイト目取得
		AUChar	ch = ioText.Get(pos);
		if( ch == 0x01 )
		{
			//0x01格納
			escape01Array.Add(0x01);
			text.Add(0x01);
		}
		else if( ((ch>=0x81&&ch<=0x9F)||(ch>=0xE0&&ch<=0xFC)) )
		{
			//2バイト目取得
			AUChar	ch2 = 0;
			if( pos+1 < len )
			{
				ch2 = ioText.Get(pos+1);
			}
			if( ((ch2>=0x40&&ch2<=0x7E)||(ch2>=0x80&&ch2<=0xFC)) )
			{
				//正常SJIS２バイト文字
				text.Add(ch);
				text.Add(ch2);
				pos++;
				continue;
			}
			else
			{
				//異常（2バイト目が規格外）
				escape01Array.Add(ch);
				text.Add(0x01);//0x01格納
				abnormalCharExist = true;
				continue;
			}
		}
		else
		{
			//１バイト文字
			text.Add(ch);
			continue;
		}
	}
	//異常文字が存在しなければreturn
	if( abnormalCharExist == false )   return false;
	
	//UTF8へ変換
	ABool	fallback = false;
	if( text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding(),inUseFallback,fallback,inFromExternalData) == true )
	{
		//#764 if( text.CheckUTF8Text() == true )
		{
			//ioText全削除
			ioText.DeleteAll();
			//変換後のUTF8テキストの0x01を検索して、escape01Arrayに従って、文字変換する
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
						//0x01のまま追加
						ioText.Add(0x01);
					}
					else
					{
						//私用面 U+105CXXに変換して追加
						ioText.Add(0xF4);
						ioText.Add(0x85);
						ioText.Add(0xB0+((escapeCh&0xC0)>>6));
						ioText.Add(0x80+((escapeCh&0x3F)));
					}
				}
				else
				{
					//変換せず追加
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
ModeIndexをファイルから取得する（DocPrefがあればそれを優先する）
*/
AModeIndex	AApp::SPI_GetModeIndexFromFile( const AFileAcc& inFile ) const
{
	//DocPrefをファイルから読み込み
	ADocPrefDB	docPrefDB;
	AText	textfilepath;
	inFile.GetPath(textfilepath);
	ABool	loadedFromFile = false;//#1429
	return docPrefDB.LoadPref(textfilepath, loadedFromFile);//#1429
}

#pragma mark ===========================

#pragma mark --- SCM
//R0006

//SCMManagerにフォルダ未登録なら登録する
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

//SCMFolderDataがupdateされたときにコールされる
//ファイルリスト等の表示更新
void	AApp::SPI_SCMFolderDataUpdated( const AFileAcc& inFolder )
{
	SPI_UpdateSCMDisplayAll();
}

void	AApp::SPI_UpdateSCMDisplayAll()
{
	/*#725 InfoHeader廃止
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObjectConst(index).NVI_GetWindowType() == kWindowType_Text )
		{
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_SetInfoHeaderText(GetEmptyText());
		}
	}
	*/
	//タイトルバーのSVN状態更新 #725
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

#pragma mark --- テンポラリテキスト保存

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

#pragma mark --- 更新反映

//#291
/**
JumpList設定データ更新
*/
/*#725
void	AApp::SPI_UpdatePropertyJumpListAll()
{
	//JumpListウインドウの透明度更新
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_JumpList )
		{
			mWindowArray.GetObject(index).NVI_UpdateProperty();
		}
	}
	//JumpList Viewのフォント更新
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
指定WindowID以外のJumpListを全てHideにする
*/
/*#725 SPI_HideOtherFloatingJumpListWindows()に変更
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

#pragma mark --- ジャンプリスト履歴
//#454

/**
ジャンプリスト履歴更新
*/
void	AApp::SPI_UpdateJumpListHistory( const AText& inSelectionText, const AFileAcc& inDocFile )
{
	//履歴追加禁止なら何もしない
	if( GetAppPref().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )   return;
	
	//
	mJumpListHistory.Insert1(0,inSelectionText);
	AText	path;
	inDocFile.GetPath(path);
	path.Add(kUChar_LineEnd);
	path.AddText(inSelectionText);
	mJumpListHistory_Path.Insert1(0,path);
	//同じ項目があったら削除
	for( AIndex i = 1; i < mJumpListHistory_Path.GetItemCount(); i++ )
	{
		if( mJumpListHistory_Path.GetTextConst(i).Compare(path) == true )
		{
			mJumpListHistory.Delete1(i);
			mJumpListHistory_Path.Delete1(i);
			break;
		}
	}
	//履歴maxより多ければ一番最後のを削除
	if( mJumpListHistory.GetItemCount() >= kJumpListHistoryMax )
	{
		mJumpListHistory.Delete1(mJumpListHistory.GetItemCount()-1);
		mJumpListHistory_Path.Delete1(mJumpListHistory_Path.GetItemCount()-1);
	}
	//メニュー更新
	GetApp().NVI_GetMenuManager().SetContextMenuTextArray(kContextMenuID_JumpListHistory,mJumpListHistory,mJumpListHistory_Path);
}

#pragma mark ===========================

#pragma mark --- 検索パラメータ
//#693

/**
検索パラメータ取得
*/
void	AApp::SPI_GetFindParam( AFindParameter& outParam ) const
{
	outParam.Set(mFindParam);
}

/**
検索パラメータ設定
*/
void	AApp::SPI_SetFindParam( const AFindParameter& inFindParam )
{
	//検索パラメータ設定
	mFindParam.Set(inFindParam);
	//検索文字列長制限
	mFindParam.findText.LimitLength(0,kLimit_FindTextLength);
	//ウインドウへ反映
	if( mFindWindowID != kObjectID_Invalid )
	{
		SPI_GetFindWindow().SPI_UpdateUI_CurrentFindParam();
	}
}

/**
検索パラメータ（検索文字列のみ）設定
*/
void	AApp::SPI_SetFindText( const AText& inText )
{
	//#928
	//コマンド＋E時に正規表現検索することはないはずなので、正規表現解除する
	mFindParam.regExp = false;
	//検索文字列設定
	mFindParam.findText.SetText(inText);
	//検索文字列長制限
	mFindParam.findText.LimitLength(0,kLimit_FindTextLength);
	//ウインドウへ反映
	if( mFindWindowID != kObjectID_Invalid )
	{
		SPI_GetFindWindow().SPI_UpdateUI_CurrentFindParam();
	}
}

/**
検索パラメータ（置換文字列のみ）設定
*/
void	AApp::SPI_SetReplaceText( const AText& inText )
{
	//置換文字列設定
	mFindParam.replaceText.SetText(inText);
	//ウインドウへ反映
	if( mFindWindowID != kObjectID_Invalid )
	{
		SPI_GetFindWindow().SPI_UpdateUI_CurrentFindParam();
	}
}

/**
検索パラメータ初期化
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
指定find parameterに対応する検索ツールコマンドを取得する
*/
void	AApp::SPI_GetFindParamToolCommand( const AFindParameter& inParam, AText& outText, const char* inCommand ) const
{
	//現在の検索パラメータをツールコマンド検索オプションに変換する
	AText	optiontext;
	GetToolCommandFindOptionFromFindParameter(inParam,false,optiontext);
	
	//ツールコマンド生成
	
	//コマンド行
	outText.SetCstring("<<<");
	outText.AddCstring(inCommand);
	outText.AddCstring("(");
	outText.AddText(optiontext);
	outText.AddCstring(")\r");
	
	//検索文字列の行
	AText	findtext;
	findtext.SetText(inParam.findText);
	//検索テキストの、改行コードとバックスラッシュをエスケープする。
	findtext.ConvertToEscapedText(true,false);
	//検索文字列追加
	outText.AddText(findtext);
	
	//最終行
	outText.AddCstring("\r>>>");
}

/**
指定find parameterに対応する置換ツールコマンドを取得する
*/
void	AApp::SPI_GetReplaceParamToolCommand( const AFindParameter& inParam, const AReplaceRangeType inReplaceRange, AText& outText ) const
{
	//現在の検索パラメータをツールコマンド検索オプションに変換する
	AText	optiontext;
	AApp::GetToolCommandFindOptionFromFindParameter(inParam,false,optiontext);
	
	//ツールコマンド生成
	
	//コマンド行
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
			//処理なし
			break;
		}
	}
	outText.AddText(optiontext);
	outText.AddCstring(")\r");
	
	//検索文字列の行
	AText	findtext;
	findtext.SetText(inParam.findText);
	//検索テキストの、改行コードとバックスラッシュをエスケープする。
	findtext.ConvertToEscapedText(true,false);
	//検索文字列追加
	outText.AddText(findtext);
	outText.AddCstring("\r");
	
	//置換文字列の行
	AText	replacetext;
	replacetext.SetText(inParam.replaceText);
	//検索テキストの、改行コードとバックスラッシュをエスケープする。
	replacetext.ConvertToEscapedText(true,false);
	//置換文字列追加
	outText.AddText(replacetext);
	
	//最終行
	outText.AddCstring("\r>>>");
}

//#887
/**
FindParameterから、ツールコマンドの検索オプション用テキストを生成する
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
最近使った検索文字列に追加
*/
void	AApp::SPI_AddRecentlyUsedFindText( const AFindParameter& inParam )
{
	SPI_GetFindWindow().SPI_AddRecentlyUsedFindText(inParam);
}

/**
最近使った置換文字列に追加
*/
void	AApp::SPI_AddRecentlyUsedReplaceText( const AFindParameter& inParam )
{
	SPI_GetFindWindow().SPI_AddRecentlyUsedReplaceText(inParam);
}

#pragma mark ===========================

#pragma mark --- リモート接続
//#361

/**
リモート接続オブジェクト生成
pparam inType タイプ：ファイルリストからのLIST要求 or ファイルオープン要求
@param inRequestorObjectID ファイルリストからのLIST要求の場合：ファイルリストウインドウobject　ファイルオープン要求：invalid
*/
AObjectID	AApp::SPI_CreateRemoteConnection( const ARemoteConnectionType inType, const AObjectID inRequestorObjectID )
{
	ARemoteConnectionFactory	factory(inType,inRequestorObjectID);
	AIndex	index = mRemoteConnectionArray.AddNewObject(factory);
	return mRemoteConnectionArray.GetObjectConst(index).GetObjectID();
}

/**
リモート接続オブジェクト削除
*/
void	AApp::SPI_DeleteRemoteConnection( const AObjectID inRemoteConnectionObjectID )
{
	AIndex	index = mRemoteConnectionArray.GetIndexByID(inRemoteConnectionObjectID);
	mRemoteConnectionArray.Delete1Object(index);
}

/**
リモートファイルURLを開いているドキュメントを検索
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
URLからリモート接続オブジェクトを検索する（ファイルオープン中はTextDocumentが未生成なのでSPI_GetTextDocumentIDByRemoteFileURL()で見つからないので、こちらで検索する必要がある。）
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
URLから使用するアクセスプラグインを検索する
*/
/*#1231
AIndex	AApp::FindAccessPluginIndexFromURL( const AText& inURL )
{
	//アカウントごとのループ
	AItemCount	itemCount = GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kFTPAccount_ConnectionType);
	for( AIndex index = 0; index < itemCount; index++ )
	{
		//コネクションタイプからプロトコル取得
		AText	connectionType;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_ConnectionType,index,connectionType);
		AIndex	foundpos = 0;
		if( connectionType.FindCstring(0,"(",foundpos) == true )
		{
			connectionType.DeleteAfter(foundpos);
		}
		connectionType.ChangeCaseLower();
		//アカウントの、サーバー、ユーザー、パスデータ取得
		AText	server, user, path;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_Server,index,server);
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_User,index,user);
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_RootPath,index,path);
		//URL取得
		AText	url;
		url.SetFTPURL(connectionType,server,user,path,0);
		//プロトコル部分を比較して、同じならindexを返す
		if( inURL.CompareMin(url) == true )
		{
			return index;
		}
	}
	return kIndex_Invalid;
}
*/

/**
リモート接続へファイルオープン要求（リモート接続オブジェクト経由でスレッドへGET要求）
*/
void	AApp::SPI_RequestOpenFileToRemoteConnection( const AText& inRemoteFileURL, const ABool inAllowNoFile )
{
	/* #1231
	AText	connectionType;
	//URLから対応するコネクションタイプを取得
	AIndex	index = FindAccessPluginIndexFromURL(inRemoteFileURL);
	if( index != kIndex_Invalid )
	{
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_ConnectionType,index,connectionType);
	}
	//リモートファイルを開く
	SPI_RequestOpenFileToRemoteConnection(connectionType,inRemoteFileURL,inAllowNoFile);
}
void	AApp::SPI_RequestOpenFileToRemoteConnection( const AText& inConnectionType, const AText& inRemoteFileURL, const ABool inAllowNoFile )
{
*/
	//リモートファイルURLに対応するドキュメントを検索
	ADocumentID	docID = SPI_GetTextDocumentIDByRemoteFileURL(inRemoteFileURL);
	if( docID != kObjectID_Invalid )
	{
		//リモートファイルをドキュメントに開いていれば、そのドキュメントをrevealする
		SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();
	}
	else
	{
		//#1228
		//URLからリモート接続オブジェクトを検索して、存在していれば何もせず終了。
		//ファイルオープン中はTextDocumentが未生成なのでSPI_GetTextDocumentIDByRemoteFileURL()で見つからないので。）
		if( SPI_FindRemoteConnectionByRemoteURL(inRemoteFileURL) != kObjectID_Invalid )
		{
			return;
		}
		
		//リモート接続オブジェクト生成
		AObjectID	remoteConnectionObjectID = SPI_CreateRemoteConnection(kRemoteConnectionType_TextDocument,kObjectID_Invalid);
		//接続、GET要求
		//#1231 SPI_GetRemoteConnection(remoteConnectionObjectID).SetConnectionType(inConnectionType);
		SPI_GetRemoteConnection(remoteConnectionObjectID).ExecuteGET(kRemoteConnectionRequestType_GETforOpenDocument,
																	 inRemoteFileURL,inAllowNoFile);
	}
}

/**
リモート接続オブジェクトからの応答受信時処理
*/
void	AApp::SPI_DoAccessPluginResponseReceived( const AObjectID inRemoteConnectionObjectID, const AText& inText, const AText& inStderrText )
{
	if( mRemoteConnectionArray.ExistObject(inRemoteConnectionObjectID) == true )
	{
		SPI_GetRemoteConnection(inRemoteConnectionObjectID).DoResponseReceived(inText,inStderrText);//#1231
	}
}

/**
リモートファイルドキュメント生成（GET応答受信時に、リモート接続オブジェクトからコールされる）
*/
void	AApp::SPNVI_CreateDocumentFromRemoteFile( const AObjectID inRemoteConnectionObjectID, 
		const AText& inRemoteFileURL, const AText& inDocumentText )
{
#if IMPLEMENTATION_FOR_WINDOWS
	if( CAppImp::SystemResourceCheck_GDIObject() == false )   return;
#endif
	
	//DocImp生成 #1034
	AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
	//ドキュメント生成（リモート接続オブジェクトをドキュメントに渡す）
	ATextDocumentFactory	factory(this,docImpObjectID);//#1034
	AObjectID	docID = NVI_CreateDocument(factory);
	SPI_GetTextDocumentByID(docID).SPI_Init_RemoteFile(inRemoteConnectionObjectID,inRemoteFileURL,inDocumentText);
	//#379 ウインドウ生成
	SPI_CreateWindowOrTabForNewTextDocument(docID,kIndex_Invalid,kIndex_Invalid,true);
	//最近開いたファイルに追加
	SPI_AddToRecentlyOpenedFile(inRemoteFileURL);//#235
}

/**
リモート接続用プラグインパス取得（"sftp"等のタイプ名から、プラグインのファイルパスを取得）
*/
ABool	AApp::SPI_GetPluginPath( const AText& inConnectionTypeName, AText& outPluginPath )
{
	outPluginPath.DeleteAll();
	if( inConnectionTypeName.GetItemCount() == 0 )
	{
		return true;//（レガシーFTP）
	}
	/*★未対応
	//プラグインのファイルパスを取得
	{
		AFileAcc	folder;
		//ユーザー定義アクセスプラグインフォルダ
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
	//アプリ内アクセスプラグインフォルダ
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

#pragma mark <所有クラス(ADocument_Text)生成／削除／取得>

#pragma mark ===========================

//新規テキストドキュメント生成
AObjectID	AApp::SPNVI_CreateNewTextDocument( const AModeIndex inModeIndex, const AText& inTextEncodingName, 
			const AText& inDocumentText, const ABool inSelect,
			const AWindowID inCreateTabInWindowID, const ABool inAlwaysCreateWindow,
			const ARect inInitialWindowBounds )
{
#if IMPLEMENTATION_FOR_WINDOWS
	if( CAppImp::SystemResourceCheck_GDIObject() == false )   return kObjectID_Invalid;
#endif
	
	//DocImp生成 #1034
	AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
	//ドキュメント生成
	ATextDocumentFactory	factory(this,docImpObjectID);//#1034
	ADocumentID	docID = NVI_CreateDocument(factory);
	SPI_GetTextDocumentByID(docID).SPI_Init_NewFile(inModeIndex,inTextEncodingName,inDocumentText);
	//#379 ウインドウ生成（ドキュメント内ではウインドウ・タブは生成しない。ドキュメントはあくまでデータのみとする。）
	SPI_CreateWindowOrTabForNewTextDocument(docID,kIndex_Invalid,kIndex_Invalid,inSelect,true,inCreateTabInWindowID,inAlwaysCreateWindow,inInitialWindowBounds);
	return docID;
}

//#567 #1170
/**
LuaConsoleドキュメント生成
*/
AObjectID	AApp::SPNVI_CreateLuaConsoleDocument()
{
#if IMPLEMENTATION_FOR_WINDOWS
	if( CAppImp::SystemResourceCheck_GDIObject() == false )   return kObjectID_Invalid;
#endif
	
    //DocImp生成 #1034
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
	//#379 ウインドウ生成（ドキュメント内ではウインドウ・タブは生成しない。ドキュメントはあくまでデータのみとする。）
	SPI_CreateWindowOrTabForNewTextDocument(docID,kIndex_Invalid,kIndex_Invalid,false,true,kObjectID_Invalid,true);
	SPI_GetTextDocumentByID(docID).SPI_Lua_Prompt();
	return docID;
}

//#379
/**
読み込み専用ドキュメント生成（Diffドキュメントなど用）
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
	
	//DocImp生成 #1034
	AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
	//ドキュメント生成
	ATextDocumentFactory	factory(this,docImpObjectID);//#1034
	ADocumentID	docID = NVI_CreateDocument(factory);
	SPI_GetTextDocumentByID(docID).SPI_Init_DiffTargetDocument(inTitle,inDocumentText,inModeIndex,inTextEncodingName);
	//#379 ウインドウ生成（ドキュメント内ではウインドウ・タブは生成しない。ドキュメントはあくまでデータのみとする。）
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
	
	//DocImp生成 #1034
	AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
	//ドキュメント生成
	ATextDocumentFactory	factory(this,docImpObjectID);//#1034
	ADocumentID	docID = NVI_CreateDocument(factory);
	SPI_GetTextDocumentByID(docID).SPI_Init_DiffTargetDocument(inTitle,inFile,inModeIndex,inTextEncodingName);
	//#379 ウインドウ生成（ドキュメント内ではウインドウ・タブは生成しない。ドキュメントはあくまでデータのみとする。）
	SPI_CreateWindowOrTabForNewTextDocument(docID,inInitialSelectStart,inInitialSelectEnd,inSelect,inSelectableTab,inCreateTabInWindowID);
	return docID;
}

//#856
/**
既に生成済みのドキュメントに対し、新たなウインドウを生成
*/
void	AApp::SPI_CreateNewWindowForExistingTextDocument( const ADocumentID inTextDocumentID,
			const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd )
{
	SPI_CreateWindowOrTabForNewTextDocument(inTextDocumentID,inInitialSelectStart,inInitialSelectEnd,true,
							true,kObjectID_Invalid,true);
}

//ローカルファイルからドキュメント生成
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
	/*#932 エイリアス解決、ファイル存在チェック、フォルダチェックは、ドキュメント生成した後にスレッドで行う
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
			
			//タブモードではない場合は先に自動新規生成ドキュメントを閉じる #1056
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == false )
			{
				CloseAutomaticallyCreatedDocument(firstDocID);
			}
			
			AObjectID	docID = NVI_GetDocumentIDByFile(kDocumentType_Text,file);
			if( docID == kObjectID_Invalid )
			{
				/*#932 ファイルサイズチェックは、ドキュメント生成した後にスレッドで行う。
				//ファイルサイズが100MBを超えるファイルの場合は、モーダルエラー表示して、ファイルを開かない
				AByteCount	filesize = file.GetFileSize();
				if( filesize > kLimit_OpenTextFileSize )
				{
					AText	message1, message2;
					message1.SetLocalizedText("CantOpen_Title");
					message2.SetLocalizedText("CantOpen_FileSize");
					NVI_ShowModalAlertWindow(message1,message2);
					break;
				}
				//トータルのファイルサイズが200MBを超える場合は、モーダルエラー表示して、ファイルを開かない
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
				//DocImp生成 #1034
				AObjectID	docImpObjectID = inDocImpID;
				if( docImpObjectID == kObjectID_Invalid )
				{
					docImpObjectID = SPI_CreateDocImp(kDocumentType_Text);
				}
				//ローカルファイルからテキストドキュメント生成
				ATextDocumentFactory	factory(this,docImpObjectID);//#1034
				docID = NVI_CreateDocument(factory);
				SPI_GetTextDocumentByID(docID).SPI_Init_LocalFile(file,inTextEncodingName);//win 080715 #212
				//#379 ウインドウ生成
				SPI_CreateWindowOrTabForNewTextDocument(docID,inInitialSelectStart,inInitialSelectEnd,inSelect,true,
							inCreateTabInWindowID,inAlwaysCreateWindow,
							inInitialWindowBounds,inSetInitialSideBarDisplay,inLeftSideBar,inRightSideBar,inSubText);
				//最近開いたファイルへ追加
				SPI_AddToRecentlyOpenedFile(inFile);
				//R0006
				//#932 同じフォルダのデータ更新時（同じフォルダローダスレッド終了後）に登録するのでここは不要 SCMRegisterFolderByFile(inFile);
			}
			else
			{
				//ファイルに対応するドキュメントが既に存在している場合はそのウインドウを選択する。
				SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();//#92 SPI_SelectFirstWindow();
			}
			mLastOpenedTextFile.CopyFrom(inFile);
			result = docID;
			
			//タブモードの場合は後で自動新規生成ドキュメントを閉じる
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
				//ファイル読み込み
				SPI_GetIndexWindowDocumentByID(docID).Load(file);//#92 Init(file);
				
				//Window生成 #92 Document内ではWindowを生成しないように変更
				AIndexWindowFactory	winfactory(docID);
				AWindowID	winID = GetApp().NVI_CreateWindow(winfactory);
				NVI_GetWindowByID(winID).NVI_Create(docID);
				NVI_GetWindowByID(winID).NVI_Show();
				
				//ウインドウメニュー更新
				SPI_UpdateWindowMenu();
				*/
				ADocumentID	docID = SPI_GetOrCreateFindResultWindowDocument();
				//ファイル読み込み
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
		/*#858 モード設定ファイルダブルクリックでのオープンは未対応にする
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
		//タブセット
	  case kDocumentType_TabSet:
		{
			OpenTabSet(file);
			//タブセット自体はドキュメントではないので、CDocImpオブジェクトがあれば、削除する。 #1078
			if( inDocImpID != kObjectID_Invalid )
			{
				AApplication::GetApplication().GetImp().DeleteDocImp(inDocImpID);
			}
			break;
		}
	  default:
		{
			//処理なし
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
	//#379 ウインドウ生成
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
	SPI_LoadTextFromFile(kLoadTextPurposeType_CreateDocumentFromTemplateFile,inFile,text,modeIndex,tecname);//#723 ドキュメントからテキスト取得する必要が無いのでSPI_GetTextFromFileOrDocument()から変更
	AObjectID	docID = SPNVI_CreateNewTextDocument(modeIndex,tecname);
	if( docID == kObjectID_Invalid )   return;//win
	SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
	SPI_GetTextDocumentByID(docID).NVI_SetDirty(false);
}

//#1034
/**
DocImp生成
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
ドキュメントの指定位置を開く、または、revealする
*/
ADocumentID	AApp::SPI_OpenOrRevealTextDocument( const AText& inFilePath, const AIndex inSpos, const AIndex inEpos, 
			const AWindowID inTextWindowID, const ABool inInAnotherView,
			const AAdjustScrollType inAdjustScrollType )
{
	//ファイルパスが空のときは何もしない
	if( inFilePath.GetItemCount() == 0 )
	{
		return kObjectID_Invalid;
	}
	//http://で始まる時はブラウザでURLを開く
	if( inFilePath.CompareMin("http://") == true )
	{
		ALaunchWrapper::OpenURLWithDefaultBrowser(inFilePath);
		return kObjectID_Invalid;
	}
	
	//------------------まずリモートファイルから検索------------------
	ADocumentID	docID = SPI_GetTextDocumentIDByRemoteFileURL(inFilePath);
	if( docID == kObjectID_Invalid )
	{
		//------------------ローカルファイルから検索------------------
		//ファイル取得
		AFileAcc	file;
		file.Specify(inFilePath);
		//ファイルからドキュメントを取得
		docID = NVI_GetDocumentIDByFile(kDocumentType_Text,file);
		//------------------未存在なら、ドキュメントを生成------------------
		//ドキュメント未生成なら、ドキュメントを生成。（タブを生成するテキストウインドウを指定）
		if( docID == kObjectID_Invalid )
		{
			docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,GetEmptyText(),kObjectID_Invalid,true,inSpos,inEpos,false,inTextWindowID);//#1090のついで inSelect=trueへ変更 →falseに戻す。下でSPI_RevealTextViewByDocumentID()により、revealされる。trueだと、コマンド＋infoクリックで、サブテキストとメインテキスト両方に表示されてしまう
		}
	}
	//------------------ドキュメント上の該当箇所を表示------------------
	if( docID != kObjectID_Invalid )
	{
		//表示先テキストウインドウ取得（未指定ならドキュメントの最初のウインドウにする）
		AWindowID	textWindowID = inTextWindowID;
		if( textWindowID == kObjectID_Invalid )
		{
			textWindowID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
		}
		//テキストウインドウ上に表示
		GetApp().SPI_GetTextWindowByID(textWindowID).SPI_RevealTextViewByDocumentID(docID,inSpos,inEpos,inInAnotherView,inAdjustScrollType);
	}
	/*#新規ウインドウで開く機能はdrop
	else
	{
		//ウインドウを常に生成する場合
		
		//ドキュメント生成済みなら、生成済みドキュメントに対し新規ウインドウ生成
		if( docID != kObjectID_Invalid )
		{
			SPI_CreateNewWindowForExistingTextDocument(docID,inSpos,inEpos);
		}
		//ドキュメント未生成なら、ドキュメントを新規ウインドウに生成。
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
テキストウインドウ・タブを生成
*/
AObjectID	AApp::SPI_CreateWindowOrTabForNewTextDocument( const ADocumentID inDocumentID, 
			const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd,
			const ABool inSelect, const ABool inSelectableTab,
			const AWindowID inCreateTabInWindowID, 
			const ABool inAlwaysCreateWindow, //#567
			const ARect inInitialWindowBounds,
			const ABool inSetInitialSideBarDisplay, const ABool inLeftSideBar, const ABool inRightSideBar, const ABool inSubText )
{
	////==============ウインドウ生成 or タブ生成の判定==============
	AWindowID	winID = kObjectID_Invalid;
	//#850 ABool	tabMode = NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow);
	ABool	createWindow = true;
	AWindowID	frontTextWindowID = NVI_GetMostFrontWindowID(kWindowType_Text);//#850
	if( NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true )
	{
		//タブ生成の場合、かつ、最前面テキストウインドウが存在している場合、ウインドウを生成せずタブを生成する
		//#850 if( SPI_GetTabModeMainTextWindowID() != kObjectID_Invalid )
		if( frontTextWindowID != kObjectID_Invalid )//#850
		{ 
			createWindow = false;
		}
	}
	//
	if( (createWindow == true && inCreateTabInWindowID == kObjectID_Invalid) || 
				inAlwaysCreateWindow == true )//引数inAlwaysCreateWindowがtrueなら上記の結果に関わらずウインドウ生成する #567
	{
		//==============ウインドウ生成の場合==============
		AWindowDefaultFactory<AWindow_Text>	factory;
		winID = NVI_CreateWindow(factory);
		//ウインドウ・タブ生成中はUpdateViewBounds()を抑制する #1090
		SPI_GetTextWindowByID(winID).SPI_IncrementSuppressUpdateViewBoundsCounter();
		/*#850
		if( tabMode == true )
		{
			SPI_GetTextWindowByID(winID).SPI_SetTabModeMainWindow(true);
		}
		*/
		
		//#850
		//inInitialWindowBoundsが空でなければ、生成したウインドウの初期ウインドウboundsとして設定
		if( inInitialWindowBounds.bottom - inInitialWindowBounds.top > 0 && inInitialWindowBounds.right - inInitialWindowBounds.left > 0 )
		{
			NVI_GetWindowByID(winID).NVI_SetInitialWindowBounds(inInitialWindowBounds);
		}
		//サイドバー／サブテキストの初期状態設定
		if( inSetInitialSideBarDisplay == true )
		{
			SPI_GetTextWindowByID(winID).SPI_SetInitialSideBarDisplay(inLeftSideBar,inRightSideBar,inSubText);
		}
		
		//ウインドウ生成
		NVI_GetWindowByID(winID).NVI_Create(inDocumentID);
		//NVI_GetWindowByID(winID).NVI_SendBehind(kObjectID_Invalid);//#567
	}
	else
	{
		//==============タブ生成の場合==============
		if( inCreateTabInWindowID == kObjectID_Invalid )
		{
			//引数で、タブ生成先ウインドウの指定がなければ、最前面のテキストウインドウにタブを生成する
			winID = frontTextWindowID;//#850 SPI_GetTabModeMainTextWindowID();
		}
		else
		{
			//引数で、タブ生成先ウインドウの指定があれば、そのウインドウ内にタブを生成する。
			winID = inCreateTabInWindowID;
		}
		//ウインドウ・タブ生成中はUpdateViewBounds()を抑制する #1090
		SPI_GetTextWindowByID(winID).SPI_IncrementSuppressUpdateViewBoundsCounter();
		//タブ生成（第２引数はfalseにする。あとでinSelectがtrueならNVI_SelectTabByDocumentID()を行う）
		SPI_GetTextWindowByID(winID).SPNVI_CreateTextDocumentTab(inDocumentID,false,inSelectableTab);//#212 #379
	}
	
	//ウインドウメニュー更新
	//#922 SPI_UpdateWindowMenu();
	
	//ファイルリストウインドウ更新
	//#725 SPI_UpdateFileListWindow_DocumentOpened(inDocumentID);
	SPI_UpdateFileListWindows(kFileListUpdateType_DocumentOpened,inDocumentID);//#725
	
	//最初の選択範囲設定 win 080715
	if( inInitialSelectStart != kIndex_Invalid && inInitialSelectEnd != kIndex_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(winID).SPI_SetSelect(inDocumentID,inInitialSelectStart,inInitialSelectEnd);
	}
	//
	GetApp().SPI_GetTextWindowByID(winID).SPI_AdjustScroll_Center(inDocumentID);
	//B0000 タブモードの場合は既にウインドウ表示済みなのでNVI_Show()で描画されない。そのため、上記行折り返し計算処理等を反映した再描画がされない。
	//それを回避するため、ウインドウ表示済みの場合はウインドウ再描画する
	/*win この時点では行折り返し計算は未実行になった（TabActivate時にSPI_DoViewActivating()で計算される)のでコメントアウト。
	if( GetApp().NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
	{
		GetApp().NVI_GetWindowByID(winID).NVI_RefreshWindow();
	}*/
	
	//
	SPI_GetTextDocumentByID(inDocumentID).SPI_UpdateJumpList();//#291
	
	//ウインドウ表示
	NVI_GetWindowByID(winID).NVI_Show(inSelect);//#567
	//win 上から移動
	if( inSelect == true )
	{
		if( NVI_GetWindowByID(winID).NVI_IsChildWindowVisible() == false )
		{
			NVI_GetWindowByID(winID).NVI_SelectTabByDocumentID(inDocumentID);
		}
	}
	//UpdateViewBounds()の抑制解除 #1090
	SPI_GetTextWindowByID(winID).SPI_DecrementSuppressUpdateViewBoundsCounter();
	//
	SPI_GetTextWindowByID(winID).SPI_UpdateViewBounds();//#410
	//#725 上で実行済みなので削除 SPI_GetTextDocumentByID(inDocumentID).SPI_UpdateJumpList();//#291
	
	//テキストエンコーディング修正ウインドウ表示
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
	/*#932 タイプによる読み分けは無しにする。インデックスウインドウドキュメントも使われていないだろうし。
#if IMPLEMENTATION_FOR_MACOSX
	OSType	type = AFileWrapper::GetType(inFile);
	if( type == 'ftpa' )   return kDocumentType_FTPAlias;
	if( type == 'INDX' )   return kDocumentType_IndexWindow;
	if( type == 'MODE' )   return kDocumentType_ModePref;
#endif
	*/
	//#1050
	//ファイル拡張子取得
	AText	path;
	inFile.GetPath(path);
	AText	suffix;
	path.GetSuffix(suffix);
	//mitabsetならタブセット
	if( suffix.Compare(".mitabset") == true )
	{
		return kDocumentType_TabSet;
	}
	//miremoteならリモートファイルエイリアス #1073
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


/*#725 SPI_OpenOrRevealTextDocument()へ統合
void	AApp::SPI_SelectTextDocument( const AFileAcc& inFile, const ATextIndex inStartIndex, const AItemCount inLength )
{
	//win 080715 Createした場合は、最初の選択範囲を指定するように設定（前回保存時のキャレット位置が表示→その後、すぐに移動となってしまうのを防ぐ）
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

//ワードラップのドキュメントの行情報を再計算
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

//行情報を再計算
void	AApp::SPI_RecalcAll( const AModeIndex inModeIndex )
{
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			SPI_GetTextDocumentByID(docID).SPI_DoWrapRelatedDataChanged(true);
		}
	}
	//行情報が変わるので、イメージサイズの反映等必要
	GetApp().SPI_UpdateTextDrawPropertyAll(inModeIndex);
}

//#349
/**
各ドキュメントのモードを再設定する
*/
void	AApp::SPI_ReSetModeAll( const AModeIndex inModeIndex )
{
	//#844
	//まず最初にそのモードのドキュメントの文法データを全て削除する
	//（SPI_SetMode()の途中で、別のドキュメントの文法データを参照すると、存在しないstateにアクセスしたりする問題があるため。）
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
	//行情報が変わるので、イメージサイズの反映等必要
	GetApp().SPI_UpdateTextDrawPropertyAll(inModeIndex);
}

/**
指定モードのドキュメントを全て文法再認識する
*/
void	AApp::SPI_ReRecognizeSyntaxAll( const AModeIndex inModeIndex )
{
	//各ドキュメントループ
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid  )
		{
			//文法再認識トリガー
			SPI_GetTextDocumentByID(docID).SPI_ReRecognizeSyntax();
			//#683 現状、行再計算は必要ない。ただ、行折り返しありかつ見出しフォント変更機能対応時、ここは変更必要。
		}
	}
	//#450
	//行情報更新
	SPI_UpdateLineImageInfo(inModeIndex);
	//再描画
	GetApp().NVI_RefreshAllWindow();
}

//#890
/**
指定モードのドキュメントの文法認識スレッドを全て停止する
*/
void	AApp::SPI_StopSyntaxRecognizerThreadAll( const AModeIndex inModeIndex )
{
	//各ドキュメントループ
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
指定モードの全てのドキュメント内のRegExpオブジェクトを更新する
*/
void	AApp::SPI_UpdateRegExpForAllDocuments( const AModeIndex inModeIndex )//#683
{
	//各ドキュメントループ
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		if( SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid  )
		{
			//#683 処理速度改善 SPI_GetTextDocumentByID(docID).SPI_SetMode(inModeIndex);//IndentRegExpの更新および行再計算
			//TextInfoのRegExpの更新を行う #683
			SPI_GetTextDocumentByID(docID).SPI_UpdateTextInfoRegExp();
		}
	}
}

//#699
/**
次の行折り返し計算未開始ドキュメントを検索し、折り返し計算を開始する
//AApp::EVTDO_DoTickTimerAction()からコールされ、裏タブを読み込むための処理だが、現状、使用していない。
//裏タブを読み込むと、メモリを食うし、起動後しばらくスレッドが動いて重くなるデメリットがあるため。
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
自動バックアップフォルダ変更を全てのドキュメントに反映
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
モード設定のテキストフォントが変更されたときの各ドキュメントへの反映処理
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

#pragma mark テキストドキュメントからの通知

void	AApp::SPI_DoTextDocumentSaved( const AFileAcc& inFile )
{
	mImportFileManager.DoFileSaved(inFile);
	//マルチファイル置換中以外ならSCMのupdateコマンドを発行する
	//（マルチファイル置換中は大量にupdate発行して、終了時の結果受け取り処理が重くなる）
	if( SPI_GetMultiFileFindWindow().SPI_ExecutingMultiFileReplace() == false )
	{
		SCMUpdateFolderStatusByFile(inFile);//R0006
	}
}

//#896
/**
テキストドキュメントを閉じた時の処理
*/
void	AApp::SPI_DoTextDocumentClosed( const AFileAcc& inFile, const AFileAcc& inProjectFolder )
{
	//プロジェクトフォルダのパス取得
	AText	projectFolderPath;
	inProjectFolder.GetPath(projectFolderPath);
	//ファイルパス取得
	AText	closingDocumentFilePath;
	inFile.GetPath(closingDocumentFilePath);
	//プロジェクトを検索
	AIndex	projectIndex = SPI_GetSameProjectArrayIndex(inProjectFolder);
	if( projectIndex != kIndex_Invalid )
	{
		//==================プロジェクト内のファイルが１つも開かれていない状態になったらメモリ削減のためpurgeする==================
		//プロジェクトが見つかった場合は、そのプロジェクトに属する他のドキュメントが存在するかどうかを調べ、
		//ドキュメントが存在していない場合は、そのプロジェクトに属するファイルのImport File Dataを全てpurgeする
		
		//プロジェクト内のファイルがまだ存在しているかどうかのフラグ
		ABool	oneOrMoreFileStillOpenedInProject = false;
		//各ドキュメントのループ
		for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
		{
			//ドキュメントのファイルが、プロジェクトのフォルダの配下かどうかを調べる。
			ADocumentID	docID = mDocumentArray.GetObjectConst(index).GetObjectID();
			if( NVI_GetDocumentByID(docID).NVI_GetDocumentType() == kDocumentType_Text && 
						NVI_GetDocumentByID(docID).NVI_IsFileSpecified() == true )
			{
				AText	filePath;
				NVI_GetDocumentByID(docID).NVI_GetFilePath(filePath);
				if( filePath.CompareMin(projectFolderPath) == true && filePath.Compare(closingDocumentFilePath) == false )
				{
					//ドキュメントのファイルが、プロジェクトのフォルダの配下なのでフラグONにする
					oneOrMoreFileStillOpenedInProject = true;
				}
			}
		}
		
		//プロジェクト内のファイルが一つも開かれていない場合、プロジェクト内のファイルのimport file dataを全てpurgeする
		if( oneOrMoreFileStillOpenedInProject == false )
		{
			//ワードリストウインドウ検索スレッド停止
			SPI_AbortCurrentWordsListFinderRequestInWordsListWindow();
			//コールグラフ検索スレッド停止
			SPI_AbortCurrentWordsListFinderRequestInCallGrafWindow();
			//プロジェクトに属するファイルのImport File Dataを全てpurgeする
			mImportFileManager.PurgeImportFileData(mSameProjectArray_Pathname.GetObjectConst(projectIndex));
		}
	}
}

/**
ドキュメント編集時処理
*/
void	AApp::SPI_DoTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	//Index windowへ通知
	for( AObjectID docID = NVI_GetFirstDocumentID(kDocumentType_IndexWindow); docID != kObjectID_Invalid; docID = NVI_GetNextDocumentID(docID) )
	{
		SPI_GetIndexWindowDocumentByID(docID).SPI_Update_TextDocumentEdited(inFile,inTextIndex,inInsertedCount);
	}
	//CallGrafへドキュメント編集を通知
	SPI_NotifyToCallGrafByTextDocumentEdited(inFile,inTextIndex,inInsertedCount);
	//WordsListへドキュメント編集を通知
	SPI_NotifyToWordsListByTextDocumentEdited(inFile,inTextIndex,inInsertedCount);
}

//R0199
//指定modeIndexのスペルチェックキャッシュを削除
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
//各ドキュメントのプロジェクトフォルダ設定を更新
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
各ドキュメントのDiffファイル対象を更新
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
Diff対象ドキュメントの行折り返し設定（常にサブペイン幅に従って行折り返しする設定）の更新処理
*/
/*#drop diff infoの表示更新等が面倒なので既に開いているドキュメントには適用しないことにする。
void	AApp::SPI_UpdateDiffTargetDocumentWrap()
{
	for( AIndex index = 0; index < mDocumentArray.GetItemCount(); index++ )
	{
		if( mDocumentArray.GetObject(index).NVI_GetDocumentType() == kDocumentType_Text )
		{
			//Diff対象ドキュメントかどうかの判定
			if( SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_IsDiffTargetDocument() == true )
			{
				//設定判定
				if( NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowDiffTargetWithWordWrap) == true )
				{
					//常に行折り返しあり
					SPI_GetTextDocumentByID(mDocumentArray.GetObject(index).GetObjectID()).SPI_SetWrapMode(kWrapMode_WordWrap,80);
				}
				else
				{
					//Workingドキュメントと同じ設定を適用する
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
SCMのstatus updateコマンド実行完了後にコールされる（ドキュメントのDiffモード更新のため）
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
最前面のドキュメントの親フォルダ取得
*/
ABool	AApp::SPI_GetMostFrontDocumentParentFolder( AFileAcc& outFolder ) const
{
	//デフォルトはユーザードキュメントフォルダ
	AFileWrapper::GetDocumentsFolder(outFolder);
	//最前面ドキュメント取得
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		if( SPI_GetTextDocumentConstByID(docID).NVI_IsFileSpecified() == true )
		{
			//最前面ドキュメントの親フォルダ取得
			AFileAcc	file;
			SPI_GetTextDocumentConstByID(docID).NVI_GetFile(file);
			outFolder.SpecifyParent(file);
			return true;
		}
	}
	return false;
}

/**
現在開いているファイルのトータルのメモリサイズを取得
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

#pragma mark <所有クラス(ADocument_IndexWindow)生成／削除／取得>

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

#pragma mark <所有クラス(AWindow_Text)生成／削除／取得>

#pragma mark ===========================

/**
テキストウインドウ取得
*/
AWindow_Text&	AApp::SPI_GetTextWindowByID( const AObjectID inWindowID )
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,inWindowID);
	//#199 return dynamic_cast<AWindow_Text&>(NVI_GetWindowByID(inWindowID));
}

//#850
/**
テキストウインドウ取得
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
タブ数が一番多いテキストウインドウを取得
*/
AWindowID	AApp::SPI_GetMostNumberTabsTextWindowID() const
{
	AItemCount	maxTabCount = 0;
	AWindowID	maxTabWindowID = kObjectID_Invalid;
	//ウインドウ毎のループ
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObjectConst(index).NVI_GetWindowType() == kWindowType_Text )
		{
			AWindowID	winID = mWindowArray.GetObjectConst(index).GetObjectID();
			AItemCount	tabCount = SPI_GetTextWindowConstByID(winID).NVI_GetSelectableTabCount();
			if( tabCount > maxTabCount )
			{
				//タブ数最大のウインドウ
				maxTabCount = tabCount;
				maxTabWindowID = winID;
			}
		}
	}
	//全てのウインドウのタブ数が1個以下なら、最前面のテキストウインドウを返す
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
すべてのウインドウの右サイドバーを表示／非表示する
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
すべてのウインドウの左サイドバーを表示／非表示する
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
全テキストウインドウ透明度更新
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
	/*#725 InfoHeader廃止
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
//アクティブではない、フローティングウインドウの表示を隠す
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
全てのウインドウのキャレット描画更新
*/
void	AApp::SPI_RefreshCaretAll()
{
	//各ウインドウ毎のループ
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			//キャレット描画更新
			SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_RefreshCaret();
		}
	}
}

//#724
/**
全てのウインドウのマクロバー項目テキストを更新（ショートカット更新時等にコールされる）
*/
void	AApp::SPI_UpdateMacroBarItemContentAll()
{
	//各ウインドウ毎のループ
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			//マクロバー項目のテキストを更新
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
各ウインドウのツールバー項目の値の更新（テキストエンコーディングメニューなど）
*/
void	AApp::SPI_UpdateToolbarItemValue( const ADocumentID inDocumentID )
{
	//すべてのTextWindowに対してツールバー項目の値更新を通知する
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				//AWindow_Text::SPI_UpdateToolbarItemValue()でdocumentIDに一致するタブだけ更新する
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateToolbarItemValue(inDocumentID);
			}
		}
	}
}

/**
全てのテキストウインドウのツールバーの値表示を更新
*/
void	AApp::SPI_UpdateToolbarItemValueAll()
{
	//すべてのTextWindowに対してツールバー項目の値更新を通知する
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
各ウインドウのTextDrawProperty更新
*/
void	AApp::SPI_UpdateTextDrawProperty( const ADocumentID inDocumentID )
{
	//すべてのTextWindowに対してTextDrawProperty更新を通知する
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				//AWindow_Text::SPI_UpdateTextDrawProperty()でdocumentIDに一致するタブだけ更新する
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateTextDrawProperty(inDocumentID);
			}
		}
	}
}

//#379
/**
各ウインドウのViewBounds更新
*/
void	AApp::SPI_UpdateViewBounds( const ADocumentID inDocumentID )
{
	//すべてのTextWindowに対してTextDrawProperty更新を通知する
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				//AWindow_Text::SPI_UpdateViewBounds()でdocumentIDに一致するタブだけ更新する
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateViewBounds(inDocumentID);
			}
		}
	}
}

//#379
/**
各ウインドウのInfoHeader更新
*/
/*#725
void	AApp::SPI_UpdateInfoHeader( const ADocumentID inDocumentID )
{
	//すべてのTextWindowに対してInfoHeader更新を通知する
	for( AIndex index = 0; index < mWindowArray.GetItemCount(); index++ )
	{
		if( mWindowArray.GetObject(index).NVI_GetWindowType() == kWindowType_Text )
		{
			if( mWindowArray.GetObject(index).NVI_GetTabIndexByDocumentID(inDocumentID) != kIndex_Invalid )//#673
			{
				//AWindow_Text::SPI_UpdateInfoHeader()でdocumentIDに一致するタブだけ更新する
				SPI_GetTextWindowByID(mWindowArray.GetObject(index).GetObjectID()).SPI_UpdateInfoHeader(inDocumentID);
			}
		}
	}
}
*/

//#379
/**
ドキュメントに対応するWindow/TabをTryClose
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
ドキュメントに対応するWindow/TabをTryClose
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
各ウインドウのDiff表示更新
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

#pragma mark <所有クラス(AModePrefDB)生成／削除／取得>

#pragma mark ===========================

//ModePrefDB取得
AModePrefDB&	AApp::SPI_GetModePrefDB( const AModeIndex inModeIndex, const ABool inLoadIfNotLoaded )//#253
{
	if( inModeIndex >= mModePrefDBArray.GetItemCount() )
	{
		//このチェック必要かどうかわからないが、旧バージョンがチェックしているので・・・
		_ACError("modeIndex error",this);
		return SPI_GetModePrefDB(kStandardModeIndex);
	}
	//未ロードならロードする
	if( mModePrefDBArray.GetObject(inModeIndex).IsLoaded() == false && inLoadIfNotLoaded == true )
	{
		mModePrefDBArray.GetObject(inModeIndex).LoadOrWaitLoadComplete(true);
	}
	return mModePrefDBArray.GetObject(inModeIndex);
}

/**
モード名(raw)からモードIndex取得
@note thread-safe
*/
AModeIndex	AApp::SPI_FindModeIndexByModeRawName( const AText& inModeName ) const
{
	AStMutexLocker	locker(mModeNameArrayMutex);
	return mModeRawNameArray.Find(inModeName);
}

/**
モード名(表示名)からモードIndex取得
@note thread-safe
@note 表示名は重複している可能性もあるので使用は非推奨。AppleScriptの互換性のために残している。
*/
AModeIndex	AApp::SPI_FindModeIndexByModeDisplayName( const AText& inModeName ) const
{
	AStMutexLocker	locker(mModeNameArrayMutex);
	return mModeDisplayNameArray.Find(inModeName);
}

//#427
/**
モード名(raw)の重複チェック（大文字小文字無視）
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
モード名変更時処理
*/
void	AApp::SPI_ModeNameUpdated( const AModeIndex inModeIndex )
{
	AStMutexLocker	locker(mModeNameArrayMutex);
	//モード名配列更新
	AText	modeRawName;
	SPI_GetModePrefDB(inModeIndex).GetModeRawName(modeRawName);
	mModeRawNameArray.Set(inModeIndex,modeRawName);
	AText	modeDisplayName;
	SPI_GetModePrefDB(inModeIndex).GetModeDisplayName(modeDisplayName);
	mModeRawNameArray.Set(inModeIndex,modeDisplayName);
	//モードメニュー更新
	SPI_UpdateModeMenu();
	//モード設定ウインドウ更新
	GetApp().SPI_GetModePrefWindow(inModeIndex).NVI_UpdateProperty();//#858
	GetApp().SPI_GetModePrefWindow(inModeIndex).NVI_RefreshWindow();//#858
}

/**
テキストファイルのファイル名からモードIndexを取得する
@note thread-safe
*/
AModeIndex	AApp::SPI_FindModeIDByTextFilePath( const AText& inFilePath ) const//R0210
{
	//==================拡張子設定が一致するモードを検索==================
	//拡張子設定が一致するモードを全て取得する
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
	//拡張子が一致するモードがない場合は、標準モードを返す
	if( modeIndexArray.GetItemCount() == 0 )
	{
		return kStandardModeIndex;
	}
	//拡張子が一致するモードが１つのみの場合は、そのモードで確定。
	if( modeIndexArray.GetItemCount() == 1 )
	{
		return modeIndexArray.Get(0);
	}
	
	//#868
	//==================複数のモードの拡張子が一致した場合、モードファイルパスに一致するモードを検索==================
	//下記の優先順位
	//1. モードファイルパスが一致するモード
	//2. モードファイルパス設定が無いモード（上のモード優先）
	//3. 最初のモード
	//モードファイルパス設定が無いモードを記憶するための変数
	AIndex	modeIndexWithoutFilePathSetting = kIndex_Invalid;
	//上で検索した拡張子が一致するモード毎のループ
	for( AIndex i = 0; i < modeIndexArray.GetItemCount(); i++ )
	{
		//モードファイルパスに一致したらそのモードindexを返り値として終了
		if( mModePrefDBArray.GetObjectConst(modeIndexArray.Get(i)).IsThisModeSelectFilePath(inFilePath) == true )
		{
			return modeIndexArray.Get(i);
		}
		//モードファイルパス設定が無い場合は、そのモード（最初のもの）を記憶する
		if( modeIndexWithoutFilePathSetting == kIndex_Invalid )
		{
			if( mModePrefDBArray.GetObjectConst(modeIndexArray.Get(i)).GetItemCount_TextArray(AModePrefDB::kModeSelectFilePath) == 0 )
			{
				modeIndexWithoutFilePathSetting = modeIndexArray.Get(i);
			}
		}
	}
	//------------------モードファイルパス設定に一つも一致しなかった場合------------------
	if( modeIndexWithoutFilePathSetting == kIndex_Invalid )
	{
		//モードファイルパス設定が無いモードが無かった場合は、最初のモードを返す
		return modeIndexArray.Get(0);
	}
	else
	{
		//モードファイルパス設定が無いモードがあれば、そのモード
		return modeIndexWithoutFilePathSetting;
	}
}

//#427
/**

下記の箇所からコールされる。
1. AApp::InitModePref()の、標準モード作成箇所。
標準モードを新規作成したとき（＝mi新規インストール時）はinConvertToAutoUpdate=trueとなる。
2. AApp::InitModePref()の、標準以外のモード作成箇所。
モードを新規作成したとき（＝mi新規インストール時、および、組み込みモード追加時）はinConvertToAutoUpdate=trueとなる。
3. AApp::SPI_AddNewMode()新規モード（inConvertToAutoUpdateはfalse）
4. SPI_AddNewModeImportFromFolder()
インポートの場合はinConvertToAutoUpdateはfalse。
Webから取得の場合はinConvertToAutoUpdateはtrue。

つまり、組み込みモードインストール時のみinConvertToAutoUpdate=true、モード新規作成（インポート含む）はinConvertToAutoUpdate=falseとなる。

inConvertToAutoUpdate=trueのときは、下記処理を行う。#1374
・ユーザー編集項目のキーワード・見出しを削除
*/
AModeIndex	AApp::AddMode( const AFileAcc inModeFolder, const ABool inConvertToAutoUpdate, const ABool inRemoveMultiLanguageModeName )
{
	//モードファイル取得
	AText	filename;
	inModeFolder.GetFilename(filename);
	
	//==================自動更新モードデータへの変換を行う①（キーワードとツールを自動更新フォルダへ移動）==================
	
	//自動更新モードデータへの変換は、ModePreferences.miが存在する場合のみ対応する
	//ModePreferences.miが無い場合、autoupdateフォルダ配下からもMMKEリソース等から読み込む必要があるが、
	//処理が煩雑になるため。
	//（旧バージョンで配布されているデータは、自動更新で配布されることも無いと思われるため、
	//旧バージョンデータについて、自動更新データへの変換をする必要はない）
	ABool	convertToAutoUpdate = inConvertToAutoUpdate;
	AFileAcc 	modePrefFile;
	modePrefFile.SpecifyChild(inModeFolder,"data/ModePreferences.mi");
	if( modePrefFile.Exist() == false )
	{
		convertToAutoUpdate = false;
	}
	
	/*#1374
	組み込みモードインストール時、ユーザーデータへコピーするのはdata/ModePreferences.miのみにしたので、autoupdateフォルダへ移す作業は不要となった。
	//自動更新モードデータを構成
	if( convertToAutoUpdate == true )
	{
		//autoupdateフォルダ生成
		AFileAcc	autoUpdateFolder;
		autoUpdateFolder.SpecifyChild(inModeFolder,"autoupdate");
		autoUpdateFolder.CreateFolder();
		
		//モードフォルダ内ののautoupdate, autoupdate_userdata以外を、autoupdateフォルダへコピー
		AObjectArray<AFileAcc>	children;
		inModeFolder.GetChildren(children);
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			//ファイル／フォルダ名取得
			AText	filename;
			children.GetObjectConst(i).GetFilename(filename);
			//autoupdate, autoupdate_userdataフォルダはコピー対象外
			if( filename.Compare("autoupdate") == true )   continue;
			if( filename.Compare("autoupdate_userdata") == true )   continue;
			//ファイルまたはフォルダをコピー
			AFileAcc	dst;
			dst.SpecifyChild(autoUpdateFolder,filename);
			children.GetObject(i).CopyFileOrFolderTo(dst,false,false);
		}
		
		//ユーザーツールフォルダを削除
		AFileAcc	userToolFolder;
		userToolFolder.SpecifyChild(inModeFolder,"tool");
		userToolFolder.DeleteFileOrFolderRecursive();
		
		//キーワード説明ファイルを削除
		AFileAcc	keywordExplanationFile;
		keywordExplanationFile.SpecifyChild(inModeFolder,"data/keywords_explanation.txt");
		keywordExplanationFile.DeleteFileOrFolderRecursive();
		
		//ユーザーキーワードフォルダ中身を削除 #954
		AFileAcc	userKeywordFolder;
		userKeywordFolder.SpecifyChild(inModeFolder,"keyword");
		userKeywordFolder.DeleteFileOrFolderRecursive();
	}
	*/
	
	//#954
	//==================ユーザーtool, keywordフォルダ生成==================
	{
		AFileAcc	userToolFolder;
		userToolFolder.SpecifyChild(inModeFolder,"tool");
		userToolFolder.CreateFolderRecursive();
		
		AFileAcc	userKeywordFolder;
		userKeywordFolder.SpecifyChild(inModeFolder,"keyword");
		userKeywordFolder.CreateFolderRecursive();
	}
	
	//==================モードDBオブジェクト生成==================
	//オブジェクト生成
	AModeIndex	modeIndex = mModePrefDBArray.AddNewObject();
	//mode pref db初期化
	mModePrefDBArray.GetObject(modeIndex).InitMode(modeIndex,inModeFolder);
	//自動更新モードへの変換、または、多言語モード名の削除を行う時は、それらの処理を行うために、ここでmode prefのロードを行う
	if( inConvertToAutoUpdate == true || inRemoveMultiLanguageModeName == true )
	{
		SPI_GetModePrefDB(modeIndex);
	}
	
	//#868
	//==================モードウインドウ生成==================
	mModePrefWindowIDArray.Add(kObjectID_Invalid);
	
	
	//==================多言語モード名の削除==================
	//モード名を指定してインポートしたときは、多言語モード名は削除する
	if( inRemoveMultiLanguageModeName == true )
	{
		mModePrefDBArray.GetObject(modeIndex).RemoveMultiLanguageModeNames();
	}
	
	//==================モード名データ取得==================
	//mModeNameArrayを設定
	{
		AStMutexLocker	locker(mModeNameArrayMutex);
		AText	modeRawName;
		mModePrefDBArray.GetObjectConst(modeIndex).GetModeRawName(modeRawName);
		mModeRawNameArray.Add(modeRawName);
		AText	modeDisplayName;
		mModePrefDBArray.GetObjectConst(modeIndex).GetModeDisplayName(modeDisplayName);
		mModeDisplayNameArray.Add(modeDisplayName);
	}
	
	//==================自動更新モードデータへの変換を行う②（ユーザーデータから組み込みキーワードを削除）==================
	//#427 
	if( convertToAutoUpdate == true )
	{
		//ユーザーキーワードカテゴリー／見出しを削除する
		//キーワードカテゴリー
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
		//見出し
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
モード拡張子検索ハッシュを更新
*/
void	AApp::SPI_UpdateModeSuffixArray( const AModeIndex inModeIndex )
{
	//排他制御
	AStMutexLocker	locker(mModeSuffixArrayMutex);
	
	//指定モードのデータを削除
	{
		for( AIndex i = 0; i < mModeSuffixArray_Suffix.GetItemCount(); )
		{
			//モードが一致していたら削除
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
	
	//モードがdisableならデータ追加せずリターン
	if( SPI_GetModePrefDB(inModeIndex,false).GetData_Bool(AModePrefDB::kEnableMode) == false )
	{
		return;
	}
	
	//モード設定に設定されたsuffixを追加
	{
		AItemCount	itemCount = SPI_GetModePrefDB(inModeIndex,false).GetItemCount_Array(AModePrefDB::kSuffix);
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//suffix取得
			AText	suffix;
			SPI_GetModePrefDB(inModeIndex,false).GetData_TextArray(AModePrefDB::kSuffix,i,suffix);
			//データ追加
			mModeSuffixArray_Suffix.Add(suffix);
			mModeSuffixArray_ModeIndex.Add(inModeIndex);
		}
	}
}

/**
拡張子に対応するモードを検索
*/
void	AApp::SPI_FindModeIndexFromSuffix( const AText& inFilePath, AArray<AIndex>& outModeIndexArray ) const
{
	//結果初期化
	outModeIndexArray.DeleteAll();
	//suffix開始位置を取得
	AIndex	suffixspos = inFilePath.GetSuffixStartPos();
	//#1428 拡張子なしが一致するようにする if( suffixspos == inFilePath.GetItemCount() )   return;
	//排他制御
	AStMutexLocker	locker(mModeSuffixArrayMutex);
	//拡張子ハッシュからinFilePathの拡張子に対応する項目のindexを検索
	AArray<AIndex>	indexArray;
	mModeSuffixArray_Suffix.FindAll(inFilePath,suffixspos,inFilePath.GetItemCount()-suffixspos,indexArray);
	//モードindexを取得
	AItemCount	itemCount = indexArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		outModeIndexArray.Add(mModeSuffixArray_ModeIndex.Get(indexArray.Get(i)));
	}
}

//
AModeIndex	AApp::SPI_AddNewMode( const AText& inModeName )
{
	//既に存在するモード名なら終了
	AModeIndex	modeIndex = SPI_FindModeIndexByModeRawName(inModeName);
	if( modeIndex != kIndex_Invalid )   return kIndex_Invalid;
	
	//モードフォルダの作成
	AFileAcc	rootModeFolder;
	SPI_GetModeRootFolder(rootModeFolder);//アプリケーション(.app)と同列の_modeフォルダ（優先）か、Prefフォルダのmodeフォルダ
	AFileAcc	modeFolder;
	modeFolder.SpecifyChild(rootModeFolder,inModeName);
	modeFolder.CreateFolder();
	
	//ツールフォルダの生成
	AFileAcc	toolFolder;
	toolFolder.SpecifyChild(modeFolder,"tool");
	
	//モード作成
	modeIndex = AddMode(modeFolder,false,true);
	
	//モードファイル保存
	SPI_GetModePrefDB(modeIndex).SaveToFile();
	
	SPI_UpdateModeMenu();
	
	return modeIndex;
}

/**
他モードフォルダからインポートして、モードを新規追加
*/
AModeIndex	AApp::SPI_AddNewModeImportFromFolder( const AText& inModeName, const AFileAcc& inImportFolder, 
			const ABool inSecurityScreened, const ABool inConvertToAutoUpdate )//R0000 #427
{
	//既に存在するモード名なら終了
	AModeIndex	modeIndex = SPI_FindModeIndexByModeRawName(inModeName);
	if( modeIndex != kIndex_Invalid )   return kIndex_Invalid;
	
	//R0000 security 結局こうするぐらいしかない
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
	
	//コピー先モードフォルダの取得
	AFileAcc	rootModeFolder;
	SPI_GetModeRootFolder(rootModeFolder);//アプリケーション(.app)と同列の_modeフォルダ（優先）か、Prefフォルダのmodeフォルダ
	AFileAcc	modeFolder;
	modeFolder.SpecifyChild(rootModeFolder,inModeName);
	
	//モードフォルダをコピー
	inImportFolder.CopyFolderTo(modeFolder,true,true);
	
	//モード作成
	modeIndex = AddMode(modeFolder,inConvertToAutoUpdate,true);//#427 モード追加し、autoupdateフォルダ構成
	
	//インポート元のモードがdisableになっている可能性があるので、ここでコピー先モードはenableにする
	SPI_GetModePrefDB(modeIndex).SetData_Bool(AModePrefDB::kEnableMode,true);
	
	//モードメニューの更新
	SPI_UpdateModeMenu();
	
	//「同じプロジェクト」のデータを更新
	SPI_UpdateAllSameProjectData();
	
	return modeIndex;
}

//R0000 security
ABool	AApp::SPI_ScreenModeExecutable( const AFileAcc& inItem, AText& outText, AText& outReport ) 
{
	/*R0137 outText.DeleteAll();
	outReport.DeleteAll();
	return false;//暫定
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
	//パスはエイリアス解決前のパスを表示する
	AText	path;
	file.GetNormalizedPath(path);
	path.ConvertToCanonicalComp();
	if( file.IsFolder() == true )
	{
		//フォルダはツールメニューの実行対象にならないが、将来のため、.appはチェックする
		AText	filename;
		file.GetFilename(filename);
		AText	suffix;
		filename.GetSuffix(suffix);
		if( suffix.Compare(".app") == true )
		{
			//アプリケーション
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
		//エイリアス解決
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
			//アプリケーション
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
			//実行可能ファイル
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
				//シェルスクリプト
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
実行可能ファイル警告表示（モード追加時）
*/
void	AApp::SPI_ShowModeExecutableAlertWindow( const AText& inModeName, 
												const AFileAcc& inImportFile, const AText& inFileList, const AText& inReport, const ABool inConvertToAutoUpdate )//#427
{
	SPI_GetModeExecutableAlertWindow().NVI_CreateAndShow();
	SPI_GetModeExecutableAlertWindow().SPI_Set(inModeName,inImportFile,inFileList,inReport,inConvertToAutoUpdate);//#427
	SPI_GetModeExecutableAlertWindow().NVI_SwitchFocusToFirst();
	//モーダル開始
	SPI_GetModeExecutableAlertWindow().NVI_RunModalWindow();
	//ウインドウを閉じる
	SPI_GetModeExecutableAlertWindow().NVI_Close();
}

/**
実行可能ファイル警告表示（ツール追加時）
*/
void	AApp::SPI_ShowModeExecutableAlertWindow_Tool( const AModeIndex inModeIndex, const AObjectID inToolMenuObjectID, const AIndex inRowIndex, 
													 const AFileAcc& inImportFile, const AFileAcc& inDstFile, const AText& inFileList, const AText& inReport )
{
	SPI_GetModeExecutableAlertWindow().NVI_CreateAndShow();
	SPI_GetModeExecutableAlertWindow().SPI_Set_Tool(inModeIndex,inToolMenuObjectID,inRowIndex,inImportFile,inDstFile,inFileList,inReport);
	SPI_GetModeExecutableAlertWindow().NVI_SwitchFocusToFirst();
	//モーダル開始
	SPI_GetModeExecutableAlertWindow().NVI_RunModalWindow();
	//ウインドウを閉じる
	SPI_GetModeExecutableAlertWindow().NVI_Close();
}

#pragma mark ===========================

#pragma mark <所有クラス(AMultiFileFinder)>

#pragma mark ===========================

//マルチファイル検索スレッド取得  
AMultiFileFinder&	AApp::GetMultiFileFinder()
{
	return dynamic_cast<AMultiFileFinder&>(NVI_GetThreadByID(mMultiFileFinderObjectID));
}

//マルチファイル検索実行中かどうかを返す 
ABool	AApp::SPI_IsMultiFileFindWorking()
{
	//#1378 検索終了イベント受信時にまだスレッドが動作しているのでフラグ参照に変える。 return GetMultiFileFinder().NVI_IsThreadWorking();
	return GetMultiFileFinder().SPI_IsWorking();
}

//マルチファイル検索開始 
void	AApp::SPI_StartMultiFileFind( const AFindParameter& inFindParam, const ADocumentID inIndexDocumentID,
									 const ABool inForMultiFileReplace, const ABool inExtractMatchedText )
{
	//検索前に存在するグループの最新を折りたたむ #0
	SPI_GetIndexWindowDocumentByID(inIndexDocumentID).SPI_CollapseFirstGroup();
	//検索実行
	GetMultiFileFinder().SPNVI_Run(inFindParam,inIndexDocumentID,inForMultiFileReplace,inExtractMatchedText);
}

//マルチファイル検索中断 
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

#pragma mark <所有クラス(AImportFileRecognizer)>

#pragma mark ===========================

//
AImportFileRecognizer&	AApp::SPI_GetImportFileRecognizer()
{
	return dynamic_cast<AImportFileRecognizer&>(NVI_GetThreadByID(mImportFileRecognizerObjectID));
}

#pragma mark ===========================

#pragma mark <所有クラス(AFTPConnection)>

#pragma mark ===========================

//
AFTPConnection&	AApp::SPI_GetFTPConnection()
{
	return dynamic_cast<AFTPConnection&>(NVI_GetThreadByID(mFTPConnectionObjectID));
}

#if IMPLEMENTATION_FOR_MACOSX

#pragma mark ===========================

#pragma mark <所有クラス(AAppleScriptExecuter)>

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

#pragma mark <所有クラス(AWindow_MultiFileFind)>

#pragma mark ===========================

//マルチファイル検索ウインドウ表示
void	AApp::SPI_ShowMultiFileFindWindow( const AText& inFindText )
{
	SPI_GetMultiFileFindWindow().NVI_Show();
	if( inFindText.GetItemCount() > 0 )//win
	{
		SPI_GetMultiFileFindWindow().SPI_SetFindText(inFindText);
	}
	SPI_GetMultiFileFindWindow().NVI_SwitchFocusToFirst();
}

//「最近使った～」に追加
void	AApp::SPI_AddRecentlyUsed( const AText& inFindText, const AFileAcc& inFolder )
{
	SPI_GetMultiFileFindWindow().SPI_AddRecentlyUsedFindText(inFindText);
	SPI_GetMultiFileFindWindow().SPI_AddRecentlyUsedPath(inFolder);
}

//マルチファイル検索ウインドウ表示更新
void	AApp::SPI_UpdateMultiFileFindWindow()
{
	SPI_GetMultiFileFindWindow().NVI_UpdateProperty();
}

#pragma mark ===========================

#pragma mark <所有クラス(AWindow_Find)>

#pragma mark ===========================

//検索ウインドウ表示
void	AApp::SPI_ShowFindWindow( const AText& inFindText )
{
	SPI_GetFindWindow().NVI_Show();
	if( inFindText.GetItemCount() > 0 )//win
	{
		SPI_SetFindText(inFindText);
	}
	SPI_GetFindWindow().NVI_SwitchFocusToFirst();
}

//検索ウインドウ表示更新
void	AApp::SPI_UpdateFindWindow()
{
	SPI_GetFindWindow().NVI_UpdateProperty();
}

#pragma mark ===========================

#pragma mark <所有クラス(AWindow_ModePref)>

#pragma mark ===========================

//
void	AApp::SPI_ShowModePrefWindow( const AModeIndex inModeIndex )
{
	if( SPI_GetModePrefWindow(inModeIndex).NVI_IsWindowVisible() == false )//#858
	{
		//ウインドウ生成
		SPI_GetModePrefWindow(inModeIndex).NVI_CreateAndShow();
		//生成ウインドウ数をカウント（ウインドウ位置オフセットに使用する）
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
		//ウインドウ位置設定
		APoint	windowPosition = {0};
		SPI_GetModePrefWindow(inModeIndex).NVI_GetWindowPosition(windowPosition);
		windowPosition.x += 8*(createdWindowCount-1);
		windowPosition.y += 8*(createdWindowCount-1);
		SPI_GetModePrefWindow(inModeIndex).NVI_SetWindowPosition(windowPosition);
		//最初のフォーカスに移動
		SPI_GetModePrefWindow(inModeIndex).NVI_UpdateProperty();//#858
		SPI_GetModePrefWindow(inModeIndex).NVI_SwitchFocusToFirst();//#858
	}
	else
	{
		//ウインドウ選択
		SPI_GetModePrefWindow(inModeIndex).NVI_SelectWindow();//#858
		/*if( inModeIndex != kIndex_Invalid )
		{
			SPI_GetModePrefWindow(inModeIndex).SPI_SelectMode(inModeIndex);//#858
		}*/
	}
}

//#868
/**
モード設定ウインドウ取得
*/
AWindow_ModePref&	AApp::SPI_GetModePrefWindow( const AIndex inModeIndex )
{
	//
	if( mModePrefWindowIDArray.Get(inModeIndex) == kObjectID_Invalid )
	{
		//ウインドウ生成
		AWindowDefaultFactory<AWindow_ModePref>	modePrefWindowFactory;
		mModePrefWindowIDArray.Set(inModeIndex,NVI_CreateWindow(modePrefWindowFactory));
		SPI_GetModePrefWindow(inModeIndex).SPI_SetModeIndex(inModeIndex);
	}
	//
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ModePref,kWindowType_ModePref,mModePrefWindowIDArray.Get(inModeIndex));
}

/**
モード生成ウインドウ生成済みかどうかを取得
*/
ABool	AApp::SPI_IsModePrefWindowCreated( const AIndex inModeIndex ) const
{
	return (mModePrefWindowIDArray.Get(inModeIndex) != kObjectID_Invalid);
}

#pragma mark ===========================

#pragma mark --- 新規モード追加ウインドウ


#pragma mark ===========================

#pragma mark --- サブウインドウ
//#725

/**
サブウインドウ生成
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
			//DocImp生成 #1034
			AObjectID	docImpObjectID = SPI_CreateDocImp(kDocumentType_IndexWindow);
			//Index Document生成
			AIndexWindowDocumentFactory	docfactory(this,docImpObjectID);//#1034
			ADocumentID	docID = GetApp().NVI_CreateDocument(docfactory);
			//検索結果ウインドウ生成
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
	//データ追加
	mSubWindowArray_WindowID.Add(winID);
	mSubWindowArray_SubWindowType.Add(inType);
	mSubWindowArray_TextWindowID.Add(inTextWindowID);
	mSubWindowArray_SubWindowLocationType.Add(inLocationType);
	mSubWindowArray_SubWindowLocationIndex.Add(inLocationIndex);
	//サイドバー表示ならオーバーレイモードに設定
	if( inLocationType == kSubWindowLocationType_LeftSideBar || inLocationType == kSubWindowLocationType_RightSideBar ||
				inLocationType == kSubWindowLocationType_Popup )
	{
		NVI_GetWindowByID(winID).NVI_ChangeToOverlay(inTextWindowID,false);
	}
	//最小サイズ設定
	if( inCollapsed == false )
	{
		//折りたたみではない場合の最小サイズ設定
		NVI_GetWindowByID(winID).NVI_SetWindowMinimumSize(kSubWindowMinWidth,SPI_GetSubWindowMinHeight(winID));
	}
	else
	{
		//折りたたみ時の最小サイズ設定
		NVI_GetWindowByID(winID).NVI_SetWindowMinimumSize(kSubWindowMinWidth,SPI_GetSubWindowCollapsedHeight());
	}
	//ウインドウ生成
	if( inType == kSubWindowType_JumpList )
	{
		//------------------ジャンプリストの場合------------------
		//テキストウインドウに存在するタブをここで生成
		AWindow_Text&	textWindow = GetApp().SPI_GetTextWindowByID(inTextWindowID);
		for( AIndex tabIndex = 0; tabIndex < textWindow.NVI_GetTabCount(); tabIndex++ )
		{
			GetApp().SPI_GetJumpListWindow(winID).NVI_CreateTab(textWindow.NVI_GetDocumentIDByTabIndex(tabIndex));
		}
	}
	else
	{
		//------------------それ以外のウインドウの場合------------------
		NVI_GetWindowByID(winID).NVI_Create(kObjectID_Invalid);
	}
	//パラメータ設定
	switch( inType )
	{
	  case kSubWindowType_FileList:
		{
			SPI_GetFileListWindow(winID).SPI_ChangeMode(inParameter);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return winID;
}

/**
サブウインドウ削除
*/
void	AApp::SPI_DeleteSubWindow( const AWindowID inSubWindowID )
{
	//ウインドウ削除
	NVI_DeleteWindow(inSubWindowID);
	
	//データ削除
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return;}
	mSubWindowArray_WindowID.Delete1(index);
	mSubWindowArray_SubWindowType.Delete1(index);
	mSubWindowArray_TextWindowID.Delete1(index);
	mSubWindowArray_SubWindowLocationType.Delete1(index);
	mSubWindowArray_SubWindowLocationIndex.Delete1(index);
	
	//項目選択アクティブサブウインドウを削除した場合は、アクティブを解除
	if( inSubWindowID == mActiveSubWindowForItemSelector )
	{
		mActiveSubWindowForItemSelector = kObjectID_Invalid;
	}
}

/**
サブウインドウを閉じる（ウインドウのクローズボタンクリック時など）
*/
void	AApp::SPI_CloseSubWindow( const AWindowID inSubWindowID )
{
	//サブウインドウindex検索
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return;}
	
	//テキストウインドウに閉じたことを通知
	AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
	if( textWindowID != kObjectID_Invalid )
	{
		SPI_GetTextWindowByID(textWindowID).SPI_DoSubWindowClosed(inSubWindowID);
	}
	
	//ウインドウ削除
	SPI_DeleteSubWindow(inSubWindowID);
}

/**
サブウインドウプロパティ（表示位置）取得
*/
void	AApp::SPI_GetSubWindowProperty( const AWindowID inSubWindowID,
		ASubWindowLocationType& outLocationType, AIndex& outLocationIndex ) const
{
	//サブウインドウindex検索
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return;}
	//プロパティ取得
	outLocationType = mSubWindowArray_SubWindowLocationType.Get(index);
	outLocationIndex = mSubWindowArray_SubWindowLocationIndex.Get(index);
}

/**
サブウインドウプロパティ（表示位置）設定
*/
void	AApp::SPI_SetSubWindowProperty( const AWindowID inSubWindowID,
		const ASubWindowLocationType inLocationType, const AIndex inLocationIndex, const AWindowID inTextWindowID )
{
	//サブウインドウindex検索
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return;}
	//プロパティ設定
	mSubWindowArray_SubWindowLocationType.Set(index,inLocationType);
	mSubWindowArray_SubWindowLocationIndex.Set(index,inLocationIndex);
	mSubWindowArray_TextWindowID.Set(index,inTextWindowID);
	
	if( inLocationType == kSubWindowLocationType_LeftSideBar || inLocationType == kSubWindowLocationType_RightSideBar ||
				inLocationType == kSubWindowLocationType_Popup )
	{
		//------------------サイドバー、ポップアップの場合------------------
		if( NVI_GetWindowByID(inSubWindowID).NVI_GetOverlayMode() == false )
		{
			//タイプ変更したときには最小サイズを通常に戻す（collapse時の最小サイズを解除するため）
			NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(kSubWindowMinWidth,SPI_GetSubWindowMinHeight(inSubWindowID));
			//オーバーレイウインドウに設定
			NVI_GetWindowByID(inSubWindowID).NVI_ChangeToOverlay(inTextWindowID,false);
			//表示更新
			NVI_GetWindowByID(inSubWindowID).NVI_UpdateProperty();
			NVI_GetWindowByID(inSubWindowID).NVI_RefreshWindow();
		}
	}
	else
	{
		//------------------フローティングの場合------------------
		if( NVI_GetWindowByID(inSubWindowID).NVI_GetOverlayMode() == true )
		{
			//タイプ変更したときには最小サイズを通常に戻す（collapse時の最小サイズを解除するため）
			NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(kSubWindowMinWidth,SPI_GetSubWindowMinHeight(inSubWindowID));
			//フローティングウインドウに設定
			NVI_GetWindowByID(inSubWindowID).NVI_ChangeToFloating();
			//表示更新
			NVI_GetWindowByID(inSubWindowID).NVI_UpdateProperty();
			NVI_GetWindowByID(inSubWindowID).NVI_RefreshWindow();
		}
	}
}

/**
サイドバーの最終項目かどうかを取得
*/
ABool	AApp::SPI_IsSubWindowSideBarBottom( const AWindowID inSubWindowID ) const
{
	//サブウインドウindex検索
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return false;}
	//プロパティ取得
	AWindowID	winID = mSubWindowArray_WindowID.Get(index);
	AWindowID	textWinID = mSubWindowArray_TextWindowID.Get(index);
	if( textWinID == kObjectID_Invalid )
	{
		return false;
	}
	//最終項目かどうかを取得
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
			//処理なし
			break;
		}
	}
	return false;
}

/**
サブウインドウ表示位置タイプ取得
*/
ASubWindowLocationType	AApp::SPI_GetSubWindowLocationType( const AWindowID inSubWindowID ) const
{
	//サブウインドウindex検索
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return kSubWindowLocationType_None;}
	//表示位置タイプ取得
	return mSubWindowArray_SubWindowLocationType.Get(index);
}


/**
サブウインドウタイプ取得
*/
ASubWindowType	AApp::SPI_GetSubWindowType( const AWindowID inSubWindowID ) const
{
	//サブウインドウindex検索
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return kSubWindowType_None;}
	//タイプ取得
	return mSubWindowArray_SubWindowType.Get(index);
}

/**
サブウインドウパラメータ取得
*/
ANumber	AApp::SPI_GetSubWindowParameter( const AWindowID inSubWindowID ) const
{
	//サブウインドウindex検索
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return 0;}
	//パラメータ取得
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
			//処理なし
			break;
		}
	}
	return parameter;
}

/**
オーバーレイサブウインドウ配置
*/
void	AApp::SPI_UpdateLayoutOverlaySubWindow( const AWindowID inTextWindowID, const ASubWindowType inType, const AWindowID inSubWindowID,
		const AWindowPoint inPoint, const ANumber inWidth, const ANumber inHeight, const ABool inAnimate )
{
	NVI_GetWindowByID(inSubWindowID).NVI_Show(false);
	NVI_GetWindowByID(inSubWindowID).NVI_SetOffsetOfOverlayWindowAndSize(inTextWindowID,inPoint,inWidth,inHeight,inAnimate);
}

/**
オーバーレイサブウインドウ項目移動
*/
void	AApp::SPI_MoveOverlaySubWindow( const ASubWindowLocationType inSrcType, const AIndex inSrcIndex,
		const ASubWindowLocationType inDstType, const AIndex inDstIndex )
{
	//全てのテキストウインドウに適用する
	for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
	{
		//オーバーレイサブウインドウ移動
		SPI_GetTextWindowByID(winID).SPI_MoveOverlaySubWindow(inSrcType,inSrcIndex,inDstType,inDstIndex);
	}
}

/**
オーバーレイサブウインドウ追加
*/
void	AApp::SPI_AddOverlaySubWindow( const ASubWindowLocationType inLocationType, const AIndex inIndex,
		const ASubWindowType inSubWindowType )
{
	//全てのテキストウインドウに適用する
	for( AWindowID winID = NVI_GetFirstWindowID(kWindowType_Text); winID != kObjectID_Invalid; winID = NVI_GetNextWindowID(winID) )
	{
		//オーバーレイサブウインドウ追加
		SPI_GetTextWindowByID(winID).SPI_AddOverlaySubWindow(inLocationType,inIndex,inSubWindowType);
	}
}

/**
サブウインドウフォント取得
*/
void	AApp::SPI_GetSubWindowsFont( AText& outFontName, AFloatNumber& outFontSize ) const
{
	NVI_GetAppPrefDBConst().GetData_Text(AAppPrefDB::kSubWindowsFontName,outFontName);
	outFontSize = NVI_GetAppPrefDBConst().GetData_FloatNumber(AAppPrefDB::kSubWindowsFontSize);
}

/**
サブウインドウ用背景色取得
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
サブウインドウ内ボックス色取得
→ポップアップウインドウの色に使用する。（テキストエリアの色に似た色を返す。）
*/
void	AApp::SPI_GetSubWindowBoxColor( const AWindowID inSubWindowID, 
										AColor& outLetterColor, //#1316 AColor& outDropShadowColor,
									   AColor& outBoxColor1, AColor& outBoxColor2, AColor& outBoxColor3 ) const
{
	//ポップアップウインドウ以外の場合は、文字色：黒、ボックス：白
	outLetterColor = AColorWrapper::GetColorByHTMLFormatColor("303030");//#1316 mSkinColor_SubWindowBoxLetterColor;
	//#1316 outDropShadowColor = mSkinColor_SubWindowBoxLetterDropShadowColor;
	AColor	boxBaseColor = AColorWrapper::GetColorByHTMLFormatColor("F2F2F2");//#1316 mSkinColor_SubWindowBoxBackgroundColor;
	//
	if( GetApp().NVI_IsDarkMode() == true )
	{
		outLetterColor = AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
		boxBaseColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
	}
	//ポップアップウインドウの場合は、モード設定の文字色、背景色に従う
	if( GetApp().SPI_GetSubWindowLocationType(inSubWindowID) == kSubWindowLocationType_Popup )
	{
		AWindowID	textWindowID = GetApp().SPI_GetSubWindowParentTextWindow(inSubWindowID);
		if( textWindowID != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetTextWindowByID(textWindowID).NVI_GetCurrentDocumentID() != kObjectID_Invalid )
			{
				AModeIndex	modeIndex = GetApp().SPI_GetTextWindowByID(textWindowID).SPI_GetCurrentFocusTextDocument().SPI_GetModeIndex();
				//背景色
				GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kBackgroundColor,boxBaseColor);
				//文字色
				GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kLetterColor,outLetterColor);
			}
		}
	}
	outBoxColor1 = AColorWrapper::ChangeHSV(boxBaseColor,0,1.0,0.99);//#1316 1.2);
	outBoxColor2 = AColorWrapper::ChangeHSV(boxBaseColor,0,1.0,0.99);//#1316 0.95);
	outBoxColor3 = AColorWrapper::ChangeHSV(boxBaseColor,0,1.0,0.99);//#1316 0.93);
}

/**
サブウインドウrounded rect選択時囲み色取得
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
サブウインドウrounded rectホバー時囲み色取得
*/
AColor	AApp::SPI_GetSubWindowRoundedRectBoxHoverColor() const
{
	return kColor_Blue;
}

/**
サブウインドウ標準文字色取得
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
サイドバーフレーム色取得
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
サイドバースペース背景色取得
*/
/*
AColor	AApp::SPI_GetSideBarEmptySpaceColor() const
{
	//やっぱり古い印象を受けるのでやめるreturn AColorWrapper::GetColorByHTMLFormatColor("a8acb1");
	return AColorWrapper::GetColorByHTMLFormatColor("e9eef4");
}
*/

/**
サブウインドウヘッダー部分背景色取得
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
サブウインドウヘッダー部分文字色取得
*/
/*#1316
AColor	AApp::SPI_GetSubWindowHeaderLetterColor() const
{
	return mSkinColor_SubWindowHeaderLetterColor;
}
*/

/**
サブウインドウ用タイトルバー文字色取得
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
サブウインドウ用タイトルバー文字ドロップシャドウ色取得
*/
/*#1316
AColor	AApp::SPI_GetSubWindowTitlebarTextDropShadowColor() const
{
	return mSkinColor_SubWindowTitlebarTextDropShadowColor;
}
*/

/**
サブウインドウタイトルのボタンホバー色取得
*/
AColor	AApp::SPI_GetSubWindowTitlebarButtonHoverColor() const
{
	return AColorWrapper::GetColorByHTMLFormatColor("778c9c");
}

/**
ポップアップサブウインドウα取得
*/
AFloatNumber	AApp::SPI_GetPopupWindowAlpha() const
{
	AFloatNumber	alpha = NVI_GetAppPrefDBConst().GetData_Number(AAppPrefDB::kSubWindowsAlpha_Popup);
	return alpha/100;
}

/**
フローティングサブウインドウα取得
*/
AFloatNumber	AApp::SPI_GetFloatingWindowAlpha() const
{
	AFloatNumber	alpha = NVI_GetAppPrefDBConst().GetData_Number(AAppPrefDB::kSubWindowsAlpha_Floating);
	return alpha/100;
}

/**
サブウインドウタイトルバー高さ取得
*/
ANumber	AApp::SPI_GetSubWindowTitleBarHeight() const
{
	return 19;
}

/**
サブウインドウ分割線高さ取得
*/
ANumber	AApp::SPI_GetSubWindowSeparatorHeight() const
{
	return 11;
}

/**
サブウインドウ最小高さ取得
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
全てのサブウインドウ表示更新
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
対象サブウインドウかどうかを判定する
ポップアップ・フローティングウインドウは常に対象。オーバーレイウインドウは指定text windowに表示されている場合のみ対象。
*/
ABool	AApp::JudgeIfTargetSubWindowByTextWindowID( const AWindowID inSubWindowID, const AWindowID inTextWindowID ) 
{
	//サブウインドウ検索
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return false;}
	
	//ポップアップ・フローティングウインドウは常に対象。オーバーレイウインドウは指定text windowに表示されている場合のみ対象。
	switch(mSubWindowArray_SubWindowLocationType.Get(index))
	{
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//サブウインドウの属するテキストウインドウが、指定ウインドウと一致していたら対象とする。
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
			//常に対象。
			return true;
			break;
		}
	}
}

/**
対象サブウインドウかどうかを判定する
ポップアップ・フローティングウインドウは常に対象。オーバーレイウインドウは指定text documentが表示されている場合のみ対象。
*/
ABool	AApp::JudgeIfTargetSubWindowByTextDocumentID( const AWindowID inSubWindowID, const ADocumentID inTextDocumentID ) 
{
	//サブウインドウ検索
	AIndex	index = mSubWindowArray_WindowID.Find(inSubWindowID);
	if( index == kIndex_Invalid )   {_ACError("",NULL);return false;}
	
	//ポップアップ・フローティングウインドウは常に対象。オーバーレイウインドウは指定text documentが表示されている場合のみ対象。
	switch(mSubWindowArray_SubWindowLocationType.Get(index))
	{
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//サブウインドウの属するテキストウインドウの現在のドキュメントが、指定ドキュメントと一致していたら対象とする。
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
			//常に対象。
			return true;
			break;
		}
	}
}

/**
各サブウインドウのモード関連データを削除する
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
				//★
				break;
			}
		  case kSubWindowType_WordsList:
			{
				SPI_GetWordsListWindow(winID).SPI_GetWordsListView().SPI_DeleteAllData();
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
}

/**
フローティングサブウインドウ生成
*/
AWindowID	AApp::SPI_CreateFloatingSubWindow( const ASubWindowType inSubWindowType, const ANumber inParameter, 
			const ANumber inWidth, const ANumber inHeight )
{
	//サブウインドウ生成
	AWindowID	winID = SPI_CreateSubWindow(inSubWindowType,inParameter,kObjectID_Invalid,kSubWindowLocationType_Floating,kIndex_Invalid,false);
	if( winID == kObjectID_Invalid )
	{
		return kObjectID_Invalid;
	}
	//ウインドウ配置
	AGlobalRect	screenRect = {0};
	AWindow::NVI_GetAvailableWindowBoundsInMainScreen(screenRect);
	ARect	rect = {0};
	rect.left		= screenRect.left + 16;
	rect.top		= screenRect.top + 16;
	rect.right		= rect.left + inWidth;
	rect.bottom		= rect.top + inHeight;
	NVI_GetWindowByID(winID).NVI_SetWindowBounds(rect);
	//ウインドウ生成・表示
	NVI_GetWindowByID(winID).NVI_CreateAndShow(false);
	//
	return winID;
}

/**
フローティングサブウインドウ一括表示・非表示切替
*/
void	AApp::SPI_ShowHideFloatingSubWindowsTemporary()
{
	//表示・非表示切替
	mShowFloatingSubWindow = !mShowFloatingSubWindow;
	//フローティングウインドウ表示状態更新 #959
	SPI_UpdateFloatingSubWindowsVisibility();
	//ツールバーの表示更新
	SPI_UpdateToolbarItemValueAll();
}

//#959
/**
フローティングウインドウ表示状態更新
*/
void	AApp::SPI_UpdateFloatingSubWindowsVisibility()
{
	ABool	shouldShow = mShowFloatingSubWindow;
	//最前面のウインドウのタイプがテキストウインドウ以外のときはフローティングウインドウは表示しない。
	AWindowID	winID = NVI_GetMostFrontWindowID(kWindowType_Invalid,false);
	if( winID != kObjectID_Invalid )
	{
		if( NVI_GetWindowByID(winID).NVI_GetWindowType() != kWindowType_Text )
		{
			shouldShow = false;
		}
	}
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Floating &&
			mSubWindowArray_SubWindowType.Get(index) != kSubWindowType_FindResult )//#1336 検索結果ウインドウは表示状態更新対象外とする
		{
			//表示・非表示設定
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
フローティングサブウインドウをapp prefに従って開く
*/
void	AApp::ReopenFloatingSubWindows()
{
	//各設定毎のループ
	AItemCount	itemCount = NVI_GetAppPrefDB().GetItemCount_Array(AAppPrefDB::kFloatingSubWindowArray_Type);
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//ウインドウ生成
		ASubWindowType	type = (ASubWindowType)(NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Type,i));
		ANumber	param = NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Parameter,i);
		AWindowID	winID = SPI_CreateSubWindow(type,param,kObjectID_Invalid,kSubWindowLocationType_Floating,kIndex_Invalid,false);
		if( winID != kObjectID_Invalid )
		{
			//表示位置取得、設定
			ARect	rect = {0};
			rect.left 	= NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_X,i);
			rect.top 	= NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Y,i);
			rect.right	= rect.left + NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Width,i);
			rect.bottom	= rect.top  + NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFloatingSubWindowArray_Height,i);
			NVI_GetWindowByID(winID).NVI_SetWindowBounds(rect);
			//位置補正
			NVI_GetWindowByID(winID).NVI_ConstrainWindowPosition(false);
			//表示
			NVI_GetWindowByID(winID).NVI_Show(false);
		}
		//現在のパスを設定
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
				//処理なし
				break;
			}
		}
	}
}

/**
フローティングサブウインドウをapp prefに保存
*/
void	AApp::SaveFloatingSubWindows()
{
	//データ削除
	NVI_GetAppPrefDB().DeleteAllRows_Table(AAppPrefDB::kFloatingSubWindowArray_Type);
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		if( mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Floating &&
					mSubWindowArray_SubWindowType.Get(index) != kSubWindowType_JumpList &&
					mSubWindowArray_SubWindowType.Get(index) != kSubWindowType_FindResult )
		{
			//ウインドウのデータ取得
			AWindowID	winID = mSubWindowArray_WindowID.Get(index);
			ANumber	param = SPI_GetSubWindowParameter(winID);
			ANumber	type = mSubWindowArray_SubWindowType.Get(index);
			ARect	rect = {0};
			NVI_GetWindowByID(winID).NVI_GetWindowBounds(rect);
			//現在のパスを取得
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
					//処理なし
					break;
				}
			}
			//DBに設定
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
フローティングサブウインドウの吸着
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
			//上下吸着
			if( (ioWindowRect.left >= rect.left && ioWindowRect.left <= rect.right) ||
						(ioWindowRect.right >= rect.left && ioWindowRect.right <= rect.right) )
			{
				ABool	docked = false;
				//上吸着
				if( abs(ioWindowRect.bottom - rect.top) < 10 )
				{
					ioWindowRect.top = rect.top - (ioWindowRect.bottom-ioWindowRect.top);
					ioWindowRect.bottom = rect.top;
					//
					docked = true;
				}
				//下吸着
				if( abs(ioWindowRect.top - rect.bottom) < 10 )
				{
					ioWindowRect.bottom = rect.bottom + (ioWindowRect.bottom-ioWindowRect.top);
					ioWindowRect.top = rect.bottom;
					//
					docked = true;
				}
				//左右端吸着
				if( docked == true )
				{
					//右端吸着
					if( abs(ioWindowRect.right - rect.right) < 10 )
					{
						ioWindowRect.left = rect.right - (ioWindowRect.right-ioWindowRect.left);
						ioWindowRect.right = rect.right;
					}
					//左端吸着
					if( abs(ioWindowRect.left - rect.left) < 10 )
					{
						ioWindowRect.right = rect.left + (ioWindowRect.right-ioWindowRect.left);
						ioWindowRect.left = rect.left;
					}
				}
			}
			//左右吸着
			if( (ioWindowRect.top >= rect.top && ioWindowRect.top <= rect.bottom) ||
						(ioWindowRect.bottom >= rect.top && ioWindowRect.bottom <= rect.bottom) )
			{
				ABool	docked = false;
				//左吸着
				if( abs(ioWindowRect.right - rect.left) < 10 )
				{
					ioWindowRect.left = rect.left - (ioWindowRect.right-ioWindowRect.left);
					ioWindowRect.right = rect.left;
					//
					docked = true;
				}
				//上吸着
				if( abs(ioWindowRect.left - rect.right) < 10 )
				{
					ioWindowRect.right = rect.right + (ioWindowRect.right-ioWindowRect.left);
					ioWindowRect.left = rect.right;
					//
					docked = true;
				}
				//上下端吸着
				if( docked == true )
				{
					//下端吸着
					if( abs(ioWindowRect.bottom - rect.bottom) < 10 )
					{
						ioWindowRect.top = rect.bottom - (ioWindowRect.bottom-ioWindowRect.top);
						ioWindowRect.bottom = rect.bottom;
					}
					//上端吸着
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
フローティングサブウインドウのz-orderの補正
下のほうのウインドウほど前面に来るようにする
*/
void	AApp::SPI_AdjustAllFlotingWindowsZOrder()
{
	//z-order順にこの変数に格納していく
	AArray<AWindowID>	floatingWindowOrderArray;
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		if( mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Floating )
		{
			AWindowID	winID = mSubWindowArray_WindowID.Get(index);
			//サブウインドウがフローティング、かつ、表示中なら処理を行う
			if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
			{
				//bounds取得
				ARect	rect = {0};
				NVI_GetWindowConstByID(winID).NVI_GetWindowBounds(rect);
				
				//floatingWindowOrderArrayの中で最初にy位置が大きくなる項目の位置に追加
				//（y位置が小さい順に並べる）
				AItemCount	orderItemCount = floatingWindowOrderArray.GetItemCount();
				AIndex	insertIndex = orderItemCount;
				for( AIndex j = 0; j < orderItemCount; j++ )
				{
					AWindowID	w = floatingWindowOrderArray.Get(j);
					ARect	r = {0};
					NVI_GetWindowConstByID(w).NVI_GetWindowBounds(r);
					//最初にy位置が大きくなる項目indexを取得
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
	//上で取得したz-orderに従って、順に最前面にする
	for( AIndex index = 0; index < floatingWindowOrderArray.GetItemCount(); index++ )
	{
		NVI_GetWindowByID(floatingWindowOrderArray.Get(index)).NVI_SendAbove(kObjectID_Invalid);
	}
}

/**
項目選択アクティブサブウインドウを設定（cmd+option+↑↓の対象ウインドウを設定）
*/
void	AApp::SPI_SetActiveSubWindowForItemSelector( const AWindowID inWindowID )
{
	//項目選択アクティブサブウインドウを設定
	mActiveSubWindowForItemSelector = inWindowID;
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		switch(mSubWindowArray_SubWindowType.Get(index) )
		{
		  case kSubWindowType_WordsList:
		  case kSubWindowType_CallGraf:
		  case kSubWindowType_FindResult:
			{
				//ウインドウ表示中なら描画更新（選択色を変えるため）
				AWindowID	winID = mSubWindowArray_WindowID.Get(index);
				if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
				{
					NVI_GetWindowByID(winID).NVI_RefreshWindow();
				}
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
}

/**
項目選択アクティブウインドウにて、次の項目へ移動
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
				//処理なし
				break;
			}
		}
	}
}

/**
項目選択アクティブウインドウにて、前の項目へ移動
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
				//処理なし
				break;
			}
		}
	}
}

/**
各サブウインドウロック解除
*/
void	AApp::SPI_ClearAllLockOfSubWindows()
{
	//各サブウインドウ毎のループ
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
				//処理なし
				break;
			}
		}
	}
}

#pragma mark ===========================

#pragma mark --- AWindow_CallGraf

/**
CallGrafウインドウに現在functionを設定
*/
void	AApp::SPI_SetCallGrafCurrentFunction( const AWindowID inTextWindowID, const AFileAcc& inProjectFolder,
		const AIndex inModeIndex, const AIndex inCategoryIndex, 
		const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
		const AIndex inStartIndex, const AIndex inEndIndex, const ABool inByEdit )
{
	//各CallGrafウインドウのうち、TextWindowIDが位置するもの（オーバーレイ）、および、TextWindowIDが未設定（フローティング）のものに適用
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
					//現在functionを設定
					SPI_GetCallGrafWindow(winID).SPI_GetCallGrafView().SPI_SetCurrentFunction(inProjectFolder,
								inModeIndex,inCategoryIndex,
								inFilePath,inClassName,inFunctionName,inStartIndex,inEndIndex,inByEdit);
				}
			}
		}
	}
}

/**
CallGrafウインドウに現在functionを設定
*/
void	AApp::SPI_SetCallGrafEditFlag( const AWindowID inTextWindowID, 
									  const AText& inFilePath, const AText& inClassName, const AText& inFunctionName )
{
	//各CallGrafウインドウのうち、TextWindowIDが位置するもの（オーバーレイ）、および、TextWindowIDが未設定（フローティング）のものに適用
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
					//現在functionを設定
					SPI_GetCallGrafWindow(winID).SPI_GetCallGrafView().SPI_SetEditFlag(inFilePath,inClassName,inFunctionName);
				}
			}
		}
	}
}

/**
IdInfoによるジャンプ時CallGrafに通知
*/
void	AApp::SPI_NotifyToCallGrafByIdInfoJump( const AText& inCallerFuncIdText, 
		const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
		const AIndex inStartIndex, const AIndex inEndIndex )
{
	//各CallGrafウインドウに通知
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
ドキュメント編集を各CallGrafに通知
*/
void	AApp::SPI_NotifyToCallGrafByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	//各CallGrafウインドウに通知
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
CallGrafウインドウにインポートファイルprogress表示・非表示
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
CallGrafウインドウの現在の検索を中止する
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
ワーズリストウインドウに単語検索を要求
@note ワーズリストウインドウがなければ何もしない
*/
void	AApp::SPI_RequestWordsList( const AWindowID inTextWindowID, const AFileAcc& inProjectFolder, const AIndex inModeIndex,
		const AText& inWord )
{
	//各ワーズリストウインドウのうち、TextWindowIDが位置するもの（オーバーレイ）、および、TextWindowIDが未設定（フローティング）のものに適用
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
					//単語検索を要求
					SPI_GetWordsListWindow(winID).SPI_GetWordsListView().SPI_RequestWordsList(inProjectFolder,inModeIndex,inWord);
				}
			}
		}
	}
}

/**
ワーズリストウインドウにimport fileプログレスを表示・非表示
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
ドキュメント編集を各ワーズリストウインドウに通知
*/
void	AApp::SPI_NotifyToWordsListByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	//各wordslistウインドウに通知
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
ワーズリストウインドウの現在の検索を中止
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

#pragma mark ---ワーズリスト
//#723

/**
指定プロジェクト、モードに属する全てのファイルのImportFileDataを取得（ImportFileDataオブジェクト未存在なら生成）
@note thread-safe
*/
void	AApp::SPI_GetImportFileDataIDArrayForWordsList( const AFileAcc& inProjectFolder, const AIndex inModeIndex, 
			AArray<AObjectID>& outImportFileDataIDArray, const ABool& inAbort ) 
{
	//結果初期化
	outImportFileDataIDArray.DeleteAll();
	ATextArray	filePathArray;
	//プロジェクトフォルダからプロジェクト取得
	//（project arrayの要素が削除されることはないので、ここはmutex不要。
	AIndex	projectIndex = SPI_GetSameProjectArrayIndex(inProjectFolder);
	if( projectIndex == kIndex_Invalid )
	{
		//------------------プロジェクト無しの場合------------------
		//プロジェクト無しの場合、同じフォルダ内のファイルを非再帰的に取得 #908
		AObjectArray<AFileAcc>	children;
		inProjectFolder.GetChildren(children);
		//inProjectFolderフォルダ内の各ファイルごとのループ
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			AFileAcc	file = children.GetObjectConst(i);
			if( file.IsFolder() == false )
			{
				AText	path;
				file.GetPath(path);
				//ファイルパスを取得
				filePathArray.Add(path);
			}
		}
	}
	else
	{
		//------------------プロジェクト有りの場合------------------
		//プロジェクト内のファイル毎のループ
		{
			//#723
			AStMutexLocker	locker(mSameProjectArrayMutex);
			
			//プロジェクト内ファイル毎のループ
			for( AIndex i = 0; i < mSameProjectArray_Pathname.GetObjectConst(projectIndex).GetItemCount(); i++ )
			{
				//ファイルのモードが一致する場合のみ、ImportFileDataオブジェクト取得
				AIndex	modeIndex = mSameProjectArray_ModeIndex.GetObjectConst(projectIndex).Get(i);
				if( inModeIndex == modeIndex )
				{
					//ファイルパス取得
					AText	path;
					mSameProjectArray_Pathname.GetObjectConst(projectIndex).Get(i,path);
					filePathArray.Add(path);
				}
			}
		}
	}
	//上で取得したファイルパスリスト項目毎のループ
	for( AIndex i = 0; i < filePathArray.GetItemCount(); i++ )
	{
		//abortされたらbreak
		if( inAbort == true )
		{
			break;
		}
		//ファイル取得
		AFileAcc	file;
		file.Specify(filePathArray.GetTextConst(i));
		//プロジェクト内の全てのファイルについて、ImportFileDataオブジェクトを生成する。
		//ただし、この時点で生成されていないものについては、TextInfo内のワードリストは空になる
		AObjectID	importFileDataID = SPI_GetImportFileManager().FindOrLoadImportFileDataByFile(file,false);
		if( importFileDataID != kObjectID_Invalid )
		{
			outImportFileDataIDArray.Add(importFileDataID);
		}
	}
}

//#723
/**
プロジェクト内のいちファイルからプロジェクトを検索
*/
/*#723
AIndex	AApp::SPI_FindProjectIndex( const AFileAcc& inFile ) const
{
	//パス取得
	AText	path;
	inFile.GetPath(path);
	//プロジェクト内各ファイルと比較
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
プレビューワにURLロード要求
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
					//プレビューワーにロード要求
					SPI_GetPreviewerWindow(winID).SPI_RequestLoadURL(inURL);
				}
			}
		}
	}
}

/**
プレビューワにURLリロード要求（最後の要求URLをリロード）
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
					//プレビューワーにリロード要求
					SPI_GetPreviewerWindow(winID).SPI_RequestReload();
				}
			}
		}
	}
}

#if 0
/**
プレビューワーでJavaScript実行
*/
void	AApp::SPI_ExecuteJavaScriptInPreviewerWindow( const AWindowID inTextWindowID, const AText& inText )
{
	//各Previewerウインドウのうち、TextWindowIDが位置するもの（オーバーレイ）、および、TextWindowIDが未設定（フローティング）のものに適用
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
プレビューワーウインドウが存在しているかどうかを取得
*/
ABool	AApp::SPI_PreviewWindowExist( const AWindowID inTextWindowID ) const
{
	//各サブウインドウ毎のループ
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
クリップボード履歴に追加
*/
void	AApp::SPI_AddClipboardHistory( const AText& inText )
{
	//#861
	//履歴を記憶しないモードなら何もしない
	if( NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )
	{
		return;
	}
	//クリップボード履歴記憶
	mClipboardHistory.Insert1(0,inText);
	//各サブウインドウに反映
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
テキストマーカー　現在グループを設定
*/
void	AApp::SPI_SetCurrentTextMarkerGroupIndex( const AIndex inIndex )
{
	//現在グループを設定
	mCurrentTextMarkerGroupIndex = inIndex;
	//テキストマーカーを全てのドキュメントに適用
	SPI_ApplyTextMarkerForAllDocuments();
	//すべてのテキストマーカー設定ウインドウに適用
	UpdateTextMarkerWindows();
}

/**
テキストマーカー　現在グループからデータ取得
*/
void	AApp::SPI_GetCurrentTextMarkerData( AText& outTitle, AText& outText ) 
{
	//データ取得
	NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kTextMarkerArray_Title,mCurrentTextMarkerGroupIndex,outTitle);
	NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kTextMarkerArray_Text,mCurrentTextMarkerGroupIndex,outText);
}

/**
テキストマーカー　現在グループにテキストデータ設定
*/
void	AApp::SPI_SetCurrentTextMarkerData_Text( const AText& inText )
{
	//現在のデータ取得し、同じなら何もしない
	AText	text;
	NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kTextMarkerArray_Text,mCurrentTextMarkerGroupIndex,text);
	if( inText.Compare(text) == true )
	{
		return;
	}
	//現在グループにテキストデータ設定
	NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kTextMarkerArray_Text,mCurrentTextMarkerGroupIndex,inText);
	//テキストマーカーを全てのドキュメントに適用
	SPI_ApplyTextMarkerForAllDocuments();
	//すべてのテキストマーカー設定ウインドウに反映
	UpdateTextMarkerWindows();
}

/**
テキストマーカー　現在グループにタイトル設定
*/
void	AApp::SPI_SetCurrentTextMarkerData_Title( const AText& inTitle )
{
	//現在のデータ取得し、同じなら何もしない
	AText	text;
	NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kTextMarkerArray_Title,mCurrentTextMarkerGroupIndex,text);
	if( inTitle.Compare(text) == true )
	{
		return;
	}
	//現在グループにタイトル設定
	NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kTextMarkerArray_Title,mCurrentTextMarkerGroupIndex,inTitle);
	//テキストマーカーグループメニュー更新
	UpdateTextMarkerMenu();
	//すべてのテキストマーカー設定ウインドウに反映
	UpdateTextMarkerWindows();
}

/**
テキストマーカーを全てのドキュメントに適用
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
テキストマーカーグループを新規生成
*/
void	AApp::SPI_CreateNewTextMarkerGroup()
{
	//------------------グループ新規生成------------------
	AText	title, title0;
	title.SetLocalizedText("TextMarkerNewGroup");
	title0.SetText(title);
	//------------------ユニークなグループ名を取得------------------
	//同じタイトルのグループがあるかどうかを判定
	if( NVI_GetAppPrefDB().Find_TextArray(AAppPrefDB::kTextMarkerArray_Title,title0) != kIndex_Invalid )
	{
		//同じタイトルのグループが既に存在していたら、"-2"から順に、同じタイトルが存在しなくなるような数字文字列を追加する
		for( ANumber num = 2; num < 9999; num++ )
		{
			//ハイフォン＋数字を追加
			title.SetText(title0);
			title.AddCstring("-");
			title.AddNumber(num);
			//同じタイトルのグループが存在していなければループ終了
			if( NVI_GetAppPrefDB().Find_TextArray(AAppPrefDB::kTextMarkerArray_Title,title) == kIndex_Invalid )
			{
				break;
			}
		}
	}
	//グループ追加
	mCurrentTextMarkerGroupIndex = NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kTextMarkerArray_Title,title);
	NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kTextMarkerArray_Text,GetEmptyText());
	//テキストマーカーを全てのドキュメントに適用
	SPI_ApplyTextMarkerForAllDocuments();
	//テキストマーカーグループメニュー更新
	UpdateTextMarkerMenu();
	//すべてのテキストマーカー設定ウインドウに反映
	UpdateTextMarkerWindows();
}

/**
テキストマーカーグループを削除
*/
void	AApp::SPI_DeleteCurrentTextMarkerGroup()
{
	//最後１つのハイライトグループは削除不可
	//if( mCurrentTextMarkerGroupIndex == 0 )
	if( GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kTextMarkerArray_Text) <= 1 )
	{
		return;
	}
	
	//現在のグループ削除
	NVI_GetAppPrefDB().Delete1_TextArray(AAppPrefDB::kTextMarkerArray_Title,mCurrentTextMarkerGroupIndex);
	NVI_GetAppPrefDB().Delete1_TextArray(AAppPrefDB::kTextMarkerArray_Text,mCurrentTextMarkerGroupIndex);
	mCurrentTextMarkerGroupIndex = 0;
	//テキストマーカーを全てのドキュメントに適用
	SPI_ApplyTextMarkerForAllDocuments();
	//テキストマーカーグループメニュー更新
	UpdateTextMarkerMenu();
	//すべてのテキストマーカー設定ウインドウに反映
	UpdateTextMarkerWindows();
}

/**
すべてのテキストマーカー設定ウインドウに反映
*/
void	AApp::UpdateTextMarkerWindows()
{
	//各サブウインドウに反映
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
テキストマーカー設定ウインドウ選択（なければ生成）
*/
void	AApp::SPI_SelectOrCreateTextMarkerWindow()
{
	//サブウインドウ検索
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
	//上記で見つからなければ、生成
	SPI_CreateFloatingSubWindow(kSubWindowType_TextMarker,0,300,180);//#1316 幅変更
}

/**
テキストマーカーメニューを更新
*/
void	AApp::UpdateTextMarkerMenu()
{
	//テキストマーカータイトルデータ取得
	ATextArray	textArray;
	textArray.SetFromTextArray(NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kTextMarkerArray_Title));
	//セパレータ
	AText	text;
	text.SetCstring("-");
	textArray.Add(text);
	//「設定」メニュー項目追加
	text.SetLocalizedText("TextMarker_Setup");
	textArray.Add(text);
	//TextArrayメニュー（コントロール上のメニュー）の更新
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().UpdateTextArrayMenu(kTextArrayMenuID_TextMarker,textArray);
}

#pragma mark ===========================

#pragma mark --- AWindow_DocInfo
//#142

/**
DocInfo（テキストカウント）全文字数・単語数更新
*/
void	AApp::SPI_UpdateDocInfoWindows_TotalCount( const ADocumentID inDocumentID, 
		const AItemCount inTotalCharCount, const AItemCount inTotalWordCount )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#1312 if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_DocInfo )
			{
				//対象サブウインドウかどうか判定。（フローティングは常に対象。オーバーレイは属するテキストウインドウの現在ドキュメントが一致していれば対象。）
				if( JudgeIfTargetSubWindowByTextDocumentID(winID,inDocumentID) == true )
				{
					//全文字数・単語数反映
					SPI_GetDocInfoWindow(winID).SPI_UpdateDocInfo_TotalCount(inTotalCharCount,inTotalWordCount,
								SPI_GetTextDocumentByID(inDocumentID).SPI_GetParagraphCount());
				}
			}
		}
	}
}

/**
DocInfo（テキストカウント）選択文字数・単語数更新
*/
void	AApp::SPI_UpdateDocInfoWindows_SelectedCount( const AWindowID inTextWindowID, 
													 const AItemCount inSelectedCharCount, const AItemCount inSelectedWordCount, 
													 const AItemCount inSelectedParagraphCount,
													 const AText& inSelectedText )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#1312 if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_DocInfo )
			{
				//対象サブウインドウかどうか判定。（フローティングは常に対象。オーバーレイは属するテキストウインドウの現在ドキュメントが一致していれば対象。）
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					//選択文字数・単語数反映
					SPI_GetDocInfoWindow(winID).
							SPI_UpdateDocInfo_SelectedCount(inSelectedCharCount,inSelectedWordCount,inSelectedParagraphCount,
															inSelectedText);
				}
			}
		}
	}
}

/**
doc infoウインドウ現在文字更新
*/
void	AApp::SPI_UpdateDocInfoWindows_CurrentChar( const AWindowID inTextWindowID, 
		const AUCS4Char inChar, const AUCS4Char inCanonicalDecomp, 
		const ATextArray& inHeaderTextArray, const AArray<AIndex>& inHeaderParagraphIndexArray,
		const AText& inDebugText )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#1312 if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_DocInfo )
			{
				//対象サブウインドウかどうか判定。（フローティングは常に対象。オーバーレイは属するテキストウインドウの現在ドキュメントが一致していれば対象。）
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					//現在文字更新
					SPI_GetDocInfoWindow(winID).SPI_UpdateDocInfo_CurrentChar(inChar,inCanonicalDecomp,inHeaderTextArray,inHeaderParagraphIndexArray,inDebugText);
				}
			}
		}
	}
}

/**
doc infoウインドウ　プラグインからテキスト設定
*/
void	AApp::SPI_UpdateDocInfoWindows_PluginText( const AWindowID inTextWindowID, const AText& inPluginText )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#1312 if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_DocInfo )
			{
				//対象サブウインドウかどうか判定。（フローティングは常に対象。オーバーレイは属するテキストウインドウの現在ドキュメントが一致していれば対象。）
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					//プラグイン用テキスト更新
					SPI_GetDocInfoWindow(winID).SPI_UpdateDocInfo_PluginText(inPluginText);
				}
			}
		}
	}
}

/**
doc infoウインドウが存在しているかどうかを取得
*/
ABool	AApp::SPI_DocInfoWindowExist( const AWindowID inTextWindowID )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		//#1312 if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_DocInfo )
			{
				//対象サブウインドウかどうか判定。（フローティングは常に対象。オーバーレイは属するテキストウインドウの現在ドキュメントが一致していれば対象。）
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
現在開いているドキュメントすべてについてdoc info更新処理を行う
（doc infウインドウをdocumentよりも後に開いた場合にこの処理を行う）
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
ファイルリストを更新
*/
void	AApp::SPI_UpdateFileListWindows( const AFileListUpdateType inType, const ADocumentID inDocumentID )
{
	//各ファイルリストウインドウ毎ループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FileList )
		{
			if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
			{
				//ファイルリスト更新
				SPI_GetFileListWindow(winID).SPI_UpdateFileList(inType,inDocumentID);
			}
		}
	}
}

/**
ファイルリストの描画プロパティ更新
*/
void	AApp::SPI_UpdateFileListWindowsProperty()
{
	//各ファイルリストウインドウ毎ループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FileList )
		{
			if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
			{
				//描画プロパティ更新
				SPI_GetFileListWindow(winID).NVI_UpdateProperty();
			}
		}
	}
}

//#361
/**
ファイルリストにリモートファイル表示時、リモートフォルダを再読込する
*/
void	AApp::SPI_UpdateFileListRemoteFolder( const AText& inFolderURL )
{
	//各ファイルリストウインドウ毎ループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FileList )
		{
			if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
			{
				//リモートフォルダを再読込する
				SPI_GetFileListWindow(winID).SPI_UpdateRemoteFolder(inFolderURL);
			}
		}
	}
}

//#798
/**
指定モードを表示しているファイルリストを検索
*/
AWindowID	AApp::FindFileListWindow( const AWindowID inTextWindowID, const ANumber inMode ) const
{
	//各ファイルリストウインドウ毎ループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_FileList )
		{
			//モード一致チェック
			if( SPI_GetFileListWindowConst(winID).SPI_GetCurrentMode() == inMode )
			{
				//TextWindowID一致チェック
				if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
				{
					//ウインドウ表示中かチェック
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
「同じプロジェクトのファイルを検索」（ファイルリストのフィルタにフォーカス移動）
*/
void	AApp::SPI_SearchInProjectFolder()
{
	//１．最善面のテキストウインドウのオーバーレイウインドウ
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
	//２．テキストウインドウ指定がないもの（＝フローティングウインドウ）から検索
	{
		AWindowID	winID = FindFileListWindow(kObjectID_Invalid,kFileListMode_SameProject);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//３．上記でウインドウが無ければ、新規生成する。
	{
		AWindowID	winID = GetApp().SPI_CreateFloatingSubWindow(kSubWindowType_FileList,kFileListMode_SameProject,250,250);
		SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
		return;
	}
}

//#798
/**
「同じフォルダのファイルを検索」（ファイルリストのフィルタにフォーカス移動）
*/
void	AApp::SPI_SearchInSameFolder()
{
	//１．最善面のテキストウインドウのオーバーレイウインドウ
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
	//２．テキストウインドウ指定がないもの（＝フローティングウインドウ）から検索
	{
		AWindowID	winID = FindFileListWindow(kObjectID_Invalid,kFileListMode_SameFolder);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//３．上記でウインドウが無ければ、新規生成する。
	{
		AWindowID	winID = GetApp().SPI_CreateFloatingSubWindow(kSubWindowType_FileList,kFileListMode_SameFolder,250,250);
		SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
		return;
	}
}

//#798
/**
「最近開いたファイルを検索」（ファイルリストのフィルタにフォーカス移動）
*/
void	AApp::SPI_SearchInRecentlyOpenFiles()
{
	//１．最善面のテキストウインドウのオーバーレイウインドウ
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
	//２．テキストウインドウ指定がないもの（＝フローティングウインドウ）から検索
	{
		AWindowID	winID = FindFileListWindow(kObjectID_Invalid,kFileListMode_RecentlyOpened);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetFileListWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//３．上記でウインドウが無ければ、新規生成する。
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
ポップアップIdInfoウインドウ取得
*/
/*
AWindow_IdInfo&	AApp::SPI_GetPopupIdInfoWindow()
{
	//未生成なら生成
	if( mPopupIdInfoWindowID == kObjectID_Invalid )
	{
		mPopupIdInfoWindowID = SPI_CreateSubWindow(kSubWindowType_IdInfo,0,kObjectID_Invalid,
					kSubWindowLocationType_Popup,kIndex_Invalid);
		SPI_GetPopupIdInfoWindow().NVI_Create(kObjectID_Invalid);
	}
	//IdInfoウインドウ取得
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_IdInfo,kWindowType_IdInfo,mPopupIdInfoWindowID);
}
*/

//#853

/**
IdInfo検索スレッド取得
*/
AThread_IdInfoFinder&	AApp::SPI_GetIdInfoFinderThread()
{
	//未生成なら生成
	if( mIdInfoFinderThreadID == kObjectID_Invalid )
	{
		AThread_IdInfoFinderFactory	factory(this);
		mIdInfoFinderThreadID = NVI_CreateThread(factory);
		NVI_GetThreadByID(mIdInfoFinderThreadID).NVI_Run(true);
	}
	//スレッド取得
	return (dynamic_cast<AThread_IdInfoFinder&>(NVI_GetThreadByID(mIdInfoFinderThreadID)));
}

/**
IdInfo検索スレッド結果
*/
void	AApp::DoIdInfoFinderPaused( const ADocumentID inTextDocumentID,
		const AWindowID inIdInfoWindowID, const AWindowID inTextWindowID )
{
	//検索実行時word, parentwordを取得
	AText	word;
	ATextArray	parentWord;
	SPI_GetIdInfoFinderThread().GetWordForResult(word,parentWord);
	//検索結果取得
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
	//IdInfoウインドウ指定の場合（＝IdInfoウインドウの検索ボックスで検索した場合）
	if( inIdInfoWindowID != kObjectID_Invalid )
	{
		if( NVI_ExistWindow(inIdInfoWindowID) == true )
		{
			//指定IdInfoウインドウへ反映
			SPI_GetIdInfoWindow(inIdInfoWindowID).SPI_GetIdInfoView().SPI_SetIdInfo(word,parentWord,inTextDocumentID,
						keywordTextArray,parentKeywordTextArray,
						typeTextArray,infoTextArray,commentTextArray,categoryIndexArray,colorSlotIndexArray,scopeArray,
						startIndexArray,endIndexArray,filePathArray,argIndex,callerFuncIdText);
		}
	}
	//TextWindow指定の場合（＝Textウインドウ上でIDクリック等した場合）
	else if( inTextWindowID != kObjectID_Invalid )
	{
		if( NVI_ExistWindow(inTextWindowID) == true )
		{
			//各ウインドウ毎のループ
			for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
			{
				AWindowID	winID = mSubWindowArray_WindowID.Get(index);
				if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_IdInfo )
				{
					//指定TextWindowに一致した場合、または、フローティング・ポップアップの場合、そのウインドウに反映
					AWindowID	textWindowID = mSubWindowArray_TextWindowID.Get(index);
					if( inTextWindowID == textWindowID || textWindowID == kObjectID_Invalid )
					{
						//反映
						SPI_GetIdInfoWindow(winID).SPI_GetIdInfoView().SPI_SetIdInfo(word,parentWord,inTextDocumentID,
									keywordTextArray,parentKeywordTextArray,
									typeTextArray,infoTextArray,commentTextArray,categoryIndexArray,colorSlotIndexArray,scopeArray,
									startIndexArray,endIndexArray,filePathArray,argIndex,callerFuncIdText);
					}
				}
			}
			//ポップアップウインドウ表示（SPI_ShowIdInfoPopupWindow()内で、項目が存在しているかどうかの判断を行い、存在している場合のみ表示している）
			SPI_GetTextWindowByID(inTextWindowID).SPI_ShowIdInfoPopupWindow(true);
			//==================親が一致するキーワードを補完表示==================
			if( parentKeywordTextArray.GetItemCount() > 0 )
			{
				SPI_GetTextWindowByID(inTextWindowID).SPI_GetCurrentFocusTextView().SPI_RequestCandidateFromParentKeyword(parentKeywordTextArray);
			}
		}
	}
}

/**
IdInfoウインドウの指定引数indexをハイライト
*/
ABool	AApp::SPI_HighlightIdInfoArgIndex( const AWindowID inTextWindowID ,const AText& inKeyword, const AIndex inArgIndex )
{
	ABool	argHighlighted = false;
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_IdInfo )
			{
				//指定TextWindowに一致した場合、または、フローティング・ポップアップの場合、そのウインドウに反映
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
ポップアップのキーワード情報サブウインドウを閉じる
*/
void AApp::SPI_HideFloatingIdInfoWindow()
{
	//各サブウインドウ毎のループ
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
キーワード検索ウインドウを検索
*/
AWindowID	AApp::SPI_FindIdInfoWindow( const AWindowID inTextWindowID ) const
{
	//各ファイルリストウインドウ毎ループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowConstByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_IdInfo )
			{
				//TextWindowID一致チェック
				if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
				{
					//ウインドウ表示中かチェック
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
「キーワードを検索」（フィルタにフォーカス移動）
*/
void	AApp::SPI_SearchInKeywordList()
{
	//１．最善面のテキストウインドウのオーバーレイウインドウ
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
	//２．テキストウインドウ指定がないもの（＝フローティングウインドウ）から検索
	{
		AWindowID	winID = SPI_FindIdInfoWindow(kObjectID_Invalid);
		if( winID != kObjectID_Invalid )
		{
			SPI_GetIdInfoWindow(winID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//３．上記でウインドウが無ければ、新規生成する。
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
指定WindowID以外のフローティングJumpListを全てHideにする
*/
void	AApp::SPI_HideOtherFloatingJumpListWindows( const AWindowID inWindowID )
{
	//各サブウインドウ毎のループ
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
ジャンプリストウインドウにtabを生成
*/
void	AApp::SPI_CreateJumpListWindowsTab( const AWindowID inTextWindowID, const ADocumentID inDocumentID )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_JumpList )
		{
			//指定text windowに属するジャンプリストウインドウならタブ生成
			if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
			{
				SPI_GetJumpListWindow(winID).NVI_CreateTab(inDocumentID);
			}
		}
	}
}

/**
ジャンプリストウインドウのtabを閉じる
*/
void	AApp::SPI_CloseJumpListWindowsTab( const AWindowID inTextWindowID, const ADocumentID inDocumentID )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_JumpList )
		{
			//指定text windowに属するジャンプリストウインドウなら、タブ削除
			if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
			{
				AIndex	tabIndex = SPI_GetJumpListWindow(winID).NVI_GetTabIndexByDocumentID(inDocumentID);
				SPI_GetJumpListWindow(winID).NVI_DeleteTabAndView(tabIndex);
			}
		}
	}
}

/**
ジャンプリストウインドウのtabを選択
*/
void	AApp::SPI_SelectJumpListWindowsTab( const AWindowID inTextWindowID, const ADocumentID inDocumentID )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_JumpList )
			{
				//指定text windowに属するジャンプリストウインドウなら、タブ選択
				if( mSubWindowArray_TextWindowID.Get(index) == inTextWindowID )
				{
					//タブ選択
					AIndex	tabIndex = SPI_GetJumpListWindow(winID).NVI_GetTabIndexByDocumentID(inDocumentID);
					if( tabIndex != SPI_GetJumpListWindow(winID).NVI_GetCurrentTabIndex() )
					{
						SPI_GetJumpListWindow(winID).NVI_SelectTab(tabIndex);
						//ジャンプリストウインドウのタイトルバー更新（ファイル名を表示）
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
ジャンプリストウインドウのタイトルを設定
*/
void	AApp::SPI_SetJumpListWindowTitle( const AWindowID inTextWindowID, const AText& inTitle )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_JumpList )
			{
				//指定text windowに属するジャンプリストウインドウなら、タイトル設定
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
ポップアップの候補リストウインドウ取得
*/
/*
AWindow_CandidateList&	AApp::SPI_GetPopupCandidateWindow()
{
	//未生成なら生成
	if( mPopupCandidateWindowID == kObjectID_Invalid )
	{
		mPopupCandidateWindowID = SPI_CreateSubWindow(kSubWindowType_CandidateList,0,kObjectID_Invalid,
					kSubWindowLocationType_Popup,kIndex_Invalid);
		SPI_GetPopupCandidateWindow().NVI_Create(kObjectID_Invalid);
	}
	//ポップアップウインドウ生成
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_CandidateList,kWindowType_CandidateList,mPopupCandidateWindowID);
}
*/

/**
候補リストウインドウへ候補検索結果反映
*/
void	AApp::SPI_UpdateCandidateListWindows( const AModeIndex inModeIndex,
		const AWindowID inTextWindowID, const AControlID inTextViewControlID,
		const ATextArray& inIdTextArray, const ATextArray& inInfoTextArray,
		const AArray<AIndex>& inCategoryIndexArray, const AArray<AScopeType>& inScopeTypeArray, 
		const ATextArray& inFilePathArray, const AArray<ACandidatePriority>& inPriorityArray,
		const ATextArray& inParentKeywordArray,
		const AArray<AItemCount>& inMatchedCountArray )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true ||
					mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Popup )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CandidateList )
			{
				//対象ウインドウなら反映
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
候補リストウインドウの項目選択
*/
void	AApp::SPI_SetCandidateListWindowsSelectedIndex( const AWindowID inTextWindowID, const AIndex inSelectedIndex )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CandidateList )
			{
				//対象ウインドウなら項目選択
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					SPI_GetCandidateListWindow(winID).SPI_SetIndex(inSelectedIndex);
				}
			}
		}
	}
}

/**
候補リストウインドウの項目クリックを無効化
*/
void	AApp::SPI_InvalidateCandidateListWindowClick( const AWindowID inTextWindowID )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_CandidateList )
			{
				//対象ウインドウなら項目選択
				if( JudgeIfTargetSubWindowByTextWindowID(winID,inTextWindowID) == true )
				{
					SPI_GetCandidateListWindow(winID).SPI_GetCandidateListView().SPI_InvalidateClick();
				}
			}
		}
	}
}

/**
候補検索スレッド取得
*/
AThread_CandidateFinder&	AApp::SPI_GetCandidateFinderThread()
{
	//未生成なら生成
	if( mCandidateFinderThreadID == kObjectID_Invalid )
	{
		AThread_CandidateFinderFactory	factory(this);
		mCandidateFinderThreadID = NVI_CreateThread(factory);
		NVI_GetThreadByID(mCandidateFinderThreadID).NVI_Run(true);
	}
	//取得
	return (dynamic_cast<AThread_CandidateFinder&>(NVI_GetThreadByID(mCandidateFinderThreadID)));
}

#pragma mark ===========================

#pragma mark --- AWindow_KeyToolList
//#725

/**
ツールリストにデータ設定
*/
void	AApp::SPI_SetKeyToolListInfo( const AWindowID inTextWindowID, const AControlID inTextViewControlID,
									 const AGlobalPoint inPoint, const ANumber inLineHeight, 
									 const ATextArray& inTextArray, const ABoolArray& inIsUserToolArray )
{
	//各サブウインドウ毎のループ
	for( AIndex index = 0; index < mSubWindowArray_WindowID.GetItemCount(); index++ )
	{
		AWindowID	winID = mSubWindowArray_WindowID.Get(index);
		if( NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true ||
					mSubWindowArray_SubWindowLocationType.Get(index) == kSubWindowLocationType_Popup )
		{
			if( mSubWindowArray_SubWindowType.Get(index) == kSubWindowType_KeyToolList )
			{
				//対象ウインドウなら項目選択
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
検索結果ウインドウ取得（なければ生成）
*/
ADocumentID	AApp::SPI_GetOrCreateFindResultWindowDocument()
{
	//検索結果ウインドウを検索
	AWindowID	winID = SPI_GetCurrentFindResultWindowID();
	if( winID == kObjectID_Invalid )
	{
		//検索結果ウインドウが無ければ生成する
		winID = SPI_CreateSubWindow(kSubWindowType_FindResult,0,kObjectID_Invalid,kSubWindowLocationType_Floating,kIndex_Invalid,false);
		//配置・表示（app pref dbに記憶された位置に表示）
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
	//検索結果フローティングウインドウが非表示だったら表示する（検索ウインドウを表示したときにフローティングウインドウを非表示にしたため）
	if( SPI_GetFindResultWindow(winID).NVI_IsWindowVisible() == false )
	{
		SPI_GetFindResultWindow(winID).NVI_Show(false);
	}
	return SPI_GetFindResultWindow(winID).SPI_GetDocumentID();
}

/**
検索結果ウインドウ検索
*/
AWindowID	AApp::SPI_GetCurrentFindResultWindowID()
{
	//------------------まず現在の最前面テキストウインドウのサイドバーから探す------------------
	AWindowID	frontTextWindowID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
	if( frontTextWindowID != kObjectID_Invalid )
	{
		//各サブウインドウ毎のループ
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
								//サイドバー上でおりたたまれていたら、折りたたみを解除する
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
							//処理なし
							break;
						}
					}
				}
			}
		}
	}
	//------------------サイドバーに無ければ、フローティングウインドウを探す------------------
	//各サブウインドウ毎のループ
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
						//処理なし
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
検索結果ウインドウ（フローティングのみ）検索
*/
AWindowID	AApp::SPI_GetFloatingFindResultWindowID()
{
	//各サブウインドウ毎のループ
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
						//処理なし
						break;
					}
				}
			}
		}
	}
	return kObjectID_Invalid;
}

#pragma mark ===========================

#pragma mark --- キー記録
//#725

/**
キー記録ウインドウが未生成なら生成
*/
void	AApp::SPI_ShowKeyRecordWindow()
{
	if( SPI_FindKeyRecordWindow() == kObjectID_Invalid )
	{
		SPI_CreateFloatingSubWindow(kSubWindowType_KeyRecord,0,300,150);//#1316 幅変更
	}
}

/**
キー記録ウインドウを検索
*/
AWindowID	AApp::SPI_FindKeyRecordWindow() const
{
	//各サブウインドウ毎のループ
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
キー記録
*/
void	AApp::AddRecordedText( const AText& inText, const ABool inEscape )
{
	//キー記録
	AText	text;
	if( inEscape == true )
	{
		//エスケープ有りの場合（＝通常テキストの追加）
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
		//エスケープ無しの場合（＝ツールコマンドテキストの追加）
		text.SetText(inText);
	}
	mKeyRecordedText.AddText(text);
	
	//キー記録ウインドウ更新
	//各サブウインドウ毎のループ
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
キー記録（テキスト）
*/
void	AApp::SPI_RecordKeyText( const AText& inText )
{
	//キー記録中でなければ何もせず終了
	if( mKeyRecording == false )
	{
		return;
	}
	
	//キー記録
	AddRecordedText(inText,true);
}

/**
キー記録（キーアクション）
*/
void	AApp::SPI_RecordKeybindAction( const AKeyBindAction inAction, const AText& inText )
{
	//キー記録中でなければ何もせず終了
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
検索の記録
*/
void	AApp::SPI_RecordFindNext( const AFindParameter& inParam )
{
	//キー記録中でなければ何もせず終了
	if( mKeyRecording == false )
	{
		return;
	}
	
	//検索のツールコマンド取得
	AText	text;
	SPI_GetFindParamToolCommand(inParam,text,"FIND-TEXT-NEXT");
	
	//キー記録
	AddRecordedText(text,false);
}

/**
検索の記録
*/
void	AApp::SPI_RecordFindPrevious( const AFindParameter& inParam )
{
	//キー記録中でなければ何もせず終了
	if( mKeyRecording == false )
	{
		return;
	}
	
	//検索のツールコマンド取得
	AText	text;
	SPI_GetFindParamToolCommand(inParam,text,"FIND-TEXT-PREV");
	
	//キー記録
	AddRecordedText(text,false);
}

/**
検索の記録
*/
void	AApp::SPI_RecordFindFromFirst( const AFindParameter& inParam )
{
	//キー記録中でなければ何もせず終了
	if( mKeyRecording == false )
	{
		return;
	}
	
	//検索のツールコマンド取得
	AText	text;
	SPI_GetFindParamToolCommand(inParam,text,"FIND-TEXT-FROMFIRST");
	
	//キー記録
	AddRecordedText(text,false);
}

/**
置換の記録
*/
void	AApp::SPI_RecordReplace( const AFindParameter& inParam, const AReplaceRangeType inReplaceRange )
{
	//キー記録中でなければ何もせず終了
	if( mKeyRecording == false )
	{
		return;
	}
	
	//置換のツールコマンド取得
	AText	text;
	SPI_GetReplaceParamToolCommand(inParam,inReplaceRange,text);
	
	//キー記録
	AddRecordedText(text,false);
}

/**
記録開始
*/
void	AApp::SPI_StartRecord()
{
	//停止中なら記録テキストを全削除し、停止フラグ解除
	if( mStoppedFlag == true )
	{
		mKeyRecordedText.DeleteAll();
		mStoppedFlag = false;
	}
	//キー記録中フラグON
	mKeyRecording = true;
	
	//キー記録ウインドウ更新
	//各サブウインドウ毎のループ
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
ポーズ
*/
void	AApp::SPI_PauseRecord()
{
	//キー記録中フラグOFF
	mKeyRecording = false;
	//停止中フラグOFF（SPI_StartRecord()をコールすると、続きから）
	mStoppedFlag = false;
	
	//キー記録ウインドウ更新
	//各サブウインドウ毎のループ
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
停止
*/
void	AApp::SPI_StopRecord()
{
	//キー記録中フラグOFF
	mKeyRecording = false;
	//停止中フラグON
	mStoppedFlag = true;
	
	//キー記録ウインドウ更新
	//各サブウインドウ毎のループ
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
記録テキスト取得
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

#pragma mark --- EditProgressモーダルセッション
//#846

/**
編集プログレス　モーダルセッション開始
*/
ABool	AApp::SPI_StartEditProgressModalSession( const AEditProgressType inType, const ABool inEnableCancelButton,
		const ABool inShowProgressWindowNow )
{
	//既に開始済みなら何もしない
	if( mEditProgressModalSessionStarted == true )
	{
		return false;
	}
	//パラメータ設定
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
編集プログレス　モーダルセッション終了
*/
void	AApp::SPI_EndEditProgressModalSession()
{
	if( mEditProgressModalSessionStartedInActual == true )
	{
		//OSモーダルセッション終了
		SPI_GetEditProgressWindow().NVI_EndModalSession();
		//
		/*hideだとしばらくしてnotificationが発生する？
		SPI_GetEditProgressWindow().NVI_Hide();
		*/
		//プログレスウインドウ削除
		SPI_GetEditProgressWindow().NVI_Close();
		NVI_DeleteWindow(mEditProgressWindowID);
		mEditProgressWindowID = kObjectID_Invalid;
	}
	//パラメータ初期化
	mEditProgressType = kEditProgressType_None;
	mEditProgressStartDateTime = 0;
	mEditProgressContinueCheckDateTime = 0;
	mEditProgressModalSessionStartedInActual = false;
	mEditProgressModalSessionStarted = false;
	//internal event実行（モーダルセッション中はinternal event実行を抑制しているので）
	//ここでinternal event実行すると、処理実行順番がおかしなことになる可能性がある。（documentのview activated実行途中で、再度view activatedがコールされるなど） EVT_DoInternalEvent();
}

/**
モーダルセッション継続判定
*/
ABool	AApp::SPI_CheckContinueingEditProgressModalSession( const AEditProgressType inType, const AItemCount inCount,
															const ABool inUpdateProgress, const AItemCount inCurrentProgress, const AItemCount inTotalProgress, const ABool inForceShowDialog  )//#1374
{
	//現在時刻取得
	ADateTime	currentDateTime = ADateTimeWrapper::GetCurrentTime();
	//OSモーダルセッション未開始の場合、開始から5秒経っていなければ、無条件で継続と判定し、ここで終了
	if( mEditProgressModalSessionStartedInActual == false )
	{
		if( ADateTimeWrapper::GetDateTimeDiff(currentDateTime,mEditProgressStartDateTime) <= 5 && inForceShowDialog == false )//#1374
		{
			return true;
		}
	}
	
	//OSモーダルセッション未開始の場合、開始する
	if( mEditProgressModalSessionStartedInActual == false )
	{
		ShowProgressWindowAndStartModalSessionInActual();
	}
	//プログレスウインドウ更新
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
	//（16000件置換 置換件数表示有り：66s、表示無し：38s）
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
	
	//1秒おきに継続チェックする。
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
モーダルセッション中かどうかを取得（OS上で実際にモーダルセッションかどうかに関わらず、AAppクラス上での状態を返す）
*/
ABool	AApp::SPI_InEditProgressModalSession() const
{
	return (mEditProgressType != kEditProgressType_None);
}

/**
モーダルセッションウインドウを表示し、OS上のモーダルセッションを開始する
*/
void	AApp::ShowProgressWindowAndStartModalSessionInActual()
{
	AWindow_EditProgress&	win = SPI_GetEditProgressWindow();
	//プログレスウインドウ設定
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
			//処理なし
			break;
		}
	}
	win.SPI_SetText(title,message);
	//プログレスウインドウ表示
	win.NVI_Show(true);
	//OSモーダルセッション開始
	win.NVI_StartModalSession();
	mEditProgressModalSessionStartedInActual = true;
}

#pragma mark ===========================

#pragma mark --- JavaScript実行スレッド
//

/**
JavaScript実行スレッド取得
*/
AThread_JavaScriptExecuter&	AApp::SPI_GetJavaScriptExecuterThread()
{
	//未生成なら生成
	if( mJavaScriptExecuterThreadID == kObjectID_Invalid )
	{
		AThread_JavaScriptExecuterFactory	factory(this);
		mJavaScriptExecuterThreadID = NVI_CreateThread(factory);
	}
	//取得
	return (dynamic_cast<AThread_JavaScriptExecuter&>(NVI_GetThreadByID(mJavaScriptExecuterThreadID)));
}

/**
JavaScript実行
*/
void	AApp::SPI_ExecuteJavaScript( const AText& inScriptText, const AFileAcc& inFolder )
{
	//実行中フラグON
	mExecutingJavaScript = true;
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_JavaScript,true,false,true);
	try
	{
		//JavaScript実行 #1222 JavaScriptはメインスレッドで実行することにする。
		NVI_DoJavaScript(inScriptText,inFolder);
		/*#1222 JavaScriptはメインスレッドで実行することにする。
		//スレッドでJavaScript実行
		SPI_GetJavaScriptExecuterThread().SPNVI_Run_ExecuteJavaScript(inScriptText,inFolder);
		while( SPI_GetJavaScriptExecuterThread().SPI_IsCompleted() == false )
		{
			if( SPI_CheckContinueingEditProgressModalSession(kEditProgressType_JavaScript,0,true,-1,0) == false )
			{
				SPI_GetJavaScriptExecuterThread().NVI_Kill();
				//必ずしもkillできるとは限らないので、ループを抜ける。
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
	//実行中フラグOFF
	mExecutingJavaScript = false;
}

//#1217
/**
JavaScriptダイアログ表示
*/
void	AApp::SPI_ShowJavaScriptModalAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, ABool& outAborted )
{
	NVI_GetWindowByID(mJavaScriptModalAlertWindowID).NVI_Create(kObjectID_Invalid);
	(dynamic_cast<AWindow_JavaScriptModalAlert&>(NVI_GetWindowByID(mJavaScriptModalAlertWindowID))).SPNVI_Show(inMessage1,inMessage2,inOKButtonText,outAborted);
}

//#1217
/**
JavaScriptダイアログ表示
*/
ABool	AApp::SPI_ShowJavaScriptModalCancelAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, const AText& inCancelButtonText, ABool& outAborted )
{
	NVI_GetWindowByID(mJavaScriptModalCancelAlertWindowID).NVI_Create(kObjectID_Invalid);
	return (dynamic_cast<AWindow_JavaScriptModalCancelAlert&>(NVI_GetWindowByID(mJavaScriptModalCancelAlertWindowID))).SPNVI_Show(inMessage1,inMessage2,inOKButtonText,inCancelButtonText,outAborted);
}

#pragma mark ===========================

#pragma mark --- JavaScriptプラグイン

//#994
/**
プラグインのロード（AModePrefDBからコールされる）
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

#pragma mark <所有クラス(AProjectPrefDB)>

#pragma mark ===========================

//ファイルに対応するプロジェクトフォルダを検索する。
void	AApp::SPI_FindProjectFolder( const AFileAcc& inFile, AFileAcc& outProjectFolder, AObjectID& outProjectObjectID, AText& outRelativePath )
{
	//RC3
	outProjectObjectID = kObjectID_Invalid;
	outRelativePath.DeleteAll();
	//
	AFileAcc	parent;
	parent.SpecifyParent(inFile);
	//プロジェクトフォルダ
	AFileAcc	projectFolder;
	ABool	found = false;
	//miproj検索
	AText	miproj("miproj");
	if( FindProjectFolderByFilenameRecursive(parent,miproj,projectFolder) == true )
	{
		found = true;
	}
	else
	{
		//#1284
		//ここにgitフォルダ対応予定
		
		//★
		//svnのルートフォルダをプロジェクトフォルダにしたい→でも、勝手にプロジェクトフォルダにすると、大規模なばあいにメモリが増えすぎる問題もある
	}
	/* if( found == false )
	{
		//xcodeproj検索
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
		//プロジェクトDBオブジェクト取得、または、生成
		AText	projectfolderpath;
		projectFolder.GetPath(projectfolderpath);
		for( AIndex i = 0; i < mProjectPrefDBArray.GetItemCount(); i++ )
		{
			//#138 if( mProjectPrefDBArray.GetObjectConst(i).GetPath().Compare(projectpath) == true )
			//#138 projectpathをプロジェクトフォルダのパスとみなしたり、miprojファイルのパスとみなしたりしていたので修正。
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

//inFolderからフォルダをさかのぼって、inFilenameの名前を持つファイルが存在するフォルダを検索する
ABool	AApp::FindProjectFolderByFilenameRecursive( const AFileAcc& inFolder, const AText& inFilename, AFileAcc& outProjectFolder ) const
{
	/*#298 処理高速化
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
	//inFolder内でinFilenameの名前のファイルが存在するかどうかを調べる
	AFileAcc	file;
	file.SpecifyChild(inFolder,inFilename);
	if( file.Exist() == true )
	{
		outProjectFolder.CopyFrom(inFolder);
		return true;
	}
	//存在していなければ親フォルダで検索（再帰）
	AFileAcc	parent;
	parent.SpecifyParent(inFolder);
	if( parent.IsRoot() == true )
	{
		return false;
	}
	return FindProjectFolderByFilenameRecursive(parent,inFilename,outProjectFolder);
}

//inFolderからフォルダをさかのぼって、inProjectSuffixの拡張子を持つファイルが存在するフォルダを検索する
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
//外部コメント取得
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
//外部コメント設定
//位置は移動しない
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
		//既存のデータがある場合は上書き
		mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).SetData_TextArray(AProjectPrefDB::kExternalComment_Comment,index,inComment);
	}
	else
	{
		//既存のデータがない場合は新規生成
		mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Add_TextArray(AProjectPrefDB::kExternalComment_PathAndModuleName,text);
		mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Add_TextArray(AProjectPrefDB::kExternalComment_Comment,inComment);
	}
	mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Save();
	//ドキュメントへ更新通知
	NotifyExternalCommentUpdatedToDocument(inProjectDBObjectID,inRelativePath);
}

//#138
/**
外部コメント位置更新

外部コメントの位置（モジュール名＋オフセット）を更新する。
@param inProjectDBObjectID ProjectDBのObjectID
@param inRelativePath プロジェクトのルートフォルダからの相対パス
@param inModuleName 旧モジュール名
@param inOffset 旧オフセット
@param inNewModuleName 新モジュール名
@param inNewOffset 新オフセット
*/
void	AApp::SPI_UpdateExternalCommentPosition( const AObjectID inProjectDBObjectID, const AText& inRelativePath, 
		const AText& inModuleName, const AIndex inOffset, const AText& inNewModuleName, const AIndex inNewOffset )
{
	//プロジェクトDBObjectID不正なら何もせずに終了
	if( inProjectDBObjectID == kObjectID_Invalid )   return;
	//旧位置
	AText	text;
	text.SetText(inRelativePath);
	text.Add(kUChar_CR);
	text.AddText(inModuleName);
	text.Add(kUChar_CR);
	text.AddNumber(inOffset);
	//新規位置
	AText	newtext;
	newtext.SetText(inRelativePath);
	newtext.Add(kUChar_CR);
	newtext.AddText(inNewModuleName);
	newtext.Add(kUChar_CR);
	newtext.AddNumber(inNewOffset);
	//新規位置に既に付箋紙が存在する場合は何もせずに終了
	if( mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Find_TextArray(AProjectPrefDB::kExternalComment_PathAndModuleName,newtext) != kIndex_Invalid )   return;
	//位置（モジュール名＋オフセット）更新
	AIndex	index = mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Find_TextArray(AProjectPrefDB::kExternalComment_PathAndModuleName,text);
	if( index != kIndex_Invalid )
	{
		mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).SetData_TextArray(AProjectPrefDB::kExternalComment_PathAndModuleName,index,newtext);
	}
	mProjectPrefDBArray.GetObjectByID(inProjectDBObjectID).Save();
	//ドキュメントへ更新通知
	NotifyExternalCommentUpdatedToDocument(inProjectDBObjectID,inRelativePath);
}

//#138
/**
外部コメント取得（ドキュメント指定、配列へ取得）

ドキュメント内の全レコード（pathに一致するレコード）を全て取得
*/
void	AApp::SPI_GetExternalCommentArrayFromProjectDB( const AObjectID inProjectDBObjectID, const AText& inRelativePath, ATextArray& outModuleNameArray, ANumberArray& outOffsetArray, ATextArray& outCommentArray ) const
{
	//初期化
	outModuleNameArray.DeleteAll();
	outOffsetArray.DeleteAll();
	outCommentArray.DeleteAll();
	//DBObjectIDが不正なら何もせず終了
	if( inProjectDBObjectID == kObjectID_Invalid )   return;
	//相対パスが一致するレコードを検索
	AProjectPrefDB&	projectPrefDB = mProjectPrefDBArray.GetObjectConstByID(inProjectDBObjectID);
	for( AIndex i = 0; i < projectPrefDB.GetItemCount_Array(AProjectPrefDB::kExternalComment_PathAndModuleName); i++ )
	{
		const AText&	pathAndModuleName = projectPrefDB.GetData_TextArrayRef(AProjectPrefDB::kExternalComment_PathAndModuleName).GetTextConst(i);
		if( pathAndModuleName.CompareMin(inRelativePath) == true )
		{
			//相対パスが一致したら、modulenameとoffsetを取り出す
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
			//出力配列に追加
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
外部コメント更新をドキュメントへ通知する
*/
void	AApp::NotifyExternalCommentUpdatedToDocument( const AObjectID inProjectDBObjectID, const AText& inRelativePath )
{
	ADocumentID docID = SPI_GetDocumentIDByProjectObjectIDAndPath(inProjectDBObjectID,inRelativePath,false);
	if( docID == kObjectID_Invalid )   return;
	
	//ドキュメントの外部コメント配列を更新（付箋紙ウインドウもこのメソッドから更新される）
	SPI_GetTextDocumentByID(docID).SPI_UpdateExternalCommentArray();
}
#endif

//#138
/**
ProjectObjectIDとパスからドキュメントのIDを取得
*/
ADocumentID	AApp::SPI_GetDocumentIDByProjectObjectIDAndPath( const AObjectID inProjectDBObjectID, const AText& inRelativePath,
			const ABool inCreateDocument )
{
	//ドキュメントのパスを取得
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

#pragma mark <所有クラス(AWindow_FusenList)>

#pragma mark ===========================
//#138

/**
付箋紙リストウインドウのアクティブドキュメントを設定する
アクティブウインドウ（タブ）の変更時にコールされる。
@param inDocumentFile アクティブドキュメントのファイル
*/
void	AApp::SPI_SetFusenListActiveDocument( /*#298 const AFileAcc& inDocumentFile*/ const AObjectID inProjectObjectID, const AText& inRelativePath )
{
	//付箋紙リストウインドウ未生成なら終了
	if( SPI_GetFusenListWindow().NVI_IsWindowCreated() == false )   return;
	
	/*#298
	//ProjectObjectIDと相対パスを取得
	AFileAcc	projectFolder;
	AObjectID	projectObjectID;
	AText	relativePath;
	GetApp().SPI_FindProjectFolder(inDocumentFile,projectFolder,projectObjectID,relativePath);
	if( projectObjectID == kObjectID_Invalid )   return;
	*/
	if( inProjectObjectID == kObjectID_Invalid )   return;
	
	//Viewに対してアクティブドキュメント更新を指示
	SPI_GetFusenListWindow().SPI_GetListView().SPI_ChangeActiveDocument(inProjectObjectID,inRelativePath);
}
#endif

#pragma mark ===========================

#pragma mark ---ジャンプメニュー
//#857

/**
ジャンプメニュー更新(realize＋チェックマーク)（同じジャンプメニューの場合は更新しない）
*/
void	AApp::SPI_UpdateJumpMenu( const AObjectID inDynamicMenuObjectID, const AIndex inCheckItemIndex )
{
	ABool	shouldRealize = (inDynamicMenuObjectID!=mCurrentJumpMenu_DynamicMenuObjectID);
	//ジャンプメニュー実体化
	if( shouldRealize == true )
	{
		GetApp().NVI_GetMenuManager().RealizeDynamicMenu(inDynamicMenuObjectID);
		mCurrentJumpMenu_DynamicMenuObjectID = inDynamicMenuObjectID;
	}
	//今回実体化した、もしくは、チェック項目が異なる場合は、チェック更新
	if( shouldRealize == true || inCheckItemIndex != mCurrentJumpMenu_CheckItemIndex )
	{
		GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenu(kMenuItemID_Jump,inCheckItemIndex);
		mCurrentJumpMenu_CheckItemIndex = inCheckItemIndex;
	}
}

#pragma mark ===========================

#pragma mark ---各種フローティングウインドウ

#pragma mark ===========================
//#291

/**
識別子情報ウインドウShow/Hide
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
	//DBに保存
	GetAppPref().SetData_Bool(AAppPrefDB::kDisplayIdInfoWindow,inShow);
}
 */

/**
ファイルリストウインドウShow/Hide
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
	//DBに保存
	GetAppPref().SetData_Bool(AAppPrefDB::kDisplayWindowList,inShow);
}
 */

#pragma mark ===========================

#pragma mark ---Navigateボタン

#pragma mark ===========================
//#146

/**
Nagivate位置登録

現在の最前面のドキュメントの選択位置をNavigate履歴に保存する
*/
void	AApp::SPI_RegisterNavigationPosition()
{
	//最前面ウインドウの現在ドキュメントの選択位置を取得
	AWindowID	winID = NVI_GetMostFrontWindowID(kWindowType_Text);
	if( winID != kObjectID_Invalid )
	{
		ADocumentID	docID = SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextDocument().GetObjectID();
		//ファイル情報取得（specifiedでなければ登録しない）
		AFileAcc	file;
		if( SPI_GetTextDocumentByID(docID).NVI_IsFileSpecified() == true )
		{
			SPI_GetTextDocumentByID(docID).NVI_GetFile(file);
			//選択位置取得
			ATextIndex	start,end;
			SPI_GetTextWindowByID(winID).SPI_GetSelect(docID,start,end);
			//現在のIndex以降の要素を削除
			while( mNavigate_File.GetItemCount() > mNavigateIndex )
			{
				mNavigate_File.Delete1Object(mNavigateIndex);
				mNavigate_StartIndex.Delete1(mNavigateIndex);
				mNavigate_Length.Delete1(mNavigateIndex);
			}
			//追加
			mNavigate_File.GetObject(mNavigate_File.AddNewObject()).CopyFrom(file);
			mNavigate_StartIndex.Add(start);
			mNavigate_Length.Add(end-start);
			//indexをItemCount個数と同じにする
			mNavigateIndex++;
		}
	}
}

/**
次へNavigate
*/
void	AApp::SPI_NavigateNext()
{
	if( SPI_GetCanNavigateNext() == false )   return;
	
	//index++
	mNavigateIndex++;
	//テキスト選択
	if( mNavigateIndex == mNavigate_File.GetItemCount() )
	{
		//履歴の最後の場合はLastを選択する（履歴のItemCountの位置には、要素がない。）
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
前へNavigate
*/
void	AApp::SPI_NavigatePrev() 
{
	if( SPI_GetCanNavigatePrev() == false )   return;
	
	//最後から前に戻る場合はLastを記憶する
	if( mNavigateIndex == mNavigate_File.GetItemCount() )
	{
		//最前面ウインドウの現在ドキュメントの選択位置を保存
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
	//テキスト選択
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
前への移動可否を返す。
*/
ABool	AApp::SPI_GetCanNavigatePrev() const
{
	return (mNavigateIndex > 0);
}

/**
次への移動可否を返す
*/
ABool	AApp::SPI_GetCanNavigateNext() const
{
	return (mNavigateIndex < mNavigate_File.GetItemCount());
}

#pragma mark ===========================

/*#725 SPI_UpdateFileListWindows()に変更
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
prependされた新規ファイル自動生成処理を実行
（起動時にEVT_OpenUntitledFile()経由でSPI_CreateNewWindowAutomatic()がコールされるが、
　mReopenProcessCompletedフラグがfalseの間は実行せず、prependフラグをtrueにするのみとしている。
　prependされた新規ファイル自動生成処理をここで実行する。）
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
	//reopen処理中は新規生成処理しない。reopenの最後にここが呼ばれ、そのときには実行される。
	if( mReopenProcessCompleted == false )
	{
		mPrependOpenUntitledFileEvent = true;//#1056
		return;
	}
	
	//B0395 DockにしまわれたウインドウはCWindowImp::GetMostFrontWindow()で取得できないようなので、
	//テキストドキュメントが0個の場合のみ新規ドキュメント生成するように変更
	//if( NVI_GetMostFrontWindowID(kWindowType_Text) == kObjectID_Invalid )
	if( NVI_GetDocumentCount(kDocumentType_Text) == 0 )
	{
		//#844 if( GetAppPref().GetData_Bool(AAppPrefDB::kCreateNewWindowAutomatic) == true )//B0000 この設定OFFでもcollapse解除は行うようにする
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
	//B0395 全てのテキストウインドウがcollapsedなら、最初のウインドウをcollapse解除する
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
自動新規生成ドキュメントを閉じる
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
テンポラリフォルダ取得（なければ生成）
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
１つユニークなテンポラリフォルダを生成し取得
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
格言語ごとdataフォルダ取得
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
マルチファイル置換バックアップフォルダデフォルト取得
*/
void	AApp::SPI_GetDefaultMultiFileReplaceBackupFolder( AFileAcc& outFolder )
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChild(appPrefFolder,"MultiFileReplaceBackup");
	outFolder.CreateFolder();
	//既に"MultiFileReplaceBackup"名称の、フォルダでないもの（ファイル）が存在していた場合は、app pref folderにする
	//★警告表示
	if( outFolder.IsFolder() == false )
	{
		outFolder.CopyFrom(appPrefFolder);
	}
}

//#81
/**
自動バックアップフォルダ取得
*/
void	AApp::SPI_GetAutoBackupRootFolder( AFileAcc& outFolder )
{
	//==================自動バックアップ保存先フォルダ取得==================
	if( NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kEnableAutoBackup) == true )
	{
		//app prefの自動バックアップ保存先設定データ取得
		AText	autoBackupRootFolderPath;
		NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kAutoBackupRootFolder,autoBackupRootFolderPath);
		outFolder.Specify(autoBackupRootFolderPath);
		//フォルダが存在していなければフォルダ生成
		if( outFolder.Exist() == false )
		{
			outFolder.CreateFolderRecursive();
		}
		//フォルダ生成できない場合は、デフォルトのバックアップ先にバックアップする
		if( outFolder.Exist() == false || outFolder.IsFolder() == false )
		{
			SPI_GetDefaultAutoBackupRootFolder(outFolder);
			//★警告
		}
	}
	else
	{
		//自動バックアップがenableで無い場合は、デフォルトauto backupフォルダを取得
		//ドキュメントデータ保存先フォルダとしてのみ使用する。
		SPI_GetDefaultAutoBackupRootFolder(outFolder);
	}
}

//#81
/**
自動バックアップ（ドキュメントデータ保存）デフォルトフォルダ取得
*/
void	AApp::SPI_GetDefaultAutoBackupRootFolder( AFileAcc& outFolder )
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChild(appPrefFolder,"AutoBackup");
	outFolder.CreateFolderRecursive();
	//既に"AutoBackup"名称の、フォルダでないもの（ファイル）が存在していた場合は、app pref folderにする
	//★警告表示
	if( outFolder.IsFolder() == false )
	{
		outFolder.CopyFrom(appPrefFolder);
	}
}

//#81
/**
未保存データ保存フォルダ取得
*/
void	AApp::SPI_GetUnsavedDataFolder( AFileAcc& outFolder )
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChild(appPrefFolder,"UnsavedData");
	outFolder.CreateFolder();
	//既に"UnsavedData"名称のフォルダでないもの（ファイル）が存在していた場合は、app pref folderにする
	//★警告表示
	if( outFolder.IsFolder() == false )
	{
		outFolder.CopyFrom(appPrefFolder);
	}
}

//#898
/**
ドキュメントデータ保存フォルダ取得
*/
void	AApp::SPI_GetDocPrefRootFolder( AFileAcc& outFolder )
{
	outFolder.SpecifyChild(AFileWrapper::GetAppPrefFolder(),"DocumentData");
}

//#889
/**
カラースキームフォルダ（アプリ内data/ColorScheme）取得
*/
void	AApp::SPI_GetAppColorSchemeFolder( AFileAcc& outFolder )
{
	AFileWrapper::GetResourceFile("data/ColorScheme",outFolder);
}

/**
ユーザー用カラースキームフォルダ（miアプリ設定フォルダ内ColorScheme/）取得
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
アクセスプラグインフォルダ（アプリ内AccessPlugin）取得
*/
void	AApp::SPI_GetAppAccessPluginFolder( AFileAcc& outFolder )
{
	AFileWrapper::GetResourceFile("AccessPlugin",outFolder);
}

/**
ユーザー用アクセスプラグインフォルダ（miアプリ設定フォルダ内ColorScheme/）取得
*/
void	AApp::SPI_GetUserAccessPluginFolder( AFileAcc& outFolder ) const
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	outFolder.SpecifyChild(appPrefFolder,"AccessPlugin");
	outFolder.CreateFolderRecursive();
}

/**
テキストエンコーディングファイル（アプリ内data/TextEncoding/TextEncoding.csv）取得
*/
void	AApp::SPI_GetAppTextEncodingFile( AFileAcc& outFile ) const
{
	AFileWrapper::GetResourceFile("data/TextEncoding/TextEncoding.csv",outFile);
}

/**
テキストエンコーディングファイル（miアプリ設定フォルダ内TextEncoding/TextEncoding.csv）取得
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

#pragma mark --- ツールコマンド管理

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

#pragma mark --- 候補履歴
//#717

/**
候補履歴追加
*/
void	AApp::SPI_AddToRecentCompletionWord( const AText& inWord )
{
	AStMutexLocker	locker(mRecentCompletionWordArrayMutex);
	
	//候補履歴から検索
	AIndex	index = mRecentCompletionWordArray.Find(inWord);
	if( index != kIndex_Invalid )
	{
		//見つかったらそれを削除
		mRecentCompletionWordArray.Delete1(index);
	}
	//候補履歴に追加
	mRecentCompletionWordArray.Insert1(0,inWord);
	//候補履歴個数制限
	if( mRecentCompletionWordArray.GetItemCount() > kLimit_MaxRecentCompletionWordArray )
	{
		mRecentCompletionWordArray.Delete1(kLimit_MaxRecentCompletionWordArray);
	}
}

/**
候補履歴取得
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
LuaConsoleのDocumentID取得（未生成ならInvalidを返す）
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
LuaConsoleのDocumentID取得（未生成なら生成して取得）
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
アクティブなTextWindowを取得
*/
AWindowID	AApp::Lua_GetActiveTextWindowID()
{
	//LuaConsoleのWindowIDを取得
	ADocumentID	luaConsoleDocID = GetApp().SPI_GetLuaConsoleDocumentID();
	AWindowID	luaConsoleWinID = kObjectID_Invalid;
	if( luaConsoleDocID != kObjectID_Invalid )
	{
		luaConsoleWinID = GetApp().SPI_GetTextDocumentByID(luaConsoleDocID).SPI_GetFirstWindowID();
	}
	//LuaConsole以外の最前面WindowIDを取得
	return GetApp().NVI_GetMostFrontWindowID(kWindowType_Text,luaConsoleWinID);
}

/**
アクティブなTextDocumentを取得
*/
AUniqID	AApp::Lua_GetActiveTextDocumentUniqID()//#693
{
	AWindowID	winID = Lua_GetActiveTextWindowID();
	if( winID == kObjectID_Invalid )   return kUniqID_Invalid;
	return GetApp().NVI_GetDocumentUniqID(GetApp().SPI_GetTextWindowByID(winID).NVI_GetCurrentDocumentID());//#693
}

/**
アクティブなTextViewを取得
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

#pragma mark ---ドキュメント操作

/**
InsertText(int docID,int textIndex,string text)
@return void
*/
int	AApp::Lua_InsertText( lua_State* L )
{
	try
	{
		//引数取得
		AText	text;
		Lua_PopArgumentText(L,text);
		AIndex	textIndex = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブドキュメントに対して実行
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqIDからADocumentIDを取得
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//ドキュメント存在チェック
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndexチェック
		if( textIndex < 0 || textIndex > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range");
		//TextIndex UTF-8位置チェック
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetCurrentCharTextIndex(textIndex) != textIndex )   Lua_Error(L,"(MI002) text index is not at first byte of UTF-8");
		
		//InsertText実行
		GetApp().SPI_GetTextDocumentByID(docID).SPI_RecordUndoActionTag(undoTag_Tool);
		GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(textIndex,text);
		
		//返り値設定
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
		//引数取得
		AIndex	epos = Lua_PopArgumentInteger(L);
		AIndex	spos = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブドキュメントに対して実行
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqIDからADocumentIDを取得
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//ドキュメント存在チェック
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndexチェック
		if( spos < 0 || spos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (start index)");
		if( epos < 0 || epos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (end index)");
		//TextIndex UTF-8位置チェック
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetCurrentCharTextIndex(spos) != spos )   Lua_Error(L,"(MI002) text index is not at first byte of UTF-8 (start index)");
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetCurrentCharTextIndex(epos) != epos )   Lua_Error(L,"(MI002) text index is not at first byte of UTF-8 (end index)");
		
		//DeleteText実行
		GetApp().SPI_GetTextDocumentByID(docID).SPI_RecordUndoActionTag(undoTag_Tool);
		GetApp().SPI_GetTextDocumentByID(docID).SPI_DeleteText(spos,epos);
		
		//返り値設定
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
		//引数取得
		AIndex	pos = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブドキュメントに対して実行
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqIDからADocumentIDを取得
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//ドキュメント存在チェック
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndexチェック
		if( pos < 0 || pos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range");
		
		//GetNextCharPos実行
		pos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetNextCharTextIndex(pos);
		
		//返り値設定
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
		//引数取得
		AIndex	pos = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブドキュメントに対して実行
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqIDからADocumentIDを取得
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//ドキュメント存在チェック
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndexチェック
		if( pos < 0 || pos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range");
		
		//GetPrevCharPos実行
		pos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetPrevCharTextIndex(pos);
		
		//返り値設定
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
		//引数取得
		AIndex	pos = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブドキュメントに対して実行
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqIDからADocumentIDを取得
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//ドキュメント存在チェック
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndexチェック
		if( pos < 0 || pos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range");
		
		//GetParagraphIndex実行
		ATextPoint	textpt = {0,0};
		GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextPointFromTextIndex(pos,textpt);
		AIndex	paraIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphIndexByLineIndex(textpt.y);
		
		//返り値設定
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
		//引数取得
		AIndex	paraIndex = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブドキュメントに対して実行
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqIDからADocumentIDを取得
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//ドキュメント存在チェック
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//paraIndexチェック
		if( paraIndex < 0 || paraIndex > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphCount() )   Lua_Error(L,"(MI003) paragraph index out of range");
		
		//GetParagraphStart実行
		AIndex	pos = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphStartTextIndex(paraIndex);
		
		//返り値設定
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
		//引数取得
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブドキュメントに対して実行
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqIDからADocumentIDを取得
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//ドキュメント存在チェック
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		
		//GetParagraphCount実行
		AItemCount	paraCount = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParagraphCount();
		
		//返り値設定
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
		//引数取得
		AIndex	epos = Lua_PopArgumentInteger(L);
		AIndex	spos = Lua_PopArgumentInteger(L);
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブドキュメントに対して実行
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqIDからADocumentIDを取得
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//ドキュメント存在チェック
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		//TextIndexチェック
		if( spos < 0 || spos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (start index)");
		if( epos < 0 || epos > GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (end index)");
		
		//GetText実行
		AText	text;
		GetApp().SPI_GetTextDocumentByID(docID).SPI_GetText(spos,epos,text);
		
		//返り値設定
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

#pragma mark ---View操作

/**
GetSelectedRange(int viewID)
@return {int s,int e}
*/
int	AApp::Lua_GetSelectedRange( lua_State* L )
{
	try
	{
		//引数取得
		AUniqID	viewUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブViewに対して実行
		if( viewUniqID == kUniqID_Invalid )
		{
			viewUniqID = Lua_GetActiveTextViewUniqID();
		}
		//UniqIDからAViewIDを取得
		AViewID	viewID = GetApp().NVI_GetViewIDByUniqID(viewUniqID);//#693
		//View存在チェック
		GetApp().Lua_CheckExistView(L,viewID,kViewType_Text);
		
		//最前面のドキュメントに対してGetSelectedText実行
		AIndex	startIndex = kIndex_Invalid, endIndex = kIndex_Invalid;
		AView_Text::GetTextViewByViewID(viewID).SPI_GetSelect(startIndex,endIndex);
		
		//返り値設定
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
		//引数取得
		AIndex	epos = Lua_PopArgumentInteger(L);
		AIndex	spos = Lua_PopArgumentInteger(L);
		AUniqID	viewUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブViewに対して実行
		if( viewUniqID == kUniqID_Invalid )
		{
			viewUniqID = Lua_GetActiveTextViewUniqID();
		}
		//UniqIDからAViewIDを取得
		AViewID	viewID = GetApp().NVI_GetViewIDByUniqID(viewUniqID);//#693
		//View存在チェック
		GetApp().Lua_CheckExistView(L,viewID,kViewType_Text);
		//TextIndexチェック
		if( spos < 0 || spos > AView_Text::GetTextViewByViewID(viewID).SPI_GetTextDocument().SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (start index)");
		if( epos < 0 || epos > AView_Text::GetTextViewByViewID(viewID).SPI_GetTextDocument().SPI_GetTextLength() )   Lua_Error(L,"(MI001) text index out of range (end index)");
		
		//最前面のドキュメントに対してGetSelectedText実行
		AView_Text::GetTextViewByViewID(viewID).SPI_SetSelect(spos,epos);
		
		//返り値設定
		return 0;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

#pragma mark ===========================

#pragma mark ---ObjectID取得

/**
GetActiveDocumentID()
@return int
*/
int	AApp::Lua_GetActiveDocumentID( lua_State* L )
{
	try
	{
		//引数取得
		Lua_CheckArgumentEnd(L);
		
		//アクティブドキュメント取得
		AUniqID	docID = Lua_GetActiveTextDocumentUniqID();//#693
		
		//返り値設定
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
		//引数取得
		Lua_CheckArgumentEnd(L);
		
		//アクティブビュー取得
		AUniqID	viewID = Lua_GetActiveTextViewUniqID();//#693
		
		//返り値設定
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
		//引数取得
		AUniqID	viewUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブViewに対して実行
		if( viewUniqID == kUniqID_Invalid )
		{
			viewUniqID = Lua_GetActiveTextViewUniqID();
		}
		//UniqIDからAViewIDを取得
		AViewID	viewID = GetApp().NVI_GetViewIDByUniqID(viewUniqID);//#693
		//View存在チェック
		GetApp().Lua_CheckExistView(L,viewID,kViewType_Text);
		
		//
		AUniqID	docID = GetApp().NVI_GetDocumentUniqID(AView_Text::GetTextViewByViewID(viewID).SPI_GetTextDocument().GetObjectID());//#693
		
		//返り値設定
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

#pragma mark ---ドキュメントオブジェクト

/**
CreateNewDocument(string modeName)
@return int
*/
int	AApp::Lua_CreateNewDocument( lua_State* L )
{
	try
	{
		//引数取得
		AText	modeName;
		Lua_PopArgumentText(L,modeName);
		Lua_CheckArgumentEnd(L);
		
		//CreateNewDocument実行
		AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modeName);
		if( modeIndex == kIndex_Invalid )
		{
			modeIndex = kStandardModeIndex;
		}
		AText	utf8("UTF-8");
		AUniqID	docID = GetApp().NVI_GetDocumentUniqID(GetApp().SPNVI_CreateNewTextDocument(modeIndex,utf8));//#693
		
		//返り値設定
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
		//引数取得
		AUniqID	docUniqID = Lua_PopArgumentUniqID(L);
		Lua_CheckArgumentEnd(L);
		//"-1"の場合、アクティブドキュメントに対して実行
		if( docUniqID == kUniqID_Invalid )
		{
			docUniqID = Lua_GetActiveTextDocumentUniqID();
		}
		//UniqIDからADocumentIDを取得
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		//ドキュメント存在チェック
		GetApp().Lua_CheckExistDocument(L,docID,kDocumentType_Text);
		
		//Reveal実行
		GetApp().SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();
		
		//返り値設定
		return 0;
	}
	catch(...)
	{
		Lua_Error(L,"(MI999) unknown error happens.");
	}
	return 0;
}

#pragma mark ===========================

#pragma mark ---その他

/**
print(string text)
@return void
*/
int	AApp::Lua_Output( lua_State* L )
{
	try
	{
		//引数取得
		AText	text;
		Lua_PopArgumentText(L,text);
		Lua_CheckArgumentEnd(L);
		
		//LuaConsoleへoutput
		ADocumentID	docID = GetApp().SPI_CreateOrGetLuaConsoleDocumentID();
		GetApp().SPI_GetTextDocumentByID(docID).SPI_Lua_Output(text);
		
		//返り値設定
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
		//引数取得
		Lua_CheckArgumentEnd(L);
		
		//返り値設定
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
		//引数取得
		Lua_CheckArgumentEnd(L);
		
		//LuaConsoleへoutput
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
		//返り値設定
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
はじめにお読みくださいを表示する
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

#pragma mark <内部モジュール>

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
	//ディレクトリ設定
	AStCreateCstringFromAText	dirPath(folderpath);
	chdir(dirPath.GetPtr());
	//dvi to pdfコマンドパス取得
	AText	command;
	NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kTexDviToPdfPath,command);
	command.AddCstring(" \"");
	command.AddText(dvifilename);
	command.AddCstring("\"");
	//dvi to pdfコマンド実行
	AStCreateCstringFromAText	cstr(command);
	system(cstr.GetPtr());
	
	//pdfファイル取得
	AText	pdffilename;
	pdffilename.SetText(filenamewithoutsuffix);
	pdffilename.AddCstring(".pdf");
	AFileAcc	pdffile;
	pdffile.SpecifyChild(folder,pdffilename);
	//pdfファイルを開く
	ALaunchWrapper::Open(pdffile);
}


//致命的エラー発生時のデータバックアップ
//ABaseFunction::CriticalError等からコールされる
void	AApp::NVIDO_BackupDataForCriticalError()//#182
{
	//対応検討
	//各ドキュメントの内容をファイルに書き出し
}

/*
#182 AApplication.cppに移動
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

//アプリケーション名
void	GetApplicationName( AText& outName )
{
	outName.SetCstring("mi");
}

/*
#182 AApplication.cppに移動
void	EventTransactionPostProcess( const ABool inPeriodical )
{
	try
	{
		if( inPeriodical == false )
		{
			//今のところ処理無し
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

/*覚え書き
自動テスト実行のための環境
・環境設定フォルダに"debug"ファイルを作成
・ボリュームのルートにmitestフォルダ＋2ファイルを保存
・アプリケーションフォルダにmi(a7)フォルダ＋中身を保存
*/


#pragma mark ===========================
#pragma mark [クラス]AStEditProgressModalSession
#pragma mark ===========================

/**
モーダルセッション開始・終了用スタッククラス（コンストラクタ）
*/
AStEditProgressModalSession::AStEditProgressModalSession( const AEditProgressType inType, 
														 const ABool inEnableCancelButton,
														 const ABool inShowProgressWindowNow,
														const ABool inEnableModalSession ) : mModalSessionStarted(false)
{
	//モーダルセッション開始
	if( inEnableModalSession == true )
	{
		mModalSessionStarted = GetApp().SPI_StartEditProgressModalSession(inType,inEnableCancelButton,inShowProgressWindowNow);
	}
}

/**
モーダルセッション開始・終了用スタッククラス（デストラクタ）
*/
AStEditProgressModalSession::~AStEditProgressModalSession()
{
	//モーダルセッション終了
	if( mModalSessionStarted == true )
	{
		GetApp().SPI_EndEditProgressModalSession();
	}
}



