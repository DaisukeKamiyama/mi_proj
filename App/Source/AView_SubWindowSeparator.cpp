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

AView_SubWindowSeparator
#725

*/

#include "stdafx.h"

#include "AView_SubWindowSeparator.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_SubWindowSeparator
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^

@param inWindowID �e�E�C���h�E��ID
@param inID View��ControlID
*/
AView_SubWindowSeparator::AView_SubWindowSeparator( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mPreviousMouseGlobalPoint(kGlobalPoint_00)//, mDragReceiving(false)
{
}

/**
�f�X�g���N�^
*/
AView_SubWindowSeparator::~AView_SubWindowSeparator()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_SubWindowSeparator::NVIDO_Init()
{
	/*
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_SubWindowTitlebar);
	NVMC_EnableDrop(scrapTypeArray);
	*/
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_SubWindowSeparator::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_SubWindowSeparator::EVTDO_DoDraw()
{
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	
#if 0
	if( mDragReceiving == true )
	{
		ALocalRect	r = localFrameRect;
		r.top = r.bottom - 4;
		AColor	color = GetApp().SPI_GetSubWindowBackgroundColor(true);
		NVMC_PaintRect(r,color);
		//
		AColor	dragColor = AColorWrapper::GetColorByHTMLFormatColor("526cdc");
		//
		ALocalPoint	caretLocalStart = {localFrameRect.left+8,localFrameRect.bottom-4};
		ALocalPoint	caretLocalEnd = {localFrameRect.right-5,localFrameRect.bottom-4};
		NVMC_DrawLine(caretLocalStart,caretLocalEnd,dragColor,1.0,0,2.0);
		//
		ALocalRect	circleRect = {0};
		circleRect.left		= caretLocalStart.x -7;
		circleRect.top		= caretLocalStart.y - 3;
		circleRect.right	= caretLocalStart.x;
		circleRect.bottom	= caretLocalStart.y + 4;
		NVMC_DrawCircle(circleRect,dragColor,2);
	}
	return;
	
	//�C���[�W�`��
	ALocalPoint	pt = {0,0};
	/*
	NVMC_DrawImageFlexibleWidth(kImageID_HSeparator,kImageID_HSeparator,kImageID_HSeparator,
	pt,localFrameRect.right-localFrameRect.left);
	*/
	AColor	color = GetApp().SPI_GetSubWindowBackgroundColor(true);//NVI_IsActive());
	NVMC_PaintRect(localFrameRect,color);
	ALocalPoint	spt = {0,0}, ept = {localFrameRect.right,0};
	NVMC_DrawLine(spt,ept,kColor_Gray);
#endif
}

/**
�}�E�X�{�^���_�E��������
*/
ABool	AView_SubWindowSeparator::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//�O��}�E�X�ʒu�̕ۑ�
	AWindow::NVI_GetCurrentMouseLocation(mPreviousMouseGlobalPoint);
	
	//Tracking���[�h�ݒ�
	NVM_SetMouseTrackingMode(true);
	NVI_Refresh();
	return true;
}

/**
�}�E�X�{�^��������Tracking��OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_SubWindowSeparator::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�O���[�o�����W�ɕϊ�
	AGlobalPoint	mouseGlobalPoint = {0};
	AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
	//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
	if( mouseGlobalPoint.x == mPreviousMouseGlobalPoint.x && mouseGlobalPoint.y == mPreviousMouseGlobalPoint.y )
	{
		return;
	}
	//�e�L�X�gWindow��Separator�ړ���ʒm
	ANumber	hDelta = mouseGlobalPoint.y-mPreviousMouseGlobalPoint.y;
	ANumber	resultDelta = NVM_GetWindow().NVI_SeparatorMoved(NVM_GetWindow().NVI_GetOverlayParentWindowID(),NVI_GetControlID(),hDelta,false);
	mouseGlobalPoint.y = mPreviousMouseGlobalPoint.y + resultDelta;
	//�O��}�E�X�ʒu�̕ۑ�
	mPreviousMouseGlobalPoint = mouseGlobalPoint;
}

/**
�}�E�X�{�^��������Tracking�I������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
*/
void	AView_SubWindowSeparator::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	NVM_SetMouseTrackingMode(false);
	
	//�O���[�o�����W�ɕϊ�
	AGlobalPoint	mouseGlobalPoint = {0};
	AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
	//�e�L�X�gWindow��Separator�ړ��i�����j��ʒm
	ANumber	hDelta = mouseGlobalPoint.y-mPreviousMouseGlobalPoint.y;
	ANumber	resultDelta = NVM_GetWindow().NVI_SeparatorMoved(NVM_GetWindow().NVI_GetOverlayParentWindowID(),NVI_GetControlID(),hDelta,true);
	mouseGlobalPoint.y = mPreviousMouseGlobalPoint.y + resultDelta;
}

/**
�J�[�\���^�C�v�擾
*/
ACursorType	AView_SubWindowSeparator::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	return kCursorType_ResizeUpDown;
}

#if 0
/**
Drag&Drop�����iDrag���j
*/
void	AView_SubWindowSeparator::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation )
{
}

/**
Drag&Drop�����iDrag��View�ɓ������j
*/
void	AView_SubWindowSeparator::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_SubWindowTitlebar) == true )
	{
		mDragReceiving = true;
		NVI_Refresh();
	}
}

/**
Drag&Drop�����iDrag��View����o���j
*/
void	AView_SubWindowSeparator::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_SubWindowTitlebar) == true )
	{
		mDragReceiving = false;
		NVI_Refresh();
	}
}

/**
Drag&Drop�����iDrop�j
*/
void	AView_SubWindowSeparator::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_SubWindowTitlebar) == true )
	{
		AText	text;
		AScrapWrapper::GetDragData(inDragRef,kScrapType_SubWindowTitlebar,text);
		
		//
		ASubWindowLocationType	subWindowLocationType = kSubWindowLocationType_None;
		AIndex	subWindowLocationIndex = kIndex_Invalid;
		GetApp().SPI_GetSubWindowProperty(NVM_GetWindow().GetObjectID(),subWindowLocationType,subWindowLocationIndex);
		//
		ANumber	num = text.GetNumber();
		ASubWindowLocationType	droppedType = (ASubWindowLocationType)(num/100);
		AIndex	droppedIndex = (num%100);
		if( droppedIndex == 99 )
		{
			GetApp().SPI_AddOverlaySubWindow(subWindowLocationType,subWindowLocationIndex,(ASubWindowType)(num/100));
		}
		else
		{
			GetApp().SPI_MoveOverlaySubWindow(droppedType,droppedIndex,subWindowLocationType,subWindowLocationIndex);
		}
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#endif


