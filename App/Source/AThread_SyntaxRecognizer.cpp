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

AThread_SyntaxRecognizer
#698

*/

#include "stdafx.h"

#include "AThread_SyntaxRecognizer.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AThread_SyntaxRecognizer
#pragma mark ===========================
//�}���`�t�@�C���������s

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AThread_SyntaxRecognizer::AThread_SyntaxRecognizer( AObjectArrayItem* inParent ) : AThread(inParent), mTextInfo(this),
		mTextDocumentID(kObjectID_Invalid), mRecognizerJudgedAsEnd(false), mRecognizeEndLineIndex(kIndex_Invalid),
		mModeIndex(kIndex_Invalid)
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/**
�X���b�h���C�����[�`��
*/
void	AThread_SyntaxRecognizer::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_SyntaxRecognizer::NVIDO_ThreadMain started.",this);
	try
	{
		while( NVM_IsAborted() == false )//NVI_AbortThread()���R�[������Ă�����X���b�h�I��
		{
			//pause�����܂ő҂�
			NVM_WaitForUnpaused();
			//Abort����Ă�����I���i���@�F���������A���C���X���b�h����NVI_AbortThread()+Unpause()�ɂ��I�������B�j
			if( NVM_IsAborted() == true )
			{
				break;
			}
			//���@�F�����s
			AArray<AUniqID>	deletedIdentifier;
			AArray<AUniqID>	addedGlobalIdentifier;
			AArray<AIndex>	addedGlobalIdentifierLineIndex;
			ABool	importChanged = false;
			{
				AStMutexLocker	locker(mTextInfoMutex);
				mRecognizeEndLineIndex = mTextInfo.ExecuteRecognizeSyntax(mText,kIndex_Invalid,
																		  deletedIdentifier,addedGlobalIdentifier,addedGlobalIdentifierLineIndex,importChanged,mAborted);
				if( NVM_IsAborted() == true )   break;
				//ExecuteRecognizeSyntax()�ɂ��F�������Ɣ��f���ꂽ���ǂ�����mRecognizerJudgedAsEnd�Ɋi�[����
				//�i�F�������Ɣ��f���ꂽ�炻�̍s�ŔF���I���ƂȂ��Ă���j
				mRecognizerJudgedAsEnd = (mRecognizeEndLineIndex < mTextInfo.GetLineCount());
			}
			//fprintf(stderr,"SyntaxRecognizerThreadPause:%d ",mRecognizeEndLineIndex);
			//�X���b�h��pause��Ԃɂ���
			NVI_Pause();
			//���C���X���b�h�֍���˗����I����ʒm
			if( NVM_IsAborted() == false )
			{
				AObjectIDArray	objectIDArray;
				objectIDArray.Add(mTextDocumentID);
				ABase::PostToMainInternalEventQueue(kInternalEvent_SyntaxRecognizerPaused,GetObjectID(),0,GetEmptyText(),objectIDArray);
			}
			//�X���b�h�I���́A���C���X���b�h�Ŕ��f����B
			//���@�F���������iFindFirstUnrecognizedLine()�ɂ�薢�F�����Ȃ��Ɣ��f���ꂽ�Ƃ��j�́A
			//NVI_AbortThread()+Unpause()�ɂ��X���b�h�I������B
		}
	}
	catch(...)
	{
		_ACError("AThread_SyntaxRecognizer::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	//text info��reg exp�I�u�W�F�N�g��purge
	{
		AStMutexLocker	locker(mTextInfoMutex);
		mTextInfo.PurgeRegExpObject();
	}
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_SyntaxRecognizer::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---�X���b�h�ݒ�E�N��

/**
�����ݒ�
*/
void	AThread_SyntaxRecognizer::SPI_Init( const ADocumentID inTextDocumentID )
{
	mTextDocumentID = inTextDocumentID;
}

/**
���@�F���R���e�L�X�g��ݒ�i���C���X���b�h���炱�̃X���b�h�֏��R�s�[�j
*/
void	AThread_SyntaxRecognizer::SPI_SetContext( const AModeIndex inModeIndex, const ATextInfo& inSrcTextInfo, const AIndex inStartLineIndex )
{
	AStMutexLocker	locker(mTextInfoMutex);
	//���[�h�ݒ�
	if( mModeIndex != inModeIndex )
	{
		mModeIndex = inModeIndex;
		//TextInfo�Ƀ��[�h�ݒ�
		mTextInfo.SetMode(mModeIndex);
	}
	//�R���e�L�X�g���擾
	mTextInfo.CopyRecognizeContext(inSrcTextInfo,inStartLineIndex);
}

/**
�e�L�X�g�ݒ�i���C���X���b�h���炱�̃X���b�h�֏��R�s�[�j
*/
void	AThread_SyntaxRecognizer::SPI_SetText( const AText& inText, const AIndex inIndex, const AItemCount inCount )
{
	inText.GetText(inIndex,inCount,mText);
}

/**
TextInfo�̑Ώۍs�̏���ݒ�i���C���X���b�h���炱�̃X���b�h�֏��R�s�[�j
*/
void	AThread_SyntaxRecognizer::SPI_SetTextInfo( const ATextInfo& inSrcTextInfo, 
		const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	AStMutexLocker	locker(mTextInfoMutex);
	mTextInfo.CopyLineInfoToSyntaxRecognizer(inSrcTextInfo,inStartLineIndex,inEndLineIndex);
}

/**
text info�̐��K�\���I�u�W�F�N�g��purge
*/
void	AThread_SyntaxRecognizer::SPI_PurgeTextinfoRegExp()
{
	//���̊֐��̓��C���X���b�h����R�[�������B�i���̂��߁AmTextInfoMutex�̔r�����K�v�j
	AStMutexLocker	locker(mTextInfoMutex);
	mTextInfo.PurgeRegExpObject();
}

#pragma mark ===========================

#pragma mark ---�X���b�h�I���㏈��

/**
�X���b�h���f�[�^�폜
*/
void	AThread_SyntaxRecognizer::ClearData()
{
	//���@�F���������Ƀf�[�^���폜�i�������ߖ�j
	//SPI_Init()�Őݒ肵���f�[�^�͍폜���Ȃ�
	mText.DeleteAll();
	{
		AStMutexLocker	locker(mTextInfoMutex);
		mTextInfo.DeleteLineInfoAll();
	}
}

