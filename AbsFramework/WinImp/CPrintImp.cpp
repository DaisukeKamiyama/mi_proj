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

#include "stdafx.h"

#include "CPrintImp.h"
#include "CWindowImp.h"
#include "CTextDrawData.h"
#include "../AbsFrame/AApplication.h"
#include "../AbsFrame/ADocument.h"
#include <CommDlg.h>
#include <windowsx.h>
#include <commctrl.h>

#pragma mark ===========================
#pragma mark [クラス]APrintPagePrefData
#pragma mark ===========================

#pragma mark --- コンストラクタ／デストラクタ

APrintPagePrefData::APrintPagePrefData()
{
}

APrintPagePrefData::~APrintPagePrefData()
{
}

#pragma mark ===========================
#pragma mark [クラス]CPrintImp
#pragma mark ===========================

#pragma mark --- コンストラクタ／デストラクタ

//コンストラクタ
CPrintImp::CPrintImp( const ADocumentID inDocumentID ) : AObjectArrayItem(NULL), mDocumentID(inDocumentID),
		mHDC(0), mHDevMode(NULL), mHDevNames(NULL)
{
	mMargin.left		= 1000;
	mMargin.top			= 1000;
	mMargin.right		= 1000;
	mMargin.bottom		= 1000;
}

CPrintImp::~CPrintImp()
{
	//
	if( mHDevMode != NULL )
	{
		::GlobalFree(mHDevMode);
	}
	if( mHDevNames != NULL )
	{
		::GlobalFree(mHDevNames);
	}
}

/*#902
#pragma mark ===========================

#pragma mark --- 

void	CPrintImp::SetPrintPagePrefData( const APrintPagePrefData& inSetupData )
{
}

void	CPrintImp::GetPrintPagePrefData( APrintPagePrefData& outSetupData )
{
}
*/
#pragma mark ===========================

#pragma mark --- ページ設定

void	CPrintImp::PageSetup( const AWindowRef inWindowRef )
{
	PAGESETUPDLG	psd;
	memset(&psd, 0, sizeof(psd));
	psd.lStructSize		= sizeof(psd);
	psd.hwndOwner		= inWindowRef;
	psd.hDevMode		= mHDevMode;
	psd.hDevNames		= mHDevNames;
	psd.Flags			= PSD_MARGINS | PSD_INHUNDREDTHSOFMILLIMETERS;
	psd.rtMargin		= mMargin;
	if( ::PageSetupDlg(&psd) == TRUE )
	{
		mHDevMode = psd.hDevMode;
		mHDevNames = psd.hDevNames;
		mMargin = psd.rtMargin;
	}
}

#pragma mark ===========================

#pragma mark --- プリント

const AControlID	kControlID_Progress = 102;
const AControlID	kControlID_Cancel = 103;
const AControlID	kControlID_Caption = 104;

HWND			CPrintImp::sCancelDialog;
AText			CPrintImp::sTitle;
ABool			CPrintImp::sCanceled;

void	CPrintImp::Print( const AWindowRef inWindowRef, const AText& inJobTitle )
{
	PRINTDLG pd;
	
	// Initialize PRINTDLG
	memset(&pd, 0, sizeof(pd));
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner   = inWindowRef;
	pd.hDevMode    = mHDevMode;
	pd.hDevNames   = mHDevNames;
	pd.Flags       = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_NOSELECTION; 
	pd.nCopies     = 1;
	pd.nFromPage   = 1; 
	pd.nToPage     = 1; 
	pd.nMinPage    = 1; 
	pd.nMaxPage    = 0xFFFF; 
	
	if (::PrintDlg(&pd)==TRUE) 
	{
		//
		mHDC = pd.hDC;
		mHDevMode = pd.hDevMode;
		mHDevNames = pd.hDevNames;
		
		// Zero and then initialize the members of a DOCINFO structure.
		AStCreateWcharStringFromAText	titlestr(inJobTitle);
		DOCINFO	di;
		::memset( &di, 0, sizeof(DOCINFO) );
		di.cbSize	= sizeof(DOCINFO);
		di.lpszDocName	= (LPCTSTR)(titlestr.GetPtr());
		di.lpszOutput	= (LPTSTR) NULL;
		di.lpszDatatype	= (LPTSTR) NULL;
		di.fwType		= 0;
		
		//キャンセルダイアログ
		sTitle.SetText(inJobTitle);
		sCanceled = false;
		{
			AText	name;
			//CAppImp::GetLangText(name);
			name.SetCstring("jp_main_PrintCancel");
			AStCreateWcharStringFromAText	namestr(name);
			sCancelDialog = ::CreateDialog(CAppImp::GetHResourceInstance(),namestr.GetPtr(),//satDLL
					inWindowRef,(DLGPROC)CPrintImp::CancelDlgProc);
		}
		::ShowWindow(sCancelDialog,SW_SHOW);
		::SetAbortProc(mHDC,CPrintImp::AbortProc);
		
		try
		{
			 // Begin a print job by calling the StartDoc function.
			if( ::StartDoc(mHDC, &di) == SP_ERROR )
			{
				throw 0;
			}
			
			//
			//PHYSICALWIDTH, PHYSICALHEIGHT: デバイス単位（＝ピクセル）で紙全体の物理的なサイズを取得
			//LOGPIXELSX, LOGPIXELSY: 1インチあたりのピクセル数
			//1インチは25.4mm
			//mMarginはmm単位のサイズに100をかけたもの
			//DCの座標系はprintable areaの左上が(0,0)（紙全体の左上ではない）
			ALocalRect	pageRect;
			pageRect.left		= 0										
								+ mMargin.left		*::GetDeviceCaps(mHDC,LOGPIXELSX)/2540
								- ::GetDeviceCaps(mHDC, PHYSICALOFFSETX);
			pageRect.top		= 0										
								+ mMargin.top		*::GetDeviceCaps(mHDC,LOGPIXELSY)/2540
								- ::GetDeviceCaps(mHDC, PHYSICALOFFSETY);
			pageRect.right		= - ::GetDeviceCaps(mHDC, PHYSICALOFFSETX)
								+ ::GetDeviceCaps(mHDC, PHYSICALWIDTH)	
								- mMargin.right		*::GetDeviceCaps(mHDC,LOGPIXELSX)/2540;
			pageRect.bottom		= - ::GetDeviceCaps(mHDC, PHYSICALOFFSETY)
								+ ::GetDeviceCaps(mHDC, PHYSICALHEIGHT)	
								- mMargin.bottom	*::GetDeviceCaps(mHDC,LOGPIXELSY)/2540;
			AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_StartPrintMode(pageRect);
			AItemCount	totalPageCount = AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_GetPrintPageCount();
			ANumber	startPageNumber = 1;
			ANumber	endPageNumber = totalPageCount;
			if( (pd.Flags&PD_PAGENUMS) != 0 )
			{
				if( pd.nFromPage >= 1 && pd.nFromPage <= totalPageCount )
				{
					startPageNumber = pd.nFromPage;
				}
				if( pd.nToPage >= 1 && pd.nToPage <= totalPageCount )
				{
					endPageNumber = pd.nToPage;
				}
			}
			//progress
			HWND	progressbar = ::GetDlgItem(sCancelDialog,kControlID_Progress);
			::SendMessageW(progressbar,PBM_SETRANGE,0,(LPARAM)MAKELPARAM(0,endPageNumber-startPageNumber));
			::SendMessageW(progressbar,PBM_SETPOS,(WPARAM)0,0);
			
			for( ANumber pageNumber = startPageNumber; pageNumber <= endPageNumber; pageNumber++ )
			{
				// Inform the driver that the application is about to begin
				// sending data.
				
				if( ::StartPage(mHDC) <= 0 )
				{
					throw 0;
				}
				
				//
				AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID).EVT_PrintPage(pageNumber-1);
				
				//
				::EndPage(mHDC);
				
				//中止判定
				if( sCanceled == true )   throw 0;
				
				//progress
				::SendMessageW(progressbar,PBM_SETPOS,(WPARAM)pageNumber+1-startPageNumber,0);
				
			}
			//
			::EndDoc(mHDC);
		}
		catch(...)
		{
			::AbortDoc(mHDC);
		}
		//
		if( sCancelDialog != 0 )
		{
			::DestroyWindow(sCancelDialog);
		}
		
		// Delete DC when done.
		::DeleteDC(mHDC);
	}
}

LRESULT CALLBACK	CPrintImp::CancelDlgProc( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch(msg)
	{
	  case WM_INITDIALOG:
		{
			::SetFocus(hWnd);
			AText	text;
			text.SetLocalizedText("PrintCancelDialog");
			text.ReplaceParamText('0',sTitle);
			AStCreateWcharStringFromAText	str(text);
			::SetDlgItemText(hWnd,kControlID_Caption,str.GetPtr());
			return TRUE;
		}
	  case WM_COMMAND:
		{
			if (LOWORD(wp) == kControlID_Cancel)
			{
				sCanceled = TRUE;
				::DestroyWindow(sCancelDialog);
				sCancelDialog = 0;
				return TRUE;
			}
			break;
		}
	  default:
		{
			break;
		}
	}
	return FALSE; 
}

BOOL CALLBACK	CPrintImp::AbortProc(HDC, int)
{
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{ 
		if (!IsDialogMessage(sCancelDialog, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (!sCanceled);
}

ANumber	CPrintImp::GetXInterfaceSizeByMM( const AFloatNumber inMillimeter )
{
	return static_cast<ANumber>(inMillimeter*::GetDeviceCaps(mHDC,LOGPIXELSX)/25.4);
}

ANumber	CPrintImp::GetYInterfaceSizeByMM( const AFloatNumber inMillimeter )
{
	return static_cast<ANumber>(inMillimeter*::GetDeviceCaps(mHDC,LOGPIXELSY)/25.4);
}

#pragma mark ===========================

#pragma mark ---描画ルーチン

void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification ) const
{
	DrawText(inDrawRect,inDrawRect,inText,mColor,mStyle,inJustification);
}

void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, const AJustification inJustification ) const
{
	DrawText(inDrawRect,inClipRect,inText,mColor,mStyle,inJustification);
}

//テキスト表示
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification ) const
{
	DrawText(inDrawRect,inDrawRect,inText,inColor,inStyle,inJustification);
}

void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification ) const
{
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(inText);
	textDrawData.UTF16DrawText.ConvertFromUTF8ToUTF16();
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(inColor);
	textDrawData.attrStyle.Add(inStyle);
	DrawText(inDrawRect,inClipRect,textDrawData,inJustification);
}

//テキスト表示
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification ) const
{
	DrawText(inDrawRect,inDrawRect,inTextDrawData,inJustification);
}
void	CPrintImp::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, CTextDrawData& inTextDrawData, const AJustification inJustification ) const
{
	::SelectObject(mHDC,mHDefaultFont);
	::SetBkMode(mHDC,TRANSPARENT);
	
	ANumber	textWidth = 1;
	ANumber	x = inDrawRect.left;
	{
		AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
		
		SIZE	textsize;
		::GetTextExtentPoint32W(mHDC,(LPCWSTR)(utf16textptr.GetPtr()),static_cast<int>(utf16textptr.GetByteCount()/sizeof(AUTF16Char)),&textsize);
		textWidth = textsize.cx;
		{
			switch(inJustification)
			{
			  case kJustification_Right:
				{
					x = inDrawRect.right - textWidth;
					break;
				}
			  case kJustification_Center:
				{
					x = (inDrawRect.left+inDrawRect.right)/2 - textWidth/2;
					break;
				}
			}
		}
		
		//clips
		HRGN	hrgn = ::CreateRectRgn(inClipRect.left,inClipRect.top,inClipRect.right,inClipRect.bottom);
		::SelectClipRgn(mHDC,hrgn);
		
		::SetTextAlign(mHDC,TA_UPDATECP);
		::MoveToEx(mHDC,x,inDrawRect.top,NULL);
		if( inTextDrawData.attrPos.GetItemCount() > 0 )
		{
		for( AIndex i = 0; i < inTextDrawData.attrPos.GetItemCount(); i++ )
		{
			//lenに、処理するRunLengthの、UniCharでの長さを格納する。
			AItemCount	len;
			if( i+1 < inTextDrawData.attrPos.GetItemCount() )
			{
				len = inTextDrawData.attrPos.Get(i+1) - inTextDrawData.attrPos.Get(i);
			}
			else
			{
				len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - inTextDrawData.attrPos.Get(i);
			}
			if( len <= 0 )   continue;
			
			AColor	color = inTextDrawData.attrColor.Get(i);
			::SetTextColor(mHDC,color);
			::TextOutW(mHDC,0,0,
					(LPCWSTR)(utf16textptr.GetPtr()+inTextDrawData.attrPos.Get(i)*sizeof(AUTF16Char)),len);
		}
		}
		else
		{
			::SetTextColor(mHDC,mColor);
			::TextOutW(mHDC,0,0,(LPCWSTR)(utf16textptr.GetPtr()),static_cast<int>(utf16textptr.GetByteCount()/sizeof(AUTF16Char)));
		}
		
		::DeleteObject(hrgn);
		::SelectClipRgn(mHDC,NULL);
	}
}

void	CPrintImp::DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
		const AFloatNumber inAlpha, const AFloatNumber inLineDash, const AFloatNumber inLineWidth ) const
{
	HPEN	hpen = NULL;
	if( inLineDash == 0.0 )
	{
		hpen = ::CreatePen(PS_SOLID,static_cast<int>(inLineWidth),inColor);
	}
	else
	{
		hpen = ::CreatePen(PS_DOT,1,inColor);
		//★暫定
	} 
	if( hpen == NULL )   {_ACError("",NULL);return;}
	::SetBkMode(mHDC,TRANSPARENT);
	::SelectObject(mHDC,hpen);
	::MoveToEx(mHDC,inStartPoint.x,inStartPoint.y,NULL);
	::LineTo(mHDC,inEndPoint.x,inEndPoint.y);
	::DeleteObject(hpen);
	//★アルファ未対応
}

#pragma mark ===========================

#pragma mark ---描画属性設定

//テキストデフォルト属性設定 
void	CPrintImp::SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, 
		const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing )
{
	mColor = inColor;
	mStyle = inStyle;
	if( mHDefaultFont != NULL )
	{
		::DeleteObject(mHDefaultFont);
	}
	if( mHDC == NULL )   {_ACError("",NULL);return;}
	mHDefaultFont = CWindowImp::CreateFont_(mHDC,inFontName,inFontSize,inStyle,inAntiAliasing);
}

//
void	CPrintImp::GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent ) const
{
	if( mHDC == NULL )   {_ACError("",NULL);return;}
	
	::SelectObject(mHDC,mHDefaultFont);
	TEXTMETRIC	tm;
	::GetTextMetrics(mHDC,&tm);
	outLineHeight = tm.tmHeight;
	outLineAscent = tm.tmAscent;
}




