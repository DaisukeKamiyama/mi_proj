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

CocoaLiveResizeDetectionView

*/

#import "CocoaLiveResizeDetectionView.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

@implementation CocoaLiveResizeDetectionView

#pragma mark ===========================
#pragma mark [クラス]CocoaLiveResizeDetectionView
#pragma mark ===========================

/*
ウインドウのLiveResize完了を検知するためのダミーview。
NSWindowのLiveResize検知メソッドは10.3等でサポートされていないため、（Resize途中は検知できるが、途中なのか完了なのかが判定できない）
このviewを常に登録しておくことで、LiveResize完了を検知する。
*/

/**
初期設定
*/
- (id)initWithWindowImp:(CWindowImp*)inWindowImp
{
	//継承処理実行
	self = [super init];
	//初期設定
	if( self != nil )
	{
		mWindowImp = inWindowImp;
		//常に非表示にする
		[self setHidden:YES];
	}
	return self;
}

/**
KeyView可否
*/
- (BOOL)canBecomeKeyView
{
	return NO;
}

/**
FirstResponder可否
*/
- (BOOL)acceptsFirstResponder
{
	return NO;
}


/**
LiveResize完了時処理
*/
- (void)viewDidEndLiveResize
{
	//継承処理実行
	[super viewDidEndLiveResize];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_WindowResizeCompleted()実行
	mWindowImp->APICB_CocoaWindowResizeCompleted();
	
	OS_CALLBACKBLOCK_END;
}

@end

