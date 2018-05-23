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

AThread_IdInfoFinder
#853

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextInfo.h"

#pragma mark ===========================
#pragma mark [クラス]AThread_IdInfoFinder
/**
AThread_IdInfoFinderスレッドオブジェクト
*/
class AThread_IdInfoFinder : public AThread
{
	//コンストラクタ
  public:
	AThread_IdInfoFinder( AObjectArrayItem* inParent );
	~AThread_IdInfoFinder();
	
	//スレッドメインルーチン
  private:
	void	NVIDO_ThreadMain();
	
	//要求データ設定
  public:
	void						SetRequestData( const ADocumentID inTextDocumentID, 
											   const AWindowID inIdInfoWindowID, 
											   const AWindowID inTextWindowID, const AViewID inTextViewID,
											   const AText& inWord, const ATextArray& inParentWord,
											   const AIndex inArgIndex, const AText& inCallerFuncIdText,
											   const AIndex inCurrentStateIndex );
	void						AbortCurrentRequest();
	//要求データ
	//mRequestDataMutexにより排他制御。
	//スレッド実行開始時に、mTextDocumentID等へコピーされる。
  private:
	ADocumentID					mRequestData_TextDocumentID;
	AWindowID					mRequestData_IdInfoWindowID;
	AWindowID					mRequestData_TextWindowID;
	AViewID						mRequestData_TextViewID;
	AText						mRequestData_Word;
	ATextArray					mRequestData_ParentWord;
	AIndex						mRequestData_ArgIndex;
	AText						mRequestData_CallerFuncIdText;
	AIndex						mRequestData_CurrentStateIndex;
	mutable AThreadMutex		mRequestDataMutex;
	
	//実行用データ
  private:
	ADocumentID					mTextDocumentID;
	AWindowID					mIdInfoWindowID;
	AWindowID					mTextWindowID;
	AViewID						mTextViewID;
	AText						mWord;
	AHashTextArray				mParentWord;
	AIndex						mArgIndex;
	AText						mCallerFuncIdText;
	AIndex						mCurrentStateIndex;
	
	//Abortフラグ
  public:
	ABool						mAbortCurrentRequest;
	
	//結果データ取得
  public:
	void						GetWordForResult( AText& outWordForResult, ATextArray& outParentWordForResult ) const;
	void						GetResult( ATextArray& outKeywordTextArray, ATextArray& outParentKeywordTextArray,
								ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, ATextArray& outCommentTextArray,
								AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray, AArray<AScopeType>& outScopeArray,
								AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
								ATextArray& outFilePathArray, AIndex& outArgIndex, AText& outCallerFuncIdText ) const;
  private:
	//結果データ
	//mResultMutexにより排他制御。
	//スレッド内で検索実行後、ローカル変数からコピーされる。
	AText						mResult_WordForResult;
	ATextArray					mResult_ParentWordForResult;
	ATextArray					mResult_KeywordTextArray;
	ATextArray					mResult_ParentKeywordTextArray;
	ATextArray					mResult_TypeTextArray;
	ATextArray					mResult_InfoTextArray;
	ATextArray					mResult_CommentTextArray;
	AArray<AIndex>				mResult_CategoryIndexArray;
	AArray<AIndex>				mResult_ColorSlotIndexArray;
	AArray<AScopeType>			mResult_ScopeArray;
	AArray<AIndex>				mResult_StartIndexArray;
	AArray<AIndex>				mResult_EndIndexArray;
	ATextArray					mResult_FilePathArray;
	mutable AThreadMutex		mResultMutex;
};


#pragma mark ===========================
#pragma mark [クラス]AThread_IdInfoFinderFactory
//生成Factory
class AThread_IdInfoFinderFactory : public AThreadFactory
{
  public:
	AThread_IdInfoFinderFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AThread_IdInfoFinder*	Create() 
	{
		return new AThread_IdInfoFinder(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};
