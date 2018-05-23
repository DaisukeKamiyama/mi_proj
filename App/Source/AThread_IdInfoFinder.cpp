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

#include "stdafx.h"

#include "AThread_IdInfoFinder.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AThread_IdInfoFinder
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AThread_IdInfoFinder::AThread_IdInfoFinder( AObjectArrayItem* inParent ) : AThread(inParent)
,mRequestData_TextDocumentID(kObjectID_Invalid),mRequestData_IdInfoWindowID(kObjectID_Invalid)
,mTextDocumentID(kObjectID_Invalid),mIdInfoWindowID(kObjectID_Invalid)
,mRequestData_TextWindowID(kObjectID_Invalid),mTextWindowID(kObjectID_Invalid)
,mRequestData_TextViewID(kObjectID_Invalid),mTextViewID(kObjectID_Invalid)
,mRequestData_ArgIndex(kIndex_Invalid),mArgIndex(kIndex_Invalid)
,mRequestData_CurrentStateIndex(kIndex_Invalid),mCurrentStateIndex(kIndex_Invalid)
,mAbortCurrentRequest(false)
{
}

/**
デストラクタ
*/
AThread_IdInfoFinder::~AThread_IdInfoFinder()
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/**
スレッドメインルーチン
スレッドはアプリ動作中、常に動作している。
要求データを設定して、unpauseすると、検索を実行する。
*/
void	AThread_IdInfoFinder::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_IdInfoFinder::NVIDO_ThreadMain started.",this);
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
		AArray<AIndex>	colorSlotIndexArray;
		AArray<AIndex>	startIndexArray;
		AArray<AIndex>	endIndexArray;
		AArray<AScopeType>	scopeArray;
		ATextArray	filePathArray;
		AArray<AItemCount>	matchedCountArray;
		
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
						mResult_KeywordTextArray.SetFromTextArray(keywordArray);
						mResult_ParentKeywordTextArray.SetFromTextArray(parentKeywordArray);
						mResult_TypeTextArray.SetFromTextArray(typeTextArray);
						mResult_InfoTextArray.SetFromTextArray(infoTextArray);
						mResult_CommentTextArray.SetFromTextArray(commentTextArray);
						mResult_CategoryIndexArray.SetFromObject(categoryIndexArray);
						mResult_ColorSlotIndexArray.SetFromObject(colorSlotIndexArray);
						mResult_ScopeArray.SetFromObject(scopeArray);
						mResult_StartIndexArray.SetFromObject(startIndexArray);
						mResult_EndIndexArray.SetFromObject(endIndexArray);
						mResult_FilePathArray.SetFromTextArray(filePathArray);
					}
					//スレッドをpause状態にする
					NVI_Pause();
					//途中中断されていなければ、メインスレッドへ完了通知
					if( mAbortCurrentRequest == false )
					{
						AObjectIDArray	objectIDArray;
						objectIDArray.Add(mTextDocumentID);
						objectIDArray.Add(mIdInfoWindowID);
						objectIDArray.Add(mTextWindowID);
						ABase::PostToMainInternalEventQueue(kInternalEvent_IdInfoFinderPaused,GetObjectID(),0,GetEmptyText(),objectIDArray);
					}
					//unpause待ち状態へ。
					continue;
				}
				//要求データを読み取って、空にする
				//ドキュメントID
				mTextDocumentID = mRequestData_TextDocumentID;
				mRequestData_TextDocumentID = kObjectID_Invalid;
				//IdInfoWindow
				mIdInfoWindowID = mRequestData_IdInfoWindowID;
				mRequestData_IdInfoWindowID = kObjectID_Invalid;
				//TextWindowID
				mTextWindowID = mRequestData_TextWindowID;
				mRequestData_TextWindowID = kObjectID_Invalid;
				//要求元text view
				mTextViewID = mRequestData_TextViewID;
				mRequestData_TextViewID = kObjectID_Invalid;
				//検索word
				mWord.SetText(mRequestData_Word);
				mRequestData_Word.DeleteAll();
				//検索parent word
				mParentWord.SetFromTextArray(mRequestData_ParentWord);
				mRequestData_ParentWord.DeleteAll();
				//引数index
				mArgIndex = mRequestData_ArgIndex;
				mRequestData_ArgIndex = kIndex_Invalid;
				//現在state
				mCurrentStateIndex = mRequestData_CurrentStateIndex;
				mRequestData_CurrentStateIndex = kIndex_Invalid;
				//コール元FuncId
				mCallerFuncIdText.SetText(mRequestData_CallerFuncIdText);
				mRequestData_CallerFuncIdText.DeleteAll();
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
			colorSlotIndexArray.DeleteAll();
			startIndexArray.DeleteAll();
			endIndexArray.DeleteAll();
			scopeArray.DeleteAll();
			filePathArray.DeleteAll();
			matchedCountArray.DeleteAll();
			
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
				//ブロックを出たときにDecrementRetainCount()をコールすることを保証するためのstackクラス
				//上のNVI_GetDocumentAndRetain()に対応
				AStDecrementRetainCounter	releaser(doc);
				
				ADocument_Text&	textdoc = reinterpret_cast<ADocument_Text&>(*doc);
				AModeIndex	modeIndex = textdoc.SPI_GetModeIndex();
				
				//ドキュメントのファイルパスを取得
				AText	docFilePath;
				textdoc.NVI_GetFilePath(docFilePath);
				//拡張子を削除
				AText	docFilePathWithoutSuffix;
				docFilePathWithoutSuffix.SetText(docFilePath);
				docFilePathWithoutSuffix.DeleteAfter(docFilePathWithoutSuffix.GetSuffixStartPos());
				
				//検索オプション設定
				//IdInfoWindow未設定（＝text viewのidクリック等）の場合は、検索オプションなし（キーワード完全一致のみ）
				AKeywordSearchOption	option = kKeywordSearchOption_OnlyEnabledCategoryForCurrentState;
				if( mIdInfoWindowID != kObjectID_Invalid )
				{
					//IdInfoWindow設定（IdInfoWindowの検索ボックス）の場合は、検索オプション部分一致・大文字小文字無視・InfoText内も検索
					option = kKeywordSearchOption_Partial | 
							kKeywordSearchOption_IgnoreCases | 
							kKeywordSearchOption_IncludeInfoText |
							kKeywordSearchOption_OnlyEnabledCategoryForCurrentState;
				}
				
				// =================== ローカル識別子から候補検索 =================== 
				if( mTextViewID != kObjectID_Invalid )
				{
					textdoc.SPI_SearchKeywordLocal(mWord,mParentWord,option,mCurrentStateIndex,
												   keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
												   completionTextArray,urlArray,
												   categoryIndexArray,colorSlotIndexArray,
												   startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
												   mTextViewID,
												   mAbortCurrentRequest);
				}
				
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
				AIndex	modeKeywordStartIndex = keywordArray.GetItemCount();
				GetApp().SPI_GetModePrefDB(modeIndex).
						SPI_SearchKeywordKeywordList(mWord,mParentWord,option,mCurrentStateIndex,
													 keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
													 completionTextArray,urlArray,
													 categoryIndexArray,colorSlotIndexArray,
													 startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
													 mAbortCurrentRequest);
				
				//キーワード項目のうちinfotextが空のものは削除する
				{
					for( AIndex index = modeKeywordStartIndex; index < keywordArray.GetItemCount();  )
					{
						if( scopeArray.Get(index) == kScopeType_ModeKeyword &&
									infoTextArray.GetTextLen(index) == 0 )
						{
							keywordArray.Delete1(index);
							parentKeywordArray.Delete1(index);
							typeTextArray.Delete1(index);
							infoTextArray.Delete1(index);
							commentTextArray.Delete1(index);
							completionTextArray.Delete1(index);
							urlArray.Delete1(index);
							categoryIndexArray.Delete1(index);
							colorSlotIndexArray.Delete1(index);
							startIndexArray.Delete1(index);
							endIndexArray.Delete1(index);
							scopeArray.Delete1(index);
							filePathArray.Delete1(index);
							matchedCountArray.Delete1(index);
						}
						else
						{
							index++;
						}
					}
				}
				
				//==================優先順位設定==================
				//順序変更。優先項目を先に表示する
				//優先項目のindex
				AIndex	priorIndex = 0;
				//全項目ループ
				AItemCount	itemCount = keywordArray.GetItemCount();
				for( AIndex index = 0; index < itemCount; index++ )
				{
					//優先するかどうかの判定結果格納フラグ
					ABool	isPrior = false;
					//parent keyword一致による優先
					if( mParentWord.GetItemCount() > 0 )
					{
						//parent keyword取得
						AText	parentKeyword;
						parentKeywordArray.Get(index,parentKeyword);
						//要求データで指定されたparent keywordなら優先する
						if( mParentWord.Find(parentKeyword) != kIndex_Invalid )
						{
							isPrior = true;
						}
					}
					//parent keyword未指定なら、拡張子を除いたファイル名が同じ場合に優先する
					else
					{
						//ファイルパスを取得し、拡張子を除く
						AText	filepath;
						filePathArray.Get(index,filepath);
						filepath.DeleteAfter(filepath.GetSuffixStartPos());
						//拡張子を除いたファイル名が同じなら優先する
						if( filepath.Compare(docFilePathWithoutSuffix) == true )
						{
							isPrior = true;
						}
					}
					//優先項目なら、最初のほうに移動する
					if( isPrior == true )
					{
						keywordArray.MoveObject(index,priorIndex);
						parentKeywordArray.MoveObject(index,priorIndex);
						typeTextArray.MoveObject(index,priorIndex);
						infoTextArray.MoveObject(index,priorIndex);
						commentTextArray.MoveObject(index,priorIndex);
						completionTextArray.MoveObject(index,priorIndex);
						urlArray.MoveObject(index,priorIndex);
						categoryIndexArray.Move(index,priorIndex);
						colorSlotIndexArray.Move(index,priorIndex);
						startIndexArray.Move(index,priorIndex);
						endIndexArray.Move(index,priorIndex);
						scopeArray.Move(index,priorIndex);
						filePathArray.MoveObject(index,priorIndex);
						matchedCountArray.Move(index,priorIndex);
						priorIndex++;
					}
				}
			}
		}
	}
	catch(...)
	{
		_ACError("AThread_IdInfoFinder::NVIDO_ThreadMain() caught exception.",NULL);
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_IdInfoFinder::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---要求データ設定

/**
要求データ設定
@note inParentWordに一致する項目が優先表示される。
*/
void	AThread_IdInfoFinder::SetRequestData( const ADocumentID inTextDocumentID, 
											 const AWindowID inIdInfoWindowID, 
											 const AWindowID inTextWindowID, const AViewID inTextViewID, 
											 const AText& inWord, const ATextArray& inParentWord,
											 const AIndex inArgIndex, const AText& inCallerFuncIdText,
											 const AIndex inCurrentStateIndex )
{
	//要求データmutex lock
	AStMutexLocker	locker(mRequestDataMutex);
	//ドキュメントID、IdInfowWindowID, TextWindowID設定
	mRequestData_TextDocumentID = inTextDocumentID;
	mRequestData_IdInfoWindowID = inIdInfoWindowID;
	mRequestData_TextWindowID = inTextWindowID;
	mRequestData_TextViewID = inTextViewID;
	//検索word, parent word設定
	mRequestData_Word.SetText(inWord);
	mRequestData_ParentWord.SetFromTextArray(inParentWord);
	//arg index
	mRequestData_ArgIndex = inArgIndex;
	//コール元FuncId
	mRequestData_CallerFuncIdText.SetText(inCallerFuncIdText);
	//現在state
	mRequestData_CurrentStateIndex = inCurrentStateIndex;
	//AbortフラグON
	mAbortCurrentRequest = true;
}

/**
現在の検索要求を中断
*/
void	AThread_IdInfoFinder::AbortCurrentRequest()
{
	mRequestData_TextDocumentID = kObjectID_Invalid;
	mAbortCurrentRequest = true;
}

#pragma mark ===========================

#pragma mark ---結果データ取得

/**
今の結果データに対する要求データ（word, parent word）の取得
*/
void	AThread_IdInfoFinder::GetWordForResult( AText& outWordForResult, ATextArray& outParentWordForResult ) const
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
void	AThread_IdInfoFinder::GetResult( ATextArray& outKeywordTextArray, ATextArray& outParentKeywordTextArray,
		ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, ATextArray& outCommentTextArray,
		AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray, AArray<AScopeType>& outScopeArray,
		AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
		ATextArray& outFilePathArray, AIndex& outArgIndex, AText& outCallerFuncIdText ) const
{
	//結果データ mutex lock
	AStMutexLocker	locker(mResultMutex);
	//
	//データ取得
	outKeywordTextArray.AddFromTextArray(mResult_KeywordTextArray);
	outParentKeywordTextArray.AddFromTextArray(mResult_ParentKeywordTextArray);
	outTypeTextArray.AddFromTextArray(mResult_TypeTextArray);
	outInfoTextArray.AddFromTextArray(mResult_InfoTextArray);
	outCommentTextArray.AddFromTextArray(mResult_CommentTextArray);
	outCategoryIndexArray.AddFromObject(mResult_CategoryIndexArray);
	outColorSlotIndexArray.AddFromObject(mResult_ColorSlotIndexArray);
	outScopeArray.AddFromObject(mResult_ScopeArray);
	outStartIndexArray.AddFromObject(mResult_StartIndexArray);
	outEndIndexArray.AddFromObject(mResult_EndIndexArray);
	outFilePathArray.AddFromTextArray(mResult_FilePathArray);
	outArgIndex = mArgIndex;
	outCallerFuncIdText.SetText(mCallerFuncIdText);
}
