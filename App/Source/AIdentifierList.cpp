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

#include "stdafx.h"

#include "AIdentifierList.h"
#include "AApp.h"

//キーワードテキストが空の場合のマーク文字
const AUChar	kEmptyKeywordTextMark = 0x00;

#pragma mark ===========================
#pragma mark [クラス]AIdentifierList
#pragma mark ===========================
//
#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AIdentifierList::AIdentifierList( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent)//#348, mIdentifierArray(NULL,10)
//,mIdentifierArray_KeywordText(NULL)//#348
,mDontCheckDuplicateIfInfoTextExists(false)
{
}
//デストラクタ
AIdentifierList::~AIdentifierList()
{
}

#pragma mark ===========================

#pragma mark ---識別子の追加／削除

/**
識別子追加
*/
AUniqID	AIdentifierList::AddIdentifier( const AUniqID inLineUniqID, //#695
									   const AUniqID inPreviousIdentifierUniqID, const AIdentifierType inType, 
									   const AIndex inCategoryIndex, const AText& inKeywordText, 
									   const AText& inMenuText, const AText& inInfoText,
									   const ATextIndex inStartIndex, const ATextIndex inEndIndex, 
									   const ABool inIgnoreCase, //B0368
									   const AText& inCommentText, const AText& inParentKeywordText, const AText& inTypeText,
									   const AText& inCompletionText, const AText& inURLText,
									   const ABool inLocalRangeDelimiter, const AIndex inOutlineLevel, const AIndex inColorSlot )//RC2 #593 #130 #844
{
	//変換処理（AIdentifierから処理移動）
	AText	menuText(inMenuText);
	menuText.ReplaceChar(kUChar_Tab,kUChar_Space);//Tabをスペースに変換
	AText	infoText(inInfoText);
	infoText.ReplaceChar(kUChar_Tab,kUChar_Space);//Tabをスペースに変換
	AText	keywordText(inKeywordText);
	//#1266
	if( inIgnoreCase == true )
	{
		//キーワードを小文字化して保存
		keywordText.ChangeCaseLowerFast();
	}
	//キーワードテキストが空の場合は、hashが団子にならないよう、適当な（検索ヒットしない）テキストを設定する #695
	//AIdentifierList::SearchKeyword()にて、検索の対象外とする。
	if( keywordText.GetItemCount() == 0 )
	{
		keywordText.Add(kEmptyKeywordTextMark);
		keywordText.AddNumber(inLineUniqID.val);
	}
	
	//配列へ追加
	AIndex	index = mIdentifierArray_KeywordText.Add(keywordText);
	AUniqID	addedUniqID = mIdentifierArray_KeywordText.GetUniqIDByIndex(index);
	
	mIdentifierArray_LineUniqID.Add(inLineUniqID);//#695
	mIdentifierArray_MenuText.Add(menuText);
	mIdentifierArray_InfoText.Add(infoText);
	mIdentifierArray_CommentText.Add(inCommentText);
	mIdentifierArray_ParentKeywordText.Add(inParentKeywordText);
	mIdentifierArray_TypeText.Add(inTypeText);
	mIdentifierArray_NextUniqID.Add(kUniqID_Invalid);
	mIdentifierArray_IdentifierType.Add(inType);
	mIdentifierArray_CategoryIndex.Add(inCategoryIndex);
	mIdentifierArray_StartIndex.Add(inStartIndex);
	mIdentifierArray_EndIndex.Add(inEndIndex);
	mIdentifierArray_IgnoreCase.Add(inIgnoreCase);
	mIdentifierArray_LocalRangeDelimiter.Add(inLocalRangeDelimiter);//#593
	mIdentifierArray_OutlineLevel.Add(inOutlineLevel);//#130
	mIdentifierArray_ColorSlot.Add(inColorSlot);//#844
	mIdentifierArray_CompletionText.Add(inCompletionText);
	mIdentifierArray_URL.Add(inURLText);
	//リスト接続
	//inPreviousIdentifierUniqIDがkUniqID_Invalidでない場合（＝リスト先頭以外）、前のNextに自オブジェクトを設定する。
	if( inPreviousIdentifierUniqID != kUniqID_Invalid )
	{
		//前のIdentifierのNextUniqIDに、追加したIdentifierのUniqIDを設定する。
		AIndex	prevIndex = mIdentifierArray_KeywordText.GetIndexByUniqID(inPreviousIdentifierUniqID);
		if( mIdentifierArray_NextUniqID.Get(prevIndex) == kUniqID_Invalid )
		{
			mIdentifierArray_NextUniqID.Set(prevIndex,addedUniqID);
		}
		else
		{
			//prevにUniqID設定済み（最後尾追加でない）場合はthrow
			_ACThrow("not last object",this);
		}
	}
	
	return addedUniqID;
}

/**
識別子削除
リストに連なる識別子を全て削除する。
*/
void	AIdentifierList::DeleteIdentifiers( const AUniqID inFirstUniqID )
{
	/*#348
	//まず次の識別子以降を削除（再帰）
	AUniqID	nextUniqID = mIdentifierArray.GetObjectByID(inFirstUniqID).GetNextUniqID();
	if( nextUniqID != kUniqID_Invalid )
	{
		DeleteIdentifiers(nextUniqID);
	}
	//自身を削除
	//indexはここで再度検索する必要がある。（再帰コールで次以降が削除済みなので、最初とはIndexが変わっている。）
	AIndex	index = mIdentifierArray.GetIndexByUniqID(inFirstUniqID);//検討必要（速度）
	mIdentifierArray.Delete1Object(index);
	*/
	//まず次の識別子以降を削除（再帰）
	AIndex	index0 = mIdentifierArray_KeywordText.GetIndexByUniqID(inFirstUniqID);
	AUniqID	nextUniqID = mIdentifierArray_NextUniqID.Get(index0);
	if( nextUniqID == inFirstUniqID )   {_ACError("uniq id error (loop)",NULL);return;}
	if( nextUniqID != kUniqID_Invalid )
	{
		DeleteIdentifiers(nextUniqID);
	}
	//自身を削除
	//indexはここで再度検索する必要がある。（再帰コールで次以降が削除済みなので、最初とはIndexが変わっている。）
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inFirstUniqID);
	mIdentifierArray_KeywordText.Delete1(index);
	mIdentifierArray_LineUniqID.Delete1(index);//#695
	mIdentifierArray_MenuText.Delete1(index);
	mIdentifierArray_InfoText.Delete1(index);
	mIdentifierArray_CommentText.Delete1(index);
	mIdentifierArray_ParentKeywordText.Delete1(index);
	mIdentifierArray_TypeText.Delete1(index);
	mIdentifierArray_NextUniqID.Delete1(index);
	mIdentifierArray_IdentifierType.Delete1(index);
	mIdentifierArray_CategoryIndex.Delete1(index);
	mIdentifierArray_StartIndex.Delete1(index);
	mIdentifierArray_EndIndex.Delete1(index);
	mIdentifierArray_IgnoreCase.Delete1(index);
	mIdentifierArray_LocalRangeDelimiter.Delete1(index);//#593
	mIdentifierArray_OutlineLevel.Delete1(index);//#130
	mIdentifierArray_ColorSlot.Delete1(index);//#844
	mIdentifierArray_CompletionText.Delete1(index);
	mIdentifierArray_URL.Delete1(index);
}

//#641
/**
識別子全削除
*/
void	AIdentifierList::DeleteAllIdentifiers()
{
	mIdentifierArray_KeywordText.DeleteAll();
	mIdentifierArray_LineUniqID.DeleteAll();//#695
	mIdentifierArray_MenuText.DeleteAll();
	mIdentifierArray_InfoText.DeleteAll();
	mIdentifierArray_CommentText.DeleteAll();
	mIdentifierArray_ParentKeywordText.DeleteAll();
	mIdentifierArray_TypeText.DeleteAll();
	mIdentifierArray_NextUniqID.DeleteAll();
	mIdentifierArray_IdentifierType.DeleteAll();
	mIdentifierArray_CategoryIndex.DeleteAll();
	mIdentifierArray_StartIndex.DeleteAll();
	mIdentifierArray_EndIndex.DeleteAll();
	mIdentifierArray_IgnoreCase.DeleteAll();
	mIdentifierArray_LocalRangeDelimiter.DeleteAll();//#593
	mIdentifierArray_OutlineLevel.DeleteAll();//#130
	mIdentifierArray_ColorSlot.DeleteAll();//#844
	mIdentifierArray_CompletionText.DeleteAll();
	mIdentifierArray_URL.DeleteAll();
}

#pragma mark ===========================

#pragma mark ---データ取得（UniqIDから取得）

/**
UniqIDからキーワードテキスト取得
*/
ABool	AIdentifierList::GetKeywordText( const AUniqID inUniqID, AText& outText ) const
{
	//#348 outText.SetText(mIdentifierArray.GetObjectConstByID(inUniqID).GetKeywordText());
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_KeywordText.Get(index,outText);//#348
	ABool	keywordValid = false;
	if( outText.GetItemCount() > 0 )
	{
		//0x00で始まるキーワードはキーワードテキストが実際には空であることを示す。（hash団子対策）
		if( outText.Get(0) != kEmptyKeywordTextMark )
		{
			keywordValid = true;
		}
	}
	return keywordValid;
}

//#695
/**
行UniqIDを取得
*/
AUniqID	AIdentifierList::GetLineUniqID( const AUniqID inUniqID ) const
{
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);
	return mIdentifierArray_LineUniqID.Get(index);
}

/**
UniqIDからカテゴリーIndex取得
*/
AIndex	AIdentifierList::GetCategoryIndex( const AUniqID inUniqID ) const
{
	//#348 return mIdentifierArray.GetObjectConstByID(inUniqID).GetCategoryIndex();
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	return mIdentifierArray_CategoryIndex.Get(index);//#348
}

/**
UniqIDからメニューテキスト取得
*/
void	AIdentifierList::GetMenuText( const AUniqID inUniqID, AText& outText ) const
{
	//#348 outText.SetText(mIdentifierArray.GetObjectConstByID(inUniqID).GetMenuText());
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_MenuText.Get(index,outText);//#348
}

/**
UniqIDから情報テキスト取得
*/
void	AIdentifierList::GetInfoText( const AUniqID inUniqID, AText& outText ) const
{
	//#348 outText.SetText(mIdentifierArray.GetObjectConstByID(inUniqID).GetInfoText());
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_InfoText.Get(index,outText);//#348
}

//RC2
/**
UniqIDからコメントテキスト取得
*/
void	AIdentifierList::GetCommentText( const AUniqID inUniqID, AText& outText ) const
{
	//#348 outText.SetText(mIdentifierArray.GetObjectConstByID(inUniqID).GetCommentText());
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_CommentText.Get(index,outText);//#348
}

//RC2
/**
UniqIDからParentキーワードテキスト取得
*/
void	AIdentifierList::GetParentKeywordText( const AUniqID inUniqID, AText& outText ) const
{
	//#348 outText.SetText(mIdentifierArray.GetObjectConstByID(inUniqID).GetParentKeywordText());
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_ParentKeywordText.Get(index,outText);//#348
}

/**
UniqIDから、見出しメニュー表示対象かどうかを取得
*/
ABool	AIdentifierList::IsMenuIdentifier( const AUniqID inUniqID ) const
{
	//#348 return (mIdentifierArray.GetObjectConstByID(inUniqID).GetMenuText().GetItemCount() > 0);
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	AText	menuText;//#348
	mIdentifierArray_MenuText.Get(index,menuText);//#348
	return (menuText.GetItemCount() > 0);//#348
}

/**
UniqIDから、リストの次のUniqIDを取得
*/
AUniqID	AIdentifierList::GetNextUniqID( const AUniqID inUniqID ) const
{
	//#348 return mIdentifierArray.GetObjectConstByID(inUniqID).GetNextUniqID();
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	return mIdentifierArray_NextUniqID.Get(index);//#348
}

/**
UniqIDから、開始Indexを取得
*/
AIndex	AIdentifierList::GetStartIndex( const AUniqID inUniqID ) const
{
	//#348 return mIdentifierArray.GetObjectConstByID(inUniqID).GetStartIndex();
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	return mIdentifierArray_StartIndex.Get(index);//#348
}

/**
UniqIDから、終了Indexを取得
*/
AIndex	AIdentifierList::GetEndIndex( const AUniqID inUniqID ) const
{
	//#348 return mIdentifierArray.GetObjectConstByID(inUniqID).GetEndIndex();
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	return mIdentifierArray_EndIndex.Get(index);//#348
}

/**
UniqIDから、Typeテキストを取得
*/
void	AIdentifierList::GetTypeText( const AUniqID inUniqID, AText& outText ) const
{
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	mIdentifierArray_TypeText.Get(index,outText);//#348
}

/**
UniqIDから、識別子タイプを取得
*/
AIdentifierType	AIdentifierList::GetIdentifierType( const AUniqID inUniqID ) const
{
	//#348 return mIdentifierArray.GetObjectConstByID(inUniqID).GetIdentifierType();
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);//#348
	return mIdentifierArray_IdentifierType.Get(index);//#348
}

//#593
/**
UniqIDから、ローカル識別子の範囲区切りとなるかどうかを取得
*/
ABool	AIdentifierList::IsLocalRangeDelimiter( const AUniqID inUniqID ) const
{
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);
	return mIdentifierArray_LocalRangeDelimiter.Get(index);
}

//#130
/**
UniqIDから、Outline level取得
*/
AIndex	AIdentifierList::GetOutlineLevel( const AUniqID inUniqID ) const
{
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);
	return mIdentifierArray_OutlineLevel.Get(index);
}

//#844
/**
UniqIDから、カラースロット取得
*/
AIndex	AIdentifierList::GetKeywordColorSlot( const AUniqID inUniqID ) const
{
	AIndex	index = mIdentifierArray_KeywordText.GetIndexByUniqID(inUniqID);
	return mIdentifierArray_ColorSlot.Get(index);
}

#pragma mark ===========================

#pragma mark ---データ取得（キーワードから検索）

//#853 #717
/**
各種キーワード検索スレッド用統合キーワード検索処理
@note ハッシュを使わないので低速なため、スレッド使用が必須だが、いろいろな検索方法ができる。
outLineUniqIDArray以外は、項目を蓄積する。
*/
ABool	AIdentifierList::SearchKeyword( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
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
									   const ABool& inAbort ) const
{
	//outLineUniqIDArrayのみ項目削除
	//それ以外の結果配列は、項目を蓄積するので、削除しない。
	outLineUniqIDArray.DeleteAll();
	/*
	//検索ワードを取得
	AText	origword;
	origword.SetText(inWord);
	//件枠ワードを大文字小文字無視オプション有りなら小文字に変換
	if( (inOption&kKeywordSearchOption_IgnoreCases) != 0 )
	{
		word.ChangeCaseLowerFast();
	}
	*/
	//小文字化した場合の検索キーワードを取得
	AText	wordLowerCase;
	wordLowerCase.SetText(inWord);
	wordLowerCase.ChangeCaseLower();
	//親キーワードと比較するかどうかのフラグを設定
	ABool	compareParentKeyword = ((inOption&kKeywordSearchOption_CompareParentKeyword)!=0);
	//検索対象の行が限定されているかどうかのフラグを設定
	ABool	limitLineUniqID = (inLimitLineUniqIDArray.GetItemCount() > 0 );
	//各identifier毎のループ
	AItemCount	itemCount = mIdentifierArray_KeywordText.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//abortフラグonなら終了（別スレッドから設定される）
		if( inAbort == true )
		{
			break;
		}
		//対象行限定なら、指定行に含まれる行でない場合は、次のidentifierへ。
		if( limitLineUniqID == true )
		{
			if( inLimitLineUniqIDArray.Find(mIdentifierArray_LineUniqID.Get(i)) == kIndex_Invalid )
			{
				continue;
			}
		}
		//親キーワードとの比較フラグONなら、親キーワードが指定親キーワードリスト内に存在しないなら、次のidentifierへ
		if( compareParentKeyword == true )
		{
			AText	parentKeywordText;
			mIdentifierArray_ParentKeywordText.Get(i,parentKeywordText);
			if( inParentWord.Find(parentKeywordText) == kIndex_Invalid )
			{
				continue;
			}
		}
		//キーワード取得
		AText	keywordText;
		mIdentifierArray_KeywordText.Get(i,keywordText);
		//キーワードが空なら次のidentifierへ
		if( keywordText.GetItemCount() == 0 )
		{
			continue;
		}
		//キーワードが0x00で始まるなら次のidentifierへ
		if( keywordText.Get(0) == kEmptyKeywordTextMark )
		{
			continue;
		}
		//カテゴリ=-1なら対象外。次のidentifierへ。
		AIndex	categoryIndex = mIdentifierArray_CategoryIndex.Get(i);
		if( categoryIndex == kIndex_Invalid )
		{
			continue;
		}
		//大文字小文字無視なら、識別子のキーワードを小文字にする。
		ABool	ignoreCase = false;
		if( (inOption&kKeywordSearchOption_IgnoreCases) != 0 || mIdentifierArray_IgnoreCase.Get(i) == true )
		{
			//識別子のキーワードを小文字化
			keywordText.ChangeCaseLowerFast();
			//大文字小文字無視フラグをON
			ignoreCase = true;
		}
		//比較キーワード取得（大文字小文字無視なら引数を小文字化したものを取得、大文字小文字無視でなければ引数そのまま）
		const AText&	word = (ignoreCase?wordLowerCase:inWord);
		//一致フラグ
		ABool	matched = false;
		//検索ワードが空なら常に一致
		if( word.GetItemCount() == 0 )
		{
			matched = true;
		}
		//検索ワードが空でないなら、キーワードと比較
		else
		{
			//部分一致
			if( (inOption&kKeywordSearchOption_Partial) != 0 )
			{
				if( keywordText.GetItemCount() >= word.GetItemCount() )
				{
					AIndex	pos = 0;
					matched = keywordText.FindText(0,word,pos);
				}
			}
			//前方一致
			else if( (inOption&kKeywordSearchOption_Leading) != 0 )
			{
				if( keywordText.GetItemCount() >= word.GetItemCount() )
				{
					matched = keywordText.CompareMin(word);
				}
			}
			//全体一致
			else
			{
				matched = keywordText.Compare(word);
			}
		}
		//InfoText内も含むオプションONの場合、info textから部分一致で検索
		if( matched == false )
		{
			if( (inOption&kKeywordSearchOption_IncludeInfoText) != 0 )
			{
				AText	infoText;
				mIdentifierArray_InfoText.Get(i,infoText);
				AIndex	pos = 0;
				matched = infoText.FindText(0,word,pos);
			}
		}
		//現在のstateで有効なカテゴリのみ対象とする検索オプションの処理
		if( (inOption&kKeywordSearchOption_OnlyEnabledCategoryForCurrentState) != 0 && inCurrentStateIndex != kIndex_Invalid )
		{
			if( GetApp().SPI_GetModePrefDB(inModeIndex).GetCategoryDisabled(inCurrentStateIndex,categoryIndex) == true )
			{
				matched = false;
			}
		}
		//一致した場合は、結果配列へ追加
		if( matched == true )
		{
			//キーワードテキスト取得
			AText	keyword;
			mIdentifierArray_KeywordText.Get(i,keyword);
			//親キーワード取得
			AText	parentKeywordText;
			mIdentifierArray_ParentKeywordText.Get(i,parentKeywordText);
			//情報テキスト取得
			AText	infoText;
			mIdentifierArray_InfoText.Get(i,infoText);
			//キーワード重複チェック
			//検索オプションkKeywordSearchOption_InhibitDuplicatedKeywordがONのときは重複チェックを行う
			//（ただし、mDontCheckDuplicateIfInfoTextExistsがtrueで、かつ、情報テキストが存在するときは、重複チェックしない。キーワード情報表示のため。）
			ABool	checkDuplicate = ((inOption&kKeywordSearchOption_InhibitDuplicatedKeyword)!= 0);
			if( mDontCheckDuplicateIfInfoTextExists == true && infoText.GetItemCount() > 0 )
			{
				checkDuplicate = false;
			}
			AIndex	duplicatedKeywordArrayIndex = kIndex_Invalid;
			if( checkDuplicate == true )
			{
				duplicatedKeywordArrayIndex = outKeywordArray.Find(keyword);
			}
			if( duplicatedKeywordArrayIndex != kIndex_Invalid )
			{
				//------------------重複キーワード有りの場合------------------
				//既に同じキーワードがある場合は追加せずにmatched countを+1する
				outMatchedCountArray.Set(duplicatedKeywordArrayIndex,outMatchedCountArray.Get(duplicatedKeywordArrayIndex)+1);
				//parent keywordが一致するときは、各データを上書きする
				if( inParentWord.Find(parentKeywordText) != kIndex_Invalid )
				{
					//現状、重複禁止オプションは候補検索だけでしか使わないので、表示データのみを上書きする。
					//start index等は、最初に見つかったデータのままとする。file path等のデータはこの関数の外で設定するので、
					//これらも上書きや追加するなら、関数インターフェイス変更必要。
					outParentKeywordArray.Set(duplicatedKeywordArrayIndex,parentKeywordText);
					outTypeTextArray.Set(duplicatedKeywordArrayIndex,mIdentifierArray_TypeText.GetTextConst(i));
					outInfoTextArray.Set(duplicatedKeywordArrayIndex,infoText);
					outCommentTextArray.Set(duplicatedKeywordArrayIndex,mIdentifierArray_CommentText.GetTextConst(i));
					outCompletionTextArray.Set(duplicatedKeywordArrayIndex,mIdentifierArray_CompletionText.GetTextConst(i));
					outURLArray.Set(duplicatedKeywordArrayIndex,mIdentifierArray_URL.GetTextConst(i));
					outCategoryIndexArray.Set(duplicatedKeywordArrayIndex,categoryIndex);
				}
			}
			else
			{
				//------------------重複キーワード無しの場合------------------
				//検索結果追加
				outKeywordArray.Add(keyword);
				outParentKeywordArray.Add(parentKeywordText);
				outTypeTextArray.Add(mIdentifierArray_TypeText.GetTextConst(i));
				outInfoTextArray.Add(infoText);
				outCommentTextArray.Add(mIdentifierArray_CommentText.GetTextConst(i));
				outCompletionTextArray.Add(mIdentifierArray_CompletionText.GetTextConst(i));
				outURLArray.Add(mIdentifierArray_URL.GetTextConst(i));
				outCategoryIndexArray.Add(categoryIndex);
				outStartIndexArray.Add(mIdentifierArray_StartIndex.Get(i));
				outColorSlotIndexArray.Add(mIdentifierArray_ColorSlot.Get(i));
				outEndIndexArray.Add(mIdentifierArray_EndIndex.Get(i));
				outLineUniqIDArray.Add(mIdentifierArray_LineUniqID.Get(i));
				outMatchedCountArray.Add(1);
				//制限値オーバーの場合は、終了。
				if( outKeywordArray.GetItemCount() >= kLimit_KeywordSearchResultCount )
				{
					return false;
				}
			}
		}
	}
	return true;
}

#if 0
//RC2
/**
補完入力候補追加（outAbbrevCandidateArray等には候補データが追加される。最初にDeleteAll()はされない）
*/
void	AIdentifierList::GetAbbrevCandidate( const AText& inText,
		ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
		AArray<AIndex>& outCategoryIndexArray ) const
{
	//#348 AItemCount	itemCount = mIdentifierArray.GetItemCount();
	AItemCount	itemCount = mIdentifierArray_KeywordText.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//#348 const AText&	keywordText = mIdentifierArray.GetObjectConst(i).GetKeywordText();
		AText	keywordText;//#348
		mIdentifierArray_KeywordText.Get(i,keywordText);//#348
		if( keywordText.GetItemCount() >= inText.GetItemCount() )
		{
			if( keywordText.CompareMin(inText) == true )
			{
				//重複キーワードを追加しないようにしようとしたがやめた（候補ウインドウ表示で情報が出なくなるため）if( outAbbrevCandidateArray.Find(keywordText) == kIndex_Invalid )
				{
					outAbbrevCandidateArray.Add(keywordText);
					//#348 outInfoTextArray.Add(mIdentifierArray.GetObjectConst(i).GetInfoText());
					//#348 outCategoryIndexArray.Add(mIdentifierArray.GetObjectConst(i).GetCategoryIndex());
					AText	infoText;//#348
					mIdentifierArray_InfoText.Get(i,infoText);//#348
					outInfoTextArray.Add(infoText);//#348
					outCategoryIndexArray.Add(mIdentifierArray_CategoryIndex.Get(i));//#348
				}
			}
		}
	}
}

//R0243
/**
補完入力候補追加（outAbbrevCandidateArray等には候補データが追加される。最初にDeleteAll()はされない）
Parentキーワード（クラス名等）から検索
*/
void	AIdentifierList::GetAbbrevCandidateByParentKeyword( const AText& inParentKeyword,
		ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
		AArray<AIndex>& outCategoryIndexArray ) const
{
	//#348 AItemCount	itemCount = mIdentifierArray.GetItemCount();
	AItemCount	itemCount = mIdentifierArray_KeywordText.GetItemCount();//#348
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//#348 const AText&	keywordText = mIdentifierArray.GetObjectConst(i).GetKeywordText();
		//#348 if( mIdentifierArray.GetObjectConst(i).GetParentKeywordText().Compare(inParentKeyword) == true )
		if( mIdentifierArray_ParentKeywordText.Compare(i,inParentKeyword) == true )//#348
		{
			AText	keywordText;//#348
			mIdentifierArray_KeywordText.Get(i,keywordText);//#348
			outAbbrevCandidateArray.Add(keywordText);
			//#348 outInfoTextArray.Add(mIdentifierArray.GetObjectConst(i).GetInfoText());
			//#348 outCategoryIndexArray.Add(mIdentifierArray.GetObjectConst(i).GetCategoryIndex());
			AText	infoText;//#348
			mIdentifierArray_InfoText.Get(i,infoText);//#348
			outInfoTextArray.Add(infoText);//#348
			outCategoryIndexArray.Add(mIdentifierArray_CategoryIndex.Get(i));//#348
		}
	}
}
#endif

/**
キーワード文字列から識別子のUniqIDのリストを取得
*/
void	AIdentifierList::GetIdentifierListByKeyword( const AText& inText, AArray<AUniqID>& outIdentifierIDArray ) const
{
	//
	outIdentifierIDArray.DeleteAll();
	
	//引数キーワードテキストが空なら何もせずリターン
	if( inText.GetItemCount() == 0 )   return;
	
	//#348 mIdentifierArray.FindUniqID(inText,0,kIndex_Invalid,outIdentifierIDArray);
	mIdentifierArray_KeywordText.FindUniqIDs(inText,0,kIndex_Invalid,outIdentifierIDArray);//#348
}

/**
キーワード文字列から識別子のUniqIDを取得（最初に見つかったもの）
*/
AUniqID	AIdentifierList::GetIdentifierByKeywordText( const AText& inKeywordText, 
			const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray ) const//R0000 カテゴリー可否
{
	AIndex	index = GetIdentifierIndexByKeywordText(inKeywordText,inNotUseDisabledCategoryArray,inDisabledCategoryArray);//#348
	if( index == kIndex_Invalid )   return kUniqID_Invalid;//#348
	return mIdentifierArray_KeywordText.GetUniqIDByIndex(index);//#348
}

/**
キーワード文字列から識別子のIndexを取得（最初に見つかったもの）
*/
AIndex	AIdentifierList::GetIdentifierIndexByKeywordText( const AText& inKeywordText, 
			const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray ) const//R0000 カテゴリー可否
{
	//#629 IgnoreCase分も全て含めて、カテゴリ番号が一番大きいもののを返すように変更
	
	//引数キーワードテキストが空なら何もせずリターン
	if( inKeywordText.GetItemCount() == 0 )   return kIndex_Invalid;
	
	//大文字小文字そのままで検索
	AArray<AIndex>	indexArray;
	mIdentifierArray_KeywordText.FindAll(inKeywordText,0,kIndex_Invalid,indexArray);
	//小文字化して検索
	AUChar	ch = inKeywordText.Get(0);
	if( IsUTF8Multibyte(ch) == false )//最初の文字がマルチバイト文字ならignore case処理しない
	{
		AText	lowerCaseText;
		lowerCaseText.SetText(inKeywordText);
		if( lowerCaseText.ChangeCaseLowerFast() == true )//変換しなかったなら検索不要
		{
			AArray<AIndex>	ignoreCaseIndexArray;
			mIdentifierArray_KeywordText.FindAll(lowerCaseText,0,kIndex_Invalid,ignoreCaseIndexArray);
			//IgnoreCaseに設定されている識別子のみindexArrayへコピーする
			AItemCount	count = ignoreCaseIndexArray.GetItemCount();
			for( AIndex i = 0; i < count; i++ )
			{
				AIndex	index = ignoreCaseIndexArray.Get(i);
				if( mIdentifierArray_IgnoreCase.Get(index) == true )
				{
					indexArray.Add(index);
				}
			}
		}
	}
	//
	AIndex	foundLargestCategoryIndex = kIndex_Invalid;
	AIndex	foundIdentifierIndex = kIndex_Invalid;
	//indexArrayの中から、Disableに指定されたカテゴリではないもののうち、カテゴリ番号が最大のものを検索
	AItemCount	count = indexArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AIndex	index = indexArray.Get(i);
		AIndex	categoryIndex = mIdentifierArray_CategoryIndex.Get(index);
		//disabledカテゴリでないかを判定
		if( inNotUseDisabledCategoryArray == true ||
					inDisabledCategoryArray.Get(categoryIndex) == false )
		{
			//カテゴリ番号が今までで最大かどうかを判定
			if( foundLargestCategoryIndex == kIndex_Invalid ||
						categoryIndex > foundLargestCategoryIndex )
			{
				//今までで最大のカテゴリ番号なので、indexとcategoryIndexを記憶
				foundLargestCategoryIndex = categoryIndex;
				foundIdentifierIndex = index;
			}
		}
	}
	return foundIdentifierIndex;
	/*#629
	//B0368 return mIdentifierArray.FindUniqID(inKeywordText);
	AIndex	resultIndex = kIndex_Invalid;
	if( inNotUseDisabledCategoryArray == true )
	{
		resultIndex = mIdentifierArray_KeywordText.Find(inKeywordText);
	}
	else
	{
		//R0000 カテゴリー可否
		AArray<AIndex>	indexArray;
		mIdentifierArray_KeywordText.FindAll(inKeywordText,0,kIndex_Invalid,indexArray);
		AItemCount	count = indexArray.GetItemCount();
		for( AIndex i = 0; i < count; i++ )
		{
			resultIndex = indexArray.Get(i);
			if( inDisabledCategoryArray.Get(mIdentifierArray_CategoryIndex.Get(resultIndex)) == false )
			{
				return resultIndex;
			}
		}
		resultIndex = kIndex_Invalid;
	}
	if( resultIndex != kIndex_Invalid )   return resultIndex;
	
	//B0368
	//小文字化して検索。ただし、AIdentifierオブジェクトがIgnoreCaseのもののみヒット。
	if( inKeywordText.GetItemCount() == 0 )   return kIndex_Invalid;
	//B0000 if( Macro_IsAlphabet(inKeywordText.Get(0)) == false )   return kUniqID_Invalid;
	AUChar	ch = inKeywordText.Get(0);
	if( IsUTF8Multibyte(ch) == true )   return kIndex_Invalid;//最初の文字がマルチバイト文字ならignore case処理しない
	
	AText	lowerCaseText;
	lowerCaseText.SetText(inKeywordText);
	if( lowerCaseText.ChangeCaseLowerFast() == false )   return kIndex_Invalid;//B0000 変換しなかったなら検索不要
	AArray<AIndex>	indexArray;
	mIdentifierArray_KeywordText.FindAll(lowerCaseText,0,kIndex_Invalid,indexArray);
	AItemCount	count = indexArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		resultIndex = indexArray.Get(i);
		if( mIdentifierArray_IgnoreCase.Get(resultIndex) == true )
		{
			if( inNotUseDisabledCategoryArray == true )
			{
				return resultIndex;
			}
			else
			{
				//R0000 カテゴリー可否
				if( inDisabledCategoryArray.Get(mIdentifierArray_CategoryIndex.Get(resultIndex)) == false )
				{
					return resultIndex;
				}
			}
		}
	}
	return kIndex_Invalid;
	*/
}

/**
キーワード文字列から検索、カテゴリIndexを取得
*/
AIndex	AIdentifierList::GetCategoryIndexByKeywordText( const AText& inKeywordText, 
		const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray,
		AIndex& outColorSlot ) const//R0000 カテゴリー可否 #844
{
	outColorSlot = kIndex_Invalid;
	AIndex	index = GetIdentifierIndexByKeywordText(inKeywordText,inNotUseDisabledCategoryArray,inDisabledCategoryArray);//#348
	if( index == kIndex_Invalid )   return kIndex_Invalid;//#348
	
	//#844
	outColorSlot = mIdentifierArray_ColorSlot.Get(index);
	//
	return mIdentifierArray_CategoryIndex.Get(index);//#348
}

//R0243
/**
キーワード文字列から検索、Typeテキストを取得
*/
ABool	AIdentifierList::GetTypeTextByKeywordText( const AText& inKeywordText, 
		const ABool inNotUseDisabledCategoryArray, const ABoolArray& inDisabledCategoryArray, AText& outTypeText ) const
{
	outTypeText.DeleteAll();
	AIndex	index = GetIdentifierIndexByKeywordText(inKeywordText,inNotUseDisabledCategoryArray,inDisabledCategoryArray);//#348
	if( index == kIndex_Invalid )   return false;//#348
	mIdentifierArray_TypeText.Get(index,outTypeText);//#348
	return true;
}

#pragma mark ===========================

#pragma mark ---データ取得（Indexから取得）

/**
Indexからキーワードテキスト取得
*/
void	AIdentifierList::GetKeywordTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mIdentifierArray_KeywordText.Get(inIndex,outText);//#348
}

//#348
/**
Indexからキーワードテキスト比較
*/
ABool	AIdentifierList::CompareKeywordTextByIdentifierIndex( const AIndex inIndex, const AText& inText ) const
{
	return mIdentifierArray_KeywordText.Compare(inIndex,inText);
}

/**
IndexからカテゴリーIndex取得
*/
AIndex	AIdentifierList::GetCategoryIndexByIdentifierIndex( const AIndex inIndex ) const
{
	//#348 return mIdentifierArray.GetObjectConst(inIndex).GetCategoryIndex();
	return mIdentifierArray_CategoryIndex.Get(inIndex);//#348
}

/**
IndexからカラースロットIndex取得
*/
AIndex	AIdentifierList::GetColorSlotIndexByIdentifierIndex( const AIndex inIndex ) const
{
	return mIdentifierArray_ColorSlot.Get(inIndex);
}

/**
Indexから情報テキスト取得
*/
void	AIdentifierList::GetInfoTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mIdentifierArray_InfoText.Get(inIndex,outText);//#348
}

/**
IndexからUniqID取得
*/
AUniqID	AIdentifierList::GetUniqIDByIdentifierIndex( const AIndex inIndex ) const
{
	//#348 return mIdentifierArray.GetObjectConst(inIndex).GetUniqID();
	return mIdentifierArray_KeywordText.GetUniqIDByIndex(inIndex);//#348
}

/**
Indexからコメントテキスト取得
*/
void	AIdentifierList::GetCommentTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mIdentifierArray_CommentText.Get(inIndex,outText);//#348
}

/**
IndexからParentキーワードテキスト取得
*/
void	AIdentifierList::GetParentKeywordTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mIdentifierArray_ParentKeywordText.Get(inIndex,outText);//#348
}

/**
IndexからTypeテキスト取得
*/
void	AIdentifierList::GetTypeTextByIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mIdentifierArray_TypeText.Get(inIndex,outText);//#348
}

/**
使用サイズ取得（デバッグ用）
*/
AByteCount	AIdentifierList::GetAllocatedSize() const
{
	return (mIdentifierArray_MenuText.GetTextDataSize()+
	mIdentifierArray_InfoText.GetTextDataSize()+
	mIdentifierArray_CommentText.GetTextDataSize()+
	mIdentifierArray_ParentKeywordText.GetTextDataSize()+
	mIdentifierArray_TypeText.GetTextDataSize());
}

