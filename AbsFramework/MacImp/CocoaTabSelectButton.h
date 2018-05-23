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

#include "CWindowImp.h"
#import <Cocoa/Cocoa.h>

/**
タブを選択するボタン（ラジオボタン等）
*/
@interface CocoaTabSelectButton : NSButton
{
	//ターゲットとなるtab view
	IBOutlet NSTabViewItem*	tabViewItem;
}
-(void)selectTabView;
@end


