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

#pragma once

#include "ABaseTypes.h"
#include "AObjectArrayItem.h"
#include "AHashArray.h"
#include "AStArrayPtr.h"

/**
��ӂƂȂ�ID�𐶐����AIndex�ԍ��ƌ��ѕt�����s���z��N���X
*/
class AUniqIDArray
{
	//�R���X�g���N�^�E�f�X�g���N�^
  public:
	AUniqIDArray( const AItemCount inInitialAllocateCount = 64, const AItemCount inReallocateStepCount = 64 );
	~AUniqIDArray();
	
	//UniqID�����E�폜�i+Index�ԍ��Ƃ̌��т��j
  public:
	void				InsertItems( const AIndex inIndex, const AItemCount inInsertCount );
	void				Insert1Item( const AIndex inIndex );
	AIndex				AddItem();
	void				DeleteItems( const AIndex inIndex, const AItemCount inDeleteCount );
	void				Delete1Item( const AIndex inIndex );
	void				DeleteAll();
  private:
	void				AssignUniqIDs( const AIndex inIndex, const AItemCount inCount );
	
	//�擾�E����
  public:
	AUniqID				Get( const AIndex inIndex ) const;
	AIndex				Find( const AUniqID inUniqID ) const;
	
	//UnusedItem�Ǘ��i�폜�}�[�N���ځj
  public:
	AIndex				ReuseUnusedItem();
	void				Unuse1Item( AIndex inIndex );
	
	/**
	�������g�厞�̍Ċ��蓖�đ����ʂ̐ݒ�
	*/
  public:
	void	SetReallocateStep( const AItemCount inReallocateCount )
	{
		mUniqIDArray.SetReallocateStep(inReallocateCount);
		mUniqIDToIndexArray.SetReallocateStep(inReallocateCount);
	}
	
	//�f�[�^
  private:
	//��z��
	//UniqID���i�[����z��
	AArray<AUniqID>						mUniqIDArray;
	
	//�⏕�z��
	//index:UniqID��  ���g:��z���index
	AArray<AIndex>						mUniqIDToIndexArray;
	static const ANumber				kUniqIDValMin = 1;
	
	//mUnusedItemIndexArray: Unuse�}�[�N���������ڂ̔z��index���i�[����
	AArray<AIndex>						mUnusedItemIndexArray;
	//���������蓖�ăp�����[�^
	static const AItemCount				kUnusedItemIndexArray_InitialAllocateCount = 10000;
	static const AItemCount				kUnusedItemIndexArray_ReallocateStepCount = 10000;
	
	//mUnusignedUniqIDValArray: mUniqIDToIndexArray�ɑ��݂���UniqID�̂��������蓖�āi�l��kIndex_Invalid�j�̂��̂��i�[����
	AArray<ANumber>						mUnusignedUniqIDValArray;
	//���������蓖�ăp�����[�^
	static const AItemCount				kUnusignedUniqIDValArray_InitialAllocateCount = 10000;
	static const AItemCount				kUnusignedUniqIDValArray_ReallocateStepCount = 10000;
	
};

