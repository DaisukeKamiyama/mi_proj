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

CRebarContent

*/

#include "stdafx.h"

#include "CRebarContent.h"
#include "../Frame.h"
#include <Commdlg.h>

#pragma mark ===========================
#pragma mark [�N���X]CRebarContent
#pragma mark ===========================
//Rebar�R���g���[��

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
CRebarContent::CRebarContent( AObjectArrayItem* inParent, CWindowImp& inWindow ) 
	: CUserPane(inParent,inWindow)
{
}

//�f�X�g���N�^
CRebarContent::~CRebarContent()
{
}

#pragma mark ===========================

#pragma mark ---�C�x���g�n���h��

//�`��
ABool	CRebarContent::DoDraw()
{
	
	return true;
}

//�}�E�X�N���b�N�ɑ΂��鏈�� 
ABool	CRebarContent::DoMouseDown( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
									  const ANumber inClickCount, const ABool inRightButton )
{
	if( IsControlEnabled() == false )   return false;
	
	
	return true;
}

