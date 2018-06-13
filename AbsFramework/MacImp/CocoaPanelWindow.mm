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

CocoaPanelWindow

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

@implementation CocoaPanelWindow

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

#pragma mark ---シート終了時処理

/**
シート終了時処理
*/
- (void)didEndSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[self orderOut:nil];
}

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

@end

