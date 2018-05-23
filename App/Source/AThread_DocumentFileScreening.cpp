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

AThread_DocumentFileScreening

*/

#include "stdafx.h"

#include "AThread_DocumentFileScreening.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AThread_DocumentFileScreening
#pragma mark ===========================
//�t�@�C���̃X�N���[�j���O���s
//Mac OS X�ł́A�l�b�g���[�N�t�@�C����̃t�@�C���ɂ��āA�����[�g�����l�b�g���[�N�ؒf���ꂽ�ꍇ�A
//�t�@�C���A�N�Z�X����NG�Ɣ��肷��܂ŁA���Ɏ��Ԃ�������i90�b���x�j�B
//���̊ԁA�֐��̓u���b�N����̂ŁA���C���X���b�h�Ŏ��s����ƃA�v������~����B

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AThread_DocumentFileScreening::AThread_DocumentFileScreening( AObjectArrayItem* inParent ) : AThread(inParent), mDocumentID(kObjectID_Invalid)
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/**
�X���b�h���C�����[�`��
*/
void	AThread_DocumentFileScreening::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_DocumentFileScreening::NVIDO_ThreadMain started.",this);
	try
	{
		//document�I�u�W�F�N�g���擾���Aretain����i�X���b�h������R�[������邽�߁Aretain����document�I�u�W�F�N�g�������Ȃ��悤�ɂ���j
		ADocument*	doc = GetApp().NVI_GetDocumentAndRetain(mDocumentID);
		if( doc != NULL )
		{
			//document�I�u�W�F�N�g��retain��K��������邽�߂̃X�^�b�N�N���X
			AStDecrementRetainCounter	releaser(doc);
			
			//document�擾
			ADocument_Text&	textdoc = reinterpret_cast<ADocument_Text&>(*doc);
			
			//�X�N���[�j���O���s
			textdoc.SPI_ScreenDocumentFile();
		}
	}
	catch(...)
	{
		_ACError("AThread_DocumentFileScreening::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	//�����ʒm
	AObjectIDArray	objectIDArray;
	objectIDArray.Add(mDocumentID);
	ABase::PostToMainInternalEventQueue(kInternalEvent_DocumentFileScreenCompleted,
										GetObjectID(),0,GetEmptyText(),objectIDArray);
	//���C���X���b�h��sleep���������邽�߂̃g���K�[
	GetApp().NVI_WakeTrigger();
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_DocumentFileScreening::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---�X���b�h�N��

/**
�X���b�h�N��
*/
void	AThread_DocumentFileScreening::SPNVI_Run( const ADocumentID inDocumentID )
{
	if( NVI_IsThreadWorking() == true )   {_ACError("",this);return;}
	
	//�v���f�[�^���R�s�[
	mDocumentID = inDocumentID;
	
	//�X���b�h�N��
	NVI_Run();
}

