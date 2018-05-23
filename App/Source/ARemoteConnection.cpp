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
#pragma mark [�N���X]ARemoteConnection
#pragma mark ===========================
/*
�����[�g�ڑ��I�u�W�F�N�g
�E�t�@�C�����X�g�E�C���h�E�ɑ΂��ĂP�A�܂��́A�����[�g�t�@�C���̃h�L�������g�ɑ΂��ĂP���݂��A�����[�g�t�@�C���̃f�[�^�擾�A�ۑ����s��
�E�v���O�C���ڑ����v����������M�i�����ɏ�Ƀv���O�C���ڑ����������j�̗���
�E�v���͏�ɂP�̂݁B�������ς���܂ł́A���̗v���͂ł��Ȃ��i���������j�B
*/

/*
2018/2/3 �����[�g�A�N�Z�X�Ή�
SFTP�́Aexpect�X�N���v�g�őΉ�����B
���R�F
�Ecurl�́Aknown host���ǂ����̔���̎d�l��SFTP�����ɂł��Ȃ��Bman-in-the-middle�΍􂪕s�\���ɂȂ�B
�EMac OS X�W����curl�ł�SFTP�Ή����Ă��Ȃ��̂ŁA�����Ńr���h�������̂��g�������Ȃ����A���ɂ���ē����Ȃ����Ƃ�����A���؂�����Ȃ��B
�_�C�i�~�b�N���C�u�����͑S���A�v���ɓY�t���āAotool�Ń_�C�i�~�b�N���C�u�����Q�Ɛ���A�v�������C�u�����ɂ���Ηǂ��������A
Mac OS X 10.7�ȑO�ł����삷��悤�ɂ�����Ȃ������B�܂��AMBP�Ńr���h����curl��Air��illegal instruction�ɂȂ�ȂǁA�܂��킩��Ȃ��_������B
*/

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
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
�f�X�g���N�^
*/
ARemoteConnection::~ARemoteConnection()
{
}

/**
���K�V�[FTP�ڑ��I�u�W�F�N�g�擾
*/
AFTPConnection&	ARemoteConnection::GetFTPConnection()
{
	//FTP�ڑ��I�u�W�F�N�g�������Ȃ琶��
	if( mFTPConnectionObjectID == kObjectID_Invalid )
	{
		AFTPConnectionFactory	factory(this);
		mFTPConnectionObjectID = GetApp().NVI_CreateThread(factory);
		GetFTPConnection().SPI_SetRemoteConnectionObjectID(GetObjectID());
	}
	return dynamic_cast<AFTPConnection&>(GetApp().NVI_GetThreadByID(mFTPConnectionObjectID));
}

#pragma mark ===========================

#pragma mark ---�ڑ��^�ؒf

//#1231
/**
�ڑ����ݒ�
�i�T�[�o�[�E���[�U�[������Ή�����FTP�A�J�E���g���������Ď擾�j
*/
ABool	ARemoteConnection::SetConnectionProperty( const AText& inRemoteURL )
{
	//server, user����FTP�A�J�E���g������
	AText	server, user;
	inRemoteURL.GetFTPURLServer(server);
	inRemoteURL.GetFTPURLUser(user);
	AIndex	index = GetApp().GetAppPref().FindFTPAccountPrefIndex(server,user);
	if( index != kIndex_Invalid )
	{
		//FTP�A�J�E���g���擾
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
		
		//�v���O�C������
		AText	pluginFilePath;
		if( GetApp().SPI_GetPluginPath(mConnectionType,pluginFilePath) == true )
		{
			//�v���O�C�������������ꍇ�͂�����g�p
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
�ڑ��^�C�v�ݒ�
*/
/*#1231
ABool	ARemoteConnection::SetConnectionType( const AText& inConnectionType )
{
	//�R�l�N�V�����^�C�v����v���O�C���̃t�@�C���p�X���擾
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
�v���O�C�����s
*/
ABool	ARemoteConnection::ExecutePlugin( const AConstCharPtr inPluginType )
{ 
	//�����^�C�v
	AText	type(inPluginType);
	//�����[�g�t�@�C��URL����T�[�o�[�����؂�o��
	AText	server, user, path, password;
	mRemoteURL.GetFTPURLServer(server);
	mRemoteURL.GetFTPURLUser(user);
	mRemoteURL.GetFTPURLPath(path);
	AText	porttext;
	porttext.SetNumber(mPort);//#1231 inRemoteURL.GetFTPURLPortNumber());
	//�p�X���[�h�擾
	password.SetCstring(" ");
#if IMPLEMENTATION_FOR_MACOSX
	 ASecurityWrapper::GetInternetPassword(server,user,password);
	if( password.GetItemCount() == 0 )
	{
		password.SetCstring(" ");
	}
#endif
	
	//�v���O�C���p�X�擾
	AText	pluginPath;
	mPluginFile.GetPath(pluginPath);
	
	//�v���O�C�����s
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
	//SFTP��p����
	if( mConnectionType.Compare("SFTP") == true )
	{
		//�閧���p�X
		AText	keyfilePath(" ");
		if( mUsePrivateKey == true )
		{
			keyfilePath.SetText(mPrivateKeyFilePath);
		}
		argTextArray.Add(keyfilePath);
		//�t�B���K�[�v�����g
		AText	fingerprint(" ");
		if( mFingerprint.GetItemCount() > 0 )
		{
			fingerprint.SetText(mFingerprint);
		}
		argTextArray.Add(fingerprint);
	}
	//FTPS��p����
	else if( mConnectionType.Compare("FTPS") == true )
	{
		//�T�[�o�[�ؖ����`�F�b�N�I�v�V����
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

#pragma mark ---�e�R�}���h�v��

/**
LIST�R�}���h�v��
*/
ABool	ARemoteConnection::ExecuteLIST( const ARemoteConnectionRequestType inRequestType, const AText& inRemoteFolderURL, const AText& inFingerprint )//#1231
{
	//���̃R�}���h�v�����͌��݂̗v�����A�{�[�g���A���΂炭�҂��āi���X���b�h�I���҂��j���獡��̗v�������s����
	if( mCurrentRequestType != kRemoteConnectionRequestType_None )
	{
		AbortCurrentRequest();
		GetApp().NVI_SleepWithTimer(1);
		//�X���b�h���I���d�؂�Ȃ������ꍇ�ASPNVI_Run_GetDirectory()�ŃX���b�h�N�����ꂸ�ɏI������B
		//���̏ꍇ�A�ēxExecuteLIST()���s���ɁA������x���������s�����B���̂Ƃ��ɃX���b�h���I�����Ă���΁A���̂Ƃ��ɂ͐������邵�A
		//�X���b�h���I�����Ȃ��Ȃ��Ă��܂��Ă��Ă��A���̃����[�g�ڑ��ŃR�}���h���s�ł��Ȃ��ȏ�̖��ɂ͂Ȃ�Ȃ��B
	}
	
	//�ڑ����ݒ�i�T�[�o�[�E���[�U�[������Ή�����FTP�A�J�E���g���������Ď擾�j #1231
	if( SetConnectionProperty(inRemoteFolderURL) == false )
	{
		return false;
	}
	
	//���O�o�� #1231
	AText	log("Connecting ");
	log.AddText(inRemoteFolderURL);
	log.AddCstring("...(LIST)\n");
	GetApp().SPI_GetFTPLogWindow().SPI_AddLog(log);
	
	//�v���^�C�v�L��
	mCurrentRequestType = inRequestType;
	//�����[�gURL�L�� #1231
	mRemoteURL.SetText(inRemoteFolderURL);
	//�t�B���K�[�v�����g�L�� #1231
	mFingerprint.SetText(inFingerprint);
	
	// #1231 if( mLegacyFTPMode == true )
	switch(mRemoteConnectionProtocolType)
	{
	  case kRemoteConnectionProtocolType_FTP:
		{
			//==================���K�V�[FTP�ڑ���==================
			//FTP�X���b�hRun�i�f�B���N�g���擾�j
			GetFTPConnection().SPNVI_Run_GetDirectory(inRemoteFolderURL);
			break;
		}
		//
	  case kRemoteConnectionProtocolType_AccessPlugin:
	  default:
		{
			//==================�v���O�C���ڑ���==================
			//���X�g�擾��e���|�����t�@�C���i�ꉞ�ݒ肷�邪�g��Ȃ��j
			AFileAcc	tmpFolder;
			AFileWrapper::GetTempFolder(tmpFolder);
			AFileAcc	tmpFile;
			tmpFile.SpecifyUniqChildFile(tmpFolder,"AccessPluginLocalTempFileForLIST");
			//#1231 tmpFile.CreateFile();
			tmpFile.GetPath(mLocalFilePath);
			//�v���O�C�����s
			ExecutePlugin("list");
			break;
		}
	}
	return true;
}

/**
GET�R�}���h�v��
*/
ABool	ARemoteConnection::ExecuteGET( const ARemoteConnectionRequestType inRequestType, const AText& inRemoteFileURL, const ABool inAllowNoFile )
{
	//���̃R�}���h�v�����͉��������I���i����̗v���͖��������j
	if( mCurrentRequestType != kRemoteConnectionRequestType_None )
	{
		return false;
	}
	
	//�ڑ����ݒ�i�T�[�o�[�E���[�U�[������Ή�����FTP�A�J�E���g���������Ď擾�j #1231
	if( SetConnectionProperty(inRemoteFileURL) == false )
	{
		return false;
	}
	
	//���O�o�� #1231
	AText	log("Connecting ");
	log.AddText(inRemoteFileURL);
	log.AddCstring("...(GET)\n");
	GetApp().SPI_GetFTPLogWindow().SPI_AddLog(log);
	
	//�v���^�C�v�L��
	mCurrentRequestType = inRequestType;
	//�����[�g�t�@�C���p�X�L��
	mRemoteURL.SetText(inRemoteFileURL);
	//�t�B���K�[�v�����g�L�� #1231
	mFingerprint.SetText(GetEmptyText());
	//�t�@�C�����Ȃ��Ă�OK�Ƃ݂Ȃ����ǂ����̃t���O�i�V�K�t�@�C���p�j #1231
	mAllowNoFile = inAllowNoFile;
	
	//#1231 if( mLegacyFTPMode == true )
	switch(mRemoteConnectionProtocolType)
	{
		//
	  case kRemoteConnectionProtocolType_FTP:
		{
			//==================���K�V�[FTP�ڑ���==================
			//FTP�X���b�hRun�i�t�@�C�����e�擾�j
			GetFTPConnection().SPNVI_Run_GetFile(inRemoteFileURL,inAllowNoFile);
			break;
		}
		//
	  case kRemoteConnectionProtocolType_AccessPlugin:
	  default:
		{
			//==================�v���O�C���ڑ���==================
			//�t�@�C���擾��e���|�����t�@�C��
			AFileAcc	tmpFolder;
			AFileWrapper::GetTempFolder(tmpFolder);
			//�t�H���_���쐬���ăt�@�C�����d����h�~����BGET���ʂ̓t�@�C���L���Ŕ���B#1249 #1231�Ń��[�J���t�@�C���𐶐����Ȃ��悤�ɂ������ʃt�@�C�������d�����Ă��܂�����΍�B
			AFileAcc	tmpFileFolder;
			{
				AStMutexLocker	locker(sTempFileFolderCreateMutex);
				tmpFileFolder.SpecifyUniqChildFile(tmpFolder,"AccessPluginLocalTempFileForGET");
				//#1231 ���[�J���t�@�C���͐������Ȃ��B�v���O�C���Ő���������B���ʎ�M���Ƀt�@�C�����Ȃ�������G���[�Ƃ݂Ȃ��BtmpFile.CreateFile();
				tmpFileFolder.CreateFolder();
			}
			AFileAcc	tmpFile;
			AText	child("data");
			tmpFile.SpecifyChild(tmpFileFolder,child);
			tmpFile.GetPath(mLocalFilePath);
			//�v���O�C�����s
			ExecutePlugin("get");
		}
	}
	return true;
}
//GET�p�e���|�����t�@�C���t�H���_�����pMutex #1249
AThreadMutex	ARemoteConnection::sTempFileFolderCreateMutex;


/**
PUT�R�}���h�v��
*/
ABool	ARemoteConnection::ExecutePUT( const ARemoteConnectionRequestType inRequestType, const AText& inRemoteFileURL,
		const AText& inText )
{
	//���̃R�}���h�v�����͉��������I���i����̗v���͖��������j
	if( mCurrentRequestType != kRemoteConnectionRequestType_None )
	{
		return false;
	}
	
	//�ڑ����ݒ�i�T�[�o�[�E���[�U�[������Ή�����FTP�A�J�E���g���������Ď擾�j #1231
	if( SetConnectionProperty(inRemoteFileURL) == false )
	{
		return false;
	}
	
	//���O�o�� #1231
	AText	log("Connecting ");
	log.AddText(inRemoteFileURL);
	log.AddCstring("...(PUT)\n");
	GetApp().SPI_GetFTPLogWindow().SPI_AddLog(log);
	
	//�v���^�C�v�L��
	mCurrentRequestType = inRequestType;
	//�����[�g�t�@�C���p�X�L��
	mRemoteURL.SetText(inRemoteFileURL);
	//�t�B���K�[�v�����g�L�� #1231
	mFingerprint.SetText(GetEmptyText());
	
	//#1231 if( mLegacyFTPMode == true )
	switch(mRemoteConnectionProtocolType)
	{
		//
	  case kRemoteConnectionProtocolType_FTP:
		{
			//==================���K�V�[FTP�ڑ���==================
			//FTP�X���b�hRun�i�t�@�C�����e�ۑ��j
			//����A�������s���Ă���B���̂��߁A�����Ƀv���O�C���ڑ������E�v���^�C�v�N���A����
			GetFTPConnection().SPNVI_Run_PutFile(inRemoteFileURL,inText);
			//�v���^�C�v�N���A
			mCurrentRequestType = kRemoteConnectionRequestType_None;
			break;
		}
		//
	  case kRemoteConnectionProtocolType_AccessPlugin:
	  default:
		{
			//==================�v���O�C���ڑ���==================
			//�t�@�C���A�b�v���[�h���e���|�����t�@�C��
			AFileAcc	tmpFolder;
			AFileWrapper::GetTempFolder(tmpFolder);
			AFileAcc	tmpFile;
			tmpFile.SpecifyUniqChildFile(tmpFolder,"AccessPluginLocalTempFileForPUT");
			tmpFile.CreateFile();
			tmpFile.WriteFile(inText);
			tmpFile.GetPath(mLocalFilePath);
			//�v���O�C�����s
			ExecutePlugin("put");
			break;
		}
	}
	return true;
}

/**
���݂̗v�����A�{�[�g
*/
void	ARemoteConnection::AbortCurrentRequest()
{
	//#1231 if( mLegacyFTPMode == true )
	switch(mRemoteConnectionProtocolType)
	{
		//
	  case kRemoteConnectionProtocolType_FTP:
		{
			//==================���K�V�[FTP�ڑ�==================
			//
			GetFTPConnection().NVI_AbortThread();
			break;
		}
	  default:
		{
			//������
		}
	}
	//���݂̗v���^�C�v���N���A
	mCurrentRequestType = kRemoteConnectionRequestType_None;
}

#pragma mark ===========================

#pragma mark ---�R�}���h������M������

/**
�R�}���h������M�������i���s���̃R�}���h�^�C�v�ɏ]���ď�������j
*/
void	ARemoteConnection::DoResponseReceived( const AText& inText, const AText& inStderrText )//#1231
{
	//���ʃe�L�X�g
	AText	resultText;
	//�e���|�����t�@�C���擾
	AFileAcc	tmpFile;
	tmpFile.Specify(mLocalFilePath);
	//
	if( mRemoteConnectionProtocolType != kRemoteConnectionProtocolType_FTP &&
		mCurrentRequestType == kRemoteConnectionRequestType_GETforOpenDocument )//#1231 LIST�͑S�ĕW���o�͂���擾����悤�ɂ���i�t�@�C���Ɉ�U�o�͂���͖̂�����p�X���[�h���W���o�͂ɏo�Ă��܂����Ƃ��ɃZ�L�����e�B���낵���Ȃ��j
	{
		//FTP�ȊO�ŁAGET�����̏ꍇ
		//�e���|�����t�@�C�����猋�ʃe�L�X�g�ǂݍ���
		//���[�J���t�@�C�������݂��Ă��Ȃ�������G���[ #1231
		if( tmpFile.Exist() == false )
		{
			if( mAllowNoFile == false )
			{
				//�G���[�_�C�A���O�\��
				AText	message1, message2;
				message1.SetLocalizedText("RemoteAccess_ConnectError1");
				message2.SetLocalizedText("RemoteAccess_ConnectError2");
				message2.ReplaceParamText('0',inStderrText);
				GetApp().NVI_ShowModalAlertWindow(message1,message2);
				//�v���^�C�v�N���A
				mCurrentRequestType = kRemoteConnectionRequestType_None;
				//�I��
				return;
			}
		}
		//�e���|�����t�@�C���ǂݍ���
		tmpFile.ReadTo(resultText);
	}
	else
	{
		//���ʃe�L�X�g�ɕW���o�̓e�L�X�g��ݒ�
		resultText.SetText(inText);
	}
	//�e���|�����t�@�C�������݂��Ă�����폜����B
	if( tmpFile.Exist() == true )
	{
		tmpFile.DeleteFile();
	}
	//���O�o�� #1231
	if( mRemoteConnectionProtocolType != kRemoteConnectionProtocolType_FTP )
	{
		AText	log(inStderrText);
		log.AddCstring("\n");
		GetApp().SPI_GetFTPLogWindow().SPI_AddLog(log);
	}
	//
	switch(mCurrentRequestType)
	{
		//�t�@�C�����X�g�E�C���h�E����̃f�B���N�g�����X�g�v����
	  case kRemoteConnectionRequestType_LISTfromFileListWindow:
		{
			//#1231 if( mLegacyFTPMode == true )
			switch(mRemoteConnectionProtocolType)
			{
			  case kRemoteConnectionProtocolType_FTP:
				{
					//==================���K�V�[FTP�ڑ���==================
					//FTP�ڑ��I�u�W�F�N�g���猋�ʎ擾
					ATextArray	textArray;
					ABoolArray	isDirectoryArray;
					GetFTPConnection().SPI_GetDirectoryResult(textArray,isDirectoryArray);
					//�t�@�C�����X�g�E�C���h�E�Ɍ��ʐݒ�
					GetApp().SPI_GetFileListWindow(mRequestorObjectID).SPI_SetRemoteConnectionResult(textArray,isDirectoryArray);
					break;
				}
			  default:
				{
					//==================curl/�v���O�C���ڑ���==================
					//LF�ŋ�؂�e�s�ǂݍ���
					ATextArray	textArray;
					textArray.ExplodeFromText(resultText,'\n');
					//�t�@�C�����A�f�B���N�g�����ǂ����̃t���O�̔z��
					ATextArray	fileNameArray;
					ABoolArray	isDirectoryArray;
					//�t�B���K�[�v�����g�����񒊏o�p���K�\��
					ARegExp	regexp_fingerprint("^.+fingerprint.+:(.+)\.");
					AText	fingerprint;
					//�e�s���Ƃ̃��[�v
					for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
					{
						AText	text;
						textArray.Get(i,text);
						//SFTP��p����
						if( mConnectionType.Compare("SFTP") == true )
						{
							//mFingerprint����i���V�K�T�[�o�[�ڑ��_�C�A���O�Łu�ڑ�����v��I�񂾏ꍇ�ł͂Ȃ��j�ł��邱�Ƃ̔���
							if( mFingerprint.GetItemCount() == 0 )
							{
								//�t�B���K�[�v�����g���o
								if( regexp_fingerprint.Match(text) == true )
								{
									regexp_fingerprint.GetMatchedText(text,fingerprint);
								}
								//known_hosts�ւ̓o�^�Ȃ�����
								//known_hosts�ւ̓o�^���Ȃ��Ƃ����G���[���o�����́A�t�B���K�[�v�����g�ƂƂ��Ƀ��b�Z�[�W���o���āA
								//�V�K�ɐڑ���known_hosts�֒ǉ����ėǂ����ǂ�����q�˂�B���q�˂��̂��ēxLIST���N�G�X�g
								if( text.FindCstring("Are you sure you want to continue connecting (yes/no)?") == true )
								{
									//�V�K�T�[�o�[�ɐڑ����邩�ǂ����̃_�C�A���O��\��
									AText	message1, message2;
									message1.SetLocalizedText("SFTP_NewServer1");
									message2.SetLocalizedText("SFTP_NewServer2");
									message2.ReplaceParamText('1',fingerprint);
									AText	okButton, cancelButton;
									okButton.SetLocalizedText("SFTP_NewServer_OK");
									cancelButton.SetLocalizedText("SFTP_NewServer_Cancel");
									if( GetApp().NVI_ShowModalCancelAlertWindow(message1,message2,okButton,cancelButton) == true )
									{
										//�ڑ�����ꍇ
										//�v���^�C�v�N���A
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
							//�ŏ��̕�����d�̏ꍇ�i�f�B���N�g���j
							if( firstch == 'd' )
							{
								AText	filename;
								if( text.ExtractUsingRegExp("^d\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+\\S+\\s+(\\S.+)$",1,filename) == true )
								{
									fileNameArray.Add(filename);
									isDirectoryArray.Add(true);
								}
							}
							//�ŏ��̕�����-�̏ꍇ�i�t�@�C���j
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
					//���X�g���ʂ��t�@�C�����X�g�E�C���h�E�ɐݒ�
					GetApp().SPI_GetFileListWindow(mRequestorObjectID).SPI_SetRemoteConnectionResult(fileNameArray,isDirectoryArray);
					//LIST���ʂ��Ȃ��ꍇ�̓G���[�Ƃ݂Ȃ��A�G���[�_�C�A���O�\���i����̏ꍇ�́A.��..�����݂���͂��B�j #1236
					if( fileNameArray.GetItemCount() == 0 )
					{
						//�G���[�_�C�A���O�\��
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
		//�h�L�������g���J�����߂�GET
	  case kRemoteConnectionRequestType_GETforOpenDocument:
		{
			/*#1231 
			if( mLegacyFTPMode == true )
			{
				//==================���K�V�[FTP�ڑ���==================
				//�����[�g�t�@�C���p�h�L�������g����
				GetApp().SPNVI_CreateDocumentFromRemoteFile(GetObjectID(),mRemoteURL,inText);
			}
			else
			{
				//==================�v���O�C���ڑ���==================
				//�e���|�����t�@�C������e�L�X�g�ǂݍ���
				AFileAcc	tmpFile;
				tmpFile.Specify(mLocalFilePath);
				AText	text;
				tmpFile.ReadTo(text);
				tmpFile.DeleteFile();
			*/
			//�����[�g�t�@�C���p�h�L�������g����
			GetApp().SPNVI_CreateDocumentFromRemoteFile(GetObjectID(),mRemoteURL,resultText);
			/*#1231
			}
			*/
			break;
		}
		//�h�L�������g�ۑ�PUT
	  case kRemoteConnectionRequestType_PUTfromTextDocument:
		{
			//�����Ȃ�
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//�v���^�C�v�N���A
	mCurrentRequestType = kRemoteConnectionRequestType_None;
}

