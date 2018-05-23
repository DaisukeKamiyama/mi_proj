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

#pragma once

#include "../AbsBase/ABase.h"

//カーソルタイプ
typedef ANumber ACursorType;
const ACursorType	kCursorType_Arrow = 0;
const ACursorType	kCursorType_IBeam = 1;
const ACursorType	kCursorType_ArrowCopy = 2;
const ACursorType	kCursorType_ResizeUpDown = 3;
const ACursorType	kCursorType_ResizeLeftRight = 4;
const ACursorType	kCursorType_ClosedHand = 5;//#606
const ACursorType	kCursorType_OpenHand = 6;//#606
const ACursorType	kCursorType_ResizeNWSE = 7;

/**
マウスカーソル Wrapper
*/
class ACursorWrapper
{
  public:
	static void	ObscureCursor();
	static void	ShowCursor();
	static void	SetCursor( const ACursorType inCursorType );
};



