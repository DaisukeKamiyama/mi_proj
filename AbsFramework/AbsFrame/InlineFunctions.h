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

#pragma mark ---座標系変換
/*
[座標系]イメージ座標系
イメージ座標系はドキュメント全体の論理的な座標系である。
[座標系]ローカル座標系
ローカル座標系は、実際に画面上に表示されるViewの左上を原点とする座標系である。
イメージ座標空間の一部を切り取ったものがローカル座標空間であるといえる。
また、ウインドウ座標空間の一部を切り取ったものがローカル座標空間であるともいえる。
[座標系]ウインドウ座標系
ウインドウの左上を原点とする座標系である。

↓イメージ座標(0,0)
+---------------+
|               |
|               |
|               |
|               |
|    ↓mOriginOfLocalFrame（イメージ座標系）
|    o------------+
|    |            |
|    |  画面上の   |
|    |  View      |←スクロールすると、この領域が移動する。
|    |            |
|    |            |
|    +------------+
|               |
|               |
+---------------+

ウインドウ
+------------+
↓ウインドウ座標(0,0)
o------------+
|   ↓CWindowImp::GetControlPosition()で取得される座標（ウインドウ座標系）
|   o------+ |
|   |      |←上記の画面上のViewと同一の内容
|   +------+ |
+------------+

*/

//ローカル座標→イメージ座標
inline void	AView::NVM_GetImagePointFromLocalPoint( const ALocalPoint& inLocalPoint, AImagePoint& outImagePoint ) const
{
	outImagePoint.x = inLocalPoint.x + mOriginOfLocalFrame.x;
	outImagePoint.y = inLocalPoint.y + mOriginOfLocalFrame.y;
}

//ローカル座標→イメージ座標
inline void	AView::NVM_GetImageRectFromLocalRect( const ALocalRect& inLocalRect, AImageRect& outImageRect ) const
{
	outImageRect.left	= inLocalRect.left		+ mOriginOfLocalFrame.x;
	outImageRect.top	= inLocalRect.top		+ mOriginOfLocalFrame.y;
	outImageRect.right	= inLocalRect.right		+ mOriginOfLocalFrame.x;
	outImageRect.bottom	= inLocalRect.bottom	+ mOriginOfLocalFrame.y;
}

//イメージ座標→ローカル座標
inline void	AView::NVM_GetLocalPointFromImagePoint( const AImagePoint& inImagePoint, ALocalPoint& outLocalPoint ) const
{
	outLocalPoint.x = inImagePoint.x - mOriginOfLocalFrame.x;
	outLocalPoint.y = inImagePoint.y - mOriginOfLocalFrame.y;
}

//イメージ座標→ローカル座標
inline void	AView::NVM_GetLocalRectFromImageRect( const AImageRect& inImageRect, ALocalRect& outLocalRect ) const
{
	outLocalRect.left	= inImageRect.left		- mOriginOfLocalFrame.x;
	outLocalRect.top	= inImageRect.top		- mOriginOfLocalFrame.y;
	outLocalRect.right	= inImageRect.right		- mOriginOfLocalFrame.x;
	outLocalRect.bottom	= inImageRect.bottom	- mOriginOfLocalFrame.y;
}



#pragma mark ===========================

#pragma mark ---ViewRect情報取得

//ローカル座標系でView全体のRectを取得
inline void	AView::NVM_GetLocalViewRect( ALocalRect& outLocalRect ) const
{
	outLocalRect.left	= 0;
	outLocalRect.top	= 0;
	outLocalRect.right	= NVI_GetViewWidth();
	outLocalRect.bottom	= NVI_GetViewHeight();
}

//イメージ座標系でView全体のRectを取得
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
	//通常はcontrolのwidth、縦書きモード時はcontrolのheightを返す #558
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
	//通常はcontrolのheight、縦書きモード時はcontrolのwidthを返す #558
	if( mViewImp.GetVerticalMode() == false )
	{
		return NVM_GetWindowConst().NVI_GetControlHeight(mControlID);
	}
	else
	{
		return NVM_GetWindowConst().NVI_GetControlWidth(mControlID);
	}
}

