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

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextInfo.h"

class AImportIdentifierLinkList;


#pragma mark ===========================
#pragma mark [クラス]AImportFileRecognizer
//ImportFileの文法解析スレッド

class AImportFileRecognizer : public AThread
{
	//コンストラクタ／デストラクタ
  public:
	AImportFileRecognizer( AObjectArrayItem* inParent );//#272
	~AImportFileRecognizer();
	
	//要求キューへの追加
  public:
	void					AddToRequestQueue( const AObjectID inImportFileDataObjectID );
	void					ClearRequestQueue( const AModeIndex inModeIndex );
	void					RemoveFromRequestQueue( const AObjectID inImportFileDataObjectID );//#896
	AItemCount				GetRequestQueueItemCount() const;
	//purgeキュー
  public:
	void					AddToImportFileDataPurgeRequestQueue( const AArray<AObjectID>& inImportFileDataArray );
	void					RemoveFromImportFileDataPurgeRequestQueue( const AObjectID inImportFileDataObjectID );
	
  private:
	void					NVIDO_ThreadMain();
	
  private:
	AArray<AObjectID>					mRequestQueue;
	AArray<AObjectID>					mImportFileDataPurgeRequestQueue;
	mutable AThreadMutex				mRequestQueueMutex;
	//現在解析中のimport file data
	AObjectID							mRequestQueue_CurrentProcessingImportFileDataObjectID;
	
	
};

#pragma mark ===========================
#pragma mark [クラス]AImportFileRecognizerFactory
//
class AImportFileRecognizerFactory : public AThreadFactory
{
  public:
	AImportFileRecognizerFactory( AObjectArrayItem* inParent ) : mParent(inParent)//#272
	{
	}
	AImportFileRecognizer*	Create() 
	{
		return new AImportFileRecognizer(mParent);//#272
	}
  private:
	AObjectArrayItem* mParent;//#272
};

#pragma mark ===========================
#pragma mark [クラス]AImportFileData
//ImportFileごとのデータ
class AImportFileData : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	AImportFileData( AObjectArrayItem* inParent, const AFileAcc& inFile );
	~AImportFileData();
	void					Init( const ABool inIsSystemHeader );//B0000 mutex解除後にInitする #852
	
	//
  public:
	AModeIndex				GetModeIndex() const { return mModeIndex; }
	const AFileAcc&			GetFile() const { return mFile; }
	ATextInfo&				GetTextInfo() { return mTextInfo; }
	const ATextInfo&		GetTextInfoConst() const { return mTextInfo; }
	AArray<AObjectID>&		GetRecursiveImportFileDataObjectIDArray() { return mRecursiveImportFileDataObjectIDArray; }
	const AArray<AObjectID>&		GetRecursiveImportFileDataObjectIDArrayConst() const { return mRecursiveImportFileDataObjectIDArray; }
	ABool					IsRecognized() const { return mRecognized; }
	void					SetRecognized( const ABool inRecognized = true ) { mRecognized = inRecognized; }
	void					DeleteTextInfoData();
  private:
	AFileAcc							mFile;
	ATextInfo							mTextInfo;
	AModeIndex							mModeIndex;
	AArray<AObjectID>					mRecursiveImportFileDataObjectIDArray;
	ABool								mRecognized;
	
	//ロック #723
  public:
	void					Lock() const { mMutex.Lock(); }
	void					Unlock() const { mMutex.Unlock(); }
  private:
	mutable AThreadMutex				mMutex;
	
	//Revision Number #423
  public:
	ANumber					GetRevisionNumber() const { return mRevisionNumber; }
	void					IncrementRevisionNumber() { mRevisionNumber++; }
	ANumber								mRevisionNumber;
};

//#723
/**
Import File Data排他のためのクラス
文法解析スレッドからの文法解析による識別子追加、ワードリスト検索スレッド等からの識別子検索を排他する
*/
class AStImportFileDataLocker
{
  public:
	AStImportFileDataLocker( const AImportFileData& inImportFileData ) : mImportFileData(inImportFileData)
	{ mImportFileData.Lock(); }
	~AStImportFileDataLocker()
	{ mImportFileData.Unlock(); }
  private:
	const AImportFileData&	mImportFileData;
};

#pragma mark ===========================
#pragma mark [クラス]AImportFileDataFactory
//
class AImportFileDataFactory : public AAbstractFactoryForObjectArray<AImportFileData>
{
  public:
	AImportFileDataFactory( AObjectArrayItem* inParent, const AFileAcc& inFile )
	: mParent(inParent)
	{
		mFile.CopyFrom(inFile);
	}
	AImportFileData*	Create() 
	{
		return new AImportFileData(mParent,mFile);
	}
  private:
	AObjectArrayItem*	mParent;
	AFileAcc			mFile;
};



#pragma mark ===========================
#pragma mark [クラス]AImportFileManager
//
class AImportFileManager : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	AImportFileManager();
	~AImportFileManager();
	
	//取得
  public:
	AImportFileData&		GetImportFileDataByID( const AObjectID inImportFileDataObjectID );
	const AImportFileData&	GetImportFileDataConstByID( const AObjectID inImportFileDataObjectID ) const;
	AImportFileData*		GetImportFileDataAndRetain( const AObjectID inImportFileDataObjectID );
	
	//
  public:
	void					ClearRecognizedDataAndStopRecognizer( const AIndex inModeIndex );//#349
	void					RestartRecognizer( const AIndex inModeIndex );
	
	//ImportFileData検索orロード
  public:
	AObjectID				FindOrLoadImportFileDataByFile( const AFileAcc& inImportFile, const ABool inIsSystemHeader );//#852
	
	//インポートファイル変更反映
  public:
	void					DoFileSaved( const AFileAcc& inFile );
	void					PurgeImportFileData( const ATextArray& inPathArray );//#896
  private:
	AObjectID				SetUnrecognizeAndDeleteFromLinkList( const AFileAcc& inFile, const ABool inForReRecognize );//#896
	
	//インポートファイルロード、識別子情報を取得
  public:
	ABool					GetImportIdentifierLinkList( const ADocumentID inTextDocumentID, const AObjectArray<AFileAcc>& inImportFileArray, 
							const AModeIndex inModeIndex, AImportIdentifierLinkList& outImportKeywordDataList, const ABool inIsSystemHeader );//#852
	void					DoImportFileRecognized( const ABool inCompleted );
  private:
	void					MakeImportFileDataObjectIDArrayRecursive( const AObjectID inImportFileDataObjectID, AHashArray<AObjectID>& ioObjectIDArray ) const;
	AIndex					GetImportFileArrayItemIndex( const AFileAcc& inImportFile );
	
	//#0 高速化
	//Import FIle DataをAImportIdentifierLinkListクラスからアクセスするための関数
  public:
	AThreadMutex&			GetImportFileDataArrayMutex() { return mImportFileDataArrayMutex; }
	const AObjectArrayIDIndexed< AImportFileData >&	GetImportFileDataArrayConst() const { return mImportFileDataArray; }
	
  private:
	AObjectArrayIDIndexed< AImportFileData >		mImportFileDataArray;//#693
	mutable AThreadMutex				mImportFileDataArrayMutex;
	AHashTextArray						mImportFileDataArray_Path;//B0000 高速化
	
	AArray<ADocumentID>					mWaitingTextDocumentIDArray;
	AArray<AModeIndex>					mWaitingModePrefArray;//#253
};






