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

AWrapData
//#532

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [�N���X]AWrapData
//�����E�C���h�E
class AWrapData
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWrapData();
	~AWrapData();
	
	//�ǉ��E�폜
  public:
	void					Insert1( const AIndex inLineIndex );
	void					Add();
	void					Delete( const AIndex inLineIndex, const AItemCount inDeleteLineCount );
	void					DeleteAll();
	
	//Wrap�X�V
  public:
	void					Update( const AIndex inLineIndex );
	
	//
  private:
	AObjectArray< AArray<AIndex> >			mWrapOffsetArray;
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWrapData"; }
};





