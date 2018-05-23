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

AFTP

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ASocket.h"

enum AFTPConnectionProcessType
{
	kFTPConnectionProcessType_GetDirectory = 0,
	kFTPConnectionProcessType_GetFile,
	kFTPConnectionProcessType_PutFile
};

#pragma mark ===========================

#pragma mark [クラス]AFTPConnection
//FTP接続スレッドオブジェクト

class AFTPConnection : public AThread
{
	//コンストラクタ／デストラクタ
  public:
AFTPConnection( AObjectArrayItem* inParent );
void	SPI_SetRemoteConnectionObjectID( const AObjectID inRemoteConnectionObjectID );
  private:
	AFTPConnectionProcessType			mProcessType;
	ABool								mBinaryMode;
	ASocket								mControlSocket;
	ASocket								mDataSocket;
	ASocket								mListenSocket;
	AText								mServer;
	AText								mUser;
	AText								mPassword;
	AText								mPath;
	AText								mFTPURL;
	APortNumber							mPort;
	ABool								mPassive;
	ANumber								mProxy;
	AText								mProxyServer;
	AText								mProxyUser;
	AText								mProxyPassword;
	ANumber								mProxyMode;
	AText								mFTPFolderActionText;
	ANumber								mFTPFolderDepth;
	AObjectID							mFTPFolderMenuObjectID;
	//#361AObjectID							mGetFileReceiverObjectID;
	AText								mSendData;
	AObjectID							mRemoteConnectionObjectID;//#361
	AText								mReceivedText;//#361
	ATextArray							mDirectoryNameList;//#361
	ABoolArray							mIsDirectoryList;//#361
	
	//スレッド中断
  private:
	void					NVIDO_AbortThread();
	
	//フォルダ取得
  public:
	void					SPNVI_Run_GetDirectoryForFTPFolder( const AText& inFTPURL, const ANumber inDepth, const AText& inActionText );
	void					SPNVI_Run_GetDirectory( const AText& inFTPURL );//#361
	void					SPI_GetDirectoryResult( ATextArray& outFileNameArray, ABoolArray& outIsDirectoryArray ) const;//#361
  private:
	void					ThreadMain_GetDirectory();
	void					GetDirectoryRecursive( const AText& inPath, const ANumber inDepth, const AObjectID inDynamicMenuObjectID );
	
	//ファイル取得
  public:
	void					SPNVI_Run_GetFile( const AText& inFTPURL, const ABool inAllowNoFile );
  private:
	void					ThreadMain_GetFile();
	ABool								mGetFileAllowNoFile;
	
	//ファイル保存
  public:
	void					SPNVI_Run_PutFile( const AText& inFTPURL, const AText& inData );
	ABool					SPI_GetLastPutFileOK() const { return mLastPutFileOK; }
  private:
	void					ThreadMain_PutFile();
	ABool								mLastPutFileOK;
	
	//共通処理
	
	//スレッドメインルーチン（ディスパッチ）
  private:
	void					NVIDO_ThreadMain();
	
	//コントロールポート制御
  private:
	void					ConnectControlPort( const AText& inServer, const APortNumber inPort );
	void					Read_WaitFTPResponceCode( AText& outText, ANumber& outResponceCode );
	ABool					DequeueLine( AText& outText );
	void					SendUSER( const AText& inUser );
	void					SendPASS( const AText& inPassword );
	void					SendSITE( const AText& inSite );
	void					Login();
	void					InitByFTPURL( const AText& inFTPURL );
  private:
	AText								mControlSocketReadQueue;
	
	//データポート制御
  private:
	void					ConnectOrListenDataPort();
	void					CloseDataPort();
	
	//プログレス表示イベント送信
  private:
	void					HideProgress();
	void					ProgressText( AText& inText );
	void					ProgressPercent( ANumber inPercent );
	void					_Log( const AText& inText, const ABool inSend );
	void					ShowError( AConstCharPtr inErrorString );
	
	//FTP応答番号(RFC)
	static const ANumber kCommandOK = 200;
	static const ANumber kServiceReady = 220;
	static const ANumber kUserNameOK = 331;
	static const ANumber kUserLoggedIn = 230;
	static const ANumber kCommandNotImplemented = 202;
	static const ANumber kEnteringPassiveMode = 227;
	static const ANumber kDataConnectionAlreadyOpen = 125;
	static const ANumber kFileStatusOK = 150;
	static const ANumber kClosingDataConnection = 226;
	static const ANumber kConnectionClosed = 426;
};

#pragma mark ===========================
#pragma mark [クラス]AFTPConnectionFactory
//
class AFTPConnectionFactory : public AThreadFactory
{
  public:
	AFTPConnectionFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AFTPConnection*	Create() 
	{
		return new AFTPConnection(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};

