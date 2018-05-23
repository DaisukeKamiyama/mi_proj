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

CPrintImp

*/

#pragma once

#include "../AbsBase/ABase.h"
//#include "../AbsFrame/AApplication.h"
#include "../AbsFrame/AbsFrame.h"

class CWindowImp;
class CTextDrawData;

@class CocoaPrintView;//#558

class CUserPaneCocoaObjects;

#pragma mark ===========================
#pragma mark [クラス]CPrintImp
/**
PrintImpクラス（OS APIを直接コールし、OS間の違いを吸収する）（ドキュメント１つに対して１つ）
*/
class CPrintImp: public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	CPrintImp( const ADocumentID inDocumentID );
	~CPrintImp();
  private:
	ADocumentID				mDocumentID;
	//#1034 ABool					mUseSheet;//B0335
	
	//#1034
  public:
	CUserPaneCocoaObjects&	GetCocoaObjects() { return (*CocoaObjects); }
	const CUserPaneCocoaObjects&	GetCocoaObjectsConst() const { return (*CocoaObjects); }
  private:
	CUserPaneCocoaObjects*				CocoaObjects;
	
	/*#902
	//
  public:
	void			SetPrintPagePrefData( const APrintPagePrefData& inSetupData );
	void			GetPrintPagePrefData( APrintPagePrefData& outSetupData );
	*/
	
	//ページ設定
  public:
	void			PageSetup( /*#182 CWindowImp& inWindowImp*/ const AWindowRef inWindowRef );
	//#1034 static pascal void	PageSetupDoneProc( PMPrintSession printSession, WindowRef documentWindow, Boolean accepted );
	
	//プリント
  public:
	void			Print( /*#182 CWindowImp& inWindowImp*/const AWindowRef inWindowRef, const AText& inJobTitle );
	//#1034 void			PrintLoop( PMPrintSession inPrintSession, const ABool inAccepted );
	//#1034 static pascal void	PrintDoneProc( PMPrintSession printSession, WindowRef documentWindow, Boolean accepted );
  private:
	CocoaPrintView*	mPrintView;//#558
	
	//win 080729
  public:
	AFloatNumber	GetXInterfaceSizeByMM( const AFloatNumber inMillimeter ) const;
	AFloatNumber	GetYInterfaceSizeByMM( const AFloatNumber inMillimeter ) const;
  private:
	AFloatNumber	mXResolution;
	AFloatNumber	mYResolution;
	
	/*#1034
  private:
	void			SetupPageFormat();//#1033
	*/
	
	//描画ルーチン
  public:
	void			DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification = kJustification_Left );
	void			DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, 
							  const AJustification inJustification = kJustification_Left );
	void			DrawText( const ALocalRect& inDrawRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification = kJustification_Left );
	void			DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification = kJustification_Left );
	void			DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification = kJustification_Left );
	void			DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, CTextDrawData& inTextDrawData, 
							  const AJustification inJustification = kJustification_Left );
	void			DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
							  const AFloatNumber inAlpha = 1.0, const AFloatNumber inLineDash = 0.0, const AFloatNumber inLineWidth = 1.0 ) const;
	AFloatNumber	GetDrawTextWidth( const AText& inText );//#1131
	AFloatNumber	GetDrawTextWidth( CTextDrawData& inTextDrawData );//#1131
  private:
	ATSUTextLayout	CreateTextLayout( const AText& inUTF16Text, const ABool inUseFontFallback ) const;//#703
	CTLineRef	GetCTLineFromTextDrawData( CTextDrawData& inTextDrawData );//#1034
	void	UpdateFont();//#1034
	AText						mTextForMetricsCalculation;//#1034
	//描画属性設定
  public:
	/*win void			SetDefaultTextProperty( const AFont inFont, const AFloatNumber inFontSize, const AColor& inColor, const ATextStyle inStyle, 
			const ABool inAntiAliasing );*/
	void			SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, const AColor& inColor, const ATextStyle inStyle, 
			const ABool inAntiAliasing );//win
	void			GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent );
  protected:
	//#688 AFontNumber					mFont;
	AText						mFontName;//#688
	AFloatNumber				mFontSize;
	AColor						mColor;
	ATextStyle					mStyle;
	ABool						mAntiAliasing;
	//#1034 ATSUStyle					mATSUTextStyle;
	
	//縦書き #558
  public:
	void			SetVerticalMode( const ABool inVerticalMode );
  private:
	void			AddVerticalAttribute( NSMutableAttributedString *inAttrString, NSString* inString );
	ABool						mVerticalMode;
	ABool						mVerticalCTM;
	
	//描画callback #1034
  public:
	void			APICB_CocoaDrawRect( const ALocalRect& inDirtyRect );
	
	/*#1034
	//
  private:
	PMPageFormat				mPageFormat;
	PMPrintSettings				mPrintSettings;
	PMSheetDoneUPP				mPageFormatDoneProc;
	PMSheetDoneUPP				mPrintDoneProc;
	*/
	
	CGContextRef				mContextRef;
	
	//CWindowImp*					mParentWindow;//暫定
};

//#1034
/**
CocoaPrintViewクラス
*/
@interface CocoaPrintView : NSControl
{
	CPrintImp*	mPrintImp;
	NSInteger	mTotalPageCount;
}
- (id)initWithFrame:(NSRect)inFrame printImp:(CPrintImp*)inPrintImp totalPageCount:(NSInteger)totalPageCount;

@end

