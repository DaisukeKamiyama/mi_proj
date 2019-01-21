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

AView_CallGrafHeader
#723

*/

#include "stdafx.h"

#include "AView_CallGrafHeader.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_CallGrafHeader
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_CallGrafHeader::AView_CallGrafHeader( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mStartLevel(1)
{
	NVMC_SetOffscreenMode(true);
}

//�f�X�g���N�^
AView_CallGrafHeader::~AView_CallGrafHeader()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_CallGrafHeader::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_CallGrafHeader::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�`��v�����̃R�[���o�b�N(AView�p)
*/
void	AView_CallGrafHeader::EVTDO_DoDraw()
{
	//�t�H���g�ݒ�
	AText	labelfontname;
	AFontWrapper::GetDialogDefaultFontName(labelfontname);
	NVMC_SetDefaultTextProperty(labelfontname,9,kColor_Black,kTextStyle_Normal,true);
	
	//�w�i�F�`��
	AColor	backgroundColor = GetApp().SPI_GetSubWindowHeaderBackgroundColor();
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	NVMC_PaintRect(frameRect,backgroundColor);
	
	//�����F�擾
	AColor	letterColor = kColor_Black;//#1316 ���@�\��������Ȃ�_�[�N���[�h�Ή��K�vGetApp().SPI_GetSubWindowHeaderLetterColor();
	
	//�J�����̕����v�Z
	ANumber	columnWidth = (frameRect.right-frameRect.left)/2;
	
	//�e�J�������̃��[�v
	for( AIndex column = 0; column <= 1; column++ )
	{
		//���x���擾
		AIndex	level = mStartLevel - column;
		
		//�`��
		const ANumber	kMarginWidth = 2;
		const ANumber	kMarginHeight = 1;
		
		//1�s�ځi���x���\���j
		{
			AText	text;
			text.SetLocalizedText("CallGrafLevel");
			text.ReplaceParamText('0',-level);
			ALocalRect	rect = frameRect;
			rect.top		= frameRect.top + kMarginHeight + 1;
			rect.bottom		= rect.top + kLineHeight;
			rect.left 		= column*columnWidth + kMarginWidth;
			rect.right 		= (column+1)*columnWidth - kMarginWidth;
			NVMC_DrawText(rect,text,letterColor,kTextStyle_Bold,kJustification_Center);
		}
		
		//2�s�ځi�e��^�C�g���j
		{
			//�e�L�X�g�擾
			AText	text;
			if( level == 0 )
			{
				//���x��0�p�e�L�X�g
				text.SetLocalizedText("CallGrafLevel0");
			}
			else if( level-1 < mCurrentWordArray.GetItemCount() )
			{
				//const ANumber	kMaxWordDisplayLength = 16;
				//�e���x���p�e�L�X�g
				text.SetLocalizedText("CallGrafLevelX");
				AText	t;
				mCurrentWordArray.Get(level-1,t);
				text.ReplaceParamText('0',t);
			}
			
			//�`��
			ALocalRect	rect = frameRect;
			rect.top		= frameRect.top + kMarginHeight + kLineHeight;
			rect.bottom		= rect.top + kLineHeight;
			rect.left 		= column*columnWidth + kMarginWidth;
			rect.right 		= (column+1)*columnWidth - kMarginWidth;
			AText	ellipsisText;
			NVI_GetEllipsisTextWithFixedFirstCharacters(text,rect.right-rect.left-8,8,ellipsisText);
			NVMC_DrawText(rect,ellipsisText,letterColor,kTextStyle_Bold,kJustification_Center);
		}
	}
	
	//�����x���{�^���`��
	{
		ALocalPoint	pt = {0};
		pt.x = frameRect.left + 2;
		pt.y = frameRect.top + 2;
		NVMC_DrawImage(kImageID_btnNaviLeft,pt);
	}
	//�E���x���{�^���`��
	if( mStartLevel >= 2 )
	{
		ALocalPoint	pt = {0};
		pt.x = frameRect.right -18;
		pt.y = frameRect.top + 2;
		NVMC_DrawImage(kImageID_btnNaviRight,pt);
	}
}

/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_CallGrafHeader::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//�����x���ړ�
	if( inLocalPoint.x < frameRect.left + 18 )
	{
		NVM_GetWindow().EVT_Clicked(AWindow_CallGraf::kControlID_LeftLevelButton,0);
	}
	//�E���x���ړ�
	else if( inLocalPoint.x > frameRect.right - 18 )
	{
		NVM_GetWindow().EVT_Clicked(AWindow_CallGraf::kControlID_RightLevelButton,0);
	}
	
	return false;
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
���݂̃f�[�^�ݒ�
*/
void	AView_CallGrafHeader::SPI_SetCurrentStatus( const AIndex inStartLevel, const ATextArray& inCurrentWordArray )
{
	mStartLevel = inStartLevel;
	mCurrentWordArray.SetFromTextArray(inCurrentWordArray);
	NVI_Refresh();
}


