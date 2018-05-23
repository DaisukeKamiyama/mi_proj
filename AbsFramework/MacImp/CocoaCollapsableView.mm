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

CocoaCollapsableView, CocoaDisclosureButton
開閉可能なビューと、開閉用三角ボタン

*/

#import "CocoaCollapsableView.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

@implementation CocoaCollapsableView

/**
初期設定
*/
- (id)initWithFrame:(NSRect)inFrame
{
	//継承処理実行
	self = [super initWithFrame:inFrame];
	if( self != nil )
	{
		//開いたときの高さを記憶
		expandedHeight = [self frame].size.height;
	}
	return self;
}

/**
開閉用トライアングルボタンクリック時処理
*/
- (IBAction) doDisclosureButton:(id)sender
{
	//閉じたときの高さ
	const ANumber	kCollapsedViewHeight = 20;
	
	if( [sender isKindOfClass:[NSButton class]] == YES )
	{
		NSButton*	button = static_cast<NSButton*>(sender);
		//トライアングルボタンのON/OFFによって、viewのサイズ設定
		if( [button state] == NSOffState )
		{
			//旧frameを取得
			NSRect	oldFrame = [self frame];
			//top位置を保ったまま、閉じたときの高さにする
			NSRect	newFrame = oldFrame;
			newFrame.size.height = kCollapsedViewHeight;
			newFrame.origin.y = oldFrame.origin.y + oldFrame.size.height -kCollapsedViewHeight;
			[self setFrame:newFrame];
		}
		else
		{
			//旧frameを取得
			NSRect	oldFrame = [self frame];
			//top位置を保ったまま、開いたときの高さにする
			NSRect	newFrame = oldFrame;
			newFrame.size.height = expandedHeight;
			newFrame.origin.y = oldFrame.origin.y + oldFrame.size.height -expandedHeight;
			[self setFrame:newFrame];
		}
	}
	//登録されている次viewの位置をずらす
	[self updateNextViewFrame];
}

/**
登録されている次viewの位置をずらす
*/
- (void) updateNextViewFrame
{
	if( nextView != nil )
	{
		//次ビューのtop位置を、自ビューの真下にする
		NSRect	nextViewFrame = [nextView frame];
		nextViewFrame.origin.y = [self frame].origin.y - nextViewFrame.size.height;
		[nextView setFrame:nextViewFrame];
		//次ビューがCocoaCollapsableViewなら同じ関数を呼んで、連なっているビューの位置をすべてずらす
		if( [nextView isKindOfClass:[CocoaCollapsableView class]] == YES )
		{
			[nextView updateNextViewFrame];
		}
	}
}

@end

@implementation CocoaDisclosureButton
- (void) updateCollapsableView
{
	//collapsable viewで開閉用トライアングルボタンクリック時処理を実行
	if( collapsableView != nil )
	{
		[collapsableView doDisclosureButton:self];
	}
}
@end

