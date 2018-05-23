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

Cocoa

Cocoa用サブルーチン
cppのファイルにはCocoaのコードはかけないので、ここに書く。
Cocoa.hはcppから直接includeする（ヘッダファイルからincludeしない）こと。（NSEvent*等はcppのヘッダでは使えない）

*/

#include "Cocoa.h"

/**
NSEventのmodifierFlagsをAModifierKeys形式へ変換
*/
AModifierKeys	ACocoa::ConvertToAModifierKeysFromNSEvent( const NSEvent* inEvent )
{
	NSUInteger	modifier = [inEvent modifierFlags];
	AModifierKeys	result = 0;
	if( (modifier&NSShiftKeyMask) != 0 )
	{
		result |= kModifierKeysMask_Shift;
	}
	if( (modifier&NSControlKeyMask) != 0 )
	{
		result |= kModifierKeysMask_Control;
	}
	if( (modifier&NSAlternateKeyMask) != 0 )
	{
		result |= kModifierKeysMask_Option;
	}
	if( (modifier&NSCommandKeyMask) != 0 )
	{
		result |= kModifierKeysMask_Command;
	}
	return result;
}

/**
NSEventのcharactersIgnoringModifiersの最初の文字からキーバインドキーを取得
*/
AKeyBindKey	ACocoa::GetKeyBindKeyFromNSEvent( const NSEvent* inEvent, const AText& inText )//#942
{
	//文字パレットからの文字入力の場合、キーイベントでないので、charactersIgnoringModifiersがエラーをおこしてしまうので、ここでリターン
	if( [inEvent type] != NSKeyDown )
	{
		return kKeyBindKey_Invalid;
	}
	//念のため、レングスチェックエラーならここでリターン（下でcharacterAtIndex:0でアクセスするので）
	if( [[inEvent charactersIgnoringModifiers] length] == 0 )
	{
		return kKeyBindKey_Invalid;
	}
	//
	AKeyBindKey	key = kKeyBindKey_Invalid;
	unichar	keyChar = [[inEvent charactersIgnoringModifiers] characterAtIndex:0];
	switch(keyChar)
	{
	  case 'A':
	  case 'a':
		{
			key = kKeyBindKey_A;
			break;
		}
	  case 'B':
	  case 'b':
		{
			key = kKeyBindKey_B;
			break;
		}
	  case 'C':
	  case 'c':
		{
			key = kKeyBindKey_C;
			break;
		}
	  case 'D':
	  case 'd':
		{
			key = kKeyBindKey_D;
			break;
		}
	  case 'E':
	  case 'e':
		{
			key = kKeyBindKey_E;
			break;
		}
	  case 'F':
	  case 'f':
		{
			key = kKeyBindKey_F;
			break;
		}
	  case 'G':
	  case 'g':
		{
			key = kKeyBindKey_G;
			break;
		}
	  case 'H':
	  case 'h':
		{
			key = kKeyBindKey_H;
			break;
		}
	  case 'I':
	  case 'i':
		{
			key = kKeyBindKey_I;
			break;
		}
	  case 'J':
	  case 'j':
		{
			key = kKeyBindKey_J;
			break;
		}
	  case 'K':
	  case 'k':
		{
			key = kKeyBindKey_K;
			break;
		}
	  case 'L':
	  case 'l':
		{
			key = kKeyBindKey_L;
			break;
		}
	  case 'M':
	  case 'm':
		{
			key = kKeyBindKey_M;
			break;
		}
	  case 'N':
	  case 'n':
		{
			key = kKeyBindKey_N;
			break;
		}
	  case 'O':
	  case 'o':
		{
			key = kKeyBindKey_O;
			break;
		}
	  case 'P':
	  case 'p':
		{
			key = kKeyBindKey_P;
			break;
		}
	  case 'Q':
	  case 'q':
		{
			key = kKeyBindKey_Q;
			break;
		}
	  case 'R':
	  case 'r':
		{
			key = kKeyBindKey_R;
			break;
		}
	  case 'S':
	  case 's':
		{
			key = kKeyBindKey_S;
			break;
		}
	  case 'T':
	  case 't':
		{
			key = kKeyBindKey_T;
			break;
		}
	  case 'U':
	  case 'u':
		{
			key = kKeyBindKey_U;
			break;
		}
	  case 'V':
	  case 'v':
		{
			key = kKeyBindKey_V;
			break;
		}
	  case 'W':
	  case 'w':
		{
			key = kKeyBindKey_W;
			break;
		}
	  case 'X':
	  case 'x':
		{
			key = kKeyBindKey_X;
			break;
		}
	  case 'Y':
	  case 'y':
		{
			key = kKeyBindKey_Y;
			break;
		}
	  case 'Z':
	  case 'z':
		{
			key = kKeyBindKey_Z;
			break;
		}
	  case NSF1FunctionKey:
		{
			key = kKeyBindKey_F1;
			break;
		}
	  case NSF2FunctionKey:
		{
			key = kKeyBindKey_F2;
			break;
		}
	  case NSF3FunctionKey:
		{
			key = kKeyBindKey_F3;
			break;
		}
	  case NSF4FunctionKey:
		{
			key = kKeyBindKey_F4;
			break;
		}
	  case NSF5FunctionKey:
		{
			key = kKeyBindKey_F5;
			break;
		}
	  case NSF6FunctionKey:
		{
			key = kKeyBindKey_F6;
			break;
		}
	  case NSF7FunctionKey:
		{
			key = kKeyBindKey_F7;
			break;
		}
	  case NSF8FunctionKey:
		{
			key = kKeyBindKey_F8;
			break;
		}
	  case NSF9FunctionKey:
		{
			key = kKeyBindKey_F9;
			break;
		}
	  case NSF10FunctionKey:
		{
			key = kKeyBindKey_F10;
			break;
		}
	  case NSF11FunctionKey:
		{
			key = kKeyBindKey_F11;
			break;
		}
	  case NSF12FunctionKey:
		{
			key = kKeyBindKey_F12;
			break;
		}
	  case NSF13FunctionKey:
		{
			key = kKeyBindKey_F13;
			break;
		}
	  case NSF14FunctionKey:
		{
			key = kKeyBindKey_F14;
			break;
		}
	  case NSF15FunctionKey:
		{
			key = kKeyBindKey_F15;
			break;
		}
	  case 0x000D:
		{
			key = kKeyBindKey_Return;
			break;
		}
	  case 0x0009:
	  case 0x0019:
		{
			key = kKeyBindKey_Tab;
			break;
		}
	  case 0x0020:
		{
			//テキストが全角スペース以外の場合のみspaceキーとしてキーバインド可能とする。（#942）
			if( inText.CompareZenkakuSpace() == false )
			{
				key = kKeyBindKey_Space;
			}
			break;
		}
	  case 0x001B:
		{
			key = kKeyBindKey_Escape;
			break;
		}
	  case 0x0003:
		{
			key = kKeyBindKey_Enter;
			break;
		}
	  case 0x0008:
	  case 0x007F:
		{
			key = kKeyBindKey_BS;
			break;
		}
	  case NSDeleteFunctionKey:
		{
			key = kKeyBindKey_DEL;
			break;
		}
	  case NSUpArrowFunctionKey:
		{
			key = kKeyBindKey_Up;
			break;
		}
	  case NSDownArrowFunctionKey:
		{
			key = kKeyBindKey_Down;
			break;
		}
	  case NSLeftArrowFunctionKey:
		{
			key = kKeyBindKey_Left;
			break;
		}
	  case NSRightArrowFunctionKey:
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
	  case 0x00A5://半角￥（日本語キーボードの￥はこのコードで来ることを確認済み）
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
	  case NSHomeFunctionKey:
		{
			key = kKeyBindKey_Home;
			break;
		}
	  case NSEndFunctionKey:
		{
			key = kKeyBindKey_End;
			break;
		}
	  case NSPageUpFunctionKey:
		{
			key = kKeyBindKey_PageUp;
			break;
		}
	  case NSPageDownFunctionKey:
		{
			key = kKeyBindKey_PageDown;
			break;
		}
		//#915
	  case '1':
		{
			key = kKeyBindKey_1;
			break;
		}
	  case '2':
		{
			key = kKeyBindKey_2;
			break;
		}
	  case '3':
		{
			key = kKeyBindKey_3;
			break;
		}
	  case '4':
		{
			key = kKeyBindKey_4;
			break;
		}
	  case '5':
		{
			key = kKeyBindKey_5;
			break;
		}
	  case '6':
		{
			key = kKeyBindKey_6;
			break;
		}
	  case '7':
		{
			key = kKeyBindKey_7;
			break;
		}
	  case '8':
		{
			key = kKeyBindKey_8;
			break;
		}
	  case '9':
		{
			key = kKeyBindKey_9;
			break;
		}
	  case '0':
		{
			key = kKeyBindKey_0;
			break;
		}
	}
	//#1129
	//Mac OS X 10.10?にて、日本語入力中に英数キーダブルクリックすると、スペース入力になってしまう問題対策
	//（なぜかキーがスペースでくるのでそちらが優先されてしまう。キーがスペースでも入力テキストが2文字以上あるときは入力テキストを優先することにする。）
	if( key == kKeyBindKey_Space && inText.GetItemCount() > 1 )
	{
		key = kKeyBindKey_Invalid;
	}
	return key;
}

/**
ScrapTypeからPasteboardTypeNSStringを取得
*/
NSString*	ACocoa::GetPasteboardTypeNSString( const AScrapType inScrapType )
{
	NSString*	typestr = nil;
	switch(inScrapType)
	{
	  case kScrapType_UnicodeText:
		{
			typestr = NSStringPboardType;
			break;
		}
	  case kScrapType_File:
		{
			typestr = NSFilenamesPboardType;
			break;
		}
	  case kScrapType_URL:
		{
			typestr = @"WebURLsWithTitlesPboardType";
			break;
		}
	  default:
		{
			AText	text;
			text.SetNumber(inScrapType);
			AStCreateNSStringFromAText	str(text);
			typestr = [[str.GetNSString() retain] autorelease];
			break;
		}
	}
	return typestr;
}

/**
内部イベントpost
*/
void	PostInternalEvent()
{
	NSEvent*	event = [NSEvent otherEventWithType:NSApplicationDefined location:NSMakePoint(0,0) modifierFlags:0 
				timestamp:0 windowNumber:0 context:nil subtype:kNSEvent_InternalEvent data1:nil data2:nil];
	[NSApp postEvent:event atStart:NO];
}

/**
AutoreleasePool生成（スレッド用）
*/
void*	AllocAutoreleasePool()
{
	return [[NSAutoreleasePool alloc] init];
}

/**
AutoreleasePool解放（スレッド用）
*/
void	ReleaseAutoreleasePool( void* inPool )
{
	[inPool release];
}

/**
Menuのaction/targetを初期化（標準メニュー以外は、target:[NSApp delegate], action:doMenuActions）
*/
void	ACocoa::InitMenuActionAndTarget( NSMenu* inMenu )
{
	AItemCount	menuItemCount = [inMenu numberOfItems];
	for( AIndex index = 0; index < menuItemCount; index++ )
	{
		NSMenuItem*	menuItem = [inMenu itemAtIndex:index];
		//標準メニューのターゲットをFirst Responderに設定し、各標準処理を割り当て
		//NSTextFieldでの標準テキスト処理を行うため。
		//なお、CocoaUserPaneViewがFirst Responderの場合、validateMenuItem:はResponder Chainに従って、
		//AppDelegateまでさかのぼるはずだが、undo:についてはどこかで定義されているためか、AppDelegateのvalidateMenuItem:まで来ない。
		//そのため、CocoaUserPaneView等にて、AppDelegateへactionとvalidateMenuItemをリダイレクトすることにした。
		switch([menuItem tag])
		{
		  case kMenuItemID_Undo:
			{
				[menuItem setTarget:nil];
				[menuItem setAction:@selector(undo:)];
				break;
			}
		  case kMenuItemID_Redo:
			{
				[menuItem setTarget:nil];
				[menuItem setAction:@selector(redo:)];
				break;
			}
		  case kMenuItemID_Copy:
			{
				[menuItem setTarget:nil];
				[menuItem setAction:@selector(copy:)];
				break;
			}
		  case kMenuItemID_Cut:
			{
				[menuItem setTarget:nil];
				[menuItem setAction:@selector(cut:)];
				break;
			}
		  case kMenuItemID_Paste:
			{
				[menuItem setTarget:nil];
				[menuItem setAction:@selector(paste:)];
				break;
			}
		  case kMenuItemID_SelectAll:
			{
				[menuItem setTarget:nil];
				[menuItem setAction:@selector(selectAll:)];
				break;
			}
		  case kMenuItemID_Clear:
			{
				[menuItem setTarget:nil];
				[menuItem setAction:@selector(delete:)];
				break;
			}
			//標準メニュー以外はAppDelegateのdoMenuActions:で処理する
		  default:
			{
				[menuItem setTarget:[NSApp delegate]];
				[menuItem setAction:@selector(doMenuActions:)];
				break;
			}
		}
		//サブメニューがあれば再帰コール
		if( [menuItem hasSubmenu] == YES )
		{
			InitMenuActionAndTarget([menuItem submenu]);
		}
	}
}

/**
system font取得
*/
void	GetCocoaSystemFontName( AText& outFontName )
{
	ACocoa::SetTextFromNSString([[NSFont systemFontOfSize:12.0] fontName],outFontName);
}

#pragma mark ===========================
#pragma mark [クラス]AStCreateNSStringFromAText
#pragma mark ===========================
//ATextからNSStringを取得するためのクラス(Mac専用)
//（スタック生成専用クラス）
//（AStCreateCFStringFromATextからコピーして作成）

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AStCreateNSStringFromAText::AStCreateNSStringFromAText( const AText& inText )
{
	//NSString取得
	{
		AStTextPtr	textptr(inText,0,inText.GetItemCount());
		mNSString = [[NSString alloc] initWithBytes:textptr.GetPtr() length:textptr.GetByteCount() encoding:NSUTF8StringEncoding];
	}
	if( mNSString == NULL )
	{
		//正しいUTF8コードでなかった場合等、CFString生成不可となるので、文字列長0のCFStringを生成して返す
		//_ACError("cannot create NSString",NULL);
		mNSString = [[NSString alloc] init];
		//文字列長0のCFStringすら生成できない場合（ふつうありえない）throwしとく
		if( mNSString == NULL )   _ACThrow("cannot create NSString with null string",NULL);
	}
}

/**
デストラクタ
*/
AStCreateNSStringFromAText::~AStCreateNSStringFromAText()
{
	//CFString解放
	if( mNSString != NULL )
	{
		[mNSString release];
	}
}

/**
NSString*取得
*/
NSString*	AStCreateNSStringFromAText::GetNSString()//#1012 const NSString*→NSString*
{
	return mNSString;
}

#pragma mark ===========================
#pragma mark [クラス]AStCreateNSStringFromUTF16Text
#pragma mark ===========================
//ATextからNSStringを取得するためのクラス(Mac専用)
//（スタック生成専用クラス）
//（AStCreateCFStringFromATextからコピーして作成）

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AStCreateNSStringFromUTF16Text::AStCreateNSStringFromUTF16Text( const AText& inText )
{
	//NSString取得
	{
		AStTextPtr	textptr(inText,0,inText.GetItemCount());
		mNSString = [[NSString alloc] initWithBytes:textptr.GetPtr() length:textptr.GetByteCount() encoding:NSUTF16LittleEndianStringEncoding];
	}
	if( mNSString == NULL )
	{
		//正しいUTF8コードでなかった場合等、CFString生成不可となるので、文字列長0のCFStringを生成して返す
		//_ACError("cannot create NSString",NULL);
		mNSString = [[NSString alloc] init];
		//文字列長0のCFStringすら生成できない場合（ふつうありえない）throwしとく
		if( mNSString == NULL )   _ACThrow("cannot create NSString with null string",NULL);
	}
}

/**
デストラクタ
*/
AStCreateNSStringFromUTF16Text::~AStCreateNSStringFromUTF16Text()
{
	//CFString解放
	if( mNSString != NULL )
	{
		[mNSString release];
	}
}

/**
NSString*取得
*/
NSString*	AStCreateNSStringFromUTF16Text::GetNSString()//#1012 const NSString*→NSString*
{
	return mNSString;
}

