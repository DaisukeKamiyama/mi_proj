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

AThread_CandidateFinder
#717

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextInfo.h"

/**
候補優先順位タイプ
*/
enum ACandidatePriority
{
	kCandidatePriority_None = 0,
	kCandidatePriority_History = 1,
	kCandidatePriority_ParentKeyword = 2,
	kCandidatePriority_ParentKeywordWithHistory = 3,
	kCandidatePriority_Local = 4,
	kCandidatePriority_Highest = 4
};

#pragma mark ===========================
#pragma mark [クラス]AThread_CandidateFinder
/**
AThread_CandidateFinderスレッドオブジェクト
*/
class AThread_CandidateFinder : public AThread
{
	//コンストラクタ
  public:
	AThread_CandidateFinder( AObjectArrayItem* inParent );
	~AThread_CandidateFinder();
	
	//スレッドメインルーチン
  private:
	void	NVIDO_ThreadMain();
	
	//要求データ設定
  public:
	void						SetRequestData( const ADocumentID inTextDocumentID, const AViewID inTextViewID,
											   const AText& inWord, const ATextArray& inParentWord,
											   const AIndex inCurrentStateIndex );
	//要求データ
	//mRequestDataMutexにより排他制御。
	//スレッド実行開始時に、mTextDocumentID等へコピーされる。
  private:
	ADocumentID					mRequestData_TextDocumentID;
	AViewID						mRequestData_TextViewID;
	AText						mRequestData_Word;
	ATextArray					mRequestData_ParentWord;
	mutable AThreadMutex		mRequestDataMutex;
	AIndex						mRequestData_CurrentStateIndex;
	
	//実行用データ
  private:
	ADocumentID					mTextDocumentID;
	AViewID						mTextViewID;
	AText						mWord;
	AHashTextArray				mParentWord;
	AIndex						mCurrentStateIndex;
	
	//Abortフラグ
  public:
	ABool						mAbortCurrentRequest;
	
	//結果データ取得
  public:
	void						GetWordForResult( AText& outWordForResult, ATextArray& outParentWordForResult ) const;
	void						GetResult( ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
										  AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
										  ATextArray& outFilePathArray, AArray<ACandidatePriority>& outPriorityArray,
										  ATextArray& outParentKeywordArray, ATextArray& outCompletionTextArray,
										  AArray<AItemCount>& outMatchedCountArray ) const;
  private:
	//結果データ
	//mResultMutexにより排他制御。
	//スレッド内で検索実行後、ローカル変数からコピーされる。
	AText						mResult_WordForResult;
	ATextArray					mResult_ParentWordForResult;
	ATextArray					mResult_KeywordArray;
	ATextArray					mResult_InfoTextArray;
	ATextArray					mResult_CompletionTextArray;
	AArray<AIndex>				mResult_CategoryIndexArray;
	AArray<AScopeType>			mResult_ScopeArray;
	ATextArray					mResult_FilePathArray;
	AArray<ACandidatePriority>	mResult_PriorityArray;
	ATextArray					mResult_ParentKeywordArray;
	AArray<AItemCount>			mResult_MatchedCountArray;
	mutable AThreadMutex		mResultMutex;
};


#pragma mark ===========================
#pragma mark [クラス]AThread_CandidateFinderFactory
//生成Factory
class AThread_CandidateFinderFactory : public AThreadFactory
{
  public:
	AThread_CandidateFinderFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AThread_CandidateFinder*	Create() 
	{
		return new AThread_CandidateFinder(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};
