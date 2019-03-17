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

CocoaWindow

*/

#import "CocoaWindow.h"
#import "CocoaPanelWindow.h"
#import "CocoaTextField.h"
#import "CocoaFontField.h"
#import "CocoaTabSelectButton.h"
#import "CocoaProgressIndicator.h"
#import "CocoaCollapsableView.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"
#include "CWindowImpCocoaObjects.h"
#include "../../App/Source/ATypes.h"
#include "../../App/Source/AApp.h"
#import "AppDelegate.h"

@implementation CASWindow
#pragma mark ===========================
#pragma mark [クラス]CASWindow
#pragma mark ===========================

#pragma mark --- property

/**
init
*/
- (id)init:(AWindowID)windowID
{
	self = [super init];
	if(self)
	{
		mWindowID = windowID;
	}
	return self;
}

// -------------------- bounds プロパティ --------------------

/**
bounds取得
*/
- (NSAppleEventDescriptor*)asBounds
{
	ARect	rect;
	AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_GetWindowBounds(rect);
	return [NSAppleEventDescriptor descriptorWithARect:rect];
}

/**
bounds設定
*/
- (void)setAsBounds:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	ARect	rect = [aeDesc arectValue];
	if( rect.right - rect.left > 0 )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_SetWindowBounds(rect);
	}
	else
	{
		//エラー
		[CocoaApp appleScriptNotHandledError];
	}
}

// -------------------- closable プロパティ --------------------

/**
closable取得
*/
- (NSAppleEventDescriptor*)asClosable
{
	Boolean b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_HasCloseButton();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- titled プロパティ --------------------

/**
titled取得
*/
- (NSAppleEventDescriptor*)asTitled
{
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_HasTitleBar();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- index プロパティ --------------------

/**
index取得
*/
- (NSAppleEventDescriptor*)asIndex
{
	//
	AItemCount	count = GetApp().NVI_GetWindowCount(kWindowType_Invalid);
	for( ANumber num = 1; num <= count; num++ )
	{
		if( GetApp().NVI_GetNthWindowID(kWindowType_Invalid,num) == mWindowID )
		{
			return [NSAppleEventDescriptor descriptorWithInt32:num];
		}
	}
	return nil;
}

/**
index設定
*/
- (void)setAsIndex:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	[self setWindowOrder:[aeDesc int32Value]];
}

/**
ウインドウ順設定
*/
- (void)setWindowOrder:(SInt32)orderNumber
{
	//
	if( orderNumber <= 1 )
	{
		//最前面にする
		AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_SelectWindow();
	}
	else
	{
		//現在のウインドウ順取得
		AItemCount	count = GetApp().NVI_GetWindowCount(kWindowType_Invalid);
		ANumber currentnum = 1;
		for( ; currentnum <= count; currentnum++ )
		{
			if( GetApp().NVI_GetNthWindowID(kWindowType_Invalid,currentnum) == mWindowID )
			{
				break;
			}
		}
		//現在のnumberと同じなら何もせずreturn、現在のnumberよりも後に配置するなら指定numberを+1する。
		ANumber	num = orderNumber;
		if( currentnum == num )   return;
		if( currentnum < num )
		{
			num++;
		}
		//直前のウインドウ取得
		AWindowID winID = GetApp().NVI_GetNthWindowID(kWindowType_Invalid,num-1);
		if( winID == kObjectID_Invalid )
		{
			//エラー
			[CocoaApp appleScriptNotHandledError];
		}
		else
		{
			//直前のウインドウの後ろに移動する
			AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_SendBehind(winID);
		}
	}
}

// -------------------- floating プロパティ --------------------

/**
floating取得
*/
- (NSAppleEventDescriptor*)asFloating
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsFloating();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- modal プロパティ --------------------

/**
modal取得
*/
- (NSAppleEventDescriptor*)asModal
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsModal();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- resizable プロパティ --------------------

/**
resizable取得
*/
- (NSAppleEventDescriptor*)asResizable
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsResizable();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- zoomable プロパティ --------------------

/**
zoomable取得
*/
- (NSAppleEventDescriptor*)asZoomable
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsZoomable();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- zoomed プロパティ --------------------

/**
zoomed取得
*/
- (NSAppleEventDescriptor*)asZoomed
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsZoomed();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- name プロパティ --------------------

/**
name取得
*/
- (NSAppleEventDescriptor*)asName
{
	//
	AText	text;
	AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_GetCurrentTitle(text);
	return [NSAppleEventDescriptor descriptorWithString:text.CreateNSString(true)];
}

/**
name設定
*/
- (void)setAsName:(id)value
{
	NSAppleEventDescriptor*	aeDesc = (NSAppleEventDescriptor*)value;
	//
	//
	AText	text;
	text.SetFromNSString([aeDesc stringValue]);
	AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_SetDefaultTitle(text);
}

// -------------------- visible プロパティ --------------------

/**
visible取得
*/
- (NSAppleEventDescriptor*)asVisible
{
	//
	Boolean	b = AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_IsWindowVisible();
	return [NSAppleEventDescriptor descriptorWithBoolean:b];
}

// -------------------- position プロパティ --------------------

/**
position取得
*/
- (NSAppleEventDescriptor*)asPosition
{
	APoint	pt = {0};
	AApplication::GetApplication().NVI_GetWindowByID(mWindowID).NVI_GetWindowPosition(pt);
	return [NSAppleEventDescriptor descriptorWithAPoint:pt];
}
@end

@implementation CocoaWindow

#pragma mark ===========================
#pragma mark [クラス]CocoaWindow
#pragma mark ===========================

#pragma mark ===========================

#pragma mark ---初期設定

/**
viewの初期設定
*/
- (void)setupView: (NSView*)view
{
	//tabViewについては、現在選択されているtab内以外のviewはsubviewsに含まれないので、
	//tabViewの各tabについて検索する必要がある。
	if( [view isKindOfClass:[NSTabView class]] == YES )
	{
		//tab view取得
		NSTabView*	tabview = static_cast<NSTabView*>(view);
		/*tab viewのbottom tabをtablessへ自動変換するテスト
		if( [tabview tabViewType] == NSBottomTabsBezelBorder )
		{
			[tabview setTabViewType:NSNoTabsNoBorder];
			//
			NSRect	frame = [tabview frame];
			frame.origin.x += 10;
			frame.origin.y -= 10;
			[tabview setFrame:frame];
		}
		*/
		//tab view itemごとのループ
		AItemCount	tabViewItemCount = [[tabview tabViewItems] count];
		for( AIndex i = 0; i < tabViewItemCount; i++ )
		{
			//tab view item内のviewについてsetupView実行
			[self setupView:[[tabview tabViewItemAtIndex:i] view]];
		}
		//tab viewリストに追加
		mTabViewArray.Add(tabview);
	}
	//
	if( [view isKindOfClass:[NSControl class]] == YES )
	{
		NSControl*	control = (NSControl*)view;
		if( [control tag] > 0 && [control target] == nil )
		{
			//NSPopUpButtonの場合
			if( [control isKindOfClass:[NSPopUpButton class]] == YES )
			{
				//target/action設定
				[control setTarget:self];
				[control setAction:@selector(doPopupMenuAction:)];
			}
			//NSScrollerの場合
			else if( [control isKindOfClass:[NSScroller class]] == YES )
			{
				//target/action設定
				[control setTarget:self];
				[control setAction:@selector(doScrollbarAction:)];
			}
			//NSButtonの場合
			else if( [control isKindOfClass:[NSButton class]] == YES )
			{
				//target/action設定
				[control setTarget:self];
				[control setAction:@selector(doButtonAction:)];
			}
			//CocoaTextFieldの場合
			else if( [control isKindOfClass:[CocoaTextField class]] == YES )
			{
				CocoaTextField*	cocoaTextField = reinterpret_cast<CocoaTextField*>(control);
				//初期設定
				[cocoaTextField setup:mWindowImp];
				//target/action設定
				[cocoaTextField setTarget:self];
				[cocoaTextField setAction:@selector(doTextFieldAction:)];
			}
			//CocoaFontFieldの場合
			else if( [control isKindOfClass:[CocoaFontField class]] == YES )
			{
				CocoaFontField*	cocoaFontField = reinterpret_cast<CocoaFontField*>(control);
				//初期設定
				[cocoaFontField setup:mWindowImp];
				//target/action設定はしない
			}
			//NSTextFieldの場合
			else if( [control isKindOfClass:[NSTextField class]] == YES )
			{
				//target/action設定
				[control setTarget:self];
				[control setAction:@selector(doTextFieldAction:)];
			}
			//NSStepperの場合
			else if( [control isKindOfClass:[NSStepper class]] == YES )
			{
				//target/action設定
				[control setTarget:self];
				[control setAction:@selector(doStepper:)];
				//
				NSStepper*	stepper = reinterpret_cast<NSStepper*>(control);
				[stepper setMinValue:0];
				[stepper setMaxValue:100];
				[stepper setIntValue:50];
				[stepper setIncrement:1];
			}
			//上記以外の場合
			else 
			{
				//target/action設定
				[control setTarget:self];
				[control setAction:@selector(doButtonAction:)];
			}
		}
	}
	//子viewがあれば、再帰的にsetupViews実行
	if( [[view subviews] count] > 0 )
	{
		[self setupViews:view];
	}
}

/**
各viewの初期設定
*/
- (void)setupViews:(NSView*)inParentView
{
	//各view取得
	NSArray*	subviews = [inParentView subviews];
	NSInteger itemCount = [subviews count];
	for( NSInteger index = 0; index < itemCount; index++  )
	{
		//index番目のview取得
		NSView*	view = [subviews objectAtIndex:index];
		[self setupView:view];
	}
}

/**
Toolbarの初期設定
*/
- (void)setupToolbar
{
	//toolbar無しなら何もしない
	if( [self toolbar] == nil )   return;
	
	//各ツールバー項目毎のループ
	NSArray*	toolbarItems = [[self toolbar] items];
	NSInteger	itemCount = [toolbarItems count];
	for( NSInteger index = 0; index < itemCount; index++ )
	{
		//ツールバー項目のtarget/action設定
		NSToolbarItem*	item = [toolbarItems objectAtIndex:index];
		[item setTarget:self];
		[item setAction:@selector(doToolbarItemAction:)];
		//ツールバー項目内のviewについてsetupを行う
		NSView*	view = [item view];
		if( view != nil )
		{
			[self setupView:view];
		}
	}
}

/**
初期設定
*/
- (void)setup:(CWindowImp*)inWindowImp
{
	//CWindowImpへのポインタ設定
	mWindowImp = inWindowImp;
	//Overlayウインドウレイヤー初期設定
	mOverlayWindowLayer = kOverlayWindowLayer_None;
	//mCanBecomeMainWindow, mCanBecomeKeyWindowのデフォルト設定
	mCanBecomeMainWindow = YES;
	mCanBecomeKeyWindow = YES;
	//mIgnoresMouseEvents = NO;
	//各viewの初期設定
	[self setupViews:[self contentView]];
	//ツールバーの初期設定
	[self setupToolbar];
	//mousemovedイベントを受け取る
	[self setAcceptsMouseMovedEvents:YES];
	//クローズ時のreleaseはしない（CocoaObjectsで管理する）
	[self setReleasedWhenClosed:NO];
	//追加しようとしているツールバー項目へのリンクを初期化
	mAddingToolbarItem = nil;
	//ツールバータイプ
	mToolbarType = 0;
}

/**
ツールバータイプ設定
*/
- (void)setToolbarType:(AIndex)inType
{
	mToolbarType = inType;
}

/**
Overlayウインドウレイヤー設定
*/
- (void)setOverlayWindowLayer:(AOverlayWindowLayer)inOverlayWindowLayer
{
	mOverlayWindowLayer = inOverlayWindowLayer;
}

/**
Overlayウインドウレイヤー取得
*/
- (AOverlayWindowLayer)getOverlayWindowLayer
{
	return mOverlayWindowLayer;
}

/**
ControlIDからviewを検索
*/
- (NSView*)findViewByControlID:(AControlID)inControlID
{
	//viewWithTagを使って検索
	NSView*	view = [[self contentView] viewWithTag:inControlID];
	if( view == nil )
	{
		//segmented control内のtagはviewWithTagで検索されないので、それぞれのNSSegmentedControlについて検索
		//ただし、segment全体のviewを返すので、segmentごとにアクセスする必要がある場合は、別途mSegmentedControlArray_Segmentを使ってアクセスする必要がある
		AIndex	segmentedControlIndex = mSegmentedControlArray_ControlID.Find(inControlID);
		if( segmentedControlIndex != kIndex_Invalid )
		{
			return mSegmentedControlArray_SegmentedControl.Get(segmentedControlIndex);
		}
		
		//NSTabView内の、表示されていない項目は、viewWithTagで検索されないので、それぞれのNSTabViewについて検索
		//mTabViewArrayはsetupView内で設定される。ウインドウ内の全てのNSTabViewが入っている。
		AItemCount	itemCount = mTabViewArray.GetItemCount();
		for( AIndex j = 0; j < itemCount; j++ )
		{
			//NSTabView内の各NSTabViewItem毎のループ
			AItemCount	tabViewItemCount = [[mTabViewArray.Get(j) tabViewItems] count];
			for( AIndex i = 0; i < tabViewItemCount; i++ )
			{
				//NSTabViewItemのviewについてviewWithTagを使って検索
				view = [[[mTabViewArray.Get(j) tabViewItemAtIndex:i] view] viewWithTag:inControlID];
				if( view != nil )
				{
					return view;
				}
			}
		}
	}
	return view;
}

//#1034
/**
WindowIDを取得
*/
- (AWindowID)windowID
{
	return mWindowImp->GetAWin().GetObjectID();
}

#pragma mark ===========================

#pragma mark ---各種設定／取得

/**
IBで設定されたProgressIndicator取得
*/
- (NSProgressIndicator*)getProgressIndicator
{
	return progressIndicator;
}

/**
tab view取得
*/
- (NSTabView*)getTabView
{
	return tabView;
}

/**
mCanBecomeMainWindow設定
*/
- (void)setCanBecomeMainWindow:(BOOL)inVal
{
	mCanBecomeMainWindow = inVal;
}

/**
mCanBecomeKeyWindow設定
*/
- (void)setCanBecomeKeyWindow:(BOOL)inVal
{
	mCanBecomeKeyWindow = inVal;
}

/**
マウスイベントignoreするかどうかを設定
*/
/*
- (void)setIgnoresMouseEvents:(BOOL)inVal
{
	mIgnoresMouseEvents = inVal;
}
*/

/**
ウインドウのframeをAGlobalRectで取得
*/
- (AGlobalRect)getGlobalRectFromFrame:(NSRect)frame
{
	AGlobalRect	rect = {0};
	NSRect	contentFrame = [self contentRectForFrameRect:frame];
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	rect.left		= contentFrame.origin.x;
	rect.top		= firstScreenFrame.size.height - (contentFrame.origin.y + contentFrame.size.height);
	rect.right		= contentFrame.origin.x + contentFrame.size.width;
	rect.bottom		= firstScreenFrame.size.height - contentFrame.origin.y;
	return rect;
}

#pragma mark ===========================

#pragma mark ---オーバーライド

/**
FirstResponder可否
*/
- (BOOL)acceptsFirstResponder
{
	return YES;
}

/**
MainWindow可否
*/
- (BOOL)canBecomeMainWindow
{
	return mCanBecomeMainWindow;
}

/**
KeyWindow可否
*/
- (BOOL)canBecomeKeyWindow
{
	return mCanBecomeKeyWindow;
}

/**
マウスイベントignoreするかどうか
*/
/*
- (BOOL)ignoresMouseEvents
{
	return mIgnoresMouseEvents;
}
*/

#pragma mark ===========================

#pragma mark ---

/**
子オーバーレイウインドウのzorderをmOverlayWindowLayerに従って再配置
*/
- (void)reorderOverlayChildWindows
{
	NSArray*	childwindows = [self childWindows];
	AItemCount	childCount = [childwindows count];
	//各レイヤー毎のループ
	for( AOverlayWindowLayer overlayLayer = kOverlayWindowLayer_None; overlayLayer <= kOverlayWindowLayer_Top; overlayLayer++ )
	{
		//各子ウインドウごとのループ
		for( AIndex i = 0; i < childCount; i++ )
		{
			//子ウインドウ取得
			NSWindow*	w = [childwindows objectAtIndex:i];
			//子ウインドウがCocoaPanelWindowの場合に限り、ウインドウを再配置する。
			if( [w isKindOfClass:[CocoaPanelWindow class]] == YES )
			{
				CocoaPanelWindow*	childWindow = w;
				if( [childWindow getOverlayWindowLayer] == overlayLayer )
				{
					[self removeChildWindow:childWindow];
					[self addChildWindow:childWindow ordered:NSWindowAbove];
				}
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---フォントパネル

/**
フォントパネル表示
*/
- (void)showFontPanel:(NSFont*)inFont
{
	//指定フォントを記憶
	mCurrentFontPanelFont = inFont;
	//指定フォントをフォントパネルに設定
	[[NSFontManager sharedFontManager] setSelectedFont:mCurrentFontPanelFont isMultiple:NO];
	//フォントパネル表示
	[[NSFontPanel sharedFontPanel] orderFront:nil];
	//delegate, targetを自分に設定する
	[[NSFontManager sharedFontManager] setDelegate:self];
	[[NSFontManager sharedFontManager] setTarget:self];
}

/**
フォント変更時処理（NSFontManagerのdelegate）
*/
- (void)changeFont:(id)sender
{
	//Appleドキュメントに記載されているコード
	NSFont *newFont = [sender convertFont:mCurrentFontPanelFont];
	mCurrentFontPanelFont = newFont;
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_ValueChanged()実行
	mWindowImp->APICB_CocoaDoFontPanelFontChanged();
	
	OS_CALLBACKBLOCK_END;
}

/**
現在のフォントパネルに設定されているフォントを取得
*/
- (NSFont*)getFontPanelFont
{
	return mCurrentFontPanelFont;
}

#pragma mark ===========================

#pragma mark ---各種イベントオーバーライド

/**
MainWindowになったときの処理
*/
- (void)becomeMainWindow
{
	//継承処理実行
	[super becomeMainWindow];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowActivated()実行
	mWindowImp->APICB_CocoaBecomeMainWindow();
	
	OS_CALLBACKBLOCK_END;
}

/**
MainWindowでなくなったときの処理
*/
- (void)resignMainWindow
{
	//継承処理実行
	[super resignMainWindow];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowDeactivated()実行
	mWindowImp->APICB_CocoaResignMainWindow();
	
	OS_CALLBACKBLOCK_END;
}

/**
KeyWindowになったときの処理
*/
- (void)becomeKeyWindow
{
	//継承処理実行
	[super becomeKeyWindow];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowActivated()実行
	mWindowImp->APICB_CocoaBecomeKeyWindow();
	
	OS_CALLBACKBLOCK_END;
}

/**
KeyWindowでなくなったときの処理
*/
- (void)resignKeyWindow
{
	//継承処理実行
	[super resignKeyWindow];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowDeactivated()実行
	mWindowImp->APICB_CocoaResignKeyWindow();
	
	OS_CALLBACKBLOCK_END;
}

/**
Dock化されたときの処理
*/
- (void)miniaturize:(id)sender
{
	//継承処理実行
	[super miniaturize:sender];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowCollapsed()実行
	mWindowImp->APICB_CocoaMiniaturize();
	
	OS_CALLBACKBLOCK_END;
}

/**
Zoomボタンクリック時処理
*/
- (void)zoom:(id)sender
{
	//継承処理実行
	[super zoom:sender];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowResizeCompleted()実行
	mWindowImp->APICB_CocoaWindowResizeCompleted();
	
	OS_CALLBACKBLOCK_END;
}

/**
Frame設定時処理
*/
- (void)setFrame:(NSRect)windowFrame display:(BOOL)displayViews
{
	//fprintf(stderr,"CocoaWindow#setFrame - start \n");
	//変更前frame取得
	NSRect	oldFrame = [self frame];
	
	//継承処理実行
	[super setFrame:windowFrame display:displayViews];
	
	//setup前に呼ばれた場合は、APICB_CocoaWindowSetFrame()を実行せずに終了
	if( mWindowImp == nil )
	{
		return;
	}
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowBoundsChanged()実行
	//AGlobalRect	oldFrameGlobalRect = [self getGlobalRectFromFrame:oldFrame];
	AGlobalRect	newFrameGlobalRect = [self getGlobalRectFromFrame:[self frame]];
	mWindowImp->APICB_CocoaWindowBoundsChanged(newFrameGlobalRect);
	
	OS_CALLBACKBLOCK_END;
	//fprintf(stderr,"CocoaWindow#setFrame - end \n");
}

/**
キャンセルキー
*/
- (void)cancelOperation:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
	//キャンセルキー処理実行
	mWindowImp->APICB_CocoaDoCancelOperation(modifier);
	
	OS_CALLBACKBLOCK_END;
}

/**
タブキー等によるキーフォーカス移動時処理
*/
- (void)selectNextKeyView:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//フォーカス移動実行中のフラグを設定する
	mWindowImp->SetSwitchingFocusNow(true);
	
	//継承処理実行
	[super selectNextKeyView:sender];
	
	//フォーカス移動実行中のフラグを解除する
	mWindowImp->SetSwitchingFocusNow(false);
	
	OS_CALLBACKBLOCK_END;
}

/**
タブキー等によるキーフォーカス移動時処理
*/
- (void)selectPreviousKeyView:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//フォーカス移動実行中のフラグを設定する
	mWindowImp->SetSwitchingFocusNow(true);
	
	//継承処理実行
	[super selectPreviousKeyView:sender];
	
	//フォーカス移動実行中のフラグを解除する
	mWindowImp->SetSwitchingFocusNow(false);
	
	OS_CALLBACKBLOCK_END;
}

#pragma mark ===========================

#pragma mark ---各種Actionコールバック処理

/**
ボタンのsentAction
*/
- (IBAction) doButtonAction:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//CocoaTabSelectButtonクリック時処理（タブ選択）
	if( [sender isKindOfClass:[CocoaTabSelectButton class]] == YES )
	{
		CocoaTabSelectButton*	button = static_cast<CocoaTabSelectButton*>(sender);
		if( [button state] == NSOnState )
		{
			[button selectTabView];
		}
	}
	
	//CocoaDisclosureButtonクリック時処理（折りたたみ）
	if( [sender isKindOfClass:[CocoaDisclosureButton class]] == YES )
	{
		CocoaDisclosureButton*	button = static_cast<CocoaDisclosureButton*>(sender);
		[button updateCollapsableView];
	}
	
	//ボタンsentAction処理実行
	mWindowImp->APICB_CocoaDoButtonAction([sender tag]);
	
	OS_CALLBACKBLOCK_END;
}

/**
スクロールバー各パートクリック時処理
*/
- (IBAction) doScrollbarAction:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	NSScroller*	scroller = static_cast<NSScroller*>(sender);
	AScrollBarPart	scrollbarPart = kScrollBarPart_None;
	switch([scroller hitPart])
	{
	  case NSScrollerDecrementLine:
		{
			scrollbarPart = kScrollBarPart_UpButton;
			break;
		}
	  case NSScrollerIncrementLine:
		{
			scrollbarPart = kScrollBarPart_DownButton;
			break;
		}
	  case NSScrollerDecrementPage:
		{
			scrollbarPart = kScrollBarPart_PageUpButton;
			break;
		}
	  case NSScrollerIncrementPage:
		{
			scrollbarPart = kScrollBarPart_PageDownButton;
			break;
		}
	  case NSScrollerKnob:
		{
			scrollbarPart = kScrollBarPart_ThumbTracking;
			break;
		}
	  default:
		{
			//処理無し
			break;
		}
	}
	
	//ボタンsentAction処理実行
	mWindowImp->APICB_DoScrollBarAction([sender tag],scrollbarPart);
	
	OS_CALLBACKBLOCK_END;
}


/**
ポップアップメニューのsentAction
*/
- (IBAction) doPopupMenuAction:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//ポップアップメニューsentAction処理実行
	mWindowImp->APICB_CocoaDoPopupMenuAction([sender tag]);
	
	OS_CALLBACKBLOCK_END;
}

/**
テキストフィールドのsentAction
*/
- (IBAction) doTextFieldAction:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//テキストフィールドのsentAction処理実行
	mWindowImp->APICB_CocoaDoTextFieldAction([sender tag]);
	
	OS_CALLBACKBLOCK_END;
}

/**
Stepper値変更時処理
*/
- (IBAction) doStepper:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//stepperオブジェクト取得
	NSStepper*	stepper = reinterpret_cast<NSStepper*>(sender);
	//増加・減少判定（常に50にしておいて、49か51かで増加現象を判定）
	ABool	inc = true;
	if( [stepper intValue] < 50 )
	{
		inc = false;
	}
	[stepper setMinValue:0];
	[stepper setMaxValue:100];
	[stepper setIntValue:50];
	[stepper setIncrement:1];
	
	//対象コントロール取得（stepperのcontrolIDからkControlID_StepperOffsetを引いた値が対象コントロールのid）
	AControlID	targetControlID = [sender tag]-kControlID_StepperOffset;
	//対象コントロールの値を増減
	ANumber	num = mWindowImp->GetNumber(targetControlID);
	if( inc == true )
	{
		mWindowImp->SetNumber(targetControlID,num+1);
	}
	else
	{
		mWindowImp->SetNumber(targetControlID,num-1);
	}
	//対象コントロールの値変更時処理
	mWindowImp->APICB_CocoaDoTextFieldAction(targetControlID);
	
	OS_CALLBACKBLOCK_END;
}

/**
ツールバー項目のsentAction
*/
- (IBAction) doToolbarItemAction:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	NSInteger	tag = [sender tag];
	//segmented controlの場合は、segment毎のtagを取得
	if( [sender isKindOfClass:[NSSegmentedControl class]] == YES )
	{
		NSInteger clickedSegment = [sender selectedSegment];
		tag = [[sender cell] tagForSegment:clickedSegment];
	}
	//ツールバー項目のsentAction処理実行
	mWindowImp->APICB_CocoaToolbarItemAction(tag);
	
	OS_CALLBACKBLOCK_END;
}

/**
mouse moved時、子ウインドウ上で該当するviewを検索する。
@return 子ウインドウ上で処理実行済みならtrue
*/
- (ABool)doMouseMovedForChildWindows:(NSPoint)inGloalPoint modifierKeys:(AModifierKeys)modifier
{
	//子ウインドウ上でhitTestを行う
	//子ウインドウリスト取得
	NSArray*	childwindows = [self childWindows];
	AItemCount	childCount = [childwindows count];
	//ウインドウレイヤー毎のループ（トップから順に）
	for( AOverlayWindowLayer overlayLayer = kOverlayWindowLayer_Top; overlayLayer >= kOverlayWindowLayer_None; overlayLayer-- )
	{
		//各子ウインドウごとのループ
		for( AIndex i = 0; i < childCount; i++ )
		{
			NSWindow*	w = [childwindows objectAtIndex:i];
			//子ウインドウがマウスイベント無視なら対象外
			if( [w ignoresMouseEvents] == YES )
			{
				continue;
			}
			//子ウインドウがCocoaPanelWindowの場合、子ウインドウ上でhitTest実行し、hitしたらそのviewに対し、mouse moved 処理を行う。
			if( [w isKindOfClass:[CocoaPanelWindow class]] == YES )
			{
				CocoaPanelWindow*	childWindow = w;
				if( [childWindow getOverlayWindowLayer] == overlayLayer )
				{
					//子ウインドウ上の座標を取得
					NSPoint	ptInChildWindow = inGloalPoint;
					NSRect	childWindowFrame = [childWindow frame];
					ptInChildWindow.x -= childWindowFrame.origin.x;
					ptInChildWindow.y -= childWindowFrame.origin.y;
					//子ウインドウ上でhitTest
					NSView*	childContentView = [childWindow contentView];
					NSPoint ptInChildContentView = [childContentView convertPoint:ptInChildWindow fromView:nil];
					NSView*	view = [childContentView hitTest:ptInChildContentView];
					//viewが見つかったらAWindow::EVT_DoMouseMoved()を実行して、リターン
					if( view != nil )
					{
						//window refを取得し、window refからWindowImpを取得
						AWindowRef	winRef = childWindow;//#1290 [childWindow windowNumber];//#1275 (WindowRef)([childWindow windowRef]);
						if( CWindowImp::ExistWindowImpForWindowRef(winRef) == true )
						{
							//Local座標に変換
							NSPoint curPoint = [view convertPoint:ptInChildWindow fromView:nil];
							ALocalPoint	localPoint = {curPoint.x,curPoint.y};
							CWindowImp::GetWindowImpByWindowRef(winRef).APICB_CocoaMouseMoved([view tag],localPoint,modifier);
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}

/**
mouse moved時処理
*/
- (void)mouseMoved:(NSEvent *)inEvent
{
	//ウインドウを閉じた後も数回mouseMovedがくることがあるので、
	//ウインドウがvisibleでなければ、何もせずリターン
	if( [self isVisible] == NO )
	{
		return;
	}
	
	OS_CALLBACKBLOCK_START(false);
	
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent(inEvent);
	
	//マウス位置をグローバル座標に変換
	NSPoint	ptInGlobal = [inEvent locationInWindow];
	NSRect	winFrame = [self frame];
	ptInGlobal.x += winFrame.origin.x;
	ptInGlobal.y += winFrame.origin.y;
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	AGlobalPoint	globalPoint = {0};
	globalPoint.x = ptInGlobal.x;
	globalPoint.y = firstScreenFrame.size.height - ptInGlobal.y;
	//fprintf(stderr,"mouseModed(%d,%d) ",globalPoint.x,globalPoint.y);
	
	//１．フローティングウインドウ上で検索し、フローティング上ならフローティングウインドウ上で実行
	CWindowImp*	floatingWindowImp = CWindowImp::FindFloatingWindowByGlobalPoint(globalPoint);
	if( floatingWindowImp != NULL )
	{
		[floatingWindowImp->GetCocoaObjects().GetWindow() doMouseMovedForThisWindow:ptInGlobal modifierKeys:modifier];
	}
	else
	{
		//２．親ウインドウ（自身が親なら自ウインドウ）上で、子ウインドウ上で該当viewの検索を行い、hitしたら子ウインドウで処理する。
		NSWindow*	rootwin = self;
		if( [self parentWindow] != nil )
		{
			rootwin = [self parentWindow];
		}
		if( [rootwin doMouseMovedForChildWindows:ptInGlobal modifierKeys:modifier] == false )
		{
			//３．子ウインドウ上で未実行なら、自ウインドウで実行
			[self doMouseMovedForThisWindow:ptInGlobal modifierKeys:modifier];
		}
	}
	
	OS_CALLBACKBLOCK_END;
}

/**
mouse moved時処理（他のウインドウでの検索なし。必ず自ウインドウで実行）
*/
- (void)doMouseMovedForThisWindow:(NSPoint)inGloalPoint modifierKeys:(AModifierKeys)modifier
{
	//ウインドウ内ポイントへ変換
	NSPoint	ptInWindow = inGloalPoint;
	NSRect	windowFrame = [self frame];
	ptInWindow.x -= windowFrame.origin.x;
	ptInWindow.y -= windowFrame.origin.y;
	//hitTestを行い、対応するviewを検索
	NSView*	contentView = [self contentView];
	NSPoint ptInContentView = [contentView convertPoint:ptInWindow fromView:nil];
	NSView*	view = [contentView hitTest:ptInContentView];
	if( view != nil )
	{
		//viewが存在する場合
		
		//fprintf(stderr,"%d ",[view tag]);
		
		//Local座標に変換
		NSPoint curPoint = [view convertPoint:ptInWindow fromView:nil];
		ALocalPoint	localPoint = {curPoint.x,curPoint.y};
		//AWindow::EVT_DoMouseMoved()を実行
		mWindowImp->APICB_CocoaMouseMoved([view tag],localPoint,modifier);
	}
	else
	{
		//viewが存在しない場合
		
		ALocalPoint	localPoint = {0,0};
		//modifierKeys取得
		//AWindow::EVT_DoMouseMoved()を実行（第一引数をkControlID_Invalidにする）
		mWindowImp->APICB_CocoaMouseMoved(kControlID_Invalid,localPoint,modifier);
	}
}

/**
ツールバー表示／非表示切替時処理
*/
- (void)toggleToolbarShown:(id)sender
{
	//継承処理
	[super toggleToolbarShown:sender];
	
	OS_CALLBACKBLOCK_START(false);
	
	//bounds変更時処理
	AGlobalRect	newFrameGlobalRect = [self getGlobalRectFromFrame:[self frame]];
	mWindowImp->APICB_CocoaWindowBoundsChanged(newFrameGlobalRect);
	
	OS_CALLBACKBLOCK_END;
}

#pragma mark ===========================

#pragma mark ---Open/Saveシート
//#1034

/**
フォルダ選択ダイアログ／シート表示
*/
- (void)showChooseFolderWindow:(NSString*)dirPath message:(NSString*)message isSheet:(ABool)isSheet
{
	//OpenPanel取得
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel setCanChooseFiles:NO];
	[panel setCanChooseDirectories:YES];
	[panel setResolvesAliases:YES];
	[panel setAllowsMultipleSelection:NO];
	[panel setExtensionHidden:NO];
	[panel setTreatsFilePackagesAsDirectories:YES];
	[panel setShowsHiddenFiles:YES];
	if( message != nil )
	{
		[panel setMessage:message];
	}
	
	//OpenPanel表示
	if( isSheet == false )
	{
		//モーダルダイアログ
		int	returnCode = [panel runModal];
		[self showChooseFolderWindowDidEnd:panel returnCode:returnCode contextInfo:nil];
	}
	else
	{
		//シート
		[panel beginSheetForDirectory:dirPath file:nil modalForWindow:self 
			modalDelegate:self didEndSelector:@selector(showChooseFolderWindowDidEnd:returnCode:contextInfo:) 
		contextInfo:nil];
	}
}

/**
フォルダ選択ダイアログ／シート完了時処理
*/
- (void)showChooseFolderWindowDidEnd:(NSSavePanel *)panel returnCode:(int)returnCode contextInfo:(void *)contextInfo;
{
	OS_CALLBACKBLOCK_START(false);
	
	//OK/Cancel判定
	if( returnCode == NSFileHandlingPanelOKButton )
	{
		//結果取得
		NSArray*	urls = [panel URLs];
		if( [urls count] > 0 )
		{
			NSURL*	url = [urls objectAtIndex:0];
			AText	path;
			path.SetCstring([[url path] UTF8String]);
			//WindowImpへ通知
			mWindowImp->APICB_ShowChooseFolderWindowAction(path);
		}
	}
	else
	{
		//WindowImpへ通知（シート表示状態解除のため）
		mWindowImp->APICB_ShowChooseFolderWindowAction(GetEmptyText());
	}
	
	OS_CALLBACKBLOCK_END;
}

/**
ファイル選択ダイアログ／シート表示
*/
- (void)showChooseFileWindow:(NSString*)dirPath message:(NSString*)message isSheet:(ABool)isSheet onlyApp:(ABool)onlyApp
{
	//★onlyApp未実装
	
	//OpenPanel取得
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel setCanChooseFiles:YES];
	[panel setCanChooseDirectories:NO];
	[panel setResolvesAliases:YES];
	[panel setAllowsMultipleSelection:NO];
	[panel setExtensionHidden:NO];
	[panel setTreatsFilePackagesAsDirectories:YES];
	[panel setShowsHiddenFiles:YES];
	if( message != nil )
	{
		[panel setMessage:message];
	}
	
	//OpenPanel表示
	if( isSheet == false )
	{
		//モーダルダイアログ
		int	returnCode = [panel runModal];
		[self showChooseFileWindowDidEnd:panel returnCode:returnCode contextInfo:nil];
	}
	else
	{
		//シート
		[panel beginSheetForDirectory:dirPath file:nil modalForWindow:self 
			modalDelegate:self didEndSelector:@selector(showChooseFileWindowDidEnd:returnCode:contextInfo:) 
		contextInfo:nil];
	}
}

/**
ファイル選択ダイアログ／シート完了時処理
*/
- (void)showChooseFileWindowDidEnd:(NSSavePanel *)panel returnCode:(int)returnCode contextInfo:(void *)contextInfo;
{
	OS_CALLBACKBLOCK_START(false);
	
	//OK/Cancel判定
	if( returnCode == NSFileHandlingPanelOKButton )
	{
		//結果取得
		NSArray*	urls = [panel URLs];
		if( [urls count] > 0 )
		{
			NSURL*	url = [urls objectAtIndex:0];
			AText	path;
			path.SetCstring([[url path] UTF8String]);
			//WindowImpへ通知
			mWindowImp->APICB_ShowChooseFileWindowAction(path);
		}
	}
	else
	{
		//WindowImpへ通知（シート表示状態解除のため）
		mWindowImp->APICB_ShowChooseFileWindowAction(GetEmptyText());
	}
	
	OS_CALLBACKBLOCK_END;
}

/**
保存シート表示
*/
- (void)showSaveWindow:(NSString*)dirPath filename:(NSString*)filename
{
	//SavePanel取得
	NSSavePanel* panel = [NSSavePanel savePanel];
	[panel setAllowsOtherFileTypes:YES];
	[panel setCanCreateDirectories:YES];
	[panel setCanSelectHiddenExtension:NO];
	[panel setTreatsFilePackagesAsDirectories:YES];
	[panel setShowsHiddenFiles:YES];
	[panel setExtensionHidden:NO];
	//ファイル名設定
	NSString*	fn = nil;
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_6 )
	{
		[panel setNameFieldStringValue:filename];
	}
	else
	{
		//10.6未満の場合はbeginSheetForDirectoryの引数でファイル名指定
		fn = filename;
	}
	
	//シート
	[panel beginSheetForDirectory:dirPath file:fn modalForWindow:self 
		modalDelegate:self didEndSelector:@selector(showSaveWindowDidEnd:returnCode:contextInfo:) 
	contextInfo:nil];
}

/**
保存シート完了時処理
*/
- (void)showSaveWindowDidEnd:(NSSavePanel *)panel returnCode:(int)returnCode contextInfo:(void *)contextInfo;
{
	OS_CALLBACKBLOCK_START(false);
	
	//OK/Cancel判定
	if( returnCode == NSFileHandlingPanelOKButton )
	{
		//結果取得
		NSURL*	url = [panel URL];
		AText	path;
		path.SetCstring([[url path] UTF8String]);
		//WindowImpへ通知
		mWindowImp->APICB_ShowSaveWindowAction(path);
	}
	else
	{
		//WindowImpへ通知（シート表示状態解除のため）
		mWindowImp->APICB_ShowSaveWindowAction(GetEmptyText());
	}
	
	OS_CALLBACKBLOCK_END;
}
#pragma mark ===========================

#pragma mark ---シート終了時処理

/**
シート終了時処理
*/
- (void)didEndSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[self orderOut:nil];
}

#pragma mark ===========================

#pragma mark ---ツールバー関連

/**
ツールバー項目が追加されるときの処理
*/
- (void) toolbarWillAddItem:(NSNotification *)notification
{
	OS_CALLBACKBLOCK_START(false);
	
	mAddingToolbarItem = [[notification userInfo] objectForKey: @"item"];
	
	//ボタンsentAction処理実行
	mWindowImp->APICB_CocoaDoToolbarItemAddedAction([mAddingToolbarItem tag]);
	
	mAddingToolbarItem = nil;
	
	OS_CALLBACKBLOCK_END;
}

/**
ツールバー削除時処理
*/
- (void)toolbarDidRemoveItem:(NSNotification *)notification
{
	OS_CALLBACKBLOCK_START(false);
	
	//ボタンsentAction処理実行
	mWindowImp->APICB_CocoaDoToolbarItemAddedAction(kControlID_Invalid);
	
	OS_CALLBACKBLOCK_END;
}

/**
追加されるツールバー項目を取得
*/
- (NSToolbarItem*)getAddingToolbarItem
{
	return mAddingToolbarItem;
}

/**
指定controlがsegmented controlかどうかを取得
*/
- (BOOL)isSegmentControl:(AControlID)inControlID
{
	AIndex	index = mSegmentedControlArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		return YES;
	}
	return NO;
}

/**
segmented controlに値設定
*/
- (BOOL)setBoolForSegmentControl:(AControlID)inControlID select:(BOOL)inSelect
{
	AIndex	index = mSegmentedControlArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		NSSegmentedControl*	segmentedControl = mSegmentedControlArray_SegmentedControl.Get(index);
		NSInteger	segment = mSegmentedControlArray_Segment.Get(index);
		[[segmentedControl cell] setSelected:inSelect forSegment:segment];
		return YES;
	}
	return NO;
}

/**
segmented controlをenable/disable
*/
- (BOOL)setEnableForSegmentControl:(AControlID)inControlID enable:(BOOL)inEnable
{
	AIndex	index = mSegmentedControlArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		NSSegmentedControl*	segmentedControl = mSegmentedControlArray_SegmentedControl.Get(index);
		NSInteger	segment = mSegmentedControlArray_Segment.Get(index);
		[[segmentedControl cell] setEnabled:inEnable forSegment:segment];
		return YES;
	}
	return NO;
}

/**
segmented controlのenable/disable状態を取得
*/
- (BOOL)getEnableForSegmentControl:(AControlID)inControlID
{
	AIndex	index = mSegmentedControlArray_ControlID.Find(inControlID);
	if( index != kIndex_Invalid )
	{
		NSSegmentedControl*	segmentedControl = mSegmentedControlArray_SegmentedControl.Get(index);
		NSInteger	segment = mSegmentedControlArray_Segment.Get(index);
		return [[segmentedControl cell] isEnabledForSegment:segment];
	}
	return NO;
}





//==================ツールバー設定==================
//アプリ汎用化できていない

/**
ツールバー全項目
*/
- (NSArray *) toolbarAllowedItemIdentifiers: (NSToolbar *) toolbar 
{
	switch( mToolbarType )
	{
	  case 0:
		{
			return [NSArray arrayWithObjects: 
			NSToolbarSpaceItemIdentifier,
			NSToolbarFlexibleSpaceItemIdentifier,
			@"TB_SubWindow",
			@"TB_Document",
			@"TB_TextEncoding",
			@"TB_ReturnCode",
			@"TB_WrapMode",
			@"TB_Mode",
			//@"TB_Diff",
			@"TB_Nav",
			//@"TB_Keyword",
			@"TB_LayoutLeft",
			@"TB_LayoutRight",
			@"TB_Split",
			@"TB_KeyRecord",
			@"TB_ProhibitPopup",
			@"TB_ShowHideFloating",
			@"TB_Pref",
			@"TB_MultiFileFind",
			//@"TB_MultiFileFindExecute",
			@"TB_Find",
			@"TB_Marker",
			@"TB_SearchField",
			@"TB_Commit",
				@"TB_Save",//#1389
				@"TB_UndoRedo",//#1389
				@"TB_FindDialog",//#1389
				@"TB_Copy",//#1400
				@"TB_Cut",//#1400
				@"TB_Paste",//#1400
			nil];
			break;
		}
		//apppref
	  case 1:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999009",
			@"999010",
			@"999001",
			@"999002",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999007",
			nil];
			break;
		}
		//modepref
	  case 2:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999001",
			@"999002",
			@"999011",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999009",
			@"999007",
			@"999010",
			nil];
			break;
		}
	}
	return [[NSArray alloc] init];
}

/**
ツールバーデフォルト項目
*/
- (NSArray *) toolbarDefaultItemIdentifiers: (NSToolbar *) toolbar 
{
	switch( mToolbarType )
	{
	  case 0:
		{
			return [NSArray arrayWithObjects: 
			@"TB_Document",
			@"TB_SubWindow",
			@"TB_TextEncoding",
			@"TB_ReturnCode",
			//@"TB_WrapMode",
			@"TB_Mode",
			//@"TB_Diff",
			//@"TB_Nav",
			//NSToolbarSpaceItemIdentifier,
			@"TB_Split",
			@"TB_Nav",
			//NSToolbarSpaceItemIdentifier,
			//@"TB_KeyRecord",
			NSToolbarFlexibleSpaceItemIdentifier,
			//@"TB_Nav",
			//@"TB_Keyword",
			@"TB_Commit",
			//@"TB_ProhibitPopup",
			//@"TB_ShowHideFloating",
			@"TB_Pref",
			@"TB_LayoutRight",
			NSToolbarSpaceItemIdentifier,
			@"TB_MultiFileFind",
			//@"TB_Find",
			//@"TB_Marker",
			@"TB_SearchField",
			nil];
			break;
		}
		//apppref
	  case 1:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999009",
			@"999010",
			@"999001",
			@"999002",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999007",
			nil];
			break;
		}
		//modepref
	  case 2:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999001",
			@"999002",
			@"999011",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999009",
			@"999007",
			@"999010",
			nil];
			break;
		}
	}
	return [[NSArray alloc] init];
}

/**
選択可能ツールバー項目
*/
- (NSArray *)toolbarSelectableItemIdentifiers: (NSToolbar *)toolbar
{
	switch( mToolbarType )
	{
		//apppref
	  case 1:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999009",
			@"999010",
			@"999001",
			@"999002",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999007",
			nil];
			break;
		}
		//modepref
	  case 2:
		{
			return [NSArray arrayWithObjects: 
			@"999000",
			@"999001",
			@"999002",
			@"999011",
			@"999003",
			@"999004",
			@"999005",
			@"999006",
			@"999008",
			@"999009",
			@"999007",
			@"999010",
			nil];
			break;
		}
	}
	return [[NSArray alloc] init];
}

//各項目サイズ
const ANumber	kWidth_Toolbar1Button = 34;
const ANumber	kWidth_Toolbar2Button = 80;
const ANumber	kWidth_Toolbar3Button = 120;
const ANumber	kWidth_Toolbar4Button = 160;
const ANumber	kHeight_ToolbarButton = 25;//24以下にするとボタン位置がずれる？
const ANumber	kWidth_ToolbarImageButton = 32;
const ANumber	kHeight_ToolbarImageButton = 32;

/**
ツールバー項目（イメージボタン）生成
*/
-(NSToolbarItem*)createToolbarItem_ImageButton:(NSString *)itemIdentifier 
tag:(NSInteger)tag image:(NSImage*)image labelText:(const char*)labelText
toggleMode:(BOOL)toggleMode
{
	
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	NSRect	rect = {0};
	rect.size.width = kWidth_ToolbarImageButton;
	rect.size.height = kHeight_ToolbarImageButton;
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_ToolbarImageButton;
	minsize.height = kHeight_ToolbarImageButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_ToolbarImageButton;
	maxsize.height = kHeight_ToolbarImageButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	[toolbarItem setTag:tag];
	[toolbarItem setImage:image];
	return toolbarItem;
}

/**
ツールバー項目（１ボタン）生成
*/
-(NSToolbarItem*)createToolbarItem_Button:(NSString *)itemIdentifier 
tag:(NSInteger)tag image:(NSImage*)image labelText:(const char*)labelText
toggleMode:(BOOL)toggleMode
{
	
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rectの値を設定しても意味が無いようだが、念のため幅と高さを設定する
	NSRect	rect = {0};
	rect.size.width = kWidth_Toolbar1Button;
	rect.size.height = kHeight_ToolbarButton;
	//ボタン生成
	NSButton*	button = [[NSButton alloc] initWithFrame:rect];
	//
	if( toggleMode == YES )
	{
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
		{
			[button setButtonType:NSSwitchButton];
		}
		else
		{
			[button setButtonType:NSPushOnPushOffButton];
		}
		[button setTitle:@""];
	}
	else
	{
		[button setButtonType:NSMomentaryPushInButton];
	}
	[button setImagePosition:NSImageOnly];
	//
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
	{
		[button setBezelStyle:NSTexturedRoundedBezelStyle];
	}
	else
	{
		[button setBezelStyle:NSRegularSquareBezelStyle];
	}
	[button setTag:tag];
	[button setImage:image];
	[button setBordered:YES];
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_Toolbar1Button;
	minsize.height = kHeight_ToolbarButton-2;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_Toolbar1Button;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:button];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	[toolbarItem setTag:tag];
	return toolbarItem;
	
	/*
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rectの値を設定しても意味が無いようだが、念のため幅と高さを設定する
	NSRect	rect = {0};
	rect.size.width = kWidth_Toolbar1Button-16;
	rect.size.height = kHeight_ToolbarButton;
	//
	NSSegmentedControl*	segment = [[NSSegmentedControl alloc] initWithFrame:rect];
	[segment setSegmentCount:1];
	[[segment cell] setTag:tag forSegment:0];
	[segment setImage:image forSegment:0];
	//
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		[segment setSegmentStyle:NSSegmentStyleAutomatic];
	}
	[[segment cell] setTrackingMode:NSSegmentSwitchTrackingMomentary];
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_Toolbar1Button;
	minsize.height = kHeight_ToolbarButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_Toolbar1Button;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setView:segment];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	return toolbarItem;
	*/
}

/**
ツールバー項目（２ボタン）生成
*/
-(NSToolbarItem*)createToolbarItem_2Buttons:(NSString *)itemIdentifier 
tag1:(NSInteger)tag1 image1:(NSImage*)image1
tag2:(NSInteger)tag2 image2:(NSImage*)image2
labelText:(const char*)labelText
{
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rectの値を設定しても意味が無いようだが、念のため幅と高さを設定する
	NSRect	rect = {0};
	rect.size.width = kWidth_Toolbar2Button;
	rect.size.height = kHeight_ToolbarButton;
	//
	NSSegmentedControl*	segment = [[NSSegmentedControl alloc] initWithFrame:rect];
	[segment setSegmentCount:2];
	[[segment cell] setTag:tag1 forSegment:0];
	[[segment cell] setTag:tag2 forSegment:1];
	[segment setImage:image1 forSegment:0];
	[segment setImage:image2 forSegment:1];
	[segment setWidth:0 forSegment:0];
	[segment setWidth:0 forSegment:1];
	//
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		[segment setSegmentStyle:NSSegmentStyleAutomatic];
	}
	[[segment cell] setTrackingMode:NSSegmentSwitchTrackingMomentary];
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_Toolbar2Button;
	minsize.height = kHeight_ToolbarButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_Toolbar2Button;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:segment];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	//
	mSegmentedControlArray_ControlID.Add(tag1);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(0);
	//
	mSegmentedControlArray_ControlID.Add(tag2);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(1);
	return toolbarItem;
}

/**
ツールバー項目（３ボタン）生成
*/
-(NSToolbarItem*)createToolbarItem_3Buttons:(NSString *)itemIdentifier 
tag1:(NSInteger)tag1 image1:(NSImage*)image1
tag2:(NSInteger)tag2 image2:(NSImage*)image2
tag3:(NSInteger)tag3 image3:(NSImage*)image3
labelText:(const char*)labelText
{
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rectの値を設定しても意味が無いようだが、念のため幅と高さを設定する
	NSRect	rect = {0};
	rect.size.width = kWidth_Toolbar3Button;
	rect.size.height = kHeight_ToolbarButton;
	//
	NSSegmentedControl*	segment = [[NSSegmentedControl alloc] initWithFrame:rect];
	[segment setSegmentCount:3];
	[[segment cell] setTag:tag1 forSegment:0];
	[[segment cell] setTag:tag2 forSegment:1];
	[[segment cell] setTag:tag3 forSegment:2];
	[segment setImage:image1 forSegment:0];
	[segment setImage:image2 forSegment:1];
	[segment setImage:image3 forSegment:2];
	//
	[[segment cell] setTrackingMode:NSSegmentSwitchTrackingMomentary];
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		[segment setSegmentStyle:NSSegmentStyleAutomatic];
	}
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_Toolbar3Button;
	minsize.height = kHeight_ToolbarButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_Toolbar3Button;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:segment];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	//
	mSegmentedControlArray_ControlID.Add(tag1);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(0);
	//
	mSegmentedControlArray_ControlID.Add(tag2);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(1);
	//
	mSegmentedControlArray_ControlID.Add(tag3);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(2);
	return toolbarItem;
}

/**
ツールバー項目（４ボタン）生成
*/
-(NSToolbarItem*)createToolbarItem_4Buttons:(NSString *)itemIdentifier 
tag1:(NSInteger)tag1 image1:(NSImage*)image1
tag2:(NSInteger)tag2 image2:(NSImage*)image2
tag3:(NSInteger)tag3 image3:(NSImage*)image3
tag4:(NSInteger)tag4 image4:(NSImage*)image4
labelText:(const char*)labelText
toggleMode:(BOOL)toggleMode
{
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rectの値を設定しても意味が無いようだが、念のため幅と高さを設定する
	NSRect	rect = {0};
	rect.size.width = kWidth_Toolbar4Button;
	rect.size.height = kHeight_ToolbarButton;
	//
	NSSegmentedControl*	segment = [[NSSegmentedControl alloc] initWithFrame:rect];
	[segment setSegmentCount:4];
	[[segment cell] setTag:tag1 forSegment:0];
	[[segment cell] setTag:tag2 forSegment:1];
	[[segment cell] setTag:tag3 forSegment:2];
	[[segment cell] setTag:tag4 forSegment:3];
	[segment setImage:image1 forSegment:0];
	[segment setImage:image2 forSegment:1];
	[segment setImage:image3 forSegment:2];
	[segment setImage:image4 forSegment:3];
	/*
	//
	[[segment cell] setToolTip:@"test0" forSegment:0];
	[[segment cell] setToolTip:@"test1" forSegment:1];
	[[segment cell] setToolTip:@"test2" forSegment:2];
	[[segment cell] setToolTip:@"test3" forSegment:3];
	*/
	//
	if( toggleMode == YES )
	{
		[[segment cell] setTrackingMode:NSSegmentSwitchTrackingSelectOne];
	}
	else
	{
		[[segment cell] setTrackingMode:NSSegmentSwitchTrackingMomentary];
	}
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		[segment setSegmentStyle:NSSegmentStyleAutomatic];
	}
	//
	NSSize	minsize = {0};
	minsize.width = kWidth_Toolbar4Button;
	minsize.height = kHeight_ToolbarButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = kWidth_Toolbar4Button;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:segment];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	//
	mSegmentedControlArray_ControlID.Add(tag1);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(0);
	//
	mSegmentedControlArray_ControlID.Add(tag2);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(1);
	//
	mSegmentedControlArray_ControlID.Add(tag3);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(2);
	//
	mSegmentedControlArray_ControlID.Add(tag4);
	mSegmentedControlArray_SegmentedControl.Add(segment);
	mSegmentedControlArray_Segment.Add(3);
	return toolbarItem;
}

/**
ツールバー項目（ポップアップボタン）生成
*/
-(NSToolbarItem*)createToolbarItem_PopupButton:(NSString *)itemIdentifier  width:(ANumber)width
tag:(NSInteger)tag image:(NSImage*)image labelText:(const char*)labelText
{
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//ボタンのframeサイズを設定
	NSRect	rect = {0};
	rect.size.width = width;
	rect.size.height = kHeight_ToolbarButton;
	//ポップアップボタン生成
	NSPopUpButton*	button = [[[NSPopUpButton alloc] initWithFrame:rect pullsDown:YES] autorelease];
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
	{
		[button setBezelStyle:NSTexturedRoundedBezelStyle];
	}
	else
	{
		[button setBezelStyle:NSRoundedBezelStyle];
	}
	[button setTag:tag];
	[button setBordered:YES];
	[[button cell] setArrowPosition:NSPopUpArrowAtBottom];
	//enable/disableは手動更新
	[[button menu] setAutoenablesItems:NO];
	//
	NSMenuItem*	menuItem = [ACocoa::GetNSMenu([button menu]) insertItemWithTitle:@"" action:nil keyEquivalent:@"" atIndex:0];
	if( image != nil )
	{
		[menuItem setImage:image];
	}
	/*setView時にボタンのサイズに合わせてツールバー項目のサイズは設定される。
	NSSize	minsize = {0};
	minsize.width = width;
	minsize.height = kHeight_ToolbarButton;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = width;
	maxsize.height = kHeight_ToolbarButton;
	[toolbarItem setMaxSize:maxsize];
	*/
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:button];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	[toolbarItem setTag:tag];
	return toolbarItem;
}

/**
ツールバー項目（検索ボックス）生成
*/
-(NSToolbarItem*)createToolbarItem_SearchBox:(NSString *)itemIdentifier  minwidth:(ANumber)minwidth maxwidth:(ANumber)maxwidth
tag:(NSInteger)tag labelText:(const char*)labelText
{
	AText	label;
	label.SetLocalizedText(labelText);
	AStCreateNSStringFromAText	labelstr(label);
	//
	NSToolbarItem*	toolbarItem = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];;
	//
	//rectの値を設定しても意味が無いようだが、念のため幅と高さを設定する
	NSRect	rect = {0};
	rect.size.width = maxwidth;
	rect.size.height = kHeight_ToolbarButton-1;
	//サーチフィールド生成
	NSSearchField*	searchfield = [[[NSSearchField alloc] initWithFrame:rect] autorelease];
	[[searchfield cell] setSendsWholeSearchString:YES];
	[searchfield setTag:tag];
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == false )
	{
		[[searchfield cell] setRecentsAutosaveName:labelstr.GetNSString()];
		[[searchfield cell] setMaximumRecents:20];
		
		NSMenu *cellMenu = [[NSMenu alloc] initWithTitle:@"SearchBox"];
		
		AText	recentTitleMenuItemText;
		recentTitleMenuItemText.SetLocalizedText("SearchBoxMenu_Recent");
		AStCreateNSStringFromAText	recentTitleMenuItemStr(recentTitleMenuItemText);
		NSMenuItem*	recentTitleMenuItem = [[NSMenuItem alloc] initWithTitle:recentTitleMenuItemStr.GetNSString() 
																	 action:NULL keyEquivalent:@""];
		[recentTitleMenuItem setTag:NSSearchFieldRecentsTitleMenuItemTag];
		[cellMenu insertItem:recentTitleMenuItem atIndex:0];
		
		AText	recentSearchMenuItemText;
		recentSearchMenuItemText.SetLocalizedText("SearchBoxMenu_Recent");
		AStCreateNSStringFromAText	recentSearchMenuItemStr(recentSearchMenuItemText);
		NSMenuItem*	recentSearchMenuItem = [[NSMenuItem alloc] initWithTitle:recentSearchMenuItemStr.GetNSString() 
																	  action:NULL keyEquivalent:@""];
		[recentSearchMenuItem setTag:NSSearchFieldRecentsMenuItemTag];
		[cellMenu insertItem:recentSearchMenuItem atIndex:1];
		
		NSMenuItem*	separatorMenuItem = [NSMenuItem separatorItem];
		[separatorMenuItem setTag:NSSearchFieldRecentsTitleMenuItemTag];
		[cellMenu insertItem:separatorMenuItem atIndex:2];
		
		AText	clearMenuItemText;
		clearMenuItemText.SetLocalizedText("SearchBoxMenu_Clear");
		AStCreateNSStringFromAText	clearMenuItemStr(clearMenuItemText);
		NSMenuItem*	clearMenuItem = [[NSMenuItem alloc] initWithTitle:clearMenuItemStr.GetNSString() 
															   action:NULL keyEquivalent:@""];
		[clearMenuItem setTag:NSSearchFieldClearRecentsMenuItemTag];
		[cellMenu insertItem:clearMenuItem atIndex:3];
		
		[[searchfield cell] setSearchMenuTemplate:cellMenu];
	}
	NSSize	minsize = {0};
	minsize.width = minwidth;
	minsize.height = kHeight_ToolbarButton-1;
	[toolbarItem setMinSize:minsize];
	NSSize	maxsize = {0};
	maxsize.width = maxwidth;
	maxsize.height = kHeight_ToolbarButton-1;
	[toolbarItem setMaxSize:maxsize];
	[toolbarItem setLabel:labelstr.GetNSString()];
	[toolbarItem setToolTip:labelstr.GetNSString()];
	[toolbarItem setPaletteLabel:labelstr.GetNSString()];
	[toolbarItem setView:searchfield];
	[toolbarItem setTarget:self];
	[toolbarItem setAction:@selector(doToolbarItemAction:)];
	[toolbarItem setTag:tag];
	return toolbarItem;
}

/**
ツールバー項目生成
*/
-(NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag
{
	switch( mToolbarType )
	{
	  case 0:
		{
			if( [itemIdentifier compare:@"TB_TextEncoding"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:110 tag:995001 image:nil labelText:"TB_TextEncoding"];
			}
			if( [itemIdentifier compare:@"TB_ReturnCode"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:110 tag:995002 image:nil labelText:"TB_ReturnCode"];
			}
			if( [itemIdentifier compare:@"TB_WrapMode"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:110 tag:995003 image:nil labelText:"TB_WrapMode"];
			}
			if( [itemIdentifier compare:@"TB_Mode"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:110 tag:995004 image:nil labelText:"TB_Mode"];
			}
			if( [itemIdentifier compare:@"TB_Diff"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:110 tag:995005 image:nil labelText:"TB_Diff"];
			}
			if( [itemIdentifier compare:@"TB_Document"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:42 tag:995013 
				image:[NSImage imageNamed:@"iconTbDocument.Template"] labelText:"TB_Document"];
			}
			if( [itemIdentifier compare:@"TB_SubWindow"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:42 tag:995027
				image:[NSImage imageNamed:@"iconTbSubwindows.Template"] labelText:"TB_SubWindow"];
				//pdfだとアイコンが表示されないので当面pngにしておく（pdf: iconTbSubwindows.Template）→解決したのでpdfにする。
			}
			if( [itemIdentifier compare:@"TB_Pref"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:42 tag:995012
				image:[NSImage imageNamed:@"iconTbPreference.Template"] labelText:"TB_Pref"];
			}
			if( [itemIdentifier compare:@"TB_MultiFileFind"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995014 
				image:[NSImage imageNamed:@"iconTbFolderMagnifier.Template"]
				labelText:"TB_MultiFileFind" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_MultiFileFindExecute"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995033 
				image:[NSImage imageNamed:@"iconTbFolderMagnifier.Template"]
				labelText:"TB_MultiFileFindExecute" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_Find"] == NSOrderedSame )
			{
				return [self createToolbarItem_2Buttons:itemIdentifier 
				tag1:995016 image1:[NSImage imageNamed:@"iconTbMagnifierUp.Template"]
				tag2:995015 image2:[NSImage imageNamed:@"iconTbMagnifierDown.Template"]
				labelText:"TB_Find"];
			}
			if( [itemIdentifier compare:@"TB_Marker"] == NSOrderedSame )
			{
				return [self createToolbarItem_PopupButton:itemIdentifier width:42 tag:995017 
				image:[NSImage imageNamed:@"iconTbMarkerPen.Template"] labelText:"TB_Marker"];
			}
			if( [itemIdentifier compare:@"TB_Nav"] == NSOrderedSame )
			{
				return [self createToolbarItem_2Buttons:itemIdentifier 
				tag1:995018 image1:[NSImage imageNamed:@"NSGoLeftTemplate"]//NSGoLeftTemplate"]
				tag2:995019 image2:[NSImage imageNamed:@"NSGoRightTemplate"]//NSGoRightTemplate"]
				labelText:"TB_Nav"];
			}
			if( [itemIdentifier compare:@"TB_Keyword"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995020 
				image:[NSImage imageNamed:@"iconTbKey.Template"]
				labelText:"TB_Keyword" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_Commit"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995021 
				image:[NSImage imageNamed:@"iconTbDiscUp.Template"]
				labelText:"TB_Commit" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_LayoutLeft"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier 
				tag:995022 image:[NSImage imageNamed:@"iconTbSidebarLeft.Template"]
				labelText:"TB_LayoutLeft" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_LayoutRight"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier 
				tag:995025 image:[NSImage imageNamed:@"iconTbSidebarRight.Template"]
				labelText:"TB_LayoutRight" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_Split"] == NSOrderedSame )
			{
				return [self createToolbarItem_2Buttons:itemIdentifier 
				tag1:995023 image1:[NSImage imageNamed:@"iconTbSeparateHorizontal.Template"]
				tag2:995024 image2:[NSImage imageNamed:@"iconTbSeparateVertical.Template"]
				labelText:"TB_Split"];
			}
			if( [itemIdentifier compare:@"TB_SearchField"] == NSOrderedSame )
			{
				return [self createToolbarItem_SearchBox:itemIdentifier minwidth:50 maxwidth:200 tag:995101
				labelText:"TB_SearchField"];
			}
			if( [itemIdentifier compare:@"TB_KeyRecord"] == NSOrderedSame )
			{
				return [self createToolbarItem_4Buttons:itemIdentifier 
				tag1:995029 image1:[NSImage imageNamed:@"iconTbKeyRecord.Template"]
				tag2:995030 image2:[NSImage imageNamed:@"iconTbKeyStop.Template"]
				tag3:995031 image3:[NSImage imageNamed:@"iconTbKeyPlay.Template"]
				tag4:995028 image4:[NSImage imageNamed:@"iconTbCircleCursorButton.Template"]
				labelText:"TB_KeyRecord" toggleMode:YES];
			}
			/*
	if( [itemIdentifier compare:@"TB_ModePref"] == NSOrderedSame )
	{
		return [self createToolbarItem_Button:itemIdentifier tag:995011 
		image:[NSImage imageNamed:@"iconTbModePreference.Template"]
		labelText:"TB_ModePref" toggleMode:NO];
	}
	if( [itemIdentifier compare:@"TB_AppPref"] == NSOrderedSame )
	{
		return [self createToolbarItem_Button:itemIdentifier tag:995012 
		image:[NSImage imageNamed:@"iconTbPreference.Template"]
		labelText:"TB_AppPref" toggleMode:NO];
	}
	*/
			if( [itemIdentifier compare:@"TB_ProhibitPopup"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995032 
				image:[NSImage imageNamed:@"iconTbProhibitPopup.Template"]
				labelText:"TB_ProhibitPopup" toggleMode:YES];
			}
			if( [itemIdentifier compare:@"TB_ShowHideFloating"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995034 
				image:[NSImage imageNamed:@"iconTbProhibitPopup.Template"]
				labelText:"TB_ShowHideFloating" toggleMode:YES];
			}
			//#1389
			if( [itemIdentifier compare:@"TB_Save"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995035 
					image:[NSImage imageNamed:@"iconTbSave.Template"]
				labelText:"TB_Save" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_UndoRedo"] == NSOrderedSame )
			{
				return [self createToolbarItem_2Buttons:itemIdentifier 
					tag1:995036 image1:[NSImage imageNamed:@"iconTbUndo.Template"]
					tag2:995037 image2:[NSImage imageNamed:@"iconTbRedo.Template"]
				labelText:"TB_UndoRedo"];
			}
			if( [itemIdentifier compare:@"TB_FindDialog"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995038 
					image:[NSImage imageNamed:@"iconTbFind.Template"]
				labelText:"TB_FindDialog" toggleMode:NO];
			}
			//#1400
			if( [itemIdentifier compare:@"TB_Copy"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995039 
					image:[NSImage imageNamed:@"iconTbCopy.Template"]
				labelText:"TB_Copy" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_Cut"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995040 
					image:[NSImage imageNamed:@"iconTbCut.Template"]
				labelText:"TB_Cut" toggleMode:NO];
			}
			if( [itemIdentifier compare:@"TB_Paste"] == NSOrderedSame )
			{
				return [self createToolbarItem_Button:itemIdentifier tag:995041 
					image:[NSImage imageNamed:@"iconTbPaste.Template"]
				labelText:"TB_Paste" toggleMode:NO];
			}
			break;
		}
	  case 1:
		{
			//
			if( [itemIdentifier compare:@"999000"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999000 
				image:[NSImage imageNamed:@"NSPreferencesGeneral"]
				labelText:"AppPref_General" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999001"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999001 
				image:[NSImage imageNamed:@"iconEnvSubwindow"]
				labelText:"AppPref_SubWindows" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999002"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999002 
				image:[NSImage imageNamed:@"iconEnvText"]
				labelText:"AppPref_Text" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999003"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999003 
				image:[NSImage imageNamed:@"iconEnvMagnifier"]
				labelText:"AppPref_Find" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999004"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999004 
				image:[NSImage imageNamed:@"iconEnvRemoteConnection"]
				labelText:"AppPref_RemoteFile" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999005"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999005 
				image:[NSImage imageNamed:@"iconEnvCompareDocument"]
				labelText:"AppPref_Compare" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999006"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999006 
				image:[NSImage imageNamed:@"iconModePrinter"]
				labelText:"AppPref_Print" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999007"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999007 
				image:[NSImage imageNamed:@"NSAdvanced"]
				labelText:"AppPref_Detail" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999008"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999008 
				image:[NSImage imageNamed:@"iconModeInputSupport"]
				labelText:"AppPref_Features" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999009"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999009 
					image:[NSImage imageNamed:@"iconModeView"]
				labelText:"AppPref_Display" toggleMode:NO];
			}
			//#1373
			if( [itemIdentifier compare:@"999010"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999010 
					image:[NSImage imageNamed:@"iconEnvOperation"]
				labelText:"AppPref_Operation" toggleMode:NO];
			}
			break;
		}
	  case 2:
		{
			//
			if( [itemIdentifier compare:@"999000"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999000 
				image:[NSImage imageNamed:@"iconModeMode"]
				labelText:"ModePref_General" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999011"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999011 
				image:[NSImage imageNamed:@"NSColorPanel"]
				labelText:"ModePref_Colors" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999002"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999002 
				image:[NSImage imageNamed:@"iconModeEdit"]
				labelText:"ModePref_Edit" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999001"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999001 
				image:[NSImage imageNamed:@"iconModeView"]
				labelText:"ModePref_Display" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999003"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999003 
				image:[NSImage imageNamed:@"iconModeKey"]
				labelText:"ModePref_Keyword" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999004"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999004 
				image:[NSImage imageNamed:@"iconModeHeading"]
				labelText:"ModePref_Header" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999005"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999005 
				image:[NSImage imageNamed:@"iconModeSyntax"]
				labelText:"ModePref_Syntax" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999007"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999007 
				image:[NSImage imageNamed:@"iconModeInputSupport"]
				labelText:"ModePref_Features" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999006"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999006 
				image:[NSImage imageNamed:@"iconModeIndent"]
				labelText:"ModePref_Indent" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999008"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999008 
				image:[NSImage imageNamed:@"iconModeKeybind"]
				labelText:"ModePref_KeyBind" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999009"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999009 
				image:[NSImage imageNamed:@"iconModeTool"]
				labelText:"ModePref_Tool" toggleMode:NO];
			}
			//
			if( [itemIdentifier compare:@"999010"] == NSOrderedSame )
			{
				return [self createToolbarItem_ImageButton:itemIdentifier tag:999010 
				image:[NSImage imageNamed:@"NSAdvanced"]
				labelText:"ModePref_Advanced" toggleMode:NO];
			}
			break;
		}
	}
	return nil;
}

@end

