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

AView_TextPanel

*/

#include "stdafx.h"

#include "AView_TextPanel.h"
#include "../Imp.h"
#include "../Wrapper.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_TextPanel
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

@param inWindowID �e�E�C���h�E��ID
@param inID View��ControlID
*/
AView_TextPanel::AView_TextPanel( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mFontSize(9.0)
,mTextColor(kColor_Black), mTextStyle(kTextStyle_Normal), mTextJustification(kJustification_Left)
{
}

/**
�f�X�g���N�^
*/
AView_TextPanel::~AView_TextPanel()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_TextPanel::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_TextPanel::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_TextPanel::EVTDO_DoDraw()
{
	//�w�i�`��
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	
	//���F�X�e�[�^�X�o�[
	NVMC_PaintRoundedRect(localFrameRect,
						  kColor_White,kColor_Gray95Percent,kGradientType_Vertical,mBackgroundTransparency,mBackgroundTransparency,
						  5.0,false,false,true,false);
	NVMC_FrameRoundedRect(localFrameRect,kColor_Gray20Percent,1.0,5.0,false,false,true,false);
	
	//��؂��
	ALocalPoint	spt = {localFrameRect.left,localFrameRect.top};
	ALocalPoint	ept = {localFrameRect.right,localFrameRect.top};
	NVMC_DrawLine(spt,ept,kColor_Gray80Percent);
	
	//�e�L�X�g�`��
	ALocalRect	drawRect = localFrameRect;
	drawRect.top	+= 2;
	drawRect.left	+= 10;
	drawRect.bottom	-= 2;
	drawRect.right	-= 3;
	NVMC_SetDefaultTextProperty(mFontName,mFontSize,mTextColor,mTextStyle,true);
	NVMC_DrawText(drawRect,mText,mTextJustification);
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
�t�H���g�ݒ�
*/
void	AView_TextPanel::NVIDO_SetTextFont( const AText& inFontName, const AFloatNumber inFontSize )
{
	mFontName = inFontName;
	mFontSize = inFontSize;
}

/**
�e�L�X�g�ݒ�
*/
void	AView_TextPanel::NVIDO_SetText( const AText& inText )
{
	mText.SetText(inText);
	NVI_Refresh();
}

/**
�e�L�X�g�v���p�e�B�ݒ�
*/
void	AView_TextPanel::NVIDO_SetTextProperty( const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification )
{
	mTextColor = inColor;
	mTextStyle = inStyle;
	mTextJustification = inJustification;
	NVI_Refresh();
}



