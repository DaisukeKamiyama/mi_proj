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

CocoaCaretOverlayWindow

*/

#import <Cocoa/Cocoa.h>
#include "../AbsBase/Cocoa.h"

/**
CocoaCaretOverlayWindowクラス
キャレット表示用ウインドウクラス
*/
@interface CocoaCaretOverlayWindow : NSPanel
{
}
@end


/**
CocoaCaretOverlayWindowクラス
キャレット表示用viewクラス
*/
@interface CocoaCaretOverlayView : NSView
{
	ABool	drawCaretLine;
	ABool	drawCaretRect;
	ALocalPoint	caretStartPoint;
	ALocalPoint	caretEndPoint;
	ALocalRect	caretRect;
	ANumber	caretLineWidth;
	AColor	caretColor;
	ABool	caretIsDashed;
}
- (void)setCaretLineFrom:(ALocalPoint)spt to:(ALocalPoint)ept width:(ANumber)width color:(AColor)color isDash:(ABool)isDash isShow:(ABool)isShow;
- (void)setCaretRect:(ALocalRect)rect width:(ANumber)width color:(AColor)color isShow:(ABool)isShow;
@end

