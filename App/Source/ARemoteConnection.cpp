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

ARemoteConnection
#361

*/

#include "stdafx.h"

#include "ARemoteConnection.h"
#include "AApp.h"
#if IMPLEMENTATION_FOR_MACOSX
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#endif
#include "AFTP.h"


#pragma mark ===========================
#pragma mark [クラス]ARemoteConnection
#pragma mark ===========================
/*
リモート接続オブジェクト
・ファイルリストウインドウに対して１つ、または、リモートファイルのドキュメントに対して１つ存在し、リモートファイルのデータ取得、保存を行う
・プラグイン接続→要求→応答受信（同時に常にプラグイン接続解除される）の流れ
・要求は常に１つのみ。応答が変えるまでは、他の要求はできない（無視される）。
*/

/*
2018/2/3 リモートアクセス対応
SFTPは、expectスクリプトで対応する。
理由：
・curlは、known hostかどうかの判定の仕様がSFTP同等にできない。man-in-the-middle対策が不十分になる。
・Mac OS X標準のcurlではSFTP対応していないので、自分でビルドしたものを使うしかないが、環境によって動かないことがあり、検証しきれない。
ダイナミックライブラリは全部アプリに添付して、otoolでダイナミックライブラリ参照先をアプリ内ライブラリにすれば良かったが、
Mac OS X 10.7以前でも動作するようにしきれなかった。また、MBPでビルドしたcurlがAirでillegal instructionになるなど、まだわからない点がある。
*/

#pragma mark ---コンストラクタ／デストラクタ

/**
コンストラクタ
*/
ARemoteConnection::ARemoteConnection( const ARemoteConnectionType inType, const AObjectID inRequestorObjectID )
: AObjectArrayItem(NULL), mAccessPluginConnectionType(inType), mRequestorObjectID(inRequestorObjectID), 
		mCurrentRequestType(kRemoteConnectionRequestType_None),
		/*#1231 mLegacyFTPMode(true), */mFTPConnectionObjectID(kObjectID_Invalid)
		,mRemoteConnectionProtocolType(kRemoteConnectionProtocolType_AccessPlugin)//#1231
,mPort(0),mPassive(false),mBinaryMode(false),mUsePrivateKey(false),mDontCheckServerCert(false),mAllowNoFile(false)//#1231
{
}

/**
デストラクタ
*/
ARemoteConnection::~ARemoteConnection()
{
}

/**
レガシーFTP接続オブジェクト取得
*/
AFTPConnection&	ARemoteConnection::GetFTPConnection()
{
	//FTP接続オブジェクト未生成なら生成
	if( mFTPConnectionObjectID == kObjectID_Invalid )
	{
		AFTPConnectionFactory	factory(this);
		mFTPConnectionObjectID = GetApp().NVI_CreateThread(factory);
		GetFTPConnection().SPI_SetRemoteConnectionObjectID(GetObjectID());
	}
	return dynamic_cast<AFTPConnection&>(GetApp().NVI_GetThreadByID(mFTPConnectionObjectID));
}

#pragma mark ===========================

#pragma mark ---接続／切断

//#1231
/**
接続情報設定
（サーバー・ユーザー名から対応するFTPアカウントを検索して取得）
*/
ABool	ARemoteConnection::SetConnectionProperty( const AText& inRemoteURL )
{
	//server, userからFTPアカウントを検索
	AText	server, user;
	inRemoteURL.GetFTPURLServer(server);
	inRemoteURL.GetFTPURLUser(user);
	AIndex	index = GetApp().GetAppPref().FindFTPAccountPrefIndex(server,user);
	if( index != kIndex_Invalid )
	{
		//FTPアカウント情報取得
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFTPAccount_ConnectionType,index,mConnectionType);
		mPort = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFTPAccount_Port,index);
		if( GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kFTPAccount_UseDefaultPort,index) == true )
		{
			mPort = 0;
		}
		mPassive = GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_Passive,index);
		mBinaryMode = GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_BinaryMode,index);
		mUsePrivateKey = GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_UsePrivateKey,index);
		GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFTPAccount_PrivateKeyFilePath,index,mPrivateKeyFilePath);
		mDontCheckServerCert = GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_DontCheckServerCert,index);
		
		//プラグイン検索
		AText	pluginFilePath;
		if( GetApp().SPI_GetPluginPath(mConnectionType,pluginFilePath) == true )
		{
			//プラグインが見つかった場合はそれを使用
			mRemoteConnectionProtocolType = kRemoteConnectionProtocolType_AccessPlugin;//#1231
			mPluginFile.Specify(pluginFilePath);
			return true;
		}
		else
		{
			//FTP
			if( mConnectionType.Compare("FTP") == true )
			{
				mRemoteConnectionProtocolType = kRemoteConnectionProtocolType_FTP;//#1231
				return true;
			}
		}
	}
	return false;
}

/**
接続タイプ設定
*/
/*#1231
ABool	ARemoteConnection::SetConnectionType( const AText& inConnectionType )
{
	//コネクションタイプからプラグインのファイルパスを取得
	AText	pluginFilePath;
	if( GetApp().SPI_GetPluginPath(inConnectionType,pluginFilePath) == true )
	{
		if( pluginFilePath.GetItemCount() == 0 )
		{
			mLegacyFTPMode = true;
		}
		else
		{
			mLegacyFTPMode = false;
			mPluginFile.Specify(pluginFilePath);
		}
		return true;
	}
	else
	{
		if( inConnectionType.Compare("FTP") == true )
		{
			mLegacyFTPMode = true;
			return true;
		}
		return false;
	}
}
*/

/**
プラグイン実行
*/
ABool	ARemoteConnection::ExecutePlugin( const AConstCharPtr inPluginType )
{ 
	//処理タイプ
	AText	type(inPluginType);
	//リモートファイルURLからサーバー名等切り出し
	AText	server, user, path, password;
	mRemoteURL.GetFTPURLServer(server);
	mRemoteURL.GetFTPURLUser(user);
	mRemoteURL.GetFTPURLPath(path);
	AText	porttext;
	porttext.SetNumber(mPort);//#1231 inRemoteURL.GetFTPURLPortNumber());
	//パスワード取得
	password.SetCstring(" ");
#if IMPLEMENTATION_FOR_MACOSX
	 ASecurityWrapper::GetInternetPassword(server,user,password);
	if( password.GetItemCount() == 0 )
	{
		password.SetCstring(" ");
	}
#endif
	
	//プラグインパス取得
	AText	pluginPath;
	mPluginFile.GetPath(pluginPath);
	
	//プラグイン実行
	AText	directoryPath;
	ATextArray	argTextArray;
	argTextArray.Add(pluginPath);
	argTextArray.Add(type);
	argTextArray.Add(server);
	argTextArray.Add(porttext);
	argTextArray.Add(user);
	argTextArray.Add(password);
	argTextArray.Add(path);
	argTextArray.Add(mLocalFilePath);
	//SFTP専用処理
	if( mConnectionType.Compare("SFTP") == true )
	{
		//秘密鍵パス
		AText	keyfilePath(" ");
		if( mUsePrivateKey == true )
		{
			keyfilePath.SetText(mPrivateKeyFilePath);
		}
		argTextArray.Add(keyfilePath);
		//フィンガープリント
		AText	fingerprint(" ");
		if( mFingerprint.GetItemCount() > 0 )
		{
			fingerprint.SetText(mFingerprint);
		}
		argTextArray.Add(fingerprint);
	}
	//FTPS専用処理
	else if( mConnectionType.Compare("FTPS") == true )
	{
		//サーバー証明書チェックオプション
		if( mDontCheckServerCert == true )
		{
			AText	ignoreServerCert("-k");
			argTextArray.Add(ignoreServerCert);
		}
	}
	//inLocalTemporaryFilePath.OutputToStderr();
	GetApp().SPI_GetChildProcessManager().ExecuteAccessPlugin(pluginPath,argTextArray,directoryPath,GetObjectID());
	return true;
}

#pragma mark ===========================

#pragma mark ---各コマンド要求

/**
LISTコマンド要求
*/
ABool	ARemoteConnection::ExecuteLIST( const ARemoteConnectionRequestType inRequestType, const AText& inRemoteFolderURL, const AText& inFingerprint )//#1231
{
	//他のコマンド要求中は現在の要求をアボートし、しばらく待って（＝スレッド終了待ち）から今回の要求を実行する
	if( mCurrentRequestType != kRemoteConnectionRequestType_None )
	{
		AbortCurrentRequest();
		GetApp().NVI_SleepWithTimer(1);
		//スレッドが終了仕切らなかった場合、SPNVI_Run_GetDirectory()でスレッド起動されずに終了する。
		//この場合、再度ExecuteLIST()実行時に、もう一度ここが実行される。そのときにスレッドが終了していれば、そのときには成功するし、
		//スレッドが終了しなくなってしまっていても、このリモート接続でコマンド実行できない以上の問題にはならない。
	}
	
	//接続情報設定（サーバー・ユーザー名から対応するFTPアカウントを検索して取得） #1231
	if( SetConnectionProperty(inRemoteFolderURL) == false )
	{
		return false;
	}
	
	//ログ出力 #1231
	AText	log("Connecting ");
	log.AddText(inRemoteFolderURL);
	log.AddCstring("...(LIST)\n");
	GetApp().SPI_GetFTPLogWindow().SPI_AddLog(log);
	
	//要求タイプ記憶
	mCurrentRequestType = inRequestType;
	//リモートURL記憶 #1231
	mRemoteURL.SetText(inRemoteFolderURL);
	//フィンガープリント記憶 #1231
	mFingerprint.SetText(inFingerprint);
	
	// #1231 if( mLegacyFTPMode == true )
	switch(mRemoteConnectionProtocolType)
	{
	  case kRemoteConnectionProtocolType_FTP:
		{
			//==================レガシーFTP接続時==================
			//FTPスレッドRun（ディレクトリ取得）
			GetFTPConnection().SPNVI_Run_GetDirectory(inRemoteFolderURL);
			break;
		}
		//
	  case kRemoteConnectionProtocolType_AccessPlugin:
	  default:
		{
			//==================プラグイン接続時==================
			//リスト取得先テンポラリファイル（一応設定するが使わない）
			AFileAcc	tmpFolder;
			AFileWrapper::GetTempFolder(tmpFolder);
			AFileAcc	tmpFile;
			tmpFile.SpecifyUniqChildFile(tmpFolder,"AccessPluginLocalTempFileForLIST");
			//#1231 tmpFile.CreateFile();
			tmpFile.GetPath(mLocalFilePath);
			//プラグイン実行
			ExecutePlugin("list");
			break;
		}
	}
	return true;
}

/**
GETコマンド要求
*/
ABool	ARemoteConnection::ExecuteGET( const ARemoteConnectionRequestType inRequestType, const AText& inRemoteFileURL, const ABool inAllowNoFile )
{
	//他のコマンド要求中は何もせず終了（今回の要求は無視される）
	if( mCurrentRequestType != kRemoteConnectionRequestType_None )
	{
		return false;
	}
	
	//接続情報設定（サーバー・ユーザー名から対応するFTPアカウントを検索して取得） #1231
	if( SetConnectionProperty(inRemoteFileURL) == false )
	{
		return false;
	}
	
	//ログ出力 #1231
	AText	log("Connecting ");
	log.AddText(inRemoteFileURL);
	log.AddCstring("...(GET)\n");
	GetApp().SPI_GetFTPLogWindow().SPI_AddLog(log);
	
	//要求タイプ記憶
	mCurrentRequestType = inRequestType;
	//リモートファイルパス記憶
	mRemoteURL.SetText(inRemoteFileURL);
	//フィンガープリント記憶 #1231
	mFingerprint.SetText(GetEmptyText());
	//ファイルがなくてもOKとみなすかどうかのフラグ（新規ファイル用） #1231
	mAllowNoFile = inAllowNoFile;
	
	//#1231 if( mLegacyFTPMode == true )
	switch(mRemoteConnectionProtocolType)
	{
		//
	  case kRemoteConnectionProtocolType_FTP:
		{
			//==================レガシーFTP接続時==================
			//FTPスレッドRun（ファイル内容取得）
			GetFTPConnection().SPNVI_Run_GetFile(inRemoteFileURL,inAllowNoFile);
			break;
		}
		//
	  case kRemoteConnectionProtocolType_AccessPlugin:
	  default:
		{
			//==================プラグイン接続時==================
			//ファイル取得先テンポラリファイル
			AFileAcc	tmpFolder;
			AFileWrapper::GetTempFolder(tmpFolder);
			//フォルダを作成してファイル名重複を防止する。GET結果はファイル有無で判定。#1249 #1231でローカルファイルを生成しないようにした結果ファイル名が重複してしまう問題を対策。
			AFileAcc	tmpFileFolder;
			{
				AStMutexLocker	locker(sTempFileFolderCreateMutex);
				tmpFileFolder.SpecifyUniqChildFile(tmpFolder,"AccessPluginLocalTempFileForGET");
				//#1231 ローカルファイルは生成しない。プラグインで生成させる。結果受信時にファイルがなかったらエラーとみなす。tmpFile.CreateFile();
				tmpFileFolder.CreateFolder();
			}
			AFileAcc	tmpFile;
			AText	child("data");
			tmpFile.SpecifyChild(tmpFileFolder,child);
			tmpFile.GetPath(mLocalFilePath);
			//プラグイン実行
			ExecutePlugin("get");
		}
	}
	return true;
}
//GET用テンポラリファイルフォルダ生成用Mutex #1249
AThreadMutex	ARemoteConnection::sTempFileFolderCreateMutex;


/**
PUTコマンド要求
*/
ABool	ARemoteConnection::ExecutePUT( const ARemoteConnectionRequestType inRequestType, const AText& inRemoteFileURL,
		const AText& inText )
{
	//他のコマンド要求中は何もせず終了（今回の要求は無視される）
	if( mCurrentRequestType != kRemoteConnectionRequestType_None )
	{
		return false;
	}
	
	//接続情報設定（サーバー・ユーザー名から対応するFTPアカウントを検索して取得） #1231
	if( SetConnectionProperty(inRemoteFileURL) == false )
	{
		return false;
	}
	
	//ログ出力 #1231
	AText	log("Connecting ");
	log.AddText(inRemoteFileURL);
	log.AddCstring("...(PUT)\n");
	GetApp().SPI_GetFTPLogWindow().SPI_AddLog(log);
	
	//要求タイプ記憶
	mCurrentRequestType = inRequestType;
	//リモートファイルパス記憶
	mRemoteURL.SetText(inRemoteFileURL);
	//フィンガープリント記憶 #1231
	mFingerprint.SetText(GetEmptyText());
	
	//#1231 if( mLegacyFTPMode == true )
	switch(mRemoteConnectionProtocolType)
	{
		//
	  case kRemoteConnectionProtocolType_FTP:
		{
			//==================レガシーFTP接続時==================
			//FTPスレッドRun（ファイル内容保存）
			//現状、同期実行している。そのため、すぐにプラグイン接続解除・要求タイプクリアする
			GetFTPConnection().SPNVI_Run_PutFile(inRemoteFileURL,inText);
			//要求タイプクリア
			mCurrentRequestType = kRemoteConnectionRequestType_None;
			break;
		}
		//
	  case kRemoteConnectionProtocolType_AccessPlugin:
	  default:
		{
			//==================プラグイン接続時==================
			//ファイルアップロード元テンポラリファイル
			AFileAcc	tmpFolder;
			AFileWrapper::GetTempFolder(tmpFolder);
			AFileAcc	tmpFile;
			tmpFile.SpecifyUniqChildFile(tmpFolder,"AccessPluginLocalTempFileForPUT");
			tmpFile.CreateFile();
			tmpFile.WriteFile(inText);
			tmpFile.GetPath(mLocalFilePath);
			//プラグイン実行
			ExecutePlugin("put");
			break;
		}
	}
	return true;
}

/**
現在の要求をアボート
*/
void	ARemoteConnection::AbortCurrentRequest()
{
	//#1231 if( mLegacyFTPMode == true )
	switch(mRemoteConnectionProtocolType)
	{
		//
	  case kRemoteConnectionProtocolType_FTP:
		{
			//==================レガシーFTP接続==================
			//
			GetFTPConnection().NVI_AbortThread();
			break;
		}
	  default:
		{
			//未実装
		}
	}
	//現在の要求タイプをクリア
	mCurrentRequestType = kRemoteConnectionRequestType_None;
}

#pragma mark ===========================

#pragma mark ---コマンド応答受信時処理

/**
コマンド応答受信時処理（発行中のコマンドタイプに従って処理する）
*/
void	ARemoteConnection::DoResponseReceived( const AText& inText, const AText& inStderrText )//#1231
{
	//結果テキスト
	AText	resultText;
	//テンポラリファイル取得
	AFileAcc	tmpFile;
	tmpFile.Specify(mLocalFilePath);
	//
	if( mRemoteConnectionProtocolType != kRemoteConnectionProtocolType_FTP &&
		mCurrentRequestType == kRemoteConnectionRequestType_GETforOpenDocument )//#1231 LISTは全て標準出力から取得するようにする（ファイルに一旦出力するのは万が一パスワードが標準出力に出てしまったときにセキュリティ上よろしくない）
	{
		//FTP以外で、GET処理の場合
		//テンポラリファイルから結果テキスト読み込み
		//ローカルファイルが存在していなかったらエラー #1231
		if( tmpFile.Exist() == false )
		{
			if( mAllowNoFile == false )
			{
				//エラーダイアログ表示
				AText	message1, message2;
				message1.SetLocalizedText("RemoteAccess_ConnectError1");
				message2.SetLocalizedText("RemoteAccess_ConnectError2");
				message2.ReplaceParamText('0',inStderrText);
				GetApp().NVI_ShowModalAlertWindow(message1,message2);
				//要求タイプクリア
				mCurrentRequestType = kRemoteConnectionRequestType_None;
				//終了
				return;
			}
		}
		//テンポラリファイル読み込み
		tmpFile.ReadTo(resultText);
	}
	else
	{
		//結果テキストに標準出力テキストを設定
		resultText.SetText(inText);
	}
	//テンポラリファイルが存在していたら削除する。
	if( tmpFile.Exist() == true )
	{
		tmpFile.DeleteFile();
	}
	//ログ出力 #1231
	if( mRemoteConnectionProtocolType != kRemoteConnectionProtocolType_FTP )
	{
		AText	log(inStderrText);
		log.AddCstring("\n");
		GetApp().SPI_GetFTPLogWindow().SPI_AddLog(log);
	}
	//
	switch(mCurrentRequestType)
	{
		//ファイルリストウインドウからのディレクトリリスト要求時
	  case kRemoteConnectionRequestType_LISTfromFileListWindow:
		{
			//#1231 if( mLegacyFTPMode == true )
			switch(mRemoteConnectionProtocolType)
			{
			  case kRemoteConnectionProtocolType_FTP:
				{
					//==================レガシーFTP接続時==================
					//FTP接続オブジェクトから結果取得
					ATextArray	textArray;
					ABoolArray	isDirectoryArray;
					GetFTPConnection().SPI_GetDirectoryResult(textArray,isDirectoryArray);
					//ファイルリストウインドウに結果設定
					GetApp().SPI_GetFileListWindow(mRequestorObjectID).SPI_SetRemoteConnectionResult(textArray,isDirectoryArray);
					break;
				}
			  default:
				{
					//==================curl/プラグイン接続時==================
					//LFで区切り各行読み込み
					ATextArray	textArray;
					textArray.ExplodeFromText(resultText,'\n');
					//ファイル名、ディレクトリかどうかのフラグの配列
					ATextArray	fileNameArray;
					ABoolArray	isDirectoryArray;
					//フィンガープリント文字列抽出用正規表現
					ARegExp	regexp_fingerprint("^.+fingerprint.+:(.+)\.");
					AText	fingerprint;
					//各行ごとのループ
					for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
					{
						AText	text;
						textArray.Get(i,text);
						//SFTP専用処理
						if( mConnectionType.Compare("SFTP") == true )
						{
							//mFingerprintが空（＝新規サーバー接続ダイアログで「接続する」を選んだ場合ではない）であることの判定
							if( mFingerprint.GetItemCount() == 0 )
							{
								//フィンガープリント抽出
								if( regexp_fingerprint.Match(text) == true )
								{
									regexp_fingerprint.GetMatchedText(text,fingerprint);
								}
								//known_hostsへの登録なし判定
								//known_hostsへの登録がないというエラーが出た時は、フィンガープリントとともにメッセージを出して、
								//新規に接続しknown_hostsへ追加して良いかどうかを尋ねる。→尋ねたのち再度LISTリクエスト
								if( text.FindCstring("Are you sure you want to continue connecting (yes/no)?") == true )
								{
									//新規サーバーに接続するかどうかのダイアログを表示
									AText	message1, message2;
									message1.SetLocalizedText("SFTP_NewServer1");
									message2.SetLocalizedText("SFTP_NewServer2");
									message2.ReplaceParamText('1',fingerprint);
									AText	okButton, cancelButton;
									okButton.SetLocalizedText("SFTP_NewServer_OK");
									cancelButton.SetLocalizedText("SFTP_NewServer_Cancel");
									if( GetApp().NVI_ShowModalCancelAlertWindow(message1,message2,okButton,cancelButton) == true )
									{
										//接続する場合
										//要求タイプクリア
										mCurrentRequestType = kRemoteConnectionRequestType_None;
										//Connect
										ExecuteLIST(kRemoteConnectionRequestType_LISTfromFileListWindow,mRemoteURL,fingerprint);
									}
									return;
								}
							}
						}
						//
						if( text.GetItemCount() > 0 )
						{
							AUChar	firstch = text.Get(0);
							//最初の文字がdの場合（ディレクトリ）
							if( firstch == 'd' )
							{
								AText	filename;
								if( text.ExtractUsingRegExp("^d\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+(\\S.+)$",1,filename) == true )
								{
									fileNameArray.Add(filename);
									isDirectoryArray.Add(true);
								}
							}
							//最初の文字が-の場合（ファイル）
							else if( firstch == '-' )
							{
								AText	filename;
								if( text.ExtractUsingRegExp("^-\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+(\\S.+)$",1,filename) == true )
								{
									fileNameArray.Add(filename);
									isDirectoryArray.Add(false);
								}
							}
						}
					}
					//リスト結果をファイルリストウインドウに設定
					GetApp().SPI_GetFileListWindow(mRequestorObjectID).SPI_SetRemoteConnectionResult(fileNameArray,isDirectoryArray);
					//LIST結果がない場合はエラーとみなし、エラーダイアログ表示（正常の場合は、.と..が存在するはず。） #1236
					if( fileNameArray.GetItemCount() == 0 )
					{
						//エラーダイアログ表示
						AText	message1, message2;
						message1.SetLocalizedText("RemoteAccess_ConnectError1");
						message2.SetLocalizedText("RemoteAccess_ConnectError2");
						message2.ReplaceParamText('0',inStderrText);
						GetApp().NVI_ShowModalAlertWindow(message1,message2);
						return;
					}
					break;
				}
			}
			break;
		}
		//ドキュメントを開くためのGET
	  case kRemoteConnectionRequestType_GETforOpenDocument:
		{
			/*#1231 
			if( mLegacyFTPMode == true )
			{
				//==================レガシーFTP接続時==================
				//リモートファイル用ドキュメント生成
				GetApp().SPNVI_CreateDocumentFromRemoteFile(GetObjectID(),mRemoteURL,inText);
			}
			else
			{
				//==================プラグイン接続時==================
				//テンポラリファイルからテキスト読み込み
				AFileAcc	tmpFile;
				tmpFile.Specify(mLocalFilePath);
				AText	text;
				tmpFile.ReadTo(text);
				tmpFile.DeleteFile();
			*/
			//リモートファイル用ドキュメント生成
			GetApp().SPNVI_CreateDocumentFromRemoteFile(GetObjectID(),mRemoteURL,resultText);
			/*#1231
			}
			*/
			break;
		}
		//ドキュメント保存PUT
	  case kRemoteConnectionRequestType_PUTfromTextDocument:
		{
			//処理なし
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//要求タイプクリア
	mCurrentRequestType = kRemoteConnectionRequestType_None;
}

