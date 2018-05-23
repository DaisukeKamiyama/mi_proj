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

AView_SubTextShadow

*/

#include "stdafx.h"

#include "AView_SubTextShadow.h"
#include "AView_LineNumber.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_SubTextShadow
#pragma mark ===========================
/*
���C���e�L�X�g�J�����ƃT�u�e�L�X�g�J�����̊Ԃɕ������i�V���h�E�j��\��
*/

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_SubTextShadow::AView_SubTextShadow( const AWindowID inWindowID, const AControlID inID ) : AView(inWindowID,inID)
{
	NVMC_SetOffscreenMode(true);
}

//�f�X�g���N�^
AView_SubTextShadow::~AView_SubTextShadow()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================



#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_SubTextShadow::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_SubTextShadow::NVIDO_DoDeleted()
{
}


/**
�`��v�����̃R�[���o�b�N(AView�p)
*/
void	AView_SubTextShadow::EVTDO_DoDraw()
{
	//frame�擾
	ALocalRect	frameRect;
	NVM_GetLocalViewRect(frameRect);
	
	//������E�փ��l��ύX����gradient�\��
	ALocalRect	shadowrect = frameRect;
	NVMC_PaintRectWithHorizontalGradient(shadowrect,kColor_Gray,kColor_Gray,0.0,0.3);
}

/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_SubTextShadow::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	return false;
}


