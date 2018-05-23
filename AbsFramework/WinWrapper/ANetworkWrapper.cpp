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

#include "stdafx.h"

#include "ANetworkWrapper.h"

AIPAddress	ANetworkWrapper::GetIPAddressFromName( const AText& inName )
{
	AStCreateCstringFromAText	servercstr(inName);
	struct hostent*	host = ::gethostbyname(servercstr.GetPtr());
	if( host == NULL )   _AThrow("",NULL);
	struct in_addr*	addr = (struct in_addr*)(host->h_addr_list[0]);
	return ntohl(addr->S_un.S_addr);
}

ASocketDescriptor	ANetworkWrapper::OpenSocket()
{
	ASocketDescriptor	result = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if( result == INVALID_SOCKET )   _AThrow("",NULL);
	return result;
}

void	ANetworkWrapper::Connect( ASocketDescriptor inSocket, AIPAddress inIPAddress, APortNumber inPortNumber )
{
	struct sockaddr_in	serveraddr;
	::memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family			= AF_INET;
	serveraddr.sin_port				= ::htons(inPortNumber);
	serveraddr.sin_addr.S_un.S_addr	= htonl(inIPAddress);
	if( ::connect(inSocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) == SOCKET_ERROR )   _AThrow("",NULL);
}

ASocketDescriptor	ANetworkWrapper::Accept( ASocketDescriptor inListenSocket )
{
	struct sockaddr_in	clientaddr;
	int	addrlen = sizeof(clientaddr);
	ASocketDescriptor	result = ::accept(inListenSocket,(struct sockaddr*)&clientaddr,&addrlen);
	if( result == INVALID_SOCKET )   _AThrow("",NULL);
	return result;
}

void	ANetworkWrapper::Close( ASocketDescriptor inSocket )
{
	::closesocket(inSocket);
}

//短命ポートによるListen
void	ANetworkWrapper::Listen( ASocketDescriptor inSocket, ANumber inBackLog )
{
	if( ::listen(inSocket,inBackLog) == SOCKET_ERROR )   _AThrow("",NULL);
}

//ポート番号指定によるlisten
void	ANetworkWrapper::BindListen( const ASocketDescriptor inSocket, const APortNumber inPortNumber, const ANumber inBackLog )
{
	int	val = 1;
	int	len = sizeof(int);
	::setsockopt(inSocket,SOL_SOCKET,SO_REUSEADDR,(char*)&val,len);//★暫定 確認必要SO_REUSEADDR
	struct sockaddr_in	serveraddr;
	::memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family			= AF_INET;
	serveraddr.sin_port				= ::htons(inPortNumber);
	serveraddr.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);
	if( ::bind(inSocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) == SOCKET_ERROR )   _AThrow("",NULL);
	if( ::listen(inSocket,inBackLog) == SOCKET_ERROR )   _AThrow("",NULL);
}

void	ANetworkWrapper::GetLocalhostAddress( ASocketDescriptor inSocket, AIPAddress& outIPAddress, APortNumber& outPort )
{
	struct sockaddr_in	addr;
	int	addrlen = sizeof(addr);
	if( ::getsockname(inSocket,(struct sockaddr*)&addr,&addrlen) == SOCKET_ERROR )   _AThrow("",NULL);
	outIPAddress = ntohl(addr.sin_addr.S_un.S_addr);
	outPort = ntohs(addr.sin_port);
}

//
void	ANetworkWrapper::Recv( const ASocketDescriptor inSocket, AText& outQueueBuffer, ABool& outDisconnected )
{
	outDisconnected = false;
	//
	int	readCount = 0;
	char	buffer[4096];
	//recvは、データを受信するまでブロックする
	readCount = ::recv(inSocket,buffer,sizeof(buffer),0);
	if( readCount < 0 )   _AThrow("",NULL);
	if( readCount == 0 )
	{
		//相手から切断
		outDisconnected = true;
	}
	else
	{
		outQueueBuffer.AddFromTextPtr(buffer,readCount);
	}
}

//
void	ANetworkWrapper::Send( const ASocketDescriptor inSocket, const AText& inBuffer )
{
	AStTextPtr	bufptr(inBuffer,0,inBuffer.GetItemCount());
	size_t	writeCount = ::send(inSocket,(char*)(bufptr.GetPtr()),bufptr.GetByteCount(),0);
}


