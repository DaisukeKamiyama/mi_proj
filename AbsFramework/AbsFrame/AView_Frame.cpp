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
		: AView(inWindowID,inID), mContentControlID(inContentControlID), 
		mBackgroundColor(kColor_White), mFrameColor(kColor_Gray), mFrameColorDeactive(kColor_Gray70Percent)
		,mFocused(false)//#135
		,mEnableFrameBackgroundColor(false),mFrameBackgroundColor(kColor_White)//win
,mEnableFocusRing(true), mFrameViewType(kFrameViewType_Normal) //#798 #725
,mBackgroundTransparency(1.0)//#688
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
	
	//==============�w�i�`��==============
	
	//�S�̂�w�i�F���� win
	if( mEnableFrameBackgroundColor == false )
	{
		NVMC_EraseRect(localFrameRect);
	}
	else
	{
		NVMC_PaintRect(localFrameRect,mFrameBackgroundColor);
	}
	//
	AColor	framecolor = mFrameColor;
	if( NVM_GetWindow().NVI_IsWindowActive() == false )
	{
		framecolor = mFrameColorDeactive;
	}
	//Disable����Deactive�F�ɂ���
	if( NVI_GetEnabled() == false )
	{
		framecolor = mFrameColorDeactive;
	}
#if IMPLEMENTATION_FOR_WINDOWS
	//win
	if( mFocused == true && mEnableFocusRing == true )//#798
	{
		AColorWrapper::GetHighlightColor(framecolor);
	}
#endif
	//#135
	ALocalRect	realFrameRect = localFrameRect;
	realFrameRect.left		+= kFocusWidth;
	realFrameRect.right		-= kFocusWidth;
	realFrameRect.top		+= kFocusWidth;
	realFrameRect.bottom	-= kFocusWidth;
	//�w�i�`��
	NVMC_PaintRect(realFrameRect,mBackgroundColor,mBackgroundTransparency);
	
	//==============�t���[���`��==============
	
	switch(mFrameViewType)
	{
	  case kFrameViewType_Normal:
		{
			NVMC_FrameRect(realFrameRect,framecolor);
			break;
		}
		//edit box�̏ꍇ
	  case kFrameViewType_EditBox:
		{
			//�R���g���[����enable�̏ꍇ�̂݃t���[���`��
			if( NVI_GetEnabled() == true )
			{
				//Mac OS X 10.7�ɋ߂��t���[���\��
				//�t���[��
				NVMC_FrameRect(realFrameRect,AColorWrapper::GetColorByHTMLFormatColor("9e9e9e"));
				ALocalPoint	spt = {0}, ept = {0};
				//���V���h�E
				spt.x = realFrameRect.left+1;
				spt.y = realFrameRect.top +1;
				ept.x = realFrameRect.left+1;
				ept.y = realFrameRect.bottom-2;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("f7f7f7"));
				//�E�V���h�E
				spt.x = realFrameRect.right-2;
				spt.y = realFrameRect.top+1;
				ept.x = realFrameRect.right-2;
				ept.y = realFrameRect.bottom-2;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("f7f7f7"));
				//���V���h�E
				spt.x = realFrameRect.left+1;
				spt.y = realFrameRect.bottom-2;
				ept.x = realFrameRect.right-3;
				ept.y = realFrameRect.bottom-2;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("fdfdfd"));
				//��V���h�E
				ALocalRect	r = realFrameRect;
				r.left		+= 1;
				r.right		-= 1;
				r.top		+= 1;
				r.bottom	= r.top+2;
				NVMC_PaintRectWithVerticalGradient(r,AColorWrapper::GetColorByHTMLFormatColor("e0e0e0"),
													 AColorWrapper::GetColorByHTMLFormatColor("f8f8f8"),1.0,1.0);
				/*
				ALocalPoint	spt = {0}, ept = {0};
				//���
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.top;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.top;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("c1c1c1"));
				//����
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.top+1;
				ept.x = realFrameRect.left;
				ept.y = realFrameRect.bottom-1;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("c1c1c1"));
				//�E��
				spt.x = realFrameRect.right-1;
				spt.y = realFrameRect.top +1;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.bottom-1;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("c1c1c1"));
				//����
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.bottom-1;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.bottom-1;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("c1c1c1"));
				//��V���h�E
				spt.x = realFrameRect.left+1;
				spt.y = realFrameRect.top+1;
				ept.x = realFrameRect.right-2;
				ept.y = realFrameRect.top+1;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("d4d4d4"),0.8);
				*/
				
			}
			break;
		}
		//filter box�̏ꍇ
	  case kFrameViewType_FilterBox:
		{
			//���ዾ�A�C�R��
			{
				ALocalPoint	pt = {realFrameRect.left + 3, 6};
				NVMC_DrawImage(kImageID_iconSwSearch,pt);
			}
			//��̐��`��
			{
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.top;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.top;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("bbc6d2"));//"4a91d5"));//"bbc6d2"));
			}
			//���̐��`��
			{
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.bottom-1;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.bottom-1;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("bbc6d2"));//"4a91d5"));//"bbc6d2"));
			}
			break;
		}
		//�t���[���`�斳��
	  case kFrameViewType_NoFrameDraw:
		{
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
void	AView_Frame::SPI_SetColor( const AColor inBackColor, const AColor inFrameColor, const AColor inFrameColorDeactive,
		const AFloatNumber inTransparency )
{
	//
	mBackgroundColor = inBackColor;
	mFrameColor = inFrameColor;
	mFrameColorDeactive = inFrameColorDeactive;
	mBackgroundTransparency = inTransparency;
}

//win
/**
�t���[�������w�i�F�ݒ�
*/
void	AView_Frame::SPI_SetFrameBackgroundColor( const ABool inEnableFrameBackgroundColor, const AColor inColor )
{
	mEnableFrameBackgroundColor = inEnableFrameBackgroundColor;
	mFrameBackgroundColor = inColor;
}

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



