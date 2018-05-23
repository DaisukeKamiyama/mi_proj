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

#pragma once

#include "ABase.h"
#import <Cocoa/Cocoa.h>

#pragma mark ===========================
#pragma mark [クラス]ACocoa
class ACocoa
{
  public:
	/**
	idからNSMenuItem*へ変換
	*/
	static NSMenuItem*	CastNSMenuItemFromId( const id inId )
	{
		//クラスタイプチェック
		if( [inId isKindOfClass:[NSMenuItem class]] == NO )
		{
			_ACThrow("Class error",NULL);
		}
		//変換
		return static_cast<NSMenuItem*>(inId);
	}
	/**
	idからNSView*へ変換
	*/
	static NSView*	CastNSViewFromId( const id inId )
	{
		//クラスタイプチェック
		if( [inId isKindOfClass:[NSView class]] == NO )
		{
			_ACThrow("Class error",NULL);
		}
		//変換
		return static_cast<NSView*>(inId);
	}
	/**
	idからNSControl*へ変換
	*/
	static NSControl*	CastNSControlFromId( const id inId )
	{
		//クラスタイプチェック
		if( [inId isKindOfClass:[NSControl class]] == NO )
		{
			_ACThrow("Class error",NULL);
		}
		//変換
		return static_cast<NSControl*>(inId);
	}
	/**
	idからNSTextField*へ変換
	*/
	static NSTextField*	CastNSTextFieldFromId( const id inId )
	{
		//クラスタイプチェック
		if( [inId isKindOfClass:[NSTextField class]] == NO )
		{
			_ACThrow("Class error",NULL);
		}
		//変換
		return static_cast<NSTextField*>(inId);
	}
	/**
	idからNSButton*へ変換
	*/
	static NSButton*	CastNSButtonFromId( const id inId )
	{
		//クラスタイプチェック
		if( [inId isKindOfClass:[NSButton class]] == NO )
		{
			_ACThrow("Class error",NULL);
		}
		//変換
		return static_cast<NSButton*>(inId);
	}
	/**
	idからNSPopUpButton*へ変換
	*/
	static NSPopUpButton*	CastNSPopUpButtonFromId( const id inId )
	{
		//クラスタイプチェック
		if( [inId isKindOfClass:[NSPopUpButton class]] == NO )
		{
			_ACThrow("Class error",NULL);
		}
		//変換
		return static_cast<NSPopUpButton*>(inId);
	}
	/**
	idからNSScroller*へ変換
	*/
	static NSScroller*	CastNSScrollerFromId( const id inId )
	{
		//クラスタイプチェック
		if( [inId isKindOfClass:[NSScroller class]] == NO )
		{
			_ACThrow("Class error",NULL);
		}
		//変換
		return static_cast<NSScroller*>(inId);
	}
	/**
	idからCocoaFontField*へ変換
	*/
	/*
	static CocoaFontField*	CastCocoaFontFieldFromId( const id inId )
	{
		//クラスタイプチェック
		if( [inId isKindOfClass:[CocoaFontField class]] == NO )
		{
			_ACThrow("Class error",NULL);
		}
		//変換
		return static_cast<CocoaFontField*>(inId);
	}
	*/
	/**
	AMenuRef->NSMenu*キャスト
	*/
	static NSMenu*	GetNSMenu( const AMenuRef inMenuRef )
	{
		//ポインタNULLチェック
		if( inMenuRef == NULL )
		{
			_ACThrow("AMenuRef is NULL",NULL);
		}
		//idへキャスト
		id	menu = static_cast<id>(inMenuRef);
		//クラスタイプチェック
		if( [menu isKindOfClass:[NSMenu class]] == NO )
		{
			_ACThrow("Class error",NULL);
		}
		//NSMenu*へキャスト
		return static_cast<NSMenu*>(inMenuRef);
	}
	
	/**
	NSMenuItem取得（indexチェック付き）
	*/
	static NSMenuItem*	GetNSMenuItem( const NSMenu* inMenu, const AIndex inIndex )
	{
		//indexチェック
		if( inIndex < 0 || inIndex >= [inMenu numberOfItems] )
		{
			_ACThrow("GetNSMenuItem() index error",NULL);
		}
		//NSMenuItem取得
		return [inMenu itemAtIndex:inIndex];
	}
	
	/**
	NSStringからAText取得
	*/
	static void	SetTextFromNSString( const NSString* inString, AText& outText )
	{
		outText.SetFromCFString(reinterpret_cast<CFStringRef>(inString));
	}
	/**
	NSEventのmodifierFlagsをAModifierKeys形式へ変換
	*/
	static AModifierKeys	ConvertToAModifierKeysFromNSEvent( const NSEvent* inEvent );
	/**
	NSEventのcharactersIgnoringModifiersの最初の文字からキーバインドキーを取得
	*/
	static AKeyBindKey	GetKeyBindKeyFromNSEvent( const NSEvent* inEvent, const AText& inText );
	/*
	ScrapTypeからPasteboardTypeNSStringを取得
	*/
	static NSString*	GetPasteboardTypeNSString( const AScrapType inScrapType );
	/**
	Menuのaction/targetを初期化（標準メニュー以外は、target:[NSApp delegate], action:doMenuActions）
	*/
	static void	InitMenuActionAndTarget( NSMenu* inMenu );
	/**
	画面描画を一時的に有効・無効にする（高速化用）
	*/
	static void	DisableScreenUpdates()
	{
		NSDisableScreenUpdates();
	}
	static void EnableScreenUpdates()
	{
		NSEnableScreenUpdates();
	}
};

#pragma mark ===========================
#pragma mark [クラス]AStCreateNSStringFromAText
//ATextからNSStringを取得するためのクラス(Mac専用)
//（スタック生成専用クラス）
class AStCreateNSStringFromAText : public AStackOnlyObject
{
	//コンストラクタ／デストラクタ
  public:
	AStCreateNSStringFromAText( const AText& inText );
	~AStCreateNSStringFromAText();
	
	//NSStringRef取得
	NSString*	GetNSString();//#1012 const NSString*→NSString*
	
  private:
	NSString*	mNSString;
};

#pragma mark ===========================
#pragma mark [クラス]AStCreateNSStringFromUTF16Text
//ATextからNSStringを取得するためのクラス(Mac専用)
//（スタック生成専用クラス）
class AStCreateNSStringFromUTF16Text : public AStackOnlyObject
{
	//コンストラクタ／デストラクタ
  public:
	AStCreateNSStringFromUTF16Text( const AText& inText );
	~AStCreateNSStringFromUTF16Text();
	
	//NSStringRef取得
	NSString*	GetNSString();
	
  private:
	NSString*	mNSString;
};

