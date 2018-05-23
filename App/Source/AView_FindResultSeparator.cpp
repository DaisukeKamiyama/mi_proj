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

AView_FindResultSeparator
#92

*/

#include "stdafx.h"

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_FindResultSeparator.h"
#include "AWindow_Text.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_FindResultSeparator
#pragma mark ===========================
//�������r���[

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_FindResultSeparator::AView_FindResultSeparator( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mMouseTrackingMode(kMouseTrackingMode_None), /*#688 mMouseTrackByLoop(true),*/ mHover(false)
{
	//#92 ��������NVIDO_Init()�ֈړ�
}

//�f�X�g���N�^
AView_FindResultSeparator::~AView_FindResultSeparator()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_FindResultSeparator::NVIDO_Init()
{
	//win
	NVI_SetDefaultCursor(kCursorType_ResizeUpDown);
	/*#688
	//win 080709
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
	//B0000 080810
	NVMC_EnableMouseLeaveEvent();
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_FindResultSeparator::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//
AWindow_Text&	AView_FindResultSeparator::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,NVM_GetWindowID());
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

const ANumber kViewDeleteHeight = 16;

#pragma mark ===========================

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)win 080709 drag loop��������
ABool	AView_FindResultSeparator::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	
	//ViewRect�擾
	ALocalRect	viewLocalRect;
	NVM_GetLocalViewRect(viewLocalRect);
	
	//�ŏ���Drag���C���\��
	mDragLineStart = inLocalPoint;
	mDragLineStart.x = viewLocalRect.left;
	mDragLineEnd = mDragLineStart;
	mDragLineEnd.x = viewLocalRect.right;
	NVMC_DrawXorCaretLine(mDragLineStart,mDragLineEnd,false);
	//
	//�O��}�E�X�ʒu�̕ۑ�
	mPreviousMouseLocalPoint = inLocalPoint;
	
	//�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
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
	DoMouseTrackEnd();
	return false;
	*/
}

void	AView_FindResultSeparator::DoMouseTrack( const ALocalPoint& inLocalPoint )
{
	//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
	if( inLocalPoint.x == mPreviousMouseLocalPoint.x && inLocalPoint.y == mPreviousMouseLocalPoint.y )
	{
		return;
	}
	//
	ANumber	separateLineOffset = NVI_GetViewHeight()/2;
	ANumber	separateLineLocalY = mDragLineStart.y;
	//ViewRect�擾
	ALocalRect	viewLocalRect;
	NVM_GetLocalViewRect(viewLocalRect);
	
	//�O��}�E�X�ʒu�̕ۑ�
	mPreviousMouseLocalPoint = inLocalPoint;
	
	//
	ANumber	textviewheight = GetTextWindow().SPI_GetTextViewAreaHeight();
	ANumber	findresultheight = GetTextWindow().SPI_GetFindResultHeight();
	//����A�����ŋz��
	separateLineLocalY = inLocalPoint.y + separateLineOffset;
	if( separateLineLocalY < -textviewheight + kViewDeleteHeight + separateLineOffset )
	{
		separateLineLocalY = -textviewheight + separateLineOffset;
	}
	if( separateLineLocalY > findresultheight - kViewDeleteHeight + separateLineOffset )
	{
		separateLineLocalY = findresultheight + separateLineOffset;
	}
	//Drag���C���\���X�V
	NVMC_DrawXorCaretLine(mDragLineStart,mDragLineEnd,false);
	mDragLineStart.x = viewLocalRect.left;
	mDragLineStart.y = separateLineLocalY;
	mDragLineEnd.x = viewLocalRect.right;
	mDragLineEnd.y = separateLineLocalY;
	NVMC_DrawXorCaretLine(mDragLineStart,mDragLineEnd,false);
}

void	AView_FindResultSeparator::DoMouseTrackEnd()
{
	//�Ō��Drag���C���̍폜
	NVMC_DrawXorCaretLine(mDragLineStart,mDragLineEnd,false);
	
	//
	ANumber	separateLineOffset = NVI_GetViewHeight()/2;
	ANumber	separateLineLocalY = mDragLineStart.y;
	//
	ANumber	heightDelta = separateLineLocalY-separateLineOffset;
	
	//
	GetTextWindow().SPI_SetFindResultHeight(GetTextWindow().SPI_GetFindResultHeight()-heightDelta);
}

//B0000 080810
//�J�[�\��
ABool	AView_FindResultSeparator::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
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

//B0000 080810
void	AView_FindResultSeparator::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	mHover = false;
	NVI_Refresh();
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_FindResultSeparator::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	DoMouseTrack(inLocalPoint);
}

//Mouse Tracking�I���i�}�E�X�{�^��Up�j
void	AView_FindResultSeparator::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//TrackingMode�����iDoMouseTrackEnd()�̑O�ɂ��Ȃ��ƁADoMouseTrackEnd()��Separator�R���g���[�����폜����Ă��܂��̂Łj
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
	//
	DoMouseTrackEnd();
}
//#688 #endif

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_FindResultSeparator::EVTDO_DoDraw()
{
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	NVMC_PaintRect(viewRect,kColor_White);
	
	ALocalPoint	spt,ept;
	spt.x = viewRect.left;
	spt.y = viewRect.top+NVI_GetViewHeight()/2;//win 080618
	ept.x = viewRect.right;
	ept.y = viewRect.top+NVI_GetViewHeight()/2;//win 080618
	//B0000 080810
	if( mHover == true )
	{
		AColor	color;
		AColorWrapper::GetHighlightColor(color);
		NVMC_DrawLine(spt,ept,color,1.0,0.0,2.0);
	}
	else
	{
		NVMC_DrawLine(spt,ept,kColor_Gray70Percent,1.0,0.0,1.0);
	}
}

