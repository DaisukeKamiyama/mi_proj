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

AImportFileManager

*/

#include "stdafx.h"

#include "AImportFileManager.h"
#include "AImportIdentifierLink.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AImportFileManager
#pragma mark ===========================
//インポートファイルを保持・管理するクラス（アプリ全体の各ファイルのインポート(include)ファイルの文法情報を保持する）
//ImportFileManagerはプロジェクトには依存しない。どのファイルをロードするかは外側からファイルごとに指定する
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AImportFileManager::AImportFileManager() : AObjectArrayItem(NULL)
,mImportFileDataArray(NULL)//#693,true)//#417 #693
{
	//mImportFileDataArrayMutexでmutex制御するのでmImportFileDataArray自体のmutex制御は無しにする。
	//mImportFileDataArray.EnableThreadSafe(false);
}
//デストラクタ
AImportFileManager::~AImportFileManager()
{
}

#pragma mark ===========================

#pragma mark ---ImportFileData取得

//取得
AImportFileData&	AImportFileManager::GetImportFileDataByID( const AObjectID inImportFileDataObjectID )
{
	//fprintf(stderr,"GetImportFileDataByID()lock ");
	//Array自体はスレッド間で共有なのでmutex必要。中身の要素はスレッド間で同時アクセスすることはない（mImportFileDataArray_RecognizedFlagで制御される）
	AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
	//fprintf(stderr,"locked ");
	return mImportFileDataArray.GetObjectByID(inImportFileDataObjectID);
}
const AImportFileData&	AImportFileManager::GetImportFileDataConstByID( const AObjectID inImportFileDataObjectID ) const
{
	//fprintf(stderr,"GetImportFileDataConstByID()lock ");
	//Array自体はスレッド間で共有なのでmutex必要。中身の要素はスレッド間で同時アクセスすることはない（mImportFileDataArray_RecognizedFlagで制御される）
	AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
	//fprintf(stderr,"locked ");
	return mImportFileDataArray.GetObjectConstByID(inImportFileDataObjectID);
}

//#723
/**
ImportFileData取得・retain
@note ワードリスト検索スレッドからのコール用。すでに存在しないかもしれないのでそのときはNULLを返す。decrementするまではオブジェクトは解放されない。
*/
AImportFileData*	AImportFileManager::GetImportFileDataAndRetain( const AObjectID inImportFileDataObjectID )
{
	return mImportFileDataArray.GetObjectAndRetain(inImportFileDataObjectID);
}

#pragma mark ===========================

/**
ImportFileData検索orロード
@note ファイルからImportFileDataオブジェクトを検索する。未ロードなら、オブジェクト生成して、スレッドに解析要求して、そのオブジェクトを返す。thread-safe
*/
AObjectID	AImportFileManager::FindOrLoadImportFileDataByFile( const AFileAcc& inImportFile, const ABool inIsSystemHeader )//#852
{
	//不可視ファイル、および、非テキストファイルは対象外とする
	//（特に標準モードでインポートファイルの数が非常に大量になるのを防ぐため、非テキストファイルを除外する必要がある）
	if( inImportFile.IsInvisible() == true || GetApp().GetAppPref().IsBinaryFile(inImportFile) == true )
	{
		return kObjectID_Invalid;
	}
	
	//ロード済みのなかから検索し、見つかればそのObjectIDを返す
	
	//Init必要フラグ #860
	ABool	shouldInit = false;
	//#723 mImportFileDataArray_Pathのindexのずれ防止のため、排他制御箇所変更。
	AObjectID	importFileDataObjectID = kObjectID_Invalid;
	{
		//ImportFileDataArrayの排他制御（arrayの原子性保証。mImportFileDataArray_Pathも含めてindexのずれが発生しないようにする）
		AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
		
		AIndex	index = kIndex_Invalid;
		AText	path;
		inImportFile.GetNormalizedPath(path);
		index = mImportFileDataArray_Path.Find(path);
		if( index == kIndex_Invalid )
		{
			//未存在なので、新規生成する。
			
			//fprintf(stderr,"FindOrLoadImportFileDataByFile()lock ");
			//配列への追加
			//#723 上へ移動 AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
			//fprintf(stderr,"locked ");
			AImportFileDataFactory	factory(this,inImportFile);
			//fprintf(stderr,"before:%d ",AArrayAbstract::GetTotalAllocatedByteCount());
			index = mImportFileDataArray.AddNewObject(factory);
			mImportFileDataArray_Path.Add(path);//B0000 高速化
			//fprintf(stderr,"after:%d ",AArrayAbstract::GetTotalAllocatedByteCount());
			//fprintf(stderr,"added ");
			//path.OutputToStderr();
			//#860
			shouldInit = true;
		}
		//#723 mImportFileDataArray.GetObject(index).Init();//B0000 mutex解除後にinitしないと、SetMode()内からGetImportFileDataConstByID()が呼ばれる
		//Object ID取得
		importFileDataObjectID = mImportFileDataArray.GetObjectConst(index).GetObjectID();
	}
	//#723 上から移動
	//新規生成した場合、ImportFileObject初期化し、解析要求
	if( shouldInit == true )
	{
		GetImportFileDataByID(importFileDataObjectID).Init(inIsSystemHeader);//#852
	}
	//解析要求
	if( GetImportFileDataByID(importFileDataObjectID).IsRecognized() == false )//#860
	{
		if( GetApp().NVI_IsWorking() == true)//アプリケーション終了中はスレッドが終了している可能性があるのでキューに積まない
		{
			//解析要求キューにつむ
			GetApp().SPI_GetImportFileRecognizer().AddToRequestQueue(importFileDataObjectID);
			//スレッドをsleepから起こす
			GetApp().SPI_GetImportFileRecognizer().NVI_WakeTrigger();
		}
	}
	return importFileDataObjectID;
}

#pragma mark ===========================

#pragma mark ---インポートファイルロード、識別子情報を取得

/**
指定FileArrayについて、ImportFileData生成し、リンクリストを取得
inFileArrayで指定したファイルを、mImportFileArray配列にロードし、識別子情報を取得する。
ADocument_Textから、インポートファイルに変更が生じた場合、および、解析スレッド処理が完了した場合に、コールされる
*/
ABool	AImportFileManager::GetImportIdentifierLinkList( const ADocumentID inTextDocumentID, const AObjectArray<AFileAcc>& inFileArray, 
		const AModeIndex inModeIndex, AImportIdentifierLinkList& outImportIdentifierLinkList, const ABool inIsSystemHeader )//#852
{
	//fprintf(stderr,"GetImportIdentifierLinkList()");
	//各インポートファイルごとにImportFileDataを検索orロードする
	AArray<AObjectID>	rootObjectIDArray;
	for( AIndex i = 0; i < inFileArray.GetItemCount(); i++ )
	{
		//インポートファイルのインデックスを取得する（未ロードならロード・解析要求）
		AObjectID	objectID = FindOrLoadImportFileDataByFile(inFileArray.GetObjectConst(i),inIsSystemHeader);//#852
		if( objectID != kObjectID_Invalid )
		{
			//モードが同じ場合のみrootImportFileIndexに追加
			if( inModeIndex == GetImportFileDataConstByID(objectID).GetModeIndex() )
			{
				rootObjectIDArray.Add(objectID);
			}
		}
	}
	//解析途中完了用の区切りマークを入れる
	//途中完了マークdrop GetApp().SPI_GetImportFileRecognizer().AddToRequestQueue(kObjectID_Invalid);
	//
	ABool	threadRecognizing = false;
	//インポートファイル中のインポートファイルも含め、参照しているインポートファイル全てを検索し、ObjectIDをallObjectIDArrayへ格納する
	AHashArray<AObjectID>	allObjectIDArray;//#423
	for( AIndex i = 0; i < rootObjectIDArray.GetItemCount(); i++ )
	{
		if( GetImportFileDataConstByID(rootObjectIDArray.Get(i)).IsRecognized() == true )
		{
			MakeImportFileDataObjectIDArrayRecursive(rootObjectIDArray.Get(i),allObjectIDArray);
		}
		else
		{
			threadRecognizing = true;
		}
	}
	//B0000 080808 自分のファイルに対応するImportFileDataはallObjectIDArrayから取り除く
	if( inTextDocumentID != kObjectID_Invalid )//#253
	{
		//#0 高速化
		//自ドキュメントのファイルパス取得
		AFileAcc	docfile;
		GetApp().SPI_GetTextDocumentByID(inTextDocumentID).NVI_GetFile(docfile);
		AText	textdocfilepath;
		docfile.GetNormalizedPath(textdocfilepath);
		//自ドキュメントに対応するImportFileDataのObjectIDをファイルパスから検索
		AObjectID	textdocImportFileObjectID = kObjectID_Invalid;
		{
			AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
			
			//ファイルパスから検索
			AIndex	objindex = mImportFileDataArray_Path.Find(textdocfilepath);
			if( objindex != kIndex_Invalid )
			{
				textdocImportFileObjectID = mImportFileDataArray.GetObject(objindex).GetObjectID();
			}
		}
		//自ドキュメントに対応するImportFileDataをallObjectIDArrayから取り除く
		if( textdocImportFileObjectID != kObjectID_Invalid )
		{
			for( AIndex i = 0; i < allObjectIDArray.GetItemCount(); )
			{
				if( allObjectIDArray.Get(i) == textdocImportFileObjectID )
				{
					allObjectIDArray.Delete1(i);
				}
				else
				{
					i++;
				}
			}
		}
	}
	//参照しておりかつ解析済みのファイルの識別子へのリンクをoutImportKeywordDataListへ格納する
	outImportIdentifierLinkList.UpdateIdentifierLinkList(allObjectIDArray);//#423
	if( threadRecognizing == true )//B0000 全部Recognizedならスレッドwakeする必要がないので、NVI_WakeTrigger()もこのif内に含める
	{
		//解析スレッド開始トリガー
		GetApp().SPI_GetImportFileRecognizer().NVI_WakeTrigger();
		if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AImportFileRecognizer WakeTrigger from GetImportIdentifierLinkList()",this);
		//fprintf(stderr,"waketrigger(GetImportIdentifierLinkList)  ");
		//
		if( inTextDocumentID != kObjectID_Invalid )//#253
		{
			if( mWaitingTextDocumentIDArray.Find(inTextDocumentID) == kIndex_Invalid )
			{
				mWaitingTextDocumentIDArray.Add(inTextDocumentID);
			}
		}
		else//#253
		{
			if( mWaitingModePrefArray.Find(inModeIndex) == kIndex_Invalid )
			{
				mWaitingModePrefArray.Add(inModeIndex);
			}
		}
	}
	return threadRecognizing;
}

//ロード済みファイルの中で指定ファイルから再帰的に参照している全てのファイルを検索する
void	AImportFileManager::MakeImportFileDataObjectIDArrayRecursive( const AObjectID inImportFileDataObjectID, AHashArray<AObjectID>& ioObjectIDArray ) const
{
	//既にObjectIDがioObjectIDArrayに追加済み（訪問済み）なら何もせずリターン
	if( ioObjectIDArray.Find(inImportFileDataObjectID) != kIndex_Invalid )   return;
	//まず自分自身を訪問済みにする
	ioObjectIDArray.Add(inImportFileDataObjectID);
	//
	const AArray<AObjectID>&	recursiveObjectIDArray = GetImportFileDataConstByID(inImportFileDataObjectID).GetRecursiveImportFileDataObjectIDArrayConst();
	for( AIndex i = 0; i < recursiveObjectIDArray.GetItemCount(); i++ )
	{
		AObjectID	objectID = recursiveObjectIDArray.Get(i);
		if( GetImportFileDataConstByID(objectID).IsRecognized() == true )
		{
			MakeImportFileDataObjectIDArrayRecursive(objectID,ioObjectIDArray);
		}
	}
}

//Recognizerスレッドから、現在の解析キュー内の解析が完了した場合に、AApp経由でコールされる。
void	AImportFileManager::DoImportFileRecognized( const ABool inCompleted )
{
	//一旦ローカルにコピーしてから、その配列について実行する。
	//SPI_DoImportFileRecognized()のなかで、再度、mWaitingTextDocumentIDArrayに追加される可能性があるため。（SleepFlagセット中）
	AArray<AObjectID>	idarray;
	for( AIndex i = 0; i < mWaitingTextDocumentIDArray.GetItemCount(); i++ )
	{
		idarray.Add(mWaitingTextDocumentIDArray.Get(i));
	}
	//認識完了時、通知先ドキュメントリストを削除（現状、inCompletedは常にtrue）
	if( inCompleted == true )
	{
		mWaitingTextDocumentIDArray.DeleteAll();
	}
	//==================ドキュメントに通知==================
	while(idarray.GetItemCount()>0)
	{
		ADocumentID	docID = idarray.Get(idarray.GetItemCount()-1);
		if( GetApp().NVI_IsDocumentValid(docID) == true )
		{
			//fprintf(stderr,"SPI_DoImportFileRecognized()called%d  ",docID);
			GetApp().SPI_GetTextDocumentByID(docID).SPI_DoImportFileRecognized();
		}
		idarray.Delete1(idarray.GetItemCount()-1);
	}
	//==================モード設定に通知==================
	//#253
	//認識完了時、通知先モードを削除（現状、inCompletedは常にtrue）
	if( inCompleted == true )
	{
		AArray<AModeIndex>	modeindexarray;
		for( AIndex i = 0; i < mWaitingModePrefArray.GetItemCount(); i++ )
		{
			modeindexarray.Add(mWaitingModePrefArray.Get(i));
		}
		mWaitingModePrefArray.DeleteAll();
		while(modeindexarray.GetItemCount()>0)
		{
			AModeIndex	modeIndex = modeindexarray.Get(modeindexarray.GetItemCount()-1);
			GetApp().SPI_GetModePrefDB(modeIndex).DoImportFileRecognized();
			modeindexarray.Delete1(modeindexarray.GetItemCount()-1);
		}
	}
}

#pragma mark ===========================

#pragma mark ---インポートファイル変更反映

/**
ドキュメント保存時にAApp::SPI_DoTextDocumentSaved()経由でコールされる。再解析を行う。
*/
void	AImportFileManager::DoFileSaved( const AFileAcc& inFile )
{
	//==================リンクリストから参照を削除==================
	//ファイルに対応するImport File Dataのrecognized状態解除し、参照しているリンクリストから参照を全て削除する
	AObjectID	importFileDataObjectID = SetUnrecognizeAndDeleteFromLinkList(inFile,true);
	
	if( importFileDataObjectID != kObjectID_Invalid )
	{
		//==================再解析要求==================
		//解析要求
		GetApp().SPI_GetImportFileRecognizer().AddToRequestQueue(importFileDataObjectID);
		//解析スレッド開始トリガー
		GetApp().SPI_GetImportFileRecognizer().NVI_WakeTrigger();
		if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AImportFileRecognizer WakeTrigger from DoFileSaved()",this);
		//fprintf(stderr,"waketrigger(DoFileSaved)  ");
	}
}

//#896
/**
ImportFileDataをpurgeする（プロジェクト内のドキュメントが全て閉じられたときにコールされる。メモリー削減のため。）
*/
void	AImportFileManager::PurgeImportFileData( const ATextArray& inPathArray )
{
	//==================リンクリストからの参照を削除==================
	//ImportFileDataオブジェクトを検索し、それを参照しているLinkListを削除する
	AArray<AObjectID>	importFileDataArray;
	for( AIndex i = 0; i < inPathArray.GetItemCount(); i++ )
	{
		//ファイル取得
		AFileAcc	file;
		file.Specify(inPathArray.GetTextConst(i));
		//ファイルに対応するImport File Dataのrecognized状態解除し、参照しているリンクリストから参照を全て削除する
		AObjectID	importFileDataObjectID = SetUnrecognizeAndDeleteFromLinkList(file,false);
		if( importFileDataObjectID != kObjectID_Invalid )
		{
			importFileDataArray.Add(importFileDataObjectID);
		}
	}
	//==================ImportFileDataパージキューに追加する==================
	//（ここで直接実行してしまうと、import file dataにmutexがあるので、認識スレッドで認識実行中はmutex解除待ちになり、時間がかかる。）
	if( importFileDataArray.GetItemCount() > 0 )
	{
		//パージキューに追加
		GetApp().SPI_GetImportFileRecognizer().AddToImportFileDataPurgeRequestQueue(importFileDataArray);
		//sleep解除
		GetApp().SPI_GetImportFileRecognizer().NVI_WakeTrigger();
	}
}

/**
指定ファイルのImportFileDataのrecognized状態を解除し、参照しているリンクリストから参照を全て削除する
*/
AObjectID	AImportFileManager::SetUnrecognizeAndDeleteFromLinkList( const AFileAcc& inFile, const ABool inForReRecognize )
{
	//==================対応するImportFileDataのRecognizeフラグをOFFにする。==================
	//#723 mImportFileDataArray_Pathのindexのずれ防止のため、排他制御箇所変更。
	AObjectID	importFileDataObjectID = kObjectID_Invalid;
	{
		AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
		
		//指定ファイルに対応するImportFileDataを取得する
		AIndex	importFileDataArrayIndex = kIndex_Invalid;
		AText	path;
		inFile.GetNormalizedPath(path);
		importFileDataArrayIndex = mImportFileDataArray_Path.Find(path);
		
		//対応するimport file dataが無ければリターン
		if( importFileDataArrayIndex == kIndex_Invalid )   return kObjectID_Invalid;
		//Recognizedでなければ、そのうちRecognizeされるので、そのままにしておく
		//if( mImportFileDataArray.GetObject(importFileDataArrayIndex).IsRecognized() == false )   return;
		//Recognizedフラグを消す
		mImportFileDataArray.GetObject(importFileDataArrayIndex).SetRecognized(false);
		
		//指定ファイルに対応するImportFileDataのObjectIDを取得する
		importFileDataObjectID = mImportFileDataArray.GetObjectConst(importFileDataArrayIndex).GetObjectID();
	}
	
	//==================ドキュメントのLinkListを更新==================
	//全てのドキュメントのインポートファイルデータからこのファイルの識別子情報を削除する。
	for( AObjectID docID = GetApp().NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = GetApp().NVI_GetNextDocumentID(docID) )
	{
		//ドキュメントのLinkListから削除する
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_DeleteImportFileData(importFileDataObjectID,inFile) == true )
		{
			//再認識する場合（＝DoFileSaved()経由）は、通知先ドキュメントリストにドキュメントを追加しておく
			//（ただ、purgeの場合は、ドキュメントが削除されているのでここに来ない）
			if( inForReRecognize == true )
			{
				if( mWaitingTextDocumentIDArray.Find(docID) == kIndex_Invalid )
				{
					mWaitingTextDocumentIDArray.Add(docID);
				}
			}
		}
	}
	
	//==================ModePrefDBのシステムヘッダLinkListを更新==================
	//#253 全てのモード設定のインポートファイルデータからこのファイルの識別子情報を削除する。
	for( AModeIndex modeIndex = 0; modeIndex < GetApp().SPI_GetModeCount(); modeIndex++ )
	{
		if( GetApp().SPI_GetModePrefDB(modeIndex,false).IsLoaded() == true )//#517 ロード済みのモードのみ実行
		{
			//システムヘッダのLinkListから削除
			if( GetApp().SPI_GetModePrefDB(modeIndex).DeleteImportFileData(importFileDataObjectID,inFile) == true )
			{
				//再認識する場合（＝DoFileSaved()経由）は、通知先モード設定リストにモード設定を追加しておく
				//（ただ、システムヘッダをpurgeすることがないので、purgeの場合はここには来ない）
				if( inForReRecognize == true )
				{
					if( mWaitingModePrefArray.Find(modeIndex) == kIndex_Invalid )
					{
						mWaitingModePrefArray.Add(modeIndex);
					}
				}
			}
		}
	}
	
	return importFileDataObjectID;
}

#pragma mark ===========================

#pragma mark ---削除

//#349
/**
指定モードの解析データを全て削除し、文法解析スレッドを停止する
*/
void	AImportFileManager::ClearRecognizedDataAndStopRecognizer( const AIndex inModeIndex )
{
	//==================認識スレッド停止==================
	//ImportFileRecognizerの動作停止待ち
	GetApp().SPI_GetImportFileRecognizer().ClearRequestQueue(inModeIndex);
	GetApp().SPI_GetImportFileRecognizer().NVI_WaitAbortThread();
	GetApp().SPI_GetImportFileRecognizer().ClearRequestQueue(inModeIndex);//#349 AImportFileRecognizer::NVIDO_ThreadMain()内のループを抜けるときにキュー追加されている可能性があるため
	
	//==================ドキュメント内のLinkList削除==================
	//ドキュメント内のAImportIdentifierLinkListからAImportFileDataへのリンクが貼られているので、
	//AImportFileDataを全削除する前に、指定モードのドキュメント内の、リンクデータを全て削除する
	for( AObjectID docID = GetApp().NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; 
				docID = GetApp().NVI_GetNextDocumentID(docID) )
	{
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex )
		{
			//ドキュメント内のLinkListを削除
			GetApp().SPI_GetTextDocumentByID(docID).SPI_DeleteAllImportFileData();
			//認識要求中のドキュメントの記憶を削除
			mWaitingTextDocumentIDArray.FindAndDelete(docID);
		}
	}
	
	//==================モード設定内のシステムヘッダLinkList削除==================
	//#253 全てのモード設定のインポートファイルデータからこのファイルの識別子情報を削除する。
	/*
	for( AModeIndex modeIndex = 0; modeIndex < GetApp().SPI_GetModeCount(); modeIndex++ )
	{
		//SPI_GetModePrefDB()の第2引数をfalseにすることにより、モードデータがLoadされていない場合に、Loadしないようにする。
		//モードのLoadが走ると、Recoginizerのスレッドが動き出してしまうため。
		GetApp().SPI_GetModePrefDB(modeIndex,false).DeleteAllImportFileData();
	}
	*/
	GetApp().SPI_GetModePrefDB(inModeIndex).DeleteAllImportFileData();
	//認識要求中のmode indexの記憶を削除
	mWaitingModePrefArray.FindAndDelete(inModeIndex);//#253
	
	//==================RecognizedフラグをOFFにする==================
	{
		//排他制御
		AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
		
		AItemCount	itemCount = mImportFileDataArray.GetItemCount();
		for( AIndex index = 0; index < itemCount; index++ )
		{
			if( mImportFileDataArray.GetObject(index).GetModeIndex() == inModeIndex )
			{
				//Recognizedフラグを消す
				mImportFileDataArray.GetObject(index).SetRecognized(false);
			}
		}
	}
}

/**
文法解析スレッドを再開する
*/
void	AImportFileManager::RestartRecognizer( const AIndex inModeIndex )
{
	//==================認識スレッド再起動==================
	if( GetApp().SPI_GetImportFileRecognizer().NVI_IsThreadWorking() == false )
	{
		GetApp().SPI_GetImportFileRecognizer().NVI_Run();
	}
	
	/*各ドキュメントのLinkListは、次回文法認識完了時に更新されるので、ここからは明示的な更新はしない。
	//==================ドキュメントLinkList更新==================
	for( AObjectID docID = GetApp().NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; 
				docID = GetApp().NVI_GetNextDocumentID(docID) )
	{
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex )
		{
			//ドキュメント内のLinkListを更新
			GetApp().SPI_GetTextDocumentByID(docID).SPI_UpdateImportFileData();
		}
	}
	*/
	
	//==================モード設定内のシステムヘッダLinkList更新==================
	GetApp().SPI_GetModePrefDB(inModeIndex).UpdateSystemHeaderFileData();
	
}

#pragma mark ===========================
#pragma mark [クラス]AImportFileRecognizer
#pragma mark ===========================
//ImportFileの文法解析スレッド（常時起動スレッド）

#pragma mark ---コンストラクタ／デストラクタ

AImportFileRecognizer::AImportFileRecognizer( AObjectArrayItem* inParent ) : AThread(inParent)//#272
,mRequestQueue_CurrentProcessingImportFileDataObjectID(kObjectID_Invalid)
{
}
AImportFileRecognizer::~AImportFileRecognizer()
{
}

#pragma mark ===========================

#pragma mark ---要求キューへの追加

void	AImportFileRecognizer::AddToRequestQueue( const AObjectID inImportFileDataObjectID )
{
	if( NVM_IsAborted() == true )   return;//#253
	
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	
	/*解析途中結果drop
	if( inImportFileDataObjectID == kObjectID_Invalid )
	{
		mRequestQueue.Add(kObjectID_Invalid);
		return;
	}
	*/
	
	//------------------パージキューから削除------------------
	//（この時点でパージ待ちがあると、認識を実行した後、パージされてしまう可能性がある。）
	{
		AIndex	index = mImportFileDataPurgeRequestQueue.Find(inImportFileDataObjectID);
		if( index != kIndex_Invalid )
		{
			mImportFileDataPurgeRequestQueue.Delete1(index);
		}
	}
	
	//------------------認識要求キューに追加------------------
	if( mRequestQueue.Find(inImportFileDataObjectID) == kIndex_Invalid &&
				mRequestQueue_CurrentProcessingImportFileDataObjectID != inImportFileDataObjectID )
	{
		mRequestQueue.Add(inImportFileDataObjectID);
		//fprintf(stderr,"AddToRequestQueue:%d \n",mRequestQueue.GetItemCount());
	}
}

void	AImportFileRecognizer::ClearRequestQueue( const AModeIndex inModeIndex )
{
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	for( AIndex i = 0; i < mRequestQueue.GetItemCount();  )
	{
		AObjectID	id = mRequestQueue.Get(i);
		if( id != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetImportFileManager().GetImportFileDataByID(id).GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )//#349
			{
				mRequestQueue.Delete1(i);
			}
			else
			{
				i++;
			}
		}
		else
		{
			mRequestQueue.Delete1(i);
		}
	}
	//fprintf(stderr,"ClearRequestQueue:%d \n",mRequestQueue.GetItemCount());
}

//#0
/**
要求キュー個数取得
*/
AItemCount	AImportFileRecognizer::GetRequestQueueItemCount() const
{
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	return mRequestQueue.GetItemCount();
}

//#896
/**
要求キューから削除
*/
void	AImportFileRecognizer::RemoveFromRequestQueue( const AObjectID inImportFileDataObjectID )
{
	if( NVM_IsAborted() == true )   return;//#253
	
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	AIndex	index = mRequestQueue.Find(inImportFileDataObjectID);
	if( index != kIndex_Invalid )
	{
		mRequestQueue.Delete1(index);
	}
}

//#896
/**
purgeキューへ追加
*/
void	AImportFileRecognizer::AddToImportFileDataPurgeRequestQueue( const AArray<AObjectID>& inImportFileDataArray )
{
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	for( AIndex i = 0; i < inImportFileDataArray.GetItemCount(); i++ )
	{
		AObjectID	importFileDataObjectID = inImportFileDataArray.Get(i);
		
		//------------------解析要求キューから削除------------------
		{
			AIndex	index = mRequestQueue.Find(importFileDataObjectID);
			if( index != kIndex_Invalid )
			{
				mRequestQueue.Delete1(index);
			}
		}
		
		//------------------purgeキューへ追加------------------
		if( mImportFileDataPurgeRequestQueue.Find(importFileDataObjectID) == kIndex_Invalid )
		{
			mImportFileDataPurgeRequestQueue.Add(importFileDataObjectID);
		}
	}
}

//#896
/**
purgeキューから削除
*/
void	AImportFileRecognizer::RemoveFromImportFileDataPurgeRequestQueue( const AObjectID inImportFileDataObjectID )
{
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	AIndex	index = mImportFileDataPurgeRequestQueue.Find(inImportFileDataObjectID);
	if( index != kIndex_Invalid )
	{
		mImportFileDataPurgeRequestQueue.Delete1(index);
	}
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

void	AImportFileRecognizer::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AImportFileRecognizer::NVIDO_ThreadMain started.",this);
	
	//要求キュー初期化
	{
		AStMutexLocker	mutexlocker(mRequestQueueMutex);
		mRequestQueue.DeleteAll();
		mRequestQueue_CurrentProcessingImportFileDataObjectID = kObjectID_Invalid;
		mImportFileDataPurgeRequestQueue.DeleteAll();
	}
	
	ABool	debugmode = GetApp().SPI_GetMiscDebugMode();
	try
	{
		while(NVM_IsAborted()==false)
		{
			//トリガーまでスリープ
			if( debugmode == true )   fprintf(stderr,"sleep ");
			NVM_Sleep();
			if( NVI_GetSleepFlag() == true )   continue;
			if( NVM_IsAborted() == true )   break;
			if( debugmode == true )   fprintf(stderr,"wake\n");
			NVM_SleepWithTimer(2);//二度寝
			if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AImportFileRecognizer::NVIDO_ThreadMain wake.",this);
			if( NVM_IsAborted() == true )   break;
			
			//キューがないときに永久に繰り返す問題を対策（ABase::PostToMainInternalEventQueue()をコールしないようにここでcontinue）
			if( /*#0 mRequestQueue.GetItemCount()*/GetRequestQueueItemCount() == 0 )   continue;
			//キューが空になるまでループ
			//fprintf(stderr,"%d ",mRequestQueue.GetItemCount());
			AItemCount	dequeueObjectCount = 0;
			while( /*#0 mRequestQueue.GetItemCount()*/GetRequestQueueItemCount() > 0 && 
						NVM_IsAborted() == false && NVI_GetSleepFlag() == false )
			{
				//キューから取りだし
				AObjectID	importFileDataObjectID = kObjectID_Invalid;
				{
					AStMutexLocker	mutexlocker(mRequestQueueMutex);
					if( mRequestQueue.GetItemCount() == 0 )   continue;//#349
					importFileDataObjectID = mRequestQueue.Get(0);
					mRequestQueue.Delete1(0);
					/*途中完了通知 drop
					if( importFileDataObjectID == kObjectID_Invalid )
					{
						if( dequeueObjectCount > 0 )
						{
							ABase::PostToMainInternalEventQueue(kInternalEvent_ImportFileRecognizePartialCompleted,GetObjectID(),0,GetEmptyText(),GetEmptyObjectIDArray());
						}
						//
						continue;
					}
					*/
					dequeueObjectCount++;
					//既にrecognizedなImportFileDataオブジェクトの場合は何もせずリターン
					//recognized判定～キューへの追加の間に、認識完了が発生すると、ここに来る可能性がある。
					//認識済みフラグは排他制御のためのフラグにもなっているので、認識フラグONなのに認識処理を行うと、
					//各リンクリストのindexの指すデータが存在しなくなってしまう。
					if( GetApp().SPI_GetImportFileManager().GetImportFileDataByID(importFileDataObjectID).IsRecognized() == true )
					{
						//_ACError("",NULL);
						continue;
					}
					//下記の処理中に、同じImportFileDataを要求キューに積んでしまわないように、
					//現在処理中のImportFileDataのIDを記憶する。（AddToRequestQueue()にて、キュー内と、
					//mRequestQueue_CurrentProcessingImportFileDataObjectIDに一致する場合は、キューに積まないようにしている。）
					mRequestQueue_CurrentProcessingImportFileDataObjectID = importFileDataObjectID;
				}
				{
					//メインスレッドへProgress通知
					AObjectIDArray	objectIDArray;
					ANumber	prog = GetRequestQueueItemCount();//#0 mRequestQueue.GetItemCount();
					if( (prog%10) == 0 )
					{
						ABase::PostToMainInternalEventQueue(kInternalEvent_ImportFileRecognizeProgress,GetObjectID(),prog,GetEmptyText(),objectIDArray);
					}
				}
				
				//キューからとりだしたObjectIDに対応するImportFileDataの解析
				//#723 AImportFileData&	importFileData = GetApp().SPI_GetImportFileManager().GetImportFileDataByID(importFileDataObjectID);
				//ImportFileData削除時、その中のTextInfoのデータが削除されるのはImportFileDataのデストラクタ内なので、
				//AObjectArrayIDIndexedのgetandretain機構を使えば、DecrementRetainCount()するまでは、TextInfoのデータが保証される。
				AImportFileData*	importFileDataPtr = 
									GetApp().SPI_GetImportFileManager().GetImportFileDataAndRetain(importFileDataObjectID);
				if( importFileDataPtr != NULL )//既にImportFileDataが削除済みの場合は、NULLが返る
				{
					//Import File Dataへのポインタのretainを確実に解放するためのスタッククラス
					AStDecrementRetainCounter	releaser(importFileDataPtr);
					
					AImportFileData&	importFileData = *(importFileDataPtr);
					//#723
					//ImportFileDataの更新を行う間、words list finder等がキーワード検索時にImportFileData(の中のTextInfo)にアクセスしないように排他する
					AStImportFileDataLocker	locker(importFileData);
					//
					AFileAcc	importFile = importFileData.GetFile();
					//サイズが制限値未満のファイルのみ文法認識する（大きすぎるファイルの文法認識にスレッド占有されるのを防ぐため） #695
					if( importFile.GetFileSize() < kLimit_ImportFileRecognizer_MaxByteCountToRecognize )
					{
						//テキスト読み込み
						AText	text;
						AModeIndex	modeIndex = kStandardModeIndex;
						AText	tecname;
						GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_ImportFileRecognize,importFile,text,modeIndex,tecname);//#723 ドキュメントからテキスト取得する必要が無いのでSPI_GetTextFromFileOrDocument()から変更
						//#723 バイナリファイルと思しきときは、認識もワードリスト作成もしない
						if( text.SuspectBinaryData() == false )
						{
							//ImportFileDataオブジェクトに設定したmodeindexを取得する。（上のSPI_LoadTextFromFile()で取得したmodeIndexは使用しない）
							modeIndex = importFileData.GetModeIndex();
							//debug
							if( false && debugmode  == true )
							{
								AText	t;
								t.SetFormattedCstring("Recog:");
								t.OutputToStderr();
								AText	filename;
								importFileData.GetFile().GetFilename(filename);
								filename.OutputToStderr();
								t.SetFormattedCstring(" size:%d mode:%d  totalalloc:%d\n",
											importFile.GetFileSize(),
											modeIndex,
											AArrayAbstract::GetTotalAllocatedByteCount());
								t.OutputToStderr();
							}
							//TextInfo解析
							importFileData.GetTextInfo().CalcLineInfoAllWithoutView(text,mAborted);//win
							if( NVM_IsAborted() == true )   break;
							//#695 AIndex	startLineIndex, endLineIndex;
							ABool	importChanged = false;
							AArray<AUniqID>	addedIdentifier;
							AArray<AIndex>		addedIdentifierLineIndex;
							//#695 AArray<AUniqID>	deletedIdentifier;//win
							//上へ移動 if( text.SuspectBinaryData() == false )//#723 バイナリファイルと思しきときは文法解析はせず、ワードリスト生成のみ行う
							{
								AText	path;//#998
								importFile.GetPath(path);//#998
								importFileData.GetTextInfo().RecognizeSyntaxAll(text,path,//#695 startLineIndex,endLineIndex,deletedIdentifier, #998
											addedIdentifier,addedIdentifierLineIndex,importChanged,mAborted);//R0000 #698
							}
							if( NVM_IsAborted() == true )   break;
							//TextInfoにワードリスト生成
							//#1210 ワードリスト ドロップ importFileData.GetTextInfo().UpdateWordsList(text);//#723
							//
							importFileData.IncrementRevisionNumber();//#423
							//spos, eposをTextIndex（文書の最初からのオフセット）に変換して保存sharkcheck
							/* for( AIndex i = 0; i < addedIdentifier.GetItemCount(); i++ )
							{
							AObjectID	objectID = addedIdentifier.Get(i);
							ATextPoint	spt, ept;
							importFileData.GetTextInfo().GetGlobalIdentifierRange(objectID,spt,ept);
							ATextIndex	spos = importFileData.GetTextInfo().GetTextIndexFromTextPoint(spt);
							ATextIndex	epos = importFileData.GetTextInfo().GetTextIndexFromTextPoint(ept);
							importFileData.GetTextInfo().SetGlobalIdentifierStartEndIndex(objectID,spos,epos);
							}*/
							
							//win 080725 ImportRecursive指定対応
							//（たとえばxxx.cppからさらにそのインポートファイルを読み出さないようにする。
							//　ADocument_Text::UpdateImportFileData()のほうは、xxx.hから別拡張子xxx.cppも読み出す。）
							AText	filename;
							importFile.GetFilename(filename);
							AText	suffix;
							filename.GetSuffix(suffix);
							if( GetApp().SPI_GetModePrefDB(modeIndex).GetSyntaxDefinition().IsImportRecursive(suffix) == true )
							{
								//TextInfoからインポートファイルのArrayをimportFileArrayに取得する。
								AFileAcc	folder;
								folder.SpecifyParent(importFile);
								AObjectArray<AFileAcc>	importFileArray;
								importFileData.GetTextInfo().GetImportFileArray(folder,importFileArray);
								//再帰インポートファイルを検索・ロード（＋解析要求）して追加
								for( AIndex i = 0; i < importFileArray.GetItemCount(); i++ )
								{
									AObjectID	objectID = GetApp().SPI_GetImportFileManager().FindOrLoadImportFileDataByFile(importFileArray.GetObjectConst(i),false);
									if( objectID != kObjectID_Invalid )
									{
										if( modeIndex == GetApp().SPI_GetImportFileManager().GetImportFileDataByID(objectID).GetModeIndex() )
										{
											importFileData.GetRecursiveImportFileDataObjectIDArray().Add(objectID);
										}
									}
								}
							}
							
							//#693 メモリ節約のためRegExpデータをPurge
							importFileData.GetTextInfo().PurgeForImportFileData();
						}
					}
					//解析済みにする
					{
						AStMutexLocker	mutexlocker(mRequestQueueMutex);
						//現在の解析ImportFileDataオブジェクト変数クリア
						mRequestQueue_CurrentProcessingImportFileDataObjectID = kObjectID_Invalid;
						//認識済みフラグON
						importFileData.SetRecognized();
					}
				}
			}
			if( NVM_IsAborted() == false )
			{
				//完了通知
				ABase::PostToMainInternalEventQueue(kInternalEvent_ImportFileRecognizeCompleted,GetObjectID(),0,GetEmptyText(),GetEmptyObjectIDArray());
			}
			
			//==================purge==================
			//#896
			//purgeキューにつまれた全てのImport File Dataのデータをpurgeする
			{
				AStMutexLocker	mutexlocker(mRequestQueueMutex);
				while( mImportFileDataPurgeRequestQueue.GetItemCount() > 0 )
				{
					if( NVM_IsAborted() == true )   break;
					
					//キューから取得
					AObjectID	importFileDataObjectID = mImportFileDataPurgeRequestQueue.Get(0);
					mImportFileDataPurgeRequestQueue.Delete1(0);
					
					//purge実行
					GetApp().SPI_GetImportFileManager().GetImportFileDataByID(importFileDataObjectID).DeleteTextInfoData();
				}
			}
			if( NVM_IsAborted() == true )   break;
		}
	}
	catch(...)
	{
		_ACError("AImportFileRecognizer::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AImportFileRecognizer::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================
#pragma mark [クラス]AImportFileData
#pragma mark ===========================
//ImportFileごとのデータ

#pragma mark ---コンストラクタ／デストラクタ

//コンストラクタ
AImportFileData::AImportFileData( AObjectArrayItem* inParent, const AFileAcc& inFile ) 
	: AObjectArrayItem(inParent), mTextInfo(this), mRecognized(false)
	,mRevisionNumber(0)//#423
,mModeIndex(kStandardModeIndex)
{
	mFile.CopyFrom(inFile);
}
void	AImportFileData::Init( const ABool inIsSystemHeader )//B0000 mutex解除後にInitする #852
{
	mModeIndex = GetApp().SPI_GetModeIndexFromFile(mFile);
	
	AStImportFileDataLocker	locker(*this);
	
	mTextInfo.SetMode(mModeIndex,false);
	//#467 システムヘッダ判定してTextInfoへ設定。（directiveを無効にするため。）
	//#852 if( GetApp().SPI_GetModePrefDB(mModeIndex).IsSystemHeaderFile(mFile) == true )
	if( inIsSystemHeader == true )//#852
	{
		mTextInfo.SetSystemHeaderMode();
	}
}
//デストラクタ
AImportFileData::~AImportFileData()
{
}

//#896
/**
text infoデータを全て削除(purge)
*/
void	AImportFileData::DeleteTextInfoData()
{
	AStImportFileDataLocker	locker(*this);
	
	mTextInfo.DeleteAllInfo();
	mRecognized = false;
	mRevisionNumber = 0;
}




