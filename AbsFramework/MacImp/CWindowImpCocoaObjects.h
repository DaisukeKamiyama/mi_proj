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

	CWindowImpCocoaObjects

*/

#pragma once

#include "CWindowImp.h"
#include "../Frame.h"

#import <Cocoa/Cocoa.h>
#import "CocoaUserPaneView.h"
#import "CocoaWindow.h"
#import "CocoaPanelWindow.h"
#import "CocoaWindowDelegate.h"
#import "CocoaTextField.h"
#import "CocoaLiveResizeDetectionView.h"
#import "CocoaCaretOverlayWindow.h"
#include "../AbsBase/Cocoa.h"

#pragma mark ===========================
#pragma mark [クラス]CWindowImpCocoaObjects
#pragma mark ===========================

//#688
/**
CWindowImp内のCocoa関連データを格納するクラス
CWindowImp.hはC++ファイルからもincludeされるので、ObjCのデータはこちらに定義している。
*/
class CWindowImpCocoaObjects
{
	//コンストラクタ／デストラクタ
  public:
	CWindowImpCocoaObjects( CWindowImp& inWindowImp );
	~CWindowImpCocoaObjects();
  private:
	CWindowImp&							mWindowImp;
	
	//データ
  private:
	NSWindow*							mWindow;
	CocoaWindowDelegate*				mWindowDelegate;
	NSWindowController*					mWindowController;
	NSWindow*							mParentWindow;
	AArray<NSProgressIndicator*>		mProgressIndicatorArray;
	AHashArray<AControlID>				mCocoaUserPaneViewArray_ControlID;
	AArray<CocoaUserPaneView*>			mCocoaUserPaneViewArray_UserPaneView;
	/*
	AArray<CocoaCaretOverlayWindow*>	mCaretOverlayWindowArray;
	AArray<CocoaCaretOverlayView*>		mCaretOverlayViewArray;
	*/
	
	//ウインドウ生成／削除
  public:
	void					CreateWindow( const AText& inIBName );
	void					CreateWindow( const AWindowClass inWindowClass, NSWindow* inParentWindow, 
							const AOverlayWindowLayer inOverlayWindowLayer,
							const ABool inHasCloseButton, const ABool inHasCollapseButton, 
							const ABool inHasZoomButton, const ABool inHasGrowBox,
							const ABool inIgnoreClick, const ABool inHasShadow, const ABool inHasTitlebar,
							const ABool inNonFloating );//#1133
	void					DeleteWindow();
	void					CreateToolbar( /*#1012 const*/ NSString* inString, const ABool inForPref );
	ABool					IsCreatingToolbar() const { return mCreatingToolbar; }
	void					SetupFloatingWindow( const ABool inNonFloating, const ABool inIgnoreClick );//#1133
  private:
	//void					CreateCaretOverlayWindow( const AIndex inCaretWindowIndex );
	ABool								mCreatingToolbar;
	
	//
  public:
	void					SetParentWindow( NSWindow* inParentWindow );
	void					ClearParentWindow();
	
	//#846
  public:
	void					StartModalSession();
	ABool					CheckContinueingModalSession();
	void					EndModalSession();
	void					RunModalWindow();
	void					StopModalSession();
  private:
	NSModalSession						mModalSession;
	
	//コントロール登録／解除
  public:
	void					RegisterUserPaneView( const AControlID inControlID, CocoaUserPaneView* inUserPane );
	void					UnregisterUserPaneView( const AControlID inControlID );
	
	//Objectへのアクセス
  public:
	NSWindow*				GetWindow() const { return mWindow; }
	WindowRef				GetWindowRef() const { return static_cast<WindowRef>([mWindow windowRef]); }
	NSView*					GetContentView() const { return [mWindow contentView]; }
	NSWindow*				GetParentWindow() const { return mParentWindow; }
	//CocoaCaretOverlayWindow*	GetCaretOverlayWindow( const AIndex inCaretWindowIndex, const ABool inCreateIfNotCreated = true );
	//CocoaCaretOverlayView*	GetCaretOverlayView( const AIndex inCaretWindowIndex ) { return mCaretOverlayViewArray.Get(inCaretWindowIndex); }
	
	//コントロールID(=tag)/id変換
  public:
	NSView*					FindViewByTag( const AControlID inControlID, const ABool inThrowIfNotFound = true ) const;
	CocoaUserPaneView*		FindCocoaUserPaneViewByTag( const AControlID inControlID ) const;
	NSProgressIndicator*	FindProgressIndicatorByIndex( const AIndex inProgressIndex ) const;
	NSToolbarItem*			FindToolbarItemByTag( const AControlID inControlID ) const;
	void					AddToControlIDCache( const AControlID inControlID, NSView* inView ) const ;
	void					RemoveFromControlIDCache( const AControlID inControlID ) const;
	mutable AHashArray<AControlID>				mControlIDCacheArray_ControlID;
	mutable AArray<NSView*>						mControlIDCacheArray_NSView;
	
	//#1034
	//AppleScript用オブジェクト
  public:
	CASWindow*				GetCASWindow() const { return mCASWindow; }
  private:
	CASWindow*							mCASWindow;
};


/**
CUserPane内のCocoa関連データを格納するクラス
CWindowImp.hはC++ファイルからもincludeされるので、ObjCのデータはこちらに定義している。
*/
class CUserPaneCocoaObjects
{
  public:
	CUserPaneCocoaObjects();
	~CUserPaneCocoaObjects();
	
  public:
	void					SetNormalFont( NSFont* inFont ) { mNormalFont = inFont; [mNormalFont retain]; }
	void					SetBoldFont( NSFont* inFont ) { mBoldFont = inFont; [mBoldFont retain]; }
	void					SetItalicFont( NSFont* inFont ) { mItalicFont = inFont; [mItalicFont retain]; }
	void					SetBoldItalicFont( NSFont* inFont ) { mBoldItalicFont = inFont; [mBoldItalicFont retain]; }
	NSFont*					GetNormalFont() const { return mNormalFont; }
	NSFont*					GetBoldFont() const { return mBoldFont; }
	NSFont*					GetItalicFont() const { return mItalicFont; }
	NSFont*					GetBoldItalicFont() const { return mBoldItalicFont; }
	
  private:
	NSFont*								mNormalFont;
	NSFont*								mBoldFont;
	NSFont*								mItalicFont;
	NSFont*								mBoldItalicFont;
};


