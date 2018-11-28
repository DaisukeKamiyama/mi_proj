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

#pragma once

#include "../../AbsFramework/Frame.h"
#include "AWindow_Text.h"
#include "AWindow_TextSheet.h"
#include "AWindow_JumpList.h"
#include "AWindow_TextCount.h"
#include "ATextInfo.h"
#include "ADocPrefDB.h"
#include "ATextUndoer.h"
#include "ATextFinder.h"
#include "AIdentifierList.h"
#include "AImportIdentifierLink.h"
#if IMPLEMENTATION_FOR_MACOSX
#include "AppleScript.h"
#endif
#include "ASCMFolderManager.h"//#568
#include "AThread_SyntaxRecognizer.h"
#include "AThread_WrapCalculator.h"
#include "AThread_CandidateFinder.h"
#import "TextDocument.h"

//#699
/**
ドキュメント初期化状態
*/
enum ADocumentInitState
{
	kDocumentInitState_Initial,
	kDocumentInitState_Initial_FileScreeningInThread,
	kDocumentInitState_Initial_FileScreened,
	kDocumentInitState_WrapCalculating,
	kDocumentInitState_SyntaxRecognizing,
	kDocumentInitState_Completed
};

//#361
/**
テキストドキュメントタイプ
*/
enum ATextDocumentType
{
	kTextDocumentType_Normal = 0,
	kTextDocumentType_RemoteFile,
	kTextDocumentType_DiffTarget
};

/*#379
enum ADiffDisplayMode
{
	kDiffDisplayMode_None = 0,
	kDiffDisplayMode_RepositoryLatest
};
*/
enum ADiffTargetMode
{
	kDiffTargetMode_None = 0,
	kDiffTargetMode_RepositoryLatest,
	kDiffTargetMode_File,
	kDiffTargetMode_TextWithRange,
};

//#81
/**
比較モード（ADiffTargetModeもより詳細な表示用の種別）
*/
enum ACompareMode
{
	kCompareMode_NoCompare,
	kCompareMode_AutoBackupMinutely,
	kCompareMode_AutoBackupDaily,
	kCompareMode_AutoBackupHourly,
	kCompareMode_AutoBackupMonthly,
	kCompareMode_Repository,
	kCompareMode_File,
	kCompareMode_Clipboard,
	kCompareMode_UnsavedData,
};

//#912
/**
インデントタイプ
*/
enum AIndentType
{
	kIndentType_NoIndent = 0,
	kIndentType_SameAsPreviousParagraph,
	kIndentType_SDFIndent,
	kIndentType_RegExpIndent,
};

/**
ファイルスクリーニングエラー
*/
enum AFileScreenError
{
	kFileScreenError_NoError = 0,
	kFileScreenError_FileNotExist,
	kFileScreenError_FileIsFolder,
	kFileScreenError_FileSizeOver,
	kFileScreenError_TotalFileSizeOver,
	kFileScreenError_DocumentDuplicatedAfterLinkResolved,
};

#pragma mark ===========================
#pragma mark [クラス]AAnalyzeDrawData
//RC1
//テキスト描画にオーバーラップして表示する解析データ格納用クラス
class AAnalyzeDrawData
{
  public:
	AAnalyzeDrawData()
	{
		Init();
	}
	void	Init()
	{
		//ArrowToDef
		flagArrowToDef = false;
		defStartArrowToDef.DeleteAll();
		defEndArrowToDef.DeleteAll();
		refStartArrowToDef.DeleteAll();
		refEndArrowToDef.DeleteAll();
		//ModuleComment
		flagModuleComment = true;
	}
	//ArrowToDef
	ABool					flagArrowToDef;
	AArray<ATextPoint>		defStartArrowToDef;
	AArray<ATextPoint>		defEndArrowToDef;
	AArray<ATextPoint>		refStartArrowToDef;
	AArray<ATextPoint>		refEndArrowToDef;
	//ModuleComment RC3
	ABool					flagModuleComment;
	AText					moduleComment;
};

#pragma mark ===========================
#pragma mark [クラス]ALocalIdentifierData
//ローカル識別子の情報を格納するクラス（識別子1つ分）
//AHashObjectArrayの要素として使用される。（mCurrentLocalIdentifier用）
class ALocalIdentifierData : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	ALocalIdentifierData( AObjectArrayItem* inParent, const AText& inKeywordText, const AIndex inCategoryIndex,
			const AText& inInfoText, const AIndex inStartIndex, const AIndex inEndIndex, const AText& inTypeText ) //RC1, RC2
	: AObjectArrayItem(inParent), mKeywordText(inKeywordText), mCategoryIndex(inCategoryIndex),
			mInfoText(inInfoText), mStartIndex(inStartIndex), mEndIndex(inEndIndex),//RC1
			mTypeText(inTypeText) {}//R0243
	~ALocalIdentifierData() {}
  private:
	AText						mKeywordText;
	AText						mInfoText;//RC2
	AIndex						mCategoryIndex;
	AIndex						mStartIndex;//RC1
	AIndex						mEndIndex;//RC1
	AText						mTypeText;//R0243
	
	//識別子情報取得
  public:
	const AText&	GetKeywordText() const { return mKeywordText; }
	const AText&	GetInfoText() const { return mInfoText; }//RC2
	AIndex			GetCategoryIndex() const { return mCategoryIndex; }
	AIndex			GetStartIndex() const { return mStartIndex; }//RC1
	AIndex			GetEndIndex() const { return mEndIndex; }//RC1
	const AText&	GetTypeText() const { return mTypeText; }//R0243
	
	//ハッシュ検索用比較処理
  public:
	ABool			Compare( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const;
	
	//AHashObjectArrayに保持されるときの検索キーを返すメソッド
  public:
	const AText&	GetPrimaryKeyData() const {return GetKeywordText();}
	
};


#pragma mark ===========================
#pragma mark [クラス]ALocalIdentifierDataFactory
//
class ALocalIdentifierDataFactory : public AAbstractFactoryForObjectArray<ALocalIdentifierData>
{
  public:
	ALocalIdentifierDataFactory( AObjectArrayItem* inParent, const AText& inKeywordText, const AIndex inCategoryIndex,
			const AText& inInfoText, const AIndex inStartIndex, const AIndex inEndIndex, const AText& inTypeText )//RC1, RC2, R0243
	: mParent(inParent), mKeywordText(inKeywordText), mCategoryIndex(inCategoryIndex),
			mStartIndex(inStartIndex), mEndIndex(inEndIndex), mInfoText(inInfoText), mTypeText(inTypeText)
	{
	}
	ALocalIdentifierData*	Create() 
	{
		return new ALocalIdentifierData(mParent,mKeywordText,mCategoryIndex,mInfoText,mStartIndex,mEndIndex,mTypeText);
	}
  private:
	AObjectArrayItem*			mParent;
	AText						mKeywordText;
	AText						mInfoText;//RC2
	AIndex						mCategoryIndex;
	AIndex						mStartIndex;//RC1
	AIndex						mEndIndex;//RC1
	AText						mTypeText;//R0243
};



#pragma mark ===========================
#pragma mark [クラス]ADocument_Text
//
class ADocument_Text : public ADocument
{
	//コンストラクタ、デストラクタ
  public:
	ADocument_Text( AObjectArrayItem* inParent, const AObjectID inDocImpID );//#1034
	~ADocument_Text();
	void					NVIDO_DocumentWillBeDeleted();//#379
	
	//初期設定
  public:
	void					SPI_Init_RemoteFile( const AObjectID inAccessPuginObjectID, 
							const AText& inFileURL, const AText& inDocumentText );//#361
	//#361 void					SPI_Init_FTP( const AText& inFTPURL, const AText& inDocumentText );
	void					SPI_Init_LocalFile( const AFileAcc& inFileAcc, const AText& inTextEncodingName );//win 080715 #212 #379
	void					SPI_Init_NewFile( const AModeIndex inModeIndex, const AText& inTextEncodingName, const AText& inDocumentText );
	void					SPI_Init_DiffTargetDocument( const AText& inTitle, const AText& inDocumentText,
							const AModeIndex inModeIndex, const AText& inTextEncodingName );//#379
	void					SPI_Init_DiffTargetDocument( const AText& inTitle, const AFileAcc& inFile,
														const AModeIndex inModeIndex, const AText& inTextEncodingName );//#379
	ABool					SPI_ShouldShowCorrectEncodingWindow( ATextEncodingFallbackType& outFallbackType ) const //#379 #473
	{
		outFallbackType = mShouldShowCorrectEncodingWindow_FallbackType;//#473
		return mShouldShowCorrectEncodingWindow;
	}
	ABool					SPI_IsLoaded() const { return mLoaded; }
	void					InitLineInfoData();//#699
	ABool					SPI_ScreenAndLoadDocumentFile();
	void					SPI_ScreenDocumentFile();
	ABool					SPI_IsJISTextEncoding() const;
	ABool					SPI_ShouldConvertFrom5CToA5ForCopy() const;//#1300
  private:
	void					LoadDocumentFile();//#831
	void					StartScreeningForLoadDocumentFile();
	void					ConvertToUTF8( AText& ioText );//#764
	//#473 ABool					GuessTextEncoding();
	//#940 void					Convert5CToA5WhenOpenJIS( AText& ioText );
	void					Init_Common();//win 080715 #212 #379
	void					SPI_Init_Text( const AText& inTitle, const AText& inDocumentText,
							const AModeIndex inModeIndex, const AText& inTextEncodingName );//#379
	//#379 ABool					IsInitDone() { return mInitDone; }
	//#379 ABool								mInitDone;
	ABool								mShouldShowCorrectEncodingWindow;//#379
	ATextEncodingFallbackType			mShouldShowCorrectEncodingWindow_FallbackType;//#473
	//#699 ABool								mViewDataInited;//#379
	ATextDocumentType					mTextDocumentType;//#361
	ABool								mTextEncodingIsDirectedByInitArgument;//#830
	ABool								mLoaded;//#831
	ABool								mSuspectBinaryData;//#703
	AFileAcc							mFileForDiffTargetDocument;//#764
	AText								mTextEncodingWhenLastLoadOrSave;//#81
	AFileScreenError					mFileScreenError;
	ADocumentID							mDuplicatedDocumentID;
	
	//#65
	//マルチファイル置換用のロード
  public:
	ABool					SPI_LoadForMultiFileReplace();
  private:
	ABool								mForMultiFileReplace;
	
	//#361
	//リモートファイル
  public:
  private:
	AObjectID							mRemoteConnectionObjectID;
	AText								mRemoteFileURL;
	AText								mTextOnRemoteServer;
	
	//ドキュメント初期化状態遷移
  public:
	void					SPI_TransitInitState_ViewActivated();//#699
	void					SPI_TransitInitState_WrapCalculateCompleted();//#699
	void					SPI_TransitInitState_SyntaxRecognizeCompleted();//#699
	ADocumentInitState		SPI_GetDocumentInitState() const 
	{ AStMutexLocker locker(mDocumentInitStateMutex); return mDocumentInitState; }
	void					SPI_SetDocumentInitState( const ADocumentInitState inState )
	{ AStMutexLocker locker(mDocumentInitStateMutex);  mDocumentInitState = inState; }
	void					SPI_ShowScreenErrorAndClose();//#962
  private:
	void					ReTransitInitState_Recalculate();
	mutable AThreadMutex						mDocumentInitStateMutex;
	ADocumentInitState					mDocumentInitState;//#699
	ABool								mCalculateWrapInThread;
	
  public:
	void					SPI_DoViewActivating( const ABool inCalculateWrapInThread = true );//#379
  private:
	void					InitViewData( const AViewID inViewID );//#379
	void					NVIDO_ViewRegistered( const AViewID inViewID );//#379
	void					NVIDO_ViewUnregistered( const AViewID inViewID );//#893
	//#0 未使用のため削除 ABool								mDiffInited;//#379
	
	//DocPrefからのデータ読み出し反映・DocPrefへのデータ保存
  private:
	void					ApplyDocPref_CollapsedLine();//#450
	void					SaveToDocPref_CollapsedLine();//#450
	void					ApplyDocPref_LineCheckedDate();//#842
	void					SaveToDocPref_LineCheckedDate();//#842
	
	//#898
	//doc pref
  public:
	void					SPI_SaveDocPrefIfLoaded();
  private:
	void					InitDocPref( const AModeIndex inModeIndex );
	AModeIndex				LoadDocPref();
	void					SaveDocPref();
	
	//#379
  public:
	ABool					SPI_ExistAnotherWindow( const AWindowID inWindowID );//#379
	
	/*#379
	//ウインドウ生成
  public:
	AObjectID				CreateWindowOrTab( const ABool inSelect = true );//#212
	void					SPI_RegisterWindow( const AWindowID inWindowID );
	AItemCount				SPI_GetWindowCount() const;
	*/
	
	//#567 LuaConsole
  public:
	void					SPI_Lua_Output( const AText& inText );
	void					SPI_Lua_Prompt();
	void					SPI_SetLuaConsoleMode() { mLuaConsoleMode = true; }
	ABool					SPI_GetLuaConsoleMode() const { return mLuaConsoleMode; }
	ATextIndex				SPI_GetLuaPromptTextIndex() const { return mLuaPromptTextIndex; }
  private:
	ABool								mLuaConsoleMode;
	ATextIndex							mLuaPromptTextIndex;
	
	//<関連オブジェクト取得>
  public:
	ADocPrefDB&				GetDocPrefDB() {return mDocPrefDB;}
	const ADocPrefDB&		GetDocPrefDBConst() const {return mDocPrefDB;}
  private:
	//#379 AWindow_Text&			GetTextWindowByIndex( const AIndex inIndex );
	ADocPrefDB							mDocPrefDB;
	
	/*#450
	//View登録 #199
  public:
	void					SPI_RegisterLineNumberView( const AViewID inViewID );
	void					SPI_UnregisterLineNumberView( const AViewID inViewID );
  private:
	AArray<AViewID>						mLineNumberViewIDArray;
	*/
	
	//TextView #138
	/*#199
  public:
	void					SPI_RegisterTextViewID( const AViewID inViewID );
	void					SPI_UnregisterTextViewID( const AViewID inViewID );
	*/
  private:
	//#199 AView_Text&				GetTextView( const AViewID inViewID );
	//#199 AArray<AViewID>						mTextViewIDArray;
	
	//<イベント処理>
  private:
	void					EVTDO_StartPrintMode( const ALocalRect& inRect, const ANumber inPaperWidth, const ANumber inPaperHeight );
	void					EVTDO_EndPrintMode();
	AItemCount				EVTDO_GetPrintPageCount();
	void					EVTDO_PrintPage( const AIndex inPageIndex );
	void					GetPrintFont_Text( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize );
	void					GetPrintRect_Text( ALocalRect& outRect );
	void					GetPrintFont_PageNumber( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize );
	void					GetPrintRect_PageNumber( ALocalRect& outRect );
	void					GetPrintFont_LineNumber( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize );
	void					GetPrintRect_LineNumber( ALocalRect& outRect );
	void					GetPrintFont_FileName( /*win AFont& outFont*/AText& outFontName, AFloatNumber& outFontSize );
	void					GetPrintRect_FileName( ALocalRect& outRect );
	/*win 080729 static const ANumber			kPrintLeftMargin = 30;
	static const ANumber			kPrintTopMargin = 0;//検討必要
	static const ANumber			kPrintRightMargin = 30;
	static const ANumber			kPrintBottomMargin = 30;
	static const ANumber			kPrintPageNumberHeight = 16;
	static const ANumber			kPrintLineNumberWidth = 40;
	static const ANumber			kPrintLineNumberRightMargin = 8;
	static const ANumber			kPrintFileNameHeight = 32;
	static const ANumber			kPrintSeparateLineHeight = 3;*/
	const AFloatNumber				mPrintLineNumberWidthMM;
	const AFloatNumber				mPrintLineNumberRightMarginMM;
	const AFloatNumber				mPrintFileNameHeightMM;
	const AFloatNumber				mPrintSeparateLineHeightMM;
	AIndex							mPrintStartParagraphIndex;//#524
	AIndex							mPrintEndParagraphIndex;//#524
	
	//<インターフェイス>
	//テキスト編集
  public:
	AItemCount				SPI_InsertText( const ATextPoint& inInsertPoint, AText& ioInsertText,
										   const ABool inRecognizeSyntaxAlwaysInThread = false, 
										   const ABool inDontRedraw = false,
										   const ABool inDontUpdateSubWindows = false);
	AItemCount				SPI_InsertText( const ATextIndex inInsertIndex, AText& ioInsertText,
										   const ABool inRecognizeSyntaxAlwaysInThread = false, 
										   const ABool inDontRedraw = false,
										   const ABool inDontUpdateSubWindows = false );
	AItemCount				SPI_DeleteText( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, 
										   const ABool inRecognizeSyntaxAlwaysInThread = false, 
										   const ABool inDontRedraw = false,
										   const ABool inDontUpdateSubWindows = false );
	AItemCount				SPI_DeleteText( const ATextIndex inStart, const ATextIndex inEnd, 
										   const ABool inRecognizeSyntaxAlwaysInThread = false, 
										   const ABool inDontRedraw = false,
										   const ABool inDontUpdateSubWindows = false );
	void					SPI_RecordUndoActionTag( const AUndoTag inTag );
	void					SPI_InsertLineEndAtWrap( const AIndex inStartLineIndex, const AIndex inEndLineIndex );
	void					SPI_DeferredRefreshAfterInsertDeleteText();
	void					SPI_DeferredRefreshSubWindowsAfterInsertDeleteText();
  private:
	AItemCount				AdjustLineInfo( const AIndex inStartLineIndex, const AIndex inEndLineIndex, AIndex& outUpdateEndLineIndex );
	
	//テキストに対する各種操作
  public:
	void					SPI_Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength );
	void					SPI_Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength );
	ABool					SPI_GetUndoText( AText& outText ) const;
	ABool					SPI_GetRedoText( AText& outText ) const;
	ABool					SPI_Save( const ABool inSaveAs );//#65
	void					SPI_Print();//#524
	void					SPI_PrintWithRange( const AIndex inStartParagraph, const AIndex inEndParagraph );//#524
  private:
	//#844 ABool					ConvertFromUTF8ToSJISUsingLosslessFallback( AText& outText ) const;//#473
	void					UpdateDirtyByUndoPosition();//#267
	void					NVIDO_SetDirty( const ABool inDirty );//#81
	AIndex								mLastSavedUndoPosition;//#267
	
	//自動保存 #81
  public:
	void					SPI_UpdateAutoBackupArray();
	void					SPI_AutoSave_FileBeforeSave( const ABool inAlways );
	void					SPI_UpdateComopareWithAutoBackupMenu();
	ABool					SPI_GetAutoBackupMinutelyFile( const AText& inFilename, AFileAcc& outFile );
	ABool					SPI_GetAutoBackupHourlyFile( const AText& inFilename, AFileAcc& outFile );
	ABool					SPI_GetAutoBackupDailyFile( const AText& inFilename, AFileAcc& outFile );
	ABool					SPI_GetAutoBackupMonthlyFile( const AText& inFilename, AFileAcc& outFile );
	static void				GetAutoBackupNumberArray( const AFileAcc& inFolder, AArray<ANumber64bit>& outArray );
  private:
	void					GetAutoBackupCompareMenuText( const ANumber64bit inNumber, AText& outText ) const;
	ABool					GetAutoBackupFolder( AFileAcc& outMonthlyFolder, AFileAcc& outDailyFolder,
							AFileAcc& outHourlyFolder, AFileAcc& outMinutelyFolder ) const;
	void					GetPathTextForAutoBackupAndDocPref( AText& outPath ) const;
	static void				GetAutoSaveCurrentDateTimeText( AText& outText );
	ABool								mAutoSaveDirty;
	AArray<ANumber64bit>				mAutoBackupMonthlyArray_Number;
	AArray<ANumber64bit>				mAutoBackupDailyArray_Number;
	AArray<ANumber64bit>				mAutoBackupHourlyArray_Number;
	AArray<ANumber64bit>				mAutoBackupMinutelyArray_Number;
	
	//未保存バックアップ #81
  public:
	void					SPI_AutoSave_Unsaved();
  private:
	void					AutoSave_RemoveUnsavedDataFile();
	void					CreateUnsavedDataBackupFolder();
	void					DeleteUnsavedDataBackupFolder();
	AFileAcc							mAutoBackupUnsavedDataFolder;
	AArray<ANumber64bit>				mAutoBackupUnsavedDataArray_Number;
	
//テキスト情報取得
  public:
	void					SPI_GetTextDrawDataWithoutStyle( const AIndex inLineIndex, CTextDrawData& outTextDrawData ) const;
	void					SPI_GetTextDrawDataWithStyle( const AIndex inLineIndex, CTextDrawData& outTextDrawData,
							const ABool inSelectMode, const ATextPoint& inSelectStartTextPoint, const ATextPoint& inSelectEndTextPoint,
							AAnalyzeDrawData& ioAnalyzeDrawData, const AViewID inViewID ) const;//RC1
	void					SPI_GetText( const ATextIndex inStart, const ATextIndex inEnd, AText& outText ) const;
	void					SPI_GetText( const ATextPoint& inStart, const ATextPoint& inEnd, AText& outText ) const;
	void					SPI_GetText( AText& outText ) const;
	void					SPI_GetLineText( const AIndex inLineIndex, AText& outText ) const;//#450
	AIndex					SPI_GetNextLineEndCharIndex( const AIndex inStartTextIndex ) const;//#750
	AIndex					SPI_GetPrevCharTextIndex( const ATextIndex inIndex, const ABool inSkipCanonicalDecomp = true,
													 const ABool inReturnMinusWhen0 = false ) const;//#863
	AIndex					SPI_GetNextCharTextIndex( const ATextIndex inIndex, const ABool inSkipCanonicalDecomp = true ) const;//#863
	AIndex					SPI_GetCurrentCharTextIndex( const ATextIndex inIndex ) const;
	ATextIndex				SPI_GetParagraphStartTextIndexFromLineIndex( const AIndex inLineIndex ) const;
	AIndex					SPI_GetCurrentParagraphStartLineIndex( const AIndex inLineIndex ) const;
	AIndex					SPI_GetNextParagraphStartLineIndex( const AIndex inLineIndex ) const;
	ATextIndex				SPI_GetParagraphStartTextIndex( const AIndex inParagraphIndex ) const;
	AIndex					SPI_GetParagraphIndexByLineIndex( const AIndex inLineIndex ) const;
	AIndex					SPI_GetLineIndexByParagraphIndex( const AIndex inParagraphIndex ) const;
	void					SPI_GetParagraphTextByLineIndex( const AIndex inLineIndex, AText& outText ) const;//#465
	void					SPI_GetParagraphText( const AIndex inParagraphIndex, AText& outText ) const;//#379
	AItemCount				SPI_GetParagraphLengthWithoutLineEnd( const AIndex inParagraphIndex ) const;//#996
	AItemCount				SPI_GetParagraphCount() const;
	ABool					SPI_IsParagraphStartLine( const AIndex inLineIndex ) const;
	ABool					SPI_IsUTF8Multibyte( const ATextIndex inIndex ) const;
	AUChar					SPI_GetTextChar( const ATextIndex inIndex ) const;
	void					SPI_GetText1CharText( const ATextIndex inIndex, AText& outText ) const;
	AUCS4Char				SPI_GetText1UCS4Char( const ATextIndex inIndex ) const;//#142
	void					SPI_GetText1UCS4Char( const ATextIndex inIndex, AUCS4Char& outChar, AUCS4Char& outCanonicalDecomp2ndChar ) const;//#142
	ABool					SPI_FindWord( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const;
	ABool					SPI_FindWordForKeyword( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const;//B0424
	void					SPI_FindQuadWord( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const;
	ABool					SPI_IsAsciiAlphabet( const ATextIndex inPos ) const;
	ATextIndex				SPI_GetTextIndexByUTF8CharOffset( const ATextIndex inTextIndex, const AItemCount inCharOffset ) const;
	
	AItemCount				SPI_GetLineCount() const;
	AIndex					SPI_GetLineLengthWithoutLineEnd( const AIndex inLineIndex ) const;
	AIndex					SPI_GetLineLengthWithLineEnd( const AIndex inLineIndex ) const;
	ABool					SPI_GetLineExistLineEnd( const AIndex inLineIndex ) const;
	ATextIndex				SPI_GetLineStart( const AIndex inLineIndex ) const;
	ABool					SPI_GetLineRecognized( const AIndex inLineIndex ) const 
	{ return mTextInfo.GetLineRecognized(inLineIndex); }
	void					SPI_GetGlobalIdentifierRange( const AUniqID inIdentifierUniqID, ATextPoint& outStart, ATextPoint& outEnd ) const;
	void					SPI_UpdateCurrentIdentifierDefinitionList( const AText& inText );
	/*#717
	void					SPI_GetAbbrevCandidate( const AText& inText, ATextArray& outAbbrevCandidateArray,
							ATextArray& outInfoTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
							AObjectArray<AFileAcc>& outFileArray ) const;//RC2
	void					SPI_GetAbbrevCandidateByParentKeyword( const AText& inParentKeyword, ATextArray& outAbbrevCandidateArray,
							ATextArray& outInfoTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
							AObjectArray<AFileAcc>& outFileArray ) const;//R0243
	*/
	ABool					SPI_GetTypeOfKeyword( const AText& inText, AText& outType, const AViewID inViewID ) const;//R0243
	//#725 void					SPI_GetInfoText( const AText& inText, AText& outInfoText ) const;
	void					SPI_GetIdInfoArray( const AText& inText, ATextArray& outInfoTextArray, AObjectArray<AFileAcc>& outFileArray,
							ATextArray& outCommentTextArray, AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
							AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
							ATextArray& outParentKeywordArray ) const;//RC2
	ABool					SPI_GetBraceSelection( ATextPoint& ioStart, ATextPoint& ioEnd, const ABool inLimitSearchRange = false ) const;
	ABool					SPI_CheckBrace( const ATextPoint& inCaretTextPoint, ATextPoint& outBraceTextPoint, ABool& outCheckOK ) const;
	ABool					SPI_GetCorrespondText( const ATextPoint& inCaretTextPoint, AText& outText, ATextPoint& outStart, ATextPoint& outEnd ) const;//R0000
	ABool					SPI_GetCorrespondenceSelection( const ATextPoint& inStart, const ATextPoint& inEnd,
														   ATextPoint& outStart, ATextPoint& outEnd ) const;//#359
	ABool					CalcCorrespond( const ATextPoint& inStart, const ATextPoint& inEnd, 
										   const ABool inFindForCorrespondInput, AText& outCorrespondText,
										   ATextPoint& outStart, ATextPoint& outEnd ) const;
	ABool					SPI_IsCodeChar( const ATextPoint& inPoint ) const;
	void					SPI_ReportTextEncodingErrors();
	void					SPI_ReportSpellCheck();//R0199
	void					SPI_GetTextCount( ATextCountData& ioTextCount, const AArray<ATextIndex>& inStart, const AArray<ATextIndex>& inEnd );
	void					SPI_GetTextForOpenSelected( const ATextPoint& inSelectStart, const ATextPoint& inSelectEnd, AText& outPathText ) const;
	void					SPI_GetPositionTextByLineIndex( const AIndex inLineIndex, AText& outText ) const;
	void					SPI_SetLineCheckedDate( const AIndex inLineIndex, const ANumber inDate )//#842
	{ mTextInfo.SetLineCheckedDate(inLineIndex,inDate); }
	ANumber					SPI_GetLineCheckedDate( const AIndex inLineIndex ) const//#842
	{ return mTextInfo.GetLineCheckedDate(inLineIndex); }
	void					SPI_GetWordCount( const ATextPoint& inStartTextPoint, const ATextPoint& inEndTextPoint,
											 AItemCount& outCharCount, AItemCount& outWordCount, AItemCount& outParagraphCount ) const;//#142
	ABool					SPI_GetSyntaxWarning( const AIndex inLineIndex ) const;
	ABool					SPI_GetSyntaxError( const AIndex inLineIndex ) const;
	void					SPI_ClearCheckerPluginWarnings();//#992
	void					SPI_AddCheckerPluginWarning( const ANumber inParagraphNumber, const AIndex inColIndex,
													 const AText& inTitleText, const AText& inDetailText,
													 const AIndex inTypeIndex, const ABool inDisplayInTextView );//#992
	void					SPI_GetCheckerPluginWarnings( const ANumber inParagraphNumber, 
													  ANumberArray& outColIndexArray, 
													  ATextArray& outTitleTextArray, ATextArray& outDetailTextArray,
													  AColorArray& outColorArray, ABoolArray& outDisplayInTextViewArray ) const;//#992
  private:
	ABool					GetKeywordToken( AIndex& ioPos, const AIndex inEndPos, AText& outToken, AIndex& outTokenStartPos,
							const AIndex inSyntaxPartIndex ) const;
	void					GetTextDrawDataWithoutStyle( const AText& inText, const ATextInfo& inTextInfo, const AIndex inLineIndex, CTextDrawData& outTextDrawData ) const;//#695
	void					GetTextDrawDataWithStyle( const AText& inText, const ATextInfoForDocument& inTextInfo, const AIndex inLineIndex, CTextDrawData& outTextDrawData, //#695
							const ABool inSelectMode, const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2,
							AAnalyzeDrawData& ioAnalyzeDrawData, const AViewID inViewID ) const;//RC1
	//ABool					FindWord( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd, const ABool useMiWordSelect ) const;//B0424
	void					GetText( const ATextIndex inStart, const ATextIndex inEnd, AText& outText ) const;
	AHashArray<ANumber>					mCheckerPlugin_ParagraphNumber;//#992
	AArray<AIndex>						mCheckerPlugin_ColIndex;//#992
	ATextArray							mCheckerPlugin_TitleText;//#992
	ATextArray							mCheckerPlugin_DetailText;//#992
	AArray<AIndex>						mCheckerPlugin_TypeIndex;//#992
	ABoolArray							mCheckerPlugin_DisplayInTextView;//#992
	
	//TextPoint/TextIndex変換
  public:
	AIndex					SPI_GetTextIndexFromTextPoint( const ATextPoint& inPoint ) const;
	void					SPI_GetTextPointFromTextIndex( const AIndex inTextPosition, ATextPoint& outPoint, const ABool inPreferNextLine = false ) const;
	
	//ドキュメント情報設定／取得
  public:
	void					SPI_GetURL( AText& outURL ) const;//#994
	void					UpdateDocumentByFontChange();//#868
	void					SPI_GetFilename( AText& outFilename ) const;
	void					SPI_DoWrapRelatedDataChanged( const ABool inForceRecalc );
	//win void					SPI_SetFont( const AFont inFont );
	void					SPI_SetFontName( const AText& inText );//win
	//win AFont					SPI_GetFont() const;
	void					SPI_GetFontName( AText& outText ) const;//win
	void					SPI_SetFontSize( const AFloatNumber inFontSize );
	AFloatNumber			SPI_GetFontSize() const;
	void					SPI_SetFontNameAndSize( const AText& inFontName, const AFloatNumber inFontSize );//#354
	void					SPI_SetUseModeFont( const ABool inUseModeFont );//#868
	ABool					SPI_GetUseModeFont() const;//#868
	void					SPI_TextFontSetInModeIsUpdated( const AIndex inUpdatedModeIndex );//#868
	void					SPI_RevertFontNameAndSize();//#354
	//#688 #844 ATextEncoding			SPI_GetPreferLegacyTextEncoding() const;//win
	//#844 void					SPI_SetTabWidth( const ANumber inTabWidth );
	ANumber					SPI_GetTabWidth() const;
	AItemCount				SPI_GetIndentWidth() const;
	//#844 void					SPI_SetIndentWidth( const AItemCount inIndentWidth );
	ATextEncoding			SPI_GetTextEncoding() const;
	void					SPI_GetTextEncoding( AText& outTextEncodingName ) const;
	void					SPI_SetTextEncoding( const AText& inTextEncodingName );
	//#934 void					SPI_SetTextEncoding( const ATextEncoding inTextEncoding );
	void					SPI_ModifyTextEncoding( const AText& inTextEncodingName, ATextEncodingFallbackType& outResultFallbackType ) ;//#473
	//#844 void					SPI_SetAntiAliasMode( const ANumber inMode );
	//#844 AAntiAliasMode			SPI_GetAntiAliasMode() const;
	ABool					SPI_IsAntiAlias() const;
	void					SPI_SetModeByRawName( const AText& inModeName );
	void					SPI_SetModeByDisplayName( const AText& inModeName );
	void					SPI_SetMode( const AModeIndex inModeIndex );
	void					SPI_UpdateTextInfoRegExp();//#683
	AModeIndex				SPI_GetModeIndex() const { AStMutexLocker	locker(mModeIndexMutex); return mModeIndex; }
	void					SPI_GetModeRawName( AText& outText ) const;
	void					SPI_GetModeDisplayName( AText& outText ) const;
	void					SPI_SetReturnCode( const AReturnCode inReturnCode );
	AReturnCode				SPI_GetReturnCode() const;
	void					SPI_SetWrapMode( const AWrapMode inWrapMode, const ANumber inLetterCount );
	AWrapMode				SPI_GetWrapMode() const;
	ANumber					SPI_GetWrapLetterCount() const;
	ABool 					SPI_IsRemoteFileMode() const;
	ANumber					SPI_GetLineMargin() const;
	ABool					SPI_GetParentFolder( AFileAcc& outFolder ) const;
	void					SPI_GetRemoteFileURL( AText& outRemoteFileURL ) const;
	void					SPI_SetTemporaryTitle( const AText& inText );
	void					SPI_GetTemporaryTitle( AText& outText ) const { outText.SetText(mTemporaryTitle); }
	void					SPI_GetProjectFolder( AFileAcc& outProjectFolder ) const { outProjectFolder.CopyFrom(mProjectFolder); }
	void					SPI_CorrectTextPoint( ATextPoint& ioTextPoint ) const;
	//#698 未使用のため削除void					SPI_RecognizeSyntaxIfDirty();
	void					SPI_UpdateFileAttribute();//R0000
	ABool					SPI_IsNewAndUneditted();//win
	AObjectID				SPI_GetProjectObjectID() const { return mProjectObjectID; }//RC3
	const AText&			SPI_GetProjectRelativePath() const { return mProjectRelativePath; }//#138
	void					SPI_UpdateProjectFolder();//RC3
	AItemCount				SPI_GetLineReallocateStep() const { return mTextInfo.GetLineReallocateStep(); }//#695
	void					SPI_GetReopenFilePath( AText& outPath, ABool& outIsRemoteFile ) const;//#850
	void					SPI_GuessIndentCharacterType();//#912
	ABool					SPI_UpdateIndentCharacterTypeModePref();//#912
	void					SPI_SetTemporaryFontSize( const AFloatNumber inFontSize );//#966
	ABool					SPI_GetVerticalMode() const;//#558
  private:
	void					SetModeIndex( const AModeIndex inModeIndex ) { AStMutexLocker	locker(mModeIndexMutex); mModeIndex = inModeIndex; }
	ADocumentType			NVIDO_GetDocumentType() const {return kDocumentType_Text;}
	void					NVIDO_SpecifyFile( const AFileAcc& inFileAcc );
	ABool					NVIDO_IsSpecified() const;
	void					NVIDO_GetTitle( AText& outTitle ) const;
	void					NVIDO_GetPrintMargin( AFloatNumber& outLeftMargin, AFloatNumber& outRightMargin, AFloatNumber& outTopMargin, AFloatNumber& outBottomMargin ) const;//R0242
	ABool					NVIDO_IsReadOnly() const;//#699
	//#379 void					UpdateTextDrawProperty();
	AText								mTemporaryTitle;
	AModeIndex							mModeIndex;
	mutable AThreadMutex				mModeIndexMutex;//#890
	//#764 ATextEncoding						mTextEncoding;
	AFileAcc							mProjectFolder;
	AObjectID							mProjectObjectID;//RC3
	AText								mProjectRelativePath;//RC3
	AText								mOriginalFontName;//#354
	AFloatNumber						mOriginalFontSize;//#354
	AIndentCharacterType				mGuessedIndnetCharacterType;//#912
	AFloatNumber						mTemporaryFontSize;//#966
	
	/*#858
	//#138
	//外部コメント
  public:
	void					SPI_SetExternalComment( const AText& inModuleName, const AIndex inOffset, const AText& inComment );//RC3
	//#138 void					SPI_GetExternalComment( const AText& inModuleName, const AIndex inOffset, AText& outComment ) const;//RC3
	AIndex					SPI_FindExternalCommentIndexByLineIndex( const AIndex inLineIndex ) const;
	const AText				SPI_GetExternalCommentText( const AIndex inCommentIndex ) const;//#693 const AText&->const AText
	AIndex					SPI_GetExternalCommentLineIndex( const AIndex inCommentIndex ) const;
	const AText				SPI_GetExternalCommentModuleName( const AIndex inCommentIndex ) const;//#693 const AText&->const AText
	AIndex					SPI_GetExternalCommentOffset( const AIndex inCommentIndex ) const;
	void					SPI_GetModuleNameAndOffsetByLineIndex( const AIndex inLineIndex, AText& outText, AIndex& outOffset ) const;//RC3
	void					SPI_SelectTextByModuleNameAndOffset( const AText& inModuleName, const ANumber inOffset );
	void					SPI_UpdateExternalCommentArray();
	void					SPI_GetExternalCommentArray( ATextArray& outModuleNameArray, ANumberArray& outOffsetArray, ATextArray& outCommentArray, ANumberArray& outLineIndexArray ) const;
  private:
	void					AdjustExternalCommentArray( const AIndex inStartLineIndex, const AIndex inDelta );
	AIndex					FindLineIndexByModuleNameAndOffset( const AText& inModuleName, const ANumber inOffset ) const;
	AHashNumberArray					mExternalCommentArray_LineIndex;
	ATextArray							mExternalCommentArray_Comment;
	ATextArray							mExternalCommentArray_ModuleName;
	ANumberArray						mExternalCommentArray_Offset;
	*/
  public:
	void					SPI_GetFileAttribute( AFileAttribute& outFileAttribute ) const;
#if IMPLEMENTATION_FOR_MACOSX
	//クリエータ／タイプ
  public:
	void					SPI_SetFileCreator( const OSType inCreator ) { mCreator = inCreator; NVI_SetDirty(true); }
	void					SPI_SetFileType( const OSType inType ) { mType = inType; NVI_SetDirty(true); }
	OSType					SPI_GetFileCreator() const { return mCreator; }
	OSType					SPI_GetFileType() const { return mType; }
  private:
	OSType								mCreator;
	OSType								mType;
#endif
	
	/*#379
	//
  public:
	void					SPI_TryClose();
	void					SPI_ExecuteClose();
	*/
	//検索
  public:
	ABool					SPI_Find( const AFindParameter& inParam, const ABool inBackward,//#789
									 const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos, 
									 const ABool inUseModalSession, ABool& outModalSessionAborted );
	void					SPI_ChangeReplaceText( const AText& inReplaceText, AText& outText ) const;
	ABool					SPI_FindForHighlight( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos ) ;
	ABool					SPI_FindForHighlight2( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos ) ;//R0244
	void					SPI_SetFindHighlight( const AFindParameter& inParam );
	ABool					SPI_IsFindHighlightSet() const;
	void					SPI_ClearFindHighlight();
	ABool					SPI_ReplaceText( const AFindParameter& inParam, const ATextIndex inStart, const ATextIndex inEnd, 
										AItemCount& outReplacedCount, AItemCount& outOffset, AIndex& ioSelectSpos, AIndex& ioSelectEpos );//#245 #65
  private:
	ATextFinder							mTextFinder;
	ATextFinder							mTextFinderForHighlight;
	ATextFinder							mTextFinderForHighlight2;//R0244
	AFindParameter						mFindHighlightParam;
	AFindParameter						mFindHighlightParam2;//R0244
	ABool								mFindHighlightSet;
	ABool								mFindHighlightSet2;//R0244
	
	//テキストマーカー #750
  public:
	void					SPI_ApplyTextMarker();
	ABool					SPI_FindForTextMarker( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos ) ;
  private:
	AObjectArray<ATextFinder>			mTextMarkerArray_TextFinder;
	ATextArray							mTextMarkerArray_FindText;
	AText								mTextMarkerText;
	AFindParameter						mTextMarkerParam;
	
	//現在単語マーカー #823
  public:
	ABool					SPI_SetCurrentWordForHighlight( const AText& inWord, const ABool inWholeWord, const ABool inRedraw );
	ABool					SPI_FindForCurrentWordHighlight( const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos );
	ABool					SPI_ClearCurrentWordForHighlight( const ABool inRedraw );
  private:
	ATextFinder							mTextFinderForCurrentWordHighlight;
	AFindParameter						mCurrentWordFindHighlightParam;
	ABool								mCurrentWordFindHighlightSet;
	
	//所有・被所有 #379
  public:
	void					SPI_GetOwnDocIDArray( AArray<ADocumentID>& outOwnDocArray ) const;
	ABool					SPI_IsOwnedDocument() const { return mIsOwnedDocument; }
	void					SPI_SetOwnedDocument() { mIsOwnedDocument = true; }
  private:
	ABool								mIsOwnedDocument;
	
	//Diffモード制御
  public:
	void					SPI_SetDiffMode_Repository();
	void					SPI_SetDiffMode_File( const AFileAcc& inDiffTargetFile, const ACompareMode inCompareMode );
	void					SPI_SetDiffMode_None();
	void					SPI_SetDiffMode_TextWithRange( const AText& inTargetText, 
							const AIndex inDiffTextRangeStart, const AIndex inDiffTextRangeEnd,
							const ACompareMode inCompareMode );//#192
	ADiffTargetMode			SPI_GetDiffMode() const { return mDiffTargetMode; }//#379
	void					SPI_UpdateDiffDisplayData( const ABool inReloadRepository = false );//#379
	ADocumentID				SPI_GetDiffTargetDocumentID() const;//#379
	//#81 void					SPI_SetDiffTargetFile( const AFileAcc& inDiffTargetFile );//#379
	ADocumentID				SPI_GetDiffWorkingDocumentID() const { return mDiffWorkingDocumentID; }
	ABool					SPI_CanDiffWithRepositiory() const;
	ACompareMode			SPI_GetCompareMode( AText& outFilename ) const;
  private:
	void					SetDefaultDiffMode();
	ABool					GetDiffFileInDiffSourceFolder( AFileAcc& outFile ) const;
	void					SetDiffMode( const ADiffTargetMode inMode, const AFileAcc& inDiffTargetFile,
							const AIndex inDiffTextRangeStart, const AIndex inDiffTextRangeEnd );
	void					CloseCurrentDiffTargetDocument();
	ABool								mDiffDisplayWaitFlag;
	ADiffTargetMode						mDiffTargetMode;//#379
	ADocumentID							mDiffWorkingDocumentID;//#379
	ADocumentID							mDiffTargetFileDocumentID;//#379
	AFileAcc							mDiffTargetFile;//#379
	AFileAcc							mDiffTempFolder;//#81
	AIndex								mDiffTextRangeStart;//#192
	AIndex								mDiffTextRangeEnd;//#192
	AIndex								mDiffTextRangeStartParagraph;//#192
	AIndex								mDiffTextRangeEndParagraph;//#192
	ACompareMode						mCompareMode;//#81
	
	//Diff表示データ #379
  public:
	AIndex					SPI_GetDiffTargetParagraphIndexFromThisDocumentParagraphIndex( const AIndex inParagraphIndex ) const;//#379
	AItemCount				SPI_GetDiffCount() const { return mDiffDisplayArray_DiffType.GetItemCount(); }//#379
	ADiffType				SPI_GetDiffTypeByDiffIndex( const AIndex inDiffIndex ) const;//#379
	ADiffType				SPI_GetDiffTypeByIncludingLineIndex( const AIndex inLineIndex ) const;
	AIndex					SPI_GetDiffStartLineIndexByDiffIndex( const AIndex inDiffIndex ) const;//#379
	AIndex					SPI_GetDiffStartParagraphIndexByDiffIndex( const AIndex inDiffIndex ) const;//#379
	AIndex					SPI_GetDiffEndLineIndexByDiffIndex( const AIndex inDiffIndex ) const;//#379
	AIndex					SPI_GetDiffEndParagraphIndexByDiffIndex( const AIndex inDiffIndex ) const;//#379
	AIndex					SPI_GetDiffIndexByIncludingLineIndex( const AIndex inLineIndex ) const;//#379
	AIndex					SPI_GetDiffIndexByStartLineIndex( const AIndex inLineIndex ) const;//#379
	AIndex					SPI_GetDiffIndexByEndLineIndex( const AIndex inLineIndex ) const;//#379
	AIndex					SPI_GetDiffIndexByStartParagraphIndex( const AIndex inParagraphIndex ) const;//#379
	AIndex					SPI_GetDiffIndexByEndParagraphIndex( const AIndex inParagraphIndex ) const;//#379
	ABool					SPI_GetCorrespondingTextPointByDiffTargetTextPoint( const ATextPoint& inDiffTargetTextPoint,
																			   ATextPoint& outTextPoint ) const;//#379
	void					SPI_GetDiffTextRangeParagraphIndex( AIndex& outStartParagraphIndex, AIndex& outEndParagraphIndex ) const;
	void					SPI_GetCharDiffInfo( const AIndex inLineIndex, 
							AArray<AIndex>& outDiffStart, AArray<AIndex>& outDiffEnd );//#379
	void					SPI_GetDiffStatusText( AText& outText ) const;//#379
	//#81 ABool					SPI_GetDiffAvailability( const ADiffTargetMode inDiffTargetMode, AFileAcc& outFile ) const;//#379
	//#81 ABool					SPI_UpdateDiffFileTarget();//#379
	ABool					SPI_IsDiffTargetDocument() const //#361 { return mIsDiffTargetDocument; }//#379
	{ return (mTextDocumentType == kTextDocumentType_DiffTarget); }
	//ABool					SPI_DiffExistInCurrentFunction( const AIndex inLineIndex ) const;//#450
	ABool					SPI_DiffExistInRange( const AIndex inStartLineIndex, const AIndex inEndLineIndex ) const;//#450
	/*#drop
	static void				GetDiffDataFor2Texts( const AFileAcc& inDiffTempFolder,
							const AText& inText1, const AText& inText2, 
							AArray<ADiffType>& diffTypeArray,
							AArray<AIndex>& startParagraphIndexArray, AArray<AIndex>& endParagraphIndexArray,
							AArray<AIndex>& oldStartParagraphIndexArray, AArray<AIndex>& oldEndParagraphIndexArray,
							ATextArray& deletedTextArray );//#737
							*/
	static void				SPI_GetDiffParagraphIndexArray( const AItemCount inParagraphCount, const AArray<ADiffType>& inDiffTypeArray, 
							const AHashArray<AIndex>& inStartParagraphIndexArray, const AHashArray<AIndex>& inEndParagraphIndexArray,
							const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray,
							AArray<AIndex>& outDiffParagraphIndexArray );//#737
private:
	void					ExecuteDiff();
	void					NVIDO_DoGeneralAsyncCommandResult( const AGeneralAsyncCommandType inType, const AText& inText );//#379
	static void				ParseDiffText( const AText& inText,
							AArray<ADiffType>& diffTypeArray,
							AArray<AIndex>& startParagraphIndexArray, AArray<AIndex>& endParagraphIndexArray,
							AArray<AIndex>& oldStartParagraphIndexArray, AArray<AIndex>& oldEndParagraphIndexArray,
							ATextArray& deletedTextArray );//#379
	void					SetReverseDiffToDiffTargetDocument( const ADocumentID inDiffTargetDocumentID,
							const AArray<ADiffType>& inDiffTypeArray,
							const AArray<AIndex>& inStartParagraphIndexArray, const AArray<AIndex>& inEndParagraphIndexArray,
							const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray );//#379
	void					SetDiffArray( const AArray<ADiffType>& inDiffTypeArray, 
							const AArray<AIndex>& inStartParagraphIndexArray, const AArray<AIndex>& inEndParagraphIndexArray,
							const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray,//#379
							const ATextArray& inDeletedText, const ABool inSetParagraphIndexArray );//#379
	void					SPI_DoDiffTargetDocumentClosed( const ADocumentID inDiffTargetDocumentID );//#379
	void					SPI_DoWorkingDocumentClosed( const ADocumentID inWorkingDocumentID );//#379
	AArray<ADiffType>					mDiffDisplayArray_DiffType;
	AHashArray<AIndex>					mDiffDisplayArray_StartParagraphIndex;
	AHashArray<AIndex>					mDiffDisplayArray_EndParagraphIndex;
	ATextArray							mDiffDisplayArray_DeletedText;//#379
	AArray<AIndex>						mDiffParagraphIndexArray;//#379
	
	//SCM R0006
  public:
	void					SPI_DoSCMCompareResult(/*#379 const AArray<ADiffType>& inDiffTypeArray, 
	const AArray<AIndex>& inStartParagraphIndexArray, const AArray<AIndex>& inEndParagraphIndexArray,
	const AArray<AIndex>& inOldStartParagraphIndexArray, const AArray<AIndex>& inOldEndParagraphIndexArray,//#379
	const ATextArray& inDeletedText*/ const AText& inDiffText );//#379
	void					SPI_GetNextDiffLineIndex( const AIndex inLineIndex, AIndex& outStart, AIndex& outEnd ) const;//#512
	void					SPI_GetPrevDiffLineIndex( const AIndex inLineIndex, AIndex& outStart, AIndex& outEnd ) const;//#521
	void					SPI_DoRepositoryUpdated();
	ABool					SPI_CanCommit() const;//#455
	void					SPI_DoSCMStatusUpdated( const AFileAcc& inFolder );//#379
  private:
	void					ClearDiffDisplay();
	ADocumentID							mRepositoryDocumentID;//#379
	AFileAcc							mRepositoryTempFile;//#379
	ASCMFileState						mSCMFileState;//#568
	
	//#868
	//最前面ドキュメントの親フォルダを取得
  public:
	ABool					SPI_GetMostFrontDocumentParentFolder( AFileAcc& outFolder ) const;
	
	//ウインドウ選択
  public:
	//#92 void					SPI_SelectFirstWindow();
	void					SPI_SelectText( const AIndex inStartIndex, const AItemCount inLength, const ABool inByFind = false );
	
	//ジャンプメニュー管理
  private:
	AObjectID							mJumpMenuObjectID;
	
	
	//定義リスト
  public:
	void					SPI_GetCurrentIdentifierDefinitionRange( const AIndex inIndex, ATextIndex& outStart, ATextIndex& outEnd,
							ABool& outImport, AFileAcc& outFile ) const;
	AItemCount				SPI_GetIdentifierDefinitionListCount() const;
  private:
	ATextArray							mIdentifierDefinitionList_InfoText;
	AObjectArray<AFileAcc>				mIdentifierDefinitionList_File;
	AArray<ATextIndex>					mIdentifierDefinitionList_StartTextIndex;
	AArray<AItemCount>					mIdentifierDefinitionList_Length;
	
	/*#379
	//<所有クラス(AWindow)インターフェイス>
  public:
	void					OWICB_DoWindowClosed( const AWindowID inWindowID );
	*/
	
	//
  public:
	AWindowID				SPI_GetFirstWindowID() const;
	
	//行折り返し計算 #699
  public:
	ABool					SPI_DoWrapCalculatorPaused();
	ABool					SPI_IsWrapCalculating() const 
	{
		return ((mDocumentInitState == kDocumentInitState_Initial) || 
		(mDocumentInitState == kDocumentInitState_Initial_FileScreeningInThread) ||
		(mDocumentInitState == kDocumentInitState_Initial_FileScreened) ||
		(mDocumentInitState == kDocumentInitState_WrapCalculating) ||
		(mWrapCalculatorWorkingByInsertText == true)); 
	}
	ANumber					SPI_GetWrapCalculatorProgressPercent();
  private:
	void					StartWrapCalculator( const AIndex inLineIndex );
	void					ContinueWrapCalculator();
	AThread_WrapCalculator&	GetWrapCalculatorThread();
	ABool								mWrapCalculator_ThreadUnpausedFlag;
	AIndex								mWrapCalculator_CurrentLineIndex;
	AObjectID							mWrapCalculatorThreadID;
	ABool								mWrapCalculatorWorkingByInsertText;
	AItemCount							mWrapCalculatorProgressTotalTextLength;
	AItemCount							mWrapCalculatorProgressRemainedTextLength;
	
	//文法認識 #698
  public:
	void					SPI_DoSyntaxRecognizerPaused();
	void					SPI_StopSyntaxRecognizerThread();//#890
	void					SPI_RecognizeSyntaxUnrecognizedLines();
  private:
	void					StartSyntaxRecognizer( const AIndex inLineIndex );
	void					CopySyntaxRecognizerResult();
	AIndex					StartSyntaxRecognizerByEdit( const AIndex inLineIndex, const AItemCount inInsertedLineCount,
							const ABool inRecognizeSyntaxAlwaysInThread = false );
	AIndex					RecognizeSyntaxInMainThread( const AIndex inStartLineIndex, const AIndex inEndLineIndex );//#698
	AThread_SyntaxRecognizer&	GetSyntaxRecognizerThread();
	ABool								mSyntaxRecognizer_ThreadRunningFlag;
	ABool								mSyntaxRecognizer_ThreadUnpausedFlag;
	ABool								mSyntaxRecognizer_RestartFlag;
	AIndex								mSyntaxRecognizer_StartLineIndex;
	AIndex								mSyntaxRecognizer_EndLineIndex;
	AObjectID							mSyntaxRecognizerThreadID;
	
	//<内部モジュール>
  public:
	void					SPI_ReRecognizeSyntax();
	void					SPI_ClearSyntaxDataForAllLines();//#844
	//#212 void					SPI_RefreshWindow();
	void					SPI_RefreshTextViews();//win 080727
	void					SPI_RefreshTextViews( const AIndex inStartLineIndex, const AIndex inEndLineIndex );//#695
	void					SPI_CorrectCaretIfInvalid();//#413
	ABool					SPI_IsTextPointValid( const ATextPoint& inTextPoint ) const
	{ return IsTextPointValid(inTextPoint); }//#413
  private:
	void					CalcTextData();
	//#695 void					ClearSyntaxData( const AIndex inStartLineIndex, const AIndex inEndLineIndex );
	void					UpdateWindowText( const AIndex inEditStartLineIndex, 
				const AIndex inRedrawStartLineIndex, const AIndex inRedrawEndLineIndex, const AItemCount inInsertedLineCount, const ABool inParagraphCountChanged,
				const ABool inDontRedraw );//#627
	void					UpdateInfoHeader();
	//#212 void					RefreshWindow();
	void					ExecuteTool( const AFileAcc& inFile );
	void					ExecuteTool_Shell( const AFileAcc& inFile );
	ABool					IsTextPointValid( const ATextPoint& inTextPoint ) const;
	ANumber					GetTextViewWidth();
  public:
	void					UpdateWindowViewImageSize();
	//#291 void					UpdateWindowJumpList();
	//#291 void					UpdateWindowJumpListWithColor();//R0006
	
	//インデント
  public:
	AItemCount				SPI_CalcIndentCount( const AIndex inLineIndex, const AItemCount inPrevParagraphIndent, AItemCount& outNextParagraphIndent,
							//#650 const ABool inUseCurrentParagraphIndentForRegExp );
							const AIndentTriggerType inIndentTriggerType );//#650
	void					GetIndentText( const AItemCount inIndentCount, AText& outText, const ABool inInputSpaces ) const;//#249
	AItemCount				SPI_GetCurrentIndentCount( const AIndex inLineIndex ) const;
	AItemCount				SPI_Indent( const AIndex inLineIndex, const AItemCount inIndentCount, const ABool inInputSpaces,
									   const ABool inDontRedrawSubWindows );//#249
	AItemCount				SPI_GetTextInfoIndentCount( const AIndex inLineIndex ) const;//B0000
	AIndentType				SPI_GetIndentType() const;//B0381
	
	//#450 折りたたみ
  public:
	void					SPI_InitLineImageInfo();
	AFoldingLevel			SPI_GetFoldingLevel( const AIndex inLineIndex ) const;
	AFloatNumber			SPI_GetLineFontSizeMultiply( const AIndex inLineIndex ) const;
	
	//ディレクティブ
  public:
	AIndex					SPI_GetDirectiveLevel( const AIndex inLineIndex ) const { return mTextInfo.GetDirectiveLevel(inLineIndex); }
	
	//ローカル識別子
  public:
	void					SPI_SetCurrentLocalTextPoint( const ATextPoint& inTextPoint, const AViewID inViewID );
	const AAnalyzeDrawData&		SPI_GetCurrentLocalIdentifierAnalyzeDrawData() const {return mCurrentLocalIdentifierAnalyzeDrawData;}//RC1
	void					SPI_SetDrawArrowToDef( const ABool inDraw ) { mDrawArrowToDef = inDraw; SPI_UpdateArrowToDef(true); }//RC1
	ABool					SPI_GetDrawArrowToDef() const { return mDrawArrowToDef; }//RC1
	void					SPI_UpdateArrowToDef( const ABool inRefresh );//RC1
	//AIndex					SPI_GetCurrentLocalIdentifierStartLineIndex() const { return mCurrentLocalIdentifierStartLineIndex; }//RC2
	void					SPI_GetLocalRangeForLineIndex( const AIndex inLineIndex, AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const;//#130
	void					SPI_GetCurrentLocalStartIdentifierParentKeyword( const AIndex inLineIndex ,AText& outParentKeyword ) const;//#717
	void					SPI_GetLocalStartIdentifierDataByLineIndex( const AIndex inLineIndex, 
							AText& outFunctionName, AText& outParentKeywordName, 
							AIndex& outCategoryIndex, ATextPoint& outStart, ATextPoint& outEnd ) const;//#723
	void					SPI_DeleteAllLocalIdentifiers();//#699
  private:
	void					UpdateLocalIdentifierList( const AIndex viewIndex, const ABool inAlwaysRefresh );
	void					ClearArrowToDef();//RC1
	AIndex					GetPrevIndent0LineIndex( const AIndex inLineIndex ) const;//B0340
	AObjectArray< AHashObjectArray<ALocalIdentifierData,AText> >	mCurrentLocalIdentifierListArray;//#893
	/**
	@note mCurrentLocalIdentifierMutexは、メインスレッドでは書き込み時のみロックする。サブスレッドでは読み込みのみ行い、読み込み時にロックする。
	*/
	mutable AThreadMutex				mCurrentLocalIdentifierMutex;//#717
	//
	AArray<AIndex>						mCurrentLocalIdentifierStartLineIndexArray;//#893
	AArray<AIndex>						mCurrentLocalIdentifierEndLineIndexArray;//#893
	AAnalyzeDrawData					mCurrentLocalIdentifierAnalyzeDrawData;//RC1
	ABool								mDrawArrowToDef;//RC1
	
	//ジャンプメニュー管理
  public:
	AObjectID				SPI_GetJumpMenuObjectID() const;
	const ATextArray&		SPI_GetJumpMenuTextArray() const {return mJump_MenuTextArray;}
	//#699 未使用のため削除const ATextArray&		SPI_GetJumpParagraphTextArray() const {return mJump_ParagraphTextArray;}
	const AArray<ATextStyle>&	SPI_GetJumpTextStyleArray() const { return mJump_MenuTextStyleArray; }
	const AArray<AColor>&	SPI_GetJumpMenuColorArray() const { return mJump_MenuColorArray; }
	AUniqID					SPI_GetJumpMenuIdentifierUniqID( const AIndex inIndex ) const;
	AIndex					SPI_GetJumpMenuItemIndexByLineIndex( const AIndex inLineIndex ) const;
	void					SPI_GetJumpMenuTextByMenuItemIndex( const AIndex inMenuItemIndex, AText& outText ) const;
	void					SPI_GetJumpMenuItemInfoTextByLineIndex( const AIndex inLineIndex, AText& outText ) const;
	void					SPI_RevealFromJumpMenuText( const AText& inText ) const;//#454
	void					SPI_GetJumpMenuItemKeywordTextByLineIndex( const AIndex inLineIndex, 
							AText& outFunctionName, AText& outParentKeywordName, 
							AIndex& outCategoryIndex, ATextPoint& outStart, ATextPoint& outEnd ) const;//#723
	void					SPI_GetHeaderPathArray( const AIndex inLineIndex, 
							ATextArray& outHeaderPathTextArray, AArray<AIndex>& outHeaderParagraphIndexArray ) const;//#906
	AIndex					SPI_GetOutlineLevel( const AIndex inLineIndex ) const;//#875
  private:
	void					AddToJumpMenu( const AArray<AUniqID>& inIdentifierUniqIDArray, const AArray<AIndex>& inLineIndexArray );//#695
	ABool					DeleteFromJumpMenu( const AArray<AUniqID>& inIdentifierUniqIDArray );
	void					DeleteAllJumpItems();//#695
	ATextArray							mJump_MenuTextArray;
	AHashArray<AUniqID>					mJump_IdentifierObjectArray;//#698
	//#699 未使用のため削除ATextArray							mJump_ParagraphTextArray;
	AArray<ATextStyle>					mJump_MenuTextStyleArray;
	AArray<AColor>						mJump_MenuColorArray;//R0006
	AArray<AIndex>						mJump_MenuOutlineLevelArray;//#130
	mutable AIndex						mCacheForGetJumpMenuItemIndexByLineIndex;//#695
	
	//ジャンプリスト管理 #291
  public:
	void					SPI_RegisterJumpListView( const AViewID inJumpListViewID );
	void					SPI_UnregisterJumpListView( const AViewID inJumpListViewID );
	void					SPI_UpdateJumpList(/*#695 const ABool inForceSetAll = false*/ );
	void					SPI_UpdateJumpListSelection( const AIndex inLineIndex, const ABool inAdjustScroll );//#698
	void					SPI_SetJumpListSortMode( const AViewID inViewID, const ABool inSort );
	//#725 void					SPI_UpdateJumpListProperty();
  private:
	//#725 void					UpdateJumpListProperty( const AViewID inViewID );
	AView_List&				GetJumpListView( const AViewID inViewID );
	void					SPI_UpdateJumpList_Deleted( const AIndex inIndex );//#695
	void					SPI_UpdateJumpList_Inserted( const AIndex inIndex, const AItemCount inCount );//#695
	void					SPI_RefreshJumpList();//#695
	AArray<AViewID>						mJumpListArray_ViewID;
	AArray<ABool>						mJumpListArray_Sort;
	AIndex								mLastSelectedJumpListItemIndex;//#454
	
	/*#361
	//FTP用データ
  private:
	ABool								mFTPMode;
	AText								mFTPURL;
	AText								mFTPTextOnServer;
	*/
	
	//テキストデータ
  public:
	AItemCount				SPI_GetTextLength() const { return mText.GetItemCount(); }
	const AText&			SPI_GetTextConst() const { return mText; }//R0243
	ATextInfoForDocument&	ForDebug_GetTextInfo() { return mTextInfo; }
  private:
	AText								mText;
	/**
	@note mTextのmutex。メインスレッドでのmText書き込み箇所と、サブスレッドからコールされるリード箇所で排他。
	*/
	mutable AThreadMutex				mTextMutex;//#598
	ATextInfoForDocument				mTextInfo;
	//#698 未使用のため削除ABool								mSyntaxDirty;
	
	//補完入力 #717
  public:
	ABool					SPI_SearchKeywordGlobal( const AText& inWord, const AHashTextArray& inParentWord, 
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
													const ABool& inAbort ) const;//#853
	ABool					SPI_SearchKeywordLocal( const AText& inWord, const AHashTextArray& inParentWord, 
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
												   const ABool& inAbort ) const;//#853
	ABool					SPI_SearchKeywordImport( const AText& inWord, const AHashTextArray& inParentWord, 
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
													const ABool& inAbort ) const;//#853
	//Importファイル
  public:
	const AObjectArray<AFileAcc>&	SPI_GetImportFileArray() const;
	void					SPI_DoImportFileRecognized();
	void					SPI_UpdateImportFileDataIfDirty();
	ABool					SPI_DeleteImportFileData( const AObjectID inImportFileDataObjectID, const AFileAcc& inFile );
	void					SPI_DeleteAllImportFileData();//#349
	void					SPI_UpdateImportFileData() { UpdateImportFileData(); }//#890
  private:
	void					UpdateImportFileData();
	AObjectArray< AFileAcc >			mImportFileArray;
	AImportIdentifierLinkList			mImportIdentifierLinkList;
	/**
	@note mImportIdentifierLinkListMutexは、メインスレッドでは書き込み時のみロックする。サブスレッドでは読み込みのみ行い、読み込み時にロックする。
	*/
	mutable AThreadMutex				mImportIdentifierLinkListMutex;//#717
	ABool								mImportFileDataDirty;
	
	//しおり
  public:
	AIndex					SPI_GetBookmarkLineIndex() const { return mBookmarkLineIndex; }
	void					SPI_SetBookmarkLineIndex( const AIndex inBookmarkLineIndex ) { mBookmarkLineIndex = inBookmarkLineIndex; }
  private:
	AIndex								mBookmarkLineIndex;
	
	//Undoer
  public:
	AUndoTag				SPI_GetCurrentUndoActionTag() const { return mUndoer.GetCurrentAction(); }//#299
	ABool					SPI_IsCurrentUndoActionDangling() const { return mUndoer.IsCurrentActionDangling(); }//#314
  private:
	ATextUndoer							mUndoer;
	
	//ファイル排他制御
  private:
	AFileWriteLocker					mFileWriteLocker;
	
	//他アプリによる編集
  public:
	void					SPI_CheckEditByOtherApp();//R0232
	void					SPI_ReloadEditByOtherApp( const ABool inReload );//R0232
  private:
	void					GetLastLoadOrSaveCopyFile( AFileAcc& outFile ) const;//#693
	
	//#187
	//インライン入力の下線表示をDisableにするフラグ
	//SPI_InsertText()で差分表示等を消すときにOnにする。
	//インライン入力下線が、ドキュメントのText内容よりも先に更新されるので、下線表示がずれる（次の行に表示される）のを防ぐ
	//AView_Textで参照する
  public:
	ABool					SPI_GetDisableInlineInputHilite() const { return mDisableInlineInputHilite; }
  private:
	ABool								mDisableInlineInputHilite;
	
	//
  public:
	ADateTime				SPI_GetLastModifiedDateTime() const { return mLastModifiedDateTime; }
  private:
	ADateTime							mLastModifiedDateTime;
	//#693 AText								mLastReadOrWriteFileRawData;//#653 最後にRead/Writeしたファイルの生データ（tec未変換）
	
	//印刷Imp
	/*#182
  public:
	CPrintImp&				SPI_GetPrintImp() { return mPrintImp; }
	*/
	
	//
  private:
	//#182 CPrintImp							mPrintImp;
	ATextInfoForDocument				mTextInfoForPrint;
	ALocalRect							mPrintRect;
	ANumber								mPrintPaperWidth;
	ANumber								mPrintPaperHeight;
	
	/*#1034
#if IMPLEMENTATION_FOR_MACOSX
	//AppleScript
  public:
	CASTextDoc&				SPI_GetASImp() { return mASImp; }
  private:
	CASTextDoc							mASImp;
#endif
	*/
	
	//CallGraf #723
  public:
	void					SPI_SetCallGrafCurrentFunction( const AWindowID inTextWindowID, const AIndex inLineIndex, 
														   const ABool inByEdit );
	void					SPI_SetCallGrafEditFlag( const AWindowID inTextWindowID, const AIndex inLineIndex );
	
	//図表モード #478
  public:
	ABool					SPI_GetSurroundingTable( const ATextPoint& inTextPoint, 
							AArray<AIndex>& outRowArray_LineIndex, AArray<ABool>& outRowArray_IsRuledLine,
							AArray<AIndex>& outRowArray_ColumnEndX ) const;
	
	//DocInfoウインドウ #142
  public:
	void					SPI_UpdateDocInfoWindows() { UpdateDocInfoWindows(); }
  private:
	void					UpdateDocInfoWindows();
	
	//ヒントテキスト
  public:
	AIndex					SPI_AddHintText( const ATextIndex inTextIndex, const AText& inHintText );
	void					SPI_ClearHintText();
	void					SPI_GetHintText( const ATextIndex inTextIndex, AText& outHintText ) const;
	//void					SPI_ReplaceHintTextCharToSpace( const ATextIndex inHintTextIndex );
	void					SPI_DeleteAllHintText();
	AIndex					SPI_FindHintIndexFromTextIndex( const ATextIndex inHintTextPos ) const;
	
	//フラグ
  public:
	ABool					SPI_GetDocumentFlag() const { return mDocPrefDB.GetData_Bool(ADocPrefDB::kDocumentFlag); }
	void					SPI_SetDocumentFlag( const ABool inFlag ) { mDocPrefDB.SetData_Bool(ADocPrefDB::kDocumentFlag,inFlag); }
	
	//AppleScript用ドキュメントデータ取得
  public:
	ABool					SPI_ASGetParagraph( const ATextIndex inStartTextIndex, const AIndex inParagraphOffsetIndex, 
							ATextIndex &outStart, ATextIndex &outEnd ) const;
	ABool					SPI_ASGetWord( const ATextIndex inStartTextIndex, const AIndex inWordOffsetIndex, 
							ATextIndex& outStart, ATextIndex& outEnd ) const;
	ABool					SPI_ASGetChar( const ATextIndex inStartTextIndex, const AIndex inCharOffsetIndex, 
							ATextIndex& outStart, ATextIndex& outEnd ) const;
	ABool					SPI_ASGetInsertionPoint( const ATextIndex inStartTextIndex, const AIndex inOffsetIndex, ATextIndex& outPos ) const;//B0331
	
	AItemCount				SPI_ASGetParagraphCount( const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex ) const;
	AItemCount				SPI_ASGetWordCount( const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex ) const;
	AItemCount				SPI_ASGetCharCount( const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex ) const;
	
	//
	static void				OrderTextPoint( const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2, 
							ATextPoint& outStartTextPoint, ATextPoint& outEndTextPoint );
	
#if IMPLEMENTATION_FOR_MACOSX
	//ODB
  public:
	void					SPI_SetODBMode( const OSType inODBSender, const AText& inODBToken, const AText& inODBCustomPath );
	ABool					SPI_IsODBMode() const { return mODBMode; }
	void					SPI_GetODBCustomPath( AText& outCustomPath ) const { outCustomPath.SetText(mODBCustomPath); }
  private:
	ABool								mODBMode;
	OSType								mODBSender;
	AText								mODBToken;
	AText								mODBCustomPath;
	AFileAcc							mODBFile;
#else
	ABool					SPI_IsODBMode() const { return false; }
#endif
	//
  public:
	void					SPI_GetCurrentStateIndexAndName( const ATextPoint& inTextPoint, 
							AIndex& outStateIndex, AIndex& outPushedStateIndex,
							AText& outStateText, AText& outPushedStateText ) const;
	AIndex					SPI_GetCurrentStateIndex( const ATextPoint& inTextPoint ) const;
	AUniqID					SPI_GetFirstGlobalIdentifierUniqID( const AIndex inLineIndex ) const
	{ return mTextInfo.GetFirstGlobalIdentifierUniqID(inLineIndex); }
	AIndex					SPI_GetStateIndexByLineIndex( const AIndex inLineIndex ) const
	{ AIndex stateIndex = kIndex_Invalid; AIndex pushedStateIndex = kIndex_Invalid;
	mTextInfo.GetLineStateIndex(inLineIndex,stateIndex,pushedStateIndex); return stateIndex; }
	
	//Previewer #734
  public:
	void					SPI_UpdatePreview( const AIndex inLineIndex );
	void					SPI_ReloadPrevew();
  private:
	AIndex								mPreviewCurrentLineIndex;
	AText								mPreviewCurrentAnchor;
	
	//B0441
  public:
	void					SPI_SetCreatedAutomatically( const ABool inCreatedAutomatically ) { mCreatedAutomatically = inCreatedAutomatically; }
	ABool					SPI_GetCreatedAutomatically() const { return mCreatedAutomatically; }
  private:
	ABool								mCreatedAutomatically;
	
	//#1351
  public:
	void					SPI_SetReadMeFile() { mIsReadMeFile = true; }
  private:
	ABool								mIsReadMeFile;
	
	//R0199スペルチェック
  public:
	void					SPI_ClearSpellCheckCache() { mMisspellCacheArray.DeleteAll(); mCorrectspellCacheArray.DeleteAll(); }
  private:
	mutable AHashTextArray				mMisspellCacheArray;
	mutable AHashTextArray				mCorrectspellCacheArray;
	static const ANumber				kMisspellCacheSize = 10000;
	static const ANumber				kMisspellCacheDeleteSize = 500;
	
	//#904
	//JavaScript用関数
  public:
	AIndex					SPI_GetUnicodeTextIndexFromTextIndex( const AIndex inTextIndex ) const;
	AIndex					SPI_GetTextIndexFromUnicodeTextIndex( const AIndex inUnicodeTextIndex ) const;
	
	//プラグイン #994
  public:
	void					SPI_ExecuteEventListener( AConstCstringPtr inPtr, const AText& inParameter ) const;
	
	//TextInfoデータ整合性チェック#699
  public:
	void					SPI_CheckLineInfoDataForDebug();
	
	//
  public:
	AByteCount				SPI_GetTextDocumentMemoryByteCount() const;
	
	//
  public:
	void					TestGuessTextEncoding();
};

#pragma mark ===========================
#pragma mark [クラス]ATextDocumentFactory
typedef AAbstractFactoryForObjectArray<ADocument>	ADocumentFactory;
class ATextDocumentFactory : public ADocumentFactory
{
  public:
	ATextDocumentFactory( AObjectArrayItem*	inParent, const AObjectID inDocImpID ) : mParent(inParent), mDocImpID(inDocImpID)//#1034
	{
	}
	ADocument*	Create()
	{
		return new ADocument_Text(mParent,mDocImpID);//#1034
	}
  private:
	AObjectArrayItem* mParent;
	AObjectID	mDocImpID;//#1034
};



