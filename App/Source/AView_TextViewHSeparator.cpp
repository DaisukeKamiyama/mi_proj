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

AView_TextViewHSeparator

*/

#include "stdafx.h"

#include "AView_TextViewHSeparator.h"
#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AView_Text.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_TextViewHSeparator
#pragma mark ===========================
//�������r���[

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AView_TextViewHSeparator::AView_TextViewHSeparator( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mMouseTrackingMode(kMouseTrackingMode_None), mHover(false)
		,mLinePosition(kSeparatorLinePosition_Middle),mTargetWindowID(inWindowID)
{
	NVI_SetDefaultCursor(kCursorType_ResizeUpDown);
	NVMC_EnableMouseLeaveEvent();
}

/**
�f�X�g���N�^
*/
AView_TextViewHSeparator::~AView_TextViewHSeparator()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_TextViewHSeparator::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_TextViewHSeparator::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

const ANumber kViewDeleteHeight = 16;

#pragma mark ===========================

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)win 080709 drag loop��������
ABool	AView_TextViewHSeparator::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( inClickCount == 2 )
	{
		NVM_GetWindow().NVI_SeparatorDoubleClicked(mTargetWindowID,NVI_GetControlID());
		return true;
	}
	
	//�O��}�E�X�ʒu�̕ۑ�
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,mPreviousMouseGlobalPoint);
	
	//Windows�̏ꍇ�́A�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	NVM_SetMouseTrackingMode(true);
	return false;
}

void	AView_TextViewHSeparator::DoMouseTrack( const ALocalPoint& inLocalPoint )
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
	ANumber	resultDelta = NVM_GetWindow().NVI_SeparatorMoved(mTargetWindowID,NVI_GetControlID(),widthDelta,false);//#409
	mouseGlobalPoint.y = mPreviousMouseGlobalPoint.y + resultDelta;
	//�O��}�E�X�ʒu�̕ۑ�
	mPreviousMouseGlobalPoint = mouseGlobalPoint;
}

/**
�}�E�X�ړ��C�x���g
*/
ABool	AView_TextViewHSeparator::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
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
void	AView_TextViewHSeparator::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	mHover = false;
	NVI_Refresh();
}

//Mouse Tracking
void	AView_TextViewHSeparator::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	DoMouseTrack(inLocalPoint);
}

//Mouse Tracking�I���i�}�E�X�{�^��Up�j
void	AView_TextViewHSeparator::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�����ʒm #409
	NVM_GetWindow().NVI_SeparatorMoved(mTargetWindowID,NVI_GetControlID(),0,true);
	//TrackingMode�����iDoMouseTrackEnd()�̑O�ɂ��Ȃ��ƁADoMouseTrackEnd()��Separator�R���g���[�����폜����Ă��܂��̂Łj
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}

/**
�`��v�����̃R�[���o�b�N(AView�p)
*/
void	AView_TextViewHSeparator::EVTDO_DoDraw()
{
	AWindowID	winID = NVM_GetWindow().GetObjectID();
	AModeIndex	modeIndex = GetApp().SPI_GetTextWindowByID(winID).SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
	//
	AColor	backgroundColor = kColor_White;
	GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kBackgroundColor,backgroundColor);
	//�w�i�h��Ԃ�
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	NVMC_PaintRect(viewRect,backgroundColor);
	
	//
	if( mLinePosition == kSeparatorLinePosition_None )   return;
	
	//�������`��
	ALocalPoint	spt,ept;
	spt.x = viewRect.left;
	spt.y = viewRect.top+GetLineOffset();
	ept.x = viewRect.right;
	ept.y = viewRect.top+GetLineOffset();
	
	//gradient paint
	ALocalRect	rect = viewRect;
	rect.bottom = spt.y;
	//NVMC_PaintRectWithVerticalGradient(rect,kColor_White,kColor_Gray85Percent,0.8,0.8);
	NVMC_PaintRectWithVerticalGradient(rect,kColor_Gray70Percent,kColor_Gray70Percent,0.0,0.5);
	//���`��
	NVMC_DrawLine(spt,ept,kColor_Gray70Percent,0.8,0.0,1.0);
}

#pragma mark ===========================

#pragma mark --- ���̈ʒu�ݒ�

/**
���̈ʒu�ݒ�
*/
void	AView_TextViewHSeparator::SPI_SetLinePosition( const ASeparatorLinePosition inPosition )
{
	mLinePosition = inPosition;
	NVI_Refresh();
}

/**
���̈ʒu�擾
*/
ANumber	AView_TextViewHSeparator::GetLineOffset() const
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



