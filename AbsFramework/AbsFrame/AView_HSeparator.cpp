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

AView_HSeparator

*/

#include "stdafx.h"

#include "AView_HSeparator.h"
//#include "AWindow_Text.h"
#include "../Imp.h"
#include "../Wrapper.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_HSeparator
#pragma mark ===========================
//�������r���[

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AView_HSeparator::AView_HSeparator( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mMouseTrackingMode(kMouseTrackingMode_None), /*#688 mMouseTrackByLoop(true),*/ mHover(false)
		,mLinePosition(kSeparatorLinePosition_Middle)
,mEventTargetWindowID(inWindowID), mEnableBackgroundColor(false), mBackgroundColor(kColor_White)//#864, mTransparency(0.5)
{
	NVI_SetDefaultCursor(kCursorType_ResizeUpDown);
	/*#688
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
	NVMC_EnableMouseLeaveEvent();
}

/**
�f�X�g���N�^
*/
AView_HSeparator::~AView_HSeparator()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_HSeparator::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_HSeparator::NVIDO_DoDeleted()
{
}

/**
�^�[�Q�b�g�E�C���h�E�ݒ�
*/
void	AView_HSeparator::SPI_SetTargetWindowID( const AWindowID inTargetWindowID )
{
	mEventTargetWindowID = inTargetWindowID;
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

const ANumber kViewDeleteHeight = 16;

#pragma mark ===========================

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)win 080709 drag loop��������
ABool	AView_HSeparator::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( inClickCount == 2 )
	{
		NVM_GetWindow().NVI_SeparatorDoubleClicked(mEventTargetWindowID,NVI_GetControlID());
		return true;
	}
	
	//�O��}�E�X�ʒu�̕ۑ�
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,mPreviousMouseGlobalPoint);
	
	//Windows�̏ꍇ�́A�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	NVM_SetMouseTrackingMode(true);
	return false;
	//#688 }
	/*#688
	//�}�E�X�{�^�����������܂Ń��[�v
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//���[�J�����W�ɕϊ�
		ALocalPoint	mouseLocalPoint;
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		//
		DoMouseTrack(mouseLocalPoint);
	}
	//�����ʒm #409
	NVM_GetWindow().NVI_SeparatorMoved(mEventTargetWindowID,NVI_GetControlID(),0,true);
	return false;
	*/
}

void	AView_HSeparator::DoMouseTrack( const ALocalPoint& inLocalPoint )
{
	//�O���[�o�����W�ɕϊ�
	AGlobalPoint	mouseGlobalPoint = {0};
	AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
	//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
	if( mouseGlobalPoint.x == mPreviousMouseGlobalPoint.x && mouseGlobalPoint.y == mPreviousMouseGlobalPoint.y )
	{
		return;
	}
	//TargetWindow��Separator�ړ���ʒm
	ANumber	widthDelta = mouseGlobalPoint.y-mPreviousMouseGlobalPoint.y;
	ANumber	resultDelta = NVM_GetWindow().NVI_SeparatorMoved(mEventTargetWindowID,NVI_GetControlID(),widthDelta,false);//#409
	mouseGlobalPoint.y = mPreviousMouseGlobalPoint.y + resultDelta;
	//�O��}�E�X�ʒu�̕ۑ�
	mPreviousMouseGlobalPoint = mouseGlobalPoint;
}

/**
�}�E�X�ړ��C�x���g
*/
ABool	AView_HSeparator::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( inLocalPoint.y >= 0 && inLocalPoint.y <= NVI_GetViewHeight() )
	{
		if( mHover == false )
		{
			mHover = true;
			NVI_Refresh();
		}
	}
	else
	{
		if( mHover == true )
		{
			mHover = false;
			NVI_Refresh();
		}
	}
	return true;
}

/**
�}�E�X�̈�O�ړ��C�x���g
*/
void	AView_HSeparator::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	mHover = false;
	NVI_Refresh();
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_HSeparator::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	DoMouseTrack(inLocalPoint);
}

//Mouse Tracking�I���i�}�E�X�{�^��Up�j
void	AView_HSeparator::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�����ʒm #409
	NVM_GetWindow().NVI_SeparatorMoved(mEventTargetWindowID,NVI_GetControlID(),0,true);
	//TrackingMode�����iDoMouseTrackEnd()�̑O�ɂ��Ȃ��ƁADoMouseTrackEnd()��Separator�R���g���[�����폜����Ă��܂��̂Łj
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}
//#688 #endif

/**
�`��v�����̃R�[���o�b�N(AView�p)
*/
void	AView_HSeparator::EVTDO_DoDraw()
{
	//�w�i�h��Ԃ�
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	if( mEnableBackgroundColor == true )//#864
	{
		NVMC_PaintRect(viewRect,mBackgroundColor);
	}
	else
	{
		//NVMC_EraseRect(viewRect);
		//EraseRect�ɂ���ƁA�`�悵�Ȃ��������}�E�X�C�x���g���߂���Ă��܂��̂ŁA���ɔ����F�ŕ`�悷��B
		NVMC_PaintRect(viewRect,kColor_White,0.05);
	}
	
	//
	if( mLinePosition == kSeparatorLinePosition_None )   return;
	
	//�������`��
	ALocalPoint	spt,ept;
	spt.x = viewRect.left;
	spt.y = viewRect.top+GetLineOffset();
	ept.x = viewRect.right;
	ept.y = viewRect.top+GetLineOffset();
	/*#725
	if( mHover == true )
	{
		AColor	color;
		AColorWrapper::GetHighlightColor(color);
		NVMC_DrawLine(spt,ept,color,1.0,0.0,2.0);
	}
	else
	*/
	{
		/*
		ALocalRect	rect = viewRect;
		rect.bottom = spt.y;
		NVMC_PaintRectWithVerticalGradient(rect,kColor_Gray95Percent,kColor_Gray70Percent,0.0,0.5);
		*/
		NVMC_DrawLine(spt,ept,kColor_Gray70Percent,0.8,0.0,1.0);
	}
}

#pragma mark ===========================

#pragma mark --- ���̈ʒu�ݒ�

/**
���̈ʒu�ݒ�
*/
void	AView_HSeparator::SPI_SetLinePosition( const ASeparatorLinePosition inPosition )
{
	mLinePosition = inPosition;
	NVI_Refresh();
}

/**
���̈ʒu�擾
*/
ANumber	AView_HSeparator::GetLineOffset() const
{
	ANumber	result = 0;
	switch(mLinePosition)
	{
	  case kSeparatorLinePosition_Top:
		{
			result = 0;
			break;
		}
	  case kSeparatorLinePosition_Bottom:
		{
			result = NVI_GetViewHeight()-1;
			break;
		}
	  case kSeparatorLinePosition_Middle:
	  default:
		{
			result = NVI_GetViewHeight()/2;
			break;
		}
	}
	return result;
}



