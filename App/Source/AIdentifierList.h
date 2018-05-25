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

AIdentifierList

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

enum AIdentifierType
{
	kIdentifierType_GlobalIdentifier = 0,
	kIdentifierType_ImportFile,
	kIdentifierType_KeywordInSyntaxDefinitionFile,
	kIdentifierType_AdditionalKeyword,
	kIdentifierType_JumpSetupRegExp,
	kIdentifierType_LocalIdentifier,
	kIdentifierType_OutlineHeader,//#130
};

#pragma mark ===========================
#pragma mark [クラス]AIdentifierList
//識別子リストクラス
class AIdentifierList : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	AIdentifierList( AObjectArrayItem* inParent );
	~AIdentifierList();
	
	//識別子の追加／削除
  public:
	AUniqID					AddIdentifier( const AUniqID inLineUniqID, //#695
										  const AUniqID inPreviousIdentifierUniqID, const AIdentifierType inType, 
										  const AIndex inCategoryIndex, const AText& inKeywordText, 
										  const AText& inMenuText, const AText& inInfoText,
										  const ATextIndex inStartIndex, const ATextIndex inEndIndex, const ABool inIgnoreCase,//B0368
										  const AText& inCommentText, const AText& inParentKeywordText, const AText& inTypeText,
										  const AText& inCompletionText, const AText& inURLText,
										  const ABool inLocalRangeDelimiter, 
										  const AIndex inOutlineLevel, const AIndex inColorSlot );//RC2 #593 #130 #844
	void					DeleteIdentifiers( const AUniqID inFirstUniqID );
	void					DeleteAllIdentifiers();//#641
	
	//データ取得（UniqIDから取得）
  public:
	ABool					GetKeywordText( const AUniqID inUniqID, AText& outText ) const;
	AUniqID					GetLineUniqID( const AUniqID inUniqID ) const;//#695
	//#348 const AText&			GetKeywordText( const AUniqID inUniqID ) const;
	AIndex					GetCategoryIndex( const AUniqID inUniqID ) const;
	void					GetMenuText( const AUniqID inUniqID, AText& outText ) const;
	void					GetInfoText( const AUniqID inUniqID, AText& outText ) const;
	//#348 const AText&			GetInfoText( const AUniqID inUniqID ) const;//RC2
	void					GetCommentText( const AUniqID inUniqID, AText& outText ) const;//RC2
	void					GetParentKeywordText( const AUniqID inUniqID, AText& outText ) const;//RC2
	ABool					IsMenuIdentifier( const AUniqID inIdentifierID ) const;
	AUniqID					GetNextUniqID( const AUniqID inUniqID ) const;
	AIndex					GetStartIndex( const AUniqID inUniqID ) const;
	AIndex					GetEndIndex( const AUniqID inUniqID ) const;
	//#348 const AText&			GetTypeText( const AUniqID inUniqID ) const;//R0243
	void					GetTypeText( const AUniqID inUniqID, AText& outText ) const;//#348
	AIdentifierType			GetIdentifierType( const AUniqID inUniqID ) const;
	ABool					IsLocalRangeDelimiter( const AUniqID inUniqID ) const;//#593
	AIndex					GetOutlineLevel( const AUniqID inUniqID ) const;//#130
	AIndex					GetKeywordColorSlot( const AUniqID inUniqID ) const;//#844
	
	//データ取得（キーワードから検索）
  public:
	ABool					SearchKeyword( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
										  const AModeIndex inModeIndex, const AIndex inCurrentStateIndex,
										  const AHashArray<AUniqID>& inLimitLineUniqIDArray,
										  AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
										  ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
										  ATextArray& outCommentTextArray,
										  ATextArray& outCompletionTextArray, ATextArray& outURLArray,
										  AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
										  AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
										  AArray<AItemCount>& outMatchedCountArray,
										  AArray<AUniqID>& outLineUniqIDArray,
										  const ABool& inAbort ) const;//#853
	/*#717
	void					GetAbbrevCandidate( const AText& inText, 
							ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
							AArray<AIndex>& outCategoryIndexArray ) const;//RC2
	void					GetAbbrevCandidateByParentKeyword( const AText& inParentKeyword,
							ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
							AArray<AIndex>& outCategoryIndexArray ) const;//R0243
	*/
	void					GetIdentifierListByKeyword( const AText& inText, AArray<AUniqID>& outIdentifierIDArray ) const;
	AUniqID					GetIdentifierByKeywordText( const AText& inKeywordText,
							const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray ) const;//R0000 カテゴリー可否) const;
	AIndex					GetIdentifierIndexByKeywordText( const AText& inKeywordText, 
							const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray ) const;//#348
	AIndex					GetCategoryIndexByKeywordText( const AText& inKeywordText,
							const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray,
							AIndex& outColorSlot ) const;//R0000 カテゴリー可否 #844
	ABool					GetTypeTextByKeywordText( const AText& inKeywordText, 
							const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray, AText& outTypeText ) const;//R0243
	
	//データ取得（Indexから取得）
  public:
	//#348 const AText&			GetKeywordTextByIdentifierIndex( const AIndex inIndex ) const;
	void					GetKeywordTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const;//#348
	ABool					CompareKeywordTextByIdentifierIndex( const AIndex inIndex, const AText& inText ) const;//#348
	AIndex					GetCategoryIndexByIdentifierIndex( const AIndex inIndex ) const;
	AIndex					GetColorSlotIndexByIdentifierIndex( const AIndex inIndex ) const;
	//#348 const AText&			GetInfoTextByIdentifierIndex( const AIndex inIndex ) const;
	void					GetInfoTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const;//#348
	AItemCount				GetItemCount() const { return /*#348 mIdentifierArray*/mIdentifierArray_KeywordText.GetItemCount(); }
	AUniqID					GetUniqIDByIdentifierIndex( const AIndex inIndex ) const;
	//#348 const AText&			GetCommentTextByIdentifierIndex( const AIndex inIndex ) const;//RC2
	void					GetCommentTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const;//#348
	//#348 const AText&			GetParentKeywordTextByIdentifierIndex( const AIndex inIndex ) const;//RC2
	void					GetParentKeywordTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const;//#348
	//#348 const AText&			GetTypeTextByIdentifierIndex( const AIndex inIndex ) const;//R0243
	void					GetTypeTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const;//#348
	//#348 private:
	//#348 AHashObjectArray<AIdentifier,AText>	mIdentifierArray;
	
	//データ
	//#348 AIdentifierは廃止。AIdentifierList内でArrayとして持つ。
  private:
	AHashTextArray						mIdentifierArray_KeywordText;
	AArray<AUniqID>						mIdentifierArray_LineUniqID;//#695
	ATextArray							mIdentifierArray_OriginalKeywordText;//#1277
	ATextArray							mIdentifierArray_MenuText;
	ATextArray							mIdentifierArray_InfoText;
	ATextArray							mIdentifierArray_CommentText;
	ATextArray							mIdentifierArray_ParentKeywordText;
	ATextArray							mIdentifierArray_TypeText;
	AArray<AIdentifierType>				mIdentifierArray_IdentifierType;
	AArray<char>						mIdentifierArray_CategoryIndex;
	AArray<AIndex>						mIdentifierArray_StartIndex;
	AArray<AIndex>						mIdentifierArray_EndIndex;
	AArray<ABool>						mIdentifierArray_IgnoreCase;
	AArray<AUniqID>						mIdentifierArray_NextUniqID;
	AArray<ABool>						mIdentifierArray_LocalRangeDelimiter;//#593
	AArray<AIndex>						mIdentifierArray_OutlineLevel;//#130
	AArray<char>						mIdentifierArray_ColorSlot;//#844
	ATextArray							mIdentifierArray_CompletionText;
	ATextArray							mIdentifierArray_URL;
	
	//情報テキストが存在する場合に、検索の重複結果を許すかどうかの設定
  public:
	void					SetDontCheckDuplicateIfInfoTextExists() { mDontCheckDuplicateIfInfoTextExists = true; }
  private:
	ABool								mDontCheckDuplicateIfInfoTextExists;
	
	//#717
	//
  public:
	void					RegisterParentChild( const AText& inParentKeyword, const AText& inChildKeyword );
  private:
	AHashTextArray						mParentChildArray_ParentKeyword;
	AObjectArray< ATextArray >			mParentChildArray_ChildKeywords;
	
	//Object情報取得
  public:
	AByteCount				GetAllocatedSize() const;
	AConstCharPtr			GetClassName() const { return "AIdentifierList"; }
	
};

/*#348
#pragma mark ===========================
#pragma mark [クラス]AIdentifierFactory
//
class AIdentifierFactory : public AAbstractFactoryForObjectArray<AIdentifier>
{
  public:
	AIdentifierFactory( AObjectArrayItem* inParent, const AIdentifierType inIdentifierType, const AIndex inCategoryIndex, const AText& inKeywordText, 
			const AText& inMenuText, const AText& inInfoText, const AIndex inStartIndex, const AIndex inEndIndex, const ABool inIgnoreCase,
			const AText& inCommentText, const AText& inParentKeywordText, const AText& inTypeText )//B0368, RC2
			: mParent(inParent), mKeywordText(inKeywordText), mIdentifierType(inIdentifierType), mCategoryIndex(inCategoryIndex),
			mMenuText(inMenuText), mInfoText(inInfoText), mStartIndex(inStartIndex), mEndIndex(inEndIndex), mIgnoreCase(inIgnoreCase),
			mCommentText(inCommentText), mParentKeywordText(inParentKeywordText), mTypeText(inTypeText)//RC2
	{
	}
	AIdentifier*	Create() 
	{
		return new AIdentifier(mParent,mIdentifierType,mCategoryIndex,mKeywordText,mMenuText,mInfoText,mStartIndex,mEndIndex,mIgnoreCase,
		mCommentText,mParentKeywordText,mTypeText);//B0368,RC2
	}
  private:
	AObjectArrayItem*	mParent;
	const AText&		mKeywordText;
	AIdentifierType		mIdentifierType;
	AIndex				mCategoryIndex;
	const AText&		mMenuText;
	const AText&		mInfoText;
	AIndex				mStartIndex;
	AIndex				mEndIndex;
	ABool				mIgnoreCase;//B0368
	const AText&		mCommentText;//RC2
	const AText&		mParentKeywordText;//RC2
	const AText&		mTypeText;//RC2
};
*/

