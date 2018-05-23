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

ACursorWrapper

*/

#include "stdafx.h"

#include "ACursorWrapper.h"
#include "../Imp.h"

/**
マウスカーソル消去
*/
void	ACursorWrapper::ObscureCursor()
{
	CURSORINFO	ci = {0};
	ci.cbSize = sizeof(ci);
	if( ::GetCursorInfo(&ci) != 0 )
	{
		if( ci.flags == CURSOR_SHOWING )
		{
			::ShowCursor(FALSE);//カーソル表示カウンタを-1する
		}
	}
}

/**
マウスカーソル表示
*/
void	ACursorWrapper::ShowCursor()
{
	CURSORINFO	ci = {0};
	ci.cbSize = sizeof(ci);
	if( ::GetCursorInfo(&ci) != 0 )
	{
		if( ci.flags != CURSOR_SHOWING )
		{
			::ShowCursor(TRUE);//カーソル表示カウンタを+1する
		}
	}
}

/**
マウスカーソル設定
*/
void	ACursorWrapper::SetCursor( const ACursorType inCursorType )
{
	switch(inCursorType)
	{
	  case kCursorType_IBeam:
		{
			HCURSOR hcursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_IBEAM));
			::SetCursor(hcursor);
			break;
		}
	  case kCursorType_ArrowCopy:
		{
			HCURSOR hcursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));//★要検討  
			::SetCursor(hcursor);
			break;
		}
	  case kCursorType_ResizeUpDown:
		{
			HCURSOR hcursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS));
			::SetCursor(hcursor);
			break;
		}
	  case kCursorType_ResizeLeftRight:
		{
			HCURSOR hcursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE));
			::SetCursor(hcursor);
			break;
		}
	  case kCursorType_ResizeNWSE:
		{
			HCURSOR hcursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENWSE));
			::SetCursor(hcursor);
			break;
		}
	  case kCursorType_Arrow:
	  default:
		{
			HCURSOR hcursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));
			::SetCursor(hcursor);
			break;
		}
	}
	ACursorWrapper::ShowCursor();
}
