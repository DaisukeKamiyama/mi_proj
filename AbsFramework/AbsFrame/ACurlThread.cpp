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
#pragma mark [�N���X]ACurlThread
#pragma mark ===========================
//�}���`�t�@�C���������s

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
ACurlThread::ACurlThread( AObjectArrayItem* inParent ) : AThread(inParent), 
		mRequestType(kCurlRequestType_None), mTransactionType(kCurlTransactionType_None)
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/**
�ǂݍ��݃R�[���o�b�N���[�`��
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
�X���b�h���C�����[�`��
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
				//Mac OS X�W���Y�t��curl�R�}���h���g�p����
				AText	command;
				command.SetCstring("/usr/bin/curl");
				ATextArray	argArray;
				argArray.Add(command);
				argArray.Add(mURL);
				AText	resultText;
				GetApp().SPI_GetChildProcessManager().ExecuteGeneralSyncCommand(command,argArray,GetEmptyText(),mResultText);
				
				/*#1159
				//curl�Z�b�V����������
				CURL *curl_handle = curl_easy_init();
				
				CURLcode	err;
				//URL�ݒ�
				{
					AStCreateCstringFromAText	cstr(mURL);
					err = curl_easy_setopt(curl_handle, CURLOPT_URL, cstr.GetPtr());
				}
				
				//�R�[���o�b�N���[�`���ݒ�
				err = curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, ACurlThread::ReadToMemory);
				
				//�R�[���o�b�N���[�`���ւ̈����ݒ�
				AObjectID	objID = GetObjectID();
				err = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)(&objID));
				
				//��������web server�ł�user-agent���Ȃ��ƃA�N�Z�X�ł��Ȃ��炵��
				err = curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
				
				//�擾���s�i�R�[���o�b�N���s�x�R�[�������j
				err = curl_easy_perform(curl_handle);
				
				//�I������
				curl_easy_cleanup(curl_handle);
				*/
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
	catch(...)
	{
		_ACError("ACurlThread::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	
	//���C���X���b�h�֒ʒm
	AObjectIDArray	objectIDArray;
	objectIDArray.Add(GetObjectID());
	ABase::PostToMainInternalEventQueue(kInternalEvent_Curl,GetObjectID(),0,GetEmptyText(),objectIDArray);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ACurlThread::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---�X���b�h�N��

/**
�_�E�����[�h�J�n�i�X���b�h�N���j
*/
void	ACurlThread::SPNVI_Run( const ECurlRequestType inRequestType, const AText& inURL, 
			const ACurlTransactionType inTransactionType,  const ATextArray& inInfoTextArray )
{
	//���URL�œ��삳�����bad access��������̂ł��̃`�F�b�N�K�v
	if( inURL.GetItemCount() == 0 )
	{
		return;
	}
	//
	mRequestType = inRequestType;
	mURL.SetText(inURL);
	mTransactionType = inTransactionType;
	mInfoTextArray.SetFromTextArray(inInfoTextArray);//�񓯊������Ȃ̂ŁA���ʎ擾���̏����̂��߂̏��e�L�X�g���L�����Ă����B
	//�X���b�h�N��
	NVI_Run();
}

#pragma mark ===========================

#pragma mark ---���ʎ擾

/**
���ʎ擾
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

