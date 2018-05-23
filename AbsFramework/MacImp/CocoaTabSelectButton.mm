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
�^�u�I��
*/
- (void)selectTabView
{
	[[tabViewItem tabView] selectTabViewItem:tabViewItem];
}

/**
��Ԑݒ莞�̌p������
*/
- (void)setState:(NSInteger)value
{
	//�p���������s
	[super setState:value];
	//On�ɂȂ�����tab�I��
	if( value == NSOnState )
	{
		[[tabViewItem tabView] selectTabViewItem:tabViewItem];
	}
}

@end


