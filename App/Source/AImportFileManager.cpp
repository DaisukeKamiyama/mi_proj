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

AImportFileManager

*/

#include "stdafx.h"

#include "AImportFileManager.h"
#include "AImportIdentifierLink.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AImportFileManager
#pragma mark ===========================
//�C���|�[�g�t�@�C����ێ��E�Ǘ�����N���X�i�A�v���S�̂̊e�t�@�C���̃C���|�[�g(include)�t�@�C���̕��@����ێ�����j
//ImportFileManager�̓v���W�F�N�g�ɂ͈ˑ����Ȃ��B�ǂ̃t�@�C�������[�h���邩�͊O������t�@�C�����ƂɎw�肷��
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AImportFileManager::AImportFileManager() : AObjectArrayItem(NULL)
,mImportFileDataArray(NULL)//#693,true)//#417 #693
{
	//mImportFileDataArrayMutex��mutex���䂷��̂�mImportFileDataArray���̂�mutex����͖����ɂ���B
	//mImportFileDataArray.EnableThreadSafe(false);
}
//�f�X�g���N�^
AImportFileManager::~AImportFileManager()
{
}

#pragma mark ===========================

#pragma mark ---ImportFileData�擾

//�擾
AImportFileData&	AImportFileManager::GetImportFileDataByID( const AObjectID inImportFileDataObjectID )
{
	//fprintf(stderr,"GetImportFileDataByID()lock ");
	//Array���̂̓X���b�h�Ԃŋ��L�Ȃ̂�mutex�K�v�B���g�̗v�f�̓X���b�h�Ԃœ����A�N�Z�X���邱�Ƃ͂Ȃ��imImportFileDataArray_RecognizedFlag�Ő��䂳���j
	AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
	//fprintf(stderr,"locked ");
	return mImportFileDataArray.GetObjectByID(inImportFileDataObjectID);
}
const AImportFileData&	AImportFileManager::GetImportFileDataConstByID( const AObjectID inImportFileDataObjectID ) const
{
	//fprintf(stderr,"GetImportFileDataConstByID()lock ");
	//Array���̂̓X���b�h�Ԃŋ��L�Ȃ̂�mutex�K�v�B���g�̗v�f�̓X���b�h�Ԃœ����A�N�Z�X���邱�Ƃ͂Ȃ��imImportFileDataArray_RecognizedFlag�Ő��䂳���j
	AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
	//fprintf(stderr,"locked ");
	return mImportFileDataArray.GetObjectConstByID(inImportFileDataObjectID);
}

//#723
/**
ImportFileData�擾�Eretain
@note ���[�h���X�g�����X���b�h����̃R�[���p�B���łɑ��݂��Ȃ���������Ȃ��̂ł��̂Ƃ���NULL��Ԃ��Bdecrement����܂ł̓I�u�W�F�N�g�͉������Ȃ��B
*/
AImportFileData*	AImportFileManager::GetImportFileDataAndRetain( const AObjectID inImportFileDataObjectID )
{
	return mImportFileDataArray.GetObjectAndRetain(inImportFileDataObjectID);
}

#pragma mark ===========================

/**
ImportFileData����or���[�h
@note �t�@�C������ImportFileData�I�u�W�F�N�g����������B�����[�h�Ȃ�A�I�u�W�F�N�g�������āA�X���b�h�ɉ�͗v�����āA���̃I�u�W�F�N�g��Ԃ��Bthread-safe
*/
AObjectID	AImportFileManager::FindOrLoadImportFileDataByFile( const AFileAcc& inImportFile, const ABool inIsSystemHeader )//#852
{
	//�s���t�@�C���A����сA��e�L�X�g�t�@�C���͑ΏۊO�Ƃ���
	//�i���ɕW�����[�h�ŃC���|�[�g�t�@�C���̐������ɑ�ʂɂȂ�̂�h�����߁A��e�L�X�g�t�@�C�������O����K�v������j
	if( inImportFile.IsInvisible() == true || GetApp().GetAppPref().IsBinaryFile(inImportFile) == true )
	{
		return kObjectID_Invalid;
	}
	
	//���[�h�ς݂̂Ȃ����猟�����A������΂���ObjectID��Ԃ�
	
	//Init�K�v�t���O #860
	ABool	shouldInit = false;
	//#723 mImportFileDataArray_Path��index�̂���h�~�̂��߁A�r������ӏ��ύX�B
	AObjectID	importFileDataObjectID = kObjectID_Invalid;
	{
		//ImportFileDataArray�̔r������iarray�̌��q���ۏ؁BmImportFileDataArray_Path���܂߂�index�̂��ꂪ�������Ȃ��悤�ɂ���j
		AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
		
		AIndex	index = kIndex_Invalid;
		AText	path;
		inImportFile.GetNormalizedPath(path);
		index = mImportFileDataArray_Path.Find(path);
		if( index == kIndex_Invalid )
		{
			//�����݂Ȃ̂ŁA�V�K��������B
			
			//fprintf(stderr,"FindOrLoadImportFileDataByFile()lock ");
			//�z��ւ̒ǉ�
			//#723 ��ֈړ� AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
			//fprintf(stderr,"locked ");
			AImportFileDataFactory	factory(this,inImportFile);
			//fprintf(stderr,"before:%d ",AArrayAbstract::GetTotalAllocatedByteCount());
			index = mImportFileDataArray.AddNewObject(factory);
			mImportFileDataArray_Path.Add(path);//B0000 ������
			//fprintf(stderr,"after:%d ",AArrayAbstract::GetTotalAllocatedByteCount());
			//fprintf(stderr,"added ");
			//path.OutputToStderr();
			//#860
			shouldInit = true;
		}
		//#723 mImportFileDataArray.GetObject(index).Init();//B0000 mutex�������init���Ȃ��ƁASetMode()������GetImportFileDataConstByID()���Ă΂��
		//Object ID�擾
		importFileDataObjectID = mImportFileDataArray.GetObjectConst(index).GetObjectID();
	}
	//#723 �ォ��ړ�
	//�V�K���������ꍇ�AImportFileObject���������A��͗v��
	if( shouldInit == true )
	{
		GetImportFileDataByID(importFileDataObjectID).Init(inIsSystemHeader);//#852
	}
	//��͗v��
	if( GetImportFileDataByID(importFileDataObjectID).IsRecognized() == false )//#860
	{
		if( GetApp().NVI_IsWorking() == true)//�A�v���P�[�V�����I�����̓X���b�h���I�����Ă���\��������̂ŃL���[�ɐς܂Ȃ�
		{
			//��͗v���L���[�ɂ�
			GetApp().SPI_GetImportFileRecognizer().AddToRequestQueue(importFileDataObjectID);
			//�X���b�h��sleep����N����
			GetApp().SPI_GetImportFileRecognizer().NVI_WakeTrigger();
		}
	}
	return importFileDataObjectID;
}

#pragma mark ===========================

#pragma mark ---�C���|�[�g�t�@�C�����[�h�A���ʎq�����擾

/**
�w��FileArray�ɂ��āAImportFileData�������A�����N���X�g���擾
inFileArray�Ŏw�肵���t�@�C�����AmImportFileArray�z��Ƀ��[�h���A���ʎq�����擾����B
ADocument_Text����A�C���|�[�g�t�@�C���ɕύX���������ꍇ�A����сA��̓X���b�h���������������ꍇ�ɁA�R�[�������
*/
ABool	AImportFileManager::GetImportIdentifierLinkList( const ADocumentID inTextDocumentID, const AObjectArray<AFileAcc>& inFileArray, 
		const AModeIndex inModeIndex, AImportIdentifierLinkList& outImportIdentifierLinkList, const ABool inIsSystemHeader )//#852
{
	//fprintf(stderr,"GetImportIdentifierLinkList()");
	//�e�C���|�[�g�t�@�C�����Ƃ�ImportFileData������or���[�h����
	AArray<AObjectID>	rootObjectIDArray;
	for( AIndex i = 0; i < inFileArray.GetItemCount(); i++ )
	{
		//�C���|�[�g�t�@�C���̃C���f�b�N�X���擾����i�����[�h�Ȃ烍�[�h�E��͗v���j
		AObjectID	objectID = FindOrLoadImportFileDataByFile(inFileArray.GetObjectConst(i),inIsSystemHeader);//#852
		if( objectID != kObjectID_Invalid )
		{
			//���[�h�������ꍇ�̂�rootImportFileIndex�ɒǉ�
			if( inModeIndex == GetImportFileDataConstByID(objectID).GetModeIndex() )
			{
				rootObjectIDArray.Add(objectID);
			}
		}
	}
	//��͓r�������p�̋�؂�}�[�N������
	//�r�������}�[�Ndrop GetApp().SPI_GetImportFileRecognizer().AddToRequestQueue(kObjectID_Invalid);
	//
	ABool	threadRecognizing = false;
	//�C���|�[�g�t�@�C�����̃C���|�[�g�t�@�C�����܂߁A�Q�Ƃ��Ă���C���|�[�g�t�@�C���S�Ă��������AObjectID��allObjectIDArray�֊i�[����
	AHashArray<AObjectID>	allObjectIDArray;//#423
	for( AIndex i = 0; i < rootObjectIDArray.GetItemCount(); i++ )
	{
		if( GetImportFileDataConstByID(rootObjectIDArray.Get(i)).IsRecognized() == true )
		{
			MakeImportFileDataObjectIDArrayRecursive(rootObjectIDArray.Get(i),allObjectIDArray);
		}
		else
		{
			threadRecognizing = true;
		}
	}
	//B0000 080808 �����̃t�@�C���ɑΉ�����ImportFileData��allObjectIDArray�����菜��
	if( inTextDocumentID != kObjectID_Invalid )//#253
	{
		//#0 ������
		//���h�L�������g�̃t�@�C���p�X�擾
		AFileAcc	docfile;
		GetApp().SPI_GetTextDocumentByID(inTextDocumentID).NVI_GetFile(docfile);
		AText	textdocfilepath;
		docfile.GetNormalizedPath(textdocfilepath);
		//���h�L�������g�ɑΉ�����ImportFileData��ObjectID���t�@�C���p�X���猟��
		AObjectID	textdocImportFileObjectID = kObjectID_Invalid;
		{
			AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
			
			//�t�@�C���p�X���猟��
			AIndex	objindex = mImportFileDataArray_Path.Find(textdocfilepath);
			if( objindex != kIndex_Invalid )
			{
				textdocImportFileObjectID = mImportFileDataArray.GetObject(objindex).GetObjectID();
			}
		}
		//���h�L�������g�ɑΉ�����ImportFileData��allObjectIDArray�����菜��
		if( textdocImportFileObjectID != kObjectID_Invalid )
		{
			for( AIndex i = 0; i < allObjectIDArray.GetItemCount(); )
			{
				if( allObjectIDArray.Get(i) == textdocImportFileObjectID )
				{
					allObjectIDArray.Delete1(i);
				}
				else
				{
					i++;
				}
			}
		}
	}
	//�Q�Ƃ��Ă��肩��͍ς݂̃t�@�C���̎��ʎq�ւ̃����N��outImportKeywordDataList�֊i�[����
	outImportIdentifierLinkList.UpdateIdentifierLinkList(allObjectIDArray);//#423
	if( threadRecognizing == true )//B0000 �S��Recognized�Ȃ�X���b�hwake����K�v���Ȃ��̂ŁANVI_WakeTrigger()������if���Ɋ܂߂�
	{
		//��̓X���b�h�J�n�g���K�[
		GetApp().SPI_GetImportFileRecognizer().NVI_WakeTrigger();
		if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AImportFileRecognizer WakeTrigger from GetImportIdentifierLinkList()",this);
		//fprintf(stderr,"waketrigger(GetImportIdentifierLinkList)  ");
		//
		if( inTextDocumentID != kObjectID_Invalid )//#253
		{
			if( mWaitingTextDocumentIDArray.Find(inTextDocumentID) == kIndex_Invalid )
			{
				mWaitingTextDocumentIDArray.Add(inTextDocumentID);
			}
		}
		else//#253
		{
			if( mWaitingModePrefArray.Find(inModeIndex) == kIndex_Invalid )
			{
				mWaitingModePrefArray.Add(inModeIndex);
			}
		}
	}
	return threadRecognizing;
}

//���[�h�ς݃t�@�C���̒��Ŏw��t�@�C������ċA�I�ɎQ�Ƃ��Ă���S�Ẵt�@�C������������
void	AImportFileManager::MakeImportFileDataObjectIDArrayRecursive( const AObjectID inImportFileDataObjectID, AHashArray<AObjectID>& ioObjectIDArray ) const
{
	//����ObjectID��ioObjectIDArray�ɒǉ��ς݁i�K��ς݁j�Ȃ牽���������^�[��
	if( ioObjectIDArray.Find(inImportFileDataObjectID) != kIndex_Invalid )   return;
	//�܂��������g��K��ς݂ɂ���
	ioObjectIDArray.Add(inImportFileDataObjectID);
	//
	const AArray<AObjectID>&	recursiveObjectIDArray = GetImportFileDataConstByID(inImportFileDataObjectID).GetRecursiveImportFileDataObjectIDArrayConst();
	for( AIndex i = 0; i < recursiveObjectIDArray.GetItemCount(); i++ )
	{
		AObjectID	objectID = recursiveObjectIDArray.Get(i);
		if( GetImportFileDataConstByID(objectID).IsRecognized() == true )
		{
			MakeImportFileDataObjectIDArrayRecursive(objectID,ioObjectIDArray);
		}
	}
}

//Recognizer�X���b�h����A���݂̉�̓L���[���̉�͂����������ꍇ�ɁAAApp�o�R�ŃR�[�������B
void	AImportFileManager::DoImportFileRecognized( const ABool inCompleted )
{
	//��U���[�J���ɃR�s�[���Ă���A���̔z��ɂ��Ď��s����B
	//SPI_DoImportFileRecognized()�̂Ȃ��ŁA�ēx�AmWaitingTextDocumentIDArray�ɒǉ������\�������邽�߁B�iSleepFlag�Z�b�g���j
	AArray<AObjectID>	idarray;
	for( AIndex i = 0; i < mWaitingTextDocumentIDArray.GetItemCount(); i++ )
	{
		idarray.Add(mWaitingTextDocumentIDArray.Get(i));
	}
	//�F���������A�ʒm��h�L�������g���X�g���폜�i����AinCompleted�͏��true�j
	if( inCompleted == true )
	{
		mWaitingTextDocumentIDArray.DeleteAll();
	}
	//==================�h�L�������g�ɒʒm==================
	while(idarray.GetItemCount()>0)
	{
		ADocumentID	docID = idarray.Get(idarray.GetItemCount()-1);
		if( GetApp().NVI_IsDocumentValid(docID) == true )
		{
			//fprintf(stderr,"SPI_DoImportFileRecognized()called%d  ",docID);
			GetApp().SPI_GetTextDocumentByID(docID).SPI_DoImportFileRecognized();
		}
		idarray.Delete1(idarray.GetItemCount()-1);
	}
	//==================���[�h�ݒ�ɒʒm==================
	//#253
	//�F���������A�ʒm�惂�[�h���폜�i����AinCompleted�͏��true�j
	if( inCompleted == true )
	{
		AArray<AModeIndex>	modeindexarray;
		for( AIndex i = 0; i < mWaitingModePrefArray.GetItemCount(); i++ )
		{
			modeindexarray.Add(mWaitingModePrefArray.Get(i));
		}
		mWaitingModePrefArray.DeleteAll();
		while(modeindexarray.GetItemCount()>0)
		{
			AModeIndex	modeIndex = modeindexarray.Get(modeindexarray.GetItemCount()-1);
			GetApp().SPI_GetModePrefDB(modeIndex).DoImportFileRecognized();
			modeindexarray.Delete1(modeindexarray.GetItemCount()-1);
		}
	}
}

#pragma mark ===========================

#pragma mark ---�C���|�[�g�t�@�C���ύX���f

/**
�h�L�������g�ۑ�����AApp::SPI_DoTextDocumentSaved()�o�R�ŃR�[�������B�ĉ�͂��s���B
*/
void	AImportFileManager::DoFileSaved( const AFileAcc& inFile )
{
	//==================�����N���X�g����Q�Ƃ��폜==================
	//�t�@�C���ɑΉ�����Import File Data��recognized��ԉ������A�Q�Ƃ��Ă��郊���N���X�g����Q�Ƃ�S�č폜����
	AObjectID	importFileDataObjectID = SetUnrecognizeAndDeleteFromLinkList(inFile,true);
	
	if( importFileDataObjectID != kObjectID_Invalid )
	{
		//==================�ĉ�͗v��==================
		//��͗v��
		GetApp().SPI_GetImportFileRecognizer().AddToRequestQueue(importFileDataObjectID);
		//��̓X���b�h�J�n�g���K�[
		GetApp().SPI_GetImportFileRecognizer().NVI_WakeTrigger();
		if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AImportFileRecognizer WakeTrigger from DoFileSaved()",this);
		//fprintf(stderr,"waketrigger(DoFileSaved)  ");
	}
}

//#896
/**
ImportFileData��purge����i�v���W�F�N�g���̃h�L�������g���S�ĕ���ꂽ�Ƃ��ɃR�[�������B�������[�팸�̂��߁B�j
*/
void	AImportFileManager::PurgeImportFileData( const ATextArray& inPathArray )
{
	//==================�����N���X�g����̎Q�Ƃ��폜==================
	//ImportFileData�I�u�W�F�N�g���������A������Q�Ƃ��Ă���LinkList���폜����
	AArray<AObjectID>	importFileDataArray;
	for( AIndex i = 0; i < inPathArray.GetItemCount(); i++ )
	{
		//�t�@�C���擾
		AFileAcc	file;
		file.Specify(inPathArray.GetTextConst(i));
		//�t�@�C���ɑΉ�����Import File Data��recognized��ԉ������A�Q�Ƃ��Ă��郊���N���X�g����Q�Ƃ�S�č폜����
		AObjectID	importFileDataObjectID = SetUnrecognizeAndDeleteFromLinkList(file,false);
		if( importFileDataObjectID != kObjectID_Invalid )
		{
			importFileDataArray.Add(importFileDataObjectID);
		}
	}
	//==================ImportFileData�p�[�W�L���[�ɒǉ�����==================
	//�i�����Œ��ڎ��s���Ă��܂��ƁAimport file data��mutex������̂ŁA�F���X���b�h�ŔF�����s����mutex�����҂��ɂȂ�A���Ԃ�������B�j
	if( importFileDataArray.GetItemCount() > 0 )
	{
		//�p�[�W�L���[�ɒǉ�
		GetApp().SPI_GetImportFileRecognizer().AddToImportFileDataPurgeRequestQueue(importFileDataArray);
		//sleep����
		GetApp().SPI_GetImportFileRecognizer().NVI_WakeTrigger();
	}
}

/**
�w��t�@�C����ImportFileData��recognized��Ԃ��������A�Q�Ƃ��Ă��郊���N���X�g����Q�Ƃ�S�č폜����
*/
AObjectID	AImportFileManager::SetUnrecognizeAndDeleteFromLinkList( const AFileAcc& inFile, const ABool inForReRecognize )
{
	//==================�Ή�����ImportFileData��Recognize�t���O��OFF�ɂ���B==================
	//#723 mImportFileDataArray_Path��index�̂���h�~�̂��߁A�r������ӏ��ύX�B
	AObjectID	importFileDataObjectID = kObjectID_Invalid;
	{
		AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
		
		//�w��t�@�C���ɑΉ�����ImportFileData���擾����
		AIndex	importFileDataArrayIndex = kIndex_Invalid;
		AText	path;
		inFile.GetNormalizedPath(path);
		importFileDataArrayIndex = mImportFileDataArray_Path.Find(path);
		
		//�Ή�����import file data��������΃��^�[��
		if( importFileDataArrayIndex == kIndex_Invalid )   return kObjectID_Invalid;
		//Recognized�łȂ���΁A���̂���Recognize�����̂ŁA���̂܂܂ɂ��Ă���
		//if( mImportFileDataArray.GetObject(importFileDataArrayIndex).IsRecognized() == false )   return;
		//Recognized�t���O������
		mImportFileDataArray.GetObject(importFileDataArrayIndex).SetRecognized(false);
		
		//�w��t�@�C���ɑΉ�����ImportFileData��ObjectID���擾����
		importFileDataObjectID = mImportFileDataArray.GetObjectConst(importFileDataArrayIndex).GetObjectID();
	}
	
	//==================�h�L�������g��LinkList���X�V==================
	//�S�Ẵh�L�������g�̃C���|�[�g�t�@�C���f�[�^���炱�̃t�@�C���̎��ʎq�����폜����B
	for( AObjectID docID = GetApp().NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = GetApp().NVI_GetNextDocumentID(docID) )
	{
		//�h�L�������g��LinkList����폜����
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_DeleteImportFileData(importFileDataObjectID,inFile) == true )
		{
			//�ĔF������ꍇ�i��DoFileSaved()�o�R�j�́A�ʒm��h�L�������g���X�g�Ƀh�L�������g��ǉ����Ă���
			//�i�����Apurge�̏ꍇ�́A�h�L�������g���폜����Ă���̂ł����ɗ��Ȃ��j
			if( inForReRecognize == true )
			{
				if( mWaitingTextDocumentIDArray.Find(docID) == kIndex_Invalid )
				{
					mWaitingTextDocumentIDArray.Add(docID);
				}
			}
		}
	}
	
	//==================ModePrefDB�̃V�X�e���w�b�_LinkList���X�V==================
	//#253 �S�Ẵ��[�h�ݒ�̃C���|�[�g�t�@�C���f�[�^���炱�̃t�@�C���̎��ʎq�����폜����B
	for( AModeIndex modeIndex = 0; modeIndex < GetApp().SPI_GetModeCount(); modeIndex++ )
	{
		if( GetApp().SPI_GetModePrefDB(modeIndex,false).IsLoaded() == true )//#517 ���[�h�ς݂̃��[�h�̂ݎ��s
		{
			//�V�X�e���w�b�_��LinkList����폜
			if( GetApp().SPI_GetModePrefDB(modeIndex).DeleteImportFileData(importFileDataObjectID,inFile) == true )
			{
				//�ĔF������ꍇ�i��DoFileSaved()�o�R�j�́A�ʒm�惂�[�h�ݒ胊�X�g�Ƀ��[�h�ݒ��ǉ����Ă���
				//�i�����A�V�X�e���w�b�_��purge���邱�Ƃ��Ȃ��̂ŁApurge�̏ꍇ�͂����ɂ͗��Ȃ��j
				if( inForReRecognize == true )
				{
					if( mWaitingModePrefArray.Find(modeIndex) == kIndex_Invalid )
					{
						mWaitingModePrefArray.Add(modeIndex);
					}
				}
			}
		}
	}
	
	return importFileDataObjectID;
}

#pragma mark ===========================

#pragma mark ---�폜

//#349
/**
�w�胂�[�h�̉�̓f�[�^��S�č폜���A���@��̓X���b�h���~����
*/
void	AImportFileManager::ClearRecognizedDataAndStopRecognizer( const AIndex inModeIndex )
{
	//==================�F���X���b�h��~==================
	//ImportFileRecognizer�̓����~�҂�
	GetApp().SPI_GetImportFileRecognizer().ClearRequestQueue(inModeIndex);
	GetApp().SPI_GetImportFileRecognizer().NVI_WaitAbortThread();
	GetApp().SPI_GetImportFileRecognizer().ClearRequestQueue(inModeIndex);//#349 AImportFileRecognizer::NVIDO_ThreadMain()���̃��[�v�𔲂���Ƃ��ɃL���[�ǉ�����Ă���\�������邽��
	
	//==================�h�L�������g����LinkList�폜==================
	//�h�L�������g����AImportIdentifierLinkList����AImportFileData�ւ̃����N���\���Ă���̂ŁA
	//AImportFileData��S�폜����O�ɁA�w�胂�[�h�̃h�L�������g���́A�����N�f�[�^��S�č폜����
	for( AObjectID docID = GetApp().NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; 
				docID = GetApp().NVI_GetNextDocumentID(docID) )
	{
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex )
		{
			//�h�L�������g����LinkList���폜
			GetApp().SPI_GetTextDocumentByID(docID).SPI_DeleteAllImportFileData();
			//�F���v�����̃h�L�������g�̋L�����폜
			mWaitingTextDocumentIDArray.FindAndDelete(docID);
		}
	}
	
	//==================���[�h�ݒ���̃V�X�e���w�b�_LinkList�폜==================
	//#253 �S�Ẵ��[�h�ݒ�̃C���|�[�g�t�@�C���f�[�^���炱�̃t�@�C���̎��ʎq�����폜����B
	/*
	for( AModeIndex modeIndex = 0; modeIndex < GetApp().SPI_GetModeCount(); modeIndex++ )
	{
		//SPI_GetModePrefDB()�̑�2������false�ɂ��邱�Ƃɂ��A���[�h�f�[�^��Load����Ă��Ȃ��ꍇ�ɁALoad���Ȃ��悤�ɂ���B
		//���[�h��Load������ƁARecoginizer�̃X���b�h�������o���Ă��܂����߁B
		GetApp().SPI_GetModePrefDB(modeIndex,false).DeleteAllImportFileData();
	}
	*/
	GetApp().SPI_GetModePrefDB(inModeIndex).DeleteAllImportFileData();
	//�F���v������mode index�̋L�����폜
	mWaitingModePrefArray.FindAndDelete(inModeIndex);//#253
	
	//==================Recognized�t���O��OFF�ɂ���==================
	{
		//�r������
		AStMutexLocker	mutexlocker(mImportFileDataArrayMutex);
		
		AItemCount	itemCount = mImportFileDataArray.GetItemCount();
		for( AIndex index = 0; index < itemCount; index++ )
		{
			if( mImportFileDataArray.GetObject(index).GetModeIndex() == inModeIndex )
			{
				//Recognized�t���O������
				mImportFileDataArray.GetObject(index).SetRecognized(false);
			}
		}
	}
}

/**
���@��̓X���b�h���ĊJ����
*/
void	AImportFileManager::RestartRecognizer( const AIndex inModeIndex )
{
	//==================�F���X���b�h�ċN��==================
	if( GetApp().SPI_GetImportFileRecognizer().NVI_IsThreadWorking() == false )
	{
		GetApp().SPI_GetImportFileRecognizer().NVI_Run();
	}
	
	/*�e�h�L�������g��LinkList�́A���񕶖@�F���������ɍX�V�����̂ŁA��������͖����I�ȍX�V�͂��Ȃ��B
	//==================�h�L�������gLinkList�X�V==================
	for( AObjectID docID = GetApp().NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; 
				docID = GetApp().NVI_GetNextDocumentID(docID) )
	{
		if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == inModeIndex )
		{
			//�h�L�������g����LinkList���X�V
			GetApp().SPI_GetTextDocumentByID(docID).SPI_UpdateImportFileData();
		}
	}
	*/
	
	//==================���[�h�ݒ���̃V�X�e���w�b�_LinkList�X�V==================
	GetApp().SPI_GetModePrefDB(inModeIndex).UpdateSystemHeaderFileData();
	
}

#pragma mark ===========================
#pragma mark [�N���X]AImportFileRecognizer
#pragma mark ===========================
//ImportFile�̕��@��̓X���b�h�i�펞�N���X���b�h�j

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

AImportFileRecognizer::AImportFileRecognizer( AObjectArrayItem* inParent ) : AThread(inParent)//#272
,mRequestQueue_CurrentProcessingImportFileDataObjectID(kObjectID_Invalid)
{
}
AImportFileRecognizer::~AImportFileRecognizer()
{
}

#pragma mark ===========================

#pragma mark ---�v���L���[�ւ̒ǉ�

void	AImportFileRecognizer::AddToRequestQueue( const AObjectID inImportFileDataObjectID )
{
	if( NVM_IsAborted() == true )   return;//#253
	
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	
	/*��͓r������drop
	if( inImportFileDataObjectID == kObjectID_Invalid )
	{
		mRequestQueue.Add(kObjectID_Invalid);
		return;
	}
	*/
	
	//------------------�p�[�W�L���[����폜------------------
	//�i���̎��_�Ńp�[�W�҂�������ƁA�F�������s������A�p�[�W����Ă��܂��\��������B�j
	{
		AIndex	index = mImportFileDataPurgeRequestQueue.Find(inImportFileDataObjectID);
		if( index != kIndex_Invalid )
		{
			mImportFileDataPurgeRequestQueue.Delete1(index);
		}
	}
	
	//------------------�F���v���L���[�ɒǉ�------------------
	if( mRequestQueue.Find(inImportFileDataObjectID) == kIndex_Invalid &&
				mRequestQueue_CurrentProcessingImportFileDataObjectID != inImportFileDataObjectID )
	{
		mRequestQueue.Add(inImportFileDataObjectID);
		//fprintf(stderr,"AddToRequestQueue:%d \n",mRequestQueue.GetItemCount());
	}
}

void	AImportFileRecognizer::ClearRequestQueue( const AModeIndex inModeIndex )
{
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	for( AIndex i = 0; i < mRequestQueue.GetItemCount();  )
	{
		AObjectID	id = mRequestQueue.Get(i);
		if( id != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetImportFileManager().GetImportFileDataByID(id).GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )//#349
			{
				mRequestQueue.Delete1(i);
			}
			else
			{
				i++;
			}
		}
		else
		{
			mRequestQueue.Delete1(i);
		}
	}
	//fprintf(stderr,"ClearRequestQueue:%d \n",mRequestQueue.GetItemCount());
}

//#0
/**
�v���L���[���擾
*/
AItemCount	AImportFileRecognizer::GetRequestQueueItemCount() const
{
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	return mRequestQueue.GetItemCount();
}

//#896
/**
�v���L���[����폜
*/
void	AImportFileRecognizer::RemoveFromRequestQueue( const AObjectID inImportFileDataObjectID )
{
	if( NVM_IsAborted() == true )   return;//#253
	
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	AIndex	index = mRequestQueue.Find(inImportFileDataObjectID);
	if( index != kIndex_Invalid )
	{
		mRequestQueue.Delete1(index);
	}
}

//#896
/**
purge�L���[�֒ǉ�
*/
void	AImportFileRecognizer::AddToImportFileDataPurgeRequestQueue( const AArray<AObjectID>& inImportFileDataArray )
{
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	for( AIndex i = 0; i < inImportFileDataArray.GetItemCount(); i++ )
	{
		AObjectID	importFileDataObjectID = inImportFileDataArray.Get(i);
		
		//------------------��͗v���L���[����폜------------------
		{
			AIndex	index = mRequestQueue.Find(importFileDataObjectID);
			if( index != kIndex_Invalid )
			{
				mRequestQueue.Delete1(index);
			}
		}
		
		//------------------purge�L���[�֒ǉ�------------------
		if( mImportFileDataPurgeRequestQueue.Find(importFileDataObjectID) == kIndex_Invalid )
		{
			mImportFileDataPurgeRequestQueue.Add(importFileDataObjectID);
		}
	}
}

//#896
/**
purge�L���[����폜
*/
void	AImportFileRecognizer::RemoveFromImportFileDataPurgeRequestQueue( const AObjectID inImportFileDataObjectID )
{
	AStMutexLocker	mutexlocker(mRequestQueueMutex);
	AIndex	index = mImportFileDataPurgeRequestQueue.Find(inImportFileDataObjectID);
	if( index != kIndex_Invalid )
	{
		mImportFileDataPurgeRequestQueue.Delete1(index);
	}
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

void	AImportFileRecognizer::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AImportFileRecognizer::NVIDO_ThreadMain started.",this);
	
	//�v���L���[������
	{
		AStMutexLocker	mutexlocker(mRequestQueueMutex);
		mRequestQueue.DeleteAll();
		mRequestQueue_CurrentProcessingImportFileDataObjectID = kObjectID_Invalid;
		mImportFileDataPurgeRequestQueue.DeleteAll();
	}
	
	ABool	debugmode = GetApp().SPI_GetMiscDebugMode();
	try
	{
		while(NVM_IsAborted()==false)
		{
			//�g���K�[�܂ŃX���[�v
			if( debugmode == true )   fprintf(stderr,"sleep ");
			NVM_Sleep();
			if( NVI_GetSleepFlag() == true )   continue;
			if( NVM_IsAborted() == true )   break;
			if( debugmode == true )   fprintf(stderr,"wake\n");
			NVM_SleepWithTimer(2);//��x�Q
			if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AImportFileRecognizer::NVIDO_ThreadMain wake.",this);
			if( NVM_IsAborted() == true )   break;
			
			//�L���[���Ȃ��Ƃ��ɉi�v�ɌJ��Ԃ�����΍�iABase::PostToMainInternalEventQueue()���R�[�����Ȃ��悤�ɂ�����continue�j
			if( /*#0 mRequestQueue.GetItemCount()*/GetRequestQueueItemCount() == 0 )   continue;
			//�L���[����ɂȂ�܂Ń��[�v
			//fprintf(stderr,"%d ",mRequestQueue.GetItemCount());
			AItemCount	dequeueObjectCount = 0;
			while( /*#0 mRequestQueue.GetItemCount()*/GetRequestQueueItemCount() > 0 && 
						NVM_IsAborted() == false && NVI_GetSleepFlag() == false )
			{
				//�L���[�����肾��
				AObjectID	importFileDataObjectID = kObjectID_Invalid;
				{
					AStMutexLocker	mutexlocker(mRequestQueueMutex);
					if( mRequestQueue.GetItemCount() == 0 )   continue;//#349
					importFileDataObjectID = mRequestQueue.Get(0);
					mRequestQueue.Delete1(0);
					/*�r�������ʒm drop
					if( importFileDataObjectID == kObjectID_Invalid )
					{
						if( dequeueObjectCount > 0 )
						{
							ABase::PostToMainInternalEventQueue(kInternalEvent_ImportFileRecognizePartialCompleted,GetObjectID(),0,GetEmptyText(),GetEmptyObjectIDArray());
						}
						//
						continue;
					}
					*/
					dequeueObjectCount++;
					//����recognized��ImportFileData�I�u�W�F�N�g�̏ꍇ�͉����������^�[��
					//recognized����`�L���[�ւ̒ǉ��̊ԂɁA�F����������������ƁA�����ɗ���\��������B
					//�F���ς݃t���O�͔r������̂��߂̃t���O�ɂ��Ȃ��Ă���̂ŁA�F���t���OON�Ȃ̂ɔF���������s���ƁA
					//�e�����N���X�g��index�̎w���f�[�^�����݂��Ȃ��Ȃ��Ă��܂��B
					if( GetApp().SPI_GetImportFileManager().GetImportFileDataByID(importFileDataObjectID).IsRecognized() == true )
					{
						//_ACError("",NULL);
						continue;
					}
					//���L�̏������ɁA����ImportFileData��v���L���[�ɐς�ł��܂�Ȃ��悤�ɁA
					//���ݏ�������ImportFileData��ID���L������B�iAddToRequestQueue()�ɂāA�L���[���ƁA
					//mRequestQueue_CurrentProcessingImportFileDataObjectID�Ɉ�v����ꍇ�́A�L���[�ɐς܂Ȃ��悤�ɂ��Ă���B�j
					mRequestQueue_CurrentProcessingImportFileDataObjectID = importFileDataObjectID;
				}
				{
					//���C���X���b�h��Progress�ʒm
					AObjectIDArray	objectIDArray;
					ANumber	prog = GetRequestQueueItemCount();//#0 mRequestQueue.GetItemCount();
					if( (prog%10) == 0 )
					{
						ABase::PostToMainInternalEventQueue(kInternalEvent_ImportFileRecognizeProgress,GetObjectID(),prog,GetEmptyText(),objectIDArray);
					}
				}
				
				//�L���[����Ƃ肾����ObjectID�ɑΉ�����ImportFileData�̉��
				//#723 AImportFileData&	importFileData = GetApp().SPI_GetImportFileManager().GetImportFileDataByID(importFileDataObjectID);
				//ImportFileData�폜���A���̒���TextInfo�̃f�[�^���폜�����̂�ImportFileData�̃f�X�g���N�^���Ȃ̂ŁA
				//AObjectArrayIDIndexed��getandretain�@�\���g���΁ADecrementRetainCount()����܂ł́ATextInfo�̃f�[�^���ۏ؂����B
				AImportFileData*	importFileDataPtr = 
									GetApp().SPI_GetImportFileManager().GetImportFileDataAndRetain(importFileDataObjectID);
				if( importFileDataPtr != NULL )//����ImportFileData���폜�ς݂̏ꍇ�́ANULL���Ԃ�
				{
					//Import File Data�ւ̃|�C���^��retain���m���ɉ�����邽�߂̃X�^�b�N�N���X
					AStDecrementRetainCounter	releaser(importFileDataPtr);
					
					AImportFileData&	importFileData = *(importFileDataPtr);
					//#723
					//ImportFileData�̍X�V���s���ԁAwords list finder�����L�[���[�h��������ImportFileData(�̒���TextInfo)�ɃA�N�Z�X���Ȃ��悤�ɔr������
					AStImportFileDataLocker	locker(importFileData);
					//
					AFileAcc	importFile = importFileData.GetFile();
					//�T�C�Y�������l�����̃t�@�C���̂ݕ��@�F������i�傫������t�@�C���̕��@�F���ɃX���b�h��L�����̂�h�����߁j #695
					if( importFile.GetFileSize() < kLimit_ImportFileRecognizer_MaxByteCountToRecognize )
					{
						//�e�L�X�g�ǂݍ���
						AText	text;
						AModeIndex	modeIndex = kStandardModeIndex;
						AText	tecname;
						GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_ImportFileRecognize,importFile,text,modeIndex,tecname);//#723 �h�L�������g����e�L�X�g�擾����K�v�������̂�SPI_GetTextFromFileOrDocument()����ύX
						//#723 �o�C�i���t�@�C���Ǝv�����Ƃ��́A�F�������[�h���X�g�쐬�����Ȃ�
						if( text.SuspectBinaryData() == false )
						{
							//ImportFileData�I�u�W�F�N�g�ɐݒ肵��modeindex���擾����B�i���SPI_LoadTextFromFile()�Ŏ擾����modeIndex�͎g�p���Ȃ��j
							modeIndex = importFileData.GetModeIndex();
							//debug
							if( false && debugmode  == true )
							{
								AText	t;
								t.SetFormattedCstring("Recog:");
								t.OutputToStderr();
								AText	filename;
								importFileData.GetFile().GetFilename(filename);
								filename.OutputToStderr();
								t.SetFormattedCstring(" size:%d mode:%d  totalalloc:%d\n",
											importFile.GetFileSize(),
											modeIndex,
											AArrayAbstract::GetTotalAllocatedByteCount());
								t.OutputToStderr();
							}
							//TextInfo���
							importFileData.GetTextInfo().CalcLineInfoAllWithoutView(text,mAborted);//win
							if( NVM_IsAborted() == true )   break;
							//#695 AIndex	startLineIndex, endLineIndex;
							ABool	importChanged = false;
							AArray<AUniqID>	addedIdentifier;
							AArray<AIndex>		addedIdentifierLineIndex;
							//#695 AArray<AUniqID>	deletedIdentifier;//win
							//��ֈړ� if( text.SuspectBinaryData() == false )//#723 �o�C�i���t�@�C���Ǝv�����Ƃ��͕��@��͂͂����A���[�h���X�g�����̂ݍs��
							{
								AText	path;//#998
								importFile.GetPath(path);//#998
								importFileData.GetTextInfo().RecognizeSyntaxAll(text,path,//#695 startLineIndex,endLineIndex,deletedIdentifier, #998
											addedIdentifier,addedIdentifierLineIndex,importChanged,mAborted);//R0000 #698
							}
							if( NVM_IsAborted() == true )   break;
							//TextInfo�Ƀ��[�h���X�g����
							//#1210 ���[�h���X�g �h���b�v importFileData.GetTextInfo().UpdateWordsList(text);//#723
							//
							importFileData.IncrementRevisionNumber();//#423
							//spos, epos��TextIndex�i�����̍ŏ�����̃I�t�Z�b�g�j�ɕϊ����ĕۑ�sharkcheck
							/* for( AIndex i = 0; i < addedIdentifier.GetItemCount(); i++ )
							{
							AObjectID	objectID = addedIdentifier.Get(i);
							ATextPoint	spt, ept;
							importFileData.GetTextInfo().GetGlobalIdentifierRange(objectID,spt,ept);
							ATextIndex	spos = importFileData.GetTextInfo().GetTextIndexFromTextPoint(spt);
							ATextIndex	epos = importFileData.GetTextInfo().GetTextIndexFromTextPoint(ept);
							importFileData.GetTextInfo().SetGlobalIdentifierStartEndIndex(objectID,spos,epos);
							}*/
							
							//win 080725 ImportRecursive�w��Ή�
							//�i���Ƃ���xxx.cpp���炳��ɂ��̃C���|�[�g�t�@�C����ǂݏo���Ȃ��悤�ɂ���B
							//�@ADocument_Text::UpdateImportFileData()�̂ق��́Axxx.h����ʊg���qxxx.cpp���ǂݏo���B�j
							AText	filename;
							importFile.GetFilename(filename);
							AText	suffix;
							filename.GetSuffix(suffix);
							if( GetApp().SPI_GetModePrefDB(modeIndex).GetSyntaxDefinition().IsImportRecursive(suffix) == true )
							{
								//TextInfo����C���|�[�g�t�@�C����Array��importFileArray�Ɏ擾����B
								AFileAcc	folder;
								folder.SpecifyParent(importFile);
								AObjectArray<AFileAcc>	importFileArray;
								importFileData.GetTextInfo().GetImportFileArray(folder,importFileArray);
								//�ċA�C���|�[�g�t�@�C���������E���[�h�i�{��͗v���j���Ēǉ�
								for( AIndex i = 0; i < importFileArray.GetItemCount(); i++ )
								{
									AObjectID	objectID = GetApp().SPI_GetImportFileManager().FindOrLoadImportFileDataByFile(importFileArray.GetObjectConst(i),false);
									if( objectID != kObjectID_Invalid )
									{
										if( modeIndex == GetApp().SPI_GetImportFileManager().GetImportFileDataByID(objectID).GetModeIndex() )
										{
											importFileData.GetRecursiveImportFileDataObjectIDArray().Add(objectID);
										}
									}
								}
							}
							
							//#693 �������ߖ�̂���RegExp�f�[�^��Purge
							importFileData.GetTextInfo().PurgeForImportFileData();
						}
					}
					//��͍ς݂ɂ���
					{
						AStMutexLocker	mutexlocker(mRequestQueueMutex);
						//���݂̉��ImportFileData�I�u�W�F�N�g�ϐ��N���A
						mRequestQueue_CurrentProcessingImportFileDataObjectID = kObjectID_Invalid;
						//�F���ς݃t���OON
						importFileData.SetRecognized();
					}
				}
			}
			if( NVM_IsAborted() == false )
			{
				//�����ʒm
				ABase::PostToMainInternalEventQueue(kInternalEvent_ImportFileRecognizeCompleted,GetObjectID(),0,GetEmptyText(),GetEmptyObjectIDArray());
			}
			
			//==================purge==================
			//#896
			//purge�L���[�ɂ܂ꂽ�S�Ă�Import File Data�̃f�[�^��purge����
			{
				AStMutexLocker	mutexlocker(mRequestQueueMutex);
				while( mImportFileDataPurgeRequestQueue.GetItemCount() > 0 )
				{
					if( NVM_IsAborted() == true )   break;
					
					//�L���[����擾
					AObjectID	importFileDataObjectID = mImportFileDataPurgeRequestQueue.Get(0);
					mImportFileDataPurgeRequestQueue.Delete1(0);
					
					//purge���s
					GetApp().SPI_GetImportFileManager().GetImportFileDataByID(importFileDataObjectID).DeleteTextInfoData();
				}
			}
			if( NVM_IsAborted() == true )   break;
		}
	}
	catch(...)
	{
		_ACError("AImportFileRecognizer::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AImportFileRecognizer::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================
#pragma mark [�N���X]AImportFileData
#pragma mark ===========================
//ImportFile���Ƃ̃f�[�^

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

//�R���X�g���N�^
AImportFileData::AImportFileData( AObjectArrayItem* inParent, const AFileAcc& inFile ) 
	: AObjectArrayItem(inParent), mTextInfo(this), mRecognized(false)
	,mRevisionNumber(0)//#423
,mModeIndex(kStandardModeIndex)
{
	mFile.CopyFrom(inFile);
}
void	AImportFileData::Init( const ABool inIsSystemHeader )//B0000 mutex�������Init���� #852
{
	mModeIndex = GetApp().SPI_GetModeIndexFromFile(mFile);
	
	AStImportFileDataLocker	locker(*this);
	
	mTextInfo.SetMode(mModeIndex,false);
	//#467 �V�X�e���w�b�_���肵��TextInfo�֐ݒ�B�idirective�𖳌��ɂ��邽�߁B�j
	//#852 if( GetApp().SPI_GetModePrefDB(mModeIndex).IsSystemHeaderFile(mFile) == true )
	if( inIsSystemHeader == true )//#852
	{
		mTextInfo.SetSystemHeaderMode();
	}
}
//�f�X�g���N�^
AImportFileData::~AImportFileData()
{
}

//#896
/**
text info�f�[�^��S�č폜(purge)
*/
void	AImportFileData::DeleteTextInfoData()
{
	AStImportFileDataLocker	locker(*this);
	
	mTextInfo.DeleteAllInfo();
	mRecognized = false;
	mRevisionNumber = 0;
}




