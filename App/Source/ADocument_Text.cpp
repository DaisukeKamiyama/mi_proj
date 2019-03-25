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

ADocument_Text

*/

#include "stdafx.h"

#include "ADocument_Text.h"
#include "ADocument_IndexWindow.h"
#include "AApp.h"
#include "ASyntaxDefinition.h"
#include "AView_Text.h"
//#858 #include "AView_List_FusenList.h"
#include "AView_LineNumber.h"
#include "AThread_SyntaxRecognizer.h"
#include "AWindow_Notification.h"
#include "AView_Notification.h"
#include "AThread_DocumentFileScreening.h"

#pragma mark ===========================
#pragma mark [クラス]ADocument_Text 
#pragma mark ===========================
//テキストドキュメントのクラス
#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
ADocument_Text::ADocument_Text( AObjectArrayItem* inParent, const AObjectID inDocImpID ) : ADocument(inParent,inDocImpID),//#1034
		/*#379 未使用のためコメントアウト mInitDone(false),*/ mUndoer(*this), 
		/*#361 mFTPMode(false),*/ mTextInfo(this), mTextInfoForPrint(this), mBookmarkLineIndex(kIndex_Invalid), //#182NVM_GetPrintImp()(GetObjectID()),
#if IMPLEMENTATION_FOR_MACOSX
		/*#1034 mASImp(&(GetApp().SPI_GetASImp()),GetObjectID()),*/ mCreator(NULL), mType(NULL), 
#endif
		/*#698 未使用のため削除mSyntaxDirty(false), */mImportFileDataDirty(false),
		mFindHighlightSet(false), 
		//#893 mCurrentLocalIdentifierStartLineIndex(kIndex_Invalid), mCurrentLocalIdentifierEndLineIndex(kIndex_Invalid),
		mDiffDisplayWaitFlag(false), //#379 mDiffDisplayMode(kDiffDisplayMode_None),//R0006
		mDrawArrowToDef(false),
		//win 080729
		mPrintLineNumberWidthMM(14.0)/*40(pix)/72(dpi)*25.4=14.111...*/,
		mPrintLineNumberRightMarginMM(2.5),
		mPrintFileNameHeightMM(10.0),
		mPrintSeparateLineHeightMM(1.0)
		,mProjectObjectID(kObjectID_Invalid)//RC3
		,mFindHighlightSet2(false)//R0244
		,mCreatedAutomatically(false)//B0441
		,mDisableInlineInputHilite(false)//#187
		,mOriginalFontSize(9.0)//#354
		,mLastSavedUndoPosition(kIndex_Invalid)//#267
		,mLastSelectedJumpListItemIndex(kIndex_Invalid)//#454
		,mShouldShowCorrectEncodingWindow(false)//#379
		,mDiffTargetMode(kDiffTargetMode_None)//#379
		,mDiffWorkingDocumentID(kObjectID_Invalid)//#379
		,mDiffTargetFileDocumentID(kObjectID_Invalid)//#379
		,mRepositoryDocumentID(kObjectID_Invalid)//#379
//#361 mTextDocumentTypeを使う,mIsDiffTargetDocument(false)//#379
		,mPrintStartParagraphIndex(kIndex_Invalid),mPrintEndParagraphIndex(kIndex_Invalid)//#524
//#0 未使用のため削除		,mDiffInited(true)//#379
//#699,mViewDataInited(false)//#379
		,mIsOwnedDocument(false)//#379
		,mShouldShowCorrectEncodingWindow_FallbackType(kTextEncodingFallbackType_None)//#473
		,mLuaConsoleMode(false),mLuaPromptTextIndex(kIndex_Invalid)//#567
		,mSCMFileState(kSCMFileState_NotSCMFolder)//#568
,mLastModifiedDateTime(0)//#653
,mSyntaxRecognizer_ThreadRunningFlag(false),mSyntaxRecognizer_ThreadUnpausedFlag(false)//#698
,mSyntaxRecognizer_RestartFlag(false)//#698
,mSyntaxRecognizer_StartLineIndex(kIndex_Invalid),mSyntaxRecognizer_EndLineIndex(kIndex_Invalid)//#698
,mSyntaxRecognizerThreadID(kObjectID_Invalid)//#698
,mWrapCalculator_ThreadUnpausedFlag(false),mWrapCalculator_CurrentLineIndex(kIndex_Invalid)//#699
,mWrapCalculatorThreadID(kObjectID_Invalid)//#699
,mCacheForGetJumpMenuItemIndexByLineIndex(kIndex_Invalid)//#&95
,mDocumentInitState(kDocumentInitState_Initial)//#699
,mWrapCalculatorWorkingByInsertText(false)//#699
,mWrapCalculatorProgressTotalTextLength(kIndex_Invalid),mWrapCalculatorProgressRemainedTextLength(0)//#699
,mRemoteConnectionObjectID(kObjectID_Invalid)//#361
,mTextDocumentType(kTextDocumentType_Normal)//#361
,mTextEncodingIsDirectedByInitArgument(false)//#830
,mLoaded(false)//#831
,mCurrentWordFindHighlightSet(false)//#750
,mSuspectBinaryData(false)//#703
,mAutoSaveDirty(false)
,mDiffTextRangeStartParagraph(0),mDiffTextRangeEndParagraph(0),mDiffTextRangeStart(kIndex_Invalid),mDiffTextRangeEnd(kIndex_Invalid)//#192
,mCompareMode(kCompareMode_NoCompare)//#81
,mPreviewCurrentLineIndex(kIndex_Invalid)//#734
,mGuessedIndnetCharacterType(kIndentCharacterType_Unknown)//#912
,mForMultiFileReplace(false)
,mCalculateWrapInThread(true)
,mFileScreenError(kFileScreenError_NoError)
,mDuplicatedDocumentID(kObjectID_Invalid)
,mTemporaryFontSize(0)//#966
,mPrintRect(kLocalRect_0000),mPrintPaperWidth(0),mPrintPaperHeight(0)//#558
,mIsReadMeFile(false)//#1351
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::ADocument_Text() started.",this);
	
	//初期化
	//#764 mTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
	
	//mWindow_SimpleCVSCommit.Setup(*this);
	//mWindow_SimpleCVSAdd.Setup(*this);
	//mPropertyWindow.Setup(this);
	mJumpMenuObjectID = GetApp().NVI_GetMenuManager().CreateDynamicMenu(kMenuItemID_Jump,true);
	
#if IMPLEMENTATION_FOR_MACOSX
	mODBMode = false;
#endif
	
	//#81
	//未保存データ保存フォルダ生成
	CreateUnsavedDataBackupFolder();
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::ADocument_Text() ended.",this);
}

//デストラクタ
ADocument_Text::~ADocument_Text()
{
	//他スレッドでのmTextを参照が完了後にオブジェクト削除することを保証するため。
	//（ADocument_TextがAAppに登録中にスレッドから参照され、mTextを参照中に、ADocument_TextがDeleteされた場合のため。他スレッド参照が終了するまでLock()はブロックする。）
	mTextMutex.Lock();
	mTextMutex.Unlock();
#if IMPLEMENTATION_FOR_MACOSX
	if( mODBMode == true )
	{
		ALaunchWrapper::SendODBClosed(mODBSender,mODBToken,mODBFile);
	}
#endif
	
	//#361
	//リモート接続オブジェクト削除
	if( mRemoteConnectionObjectID != kObjectID_Invalid )
	{
		GetApp().SPI_DeleteRemoteConnection(mRemoteConnectionObjectID);
		mRemoteConnectionObjectID = kObjectID_Invalid;
	}
	
	//#1229
	//ロード時、および、最後のセーブ時のファイルのコピーを保存するファイルを削除
	AFileAcc	lastLoadOrSaveFile;
	GetLastLoadOrSaveCopyFile(lastLoadOrSaveFile);
	lastLoadOrSaveFile.DeleteFile();
}

//#379
/**
Document削除時処理
*/
void	ADocument_Text::NVIDO_DocumentWillBeDeleted()
{
	//#890
	//インポートファイルデータを全削除
	SPI_DeleteAllImportFileData();
	//#81
	//diff用テンポラリフォルダを削除
	if( mDiffTempFolder.IsSpecified() == true )
	{
		mDiffTempFolder.DeleteFileOrFolderRecursive();
	}
	//#81
	//未保存データ保存フォルダ削除
	DeleteUnsavedDataBackupFolder();
	
	//Working側にDiff対象ドキュメントが閉じられたことを通知
	if( mDiffWorkingDocumentID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextDocumentByID(mDiffWorkingDocumentID).SPI_DoDiffTargetDocumentClosed(GetObjectID());
	}
	//Diff対象側にWorkingドキュメントが閉じられたことを通知
	if( mDiffTargetFileDocumentID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextDocumentByID(mDiffTargetFileDocumentID).SPI_DoWorkingDocumentClosed(GetObjectID());
	}
	if( mRepositoryDocumentID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_DoWorkingDocumentClosed(GetObjectID());
	}
	//文法認識スレッド削除#698
	if( mSyntaxRecognizerThreadID != kObjectID_Invalid )
	{
		//スレッド削除（スレッド終了したらNVI_DeleteThread()から戻ってくる。）
		GetApp().NVI_DeleteThread(mSyntaxRecognizerThreadID);
		mSyntaxRecognizerThreadID = kObjectID_Invalid;
	}
	//行計算スレッド削除#699
	if( mWrapCalculatorThreadID != kObjectID_Invalid )
	{
		//スレッド削除（スレッド終了したらNVI_DeleteThread()から戻ってくる。）
		GetApp().NVI_DeleteThread(mWrapCalculatorThreadID);
		mWrapCalculatorThreadID = kObjectID_Invalid;
	}
	//#890
	//テキスト全削除
	{
		AStMutexLocker	locker(mTextMutex);
		mText.DeleteAll();
	}
	//#896
	//ドキュメントが閉じられたことを通知（プロジェクトのドキュメントを全て閉じた時にインポート識別子データをpurgeするなどのため）
	if( NVI_IsFileSpecified() == true )
	{
		AFileAcc	file;
		NVI_GetFile(file);
		GetApp().SPI_DoTextDocumentClosed(file,mProjectFolder);
	}
	/*
	//
	AText	t;
	NVI_GetTitle(t);
	t.AddCstring(" closed.\r");
	t.OutputToStderr();
	*/
}

#pragma mark ===========================

#pragma mark ---初期設定
//ADocument_Text生成後、以下のいずれかのSetupがコールされる

//#361
/**
アクセスプラグイン
@param inDocumentText テキストエンコーディング未返還のリモートテキスト
*/
void	ADocument_Text::SPI_Init_RemoteFile( const AObjectID inAccessPuginObjectID, 
		const AText& inFileURL, const AText& inDocumentText )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_RemoteFile() started.",this);
	
	//ドキュメントタイプ設定
	mTextDocumentType = kTextDocumentType_RemoteFile;
	
	//アクセスプラグインデータ設定
	mRemoteConnectionObjectID = inAccessPuginObjectID;
	mRemoteFileURL.SetText(inFileURL);
	
	//=======================
	// ドキュメント設定
	//=======================
	
	//#241 DocPref読み込み（mRemoteFileURLに従ったデータ保存フォルダからdoc prefを読みこむ）
	//#898 mDocPrefDB.LoadForRemoteFile(inFileURL);
	AModeIndex	modeIndex = LoadDocPref();//#898
	
	//mModeIndexを設定
	SetModeIndex(modeIndex);//#890 #898
	
	/*#898
	//ModeIndex決定（ファイル名から決定）
	SetModeIndex(GetApp().SPI_FindModeIDByTextFilePath(inFileURL));//R0210 #890
	AText	text;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeName(text);
	GetDocPrefDB().SetData_Text(ADocPrefDB::kModeName,text);
	*/
	
	//=======================
	// テキスト設定
	//=======================
	//テキスト設定
	{
		AStMutexLocker	locker(mTextMutex);//#890
		mText.SetText(inDocumentText);
	}
	//サーバー上テキストを記憶
	mTextOnRemoteServer.SetText(inDocumentText);
	
	//初期設定共通処理
	Init_Common();
	
	//#81
	//自動バックアップファイルリスト更新
	SPI_UpdateAutoBackupArray();
}

/*#361
//FTPドキュメントSetup
void	ADocument_Text::SPI_Init_FTP( const AText& inFTPURL, const AText& inDocumentText )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_FTP() started.",this);
	//=======================
	// ドキュメント設定
	//=======================
	
	//#241 DocPref読み込み
	mDocPrefDB.LoadForRemoteFile(inFTPURL);
	
	//ModeIndex決定（ファイル名から決定）
	mModeIndex = GetApp().SPI_FindModeIDByTextFilePath(inFTPURL);//R0210
	AText	text;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeName(text);
	GetDocPrefDB().SetData_Text(ADocPrefDB::kModeName,text);
	
	//=======================
	// FTPデータ設定
	//=======================
	mFTPMode = true;
	mFTPURL.SetText(inFTPURL);
	mFTPTextOnServer.SetText(inDocumentText);
	
	//=======================
	// テキスト設定
	//=======================
	mText.SetText(inDocumentText);
	
	Init_Common();
}
*/

//ローカルファイルドキュメントSetup
void	ADocument_Text::SPI_Init_LocalFile( const AFileAcc& inFileAcc, const AText& inTextEncodingName )//win 080715 #212
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_LocalFile() started.",this);
	
	//#361
	//ドキュメントタイプ設定
	mTextDocumentType = kTextDocumentType_Normal;
	
	//ファイル存在チェック
	//#932 ファイル存在チェックはSPI_ScreenDocumentFile()で実行 if( inFileAcc.Exist() == false ) _ACThrow("file not exist",this);
	/*#932 クリエータタイプ読み込みはSPI_ScreenDocumentFile()で実行。ttroは対応しない。
#if IMPLEMENTATION_FOR_MACOSX
	//クリエータ／タイプ読み込み
	inFileAcc.GetCreatorType(mCreator,mType);
	
	if( mType == 'ttro' )
	{
		NVI_SetReadOnly(true);
	}
#endif
	*/
	//ファイル設定（未スクリーニングとして設定）
	//#932 NVI_SpecifyFile(inFileAcc);
	NVI_SpecifyFileUnscreened(inFileAcc);
	
	//=======================
	// ドキュメント設定
	//=======================
	
	//DocPref読み込み
	/*#898
	APrintPagePrefData	printpref;
	mDocPrefDB.Load(inFileAcc,printpref);
	NVM_GetPrintImp().SetPrintPagePrefData(printpref);
	*/
	AModeIndex	modeIndex = LoadDocPref();//#898
	
	//mModeIndexを設定
	SetModeIndex(modeIndex);//#890 #898
	
	//inTextEncodingNameが指定されている場合（AppleScript経由によるオープン等）はそちらを優先させる
	if( inTextEncodingName.GetLength() > 0 )
	{
		//DocPrefにTextEncoding設定
		//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
		GetDocPrefDB().SetTextEncoding(inTextEncodingName);//#1040
		//テキストエンコーディング指定フラグON
		mTextEncodingIsDirectedByInitArgument = true;//#830
	}
	
	Init_Common();//#212
}

//新規ファイルSetup
void	ADocument_Text::SPI_Init_NewFile( const AModeIndex inModeIndex, const AText& inTextEncodingName, const AText& inDocumentText )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_NewFile() started.",this);
	
	//#361
	//ドキュメントタイプ設定
	mTextDocumentType = kTextDocumentType_Normal;
	
	//仮タイトル設定
	AText	title;
	title.SetLocalizedText("UntitledDocumentName");
	ANumber	num = GetApp().SPI_GetAndIncrementUntitledDocumentNumber();
	if( num >= 2 )
	{
		title.Add(kUChar_Space);//B0000 わかちがき
		title.AddFormattedCstring("%d",num);
	}
	SPI_SetTemporaryTitle(title);
	
	AText	text;
	
	//=======================
	// ドキュメント設定
	//=======================
	
	//mModeIndex設定
	SetModeIndex(inModeIndex);//#890
	
	//DocPref初期化 #898
	InitDocPref(inModeIndex);
	
	/*#898
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeName(text);
	GetDocPrefDB().SetData_Text(ADocPrefDB::kModeName,text);
	
	//DocPrefにModePrefからコピーする
	mDocPrefDB.CopyFromModePrefDBIfStillDefault();
	*/
	
	if( inTextEncodingName.GetLength() > 0 ) 
	{
		//DocPrefにTextEncoding設定
		//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
		GetDocPrefDB().SetTextEncoding(inTextEncodingName);//#1040
		//テキストエンコーディング指定フラグON
		mTextEncodingIsDirectedByInitArgument = true;//#830
	}
	
#if IMPLEMENTATION_FOR_MACOSX
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSetDefaultCreator) == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kDefaultCreator,text);
		mCreator = text.GetOSTypeFromText();//AUtil::GetOSTypeFromAText(text);
		mType = 'TEXT';
	}
#endif
	
	//=======================
	// テキスト設定
	//=======================
	//テキスト設定
	{
		AStMutexLocker	locker(mTextMutex);//#890
		mText.SetText(inDocumentText);
	}
	
	Init_Common();
}

//#379
/**
Diff対象ドキュメントを生成（テキストから生成）
*/
void	ADocument_Text::SPI_Init_DiffTargetDocument( const AText& inTitle, const AText& inDocumentText,
		const AModeIndex inModeIndex, const AText& inTextEncodingName )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_DiffTargetDocument() started.",this);
	
	//#361
	//ドキュメントタイプ設定
	mTextDocumentType = kTextDocumentType_DiffTarget;
	
	//ReadOnly
	NVI_SetReadOnly(true);
	
	//タイトル設定
	SPI_SetTemporaryTitle(inTitle);
	
	//=======================
	// ドキュメント設定
	//=======================
	
	//mModeIndex設定
	SetModeIndex(inModeIndex);//#890
	
	//DocPref初期化 #898
	InitDocPref(inModeIndex);
	
	//TextEncoding
	if( inTextEncodingName.GetLength() > 0 ) 
	{
		//DocPrefにTextEncoding設定
		//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
		GetDocPrefDB().SetTextEncoding(inTextEncodingName);//#1040
		//テキストエンコーディング指定フラグON
		mTextEncodingIsDirectedByInitArgument = true;//#830
	}
	
	//テキスト設定
	{
		AStMutexLocker	locker(mTextMutex);//#890
		mText.SetText(inDocumentText);
	}
	
	//共通設定
	Init_Common();
}
/**
Diff対象ドキュメントを生成（ファイルから生成）
*/
void	ADocument_Text::SPI_Init_DiffTargetDocument( const AText& inTitle, const AFileAcc& inFile,
													const AModeIndex inModeIndex, const AText& inTextEncodingName )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::SPI_Init_DiffTargetDocument() started.",this);
	
	//#361
	//ドキュメントタイプ設定
	mTextDocumentType = kTextDocumentType_DiffTarget;
	
	//ReadOnly
	NVI_SetReadOnly(true);
	
	//タイトル設定
	SPI_SetTemporaryTitle(inTitle);
	
	//ファイル設定
	mFileForDiffTargetDocument = inFile;
	
	//=======================
	// ドキュメント設定
	//=======================
	
	//DocPref読み込み
	AModeIndex	modeIndex = LoadDocPref();//#898
	
	//mModeIndex設定
	SetModeIndex(inModeIndex);//#890
	
	//inTextEncodingNameが指定されている場合（AppleScript経由によるオープン等）はそちらを優先させる
	if( inTextEncodingName.GetLength() > 0 )
	{
		//DocPrefにTextEncoding設定
		//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
		GetDocPrefDB().SetTextEncoding(inTextEncodingName);//#1040
		//テキストエンコーディング指定フラグON
		mTextEncodingIsDirectedByInitArgument = true;//#830
	}
	
	//共通設定
	Init_Common();
	
	//ファイル読み込み
	SPI_ScreenAndLoadDocumentFile();
}

//Init共通部分
void	ADocument_Text::Init_Common()//#212
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::Init_Common() started.",this);
	
	/*#898
	//ここにきたときに、まだDocPrefで指定されていないデータはModePrefからコピーする
	GetDocPrefDB().CopyFromModePrefDBIfStillDefault();
	*/
	
	/*#699
	//行情報計算（ウインドウ生成前にとりあえずkWrapMode_NoWrapで行情報計算する）
	mTextInfo.CalcLineInfoAllWithoutView(mText);//win
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("Document text info calculated.",this);
	*/
#if IMPLEMENTATION_FOR_MACOSX
	//クリエータ／タイプ強制変更
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kForceMiTextFile) == true )
	{
		mCreator = 'MMKE';
		mType = 'TEXT';
	}
#endif
	
	//#354
	//ドキュメント生成時のフォントを記憶
	SPI_GetFontName(mOriginalFontName);
	mOriginalFontSize = SPI_GetFontSize();
	
	//#379
	//Diff target documentの場合の行折り返し設定
	if( /*#361 mIsDiffTargetDocument*/SPI_IsDiffTargetDocument() == true && 
	   GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowDiffTargetWithWordWrap) == true )
	{
		GetDocPrefDB().SetData_Number(ADocPrefDB::kWrapMode,kWrapMode_WordWrap);
	}
	
	//行計算スレッド初期化#699
	GetWrapCalculatorThread().SPI_Init(GetObjectID());
	
	//文法認識スレッド初期化#698
	GetSyntaxRecognizerThread().SPI_Init(GetObjectID());
	
	//他のオブジェクトとの整合性のため、まず一行追加しておく#699
	mTextInfo.AddLineInfo(0,mText.GetItemCount(),false);
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::Init_Common() ended.",this);
}

/*
【通常ロード時】
・viewが最初にactivateになった時に、SPI_DoViewActivating()経由で、StartScreeningForLoadDocumentFile()がコールされる
・StartScreeningForLoadDocumentFile()から、スクリーニングスレッドを起動する
・スクリーニングスレッドからSPI_ScreenDocumentFile()を実行する。完了後、メインスレッドにイベント通知される。
・メインスレッドのEVTDO_DoInternalEvent()から、SPI_TransitInitState_ViewActivated()がコールされる。
・SPI_TransitInitState_ViewActivated()で、ロード、行折り返し計算、状態繊維を実行。

【diff targetドキュメント（ファイル）ロード時、および、マルチファイル置換時】
・SPI_ScreenAndLoadDocumentFile()がコールされる。
・SPI_ScreenAndLoadDocumentFile()内で、スクリーニング、ロードを実行する。
・diff taretドキュメントの場合：その後SPI_DoViewActivating()がコールされたら、
 直接SPI_TransitInitState_ViewActivated()をコールする。
・マルチファイル置換の場合：viewはactivateされない。
*/

/**
@return 読み込み成功したらtrue
@note diff targetドキュメントロード時、および、マルチファイル置換時にコールされる。スクリーニングとロードを同期実行する。
*/
ABool	ADocument_Text::SPI_ScreenAndLoadDocumentFile()
{
	//==================スクリーニング実行（同期実行）==================
	SPI_ScreenDocumentFile();
	
	//==================ロード実行==================
	LoadDocumentFile();
	
	//スクリーニングエラー無しならtrueを返す
	if( mFileScreenError == kFileScreenError_NoError )
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
ドキュメントファイルスクリーニングスレッド実行開始する。
@note 通常ロード時に、SPI_DoViewActivating()からコールされる。
サブスレッドからSPI_ScreenDocumentFile()を実行後、メインスレッドに完了通知を送信する。
完了通知により、SPI_TransitInitState_ViewActivated()がコールされ、状態繊維（ロード・行折り返し計算）が実行される。
*/
void	ADocument_Text::StartScreeningForLoadDocumentFile()
{
	//==================ドキュメント初期状態遷移==================
	//スレッドでのスクリーン中
	SPI_SetDocumentInitState(kDocumentInitState_Initial_FileScreeningInThread);
	
	//==================スレッド起動==================
	AThread_DocumentFileScreeningFactory	factory(NULL);
	AObjectID	threadObjectID = GetApp().NVI_CreateThread(factory);
	(dynamic_cast<AThread_DocumentFileScreening&>(GetApp().NVI_GetThreadByID(threadObjectID))).SPNVI_Run(GetObjectID());
	//最大3秒ウエイト（その前にスクリーニングスレッドが終了したら、スリープ解除される）
	for( AIndex i = 0; i < 3; i++ )
	{
		if( GetApp().NVI_GetThreadByID(threadObjectID).NVI_IsThreadWorking() == false )   break;
		GetApp().NVI_SleepWithTimer(1);
	}
	//スクリーニングスレッド完了していて、エラーが無ければ、状態遷移をここで行う。（これにより、何も表示されていない状態が一瞬表示されるのを防ぐ）
	//エラーがある場合は、ドキュメントが閉じられてしまい、SPI_DoViewActivating()の後の処理に影響があるので、状態遷移は処理しない。（次回EVT_DoInternalEvent()で処理）
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		if( mFileScreenError == kFileScreenError_NoError )
		{
			SPI_TransitInitState_ViewActivated();
		}
	}
}

/**
ドキュメントファイルのスクリーニングを実行する
通常ロード時：サブスレッドから非同期実行される
diff targetドキュメントロード時、および、マルチファイル置換時：メインスレッドから同期実行される
*/
void	ADocument_Text::SPI_ScreenDocumentFile()
{
	//==================ドキュメント初期状態遷移==================
	//スクリーン済み
	SPI_SetDocumentInitState(kDocumentInitState_Initial_FileScreened);
	
	//==================ファイル未specify（新規ドキュメント等）なら何もせず終了==================
	if( mFileAcc.IsSpecified() == false )
	{
		return;
	}
	//==================エイリアス解決==================
	AFileAcc	file;
	file.ResolveAnyLink(mFileAcc);
	//mFileAccのファイルが非存在のときはfileがspecifyされないので、ここでファイル非存在エラーで終了。#962
	if( file.IsSpecified() == false )
	{
		mFileScreenError = kFileScreenError_FileNotExist;
		return;
	}
	//エイリアスだったかどうかを判定
	if( file.Compare(mFileAcc) == false )
	{
		//エイリアスだった場合、エイリアス解決後のファイルがすでに開かれていないかどうかを判定
		mDuplicatedDocumentID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
		if( mDuplicatedDocumentID != kObjectID_Invalid )
		{
			//すでに同じファイルが開かれているときは、エラー。
			mFileScreenError = kFileScreenError_DocumentDuplicatedAfterLinkResolved;
			return;
		}
		//エイリアス解決後のファイルを設定
		NVI_SpecifyFileUnscreened(file);
	}
	
	//==================ファイル存在チェック==================
	if( file.Exist() == false )
	{
		mFileScreenError = kFileScreenError_FileNotExist;
		return;
	}
	
	//==================フォルダでないことのチェック==================
	if( file.IsFolder() == true )
	{
		mFileScreenError = kFileScreenError_FileIsFolder;
		return;
	}
	
	/*#1116 ファイルサイズ制限解除
	//==================ファイルサイズチェック==================
	//ファイルサイズが100MBを超えるファイルの場合は、モーダルエラー表示して、ファイルを開かない
	AByteCount	filesize = file.GetFileSize();
	if( filesize > kLimit_OpenTextFileSize )
	{
		mFileScreenError = kFileScreenError_FileSizeOver;
		return;
	}
	//トータルのファイルサイズが200MBを超える場合は、モーダルエラー表示して、ファイルを開かない
	if( GetApp().SPI_GetTotalTextDocumentMemoryByteCount() + filesize > kLimit_TotalOpenTextFileSize )
	{
		mFileScreenError = kFileScreenError_TotalFileSizeOver;
		return;
	}
	*/
	//==================クリエータ／タイプ読み込み==================
#if IMPLEMENTATION_FOR_MACOSX
	//クリエータ／タイプ読み込み
	file.GetCreatorType(mCreator,mType);
#endif
}

//#831
/**
ロード（SPI_TransitInitState_ViewActivated()からコールされる。新規ドキュメントの場合も含み、全てのドキュメントタイプでコールされる）
*/
void	ADocument_Text::LoadDocumentFile()
{
	//ファイルをスクリーン済みとして設定
	if( NVI_IsFileSpecified() == true )
	{
		AFileAcc	file;
		NVI_GetFile(file);
		NVI_SpecifyFile(file);
	}
	
	//==================TextInfoにモード設定==================
	//TextInfoへモード設定
	mTextInfo.SetMode(mModeIndex);
	
	//==================ファイル読み込み==================
	//ファイル読み込みするかどうかのフラグ
	ABool	shouldLoadFile = false;
	AFileAcc	loadFile;
	//ファイル指定されている場合はそのファイルを読み込むことにする
	if( NVI_IsFileSpecified() == true )
	{
		//ファイル取得
		NVI_GetFile(loadFile);
		shouldLoadFile = true;
	}
	//diffターゲットドキュメントの場合で、ファイル指定されている場合は、そのファイルを読み込むことにする
	else if( mTextDocumentType == kTextDocumentType_DiffTarget && mFileForDiffTargetDocument.IsSpecified() == true )
	{
		//ファイル取得
		loadFile = mFileForDiffTargetDocument;
		shouldLoadFile = true;
	}
	//ファイル読み込みするかどうかのフラグがONのときは、ファイルを読み込む
	if( shouldLoadFile == true )
	{
		//ファイル読み込み
		{
			AStMutexLocker	locker(mTextMutex);//#890
			loadFile.ReadTo(mText);
		}
		
		//最終編集日時記憶
		loadFile.GetLastModifiedDataTime(mLastModifiedDateTime);
		//#653 読み込み時のファイルデータを記憶（テキストエンコーディング未変換）他アプリでの編集検知に使用
		//#693 mLastReadOrWriteFileRawData.SetText(mText);
		//読み込み時のファイルデータをファイルとして記憶する。
		AFileAcc	lastLoadOrSaveFile;
		GetLastLoadOrSaveCopyFile(lastLoadOrSaveFile);
		lastLoadOrSaveFile.CreateFile();
		lastLoadOrSaveFile.WriteFile(mText);
	}
	
	/*例外を作ると分かりづらくなるので強制的にNoWrapにする処理は削除。（行折り返しは、ドキュメント設定があればそれに従い、無ければモード設定（「編集」タブ）に従う。サイズが大きくても。折り返し設定はスレッド化しているので、折り返し計算に時間がかかる場合は、行折り返し無しに途中で変更することが可能）
	//R0208 1MBを越えたら強制的にNoWrap
	if( mText.GetItemCount() > 1024*1024 )
	{
		GetDocPrefDB().SetData_Number(ADocPrefDB::kWrapMode,kWrapMode_NoWrap);
	}
	*/
	//テキストが100MBを超える場合は行折り返しなしとする #1118 サイズが大きいときに強制的にNoWrapにする処理はやっぱり復活。毎回行折り返しを変更する操作が面倒だったため。
	if( mText.GetItemCount() > 100*1024*1024 )
	{
		GetDocPrefDB().SetData_Number(ADocPrefDB::kWrapMode,kWrapMode_NoWrap);
	}
	
	//==================テキストエンコーディング変換==================
	
	//UTF8へ変換
	{
		AStMutexLocker	locker(mTextMutex);//#890
		ConvertToUTF8(mText);
	}
	
#if 0
	//TextEncoding自動認識
	ABool	kakuteiByPref = false;//#473
	ABool	kakuteiByGuess = false;//#473
	ATextEncoding	guessTextEncoding = SPI_GetTextEncoding();//#764 mTextEncoding;//#473
	if( mText.GetItemCount() > 0 )
	{
		//#473 kakutei = GuessTextEncoding();//TextEncoding自動認識はADocPrefDB::kTextEncodingNameがデフォルトのままの場合のみ実行
		//推測TextEncodingを取得
		AText	tecname;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kDefaultTextEncoding,tecname);
		ATextEncodingWrapper::GetTextEncodingFromName(tecname,guessTextEncoding);
		
		kakuteiByGuess = mText.GuessTextEncoding(GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSniffTextEncoding_BOM),
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSniffTextEncoding_Charset),
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSniffTextEncoding_Legacy),
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSetDefaultTextEncodingWhenNotDecided),//B0390
				guessTextEncoding);
		//DocPrefがデフォルトのままなら推測TextEncodingを設定
		/* これによりDocPrefと推測TextEncodingは同じ値になる。確定フラグは推測の結果となる。 */
		//#830 if( GetDocPrefDB().IsStillDefault(ADocPrefDB::kTextEncodingName) == true )
		if( mTextEncodingIsDirectedByInitArgument == false )//#830
		{
			//#764 mTextEncoding = guessTextEncoding;
			AText	text;
			ATextEncodingWrapper::GetTextEncodingName(guessTextEncoding,text);
			GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,text);
		}
		//DocPrefでTextEncoding設定済みなら確定フラグOnにする
		/* DocPrefにTextEncoding設定済みとなるケース
		1. AppleScriptによりTextEncoding指定されて既存ファイルを開いた場合
		2. Xattr付きの既存ファイルを開いた場合
		3. 新規ファイル・DiffTargetドキュメント（少なくともモード設定からのコピー処理が動作する）
		*/
		else
		{
			kakuteiByPref = true;
		}
	}
	else//#570
	{
		kakuteiByPref = true;
		kakuteiByGuess = true;
	}
	
	//B0343 TextEncodingが非対応のときはUTF-8にしておく
	AText	text;
	GetDocPrefDB().GetData_Text(ADocPrefDB::kTextEncodingName,text);
	if( ATextEncodingWrapper::CheckTextEncodingAvailability(text) == false )
	{
		text.SetCstring("UTF-8");
		GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,text);
	}
	
	//ここにきたときに、テキストエンコーディング未確定、かつ、モード設定で「テキストエンコーディングを確定できないときはダイアログを表示する」をOnにした場合は
	//エンコーディング修正ダイアログ表示フラグOn
	//#379 ABool	showCorrectEncodingWindow = false;
	if( kakuteiByPref == false && kakuteiByGuess == false &&//#473 docprefでも、推測でも確定できない場合
	GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kShowDialogWhenTextEncodingNotDecided) == true )
	{
		//#379 showCorrectEncodingWindow = true;
		mShouldShowCorrectEncodingWindow = true;//#379
		mShouldShowCorrectEncodingWindow_FallbackType = kTextEncodingFallbackType_None;//#473
	}
	
	//mTextEncodingの設定
	GetDocPrefDB().GetData_Text(ADocPrefDB::kTextEncodingName,text);
	ATextEncodingWrapper::GetTextEncodingFromName(text,mTextEncoding);
	
	//TextInfoへモード設定
	mTextInfo.SetMode(mModeIndex);
	
	//テキストを内部形式へ変換
	if( mText.GetItemCount() > 0 )
	{
		//UTF8へ変換
		ATextEncoding	actualTec = mTextEncoding;
		AReturnCode	actualReturnCode = static_cast<AReturnCode>(GetDocPrefDB().GetData_Number(ADocPrefDB::kReturnCode));//#307
		//B0400 if( AUtil::ConvertToUTF8CRAnyway(mText,mTextEncoding,actualTec) == false )
		//テキストエンコーディング変換実行
		//Shift_JISロスレスフォールバックは確定フラグONのときのみ行う。（未確定で行うとUTF-8を無理矢理SJIS変換したりするため）
		{
			AStMutexLocker	locker(mTextMutex);//#890
			if( GetApp().NVI_ConvertToUTF8CRAnyway(mText,mTextEncoding,guessTextEncoding,
							true,/*#392 docprefによる確定時はフォールバック実行する false*/kakuteiByPref,
							((kakuteiByPref||kakuteiByGuess)&&
								GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kUseSJISLosslessFallback)),
							false,actualTec,actualReturnCode,mShouldShowCorrectEncodingWindow_FallbackType) == false )//B0400 #182 #307 #473
			{
				//変換エラー発生の場合
				//エンコーディング修正ダイアログ表示フラグOn
				//モード設定「テキストエンコーディングを確定できないときはダイアログを表示する」に関わらずダイアログ表示する
				//#379 showCorrectEncodingWindow = true;
				mShouldShowCorrectEncodingWindow = true;//#379
			}
		}
		//
		Convert5CToA5WhenOpenJIS();
		
		//実際のテキストエンコーディングをPrefDBと、mTextEncodingに保存
		AText	text;
		ATextEncodingWrapper::GetTextEncodingName(actualTec,text);
		GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,text);
		mTextEncoding = actualTec;
		
		//改行コードを保存 #307
		GetDocPrefDB().SetData_Number(ADocPrefDB::kReturnCode,actualReturnCode);
		
		/*#844
		//DOSEOF(1A)削除
		if( mText.GetLastChar() == kUChar_DOSEOF )
		{
			AStMutexLocker	locker(mTextMutex);//#890
			mText.Delete1(mText.GetItemCount()-1);
		}
		*/
	}
#endif
	
	//==================バイナリファイル判定========================
	//#703
	mSuspectBinaryData = mText.SuspectBinaryData();
	
	//========================テキストエンコーディング確定済みを設定========================
	//doc prefの確定済みフラグをONにする。
	mDocPrefDB.SetTextEncodingFixed(true);
	
	//#81
	//最後にロード／セーブした時のテキストエンコーディング（＝ファイルの現在のテキストエンコーディング）を記憶→自動バックアップ時に保存
	SPI_GetTextEncoding(mTextEncodingWhenLastLoadOrSave);
	
	//==================ロード済みフラグON========================
	//#831
	//ロード済みフラグON
	mLoaded = true;
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("Document text has converted to UTF-8.",this);
}

//#764
/**
テキストエンコーディングを内部形式UTF8へ変換（Load(), SPI_ReloadEditByOtherApp()からコールされる）
*/
void	ADocument_Text::ConvertToUTF8( AText& ioText )
{
	//==================テキストエンコーディング変換==================
	const ATextEncoding	origtec = SPI_GetTextEncoding();
	ATextEncoding	resultTec = origtec;
	AReturnCode	resultReturnCode = static_cast<AReturnCode>(GetDocPrefDB().GetData_Number(ADocPrefDB::kReturnCode));//#307
	ATextEncodingFallbackType	resultFallbackType = kTextEncodingFallbackType_None;
	ABool	convertSuccessful = true;
	ABool	notMixed = true;//#995
	//------------------Init関数の引数でテキストエンコーディングが指定されていた場合は、そのtecでのみ変換する------------------
	if( mTextEncodingIsDirectedByInitArgument == true )
	{
		//指定tecで変換
		//指定tecでの変換なので、フォールバック、SJISロスレスフォールバックは両方とも有りにする。
		//返り値convertSuccessfulはフォールバックを使ってでも変換ができたときはtrueになる。
		convertSuccessful = GetApp().
				SPI_ConvertToUTF8CR(ioText,origtec,true,
									true,//フォールバック
									false,//#844 SJISロスレスフォールバックはdrop GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kUseSJISLosslessFallback),//SJISロスレス
									resultReturnCode,resultTec,resultReturnCode,resultFallbackType,notMixed);//#995
	}
	//------------------Init関数の引数でテキストエンコーディングが指定されていない場合は、doc prefに従って変換する------------------
	else
	{
		//doc prefに従って変換。
		//doc prefの確定済みフラグがOFFなら推測も行う。
		//返り値convertSuccessfulはフォールバックを使ってでも変換ができたときはtrueになる。
		AText	filepath;;
		NVI_GetFilePath(filepath);
		ALoadTextPurposeType	purposeType = kLoadTextPurposeType_LoadDocument;
		if( mForMultiFileReplace == true )
		{
			purposeType = kLoadTextPurposeType_LoadDocument_ForMultiFileReplace;
		}
		convertSuccessful = GetApp().SPI_ConvertToUTF8CRUsingDocPref(purposeType,
																	 ioText,mDocPrefDB,filepath,
																	 resultTec,resultReturnCode,resultFallbackType,notMixed);//#995
	}
	
	//#995
	//複数の改行コードが混合している場合はエラーを表示する
	if( notMixed == false )
	{
		AText	notificationTitle,notificationText;
		notificationTitle.SetLocalizedText("Notification_ReturnCodeMixedError_Title");
		notificationText.SetLocalizedText("Notification_ReturnCodeMixedError");
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_General(notificationTitle,notificationText,GetEmptyText());
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_ShowNotificationPopupWindow(true);
	}
	
	//==================テキストエンコーディング・改行コード結果を保存==================
	//
	//実際のテキストエンコーディングをPrefDBに保存
	//結果が違う場合、または、現在のtec名がメニュー内に存在しないのみ設定する
	//（ATextEncodingWrapper::GetTextEncodingFromName()にて、tec名ISO-8859-1に対してtec:0x500が取得されるが、
	//0x500は本来Windows-1252なので、元々ISO-885-1だったものがWindows-1252に変化してしまう問題(#934)の対応のため）
	AText	tecName;
	GetDocPrefDB().GetData_Text(ADocPrefDB::kTextEncodingNameCocoa,tecName);//#1040
	if( origtec != resultTec || GetApp().GetAppPref().IsTextEncodingAvailable(tecName) == false )
	{
		AText	text;
		ATextEncodingWrapper::GetTextEncodingName(resultTec,text);
		//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,text);
		GetDocPrefDB().SetTextEncoding(text);//#1040
	}
	
	//改行コードを保存 #307
	GetDocPrefDB().SetData_Number(ADocPrefDB::kReturnCode,resultReturnCode);
	
	//==================テキストエンコーディングフォールバック使用ダイアログ／notification==================
	
	if( convertSuccessful == true && resultFallbackType != kTextEncodingFallbackType_None )
	{
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowDialogWhenTextEncodingFallback) == true )
		{
			//テキストウインドウ取得
			AWindowID	firstWindowID = SPI_GetFirstWindowID();
			if( firstWindowID != kObjectID_Invalid )
			{
				//テキストウインドウ生成済みであれば、ダイアログを表示
				GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_ShowTextEncodingCorrectWindow(false,resultFallbackType);//#473
			}
		}
		else
		{
			AText	notificationTitle,notificationText;
			notificationTitle.SetLocalizedText("Notification_TextEncodingFallback_Title");
			notificationText.SetLocalizedText("Notification_TextEncodingFallback");
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
					SPI_SetNotification_General(notificationTitle,notificationText,GetEmptyText());
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_ShowNotificationPopupWindow(true);
		}
	}
	
	//==================テキストエンコーディングNGダイアログ==================
	
	if( convertSuccessful == false )
	{
		//テキストウインドウ取得
		AWindowID	firstWindowID = SPI_GetFirstWindowID();
		if( firstWindowID != kObjectID_Invalid )
		{
			//テキストウインドウ生成済みであれば、ダイアログを表示
			GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_ShowTextEncodingCorrectWindow(false,kTextEncodingFallbackType_None);//#473
		}
		else
		{
			//テキストウインドウ未生成であれば、ダイアログ表示予約
			mShouldShowCorrectEncodingWindow = true;
			mShouldShowCorrectEncodingWindow_FallbackType = kTextEncodingFallbackType_None;
		}
	}
}

/**
5cをa5に変換（バックスラッシュを\で表示する）
*/
/*#940 ロード時の5c->a5変換はしないようにする。表示時にa5に変換する。
void	ADocument_Text::Convert5CToA5WhenOpenJIS( AText& ioText )
{
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kConvert5CToA5WhenOpenJIS) == false )   return;
	if( SPI_IsJISTextEncoding() == true )
	{
		AText	yen;
		yen.SetLocalizedText("Text_Yen");
		for( ATextIndex pos = 0; pos < ioText.GetItemCount(); pos = ioText.GetNextCharPos(pos) )
		{
			if( ioText.Get(pos) == kUChar_Backslash )
			{
				ioText.Delete1(pos);
				ioText.InsertText(pos,yen);
			}
		}
	}
}
*/

/**
JIS系テキストエンコーディングかどうかを取得
*/
ABool	ADocument_Text::SPI_IsJISTextEncoding() const 
{
	ATextEncoding	tec = SPI_GetTextEncoding();//#764
	return ( ATextEncodingWrapper::IsSJISSeriesTextEncoding(tec) ||//#1040
		tec == ATextEncodingWrapper::GetJISTextEncoding() ||
		tec == ATextEncodingWrapper::GetEUCTextEncoding() );
}

//#1300
/**
コピー時に5cをa5へ変換するかどうかを返す。
JIS系、かつ、AModePrefDB::kConvert5CToA5WhenOpenJISがtrueの場合、内部コード5c（バックスラッシュ）で、表示はa5（半角￥）にしているので、
コピーするときは、a5（半角￥）で出力するようにするため。
*/
ABool	ADocument_Text::SPI_ShouldConvertFrom5CToA5ForCopy() const
{
	return ( SPI_IsJISTextEncoding() == true &&
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kConvert5CToA5WhenOpenJIS) == true );
}

//#65
/**
マルチファイル置換用ロード・行折り返し計算
*/
ABool	ADocument_Text::SPI_LoadForMultiFileReplace()
{
	//初期状態以外でコールされたらエラー
	if( SPI_GetDocumentInitState() != kDocumentInitState_Initial )
	{
		_ACError("",NULL);
		return false;
	}
	
	//マルチファイル置換用であることを示すフラグON
	mForMultiFileReplace = true;
	//スクリーン＆ロード
	if( SPI_ScreenAndLoadDocumentFile() == true )
	{
		//行情報全削除
		mTextInfo.DeleteLineInfoAll();
		//一行に全データを格納
		mTextInfo.AddLineInfo(0,mText.GetItemCount(),false);
		//
		return true;
	}
	else
	{
		return false;
	}
}


//#699
/**
行情報初期化
*/
void	ADocument_Text::InitLineInfoData()
{
	//=========================各種データ初期化=========================
	//arrowtodefデータクリア
	ClearArrowToDef();
	//diff表示クリア
	ClearDiffDisplay();
	//行情報削除
	mTextInfo.DeleteLineInfoAll();
	//ジャンプ項目全削除
	DeleteAllJumpItems();
	//ローカル識別子削除
	SPI_DeleteAllLocalIdentifiers();
	//インポートファイルデータ削除
	SPI_DeleteAllImportFileData();
}

//#699
/**
行折り返しから再計算する
*/
void	ADocument_Text::ReTransitInitState_Recalculate()
{
	//初期状態の場合は何もしない。（まだview activateされていないので、次にactivateされたときに通常ルートで行折り返し計算される）
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial )
	{
		return;
	}
	
	//行折り返し計算スレッドが動作中なら、行折り返し計算スレッド停止
	if( mWrapCalculator_ThreadUnpausedFlag == true )
	{
		mWrapCalculator_ThreadUnpausedFlag = false;
		mWrapCalculator_CurrentLineIndex = kIndex_Invalid;
		mWrapCalculatorWorkingByInsertText = false;
		mWrapCalculatorProgressTotalTextLength = kIndex_Invalid;
		mWrapCalculatorProgressRemainedTextLength = 0;
		GetWrapCalculatorThread().NVI_AbortThread();
		GetWrapCalculatorThread().NVI_UnpauseIfPaused();
		GetWrapCalculatorThread().NVI_WaitThreadEnd();
		GetWrapCalculatorThread().ClearData();
	}
	//行折り返しスレッドから送付された折り返し計算終了イベントを、キューから削除する
	ABase::RemoveEventFromMainInternalEventQueue(kInternalEvent_WrapCalculatorPaused,GetWrapCalculatorThread().GetObjectID());
	
	//文法認識スレッドが動作中なら、文法認識スレッドを停止する
	SPI_StopSyntaxRecognizerThread();
	
	//行関連データ初期化
	InitLineInfoData();
	
	//TextViewの選択設定のため、まず一行追加しておく#699
	mTextInfo.AddLineInfo(0,mText.GetItemCount(),false);
	
	//各ビューの選択を解除（0,0を選択）する。
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_InitSelect();
	}
	
	//状態を初期状態（スクリーン済み）にする
	SPI_SetDocumentInitState(kDocumentInitState_Initial_FileScreened);
	
	//状態遷移（折り返し計算開始）
	mCalculateWrapInThread = true;
	SPI_TransitInitState_ViewActivated();
}

//#962
/**
screenエラーを表示し、ドキュメントを閉じる
（SPI_TransitInitState_ViewActivated()にてエラー検知したときに、Internal Eventが送付され、
　EVTDO_DoInternalEvent()経由でここに来る。）
*/
void	ADocument_Text::SPI_ShowScreenErrorAndClose()
{
	//ファイル取得
	AFileAcc	file;
	NVI_GetFile(file);
	AText	filename;
	file.GetFilename(filename);
	switch(mFileScreenError)
	{
		//ファイルが見つからない場合
	  case kFileScreenError_FileNotExist:
		{
			AText	message1, message2;
			message1.SetLocalizedText("CantOpen_Title");
			message2.SetLocalizedText("CantOpen_FileNotExist");
			message2.ReplaceParamText('0',filename);
			GetApp().NVI_ShowModalAlertWindow(message1,message2);
			break;
		}
		//フォルダの場合
	  case kFileScreenError_FileIsFolder:
		{
			AText	message1, message2;
			message1.SetLocalizedText("CantOpen_Title");
			message2.SetLocalizedText("CantOpen_FileIsFolder");
			message2.ReplaceParamText('0',filename);
			GetApp().NVI_ShowModalAlertWindow(message1,message2);
			break;
		}
		//ファイルサイズが100MBを超えるファイルの場合は、モーダルエラー表示して、ファイルを開かない
	  case kFileScreenError_FileSizeOver:
		{
			AText	message1, message2;
			message1.SetLocalizedText("CantOpen_Title");
			message2.SetLocalizedText("CantOpen_FileSize");
			message2.ReplaceParamText('0',filename);
			GetApp().NVI_ShowModalAlertWindow(message1,message2);
			break;
		}
		//トータルのファイルサイズが200MBを超える場合は、モーダルエラー表示して、ファイルを開かない
	  case kFileScreenError_TotalFileSizeOver:
		{
			AText	message1, message2;
			message1.SetLocalizedText("CantOpen_Title");
			message2.SetLocalizedText("CantOpen_TotalFileSize");
			message2.ReplaceParamText('0',filename);
			GetApp().NVI_ShowModalAlertWindow(message1,message2);
			break;
		}
		//エイリアス解決後、すでに同じファイルのドキュメントが存在していた場合は、そのドキュメントを選択
	  case kFileScreenError_DocumentDuplicatedAfterLinkResolved:
		{
			GetApp().SPI_GetTextDocumentByID(mDuplicatedDocumentID).NVI_RevealDocumentWithAView();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//ドキュメントを閉じる
	GetApp().SPI_ExecuteCloseTextDocument(GetObjectID());
}

//#699
/**
ドキュメント初期化状態遷移（Viewがactivateされた時）
@param inCalculateWrapInThread falseのときは、常にメインスレッド内で計算する（diffドキュメント用）
*/
void	ADocument_Text::SPI_TransitInitState_ViewActivated()
{
	//初期状態（スクリーン済み）でない場合はエラー
	if( SPI_GetDocumentInitState() != kDocumentInitState_Initial_FileScreened )
	{
		_ACError("",NULL);
		return;
	}
	
	//==================スクリーニング結果チェック==================
	if( mFileScreenError != kFileScreenError_NoError )
	{
		//#962
		//この関数はウインドウのNVI_Create()からもコールされるので、ここでドキュメントを削除すると、
		//その後の処理の整合性が合わなくなる。そのため、エラーがあっても、状態を変化させないで、そのまま終了する。
		//（内部イベントを送信し、次のイベント処理でエラーダイアログを表示し、OKを押されたらドキュメント削除する。）
		
		//内部イベント送信
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(GetObjectID());
		ABase::PostToMainInternalEventQueue(kInternalEvent_DocumentFileScreenError,
											GetObjectID(),0,GetEmptyText(),objectIDArray);
		//終了
		return;
	}
	//#831
	//ロード実行
	if( mLoaded == false )
	{
		LoadDocumentFile();
	}
	
	//行関連データ初期化
	InitLineInfoData();
	
	//=========================折り返し計算=========================
	//行折り返し計算（最初の200行のみ計算する。残りがあれば、スレッドに計算依頼）
	AText	fontname;
	SPI_GetFontName(fontname);
	AItemCount	limitLineCount = kIndex_Invalid;//mCalculateWrapInThreadがfalseのときは、全行について、下で折り返し計算する。
	if( mCalculateWrapInThread == true )
	{
		//mCalculateWrapInThreadがtrueのときは、kMainThreadWrapCalculateLineCount行までを、折り返し計算する。残りはスレッド。
		limitLineCount = kMainThreadWrapCalculateLineCount;
	}
	ANumber	viewWidth = AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetTextViewWidth();
	AItemCount	completedTextLen = mTextInfo.CalcLineInfoAll(mText,true,limitLineCount,
				fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
				SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),viewWidth,
				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("Document wrap 200 lines calculated.",this);
	
	//全テキスト計算済みかどうかを判定
	if( completedTextLen < mText.GetItemCount() )
	{
		//==================行折り返し計算未完了時（200行以上）==================
		
		//テキストの最後に改行コードがあるかどうかを取得（スレッド計算対象行に設定するため）
		ABool	existLineEnd = false;
		if( mText.GetItemCount()-1 >= 0 )
		{
			existLineEnd = (mText.Get(mText.GetItemCount()-1)==kUChar_LineEnd);
		}
		//スレッド計算対象行生成
		mTextInfo.AddLineInfo(completedTextLen,mText.GetItemCount()-completedTextLen,existLineEnd);
		//Viewの行データ初期化
		SPI_InitLineImageInfo();
		//スレッドに計算指示
		StartWrapCalculator(mTextInfo.GetLineCount()-1);
		//行折り返し計算中状態へ状態遷移
		SPI_SetDocumentInitState(kDocumentInitState_WrapCalculating);
	}
	else
	{
		//==================行折り返し計算完了時（200行未満）==================
		
		//Viewの行データ初期化
		SPI_InitLineImageInfo();
		//行折り返し計算完了時状態遷移
		SPI_TransitInitState_WrapCalculateCompleted();
	}
	
	//描画更新
	SPI_RefreshTextViews();
	
	//#750
	//テキストマーカー初期化
	SPI_ApplyTextMarker();
	
}

//#699
/**
ドキュメント初期化状態遷移（行折り返し計算完了時）
*/
void	ADocument_Text::SPI_TransitInitState_WrapCalculateCompleted()
{
	//行チェックデータを反映 #842
	ApplyDocPref_LineCheckedDate();
	//文法認識開始
	StartSyntaxRecognizer(0);//#698
	//文法認識中状態へ状態遷移
	SPI_SetDocumentInitState(kDocumentInitState_SyntaxRecognizing);
	
	//TextViewに行折り返し計算完了を通知（予約した選択位置が反映される）
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_DoWrapCalculated();
	}
	//#142
	//テキストカウントウインドウ更新
	UpdateDocInfoWindows();
	
	//イベントリスナー実行 #994
	SPI_ExecuteEventListener("onOpened",GetEmptyText());
}

//#699
/**
ドキュメント初期化状態遷移（文法認識完了時）
*/
void	ADocument_Text::SPI_TransitInitState_SyntaxRecognizeCompleted()
{
	//ImportFileData更新（SPI_DoSyntaxRecognizerPaused()からCopySyntaxRecognizerResult()をコールした時に、mImportFileDataDirtyフラグが設定されている。）
	UpdateImportFileData();
	mImportFileDataDirty = false;
	//Viewの行情報更新
	SPI_InitLineImageInfo();//#450
	//折りたたみデータをTextviewに反映 #450
	ApplyDocPref_CollapsedLine();
	
	//#664 SPI_DoSCMStatusUpdated()よりも先にInitViewData()がコールされたときは、
	//mSCMFileStateがkDiffTargetMode_Noneのままになっている。
	//SPI_UpdateDiffDisplayData()を実行するためには、ここでSCMFileStateを取得しておく必要がある。
	//ただし、SCMフォルダのupdate statusがまだのときは、SPI_DoSCMStatusUpdated()がコールされたタイミングで、
	//mSCMFileStateを設定し、SPI_UpdateDiffDisplayData()が実行される。
	AFileAcc	file;
	NVI_GetFile(file);
	mSCMFileState = GetApp().SPI_GetSCMFolderManager().GetFileState(file);
	
	//#81
	//デフォルトのDiffモードを設定
	//優先順位
	//１．比較先フォルダが設定されており、比較先ファイルが存在する場合
	//２．レポジトリ
	if( mDiffTargetMode == kDiffTargetMode_None )
	{
		SetDefaultDiffMode();
	}
	
	//Diff表示データ更新
	SPI_UpdateDiffDisplayData();
	
	//#379 行折り返し表示反映
	SPI_RefreshTextViews();
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::InitViewData() ended.",this);
	
	//初期化完了状態へ状態遷移
	SPI_SetDocumentInitState(kDocumentInitState_Completed);
	
	//#912
	//インデント文字自動設定
	SPI_GuessIndentCharacterType();
	
	//
	//GetApp().SPI_ReserveTimedExecution(kTimedExecutionType_TransitNextDocumentToWrapCalculatingState,3000);
}

//#379
/**
View登録後にコールされる
*/
void	ADocument_Text::NVIDO_ViewRegistered( const AViewID inViewID )
{
	//ローカル識別子データ追加（ローカル識別子データはview毎）
	AStMutexLocker	locker(mCurrentLocalIdentifierMutex);
	mCurrentLocalIdentifierListArray.AddNewObject();
	mCurrentLocalIdentifierStartLineIndexArray.Add(kIndex_Invalid);
	mCurrentLocalIdentifierEndLineIndexArray.Add(kIndex_Invalid);
}

//#893
/**
View登録解除後にコールされる
*/
void	ADocument_Text::NVIDO_ViewUnregistered( const AViewID inViewID )
{
	AIndex	index = mViewIDArray.Find(inViewID);
	
	//ローカル識別子データ削除（ローカル識別子データはview毎）
	AStMutexLocker	locker(mCurrentLocalIdentifierMutex);
	mCurrentLocalIdentifierListArray.Delete1Object(index);
	mCurrentLocalIdentifierStartLineIndexArray.Delete1(index);
	mCurrentLocalIdentifierEndLineIndexArray.Delete1(index);
}

//#379
/**
ビューがアクティブになるときにコールされる
@param inCalculateWrapInThread falseのときは、常にメインスレッド内で計算する（diffドキュメント用）
*/
void	ADocument_Text::SPI_DoViewActivating( const ABool inCalculateWrapInThread )
{
	/*#699
	if( mViewDataInited == false )
	{
		InitViewData(inViewID);
	}
	*/
	//状態がinitialかscreenedの場合のみ実行する。screening中はスレッドでscreening中でありダブルでscreenや状態遷移を実行しないために実行しないようにする。
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial || SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		//==================ドキュメント初期化状態遷移==================
		//#932 SPI_TransitInitState_ViewActivated(inCalculateWrapInThread);
		mCalculateWrapInThread = inCalculateWrapInThread;
		if( SPI_GetDocumentInitState() == kDocumentInitState_Initial )
		{
			//------------------未スクリーンの場合＝通常ロード時、および、diff targetドキュメント（レポジトリ）時------------------
			//スクリーン実行要求（スレッドでのスクリーン完了後にSPI_TransitInitState_ViewActivated()実行。
			StartScreeningForLoadDocumentFile();
		}
		else
		{
			//------------------スクリーン済みの場合＝diff targetドキュメント（ファイル）時------------------
			//diff targetドキュメントの場合は、既にSPI_ScreenAndLoadDocumentFile()経由でスクリーニング実行済みなので、
			//直接状態遷移実行する。
			SPI_TransitInitState_ViewActivated();
		}
		
		//==================フォルダラベル表示==================
		//ファイルパス取得
		AText	filepath;
		NVI_GetFilePath(filepath);
		if( filepath.GetItemCount() > 0 )
		{
			//ドキュメントのファイルパスに対応する、フォルダラベル取得
			ATextArray	folderpathArray, labeltextArray;
			GetApp().SPI_FindFolderLabel(filepath,folderpathArray,labeltextArray);
			AItemCount	itemCount = folderpathArray.GetItemCount();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				//最初のビューにnotificationポップアップ表示
				AText	folderpath, labeltext;
				folderpathArray.Get(i,folderpath);
				labeltextArray.Get(i,labeltext);
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetPopupNotificationWindow().
						SPI_GetNotificationView().SPI_SetNotification_FolderLabel(folderpath,labeltext);
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_ShowNotificationPopupWindow(true);
			}
		}
		
	}
}

//#450
/**
折りたたみ状態をDocPrefから読み込み、text viewへ反映
*/
void	ADocument_Text::ApplyDocPref_CollapsedLine()
{
	if( mViewIDArray.GetItemCount() > 0 )
	{
		//折りたたみを適用（doc pref、および、デフォルト適用）
		ANumberArray	collapsedLines;
		collapsedLines.SetFromObject(mDocPrefDB.GetData_NumberArrayRef(ADocPrefDB::kFolding_CollapsedLineIndex));
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_CollapseLines(collapsedLines,true);
	}
}

//#450
/**
折りたたみ状態をDocPrefへ保存
*/
void	ADocument_Text::SaveToDocPref_CollapsedLine()
{
	if( mViewIDArray.GetItemCount() > 0 )
	{
		ANumberArray	collapsedLines;
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetFoldingCollapsedLines(collapsedLines);
		mDocPrefDB.SetData_NumberArray(ADocPrefDB::kFolding_CollapsedLineIndex,collapsedLines);
	}
}

//#842
/**
行チェック日時をDocPrefから読み込み、反映
*/
void	ADocument_Text::ApplyDocPref_LineCheckedDate()
{
	const ANumberArray&	lineCheckedDateArray_LineIndex = mDocPrefDB.GetData_NumberArrayRef(ADocPrefDB::kLineCheckedDate_LineIndex);
	const ANumberArray&	lineCheckedDateArray_Date = mDocPrefDB.GetData_NumberArrayRef(ADocPrefDB::kLineCheckedDate_Date);
	AItemCount	lineCheckedDateArrayCount = lineCheckedDateArray_LineIndex.GetItemCount();
	for( AIndex index = 0; index < lineCheckedDateArrayCount; index++ )
	{
		SPI_SetLineCheckedDate(lineCheckedDateArray_LineIndex.Get(index),lineCheckedDateArray_Date.Get(index));
	}
}

//#842
/**
行チェック日時をDocPrefへ保存
*/
void	ADocument_Text::SaveToDocPref_LineCheckedDate()
{
	mDocPrefDB.DeleteAll_NumberArray(ADocPrefDB::kLineCheckedDate_LineIndex);
	mDocPrefDB.DeleteAll_NumberArray(ADocPrefDB::kLineCheckedDate_Date);
	AItemCount	lineCount = SPI_GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		ANumber	date = SPI_GetLineCheckedDate(lineIndex);
		if( date != 0 )
		{
			mDocPrefDB.Add_NumberArray(ADocPrefDB::kLineCheckedDate_LineIndex,lineIndex);
			mDocPrefDB.Add_NumberArray(ADocPrefDB::kLineCheckedDate_Date,date);
		}
	}
}

#if IMPLEMENTATION_FOR_MACOSX
void	ADocument_Text::SPI_SetODBMode( const OSType inODBSender, const AText& inODBToken, const AText& inODBCustomPath )
{
	mODBMode = true;
	mODBSender = inODBSender;
	mODBToken.SetText(inODBToken);
	mODBCustomPath.SetText(inODBCustomPath);
	AFileAcc	file;
	NVI_GetFile(file);
	mODBFile.CopyFrom(file);
	//#379 UpdateInfoHeader();
	//#725 GetApp().SPI_UpdateInfoHeader(GetObjectID());//#379
}
#endif

void	ADocument_Text::SPI_SetTemporaryTitle( const AText& inText )
{
	mTemporaryTitle.SetText(inText);
	//
	//#922 GetApp().SPI_UpdateWindowMenu();
	//ファイルリストウインドウ更新
	//#725 GetApp().SPI_UpdateFileListWindow_DocumentOpened(GetObjectID());
	GetApp().SPI_UpdateFileListWindows(kFileListUpdateType_DocumentOpened,GetObjectID());//#725
}

//#379
/**
指定ウインドウ以外のウインドウが存在するかどうかを返す
TryClose()等からコールされる（ほかに存在しない場合のみAskSave等の処理）
*/
ABool	ADocument_Text::SPI_ExistAnotherWindow( const AWindowID inWindowID )
{
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AWindowID	textWindowID = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetTextWindowID();
		if( textWindowID != kObjectID_Invalid )
		{
			if( textWindowID != inWindowID )   return true;
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark ---DocPref
//#898

/**
doc prefの初期化（モードに従った初期値を設定）
*/
void	ADocument_Text::InitDocPref( const AModeIndex inModeIndex )
{
	mDocPrefDB.InitPref(inModeIndex);
}

/**
doc prefをロード
*/
AModeIndex	ADocument_Text::LoadDocPref()
{
	//autobackup/docpref用のパステキストを取得
	AText	docpathtext;
	GetPathTextForAutoBackupAndDocPref(docpathtext);
	
	//doc prefをロード
	//#902 APrintPagePrefData	printpref;
	AModeIndex	modeIndex = mDocPrefDB.LoadPref(docpathtext);//#902 ,true,printpref);
	//#902 NVM_GetPrintImp().SetPrintPagePrefData(printpref);
	return modeIndex;
}

/**
doc prefへ保存
*/
void	ADocument_Text::SaveDocPref()
{
	//#450
	//------------------行折りたたみ状態をDocPrefへ保存------------------
	SaveToDocPref_CollapsedLine();
	//#842
	//------------------行チェック日時をDocPrefへ保存------------------
	SaveToDocPref_LineCheckedDate();
	
	/*#902
	//------------------印刷設定を取得------------------
	APrintPagePrefData	printpref;
	NVM_GetPrintImp().GetPrintPagePrefData(printpref);
	*/
	
	//------------------DocPrefをファイル保存------------------
	//autobackup/docpref用のパステキストを取得
	AText	docpathtext;
	GetPathTextForAutoBackupAndDocPref(docpathtext);
	//doc prefを保存
	mDocPrefDB.SavePref(docpathtext);//#902 ,true,printpref);
	
}

/**
ロード済みならdoc pref保存
*/
void	ADocument_Text::SPI_SaveDocPrefIfLoaded()
{
	if( mLoaded == true )
	{
		//ウインドウ取得
		AWindowID	firstWindowID = SPI_GetFirstWindowID();
		if( firstWindowID != kObjectID_Invalid )
		{
			//保存前処理
			GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_SavePreProcess(GetObjectID());
			//doc pref保存
			SaveDocPref();
		}
	}
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//
void	ADocument_Text::EVTDO_StartPrintMode( const ALocalRect& inRect, const ANumber inPaperWidth, const ANumber inPaperHeight )//#558
{
	mPrintRect = inRect;
	mPrintPaperWidth = inPaperWidth;//#558
	mPrintPaperHeight = inPaperHeight;//#558
	ALocalRect	textRect;
	GetPrintRect_Text(textRect);
	//win AFont	font;
	AText	fontname;//win
	AFloatNumber	fontsize;
	GetPrintFont_Text(fontname,fontsize);//win
	AWrapMode	wrapMode = SPI_GetWrapMode();
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_ForceWordWrap) == true && wrapMode == kWrapMode_NoWrap )//#1161
	{
		wrapMode = kWrapMode_WordWrap;
	}
	mTextInfoForPrint.SetMode(SPI_GetModeIndex());
	AArray<AUniqID>	addedIdentifier;
	AArray<AIndex>		addedIdentifierLineIndex;
	//#558
	//印刷横幅取得
	ANumber	width = textRect.right-textRect.left;
	if( SPI_GetVerticalMode() == true )
	{
		width = textRect.bottom-textRect.top;
	}
	//行折り返し計算
	mTextInfoForPrint.CalcLineInfoAll(mText,true,kIndex_Invalid,fontname,fontsize,true,//win #699
			SPI_GetTabWidth(),SPI_GetIndentWidth(),wrapMode,SPI_GetWrapLetterCount(),width,
			GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
	SPI_InitLineImageInfo();//#450
	
	ABool	importChanged;
	AFileAcc	file;//R0000
	NVI_GetFile(file);//R0000
	ABool	abortFlag = false;
	AText	url;//#998
	SPI_GetURL(url);//#998
	mTextInfoForPrint.RecognizeSyntaxAll(mText,url,
										 addedIdentifier,addedIdentifierLineIndex,importChanged,abortFlag);//R0000 #698 #998
}

//
void	ADocument_Text::EVTDO_EndPrintMode()
{
	//#695 AArray<AUniqID>	deletedIdentifier;
	mTextInfoForPrint.DeleteLineInfoAll(/*#695 deletedIdentifier*/);
}

//
AItemCount	ADocument_Text::EVTDO_GetPrintPageCount() 
{
	ALocalRect	textRect;
	GetPrintRect_Text(textRect);
	//win AFont	font;
	AText	fontname;//win
	AFloatNumber	fontsize;
	GetPrintFont_Text(fontname,fontsize);//win
	NVM_GetPrintImp().SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);//win
	NVM_GetPrintImp().SetVerticalMode(SPI_GetVerticalMode());//#558
	ANumber	lineHeight, lineAscent;
	NVM_GetPrintImp().GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
	lineHeight += GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kPrintOption_LineMargin);
	//ページ内行数計算
	AItemCount	lineCountInPage = (textRect.bottom-textRect.top)/lineHeight;
	if( SPI_GetVerticalMode() == true )//#558
	{
		//縦書きの場合のページ内行数計算
		lineCountInPage = (textRect.right-textRect.left)/lineHeight;
	}
	//
	if( mPrintStartParagraphIndex == kIndex_Invalid )//#524
	{
		return (mTextInfoForPrint.GetLineCount()+lineCountInPage-1)/lineCountInPage;
	}
	else//#524
	{
		AIndex	printRangeStartLineIndex = mTextInfoForPrint.GetLineIndexByParagraphIndex(mPrintStartParagraphIndex);
		AIndex	printRangeEndLineIndex = mTextInfoForPrint.GetLineIndexByParagraphIndex(mPrintEndParagraphIndex);
		return (printRangeEndLineIndex-printRangeStartLineIndex+lineCountInPage-1)/lineCountInPage;
	}
}

//
void	ADocument_Text::EVTDO_PrintPage( const AIndex inPageIndex ) 
{
	//#1316
	//印刷中カラースキームモードにする
	GetApp().GetAppPref().SetPrintColorSchemeMode(true);
	
	//#524
	AIndex	printRangeStartLineIndex = 0;
	AIndex	printRangeEndLineIndex = mTextInfoForPrint.GetLineCount();
	if( mPrintStartParagraphIndex != kIndex_Invalid )
	{
		printRangeStartLineIndex = mTextInfoForPrint.GetLineIndexByParagraphIndex(mPrintStartParagraphIndex);
		printRangeEndLineIndex = mTextInfoForPrint.GetLineIndexByParagraphIndex(mPrintEndParagraphIndex);
	}
	//
	ANumber	lineHeight, lineAscent;
	//Text
	{
		//縦書きモード設定 #558
		NVM_GetPrintImp().SetVerticalMode(SPI_GetVerticalMode());
		//
		ALocalRect	textRect;
		GetPrintRect_Text(textRect);
		//win AFont	font;
		AText	fontname;//win
		AFloatNumber	fontsize;
		GetPrintFont_Text(fontname,fontsize);//win
		NVM_GetPrintImp().SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);//win
		//#1131
		//全角スペース幅取得（原稿用紙マス目用）
		AText	zenkakuSpaceText;
		zenkakuSpaceText.SetLocalizedText("Text_ZenkakuSpace");
		ANumber	zenkakuSpaceWidth = static_cast<ANumber>(NVM_GetPrintImp().GetDrawTextWidth(zenkakuSpaceText));
		//
		NVM_GetPrintImp().GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
		ANumber	letterHeight = lineHeight;//#1131
		lineHeight += GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kPrintOption_LineMargin);
		//ページ内行数計算
		AItemCount	lineCountInPage = (textRect.bottom-textRect.top)/lineHeight;
		if( SPI_GetVerticalMode() == true )//#558
		{
			//縦書きの場合のページ内行数計算
			lineCountInPage = (textRect.right-textRect.left)/lineHeight;
		}
		//
		AIndex	startLineIndex = inPageIndex * lineCountInPage + printRangeStartLineIndex;//#524
		AIndex	endLineIndex = (inPageIndex+1) * lineCountInPage + printRangeStartLineIndex;//#524
		if( endLineIndex > mTextInfoForPrint.GetLineCount() )   endLineIndex = mTextInfoForPrint.GetLineCount();
		for( AIndex lineIndex = startLineIndex; lineIndex < endLineIndex; lineIndex++ )
		{
			//#524
			if( lineIndex >= printRangeEndLineIndex )   break;//#524
			//
			CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
			ATextPoint	tmp = {0,0};
			AAnalyzeDrawData	tmp2;//RC1
			GetTextDrawDataWithStyle(mText,mTextInfoForPrint,lineIndex,textDrawData,false,tmp,tmp,tmp2,kObjectID_Invalid);//#695
			//白を黒に変換
			if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kPrintColored) == true && 
			GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kPrintBlackForWhite) == true )
			{
				for( AIndex i = 0; i < textDrawData.attrColor.GetItemCount(); i++ )
				{
					AColor	color = textDrawData.attrColor.Get(i);
					if( AColorWrapper::ChangeBlackIfWhite(color) == true )
					{
						textDrawData.attrColor.Set(i,color);
					}
				}
			}
			//色無しに変換
			if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kPrintColored) == false )
			{
				for( AIndex i = 0; i < textDrawData.attrColor.GetItemCount(); i++ )
				{
					textDrawData.attrColor.Set(i,kColor_Black);
				}
			}
			//テキスト描画rect計算
			ALocalRect	drawRect = textRect;
			if( SPI_GetVerticalMode() == false )
			{
				drawRect.top		= textRect.top + (lineIndex-startLineIndex) * lineHeight;
				drawRect.bottom		= textRect.top + (lineIndex-startLineIndex+1) * lineHeight;
			}
			else
			{
				//縦書きの場合のテキスト描画rect計算 #558
				drawRect.top		= (lineIndex-startLineIndex+1) * lineHeight;
				drawRect.bottom		= (lineIndex-startLineIndex) * lineHeight;
				drawRect.left		= textRect.top;
				drawRect.right		= textRect.bottom;
			}
			//#1131
			//原稿用紙マス目印刷
			ABool	drawMasume = (GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kDisplayGenkoyoshi) == true);
			if( drawMasume == true && false )//#1171
			{
				AIndex	start 	= 0;
				AIndex	end 	= (drawRect.right-drawRect.left) / zenkakuSpaceWidth;
				AWrapMode	wrapMode = SPI_GetWrapMode();
				if( wrapMode == kWrapMode_WordWrapByLetterCount || wrapMode == kWrapMode_LetterWrapByLetterCount )//#1113
				{
					end = SPI_GetWrapLetterCount()/2;
				}
				ALocalPoint	spt, ept;
				for( AIndex i = start; i <= end; i++ )
				{
					spt.x = drawRect.left + zenkakuSpaceWidth * i;
					spt.y = drawRect.top;
					ept.x = spt.x;
					ept.y = spt.y + letterHeight -1;
					NVM_GetPrintImp().DrawLine(spt,ept,kColor_Gray80Percent);
				}
				//
				spt.x = drawRect.left;
				spt.y = drawRect.top;
				ept.x = drawRect.left + zenkakuSpaceWidth * end;
				ept.y = drawRect.top;
				NVM_GetPrintImp().DrawLine(spt,ept,kColor_Gray80Percent);
				//
				spt.y += letterHeight;
				ept.y += letterHeight;
				NVM_GetPrintImp().DrawLine(spt,ept,kColor_Gray80Percent);
			}
			//テキスト印刷
			NVM_GetPrintImp().DrawText(drawRect,textDrawData);
		}
	}
	//PageNumber
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintPageNumber) == true )
	{
		//縦書きモード解除 #558
		NVM_GetPrintImp().SetVerticalMode(false);
		//
		ALocalRect	pageNumberRect;
		GetPrintRect_PageNumber(pageNumberRect);
		//win AFont	font;
		AText	fontname;//win
		AFloatNumber	fontsize;
		GetPrintFont_PageNumber(fontname,fontsize);//win
		NVM_GetPrintImp().SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);//win
		AText	text;
		text.SetFormattedCstring("%d",inPageIndex+1);
		NVM_GetPrintImp().DrawText(pageNumberRect,text,kJustification_Right);
	}
	//LineNumber
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintLineNumber) == true )
	{
		//縦書きモード設定 #558
		NVM_GetPrintImp().SetVerticalMode(SPI_GetVerticalMode());
		//
		ALocalRect	rect;
		GetPrintRect_LineNumber(rect);
		//win AFont	font;
		AText	fontname;//win
		AFloatNumber	fontsize;
		GetPrintFont_LineNumber(fontname,fontsize);//win
		NVM_GetPrintImp().SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);//win
		ANumber	lineNumberHeight, lineNumberAscent;
		NVM_GetPrintImp().GetMetricsForDefaultTextProperty(lineNumberHeight,lineNumberAscent);
		//ページ内行数計算
		AItemCount	lineCountInPage = (rect.bottom-rect.top)/lineHeight;
		if( SPI_GetVerticalMode() == true )
		{
			//縦書きの場合のページ内行数計算 #558
			lineCountInPage = (rect.right-rect.left)/lineHeight;
		}
		//
		AIndex	startLineIndex = inPageIndex * lineCountInPage + printRangeStartLineIndex;//#524
		AIndex	endLineIndex = (inPageIndex+1) * lineCountInPage + printRangeStartLineIndex;//#524
		if( endLineIndex > mTextInfoForPrint.GetLineCount() )   endLineIndex = mTextInfoForPrint.GetLineCount();
		for( AIndex lineIndex = startLineIndex; lineIndex < endLineIndex; lineIndex++ )
		{
			//#524
			if( lineIndex >= printRangeEndLineIndex )   break;//#524
			//#334
			ABool	shouldPrint = true;
			AIndex	printLineIndex = lineIndex;
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintEachParagraphAlways) == true ||
						GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber_AsParagraph) == true )
			{
				//段落番号印刷の場合
				shouldPrint = (mTextInfoForPrint.GetCurrentParagraphStartLineIndex(lineIndex) == lineIndex);
				printLineIndex = mTextInfoForPrint.GetParagraphIndexByLineIndex(printLineIndex);
			}
			//
			if( shouldPrint == true )//#334
			{
				//行番号表示rect計算
				ALocalRect	drawRect = rect;
				if( SPI_GetVerticalMode() == false )
				{
					drawRect.top		= rect.top + (lineIndex-startLineIndex) * lineHeight - lineNumberAscent + lineAscent;//ベースライン合わせる
					drawRect.bottom		= rect.top + (lineIndex-startLineIndex+1) * lineHeight - lineNumberAscent + lineAscent;
					drawRect.right		-= NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberRightMarginMM);//win 080729 kPrintLineNumberRightMargin;
				}
				else
				{
					//縦書きの場合の行番号表示rect計算 #558
					drawRect.top		= (mPrintPaperWidth-rect.right) + (lineIndex-startLineIndex) * lineHeight - lineNumberAscent + lineAscent;//ベースライン合わせる
					drawRect.bottom		= (mPrintPaperWidth-rect.right) + (lineIndex-startLineIndex+1) * lineHeight - lineNumberAscent + lineAscent;
					drawRect.left		= rect.top;
					drawRect.right		= rect.bottom - NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberRightMarginMM);//win 080729 kPrintLineNumberRightMargin;
				}
				AText	text;
				text.SetFormattedCstring("%d",printLineIndex+1);//#334
				NVM_GetPrintImp().DrawText(drawRect,text,kJustification_Right);
			}
		}
	}
	//FileName
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintFileName) == true )
	{
		//縦書きモード解除 #558
		NVM_GetPrintImp().SetVerticalMode(false);
		//
		ALocalRect	rect;
		GetPrintRect_FileName(rect);
		//win AFont	font;
		AText	fontname;//win
		AFloatNumber	fontsize;
		GetPrintFont_FileName(fontname,fontsize);//win
		NVM_GetPrintImp().SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);//win
		AText	text;
		NVI_GetTitle(text);
		NVM_GetPrintImp().DrawText(rect,text,kJustification_Center);
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintSeparateLine) == true )
		{
			ALocalPoint	spt, ept;
			spt.x = rect.left;
			spt.y = rect.top - NVM_GetPrintImp().GetYInterfaceSizeByMM(mPrintSeparateLineHeightMM);
			ept.x = rect.right;
			ept.y = spt.y;
			NVM_GetPrintImp().DrawLine(spt,ept,kColor_Black,1.0,0.0,0.25);
		}
	}
	//#1393
	else
	{
		//縦書きモード解除
		NVM_GetPrintImp().SetVerticalMode(false);
	}
	
	//テスト
	/*ALocalPoint	spt, ept;
	spt.x = mPrintRect.left;
	spt.y = mPrintRect.top;
	ept.x = mPrintRect.left;
	ept.y = mPrintRect.bottom;
	NVM_GetPrintImp().DrawLine(spt,ept,kColor_Black,1.0,0.0,0.25);
	spt.x = mPrintRect.right;
	spt.y = mPrintRect.top;
	ept.x = mPrintRect.right;
	ept.y = mPrintRect.bottom;
	NVM_GetPrintImp().DrawLine(spt,ept,kColor_Black,1.0,0.0,0.25);
	1mmくらい誤差が出るが、まあよしとする
	72dpi座標系に変換しているから？
	*/
	
	//#1316
	//印刷中カラースキームモードを解除する
	GetApp().GetAppPref().SetPrintColorSchemeMode(false);
}

//テキスト印刷フォントを取得
void	ADocument_Text::GetPrintFont_Text( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize )
{
	//win outFont = SPI_GetFont();
	SPI_GetFontName(outFontName);//win
	outFontSize = SPI_GetFontSize();
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_UsePrintFont) == true )
	{
		//win outFont = mDocPrefDB.GetData_Font(AAppPrefDB::kPrintOption_PrintFont_compat);
		GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kPrintOption_PrintFontName,outFontName);//win
		outFontSize = GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintOption_PrintFontSize);
	}
}

//テキスト印刷エリアのRectを取得
void	ADocument_Text::GetPrintRect_Text( ALocalRect& outRect )
{
	outRect = mPrintRect;
	/*win 080729 outRect.left += kPrintLeftMargin;
	outRect.top += kPrintTopMargin;
	outRect.right -= kPrintRightMargin;
	outRect.bottom -= kPrintBottomMargin;
	if( mDocPrefDB.GetData_Bool(AAppPrefDB::kPrintOption_PrintPageNumber) == true )
	{
		outRect.top += kPrintPageNumberHeight;
	}*/
	//各行行番号を印刷する場合は、行番号エリアの幅だけleftをずらす
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintLineNumber) == true )
	{
		if( SPI_GetVerticalMode() == false )
		{
			outRect.left += NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberWidthMM);//win 080729 kPrintLineNumberWidth;
		}
		else
		{
			//縦書きモードの場合 #558
			outRect.top += NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberWidthMM);
		}
	}
	//ファイル名を印刷する場合は、ファイル名エリア＋分割線の高さだけbottomをずらす
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPrintOption_PrintFileName) == true )
	{
		outRect.bottom -= NVM_GetPrintImp().GetYInterfaceSizeByMM(mPrintFileNameHeightMM) 
				+ NVM_GetPrintImp().GetYInterfaceSizeByMM(mPrintSeparateLineHeightMM);//win 080729 kPrintFileNameHeight + kPrintSeparateLineHeight;
	}
}

//ページ番号のフォントを取得
void	ADocument_Text::GetPrintFont_PageNumber( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize )
{
	//win outFont = mDocPrefDB.GetData_Font(AAppPrefDB::kPrintOption_PageNumberFont);
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kPrintOption_PageNumberFontName,outFontName);//win
	outFontSize = GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintOption_PageNumberFontSize);
}

//ページ番号エリアのRectを取得
void	ADocument_Text::GetPrintRect_PageNumber( ALocalRect& outRect )
{
	outRect = mPrintRect;
	/*win 080729 outRect.left += kPrintLeftMargin;
	outRect.top += kPrintTopMargin;
	outRect.right -= kPrintRightMargin;
	outRect.bottom -= kPrintBottomMargin;*/
}

//各行行番号印刷のフォントを取得
void	ADocument_Text::GetPrintFont_LineNumber( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize )
{
	GetPrintFont_Text(outFontName,outFontSize);//win
	//win outFont = mDocPrefDB.GetData_Font(AAppPrefDB::kPrintOption_LineNumberFont_compat);
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kPrintOption_LineNumberFontName,outFontName);//win
	outFontSize = GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintOption_LineNumberFontSize);//#1401
}

//各行行番号印刷エリアのRectを取得
void	ADocument_Text::GetPrintRect_LineNumber( ALocalRect& outRect )
{
	GetPrintRect_Text(outRect);
	if( SPI_GetVerticalMode() == false )
	{
		outRect.left = mPrintRect.left;//win 080729  + kPrintLeftMargin;
		outRect.right = mPrintRect.left + NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberWidthMM);//win 080729 kPrintLineNumberWidth;
	}
	else
	{
		//縦書きモードの場合 #558
		outRect.top = mPrintRect.top;
		outRect.bottom = mPrintRect.top + NVM_GetPrintImp().GetXInterfaceSizeByMM(mPrintLineNumberWidthMM);
	}
}

//ファイル名印刷のフォントを取得
void	ADocument_Text::GetPrintFont_FileName( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize )
{
	//win outFont = mDocPrefDB.GetData_Font(AAppPrefDB::kPrintOption_FileNameFont_compat);
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kPrintOption_FileNameFontName,outFontName);//win
	outFontSize = GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintOption_FileNameFontSize);
}

//ファイル名印刷エリアのRectを取得
void	ADocument_Text::GetPrintRect_FileName( ALocalRect& outRect )
{
	outRect = mPrintRect;
	/*win 080729 outRect.left += kPrintLeftMargin;
	outRect.right -= kPrintRightMargin;
	outRect.bottom -= kPrintBottomMargin;
	outRect.top = outRect.bottom - kPrintFileNameHeight;*/
	outRect.top = outRect.bottom - NVM_GetPrintImp().GetYInterfaceSizeByMM(mPrintFileNameHeightMM);
}

//#524
/**
印刷
*/
void	ADocument_Text::SPI_Print()
{
	SPI_PrintWithRange(kIndex_Invalid,kIndex_Invalid);
}

//#524
/**
印刷範囲指定印刷
*/
void	ADocument_Text::SPI_PrintWithRange( const AIndex inStartParagraph, const AIndex inEndParagraph )
{
	//印刷範囲設定
	mPrintStartParagraphIndex = inStartParagraph;
	mPrintEndParagraphIndex = inEndParagraph;
	//印刷
	AText	title;
	NVI_GetTitle(title);
	NVI_Print(SPI_GetFirstWindowID(),title);
}

/*#379
#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

//ウインドウ取得
AWindow_Text&	ADocument_Text::GetTextWindowByIndex( const AIndex inIndex )
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mWindowIDArray.Get(inIndex));
	//#199 return dynamic_cast<AWindow_Text&>(GetApp().NVI_GetWindowByID(mWindowIDArray.Get(inIndex)));
}
*/

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---テキスト編集

//テキスト挿入
AItemCount	ADocument_Text::SPI_InsertText( const ATextIndex inInsertIndex, AText& ioInsertText,
										   const ABool inRecognizeSyntaxAlwaysInThread, const ABool inDontRedraw, 
										   const ABool inDontUpdateSubWindows )
{
	//#65
	//マルチファイル置換用にドキュメントを開いた場合の処理
	if( mForMultiFileReplace == true )
	{
		//排他制御
		AStMutexLocker	locker(mTextMutex);
		//テキストへ挿入
		mText.InsertText(inInsertIndex,ioInsertText);
		//★行情報変更不要？
		//dirty設定
		NVI_SetDirty(true);
		return 0;
	}
	
	//
	ATextPoint	textpoint = {0,0};
	SPI_GetTextPointFromTextIndex(inInsertIndex,textpoint);
	return SPI_InsertText(textpoint,ioInsertText,inRecognizeSyntaxAlwaysInThread,inDontRedraw,inDontUpdateSubWindows);
}
AItemCount	ADocument_Text::SPI_InsertText( const ATextPoint& inInsertPoint, AText& ioInsertText,
										   const ABool inRecognizeSyntaxAlwaysInThread, const ABool inDontRedraw, 
										   const ABool inDontUpdateSubWindows )
{
	//#65
	//マルチファイル置換用にドキュメントを開いた場合の処理（ここには来ない）
	if( mForMultiFileReplace == true )
	{
		_ACError("",NULL);
		return 0;
	}
	
	//行折り返し計算中は編集不可 #699
	if( SPI_IsWrapCalculating() == true )
	{
		_ACError("",NULL);
		return 0;
	}
	
	//#897
	//LFを内部改行コードCRに変換（音声入力の場合「かいぎょう」でLFが来るため）
	ioInsertText.ReplaceChar(0x0A,kUChar_LineEnd);
	
	//JIS系テキストエンコーディングの場合、半角￥を0x5Cに変換。（内部コードとしては常に0x5Cにする。）
	if( SPI_IsJISTextEncoding() == true )
	{
		AText	yen;
		yen.Add(0xC2);
		yen.Add(0xA5);
		AText	backslash;
		backslash.Add(0x5C);
		ioInsertText.ReplaceText(yen,backslash);
	}
	
	//R0006
	mDisableInlineInputHilite = true;//#187
	ClearDiffDisplay();
	ClearArrowToDef();//RC1
	mDisableInlineInputHilite = false;//#187
	
	//挿入段落数を取得 #695
	AItemCount	insertedParagraphCount = ioInsertText.GetCountOfChar(kUChar_LineEnd,0,ioInsertText.GetItemCount());
	
	//#695 テキスト内の改行数分をLineInfoのメモリ割り当て増加ステップに設定（行折り返しありの場合はこれよりも多く行数追加されるが、増加ステップとしての設定なので厳密に合う必要はない。）
	//（ただしALineInfoにてkLineInfoAllocationStepより小さくは設定されない）
	mTextInfo.SetLineReallocateStep(insertedParagraphCount);
	
	//R0208
	ABool	containLineEnd = ioInsertText.IsCharContained(kUChar_LineEnd,0,ioInsertText.GetItemCount());
	
	{
		
		//
		ATextPoint	insertPoint = inInsertPoint;
		ATextIndex	insertIndex = mTextInfo.GetTextIndexFromTextPoint(inInsertPoint);
		
		//挿入段落番号取得 #992
		AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(insertPoint.y);
		
		//selection修正のため
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPreProcess(insertIndex,ioInsertText.GetItemCount());
		}
		
		//Undo記録
		mUndoer.RecordInsertText(insertIndex,ioInsertText.GetItemCount());
		
		//次の段落開始行を取得
		AIndex	nextParagraphStartLineIndex = mTextInfo.GetNextParagraphStartLineIndex(insertPoint.y);
		
		//Dirtyにする。下から移動#688
		NVI_SetDirty(true);
		
		//================== テキスト挿入 ==================
		
		{
			AStMutexLocker	locker(mTextMutex);//#598
			//#598 mTextMutex.Lock();
			mText.InsertText(insertIndex,ioInsertText);
			//#598 mTextMutex.Unlock();
		}
		
		//==================ヒントテキスト更新==================
		mTextInfo.UpdateHintTextByInsertText(insertIndex,ioInsertText.GetItemCount());
		
		//================== 行情報更新 ==================
		
		//挿入行より後の行のLineStartを挿入文字数分ずらす
		mTextInfo.ShiftLineStarts(insertPoint.y+1,ioInsertText.GetItemCount());
		
		//挿入行より後の行の段落indexをずらす#695
		if( insertedParagraphCount > 0 )
		{
			mTextInfo.ShiftParagraphIndex(insertPoint.y+1,insertedParagraphCount);
		}
		
		AIndex	updateStartLineIndex = insertPoint.y;
		//条件によって、前行のLineInfo計算
		//（挿入行の前行に改行コードが無いときは、ラップの関係で、前行のLineInfoに更新が必要な場合がある）
		if( insertPoint.y > 0 )
		{
			if( SPI_GetLineExistLineEnd(insertPoint.y-1) == false )
			{
				//B0000 行折り返し計算高速化
				CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
				AIndex	textDrawDataStartOffset = kIndex_Invalid;
				
				AItemCount	oldLen = SPI_GetLineLengthWithLineEnd(insertPoint.y-1);
				AText	fontname;//win
				SPI_GetFontName(fontname);//win
				mTextInfo.CalcLineInfo(mText,insertPoint.y-1,/*win SPI_GetFont()*/fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
							SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),//#117
							GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),
							textDrawData,textDrawDataStartOffset);//B0000 行折り返し計算高速化
				if( oldLen != SPI_GetLineLengthWithLineEnd(insertPoint.y-1) )
				{
					updateStartLineIndex = insertPoint.y-1;
				}
			}
		}
		
		//行情報更新、表示更新
		AIndex	updateEndLineIndex;
		AItemCount insertedLineCount = AdjustLineInfo(/* insertPoint.y*/updateStartLineIndex,nextParagraphStartLineIndex,updateEndLineIndex);
		
		//#546 すでに行情報更新されているので、編集開始行番号が変わっている可能性がある（最終行で禁則文字入力などの場合）（#546 以降のsptをeditStartTextPointに置換しました）
		ATextPoint	editStartTextPoint = {0,0};
		SPI_GetTextPointFromTextIndex(insertIndex,editStartTextPoint);
		
		/*#695上へ移動
		//R0208
		if( containLineEnd == true )
		{
			mTextInfo.AdjustParagraphIndex();
		}
		*/
		
		//================== 各種範囲補正 ==================
		
		//#138
		//#858 AdjustExternalCommentArray(editStartTextPoint.y,insertedLineCount);
		
		//しおり補正
		if( mBookmarkLineIndex > editStartTextPoint.y )
		{
			mBookmarkLineIndex += insertedLineCount;
		}
		
		//ローカル範囲補正 #502 下から移動＋しおり補正と同様の処理に修正 #893
		//各ビュー毎のローカル範囲ごとのループ（ビュー毎にローカル範囲が違うため）
		for( AIndex i = 0; i < mCurrentLocalIdentifierStartLineIndexArray.GetItemCount(); i++ )
		{
			//ローカル開始行が編集開始行より後だったら、挿入行分だけずらす
			AIndex	localRangeStartLineIndex = mCurrentLocalIdentifierStartLineIndexArray.Get(i);
			if( localRangeStartLineIndex > editStartTextPoint.y && localRangeStartLineIndex != kIndex_Invalid )
			{
				mCurrentLocalIdentifierStartLineIndexArray.Set(i,localRangeStartLineIndex+insertedLineCount);
			}
			//ローカル終了行が編集終了行より後だったら、挿入行分だけずらす
			AIndex	localRangeEndLineIndex = mCurrentLocalIdentifierEndLineIndexArray.Get(i);
			if( localRangeEndLineIndex > editStartTextPoint.y && localRangeEndLineIndex != kIndex_Invalid )
			{
				mCurrentLocalIdentifierEndLineIndexArray.Set(i,localRangeEndLineIndex+insertedLineCount);
			}
		}
		
		//文法チェッカーワーニングの段落番号を補正 #992
		for( AIndex i = 0; i < mCheckerPlugin_ParagraphNumber.GetItemCount(); i++ )
		{
			AIndex	p = mCheckerPlugin_ParagraphNumber.Get(i);
			if( p >= paraIndex )
			{
				mCheckerPlugin_ParagraphNumber.Set(i,p+insertedParagraphCount);
			}
		}
		
		//================== 文法認識 ==================
		
		//#695if( true )
		{
			//変更終了行の状態を保存する
			AIndex	svNextLineStateIndex = 0, svNextLinePushedStateIndex = 0;
			if( updateEndLineIndex < SPI_GetLineCount() )
			{
				mTextInfo.GetLineStateIndex(updateEndLineIndex,svNextLineStateIndex,svNextLinePushedStateIndex);
			}
			//#467 変更行の次の行のdirective level, directive disabled状態を保存
			AIndex	svDirectiveLevel = 0;
			AIndex	svDisabledDirectiveLevel = kIndex_Invalid;
			if( editStartTextPoint.y+1 < SPI_GetLineCount() )
			{
				svDirectiveLevel = mTextInfo.GetDirectiveLevel(editStartTextPoint.y+1);
				svDisabledDirectiveLevel = mTextInfo.GetDisabledDirectiveLevel(editStartTextPoint.y+1);
			}
			
			//文法認識実行
			AIndex	recognizeEndLineIndex = StartSyntaxRecognizerByEdit(editStartTextPoint.y,insertedLineCount,inRecognizeSyntaxAlwaysInThread);//#698 RecognizeSyntax(editStartTextPoint);
			
			//変更終了行の状態を取得し、文法認識前と変わっていたら、redrawを文法認識終了行までにする。（常に文法認識終了行にすると多行段落編集が遅くなるため）
			AIndex	nextLineStateIndex = 0, nextLinePushedStateIndex = 0;
			if( updateEndLineIndex < SPI_GetLineCount() )
			{
				mTextInfo.GetLineStateIndex(updateEndLineIndex,nextLineStateIndex,nextLinePushedStateIndex);
			}
			//fprintf(stderr,"%d,",updateEndLineIndex);
			if( svNextLineStateIndex != nextLineStateIndex )
			{
				AColor	color;
				ABool	valid1, valid2;
				ATextStyle	textstyle = kTextStyle_Normal;
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(svNextLineStateIndex,color,textstyle,valid1);
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(nextLineStateIndex,color,textstyle,valid2);
				if( valid1 == true || valid2 == true )
				{
					if( recognizeEndLineIndex > updateEndLineIndex )   updateEndLineIndex = recognizeEndLineIndex;
				}
			}
			//#467 変更行の次の行のdirective level, directive disabled状態に変更があれば、redrawを文法認識終了行までにする。
			AIndex	directiveLevel = 0;
			AIndex	disabledDirectiveLevel = kIndex_Invalid;
			if( editStartTextPoint.y+1 < SPI_GetLineCount() )
			{
				directiveLevel = mTextInfo.GetDirectiveLevel(editStartTextPoint.y+1);
				disabledDirectiveLevel = mTextInfo.GetDisabledDirectiveLevel(editStartTextPoint.y+1);
			}
			if( svDirectiveLevel != directiveLevel || svDisabledDirectiveLevel != disabledDirectiveLevel )
			{
				if( recognizeEndLineIndex > updateEndLineIndex )   updateEndLineIndex = recognizeEndLineIndex;
			}
			//fprintf(stderr,"%d ",updateEndLineIndex);
		}
		/*#695
		else
		{
			ClearSyntaxData(updateStartLineIndex,updateEndLineIndex);
			//
			//#698 未使用のため削除mSyntaxDirty = true;
		}
		*/
		//================== 描画更新 ==================
		
		//redraw
		UpdateWindowText(insertPoint.y,updateStartLineIndex,updateEndLineIndex,insertedLineCount,containLineEnd,inDontRedraw);//#627 R0208(SPI_GetParagraphCount()!=svParagraphCount)); #450
		
		//================== その他処理 ==================
		
		//IndexWindow等へ変更通知
		AFileAcc	file;
		NVI_GetFile(file);
		GetApp().SPI_DoTextDocumentEdited(file,insertIndex,ioInsertText.GetItemCount());
		
		/*#699 上へ移動
		//
		NVI_SetDirty(true);
		*/
		
		//selection修正のため
		/*#199
		for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
		{
			GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_EditPostProcess(GetObjectID(),insertIndex,ioInsertText.GetItemCount());
		}
		*/
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPostProcess(insertIndex,ioInsertText.GetItemCount(),(inDontRedraw==false));//#846
		}
		
		if( inDontRedraw == false && inDontUpdateSubWindows == false )
		{
			//#142
			//テキストカウントウインドウ更新
			UpdateDocInfoWindows();
			
			//#723 CallGraf更新
			SPI_SetCallGrafEditFlag(SPI_GetFirstWindowID(),inInsertPoint.y);
			
			//タイトルバーテキスト更新
			if( insertedLineCount != 0 )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_UpdateWindowTitleBarText();
			}
		}
		
		return insertedLineCount;
	}
}

//テキスト削除
AItemCount	ADocument_Text::SPI_DeleteText( const ATextIndex inStart, const ATextIndex inEnd, 
										   const ABool inRecognizeSyntaxAlwaysInThread, 
										   const ABool inDontRedraw,
										   const ABool inDontUpdateSubWindows )
{
	//#65
	//マルチファイル置換用にドキュメントを開いた場合の処理
	if( mForMultiFileReplace == true )
	{
		//排他制御
		AStMutexLocker	locker(mTextMutex);
		//テキスト削除
		//★行情報変更不要？
		mText.Delete(inStart,inEnd-inStart);
		//dirty設定
		NVI_SetDirty(true);
		return 0;
	}
	
	//
	//行折り返し計算中は編集不可 #699
	if( SPI_IsWrapCalculating() == true )
	{
		_ACError("",NULL);
		return 0;
	}
	
	//R0006
	mDisableInlineInputHilite = true;//#187
	ClearDiffDisplay();
	ClearArrowToDef();//RC1
	mDisableInlineInputHilite = false;//#187
	
	if( inStart > inEnd )   {_ACError("delete inStart>inEnd",this); return 0; }
	
	//selection修正のため
	/*#199
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_EditPreProcess(GetObjectID(),inStart,-(inEnd-inStart));
	}
	*/
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPreProcess(inStart,-(inEnd-inStart));
	}
	
	//削除段落数を取得#695
	AItemCount	deletedParagraphCount = mText.GetCountOfChar(kUChar_LineEnd,inStart,(inEnd-inStart));
	
	//R0208 AItemCount	svParagraphCount = SPI_GetParagraphCount();
	//R0208
	ABool	containLineEnd = mText.IsCharContained(kUChar_LineEnd,inStart,inEnd);
	
	//
	ATextPoint	spt, ept;
	SPI_GetTextPointFromTextIndex(inStart,spt,true);//B0374
	SPI_GetTextPointFromTextIndex(inEnd,ept,true);//B0374
	//B0374 説明
	//行折り返し段落の最終行の行頭にて次文字削除を行った結果、前行の最終文字が、最終行の最初に来る場合、（たとえば、最終行が「い。」で、「い」を削除）
	//最終行が再描画されない問題が発生した。
	//これは、上記sptが前行の最後になっており、AdjustLineInfo()の「Insert/Delete前のlinestart情報と同じになったら終了」する判定で、
	//前行で終了と判定されてしまうため。（たとえば、最終行が「あ。」となってしまうため、最終行のlinestart情報は、削除前と同じ（ずらし考慮）となってしまう）
	//上記のように、次行preferにしておけば、最終行は必ず再描画される。また、前行は、下記の「条件によって、前行のLineInfo計算」の対象となるので再描画される。
	
	//挿入段落番号取得 #992
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(spt.y);
	
	//Undo記録
	mUndoer.RecordDeleteText(inStart,inEnd);
	
	//次の段落開始行を取得
	AIndex	nextParagraphStartLineIndex = mTextInfo.GetNextParagraphStartLineIndex(ept.y);
	
	//#399
	//dirtyにする（比較表示スクロール同期処理で判定するためSPI_EditPostProcess()より前に実行するため、また、SPI_InsertText()と合わせるため、この位置で実行）
	NVI_SetDirty(true);
	
	//==================テキスト削除==================
	{
		AStMutexLocker	locker(mTextMutex);//#598
		//#598 mTextMutex.Lock();
		mText.Delete(inStart,inEnd-inStart);
		//#598 mTextMutex.Unlock();
	}
	
	//==================ヒントテキスト更新==================
	mTextInfo.UpdateHintTextByDeleteText(inStart,inEnd);
	
	//削除行より後の行のLineStartを削除文字数分ずらす
	mTextInfo.ShiftLineStarts(spt.y+1,-(inEnd-inStart));
	
	//削除開始行よりあとの行の段落indexをずらす#695
	if( deletedParagraphCount > 0 )
	{
		mTextInfo.ShiftParagraphIndex(spt.y+1,-deletedParagraphCount);
	}
	
	AIndex	updateStartLineIndex = spt.y;
	//条件によって、前行のLineInfo計算
	//（挿入行の前行に改行コードが無いときは、ラップの関係で、前行のLineInfoに更新が必要な場合がある）
	if( spt.y > 0 )
	{
		if( SPI_GetLineExistLineEnd(spt.y-1) == false )
		{
			//B0000 行折り返し計算高速化
			CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
			AIndex	textDrawDataStartOffset = kIndex_Invalid;
			
			AItemCount	oldLen = SPI_GetLineLengthWithLineEnd(spt.y-1);
			AText	fontname;//win
			SPI_GetFontName(fontname);//win
			mTextInfo.CalcLineInfo(mText,spt.y-1,/*win SPI_GetFont()*/fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
					SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
					GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),
					textDrawData,textDrawDataStartOffset);//B0000 行折り返し計算高速化
			if( oldLen != SPI_GetLineLengthWithLineEnd(spt.y-1) )
			{
				updateStartLineIndex = spt.y-1;
			}
		}
	}
	
	//行情報更新、表示更新
	AIndex	updateEndLineIndex;
	AItemCount insertedLineCount = AdjustLineInfo(/* spt.y*/updateStartLineIndex,nextParagraphStartLineIndex,updateEndLineIndex);
	
	//#546 すでに行情報更新されているので、編集開始行番号が変わっている可能性がある（最終行で禁則文字入力などの場合）（#546 以降のsptをeditStartTextPointに置換しました）
	ATextPoint	editStartTextPoint = {0,0};
	SPI_GetTextPointFromTextIndex(inStart,editStartTextPoint);
	
	/*#695上へ移動
	//R0208
	if( containLineEnd == true )
	{
		mTextInfo.AdjustParagraphIndex();
	}
	*/
	
	//================== 各種範囲補正 ==================
	
	//#138
	//#858 AdjustExternalCommentArray(editStartTextPoint.y+1,insertedLineCount);
	
	//しおり補正
	if( mBookmarkLineIndex > editStartTextPoint.y && mBookmarkLineIndex <= editStartTextPoint.y-insertedLineCount )
	{
		mBookmarkLineIndex = editStartTextPoint.y;
	}
	else if( mBookmarkLineIndex > editStartTextPoint.y-insertedLineCount )
	{
		mBookmarkLineIndex += insertedLineCount;
	}
	
	//ローカル範囲補正 #502 下から移動＋しおり補正と同様の処理に修正
	//各ビュー毎のローカル範囲ごとのループ（ビュー毎にローカル範囲が違うため）
	for( AIndex i = 0; i < mCurrentLocalIdentifierStartLineIndexArray.GetItemCount(); i++ )
	{
		//ローカル範囲開始行が、編集開始行の次の行～削除した行の間にあるときは、ローカル範囲開始行は編集開始行にする
		AIndex	localRangeStartLineIndex = mCurrentLocalIdentifierStartLineIndexArray.Get(i);
		if( localRangeStartLineIndex != kIndex_Invalid )
		{
			if( localRangeStartLineIndex > editStartTextPoint.y && localRangeStartLineIndex <= editStartTextPoint.y-insertedLineCount )
			{
				mCurrentLocalIdentifierStartLineIndexArray.Set(i,editStartTextPoint.y);
			}
			//ローカル範囲開始行が、削除された行より後にあるときは、ローカル範囲開始行をずらす（削除行分だけ前にする）
			else if( localRangeStartLineIndex > editStartTextPoint.y-insertedLineCount )
			{
				mCurrentLocalIdentifierStartLineIndexArray.Set(i,localRangeStartLineIndex+insertedLineCount);
			}
		}
		//ローカル範囲終了行が、編集開始行の次の行～削除した行の間にあるときは、ローカル範囲終了行は編集開始行にする
		AIndex	localRangeEndLineIndex = mCurrentLocalIdentifierEndLineIndexArray.Get(i);
		if( localRangeEndLineIndex != kIndex_Invalid )
		{
			if( localRangeEndLineIndex > editStartTextPoint.y && localRangeEndLineIndex <= editStartTextPoint.y-insertedLineCount )
			{
				mCurrentLocalIdentifierEndLineIndexArray.Set(i,editStartTextPoint.y);
			}
			//ローカル範囲終了行が、削除された行より後にあるときは、ローカル範囲終了行をずらす（削除行分だけ前にする）
			else if( localRangeEndLineIndex > editStartTextPoint.y-insertedLineCount )
			{
				mCurrentLocalIdentifierEndLineIndexArray.Set(i,localRangeEndLineIndex+insertedLineCount);
			}
		}
	}
	
	//文法チェッカーワーニングの段落番号を補正 #992
	for( AIndex i = 0; i < mCheckerPlugin_ParagraphNumber.GetItemCount();  )
	{
		AIndex	p = mCheckerPlugin_ParagraphNumber.Get(i);
		if( p < paraIndex )
		{
			i++;
		}
		else if( p >= paraIndex && p < paraIndex+deletedParagraphCount )
		{
			mCheckerPlugin_ParagraphNumber.Delete1(i);
			mCheckerPlugin_ColIndex.Delete1(i);
			mCheckerPlugin_TitleText.Delete1(i);
			mCheckerPlugin_DetailText.Delete1(i);
			mCheckerPlugin_TypeIndex.Delete1(i);
			mCheckerPlugin_DisplayInTextView.Delete1(i);
		}
		else
		{
			mCheckerPlugin_ParagraphNumber.Set(i,p-deletedParagraphCount);
			i++;
		}
	}
	
	//================== 文法認識 ==================
	
	//if( inDontRecognize == false )
	{
		//#695 if( true )
		{
			//変更終了行の状態を保存する
			AIndex	svNextLineStateIndex = 0, svNextLinePushedStateIndex = 0;
			if( updateEndLineIndex < SPI_GetLineCount() )
			{
				mTextInfo.GetLineStateIndex(updateEndLineIndex,svNextLineStateIndex,svNextLinePushedStateIndex);
			}
			//#467 変更行の次の行のdirective level, directive disabled状態を保存
			AIndex	svDirectiveLevel = 0;
			AIndex	svDisabledDirectiveLevel = kIndex_Invalid;
			if( editStartTextPoint.y+1 < SPI_GetLineCount() )
			{
				svDirectiveLevel = mTextInfo.GetDirectiveLevel(editStartTextPoint.y+1);
				svDisabledDirectiveLevel = mTextInfo.GetDisabledDirectiveLevel(editStartTextPoint.y+1);
			}
			
			//文法認識実行
			AIndex	recognizeEndLineIndex = StartSyntaxRecognizerByEdit(editStartTextPoint.y,insertedLineCount,inRecognizeSyntaxAlwaysInThread);//#698 RecognizeSyntax(editStartTextPoint);
			
			//================== 文法認識による描画更新範囲の更新 ==================
			
			//変更終了行の状態を取得し、文法認識前と変わっていたら、redrawを文法認識終了行までにする。（常に文法認識終了行にすると多行段落編集が遅くなるため）
			AIndex	nextLineStateIndex = 0, nextLinePushedStateIndex = 0;
			if( updateEndLineIndex < SPI_GetLineCount() )
			{
				mTextInfo.GetLineStateIndex(updateEndLineIndex,nextLineStateIndex,nextLinePushedStateIndex);
			}
			//fprintf(stderr,"%d,",updateEndLineIndex);
			if( svNextLineStateIndex != nextLineStateIndex )
			{
				AColor	color;
				ABool	valid1, valid2;
				ATextStyle	textstyle = kTextStyle_Normal;
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(svNextLineStateIndex,color,textstyle,valid1);
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(nextLineStateIndex,color,textstyle,valid2);
				if( valid1 == true || valid2 == true )
				{
					if( recognizeEndLineIndex > updateEndLineIndex )   updateEndLineIndex = recognizeEndLineIndex;
				}
			}
			//#467 変更行の次の行のdirective level, directive disabled状態に変更があれば、redrawを文法認識終了行までにする。
			AIndex	directiveLevel = 0;
			AIndex	disabledDirectiveLevel = kIndex_Invalid;
			if( editStartTextPoint.y+1 < SPI_GetLineCount() )
			{
				directiveLevel = mTextInfo.GetDirectiveLevel(editStartTextPoint.y+1);
				disabledDirectiveLevel = mTextInfo.GetDisabledDirectiveLevel(editStartTextPoint.y+1);
			}
			if( svDirectiveLevel != directiveLevel || svDisabledDirectiveLevel != disabledDirectiveLevel )
			{
				if( recognizeEndLineIndex > updateEndLineIndex )   updateEndLineIndex = recognizeEndLineIndex;
			}
			//fprintf(stderr,"updateEnd:%d ",updateEndLineIndex);
		}
		/*#695
		else
		{
			ClearSyntaxData(updateStartLineIndex,updateEndLineIndex);
			//
			//#698 未使用のため削除mSyntaxDirty = true;
		}
		*/
	}
	
	//================== 描画更新 ==================
	
	//redraw
	//#627 inDontRedrawがfalseでもLineImageInfoの更新は行う必要がある if( inDontRedraw == false )
	//#627 {
	UpdateWindowText(editStartTextPoint.y,updateStartLineIndex,updateEndLineIndex,insertedLineCount,containLineEnd,
				inDontRedraw);//#627 R0208(SPI_GetParagraphCount()!=svParagraphCount)); #450
	//#627 }
	
	//================== その他処理 ==================
	
	//IndexWindow等へ変更通知
	AFileAcc	file;
	NVI_GetFile(file);
	GetApp().SPI_DoTextDocumentEdited(file,inStart,-(inEnd-inStart));
	
	//
	//#399 下へ移動（この内部からDoInlineInputGetSelectedText()がコールされる可能性があるため） NVI_SetDirty(true);
	
	//selection修正のため
	/*#199
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
	GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_EditPostProcess(GetObjectID(),inStart,-(inEnd-inStart));
	}
	*/
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPostProcess(inStart,-(inEnd-inStart),(inDontRedraw==false));
	}
	
	
	if( inDontRedraw == false && inDontUpdateSubWindows == false  )
	{
		//#142
		//テキストウインドウ更新
		UpdateDocInfoWindows();
		//#723 CallGraf更新
		SPI_SetCallGrafEditFlag(SPI_GetFirstWindowID(),spt.y);
		
		//タイトルバーテキスト更新
		if( insertedLineCount != 0 )
		{
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_UpdateWindowTitleBarText();
		}
	}
	
	return -insertedLineCount;
}

//テキスト削除
AItemCount	ADocument_Text::SPI_DeleteText( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, 
										   const ABool inRecognizeSyntaxAlwaysInThread, 
										   const ABool inDontRedraw,
										   const ABool inDontUpdateSubWindows )
{
	//#65
	//マルチファイル置換用にドキュメントを開いた場合の処理（ここには来ない）
	if( mForMultiFileReplace == true )
	{
		_ACError("",NULL);
		return 0;
	}
	
	return SPI_DeleteText(SPI_GetTextIndexFromTextPoint(inStartPoint),
		SPI_GetTextIndexFromTextPoint(inEndPoint),
		inRecognizeSyntaxAlwaysInThread,inDontRedraw,inDontUpdateSubWindows);
}

//テキスト編集(Insert/Delete)時に、影響行のLineInfoを更新する。
//Insert時
//xxxxxxxx [挿入文字列(改行コード含む)] xxxxxxx←inStartLineIndex
//xxxxxxxxxxxxxxxxxxxxx
//xxxxxxxx<CR>
//xxxxxxxxxxxxxxxxxxxxx←inEndLineIndex
//↑endTextIndex
//lineStartは、ずらし済み。
//よって、inEndLineIndexのlineStartまで計算すれば完了となる。
AItemCount	ADocument_Text::AdjustLineInfo( const AIndex inStartLineIndex, const AIndex inEndLineIndex, AIndex& outUpdateEndLineIndex )
{
	ABool	jumpMenuChanged = false;
	ABool	imageSizeChanged = false;//#695
	AItemCount	insertedLineCount = 0;
	//どのTextIndexまで計算すれば完了か、をendTextIndexに格納する
	AIndex	endTextIndex = SPI_GetLineStart(inEndLineIndex);
	//B0000 行折り返し計算高速化
	CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	AIndex	textDrawDataStartOffset = kIndex_Invalid;
	//
	AText	fontname;//win
	SPI_GetFontName(fontname);//win
	AIndex	lineIndex;
	for( lineIndex = inStartLineIndex; lineIndex < inEndLineIndex; lineIndex++ )
	{
		//行情報計算
		mTextInfo.CalcLineInfo(mText,lineIndex,/*win SPI_GetFont()*/fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
				SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),
				textDrawData,textDrawDataStartOffset);//B0000 行折り返し計算高速化
		//計算した行の行情報から、次の行のLineStartを計算する
		AIndex	nextLineStart = SPI_GetLineStart(lineIndex)+SPI_GetLineLengthWithLineEnd(lineIndex);
		if( nextLineStart >= endTextIndex )   break;
		//R0108 Insert/Delete前のlinestart情報と同じになったらここで終了してよい（高速化）
		if( lineIndex+1 < inEndLineIndex )
		{
			if( nextLineStart == SPI_GetLineStart(lineIndex+1) )
			{
				outUpdateEndLineIndex = lineIndex+1;
				return insertedLineCount;
			}
		}
	}
	outUpdateEndLineIndex = lineIndex+1;
	
	//まだ追加テキスト部分の行情報計算が終了していない場合（改行を含むテキスト挿入時）
	if( lineIndex >= inEndLineIndex )
	{
		//B0000 行折り返し計算高速化
		CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
		AIndex	textDrawDataStartOffset = kIndex_Invalid;
		//
		for( ; SPI_GetLineStart(lineIndex-1) + mTextInfo.GetLineLengthWithLineEnd(lineIndex-1) < SPI_GetLineStart(lineIndex); lineIndex++ )
		{
			//追加行数が閾値以上になった場合、残りはスレッドで計算する#699
			if( insertedLineCount >= kThreashold_LineCountToUseWrapCalculatorThread )
			{
				//残りテキスト計算（上のfor文の条件と同じ計算式）
				AIndex	currentLineStart = SPI_GetLineStart(lineIndex-1) + mTextInfo.GetLineLengthWithLineEnd(lineIndex-1);
				AIndex	endLineStart = SPI_GetLineStart(lineIndex);
				//スレッド計算対象行の最後が改行かどうかを判定する。
				//（ここで正しく設定しておかないと、この後のAddEmptyEOFLineIfNeeded()で、改行が最後に存在していないのにも関わらず
				//EOF行が追加されてしまう。（EOF行追加は最終行のExistLineEndがONの場合に追加されるため））
				ABool	existLineEnd = false;
				if( endLineStart-1 >= 0 )
				{
					existLineEnd = (mText.Get(endLineStart-1) == kUChar_LineEnd);
				}
				//スレッド計算対象行生成（この行に残りテキストをつめこんでおいて、スレッドで計算してから、計算終了後に展開する）
				mTextInfo.InsertLineInfo(lineIndex,currentLineStart,endLineStart-currentLineStart,existLineEnd);
				//fprintf(stderr,"currentLineStart:%d len:%d lineindex:%d\n",currentLineStart,endLineStart-currentLineStart,
				//			lineIndex);
				//スレッドに計算指示
				StartWrapCalculator(lineIndex);
				//行計算中フラグon
				mWrapCalculatorWorkingByInsertText = true;
				//追加行数インクリメント（スレッド計算対象行の分）
				insertedLineCount++;
				break;
			}
			//行情報追加
			mTextInfo.InsertLineInfo(lineIndex);
			insertedLineCount++;
			mTextInfo.CalcLineInfo(mText,lineIndex,/*win SPI_GetFont()*/fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
					SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
					GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),
						textDrawData,textDrawDataStartOffset);//B0000 行折り返し計算高速化
		}
		if( lineIndex > inEndLineIndex )
		{
			//UpdateWindowTextByScroll(lineIndex,lineIndex-inEndLineIndex);
			outUpdateEndLineIndex = SPI_GetLineCount();
			
			//#379UpdateWindowViewImageSize();
			/*フラグ化して、下に処理をまとめる#695
			//ImageSize更新をTextView(+LineNumberView)へ通知
			for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
			}
			*/
			imageSizeChanged = true;//#695
		}
	}
	//行情報計算が完了したが、まだ余分な行情報データが残っている場合（改行を含むテキスト削除時）
	else if( lineIndex < inEndLineIndex )
	{
		lineIndex++;
		//#695 高速化のため、ループではなくDeleteLineInfos()内でまとめて削除する for( AIndex i = 0; i < inEndLineIndex-lineIndex; i++ )
		{
			//行情報削除
			AArray<AUniqID>	deletedIdentifier;
			ABool	importChanged = false;
			mTextInfo.DeleteLineInfos(lineIndex,inEndLineIndex-lineIndex,deletedIdentifier,importChanged);//#695
			if( importChanged == true )
			{
				//UpdateImportFileData();
				mImportFileDataDirty = true;
			}
			insertedLineCount -= inEndLineIndex-lineIndex;//#695
			if( DeleteFromJumpMenu(deletedIdentifier) == true )
			{
				jumpMenuChanged = true;
			}
		}
		if( inEndLineIndex > lineIndex )
		{
			//UpdateWindowTextByScroll(lineIndex,lineIndex-inEndLineIndex);
			outUpdateEndLineIndex = SPI_GetLineCount();
			
			/*フラグ化して、下に処理をまとめる#695
			//#379UpdateWindowViewImageSize();
			//ImageSize更新をTextView(+LineNumberView)へ通知
			for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
			}
			*/
			imageSizeChanged = true;//#695
		}
	}
	//最終行で終了した場合
	/*#699 AddEmptyEOFLineIfNeeded()にモジュール化
	if( lineIndex == SPI_GetLineCount() )
	{
		//B0000 行折り返し計算高速化
		CTextDrawData	textDrawData;
		AIndex	textDrawDataStartOffset = kIndex_Invalid;
		//
		if( SPI_GetLineExistLineEnd(lineIndex-1) == true )
		{
			mTextInfo.InsertLineInfo(lineIndex);
			insertedLineCount++;
			mTextInfo.CalcLineInfo(mText,lineIndex,fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
					SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
					GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),
					textDrawData,textDrawDataStartOffset);//B0000 行折り返し計算高速化
			//#379UpdateWindowViewImageSize();
			//ImageSize更新をTextView(+LineNumberView)へ通知
			for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
			}
		}
	}
	*/
	if( mTextInfo.AddEmptyEOFLineIfNeeded() == true )
	{
		insertedLineCount++;
		imageSizeChanged = true;
	}
	//TextViewのイメージサイズ更新
	if( imageSizeChanged == true )
	{
		//ImageSize更新をTextView(+LineNumberView)へ通知
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
		}
	}
	//ジャンプリスト更新
	if( jumpMenuChanged == true )
	{
		//#695 DeleteFromJumpMenu()内でリスト内のデータは更新済み SPI_UpdateJumpList();//#291
		SPI_RefreshJumpList();//#695
	}
	return insertedLineCount;
}

//
void	ADocument_Text::SPI_RecordUndoActionTag( const AUndoTag inTag )
{
	mUndoer.RecordActionTag(inTag);
}

//
void	ADocument_Text::SPI_InsertLineEndAtWrap( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	if( inStartLineIndex >= inEndLineIndex )   return;
	SPI_RecordUndoActionTag(undoTag_ReturnAtWrap);
	AText	lineendText;
	lineendText.Add(kUChar_LineEnd);
	for( AIndex lineIndex = inStartLineIndex; lineIndex < inEndLineIndex; lineIndex++ )
	{
		if( SPI_GetLineExistLineEnd(lineIndex) == false )
		{
			ATextPoint	textpoint;
			textpoint.x = SPI_GetLineLengthWithoutLineEnd(lineIndex);
			textpoint.y = lineIndex;
			SPI_InsertText(textpoint,lineendText);
		}
	}
}

/**
InsertText/DeleteTextでの描画更新抑制の後の描画まとめて更新
*/
void	ADocument_Text::SPI_DeferredRefreshAfterInsertDeleteText()
{
	//text view更新
	SPI_RefreshTextViews();
}

/**
InsertText/DeleteTextでのサブウインドウ描画更新抑制の後のサブウインドウ描画まとめて更新
*/
void	ADocument_Text::SPI_DeferredRefreshSubWindowsAfterInsertDeleteText()
{
	//テキストカウントウインドウ更新
	UpdateDocInfoWindows();
	
	//タイトルバーテキスト更新
	AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_UpdateWindowTitleBarText();
}

#pragma mark ===========================

#pragma mark ---テキストに対する各種操作

void	ADocument_Text::SPI_Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	if( mUndoer.Undo(outSelectTextIndex,outSelectTextLength) == true )
	{
		//返り値がtrueは、undoによるaction回数が多い（置換等）ため、SPI_InsertText()/SPI_DeleteText()をinDontRedraw=trueでコールしていることを
		//示している。なので、ここでredrawを行う。
		SPI_DeferredRefreshAfterInsertDeleteText();
	}
	//#267
	UpdateDirtyByUndoPosition();
}

void	ADocument_Text::SPI_Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	if( mUndoer.Redo(outSelectTextIndex,outSelectTextLength) == true )
	{
		//返り値がtrueは、undoによるaction回数が多い（置換等）ため、SPI_InsertText()/SPI_DeleteText()をinDontRedraw=trueでコールしていることを
		//示している。なので、ここでredrawを行う。
		SPI_DeferredRefreshAfterInsertDeleteText();
	}
	//#267
	UpdateDirtyByUndoPosition();
}

//#267
/**
*/
void	ADocument_Text::UpdateDirtyByUndoPosition()
{
	AIndex	currentUndoPos = mUndoer.GetCurrentLastEditRecordPosition();
	if( mLastSavedUndoPosition == currentUndoPos )
	{
		NVI_SetDirty(false);
		//#485 Diffデータ更新（ファイルと同じなので、Diffデータ表示可能状態になる）svn diffは実行せず、内部diffにしたので、svnへの負荷の心配がなくなったため対応。
		SPI_UpdateDiffDisplayData();
	}
	else
	{
		NVI_SetDirty(true);
	}
}

//
ABool	ADocument_Text::SPI_GetUndoText( AText& outText ) const
{
	return mUndoer.GetUndoMenuText(outText); 
}

//
ABool	ADocument_Text::SPI_GetRedoText( AText& outText ) const
{
	return mUndoer.GetRedoMenuText(outText);
}

//保存
ABool	ADocument_Text::SPI_Save( const ABool inSaveAs )
{
	//万一未ロードでここに来た場合はリターン（保存してしまうと空のテキストを保存してしまうので万一にも保存してはいけない。）
	//★条件等再確認
	if( mLoaded == false )
	{
		_ACError("",NULL);
		return false;
	}
	
	//#81
	//==================自動バックアップ実行==================
	SPI_AutoSave_FileBeforeSave(mForMultiFileReplace);
	
	//==================ヒントテキスト全削除==================
	SPI_DeleteAllHintText();
	
	//==================デバッグ用処理==================
	//保存時に、データ整合性チェック#699
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )
	{
		mTextInfo.CheckLineInfoDataForDebug();
	}
	
	//==================マウスカーソル表示==================
	//win
	ACursorWrapper::ShowCursor();
	//
	ABool	saveOK = false;
	//保存前処理
	//（ウインドウ位置や、キャレット位置等、Window, Viewオブジェクトが保持するデータを、ドキュメントのPrefDBへ格納する処理等）
	/*#379
	if( mWindowIDArray.GetItemCount() > 0 )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_SavePreProcess(GetObjectID());
	}
	*/
	//#379
	//==================ウインドウ毎の保存前処理==================
	AWindowID	firstWindowID = SPI_GetFirstWindowID();
	if( firstWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_SavePreProcess(GetObjectID());
	}
	
	//==================ファイル保存ダイアログ表示==================
	//File未決定、もしくは、SaveAsなら、ファイル保存ダイアログを表示して終了
	if( ((NVI_IsFileSpecified()==false)&&(SPI_IsRemoteFileMode()==false)) || inSaveAs == true )//#361
	{
		/*#379
		if( mWindowIDArray.GetItemCount() > 0 )
		{
			GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_ShowSaveWindow();
		}
		*/
		if( firstWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_ShowSaveWindow();
		}
		//一旦終了。ファイル保存ダイアログ結果OKの場合、File決定後、そこから再度SPI_Save(false)がコールされる。
		return false;
	}
	
	//==================ファイル保存前処理（ローカルファイルの場合のみ）==================
	//ローカルファイルの場合のみ処理する。
	//現在のファイル内容をmiLastSaveOldTextファイルへ保存、編集中にファイルが移動された場合の対策などを行う
	
	//AFileAcc	folder;
	AText	oldText;
	ABool	oldTextOK = false;
	if( SPI_IsRemoteFileMode() == false )//#361
	{
		//
		AFileAcc	file;
		NVI_GetFile(file);
		
		//ファイルロック未SpecifyならSpecifyする
		if( mFileWriteLocker.IsSpecified() == false )
		{
			mFileWriteLocker.Specify(file);
		}
		
		/*#666 mFileWriteLocker.Lock()の中身がopen(,O_NONBLOCK|O_SHLOCK)なので、
		O_SHLOCKできないようなネットワークファイルサーバーだと、常にfalseが返る可能性がある。
		ここでLockする必要性はなく、書き込み権限チェックしなくても、後のWriteFile()でエラー検知できるので、
		ここのチェックは外す。
		//ファイルロックしていない場合は書き込みパーミションチェック（ロック）
		if( mFileWriteLocker.IsLocked() == false )
		{
			if( mFileWriteLocker.Lock() == false )
			{
				AText	text1, text2;
				text1.SetLocalizedText("FileSaveError_WritePermision1");
				text2.SetLocalizedText("FileSaveError_WritePermision2");
				GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2);
				return;
			}
		}
		*/
		
		//現在のファイル内容をoldTextへ格納（ディスク容量エラー等で保存失敗した場合に、元のファイル内容に戻すため）
		oldTextOK = file.ReadTo(oldText);
		
		//編集中にファイルが移動された場合の対策
		if( file.Exist() == false )
		{
			file.CreateFile();
		}
		//folder.SpecifyParent(file);
	}
	/*
	else
	{
		AFileWrapper::GetAppPrefFolder(folder);
	}
	*/
	
	//現在のファイル内容を"miSaving"ファイルに保存し、save終了後、miSavingを削除する。（もし、途中で強制終了したら、miSavingに残っている）
	//B0446 AFileAcc	miSavingFile;
	//B0446 miSavingFile.SpecifyUniqChildFile(folder,"miSaving");
	//B0446 miSavingFile.CreateFile();//win 080702
	//B0446 miSavingFile.WriteFile(oldText);
	GetApp().SPI_SaveTemporaryText(oldText);//B0446
	
	try//B0446
	{
		//==================テキストエンコーディング変換==================
		
		//UTF-8, CRから、各テキストエンコーディングへ変換
		AText	buffer;
		ABool	converted = false;
		/*#844 SJISロスレスフォールバックはdrop
		//#473 SJISロスレスフォールバック
		if( SPI_GetTextEncoding() == ATextEncodingWrapper::GetSJISTextEncoding() &&
					GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kUseSJISLosslessFallback) == true )
		{
			if( ConvertFromUTF8ToSJISUsingLosslessFallback(buffer) == true )
			{
				converted = true;
			}
		}
		*/
		//
		if( converted == false )//#473
		{
			//Canonical Comp変換設定ONならCanonical Compへ変換する #1044
			ABool	convertToCanonical = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp);
			//保存用テキストエンコーディングへ変換
			if( mText.ConvertFromUTF8CR(SPI_GetTextEncoding(),SPI_GetReturnCode(),buffer,convertToCanonical) == false )
			{
				//------------------テキストエンコーディング変換に失敗した場合------------------
				
				//#65
				//ウインドウがない場合（＝マルチファイル置換によるオープン中）の場合、ここでリターン（返り値：エラー）する
				if( SPI_GetFirstWindowID() == kObjectID_Invalid )
				{
					return false;
				}
				/*#926
				//ダイアログ表示
				AText	text1, text2;
				text1.SetLocalizedText("FileSave_ConvertedToUTF8_1");
				text2.SetLocalizedText("FileSave_ConvertedToUTF8_2");
				GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2);
				*/
				AObjectIDArray	objectIDArray;
				objectIDArray.Add(SPI_GetFirstWindowID());
				ABase::PostToMainInternalEventQueue(kInternalEvent_SaveResultDialog_ForcedToUTF8,
													GetObjectID(),0,GetEmptyText(),objectIDArray);
				//UTF-8に変更
				//#934 SPI_SetTextEncoding(ATextEncodingWrapper::GetUTF8TextEncoding());
				AText	tecname("UTF-8");
				SPI_SetTextEncoding(tecname);
				mText.ConvertReturnCodeFromCR(SPI_GetReturnCode(),buffer);
			}
		}
		
		/*#844
		//DOSEOF(1A)付加
		if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSaveDOSEOF) == true )
		{
			buffer.Add(kUChar_DOSEOF);
		}
		*/
		
		//------------------BOM付加処理------------------
		//無印UTF16のBOM(FEFF)追加
		//win if( SPI_GetTextEncoding() == ATextEncodingWrapper::GetUTF16TextEncoding() )
		//Encodingの値がUTF-16かつ、名前がUTF-16LE, UTF-16BE以外の場合にBOM追加（Windowsでは値はUTF-16とUTF-16LEが同じのため）
		AText	encname;//win
		SPI_GetTextEncoding(encname);//win
		if( SPI_GetTextEncoding() == ATextEncodingWrapper::GetUTF16TextEncoding() &&
					encname.Compare("UTF-16LE") == false && encname.Compare("UTF-16BE") == false )//win
		{
			if( ATextEncodingWrapper::UTF16IsLE() == false )
			{
				//Big Endian
				buffer.Insert1(0,0xFE);
				buffer.Insert1(1,0xFF);
			}
			else
			{
				//Little Endian
				buffer.Insert1(0,0xFF);
				buffer.Insert1(1,0xFE);
			}
		}
		// UTF8 BOM追加 EF BB BF   B0016
		if( SPI_GetTextEncoding() == ATextEncodingWrapper::GetUTF8TextEncoding() &&
					GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kSaveUTF8BOM) == true )
		{
			buffer.Insert1(0,0xEF);
			buffer.Insert1(1,0xBB);
			buffer.Insert1(2,0xBF);
		}
		
		if( SPI_IsRemoteFileMode() == false )
		{
			//==================ファイル書き込み処理（ローカルファイルの場合）==================
			
			//
			AFileAcc	file;
			NVI_GetFile(file);
			
			//ファイルロック解除
			mFileWriteLocker.Unlock();
			
			//ファイル書き込み
			AFileError	err;
			if( file.WriteFile(buffer,err) == false )
			{
				//#1034 if( err == dskFulErr )//B0224
				if( err == kFileError_DiskFull )//#1034
				{
					if( SPI_GetFirstWindowID() != kObjectID_Invalid )//#65
					{
						AText	text1, text2;
						text1.SetLocalizedText("FileSaveError_DiskFull_A");
						text2.SetLocalizedText("FileSaveError_DiskFull_B");
						GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2,true);
					}
					if( oldTextOK == true )//B0290
					{
						file.WriteFile(oldText);
					}
				}
				else
				{
					if( SPI_GetFirstWindowID() != kObjectID_Invalid )//#65
					{
						AText	text1, text2;
						text1.SetLocalizedText("FileSaveError_Other_A");
						text2.SetLocalizedText("FileSaveError_Other_B");
						GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2,true);
					}
					if( oldTextOK == true )//B0290
					{
						file.WriteFile(oldText);
					}
				}
				//ファイルロック
				if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kLockWhileEdit) == true )//B0355
				{
					mFileWriteLocker.Lock();
				}
				//B0446 miSavingFile.DeleteFile();//win 080702
				return false;
			}
			else
			{
				saveOK = true;
			}
			
			//==================DocPref保存==================
			
			if( SPI_GetFirstWindowID() != kObjectID_Invalid )//#65 ウインドウがある場合のみ実行
			{
				SaveDocPref();//#898
			}
			
#if IMPLEMENTATION_FOR_MACOSX
			//==================Mac独自データ保存==================
			if( SPI_GetFirstWindowID() != kObjectID_Invalid )//#65 ウインドウがある場合のみ実行
			{
				//------------------クリエータ・タイプを保存------------------
				AFileAttribute	attr;
				attr.creator = mCreator;
				attr.type = mType;
				//win AFileWrapper::SetFileAttribute(file,attr);
				file.SetFileAttribute(attr);
				
				//------------------usroリソース保存------------------
				//#311 Mac OS X 10.6でダブルクリックでファイルが開けない問題対策。usroリソースを自分で追加。
				if( mCreator == 'MMKE' )
				{
					//FSRef取得
					FSRef	ref;
					file.GetFSRef(ref);
					//リソースフォーク追加
					{
						HFSUniStr255	forkName;
						::FSGetResourceForkName(&forkName);
						::FSCreateResourceFork(&ref,forkName.length,forkName.unicode,0);
					}
					//リソースフォークを開く
					short	rRefNum = ::FSOpenResFile(&ref,fsWrPerm);
					if( rRefNum != -1 )
					{
						//元々のusroリソースを削除
						Handle	res = ::Get1Resource('usro',0);
						if( res != NULL ) 
						{
							::RemoveResource(res);
							::DisposeHandle(res);
						}
						//ハンドルalloc
						Handle	h = ::NewHandle(0x404);
						::HLock(h);
						for( AIndex i = 0; i < 0x404; i++ )
						{
							*((*h)+i) = 0x00;
						}
						//アプリケーションパス取得
						AFileAcc	appFile;
						AFileWrapper::GetAppFile(appFile);
						AText	appPath;
						appFile.GetPath(appPath);
						AItemCount	len = appPath.GetItemCount();
						if( len <= 0x400 )
						{
							//レングス書き込み
							UInt32*	p = (UInt32*)(*h);
							(*p) = len;//なぜかhost endianのまま書き込まないと最終的にリソースに書き込まれるデータが逆になる
							//アプリパス書き込み
							for( AIndex i = 0; i < len; i++ )
							{
								*((*h)+4+i) = appPath.Get(i);
							}
						}
						::HUnlock(h);
						//リソース書き込み・クローズ
						::AddResource(h,'usro',0,"\p");
						::WriteResource(h);
						::ReleaseResource(h);
						::CloseResFile(rRefNum);
					}
				}
				
				//R0230
				//------------------拡張属性------------------
				//#844 if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSaveTextEncodingToEA) == true )
				if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 ) //#844 設定削除。10.4以上なら常に設定。（10.3は未対応。）
				{
					AText	tecname;
					SPI_GetTextEncoding(tecname);
					//#1040
					//IANA名称を取得（上で取得した名称はあくまでmi用設定名称）
					ATextEncoding	tec = ATextEncodingWrapper::GetTextEncodingFromName(tecname);
					ATextEncodingWrapper::GetTextEncodingIANAName(tec,tecname);
					//
					file.SetTextEncodingByXattr(tecname);
				}
				else
				{
					file.SetTextEncodingByXattr(GetEmptyText());
				}
			}
#endif
			
			//==================ファイルロック==================
			if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kLockWhileEdit) == true )//B0355
			{
				mFileWriteLocker.Lock();
			}
			
			//==================OSへファイル保存通知==================
			ALaunchWrapper::NotifyFileChanged(file);
			
#if IMPLEMENTATION_FOR_MACOSX
			//==================ODB送信==================
			//ODBModified送信(Mac only)
			if( mODBMode == true )
			{
				ABool	fileChanged = false;
				if( mODBFile.Compare(file) == false )
				{
					fileChanged = true;
				}
				ALaunchWrapper::SendODBModified(mODBSender,mODBToken,mODBFile,fileChanged,file);
				mODBFile.CopyFrom(file);
			}
#endif
			
			//#976
			//最後に開いたファイルを保存（新規保存時のデフォルトフォルダに使用）
			GetApp().SPI_SetLastOpenedTextFile(file);
		}
		else
		{
			//==================ファイル書き込み処理（リモートファイルの場合）==================
			/*#844 ローカルバックアップ機能削除
			//------------------バックアップ保存------------------
			//バックアップ保存
			AText	server, user, path;
			mRemoteFileURL.GetFTPURLServer(server);
			mRemoteFileURL.GetFTPURLUser(user);
			mRemoteFileURL.GetFTPURLPath(path);//B0369
			AIndex	index = GetApp().GetAppPref().FindFTPAccountPrefIndex(server,user);
			if( index != kIndex_Invalid )
			{
				if( GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_Backup,index) == true )
				{
					AText	text;
					GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFTPAccount_BackupFolderPath,index,text);
					AFileAcc	folder, file;
					folder.SpecifyWithAnyFilePathType(text);//B0389 OK
					if( folder.Exist() == false )
					{
						AText	text1, text2;
						text1.SetLocalizedText("FileSaveError_FTPLocalBackupError1");
						text2.SetLocalizedText("FileSaveError_FTPLocalBackupError2");
						GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2);
					}
					else
					{
						//B0369 FTPパスと対応するローカル上のフォルダを検索（生成）
						ATextArray	foldernamearray;
						foldernamearray.ExplodeFromText(path,'/');
						for( AIndex i = 0; i < foldernamearray.GetItemCount()-1; i++ )
						{
							if( foldernamearray.GetTextConst(i).GetItemCount() == 0 )   continue;
							AFileAcc	f;
							f.SpecifyChild(folder,foldernamearray.GetTextConst(i));
							folder.CopyFrom(f);
							if( folder.Exist() == false )
							{
								folder.CreateFolder();
							}
						}
						
						AText	filename;
						mRemoteFileURL.GetFilename(filename);
						//R0207
						if( GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_BackupWithDateTime,index) == true )
						{
							AText	suffix;
							filename.GetSuffix(suffix);
							AText	text;
							ADateTimeWrapper::Get6LettersDateText(text);
							text.Insert1(0,'_');
							filename.InsertText(filename.GetItemCount()-suffix.GetItemCount(),text,0,text.GetItemCount());
							ADateTimeWrapper::GetTimeText(text);
							text.Insert1(0,'_');
							text.ReplaceChar(':','_');
							text.ReplaceChar(' ','_');
							filename.InsertText(filename.GetItemCount()-suffix.GetItemCount(),text,0,text.GetItemCount());
						}
						file.SpecifyChild(folder,filename);
						file.CreateFile();
						file.WriteFile(buffer);
					}
				}
			}
			*/
			//------------------リモート保存------------------
			/*#361
			GetApp().SPI_GetFTPConnection().SPNVI_Run_PutFile(mFTPURL,buffer);
			saveOK = GetApp().SPI_GetFTPConnection().SPI_GetLastPutFileOK();
			*/
			//プラグイン再接続（ドキュメントを開いた時と同じプラグインに接続する）
			//#193 GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).ReconnectPlugin();
			//リモート接続オブジェクトを使用して保存（PUTコマンド送信） #361
			//現状、スレッドを使用せず、同期保存している
			if( GetApp().SPI_GetRemoteConnection(mRemoteConnectionObjectID).
			   ExecutePUT(kRemoteConnectionRequestType_PUTfromTextDocument,mRemoteFileURL,buffer) == false )
			{
				//★保存不可（コマンド実行中）時処理
			}
			//#361
			//ファイルリストのフォルダを更新
			AText	parentFolderURL;
			mRemoteFileURL.GetParentURL(parentFolderURL);
			GetApp().SPI_UpdateFileListRemoteFolder(parentFolderURL);
			
			//
			saveOK = true;
			
			//#241
			//#898 mDocPrefDB.SaveForRemoteFile(mRemoteFileURL);
			SaveDocPref();//#898
			
			//#361
			mTextOnRemoteServer.SetText(buffer);
		}
		
		//==================保存OK時、保存後処理==================
		if( saveOK == true )
		{
			//#81
			//------------------unsaved dataファイルの削除------------------
			AutoSave_RemoveUnsavedDataFile();
			
			//#734
			//------------------プレビュー更新------------------
			SPI_ReloadPrevew();
			
			//------------------Dirty解除------------------
			NVI_SetDirty(false);
			
			//------------------保存時undo position記憶------------------
			//#267 UndoPosition保存
			mLastSavedUndoPosition = mUndoer.GetCurrentLastEditRecordPosition();
			mUndoer.SetInhibitConcat();//結合防止フラグ設定（次のUndoと結合すると保存位置へのUndoにならなくなる）
			
			//------------------charsetチェック------------------
			//charsetチェック R0045
			if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kCheckCharsetWhenSave) == true )
			{
				ATextEncoding	charsetEnc;
				if( mText.GuessTextEncodingFromCharset(charsetEnc) == true && SPI_GetFirstWindowID() != kObjectID_Invalid )//#65
				{
					//SJIS系の場合は、全く同じでなくても、どちらもSJIS系なら同じとみなす。#1331
					if( ATextEncodingWrapper::IsSJISSeriesTextEncoding(charsetEnc) == true &&
						ATextEncodingWrapper::IsSJISSeriesTextEncoding(SPI_GetTextEncoding()) == true )
					{
						//SJIS系は同じとみなす。処理なし。
					}
					//
					else if( charsetEnc != SPI_GetTextEncoding() )
					{
						/*#926
						AText	text1, text2;
						text1.SetLocalizedText("CharsetCheck_Error1");
						text2.SetLocalizedText("CharsetCheck_Error2");
						GetApp().SPI_GetTextWindowByID(SPI_GetFirstWindowID()).SPI_ShowChildErrorWindow(GetObjectID(),text1,text2);
						*/
						AObjectIDArray	objectIDArray;
						objectIDArray.Add(SPI_GetFirstWindowID());
						ABase::PostToMainInternalEventQueue(kInternalEvent_SaveResultDialog_CharsetError,
															GetObjectID(),0,GetEmptyText(),objectIDArray);
						
					}
				}
			}
			
			//==================ローカルファイルの場合のみ保存後処理==================
			if( SPI_IsRemoteFileMode() == false )
			{
				//------------------Appにドキュメント保存を通知------------------
				//
				AFileAcc	file;
				NVI_GetFile(file);
				//
				GetApp().SPI_DoTextDocumentSaved(file);
				
				//------------------最後に保存したファイルデータを記憶------------------
				file.GetLastModifiedDataTime(mLastModifiedDateTime);
				//#653 保存したファイルデータ（ファイルに書いた生データ）をファイルに記憶（他アプリでの編集検知に使用）
				//#693 mLastReadOrWriteFileRawData.SetText(buffer);
				AFileAcc	lastLoadOrSaveFile;
				GetLastLoadOrSaveCopyFile(lastLoadOrSaveFile);
				lastLoadOrSaveFile.CreateFile();
				lastLoadOrSaveFile.WriteFile(buffer);
				//
				//#379 UpdateInfoHeader();
				//#725 GetApp().SPI_UpdateInfoHeader(GetObjectID());//#379
			}
		}
		//#81
		//最後にロード／セーブした時のテキストエンコーディング（＝ファイルの現在のテキストエンコーディング）を記憶→自動バックアップ時に保存
		SPI_GetTextEncoding(mTextEncodingWhenLastLoadOrSave);
		
		//------------------Diffデータ更新------------------
		//#379if( mDiffTargetMode == kDiffTargetMode_File || GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCompareWithLatestWhenSave) == true )//#379
		//#379{
		SPI_UpdateDiffDisplayData();
		//#379}
		
		//------------------text view更新------------------
		//SPI_ReRecognizeSyntax();//B0000
		//#212 RefreshWindow();
		SPI_RefreshTextViews();//#212
		
		//#688
		//------------------タイトルバー更新（従来InfoHeaderに表示していた内容をタイトルバーに表示するため）------------------
		GetApp().NVI_UpdateTitleBar();
		
		//イベントリスナー実行 #992
		SPI_ExecuteEventListener("onSaved",GetEmptyText());
	}
	catch(...)//B0446
	{
		_ACError("ADocument_Text::SPI_Save() caught exception.",NULL);//#199
	}
	//
	//B0446 miSavingFile.DeleteFile();//win 080702
	return true;
}

//#844 SJISロスレスフォールバックはdrop
#if 0
/**
SJISロスレスフォールバック変換
@return 異常文字が存在し、かつ、フォールバック変換成功したらtrue
*/
ABool	ADocument_Text::ConvertFromUTF8ToSJISUsingLosslessFallback( AText& outText ) const
{
	//結果テキスト削除
	outText.DeleteAll();
	//異常文字が存在したかどうかのフラグ
	ABool	abnormalCharExist = false;
	//
	AArray<AUChar>	escape01Array;
	AText	text;
	//UTF-8テキスト内を検索（U+105CXX/0x01が存在すればescape01Arrayへ格納していく）
	AItemCount	textlen = mText.GetItemCount();
	for( AIndex pos = 0; pos < textlen; pos++ )
	{
		AUChar	ch = mText.Get(pos);
		if( ch == 0x01 )
		{
			//0x01
			escape01Array.Add(0x01);
			text.Add(0x01);
		}
		else if( ch == 0xF4 )
		{
			if( mText.Get(pos+1) == 0x85 )
			{
				//U+105CXX SJIS2バイト目不在エラー
				AUChar	escapeCh = ((mText.Get(pos+2)&0x03)<<6)+(mText.Get(pos+3)&0x3F);
				escape01Array.Add(escapeCh);
				text.Add(0x01);
				abnormalCharExist = true;
				pos += 3;
			}
			else
			{
				//普通に1byte追加
				text.Add(ch);
			}
		}
		else
		{
			//普通に1byte追加
			text.Add(ch);
		}
	}
	//異常文字が存在しなければreturn
	if( abnormalCharExist == false )   return false;
	
	//SJISへ変換
	if( text.ConvertFromUTF8CR(ATextEncodingWrapper::GetSJISTextEncoding(),SPI_GetReturnCode(),outText) == false )
	{
		return false;
	}
	
	//0x01をescape01Arrayに従って変換
	AItemCount	len = outText.GetItemCount();
	AIndex	escape01pos = 0;
	for( AIndex pos = 0; pos < len; pos++ )
	{
		//0x01かどうかを判定
		AUChar	ch = outText.Get(pos);
		if( ch == 0x01 )
		{
			//escape01Arrayに従って文字変換
			AUChar	escapeCh = escape01Array.Get(escape01pos);
			escape01pos++;
			if( escapeCh != 0x01 )
			{
				outText.Set(pos,escapeCh);
			}
		}
	}
	return true;
}
#endif

//#81
/**
dirty状態変更時処理
*/
void	ADocument_Text::NVIDO_SetDirty( const ABool inDirty )
{
	//未保存データ用のフラグ（前回自動保存からの編集があるかどうかのフラグ）を設定
	mAutoSaveDirty = inDirty;
}

#pragma mark ===========================

#pragma mark ---自動バックアップ
//#81

//自動バックアップ世代
const AItemCount	kAutoBackup_MaxGenerations_Monthly = 24;
const AItemCount	kAutoBackup_MaxGenerations_Daily = 10;
const AItemCount	kAutoBackup_MaxGenerations_Hourly = 10;
const AItemCount	kAutoBackup_MaxGenerations_Minutely = 6;

/**
自動バックアップされたファイルのリストを取得
*/
void	ADocument_Text::SPI_UpdateAutoBackupArray()
{
	//自動バックアップフォルダを取得
	AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
	if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
	{
		//各自動バックアップフォルダ内のファイルのリストを取得
		GetAutoBackupNumberArray(monthlyFolder,mAutoBackupMonthlyArray_Number);
		GetAutoBackupNumberArray(dailyFolder,mAutoBackupDailyArray_Number);
		GetAutoBackupNumberArray(hourlyFolder,mAutoBackupHourlyArray_Number);
		GetAutoBackupNumberArray(minutelyFolder,mAutoBackupMinutelyArray_Number);
	}
}

/**
上書き保存時の保存前ファイルの自動バックアップ
*/
void	ADocument_Text::SPI_AutoSave_FileBeforeSave( const ABool inAlways )
{
	//自動バックアップdisableなら何もしない
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kEnableAutoBackup) == false )
	{
		return;
	}
	
	//現在の日時テキストを取得
	AText	currentDateTimeText;
	GetAutoSaveCurrentDateTimeText(currentDateTimeText);
	ANumber64bit	currentNumber = currentDateTimeText.GetNumber64bit();
	
	//===============Minutelyバックアップ===============
	//分ごとバックアップのファイルリストから最新のものの日時テキストを取得
	ANumber64bit	latestMinutelyNumber = 0;
	if( mAutoBackupMinutelyArray_Number.GetItemCount() > 0 )
	{
		latestMinutelyNumber = mAutoBackupMinutelyArray_Number.Get(mAutoBackupMinutelyArray_Number.GetItemCount()-1);
	}
	//現在の日時テキストと最新の日時テキストを比較して、10分の桁より上が増えていれば、バックアップを行う
	if( currentNumber/10 > latestMinutelyNumber/10 || inAlways == true )
	{
		//自動バックアップフォルダ取得
		AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
		if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
		{
			//バックアップファイル取得
			AFileAcc	backupfile;
			backupfile.SpecifyChild(minutelyFolder,currentDateTimeText);
			switch( mTextDocumentType )
			{
				//ローカルファイルの場合は、ファイルを自動バックアップファイルにコピーする
			  case kTextDocumentType_Normal:
				{
					AFileAcc	srcfile;
					NVI_GetFile(srcfile);
					srcfile.CopyFileTo(backupfile,true);
					break;
				}
				//リモートファイルの場合は、ロード時に読み込んだ（あるいは、保存時に記憶した）テキストを自動バックアップファイルに書き込む
			  case kTextDocumentType_RemoteFile:
				{
					backupfile.CreateFile();
					backupfile.WriteFile(mTextOnRemoteServer);
					break;
				}
				//
			  case kTextDocumentType_DiffTarget:
				{
					//ここには来ないので処理無し
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			//バックアップファイルのテキストエンコーディングを記憶
			if( mForMultiFileReplace == false )
			{
				AText	tecfilename;
				tecfilename.SetText(currentDateTimeText);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(minutelyFolder,tecfilename);
				tecfile.CreateFile();
				tecfile.WriteFile(mTextEncodingWhenLastLoadOrSave);
			}
			//最大世代数-1になるまで、最も古いファイルを削除する
			while( mAutoBackupMinutelyArray_Number.GetItemCount() > kAutoBackup_MaxGenerations_Minutely-1 )
			{
				//ファイルリストの最初の日時テキストを取得
				AText	deletefilename;
				deletefilename.SetNumber64bit(mAutoBackupMinutelyArray_Number.Get(0));
				//削除ファイルを取得
				AFileAcc	deletefile;
				deletefile.SpecifyChild(minutelyFolder,deletefilename);
				//ファイルを削除
				deletefile.DeleteFile();
				//ファイルリストの項目を削除
				mAutoBackupMinutelyArray_Number.Delete1(0);
				//テキストエンコーディング記憶ファイルを削除
				AText	tecfilename;
				tecfilename.SetText(deletefilename);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(minutelyFolder,tecfilename);
				tecfile.DeleteFile();
			}
			//ファイルリストを更新
			GetAutoBackupNumberArray(minutelyFolder,mAutoBackupMinutelyArray_Number);
			//「自動バックアップと比較」メニューを更新
			SPI_UpdateComopareWithAutoBackupMenu();
		}
	}
	//===============Hourlyバックアップ===============
	//時ごとバックアップのファイルリストから最新のものの日時テキストを取得
	ANumber64bit	latestHourlyNumber = 0;
	if( mAutoBackupHourlyArray_Number.GetItemCount() > 0 )
	{
		latestHourlyNumber = mAutoBackupHourlyArray_Number.Get(mAutoBackupHourlyArray_Number.GetItemCount()-1);
	}
	//現在の日時テキストと最新の日時テキストを比較して、時の桁より上が増えていれば、バックアップを行う
	if( currentNumber/100 > latestHourlyNumber/100 || inAlways == true )
	{
		//自動バックアップフォルダ取得
		AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
		if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
		{
			//バックアップファイル取得
			AFileAcc	backupfile;
			backupfile.SpecifyChild(hourlyFolder,currentDateTimeText);
			switch( mTextDocumentType )
			{
				//ローカルファイルの場合は、ファイルを自動バックアップファイルにコピーする
			  case kTextDocumentType_Normal:
				{
					AFileAcc	srcfile;
					NVI_GetFile(srcfile);
					srcfile.CopyFileTo(backupfile,true);
					break;
				}
				//リモートファイルの場合は、ロード時に読み込んだ（あるいは、保存時に記憶した）テキストを自動バックアップファイルに書き込む
			  case kTextDocumentType_RemoteFile:
				{
					backupfile.CreateFile();
					backupfile.WriteFile(mTextOnRemoteServer);
					break;
				}
				//
			  case kTextDocumentType_DiffTarget:
				{
					//ここには来ないので処理無し
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			//バックアップファイルのテキストエンコーディングを記憶
			if( mForMultiFileReplace == false )
			{
				AText	tecfilename;
				tecfilename.SetText(currentDateTimeText);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(hourlyFolder,tecfilename);
				tecfile.CreateFile();
				tecfile.WriteFile(mTextEncodingWhenLastLoadOrSave);
			}
			//最大世代数-1になるまで、最も古いファイルを削除する
			while( mAutoBackupHourlyArray_Number.GetItemCount() > kAutoBackup_MaxGenerations_Hourly-1 )
			{
				//ファイルリストの最初の日時テキストを取得
				AText	deletefilename;
				deletefilename.SetNumber64bit(mAutoBackupHourlyArray_Number.Get(0));
				//削除ファイルを取得
				AFileAcc	deletefile;
				deletefile.SpecifyChild(hourlyFolder,deletefilename);
				//ファイルを削除
				deletefile.DeleteFile();
				//ファイルリストの項目を削除
				mAutoBackupHourlyArray_Number.Delete1(0);
				//テキストエンコーディング記憶ファイルを削除
				AText	tecfilename;
				tecfilename.SetText(deletefilename);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(hourlyFolder,tecfilename);
				tecfile.DeleteFile();
			}
			//ファイルリストを更新
			GetAutoBackupNumberArray(hourlyFolder,mAutoBackupHourlyArray_Number);
			//「自動バックアップと比較」メニューを更新
			SPI_UpdateComopareWithAutoBackupMenu();
		}
	}
	//===============Dailyバックアップ===============
	//日ごとバックアップのファイルリストから最新のものの日時テキストを取得
	ANumber64bit	latestDailyNumber = 0;
	if( mAutoBackupDailyArray_Number.GetItemCount() > 0 )
	{
		latestDailyNumber = mAutoBackupDailyArray_Number.Get(mAutoBackupDailyArray_Number.GetItemCount()-1);
	}
	//現在の日時テキストと最新の日時テキストを比較して、日の桁より上が増えていれば、バックアップを行う
	if( currentNumber/10000 > latestDailyNumber/10000 || inAlways == true )
	{
		//自動バックアップフォルダ取得
		AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
		if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
		{
			//バックアップファイル取得
			AFileAcc	backupfile;
			backupfile.SpecifyChild(dailyFolder,currentDateTimeText);
			switch( mTextDocumentType )
			{
				//ローカルファイルの場合は、ファイルを自動バックアップファイルにコピーする
			  case kTextDocumentType_Normal:
				{
					AFileAcc	srcfile;
					NVI_GetFile(srcfile);
					srcfile.CopyFileTo(backupfile,true);
					break;
				}
				//リモートファイルの場合は、ロード時に読み込んだ（あるいは、保存時に記憶した）テキストを自動バックアップファイルに書き込む
			  case kTextDocumentType_RemoteFile:
				{
					backupfile.CreateFile();
					backupfile.WriteFile(mTextOnRemoteServer);
					break;
				}
				//
			  case kTextDocumentType_DiffTarget:
				{
					//ここには来ないので処理無し
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			//バックアップファイルのテキストエンコーディングを記憶
			if( mForMultiFileReplace == false )
			{
				AText	tecfilename;
				tecfilename.SetText(currentDateTimeText);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(dailyFolder,tecfilename);
				tecfile.CreateFile();
				tecfile.WriteFile(mTextEncodingWhenLastLoadOrSave);
			}
			//最大世代数-1になるまで、最も古いファイルを削除する
			while( mAutoBackupDailyArray_Number.GetItemCount() > kAutoBackup_MaxGenerations_Daily-1 )
			{
				//ファイルリストの最初の日時テキストを取得
				AText	deletefilename;
				deletefilename.SetNumber64bit(mAutoBackupDailyArray_Number.Get(0));
				//削除ファイルを取得
				AFileAcc	deletefile;
				deletefile.SpecifyChild(dailyFolder,deletefilename);
				//ファイルを削除
				deletefile.DeleteFile();
				//ファイルリストの項目を削除
				mAutoBackupDailyArray_Number.Delete1(0);
				//テキストエンコーディング記憶ファイルを削除
				AText	tecfilename;
				tecfilename.SetText(deletefilename);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(dailyFolder,tecfilename);
				tecfile.DeleteFile();
			}
			//ファイルリストを更新
			GetAutoBackupNumberArray(dailyFolder,mAutoBackupDailyArray_Number);
			//「自動バックアップと比較」メニューを更新
			SPI_UpdateComopareWithAutoBackupMenu();
		}
	}
	//===============Monthlyバックアップ===============
	//分ごとバックアップのファイルリストから最新のものの日時テキストを取得
	ANumber64bit	latestMonthlyNumber = 0;
	if( mAutoBackupMonthlyArray_Number.GetItemCount() > 0 )
	{
		latestMonthlyNumber = mAutoBackupMonthlyArray_Number.Get(mAutoBackupMonthlyArray_Number.GetItemCount()-1);
	}
	//現在の日時テキストと最新の日時テキストを比較して、月の桁より上が増えていれば、バックアップを行う
	if( currentNumber/1000000 > latestMonthlyNumber/1000000 || inAlways == true )
	{
		//自動バックアップフォルダ取得
		AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
		if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
		{
			//バックアップファイル取得
			AFileAcc	backupfile;
			backupfile.SpecifyChild(monthlyFolder,currentDateTimeText);
			switch( mTextDocumentType )
			{
				//ローカルファイルの場合は、ファイルを自動バックアップファイルにコピーする
			  case kTextDocumentType_Normal:
				{
					AFileAcc	srcfile;
					NVI_GetFile(srcfile);
					srcfile.CopyFileTo(backupfile,true);
					break;
				}
				//リモートファイルの場合は、ロード時に読み込んだ（あるいは、保存時に記憶した）テキストを自動バックアップファイルに書き込む
			  case kTextDocumentType_RemoteFile:
				{
					backupfile.CreateFile();
					backupfile.WriteFile(mTextOnRemoteServer);
					break;
				}
				//
			  case kTextDocumentType_DiffTarget:
				{
					//ここには来ないので処理無し
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			//バックアップファイルのテキストエンコーディングを記憶
			if( mForMultiFileReplace == false )
			{
				AText	tecfilename;
				tecfilename.SetText(currentDateTimeText);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(monthlyFolder,tecfilename);
				tecfile.CreateFile();
				tecfile.WriteFile(mTextEncodingWhenLastLoadOrSave);
			}
			//最大世代数-1になるまで、最も古いファイルを削除する
			while( mAutoBackupMonthlyArray_Number.GetItemCount() > kAutoBackup_MaxGenerations_Monthly-1 )
			{
				//ファイルリストの最初の日時テキストを取得
				AText	deletefilename;
				deletefilename.SetNumber64bit(mAutoBackupMonthlyArray_Number.Get(0));
				//削除ファイルを取得
				AFileAcc	deletefile;
				deletefile.SpecifyChild(monthlyFolder,deletefilename);
				//ファイルを削除
				deletefile.DeleteFile();
				//ファイルリストの項目を削除
				mAutoBackupMonthlyArray_Number.Delete1(0);
				//テキストエンコーディング記憶ファイルを削除
				AText	tecfilename;
				tecfilename.SetText(deletefilename);
				tecfilename.AddCstring("_attr");
				AFileAcc	tecfile;
				tecfile.SpecifyChild(monthlyFolder,tecfilename);
				tecfile.DeleteFile();
			}
			//ファイルリストを更新
			GetAutoBackupNumberArray(monthlyFolder,mAutoBackupMonthlyArray_Number);
			//「自動バックアップと比較」メニューを更新
			SPI_UpdateComopareWithAutoBackupMenu();
		}
	}
}

/**
auto backup/doc pref用のパステキストを取得
*/
void	ADocument_Text::GetPathTextForAutoBackupAndDocPref( AText& outPath ) const
{
	//==================
	//結果初期化
	outPath.DeleteAll();
	switch( mTextDocumentType )
	{
		//------------------ローカルファイルの場合------------------
	  case kTextDocumentType_Normal:
		{
			//ファイルパスを取得
			NVI_GetFilePath(outPath);
			break;
		}
		//------------------diffターゲットドキュメントの場合------------------
	  case kTextDocumentType_DiffTarget:
		{
			//ファイルパスを取得
			mFileForDiffTargetDocument.GetPath(outPath);
			break;
		}
		//------------------リモートファイルの場合------------------
	  case kTextDocumentType_RemoteFile:
		{
			//URLのうちサーバーテキスト部分のコロン、スラッシュをアンダーバーに置き換えたものを取得する。
			
			//リモートファイルのURLを取得
			outPath.SetText(mRemoteFileURL);
			//"://"の位置を取得
			AIndex	foundpos = kIndex_Invalid;
			if( mRemoteFileURL.FindCstring(0,"://",foundpos) == true )
			{
				if( mRemoteFileURL.FindCstring(foundpos+3,"/",foundpos) == true )
				{
					//"://"より後の最初の"/"の位置をposに格納
					AIndex	pos = foundpos;
					//サーバーテキスト部分、パステキスト部分に分けてテキストを取得
					AText	servertext, pathtext;
					mRemoteFileURL.GetText(0,pos,servertext);
					mRemoteFileURL.GetText(pos,mRemoteFileURL.GetItemCount()-pos,pathtext);
					//サーバーテキスト部分のコロン、スラッシュをアンダーバーに置き換える
					servertext.ReplaceChar(':','_');
					servertext.ReplaceChar('/','_');
					//サーバーテキスト部分、パステキスト部分を結合
					outPath.SetText(servertext);
					outPath.AddText(pathtext);
				}
			}
			break;
		}
	  default:
		{
			return;
			break;
		}
	}
}

/**
バックアップフォルダ・ドキュメント保存先フォルダ取得
*/
ABool	ADocument_Text::GetAutoBackupFolder( AFileAcc& outMonthlyFolder, AFileAcc& outDailyFolder,
		AFileAcc& outHourlyFolder, AFileAcc& outMinutelyFolder ) const
{
	//自動バックアップ用パステキスト取得
	AText	docfilepath;
	GetPathTextForAutoBackupAndDocPref(docfilepath);
	if( docfilepath.GetItemCount() == 0 )
	{
		//ファイルパスが空（未specify）の場合は、取得不可
		return false;
	}
	else
	{
		//自動バックアップ用ルートフォルダ取得
		AFileAcc	autoBackupRootFolder;
		GetApp().SPI_GetAutoBackupRootFolder(autoBackupRootFolder);
		//このドキュメントの自動バックアップ保存フォルダ取得
		AFileAcc	autoSaveFolder;
		autoSaveFolder.SpecifyChild(autoBackupRootFolder,docfilepath);
		autoSaveFolder.CreateFolderRecursive();
		//各バックアップフォルダ生成
		outMonthlyFolder.SpecifyChild(autoSaveFolder,"Monthly");
		outMonthlyFolder.CreateFolder();
		outDailyFolder.SpecifyChild(autoSaveFolder,"Daily");
		outDailyFolder.CreateFolder();
		outHourlyFolder.SpecifyChild(autoSaveFolder,"Hourly");
		outHourlyFolder.CreateFolder();
		outMinutelyFolder.SpecifyChild(autoSaveFolder,"Minutely");
		outMinutelyFolder.CreateFolder();
		return true;
	}
}

/**
自動バックアップフォルダのファイルの日時テキストリストを取得
*/
void	ADocument_Text::GetAutoBackupNumberArray( const AFileAcc& inFolder, AArray<ANumber64bit>& outArray ) 
{
	//結果初期化
	outArray.DeleteAll();
	//現在の日時テキストを取得
	AText	currentDateTimeText;
	GetAutoSaveCurrentDateTimeText(currentDateTimeText);
	ANumber64bit	currentNumber = currentDateTimeText.GetNumber64bit();
	//フォルダのファイルリストを取得
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	//各ファイルについてのループ
	AItemCount	itemCount = children.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//ファイル名取得
		AText	filename;
		children.GetObjectConst(i).GetFilename(filename);
		//ファイル名を数値化
		ANumber64bit	num = filename.GetNumber64bit();
		//周知が日時テキストのフォーマットに一致していない、または、現在よりも後の場合は、何もしない。
		if( filename.GetItemCount() != 12 || num < 200001010000LL || num > currentNumber )
		{
			continue;
		}
		//リストに追加
		outArray.Add(num);
	}
	//リストをソート
	outArray.Sort(true);
}

/**
自動バックアップ用 現在の日時テキストを取得
*/
void	ADocument_Text::GetAutoSaveCurrentDateTimeText( AText& outText ) 
{
	outText.SetCstring("20");
	AText	t;
	ADateTimeWrapper::Get6LettersDateText(t);
	outText.AddText(t);
	ADateTimeWrapper::Get2LettersHourText(t);
	outText.AddText(t);
	ADateTimeWrapper::Get2LettersMinuteText(t);
	outText.AddText(t);
}

/**
自動バックアップとの比較メニューを更新
*/
void	ADocument_Text::SPI_UpdateComopareWithAutoBackupMenu()
{
	{
		//メニュー項目全削除
		GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_CompareWithAutoBackup_Monthly);
		//月毎ファイルリスト項目毎のループ
		AItemCount	itemCount = mAutoBackupMonthlyArray_Number.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//ファイル名(action text)、メニューテキスト取得
			ANumber64bit	num = mAutoBackupMonthlyArray_Number.Get(i);
			AText	filename;
			filename.SetNumber64bit(num);
			AText	menutext;
			GetAutoBackupCompareMenuText(num,menutext);
			//メニューに追加
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_CompareWithAutoBackup_Monthly,0,menutext,filename,kObjectID_Invalid,NULL,0,false);
		}
	}
	{
		//メニュー項目全削除
		GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_CompareWithAutoBackup_Daily);
		//日毎ファイルリスト項目毎のループ
		AItemCount	itemCount = mAutoBackupDailyArray_Number.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//ファイル名(action text)、メニューテキスト取得
			ANumber64bit	num = mAutoBackupDailyArray_Number.Get(i);
			AText	filename;
			filename.SetNumber64bit(num);
			AText	menutext;
			GetAutoBackupCompareMenuText(num,menutext);
			//メニューに追加
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_CompareWithAutoBackup_Daily,0,menutext,filename,kObjectID_Invalid,NULL,0,false);
		}
	}
	{
		//メニュー項目全削除
		GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_CompareWithAutoBackup_Hourly);
		//時毎ファイルリスト項目毎のループ
		AItemCount	itemCount = mAutoBackupHourlyArray_Number.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//ファイル名(action text)、メニューテキスト取得
			ANumber64bit	num = mAutoBackupHourlyArray_Number.Get(i);
			AText	filename;
			filename.SetNumber64bit(num);
			AText	menutext;
			GetAutoBackupCompareMenuText(num,menutext);
			//メニューに追加
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_CompareWithAutoBackup_Hourly,0,menutext,filename,kObjectID_Invalid,NULL,0,false);
		}
	}
	{
		//メニュー項目全削除
		GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_CompareWithAutoBackup_Minutely);
		//時毎ファイルリスト項目毎のループ
		AItemCount	itemCount = mAutoBackupMinutelyArray_Number.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//ファイル名(action text)、メニューテキスト取得
			ANumber64bit	num = mAutoBackupMinutelyArray_Number.Get(i);
			AText	filename;
			filename.SetNumber64bit(num);
			AText	menutext;
			GetAutoBackupCompareMenuText(num,menutext);
			//メニューに追加
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByMenuItemID(kMenuItemID_CompareWithAutoBackup_Minutely,0,menutext,filename,kObjectID_Invalid,NULL,0,false);
		}
	}
}

/**
「自動バックアップと比較」メニューのメニュー表示テキストを取得
*/
void	ADocument_Text::GetAutoBackupCompareMenuText( const ANumber64bit inNumber, AText& outText ) const
{
	//inNumberを年月日時分に分ける
	ANumber	year = (inNumber/100000000LL)%10000LL;
	ANumber	month = (inNumber/1000000LL)%100LL;
	ANumber	day = (inNumber/10000LL)%100LL;
	ANumber	hour = (inNumber/100LL)%100LL;
	ANumber	minute = (inNumber)%100LL;
	//inNumberに対応するdate timeテキストを取得（秒は削除）
	ADateTime	datetime = ADateTimeWrapper::GetDateTime(year,month,day,hour,minute,0);
	ADateTimeWrapper::GetDateTimeText(datetime,outText);
	outText.Delete(outText.GetItemCount()-3,3);
	//inNumberに対応するdate timeと現在日時との差分を取得
	ANumber	years = 0, months = 0, days = 0, hours = 0, minutes = 0, seconds = 0;
	ADateTimeWrapper::GetDateTimeDiff(ADateTimeWrapper::GetCurrentTime(),datetime,
				years,months,days,hours,minutes,seconds);
	//差分時間テキストを追加
	outText.AddCstring("  ( ");
	AText	t;
	if( years > 0 )
	{
		t.SetLocalizedText("Year");
		t.ReplaceParamText('1',years);
		outText.AddText(t);
	}
	if( months > 0 )
	{
		t.SetLocalizedText("Month");
		t.ReplaceParamText('1',months);
		outText.AddText(t);
	}
	if( days > 0 )
	{
		t.SetLocalizedText("Day");
		t.ReplaceParamText('1',days);
		outText.AddText(t);
	}
	if( hours > 0 )
	{
		t.SetLocalizedText("Hour");
		t.ReplaceParamText('1',hours);
		outText.AddText(t);
	}
	if( minutes > 0 || (years==0&&months==0&&days==0&&hours==0))
	{
		t.SetLocalizedText("Minute");
		t.ReplaceParamText('1',minutes);
		outText.AddText(t);
	}
	{
		t.SetLocalizedText("Before");
		outText.AddText(t);
	}
	outText.AddCstring(" )");
}

/**
分ごとバックアップファイルを取得
*/
ABool	ADocument_Text::SPI_GetAutoBackupMinutelyFile( const AText& inFilename, AFileAcc& outFile )
{
	//自動バックアップフォルダを取得
	AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
	if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
	{
		//バックアップファイルを取得
		outFile.SpecifyChild(minutelyFolder,inFilename);
		return true;
	}
	else
	{
		return false;
	}
}

/**
時間ごとバックアップファイルを取得
*/
ABool	ADocument_Text::SPI_GetAutoBackupHourlyFile( const AText& inFilename, AFileAcc& outFile )
{
	//自動バックアップフォルダを取得
	AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
	if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
	{
		//バックアップファイルを取得
		outFile.SpecifyChild(hourlyFolder,inFilename);
		return true;
	}
	else
	{
		return false;
	}
}

/**
日ごとバックアップファイルを取得
*/
ABool	ADocument_Text::SPI_GetAutoBackupDailyFile( const AText& inFilename, AFileAcc& outFile )
{
	//自動バックアップフォルダを取得
	AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
	if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
	{
		//バックアップファイルを取得
		outFile.SpecifyChild(dailyFolder,inFilename);
		return true;
	}
	else
	{
		return false;
	}
}

/**
月ごとバックアップファイルを取得
*/
ABool	ADocument_Text::SPI_GetAutoBackupMonthlyFile( const AText& inFilename, AFileAcc& outFile )
{
	//自動バックアップフォルダを取得
	AFileAcc	monthlyFolder, dailyFolder, hourlyFolder, minutelyFolder;
	if( GetAutoBackupFolder(monthlyFolder,dailyFolder,hourlyFolder,minutelyFolder) == true )
	{
		//バックアップファイルを取得
		outFile.SpecifyChild(monthlyFolder,inFilename);
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark ===========================

#pragma mark ---未保存バックアップ
//#81

//未保存バックアップのインターバルタイマー
const ANumber	kAutoBackup_UnsavedTextIntervalMinute = 5;

/**
未保存データの自動保存（ビューdeactivate時、タイマー経過時にコールされる）
*/
void	ADocument_Text::SPI_AutoSave_Unsaved()
{
	//前回自動保存からの編集があるかどうかのフラグを判定し、なければ何もしない
	if( mAutoSaveDirty == false )
	{
		return;
	}
	
	//現在の日時テキストを取得
	AText	currentDateTimeText;
	GetAutoSaveCurrentDateTimeText(currentDateTimeText);
	ANumber64bit	currentNumber = currentDateTimeText.GetNumber64bit();
	//未保存バックアップのファイルリストから最新のものの日時テキストを取得
	ANumber64bit	latestUnsavedTextNumber = 0;
	if( mAutoBackupUnsavedDataArray_Number.GetItemCount() > 0 )
	{
		latestUnsavedTextNumber = mAutoBackupUnsavedDataArray_Number.Get(mAutoBackupUnsavedDataArray_Number.GetItemCount()-1);
	}
	//現在の日時テキストと最新の日時テキストを比較して、インターバルタイマー区切りを超えていれば、バックアップを行う
	if( currentNumber/kAutoBackup_UnsavedTextIntervalMinute > latestUnsavedTextNumber/kAutoBackup_UnsavedTextIntervalMinute )
	{
		//バックアップファイル取得
		AFileAcc	backupfile;
		backupfile.SpecifyChild(mAutoBackupUnsavedDataFolder,currentDateTimeText);
		backupfile.CreateFile();
		//テキストをファイルに保存
		backupfile.WriteFile(mText);
		//ファイルパス取得し、"filepath"ファイルに保存
		AText	filepath;
		switch( mTextDocumentType )
		{
		  case kTextDocumentType_Normal:
			{
				NVI_GetFilePath(filepath);
				break;
			}
		  case kTextDocumentType_RemoteFile:
			{
				filepath.SetText(mRemoteFileURL);
				break;
			}
			//
		  case kTextDocumentType_DiffTarget:
			{
				//ここには来ないので処理無し
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		AFileAcc	filepathfile;
		filepathfile.SpecifyChild(mAutoBackupUnsavedDataFolder,"filepath");
		filepathfile.CreateFile();
		filepathfile.WriteFile(filepath);
		
		//-----------------既存のunsaved fileは全削除-----------------
		//（mAutoBackupUnsavedDataArray_Numberリストとそのファイルを消す。上で作成したファイルは含まれない）
		AutoSave_RemoveUnsavedDataFile();
		
		//リストを更新
		GetAutoBackupNumberArray(mAutoBackupUnsavedDataFolder,mAutoBackupUnsavedDataArray_Number);
		
		//前回自動保存からの編集があるかどうかのフラグをOFFにする
		//（次にNVI_SetDirty()がコールされるとmAutoSaveDirtyもtrueになる）
		mAutoSaveDirty = false;
	}
}

/**
unsaved fileを全削除する
@note 保存時にもコールされる。保存時にunsavedを削除しておかないと、savedファイルの内容がunsavedよりも新しくなってしまう。
*/
void	ADocument_Text::AutoSave_RemoveUnsavedDataFile()
{
	//リスト内のデータが無くなるまでループ
	while( mAutoBackupUnsavedDataArray_Number.GetItemCount() > 0 )
	{
		//リストの最初のテキストを取得
		AText	deletefilename;
		deletefilename.SetNumber64bit(mAutoBackupUnsavedDataArray_Number.Get(0));
		//削除ファイルを取得
		AFileAcc	deletefile;
		deletefile.SpecifyChild(mAutoBackupUnsavedDataFolder,deletefilename);
		//ファイル削除
		deletefile.DeleteFile();
		//リスト項目削除
		mAutoBackupUnsavedDataArray_Number.Delete1(0);
	}
}

/**
未保存データバックアップフォルダ生成
*/
void	ADocument_Text::CreateUnsavedDataBackupFolder()
{
	//未保存データバックアップルートフォルダ取得
	AFileAcc	unsavedDataRootFolder;
	GetApp().SPI_GetUnsavedDataFolder(unsavedDataRootFolder);
	//"UnisavedData-x"フォルダを取得
	mAutoBackupUnsavedDataFolder.SpecifyUniqChildFile(unsavedDataRootFolder,"UnsavedData");
	mAutoBackupUnsavedDataFolder.CreateFolder();
	//リスト取得
	GetAutoBackupNumberArray(mAutoBackupUnsavedDataFolder,mAutoBackupUnsavedDataArray_Number);
}

/**
未保存データバックアップフォルダ削除
*/
void	ADocument_Text::DeleteUnsavedDataBackupFolder()
{
	//"UnsavedData-x"フォルダを削除
	mAutoBackupUnsavedDataFolder.DeleteFileOrFolderRecursive();
}

#pragma mark ===========================

#pragma mark ---テキスト描画用データ取得

//テキスト描画用データ取得
//UTF16化、タブのスペース化、制御文字の可視化等を行う。
//CTextDrawDataクラスのうち、下記を設定する。
//UTF16DrawText: UTF16での描画用テキスト
//UnicodeDrawTextIndexArray: インデックス：ドキュメントが保持している元のテキストのインデックス　値：描画テキストのUnicode文字単位でのインデックス
void	ADocument_Text::SPI_GetTextDrawDataWithoutStyle( const AIndex inLineIndex, CTextDrawData& outTextDrawData ) const
{
	AStMutexLocker	locker(mTextMutex);
	GetTextDrawDataWithoutStyle(mText,mTextInfo,inLineIndex,outTextDrawData);
}
void	ADocument_Text::GetTextDrawDataWithoutStyle( const AText& inText, const ATextInfo& inTextInfo, const AIndex inLineIndex, CTextDrawData& outTextDrawData ) const//#695
{
	//#703
	outTextDrawData.SetFontFallbackEnabled(!mSuspectBinaryData);
	//
	inTextInfo.GetTextDrawDataWithoutStyle(/*#695 mText*/inText,SPI_GetTabWidth(),SPI_GetIndentWidth(),0,inLineIndex,outTextDrawData,true,//#117
										   GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters),false,true,
										   (SPI_IsJISTextEncoding() == true && GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kConvert5CToA5WhenOpenJIS) == true ));
}

//テキスト描画用データ取得
void	ADocument_Text::SPI_GetTextDrawDataWithStyle( const AIndex inLineIndex, CTextDrawData& outTextDrawData, 
		const ABool inSelectMode, const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2,
		AAnalyzeDrawData& ioAnalyzeDrawData, const AViewID inViewID ) const //RC1 #893
{
	AStMutexLocker	locker(mTextMutex);
	GetTextDrawDataWithStyle(mText,mTextInfo,inLineIndex,outTextDrawData,
							 inSelectMode,inSelectTextPoint1,inSelectTextPoint2,ioAnalyzeDrawData,inViewID);
}
void	ADocument_Text::GetTextDrawDataWithStyle( const AText& inText, const ATextInfoForDocument& inTextInfo, //#695
		const AIndex inLineIndex, CTextDrawData& outTextDrawData, 
		const ABool inSelectMode, const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2,
		AAnalyzeDrawData& ioAnalyzeDrawData, const AViewID inViewID ) const //RC1 #893
{
	//styleデータ無しのデータを取得
	GetTextDrawDataWithoutStyle(inText,inTextInfo,inLineIndex,outTextDrawData);//#695
	
	//行折り返し未計算の場合は、styleデータは付加せずに終了
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial || 
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened ||
	   SPI_GetDocumentInitState() == kDocumentInitState_WrapCalculating )
	{
		return;
	}
	
	//#893
	//text viewのview id指定があれば、view indexを取得
	AIndex	viewIndex = kIndex_Invalid;
	if( inViewID != kObjectID_Invalid )
	{
		viewIndex = mViewIDArray.Find(inViewID);
	}
	
	//R0199
	ABool	enableSpellCheck = false;
	
#if IMPLEMENTATION_FOR_MACOSX
	//スペルチェックするかどうかを取得
	enableSpellCheck = ( /*#688 AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 && */
			GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kDisplaySpellCheck) == true);
#endif

	/*
	//行に色指定がある場合はそちら優先
	AColor	lineColor;
	if( inTextInfo.GetLineColor(inLineIndex,lineColor) == true )
	{
		outTextDrawData.attrPos.Add(0);
		outTextDrawData.attrColor.Add(lineColor);
		outTextDrawData.attrStyle.Add(kTextStyle_Normal);
	}
	else
	*/
	{
		AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex());
		ASyntaxDefinition&	syntaxDefinition = modePrefDB.GetSyntaxDefinition();//R0199
		//行情報取得
		AIndex	lineInfo_Start = inTextInfo.GetLineStart(inLineIndex);
		//#695 終了位置はGetTextDrawDataWithoutStyle()で制限された長さに従うようにするAItemCount	lineInfo_LengthWithoutCR = inTextInfo.GetLineLengthWithoutLineEnd(inLineIndex);
		AIndex	stateIndex, pushedStateIndex;
		inTextInfo.GetLineStateIndex(inLineIndex,stateIndex,pushedStateIndex);
		
		//state color有効フラグをfalseに初期設定する
		ABool	stateColorValid = false;
		//行頭状態に対応した色を取得。状態が色を持たない場合は、モードの文字色を取得。
		AColor	normalColor;
		ATextStyle	normalTextStyle = kTextStyle_Normal;//#844
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(stateIndex,normalColor,normalTextStyle,stateColorValid);
		//#603 見出し正規表現一致グループの色づけ
		AColor	jumpsetupRegExpGroupColor = kColor_Black;
		AIndex	jumpsetupRegExpGroupColorSpos = kIndex_Invalid;
		AIndex	jumpsetupRegExpGroupColorEpos = kIndex_Invalid;
		inTextInfo.GetLineRegExpGroupColor(inLineIndex,jumpsetupRegExpGroupColor,
					jumpsetupRegExpGroupColorSpos,jumpsetupRegExpGroupColorEpos);
		//B0436
		ABool	lineColorValid = false;
		AColor	lineColor;
		if( inTextInfo.GetLineColor(inLineIndex,lineColor) == true )
		{
			normalColor = lineColor;
			lineColorValid = true;
		}
		//
		outTextDrawData.attrPos.Add(0);
		outTextDrawData.attrColor.Add(normalColor);
		outTextDrawData.attrStyle.Add(normalTextStyle);//#844
		//
		AIndex	skipKeywordRecognition = 0;//#592
		//色付け等計算終了位置取得
		//#695 終了位置はGetTextDrawDataWithoutStyle()で制限された長さに従うようにする
		AItemCount	drawDataTextLength = 0;
		if( outTextDrawData.UTF16DrawTextArray_OriginalTextIndex.GetItemCount() > 0 )
		{
			//UTF16DrawTextArray_OriginalTextIndexの最後の要素に、元のテキストのend indexが入っている
			drawDataTextLength = outTextDrawData.UTF16DrawTextArray_OriginalTextIndex.
					Get(outTextDrawData.UTF16DrawTextArray_OriginalTextIndex.GetItemCount()-1);
		}
		AIndex	endpos = lineInfo_Start + drawDataTextLength;//#695 lineInfo_LengthWithoutCR;
		//
		for( ATextIndex pos = lineInfo_Start; pos < endpos; )
		{
			//#603 見出し正規表現一致グループの色づけ
			if( jumpsetupRegExpGroupColorSpos != kIndex_Invalid )
			{
				//現在位置posが見出し正規表現一致グループ色づけ開始位置を初めて超えたとき、attrPos/Color/Styleを追加
				if( pos > lineInfo_Start+jumpsetupRegExpGroupColorSpos )
				{
					outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.
								Get(jumpsetupRegExpGroupColorSpos));
					outTextDrawData.attrColor.Add(jumpsetupRegExpGroupColor);
					outTextDrawData.attrStyle.Add(kTextStyle_Normal);
					jumpsetupRegExpGroupColorSpos = kIndex_Invalid;
				}
			}
			if( jumpsetupRegExpGroupColorEpos != kIndex_Invalid )
			{
				//現在位置posが見出し正規表現一致グループ色づけ終了位置を初めて超えたとき、attrPos/Color/Styleを追加
				if( pos > lineInfo_Start+jumpsetupRegExpGroupColorEpos )
				{
					outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.
								Get(jumpsetupRegExpGroupColorEpos));
					outTextDrawData.attrColor.Add(normalColor);//通常文字色に戻す
					outTextDrawData.attrStyle.Add(normalTextStyle);//#844 
					jumpsetupRegExpGroupColorEpos = kIndex_Invalid;
				}
			}
			//pos位置から1トークン解析
			AIndex	nextStateIndex;
			ABool	changeStateFromNextChar;
			ATextIndex	nextPos;
			ABool	seqFound;
			AText	token;
			ATextIndex	tokenspos;
			AItemCount	currentIndent = 0, nextIndent = 0;
			inTextInfo.RecognizeStateAndIndent(/*#695 mText*/inText,stateIndex,pushedStateIndex,
					pos,endpos,nextStateIndex,changeStateFromNextChar,nextPos,seqFound,token,tokenspos,0,0,currentIndent,nextIndent);
			
			AIndex	currentStateIndex = stateIndex;//R0000
			ABool	nextStateColorValid = stateColorValid;//#274
			ATextIndex	stateChangePos = kIndex_Invalid;//#435 現在位置以外から状態遷移の場合にこの値を設定することにする
			//アクションシーケンスが見つかった場合
			if( seqFound == true )
			{
				//状態遷移
				if( stateIndex != nextStateIndex )
				{
					//状態遷移
					stateIndex = nextStateIndex;
					//状態色取得（状態色がない場合は、モードの文字色取得）
					if( lineColorValid == false )//B0436
					{
						GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetLetterColorForState(stateIndex,normalColor,normalTextStyle,nextStateColorValid);//#274 #844
					}
					//
					//#435 ATextIndex	stateChangePos = pos;
					if( changeStateFromNextChar == true )
					{
						stateChangePos = nextPos;
					}
					//#14 現在の文字列から状態変更の場合は、currentStateIndexを新規状態に変更する
					//</script>の色づけのため。/scriptでjavascirpt状態から、HTMLのタグ内状態へ戻す。
					else
					{
						currentStateIndex = nextStateIndex;
						stateColorValid = nextStateColorValid;//#274
					}
					//#435 changeStateFromNextCharの場合、winキーワードのattr追加後に、追加位置判断して追加することにする
					//#435 if( stateChangePos <= endpos )
					if( stateChangePos == kIndex_Invalid && pos <= endpos )//#435
					{
						//現在位置から状態遷移
						outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(pos-lineInfo_Start));
						outTextDrawData.attrColor.Add(normalColor);
						outTextDrawData.attrStyle.Add(normalTextStyle);//#844
					}
				}
			}
			//R0195 if( stateColorValid == false )
			if( tokenspos >= skipKeywordRecognition )//#633
			{
				//文法パートindexを取得
				AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
				//
				ATextIndex	p = tokenspos;
				GetKeywordToken(p,endpos,token,tokenspos,syntaxPartIndex);
				ABool	found = false;
				
				AColor	color;
				ATextStyle	textstyle;
				ABool	priorToOtherColor;//R0195
				//識別子検索
				AObjectID	objectID = kObjectID_Invalid;
				if( viewIndex != kIndex_Invalid )//#893
				{
					//指定行がローカル範囲内ならローカル識別子を検索
					if( inLineIndex >= mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex) && inLineIndex < mCurrentLocalIdentifierEndLineIndexArray.Get(viewIndex) )
					{
						objectID = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).FindObjectID(token,0,token.GetItemCount());
					}
				}
				if( objectID != kObjectID_Invalid )
				{
					if( (stateColorValid == false && lineColorValid == false) )//R0195 B0436
					{
						AIndex	categoryIndex = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetObjectConstByID(objectID).GetCategoryIndex();
						modePrefDB.GetCategoryLocalColor(categoryIndex,color);
						outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
						outTextDrawData.attrColor.Add(color);
						outTextDrawData.attrStyle.Add(/*R0195 kTextStyle_Normal*/modePrefDB.GetCategoryTextStyle(categoryIndex));
						outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
						outTextDrawData.attrColor.Add(normalColor);
						outTextDrawData.attrStyle.Add(normalTextStyle);//#844
						found = true;
					}
					
					//RC1
					if( ioAnalyzeDrawData.flagArrowToDef == true )
					{
						AIndex	defspos = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetObjectConstByID(objectID).GetStartIndex();
						AIndex	defepos = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetObjectConstByID(objectID).GetEndIndex();
						AIndex	refspos = tokenspos;
						AIndex	refepos = p;
						ATextPoint	defspt, defept, refspt, refept;
						SPI_GetTextPointFromTextIndex(defspos,defspt);
						SPI_GetTextPointFromTextIndex(defepos,defept);
						SPI_GetTextPointFromTextIndex(refspos,refspt);
						SPI_GetTextPointFromTextIndex(refepos,refept);
						ioAnalyzeDrawData.defStartArrowToDef.Add(defspt);
						ioAnalyzeDrawData.defEndArrowToDef.Add(defept);
						ioAnalyzeDrawData.refStartArrowToDef.Add(refspt);
						ioAnalyzeDrawData.refEndArrowToDef.Add(refept);
					}
				}
				else
				{
					//グローバル識別子検索
					AIndex	categoryIndex = inTextInfo.GetGlobalIdentifierCategoryIndexByKeywordText(token);
					if( categoryIndex != kIndex_Invalid )
					{
						if( (stateColorValid == false && lineColorValid == false) )//R0195 B0436
						{
							modePrefDB.GetCategoryColor(categoryIndex,color);
							outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
							outTextDrawData.attrColor.Add(color);
							outTextDrawData.attrStyle.Add(/*R0195 kTextStyle_Normal*/modePrefDB.GetCategoryTextStyle(categoryIndex));
							outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
							outTextDrawData.attrColor.Add(normalColor);
							outTextDrawData.attrStyle.Add(normalTextStyle);//#844
							found = true;
						}
					}
					//キーワード検索
					if( found == false )
					{
						if( modePrefDB.FindKeyword(token,color,textstyle,priorToOtherColor,currentStateIndex) == true )//R0000
						{
							if( (stateColorValid == false && lineColorValid == false) || priorToOtherColor == true )//R0195 B0436
							{
								outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
								outTextDrawData.attrColor.Add(color);
								outTextDrawData.attrStyle.Add(textstyle);
								outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
								outTextDrawData.attrColor.Add(normalColor);
								outTextDrawData.attrStyle.Add(normalTextStyle);//#844
								found = true;
							}
						}
					}
					//インポートファイルキーワード検索
					if( found == false )
					{
						if( mImportIdentifierLinkList.FindKeyword(token,categoryIndex) == true )
						{
							if( categoryIndex != kIndex_Invalid )
							{
								if( (stateColorValid == false && lineColorValid == false) )//R0195 B0436
								{
									modePrefDB.GetCategoryImportColor(categoryIndex,color);
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
									outTextDrawData.attrColor.Add(color);
									outTextDrawData.attrStyle.Add(/*R0195 kTextStyle_Normal*/modePrefDB.GetCategoryTextStyle(categoryIndex));
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
									outTextDrawData.attrColor.Add(normalColor);
									outTextDrawData.attrStyle.Add(normalTextStyle);//#844
									found = true;
								}
							}
						}
					}
					//システムヘッダキーワード検索 #253
					if( found == false )
					{
						if( modePrefDB.GetSystemHeaderIdentifierLinkList().FindKeyword(token,categoryIndex) == true )
						{
							if( categoryIndex != kIndex_Invalid )
							{
								if( (stateColorValid == false && lineColorValid == false) )//R0195 B0436
								{
									modePrefDB.GetCategoryImportColor(categoryIndex,color);
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
									outTextDrawData.attrColor.Add(color);
									outTextDrawData.attrStyle.Add(/*R0195 kTextStyle_Normal*/modePrefDB.GetCategoryTextStyle(categoryIndex));
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
									outTextDrawData.attrColor.Add(normalColor);
									outTextDrawData.attrStyle.Add(normalTextStyle);//#844
									found = true;
								}
							}
						}
					}
					//RegExpキーワード検索 
					if( found == false )
					{
						categoryIndex = mTextInfo.MatchKeywordRegExp(token);
						if( categoryIndex != kIndex_Invalid )
						{
							if( modePrefDB.GetCategoryDisabled(stateIndex,categoryIndex) == false )
							{
								if( (stateColorValid == false && lineColorValid == false) || modePrefDB.GetCategoryPriorToOtherColor(categoryIndex) == true )//R0195 B0436
								{
									modePrefDB.GetCategoryColor(categoryIndex,color);
									textstyle = modePrefDB.GetCategoryTextStyle(categoryIndex);
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
									outTextDrawData.attrColor.Add(color);
									outTextDrawData.attrStyle.Add(textstyle);
									outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
									outTextDrawData.attrColor.Add(normalColor);
									outTextDrawData.attrStyle.Add(normalTextStyle);//#844
									found = true;
								}
							}
						}
					}
					/*色テキストをその色で表示→drop 色によってはテキストが見えない・・・
					if( found == false )
					{
						if( token.GetItemCount() == 6 )
						{
							ABool	isHTMLColor = true;
							for( AIndex i = 0; i < 6; i++ )
							{
								AUChar	ch = token.Get(i);
								if( ch >= '0' && ch <= '9' )
								{
								}
								else if( ch >= 'A' && ch <= 'F' )
								{
								}
								else if( ch >= 'a' && ch <= 'f' )
								{
								}
								else
								{
									isHTMLColor = false;
									break;
								}
							}
							//
							if( isHTMLColor == true )
							{
								outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
								outTextDrawData.attrColor.Add(AColorWrapper::GetColorByHTMLFormatColor(token));
								outTextDrawData.attrStyle.Add(kTextStyle_Normal);
								outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
								outTextDrawData.attrColor.Add(normalColor);
								outTextDrawData.attrStyle.Add(normalTextStyle);
								found = true;
							}
						}
					}
					*/
				}
				
				//ここでの各変数の状態：nextPos:文法解析による次の位置 p:1token読み込み後の位置
				
				//キーワード一致したら、次の読み込み位置は、キーワードの次の文字にする。
				//キーワード一致しない場合は、次の読み込み位置は、（基本的には）文法解析で決定した文字の位置
				if( found == true )
				{
					//B0427 エスケープ文字がバックスラッシュで、キーワードにもバックスラッシュが存在する場合、
					//「\"」という文字列について、文法解析ではnextPosは"のあとにあるが、キーワードが存在することにより\のあとになってしまう
					//文法解析結果の位置をずらす必要はないと思われるので、この処理は削除
					/*
					nextPos = p;
					*/
					//R0199
					//posはループ最後にnextPosで更新される
					//nextPos: 文法解析(RecognizeStateAndIndent)の結果位置
					//p: キーワードtoken取得の結果位置
					//キーワード一致の場合は、キーワードの後にposを持ってこないと、
					//たとえば、texの"\samepage"にて、posが"\"の後になってしまい、次のループでsamepageがミススペル対象になってしまう。
					//nextPosとpを比べて、より後のほうの位置でpos更新したい。
					//nextPosの方が後ならそのままでOK、nextPosの方が先ならpで更新する（キーワードtokenの後の位置を採用する。）
					/*#592
					ここでnextPosを更新してしまうと、文法解析がただしく行われない。（文字を抜かして文法認識してしまう）
					if( nextPos < p )
					{
						nextPos = p;
					}
					*/
					//skipKeywordRecognition: 次以降のループで、この位置を超えるまで、キーワード認識処理は行わない
					//nextPosは、文法認識による次位置であるため、1tokenよりも前になることがある。
					//この場合、文法認識を正しく行い、かつ、途中からキーワード検索しないようにする必要がある。
					skipKeywordRecognition = p;//#592 #633 skipKeywordRecognitionを超えるまではキーワード認識を無効にする #633
				}
				//R0194
				else
				{
					//マルチバイト文字の場合は、nextPosは、tokenの最後にする。
					if( token.GetItemCount() > 0 )
					{
						AUChar	ch = token.Get(0);
						if( token.IsUTF8MultiByte(ch) == true )
						{
							nextPos = p;
						}
					}
				}
#if IMPLEMENTATION_FOR_MACOSX
				//R0199 自動スペルチェック
				//キーワードなし、かつ、設定On、かつ、tokenの文字数が2以上（1文字はチェック対象外）の場合に実施
				if( found == false && enableSpellCheck == true && token.GetItemCount() > 1 &&
							syntaxDefinition.GetSyntaxDefinitionState(currentStateIndex).IsNoSpellCheck() == false )
				{
					//U+2000未満の文字で始まる単語のみチェック対象
					if( outTextDrawData.UTF16DrawText.GetUTF16CharFromUTF16Text(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start)*sizeof(AUTF16Char)) < 0x2000 )
					{
						//#371 アポストロフィでスペルエラーになる問題を修正
						//単語の直後にアポストロフィ＋アルファベットが存在していたらそれらを追加してスペルチェック
						if( p < endpos )
						{
							ATextIndex	p2 = p, tokenspos2;
							AText	token2;
							GetKeywordToken(p2,endpos,token2,tokenspos2,syntaxPartIndex);
							if( token2.Compare("'") == true )
							{
								if( p2 < endpos )
								{
									ATextIndex	p3 = p2, tokenspos3;
									AText	token3;
									GetKeywordToken(p3,endpos,token3,tokenspos3,syntaxPartIndex);
									{
										if( token3.GetItemCount() > 0 )
										{
											if( GetApp().SPI_GetModePrefDB(mModeIndex).IsAlphabet(token3.Get(0),syntaxPartIndex) == true )
											{
												p = p3;
												token.AddText(token2);
												token.AddText(token3);
											}
										}
									}
								}
							}
						}
						//
						ABool	misspell = false;
						//ミススペルキャッシュにあればミススペルと判断
						if( mMisspellCacheArray.Find(token) != kIndex_Invalid )
						{
							misspell = true;
						}
						//正しいスペルキャッシュにあればミススペルでないと判断
						else if( mCorrectspellCacheArray.Find(token) != kIndex_Invalid )
						{
							misspell = false;
						}
						//キャッシュで判断できない場合のみスペルチェック実施
						else
						{
							//スペルチェック
							misspell = (ASpellCheckerWrapper::CheckSpell(outTextDrawData.UTF16DrawText,
									outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start),
									outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start),
									modePrefDB.GetModeData_Number(AModePrefDB::kSpellCheckLanguage)) == false);
							//キャッシュ
							if( misspell == true )
							{
								//キャッシュ追加
								mMisspellCacheArray.Add(token);
								//キャッシュサイズオーバーなら最初の方を削除
								if( mMisspellCacheArray.GetItemCount() > kMisspellCacheSize )
								{
									while( mMisspellCacheArray.GetItemCount() > kMisspellCacheSize-kMisspellCacheDeleteSize )
									{
										mMisspellCacheArray.Delete1(0);
									}
								}
							}
							else
							{
								//キャッシュ追加
								mCorrectspellCacheArray.Add(token);
								//キャッシュサイズオーバーなら最初の方を削除
								if( mCorrectspellCacheArray.GetItemCount() > kMisspellCacheSize )
								{
									while( mCorrectspellCacheArray.GetItemCount() > kMisspellCacheSize-kMisspellCacheDeleteSize )
									{
										mCorrectspellCacheArray.Delete1(0);
									}
								}
							}
							//fprintf(stderr,"%d,%d\n",mMisspellCacheArray.GetItemCount(),mCorrectspellCacheArray.GetItemCount());
						}
						//描画データ
						if( misspell == true )
						{
							outTextDrawData.misspellStartArray.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos-lineInfo_Start));
							outTextDrawData.misspellEndArray.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(p-lineInfo_Start));
						}
					}
				}
#endif
			}
			//#435 changeStateFromNextCharの場合の、状態遷移attr追加
			if( stateChangePos != kIndex_Invalid )
			{
				AIndex	utf8pos = stateChangePos-lineInfo_Start;
				if( utf8pos >= outTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount() )
				{
					//状態変化位置が次の行になる場合があるので、値補正
					utf8pos = outTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount()-1;
				}
				outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.Get(utf8pos));
				outTextDrawData.attrColor.Add(normalColor);
				outTextDrawData.attrStyle.Add(normalTextStyle);//#844
			}
			//#274
			stateColorValid = nextStateColorValid;
			//pos更新
			pos = nextPos;
		}
		//#603
		if( jumpsetupRegExpGroupColorEpos != kIndex_Invalid )
		{
			//現在位置posが見出し正規表現一致グループ色づけ終了位置を初めて超えたとき、attrPos/Color/Styleを追加
			outTextDrawData.attrPos.Add(outTextDrawData.OriginalTextArray_UnicodeOffset.
						Get(jumpsetupRegExpGroupColorEpos));
			outTextDrawData.attrColor.Add(normalColor);//通常文字色に戻す
			outTextDrawData.attrStyle.Add(normalTextStyle);//#844
			jumpsetupRegExpGroupColorEpos = kIndex_Invalid;
		}
	}
	//選択表示
	outTextDrawData.drawSelection = false;
	if( inSelectMode == true )
	{
		//ほかのViewでのテキスト編集を反映させる場合等、どうしても選択範囲がイリーガルになることがありうるので、
		//イリーガルの場合は選択範囲表示はさせないようにする。
		if( IsTextPointValid(inSelectTextPoint1) == true && IsTextPointValid(inSelectTextPoint2) == true )
		{
			ATextPoint	spt, ept;
			ADocument_Text::OrderTextPoint(inSelectTextPoint1,inSelectTextPoint2,spt,ept);
			if( spt.y == inLineIndex && ept.y == inLineIndex )
			{
				outTextDrawData.drawSelection = true;
				outTextDrawData.selectionStart = outTextDrawData.OriginalTextArray_UnicodeOffset.Get(spt.x);
				outTextDrawData.selectionEnd = outTextDrawData.OriginalTextArray_UnicodeOffset.Get(ept.x);
			}
			else if( spt.y < inLineIndex && ept.y == inLineIndex )
			{
				outTextDrawData.drawSelection = true;
				outTextDrawData.selectionStart = kIndex_Invalid;
				outTextDrawData.selectionEnd = outTextDrawData.OriginalTextArray_UnicodeOffset.Get(ept.x);
			}
			else if( spt.y < inLineIndex && ept.y > inLineIndex )
			{
				outTextDrawData.drawSelection = true;
				outTextDrawData.selectionStart = outTextDrawData.OriginalTextArray_UnicodeOffset.Get(0);
				outTextDrawData.selectionEnd = kIndex_Invalid;
			}
			else if( spt.y == inLineIndex && ept.y > inLineIndex )
			{
				outTextDrawData.drawSelection = true;
				outTextDrawData.selectionStart = outTextDrawData.OriginalTextArray_UnicodeOffset.Get(spt.x);
				outTextDrawData.selectionEnd = kIndex_Invalid;
			}
		}
	}
	//ModuleComment RC3
	/*#138
	if( ioAnalyzeDrawData.flagModuleComment == true )
	{
		ioAnalyzeDrawData.moduleComment.DeleteAll();
		AIndex	identifierLineIndex = kIndex_Invalid;
		AObjectID	objectID = mTextInfo.GetMenuIdentifierByLineIndex(inLineIndex,identifierLineIndex);
		if( objectID != kIndex_Invalid )
		{
			AText	moduleName;
			//#138 mTextInfo.GetGlobalIdentifierKeywordText(objectID,moduleName);
			//#138 if( moduleName.GetItemCount() == 0 )
			//#138 {
			mTextInfo.GetGlobalIdentifierMenuText(objectID,moduleName);
			//#138 }
			SPI_GetExternalComment(moduleName,inLineIndex-identifierLineIndex,ioAnalyzeDrawData.moduleComment);
		}
	}
	*/
}
//
ABool	ADocument_Text::GetKeywordToken( AIndex& ioPos, const AIndex inEndPos, AText& outToken, AIndex& outTokenStartPos,
		const AIndex inSyntaxPartIndex ) const
{
	outToken.DeleteAll();
	
	if( mText.SkipTabSpace(ioPos,inEndPos) == false )   return false;
	
	outTokenStartPos = ioPos;
	
	//単語検索（outTokenStartPosより後方向にのみ検索、アルファベット判定はキーワード用の判定（＝モード設定orSDFを使う））
	ATextIndex	start = 0, end = 0;
	ATextInfo::FindWord(mText,mModeIndex,true,outTokenStartPos,start,end,true,inSyntaxPartIndex);
	//FindWord()は行終了を考慮せずに単語終了位置を取得するので、endがinEndPosより後なら、inEndPosを終了位置にする
	if( end > inEndPos )
	{
		end = inEndPos;
	}
	//終了位置をioPosに格納
	ioPos = end;
	//テキスト取得
	mText.GetText(outTokenStartPos,end-outTokenStartPos,outToken);
	return true;
	
	/*#R0199 
	AUChar	ch = mText.Get(ioPos);
	ioPos++;
	outToken.Add(ch);
	*/
	/*#単語選択ロジックの共通化
	//
	const AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(mModeIndex);
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();//R0199
	
	//=====================最初の文字の判定=====================
	
	//R0199 アスキー外アルファベットは無条件でアルファベット扱い
	//読み込み
	AUTF16Char	utf16char;
	AItemCount	bc = mText.Convert1CharToUTF16(ioPos,utf16char);
	AUChar	ch = mText.Get(ioPos);
	//アルファベット判定
	AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(utf16char);
	if( bc == 1 )
	{
		//=====================最初の文字がアスキー内アルファベットの場合=====================
		
		//アスキー内の場合：文法定義のアルファベット以外ならここ（アルファベット以外の文字の前）で終了
		if( (modePrefDB.IsAlphabet(ch) == false) )
		{
			outToken.Add(ch);//#371
			ioPos++;//#371
			return true;
		}
	}
	else if( category == kUnicodeCharCategory_LatinAndNumber )
	{
		//=====================最初の文字がアスキー外アルファベットの場合=====================
		
		//アスキー外のLatinAndNumberの場合、ノーチェック
	}
	else
	{
		//=====================最初の文字がアスキー外ノンアルファベット文字の場合=====================
		
		//アスキー外の場合：
		//R0194
		AIndex	spos = ioPos;//R0199 -1;
		AUTF16Char	utf16char;
		ioPos += mText.Convert1CharToUTF16(spos,utf16char);//R0199 -1;
		AUnicodeCharCategory	firstCharCategory = unicodeData.GetUnicodeCharCategory(utf16char);
		if( firstCharCategory != kUnicodeCharCategory_Others )
		{
			AItemCount	uniCharCount = 1;
			for( ; ioPos < inEndPos;  )
			{
				AItemCount	bc = mText.Convert1CharToUTF16(ioPos,utf16char);
				AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(utf16char);
				if( firstCharCategory != category )
				{
					if( (firstCharCategory==kUnicodeCharCategory_ZenkakuHiragana||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) &&
								category == kUnicodeCharCategory_ZenkakuOtohiki )
					{
						//継続
					}
					else
					{
						break;
					}
				}
				ioPos += bc;
				uniCharCount++;
				if( uniCharCount == 32 )   break;
			}
		}
		if( ioPos > inEndPos )   ioPos = inEndPos;
		mText.GetText(spos,ioPos-spos,outToken);
		return true;
	}
	//Unicode 1文字格納
	for( AIndex i = 0; i < bc; i++ )
	{
		if( ioPos >= mText.GetItemCount() )   break;//マルチファイル検索等でバイナリファイルを読んだ場合などの対策
		outToken.Add(mText.Get(ioPos));
		ioPos++;
	}
	
	//B0444 tail判定
	if( bc == 1 )
	{
		if( (modePrefDB.IsTailAlphabet(ch) == true) )   return true;
	}
	
	//
	while( ioPos < inEndPos )
	{
		//読み込み
		bc = mText.Convert1CharToUTF16(ioPos,utf16char);
		ch = mText.Get(ioPos);
		category = unicodeData.GetUnicodeCharCategory(utf16char);
		//アルファベット判定
		if( bc == 1 )
		{
			//アスキー文字内の場合：文法定義のアルファベット以外ならここ（アルファベット以外の文字の前）で終了
			if( modePrefDB.IsContinuousAlphabet(ch) == false )   return true;
		}
		else if( category == kUnicodeCharCategory_LatinAndNumber )
		{
			//アスキー外のLatinAndNumberの場合、ノーチェック
		}
		else
		{
			//上記以外のマルチバイト文字：マルチバイト文字の前で終了
			return true;
		}
		//Unicode 1文字格納
		for( AIndex i = 0; i < bc; i++ )
		{
			if( ioPos >= mText.GetItemCount() )   break;//マルチファイル検索等でバイナリファイルを読んだ場合などの対策
			outToken.Add(mText.Get(ioPos));
			ioPos++;
		}
		//tail判定
		if( bc == 1 )
		{
			if( (modePrefDB.IsTailAlphabet(ch) == true) )   return true;
		}
	}
	return true;
	*/
	/*#
	//UTF8マルチバイト文字の場合は、Unicode１文字分読んでリターン
	if( mText.IsUTF8MultiByte(ch) == true )
	{
		//R0194
		AIndex	spos = ioPos-1;
		AUTF16Char	utf16char;
		ioPos += mText.Convert1CharToUTF16(spos,utf16char) -1;
		const AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
		AUnicodeCharCategory	firstCharCategory = unicodeData.GetUnicodeCharCategory(utf16char);
		if( firstCharCategory != kUnicodeCharCategory_Others )
		{
			AItemCount	uniCharCount = 1;
			for( ; ioPos < inEndPos;  )
			{
				AItemCount	bc = mText.Convert1CharToUTF16(ioPos,utf16char);
				AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(utf16char);
				if( firstCharCategory != category )
				{
					if( (firstCharCategory==kUnicodeCharCategory_ZenkakuHiragana||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) &&
								category == kUnicodeCharCategory_ZenkakuOtohiki )
					{
						//継続
					}
					else
					{
						break;
					}
				}
				ioPos += bc;
				uniCharCount++;
				if( uniCharCount == 32 )   break;
			}
		}
		if( ioPos > inEndPos )   ioPos = inEndPos;
		mText.GetText(spos,ioPos-spos,outToken);
		//outToken.OutputToStderr();
		//fprintf(stderr, "   ");
		*AItemCount	bc = mText.GetUTF8ByteCount(ch);
		for( AIndex i = 0; i < bc-1; i++ )
		{
			outToken.Add(mText.Get(ioPos));
			ioPos++;
		}*
		return true;
	}
	
	//
	const AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(mModeIndex);
	//
	if( (modePrefDB.IsAlphabet(ch) == false) )   return true;
	
	while( ioPos < inEndPos )
	{
		ch = mText.Get(ioPos);
		if( modePrefDB.IsContinuousAlphabet(ch) == false )   return true;
		
		ioPos++;
		outToken.Add(ch);
		if( (modePrefDB.IsTailAlphabet(ch) == true) )   return true;
	}
	return true;
	*/
}


#pragma mark ===========================

#pragma mark ---テキスト情報取得

//TextPointが有効かどうかを返す
ABool	ADocument_Text::IsTextPointValid( const ATextPoint& inTextPoint ) const
{
	if( inTextPoint.y < 0 || inTextPoint.y >= SPI_GetLineCount() )   return false;
	if( inTextPoint.x < 0 || inTextPoint.x > SPI_GetLineLengthWithoutLineEnd(inTextPoint.y) )   return false;
	return true;
}

//テキスト取得（inStart>inEndでも可）
void	ADocument_Text::GetText( const ATextIndex inStart, const ATextIndex inEnd, AText& outText ) const
{
	ATextIndex	start = inStart;
	ATextIndex	end = inEnd;
	if( inEnd < inStart )
	{
		start = inEnd;
		end = inStart;
	}
	{
		mText.GetText(start,end-start,outText);
	}
}
/**
テキスト取得
@note thread-safe
*/
void	ADocument_Text::SPI_GetText( const ATextIndex inStart, const ATextIndex inEnd, AText& outText ) const
{
	AStMutexLocker	locker(mTextMutex);//#598
	GetText(inStart,inEnd,outText);
}
void	ADocument_Text::SPI_GetText( const ATextPoint& inStart, const ATextPoint& inEnd, AText& outText ) const
{
	AStMutexLocker	locker(mTextMutex);//#598
	GetText(SPI_GetTextIndexFromTextPoint(inStart),SPI_GetTextIndexFromTextPoint(inEnd),outText);
}
void	ADocument_Text::SPI_GetText( AText& outText ) const
{
	AStMutexLocker	locker(mTextMutex);//#598
	GetText(0,mText.GetItemCount(),outText);
}

//#450
/**
指定行のテキストを取得
@note 改行コード含む。
*/
void	ADocument_Text::SPI_GetLineText( const AIndex inLineIndex, AText& outText ) const
{
	AIndex	spos = SPI_GetLineStart(inLineIndex);
	AIndex	epos = spos + SPI_GetLineLengthWithLineEnd(inLineIndex);
	SPI_GetText(spos,epos,outText);
}

//#750
/**
指定text index以降の最初の改行位置text indexを取得
*/
AIndex	ADocument_Text::SPI_GetNextLineEndCharIndex( const AIndex inStartTextIndex ) const
{
	AItemCount	len = mText.GetItemCount();
	for( AIndex pos = inStartTextIndex; pos < len; pos++ )
	{
		if( mText.Get(pos) == kUChar_LineEnd )
		{
			return pos;
		}
	}
	return len;
}

/**
前のunicode文字の開始位置のtext indexを取得
@param inSkipCanonicalDecomp decomp文字をスキップするかどうか（デフォルト；スキップする）
*/
AIndex	ADocument_Text::SPI_GetPrevCharTextIndex( const ATextIndex inIndex, const ABool inSkipCanonicalDecomp,
												 const ABool inReturnMinusWhen0 ) const
{
	if( inSkipCanonicalDecomp == true )
	{
		//#863
		//Unicode定義に従い、decomp文字はスキップして、前の文字のtext indexを取得
		return GetApp().NVI_GetUnicodeData().GetPrevCharPosSkipCanonicalDecomp(mText,inIndex,inReturnMinusWhen0);
	}
	else
	{
		return mText.GetPrevCharPos(inIndex,inReturnMinusWhen0);
	}
}

/**
次のunicode文字の開始位置のtext indexを取得
@param inSkipCanonicalDecomp decomp文字をスキップするかどうか（デフォルト；スキップする）
*/
AIndex	ADocument_Text::SPI_GetNextCharTextIndex( const ATextIndex inIndex, const ABool inSkipCanonicalDecomp ) const
{
	if( inSkipCanonicalDecomp == true )
	{
		//#863
		//Unicode定義に従い、decomp文字はスキップして、次の文字のtext indexを取得
		return GetApp().NVI_GetUnicodeData().GetNextCharPosSkipCanonicalDecomp(mText,inIndex);
	}
	else
	{
		return mText.GetNextCharPos(inIndex);
	}
}

//
AIndex	ADocument_Text::SPI_GetCurrentCharTextIndex( const ATextIndex inIndex ) const
{
	return mText.GetCurrentCharPos(inIndex);
}

//
ATextIndex	ADocument_Text::SPI_GetParagraphStartTextIndexFromLineIndex( const AIndex inLineIndex ) const
{
	return SPI_GetLineStart(mTextInfo.GetCurrentParagraphStartLineIndex(inLineIndex));
}

//
AIndex	ADocument_Text::SPI_GetCurrentParagraphStartLineIndex( const AIndex inLineIndex ) const
{
	return mTextInfo.GetCurrentParagraphStartLineIndex(inLineIndex);
}

//
AIndex	ADocument_Text::SPI_GetNextParagraphStartLineIndex( const AIndex inLineIndex ) const
{
	return mTextInfo.GetNextParagraphStartLineIndex(inLineIndex);
}

ATextIndex	ADocument_Text::SPI_GetParagraphStartTextIndex( const AIndex inParagraphIndex ) const
{
	return mTextInfo.GetParagraphStartTextIndex(inParagraphIndex);
}

//行Indexから段落Indexを取得
AIndex	ADocument_Text::SPI_GetParagraphIndexByLineIndex( const AIndex inLineIndex ) const
{
	return mTextInfo.GetParagraphIndexByLineIndex(inLineIndex);
}

//段落Indexから行Indexを取得
AIndex	ADocument_Text::SPI_GetLineIndexByParagraphIndex( const AIndex inParagraphIndex ) const
{
	return mTextInfo.GetLineIndexByParagraphIndex(inParagraphIndex);
}

//#465
/**
*/
void	ADocument_Text::SPI_GetParagraphTextByLineIndex( const AIndex inLineIndex, AText& outText ) const
{
	AIndex	startLineIndex = SPI_GetCurrentParagraphStartLineIndex(inLineIndex);
	AIndex	endLineIndex = startLineIndex;
	for( ; endLineIndex < SPI_GetLineCount(); endLineIndex++ )
	{
		if( SPI_GetLineExistLineEnd(endLineIndex) == true )
		{
			endLineIndex++;
			break;
		}
	}
	ATextPoint	spt = {0};
	spt.x = 0;
	spt.y = startLineIndex;
	ATextPoint	ept = {0};
	ept.x = SPI_GetLineLengthWithLineEnd(endLineIndex-1);//#0 最終行の場合を考慮し、(0,endLineIndex+1)とはせずに、左記のようにしている
	ept.y = endLineIndex-1;
	SPI_GetText(spt,ept,outText);
}

//#379
/**
指定パラグラフのテキストを取得
*/
void	ADocument_Text::SPI_GetParagraphText( const AIndex inParagraphIndex, AText& outText ) const
{
	AIndex	startLineIndex = SPI_GetLineIndexByParagraphIndex(inParagraphIndex);
	AIndex	endLineIndex = startLineIndex;
	for( ; endLineIndex < SPI_GetLineCount(); endLineIndex++ )
	{
		if( SPI_GetLineExistLineEnd(endLineIndex) == true )
		{
			endLineIndex++;
			break;
		}
	}
	ATextPoint	spt = {0};
	spt.x = 0;
	spt.y = startLineIndex;
	ATextPoint	ept = {0};
	ept.x = SPI_GetLineLengthWithLineEnd(endLineIndex-1);//#0 最終行の場合を考慮し、(0,endLineIndex+1)とはせずに、左記のようにしている
	ept.y = endLineIndex-1;
	SPI_GetText(spt,ept,outText);
}

//#996
/**
段落の長さ取得（改行コード含まず）
*/
AItemCount	ADocument_Text::SPI_GetParagraphLengthWithoutLineEnd( const AIndex inParagraphIndex ) const
{
	AIndex	startLineIndex = SPI_GetLineIndexByParagraphIndex(inParagraphIndex);
	AIndex	endLineIndex = startLineIndex;
	for( ; endLineIndex < SPI_GetLineCount(); endLineIndex++ )
	{
		if( SPI_GetLineExistLineEnd(endLineIndex) == true )
		{
			endLineIndex++;
			break;
		}
	}
	ATextPoint	spt = {0};
	spt.x = 0;
	spt.y = startLineIndex;
	ATextPoint	ept = {0};
	ept.x = SPI_GetLineLengthWithLineEnd(endLineIndex-1);//#0 最終行の場合を考慮し、(0,endLineIndex+1)とはせずに、左記のようにしている
	ept.y = endLineIndex-1;
	return SPI_GetTextIndexFromTextPoint(ept) - SPI_GetTextIndexFromTextPoint(spt);
}

//inTextIndexから、UTF8文字inCharOffset文字分進んだ位置のTextIndexを返す
ATextIndex	ADocument_Text::SPI_GetTextIndexByUTF8CharOffset( const ATextIndex inTextIndex, const AItemCount inCharOffset ) const
{
	ATextIndex pos = inTextIndex;
	AItemCount	charOffset = inCharOffset;
	while( charOffset > 0 && pos < mText.GetItemCount() )
	{
		pos = mText.GetNextCharPos(pos);
		charOffset--;
	}
	return pos;
}

//段落数取得
AItemCount	ADocument_Text::SPI_GetParagraphCount() const
{
	return SPI_GetParagraphIndexByLineIndex(SPI_GetLineCount()-1)+1;
}

/**
段落開始行かどうかを取得
*/
ABool	ADocument_Text::SPI_IsParagraphStartLine( const AIndex inLineIndex ) const
{
	ABool	isParagraphStart = true;
	if( inLineIndex > 0 )
	{
		isParagraphStart = mTextInfo.GetLineExistLineEnd(inLineIndex-1);
	}
	return isParagraphStart;
}

ABool	ADocument_Text::SPI_IsUTF8Multibyte( const ATextIndex inIndex ) const
{
	return mText.IsUTF8MultiByte(inIndex);
}

AUChar	ADocument_Text::SPI_GetTextChar( const ATextIndex inIndex ) const
{
	return mText.Get(inIndex);
}

void	ADocument_Text::SPI_GetText1CharText( const ATextIndex inIndex, AText& outText ) const
{
	ATextIndex	epos = mText.GetNextCharPos(inIndex);
	mText.GetText(inIndex,epos-inIndex,outText);
}

//#142
/**
指定text indexのunicode文字を取得
*/
AUCS4Char	ADocument_Text::SPI_GetText1UCS4Char( const ATextIndex inIndex ) const
{
	AUCS4Char	ch = 0;
	mText.Convert1CharToUCS4(inIndex,ch);
	return ch;
}

//#142
/**
指定text indexのunicode文字を取得（次にdecomp文字が続く場合は、その文字も取得）
*/
void	ADocument_Text::SPI_GetText1UCS4Char( const ATextIndex inIndex, AUCS4Char& outChar, AUCS4Char& outCanonicalDecomp2ndChar ) const
{
	//結果初期化
	outChar = 0;
	outCanonicalDecomp2ndChar = 0;
	//unicode文字取得
	AItemCount	bc = mText.Convert1CharToUCS4(inIndex,outChar);
	//次の文字がテキスト内かの判定
	if( inIndex+bc < mText.GetItemCount() )
	{
		//次の文字を取得
		AUCS4Char	nextchar = 0;
		mText.Convert1CharToUCS4(inIndex+bc,nextchar);
		//decomp文字ならoutCanonicalDecomp2ndCharに格納
		if( GetApp().NVI_GetUnicodeData().IsCanonicalDecomp2ndChar(nextchar) == true )
		{
			outCanonicalDecomp2ndChar = nextchar;
		}
	}
}

//B0424
/**
単語検索（モード設定によってキーワード単語にするかどうかを決定）
@note 単語がアルファベットかマルチバイトならtrueを返す
*/
ABool	ADocument_Text::SPI_FindWord( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const
{
	//指定text indexの文法パートの取得
	ATextPoint	spt = {0};
	SPI_GetTextPointFromTextIndex(inStartPos,spt);
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition();
	AIndex	stateIndex = SPI_GetCurrentStateIndex(spt);//SPI_GetStateIndexByLineIndex(spt.y);
	AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
	//単語検索（前方向含む。アルファベット判定がモード設定orSDFに従うか、unicodeの判定のみに従うか、はモード設定に従う）
	return ATextInfo::FindWord(mText,mModeIndex,false,inStartPos,outStart,outEnd,
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kApplyCharsForWordToDoubleClick),
		syntaxPartIndex);
}

//B0424
/**
単語検索（キーワード単語）
@note 単語がアルファベットかマルチバイトならtrueを返す
*/
ABool	ADocument_Text::SPI_FindWordForKeyword( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const
{
	//指定text indexの文法パートの取得
	ATextPoint	spt = {0};
	SPI_GetTextPointFromTextIndex(inStartPos,spt);
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition();
	AIndex	stateIndex = SPI_GetCurrentStateIndex(spt);//SPI_GetStateIndexByLineIndex(spt.y);
	AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
	//単語検索（後方向のみ検索。アルファベット判定はモード設定orSDFに従う。）
	return ATextInfo::FindWord(mText,mModeIndex,false,inStartPos,outStart,outEnd,true,syntaxPartIndex);
}

void	ADocument_Text::SPI_FindQuadWord( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const
{
	AText	text;
	GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Text(AModePrefDB::kQuadClickText,text);
	for( outStart = mText.GetPrevCharPos(inStartPos); outStart > 0; outStart = mText.GetPrevCharPos(outStart) )
	{
		ABool	found = false;
		for( AIndex i = 0; i < text.GetItemCount();  )
		{
			AText	t;
			text.Get1UTF8Char(i,t);
			if( outStart+t.GetItemCount() > mText.GetItemCount() )   continue;
			if( t.Compare(mText,outStart,t.GetItemCount()) == true )
			{
				found = true;
				break;
			}
		}
		if( found == true )
		{
			outStart = mText.GetNextCharPos(outStart);
			break;
		}
	}
	if( outStart < 0 )   outStart = 0;
	for( outEnd = inStartPos; outEnd < mText.GetItemCount(); outEnd = mText.GetNextCharPos(outEnd) )
	{
		ABool	found = false;
		for( AIndex i = 0; i < text.GetItemCount();  )
		{
			AText	t;
			text.Get1UTF8Char(i,t);
			if( outEnd+t.GetItemCount() > mText.GetItemCount() )   continue;
			if( t.Compare(mText,outEnd,t.GetItemCount()) == true )
			{
				found = true;
				break;
			}
		}
		if( found == true )   break;
	}
}

ABool	ADocument_Text::SPI_IsAsciiAlphabet( const ATextIndex inPos ) const
{
	AUChar ch = mText.Get(inPos);
	return Macro_IsAlphabet(ch);
	/*#416 ダブルクリック動作が変わってしまうので、とりあえず、下記は保留
	AIndex	curPos = mText.GetCurrentCharPos(inPos);//inPosがUTF-8の途中でコールしている箇所があるので位置補正
	AUCS4Char	ch = 0;
	mText.Convert1CharToUCS4(curPos,ch);
	return GetApp().NVI_GetUnicodeData().IsAlphabetOrNumber(ch);
	*/
}

AItemCount	ADocument_Text::SPI_GetLineCount() const
{
	return mTextInfo.GetLineCount();
}

AIndex	ADocument_Text::SPI_GetLineLengthWithoutLineEnd( const AIndex inLineIndex ) const
{
	return mTextInfo.GetLineLengthWithoutLineEnd(inLineIndex);
}

AIndex	ADocument_Text::SPI_GetLineLengthWithLineEnd( const AIndex inLineIndex ) const
{
	return mTextInfo.GetLineLengthWithLineEnd(inLineIndex);
}

ABool	ADocument_Text::SPI_GetLineExistLineEnd( const AIndex inLineIndex ) const
{
	return mTextInfo.GetLineExistLineEnd(inLineIndex);
}

ATextIndex	ADocument_Text::SPI_GetLineStart( const AIndex inLineIndex ) const
{
	if( inLineIndex < SPI_GetLineCount() )
	{
		return mTextInfo.GetLineStart(inLineIndex);
	}
	else
	{
		return SPI_GetTextLength();
	}
}

void	ADocument_Text::SPI_GetGlobalIdentifierRange( const AUniqID inIdentifierUniqID, ATextPoint& outStart, ATextPoint& outEnd ) const
{
	mTextInfo.GetGlobalIdentifierRange(inIdentifierUniqID,outStart,outEnd);
}

//
void	ADocument_Text::SPI_UpdateCurrentIdentifierDefinitionList( const AText& inText )
{
	mIdentifierDefinitionList_InfoText.DeleteAll();
	mIdentifierDefinitionList_File.DeleteAll();
	mIdentifierDefinitionList_StartTextIndex.DeleteAll();
	mIdentifierDefinitionList_Length.DeleteAll();
	//global
	AArray<AUniqID>	UniqIDArray;
	mTextInfo.GetGlobalIdentifierListByKeyword(inText,UniqIDArray);
	AFileAcc	file;
	NVI_GetFile(file);
	for( AIndex i = 0; i < UniqIDArray.GetItemCount(); i++ )
	{
		AUniqID	UniqID = UniqIDArray.Get(i);
		AText	categoryName;
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(mTextInfo.GetGlobalIdentifierCategoryIndex(UniqID),categoryName);
		AText	infotext;
		infotext.SetCstring("[");
		infotext.AddText(categoryName);
		infotext.AddCstring("] ");
		AText	info;
		mTextInfo.GetGlobalIdentifierInfoText(UniqID,info);
		if( info.GetItemCount() > 0 )
		{
			infotext.AddText(info);
		}
		else//R0241
		{
			infotext.AddText(inText);
		}
		ATextPoint	spt, ept;
		mTextInfo.GetGlobalIdentifierRange(UniqID,spt,ept);
		ATextIndex	spos = SPI_GetTextIndexFromTextPoint(spt);
		ATextIndex	epos = SPI_GetTextIndexFromTextPoint(ept);
		mIdentifierDefinitionList_InfoText.Add(infotext);
		mIdentifierDefinitionList_File.GetObject(mIdentifierDefinitionList_File.AddNewObject()).CopyFrom(file);
		mIdentifierDefinitionList_StartTextIndex.Add(spos);
		mIdentifierDefinitionList_Length.Add(epos-spos);
	}
	//import
	AArray<AIndex>	categoryArray;
	ATextArray	infotextArray, commentArray, parentArray;
	AObjectArray<AFileAcc>	fileArray;
	AArray<ATextIndex>	sposArray, eposArray;
	mImportIdentifierLinkList.GetIdentifierListByKeyword(
				inText,categoryArray,infotextArray,fileArray,sposArray,eposArray,commentArray,parentArray);
	GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().GetIdentifierListByKeyword(
			inText,categoryArray,infotextArray,fileArray,sposArray,eposArray,commentArray,parentArray);//#253
	for( AIndex i = 0; i < categoryArray.GetItemCount(); i++ )
	{
		if( categoryArray.Get(i) == kIndex_Invalid )   continue;//要調査？
		if( fileArray.GetObjectConst(i).Compare(file) == true )   continue;//win 080722 自ファイルの識別子はglobalで追加済みのはずなのでパス
		AText	categoryName;
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryArray.Get(i),categoryName);
		AText	infotext;
		AText	filename;
		fileArray.GetObjectConst(i).GetFilename(filename);
		infotext.SetCstring("(");
		infotext.AddText(filename);
		infotext.AddCstring(") ");
		infotext.AddCstring("[");
		infotext.AddText(categoryName);
		infotext.AddCstring("] ");
		AText	info;
		infotextArray.Get(i,info);
		if( info.GetItemCount() > 0 )
		{
			infotext.AddText(info);
		}
		else//R0241
		{
			infotext.AddText(inText);
		}
		ATextIndex	spos = sposArray.Get(i);
		ATextIndex	epos = eposArray.Get(i);
		mIdentifierDefinitionList_InfoText.Add(infotext);
		mIdentifierDefinitionList_File.GetObject(mIdentifierDefinitionList_File.AddNewObject()).CopyFrom(fileArray.GetObjectConst(i));
		mIdentifierDefinitionList_StartTextIndex.Add(spos);
		mIdentifierDefinitionList_Length.Add(epos-spos);
	}
	/*#893
	//local
	mTextInfo.GetLocalIdentifierListByKeyword(inText,UniqIDArray);
	for( AIndex i = 0; i < UniqIDArray.GetItemCount(); i++ )
	{
		AUniqID	UniqID = UniqIDArray.Get(i);
		AText	categoryName;
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(mTextInfo.GetLocalIdentifierCategoryIndex(UniqID),categoryName);
		AText	infotext;
		infotext.SetCstring("(local) ");
		AText	info;
		mTextInfo.GetLocalIdentifierInfoText(UniqID,info);
		if( info.GetItemCount() > 0 )
		{
			infotext.AddText(info);
		}
		else//R0241
		{
			infotext.AddText(inText);
		}
		ATextPoint	spt, ept;
		mTextInfo.GetLocalIdentifierRange(UniqID,spt,ept);
		if( spt.y >= mCurrentLocalIdentifierStartLineIndex && ept.y < mCurrentLocalIdentifierEndLineIndex )
		{
			ATextIndex	spos = SPI_GetTextIndexFromTextPoint(spt);
			ATextIndex	epos = SPI_GetTextIndexFromTextPoint(ept);
			mIdentifierDefinitionList_InfoText.Add(infotext);
			mIdentifierDefinitionList_File.GetObject(mIdentifierDefinitionList_File.AddNewObject()).CopyFrom(file);
			mIdentifierDefinitionList_StartTextIndex.Add(spos);
			mIdentifierDefinitionList_Length.Add(epos-spos);
		}
	}
	*/
	
	//現在のメニュー項目を削除
	GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_MoveToDefinition);
	//定義場所メニュー更新
	for( AIndex i = 0; i < mIdentifierDefinitionList_InfoText.GetItemCount(); i++ )
	{
		AText	menutext;
		mIdentifierDefinitionList_InfoText.Get(i,menutext);
		AText	actiontext;
		actiontext.SetFormattedCstring("%d",i);
		GetApp().NVI_GetMenuManager().AddDynamicMenuItemByMenuItemID(kMenuItemID_MoveToDefinition,menutext,actiontext,kObjectID_Invalid,NULL,0,false);
	}
}

//
void	ADocument_Text::SPI_GetCurrentIdentifierDefinitionRange( const AIndex inIndex, ATextIndex& outStart, ATextIndex& outEnd,
		ABool& outImport, AFileAcc& outFile ) const
{
	outFile.CopyFrom(mIdentifierDefinitionList_File.GetObjectConst(inIndex));
	AFileAcc	file;
	NVI_GetFile(file);
	outImport = ( file.Compare(outFile) == false );
	outStart = mIdentifierDefinitionList_StartTextIndex.Get(inIndex);
	outEnd = outStart + mIdentifierDefinitionList_Length.Get(inIndex);
}

AItemCount	ADocument_Text::SPI_GetIdentifierDefinitionListCount() const
{
	return mIdentifierDefinitionList_File.GetItemCount();
}

#if 0
//#717 削除

//inTextで始まる文字列に関して省略入力候補リストを取得する
//RC2 全体的に修正
void	ADocument_Text::SPI_GetAbbrevCandidate( const AText& inText, ATextArray& outAbbrevCandidateArray,
		ATextArray& outInfoTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
		AObjectArray<AFileAcc>& outFileArray ) const
{
	//出力配列を全消去
	outAbbrevCandidateArray.DeleteAll();
	outInfoTextArray.DeleteAll();
	outCategoryIndexArray.DeleteAll();
	outScopeArray.DeleteAll();
	outFileArray.DeleteAll();
	//
	AArray<AUniqID>	identifierIDArray;
	//ローカル
	AItemCount	itemCount = mCurrentLocalIdentifier.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		const AText&	keywordText = mCurrentLocalIdentifier.GetObjectConst(i).GetKeywordText();
		if( keywordText.GetItemCount() >= inText.GetItemCount() )
		{
			if( keywordText.CompareMin(inText) == true )
			{
				if( outAbbrevCandidateArray.Find(keywordText) == kIndex_Invalid )//重複するキーワードは表示しない
				{
					outAbbrevCandidateArray.Add(keywordText);
					AText	infotext;
					infotext.SetText(mCurrentLocalIdentifier.GetObjectConst(i).GetInfoText());
					outInfoTextArray.Add(infotext);
					outCategoryIndexArray.Add(mCurrentLocalIdentifier.GetObjectConst(i).GetCategoryIndex());
					outScopeArray.Add(kScopeType_Local);
				}
			}
		}
	}
	//
	//グローバル
	mTextInfo.GetAbbrevCandidateGlobal(inText,outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray);
	//outFileArrayを埋める
	AFileAcc	file;
	NVI_GetFile(file);
	for( AIndex i = 0; i < outAbbrevCandidateArray.GetItemCount(); i++ )
	{
		outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(file);
	}
	//インポート
	mImportIdentifierLinkList.GetAbbrevCandidate(inText,file,
			outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray,outFileArray);
	//システムヘッダファイル #253
	GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().
			GetAbbrevCandidate(inText,file,
			outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray,outFileArray);
	
	//モードのキーワード
	GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetAbbrevCandidate(inText,
			outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray);
	//outFileArrayを埋める
	for( AIndex i = outFileArray.GetItemCount(); i < outAbbrevCandidateArray.GetItemCount(); i++ )
	{
		outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(file);
	}
	//mImportIdentifierLinkList.GetAbbrevCandidate(inText,outAbbrevCandidateArray);//win 080714
	//GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetAbbrevCandidate(inText,outAbbrevCandidateArray,identifierIDArray);
	//GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_GetInfoWindow().SPI_SetAbbrevCandidate(outAbbrevCandidateArray);//ルート検討
	/*AText	text;
	if( identifierIDArray.GetItemCount() > 0 )
	{
		AUniqID	identifierID = identifierIDArray.Get(0);
		AIndex	lineIndex = mTextInfo.GetLineIndexByIdentifier(identifierID);
		ATextPoint	spt, ept;
		spt.x = 0;
		spt.y = lineIndex;
		ept.x = 0;
		ept.y = lineIndex+20;
		if( ept.y > SPI_GetLineCount() )   ept.y = SPI_GetLineCount();
		SPI_GetText(spt,ept,text);
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_GetInfoWindow().SPI_SetInfoText(text);
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_Test(lineIndex);
	}*/
}

//R0243
//ParentKeyword省略入力候補リストを取得する
void	ADocument_Text::SPI_GetAbbrevCandidateByParentKeyword( const AText& inParentKeyword, ATextArray& outAbbrevCandidateArray,
		ATextArray& outInfoTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
		AObjectArray<AFileAcc>& outFileArray ) const
{
	//出力配列を全消去
	outAbbrevCandidateArray.DeleteAll();
	outInfoTextArray.DeleteAll();
	outCategoryIndexArray.DeleteAll();
	outScopeArray.DeleteAll();
	outFileArray.DeleteAll();
	//ローカル
	//ローカルは対応しない。（ParentKeyword（クラス）定義はグローバルかインポート）
	//グローバル
	mTextInfo.GetAbbrevCandidateGlobalByParentKeyword(inParentKeyword,outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray);
	//outFileArrayを埋める
	AFileAcc	file;
	NVI_GetFile(file);
	for( AIndex i = 0; i < outAbbrevCandidateArray.GetItemCount(); i++ )
	{
		outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(file);
	}
	//インポート
	mImportIdentifierLinkList.GetAbbrevCandidateByParentKeyword(inParentKeyword,
			outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray,outFileArray);
	//システムヘッダファイル #253
	GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().
			GetAbbrevCandidateByParentKeyword(inParentKeyword,
			outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray,outFileArray);
}
#endif

//R0243
//
ABool	ADocument_Text::SPI_GetTypeOfKeyword( const AText& inText, AText& outType, const AViewID inViewID ) const
{
	outType.DeleteAll();
	//#893
	//指定view IDからview indexを取得
	AIndex	viewIndex = kIndex_Invalid;
	if( inViewID != kObjectID_Invalid )
	{
		viewIndex = mViewIDArray.Find(inViewID);
	}
	//ローカル
	if( viewIndex != kIndex_Invalid )
	{
		//指定viewのローカル範囲の識別子を検索
		AObjectID	objectID = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).FindObjectID(inText,0,inText.GetItemCount());
		if( objectID != kObjectID_Invalid )
		{
			outType.SetText(mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetObjectConstByID(objectID).GetTypeText());
			if( outType.GetItemCount() > 0 )
			{
				return true;
			}
		}
	}
	//グローバル
	if( mTextInfo.GetGlobalIdentifierTypeTextByKeywordText(inText,outType) == true )
	{
		if( outType.GetItemCount() > 0 )
		{
			return true;
		}
	}
	//インポート
	if( mImportIdentifierLinkList.FindKeywordType(inText,outType) == true )
	{
		if( outType.GetItemCount() > 0 )
		{
			return true;
		}
	}
	//システムヘッダファイルから検索 #253
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().FindKeywordType(inText,outType) == true )
	{
		if( outType.GetItemCount() > 0 )
		{
			return true;
		}
	}
	//キーワードリストから検索
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).FindKeywordTypeFromKeywordList(inText,outType) == true )
	{
		if( outType.GetItemCount() > 0 )
		{
			return true;
		}
	}
	//一致なし
	return false;
}

/*
//ダブルクリック時等にキーワードの情報を表示する機能用
void	ADocument_Text::SPI_GetInfoText( const AText& inText, AText& outInfoText ) const
{
	outInfoText.DeleteAll();
	//#147 キーワード説明を表示（一番優先）
	{
		ATextArray	explanationArray;
		AArray<AIndex>	categoryIndexArray;
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).FindKeywordExplanation(inText,explanationArray,categoryIndexArray);
		//
		if( explanationArray.GetItemCount() > 0 )
		{
			AText	categoryName;
			GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndexArray.Get(0),categoryName);
			outInfoText.AddCstring("[");
			outInfoText.AddText(categoryName);
			outInfoText.AddCstring("] ");
			outInfoText.AddText(explanationArray.GetTextConst(0));
			return;
		}
	}
	//
	AUniqID identifierID = mTextInfo.GetLocalIdentifierByKeywordText(inText);
	if( identifierID != kUniqID_Invalid )
	{
		AText	infotext;
		mTextInfo.GetLocalIdentifierInfoText(identifierID,infotext);
		if( infotext.GetItemCount() == 0 )//R0241
		{
			infotext.SetText(inText);
		}
		AIndex	categoryIndex = mTextInfo.GetLocalIdentifierCategoryIndex(identifierID);
		if( categoryIndex != kIndex_Invalid )
		{
			outInfoText.SetCstring("(Local) ");
			AText	categoryName;
			GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
			outInfoText.AddCstring("[");
			outInfoText.AddText(categoryName);
			outInfoText.AddCstring("] ");
			outInfoText.AddText(infotext);
		}
	}
	else
	{
		AUniqID identifierID = mTextInfo.GetGlobalIdentifierByKeywordText(inText);
		if( identifierID != kUniqID_Invalid )
		{
			AText	infotext;
			mTextInfo.GetGlobalIdentifierInfoText(identifierID,infotext);
			if( infotext.GetItemCount() == 0 )//R0241
			{
				infotext.SetText(inText);
			}
			AIndex	categoryIndex = mTextInfo.GetGlobalIdentifierCategoryIndex(identifierID);
			if( categoryIndex != kIndex_Invalid )
			{
				AText	categoryName;
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
				outInfoText.SetCstring("[");
				outInfoText.AddText(categoryName);
				outInfoText.AddCstring("] ");
				outInfoText.AddText(infotext);
			}
		}
		else
		{
			AFileAcc	file;
			AIndex	categoryIndex;
			AText	infotext;
			if( mImportIdentifierLinkList.FindKeyword(inText,file,categoryIndex,infotext) == true )
			{
				if( infotext.GetItemCount() == 0 )//R0241
				{
					infotext.SetText(inText);
				}
				AText	filename;
				file.GetFilename(filename);
				outInfoText.SetCstring("(");
				outInfoText.AddText(filename);
				outInfoText.AddCstring(") ");
				if( categoryIndex != kIndex_Invalid )
				{
					AText	categoryName;
					GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
					outInfoText.AddCstring("[");
					outInfoText.AddText(categoryName);
					outInfoText.AddCstring("] ");
					outInfoText.AddText(infotext);
				}
			}
			//#253 システムヘッダファイル
			else if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().
						FindKeyword(inText,file,categoryIndex,infotext) == true )
			{
				if( infotext.GetItemCount() == 0 )//R0241
				{
					infotext.SetText(inText);
				}
				AText	filename;
				file.GetFilename(filename);
				outInfoText.SetCstring("(");
				outInfoText.AddText(filename);
				outInfoText.AddCstring(") ");
				if( categoryIndex != kIndex_Invalid )
				{
					AText	categoryName;
					GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
					outInfoText.AddCstring("[");
					outInfoText.AddText(categoryName);
					outInfoText.AddCstring("] ");
					outInfoText.AddText(infotext);
				}
			}
		}
	}
}
*/

//RC2
//識別子情報取得
void	ADocument_Text::SPI_GetIdInfoArray( const AText& inText, ATextArray& outInfoTextArray, AObjectArray<AFileAcc>& outFileArray,
		ATextArray& outCommentTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
		AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
		ATextArray& outParentKeywordArray ) const
{
	AFileAcc	docfile;
	NVI_GetFile(docfile);
	//出力配列全削除
	outInfoTextArray.DeleteAll();
	outFileArray.DeleteAll();
	outCommentTextArray.DeleteAll();
	outCategoryIndexArray.DeleteAll();
	outScopeArray.DeleteAll();
	outStartIndexArray.DeleteAll();
	outEndIndexArray.DeleteAll();
	outParentKeywordArray.DeleteAll();
	//ローカル
	/*ローカル識別子は表示しないことにする
	mTextInfoに格納されているローカル識別子は、ファイル全体のローカル識別子である。
	よって、下記の方法で識別子を取得すると、現在のローカル範囲を無視して、全ての識別子の情報が取得されてしまう
	一方、mCurrentLocalIdentifierには、キーワードとカテゴリーの情報しか含まれていない。
	そもそも、ローカル識別子の識別子情報を取得する必要性は、現在の仕様では少ないと思われる。
	{
		AArray<AUniqID>	UniqIDArray;
		mTextInfo.GetLocalIdentifierListByKeywordText(inText,UniqIDArray);
		for( AIndex i = 0; i < UniqIDArray.GetItemCount(); i++ )
		{
			AUniqID	identifierID = UniqIDArray.Get(i);
			//
			AText	infotext;
			mTextInfo.GetLocalIdentifierInfoText(identifierID,infotext);
			AIndex	categoryIndex = mTextInfo.GetLocalIdentifierCategoryIndex(identifierID);
			if( categoryIndex != kIndex_Invalid )
			{
				AText	categoryName;
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
				//
				outInfoTextArray.Add(infotext);
				outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(docfile);
				outCommentTextArray.Add(GetEmptyText());
				outCategoryIndexArray.Add(categoryIndex);
				outScopeArray.Add(kScopeType_Local);
				ATextPoint	spt, ept;
				mTextInfo.GetLocalIdentifierRange(identifierID,spt,ept);
				ATextIndex	spos = SPI_GetTextIndexFromTextPoint(spt);
				ATextIndex	epos = SPI_GetTextIndexFromTextPoint(ept);
				outStartIndexArray.Add(spos);
				outEndIndexArray.Add(epos);
				outParentKeywordArray.Add(GetEmptyText());
			}
		}
	}*/
	//#147 キーワード説明
	{
		ATextArray	explanationArray;
		AArray<AIndex>	categoryIndexArray;
		ATextArray	parentKeywordArray;
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).FindKeywordIdInfo(inText,explanationArray,categoryIndexArray,parentKeywordArray);
		//
		for( AIndex i = 0; i < explanationArray.GetItemCount(); i++ )
		{
			//各情報設定
			outInfoTextArray.Add(explanationArray.GetTextConst(i));
			outFileArray.AddNewObject();
			outCommentTextArray.Add(GetEmptyText());
			outCategoryIndexArray.Add(categoryIndexArray.Get(i));
			outScopeArray.Add(kScopeType_ModeKeyword);
			//識別子キーワードの範囲を取得して設定
			outStartIndexArray.Add(0);
			outEndIndexArray.Add(0);
			outParentKeywordArray.Add(parentKeywordArray.GetTextConst(i));
		}
	}
	//グローバル
	{
		//キーワードに対応するグローバル識別子を全て取得
		AArray<AUniqID>	UniqIDArray;
		mTextInfo.GetGlobalIdentifierListByKeywordText(inText,UniqIDArray);
		//各識別子毎に処理
		for( AIndex i = 0; i < UniqIDArray.GetItemCount(); i++ )
		{
			//識別子UniqID取得
			AUniqID	identifierID = UniqIDArray.Get(i);
			//各情報取得
			AText	infotext;
			mTextInfo.GetGlobalIdentifierInfoText(identifierID,infotext);
			AText	comment;
			mTextInfo.GetGlobalIdentifierCommentText(identifierID,comment);
			AText	parent;
			mTextInfo.GetGlobalIdentifierParentKeywordText(identifierID,parent);
			AIndex	categoryIndex = mTextInfo.GetGlobalIdentifierCategoryIndex(identifierID);
			if( categoryIndex != kIndex_Invalid )
			{
				AText	categoryName;
				GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetCategoryName(categoryIndex,categoryName);
				//各情報設定
				outInfoTextArray.Add(infotext);
				outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(docfile);
				outCommentTextArray.Add(comment);
				outCategoryIndexArray.Add(categoryIndex);
				outScopeArray.Add(kScopeType_Global);
				//識別子キーワードの範囲を取得して設定
				ATextPoint	spt, ept;
				mTextInfo.GetGlobalIdentifierRange(identifierID,spt,ept);
				ATextIndex	spos = SPI_GetTextIndexFromTextPoint(spt);
				ATextIndex	epos = SPI_GetTextIndexFromTextPoint(ept);
				outStartIndexArray.Add(spos);
				outEndIndexArray.Add(epos);
				outParentKeywordArray.Add(parent);
			}
		}
	}
	//インポート
	{
		//キーワードに対応するインポート識別子を全て取得
		ATextArray	infoTextArray, commentTextArray, parentTextArray;
		AArray<AIndex>	categoryIndexArray;
		AObjectArray<AFileAcc>	fileArray;
		AArray<ATextIndex>	startArray;
		AArray<ATextIndex>	endArray;
		mImportIdentifierLinkList.GetIdentifierListByKeyword(inText,categoryIndexArray,infoTextArray,fileArray,startArray,endArray,
					commentTextArray,parentTextArray);
		//システムヘッダファイル #253
		GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSystemHeaderIdentifierLinkList().
				GetIdentifierListByKeyword(inText,categoryIndexArray,infoTextArray,fileArray,startArray,endArray,
				commentTextArray,parentTextArray);
		//各識別子ごとに処理
		for( AIndex i = 0; i < infoTextArray.GetItemCount(); i++ )
		{
			//各情報設定
			outInfoTextArray.Add(infoTextArray.GetTextConst(i));
			outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(fileArray.GetObjectConst(i));
			outCommentTextArray.Add(commentTextArray.GetTextConst(i));
			outCategoryIndexArray.Add(categoryIndexArray.Get(i));
			outScopeArray.Add(kScopeType_Import);
			outStartIndexArray.Add(startArray.Get(i));
			outEndIndexArray.Add(endArray.Get(i));
			outParentKeywordArray.Add(parentTextArray.GetTextConst(i));
		}
	}
}

//
ABool	ADocument_Text::SPI_IsCodeChar( const ATextPoint& inPoint ) const
{
	AArray<ABool>	isCode;
	mTextInfo.GetIsCodeArray(mText,inPoint.y,isCode);
	return isCode.Get(inPoint.x);
}

/**
括弧内範囲取得
@note inLimitSearchRangeにtrueを指定するとサーチ範囲は限定される。
*/
ABool	ADocument_Text::SPI_GetBraceSelection( ATextPoint& ioStart, ATextPoint& ioEnd, const ABool inLimitSearchRange ) const
{
	ABool	result = true;
	ANumber	braceLev = 0;
	ANumber	bracketLev = 0;
	ANumber	kakkoLev = 0;
	
	//#853
	//処理開始前の位置を記憶
	ATextPoint	originalStart = ioStart;
	ATextPoint	originalEnd = ioEnd;
	
	//括弧開始位置計算
	ABool	loopend = false;
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(ioStart);
	ATextPoint	spt;
	SPI_GetTextPointFromTextIndex(spos,spt);
	spt.x--;
	spos--;
	AArray<ABool>	isCode;
	while(true)
	{
		//inLimitSearchRangeがtrueの場合、サーチ範囲限定する #853
		if( inLimitSearchRange == true )
		{
			if( originalStart.y - spt.y > kLimit_BraseSelectionSearchRange )
			{
				//戻りすぎたら、ioStart, ioEndを元に戻して、リターン
				ioStart = originalStart;
				ioEnd = originalEnd;
				return false;
			}
		}
		//
		mTextInfo.GetIsCodeArray(mText,spt.y,isCode);
		for( ; spt.x >= 0; spt.x--, spos-- )
		{
			if( isCode.Get(spt.x) == false )
			{
				continue;
			}
			AUChar	ch = mText.Get(spos);
			switch(ch)
			{
			  case '{':
				{
					braceLev--;
					break;
				}
			  case '}':
				{
					braceLev++;
					break;
				}
			  case '[':
				{
					bracketLev--;
					break;
				}
			  case ']':
				{
					bracketLev++;
					break;
				}
			  case '(':
				{
					kakkoLev--;
					break;
				}
			  case ')':
				{
					kakkoLev++;
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			if( braceLev < 0 || bracketLev < 0 || kakkoLev < 0 )
			{
				spos++;
				loopend = true;
				break;
			}
		}
		if( loopend == true )   break;
		spt.y--;
		if( spt.y < 0 )
		{
			result = false;
			spos = 0;
			break;
		}
		spt.x = SPI_GetLineLengthWithoutLineEnd(spt.y)-1;
		spos = SPI_GetTextIndexFromTextPoint(spt);
	}
	
	//括弧終了位置計算
	loopend = false;
	braceLev = 0;
	bracketLev = 0;
	kakkoLev = 0;
	ATextPoint	ept = ioEnd;
	ATextIndex	epos = SPI_GetTextIndexFromTextPoint(ept);
	while(true)
	{
		//inLimitSearchRangeがtrueの場合、サーチ範囲限定する
		if( inLimitSearchRange == true )
		{
			if( ept.y - originalEnd.y > kLimit_BraseSelectionSearchRange )
			{
				//進みすぎたら、ioStart, ioEndを元に戻して、リターン
				ioStart = originalStart;
				ioEnd = originalEnd;
				return false;
			}
		}
		//
		mTextInfo.GetIsCodeArray(mText,ept.y,isCode);
		AItemCount	lineLength = SPI_GetLineLengthWithoutLineEnd(ept.y);
		for( ; ept.x < lineLength; ept.x++, epos++ )
		{
			if( isCode.Get(ept.x) == false )
			{
				continue;
			}
			AUChar	ch = mText.Get(epos);
			switch(ch)
			{
			  case '{':
					{
						braceLev--;
						break;
					}
			  case '}':
				{
					braceLev++;
					break;
				}
			  case '[':
				{
					bracketLev--;
					break;
				}
			  case ']':
				{
					bracketLev++;
					break;
				}
			  case '(':
				{
					kakkoLev--;
					break;
				}
			  case ')':
				{
					kakkoLev++;
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			if( braceLev > 0 || bracketLev > 0 || kakkoLev > 0 )
			{
				loopend = true;
				break;
			}
		}
		if( loopend == true )   break;
		ept.y++;
		if( ept.y >= SPI_GetLineCount() )
		{
			result = false;
			epos = SPI_GetTextLength();
			break;
		}
		ept.x = 0;
		epos = SPI_GetTextIndexFromTextPoint(ept);
	}
	//
	SPI_GetTextPointFromTextIndex(spos,ioStart);
	SPI_GetTextPointFromTextIndex(epos,ioEnd);
	return result;
}

//
ABool	ADocument_Text::SPI_CheckBrace( const ATextPoint& inCaretTextPoint, ATextPoint& outBraceTextPoint, ABool& outCheckOK ) const
{
	//
	ABoolArray	isCode;
	mTextInfo.GetIsCodeArray(mText,inCaretTextPoint.y,isCode);
	if( inCaretTextPoint.x-1 >= 0 )
	{
		if( isCode.Get(inCaretTextPoint.x-1) == false )   return false;
	}
	
	//
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(inCaretTextPoint);
	spos--;
	if( spos-1 < 0 )   return false;
	ATextPoint	pt;
	SPI_GetTextPointFromTextIndex(spos-1,pt);
	ABool	found = false;
	ABool	end = false;
	AUChar	out = mText.Get(spos);
	AUChar	in = 0;
	ANumber	braceLev = 0;
	ANumber	bracketLev = 0;
	ANumber	kakkoLev = 0;
	switch(out)
	{
	  case '}':
		{
			in = '{';
			braceLev = 1;
			break;
		}
	  case ')':
		{
			in = '(';
			kakkoLev = 1;
			break;
		}
	  case ']':
		{
			in = '[';
			bracketLev = 1;
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	if( in == 0 )   return false;
	for( ; pt.y >= 0; pt.y-- )
	{
		isCode.DeleteAll();
		mTextInfo.GetIsCodeArray(mText,pt.y,isCode);
		AIndex	lineStart = SPI_GetLineStart(pt.y);
		for( ; pt.x >= 0; pt.x-- )
		{
			if( pt.x >= isCode.GetItemCount() )   continue;
			if( isCode.Get(pt.x) == false )   continue;
			AUChar	ch = mText.Get(lineStart+pt.x);
			switch(ch)
			{
			  case '{':
				{
					braceLev--;
					break;
				}
			  case '}':
				{
					braceLev++;
					break;
				}
			  case '[':
				{
					bracketLev--;
					break;
				}
			  case ']':
				{
					bracketLev++;
					break;
				}
			  case '(':
				{
					kakkoLev--;
					break;
				}
			  case ')':
				{
					kakkoLev++;
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			switch(in)
			{
			  case '{':
				{
					if( braceLev == 0 )
					{
						if( bracketLev == 0 && kakkoLev == 0 )    found = true;
						end = true;
					}
					break;
				}
			  case '(':
				{
					if( kakkoLev == 0 )
					{
						if( braceLev == 0 && bracketLev == 0 )   found = true;
						end = true;
					}
					break;
				}
			  case '[':
				{
					if( bracketLev == 0 )
					{
						if( braceLev == 0 && kakkoLev == 0 )   found = true;
						end = true;
					}
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			if( end == true )   break;
		}
		if( end == true )   break;
		if( pt.y == 0 )   break;
		pt.x = SPI_GetLineLengthWithoutLineEnd(pt.y-1);
	}
	outBraceTextPoint = pt;
	outCheckOK = found;
	return true;
}

//対応文字列入力
ABool	ADocument_Text::SPI_GetCorrespondText( const ATextPoint& inCaretTextPoint, AText& outText, ATextPoint& outStart, ATextPoint& outEnd ) const//R0000
{
	return CalcCorrespond(inCaretTextPoint,inCaretTextPoint,true,outText,outStart,outEnd);
#if 0
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition();
	AIndex	stateIndex = SPI_GetStateIndexByLineIndex(inCaretTextPoint.y);
	AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
	
	//#12
	//終了文字列の文字列長が長い方を優先して一致させる
	//startTextArray, endTextArrayに終了文字列長い方から格納する
	//abc
	//def
	//abcd
	//なら
	//abcd
	//abc
	//def
	//の順にする。
	ATextArray	startTextArray, endTextArray;
	//startTextArray, endTextArrayに追加していく
	AItemCount	itemCountInModePref = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetItemCount_TextArray(AModePrefDB::kCorrespond_StartText);
	for( AIndex i = 0; i < itemCountInModePref; i++ )
	{
		//追加済みの配列を後ろから検索して、追加しようとしている文字列以上の長さの文字列がはじめて見つかった位置の、次の位置に追加する
		//これにより文字列長が長い順になる。かつ、同じ長さなら元の順番が保たれる。
		AItemCount	len = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_StartText).GetTextConst(i).GetItemCount();
		AIndex j = startTextArray.GetItemCount()-1;
		for( ; j >= 0 ; j-- )
		{
			if( startTextArray.GetTextConst(j).GetItemCount() >= len )
			{
				startTextArray.Insert1(j+1, GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_StartText).GetTextConst(i));
				endTextArray.Insert1(j+1, GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_EndText).GetTextConst(i));
				break;
			}
		}
		//追加しようとしている文字列以上の長さの文字列が見つからない場合は最初に追加する。
		if( j < 0 )
		{
			startTextArray.Insert1(0, GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_StartText).GetTextConst(i));
			endTextArray.Insert1(0, GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_EndText).GetTextConst(i));
		}
	}
	//
	if( syntaxPartIndex != kIndex_Invalid )
	{
		const ATextArray&	sdfStartTextArray = syntaxDefinition.GetCorrespondStartArray(syntaxPartIndex);
		const ATextArray&	sdfEndTextArray = syntaxDefinition.GetCorrespondEndArray(syntaxPartIndex);
		AItemCount	itemCountInSDF = sdfStartTextArray.GetItemCount();
		for( AIndex i = 0; i < itemCountInSDF; i++ )
		{
			//追加済みの配列を後ろから検索して、追加しようとしている文字列以上の長さの文字列がはじめて見つかった位置の、次の位置に追加する
			//これにより文字列長が長い順になる。かつ、同じ長さなら元の順番が保たれる。
			AItemCount	len = sdfStartTextArray.GetTextConst(i).GetItemCount();
			AIndex j = startTextArray.GetItemCount()-1;
			for( ; j >= 0 ; j-- )
			{
				if( startTextArray.GetTextConst(j).GetItemCount() >= len )
				{
					startTextArray.Insert1(j+1, sdfStartTextArray.GetTextConst(i));
					endTextArray.Insert1(j+1, sdfEndTextArray.GetTextConst(i));
					break;
				}
			}
			//追加しようとしている文字列以上の長さの文字列が見つからない場合は最初に追加する。
			if( j < 0 )
			{
				startTextArray.Insert1(0, sdfStartTextArray.GetTextConst(i));
				endTextArray.Insert1(0, sdfEndTextArray.GetTextConst(i));
			}
		}
	}
	
	//
	AItemCount	startTextArrayCount = startTextArray.GetItemCount();
	if( startTextArrayCount == 0 )   return false;
	//
	ANumberArray	levelArray;
	for( AIndex i = 0; i < startTextArrayCount; i++ )
	{
		levelArray.Add(0);
	}
	
	//B0443 全体的に書き換え
	//【アルゴリズム】
	//キャレット位置から戻っていく posに位置が入っている
	//levelArrayに、対応文字列設定の各項目ごとに、レベル情報が入っている。最初は全部0]]
	
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(inCaretTextPoint);
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_Correspond,true);
	try
	{
		AStTextPtr	textptr(mText,0,mText.GetItemCount());
		AUChar*	p = textptr.GetPtr();
		AItemCount	textlen = mText.GetItemCount();
		
		for( ATextIndex pos = spos; pos > 0; )
		{
			//モーダルセッション継続判定
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Correspond,0,true,
																	  spos-pos,
																	  spos) == false )
			{
				break;
			}
			//
			AText	st, et;
			//対応文字列設定の各項目毎に確認
			ABool	found = false;//B0000 最初に見つかったものだけlevel変更する
			for( AIndex i = 0; i < startTextArrayCount; i++ )
			{
				const AText&	startText = startTextArray.GetTextConst(i);
				const AText&	endText = endTextArray.GetTextConst(i);
				//pos以前に終了文字があればlevel変更
				if( pos - endText.GetItemCount() >= 0 && endText.GetItemCount() > 0/*#11*/ )
				{
					if( endText.Compare(mText,pos - endText.GetItemCount(),endText.GetItemCount()) == true )
					{
						levelArray.Set(i,levelArray.Get(i)+1);
						found = true;//B0000 最初に見つかったものだけlevel変更する
						pos -= endText.GetItemCount();
					}
				}
				//終了文字列優先
				if( found == false )
				//pos以前に開始文字があればlevel変更
				if( pos - startText.GetItemCount() >= 0 && startText.GetItemCount() > 0/*#11*/ )
				{
					if( startText.Compare(mText,pos - startText.GetItemCount(),startText.GetItemCount()) == true )
					{
						levelArray.Set(i,levelArray.Get(i)-1);
						found = true;//B0000 最初に見つかったものだけlevel変更する
						pos -= startText.GetItemCount();
					}
				}
				//levelがマイナスになったら終了
				if( levelArray.Get(i) < 0 && endText.GetItemCount() > 0 )
				{
					outText.SetText(endText);
					SPI_GetTextPointFromTextIndex(pos,outStart);
					SPI_GetTextPointFromTextIndex(pos+startText.GetItemCount(),outEnd);
					return true;
				}
				if( found == true )   break;//B0000 最初に見つかったものだけlevel変更する
			}
			if( found == false )
			{
				pos = SPI_GetPrevCharTextIndex(pos);
			}
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	
#if 0
	//【アルゴリズム】
	//キャレット位置から戻っていく ptに位置が入っている
	//levelArrayに、対応文字列設定の各項目ごとに、レベル情報が入っている。最初は全部0
	
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(inCaretTextPoint);
	//B0443 spos--;
	if( spos < 0 )   return false;
	//B0443 ATextPoint	pt;
	//B0443 SPI_GetTextPointFromTextIndex(spos,pt);
	//ABoolArray	isCode;
	//B0000高速化AText	startText, endText;
	//B0443 for( ; pt.y >= 0; pt.y-- )
	for( ATextIndex pos = spos; pos > 0; )
	{
		//isCode.DeleteAll();
		//mTextInfo.GetIsCodeArray(mText,SPI_GetModeIndex(),pt.y,isCode);
		//B0443 AIndex	lineStart = SPI_GetLineStart(pt.y);
		//B0443 for( ; pt.x >= 0; pt.x-- )
		//{
			//if( isCode.Get(pt.x) == false )   continue;
			/*B0443 AUChar	ch = mText.Get(lineStart+pt.x);
			if( mText.IsUTF8MultiByte(ch) == true )
			{
				if( mText.IsUTF8FirstByteInMultiByte(ch) == false )   continue;
			}*/
			//対応文字列設定の各項目毎に確認
			ABool	found = false;//B0000 最初に見つかったものだけlevel変更する
			for( AIndex i = 0; i < taiocount; i++ )
			{
				//B0000高速化GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArray(AModePrefDB::kCorrespond_StartText,i,startText);
				//B0000高速化GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArray(AModePrefDB::kCorrespond_EndText,i,endText);
				const AText&	startText = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_StartText).GetTextConst(i);
				const AText&	endText   = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_TextArrayRef(AModePrefDB::kCorrespond_EndText).GetTextConst(i);
				//pt以前に終了文字があればlevel変更
				//B0443 if( lineStart+pt.x+endText.GetItemCount() <= mText.GetItemCount() )
				if( pos - endText.GetItemCount() >= 0 )
				{
					//B0443 if( endText.Compare(mText,lineStart+pt.x,endText.GetItemCount()) == true )
					if( endText.Compare(mText,pos - endText.GetItemCount(),endText.GetItemCount()) == true )
					{
						levelArray.Set(i,levelArray.Get(i)+1);
						found = true;//B0000 最初に見つかったものだけlevel変更する
						pos -= endText.GetItemCount();//B0443
					}
				}
				//終了文字列優先
				if( found == false )
				//pt以前に開始文字があればlevel変更
				//B0443 if( lineStart+pt.x+startText.GetItemCount() <= mText.GetItemCount() )
				if( pos - startText.GetItemCount() >= 0 )
				{
					//B0443 if( startText.Compare(mText,lineStart+pt.x,startText.GetItemCount()) == true )
					if( startText.Compare(mText,pos - startText.GetItemCount(),startText.GetItemCount()) == true )
					{
						levelArray.Set(i,levelArray.Get(i)-1);
						found = true;//B0000 最初に見つかったものだけlevel変更する
						pos -= startText.GetItemCount();//B0443
					}
				}
				/*B0443
				if( lineStart+pt.x+endText.GetItemCount() <= mText.GetItemCount() )
				{
					if( endText.Compare(mText,lineStart+pt.x,endText.GetItemCount()) == true )
					{
						levelArray.Set(i,levelArray.Get(i)+1);
						found = true;//B0000 最初に見つかったものだけlevel変更する
					}
				}
				*/
				//levelがマイナスになったら終了
				if( levelArray.Get(i) < 0 )
				{
					outText.SetText(endText);
					//R0000
					/*B0443
					outStart = pt;
					outEnd = pt;
					outEnd.x += startText.GetItemCount();
					*/
					SPI_GetTextPointFromTextIndex(pos,outStart);
					SPI_GetTextPointFromTextIndex(pos+startText.GetItemCount(),outEnd);
					return true;
				}
				if( found == true )   break;//B0000 最初に見つかったものだけlevel変更する
			}
		//}
		//B0443 if( pt.y == 0 )   break;
		//B0443 pt.x = SPI_GetLineLengthWithoutLineEnd(pt.y-1);
		//B0443
		if( found == false )
		{
			pos = SPI_GetPrevCharTextIndex(pos);
		}
	}
#endif
	return false;
#endif
}

//#359 #419(処理を全体的に見直し)
/**
*/
ABool	ADocument_Text::SPI_GetCorrespondenceSelection( const ATextPoint& inStart, const ATextPoint& inEnd,
		ATextPoint& outStart, ATextPoint& outEnd ) const
{
	AText	correspondText;
	return CalcCorrespond(inStart,inEnd,false,correspondText,outStart,outEnd);
}
ABool	ADocument_Text::CalcCorrespond( const ATextPoint& inStart, const ATextPoint& inEnd, 
									   const ABool inFindForCorrespondInput, AText& outCorrespondText,
									   ATextPoint& outStart, ATextPoint& outEnd ) const
{
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition();
	AIndex	stateIndex = SPI_GetCurrentStateIndex(inStart);//SPI_GetStateIndexByLineIndex(inStart.y);
	AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
	
	ABool	result = true;
	outStart = inStart;
	outEnd = inEnd;
	
	AObjectArray<AText>	startTextArray, endTextArray;
	
	//==================モード設定から追加==================
	{
		AItemCount	itemCountInModePref = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetItemCount_TextArray(AModePrefDB::kCorrespond_StartText);
		//モード設定のデータをAObjectArray<AText>へコピー
		AObjectArray<AText>	prefStartTextArray, prefEndTextArray;
		for( AIndex i = 0; i < itemCountInModePref; i++ )
		{
			prefStartTextArray.AddNewObject();
			prefEndTextArray.AddNewObject();
			prefStartTextArray.GetObject(i).SetText(GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).
													GetData_TextArrayRef(AModePrefDB::kCorrespond_StartText).GetTextConst(i));
			prefEndTextArray.GetObject(i).SetText(GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).
												  GetData_TextArrayRef(AModePrefDB::kCorrespond_EndText).GetTextConst(i));
		}
		//startTextArray, endTextArrayに追加していく
		for( AIndex i = 0; i < itemCountInModePref; i++ )
		{
			//追加済みの配列を後ろから検索して、追加しようとしている文字列以上の長さの文字列がはじめて見つかった位置の、次の位置に追加する
			//これにより文字列長が長い順になる。かつ、同じ長さなら元の順番が保たれる。
			AItemCount	len = prefStartTextArray.GetObjectConst(i).GetItemCount();
			AIndex j = startTextArray.GetItemCount()-1;
			for( ; j >= 0 ; j-- )
			{
				if( startTextArray.GetObjectConst(j).GetItemCount() >= len )
				{
					startTextArray.InsertNew1Object(j+1);
					endTextArray.InsertNew1Object(j+1);
					startTextArray.GetObject(j+1).SetText(prefStartTextArray.GetObjectConst(i));
					endTextArray.GetObject(j+1).SetText(prefEndTextArray.GetObjectConst(i));
					break;
				}
			}
			//追加しようとしている文字列以上の長さの文字列が見つからない場合は最初に追加する。
			if( j < 0 )
			{
				startTextArray.InsertNew1Object(0);
				endTextArray.InsertNew1Object(0);
				startTextArray.GetObject(0).SetText(prefStartTextArray.GetObjectConst(i));
				endTextArray.GetObject(0).SetText(prefEndTextArray.GetObjectConst(i));
			}
		}
	}
	//==================SDFから取得==================
	//
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kEnableSDFCorrespond) == true )
	{
		if( syntaxPartIndex != kIndex_Invalid )
		{
			AItemCount	itemCountInSDF = syntaxDefinition.GetCorrespondStartArray(syntaxPartIndex).GetItemCount();
			//SDFのデータをAObjectArray<AText>へコピー
			AObjectArray<AText>	sdfStartTextArray, sdfEndTextArray;
			for( AIndex i = 0; i < itemCountInSDF; i++ )
			{
				sdfStartTextArray.AddNewObject();
				sdfEndTextArray.AddNewObject();
				sdfStartTextArray.GetObject(i).SetText(syntaxDefinition.GetCorrespondStartArray(syntaxPartIndex).GetTextConst(i));
				sdfEndTextArray.GetObject(i).SetText(syntaxDefinition.GetCorrespondEndArray(syntaxPartIndex).GetTextConst(i));
			}
			//
			for( AIndex i = 0; i < itemCountInSDF; i++ )
			{
				//追加済みの配列を後ろから検索して、追加しようとしている文字列以上の長さの文字列がはじめて見つかった位置の、次の位置に追加する
				//これにより文字列長が長い順になる。かつ、同じ長さなら元の順番が保たれる。
				AItemCount	len = sdfStartTextArray.GetObjectConst(i).GetItemCount();
				AIndex j = startTextArray.GetItemCount()-1;
				for( ; j >= 0 ; j-- )
				{
					if( startTextArray.GetObjectConst(j).GetItemCount() >= len )
					{
						startTextArray.InsertNew1Object(j+1);
						endTextArray.InsertNew1Object(j+1);
						startTextArray.GetObject(j+1).SetText(sdfStartTextArray.GetObjectConst(i));
						endTextArray.GetObject(j+1).SetText(sdfEndTextArray.GetObjectConst(i));
						break;
					}
				}
				//追加しようとしている文字列以上の長さの文字列が見つからない場合は最初に追加する。
				if( j < 0 )
				{
					startTextArray.InsertNew1Object(0);
					endTextArray.InsertNew1Object(0);
					startTextArray.GetObject(0).SetText(sdfStartTextArray.GetObjectConst(i));
					endTextArray.GetObject(0).SetText(sdfEndTextArray.GetObjectConst(i));
				}
			}
		}
	}
	
	//開始文字列と終了文字列が同じ場合や、どちらかが空の場合は対象外にする
	for( AIndex i = 0; i < startTextArray.GetItemCount(); )
	{
		if( startTextArray.GetObjectConst(i).GetItemCount() == 0 ||
		   //endTextArray.GetObjectConst(i).GetItemCount() == 0 ||
		   startTextArray.GetObjectConst(i).Compare(endTextArray.GetObjectConst(i)) == true )
		{
			startTextArray.Delete1Object(i);
			endTextArray.Delete1Object(i);
		}
		else
		{
			i++;
		}
	}
	
	//
	AItemCount	startTextArrayCount = startTextArray.GetItemCount();
	if( startTextArrayCount == 0 )   return false;
	
	AStTextPtr	textptr(mText,0,mText.GetItemCount());
	AUChar*	p = textptr.GetPtr();
	AItemCount	textlen = mText.GetItemCount();
	
	//開始位置、対応文字列インデックス検索
	AArray<ABool>	isCodeArray;
	AIndex	isCodeArray_LineIndex = inStart.y;
	AIndex	isCodeArray_LineStart = SPI_GetLineStart(isCodeArray_LineIndex);
	mTextInfo.GetIsCodeArray(mText,isCodeArray_LineIndex,isCodeArray);
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(inStart);
	spos = SPI_GetPrevCharTextIndex(spos);
	//
	AIndex	resultspos = spos;
	AIndex	resultepos = spos;
	//==================開始位置検索==================
	AIndex	taioStartIndex = kIndex_Invalid;
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_Correspond,true,false,true);
	try
	{
		AArray<AIndex>	taioEndIndexStack;
		for( ; spos >= 0; spos = SPI_GetPrevCharTextIndex(spos,true,true) )
		{
			//モーダルセッション継続判定
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Correspond,0,true,
																	  (resultspos-spos)/2,resultspos) == false )
			{
				taioStartIndex = kIndex_Invalid;
				result = false;
				break;
			}
			//Code部分かどうかの判定
			if( spos < isCodeArray_LineStart )
			{
				ATextPoint	pt;
				SPI_GetTextPointFromTextIndex(spos,pt,false);
				isCodeArray_LineIndex = pt.y;
				isCodeArray_LineStart = SPI_GetLineStart(isCodeArray_LineIndex);
				mTextInfo.GetIsCodeArray(mText,isCodeArray_LineIndex,isCodeArray);
			}
			if( isCodeArray.Get(spos-isCodeArray_LineStart) == false )   continue;
			//
			AUChar	ch = p[spos];
			if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
			{
				continue;
			}
			//
			AText	st, et;
			//各対応文字列ループ
			for( AIndex i = 0; i < startTextArrayCount; i++ )
			{
				const AText&	startText = startTextArray.GetObjectConst(i);
				const AText&	endText = endTextArray.GetObjectConst(i);
				//------------------開始文字列検索------------------
				//
				if( st.GetItemCount() != startText.GetItemCount() )
				{
					AText::GetCharactersOtherThanSpaces(p,textlen,spos,startText.GetItemCount(),st);
				}
				//
				if( startText.Compare(st) == true )
				{
					//
					if( endText.GetItemCount() == 0 )
					{
						break;
					}
					//
					if( taioEndIndexStack.GetItemCount() == 0 )
					{
						//終了文字列スタックがない場合は、この開始文字以降を選択する
						taioStartIndex = i;
						resultspos = spos;
						spos += startText.GetItemCount();
					}
					else
					{
						if( taioEndIndexStack.Get(taioEndIndexStack.GetItemCount()-1) == i )
						{
							//開始と終了のindexが一致していれば、終了文字列スタックから取り除く
							taioEndIndexStack.Delete1(taioEndIndexStack.GetItemCount()-1);
						}
						else
						{
							//一致していない場合は、この開始文字列以降を選択する
							result = false;
							taioStartIndex = i;
							resultspos = spos;
							spos += startText.GetItemCount();
						}
					}
					break;
				}
				//終了文字列検索
				if( endText.GetItemCount() > 0 )
				{
					//
					if( et.GetItemCount() != endText.GetItemCount() )
					{
						AText::GetCharactersOtherThanSpaces(p,textlen,spos,endText.GetItemCount(),et);
					}
					//
					if( endText.Compare(et) == true )
					{
						//終了文字列スタックへ追加
						taioEndIndexStack.Add(i);
						break;
					}
				}
			}
			if( taioStartIndex != kIndex_Invalid )   break;
		}
		//
		if( spos < 0 )   spos = 0;
		//
		if( taioStartIndex == kIndex_Invalid )
		{
			return false;
		}
		
		if( inFindForCorrespondInput == true )
		{
			outCorrespondText.SetText(endTextArray.GetObjectConst(taioStartIndex));
			SPI_GetTextPointFromTextIndex(resultspos,outStart);
			outEnd = inEnd;
			return true;
		}
		
		//==================終了位置検索==================
		ATextIndex	epos = spos;
		//
		AIndex	taioEndIndex = kIndex_Invalid;
		AArray<AIndex>	taioIndexStack;
		taioIndexStack.Add(taioStartIndex);
		for( ; epos < textlen; epos = SPI_GetNextCharTextIndex(epos) )
		{
			//モーダルセッション継続判定
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Correspond,0,true,
																	  textlen/2+epos,textlen) == false )
			{
				taioEndIndex = kIndex_Invalid;
				break;
			}
			//Code部分かどうかの判定
			if( epos >= isCodeArray_LineStart+isCodeArray.GetItemCount() )
			{
				ATextPoint	pt;
				SPI_GetTextPointFromTextIndex(epos,pt,true);
				isCodeArray_LineIndex = pt.y;
				isCodeArray_LineStart = SPI_GetLineStart(isCodeArray_LineIndex);
				mTextInfo.GetIsCodeArray(mText,isCodeArray_LineIndex,isCodeArray);
			}
			if( isCodeArray.Get(epos-isCodeArray_LineStart) == false )   continue;
			//
			AUChar	ch = p[epos];
			if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
			{
				continue;
			}
			//
			AText	st, et;
			AIndex	et_pos = kIndex_Invalid;
			//各対応文字列ループ
			for( AIndex i = 0; i < startTextArrayCount; i++ )
			{
				const AText&	startText = startTextArray.GetObjectConst(i);
				const AText&	endText = endTextArray.GetObjectConst(i);
				//------------------開始文字列検索------------------
				//
				if( st.GetItemCount() != startText.GetItemCount() )
				{
					AText::GetCharactersOtherThanSpaces(p,textlen,epos,startText.GetItemCount(),st);
				}
				//
				if( startText.Compare(st) == true )
				{
					//
					if( endText.GetItemCount() > 0 )
					{
						//スタックへ追加
						taioIndexStack.Add(i);
					}
					break;
				}
				//------------------終了文字列検索------------------
				if( endText.GetItemCount() > 0 )
				{
					//
					if( et.GetItemCount() != endText.GetItemCount() )
					{
						et_pos = AText::GetCharactersOtherThanSpaces(p,textlen,epos,endText.GetItemCount(),et);
					}
					//
					if( endText.Compare(et) == true )
					{
						if( taioIndexStack.Get(taioIndexStack.GetItemCount()-1) == i )
						{
							//スタックの最初と一致した場合はスタックから取り除く
							taioIndexStack.Delete1(taioIndexStack.GetItemCount()-1);
							//スタックがからになったら終了
							if( taioIndexStack.GetItemCount() == 0 )
							{
								taioEndIndex = i;
							}
						}
						else
						{
							//一致していない場合は、この終了文字列以前を選択する
							result = false;
							taioEndIndex = i;
						}
						//
						resultepos = et_pos;
						break;
					}
				}
				
			}
			if( taioEndIndex != kIndex_Invalid )   break;
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	//
	SPI_GetTextPointFromTextIndex(resultspos,outStart);
	SPI_GetTextPointFromTextIndex(resultepos,outEnd);
	return result;
}

const AItemCount	kPreHitTextLength = 100;
const AItemCount	kPostHitTextLength = 100;

//
//
void	ADocument_Text::SPI_ReportTextEncodingErrors()
{
	AText	returntext;
	returntext.SetLocalizedText("ReturnCodeText");
	AText	wintitle("TextEncoding Errors");
	ADocumentID	docID = GetApp().SPI_GetOrCreateFindResultWindowDocument();//#725
	AText	grouptitle1, grouptitle2;
	grouptitle1.SetLocalizedText("IndexWindow_Title_TextEncodingErrors");
	AFileAcc	basefolder;//#465
	NVI_GetParentFolder(basefolder);//#465
	GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_InsertGroup(0,grouptitle1,grouptitle2,basefolder);//#465
	//#208
	ABool	checkKishuIson = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCheckEncodingError_KishuIzon);
	ABool	checkHankakuKana = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCheckEncodingError_HankakuKana);
	//
	AItemCount	count = mText.GetItemCount();
	for( ATextIndex pos = 0; pos < count; )
	{
		ATextIndex	spos = pos;
		AText	ch;
		mText.Get1UTF8Char(pos,ch);
		ABool	convertError = false;//#208
		ABool	kishuIzon = false;//#208
		ABool	hankakuKana = false;//#208
		//#473 SJISエラー文字
		ABool	shiftJISNo2ndByte = false;
		if( ch.GetItemCount() == 4 )
		{
			if( ch.Get(0) == 0xF4 && ch.Get(1) == 0x85 )
			{
				shiftJISNo2ndByte = true;
			}
		}
		//
		AText	tmp;
		if( ch.ConvertFromUTF8CR(SPI_GetTextEncoding(),returnCode_CR,tmp) == false )
		{
			convertError = true;//#208
		}
		//#208
		else
		{
			ATextEncodingWrapper::CheckJISX0208(SPI_GetTextEncoding(),tmp,kishuIzon,hankakuKana);
		}
		if( convertError == true || 
					(kishuIzon == true && checkKishuIson == true) || 
					(hankakuKana == true && checkHankakuKana == true) || 
					(shiftJISNo2ndByte == true) )//#473
		{
			ATextPoint	textpoint;
			SPI_GetTextPointFromTextIndex(spos,textpoint);
			//
			AFileAcc	file;
			NVI_GetFile(file);
			AText	filepath;
			file.GetPath(filepath);
			//
			AText	prehit;
			ATextIndex	p = spos - kPreHitTextLength;
			if( p < 0 )   p = 0;
			p = mText.GetCurrentCharPos(p);
			mText.GetText(p,spos-p,prehit);
			//
			AText	posthit;
			p = pos + kPostHitTextLength;
			if( p >= mText.GetItemCount() )   p = mText.GetItemCount();
			p = mText.GetCurrentCharPos(p);
			mText.GetText(pos,p-pos,posthit);
			prehit.ReplaceChar(kUChar_CR,returntext);
			posthit.ReplaceChar(kUChar_CR,returntext);
			//場所情報取得
			AText	position;
			SPI_GetPositionTextByLineIndex(textpoint.y,position);
			//#208
			AText	comment;
			//#473
			if( shiftJISNo2ndByte == true )
			{
				comment.SetLocalizedText("TextEncodingError_ShiftJIS_No2ndByte");
			}
			//
			else if( kishuIzon == true )
			{
				comment.SetLocalizedText("TextEncodingError_KishuIzon");
				comment.AddText(ch);
			}
			else if( hankakuKana == true )
			{
				comment.SetLocalizedText("TextEncodingError_HankakuKana");
				comment.AddText(ch);
			}
			else
			{
				comment.SetLocalizedText("TextEncodingError_CannotConvert");
				comment.AddText(ch);
			}
			//#465
			AText	paratext;
			SPI_GetParagraphTextByLineIndex(textpoint.y,paratext);
			//
			GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_AddItem_TextPosition(0,filepath,ch,prehit,posthit,position,
					paratext,//#465
					spos,pos-spos,SPI_GetParagraphIndexByLineIndex(textpoint.y),comment);
		}
	}
}

//R0199
//スペルチェック一覧表示
void	ADocument_Text::SPI_ReportSpellCheck()
{
	AText	returntext;
	returntext.SetLocalizedText("ReturnCodeText");
	AText	wintitle("TextEncoding Errors");
	ADocumentID	docID = GetApp().SPI_GetOrCreateFindResultWindowDocument();//#725
	AText	grouptitle1, grouptitle2;
	grouptitle1.SetLocalizedText("IndexWindow_Title_Misspell");
	AFileAcc	basefolder;//#465
	NVI_GetParentFolder(basefolder);//#465
	GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_InsertGroup(0,grouptitle1,grouptitle2,basefolder);//#465
	for( AIndex lineIndex = 0; lineIndex < mTextInfo.GetLineCount(); lineIndex++ )
	{
		//
		CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
		ATextPoint	tmp1, tmp2;
		AAnalyzeDrawData	analyze;
		GetTextDrawDataWithStyle(mText,mTextInfo,lineIndex,textDrawData,false,tmp1,tmp2,analyze,kObjectID_Invalid);//#695
		//
		for( AIndex index = 0; index < textDrawData.misspellStartArray.GetItemCount(); index++ )
		{
			ATextPoint	textspt;
			textspt.x = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(textDrawData.misspellStartArray.Get(index));
			textspt.y = lineIndex;
			ATextPoint	textept;
			textept.x = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(textDrawData.misspellEndArray.Get(index));
			textept.y = lineIndex;
			ATextIndex	spos = SPI_GetTextIndexFromTextPoint(textspt);
			ATextIndex	epos = SPI_GetTextIndexFromTextPoint(textept);
			//
			AFileAcc	file;
			NVI_GetFile(file);
			AText	filepath;
			file.GetPath(filepath);
			//
			AText	prehit;
			ATextIndex	p = spos - kPreHitTextLength;
			if( p < 0 )   p = 0;
			p = mText.GetCurrentCharPos(p);
			mText.GetText(p,spos-p,prehit);
			//
			AText	posthit;
			p = epos + kPostHitTextLength;
			if( p >= mText.GetItemCount() )   p = mText.GetItemCount();
			p = mText.GetCurrentCharPos(p);
			mText.GetText(epos,p-epos,posthit);
			prehit.ReplaceChar(kUChar_CR,returntext);
			posthit.ReplaceChar(kUChar_CR,returntext);
			//場所情報取得
			AText	position;
			SPI_GetPositionTextByLineIndex(lineIndex,position);
			//#465
			AText	paratext;
			SPI_GetParagraphTextByLineIndex(textspt.y,paratext);
			//
			AText	text;
			SPI_GetText(spos,epos,text);
			GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_AddItem_TextPosition(0,filepath,text,prehit,posthit,position,
					paratext,//#465
					spos,epos-spos,SPI_GetParagraphIndexByLineIndex(lineIndex),GetEmptyText());
		}
	}
}

/**
指定行に対応するmenu identifierのテキストを取得
*/
void	ADocument_Text::SPI_GetPositionTextByLineIndex( const AIndex inLineIndex, AText& outText ) const
{
	outText.DeleteAll();
	//#695 AUniqID	identifierID = mTextInfo.GetMenuIdentifierByLineIndex(inLineIndex);
	//#695 if( identifierID != kUniqID_Invalid )
	AIndex	jumpIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);//#695
	if( jumpIndex != kIndex_Invalid )//#695
	{
		AUniqID identifierID = mJump_IdentifierObjectArray.Get(jumpIndex);//#695
		mTextInfo.GetGlobalIdentifierMenuText(identifierID,outText);
	}
}

void	ADocument_Text::SPI_GetTextCount( ATextCountData& ioTextCount, const AArray<ATextIndex>& inStart, const AArray<ATextIndex>& inEnd )
{
	//高速化や制限検討→用途的に高速化は不要と思われるため、バージョン2.1同等のロジックとする。#1328
	//#695
	ABool	supportComment = true;
	if( SPI_GetLineCount() > kLimit_TextCount_MaxLineCountForSyntaxRecognize )
	{
		supportComment = false;
	}
	//段落数
	ioTextCount.paragraphCount = SPI_GetParagraphCount();
	//
	ATextInfoForDocument	textInfo(this);
	textInfo.SetMode(SPI_GetModeIndex());
	//#695 AArray<AUniqID>	deletedIdentifier;
	AArray<AUniqID>	addedIdentifier;
	AArray<AIndex>		addedIdentifierLineIndex;
	AText	fontname;//win
	SPI_GetFontName(fontname);//win
	textInfo.CalcLineInfoAll(mText,true,kIndex_Invalid,/*win SPI_GetFont()*/fontname,SPI_GetFontSize(),true,SPI_GetTabWidth(),SPI_GetIndentWidth(),kWrapMode_WordWrapByLetterCount,//#699
				ioTextCount.conditionLetterCount,0,/*#695 deletedIdentifier,*/ioTextCount.countAs2Letters);
	SPI_InitLineImageInfo();//#450
	//
	ioTextCount.lineCountWithCondition = textInfo.GetLineCount();
	//文字数カウント
	ioTextCount.letterCount = 0;
	ioTextCount.commentLetterCount = 0;
	ioTextCount.letterCountInSelect = 0;
	ioTextCount.commentLetterCountInSelect = 0;
	ioTextCount.paragraphCountInSelect = 0;
	//#695 AIndex	startLineIndex, endLineIndex;
	ABool	importChanged = false;
	AFileAcc	file;//R0000
	NVI_GetFile(file);//R0000
	//
	if( supportComment == true )//#695
	{
		ABool	abortFlag = false;
		AText	url;//#998
		SPI_GetURL(url);//#998
		textInfo.RecognizeSyntaxAll(mText,url,addedIdentifier,addedIdentifierLineIndex,importChanged,abortFlag);//R0000 #698 #998
	}
	AArray<ABool>	isCommentArray;
	AItemCount	selArrayCount = inStart.GetItemCount();
	ABool	selparaflag = false;
	for( AIndex lineIndex = 0; lineIndex < textInfo.GetLineCount(); lineIndex++ )
	{
		if( supportComment == true )//#695
		{
			textInfo.GetIsCommentArray(mText,lineIndex,isCommentArray);
		}
		ATextIndex	spos = textInfo.GetLineStart(lineIndex);
		ATextIndex	epos = spos + textInfo.GetLineLengthWithLineEnd(lineIndex);
		for( ATextIndex pos = spos; pos < epos;  )
		{
			AItemCount	bc = mText.GetUTF8ByteCount(pos);
			AUChar	ch = mText.Get(pos);
			ABool	isComment = false;//#695
			if( supportComment == true )//#695
			{
				isComment = isCommentArray.Get(pos-spos);
			}
			ABool	inSelect = false;
			for( AIndex i = 0; i < selArrayCount; i++ )
			{
				if( inStart.Get(i) <= pos && pos < inEnd.Get(i) )
				{
					inSelect = true;
					break;
				}
			}
			if( inSelect )
			{
				if( ch == kUChar_LineEnd )
				{
					ioTextCount.paragraphCountInSelect++;
					selparaflag = false;
				}
				else
				{
					selparaflag = true;
				}
			}
			if( ioTextCount.countAs2Letters == true && bc >= 3 )
			{
				ioTextCount.letterCount += 2;
				if( isComment == true )
				{
					ioTextCount.commentLetterCount += 2;
				}
				if( inSelect == true )
				{
					ioTextCount.letterCountInSelect += 2;
					if( isComment == true )
					{
						ioTextCount.commentLetterCountInSelect += 2;
					}
				}
			}
			else
			{
				if( (ch==kUChar_Space||ch==kUChar_Tab) && ioTextCount.countSpaceTab == false )
				{
					//カウントしない
				}
				else if( ch == kUChar_LineEnd && ioTextCount.countReturn == false )
				{
					//カウントしない
				}
				else
				{
					ioTextCount.letterCount++;
					if( isComment == true )
					{
						ioTextCount.commentLetterCount++;
					}
					if( inSelect == true )
					{
						ioTextCount.letterCountInSelect ++;
						if( isComment == true )
						{
							ioTextCount.commentLetterCountInSelect ++;
						}
					}
				}
			}
			pos += bc;
		}
	}
	if( selparaflag == true )   ioTextCount.paragraphCountInSelect++;
	//#695
	if( supportComment == false )
	{
		ioTextCount.commentLetterCount = kIndex_Invalid;
		ioTextCount.commentLetterCountInSelect = kIndex_Invalid;
	}
}

void	ADocument_Text::SPI_GetTextForOpenSelected( const ATextPoint& inSelectStart, const ATextPoint& inSelectEnd, AText& outPathText ) const
{
	outPathText.DeleteAll();
	ATextIndex	spos = SPI_GetTextIndexFromTextPoint(inSelectStart);
	for( ATextIndex	pos = SPI_GetPrevCharTextIndex(spos); pos > 0; pos = SPI_GetPrevCharTextIndex(pos) )
	{
		AUChar	ch = mText.Get(pos);
		if( SPI_IsAsciiAlphabet(pos) == false && ch != '.' && ch != '/' && ch != ':' && ch != '%' && ch != '~' &&
				ch != '?' && ch != '(' && ch != ')' && ch != '-' && ch != '=' )
		{
			spos = SPI_GetNextCharTextIndex(pos);
			break;
		}
	}
	ATextIndex	epos = SPI_GetTextIndexFromTextPoint(inSelectEnd);
	for( ATextIndex	pos = epos; pos < mText.GetItemCount(); pos = SPI_GetNextCharTextIndex(pos) )
	{
		AUChar	ch = mText.Get(pos);
		if( SPI_IsAsciiAlphabet(pos) == false && ch != '.' && ch != '/' && ch != ':' && ch != '%' && ch != '~' &&
				ch != '?' && ch != '(' && ch != ')' && ch != '-' && ch != '=' )
		{
			epos = pos;
			break;
		}
	}
	if( epos-spos <= 0 )   return;
	mText.GetText(spos,epos-spos,outPathText);
}

//#142
/**
文字数・単語数取得（指定範囲）
*/
void	ADocument_Text::SPI_GetWordCount( const ATextPoint& inStartTextPoint, const ATextPoint& inEndTextPoint,
		AItemCount& outCharCount, AItemCount& outWordCount, AItemCount& outParagraphCount ) const
{
	AIndex	spos = SPI_GetTextIndexFromTextPoint(inStartTextPoint);
	AIndex	epos = SPI_GetTextIndexFromTextPoint(inEndTextPoint);
	mTextInfo.GetWordCount(mText,spos,epos-spos,outCharCount,outWordCount,outParagraphCount);
}

/**
SyntaxWarning取得
*/
ABool	ADocument_Text::SPI_GetSyntaxWarning( const AIndex inLineIndex ) const
{
	return mTextInfo.GetSyntaxWarning(inLineIndex);
}

/**
SyntaxError取得
*/
ABool	ADocument_Text::SPI_GetSyntaxError( const AIndex inLineIndex ) const
{
	return mTextInfo.GetSyntaxError(inLineIndex);
}

//#992
/**
文法チェッカーデータ全削除
*/
void	ADocument_Text::SPI_ClearCheckerPluginWarnings()
{
	mCheckerPlugin_ParagraphNumber.DeleteAll();
	mCheckerPlugin_ColIndex.DeleteAll();
	mCheckerPlugin_TitleText.DeleteAll();
	mCheckerPlugin_DetailText.DeleteAll();
	mCheckerPlugin_TypeIndex.DeleteAll();
	mCheckerPlugin_DisplayInTextView.DeleteAll();
	//描画更新
	SPI_RefreshTextViews();
}

//#992
/**
文法チェッカーデータ追加
*/
void	ADocument_Text::SPI_AddCheckerPluginWarning( const ANumber inParagraphNumber, const AIndex inColIndex,
													 const AText& inTitleText, const AText& inDetailText,
													 const AIndex inTypeIndex, const ABool inDisplayInTextView )
{
	mCheckerPlugin_ParagraphNumber.Add(inParagraphNumber);
	mCheckerPlugin_ColIndex.Add(inColIndex);
	mCheckerPlugin_TitleText.Add(inTitleText);
	mCheckerPlugin_DetailText.Add(inDetailText);
	mCheckerPlugin_TypeIndex.Add(inTypeIndex);
	mCheckerPlugin_DisplayInTextView.Add(inDisplayInTextView);
	//描画更新
	AIndex	lineIndex = SPI_GetLineIndexByParagraphIndex(inParagraphNumber);
	SPI_RefreshTextViews(lineIndex,lineIndex+1);
}

//#992
/**
文法チェッカーデータ取得
*/
void	ADocument_Text::SPI_GetCheckerPluginWarnings( const ANumber inParagraphNumber, 
													  ANumberArray& outColIndexArray, 
													  ATextArray& outTitleTextArray, ATextArray& outDetailTextArray,
													  AColorArray& outColorArray, ABoolArray& outDisplayInTextViewArray ) const
{
	//結果前削除
	outColIndexArray.DeleteAll();
	outTitleTextArray.DeleteAll();
	outDetailTextArray.DeleteAll();
	outColorArray.DeleteAll();
	outDisplayInTextViewArray.DeleteAll();
	//指定段落のデータを取得
	AArray<AIndex>	indexArray;
	mCheckerPlugin_ParagraphNumber.FindAll(inParagraphNumber,indexArray);
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		//index取得
		AIndex	index = indexArray.Get(i);
		//ワーニングタイプ取得
		AIndex	typeIndex = mCheckerPlugin_TypeIndex.Get(index);
		//タイプが正のときだけワーニングに追加
		if( typeIndex > 0 )
		{
			//タイプに従って色取得
			AColor	color = kColor_White;
			switch(typeIndex)
			{
			  case 1:
			  default:
				{
					color = kColor_White;
					break;
				}
			  case 2:
				{
					color = kColor_Yellow;
					break;
				}
			  case 3:
				{
					color = kColor_Red;
					break;
				}
			}
			//データ取得
			outColIndexArray.Add(mCheckerPlugin_ColIndex.Get(index));
			outTitleTextArray.Add(mCheckerPlugin_TitleText.GetTextConst(index));
			outDetailTextArray.Add(mCheckerPlugin_DetailText.GetTextConst(index));
			outColorArray.Add(color);
			outDisplayInTextViewArray.Add(mCheckerPlugin_DisplayInTextView.Get(index));
		}
	}
}

#pragma mark ===========================

#pragma mark ---TextPoint/TextIndex変換

//TextPointからIndex取得
AIndex	ADocument_Text::SPI_GetTextIndexFromTextPoint( const ATextPoint& inPoint ) const
{
	//行計算がまだ（行情報が全くない場合）の場合用。
	//(0,0)の場合は、必ず0を返す。
	if( inPoint.x == 0 && inPoint.y == 0 )
	{
		return 0;
	}
	//
	return mTextInfo.GetTextIndexFromTextPoint(inPoint);
}

//IndexからTextPoint取得
void	ADocument_Text::SPI_GetTextPointFromTextIndex( const AIndex inTextPosition, ATextPoint& outPoint, const ABool inPreferNextLine ) const
{
	mTextInfo.GetTextPointFromTextIndex(inTextPosition,outPoint,inPreferNextLine);
}

#pragma mark ===========================

#pragma mark ---ドキュメント情報設定／取得

//URL取得 #994
void	ADocument_Text::SPI_GetURL( AText& outURL ) const
{
	if( SPI_IsRemoteFileMode() == true )
	{
		SPI_GetRemoteFileURL(outURL);
	}
	else if( NVI_IsFileSpecified() == false )
	{
		SPI_GetTemporaryTitle(outURL);
	}
	else
	{
		AFileAcc	file;
		NVI_GetFile(file);
		file.GetPath(outURL);
		outURL.InsertCstring(0,"file://");
	}
}

//タイトル取得
void	ADocument_Text::NVIDO_GetTitle( AText& outTitle ) const
{
	if( SPI_IsRemoteFileMode() == true )
	{
		//URLだと長くてタブに表示しきれないので、ファイル名＋(プロトコル名)のみにする #1247
		AText	url;
		SPI_GetRemoteFileURL(url);
		url.GetFilename(outTitle);
		AText	protocol;
		url.GetFTPURLProtocol(protocol);
		outTitle.AddCstring(" (");
		outTitle.AddText(protocol);
		outTitle.AddCstring(")");
	}
	else if( NVI_IsFileSpecified() == false )
	{
		SPI_GetTemporaryTitle(outTitle);
	}
	else
	{
		AFileAcc	file;
		NVI_GetFile(file);
		file.GetFilename(outTitle);
	}
}

//#699
/**
書き込み可否判定処理
*/
ABool	ADocument_Text::NVIDO_IsReadOnly() const
{
	//ADocumentのmReadOnlyフラグがonならread only
	if( mReadOnly == true )   return true;
	//行折り返し計算中ならread only
	if( SPI_GetDocumentInitState() == kDocumentInitState_WrapCalculating || mWrapCalculatorWorkingByInsertText == true )
	{
		return true;
	}
	//上記以外は書き込み可
	return false;
}

//R0242
void	ADocument_Text::NVIDO_GetPrintMargin( AFloatNumber& outLeftMargin, AFloatNumber& outRightMargin, AFloatNumber& outTopMargin, AFloatNumber& outBottomMargin ) const
{
	outLeftMargin	= GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintMargin_Left);
	outRightMargin	= GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintMargin_Right);
	outTopMargin	= GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintMargin_Top);
	outBottomMargin	= GetApp().NVI_GetAppPrefDB().GetData_FloatNumber(AAppPrefDB::kPrintMargin_Bottom);
}

//ファイル名取得
void	ADocument_Text::SPI_GetFilename( AText& outFilename ) const
{
	if( SPI_IsRemoteFileMode() == true )
	{
		AText	text;
		SPI_GetRemoteFileURL(text);
		text.GetFilename(outFilename);
	}
	else if( NVI_IsFileSpecified() == false )
	{
		SPI_GetTemporaryTitle(outFilename);
	}
	else
	{
		AFileAcc	file;
		NVI_GetFile(file);
		file.GetFilename(outFilename);
	}
}

//TextEncoding取得
ATextEncoding	ADocument_Text::SPI_GetTextEncoding() const
{
	//#764 return mTextEncoding;
	AText	tecname;
	GetDocPrefDBConst().GetData_Text(ADocPrefDB::kTextEncodingNameCocoa,tecname);//#1040
	return ATextEncodingWrapper::GetTextEncodingFromName(tecname);
}

//TextEncoding名取得
void	ADocument_Text::SPI_GetTextEncoding( AText& outTextEncodingName ) const
{
	GetDocPrefDBConst().GetData_Text(ADocPrefDB::kTextEncodingNameCocoa,outTextEncodingName);//#1040
}

/*#934
//TextEncoding設定（ATextEncodingから設定）
void	ADocument_Text::SPI_SetTextEncoding( const ATextEncoding inTextEncoding )
{
	AText	text;
	ATextEncodingWrapper::GetTextEncodingName(inTextEncoding,text);
	SPI_SetTextEncoding(text);
}
*/

//TextEncoding設定（TextEncoding名から設定）
void	ADocument_Text::SPI_SetTextEncoding( const AText& inTextEncodingName )
{
	//ReadOnlyの場合は何もせずリターン
	if( NVI_IsReadOnly() == true )   return;
	//DBに設定
	//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
	GetDocPrefDB().SetTextEncoding(inTextEncodingName);//#1040
	/*#764
	//mTextEncodingに設定
	if( ATextEncodingWrapper::GetTextEncodingFromName(inTextEncodingName,mTextEncoding) == false )
	{
		//TextEncodingのチェックはこの関数をコールする前に行わなければならない。
		_ACError("text encoding not found",this);
	}
	*/
	//ウインドウのツールバーを更新
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateToolbarItemValue(GetUniqID());
	}
	*/
	GetApp().SPI_UpdateToolbarItemValue(GetObjectID());//#379
	//SetDirty
	NVI_SetDirty(true);
}

//TextEncoding修正
void	ADocument_Text::SPI_ModifyTextEncoding( const AText& inTextEncodingName, ATextEncodingFallbackType& outResultFallbackType ) //#473
{
	outResultFallbackType = kTextEncodingFallbackType_None;//#473
	
	//Dirtyの場合はエラー
	if( NVI_IsDirty() == true )   { _ACError("",this); return; }
	
	//テキストエンコーディング設定
	//#1040 GetDocPrefDB().SetData_Text(ADocPrefDB::kTextEncodingName,inTextEncodingName);
	SPI_SetTextEncoding(inTextEncodingName);//#1040
	//#764 ATextEncodingWrapper::GetTextEncodingFromName(inTextEncodingName,mTextEncoding);
	
	//比較情報削除
	ClearDiffDisplay();
	
	//行情報、identifierをクリア
	//#695 AArray<AUniqID>	deletedIdentifier;
	mTextInfo.DeleteLineInfoAll(/*#695 deletedIdentifier*/);
	//#695 DeleteFromJumpMenu(deletedIdentifier);
	//ジャンプ項目全削除
	DeleteAllJumpItems();
	//インポートファイルデータ消去
	//#349 UpdateImportFileData();
	SPI_DeleteAllImportFileData();//#349
	
	//#81
	//最後にロード／セーブした時のテキストエンコーディング（＝ファイルの現在のテキストエンコーディング）を記憶→自動バックアップ時に保存
	SPI_GetTextEncoding(mTextEncodingWhenLastLoadOrSave);
	
	//テキスト再読込
	{
		AStMutexLocker	locker(mTextMutex);//#890
		mText.DeleteAll();
		if( SPI_IsRemoteFileMode() == true )
		{
			mText.SetText(mTextOnRemoteServer);
		}
		else
		{
			if( NVI_IsFileSpecified() == true )
			{
				AFileAcc	file;
				NVI_GetFile(file);
				file.ReadTo(mText);
			}
		}
	}
	
	//テキストを内部形式へ変換
	if( mText.GetItemCount() > 0 )
	{
		//UTF8へ変換
		ATextEncoding	tec = SPI_GetTextEncoding();
		ATextEncoding	resultTec = tec;
		AReturnCode	resultReturnCode = static_cast<AReturnCode>(GetDocPrefDB().GetData_Number(ADocPrefDB::kReturnCode));//#307
		ABool	convertSuccessful = true;
		ABool	notMixed = true;//#995
		{
			AStMutexLocker	locker(mTextMutex);//#890
			//指定tecでの変換なので、フォールバック、SJISロスレスフォールバックは両方とも有りにする。
			//返り値convertSuccessfulはフォールバックを使ってでも変換ができたときはtrueになる。
			convertSuccessful = GetApp().
					SPI_ConvertToUTF8CR(mText,tec,true,
										true,//フォールバック
										false,//#844 SJISロスレスフォールバックはdropGetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kUseSJISLosslessFallback),
										resultReturnCode,resultTec,resultReturnCode,outResultFallbackType,notMixed);//#995
			//5cをa5に変換（バックスラッシュを\で表示する）
			//#940 Convert5CToA5WhenOpenJIS(mText);
		}
	}
	
	/*#699
	//行情報計算
	AText	fontname;//win
	SPI_GetFontName(fontname);//win
	mTextInfo.CalcLineInfoAll(mText,true,kIndex_Invalid,fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),//#699
				SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
	//ジャンプ項目全削除
	DeleteAllJumpItems();
	//
	SPI_InitLineImageInfo();//#450
	//折りたたみデータをTextviewに反映 #450
	ApplyDocPref_CollapsedLine();
	//行チェックデータを反映 #842
	ApplyDocPref_LineCheckedDate();
	//文法解析
	StartSyntaxRecognizer(0);
	*/
	
	//ドキュメント状態遷移。行折り返しから再計算する
	//（行折り返し計算状態に遷移し、スレッドで行計算を行う。）
	ReTransitInitState_Recalculate();
	
	//ウインドウのツールバーを更新
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateToolbarItemValue(GetObjectID());
	}
	*/
	GetApp().SPI_UpdateToolbarItemValue(GetObjectID());//#379
	//
	//#212 RefreshWindow();
	SPI_RefreshTextViews();//#212
	
	//#378
	SPI_UpdateJumpList();
	
	//#413
	SPI_CorrectCaretIfInvalid();
	
	//#1040
	NVI_SetDirty(false);
}

//行情報再計算
void	ADocument_Text::SPI_DoWrapRelatedDataChanged( const ABool inForceRecalc )
{
	//R006 行情報が変わるので変更箇所情報も消去する
	//#379 Diffデータは段落単位なので変化しない ClearDiffDisplay();
	ClearArrowToDef();//RC1
	
	//selection修正のため
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPreProcess(0,0);
	}
	//モードデータ変更後に呼ばれることが多いので、ここでregexpの再構成などやってしまう
	mTextInfo.SetMode(mModeIndex);
	
	if( SPI_GetWrapMode() == kWrapMode_WordWrap || SPI_GetWrapMode() == kWrapMode_LetterWrap || //#1113
		inForceRecalc == true )
	{
		/*#699
		//行情報再計算
		AText	fontname;//win
		SPI_GetFontName(fontname);//win
		mTextInfo.CalcLineInfoAll(mText,true,kIndex_Invalid,fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),//#699
					SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),
					GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
		//ジャンプ項目全削除
		DeleteAllJumpItems();
		//インポートファイルデータ消去
		//#349 UpdateImportFileData();
		SPI_DeleteAllImportFileData();//#349
		//文法解析
		StartSyntaxRecognizer(0);
		*/
		//
		
		//ドキュメント状態遷移。行折り返しから再計算する
		//（行折り返し計算状態に遷移し、スレッドで行計算を行う。）
		ReTransitInitState_Recalculate();
	}
	
	//#450 フォントサイズ変更時等で行高さ情報更新する
	SPI_InitLineImageInfo();
	
	//selection修正のため
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_EditPostProcess(0,0,false);
	}
	
	//#218 diff情報更新
	//#379 LineInfo更新してもDiffデータは更新不要なためコメントアウト（Diff情報は段落単位） SPI_UpdateDiffDisplayData();
}

/**
フォント設定
@note フォントを設定し、モードフォントは使用しない設定にする
*/
void	ADocument_Text::SPI_SetFontName( const AText& inText )
{
	AFloatNumber	fontsize = GetDocPrefDB().GetData_FloatNumber(ADocPrefDB::kTextFontSize);
	SPI_SetFontNameAndSize(inText,fontsize);
}

/**
フォント名の取得
@note モードフォントを使用する設定の場合は、モードフォントを取得
*/
void	ADocument_Text::SPI_GetFontName( AText& outText ) const
{
	if( GetDocPrefDBConst().GetData_Bool(ADocPrefDB::kUseModeFont) == true )
	{
		//モードのフォントを使う場合
		GetApp().SPI_GetModePrefDB(mModeIndex).GetModeData_Text(AModePrefDB::kDefaultFontName,outText);
	}
	else
	{
		//ドキュメントのフォントを使う場合
		GetDocPrefDBConst().GetData_Text(ADocPrefDB::kTextFontName,outText);
	}
}

/**
フォントサイズの設定
@note フォントサイズを設定し、モードフォントは使用しない設定にする
*/
void	ADocument_Text::SPI_SetFontSize( const AFloatNumber inFontSize )
{
	AText	fontname;
	GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,fontname);
	SPI_SetFontNameAndSize(fontname,inFontSize);
}

/**
フォントサイズの取得
@note モードフォントを使用する設定の場合は、モードフォントサイズを取得
*/
AFloatNumber	ADocument_Text::SPI_GetFontSize() const
{
	//#966
	//一時フォントサイズが設定されていたらそれを返す
	if( mTemporaryFontSize > 0 )
	{
		return mTemporaryFontSize;
	}
	
	//
	if( GetDocPrefDBConst().GetData_Bool(ADocPrefDB::kUseModeFont) == true )
	{
		//モードのフォントを使う場合
		return GetApp().SPI_GetModePrefDB(mModeIndex).GetModeData_FloatNumber(AModePrefDB::kDefaultFontSize);
	}
	else
	{
		//ドキュメントのフォントを使う場合
		return GetDocPrefDBConst().GetData_FloatNumber(ADocPrefDB::kTextFontSize);
	}
}

//#354
/**
フォント・フォントサイズの設定
@note フォント・フォントサイズを設定し、モードフォントは使用しない設定にする
*/
void	ADocument_Text::SPI_SetFontNameAndSize( const AText& inFontName, const AFloatNumber inFontSize )
{
	//一時フォントサイズクリア #966
	mTemporaryFontSize = 0;
	
	//==================フォント名・サイズ補正==================
	//#521 使用出来ないフォントの場合はdefaultフォントに補正する
	AText	fontname;
	fontname.SetText(inFontName);
	if( AFontWrapper::IsFontEnabled(fontname) == false )
	{
		AFontWrapper::GetAppDefaultFontName(fontname);//#868
	}
	
	//フォントサイズが小さすぎる場合補正
	AFloatNumber	fontsize = inFontSize;
	if( fontsize < 6.0 )   fontsize = 6.0;//B0398 最小値変更
	
	//#868
	//変更無しの場合、何もせずリターン
	AText	origfontname;
	GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,origfontname);
	AFloatNumber	origfontsize = GetDocPrefDB().GetData_FloatNumber(ADocPrefDB::kTextFontSize);
	ABool	origUseModeFont = GetDocPrefDB().GetData_Bool(ADocPrefDB::kUseModeFont);
	if( fontname.Compare(origfontname) == true && fontsize == origfontsize && origUseModeFont == false )
	{
		return;
	}
	
	//DB設定
	GetDocPrefDB().SetData_Text(ADocPrefDB::kTextFontName,fontname);
	GetDocPrefDB().SetData_FloatNumber(ADocPrefDB::kTextFontSize,fontsize);
	
	//モードのフォントは使用しないように設定
	GetDocPrefDB().SetData_Bool(ADocPrefDB::kUseModeFont,false);
	
	//フォント変更をドキュメント・ビューデータに反映
	UpdateDocumentByFontChange();
}

//#354
/**
フォント・サイズをRevertする
*/
void	ADocument_Text::SPI_RevertFontNameAndSize()
{
	SPI_SetFontNameAndSize(mOriginalFontName,mOriginalFontSize);
}

//#868
/**
モードのフォントを使うかどうかの設定を行う
*/
void	ADocument_Text::SPI_SetUseModeFont( const ABool inUseModeFont )
{
	//変更無しの場合、何もせず終了
	if( GetDocPrefDB().GetData_Bool(ADocPrefDB::kUseModeFont) == inUseModeFont )
	{
		return;
	}
	//モードのフォントを使うかどうかを設定
	GetDocPrefDB().SetData_Bool(ADocPrefDB::kUseModeFont,inUseModeFont);
	//フォント変更をドキュメント・ビューデータに反映
	UpdateDocumentByFontChange();
}

//#868
/**
モードのフォントを使うかどうかを取得
*/
ABool	ADocument_Text::SPI_GetUseModeFont() const
{
	return GetDocPrefDBConst().GetData_Bool(ADocPrefDB::kUseModeFont);
}

//#966
/**
一時フォントサイズ設定
*/
void	ADocument_Text::SPI_SetTemporaryFontSize( const AFloatNumber inFontSize )
{
	//一時フォントサイズ設定
	mTemporaryFontSize = inFontSize;
	//フォント変更をドキュメント・ビューデータに反映
	UpdateDocumentByFontChange();
}

//#868
/**
モード設定のテキストフォントが変更されたときのドキュメントへの反映処理
*/
void	ADocument_Text::SPI_TextFontSetInModeIsUpdated( const AIndex inUpdatedModeIndex )
{
	//モードのフォントを使用する設定ではない場合は、何もせず終了
	if( SPI_GetUseModeFont() == false )
	{
		return;
	}
	
	//このドキュメントに反映するかどうかのフラグ
	ABool	related = false;
	if( inUpdatedModeIndex == SPI_GetModeIndex() )
	{
		//モードが同じなら反映フラグON
		related = true;
	}
	if( inUpdatedModeIndex == kStandardModeIndex )
	{
		if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSameAsNormalMode_Font) == true )
		{
			//変更されたモードが標準モードで、かつ、このドキュメントのモードが「標準モードと同じ」設定なら、反映フラグON
			related = true;
		}
	}
	//フォント変更をドキュメント・ビューデータに反映
	if( related == true )
	{
		UpdateDocumentByFontChange();
	}
}

//#868
/**
フォント変更をドキュメント・ビューデータに反映
*/
void	ADocument_Text::UpdateDocumentByFontChange()
{
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		return;
	}
	
	//行情報再計算
	SPI_DoWrapRelatedDataChanged(false);
	
	//ウインドウ、および、PrintImpの描画プロパティ更新
	GetApp().SPI_UpdateViewBounds(GetObjectID());//行番号の幅更新、スクロールバーの表示更新など
	GetApp().SPI_UpdateTextDrawProperty(GetObjectID());//#379
	
	//各ビューのline image infoを更新
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateLineImageInfoAllByCollapsedParameter();
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshTextView();
	}
}

//改行コードの設定
void	ADocument_Text::SPI_SetReturnCode( const AReturnCode inReturnCode )
{
	//ReadOnlyの場合は何もせずリターン
	if( NVI_IsReadOnly() == true )   return;
	//DB設定
	GetDocPrefDB().SetData_Number(ADocPrefDB::kReturnCode,inReturnCode);
	//ウインドウのツールバーを更新
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateToolbarItemValue(GetObjectID());
	}
	*/
	GetApp().SPI_UpdateToolbarItemValue(GetObjectID());//#379
	//SetDirty
	NVI_SetDirty(true);
}

//改行コードの取得
AReturnCode	ADocument_Text::SPI_GetReturnCode() const
{
	return static_cast<AReturnCode>(GetDocPrefDBConst().GetData_Number(ADocPrefDB::kReturnCode));
}

//行送りモード設定
void	ADocument_Text::SPI_SetWrapMode( const AWrapMode inWrapMode, const ANumber inLetterCount )
{
	//設定が変化しない場合はリターン
	if( inWrapMode == kWrapMode_WordWrapByLetterCount || inWrapMode == kWrapMode_LetterWrapByLetterCount )//#1113
	{
		if( static_cast<AWrapMode>(GetDocPrefDBConst().GetData_Number(ADocPrefDB::kWrapMode)) == inWrapMode && 
		GetDocPrefDBConst().GetData_Number(ADocPrefDB::kWrapMode_LetterCount) == inLetterCount )   return;
	}
	else
	{
		if( static_cast<AWrapMode>(GetDocPrefDBConst().GetData_Number(ADocPrefDB::kWrapMode)) == inWrapMode )   return;
	}
	//DB設定
	GetDocPrefDB().SetData_Number(ADocPrefDB::kWrapMode,inWrapMode);
	if( inWrapMode == kWrapMode_WordWrapByLetterCount || inWrapMode == kWrapMode_LetterWrapByLetterCount )//#1113
	{
		GetDocPrefDB().SetData_Number(ADocPrefDB::kWrapMode_LetterCount,inLetterCount);
	}
	
	//関連するウインドウへLineInfoが変更されることを通知
	/*#199
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_DoLineInfoChangeStart(GetObjectID());
	}
	*/
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_DoLineInfoChangeStart();
	}
	
	//行情報再計算
	SPI_DoWrapRelatedDataChanged(true);
	
	//関連するウインドウへ通知
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		//#199 GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_DoLineInfoChangeEnd(GetObjectID());
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateToolbarItemValue(GetObjectID());
	}
	*/
	GetApp().SPI_UpdateToolbarItemValue(GetObjectID());//#379
	//#199
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_DoLineInfoChangeEnd();
	}
	
	//横スクロールバー表示有無更新 #1068
	GetApp().SPI_UpdateViewBounds(GetObjectID());
	
	//ウインドウ、および、PrintImpの描画プロパティ更新
	//#379 UpdateTextDrawProperty();
	GetApp().SPI_UpdateTextDrawProperty(GetObjectID());//#379
	
	/*#1192 行折り返しのみ変更の場合はdirtyにしない。（ExecuteClose()内でSPI_SaveDocPrefIfLoaded()をコールしているのでdoc prefは保存される）
	//SetDirty
	if( NVI_IsReadOnly() == false )
	{
		NVI_SetDirty(true);
	}
	*/
}

//行送りモード取得
AWrapMode	ADocument_Text::SPI_GetWrapMode() const
{
	return static_cast<AWrapMode>(GetDocPrefDBConst().GetData_Number(ADocPrefDB::kWrapMode));
}

//文字数ワードラップの文字数取得
ANumber	ADocument_Text::SPI_GetWrapLetterCount() const
{
	return GetDocPrefDBConst().GetData_Number(ADocPrefDB::kWrapMode_LetterCount);
}

/*#844
//タブ幅設定
void	ADocument_Text::SPI_SetTabWidth( const ANumber inTabWidth )
{
	//DB設定
	GetDocPrefDB().SetData_Number(ADocPrefDB::kTabWidth,inTabWidth);
	
	//行情報再計算
	SPI_DoWrapRelatedDataChanged(false);
	
	//ウインドウ、および、PrintImpの描画プロパティ更新
	//#379 UpdateTextDrawProperty();
	GetApp().SPI_UpdateTextDrawProperty(GetObjectID());//#379
	
	//SetDirty
	if( NVI_IsReadOnly() == false )
	{
		NVI_SetDirty(true);
	}
}
*/

//タブ幅取得
ANumber	ADocument_Text::SPI_GetTabWidth() const
{
	//#868 return GetDocPrefDBConst().GetData_Number(ADocPrefDB::kTabWidth);
	return GetApp().SPI_GetModePrefDB(mModeIndex).GetModeData_Number(AModePrefDB::kDefaultTabWidth);//#868
}

AItemCount	ADocument_Text::SPI_GetIndentWidth() const
{
	//#868 return GetDocPrefDBConst().GetData_Number(ADocPrefDB::kIndentWidth);
	//#912
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == true )
	{
		//インデント文字：スペースの場合は、インデント幅設定の値を返す
		return GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Number(AModePrefDB::kIndentWidth);//#868
	}
	else
	{
		//インデント文字：タブの場合は、常にタブ表示幅の値を返す。
		//（タブの場合に、インデント幅を設定するというのが、分かりづらいため。インデント幅はあくまでスペースのときのスペース文字列の設定にする。）#912
		return GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Number(AModePrefDB::kDefaultTabWidth);
	}
}

/*#844
void	ADocument_Text::SPI_SetIndentWidth( const AItemCount inIndentWidth )
{
	return GetDocPrefDB().SetData_Number(ADocPrefDB::kIndentWidth,inIndentWidth);
}

void	ADocument_Text::SPI_SetAntiAliasMode( const ANumber inMode )
{
	//DB設定
	GetDocPrefDB().SetData_Number(ADocPrefDB::kAntiAliasMode,inMode);
	
	//行情報再計算
	SPI_DoWrapRelatedDataChanged(false);
	
	//ウインドウ、および、PrintImpの描画プロパティ更新
	//#379 UpdateTextDrawProperty();
	GetApp().SPI_UpdateTextDrawProperty(GetObjectID());//#379
	
	//SetDirty
	if( NVI_IsReadOnly() == false )
	{
		NVI_SetDirty(true);
	}
}

AAntiAliasMode	ADocument_Text::SPI_GetAntiAliasMode() const
{
	return static_cast<AAntiAliasMode>(GetDocPrefDBConst().GetData_Number(ADocPrefDB::kAntiAliasMode));
}
*/

/**
アンチエイリアス使用するかどうかの判定
*/
ABool	ADocument_Text::SPI_IsAntiAlias() const
{
	/*#844
	switch(SPI_GetAntiAliasMode())
	{
	  case kAntiAliasMode_Default:
		{
			return GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kDefaultEnableAntiAlias);
		}
	  case kAntiAliasMode_On:
		{
			return true;
		}
	  case kAntiAliasMode_Off:
		{
			return false;
		}
	  default:
		{
			return true;
		}
	}
	*/
	return GetApp().NVI_GetAppPrefDBConst().GetData_Bool(AAppPrefDB::kEnableAntiAlias);//#844
}

/**
モード設定（モード名からモード設定）
*/
void	ADocument_Text::SPI_SetModeByRawName( const AText& inModeName )
{
	AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(inModeName);
	if( modeIndex != kIndex_Invalid )
	{
		SPI_SetMode(modeIndex);
	}
}

/**
モード設定（モード表示名からモード設定）
*/
void	ADocument_Text::SPI_SetModeByDisplayName( const AText& inModeName )
{
	AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeDisplayName(inModeName);
	if( modeIndex != kIndex_Invalid )
	{
		SPI_SetMode(modeIndex);
	}
}

/**
モード設定
*/
void	ADocument_Text::SPI_SetMode( const AModeIndex inModeIndex )
{
	//モードがdisable中なら何もしない
	if( GetApp().SPI_GetModePrefDB(inModeIndex,false).IsModeEnabled() == false )
	{
		return;
	}
	
	//行情報クリア後、ReTransitInitState_Recalculate()内でキャレット初期化(SPI_InitSelect())する前に、
	//モードのロード処理内で、textviewの選択関連処理が動作してしまわないように、モードのロードをここで行っておく。
	GetApp().SPI_GetModePrefDB(inModeIndex);
	
	//比較情報削除
	ClearDiffDisplay();
	
	//B0071 identifierをクリアしてからモード切り替えする
	//行情報、identifierをクリア
	//#695 AArray<AUniqID>	deletedIdentifier;
	mTextInfo.DeleteLineInfoAll(/*#695 deletedIdentifier*/);
	//#695 DeleteFromJumpMenu(deletedIdentifier);
	//ジャンプ項目全削除
	DeleteAllJumpItems();
	/*
	//
	{
		AStMutexLocker	locker(mCurrentLocalIdentifierMutex);//#717
		mCurrentLocalIdentifier.DeleteAll();
	}
	mCurrentLocalIdentifierStartLineIndex = kIndex_Invalid;
	mCurrentLocalIdentifierEndLineIndex = kIndex_Invalid;
	*/
	//#699
	//ローカル識別子データ全削除
	SPI_DeleteAllLocalIdentifiers();
	
	//インポートファイルデータ消去
	//#349 UpdateImportFileData();
	SPI_DeleteAllImportFileData();//#349
	
	//モード設定
	AModeIndex	modeIndex = inModeIndex;
	if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
	SetModeIndex(modeIndex);//#890
	
	//モード名をdoc prefに設定
	AText	text;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeRawName(text);
	GetDocPrefDB().SetData_Text(ADocPrefDB::kModeName,text);
	
	//text infoにモード設定
	mTextInfo.SetMode(mModeIndex);
	
	/*#699
	//行情報計算
	AText	fontname;//win
	SPI_GetFontName(fontname);//win
	//#695 deletedIdentifier.DeleteAll();//#571
	mTextInfo.CalcLineInfoAll(mText,true,kIndex_Invalid,fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),//#699
				SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),GetTextViewWidth(),//#695 deletedIdentifier,
				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
	//#695 DeleteFromJumpMenu(deletedIdentifier);//#571
	//ジャンプ項目全削除
	DeleteAllJumpItems();
	//
	SPI_InitLineImageInfo();//#450
	//折りたたみデータをTextviewに反映 #450
	ApplyDocPref_CollapsedLine();
	//行チェックデータを反映 #842
	ApplyDocPref_LineCheckedDate();
	//文法解析
	StartSyntaxRecognizer(0);
	*/
	
	//ドキュメント状態遷移。行折り返しから再計算する
	//（行折り返し計算状態に遷移し、スレッドで行計算を行う。）
	ReTransitInitState_Recalculate();
	
	//ウインドウ、および、PrintImpの描画プロパティ更新
	//#379 UpdateTextDrawProperty();
	GetApp().SPI_UpdateTextDrawProperty(GetObjectID());//#379
	
	/*#379
	//B0341
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateViewBounds();
	}
	*/
	GetApp().SPI_UpdateViewBounds(GetObjectID());//#379
	
	//#378
	SPI_UpdateJumpList();
	
	//ツールバー値更新
	GetApp().SPI_UpdateToolbarItemValue(GetObjectID());//#379
	
	//モードメニューのショートカット（現在のドキュメントのモード）設定
	GetApp().SPI_UpdateModeMenuShortcut();
}

//#683
/**
TextInfo内で持っているRegExpオブジェクトを更新する（RegExpの設定を変更時にコール必要）
*/
void	ADocument_Text::SPI_UpdateTextInfoRegExp()
{
	//purgeのみ行う。purgeすれば、次回、text info内で使用しようとするときに、updateされる。
	
	//text infoのreg expをpurgeする
	mTextInfo.PurgeRegExpObject();
	//文法認識スレッド内のtext infoのreg expをpurgeする
	GetSyntaxRecognizerThread().SPI_PurgeTextinfoRegExp();
}

/**
モード名（モードファイル名）取得
*/
void	ADocument_Text::SPI_GetModeRawName( AText& outText ) const
{
	//#914 GetDocPrefDBConst().GetData_Text(ADocPrefDB::kModeName,outText);
	AStMutexLocker	locker(mModeIndexMutex);
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeRawName(outText);
}

//#914
/**
モード表示名取得
*/
void	ADocument_Text::SPI_GetModeDisplayName( AText& outText ) const
{
	AStMutexLocker	locker(mModeIndexMutex);
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeDisplayName(outText);
}

ABool	ADocument_Text::NVIDO_IsSpecified() const
{
	if( SPI_IsRemoteFileMode() == true )
	{
		return true;
	}
	else
	{
		return true;//file.IsSpecified();
	}
}

/**
ドキュメントタイプがリモートファイルかどうかを取得
*/
ABool ADocument_Text::SPI_IsRemoteFileMode() const
{
	//#361 return mFTPMode;
	return (mTextDocumentType==kTextDocumentType_RemoteFile);//#361
}

ANumber	ADocument_Text::SPI_GetLineMargin() const
{
	ANumber	lineMargin = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetModeData_Number(AModePrefDB::kLineMargin);
	return lineMargin;
}

ABool	ADocument_Text::SPI_GetParentFolder( AFileAcc& outFolder ) const
{
	AFileAcc	file;
	if( NVI_GetFile(file) == true )
	{
		outFolder.SpecifyParent(file);
		return true;
	}
	else
	{
		return false;
	}
}

void	ADocument_Text::SPI_GetRemoteFileURL( AText& outRemoteFileURL ) const
{
	outRemoteFileURL.SetText(mRemoteFileURL);
}


void	ADocument_Text::NVIDO_SpecifyFile( const AFileAcc& inFileAcc )
{
	//FTPモードからSaveAsで通常ファイル保存した場合
	if( SPI_IsRemoteFileMode() == true )
	{
		//#361 mFTPMode = false;
		mTextDocumentType = kTextDocumentType_Normal;//#361
		//★
		//リモート接続オブジェクト解放？
	}
	
	mFileWriteLocker.Unlock();
	mFileWriteLocker.Specify(inFileAcc);
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kLockWhileEdit) == true && NVI_IsReadOnly() == false )
	{
		mFileWriteLocker.Lock();
	}
	
	//mCVSProperty.Specify(mFileAcc);//R0006
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).NVI_UpdateTitleBar();
	}
	*/
	GetApp().NVI_UpdateTitleBar();//#379
	
	//ProjectFolder設定
	GetApp().SPI_FindProjectFolder(inFileAcc,mProjectFolder,mProjectObjectID,mProjectRelativePath);//RC3
	//プロジェクトフォルダを登録
	GetApp().SPI_RegisterProjectFolder(mProjectFolder);
	//
	//#922 GetApp().SPI_UpdateWindowMenu();
	//ファイルリストウインドウ更新
	//#725 GetApp().SPI_UpdateFileListWindow_DocumentOpened(GetObjectID());
	GetApp().SPI_UpdateFileListWindows(kFileListUpdateType_DocumentOpened,GetObjectID());//#725
	
	//#81
	//自動バックアップファイルリスト更新
	SPI_UpdateAutoBackupArray();
}

//RC3
//ドキュメントオープン中にプロジェクト設定された場合にコールされる
void	ADocument_Text::SPI_UpdateProjectFolder()
{
	if( NVI_IsFileSpecified() == false )   return;
	if( SPI_IsRemoteFileMode() == true )   return;
	
	AFileAcc	file;
	NVI_GetFile(file);
	GetApp().SPI_FindProjectFolder(file,mProjectFolder,mProjectObjectID,mProjectRelativePath);
	//プロジェクトフォルダ登録
	GetApp().SPI_RegisterProjectFolder(mProjectFolder);
}

void	ADocument_Text::SPI_GetFileAttribute( AFileAttribute& outFileAttribute ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	outFileAttribute.creator = mCreator;
	outFileAttribute.type = mType;
#endif
}

void	ADocument_Text::SPI_CorrectTextPoint( ATextPoint& ioTextPoint ) const
{
	if( ioTextPoint.y >= SPI_GetLineCount() )
	{
		ioTextPoint.y = SPI_GetLineCount()-1;
	}
	if( ioTextPoint.x > SPI_GetLineLengthWithoutLineEnd(ioTextPoint.y) )
	{
		ioTextPoint.x = SPI_GetLineLengthWithoutLineEnd(ioTextPoint.y);
	}
	ATextIndex	textindex = SPI_GetTextIndexFromTextPoint(ioTextPoint);
	textindex = mText.GetCurrentCharPos(textindex);
	SPI_GetTextPointFromTextIndex(textindex,ioTextPoint);
}

//R0000
void	ADocument_Text::SPI_UpdateFileAttribute()
{
	//ReadMeファイルの場合は何もせずリターン（readonlyを維持するため） #1351
	if( mIsReadMeFile == true )   return;
	
	AFileAcc	file;
	if( NVI_GetFile(file) == true )
	{
		//ドキュメントが現在ReadOnlyであり、ファイルがReadOnlyで無くなった場合は、ドキュメントのReadOnlyを解除
		if( NVI_IsReadOnly() == true )
		{
			if( file.IsReadOnly() == false )
			{
				NVI_SetReadOnly(false);
			}
		}
	}
}

//win
//ドキュメントが新規ドキュメントかつ未編集であればtrueを返す
ABool	ADocument_Text::SPI_IsNewAndUneditted()
{
	if( NVI_IsFileSpecified() == true )   return false;
	if( SPI_IsRemoteFileMode() == true )   return false;
	if( NVI_IsDirty() == true )   return false;
	AText	text;
	if( mUndoer.GetUndoMenuText(text) == true )   return false;
	if( mUndoer.GetRedoMenuText(text) == true )   return false;
	return true;
}

//#850
/**
リオープン用のファイルパステキストを取得
*/
void	ADocument_Text::SPI_GetReopenFilePath( AText& outPath, ABool& outIsRemoteFile ) const
{
	//結果初期化
	outPath.DeleteAll();
	outIsRemoteFile = false;
	switch( mTextDocumentType )
	{
		//ローカルファイルの場合、ファイルパスを取得
	  case kTextDocumentType_Normal:
		{
			if( NVI_IsFileSpecified() == true )
			{
				NVI_GetFilePath(outPath);
			}
			break;
		}
		//リモートファイルの場合、リモートファイルのURLを取得
	  case kTextDocumentType_RemoteFile:
		{
			outPath.SetText(mRemoteFileURL);
			outIsRemoteFile = true;
			break;
		}
		//
	  case kTextDocumentType_DiffTarget:
		{
			//処理無し（返り値は空にする）
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#912
/**
インデント文字の推測
*/
void	ADocument_Text::SPI_GuessIndentCharacterType()
{
	//==================ヘッダー内文字列による推測==================
	//ヘッダー内文字列による推測フラグ （#957 排他制御内でSPI_UpdateIndentCharacterTypeModePref()を実行しないようにする）
	ABool	indentGuessedByHeader = false;
	{
		//排他制御
		AStMutexLocker	locker(mTextMutex);
		//"tab-width:"テキストがあればそれに従う
		AText	firstLineText;
		AIndex	pos = 0;
		mText.GetLine(pos,firstLineText);
		pos = 0;
		if( firstLineText.FindCstring(0,"tab-width:",pos) == true )
		{
			pos += 10;
			mText.SkipTabSpace(pos,firstLineText.GetItemCount());
			//
			ANumber	tabwidth = 0;
			mText.ParseIntegerNumber(pos,tabwidth);
			switch(tabwidth)
			{
			  case 2:
				{
					mGuessedIndnetCharacterType = kIndentCharacterType_2Spaces;
					indentGuessedByHeader = true;
					break;
				}
			  case 4:
				{
					mGuessedIndnetCharacterType = kIndentCharacterType_4Spaces;
					indentGuessedByHeader = true;
					break;
				}
			  case 8:
				{
					mGuessedIndnetCharacterType = kIndentCharacterType_8Spaces;
					indentGuessedByHeader = true;
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
	//ヘッダー内文字列による推測フラグONのときは、設定を更新して終了 #957
	if( indentGuessedByHeader == true )
	{
		SPI_UpdateIndentCharacterTypeModePref();
		return;
	}
	
	//==================各行のタブ／スペース数から推測==================
	{
		//排他制御（#957 排他制御内でSPI_UpdateIndentCharacterTypeModePref()を実行しないようにする）
		AStMutexLocker	locker(mTextMutex);
		//ポインタ取得
		AStTextPtr	textptr(mText,0,mText.GetItemCount());
		AUChar*	p = textptr.GetPtr();
		//各タイプごとのカウント数初期化
		AItemCount	space2Count = 0;
		AItemCount	space4Count = 0;
		AItemCount	space6Count = 0;
		AItemCount	space8Count = 0;
		AItemCount	tabCount = 0;
		//行ごとのループ（ただし、最大は1000行まで）
		AItemCount  lineCount = SPI_GetLineCount();
		if( lineCount > 1000 )   lineCount = 1000;
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			//段落開始行かどうかを取得
			ABool	isParagraphStart = true;
			if( lineIndex > 0 )
			{
				isParagraphStart = mTextInfo.GetLineExistLineEnd(lineIndex);
			}
			//段落開始行の場合、かつ、行頭がコード内の場合のみ、処理する
			if( isParagraphStart == true && mTextInfo.GetLineStartIsCode(lineIndex) == true )//#1003
			{
				//行頭がタブか、スペースか（スペースの場合はスペース数も）を判定
				AIndex	lineStart = mTextInfo.GetLineStart(lineIndex);
				AItemCount	len = mTextInfo.GetLineLengthWithoutLineEnd(lineIndex);
				AItemCount	spaceCount = 0;
				ABool	tab = false;
				for( AIndex pos = 0; pos < len; pos++ )
				{
					AUChar	ch = p[lineStart+pos];
					if( ch == kUChar_Tab )
					{
						tab = true;
						break;
					}
					else if( ch == kUChar_Space )
					{
						spaceCount++;
					}
					else
					{
						break;
					}
				}
				//各タイプのカウンタをインクリメント
				if( tab == true )
				{
					tabCount++;
				}
				switch( spaceCount )
				{
				  case 2:
					{
						space2Count++;
						break;
					}
				  case 4:
					{
						space4Count++;
						break;
					}
				  case 6:
					{
						space6Count++;
						break;
					}
				  case 8:
					{
						space8Count++;
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
		//------------------各タイプのカウンタ数に従って、判定------------------
		//全体の1%を閾値として、タブ、スペース2文字、4文字、8文字、判定不可の順に、最初に閾値を超えるものを選択する
		AItemCount	threashold = 0;//#1003 tabCount+space2Count+space4Count+space6Count+space8Count;
		//#1003 threashold /= 100;
		//#1003 threashold += 10;//#1003 
		//#1003 結局しきい値は0（より大きい）がベター。 Pythonなどでクラス内の def行が1行しかない場合などは、最小インデントが1行しかないこともあり得る。
		//
		if( tabCount > threashold )
		{
			mGuessedIndnetCharacterType = kIndentCharacterType_Tab;
		}
		else if( space2Count > threashold )
		{
			mGuessedIndnetCharacterType = kIndentCharacterType_2Spaces;
		}
		else if( space4Count > threashold )
		{
			mGuessedIndnetCharacterType = kIndentCharacterType_4Spaces;
		}
		else if( space8Count > threashold )
		{
			mGuessedIndnetCharacterType = kIndentCharacterType_8Spaces;
		}
		else
		{
			mGuessedIndnetCharacterType = kIndentCharacterType_Unknown;
		}
	}
	//------------------モード設定を更新------------------
	SPI_UpdateIndentCharacterTypeModePref();
}

/**
自動インデント文字設定
ドキュメントで推測されているインデント文字タイプをモード設定に設定する
*/
ABool	ADocument_Text::SPI_UpdateIndentCharacterTypeModePref()
{
	//自動インデント文字設定がOFFなら何もしないで終了
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kIndentAutoDetect) == false )
	{
		return false;
	}
	//自動インデント文字が不明なら何もしないで終了
	if( mGuessedIndnetCharacterType == kIndentCharacterType_Unknown )
	{
		return false;
	}
	
	//推測タイプに従って、インデント文字とインデント幅を取得
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex());
	ANumber	indentWidth = SPI_GetTabWidth();
	ABool	spaceIndent = false;
	switch(mGuessedIndnetCharacterType)
	{
	  case kIndentCharacterType_Tab:
		{
			break;
		}
	  case kIndentCharacterType_2Spaces:
		{
			indentWidth = 2;
			spaceIndent = true;
			break;
		}
	  case kIndentCharacterType_4Spaces:
		{
			indentWidth = 4;
			spaceIndent = true;
			break;
		}
	  case kIndentCharacterType_8Spaces:
		{
			indentWidth = 8;
			spaceIndent = true;
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//データ設定
	ABool	changed = false;
	if( modePrefDB.GetData_Number(AModePrefDB::kIndentWidth) != indentWidth )
	{
		modePrefDB.SetData_Number(AModePrefDB::kIndentWidth,indentWidth);
		changed = true;
	}
	if( modePrefDB.GetData_Bool(AModePrefDB::kInputSpacesByTabKey) != spaceIndent )
	{
		modePrefDB.SetData_Bool(AModePrefDB::kInputSpacesByTabKey,spaceIndent);
		changed = true;
	}
	//データに変更があった場合は、モード設定ウインドウに反映し、notificationを表示する
	if( changed == true )
	{
		//モード設定ウインドウ表示中なら、モード設定ウインドウに表示反映
		if( GetApp().SPI_GetModePrefWindow(SPI_GetModeIndex()).NVI_IsWindowVisible() == true )
		{
			GetApp().SPI_GetModePrefWindow(SPI_GetModeIndex()).NVI_UpdateProperty();
		}
		//モード画面を開くURLスキーム作成 #1003
		AText	modeName;
		SPI_GetModeRawName(modeName);
		AText	modeprefURI;
		modeprefURI.SetCstring("net.mimikaki.mi://modepref/");
		modeprefURI.AddText(modeName);
		modeprefURI.AddCstring("/6");
		//notificationウインドウをポップアップ表示
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetPopupNotificationWindow().
				SPI_GetNotificationView().SPI_SetNotification_IndentCharacterChanged(SPI_GetModeIndex(),modeprefURI);
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_ShowNotificationPopupWindow(true);
	}
	return changed;
}

//#558
/**
縦書きモードかどうかを取得
*/
ABool	ADocument_Text::SPI_GetVerticalMode() const
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
	{
		return GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kVerticalMode);
	}
	else
	{
		//10.6以下は縦書き未対応
		return false;
	}
}

//#858
#if 0
#pragma mark ===========================

#pragma mark ---外部コメント（付箋紙）

//#138
/**
外部コメントデータ（付箋紙データ）配列を構成
*/
void	ADocument_Text::SPI_UpdateExternalCommentArray()
{
	//まず全削除
	mExternalCommentArray_LineIndex.DeleteAll();
	mExternalCommentArray_Comment.DeleteAll();
	mExternalCommentArray_ModuleName.DeleteAll();
	mExternalCommentArray_Offset.DeleteAll();
	
	//プロジェクト未設定なら何もせず終了
	if( mProjectObjectID == kObjectID_Invalid )   return;
	
	//Projectから、このドキュメントの全データを取得する
	ATextArray	moduleNameArray, commentArray;
	ANumberArray	offsetArray;
	GetApp().SPI_GetExternalCommentArrayFromProjectDB(mProjectObjectID,mProjectRelativePath,moduleNameArray,offsetArray,commentArray);
	//取得したデータごとに行番号・コメントのレコードに変換して、配列を構成する
	for( AIndex i = 0; i < moduleNameArray.GetItemCount(); i++ )
	{
		//モジュール名・オフセットから行番号検索
		AIndex	lineIndex = FindLineIndexByModuleNameAndOffset(moduleNameArray.GetTextConst(i),offsetArray.Get(i));
		//行番号補正
		if( lineIndex < 0 )   lineIndex = 0;
		if( lineIndex >= SPI_GetLineCount() )   lineIndex = SPI_GetLineCount()-1;
		//表示行とコメントを配列に追加
		mExternalCommentArray_LineIndex.Add(lineIndex);
		mExternalCommentArray_Comment.Add(commentArray.GetTextConst(i));
		mExternalCommentArray_ModuleName.Add(moduleNameArray.GetTextConst(i));
		mExternalCommentArray_Offset.Add(offsetArray.Get(i));
	}
	
	//TextViewへ反映
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateFusen(true);
	}
	
	//付箋紙リストウインドウ生成済みならテーブル更新
	if( GetApp().SPI_GetFusenListWindow().NVI_IsWindowCreated() == true )
	{
		//Viewに対してリスト更新を指示
		GetApp().SPI_GetFusenListWindow().SPI_GetListView().SPI_UpdateTable(mProjectObjectID,mProjectRelativePath);
	}
}

//#138
/**
モジュール名・オフセットから行番号取得

@param inModuleName モジュール名
@param inOffset オフセット
@return 行番号
*/
AIndex	ADocument_Text::FindLineIndexByModuleNameAndOffset( const AText& inModuleName, const ANumber inOffset ) const
{
	//inModuleNameに一致するメニューテキストを検索し、その行番号を得る
	AIndex	lineIndex = mTextInfo.FindLineIndexByMenuText(inModuleName);
	if( lineIndex == kIndex_Invalid )
	{
		//メニューテキストで一致しない場合は、キーワードで検索する
		lineIndex = mTextInfo.FindLineIndexByMenuKeywordText(inModuleName);
		if( lineIndex == kIndex_Invalid )
		{
			//見つからなければ最初の行にしておく
			lineIndex = 0;
		}
	}
	//行番号補正
	AIndex	result = lineIndex+inOffset;
	if( result < 0 )   result = 0;
	if( result >= SPI_GetLineCount() )   result = SPI_GetLineCount()-1;
	return result;
}

//#138
/**
行番号に対応する外部コメントのIndexを取得

@param inLineIndex 行番号
@return コメントIndex
*/
AIndex	ADocument_Text::SPI_FindExternalCommentIndexByLineIndex( const AIndex inLineIndex ) const
{
	return mExternalCommentArray_LineIndex.Find(inLineIndex);
}

//#138
/**
外部コメントText取得

@param inCommentIndex コメントIndex
@return コメントText
*/
const AText	ADocument_Text::SPI_GetExternalCommentText( const AIndex inCommentIndex ) const
{
	return mExternalCommentArray_Comment.GetTextConst(inCommentIndex);
}

//#138
/**
外部コメントlineIndex取得

@param inCommentIndex コメントIndex
@return 行番号
*/
AIndex	ADocument_Text::SPI_GetExternalCommentLineIndex( const AIndex inCommentIndex ) const
{
	return mExternalCommentArray_LineIndex.Get(inCommentIndex);
}

//#138
/**
外部コメントmoduleName取得

@param inCommentIndex コメントIndex
@return モジュール名
*/
const AText	ADocument_Text::SPI_GetExternalCommentModuleName( const AIndex inCommentIndex ) const
{
	return mExternalCommentArray_ModuleName.GetTextConst(inCommentIndex);
}

//#138
/**
外部コメントlineIndex取得

@param inCommentIndex コメントIndex
@return オフセット
*/
AIndex	ADocument_Text::SPI_GetExternalCommentOffset( const AIndex inCommentIndex ) const
{
	return mExternalCommentArray_Offset.Get(inCommentIndex);
}

//#138
/**
テキストの追加・削除時に、外部コメント配列の行番号をAdjustする

@param inStarLineIndex 更新開始行番号
@param inDelta 追加・削除行数（削除はマイナス）
*/
void	ADocument_Text::AdjustExternalCommentArray( const AIndex inStartLineIndex, const AIndex inDelta )
{
	//配列内データをそれぞれ調べて、inStartLineIndex以降の行のものは、inDeltaだけずらす
	AItemCount	itemCount = mExternalCommentArray_LineIndex.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AIndex	lineIndex = mExternalCommentArray_LineIndex.Get(i);
		if( lineIndex >= inStartLineIndex )
		{
			mExternalCommentArray_LineIndex.Set(i,lineIndex+inDelta);
		}
	}
}

//外部コメント取得 RC3
/*#138
void	ADocument_Text::SPI_GetExternalComment( const AText& inModuleName, const AIndex inOffset, AText& outComment ) const
{
	if( mProjectObjectID != kObjectID_Invalid )
	{
		GetApp().SPI_GetExternalCommentFromProjectDB(mProjectObjectID,mProjectRelativePath,inModuleName,inOffset,outComment);
	}
}
*/

//RC3 #138
/**
行に対応するモジュール名・オフセットを取得

対応するモジュール名が存在しない場合は、outTextは空、outOffsetは行番号となる。
@param inLineIndex 行番号(in)
@param outText モジュール名(out)
@param outOffset オフセット(out)
*/
void	ADocument_Text::SPI_GetModuleNameAndOffsetByLineIndex( const AIndex inLineIndex, AText& outText, AIndex& outOffset ) const
{
	//★要テスト
	
	//モジュール名・オフセット初期化
	outText.DeleteAll();
	outOffset = kIndex_Invalid;
	//行番号からメニューテキスト取得
	AIndex	identifierLineIndex = 0;
	//#695 AUniqID	UniqID = mTextInfo.GetMenuIdentifierByLineIndex(inLineIndex,identifierLineIndex);
	//#695 if( UniqID != kUniqID_Invalid )//#216
	AIndex	jumpIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);//#695
	if( jumpIndex != kIndex_Invalid )//#695
	{
		AUniqID	UniqID = mJump_IdentifierObjectArray.Get(jumpIndex);//#695
		identifierLineIndex = mTextInfo.GetLineIndexByGlobalIdentifier(UniqID);//#695
		outOffset = inLineIndex - identifierLineIndex;
		mTextInfo.GetGlobalIdentifierMenuText(UniqID,outText);
	}
	else
	{
		outOffset = inLineIndex;
	}
}

/**
モジュール名・オフセットに対応する行の最初を選択する
*/
void	ADocument_Text::SPI_SelectTextByModuleNameAndOffset( const AText& inModuleName, const ANumber inOffset )
{
	//モジュール名・オフセットから行番号を検索
	AIndex	lineIndex = FindLineIndexByModuleNameAndOffset(inModuleName,inOffset);
	//#199 if( mWindowIDArray.GetItemCount() == 0 )   return;
	if( mViewIDArray.GetItemCount() == 0 )   return;
	//行番号補正
	if( lineIndex < 0 )   lineIndex = 0;
	if( lineIndex > SPI_GetLineCount() )   lineIndex = SPI_GetLineCount();
	//テキストを選択
	//#199 GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_SelectText(GetObjectID(),SPI_GetLineStart(lineIndex),0);
	SPI_SelectText(SPI_GetLineStart(lineIndex),0);
}

/**
外部コメント配列データ取得
*/
void	ADocument_Text::SPI_GetExternalCommentArray( ATextArray& outModuleNameArray, ANumberArray& outOffsetArray, ATextArray& outCommentArray, ANumberArray& outLineIndexArray ) const
{
	//初期化
	outModuleNameArray.DeleteAll();
	outOffsetArray.DeleteAll();
	outCommentArray.DeleteAll();
	outLineIndexArray.DeleteAll();
	//格納
	for( AIndex i = 0; i < mExternalCommentArray_ModuleName.GetItemCount(); i++ )
	{
		outModuleNameArray.Add(mExternalCommentArray_ModuleName.GetTextConst(i));
		outOffsetArray.Add(mExternalCommentArray_Offset.Get(i));
		outCommentArray.Add(mExternalCommentArray_Comment.GetTextConst(i));
		outLineIndexArray.Add(mExternalCommentArray_LineIndex.Get(i));
	}
}
#endif

#pragma mark ===========================

#pragma mark ---検索

//検索
ABool	ADocument_Text::SPI_Find( const AFindParameter& inParam, const ABool inBackward,//#789
								 const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos, 
								 const ABool inUseModalSession, ABool& outModalSessionAborted )
{
	ABool	abortflag = false;//B0359
	AFindParameter	param;
	param.Set(inParam);
	//JIS系テキストエンコーディングの場合は、常に￥／バックスラッシュ無視にする。 #940
	if( SPI_IsJISTextEncoding() == true )
	{
		param.ignoreBackslashYen = true;
	}
	//検索実行
	return mTextFinder.ExecuteFind(mText,param,inBackward,inSpos,inEpos,outSpos,outEpos,abortflag,inUseModalSession,outModalSessionAborted);
}

void	ADocument_Text::SPI_ChangeReplaceText( const AText& inReplaceText, AText& outText ) const
{
	mTextFinder.ChangeReplaceText(mText,inReplaceText,outText);
}

ABool	ADocument_Text::SPI_FindForHighlight( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos ) 
{
	//ハイライト長制限
	AIndex	epos = inEpos;
	if( inEpos-inSpos > kLimit_HighlightTargetTextLength )
	{
		epos = SPI_GetCurrentCharTextIndex(inSpos+kLimit_HighlightTargetTextLength);
	}
	//
	if( mFindHighlightSet == false )   return false;
	ABool	abortflag = false;//B0359
	ABool	modalSessionAborted = false;//#695
	return mTextFinderForHighlight.ExecuteFind(mText,mFindHighlightParam,false,inSpos,epos,outSpos,outEpos,abortflag,false,modalSessionAborted);
}

//R0244
ABool	ADocument_Text::SPI_FindForHighlight2( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos ) 
{
	//ハイライト長制限
	AIndex	epos = inEpos;
	if( inEpos-inSpos > kLimit_HighlightTargetTextLength )
	{
		epos = SPI_GetCurrentCharTextIndex(inSpos+kLimit_HighlightTargetTextLength);
	}
	//
	if( mFindHighlightSet2 == false )   return false;
	ABool	abortflag = false;//B0359
	ABool	modalSessionAborted = false;//#695
	return mTextFinderForHighlight2.ExecuteFind(mText,mFindHighlightParam2,false,inSpos,epos,outSpos,outEpos,abortflag,false,modalSessionAborted);
}

void	ADocument_Text::SPI_SetFindHighlight( const AFindParameter& inParam )
{
	if( mFindHighlightSet == true )
	{
		if( 	inParam.findText.Compare(mFindHighlightParam.findText) == true &&
				inParam.ignoreCase == mFindHighlightParam.ignoreCase &&
				inParam.wholeWord == mFindHighlightParam.wholeWord &&
				inParam.fuzzy == mFindHighlightParam.fuzzy &&
				inParam.regExp == mFindHighlightParam.regExp &&
				inParam.ignoreBackslashYen == mFindHighlightParam.ignoreBackslashYen &&
				inParam.ignoreSpaces == mFindHighlightParam.ignoreSpaces )//#165
		{
			return;
		}
		//R0244
		mFindHighlightParam2.Set(mFindHighlightParam);
		mFindHighlightSet2 = true;
	}
	mFindHighlightParam.Set(inParam);
	mFindHighlightSet = true;
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFindHighlight) == true )
	{
		//TextView表示更新
		//win 080727 RefreshWindow();
		SPI_RefreshTextViews();//win 080727
	}
}

ABool	ADocument_Text::SPI_IsFindHighlightSet() const
{
	return mFindHighlightSet;
}

void	ADocument_Text::SPI_ClearFindHighlight()
{
	mFindHighlightSet = false;
	mFindHighlightSet2 = false;//R0244
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFindHighlight) == true )
	{
		//TextView表示更新
		//win 080727 RefreshWindow();
		SPI_RefreshTextViews();//win 080727
	}
}

//#65
/**
@note abort/error時は返り値がfalse
*/
ABool	ADocument_Text::SPI_ReplaceText( const AFindParameter& inParam, 
													const ATextIndex inStart, const ATextIndex inEnd, 
		AItemCount& outReplacedCount, AItemCount& outOffset, AIndex& ioSelectSpos, AIndex& ioSelectEpos )//#245
{
	if( inStart > inEnd )
	{
		_ACError("",NULL);
		return false;
	}
	
	//結果初期化
	outOffset = 0;//#245
	outReplacedCount = 0;
	
	ATextIndex	startPos = inStart;
	ATextIndex	endPos = inEnd;
	
	//Undoアクションタグ記録
	SPI_RecordUndoActionTag(undoTag_Replace);
	
	//#846
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_Replace,true,false,true);
	ABool	result = true;
	try
	{
		
		//１つ目の検索
		ATextIndex	replaceStart1, replaceEnd1;
		ABool	modalSessionAborted = false;
		if( SPI_Find(inParam,false,startPos,endPos,replaceStart1,replaceEnd1,true,modalSessionAborted) == false )
		{
			//modalSessionAbortedフラグONなら、返り値はtrue
			result = (modalSessionAborted==false);
		}
		else
		{
			//置換文字列作成
			AText	replaceText1;
			if( inParam.regExp == true )
			{
				SPI_ChangeReplaceText(inParam.replaceText,replaceText1);
			}
			else
			{
				replaceText1.SetText(inParam.replaceText);
			}
			//次の検索開始位置
			if( replaceStart1 == replaceEnd1 )
			{
				if( replaceEnd1 >= endPos )
				{
					startPos = endPos+1;//下までいったら終わらせる
				}
				else
				{
					startPos = SPI_GetNextCharTextIndex(replaceEnd1);
				}
			}
			else
			{
				startPos = replaceEnd1;
			}
			
			//２つ目以降の検索
			AText	replaceText2;
			while(true) 
			{
				//モーダルセッション継続判定
				if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Replace,outReplacedCount,true,startPos-inStart,inEnd-inStart) == false )
				{
					result = false;
					break;
				}
				
				//次の検索
				ABool	found = false;
				ATextIndex	replaceStart2, replaceEnd2;
				if( startPos <= endPos )
				{
					found = SPI_Find(inParam,false,startPos,endPos,replaceStart2,replaceEnd2,true,modalSessionAborted);
					//modalSessionAbortedフラグONなら、返り値はtrue
					if( modalSessionAborted == true )
					{
						result = false;
						break;
					}
					//次の検索開始位置
					if( replaceStart2 == replaceEnd2 )
					{
						if( replaceEnd2 >= endPos )
						{
							startPos = endPos+1;//下までいったら終わらせる
						}
						else
						{
							startPos = SPI_GetNextCharTextIndex(replaceEnd2);
						}
					}
					else
					{
						startPos = replaceEnd2;
					}
				}
				else
				{
					replaceStart2 = replaceEnd2 = startPos;
				}
				
				//B0182 まずreplaceText2に次の置換文字列を作成して保存する
				//次の置換文字列の作成
				replaceText2.DeleteAll();
				if( found == true )
				{
					if( inParam.regExp == true )
					{
						SPI_ChangeReplaceText(inParam.replaceText,replaceText2);
					}
					else
					{
						replaceText2.SetText(inParam.replaceText);
					}
				}
				
				//１つ前に検索した部分の置換
				//DeleteTextFromDocument(replaceStart1,replaceEnd1,true);//dont redraw
				SPI_DeleteText(replaceStart1,replaceEnd1,true,true);
				if( replaceText1.GetItemCount() > 0 )
				{
					//InsertTextToDocument(replaceStart1,replaceText1,true);//#846
					SPI_InsertText(replaceStart1,replaceText1,true,true);
				}
				outReplacedCount++;
				//置換による検索範囲の移動
				AItemCount	offset = replaceText1.GetItemCount() - (replaceEnd1-replaceStart1);
				startPos += offset;
				endPos += offset;
				outOffset += offset;//#245
				
				if( replaceStart1 < ioSelectSpos ) 	ioSelectSpos += offset;
				if( replaceEnd1 <= ioSelectEpos )  	ioSelectEpos += offset;
				
				//replaceStart1/replaceEnd1の更新
				replaceStart1 	= replaceStart2 + offset;
				replaceEnd1 	= replaceEnd2 + offset;
				
				if( found == false )   break;
				
				//次の置換文字列をreplaceText1にコピー
				replaceText1.SetText(replaceText2);
			}
			
			//SPI_DeleteText(), SPI_InsertText()でdont redraw指定しているので、ここで描画を行う
			SPI_RefreshTextViews();
			UpdateDocInfoWindows();
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	//
	return result;
}

#pragma mark ===========================

#pragma mark ---テキストマーカー
//#750

//#750
/**
テキストマーカー文字列を検索する
*/
ABool	ADocument_Text::SPI_FindForTextMarker( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos ) 
{
	//ハイライト長制限
	AIndex	epos = inEpos;
	if( inEpos-inSpos > kLimit_HighlightTargetTextLength )
	{
		epos = SPI_GetCurrentCharTextIndex(inSpos+kLimit_HighlightTargetTextLength);
	}
	//
	//検索パラメータ設定
	AFindParameter	param;
	param.Set(mTextMarkerParam);
	//JIS系テキストエンコーディングの場合は、常に￥／バックスラッシュ無視にする。 #940
	if( SPI_IsJISTextEncoding() == true )
	{
		param.ignoreBackslashYen = true;
	}
	//最小の一致位置を記憶するための変数
	ABool	matched = false;
	AIndex	firstMatchedSpos = epos;
	AIndex	firstMatchedEpos = epos;
	//テキストマーカー各文字列毎のループ
	for( AIndex index = 0; index < mTextMarkerArray_TextFinder.GetItemCount(); index++ )
	{
		//テキストマーカー文字列を取得し、検索パラメータに設定
		mTextMarkerArray_FindText.Get(index,param.findText);
		//検索文字長制限
		param.findText.LimitLength(0,kLimit_FindTextLength);
		//検索実行
		ABool	abortflag = false;
		ABool	modalSessionAborted = false;//#695
		AIndex	spos = kIndex_Invalid, epos = kIndex_Invalid;
		if( mTextMarkerArray_TextFinder.GetObject(index).
		   ExecuteFind(mText,param,false,inSpos,inEpos,spos,epos,abortflag,false,modalSessionAborted) == true )
		{
			//最小の一致位置を記憶
			if( spos < firstMatchedSpos )
			{
				firstMatchedSpos = spos;
				firstMatchedEpos = epos;
			}
			//一致フラグON
			matched = true;
		}
	}
	//最小の一致位置を結果に返す
	outSpos = firstMatchedSpos;
	outEpos = firstMatchedEpos;
	return matched;
}

//#750
/**
テキストマーカーを適用する
*/
void	ADocument_Text::SPI_ApplyTextMarker()
{
	//初期状態の場合は何もしない
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		return;
	}
	
	//現在のテキストマーカーのデータを取得
	AText	title, text;
	GetApp().SPI_GetCurrentTextMarkerData(title,text);
	
	//今設定されているものと同じなら何もせずリターン
	if( mTextMarkerText.Compare(text) == true )
	{
		return;
	}
	mTextMarkerText.SetText(text);
	
	//検索パラメータ設定
	mTextMarkerParam.ignoreCase 		= true;
	mTextMarkerParam.wholeWord 			= false;
	mTextMarkerParam.fuzzy				= false;
	mTextMarkerParam.regExp 			= false;
	mTextMarkerParam.loop 				= false;
	mTextMarkerParam.ignoreBackslashYen = true;
	mTextMarkerParam.ignoreSpaces		= true;
	
	//データ初期化
	mTextMarkerArray_TextFinder.DeleteAll();
	mTextMarkerArray_FindText.DeleteAll();
	
	//指定されたテキストをexplodeして、各text毎のループ
	ATextArray	textArray;
	textArray.ExplodeFromText(text,kUChar_LineEnd);
	for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
	{
		//テキスト取得
		AText	findText;
		textArray.Get(i,findText);
		if( findText.GetItemCount() == 0 )   continue;
		
		//各text毎に、TextFinder生成
		mTextMarkerArray_TextFinder.AddNewObject();
		//text設定
		mTextMarkerArray_FindText.Add(findText);
	}
	
	//描画更新
	SPI_RefreshTextViews();
}

#pragma mark ===========================

#pragma mark ---現在単語マーカー
//#823

/**
@return 画面refreshが必要かどうか
*/
ABool	ADocument_Text::SPI_SetCurrentWordForHighlight( const AText& inWord, const ABool inWholeWord, const ABool inRedraw )
{
	//指定テキストの改行コードまでを検索文字列とする。
	AText	firstlineText;
	AIndex	p = 0;
	inWord.GetLine(p,firstlineText);
	//最初の改行コードまでに文字が無ければ、ハイライト消去
	if( firstlineText.GetItemCount() == 0 )
	{
		return SPI_ClearCurrentWordForHighlight(true);
	}
	
	//現在の設定データと同じならリターン
	if( mCurrentWordFindHighlightSet == true && mCurrentWordFindHighlightParam.findText.Compare(inWord) == true )
	{
		return false;
	}
	
	//再描画が必要な行indexを記憶
	AObjectArray< AArray<ANumber> >	redrawLineIndexArray;
	if( inRedraw == true )
	{
		//各view毎のループ
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			//view毎にnumber arrayオブジェクト生成
			redrawLineIndexArray.AddNewObject();
			//viewの現在の表示行範囲を取得
			AIndex	startLineIndex = 0, endLineIndex = 0;
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetDrawLineRange(startLineIndex,endLineIndex);
			if( endLineIndex >= SPI_GetLineCount() )   endLineIndex = SPI_GetLineCount()-1;
			//各行ごとのループ
			for( AIndex lineIndex = startLineIndex; lineIndex <= endLineIndex; lineIndex++ )
			{
				//行を含む段落内を検索して、対象テキストが見つかったら、行indexを追加
				//段落全体を検索すると時間がかかるので前の行から次の行までを検索
				AIndex	findStart = 0, findEnd = 0;
				AIndex	paragraphStartLineIndex = SPI_GetCurrentParagraphStartLineIndex(lineIndex);
				if( paragraphStartLineIndex < lineIndex && lineIndex-1 >= 0 )
				{
					findStart = SPI_GetLineStart(lineIndex-1);
				}
				else
				{
					findStart = SPI_GetLineStart(lineIndex);
				}
				AIndex	paragraphEndLineIndex = SPI_GetNextParagraphStartLineIndex(lineIndex);
				if( paragraphEndLineIndex > lineIndex+1 && lineIndex+2 < SPI_GetLineCount() )
				{
					findEnd = SPI_GetLineStart(lineIndex+2);
				}
				else
				{
					findEnd = findStart + SPI_GetLineLengthWithLineEnd(lineIndex);
				}
				//
				ATextIndex	spos = 0, epos = 0;
				if( SPI_FindForCurrentWordHighlight(findStart,findEnd,spos,epos) == true )
				{
					redrawLineIndexArray.GetObject(i).Add(lineIndex);
				}
			}
		}
	}
	
	//検索パラメータ設定
	mCurrentWordFindHighlightParam.findText.SetText(firstlineText);
	mCurrentWordFindHighlightParam.ignoreCase 			= false;
	mCurrentWordFindHighlightParam.wholeWord 			= inWholeWord;
	mCurrentWordFindHighlightParam.fuzzy				= false;
	mCurrentWordFindHighlightParam.regExp 				= false;
	mCurrentWordFindHighlightParam.loop 				= false;
	mCurrentWordFindHighlightParam.ignoreBackslashYen 	= false;
	mCurrentWordFindHighlightParam.ignoreSpaces			= false;
	//検索文字長制限
	mCurrentWordFindHighlightParam.findText.LimitLength(0,kLimit_FindTextLength);
	//フラグON
	mCurrentWordFindHighlightSet = true;
	
	//描画更新
	if( inRedraw == true )
	{
		//各view毎のループ
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			//viewの現在の表示行範囲を取得
			AIndex	startLineIndex = 0, endLineIndex = 0;
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetDrawLineRange(startLineIndex,endLineIndex);
			if( endLineIndex >= SPI_GetLineCount() )   endLineIndex = SPI_GetLineCount()-1;
			//各行ごとのループ
			for( AIndex lineIndex = startLineIndex; lineIndex <= endLineIndex; lineIndex++ )
			{
				//行を含む段落内を検索して、対象テキストが見つかったら、行indexを追加
				//段落全体を検索すると時間がかかるので前の行から次の行までを検索
				AIndex	findStart = 0, findEnd = 0;
				AIndex	paragraphStartLineIndex = SPI_GetCurrentParagraphStartLineIndex(lineIndex);
				if( paragraphStartLineIndex < lineIndex && lineIndex-1 >= 0 )
				{
					findStart = SPI_GetLineStart(lineIndex-1);
				}
				else
				{
					findStart = SPI_GetLineStart(lineIndex);
				}
				AIndex	paragraphEndLineIndex = SPI_GetNextParagraphStartLineIndex(lineIndex);
				if( paragraphEndLineIndex > lineIndex+1 && lineIndex+2 < SPI_GetLineCount() )
				{
					findEnd = SPI_GetLineStart(lineIndex+2);
				}
				else
				{
					findEnd = SPI_GetLineStart(lineIndex) + SPI_GetLineLengthWithLineEnd(lineIndex);
				}
				//
				ATextIndex	spos = 0, epos = 0;
				if( SPI_FindForCurrentWordHighlight(findStart,findEnd,spos,epos) == true )
				{
					if( redrawLineIndexArray.GetObjectConst(i).Find(lineIndex) == kIndex_Invalid )
					{
						redrawLineIndexArray.GetObject(i).Add(lineIndex);
					}
				}
			}
			
			//描画更新実行
			for( AIndex j = 0; j < redrawLineIndexArray.GetObjectConst(i).GetItemCount(); j++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshLine(redrawLineIndexArray.GetObjectConst(i).Get(j));
			}
		}
	}
	return true;
}

/**
現在単語ハイライトクリア
*/
ABool	ADocument_Text::SPI_ClearCurrentWordForHighlight( const ABool inRedraw )
{
	//現在ハイライト設定無しなら何もせずリターン
	if( mCurrentWordFindHighlightSet == false )
	{
		return false;
	}
	
	//再描画が必要な行indexを記憶
	AObjectArray< AArray<ANumber> >	redrawLineIndexArray;
	if( inRedraw == true )
	{
		//各view毎のループ
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			//view毎にnumber arrayオブジェクト生成
			redrawLineIndexArray.AddNewObject();
			//viewの現在の表示行範囲を取得
			AIndex	startLineIndex = 0, endLineIndex = 0;
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetDrawLineRange(startLineIndex,endLineIndex);
			if( endLineIndex >= SPI_GetLineCount() )   endLineIndex = SPI_GetLineCount()-1;
			//各行ごとのループ
			for( AIndex lineIndex = startLineIndex; lineIndex <= endLineIndex; lineIndex++ )
			{
				//行を含む段落内を検索して、対象テキストが見つかったら、行indexを追加
				//段落全体を検索すると時間がかかるので前の行から次の行までを検索
				AIndex	findStart = 0, findEnd = 0;
				AIndex	paragraphStartLineIndex = SPI_GetCurrentParagraphStartLineIndex(lineIndex);
				if( paragraphStartLineIndex < lineIndex && lineIndex-1 >= 0 )
				{
					findStart = SPI_GetLineStart(lineIndex-1);
				}
				else
				{
					findStart = SPI_GetLineStart(lineIndex);
				}
				AIndex	paragraphEndLineIndex = SPI_GetNextParagraphStartLineIndex(lineIndex);
				if( paragraphEndLineIndex > lineIndex+1 && lineIndex+2 < SPI_GetLineCount() )
				{
					findEnd = SPI_GetLineStart(lineIndex+2);
				}
				else
				{
					findEnd = findStart + SPI_GetLineLengthWithLineEnd(lineIndex);
				}
				//
				ATextIndex	spos = 0, epos = 0;
				if( SPI_FindForCurrentWordHighlight(findStart,findEnd,spos,epos) == true )
				{
					redrawLineIndexArray.GetObject(i).Add(lineIndex);
				}
			}
		}
	}
	
	//検索文字列クリア
	mCurrentWordFindHighlightParam.findText.DeleteAll();
	mCurrentWordFindHighlightSet = false;
	
	if( inRedraw == true )
	{
		//各view毎のループ
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			//描画更新実行
			for( AIndex j = 0; j < redrawLineIndexArray.GetObjectConst(i).GetItemCount(); j++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshLine(redrawLineIndexArray.GetObjectConst(i).Get(j));
			}
		}
	}
	return true;
}

/**
現在単語ハイライト用検索
*/
ABool	ADocument_Text::SPI_FindForCurrentWordHighlight( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos )
{
	if( mCurrentWordFindHighlightSet == false )   return false;
	//ハイライト長さ制限
	AIndex	epos = inEpos;
	if( inEpos-inSpos > kLimit_HighlightTargetTextLength )
	{
		epos = SPI_GetCurrentCharTextIndex(inSpos+kLimit_HighlightTargetTextLength);
	}
	//
	ABool	abortflag = false;
	ABool	modalSessionAborted = false;//#695
	return mTextFinderForCurrentWordHighlight.ExecuteFind(mText,mCurrentWordFindHighlightParam,false,inSpos,epos,outSpos,outEpos,abortflag,false,modalSessionAborted);
}

#pragma mark ===========================

#pragma mark ---Diffモード制御

//#81
/**
デフォルトのDiffモードを設定する
*/
void	ADocument_Text::SetDefaultDiffMode()
{
	//デフォルトのDiffモードを設定する。
	//優先順位
	//１．比較先フォルダが設定されており、比較先ファイルが存在する場合
	//２．レポジトリ
	
	//比較先フォルダが設定されており、比較先ファイルが存在する場合は、そのファイルと比較
	AFileAcc	file;
	if( GetDiffFileInDiffSourceFolder(file) == true )
	{
		SPI_SetDiffMode_File(file,kCompareMode_File);
		return;
	}
	//レポジトリと比較
	if( SPI_CanDiffWithRepositiory() == true )
	{
		SPI_SetDiffMode_Repository();
		return;
	}
}

/**
レポジトリ比較対象ファイルかどうか（SCMファイルかどうか）を判定
*/
ABool	ADocument_Text::SPI_CanDiffWithRepositiory() const
{
	if( NVI_IsFileSpecified() == true )
	{
		if( mSCMFileState != kSCMFileState_NotSCMFolder && mSCMFileState != kSCMFileState_NotEntried )//#568
		{
			return true;
		}
	}
	return false;
}

/**
比較先フォルダが設定されており、比較先ファイルが存在するかどうかを判定し、比較先ファイルを取得
*/
ABool	ADocument_Text::GetDiffFileInDiffSourceFolder( AFileAcc& outFile ) const
{
	ABool	result = false;
	//
	if( NVI_IsFileSpecified() == true )
	{
		//ファイルパス取得
		AFileAcc	file;
		NVI_GetFile(file);
		AText	path;
		file.GetPath(path);
		//Diff比較先フォルダ設定取得
		AText	diffSourceFolderWorkingPath;
		GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffSourceFolder_Working,diffSourceFolderWorkingPath);
		//このドキュメントのファイルがDiff比較先フォルダ内のファイルかどうかを判定
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDiffSourceFolder_Enable) == true &&
					diffSourceFolderWorkingPath.GetItemCount() > 0 && path.GetItemCount() > 0 && 
					path.GetItemCount() > diffSourceFolderWorkingPath.GetItemCount() && 
					diffSourceFolderWorkingPath.CompareMin(path) == true )
		{
			//相対パスを取得
			AText	relativePath;
			relativePath.InsertText(0,path,diffSourceFolderWorkingPath.GetItemCount(),
						path.GetItemCount()-diffSourceFolderWorkingPath.GetItemCount());
			//Diff比較元フォルダ取得
			AText	diffSourceFolderPath;
			GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffSourceFolder_Source,diffSourceFolderPath);
			AFileAcc	diffSourceFolder;
			diffSourceFolder.Specify(diffSourceFolderPath);
			//Diff比較元ファイル取得
			AFileAcc	file;
			file.SpecifyChild(diffSourceFolder,relativePath);
			//ファイルが存在しているかどうか判定
			if( file.Exist() == true )
			{
				outFile.CopyFrom(file);
				result = true;
			}
		}
	}
	return result;
}

//#379 
/**
Diffモードを設定（レポジトリとの比較を設定）
*/
void	ADocument_Text::SPI_SetDiffMode_Repository()
{
	//既にレポジトリとの比較モードであれば、何もせず終了
	if( mDiffTargetMode == kDiffTargetMode_RepositoryLatest )
	{
		return;
	}
	
	//compareモード（より詳細な比較先種別）設定
	mCompareMode = kCompareMode_Repository;
	//diffモード設定
	AFileAcc	dummy;
	SetDiffMode(kDiffTargetMode_RepositoryLatest,dummy,kIndex_Invalid,kIndex_Invalid);
}

//#379
/**
Diffモードを設定（ファイルとの比較を設定）
*/
void	ADocument_Text::SPI_SetDiffMode_File( const AFileAcc& inDiffTargetFile, const ACompareMode inCompareMode )
{
	/*比較元ファイルを変更時、再度比較元ファイルを選択して比較出来るようにするため、同じファイルかどうかのチェックは行わない
	//
	if( mDiffTargetMode == kDiffTargetMode_File && mDiffTargetFile.Compare(inDiffTargetFile) == true )
	{
		return;
	}
	*/
	
	//compareモード（より詳細な比較先種別）設定
	mCompareMode = inCompareMode;
	//diffモード設定
	SetDiffMode(kDiffTargetMode_File,inDiffTargetFile,kIndex_Invalid,kIndex_Invalid);
}

/**
Diffモード設定解除
*/
void	ADocument_Text::SPI_SetDiffMode_None()
{
	//既にdiffモード設定解除していれば、何もせず終了
	if( mDiffTargetMode == kDiffTargetMode_None )
	{
		return;
	}
	
	//compareモード（より詳細な比較先種別）設定
	mCompareMode = kCompareMode_NoCompare;
	//diffモード設定
	AFileAcc	dummy;
	SetDiffMode(kDiffTargetMode_None,dummy,kIndex_Invalid,kIndex_Invalid);
}

//#192
/**
Diffモードを設定（指定テキスト範囲と比較）
*/
void	ADocument_Text::SPI_SetDiffMode_TextWithRange( const AText& inTargetText, 
		const AIndex inDiffTextRangeStart, const AIndex inDiffTextRangeEnd,
		const ACompareMode inCompareMode )
{
	//diff比較用テンポラリフォルダ未生成なら生成、取得する
	if( mDiffTempFolder.IsSpecified() == false || mDiffTempFolder.Exist() == false )
	{
		GetApp().SPI_GetUniqTempFolder(mDiffTempFolder);
		mDiffTempFolder.CreateFolder();
	}
	//inTargetTextをUTF8/LFでテンポラリファイルに保存する
	AFileAcc	diffTargetFile;
	diffTargetFile.SpecifyChild(mDiffTempFolder,"difftargetfile");
	diffTargetFile.CreateFile();
	AText	text;
	text.SetText(inTargetText);
	text.ConvertLineEndToLF();
	diffTargetFile.WriteFile(text);
	//compareモード（より詳細な比較先種別）設定
	mCompareMode = inCompareMode;
	//diffモード設定
	SetDiffMode(kDiffTargetMode_TextWithRange,diffTargetFile,inDiffTextRangeStart,inDiffTextRangeEnd);
}

/**
Diffモード設定（共通処理）
*/
void	ADocument_Text::SetDiffMode( const ADiffTargetMode inMode, const AFileAcc& inDiffTargetFile,
		const AIndex inDiffTextRangeStart, const AIndex inDiffTextRangeEnd )
{
	//Diff情報全削除
	ClearDiffDisplay();
	//既存のdiff targetドキュメントを閉じる
	CloseCurrentDiffTargetDocument();
	//diff target modeを設定
	mDiffTargetMode = inMode;
	//DiffTargetFile設定
	mDiffTargetFile.CopyFrom(inDiffTargetFile);
	//diffテキスト範囲を設定
	mDiffTextRangeStart = inDiffTextRangeStart;
	mDiffTextRangeEnd = inDiffTextRangeEnd;
	//diffテキスト範囲開始段落を初期化
	//（必要な場合(=kDiffTargetMode_TextWithRange)、ExecuteDiff()で値が設定される）
	mDiffTextRangeStartParagraph = 0;
	//diff表示データ更新
	SPI_UpdateDiffDisplayData();
}

//
/**
既存のdiff targetドキュメントを閉じる
*/
void	ADocument_Text::CloseCurrentDiffTargetDocument()
{
	if( mDiffTargetFileDocumentID != kObjectID_Invalid )
	{
		//ドキュメント（に対応するウインドウ）を閉じる
		GetApp().SPI_ExecuteCloseTextDocument(mDiffTargetFileDocumentID);
		//diff target document idをクリア
		mDiffTargetFileDocumentID = kObjectID_Invalid;
	}
}

//R0006
/**
Diff表示データの更新を行う。Open/Save時等にコールされる。
何と比較するかはmDiffTargetMdoeで決まる
*/
void	ADocument_Text::SPI_UpdateDiffDisplayData( const ABool inReloadRepository )
{
	//#379 NVIDO_ViewRegistered()で判定if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == false )   return;
	if( NVI_IsDirty() == true )   return;
	//if( SPI_IsRemoteFileMode() == true || NVI_IsFileSpecified() == false )  return;
	
	switch(mDiffTargetMode)
	{
	  case kDiffTargetMode_RepositoryLatest:
		{
			if( SPI_IsRemoteFileMode() == true )  return;
			if( NVI_IsFileSpecified() == false )   return;
			//Diffデータ削除
			mDiffDisplayArray_DiffType.DeleteAll();
			mDiffDisplayArray_StartParagraphIndex.DeleteAll();
			mDiffDisplayArray_EndParagraphIndex.DeleteAll();
			mDiffDisplayArray_DeletedText.DeleteAll();//#379
			mDiffParagraphIndexArray.DeleteAll();//#379
			//#568 現在のSCMファイル状態チェック
			if( mSCMFileState == kSCMFileState_NotSCMFolder || mSCMFileState == kSCMFileState_NotEntried )
			{
				return;
			}
			//#379 mDiffDisplayMode = kDiffDisplayMode_None;
			//#379 Diffデータ更新（レポジトリテキスト生成＋Diffの場合は非同期実行、Diffのみの場合は同期実行）
			if( mRepositoryDocumentID == kObjectID_Invalid || inReloadRepository == true )
			{
				//SCMツールの比較コマンド実行（実行完了後、SPI_DoSCMCompareResult()がコールされる）
				AFileAcc	docfile;
				NVI_GetFile(docfile);
				mDiffDisplayWaitFlag = true;
				GetApp().SPI_GetSCMFolderManager().CompareWithLatest(docfile,GetObjectID());//非同期実行
			}
			else
			{
				//diffコマンド実行
				ExecuteDiff();
			}
			break;
		} 
	  case kDiffTargetMode_File:
	  case kDiffTargetMode_TextWithRange://#192
		{
			//if( SPI_IsRemoteFileMode() == false && NVI_IsFileSpecified() == false )   return;
			//
			mDiffDisplayArray_DiffType.DeleteAll();
			mDiffDisplayArray_StartParagraphIndex.DeleteAll();
			mDiffDisplayArray_EndParagraphIndex.DeleteAll();
			mDiffDisplayArray_DeletedText.DeleteAll();
			mDiffParagraphIndexArray.DeleteAll();
			//
			if( mDiffTargetFileDocumentID == kObjectID_Invalid )
			{
				//diff targetドキュメント生成
				//（SetDiffMode()経由の場合は、必ずこちらになる。SetDiffMode()で既存のdiff target documentを閉じているため）
				
				//mDiffTargetFileは、ファイルとの比較モードの場合は比較先ファイル、テキスト範囲比較モードの場合は比較テキストを保存したテンポラリファイル
				
				//タイトル取得
				AText	title;
				mDiffTargetFile.GetPath(title);
				title.AddCstring(":Diff");
				AText	tec;
				if( mDiffTargetMode == kDiffTargetMode_TextWithRange )
				{
					//テキスト範囲比較モードの場合は、必ずUTF8なので、テキストエンコーディングを指定してドキュメント生成
					tec.SetCstring("UTF-8");
				}
				else if( mDiffTargetMode == kDiffTargetMode_File )
				{
					//ファイル比較モードの場合は、比較先ファイルのTECは様々。
					
					//自動バックアップとの比較の場合は、自動バックアップファイル名＋_attrファイルにテキストエンコーディングが保存されているので、
					//それを指定
					switch(mCompareMode)
					{
					  case kCompareMode_AutoBackupMinutely:
					  case kCompareMode_AutoBackupHourly:
					  case kCompareMode_AutoBackupDaily:
					  case kCompareMode_AutoBackupMonthly:
						{
							AFileAcc	parentFolder;
							parentFolder.SpecifyParent(mDiffTargetFile);
							AText	filename;
							mDiffTargetFile.GetFilename(filename);
							filename.AddCstring("_attr");
							AFileAcc	tecfile;
							tecfile.SpecifyChild(parentFolder,filename);
							if( tecfile.Exist() == true )
							{
								tecfile.ReadTo(tec);
							}
							else
							{
								//_attrファイルが非存在の場合は、自ドキュメントのテキストエンコーディングにする
								SPI_GetTextEncoding(tec);
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
				//ウインドウ生成せずにドキュメント生成
				ATextDocumentFactory	factory(this,kObjectID_Invalid);
				mDiffTargetFileDocumentID = GetApp().SPNVI_CreateDiffTargetDocument(title,mDiffTargetFile,mModeIndex,tec,
						kIndex_Invalid,kIndex_Invalid,false,false,SPI_GetFirstWindowID());
				GetApp().SPI_GetTextDocumentByID(mDiffTargetFileDocumentID).mDiffWorkingDocumentID = GetObjectID();
				GetApp().SPI_GetTextDocumentByID(mDiffTargetFileDocumentID).SPI_SetOwnedDocument();
				//行折り返し計算（メインスレッド内で計算）
				GetApp().SPI_GetTextDocumentByID(mDiffTargetFileDocumentID).SPI_DoViewActivating(false);
			}
			else
			{
				//比較元ファイルが変更されている場合に、diff target documentのデータを更新する。
				//（diff target documentはdirtyにはならないので、ダイアログ無しでリロードされる）
				GetApp().SPI_GetTextDocumentByID(mDiffTargetFileDocumentID).SPI_CheckEditByOtherApp();
			}
			//Diffコマンド実行
			ExecuteDiff();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#379
/**
現在のDiffモードに応じてDiff対象ドキュメントを返す
*/
ADocumentID	ADocument_Text::SPI_GetDiffTargetDocumentID() const
{
	ADocumentID	diffTargetDocumentID = kObjectID_Invalid;
	switch(mDiffTargetMode)
	{
		//
	  case kDiffTargetMode_RepositoryLatest:
		{
			diffTargetDocumentID = mRepositoryDocumentID;
			break;
		}
	  case kDiffTargetMode_File:
	  case kDiffTargetMode_TextWithRange://#192
		{
			diffTargetDocumentID = mDiffTargetFileDocumentID;
			break;
		}
	  case kDiffTargetMode_None:
		{
			//Diff対象ドキュメントの場合は、対応するworkingドキュメントを返す
			if( mDiffWorkingDocumentID != kObjectID_Invalid )
			{
				diffTargetDocumentID = mDiffWorkingDocumentID;
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return diffTargetDocumentID;
}

/**
diff比較モード（diffモードより詳細な比較先）、および、比較先ファイル名取得
*/
ACompareMode	ADocument_Text::SPI_GetCompareMode( AText& outFilename ) const
{
	outFilename.DeleteAll();
	if( mDiffTargetMode == kDiffTargetMode_File )
	{
		mDiffTargetFile.GetFilename(outFilename);
	}
	return mCompareMode;
}

#pragma mark ===========================

#pragma mark ---Diff表示データ

//#379
/**
DiffArrayに差分データを設定する
*/
void	ADocument_Text::SetDiffArray( const AArray<ADiffType>& inDiffTypeArray, 
		const AArray<AIndex>& inStartParagraphIndexArray, const AArray<AIndex>& inEndParagraphIndexArray,
		const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray,//#379
		const ATextArray& inDeletedText, const ABool inSetParagraphIndexArray )
{
	//色取得
	AColor	subWindowNormalColor = kColor_List_Normal;//#1316 GetApp().SPI_GetSubWindowLetterColor();
	AColor	jumpListDiffColor = kColor_List_Red;//#1316 GetApp().SPI_GetJumpListDiffColor();
	
	//#304 上記フラグの前にあった処理をここに移動
	mDiffDisplayArray_DiffType.DeleteAll();
	mDiffDisplayArray_StartParagraphIndex.DeleteAll();
	mDiffDisplayArray_EndParagraphIndex.DeleteAll();
	mDiffDisplayArray_DeletedText.DeleteAll();//#379
	mDiffParagraphIndexArray.DeleteAll();//#379
	
	if( inDiffTypeArray.GetItemCount() > 0 )
	{
		//Diffデータ追加
		for( AIndex i = 0; i < inDiffTypeArray.GetItemCount(); i++ )
		{
			mDiffDisplayArray_DiffType.Add(inDiffTypeArray.Get(i));
			mDiffDisplayArray_StartParagraphIndex.Add(inStartParagraphIndexArray.Get(i));//#379 SPI_GetLineIndexByParagraphIndex(inStartParagraphIndexArray.Get(i)));
			mDiffDisplayArray_EndParagraphIndex.Add(inEndParagraphIndexArray.Get(i));//#379 SPI_GetLineIndexByParagraphIndex(inEndParagraphIndexArray.Get(i)));
			mDiffDisplayArray_DeletedText.Add(inDeletedText.GetTextConst(i));//#379
		}
		//#379 mDiffDisplayMode = kDiffDisplayMode_RepositoryLatest;
		
		//ジャンプリスト更新
		//#844 AColor	diffcolor = kColor_DarkBlue;
		//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_JumpList,diffcolor);
		//各ジャンプリスト項目毎のループ
		for( AIndex i = 0; i < mJump_IdentifierObjectArray.GetItemCount(); i++ )
		{
			/*spt,eptではなく直接paragraphを取得するように変更#695
			ATextPoint	spt,ept;
			SPI_GetGlobalIdentifierRange(mJump_IdentifierObjectArray.Get(i),spt,ept);
			//暫定 mJump_IdentifierObjectArrayに存在するIdentifierが、TextInfoのmGlobalIdentifierListに存在しないことがある。
			//理由調査中だが、とりあえず、問題発生時はbreakすることにする。#571
			*/
			AIndex	startParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(i));//#695
			if( /*#695 spt.y == kIndex_Invalid || ept.y == kIndex_Invalid*/startParagraphIndex == kIndex_Invalid )
			{
				_ACError("Object in mJump_IdentifierObjectArray does not exist.",this);
				break;
			}
			//
			AIndex	endParagraphIndex = SPI_GetParagraphCount();//#695
			if( i+1 < mJump_IdentifierObjectArray.GetItemCount() )
			{
				/*#695
				ATextPoint	s,e;
				SPI_GetGlobalIdentifierRange(mJump_IdentifierObjectArray.Get(i+1),s,e);
				ept = e;
				*/
				endParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(i+1));//#695
			}
			/*#695
			else
			{
				ept.x = 0;
				ept.y = SPI_GetLineCount()-1;//#379
			}
			*/
			//ジャンプリスト項目範囲内にdiff範囲が存在するかどうかを判定
			for( AIndex j = 0; j < mDiffDisplayArray_StartParagraphIndex.GetItemCount(); j++ )
			{
				if( ( /*#695 SPI_GetParagraphIndexByLineIndex(ept.y)*/
								endParagraphIndex <= mDiffDisplayArray_StartParagraphIndex.Get(j) || 
								/*#695 SPI_GetParagraphIndexByLineIndex(spt.y)*/
								startParagraphIndex >= mDiffDisplayArray_EndParagraphIndex.Get(j) ) == false )//#379
				{
					mJump_MenuColorArray.Set(i,jumpListDiffColor);
				}
				//else fprintf(stderr,"s:%d e:%d sd:%d ed:%d   ",spt.y,ept.y,mDiffDisplayArray_StartParagraphIndex.Get(j),mDiffDisplayArray_EndParagraphIndex.Get(j));
			}
		}
	}
	else
	{
		for( AIndex i = 0; i < mJump_MenuColorArray.GetItemCount(); i++ )
		{
			mJump_MenuColorArray.Set(i,subWindowNormalColor);
		}
	}
	//#212 RefreshWindow();
	//関数外側でコールするSPI_RefreshTextViews();//#212
	SPI_UpdateJumpList();//#695 true);//#291
	
	if( inSetParagraphIndexArray == true )
	{
		/*#737
		//Repositoryドキュメントの対応する段落インデックスデータmDiffParagraphIndexArrayを作成
		AIndex	lastWorkingParagraphIndex = 0;
		AIndex	lastRepositoryParagraphIndex = 0;
		//各DiffPartごとのループ
		for( AIndex i = 0; i < mDiffDisplayArray_DiffType.GetItemCount(); i++ )
		{
			//前回のDiffPart開始位置までの対応行を追加
			AIndex	diffStart = mDiffDisplayArray_StartParagraphIndex.Get(i);
			for( ; lastWorkingParagraphIndex < diffStart; lastWorkingParagraphIndex++ )
			{
				mDiffParagraphIndexArray.Add(lastRepositoryParagraphIndex);
				lastRepositoryParagraphIndex++;
			}
			//DiffPart内の対応行を追加
			AIndex	diffEnd = mDiffDisplayArray_EndParagraphIndex.Get(i);
			for( ; lastWorkingParagraphIndex < diffEnd; lastWorkingParagraphIndex++ )
			{
				mDiffParagraphIndexArray.Add(inOldStartParagraphIndexArray.Get(i));
			}
			//次のRepository側開始位置を設定
			if( mDiffDisplayArray_DiffType.Get(i) == kDiffType_Added )
			{
				lastRepositoryParagraphIndex = inOldStartParagraphIndexArray.Get(i);
			}
			else
			{
				lastRepositoryParagraphIndex = inOldEndParagraphIndexArray.Get(i);
			}
		}
		//最後のDiffPart以降の対応行を追加
		AItemCount	paraCount = SPI_GetParagraphCount();
		for( ; lastWorkingParagraphIndex < paraCount; lastWorkingParagraphIndex++ )
		{
			mDiffParagraphIndexArray.Add(lastRepositoryParagraphIndex);
			lastRepositoryParagraphIndex++;
		}
		*/
		SPI_GetDiffParagraphIndexArray(SPI_GetParagraphCount(),mDiffDisplayArray_DiffType,
					mDiffDisplayArray_StartParagraphIndex,mDiffDisplayArray_EndParagraphIndex,
					inOldStartParagraphIndexArray,inOldEndParagraphIndexArray,mDiffParagraphIndexArray);
	}
}

/**
diff target documentの段落との対応テーブルを取得
*/
void	ADocument_Text::SPI_GetDiffParagraphIndexArray( const AItemCount inParagraphCount,
		const AArray<ADiffType>& inDiffTypeArray, 
		const AHashArray<AIndex>& inStartParagraphIndexArray, const AHashArray<AIndex>& inEndParagraphIndexArray,
		const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray,
		AArray<AIndex>& outDiffParagraphIndexArray )
{
	outDiffParagraphIndexArray.DeleteAll();
	//Repositoryドキュメントの対応する段落インデックスデータmDiffParagraphIndexArrayを作成
	AIndex	lastWorkingParagraphIndex = 0;
	AIndex	lastRepositoryParagraphIndex = 0;
	//各DiffPartごとのループ
	for( AIndex i = 0; i < inDiffTypeArray.GetItemCount(); i++ )
	{
		//前回のDiffPart開始位置までの対応行を追加
		AIndex	diffStart = inStartParagraphIndexArray.Get(i);
		for( ; lastWorkingParagraphIndex < diffStart; lastWorkingParagraphIndex++ )
		{
			outDiffParagraphIndexArray.Add(lastRepositoryParagraphIndex);
			lastRepositoryParagraphIndex++;
		}
		//DiffPart内の対応行を追加
		AIndex	diffEnd = inEndParagraphIndexArray.Get(i);
		for( ; lastWorkingParagraphIndex < diffEnd; lastWorkingParagraphIndex++ )
		{
			outDiffParagraphIndexArray.Add(inOldStartParagraphIndexArray.Get(i));
		}
		//次のRepository側開始位置を設定
		if( inDiffTypeArray.Get(i) == kDiffType_Added )
		{
			lastRepositoryParagraphIndex = inOldStartParagraphIndexArray.Get(i);
		}
		else
		{
			lastRepositoryParagraphIndex = inOldEndParagraphIndexArray.Get(i);
		}
	}
	//最後のDiffPart以降の対応行を追加
	for( ; lastWorkingParagraphIndex < inParagraphCount; lastWorkingParagraphIndex++ )
	{
		outDiffParagraphIndexArray.Add(lastRepositoryParagraphIndex);
		lastRepositoryParagraphIndex++;
	}
	
}

//#450
/**
指定行を含む関数内に差分が存在するかどうかを返す
*/
/*
ABool	ADocument_Text::SPI_DiffExistInCurrentFunction( const AIndex inLineIndex ) const
{
	//指定行を含む関数のjump item indexを取得
	AIndex	jumpItemIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);
	
	//関数の開始段落index取得
	AIndex	startParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpItemIndex));//#695
	if( startParagraphIndex == kIndex_Invalid )
	{
		_ACError("Object in mJump_IdentifierObjectArray does not exist.",this);
		return false;
	}
	//次の関数の開始段落index取得
	AIndex	endParagraphIndex = SPI_GetParagraphCount();//#695
	if( jumpItemIndex+1 < mJump_IdentifierObjectArray.GetItemCount() )
	{
		endParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpItemIndex+1));//#695
	}
	//ジャンプリスト項目範囲内にdiff範囲が存在するかどうかを判定
	for( AIndex j = 0; j < mDiffDisplayArray_StartParagraphIndex.GetItemCount(); j++ )
	{
		if( ( endParagraphIndex <= mDiffDisplayArray_StartParagraphIndex.Get(j) || 
						startParagraphIndex >= mDiffDisplayArray_EndParagraphIndex.Get(j) ) == false )//#379
		{
			return true;
		}
	}
	return false;
}
*/

/**
指定範囲内にdiff targetドキュメントとの差分があるかどうかを取得
*/
ABool	ADocument_Text::SPI_DiffExistInRange( const AIndex inStartLineIndex, const AIndex inEndLineIndex ) const
{
	//指定範囲の段落indexを取得
	AIndex	startParagraphIndex = SPI_GetParagraphIndexByLineIndex(inStartLineIndex);
	AIndex	endParagraphIndex = SPI_GetParagraphIndexByLineIndex(inEndLineIndex);
	//ジャンプリスト項目範囲内にdiff範囲が存在するかどうかを判定
	for( AIndex j = 0; j < mDiffDisplayArray_StartParagraphIndex.GetItemCount(); j++ )
	{
		if( ( endParagraphIndex <= mDiffDisplayArray_StartParagraphIndex.Get(j) || 
						startParagraphIndex >= mDiffDisplayArray_EndParagraphIndex.Get(j) ) == false )//#379
		{
			return true;
		}
	}
	return false;
}

//#379
/**
このドキュメントの段落indexに対応する、Diff対象ドキュメントの段落indexを取得
*/
AIndex	ADocument_Text::SPI_GetDiffTargetParagraphIndexFromThisDocumentParagraphIndex( const AIndex inParagraphIndex ) const
{
	//
	if( inParagraphIndex >= mDiffParagraphIndexArray.GetItemCount() )
	{
		return kIndex_Invalid;
	}
	//mDiffParagraphIndexArrayの指定indexの値を返す
	return mDiffParagraphIndexArray.Get(inParagraphIndex);
}

//#379
/**
所有中のドキュメントのIDを返す
*/
void	ADocument_Text::SPI_GetOwnDocIDArray( AArray<ADocumentID>& outOwnDocArray ) const
{
	//RepositoryDocumentありならそれを追加
	if( mRepositoryDocumentID != kObjectID_Invalid )
	{
		outOwnDocArray.Add(mRepositoryDocumentID);
	}
	//DiffTargetFileDocumentありならそれを追加
	if( mDiffTargetFileDocumentID != kObjectID_Invalid )
	{
		outOwnDocArray.Add(mDiffTargetFileDocumentID);
	}
}

//#379
/**
DiffPartのインデックスからDiffTypeを取得
*/
ADiffType	ADocument_Text::SPI_GetDiffTypeByDiffIndex( const AIndex inDiffIndex ) const
{
	return mDiffDisplayArray_DiffType.Get(inDiffIndex);
}

/**
行IndexからDiffTypeを取得
*/
ADiffType	ADocument_Text::SPI_GetDiffTypeByIncludingLineIndex( const AIndex inLineIndex ) const
{
	//段落indexを取得
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);//#379
	//テキスト範囲比較モードの場合、テキスト範囲外であれば、ここで終了（指定行はグレー表示される）
	if( mDiffTargetMode == kDiffTargetMode_TextWithRange )
	{
		if( paraIndex < mDiffTextRangeStartParagraph || paraIndex > mDiffTextRangeEndParagraph )
		{
			return kDiffType_OutOfDiffRange;
		}
	}
	//
	AItemCount	itemcount = mDiffDisplayArray_StartParagraphIndex.GetItemCount();
	for( AIndex i = 0; i < itemcount; i++ )
	{
		if( /*#379 inLineIndex*/paraIndex >= mDiffDisplayArray_StartParagraphIndex.Get(i) &&
					/*#379 inLineIndex*/paraIndex < mDiffDisplayArray_EndParagraphIndex.Get(i) )
		{
			return mDiffDisplayArray_DiffType.Get(i);
		}
	}
	return kDiffType_None;
}

//#379
/**
DiffPartのインデックスからそのPartの開始段落を取得
*/
AIndex	ADocument_Text::SPI_GetDiffStartParagraphIndexByDiffIndex( const AIndex inDiffIndex ) const
{
	return mDiffDisplayArray_StartParagraphIndex.Get(inDiffIndex);
}

//#379
/**
DiffPartのインデックスからそのPartの開始行を取得
*/
AIndex	ADocument_Text::SPI_GetDiffStartLineIndexByDiffIndex( const AIndex inDiffIndex ) const
{
	AIndex	paraIndex = mDiffDisplayArray_StartParagraphIndex.Get(inDiffIndex);
	return SPI_GetLineIndexByParagraphIndex(paraIndex);
}

//#379
/**
DiffPartのインデックスからそのPartの終了行を取得
*/
AIndex	ADocument_Text::SPI_GetDiffEndLineIndexByDiffIndex( const AIndex inDiffIndex ) const
{
	AIndex	paraIndex = mDiffDisplayArray_EndParagraphIndex.Get(inDiffIndex);
	return SPI_GetLineIndexByParagraphIndex(paraIndex);
}

//#379
/**
行IndexからDiffPartインデックスを取得
*/
AIndex	ADocument_Text::SPI_GetDiffIndexByIncludingLineIndex( const AIndex inLineIndex ) const
{
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	AItemCount	itemcount = mDiffDisplayArray_StartParagraphIndex.GetItemCount();
	for( AIndex i = 0; i < itemcount; i++ )
	{
		if( paraIndex >= mDiffDisplayArray_StartParagraphIndex.Get(i) &&
					paraIndex < mDiffDisplayArray_EndParagraphIndex.Get(i) )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

//#379
/**
DiffPart開始段落からDiffPartインデックスを取得
*/
AIndex	ADocument_Text::SPI_GetDiffIndexByStartParagraphIndex( const AIndex inParagraphIndex ) const
{
	return mDiffDisplayArray_StartParagraphIndex.Find(inParagraphIndex);
}

//#379
/**
DiffPart終了行からDiffPartインデックスを取得
*/
AIndex	ADocument_Text::SPI_GetDiffIndexByEndParagraphIndex( const AIndex inParagraphIndex ) const
{
	return mDiffDisplayArray_EndParagraphIndex.Find(inParagraphIndex);
}

//#379
/**
DiffPart開始行からDiffPartインデックスを取得
*/
AIndex	ADocument_Text::SPI_GetDiffIndexByStartLineIndex( const AIndex inLineIndex ) const
{
	//段落開始行以外なら検索しない
	if( inLineIndex > 0 )
	{
		if( SPI_GetLineExistLineEnd(inLineIndex-1) == false )   return kIndex_Invalid;
	}
	//指定行から始まるDiffIndexを検索
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	return mDiffDisplayArray_StartParagraphIndex.Find(paraIndex);
}

//#379
/**
DiffPart終了行からDiffPartインデックスを取得
*/
AIndex	ADocument_Text::SPI_GetDiffIndexByEndLineIndex( const AIndex inLineIndex ) const
{
	//段落開始行以外なら検索しない
	if( inLineIndex > 0 )
	{
		if( SPI_GetLineExistLineEnd(inLineIndex-1) == false )   return kIndex_Invalid;
	}
	//指定行で終了するDiffIndexを検索
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	return mDiffDisplayArray_EndParagraphIndex.Find(paraIndex);
}

/**
diff相手側ドキュメントのtext pointから、自分側の対応するtext pointを取得する。
*/
ABool	ADocument_Text::SPI_GetCorrespondingTextPointByDiffTargetTextPoint( const ATextPoint& inDiffTargetTextPoint,
		ATextPoint& outTextPoint ) const
{
	//結果初期化
	outTextPoint.x = outTextPoint.y = 0;
	
	//相手側ドキュメント取得
	if( SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )   return false;
	const ADocument_Text&	otherDocument = GetApp().SPI_GetTextDocumentByID(SPI_GetDiffTargetDocumentID());
	
	//引数inDiffTargetTextPoint.yの行に対応する段落indexを取得
	AIndex	otherDocParaIndex = otherDocument.SPI_GetParagraphIndexByLineIndex(inDiffTargetTextPoint.y);
	
	//引数inLineIndexの行を含んでいるdiffパートindexを取得
	AIndex	diffIndex = kIndex_Invalid;
	ABool	insideDiffPart = false;
	AItemCount	diffitemcount = otherDocument.mDiffDisplayArray_StartParagraphIndex.GetItemCount();
	for( AIndex i = 0; i < diffitemcount; i++ )
	{
		if( otherDocParaIndex >= otherDocument.mDiffDisplayArray_StartParagraphIndex.Get(i) &&
					otherDocParaIndex < otherDocument.mDiffDisplayArray_EndParagraphIndex.Get(i) )
		{
			diffIndex = i;
			insideDiffPart = true;
			break;
		}
		if( otherDocParaIndex < otherDocument.mDiffDisplayArray_StartParagraphIndex.Get(i) )
		{
			diffIndex = i;
			insideDiffPart = false;
			break;
		}
	}
	//自分側の対応する段落index格納変数
	AIndex	thisDocParaIndex = kIndex_Invalid;
	if( diffIndex == kIndex_Invalid )
	{
		//diffパートが見つからない場合（＝最後のdiffパートより後）、最終段落からの段落数を合わせる
		AIndex	otherDocDiffRangeStartParaIndex = 0, otherDocDiffRangeEndParaIndex = 0;
		otherDocument.SPI_GetDiffTextRangeParagraphIndex(otherDocDiffRangeStartParaIndex,otherDocDiffRangeEndParaIndex);
		thisDocParaIndex = SPI_GetParagraphCount() - (otherDocDiffRangeEndParaIndex - otherDocParaIndex);
	}
	else
	{
		//こちら側のDiff情報が未設定時にこの関数がコールされる場合があるので、その場合はここで終了
		if( diffIndex >= mDiffDisplayArray_StartParagraphIndex.GetItemCount() )   return false;
		
		//こちら側diffパートの開始段落・終了段落を取得
		ATextIndex	thisDocStartParaIndex = mDiffDisplayArray_StartParagraphIndex.Get(diffIndex);
		ATextIndex	thisDocEndParaIndex = mDiffDisplayArray_EndParagraphIndex.Get(diffIndex);
		
		//相手側diffパートの開始段落・終了段落を取得
		ATextIndex	otherDocStartParaIndex = otherDocument.mDiffDisplayArray_StartParagraphIndex.Get(diffIndex);
		ATextIndex	otherDocEndParaIndex = otherDocument.mDiffDisplayArray_EndParagraphIndex.Get(diffIndex);
		
		//diffパート内かどうかの判定
		if( insideDiffPart == true )
		{
			//------------------diffパート内の場合------------------
			//こちら側の対応する段落indexを取得
			thisDocParaIndex = thisDocStartParaIndex + (otherDocParaIndex-otherDocStartParaIndex);
			//こちら側に対応する行がない（diff part範囲外）場合はdiff部分の最終行にする
			if( thisDocParaIndex > thisDocEndParaIndex-1 )
			{
				thisDocParaIndex = thisDocEndParaIndex-1;
			}
		}
		else
		{
			//------------------diffパート外の場合------------------
			//こちら側の対応する段落indexを取得
			thisDocParaIndex = thisDocStartParaIndex - (otherDocStartParaIndex-otherDocParaIndex);
		}
	}
	
	//自分側の対応する段落indexが不正の場合はfalseで終了
	if( thisDocParaIndex < 0 || thisDocParaIndex >= SPI_GetParagraphCount() )
	{
		return false;
	}
	
	//自分側の対応する段落indexから、text pointに変換
	AIndex	thisDocLineIndex = SPI_GetLineIndexByParagraphIndex(thisDocParaIndex);
	outTextPoint.x = 0;
	outTextPoint.y = thisDocLineIndex;
	return true;
}

//#0
/**
diffの対象範囲段落を取得する
*/
void	ADocument_Text::SPI_GetDiffTextRangeParagraphIndex( AIndex& outStartParagraphIndex, AIndex& outEndParagraphIndex ) const
{
	if( mDiffTargetMode == kDiffTargetMode_TextWithRange )
	{
		outStartParagraphIndex = mDiffTextRangeStartParagraph;
		outEndParagraphIndex = mDiffTextRangeEndParagraph;
	}
	else
	{
		outStartParagraphIndex = 0;
		outEndParagraphIndex = SPI_GetParagraphCount();
	}
}

//#379
/**
文字ごと比較実行
@param inLineIndex 比較する行
@param outDiffStart 差分部分開始位置（行内のindex)
@param outDiffEnd 差分部分終了位置（行内のindex)
*/
void	ADocument_Text::SPI_GetCharDiffInfo( const AIndex inLineIndex, 
		AArray<AIndex>& outDiffStart, AArray<AIndex>& outDiffEnd )
{
	//結果初期化
	outDiffStart.DeleteAll();
	outDiffEnd.DeleteAll();
	
	//相手側ドキュメント取得
	if( SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )   return;
	const ADocument_Text&	otherDocument = GetApp().SPI_GetTextDocumentByID(SPI_GetDiffTargetDocumentID());
	
	//引数inLineIndexの行に対応する段落indexを取得
	AIndex	thisDocParaIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	
	//引数inLineIndexの行を含んでいるdiffパートindexを取得
	AIndex	diffIndex = SPI_GetDiffIndexByIncludingLineIndex(inLineIndex);
	if( diffIndex == kIndex_Invalid )   return;
	
	//相手側のDiff情報が未設定時にこの関数がコールされる場合があるので、その場合はここで終了
	if( diffIndex >= otherDocument.mDiffDisplayArray_StartParagraphIndex.GetItemCount() )   return;
	
	//こちら側diffパートの開始段落・終了段落を取得
	ATextIndex	thisDocStartParaIndex = mDiffDisplayArray_StartParagraphIndex.Get(diffIndex);
	
	//相手側diffパートの開始段落・終了段落を取得
	ATextIndex	otherDocStartParaIndex = otherDocument.mDiffDisplayArray_StartParagraphIndex.Get(diffIndex);
	ATextIndex	otherDocEndParaIndex = otherDocument.mDiffDisplayArray_EndParagraphIndex.Get(diffIndex);
	
	//相手側に対応する行がない場合は終了
	if( thisDocParaIndex-thisDocStartParaIndex >= otherDocEndParaIndex-otherDocStartParaIndex )   return;
	
	//相手側の対応する段落indexを取得
	AIndex	otherDocParaIndex = otherDocStartParaIndex + (thisDocParaIndex-thisDocStartParaIndex);
	
	//------------------ここまでで得た開始段落・終了段落はthidDoc/otherDocごとなので、working/diffTargetに変換する------------------
	AIndex	workingParaIndex = kIndex_Invalid;
	AIndex	diffParaIndex = kIndex_Invalid;
	ADocumentID	workingDocumentID = kObjectID_Invalid;
	ADocumentID	diffTargetDocumentID = kObjectID_Invalid;
	if( mTextDocumentType == kTextDocumentType_DiffTarget )
	{
		//DiffTargetドキュメント側の場合
		workingParaIndex = otherDocParaIndex;
		diffParaIndex = thisDocParaIndex;
		workingDocumentID = SPI_GetDiffTargetDocumentID();
		diffTargetDocumentID = GetObjectID();
	}
	else
	{
		//通常ドキュメント側の場合
		workingParaIndex = thisDocParaIndex;
		diffParaIndex = otherDocParaIndex;
		workingDocumentID = GetObjectID();
		diffTargetDocumentID = SPI_GetDiffTargetDocumentID();
	}
	const ADocument_Text&	diffTargetDocument = GetApp().SPI_GetTextDocumentByID(diffTargetDocumentID);
	const ADocument_Text&	workingDocument = GetApp().SPI_GetTextDocumentByID(workingDocumentID);
	
	//working側の段落開始TextIndex、行の長さを取得
	ATextIndex	workingParaStartTextIndex = workingDocument.SPI_GetParagraphStartTextIndex(workingParaIndex);
	ATextIndex	workingParaEndTextIndex = workingDocument.SPI_GetParagraphStartTextIndex(workingParaIndex+1);// -1;
	//Diff対象側の段落開始TextIndexを取得
	ATextIndex	diffParaStartTextIndex = diffTargetDocument.SPI_GetParagraphStartTextIndex(diffParaIndex);
	ATextIndex	diffParaEndTextIndex = diffTargetDocument.SPI_GetParagraphStartTextIndex(diffParaIndex+1);// -1;
	//working, diffそれぞれの現在位置
	AIndex	workingPos = workingParaStartTextIndex;
	AIndex	diffPos = diffParaStartTextIndex;
	//------------------ループ（基本的にworking側の対象行の各文字について比較実行→不一致のときは次に同期する場所を検索）------------------
	AStTextPtr	workingTextPtr(workingDocument.mText,0,workingDocument.mText.GetItemCount());
	AStTextPtr	diffTextPtr(diffTargetDocument.mText,0,diffTargetDocument.mText.GetItemCount());
	for( ;workingPos < workingParaEndTextIndex; )
	{
		//Diff側に対応文字がなければ終了
		if( diffPos >= diffTargetDocument.mText.GetItemCount() )   break;
		//working側文字取得
		AUCS4Char	workingChar = 0;
		ATextIndex	s = workingPos;
		ATextIndex	e = workingPos + AText::Convert1CharToUCS4(workingTextPtr.GetPtr(),workingTextPtr.GetByteCount(),workingPos,workingChar);
		//Diff側文字位置取得
		AUCS4Char	diffChar = 0;
		ATextIndex	ds = diffPos;
		ATextIndex	de = diffPos + AText::Convert1CharToUCS4(diffTextPtr.GetPtr(),diffTextPtr.GetByteCount(),diffPos,diffChar);
		//比較
		if( workingChar != diffChar )
		{
			//------------------文字不一致時------------------
			//今回の文字以降で、次に同期する場所を検索する
			ABool	resync = false;
			for( AIndex pos = s; pos < workingParaEndTextIndex; )
			{
				//
				AIndex	epos = pos + 8;
				if( epos > workingParaEndTextIndex )
				{
					epos = workingParaEndTextIndex;
				}
				epos = workingDocument.mText.GetCurrentCharPos(epos);
				AText	text;
				workingDocument.SPI_GetText(pos,epos,text);
				//
				AIndex	diffResyncPos = kIndex_Invalid;
				if( diffTargetDocument.mText.FindText(ds,text,diffResyncPos,diffParaEndTextIndex) == true )
				{
					if( mTextDocumentType == kTextDocumentType_DiffTarget )
					{
						outDiffStart.Add(ds);
						outDiffEnd.Add(diffResyncPos);
					}
					else
					{
						outDiffStart.Add(s);
						outDiffEnd.Add(pos);
					}
					//次の位置
					workingPos = pos;
					diffPos = diffResyncPos;
					//念のため
					if( workingPos <= s && diffPos <= ds )
					{
						workingPos = e;
						diffPos = de;
					}
					//
					resync = true;
					break;
				}
				//
				pos = workingDocument.mText.GetNextCharPos(pos);
			}
			//
			if( resync == false )
			{
				if( mTextDocumentType == kTextDocumentType_DiffTarget )
				{
					outDiffStart.Add(ds);
					outDiffEnd.Add(diffParaEndTextIndex);
				}
				else
				{
					outDiffStart.Add(s);
					outDiffEnd.Add(workingParaEndTextIndex);
				}
				break;
			}
		}
		else
		{
			//------------------文字一致時------------------
			//次の位置
			workingPos = e;
			diffPos = de;
		}
	}
}

//#379
/**
Diff実行
*/
void	ADocument_Text::ExecuteDiff()
{
	//Diffパス・オプション取得
	AText	diffExeFilePath;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffPath,diffExeFilePath);
	AText	diffOption;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffOption,diffOption);
	
#if IMPLEMENTATION_FOR_WINDOWS
	//★暫定 Windows版はDiffパス固定（アプリに付属のdiff.exeを実行）
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	AFileAcc	diffExeFile;
	diffExeFile.SpecifySister(appFile,"diff\\diff.exe");
	diffExeFile.GetPath(diffExeFilePath);
#endif
	
	//diffコマンドパス
	AText	command;
	command.SetText(diffExeFilePath);
	//引数
	ATextArray	argArray;
	argArray.Add(command);
	//Unified Diff設定
	AText	t;
	t.SetCstring("-u");
	argArray.Add(t);
	//Diffオプション
	if( diffOption.GetItemCount() > 0 )
	{
		argArray.Add(diffOption);
	}
	//ターゲットモード毎の処理
	//いずれの場合もdiff比較テンポラリフォルダを生成し、diff対象ドキュメントのファイルと、自ドキュメントのファイルに書き込んでファイル間で比較
	AText	dirpath;
	switch(mDiffTargetMode)
	{
	  case kDiffTargetMode_RepositoryLatest:
		{
			//==================レポジトリと比較==================
			//diff比較テンポラリフォルダ未生成なら生成
			if( mDiffTempFolder.IsSpecified() == false || mDiffTempFolder.Exist() == false )
			{
				GetApp().SPI_GetUniqTempFolder(mDiffTempFolder);
				mDiffTempFolder.CreateFolder();
			}
			//------------Diff対象ファイルを生成、引数に設定------------
			//diff対象ドキュメントからテキストを取得し、LFへ変換する
			AText	diffTargetText;
			GetApp().SPI_GetTextDocumentConstByID(mRepositoryDocumentID).SPI_GetText(diffTargetText);
			diffTargetText.ConvertLineEndToLF();
			//テンポラリファイルに保存
			AFileAcc	targetFile;
			targetFile.SpecifyChild(mDiffTempFolder,"difftargetfile");
			targetFile.CreateFile();
			targetFile.WriteFile(diffTargetText);
			//引数に設定
			AText	targetFilePath;
			targetFile.GetPath(targetFilePath);
			argArray.Add(targetFilePath);
			//------------このドキュメントのファイル------------
			//自ドキュメントのテキストを取得し、LFへ変換
			AText	text;
			text.SetText(mText);
			text.ConvertLineEndToLF();
			//テンポラリファイルに保存
			AFileAcc	workingFile;
			workingFile.SpecifyChild(mDiffTempFolder,"workingfile");
			workingFile.CreateFile();
			workingFile.WriteFile(text);
			//引数に設定
			AText	workingFilePath;
			workingFile.GetPath(workingFilePath);
			argArray.Add(workingFilePath);
			//------------カレントディレクトリ------------
			AText	dirpath;
			mDiffTempFolder.GetNormalizedPath(dirpath);
			break;
		}
	  case kDiffTargetMode_File:
		{
			//diff比較テンポラリフォルダ未生成なら生成
			if( mDiffTempFolder.IsSpecified() == false || mDiffTempFolder.Exist() == false )
			{
				GetApp().SPI_GetUniqTempFolder(mDiffTempFolder);
				mDiffTempFolder.CreateFolder();
			}
			//------------Diff対象ファイル------------
			//diff対象ドキュメントからテキストを取得し、LFへ変換する
			AText	diffTargetText;
			GetApp().SPI_GetTextDocumentConstByID(mDiffTargetFileDocumentID).SPI_GetText(diffTargetText);
			diffTargetText.ConvertLineEndToLF();
			//テンポラリファイルに保存
			AFileAcc	targetFile;
			targetFile.SpecifyChild(mDiffTempFolder,"difftargetfile");
			targetFile.CreateFile();
			targetFile.WriteFile(diffTargetText);
			//引数に設定
			AText	targetFilePath;
			targetFile.GetPath(targetFilePath);
			argArray.Add(targetFilePath);
			//------------このドキュメントのファイル------------
			//自ドキュメントのテキストを取得し、LFへ変換
			AText	text;
			text.SetText(mText);
			text.ConvertLineEndToLF();
			//テンポラリファイルに保存
			AFileAcc	workingFile;
			workingFile.SpecifyChild(mDiffTempFolder,"workingfile");
			workingFile.CreateFile();
			workingFile.WriteFile(text);
			//引数に設定
			AText	workingFilePath;
			workingFile.GetPath(workingFilePath);
			argArray.Add(workingFilePath);
			//------------カレントディレクトリ------------
			AText	dirpath;
			mDiffTempFolder.GetNormalizedPath(dirpath);
			break;
		}
		//#192
	  case kDiffTargetMode_TextWithRange:
		{
			//diff比較テンポラリフォルダ未生成なら生成
			if( mDiffTempFolder.IsSpecified() == false || mDiffTempFolder.Exist() == false )
			{
				GetApp().SPI_GetUniqTempFolder(mDiffTempFolder);
				mDiffTempFolder.CreateFolder();
			}
			//------------Diff対象ファイル------------
			//diff対象ドキュメントからテキストを取得し、LFへ変換する
			AText	diffTargetText;
			GetApp().SPI_GetTextDocumentConstByID(mDiffTargetFileDocumentID).SPI_GetText(diffTargetText);
			diffTargetText.ConvertLineEndToLF();
			//テンポラリファイルに保存
			AFileAcc	targetFile;
			targetFile.SpecifyChild(mDiffTempFolder,"difftargetfile");
			targetFile.CreateFile();
			targetFile.WriteFile(diffTargetText);
			//引数に設定
			AText	targetFilePath;
			targetFile.GetPath(targetFilePath);
			argArray.Add(targetFilePath);
			//------------このドキュメントのファイル------------
			//開始・終了text index取得
			AIndex	start = mDiffTextRangeStart;
			if( start > mText.GetItemCount() )
			{
				start = mText.GetItemCount();
			}
			start = mText.GetCurrentCharPos(start);
			AIndex	end = mDiffTextRangeEnd;
			if( end > mText.GetItemCount() )
			{
				end = mText.GetItemCount();
			}
			end = mText.GetCurrentCharPos(end);
			//開始段落index取得
			ATextPoint	startpt = {0};
			SPI_GetTextPointFromTextIndex(start,startpt);
			mDiffTextRangeStartParagraph = SPI_GetParagraphIndexByLineIndex(startpt.y);
			//終了段落index取得
			ATextPoint	endpt = {0};
			SPI_GetTextPointFromTextIndex(end,endpt);
			mDiffTextRangeEndParagraph = SPI_GetParagraphIndexByLineIndex(endpt.y);
			//自ドキュメントのテキストを取得し、LFへ変換
			AText	text;
			mText.GetText(start,end-start,text);
			text.ConvertLineEndToLF();
			//テンポラリファイルに保存
			AFileAcc	workingFile;
			workingFile.SpecifyChild(mDiffTempFolder,"workingfile");
			workingFile.CreateFile();
			workingFile.WriteFile(text);
			//引数に設定
			AText	workingFilePath;
			workingFile.GetPath(workingFilePath);
			argArray.Add(workingFilePath);
			//------------カレントディレクトリ------------
			AText	dirpath;
			mDiffTempFolder.GetNormalizedPath(dirpath);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//実行
	GetApp().SPI_GetChildProcessManager().ExecuteGeneralAsyncCommand(command,argArray,dirpath,
				GetObjectID(),kGeneralAsyncCommandType_Diff);
}

//#379
/**
非同期コマンド結果実行
*/
void	ADocument_Text::NVIDO_DoGeneralAsyncCommandResult( const AGeneralAsyncCommandType inType, const AText& inText )
{
	switch(inType)
	{
	  case kGeneralAsyncCommandType_Diff:
		{
			AText	resulttext;
			resulttext.SetText(inText);
			
			//CRへ変換（テキストエンコーディングはUTF8固定。（ExecuteDiff()で、UTF8/LFで比較用テンポラリファイルに書き込んでいるため。）））
			resulttext.ConvertReturnCodeToCR();
			
			//
			AArray<ADiffType>	diffTypeArray;
			AArray<AIndex>	startParagraphIndexArray;
			AArray<AIndex>	endParagraphIndexArray;
			AArray<AIndex>	oldStartParagraphIndexArray;
			AArray<AIndex>	oldEndParagraphIndexArray;
			ATextArray	deletedTextArray;
			
			//DiffText解析
			ParseDiffText(resulttext,diffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
						oldStartParagraphIndexArray,oldEndParagraphIndexArray,deletedTextArray);
			
			//#192
			//テキスト範囲比較の場合は、段落indexを補正
			if( mDiffTargetMode == kDiffTargetMode_TextWithRange )
			{
				AItemCount	itemCount = startParagraphIndexArray.GetItemCount();
				for( AIndex i = 0; i < itemCount; i++ )
				{
					startParagraphIndexArray.Set(i,startParagraphIndexArray.Get(i)+mDiffTextRangeStartParagraph);
					endParagraphIndexArray.Set(i,endParagraphIndexArray.Get(i)+mDiffTextRangeStartParagraph);
				}
			}
			
			//DiffArrayの設定処理を行う（#379 以前ここにあった処理をSPI_SetDiffArray()へ移動）
			SetDiffArray(diffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
						oldStartParagraphIndexArray,oldEndParagraphIndexArray,deletedTextArray,true);
			
			//反転Diff設定
			ADocumentID	targetDocID = kObjectID_Invalid;
			switch(mDiffTargetMode)
			{
				//
			  case kDiffTargetMode_RepositoryLatest:
				{
					targetDocID = mRepositoryDocumentID;
					break;
				}
			  case kDiffTargetMode_File:
			  case kDiffTargetMode_TextWithRange://#192
				{
					targetDocID = mDiffTargetFileDocumentID;
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			if( targetDocID != kObjectID_Invalid )
			{
				SetReverseDiffToDiffTargetDocument(targetDocID,diffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
												   oldStartParagraphIndexArray,oldEndParagraphIndexArray);
			}
			
			//各ウインドウのDiff表示を更新
			GetApp().SPI_UpdateDiffDisplay(GetObjectID());
			
			//Diff側のデータ作成後でないと、文字ごとDiffが作成できないので、もう一度TextViewを更新する。
			SPI_RefreshTextViews();
			
			//メニュー更新（イベントから来ていないのでここで実行する必要がある）
			AApplication::GetApplication().NVI_UpdateMenu();
			
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#379
/**
Diff状態テキスト取得（サブテキストの上のボタンに表示）
*/
void	ADocument_Text::SPI_GetDiffStatusText( AText& outText ) const
{
	switch(mCompareMode)
	{
		//
	  case kCompareMode_Repository:
		{
			outText.SetLocalizedText("SubPaneDiffButton_Repository");
			AText	diffCountText;
			diffCountText.SetNumber(SPI_GetDiffCount());
			outText.ReplaceParamText('0',diffCountText);
			break;
		}
	  case kCompareMode_File:
		{
			outText.SetLocalizedText("SubPaneDiffButton_File");
			AText	path;
			mDiffTargetFile.GetPath(path);
			outText.ReplaceParamText('0',path);
			AText	diffCountText;
			diffCountText.SetNumber(SPI_GetDiffCount());
			outText.ReplaceParamText('1',diffCountText);
			break;
		}
	  case kCompareMode_AutoBackupMinutely:
	  case kCompareMode_AutoBackupDaily:
	  case kCompareMode_AutoBackupHourly:
	  case kCompareMode_AutoBackupMonthly:
		{
			//自動バックアップとの比較の場合
			outText.SetLocalizedText("SubPaneDiffButton_AutoBackup");
			AText	filename;
			mDiffTargetFile.GetFilename(filename);
			AText	menutext;
			GetAutoBackupCompareMenuText(filename.GetNumber64bit(),menutext);
			outText.ReplaceParamText('0',menutext);
			AText	diffCountText;
			diffCountText.SetNumber(SPI_GetDiffCount());
			outText.ReplaceParamText('1',diffCountText);
			break;
		}
	  case kCompareMode_Clipboard:
		{
			//クリップボードとの比較の場合
			outText.SetLocalizedText("SubPaneDiffButton_Clipboard");
			AText	diffCountText;
			diffCountText.SetNumber(SPI_GetDiffCount());
			outText.ReplaceParamText('0',diffCountText);
			break;
		}
	  case kCompareMode_UnsavedData:
		{
			//未保存データとの比較の場合
			outText.SetLocalizedText("SubPaneDiffButton_UnsavedData");
			AText	diffCountText;
			diffCountText.SetNumber(SPI_GetDiffCount());
			outText.ReplaceParamText('0',diffCountText);
			break;
		}
	  case kCompareMode_NoCompare:
		{
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#379
/**
各モードごとのDiff可否取得
*/
/*#81
ABool	ADocument_Text::SPI_GetDiffAvailability( const ADiffTargetMode inDiffTargetMode, AFileAcc& outFile ) const
{
	ABool	result = false;
	switch(inDiffTargetMode)
	{
		//
	  case kDiffTargetMode_RepositoryLatest:
		{
			if( NVI_IsFileSpecified() == true )
			{
				AFileAcc	file;
				NVI_GetFile(file);
				//#568 if( GetApp().SPI_GetSCMFolderManager().GetFileState(file) != kSCMFileState_NotSCMFolder )
				if( mSCMFileState != kSCMFileState_NotSCMFolder && mSCMFileState != kSCMFileState_NotEntried )//#568
				{
					result = true;
				}
			}
			break;
		}
	  case kDiffTargetMode_File:
		{
			AText	path;
			if( NVI_IsFileSpecified() == true )
			{
				AFileAcc	file;
				NVI_GetFile(file);
				file.GetPath(path);
				//Diff比較先フォルダ設定取得
				AText	diffSourceFolderWorkingPath;
				GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffSourceFolder_Working,diffSourceFolderWorkingPath);
				//このドキュメントのファイルがDiff比較先フォルダ内のファイルかどうかを判定
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDiffSourceFolder_Enable) == true &&
							diffSourceFolderWorkingPath.GetItemCount() > 0 && path.GetItemCount() > 0 && 
							path.GetItemCount() > diffSourceFolderWorkingPath.GetItemCount() && 
							diffSourceFolderWorkingPath.CompareMin(path) == true )
				{
					//相対パスを取得
					AText	relativePath;
					relativePath.InsertText(0,path,diffSourceFolderWorkingPath.GetItemCount(),
							path.GetItemCount()-diffSourceFolderWorkingPath.GetItemCount());
					//Diff比較元フォルダ取得
					AText	diffSourceFolderPath;
					GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffSourceFolder_Source,diffSourceFolderPath);
					AFileAcc	diffSourceFolder;
					diffSourceFolder.Specify(diffSourceFolderPath);
					//Diff比較元ファイル取得
					outFile.SpecifyChild(diffSourceFolder,relativePath);
					//ファイルが存在しているかどうか判定
					if( outFile.Exist() == true )
					{
						result = true;
					}
				}
			}
			break;
		}
	}
	return result;
}
*/
//#379
/**
Diff対象ファイルを更新（Diff設定ウインドウでフォルダ設定更新時）
*/
/*#81
ABool	ADocument_Text::SPI_UpdateDiffFileTarget()
{
	ABool	result = false;
	AFileAcc	diffFile;
	if( SPI_GetDiffAvailability(kDiffTargetMode_File,diffFile) == true )
	{
		//ファイル比較モード
		mDiffTargetMode = kDiffTargetMode_File;
		//Diff対象ファイル設定
		SPI_SetDiffTargetFile(diffFile);
		//
		result = true;
	}
	else if( SPI_GetDiffAvailability(kDiffTargetMode_RepositoryLatest,diffFile) == true )
	{
		//レポジトリ比較モード
		mDiffTargetMode = kDiffTargetMode_RepositoryLatest;
		//Diffデータ更新
		SPI_UpdateDiffDisplayData();
		//
		result = true;
	}
	//
	else
	{
		ClearDiffDisplay();
		//
		mDiffTargetMode = kDiffTargetMode_None;
	}
	return result;
}
*/
/*
UnifiedDiff形式
Index: AApp.cpp
===================================================================
--- AApp.cpp	(revision 2247)
+++ AApp.cpp	(working copy)
@@ -5632,7 +5649,7 @@
 	SPI_GetMultiFileFindWindow().NVI_SwitchFocusToFirst();
 }
 
-//「最近使った?」に追加
+//「最近使った?」に追加
 void	AApp::SPI_AddRecentlyUsed( const AText& inFindText, const AFileAcc& inFolder )
 {
 	SPI_GetMultiFileFindWindow().SPI_AddRecentlyUsedFindText(inFindText);

旧diff形式
Index: AView_EditBox.cpp
===================================================================
70a71
> 	GetTextDocument().NVI_RegisterView(GetObjectID());
141c142,143
< 	AApplication::GetApplication().NVI_DeleteDocument(mDocumentID);
---
> 	//#379 AApplication::GetApplication().NVI_DeleteDocument(mDocumentID);
> 	GetTextDocument().NVI_UnregisterView(GetObjectID());
*/
//#379
/**
*/
void	ADocument_Text::ParseDiffText( const AText& inText,
		AArray<ADiffType>& diffTypeArray,
		AArray<AIndex>& startParagraphIndexArray, AArray<AIndex>& endParagraphIndexArray,
		AArray<AIndex>& oldStartParagraphIndexArray, AArray<AIndex>& oldEndParagraphIndexArray,
		ATextArray& deletedTextArray )
{
	//#379 差分テキスト内容で形式判断するように変更if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseExternalDiffForSubversion) == false )//#335
	ABool	unifiedDiffFormat = false;
	AIndex	pos = 0;
	AText	line;
	AItemCount	lineCount = 0;
	for( ; pos < inText.GetItemCount(); )
	{
		inText.GetLine(pos,line);
		if( line.GetItemCount() == 0 )   continue;
		if( line.Get(0) == '@' )
		{
			//@で始まる行があればUnifiedDiff形式とみなす
			unifiedDiffFormat = true;
			break;
		}
		//最初から16行目までをチェック
		lineCount++;
		if( lineCount > 16 )   break;
	}
	//形式別に比較内容取得
	if( unifiedDiffFormat == true )
	{
		//Unified Diff形式 #335
		
		//@で始まる最初の行を検索
		AIndex	pos = 0;
		AText	line;
		for( ; pos < inText.GetItemCount(); )
		{
			inText.GetLine(pos,line);
			if( line.GetItemCount() == 0 )   continue;
			if( line.Get(0) == '@' )   break;
		}
		//各変更点ごとのループ
		for( ; pos < inText.GetItemCount(); )
		{
			//ここにきたときには、かならず@@行をlineに読み込んだ時点（@@行の次の行にposがある）
			AIndex	new_startlineindex = 0;//変更後側の開始行
			AIndex	new_linecount = 1;//変更後側の行数
			AIndex	old_startlineindex = 0;//変更前側の開始行 #379
			AIndex	old_linecount = 1;//変更前側の行数 #379
			//@@行の解析
			//例：@@ -5632,7 +5649,7 @@
			for( AIndex linepos = 1; linepos < line.GetItemCount(); linepos++ )
			{
				AUChar	ch = line.Get(linepos);
				//変更前側開始行・行数取得 #379
				if( ch == '-' )
				{
					linepos++;
					if( line.ParseIntegerNumber(linepos,old_startlineindex,false) == false )   {_ACError("",NULL);return;}
					old_startlineindex--;//行番号は1はじまりなので、0はじまりにする
					if( old_startlineindex < 0 )   old_startlineindex = 0;//空ドキュメントだと0,0になるため。
					if( line.Get(linepos) == ',' )
					{
						linepos++;
						line.ParseIntegerNumber(linepos,old_linecount,false);
					}
				}
				//変更後側開始行・行数取得
				if( ch == '+' )
				{
					linepos++;
					if( line.ParseIntegerNumber(linepos,new_startlineindex,false) == false )   {_ACError("",NULL);return;}
					new_startlineindex--;//行番号は1はじまりなので、0はじまりにする
					if( new_startlineindex < 0 )   new_startlineindex = 0;//空ドキュメントだと0,0になるため。
					if( line.Get(linepos) == ',' )
					{
						linepos++;
						line.ParseIntegerNumber(linepos,new_linecount,false);
					}
					break;
				}
			}
			//変更内容部分の解析
			//+のブロック、-のブロックから移行するときに、差分行格納処理を行う
			AIndex	new_lineindex = new_startlineindex;//working側の現在行を格納
			AIndex	old_lineindex = old_startlineindex;//#379 repos側の現在行を格納
			AItemCount	pluscount = 0;
			AItemCount	minuscount = 0;
			AText	deletedText;//#379
			for( ; pos < inText.GetItemCount(); )
			{
				//1行取得
				inText.GetLine(pos,line);
                if( line.GetItemCount() == 0 )   {_ACError("",NULL);return;}
				AUChar	ch = line.Get(0);
				//@行ならこの変更内容部分ループ終了
				if( ch == '@' )   break;
				//+ブロック終了
				if( ch != '+' && pluscount > 0 )
				{
					if( minuscount > 0 )
					{
						//-ブロックあり、+ブロック終了なので、「変更」
						diffTypeArray.Add(kDiffType_Changed);
						startParagraphIndexArray.Add(new_lineindex-pluscount);
						endParagraphIndexArray.Add(new_lineindex);
						//#379 レポジトリ側のStart,End段落設定
						oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
						oldEndParagraphIndexArray.Add(old_lineindex);
						//削除テキスト追加
						deletedTextArray.Add(deletedText);//#379
					}
					else
					{
						//-ブロックなし、+ブロック終了なので、「追加」
						diffTypeArray.Add(kDiffType_Added);
						startParagraphIndexArray.Add(new_lineindex-pluscount);
						endParagraphIndexArray.Add(new_lineindex);
						//#379 レポジトリ側のStart,End段落設定
						oldStartParagraphIndexArray.Add(old_lineindex);
						oldEndParagraphIndexArray.Add(old_lineindex+1);
						//削除テキスト追加
						deletedTextArray.Add(GetEmptyText());//#379
					}
					pluscount = 0;
					minuscount = 0;
					deletedText.DeleteAll();
				}
				//-ブロック終了（ただし、次が+の場合は、changedなので何もしない。
				if( ch == ' ' && minuscount > 0 )
				{
					//-ブロック終了→変更なしブロックなので、「削除」
					diffTypeArray.Add(kDiffType_Deleted);
					startParagraphIndexArray.Add(new_lineindex);
					endParagraphIndexArray.Add(new_lineindex+1);
					//#379 レポジトリ側のStart,End段落設定
					oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
					oldEndParagraphIndexArray.Add(old_lineindex);
					//削除テキスト追加
					deletedTextArray.Add(deletedText);//#379
					minuscount = 0;
					deletedText.DeleteAll();
				}
				//削除テキスト記憶 #379
				if( ch == '-' )
				{
					AText	text;
					text.SetText(line);
					text.Delete(0,1);
					text.Add(kUChar_LineEnd);
					deletedText.AddText(text);
				}
				//+, -カウント、行数インクリメント
				switch(ch)
				{
				  case '+':
					{
						pluscount++;
						new_lineindex++;
						break;
					}
				  case '-':
					{
						minuscount++;
						old_lineindex++;//#379
						break;
					}
				  case ' ':
					{
						new_lineindex++;
						old_lineindex++;//#379
						break;
					}
				  default:
					{
						//★とりあえずコメントアウトしておく。ここに来る場合の対策必要。（ドキュメント最後で発生することがある？）_ACError("",NULL);
						break;
					}
				}
			}
			//変更内容部分ループ終了処理
			//+, -のカウント残りがあれば処理する
			if( pluscount > 0 )
			{
				if( minuscount > 0 )
				{
					//-ブロックあり、+ブロック終了なので、「変更」
					diffTypeArray.Add(kDiffType_Changed);
					startParagraphIndexArray.Add(new_lineindex-pluscount);
					endParagraphIndexArray.Add(new_lineindex);
					//#379 レポジトリ側のStart,End段落設定
					oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
					oldEndParagraphIndexArray.Add(old_lineindex);
					//削除テキスト追加
					deletedTextArray.Add(deletedText);//#379
				}
				else
				{
					//-ブロックなし、+ブロック終了なので、「追加」
					diffTypeArray.Add(kDiffType_Added);
					startParagraphIndexArray.Add(new_lineindex-pluscount);
					endParagraphIndexArray.Add(new_lineindex);
					//#379 レポジトリ側のStart,End段落設定
					oldStartParagraphIndexArray.Add(old_lineindex);
					oldEndParagraphIndexArray.Add(old_lineindex+1);
					//削除テキスト追加
					deletedTextArray.Add(GetEmptyText());//#379
				}
				pluscount = 0;
				minuscount = 0;
			}
			if( minuscount > 0 )
			{
				//-ブロックなし終了なので、「削除」
				diffTypeArray.Add(kDiffType_Deleted);
				startParagraphIndexArray.Add(new_lineindex);
				endParagraphIndexArray.Add(new_lineindex+1);
				//#379 レポジトリ側のStart,End段落設定
				oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
				oldEndParagraphIndexArray.Add(old_lineindex);
				//削除テキスト追加
				deletedTextArray.Add(deletedText);//#379
				minuscount = 0;
			}
		}
	}
	else
	{
		//旧diff形式
		for( AIndex pos = 0; pos < inText.GetItemCount(); )
		{
			AText	line;
			inText.GetLine(pos,line);
			if( line.GetItemCount() == 0 )   continue;
			AUChar	ch = line.Get(0);
			if( ch >= '0' && ch <= '9' )
			{
				AIndex	linepos = 0;
				ANumber	oldsnum = 0, oldenum = 0;
				if( line.ParseIntegerNumber(linepos,oldsnum,false) == false )   continue;
				if( linepos >= line.GetItemCount() )   continue;
				ch = line.Get(linepos);
				linepos++;
				if( ch == ',' )
				{
					if( line.ParseIntegerNumber(linepos,oldenum,false) == false )   continue;
					if( linepos >= line.GetItemCount() )   continue;
					ch = line.Get(linepos);
					linepos++;
				}
				if( oldenum == 0 )   oldenum = oldsnum;//#379
				//
				switch(ch)
				{
				  case 'a':
					{
						ANumber	newsnum = 0, newenum = 0;
						if( line.ParseIntegerNumber(linepos,newsnum,false) == false )   continue;
						if( linepos < line.GetItemCount() )
						{
							if( line.Get(linepos) == ',' )
							{
								linepos++;
								line.ParseIntegerNumber(linepos,newenum,false);
							}
						}
						if( newenum == 0 )   newenum = newsnum;
						
						diffTypeArray.Add(kDiffType_Added);
						startParagraphIndexArray.Add(newsnum-1);
						endParagraphIndexArray.Add(newenum+1-1);
						//#379 レポジトリ側のStart,End段落設定
						oldStartParagraphIndexArray.Add(oldsnum-1+1);
						oldEndParagraphIndexArray.Add(oldenum+1-1+1);
						//#379 追加行なので削除テキストはない
						deletedTextArray.Add(GetEmptyText());
						break;
					}
				  case 'c':
					{
						ANumber	newsnum = 0, newenum = 0;
						if( line.ParseIntegerNumber(linepos,newsnum,false) == false )   continue;
						if( linepos < line.GetItemCount() )
						{
							if( line.Get(linepos) == ',' )
							{
								linepos++;
								line.ParseIntegerNumber(linepos,newenum,false);
							}
						}
						if( newenum == 0 )   newenum = newsnum;
						
						diffTypeArray.Add(kDiffType_Changed);
						startParagraphIndexArray.Add(newsnum-1);
						endParagraphIndexArray.Add(newenum+1-1);
						//#379 レポジトリ側のStart,End段落設定
						oldStartParagraphIndexArray.Add(oldsnum-1);
						oldEndParagraphIndexArray.Add(oldenum+1-1);
						//#379 削除テキスト取得
						AText	deletedText;
						for( AIndex p = pos; p < inText.GetItemCount(); )
						{
							AText	l;
							inText.GetLine(p,l);
							if( l.GetItemCount() < 2 )   continue;
							if( l.Get(0) != '<' )   break;
							l.Delete(0,2);
							deletedText.AddText(l);
							deletedText.Add(kUChar_LineEnd);
						}
						deletedTextArray.Add(deletedText);
						break;
					}
				  case 'd':
					{
						ANumber	newsnum = 0, newenum = 0;
						if( line.ParseIntegerNumber(linepos,newsnum,false) == false )   continue;
						if( linepos < line.GetItemCount() )
						{
							if( line.Get(linepos) == ',' )
							{
								linepos++;
								line.ParseIntegerNumber(linepos,newenum,false);
							}
						}
						if( newenum == 0 )   newenum = newsnum;
						
						diffTypeArray.Add(kDiffType_Deleted);
						//'d'の場合、表示された段落番号の後に削除行があることを意味する。
						//newsnum==0もありうる。'a'や'c'と同じくnewsnum-1を格納してしまうと、-1が格納されて、処理の都合が悪い。
						//よって、格納された段落番号の前に削除行があることを意味するようにする。
						startParagraphIndexArray.Add(newsnum-1+1);
						endParagraphIndexArray.Add(newenum+1-1+1);
						//#379 レポジトリ側のStart,End段落設定
						oldStartParagraphIndexArray.Add(oldsnum-1);
						oldEndParagraphIndexArray.Add(oldenum+1-1);
						//#379 削除テキスト取得
						AText	deletedText;
						for( AIndex p = pos; p < inText.GetItemCount(); )
						{
							AText	l;
							inText.GetLine(p,l);
							if( l.GetItemCount() < 2 )   continue;
							if( l.Get(0) != '<' )   break;
							l.Delete(0,2);
							deletedText.AddText(l);
							deletedText.Add(kUChar_LineEnd);
						}
						deletedTextArray.Add(deletedText);
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

//#732
/**
*/
/*#drop
void	ADocument_Text::GetDiffDataFor2Texts( const AFileAcc& inDiffTempFolder,
		const AText& inText1, const AText& inText2, 
		AArray<ADiffType>& diffTypeArray,
		AArray<AIndex>& startParagraphIndexArray, AArray<AIndex>& endParagraphIndexArray,
		AArray<AIndex>& oldStartParagraphIndexArray, AArray<AIndex>& oldEndParagraphIndexArray,
		ATextArray& deletedTextArray )
{
	//
	diffTypeArray.DeleteAll();
	startParagraphIndexArray.DeleteAll();
	endParagraphIndexArray.DeleteAll();
	oldStartParagraphIndexArray.DeleteAll();
	oldEndParagraphIndexArray.DeleteAll();
	deletedTextArray.DeleteAll();
	//
	AText	text1;
	inText1.ConvertReturnCodeFromCR(returnCode_LF,text1);
	AFileAcc	file1;
	file1.SpecifyChild(inDiffTempFolder,"text1");
	file1.CreateFile();
	file1.WriteFile(text1);
	AText	file1path;
	file1.GetPath(file1path);
	//
	AText	text2;
	inText2.ConvertReturnCodeFromCR(returnCode_LF,text2);
	AFileAcc	file2;
	file2.SpecifyChild(inDiffTempFolder,"text2");
	file2.CreateFile();
	file2.WriteFile(text2);
	AText	file2path;
	file2.GetPath(file2path);
	
	//
	//Diffパス・オプション取得
	AText	diffExeFilePath;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffPath,diffExeFilePath);
	AText	diffOption;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kDiffOption,diffOption);
	
#if IMPLEMENTATION_FOR_WINDOWS
	//★暫定 Windows版はDiffパス固定（アプリに付属のdiff.exeを実行）
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	AFileAcc	diffExeFile;
	diffExeFile.SpecifySister(appFile,"diff\\diff.exe");
	diffExeFile.GetPath(diffExeFilePath);
#endif
	
	//diffコマンドパス
	AText	command;
	command.SetText(diffExeFilePath);
	//引数
	ATextArray	argArray;
	argArray.Add(command);
	//Unified Diff設定
	AText	t;
	t.SetCstring("-u");
	argArray.Add(t);
	//Diffオプション
	if( diffOption.GetItemCount() > 0 )
	{
		argArray.Add(diffOption);
	}
	//
	//file1path.Insert1(0,'\"');
	//file1path.Add('\"');
	argArray.Add(file1path);
	//
	//file2path.Insert1(0,'\"');
	//file2path.Add('\"');
	argArray.Add(file2path);
	
	//
	AText	tmpFolderPath;
	inDiffTempFolder.GetPath(tmpFolderPath);
	//実行
	AText	resulttext;
	GetApp().SPI_GetChildProcessManager().ExecuteGeneralSyncCommand(command,argArray,tmpFolderPath,resulttext);
	//
	ParseDiffText(resulttext,diffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
				oldStartParagraphIndexArray,oldEndParagraphIndexArray,deletedTextArray);
	resulttext.OutputToStderr();
}
*/

//#379
/**
反転Diffを指定ドキュメントに設定
*/
void	ADocument_Text::SetReverseDiffToDiffTargetDocument( const ADocumentID inDiffTargetDocumentID,
		const AArray<ADiffType>& inDiffTypeArray,
		const AArray<AIndex>& inStartParagraphIndexArray, const AArray<AIndex>& inEndParagraphIndexArray,
		const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray )
{
	//レポジトリ表示用ドキュメント用のdiffデータ（反転diff）作成
	AArray<ADiffType>	reverseDiffArray_DiffType;
	ATextArray	reverseDiffArray_DeletedText;
	for( AIndex i = 0; i < inDiffTypeArray.GetItemCount(); i++ )
	{
		switch(inDiffTypeArray.Get(i))
		{
			//追加の場合
		  case kDiffType_Added:
			{
				reverseDiffArray_DiffType.Add(kDiffType_Deleted);
				reverseDiffArray_DeletedText.Add(GetEmptyText());
				break;
			}
			//削除の場合（reposTextへは削除されたテキストを追加）
		  case kDiffType_Deleted:
			{
				reverseDiffArray_DiffType.Add(kDiffType_Added);
				reverseDiffArray_DeletedText.Add(GetEmptyText());
				break;
			}
			//変更の場合（reposTextへは削除されたテキストを追加）
		  case kDiffType_Changed:
			{
				reverseDiffArray_DiffType.Add(kDiffType_Changed);
				reverseDiffArray_DeletedText.Add(GetEmptyText());
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	//レポジトリ表示用ドキュメントに反転diff設定
	GetApp().SPI_GetTextDocumentByID(inDiffTargetDocumentID).SetDiffArray(reverseDiffArray_DiffType,
		inOldStartParagraphIndexArray,inOldEndParagraphIndexArray,
				inStartParagraphIndexArray,inEndParagraphIndexArray,reverseDiffArray_DeletedText,false);//#379
	GetApp().SPI_GetTextDocumentByID(inDiffTargetDocumentID).SPI_RefreshTextViews();//#379
}

//#379
/**
Diff対象側のドキュメントが閉じられたときにコールされる
*/
void	ADocument_Text::SPI_DoDiffTargetDocumentClosed( const ADocumentID inDiffTargetDocumentID )
{
	if( mDiffTargetFileDocumentID == inDiffTargetDocumentID )
	{
		mDiffTargetFileDocumentID = kObjectID_Invalid;
	}
	else if( mRepositoryDocumentID == inDiffTargetDocumentID )
	{
		mRepositoryDocumentID = kObjectID_Invalid;
		//レポジトリテキストファイルを削除
		mRepositoryTempFile.DeleteFile();
	}
	else
	{
		_ACError("",this);
	}
}

//#379
/**
Woking側のドキュメントが閉じられたときにコールされる
*/
void	ADocument_Text::SPI_DoWorkingDocumentClosed( const ADocumentID inWorkingDocumentID )
{
	if( mDiffWorkingDocumentID == inWorkingDocumentID )
	{
		mDiffWorkingDocumentID = kObjectID_Invalid;
	}
	else
	{
		_ACError("",this);
	}
}

#pragma mark ===========================

#pragma mark ---SCM

/**
SCM比較コマンド実行完了時の処理
*/
void	ADocument_Text::SPI_DoSCMCompareResult(/*#379 const AArray<ADiffType>& inDiffTypeArray, 
		const AArray<AIndex>& inStartParagraphIndexArray, const AArray<AIndex>& inEndParagraphIndexArray,
		const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray,//#379
		const ATextArray& inDeletedText*/ const AText& inDiffText )
{
	if( mDiffDisplayWaitFlag == false )   return;
	mDiffDisplayWaitFlag = false;
	
	//#379 diff結果テキスト解析、結果を配列に格納
	AArray<ADiffType>	diffTypeArray;
	AArray<AIndex>	startParagraphIndexArray;
	AArray<AIndex>	endParagraphIndexArray;
	AArray<AIndex>	oldStartParagraphIndexArray;
	AArray<AIndex>	oldEndParagraphIndexArray;
	ATextArray	deletedTextArray;
	ParseDiffText(inDiffText,diffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
			oldStartParagraphIndexArray,oldEndParagraphIndexArray,deletedTextArray);
	
	//==================レポジトリテキスト生成==================
	
	AText	reposText;
	//★reposTextをUTF8にする処理を確認する。
	//★ここは、CompareWithLatest()でのsvn diff実行に対する結果処理。svn diffでないほうがいいかもしれない。
	
	//本ドキュメントの段落開始位置リストを取得
	AArray<AIndex>	paraStartIndex;
	for( AIndex lineIndex = 0; lineIndex < mTextInfo.GetLineCount();  )
	{
		paraStartIndex.Add(mTextInfo.GetLineStart(lineIndex));
		while( lineIndex < mTextInfo.GetLineCount() )
		{
			if( mTextInfo.GetLineExistLineEnd(lineIndex) == true )   break;
			lineIndex++;
		}
		lineIndex++;
	}
	
	//本ドキュメント内の次の開始段落index
	AIndex	nextStartParagraphIndex = 0;
	//
	for( AIndex i = 0; i < diffTypeArray.GetItemCount(); i++ )
	{
		//reposTextへテキスト追加
		//diff箇所の前の部分の開始位置取得
		if( nextStartParagraphIndex >= paraStartIndex.GetItemCount() )
		{
			//nextStartParagraphIndexが上で取得した段落開始位置リスト以降の場合は、最後の段落にする
			//★UTF-16の場合等、正常に段落取得できなかった場合の補正処理。
			nextStartParagraphIndex = paraStartIndex.GetItemCount() -1;
		}
		AIndex	spos = paraStartIndex.Get(nextStartParagraphIndex);
		//diff箇所の前の部分の終了位置取得
		AIndex	startParagraphIndex = startParagraphIndexArray.Get(i);
		if( startParagraphIndex >= paraStartIndex.GetItemCount() )
		{
			//startParagraphIndexが上で取得した段落開始位置リスト以降の場合は、最後の段落にする
			//★UTF-16の場合等、正常に段落取得できなかった場合の補正処理。
			startParagraphIndex = paraStartIndex.GetItemCount()-1;
		}
		AIndex	epos = paraStartIndex.Get(startParagraphIndex);
		//
		reposText.InsertText(reposText.GetItemCount(),mText,spos,epos-spos);
		//次の開始段落indexを設定
		if( diffTypeArray.Get(i) == kDiffType_Deleted )
		{
			nextStartParagraphIndex = startParagraphIndexArray.Get(i);
		}
		else
		{
			nextStartParagraphIndex = endParagraphIndexArray.Get(i);
		}
		//削除テキストを取得してUTF8へ変換
		AText	deletedTextUTF8;
		deletedTextUTF8.SetText(deletedTextArray.GetTextConst(i));
		deletedTextUTF8.ConvertToUTF8(SPI_GetTextEncoding());
		//reposTextへ削除テキストを追加
		switch(diffTypeArray.Get(i))
		{
			//追加の場合
		  case kDiffType_Added:
			{
				//処理無し
				break;
			}
			//削除の場合（reposTextへは削除されたテキストを追加）
		  case kDiffType_Deleted:
			{
				//reposTextへテキスト追加（削除されたテキストの追加）
				reposText.AddText(deletedTextUTF8);
				break;
			}
			//変更の場合（reposTextへは削除されたテキストを追加）
		  case kDiffType_Changed:
			{
				//reposTextへテキスト追加（削除されたテキストの追加）
				reposText.AddText(deletedTextUTF8);
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	//最後の部分のテキスト追加
	if( nextStartParagraphIndex < paraStartIndex.GetItemCount() )
	{
		AIndex	spos = paraStartIndex.Get(nextStartParagraphIndex);
		AIndex	epos = mText.GetItemCount();
		reposText.InsertText(reposText.GetItemCount(),mText,spos,epos-spos);
	}
	
	//レポジトリ表示用のドキュメントを生成
	if( mRepositoryDocumentID == kObjectID_Invalid )
	{
		//ドキュメント生成
		AText	title;
		NVI_GetTitle(title);
		title.AddCstring(":Repository");
		AText	tecName;
		//SPI_GetTextEncoding(tecName);
		ATextEncodingWrapper::GetTextEncodingName(ATextEncodingWrapper::GetUTF8TextEncoding(),tecName);
		//ウインドウ生成せずにドキュメント生成
		ATextDocumentFactory	factory(this,kObjectID_Invalid);
		mRepositoryDocumentID = GetApp().SPNVI_CreateDiffTargetDocument(title,reposText,mModeIndex,tecName,
					kIndex_Invalid,kIndex_Invalid,false,false,SPI_GetFirstWindowID());
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).mDiffWorkingDocumentID = GetObjectID();
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_SetOwnedDocument();
		//View activate時の処理（行計算実行等）#699 diff targetドキュメントはコミット時までviewが一度もactiveにならない可能性があるので、ここで処理する
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_DoViewActivating(false);
		
		//レポジトリテキスト格納用ファイル生成（Diff実行用）
		AFileAcc	appPrefFolder;
		AFileWrapper::GetAppPrefFolder(appPrefFolder);
		AFileAcc	tmpFolder;
		//#427 tmpFolder.SpecifyChild(appPrefFolder,"temp");
		//#427 tmpFolder.CreateFolder();
		GetApp().SPI_GetTempFolder(tmpFolder);//#427
		mRepositoryTempFile.SpecifyUniqChildFile(tmpFolder,"temprepos");
		mRepositoryTempFile.CreateFile();
	}
	else
	{
		//全削除
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_DeleteText(0,
				GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_GetTextLength());
		//テキスト挿入
		GetApp().SPI_GetTextDocumentByID(mRepositoryDocumentID).SPI_InsertText(0,reposText);
	}
	
	//レポジトリテキストをファイル保存（Diff実行用）
	AText	tmpText;
	reposText.ConvertFromUTF8CR(SPI_GetTextEncoding(),SPI_GetReturnCode(),tmpText);
	mRepositoryTempFile.WriteFile(tmpText);
	
	//ファイル化したレポジトリテキストとのDiff実行
	ExecuteDiff();
}

void	ADocument_Text::SPI_GetNextDiffLineIndex( const AIndex inLineIndex, AIndex& outStart, AIndex& outEnd ) const//#512
{
	//#379
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	//#512
	outStart = outEnd = kIndex_Invalid;
	//
	for( AIndex i = 0; i < mDiffDisplayArray_StartParagraphIndex.GetItemCount(); i++ )
	{
		if( /*#379 inLineIndex*/paraIndex < mDiffDisplayArray_StartParagraphIndex.Get(i) )
		{
			outStart = SPI_GetLineIndexByParagraphIndex(mDiffDisplayArray_StartParagraphIndex.Get(i));//#512
			outEnd = SPI_GetLineIndexByParagraphIndex(mDiffDisplayArray_EndParagraphIndex.Get(i));//#512
			//レポジトリと比較して最終行（空行）が削除され、最終行が改行のみではない場合、outStart/outEndは最終行の後になる。
			//最終行の後は選択できないので、最終行に補正する。#0
			if( outStart >= SPI_GetLineCount() )   outStart = SPI_GetLineCount()-1;
			if( outEnd >= SPI_GetLineCount() )   outEnd = SPI_GetLineCount()-1;
			return;//#512
		}
	}
}

void	ADocument_Text::SPI_GetPrevDiffLineIndex( const AIndex inLineIndex, AIndex& outStart, AIndex& outEnd ) const//#512
{
	//#379
	AIndex	paraIndex = SPI_GetParagraphIndexByLineIndex(inLineIndex);
	//#512
	outStart = outEnd = kIndex_Invalid;
	//
	for( AIndex i = mDiffDisplayArray_StartParagraphIndex.GetItemCount()-1; i >= 0; i-- )
	{
		if( /*#379 inLineIndex*/paraIndex >= mDiffDisplayArray_EndParagraphIndex.Get(i) )
		{
			outStart = SPI_GetLineIndexByParagraphIndex(mDiffDisplayArray_StartParagraphIndex.Get(i));//#512
			outEnd = SPI_GetLineIndexByParagraphIndex(mDiffDisplayArray_EndParagraphIndex.Get(i));//#512
			//レポジトリと比較して最終行（空行）が削除され、最終行が改行のみではない場合、outStart/outEndは最終行の後になる。
			//最終行の後は選択できないので、最終行に補正する。#0
			if( outStart >= SPI_GetLineCount() )   outStart = SPI_GetLineCount()-1;
			if( outEnd >= SPI_GetLineCount() )   outEnd = SPI_GetLineCount()-1;
			return;//#512
		}
	}
}

void	ADocument_Text::ClearDiffDisplay()
{
	if( mTextDocumentType == kTextDocumentType_DiffTarget )
	{
		//#0
		//diff target側のdiff display dataは、working側のSetReverseDiffToDiffTargetDocument()からのみ設定する
		//（モード設定のフォント変更時などで、ここがコールされるが、ここでクリアすると、先にworking documentのほうのdiff結果取得完了していた場合、
		//既にdiff target側の結果が格納されているのを消去することになってしまう。）
		//★上記括弧内の場合、途中でdiff情報は存在するが、行情報が存在しないタイミングが存在しうる。その場合に、throwが発生しないようにしたい。
		return;
	}
	
	mDiffDisplayWaitFlag = false;
	//#379 mDiffDisplayMode = kDiffDisplayMode_None;
	if( mDiffDisplayArray_DiffType.GetItemCount() == 0 )   return;
	mDiffDisplayArray_DiffType.DeleteAll();
	mDiffDisplayArray_StartParagraphIndex.DeleteAll();
	mDiffDisplayArray_EndParagraphIndex.DeleteAll();
	mDiffDisplayArray_DeletedText.DeleteAll();//#379
	mDiffParagraphIndexArray.DeleteAll();//#379
	//#212 RefreshWindow();
	SPI_RefreshTextViews();//#212
	
	//色取得
	AColor	subWindowNormalColor = kColor_List_Normal;//#1316 GetApp().SPI_GetSubWindowLetterColor();
	//
	for( AIndex i = 0; i < mJump_MenuColorArray.GetItemCount(); i++ )
	{
		mJump_MenuColorArray.Set(i,subWindowNormalColor);
	}
	SPI_UpdateJumpList();//#695 true);//#291
}

/**
Repositoryのデータ更新時の処理
*/
void	ADocument_Text::SPI_DoRepositoryUpdated()
{
	/*#379
	switch(mDiffDisplayMode)
	{
	  case kDiffDisplayMode_RepositoryLatest:
		{
			SPI_CompareWithRepository();
			break;
		}
	  default:
		{
			//処理無し
			break;
		}
		
	}
	*/
	//#379 レポジトリテキスト＋Diffデータの更新を行う
	SPI_UpdateDiffDisplayData(true);
}

//#455
/**
コミット機能利用可能かどうかを返す
*/
ABool	ADocument_Text::SPI_CanCommit() const
{
	ABool	canCommit = false;
	AFileAcc	file;
	NVI_GetFile(file);
	ASCMFolderType	scmFolderType = GetApp().SPI_GetSCMFolderManager().GetSCMFolderType(file);//#589
	if( scmFolderType == kSCMFolderType_SVN || scmFolderType == kSCMFolderType_Git )//#589
	{
		switch( /*#568 GetApp().SPI_GetSCMFolderManager().GetFileState(file)*/mSCMFileState )
		{
		  case kSCMFileState_Modified:
		  case kSCMFileState_Added:
			{
				canCommit = true;
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	return canCommit;
}

//#379
/**
SCMのstatus updateコマンド実行完了後にコールされる（ドキュメントのDiffモード更新のため）
*/
void	ADocument_Text::SPI_DoSCMStatusUpdated( const AFileAcc& inFolder )
{
	//#568 mSCMFileStateを更新
	AFileAcc	file;
	NVI_GetFile(file);
	mSCMFileState = GetApp().SPI_GetSCMFolderManager().GetFileState(file);
	//status updateコマンドの結果の方が、ドキュメント初期状態遷移完了よりも遅かった場合、
	//ドキュメント初期状態遷移完了時にSCM対象かどうかが判別できないので、レポジトリ比較モードにできない。
	//そのため、ここで比較モード無し、かつ、ドキュメント初期状態遷移完了済みであれば、再度レポジトリ比較モードにするかどうかの判定を行う。
	if( mDiffTargetMode == kDiffTargetMode_None && //#699 mViewDataInited == true )//#664 高速化のためまだViewData初期化していない段階（タブをまだ開いていない）ではレポジトリ取得しないようにする
	   SPI_GetDocumentInitState() == kDocumentInitState_Completed )//#699
	{
		/*#81
		AFileAcc	diffFile;
		if( SPI_GetDiffAvailability(kDiffTargetMode_RepositoryLatest,diffFile) == true )
		{
			//レポジトリ比較モード
			mDiffTargetMode = kDiffTargetMode_RepositoryLatest;
			//Diffデータ更新
			SPI_UpdateDiffDisplayData();
		}
		*/
		//レポジトリ比較可能であれば、レポジトリ比較モードにする
		if( SPI_CanDiffWithRepositiory() == true )
		{
			SPI_SetDiffMode_Repository();
		}
	}
	
	//#688
	//タイトルバー更新（infoheader廃止のため、タイトルバーにsvn情報表示）
	GetApp().NVI_UpdateTitleBar();
}

#pragma mark ===========================

#pragma mark ---他アプリによる編集

//R0232
//他アプリによる編集有無の確認（編集有りなら反映確認ダイアログ表示）
void	ADocument_Text::SPI_CheckEditByOtherApp()
{
	//ファイル取得
	AFileAcc	file;
	switch( mTextDocumentType )
	{
	  case kTextDocumentType_Normal:
		{
			//------------------通常ローカルファイルの場合------------------
			//ファイル未specifyであれば何もしない
			if( NVI_IsFileSpecified() == false )
			{
				return;
			}
			//ファイル取得
			NVI_GetFile(file);
			break;
		}
	  case kTextDocumentType_DiffTarget:
		{
			//------------------diff targetファイルの場合------------------
			//ファイル未specifyであれば何もしない
			if( mFileForDiffTargetDocument.IsSpecified() == false )
			{
				return;
			}
			//ファイル取得
			file = mFileForDiffTargetDocument;
			break;
		}
	  default:
		return;
		break;
	}
	
	//#831
	//未ロードなら何もしない
	if( mLoaded == false )   return;
	
	/*
	下記の条件を両方満たす場合、他アプリによる編集反映ダイアログを表示する
	1. 前回オープン／保存時と最終更新日時が異なるとき、もしくは、最終更新日時が取得できない
	2. 前回オープン／保存時とファイル内容が異なる
	*/
	
	//最終更新日時確認
	if( file.Exist() == false )
	{
		//ファイル削除等された場合は、チェックしない（次に上書き保存すれば、新たにファイル作成される。）
		return;
	}
	ADateTime	datetime = -1;
	ABool	lastModifiedDateObtained = file.GetLastModifiedDataTime(datetime);//#653
	//前回オープン／保存時と最終更新日時が異なるとき、もしくは、最終更新日時が取得できなかったら、
	//ファイル内容比較処理を行う。
	if( datetime != mLastModifiedDateTime || lastModifiedDateObtained == false )//★lastModifiedDateObtained取得できない場合あるのか再検討。アプリ切り替え時のボトルネックの可能性？#1267
	{
		//現在のファイル内容と、前回オープン／保存時のファイル内容(mLastReadOrWriteFileRawData)を比較する #653
		//ロック解除、ロックを行う。（ロック必要かどうかわからないがSPI_ReloadEditByOtherApp()の処理に合わせておく。）
		//ロック解除
		mFileWriteLocker.Unlock();
		//読み込み（テキストエンコーディング変更しない。生データで比較）
		AText	text;
		file.ReadTo(text);
		//ロック
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kLockWhileEdit) == true && NVI_IsReadOnly() == false )
		{
			mFileWriteLocker.Lock();
		}
		//最後にロード・セーブしたときのファイル内容との比較（他アプリでの編集の有無をチェック）
		//#693 if( text.Compare(mLastReadOrWriteFileRawData) == true )
		AFileAcc	lastLoadOrSaveFile;
		GetLastLoadOrSaveCopyFile(lastLoadOrSaveFile);
		AText	lastLoadOrSaveText;
		lastLoadOrSaveFile.ReadTo(lastLoadOrSaveText);
		if( text.Compare(lastLoadOrSaveText) == true || lastLoadOrSaveFile.Exist() == false )
		{
			//ファイル内容が同じときは、最終更新日時のみ更新して、何もしない。（ダイアログ表示しない）
			if( lastModifiedDateObtained == true )
			{
				mLastModifiedDateTime = datetime;
			}
		}
		else
		{
			//#379 GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_ShowAskApplyEditByOtherApp(GetObjectID());
			/*#653 kDontShowEditByOhterAppDialog設定は削除 if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontShowEditByOhterAppDialog) == true )
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontShowEditByOhterAppDialog) == true )
			{
				//kDontShowEditByOhterAppDialogがONなら、常にリロードする #545
				SPI_ReloadEditByOtherApp(true);
			}
			else
			{
			*/
			if( NVI_IsDirty() == false )
			{
				//dirtyではない（diff target documentの場合含む）の場合は、常に変更を読み込む
				SPI_ReloadEditByOtherApp(true);
			}
			else
			{
				//dirtyのときは、読み込むかどうかのダイアログを表示
				AWindowID	firstWindowID = SPI_GetFirstWindowID();
				if( firstWindowID != kObjectID_Invalid )
				{
					GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_ShowAskApplyEditByOtherApp(GetObjectID());
				}
			}
			//#653}
		}
	}
}

//R0232
//他アプリの編集結果の読込
//inReload: trueなら読み込みする  falseなら読み込みせずに最終更新日時のみupdateする（次のappアクティベート時に再度ダイアログを表示しないように）
void	ADocument_Text::SPI_ReloadEditByOtherApp( const ABool inReload )
{
	//ファイル取得
	AFileAcc	file;
	switch( mTextDocumentType )
	{
	  case kTextDocumentType_Normal:
		{
			//------------------通常ローカルファイルの場合------------------
			//ファイル未specifyであれば何もしない
			if( NVI_IsFileSpecified() == false )
			{
				return;
			}
			//ファイル取得
			NVI_GetFile(file);
			break;
		}
		/*
		//------------------diff targetファイルの場合------------------
		とりあえず対応しない。対応する奈良、下記の対応が必要。
		・miで比較元ファイルを変更した場合、アプリケーション切替以外でもここに来る必要がある。
		・ワーキングファイルとともにdiff dataを更新する必要がある。
	  case kTextDocumentType_DiffTarget:
		{
			if( mFileForDiffTargetDocument.IsSpecified() == false )
			{
				return;
			}
			file = mFileForDiffTargetDocument;
			break;
		}
		*/
	  default:
		return;
		break;
	}
	
	//Reloadしない場合でも、ロック解除、ロックを行う。
	//そうしないと、Save時に、旧ファイルデータをリカバリ用に読み込むときに、ファイルサイズの違うエラーが起こる。
	//（たぶん、miでファイルを開いたまま、旧ファイルデータを読み込むので、ファイルの長さを読むときに旧データの長さが適用される。）
	//ロック解除
	mFileWriteLocker.Unlock();
	//読み込み#653
	//#693 file.ReadTo(mLastReadOrWriteFileRawData);
	AText	fileText;
	file.ReadTo(fileText);
	//Reloadしない場合でもlastloadorsaveファイルには記憶する。Reloadしないことを選択したときのデータを記憶するため。
	AFileAcc	lastLoadOrSaveFile;
	GetLastLoadOrSaveCopyFile(lastLoadOrSaveFile);
	lastLoadOrSaveFile.CreateFile();
	lastLoadOrSaveFile.WriteFile(fileText);
	//最終更新日時更新
	file.GetLastModifiedDataTime(mLastModifiedDateTime);
	//ロック
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kLockWhileEdit) == true && NVI_IsReadOnly() == false )
	{
		mFileWriteLocker.Lock();
	}
	//
	if( inReload == true )
	{
		//各viewの選択範囲を記憶
		AArray<ATextPoint>	sptArray, eptArray;
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			ATextPoint	spt = {0}, ept = {0};
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetSelect(spt,ept);
			sptArray.Add(spt);
			eptArray.Add(ept);
		}
		//Undoアクションタグ記録
		SPI_RecordUndoActionTag(undoTag_EditByOtherApp);
		//全削除
		SPI_DeleteText(0,SPI_GetTextLength(),true,false);
		//UTF8/CRに変換
		ConvertToUTF8(fileText);//#764
		//テキスト挿入
		SPI_InsertText(0,fileText,true,false);
		//ファイル名取得
		AText	fileName;
		file.GetFilename(fileName);
		//各viewの選択範囲を復元、notification表示
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			//選択範囲復元
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_SetSelect(sptArray.Get(i),eptArray.Get(i));
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_AdjustScroll_Center();
			//notification表示
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetPopupNotificationWindow().
					SPI_GetNotificationView().SPI_SetNotification_ReloadEditByOtherApp(fileName);
			AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_ShowNotificationPopupWindow(true);
		}
		//元々dirtyだった場合でも、ファイルから読み込んだので、not dirtyにするのが妥当。
		NVI_SetDirty(false);
		//------------------Diffデータ更新------------------
		SPI_UpdateDiffDisplayData();
	}
}

//#693
/**
ロード時、および、最後のセーブ時のファイルのコピーを保存するファイルを取得
*/
void	ADocument_Text::GetLastLoadOrSaveCopyFile( AFileAcc& outFile ) const
{
	//ファイルパス取得
	AText	filepath;
	NVI_GetFilePath(filepath);
	//doc prefフォルダ取得
	AFileAcc	docPrefRootFolder;
	GetApp().SPI_GetDocPrefRootFolder(docPrefRootFolder);
	AFileAcc	docPrefFolder;
	docPrefFolder.SpecifyChild(docPrefRootFolder,filepath);
	docPrefFolder.CreateFolderRecursive();
	//doc prefフォルダ内のlastloadorsaveファイルを取得
	outFile.SpecifyChild(docPrefFolder,"lastloadorsave");
}

#pragma mark ===========================

/*#92
void	ADocument_Text::SPI_SelectFirstWindow()
{
	if( mWindowIDArray.GetItemCount() == 0 )   return;
	GetApp().NVI_GetWindowByID(mWindowIDArray.Get(0)).NVI_SelectTabByDocumentID(GetObjectID());
	GetApp().NVI_GetWindowByID(mWindowIDArray.Get(0)).NVI_SelectWindow();
}
*/

//#199
/**
最初のView上で指定範囲を選択し、最前面に持ってくる
*/
void	ADocument_Text::SPI_SelectText( const AIndex inStartIndex, const AItemCount inLength, const ABool inByFind )
{
	/*#199
	if( mWindowIDArray.GetItemCount() == 0 )   return;
	GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(0)).SPI_SelectText(GetObjectID(),inStartIndex,inLength);
	*/
	if( mViewIDArray.GetItemCount() == 0 )   return;
	//#312 フォーカスされているViewがあればそれを優先（なければ、最初のView）
	AViewID	viewID = mViewIDArray.Get(0);
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		if( AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).NVI_IsFocusActive() == true ||
					AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).NVI_IsFocusInTab() == true )
		{
			viewID = mViewIDArray.Get(i);
			break;
		}
	}
	//選択
	AView_Text::GetTextViewByViewID(viewID).SPI_SelectText(inStartIndex,inLength,inByFind);
	//表示
	AView_Text::GetTextViewByViewID(viewID).NVI_RevealView();
}

AWindowID	ADocument_Text::SPI_GetFirstWindowID() const
{
	/* #379 mViewIDArrayからWindowIDを取得するように変更
	if( mWindowIDArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	return mWindowIDArray.Get(0);
	*/
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AWindowID	textWindowID = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetTextWindowID();
		if( textWindowID != kObjectID_Invalid )
		{
			return textWindowID;
		}
	}
	return kObjectID_Invalid;
}

#pragma mark ===========================

#pragma mark ---行折り返し計算
//#699

/*
スレッドによる行折り返し計算の流れ
1. スレッドによる行折り返しを実行するときは、事前に、1行に行折り返し計算したいテキストが全て入っている状態
（それ以外の行データは整合がとれている状態）で、実行する。その行を「現在行」として記憶。
スレッドへその行のテキストを渡す。
2. スレッドはk行を折り返し計算して、メインスレッドに戻す。（テキストのどこまで実行したかはスレッドで記憶）
3. メインスレッドは、「現在行」以降にスレッドで計算されたk行分の行データをコピーする。（k行追加）
4. メインスレッドは、「現在行」として、コピーした次の行に、残りのテキストが全て入っている状態にしておく。
5. スレッドに継続実行を指示する。
6. 折り返し計算すべきテキストが無くなるまで繰り返す。
（つまり、常に「現在行」mWrapCalculator_CurrentLineIndexをスレッドが展開していく形にする。）
*/

/**
スレッドに行折り返し計算を依頼する
@param inLineIndex 折り返しを計算する対象行
*/
void	ADocument_Text::StartWrapCalculator( const AIndex inLineIndex )
{
	if( mWrapCalculator_ThreadUnpausedFlag == false )
	{
		//スレッド動作中でない場合
		
		//指定行の識別子を削除
		AArray<AUniqID>	deletedIdentifier;
		ABool	importChanged = false;
		mTextInfo.DeleteLineIdentifiers(inLineIndex,deletedIdentifier,importChanged);
		DeleteFromJumpMenu(deletedIdentifier);
		if( importChanged == true )
		{
			mImportFileDataDirty = true;
		}
		
		//mWrapCalculator_CurrentLineIndexに折り返し計算対象行を設定
		mWrapCalculator_CurrentLineIndex = inLineIndex;
		//折り返し計算対象行（＝計算対象テキストがつめこまれている）のLineStart, len取得
		AIndex	lineStart = mTextInfo.GetLineStart(inLineIndex);
		AItemCount	len = mTextInfo.GetLineLengthWithLineEnd(inLineIndex);
		//Progress表示用に、計算する全体のレングスを記憶
		mWrapCalculatorProgressTotalTextLength = len;
		mWrapCalculatorProgressRemainedTextLength = len;
		//計算対象テキストをスレッドに設定
		GetWrapCalculatorThread().SPI_SetText(mText,lineStart,len);
		//メモリ割り当てステップを設定（メインスレッドのmTextInfo）
		mTextInfo.SetLineReallocateStep(mText.GetCountOfChar(kUChar_LineEnd,lineStart,len));
		//スレッドに行折り返しパラメータを設定
		AText	fontname;
		SPI_GetFontName(fontname);
		ANumber	viewWidth = AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_GetTextViewWidth();
		GetWrapCalculatorThread().SPI_SetWrapParameters(fontname,SPI_GetFontSize(),SPI_IsAntiAlias(),
					SPI_GetTabWidth(),SPI_GetIndentWidth(),SPI_GetWrapMode(),SPI_GetWrapLetterCount(),
					viewWidth,GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters));
		//スレッド起動（起動後、スレッドはpauseされた状態になる。ContinueWrapCalculator()でpause解除。）
		GetWrapCalculatorThread().NVI_Run(true);//最初はpause状態
		//スレッドunpause
		ContinueWrapCalculator();
	}
	else
	{
		//スレッド動作中の場合
		
		//行折り返し計算中は編集不可とするため、ここには来ないはず
		_ACError("",NULL);
	}
}

/**
行折り返し計算スレッドpause時処理
*/
ABool	ADocument_Text::SPI_DoWrapCalculatorPaused()
{
	//スレッド動作中のチェック
	if( mWrapCalculator_ThreadUnpausedFlag == false )
	{
		//ここに来るときはスレッド動作中のはず
		_ACError("",NULL);
		return false;
	}
	
	//スレッド動作中フラグoff
	mWrapCalculator_ThreadUnpausedFlag = false;
	
	//スレッドの計算結果をmTextInfoへコピー
	//スレッド内の行データが全てmWrapCalculator_CurrentLineIndex以降にコピー追加される
	//行計算完了の場合は、trueを返す。EOF空行が必要な場合は、CopyFromCalculatorResult()にて追加済み。
	ABool	completed = mTextInfo.CopyFromCalculatorResult(mWrapCalculator_CurrentLineIndex,
			GetWrapCalculatorThread().SPI_GetTextInfo());
	
	//TextViewのLineImageInfo更新
	AItemCount	insertedLineCount = GetWrapCalculatorThread().SPI_GetTextInfo().GetLineCount();
	if( completed == true )
	{
		insertedLineCount--;
	}
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateLineImageInfo(mWrapCalculator_CurrentLineIndex,
			insertedLineCount,mWrapCalculator_CurrentLineIndex);
	}
	
	//TextViewに行計算中を通知（行番号ボタン更新のため）
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_DoWrapCalculating();
	}
	
	//描画更新
	SPI_RefreshTextViews(mWrapCalculator_CurrentLineIndex,SPI_GetLineCount());
	
	//行折り返し計算完了判定
	if( completed == false )
	{
		//未完了の場合は、スレッド行計算対象行が計算済み行の後に入っている。
		mWrapCalculator_CurrentLineIndex += GetWrapCalculatorThread().SPI_GetTextInfo().GetLineCount();
		
		//Progress表示用に、残っているテキストの量を取得
		mWrapCalculatorProgressRemainedTextLength = 
				mTextInfo.GetLineLengthWithLineEnd(mWrapCalculator_CurrentLineIndex);
		//スレッドunpause
		ContinueWrapCalculator();
	}
	else
	{
		//行折り返し完了
		
		//Progress表示用データ初期化
		mWrapCalculatorProgressTotalTextLength = kIndex_Invalid;
		mWrapCalculatorProgressRemainedTextLength = 0;
		
		//スレッド停止
		GetWrapCalculatorThread().NVI_AbortThread();
		GetWrapCalculatorThread().NVI_Unpause();
		GetWrapCalculatorThread().NVI_WaitThreadEnd();
		GetWrapCalculatorThread().ClearData();
		
		//行折り返し計算完了時処理
		if( SPI_GetDocumentInitState() == kDocumentInitState_WrapCalculating )
		{
			//初期化中の場合
			
			//状態遷移
			SPI_TransitInitState_WrapCalculateCompleted();
		}
		else if( mWrapCalculatorWorkingByInsertText == true )
		{
			//編集による行折り返し計算中の場合
			
			//編集による行折り返し計算中フラグoff
			mWrapCalculatorWorkingByInsertText = false;
			//文法認識未認識の行から文法認識
			AIndex	unrecognizedLineIndex = mTextInfo.FindFirstUnrecognizedLine();
			if( unrecognizedLineIndex != kIndex_Invalid )
			{
				StartSyntaxRecognizer(unrecognizedLineIndex);
			}
			//TextViewに行折り返し計算完了を通知（予約した選択位置が反映される）
			for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
			{
				AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_DoWrapCalculated();
			}
			//#142
			//テキストカウントウインドウ更新
			UpdateDocInfoWindows();
		}
		else
		{
			_ACError("",NULL);
		}
	}
	//タイトルバー表示更新
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_UpdateWindowTitleBarText();
	}
	//リターン
	return completed;
}

/**
行折り返し計算スレッドunpause
*/
void	ADocument_Text::ContinueWrapCalculator()
{
	mWrapCalculator_ThreadUnpausedFlag = true;
	//スレッド実行再開（スレッドpause状態解除）
	GetWrapCalculatorThread().NVI_Unpause();
}

/**
行折り返し計算スレッド取得
*/
AThread_WrapCalculator&	ADocument_Text::GetWrapCalculatorThread()
{
	if( mWrapCalculatorThreadID == kObjectID_Invalid )
	{
		AThread_WrapCalculatorFactory	factory(this);
		mWrapCalculatorThreadID = GetApp().NVI_CreateThread(factory);
	}
	return (dynamic_cast<AThread_WrapCalculator&>(GetApp().NVI_GetThreadByID(mWrapCalculatorThreadID)));
}

/**
行折り返し計算の進捗取得
*/
ANumber	ADocument_Text::SPI_GetWrapCalculatorProgressPercent()
{
	if( SPI_GetDocumentInitState() == kDocumentInitState_WrapCalculating || mWrapCalculatorWorkingByInsertText == true )
	{
		//テキストのサイズが小さい場合に、ドキュメントを開いた直後に、「折り返し計算中」「書き込み禁止」どちらも表示させないため
		if( mWrapCalculatorProgressTotalTextLength < 1024*1024 )
		{
			return 0;
		}
		//0除算防止（念のため）
		if( mWrapCalculatorProgressTotalTextLength == 0 )   return 100;
		//パーセント計算
		AFloatNumber	result = mWrapCalculatorProgressTotalTextLength-mWrapCalculatorProgressRemainedTextLength;
		result *= 100;
		result /= mWrapCalculatorProgressTotalTextLength;
		return result;
	}
	else
	{
		return kIndex_Invalid;
	}
}

#pragma mark ===========================

#pragma mark ---文法認識
//#698

//#905
/**
unrecognizedな行をサーチして全て（スレッドで）認識する
*/
void	ADocument_Text::SPI_RecognizeSyntaxUnrecognizedLines()
{
	//文法認識未完了なら何もせず終了 #0
	if( SPI_GetDocumentInitState() != kDocumentInitState_Completed )
	{
		return;
	}
	//
	AIndex	unrecognizedLineIndex = mTextInfo.FindFirstUnrecognizedLine();
	if( unrecognizedLineIndex != kIndex_Invalid )
	{
		StartSyntaxRecognizer(unrecognizedLineIndex);
	}
}

/**
スレッドによる文法認識開始
既にスレッド動作中の場合は、次回スレッドから戻ってきたときにスレッド文法認識を再開する
（inLineIndexがいまの認識開始行よりも小さければ、次回認識開始行を更新する）
@param inLineIndex 認識開始行（0行目以外は、既に行の開始状態等が設定済みの行を指定すること。）
*/
void	ADocument_Text::StartSyntaxRecognizer( const AIndex inLineIndex )
{
	//行折り返し計算中は何もせずリターン
	if( mWrapCalculatorWorkingByInsertText == true )
	{
		return;
	}
	
	//スレッド未起動ならスレッド起動
	if( mSyntaxRecognizer_ThreadRunningFlag == false )
	{
		//スレッド起動（起動後、スレッドはpauseされた状態になる。ContinueWrapCalculator()でpause解除。）
		GetSyntaxRecognizerThread().NVI_Run(true);//最初はpause状態
		mSyntaxRecognizer_ThreadRunningFlag = true;
	}
	
	//スレッド動作中判定
	if( mSyntaxRecognizer_ThreadUnpausedFlag == false )
	{
		//スレッド動作中で無い場合
		
		// =============== スレッド開始処理 ===============
		
		//文法認識前処理（コンテキスト作成）
		AText	url;//#998
		SPI_GetURL(url);//#998
		mSyntaxRecognizer_StartLineIndex = mTextInfo.PrepareRecognizeSyntaxContext(mText,inLineIndex,url);//#998
		
		//スレッドにコンテキストをコピー
		GetSyntaxRecognizerThread().SPI_SetContext(mModeIndex,mTextInfo,mSyntaxRecognizer_StartLineIndex);
		
		//文法認識終了行設定（今回実行分）
		mSyntaxRecognizer_EndLineIndex = mSyntaxRecognizer_StartLineIndex 
				+ kSyntaxRecognizerThreadPauseLineCount;//一回の実行分
		if( mSyntaxRecognizer_EndLineIndex >= SPI_GetLineCount() )
		{
			mSyntaxRecognizer_EndLineIndex = SPI_GetLineCount();
		}
		
		//スレッドに、今回実行分の対象テキストとTextInfoを設定
		AIndex	startLine_LineStart = SPI_GetLineStart(mSyntaxRecognizer_StartLineIndex);
		AIndex	endLine_LineStart = SPI_GetLineStart(mSyntaxRecognizer_EndLineIndex);
		GetSyntaxRecognizerThread().SPI_SetText(mText,startLine_LineStart,endLine_LineStart-startLine_LineStart);
		GetSyntaxRecognizerThread().SPI_SetTextInfo(mTextInfo,
					mSyntaxRecognizer_StartLineIndex,mSyntaxRecognizer_EndLineIndex);
		//スレッド動作中フラグon
		mSyntaxRecognizer_ThreadUnpausedFlag = true;
		//スレッドリスタートフラグ初期化（off）
		mSyntaxRecognizer_RestartFlag = false;
		//スレッドunpause
		GetSyntaxRecognizerThread().NVI_Unpause();
		//
		if( AApplication::NVI_GetEnableDebugTraceMode() == true )
		{
			fprintf(stderr,"SyntaxRecognizerStart:%ld-%ld\n",
						mSyntaxRecognizer_StartLineIndex,mSyntaxRecognizer_EndLineIndex);
		}
	}
	else
	{
		//スレッド動作中の場合
		
		//スレッドリスタートフラグon。次回スレッドから戻ってきたときに、今回の結果は破棄してリスタートを行う
		mSyntaxRecognizer_RestartFlag = true;
		//inLineIndexが、いまの認識開始行よりも小さければ、次回認識開始行を更新する
		if( inLineIndex < mSyntaxRecognizer_StartLineIndex )
		{
			mSyntaxRecognizer_StartLineIndex = inLineIndex;
		}
	}
}

/**
テキスト編集（InsertText/DeleteText）による文法再認識開始
@return 文法認識終了行（InsertText/DeleteTextでTextViewの描画更新のために使う）
*/
AIndex	ADocument_Text::StartSyntaxRecognizerByEdit( const AIndex inLineIndex, const AItemCount inInsertedLineCount,
		const ABool inRecognizeSyntaxAlwaysInThread )
{
	//#905 テキスト編集時には文法認識は行わない。
	//編集後、時間経過や改行などのタイミングでSPI_RecognizeSyntaxUnrecognizedLines()がコールされ、スレッドで文法認識される。
	return inLineIndex;
	/*#905
	//行折り返し計算中は何もせずリターン
	if( mWrapCalculatorWorkingByInsertText == true )
	{
		return inLineIndex;
	}
	
	//スレッド動作中でない場合：閾値未満ならメインスレッド内で認識、閾値以上なら認識スレッドに認識依頼
	//スレッド動作中の場合：次回スレッドから戻ったときに、inLineIndexから再開
	
	//recognizeEndLineIndex: 返り値（文法認識終了行）
	//スレッド実行にする場合は、開始行（inLineIndex）を返すようにする
	//（返り値は、文法認識による描画更新範囲を広げるために使用されているので、
	//inLineIndexを返せば、描画更新範囲は広がらないことになる。（描画更新はスレッド終了時に処理すべきなので。））
	AIndex	recognizeEndLineIndex = inLineIndex;
	
	if( inRecognizeSyntaxAlwaysInThread == true )
	{
		//スレッドで認識開始
		StartSyntaxRecognizer(inLineIndex);
		//
		return inLineIndex;
	}
	
	//スレッド動作中判定
	if( mSyntaxRecognizer_ThreadUnpausedFlag == false )
	{
		//スレッド動作中でない場合
		if( inInsertedLineCount >= kThreashold_LineCountToUseSyntaxRecognizerThreadByEdit )
		{
			//挿入行数が閾値以上の場合
			
			//スレッドで認識開始
			StartSyntaxRecognizer(inLineIndex);
		}
		else
		{
			//挿入行数が閾値未満の場合
			
			//メインスレッド内で認識処理
			recognizeEndLineIndex = RecognizeSyntaxInMainThread(inLineIndex,kIndex_Invalid);
		}
	}
	else
	{
		//スレッド動作中の場合
		
		//スレッドで認識開始
		StartSyntaxRecognizer(inLineIndex);
	}
	return recognizeEndLineIndex;
	*/
}

/**
文法認識スレッドpause時処理
*/
void	ADocument_Text::SPI_DoSyntaxRecognizerPaused()
{
	//行折り返し計算中は何もせずリターン
	if( mSyntaxRecognizer_ThreadUnpausedFlag == false )
	{
		_ACError("",NULL);
		return;
	}
	
	//スレッド動作中フラグoff
	mSyntaxRecognizer_ThreadUnpausedFlag = false;
	
	//リスタートフラグ判定
	if( mSyntaxRecognizer_RestartFlag == true )
	{
		//RestartフラグがOnのときは、今回のスレッド文法認識結果を破棄して、
		//指定された行（mSyntaxRecognizer_StartLineIndex）からスレッド文法認識をリスタートする。
		StartSyntaxRecognizer(mSyntaxRecognizer_StartLineIndex);
	}
	else
	{
		//今回のスレッド文法認識結果を、スレッドから本TextInfoへコピーする。
		CopySyntaxRecognizerResult();
		
		//文法認識終了判定
		//認識モジュールで文法認識終了と判断された場合（＝ExecuteRecognizeSyntax()が途中の行で終了した場合）、または、
		//スレッドに認識指示した最終行がテキストの最終行の場合は、認識終了と見なす。
		//それ以外の場合は、今回スレッドに認識指示していた最終行を開始行としてスレッド継続実行する。
		if( GetSyntaxRecognizerThread().SPI_RecognizerJudgedAsEnd() == false && 
					mSyntaxRecognizer_EndLineIndex < SPI_GetLineCount() )
		{
			//文法認識未完了
			
			//今回スレッドに認識指示していた行の最後の行を開始行としてスレッド継続実行
			//ContinueSyntaxRecognizer(mSyntaxRecognizer_EndLineIndex);
			AIndex	lineIndex = mSyntaxRecognizer_EndLineIndex;
			if( lineIndex > 0 )
			{
				//最後の認識済み行を開始行として指示する
				//（認識開始行は、行開始時点でのインデント情報等を要するため。）
				lineIndex--;
			}
			//StartSyntaxRecognizer()は、stable stateまでさかのぼって開始する。
			//これは、各種変数やtoken stackのspos情報は保持できない（spos→point変換するためにはtext infoの保持が必要）ため、
			//毎回、コンテキストは作り直し、変数・token stackは初期化するため。stable stateまでさかのぼれば、
			//変数・token stack初期化の影響は少ない（従来の通常編集時と同じになる）。
			StartSyntaxRecognizer(lineIndex);
		}
		else
		{
			//文法認識完了
			
			//テキストの最初から文法認識が未完了の行を検索
			//（スレッド認識中に認識済みの箇所を編集した場合など、未認識→認識済み→未認識の状態になる。
			//このとき、最初の未認識の終了箇所で認識モジュールで文法認識終了と判断されるので、
			//未認識の箇所から認識再開させる必要がある。）
			AIndex	unrecognizedLineIndex = mTextInfo.FindFirstUnrecognizedLine();
			if( unrecognizedLineIndex != kIndex_Invalid )
			{
				//未完了の行があればその行から文法認識をリスタート
				if( unrecognizedLineIndex > 0 )
				{
					//最後の認識済み行を開始行として指示する
					//（認識開始行は、行開始時点でのインデント情報等を要するため。）
					unrecognizedLineIndex--;
				}
				StartSyntaxRecognizer(unrecognizedLineIndex);
			}
			else
			{
				//文法認識未完了の行無し（文法認識完了）
				
				//スレッド終了
				mSyntaxRecognizer_ThreadRunningFlag = false;
				GetSyntaxRecognizerThread().NVI_AbortThread();
				GetSyntaxRecognizerThread().NVI_Unpause();
				GetSyntaxRecognizerThread().NVI_WaitThreadEnd();
				GetSyntaxRecognizerThread().ClearData();
				mSyntaxRecognizer_StartLineIndex = SPI_GetLineCount();
				mSyntaxRecognizer_EndLineIndex = SPI_GetLineCount();
				//状態遷移
				if( SPI_GetDocumentInitState() == kDocumentInitState_SyntaxRecognizing )
				{
					SPI_TransitInitState_SyntaxRecognizeCompleted();
				}
			}
		}
	}
}

/**
スレッド文法認識結果をメインスレッドのTextInfoへコピー
*/
void	ADocument_Text::CopySyntaxRecognizerResult()
{
	AArray<AUniqID>	deletedIdentifier;
	AArray<AUniqID>	addedGlobalIdentifier;
	AArray<AIndex>	addedGlobalIdentifierLineIndex;
	ABool	importChanged = false;
	//コピー先の行を取得
	AIndex	dstStartLineIndex = mSyntaxRecognizer_StartLineIndex;
	AIndex	dstEndLineIndex = mSyntaxRecognizer_StartLineIndex 
			+ GetSyntaxRecognizerThread().SPI_GetRecognizeEndLineIndex();
	//文法認識結果コピー実行
	mTextInfo.CopyFromSyntaxRecognizerResult(dstStartLineIndex,dstEndLineIndex,
				GetSyntaxRecognizerThread().SPI_GetTextInfo(),
				deletedIdentifier,addedGlobalIdentifier,addedGlobalIdentifierLineIndex,importChanged);
	//文法認識によるimport更新フラグ設定
	if( importChanged == true )
	{
		mImportFileDataDirty = true;
	}
	//文法認識により削除されたIdentifierに対応するメニュー項目を削除する
	ABool	jumpMenuChanged = DeleteFromJumpMenu(deletedIdentifier);
	//文法認識により追加されたIdentifierをメニューに登録
	if( addedGlobalIdentifier.GetItemCount() > 0 )
	{
		AddToJumpMenu(addedGlobalIdentifier,addedGlobalIdentifierLineIndex);
		jumpMenuChanged = true;
	}
	//ジャンプリスト更新
	if( jumpMenuChanged == true )
	{
		//ジャンプリスト描画更新
		SPI_RefreshJumpList();
		//ジャンプリスト選択位置更新
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AView_Text&	textview = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i));
			if( textview.NVI_IsFocusActive() == true )
			{
				ATextPoint	spt = {0,0}, ept = {0,0};
				textview.SPI_GetSelect(spt,ept);
				SPI_UpdateJumpListSelection(spt.y,false);//キャレット選択による更新ではないのでスクロールはadjustしない
			}
		}
	}
	
	//#905
	//文法データを更新した範囲が、各ビューのローカル範囲と重なる場合は、ローカル識別子リストを更新する
	for( AIndex i = 0; i < mCurrentLocalIdentifierStartLineIndexArray.GetItemCount(); i++ )
	{
		AIndex	localStartLineIndex = mCurrentLocalIdentifierStartLineIndexArray.Get(i);
		AIndex	localEndLineIndex = mCurrentLocalIdentifierEndLineIndexArray.Get(i);
		if( localStartLineIndex != kIndex_Invalid && localEndLineIndex != kIndex_Invalid )
		{
			if( ! (dstEndLineIndex < localStartLineIndex || localEndLineIndex <= dstStartLineIndex) )
			{
				UpdateLocalIdentifierList(i,false);
			}
		}
	}
	
	//テキストビュー更新
	SPI_RefreshTextViews(dstStartLineIndex,dstEndLineIndex);
	
	//#905（タイミング変更）
	//Import File Data更新
	SPI_UpdateImportFileDataIfDirty();
}

/**
文法認識スレッド取得
*/
AThread_SyntaxRecognizer&	ADocument_Text::GetSyntaxRecognizerThread()
{
	if( mSyntaxRecognizerThreadID == kObjectID_Invalid )
	{
		AThread_SyntaxRecognizerFactory	factory(this);
		mSyntaxRecognizerThreadID = GetApp().NVI_CreateThread(factory);
	}
	return (dynamic_cast<AThread_SyntaxRecognizer&>(GetApp().NVI_GetThreadByID(mSyntaxRecognizerThreadID)));
}

//文法認識
AIndex	ADocument_Text::RecognizeSyntaxInMainThread( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	ABool	jumpMenuChanged = false;
	//文法認識実行
	AArray<AUniqID>	deletedIdentifier;
	AArray<AUniqID>	addedGlobalIdentifier;
	AArray<AIndex>	addedGlobalIdentifierLineIndex;
	//#698 AIndex	recognizedStartLineIndex, recognizedEndLineIndex;
	ABool	importChanged = false;
	AText	url;//#998
	SPI_GetURL(url);//#998
	mTextInfo.PrepareRecognizeSyntaxContext(mText,inStartLineIndex,url);//#698 #998
	ABool	abortFlag = false;
	AIndex	recognizedEndLineIndex = mTextInfo.ExecuteRecognizeSyntax(mText,inEndLineIndex,deletedIdentifier,
				addedGlobalIdentifier,addedGlobalIdentifierLineIndex,importChanged,abortFlag);//#698
	mTextInfo.ClearRecognizeSyntaxContext();//#698
	//fprintf(stderr,"%d-%d   ",recognizedStartLineIndex,recognizedEndLineIndex);
	if( importChanged == true )
	{
		//UpdateImportFileData();
		mImportFileDataDirty = true;
	}
	//文法認識により削除されたIdentifierに対応するメニュー項目を削除する
	jumpMenuChanged = DeleteFromJumpMenu(deletedIdentifier);
	//if( jumpMenuChanged == true )  fprintf(stderr,"deleted:%d ",deletedIdentifier.GetItemCount());
	//文法認識により追加されたIdentifierをメニューに登録
	/*#695
	for( AIndex i = 0; i < addedGlobalIdentifier.GetItemCount(); i++ )
	{
		if( mTextInfo.IsMenuIdentifier(addedGlobalIdentifier.Get(i)) == true )
		{
			AddToJumpMenu(addedGlobalIdentifier.Get(i),addedGlobalIdentifierLineIndex.Get(i));
			jumpMenuChanged = true;
		}
		//fprintf(stderr,"added:%d ",addedIdentifierLineIndex.Get(i));
	}
	*/
	if( addedGlobalIdentifier.GetItemCount() > 0 )
	{
		AddToJumpMenu(addedGlobalIdentifier,addedGlobalIdentifierLineIndex);
		jumpMenuChanged = true;
	}
	//ジャンプリスト更新
	if( jumpMenuChanged == true )
	{
		//#695 SPI_UpdateJumpList();//#291
		SPI_RefreshJumpList();//#695
	}
	return recognizedEndLineIndex;
}

/* #695 高速化して、ClearSyntaxDataForAllLinesへ移動
void	ADocument_Text::ClearSyntaxData( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	ABool	jumpMenuChanged = false;
	AArray<AUniqID>	deletedIdentifier;//#695
	for( AIndex lineIndex = inEndLineIndex-1; lineIndex >= inStartLineIndex; lineIndex-- )//#695
	{
		//対象行のIdentifierを削除
		ABool	importChanged = false;
		mTextInfo.DeleteLineIdentifiers(lineIndex,deletedIdentifier,importChanged);
		if( importChanged == true )
		{
			//UpdateImportFileData();
			mImportFileDataDirty = true;
		}
		mTextInfo.SetLineRecognized(lineIndex,false);
	}
	//文法認識により削除されたIdentifierに対応するメニュー項目を削除する #695 ループ内から移動
	if( DeleteFromJumpMenu(deletedIdentifier) == true )
	{
		jumpMenuChanged = true;
	}
	//ジャンプリスト更新
	if( jumpMenuChanged == true )
	{
		//#695 SPI_UpdateJumpList();//#291  
		SPI_RefreshJumpList();//#695
	}
}
*/

/**
文法再認識
*/
void	ADocument_Text::SPI_ReRecognizeSyntax()
{
	//#695 ClearSyntaxData(0,SPI_GetLineCount());
	/*#698
	ATextPoint	pt = {0,0};
	RecognizeSyntax(pt);
	*/
	
	//#695
	//初期状態であれば、何もせず終了
	if( SPI_GetDocumentInitState() == kDocumentInitState_Initial || 
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened ||
	   SPI_GetDocumentInitState() == kDocumentInitState_WrapCalculating )
	{
		return;
	}
	
	//全文法認識データ削除
	mTextInfo.ClearSyntaxDataForAllLines();
	//ジャンプ項目全削除
	DeleteAllJumpItems();
	
	//#890
	//文法認識スレッドが動作中なら、文法認識スレッドを停止する
	SPI_StopSyntaxRecognizerThread();
	
	//文法認識開始
	StartSyntaxRecognizer(0);
	
	//#554
	//描画更新
	SPI_RefreshTextViews();
}

//#844
/**
文法認識データ全削除、全行識別子削除・認識済みフラグOFF
*/
void	ADocument_Text::SPI_ClearSyntaxDataForAllLines()
{
	//全文法認識データ削除、全行識別子削除・認識済みフラグOFF
	mTextInfo.ClearSyntaxDataForAllLines();
	//ジャンプ項目全削除
	DeleteAllJumpItems();
}

/**
文法認識スレッドを停止する
*/
void	ADocument_Text::SPI_StopSyntaxRecognizerThread()
{
	if( mSyntaxRecognizer_ThreadRunningFlag == true )
	{
		//スレッド停止
		mSyntaxRecognizer_ThreadUnpausedFlag = false;
		mSyntaxRecognizer_ThreadRunningFlag = false;
		mSyntaxRecognizer_RestartFlag = false;
		mSyntaxRecognizer_StartLineIndex = kIndex_Invalid;
		mSyntaxRecognizer_EndLineIndex = kIndex_Invalid;
		GetSyntaxRecognizerThread().NVI_AbortThread();
		GetSyntaxRecognizerThread().NVI_UnpauseIfPaused();
		GetSyntaxRecognizerThread().NVI_WaitThreadEnd();
		GetSyntaxRecognizerThread().ClearData();
	}
	//イベントキューから、このドキュメントの文法認識スレッドから送信した文法認識完了イベントを削除
	ABase::RemoveEventFromMainInternalEventQueue(kInternalEvent_SyntaxRecognizerPaused,GetSyntaxRecognizerThread().GetObjectID());
}


void	ADocument_Text::UpdateWindowText( const AIndex inEditStartLineIndex, 
			const AIndex inRedrawStartLineIndex, const AIndex inRedrawEndLineIndex, const AItemCount inInsertedLineCount, const ABool inParagraphCountChanged,
			const ABool inDontRedraw )//#627
{
	/*#199 Viewのメソッドを直接コールするように修正
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateText(GetObjectID(),inStartLineIndex,inEndLineIndex,inInsertedLineCount,inParagraphCountChanged);
	}
	*/
	//textview
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		//#450
		AIndex	updateLineImageInfoStart = inRedrawStartLineIndex;
		if( mCurrentLocalIdentifierStartLineIndexArray.Get(i) != kIndex_Invalid && mCurrentLocalIdentifierStartLineIndexArray.Get(i) < updateLineImageInfoStart )
		{
			updateLineImageInfoStart = mCurrentLocalIdentifierStartLineIndexArray.Get(i);
			//★#695 lineViewInfoの計算開始位置が戻りすぎないようにする
			if( updateLineImageInfoStart < inRedrawStartLineIndex-10 )
			{
				updateLineImageInfoStart = inRedrawStartLineIndex-10;
			}
		}
		//text view取得
		AView_Text&	textview = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i));
		//viewのorigin point取得
		AImagePoint	frameOriginPoint = {0};
		textview.NVI_GetOriginOfLocalFrame(frameOriginPoint);
		//line image infoを更新
		ANumber	imageYDelta = textview.SPI_UpdateLineImageInfo(inEditStartLineIndex,inInsertedLineCount,updateLineImageInfoStart);//#627
		if( inDontRedraw == false )//#627
		{
			//==================描画更新==================
			if( imageYDelta > 0 )//#627
			{
				//------------------text viewでimageY変更が発生している場合------------------
				
				//編集終了行を取得
				AIndex	editEndLineIndex = inEditStartLineIndex;
				if( inInsertedLineCount > 0 )
				{
					editEndLineIndex += inInsertedLineCount;
				}
				if( editEndLineIndex < textview.SPI_GetOriginLineIndex() )
				{
					//------------------編集終了行が、viewのframe開始行より前の場合------------------
					//（＝他のviewで編集が発生している場合）
					//view frame内の開始行が変化しないようにする
					frameOriginPoint.y += imageYDelta;
					textview.NVI_SetOriginOfLocalFrame(frameOriginPoint);
					//背景表示ありの場合のみ描画更新
					if( GetApp().SPI_GetModePrefDB(mModeIndex).GetModeData_Bool(AModePrefDB::kDisplayBackgroundImage) == true )
					{
						textview.NVI_Refresh();
					}
					//行番号viewのorigin point更新、描画更新
					frameOriginPoint.x = 0;
					textview.SPI_GetLineNumberView().NVI_SetOriginOfLocalFrame(frameOriginPoint);
					textview.SPI_GetLineNumberView().NVI_Refresh();
				}
				else
				{
					//------------------編集終了行が、viewのframe開始行以降の場合------------------
					//編集行以降を全て更新する
					//#688 AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshTextView();//#450 NVI_Refresh();
					textview.SPI_RefreshLines(inRedrawStartLineIndex,SPI_GetLineCount());
				}
			}
			else
			{
				//------------------text viewでimageY変更が発生していない場合------------------
				//textviewでImageY変更が発生していなければ通常更新
				textview.SPI_UpdateText(inRedrawStartLineIndex,inRedrawEndLineIndex,inInsertedLineCount,inParagraphCountChanged);//#450
			}
		}
	}
	/*#450
	//行番号view
	for( AIndex i = 0; i < mLineNumberViewIDArray.GetItemCount(); i++ )
	{
		AView_LineNumber::GetLineNumberViewByViewID(mLineNumberViewIDArray.Get(i)).
				SPI_UpdateText(inStartLineIndex,inEndLineIndex,inInsertedLineCount,inParagraphCountChanged);
	}
	*/
}

/*#379
void	ADocument_Text::UpdateWindowViewImageSize()
{
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_UpdateViewImageSize(GetObjectID());
	}
}
*/
/*#291
void	ADocument_Text::UpdateWindowJumpList()
{
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		GetTextWindowByIndex(index).SPI_UpdateJumpList(GetObjectID());
	}
}

//R0006
void	ADocument_Text::UpdateWindowJumpListWithColor()
{
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		GetTextWindowByIndex(index).SPI_UpdateJumpListWithColor(GetObjectID());
	}
}
*/
/*#379
void	ADocument_Text::UpdateInfoHeader()
{
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		GetApp().SPI_GetTextWindowByID(mWindowIDArray.Get(i)).SPI_SetInfoHeaderText(GetEmptyText());
	}
}
*/
/*#212
void	ADocument_Text::SPI_RefreshWindow()
{
	RefreshWindow();
}

void	ADocument_Text::RefreshWindow()
{
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		if( GetTextWindowByIndex(index).SPI_IsDocumentVisible(GetObjectID()) == true )
		{
			GetTextWindowByIndex(index).NVI_RefreshWindow();
		}
	}
}
*/

//win 080727
//TextView表示更新
void	ADocument_Text::SPI_RefreshTextViews()
{
	/*#199 Viewのメソッドを直接コールするように修正
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		if( GetTextWindowByIndex(index).SPI_IsDocumentVisible(GetObjectID()) == true )
		{
			GetTextWindowByIndex(index).SPI_RefreshTextViews(GetObjectID());
		}
	}
	*/
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshTextView();//TextViewとLineNumberViewを両方更新 #379
	}
}

//#695
/**
TextView表示更新（行範囲指定）
*/
void	ADocument_Text::SPI_RefreshTextViews( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_RefreshLines(inStartLineIndex,inEndLineIndex);//TextViewとLineNumberViewを両方更新 #379
	}
}

//#413
/**
キャレット位置補正
*/
void	ADocument_Text::SPI_CorrectCaretIfInvalid()
{
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_CorrectCaretIfInvalid();
	}
}

/*#379
void	ADocument_Text::SPI_TryClose()
{
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		GetTextWindowByIndex(index).SPI_TryCloseDocument(GetObjectID());
	}
}

void	ADocument_Text::SPI_ExecuteClose()
{
	for( AIndex index = 0; index < mWindowIDArray.GetItemCount(); index++ )
	{
		GetTextWindowByIndex(index).SPI_ExecuteCloseDocument(GetObjectID());
	}
}
*/
ANumber	ADocument_Text::GetTextViewWidth()
{
	/*#379
	if( mWindowIDArray.GetItemCount() > 0 )
	{
		return GetTextWindowByIndex(0).SPI_GetTextViewWidth(GetObjectID());
	}
	*/
	AWindowID	firstWindowID = SPI_GetFirstWindowID();
	if( firstWindowID != kObjectID_Invalid )
	{
		return GetApp().SPI_GetTextWindowByID(firstWindowID).SPI_GetTextViewWidth(GetObjectID());
	}
	//
	return 500;//zantei
}

#pragma mark ===========================

#pragma mark ---補完入力
//#717

#if 0
/**
@note thread-safe
*/
void	ADocument_Text::SPI_GetImportIdentifierAbbrevCandidate( const AText& inText, const AText& inParentWord,
		ATextArray& outAbbrevCandidateArray,
		ATextArray& outInfoTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
		ATextArray& outFilePathArray ) const
{
	//
	AFileAcc	file;
	NVI_GetFile(file);
	//
	AStMutexLocker	locker(mImportIdentifierLinkListMutex);
	//
	mImportIdentifierLinkList.GetAbbrevCandidate(inText,inParentWord,file,
				outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray,outScopeArray,outFilePathArray);
}

/**
現在のローカル識別子から補完候補検索
@note thread-safe
*/
void	ADocument_Text::SPI_GetLocalIdentifierAbbrevCandidate( const AText& inText, const AText& inParentWord,
		ATextArray& outAbbrevCandidateArray,
		ATextArray& outInfoTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
		ATextArray& outFilePathArray ) const
{
	AFileAcc	file;
	NVI_GetFile(file);
	AText	path;
	file.GetPath(path);
	//
	AStMutexLocker	locker(mCurrentLocalIdentifierMutex);
	//
	AItemCount	itemCount = mCurrentLocalIdentifier.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		const AText&	keywordText = mCurrentLocalIdentifier.GetObjectConst(i).GetKeywordText();
		if( keywordText.GetItemCount() >= inText.GetItemCount() )
		{
			if( keywordText.CompareMin(inText) == true )
			//test AIndex	pos = 0;
			//test if( keywordText.FindText(0,inText,pos) == true )
			{
				if( outAbbrevCandidateArray.Find(keywordText) == kIndex_Invalid )//重複するキーワードは表示しない
				{
					outAbbrevCandidateArray.Add(keywordText);
					AText	infotext;
					infotext.SetText(mCurrentLocalIdentifier.GetObjectConst(i).GetInfoText());
					outInfoTextArray.Add(infotext);
					outCategoryIndexArray.Add(mCurrentLocalIdentifier.GetObjectConst(i).GetCategoryIndex());
					outScopeArray.Add(kScopeType_Local);
					outFilePathArray.Add(path);
				}
			}
		}
	}
}
#endif
//#853
/**
各種キーワード検索スレッド用統合キーワード検索処理(Global識別子)
@note ハッシュを使わないので低速なため、スレッド使用が必須だが、いろいろな検索方法ができる。thread-safe
*/
ABool	ADocument_Text::SPI_SearchKeywordGlobal( const AText& inWord, const AHashTextArray& inParentWord, 
												const AKeywordSearchOption inOption,
												const AIndex inCurrentStateIndex,
												AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
												ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
												ATextArray& outCommentTextArray,
												ATextArray& outCompletionTextArray, ATextArray& outURLArray,
												AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
												AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
												AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
												AArray<AItemCount>& outMatchedCountArray,
		const ABool& inAbort ) const
{
	//キーワード検索実行
	ABool	result = mTextInfo.SearchKeywordGlobal(inWord,inParentWord,inOption,
												   inCurrentStateIndex,
												   outKeywordArray,outParentKeywordArray,
												   outTypeTextArray,outInfoTextArray,outCommentTextArray,
												   outCompletionTextArray,outURLArray,
												   outCategoryIndexArray,outColorSlotIndexArray,
												   outStartIndexArray,outEndIndexArray,outScopeArray,outMatchedCountArray,
												   inAbort);
	//outFileArrayを埋める
	AFileAcc	file;
	NVI_GetFile(file);
	AText	path;
	file.GetPath(path);
	if( mTextDocumentType == kTextDocumentType_RemoteFile )
	{
		path.SetText(mRemoteFileURL);
	}
	while( outFilePathArray.GetItemCount() < outKeywordArray.GetItemCount() )
	{
		outFilePathArray.Add(path);
	}
	return result;
}

//#853
/**
各種キーワード検索スレッド用統合キーワード検索処理(ローカル識別子)
@note ハッシュを使わないので低速なため、スレッド使用が必須だが、いろいろな検索方法ができる。thread-safe
*/
ABool	ADocument_Text::SPI_SearchKeywordLocal( const AText& inWord, const AHashTextArray& inParentWord, 
											   const AKeywordSearchOption inOption,
											   const AIndex inCurrentStateIndex,
											   AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
											   ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
											   ATextArray& outCommentTextArray,
											   ATextArray& outCompletionTextArray, ATextArray& outURLArray,
											   AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
											   AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
											   AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray, 
											   AArray<AItemCount>& outMatchedCountArray,
											   const AViewID inViewID, 
		const ABool& inAbort ) const
{
	//#893
	//viewのindexを取得
	AIndex	viewIndex = kIndex_Invalid;
	if( inViewID != kObjectID_Invalid )
	{
		viewIndex = mViewIDArray.Find(inViewID);
	}
	if( viewIndex == kIndex_Invalid )
	{
		//view indexが取得できない場合は、何もせず終了
		return true;
	}
	
	//ローカル範囲が未設定なら何もせずリターン
	if( mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex) == kIndex_Invalid )   return true;
	
	//キーワード検索実行
	ABool	result = mTextInfo.SearchKeywordLocal(inWord,inParentWord,inOption,inCurrentStateIndex,
												  mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex),
												  mCurrentLocalIdentifierEndLineIndexArray.Get(viewIndex),
												  outKeywordArray,outParentKeywordArray,
												  outTypeTextArray,outInfoTextArray,outCommentTextArray,
												  outCompletionTextArray,outURLArray,
												  outCategoryIndexArray,outColorSlotIndexArray,
												  outStartIndexArray,outEndIndexArray,outScopeArray,outMatchedCountArray,
												  inAbort);
	//outFileArrayを埋める
	AFileAcc	file;
	NVI_GetFile(file);
	AText	path;
	file.GetPath(path);
	if( mTextDocumentType == kTextDocumentType_RemoteFile )
	{
		path.SetText(mRemoteFileURL);
	}
	while( outFilePathArray.GetItemCount() < outKeywordArray.GetItemCount() )
	{
		outFilePathArray.Add(path);
	}
	return result;
}

//#853
/**
各種キーワード検索スレッド用統合キーワード検索処理(Import識別子)
@note ハッシュを使わないので低速なため、スレッド使用が必須だが、いろいろな検索方法ができる。thread-safe
*/
ABool	ADocument_Text::SPI_SearchKeywordImport( const AText& inWord, const AHashTextArray& inParentWord, 
												const AKeywordSearchOption inOption,
												const AIndex inCurrentStateIndex,
												AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
												ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
												ATextArray& outCommentTextArray,
												ATextArray& outCompletionTextArray, ATextArray& outURLArray,
												AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
												AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
												AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
												AArray<AItemCount>& outMatchedCountArray,
												const ABool& inAbort ) const
{
	//ImportIdentifierLinkList排他制御
	AStMutexLocker	locker(mImportIdentifierLinkListMutex);
	//インポートキーワード検索実行
	return mImportIdentifierLinkList.SearchKeywordImport(inWord,inParentWord,inOption,inCurrentStateIndex,
		outKeywordArray,outParentKeywordArray,
		outTypeTextArray,outInfoTextArray,outCommentTextArray,
		outCompletionTextArray,outURLArray,
		outCategoryIndexArray,outColorSlotIndexArray,
		outStartIndexArray,outEndIndexArray,outScopeArray,outFilePathArray,outMatchedCountArray,
		inAbort);
}

#pragma mark ===========================

#pragma mark ---Importファイル

//ヘッダファイル等のインポートファイルのデータを更新する。（#include文が更新された場合や、最初の文法解析のとき、および、インポートファイルの解析スレッド完了時に呼ばれる。）
void	ADocument_Text::UpdateImportFileData()
{
	if( NVI_IsFileSpecified() == false )   return;
	
	//#349 SDF未使用時はインポートファイル無し
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseBuiltinSyntaxDefinitionFile) == false &&
	   GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseUserDefinedSDF) == false )
	{
		mImportFileArray.DeleteAll();
		AStMutexLocker	locker(mImportIdentifierLinkListMutex);//#717
		mImportIdentifierLinkList.DeleteAll();
		return;
	}
	
	//インポート識別子リンクリスト全削除
	mImportFileArray.DeleteAll();
	{
		AStMutexLocker	locker(mImportIdentifierLinkListMutex);
		mImportIdentifierLinkList.DeleteAll();
	}
	AFileAcc	file;
	NVI_GetFile(file);
	AFileAcc	folder;
	folder.SpecifyParent(file);
	//win 080722 mTextInfo.GetImportFileArray(folder,mImportFileArray);
	//win 080722
	//SDFにImportLinkToProjectAllの設定があるときは、プロジェクト内の全てのファイルをインポート扱いする
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetImportLinkToProjectAll() == true )
	{
		//プロジェクトフォルダ設定済みの場合は、プロジェクトフォルダ内の同じモードのファイルを全てインポート対象
		AIndex	projectindex = GetApp().SPI_GetSameProjectArrayIndex(mProjectFolder);
		if( projectindex != kIndex_Invalid )
		{
			const ATextArray&	pathnamearray = GetApp().SPI_GetSameProjectPathnameArray(projectindex);
			for( AIndex i = 0; i < pathnamearray.GetItemCount(); i++ )
			{
				const AText&	path = pathnamearray.GetTextConst(i);
				if( GetApp().SPI_GetModePrefDB(mModeIndex).IsThisModeSuffix(path) == true )
				{
					AFileAcc	file;
					file.Specify(path);
					mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(file);
				}
			}
		}
		//プロジェクトフォルダ未設定の場合は、同じフォルダの同じモードのファイルをインポート対象
		else
		{
			ATextArray	samefolder;
			GetApp().SPI_GetSameFolderFilenameTextArray(samefolder);
			for( AIndex i = 0; i < samefolder.GetItemCount(); i++ )
			{
				const AText&	filename = samefolder.GetTextConst(i);
				if( GetApp().SPI_GetModePrefDB(mModeIndex).IsThisModeSuffix(filename) == true )
				{
					AFileAcc	file;
					file.SpecifyChild(folder,filename);
					mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(file);
				}
			}
		}
	}
	else
	{
		//文法認識によって得られたインポートファイルを取得
		mTextInfo.GetImportFileArray(folder,mImportFileArray);
		//ドキュメントのファイルと同じフォルダの同名別拡張子のファイルを追加する（たとえば、App.hのドキュメントからApp.cppの定義を参照するため）
		AText	origfn;
		file.GetFilename(origfn);
		origfn.DeleteAfter(origfn.GetSuffixStartPos());
		//同じフォルダ内の全てのファイルについてモードが同じ、かつ、拡張子ぬきの名前が一緒なものを検索する
		ATextArray	samefolder;
		GetApp().SPI_GetSameFolderFilenameTextArray(samefolder);
		for( AIndex i = 0; i < samefolder.GetItemCount(); i++ )
		{
			const AText&	filename = samefolder.GetTextConst(i);
			if( GetApp().SPI_GetModePrefDB(mModeIndex).IsThisModeSuffix(filename) == true )
			{
				AText	fn;
				fn.SetText(filename);
				fn.DeleteAfter(fn.GetSuffixStartPos());
				if( origfn.Compare(fn) == true )
				{
					AFileAcc	folder;
					folder.SpecifyParent(file);
					AFileAcc	f;
					f.SpecifyChild(folder,filename);
					if( file.Compare(f) == false )
					{
						mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(f);
					}
				}
			}
		}
	}
	//test
	/*
	{
		AFileAcc	folder;
		folder.Specify("C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Include\\");
		AObjectArray<AFileAcc>	children;
		folder.GetChildren(children);
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			AText	filename;
			children.GetObjectConst(i).GetFilename(filename);
			AText	suffix;
			filename.GetSuffix(suffix);
			if( suffix.Compare(".h") == true )
			{
				mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(children.GetObjectConst(i));
			}
		}
	}
	*/
	/*{
		AFileAcc	file;
		file.Specify("C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Include\\Windows.h");
		mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(file);
	}*/
	/*
	//#253
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kImportSystemHeaderFiles) == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kSystemHeaderFiles,text);
		for( AIndex pos = 0; pos < text.GetItemCount();  )
		{
			AText	line;
			text.GetLine(pos,line);
			AFileAcc	folder;
			folder.Specify(line);
			if( folder.IsFolder() == true )
			{
				AObjectArray<AFileAcc>	children;
				folder.GetChildren(children);
				for( AIndex i = 0; i < children.GetItemCount(); i++ )
				{
					AText	filename;
					children.GetObjectConst(i).GetFilename(filename);
					if( GetApp().SPI_GetModePrefDB(mModeIndex).IsThisModeSuffix(filename) == true )
					{
						mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(children.GetObjectConst(i));
					}
				}
			}
			else
			{
				mImportFileArray.GetObject(mImportFileArray.AddNewObject()).CopyFrom(folder);
			}
		}
	}
	*/
	
	//
	//#423 mImportIdentifierLinkList.DeleteAll();//検討必要　必要な部分だけ更新できないか？
	if( mImportFileArray.GetItemCount() > 0 )
	{
		AStMutexLocker	locker(mImportIdentifierLinkListMutex);//#717
		//インポートファイルへのリンクデータ取得（未解析があれば解析要求）
		GetApp().SPI_GetImportFileManager().GetImportIdentifierLinkList(GetObjectID(),
			mImportFileArray,SPI_GetModeIndex(),mImportIdentifierLinkList,false);
	}
	else //#423
	{
		AStMutexLocker	locker(mImportIdentifierLinkListMutex);//#717
		mImportIdentifierLinkList.DeleteAll();
	}
	
	//「関連ファイル」メニューの更新
	GetApp().SPI_UpdateImportFileMenu(SPI_GetImportFileArray());
}

//
void	ADocument_Text::SPI_UpdateImportFileDataIfDirty()
{
	if( mImportFileDataDirty == true )
	{
		UpdateImportFileData();
		//#212 RefreshWindow();
		SPI_RefreshTextViews();//#212
		mImportFileDataDirty = false;
	}
}

/**
インポートファイルの文法認識完了時にimport file managerからコールされる
*/
void	ADocument_Text::SPI_DoImportFileRecognized()
{
	UpdateImportFileData();
	//win 080727 RefreshWindow();
	SPI_RefreshTextViews();//win 080727
}

//
const AObjectArray<AFileAcc>&	ADocument_Text::SPI_GetImportFileArray() const
{
	return mImportFileArray;
}

/**
インポート識別子リンクリストから指定ファイルのデータを削除する
*/
ABool	ADocument_Text::SPI_DeleteImportFileData( const AObjectID inImportFileDataObjectID, const AFileAcc& inFile )
{
	//インポート識別子リンクリストから指定ファイルのデータを削除
	//（高速化のため、mRevisionData_ImportFileDataIDから検索して、一致しなければ、ハッシュ内は検索しないという対策済み）
	AStMutexLocker	locker(mImportIdentifierLinkListMutex);//#717
	return mImportIdentifierLinkList.Delete(inImportFileDataObjectID);
}

//#349
/**
ドキュメント内のImportファイルデータを全て削除
AImportFileManager::DeleteAll()からコールされる。
AImportIdentifierLinkListからAImportFileDataへのリンクが貼られているので、
AImportFileDataを全削除する前に、リンクデータを全て削除する必要があるため。
*/
void	ADocument_Text::SPI_DeleteAllImportFileData()
{
	mImportFileArray.DeleteAll();
	{
		AStMutexLocker	locker(mImportIdentifierLinkListMutex);//#717
		mImportIdentifierLinkList.DeleteAll();
	}
}

#pragma mark ===========================

#pragma mark ---ローカル識別子

void	ADocument_Text::SPI_SetCurrentLocalTextPoint( const ATextPoint& inTextPoint, const AViewID inViewID )
{
	//文法解析完了までは、ローカル範囲が長くなりすぎる可能性があるので、何もしない #0
	if( SPI_GetDocumentInitState() != kDocumentInitState_Completed )
	{
		return;
	}
	//#893
	//viewのindexを取得
	AIndex	viewIndex = kIndex_Invalid;
	if( inViewID != kObjectID_Invalid )
	{
		viewIndex = mViewIDArray.Find(inViewID);
	}
	if( viewIndex == kIndex_Invalid )   return;
	
	//#867 高速化
	//指定テキストポイントが現在のローカル範囲内なら何もせずリターン
	//AIndex	textIndex = SPI_GetTextIndexFromTextPoint(inTextPoint);
	if( inTextPoint.y >= mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex) && inTextPoint.y < mCurrentLocalIdentifierEndLineIndexArray.Get(viewIndex) )
	{
		return;
	}
	//
	AIndex	start, end;
	mTextInfo.GetLocalRangeByLineIndex(inTextPoint.y,start,end);
	//ローカル範囲更新
	AIndex	oldStart = mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex);
	mCurrentLocalIdentifierStartLineIndexArray.Set(viewIndex,start);
	mCurrentLocalIdentifierEndLineIndexArray.Set(viewIndex,end);
	//ローカル識別子リスト更新（ローカル開始位置に変更が無ければredrawはしない）
	UpdateLocalIdentifierList(viewIndex,(oldStart != start));
}

/**
ローカル識別子リスト更新
*/
void	ADocument_Text::UpdateLocalIdentifierList( const AIndex viewIndex, const ABool inAlwaysRefresh )
{
	//ローカル範囲取得
	AIndex	localStartLineIndex = mCurrentLocalIdentifierStartLineIndexArray.Get(viewIndex);
	AIndex	localEndLineIndex = mCurrentLocalIdentifierEndLineIndexArray.Get(viewIndex);
	//ローカル範囲未設定時は何もせず終了 #0
	if( localStartLineIndex == kIndex_Invalid || localEndLineIndex == kIndex_Invalid )
	{
		return;
	}
	//
	AItemCount	oldCount = mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetItemCount();
	{
		//local identifier list排他制御
		AStMutexLocker	locker(mCurrentLocalIdentifierMutex);//#717
		//local identifier list取得
		mTextInfo.GetLocalRangeIdentifier(localStartLineIndex,localEndLineIndex,
					this,mCurrentLocalIdentifierListArray.GetObject(viewIndex));
	}
	//識別子個数に変化、または、inAlwaysRefreshがtrueなら描画更新
	if( mCurrentLocalIdentifierListArray.GetObjectConst(viewIndex).GetItemCount() != oldCount || inAlwaysRefresh == true )
	{
		//RC1
		SPI_UpdateArrowToDef(false);
		//win 080727 RefreshWindow();
		SPI_RefreshTextViews();
	}
}

/**
指定行を含むローカル範囲を取得
*/
void	ADocument_Text::SPI_GetLocalRangeForLineIndex( const AIndex inLineIndex, AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const
{
	//文法解析完了までは、ローカル範囲が長くなりすぎる可能性があるので、invalidを返す #0
	if( mDocumentInitState != kDocumentInitState_Completed )
	{
		outStartLineIndex = kIndex_Invalid;
		outEndLineIndex = kIndex_Invalid;
		return;
	}
	//
	outStartLineIndex = 0;
	outEndLineIndex = SPI_GetLineCount();
	mTextInfo.GetLocalRangeByLineIndex(inLineIndex,outStartLineIndex,outEndLineIndex);
}

//RC1
void	ADocument_Text::SPI_UpdateArrowToDef( const ABool inRefresh )
{
	//★
	/*
	mCurrentLocalIdentifierAnalyzeDrawData.Init();
	if( mDrawArrowToDef == true )
	{
		mCurrentLocalIdentifierAnalyzeDrawData.flagArrowToDef = true;
		for( AIndex lineIndex = mCurrentLocalIdentifierStartLineIndex; lineIndex < mCurrentLocalIdentifierEndLineIndex; lineIndex++ )
		{
			CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
			ATextPoint	tmp1, tmp2;
			GetTextDrawDataWithStyle(mText,mTextInfo,lineIndex,textDrawData,false,tmp1,tmp2,mCurrentLocalIdentifierAnalyzeDrawData);//#695
		}
	}
	if( inRefresh == true )
	{
		SPI_RefreshTextViews();
	}
	*/
}

//RC1
void	ADocument_Text::ClearArrowToDef()
{
	//★
	/*
	if( mCurrentLocalIdentifierAnalyzeDrawData.defStartArrowToDef.GetItemCount() > 0 )
	{
		mCurrentLocalIdentifierAnalyzeDrawData.Init();
		//#212 RefreshWindow();
		SPI_RefreshTextViews();//#212
	}
	*/
}

//#699
/**
ローカル識別子をローカル識別子リストから全削除し、現在のローカル範囲を解除
*/
void	ADocument_Text::SPI_DeleteAllLocalIdentifiers()
{
	for( AIndex viewIndex = 0; viewIndex < mViewIDArray.GetItemCount(); viewIndex++ )
	{
		//ローカル識別子削除
		AStMutexLocker	locker(mCurrentLocalIdentifierMutex);//#717
		mCurrentLocalIdentifierListArray.GetObject(viewIndex).DeleteAll();
		//現在のローカル範囲を解除
		mCurrentLocalIdentifierStartLineIndexArray.Set(viewIndex,kIndex_Invalid);
		mCurrentLocalIdentifierEndLineIndexArray.Set(viewIndex,kIndex_Invalid);
	}
}

//#717
/**
現在のローカル開始識別子の親キーワード取得（＝典型的には現在の関数のクラス）
*/
void	ADocument_Text::SPI_GetCurrentLocalStartIdentifierParentKeyword( const AIndex inLineIndex ,AText& outParentKeyword ) const
{
	mTextInfo.GetCurrentLocalStartIdentifierParentKeyword(inLineIndex,outParentKeyword);
}

//#723
/**
指定行を含むローカル範囲のローカル開始識別子のデータを取得
*/
void	ADocument_Text::SPI_GetLocalStartIdentifierDataByLineIndex( const AIndex inLineIndex, 
		AText& outFunctionName, AText& outParentKeywordName, 
		AIndex& outCategoryIndex, ATextPoint& outStart, ATextPoint& outEnd ) const
{
	AIndex	lineIndex = inLineIndex;
	if( lineIndex > SPI_GetLineCount() )   lineIndex = SPI_GetLineCount()-1;
	//行Indexに対応するローカル開始識別子のObjectIDを得る
	AUniqID	identifierUniqID = mTextInfo.GetLocalStartIdentifierByLineIndex(lineIndex);
	if( identifierUniqID != kUniqID_Invalid )
	{
		//識別子の各データ取得
		if( mTextInfo.GetGlobalIdentifierKeywordText(identifierUniqID,outFunctionName) == false )
		{
			//キーワードテキストが無い場合は、メニューテキストを取得
			mTextInfo.GetGlobalIdentifierMenuText(identifierUniqID,outFunctionName);
		}
		//親キーワード取得
		mTextInfo.GetGlobalIdentifierParentKeywordText(identifierUniqID,outParentKeywordName);
		//カテゴリ取得
		outCategoryIndex = mTextInfo.GetGlobalIdentifierCategoryIndex(identifierUniqID);
		//識別子の位置取得
		SPI_GetGlobalIdentifierRange(identifierUniqID,outStart,outEnd);
	}
}

#pragma mark ===========================

#pragma mark ---ジャンプメニュー管理

/**
ジャンプメニューへの識別子追加
@param inIdentifierUniqIDArray 追加識別子の配列（行順に並んでいること前提）
*/
void	ADocument_Text::AddToJumpMenu( const AArray<AUniqID>& inIdentifierUniqIDArray, const AArray<AIndex>& inLineIndexArray )//#695
{
	//追加識別子の個数を取得#695
	AItemCount	itemCount = inIdentifierUniqIDArray.GetItemCount();
	//追加無しなら何もせずリターン
	if( itemCount == 0 )   return;
	
	//最初の項目の位置に、全項目を追加する。（そのため、inIdentifierUniqIDArrayは行順に並んでいる必要がある）
	//最初の項目の行index取得
	AIndex	firstItemLineIndex = inLineIndexArray.Get(0);
	//挿入位置決定
	AIndex	firstItemMenuIndex = 0;
	if( firstItemLineIndex > 0 )
	{
		//前の行のjump indexの次に挿入
		firstItemMenuIndex = SPI_GetJumpMenuItemIndexByLineIndex(firstItemLineIndex-1) + 1;
	}
	
	//内部データReserve
	mJump_MenuTextArray.Reserve(firstItemMenuIndex,itemCount);
	mJump_MenuTextStyleArray.Reserve(firstItemMenuIndex,itemCount);
	mJump_MenuColorArray.Reserve(firstItemMenuIndex,itemCount);
	mJump_MenuOutlineLevelArray.Reserve(firstItemMenuIndex,itemCount);//#130
	
	//色取得
	AColor	subWindowNormalColor = kColor_List_Normal;//#1316 GetApp().SPI_GetSubWindowLetterColor();
	
	//各項目追加
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//uniq id取得
		AUniqID	identifierUniqID = inIdentifierUniqIDArray.Get(i);
		//メニューテキスト取得
		AText	menutext;
		mTextInfo.GetGlobalIdentifierMenuText(identifierUniqID,menutext);
		AText	actiontext;
		actiontext.SetFormattedCstring("%d",identifierUniqID.val);
		AIndex	categoryIndex = mTextInfo.GetGlobalIdentifierCategoryIndex(identifierUniqID);
		ATextStyle	style = GetApp().SPI_GetModePrefDB(mModeIndex).GetCategoryMenuTextStyle(categoryIndex);
		//#130 アウトラインレベル取得
		AIndex	outlineLevel = mTextInfo.GetGlobalIdentifierOutlineLevel(identifierUniqID);
		
		//メニュー挿入
		//挿入位置が制限値より前の場合のみメニューへ挿入する #695
		//さらにこのループの下でメニューの制限値以上の項目を削除しているので、メニュー項目の総数は常に制限値までに制限される
		//メニュー選択時、削除時は、UniqIDをキーにして検索している。削除は、存在する場合のみ項目削除している。
		if( firstItemMenuIndex + i < kLimit_MaxJumpMenuItemCount )//#695
		{
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByObjectID(
						mJumpMenuObjectID,firstItemMenuIndex + i,menutext,actiontext,kObjectID_Invalid,0,0,false);
		}
		
		//内部データに挿入
		mJump_IdentifierObjectArray.Insert1(firstItemMenuIndex + i,identifierUniqID);
		//内部データに値設定
		mJump_MenuTextArray.Set(firstItemMenuIndex + i,menutext);
		mJump_MenuTextStyleArray.Set(firstItemMenuIndex + i,style);
		mJump_MenuColorArray.Set(firstItemMenuIndex + i,subWindowNormalColor);
		mJump_MenuOutlineLevelArray.Set(firstItemMenuIndex + i,outlineLevel);//#130
	}
	//メニューの制限値以上の項目を削除 #695
	while( GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mJumpMenuObjectID) > kLimit_MaxJumpMenuItemCount )
	{
		GetApp().NVI_GetMenuManager().DeleteDynamicMenuItemByObjectID(mJumpMenuObjectID,kLimit_MaxJumpMenuItemCount);
	}
	//ジャンプリスト挿入更新#695
	SPI_UpdateJumpList_Inserted(firstItemMenuIndex,itemCount);
}

/**
ジャンプメニューから指定Identifierに対応する項目を削除
@param inIdentifierUniqIDArray 削除すべきIdentifierのUniqID（すでに削除済みでも良い。また、menu表示対象でなくても良い）
@note 複数行まとめて削除する場合は、各行ごとに呼ぶのではなく、まとめてコールすること。（この関数内でMakeHash()を毎回実行するため）
*/
ABool	ADocument_Text::DeleteFromJumpMenu( const AArray<AUniqID>& inIdentifierUniqIDArray )
{
	ABool	jumpMenuChanged = false;
	//deleteMenuItemIndexArrayに削除する項目のjump indexを取得
	AArray<AIndex>	deleteMenuItemIndexArray;//#695
	AItemCount	identifierItemCount = inIdentifierUniqIDArray.GetItemCount();
	for( AIndex i = 0; i < identifierItemCount; i++ )
	{
		AUniqID	UniqID = inIdentifierUniqIDArray.Get(i);
		AIndex	menuIndex = mJump_IdentifierObjectArray.Find(UniqID);
		if( menuIndex != kIndex_Invalid )
		{
			deleteMenuItemIndexArray.Add(menuIndex);//#695
		}
	}
	//降順ソート（下のループで各配列の後ろの項目から削除していくため）
	deleteMenuItemIndexArray.Sort(false);
	//mJump_IdentifierObjectArrayのHashを一時的にdisable（高速化のため）
	mJump_IdentifierObjectArray.SetEnableHash(false);//#695
	//各項目削除
	AItemCount	deleteMenuItemIndexArrayCount = deleteMenuItemIndexArray.GetItemCount();//#695
	for( AIndex i = 0; i < deleteMenuItemIndexArrayCount; i++ )//#695
	{
		AIndex	menuIndex = deleteMenuItemIndexArray.Get(i);//#695
		//#695 GetApp().NVI_GetMenuManager().DeleteDynamicMenuItemByObjectID(mJumpMenuObjectID,menuIndex);
		//ジャンプメニューから削除
		AText	actiontext;
		actiontext.SetNumber(mJump_IdentifierObjectArray.Get(menuIndex).val);
		AIndex	jumpMenuItemIndex = GetApp().NVI_GetMenuManager().
				FindDynamicMenuItemIndexFromActionText(mJumpMenuObjectID,actiontext);
		if( jumpMenuItemIndex != kIndex_Invalid )
		{
			GetApp().NVI_GetMenuManager().DeleteDynamicMenuItemByObjectID(mJumpMenuObjectID,jumpMenuItemIndex);
		}
		//内部データから削除
		mJump_MenuTextArray.Delete1(menuIndex);
		mJump_MenuTextStyleArray.Delete1(menuIndex);
		mJump_IdentifierObjectArray.Delete1(menuIndex);
		//#699 未使用のため削除mJump_ParagraphTextArray.Delete1(menuIndex);
		mJump_MenuColorArray.Delete1(menuIndex);//R0006
		mJump_MenuOutlineLevelArray.Delete1(menuIndex);//#130
		//ジャンプリスト削除#695
		SPI_UpdateJumpList_Deleted(menuIndex);
		//変更フラグon
		jumpMenuChanged = true;
	}
	//mJump_IdentifierObjectArrayのHashの一時的disableを解除し、再度MakeHash
	mJump_IdentifierObjectArray.SetEnableHash(true);//#695
	mJump_IdentifierObjectArray.MakeHash();//#695
	return jumpMenuChanged;
}

//#695
/**
ジャンプ項目全削除
*/
void	ADocument_Text::DeleteAllJumpItems()
{
	//メニュー全削除
	GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByObjectID(mJumpMenuObjectID);
	//内部データ全削除
	mJump_MenuTextArray.DeleteAll();
	mJump_MenuTextStyleArray.DeleteAll();
	mJump_IdentifierObjectArray.DeleteAll();
	mJump_MenuColorArray.DeleteAll();
	mJump_MenuOutlineLevelArray.DeleteAll();//#130
	//ジャンプリスト更新
	SPI_UpdateJumpList();//#695 true);
}

//行Indexからジャンプメニュー項目Indexを取得
AIndex	ADocument_Text::SPI_GetJumpMenuItemIndexByLineIndex( const AIndex inLineIndex ) const
{
	//引数が不正値なら、該当メニュー項目なしで返す
	if( inLineIndex >= SPI_GetLineCount() || inLineIndex < 0 )   return kIndex_Invalid;
	
	//段落取得
	AIndex	paragraphIndex = mTextInfo.GetParagraphIndexByLineIndex(inLineIndex);
	AItemCount	paragraphCount = SPI_GetParagraphCount();
	
	//ジャンプ項目数取得
	AItemCount	jumpItemCount = mJump_IdentifierObjectArray.GetItemCount();
	if( jumpItemCount == 0 )
	{
		//該当ジャンプ項目無し
		return kIndex_Invalid;
	}
	
	//最初のジャンプ項目よりも前なら該当ジャンプ項目無し
	if( paragraphIndex < mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(0)) )
	{
		return kIndex_Invalid;
	}
	
	//キャッシュ検索
	//前回のヒット位置（mCacheForGetJumpMenuItemIndexByLineIndex）と、その１つ後が該当するなら、それを返す
	if( mCacheForGetJumpMenuItemIndexByLineIndex != kIndex_Invalid )
	{
		for( AIndex jumpIndex = mCacheForGetJumpMenuItemIndexByLineIndex; 
					((jumpIndex < jumpItemCount)&&(jumpIndex <= mCacheForGetJumpMenuItemIndexByLineIndex+1));
					jumpIndex++ )
		{
			//jumpIndexのjump項目の開始段落取得
			AIndex	currentMenuParagraphIndex = 
					mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpIndex));
			//jumpIndexの次のjump項目の開始段落取得
			AIndex	nextMenuParagraphIndex = paragraphCount;
			if( jumpIndex+1 < jumpItemCount )
			{
				nextMenuParagraphIndex = 
						mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpIndex+1));
			}
			//範囲内に検索対象段落が入っているならそのjump項目を返す。
			if( paragraphIndex >= currentMenuParagraphIndex && paragraphIndex < nextMenuParagraphIndex ) 
			{
				mCacheForGetJumpMenuItemIndexByLineIndex = jumpIndex;
				return jumpIndex;
			}
		}
	}
	
	//2分法
	AIndex	startJumpIndex = 0;
	AIndex	endJumpIndex = jumpItemCount;
	AIndex	jumpIndex = (startJumpIndex+endJumpIndex)/2;
	for( AIndex i = 0; i < jumpItemCount*2; i++ )//無限ループ防止（多くとも行数回のループで終わるはず）
	{
		//jumpIndexのjump項目の開始段落取得
		AIndex	currentMenuParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpIndex));
		//jumpIndexの次のjump項目の開始段落取得
		AIndex	nextMenuParagraphIndex = paragraphCount;
		if( jumpIndex+1 < jumpItemCount )
		{
			nextMenuParagraphIndex = mTextInfo.GetParagraphIndexByGlobalIdentifier(mJump_IdentifierObjectArray.Get(jumpIndex+1));
		}
		//範囲内に検索対象段落が入っているならそのjump項目を返す。
		if( paragraphIndex >= currentMenuParagraphIndex && paragraphIndex < nextMenuParagraphIndex ) 
		{
			mCacheForGetJumpMenuItemIndexByLineIndex = jumpIndex;
			return jumpIndex;
		}
		//検索対象段落がjumpIndexの開始段落より前なら前の範囲で再帰的に検索
		if( paragraphIndex < currentMenuParagraphIndex )
		{
			endJumpIndex = jumpIndex;
			jumpIndex = (startJumpIndex+endJumpIndex)/2;
		}
		//検索対象段落が次の開始段落以降なら後ろの範囲で再帰的に検索
		else if( paragraphIndex >= nextMenuParagraphIndex )
		{
			startJumpIndex = jumpIndex+1;
			jumpIndex = (startJumpIndex+endJumpIndex)/2;
		}
		//startとendが入れ替わった場合の処理（発生しないはず）
		if( startJumpIndex >= endJumpIndex )
		{
			_ACError("",NULL);
			if( jumpIndex >= jumpItemCount )
			{
				jumpIndex = jumpItemCount-1;
			}
			mCacheForGetJumpMenuItemIndexByLineIndex = jumpIndex;
			return jumpIndex;
		}
	}
	_ACError("cannot find jump menu index",this);
	return kIndex_Invalid;
	/*#695 高速化のためmJump_IdentifierObjectArrayから2分法で該当するmenu identifierを見つけることにする
	AIndex	lineIndex = inLineIndex;
	if( lineIndex > SPI_GetLineCount() )   lineIndex = SPI_GetLineCount()-1;
	//行Indexに対応するメニュー登録IdentifierのUniqIDを得る
	AUniqID	identifierUniqID = mTextInfo.GetMenuIdentifierByLineIndex(lineIndex);
	if( identifierUniqID == kUniqID_Invalid )   return kIndex_Invalid;
	
	*#698
	//
	for( AIndex itemindex = 0; itemindex < mJump_IdentifierObjectArray.GetItemCount(); itemindex++ )
	{
		if( mJump_IdentifierObjectArray.Get(itemindex) == identifierUniqID )   return itemindex;
	}
	return kIndex_Invalid;
	*
	return mJump_IdentifierObjectArray.Find(identifierUniqID);
	*/
}

void	ADocument_Text::SPI_GetJumpMenuTextByMenuItemIndex( const AIndex inMenuItemIndex, AText& outText ) const
{
	mJump_MenuTextArray.Get(inMenuItemIndex,outText);
}

//R0212
void	ADocument_Text::SPI_GetJumpMenuItemInfoTextByLineIndex( const AIndex inLineIndex, AText& outText ) const
{
	AIndex	lineIndex = inLineIndex;
	if( lineIndex > SPI_GetLineCount() )   lineIndex = SPI_GetLineCount()-1;
	//行Indexに対応するメニュー登録IdentifierのUniqIDを得る
	//#695 AUniqID	identifierUniqID = mTextInfo.GetMenuIdentifierByLineIndex(lineIndex);
	//#695 if( identifierUniqID == kUniqID_Invalid )   return;
	AIndex	jumpIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);//#695
	if( jumpIndex != kIndex_Invalid )//#695
	{
		AUniqID identifierUniqID = mJump_IdentifierObjectArray.Get(jumpIndex);//#695
		mTextInfo.GetGlobalIdentifierInfoText(identifierUniqID,outText);
	}
}

//#454
/**
*/
void	ADocument_Text::SPI_RevealFromJumpMenuText( const AText& inText ) const
{
	AIndex	index = mJump_MenuTextArray.Find(inText);
	if( index == kIndex_Invalid )   return;
	
	if( mViewIDArray.GetItemCount() > 0 )
	{
		ATextPoint	spt,ept;
		SPI_GetGlobalIdentifierRange(mJump_IdentifierObjectArray.Get(index),spt,ept);
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_SetSelect(spt,ept);
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_AdjustScroll_Top();
	}
}

//
AObjectID	ADocument_Text::SPI_GetJumpMenuObjectID() const
{
	return mJumpMenuObjectID;
}

//
AUniqID	ADocument_Text::SPI_GetJumpMenuIdentifierUniqID( const AIndex inIndex ) const
{
	return mJump_IdentifierObjectArray.Get(inIndex);
}

//#723
/**
指定行を含むjump menu項目のデータを取得
*/
void	ADocument_Text::SPI_GetJumpMenuItemKeywordTextByLineIndex( const AIndex inLineIndex, 
		AText& outFunctionName, AText& outParentKeywordName, 
		AIndex& outCategoryIndex, ATextPoint& outStart, ATextPoint& outEnd ) const
{
	AIndex	lineIndex = inLineIndex;
	if( lineIndex > SPI_GetLineCount() )   lineIndex = SPI_GetLineCount()-1;
	//行Indexに対応するメニュー登録IdentifierのObjectIDを得る
	AUniqID	identifierUniqID = mTextInfo.GetMenuIdentifierByLineIndex(lineIndex);
	if( identifierUniqID != kUniqID_Invalid )
	{
		//識別子の各データ取得
		if( mTextInfo.GetGlobalIdentifierKeywordText(identifierUniqID,outFunctionName) == false )
		{
			//キーワードテキストが無ければ、メニューテキスト取得
			mTextInfo.GetGlobalIdentifierMenuText(identifierUniqID,outFunctionName);
		}
		//親キーワード取得
		mTextInfo.GetGlobalIdentifierParentKeywordText(identifierUniqID,outParentKeywordName);
		//カテゴリ取得
		outCategoryIndex = mTextInfo.GetGlobalIdentifierCategoryIndex(identifierUniqID);
		//識別子位置取得
		SPI_GetGlobalIdentifierRange(identifierUniqID,outStart,outEnd);
	}
}

//#906
/**
見出しパス取得
*/
void	ADocument_Text::SPI_GetHeaderPathArray( const AIndex inLineIndex, 
		ATextArray& outHeaderPathTextArray, AArray<AIndex>& outHeaderParagraphIndexArray ) const
{
	//結果初期化
	outHeaderPathTextArray.DeleteAll();
	outHeaderParagraphIndexArray.DeleteAll();
	//指定行に対応する見出しindex取得
	AIndex	jumpItemIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);
	if( jumpItemIndex == kIndex_Invalid )
	{
		return;
	}
	//見出しテキスト取得
	AText	headerText;
	mJump_MenuTextArray.Get(jumpItemIndex,headerText);
	outHeaderPathTextArray.Add(headerText);
	//見出し識別子位置取得
	AUniqID	uniqID = mJump_IdentifierObjectArray.Get(jumpItemIndex);
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetGlobalIdentifierRange(uniqID,spt,ept);
	outHeaderParagraphIndexArray.Add(SPI_GetParagraphIndexByLineIndex(spt.y));
	//アウトラインレベル取得
	AIndex	currentOutlineLevel = mJump_MenuOutlineLevelArray.Get(jumpItemIndex);
	//見出しを前方向に検索
	for( AIndex i = jumpItemIndex -1; i >= 0; i-- )
	{
		//アウトラインレベル取得
		AIndex	outlineLevel = mJump_MenuOutlineLevelArray.Get(i);
		//アウトラインレベルが小さくなったら、結果に追加
		if( outlineLevel < currentOutlineLevel )
		{
			//見出しテキスト取得
			mJump_MenuTextArray.Get(i,headerText);
			outHeaderPathTextArray.Add(headerText);
			//識別子位置取得
			AUniqID	uniqID = mJump_IdentifierObjectArray.Get(i);
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetGlobalIdentifierRange(uniqID,spt,ept);
			outHeaderParagraphIndexArray.Add(SPI_GetParagraphIndexByLineIndex(spt.y));
			//現在のアウトラインレベル更新
			currentOutlineLevel = outlineLevel;
		}
	}
}

//#875
/**
指定行を含む見出しのアウトラインレベル取得
*/
AIndex	ADocument_Text::SPI_GetOutlineLevel( const AIndex inLineIndex ) const
{
	//指定行に対応する見出しindex取得
	AIndex	jumpItemIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);
	if( jumpItemIndex == kIndex_Invalid )
	{
		return 0;
	}
	//アウトラインレベル取得
	return mJump_MenuOutlineLevelArray.Get(jumpItemIndex);
}

#pragma mark ===========================

#pragma mark --- ジャンプリスト管理
//#291

//カラムID
const ADataID			kColumn_Jump					= 0;

/**
ジャンプリストのListViewを登録
*/
void	ADocument_Text::SPI_RegisterJumpListView( const AViewID inJumpListViewID )
{
	mJumpListArray_ViewID.Add(inJumpListViewID);
	mJumpListArray_Sort.Add(false);
	//ジャンプリスト初期設定
	GetJumpListView(inJumpListViewID).SPI_RegisterColumn_Text(kColumn_Jump,0,"",false);
	GetJumpListView(inJumpListViewID).SPI_SetEnableCursorRow();
	//#725 UpdateJumpListProperty(inJumpListViewID);
	//ジャンプリスト内容作成
	SPI_UpdateJumpList();//#695 true);
}

/**
ジャンプリストのListViewを登録解除
*/
void	ADocument_Text::SPI_UnregisterJumpListView( const AViewID inJumpListViewID )
{
	AIndex	index = mJumpListArray_ViewID.Find(inJumpListViewID);
	if( index == kIndex_Invalid )   { _ACError("",this); return; }
	
	mJumpListArray_ViewID.Delete1(index);
	mJumpListArray_Sort.Delete1(index);
}

/**
ジャンプリストのListView取得
*/
AView_List&	ADocument_Text::GetJumpListView( const AViewID inViewID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_List,kViewType_List,inViewID);
}

//#695
/**
ジャンプリスト更新（行削除）
*/
void	ADocument_Text::SPI_UpdateJumpList_Deleted( const AIndex inIndex )
{
	//ソートボタン更新必要フラグ
	ABool	sortButtonShouldBeUpdated = false;
	//各ジャンプリストの更新
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		//ジャンプリストの行削除
		GetJumpListView(viewID).SPI_DeleteTableRow(inIndex);
		//内部データのソート状態解除（SPI_DeleteTableRow()内でソート解除されるため。）
		if( mJumpListArray_Sort.Get(i) == true )
		{
			mJumpListArray_Sort.Set(i,false);
			sortButtonShouldBeUpdated = true;
		}
	}
	//ソートボタン状態更新（ソート状態解除）
	if( sortButtonShouldBeUpdated == true )
	{
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AWindowID	textWindowID = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetTextWindowID();
			if( textWindowID != kObjectID_Invalid )
			{
				//★#725 GetApp().SPI_GetTextWindowByID(textWindowID).SPI_GetJumpListWindow().SPI_UpdateSortButtonStatus(GetObjectID(),false);
			}
		}
	}
	
	//選択はSPI_DeleteTableRow()で解除されるが、
	//編集時はキャレット設定時にNVIDO_SelectionChanged()からSPI_UpdateJumpListSelection()がコールされることにより、
	//スレッド文法認識中は、CopySyntaxRecognizerResult()からSPI_UpdateJumpListSelection()がコールされることにより、
	//現在のキャレット位置が再度選択される。
}

//#695
/**
ジャンプリスト更新（行挿入）
*/
void	ADocument_Text::SPI_UpdateJumpList_Inserted( const AIndex inIndex, const AItemCount inCount )
{
	//色取得
	AColor	subWindowNormalColor = kColor_List_Normal;//#1316 GetApp().SPI_GetSubWindowLetterColor();
	
	//ソートボタン更新必要フラグ
	ABool	sortButtonShouldBeUpdated = false;
	//各ジャンプリストの更新
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		//ジャンプリストの行挿入・設定
		GetJumpListView(viewID).SPI_InsertTableRows(inIndex,inCount,
													kTextStyle_Normal,subWindowNormalColor,false,kColor_White,false);
		for( AIndex index = inIndex; index < inIndex + inCount; index++ )
		{
			AText	menutext;
			mJump_MenuTextArray.Get(index,menutext);
			GetJumpListView(viewID).SPI_SetTableRow_Text(kColumn_Jump,index,
						menutext,mJump_MenuTextStyleArray.Get(index));
			GetJumpListView(viewID).SPI_SetTableRow_OutlineFoldingLevel(index,mJump_MenuOutlineLevelArray.Get(index));//#130
		}
		//内部データのソート状態解除（SPI_InsertTableRows()内でソート解除されるため）
		if( mJumpListArray_Sort.Get(i) == true )
		{
			mJumpListArray_Sort.Set(i,false);
			sortButtonShouldBeUpdated = true;
		}
	}
	//ソートボタン状態更新（ソート状態解除）
	if( sortButtonShouldBeUpdated == true )
	{
		for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
		{
			AWindowID	textWindowID = AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_GetTextWindowID();
			if( textWindowID != kObjectID_Invalid )
			{
				//★#725 GetApp().SPI_GetTextWindowByID(textWindowID).SPI_GetJumpListWindow().SPI_UpdateSortButtonStatus(GetObjectID(),false);
			}
		}
	}
	
	//選択はSPI_InsertTableRows()で解除されるが、
	//編集時はキャレット設定時にNVIDO_SelectionChanged()からSPI_UpdateJumpListSelection()がコールされることにより、
	//スレッド文法認識中は、CopySyntaxRecognizerResult()からSPI_UpdateJumpListSelection()がコールされることにより、
	//現在のキャレット位置が再度選択される。
}

//#695
/**
ジャンプリストの描画更新
*/
void	ADocument_Text::SPI_RefreshJumpList()
{
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		GetJumpListView(viewID).NVI_Refresh();
	}
}

/**
ジャンプリスト更新
*/
void	ADocument_Text::SPI_UpdateJumpList()//#695  const ABool inForceSetAll )
{
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		/*#695
		AIndex	svSelected = GetJumpListView(viewID).SPI_GetSelectedDBIndex();
		const ATextArray&	oldTextArray = GetJumpListView(viewID).SPI_GetColumn_Text(kColumn_Jump);
		if( mJump_MenuTextArray.GetItemCount() == oldTextArray.GetItemCount() && inForceSetAll == false )//R0006
		{
			//前とリストの個数が同じで、かつ、引数inForceSetAllがfalseの場合は、Textの違う箇所だけ更新する
			AItemCount	count = mJump_MenuTextArray.GetItemCount();
			for( AIndex i = 0; i < count; i++ )
			{ 
				if( mJump_MenuTextArray.GetTextConst(i).Compare(oldTextArray.GetTextConst(i)) == false )
				{
					GetJumpListView(viewID).SPI_SetTableRow_Text(kColumn_Jump,i,mJump_MenuTextArray.GetTextConst(i),mJump_MenuTextStyleArray.Get(i));
					GetJumpListView(viewID).SPI_SetTableRow_OutlineFoldingLevel(i,mJump_MenuOutlineLevelArray.Get(i));//#130
				}
			}
		}
		else
		*/
		{
			//上記以外の場合は全て更新
			GetJumpListView(viewID).SPI_SetSelect(kIndex_Invalid);
			GetJumpListView(viewID).SPI_BeginSetTable();
			GetJumpListView(viewID).SPI_SetColumn_Text(kColumn_Jump,mJump_MenuTextArray);
			GetJumpListView(viewID).SPI_SetTable_TextStyle(mJump_MenuTextStyleArray);
			GetJumpListView(viewID).SPI_SetTable_Color(mJump_MenuColorArray);//R0006
			GetJumpListView(viewID).SPI_SetTable_OutlineFoldingLevel(mJump_MenuOutlineLevelArray);//#130
			GetJumpListView(viewID).SPI_EndSetTable();
		}
		//ソート更新
		if( mJumpListArray_Sort.Get(i) == true )
		{
			GetJumpListView(viewID).SPI_Sort(kColumn_Jump,true);
		}
		/*#695
		//選択復元
		if( mJump_MenuTextArray.GetItemCount() == oldTextArray.GetItemCount() )
		{
			GetJumpListView(viewID).SPI_SetSelectByDBIndex(svSelected);
		}
		*/
		//ジャンプリストの選択更新
		AWindowID	winID = SPI_GetFirstWindowID();
		if( winID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(winID).SPI_UpdateJumpListSelection(true);
		}
		//描画更新
		GetJumpListView(viewID).NVI_Refresh();//win
	}
}

/**
ジャンプリスト選択更新
*/
void	ADocument_Text::SPI_UpdateJumpListSelection( const AIndex inLineIndex, const ABool inAdjustScroll )//#698
{
	AIndex	newIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);//#454
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		//#454 AIndex	newIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);
		if( newIndex != GetJumpListView(viewID).SPI_GetSelectedDBIndex() )
		{
			GetJumpListView(viewID).SPI_SetSelectByDBIndex(newIndex,inAdjustScroll);//#698
		}
		//#212
		//これがあると、filterを空にしたときにフォーカスがfilterから消えてしまうし、必要と思えないので削除GetJumpListView(viewID).NVI_RevealView(false);
	}
	//#454 ジャンプリスト履歴
	//前回の選択と変更があれば履歴更新
	if( newIndex != mLastSelectedJumpListItemIndex )
	{
		mLastSelectedJumpListItemIndex = newIndex;
		AFileAcc	file;
		if( newIndex != kIndex_Invalid && NVI_GetFile(file) == true )
		{
			GetApp().SPI_UpdateJumpListHistory(mJump_MenuTextArray.GetTextConst(newIndex),file);
		}
	}
}

/**
ジャンプリストソートモード設定
*/
void	ADocument_Text::SPI_SetJumpListSortMode( const AViewID inViewID, const ABool inSort )
{
	AIndex	index = mJumpListArray_ViewID.Find(inViewID);
	if( index != kIndex_Invalid )
	{
		if( mJumpListArray_Sort.Get(index) != inSort )
		{
			if( inSort == true )
			{
				GetJumpListView(inViewID).SPI_Sort(kColumn_Jump,true);
			}
			else
			{
				GetJumpListView(inViewID).SPI_SortOff();
			}
		}
		mJumpListArray_Sort.Set(index,inSort);
	}
}

/**
ジャンプリストフォント等設定（全て）
*/
/*#725
void	ADocument_Text::SPI_UpdateJumpListProperty()
{
	for( AIndex i = 0; i < mJumpListArray_ViewID.GetItemCount(); i++ )
	{
		AViewID	viewID = mJumpListArray_ViewID.Get(i);
		UpdateJumpListProperty(viewID);
	}
}
*/

/**
ジャンプリストフォント等設定（個別）
*/
/*#725
void	ADocument_Text::UpdateJumpListProperty( const AViewID inViewID )
{
	AText	fontname;
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kJumpListFontName,fontname);
	GetJumpListView(inViewID).SPI_SetTextDrawProperty(fontname,
			GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kJumpListFontSize),kColor_Black);
	GetJumpListView(inViewID).SPI_SetWheelScrollPercent(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
			GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));
	GetJumpListView(inViewID).NVI_Refresh();
}
*/

#pragma mark ===========================

#pragma mark ---インデント取得

//
AItemCount	ADocument_Text::SPI_CalcIndentCount( const AIndex inLineIndex, const AItemCount inPrevParagraphIndent, 
			AItemCount& outNextParagraphIndent,
			const AIndentTriggerType inIndentTriggerType )//#650
{
	//指定段落の開始行を取得
	AIndex	currentParagraphStartLineIndex = mTextInfo.GetCurrentParagraphStartLineIndex(inLineIndex);
	//指定段落のインデント量取得
	AItemCount	currentIndentCount = SPI_GetCurrentIndentCount(currentParagraphStartLineIndex);
	//次段落の開始行を取得
	AIndex	nextParagraphStartLineIndex = mTextInfo.GetNextParagraphStartLineIndex(inLineIndex);
	
	//インデント実行行の文法未認識の場合は、ここで（メインスレッド内で）認識する。
	//（#905により編集時の文法認識をしなくなったので、改行による新規行のインデントなど、文法認識がまだの場合がありうるため。）
	if( SPI_GetLineRecognized(inLineIndex) == false )
	{
		RecognizeSyntaxInMainThread(inLineIndex,inLineIndex+1);
		//inLineIndex+1まで限定で文法認識するので、#endif+改行追加時等、それ以降の行の状態変更もあるときには、上だけだと、状態変更されない。
		//そのため、下記のように、inLineIndex+1の認識済みフラグを落としておく。タイマー満了後等に、スレッドで認識される。
		if( inLineIndex+1 < SPI_GetLineCount() )
		{
			mTextInfo.ClearRecognizeFlags(inLineIndex+1);
		}
	}
	
	//指定行開始位置がコメント状態かどうかの判定
	AIndex	stateIndex = 0;
	AIndex	pushedStateIndex = 0;
	mTextInfo.GetLineStateIndex(inLineIndex,stateIndex,pushedStateIndex);
	ABool	isDisableIndentLine = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition().
			GetSyntaxDefinitionState(stateIndex).IsDisableIndent();
	ABool	isCommentLine = GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetSyntaxDefinition().
			GetSyntaxDefinitionState(stateIndex).IsCommentState();
	
	AIndentType	indentType = SPI_GetIndentType();
	
	//==================インデント無し==================
	//今のインデントを維持する
	if( indentType == kIndentType_NoIndent || isDisableIndentLine == true )
	{
		//今のインデントと同じインデント（インデント維持）
		return currentIndentCount;
	}
	
	//==================前行と同じインデント==================
	//指定段落の今のインデント量によって下記のインデントとなる
	//・0の場合：前行と同じインデント量
	//・0ではない場合：今のインデントと同じインデント（インデント維持）
	//下記のいずれかの場合に適用される
	//・ラジオボタンで「前行と同じインデントにする」を選択した場合
	//・ディレクティブによるdisable行
	//・コメントブロックの中の行（コメント状態で始まる行）
	if( indentType == kIndentType_SameAsPreviousParagraph || 
	   isCommentLine == true ||
	   mTextInfo.GetDisabledDirectiveLevel(inLineIndex) != kIndex_Invalid  )
	{
		if( inIndentTriggerType == kIndentTriggerType_ReturnKey_CurrentLine || 
		   inIndentTriggerType == kIndentTriggerType_AutoIndentCharacter )
		{
			//改行時の現在行インデントまたは自動インデント文字の場合、今のインデントと同じインデント（インデント維持）
			return currentIndentCount;
		}
		else if( currentIndentCount == 0 )
		{
			//------------------指定段落の今のインデント量が0の場合------------------
			//前行と同じインデント量
			if( currentParagraphStartLineIndex > 0 )
			{
				AIndex	prevParaLineIndex = mTextInfo.GetCurrentParagraphStartLineIndex(currentParagraphStartLineIndex-1);
				return SPI_GetCurrentIndentCount(prevParaLineIndex);
			}
			else
			{
				//最初の段落の場合は、今のインデントと同じインデント（インデント維持）
				return currentIndentCount;
			}
		}
		else
		{
			//------------------前行と同じインデント量が0ではない場合------------------
			//今のインデントと同じインデント（インデント維持）
			return currentIndentCount;
		}
	}
	
	//==================文法定義ファイルインデント==================
	if( indentType == kIndentType_SDFIndent )
	{
		//インデントは各行の行情報に含まれるが、編集後文法解析を旧インデントと新インデントが一致する行まで実施するわけにはいかない
		//（たとえば{入力で最終行まで文法解析必要になってしまう。）ので、インデント計算は適当なところまで戻って、そこから計算する。
		
		//インデント計算開始行を決定
		AIndex	startLineIndex = inLineIndex;
		AItemCount	indentCount = inPrevParagraphIndent;
		if( inPrevParagraphIndent == kIndex_Invalid )
 		{
			startLineIndex = currentParagraphStartLineIndex-1;
			if( startLineIndex < 0 )   startLineIndex = 0;
			//#695 AUniqID	menuIdentifierUniqID = mTextInfo.GetMenuIdentifierByLineIndex(inLineIndex);
			//#695 if( menuIdentifierUniqID != kUniqID_Invalid )
			AIndex	jumpIndex = SPI_GetJumpMenuItemIndexByLineIndex(inLineIndex);//#695
			if( jumpIndex != kIndex_Invalid )//#695
			{
				AUniqID menuIdentifierUniqID = mJump_IdentifierObjectArray.Get(jumpIndex);//#695
				startLineIndex = mTextInfo.GetLineIndexByGlobalIdentifier(menuIdentifierUniqID);
			}
			//B0340
			else
			{
				startLineIndex = GetPrevIndent0LineIndex(inLineIndex);
			}
			//#695 戻りすぎるとスローオペレーションになるので、戻りすぎ防止
			if( startLineIndex < inLineIndex - kLimit_SyntaxDefinitionIndent_MaxLineCountToGoBack )
			{
				startLineIndex = mTextInfo.GetPrevStableStateLineIndex(inLineIndex-kLimit_SyntaxDefinitionIndent_MaxLineCountToGoBack);
			}
			
			startLineIndex = mTextInfo.GetCurrentParagraphStartLineIndex(startLineIndex);
			
			//初期インデント数設定
			//#695 ここの処理は間違っているため下の処理に修正。テキスト内の現在行のインデントは現在行認識結果が既に反映されているため。
			//#695 indentCount = SPI_GetCurrentIndentCount(startLineIndex);
			indentCount = mTextInfo.GetIndentCount(startLineIndex) * SPI_GetIndentWidth();//#695
		}
		
		//fprintf(stderr,"s:%d,%d ",indentCount,startLineIndex);
		
		AIndex	lineIndex = startLineIndex;
		AItemCount	nextIndentCount = indentCount;
		//行毎のループ
		for( ; lineIndex < nextParagraphStartLineIndex; lineIndex++ )
		{
			//fprintf(stderr,"lineIndex:%d indent:%d\n",lineIndex,indentCount);
			//#467
			if( mTextInfo.GetDisabledDirectiveLevel(lineIndex) == kIndex_Invalid )
			{
				//
				AIndex	stateIndex = 0;
				AIndex	pushedStateIndex = 0;
				mTextInfo.GetLineStateIndex(lineIndex,stateIndex,pushedStateIndex);
				
				//fprintf(stderr,"l:%d,%d,%d,%d ",indentCount,nextIndentCount,lineIndex,stateIndex);
				ATextIndex	spos = SPI_GetLineStart(lineIndex);
				AIndex	endpos = spos + SPI_GetLineLengthWithLineEnd(lineIndex);
				for( AIndex pos = spos; pos < endpos; )
				{
					//pos位置から1トークン解析
					AIndex	nextStateIndex;
					ABool	changeStateFromNextChar;
					ATextIndex	nextPos;
					ABool	seqFound;
					AText	token;
					ATextIndex	tokenspos;
					mTextInfo.RecognizeStateAndIndent(	mText,stateIndex,pushedStateIndex,
														pos,endpos,nextStateIndex,changeStateFromNextChar,nextPos,seqFound,token,tokenspos,
														SPI_GetIndentWidth(),
														GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Number(AModePrefDB::kIndentForLabel),
														indentCount,nextIndentCount);
					//状態遷移
					stateIndex = nextStateIndex;
					//pos更新
					pos = nextPos;
				}
				//正規表現インデント
				//mTextInfo.CalcIndentByIndentRegExp(mText,spos,endpos,indentCount,nextIndentCount);
				//
			}
			
			if( lineIndex == nextParagraphStartLineIndex-1 )
			{
				outNextParagraphIndent = nextIndentCount;
				break;
			}
			//
			if( mTextInfo.GetLineExistLineEnd(lineIndex) == true )
			{
				indentCount = nextIndentCount;
			}
		}
		
		//fprintf(stderr,"e:%d ",indentCount);
		//==================statement途中判定==================
		AIndex	stateIndex = 0;
		AIndex	pushedStateIndex = 0;
		mTextInfo.GetLineStateIndex(currentParagraphStartLineIndex,stateIndex,pushedStateIndex);
		if( GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetSyntaxDefinitionState(stateIndex).IsIndentOnTheWay() == true )
		{
			indentCount += GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Number(AModePrefDB::kIndentOnTheWay);
			//------------------複文の最初の行を取得------------------
			AIndex	statementFirstLineIndex = currentParagraphStartLineIndex-1;
			for( ; statementFirstLineIndex >= 0; statementFirstLineIndex-- )
			{
				mTextInfo.GetLineStateIndex(statementFirstLineIndex,stateIndex,pushedStateIndex);
				if(	mTextInfo.GetCurrentParagraphStartLineIndex(statementFirstLineIndex) == statementFirstLineIndex &&
					GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().
					GetSyntaxDefinitionState(stateIndex).IsIndentOnTheWay() == false )
				{
					break;
				}
			}
			if( statementFirstLineIndex >= 0 )
			{
				//==================文の最初の行の2番目の字下げ位置にインデント==================
				if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kIndentOnTheWaySecondIndent) == true )
				{
					//------------------second indent取得------------------
					ATextIndex	spos = SPI_GetLineStart(statementFirstLineIndex);
					ATextIndex	epos = spos + SPI_GetLineLengthWithoutLineEnd(statementFirstLineIndex);
					ATextIndex pos = spos;
					//最初のtab/spaceを読み飛ばす
					for( ; pos < epos; pos++ )
					{
						AUChar	ch = mText.Get(pos);
						if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
						else break;
					}
					//tab以外を読み飛ばす
					for( ; pos < epos; pos++ )
					{
						AUChar	ch = mText.Get(pos);
						if( ch == kUChar_Tab )   break;
					}
					for( ; pos < epos; pos++ )
					{
						AUChar	ch = mText.Get(pos);
						if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
						else
						{
							//tab/spaceの位置のインデントを取得する
							CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
							GetTextDrawDataWithoutStyle(mText,mTextInfo,statementFirstLineIndex,textDrawData);//#695
							indentCount = textDrawData.OriginalTextArray_TabLetters.Get(pos-spos);
							break;
						}
					}
				}
				//==================括弧開始位置にインデント==================
				if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kIndentOnTheWay_ParenthesisStartPos) == true )
				{
					//括弧範囲取得
					ATextPoint	spt = {0}, ept = {0};
					spt.x = 0;
					spt.y = inLineIndex;
					ept = spt;
					if( SPI_GetBraceSelection(spt,ept,true) == true )
					{
						//括弧開始位置が、複文の開始行以降であれば、括弧開始位置をインデント量にする
						if( spt.y >= statementFirstLineIndex )
						{
							CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
							GetTextDrawDataWithoutStyle(mText,mTextInfo,spt.y,textDrawData);//#695
							indentCount = textDrawData.OriginalTextArray_TabLetters.Get(spt.x);
							//括弧の後にスペースがあればスペースの後にする
							for( ATextIndex	textIndex = SPI_GetTextIndexFromTextPoint(spt); textIndex < SPI_GetTextLength(); textIndex++ )
							{
								if( SPI_GetTextChar(textIndex) == kUChar_Space )
								{
									indentCount++;
								}
								else
								{
									break;
								}
							}
						}
					}
				}
				//==================文の最初の行の単一のコロンの位置を合わせる==================
				if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kIndentOnTheWay_SingleColon) == true )
				{
					AIndex	statementFirstLineColonPos = kIndex_Invalid;
					{
						//複文開始行のコロン位置を取得
						ATextIndex	spos = SPI_GetLineStart(statementFirstLineIndex);
						ATextIndex	epos = spos + SPI_GetLineLengthWithoutLineEnd(statementFirstLineIndex);
						ATextIndex pos = spos;
						for( ; pos < epos; pos++ )
						{
							AUChar	ch = mText.Get(pos);
							AUChar	ch2 = 0;
							if( pos+1 < mText.GetItemCount() )
							{
								ch2 = mText.Get(pos+1);
							}
							if( ch == ':' )
							{
								//次の文字がコロンではない（単一コロン）かどうかを判定
								if( ch2 != ':' )
								{
									//':'の位置を取得する
									CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
									GetTextDrawDataWithoutStyle(mText,mTextInfo,statementFirstLineIndex,textDrawData);//#695
									statementFirstLineColonPos = textDrawData.OriginalTextArray_TabLetters.Get(pos-spos) +1;
									break;
								}
								//コロン以外の文字まで読み進め
								for( ; pos < epos; pos++ )
								{
									if( mText.Get(pos) != ':' )
									{
										//posはコロン以外の文字の位置になる。その次の文字から外側のループ継続される。
										break;
									}
								}
							}
						}
					}
					if( statementFirstLineColonPos != kIndex_Invalid )
					{
						AIndex	currentLineColonPos = kIndex_Invalid;
						//指定行のコロン位置を取得
						ATextIndex	spos = SPI_GetLineStart(inLineIndex);
						ATextIndex	epos = spos + SPI_GetLineLengthWithoutLineEnd(inLineIndex);
						ATextIndex pos = spos;
						for( ; pos < epos; pos++ )
						{
							AUChar	ch = mText.Get(pos);
							AUChar	ch2 = 0;
							if( pos+1 < mText.GetItemCount() )
							{
								ch2 = mText.Get(pos+1);
							}
							if( ch == ':' )
							{
								//次の文字がコロンではない（単一コロン）かどうかを判定
								if( ch2 != ':' )
								{
									//':'の位置を取得する
									CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
									GetTextDrawDataWithoutStyle(mText,mTextInfo,inLineIndex,textDrawData);//#695
									currentLineColonPos = textDrawData.OriginalTextArray_TabLetters.Get(pos-spos) +1;
									break;
								}
								//コロン以外の文字まで読み進め
								for( ; pos < epos; pos++ )
								{
									if( mText.Get(pos) != ':' )
									{
										//posはコロン以外の文字の位置になる。その次の文字から外側のループ継続される。
										break;
									}
								}
							}
						}
						//コロン位置を合わせるようなインデント量にする
						if( currentLineColonPos != kIndex_Invalid )
						{
							AItemCount	lenBeforeColon = currentLineColonPos - SPI_GetCurrentIndentCount(inLineIndex);
							indentCount = statementFirstLineColonPos - lenBeforeColon;
						}
					}
				}
			}
		}
		return indentCount;
	}
	//正規表現インデント
	else
	{
		AItemCount indentCount = 0;
		AItemCount	nextIndentCount = 0;
		/*#650
		if( inUseCurrentParagraphIndentForRegExp == true )//B0363
		{
			nextIndentCount = SPI_GetCurrentIndentCount(currentParagraphStartLineIndex);
			indentCount = nextIndentCount;
		}
		else 
		*/
		//#650 正規表現インデントは「改行時の現在行インデントで、現在行が正規表現一致しなかった場合」以外は全て、適用する。
		//#650対応以前は、現在行が正規表現一致しない場合は正規表現インデントしないようにしていたが、
		//それだと、複数行を選択してインデント実行する場合に、インデント適用されない。前行も現在行も正規表現一致しない行を含めて
		//インデントするには、全ての行に「（前行と同じインデント）＋Σ（正規表現一致した分のインデント）」を適用する必要がある。
		//（正規表現一致しない行はΣの部分は０になるので、前行と同じインデントとなる。）
		if( currentParagraphStartLineIndex > 0 )
		{
			/*R0208 GetCurrentParagraphStartLineIndex()重いので
			AIndex	prevParagraphIndex = mTextInfo.GetParagraphIndexByLineIndex(currentParagraphStartLineIndex-1);
			AIndex	prevParagraphStartLineIndex = mTextInfo.GetLineIndexByParagraphIndex(prevParagraphIndex);*/
			AIndex	prevParagraphStartLineIndex = mTextInfo.GetCurrentParagraphStartLineIndex(currentParagraphStartLineIndex-1);
			nextIndentCount = SPI_GetCurrentIndentCount(prevParagraphStartLineIndex);
			indentCount = nextIndentCount;//B0324
			//fprintf(stderr,"prevpara:%d,%d ",prevParagraphIndex,indentCount);
			{
				ATextIndex	spos = SPI_GetLineStart(prevParagraphStartLineIndex);
				AIndex	endpos = SPI_GetLineStart(currentParagraphStartLineIndex);
				mTextInfo.CalcIndentByIndentRegExp(mText,spos,endpos,indentCount,nextIndentCount);
				indentCount = nextIndentCount;
			}
		}
		{
			//fprintf(stderr,"start:%d,%d ",currentParagraphStartLineIndex,indentCount);
			ATextIndex	spos = SPI_GetLineStart(currentParagraphStartLineIndex);
			AIndex	endpos = SPI_GetLineStart(nextParagraphStartLineIndex);
			AItemCount	svIndentCount = indentCount;//#650 現在行の正規表現によるインデント増減があるかどうかの判定のため、インデント記憶
			/*#650 ABool	regExpFound = */
			mTextInfo.CalcIndentByIndentRegExp(mText,spos,endpos,indentCount,outNextParagraphIndent);//#441
			//fprintf(stderr,"end:%d ",indentCount);
			//#650 AItemCount	currentLineLength = SPI_GetLineLengthWithoutLineEnd(currentParagraphStartLineIndex);//#441
			AItemCount	currentIndentCount = SPI_GetCurrentIndentCount(currentParagraphStartLineIndex);//#441
			//#650 if( currentLineLength == 0 || regExpFound == true )//#441
			if( (inIndentTriggerType == kIndentTriggerType_ReturnKey_CurrentLine || 
				 inIndentTriggerType == kIndentTriggerType_AutoIndentCharacter ) 
			   && (svIndentCount==indentCount) )//#650 現在行の正規表現によるインデント増減があるかどうかの判定
			{
				//改行時の現在行インデントまたは自動インデント文字の場合、現在行の正規表現によるインデント増減が無い場合は、インデント維持 #650
				//例えば</pre>のインデントが0で、次の行に手動でインデント作成し、<p>を打ちたい場合、現状インデント維持する必要がある。
				return currentIndentCount;
			}
			else
			//if( inIndentTriggerType != kIndentTriggerType_ReturnKey_CurrentLine || regExpFound == true )//#650
			{
				//インデント適用
				return indentCount;
			}
		}
	}
}

//B0340
//指定行以前の行（段落）で、インデントが0の行のインデックスを返す
AIndex	ADocument_Text::GetPrevIndent0LineIndex( const AIndex inLineIndex ) const
{
	for( AIndex lineIndex = inLineIndex-1; lineIndex > 0; lineIndex-- )
	{
		if( mTextInfo.GetLineExistLineEnd(lineIndex-1) == true )
		{
			if( SPI_GetCurrentIndentCount(lineIndex) <= 0 )   return lineIndex;
		}
	}
	return 0;
}

//
AItemCount	ADocument_Text::SPI_GetCurrentIndentCount( const AIndex inLineIndex ) const
{
	//#318
	AText	zenkakuSpaceText;
	zenkakuSpaceText.SetZenkakuSpace();
	ABool	zenkakuSpaceEnable = (GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == true &&
			GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kZenkakuSpaceIndent) == true);
	//
	ATextIndex	spos = SPI_GetLineStart(inLineIndex);
	ATextIndex	epos = spos + SPI_GetLineLengthWithoutLineEnd(inLineIndex);
	AItemCount	indentCount = 0;
	for( ATextIndex pos = spos; pos < epos; pos++ )
	{
		//#318
		if( zenkakuSpaceEnable == true )
		{
			AText	text;
			SPI_GetText1CharText(pos,text);
			if( text.Compare(zenkakuSpaceText) == true )
			{
				pos += text.GetItemCount()-1;
				indentCount += 2;
				continue;
			}
		}
		//
		AUChar	ch = SPI_GetTextChar(pos);
		if( ch == kUChar_Space )
		{
			indentCount++;
		}
		else if( ch == kUChar_Tab )
		{
			indentCount += SPI_GetTabWidth();
		}
		else break;
	}
	return indentCount;
}

//
void	ADocument_Text::GetIndentText( const AItemCount inIndentCount, AText& outText, const ABool inInputSpaces ) const//#249
{
	outText.DeleteAll();
	//#249 if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == false )
	if( inInputSpaces == false )//#249
	{
		AItemCount	tabCount = (inIndentCount/SPI_GetTabWidth());
		AItemCount	spaceCount = inIndentCount - (tabCount*SPI_GetTabWidth());
		for( AIndex i = 0; i < tabCount; i++ )
		{
			outText.Add(kUChar_Tab);
		}
		for( AIndex i = 0; i < spaceCount; i++ )
		{
			outText.Add(kUChar_Space);
		}
	}
	else
	{
		//#318
		if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kZenkakuSpaceIndent) == true )
		{
			AText	zenkakuSpaceText;
			zenkakuSpaceText.SetZenkakuSpace();
			AItemCount	zenkakuSpaceCount = inIndentCount/2;
			AItemCount	spaceCount = inIndentCount-zenkakuSpaceCount*2;
			for( AIndex i = 0; i < zenkakuSpaceCount; i++ )
			{
				outText.AddText(zenkakuSpaceText);
			}
			for( AIndex i = 0; i < spaceCount; i++ )
			{
				outText.Add(kUChar_Space);
			}
		}
		else
		{
			for( AIndex i = 0; i < inIndentCount; i++ )
			{
				outText.Add(kUChar_Space);
			}
		}
	}
}

//
AItemCount	ADocument_Text::SPI_Indent( const AIndex inLineIndex, const AItemCount inIndentCount, const ABool inInputSpaces,
									   const ABool inDontRedrawSubWindows )//#249
{
	AText	newIndentText;
	GetIndentText(inIndentCount,newIndentText,inInputSpaces);//#249
	AItemCount	insertedTextCount = 0;
	//最初のタブ・スペースの部分をspos～eposに入れる
	ATextIndex	spos = SPI_GetLineStart(inLineIndex);
	ATextIndex	epos = spos;
	//#318
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == true &&
					GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kZenkakuSpaceIndent) == true )
	{
		mText.SkipTabSpaceIncludingZenkakuSpace(epos,spos+mTextInfo.GetLineLengthWithoutLineEnd(inLineIndex));
	}
	else
	{
		mText.SkipTabSpace(epos,spos+mTextInfo.GetLineLengthWithoutLineEnd(inLineIndex));
	}
	//最初のタブ・スペースの部分の不一致箇所を探す
	ATextIndex pos = spos;
	for( ; pos < epos; pos++ )
	{
		if( pos-spos >= newIndentText.GetItemCount() )   break;
		AUChar	ch = SPI_GetTextChar(pos);
		if( ch != newIndentText.Get(pos-spos) )
		{
			break;
		}
	}
	//不一致箇所以降を削除
	if( pos < epos )
	{
		SPI_DeleteText(pos,epos,true,false,inDontRedrawSubWindows);
		insertedTextCount -= (epos-pos);
	}
	//不一致箇所以降を挿入
	newIndentText.Delete(0,pos-spos);
	if( newIndentText.GetItemCount() > 0 )
	{
		SPI_InsertText(pos,newIndentText,true,false,inDontRedrawSubWindows);
		insertedTextCount += newIndentText.GetItemCount();
	}
	return insertedTextCount;
}

//B0381
/**
インデントのタイプを取得する
*/
AIndentType	ADocument_Text::SPI_GetIndentType() const
{
	//文法定義インデント
	//※文法が手動設定のときは、文法定義インデント対象外
	//　（ブロック開始、終了に従ってインデントしたかったが、そうすると、ブロック開始、終了以外の部分のインデントが固定されてしまうので、
	//　　複文等の定義もできないと実用的ではない。従来、手動文法設定でインデントを文法定義に従うように設定していたユーザーの混乱をさけるため、現状通りにする）
	if( (GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseBuiltinSyntaxDefinitionFile) == true ||
		 GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseUserDefinedSDF) == true ) && 
				GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseSyntaxDefinitionIndent) == true )
	{
		return kIndentType_SDFIndent;
	}
	//正規表現インデント
	else if( (GetApp().SPI_GetModePrefDB(mModeIndex).GetItemCount_TextArray(AModePrefDB::kIndent_RegExp)>0) &&
				(GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kUseRegExpIndent) == true) )
	{
		return kIndentType_RegExpIndent;
	}
	//インデント無し
	else if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kNoIndent) == true )
	{
		return kIndentType_NoIndent;
	}
	//前行と同じインデント
	else
	{
		return kIndentType_SameAsPreviousParagraph;
	}
}

//B0000
AItemCount	ADocument_Text::SPI_GetTextInfoIndentCount( const AIndex inLineIndex ) const
{
	return mTextInfo.GetIndentCount(inLineIndex);
}

#pragma mark ===========================

#pragma mark ---折りたたみ
//#450

/**
*/
void	ADocument_Text::SPI_InitLineImageInfo()
{
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(i)).SPI_InitLineImageInfo();
	}
}

/**
*/
AFoldingLevel	ADocument_Text::SPI_GetFoldingLevel( const AIndex inLineIndex ) const
{
	if( SPI_IsParagraphStartLine(inLineIndex) == true )
	{
		return mTextInfo.GetFoldingLevel(inLineIndex);
	}
	else
	{
		return kFoldingLevel_None;
	}
}

//#493
/**
行のフォントサイズ倍率取得
*/
AFloatNumber	ADocument_Text::SPI_GetLineFontSizeMultiply( const AIndex inLineIndex ) const
{
	AFloatNumber	result = 1.0;
	if( SPI_GetFoldingLevel(inLineIndex) == kFoldingLevel_Header )
	{
		result *= mTextInfo.GetLineMultiply(inLineIndex);
		result /= 100;
	}
	return result;
}

/*#379
#pragma mark ===========================

#pragma mark <所有クラス(AWindow_Text)インターフェイス>

#pragma mark ===========================

#pragma mark ---処理通知コールバック
//ウインドウが閉じられた時に、ウインドウからコールされる
//mWindowIDArrayから対応ウインドウのIDを削除し、ウインドウが0個になったら、ドキュメントも削除する
void	ADocument_Text::OWICB_DoWindowClosed( const AWindowID inWindowID )
{
	AIndex	index = mWindowIDArray.Find(inWindowID);
	if( index == kIndex_Invalid )
	{
		_ACThrow("closed window not found",this);
	}
	mWindowIDArray.Delete1(index);
	if( mWindowIDArray.GetItemCount() == 0 )
	{
		GetApp().NVI_DeleteDocument(GetObjectID());
	}
}
*/

#pragma mark ===========================

#pragma mark ---CallGraf

//#723
/**
CallGrafウインドウに現在functionを設定
*/
void	ADocument_Text::SPI_SetCallGrafCurrentFunction( const AWindowID inTextWindowID, const AIndex inLineIndex, 
		const ABool inByEdit )
{
	//diff targetドキュメントであれば何もしない
	if( SPI_IsDiffTargetDocument() == true )
	{
		return;
	}
	
	//ローカル範囲開始識別子のデータを取得
	AText	funcname, classname;
	ATextPoint	menuidentifierSpt = kTextPoint_00, menuidentifierEpt = kTextPoint_00;
	AIndex	categoryIndex = kIndex_Invalid;
	SPI_GetLocalStartIdentifierDataByLineIndex(inLineIndex,funcname,classname,categoryIndex,menuidentifierSpt,menuidentifierEpt);
	if( funcname.GetItemCount() > 0 )
	{
		//ファイルパス取得
		AText	filepath;
		NVI_GetFilePath(filepath);
		//CallGrafに現在function設定
		GetApp().SPI_SetCallGrafCurrentFunction(GetObjectID(),
					mProjectFolder,mModeIndex,categoryIndex,filepath,classname,funcname,
					SPI_GetTextIndexFromTextPoint(menuidentifierSpt),
					SPI_GetTextIndexFromTextPoint(menuidentifierEpt),
					inByEdit);
	}
}

/**
コールグラフの項目の編集有りフラグを設定
*/
void	ADocument_Text::SPI_SetCallGrafEditFlag( const AWindowID inTextWindowID, const AIndex inLineIndex )
{
	//diff targetドキュメントであれば何もしない
	if( SPI_IsDiffTargetDocument() == true )
	{
		return;
	}
	
	//ローカル範囲開始識別子のデータを取得
	AText	funcname, classname;
	ATextPoint	menuidentifierSpt = kTextPoint_00, menuidentifierEpt = kTextPoint_00;
	AIndex	categoryIndex = kIndex_Invalid;
	SPI_GetLocalStartIdentifierDataByLineIndex(inLineIndex,funcname,classname,categoryIndex,menuidentifierSpt,menuidentifierEpt);
	if( funcname.GetItemCount() > 0 )
	{
		//ファイルパス取得
		AText	filepath;
		NVI_GetFilePath(filepath);
		//CallGrafのeditフラグ更新
		GetApp().SPI_SetCallGrafEditFlag(GetObjectID(),filepath,classname,funcname);
	}
}


#pragma mark ===========================

#pragma mark ---図表モード
//#478

//★未レビュー
/**
指定text pointのまわりに存在するテーブルの情報を取得
*/
ABool	ADocument_Text::SPI_GetSurroundingTable( const ATextPoint& inTextPoint, 
		AArray<AIndex>& outRowArray_LineIndex, AArray<ABool>& outRowArray_IsRuledLine,
		AArray<AIndex>& outRowArray_ColumnEndX ) const
{
	//
	outRowArray_LineIndex.DeleteAll();
	outRowArray_IsRuledLine.DeleteAll();
	outRowArray_ColumnEndX.DeleteAll();
	
	//指定textpointの段落の開始行を取得
	AIndex	targetParagraphStartLineIndex = SPI_GetCurrentParagraphStartLineIndex(inTextPoint.y);
	//開始行の最初の|の位置を取得
	AIndex	startXpos = kIndex_Invalid;
	AIndex	lineStart = SPI_GetLineStart(targetParagraphStartLineIndex);
	for( AIndex pos = lineStart; 
				pos < mText.GetItemCount(); 
				pos++ )
	{
		AUChar	ch = mText.Get(pos);
		if( ch == '|' )
		{
			startXpos = pos - lineStart;
			break;
		}
		if( ch == kUChar_LineEnd )
		{
			break;
		}
	}
	if( startXpos == kIndex_Invalid )
	{
		return false;
	}
	
	AItemCount	lineCount = SPI_GetLineCount();
	//上端を検索
	for( AIndex lineIndex = targetParagraphStartLineIndex; 
				lineIndex > 0;
				lineIndex = SPI_GetCurrentParagraphStartLineIndex(lineIndex-1) )//前の段落の開始行を取得
	{
		//
		if( startXpos >= SPI_GetLineLengthWithoutLineEnd(lineIndex) )
		{
			break;
		}
		else
		{
			AIndex	pos = SPI_GetLineStart(lineIndex) + startXpos;
			AUChar	ch = mText.Get(pos);
			if( ch == '|' || ch == '+' )
			{
				outRowArray_LineIndex.Insert1(0,lineIndex);
			}
			else
			{
				break;
			}
		}
	}
	
	//下端を検索
	for( AIndex lineIndex = SPI_GetNextParagraphStartLineIndex(targetParagraphStartLineIndex);
				lineIndex < lineCount;
				lineIndex = SPI_GetNextParagraphStartLineIndex(lineIndex) )
	{
		//
		if( startXpos >= SPI_GetLineLengthWithoutLineEnd(lineIndex) )
		{
			break;
		}
		else
		{
			AIndex	pos = SPI_GetLineStart(lineIndex) + startXpos;
			AUChar	ch = mText.Get(pos);
			if( ch == '|' || ch == '+' )
			{
				outRowArray_LineIndex.Add(lineIndex);
			}
			else
			{
				break;
			}
		}
	}
	
	//各テーブル構成行の、inTextPointより右側のカラム区切り位置を取得
	for( AIndex index = 0; index < outRowArray_LineIndex.GetItemCount(); index++ )
	{
		//
		AIndex	lineIndex = outRowArray_LineIndex.Get(index);
		//
		AIndex	lineStart = SPI_GetLineStart(lineIndex);
		AItemCount	len = SPI_GetLineLengthWithoutLineEnd(lineIndex);
		//
		if( inTextPoint.x >= len )
		{
			outRowArray_IsRuledLine.Add(false);
			outRowArray_ColumnEndX.Add(kIndex_Invalid);
		}
		else
		{
			AUChar	ch = mText.Get(lineStart + inTextPoint.x);
			if( ch == '-' || ch == '+' )
			{
				outRowArray_IsRuledLine.Add(true);
				outRowArray_ColumnEndX.Add(inTextPoint.x+1);
			}
			else
			{
				outRowArray_IsRuledLine.Add(false);
				//
				AIndex p = inTextPoint.x+1;
				for( ; p < len; p++ )
				{
					AUChar	ch = mText.Get(lineStart + p);
					if( ch == '|' )
					{
						break;
					}
				}
				outRowArray_ColumnEndX.Add(p);
			}
		}
	}
	
	//
	/*
	+------------+------+
	|  aa  aaa   |      |
	+------------+------+
	*/
	
	//
	if( outRowArray_LineIndex.GetItemCount() >= 2 )
	{
		return true;
	}
	else
	{
		outRowArray_LineIndex.DeleteAll();
		outRowArray_IsRuledLine.DeleteAll();
		outRowArray_ColumnEndX.DeleteAll();
		return false;
	}
	/*
	
	
	
	
	//
	AIndex	startLineCellStartPos = kIndex_Invalid;
	for( AIndex pos = GetTextIndexFromTextPoint(inTextPoint); pos >= 0; pos = mText.GetPrevCharPos(pos) )
	{
		AUChar	ch = mText.Get(pos);
		if( ch == '|' )
		{
			startLineCellStartPos = pos;
			break;
		}
		if( ch == kUChar_LineEnd )
		{
			break;
		}
	}
	if( startLineCellStartPos == kIndex_Invalid )
	{
		return;
	}
	ATextPoint	startLineCellStartTextPoint = {0};
	GetTextPointFromTextIndex(startLineCellStartPos,startLineCellStartTextPoint);
	//
	
	//
	AIndex	endLineCellStartPos = kIndex_Invalid;
	for( AIndex pos = GetTextIndexFromTextPoint(inTextPoint); pos < mText.GetItemCount(); pos = mText.GetNextCharPos(pos) )
	{
		AUChar	ch = mText.Get(pos);
		if( ch == '|' )
		{
			endLineCellStartPos = pos;
			break;
		}
		if( ch == kUChar_LineEnd )
		{
			return;
		}
	}
	if( endLineCellStartPos == kIndex_Invalid )
	{
		return;
	}
	ATextPoint	endLineCellStartTextPoint = {0};
	GetTextPointFromTextIndex(endLineCellStartPos,endLineCellStartTextPoint);
	//
	*/
}

#pragma mark ===========================

#pragma mark ---DocInfoウインドウ
//#142

/**
*/
void	ADocument_Text::UpdateDocInfoWindows()
{
	//
	AItemCount	totalCharCount = 0, totalWordCount = 0;
	mTextInfo.GetTotalWordCount(totalCharCount,totalWordCount);
	//
	GetApp().SPI_UpdateDocInfoWindows_TotalCount(GetObjectID(),totalCharCount,totalWordCount);
	//#1149
	//選択文字数、現在文字の表示の更新
	if( mViewIDArray.GetItemCount() > 0 )
	{
		AView_Text::GetTextViewByViewID(mViewIDArray.Get(0)).SPI_UpdateTextCountWindows();
	}
}

#pragma mark ===========================

#pragma mark ---ヒントテキスト
//

/**
ヒントテキスト追加
*/
AIndex	ADocument_Text::SPI_AddHintText( const ATextIndex inTextIndex, const AText& inHintText )
{
	return mTextInfo.AddHintText(inTextIndex,inHintText);
}

/**
ヒントテキスト削除
*/
void	ADocument_Text::SPI_ClearHintText()
{
	mTextInfo.ClearHintText();
}

/**
ヒントテキスト取得
*/
void	ADocument_Text::SPI_GetHintText( const ATextIndex inTextIndex, AText& outHintText ) const
{
	mTextInfo.GetHintText(inTextIndex,outHintText);
}

/**
指定位置のヒントテキストのindexを取得
*/
AIndex	ADocument_Text::SPI_FindHintIndexFromTextIndex( const ATextIndex inHintTextPos ) const
{
	return mTextInfo.FindHintIndexFromTextIndex(inHintTextPos);
}

/**
ヒントテキスト全削除（"?"も削除）
*/
void	ADocument_Text::SPI_DeleteAllHintText()
{
	AItemCount	itemCount = mTextInfo.GetHintTextCount();
	for( AIndex i = itemCount-1; i >= 0; i-- )//基本的に項目削除されるので後ろから
	{
		ATextIndex	textIndex = mTextInfo.GetHintTextPos(i);
		if( SPI_GetTextChar(textIndex) == kHintTextChar )
		{
			SPI_DeleteText(textIndex,textIndex+1);
		}
	}
}

#pragma mark ===========================

#pragma mark ---AppleScript用ドキュメントデータ取得

ABool	ADocument_Text::SPI_ASGetParagraph( const ATextIndex inStartTextIndex, const AIndex inParagraphOffsetIndex, 
		ATextIndex& outStart, ATextIndex& outEnd ) const
{
	ATextPoint	pt;
	SPI_GetTextPointFromTextIndex(inStartTextIndex,pt);
	AIndex	paragraphIndex = SPI_GetParagraphIndexByLineIndex(pt.y) + inParagraphOffsetIndex;
	
	if( SPI_GetLineIndexByParagraphIndex(paragraphIndex) >= SPI_GetLineCount() )   return false;
	
	outStart = SPI_GetParagraphStartTextIndex(paragraphIndex);
	outEnd = SPI_GetParagraphStartTextIndex(paragraphIndex+1);
	return true;
}

ABool	ADocument_Text::SPI_ASGetWord( const ATextIndex inStartTextIndex, const AIndex inWordOffsetIndex, 
		ATextIndex& outStart, ATextIndex& outEnd ) const
{
	AIndex	wordIndex = 0;
	AItemCount	textlength = SPI_GetTextLength();
	for( ATextIndex pos = inStartTextIndex; pos < textlength; )
	{
		if( mText.SkipTabSpaceLineEnd(pos,textlength) == false )   break;
		ATextIndex	start, end;
		SPI_FindWord(pos,start,end);
		if( wordIndex == inWordOffsetIndex )
		{
			outStart = start;
			outEnd = end;
			return true;
		}
		wordIndex++;
		pos = end;
	}
	return false;
}

ABool	ADocument_Text::SPI_ASGetChar( const ATextIndex inStartTextIndex, const AIndex inCharOffsetIndex, 
		ATextIndex& outStart, ATextIndex& outEnd ) const
{
	AIndex	charIndex = 0;
	AItemCount	textlength = SPI_GetTextLength();
	for( ATextIndex pos = inStartTextIndex; pos < textlength; )
	{
		if( charIndex == inCharOffsetIndex )
		{
			outStart = pos;
			outEnd = mText.GetNextCharPos(pos);
			return true;
		}
		pos = mText.GetNextCharPos(pos);
		charIndex++;
	}
	return false;
}

//B0331
ABool	ADocument_Text::SPI_ASGetInsertionPoint( const ATextIndex inStartTextIndex, const AIndex inOffsetIndex, ATextIndex& outPos ) const
{
	AIndex	insertionpointIndex = 0;
	AItemCount	textlength = SPI_GetTextLength();
	for( ATextIndex pos = inStartTextIndex; pos <= textlength; )
	{
		if( insertionpointIndex == inOffsetIndex )
		{
			outPos = pos;
			return true;
		}
		pos = mText.GetNextCharPos(pos);
		insertionpointIndex++;
	}
	return false;
}

//inStartTextIndex～inEndTextIndexの段落の個数を数える
//基本的にはinStartTextIndex～inEndTextIndexの改行コードの数を数える
/*
inStartTextIndexが段落の最初の場合は、inEndTextIndexの位置によって、返り値は以下のようになる
0111111111111111
1222222222222222
inStartTextIndexが段落の途中の場合は、inEndTextIndexの位置によって、返り値は以下のようになる
      0111111111
1222222222222222
inStartTextIndex == inEndTextIndexの場合は、返り値は0となるので、count of paragraphの場合のみコール側でなんとかすること。
index of paragraphでも使用するので、上記のような仕様にせざるを得ない。
*/
AItemCount	ADocument_Text::SPI_ASGetParagraphCount( const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex ) const
{
	AItemCount	count = 0;
	ATextIndex	currentParagraphStartPos = inStartTextIndex;//B0304(B0331) 071123やっぱり元に戻す kIndex_Invalid;//B0331 選択範囲が空の場合は1にするinStartTextIndex;
	ATextIndex pos = inStartTextIndex;
	for( ; pos < inEndTextIndex; pos++ )
	{
		if( mText.Get(pos) == kUChar_LineEnd )
		{
			count++;
			currentParagraphStartPos = pos+1;
		}
	}
	//ループが段落の最初で終了した場合以外はその部分も１段落に含める
	if( pos != currentParagraphStartPos )
	{
		count++;
	}
	return count;
}

AItemCount	ADocument_Text::SPI_ASGetWordCount( const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex ) const
{
	AItemCount	count = 0;
	for( ATextIndex pos = inStartTextIndex; pos < inEndTextIndex; )
	{
		if( mText.SkipTabSpaceLineEnd(pos,inEndTextIndex) == false )   break;
		ATextIndex	start, end;
		SPI_FindWord(pos,start,end);
		count++;
		pos = end;
	}
	return count;
}

AItemCount	ADocument_Text::SPI_ASGetCharCount( const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex ) const
{
	AItemCount	count = 0;
	for( ATextIndex pos = inStartTextIndex; pos < inEndTextIndex; )
	{
		pos = mText.GetNextCharPos(pos);
		count++;
	}
	return count;
}

void	ADocument_Text::SPI_GetCurrentStateIndexAndName( const ATextPoint& inTextPoint, 
		AIndex& outStateIndex, AIndex& outPushedStateIndex, 
		AText& outStateText, AText& outPushedStateText ) const
{
	mTextInfo.GetCurrentStateIndexAndName(mText,inTextPoint,outStateIndex,outPushedStateIndex,
				outStateText,outPushedStateText);
}

/**
指定テキストポイントの状態を取得
*/
AIndex	ADocument_Text::SPI_GetCurrentStateIndex( const ATextPoint& inTextPoint ) const
{
	AIndex	stateIndex = kIndex_Invalid;
	AIndex	pushedStateIndex = kIndex_Invalid;
	AText	stateName, pushedStateName;
	mTextInfo.GetCurrentStateIndexAndName(mText,inTextPoint,stateIndex,pushedStateIndex,stateName,pushedStateName);
	return stateIndex;
}

#pragma mark ===========================

#pragma mark --- Previewer
//#734

/**
プレビューワ更新
*/
void	ADocument_Text::SPI_UpdatePreview( const AIndex inLineIndex )
{
	AText	previewPluginText;
	if( GetApp().SPI_GetModePrefDB(SPI_GetModeIndex()).GetPreviewPlugin(previewPluginText) == true )
	{
		//==================プレビュープラグイン実行==================
		
		//未対応
	}
	else
	{
		/*#1257検討
		AText	modename;
		SPI_GetModeRawName(modename);
		if( modename.Compare("Markdown") == true )
		{
			AFileAcc	docfile;
			NVI_GetFile(docfile);
			AText	docpath;
			docfile.GetPath(docpath);
			//
			AFileAcc	tmpFolder;
			GetApp().SPI_GetTempFolder(tmpFolder);
			AFileAcc	tmpFile;
			tmpFile.SpecifyChildFile(tmpFolder,"markdown_preview.html");
			tmpFile.CreateFile();
			AText	tmpFilePath;
			tmpFile.GetPath(tmpFilePath);
			//
			{
				AText	command;
				command.AddCstring("/usr/local/bin/pandoc \"");
				command.AddText(docpath);
				command.AddCstring("\" -s -o \"");
				command.AddText(tmpFilePath);
				command.AddCstring("\"");
				AStCreateCstringFromAText	cstr(command);
				system(cstr.GetPtr());//asyncにしないと遅い
			}
			
			//ロード
			GetApp().SPI_RequestLoadToPreviewerWindow(SPI_GetFirstWindowID(),tmpFilePath);
			return;
		}
		*/
		/*
		AText	modename;
		SPI_GetModeRawName(modename);
		if( modename.Compare("TEX") == true )
		{
			
			AFileAcc	docfile;
			NVI_GetFile(docfile);
			AText	docpath;
			docfile.GetPath(docpath);
			//
			AText	filename;
			docfile.GetFilename(filename);
			AText	origsuffix;
			filename.GetSuffix(origsuffix);
			AText	filenamewithoutsuffix;
			filenamewithoutsuffix.SetText(filename);
			filenamewithoutsuffix.Delete(filename.GetItemCount()-origsuffix.GetItemCount(),origsuffix.GetItemCount());
			
			//
			AFileAcc	folder;
			folder.SpecifyParent(docfile);
			AText	folderpath;
			folder.GetPath(folderpath);
			//
			AStCreateCstringFromAText	dirPath(folderpath);
			chdir(dirPath.GetPtr());
			
			//
			{
				AText	command;
				GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kTexTypesetPath,command);
				command.AddCstring(" \"");
				command.AddText(docpath);
				command.AddCstring("\"");
				AStCreateCstringFromAText	cstr(command);
				system(cstr.GetPtr());
			}
			
			//
			AText	dvifilename;
			dvifilename.SetText(filenamewithoutsuffix);
			dvifilename.AddCstring(".dvi");
			AFileAcc	dvifile;
			dvifile.SpecifySister(docfile,dvifilename);
			//
			{
				AText	command;
				GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kTexDviToPdfPath,command);
				command.AddCstring(" \"");
				command.AddText(dvifilename);
				command.AddCstring("\"");
				AStCreateCstringFromAText	cstr(command);
				system(cstr.GetPtr());
			}
			//
			AText	pdffilename;
			pdffilename.SetText(filenamewithoutsuffix);
			pdffilename.AddCstring(".pdf");
			AFileAcc	pdffile;
			pdffile.SpecifyChild(folder,pdffilename);
			
			AText	url;
			pdffile.GetPath(url);
			//ロード
			GetApp().SPI_RequestLoadToPreviewerWindow(SPI_GetFirstWindowID(),url);
			return;
		}
		*/
		//==================標準プレビュー処理==================
		
		//現在行に変更があったら、現在アンカーを検索する
		if( mPreviewCurrentLineIndex != inLineIndex )
		{
			//現在アンカー初期化
			mPreviewCurrentAnchor.DeleteAll();
			//直近のアンカーのタイトルを取得
			AUniqID	anchorIdentifier = mTextInfo.GetAnchorIdentifierByLineIndex(inLineIndex);
			if( anchorIdentifier != kUniqID_Invalid )
			{
				mTextInfo.GetGlobalIdentifierKeywordText(anchorIdentifier,mPreviewCurrentAnchor);
			}
			//現在行更新
			mPreviewCurrentLineIndex = inLineIndex;
		}
		
		//URL取得
		AText	url;
		if( SPI_IsRemoteFileMode() == false )
		{
			//ローカルファイルのURL
			AFileAcc	file;
			NVI_GetFile(file);
			file.GetPath(url);
		}
		else
		{
			//リモートファイルのURL（httpのURLに変換）
			GetApp().GetAppPref().GetHTTPURLFromFTPURL(mRemoteFileURL,url);
		}
		//アンカー設定
		if( mPreviewCurrentAnchor.GetItemCount() > 0 )
		{
			url.AddCstring("#");
			url.AddText(mPreviewCurrentAnchor);
		}
		//ロード
		GetApp().SPI_RequestLoadToPreviewerWindow(SPI_GetFirstWindowID(),url);
	}
}

/**
プレビューワリロード
*/
void	ADocument_Text::SPI_ReloadPrevew()
{
	//リロード
	GetApp().SPI_RequestReloadToPreviewerWindow(SPI_GetFirstWindowID());
}

#pragma mark ===========================

#pragma mark --- LuaConsole
//#567

/**
Lua print関数出力
*/
void	ADocument_Text::SPI_Lua_Output( const AText& inText )
{
	//改行コード取得
	AText	lineEnd;
	lineEnd.Add(kUChar_LineEnd);
	//ViewID取得
	AViewID	viewID = mViewIDArray.Get(0);
	
	//テキスト最後のpt取得
	ATextPoint	pt = {0,0};
	SPI_GetTextPointFromTextIndex(mText.GetItemCount(),pt);
	//最後のptが行の途中なら、改行追加
	if( pt.x > 0 )
	{
		SPI_InsertText(mText.GetItemCount(),lineEnd);
	}
	//inTextをドキュメントの最後に追加
	AText	t(inText);
	SPI_InsertText(mText.GetItemCount(),t);
	
	//キャレット位置を最後尾に設定
	SPI_GetTextPointFromTextIndex(mText.GetItemCount(),pt);
	AView_Text::GetTextViewByViewID(viewID).SPI_SetSelect(pt,pt);
	AView_Text::GetTextViewByViewID(viewID).SPI_AdjustScroll_Center();
}

/**
Luaプロンプト表示
*/
void	ADocument_Text::SPI_Lua_Prompt()
{
	//現在のプロンプト位置がドキュメント最後ならそのまま
	if( mLuaPromptTextIndex == mText.GetItemCount() )   return;
	
	//改行コード取得
	AText	lineEnd;
	lineEnd.Add(kUChar_LineEnd);
	//ViewID取得
	AViewID	viewID = mViewIDArray.Get(0);
	
	//テキスト最後のpt取得
	ATextPoint	pt = {0,0};
	SPI_GetTextPointFromTextIndex(mText.GetItemCount(),pt);
	//最後のptが行の途中なら、改行追加
	if( pt.x > 0 )
	{
		SPI_InsertText(mText.GetItemCount(),lineEnd);
	}
	//プロンプトをドキュメントの最後に追加
	AText	prompt(">");
	SPI_InsertText(mText.GetItemCount(),prompt);
	
	//キャレット位置を最後尾に設定
	SPI_GetTextPointFromTextIndex(mText.GetItemCount(),pt);
	AView_Text::GetTextViewByViewID(viewID).SPI_SetSelect(pt,pt);
	AView_Text::GetTextViewByViewID(viewID).SPI_AdjustScroll_Center();
	
	//現在のプロンプト位置更新
	mLuaPromptTextIndex = mText.GetItemCount();
}

#pragma mark ===========================

#pragma mark ---ハッシュ検索用比較処理

//inTargetTextのinTargetIndex, inTargetItemCountの部分と比較して一致するかどうかを返す
ABool	ALocalIdentifierData::Compare( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	return GetKeywordText().Compare(inTargetText,inTargetIndex,inTargetItemCount);
}

//ハッシュ関数
template<> unsigned long	AHashObjectArray<ALocalIdentifierData,AText>::GetHashValue( const AText& inData, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	AItemCount	targetItemCount = inTargetItemCount;
	if( targetItemCount == kIndex_Invalid )
	{
		targetItemCount = inData.GetItemCount();
	}
	/*win 080726 if( targetItemCount == 0 )
	{
		return 12345;
	}
	else if( targetItemCount == 1 )
	{
		return inData.Get(inTargetIndex)*111;
	}
	else if( targetItemCount == 2 )
	{
		return inData.Get(inTargetIndex)*111 + inData.Get(inTargetIndex+1)*222;
	}
	else 
	{
		return inData.Get(inTargetIndex)*111 + inData.Get(inTargetIndex+1)*222 + inData.Get(inTargetIndex+2)*333 + targetItemCount*999;
	}*/
	if( targetItemCount == 0 )
	{
		return 1234567;
	}
	else if( targetItemCount == 1 )
	{
		return inData.Get(inTargetIndex)*67890123;
	}
	else if( targetItemCount == 2 )
	{
		return inData.Get(inTargetIndex)*78901234 + inData.Get(inTargetIndex+1)*891012345;
	}
	else 
	{
		return inData.Get(inTargetIndex)*8901234 + inData.Get(inTargetIndex+1)*9012345 
			+ inData.Get(inTargetIndex+targetItemCount-2)*1234567 + inData.Get(inTargetIndex+targetItemCount-1) + targetItemCount*4567;
	}
}

#pragma mark ===========================

#pragma mark ---

/**
整合性チェック
*/
void	ADocument_Text::SPI_CheckLineInfoDataForDebug()
{
	mTextInfo.CheckLineInfoDataForDebug();
}

void	ADocument_Text::OrderTextPoint( const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2, 
		ATextPoint& outStartTextPoint, ATextPoint& outEndTextPoint ) 
{
	if( inSelectTextPoint1.y < inSelectTextPoint2.y ) 
	{
		outStartTextPoint = inSelectTextPoint1;
		outEndTextPoint = inSelectTextPoint2;
	}
	else if( inSelectTextPoint1.y == inSelectTextPoint2.y )
	{
		if( inSelectTextPoint1.x < inSelectTextPoint2.x )
		{
			outStartTextPoint = inSelectTextPoint1;
			outEndTextPoint = inSelectTextPoint2;
		}
		else
		{
			outStartTextPoint = inSelectTextPoint2;
			outEndTextPoint = inSelectTextPoint1;
		}
	}
	else
	{
		outStartTextPoint = inSelectTextPoint2;
		outEndTextPoint = inSelectTextPoint1;
	}
}

#pragma mark ===========================

#pragma mark ---JavaScript用関数
//#904

/**
text indexからUnicode文字位置(JavaScriptでのtext index)に変換
*/
AIndex	ADocument_Text::SPI_GetUnicodeTextIndexFromTextIndex( const AIndex inTextIndex ) const
{
	AIndex	unicodeTextIndex = 0;
	//ポインタ取得
	AStMutexLocker	locker(mTextMutex);
	AStTextPtr	textptr(mText,0,mText.GetItemCount());
	AUChar*	p = textptr.GetPtr();
	//指定text indexまで、Unicode文字毎にunicodeTextIndexをインクリメント
	for( AIndex pos = 0; pos < inTextIndex; pos++ )
	{
		//UTF8開始文字ならunicodeTextIndexをインクリメント
		//Mac OS X 10.6.8でテストする限り、MacのJSエンジンはdecomposition文字はそれぞれを一文字として数えるので、decomp判定は不要
		if( (p[pos]&0xC0) != 0x80 )
		{
			unicodeTextIndex++;
		}
	}
	return unicodeTextIndex;
}

/**
Unicode文字位置(JavaScriptでのtext index)からtext indexに変換
*/
AIndex	ADocument_Text::SPI_GetTextIndexFromUnicodeTextIndex( const AIndex inUnicodeTextIndex ) const
{
	AIndex	unicodeTextIndex = 0;
	//ポインタ取得
	AStMutexLocker	locker(mTextMutex);
	AStTextPtr	textptr(mText,0,mText.GetItemCount());
	AUChar*	p = textptr.GetPtr();
	//指定Unicode文字位置まで、Unicode文字毎にunicodeTextIndexをインクリメント
	AItemCount	len = textptr.GetByteCount();
	for( AIndex pos = 0; pos < len; pos++ )
	{
		//UTF8開始文字ならunicodeTextIndexをインクリメントし、引数と一致したら、現在のposを返す
		if( (p[pos]&0xC0) != 0x80 )
		{
			//引数とUnicode文字位置とが一致したら、現在のposを返す
			if( unicodeTextIndex == inUnicodeTextIndex )
			{
				return pos;
			}
			//unicodeTextIndexをインクリメント
			unicodeTextIndex++;
		}
	}
	return len;
}

#pragma mark ===========================

#pragma mark ---プラグイン

/**
イベントリスナー実行 #994
*/
void	ADocument_Text::SPI_ExecuteEventListener( AConstCstringPtr inPtr, const AText& inParameter ) const
{
	//フォルダは空で実行（リスナー内ではモジュールロードはできないという仕様とする。）
	AFileAcc	folder;
	//このドキュメントのUniqIDを引数としてイベントリスナー実行
	AText	script;
	script.SetCstring("mi.executeEventListeners(mi.getDocumentByID(\"");
	script.AddNumber(GetApp().NVI_GetDocumentUniqID(GetObjectID()).val);
	script.AddCstring("\"),\"");
	script.AddCstring(inPtr);
	script.AddCstring("\",\"");
	script.AddText(inParameter);
	script.AddCstring("\");");
	GetApp().SPI_ExecuteJavaScript(script,folder);
}

#pragma mark ===========================

#pragma mark ---

/**
テキストバイト数取得
*/
AByteCount	ADocument_Text::SPI_GetTextDocumentMemoryByteCount() const
{
	if( mLoaded == true )
	{
		//ファイルロード後はテキストのサイズを返す
		return mText.GetItemCount();
	}
	else
	{
		//ファイル未ロード時は0を返す
		
		/*
		if( NVI_IsFileSpecified() == true )
		{
			AFileAcc	file;
			NVI_GetFile(file);
			return file.GetFileSize();
		}
		
		else
		 */
		{
			return 0;
		}
	}
}

/**
テキストエンコーディング推測テスト
*/
void	ADocument_Text::TestGuessTextEncoding()
{
	//
	ATextEncoding	tec;
	AText	text;
	
	//UTF8
	{
		text.SetText(mText);
		//
		AFloatNumber	utf8err = 0, eucerr = 0, sjiserr = 0;
		text.CalcUTF8ErrorRate(1024*1024,utf8err);
		text.CalcEUCErrorRate(1024*1024,eucerr);
		text.CalcSJISErrorRate(1024*1024,sjiserr);
		fprintf(stderr,"UTF8:UTF8Err:%.3f EUC:%.3f SJISErr:%.3f Result:",utf8err,eucerr,sjiserr);
		if( text.GuessTextEncodingByAnalysis(tec) == true )
		{
			AText	tecname;
			ATextEncodingWrapper::GetTextEncodingName(tec,tecname);
			tecname.OutputToStderr();
		}
		else
		{
			fprintf(stderr,"???");
		}
		fprintf(stderr,"\n");
	}
	
	//EUC
	{
		text.SetText(mText);
		text.ConvertFromUTF8(ATextEncodingWrapper::GetEUCTextEncoding());
		//
		AFloatNumber	utf8err = 0, eucerr = 0, sjiserr = 0;
		text.CalcUTF8ErrorRate(1024*1024,utf8err);
		text.CalcEUCErrorRate(1024*1024,eucerr);
		text.CalcSJISErrorRate(1024*1024,sjiserr);
		fprintf(stderr,"EUC:UTF8Err:%.3f EUC:%.3f SJISErr:%.3f Result:",utf8err,eucerr,sjiserr);
		if( text.GuessTextEncodingByAnalysis(tec) == true )
		{
			AText	tecname;
			ATextEncodingWrapper::GetTextEncodingName(tec,tecname);
			tecname.OutputToStderr();
		}
		else
		{
			fprintf(stderr,"???");
		}
		fprintf(stderr,"\n");
	}
	
	//SJIS
	{
		text.SetText(mText);
		text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
		//
		AFloatNumber	utf8err = 0, eucerr = 0, sjiserr = 0;
		text.CalcUTF8ErrorRate(1024*1024,utf8err);
		text.CalcEUCErrorRate(1024*1024,eucerr);
		text.CalcSJISErrorRate(1024*1024,sjiserr);
		fprintf(stderr,"SJIS:UTF8Err:%.3f EUC:%.3f SJISErr:%.3f Result:",utf8err,eucerr,sjiserr);
		if( text.GuessTextEncodingByAnalysis(tec) == true )
		{
			AText	tecname;
			ATextEncodingWrapper::GetTextEncodingName(tec,tecname);
			tecname.OutputToStderr();
		}
		else
		{
			fprintf(stderr,"???");
		}
		fprintf(stderr,"\n");
	}
}


