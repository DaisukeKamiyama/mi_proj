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

#pragma once

#include "../../AbsFramework/Frame.h"

/**
�����[�g�ڑ��^�C�v
*/
enum ARemoteConnectionType
{
	kRemoteConnectionType_FileListWindow = 0,
	kRemoteConnectionType_TextDocument,
};

/**
�����[�g�ڑ��v���^�C�v
*/
enum ARemoteConnectionRequestType
{
	kRemoteConnectionRequestType_None = 0,
	kRemoteConnectionRequestType_LISTfromFileListWindow,
	kRemoteConnectionRequestType_GETforOpenDocument,
	kRemoteConnectionRequestType_PUTfromTextDocument
};

/**
�g�ݍ��ݐڑ��^�C�v #1231
*/
enum ARemoteConnectionProtocolType
{
	/*#1231
	kRemoteConnectionProtocolType_SFTP = 0,
	kRemoteConnectionProtocolType_FTPS,
	*/
	kRemoteConnectionProtocolType_FTP = 0,
	kRemoteConnectionProtocolType_AccessPlugin,
};

class AFTPConnection;

#pragma mark ===========================
#pragma mark [�N���X]ARemoteConnection
/**
�����[�g�ڑ��I�u�W�F�N�g
*/
class ARemoteConnection : public AObjectArrayItem
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	ARemoteConnection( const ARemoteConnectionType inType, const AObjectID inRequestorObjectID );
~ARemoteConnection();
AFTPConnection&	GetFTPConnection();
  private:
	ARemoteConnectionType			mAccessPluginConnectionType;
	AObjectID						mRequestorObjectID;
	
	//�f�[�^
  public:
	ARemoteConnectionRequestType	SPI_GetCurrentRequestType() const { return mCurrentRequestType; }
	const AText&					GetRemoteFilePath() const { return mRemoteURL; }//#1228
  private:
	ARemoteConnectionRequestType	mCurrentRequestType;
	AText							mRemoteURL;
	AText							mLocalFilePath;
	AText							mFingerprint;//#1231
	AObjectID						mFTPConnectionObjectID;
	ABool							mAllowNoFile;//#1231
	
	//�ڑ�
  public:
	//#1231 ABool			SetConnectionType( const AText& inConnectionType );//#193
  private:
	ABool			ExecutePlugin( const AConstCharPtr inPluginType );//#193
	ABool			SetConnectionProperty( const AText& inRemoteFolderURL );//#1231
	AText							mConnectionType;
	//#1231 ABool							mLegacyFTPMode;
	ARemoteConnectionProtocolType	mRemoteConnectionProtocolType;//#1231
	AFileAcc						mPluginFile;
	//#1231
	APortNumber						mPort;
	ABool							mPassive;
	ABool							mBinaryMode;
	ABool							mUsePrivateKey;
	AText							mPrivateKeyFilePath;
	ABool							mDontCheckServerCert;//#1231
	
	//�v��
  public:
	ABool			ExecuteLIST(const ARemoteConnectionRequestType inRequestType, const AText& inRemoteFolderURL, const AText& inFingerprint );//#1231
	ABool			ExecuteGET( const ARemoteConnectionRequestType inRequestType, const AText& inRemoteFileURL, const ABool inAllowNoFile );
	ABool			ExecutePUT( const ARemoteConnectionRequestType inRequestType, const AText& inRemoteFileURL,
							   const AText& inText );
	void			AbortCurrentRequest();
	
	//������M������
  public:
	void			DoResponseReceived( const AText& inText, const AText& inStderrText );//#1231
	
	//GET�p�e���|�����t�@�C���t�H���_�����pMutex #1249
	static AThreadMutex				sTempFileFolderCreateMutex;
};

#pragma mark ===========================
#pragma mark [�N���X]ARemoteConnectionFactory
/**
�����[�g�ڑ��I�u�W�F�N�gFactory
*/
class ARemoteConnectionFactory : public AAbstractFactoryForObjectArray<ARemoteConnection>
{
  public:
	ARemoteConnectionFactory( const ARemoteConnectionType inType, const AObjectID inRequestorObjectID ) 
	: mType(inType), mObjectID(inRequestorObjectID)
	{
	}
	ARemoteConnection*	Create() 
	{
		return new ARemoteConnection(mType,mObjectID);
	}
  private:
	ARemoteConnectionType	mType;
	AObjectID					mObjectID;
};

