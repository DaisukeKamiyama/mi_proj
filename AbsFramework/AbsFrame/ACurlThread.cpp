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

ACurlThread
#427

*/

#include "stdafx.h"

#include "ACurlThread.h"
//#1159 #include <curl/curl.h>
#include "../../App/Source/AApp.h"

#pragma mark ===========================
#pragma mark [クラス]ACurlThread
#pragma mark ===========================
//マルチファイル検索実行

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
ACurlThread::ACurlThread( AObjectArrayItem* inParent ) : AThread(inParent), 
		mRequestType(kCurlRequestType_None), mTransactionType(kCurlTransactionType_None)
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/**
読み込みコールバックルーチン
*/
/*#1159
size_t	ACurlThread::ReadToMemory( void *ptr, size_t size, size_t nmemb, void *data )
{
	AObjectID	objID = *((AObjectID*)(data));
	ACurlThread&	object = dynamic_cast<ACurlThread&>(AApplication::GetApplication().NVI_GetThreadByID(objID));
	size_t realsize = size * nmemb;
	object.mResultText.AddFromTextPtr((AConstUCharPtr)ptr,realsize);
	return realsize;
}
*/

/**
スレッドメインルーチン
*/
void	ACurlThread::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ACurlThread::NVIDO_ThreadMain started.",this);
	try
	{
		switch(mRequestType)
		{
		  case kCurlRequestType_HTTP:
			{
				//Mac OS X標準添付のcurlコマンドを使用する
				AText	command;
				command.SetCstring("/usr/bin/curl");
				ATextArray	argArray;
				argArray.Add(command);
				argArray.Add(mURL);
				AText	resultText;
				GetApp().SPI_GetChildProcessManager().ExecuteGeneralSyncCommand(command,argArray,GetEmptyText(),mResultText);
				
				/*#1159
				//curlセッション初期化
				CURL *curl_handle = curl_easy_init();
				
				CURLcode	err;
				//URL設定
				{
					AStCreateCstringFromAText	cstr(mURL);
					err = curl_easy_setopt(curl_handle, CURLOPT_URL, cstr.GetPtr());
				}
				
				//コールバックルーチン設定
				err = curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, ACurlThread::ReadToMemory);
				
				//コールバックルーチンへの引数設定
				AObjectID	objID = GetObjectID();
				err = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)(&objID));
				
				//いくつかのweb serverではuser-agentがないとアクセスできないらしい
				err = curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
				
				//取得実行（コールバックが都度コールされる）
				err = curl_easy_perform(curl_handle);
				
				//終了処理
				curl_easy_cleanup(curl_handle);
				*/
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	catch(...)
	{
		_ACError("ACurlThread::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	
	//メインスレッドへ通知
	AObjectIDArray	objectIDArray;
	objectIDArray.Add(GetObjectID());
	ABase::PostToMainInternalEventQueue(kInternalEvent_Curl,GetObjectID(),0,GetEmptyText(),objectIDArray);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ACurlThread::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---スレッド起動

/**
ダウンロード開始（スレッド起動）
*/
void	ACurlThread::SPNVI_Run( const ECurlRequestType inRequestType, const AText& inURL, 
			const ACurlTransactionType inTransactionType,  const ATextArray& inInfoTextArray )
{
	//空のURLで動作させるとbad access発生するのでこのチェック必要
	if( inURL.GetItemCount() == 0 )
	{
		return;
	}
	//
	mRequestType = inRequestType;
	mURL.SetText(inURL);
	mTransactionType = inTransactionType;
	mInfoTextArray.SetFromTextArray(inInfoTextArray);//非同期処理なので、結果取得時の処理のための情報テキストを記憶しておく。
	//スレッド起動
	NVI_Run();
}

#pragma mark ===========================

#pragma mark ---結果取得

/**
結果取得
*/
void	ACurlThread::GetResult( AText& outText, ECurlRequestType& outRequestType, AText& outURL, 
			ACurlTransactionType& outTransactionType, ATextArray& outInfoTextArray )
{
	outText.SetText(mResultText);
	outRequestType = mRequestType;
	outURL.SetText(mURL);
	outTransactionType = mTransactionType;
	outInfoTextArray.SetFromTextArray(mInfoTextArray);
}

