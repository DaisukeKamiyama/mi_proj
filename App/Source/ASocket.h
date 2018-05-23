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

ASocket.h

*/

#pragma once

#include "../../AbsFramework/AbsBase/ABase.h"

#pragma mark ===========================
#pragma mark [クラス]ASocket
//ソケット 
class ASocket
{
	//コンストラクタ／デストラクタ 
  public:
	ASocket();
	~ASocket();
  private:
	ASocketDescriptor	mSocketDescriptor;
	
	//接続／切断
  public:
	void	Init();
	void	Connect( const AText& inRemoteHostName, const APortNumber inRemoteHostPortNumber );
	void	Connect( const AIPAddress inRemoteHostName, const APortNumber inRemoteHostPortNumber );
	void	EphemeralListen( const ANumber inBackLog );
	void	BindListen( const APortNumber inPortNumber, const ANumber inBackLog );
	void	Accept( const ASocket& inListenSocket );
	void	Close();
	
	//情報取得／設定
  public:
	void	GetLocalhostAddress( AIPAddress& outIPAddress, APortNumber& outPort ) const;
	
	//データ送受信
  public:
	void	ReceiveAll( AText& outBuffer ) const;
	void	Receive( AText& outQueueBuffer, ABool& outDisconnected ) const;
	void	Send( const AText& inBuffer ) const;
  private:
	AIPAddress	mRemoteHostIPAddress;
	APortNumber	mRemoteHostPortNumber;
	
};

