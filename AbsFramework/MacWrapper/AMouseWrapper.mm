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

AMouseWrapper

*/

#include "AMouseWrapper.h"
//#include "../Abs/AUtil.h"

/**
マウスボタン押下Tracking
*/
/*#688
ABool	AMouseWrapper::TrackMouseDown( AWindowPoint& outMousePoint, AModifierKeys& outModifierKeys )
{
	OSStatus	err = noErr;
	
	//B0353 イベント発生時点で既にボタンを離されている場合はトラックする必要はない（タップの場合、既にタップが終了してからイベントがくることがある？）
	if( ::GetCurrentEventButtonState() == 0 )
	{
		return false;
	}
	
	MouseTrackingResult	result = kMouseTrackingMousePressed;
	UInt32	modifiers;
	Point	mouseLoc;
	err = ::TrackMouseLocationWithOptions(NULL,NULL,0.01,&mouseLoc,&modifiers,&result);
	if( err != noErr )   _ACErrorNum("TrackMouseLocationWithOptions() returned error: ",err,NULL);
	outMousePoint.x = mouseLoc.h;
	outMousePoint.y = mouseLoc.v;
	outModifierKeys = modifiers;
	return (result!=kMouseTrackingMouseReleased);
}
*/

