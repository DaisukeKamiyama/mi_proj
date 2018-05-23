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
#pragma mark [�N���X]ASocket
#pragma mark ===========================
//�l�b�g���[�N�\�P�b�g

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
ASocket::ASocket() : mSocketDescriptor(kSocketDescriptor_Invalid)
{
}
//�f�X�g���N�^
ASocket::~ASocket()
{
	Close();
}

#pragma mark ===========================

#pragma mark ---�ڑ��^�ؒf

//�����ݒ�
void	ASocket::Init()
{
	mRemoteHostIPAddress = 0;
	mRemoteHostPortNumber = 0;
}

//�ڑ�(�A�N�e�B�u�I�[�v��)
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

//Listen�i�Z���|�[�g�j
void	ASocket::EphemeralListen( const ANumber inBackLog )
{
	if( mSocketDescriptor != kSocketDescriptor_Invalid )   _ACThrow("socket already open",NULL);
	Init();
	mSocketDescriptor = ANetworkWrapper::OpenSocket();
	ANetworkWrapper::Listen(mSocketDescriptor,inBackLog);
}
//Listen�i�|�[�g�ԍ��w��j
void	ASocket::BindListen( const APortNumber inPortNumber, const ANumber inBackLog )
{
	if( mSocketDescriptor != kSocketDescriptor_Invalid )   _ACThrow("socket already open",NULL);
	Init();
	mSocketDescriptor = ANetworkWrapper::OpenSocket();
	ANetworkWrapper::BindListen(mSocketDescriptor,inPortNumber,inBackLog);
}

//Accept�iListen�\�P�b�g�̃L���[����3wayHS�����ςݐڑ��\�P�b�g���P���o���j
void	ASocket::Accept( const ASocket& inListenSocket )
{
	if( mSocketDescriptor != kSocketDescriptor_Invalid )   _ACThrow("socket already open",NULL);
	Init();
	mSocketDescriptor = ANetworkWrapper::Accept(inListenSocket.mSocketDescriptor);
	
	//�ڑ���IP�A�h���X�`�F�b�N
	
}

//Close
void	ASocket::Close()
{
	//���ɃN���[�Y�ς݂̏ꍇ�͉����������^�[��
	if( mSocketDescriptor == kSocketDescriptor_Invalid )   return;
	//�\�P�b�g�N���[�Y����B�i���ɑ��肩��ؒf����Ă���\�������邪�A���̏ꍇ�ł��\�킸close�R�[������j
	ANetworkWrapper::Close(mSocketDescriptor);
	mSocketDescriptor = kSocketDescriptor_Invalid;
}

#pragma mark ===========================

#pragma mark ---���擾�^�ݒ�

//�ڑ��ς݃\�P�b�g�́A������IP�A�h���X�^�|�[�g�ԍ����擾����
void	ASocket::GetLocalhostAddress( AIPAddress& outIPAddress, APortNumber& outPort ) const
{
	if( mSocketDescriptor == kSocketDescriptor_Invalid )   _ACThrow("socket not yet open",NULL);
	ANetworkWrapper::GetLocalhostAddress(mSocketDescriptor,outIPAddress,outPort);
}

#pragma mark ===========================

#pragma mark ---�f�[�^����M

//���肩��ؒf�����܂ŁA�S�Ẵf�[�^����M����
//�f�[�^�҂��̊Ԃ�block����
void	ASocket::ReceiveAll( AText& outBuffer ) const
{
	outBuffer.DeleteAll();
	ABool	disconnected = false;
	while(disconnected==false)
	{
		ANetworkWrapper::Recv(mSocketDescriptor,outBuffer,disconnected);
	}
}

//�f�[�^�ЂƂ܂Ƃ܂����M����
//�f�[�^�҂��̊Ԃ�block����
void	ASocket::Receive( AText& outQueueBuffer, ABool& outDisconnected ) const
{
	ANetworkWrapper::Recv(mSocketDescriptor,outQueueBuffer,outDisconnected);
}

//�f�[�^���M
void	ASocket::Send( const AText& inBuffer ) const
{
	ANetworkWrapper::Send(mSocketDescriptor,inBuffer);
}

