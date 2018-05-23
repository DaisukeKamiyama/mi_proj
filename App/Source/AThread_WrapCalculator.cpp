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

AThread_WrapCalculator
#699

*/

#include "stdafx.h"

#include "AThread_WrapCalculator.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AThread_WrapCalculator
#pragma mark ===========================
//�}���`�t�@�C���������s

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AThread_WrapCalculator::AThread_WrapCalculator( AObjectArrayItem* inParent ) : AThread(inParent), mTextInfo(this),
		mTextDocumentID(kObjectID_Invalid), 
		mFontSize(9.0), mAntiAliasing(true), mTabWidth(4), mIndentWidth(4), mWrapMode(kWrapMode_NoWrap),
		mWrapLetterCount(80), mViewWidth(800), mCountAs2Letters(false)
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/*
�X���b�h����
1. �s�܂�Ԃ��v�Z�K�v�ȂƂ��ɃX���b�h�N��(SPNVI_Run)
2. ����(kCalculateLimitLineCount)����������A���C���X���b�h�փC�x���g�ʒm����wait
3. ���C���X���b�h�͏I�����̔��f��������A�X���b�h��wait����
4. 2,3���A�S�Ẵe�L�X�g�̌v�Z���I������܂Ōp��

mTextInfo���̊e�f�[�^�́A
mWaitState��true�̂Ƃ��i�X���b�h��wait��ԁj�́A���C���X���b�h����ݒ�E�Q�ƁA
mWaitState��false�̂Ƃ��i�X���b�h��wait��ԉ����j�́A���̃X���b�h����ݒ�E�Q�Ƃ���B
*/

/**
�X���b�h���C�����[�`��
*/
void	AThread_WrapCalculator::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_WrapCalculator::NVIDO_ThreadMain started.",this);
	try
	{
		while( NVM_IsAborted() == false )//NVI_AbortThread()���R�[������Ă�����X���b�h�I��
		{
			//pause�����܂ő҂�
			NVM_WaitForUnpaused();
			//Abort����Ă�����I���i�܂�Ԃ��v�Z�������A���C���X���b�h����NVI_AbortThread()+Unpause()�ɂ��I�������B�j
			if( NVM_IsAborted() == true )
			{
				break;
			}
			//�s�܂�Ԃ��v�Z���s
			//�i�S�ďI���A�܂��́AkCalculatorThreadPauseLineCount�s�I��������A�֐�����߂��Ă���B
			//��������false�Ȃ̂�EOF��s�͒ǉ�����Ȃ��B�j
			AItemCount	completedTextLen = mTextInfo.CalcLineInfoAll(mText,false,kWrapCalculatorThreadPauseLineCount,
						mFontName,mFontSize,mAntiAliasing,mTabWidth,mIndentWidth,mWrapMode,mWrapLetterCount,
						mViewWidth,mCountAs2Letters);
			//Wrap�v�Z�I�����̃e�L�X�g�͍폜�B����mText�̍ŏ�����v�Z����B
			mText.Delete(0,completedTextLen);
			//�X���b�h��pause��Ԃɂ���
			NVI_Pause();
			//���C���X���b�h�֍��񕪏I����ʒm
			//ABase::PostToMainInternalEventQueue()����Ƀ��C���X���b�h���������S�Ď��s�����\��������B
			//���̏ꍇ�AContinueWrapCalculator()�̃R�[���܂ł��S�Ď��s�����̂ŁANVI_Pause()�͂��̑O�Ŏ��s���邱�Ƃ�
			//���s����NVI_Unpause()(=ContinueWrapCalculator())��NVI_Pause()�ƂȂ��Ă��܂�Ȃ��悤�ɂ���Ȃǂ̍l�������Ă���B
			//�t�ɁA���C���X���b�h�������̂ق����������̏ꍇ�����邪�A���̏ꍇ�́ANVM_WaitForUnpaused()�ő҂��Ă���ԂɁA
			//NVI_Unpause()(=ContinueWrapCalculator())���R�[�������B
			AObjectIDArray	objectIDArray;
			objectIDArray.Add(mTextDocumentID);
			ABase::PostToMainInternalEventQueue(kInternalEvent_WrapCalculatorPaused,GetObjectID(),0,GetEmptyText(),objectIDArray);
			//�X���b�h�I���́A���C���X���b�h�Ŕ��f����B
			//�܂�Ԃ��v�Z�������iCopyFromCalculatorResult()�ɂ��I���Ɣ��f���ꂽ�Ƃ��j�́A
			//NVI_AbortThread()+Unpause()�ɂ��X���b�h�I������B
		}
	}
	catch(...)
	{
		_ACError("AThread_WrapCalculator::NVIDO_ThreadMain() caught exception.",NULL);
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_WrapCalculator::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---�X���b�h�ݒ�E�N��

/**
�f�[�^������
*/
void	AThread_WrapCalculator::SPI_Init( const ADocumentID inTextDocumentID )
{
	//�h�L�������gID��ݒ�
	mTextDocumentID = inTextDocumentID;
}

/**
mText�ݒ�
StartWrapCalculator()����R�[�������B�i�s����v�Z�J�n���Ɉ��ݒ肳���B�j
*/
void	AThread_WrapCalculator::SPI_SetText( const AText& inText, const AIndex inIndex, const AItemCount inCount )
{
	//mText�ݒ�
	inText.GetText(inIndex,inCount,mText);
}

/**
Wrap�p�����[�^�ݒ�
*/
void	AThread_WrapCalculator::SPI_SetWrapParameters( 
		const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
		const AItemCount inTabWidth, const AItemCount inIndentWidth, 
		const AWrapMode inWrapMode, const AItemCount inWrapLetterCount, 
		const ANumber inViewWidth, const ABool inCountAs2Letters )
{
	mFontName = inFontName;
	mFontSize = inFontSize;
	mAntiAliasing = inAntiAliasing;
	mTabWidth = inTabWidth;
	mIndentWidth = inIndentWidth;
	mWrapMode = inWrapMode;
	mWrapLetterCount = inWrapLetterCount;
	mViewWidth = inViewWidth;
	mCountAs2Letters = inCountAs2Letters;
}

#pragma mark ===========================

#pragma mark ---�X���b�h�I���㏈��

/**
�X���b�h���f�[�^�폜
*/
void	AThread_WrapCalculator::ClearData()
{
	//�s�܂�Ԃ��v�Z�������Ƀf�[�^���폜�i�������ߖ�j
	//�y�[�X�g�����ɍēx�v�Z���s����̂�SPI_Init()�Őݒ肵���f�[�^�͍폜���Ȃ�
	mText.DeleteAll();
	mTextInfo.DeleteLineInfoAll();
}

