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

AWindow

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "../Imp.h"
#include "../Wrapper.h"
#include "AApplication.h"
#include "AWindow.h"
#include "AView.h"

#pragma mark ===========================

#pragma mark ---���W�n�ϊ�
/*
[���W�n]�C���[�W���W�n
�C���[�W���W�n�̓h�L�������g�S�̘̂_���I�ȍ��W�n�ł���B
[���W�n]���[�J�����W�n
���[�J�����W�n�́A���ۂɉ�ʏ�ɕ\�������View�̍�������_�Ƃ�����W�n�ł���B
�C���[�W���W��Ԃ̈ꕔ��؂��������̂����[�J�����W��Ԃł���Ƃ�����B
�܂��A�E�C���h�E���W��Ԃ̈ꕔ��؂��������̂����[�J�����W��Ԃł���Ƃ�������B
[���W�n]�E�C���h�E���W�n
�E�C���h�E�̍�������_�Ƃ�����W�n�ł���B

���C���[�W���W(0,0)
+---------------+
|               |
|               |
|               |
|               |
|    ��mOriginOfLocalFrame�i�C���[�W���W�n�j
|    o------------+
|    |            |
|    |  ��ʏ��   |
|    |  View      |���X�N���[������ƁA���̗̈悪�ړ�����B
|    |            |
|    |            |
|    +------------+
|               |
|               |
+---------------+

�E�C���h�E
+------------+
���E�C���h�E���W(0,0)
o------------+
|   ��CWindowImp::GetControlPosition()�Ŏ擾�������W�i�E�C���h�E���W�n�j
|   o------+ |
|   |      |����L�̉�ʏ��View�Ɠ���̓��e
|   +------+ |
+------------+

*/

//���[�J�����W���C���[�W���W
inline void	AView::NVM_GetImagePointFromLocalPoint( const ALocalPoint& inLocalPoint, AImagePoint& outImagePoint ) const
{
	outImagePoint.x = inLocalPoint.x + mOriginOfLocalFrame.x;
	outImagePoint.y = inLocalPoint.y + mOriginOfLocalFrame.y;
}

//���[�J�����W���C���[�W���W
inline void	AView::NVM_GetImageRectFromLocalRect( const ALocalRect& inLocalRect, AImageRect& outImageRect ) const
{
	outImageRect.left	= inLocalRect.left		+ mOriginOfLocalFrame.x;
	outImageRect.top	= inLocalRect.top		+ mOriginOfLocalFrame.y;
	outImageRect.right	= inLocalRect.right		+ mOriginOfLocalFrame.x;
	outImageRect.bottom	= inLocalRect.bottom	+ mOriginOfLocalFrame.y;
}

//�C���[�W���W�����[�J�����W
inline void	AView::NVM_GetLocalPointFromImagePoint( const AImagePoint& inImagePoint, ALocalPoint& outLocalPoint ) const
{
	outLocalPoint.x = inImagePoint.x - mOriginOfLocalFrame.x;
	outLocalPoint.y = inImagePoint.y - mOriginOfLocalFrame.y;
}

//�C���[�W���W�����[�J�����W
inline void	AView::NVM_GetLocalRectFromImageRect( const AImageRect& inImageRect, ALocalRect& outLocalRect ) const
{
	outLocalRect.left	= inImageRect.left		- mOriginOfLocalFrame.x;
	outLocalRect.top	= inImageRect.top		- mOriginOfLocalFrame.y;
	outLocalRect.right	= inImageRect.right		- mOriginOfLocalFrame.x;
	outLocalRect.bottom	= inImageRect.bottom	- mOriginOfLocalFrame.y;
}



#pragma mark ===========================

#pragma mark ---ViewRect���擾

//���[�J�����W�n��View�S�̂�Rect���擾
inline void	AView::NVM_GetLocalViewRect( ALocalRect& outLocalRect ) const
{
	outLocalRect.left	= 0;
	outLocalRect.top	= 0;
	outLocalRect.right	= NVI_GetViewWidth();
	outLocalRect.bottom	= NVI_GetViewHeight();
}

//�C���[�W���W�n��View�S�̂�Rect���擾
inline void	AView::NVM_GetImageViewRect( AImageRect& outImageRect ) const
{
	ALocalRect	localRect;
	NVM_GetLocalViewRect(localRect);
	NVM_GetImageRectFromLocalRect(localRect,outImageRect);
}

//
inline ABool	AView::NVM_IsImagePointInViewRect( const AImagePoint& inImagePoint ) const
{
	AImageRect	imageViewRect;
	NVM_GetImageViewRect(imageViewRect);
	if( inImagePoint.x >= imageViewRect.left && inImagePoint.x <= imageViewRect.right &&
			inImagePoint.y >= imageViewRect.top && inImagePoint.y <= imageViewRect.bottom )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//
inline ANumber	AView::NVI_GetViewWidth() const
{
	//�ʏ��control��width�A�c�������[�h����control��height��Ԃ� #558
	if( mViewImp.GetVerticalMode() == false )
	{
		return NVM_GetWindowConst().NVI_GetControlWidth(mControlID);
	}
	else
	{
		return NVM_GetWindowConst().NVI_GetControlHeight(mControlID);
	}
}

//
inline ANumber	AView::NVI_GetViewHeight() const
{
	//�ʏ��control��height�A�c�������[�h����control��width��Ԃ� #558
	if( mViewImp.GetVerticalMode() == false )
	{
		return NVM_GetWindowConst().NVI_GetControlHeight(mControlID);
	}
	else
	{
		return NVM_GetWindowConst().NVI_GetControlWidth(mControlID);
	}
}

