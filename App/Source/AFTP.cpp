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

#include "stdafx.h"

#include "AFTP.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AFTPConnection
#pragma mark ===========================
//FTP接続制御

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AFTPConnection::AFTPConnection( AObjectArrayItem* inParent ) : AThread(inParent), mBinaryMode(true), mLastPutFileOK(false)
,mRemoteConnectionObjectID(kObjectID_Invalid)
,mPort(21)
{
}

//#361
/**
リモート接続オブジェクト設定
*/
void	AFTPConnection::SPI_SetRemoteConnectionObjectID( const AObjectID inRemoteConnectionObjectID )
{
	mRemoteConnectionObjectID = inRemoteConnectionObjectID;
}

#pragma mark ===========================

#pragma mark ---スレッド中断

//スレッド中断
void	AFTPConnection::NVIDO_AbortThread()
{
	if( mProcessType == kFTPConnectionProcessType_GetDirectory || mProcessType == kFTPConnectionProcessType_GetFile )
	{
		//connect等のブロックから戻すためにソケットを強制クローズする
		//フォルダ／ファイル取得中の強制クローズはファイル破壊等にはならない。また、強制クローズは線抜きと同等であり、いずれにしても異常系として作り込まれておくべき動作である。
		//ファイル保存中はクローズしない。（そもそもUIで中断不可だが、念のためここでも条件をつけておく）
		mDataSocket.Close();
		mControlSocket.Close();
		mListenSocket.Close();
		//検討必要：ここはメインスレッド上になる
	}
}

#pragma mark ===========================

#pragma mark ---フォルダ取得

/**
フォルダ取得スレッド起動（FTPフォルダメニュー用）
*/
void	AFTPConnection::SPNVI_Run_GetDirectoryForFTPFolder( const AText& inFTPURL, const ANumber inDepth, const AText& inActionText )
{
	//スレッド起動中は同じインスタンスでの重複起動不可
	if( NVI_IsThreadWorking() == true )   return;
	
	//ProcessType設定
	mProcessType = kFTPConnectionProcessType_GetDirectory;
	
	//
	InitByFTPURL(inFTPURL);
	mFTPFolderActionText.SetText(inActionText);
	mFTPFolderDepth = inDepth;
	mFTPFolderMenuObjectID = GetApp().NVI_GetMenuManager().CreateDynamicMenu(kMenuItemID_FTPFolder,false);
	
	//プログレスウインドウのタイトル（フォルダ取得中）設定（ここはまだスレッド外なので直接コールOK）
	AText	text;
	text.SetLocalizedText("FTPProgress_GetDirectory");
	GetApp().SPI_GetFTPProgress().SPNVI_Show(true);
	GetApp().SPI_GetFTPProgress().SPI_SetTitle(text);
	GetApp().SPI_GetFTPProgress().SPI_SetProgressText(mFTPURL);
	
	//スレッド起動
	NVI_Run();
}

/**
フォルダ取得スレッド起動（ファイルリスト用）
*/
void	AFTPConnection::SPNVI_Run_GetDirectory( const AText& inFTPURL )
{
	//スレッド起動中は同じインスタンスでの重複起動不可
	if( NVI_IsThreadWorking() == true )   return;
	
	//ProcessType設定
	mProcessType = kFTPConnectionProcessType_GetDirectory;
	
	//要求データ設定
	InitByFTPURL(inFTPURL);
	mFTPFolderActionText.DeleteAll();
	mFTPFolderDepth = 1;
	mFTPFolderMenuObjectID = kObjectID_Invalid;//ファイルリスト用を示す
	
	//スレッド起動
	NVI_Run();
}

/**
フォルダ取得結果取得
*/
void	AFTPConnection::SPI_GetDirectoryResult( ATextArray& outFileNameArray, ABoolArray& outIsDirectoryArray ) const
{
	outFileNameArray.SetFromTextArray(mDirectoryNameList);
	outIsDirectoryArray.SetFromObject(mIsDirectoryList);
}

//フォルダ取得スレッドメインルーチン
void	AFTPConnection::ThreadMain_GetDirectory()
{
	try
	{
		if( mFTPFolderMenuObjectID == kObjectID_Invalid )
		{
			//==================ファイルリスト用==================
			//ログイン
			Login();
			//ディレクトリ取得
			GetDirectoryRecursive(mPath,1,kObjectID_Invalid);
			//アボートされてなければメインスレッドに完了通知
			if( NVM_IsAborted() == false )
			{
				AObjectIDArray	objectIDArray;
				objectIDArray.Add(mRemoteConnectionObjectID);
				ABase::PostToMainInternalEventQueue(kInternalEvent_FTPDirectoryResult,GetObjectID(),0,GetEmptyText(),objectIDArray);
			}
		}
		else
		{
			//==================FTPフォルダメニュー用==================
			//
			ProgressPercent(0);
			//
			Login();
			//
			GetDirectoryRecursive(mPath,1,mFTPFolderMenuObjectID);
			//
			ProgressPercent(100);
			
			//
			if( NVM_IsAborted() == false )
			{
				//
				AObjectIDArray	objectIDArray;
				objectIDArray.Add(mFTPFolderMenuObjectID);
				ABase::PostToMainInternalEventQueue(kInternalEvent_FTPFolder,GetObjectID(),0,mFTPFolderActionText,objectIDArray);
			}
			//プログレスウインドウを閉じる（イベントを投げる）
			HideProgress();
		}
	}
	catch( AConstCharPtr inErrorString )
	{
		ShowError(inErrorString);
	}
	//ソケットクローズ（既にクローズされている場合もあるが、それはソケットオブジェクト側で判断）
	mControlSocket.Close();
	mDataSocket.Close();
	mListenSocket.Close();
	//セキュリティ上、パスワードをアプリが長期間保持するのは良く無いので忘れる。
	mPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
	mProxyPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
}

//
void	AFTPConnection::GetDirectoryRecursive( const AText& inPath, const ANumber inDepth, const AObjectID inDynamicMenuObjectID )
{
	//スレッド中断チェック
	if( NVM_IsAborted() == true )   return;
	
	AText	path;
	path.SetText(inPath);
	if( path.GetLastChar() != '/' )   path.Add('/');
	
	AText	text;
	ANumber	responceCode;
	
	ProgressText(path);
	
	//#361 ATextArray	mDirectoryNameList;
	//#361 AArray<ABool>	mDirectoryIsDirectoryList;
	
	//結果格納変数初期化
	mDirectoryNameList.DeleteAll();
	mIsDirectoryList.DeleteAll();
	
	// ========= NLST =========
	{
		//データポート接続
		ConnectOrListenDataPort();
		
		//NLST送信
		text.SetCstringAndText("NLST ",path,"\r\n");
		mControlSocket.Send(text);
		_Log(text,true);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode != kDataConnectionAlreadyOpen && responceCode != kFileStatusOK ) 
		{
			//データポート切断
			CloseDataPort();
		}
		else
		{
			//NLSTデータ受信
			if( mPassive == false )   mDataSocket.Accept(mListenSocket);
			AText	text_NLST;
			mDataSocket.ReceiveAll(text_NLST);
			_Log(text_NLST,false);
			
			//データポート切断
			CloseDataPort();
			
			//データ受信完了応答待ち
			Read_WaitFTPResponceCode(text,responceCode);
			_Log(text,false);
			if( responceCode != kClosingDataConnection && responceCode != kConnectionClosed )   _AThrow("FTP NLST data receive failed",NULL);
			
			// NLSTデータ解析
			AIndex	p = 0;
			while( p < text_NLST.GetLength() )
			{
				AText	line;
				text_NLST.GetLine(p,line);
				if( line.GetLength() == 0 )   continue;
				AIndex	pos = line.GetLength()-1;//行の最後から検索
				//'/'か、行頭が見つかるまで戻る
				for( ; pos >= 0; pos-- )
				{
					unsigned char	ch = line.Get(pos);
					if( ch == '/' )   break;
				}
				pos++;
				//戻った位置から最後までをファイル／フォルダ名と見なす
				AText	name;
				line.GetText(pos,line.GetLength()-pos,name);
				mDirectoryNameList.Add(name);
				mIsDirectoryList.Add(false);
			}
		}
	}
	mDirectoryNameList.Sort(true);
	
	// ========= LIST =========
	//ディレクトリかどうかの判定
	{
		//データポート接続
		ConnectOrListenDataPort();
		
		//LIST送信
		text.SetCstringAndText("LIST ",path,"\r\n");
		mControlSocket.Send(text);
		_Log(text,true);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode != kDataConnectionAlreadyOpen && responceCode != kFileStatusOK )   _AThrow("FTP LIST command error",NULL);
		
		//LISTデータ受信
		if( mPassive == false )   mDataSocket.Accept(mListenSocket);
		AText	text_LIST;
		mDataSocket.ReceiveAll(text_LIST);
		_Log(text_LIST,false);
		
		//データポート切断
		CloseDataPort();
		
		//データ受信完了応答待ち
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode != kClosingDataConnection && responceCode != kConnectionClosed )   throw "FTP LIST data receive failed";
		
		//LIST結果解析
		//#874 text_LIST.FindText()に時間がかかるためNLSTとLIST両方を使う場合の条件を追加（1000項目未満）
		if( mDirectoryNameList.GetItemCount() > 0 && mDirectoryNameList.GetItemCount() < 1000 )
		{
			//NLST結果ありの場合、LIST結果からディレクトリか否かの情報のみを抜き出す
			if( text_LIST.GetLength() > 0 )
			{
				for( AIndex i = 0; i < mDirectoryNameList.GetItemCount(); i++ )
				{
					AText	name;
					mDirectoryNameList.Get(i,name);
					for( AIndex pos = 0; pos < text_LIST.GetItemCount();  )
					{
						AIndex	foundpos;
						if( text_LIST.FindText(pos,name,foundpos) == false )   break;
						pos = foundpos + name.GetItemCount();
						
						AIndex	p = foundpos + name.GetItemCount();
						ABool	notlast = false;
						for( ; p < text_LIST.GetItemCount(); p++ )
						{
							AUChar	ch = text_LIST.Get(p);
							if( ch == kUChar_CR || ch == kUChar_LF )  break;
							else if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
							else
							{
								notlast = true;
								break;
							}
						}
						if( notlast == true )   continue;
						p = foundpos;
						for( ; p >= 0; p-- )
						{
							AUChar	ch = text_LIST.Get(p);
							if( ch == kUChar_CR || ch == kUChar_LF )   break;
						}
						p++;
						AUChar	ch = text_LIST.Get(p);
						if( ch == 'd' )
						{
							mIsDirectoryList.Set(i,true);
						}
						break;
					}
				}
			}
		}
		else
		{
			//NLST結果なしの場合、LIST結果から、ファイル／フォルダ名、および、ディレクトリか否かの情報を抜き出す
			AIndex	p = 0;
			while( p < text_LIST.GetLength() )
			{
				AText	line;
				text_LIST.GetLine(p,line);
				if( line.GetLength() < 15 )   continue;
				unsigned char	ch1 = line.Get(0);
				if( ( ch1 == 'd' || ch1 == '-' ) == false )   continue;
				AText	name;
				AIndex	pos = line.GetLength()-1;//行の最後から検索
				//最後のスペース／タブを飛ばす
				for( ; pos >= 0; pos-- )
				{
					unsigned char	ch =  line.Get(pos);
					if( ch != A_CHAR_SPACE && ch != A_CHAR_TAB )   break;
				}
				//スペース／タブが見つかるまでnameに格納
				for( ; pos >= 0; pos-- )
				{
					unsigned char	ch =  line.Get(pos);
					if( ch == A_CHAR_SPACE || ch == A_CHAR_TAB )   break;
					name.Insert1(0,ch);
				}
				if( name.GetLength() == 0 )   continue;
				if( name.Compare(".") == true )   continue;
				if( name.Compare("..") == true )   continue;
				mDirectoryNameList.Add(name);
				mIsDirectoryList.Add(ch1=='d');
			}
		}
	}
	
	/*#193
	//==================FTPフォルダメニューの場合==================
	//メニューデータ構成
	if( inDynamicMenuObjectID != kObjectID_Invalid )
	{
		//メニューデータ構成
		for( AIndex i = 0; i < mDirectoryNameList.GetItemCount(); i++ )
		{
			if( inDepth == 1 )
			{
				ProgressPercent((i+1)*100/mDirectoryNameList.GetItemCount());
			}
			AText	name;
			mDirectoryNameList.Get(i,name);
			if( name.GetLength() == 0 )   continue;
			if( name.Compare(".") == true )   continue;
			if( name.Compare("..") == true )   continue;
			AText	filepath;
			filepath.SetText(path);
			filepath.AddText(name);
			if( mIsDirectoryList.Get(i) == true )//ディレクトリ
			{
				if( inDepth+1 <= mFTPFolderDepth )
				{
					AObjectID	dynamicMenuObjectID = GetApp().NVI_GetMenuManager().CreateDynamicMenu(kMenuItemID_FTPFolder,false);
					GetDirectoryRecursive(filepath,inDepth+1,dynamicMenuObjectID);
					GetApp().NVI_GetMenuManager().AddDynamicMenuItemByObjectID(inDynamicMenuObjectID,name,name,dynamicMenuObjectID,0,0,false);
				}
				else
				{
					GetApp().NVI_GetMenuManager().AddDynamicMenuItemByObjectID(inDynamicMenuObjectID,name,filepath,kObjectID_Invalid,0,0,false);
				}
			}
			else
			{
				AText	url;
				url.SetFTPURL(mServer,mUser,filepath);
				GetApp().NVI_GetMenuManager().AddDynamicMenuItemByObjectID(inDynamicMenuObjectID,name,url,kObjectID_Invalid,0,0,false);
			}
		}
		//separator
		GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByObjectID(inDynamicMenuObjectID,0,GetEmptyText(),GetEmptyText(),kObjectID_Invalid,0,0,true);
		//新規ファイル
		AText	menutext;
		menutext.SetLocalizedText("FTPFolder_NewFile");
		AText	action;
		action.SetFTPURL(mServer,mUser,path);
		action.Insert1(0,'N');
		GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByObjectID(inDynamicMenuObjectID,0,menutext,action,kObjectID_Invalid,0,0,false);
		//フォルダ更新
		if( inDepth == 1 )
		{
			AText	menutext;
			menutext.SetLocalizedText("FTPFolder_Update");
			AText	action;
			action.SetText(mFTPFolderActionText);
			action.Set(0,'U');
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByObjectID(inDynamicMenuObjectID,0,menutext,action,kObjectID_Invalid,0,0,false);
		}
	}
	*/
	
	// LIST結果解析
	//例：
	//drwxr-xr-x  63 root     wheel         2142 Oct  7 11:30 sbin
	//-rw-r--r--   1 daisuke  admin        42270 Feb 10  2006 test_orig.html
	//lrwxr-xr-x   1 root     admin           11 Jan  5  2006 tmp -> private/tmp

	/*AIndex	p = 0;
	while( p < text_LIST.GetLength() )
	{
		AText	line;
		text_LIST.GetLine(p,line);
		if( inDepth == 1 )
		{
			ProgressPercent(p*100/text_LIST.GetLength()-1);
		}
		if( line.GetLength() < 15 )   continue;
		unsigned char	ch1 = line.Get(0);
		if( not( ch1 == 'd' || ch1 == '-' ) )   continue;
		AText	name;
		AIndex	pos = line.GetLength()-1;//行の最後から検索
		//最後のスペース／タブを飛ばす
		for( ; pos >= 0; pos-- )
		{
			unsigned char	ch =  line.Get(pos);
			if( ch != A_CHAR_SPACE && ch != A_CHAR_TAB )   break;
		}
		//スペース／タブが見つかるまでnameに格納
		for( ; pos >= 0; pos-- )
		{
			unsigned char	ch =  line.Get(pos);
			if( ch == A_CHAR_SPACE || ch == A_CHAR_TAB )   break;
			name.Insert(0,&ch,1);
		}
		if( name.GetLength() == 0 )   continue;
		if( name.Compare(".") == true )   continue;
		if( name.Compare("..") == true )   continue;
		AText	filepath;
		filepath.SetText(path);
		filepath.AddText(name);
		if( ch1 == 'd' )//ディレクトリ
		{
			if( inDepth+1 <= mFTPFolderDepth )
			{
				ADynamicMenu*	menu = new ADynamicMenu(kMenuItemID_FTPFolderItem);
				GetDirectoryRecursive(filepath,menu,inDepth+1);
				ioMenu->Add(name,kEmptyText,menu,true);
			}
			else
			{
				ioMenu->Add(name,filepath,NULL,false);
			}
		}
		else if( ch1 == '-' )//ファイル
		{
			AText	url;
			url.SetFTPURL(mServer,mUser,filepath);
			ioMenu->Add(name,url,NULL,true);
		}
	}*/
}

#pragma mark ===========================

#pragma mark --- ファイル取得

//
void	AFTPConnection::SPNVI_Run_GetFile( const AText& inFTPURL, const ABool inAllowNoFile )
{
	//スレッド起動中は同じインスタンスでの重複起動不可
	if( NVI_IsThreadWorking() == true )   return;
	
	//
	mGetFileAllowNoFile = inAllowNoFile;
	
	//ProcessType設定
	mProcessType = kFTPConnectionProcessType_GetFile;
	
	InitByFTPURL(inFTPURL);
	//#361 mGetFileReceiverObjectID = inReceiverObjectID;
	
	//プログレスウインドウのタイトル（ファイル取得中）設定（ここはまだスレッド外なので直接コールOK）
	AText	text;
	text.SetLocalizedText("FTPProgress_GetFile");
	GetApp().SPI_GetFTPProgress().SPNVI_Show(false);
	GetApp().SPI_GetFTPProgress().SPI_SetTitle(text);
	GetApp().SPI_GetFTPProgress().SPI_SetProgressText(mFTPURL);
	
	//スレッド起動
	NVI_Run();
}

void	AFTPConnection::ThreadMain_GetFile()
{
	try
	{
		//
		ProgressPercent(-1);
		
		Login();
		
		AText	text;
		ANumber	responceCode;
		
		//TYPE送信
		if( mBinaryMode )
		{
			text.SetCstring("TYPE I\r\n");
		}
		else
		{
			text.SetCstring("TYPE A\r\n");
		}
		mControlSocket.Send(text);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,true);
		
		//データポート接続
		ConnectOrListenDataPort();
		
		//RETR送信
		text.SetCstring("RETR ");
		text.AddText(mPath);
		text.AddCstring("\r\n");
		mControlSocket.Send(text);
		_Log(text,true);
		Read_WaitFTPResponceCode(text,responceCode);
		ABool	fileExist = true;
		if( responceCode != kDataConnectionAlreadyOpen && responceCode != kFileStatusOK )
		{
			if( mGetFileAllowNoFile == true )
			{
				fileExist = false;
			}
			else
			{
				_AThrow("FTP RETR command error",NULL);
			}
		}
		_Log(text,false);
		
		if( fileExist == true )
		{
			//RETRデータ受信
			if( mPassive == false )   mDataSocket.Accept(mListenSocket);
			mDataSocket.ReceiveAll(mReceivedText);//#361 GetApp().SPI_GetThreadDataReceiver_Text(mGetFileReceiverObjectID));
			
			//データポート切断
			CloseDataPort();
			
			//データ受信完了応答待ち
			Read_WaitFTPResponceCode(text,responceCode);
			if( responceCode/100 != 2 )   _AThrow("FTP RETR data receive failed",NULL);
			_Log(text,false);
		}
		
		//アボートされてなければメインスレッドに完了通知
		if( NVM_IsAborted() == false )
		{
			AObjectIDArray	objectIDArray;
			//#361 objectIDArray.Add(mGetFileReceiverObjectID);
			objectIDArray.Add(mRemoteConnectionObjectID);//#361
			//#193 AText	url;
			//#193 url.SetFTPURL(mServer,mUser,mPath,mPort);//#193
			ABase::PostToMainInternalEventQueue(kInternalEvent_FTPOpen,GetObjectID(),0,/*#361 url*/mReceivedText,objectIDArray);
		}
		
		//プログレスウインドウを閉じる（イベントを投げる）
		HideProgress();
	}
	catch( AConstCharPtr inErrorString )
	{
		ShowError(inErrorString);
	}
	//ソケットクローズ（既にクローズされている場合もあるが、それはソケットオブジェクト側で判断）
	mControlSocket.Close();
	mDataSocket.Close();
	mListenSocket.Close();
	//セキュリティ上、パスワードをアプリが長期間保持するのは良く無いので忘れる。
	mPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
	mProxyPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
}

#pragma mark ===========================

#pragma mark --- ファイル保存

//ファイル保存スレッド起動
void	AFTPConnection::SPNVI_Run_PutFile( const AText& inFTPURL, const AText& inData )
{
	mLastPutFileOK = false;
	
	//スレッド起動中は同じインスタンスでの重複起動不可
	if( NVI_IsThreadWorking() == true )   return;
	
	//ProcessType設定
	mProcessType = kFTPConnectionProcessType_PutFile;
	
	//送信データを内部バッファにコピー（スレッドで実行するのでコピーが必要）
	mSendData.SetText(inData);
	//
	AText	text;
	text.SetLocalizedText("FTPProgress_PutFile");
	GetApp().SPI_GetFTPProgress().SPNVI_Show(false);
	GetApp().SPI_GetFTPProgress().SPI_SetTitle(text);
	GetApp().SPI_GetFTPProgress().SPI_SetProgressText(mFTPURL);
	//
	InitByFTPURL(inFTPURL);
	//スレッド起動
	//NVI_Run();
	//スレッドは使わず、同期実行する
	ThreadMain_PutFile();
}

//ファイル保存スレッドメインルーチン
void	AFTPConnection::ThreadMain_PutFile()
{
	try
	{
		ProgressPercent(-1);
		
		Login();
		
		AText	text;
		ANumber	responceCode;
		
		//TYPE送信
		if( mBinaryMode )
		{
			text.SetCstring("TYPE I\r\n");
		}
		else
		{
			text.SetCstring("TYPE A\r\n");
		}
		mControlSocket.Send(text);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,true);
		
		//データポート接続
		ConnectOrListenDataPort();
		
		//STOR送信
		text.SetCstring("STOR ");
		text.AddText(mPath);
		text.AddCstring("\r\n");
		mControlSocket.Send(text);
		_Log(text,true);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode != kDataConnectionAlreadyOpen && responceCode != kFileStatusOK )   _AThrow("FTP STOR command error",NULL);
		
		//RETRデータ受信
		if( mPassive == false )   mDataSocket.Accept(mListenSocket);
		mDataSocket.Send(mSendData);
		
		//データポート切断
		CloseDataPort();
		
		//データ送信応答待ち
		Read_WaitFTPResponceCode(text,responceCode);
		if( responceCode/100 != 2 )   _AThrow("FTP STOR data receive failed",NULL);
		_Log(text,false);
		
		HideProgress();
		mLastPutFileOK = true;
	}
	catch( AConstCharPtr inErrorString )
	{
		ShowError(inErrorString);
	}
	//ソケットクローズ（既にクローズされている場合もあるが、それはソケットオブジェクト側で判断）
	mControlSocket.Close();
	mDataSocket.Close();
	mListenSocket.Close();
	//セキュリティ上、パスワードをアプリが長期間保持するのは良く無いので忘れる。
	mPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
	mProxyPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
}

#pragma mark ===========================

#pragma mark ---共通処理

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン（ディスパッチ）

//スレッドメインルーチン（各処理スレッドメインルーチンへのディスパッチのみ）
void	AFTPConnection::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AFTPConnection::NVIDO_ThreadMain started.",this);
	switch(mProcessType)
	{
	  case kFTPConnectionProcessType_GetDirectory:
		{
			ThreadMain_GetDirectory();
			break;
		}
	  case kFTPConnectionProcessType_GetFile:
		{
			ThreadMain_GetFile();
			break;
		}
	  case kFTPConnectionProcessType_PutFile:
		{
			ThreadMain_PutFile();
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AFTPConnection::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark --- コントロールポート制御

//制御ポート接続
void	AFTPConnection::ConnectControlPort( const AText& inServer, const APortNumber inPort )
{
	AText	text;
	ANumber	responceCode;
	
	mControlSocket.Connect(inServer,inPort);
	Read_WaitFTPResponceCode(text,responceCode);
	_Log(text,false);
	if( responceCode != kServiceReady )   _AThrow("FTP service not ready",NULL);
}

//FTP応答コードを受信するまでデータを受信し、outTextに格納する
//FTP応答コードを受信するまではブロックする
void	AFTPConnection::Read_WaitFTPResponceCode( AText& outText, ANumber& outResponceCode )
{
	outText.DeleteAll();
	outResponceCode = 0;
	
	while(true)
	{
		//受信キュー上にあるデータが空になるまで、一行ごとに、有効なFTP応答コードの有無を調べる。
		//有れば、その時点でデキューをやめ、リターンする。
		//無ければ、ソケットからの受信待ちブロックに入る。
		AText	line;
		while( DequeueLine(line) == true )
		{
			outText.AddText(line);
			//FTP応答コードは行頭＋3桁の数字＋スペース (RFC959)
			if( line.GetItemCount() >= 4 )
			{
				char	ch1, ch2, ch3, ch4;
				ch1 = line.Get(0);
				ch2 = line.Get(1);
				ch3 = line.Get(2);
				ch4 = line.Get(3);
				if( ch1>='0' && ch1<='9' && 
						ch2>='0' && ch2<='9' && 
						ch3>='0' && ch3<='9' && 
						ch4 == ' ' )
				{
					outResponceCode = (ch1-'0')*100 + (ch2-'0')*10 + (ch3-'0');
					return;
				}
			}
		}
		//受信
		ABool	disconnected = false;
		mControlSocket.Receive(mControlSocketReadQueue,disconnected);
		if( disconnected == true )   _ACThrow("control socket disconnected",this);
	}
}

//ControlSocketReadQueueから１行読みだす
//（改行コードはCR, CRLF, LFどれでも正常動作する）
ABool	AFTPConnection::DequeueLine( AText& outText )
{
	outText.DeleteAll();
	for( AIndex pos = 0; pos < mControlSocketReadQueue.GetLength(); pos++ )
	{
		AUChar ch1 = mControlSocketReadQueue.Get(pos);
		if( ch1 == kUChar_CR || ch1 == kUChar_LF )
		{
			pos++;
			if( pos < mControlSocketReadQueue.GetItemCount() )
			{
				AUChar ch2 = mControlSocketReadQueue.Get(pos);
				if( ch1 == kUChar_CR && ch2 == kUChar_LF )
				{
					pos++;
				}
			}
			mControlSocketReadQueue.GetText(0,pos,outText);
			mControlSocketReadQueue.Delete(0,pos);
			return true;
		}
	}
	return false;
}


//USER送信
void	AFTPConnection::SendUSER( const AText& inUser )
{
	AText	text;
	ANumber	responceCode;
	
	text.SetCstringAndText("USER ",inUser,"\r\n");
	mControlSocket.Send(text);
	_Log(text,true);
	Read_WaitFTPResponceCode(text,responceCode);
	_Log(text,false);
	if( responceCode != kUserNameOK )   _AThrow("FTP user name invalid",NULL);
}

//PASS送信
void	AFTPConnection::SendPASS( const AText& inPassword )
{
	AText	text;
	ANumber	responceCode;
	
	text.SetCstringAndText("PASS ",inPassword,"\r\n");
	mControlSocket.Send(text);
	Read_WaitFTPResponceCode(text,responceCode);
	//_Log(text);パスワードは表示しない
	if( responceCode != kUserLoggedIn )   _AThrow("FTP password incorrect",NULL);
	text.SetCstring("PASS ********\r\n230 \r\n");
	_Log(text,false);
}

//SITE送信
void	AFTPConnection::SendSITE( const AText& inSite )
{
	AText	text;
	ANumber	responceCode;
	
	text.SetCstringAndText("SITE ",inSite,"\r\n");
	mControlSocket.Send(text);
	_Log(text,true);
	Read_WaitFTPResponceCode(text,responceCode);
	_Log(text,false);
	//if( responceCode !=  )   _AThrow("SITE failed");
}

//ログイン
void	AFTPConnection::Login()
{
	AText	text;
	
	switch(mProxyMode)
	{
	  case AAppPrefDB::kProxyMode_NoProxy:
		{
			ConnectControlPort(mServer,mPort);
			SendUSER(mUser);
			SendPASS(mPassword);
			break;
		}
	  case AAppPrefDB::kProxyMode_FTPProxy1://USER remoteuser@remotehost->PASS remotepass
		{
			ConnectControlPort(mProxyServer,mPort);
			
			//remoteuser@remotehost
			AText	usertext;
			usertext.SetText(mUser);
			usertext.AddCstring("@");
			usertext.AddText(mServer);
			SendUSER(usertext);
			
			SendPASS(mPassword);
			break;
		}
	  case AAppPrefDB::kProxyMode_FTPProxy2://USER proxyuser->PASS proxypass->SITE remotehost->USER remoteuser->PASS remotepass
		{//未テスト
			ConnectControlPort(mProxyServer,mPort);
			SendUSER(mProxyUser);
			SendPASS(mProxyPassword);
			SendSITE(mServer);
			SendPASS(mPassword);
			break;
		}
	}
}

//FTPURLから情報を切り出し、メンバ変数へ初期設定
void	AFTPConnection::InitByFTPURL( const AText& inFTPURL )
{
	mFTPURL.SetText(inFTPURL);
	inFTPURL.GetFTPURLServer(mServer);
	inFTPURL.GetFTPURLUser(mUser);
	inFTPURL.GetFTPURLPath(mPath);
	/*#1231 ポート番号はFTPアカウント設定から取得
	//#193
	mPort = inFTPURL.GetFTPURLPortNumber();
	if( mPort == 0 )
	{
		mPort = 21;
	}
	*/
	//
	mPassword.DeleteAll();
#if IMPLEMENTATION_FOR_MACOSX
	ASecurityWrapper::GetInternetPassword(mServer,mUser,mPassword);
#endif
	
	AIndex	index = GetApp().GetAppPref().FindFTPAccountPrefIndex(mServer,mUser);
	if( index != kIndex_Invalid )
	{
		//FTPアカウント設定から各種情報取得
		//#193 mPort = static_cast<APortNumber>(GetApp().GetAppPref().GetData_NumberArray(AAppPrefDB::kFTPAccount_Port,index));
		//#1231
		mPort = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFTPAccount_Port,index);
		if( GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kFTPAccount_UseDefaultPort,index) == true )
		{
			mPort = 21;
		}
		//
		mPassive = GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_Passive,index);
		mProxyMode = GetApp().GetAppPref().GetData_NumberArray(AAppPrefDB::kFTPAccount_ProxyMode,index);
		if( mProxyMode != AAppPrefDB::kProxyMode_NoProxy )
		{
			GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFTPAccount_ProxyServer,index,mProxyServer);
			GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFTPAccount_ProxyUser,index,mProxyUser);
			ASecurityWrapper::GetInternetPassword(mProxyServer,mProxyUser,mProxyPassword);
		}
		mBinaryMode = GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_BinaryMode,index);
#if IMPLEMENTATION_FOR_WINDOWS
		GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFTPAccount_Password,index,mPassword);
#endif
	}
	else
	{
		//#193 mPort = 21;
		mPort = 21;//#1231
		mPassive = true;
		mProxyMode = AAppPrefDB::kProxyMode_NoProxy;
		mBinaryMode = false;
#if IMPLEMENTATION_FOR_WINDOWS
		mPassword.DeleteAll();
#endif
		_AError("FTP account data do not exist",this);
	}
}

#pragma mark ===========================

#pragma mark --- データポート制御

//接続、または、Listen開始
void	AFTPConnection::ConnectOrListenDataPort()
{
	AText	text;
	ANumber	responceCode;
	
	//PASVモード（＝こちら側がアクティブ接続になる）
	if( mPassive == true )
	{
		//PASVコマンド
		text.SetCstring("PASV\r\n");
		mControlSocket.Send(text);
		_Log(text,true);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode !=kEnteringPassiveMode )   _AThrow("FTP server cannot enter passive mode",NULL);
		
		//データコネクションのサーバーアドレス、ポート取得
		AIndex	pos = text.GetLength()-1;
		AText	token;
		text.GoBackToChar(pos,',');
		pos++;
		text.GetToken(pos,token);
		// pos位置(*)：h1,h2,h3,h4,p1,p2(*)
		//ここからさかのぼってトークン解析
		
		//ポート番号
		APortNumber	remote_port;
		
		text.GoBackGetToken(pos,token);
		// pos位置(*)：h1,h2,h3,h4,p1,(*)p2
		remote_port = static_cast<APortNumber>(token.GetNumber());
		
		text.GoBackGetToken(pos,token);
		// pos位置(*)：h1,h2,h3,h4,p1(*),p2
		text.GoBackGetToken(pos,token);
		// pos位置(*)：h1,h2,h3,h4,(*)p1,p2
		remote_port += static_cast<APortNumber>(token.GetNumber()*0x100);
		
		//IPアドレス
		AIPAddress	remote_ipaddress;
		
		//h4
		text.GoBackGetToken(pos,token);
		text.GoBackGetToken(pos,token);
		remote_ipaddress = token.GetNumber();
		//h3
		text.GoBackGetToken(pos,token);
		text.GoBackGetToken(pos,token);
		remote_ipaddress += token.GetNumber() *0x100;
		//h2
		text.GoBackGetToken(pos,token);
		text.GoBackGetToken(pos,token);
		remote_ipaddress += token.GetNumber() *0x10000;
		//h1
		text.GoBackGetToken(pos,token);
		text.GoBackGetToken(pos,token);
		remote_ipaddress += token.GetNumber() *0x1000000;
		
		//データコネクションConnect
		mDataSocket.Connect(remote_ipaddress,remote_port);
	}
	else
	{
		//データコネクションListen
		mListenSocket.EphemeralListen(1);
		
		//PORTコマンド
		AIPAddress	local_ipaddress;
		APortNumber	local_port;
		mListenSocket.GetLocalhostAddress(local_ipaddress,local_port);
		APortNumber	tmp;
		mControlSocket.GetLocalhostAddress(local_ipaddress,tmp);
		text.SetFormattedCstring("PORT %u,%u,%u,%u,%u,%u\r\n",
				(local_ipaddress&0xFF000000)/0x1000000,
				(local_ipaddress&0xFF0000)/0x10000,
				(local_ipaddress&0xFF00)/0x100,
				(local_ipaddress&0xFF),
				(local_port&0xFF00)/0x100,
				(local_port&0xFF));
		mControlSocket.Send(text);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode != kCommandOK )   _AThrow("FTP PORT comamnd error",NULL);
	}
}

//
void	AFTPConnection::CloseDataPort()
{
	mDataSocket.Close();
	mListenSocket.Close();
}

#pragma mark ===========================

#pragma mark ---プログレス表示イベント送信

void	AFTPConnection::HideProgress()
{
	ABase::PostToMainInternalEventQueue(kInternalEvent_FTPProgress_Hide,GetObjectID(),0,GetEmptyText(),GetEmptyObjectIDArray());
}

void	AFTPConnection::_Log( const AText& inText, const ABool inSend )
{
	AText	h;
	if( inSend == true )
	{
		h.SetCstring("(S) ");
	}
	else
	{
		h.SetCstring("(R) ");
	}
	AText	text;
	for( AIndex pos = 0; pos < inText.GetItemCount();  )
	{
		AText	line;
		inText.GetLine(pos,line);
		text.AddText(h);
		text.AddText(line);
		text.Add(kUChar_LineEnd);
	}
	//text.OutputToStderr();
	ABase::PostToMainInternalEventQueue(kInternalEvent_FTPLog,GetObjectID(),0,text,GetEmptyObjectIDArray());
}

void	AFTPConnection::ProgressPercent( ANumber inPercent )
{
	ABase::PostToMainInternalEventQueue(kInternalEvent_FTPProgress_SetProgress,GetObjectID(),inPercent,GetEmptyText(),GetEmptyObjectIDArray());
}

void	AFTPConnection::ProgressText( AText& inText )
{
	ABase::PostToMainInternalEventQueue(kInternalEvent_FTPProgress_SetProgressText,GetObjectID(),0,inText,GetEmptyObjectIDArray());
}

void	AFTPConnection::ShowError( AConstCharPtr inErrorString )
{
	AText	text;
	text.SetFormattedCstring("%s   errno:%d",inErrorString,errno);
	ABase::PostToMainInternalEventQueue(kInternalEvent_FTPProgress_Error,GetObjectID(),0,text,GetEmptyObjectIDArray());
}


