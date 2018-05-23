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

#pragma mark ===========================
#pragma mark [クラス]APrintPagePrefData

class APrintPagePrefData
{
  public:
	APrintPagePrefData();
	~APrintPagePrefData();
	
};

#pragma mark ===========================
#pragma mark [クラス]CPrintImp
/**
印刷実装クラス（ドキュメント１つに対して１つ）
*/
class CPrintImp: public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	CPrintImp( const ADocumentID inDocumentID );
	~CPrintImp();
  private:
	ADocumentID				mDocumentID;
	
	/*#902
	//
  public:
	void			SetPrintPagePrefData( const APrintPagePrefData& inSetupData );
	void			GetPrintPagePrefData( APrintPagePrefData& outSetupData );
	*/
	//ページ設定
  public:
	void			PageSetup( const AWindowRef inWindowRef );
	
	//プリント
  public:
	void			Print( const AWindowRef inWindowRef, const AText& inJobTitle );
	ANumber			GetXInterfaceSizeByMM( const AFloatNumber inMillimeter );
	ANumber			GetYInterfaceSizeByMM( const AFloatNumber inMillimeter );
  private:
	static LRESULT CALLBACK	CancelDlgProc( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp );
	static BOOL CALLBACK	AbortProc(HDC, int);
	HDC						mHDC;
	HGLOBAL					mHDevMode;
	HGLOBAL					mHDevNames;
	RECT					mMargin;
	static HWND				sCancelDialog;
	static AText			sTitle;
	static ABool			sCanceled;
	
	//描画ルーチン
  public:
	void	DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification = kJustification_Left ) const;
	void	DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, 
	const AJustification inJustification = kJustification_Left ) const;
	void	DrawText( const ALocalRect& inDrawRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification = kJustification_Left ) const;
	void	DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification = kJustification_Left ) const;
	void	DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification = kJustification_Left ) const;
	void	DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, CTextDrawData& inTextDrawData, 
			const AJustification inJustification = kJustification_Left ) const;
	void			DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
			const AFloatNumber inAlpha = 1.0, const AFloatNumber inLineDash = 0.0, const AFloatNumber inLineWidth = 1.0 ) const;
	//描画属性設定
  public:
	void			SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, const AColor& inColor, const ATextStyle inStyle, 
					const ABool inAntiAliasing );//win
	void			GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent ) const;
  protected:
  private:
	HFONT						mHDefaultFont;
	AColor						mColor;
	ATextStyle					mStyle;
};
