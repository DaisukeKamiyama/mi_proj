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

ANetworkWrapper

*/

#pragma once

#include "../AbsBase/ABase.h"

class ANetworkWrapper
{
  public:
	static AIPAddress	GetIPAddressFromName( const AText& inName );
	static ASocketDescriptor	OpenSocket();
	static void	Connect( ASocketDescriptor inSocket, AIPAddress inIPAddress, APortNumber inPortNumber );
	static ASocketDescriptor	Accept( ASocketDescriptor inListenSocket );
	static void Close( ASocketDescriptor inSocket );
	static void	Listen( ASocketDescriptor inSocket, ANumber inBackLog );
	static void	BindListen( const ASocketDescriptor inSocket, const APortNumber inPortNumber, const ANumber inBackLog );
	static void	GetLocalhostAddress( ASocketDescriptor inSocket, AIPAddress& outIPAddress, APortNumber& outPort );
	static void	Recv( const ASocketDescriptor inSocket, AText& outQueueBuffer, ABool& outDisconnected );
	static void	Send( const ASocketDescriptor inSocket, const AText& inBuffer );
};

