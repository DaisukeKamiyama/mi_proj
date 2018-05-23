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

#include "stdafx.h"

#include "AThread_CandidateFinder.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AThread_CandidateFinder
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AThread_CandidateFinder::AThread_CandidateFinder( AObjectArrayItem* inParent ) : AThread(inParent)
,mRequestData_TextDocumentID(kObjectID_Invalid),mRequestData_TextViewID(kObjectID_Invalid)
,mTextDocumentID(kObjectID_Invalid),mTextViewID(kObjectID_Invalid)
,mRequestData_CurrentStateIndex(kIndex_Invalid),mCurrentStateIndex(kIndex_Invalid)
,mAbortCurrentRequest(false)
{
}

/**
デストラクタ
*/
AThread_CandidateFinder::~AThread_CandidateFinder()
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/**
スレッドメインルーチン
スレッドはアプリ動作中、常に動作している。
要求データを設定して、unpauseすると、検索を実行する。
*/
void	AThread_CandidateFinder::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_CandidateFinder::NVIDO_ThreadMain started.",this);
	try
	{
		// =================== 結果格納用ローカル変数 =================== 
		//結果取得用ローカル変数
		//（mResult_KeywordArray等には直接格納しない。
		//メインスレッドからの（前回の）結果取得との排他制御が必要であるため。
		//排他制御時間を短くするために、SPI_GetImportIdentifierAbbrevCandidate()実行中は排他せず、
		//最後にmResult_KeywordArray等へコピーするときだけ、排他制御するようにしている。）
		AHashTextArray	keywordArray;
		ATextArray	parentKeywordArray;
		ATextArray	typeTextArray;
		ATextArray	infoTextArray;
		ATextArray	commentTextArray;
		ATextArray	completionTextArray, urlArray;
		AArray<AIndex>	categoryIndexArray;
		AArray<AIndex>	startIndexArray;
		AArray<AIndex>	endIndexArray;
		AArray<AScopeType>	scopeArray;
		ATextArray	filePathArray;
		AArray<AItemCount>	matchedCountArray;
		AArray<ACandidatePriority>	priorityArray;
		AArray<AIndex>	colorSlotIndexArray;
		//ソート済みの結果格納用ローカル変数
		ATextArray	orderedKeywordArray;
		ATextArray	orderedInfoTextArray;
		ATextArray	orderedComletionTextArray;
		AArray<AIndex>	orderedCategoryIndexArray;
		AArray<AScopeType>	orderedScopeArray;
		ATextArray	orderedFilePathArray;
		AArray<AItemCount>	orderedMatchedCountArray;
		AArray<ACandidatePriority>	orderedPriorityArray;
		ATextArray	orderedParentKeywordArray;
		
		//アプリ動作中はずっとループ
		while( NVM_IsAborted() == false )//NVI_AbortThread()がコールされていたらスレッド終了
		{
			//pause解除まで待つ
			NVM_WaitForUnpaused();
			//Abortされていたら終了（アプリ終了時、メインスレッドからNVI_AbortThread()+Unpause()により終了される。）
			if( NVM_IsAborted() == true )
			{
				break;
			}
			
			// =================== 要求データ読み取り =================== 
			{
				//要求データmutex lock
				AStMutexLocker	locker(mRequestDataMutex);
				//要求データが空なら一旦終了(pause)
				//（要求データが空＝前回ループでの検索実行後、新たな要求データが入ってきていない。）
				if( mRequestData_TextDocumentID == kObjectID_Invalid )
				{
					// =================== （要求データが無い場合）検索完了処理 =================== 
					//結果を格納
					{
						//結果mutex lock
						AStMutexLocker	locker(mResultMutex);
						mResult_WordForResult.SetText(mWord);
						mResult_ParentWordForResult.SetFromTextArray(mParentWord);
						mResult_KeywordArray.SetFromTextArray(orderedKeywordArray);
						mResult_InfoTextArray.SetFromTextArray(orderedInfoTextArray);
						mResult_CompletionTextArray.SetFromTextArray(orderedComletionTextArray);
						mResult_CategoryIndexArray.SetFromObject(orderedCategoryIndexArray);
						mResult_ScopeArray.SetFromObject(orderedScopeArray);
						mResult_FilePathArray.SetFromTextArray(orderedFilePathArray);
						mResult_PriorityArray.SetFromObject(orderedPriorityArray);
						mResult_ParentKeywordArray.SetFromTextArray(orderedParentKeywordArray);
						mResult_MatchedCountArray.SetFromObject(orderedMatchedCountArray);
					}
					//スレッドをpause状態にする
					NVI_Pause();
					//メインスレッドへ完了通知
					AObjectIDArray	objectIDArray;
					objectIDArray.Add(mTextViewID);
					ABase::PostToMainInternalEventQueue(kInternalEvent_CandidateFinderPaused,GetObjectID(),0,GetEmptyText(),objectIDArray);
					//unpause待ち状態へ。
					continue;
				}
				//要求データを読み取って、空にする
				//ドキュメントID
				mTextDocumentID = mRequestData_TextDocumentID;
				mRequestData_TextDocumentID = kObjectID_Invalid;
				//TextViewID
				mTextViewID = mRequestData_TextViewID;
				mRequestData_TextViewID = kObjectID_Invalid;
				//検索word
				mWord.SetText(mRequestData_Word);
				mRequestData_Word.DeleteAll();
				//検索parent word
				mParentWord.SetFromTextArray(mRequestData_ParentWord);
				mRequestData_ParentWord.DeleteAll();
				//現在state
				mCurrentStateIndex = mRequestData_CurrentStateIndex;
				mRequestData_CurrentStateIndex = kIndex_Invalid;
				//AbortフラグをOFFにする（実行中に要求データ設定されたらフラグONになる）
				mAbortCurrentRequest = false;
			}
			
			// =================== 結果格納先ローカル変数初期化 =================== 
			keywordArray.DeleteAll();
			parentKeywordArray.DeleteAll();
			typeTextArray.DeleteAll();
			infoTextArray.DeleteAll();
			commentTextArray.DeleteAll();
			completionTextArray.DeleteAll();
			urlArray.DeleteAll();
			categoryIndexArray.DeleteAll();
			startIndexArray.DeleteAll();
			endIndexArray.DeleteAll();
			scopeArray.DeleteAll();
			filePathArray.DeleteAll();
			matchedCountArray.DeleteAll();
			priorityArray.DeleteAll();
			colorSlotIndexArray.DeleteAll();
			
			// =================== 候補検索実行 =================== 
			
			//要求データのword, parentword両方が空のとき（＝全データが対象になる）は処理しない
			if( mWord.GetItemCount() == 0 && mParentWord.GetItemCount() == 0 )
			{
				continue;
			}
			
			//Text Documentオブジェクトを取得。また、メインスレッドでDelete1Object()されてもオブジェクト解放されないようにretainする。
			ADocument*	doc = GetApp().NVI_GetDocumentAndRetain(mTextDocumentID);
			if( doc != NULL )
			{
				//AStDecrementRetainCounterはこのブロックから抜けるときに必ずDecrementRetainCount()をコールすることを保証するためのstackオブジェクト
				//（デストラクタでDecrementRetainCount()が呼ばれる。NVI_GetDocumentAndRetain()でコールされたIncrementRetainCount()に対応
				AStDecrementRetainCounter	releaser(doc);
				
				ADocument_Text&	textdoc = reinterpret_cast<ADocument_Text&>(*doc);
				AModeIndex	modeIndex = textdoc.SPI_GetModeIndex();
				
				//ドキュメントのファイルパスを取得
				AText	docFilePath;
				textdoc.NVI_GetFilePath(docFilePath);
				//拡張子を削除する
				AText	docFilePathWithoutSuffix;
				docFilePathWithoutSuffix.SetText(docFilePath);
				docFilePathWithoutSuffix.DeleteAfter(docFilePathWithoutSuffix.GetSuffixStartPos());
				
				//検索オプション
				AKeywordSearchOption	option = kKeywordSearchOption_Leading | kKeywordSearchOption_IgnoreCases |
										kKeywordSearchOption_InhibitDuplicatedKeyword |
										kKeywordSearchOption_OnlyEnabledCategoryForCurrentState;
				if( GetApp().SPI_IsCandidatePartialSearchMode() == true )
				{
					option |= kKeywordSearchOption_Partial;
				}
				//検索対象wordが空で、parent wordが存在する場合は、parent wordに一致するもののみを対象にする
				//（検索対象wordが空ではない場合は、parent wordに一致するものを"優先"にするのみ）
				if( mWord.GetItemCount() == 0 && mParentWord.GetItemCount() > 0 )
				{
					option |= kKeywordSearchOption_CompareParentKeyword;
				}
				
				// =================== ローカル識別子から候補検索 =================== 
				textdoc.SPI_SearchKeywordLocal(mWord,mParentWord,option,mCurrentStateIndex,
											   keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
											   completionTextArray,urlArray,
											   categoryIndexArray,colorSlotIndexArray,
											   startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
											   mTextViewID,
											   mAbortCurrentRequest);
				
				// =================== グローバル識別子から候補検索 =================== 
				textdoc.SPI_SearchKeywordGlobal(mWord,mParentWord,option,mCurrentStateIndex,
												keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
												completionTextArray,urlArray,
												categoryIndexArray,colorSlotIndexArray,
												startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
												mAbortCurrentRequest);
				
				// =================== Import Identifierから候補検索 =================== 
				textdoc.SPI_SearchKeywordImport(mWord,mParentWord,option,mCurrentStateIndex,
												keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
												completionTextArray,urlArray,
												categoryIndexArray,colorSlotIndexArray,
												startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
												mAbortCurrentRequest);
				
				// =================== System Header から候補検索 =================== 
				GetApp().SPI_GetModePrefDB(modeIndex).
						SPI_SearchKeywordSystemHeader(mWord,mParentWord,option,mCurrentStateIndex,
													  keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
													  completionTextArray,urlArray,
													  categoryIndexArray,colorSlotIndexArray,
													  startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
													  mAbortCurrentRequest);
				
				// =================== モードのキーワード から候補検索 =================== 
				GetApp().SPI_GetModePrefDB(modeIndex).
						SPI_SearchKeywordKeywordList(mWord,mParentWord,option,mCurrentStateIndex,
													 keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
													 completionTextArray,urlArray,
													 categoryIndexArray,colorSlotIndexArray,
													 startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
													 mAbortCurrentRequest);
				
				//==================優先順位設定==================
				//初期設定
				AItemCount	itemCount = keywordArray.GetItemCount();
				for( AIndex index = 0; index < itemCount; index++ )
				{
					priorityArray.Add(kCandidatePriority_None);
				}
				//優先度が低い方から設定する
				//------------------１．履歴による優先------------------
				AHashTextArray	recentCompletionWordArray;
				GetApp().SPI_GetRecentCompletionWordArray(recentCompletionWordArray);
				//全項目ループ
				for( AIndex index = 0; index < itemCount; index++ )
				{
					//キーワード取得
					AText	keyword;
					keywordArray.Get(index,keyword);
					//履歴と一致したら、履歴一致優先を設定
					if( recentCompletionWordArray.Find(keyword) != kIndex_Invalid )
					{
						//履歴一致優先を設定
						priorityArray.Set(index,kCandidatePriority_History);
					}
				}
				//------------------２．parent keyword一致による優先------------------
				//全項目ループ
				for( AIndex index = 0; index < itemCount; index++ )
				{
					//キーワード取得
					AText	keyword;
					keywordArray.Get(index,keyword);
					//parent keyword一致による優先
					if( mParentWord.GetItemCount() > 0 )
					{
						//parent keyword取得
						AText	parentKeyword;
						parentKeywordArray.Get(index,parentKeyword);
						//fprintf(stderr,"PARENT:");
						//parentKeyword.OutputToStderr();
						//要求データに同じparent keywordが存在すれば優先する
						if( mParentWord.Find(parentKeyword) != kIndex_Invalid )
						{
							//履歴と一致するかどうかを判定
							if( recentCompletionWordArray.Find(keyword) != kIndex_Invalid )
							{
								//parent keyword一致かつ履歴一致優先を設定
								priorityArray.Set(index,kCandidatePriority_ParentKeywordWithHistory);
							}
							else
							{
								//parent keyword一致優先を設定
								priorityArray.Set(index,kCandidatePriority_ParentKeyword);
							}
						}
					}
				}
				//------------------３．localを優先------------------
				//全項目ループ
				for( AIndex index = 0; index < itemCount; index++ )
				{
					//スコープがローカルならローカル優先を設定
					if( scopeArray.Get(index) == kScopeType_Local )
					{
						priorityArray.Set(index,kCandidatePriority_Local);
					}
				}
				
				//------------------優先順位順にordered～変数に格納------------------
				//優先順位をつけてからordered配列に格納していく（１つの項目に複数の優先順位があり得るので、上の処理で一番優先する優先順位を最後に設定している）
				orderedKeywordArray.DeleteAll();
				orderedInfoTextArray.DeleteAll();
				orderedComletionTextArray.DeleteAll();
				orderedCategoryIndexArray.DeleteAll();
				orderedScopeArray.DeleteAll();
				orderedFilePathArray.DeleteAll();
				orderedMatchedCountArray.DeleteAll();
				orderedPriorityArray.DeleteAll();
				orderedParentKeywordArray.DeleteAll();
				//優先度高い方からループ
				for( AIndex priority = kCandidatePriority_Highest; priority >= kCandidatePriority_None; priority-- )
				{
					//全項目ループ
					for( AIndex index = 0; index < itemCount; index++ )
					{
						//表示数制限（200項目）
						//検索中も検索一致項目制限しているが、そちらは1024項目。優先順位が高い項目が後に検索されることもありうるので、
						//検索項目制限は多めにしている。
						if( orderedKeywordArray.GetItemCount() >= kLimit_CandidateListItemCount )
						{
							//制限数以上は追加しない
							break;
						}
						//優先順位一致するものを追加
						if( priorityArray.Get(index) == priority )
						{
							orderedKeywordArray.Add(keywordArray.GetTextConst(index));
							orderedInfoTextArray.Add(infoTextArray.GetTextConst(index));
							orderedComletionTextArray.Add(completionTextArray.GetTextConst(index));
							orderedCategoryIndexArray.Add(categoryIndexArray.Get(index));
							orderedScopeArray.Add(scopeArray.Get(index));
							orderedFilePathArray.Add(filePathArray.GetTextConst(index));
							orderedMatchedCountArray.Add(matchedCountArray.Get(index));
							orderedPriorityArray.Add(static_cast<ACandidatePriority>(priority));
							orderedParentKeywordArray.Add(parentKeywordArray.GetTextConst(index));
						}
					}
				}
			}
		}
	}
	catch(...)
	{
		_ACError("AThread_CandidateFinder::NVIDO_ThreadMain() caught exception.",NULL);
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_CandidateFinder::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---要求データ設定

/**
要求データ設定
*/
void	AThread_CandidateFinder::SetRequestData( const ADocumentID inTextDocumentID, const AViewID inTextViewID,
												const AText& inWord, const ATextArray& inParentWord,
												const AIndex inCurrentStateIndex )
{
	//要求データmutex lock
	AStMutexLocker	locker(mRequestDataMutex);
	//ドキュメントID、TextViewID設定
	mRequestData_TextDocumentID = inTextDocumentID;
	mRequestData_TextViewID = inTextViewID;
	//検索word, parent word設定
	mRequestData_Word.SetText(inWord);
	mRequestData_ParentWord.SetFromTextArray(inParentWord);
	//要求state
	mRequestData_CurrentStateIndex = inCurrentStateIndex;
	//AbortフラグをONにする。（search処理中断される）
	mAbortCurrentRequest = true;
}

#pragma mark ===========================

#pragma mark ---結果データ取得

/**
今の結果データに対する要求データ（word, parent word）の取得
*/
void	AThread_CandidateFinder::GetWordForResult( AText& outWordForResult, ATextArray& outParentWordForResult ) const
{
	//結果データ mutex lock
	AStMutexLocker	locker(mResultMutex);
	//データ取得
	outWordForResult.SetText(mResult_WordForResult);
	outParentWordForResult.SetFromTextArray(mResult_ParentWordForResult);
}

/**
結果データ取得
*/
void	AThread_CandidateFinder::GetResult( ATextArray& outKeywordArray, ATextArray& outInfoTextArray,
										   AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
										   ATextArray& outFilePathArray, AArray<ACandidatePriority>& outPriorityArray,
										   ATextArray& outParentKeywordArray, ATextArray& outCompletionTextArray,
										   AArray<AItemCount>& outMatchedItemCountArray ) const
{
	//結果データ mutex lock
	AStMutexLocker	locker(mResultMutex);
	//
	//データ取得
	outKeywordArray.AddFromTextArray(mResult_KeywordArray);
	outInfoTextArray.AddFromTextArray(mResult_InfoTextArray);
	outCompletionTextArray.AddFromTextArray(mResult_CompletionTextArray);
	outCategoryIndexArray.AddFromObject(mResult_CategoryIndexArray);
	outScopeArray.AddFromObject(mResult_ScopeArray);
	outFilePathArray.AddFromTextArray(mResult_FilePathArray);
	outPriorityArray.AddFromObject(mResult_PriorityArray);
	outMatchedItemCountArray.AddFromObject(mResult_MatchedCountArray);
	outParentKeywordArray.AddFromTextArray(mResult_ParentKeywordArray);
}
