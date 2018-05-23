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

#include "stdafx.h"

#include "AKeyWrapper.h"

ABool	AKeyWrapper::IsOptionKeyPressed()
{
	if( ::GetKeyState(VK_ESCAPE) < 0 )//Option�L�[�͂Ȃ��̂ŁAESC�ő�p
	{
		return true;
	}
	else
	{
		return false;
	}
}

ABool	AKeyWrapper::IsControlKeyPressed()
{
	if( ::GetKeyState(VK_CONTROL) < 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

AModifierKeys	AKeyWrapper::GetEventKeyModifiers( const AOSKeyEvent& inOSKeyEvent )
{
	return GetEventKeyModifiers();
}

AModifierKeys	AKeyWrapper::GetEventKeyModifiers()
{
	AModifierKeys	keys = 0;
	SHORT	a = ::GetKeyState(VK_SHIFT);
	if( ::GetKeyState(VK_SHIFT) < 0 )
	{
		keys |= kModifierKeysMask_Shift;
	}
	if( ::GetKeyState(VK_CONTROL) < 0 )
	{
		keys |= kModifierKeysMask_Control;
	}
	if( ::GetKeyState(VK_MENU) < 0 )//Alt�L�[
	{
		keys |= kModifierKeysMask_Command;
	}
	if( ::GetKeyState(VK_ESCAPE) < 0 )
	{
		keys |= kModifierKeysMask_Option;
	}
	return keys;
}

ABool	AKeyWrapper::IsOptionKeyPressed( const AModifierKeys inModifierKeys )
{
	if( (inModifierKeys&kModifierKeysMask_Option) != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

ABool	AKeyWrapper::IsControlKeyPressed( const AModifierKeys inModifierKeys )
{
	if( (inModifierKeys&kModifierKeysMask_Control) != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

ABool	AKeyWrapper::IsShiftKeyPressed( const AModifierKeys inModifierKeys )
{
	if( (inModifierKeys&kModifierKeysMask_Shift) != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

ABool	AKeyWrapper::IsCommandKeyPressed( const AModifierKeys inModifierKeys )
{
	if( (inModifierKeys&kModifierKeysMask_Command) != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

AModifierKeys	AKeyWrapper::MakeModifierKeys( const ABool inControl, const ABool inOption, const ABool inCommand, const ABool inShift )
{
	AModifierKeys	keys = 0;
	if( inControl == true )
	{
		keys |= kModifierKeysMask_Control;
	}
	if( inOption == true )
	{
		keys |= kModifierKeysMask_Option;
	}
	if( inShift == true )
	{
		keys |= kModifierKeysMask_Shift;
	}
	if( inCommand == true )
	{
		keys |= kModifierKeysMask_Command;
	}
	return keys;
}

AMenuShortcutModifierKeys	AKeyWrapper::MakeMenuShortcutModifierKeys( const ABool inControl, const ABool inOption, const ABool inShift )
{
	//inOption��Alt�Ƃ݂Ȃ�
	AModifierKeys	modifiers = 0;
	if( inControl == true )
	{
		modifiers |= kMenuShoftcutModifierKeysMask_Control;
	}
	if( inOption == true )
	{
		modifiers |= kMenuShoftcutModifierKeysMask_Alt;
	}
	if( inShift == true )
	{
		modifiers |= kMenuShoftcutModifierKeysMask_Shift;
	}
	return modifiers;
}

ABool	AKeyWrapper::IsControlKeyOnMenuShortcut( const AMenuShortcutModifierKeys inModifierKeys )
{
	if( (inModifierKeys&kMenuShoftcutModifierKeysMask_Control) != 0 )   return true;
	return false;
}

ABool	AKeyWrapper::IsOptionKeyOnMenuShortcut( const AMenuShortcutModifierKeys inModifierKeys )
{
	if( (inModifierKeys&kMenuShoftcutModifierKeysMask_Alt) != 0 )   return true;
	return false;
}

ABool	AKeyWrapper::IsShiftKeyOnMenuShortcut( const AMenuShortcutModifierKeys inModifierKeys )
{
	if( (inModifierKeys&kMenuShoftcutModifierKeysMask_Shift) != 0 )   return true;
	return false;
}

/*
Windows�̃L�[����
WM_KEYDOWN: �S�ẴL�[�ɑ΂��ė���C�x���g
WM_CHAR: �L�����N�^�L�[�݂̂ɑ΂��ė���C�x���g�BWM_CHAR�̌�ɗ���B

WM_CHAR�����邱�Ȃ��́AWM_KEYDOWN�̏����̎d���͉e�����Ȃ��B
�iWM_KEYDOWN�̕Ԃ�l��0�ɂ��āADefWindowProc()���R�[�����Ȃ��ꍇ�ł��AWM_CHAR�͗���B�j
����āAWM_CHAR�̑ΏۃL�[�ɑ΂���WM_KEYDOWN�ŏ��������Ă��܂��ƁA��d�ɏ��������Ă��܂��B

����āAWM_KEYDOWN�ŏ���������̂́AWM_CHAR�ɗ��Ȃ��L�[�̂݁B
�yWM_KEYDOWN�̏����Ώہz
A-Z, �L������: Ctrl, Ctrl+Shift�ƂƂ��ɉ�����Ă���ꍇ�̂ݏ�������
F1-F15: ��ɏ�������
���s�A�^�u�A�X�y�[�X�Aesc�ABS: ��ɏ������Ȃ�
enter: Windows�ɂ͑��݂��Ȃ��B���b�� �΍􌟓��K�v
�����L�[: ��ɏ�������
home, end, pageup, pagedown, delete: ��ɏ�������

�yWM_CHAR�̏����Ώہz
A-Z, �L������: ��������
F1-F15: �������Ȃ��iWM_KEYDOWN�ŏ����ρj
���s�A�^�u�A�X�y�[�X�Aesc�ABS: ��������
enter: Windows�ɂ͑��݂��Ȃ��B���b�� �΍􌟓��K�v
�����L�[�F �������Ȃ��iWM_KEYDOWN�ŏ����ρj
home, end, pageup, pagedown, delete: �������Ȃ��iWM_KEYDOWN�ŏ����ρj
*/

/**
�L�[�o�C���h�L�[�擾
*/
AKeyBindKey	AKeyWrapper::GetKeyBindKey( const AOSKeyEvent& inOSKeyEvent )
{
	AKeyBindKey	keyBindKey = kKeyBindKey_Invalid;
	if( inOSKeyEvent.message == WM_KEYDOWN )
	{
		//WM_KEYDOWN�̏ꍇ
		
		//A-Z, �L������: Ctrl, Ctrl+Shift�ƂƂ��ɉ�����A����ȊO��������Ă��Ȃ��ꍇ�̂ݏ�������
		//F1-F15: ��ɏ�������
		//���s�A�^�u�A�X�y�[�X�Aesc�ABS: ��ɏ������Ȃ�
		//enter: Windows�ɂ͑��݂��Ȃ��B���b�� �΍􌟓��K�v
		//�����L�[: ��ɏ�������
		//home, end, pageup, pagedown, delete: ��ɏ�������
		switch(inOSKeyEvent.wParam)
		{
			//A-Z, �L������: Ctrl, Ctrl+Shift�ƂƂ��ɉ�����Ă���ꍇ�̂ݏ�������
		  case 0x41:
		  case 0x42:
		  case 0x43:
		  case 0x44:
		  case 0x45:
		  case 0x46:
		  case 0x47:
		  case 0x48:
		  case 0x49:
		  case 0x4A:
		  case 0x4B:
		  case 0x4C:
		  case 0x4D:
		  case 0x4E:
		  case 0x4F:
		  case 0x50:
		  case 0x51:
		  case 0x52:
		  case 0x53:
		  case 0x54:
		  case 0x55:
		  case 0x56:
		  case 0x57:
		  case 0x58:
		  case 0x59:
		  case 0x5A:
		  case VK_OEM_1://0xBA
		  case VK_OEM_PLUS://0xBB
		  case VK_OEM_MINUS://0xBD
		  case VK_OEM_7://0xDE
		  case VK_OEM_5://0xDC
		  case VK_OEM_3://0xC0
		  case VK_OEM_4://0xDB
		  case VK_OEM_6://0xDD
		  case VK_OEM_COMMA://0xBC
		  case VK_OEM_PERIOD://0xBE
		  case VK_OEM_2://0xBF
			{
				AModifierKeys	modifiers = GetEventKeyModifiers(inOSKeyEvent);
				if( IsControlKeyPressed(modifiers) == false ||
							IsOptionKeyPressed(modifiers) == true ||
							IsCommandKeyPressed(modifiers) == true )
				{
					//�uCtrl, Ctrl+Shift�ƂƂ��ɉ�����A����ȊO��������Ă��Ȃ��ꍇ�v�ȊO�Ȃ̂ŏ������Ȃ��B
					//AAppPrefDB::Create_DefaultKeyBindTable()�Œ�`���ꂽ�L�[�o�C���h�\�����Ƃ���������
					return kKeyBindKey_Invalid;
				}
				switch(inOSKeyEvent.wParam)
				{
				  case 0x41:
					return kKeyBindKey_A;
				  case 0x42:
					return kKeyBindKey_B;
				  case 0x43:
					return kKeyBindKey_C;
				  case 0x44:
					return kKeyBindKey_D;
				  case 0x45:
					return kKeyBindKey_E;
				  case 0x46:
					return kKeyBindKey_F;
				  case 0x47:
					return kKeyBindKey_G;
				  case 0x48:
					return kKeyBindKey_H;
				  case 0x49:
					return kKeyBindKey_I;
				  case 0x4A:
					return kKeyBindKey_J;
				  case 0x4B:
					return kKeyBindKey_K;
				  case 0x4C:
					return kKeyBindKey_L;
				  case 0x4D:
					return kKeyBindKey_M;
				  case 0x4E:
					return kKeyBindKey_N;
				  case 0x4F:
					return kKeyBindKey_O;
				  case 0x50:
					return kKeyBindKey_P;
				  case 0x51:
					return kKeyBindKey_Q;
				  case 0x52:
					return kKeyBindKey_R;
				  case 0x53:
					return kKeyBindKey_S;
				  case 0x54:
					return kKeyBindKey_T;
				  case 0x55:
					return kKeyBindKey_U;
				  case 0x56:
					return kKeyBindKey_V;
				  case 0x57:
					return kKeyBindKey_W;
				  case 0x58:
					return kKeyBindKey_X;
				  case 0x59:
					return kKeyBindKey_Y;
				  case 0x5A:
					return kKeyBindKey_Z;
					//
				  case VK_OEM_1://0xBA
					return kKeyBindKey_Colon;
				  case VK_OEM_PLUS://0xBB
					return kKeyBindKey_Semicolon;
				  case VK_OEM_MINUS://0xBD
					return kKeyBindKey_Minus;
				  case VK_OEM_7://0xDE
					return kKeyBindKey_Hat;
				  case VK_OEM_5://0xDC
					return kKeyBindKey_Backslash;
				  case VK_OEM_3://0xC0
					return kKeyBindKey_Atmark;
				  case VK_OEM_4://0xDB
					return kKeyBindKey_BracketStart;
				  case VK_OEM_6://0xDD
					return kKeyBindKey_BracketEnd;
				  case VK_OEM_COMMA://0xBC
					return kKeyBindKey_Comma;
				  case VK_OEM_PERIOD://0xBE
					return kKeyBindKey_Period;
				  case VK_OEM_2://0xBF
					return kKeyBindKey_Slash;
				}
				break;
			}
			//F1-F15: ��ɏ�������
		  case VK_F1:
			return kKeyBindKey_F1;
		  case VK_F2:
			return kKeyBindKey_F2;
		  case VK_F3:
			return kKeyBindKey_F3;
		  case VK_F4:
			return kKeyBindKey_F4;
		  case VK_F5:
			return kKeyBindKey_F5;
		  case VK_F6:
			return kKeyBindKey_F6;
		  case VK_F7:
			return kKeyBindKey_F7;
		  case VK_F8:
			return kKeyBindKey_F8;
		  case VK_F9:
			return kKeyBindKey_F9;
		  case VK_F10:
			return kKeyBindKey_F10;
		  case VK_F11:
			return kKeyBindKey_F11;
		  case VK_F12:
			return kKeyBindKey_F12;
		  case VK_F13:
			return kKeyBindKey_F13;
		  case VK_F14:
			return kKeyBindKey_F14;
		  case VK_F15:
			return kKeyBindKey_F15;
			/*���b�� �K���������߂Ȃ��̂ł�͂菈�����Ȃ��悤�ɂ���BCtrl+return��WM_CHAR���Ȃ��BCtrl+space��WM_CHAR����
			//���s�A�^�u�A�X�y�[�X�Aesc�ABS: Ctrl�ƂƂ��ɉ�����Ă���ꍇ�̂ݏ�������
		  case VK_RETURN:
		  case VK_TAB:
		  case VK_SPACE:
		  case VK_ESCAPE:
		  case VK_BACK:
			{
				AModifierKeys	modifiers = GetEventKeyModifiers(inOSKeyEvent);
				if( IsControlKeyPressed(modifiers) == false )
				{
					//�uCtrl�ƂƂ��ɉ�����Ă���ꍇ�v�ȊO�Ȃ̂ŏ������Ȃ��B
					return kKeyBindKey_Invalid;
				}
				switch(inOSKeyEvent.wParam)
				{
				  case VK_RETURN:
					return kKeyBindKey_Return;
				  case VK_TAB:
					return kKeyBindKey_Tab;
				  case VK_SPACE:
					return kKeyBindKey_Space;
				  case VK_ESCAPE:
					return kKeyBindKey_Escape;
				  case VK_BACK:
					return kKeyBindKey_BS;
				}
				break;
			}
			*/
			//�����L�[: ��ɏ�������
		  case VK_UP:
			return kKeyBindKey_Up;
		  case VK_DOWN:
			return kKeyBindKey_Down;
		  case VK_LEFT:
			return kKeyBindKey_Left;
		  case VK_RIGHT:
			return kKeyBindKey_Right;
			//home, end, pageup, pagedown, delete: ��ɏ�������
		  case VK_HOME:
			return kKeyBindKey_Home;
		  case VK_END:
			return kKeyBindKey_End;
		  case VK_PRIOR:
			return kKeyBindKey_PageUp;
		  case VK_NEXT:
			return kKeyBindKey_PageDown;
		  case VK_DELETE:
			return kKeyBindKey_DEL;
			//���s�A�^�u�A�X�y�[�X�Aesc�ABS: ��ɏ������Ȃ�
			//enter: Windows�ɂ͑��݂��Ȃ��B���b�� �΍􌟓��K�v
		  default:
			return kKeyBindKey_Invalid;
		}
	}
	else if( inOSKeyEvent.message == WM_CHAR )
	{
		//WM_CHAR�̏ꍇ
		
		//A-Z, �L������: ��������
		//F1-F15: �������Ȃ��iWM_KEYDOWN�ŏ����ρj
		//���s�A�^�u�A�X�y�[�X�Aesc�ABS: ��������
		//enter: Windows�ɂ͑��݂��Ȃ��B���b�� �΍􌟓��K�v
		//�����L�[�F �������Ȃ��iWM_KEYDOWN�ŏ����ρj
		//home, end, pageup, pagedown, delete: �������Ȃ��iWM_KEYDOWN�ŏ����ρj
		switch(inOSKeyEvent.wParam)
		{
			//A-Z, �L������: ��������
		  case 0x41:
			return kKeyBindKey_A;
		  case 0x42:
			return kKeyBindKey_B;
		  case 0x43:
			return kKeyBindKey_C;
		  case 0x44:
			return kKeyBindKey_D;
		  case 0x45:
			return kKeyBindKey_E;
		  case 0x46:
			return kKeyBindKey_F;
		  case 0x47:
			return kKeyBindKey_G;
		  case 0x48:
			return kKeyBindKey_H;
		  case 0x49:
			return kKeyBindKey_I;
		  case 0x4A:
			return kKeyBindKey_J;
		  case 0x4B:
			return kKeyBindKey_K;
		  case 0x4C:
			return kKeyBindKey_L;
		  case 0x4D:
			return kKeyBindKey_M;
		  case 0x4E:
			return kKeyBindKey_N;
		  case 0x4F:
			return kKeyBindKey_O;
		  case 0x50:
			return kKeyBindKey_P;
		  case 0x51:
			return kKeyBindKey_Q;
		  case 0x52:
			return kKeyBindKey_R;
		  case 0x53:
			return kKeyBindKey_S;
		  case 0x54:
			return kKeyBindKey_T;
		  case 0x55:
			return kKeyBindKey_U;
		  case 0x56:
			return kKeyBindKey_V;
		  case 0x57:
			return kKeyBindKey_W;
		  case 0x58:
			return kKeyBindKey_X;
		  case 0x59:
			return kKeyBindKey_Y;
		  case 0x5A:
			return kKeyBindKey_Z;
		  case VK_OEM_1://0xBA
			return kKeyBindKey_Colon;
		  case VK_OEM_PLUS://0xBB
			return kKeyBindKey_Semicolon;
		  case VK_OEM_MINUS://0xBD
			return kKeyBindKey_Minus;
		  case VK_OEM_7://0xDE
			return kKeyBindKey_Hat;
		  case VK_OEM_5://0xDC
			return kKeyBindKey_Backslash;
		  case VK_OEM_3://0xC0
			return kKeyBindKey_Atmark;
		  case VK_OEM_4://0xDB
			return kKeyBindKey_BracketStart;
		  case VK_OEM_6://0xDD
			return kKeyBindKey_BracketEnd;
		  case VK_OEM_COMMA://0xBC
			return kKeyBindKey_Comma;
		  case VK_OEM_PERIOD://0xBE
			return kKeyBindKey_Period;
		  case VK_OEM_2://0xBF
			return kKeyBindKey_Slash;
			//���s�A�^�u�A�X�y�[�X�Aesc�ABS: ��������
			//enter: Windows�ɂ͑��݂��Ȃ��B���b�� �΍􌟓��K�v
		  case VK_RETURN:
			return kKeyBindKey_Return;
		  case VK_TAB:
			return kKeyBindKey_Tab;
		  case VK_SPACE:
			return kKeyBindKey_Space;
		  case VK_ESCAPE:
			return kKeyBindKey_Escape;
		  case VK_BACK:
			return kKeyBindKey_BS;
		  default:
			return kKeyBindKey_Invalid;
		}
	}
	
	return kKeyBindKey_Invalid;
}
