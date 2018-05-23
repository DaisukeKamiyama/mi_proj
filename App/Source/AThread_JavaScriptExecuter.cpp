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

AThread_JavaScriptExecuter
#904

*/

#include "stdafx.h"

#include "AThread_JavaScriptExecuter.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AThread_JavaScriptExecuter
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AThread_JavaScriptExecuter::AThread_JavaScriptExecuter( AObjectArrayItem* inParent ) : AThread(inParent), mIsCompleted(false)
{
}

/**
�f�X�g���N�^
*/
AThread_JavaScriptExecuter::~AThread_JavaScriptExecuter()
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/**
�X���b�h���C�����[�`��
*/
void	AThread_JavaScriptExecuter::NVIDO_ThreadMain()
{
	//JavaScript���s
	GetApp().NVI_DoJavaScript(mScript,mFolder);
	//�����t���O��ON
	mIsCompleted = true;
	//���C���X���b�h���N����������Ɋ����t���O���肪���s�����
	GetApp().NVI_WakeTrigger();
}

#pragma mark ===========================

#pragma mark ---

/**
JavaScript���s
*/
ABool	AThread_JavaScriptExecuter::SPNVI_Run_ExecuteJavaScript( const AText& inScriptText, const AFileAcc& inFolder )
{
	//�X���b�h���s���Ȃ牽�����Ȃ�
	if( NVI_IsThreadWorking() == true )
	{
		return false;
	}
	
	//�v��JavaScript�ݒ�
	mScript.SetText(inScriptText);
	mFolder = inFolder;
	
	//�����t���OOFF
	mIsCompleted = false;
	
	//�X���b�h���s
	NVI_Run();
	return true;
}

