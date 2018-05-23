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

AWindowWrapper

*/
/*
CWindowImpへ移動

#include "AWindowWrapper.h"

**
最前面ウインドウ取得
*
AWindowRef	AWindowWrapper::GetMostFrontWindow()
{
	return ::FrontNonFloatingWindow();
}

**
次のウインドウ取得
*
AWindowRef	AWindowWrapper::GetNextOrderWindow( const AWindowRef& inWindowRef )
{
	if( inWindowRef == NULL )   return NULL;
	return ::GetNextWindow(inWindowRef);
}

**
N番目のウインドウ取得
*
AWindowRef	AWindowWrapper::GetNthWindow( const ANumber& inNumber )
{
	if( inNumber <= 0 )   return NULL;
	ANumber	num = 0;
	for( AWindowRef windowRef = GetMostFrontWindow(); windowRef != NULL; windowRef = GetNextWindow(windowRef) )
	{
		num++;
		if( num == inNumber )   return windowRef;
	}
	return NULL;
}

**
メインデバイス上でウインドウ配置可能な領域を取得（Dock, Menubar等に重ならない領域）
*
void	AWindowWrapper::GetAvailableWindowPositioningBounds( ARect& outRect )
{
	OSStatus	err = noErr;
	
	Rect	rect;
	err = ::GetAvailableWindowPositioningBounds(::GetMainDevice(),&rect);
	if( err != noErr )   _ACErrorNum("GetAvailableWindowPositioningBounds() returned error: ",err,NULL);
	outRect.left 	= rect.left;
	outRect.top 	= rect.top;
	outRect.right 	= rect.right;
	outRect.bottom	= rect.bottom;
}

*/