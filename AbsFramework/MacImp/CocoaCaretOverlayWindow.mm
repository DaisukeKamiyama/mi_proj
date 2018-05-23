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

#import "CocoaCaretOverlayWindow.h"
#include "../Frame.h"
#include "../AbsBase/Cocoa.h"
#include "CocoaMacro.h"

@implementation CocoaCaretOverlayWindow

#pragma mark ===========================
#pragma mark [クラス]CocoaCaretOverlayWindow
#pragma mark ===========================

/**
マウスイベントを処理しない（透過する）
これがないと、キャレット上をクリックしたときに、キャレットがマウスイベントを処理してしまい、メインウインドウにマウスイベントが通知されない。
*/
- (BOOL)ignoresMouseEvents
{
	return YES;
}

@end


@implementation CocoaCaretOverlayView

/**
 初期化
 */
- (id)initWithFrame:(NSRect)frameRect
{
	//描画データ消去
	drawCaretLine = false;
	drawCaretRect = false;
	//継承処理実行
	self = [super initWithFrame:frameRect];
	if( self != nil )
	{
	}
	return self;
}

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

//線キャレット周辺の更新領域
const ANumber	kCaretLineUpdateMargin = 2;

/**
キャレット描画データ削除
*/
- (void)clearCaret
{
	//ここはCGContextScaleCTM()範囲外なので、座標系はALocalPointと同じ。
	
	//消去範囲（更新範囲）
	NSRect	updateRect = {0};
	//線描画データ存在していたら線周辺を消去範囲（更新範囲）に設定
	if( drawCaretLine == true )
	{
		//キャレット周辺のrect計算
		updateRect.origin.x = GetMinValue(caretStartPoint.x,caretEndPoint.x) - kCaretLineUpdateMargin;
		updateRect.origin.y = GetMinValue(caretStartPoint.y,caretEndPoint.y) - kCaretLineUpdateMargin;
		updateRect.size.width = abs(caretEndPoint.x-caretStartPoint.x) + kCaretLineUpdateMargin*2;
		updateRect.size.height = abs(caretEndPoint.y-caretStartPoint.y) + kCaretLineUpdateMargin*2;
		//線描画データ存在フラグOFF
		drawCaretLine = false;
	}
	//rect描画データ存在していたらrect内を消去範囲（更新範囲）に設定
	if( drawCaretRect == true )
	{
		//view全体を更新範囲に設定
		updateRect = [self bounds];
		//rect描画データ存在フラグOFF
		drawCaretRect = false;
	}
	//描画更新
	[self setNeedsDisplayInRect:updateRect];
}

/**
キャレット描画データ設定（線）
*/
- (void)setCaretLineFrom:(ALocalPoint)spt to:(ALocalPoint)ept width:(ANumber)width color:(AColor)color isDash:(ABool)isDash isShow:(ABool)isShow
{
	//ここはCGContextScaleCTM()範囲外なので、座標系はALocalPointと同じ。
	
	//現在のキャレットを消去
	[self clearCaret];
	
	//showの場合は、描画データ設定
	if( isShow == true )
	{
		//描画データ設定
		drawCaretLine = true;
		caretStartPoint = spt;
		caretEndPoint = ept;
		caretLineWidth = width;
		caretColor = color;
		caretIsDashed = isDash;
		//描画更新rect
		NSRect	updateRect = {0};
		//キャレット周辺のrect計算
		updateRect.origin.x = GetMinValue(caretStartPoint.x,caretEndPoint.x) - kCaretLineUpdateMargin;
		updateRect.origin.y = GetMinValue(caretStartPoint.y,caretEndPoint.y) - kCaretLineUpdateMargin;
		updateRect.size.width = abs(caretEndPoint.x-caretStartPoint.x) + kCaretLineUpdateMargin*2;
		updateRect.size.height = abs(caretEndPoint.y-caretStartPoint.y) + kCaretLineUpdateMargin*2;
		//描画更新
		[self setNeedsDisplayInRect:updateRect];
	}
}

/**
キャレット描画データ設定（rect）
*/
- (void)setCaretRect:(ALocalRect)rect width:(ANumber)width color:(AColor)color isShow:(ABool)isShow
{
	//ここはCGContextScaleCTM()範囲外なので、座標系はALocalPointと同じ。
	
	//現在のキャレットを消去
	[self clearCaret];
	
	//showの場合は、描画データ設定
	if( isShow == true )
	{
		//描画データ設定
		drawCaretRect = true;
		caretRect = rect;
		caretLineWidth = width;
		caretColor = color;
		caretIsDashed = false;
		//描画更新
		[self setNeedsDisplayInRect:[self bounds]];
	}
}

/**
描画
*/
- (void)drawRect:(NSRect)dirtyRect
{
	//Context取得
	CGContextRef	contextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	::CGContextSaveGState(contextRef);
	::CGContextScaleCTM(contextRef, 1.0, -1.0);
	
	//view内全消去（とはいっても、setNeedsDisplayInRectで指定したinvalid rectの範囲内のみがdrawされる。）
	CGRect	eraseRect = {0};
	eraseRect.origin.x = 0;
	eraseRect.origin.y = -[self frame].size.height;
	eraseRect.size.width = [self frame].size.width;
	eraseRect.size.height = [self frame].size.height;
	::CGContextClearRect(contextRef,eraseRect);
	
	//描画データが無ければ、リターン
	if( drawCaretLine == false && drawCaretRect == false )
	{
		return;
	}
	
	/*テスト用処理
	CGRect	rect = {0};
	rect.origin.x = 0;
	rect.origin.y = -eraseRect.size.height;
	rect.size.width = eraseRect.size.width;
	rect.size.height = eraseRect.size.height;
	::CGContextSetRGBFillColor(contextRef,1,0,0,1);
	::CGContextFillRect(contextRef,rect);
	*/
	
	//色取得
	float	red = caretColor.red;
	float	green = caretColor.green;
	float	blue = caretColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	if( drawCaretLine == true )
	{
		//線描画
		::CGContextBeginPath(contextRef);
		::CGContextSetRGBStrokeColor(contextRef,red,green,blue,1.0);
		::CGContextSetLineWidth(contextRef,caretLineWidth);
		::CGContextSetShouldAntialias(contextRef,false);
		::CGContextMoveToPoint(contextRef,caretStartPoint.x,-caretStartPoint.y-1);
		::CGContextAddLineToPoint(contextRef,caretEndPoint.x,-caretEndPoint.y-1);
		if( caretIsDashed == true )
		{
			float	lengths[2];
			lengths[0] = 1.0;
			lengths[1] = 1.0;
			::CGContextSetLineDash(contextRef,0.0,lengths,2);
		}
		::CGContextStrokePath(contextRef);
	}
	if( drawCaretRect == true )
	{
		//rect描画
		::CGContextSetRGBStrokeColor(contextRef,red,green,blue,1.0);
		::CGContextSetLineWidth(contextRef,caretLineWidth);
		::CGContextSetShouldAntialias(contextRef,false);
		CGRect	r = {0};
		r.origin.x = caretRect.left;
		r.origin.y = -caretRect.bottom;
		r.size.width = caretRect.right - caretRect.left;
		r.size.height = caretRect.bottom - caretRect.top;
		::CGContextStrokeRect(contextRef,r);
	}
	
	//graphic state restore
	::CGContextRestoreGState(contextRef);
}

@end


