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

#include "CWindowImpCocoaObjects.h"

#pragma mark ===========================

#pragma mark ---コンストラクタ

/**
コンストラクタ
*/
CWindowImpCocoaObjects::CWindowImpCocoaObjects( CWindowImp& inWindowImp ) : mWindowImp(inWindowImp),
		mWindow(nil),mWindowDelegate(nil),mWindowController(nil),mParentWindow(nil),mModalSession(nil)
,mCreatingToolbar(false),mCASWindow(nil)
{
}

/**
デストラクタ
*/
CWindowImpCocoaObjects::~CWindowImpCocoaObjects()
{
	//ウインドウ削除
	DeleteWindow();
}

#pragma mark ===========================

#pragma mark ---ウインドウ生成／削除

/**
ウインドウ生成（InterfaceBuilderで定義済みのウインドウ）
*/
void	CWindowImpCocoaObjects::CreateWindow( const AText& inIBName )
{
	AStCreateNSStringFromAText	winNameRef(inIBName);
	//Window ControllerをIBから生成
	mWindowController = [[NSWindowController alloc] initWithWindowNibName:winNameRef.GetNSString()];
	//ウインドウへのポインタ取得
	mWindow = [mWindowController window];
	//ポインタ保持するのでretainカウンタインクリメント。
	[mWindow retain];
	//ウインドウhide
	[mWindow orderOut:nil];
	//ウインドウタイプによってそれぞれ処理
	if( [mWindow isKindOfClass:[CocoaWindow class]] == YES )
	{
		//通常ウインドウ
		CocoaWindow*	win = reinterpret_cast<CocoaWindow*>(mWindow);
		//初期設定
		[win setup:(&mWindowImp)];
		//ウインドウhide（設計上、最初の状態をhide状態で固定しておきたいため）
		[win orderOut:nil];
		//LiveResizeの完了を検知するためのダミーviewを生成。ウインドウへ登録。
		//★addSubviewしたビューはウインドウ削除時にreleaseされるかどうか確認必要
		[GetContentView() addSubview:[[[CocoaLiveResizeDetectionView alloc] initWithWindowImp:(&mWindowImp)] autorelease] 
		positioned:NSWindowAbove relativeTo:nil];
		//Delegate設定
		mWindowDelegate = [[CocoaWindowDelegate alloc] initWithWindowImp:(&mWindowImp)];
		[win setDelegate:mWindowDelegate];
		//ウインドウレベル設定 #1034
		[win setLevel:NSNormalWindowLevel];
		//ウインドウdeactivate時、hideしない
		[win setHidesOnDeactivate:NO];
		//ウインドウはMainWindow, KeyWindowになれる
		[win setCanBecomeMainWindow:YES];
		[win setCanBecomeKeyWindow:YES];
		//マウスイベント無視しない
		[win setIgnoresMouseEvents:NO];
		//IBに設定されたProgressIndicatorを取得し、mProgressIndicatorArrayへ記憶
		NSProgressIndicator*	progress = [win getProgressIndicator];
		if( progress != nil )
		{
			mProgressIndicatorArray.Add(progress);
			[progress retain];
		}
		//#330
		//https://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/WinPanel/Articles/SettingWindowCollectionBehavior.html
		//IBで定義されたウインドウはSpacesのアクティブspaceに表示されるようにする
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
		{
			[win setCollectionBehavior:NSWindowCollectionBehaviorMoveToActiveSpace];
		}
		//key viewループを自動計算するようにする
		[win setAutorecalculatesKeyViewLoop:YES];
	}
	else if( [mWindow isKindOfClass:[CocoaPanelWindow class]] == YES )
	{
		//Panel window（フローティングウインドウ）
		CocoaPanelWindow*	win = reinterpret_cast<CocoaPanelWindow*>(mWindow);
		//初期設定
		[win setup:(&mWindowImp)];
		//ウインドウhide（設計上、最初の状態をhide状態で固定しておきたいため）
		[win orderOut:nil];
		//LiveResizeの完了を検知するためのダミーviewを生成。ウインドウへ登録。
		//★addSubviewしたビューはウインドウ削除時にreleaseされるかどうか確認必要
		[GetContentView() addSubview:[[[CocoaLiveResizeDetectionView alloc] initWithWindowImp:(&mWindowImp)] autorelease] 
		positioned:NSWindowAbove relativeTo:nil];
		//Delegate設定
		mWindowDelegate = [[CocoaWindowDelegate alloc] initWithWindowImp:(&mWindowImp)];
		[win setDelegate:mWindowDelegate];
		//ウインドウレベル設定 #1034
		[win setLevel:NSFloatingWindowLevel];
		//ウインドウdeactivate時、hideしない（当面、hideしないようにする。起動時のモーダルプログレスがhideされないようにするため。）
		[win setHidesOnDeactivate:NO];//YES];
		//ウインドウはMainWindow, KeyWindowになれない
		[win setCanBecomeMainWindow:NO];
		[win setCanBecomeKeyWindow:YES];
		//マウスイベント無視しない
		[win setIgnoresMouseEvents:NO];
		//フローティングパネル設定
		[win setFloatingPanel:YES];
		//IBに設定されたProgressIndicatorを取得し、mProgressIndicatorArrayへ記憶
		NSProgressIndicator*	progress = [win getProgressIndicator];
		if( progress != nil )
		{
			mProgressIndicatorArray.Add(progress);
			[progress retain];
		}
		//#330
		//https://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/WinPanel/Articles/SettingWindowCollectionBehavior.html
		//IBで定義されたウインドウはSpacesのアクティブspaceに表示されるようにする
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
		{
			[win setCollectionBehavior:NSWindowCollectionBehaviorMoveToActiveSpace];
		}
		//key viewループを自動計算するようにする
		[win setAutorecalculatesKeyViewLoop:YES];
	}
	else
	{
		_ACError("",NULL);
	}
	//AppleScript用オブジェクト生成 #1034
	mCASWindow = [[CASWindow alloc] init:mWindowImp.GetAWin().GetObjectID()];
}

/**
ウインドウ生成（InterfaceBuilderで定義されていないウインドウ）
*/
void	CWindowImpCocoaObjects::CreateWindow( const AWindowClass inWindowClass, NSWindow* inParentWindow,
		const AOverlayWindowLayer inOverlayWindowLayer,
		const ABool inHasCloseButton, const ABool inHasCollapseButton, 
		const ABool inHasZoomButton, const ABool inHasGrowBox,
		const ABool inIgnoreClick, const ABool inHasShadow, const ABool inHasTitlebar, 
		const ABool inNonFloating )//#1133
{
	//★inHasZoomButton未使用。設定方法調査
	//
	switch(inWindowClass)
	{
	  case kWindowClass_Document:
		{
			//Window Controllerは生成しない
			mWindowController = nil;
			//ウインドウの初期frame設定
			NSRect	rect = NSMakeRect(-10000,-10000,100,100);
			//スタイル設定
			//組み合わせには制限がある。
			//例：resize boxはNSTitledWindowMask | NSUtilityWindowMaskでしか使えない。
			//https://developer.apple.com/library/mac/#documentation/Cocoa/Reference/ApplicationKit/Classes/NSPanel_Class/Reference/Reference.html
			NSUInteger	stylemask = 0;//#1316 NSTexturedBackgroundWindowMaskはobsoleteなので削除。 NSTexturedBackgroundWindowMask;
			if( inHasTitlebar == true )
			{
				stylemask |= NSTitledWindowMask;
			}
			if( inHasCloseButton == true )
			{
				stylemask |= NSClosableWindowMask;
			}
			if( inHasCollapseButton == true )
			{
				stylemask |= NSMiniaturizableWindowMask;
			}
			//if( inHasGrowBox == true )
			{
				stylemask |= NSResizableWindowMask;
			}
			//ウインドウ生成
			CocoaPanelWindow*	win = (CocoaPanelWindow*)[[CocoaWindow alloc] initWithContentRect:rect styleMask:stylemask 
								backing:NSBackingStoreBuffered //buffered（Mac OS Xの標準）
								defer:NO];//defer:NOにする。YESにすると、mousemovedが動作しない。
			mWindow = win;
			//初期設定
			[win setup:(&mWindowImp)];
			//オーバーレイウインドウレイヤー設定
			[win setOverlayWindowLayer:inOverlayWindowLayer];
			//ウインドウhide
			[win orderOut:nil];
			//LiveResizeの完了を検知するためのダミーviewを生成。ウインドウへ登録。
			//★addSubviewしたビューはウインドウ削除時にreleaseされるかどうか確認必要
			[GetContentView() addSubview:[[[CocoaLiveResizeDetectionView alloc] initWithWindowImp:(&mWindowImp)] autorelease] 
			positioned:NSWindowAbove relativeTo:nil];
			//Delegate設定
			mWindowDelegate = [[CocoaWindowDelegate alloc] initWithWindowImp:(&mWindowImp)];
			[win setDelegate:mWindowDelegate];
			//ウインドウレベル設定 #1034
			[win setLevel:NSNormalWindowLevel];
			//ウインドウdeactivate時、hideしない
			[win setHidesOnDeactivate:NO];
			//KeyView loop自動設定しない #1091対策テスト
			[win setAutorecalculatesKeyViewLoop:NO];
			//マウスクリック無視判定
			if( inIgnoreClick == true )
			{
				//ウインドウはMainWindow,KeyWindowになれない
				[win setCanBecomeMainWindow:NO];
				[win setCanBecomeKeyWindow:NO];
				//マウスイベント無視する
				[win setIgnoresMouseEvents:YES];
			}
			else
			{
				//ウインドウはMainWindowになれる、KeyWindowになれる
				[win setCanBecomeMainWindow:YES];
				[win setCanBecomeKeyWindow:YES];
				//マウスイベント無視しない
				[win setIgnoresMouseEvents:NO];
			}
			//シャドウ表示
			if( inHasShadow == true )
			{
				[win setHasShadow:YES];
			}
			else
			{
				[win setHasShadow:NO];
			}
			//背景色設定
			//[win setBackgroundColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.0]];
			//透明ウインドウ設定
			[win setOpaque:YES];
			//通常ドキュメントウインドウはフルスクリーン対応にする
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
			{
				[win setCollectionBehavior:(1 << 7)];
			}
			break;
		}
	  case kWindowClass_Overlay:
		{
			//Window Controllerは生成しない
			mWindowController = nil;
			//ウインドウの初期frame設定
			NSRect	rect = NSMakeRect(-10000,-10000,100,100);
			//スタイル設定
			//組み合わせには制限がある。
			//例：resize boxはNSTitledWindowMask | NSUtilityWindowMaskでしか使えない。
			//https://developer.apple.com/library/mac/#documentation/Cocoa/Reference/ApplicationKit/Classes/NSPanel_Class/Reference/Reference.html
			NSUInteger	stylemask = NSBorderlessWindowMask;
			if( inHasTitlebar == true )
			{
				stylemask |= NSTitledWindowMask | NSUtilityWindowMask;
			}
			if( inHasCloseButton == true )
			{
				stylemask |= NSClosableWindowMask;
			}
			if( inHasCollapseButton == true )
			{
				stylemask |= NSMiniaturizableWindowMask;
			}
			if( inHasGrowBox == true )
			{
				stylemask |= NSResizableWindowMask;
			}
			//ウインドウ生成
			CocoaPanelWindow*	win = [[CocoaPanelWindow alloc] initWithContentRect:rect styleMask:stylemask 
								backing:NSBackingStoreBuffered //buffered（Mac OS Xの標準）
								defer:NO];//defer:NOにする。YESにすると、mousemovedが動作しない。
			mWindow = win;
			//初期設定
			[win setup:(&mWindowImp)];
			//オーバーレイウインドウレイヤー設定
			[win setOverlayWindowLayer:inOverlayWindowLayer];
			//ウインドウhide
			[win orderOut:nil];
			//LiveResizeの完了を検知するためのダミーviewを生成。ウインドウへ登録。
			//★addSubviewしたビューはウインドウ削除時にreleaseされるかどうか確認必要
			[GetContentView() addSubview:[[[CocoaLiveResizeDetectionView alloc] initWithWindowImp:(&mWindowImp)] autorelease] 
			positioned:NSWindowAbove relativeTo:nil];
			//ウインドウレベル設定 #1034
			[win setLevel:NSNormalWindowLevel];
			//Delegate設定
			mWindowDelegate = [[CocoaWindowDelegate alloc] initWithWindowImp:(&mWindowImp)];
			[win setDelegate:mWindowDelegate];
			//ウインドウdeactivate時、hideしない
			[win setHidesOnDeactivate:NO];
			//マウスクリック無視判定
			if( inIgnoreClick == true )
			{
				//ウインドウはMainWindow,KeyWindowになれない
				[win setCanBecomeMainWindow:NO];
				[win setCanBecomeKeyWindow:NO];
				//マウスイベント無視する
				[win setIgnoresMouseEvents:YES];
			}
			else
			{
				//ウインドウはMainWindowになれない、KeyWindowになれる
				[win setCanBecomeMainWindow:NO];
				[win setCanBecomeKeyWindow:YES];
				//マウスイベント無視しない
				[win setIgnoresMouseEvents:NO];
			}
			//シャドウ表示
			if( inHasShadow == true )
			{
				[win setHasShadow:YES];
			}
			else
			{
				[win setHasShadow:NO];
			}
			//背景色設定
			[win setBackgroundColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.0]];
			//透明ウインドウ設定
			[win setOpaque:NO];//これを設定しないと透明・半透明ウインドウは実現できない
			//必要でない場合（ボタンクリック等）ではkey windowにならないようにする
			[win setBecomesKeyOnlyIfNeeded:YES];
			break;
		}
	  case kWindowClass_Floating:
		{
			//Window Controllerは生成しない
			mWindowController = nil;
			//ウインドウの初期frame設定
			NSRect	rect = NSMakeRect(-10000,-10000,100,100);
			//スタイル設定
			NSUInteger	stylemask = NSBorderlessWindowMask;
			if( inHasTitlebar == true )
			{
				stylemask |= NSTitledWindowMask | NSUtilityWindowMask;
			}
			if( inHasCloseButton == true )
			{
				stylemask |= NSClosableWindowMask;
			}
			if( inHasCollapseButton == true )
			{
				stylemask |= NSMiniaturizableWindowMask;
			}
			if( inHasGrowBox == true )
			{
				stylemask |= NSResizableWindowMask;
			}
			//ウインドウ生成
			CocoaPanelWindow*	win = [[CocoaPanelWindow alloc] initWithContentRect:rect styleMask:stylemask 
								backing:NSBackingStoreBuffered //buffered（Mac OS Xの標準）
								defer:NO];//defer:NOにする。YESにすると、mousemovedが動作しない。
			mWindow = win;
			//初期設定
			[win setup:(&mWindowImp)];
			//オーバーレイウインドウレイヤー設定
			[win setOverlayWindowLayer:inOverlayWindowLayer];
			//ウインドウhide
			[win orderOut:nil];
			//LiveResizeの完了を検知するためのダミーviewを生成。ウインドウへ登録。
			//★addSubviewしたビューはウインドウ削除時にreleaseされるかどうか確認必要
			[GetContentView() addSubview:[[[CocoaLiveResizeDetectionView alloc] initWithWindowImp:(&mWindowImp)] autorelease] 
			positioned:NSWindowAbove relativeTo:nil];
			//フローティングウインドウ設定 #1133
			SetupFloatingWindow(inNonFloating,inIgnoreClick);
			//Delegate設定
			mWindowDelegate = [[CocoaWindowDelegate alloc] initWithWindowImp:(&mWindowImp)];
			[win setDelegate:mWindowDelegate];
			break;
		}
	}
	//親ウインドウが指定されている場合は記憶（実際に親子関係を設定するのはShow()/Hide()時）
	if( inParentWindow != nil )
	{
		//親ウインドウへのポインタを記憶
		mParentWindow = inParentWindow;
		//ポインタを記憶するのでretain
		[mParentWindow retain];
	}
	//AppleScript用オブジェクト生成 #1034
	mCASWindow = [[CASWindow alloc] init:mWindowImp.GetAWin().GetObjectID()];
}

//#1133
/**
フローティングウインドウ設定
*/
void	CWindowImpCocoaObjects::SetupFloatingWindow( const ABool inNonFloating, const ABool inIgnoreClick )
{
	if( inNonFloating == false )
	{
		//-------------------- 通常フローティング --------------------
		
		//ウインドウレベル設定 #1034
		[mWindow setLevel:NSFloatingWindowLevel];
		//ウインドウdeactivate時、hideする
		[mWindow setHidesOnDeactivate:YES];
		//マウスクリック無視判定
		if( inIgnoreClick == true )
		{
			//ウインドウはMainWindow,KeyWindowになれない
			[mWindow setCanBecomeMainWindow:NO];
			[mWindow setCanBecomeKeyWindow:NO];
			//マウスイベント無視する
			[mWindow setIgnoresMouseEvents:YES];
		}
		else
		{
			//ウインドウはMainWindow, KeyWindowになれる
			[mWindow setCanBecomeMainWindow:NO];
			[mWindow setCanBecomeKeyWindow:YES];
			//マウスイベント無視しない
			[mWindow setIgnoresMouseEvents:NO];
		}
		//フローティングパネル設定
		[mWindow setFloatingPanel:YES];
		//シャドウ表示する
		[mWindow setHasShadow:YES];
		//背景色設定
		[mWindow setBackgroundColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.0]];
		//透明ウインドウ設定
		[mWindow setOpaque:NO];//これを設定しないと透明・半透明ウインドウは実現できない
		//必要でない場合（ボタンクリック等）ではkey windowにならないようにする
		[mWindow setBecomesKeyOnlyIfNeeded:YES];
	}
	else
	{
		//-------------------- non-flating フローティング --------------------
		
		//ウインドウレベル設定 #1034
		[mWindow setLevel:NSNormalWindowLevel];
		//ウインドウdeactivate時、hideする
		[mWindow setHidesOnDeactivate:NO];
		//マウスクリック無視判定
		if( inIgnoreClick == true )
		{
			//ウインドウはMainWindow,KeyWindowになれない
			[mWindow setCanBecomeMainWindow:NO];
			[mWindow setCanBecomeKeyWindow:NO];
			//マウスイベント無視する
			[mWindow setIgnoresMouseEvents:YES];
		}
		else
		{
			//ウインドウはMainWindow, KeyWindowになれる
			[mWindow setCanBecomeMainWindow:YES];
			[mWindow setCanBecomeKeyWindow:YES];
			//マウスイベント無視しない
			[mWindow setIgnoresMouseEvents:NO];
		}
		//フローティングパネル設定
		[mWindow setFloatingPanel:NO];
		//シャドウ表示する
		[mWindow setHasShadow:YES];
		//背景色設定
		[mWindow setBackgroundColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.0]];
		//透明ウインドウ設定
		[mWindow setOpaque:NO];//これを設定しないと透明・半透明ウインドウは実現できない
		//必要でない場合（ボタンクリック等）ではkey windowにならないようにする
		[mWindow setBecomesKeyOnlyIfNeeded:NO];
	}
}

/**
キャレット描画用のオーバーレイウインドウ生成
*/
/*
void	CWindowImpCocoaObjects::CreateCaretOverlayWindow( const AIndex inCaretWindowIndex )
{
	//ウインドウのframe設定（空）
	NSRect	rect = NSMakeRect(0,0,10,10);
	//スタイル設定
	NSUInteger	stylemask = NSBorderlessWindowMask;//Overlayの場合は常にNSBorderlessWindowMask固定
	//ウインドウ生成 CocoaCaretOverlayWindowはNSPanel＋ignoresMouseEventsにYESを返すようにしたもの
	CocoaCaretOverlayWindow*	win = [[CocoaCaretOverlayWindow alloc] initWithContentRect:rect styleMask:stylemask 
								backing:NSBackingStoreBuffered //buffered（Mac OS Xの標準）
								defer:NO];//defer:NOにする。YESにすると、mousemovedが動作しない。
	//
	mCaretOverlayWindowArray.Set(inCaretWindowIndex,win);
	//ウインドウhide
	[win orderOut:nil];
	//ウインドウdeactivate時、hideしない
	[win setHidesOnDeactivate:NO];
	//シャドウ表示しない
	[win setHasShadow:NO];
	//背景色設定
	[win setBackgroundColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.0]];
	//透明ウインドウ設定
	[win setOpaque:NO];
	//α設定
	[win setAlphaValue:0.7];
	
	//ウインドウshow
	[win orderFront:nil];
	//親ウインドウ設定
	[mWindow addChildWindow:win ordered:NSWindowAbove];
	
	//view生成（ポインタ保持）
	CocoaCaretOverlayView*	view = [[CocoaCaretOverlayView alloc] initWithFrame:NSMakeRect(0,0,0,0)];
	//
	mCaretOverlayViewArray.Set(inCaretWindowIndex,view);
	//windowへaddSubview
	[[win contentView] addSubview:view positioned:NSWindowAbove relativeTo:nil];
}
*/
/**
キャレット描画用のオーバーレイウインドウ取得（未生成なら生成）
*/
/*
CocoaCaretOverlayWindow*	CWindowImpCocoaObjects::GetCaretOverlayWindow( const AIndex inCaretWindowIndex, const ABool inCreateIfNotCreated )
{
	//
	while( mCaretOverlayWindowArray.GetItemCount()-1 < inCaretWindowIndex )
	{
		mCaretOverlayWindowArray.Add(nil);
		mCaretOverlayViewArray.Add(nil);
	}
	
	//未生成なら生成
	if( mCaretOverlayWindowArray.Get(inCaretWindowIndex) == nil && inCreateIfNotCreated == true)
	{
		CreateCaretOverlayWindow(inCaretWindowIndex);
	}
	//取得
	return mCaretOverlayWindowArray.Get(inCaretWindowIndex);
}
*/
/**
ウインドウ削除
*/
void	CWindowImpCocoaObjects::DeleteWindow()
{
	//fprintf(stderr,"CWindowImpCocoaObjects::DeleteWindow() - start \n");
	//progress indicator削除
	for( AIndex i = 0; i < mProgressIndicatorArray.GetItemCount(); i++ )
	{
		[mProgressIndicatorArray.Get(i) release];
	}
	mProgressIndicatorArray.DeleteAll();
	//user pane view情報削除
	for( AIndex i = 0; i < mCocoaUserPaneViewArray_UserPaneView.GetItemCount(); i++ )
	{
		[mCocoaUserPaneViewArray_UserPaneView .Get(i) release];
	}
	mCocoaUserPaneViewArray_ControlID.DeleteAll();
	mCocoaUserPaneViewArray_UserPaneView.DeleteAll();
	//control idキャッシュ削除
	for( AIndex i = 0; i < mControlIDCacheArray_NSView.GetItemCount(); i++ )
	{
		[mControlIDCacheArray_NSView.Get(i) release];
	}
	mControlIDCacheArray_ControlID.DeleteAll();
	mControlIDCacheArray_NSView.DeleteAll();
	
	/*
	//キャレット描画用オーバーレイウインドウ削除
	for( AIndex i = 0; i < mCaretOverlayWindowArray.GetItemCount(); i++ )
	{
		CocoaCaretOverlayWindow*	win = mCaretOverlayWindowArray.Get(i);
		if( win != nil )
		{
			//キャレット描画viewのrelease（ポインタ保持しているので）
			[mCaretOverlayViewArray.Get(i) release];
			//親から登録をはずしてhide
			//（ここで登録を外しておかないと、親ウインドウがシートウインドウだった場合、
			//close時にシートウインドウの親ウインドウがhideされるなどの影響が出る。）
			[mWindow removeChildWindow:win];
			[win orderOut:nil];
			//クローズ
			[win close];
			//解放
			[win release];
		}
	}
	mCaretOverlayWindowArray.DeleteAll();
	mCaretOverlayViewArray.DeleteAll();
	*/
	//ウインドウ削除
	if( mWindow != nil )
	{
		//クローズ
		[mWindow close];
		//解放
		[mWindow release];
		mWindow = nil;
	}
	//ウインドウdelegate削除
	if( mWindowDelegate != nil )
	{
		//解放
		[mWindowDelegate release];
		mWindowDelegate = nil;
	}
	//Window Controller削除
	if( mWindowController != nil )
	{
		//解放
		[mWindowController release];
		mWindowController = nil;
	}
	//親ウインドウへのポインタ解放
	if( mParentWindow != nil )
	{
		//解放
		[mParentWindow release];
		mParentWindow = nil;
	}
	//AppleScript用オブジェクト解放 #1034
	if( mCASWindow != nil )
	{
		[mCASWindow release];
		mCASWindow = nil;
	}
	//fprintf(stderr,"CWindowImpCocoaObjects::DeleteWindow() - end \n");
}

/**
親ウインドウへのリンクを記憶
*/
void	CWindowImpCocoaObjects::SetParentWindow( NSWindow* inParentWindow )
{
	if( inParentWindow != nil )
	{
		//親ウインドウへのポインタを記憶
		mParentWindow = inParentWindow;
		//ポインタを記憶するのでretain
		[mParentWindow retain];
	}
}

/**
親ウインドウへのリンクを解放
*/
void	CWindowImpCocoaObjects::ClearParentWindow()
{
	if( mParentWindow != nil )
	{
		//解放
		[mParentWindow release];
		mParentWindow = nil;
	}
}

/**
ツールバー生成
*/
void	CWindowImpCocoaObjects::CreateToolbar( /*#1012 const*/ NSString* inString, const ABool inForPref )
{
	//CreateToolbar()実行中フラグON（EVT_ToolbarItemAdded()実行を抑制するため。）
	mCreatingToolbar = true;
	//
	NSToolbar*	toolbar = [[NSToolbar alloc] initWithIdentifier:inString];
	//
	if( inForPref == false )
	{
		[toolbar setAllowsUserCustomization:YES];
		[toolbar setAutosavesConfiguration:YES];
		[toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
		[toolbar setSizeMode:NSToolbarSizeModeRegular];
		[toolbar setDelegate:(id<NSToolbarDelegate>)GetWindow()];
		//
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
		{
			[toolbar setShowsBaselineSeparator:NO];
		}
	}
	else
	{
		[toolbar setAllowsUserCustomization:NO];
		[toolbar setAutosavesConfiguration:NO];
		[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
		[toolbar setSizeMode:NSToolbarSizeModeRegular];
		[toolbar setDelegate:(id<NSToolbarDelegate>)GetWindow()];
		//
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
		{
			[toolbar setShowsBaselineSeparator:YES];
		}
	}
	[GetWindow() setToolbar:toolbar];
	//CreateToolbar()実行中フラグOFF
	mCreatingToolbar = false;
}

#pragma mark ===========================

#pragma mark ---

/**
このウインドウでモーダルセッション開始
*/
void	CWindowImpCocoaObjects::StartModalSession()
{
	//
	mModalSession = [NSApp beginModalSessionForWindow:mWindow];
}

/**
このウインドウでのモーダルセッションでの継続チェック
*/
ABool	CWindowImpCocoaObjects::CheckContinueingModalSession()
{
	//
	if( [NSApp runModalSession:mModalSession] != NSRunContinuesResponse )
	{
		return false;
	}
	else
	{
		return true;
	}
}

/**
このウインドウでのモーダルセッション終了
@note なお、ウインドウをcloseしないと、モーダルセッションはきれいに終わらないもよう（Mac OS X 10.6での動作。異常におそくなる）。hideではだめ。
*/
void	CWindowImpCocoaObjects::EndModalSession()
{
	//
	[NSApp stopModal];
	[NSApp endModalSession:mModalSession];
	//
	mModalSession = nil;
}

/**
モーダルウインドウループ開始（ウインドウ内のイベント処理でStopModalSessionがコールされるまで終了しない）
*/
void	CWindowImpCocoaObjects::RunModalWindow()
{
	//画面中央に表示
	[mWindow center];
	//show #1034
	[mWindow orderFront:nil];
	//モーダル開始（終了するまでブロック）
	[NSApp runModalForWindow:mWindow];
}

/**
このウインドウでのモーダルセッション停止
*/
void	CWindowImpCocoaObjects::StopModalSession()
{
	//
	[NSApp stopModal];
}

#pragma mark ===========================

#pragma mark ---UserPaneView登録／解除

/**
コントロール登録
*/
void	CWindowImpCocoaObjects::RegisterUserPaneView( const AControlID inControlID, CocoaUserPaneView* inUserPane )
{
	//コントロール登録
	mCocoaUserPaneViewArray_ControlID.Add(inControlID);
	mCocoaUserPaneViewArray_UserPaneView.Add(inUserPane);
	//ポインタ保持しているのでretainする
	[inUserPane retain];
}

/**
*/
void	CWindowImpCocoaObjects::UnregisterUserPaneView( const AControlID inControlID )
{
	//★userpaneでないコントロールの削除が本当に不要かどうか確認必要
	//windowのrelease時に、子viewにすべてreleaseが送信されるなら、問題なし。
	
	AIndex	index = mCocoaUserPaneViewArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		//RegisterControl()時にretainしているので、ここでreleaseする
		[mCocoaUserPaneViewArray_UserPaneView.Get(index) release];
		//コントロール登録解除
		mCocoaUserPaneViewArray_ControlID.Delete1(index);
		mCocoaUserPaneViewArray_UserPaneView.Delete1(index);
	}
}

#pragma mark ===========================

#pragma mark ---コントロールID(=tag)/id変換

/**
Tag(ControlID)からViewを取得
*/
NSView*	CWindowImpCocoaObjects::FindViewByTag( const AControlID inControlID, const ABool inThrowIfNotFound ) const
{
	//ウインドウ生成済みチェック
	if( GetWindow() == nil ) {_ACThrow("Window not created",NULL);return NULL;}
	
	//UserPaneの場合は、Hash配列から検索
	AIndex	userPaneIndex = mCocoaUserPaneViewArray_ControlID.Find(inControlID);
	if( userPaneIndex != kIndex_Invalid )
	{
		return mCocoaUserPaneViewArray_UserPaneView.Get(userPaneIndex);
	}
	
	//キャッシュにあれば、キャッシュから取得
	AIndex	index = mControlIDCacheArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		return mControlIDCacheArray_NSView.Get(index);
	}
	
	//tag(=ControlID)からviewを検索
	NSView*	view = [GetWindow() findViewByControlID:inControlID];
	if( view == nil )
	{
		//ツールバー項目から検索
		NSToolbarItem*	item = FindToolbarItemByTag(inControlID);
		if( item != nil )
		{
			return [item view];
		}
		if( inThrowIfNotFound == true )
		{
			_ACThrow("NSView is not found by tag.",NULL);
		}
		return nil;
	}
	//キャッシュに追加
	AddToControlIDCache(inControlID,view);
	//
	return view;
}

/**
Tag(ControlID)からUserPaneViewを取得
*/
CocoaUserPaneView*	CWindowImpCocoaObjects::FindCocoaUserPaneViewByTag( const AControlID inControlID ) const
{
	//ウインドウ生成済みチェック
	if( GetWindow() == nil ) {_ACThrow("Window not created",NULL);return NULL;}
	
	//Hash配列から検索
	AIndex	userPaneIndex = mCocoaUserPaneViewArray_ControlID.Find(inControlID);
	if( userPaneIndex != kIndex_Invalid )
	{
		return mCocoaUserPaneViewArray_UserPaneView.Get(userPaneIndex);
	}
	
	//見つからない
	_ACError("CocoaUserPane not found",NULL);
	return nil;
}

/**
ProgressIndicator取得
*/
NSProgressIndicator*	CWindowImpCocoaObjects::FindProgressIndicatorByIndex( const AIndex inProgressIndex ) const
{
	//ウインドウ生成済みチェック
	if( GetWindow() == nil ) {_ACThrow("Window not created",NULL);return NULL;}
	
	//indexに対応するProgressIndicatorを返す
	return mProgressIndicatorArray.Get(inProgressIndex);
}

/**
Tag(ControlID)からツールバーを取得
*/
NSToolbarItem*	CWindowImpCocoaObjects::FindToolbarItemByTag( const AControlID inControlID ) const
{
	//ウインドウ生成済みチェック
	if( GetWindow() == nil ) {_ACThrow("Window not created",NULL);return NULL;}
	
	//ツールバーが無いときはnilを返す
	if( [GetWindow() toolbar] == nil )
	{
		return nil;
	}
	//ツールバー項目毎のループ
	NSArray*	toolbarItems = [[GetWindow() toolbar] items];
	NSInteger	itemCount = [toolbarItems count];
	for( NSInteger index = 0; index < itemCount; index++ )
	{
		//ツールバー項目取得し、tag比較
		NSToolbarItem*	item = [toolbarItems objectAtIndex:index];
		if( inControlID == [item tag] )
		{
			return item;
		}
	}
	//追加中のツールバー項目に一致すればそれを返す
	NSToolbarItem*	addingToolbarItem = [GetWindow() getAddingToolbarItem];
	if( addingToolbarItem != nil )
	{
		if( [addingToolbarItem tag] == inControlID )
		{
			return addingToolbarItem;
		}
	}
	return nil;
}

/**
control IDキャッシュに追加
*/
void	CWindowImpCocoaObjects::AddToControlIDCache( const AControlID inControlID, NSView* inView ) const
{
	AIndex	index = mControlIDCacheArray_ControlID.Find(inControlID);
	if( index == kIndex_Invalid )
	{
		//キャッシュへ追加
		index = mControlIDCacheArray_ControlID.Add(inControlID);
		mControlIDCacheArray_NSView.Add(inView);
		//view retain
		[inView retain];
	}
}

/**
control IDキャッシュから削除
*/
void	CWindowImpCocoaObjects::RemoveFromControlIDCache( const AControlID inControlID ) const
{
	AIndex	index = mControlIDCacheArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		//view release
		[mControlIDCacheArray_NSView.Get(index) release];
		//キャッシュから削除
		mControlIDCacheArray_ControlID.Delete1(index);
		mControlIDCacheArray_NSView.Delete1(index);
	}
}


//#1034
/**
CUserPaneCocoaObjects
*/
CUserPaneCocoaObjects::CUserPaneCocoaObjects() : mNormalFont(NULL),mBoldFont(NULL),mItalicFont(NULL),mBoldItalicFont(NULL)
{
}

CUserPaneCocoaObjects::~CUserPaneCocoaObjects()
{
	//フォント解放
	if( mNormalFont != NULL )
	{
		[mNormalFont release];
	}
	if( mBoldFont != NULL )
	{
		[mBoldFont release];
	}
	if( mItalicFont != NULL )
	{
		[mItalicFont release];
	}
	if( mBoldItalicFont != NULL )
	{
		[mBoldItalicFont release];
	}
}



