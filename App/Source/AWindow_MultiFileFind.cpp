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

AWindow_MultiFileFind

*/

#include "stdafx.h"

#include "AWindow_MultiFileFind.h"
#include "AApp.h"
//#include "AUtil.h"
#include "ATextFinder.h"
#include "ADocument_IndexWindow.h"
#include "AView_Index.h"

#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kTextSize = 10.0;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AFloatNumber	kTextSize = 9.0;
#endif

//コントロールID
const static AControlID		kPopup_RecentlyUsedFindText = 101;
const static AControlID		kText_FindText = 102;
const static AControlID		kPopup_RecentlyUsedFileFilter = 103;
const static AControlID		kText_FileFilter = 104;
const static AControlID		kButton_Find = 105;
//const static AControlID		kStaticText_ActiveFolder = 107;
const static AControlID		kPopup_RecentlyUsedPath = 109;
const static AControlID		kButton_SelectFolder = 110;
const static AControlID		kCheckBox_IgnoreCase = 112;
const static AControlID		kCheckBox_WholeWord = 113;
const static AControlID		kCheckBox_Aimai = 114;
const static AControlID		kCheckBox_BackslashYen = 115;
const static AControlID		kCheckBox_RegExp = 116;
//#688 const static AControlID		kCheckBox_AllowReturnTab = 117;
const static AControlID		kCheckBox_Recursive = 119;
const static AControlID		kCheckBox_DisplayPosition = 120;
const static AControlID		kCheckBox_OnlyVisibleFile = 121;
const static AControlID		kCheckBox_OnlyTextFile = 122;
const static AControlID		kCheckBox_IgnoreSpaces			= 123;//#165
const static AControlID		kButton_SetupFuzzySearch		= 124;//#166
const static AControlID		kButton_ExtractToNewDocument	= 125;
//const static AControlID		kPopup_ChooseKind = 200;
//const static AControlID		kCheckBox_UseFileFilter = 201;
//const static AControlID		kPopup_FilterKind = 202;
//
const static AControlID		kRadio_Folder_SameFolder = 301;
const static AControlID		kRadio_Folder_ParentFolder = 302;
const static AControlID		kRadio_Folder_SameProject = 303;
const static AControlID		kRadio_Folder_Select = 304;
const static AControlID		kText_Folder_Path = 305;
//
const static AControlID		kRadio_Target_AllFiles = 311;
const static AControlID		kRadio_Target_WildCard = 312;
const static AControlID		kRadio_Target_RegExp = 313;
const static AControlID		kPopup_FileFilterPreset = 314;//#617
//
const static AControlID		kButton_OpenCloseReplaceScreen = 401;
//
const AControlID		kTriangle_FindOptions			= 501;
const AControlID		kTriangleText_FindOptions		= 502;
const AControlID		kStaticText_BottomMarker		= 503;
const AControlID		kTriangle_AdvancedSearch		= 504;
const AControlID		kTriangleText_AdvancedSearch	= 505;

//
const static AControlID		kControlID_IndexView = 1002;
const static AControlID		kControlID_ReplaceTargetListView = 1003;
const static AControlID		kControlID_ReplaceExclusionListView = 1004;
const static AControlID		kButton_ExcludeFile = 1005;
const static AControlID		kButton_IncludeFile = 1006;
const static AControlID		kCheck_BackupFolder = 1007;
const static AControlID		kText_BackupFolder = 1008;
const static AControlID		kButton_CancelReplace = 1009;
const static AControlID		kButton_ExecuteReplace = 1010;
const static AControlID		kText_ReplaceText = 1011;
const static AControlID		kButton_ReplaceBasePath = 1013;
const static AControlID		kButton_ExecuteReplaceOK = 1999;
const static AControlID		kProgressText = 1014;
const static AControlID		kButton_FindAbort = 1015;
const static AControlID		kButton_SelectBackupFolder = 1017;
const static AControlID		kButton_DefaultBackupFolder = 1018;

//
const static AControlID		kVScroll_ReplaceTargetListView = 2001;
const static AControlID		kVScroll_ReplaceExclusionListView = 2001;

#pragma mark ===========================
#pragma mark [クラス]AWindow_MultiFileFind
#pragma mark ===========================
//マルチファイル検索ウインドウ

#pragma mark --- コンストラクタ／デストラクタ
//コンストラクタ
AWindow_MultiFileFind::AWindow_MultiFileFind():AWindow()
,mFindResultDocumentID(kObjectID_Invalid)
, mReplaceWindowMode(false), mExecutingFindForReplace(false), mFindForReplaceFinished(false), mFindForReplaceAborted(false)
,mOpenClosingReplaceScreen(false), mExecutingReplace(false)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kMultiFileFindWindowPosition);
	
	AText	path;
	path.SetCstring("/");
	mActiveFolder.Specify(path);
	//#175
	NVI_AddToRotateArray();
	//
	NVI_AddToTimerActionWindowArray();
}

//デストラクタ
AWindow_MultiFileFind::~AWindow_MultiFileFind()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
メニュー選択時処理
*/
ABool	AWindow_MultiFileFind::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			//AWindowでクローズ可能フラグチェック後、NVIDO_Close()がコールされる
			NVI_TryClose();
			result = true;
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
ウインドウの閉じるボタン
*/
void	AWindow_MultiFileFind::EVTDO_DoCloseButton()
{
	//AWindowでクローズ可能フラグチェック後、NVIDO_Close()がコールされる
	NVI_TryClose();//#92
}

//
ABool	AWindow_MultiFileFind::EVTDO_ValueChanged( const AControlID inControlID )
{
	ABool	result = true;
	switch(inControlID)
	{
		/*
	  case kPopup_ChooseKind:
		{
			NVI_SetControlBool(kRadio_ActiveFolder,true);
			NVM_UpdateControlStatus();
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

//コントロールクリック時処理
ABool	AWindow_MultiFileFind::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	
	switch(inID)
	{
		//検索実行
	  case kButton_Find:
		{
			Execute(false);
			result = true;
			break;
		}
		//検索実行
	  case kButton_ExtractToNewDocument:
		{
			Execute(true);
			result = true;
			break;
		}
		//最近使った検索テキスト
	  case kPopup_RecentlyUsedFindText:
		{
			AIndex	index = NVI_GetControlNumber(kPopup_RecentlyUsedFindText);
			if( index-1 >= GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFindString) )
			{
				//メニューを消去
				GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFindString);
				UpdateRecentlyUsedFindTextMenu();
			}
			else if( index > 0 )//B0000 「最近検索した文字列」は検索文字列に設定しない
			{
				//検索文字列にコピー
				AText	text;
				NVI_GetControlText(kPopup_RecentlyUsedFindText,text);
				NVI_SetControlText(kText_FindText,text);
			}
			NVI_SetControlNumber(kPopup_RecentlyUsedFindText,0);
			break;
		}
		//最近使ったファイルフィルタ
	  case kPopup_RecentlyUsedFileFilter:
		{
			AIndex	index = NVI_GetControlNumber(kPopup_RecentlyUsedFileFilter);
			if( index-1 >= GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter) )
			{
				//メニューを消去
				GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter);
				UpdateRecentlyUsedFileFilterMenu();
			}
			else if( index > 0 )//B0000 「最近検索した文字列」は検索文字列に設定しない
			{
				//ファイルフィルタ欄にコピー
				AText	text;
				NVI_GetControlText(kPopup_RecentlyUsedFileFilter,text);
				NVI_SetControlText(kText_FileFilter,text);
			}
			NVI_SetControlNumber(kPopup_RecentlyUsedFileFilter,0);
			break;
		}
		//最近使ったファイルパス
	  case kPopup_RecentlyUsedPath:
		{
			AIndex	index = NVI_GetControlNumber(kPopup_RecentlyUsedPath);
			if( index-1 >= GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder) )
			{
				//メニューを消去
				GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder);
				UpdateRecentlyUsedPathMenu();
			}
			else if( index > 0 )//B0000 「最近検索した文字列」は検索文字列に設定しない
			{
				//ファイルパス欄にコピー
				AText	text;
				NVI_GetControlText(kPopup_RecentlyUsedPath,text);
				NVI_SetControlText(kText_Folder_Path,text);
				//
				NVI_SetControlBool(kRadio_Folder_Select,true);
			}
			NVI_SetControlNumber(kPopup_RecentlyUsedPath,0);
			break;
		}
		//フォルダ選択
	  case kButton_SelectFolder:
		{
			//#551 現在テキストボックスに指定されているフォルダをデフォルトフォルダにする
			AText	path;
			NVI_GetControlText(kText_Folder_Path,path);
			AFileAcc	defaultfolder;
			defaultfolder.Specify(path);
			//フォルダ選択ウインドウを表示
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_MultiFileFindSelectFolder");
			NVI_ShowChooseFolderWindow(defaultfolder,message,kButton_SelectFolder,true);//#551
			result = true;
			break;
		}
		//バックアップフォルダ選択 #65
	  case kButton_SelectBackupFolder:
		{
			AText	path;
			NVI_GetControlText(kText_BackupFolder,path);
			AFileAcc	defaultfolder;
			defaultfolder.Specify(path);
			//フォルダ選択ウインドウ表示
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_MultiFileFindSelectBackupFolder");
			NVI_ShowChooseFolderWindow(defaultfolder,message,kButton_SelectBackupFolder,true);
			result = true;
			break;
		}
		//デフォルトのバックアップフォルダを設定
	  case kButton_DefaultBackupFolder:
		{
			AFileAcc	backupFolder;
			GetApp().SPI_GetDefaultMultiFileReplaceBackupFolder(backupFolder);
			AText	path;
			backupFolder.GetPath(path);
			NVI_SetControlText(kText_BackupFolder,path);
			break;
		}
		//#166
	  case kButton_SetupFuzzySearch:
		{
			GetApp().SPI_GetAppPrefWindow().NVI_CreateAndShow();
			GetApp().SPI_GetAppPrefWindow().NVI_SelectTabControl(3);
			GetApp().SPI_GetAppPrefWindow().NVI_RefreshWindow();
			GetApp().SPI_GetAppPrefWindow().NVI_SwitchFocusToFirst();
			break;
		}
		//#617 プリセットからファイルフィルタ設定
	  case kPopup_FileFilterPreset:
		{
			AText	text;
			GetApp().SPI_GetFileFilterPreset(NVI_GetControlNumber(kPopup_FileFilterPreset),text);
			NVI_SetControlText(kText_FileFilter,text);
			NVI_SetControlBool(kRadio_Target_WildCard,true);
			NVI_SetControlNumber(kPopup_FileFilterPreset,0);
			break;
		}
		//検索オプション折りたたみ
	  case kTriangle_FindOptions:
		{
			if( mReplaceWindowMode == false )
			{
				NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			}
			break;
		}
	  case kTriangleText_FindOptions:
		{
			if( mReplaceWindowMode == false )
			{
				NVI_SetControlBool(kTriangle_FindOptions,!(NVI_GetControlBool(kTriangle_FindOptions)));
				NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			}
			break;
		}
		//
	  case kTriangle_AdvancedSearch:
		{
			if( mReplaceWindowMode == false )
			{
				NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			}
			break;
		}
	  case kTriangleText_AdvancedSearch:
		{
			if( mReplaceWindowMode == false )
			{
				NVI_SetControlBool(kTriangle_AdvancedSearch,!(NVI_GetControlBool(kTriangle_AdvancedSearch)));
				NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			}
			break;
		}
		//マルチファイル置換画面open/close
	  case kButton_OpenCloseReplaceScreen:
		{
			SPI_OpenCloseReplaceScreen(!mReplaceWindowMode);
			result = true;
			break;
		}
		//置換キャンセル
	  case kButton_CancelReplace:
		{
			MultiFileReplace_ClearReplaceData();
			break;
		}
		//置換除外ファイルへ追加
	  case kButton_ExcludeFile:
		{
			MultiFileReplace_ExcludeButton();
			break;
		}
		//置換対象ファイルへ追加
	  case kButton_IncludeFile:
		{
			MultiFileReplace_IncludeButton();
			break;
		}
		//#65
		//置換を実行ボタン
	  case kButton_ExecuteReplace:
		{
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("MultiFileReplaceConfirmation1");
			mes2.SetLocalizedText("MultiFileReplaceConfirmation2");
			mes3.SetLocalizedText("MultiFileReplaceConfirmation3");
			mes2.ReplaceParamText('0',mFindParamerForReplace.folderPath);
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kButton_ExecuteReplaceOK);
			break;
		}
		//置換実行OKボタン
	  case kButton_ExecuteReplaceOK:
		{
			MultiFileReplace_ExecuteReplace();
			break;
		}
		//検索中断ボタン
	  case kButton_FindAbort:
		{
			//マルチファイル検索中断
			GetApp().SPI_AbortMultiFileFind();
			//プログレステキスト中断テキスト設定
			AText	text;
			text.SetLocalizedText("Progress_Aborting");
			NVI_SetControlText(kProgressText,text);
			mFindForReplaceAborted = true;
			//
			NVM_UpdateControlStatus();
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

/**
置換対象ファイルダブルクリック時処理
*/
ABool	AWindow_MultiFileFind::EVTDO_DoubleClicked( const AControlID inID )
{
	switch(inID)
	{
	  case kControlID_ReplaceTargetListView:
		{
			AIndex	index = NVI_GetListView(kControlID_ReplaceTargetListView).SPI_GetSelectedDBIndex();
			AText	path;
			mReplaceTargetFilePathArray.Get(index,path);
			AFileAcc	file;
			file.Specify(path);
			GetApp().GetAppPref().LaunchAppWithFile(file,0);
			break;
		}
	  case kControlID_ReplaceExclusionListView:
		{
			AIndex	index = NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_GetSelectedDBIndex();
			AText	path;
			mReplaceExclusionFilePathArray.Get(index,path);
			AFileAcc	file;
			file.Specify(path);
			GetApp().GetAppPref().LaunchAppWithFile(file,0);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return true;
}

//検索実行
void	AWindow_MultiFileFind::Execute( const ABool inExtractMatchedText )
{
	if( GetApp().SPI_IsMultiFileFindWorking() == true )   return;
	
	//検索パラメータ取得
	AFindParameter	findParam;
	if( GetFindParam(findParam) == false )
	{
		return;
	}
	
	//正規表現チェック
	if( CheckRegExp(findParam) == false )   return;//B0317
	
	//最近使った検索テキスト等の設定
	SPI_AddRecentlyUsedFindText(findParam.findText);
	AText	filterText;
	NVI_GetControlText(kText_FileFilter,filterText);
	SPI_AddRecentlyUsedFileFilter(filterText);//#437
	//履歴に追加
	AFileAcc	folder;
	folder.Specify(findParam.folderPath);
	SPI_AddRecentlyUsedPath(folder);
	
	//検索実行
	ADocumentID	indexDocumentID = kObjectID_Invalid;
	if( mReplaceWindowMode == true )
	{
		//------------------置換用検索------------------
		//置換データ全削除
		MultiFileReplace_ClearReplaceData();
		//置換用検索パラメータを記憶
		mFindParamerForReplace.Set(findParam);
		//Step 2の検索フォルダ表示
		NVI_SetControlText(kButton_ReplaceBasePath,mFindParamerForReplace.folderPath);
		//検索結果表示先documentはマルチファイル置換ウインドウ内の結果表示用ドキュメント
		indexDocumentID = mFindResultDocumentID;
		//状態設定
		mExecutingFindForReplace = true;//置換用検索：true
		mFindForReplaceFinished = false;//置換用検索終了：false
		mFindForReplaceAborted = false;//置換用検索中止：false
		//表示更新
		NVM_UpdateControlStatus();
	}
	else
	{
		//------------------検索------------------
		//検索結果ウインドウ取得（または生成）
		indexDocumentID = GetApp().SPI_GetOrCreateFindResultWindowDocument();
	}
	//マルチファイル検索実行
	GetApp().SPI_StartMultiFileFind(findParam,indexDocumentID,mReplaceWindowMode,inExtractMatchedText);
}

/**
検索パラメータ取得
*/
ABool	AWindow_MultiFileFind::GetFindParam( AFindParameter& findParam ) const
{
	ABool	result = true;
	AFileAcc	folder;
	
	//検索文字列取得
	NVI_GetControlText(kText_FindText,findParam.findText);
	if( findParam.findText.GetLength() == 0 )   result = false;
	
	//#437
	//フィルタ文字列取得
	AText	filterText;
	NVI_GetControlText(kText_FileFilter,filterText);
	if( NVI_GetControlBool(kRadio_Target_WildCard) == true )//wildcard filter
	{
		ARegExp::ConvertToREFromWildCard(filterText,findParam.filterText);
	}
	else
	{
		findParam.filterText.SetText(filterText);
	}
	
	//フィルタ空または全ファイル対象ラジオボタンONなら.+を設定
	if( findParam.filterText.GetLength() == 0 || NVI_GetControlBool(kRadio_Target_AllFiles) == true )
	{
		findParam.filterText.SetCstring(".+");
	}
	
	//対象フォルダ取得
	//findParam.modeIndex = kIndex_Invalid;
	if( NVI_GetControlBool(kRadio_Folder_Select) == false )
	{
		AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
		if( docID == kObjectID_Invalid )   result = false;
		folder = mActiveFolder;
	}
	else
	{
		AText	pathtext;
		NVI_GetControlText(kText_Folder_Path,pathtext);
		folder.SpecifyWithAnyFilePathType(pathtext);//B0389
	}
	if( folder.Exist() == false )   result = false;
	folder.GetPath(findParam.folderPath);
	
	//検索オプション取得
	NVI_GetControlBool(kCheckBox_IgnoreCase,findParam.ignoreCase);
	NVI_GetControlBool(kCheckBox_WholeWord,findParam.wholeWord);
	NVI_GetControlBool(kCheckBox_Aimai,findParam.fuzzy);
	findParam.ignoreBackslashYen = !(NVI_GetControlBool(kCheckBox_BackslashYen));
	NVI_GetControlBool(kCheckBox_RegExp,findParam.regExp);
	NVI_GetControlBool(kCheckBox_Recursive,findParam.recursive);
	NVI_GetControlBool(kCheckBox_DisplayPosition,findParam.displayPosition);
	NVI_GetControlBool(kCheckBox_IgnoreSpaces,findParam.ignoreSpaces);//#165
	NVI_GetControlBool(kCheckBox_OnlyVisibleFile,findParam.onlyVisibleFile);
	NVI_GetControlBool(kCheckBox_OnlyTextFile,findParam.onlyTextFile);
	
	return result;
}

void	AWindow_MultiFileFind::EVTDO_TextInputted( const AControlID inID )
{
	switch(inID)
	{
	  case kText_Folder_Path:
		{
			NVI_SetControlBool(kRadio_Folder_Select,true);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	NVM_UpdateControlStatus();
}

void	AWindow_MultiFileFind::EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder )
{
	AText	path;
	inFolder.GetPath(path);
	switch(inControlID)
	{
	  case kButton_SelectFolder:
		{
			NVI_SetControlText(kText_Folder_Path,path);
			NVI_SetControlBool(kRadio_Folder_Select,true);
			//#1443 test inFolder.CreateSecurityScopedBookmark();
			break;
		}
	  case kButton_SelectBackupFolder:
		{
			NVI_SetControlText(kText_BackupFolder,path);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//B0317
ABool	AWindow_MultiFileFind::CheckRegExp( const AFindParameter& inFindParam )
{
	if( inFindParam.regExp == true )
	{
		ARegExp	regexpcheck;
		if( regexpcheck.SetRE(inFindParam.findText) == false )
		{
			AText	message1, message2;
			message1.SetLocalizedText("RegExpError1");
			message2.SetLocalizedText("RegExpError2");
			message2.ReplaceParamText('0',inFindParam.findText);
			NVI_SelectWindow();
			NVI_ShowChildWindow_OK(message1,message2);
			return false;
		}
	}
	return true;
}

//R0000
void	AWindow_MultiFileFind::EVTDO_DoFileDropped( const AControlID inControlID, const AFileAcc& inFile )
{
	AFileAcc	folder;
	folder.CopyFrom(inFile);
	if( folder.IsFolder() == false)
	{
		folder.SpecifyParent(inFile);
	}
	AText	path;
	folder.GetPath(path);
	NVI_SetControlText(kText_Folder_Path,path);
	NVI_SetControlBool(kRadio_Folder_Select,true);
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void AWindow_MultiFileFind::NVIDO_Create( const ADocumentID inDocumentID )
{
	if( mReplaceWindowMode == true )
	{
		NVM_CreateWindow("main/MultiFileReplace");
	}
	else
	{
		NVM_CreateWindow("main/MultiFileFind");
	}
	NVI_CreateEditBoxView(kText_FindText);
	NVI_SetControlBindings(kText_FindText,true,true,true,false,false,true);
	NVI_GetEditBoxView(kText_FindText).SPI_SetCenterizeWhen1Line();
	NVI_RegisterToFocusGroup(kPopup_FileFilterPreset,true);//#617
	//NVI_CreateEditBoxView(kText_FileFilter);
	NVI_RegisterToFocusGroup(kPopup_RecentlyUsedFileFilter,true);//#353
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_Folder_SameFolder);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Folder_ParentFolder);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Folder_SameProject);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Folder_Select);
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_Target_AllFiles);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Target_WildCard);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Target_RegExp);
	NVI_RegisterToFocusGroup(kButton_Find,true);//#353
	NVI_RegisterToFocusGroup(kPopup_RecentlyUsedPath,true);//#353
	NVI_RegisterToFocusGroup(kButton_SelectFolder,true);//#353
	//NVI_CreateEditBoxView(kText_Folder_Path);
	NVI_RegisterToFocusGroup(kCheckBox_IgnoreCase,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_WholeWord,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_IgnoreSpaces,true);//#165
	NVI_RegisterToFocusGroup(kCheckBox_BackslashYen,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_RegExp,true);//#353
	//#688 NVI_RegisterToFocusGroup(kCheckBox_AllowReturnTab,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_Recursive,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_DisplayPosition,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_OnlyVisibleFile,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_OnlyTextFile,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_Aimai,true);//#353
	NVI_RegisterToFocusGroup(kPopup_RecentlyUsedFindText,true);//#353
	
	
	AText	text;
	//win text.SetCstring(".+");
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kFindDefaultFileFilter,text);//win
	
	//ファイル名一致条件復元
	//B0414
	if( GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter).GetItemCount() > 0 )
	{
		GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter).Get(0,text);
	}
	NVI_SetControlText(kText_FileFilter,text);
	
	//最近検索したフォルダ復元
	if(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder).GetItemCount() > 0 )
	{
		GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder).Get(0,text);
	}
	
	//パステキスト設定
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kMultiFileFind_FolderPathText,text);
	if( text.GetItemCount() == 0 )
	{
		//設定データが空白テキストの場合（ver3での最初の起動時、または、削除した場合）、履歴の最初のテキストを設定
		if( GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder).GetItemCount() > 0 )
		{
			GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder).Get(0,text);
		}
	}
	NVI_SetControlText(kText_Folder_Path,text);
	
	//フィルタテキスト設定
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kMultiFileFind_FileFilterText,text);
	if( text.GetItemCount() == 0 )
	{
		//設定データが空白テキストの場合（ver3での最初の起動時、または、削除した場合）、履歴の最初のテキストを設定
		if( GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter).GetItemCount() > 0 )
		{
			GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter).Get(0,text);
		}
	}
	if( text.GetItemCount() == 0 )
	{
		//上記でさらに空白テキストの場合、ワイルドカード*.*を設定
		text.SetCstring("*.*");
		GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kMultiFileFindFileFilterKind,1);
	}
	NVI_SetControlText(kText_FileFilter,text);
	
	//フォルダラジオボタン更新
	UpdateFolderRadioButtons();
	
	//検索オプションデフォルト設定
	NVI_SetControlBool(kCheckBox_IgnoreCase,		GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreCase));
	NVI_SetControlBool(kCheckBox_WholeWord,		GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultWholeWord));
	NVI_SetControlBool(kCheckBox_Aimai,			GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultAimai));
	NVI_SetControlBool(kCheckBox_BackslashYen,	(GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreBackslashYen)==false));
	NVI_SetControlBool(kCheckBox_RegExp,			GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultRegExp));
	//#688 NVI_SetControlBool(kCheckBox_AllowReturnTab,	GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultAllowReturn));
	NVI_SetControlBool(kCheckBox_Recursive,		GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultRecursive));
	NVI_SetControlBool(kCheckBox_DisplayPosition,	GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultDisplayPosition));
	NVI_SetControlBool(kCheckBox_IgnoreSpaces,		GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreSpaces));//#165
	//B0313
	NVI_SetControlBool(kCheckBox_OnlyVisibleFile,	GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultOnlyVisibleFile));
	NVI_SetControlBool(kCheckBox_OnlyTextFile,	GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultOnlyTextFile));
	
	NVI_SetDefaultOKButton(kButton_Find);
	
	UpdateRecentlyUsedFindTextMenu();
	UpdateRecentlyUsedFileFilterMenu();
	UpdateRecentlyUsedPathMenu();
	
	//対象ラジオボタン設定
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kMultiFileFind_UseFileFilter) == false )
	{
		NVI_SetControlBool(kRadio_Target_AllFiles,true);
	}
	else if( GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kMultiFileFindFileFilterKind) == 0 )
	{
		NVI_SetControlBool(kRadio_Target_RegExp,true);
	}
	else if( GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kMultiFileFindFileFilterKind) == 1 )
	{
		NVI_SetControlBool(kRadio_Target_WildCard,true);
	}
	
	//#617
	NVI_RegisterTextArrayMenu(kPopup_FileFilterPreset,kTextArrayMenuID_FileFilterPreset);
	NVI_SetControlNumber(kPopup_FileFilterPreset,0);
	
	//DocImp生成 #1034
	AObjectID	docImpObjectID = GetApp().SPI_CreateDocImp(kDocumentType_IndexWindow);
	//検索結果用Document生成
	AIndexWindowDocumentFactory	docfactory(this,docImpObjectID);//#1034
	mFindResultDocumentID = GetApp().NVI_CreateDocument(docfactory);
	
	if( mReplaceWindowMode == true )
	{
		//==================マルチファイル置換用画面==================
		
		const ANumber	kIndexViewFileColumnWidth = 250;
		
		//ウインドウ最小サイズ設定
		ARect	bounds = {0};
		NVI_GetWindowBounds(bounds);
		NVI_SetWindowMinimumSize(bounds.right-bounds.left,bounds.bottom-bounds.top);
		NVI_SetWindowMaximumSize(bounds.right-bounds.left,bounds.bottom-bounds.top);
		
		//置換テキスト
		NVI_CreateEditBoxView(kText_ReplaceText);
		NVI_SetControlBindings(kText_ReplaceText,true,true,true,false,false,true);
		NVI_GetEditBoxView(kText_ReplaceText).SPI_SetCenterizeWhen1Line();
		
		//検索結果View生成
		AIndexViewFactory	indexViewFactory(GetObjectID(),kControlID_IndexView,mFindResultDocumentID);
		AViewID	indexViewID = NVM_CreateView(kControlID_IndexView,false,indexViewFactory);
		AView_Index::GetIndexViewByViewID(indexViewID).NVI_CreateFrameView();
		AView_Index::GetIndexViewByViewID(indexViewID).NVI_CreateVScrollBar();
		AView_Index::GetIndexViewByViewID(indexViewID).SPI_SetFileColumnWidth(kIndexViewFileColumnWidth);
		
		//Replace Target File ListView生成
		AListViewFactory	targetListViewFactory(GetObjectID(),kControlID_ReplaceTargetListView);
		NVM_CreateView(kControlID_ReplaceTargetListView,false,targetListViewFactory);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_CreateFrame(true,true,false);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetTextDrawProperty(GetEmptyText(),10.0,kColor_Black);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetEllipsisFirstCharacters(true);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_RegisterColumn_Text(0,200,"MultiFileReplace_FileListTitle_File",false);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_RegisterColumn_Text(1,0,"MultiFileReplace_FileListTitle_TextEncoding",false);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetMultiSelectEnabled(true);
		
		//Replace Exclusion File ListView生成
		AListViewFactory	exclusionListViewFactory(GetObjectID(),kControlID_ReplaceExclusionListView);
		NVM_CreateView(kControlID_ReplaceExclusionListView,false,exclusionListViewFactory);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_CreateFrame(true,true,false);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetTextDrawProperty(GetEmptyText(),10.0,kColor_Black);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetEllipsisFirstCharacters(true);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_RegisterColumn_Text(0,200,"MultiFileReplace_FileListTitle_File",false);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_RegisterColumn_Text(1,0,"MultiFileReplace_FileListTitle_TextEncoding",false);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetMultiSelectEnabled(true);
		
		//バックアップフォルダパステキストボックス
		AText	backupFolderPath;
		GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kMultiFileFind_BackupFolder,backupFolderPath);
		if( backupFolderPath.GetItemCount() == 0 )
		{
			//バックアップフォルダの設定が空の場合はデフォルトを設定
			AFileAcc	backupRootFolder;
			GetApp().SPI_GetDefaultMultiFileReplaceBackupFolder(backupRootFolder);
			backupRootFolder.GetPath(backupFolderPath);
		}
		NVI_SetControlText(kText_BackupFolder,backupFolderPath);
	}
	else
	{
		//==================マルチファイル検索用画面==================
		
		const ANumber kWindowMinimumWidth = 280;
		const ANumber kWindowMaximumWidth = 100000;
		
		//ウインドウ最小サイズ設定
		ARect	bounds = {0};
		NVI_GetWindowBounds(bounds);
		NVI_SetWindowMinimumSize(kWindowMinimumWidth,bounds.bottom-bounds.top);
		NVI_SetWindowMaximumSize(kWindowMaximumWidth,bounds.bottom-bounds.top);
		
		//ウインドウ幅設定
		NVI_SetWindowWidth(GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kMultiFileFindWindow_WindowWidth));
		
		//#872
		//検索オプション折りたたみ状態によるウインドウサイズ設定
		NVI_SetControlBool(kTriangle_FindOptions,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kMultiFileFindWindow_FindOptionsExpanded));
		NVI_SetControlBool(kTriangle_AdvancedSearch,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kMultiFileFindWindow_AdvancedSearchExpanded));
		NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
		
		//プログレス初期化
		SPI_MultiFileReplace_SetProgressText(GetEmptyText());
		SPI_MultiFileReplace_Progress(0);
	}
	
	//#1373
	NVI_RegisterHelpAnchor(90001,"find.htm#option");
}

void	AWindow_MultiFileFind::NVIDO_UpdateProperty()
{
	//win
	AText	fontname;
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kFindDialogFontName,fontname);
	//
	//フォント設定
	NVI_SetControlTextFont(kText_FindText,
			//win GetApp().GetAppPref().GetData_Font(AAppPrefDB::kFindDialogFont),
			fontname,
			GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFind_FontSize));
	//R0227
	AFloatNumber	fontsize = GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFind_FontSize);
	if( fontsize > 12.0 )   fontsize = 12.0;
	//NVI_SetControlTextFont(kText_Folder_Path,fontname,fontsize);
	//#616 AFontWrapper::GetDialogDefaultFontName(fontname);
	//NVI_SetControlTextFont(kText_FileFilter,fontname,kTextSize);//win
	//NVI_SetControlTextFont(kText_Folder_Path,fontname,kTextSize);//win
}

void	AWindow_MultiFileFind::NVIDO_Show()
{
	if( mOpenClosingReplaceScreen == false )
	{
		UpdateFolderRadioButtons();
	}
}

/**
フォルダラジオボタン更新
*/
void	AWindow_MultiFileFind::UpdateFolderRadioButtons()
{
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		//==================テキストウインドウが有る場合==================
		//設定によって、下記いずれかのラジオボタンをONにする
		switch(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kMultiFileFind_ActiveFolderKind))
		{
		  case 0:
			{
				NVI_SetControlBool(kRadio_Folder_SameFolder,true);
				break;
			}
		  case 1:
			{
				NVI_SetControlBool(kRadio_Folder_ParentFolder,true);
				break;
			}
		  case 2:
			{
				NVI_SetControlBool(kRadio_Folder_SameProject,true);
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	else
	{
		//==================テキストウインドウが無い場合==================
		//手動フォルダ選択ラジオボタンをONにする
		NVI_SetControlBool(kRadio_Folder_Select,true);
	}
}

void	AWindow_MultiFileFind::NVIDO_Hide()
{
	//設定をdbに保存
	if( NVI_GetControlBool(kRadio_Folder_SameFolder) == true )
	{
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kMultiFileFind_ActiveFolderKind,0);
	}
	else if( NVI_GetControlBool(kRadio_Folder_ParentFolder) == true )
	{
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kMultiFileFind_ActiveFolderKind,1);
	}
	else if( NVI_GetControlBool(kRadio_Folder_SameProject) == true )
	{
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kMultiFileFind_ActiveFolderKind,2);
	}
	
	if( NVI_GetControlBool(kRadio_Target_AllFiles) == true )
	{
		GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kMultiFileFind_UseFileFilter,false);
	}
	else if( NVI_GetControlBool(kRadio_Target_WildCard) == true )
	{
		GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kMultiFileFind_UseFileFilter,true);
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kMultiFileFindFileFilterKind,1);
	}
	else if( NVI_GetControlBool(kRadio_Target_RegExp) == true )
	{
		GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kMultiFileFind_UseFileFilter,true);
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kMultiFileFindFileFilterKind,0);
	}
	//
	AText	text;
	NVI_GetControlText(kText_Folder_Path,text);
	GetApp().NVI_GetAppPrefDB().SetData_Text(AAppPrefDB::kMultiFileFind_FolderPathText,text);
	NVI_GetControlText(kText_FileFilter,text);
	GetApp().NVI_GetAppPrefDB().SetData_Text(AAppPrefDB::kMultiFileFind_FileFilterText,text);
	
	if( mReplaceWindowMode == false )
	{
		//==================検索用画面==================
		//#872
		//折りたたみ状態保存
		GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kMultiFileFindWindow_FindOptionsExpanded,NVI_GetControlBool(kTriangle_FindOptions));
		GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kMultiFileFindWindow_AdvancedSearchExpanded,NVI_GetControlBool(kTriangle_AdvancedSearch));
		
		//ウインドウ幅保存
		GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kMultiFileFindWindow_WindowWidth,NVI_GetWindowWidth());
	}
	else
	{
		//==================置換用画面==================
		//バックアップフォルダ保存
		AText	backupFolderPath;
		NVI_GetControlText(kText_BackupFolder,backupFolderPath);
		GetApp().NVI_GetAppPrefDB().SetData_Text(AAppPrefDB::kMultiFileFind_BackupFolder,backupFolderPath);
		
	}
}

//コントロール状態（Enable/Disable等）を更新
//NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()からコールされる
//クリックやアクティベートでコールされるので、あまり重い処理をNVMDO_UpdateControlStatus()には入れないこと。
void	AWindow_MultiFileFind::NVMDO_UpdateControlStatus()
{
	//------------------アクティブフォルダ設定------------------
	if( !(mReplaceWindowMode == true && (mFindForReplaceFinished == true || mExecutingFindForReplace == true) ) )
	{
		ADocumentID	docID = kObjectID_Invalid;
		//最前面のテキストウインドウ取得
		AWindowID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
		if( winID != kObjectID_Invalid )
		{
			//最前面のテキストウインドウがあれば、そのウインドウの現在のドキュメントを取得
			docID = GetApp().SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextDocument().GetObjectID();
		}
		if( docID != kObjectID_Invalid )
		{
			//------------------テキストウインドウがある場合------------------
			//現在のアクティブなテキストウインドウのフォルダを設定する。
			//B0431
			if( GetApp().SPI_GetTextDocumentByID(docID).SPI_IsRemoteFileMode() == true || 
			   GetApp().SPI_GetTextDocumentByID(docID).NVI_IsFileSpecified() == false )
			{
				mActiveFolder.Unspecify();
			}
			else if( NVI_GetControlBool(kRadio_Folder_SameFolder) == true )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParentFolder(mActiveFolder);
				AText	text;
				GetApp().GetAppPref().GetFilePathForDisplay(mActiveFolder,text);//B0389
				NVI_SetControlText(kText_Folder_Path,text);
			}
			else if( NVI_GetControlBool(kRadio_Folder_ParentFolder) == true )
			{
				AFileAcc	folder;
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParentFolder(folder);
				mActiveFolder.SpecifyParent(folder);
				AText	text;
				GetApp().GetAppPref().GetFilePathForDisplay(mActiveFolder,text);//B0389
				NVI_SetControlText(kText_Folder_Path,text);
			}
			else if( NVI_GetControlBool(kRadio_Folder_SameProject) == true )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetProjectFolder(mActiveFolder);
				if( mActiveFolder.IsSpecified() == false )
				{
					GetApp().SPI_GetTextDocumentByID(docID).SPI_GetParentFolder(mActiveFolder);
				}
				AText	text;
				GetApp().GetAppPref().GetFilePathForDisplay(mActiveFolder,text);//B0389
				NVI_SetControlText(kText_Folder_Path,text);
			}
			//ラジオボタンenable
			NVI_SetControlEnable(kRadio_Folder_SameFolder,true);
			NVI_SetControlEnable(kRadio_Folder_ParentFolder,true);
			NVI_SetControlEnable(kRadio_Folder_SameProject,true);
		}
		else
		{
			//------------------テキストウインドウがない場合==================
			//ラジオボタンdisable
			NVI_SetControlEnable(kRadio_Folder_SameFolder,false);
			NVI_SetControlEnable(kRadio_Folder_ParentFolder,false);
			NVI_SetControlEnable(kRadio_Folder_SameProject,false);
		}
	}
	
	//------------------検索オプション------------------
	/*#688
	NVI_SetCatchTab(kText_FindText,NVI_GetControlBool(kCheckBox_AllowReturnTab));
	NVI_SetCatchReturn(kText_FindText,NVI_GetControlBool(kCheckBox_AllowReturnTab));
	*/
	//#930 NVI_SetControlEnable(kCheckBox_IgnoreCase,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_WholeWord,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_Aimai,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_BackslashYen,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_IgnoreSpaces,(NVI_GetControlBool(kCheckBox_RegExp)==false));//#165
	
	AText	text;
	NVI_GetControlText(kText_FindText,text);
	NVI_SetControlEnable(kButton_Find,(text.GetLength()>0)&&((GetApp().SPI_IsMultiFileFindWorking()==false)));
	NVI_SetControlEnable(kButton_OpenCloseReplaceScreen,(GetApp().SPI_IsMultiFileFindWorking()==false));
	//if( NVI_GetControlBool(kRadio_Path) == true )
	{
		AText	text;
		NVI_GetControlText(kText_Folder_Path,text);
		if( text.GetLength() == 0 )
		{
			NVI_SetControlEnable(kButton_Find,false);
		}
	}
	/*
	//B0431
	if( NVI_GetControlBool(kRadio_ActiveFolder) == true )
	{
		if( mActiveFolder.IsSpecified() == false )
		{
			NVI_SetControlEnable(kButton_Find,false);
		}
	}
	*/
	
	//ラジオボタンで対象：全ファイルを選択時の各コントロールdisable
	NVI_SetControlEnable(kText_FileFilter,!NVI_GetControlBool(kRadio_Target_AllFiles));
	NVI_SetControlEnable(kPopup_RecentlyUsedFileFilter,!NVI_GetControlBool(kRadio_Target_AllFiles));
	NVI_SetControlEnable(kPopup_FileFilterPreset,!NVI_GetControlBool(kRadio_Target_AllFiles));//#617
	
	//検索中止ボタン #1376
	if( mReplaceWindowMode == false )
	{
		NVI_SetControlEnable(kButton_FindAbort,(GetApp().SPI_IsMultiFileFindWorking()==true));
	}
	
	//==================マルチファイル置換用==================
	
	if( mReplaceWindowMode == true )
	{
		//置換用検索実行中かどうかの判定
		if( mExecutingFindForReplace == true )
		{
			//検索実行中ならプログレス・中断コントロール表示
			NVI_SetShowControl(kProgressText,true);
			NVI_SetShowControl(kButton_FindAbort,true);
			NVI_SetShowControl(kProgressText,true);
		}
		else
		{
			//検索実行中でなければプログレス・中断コントロール非表示
			NVI_SetProgressIndicatorProgress(0,0,false);
			NVI_SetShowControl(kProgressText,false);
			NVI_SetShowControl(kButton_FindAbort,false);
			NVI_SetShowControl(kProgressText,false);
		}
		//置換テキスト取得
		AText	replaceText;
		NVI_GetControlText(kText_ReplaceText,replaceText);
		//置換テキストエリアは改行可能 #1060
		NVI_SetCatchTab(kText_ReplaceText,true);
		NVI_SetCatchReturn(kText_ReplaceText,true);
		//置換用検索完了かどうかの判定
		if( mFindForReplaceFinished == true )
		{
			//置換用検索完了なら置換実行・キャンセルボタンenable
			NVI_SetControlEnable(kButton_ExecuteReplace,true);//(replaceText.GetItemCount() > 0));
			NVI_SetControlEnable(kButton_CancelReplace,true);
		}
		else
		{
			//置換用検索未完了なら置換実行・キャンセルボタンdisable
			NVI_SetControlEnable(kButton_ExecuteReplace,false);
			NVI_SetControlEnable(kButton_CancelReplace,false);
		}
		//置換用検索実行中または置換用検索完了かどうかの判定
		if( mFindForReplaceFinished == true || mExecutingFindForReplace == true )
		{
			//置換用検索実行中または置換用検索完了なら各検索パラメータをdisable
			NVI_SetControlEnable(kText_FindText,false);
			NVI_SetControlEnable(kRadio_Folder_SameFolder,false);
			NVI_SetControlEnable(kRadio_Folder_ParentFolder,false);
			NVI_SetControlEnable(kRadio_Folder_SameProject,false);
			NVI_SetControlEnable(kRadio_Folder_Select,false);
			NVI_SetControlEnable(kText_Folder_Path,false);
			NVI_SetControlEnable(kButton_SelectFolder,false);
			NVI_SetControlEnable(kPopup_RecentlyUsedFindText,false);
			NVI_SetControlEnable(kPopup_RecentlyUsedPath,false);
			NVI_SetControlEnable(kRadio_Target_AllFiles,false);
			NVI_SetControlEnable(kRadio_Target_WildCard,false);
			NVI_SetControlEnable(kRadio_Target_RegExp,false);
			NVI_SetControlEnable(kPopup_FileFilterPreset,false);
			NVI_SetControlEnable(kPopup_RecentlyUsedFileFilter,false);
			NVI_SetControlEnable(kText_FileFilter,false);
			NVI_SetControlEnable(kCheckBox_IgnoreCase,false);
			NVI_SetControlEnable(kCheckBox_WholeWord,false);
			NVI_SetControlEnable(kCheckBox_Aimai,false);
			NVI_SetControlEnable(kCheckBox_BackslashYen,false);
			NVI_SetControlEnable(kCheckBox_RegExp,false);
			NVI_SetControlEnable(kCheckBox_Recursive,false);
			NVI_SetControlEnable(kCheckBox_DisplayPosition,false);
			NVI_SetControlEnable(kCheckBox_OnlyVisibleFile,false);
			NVI_SetControlEnable(kCheckBox_OnlyTextFile,false);
			NVI_SetControlEnable(kCheckBox_IgnoreSpaces,false);
			NVI_SetControlEnable(kButton_SetupFuzzySearch,false);
		}
		else
		{
			//置換用検索実行中または置換用検索完了なら各検索パラメータをenable
			NVI_SetControlEnable(kText_FindText,true);
			//NVI_SetControlEnable(kRadio_Folder_SameFolder,true);
			//NVI_SetControlEnable(kRadio_Folder_ParentFolder,true);
			//NVI_SetControlEnable(kRadio_Folder_SameProject,true);
			NVI_SetControlEnable(kRadio_Folder_Select,true);
			NVI_SetControlEnable(kText_Folder_Path,true);
			NVI_SetControlEnable(kButton_SelectFolder,true);
			NVI_SetControlEnable(kPopup_RecentlyUsedFindText,true);
			NVI_SetControlEnable(kPopup_RecentlyUsedPath,true);
			NVI_SetControlEnable(kRadio_Target_AllFiles,true);
			NVI_SetControlEnable(kRadio_Target_WildCard,true);
			NVI_SetControlEnable(kRadio_Target_RegExp,true);
			//NVI_SetControlEnable(kPopup_FileFilterPreset,true);
			//NVI_SetControlEnable(kPopup_RecentlyUsedFileFilter,true);
			//NVI_SetControlEnable(kText_FileFilter,true);
			NVI_SetControlEnable(kCheckBox_IgnoreCase,true);//#0
			//NVI_SetControlEnable(kCheckBox_WholeWord,true);
			//NVI_SetControlEnable(kCheckBox_Aimai,true);
			//NVI_SetControlEnable(kCheckBox_BackslashYen,true);
			NVI_SetControlEnable(kCheckBox_RegExp,true);
			NVI_SetControlEnable(kCheckBox_Recursive,true);
			NVI_SetControlEnable(kCheckBox_DisplayPosition,true);
			NVI_SetControlEnable(kCheckBox_OnlyVisibleFile,true);
			NVI_SetControlEnable(kCheckBox_OnlyTextFile,true);
			//NVI_SetControlEnable(kCheckBox_IgnoreSpaces,true);
			NVI_SetControlEnable(kButton_SetupFuzzySearch,true);
		}
		//置換対象ファイル除外ボタン更新
		AArray<AIndex>	indexArray;
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_GetSelectedDBIndexArray(indexArray);
		NVI_SetControlEnable(kButton_ExcludeFile,(indexArray.GetItemCount()>0));
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_GetSelectedDBIndexArray(indexArray);
		NVI_SetControlEnable(kButton_IncludeFile,(indexArray.GetItemCount()>0));
		
		//置換対象ファイル追加ボタン更新
		NVI_SetControlEnable(kText_BackupFolder,NVI_GetControlBool(kCheck_BackupFolder));
		NVI_SetControlEnable(kButton_SelectBackupFolder,NVI_GetControlBool(kCheck_BackupFolder));
		NVI_SetControlEnable(kButton_DefaultBackupFolder,NVI_GetControlBool(kCheck_BackupFolder));
		
	}
	
}

//WindowActivated時のコールバック
void	AWindow_MultiFileFind::EVTDO_WindowActivated()
{
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		NVI_SetInputBackslashByYenKeyMode(GetApp().SPI_GetModePrefDB(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex()).
					GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey));
	}
	else
	{
		NVI_SetInputBackslashByYenKeyMode(false);
	}
}

//WindowDeactivated時のコールバック
void	AWindow_MultiFileFind::EVTDO_WindowDeactivated()
{
}

/**
ウインドウを閉じるボタン・閉じるメニュー選択時処理（AWindow::NVI_TryClose()にて、AWindowのクローズ可能フラグ（＝検索中はクローズ不可）チェック済み）
*/
void	AWindow_MultiFileFind::NVIDO_TryClose( const AIndex inTabIndex )
{
	NVI_Hide();
}

#pragma mark ===========================

#pragma mark ---外部からのパラメータ設定

//FindText設定
void	AWindow_MultiFileFind::SPI_SetFindText( const AText& inFindText )
{
	NVI_SetControlText(kText_FindText,inFindText);
	NVM_UpdateControlStatus();
}

#pragma mark ===========================

#pragma mark ---マルチファイル置換

//#65
/**
置換用画面open/close
*/
void	AWindow_MultiFileFind::SPI_OpenCloseReplaceScreen( const ABool inReplaceMode )
{
	if( inReplaceMode != mReplaceWindowMode )
	{
		//置換用画面open/close中フラグON
		mOpenClosingReplaceScreen = true;
		//パラメータを記憶
		ABool	selectFolderRadioButton = NVI_GetControlBool(kRadio_Folder_Select);
		AFindParameter	findParam;
		GetFindParam(findParam);
		//ウインドウを閉じる
		NVI_Close();
		//置換用画面状態変更
		mReplaceWindowMode = inReplaceMode;
		//ウインドウ生成
		NVI_Create(kObjectID_Invalid);
		//状態復元
		NVI_SetControlText(kText_FindText,findParam.findText);
		NVI_SetControlBool(kRadio_Folder_Select,selectFolderRadioButton);
		NVI_SetControlBool(kCheckBox_IgnoreCase,findParam.ignoreCase);
		NVI_SetControlBool(kCheckBox_WholeWord,findParam.wholeWord);
		NVI_SetControlBool(kCheckBox_Aimai,findParam.fuzzy);
		NVI_SetControlBool(kCheckBox_BackslashYen,!(findParam.ignoreBackslashYen));
		NVI_SetControlBool(kCheckBox_RegExp,findParam.regExp);
		NVI_SetControlBool(kCheckBox_Recursive,findParam.recursive);
		NVI_SetControlBool(kCheckBox_DisplayPosition,findParam.displayPosition);
		NVI_SetControlBool(kCheckBox_IgnoreSpaces,findParam.ignoreSpaces);//#165
		NVI_SetControlBool(kCheckBox_OnlyVisibleFile,findParam.onlyVisibleFile);
		NVI_SetControlBool(kCheckBox_OnlyTextFile,findParam.onlyTextFile);
		//置換状態初期化
		mExecutingFindForReplace = false;
		mFindForReplaceFinished = false;
		mFindForReplaceAborted = false;
		//ウインドウ表示
		NVI_Show();
		//置換用画面open/close中フラグOFF
		mOpenClosingReplaceScreen = false;
	}
}

/**
マルチファイル検索結果InternalEvent受信時処理
*/
void	AWindow_MultiFileFind::SPI_MultiFileReplace_FindResult( const ATextArray& inFilePathArray, const AText& inTextEncoding )
{
	//ウインドウが表示されていない時や、置換用画面ではないときは何もしない
	if( NVI_IsWindowVisible() == false || mReplaceWindowMode == false )   return;
	
	//結果ファイルリストを、対象ファイルリストに追加
	AItemCount	fileCount = inFilePathArray.GetItemCount();
	for( AIndex i = 0; i < fileCount; i++ )
	{
		AText	filepath;
		inFilePathArray.Get(i,filepath);
		if( filepath.GetItemCount() > 0 &&
					mReplaceTargetFilePathArray.Find(filepath) == kIndex_Invalid && 
					mReplaceExclusionFilePathArray.Find(filepath) == kIndex_Invalid )
		{
			mReplaceTargetFilePathArray.Add(filepath);
			mReplaceTargetFilePathArray_TextEncoding.Add(inTextEncoding);
		}
	}
	//対象ファイルテーブルを更新
	MultiFileReplace_UpdateTargetFileTables();
}

/**
マルチファイル検索完了InternalEvent受信時処理
*/
void	AWindow_MultiFileFind::SPI_MultiFileReplace_FindCompleted()
{
	//ウインドウが表示されていない時や、置換用画面ではないときは何もしない
	if( NVI_IsWindowVisible() == false || mReplaceWindowMode == false )   return;
	
	//検索中断判定
	if( mFindForReplaceAborted == false )
	{
		//中断でなければ、置換用検索完了フラグON
		mFindForReplaceFinished = true;
	}
	else
	{
		//中断の場合、置換用検索完了フラグOFF
		mFindForReplaceFinished = false;
	}
	//置換用検索実行中フラグOFF
	mExecutingFindForReplace = false;
	//プログレステキストクリア
	SPI_MultiFileReplace_SetProgressText(GetEmptyText());
	
	//表示更新
	NVM_UpdateControlStatus();
}

/**
置換用パラメータデータ全削除
*/
void	AWindow_MultiFileFind::MultiFileReplace_ClearReplaceData()
{
	//全フラグOFF
	mExecutingFindForReplace = false;
	mFindForReplaceFinished = false;
	mFindForReplaceAborted = false;
	
	//対象ファイルリスト全削除
	mReplaceTargetFilePathArray.DeleteAll();
	mReplaceTargetFilePathArray_TextEncoding.DeleteAll();
	mReplaceExclusionFilePathArray.DeleteAll();
	mReplaceExclusionFilePathArray_TextEncoding.DeleteAll();
	//対象ファイルテーブル更新
	MultiFileReplace_UpdateTargetFileTables();
	//検索結果全削除
	GetApp().SPI_GetIndexWindowDocumentByID(mFindResultDocumentID).SPI_DeleteAllGroup();
	//表示更新
	NVM_UpdateControlStatus();
}

/**
置換対象ファイルテーブル更新
*/
void	AWindow_MultiFileFind::MultiFileReplace_UpdateTargetFileTables()
{
	//マルチファイル検索実行フォルダのパスの文字長取得
	AText	baseFolderPath;
	baseFolderPath.SetText(mFindParamerForReplace.folderPath);
	baseFolderPath.AddPathDelimiter(kUChar_Slash);
	AItemCount	baseFolderPathLen = baseFolderPath.GetItemCount();
	//置換対象ファイルテーブル更新
	{
		//相対パスに変換
		ATextArray	partialPathArray;
		AItemCount	itemCount = mReplaceTargetFilePathArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AText	path;
			mReplaceTargetFilePathArray.Get(i,path);
			path.Delete(0,baseFolderPathLen);
			partialPathArray.Add(path);
		}
		//テーブルに設定
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_BeginSetTable();
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetColumn_Text(0,partialPathArray);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetColumn_Text(1,mReplaceTargetFilePathArray_TextEncoding);
		NVI_GetListView(kControlID_ReplaceTargetListView).SPI_EndSetTable();
	}
	//置換除外ファイルテーブル更新
	{
		//相対パスに変換
		ATextArray	partialPathArray;
		AItemCount	itemCount = mReplaceExclusionFilePathArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AText	path;
			mReplaceExclusionFilePathArray.Get(i,path);
			path.Delete(0,baseFolderPathLen);
			partialPathArray.Add(path);
		}
		//テーブルに設定
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_BeginSetTable();
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetColumn_Text(0,partialPathArray);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetColumn_Text(1,mReplaceExclusionFilePathArray_TextEncoding);
		NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_EndSetTable();
	}
	//表示更新
	NVM_UpdateControlStatus();
}

/**
除外ボタンクリック時処理
*/
void	AWindow_MultiFileFind::MultiFileReplace_ExcludeButton()
{
	//選択項目index取得
	AArray<AIndex>	indexArray;
	NVI_GetListView(kControlID_ReplaceTargetListView).SPI_GetSelectedDBIndexArray(indexArray);
	//選択解除
	NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetSelect(kIndex_Invalid);
	NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetSelect(kIndex_Invalid);
	//対象ファイルリストから選択項目のファイルパスを取得
	ATextArray	pathArray;
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		pathArray.Add(mReplaceTargetFilePathArray.GetTextConst(indexArray.Get(i)));
	}
	//対象ファイルリストからファイルパスを削除し、除外ファイルリストにファイルパスを追加
	AText	tecname;
	for( AIndex i = 0; i < pathArray.GetItemCount(); i++ )
	{
		AText	path;
		pathArray.Get(i,path);
		//リストから削除
		for( AIndex index = 0; index < mReplaceTargetFilePathArray.GetItemCount();  )
		{
			if( mReplaceTargetFilePathArray.GetTextConst(index).Compare(path) == true )
			{
				mReplaceTargetFilePathArray.Delete1(index);
				mReplaceTargetFilePathArray_TextEncoding.Get(index,tecname);
				mReplaceTargetFilePathArray_TextEncoding.Delete1(index);
			}
			else
			{
				index++;
			}
		}
		//
		mReplaceExclusionFilePathArray.Add(path);
		mReplaceExclusionFilePathArray_TextEncoding.Add(tecname);
	}
	//テーブル更新
	MultiFileReplace_UpdateTargetFileTables();
	//表示更新
	NVM_UpdateControlStatus();
}

/**
対象追加ボタンクリック時処理
*/
void	AWindow_MultiFileFind::MultiFileReplace_IncludeButton()
{
	//選択項目index取得
	AArray<AIndex>	indexArray;
	NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_GetSelectedDBIndexArray(indexArray);
	//選択解除
	NVI_GetListView(kControlID_ReplaceTargetListView).SPI_SetSelect(kIndex_Invalid);
	NVI_GetListView(kControlID_ReplaceExclusionListView).SPI_SetSelect(kIndex_Invalid);
	//除外ファイルリストから選択項目のファイルパスを取得
	ATextArray	pathArray;
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		pathArray.Add(mReplaceExclusionFilePathArray.GetTextConst(indexArray.Get(i)));
	}
	//除外ファイルリストからファイルパスを削除し、対象ファイルリストにファイルパスを追加
	AText	tecname;
	for( AIndex i = 0; i < pathArray.GetItemCount(); i++ )
	{
		AText	path;
		pathArray.Get(i,path);
		//リストから削除
		for( AIndex index = 0; index < mReplaceExclusionFilePathArray.GetItemCount();  )
		{
			if( mReplaceExclusionFilePathArray.GetTextConst(index).Compare(path) == true )
			{
				mReplaceExclusionFilePathArray.Delete1(index);
				mReplaceExclusionFilePathArray_TextEncoding.Get(index,tecname);
				mReplaceExclusionFilePathArray_TextEncoding.Delete1(index);
			}
			else
			{
				index++;
			}
		}
		//
		mReplaceTargetFilePathArray.Add(path);
		mReplaceTargetFilePathArray_TextEncoding.Add(tecname);
	}
	//テーブル更新
	MultiFileReplace_UpdateTargetFileTables();
	//表示更新
	NVM_UpdateControlStatus();
}

/**
置換実行
*/
void	AWindow_MultiFileFind::MultiFileReplace_ExecuteReplace()
{
	//置換総数カウンタ
	AItemCount	totalReplacedCount = 0;
	
	//置換文字列取得
	NVI_GetControlText(kText_ReplaceText,mFindParamerForReplace.replaceText);
	
	//置換実行中フラグON
	mExecutingReplace = true;
	
	//#846
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_MultiFileReplace,true,true,true);
	ABool	aborted = false;
	ATextArray	saveFailFilePathArray;
	try
	{
		
		//置換プログレスウインドウ表示（バックアップ実行中を表示）
		AText	title, message;
		title.SetLocalizedText("EditProgress_Title_MultiFileReplace");
		message.SetLocalizedText("EditProgress_Message_BackupForMultiFileReplace");
		GetApp().SPI_GetEditProgressWindow().SPI_SetText(title,message);
		GetApp().SPI_GetEditProgressWindow().NVI_RefreshWindow();
		//バックアップONならバックアップ実行
		if( NVI_GetControlBool(kCheck_BackupFolder) == true )
		{
			MultiFileReplace_Backup(mFindParamerForReplace.folderPath,mFindParamerForReplace.findText,mFindParamerForReplace.replaceText);
		}
		//置換プログレスウインドウ表示（置換中を表示）
		title.SetLocalizedText("EditProgress_Title_MultiFileReplace");
		message.SetLocalizedText("EditProgress_Message_MultiFileReplace");
		GetApp().SPI_GetEditProgressWindow().SPI_SetText(title,message);
		GetApp().SPI_GetEditProgressWindow().NVI_RefreshWindow();
		
		//置換対象ファイルごとのループ
		AItemCount	fileCount = mReplaceTargetFilePathArray.GetItemCount();
		for( AIndex i = 0; i < fileCount; i++ )
		{
			//セッション継続判断
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_MultiFileReplace,
																	  totalReplacedCount,true,i,fileCount) == false )
			{
				aborted = true;
				break;
			}
			//対象ファイルパス取得
			AText	filepath;
			mReplaceTargetFilePathArray.Get(i,filepath);
			//対象ファイル取得
			AFileAcc	file;
			file.Specify(filepath);
			//対象ファイルが存在し、かつ、行折り返し完了か（＝すでに編集可能か）どうかの判定
			AObjectID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
			ABool	docExistAndWrapCalculated = false;
			if( docID != kObjectID_Invalid )
			{
				switch(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetDocumentInitState())
				{
				  case kDocumentInitState_SyntaxRecognizing:
				  case kDocumentInitState_Completed:
					{
						docExistAndWrapCalculated = true;
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
			}
			if( docExistAndWrapCalculated == false )
			{
				//------------------対象ファイルのドキュメントが存在しない、または、存在するが行折り返し計算完了前場合------------------
				
				//新規テキストドキュメント生成
				ATextDocumentFactory	factory(this,kObjectID_Invalid);
				docID = GetApp().NVI_CreateDocument(factory);
				GetApp().SPI_GetTextDocumentByID(docID).SPI_Init_LocalFile(file,GetEmptyText());
				//マルチファイル置換用のロード実行
				if( GetApp().SPI_GetTextDocumentByID(docID).SPI_LoadForMultiFileReplace() == true )
				{
					//==================ドキュメントファイルロード成功した場合==================
					
					//置換実行
					AItemCount	offset = 0;
					AItemCount	replacedCount = 0;
					AIndex	selectSpos = 0, selectEpos = 0;
					if( GetApp().SPI_GetTextDocumentByID(docID).SPI_ReplaceText(mFindParamerForReplace,0,GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength(),
																				replacedCount,offset,selectSpos,selectEpos) == false )
					{
						aborted = true;
						break;
					}
					//置換トータル個数更新
					totalReplacedCount += replacedCount;
					//保存実行
					if( GetApp().SPI_GetTextDocumentByID(docID).SPI_Save(false) == false )
					{
						//保存失敗の場合リストに追加
						saveFailFilePathArray.Add(filepath);
					}
				}
				else
				{
					//==================ドキュメントファイルロード失敗した場合==================
					//ファイルが削除された、アンマウントされた、ネットワーク切断されたなど
					
					//保存失敗の場合リストに追加
					saveFailFilePathArray.Add(filepath);
				}
				//ドキュメントを閉じる
				GetApp().NVI_DeleteDocument(docID);
			}
			else
			{
				//------------------対象ファイルのドキュメントが存在し、かつ、行折り返し計算完了後の場合------------------
				
				//元々のdirtyかどうかを記憶
				ABool	origDirty = GetApp().SPI_GetTextDocumentByID(docID).NVI_IsDirty();
				//置換実行
				AItemCount	offset = 0;
				AItemCount	replacedCount = 0;
				AIndex	selectSpos = 0, selectEpos = 0;
				if( GetApp().SPI_GetTextDocumentByID(docID).SPI_ReplaceText(mFindParamerForReplace,0,GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextLength(),
																		replacedCount,offset,selectSpos,selectEpos) == false )
				{
					aborted = true;
					break;
				}
				//置換トータル個数更新
				totalReplacedCount += replacedCount;
				//元々dirtyではなかった場合は、保存実行する
				if( origDirty == false )
				{
					if( GetApp().SPI_GetTextDocumentByID(docID).SPI_Save(false) == false )
					{
						//保存失敗の場合リストに追加
						saveFailFilePathArray.Add(filepath);
					}
				}
			}
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	
	//置換実行フラグOFF
	mExecutingReplace = false;
	
	//statusコマンド実行（マルチファイル置換実行中はstatusコマンド実行抑制しているので、ここでまとめて実行）
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
	{
		AFileAcc	folder;
		folder.Specify(mFindParamerForReplace.folderPath);
		GetApp().SPI_GetSCMFolderManager().UpdateFolderStatus(folder);
	}
	
	if( aborted == false )
	{
		//------------------置換途中中断なしの場合------------------
		//置換結果設定
		if( saveFailFilePathArray.GetItemCount() > 0 )
		{
			//------------------保存失敗ダイアログ表示------------------
			//保存失敗テキスト
			AText	message1;
			AText	message2;
			message1.SetLocalizedText("MultiFileReplace_FailSave");
			message2.SetLocalizedText("MultiFileReplace_FailSaveMessage");
			//失敗ファイル名テキストを設定
			AText	t;
			saveFailFilePathArray.Implode(kUChar_LineEnd,t);
			message2.AddText(t);
			//結果ダイアログ表示
			NVI_ShowChildWindow_OK(message1,message2,true);
		}
		else
		{
			//------------------置換OKダイアログ表示------------------
			AText	message1;
			AText	message2;
			message1.SetLocalizedText("MultiFileReplace_Result");
			AText	text;
			text.SetFormattedCstring("%d",totalReplacedCount);
			message1.ReplaceParamText('0',text);
			//結果ダイアログ表示
			NVI_ShowChildWindow_OK(message1,message2);
		}
		
		//置換データ全削除
		MultiFileReplace_ClearReplaceData();
	}
	else
	{
		//------------------置換途中中断の場合------------------
		//中断ダイアログ表示
		AText	message1;
		message1.SetLocalizedText("MultiFileReplace_Aborted");
		NVI_ShowChildWindow_OK(message1,GetEmptyText());
	}
}

/**
置換前バックアップ
*/
void	AWindow_MultiFileFind::MultiFileReplace_Backup( const AFileAcc& inSrcBaseFolder, const AText& inFindText, const AText& inReplaceText )
{
	//バックアップルートフォルダ取得
	AText	backupRootFolderPath;
	NVI_GetControlText(kText_BackupFolder,backupRootFolderPath);
	AFileAcc	backupRootFolder;
	backupRootFolder.Specify(backupRootFolderPath);
	
	//バックアップルートフォルダ生成
	backupRootFolder.CreateFolder();
	if( backupRootFolderPath.GetItemCount() == 0 || backupRootFolder.Exist() == false || backupRootFolder.IsFolder() == false )
	{
		//バックアップルートフォルダが生成出来なかった場合
		//デフォルトのバックアップルートフォルダ取得
		GetApp().SPI_GetDefaultMultiFileReplaceBackupFolder(backupRootFolder);
		backupRootFolder.GetPath(backupRootFolderPath);
		NVI_SetControlText(kText_BackupFolder,backupRootFolderPath);
	}
	//バックアップフォルダ名取得
	AText	foldername("MultiFileReplaceBackup_20");
	AText	t;
	ADateTimeWrapper::Get6LettersDateText(t);
	foldername.AddText(t);
	ADateTimeWrapper::Get2LettersHourText(t);
	foldername.AddText(t);
	ADateTimeWrapper::Get2LettersMinuteText(t);
	foldername.AddText(t);
	ADateTimeWrapper::Get2LettersSecondText(t);
	foldername.AddText(t);
	//バックアップフォルダ取得
	AFileAcc	backupFolder;
	backupFolder.SpecifyUniqChildFile(backupRootFolder,foldername);
	backupFolder.CreateFolder();
	//ログテキスト生成
	AText	datetext;
	ADateTimeWrapper::GetDateTimeText(datetext);
	AText	log;
	log.AddCstring("Multi File Replace Backup\nDate: ");
	log.AddText(datetext);
	log.AddCstring("\nFolder: ");
	AText	folderpath;
	inSrcBaseFolder.GetPath(folderpath);
	log.AddText(folderpath);
	log.AddCstring("\nFind Text: ");
	log.AddText(inFindText);
	log.AddCstring("\nReplace Text: ");
	log.AddText(inReplaceText);
	log.AddCstring("\n");
	AFileAcc	logfile;
	logfile.SpecifyChild(backupFolder,"log.txt");
	logfile.CreateFile();
	logfile.WriteFile(log);
	//対象ファイル毎のループ
	for( AIndex i = 0; i < mReplaceTargetFilePathArray.GetItemCount(); i++ )
	{
		//対象ファイルのファイルパス取得
		AText	filepath;
		mReplaceTargetFilePathArray.Get(i,filepath);
		//対象ファイル取得
		AFileAcc	srcfile;
		srcfile.Specify(filepath);
		//相対パス取得
		AText	relativePath;
		srcfile.GetPartialPath(inSrcBaseFolder,relativePath);
		//バックアップ先ファイル取得
		AFileAcc	dstfile;
		dstfile.SpecifyChild(backupFolder,relativePath);
		//ファイルコピー
		srcfile.CopyFileOrFolderTo(dstfile,false,false);
	}
}

/**
プログレステキスト設定
*/
void	AWindow_MultiFileFind::SPI_MultiFileReplace_SetProgressText( const AText& inText )
{
	if( NVI_IsWindowVisible() == false )   return;
	NVI_SetControlText(kProgressText,inText);
}

/**
プログレス設定
*/
void	AWindow_MultiFileFind::SPI_MultiFileReplace_Progress( const short inPercent )
{
	if( NVI_IsWindowVisible() == false )   return;
#if IMPLEMENTATION_FOR_MACOSX
	NVI_SetProgressIndicatorProgress(0,inPercent);
#else
	NVI_SetControlNumber(kProgressBar,inPercent);
	#endif
}


#pragma mark ===========================

#pragma mark <内部モジュール>

#pragma mark ===========================

//最近使った検索テキストメニューの更新
void	AWindow_MultiFileFind::UpdateRecentlyUsedFindTextMenu()
{
	ATextArray	textArray;
	textArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFindString));
	AText	text;
	text.SetLocalizedText("MultiFileFind_RecentlyUsedFindString");
	textArray.Insert1(0,text);
	text.SetLocalizedText("RecentlyMenus_DeleteAll");
	textArray.Add(text);
	NVI_SetMenuItemsFromTextArray(kPopup_RecentlyUsedFindText,textArray);
}

//最近使った検索テキストの追加
void	AWindow_MultiFileFind::SPI_AddRecentlyUsedFindText( const AText& inText )
{
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )   return;//#352
	
	GetApp().GetAppPref().AddRecentlyUsed_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFindString,inText,128);
	UpdateRecentlyUsedFindTextMenu();
}

//最近使ったファイルフィルタの更新
void	AWindow_MultiFileFind::UpdateRecentlyUsedFileFilterMenu()
{
	ATextArray	textArray;
	textArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter));
	AText	text;
	text.SetLocalizedText("MultiFileFind_RecentlyUsedFileFilter");
	textArray.Insert1(0,text);
	text.SetLocalizedText("RecentlyMenus_DeleteAll");
	textArray.Add(text);
	NVI_SetMenuItemsFromTextArray(kPopup_RecentlyUsedFileFilter,textArray);
}

//最近使ったファイルフィルタの追加
void	AWindow_MultiFileFind::SPI_AddRecentlyUsedFileFilter( const AText& inText )
{
	GetApp().GetAppPref().AddRecentlyUsed_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFileFilter,inText,128);
	UpdateRecentlyUsedFileFilterMenu();
}

//最近使ったファイルパスの更新
void	AWindow_MultiFileFind::UpdateRecentlyUsedPathMenu()
{
	ATextArray	textArray;
	textArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder));
	AText	text;
	text.SetLocalizedText("MultiFileFind_RecentlyUsedUsedFolder");
	textArray.Insert1(0,text);
	text.SetLocalizedText("RecentlyMenus_DeleteAll");
	textArray.Add(text);
	NVI_SetMenuItemsFromTextArray(kPopup_RecentlyUsedPath,textArray);
}

//最近使ったファイルフィルタの追加
void	AWindow_MultiFileFind::SPI_AddRecentlyUsedPath( const AFileAcc& inFolder )
{
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )   return;//#352
	
	AText	pathtext;
	//B0389 OK inFolder.GetPath(pathtext,kFilePathType_1);
	GetApp().GetAppPref().GetFilePathForDisplay(inFolder,pathtext);//B0389
	GetApp().GetAppPref().AddRecentlyUsed_TextArray(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder,pathtext,128);
	UpdateRecentlyUsedPathMenu();
}

