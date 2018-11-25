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

#include "CUserPane.h"
#import <Cocoa/Cocoa.h>

/**
UserPaneViewクラス
*/
@interface CocoaUserPaneView : NSControl//<NSToolTipOwner>
{
	AControlID	mControlID;
	CWindowImp*	mWindowImp;
	
	//フォーカス可否
	BOOL	mAcceptFocus;
	
	//deactivateウインドウをクリックしたときの最初のマウスクリックを処理するかどうか
	BOOL	mAcceptsFirstMouse;
	
	//
	BOOL	mNeedsPanelToBecomeKey;
	
	//不透明かどうか #1090
	BOOL	mIsOpaque;
	
	//縦書きモード #558
	BOOL	mVerticalMode;
	
	//サービスメニュー可否 #1309
	BOOL	mServiceMenuAvailable;
	
	//mouse tracking rect
	//AArray<ALocalRect>	mAdditionalTrackingRectArray;
	AArray<NSTrackingRectTag>	mTrackingRectTagArray;
	
	//キー処理済みフラグ
	//#1015 ABool	mKeyHandled;
	
	//InlineInput用ダミーテキスト
	NSMutableString*	mCurrentInlineInputString;
	
	//変換テキストのUTF16/UTF8変換データ
	AArray<AIndex>	mCurrentInlineInputUTF16PosIndexArray;
	AArray<AIndex>	mCurrentInlineInputUTF8PosIndexArray;
	
	//resetSelectedRangeForTextInputによる処理を抑制するフラグ
	ABool	mSuppressResetSelectedRange;
	
	//変換テキストrange
	NSRange	mMarkedRange;
	//ダミーテキスト内選択range
	NSRange	mSelectedRange;
}

- (id)initWithFrame:(NSRect)inFrame windowImp:(CWindowImp*)inWindowImp controlID:(AControlID)inControlID;
- (void)setAcceptFocus:(ABool)inAcceptFocus;
- (void)setAcceptsFirstMouse:(ABool)inAcceptsFirstMouse;
- (void)setNeedsPanelToBecomeKey:(ABool)inNeedsPanelToBecomeKey;
- (void)setOpaque:(ABool)inOpqque;
- (void)setVerticalMode:(ABool)inVertical;//#558
- (void)setServiceMenuAvailable:(ABool)inAvailable;//#1309
- (void)resetSelectedRangeForTextInput;
//- (void)setMouseTrackingRect:(const AArray<ALocalRect>&)inLocalRectArray;
- (void)fixInlineInput;

//
- (void)moveToBeginningOfParagraphAndModifySelection:(id)sender;
//
- (void)indenttab:(id)sender;

@end

/**
UserPaneViewクラス + NSTextInputClient(Mac OS X 10.5以降)
*/
@interface CocoaUserPaneTextInputClientView : CocoaUserPaneView<NSTextInputClient>
{
}
@end

/**
UserPaneViewクラス + NSTextInput(Mac OS X 10.4以前)
*/
/*#0 10.4以下はサポート外にする
@interface CocoaUserPaneTextInputView : CocoaUserPaneView<NSTextInput>
{
}
@end
*/

