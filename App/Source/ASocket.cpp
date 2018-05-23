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

ASocket.cpp

*/

#include "stdafx.h"

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "ASocket.h"

#pragma mark ===========================
#pragma mark [クラス]ASocket
#pragma mark ===========================
//ネットワークソケット

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
ASocket::ASocket() : mSocketDescriptor(kSocketDescriptor_Invalid)
{
}
//デストラクタ
ASocket::~ASocket()
{
	Close();
}

#pragma mark ===========================

#pragma mark ---接続／切断

//初期設定
void	ASocket::Init()
{
	mRemoteHostIPAddress = 0;
	mRemoteHostPortNumber = 0;
}

//接続(アクティブオープン)
void	ASocket::Connect( const AText& inRemoteHostName, const APortNumber inRemoteHostPortNumber )
{
	if( mSocketDescriptor != kSocketDescriptor_Invalid )   _ACThrow("socket already open",NULL);
	Init();
	//
	AText	hostname;
	hostname.SetText(inRemoteHostName);
	APortNumber	portnumber = inRemoteHostPortNumber;
	for( AIndex pos = 0; pos < inRemoteHostName.GetItemCount(); pos++ )
	{
		AUChar	ch = inRemoteHostName.Get(pos);
		if( ch == kUChar_Colon )
		{
			inRemoteHostName.GetText(0,pos,hostname);
			pos++;
			if( pos < inRemoteHostName.GetItemCount() )
			{
				AText	text;
				inRemoteHostName.GetText(pos,inRemoteHostName.GetItemCount()-pos,text);
				portnumber = static_cast<APortNumber>(text.GetNumber());
			}
			break;
		}
	}
	//
	mRemoteHostIPAddress = ANetworkWrapper::GetIPAddressFromName(hostname);
	mRemoteHostPortNumber = portnumber;
	mSocketDescriptor = ANetworkWrapper::OpenSocket();
	ANetworkWrapper::Connect(mSocketDescriptor,mRemoteHostIPAddress,mRemoteHostPortNumber);
}
void	ASocket::Connect( const AIPAddress inRemoteHostName, const APortNumber inRemoteHostPortNumber )
{
	if( mSocketDescriptor != kSocketDescriptor_Invalid )   _ACThrow("socket already open",NULL);
	Init();
	mRemoteHostIPAddress = inRemoteHostName;
	mRemoteHostPortNumber = inRemoteHostPortNumber;
	mSocketDescriptor = ANetworkWrapper::OpenSocket();
	ANetworkWrapper::Connect(mSocketDescriptor,mRemoteHostIPAddress,mRemoteHostPortNumber);
}

//Listen（短命ポート）
void	ASocket::EphemeralListen( const ANumber inBackLog )
{
	if( mSocketDescriptor != kSocketDescriptor_Invalid )   _ACThrow("socket already open",NULL);
	Init();
	mSocketDescriptor = ANetworkWrapper::OpenSocket();
	ANetworkWrapper::Listen(mSocketDescriptor,inBackLog);
}
//Listen（ポート番号指定）
void	ASocket::BindListen( const APortNumber inPortNumber, const ANumber inBackLog )
{
	if( mSocketDescriptor != kSocketDescriptor_Invalid )   _ACThrow("socket already open",NULL);
	Init();
	mSocketDescriptor = ANetworkWrapper::OpenSocket();
	ANetworkWrapper::BindListen(mSocketDescriptor,inPortNumber,inBackLog);
}

//Accept（Listenソケットのキューから3wayHS完了済み接続ソケットを１つ取り出す）
void	ASocket::Accept( const ASocket& inListenSocket )
{
	if( mSocketDescriptor != kSocketDescriptor_Invalid )   _ACThrow("socket already open",NULL);
	Init();
	mSocketDescriptor = ANetworkWrapper::Accept(inListenSocket.mSocketDescriptor);
	
	//接続先IPアドレスチェック
	
}

//Close
void	ASocket::Close()
{
	//既にクローズ済みの場合は何もせずリターン
	if( mSocketDescriptor == kSocketDescriptor_Invalid )   return;
	//ソケットクローズする。（既に相手から切断されている可能性もあるが、その場合でも構わずcloseコールする）
	ANetworkWrapper::Close(mSocketDescriptor);
	mSocketDescriptor = kSocketDescriptor_Invalid;
}

#pragma mark ===========================

#pragma mark ---情報取得／設定

//接続済みソケットの、自分のIPアドレス／ポート番号を取得する
void	ASocket::GetLocalhostAddress( AIPAddress& outIPAddress, APortNumber& outPort ) const
{
	if( mSocketDescriptor == kSocketDescriptor_Invalid )   _ACThrow("socket not yet open",NULL);
	ANetworkWrapper::GetLocalhostAddress(mSocketDescriptor,outIPAddress,outPort);
}

#pragma mark ===========================

#pragma mark ---データ送受信

//相手から切断されるまで、全てのデータを受信する
//データ待ちの間はblockする
void	ASocket::ReceiveAll( AText& outBuffer ) const
{
	outBuffer.DeleteAll();
	ABool	disconnected = false;
	while(disconnected==false)
	{
		ANetworkWrapper::Recv(mSocketDescriptor,outBuffer,disconnected);
	}
}

//データひとまとまりを受信する
//データ待ちの間はblockする
void	ASocket::Receive( AText& outQueueBuffer, ABool& outDisconnected ) const
{
	ANetworkWrapper::Recv(mSocketDescriptor,outQueueBuffer,outDisconnected);
}

//データ送信
void	ASocket::Send( const AText& inBuffer ) const
{
	ANetworkWrapper::Send(mSocketDescriptor,inBuffer);
}

