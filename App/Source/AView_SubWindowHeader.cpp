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

AView_SubWindowHeader
#723

*/

#include "stdafx.h"

#include "AView_SubWindowHeader.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_SubWindowHeader
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_SubWindowHeader::AView_SubWindowHeader( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID)
{
	NVMC_SetOffscreenMode(true);
}

//�f�X�g���N�^
AView_SubWindowHeader::~AView_SubWindowHeader()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_SubWindowHeader::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_SubWindowHeader::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�`��v�����̃R�[���o�b�N(AView�p)
*/
void	AView_SubWindowHeader::EVTDO_DoDraw()
{
	//=========================�`��f�[�^�擾=========================
	//�t�H���g�擾
	//���e�����t�H���g
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//�w�b�_�����t�H���g
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	
	//�t�H���g�ݒ�
	NVMC_SetDefaultTextProperty(headerfontname,9.0,kColor_Black,kTextStyle_Normal,true);
	
	//�w�i�F�`��
	AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("EEEEEE");//#1316 GetApp().SPI_GetSubWindowHeaderBackgroundColor();
	if( GetApp().NVI_IsDarkMode() == true )
	{
		backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
	}

	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	NVMC_PaintRect(frameRect,backgroundColor);
	
	//text�`��rect�擾
	ALocalRect	textRect = frameRect;
	textRect.top += 1;
	textRect.bottom -= 1;
	textRect.left += 8;
	textRect.right -= 3;
	
	//�����F�擾
	AColor	letterColor = AColorWrapper::GetColorByHTMLFormatColor("303030");//#1316 GetApp().SPI_GetSubWindowHeaderLetterColor();
	if( GetApp().NVI_IsDarkMode() == true )
	{
		letterColor = AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
	}
	
	//�e�L�X�g�擾
	AText	ellipsisTitle;
	NVI_GetEllipsisTextWithFixedFirstCharacters(mText,textRect.right-textRect.left-16,5,ellipsisTitle);
	
	//�e�L�X�g�`��
	NVMC_DrawText(textRect,ellipsisTitle,letterColor,kTextStyle_Bold,kJustification_Left);
	
	
}

/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_SubWindowHeader::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	return false;
}

/**
�e�L�X�g�ݒ�
*/
void	AView_SubWindowHeader::NVIDO_SetText( const AText& inText )
{
	mText.SetText(inText);
	NVI_Refresh();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================


