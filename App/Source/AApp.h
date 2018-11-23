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

#pragma once

#include "../../AbsFramework/Wrapper.h"
#include "../../AbsFramework/Frame.h"
#include "ADocument_Text.h"
#include "ADocument_IndexWindow.h"
//#include "ATest.h"
#include "AWindow_MultiFileFind.h"
#include "AAppPrefDB.h"
#include "AModePrefDB.h"
#include "AWindow_MultiFileFindProgress.h"
#include "AMultiFileFinder.h"
//#193 #include "AWindow_FTPFolder.h"
#include "AWindow_FolderList.h"
#include "AFTP.h"
#include "AWindow_TraceLog.h"
#include "AWindow_AppPref.h"
#include "AWindow_Find.h"
#include "AWindow_FileList.h"
#include "AWindow_ModePref.h"
//#include "AWindow_IconManager.h"
#include "AWindow_About.h"
#include "AWindow_FTPProgress.h"
#include "AWindow_AddToolButton.h"
//#include "AUnicodeData.h"
#include "ATextFinder.h"
#include "AWindow_TextCount.h"
#include "AImportFileManager.h"
#include "AChildProcessManager.h"
#include "AWindow_SetProjectFolder.h"
#include "AWindow_NewFTPFile.h"
#include "ASCMFolderManager.h"
#include "AWindow_BigFileAlert.h"//R0208
#if IMPLEMENTATION_FOR_MACOSX
#include "AAppleScriptExecuter.h"
#endif
#include "AWindow_CompareFolder.h"
#include "AWindow_IdInfo.h"//RC2
#include "AWindow_CandidateList.h"//RC2
#include "AProjectPrefDB.h"//RC3
//#858 #include "AWindow_ProjectFolderAlert.h"//RC3
#include "AWindow_KeyToolList.h"//R0073
//#858 #include "AWindow_FusenList.h"//#138
//#858 #include "AWindow_FusenEdit.h"//#138
//#858 #include "AWindow_Fusen.h"//#138
#include "AWindow_KeyRecord.h"//#390
#include "AWindow_EnabledDefines.h"//#476
//#include "AWordsLinkList.h"//#723
#include "AWindow_CallGraf.h"//#723
#include "AWindow_Previewer.h"//#734
#include "ARemoteConnection.h"//#361
#include "AWindow_TextMarker.h"//#750
#include "AWindow_ClipboardHistory.h"//#750
#include "AWindow_DocInfo.h"//#142
#include "AThread_WordsListFinder.h"
//#include "AWindow_SubWindowsList.h"
#include "AThread_IdInfoFinder.h"
#include "AWindow_KeyActionList.h"
#include "AWindow_EditProgress.h"
#include "AWindow_WordsList.h"
#include "AView_WordsList.h"
#include "AWindow_FindResult.h"
#include "AThread_JavaScriptExecuter.h"
#include "AWindow_AddNewMode.h"
#include "AView_Text.h"

/**
編集プログレスタイプ
*/
enum AEditProgressType
{
	kEditProgressType_None = 0,
	kEditProgressType_Replace = 1,
	kEditProgressType_Undo,
	kEditProgressType_Redo,
	kEditProgressType_Find,
	kEditProgressType_MultiFileReplace,
	kEditProgressType_FindList,
	kEditProgressType_JavaScript,
	kEditProgressType_Indent,
	kEditProgressType_Correspond,
	kEditProgressType_BatchReplace,
	kEditProgressType_CalcLineInfo,
	kEditProgressType_InitMode,
	kEditProgressType_CopyV2AppPrefFolder,
};

#pragma mark ===========================
#pragma mark [クラス]AApp
//アプリケーションオブジェクト
class AApp : public AApplication
{
	//コンストラクタ、デストラクタ
  public:
	AApp();
	~AApp();
	
	//起動／終了処理
  public:
	//#182int					Run();
	void					SPI_SaveReopenFile();//#513
	void					SPI_ClearReopenData();//#1102
	void					SPI_UpdateColorSchemeMenu();
	void					SPI_UpdateAccessPlugInMenu();//#193
	const ATextArray&		SPI_GetAccessPluginMenuTextArray() { return mAccessPluginMenuTextArray; }//#193
  private:
	ABool					NVIDO_Run();//#182
	ABool					NVIDO_Quit(/*#1102 const ABool inContinuing */);//win 080702 #182 #433
	void					Startup_Phase_Data();
	void					Startup_Phase_UI();
	void					Startup_Phase_UIUpdate();
	void					Quit_Phase_Data();
	void					Quit_Phase_UI();
  private:
	ABool					NVIDO_ExecCommandLineArgs( const ATextArray& inArgs );//#263
	void					ReopenFile();//#513
	void					OpenUnsavedData();//#81
	ABool								mReopenProcessCompleted;//#688
	ATextArray							mAccessPluginMenuTextArray;//#193
	
	//#1034
  private:
	void					NVIDO_GetOrderedWindowIDArray( AArray<AWindowID>& outArray, const AWindowType inWindowType );//#1034
	
	//URIスキーム #1003
  public:
	void					NVIDO_ExecuteURIScheme( const AText& inURI );
	
	//#1050
	//タブセット
  private:
	void					OpenTabSet( const AFileAcc& inTabSetFile );
	
	//skin
  public:
	void					SPI_UpdateSkinMenu();
	void					SPI_GetUserSkinFolder( AFileAcc& outFolder );
	void					SPI_LoadImageFiles( const ABool inRemoveOld );
  private:
	void					LoadImageFile( const ABool inUseCustom ,const AFileAcc& inCustomFolder, 
										  const AConstCharPtr inFilename, const AImageID inImageID, const ABool inRemoveOld );
	void					LoadColors( const ABool inUseCustom ,const AFileAcc& inCustomFolder );
	
	//skin色取得
  public:
	AColor					SPI_GetTabLetterColor() const { return mSkinColor_TabLetter; }
	AColor					SPI_GetTabLetterColorTopmost() const { return mSkinColor_TabLetterTopmost; }
	AColor					SPI_GetTabLetterColorDeactivate() const { return mSkinColor_TabLetterDeactive; }
	AColor					SPI_GetTabLetterDropShadowColor() const { return mSkinColor_TabLetterDropShadow; }
	AColor					SPI_GetTabLetterTopmostDropShadowColor() const { return mSkinColor_TabLetterTopmostDropShadow; }
	AColor					SPI_GetSubTextColumnButtonLetterColor() const { return mSkinColor_SubTextColumnButtonLetterColor; }
	AColor					SPI_GetSubTextColumnButtonLetterColorDeactivate() const { return mSkinColor_SubTextColumnButtonLetterColorDeactive; }
	AColor					SPI_GetSubTextColumnButtonDropShadowColor() const { return mSkinColor_SubTextColumnButtonDropShadowColor; }
	AColor					SPI_GetSubWindowsBoxHeaderColor() const { return mSkinColor_SubWindowBoxHeaderColor; }
	AColor					SPI_GetSubWindowBoxFunctionNameColor() const { return mSkinColor_SubWindowBoxFunctionNameColor; }
	AColor					SPI_GetSubWindowBoxMatchedColor() const { return mSkinColor_SubWindowBoxMatchedColor; }
	AColor					SPI_GetFileListDiffColor() const { return mSkinColor_FileListDiffColor; }
	AColor					SPI_GetJumpListDiffColor() const { return mSkinColor_JumpListDiffColor; }
  private:
	AColor								mSkinColor_TabLetter;
	AColor								mSkinColor_TabLetterTopmost;
	AColor								mSkinColor_TabLetterDeactive;
	AColor								mSkinColor_TabLetterDropShadow;
	AColor								mSkinColor_TabLetterTopmostDropShadow;
	AColor								mSkinColor_SubTextColumnButtonLetterColor;
	AColor								mSkinColor_SubTextColumnButtonLetterColorDeactive;
	AColor								mSkinColor_SubTextColumnButtonDropShadowColor;
	AColor								mSkinColor_SubWindowLetterColor;
	AColor								mSkinColor_SubWindowTitlebarTextColor;
	AColor								mSkinColor_SubWindowTitlebarTextDropShadowColor;
	AColor								mSkinColor_SubWindowBackgroundColor;
	AColor								mSkinColor_SubWindowBackgroundColorDeactive;
	AColor								mSkinColor_SubWindowHeaderLetterColor;
	AColor								mSkinColor_SubWindowHeaderBackgroundColor;
	AColor								mSkinColor_SubWindowBoxLetterColor;
	AColor								mSkinColor_SubWindowBoxLetterDropShadowColor;
	AColor								mSkinColor_SubWindowBoxBackgroundColor;
	AColor								mSkinColor_SubWindowBoxHeaderColor;
	AColor								mSkinColor_SubWindowBoxFunctionNameColor;
	AColor								mSkinColor_SubWindowBoxMatchedColor;
	AColor								mSkinColor_FileListDiffColor;
	AColor								mSkinColor_JumpListDiffColor;
	
	//バージョン2.1の標準添付ツールバー名称リスト
  public:
	ABool					SPI_IsV2ToolbarItemNameToHide( const AText& inName ) const;
  private:
	AHashTextArray						mV2ToolbarItemNameToHideArray;
	
	//アプリ更新日時
  public:
	void				SPI_GetAppModifiedDataTimeText( AText& outText ) const { outText.SetText(mAppModifiedDataTimeText); }
  private:
	AText								mAppModifiedDataTimeText;
	
	//モード初期化
  public:
	void					SPI_GetModeRootFolder( AFileAcc& outFileAcc ) const;
	//#920 void					SPI_CopyToLaunchBackup();//#402
	void					SPI_GetModeDefaultFolder( AFileAcc& outFileAcc ) const;//#463
  private:
	void					InitModePref();
	//#463 void					GetModeDefaultFolder( AFileAcc& outFileAcc ) const;
	AHashTextArray						mNewlyCreatedModeNameArray;
	
	//ModeUpdate #427
  public:
	void					SPI_ModeUpdate( const AModeIndex inModeIndex, const ABool inForce );
  private:
	void					ModeUpdate_Result_Revision( const AText& inText, const ATextArray& inInfoTextArray );
	void					ModeUpdate_Result_ZipFile( const AText& inZipText, const ATextArray& inInfoTextArray );
	
	//Webからモード取得 #427
  public:
	void					SPI_AddNewModeFromWeb( const AText& inModeName, const AText& inURL );
  private:
	void					AddNewModeFromWeb_Result_Revision( const AText& inText, const ATextArray& inInfoTextArray );
	void					AddNewModeFromWeb_Result_ZipFile( const AText& inText, const ATextArray& inInfoTextArray );
	
	//自動更新共通処理 #427
  public:
	void					SPI_GetModeUpdateURL( const AIndex inIndex, AText& outURL ) const;
  private:
	ABool					ParseUpdateTxt( const AText& inText, AText& outModeName, ANumber64bit& outRevision );
	ATextArray							mModeUpdateURLArray;//#427
	
	//マルチファイル検索ファイルフィルタープリセット#617
  public:
	void					SPI_GetFileFilterPreset( const AIndex inIndex, AText& outFileFilterPreset ) const;
  private:
	ATextArray							mFileFilterPresetArray;
	
	//SDF R0000
  public:
	void					SPI_GetBuiltinSDFFileByName( const AText& inSDFName, AFileAcc& outFile ) const;
	const ATextArray&		SPI_GetBuiltinSDFNameArray() const;
  private:
	void					MakeBuiltinSDFList();
	AObjectArray<AFileAcc>				mBuiltinSDFList_File;
	ATextArray							mBuiltinSDFList_Name;
	
	//正規表現サンプルリスト
  public:
	void					SPI_GetRegExpListItem( const AIndex inIndex, AText& outItem ) const;
  private:
	ATextArray							mRegExpListArray;
	
	//
  private:
	void					InitTransliterate();
	
	//FuzzySearch #166
  public:
	const AHashArray<AUCS4Char>&	SPI_GetFuzzySearchNormalizeArray_from() const { return mFuzzySearchNormalizeArray_from; }
	const AObjectArray< AArray<AUCS4Char> >&	SPI_GetFuzzySearchNormalizeArray_to() const { return mFuzzySearchNormalizeArray_to; }
  private:
	void					InitFuzzySearch();
	AHashArray<AUCS4Char>				mFuzzySearchNormalizeArray_from;
	AObjectArray< AArray<AUCS4Char> >	mFuzzySearchNormalizeArray_to;
	
	/*#724
  public:
	AArray<AIconID>&		SPI_GetToolbarIconArray() { return mToolbarIconArray; }
	void					SPI_CopyToolbarIconResource( const AIconID inIconID, const AFileAcc& inDestFile );
  private:
	void					LoadToolbarIcon();
	void					LoadToolbarIconRecursive( const AFileAcc& inFolder );
	AArray<AIconID>						mToolbarIconArray;
#if IMPLEMENTATION_FOR_MACOSX
	AArray<AIndex>						mToolbarIconArray_ResID;
	ATextArray							mToolbarIconArray_Path;
	AArray<ABool>						mToolbarIconArray_CustomIcon;
#endif
	*/
	//<イベント処理>
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	//#688 ABool					EVTDO_DoMouseMoved();
	void					EVTDO_UpdateMenu();
	void					EVTDO_DoTickTimerAction();
	void					EVTDO_DoAppActivated();
	void					EVTDO_DoAppDeactivated();//B0442
	unsigned long long					mAppElapsedTick;
	void					EVTDO_DoInternalEvent( ABool& outUpdateMenu );
	void					EVTDO_WindowActivated();//#959
	void					EVTDO_OpenUntitledFile() { SPI_CreateNewWindowAutomatic(); }//#1056
  public:
	unsigned long long		SPI_GetAppElapsedTick() { return mAppElapsedTick; }
	ABool								mPrependOpenUntitledFileEvent;//#1056
	
	//<インターフェイス>
	
	//Timer #698
  public:
	void					SPI_ReserveTimedExecution( const ATimedExecutionType inType, const ANumber inFireSecond );
  private:
	AArray<ATimedExecutionType>			mReservedTimedExecutionArray_Type;
	AArray<ANumber>						mReservedTimedExecutionArray_Timer;
	
	//Update
  public:
	//#844 void					SPI_UpdateFixedMenuShortcut();
	void					NVIDO_UpdateAll();
  private:
	void					NVIDO_OpenFile( const AFileAcc& inFile, const AFileOpenDialogRef inRef );//R0198 #1034
	void					NVIDO_MakeCustomControlInFileOpenWindow( const AFileOpenDialogRef inRef );//R0198 #1034
	//#1034 void					NVIDO_GetCustomControlRectInFileOpenWindow( ARect& ioRect );//R0198
#if IMPLEMENTATION_FOR_MACOSX
  private:
	static IBNibRef						sOpenCustomNibRef;
#endif
	
	//情報取得
  public:
	//#305 ALanguage				SPI_GetCurrentLanguage();//#189
	void					SPI_GetDefaultTextFileAttribute( AFileAttribute& outFileAttribute ) const;//win
	void					SPI_GetModeFileAttribute( AFileAttribute& outFileAttribute ) const;//win
	void					SPI_GetExportFileAttribute( AFileAttribute& outFileAttribute ) const;//win
	void					SPI_GetToolFileAttribute( AFileAttribute& outFileAttribute ) const;//win
	void					SPI_GetIndexFileAttribute( AFileAttribute& outFileAttribute ) const;//win
	void					SPI_GetFTPAliasFileAttribute( AFileAttribute& outFileAttribute ) const;//win
	void					SPI_GetDefaultFileFilter( AText& outText ) const;//win 080709
	
	//各種ファイル／フォルダ取得
  public:
	void					SPI_GetTemplateFolder( AFileAcc& outFolder ) const;
	
	//メニュー更新
  private:
	void					NVIDO_UpdateMenu();
	
	//ウインドウメニュー管理
  public:
	//#922 void					SPI_UpdateWindowMenu();
	//#922 void					SPI_GetWindowMenuTextArray( ATextArray& outTextArray ) const;
	void					SPI_SwitchWindow( const ABool inNext );//#552
	//#922 void					SPI_SelectFromWindowMenuByIndex( const AIndex inIndex );
  private:
	//#922 AIndex					FindWindowMenuIndex( const AWindowID inWindowID, const AIndex inTabIndex ) const;
	AArray<AObjectID>					mWindowMenu_WindowIDArray;
	AArray<AIndex>						mWindowMenu_TabIndexArray;
	
	//「最近開いたファイル」メニュー管理
  public:
	void					SPI_AddToRecentlyOpenedFile( const AFileAcc& inFileAcc );
	void					SPI_AddToRecentlyOpenedFile( const AText& inPath );//#235
	void					SPI_UpdateRecentlyOpenedFile( const ABool inUpdateOnlyFirstItem );//B0000高速化
	const ATextArray&		SPI_GetRecentlyOpenedFileDisplayTextArray();
	void					SPI_DeleteRecentlyOpenedFileItem( const AIndex inIndex );//R0186
	void					SPI_DeleteRecentlyOpenedFileNotFound();//R0186
	//#898 ABool					SPI_GetExternalDocPrefFile( const AFileAcc& inDocFile, AFileAcc& outDocPrefFile, const ABool inCreate );//win 080710
	//#898 ABool					SPI_GetExternalDocPrefFile( const AText& inDocPath, AFileAcc& outDocPrefFile, const ABool inCreate );//#241
	void					SPI_DeleteAllRecentlyOpenedFile();//win 080710
	const ATextArray&		SPI_GetRecentlyOpenedFileNameWithComment()//#798
	{ return mRecentlyOpenedFileNameWithComment; }
  private:
	void					UpdateRecentlyOpenedFileDisplayText( const ABool inUpdateOnlyFirstItem );//B0000高速化
	void					DeleteRecentlyOpenedFileItem( const AIndex inIndex );//#241
	ATextArray						mRecentlyOpenedFileDisplayText;
	ATextArray						mRecentlyOpenedFileNameWithComment;//B0413
	ABool							mSuppressAddToRecentlyOpenedFile;//#923
	
	//フォルダーラベル
  public:
	void					SPI_FindFolderLabel( const AText& inDocFilePath, ATextArray& outFolderPathArray, ATextArray& outLabelTextArray ) const;
	
	//キルバッファー #913
  public:
	void					SPI_AddToKillBuffer( const ADocumentID inTextDocID, const ATextPoint& inTextPoint, const AText& inText );
	void					SPI_GetKillBuffer( AText& outText );
  private:
	AText								mKillBuffer_Text;
	ADocumentID							mKillBuffer_DocumentID;
	ATextPoint							mKillBuffer_TextPoint;
	
	//「新規」メニュー管理
  public:
	void					SPI_UpdateNewMenu();
  private:
	AObjectArray<AFileAcc>				mTemplateFile;
	
	//「同じフォルダから開く」メニュー管理
  public:
	void					SPI_UpdateSameFolderMenu( const AFileAcc& inFolder );
	//ファイルリストウインドウ用データ取得メソッド
	AIndex					SPI_GetSameFolderArrayIndex( const AFileAcc& inFolder ) const;
	void					SPI_GetSameFolderFilenameTextArray( ATextArray& outFilenameTextArray ) const;//#982
	void					SPI_GetCurrentFolderPathForDisplay( AText& outText ) const;
	void					SPI_GetSameFolderFile( const AIndex inIndex, AFileAcc& outFile ) const;
	void					SPI_GetSameFolderSCMStateArray( AArray<ASCMFileState>& outSCMStateArray ) const;//R0006
	void					SPI_UpdateAllSameFolderData();//B0420
	void					SPI_MakeFilenameListSameFolder( const AIndex inIndex, const AFileAcc& inFolder );
  private:
	void					UpdateSameFolderMenu();//B0420
	AIndex					FindSameFolderIndex( const AFileAcc& inFolder );
	AIndex								mCurrentSameFolder;
	AObjectArray<AFileAcc>				mSameFolder_Folder;
	AObjectArray<ATextArray>			mSameFolder_Filename;
	mutable AThreadMutex				mSameFolderMutex;
	
	//「同じプロジェクト」
  public:
	AIndex					SPI_RegisterProjectFolder( const AFileAcc& inProjectFolder );
	void					SPI_UpdateSameProjectMenu( const AFileAcc& inProjectFolder, const AModeIndex inModeIndex );
	//ファイルリストウインドウ用データ取得メソッド
	AIndex					SPI_GetSameProjectArrayIndex( const AFileAcc& inFolder ) const;
	void					SPI_GetCurrentSameProjectData( AText& outProjectFolderPath,
														  ATextArray& ouFullPathArray, AArray<AModeIndex>& outModeArray,
														  ABoolArray& outIsFolderArray ) const;//#533
	void					SPI_GetCurrentProjectPathForDisplay( AText& outText ) const;
	void					SPI_GetSameProjectFile( const AIndex inIndex, AFileAcc& outFile ) const;
	void					SPI_GetSameProjectFolder( AFileAcc& outFolder ) const;//#892
	AIndex					SPI_GetCurrentSameProjectIndex() const { return mCurrentSameProjectIndex; }
	const ATextArray&		SPI_GetSameProjectPathnameArray() const;//R0000
	const ATextArray&		SPI_GetSameProjectPathnameArray( const AIndex inProjectIndex ) const;//win 080722
	//#892 void					SPI_GetProjectSCMStateArray( AArray<ASCMFileState>& outSCMStateArray ) const;//R0006
	void					SPI_UpdateAllSameProjectData();//B0420
	void					SPI_GetImportFileDataIDArrayForWordsList( const AFileAcc& inProjectFolder, const AIndex inModeIndex, 
							AArray<AObjectID>& outImportFileDataIDArray, const ABool& inAbort ) ;
	//#723 AIndex					SPI_FindProjectIndex( const AFileAcc& inFile ) const;//#723
	void					SPI_GetProjectFolderCollapseData( const AIndex inProjectIndex, ATextArray& outCollapseData ) const;//#892
	void					SPI_SaveProjectFolderCollapseData( const AIndex inProjectIndex, const ATextArray& inCollapseData );//#892
	void					SPI_GetSameProjectPathnameArrayMatchMode( const AModeIndex inModeIndex, ATextArray& outPathnameArray) const;
  private:
	//#402 void					MakeSameProjectArrayRecursive( const AIndex inSameProjectArrayIndex, const AFileAcc& inFolder, const AModeIndex inModeIndex );
	void					GetSameProjectMenuText( const AIndex inIndex, AText& outText ) const;
	void					UpdateSameProjectMenu();//B0420 #533
	AIndex								mCurrentSameProjectIndex;
	AModeIndex							mCurrentSameProjectModeIndex;//#533
	AObjectArray<AFileAcc>				mSameProject_ProjectFolder;
	//#533 AArray<AModeIndex>					mSameProject_ModeIndex;
	AObjectArray<ATextArray>			mSameProjectArray_Pathname;
	AObjectArray< AArray<AModeIndex> >	mSameProjectArray_ModeIndex;//#533
	AObjectArray<ATextArray>			mSameProjectArray_CollapseData;//#892
	AObjectArray<ABoolArray>			mSameProjectArray_IsFolder;
	mutable AThreadMutex				mSameProjectArrayMutex;//#723
	//キーワードCSVファイルロード #796
  public:
	void					SPI_LoadCSVForKeyword( const AText& inModeName, const AText& inURL, const AIndex inCategoryIndex );
  private:
	void					LoadCSVForKeyword_Result( const AText& inResultText, const ATextArray& inInfoTextArray );
	
	//
  public:
	void					SPI_UpdateImportFileMenu( const AObjectArray<AFileAcc>& inFileArray );
	
	//インポートファイル
  public:
	AImportFileManager&		SPI_GetImportFileManager() { return mImportFileManager; }
  private:
	AImportFileManager					mImportFileManager;
	
	//モードメニュー管理
  public:
	void					SPI_UpdateModeMenu();
	void					SPI_UpdateModeMenuShortcut();
	
	//テキストエンコーディングメニュー管理
  public:
	void					SPI_UpdateTextEncodingMenu();
	
	/*#193
	//FTPフォルダメニュー管理
  public:
	void					SPI_UpdateFTPFolderMenu( const AModificationType inModificationType, const AIndex inIndex );
  private:
	void					GetFTPFolderFTPURL( const AIndex inIndex, AText& outText ) const;
	void					DoFTPFolderMenuSelected( const AText& inActionText );
	void					MakeFTPFolderMenu();
	*/
	
	//フォルダリストメニュー管理
  public:
	void					SPI_UpdateFolderListMenu( const AModificationType inModificationType, const AIndex inIndex );
  private:
	AObjectID				MakeFolderListMenu_SubMenu( const AFileAcc& inFolder, const ANumber inDepth );
	void					MakeFolderListMenu();
	
	//ツールメニュー
  public:
	void					SPI_UpdateToolMenu( const AModeIndex inModeIndex );
  private:
	AModeIndex							mCurrentToolMenuModeIndex;
	
	//Transliterate
  public:
	ABool					SPI_Transliterate( const AText& inTitle, const AText& inText, AText& outText ) const;//R0017
  private:
	AObjectArray<ATextArray>			mTransliterate_Source;
	AObjectArray<ATextArray>			mTransliterate_Destination;
	ATextArray							mTransliterate_Title;
	
	//テキストファイル読み込み・書き込み
  public:
	void					SPI_LoadTextFromFileOrDocument( const ALoadTextPurposeType inLoadTextPurposeType,
														  const AFileAcc& inFile, AText& outText, AModeIndex& outModeIndex,
														  AText& outTextEncoding );//B0313
	void					SPI_LoadTextFromFile( const ALoadTextPurposeType inLoadTextPurposeType,
												const AFileAcc& inFile, AText& outText );//#898
	void					SPI_LoadTextFromFile( const ALoadTextPurposeType inLoadTextPurposeType,
												 const AFileAcc& inFile, 
												 AText& outText, AModeIndex& outModeIndex, AText& outTextEncodingName );//#723
	ABool					SPI_ConvertToUTF8CRUsingDocPref( const ALoadTextPurposeType inLoadTextPurposeType,
															AText& ioText, const ADocPrefDB& inDocPrefDB, const AText& inFilePath,
															ATextEncoding& outResultEncoding,
															AReturnCode& outResultReturnCode,
															 ATextEncodingFallbackType& outResultFallbackType,
															 ABool& outNotMixed ) const;//#898 #995
	void					SPI_WriteTextFile( const AText& inText, const AFileAcc& inFile );//B0310
  private:
	ABool					GuessTextEncoding( const AText& inText, const AText& inFilePath, ATextEncoding& ioTextEncoding ) const;//#898
	
	//モード取得
  public:
	AModeIndex				SPI_GetModeIndexFromFile( const AFileAcc& inFile ) const;
	
	//
  public:
	ABool					SPI_ConvertToUTF8CR( AText& ioText, const ATextEncoding& inTextEncoding, const ABool inFromExternalData,
												 const ABool inUseFallback, const ABool inUseLosslessFallback,
												 const AReturnCode inDefaultReturnCode,//改行コードが無かった場合のデフォルト改行コード
												 ATextEncoding& outResultEncoding,
												 AReturnCode& outResultReturnCode,
												 ATextEncodingFallbackType& outResultFallbackType,
												 ABool& outNotMixed ) const;//#182 #307 #473 #764 #995
private:
	/*#764
	virtual ABool			NVIDO_ConvertToUTF8CRAnyway( AText& ioText, 
							const ATextEncoding& inPrefTextEncoding, const ATextEncoding& inGuessTextEncoding,
							const ABool inFromExternalData, const ABool inUseFallback, const ABool inUseLosslessFallback,
							const ABool inOnlyPrefTextEncoding, 
							ATextEncoding& outResultEncoding,
							AReturnCode& outResultReturnCode,
	ATextEncodingFallbackType& outResultFallbackType ) const;//#307 #473
	*/
	/*#844
	ABool					ConvertFromSJISToUTF8UsingLosslessFallback( AText& ioText, 
							const ABool inFromExternalData, const ABool inUseFallback ) const;//#473
	*/
	
	//SCMFolder R0006
  public:
	ASCMFolderManager&		SPI_GetSCMFolderManager() { return mSCMFolderManager; }
  private:
	ASCMFolderManager					mSCMFolderManager;
	
  public:
	void					SPI_SCMFolderDataUpdated( const AFileAcc& inFolder );
	void					SPI_UpdateSCMDisplayAll();
	void					UpdateAllSCMData();
  private:
	void					SCMRegisterFolderByFile( const AFileAcc& inFile );
	void					SCMRegisterFolder( const AFileAcc& inFolder );
	void					SCMUpdateFolderStatusByFile( const AFileAcc& inFile );
	
	//テンポラリテキスト保存 B0446
  public:
	void					SPI_SaveTemporaryText( const AText& inTempText );
  private:
	void					DeleteTemporaryTextFile();
	
	//名称未設定ドキュメント番号
  public:
	ANumber					SPI_GetAndIncrementUntitledDocumentNumber() {mUntitledDocumentNumber++;return mUntitledDocumentNumber;}
  private:
	ANumber								mUntitledDocumentNumber;
	
	//スレッド間通信
  public:
	AText&					SPI_GetThreadDataReceiver_Text( const AObjectID& inObjectID ) 
	{
		AStMutexLocker	mutexlocker(mThreadDataReceiver_Text_Mutex);//#0
		return mThreadDataReceiver_Text.GetObjectByID(inObjectID);
	}
	
	AObjectID				SPI_MakeNewThreadDataReceiver_MultiFileFindResult()
	{
		AStMutexLocker	mutexlocker(mThreadDataReceiver_MultiFileFindResult_Mutex);//#0
		AIndex index = mThreadDataReceiver_MultiFileFindResult.AddNewObject();
		return mThreadDataReceiver_MultiFileFindResult.GetObject(index).GetObjectID();
	}
	void					SPI_Delete1ThreadDataReceiver_MultiFileFindResult( const AObjectID& inObjectID )
	{
		AStMutexLocker	mutexlocker(mThreadDataReceiver_MultiFileFindResult_Mutex);//#0
		AIndex	index = mThreadDataReceiver_MultiFileFindResult.GetIndexByID(inObjectID);
		mThreadDataReceiver_MultiFileFindResult.Delete1Object(index);
	}
	AMultiFileFindResult&	SPI_GetThreadDataReceiver_MultiFileFindResult( const AObjectID inObjectID )
	{
		AStMutexLocker	mutexlocker(mThreadDataReceiver_MultiFileFindResult_Mutex);//#0
		return mThreadDataReceiver_MultiFileFindResult.GetObjectByID(inObjectID);
	}
  private:
	AObjectArrayIDIndexed<AText>		mThreadDataReceiver_Text;//#693
	AThreadMutex						mThreadDataReceiver_Text_Mutex;//#0
	AObjectArrayIDIndexed<AMultiFileFindResult>	mThreadDataReceiver_MultiFileFindResult;//#693
	AThreadMutex						mThreadDataReceiver_MultiFileFindResult_Mutex;//#0
	
	//
  public:
	void					SPI_GetInlineInputHiliteColor( const AIndex inIndex, AColor& outColor ) const;
  private:
	AArray<AColor>						mInlineInputHiliteColorArray_Gray;
	AArray<AColor>						mInlineInputHiliteColorArray_Color;
	
	//更新反映
  public:
	//#725 void					SPI_UpdatePropertyJumpListAll();//#291
	//#725 void					SPI_HideOtherJumpList( const AWindowID inWindowID );//#240
	
	//ジャンプリスト履歴 #454
  public:
	void					SPI_UpdateJumpListHistory( const AText& inSelectionText, const AFileAcc& inDocFile );
  private:
	ATextArray							mJumpListHistory;
	ATextArray							mJumpListHistory_Path;
	
	//検索パラメータ #693
  public:
	void					SPI_GetFindParam( AFindParameter& outParam ) const;
	void					SPI_SetFindParam( const AFindParameter& inFindParam );
	void					SPI_SetFindText( const AText& inText );
	void					SPI_SetReplaceText( const AText& inText );
	void					SPI_GetFindParamToolCommand( const AFindParameter& inParam, AText& outText, const char* inCstring ) const;
	void					SPI_GetReplaceParamToolCommand( const AFindParameter& inParam, const AReplaceRangeType inReplaceRange, AText& outText ) const;
	static void				GetToolCommandFindOptionFromFindParameter( const AFindParameter& inParameter, const ABool inMultiFileFind, AText& outText );
	void					SPI_AddRecentlyUsedFindText( const AFindParameter& inParam );
	void					SPI_AddRecentlyUsedReplaceText( const AFindParameter& inParam );
  private:
	void					InitFindParam();
	AFindParameter						mFindParam;
	
	//リモート接続 #361
  public:
	AObjectID				SPI_CreateRemoteConnection( const ARemoteConnectionType inType, const AObjectID inRequestorObjectID );
	void					SPI_DeleteRemoteConnection( const AObjectID inRemoteConnectionObjectID );
	ARemoteConnection&		SPI_GetRemoteConnection( const AObjectID inAccessPluginConnectionObjectID )
	{
		return mRemoteConnectionArray.GetObjectByID(inAccessPluginConnectionObjectID);
	}
	AObjectID				SPI_GetTextDocumentIDByRemoteFileURL( const AText& inRemoteFileURL ) const;
	AObjectID				SPI_FindRemoteConnectionByRemoteURL( const AText& inRemoteURL ) const;//#1228
	//#1231 AIndex					FindAccessPluginIndexFromURL( const AText& inURL );
	void					SPI_RequestOpenFileToRemoteConnection( const AText& inRemoteFileURL, const ABool inAllowNoFile );
	//#1231 void					SPI_RequestOpenFileToRemoteConnection( const AText& inConnectionType, const AText& inFilePath, const ABool inAllowNoFile );
	void					SPNVI_CreateDocumentFromRemoteFile( const AObjectID inRemoteConnectionObjectID, 
							const AText& inRemoteFileURL, const AText& inDocumentText );
	ABool					SPI_GetPluginPath( const AText& inConnectionTypeName, AText& outPluginPath );
	void					SPI_DoAccessPluginResponseReceived( const AObjectID inAccessPluginConnectionObjectID, const AText& inText, 
																const AText& inStderrText );//#1231
  private:
	AObjectArrayIDIndexed<ARemoteConnection>	mRemoteConnectionArray;
	
	//<所有クラス(ADocument)生成／削除／取得>
  public:
	AObjectID				SPNVI_CreateNewTextDocument( const AModeIndex inModeIndex = kModeIndex_StandardMode, 
							const AText& inTextEncodingName = GetEmptyText(), const AText& inDocumentText = GetEmptyText(),
							const ABool inSelect = true,
							const AWindowID inCreateTabInWindowID = kObjectID_Invalid, 
							const ABool inAlwaysCreateWindow = false,
							const ARect inInitialWindowBounds = kRect_0000);//sdfcheck
	AObjectID				SPNVI_CreateLuaConsoleDocument();//#567 #1170
	ADocumentID				SPNVI_CreateDiffTargetDocument( const AText& inTitle, const AText& inDocumentText,
							const AModeIndex inModeIndex, const AText& inTextEncodingName,
							const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd, 
							const ABool inSelect, const ABool inSelectableTab = true,
							const AWindowID inCreateTabInWindowID = kObjectID_Invalid );//#379
	ADocumentID				SPNVI_CreateDiffTargetDocument( const AText& inTitle, const AFileAcc& inFile,
							const AModeIndex inModeIndex, const AText& inTextEncodingName,
							const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd, 
							const ABool inSelect, const ABool inSelectableTab = true,
							const AWindowID inCreateTabInWindowID = kObjectID_Invalid );//#379
	AObjectID				SPNVI_CreateDocumentFromLocalFile( const AFileAcc& inFile, 
							const AText& inTextEncodingName = GetEmptyText(), 
							const AObjectID inDocImpID = kObjectID_Invalid,//#1078
							const ABool inJudgeBigFile = true, 
							const AIndex inInitialSelectStart = kIndex_Invalid, const AIndex inInitialSelectEnd = kIndex_Invalid,
							const ABool inSelect = true, const AWindowID inCreateTabInWindowID = kObjectID_Invalid, 
							const ABool inAlwaysCreateWindow = false, //win 080715 #212
							const ARect inInitialWindowBounds = kRect_0000,
							const ABool inSetInitialSideBarDisplay = false, const ABool inLeftSideBar = false, 
							const ABool inRightSideBar = false, const ABool inSubText = false );//#850
	void					SPI_CreateNewWindowForExistingTextDocument( const ADocumentID inTextDocumentID,
							const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd );//#856
	//B0389 void					SPNVI_CreateDocumentByPathForDisplay( const AText& inFilePath );
	//#361 void					SPNVI_CreateTextDocumentFTP( const AText& inFTPURL, const AText& inDocumentText );
	void					SPNVI_CreateDocumentFromTemplateFile( const AFileAcc& inFile );
	AObjectID				SPI_CreateWindowOrTabForNewTextDocument( const ADocumentID inDocumentID, 
							const AIndex inInitialSelectStart, const AIndex inInitialSelectEnd,
							const ABool inSelect, const ABool inSelectableTab = true,
							const AWindowID inCreateTabInWindowID = kObjectID_Invalid,
							const ABool inAlwaysCreateWindow = false,//#379 #567
							const ARect inInitialWindowBounds = kRect_0000,
							const ABool inSetInitialSideBarDisplay = false, const ABool inLeftSideBar = false, 
							const ABool inRightSideBar = false, const ABool inSubText = false );//#850
	AObjectID				SPI_CreateDocImp( const ADocumentType inDocType );//#1034
	ADocumentID				SPI_OpenOrRevealTextDocument( const AText& inFilePath, const AIndex inSpos, const AIndex inEpos, 
							const AWindowID inTextWindowID, const ABool inInAnotherView,
							const AAdjustScrollType inAdjustScrollType );
	ADocumentType			GetDocumentTypeForFile( const AFileAcc& inFile );
	//#361 void					SPI_StartOpenFTPDocument( const AText& inFTPURL, const ABool inAllowNoFile );
	ADocument_Text&			SPI_GetTextDocumentByID( const AObjectID inDocID );
	const ADocument_Text&	SPI_GetTextDocumentConstByID( const AObjectID inDocID ) const;
	//#361 AObjectID				SPI_GetTextDocumentIDByFTPURL( const AText& inFTPURL );
	//#725 void					SPI_SelectTextDocument( const AFileAcc& inFile, const AIndex inStartIndex, const AItemCount inLength );
	void					SPI_RecalcWordWrapAll( const AModeIndex inModeIndex );
	void					SPI_RecalcAll( const AModeIndex inModeIndex );
	void					SPI_ReSetModeAll( const AModeIndex inModeIndex );//#349
	void					SPI_ReRecognizeSyntaxAll( const AModeIndex inModeIndex );
	void					SPI_StopSyntaxRecognizerThreadAll( const AModeIndex inModeIndex );//#890
	void					SPI_UpdateRegExpForAllDocuments( const AModeIndex inModeIndex );//#683
	void					SPI_DoTextDocumentSaved( const AFileAcc& inFile );
	void					SPI_DoTextDocumentClosed( const AFileAcc& inFile, const AFileAcc& inProjectFolder );//#896
	void					SPI_TransitNextDocumentToWrapCalculatingState();//#699
	void					SPI_DoAutoBackupFolderChangedAll();//#81
	void					SPI_TextFontSetInModeIsUpdatedAll( const AIndex inUpdatedModeIndex );//#868
	
	void					SPI_DeleteSpellCheckCache( const AModeIndex inModeIndex );//R0199
	void					SPI_UpdateProjectFolder();//RC3
	//#81 ABool					SPI_UpdateDiffFileTargetForAllDocuments();//#379
	//void					SPI_UpdateDiffTargetDocumentWrap();//#379
	void					SPI_DoSCMStatusUpdated( const AFileAcc& inFolder );//#379
	
	//
	ABool					SPI_GetMostFrontDocumentParentFolder( AFileAcc& outFolder ) const;//#868
	
	//トータルのバイト数取得
	AByteCount				SPI_GetTotalTextDocumentMemoryByteCount() const;
	
	//<所有クラス(ADocument_IndexWindow)生成／削除／取得>
  public:
	ADocument_IndexWindow&	SPI_GetIndexWindowDocumentByID( const AObjectID inDocID );
	const ADocument_IndexWindow&	SPI_GetIndexWindowDocumentConstByID( const AObjectID inDocID ) const;
	
	//<所有クラス(AWindow_Text)生成／削除／取得>
  public:
	AWindow_Text&			SPI_GetTextWindowByID( const AObjectID inWindowID );
	const AWindow_Text&			SPI_GetTextWindowConstByID( const AObjectID inWindowID ) const;//#850
	//#850 AWindowID				SPI_GetTabModeMainTextWindowID();
	AWindowID				SPI_GetMostNumberTabsTextWindowID() const;//#850 
	void					SPI_UpdateTextWindowViewBoundsAll();
	void					SPI_UpdateTextDrawPropertyAll( const AModeIndex inModeIndex );
	void					SPI_ShowHideRightSidebarAll();//#1350
	void					SPI_ShowHideLeftSidebarAll();//#1350
	void					SPI_UpdateLineImageInfo( const AModeIndex inModeIndex );//#450
	//#291 void					SPI_UpdatePropertyJumpListAll();
	//#291 void					SPI_UpdateJumpListWithColor();//R0006
	void					SPI_RemakeToolButtonsAll( const AModeIndex inModeIndex );
	//#725 void					SPI_UpdatePaneLayout();//#291
	void					SPI_UpdateToolbarItemValue( const ADocumentID inDocumentID );//#379
	void					SPI_UpdateToolbarItemValueAll();
	void					SPI_UpdateTextDrawProperty( const ADocumentID inDocumentID );//#379
	void					SPI_UpdateViewBounds( const ADocumentID inDocumentID );//#379
	//#725 void					SPI_UpdateInfoHeader( const ADocumentID inDocumentID );//#379
	void					SPI_TryCloseTextDocument( const ADocumentID inDocumentID );//#379
	void					SPI_ExecuteCloseTextDocument( const ADocumentID inDocumentID );//#379
	void					SPI_UpdateDiffDisplay( const ADocumentID inDocumentID );//#379
	void					SPI_RefreshCaretAll();//#844
	void					SPI_UpdateMacroBarItemContentAll();//#724
	void					SPI_UpdateWindowAlphaAll();//#1255
	
	//<所有クラス(AWindow_XXX)生成／削除／取得>
  public:
	AWindow_AppPref&		SPI_GetAppPrefWindow()//#199
	{
		//#693
		if( mAppPrefWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_AppPref>	appPrefWindowFactory;
			mAppPrefWindowID = NVI_CreateWindow(appPrefWindowFactory);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_AppPref,kWindowType_AppPref,mAppPrefWindowID);
	}
  private:
	AWindowID							mAppPrefWindowID;
	
	/*#725
  public:
	AWindow_FileList&		SPI_GetFileListWindow()//#199
	{
		//#693
		if( mFileListWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_FileList>	fileListWindow;
			mFileListWindowID = NVI_CreateWindow(fileListWindow);
			SPI_GetFileListWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_FileList,kWindowType_FileList,mFileListWindowID);
	}
	void					SPI_ShowHideFileListWindow( const ABool inShow );//#291
  private:
	AWindowID							mFileListWindowID;
	
  public:
	AWindow_IdInfo&			SPI_GetIdInfoWindow()//#199
	{
		//#693
		if( mIdInfoWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_IdInfo>	idInfoWindowFactory;
			mIdInfoWindowID = NVI_CreateWindow(idInfoWindowFactory);
			SPI_GetIdInfoWindow().NVI_Create(kObjectID_Invalid);//RC2
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_IdInfo,kWindowType_IdInfo,mIdInfoWindowID);
	}
	void					SPI_ShowHideIdInfoWindow( const ABool inShow );//#291
  private:
	AWindowID							mIdInfoWindowID;
  public:
	AView_IdInfo&			SPI_GetIdInfoView() { return SPI_GetIdInfoWindow().SPI_GetIdInfoView(); }//#238
	
  public:
	AWindow_CandidateList&	SPI_GetCandidateListWindow()//#199
	{
		//#693
		if( mCandidateListWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_CandidateList>	candidateListWindowFactory;
			mCandidateListWindowID = NVI_CreateWindow(candidateListWindowFactory);
			SPI_GetCandidateListWindow().NVI_Create(kObjectID_Invalid);//RC2
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_CandidateList,kWindowType_CandidateList,mCandidateListWindowID);
	}
  private:
	AWindowID							mCandidateListWindowID;
	*/
	/*#922
  public:
	AWindow_FTPFolder&		SPI_GetFTPFolderWindow() //#199
	{
		//#693
		if( mFTPFolderWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_FTPFolder>	ftpFolderWindowFactory;
			mFTPFolderWindowID = NVI_CreateWindow(ftpFolderWindowFactory);
			SPI_GetFTPFolderWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_FTPFolder,kWindowType_FTPFolder,mFTPFolderWindowID);
	}
  private:
	AWindowID							mFTPFolderWindowID;
	
  public:
	AWindow_FolderList&		SPI_GetFolderListWindow() //#199
	{
		//#693
		if( mFolderListWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_FolderList>	folderListWindowFactory;
			mFolderListWindowID = NVI_CreateWindow(folderListWindowFactory);
			SPI_GetFolderListWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_FolderList,kWindowType_FolderList,mFolderListWindowID);
	}
  private:
	AWindowID							mFolderListWindowID;
	*/
	/*#725
  public:
	AWindow_KeyToolList&	SPI_GetKeyToolListWindow()//#199
	{
		//#693
		if( mKeyToolListWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_KeyToolList>	keyToolListWindowFactory;
			mKeyToolListWindowID = NVI_CreateWindow(keyToolListWindowFactory);
			SPI_GetKeyToolListWindow().NVI_Create(kObjectID_Invalid);//R0073
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_KeyToolList,kWindowType_KeyToolList,mKeyToolListWindowID);
	}
  private:
	AWindowID							mKeyToolListWindowID;
	*/
  public:
	AWindow_FTPLog&			SPI_GetFTPLogWindow() //#199
	{
		//#693
		if( mFTPLogWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_FTPLog>	ftpLogWindowFactory;
			mFTPLogWindowID = NVI_CreateWindow(ftpLogWindowFactory);
			SPI_GetFTPLogWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_FTPLog,kWindowType_FTPLog,mFTPLogWindowID);
	}
  private:
	AWindowID							mFTPLogWindowID;
	
  public:
	AWindow_TraceLog&		SPI_GetTraceLogWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TraceLog,kWindowType_TraceLog,mTraceLogWindowID);
  private:
	AWindowID							mTraceLogWindowID;
	
	/*#695
  public:
	AWindow_BigFileAlert&	SPI_GetBigFileAlertWindow() //#199
	{
		//#693
		if( mBigFileAlertWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_BigFileAlert>	bigFileAlertFactory;
			mBigFileAlertWindowID = NVI_CreateWindow(bigFileAlertFactory);
			SPI_GetBigFileAlertWindow().NVI_Create(kObjectID_Invalid);//R0208
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_BigFileAlert,kWindowType_BigFileAlert,mBigFileAlertWindowID);
	}
  private:
	AWindowID							mBigFileAlertWindowID;
	*/
	
  public:
	AWindow_MultiFileFindProgress&	SPI_GetMultiFileFindProgress()//#199
	{
		//#693
		if( mMultiFileFindProgressWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_MultiFileFindProgress>	multiFileFindProgressWindowFactory;
			mMultiFileFindProgressWindowID = NVI_CreateWindow(multiFileFindProgressWindowFactory);
			SPI_GetMultiFileFindProgress().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_MultiFileFindProgress,kWindowType_MultiFileFindProgress,mMultiFileFindProgressWindowID);
	}
  private:
	AWindowID							mMultiFileFindProgressWindowID;
	
  public:
	AWindow_FTPProgress&	SPI_GetFTPProgress()//#199 
	{
		//#693
		if( mFTPProgressWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_FTPProgress>	ftpProgressWindowFactory;
			mFTPProgressWindowID = NVI_CreateWindow(ftpProgressWindowFactory);
			SPI_GetFTPProgress().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_FTPProgress,kWindowType_FTPProgress,mFTPProgressWindowID);
	}
  private:
	AWindowID							mFTPProgressWindowID;
	
  public:
	AWindow_About&			SPI_GetAboutWindow() //#199
	{
		//#693
		if( mAboutWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_About>	aboutWindowFactory;
			mAboutWindowID = NVI_CreateWindow(aboutWindowFactory);
			SPI_GetAboutWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_About,kWindowType_About,mAboutWindowID);
	}
  private:
	AWindowID							mAboutWindowID;
	
	//<所有クラス(AWindow_MultiFileFind)>
  public:
	AWindow_MultiFileFind&	SPI_GetMultiFileFindWindow() //#199
	{
		//#693
		if( mMultiFileFindWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_MultiFileFind>	multiFileFindWindow;
			mMultiFileFindWindowID = NVI_CreateWindow(multiFileFindWindow);
			SPI_GetMultiFileFindWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_MultiFileFind,kWindowType_MultiFileFind,mMultiFileFindWindowID);
	}
	void					SPI_AddRecentlyUsed( const AText& inFindText, const AFileAcc& inFolder );
	void					SPI_UpdateMultiFileFindWindow();
	void					SPI_ShowMultiFileFindWindow( const AText& inFindText );
  private:
	AWindowID							mMultiFileFindWindowID;
	
	//<所有クラス(AWindow_Find)>
  public:
	AWindow_Find&			SPI_GetFindWindow()
	{
		//#693
		if( mFindWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_Find>	findWindow;
			mFindWindowID = NVI_CreateWindow(findWindow);
			SPI_GetFindWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Find,kWindowType_Find,mFindWindowID);//#199
	}
	void					SPI_ShowFindWindow( const AText& inFindText );
	void					SPI_UpdateFindWindow();
  private:
	AWindowID							mFindWindowID;//#199
	
	//<所有クラス(AWindow_ModePref)>
  public:
	AWindow_ModePref&		SPI_GetModePrefWindow( const AIndex inModeIndex );
	/*#868{
		//#693
		if( mModePrefWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_ModePref>	modePrefWindowFactory;
			mModePrefWindowID = NVI_CreateWindow(modePrefWindowFactory);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ModePref,kWindowType_ModePref,mModePrefWindowID);//#199
		}
		*/
	void					SPI_ShowModePrefWindow( const AModeIndex inModeIndex );
	ABool					SPI_IsModePrefWindowCreated( const AIndex inModeIndex ) const;
  private:
	//#868 AWindowID							mModePrefWindowID;//#199
	AArray<AWindowID>					mModePrefWindowIDArray;//#868
	
	//<所有クラス(AWindow_TextCount)>
  public:
	AWindow_TextCount&		SPI_GetTextCountWindow()
	{
		//#693
		if( mTextCountWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_TextCount>	textCountWindowFactory;
			mTextCountWindowID = NVI_CreateWindow(textCountWindowFactory);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextCount,kWindowType_TextCount,mTextCountWindowID);//#199
	}
  private:
	AWindowID							mTextCountWindowID;//#199
	
	//<所有クラス(AWindow_NewFTPFile)>
  public:
	AWindow_NewFTPFile&		SPI_GetNewFTPFileWindow()
	{
		//#693
		if( mNewFTPFileWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_NewFTPFile>	newFTPFileWindowFactory;
			mNewFTPFileWindowID = NVI_CreateWindow(newFTPFileWindowFactory);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_NewFTPFile,kWindowType_NewFTPFile,mNewFTPFileWindowID);
	}
  private:
	AWindowID							mNewFTPFileWindowID;//#199
	
	/*#858
	//#138
	//<所有クラス(AWindow_FusenList)>
  public:
	AWindow_FusenList&		SPI_GetFusenListWindow()
	{
		//#693
		if( mFusenListWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_FusenList>	fusenListWindow;
			mFusenListWindowID = NVI_CreateWindow(fusenListWindow);
			SPI_GetFusenListWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_FusenList,kWindowType_FusenList,mFusenListWindowID);
	}
	void					SPI_SetFusenListActiveDocument( const AObjectID inProjectObjectID, const AText& inRelativePath );
  private:
	AWindowID							mFusenListWindowID;
	
	//#138
	//<所有クラス(AWindow_FusenEdit)>
  public:
	AWindow_FusenEdit&		SPI_GetFusenEditWindow()
	{
		//#693
		if( mFusenEditWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_FusenEdit>	fusenEditWindow;
			mFusenEditWindowID = NVI_CreateWindow(fusenEditWindow);
			SPI_GetFusenEditWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_FusenEdit,kWindowType_FusenEdit,mFusenEditWindowID);
	}
  private:
	AWindowID							mFusenEditWindowID;
	*/
	/*#725
	//#390
	//<所有クラス(AWindow_FusenEdit)>
  public:
	AWindow_KeyRecord&		SPI_GetKeyRecordWindow()
	{
		//#693
		if( mKeyRecordWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_KeyRecord>	keyRecordWindow;
			mKeyRecordWindowID = NVI_CreateWindow(keyRecordWindow);
			SPI_GetKeyRecordWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_KeyRecord,kWindowType_KeyRecord,mKeyRecordWindowID);
	}
  private:
	AWindowID							mKeyRecordWindowID;
	*/
	/*#379
	//#379
	//<所有クラス(AWindow_Diff)>
  public:
	AWindow_Diff&			SPI_GetDiffWindow()
	{
		//#693
		if( mDiffWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_Diff>	diffWindow;
			mDiffWindowID = NVI_CreateWindow(diffWindow);
			SPI_GetDiffWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Diff,kWindowType_Diff,mDiffWindowID);
	}
  private:
	AWindowID							mDiffWindowID;
	*/
	//#467
	//<所有クラス(AWindow_EnableDefines)>
  public:
	AWindow_EnabledDefines&	SPI_GetEnabledDefinesWindow()
	{
		//#693
		if( mEnabledDefinesWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_EnabledDefines>	enabledDefinesWindow;
			mEnabledDefinesWindowID = NVI_CreateWindow(enabledDefinesWindow);
			SPI_GetEnabledDefinesWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_EnabledDefines,kWindowType_EnabledDefines,mEnabledDefinesWindowID);
	}
  private:
	AWindowID							mEnabledDefinesWindowID;
	
	//#844
	//新規モード追加ウインドウ
  public:
	AWindow_AddNewMode&		SPI_GetAddNewModeWindow()
	{
		if( mAddNewModeWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_AddNewMode>	addNewModeWindowFactory;
			mAddNewModeWindowID = NVI_CreateWindow(addNewModeWindowFactory);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_AddNewMode,kWindowType_AddNewMode,mAddNewModeWindowID);
	}
  private:
	AWindowID							mAddNewModeWindowID;
	
	//ジャンプメニュー #857
  public:
	void					SPI_UpdateJumpMenu( const AObjectID inDynamicMenuObjectID, const AIndex inCheckItemIndex );
  private:
	AObjectID							mCurrentJumpMenu_DynamicMenuObjectID;
	AIndex								mCurrentJumpMenu_CheckItemIndex;
	
	//#725
	//サブウインドウ
  public:
	AWindowID				SPI_CreateSubWindow( const ASubWindowType inType, const ANumber inParameter, const AWindowID inTextWindowID,
							const ASubWindowLocationType inLocationType, const AIndex inLocationIndex, const ABool inCollapsed );
	void					SPI_DeleteSubWindow( const AWindowID inSubWindowID );
	void					SPI_CloseSubWindow( const AWindowID inSubWindowID );
	void					SPI_GetSubWindowProperty( const AWindowID inSubWindowID,
							ASubWindowLocationType& outLocationType, AIndex& outLocationIndex ) const;
	void					SPI_SetSubWindowProperty( const AWindowID inSubWindowID,
							const ASubWindowLocationType inLocationType, const AIndex inLocationIndex, const AWindowID inTextWindowID );
	ABool					SPI_IsSubWindowSideBarBottom( const AWindowID inSubWindowID ) const;
	ASubWindowLocationType	SPI_GetSubWindowLocationType( const AWindowID inSubWindowID ) const;
	ASubWindowType			SPI_GetSubWindowType( const AWindowID inSubWindowID ) const;
	ANumber					SPI_GetSubWindowParameter( const AWindowID inSubWindowID ) const;
	void					SPI_UpdateLayoutOverlaySubWindow( const AWindowID inTextWindowID, const ASubWindowType inType, const AWindowID inSubWindowID,
							const AWindowPoint inPoint, const ANumber inWidth, const ANumber inHeight, const ABool inAnimate = false );
	void					SPI_MoveOverlaySubWindow( const ASubWindowLocationType inSrcType, const AIndex inSrcIndex,
							const ASubWindowLocationType inDstType, const AIndex inDstIndex );
	void					SPI_AddOverlaySubWindow( const ASubWindowLocationType inLocationType, const AIndex inIndex,
							const ASubWindowType inSubWindowType );
	void					SPI_GetSubWindowsFont( AText& outFontName, AFloatNumber& outFontSize ) const;
	AColor					SPI_GetSubWindowBackgroundColor( const ABool inActive ) const;
	void					SPI_GetSubWindowBoxColor( const AWindowID inSubWindowID, 
													 AColor& outLetterColor, AColor& outDropShadowColor, AColor& outBoxColor1, AColor& outBoxColor2, AColor& outBoxColor3 ) const;
	//AColor					SPI_GetSideBarEmptySpaceColor() const;
	AColor					SPI_GetSubWindowRoundedRectBoxSelectionColor( const AWindowID inSubWindowID ) const;
	AColor					SPI_GetSubWindowRoundedRectBoxHoverColor() const;
	AColor					SPI_GetSubWindowLetterColor() const;
	AColor					SPI_GetSideBarFrameColor() const;
	AColor					SPI_GetSubWindowHeaderBackgroundColor() const;
	AColor					SPI_GetSubWindowHeaderLetterColor() const;
	AColor					SPI_GetSubWindowTitlebarTextColor() const;
	AColor					SPI_GetSubWindowTitlebarTextDropShadowColor() const;
	AColor					SPI_GetSubWindowTitlebarButtonHoverColor() const;
	AFloatNumber			SPI_GetPopupWindowAlpha() const;
	AFloatNumber			SPI_GetFloatingWindowAlpha() const;
	ANumber					SPI_GetSubWindowTitleBarHeight() const;
	ANumber					SPI_GetSubWindowSeparatorHeight() const;
	ANumber					SPI_GetSubWindowMinHeight( const AWindowID inSubWindowID ) ;
	ANumber					SPI_GetSubWindowCollapsedHeight() const { return 19; }
	void					SPI_UpdateSubWindowsProperty();
	void					SPI_ClearSubWindowsModeRelatedData( const AIndex inModeIndex );
	void					SPI_ShowHideFloatingSubWindowsTemporary();
	void					SPI_UpdateFloatingSubWindowsVisibility();//#959
	AWindowID				SPI_CreateFloatingSubWindow( const ASubWindowType inSubWindowType, const ANumber inParameter, 
							const ANumber inWidth, const ANumber inHeight );
	void					SPI_DockSubWindowsToSubWindows( ARect& ioWindowRect ) const;
	void					SPI_AdjustAllFlotingWindowsZOrder();
	static const ANumber				kSubWindowMinWidth = 100;
	void					SPI_SetActiveSubWindowForItemSelector( const AWindowID inWindowID );
	AWindowID				SPI_GetActiveSubWindowForItemSelector() const { return mActiveSubWindowForItemSelector; }
	void					SPI_SelectNextItemInSubWindow();
	void					SPI_SelectPreviousItemInSubWindow();
	ABool					SPI_IsSubWindow( const AWindowID inWindowID ) const { return (mSubWindowArray_WindowID.Find(inWindowID)!=kIndex_Invalid); }
	void					SPI_ClearAllLockOfSubWindows();
	ABool					SPI_GetProhibitFloatingSubWindow() const { return mShowFloatingSubWindow; }
	AWindowID				SPI_GetSubWindowParentTextWindow( const AWindowID inWindowID ) const
	{ AIndex index = mSubWindowArray_WindowID.Find(inWindowID); return mSubWindowArray_TextWindowID.Get(index); }
  private:
	ABool					JudgeIfTargetSubWindowByTextWindowID( const AWindowID inSubWindowID, const AWindowID inTextWindowID ) ;
	ABool					JudgeIfTargetSubWindowByTextDocumentID( const AWindowID inSubWindowID, const ADocumentID inTextDocumentID ) ;
	void					ReopenFloatingSubWindows();
	void					SaveFloatingSubWindows();
	AArray<AWindowID>					mSubWindowArray_WindowID;
	AArray<ASubWindowType>				mSubWindowArray_SubWindowType;
	AArray<AWindowID>					mSubWindowArray_TextWindowID;
	AArray<ASubWindowLocationType>		mSubWindowArray_SubWindowLocationType;
	AArray<AIndex>						mSubWindowArray_SubWindowLocationIndex;
	AWindowID							mActiveSubWindowForItemSelector;
	ABool								mShowFloatingSubWindow;
	
	//#723
	//AWindow_CallGraf
  public:
	AWindow_CallGraf&		SPI_GetCallGrafWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_CallGraf,kWindowType_CallGraf,inWindowID); }
	void					SPI_SetCallGrafCurrentFunction( const AWindowID inTextWindowID,
							const AFileAcc& inProjectFolder, 
							const AIndex inModeIndex, const AIndex inCategoryIndex,
							const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
							const AIndex inStartIndex, const AIndex inEndIndex, const ABool inByEdit );
	void					SPI_SetCallGrafEditFlag( const AWindowID inTextWindowID, 
													const AText& inFilePath, const AText& inClassName, const AText& inFunctionName );
	void					SPI_NotifyToCallGrafByIdInfoJump( const AText& inCallerFuncIdText, 
							const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
							const AIndex inStartIndex, const AIndex inEndIndex );
	void					SPI_NotifyToCallGrafByTextDocumentEdited( const AFileAcc& inFile, 
							const ATextIndex inTextIndex, const AItemCount inInsertedCount );
	void					SPI_ShowHideImportFileProgressInCallGrafWindow( const ABool inShow );
	void					SPI_AbortCurrentWordsListFinderRequestInCallGrafWindow();
	
	//#723
	//AWindow_WordsList
  public:
	AWindow_WordsList&		SPI_GetWordsListWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_WordsList,kWindowType_WordsList,inWindowID); }
	const AWindow_WordsList&	SPI_GetWordsListWindowConst( const AWindowID inWindowID ) const
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_WordsList,kWindowType_WordsList,inWindowID); }
	void					SPI_RequestWordsList( const AWindowID inTextWindowID, const AFileAcc& inProjectFolder, const AIndex inModeIndex,
							const AText& inWord );
	void					SPI_ShowHideImportFileProgressInWordsListWindow( const ABool inShow );
	void					SPI_NotifyToWordsListByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount );
	void					SPI_AbortCurrentWordsListFinderRequestInWordsListWindow();
	
	//#734
	//AWindow_Previewer
  public:
	AWindow_Previewer&		SPI_GetPreviewerWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Previewer,kWindowType_Previewer,inWindowID); }
	void					SPI_RequestLoadToPreviewerWindow( const AWindowID inTextWindowID, const AText& inURL );
	void					SPI_RequestReloadToPreviewerWindow( const AWindowID inTextWindowID );
	//drop void					SPI_ExecuteJavaScriptInPreviewerWindow( const AWindowID inTextWindowID, const AText& inText );
	ABool					SPI_PreviewWindowExist( const AWindowID inTextWindowID ) const;
  private:
	
	//#750
	//AWindow_TextMarker
  public:
	AWindow_TextMarker&		SPI_GetTextMarkerWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextMarker,kWindowType_TextMarker,inWindowID); }
	void					SPI_SetCurrentTextMarkerGroupIndex( const AIndex inIndex );
	AIndex					SPI_GetCurrentTextMarkerGroupIndex() { return mCurrentTextMarkerGroupIndex; }
	void					SPI_GetCurrentTextMarkerData( AText& outTitle, AText& outText ) ;
	void					SPI_SetCurrentTextMarkerData_Text( const AText& inText );
	void					SPI_SetCurrentTextMarkerData_Title( const AText& inText );
	void					SPI_ApplyTextMarkerForAllDocuments();
	void					SPI_CreateNewTextMarkerGroup();
	void					SPI_DeleteCurrentTextMarkerGroup();
	void					SPI_SelectOrCreateTextMarkerWindow();
  private:
	void					UpdateTextMarkerMenu();
	void					UpdateTextMarkerWindows();
	AIndex								mCurrentTextMarkerGroupIndex;
	
	//#142
	//AWindow_DocInfo
  public:
	AWindow_DocInfo&		SPI_GetDocInfoWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_DocInfo,kWindowType_DocInfo,inWindowID); }
	void					SPI_UpdateDocInfoWindows_TotalCount( const ADocumentID inDocumentID, 
							const AItemCount inTotalCharCount, const AItemCount inTotalWordCount );
	void					SPI_UpdateDocInfoWindows_SelectedCount( const AWindowID inTextWindowID, 
																   const AItemCount inSelectedCharCount, const AItemCount inSelectedWordCount, 
																   const AItemCount inSelectedParagraphCount,
																   const AText& inSelectedColorText );
	void					SPI_UpdateDocInfoWindows_CurrentChar( const AWindowID inTextWindowID, 
							const AUCS4Char inChar, const AUCS4Char inCanonicalDecomp, 
							const ATextArray& inHeaderTextArray, const AArray<AIndex>& inHeaderParagraphIndexArray,
							const AText& inDebugText );
	void					SPI_UpdateDocInfoWindows_PluginText( const AWindowID inTextWindowID, const AText& inPluginText );
	void					SPI_UpdateDocInfoWindowsForAllDocuments();//#1312
	ABool					SPI_DocInfoWindowExist( const AWindowID inTextWindowID );
  private:
	
	//#152
	//AWindow_ClipboardHistory
  public:
	AWindow_ClipboardHistory&		SPI_GetClipboardHistoryWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ClipboardHistory,kWindowType_ClipboardHistory,inWindowID); }
	const ATextArray&		SPI_GetClipboardHistory() const { return mClipboardHistory; }
	void					SPI_AddClipboardHistory( const AText& inText );
  private:
	ATextArray							mClipboardHistory;
	
	//#725
	//AWindow_FileList
  public:
	AWindow_FileList&		SPI_GetFileListWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_FileList,kWindowType_FileList,inWindowID); }
	const AWindow_FileList&	SPI_GetFileListWindowConst( const AWindowID inWindowID ) const
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_FileList,kWindowType_FileList,inWindowID); }
	void					SPI_UpdateFileListWindows( const AFileListUpdateType inType, 
							const ADocumentID inDocumentID = kObjectID_Invalid );
	void					SPI_UpdateFileListWindowsProperty();
	void					SPI_UpdateFileListRemoteFolder( const AText& inFolderURL );
	void					SPI_SearchInProjectFolder();
	void					SPI_SearchInSameFolder();
	void					SPI_SearchInRecentlyOpenFiles();
  private:
	AWindowID				FindFileListWindow( const AWindowID inTextWindowID, const ANumber inMode ) const;
	
	
	//#725
	//AWindow_IdInfo
  public:
	AWindow_IdInfo&			SPI_GetIdInfoWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_IdInfo,kWindowType_IdInfo,inWindowID); }
	const AWindow_IdInfo&	SPI_GetIdInfoWindowConst( const AWindowID inWindowID ) const
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_IdInfo,kWindowType_IdInfo,inWindowID); }
	ABool					SPI_UpdateIdInfoWindows( const AText& inKeyword, const ADocumentID inDocumentID, const AWindowID inParentWindowID );
	//AWindow_IdInfo&			SPI_GetPopupIdInfoWindow();
	ABool					SPI_HighlightIdInfoArgIndex( const AWindowID inTextWindowID ,const AText& inKeyword, const AIndex inArgIndex );
	void					SPI_SearchInKeywordList();//#798
	void					SPI_HideFloatingIdInfoWindow();//#1336
  private:
	AWindowID				FindIdInfoWindow( const AWindowID inTextWindowID ) const;//#798
	//AWindowID							mPopupIdInfoWindowID;
	//
  public:
	AThread_IdInfoFinder&	SPI_GetIdInfoFinderThread();
  private:
	void					DoIdInfoFinderPaused( const ADocumentID inTextDocumentID,
							const AWindowID inIdInfoWindowID, const AWindowID inTextWindowID );
	AObjectID							mIdInfoFinderThreadID;
	
	//#725
	//AWindow_JumpList
  public:
	AWindow_JumpList&		SPI_GetJumpListWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_JumpList,kWindowType_JumpList,inWindowID); }
	void					SPI_HideOtherFloatingJumpListWindows( const AWindowID inWindowID );
	void					SPI_CreateJumpListWindowsTab( const AWindowID inTextWindowID, const ADocumentID inDocumentID );
	void					SPI_CloseJumpListWindowsTab( const AWindowID inWindowID, const ADocumentID inDocumentID );
	void					SPI_SelectJumpListWindowsTab( const AWindowID inWindowID, const ADocumentID inDocumentID );
	void					SPI_SetJumpListWindowTitle( const AWindowID inTextWindowID, const AText& inTitle );
 private:
	
	//#725 #717
	//AWindow_CandidateList
  public:
	AWindow_CandidateList&	SPI_GetCandidateListWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_CandidateList,kWindowType_CandidateList,inWindowID); }
	//AWindow_CandidateList&	SPI_GetPopupCandidateWindow();
  private:
	//AWindowID							mPopupCandidateWindowID;
	//
  public:
	AThread_CandidateFinder&	SPI_GetCandidateFinderThread();
	void					SPI_UpdateCandidateListWindows( const AModeIndex inModeIndex,
							const AWindowID inTextWindowID, const AControlID inTextViewControlID,
							const ATextArray& inIdTextArray, const ATextArray& inInfoTextArray,
							const AArray<AIndex>& inCategoryIndexArray, const AArray<AScopeType>& inScopeTypeArray, 
							const ATextArray& inFilePathArray, const AArray<ACandidatePriority>& inPriorityArray,
							const ATextArray& inParentKeywordArray,
							const AArray<AItemCount>& inMatchedCountArray );
	void					SPI_SetCandidateListWindowsSelectedIndex( const AWindowID inTextWindowID, const AIndex inSelectedIndex );
	void					SPI_InvalidateCandidateListWindowClick( const AWindowID inTextWindowID );
	ABool					SPI_IsCandidatePartialSearchMode() const { return mIsCandidatePartialSearchMode; }
	void					SPI_SetCandidatePartialSearchMode( const ABool inPartialMode )
	{ mIsCandidatePartialSearchMode = inPartialMode; }
	
  private:
	ABool					IsTargetCandidateListWindow( const AWindowID inCandidateWindowID, const AWindowID inTextWindowID ) ;
	AObjectID							mCandidateFinderThreadID;
	ABool								mIsCandidatePartialSearchMode;
	
	//#725
	//AWindow_KeyToolList
  public:
	AWindow_KeyToolList&	SPI_GetKeyToolListWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_KeyToolList,kWindowType_KeyToolList,inWindowID); }
	void					SPI_SetKeyToolListInfo( const AWindowID inTextWindowID, const AControlID inTextViewControlID,
												   const AGlobalPoint inPoint, const ANumber inLineHeight, 
												   const ATextArray& inTextArray, const ABoolArray& inIsUserToolArray );
  private:
	
	//#725
	//AWindow_FindResult
  public:
	AWindow_FindResult&		SPI_GetFindResultWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_FindResult,kWindowType_FindResult,inWindowID); }
	ADocumentID				SPI_GetOrCreateFindResultWindowDocument();
	AWindowID				SPI_GetCurrentFindResultWindowID();
	AWindowID				SPI_GetFloatingFindResultWindowID();//#1336
  private:
	
	//#725
	//キー記録
  public:
	AWindow_KeyRecord&		SPI_GetKeyRecordWindow( const AWindowID inWindowID )
	{ MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_KeyRecord,kWindowType_KeyRecord,inWindowID); }
	AWindowID				SPI_FindKeyRecordWindow() const;
	void					SPI_ShowKeyRecordWindow();
	void					SPI_RecordKeyText( const AText& inText );
	void					SPI_RecordKeybindAction( const AKeyBindAction inAction, const AText& inText );
	void					SPI_RecordFindNext( const AFindParameter& inParam );
	void					SPI_RecordFindPrevious( const AFindParameter& inParam );
	void					SPI_RecordFindFromFirst( const AFindParameter& inParam );
	void					SPI_RecordReplace( const AFindParameter& inParam, const AReplaceRangeType inReplaceRange );
	void					SPI_StartRecord();
	void					SPI_PauseRecord();
	void					SPI_StopRecord();
	void					SPI_GetRecordedText( AText& outText ) const;
	void					SPI_SetKeyRecordedRawText( const AText& inText );
	ABool					SPI_IsKeyRecording() const { return mKeyRecording; }
	ABool					SPI_ExistRecordedText() const { return (mKeyRecordedText.GetItemCount()>0); }
  private:
	void					AddRecordedText( const AText& inText, const ABool inEscape );
	ABool								mKeyRecording;
	ABool								mStoppedFlag;
	AText								mKeyRecordedText;
	
	//#738
	//AWindow_KeyActionList
  public:
	AWindow_KeyActionList&	SPI_GetKeyActionListWindow()
	{
		//未生成なら生成
		if( mKeyActionListWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_KeyActionList>	window;
			mKeyActionListWindowID = NVI_CreateWindow(window);
			SPI_GetKeyActionListWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_KeyActionList,kWindowType_KeyActionList,mKeyActionListWindowID);
	}
  private:
	AWindowID							mKeyActionListWindowID;
	
	//#846
	//編集モーダルセッション
  public:
	AWindow_EditProgress&	SPI_GetEditProgressWindow()
	{
		//プログレスウインドウ未生成なら生成
		if( mEditProgressWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_EditProgress>	window;
			mEditProgressWindowID = NVI_CreateWindow(window);
			SPI_GetEditProgressWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_EditProgress,kWindowType_EditProgress,mEditProgressWindowID);
	}
	ABool					SPI_StartEditProgressModalSession( const AEditProgressType inType, const ABool inEnableCancelButton,
							const ABool inShowProgressWindowNow = false );
	void					SPI_EndEditProgressModalSession();
	ABool					SPI_CheckContinueingEditProgressModalSession( const AEditProgressType inType, const AItemCount inCount,
							const ABool inUpdateProgress, const AItemCount inCurrentProgress, const AItemCount inTotalProgress );
	ABool					SPI_InEditProgressModalSession() const;
  private:
	void					ShowProgressWindowAndStartModalSessionInActual();
	AEditProgressType					mEditProgressType;
	AWindowID							mEditProgressWindowID;
	ADateTime							mEditProgressStartDateTime;
	ABool								mEditProgressModalSessionStarted;
	ABool								mEditProgressModalSessionStartedInActual;
	ANumber								mEditProgressCurrentProgressPercent;
	AItemCount							mEditProgressCurrentCount;
	ADateTime							mEditProgressContinueCheckDateTime;
	ABool								mEditProgressEnableCancelButton;
	
	//JavaScript実行スレッド
  public:
	AThread_JavaScriptExecuter&	SPI_GetJavaScriptExecuterThread();
	void					SPI_ExecuteJavaScript( const AText& inScriptText, const AFileAcc& inFolder );
	void					SPI_ShowJavaScriptModalAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, ABool& outAborted );
	ABool					SPI_ShowJavaScriptModalCancelAlertWindow( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, const AText& inCancelButtonText, ABool& outAborted );
  private:
	AObjectID							mJavaScriptExecuterThreadID;
	ABool								mExecutingJavaScript;
	//#1217
  private:
	AWindowID							mJavaScriptModalAlertWindowID;
	AWindowID							mJavaScriptModalCancelAlertWindowID;
	
	//JavaScriptプラグイン
  public:
	void					SPI_LoadPlugin( const AIndex inModeIndex, const AText& inPluginScript, const AFileAcc& inPluginFolder );
	//#994 ABool					SPI_AddJavaScriptPlugin( const AText& inEventName, const AText& inScript, const AText& inGuid );
	//#994 void					SPI_DoJavaScriptPlugin_OnSelectionChanged();
	void					SPI_GetCurrentModeNameForPluginLoad( AText& outModeName ) 
	{
		outModeName.DeleteAll();
		if( mCurrentModeIndexForPluginLoad != kIndex_Invalid )
		{
			SPI_GetModePrefDB(mCurrentModeIndexForPluginLoad).GetModeRawName(outModeName);
		}
	}
  private:
	AHashTextArray						mJavaScriptPluginArray_OnSelectionChanged_Guid;
	ATextArray							mJavaScriptPluginArray_OnSelectionChanged_Script;
	AIndex								mCurrentModeIndexForPluginLoad;
	
	//<所有クラス(AAppPrefDB)生成／削除／取得>
  public:
	AAppPrefDB&				GetAppPref() {return mAppPref;}
	const AAppPrefDB&		GetAppPrefConst() const {return mAppPref;}
  private:
	AAppPrefDB							mAppPref;
	ADataBase&				NVIDO_GetAppPrefDB() {return mAppPref;}
	const ADataBase&		NVIDO_GetAppPrefDBConst() const {return mAppPref;}
	
	//<所有クラス(AModePrefDB)生成／削除／取得>
  public:
	AModePrefDB&			SPI_GetModePrefDB( const AModeIndex inModeIndex, const ABool inLoadIfNotLoaded = true );//#253
	AModeIndex				SPI_FindModeIndexByModeRawName( const AText& inModeName ) const;
	AModeIndex				SPI_FindModeIndexByModeDisplayName( const AText& inModeName ) const;
	ABool					SPI_CheckModeRawNameUnique( const AText& inName ) const;//#427
	void					SPI_ModeNameUpdated( const AModeIndex inModeIndex );//#427
	AModeIndex				SPI_FindModeIDByTextFilePath( const AText& inFilename ) const;//R0210
	AModeIndex				SPI_AddNewMode( const AText& inModeName );
	AModeIndex				SPI_AddNewModeImportFromFolder( const AText& inModeName, const AFileAcc& inImportFolder, 
							const ABool inSecurityScreened, const ABool inConvertToAutoUpdate );//R0000 security #427
	ABool					SPI_ScreenModeExecutable( const AFileAcc& inItem, AText& outText, AText& outReport );
	AItemCount				SPI_GetModeCount() const { return mModePrefDBArray.GetItemCount(); }//#253
  private:
	AModeIndex				AddMode( const AFileAcc inModeFolder, const ABool inConvertToAutoUpdate,
							const ABool inRemoveMultiLanguageModeName );//#427
	ABool					ScreenModeExecutableFolder( const AFileAcc& inFolder, AText& outText, AText& outReport );//R0000
	ABool					ScreenModeExecutableItem( const AFileAcc& inItem, AText& outText, AText& outReport );//R0000
	//各モードデータ
	AObjectArray<AModePrefDB>			mModePrefDBArray;
	//各モードファイル
	//#920 AObjectArray<AFileAcc>				mModeFileArray;
	//各モード名
	AHashTextArray						mModeRawNameArray;//#914
	AHashTextArray						mModeDisplayNameArray;//#914
	mutable AThreadMutex				mModeNameArrayMutex;//#914
	
	//モード拡張子検索ハッシュ
  public:
	void					SPI_UpdateModeSuffixArray( const AModeIndex inModeIndex );
	void					SPI_FindModeIndexFromSuffix( const AText& inFilePath, AArray<AIndex>& outModeIndexArray ) const;
  private:
	AHashTextArray						mModeSuffixArray_Suffix;
	AArray<AModeIndex>					mModeSuffixArray_ModeIndex;
	mutable AThreadMutex				mModeSuffixArrayMutex;
	
	//ModeExecutableAlertWindow #199
  public:
	void					SPI_ShowModeExecutableAlertWindow( const AText& inModeName, 
															  const AFileAcc& inImportFile, const AText& inFileList, const AText& inReport, const ABool inConvertToAutoUpdate );
	void					SPI_ShowModeExecutableAlertWindow_Tool( const AModeIndex inModeIndex, const AObjectID inToolMenuObjectID, const AIndex inRowIndex, 
																   const AFileAcc& inImportFile, const AFileAcc& inDstFile, const AText& inFileList, const AText& inReport );
	AWindow_ModeExecutableAlert&		SPI_GetModeExecutableAlertWindow()
	{
		//未生成なら生成
		if( mModeExecutableAlertWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_ModeExecutableAlert>	modeExecutableAlertWindowFactory;
			mModeExecutableAlertWindowID = NVI_CreateWindow(modeExecutableAlertWindowFactory);
		}
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ModeExecutableAlert,kWindowType_ModeExecutableAlert,mModeExecutableAlertWindowID);
	}
  private:
	AWindowID							mModeExecutableAlertWindowID;
	
	//<所有クラス(AMultiFileFinder)生成／削除／取得>
  public:
	ABool					SPI_IsMultiFileFindWorking();
	void					SPI_StartMultiFileFind( const AFindParameter& inFindParam, const ADocumentID inIndexDocumentID,
												   const ABool inForMultiFileReplace, const ABool inExtractMatchedText );
	void					SPI_AbortMultiFileFind();
	void					SPI_SleepForAWhileMultiFileFind();
	AMultiFileFinder&		SPI_GetMultiFileFinder() { return GetMultiFileFinder(); }
  private:
	AMultiFileFinder&		GetMultiFileFinder();
	AObjectID							mMultiFileFinderObjectID;
	
	//<所有クラス(AFTPConnection)生成／削除／取得>
  public:
	AFTPConnection&			SPI_GetFTPConnection();
  private:
	AObjectID							mFTPConnectionObjectID;
	
	//<所有クラス(AImportFileRecognizer)生成／削除／取得>
  public:
	AImportFileRecognizer&	SPI_GetImportFileRecognizer();
  private:
	AObjectID							mImportFileRecognizerObjectID;
	
#if IMPLEMENTATION_FOR_MACOSX
	//<所有クラス(AAppleScriptExecuter)生成／削除／取得>
  public:
	void					SPI_ExecuteAppleScript( const AFileAcc& inFile );
	void					SPI_GetAppleScriptSource( const AFileAcc& inFile, AText& outText );
  private:
	AObjectID							mAppleScriptExecuterObjectID;
#endif
    
    /* AApplicationへ移動 
	//<所有クラス(CAppImp)生成／削除／取得>
  private:
	CAppImp&				GetImp() {return mAppImp;}
    CAppImp								mAppImp;
    */
	
	/* AApplicationへ移動 win 080618
	//<所有クラス(AUnicodeData)生成／削除／取得>
  public:
	const AUnicodeData&			SPI_GetUnicodeData() const {return mUnicodeData;}
  private:
	AUnicodeData						mUnicodeData;
	*/
	
	//<所有クラス(AProjectPrefDB)>RC3
  public:
	void					SPI_FindProjectFolder( const AFileAcc& inFile, AFileAcc& outProjectFolder, AObjectID& outProjectObjectID, AText& outRelativePath );//RC3
	/*#858
	ABool					SPI_GetExternalCommentFromProjectDB( const AObjectID inProjectDBObjectID, const AText& inRelativePath, const AText& inModuleName,
							const AIndex inOffset, AText& outComment ) const;
	void					SPI_SetExternalCommentToProjectDB( const AObjectID inProjectDBObjectID, const AText& inRelativePath, const AText& inModuleName,
							const AIndex inOffset, const AText& inComment );
	void					SPI_UpdateExternalCommentPosition( const AObjectID inProjectDBObjectID, const AText& inRelativePath, 
							const AText& inModuleName, const AIndex inOffset, const AText& inNewModuleName, const AIndex inNewOffset );//#138
	void					SPI_GetExternalCommentArrayFromProjectDB( const AObjectID inProjectDBObjectID, const AText& inRelativePath, ATextArray& outModuleNameArray, ANumberArray& outOffsetArray, ATextArray& outCommentArray ) const;//#138
	void					SPI_RevealExternalComment( const AObjectID inProjectDBObjectID, const AText& inRelativePath, const AText& inModuleName, const ANumber inOffset );
	*/
	ADocumentID				SPI_GetDocumentIDByProjectObjectIDAndPath( const AObjectID inProjectDBObjectID, const AText& inRelativePath,
							const ABool inCreateDocument );
  private:
	ABool					FindProjectFolderBySuffixRecursive( const AFileAcc& inFolder, const AText& inProjectSuffix, AFileAcc& outProjectFolder ) const;
	ABool					FindProjectFolderByFilenameRecursive( const AFileAcc& inFolder, const AText& inFilename, AFileAcc& outProjectFolder ) const;
	AObjectArrayIDIndexed<AProjectPrefDB>		mProjectPrefDBArray;//#693
	//#858 void					NotifyExternalCommentUpdatedToDocument( const AObjectID inProjectDBObjectID, const AText& inRelativePath );
	
	/*#858
	//付箋紙表示ウインドウ
  public:
	AWindow_Fusen&			SPI_GetFusenWindow( const AWindowID inWindowID )
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Fusen,kWindowType_Fusen,inWindowID);
	const AWindow_Fusen&	SPI_GetFusenWindowConst( const AWindowID inWindowID ) const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_Fusen,kWindowType_Fusen,inWindowID);
	*/
	
	//#146 Navigateボタン
  public:
	void					SPI_RegisterNavigationPosition();
	void					SPI_NavigateNext();
	void					SPI_NavigatePrev();
	ABool					SPI_GetCanNavigatePrev() const;
	ABool					SPI_GetCanNavigateNext() const;
  private:
	AObjectArray<AFileAcc>				mNavigate_File;
	AArray<ATextIndex>					mNavigate_StartIndex;
	AArray<AItemCount>					mNavigate_Length;
	AIndex								mNavigateIndex;
	ADocumentID							mNavigate_Last_DocumentID;
	ATextIndex							mNavigate_Last_StartIndex;
	AItemCount							mNavigate_Last_Length;
	
	//
  public:
	const AFileAcc&				SPI_GetLastOpenedTextFile() const { return mLastOpenedTextFile; }
	void						SPI_SetLastOpenedTextFile( const AFileAcc& inLastOpenedTextFile ) { mLastOpenedTextFile = inLastOpenedTextFile; }//#976
  private:
	AFileAcc							mLastOpenedTextFile;
	
	//プロジェクトフォルダ設定
  public:
	void						SPI_ShowSetProjectFolderWindow( const AFileAcc& inDefaultFolder );//RC3
	AWindow_SetProjectFolder&	SPI_GetSetProjectFolderWindow() 
	{
		//#693
		if( mSetProjectFolderWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_SetProjectFolder>	setProjectWindowFactory;
			mSetProjectFolderWindowID = NVI_CreateWindow(setProjectWindowFactory);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_SetProjectFolder,kWindowType_SetProjectFolder,mSetProjectFolderWindowID);
	}
  private:
	AWindowID							mSetProjectFolderWindowID;//#199
	
	/*#688
	//#436
  private:
	AWindowID							mDevToolsWindowID;//#436
	*/
	
	/*#858
	//プロジェクトフォルダ設定アラート
  public:
	void						SPI_ShowProjectFolderAlertWindow( const AFileAcc& inDefaultFolder );//RC3
	AWindow_ProjectFolderAlert&	SPI_GetProjectFolderAlertWindow()
	{
		//#693
		if( mProjectFolderAlertWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_ProjectFolderAlert>	projectFolderAlertWindowFactory;
			mProjectFolderAlertWindowID = NVI_CreateWindow(projectFolderAlertWindowFactory);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ProjectFolderAlert,kWindowType_ProjectFolderAlert,mProjectFolderAlertWindowID);
	}
  private:
	AWindowID							mProjectFolderAlertWindowID;//#199
	*/
	//
  public:
	AWindow_CompareFolder&		SPI_GetCompareFolderWindow() //#199
	{
		//#693
		if( mCompareFolderWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_CompareFolder>	compareFolderWindow;
			mCompareFolderWindowID = NVI_CreateWindow(compareFolderWindow);
			SPI_GetCompareFolderWindow().NVI_Create(kObjectID_Invalid);
		}
		//
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_CompareFolder,kWindowType_CompareFolder,mCompareFolderWindowID);
	}
  private:
	AWindowID							mCompareFolderWindowID;
	
	/*
	//#725
	//サブウインドウリストウインドウ
  public:
	AWindow_SubWindowsList&		SPI_GetSubWindowsListWindow()
	{
		//未生成なら生成
		if( mSubWindowsListWindowID == kObjectID_Invalid )
		{
			AWindowDefaultFactory<AWindow_SubWindowsList>	subWindowsListWindowFactory;
			mSubWindowsListWindowID = NVI_CreateWindow(subWindowsListWindowFactory);
			SPI_GetSubWindowsListWindow().NVI_Create(kObjectID_Invalid);
		}
		//取得
		MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_SubWindowsList,kWindowType_SubWindowList,mSubWindowsListWindowID);
	}
  private:
	AWindowID							mSubWindowsListWindowID;
	*/
	
	//
  public:
	/*#725
	void					SPI_UpdateFileListWindow_DocumentActivated( const ADocumentID inTextDocumentID );
	void					SPI_UpdateFileListWindow_DocumentOpened( const ADocumentID inTextDocumentID );
	*/
	void					SPI_DoTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount );
	
	//
  public:
	void					SPI_Register_ToolCommandWaitText( const AText& inText, const ADocumentID inDocumentID );
	void					SPI_Register_ToolCommandWaitOpen( const AFileAcc& inFile, const AText& inText );
	//#725 void					SPI_GoNextIndexWindowItem();//#232
	//#725 void					SPI_GoPrevIndexWindowItem();//#232
	void					SPI_SwitchFullScreenMode();//#476
	void					SPI_SwitchHandToolMode();//#606
	ABool					SPI_GetHandToolMode() const { return mHandToolMode; }//#606
	void					SPI_ShowAppPrefWindow();//#724
  private:
	AText								mToolCommandWaitText;
	ADocumentID							mToolCommandWaitTextDestinationDocumentID;
	ABool								mToolCommandWaitOpenFileFlag;
	AFileAcc							mToolCommandWaitOpenFile;
	ABool								mHandToolMode;//#606
	
	//ツールコマンド管理
  public:
	AToolCommandID			SPI_FindToolCommandID( const AText& inText ) const;
	AKeyBindAction			SPI_GetKeyBindActionFromToolCommandText( const AText& inText ) const;
	void					SPI_GetToolCommandTextFromKeyBindAction( const AKeyBindAction inKeyAction, AText& outText ) const;
  private:
	void					RegisterToolCommand( AConstCharPtr inToolCommandText, const AToolCommandID inToolCommandID, 
							const AKeyBindAction inKeyAction = keyAction_NOP );
	ATextArray							mToolCommand_Text;
	AArray<AToolCommandID>				mToolCommand_ID;
	AArray<AKeyBindAction>				mToolCommand_KeyBindAction;
	
	//子プロセス起動・管理
  public:
	AChildProcessManager&	SPI_GetChildProcessManager() { return mChildProcessManager; }
  private:
	AChildProcessManager				mChildProcessManager;
	
	//R0199
  public:
	AObjectID				SPI_GetCorrectSpellMenuObjectID() const { return mCorrectSpellMenu; }
  private:
	AObjectID							mCorrectSpellMenu;
	
	//B0372
  public:
	void					SPI_CreateNewWindowAutomatic();
	void					CloseAutomaticallyCreatedDocument( const ADocumentID inDocID );//#1056
  private:
	void					DoPrependedOpenUntitledFileEvent();//#1056
	
	//R0137
  public:
	void					SPI_SetNoCheckModeExecutable( const ABool inNoCheck ) { mNoCheckModeExecutable = inNoCheck; }
	ABool					SPI_IsNoCheckModeExecutable() const { return mNoCheckModeExecutable; }
  private:
	ABool								mNoCheckModeExecutable;
	
	//#219
	/*#725
  public:
	AFloatNumber			SPI_GetOverlayWindowsAlpha() const;
	void					SPI_UpdateOverlayWindowsAlpha();
	*/
	//#291
  public:
	ABool					SPI_IsSupportPaneMode() const;
	
	//#427
  public:
	void					SPI_GetTempFolder( AFileAcc& outTempFolder );
	void					SPI_GetUniqTempFolder( AFileAcc& outUniqTempFolder );//#862
	
	//#801
	//dataフォルダ取得
  public:
	void					SPI_GetLocalizedDataFolder( AFileAcc& outFolder ) const;
	
	//#65
	//マルチファイル置換バックアップフォルダ
  public:
	void					SPI_GetDefaultMultiFileReplaceBackupFolder( AFileAcc& outFolder );
	
	//#81
	//自動バックアップフォルダ
  public:
	void					SPI_GetDefaultAutoBackupRootFolder( AFileAcc& outFolder );
	void					SPI_GetAutoBackupRootFolder( AFileAcc& outFolder );
	
	//#81
	//未保存データフォルダ
  public:
	void					SPI_GetUnsavedDataFolder( AFileAcc& outFolder );
	
	//#898
	//doc prefルートフォルダ
  public:
	void					SPI_GetDocPrefRootFolder( AFileAcc& outFolder );
	
	//#889
	//カラースキームフォルダ
  public:
	void					SPI_GetAppColorSchemeFolder( AFileAcc& outFolder );
	void					SPI_GetUserColorSchemeFolder( AFileAcc& outFolder ) const;
	
	//#193
	//アクセスプラグインフォルダ
	void					SPI_GetAppAccessPluginFolder( AFileAcc& outFolder );
	void					SPI_GetUserAccessPluginFolder( AFileAcc& outFolder ) const;
	
	//#844
  public:
	void					SPI_GetAppTextEncodingFile( AFileAcc& outFile ) const;
	void					SPI_GetUserTextEncodingFile( AFileAcc& outFile ) const;
	
	//#638
	//自動インデント認識
  public:
	ABool					SPI_GetAutoIndentMode() const { return mAutoIndentMode; }
	void					SPI_SetAutoIndentMode( const ABool inAutoIndent ) { mAutoIndentMode = inAutoIndent; }
  private:
	ABool								mAutoIndentMode;
	
	//#878
	//FIFOクリップモード
  public:
	ABool					SPI_IsFIFOClipboardMode() const { return mFIFOClipboardMode; }
	void					SPI_SetFIFIOClipboardMode( const ABool inFIFOClipboardMode ) { mFIFOClipboardMode = inFIFOClipboardMode; }
	void					SPI_EnqueueFIFOClipboard( const AText& inText ) { mFIFOClipboardArray.Add(inText); }
	void					SPI_DequeueFIFOClipboard( AText& outText ) 
	{
		mFIFOClipboardArray.Get(0,outText);
		mFIFOClipboardArray.Delete1(0);
	}
	void					SPI_GetFirstTextInFIFOClipboard( AText& outText ) const
	{
		mFIFOClipboardArray.Get(0,outText);
	}
	AItemCount				SPI_GetFIFOClipboardCount() const { return mFIFOClipboardArray.GetItemCount(); }
  private:
	ABool								mFIFOClipboardMode;
	ATextArray							mFIFOClipboardArray;
	
	//#717
	//補完履歴
  public:
	void					SPI_AddToRecentCompletionWord( const AText& inWord );
	void					SPI_GetRecentCompletionWordArray( AHashTextArray& outWordArray ) const;
  private:
	AHashTextArray						mRecentCompletionWordArray;
	mutable AThreadMutex				mRecentCompletionWordArrayMutex;
	
	//<内部モジュール>
  private:
	
	
	//未分類
  public:
	void	SPI_TexPreview( const AFileAcc& inDocFile );
	//#193 void	SPI_ShowFTPFolderWindow();
	//win void	SPI_PostCommand( const AMenuItemID inCommandID );
	//#725 void	SPI_UpdateFileListProperty();
	//#844 void	SPI_UpdateWindowAlphaAll();//B0333
	void	SPI_UpdateFilePathDisplayAll();//B0389
	//#725 void	SPI_UpdateFloatingWindowHideAll();//B0442
	
	ABool	GetTopDocumentFile( AFileAcc& outFile );
	ABool	GetTopDocumentFolder( AFileAcc& outFolder );
	
	
	/*#1034
#if IMPLEMENTATION_FOR_MACOSX
	//AppleScript
  public:
	CASApp&					SPI_GetASImp() { return mASApp; }
  private:
	PowerPlant::LModelDirector			mModelDirector;
	CASApp								mASApp;
#endif
	*/
	
	//ATest	mTest;
	//Lua #567 #1170
  public:
	ADocumentID				SPI_CreateOrGetLuaConsoleDocumentID();
	ADocumentID				SPI_GetLuaConsoleDocumentID();
	static AUniqID			Lua_GetActiveTextDocumentUniqID();
	static AWindowID		Lua_GetActiveTextWindowID();
	static AUniqID			Lua_GetActiveTextViewUniqID();
	//ドキュメント操作
  public:
	static int				Lua_InsertText( lua_State* L );
	static int				Lua_DeleteText( lua_State* L );
	static int				Lua_InputText( lua_State* L );
	static int 				Lua_GetSelectedText( lua_State* L );
	static int				Lua_GetNextCharTextIndex( lua_State* L );
	static int				Lua_GetPrevCharTextIndex( lua_State* L );
	static int				Lua_GetParagraphIndexFromTextIndex( lua_State* L );
	static int				Lua_GetParagraphStartTextIndex( lua_State* L );
	static int				Lua_GetParagraphCount( lua_State* L );
	static int				Lua_GetText( lua_State* L );
	//View操作
  public:
	static int				Lua_GetSelectedRange( lua_State* L );
	static int				Lua_SetSelectedRange( lua_State* L );
	//ObjectID取得
  public:
	static int				Lua_GetActiveDocumentID( lua_State* L );
	static int				Lua_GetActiveViewID( lua_State* L );
	static int				Lua_GetDocumentIDFromViewID( lua_State* L );
	//ドキュメントオブジェクト
  public:
	static int				Lua_CreateNewDocument( lua_State* L );
	static int				Lua_RevealDocument( lua_State* L );
	//その他
  public:
	static int				Lua_Output( lua_State* L );
	static int				Lua_GetMiLuaScriptingVersion( lua_State* L );
	static int				Lua_AutoTest( lua_State* L );
	
	//コンテクストメニュー
	
	//致命的エラー発生時のデータバックアップ
	//ABaseFunction::CriticalError等からコールされる
  private:
	void	NVIDO_BackupDataForCriticalError();//#182
	
	//ユニットテスト
  public:
	void	UnitTest();
	void	UnitTest2();
	
	//
  public:
	ABool	SPI_GetSDFDebugMode() const { return mSDFDebugMode; }
	void	SPI_SetSDFDebugMode( const ABool inSDFDebugMode ) { mSDFDebugMode = inSDFDebugMode; }//#349
  private:
	ABool	mSDFDebugMode;
	//
	public:
	ABool	SPI_GetMiscDebugMode() const { return mMiscDebugMode; }
	private:
	ABool	mMiscDebugMode;
};

//extern AApp&	GetApp();
inline AApp&	GetApp()
{
	return dynamic_cast<AApp&>(AApplication::GetApplication());
}


//R0198
#pragma mark ===========================
#pragma mark [クラス]AFileOpenCustomData
//
class AFileOpenCustomData
{
  public:
	AFileOpenCustomData() {}
	~AFileOpenCustomData() {}
	
	AText	textEncodingName;
};


/**
モーダルセッション開始・終了用スタッククラス
*/
class AStEditProgressModalSession
{
  public:
	AStEditProgressModalSession( const AEditProgressType inType, const ABool inEnableCancelButton,
								const ABool inShowProgressWindowNow,
								const ABool inEnableModalSession);
	~AStEditProgressModalSession();
  private:
	ABool	mModalSessionStarted;
};



