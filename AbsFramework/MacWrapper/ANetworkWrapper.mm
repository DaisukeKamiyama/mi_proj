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

#include "ANetworkWrapper.h"
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/**
名前からIPアドレス取得
*/
AIPAddress	ANetworkWrapper::GetIPAddressFromName( const AText& inName )
{
	AStCreateCstringFromAText	servercstr(inName);
	struct hostent*	host = gethostbyname(servercstr.GetPtr());
	if( host == NULL )   _AThrow("cannot resolve server name to IP address",NULL);
	struct in_addr*	addr = (struct in_addr*)(host->h_addr_list[0]);
	return ntohl(addr->s_addr);
}

/**
Open
*/
ASocketDescriptor	ANetworkWrapper::OpenSocket()
{
	ASocketDescriptor	result = socket(AF_INET,SOCK_STREAM,0);
	if( result == -1 )   _AThrow("cannot open socket",NULL);
	return result;
}

/**
Connect
*/
void	ANetworkWrapper::Connect( ASocketDescriptor inSocket, AIPAddress inIPAddress, APortNumber inPortNumber )
{
	struct sockaddr_in	serveraddr;
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(inPortNumber);
	serveraddr.sin_addr.s_addr = htonl(inIPAddress);
	if( connect(inSocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) == -1 )   _AThrow("cannot connect",NULL);
}

/**
Accept
*/
ASocketDescriptor	ANetworkWrapper::Accept( ASocketDescriptor inListenSocket )
{
	struct sockaddr_in	clientaddr;
	socklen_t	addrlen = sizeof(clientaddr);
	ASocketDescriptor	result =  accept(inListenSocket,(struct sockaddr*)&clientaddr,&addrlen);
	if( result == kSocketDescriptor_Invalid )   _AThrow("accept error",NULL);
	return result;
}

/**
Close
*/
void	ANetworkWrapper::Close( ASocketDescriptor inSocket )
{
	//if( close(inSocket) == -1 )   _AThrow("cannot close",NULL);
	close(inSocket);
}

/**
短命ポートによるListen
*/
void	ANetworkWrapper::Listen( ASocketDescriptor inSocket, ANumber inBackLog )
{
	if( listen(inSocket,inBackLog) == -1 )   _AThrow("cannot listen",NULL);
}

/**
ポート番号指定によるlisten
*/
void	ANetworkWrapper::BindListen( const ASocketDescriptor inSocket, const APortNumber inPortNumber, const ANumber inBackLog )
{
	int	val = 1;
	socklen_t	len = sizeof(int);
	setsockopt(inSocket,SOL_SOCKET,SO_REUSEPORT,&val,len);
	struct sockaddr_in	serveraddr;
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(inPortNumber);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if( bind(inSocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) == -1 )   {fprintf(stderr,"errno:%d",errno);_ACThrow("cannot bind",NULL);}
	//bind(inSocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if( listen(inSocket,inBackLog) == -1 )   _AThrow("cannot listen",NULL);
}

/**
自IPアドレス取得
*/
void	ANetworkWrapper::GetLocalhostAddress( ASocketDescriptor inSocket, AIPAddress& outIPAddress, APortNumber& outPort )
{
	struct sockaddr_in	addr;
	socklen_t	addrlen = sizeof(addr);
	if( getsockname(inSocket,(struct sockaddr*)&addr,&addrlen) == -1 )   _AThrow("cannot getsockname",NULL);
	outIPAddress = ntohl(addr.sin_addr.s_addr);
	outPort = ntohs(addr.sin_port);
}

/**
Recv
*/
void	ANetworkWrapper::Recv( const ASocketDescriptor inSocket, AText& outQueueBuffer, ABool& outDisconnected )
{
	outDisconnected = false;
	//
	ssize_t	readCount = 0;
	char	buffer[4096];
	//recvは、データを受信するまでblockする。
	readCount = recv(inSocket,buffer,sizeof(buffer),0);
	if( readCount < 0 )   _AThrow("recv error",NULL);//B0448
	if( readCount == 0 )
	{
		//readCount==0ということは、相手から切断されたということ。
		outDisconnected = true;
	}
	else
	{
		//
		outQueueBuffer.AddFromTextPtr(buffer,readCount);
	}
}

/**
Send
*/
void	ANetworkWrapper::Send( const ASocketDescriptor inSocket, const AText& inBuffer )
{
	AStTextPtr	bufptr(inBuffer,0,inBuffer.GetItemCount());
	/*ssize_t writeCount = */send(inSocket,bufptr.GetPtr(),bufptr.GetByteCount(),0);
}


