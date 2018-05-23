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

#pragma once

#include "ABaseTypes.h"
#include "AObjectArrayItem.h"
#include "AArray.h"

#pragma mark ===========================
#pragma mark [�N���X]AObjectArrayItemTrash
/**
ObjectArrayItem�S�~���i�K�[�x�[�W�j

ObjectArray����I�u�W�F�N�g���폜����ƁA�����ɂȂ���āA
�C�x���g�g�����U�N�V�����Ō�Œ��g�����ۂɍ폜����B
�|�C���^�s���Q�Ƃ�h���d�g�݁B
*/
class AObjectArrayItemTrash
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AObjectArrayItemTrash();
	~AObjectArrayItemTrash();
	
	//
  public:
	void	AddToObjectArrayItemTrash( AObjectArrayItem* inObjectPtr );
	void	DoObjectArrayItemTrashDelete();
	
	//
  private:
	AArray<AObjectArrayItem*>	mTrash;
};


