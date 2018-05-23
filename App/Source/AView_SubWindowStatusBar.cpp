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

AView_SubWindowStatusBar

*/

#include "stdafx.h"

#include "AView_SubWindowStatusBar.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_SubWindowStatusBar
#pragma mark ===========================

//==����A�⊮���X�g�݂̂Ŏg�p

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

@param inWindowID �e�E�C���h�E��ID
@param inID View��ControlID
*/
AView_SubWindowStatusBar::AView_SubWindowStatusBar( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mFontSize(9.0)
,mTextColor(kColor_Black), mTextStyle(kTextStyle_Normal), mTextJustification(kJustification_Left)
{
}

/**
�f�X�g���N�^
*/
AView_SubWindowStatusBar::~AView_SubWindowStatusBar()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_SubWindowStatusBar::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_SubWindowStatusBar::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_SubWindowStatusBar::EVTDO_DoDraw()
{
	//�w�i�`��
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	
	//==================�⊮��⃊�X�g�p==================
	
	//�T�u�E�C���h�E�f�[�^�擾
	ASubWindowLocationType	locationType = GetApp().SPI_GetSubWindowLocationType(NVM_GetWindowConst().GetObjectID());
	AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
	
	//==================�F�擾==================
	
	//�`��F�ݒ�
	AColor	letterColor = kColor_Black;
	AColor	dropShadowColor = kColor_White;
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
	NVMC_SetDropShadowColor(dropShadowColor);
	
	//==================
	
	//�t���[���̌`��擾
	ABool	roundedLeftBottom = false;
	ABool	roundedRightBottom = false;
	AFloatNumber	roundR = 5.0;
	switch(locationType)
	{
	  case kSubWindowLocationType_Floating:
		{
			roundedLeftBottom = true;
			roundedRightBottom = false;
			roundR = 5.0;
			break;
		}
	  case kSubWindowLocationType_Popup:
		{
			roundedLeftBottom = true;
			roundedRightBottom = true;
			roundR = kRadius_PopupCandidateList;
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	
	//1 pixel inset����
	ALocalRect	rect = localFrameRect;
	rect.left		+= 1;
	rect.right		-= 1;
	rect.bottom		-= 1;
	
	//�X�e�[�^�X�o�[
	NVMC_PaintRoundedRect(rect,
						  //kColor_White,kColor_Gray95Percent
						  boxBaseColor2,boxBaseColor3,
						  kGradientType_Vertical,
						  mBackgroundTransparency,mBackgroundTransparency,	
						  roundR,false,false,roundedLeftBottom,roundedRightBottom);
	if( locationType == kSubWindowLocationType_Popup )
	{
		//�|�b�v�A�b�v�̏ꍇ�̂݃t���[���`�悷��
		NVMC_FrameRoundedRect(rect,
							  //kColor_Gray20Percent
							  //kColor_Gray50Percent,
							  letterColor,
							  0.5,roundR,false,false,roundedLeftBottom,roundedRightBottom,
							  true,true,true,true,kLineWidth_PopupCandidateList);
	}
	
	//�e�L�X�g�`��
	ALocalRect	drawRect = localFrameRect;
	drawRect.top	+= 3;
	drawRect.left	+= 15;
	drawRect.bottom	-= 1;
	drawRect.right	-= 80+3;
	if( drawRect.right > drawRect.left )
	{
		NVMC_SetDefaultTextProperty(mFontName,mFontSize,/*kColor_Gray20Percent*/letterColor,mTextStyle,true);//GetApp().SPI_GetSubWindowHeaderLetterColor()
		AText	text;
		NVI_GetEllipsisTextWithFixedLastCharacters(mText,drawRect.right-drawRect.left,5,text);
		NVMC_DrawText(drawRect,text,mTextJustification);
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
�t�H���g�ݒ�
*/
void	AView_SubWindowStatusBar::NVIDO_SetTextFont( const AText& inFontName, const AFloatNumber inFontSize )
{
	mFontName = inFontName;
	mFontSize = inFontSize;
}

/**
�e�L�X�g�ݒ�
*/
void	AView_SubWindowStatusBar::NVIDO_SetText( const AText& inText )
{
	mText.SetText(inText);
	NVI_Refresh();
}

/**
�e�L�X�g�v���p�e�B�ݒ�
*/
void	AView_SubWindowStatusBar::NVIDO_SetTextProperty( const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification )
{
	mTextColor = inColor;
	mTextStyle = inStyle;
	mTextJustification = inJustification;
	NVI_Refresh();
}



