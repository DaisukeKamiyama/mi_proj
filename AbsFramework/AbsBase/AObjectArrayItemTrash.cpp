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

AObjectArrayItemTrash

*/

#include "stdafx.h"

#include "ABase.h"

#pragma mark ===========================
#pragma mark [�N���X]AddToObjectArrayItemTrash
#pragma mark ===========================
//ObjectArrayItem�S�~��
#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

//�R���X�g���N�^
AObjectArrayItemTrash::AObjectArrayItemTrash()
{
	mTrash.SetThrowIfOutOfBounds();//B0000
}
//�f�X�g���N�^
AObjectArrayItemTrash::~AObjectArrayItemTrash()
{
}

#pragma mark ===========================

#pragma mark ---�폜����

void	AObjectArrayItemTrash::AddToObjectArrayItemTrash( AObjectArrayItem* inObjectPtr )
{
	mTrash.Add(inObjectPtr);
}

void	AObjectArrayItemTrash::DoObjectArrayItemTrashDelete()
{
	//#717 while(mTrash.GetItemCount()>0)
	for( AIndex index = mTrash.GetItemCount()-1; index >= 0; index-- )//#717
	{
		AObjectArrayItem*	ptr = mTrash.Get(index);//#717 mTrash.GetItemCount()-1);
		if( ptr->GetRetainCount() <= 0 )//#717 retain����Ă���Ƃ��͍���͍폜���Ȃ��B
		{
			//�I�u�W�F�N�g�����S�ɍ폜����B
			//�Ȃ��ADelete1()���ŕʂ̃I�u�W�F�N�g���폜���邱�Ƃɂ��AmTrash�ɐV���ɒǉ�����邱�Ƃ�����B
			//���̏ꍇ�A����͍폜�����ɁA���̃C�x���g�g�����U�N�V�����I�����ɍ폜���邱�Ƃɂ���B
			//����́A���X���݂��Ă���mTrash���A���̊֐��R�[�����̍Ō�̈ʒu����O�֏��ɍ폜����B
			mTrash.Delete1(index);//#717 mTrash.GetItemCount()-1);
			delete ptr;
		}
	}
	//mTrash�ɃI�u�W�F�N�g���c���Ă���ꍇ��fprintf�\�� #717
	//�imTrash�Ɏc�����܂܂ɂȂ�Ȃ����Ƃ̊m�F�̂��߁j
	/*
	if( true )
	{
		if( mTrash.GetItemCount() > 0 )
		{
			fprintf(stderr,"AObjectArrayItemTrash::DoObjectArrayItemTrashDelete() trash remained:%d ",mTrash.GetItemCount());
		}
	}
	*/
}


