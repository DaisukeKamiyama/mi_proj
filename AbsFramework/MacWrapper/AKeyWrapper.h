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

AKeyWrapper

*/

#pragma once

#include "../AbsBase/ABase.h"

/**
キー Wrapper
*/
class AKeyWrapper
{
  public:
	//現在のキー状態取得
	static ABool	IsOptionKeyPressed();
	static ABool	IsShiftKeyPressed();//B0429
	static ABool	IsCommandKeyPressed();//#606
	static ABool	IsControlKeyPressed();
	static AModifierKeys	GetCurrentModifierKeys();//#1017
	
	//イベントからキー状態取得
	//#688 static AModifierKeys	GetEventKeyModifiers( const AOSKeyEvent& inOSKeyEvent );
	static ABool	IsOptionKeyPressed( const AModifierKeys inModifierKeys );
	static ABool	IsControlKeyPressed( const AModifierKeys inModifierKeys );
	static ABool	IsShiftKeyPressed( const AModifierKeys inModifierKeys );
	static ABool	IsCommandKeyPressed( const AModifierKeys inModifierKeys );
	static AModifierKeys	MakeModifierKeys( const ABool inControl, const ABool inOption, const ABool inCommand, const ABool inShift );
	
	//メニューのModifierKey
	static AMenuShortcutModifierKeys	MakeMenuShortcutModifierKeys( const ABool inControl, const ABool inOption, const ABool inShift );
	static ABool	IsControlKeyOnMenuShortcut( const AMenuShortcutModifierKeys inModifierKeys );
	static ABool	IsOptionKeyOnMenuShortcut( const AMenuShortcutModifierKeys inModifierKeys );
	static ABool	IsShiftKeyOnMenuShortcut( const AMenuShortcutModifierKeys inModifierKeys );
	
	//キーバインド
	//#688 static AKeyBindKey	GetKeyBindKey( const AOSKeyEvent& inOSKeyEvent );
	static AKeyBindKey	GetKeyBindKeyFromKeyCode( short inKeyCode );
};



