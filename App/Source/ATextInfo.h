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

ATextInfo

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AIdentifierList.h"
#include "ABlockStartDataList.h"

class CTextDrawData;
class ALocalIdentifierData;
class AUnicodeData;
class ASyntaxDefinition;

//LineInfoの各列のメモリ割り当てサイズ初期値
//（ただし、SetReallocateStep()によって、変更される。）
//ここを1000等にすると、ImportFileDataのTextInfoでのメモリ消費が多くなる(AApp.cppのみ読み込みで88MB->100MB)ので、2.1同等の64にしている。
//SetReallocateStep()で変更するので、64でも速度の低下はないはず。
const AItemCount	kLineInfoInitialAllocation = 64;
const AItemCount	kLineInfoAllocationStep = 64;

//mLineInfoStorage_RecognizeFlags用フラグマスク #695
const unsigned short int	kLineInfoRecognizeFlagMask_None = 0;
const unsigned short int	kLineInfoRecognizeFlagMask_Recognized = 1;
const unsigned short int	kLineInfoRecognizeFlagMask_ColorizeLine = 2;
const unsigned short int	kLineInfoRecognizeFlagMask_MenuIdentifierExist = 4;
const unsigned short int	kLineInfoRecognizeFlagMask_LocalDelimiterExist = 8;
const unsigned short int	kLineInfoRecognizeFlagMask_SyntaxWarning = 16;
const unsigned short int	kLineInfoRecognizeFlagMask_AnchorIdentifierExist = 32;
const unsigned short int	kLineInfoRecognizeFlagMask_SyntaxError = 64;

#pragma mark ===========================
#pragma mark [クラス]ALineInfo
//#695
/**
行情報
*/
class ALineInfo
{
	//コンストラクタ、デストラクタ
  public:
	ALineInfo();
	~ALineInfo();
	
	//データ
  private:
	//行毎データ
	AArray<AUniqID>						mLineInfoIDArray;
	//Storage
	AUniqIDArray						mLineInfoStorage_UniqID;
	AArray<AIndex>						mLineInfoStorage_Start;
	AArray<AItemCount>					mLineInfoStorage_Length;
	AArray<ABool>						mLineInfoStorage_ExistLineEnd;
	AArray<AIndex>						mLineInfoStorage_ParagraphIndex;//R0208
	AArray<AUniqID>						mLineInfoStorage_FirstGlobalIdentifierUniqID;
	AArray<unsigned short int>			mLineInfoStorage_RecognizeFlags;
	//Storage (purge可能)
	AArray<short int>					mLineInfoStorageP_StateIndex;
	AArray<short int>					mLineInfoStorageP_PushedStateIndex;
	AArray<AUniqID>						mLineInfoStorageP_FirstLocalIdentifierUniqID;
	AArray<AColor>						mLineInfoStorageP_LineColor;
	AArray<short int>					mLineInfoStorageP_IndentCount;
	AArray<AUniqID>						mLineInfoStorageP_FirstBlockStartDataUniqID;
	//drop AArray<short int>					mLineInfoStorageP_Multiply;//#450
	AArray<short int>					mLineInfoStorageP_DirectiveLevel;//#467
	AArray<short int>					mLineInfoStorageP_DisabledDirectiveLevel;//#467 
	AArray<short int>					mLineInfoStorageP_FoldingLevel;//#695
	//drop AArray<ANumber>						mLineInfoStorageP_CheckedDate;//#842
	AArray<AItemCount>					mLineInfoStorageP_CharCount;//#142
	AArray<AItemCount>					mLineInfoStorageP_WordCount;//#142
	
	//挿入・削除
  public:
	void					InsertLines( const AIndex inLineIndex, const AItemCount inLineCount );
	void					Insert1Line( const AIndex inLineIndex );
	AIndex					Add1Line();
	void					DeleteLines( const AIndex inLineIndex, const AItemCount inDeleteCount );
	void					Delete1Line( const AIndex inLineIndex );
	void					DeleteAllAndCancelPurge();
	
	//行数拡大時の再割り当て増加量
  public:
	void					SetReallocateStep( const AItemCount inCount );
	AItemCount				GetReallocateStep() const { return mReallocateStep; }
  private:
	AItemCount							mReallocateStep;
	
	//行数取得
  public:
	AItemCount				GetLineCount() const
	{ return mLineInfoIDArray.GetItemCount(); }
	
	//Get/Set
  public:
	AIndex					GetLineInfo_Start( const AIndex inLineIndex ) const;
	AIndex					GetLineInfo_Start_FromLineUniqID( const AUniqID inLineUniqID ) const;//#853
	void					SetLineInfo_Start( const AIndex inLineIndex, const AIndex inStart );
	AItemCount				GetLineInfo_Length( const AIndex inLineIndex ) const;
	void					SetLineInfo_Length( const AIndex inLineIndex, const AItemCount inLength );
	ABool					GetLineInfo_ExistLineEnd( const AIndex inLineIndex ) const;
	void					SetLineInfo_ExistLineEnd( const AIndex inLineIndex, const ABool inExistLineEnd );
	AUniqID					GetLineInfo_FirstGlobalIdentifierUniqID( const AIndex inLineIndex ) const;
	void					SetLineInfo_FirstGlobalIdentifierUniqID( const AIndex inLineIndex, const AUniqID inFirstGlobalIdentifierUniqID );
	AIndex					GetLineInfo_ParagraphIndex( const AIndex inLineIndex ) const;
	void					SetLineInfo_ParagraphIndex( const AIndex inLineIndex, const AIndex inParagraphIndex );
	void					ClearRecognizeFlags( const AIndex inLineIndex );
	ABool					GetLineInfoP_Recognized( const AIndex inLineIndex ) const;
	void					SetLineInfoP_Recognized( const AIndex inLineIndex, const ABool inRecognized );
	short int				GetLineInfoP_StateIndex( const AIndex inLineIndex ) const;
	void					SetLineInfoP_StateIndex( const AIndex inLineIndex, const short int inStateIndex );
	short int				GetLineInfoP_PushedStateIndex( const AIndex inLineIndex ) const;
	void					SetLineInfoP_PushedStateIndex( const AIndex inLineIndex, const short int inPushedStateIndex );
	AUniqID					GetLineInfoP_FirstLocalIdentifierUniqID( const AIndex inLineIndex ) const;
	void					SetLineInfoP_FirstLocalIdentifierUniqID( const AIndex inLineIndex, const AUniqID inFirstLocalIdentifierUniqID );
	ABool					GetLineInfoP_ColorizeLine( const AIndex inLineIndex ) const;
	void					SetLineInfoP_ColorizeLine( const AIndex inLineIndex, const ABool inColorizeLine );
	AColor					GetLineInfoP_LineColor( const AIndex inLineIndex ) const;
	void					SetLineInfoP_LineColor( const AIndex inLineIndex, const AColor inLineColor );
	short int				GetLineInfoP_IndentCount( const AIndex inLineIndex ) const;
	void					SetLineInfoP_IndentCount( const AIndex inLineIndex, const short int inIndentCount );
	AUniqID					GetLineInfoP_FirstBlockStartDataUniqID( const AIndex inLineIndex ) const;
	void					SetLineInfoP_FirstBlockStartDataUniqID( const AIndex inLineIndex, const AUniqID inBlockStartDataUniqID );
	short int				GetLineInfoP_Multiply( const AIndex inLineIndex ) const;
	void					SetLineInfoP_Multiply( const AIndex inLineIndex, const short int inMultiply );
	short int				GetLineInfoP_DirectiveLevel( const AIndex inLineIndex ) const;
	void					SetLineInfoP_DirectiveLevel( const AIndex inLineIndex, const short int inDirectiveLevel );
	short int				GetLineInfoP_DisabledDirectiveLevel( const AIndex inLineIndex ) const;
	void					SetLineInfoP_DisabledDirectiveLevel( const AIndex inLineIndex, const short int inDisabledDirectiveLevel );
	AFoldingLevel			GetLineInfoP_FoldingLevel( const AIndex inLineIndex ) const;
	void					SetLineInfoP_FoldingLevel( const AIndex inLineIndex, const AFoldingLevel inFoldingLevel );
	ABool					GetLineInfoP_MenuIdentifierExist( const AIndex inLineIndex ) const;
	void					SetLineInfoP_MenuIdentifierExist( const AIndex inLineIndex, const ABool inMenuIdentifierExist );
	ABool					GetLineInfoP_LocalDelimiterExist( const AIndex inLineIndex ) const;
	void					SetLineInfoP_LocalDelimiterExist( const AIndex inLineIndex, const ABool inLocalDelimiterExist );
	ABool					GetLineInfoP_SyntaxWarningExist( const AIndex inLineIndex ) const;
	void					SetLineInfoP_SyntaxWarningExist( const AIndex inLineIndex, const ABool inSyntaxWarning );
	ABool					GetLineInfoP_SyntaxErrorExist( const AIndex inLineIndex ) const;
	void					SetLineInfoP_SyntaxErrorExist( const AIndex inLineIndex, const ABool inSyntaxWarning );
	ABool					GetLineInfoP_AnchorIdentifierExist( const AIndex inLineIndex ) const;
	void					SetLineInfoP_AnchorIdentifierExist( const AIndex inLineIndex, const ABool inAnchorExist );
	ANumber					GetLineInfoP_CheckedDate( const AIndex inLineIndex ) const;//#842
	void					SetLineInfoP_CheckedDate( const AIndex inLineIndex, const ANumber inDate );//#842
	AItemCount				GetLineInfoP_CharCount( const AIndex inLineIndex ) const;//#142
	void					SetLineInfoP_CharCount( const AIndex inLineIndex, const AItemCount inCount );//#142
	AItemCount				GetLineInfoP_WordCount( const AIndex inLineIndex ) const;//#142
	void					SetLineInfoP_WordCount( const AIndex inLineIndex, const AItemCount inCount );//#142
	
	AUniqID					GetLineUniqID( const AIndex inLineIndex ) const;
	AIndex					FindLineIndexFromLineUniqID( const AUniqID inLineUniqID ) const;
	AIndex					GetParagraphIndexFromLineUniqID( const AUniqID inLineUniqID ) const;
	AIndex					GetLineIndexFromParagraphIndex( const AIndex inParagraphIndex ) const;
	AIndex					FindFirstUnrecognizedLine() const;
	AUniqID					FindCurrentMenuIdentifier( const AIndex inLineIndex ) const;
	AUniqID					FindCurrentLocalStartIdentifier( const AIndex inLineIndex ) const;//#717
	AUniqID					FindAnchorIdentifier( const AIndex inLineIndex ) const;
	void					FindLocalRange( const AIndex inLineIndex, AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const;//#695
	void					GetTotalWordCount( AItemCount& outTotalCharCount, AItemCount& outTotalWordCount ) const;//#142
	
	//Purge
  public:
	ABool					IsPurged() { return mIsPurged; }
	void					Purge();
  private:
	ABool								mIsPurged;
	
	//
  public:
	void					ShiftLineStarts( const AIndex inStartLineIndex, const AItemCount inAddedTextLength );
	void					ShiftParagraphIndex( const AIndex inStartLineIndex, const AItemCount inParagraphCount );//#695
	
	//
  public:
	void					CopyLineInfoToSyntaxRecognizer( const ALineInfo& inSrcLineInfo, const AIndex inStartLineIndex, const AIndex inEndLineIndex );
	void					CopyFromSyntaxRecognizerResult( const AIndex inDstStartLineIndex, const AIndex inDstEndLineIndex,
							const ALineInfo& inSrcLineInfo );
	
	//Debug
  public:
	void					CheckLineInfoDataForDebug();
	
};
/*#695 ABlockStartDataList.hでUniqIDベースに作り直し
#pragma mark ===========================
#pragma mark [クラス]ABlockStartData
class ABlockStartData : public AObjectArrayItem
{
	//コンストラクタ、デストラクタ
  public:
	ABlockStartData( AObjectArrayItem* inParent, const AIndex inVariableIndex, const AItemCount inIndentCount ) 
	: AObjectArrayItem(inParent), mVariableIndex(inVariableIndex), mIndentCount(inIndentCount), mNextObjectID(kObjectID_Invalid) {}//sdfcheck
	~ABlockStartData() {}
	
	//
  public:
	void					SetNextObjectID( const AObjectID inNextObjectID ) { mNextObjectID = inNextObjectID; }
	AObjectID				GetNextObjectID() const { return mNextObjectID; }
	AIndex					GetVariableIndex() const { return mVariableIndex; }
	AItemCount				GetIndentCount() const { return mIndentCount; }
	
  private:
	AIndex								mVariableIndex;
	AItemCount							mIndentCount;
	AObjectID							mNextObjectID;
};

#pragma mark ===========================
#pragma mark [クラス]ABlockStartDataFactory
//
class ABlockStartDataFactory : public AAbstractFactoryForObjectArray<ABlockStartData>
{
  public:
	ABlockStartDataFactory( AObjectArrayItem* inParent, const AIndex inVariableIndex, const AItemCount inIndentCount )
			: mParent(inParent), mVariableIndex(inVariableIndex), mIndentCount(inIndentCount)
	{
	}
	ABlockStartData*	Create() 
	{
		return new ABlockStartData(mParent,mVariableIndex,mIndentCount);
	}
  private:
	AObjectArrayItem*					mParent;
	AIndex								mVariableIndex;
	AItemCount							mIndentCount;
};


#pragma mark ===========================
#pragma mark [クラス]ABlockStartDataList
class ABlockStartDataList : public AObjectArrayItem
{
	//コンストラクタ、デストラクタ
  public:
	ABlockStartDataList() : AObjectArrayItem(NULL) ,mArray(NULL) {}//#417 #693
	~ABlockStartDataList() {}
	
	//
  public:
	AObjectID				Add( const AObjectID inPreviousID, const AIndex inVariableIndex, const AItemCount inIndentCount );
	void					DeleteIdentifiers( const AObjectID inFirstObjectID );
	
	AIndex					GetVariableIndex( const AObjectID inObjectID ) const;
	AItemCount				GetIndentCount( const AObjectID inObjectID ) const;
	AObjectID				GetNextObjectID( const AObjectID inObjectID ) const;
	
	AItemCount				GetItemCount() const { return mArray.GetItemCount(); }//#698
	
	void					DeleteAll() { mArray.DeleteAll(); }//#699
  private:
	
	AObjectArrayIDIndexed<ABlockStartData>	mArray;//#693
};
*/
#pragma mark ===========================
#pragma mark [クラス]ATextInfo
class ATextInfo : public AObjectArrayItem
{
	//コンストラクタ、デストラクタ
  public:
	ATextInfo( AObjectArrayItem* inParent = NULL );
	~ATextInfo();
	
	//
  public:
	void					SetMode( const AModeIndex inModeIndex, const ABool inLoadRegExp = true );
	void					PurgeForImportFileData();//#693
	ABool					CalcIndentByIndentRegExp( const AText& inText, const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex,
							AItemCount& ioCurrent, AItemCount& ioNext );//#441
	void					PurgeRegExpObject();//#693
	void					DeleteAllInfo();//#896
  protected:
	void					UpdateRegExp() const;//#683
	void					PurgeLineInfo();//#693
	AModeIndex							mModeIndex;
	mutable AObjectArray<ARegExp>				mJumpSetupRegExp;
	mutable AObjectArray<ARegExp>				mIndentRegExp;
	mutable AObjectArray<ARegExp>		mSyntaxDefinitionRegExp;
	mutable AArray<AIndex>						mSyntaxDefinitionRegExp_State;
	mutable AArray<AIndex>						mSyntaxDefinitionRegExp_Index;
	mutable AObjectArray<ARegExp>		mKeywordRegExp;
	mutable AArray<AIndex>						mKeywordRegExp_CategoryIndex;
	mutable ABool								mRegExpObjectPurged;//#693
	
	//行情報配列操作
  public:
	void					InsertLineInfo( const AIndex inLineIndex );
	void					InsertLineInfo( const AIndex inLineIndex, const AIndex inStart, const AItemCount inLength, const ABool inExistLineEnd );//#699
	void					AddLineInfo();
	void					AddLineInfo( const AIndex inStart, const AItemCount inLength, const ABool inExistLineEnd );//#699
	void					DeleteLineInfos( const AIndex inLineIndex, const AItemCount inDeleteCount,
							AArray<AUniqID>& outDeletedIdentifiers, ABool& outImportFileIdentifierDeleted );//#695
	AItemCount				GetLineCount() const;
	AItemCount				GetLineLengthWithoutLineEnd( const AIndex inLineIndex ) const;
	AItemCount				GetLineLengthWithLineEnd( const AIndex inLineIndex ) const;
	ATextIndex				GetLineStart( const AIndex inLineIndex ) const;
	AItemCount				GetTextLength() const;
	ABool					GetLineExistLineEnd( const AIndex inLineIndex ) const;
  protected:
	ALineInfo							mLineInfo;
	
	//TextPoint/TextIndex変換
  public:
	AIndex					GetTextIndexFromTextPoint( const ATextPoint& inPoint ) const;
	void					GetTextPointFromTextIndex( const AIndex inTextPosition, ATextPoint& outPoint, const ABool inPreferNextLine = false ) const;
	
	//描画用テキスト取得
  public:
	void					GetTextDrawDataWithoutStyle( const AText& inText, const ANumber inTabWidth, 
														const AItemCount inIndentWidth, //#117
														const AItemCount inLetterCountLimit,
														const AIndex inLineIndex, CTextDrawData& outTextDrawData, 
														const ABool inGetUTF16Text, const ABool inCountAs2Letters,
														const AIndex inToLineEndCode, const ABool inForDraw,
														const ABool inDisplayYenFor5C, const AArray<AIndex>& inTabPositions ) const;//B0000 行折り返し計算高速化 #940 #1421
	
	//行情報計算
  public:
	//void					InitLineInfoTemporarily( const AItemCount inTextLength );
	void					DeleteLineInfoAll(/*#695 AArray<AUniqID>& outDeletedIdentifiers */);
	AItemCount				CalcLineInfoAll( const AText& inText, 
							const ABool inAddEmptyEOFLineIfNeeded, const AItemCount inLimitLineCount,//#699
							/*win const AFont inFont*/const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
							const AItemCount inTabWidth, const AItemCount inIndentWidth,const AWrapMode inWrapMode, const AItemCount inWrapLetterCount, const ANumber inViewWidth, 
											 const ABool inCountAs2Letters, const AArray<AIndex>& inTabPositions );//#1421
	void					CalcLineInfo( const AText& inText, const AIndex inLineIndex, 
							/*win const AFont inFont*/const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
							const AItemCount inTabWidth, const AItemCount inIndentWidth, //#117
							const AWrapMode inWrapMode, const AItemCount inWrapLetterCount, const ANumber inViewWidth,
							const ABool inCountAs2Letters,
										  CTextDrawData& ioTextDrawData, AIndex& ioTextDrawDataStartOffset,
										  const AArray<AIndex>& inTabPositions );//B0000 行折り返し計算高速化 #1421
	void					CalcLineInfoAllWithoutView( const AText& inText, const ABool& inAbort );//win
	void					CalcLineInfoWithoutView( const AText& inText, const AIndex inLineIndex );//win
	void					ShiftLineStarts( const AIndex inStartLineIndex, const AItemCount inAddedTextLength );
	void					ShiftParagraphIndex( const AIndex inStartLineIndex, const AItemCount inParagraphCount );//#695
	//#695 void					AdjustParagraphIndex();//R0208
	
	AIndex					GetCurrentParagraphStartLineIndex( const AIndex inLineIndex ) const;
	AIndex					GetNextParagraphStartLineIndex( const AIndex inLineIndex ) const;
	ATextIndex				GetParagraphStartTextIndex( const AIndex inParagraphIndex ) const;
	AIndex					GetParagraphIndexByLineIndex( const AIndex inLineIndex ) const;
	AIndex					GetLineIndexByParagraphIndex( const AIndex inParagraphIndex ) const;
	AUniqID					GetFirstGlobalIdentifierUniqID( const AIndex inLineIndex ) const;
	ABool					IsParagraphStartLine( const AIndex inLineIndex ) const;
	ABool					CopyFromCalculatorResult( const AIndex inLineIndex, const ATextInfo& inSrcTextInfo );//#699
	void					SetLineReallocateStep( const AItemCount inCount )
	{ mLineInfo.SetReallocateStep(inCount); }//#695
	AItemCount				GetLineReallocateStep() const
	{ return mLineInfo.GetReallocateStep(); }//#695
	ABool					AddEmptyEOFLineIfNeeded();//#699
	void					ClearRecognizeFlags( const AIndex inLineIndex )
	{ mLineInfo.ClearRecognizeFlags(inLineIndex); }//#695
  protected:
	void					Kinsoku( const AText& inText, const AIndex inLineIndex );
  private:
	void					CalcWordCount( const AText& inText, const AIndex inLineIndex );//#142
public:
	void					GetWordCount( const AText& inText, const AIndex inStartIndex, const AItemCount inLength,
							AItemCount& outCharCount, AItemCount& outWordCount, AItemCount& outParagraphCount, const ABool inAdjustForLineInfo = false ) const;//#142 #1403
	//文法認識
  public:
	void					RecognizeSyntaxAll( const AText& inText, const AText& inFileURL, 
							AArray<AUniqID>& outAddedGlobalIdentifiers, AArray<AIndex>& outAddedGlobalIdentifiersLineIndex,
							ABool& outImportChanged, const ABool& inAbort );//#698
	AIndex					PrepareRecognizeSyntaxContext( const AText& inText, const AIndex inStartLineIndex, const AText& inFileURL );//#698 #998
	AIndex					ExecuteRecognizeSyntax( const AText& inText, const AIndex inEndLineIndex,
							AArray<AUniqID>& outDeletedIdentifiers, 
							AArray<AUniqID>& outAddedGlobalIdentifiers, AArray<AIndex>& outAddedGlobalIdentifiersLineIndex,
							ABool& outImportChanged, const ABool& inAbort );//#698
	void					ClearRecognizeSyntaxContext();//#698
	void					UpdateCommentBlockFoldingLevel( const AIndex inLineIndex, const ABoolArray& isCommentStateArray);
	AIndex					FindFirstUnrecognizedLine() const;//#698
	void					CopyRecognizeContext( const ATextInfo& inSrcTextInfo, const AIndex inOffsetLineIndex );//#698
	void					CopyLineInfoToSyntaxRecognizer( const ATextInfo& inSrcTextInfo, const AIndex inStartLineIndex, const AIndex inEndLineIndex );//#698
	void					CopyFromSyntaxRecognizerResult( const AIndex inDstStartLineIndex, const AIndex inDstEndLineIndex,
							const ATextInfo& inSrcTextInfo,
							AArray<AUniqID>& outDeletedIdentifiers, 
							AArray<AUniqID>& outAddedGlobalIdentifiers, AArray<AIndex>& outAddedGlobalIdentifiersLineIndex,
							ABool& outImportChanged );//#698
	//ABool					IsAlphabet( const AUChar inChar ) const;
	//ABool					IsContinuousAlphabet( const AUChar inChar ) const;
	//ABool					IsTailAlphabet( const AUChar inChar ) const;
	ABool					GetTokenForRecongnizeSyntax( const ASyntaxDefinition& inSyntaxDefinition, AUnicodeData& inUnicodeData,//#664
							const AText& inText, AIndex& ioPos, const AIndex inEndPos, AText& outToken, AIndex& outTokenStartPos,
							const AIndex inSyntaxPartIndex ) const;
	ABool					GetTokenForRecongnizeSyntax( const ASyntaxDefinition& inSyntaxDefinition, AUnicodeData& inUnicodeData,//#664
							const AText& inText, AIndex& ioPos, const AIndex inEndPos, 
							AText& outToken, AIndex& outTokenStartPos, ABool& outIsAlphabet,
							const AIndex inSyntaxPartIndex ) const;//#723
	ABool					RecognizeStateAndIndent( const AText& inText, const AIndex inStateIndex, AIndex& ioPushedStateIndex,
			const ATextIndex inPos, const ATextIndex inEndPos, 
			AIndex& outStateIndex, ABool& outChangeStateFromNextChar, ATextIndex& outPos, ABool& outSeqFound,
			AText& outToken, ATextIndex& outTokenStartPos,
			const AItemCount inIndentWidth, const AItemCount inLabelIndentWidth, 
							AItemCount& ioCurrentLineIndentCount, AItemCount& ioNextLineIndentCount ) const;
	void					ClearSyntaxDataForAllLines();//#695
  private:
	/** 文法認識開始行 */
	AIndex								mRecognizeContext_RecognizeStartLineIndex;//#698
	/** 文法認識終了可能行 */
	AIndex								mRecognizeContext_EndPossibleLineIndex;//#698
	AIndex								mRecognizeContext_CurrentStateIndex;//#698
	AIndex								mRecognizeContext_PushedStateIndex;//#698
	AItemCount							mRecognizeContext_CurrentIndent;//#698
	AIndex								mRecognizeContext_CurrentDirectiveLevel;//#698
	AIndex								mRecognizeContext_NextDirectiveLevel;//#698
	AIndex								mRecognizeContext_CurrentDisabledDirectiveLevel;//#698
	AIndex								mRecognizeContext_NextDisabledDirectiveLevel;//#698
	ATextArray							mRecognizeContext_Variable_ValueArray;//#698
	AArray<ATextIndex>					mRecognizeContext_Variable_StartPos;//#698
	AArray<AItemCount>					mRecognizeContext_Variable_Length;//#698
	AArray<AItemCount>					mRecognizeContext_Variable_BlockStartIndent;//#695
	AIndex								mRecognizeContext_CurrentTokenStackPosition;//#698
	AArray<ATextIndex>					mRecognizeContext_TokenStack_StartPos;//#698
	AArray<AItemCount>					mRecognizeContext_TokenStack_Length;//#698
	
	//単語検索
  public:
	static ABool			FindWord( const AText& inText, const AIndex inModeIndex, const ABool inFindForwardOnly,
							const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd, 
							const ABool inUseAlphabetForKeyword, const AIndex inSyntaxPartIndex ) ;
	
	//識別子情報
  public:
	void					DeleteLineIdentifiers( const AIndex inLineIndex, AArray<AUniqID>& outDeletedIdentifiers, ABool& outImportFileIdentifierDeleted );
	AUniqID					AddLineGlobalIdentifier( const AIdentifierType inType, 
							/*#698 const ATextIndex inStartIndex, const ATextIndex inEndIndex*/
							const AIndex inLineIndex, const AIndex inXSpos, const AIndex inXEpos, //#698
							const AIndex inCategoryIndex, 
							const AText& inKeywordText, const AText& inMenuText, const AText& inInfoText,
							const AText& inCommentText, const AText& inParentKeywordText, const AText& inTypeText,
							const ABool inLocalRangeDelimiter, const AIndex inOutlineLevel );//RC2 #593 #130
	AUniqID					AddLineLocalIdentifier( const AIdentifierType inType, 
							/*#698 const ATextIndex inStartIndex, const ATextIndex inEndIndex*/
							const AIndex inLineIndex, const AIndex inXSpos, const AIndex inXEpos, //#698
							const AIndex inCategoryIndex, 
							const AText& inKeywordText, const AText& inInfoText, const AText& inTypeText );//R0243
	AIndex					GetLineIndexByGlobalIdentifier( const AUniqID inIdentifierUniqID ) const;
	AIndex					GetParagraphIndexByGlobalIdentifier( const AUniqID inIdentifierUniqID ) const;//#695
	AUniqID					GetMenuIdentifierByLineIndex( const AIndex inLineIndex ) const;
	AUniqID					GetMenuIdentifierByLineIndex( const AIndex inLineIndex, AIndex& outIdnetifierLineIndex ) const;//RC3
	AIndex					FindLineIndexByMenuText( const AText& inMenuName ) const;//#138
	AIndex					FindLineIndexByMenuKeywordText( const AText& inMenuName ) const;//#138
	void					GetLocalRangeByLineIndex( const AIndex inLineIndex, AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const;
	AUniqID					GetAnchorIdentifierByLineIndex( const AIndex inLineIndex ) const;
	AUniqID					GetLocalStartIdentifierByLineIndex( const AIndex inLineIndex ) const;//#717
	void					GetCurrentLocalStartIdentifierParentKeyword( const AIndex inLineIndex ,AText& outParentKeyword ) const;//#717
	void					GetGlobalIdentifierRange( const AUniqID inIdentifierUniqID, ATextPoint& outStart, ATextPoint& outEnd ) const;
	AUniqID					GetGlobalIdentifierByKeywordText( const AText& inKeywordText ) const;
	void					GetGlobalIdentifierListByKeywordText( const AText& inKeywordText, AArray<AUniqID>& outUniqIDArray ) const;//RC2
	AIndex					GetGlobalIdentifierCategoryIndexByKeywordText( const AText& inKeywordText ) const;
	ABool					GetGlobalIdentifierKeywordText( const AUniqID inIdentifierID, AText& outText ) const;
	void					GetGlobalIdentifierMenuText( const AUniqID inIdentifierID, AText& outText ) const;
	void					GetGlobalIdentifierInfoText( const AUniqID inIdentifierID, AText& outText ) const;
	void					GetGlobalIdentifierCommentText( const AUniqID inIdentifierID, AText& outText ) const;//RC2
	void					GetGlobalIdentifierParentKeywordText( const AUniqID inIdentifierID, AText& outText ) const;//RC2
	AIndex					GetGlobalIdentifierCategoryIndex( const AUniqID inIdentifierID ) const;
	AIndex					GetGlobalIdentifierOutlineLevel( const AUniqID inIdentifierID ) const;//#130
	ABool					IsMenuIdentifier( const AUniqID inIdentifierID ) const;
	AUniqID					GetNextGlobalIdentifier( const AUniqID inIdentifierID ) const;
	AIdentifierType			GetGlobalIdentifierType( const AUniqID inIdentifierID ) const;
	/*#717
	void					GetAbbrevCandidateGlobal( const AText& inText,
							ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
							AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray ) const;//RC2
	void					GetAbbrevCandidateGlobalByParentKeyword( const AText& inText,
							ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
							AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray ) const;//R0243
	*/
	ABool					SearchKeywordGlobal( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
												const AIndex inCurrentStateIndex,
												AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
												ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
												ATextArray& outCommentTextArray,
												ATextArray& outCompletionTextArray, ATextArray& outURLArray,
												AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
												AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
												AArray<AScopeType>& outScopeArray,
												AArray<AItemCount>& outMatchedCountArray,
												const ABool& inAbort ) const;//#853
	ABool					SearchKeywordLocal( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
											   const AIndex inCurrentStateIndex,
											   const AIndex inStartLineIndex, const AIndex inEndLineIndex,
											   AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
											   ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
											   ATextArray& outCommentTextArray,
											   ATextArray& outCompletionTextArray, ATextArray& outURLArray,
											   AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
											   AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
											   AArray<AScopeType>& outScopeArray,
											   AArray<AItemCount>& outMatchedCountArray,
											   const ABool& inAbort ) const;//#853
	void					GetGlobalIdentifierListByKeyword( const AText& inText, AArray<AUniqID>& outIdentifierIDArray ) const;
	AItemCount				GetGlobalIdentifierListCount() const;
	//#348 const AText&			GetKeywordTextByGlobalIdentifierIndex( const AIndex inIndex ) const;
	void					GetKeywordTextByGlobalIdentifierIndex( const AIndex inIndex, AText& outText ) const;//#348
	ABool					CompareKeywordTextByGlobalIdentifierIndex( const AIndex inIndex, const AText& inText ) const;//#348
	AIndex					GetCategoryIndexByGlobalIdentifierIndex( const AIndex inIndex ) const;
	AIndex					GetColorSlotIndexByGlobalIdentifierIndex( const AIndex inIndex ) const;
	//#348 const AText&			GetInfoTextByGlobalIdentifierIndex( const AIndex inIndex ) const;
	void					GetInfoTextByGlobalIdentifierIndex( const AIndex inIndex, AText& outText ) const;//#348
	AUniqID					GetUniqIDByGlobalIdentifierIndex( const AIndex inIndex ) const;
	//#348 const AText&			GetCommentTextByGlobalIdentifierIndex( const AIndex inIndex ) const;//RC2
	void					GetCommentTextByGlobalIdentifierIndex( const AIndex inIndex, AText& outText ) const;//#348
	//#348 const AText&			GetParentKeywordTextByGlobalIdentifierIndex( const AIndex inIndex ) const;//RC2
	void					GetParentKeywordTextByGlobalIdentifierIndex( const AIndex inIndex, AText& outText ) const;//#348
	//#348 const AText&			GetTypeTextByGlobalIdentifierIndex( const AIndex inIndex ) const;//R0243
	void					GetTypeTextByGlobalIdentifierIndex( const AIndex inIndex, AText& outText ) const;//#348
	ABool					GetGlobalIdentifierTypeTextByKeywordText( const AText& inKeywordText, AText& outTypeText ) const;//R0243
	//
	AUniqID				GetLocalIdentifierByKeywordText( const AText& inKeywordText ) const;
	void					GetLocalIdentifierListByKeywordText( const AText& inKeywordText, AArray<AUniqID>& outUniqIDArray ) const;
	void					GetLocalIdentifierInfoText( const AUniqID inIdentifierID, AText& outText ) const;
	AIndex					GetLocalIdentifierCategoryIndex( const AUniqID inIdentifierID ) const;
	//#348 const AText&			GetLocalIdentifierTypeText( const AUniqID inIdentifierID ) const;//R0243
	void					GetLocalIdentifierTypeText( const AUniqID inIdentifierID, AText& outText ) const;//#348
	AIndex					GetLocalIdentifierCategoryIndexByKeywordText( const AText& inKeywordText ) const;
	void					GetAbbrevCandidateLocal( const AText& inText, ATextArray& outAbbrevCandidateArray, AArray<AUniqID>& outIdentifierIDArray ) const;
	void					GetLocalIdentifierListByKeyword( const AText& inText, AArray<AUniqID>& outIdentifierIDArray ) const;
	
  private:
	void					AddBlockStartData( const AIndex inLineIndex, const AIndex inVariableIndex, const AItemCount inIndentCount );
	AItemCount				FindLastBlockStartIndentCount( const AIndex inStartLineIndex, const AIndex inVariableIndex ) const;
	
	//void					SetGlobalIdentifierStartEndIndex( const AUniqID inUniqID, const AIndex inStartIndex, const Aindex inEndIndex );
	
	//#467
  public:
	void					SetSystemHeaderMode() { mSystemHeaderMode = true; }
  protected:
	ABool								mSystemHeaderMode;
	
	/*#695
  protected:
	//行情報
	AArray<AIndex>						mLineInfo_Start;
	AArray<AItemCount>					mLineInfo_Length;
	AArray<ABool>						mLineInfo_ExistLineEnd;
	AArray<ABool>						mLineInfoP_Recognized;
	AArray<AIndex>						mLineInfoP_StateIndex;
	AArray<AIndex>						mLineInfoP_PushedStateIndex;
	AArray<AUniqID>						mLineInfo_FirstGlobalIdentifierUniqID;
	AArray<AUniqID>						mLineInfoP_FirstLocalIdentifierUniqID;
	AArray<ABool>						mLineInfoP_ColorizeLine;
	AArray<AColor>						mLineInfoP_LineColor;
	AArray<AItemCount>					mLineInfoP_IndentCount;
	AArray<AObjectID>					mLineInfoP_BlockStartDataObjectID;
	AArray<AIndex>						mLineInfo_ParagraphIndex;//R0208
	AArray<ANumber>						mLineInfoP_Multiply;//#450
	** Directiveレベル *
	AArray<AIndex>						mLineInfoP_DirectiveLevel;//#467
	** 無効行の開始Directiveレベル *
	AArray<AIndex>						mLineInfoP_DisabledDirectiveLevel;//#467 
	** 正規表現グループ色づけ *#603
	AArray<AColor>						mLineInfoP_RegExpGroupColor;//#603
	AArray<AIndex>						mLineInfoP_RegExpGroupColor_StartIndex;//#603
	AArray<AItemCount>					mLineInfoP_RegExpGroupColor_EndIndex;//#603
	*/
	
	//識別子
	AIdentifierList						mGlobalIdentifierList;
	/**
	@note mGlobalIdentifierListMutexは、メインスレッドでは書き込み時のみロックする。サブスレッドでは読み込みのみ行い、読み込み時にロックする。
	*/
	mutable AThreadMutex				mGlobalIdentifierListMutex;//#717
	//
	AIdentifierList						mLocalIdentifierList;
	/**
	@note mLocalIdentifierListMutexは、メインスレッドでは書き込み時のみロックする。サブスレッドでは読み込みのみ行い、読み込み時にロックする。
	*/
	mutable AThreadMutex				mLocalIdentifierListMutex;//#717
	//
	ABlockStartDataList					mBlockStartDataList;
	
	//Importファイル
  public:
	void					GetImportFileArray( const AFileAcc& inBaseFolder, AObjectArray<AFileAcc>& outFileArray );
	
	//
  public:
	AIndex					MatchKeywordRegExp( const AText& inKeyword ) const;
	
	//ワードリスト #723
  public:
	void					UpdateWordsList( const AText& inText );
	void					FindWordsList( const AText& inWord, AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray ) const;
	void					DeleteAllWordsList();
  private:
	ATextArray						mWordsList_Word;
	AArray<AIndex> 					mWordsList_WordStartTextIndex;
	mutable AThreadMutex			mWordsListMutex;
	
	//ヒントテキスト
  public:
	AIndex					AddHintText( const ATextIndex inTextIndex, const AText& inHintText );
	void					ClearHintText();
	void					GetHintText( const ATextIndex inTextIndex, AText& outHintText ) const;
	void					UpdateHintTextByInsertText( const ATextIndex inTextIndex, const AIndex inTextCount );
	void					UpdateHintTextByDeleteText( const ATextIndex inStart, const ATextIndex inEnd );
	ATextIndex				GetHintTextPos( const AIndex inHintTextIndex ) const;
	AIndex					FindHintIndexFromTextIndex( const ATextIndex inHintTextPos ) const;
	AItemCount				GetHintTextCount() const { return mHintTextArray_TextIndex.GetItemCount(); }
  private:
	AHashArray<ATextIndex>					mHintTextArray_TextIndex;
	ATextArray							mHintTextArray_Text;
	
	//Debug
  public:
	void					CheckLineInfoDataForDebug();
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "ATextInfo"; }
	
};

#pragma mark ===========================
#pragma mark [クラス]ATextInfoForDocument
class ATextInfoForDocument : public ATextInfo
{
	//コンストラクタ、デストラクタ
  public:
	ATextInfoForDocument( AObjectArrayItem* inParent ) : ATextInfo(inParent) {}
	
  public:
	ABool					GetLineRecognized( const AIndex inLineIndex ) const;
	//void					SetLineRecognized( const AIndex inLineIndex, const ABool inRecognized );
	void					GetLineStateIndex( const AIndex inLineIndex, AIndex& outStateIndex, AIndex& outPushedStateIndex ) const;
	ABool					GetLineStartIsCode( const AIndex inLineIndex ) const;//#1003
	void					GetIsCodeArray( const AText& inText, const AIndex inLineIndex, AArray<ABool>& outIsCodeArray ) const;
	void					GetIsCommentArray( const AText& inText, const AIndex inLineIndex, AArray<ABool>& outIsCommentArray ) const;
	void					GetCurrentStateIndexAndName( const AText& inText, const ATextPoint inTextPoint, 
												AIndex& outStateIndex, AIndex& outPushedStateIndex,
												AText& outStateText, AText& outPushedStateText ) const;
	void					GetCurrentStateIndex( const AText& inText, const ATextPoint inTextPoint, 
												 AIndex& outStateIndex, AIndex& outPushedStateIndex ) const;
	AUniqID					GetFirstLocalIdentifierUniqID( const AIndex inLineIndex ) const;
	void					GetLocalRangeIdentifier( const AIndex inStartLineIndex, const AIndex inEndLineIndex, 
							AObjectArrayItem* inParent, AHashObjectArray<ALocalIdentifierData,AText>& outArray ) const;
	AIndex					GetLineIndexByLocalIdentifier( const AUniqID inIdentifierUniqID ) const;
	void					GetLocalIdentifierRange( const AUniqID inIdentifierUniqID, ATextPoint& outStart, ATextPoint& outEnd ) const;
	ABool					GetLineColor( const AIndex inLineIndex, AColor& outColor ) const;
	AItemCount				GetIndentCount( const AIndex inLineIndex ) const;
	ANumber					GetLineMultiply( const AIndex inLineIndex ) const;//#450
	AIndex					GetDirectiveLevel( const AIndex inLineIndex ) const;//#467
	AIndex					GetDisabledDirectiveLevel( const AIndex inLineIndex ) const;//#467
	void					GetLineRegExpGroupColor( const AIndex inLineIndex,
							AColor& outColor, AIndex& outStartIndex, AItemCount& outLength ) const;//#603
	//★const AArray<short int>&	GetMultiplyRef() const { return mLineInfoP_Multiply; }//#695
	ABool					IsStableStateLine( const AIndex inLineIndex ) const;//#695
	AIndex					GetPrevStableStateLineIndex( const AIndex inLineIndex ) const;//#695
	void					SetLineCheckedDate( const AIndex inLineIndex, const ANumber inDate );//#842
	ANumber					GetLineCheckedDate( const AIndex inLineIndex ) const;//#842
	void					GetTotalWordCount( AItemCount& outTotalCharCount, AItemCount& outTotalWordCount ) const;//#142
	ABool					GetSyntaxWarning( const AIndex inLineIndex ) const;
	ABool					GetSyntaxError( const AIndex inLineIndex ) const;
	
	//#450 折りたたみ
  public:
	AFoldingLevel			GetFoldingLevel( const AIndex inLineIndex ) const;
	//★const AArray<AFoldingLevel>&	GetFoldingLevelRef() const { return mLineInfoP_FoldingLevel; }//#695
};


