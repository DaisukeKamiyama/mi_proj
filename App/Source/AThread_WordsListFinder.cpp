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

AThread_WordsListFinder
#723

*/

#include "stdafx.h"

#include "AThread_WordsListFinder.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AThread_WordsListFinder
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AThread_WordsListFinder::AThread_WordsListFinder( AObjectArrayItem* inParent ) : AThread(inParent)
,mRequestData_WindowID(kObjectID_Invalid)
,mRequestData_ModeIndex(kIndex_Invalid)
,mRequestData_RequestLevelIndex(kIndex_Invalid)
,mWindowID(kObjectID_Invalid)
,mModeIndex(kIndex_Invalid)
,mRequestLevelIndex(kIndex_Invalid)
,mResult_RequestLevelIndex(kIndex_Invalid)
,mResult_WindowID(kObjectID_Invalid)
,mResult_AllFilesSearched(false)
,mWordsListFinderType(kWordsListFinderType_ForCallGraf)
,mAbortCurrentRequest(false)
{
}

/**
デストラクタ
*/
AThread_WordsListFinder::~AThread_WordsListFinder()
{
}

/**
ワードリスト検索タイプ設定
*/
void	AThread_WordsListFinder::SetWordsListFinderType( const AWordsListFinderType inType )
{
	mWordsListFinderType = inType;
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/**
スレッドメインルーチン
スレッドはアプリ動作中、常に動作している。
要求データを設定して、unpauseすると、検索を実行する。
*/
void	AThread_WordsListFinder::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_WordsListFinder::NVIDO_ThreadMain started.",this);
	try
	{
		// =================== 結果格納用ローカル変数 =================== 
		//結果取得用ローカル変数
		ATextArray	filePathArray;
		ATextArray	classNameArray;
		ATextArray	functionNameArray;
		AArray<AIndex>	startArray;
		AArray<AIndex>	endArray;
		ATextArray	contentTextArray;
		AArray<AIndex>	contentTextStartIndexArray;
		AArray<AColor>	headerColorArray;
		ABool	allFilesSearched = true;
		
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
				if( mRequestData_WordText.GetItemCount() == 0 )
				{
					// =================== （要求データが無い場合）検索完了処理 =================== 
					//結果を格納
					{
						//結果mutex lock
						AStMutexLocker	locker(mResultMutex);
						mResult_RequestLevelIndex = mRequestLevelIndex;
						mResult_WindowID = mWindowID;
						mResult_WordText.SetText(mWordText);
						mResult_FilePathArray.SetFromTextArray(filePathArray);
						mResult_ClassNameArray.SetFromTextArray(classNameArray);
						mResult_FunctionNameArray.SetFromTextArray(functionNameArray);
						mResult_StartArray.SetFromObject(startArray);
						mResult_EndArray.SetFromObject(endArray);
						mResult_ContentTextArray.SetFromTextArray(contentTextArray);
						mResult_ContentTextStartIndexArray.SetFromObject(contentTextStartIndexArray);
						mResult_HeaderColorArray.SetFromObject(headerColorArray);
						mResult_AllFilesSearched = allFilesSearched;
					}
					//スレッドをpause状態にする
					NVI_Pause();
					//途中中断されていなければ、メインスレッドへ完了通知
					if( mAbortCurrentRequest == false )
					{
						AObjectIDArray	objectIDArray;
						objectIDArray.Add(mResult_WindowID);
						switch(mWordsListFinderType)
						{
						  case kWordsListFinderType_ForCallGraf:
							{
								ABase::PostToMainInternalEventQueue(kInternalEvent_WordsListFinderPausedForCallGraf,GetObjectID(),0,GetEmptyText(),objectIDArray);
								break;
							}
						  case kWordsListFinderType_ForWordsList:
							{
								ABase::PostToMainInternalEventQueue(kInternalEvent_WordsListFinderPausedForWordsList,GetObjectID(),0,GetEmptyText(),objectIDArray);
								break;
							}
						}
					}
					//unpause待ち状態へ。
					continue;
				}
				//要求データを読み取って、空にする
				//ドキュメントID
				mProjectFolder = mRequestData_ProjectFolder;
				mRequestData_ProjectFolder.Unspecify();
				//mode index
				mModeIndex = mRequestData_ModeIndex;
				mRequestData_ModeIndex = kIndex_Invalid;
				//ウインドウID
				mWindowID = mRequestData_WindowID;
				mRequestData_WindowID = kObjectID_Invalid;
				//検索word
				mWordText.SetText(mRequestData_WordText);
				mRequestData_WordText.DeleteAll();
				//callgrafのlevel
				mRequestLevelIndex = mRequestData_RequestLevelIndex;
				mRequestData_RequestLevelIndex = kIndex_Invalid;
				//AbortフラグをOFFにする（実行中に要求データ設定されたらフラグONになる）
				mAbortCurrentRequest = false;
			}
			
			// =================== 結果格納先ローカル変数初期化 =================== 
			filePathArray.DeleteAll();
			classNameArray.DeleteAll();
			functionNameArray.DeleteAll();
			startArray.DeleteAll();
			endArray.DeleteAll();
			contentTextArray.DeleteAll();
			contentTextStartIndexArray.DeleteAll();
			headerColorArray.DeleteAll();
			allFilesSearched = true;
			
			// =================== ワードリスト検索実行 =================== 
			
			//要求project folder, mode indexに属するファイルのimportFileDataオブジェクトを取得
			//（importFileDataは未認識の可能性がある。）
			AArray<AObjectID>	importFileDataIDArray;
			GetApp().SPI_GetImportFileDataIDArrayForWordsList(mProjectFolder,mModeIndex,importFileDataIDArray,mAborted);
			//ImportFileDataオブジェクト毎のループ
			for( AIndex i = 0; i < importFileDataIDArray.GetItemCount(); i++ )
			{
				//中断要求があればbreak
				if( mAbortCurrentRequest == true || NVM_IsAborted() == true )
				{
					allFilesSearched = false;
					break;
				}
				//検索結果が制限数以上になったらbreak
				if( filePathArray.GetItemCount() >= kLimit_WordsListResultCount )
				{
					allFilesSearched = false;
					break;
				}
				//------------------Import File Dataオブジェクト取得------------------
				//ImportFileData削除時、その中のTextInfoのデータが削除されるのはImportFileDataのデストラクタ内なので、
				//AObjectArrayIDIndexedのgetandretain機構を使えば、DecrementRetainCount()するまでは、TextInfoのデータが保証される。
				AImportFileData*	importFileDataPtr = 
									GetApp().SPI_GetImportFileManager().GetImportFileDataAndRetain(importFileDataIDArray.Get(i));
				if( importFileDataPtr != NULL )//既にImportFileDataが削除済みの場合は、NULLが返る
				{
					//このブロックを抜けるときにDecrementRetainCount()を必ずコールするためのstackオブジェクト
					//（上のGetImportFileDataAndRetain()でIncrementRetainCount()しているのに対応している）
					AStDecrementRetainCounter	releaser(importFileDataPtr);
					
					const AImportFileData&	importFileData = *(importFileDataPtr);
					//排他しないと、検索途中でImportFileDataがImportFileRecognizerにて再認識処理に入る可能性がある。
					AStImportFileDataLocker	locker(importFileData);
					if( importFileData.IsRecognized() == false )
					{
						//ImportFileData未認識の場合
						
						//検索実行しない。
						
						//フラグを設定。
						allFilesSearched = false;
					}
					else
					{
						//ImportFileData認識済みの場合
						
						//------------------ワードリスト取得------------------
						//ImportFileDataのファイルパス取得
						AText	filepath;
						importFileData.GetFile().GetPath(filepath);
						//words listから検索（スレッドセーフ）
						AArray<AIndex>	sposArray, eposArray;
						importFileData.GetTextInfoConst().FindWordsList(mWordText,sposArray,eposArray);
						if( sposArray.GetItemCount() > 0 )
						{
							//------------------ファイルのテキスト読み込み------------------
							//ワードリスト用の場合のみテキストを読み込む（周辺テキスト表示のため）
							AText	importFileText;
							if( mWordsListFinderType == kWordsListFinderType_ForWordsList )
							{
								AModeIndex	modeIndex = 0;
								AText	tecname;
								GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_WordsListFinder,
															  importFileData.GetFile(),importFileText,modeIndex,tecname);
							}
							//------------------このファイルのワードリストの各項目毎のループ------------------
							for( AIndex j = 0; j < sposArray.GetItemCount(); j++ )
							{
								//中断要求があればbreak
								if( mAbortCurrentRequest == true || NVM_IsAborted() == true )
								{
									allFilesSearched = false;
									break;
								}
								//検索結果が制限数以上になったらbreak
								if( filePathArray.GetItemCount() >= kLimit_WordsListResultCount )
								{
									allFilesSearched = false;
									break;
								}
								//ファイルパス
								filePathArray.Add(filepath);
								//------------------開始・終了text index取得------------------
								AIndex	spos = sposArray.Get(j);
								AIndex	epos = eposArray.Get(j);
								startArray.Add(spos);
								endArray.Add(epos);
								//------------------見出しデータ取得------------------
								//その位置に対応するlocal start identifierを取得し、キーワード（関数名）、クラス名、色を取得
								ATextPoint	textpt = {0,0};
								importFileData.GetTextInfoConst().GetTextPointFromTextIndex(spos,textpt);
								AText	functionName, className;
								AColor	headercolor = kColor_Blue;
								AUniqID	identifierUniqID = importFileData.GetTextInfoConst().GetLocalStartIdentifierByLineIndex(textpt.y);
								if( identifierUniqID != kUniqID_Invalid )
								{
									//local start identifierのキーワードテキストまたはメニューテキストを取得
									if( importFileData.GetTextInfoConst().GetGlobalIdentifierKeywordText(identifierUniqID,functionName) == false )
									{
										importFileData.GetTextInfoConst().GetGlobalIdentifierMenuText(identifierUniqID,functionName);
									}
									//local start identifierの文字色を取得
									AIndex	categoryIndex = importFileData.GetTextInfoConst().GetGlobalIdentifierCategoryIndex(identifierUniqID);
									if( categoryIndex != kIndex_Invalid )
									{
										GetApp().SPI_GetModePrefDB(importFileData.GetModeIndex()).GetCategoryColor(categoryIndex,headercolor);
									}
									//local start identifierのクラス名を取得
									importFileData.GetTextInfoConst().GetGlobalIdentifierParentKeywordText(identifierUniqID,className);
								}
								classNameArray.Add(className);
								functionNameArray.Add(functionName);
								headerColorArray.Add(headercolor);
								//------------------周辺テキスト／周辺テキスト開始テキストインデックスを取得------------------
								AText	contentText;
								AIndex	contentTextStartIndex = kIndex_Invalid;
								if( mWordsListFinderType == kWordsListFinderType_ForWordsList )
								{
									//wordslistの場合のみ、contentTextに周辺テキスト（前後1行ずつ）を格納する
									
									//前後一行ずつとなる開始・終了ポイントを取得
									ATextPoint	spt = textpt;
									ATextPoint	ept = textpt;
									//前行の最初を開始位置にする
									spt.x = 0;
									spt.y -= 1;
									if( spt.y < 0 )
									{
										spt.y = 0;
									}
									//次の次の行の最初を開始位置にする
									ept.x = 0;
									ept.y += 2;
									if( ept.y >= importFileData.GetTextInfoConst().GetLineCount() )
									{
										ept.y = importFileData.GetTextInfoConst().GetLineCount()-1;
									}
									//開始終了text index取得
									AIndex	start = importFileData.GetTextInfoConst().GetTextIndexFromTextPoint(spt);
									AIndex	end = importFileData.GetTextInfoConst().GetTextIndexFromTextPoint(ept);
									//バイト数が1000以上となる場合は、前後400バイトにする
									if( end-start >= 1000 )
									{
										//400バイト前
										start = spos-400;
										if( start < 0 )
										{
											start = 0;
										}
										start = importFileText.GetCurrentCharPos(start);
										//400バイト後
										end = epos+400;
										if( end > importFileText.GetItemCount() )
										{
											end = importFileText.GetItemCount();
										}
										end = importFileText.GetCurrentCharPos(end);
									}
									//text index補正
									if( start < 0 )   start = 0;
									if( start >= importFileText.GetItemCount() )   start = importFileText.GetItemCount();
									if( end < 0 )   end = 0;
									if( end >= importFileText.GetItemCount() )   end = importFileText.GetItemCount();
									//周辺テキスト取得
									importFileText.GetText(start,end-start,contentText);
									//周辺テキスト開始位置設定
									contentTextStartIndex = start;
								}
								//周辺テキスト設定
								contentTextArray.Add(contentText);
								contentTextStartIndexArray.Add(contentTextStartIndex);
							}
						}
					}
				}
			}
		}
	}
	catch(...)
	{
		_ACError("AThread_WordsListFinder::NVIDO_ThreadMain() caught exception.",NULL);
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_WordsListFinder::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---要求データ設定

/**
要求データ設定（コールグラフ用）
*/
void	AThread_WordsListFinder::SetRequestDataForCallGraf( const AFileAcc& inProjectFolder, const AIndex inModeIndex,
		const AWindowID inWindowID, const AText& inWordText, const AIndex inRequestLevelIndex )
{
	//要求データmutex lock
	AStMutexLocker	locker(mRequestDataMutex);
	//Project folder, mode index, WindowID設定
	mRequestData_ProjectFolder = inProjectFolder;
	mRequestData_ModeIndex = inModeIndex;
	mRequestData_WindowID = inWindowID;
	//検索word設定
	mRequestData_WordText.SetText(inWordText);
	//callgrafのlevel
	mRequestData_RequestLevelIndex = inRequestLevelIndex;
	//AbortフラグをONにする。（現在実行中ののsearch処理は中断され、今回の要求読み込み処理へ移行する）
	mAbortCurrentRequest = true;
}

/**
要求データ設定（ワードリスト用）
*/
void	AThread_WordsListFinder::SetRequestDataForWordsList( const AFileAcc& inProjectFolder, const AIndex inModeIndex,
		const AWindowID inWindowID, const AText& inWordText )
{
	//要求データmutex lock
	AStMutexLocker	locker(mRequestDataMutex);
	//Project folder, mode index, WindowID設定
	mRequestData_ProjectFolder = inProjectFolder;
	mRequestData_ModeIndex = inModeIndex;
	mRequestData_WindowID = inWindowID;
	//検索word設定
	mRequestData_WordText.SetText(inWordText);
	//callgrafのlevel
	mRequestData_RequestLevelIndex = kIndex_Invalid;
	//AbortフラグをONにする。（現在実行中ののsearch処理は中断され、今回の要求読み込み処理へ移行する）
	mAbortCurrentRequest = true;
}

/**
現在の検索要求を中断
*/
void	AThread_WordsListFinder::AbortCurrentRequest()
{
	mRequestData_WordText.DeleteAll();
	mAbortCurrentRequest = true;
}

#pragma mark ===========================

#pragma mark ---結果データ取得

/**
結果データ取得（コールグラフ用）
*/
ABool	AThread_WordsListFinder::GetResultForCallGraf( AWindowID& outWindowID, AIndex& outRequestLevelIndex, 
		ATextArray& outFilePathArray, ATextArray& outClassNameArray, ATextArray& outFunctionNameArray,
		AArray<AIndex>& outStartArray, AArray<AIndex>& outEndArray ) const
{
	//結果データ mutex lock
	AStMutexLocker	locker(mResultMutex);
	//WindowID取得
	outWindowID = mResult_WindowID;
	//call graf leve取得
	outRequestLevelIndex = mResult_RequestLevelIndex;
	//検索結果取得
	outFilePathArray.SetFromTextArray(mResult_FilePathArray);
	outClassNameArray.SetFromTextArray(mResult_ClassNameArray);
	outFunctionNameArray.SetFromTextArray(mResult_FunctionNameArray);
	outStartArray.SetFromObject(mResult_StartArray);
	outEndArray.SetFromObject(mResult_EndArray);
	return mResult_AllFilesSearched;
}

/**
結果データ取得（ワードリスト用）
*/
ABool	AThread_WordsListFinder::GetResultForWordsList( 
		ATextArray& outFilePathArray, ATextArray& outClassNameArray, ATextArray& outFunctionNameArray,
		AArray<AIndex>& outStartArray, AArray<AIndex>& outEndArray,
		AArray<AColor>& outHeaderColorArray,
		ATextArray& outContentTextArray, AArray<AIndex>& outContentTextIndexArray ) const
{
	//結果データ mutex lock
	AStMutexLocker	locker(mResultMutex);
	//検索結果取得
	outFilePathArray.SetFromTextArray(mResult_FilePathArray);
	outClassNameArray.SetFromTextArray(mResult_ClassNameArray);
	outFunctionNameArray.SetFromTextArray(mResult_FunctionNameArray);
	outStartArray.SetFromObject(mResult_StartArray);
	outEndArray.SetFromObject(mResult_EndArray);
	outHeaderColorArray.SetFromObject(mResult_HeaderColorArray);
	outContentTextArray.SetFromTextArray(mResult_ContentTextArray);
	outContentTextIndexArray.SetFromObject(mResult_ContentTextStartIndexArray);
	return mResult_AllFilesSearched;
}
