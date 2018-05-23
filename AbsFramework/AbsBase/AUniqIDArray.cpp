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

AUniqIDArray
#693

*/

#include "AUniqIDArray.h"

//UniqID��Unuse�}�[�N�̒�`
const AUniqID	kUniqID_UnusedMark = {-2};

#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�E�f�X�g���N�^

/**
�R���X�g���N�^
*/
AUniqIDArray::AUniqIDArray( const AItemCount inInitialAllocateCount, const AItemCount inReallocateStepCount ) : 
		mUniqIDArray(NULL,inInitialAllocateCount,inReallocateStepCount),
		mUniqIDToIndexArray(NULL,inInitialAllocateCount,inReallocateStepCount),
		mUnusedItemIndexArray(NULL,kUnusedItemIndexArray_InitialAllocateCount,kUnusedItemIndexArray_ReallocateStepCount),
		mUnusignedUniqIDValArray(NULL,kUnusignedUniqIDValArray_InitialAllocateCount,kUnusignedUniqIDValArray_ReallocateStepCount)
{
}

/**
�f�X�g���N�^
*/
AUniqIDArray::~AUniqIDArray()
{
}

#pragma mark ===========================

#pragma mark ---UniqID�����E�폜�i+Index�ԍ��Ƃ̌��т��j

/**
UniqID����z��̃C���f�b�N�XinIndex����inCount���蓖�Ă�Set����B
���̊֐��R�[�����A��z��mUniqIDArray�̗v�f�͊m�ۍς݂ł���B
*/
void	AUniqIDArray::AssignUniqIDs( const AIndex inIndex, const AItemCount inCount )
{
	//currentIndex�Ɍ��݂̎�z���index�ʒu
	AIndex	currentIndex = inIndex;
	
	//�܂������蓖��UniqID���g�p����
	//mUnusignedUniqIDValArray�ɖ��g�p������UniqID�������Ă���̂Ō�납��pop���Ă���
	{
		AItemCount	itemCount = mUnusignedUniqIDValArray.GetItemCount();
		for( AIndex i = itemCount-1; i >= 0; i-- )
		{
			//UniqID��pop
			AUniqID	uniqID = {mUnusignedUniqIDValArray.Get(i)};
			mUnusignedUniqIDValArray.Delete1(i);
			//�⏕�z��mUniqIDToIndexArray���̑Ή�����index���v�Z���A�����Ɍ��݂̎�z��index��ݒ�
			AIndex	uniqIDToIndexArayIndex = uniqID.val - kUniqIDValMin;
			mUniqIDToIndexArray.Set(uniqIDToIndexArayIndex,currentIndex);
			//��z��̌��݂�index��UniqID��ݒ�
			mUniqIDArray.Set(currentIndex,uniqID);
			//��z��̌��݂�index���C���N�������g���AinCount�����蓖�Ċ����Ȃ�break
			currentIndex++;
			if( currentIndex >= inIndex + inCount )
			{
				break;
			}
		}
	}
	
	//�⏕�z��mUniqIDToIndexArray�ɁA�K�v�����̗̈���m�ہBuniqIDToIndexArayIndex�Ɍ��݂̕⏕�z��mUniqIDToIndexArray��index��ݒ�B
	AIndex	uniqIDToIndexArayIndex = mUniqIDToIndexArray.GetItemCount();
	mUniqIDToIndexArray.Reserve(uniqIDToIndexArayIndex,(inIndex + inCount) - currentIndex);
	//�⏕�z��mUniqIDToIndexArray��index����UniqID���v�Z���āA���ꂼ��ݒ肵�Ă����B
	for( ; currentIndex < inIndex + inCount; currentIndex++ )
	{
		//�⏕�z��mUniqIDToIndexArray�Ɍ��݂̎�z��index��ݒ�
		mUniqIDToIndexArray.Set(uniqIDToIndexArayIndex,currentIndex);
		//��z���UniqID��ݒ�
		AUniqID	uniqID = {kUniqIDValMin + uniqIDToIndexArayIndex};
		mUniqIDArray.Set(currentIndex,uniqID);
		//�⏕�z���index���C���N�������g
		uniqIDToIndexArayIndex++;
	}
}

/**
UniqID�����EIndex�Ƃ̌��т��i�}���j
*/
void	AUniqIDArray::InsertItems( const AIndex inIndex, const AItemCount inInsertCount )
{
	//�⏕�z��mUniqIDToIndexArray��UniqID�̒l���ƂɁA�Ή������z��mUniqIDArray��index���i�[���Ă���
	//�⏕�z��mUniqIDToIndexArray��index��UniqID���ɑΉ����Ă���A
	//0�Ԗڂ̗v�f�ɂ�UniqID:{kUniqIDValMin}���蓖�Đ�̎�z��mUniqIDArray�z���index�������Ă���
	
	//�Ō�ւ̒ǉ��łȂ���΁A�⏕�z��mUniqIDToIndexArray��index�����炷
	if( inIndex < mUniqIDArray.GetItemCount() )
	{
		//Unuse�@�\�g�p���͓r���}���͖��T�|�[�g
		if( mUnusedItemIndexArray.GetItemCount() >0 )
		{
			_ACError("",NULL);
		}
		
		//�⏕�z��mUniqIDToIndexArray��index��ǉ��������炷
		AItemCount	itemCount = mUniqIDToIndexArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mUniqIDToIndexArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			if( p[i] >= inIndex )
			{
				p[i] += inInsertCount;
			}
		}
	}
	
	//��z��ɗ̈�m��
	mUniqIDArray.Reserve(inIndex,inInsertCount);
	
	//UniqID�����蓖�ĂĐݒ�
	AssignUniqIDs(inIndex,inInsertCount);
}

/**
UniqID�����EIndex�Ƃ̌��т��i1�}���j
*/
void	AUniqIDArray::Insert1Item( const AIndex inIndex )
{
	InsertItems(inIndex,1);
}

/**
UniqID�����EIndex�Ƃ̌��т��i1�ǉ��j
*/
AIndex	AUniqIDArray::AddItem()
{
	AIndex	index = mUniqIDArray.GetItemCount();
	InsertItems(index,1);
	return index;
}

/**
UniqID�폜�EIndex�Ƃ̌��т�����
*/
void	AUniqIDArray::DeleteItems( const AIndex inIndex, const AItemCount inDeleteCount )
{
	//Unuse�@�\�g�p���͍폜�͖��T�|�[�g
	if( mUnusedItemIndexArray.GetItemCount() >0 )
	{
		_ACError("",NULL);
	}
	
	//�⏕�z����̍폜���ڂ�kIndex_Invalid��ݒ肵�A�폜�����ȍ~�̍��ڂ�index�����炷
	{
		//��z�񑤃|�C���^
		AItemCount	uniqIDArrayItemCount = mUniqIDArray.GetItemCount();
		AStArrayPtr<AUniqID>	arrayptr_uniqIDArray(mUniqIDArray,0,uniqIDArrayItemCount);
		AUniqID*	p_uniqIDArray = arrayptr_uniqIDArray.GetPtr();
		//�⏕�z�񑤃|�C���^
		AItemCount	toIndexArrayItemCount = mUniqIDToIndexArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr_toIndexArray(mUniqIDToIndexArray,0,toIndexArrayItemCount);
		AIndex*	p_toIndexArray = arrayptr_toIndexArray.GetPtr();
		//��z��̍폜���ڂ���UniqID���擾���A�Ή�����⏕�z�񍀖ڂ̒l��kIndex_Invalid�ɐݒ肷��
		for( AIndex uniqIDArrayIndex = inIndex; uniqIDArrayIndex < inIndex + inDeleteCount; uniqIDArrayIndex++ )
		{
			//UniqID�擾
			AUniqID	uniqID = p_uniqIDArray[uniqIDArrayIndex];
			//�⏕�z��̒l���X�V
			p_toIndexArray[uniqID.val-kUniqIDValMin] = kIndex_Invalid;
			//�����蓖��UniqID�z���push
			mUnusignedUniqIDValArray.Add(uniqID.val);
		}
		//��z��̍폜���ڂ����UniqID���擾���A�Ή�����⏕�z�񍀖ڂ̒l�����炷
		for( AIndex uniqIDArrayIndex = inIndex + inDeleteCount; uniqIDArrayIndex < uniqIDArrayItemCount; uniqIDArrayIndex++ )
		{
			//UniqID�擾
			AUniqID	uniqID = p_uniqIDArray[uniqIDArrayIndex];
			//�⏕�z��̒l���X�V
			p_toIndexArray[uniqID.val-kUniqIDValMin] -= inDeleteCount;
		}
	}
	//��z��̍��ڂ��폜
	mUniqIDArray.Delete(inIndex,inDeleteCount);
}

/**
UniqID�폜�EIndex�Ƃ̌��т�����
*/
void	AUniqIDArray::Delete1Item( const AIndex inIndex )
{
	DeleteItems(inIndex,1);
}

/**
�S�폜
*/
void	AUniqIDArray::DeleteAll()
{
	mUniqIDArray.DeleteAll();
	mUniqIDToIndexArray.DeleteAll();
	mUnusedItemIndexArray.DeleteAll();
	mUnusignedUniqIDValArray.DeleteAll();
}

/**
Index����UniqID�擾
*/
AUniqID	AUniqIDArray::Get( const AIndex inIndex ) const
{
	return mUniqIDArray.Get(inIndex);
}

/**
UniqID����Index�擾
*/
AIndex	AUniqIDArray::Find( const AUniqID inUniqID ) const
{
	return mUniqIDToIndexArray.Get(inUniqID.val-kUniqIDValMin);
}

#pragma mark ===========================

#pragma mark ---UnusedItem�Ǘ��i�폜�}�[�N���ځj

/*
���ڂɑ΂���Unuse�}�[�N�����邱�ƂŁA���ڂ��폜�����A���g�p�̈�ɐݒ肷��B
AUniqIDArray�Ɠ������̗�i�z��j����ׂăe�[�u�����\�����邱�ƂŁA
�s�ǉ��E�폜����memmove�����Ȃ��e�[�u�����������邽�߂̋@�\�B
*/

/**
Unuse�ƂȂ������ڂ��������A�ēxUniqID������U���āA�ė��p����BUnuse���ڂ��Ȃ����kIndex_Invalid��Ԃ��B
*/
AIndex	AUniqIDArray::ReuseUnusedItem()
{
	AItemCount	unuseItemCount = mUnusedItemIndexArray.GetItemCount();
	if( unuseItemCount > 0 )
	{
		//Unuse���ڔz�񂩂�pop�i����z���index�j
		AIndex	index = mUnusedItemIndexArray.Get(unuseItemCount-1);
		mUnusedItemIndexArray.Delete1(unuseItemCount-1);
		//����index��UniqID���蓖��
		AssignUniqIDs(index,1);
		return index;
	}
	else
	{
		return kIndex_Invalid;
	}
}

/**
���ڂ�Unuse�ɐݒ肷��
*/
void	AUniqIDArray::Unuse1Item( const AIndex inIndex )
{
	//Unuse�ɂ��鍀�ڂ�UniqID���擾�A�⏕�z��̒l��kIndex_Invalid�ɐݒ�
	AUniqID	uniqID = mUniqIDArray.Get(inIndex);
	mUniqIDToIndexArray.Set(uniqID.val-kUniqIDValMin,kIndex_Invalid);
	//Unuse���ڔz���push
	mUnusignedUniqIDValArray.Add(uniqID.val);
	//��z��̍��ڂɂ�Unuse�}�[�N��ݒ�
	mUniqIDArray.Set(inIndex,kUniqID_UnusedMark);
	//Unuse���ڔz��֎�z���index��push
	mUnusedItemIndexArray.Add(inIndex);
}

