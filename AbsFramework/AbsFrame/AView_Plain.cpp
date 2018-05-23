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

AView_Plain

*/

#include "stdafx.h"

#include "../Frame.h"
#include "AView_Plain.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_Plain
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

@param inWindowID �e�E�C���h�E��ID
@param inID View��ControlID
*/
AView_Plain::AView_Plain( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mColor(kColor_White), mColorDeactive(kColor_White)
,mLeftBottomRounded(false), mRightBottomRounded(false)
,mForRightSideBarSeparator(false), mForLeftSideBarSeparator(false)
{
	//view��s�����ɐݒ� #1090
	NVMC_SetOpaque(true);
}

/**
�f�X�g���N�^
*/
AView_Plain::~AView_Plain()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_Plain::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_Plain::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_Plain::EVTDO_DoDraw()
{
	//
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	if( NVM_GetWindow().NVI_IsWindowActive() == true )
	{
		//�E�C���h�Eactive�̏ꍇ
		NVMC_PaintRoundedRect(localFrameRect,
							  mColor,mColor,kGradientType_None,1.0,1.0,
							  7.0,false,false,mLeftBottomRounded,mRightBottomRounded);//#1275 kGradientType_Horizontal��kGradientType_None
	}
	else
	{
		//�E�C���h�Edeactive�̏ꍇ
		NVMC_PaintRoundedRect(localFrameRect,
							  mColorDeactive,mColorDeactive,kGradientType_None,1.0,1.0,
							  7.0,false,false,mLeftBottomRounded,mRightBottomRounded);//#1275 kGradientType_Horizontal��kGradientType_None
	}
	//�E�T�C�h�o�[�̏ꍇ�A�����ɕ�������\��
	if( mForRightSideBarSeparator == true )
	{
		NVMC_FrameRect(localFrameRect,mSeparatorLineColor,1.0,true,false,false,false);
	}
	//���T�C�h�o�[�̏ꍇ�A�E���ɕ�������\��
	if( mForLeftSideBarSeparator == true )
	{
		NVMC_FrameRect(localFrameRect,mSeparatorLineColor,1.0,false,false,true,false);
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
�F�ݒ�
@param inColor �w�i�F
@param inColorDeactive �E�C���h�Edeactive���̔w�i�F
*/
void	AView_Plain::SPI_SetColor( const AColor inColor, const AColor inColorDeactive, 
								  const ABool inLeftBottomRounded, const ABool inRightBottomRounded,
								  const ABool inForLeftSideBarSeparator, const ABool inForRightSideBarSeparator, const AColor inSeparatorLineColor )
{
	mColor = inColor;
	mColorDeactive = inColorDeactive;
	//rounded 
	mLeftBottomRounded = inLeftBottomRounded;
	mRightBottomRounded = inRightBottomRounded;
	//�T�C�h�o�[�p�Z�p���[�^
	mForRightSideBarSeparator = inForRightSideBarSeparator;
	mForLeftSideBarSeparator = inForLeftSideBarSeparator;
	mSeparatorLineColor = inSeparatorLineColor;
}

