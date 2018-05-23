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

#include "AKeyWrapper.h"
#import <Cocoa/Cocoa.h>
#include "../AbsBase/Cocoa.h"

#pragma mark ===========================

#pragma mark ---現在のキー状態取得

/**
Optionキーが押されているかどうかを取得
*/
ABool	AKeyWrapper::IsOptionKeyPressed()
{
	//#688 return ((::GetCurrentEventKeyModifiers()&optionKey)!=0);
	return AKeyWrapper::IsOptionKeyPressed(ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]));
}

//B0429
/**
Shiftキーが押されているかどうかを取得
*/
ABool	AKeyWrapper::IsShiftKeyPressed()
{
	//#688 return ((::GetCurrentEventKeyModifiers()&shiftKey)!=0);
	return AKeyWrapper::IsShiftKeyPressed(ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]));
}

//#606
/**
Commandキーが押されているかどうかを取得
*/
ABool	AKeyWrapper::IsCommandKeyPressed()
{
	return AKeyWrapper::IsCommandKeyPressed(ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]));
}

/**
Controlキーが押されているかどうかを取得
*/
ABool	AKeyWrapper::IsControlKeyPressed()
{
	return AKeyWrapper::IsControlKeyPressed(ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]));
}

//★未テスト
/**
*/
AModifierKeys	AKeyWrapper::GetCurrentModifierKeys()
{
	return ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
}

#pragma mark ===========================

#pragma mark ---イベントからキー状態取得

/**
各種Modifierキーが押されているかどうかを取得
*/
/*#688
AModifierKeys	AKeyWrapper::GetEventKeyModifiers( const AOSKeyEvent& inOSKeyEvent )
{
	OSStatus	err = noErr;
	
	if( inOSKeyEvent == kOSKeyEventNull )   return 0;//UnitTest用
	UInt32	modifiers = 0;
	EventRef	rawKeyEvent = NULL;
	err = ::GetEventParameter(inOSKeyEvent,kEventParamTextInputSendKeyboardEvent,typeEventRef,NULL,sizeof(EventRef),NULL,&rawKeyEvent);
	if( rawKeyEvent == NULL )   return 0;
	err = ::GetEventParameter(rawKeyEvent,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(UInt32),NULL,&modifiers);
	return modifiers;
}
*/

/**
AModifierKeys型からOptionKeyの状態を取得
*/
ABool	AKeyWrapper::IsOptionKeyPressed( const AModifierKeys inModifierKeys )
{
	//#688 return ((inModifierKeys&optionKey)!=0);
	return ((inModifierKeys&kModifierKeysMask_Option)!=0);
}

/**
AModifierKeys型からControlKeyの状態を取得
*/
ABool	AKeyWrapper::IsControlKeyPressed( const AModifierKeys inModifierKeys )
{
	//#688 return ((inModifierKeys&controlKey)!=0);
	return ((inModifierKeys&kModifierKeysMask_Control)!=0);
}

/**
AModifierKeys型からShiftKeyの状態を取得
*/
ABool	AKeyWrapper::IsShiftKeyPressed( const AModifierKeys inModifierKeys )
{
	//#688 return ((inModifierKeys&shiftKey)!=0);
	return ((inModifierKeys&kModifierKeysMask_Shift)!=0);
}

/**
AModifierKeys型からCommandKeyの状態を取得
*/
ABool	AKeyWrapper::IsCommandKeyPressed( const AModifierKeys inModifierKeys )
{
	//#688 return ((inModifierKeys&cmdKey)!=0);
	return ((inModifierKeys&kModifierKeysMask_Command)!=0);
}

/**
AModifierKeys型データを生成
*/
AModifierKeys	AKeyWrapper::MakeModifierKeys( const ABool inControl, const ABool inOption, const ABool inCommand, const ABool inShift )
{
	AModifierKeys	modifiers = 0;
	if( inControl == true )
	{
		//#688 modifiers += controlKey;
		modifiers |= kModifierKeysMask_Control;
	}
	if( inOption == true )
	{
		//#688 modifiers += optionKey;
		modifiers |= kModifierKeysMask_Option;
	}
	if( inCommand == true )
	{
		//#688 modifiers += cmdKey;
		modifiers |= kModifierKeysMask_Command;
	}
	if( inShift == true )
	{
		//#688 modifiers += shiftKey;
		modifiers |= kModifierKeysMask_Shift;
	}
	return modifiers;
}

#pragma mark ===========================

#pragma mark ---メニューのModifierKey

/**
AMenuShortcutModifierKeys型データを生成
*/
AMenuShortcutModifierKeys	AKeyWrapper::MakeMenuShortcutModifierKeys( const ABool inControl, const ABool inOption, const ABool inShift )
{
	AMenuShortcutModifierKeys	modifiers = 0;
	if( inControl == true )
	{
		modifiers |= kModifierKeysMask_Control;//#688 kMenuControlModifier;
	}
	if( inOption == true )
	{
		modifiers |= kModifierKeysMask_Option;//#688 kMenuOptionModifier;
	}
	if( inShift == true )
	{
		modifiers |= kModifierKeysMask_Shift;//#688 kMenuShiftModifier;
	}
	return modifiers;
}

/**
AMenuShortcutModifierKeys型からShiftKeyの状態を取得
*/
ABool	AKeyWrapper::IsControlKeyOnMenuShortcut( const AMenuShortcutModifierKeys inModifierKeys )
{
	return ((inModifierKeys&/*#688kMenuControlModifier*/kModifierKeysMask_Control)!=0);
}

/**
AMenuShortcutModifierKeys型からOptionKeyの状態を取得
*/
ABool	AKeyWrapper::IsOptionKeyOnMenuShortcut( const AMenuShortcutModifierKeys inModifierKeys )
{
	return ((inModifierKeys&/*#688kMenuOptionModifier*/kModifierKeysMask_Option)!=0);
}

/**
AMenuShortcutModifierKeys型からShiftKeyの状態を取得
*/
ABool	AKeyWrapper::IsShiftKeyOnMenuShortcut( const AMenuShortcutModifierKeys inModifierKeys )
{
	return ((inModifierKeys&/*#688kMenuShiftModifier*/kModifierKeysMask_Shift)!=0);
}

#pragma mark ===========================

#pragma mark ---キーバインド

#if SUPPORT_CARBON
/**
イベントからKeyBindKey型のキー種別を取得
*/
AKeyBindKey	AKeyWrapper::GetKeyBindKey( const AOSKeyEvent& inOSKeyEvent )
{
	AKeyBindKey	keyBindKey = kKeyBindKey_Invalid;
	if( inOSKeyEvent == kOSKeyEventNull )   return kKeyBindKey_Invalid;//UnitTest用
	
	EventRef	rawKeyEvent = NULL;
	::GetEventParameter(inOSKeyEvent,kEventParamTextInputSendKeyboardEvent,typeEventRef,NULL,sizeof(EventRef),NULL,&rawKeyEvent);
	if( rawKeyEvent == NULL )   return keyBindKey;
	//unsigned char	theChar;
	//::GetEventParameter(rawKeyEvent,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(unsigned char),NULL,&theChar);
	UInt32	theCode = 0;
	::GetEventParameter(rawKeyEvent,kEventParamKeyCode,typeUInt32,NULL,sizeof(UInt32),NULL,&theCode);
	//UInt32	modifiers;
	//::GetEventParameter(rawKeyEvent,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(UInt32),NULL,&modifiers);
	
	switch(theCode)
	{
	  case 0x00:
		return kKeyBindKey_A;
	  case 0x0B:
		return kKeyBindKey_B;
	  case 0x08:
		return kKeyBindKey_C;
	  case 0x02:
		return kKeyBindKey_D;
	  case 0x0E:
		return kKeyBindKey_E;
	  case 0x03:
		return kKeyBindKey_F;
	  case 0x05:
		return kKeyBindKey_G;
	  case 0x04:
		return kKeyBindKey_H;
	  case 0x22:
		return kKeyBindKey_I;
	  case 0x26:
		return kKeyBindKey_J;
	  case 0x28:
		return kKeyBindKey_K;
	  case 0x25:
		return kKeyBindKey_L;
	  case 0x2E:
		return kKeyBindKey_M;
	  case 0x2D:
		return kKeyBindKey_N;
	  case 0x1F:
		return kKeyBindKey_O;
	  case 0x23:
		return kKeyBindKey_P;
	  case 0x0C:
		return kKeyBindKey_Q;
	  case 0x0F:
		return kKeyBindKey_R;
	  case 0x01:
		return kKeyBindKey_S;
	  case 0x11:
		return kKeyBindKey_T;
	  case 0x20:
		return kKeyBindKey_U;
	  case 0x09:
		return kKeyBindKey_V;
	  case 0x0D:
		return kKeyBindKey_W;
	  case 0x07:
		return kKeyBindKey_X;
	  case 0x10:
		return kKeyBindKey_Y;
	  case 0x06:
		return kKeyBindKey_Z;
	  case 0x7A:
		return kKeyBindKey_F1;
	  case 0x78:
		return kKeyBindKey_F2;
	  case 0x63:
		return kKeyBindKey_F3;
	  case 0x76:
		return kKeyBindKey_F4;
	  case 0x60:
		return kKeyBindKey_F5;
	  case 0x61:
		return kKeyBindKey_F6;
	  case 0x62:
		return kKeyBindKey_F7;
	  case 0x64:
		return kKeyBindKey_F8;
	  case 0x65:
		return kKeyBindKey_F9;
	  case 0x6D:
		return kKeyBindKey_F10;
	  case 0x67:
		return kKeyBindKey_F11;
	  case 0x6F:
		return kKeyBindKey_F12;
	  case 0x69:
		return kKeyBindKey_F13;
	  case 0x6B:
		return kKeyBindKey_F14;
	  case 0x71:
		return kKeyBindKey_F15;
	  case 0x24:
		return kKeyBindKey_Return;
	  case 0x30:
		return kKeyBindKey_Tab;
	  case 0x31:
		return kKeyBindKey_Space;
	  case 0x35:
		return kKeyBindKey_Escape;
	  case 0x4C:
		return kKeyBindKey_Enter;
	  case 0x33:
		return kKeyBindKey_BS;
	  case 0x75:
		return kKeyBindKey_DEL;
	  case 0x7E:
		return kKeyBindKey_Up;
	  case 0x7D:
		return kKeyBindKey_Down;
	  case 0x7B:
		return kKeyBindKey_Left;
	  case 0x7C:
		return kKeyBindKey_Right;
	  case 0x27:
		return kKeyBindKey_Colon;
	  case 0x29:
		return kKeyBindKey_Semicolon;
	  case 0x1B:
		return kKeyBindKey_Minus;
	  case 0x18:
		return kKeyBindKey_Hat;
	  case 0x5D:
		return kKeyBindKey_Backslash;
	  case 0x21:
		return kKeyBindKey_Atmark;
	  case 0x1E:
		return kKeyBindKey_BracketStart;
	  case 0x2A:
		return kKeyBindKey_BracketEnd;
	  case 0x2B:
		return kKeyBindKey_Comma;
	  case 0x2F:
		return kKeyBindKey_Period;
	  case 0x2C:
		return kKeyBindKey_Slash;
	  case 0x73:
		return kKeyBindKey_Home;
	  case 0x77:
		return kKeyBindKey_End;
	  case 0x74:
		return kKeyBindKey_PageUp;
	  case 0x79:
		return kKeyBindKey_PageDown;
	  default:
		return kKeyBindKey_Invalid;
	}
}
#endif

/**
KeyCodeからKeyBindKeyへの変換（Legacy設定データからの変換用）
*/
AKeyBindKey	AKeyWrapper::GetKeyBindKeyFromKeyCode( short inKeyCode )
{
	AKeyBindKey	key = kKeyBindKey_Invalid;
	if( inKeyCode >= 'A' && inKeyCode <= 'Z' ) 
	{
		key = static_cast<AKeyBindKey>(inKeyCode-'A'+kKeyBindKey_A);
	}
	else if( inKeyCode >= 0x81 && inKeyCode <= 0x8F )
	{
		key = static_cast<AKeyBindKey>(inKeyCode-0x81+kKeyBindKey_F1);
	}
	else switch(inKeyCode)
	{
	  case '\r':
		{
			key = kKeyBindKey_Return;
			break;
		}
	  case '\t':
		{
			key = kKeyBindKey_Tab;
			break;
		}
	  case ' ':
		{
			key = kKeyBindKey_Space;
			break;
		}
	  case 0x1B:
		{
			key = kKeyBindKey_Escape;
			break;
		}
	  case 3:
		{
			key = kKeyBindKey_Enter;
			break;
		}
	  case 8:
		{
			key = kKeyBindKey_BS;
			break;
		}
	  case 127:
		{
			key = kKeyBindKey_DEL;
			break;
		}
	  case 0x1E:
		{
			key = kKeyBindKey_Up;
			break;
		}
	  case 0x1F:
		{
			key = kKeyBindKey_Down;
			break;
		}
	  case 0x1C:
		{
			key = kKeyBindKey_Left;
			break;
		}
	  case 0x1D:
		{
			key = kKeyBindKey_Right;
			break;
		}
	  case ':':
		{
			key = kKeyBindKey_Colon;
			break;
		}
	  case ';':
		{
			key = kKeyBindKey_Semicolon;
			break;
		}
	  case '-':
		{
			key = kKeyBindKey_Minus;
			break;
		}
	  case '^':
		{
			key = kKeyBindKey_Hat;
			break;
		}
	  case '\\':
		{
			key = kKeyBindKey_Backslash;
			break;
		}
	  case '@':
		{
			key = kKeyBindKey_Atmark;
			break;
		}
	  case '[':
		{
			key = kKeyBindKey_BracketStart;
			break;
		}
	  case ']':
		{
			key = kKeyBindKey_BracketEnd;
			break;
		}
	  case ',':
		{
			key = kKeyBindKey_Comma;
			break;
		}
	  case '.':
		{
			key = kKeyBindKey_Period;
			break;
		}
	  case '/':
		{
			key = kKeyBindKey_Slash;
			break;
		}
	  case 1://home
		{
			key = kKeyBindKey_Home;
			break;
		}
	  case 4://end
		{
			key = kKeyBindKey_End;
			break;
		}
	  case 11://pageup
		{
			key = kKeyBindKey_PageUp;
			break;
		}
	  case 12://pagedown
		{
			key = kKeyBindKey_PageDown;
			break;
		}
	}
	return key;
}


