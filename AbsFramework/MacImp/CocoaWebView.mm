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

CocoaWebView

*/

#import "CocoaWebView.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"


@implementation CocoaWebView

/**
init
*/
- (id)initWithFrame:(NSRect)frameRect frameName:(NSString *)frameName groupName:(NSString *)groupName withControlID:(AControlID)inControlID
{
	//ControlID設定
	mControlID = inControlID;
	//継承処理実行
	self = [super initWithFrame:frameRect frameName:frameName groupName:groupName];
	if( self != nil )
	{
		[self setFrameLoadDelegate:self];
	}
	return self;
}

/**
tag(=ControlID)を返す
*/
- (NSInteger)tag
{
	return mControlID;
}

- (void)webView:(WebView *)sender didFailLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{
	//#1012 NSLog(error);
}

@end



