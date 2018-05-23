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

CocoaTabSelectButton

*/

#import "CocoaTabSelectButton.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

@implementation CocoaTabSelectButton

/**
タブ選択
*/
- (void)selectTabView
{
	[[tabViewItem tabView] selectTabViewItem:tabViewItem];
}

/**
状態設定時の継承処理
*/
- (void)setState:(NSInteger)value
{
	//継承処理実行
	[super setState:value];
	//Onになったらtab選択
	if( value == NSOnState )
	{
		[[tabViewItem tabView] selectTabViewItem:tabViewItem];
	}
}

@end


