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

AView_Footer

*/

#include "stdafx.h"

#include "AView_Footer.h"
#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_Footer
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_Footer::AView_Footer( const AWindowID inWindowID, const AControlID inID ) : AView(inWindowID,inID)
{
	NVMC_SetOffscreenMode(true);//win
}

//�f�X�g���N�^
AView_Footer::~AView_Footer()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_Footer::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_Footer::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_Footer::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	return false;
}

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_Footer::EVTDO_DoDraw()
{
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	
	/*
	{
		ALocalRect	rect = viewRect;
		NVMC_PaintRectWithVerticalGradient(rect,AColorWrapper::GetColorByHTMLFormatColor("cbcbcb"),
					AColorWrapper::GetColorByHTMLFormatColor("a7a7a7"),1.0,1.0);
	}
	*/
	//
	ALocalPoint	spt = {viewRect.left,0}, ept = {viewRect.right,0};
	NVMC_DrawLine(spt,ept,kColor_Gray40Percent);//AColorWrapper::GetColorByHTMLFormatColor("4c4b4b"),1.0);
	
	/*call graf���Ɠ����F
	//�w�i�`��
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	//if( NVM_GetWindow().NVI_IsWindowActive() == true )
	{
		//NVMC_PaintRoundedRect(localFrameRect,mBackgroundActiveColor,mBackgroundTransparency);
		NVMC_PaintRoundedRect(localFrameRect,kColor_White,kColor_Gray95Percent,kGradientType_Vertical,mBackgroundTransparency,5,false,false,true,true);
		NVMC_FrameRoundedRect(localFrameRect,kColor_Gray20Percent,1.0,10,false,false,true,true);
	}
	*/
}

#if 0
#pragma mark ===========================
#pragma mark [�N���X]AView_FooterSeparator
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_FooterSeparator::AView_FooterSeparator( const AWindowID inWindowID, const AControlID inID ) : AView(inWindowID,inID)
{
	NVMC_SetOffscreenMode(true);//win
}

//�f�X�g���N�^
AView_FooterSeparator::~AView_FooterSeparator()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_FooterSeparator::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_FooterSeparator::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_FooterSeparator::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	return false;
}

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_FooterSeparator::EVTDO_DoDraw()
{
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	
	//
	NVMC_SetDropShadowColor(kColor_Gray60Percent);
	/*
	ALocalPoint	spt = {viewRect.left,viewRect.top+2}, ept = {viewRect.left,viewRect.bottom-4};
	NVMC_DrawLine(spt,ept,kColor_Gray30Percent,1.0,0.0,1.0,true);//AColorWrapper::GetColorByHTMLFormatColor("4c4b4b"),1.0);
	*/
	ALocalRect	rect = {0};
	rect.left		= viewRect.left;
	rect.top		= viewRect.top+4;
	rect.right		= viewRect.left + 1;
	rect.bottom		= viewRect.bottom-4;
	NVMC_PaintRect(rect,kColor_Gray30Percent,1.0,true);
}
#endif

