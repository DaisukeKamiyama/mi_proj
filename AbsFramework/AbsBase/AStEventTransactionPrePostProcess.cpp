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

AStEventTransactionPrePostProcess

*/

#include "stdafx.h"

#include "AStEventTransactionPrePostProcess.h"
#include "ABaseFunction.h"

extern void	EventTransactionPostProcess( const ABool inPeriodical );

#pragma mark ===========================
#pragma mark [�N���X]AStEventTransactionPrePostProcess
#pragma mark ===========================
//�C�x���g�g�����U�N�V�����O�����^�㏈���p�N���X
//OS�����M�����}�E�X�N���b�N���̃C�x���g�g�����U�N�V�����̊ԁA���̃I�u�W�F�N�g�𐶐�����B

//�C�x���g�������AOS API�R�[���ɂ��A�R�[���o�b�N�ŃC�x���g�������W���[�����Ă΂�邱�Ƃ�����̂ŁA
//�C�x���g�����̃R�[���̐[����sEventDepth�ŊǗ�����B
ANumber AStEventTransactionPrePostProcess::sEventDepth = 0;
//
ABool	AStEventTransactionPrePostProcess::sPeriodical = false;

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AStEventTransactionPrePostProcess::AStEventTransactionPrePostProcess( const ABool inPeriodical )
{
	if( inPeriodical == true )
	{
		sPeriodical = inPeriodical;
	}
	
	//�C�x���g�����̃R�[���̐[����+1
	AStEventTransactionPrePostProcess::sEventDepth++;
}

//�f�X�g���N�^
AStEventTransactionPrePostProcess::~AStEventTransactionPrePostProcess()
{
	try
	{
		//�C�x���g�����̃R�[���̐[����1�̂Ƃ��A�܂�A�ŏI�I�ɑҋ@��Ԃɖ߂�Ƃ����ǂ����𔻒肷��B
		if( AStEventTransactionPrePostProcess::sEventDepth == 1 )
		{
			//�C�x���g��������Garbage�ɒǉ����ꂽObject���A���ۂ�delete����B
			ABaseFunction::DoObjectArrayItemTrashDelete();
			
			EventTransactionPostProcess(sPeriodical);
			
			sPeriodical = false;
		}
		
		//�C�x���g�����̃R�[���̐[����-1
		AStEventTransactionPrePostProcess::sEventDepth--;
	}
	catch(...)
	{
	}
	
}

