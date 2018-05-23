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

AView_SizeBox

*/

#include "stdafx.h"

#include "AView_SizeBox.h"
#include "../Imp.h"
#include "../Wrapper.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_SizeBox
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AView_SizeBox::AView_SizeBox( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mMouseTrackingMode(kMouseTrackingMode_None), mHover(false), mBackgroundColor(kColor_White)
,mClickLocalPoint(kLocalPoint_00),mPreviousMouseGlobalPoint(kGlobalPoint_00)
{
	NVI_SetDefaultCursor(kCursorType_ResizeNWSE);
	NVMC_EnableMouseLeaveEvent();
}

/**
�f�X�g���N�^
*/
AView_SizeBox::~AView_SizeBox()
{
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_SizeBox::NVIDO_Init()
{
}

/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_SizeBox::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

/**
�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
*/
ABool	AView_SizeBox::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//�O��}�E�X�ʒu�̕ۑ�
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,mPreviousMouseGlobalPoint);
	//�ŏ��̃N���b�N����local point�ۑ�
	mClickLocalPoint = inLocalPoint;
	
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	NVM_SetMouseTrackingMode(true);
	//�ΏۃE�C���h�E�i���ݒ�Ȃ�e�E�C���h�E�j�̃T�C�Y��ύX
	AWindowID	winID = NVM_GetWindow().GetObjectID();
	if( NVM_GetWindow().NVI_GetSizeBoxTargetWindowID() != kObjectID_Invalid )
	{
		winID = NVM_GetWindow().NVI_GetSizeBoxTargetWindowID();
	}
	//���T�C�Y����ݒ�
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_SetVirtualLiveResizing(true);
	return false;
}

/**
�}�E�X�ړ��C�x���g
*/
ABool	AView_SizeBox::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	return true;
}

/**
�}�E�X�̈�O�ړ��C�x���g
*/
void	AView_SizeBox::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
}

//Mouse Tracking
void	AView_SizeBox::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	//�O���[�o�����W�ɕϊ�
	AGlobalPoint	mouseGlobalPoint = {0};
	AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
	//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
	if( mouseGlobalPoint.x == mPreviousMouseGlobalPoint.x && mouseGlobalPoint.y == mPreviousMouseGlobalPoint.y )
	{
		return;
	}
	//�ΏۃE�C���h�E�i���ݒ�Ȃ�e�E�C���h�E�j�擾
	AWindowID	winID = NVM_GetWindow().GetObjectID();
	if( NVM_GetWindow().NVI_GetSizeBoxTargetWindowID() != kObjectID_Invalid )
	{
		winID = NVM_GetWindow().NVI_GetSizeBoxTargetWindowID();
	}
	//�E�C���h�E���T�C�Y
	ARect	bounds = {0};
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_GetWindowBounds(bounds);
	bounds.right = mouseGlobalPoint.x + (viewRect.right - viewRect.left - mClickLocalPoint.x);
	bounds.bottom = mouseGlobalPoint.y + (viewRect.bottom - viewRect.top - mClickLocalPoint.y);
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_SetWindowBounds(bounds);
	//�O��}�E�X�ʒu�̕ۑ�
	mPreviousMouseGlobalPoint = mouseGlobalPoint;
}

//Mouse Tracking�I���i�}�E�X�{�^��Up�j
void	AView_SizeBox::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	//�O���[�o�����W�ɕϊ�
	AGlobalPoint	mouseGlobalPoint = {0};
	AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
	//�ΏۃE�C���h�E�i���ݒ�Ȃ�e�E�C���h�E�j�擾
	AWindowID	winID = NVM_GetWindow().GetObjectID();
	if( NVM_GetWindow().NVI_GetSizeBoxTargetWindowID() != kObjectID_Invalid )
	{
		winID = NVM_GetWindow().NVI_GetSizeBoxTargetWindowID();
	}
	//���T�C�Y����Ԃ�����
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_SetVirtualLiveResizing(false);
	//�E�C���h�E���T�C�Y
	ARect	bounds = {0};
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_GetWindowBounds(bounds);
	bounds.right = mouseGlobalPoint.x + (viewRect.right - viewRect.left - mClickLocalPoint.x);
	bounds.bottom = mouseGlobalPoint.y + (viewRect.bottom - viewRect.top - mClickLocalPoint.y);
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_SetWindowBounds(bounds);
	//
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_ConstrainWindowPosition(true);
	
	//TrackingMode�����iDoMouseTrackEnd()�̑O�ɂ��Ȃ��ƁADoMouseTrackEnd()��Separator�R���g���[�����폜����Ă��܂��̂Łj
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}

/**
�`��v�����̃R�[���o�b�N(AView�p)
*/
void	AView_SizeBox::EVTDO_DoDraw()
{
	//�w�i�h��Ԃ�
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	NVMC_PaintRoundedRect(viewRect,mBackgroundColor,mBackgroundColor,kGradientType_Horizontal,1.0,1.0,
						  5.0,false,false,false,true);
	
}

