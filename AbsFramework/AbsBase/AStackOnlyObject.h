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

AStackOnlyObject

*/

#pragma once

#include "../AbsBase/ABaseTypes.h"

#pragma mark ===========================
#pragma mark [�N���X]AStackOnlyObject
/**
�X�^�b�N��p�N���X�̂��߂̊��N���X
�R�s�[�^��������q�Anew/delet���Z�q���g�p�s�\�ɂ���
*/
class AStackOnlyObject
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AStackOnlyObject() {};
	virtual ~AStackOnlyObject() {};
	
	//�R�s�[�����q�^������Z�q�^new�̓T�u�N���X�Ŗ����I�ɒ�`����Ȃ�����A�g�p�s�B
  protected:
	AStackOnlyObject( const AStackOnlyObject& );//�R�s�[�����q
	AStackOnlyObject&	operator=( const AStackOnlyObject& );//������Z�q
	static void*	operator new(size_t);//new���Z�q
	static void*	operator new[](size_t);//new[]���Z�q
	//static void	operator delete(void*);//delete���Z�q
	//static void	operator delete[](void*);//delete[]���Z�q
};




