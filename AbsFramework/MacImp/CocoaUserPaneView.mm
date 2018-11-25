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

CocoaUserPaneView

*/

#import "CocoaUserPaneView.h"
#import "AppDelegate.h"
#import "CocoaWindow.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

#include "CWindowImpCocoaObjects.h"

extern NSRect	ConvertFromWindowRectToNSRect( NSView* inContentView, const AWindowRect inWindowRect );
extern NSPoint	ConvertFromWindowPointToNSPoint( NSView* inContentView, const AWindowPoint inWindowPoint );



@implementation CocoaUserPaneView

#pragma mark ===========================
#pragma mark [クラス]CocoaUserPaneView
#pragma mark ===========================

//現在のInlineInput変換中テキスト格納string最大長
const AItemCount	kCurrentInlineInputStringMax = 65536;

#pragma mark ===========================

#pragma mark ---初期設定

/**
初期設定
*/
- (id)initWithFrame:(NSRect)inFrame windowImp:(CWindowImp*)inWindowImp controlID:(AControlID)inControlID
{
	//WindowImp, ControlID設定
	mWindowImp = inWindowImp;
	mControlID = inControlID;
	//
	mNeedsPanelToBecomeKey = NO;
	//
	mIsOpaque = NO;
	//縦書きモード #558
	mVerticalMode = NO;
	//サービスメニュー #1309
	mServiceMenuAvailable = NO;
	//継承処理実行
	self = [super initWithFrame:inFrame];
	if( self != nil )
	{
		//デフォルトではフォーカスされないようにする
		mAcceptFocus = NO;
		//デフォルトでは、deactivateウインドウをクリックしたときの最初のマウスクリックは処理しない
		mAcceptsFirstMouse = NO;
		//キー処理フラグ初期化
		//#1015 mKeyHandled = false;
		//現在のInlineInput変換中テキスト格納string初期化
		mCurrentInlineInputString = [[NSMutableString alloc] initWithCapacity:kCurrentInlineInputStringMax];
		//InlineInput用選択位置格納変数初期化
		mSelectedRange = NSMakeRange(0, 0);
		//InlineInput用Markedテキスト領域変数初期化
		mMarkedRange = NSMakeRange(NSNotFound, 0);
		//選択範囲リセット抑制フラグ初期化
		mSuppressResetSelectedRange = false;
	}
	return self;
}

/**
デストラクタ
*/
- (void)dealloc
{
	//現在のInlineInput変換中テキスト解放
	if( mCurrentInlineInputString != nil )
	{
		[mCurrentInlineInputString release];
	}
	
	//継承処理実行
	[super dealloc];
}

/**
フォーカス可否を設定
*/
- (void)setAcceptFocus:(ABool)inAcceptFocus
{
	if( inAcceptFocus == true )
	{
		mAcceptFocus = YES;
	}
	else
	{
		mAcceptFocus = NO;
	}
}

/**
deactivateウインドウをクリックしたときの最初のマウスクリックを処理するかどうかを設定
*/
- (void)setAcceptsFirstMouse:(ABool)inAcceptsFirstMouse
{
	if( inAcceptsFirstMouse == true )
	{
		mAcceptsFirstMouse = YES;
	}
	else
	{
		mAcceptsFirstMouse = NO;
	}
}

/**
*/
- (void)setNeedsPanelToBecomeKey:(ABool)inNeedsPanelToBecomeKey
{
	if( inNeedsPanelToBecomeKey == true )
	{
		mNeedsPanelToBecomeKey = YES;
	}
	else
	{
		mNeedsPanelToBecomeKey = NO;
	}
}

//#1090
/**
不透明かどうかを設定
*/
- (void)setOpaque:(ABool)inOpaque
{
	if( inOpaque == true )
	{
		mIsOpaque = YES;
	}
	else
	{
		mIsOpaque = NO;
	}
}

//#558
/**
縦書きモード設定
*/
- (void)setVerticalMode:(ABool)inVertical
{
	if( inVertical == true )
	{
		mVerticalMode = YES;
	}
	else
	{
		mVerticalMode = NO;
	}
}

//#1309
/**
サービスメニュー可否
可に設定した場合のみ、validRequestorForSendTypeでselfを返す（＝メインメニューやコンテキストメニューにサービスメニューが表示される）
*/
- (void)setServiceMenuAvailable:(ABool)inAvailable
{
	if( inAvailable == true )
	{
		mServiceMenuAvailable = YES;
	}
	else
	{
		mServiceMenuAvailable = NO;
	}
}

//#1309
/**
サービスメニュー　有効なタイプかどうかの判定
*/
- (id)validRequestorForSendType:(NSString *)sendType returnType:(NSString *)returnType
{
	if( mServiceMenuAvailable == YES )
	{
		return self;
	}
	else
	{
		return [super validRequestorForSendType:sendType returnType:returnType];
	}
}

//#1309
/**
サービスメニュー（mi→サービス）
*/
- (BOOL)writeSelectionToPasteboard:(NSPasteboard *)pboard types:(NSArray *)types
{
	/*
	if ([types containsObject:NSPasteboardTypeString] == NO && [types containsObject:NSStringPboardType] == NO)
	{
		return NO;
	}
	*/
	ABool	result = false;
	
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,NO);
	
	NSArray*	typesDeclared = [NSArray arrayWithObject:NSPasteboardTypeString];
	[pboard declareTypes:typesDeclared owner:nil];
	
	//AWindow::EVT_DoServiceCopy()実行
	result = mWindowImp->APICB_CocoaDoWriteSelectionToPasteboard((AScrapRef)pboard);
	
	OS_CALLBACKBLOCK_END;
	
	return (result==true)?YES:NO;
}

//#1309
/**
サービスメニュー（サービス→mi）
*/
- (BOOL)readSelectionFromPasteboard:(NSPasteboard *)pboard
{
	/*
	NSArray*	types = [pboard types];
	if ([types containsObject:NSPasteboardTypeString] == NO && [types containsObject:NSStringPboardType] == NO)
	{
		return NO;
	}
	*/
	ABool	result = false;
	
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,NO);
	
	//AWindow::EVT_DoServicePaste()実行
	result = mWindowImp->APICB_CocoaDoReadSelectionFromPasteboard((AScrapRef)pboard);
	
	OS_CALLBACKBLOCK_END;
	
	return (result==true)?YES:NO;
}

#pragma mark ===========================

#pragma mark ---マウスtracking rect設定

/**
マウスtracking rect設定解除
*/
- (void)clearAllMouseTrackingRect
{
	if( [self window] != nil )
	{
		//updateMouseTrackingRectで登録した各rectを解除する
		for( AIndex i = 0; i < mTrackingRectTagArray.GetItemCount(); i++ )
		{
			[self removeTrackingRect:mTrackingRectTagArray.Get(i)];
		}
		mTrackingRectTagArray.DeleteAll();
	}
}

/**
マウスtracking rect更新
*/
- (void)updateMouseTrackingRect
{
	if( [self window] != nil )
	{
		//現在登録中のrectを全て解除
		[self clearAllMouseTrackingRect];
		//コントロール全体のrectを登録
		NSRect	rect = {0};
		rect.size = [self frame].size;
		mTrackingRectTagArray.Add([self addTrackingRect:rect owner:self userData:nil assumeInside:NO]);
		/*ウインドウのmouse movedイベントで対応するため、view内のrectを登録する方法は使わない。
		この方法だと、スクロールするリストの各項目のホバーアクションを実現できない。
		for( AIndex i = 0; i < mAdditionalTrackingRectArray.GetItemCount(); i++ )
		{
			ALocalRect	localRect = mAdditionalTrackingRectArray.Get(i);
			NSRect	rect = {0};
			rect.origin.x		= localRect.left;
			rect.origin.y		= localRect.top;
			rect.size.width		= localRect.right - localRect.left;
			rect.size.height	= localRect.bottom - localRect.top;
			mTrackingRectTagArray.Add([self addTrackingRect:rect owner:self userData:nil assumeInside:NO]);
		}
		*/
	}
}

/*ウインドウのmouse movedイベントで対応するため、view内のrectを登録する方法は使わない。
		この方法だと、スクロールするリストの各項目のホバーアクションを実現できない。
- (void)setMouseTrackingRect:(const AArray<ALocalRect>&)inLocalRectArray
{
	//
	mAdditionalTrackingRectArray.DeleteAll();
	for( AIndex i = 0; i < inLocalRectArray.GetItemCount(); i++ )
	{
		mAdditionalTrackingRectArray.Add(inLocalRectArray.Get(i));
	}
	//
	[self updateMouseTrackingRect];
}
*/

#pragma mark ===========================

#pragma mark ---Viewのデータ取得

/**
Y方向座標を上下変換するかどうか（YESを返す）
*/
- (BOOL)isFlipped
{
	//YESを返すことにより左上が原点になる。
	//ただし、テキスト描画等の上下反転はここでYESを返すだけでは実施されない。
	//CUserPane::APICB_CocoaDrawRect()にてCGContextScaleCTM()を実行することで上下反転する。
	//（従来CarbonでのCGContext処理と同じ。yはマイナス方向へ描画することになる。）
	return YES;
}

/**
不透明viewかどうか
*/
- (BOOL)isOpaque
{
	return mIsOpaque;
}

/**
tag(=ControlID)を返す
*/
- (NSInteger)tag
{
	return mControlID;
}

/**
KeyView可否
*/
- (BOOL)canBecomeKeyView
{
	//このクラスで記憶しているフォーカス可否の値を返す
	return mAcceptFocus;
}

/**
FirstResponder可否
*/
- (BOOL)acceptsFirstResponder
{
	//このクラスで記憶しているフォーカス可否の値を返す
	return mAcceptFocus;
}

/**
deactivateウインドウをクリックしたときの最初のマウスクリックを処理するかどうか
*/
- (BOOL)acceptsFirstMouse:(NSEvent *)inEvent
{
   return mAcceptsFirstMouse;
}

/**
doCommandBySelector
*/
- (void)doCommandBySelector:(SEL)aSelector
{
	//継承処理実行
	[super doCommandBySelector:aSelector];
	//継承処理以外の処理なしだが、デバッグにも便利なので、残しておく。
}

/**
*/
- (BOOL)needsPanelToBecomeKey
{
	return mNeedsPanelToBecomeKey;
}

#pragma mark ===========================

#pragma mark ---オーバーライド

/**
ウインドウへの登録時処理
*/
- (void)viewDidMoveToWindow
{
	//継承処理実行
	[super viewDidMoveToWindow];
	
	//mouse tracking rectの更新
	//参考：https://developer.apple.com/library/mac/#qa/qa1355/_index.html
	[self updateMouseTrackingRect];
}

/**
ウインドウからの登録削除前処理
*/
- (void)viewWillMoveToWindow:(NSWindow *)newWindow 
{
	//継承処理実行
	[super viewWillMoveToWindow:newWindow];
	
	//mouse tracking rect削除
	[self clearAllMouseTrackingRect];
}

/**
frame変更時処理
*/
- (void)setFrame:(NSRect)frame
{
	//継承処理実行
	[super setFrame:frame];
	
	
	OS_CALLBACKBLOCK_START(false);
	
	//MouseTrackingRect更新
	[self updateMouseTrackingRect];
	
	//CWindowImp経由でAWindow::EVT_DoControlBoundsChanged()実行
	mWindowImp->APICB_CocoaViewFrameChanged(mControlID);
	
	OS_CALLBACKBLOCK_END;
}

/**
bounds変更時処理
*/
- (void)setBounds:(NSRect)bounds
{
	//継承処理実行
	[super setBounds:bounds];
	
	//MouseTrackingRect更新
	[self updateMouseTrackingRect];
}

/**
FirstResponderになったときの処理
*/
- (BOOL)becomeFirstResponder
{
	//コントロールがdisabledのときは、first responderにしない
	if( [self isEnabled] == NO )
	{
		return NO;
	}
	
	//継承処理実行
	//#893 BOOL	result = [super becomeFirstResponder];
	
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,YES);
	
	//CWindowImp経由でAWindow::EVT_DoViewFocusActivated()実行
	mWindowImp->APICB_CocoaViewBecomeFirstResponder(mControlID);
	
	OS_CALLBACKBLOCK_END;
	
	//#893 return result;
	return YES;
}

/**
FirstResponderでなくなったときの処理
*/
- (BOOL)resignFirstResponder
{
	//継承処理実行
	//#893 BOOL	result = [super resignFirstResponder];
	
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,YES);
	
	//CWindowImp経由でAWindow::EVT_DoViewFocusDeactivated()実行
	mWindowImp->APICB_CocoaViewResignFirstResponder(mControlID);
	
	OS_CALLBACKBLOCK_END;
	
	//#893 return result;
	return YES;
}

#pragma mark ===========================

#pragma mark ---各キーアクションコールバック処理

/**
key down時処理（raw keyイベント）
*/
- (void)keyDown:(NSEvent *)inEvent 
{
	OS_CALLBACKBLOCK_START(false);
	
	//テキスト取得
	AText	text;
	ACocoa::SetTextFromNSString([inEvent characters],text);
	
	//modifierKeys取得 #1080
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
	//AWindow::EVT_DoKeyDown()を実行。実際に処理実行されたら、interpretKeyEventsを実行せずに、ここで終了。
	if( mWindowImp->APICB_CocoaDoKeyDown(mControlID,text,modifier) == true )
	{
		return;
	}
	
	OS_CALLBACKBLOCK_END;
	
	//キー処理フラグをoff
	//GENERAL_KEYACTION内で判定される。EVT_DoTextInput()の返り値がtrue（EVT_DoTextInput()で処理済み）だと、フラグonになる。
	//OSキーバインドが複数のキーアクションを行うことがあるので、2つ目以降のキーアクションコールバック時にEVT_DoTextInput()を処理しないようにするため。
	//#1015 mKeyHandled = false;
	[((AppDelegate*)[NSApp delegate]) setKeyHandled:NO];
	
	//interpretKeyEventsを実行。
	//OS内でOSキーバインド等が処理され、insertText, setMarkedText, およびdeleteBackward等の各アクションがコールバックされる
	[self interpretKeyEvents:[NSArray arrayWithObject:inEvent]];
	
	//キー処理フラグをoff
	//#1015 mKeyHandled = false;
	[((AppDelegate*)[NSApp delegate]) setKeyHandled:NO];
}

/**
キャンセル
*/
- (void)cancelOperation:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_cancelOperation);
	OS_CALLBACKBLOCK_END;
}

/**
単語大文字化（最初の文字のみ大文字）
*/
- (void)capitalizeWord:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_capitalizeWord);
	OS_CALLBACKBLOCK_END;
}

/**
センタライズ
*/
- (void)centerSelectionInVisibleArea:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_recenter);
	OS_CALLBACKBLOCK_END;
}

/**
文字の大文字・小文字変換
*/
- (void)changeCaseOfLetter:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_changeCaseOfLetter);
	OS_CALLBACKBLOCK_END;
}

/**
補完
*/
- (void)complete:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_abbrevnext);
	OS_CALLBACKBLOCK_END;
}

/**
前を削除
*/
- (void)deleteBackward:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_deleteprev);
	OS_CALLBACKBLOCK_END;
}

/**
前を削除（decomp）
*/
- (void)deleteBackwardByDecomposingPreviousCharacter:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_deleteBackwardByDecomposingPreviousCharacter);
	OS_CALLBACKBLOCK_END;
}

/**
次を削除
*/
- (void)deleteForward:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_deletenext);
	OS_CALLBACKBLOCK_END;
}

/**
行の最初まで削除
*/
- (void)deleteToBeginningOfLine:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_deletelinestart);
	OS_CALLBACKBLOCK_END;
}

/**
段落の最初まで削除
*/
- (void)deleteToBeginningOfParagraph:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_deleteparagraphstart);
	OS_CALLBACKBLOCK_END;
}

/**
行の最後まで削除
*/
- (void)deleteToEndOfLine:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_deletelineend);
	OS_CALLBACKBLOCK_END;
}

/**
段落の最後まで削除
*/
- (void)deleteToEndOfParagraph:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_deleteparagraphend);
	OS_CALLBACKBLOCK_END;
}

/**
マーク位置まで削除
*/
- (void)deleteToMark:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_deleteToMark);
	OS_CALLBACKBLOCK_END;
}

/**
前単語まで削除
*/
- (void)deleteWordBackward:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_deleteWordBackward);
	OS_CALLBACKBLOCK_END;
}

/**
次単語まで削除
*/
- (void)deleteWordForward:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_deleteWordForward);
	OS_CALLBACKBLOCK_END;
}

/**
インデント
*/
- (void)indent:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_indent);
	OS_CALLBACKBLOCK_END;
}

//test
/**
インデント/tab
*/
- (void)indenttab:(id)sender
		{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_indenttab);
	OS_CALLBACKBLOCK_END;
}

/**
前フィールドへの移動
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_NOPで渡す。）
*/
- (void)insertBacktab:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_NOP);
	OS_CALLBACKBLOCK_END;
}

/**
ページブレーク挿入
*/
- (void)insertContainerBreak:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_insertContainerBreak);
	OS_CALLBACKBLOCK_END;
}

/**
オートコレクトなしでダブルクオーテーションを挿入
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_NOPで渡す。）
*/
- (void)insertDoubleQuoteIgnoringSubstitution:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_NOP);
	OS_CALLBACKBLOCK_END;
}

/**
ラインブレーク挿入（段落内の改行）
*/
- (void)insertLineBreak:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_insertLineBreak);
	OS_CALLBACKBLOCK_END;
}

/**
改行
*/
- (void)insertNewline:(id)sender 
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_indentreturn);
	OS_CALLBACKBLOCK_END;
}

/**
Field editorへ入力終了通知せずに改行挿入
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_returnで渡す。）
*/
- (void)insertNewlineIgnoringFieldEditor:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_return);
	OS_CALLBACKBLOCK_END;
}

/**
段落セパレータ挿入
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_NOPで渡す。）
*/
- (void)insertParagraphSeparator:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_NOP);
	OS_CALLBACKBLOCK_END;
}

/**
オートコレクトなしでシングルクオーテーションを挿入
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_NOPで渡す。）
*/
- (void)insertSingleQuoteIgnoringSubstitution:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_NOP);
	OS_CALLBACKBLOCK_END;
}

/**
タブ挿入
*/
- (void)insertTab:(id)sender 
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_indenttab);
	OS_CALLBACKBLOCK_END;
}

/**
Field editorへ入力終了通知せずにタブ挿入
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_tabで渡す。）
*/
- (void)insertTabIgnoringFieldEditor:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_tab);
	OS_CALLBACKBLOCK_END;
}

//#952
/**
次のkey view
（control+tab）
*/
- (void)selectNextKeyView:(id)sender 
{
	OS_CALLBACKBLOCK_START(false);
	//#1015 mKeyHandled = false;//暫定（他のキーと違い、keyDown経由では来ない。最終的には、CocoaAppのsendEvent:内でsendEvent:実行直前で、mKeyHandled相当のデータを初期化し、mKeyHandledの代わりにそちらを参照したほうがいいかもしれない）
	GENERAL_KEYACTION(keyAction_tab);
	OS_CALLBACKBLOCK_END;
}

/**
単語小文字化
*/
- (void)lowercaseWord:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_lowercaseWord);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を（論理的な）前へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveBackward:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretleft);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を（論理的な）前へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveBackwardAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectleft);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を下へ移動
*/
- (void)moveDown:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretdown);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を下へ移動
*/
- (void)moveDownAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectdown);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を（論理的な）次へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveForward:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretright);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を（論理的な）次へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveForwardAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectright);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を左へ移動
*/
- (void)moveLeft:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretleft);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を左へ移動
*/
- (void)moveLeftAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectleft);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を前の段落へ移動
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_NOPで渡す。）
*/
- (void)moveParagraphBackwardAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_NOP);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を次の段落へ移動
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_NOPで渡す。）
*/
- (void)moveParagraphForwardAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_NOP);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を右へ移動
*/
- (void)moveRight:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretright);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を右へ移動
*/
- (void)moveRightAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectright);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置をドキュメント最初へ移動
*/
- (void)moveToBeginningOfDocument:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_carethome);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲をドキュメント最初へ移動
*/
- (void)moveToBeginningOfDocumentAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selecthome);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を行最初へ移動
*/
- (void)moveToBeginningOfLine:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretlinestart);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を行最初へ移動
*/
- (void)moveToBeginningOfLineAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectlinestart);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を段落最初へ移動
*/
- (void)moveToBeginningOfParagraph:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretparagraphstart);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を段落最初へ移動
*/
- (void)moveToBeginningOfParagraphAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectparagraphstart);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置をドキュメント最後へ移動
*/
- (void)moveToEndOfDocument:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretend);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲をドキュメント最後へ移動
*/
- (void)moveToEndOfDocumentAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectend);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を行最後へ移動
*/
- (void)moveToEndOfLine:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretlineend);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を行最後へ移動
*/
- (void)moveToEndOfLineAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectlineend);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を段落最後へ移動
*/
- (void)moveToEndOfParagraph:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretparagraphend);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を段落最後へ移動
*/
- (void)moveToEndOfParagraphAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectparagraphend);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を行左端へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveToLeftEndOfLine:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretlinestart);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を行左端へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveToLeftEndOfLineAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectlinestart);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を行右端へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveToRightEndOfLine:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretlineend);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を行右端へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveToRightEndOfLineAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectlineend);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を上へ移動
*/
- (void)moveUp:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_caretup);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を上へ移動
*/
- (void)moveUpAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectup);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を前の単語へ移動
*/
- (void)moveWordBackward:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_previousword);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を前の単語へ移動
*/
- (void)moveWordBackwardAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectprevword);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を次の単語へ移動
*/
- (void)moveWordForward:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_nextword);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を次の単語へ移動
*/
- (void)moveWordForwardAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectnextword);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を左の単語へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveWordLeft:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_previousword);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を左の単語へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveWordLeftAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectprevword);
	OS_CALLBACKBLOCK_END;
}

/**
キャレット位置を右の単語へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveWordRight:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_nextword);
	OS_CALLBACKBLOCK_END;
}

/**
選択範囲を右の単語へ移動
（現状、bidirectional未対応のため、左から右のテキストのみ考慮。）
*/
- (void)moveWordRightAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectnextword);
	OS_CALLBACKBLOCK_END;
}

/**
page down
*/
- (void)pageDown:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_pagedownwithcaret);
	OS_CALLBACKBLOCK_END;
}

/**
page down（選択範囲変更）
*/
- (void)pageDownAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_pageDownAndModifySelection);
	OS_CALLBACKBLOCK_END;
}

/**
page up
*/
- (void)pageUp:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_pageupwithcaret);
	OS_CALLBACKBLOCK_END;
}

/**
page up（選択範囲変更）
*/
- (void)pageUpAndModifySelection:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_pageUpAndModifySelection);
	OS_CALLBACKBLOCK_END;
}

/**
下スクロール
*/
- (void)scrollLineDown:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_scrolldown);
	OS_CALLBACKBLOCK_END;
}

/**
上スクロール
*/
- (void)scrollLineUp:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_scrollup);
	OS_CALLBACKBLOCK_END;
}

/**
page down（スクロールのみ）
*/
- (void)scrollPageDown:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_pagedown);
	OS_CALLBACKBLOCK_END;
}

/**
page up（スクロールのみ）
*/
- (void)scrollPageUp:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_pageup);
	OS_CALLBACKBLOCK_END;
}

/**
ドキュメント最初にスクロール
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_homeで渡す。）
*/
- (void)scrollToBeginningOfDocument:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_home);
	OS_CALLBACKBLOCK_END;
}

/**
ドキュメント最後にスクロール
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_endで渡す。）
*/
- (void)scrollToEndOfDocument:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_end);
	OS_CALLBACKBLOCK_END;
}

/**
全てを選択
*/
- (void)selectAll:(id)sender
{
	if( [sender isKindOfClass:[NSMenuItem class]] == YES )
	{
		//メニュー選択の場合は、App delegateでメニュー用の処理を実行
		[((AppDelegate*)[NSApp delegate]) doMenuActions:sender];
	}
	else
	{
		//メニュー選択ではない場合は、キー処理を実行
		OS_CALLBACKBLOCK_START(false);
		GENERAL_KEYACTION(keyAction_selectAll);
		OS_CALLBACKBLOCK_END;
	}
}

/**
行を選択
*/
- (void)selectLine:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectLine);
	OS_CALLBACKBLOCK_END;
}

/**
段落を選択
*/
- (void)selectParagraph:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectParagraph);
	OS_CALLBACKBLOCK_END;
}

/**
マーク位置までを選択
*/
- (void)selectToMark:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectToMark);
	OS_CALLBACKBLOCK_END;
}

/**
単語を選択
*/
- (void)selectWord:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_selectWord);
	OS_CALLBACKBLOCK_END;
}

/**
マーク位置設定
*/
- (void)setMark:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_setMark);
	OS_CALLBACKBLOCK_END;
}

/**
ヘルプ表示
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_NOPで渡す。）
*/
- (void)showContextHelp:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_NOP);
	OS_CALLBACKBLOCK_END;
}

/**
マーク位置と現在位置をswap
*/
- (void)swapWithMark:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_swapWithMark);
	OS_CALLBACKBLOCK_END;
}

/**
文字入れ替え
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_NOPで渡す。）
*/
- (void)transpose:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_NOP);
	OS_CALLBACKBLOCK_END;
}

/**
単語入れ替え
（CocoaUserPaneViewで使用することはないが、このキーバインドがOSで設定されている場合に、
AView_xxxへkeyBindKey, modifierを渡すために定義する。keyBindActionは、keyAction_NOPで渡す。）
*/
- (void)transposeWords:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_NOP);
	OS_CALLBACKBLOCK_END;
}

/**
単語大文字化
*/
- (void)uppercaseWord:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_uppercaseWord);
	OS_CALLBACKBLOCK_END;
}

/**
yank
*/
- (void)yank:(id)sender
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_yank);
	OS_CALLBACKBLOCK_END;
}

/**
noop
*/
- (void)noop:(id)sender 
{
	OS_CALLBACKBLOCK_START(false);
	GENERAL_KEYACTION(keyAction_NOP);
	OS_CALLBACKBLOCK_END;
}

#pragma mark ===========================

#pragma mark ---イベントコールバック処理

/**
マウスダウンコールバック処理
*/
- (void)mouseDown:(NSEvent *)inEvent
{
	OS_CALLBACKBLOCK_START(false);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[inEvent locationInWindow] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent(inEvent);
	//CWindowImpオブジェクトのマウスダウン処理実行 #1337 control+タップはmouseDownのほうに来るので、controlキーが押されていたら右クリックとみなすようにする
	if( (modifier&kModifierKeysMask_Control) == 0 )
	{
		mWindowImp->APICB_CocoaMouseDown(mControlID,localPoint,modifier,[inEvent clickCount]);
	}
	else
	{
		mWindowImp->APICB_CocoaRightMouseDown(mControlID,localPoint,modifier,[inEvent clickCount]);
	}
	
	OS_CALLBACKBLOCK_END;
}

/**
右クリック処理
*/
- (void)rightMouseDown:(NSEvent *)inEvent
{
	OS_CALLBACKBLOCK_START(false);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[inEvent locationInWindow] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent(inEvent);
	//CWindowImpオブジェクトのマウスダウン処理実行
	mWindowImp->APICB_CocoaRightMouseDown(mControlID,localPoint,modifier,[inEvent clickCount]);
	
	OS_CALLBACKBLOCK_END;
}

/**
マウスdragコールバック処理
*/
- (void)mouseDragged:(NSEvent *)inEvent
{
	OS_CALLBACKBLOCK_START(false);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[inEvent locationInWindow] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent(inEvent);
	//CWindowImpオブジェクトのマウスdrag処理実行
	mWindowImp->APICB_CocoaMouseDragged(mControlID,localPoint,modifier);
	
	OS_CALLBACKBLOCK_END;
}

/**
マウスupコールバック処理
*/
- (void)mouseUp:(NSEvent *)inEvent
{
	OS_CALLBACKBLOCK_START(false);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[inEvent locationInWindow] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent(inEvent);
	//CWindowImpオブジェクトのマウスup処理実行
	mWindowImp->APICB_CocoaMouseUp(mControlID,localPoint,modifier);
	
	OS_CALLBACKBLOCK_END;
}

/**
マウスホイールコールバック処理
*/
- (void)scrollWheel:(NSEvent *)inEvent
{
	OS_CALLBACKBLOCK_START(false);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[inEvent locationInWindow] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//delta取得
	CGFloat	deltaX = -[inEvent deltaX];
	if( CAppImp::GetUseWheelScrollFloatValue() == false )
	{
		if( deltaX < 0 )
		{
			deltaX = floor(deltaX);
		}
		else
		{
			deltaX = ceil(deltaX);
		}
	}
	CGFloat	deltaY = -[inEvent deltaY];
	if( CAppImp::GetUseWheelScrollFloatValue() == false )
	{
		if( deltaY < 0 )
		{
			deltaY = floor(deltaY);
		}
		else
		{
			deltaY = ceil(deltaY);
		}
	}
	
	if( deltaY != 0.0 || deltaX != 0.0 )
	{
		//modifierKeys取得
		AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent(inEvent);
		//CWindowImpオブジェクトのマウスwheel処理実行
		mWindowImp->APICB_CocoaMouseWheel(mControlID,deltaX,deltaY,modifier,localPoint);
	}
	
	OS_CALLBACKBLOCK_END;
}

#if 0
/**
マウスがtracking rectに入ったときのコールバック
tracking rectはsetMouseTrackingRect, updateMouseTrackingRect, clearAllMouseTrackingRectで設定される。
*/
- (void)mouseEntered:(NSEvent *)inEvent
{
	fprintf(stderr,"mouseEntered:%d ",[self tag]);
	
	OS_CALLBACKBLOCK_START(false);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[inEvent locationInWindow] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent(inEvent);
	mWindowImp->APICB_CocoaMouseMoved(mControlID,localPoint,modifier);
	
	OS_CALLBACKBLOCK_END;
}
#endif

/**
マウスがtracking rectから出たときのコールバック
tracking rectはsetMouseTrackingRect, updateMouseTrackingRect, clearAllMouseTrackingRectで設定される。
*/

- (void)mouseExited:(NSEvent *)inEvent
{
	//fprintf(stderr,"mouseExited:%d ",[self tag]);
	
	OS_CALLBACKBLOCK_START(false);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[inEvent locationInWindow] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent(inEvent);
	/*
	//コントロールから出たのか、コントロール内の追加tracking rectから出たのかを判定
	//updateMouseTrackingRectにて、mTrackingRectTagArrayの最初に、コントロール全体のrectのtracking rect tagを入れている
	NSInteger	trackingnum = [inEvent trackingNumber];
	if( mTrackingRectTagArray.GetItemCount() == 0 )   {_ACError("",NULL);return;}
	if( mTrackingRectTagArray.Get(0) == trackingnum )
	{
		mWindowImp->APICB_CocoaMouseExited(mControlID,localPoint);
	}
	else
	{
		mWindowImp->APICB_CocoaMouseMoved(mControlID,localPoint,modifier);
	}
	*/
	mWindowImp->APICB_CocoaMouseExited(mControlID,localPoint);
	
	OS_CALLBACKBLOCK_END;
}

//#1026
/**
QuickLook（トラックパッド３本指タップ）
*/
- (void)quickLookWithEvent:(NSEvent *)inEvent
{
	OS_CALLBACKBLOCK_START(false);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[inEvent locationInWindow] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent(inEvent);
	//
	mWindowImp->APICB_CocoaQuickLook(mControlID,localPoint,modifier);
	
	OS_CALLBACKBLOCK_END;
}

/*
- (void)quickLookPreviewItems:(id)sender
{
	fprintf(stderr,"- (void)quickLookPreviewItems:(id)sender");
}
*/

/**
画面描画コールバック処理
*/
- (void)drawRect:(NSRect)dirtyRect
{
	OS_CALLBACKBLOCK_START(false);
	
	//dirtyRectからLocalRectへ変換（isFlipped=YESなのでそのままの値）
	ALocalRect	localRect = {0,0,0,0};
	localRect.left		= dirtyRect.origin.x;
	localRect.top		= dirtyRect.origin.y;
	localRect.right		= localRect.left + dirtyRect.size.width;
	localRect.bottom	= localRect.top  + dirtyRect.size.height;
	//CUserPaneの描画処理実行
	mWindowImp->APICB_CocoaDrawRect(mControlID,localRect);
	
	OS_CALLBACKBLOCK_END;
}

#pragma mark ===========================

#pragma mark ---Dragイベントコールバック処理

/**
dragがviewに入った時のコールバック
*/
- (NSDragOperation)draggingEntered:(id < NSDraggingInfo >)sender
{
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,NSDragOperationNone);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[sender draggingLocation] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
	//AWindow::EVT_DoDragEnter()を実行
	mWindowImp->APICB_CocoaDraggingEntered(mControlID,[sender draggingPasteboard],localPoint,modifier);
	
	OS_CALLBACKBLOCK_END;
	
	return NSDragOperationMove;
}

/**
drag中コールバック
*/
- (NSDragOperation)draggingUpdated:(id < NSDraggingInfo >)sender
{
	ABool	isCopy = false;
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,NSDragOperationNone);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[sender draggingLocation] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
	//Window::EVT_DoDragLeave()/EVT_DoDragEnter()/EVT_DoDragTracking()を実行
	mWindowImp->APICB_CocoaDraggingUpdated(mControlID,[sender draggingPasteboard],localPoint,modifier,isCopy);
	
	OS_CALLBACKBLOCK_END;
	
	if( isCopy == true )
	{
		return NSDragOperationCopy;
	}
	else
	{
		return NSDragOperationMove;
	}
}

/**
他のviewでdrag終了時のコールバック
Mac OS X 10.5までは動作しないとのことなので、対応しない。
*/
/*
- (void)draggingEnded:(id < NSDraggingInfo >)sender
{
}
*/

/**
dragがviewから出た時のコールバック
*/
- (void)draggingExited:(id < NSDraggingInfo >)sender
{
	OS_CALLBACKBLOCK_START(false);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[sender draggingLocation] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
	//AWindow::EVT_DoDragLeave()を実行
	mWindowImp->APICB_CocoaDraggingExited(mControlID,[sender draggingPasteboard],localPoint,modifier);
	
	OS_CALLBACKBLOCK_END;
	
	return;
}

/**
drop時のコールバック
*/
- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,NO);
	
	//Local座標に変換
	NSPoint curPoint = [self convertPoint:[sender draggingLocation] fromView:nil];
	ALocalPoint	localPoint = {curPoint.x,curPoint.y};
	//modifierKeys取得
	AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
	//AWindow::EVT_DoDragReceive()/EVT_DoFileDropped()を実行
	mWindowImp->APICB_CocoaPerformDragOperation(mControlID,[sender draggingPasteboard],localPoint,modifier);
	
	OS_CALLBACKBLOCK_END;
	
	return YES;
}
#if 0
#pragma mark ===========================

#pragma mark ---ツールチップ

/**
*/
- (NSString *)view:(NSView *)view stringForToolTip:(NSToolTipTag)tag point:(NSPoint)point userData:(void *)userData
{
	NSString*	result = nil;
	
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,@"");
	
	AText	text;
	text.SetNumber(point.x);
	AStCreateNSStringFromAText	str(text);
	result = str.GetNSString();
	[result retain];
	
	/*
	//
	//rectは、ヘルプタグ描画位置（デフォルトでコントロールのrectを入れておく。EVT_DoGetHelpTag()で値が設定される）
	ALocalRect	rect;
	rect.left		= 0;
	rect.top		= 0;
	rect.right		= GetControlWidth(inControlID);
	rect.bottom		= GetControlHeight(inControlID);
	AText	text1, text2;
	//AWindow, AViewオブジェクトから、現在のマウスポインタに対応するヘルプタグ情報を取得
	AHelpTagSide	tagside = kHelpTagSide_Default;//#643
	if( GetAWin().EVT_DoGetHelpTag(inControlID,inLocalPoint,text1,text2,rect,tagside) == true )
	{
		SetHelpTag(inControlID,text1,tagside);
	}
	*/
	OS_CALLBACKBLOCK_END;
	
	return result;
}
#endif

/*
- (void)draggedImage:(NSImage *)draggedImage movedTo:(NSPoint)screenPoint
{
	//Image生成
	NSView*	contentView = mWindowImp->GetCocoaObjects().GetContentView();
	AWindowRect	windowRect = {0,0,300,300};
	NSData* theData = [contentView dataWithPDFInsideRect:ConvertFromWindowRectToNSRect(contentView,windowRect)];//
	NSPDFImageRep* pdfRep = [NSPDFImageRep imageRepWithData:theData];//
	//NSImage* pdfImage = [[[NSImage alloc] initWithSize:NSMakeSize(inImageWidth,inImageHeight)] autorelease];
	[draggedImage addRepresentation:pdfRep];
}
*/

#pragma mark ===========================

#pragma mark ---標準メニューのハンドラ


//CocoaUserPaneViewがFirst Responderの場合、validateMenuItem:はResponder Chainに従って、
//AppDelegateまでさかのぼるはずだが、undo:についてはどこかで定義されているためか、AppDelegateのvalidateMenuItem:まで来ない。
//そのため、CocoaUserPaneViewにて、AppDelegateへactionとvalidateMenuItemをリダイレクトすることにした。
//CocoaWindowでは定義しない。CocoaWindowで定義すると、NSTextFieldでのundoが効かなくなるため。

/**
各標準メニューハンドラ
AppDelegateへリダイレクトする。selectAllについては、上のほうで既に定義されている。
*/
- (IBAction)undo:(id)sender
{
	[((AppDelegate*)[NSApp delegate]) doMenuActions:sender];
}
- (IBAction)redo:(id)sender
{
	[((AppDelegate*)[NSApp delegate]) doMenuActions:sender];
}
- (IBAction)copy:(id)sender
{
	[((AppDelegate*)[NSApp delegate]) doMenuActions:sender];
}
- (IBAction)cut:(id)sender
{
	[((AppDelegate*)[NSApp delegate]) doMenuActions:sender];
}
- (IBAction)paste:(id)sender
{
	[((AppDelegate*)[NSApp delegate]) doMenuActions:sender];
}
- (IBAction)delete:(id)sender
{
	[((AppDelegate*)[NSApp delegate]) doMenuActions:sender];
}

/**
メニューenable/disable処理
AppDelegateへリダイレクトする。
*/
- (BOOL)validateMenuItem:(NSMenuItem *)item
{
	return [((AppDelegate*)[NSApp delegate]) validateMenuItem:item];//10.10SDKではAppDelegate*キャストがないとエラーになるのでキャスト追加
}

@end

@implementation CocoaUserPaneTextInputClientView
#pragma mark ===========================

#pragma mark ---NSTextInputClient/NSTextInput

/**
insertText/setMarkedText用 引数inStringからNSString*を取得
*/
- (NSString*)getNSStringFromInlineInputString:(id)inString
{
	//NSAttributedStringの場合はstringで取得
	if( [inString isKindOfClass:[NSAttributedString class]] == YES )
	{
		return [inString string];
	}
	//NSStringの場合はそのものを返す
	else if( [inString isKindOfClass:[NSString class]] == YES )
	{
		return inString;
	}
	else
	{
		//上記以外はエラー
		_ACError("",NULL);
		return inString;
	}
}

/**
テキスト挿入コールバック
TextInputClient/共通用
*/
- (void)insertText:(id)inString replacementRange:(NSRange)inReplaceRange
{
	OS_CALLBACKBLOCK_START(false);
	
	//fprintf(stderr,"insertText:replacementRange:(%d,%d)\n",inReplaceRange.location,inReplaceRange.length);
	
	// ================ テキスト取得 ================
	
	//テキスト取得
	NSString*	string = [self getNSStringFromInlineInputString:inString];
	AText	text;
	ACocoa::SetTextFromNSString(string,text);
	
	//Markedテキストが存在するかどうか（変換中かどうか）の判定
	if( [self hasMarkedText] == NO )
	{
		// ================ Markedテキストが存在しない場合：通常テキスト ================
		
		//key取得
		AKeyBindKey	key = ACocoa::GetKeyBindKeyFromNSEvent([NSApp currentEvent],text);//#942
		//modifierKeys取得
		AModifierKeys	modifier = ACocoa::ConvertToAModifierKeysFromNSEvent([NSApp currentEvent]);
		//CWindowImp経由でAWindow::EVT_DoTextInput()を実行
		mWindowImp->APICB_CocoaDoTextInput(mControlID,text,key,modifier,keyAction_InsertText);
	}
	else
	{
		// ================ Markedテキストが存在する場合：InlineInput（全確定） ================
		
		// ================ 置き換え判定 ================
		
		/*setMarkedText:と同じ動作で良い可能性が高いが、insertText:でinReplaceRange.length>0となるパターンが見つからず、
		テストできないので、未対応とする。*/
		AItemCount	replaceLen = 0;
		
		// ================ ダミーテキスト更新 ================
		
		//ダミーテキスト更新
		//mMarkedRangeの範囲をstringで置き換え
		//NSLog(mCurrentInlineInputString);
		[mCurrentInlineInputString replaceCharactersInRange:mMarkedRange withString:string];
		
		// ================ mSelectedRange更新 ================
		
		//SelectedRange更新
		//mCurrentInlineInputStringの最後の位置をSelectedRangeに設定
		mSelectedRange.location = [mCurrentInlineInputString length];
		mSelectedRange.length = 0;
		
		// ================ AWindow::EVT_DoInlineInput()実行 ================
		
		//CWindowImp経由でAWindow::EVT_DoInlineInput()を実行
		//EVT_DoInlineInput()は未確定（変換中）部分を新たなtextで更新する
		//下記では、fixlenにtextの長さ全体を入れているので、未確定部分が全削除されて、全て確定状態となる。
		AArray<AIndex>	hiliteLineStyleIndex;
		AArray<AIndex>	hiliteStartPos;
		AArray<AIndex>	hiliteEndPos;
		mSuppressResetSelectedRange = true;
		mWindowImp->APICB_CocoaDoInlineInput(mControlID,text,text.GetItemCount(),
											 hiliteLineStyleIndex,hiliteStartPos,hiliteEndPos,replaceLen);
		mSuppressResetSelectedRange = false;
	}
	
	// ================ mMarkedRange更新 ================
	
	mMarkedRange = NSMakeRange(NSNotFound,0);
	
	// ================ Mark状態クリア ================
	
	[self unmarkText];
	
	OS_CALLBACKBLOCK_END;
}

/**
テキスト挿入コールバック
NSTextInput／通常テキスト用
*/
- (void)insertText:(id)inString
{
	NSRange	replaceRange = NSMakeRange(NSNotFound,0);
	[self insertText:inString replacementRange:replaceRange];
}

/**
Marked（変換中）テキスト挿入コールバック
NSTextInputClient/
*/
- (void)setMarkedText:(id)inString selectedRange:(NSRange)inNewSelection replacementRange:(NSRange)inReplaceRange
{
	OS_CALLBACKBLOCK_START(false);
	
	//fprintf(stderr,"setMarkedText:selectedRange:(%d,%d):replacementRange(%d,%d)\n",
	//			inNewSelection.location,inNewSelection.length,inReplaceRange.location,inReplaceRange.length);
	
	// ================ テキスト取得 ================
	
	//テキスト取得
	NSString*	string = [self getNSStringFromInlineInputString:inString];
	AText	text;
	ACocoa::SetTextFromNSString(string,text);
	//UTF16変換データ取得
	text.GetUTF16TextIndexInfo(mCurrentInlineInputUTF16PosIndexArray,mCurrentInlineInputUTF8PosIndexArray);
	
	// ================ 下線データ取得 ================
	
	//Markedテキスト内選択位置取得（inNewSelectionはMarkedテキスト内の相対位置）
	AIndex	selStart = inNewSelection.location;
	AIndex	selEnd = inNewSelection.location + inNewSelection.length;
	//下線データ
	AArray<AIndex>	hiliteLineStyleIndex;
	AArray<AIndex>	hiliteStartPos;
	AArray<AIndex>	hiliteEndPos;
	if( [inString isKindOfClass:[NSAttributedString class]] == YES )
	{
		//NSAttributedStringの場合
		
		//inStringの最初から、下線ひとかたまりごとに、データを取得する
		AItemCount	len = [inString length];
		for( AIndex index = 0; index < len; )
		{
			//index以降のNSUnderlineのrangeを取得
			NSRange	range = {0,0};
			id	underline = [inString attribute:@"NSUnderline" atIndex:index effectiveRange:&range];
			if( underline == nil )
			{
				//NSUnderlineがindexに存在していなければ、indexをインクリメントして、continue
				index++;
				continue;
			}
			//rangeをUTF8位置に変換して格納
			hiliteStartPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(range.location));
			hiliteEndPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(range.location+range.length));
			//styleを格納
			AIndex	styleIndex = 2;
			if( range.location == selStart )
			{
				//rangeが選択位置なら3にする
				styleIndex = 3;
			}
			hiliteLineStyleIndex.Add(styleIndex);
			/*テスト用
			NSRange	limit = NSMakeRange(0,len);
			NSDictionary*	dic = [inString attributesAtIndex:index longestEffectiveRange:&range inRange:limit];
			NSArray*	keys = [dic allKeys];
			NSLog(@"%@",[keys componentsJoinedByString:@" "]);
			NSArray*	values = [dic allValues];
			NSLog(@"%@",[values componentsJoinedByString:@" "]);
			fprintf(stderr,"%d range:%d,%d   ",[dic count],range.location,range.length);
			*/
			//次のindexへ
			if( range.length > 0 )
			{
				//rangeが範囲を持っているなら、そのrangeの次の文字へ
				index += range.length;
			}
			else
			{
				//rangeが空の範囲なら、indexインクリメント
				index++;
			}
		}
		//選択範囲が0文字なら、キャレット設定
		if( selStart == selEnd )
		{
			hiliteStartPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(selStart));
			hiliteEndPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(selEnd));
			hiliteLineStyleIndex.Add(kIndex_Invalid);
		}
	}
	else if( [inString isKindOfClass:[NSString class]] == YES )
	{
		//NSStringの場合
		
		//★未テスト
		
		//選択位置までを下線にする
		hiliteStartPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(0));
		hiliteEndPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(selStart));
		hiliteLineStyleIndex.Add(2);
		//選択位置を下線またはキャレットにする
		hiliteStartPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(selStart));
		hiliteEndPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(selEnd));
		if( selStart == selEnd )
		{
			hiliteLineStyleIndex.Add(kIndex_Invalid);
		}
		else
		{
			hiliteLineStyleIndex.Add(3);
		}
		//選択位置から最後までを下線にする
		hiliteStartPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(selEnd));
		hiliteEndPos.Add(text.GetItemCount());
		hiliteLineStyleIndex.Add(2);
	}
	else
	{
		//上記以外はエラー
		_ACError("",NULL);
		return;
	}
	
	// ================ mMarkedRange更新 ================
	
	//今回削除すべきテキストの位置をremoveRangeに格納する。
	NSRange	removeRange = NSMakeRange([mCurrentInlineInputString length],0);
	//元々変換中だったかどうかの判定
	if( mMarkedRange.length == 0 )
	{
		//元々Markedテキストが存在しない状態だった場合、
		//mSelectedRangeを削除領域removeRangeに設定
		removeRange = mSelectedRange;
		//新たなMarkedRangeは、mSelectedRangeの開始位置以降
		mMarkedRange = NSMakeRange(mSelectedRange.location,[string length]);
	}
	else
	{
		//元々Markedテキストが存在する状態だった場合、
		//元のMarkedRangeを削除領域removeRangeに設定
		removeRange = mMarkedRange;
		//新たなMarkedRangeは、元のMarkedRangeの開始位置以降
		mMarkedRange = NSMakeRange(mMarkedRange.location,[string length]);
	}
	//Markedテキストが空の場合は、Marked状態を解除する（EVT_DoInlineInput()により、未確定文字列は削除される）
	if( [string length] == 0 )
	{
		mMarkedRange = NSMakeRange(NSNotFound,0);
	}
	
	// ================ 置き換え（英数キー2度押し等）判定 ================
	
	//置き換え判定
	//inReplaceRangeが、変換テキストに隣接している場合のみ、置き換え処理を行う。
	//EVT_DoInlineInput()とのインターフェイスの関係上、複雑な置き換えはできないため。
	//隣接しているかどうかを判定することで、予想外の箇所が置き換えされるのを防ぐ。
	AItemCount	replaceLen = 0;
	//inReplaceRangeが範囲を持っているかどうかの判定
	if( inReplaceRange.location != NSNotFound && inReplaceRange.length > 0 )
	{
		//inReplaceRangeのすぐ後ろに変換テキストがあるかどうかの判定。
		if( inReplaceRange.location + inReplaceRange.length == mMarkedRange.location )
		{
			//削除範囲を前方向に拡大(inReplaceRangeを追加)
			removeRange.location = inReplaceRange.location;
			removeRange.length += inReplaceRange.length;
			//新たなMarkedRangeは、inReplaceRange開始位置以降にする
			mMarkedRange = NSMakeRange(inReplaceRange.location,[string length]);
			//AWindow::EVT_DoInlineInput()に渡す置き換え文字長を設定(UTF-16での長さ)
			replaceLen = inReplaceRange.length;
		}
	}
	
	// ================ ダミーテキスト更新 ================
	
	//ダミーテキスト更新
	[mCurrentInlineInputString replaceCharactersInRange:removeRange withString:string];
	//NSLog(mCurrentInlineInputString);
	
	// ================ mSelectedRange更新 ================
	
	//SelectedRange更新
	//mCurrentInlineInputStringの最後の位置をSelectedRangeに設定
	mSelectedRange.location = [mCurrentInlineInputString length];
	mSelectedRange.length = 0;
	
	// ================ AWindow::EVT_DoInlineInput()実行 ================
	
	//CWindowImp経由でAWindow::EVT_DoInlineInput()を実行
	mSuppressResetSelectedRange = true;
	mWindowImp->APICB_CocoaDoInlineInput(mControlID,text,0,hiliteLineStyleIndex,hiliteStartPos,hiliteEndPos,replaceLen);
	mSuppressResetSelectedRange = false;
	
	OS_CALLBACKBLOCK_END;
}

/**
Marked（変換中）テキスト挿入
NSTextInput用
*/
- (void)setMarkedText:(id)inString selectedRange:(NSRange)inNewSelection
{
	NSRange	replaceRange = NSMakeRange(NSNotFound,0);
	[self setMarkedText:inString selectedRange:inNewSelection replacementRange:replaceRange];
}

/**
mSelectedRange, ダミーテキストをクリアする
*/
- (void)resetSelectedRangeForTextInput
{
	//fprintf(stderr,"resetSelectedRangeForTextInput\n");
	if( mSuppressResetSelectedRange == false && [self hasMarkedText] == NO )
	{
		//mSelectedRange, ダミーテキストをクリア
		mSelectedRange = NSMakeRange(0,0);
		[mCurrentInlineInputString deleteCharactersInRange:NSMakeRange(0,[mCurrentInlineInputString length])];
		//MarkedRangeを更新
		mMarkedRange = NSMakeRange(NSNotFound, 0);
	}
}

/**
MarkedTextの有無（＝変換中かどうか）を返す
*/
- (BOOL)hasMarkedText
{
	return (mMarkedRange.location == NSNotFound ? NO : YES);
}

/**
現在のMarkedRange（＝変換テキストの範囲）を返す
*/
- (NSRange)markedRange
{
	//fprintf(stderr,"markedRange:[%d,%d]\n",mMarkedRange.location,mMarkedRange.length);
	return mMarkedRange;
}

/**
現在のダミーテキストの選択範囲を返す
*/
- (NSRange)selectedRange
{
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,NSMakeRange(0,0));
	
	//fprintf(stderr,"selectedRange:[%d,%d]\n",mSelectedRange.location,mSelectedRange.length);
	
	//AWindow::EVT_DoInlineInputGetSelectedText()を実行して、現在の選択範囲のテキストを取得
	AText	text;
	mWindowImp->APICB_CocoaDoInlineInputGetSelectedText(mControlID,text);
	//選択テキストが存在している場合は、ダミーテキストをその選択テキストにする
	if( text.GetItemCount() > 0 )
	{
		//ダミーテキストを全削除
		[mCurrentInlineInputString deleteCharactersInRange:NSMakeRange(0,[mCurrentInlineInputString length])];
		//選択テキストを設定
		AStCreateNSStringFromAText	str(text);
		[mCurrentInlineInputString appendString:str.GetNSString()];
		//mSelectedRangeはダミーテキストの最後の位置にする
		mSelectedRange = NSMakeRange(0,[mCurrentInlineInputString length]);
		//NSLog(mCurrentInlineInputString);
		//MarkedRangeを更新
		mMarkedRange = NSMakeRange(NSNotFound, 0);
	}
	
	OS_CALLBACKBLOCK_END;
	
	return mSelectedRange;
}

/**
MarkedTextをunmark
*/
- (void)unmarkText
{
	OS_CALLBACKBLOCK_START(false);
	
	//fprintf(stderr,"unmarkText ");
	
	if( [self hasMarkedText] == YES )
	{
		//InputMethod内の入力中Marked Textデータをクリア
		
		//NSTextInputContext : Mac OS X 10.6以降
		//NSInputManager : Mac OS X 10.5以前
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_6 )
		{
			//NSTextInputContextにdiscardMarkedTextを送信
			//inputContext : Mac OS X 10.6以降で対応
			[[self inputContext] discardMarkedText];
		}
		else
		{
			[[NSInputManager currentInputManager] markedTextAbandoned:nil];
		}
		
		//CWindowImp経由でAWindow::EVT_DoInlineInput()を実行
		//APICB_CocoaDoInlineInputの第３引数(inFixLen)をkIndex_Invalidにすることで、強制確定にする。
		AArray<AIndex>	hiliteLineStyleIndex;
		AArray<AIndex>	hiliteStartPos;
		AArray<AIndex>	hiliteEndPos;
		mWindowImp->APICB_CocoaDoInlineInput(mControlID,GetEmptyText(),kIndex_Invalid,
											 hiliteLineStyleIndex,hiliteStartPos,hiliteEndPos,0);
		
		//ダミーテキストクリア
		[mCurrentInlineInputString deleteCharactersInRange:NSMakeRange(0,[mCurrentInlineInputString length])];
	}
	//MarkedRangeを更新
	mMarkedRange = NSMakeRange(NSNotFound, 0);
	//mSelectedRangeは更新しない。（insertTextからunmarkTextをコールしているので、mSelectedRangeを更新すると、英数2度押しが機能しない）
	//（fixInlineInputで実行）
	
	OS_CALLBACKBLOCK_END;
}

/**
InlineInput入力中のテキストを強制確定する
*/
- (void)fixInlineInput
{
	[self unmarkText];
	mSelectedRange = NSMakeRange(0,0);
}

/**
有効なMarkedTextを返す
*/
- (NSArray *)validAttributesForMarkedText
{
	//★要検討
	// We only allow these attributes to be set on our marked text (plus standard attributes)
	// NSMarkedClauseSegmentAttributeName is important for CJK input, among other uses
	// NSGlyphInfoAttributeName allows alternate forms of characters
	return [NSArray arrayWithObjects:NSMarkedClauseSegmentAttributeName, NSGlyphInfoAttributeName, nil];
}

/**
指定範囲のテキストを取得
*/
- (NSAttributedString *)attributedSubstringFromRange:(NSRange)inRange
{
	return [self attributedSubstringForProposedRange:inRange actualRange:nil];
}

/**
指定範囲のテキストを取得
*/
- (NSAttributedString *)attributedSubstringForProposedRange:(NSRange)inRange actualRange:(NSRangePointer)outActualRange
{
	//fprintf(stderr,"attributedSubstringForProposedRange:(%d,%d)\n",inRange.location,inRange.length);
	
	//outActualRangeには引数inRangeのままの値を設定
	if( outActualRange != NULL )
	{
		*outActualRange = inRange;
	}
	//ダミーテキストの指定範囲のテキストを返す
	return [[[NSAttributedString alloc] initWithString:[mCurrentInlineInputString substringWithRange:inRange]] autorelease];
}

/**
Pointに対応するテキスト位置取得
*/
- (NSUInteger)characterIndexForPoint:(NSPoint)inPoint 
{
	//★未テスト
	//ここに来る操作が不明なため
	
	//
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	AGlobalPoint	gpt = {0};
	gpt.x = inPoint.x;
	gpt.y = firstScreenFrame.size.height - inPoint.y;
	
	//
	AIndex	offsetUTF16 = 0;
	AIndex	offsetUTF8 = 0;
	
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,0);
	
	//AWindow::EVT_DoInlineInputPosToOffset()を実行
	mWindowImp->APICB_CocoaCharacterIndexForPoint(mControlID,gpt,offsetUTF8);
	//
	if( offsetUTF8 >= 0 && offsetUTF8 < mCurrentInlineInputUTF16PosIndexArray.GetItemCount() )
	{
		offsetUTF16 = mCurrentInlineInputUTF16PosIndexArray.Get(offsetUTF8);
	}
	
	OS_CALLBACKBLOCK_END;
	
	return mMarkedRange.location - offsetUTF16;
}

/**
テキスト位置に対応するPointを取得
*/
- (NSRect)firstRectForCharacterRange:(NSRange)inRange actualRange:(NSRangePointer)outActualRange 
{
	//返り値:rect初期化
	NSRect	rect = {0};
	
	OS_CALLBACKBLOCK_START_WITHERRRETURNVALUE(false,NSMakeRect(0,0,0,0));
	
	//outActualRangeには引数inRangeのままの値を設定
	if( outActualRange != NULL )
	{
		*outActualRange = inRange;
	}
	
	//変換テキスト内のoffset位置を取得(UTF-16)
	AIndex	startInUTF16 = inRange.location - mMarkedRange.location;
	//UTF-8テキストでの位置に変換
	AIndex	startInUTF8 = 0;
	if( startInUTF16 >= 0 && startInUTF16 < mCurrentInlineInputUTF8PosIndexArray.GetItemCount() )
	{
		startInUTF8 = mCurrentInlineInputUTF8PosIndexArray.Get(startInUTF16);
	}
	//変換テキスト内のoffset位置を取得(UTF-16)
	AIndex	endInUTF16 = inRange.location + inRange.length - mMarkedRange.location;
	//UTF-8テキストでの位置に変換
	AIndex	endInUTF8 = 0;
	if( endInUTF16 >= 0 && endInUTF16 < mCurrentInlineInputUTF8PosIndexArray.GetItemCount() )
	{
		endInUTF8 = mCurrentInlineInputUTF8PosIndexArray.Get(endInUTF16);
	}
	
	//AWindow::EVT_DoInlineInputOffsetToPos()を実行
	AGlobalRect	globalRect = {0};
	mWindowImp->APICB_CocoaFirstRectForCharacterRange(mControlID,startInUTF8,endInUTF8,globalRect);
	//NSRectへ変換
	NSRect	firstScreenFrame = [[[NSScreen screens] objectAtIndex:0] frame];
	rect.origin.x = globalRect.left;
	rect.origin.y = firstScreenFrame.size.height - globalRect.bottom;
	rect.size.width = globalRect.right - globalRect.left;
	rect.size.height = globalRect.bottom - globalRect.top;
	
	OS_CALLBACKBLOCK_END;
	
	return rect;
}

/**
テキスト位置に対応するPointを取得
*/
- (NSRect)firstRectForCharacterRange:(NSRange)inRange
{
	return [self firstRectForCharacterRange:inRange actualRange:NULL];
}

//#558
/**
縦書きかどうかを取得
*/
- (BOOL)drawsVerticallyForCharacterAtIndex:(NSUInteger)charIndex
{
	return mVerticalMode;
}

@end

/*#0 10.4以下はサポート外にする
@implementation CocoaUserPaneTextInputView
@end
*/
