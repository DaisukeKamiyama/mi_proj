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
#pragma mark [�N���X]APrintPagePrefData

class APrintPagePrefData
{
  public:
	APrintPagePrefData();
	~APrintPagePrefData();
	
};

#pragma mark ===========================
#pragma mark [�N���X]CPrintImp
/**
��������N���X�i�h�L�������g�P�ɑ΂��ĂP�j
*/
class CPrintImp: public AObjectArrayItem
{
	//�R���X�g���N�^�^�f�X�g���N�^
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
	//�y�[�W�ݒ�
  public:
	void			PageSetup( const AWindowRef inWindowRef );
	
	//�v�����g
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
	
	//�`�惋�[�`��
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
	//�`�摮���ݒ�
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
