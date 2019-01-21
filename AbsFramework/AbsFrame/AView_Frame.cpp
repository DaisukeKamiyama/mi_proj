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

AView_Frame

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_Frame
#pragma mark ===========================
/**
�g�\���̂��߂�View
*/

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

@param inWindowID �e�E�C���h�E��ID
@param inID View��ControlID
@param inContentControlID ���ɕ\������View��ControlID
*/
AView_Frame::AView_Frame( const AWindowID inWindowID, const AControlID inID, const AControlID inContentControlID ) 
		: AView(inWindowID,inID), mContentControlID(inContentControlID)
//#1316 ,mBackgroundColor(kColor_White), mFrameColor(kColor_Gray), mFrameColorDeactive(kColor_Gray70Percent)
		,mFocused(false)//#135
//#1316		,mEnableFrameBackgroundColor(false),mFrameBackgroundColor(kColor_White)//win
,mEnableFocusRing(true), mFrameViewType(/*#1316 kFrameViewType_Normal*/kFrameViewType_ToolTip) //#798 #725
//#1316 ,mBackgroundTransparency(1.0)//#688
,mExistText(false)//#1316
{
}

/**
�f�X�g���N�^
*/
AView_Frame::~AView_Frame()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_Frame::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_Frame::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

/**
�}�E�X�{�^����������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
@param inClickCout �N���b�N��
*/
ABool	AView_Frame::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//Frame�̒��̃R���g���[���փ}�E�X�_�E���C�x���g��]������
	ALocalPoint	pt = {-1,-1};
	return NVM_GetWindow().EVT_DoMouseDown(mContentControlID,pt,inModifierKeys,inClickCount);
}

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_Frame::EVTDO_DoDraw()
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	//#135
	ALocalRect	realFrameRect = localFrameRect;
	realFrameRect.left		+= kFocusWidth;
	realFrameRect.right		-= kFocusWidth;
	realFrameRect.top		+= kFocusWidth;
	realFrameRect.bottom	-= kFocusWidth;
	
	//==============�t���[���`��==============
	
	switch(mFrameViewType)
	{
		//tool tip�̏ꍇ
	  case kFrameViewType_ToolTip://#1316 kFrameViewType_Normal:
		{
			AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
			AColor	frameColor = AColorWrapper::GetColorByHTMLFormatColor("EEEEEE");
			if( AApplication::GetApplication().NVI_IsDarkMode() == true )
			{
				backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("282828");
				frameColor = AColorWrapper::GetColorByHTMLFormatColor("404040");
			}
			NVMC_PaintRect(realFrameRect,backgroundColor);
			NVMC_FrameRect(realFrameRect,frameColor);
			break;
		}
		//edit box�̏ꍇ
	  case kFrameViewType_EditBox:
		{
			//�w�i�`��
			AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
			if( AApplication::GetApplication().NVI_IsDarkMode() == true )
			{
				backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
			}
			NVMC_PaintRect(realFrameRect,backgroundColor);
			//�R���g���[����enable�̏ꍇ�̂݃t���[���`��
			if( NVI_GetEnabled() == true )
			{
				if( AApplication::GetApplication().NVI_IsDarkMode() == false )
				{
					NVMC_FrameRect(realFrameRect,AColorWrapper::GetColorByHTMLFormatColor("B1B1B1"));
					ALocalRect	frameRect2 = realFrameRect;
					frameRect2.left += 1;
					frameRect2.right -= 1;
					frameRect2.top += 1;
					frameRect2.bottom -= 1;
					NVMC_FrameRect(frameRect2,AColorWrapper::GetColorByHTMLFormatColor("F0F0F0"));
				}
				else
				{
					NVMC_FrameRect(realFrameRect,AColorWrapper::GetColorByHTMLFormatColor("434343"));
					ALocalRect	frameRect2 = realFrameRect;
					frameRect2.bottom = realFrameRect.top + 1;
					NVMC_FrameRect(frameRect2,AColorWrapper::GetColorByHTMLFormatColor("4A4A4A"));
					ALocalRect	frameRect3 = realFrameRect;
					frameRect3.top = realFrameRect.top + 1;
					frameRect3.bottom = realFrameRect.top + 2;
					NVMC_FrameRect(frameRect3,AColorWrapper::GetColorByHTMLFormatColor("383838"));
					ALocalRect	frameRect4 = realFrameRect;
					frameRect4.top = realFrameRect.bottom - 2;
					frameRect4.bottom = realFrameRect.bottom - 1;
					NVMC_FrameRect(frameRect4,AColorWrapper::GetColorByHTMLFormatColor("404040"));
					ALocalRect	frameRect5 = realFrameRect;
					frameRect5.top = realFrameRect.bottom - 1;
					NVMC_FrameRect(frameRect5,AColorWrapper::GetColorByHTMLFormatColor("666666"));
				}
			}
			break;
		}
		//filter box�̏ꍇ
	  case kFrameViewType_FilterBox:
		{
			//
			AColor	frameColor = AColorWrapper::GetColorByHTMLFormatColor("D0D0D0");
			AColor	fillColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
			if( AApplication::GetApplication().NVI_IsDarkMode() == true )
			{
				frameColor = AColorWrapper::GetColorByHTMLFormatColor("505050");
				fillColor = AColorWrapper::GetColorByHTMLFormatColor("202020");
			}
			ALocalRect	rect = realFrameRect;
			rect.left += 4;
			rect.right -= 4;
			rect.bottom -= 2;
            AFloatNumber	r = 10.0;
			if( mExistText == true )
			{
				NVMC_PaintRoundedRect(rect,fillColor,fillColor,
									  kGradientType_None,1.0,1.0,r,true,true,true,true);
			}
			NVMC_FrameRoundedRect(rect,frameColor,1.0,r,true,true,true,true,true,true,true,true,1.0);
			//���ዾ�A�C�R��
			{
				ALocalPoint	pt = {rect.left + 4, 6};
				NVMC_DrawImage(kImageID_iconSwSearch,pt);
			}
			/*#1316
			//��̐��`��
			{
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.top;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.top;
				NVMC_DrawLine(spt,ept,frameColor);//#1316 AColorWrapper::GetColorByHTMLFormatColor("bbc6d2"));//"4a91d5"));//"bbc6d2"));
			}
			//���̐��`��
			{
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.bottom-1;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.bottom-1;
				NVMC_DrawLine(spt,ept,frameColor);//#1316 AColorWrapper::GetColorByHTMLFormatColor("bbc6d2"));//"4a91d5"));//"bbc6d2"));
			}
			*/
			break;
		}
		//�t���[���`�斳��
	  case kFrameViewType_NoFrameDraw:
		{
			AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
			if( AApplication::GetApplication().NVI_IsDarkMode() == true )
			{
				backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
			}
			NVMC_PaintRect(realFrameRect,backgroundColor);
			break;
		}
	}
	
	//==============�t�H�[�J�X�����O�`��==============
	
#if IMPLEMENTATION_FOR_MACOSX
	if( mFocused == true && mEnableFocusRing == true )//#798
	{
		NVMC_DrawFocusFrame(realFrameRect);
	}
#endif
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

/**
�F�ݒ�
@param inBackgroundColor �w�i�F
@param inFrameColor �g�F
@param inFrameColorDeactive deactivate���̘g�F
*/
/*#1316
void	AView_Frame::SPI_SetColor( const AColor inBackColor, const AColor inFrameColor, const AColor inFrameColorDeactive,
		const AFloatNumber inTransparency )
{
	mBackgroundColor = inBackColor;
	mFrameColor = inFrameColor;
	mFrameColorDeactive = inFrameColorDeactive;
	mBackgroundTransparency = inTransparency;
}
*/

/**
EditBox�̒��g�̃e�L�X�g�����݂��Ă��邩�ǂ�����ݒ�
*/
void	AView_Frame::SPI_SetExistText( const ABool inExist )
{
	mExistText = inExist;
}

//win
/**
�t���[�������w�i�F�ݒ�
*/
/*#1316
void	AView_Frame::SPI_SetFrameBackgroundColor( const ABool inEnableFrameBackgroundColor, const AColor inColor )
{
	mEnableFrameBackgroundColor = inEnableFrameBackgroundColor;
	mFrameBackgroundColor = inColor;
}
*/

//#135
/**
*/
void	AView_Frame::SPI_SetFocused( const ABool inFocused )
{
	mFocused = inFocused;
	NVI_Refresh();
}

//#135
/**
���ۂ̕`��Fram��Bound���擾

AView_Framw��Focus�`�敪����Bounds���傫���Ȃ邽��
*/
void	AView_Frame::SPI_GetRealFrameBounds( AWindowRect& outRect ) const
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	NVM_GetWindowConst().NVI_GetWindowRectFromControlLocalRect(NVI_GetControlID(),localFrameRect,outRect);
	outRect.left	+= kFocusWidth;
	outRect.right	-= kFocusWidth;
	outRect.top		+= kFocusWidth;
	outRect.bottom	-= kFocusWidth;
}

//#135
/**
���ۂ̕`��Frame�̃T�C�Y�E�ʒu��ݒ�
*/
void	AView_Frame::SPI_SetRealFramePositionAndSize( const AWindowPoint& inRealFramePoint, const ANumber inRealFrameWidth, const ANumber inRealFrameHeight )
{
	AWindowPoint	framePoint = inRealFramePoint;
	framePoint.x -= kFocusWidth;
	framePoint.y -= kFocusWidth;
	ANumber	frameWidth = inRealFrameWidth + kFocusWidth*2;
	ANumber	frameHeight = inRealFrameHeight + kFocusWidth*2;
	NVI_SetPosition(framePoint);
	NVI_SetSize(frameWidth,frameHeight);
}



