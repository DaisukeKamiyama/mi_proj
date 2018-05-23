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

AFTP

*/

#include "stdafx.h"

#include "AFTP.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AFTPConnection
#pragma mark ===========================
//FTP�ڑ�����

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AFTPConnection::AFTPConnection( AObjectArrayItem* inParent ) : AThread(inParent), mBinaryMode(true), mLastPutFileOK(false)
,mRemoteConnectionObjectID(kObjectID_Invalid)
,mPort(21)
{
}

//#361
/**
�����[�g�ڑ��I�u�W�F�N�g�ݒ�
*/
void	AFTPConnection::SPI_SetRemoteConnectionObjectID( const AObjectID inRemoteConnectionObjectID )
{
	mRemoteConnectionObjectID = inRemoteConnectionObjectID;
}

#pragma mark ===========================

#pragma mark ---�X���b�h���f

//�X���b�h���f
void	AFTPConnection::NVIDO_AbortThread()
{
	if( mProcessType == kFTPConnectionProcessType_GetDirectory || mProcessType == kFTPConnectionProcessType_GetFile )
	{
		//connect���̃u���b�N����߂����߂Ƀ\�P�b�g�������N���[�Y����
		//�t�H���_�^�t�@�C���擾���̋����N���[�Y�̓t�@�C���j�󓙂ɂ͂Ȃ�Ȃ��B�܂��A�����N���[�Y�͐������Ɠ����ł���A������ɂ��Ă��ُ�n�Ƃ��č�荞�܂�Ă����ׂ�����ł���B
		//�t�@�C���ۑ����̓N���[�Y���Ȃ��B�i��������UI�Œ��f�s�����A�O�̂��߂����ł����������Ă����j
		mDataSocket.Close();
		mControlSocket.Close();
		mListenSocket.Close();
		//�����K�v�F�����̓��C���X���b�h��ɂȂ�
	}
}

#pragma mark ===========================

#pragma mark ---�t�H���_�擾

/**
�t�H���_�擾�X���b�h�N���iFTP�t�H���_���j���[�p�j
*/
void	AFTPConnection::SPNVI_Run_GetDirectoryForFTPFolder( const AText& inFTPURL, const ANumber inDepth, const AText& inActionText )
{
	//�X���b�h�N�����͓����C���X�^���X�ł̏d���N���s��
	if( NVI_IsThreadWorking() == true )   return;
	
	//ProcessType�ݒ�
	mProcessType = kFTPConnectionProcessType_GetDirectory;
	
	//
	InitByFTPURL(inFTPURL);
	mFTPFolderActionText.SetText(inActionText);
	mFTPFolderDepth = inDepth;
	mFTPFolderMenuObjectID = GetApp().NVI_GetMenuManager().CreateDynamicMenu(kMenuItemID_FTPFolder,false);
	
	//�v���O���X�E�C���h�E�̃^�C�g���i�t�H���_�擾���j�ݒ�i�����͂܂��X���b�h�O�Ȃ̂Œ��ڃR�[��OK�j
	AText	text;
	text.SetLocalizedText("FTPProgress_GetDirectory");
	GetApp().SPI_GetFTPProgress().SPNVI_Show(true);
	GetApp().SPI_GetFTPProgress().SPI_SetTitle(text);
	GetApp().SPI_GetFTPProgress().SPI_SetProgressText(mFTPURL);
	
	//�X���b�h�N��
	NVI_Run();
}

/**
�t�H���_�擾�X���b�h�N���i�t�@�C�����X�g�p�j
*/
void	AFTPConnection::SPNVI_Run_GetDirectory( const AText& inFTPURL )
{
	//�X���b�h�N�����͓����C���X�^���X�ł̏d���N���s��
	if( NVI_IsThreadWorking() == true )   return;
	
	//ProcessType�ݒ�
	mProcessType = kFTPConnectionProcessType_GetDirectory;
	
	//�v���f�[�^�ݒ�
	InitByFTPURL(inFTPURL);
	mFTPFolderActionText.DeleteAll();
	mFTPFolderDepth = 1;
	mFTPFolderMenuObjectID = kObjectID_Invalid;//�t�@�C�����X�g�p������
	
	//�X���b�h�N��
	NVI_Run();
}

/**
�t�H���_�擾���ʎ擾
*/
void	AFTPConnection::SPI_GetDirectoryResult( ATextArray& outFileNameArray, ABoolArray& outIsDirectoryArray ) const
{
	outFileNameArray.SetFromTextArray(mDirectoryNameList);
	outIsDirectoryArray.SetFromObject(mIsDirectoryList);
}

//�t�H���_�擾�X���b�h���C�����[�`��
void	AFTPConnection::ThreadMain_GetDirectory()
{
	try
	{
		if( mFTPFolderMenuObjectID == kObjectID_Invalid )
		{
			//==================�t�@�C�����X�g�p==================
			//���O�C��
			Login();
			//�f�B���N�g���擾
			GetDirectoryRecursive(mPath,1,kObjectID_Invalid);
			//�A�{�[�g����ĂȂ���΃��C���X���b�h�Ɋ����ʒm
			if( NVM_IsAborted() == false )
			{
				AObjectIDArray	objectIDArray;
				objectIDArray.Add(mRemoteConnectionObjectID);
				ABase::PostToMainInternalEventQueue(kInternalEvent_FTPDirectoryResult,GetObjectID(),0,GetEmptyText(),objectIDArray);
			}
		}
		else
		{
			//==================FTP�t�H���_���j���[�p==================
			//
			ProgressPercent(0);
			//
			Login();
			//
			GetDirectoryRecursive(mPath,1,mFTPFolderMenuObjectID);
			//
			ProgressPercent(100);
			
			//
			if( NVM_IsAborted() == false )
			{
				//
				AObjectIDArray	objectIDArray;
				objectIDArray.Add(mFTPFolderMenuObjectID);
				ABase::PostToMainInternalEventQueue(kInternalEvent_FTPFolder,GetObjectID(),0,mFTPFolderActionText,objectIDArray);
			}
			//�v���O���X�E�C���h�E�����i�C�x���g�𓊂���j
			HideProgress();
		}
	}
	catch( AConstCharPtr inErrorString )
	{
		ShowError(inErrorString);
	}
	//�\�P�b�g�N���[�Y�i���ɃN���[�Y����Ă���ꍇ�����邪�A����̓\�P�b�g�I�u�W�F�N�g���Ŕ��f�j
	mControlSocket.Close();
	mDataSocket.Close();
	mListenSocket.Close();
	//�Z�L�����e�B��A�p�X���[�h���A�v���������ԕێ�����̂͗ǂ������̂ŖY���B
	mPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
	mProxyPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
}

//
void	AFTPConnection::GetDirectoryRecursive( const AText& inPath, const ANumber inDepth, const AObjectID inDynamicMenuObjectID )
{
	//�X���b�h���f�`�F�b�N
	if( NVM_IsAborted() == true )   return;
	
	AText	path;
	path.SetText(inPath);
	if( path.GetLastChar() != '/' )   path.Add('/');
	
	AText	text;
	ANumber	responceCode;
	
	ProgressText(path);
	
	//#361 ATextArray	mDirectoryNameList;
	//#361 AArray<ABool>	mDirectoryIsDirectoryList;
	
	//���ʊi�[�ϐ�������
	mDirectoryNameList.DeleteAll();
	mIsDirectoryList.DeleteAll();
	
	// ========= NLST =========
	{
		//�f�[�^�|�[�g�ڑ�
		ConnectOrListenDataPort();
		
		//NLST���M
		text.SetCstringAndText("NLST ",path,"\r\n");
		mControlSocket.Send(text);
		_Log(text,true);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode != kDataConnectionAlreadyOpen && responceCode != kFileStatusOK ) 
		{
			//�f�[�^�|�[�g�ؒf
			CloseDataPort();
		}
		else
		{
			//NLST�f�[�^��M
			if( mPassive == false )   mDataSocket.Accept(mListenSocket);
			AText	text_NLST;
			mDataSocket.ReceiveAll(text_NLST);
			_Log(text_NLST,false);
			
			//�f�[�^�|�[�g�ؒf
			CloseDataPort();
			
			//�f�[�^��M���������҂�
			Read_WaitFTPResponceCode(text,responceCode);
			_Log(text,false);
			if( responceCode != kClosingDataConnection && responceCode != kConnectionClosed )   _AThrow("FTP NLST data receive failed",NULL);
			
			// NLST�f�[�^���
			AIndex	p = 0;
			while( p < text_NLST.GetLength() )
			{
				AText	line;
				text_NLST.GetLine(p,line);
				if( line.GetLength() == 0 )   continue;
				AIndex	pos = line.GetLength()-1;//�s�̍Ōォ�猟��
				//'/'���A�s����������܂Ŗ߂�
				for( ; pos >= 0; pos-- )
				{
					unsigned char	ch = line.Get(pos);
					if( ch == '/' )   break;
				}
				pos++;
				//�߂����ʒu����Ō�܂ł��t�@�C���^�t�H���_���ƌ��Ȃ�
				AText	name;
				line.GetText(pos,line.GetLength()-pos,name);
				mDirectoryNameList.Add(name);
				mIsDirectoryList.Add(false);
			}
		}
	}
	mDirectoryNameList.Sort(true);
	
	// ========= LIST =========
	//�f�B���N�g�����ǂ����̔���
	{
		//�f�[�^�|�[�g�ڑ�
		ConnectOrListenDataPort();
		
		//LIST���M
		text.SetCstringAndText("LIST ",path,"\r\n");
		mControlSocket.Send(text);
		_Log(text,true);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode != kDataConnectionAlreadyOpen && responceCode != kFileStatusOK )   _AThrow("FTP LIST command error",NULL);
		
		//LIST�f�[�^��M
		if( mPassive == false )   mDataSocket.Accept(mListenSocket);
		AText	text_LIST;
		mDataSocket.ReceiveAll(text_LIST);
		_Log(text_LIST,false);
		
		//�f�[�^�|�[�g�ؒf
		CloseDataPort();
		
		//�f�[�^��M���������҂�
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode != kClosingDataConnection && responceCode != kConnectionClosed )   throw "FTP LIST data receive failed";
		
		//LIST���ʉ��
		//#874 text_LIST.FindText()�Ɏ��Ԃ������邽��NLST��LIST�������g���ꍇ�̏�����ǉ��i1000���ږ����j
		if( mDirectoryNameList.GetItemCount() > 0 && mDirectoryNameList.GetItemCount() < 1000 )
		{
			//NLST���ʂ���̏ꍇ�ALIST���ʂ���f�B���N�g�����ۂ��̏��݂̂𔲂��o��
			if( text_LIST.GetLength() > 0 )
			{
				for( AIndex i = 0; i < mDirectoryNameList.GetItemCount(); i++ )
				{
					AText	name;
					mDirectoryNameList.Get(i,name);
					for( AIndex pos = 0; pos < text_LIST.GetItemCount();  )
					{
						AIndex	foundpos;
						if( text_LIST.FindText(pos,name,foundpos) == false )   break;
						pos = foundpos + name.GetItemCount();
						
						AIndex	p = foundpos + name.GetItemCount();
						ABool	notlast = false;
						for( ; p < text_LIST.GetItemCount(); p++ )
						{
							AUChar	ch = text_LIST.Get(p);
							if( ch == kUChar_CR || ch == kUChar_LF )  break;
							else if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
							else
							{
								notlast = true;
								break;
							}
						}
						if( notlast == true )   continue;
						p = foundpos;
						for( ; p >= 0; p-- )
						{
							AUChar	ch = text_LIST.Get(p);
							if( ch == kUChar_CR || ch == kUChar_LF )   break;
						}
						p++;
						AUChar	ch = text_LIST.Get(p);
						if( ch == 'd' )
						{
							mIsDirectoryList.Set(i,true);
						}
						break;
					}
				}
			}
		}
		else
		{
			//NLST���ʂȂ��̏ꍇ�ALIST���ʂ���A�t�@�C���^�t�H���_���A����сA�f�B���N�g�����ۂ��̏��𔲂��o��
			AIndex	p = 0;
			while( p < text_LIST.GetLength() )
			{
				AText	line;
				text_LIST.GetLine(p,line);
				if( line.GetLength() < 15 )   continue;
				unsigned char	ch1 = line.Get(0);
				if( ( ch1 == 'd' || ch1 == '-' ) == false )   continue;
				AText	name;
				AIndex	pos = line.GetLength()-1;//�s�̍Ōォ�猟��
				//�Ō�̃X�y�[�X�^�^�u���΂�
				for( ; pos >= 0; pos-- )
				{
					unsigned char	ch =  line.Get(pos);
					if( ch != A_CHAR_SPACE && ch != A_CHAR_TAB )   break;
				}
				//�X�y�[�X�^�^�u��������܂�name�Ɋi�[
				for( ; pos >= 0; pos-- )
				{
					unsigned char	ch =  line.Get(pos);
					if( ch == A_CHAR_SPACE || ch == A_CHAR_TAB )   break;
					name.Insert1(0,ch);
				}
				if( name.GetLength() == 0 )   continue;
				if( name.Compare(".") == true )   continue;
				if( name.Compare("..") == true )   continue;
				mDirectoryNameList.Add(name);
				mIsDirectoryList.Add(ch1=='d');
			}
		}
	}
	
	/*#193
	//==================FTP�t�H���_���j���[�̏ꍇ==================
	//���j���[�f�[�^�\��
	if( inDynamicMenuObjectID != kObjectID_Invalid )
	{
		//���j���[�f�[�^�\��
		for( AIndex i = 0; i < mDirectoryNameList.GetItemCount(); i++ )
		{
			if( inDepth == 1 )
			{
				ProgressPercent((i+1)*100/mDirectoryNameList.GetItemCount());
			}
			AText	name;
			mDirectoryNameList.Get(i,name);
			if( name.GetLength() == 0 )   continue;
			if( name.Compare(".") == true )   continue;
			if( name.Compare("..") == true )   continue;
			AText	filepath;
			filepath.SetText(path);
			filepath.AddText(name);
			if( mIsDirectoryList.Get(i) == true )//�f�B���N�g��
			{
				if( inDepth+1 <= mFTPFolderDepth )
				{
					AObjectID	dynamicMenuObjectID = GetApp().NVI_GetMenuManager().CreateDynamicMenu(kMenuItemID_FTPFolder,false);
					GetDirectoryRecursive(filepath,inDepth+1,dynamicMenuObjectID);
					GetApp().NVI_GetMenuManager().AddDynamicMenuItemByObjectID(inDynamicMenuObjectID,name,name,dynamicMenuObjectID,0,0,false);
				}
				else
				{
					GetApp().NVI_GetMenuManager().AddDynamicMenuItemByObjectID(inDynamicMenuObjectID,name,filepath,kObjectID_Invalid,0,0,false);
				}
			}
			else
			{
				AText	url;
				url.SetFTPURL(mServer,mUser,filepath);
				GetApp().NVI_GetMenuManager().AddDynamicMenuItemByObjectID(inDynamicMenuObjectID,name,url,kObjectID_Invalid,0,0,false);
			}
		}
		//separator
		GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByObjectID(inDynamicMenuObjectID,0,GetEmptyText(),GetEmptyText(),kObjectID_Invalid,0,0,true);
		//�V�K�t�@�C��
		AText	menutext;
		menutext.SetLocalizedText("FTPFolder_NewFile");
		AText	action;
		action.SetFTPURL(mServer,mUser,path);
		action.Insert1(0,'N');
		GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByObjectID(inDynamicMenuObjectID,0,menutext,action,kObjectID_Invalid,0,0,false);
		//�t�H���_�X�V
		if( inDepth == 1 )
		{
			AText	menutext;
			menutext.SetLocalizedText("FTPFolder_Update");
			AText	action;
			action.SetText(mFTPFolderActionText);
			action.Set(0,'U');
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByObjectID(inDynamicMenuObjectID,0,menutext,action,kObjectID_Invalid,0,0,false);
		}
	}
	*/
	
	// LIST���ʉ��
	//��F
	//drwxr-xr-x  63 root     wheel         2142 Oct  7 11:30 sbin
	//-rw-r--r--   1 daisuke  admin        42270 Feb 10  2006 test_orig.html
	//lrwxr-xr-x   1 root     admin           11 Jan  5  2006 tmp -> private/tmp

	/*AIndex	p = 0;
	while( p < text_LIST.GetLength() )
	{
		AText	line;
		text_LIST.GetLine(p,line);
		if( inDepth == 1 )
		{
			ProgressPercent(p*100/text_LIST.GetLength()-1);
		}
		if( line.GetLength() < 15 )   continue;
		unsigned char	ch1 = line.Get(0);
		if( not( ch1 == 'd' || ch1 == '-' ) )   continue;
		AText	name;
		AIndex	pos = line.GetLength()-1;//�s�̍Ōォ�猟��
		//�Ō�̃X�y�[�X�^�^�u���΂�
		for( ; pos >= 0; pos-- )
		{
			unsigned char	ch =  line.Get(pos);
			if( ch != A_CHAR_SPACE && ch != A_CHAR_TAB )   break;
		}
		//�X�y�[�X�^�^�u��������܂�name�Ɋi�[
		for( ; pos >= 0; pos-- )
		{
			unsigned char	ch =  line.Get(pos);
			if( ch == A_CHAR_SPACE || ch == A_CHAR_TAB )   break;
			name.Insert(0,&ch,1);
		}
		if( name.GetLength() == 0 )   continue;
		if( name.Compare(".") == true )   continue;
		if( name.Compare("..") == true )   continue;
		AText	filepath;
		filepath.SetText(path);
		filepath.AddText(name);
		if( ch1 == 'd' )//�f�B���N�g��
		{
			if( inDepth+1 <= mFTPFolderDepth )
			{
				ADynamicMenu*	menu = new ADynamicMenu(kMenuItemID_FTPFolderItem);
				GetDirectoryRecursive(filepath,menu,inDepth+1);
				ioMenu->Add(name,kEmptyText,menu,true);
			}
			else
			{
				ioMenu->Add(name,filepath,NULL,false);
			}
		}
		else if( ch1 == '-' )//�t�@�C��
		{
			AText	url;
			url.SetFTPURL(mServer,mUser,filepath);
			ioMenu->Add(name,url,NULL,true);
		}
	}*/
}

#pragma mark ===========================

#pragma mark --- �t�@�C���擾

//
void	AFTPConnection::SPNVI_Run_GetFile( const AText& inFTPURL, const ABool inAllowNoFile )
{
	//�X���b�h�N�����͓����C���X�^���X�ł̏d���N���s��
	if( NVI_IsThreadWorking() == true )   return;
	
	//
	mGetFileAllowNoFile = inAllowNoFile;
	
	//ProcessType�ݒ�
	mProcessType = kFTPConnectionProcessType_GetFile;
	
	InitByFTPURL(inFTPURL);
	//#361 mGetFileReceiverObjectID = inReceiverObjectID;
	
	//�v���O���X�E�C���h�E�̃^�C�g���i�t�@�C���擾���j�ݒ�i�����͂܂��X���b�h�O�Ȃ̂Œ��ڃR�[��OK�j
	AText	text;
	text.SetLocalizedText("FTPProgress_GetFile");
	GetApp().SPI_GetFTPProgress().SPNVI_Show(false);
	GetApp().SPI_GetFTPProgress().SPI_SetTitle(text);
	GetApp().SPI_GetFTPProgress().SPI_SetProgressText(mFTPURL);
	
	//�X���b�h�N��
	NVI_Run();
}

void	AFTPConnection::ThreadMain_GetFile()
{
	try
	{
		//
		ProgressPercent(-1);
		
		Login();
		
		AText	text;
		ANumber	responceCode;
		
		//TYPE���M
		if( mBinaryMode )
		{
			text.SetCstring("TYPE I\r\n");
		}
		else
		{
			text.SetCstring("TYPE A\r\n");
		}
		mControlSocket.Send(text);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,true);
		
		//�f�[�^�|�[�g�ڑ�
		ConnectOrListenDataPort();
		
		//RETR���M
		text.SetCstring("RETR ");
		text.AddText(mPath);
		text.AddCstring("\r\n");
		mControlSocket.Send(text);
		_Log(text,true);
		Read_WaitFTPResponceCode(text,responceCode);
		ABool	fileExist = true;
		if( responceCode != kDataConnectionAlreadyOpen && responceCode != kFileStatusOK )
		{
			if( mGetFileAllowNoFile == true )
			{
				fileExist = false;
			}
			else
			{
				_AThrow("FTP RETR command error",NULL);
			}
		}
		_Log(text,false);
		
		if( fileExist == true )
		{
			//RETR�f�[�^��M
			if( mPassive == false )   mDataSocket.Accept(mListenSocket);
			mDataSocket.ReceiveAll(mReceivedText);//#361 GetApp().SPI_GetThreadDataReceiver_Text(mGetFileReceiverObjectID));
			
			//�f�[�^�|�[�g�ؒf
			CloseDataPort();
			
			//�f�[�^��M���������҂�
			Read_WaitFTPResponceCode(text,responceCode);
			if( responceCode/100 != 2 )   _AThrow("FTP RETR data receive failed",NULL);
			_Log(text,false);
		}
		
		//�A�{�[�g����ĂȂ���΃��C���X���b�h�Ɋ����ʒm
		if( NVM_IsAborted() == false )
		{
			AObjectIDArray	objectIDArray;
			//#361 objectIDArray.Add(mGetFileReceiverObjectID);
			objectIDArray.Add(mRemoteConnectionObjectID);//#361
			//#193 AText	url;
			//#193 url.SetFTPURL(mServer,mUser,mPath,mPort);//#193
			ABase::PostToMainInternalEventQueue(kInternalEvent_FTPOpen,GetObjectID(),0,/*#361 url*/mReceivedText,objectIDArray);
		}
		
		//�v���O���X�E�C���h�E�����i�C�x���g�𓊂���j
		HideProgress();
	}
	catch( AConstCharPtr inErrorString )
	{
		ShowError(inErrorString);
	}
	//�\�P�b�g�N���[�Y�i���ɃN���[�Y����Ă���ꍇ�����邪�A����̓\�P�b�g�I�u�W�F�N�g���Ŕ��f�j
	mControlSocket.Close();
	mDataSocket.Close();
	mListenSocket.Close();
	//�Z�L�����e�B��A�p�X���[�h���A�v���������ԕێ�����̂͗ǂ������̂ŖY���B
	mPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
	mProxyPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
}

#pragma mark ===========================

#pragma mark --- �t�@�C���ۑ�

//�t�@�C���ۑ��X���b�h�N��
void	AFTPConnection::SPNVI_Run_PutFile( const AText& inFTPURL, const AText& inData )
{
	mLastPutFileOK = false;
	
	//�X���b�h�N�����͓����C���X�^���X�ł̏d���N���s��
	if( NVI_IsThreadWorking() == true )   return;
	
	//ProcessType�ݒ�
	mProcessType = kFTPConnectionProcessType_PutFile;
	
	//���M�f�[�^������o�b�t�@�ɃR�s�[�i�X���b�h�Ŏ��s����̂ŃR�s�[���K�v�j
	mSendData.SetText(inData);
	//
	AText	text;
	text.SetLocalizedText("FTPProgress_PutFile");
	GetApp().SPI_GetFTPProgress().SPNVI_Show(false);
	GetApp().SPI_GetFTPProgress().SPI_SetTitle(text);
	GetApp().SPI_GetFTPProgress().SPI_SetProgressText(mFTPURL);
	//
	InitByFTPURL(inFTPURL);
	//�X���b�h�N��
	//NVI_Run();
	//�X���b�h�͎g�킸�A�������s����
	ThreadMain_PutFile();
}

//�t�@�C���ۑ��X���b�h���C�����[�`��
void	AFTPConnection::ThreadMain_PutFile()
{
	try
	{
		ProgressPercent(-1);
		
		Login();
		
		AText	text;
		ANumber	responceCode;
		
		//TYPE���M
		if( mBinaryMode )
		{
			text.SetCstring("TYPE I\r\n");
		}
		else
		{
			text.SetCstring("TYPE A\r\n");
		}
		mControlSocket.Send(text);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,true);
		
		//�f�[�^�|�[�g�ڑ�
		ConnectOrListenDataPort();
		
		//STOR���M
		text.SetCstring("STOR ");
		text.AddText(mPath);
		text.AddCstring("\r\n");
		mControlSocket.Send(text);
		_Log(text,true);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode != kDataConnectionAlreadyOpen && responceCode != kFileStatusOK )   _AThrow("FTP STOR command error",NULL);
		
		//RETR�f�[�^��M
		if( mPassive == false )   mDataSocket.Accept(mListenSocket);
		mDataSocket.Send(mSendData);
		
		//�f�[�^�|�[�g�ؒf
		CloseDataPort();
		
		//�f�[�^���M�����҂�
		Read_WaitFTPResponceCode(text,responceCode);
		if( responceCode/100 != 2 )   _AThrow("FTP STOR data receive failed",NULL);
		_Log(text,false);
		
		HideProgress();
		mLastPutFileOK = true;
	}
	catch( AConstCharPtr inErrorString )
	{
		ShowError(inErrorString);
	}
	//�\�P�b�g�N���[�Y�i���ɃN���[�Y����Ă���ꍇ�����邪�A����̓\�P�b�g�I�u�W�F�N�g���Ŕ��f�j
	mControlSocket.Close();
	mDataSocket.Close();
	mListenSocket.Close();
	//�Z�L�����e�B��A�p�X���[�h���A�v���������ԕێ�����̂͗ǂ������̂ŖY���B
	mPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
	mProxyPassword.SetText("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
}

#pragma mark ===========================

#pragma mark ---���ʏ���

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`���i�f�B�X�p�b�`�j

//�X���b�h���C�����[�`���i�e�����X���b�h���C�����[�`���ւ̃f�B�X�p�b�`�̂݁j
void	AFTPConnection::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AFTPConnection::NVIDO_ThreadMain started.",this);
	switch(mProcessType)
	{
	  case kFTPConnectionProcessType_GetDirectory:
		{
			ThreadMain_GetDirectory();
			break;
		}
	  case kFTPConnectionProcessType_GetFile:
		{
			ThreadMain_GetFile();
			break;
		}
	  case kFTPConnectionProcessType_PutFile:
		{
			ThreadMain_PutFile();
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AFTPConnection::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark --- �R���g���[���|�[�g����

//����|�[�g�ڑ�
void	AFTPConnection::ConnectControlPort( const AText& inServer, const APortNumber inPort )
{
	AText	text;
	ANumber	responceCode;
	
	mControlSocket.Connect(inServer,inPort);
	Read_WaitFTPResponceCode(text,responceCode);
	_Log(text,false);
	if( responceCode != kServiceReady )   _AThrow("FTP service not ready",NULL);
}

//FTP�����R�[�h����M����܂Ńf�[�^����M���AoutText�Ɋi�[����
//FTP�����R�[�h����M����܂ł̓u���b�N����
void	AFTPConnection::Read_WaitFTPResponceCode( AText& outText, ANumber& outResponceCode )
{
	outText.DeleteAll();
	outResponceCode = 0;
	
	while(true)
	{
		//��M�L���[��ɂ���f�[�^����ɂȂ�܂ŁA��s���ƂɁA�L����FTP�����R�[�h�̗L���𒲂ׂ�B
		//�L��΁A���̎��_�Ńf�L���[����߁A���^�[������B
		//������΁A�\�P�b�g����̎�M�҂��u���b�N�ɓ���B
		AText	line;
		while( DequeueLine(line) == true )
		{
			outText.AddText(line);
			//FTP�����R�[�h�͍s���{3���̐����{�X�y�[�X (RFC959)
			if( line.GetItemCount() >= 4 )
			{
				char	ch1, ch2, ch3, ch4;
				ch1 = line.Get(0);
				ch2 = line.Get(1);
				ch3 = line.Get(2);
				ch4 = line.Get(3);
				if( ch1>='0' && ch1<='9' && 
						ch2>='0' && ch2<='9' && 
						ch3>='0' && ch3<='9' && 
						ch4 == ' ' )
				{
					outResponceCode = (ch1-'0')*100 + (ch2-'0')*10 + (ch3-'0');
					return;
				}
			}
		}
		//��M
		ABool	disconnected = false;
		mControlSocket.Receive(mControlSocketReadQueue,disconnected);
		if( disconnected == true )   _ACThrow("control socket disconnected",this);
	}
}

//ControlSocketReadQueue����P�s�ǂ݂���
//�i���s�R�[�h��CR, CRLF, LF�ǂ�ł����퓮�삷��j
ABool	AFTPConnection::DequeueLine( AText& outText )
{
	outText.DeleteAll();
	for( AIndex pos = 0; pos < mControlSocketReadQueue.GetLength(); pos++ )
	{
		AUChar ch1 = mControlSocketReadQueue.Get(pos);
		if( ch1 == kUChar_CR || ch1 == kUChar_LF )
		{
			pos++;
			if( pos < mControlSocketReadQueue.GetItemCount() )
			{
				AUChar ch2 = mControlSocketReadQueue.Get(pos);
				if( ch1 == kUChar_CR && ch2 == kUChar_LF )
				{
					pos++;
				}
			}
			mControlSocketReadQueue.GetText(0,pos,outText);
			mControlSocketReadQueue.Delete(0,pos);
			return true;
		}
	}
	return false;
}


//USER���M
void	AFTPConnection::SendUSER( const AText& inUser )
{
	AText	text;
	ANumber	responceCode;
	
	text.SetCstringAndText("USER ",inUser,"\r\n");
	mControlSocket.Send(text);
	_Log(text,true);
	Read_WaitFTPResponceCode(text,responceCode);
	_Log(text,false);
	if( responceCode != kUserNameOK )   _AThrow("FTP user name invalid",NULL);
}

//PASS���M
void	AFTPConnection::SendPASS( const AText& inPassword )
{
	AText	text;
	ANumber	responceCode;
	
	text.SetCstringAndText("PASS ",inPassword,"\r\n");
	mControlSocket.Send(text);
	Read_WaitFTPResponceCode(text,responceCode);
	//_Log(text);�p�X���[�h�͕\�����Ȃ�
	if( responceCode != kUserLoggedIn )   _AThrow("FTP password incorrect",NULL);
	text.SetCstring("PASS ********\r\n230 \r\n");
	_Log(text,false);
}

//SITE���M
void	AFTPConnection::SendSITE( const AText& inSite )
{
	AText	text;
	ANumber	responceCode;
	
	text.SetCstringAndText("SITE ",inSite,"\r\n");
	mControlSocket.Send(text);
	_Log(text,true);
	Read_WaitFTPResponceCode(text,responceCode);
	_Log(text,false);
	//if( responceCode !=  )   _AThrow("SITE failed");
}

//���O�C��
void	AFTPConnection::Login()
{
	AText	text;
	
	switch(mProxyMode)
	{
	  case AAppPrefDB::kProxyMode_NoProxy:
		{
			ConnectControlPort(mServer,mPort);
			SendUSER(mUser);
			SendPASS(mPassword);
			break;
		}
	  case AAppPrefDB::kProxyMode_FTPProxy1://USER remoteuser@remotehost->PASS remotepass
		{
			ConnectControlPort(mProxyServer,mPort);
			
			//remoteuser@remotehost
			AText	usertext;
			usertext.SetText(mUser);
			usertext.AddCstring("@");
			usertext.AddText(mServer);
			SendUSER(usertext);
			
			SendPASS(mPassword);
			break;
		}
	  case AAppPrefDB::kProxyMode_FTPProxy2://USER proxyuser->PASS proxypass->SITE remotehost->USER remoteuser->PASS remotepass
		{//���e�X�g
			ConnectControlPort(mProxyServer,mPort);
			SendUSER(mProxyUser);
			SendPASS(mProxyPassword);
			SendSITE(mServer);
			SendPASS(mPassword);
			break;
		}
	}
}

//FTPURL�������؂�o���A�����o�ϐ��֏����ݒ�
void	AFTPConnection::InitByFTPURL( const AText& inFTPURL )
{
	mFTPURL.SetText(inFTPURL);
	inFTPURL.GetFTPURLServer(mServer);
	inFTPURL.GetFTPURLUser(mUser);
	inFTPURL.GetFTPURLPath(mPath);
	/*#1231 �|�[�g�ԍ���FTP�A�J�E���g�ݒ肩��擾
	//#193
	mPort = inFTPURL.GetFTPURLPortNumber();
	if( mPort == 0 )
	{
		mPort = 21;
	}
	*/
	//
	mPassword.DeleteAll();
#if IMPLEMENTATION_FOR_MACOSX
	ASecurityWrapper::GetInternetPassword(mServer,mUser,mPassword);
#endif
	
	AIndex	index = GetApp().GetAppPref().FindFTPAccountPrefIndex(mServer,mUser);
	if( index != kIndex_Invalid )
	{
		//FTP�A�J�E���g�ݒ肩��e����擾
		//#193 mPort = static_cast<APortNumber>(GetApp().GetAppPref().GetData_NumberArray(AAppPrefDB::kFTPAccount_Port,index));
		//#1231
		mPort = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kFTPAccount_Port,index);
		if( GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kFTPAccount_UseDefaultPort,index) == true )
		{
			mPort = 21;
		}
		//
		mPassive = GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_Passive,index);
		mProxyMode = GetApp().GetAppPref().GetData_NumberArray(AAppPrefDB::kFTPAccount_ProxyMode,index);
		if( mProxyMode != AAppPrefDB::kProxyMode_NoProxy )
		{
			GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFTPAccount_ProxyServer,index,mProxyServer);
			GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFTPAccount_ProxyUser,index,mProxyUser);
			ASecurityWrapper::GetInternetPassword(mProxyServer,mProxyUser,mProxyPassword);
		}
		mBinaryMode = GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFTPAccount_BinaryMode,index);
#if IMPLEMENTATION_FOR_WINDOWS
		GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFTPAccount_Password,index,mPassword);
#endif
	}
	else
	{
		//#193 mPort = 21;
		mPort = 21;//#1231
		mPassive = true;
		mProxyMode = AAppPrefDB::kProxyMode_NoProxy;
		mBinaryMode = false;
#if IMPLEMENTATION_FOR_WINDOWS
		mPassword.DeleteAll();
#endif
		_AError("FTP account data do not exist",this);
	}
}

#pragma mark ===========================

#pragma mark --- �f�[�^�|�[�g����

//�ڑ��A�܂��́AListen�J�n
void	AFTPConnection::ConnectOrListenDataPort()
{
	AText	text;
	ANumber	responceCode;
	
	//PASV���[�h�i�������瑤���A�N�e�B�u�ڑ��ɂȂ�j
	if( mPassive == true )
	{
		//PASV�R�}���h
		text.SetCstring("PASV\r\n");
		mControlSocket.Send(text);
		_Log(text,true);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode !=kEnteringPassiveMode )   _AThrow("FTP server cannot enter passive mode",NULL);
		
		//�f�[�^�R�l�N�V�����̃T�[�o�[�A�h���X�A�|�[�g�擾
		AIndex	pos = text.GetLength()-1;
		AText	token;
		text.GoBackToChar(pos,',');
		pos++;
		text.GetToken(pos,token);
		// pos�ʒu(*)�Fh1,h2,h3,h4,p1,p2(*)
		//�������炳���̂ڂ��ăg�[�N�����
		
		//�|�[�g�ԍ�
		APortNumber	remote_port;
		
		text.GoBackGetToken(pos,token);
		// pos�ʒu(*)�Fh1,h2,h3,h4,p1,(*)p2
		remote_port = static_cast<APortNumber>(token.GetNumber());
		
		text.GoBackGetToken(pos,token);
		// pos�ʒu(*)�Fh1,h2,h3,h4,p1(*),p2
		text.GoBackGetToken(pos,token);
		// pos�ʒu(*)�Fh1,h2,h3,h4,(*)p1,p2
		remote_port += static_cast<APortNumber>(token.GetNumber()*0x100);
		
		//IP�A�h���X
		AIPAddress	remote_ipaddress;
		
		//h4
		text.GoBackGetToken(pos,token);
		text.GoBackGetToken(pos,token);
		remote_ipaddress = token.GetNumber();
		//h3
		text.GoBackGetToken(pos,token);
		text.GoBackGetToken(pos,token);
		remote_ipaddress += token.GetNumber() *0x100;
		//h2
		text.GoBackGetToken(pos,token);
		text.GoBackGetToken(pos,token);
		remote_ipaddress += token.GetNumber() *0x10000;
		//h1
		text.GoBackGetToken(pos,token);
		text.GoBackGetToken(pos,token);
		remote_ipaddress += token.GetNumber() *0x1000000;
		
		//�f�[�^�R�l�N�V����Connect
		mDataSocket.Connect(remote_ipaddress,remote_port);
	}
	else
	{
		//�f�[�^�R�l�N�V����Listen
		mListenSocket.EphemeralListen(1);
		
		//PORT�R�}���h
		AIPAddress	local_ipaddress;
		APortNumber	local_port;
		mListenSocket.GetLocalhostAddress(local_ipaddress,local_port);
		APortNumber	tmp;
		mControlSocket.GetLocalhostAddress(local_ipaddress,tmp);
		text.SetFormattedCstring("PORT %u,%u,%u,%u,%u,%u\r\n",
				(local_ipaddress&0xFF000000)/0x1000000,
				(local_ipaddress&0xFF0000)/0x10000,
				(local_ipaddress&0xFF00)/0x100,
				(local_ipaddress&0xFF),
				(local_port&0xFF00)/0x100,
				(local_port&0xFF));
		mControlSocket.Send(text);
		Read_WaitFTPResponceCode(text,responceCode);
		_Log(text,false);
		if( responceCode != kCommandOK )   _AThrow("FTP PORT comamnd error",NULL);
	}
}

//
void	AFTPConnection::CloseDataPort()
{
	mDataSocket.Close();
	mListenSocket.Close();
}

#pragma mark ===========================

#pragma mark ---�v���O���X�\���C�x���g���M

void	AFTPConnection::HideProgress()
{
	ABase::PostToMainInternalEventQueue(kInternalEvent_FTPProgress_Hide,GetObjectID(),0,GetEmptyText(),GetEmptyObjectIDArray());
}

void	AFTPConnection::_Log( const AText& inText, const ABool inSend )
{
	AText	h;
	if( inSend == true )
	{
		h.SetCstring("(S) ");
	}
	else
	{
		h.SetCstring("(R) ");
	}
	AText	text;
	for( AIndex pos = 0; pos < inText.GetItemCount();  )
	{
		AText	line;
		inText.GetLine(pos,line);
		text.AddText(h);
		text.AddText(line);
		text.Add(kUChar_LineEnd);
	}
	//text.OutputToStderr();
	ABase::PostToMainInternalEventQueue(kInternalEvent_FTPLog,GetObjectID(),0,text,GetEmptyObjectIDArray());
}

void	AFTPConnection::ProgressPercent( ANumber inPercent )
{
	ABase::PostToMainInternalEventQueue(kInternalEvent_FTPProgress_SetProgress,GetObjectID(),inPercent,GetEmptyText(),GetEmptyObjectIDArray());
}

void	AFTPConnection::ProgressText( AText& inText )
{
	ABase::PostToMainInternalEventQueue(kInternalEvent_FTPProgress_SetProgressText,GetObjectID(),0,inText,GetEmptyObjectIDArray());
}

void	AFTPConnection::ShowError( AConstCharPtr inErrorString )
{
	AText	text;
	text.SetFormattedCstring("%s   errno:%d",inErrorString,errno);
	ABase::PostToMainInternalEventQueue(kInternalEvent_FTPProgress_Error,GetObjectID(),0,text,GetEmptyObjectIDArray());
}


