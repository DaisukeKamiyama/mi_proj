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

#include "ACursorWrapper.h"
#import <Cocoa/Cocoa.h>


/**
マウスカーソル消去
*/
void	ACursorWrapper::ObscureCursor()
{
	[NSCursor setHiddenUntilMouseMoves:YES];
	/*#688
	::ObscureCursor();
	*/
}

/**
マウスカーソル設定
*/
void	ACursorWrapper::SetCursor( const ACursorType inCursorType )
{
	//#606
	//現在のカーソルタイプ記憶
	sCurrentCursorType = inCursorType;
	//
	switch(inCursorType)
	{
	  case kCursorType_IBeam:
		{
			[[NSCursor IBeamCursor] set];
			break;
		}
	  case kCursorType_IBeamForVertical://#558
		{
			[[NSCursor IBeamCursorForVerticalLayout] set];
			break;
		}
	  case kCursorType_ArrowCopy:
		{
			//★
			[[NSCursor arrowCursor] set];
			break;
		}
	  case kCursorType_ResizeUpDown:
		{
			[[NSCursor resizeUpDownCursor] set];
			break;
		}
	  case kCursorType_ResizeLeftRight:
		{
			[[NSCursor resizeLeftRightCursor] set];
			break;
		}
	  case kCursorType_OpenHand:
		{
			[[NSCursor openHandCursor] set];
			break;
		}
	  case kCursorType_ClosedHand:
		{
			[[NSCursor closedHandCursor] set];
			break;
		}
	  case kCursorType_PointingHand:
		{
			[[NSCursor pointingHandCursor] set];
			break;
		}
	  case kCursorType_ResizeNWSE://Mac OS Xにはこれがないので、arrowにする。
	  case kCursorType_Arrow:
	  default:
		{
			[[NSCursor arrowCursor] set];
			break;
		}
	}
	/*#688
	OSStatus	err = noErr;
	
	switch(inCursorType)
	{
	  case kCursorType_IBeam:
		{
			err = ::SetThemeCursor(kThemeIBeamCursor);
			if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,NULL);
			break;
		}
	  case kCursorType_ArrowCopy:
		{
			err = ::SetThemeCursor(kThemeCopyArrowCursor);
			if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,NULL);
			break;
		}
	  case kCursorType_ResizeUpDown:
		{
			err = ::SetThemeCursor(kThemeResizeUpDownCursor);
			if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,NULL);
			break;
		}
	  case kCursorType_ResizeLeftRight:
		{
			err = ::SetThemeCursor(kThemeResizeLeftRightCursor);
			if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,NULL);
			break;
		}
		//#606
	  case kCursorType_OpenHand:
		{
			err = ::SetThemeCursor(kThemeOpenHandCursor);
			if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,NULL);
			break;
		}
	  case kCursorType_ClosedHand:
		{
			err = ::SetThemeCursor(kThemeClosedHandCursor);
			if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,NULL);
			break;
		}
		//
	  case kCursorType_Arrow:
	  default:
		{
			err = ::SetThemeCursor(kThemeArrowCursor);
			if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,NULL);
			break;
		}
	}
	*/
}

//#606
//現在のカーソルタイプ
ACursorType	ACursorWrapper::sCurrentCursorType = kCursorType_Arrow;

