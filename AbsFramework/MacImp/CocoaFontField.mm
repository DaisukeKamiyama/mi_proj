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

CocoaWindow

*/

#import "CocoaFontField.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

@implementation CocoaFontField

#pragma mark ===========================
#pragma mark [クラス]CocoaFontField
#pragma mark ===========================

#pragma mark ===========================

#pragma mark ---初期設定

/**
初期設定
*/
- (void)setup:(CWindowImp*)inWindowImp
{
	mWindowImp = inWindowImp;
	fontSizeData = 9.0;
}

#pragma mark ===========================

#pragma mark ---

/**
フォント変更ボタンクリック時処理（IBにて、この関数を、フォント変更ボタンからactionでつなぐ）
*/
- (IBAction) doFontChangeButton:(id)sender
{
	//現在の(TextFieldの)フォントをフォントパネルに設定する
	[[NSFontManager sharedFontManager] setSelectedFont:[self font] isMultiple:NO];
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
	NSFont *oldFont = [self font];
	NSFont *newFont = [sender convertFont:oldFont];
	[self setFont:newFont];
	
	OS_CALLBACKBLOCK_START(false);
	
	//AWindow::EVT_ValueChanged()実行
	mWindowImp->APICB_CocoaDoFontFieldChanged([self tag]);
	
	OS_CALLBACKBLOCK_END;
}

/**
フォント設定
*/
- (void)setFont:(NSFont *)font
{
	//フォントサイズデータ記憶（表示フォントは上限ありだが、こちらは上限無し）
	fontSizeData = [font pointSize];
	//表示フォント設定（上限あり）
	CGFloat	fontsize = fontSizeData;
	if( fontsize > 16.0 )   fontsize = 16.0;
	NSFont	*displayFont = [NSFont fontWithName:[font fontName] size:fontsize];
	[super setFont:displayFont];
	//フォントテキスト設定
	NSString*	str = [[[NSString alloc] initWithFormat:@"%@, %1.1f pt",[font displayName],[font pointSize]] autorelease];
	[self setStringValue:str];
}

/**
フォントサイズデータ取得
*/
- (CGFloat)getFontSizeData
{
	return fontSizeData;
}

@end


