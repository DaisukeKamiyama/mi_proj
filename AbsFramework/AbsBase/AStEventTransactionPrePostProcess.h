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

#pragma once

#include "ABaseTypes.h"
#include "AStackOnlyObject.h"

class AObjectArrayItem;

#pragma mark ===========================
#pragma mark [�N���X]AStEventTransactionPrePostProcess
/**�C�x���g�g�����U�N�V�����O�����^�㏈���p�N���X
OS�����M�����}�E�X�N���b�N���̃C�x���g�g�����U�N�V�����̊ԁA���̃I�u�W�F�N�g�𐶐�����B
��̓I�ɂ́A�C�x���g�������W���[���̍ŏ��ɁA���[�J���ϐ��Ƃ��Ē�`����B
����ɂ��AAObjectArray<>�Łi���j�폜���ꂽ�I�u�W�F�N�g��delete���A�C�x���g�����̑O�����A�㏈�����s���B
*/
class AStEventTransactionPrePostProcess : public AStackOnlyObject
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AStEventTransactionPrePostProcess( const ABool inPeriodical );
	~AStEventTransactionPrePostProcess();
	
	//1�C�x���g�����I�����ɁAdelete����AObjectArrayItem��o�^����B
  public:
	void	DeleteWhenEventEnd( AObjectArrayItem* inObjectPtr );
	
  private:
	//�C�x���g�����̃R�[���̐[��
	static ANumber sEventDepth;
	//
	static ABool	sPeriodical;
};


