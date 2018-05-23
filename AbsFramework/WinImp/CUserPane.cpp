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

	CUserPane (Windows)

*/

#include "stdafx.h"

#include "CUserPane.h"
#include "../Frame.h"
#include <math.h>
#include <strsafe.h>
#include <windowsx.h>
#include <commctrl.h>
#include <windows.h>
#include <strsafe.h>
#include <shellapi.h>
#include <shlobj.h>

AArray<CUserPane*>	CUserPane::sAliveUserPaneArray;
AArray<HWND>		CUserPane::sAliveUserPaneArray_HWnd;

#pragma mark ===========================
#pragma mark [クラス]CUserPane
#pragma mark ===========================

#pragma mark --- コンストラクタ／デストラクタ

//コンストラクタ
CUserPane::CUserPane( AObjectArrayItem* inParent, CWindowImp& inWindowImp ) 
: AObjectArrayItem(inParent), mWindowImp(inWindowImp), mHWnd(0), mControlID(0), 
		mOffscreenDC(NULL), mOffscreenBitmap(NULL), mColor(kColor_Black), mStyle(kTextStyle_Normal),
		mMouseTrackingMode(false), mInWMPaint(false), mDrawLinePen(NULL), mDropTarget(NULL), 
		mMouseLeaveEventEnable(false), mMouseLeaveEventTracking(false), 
		mRedirectTextInput(false)
		,mDefaultCursorType(kCursorType_Arrow)
		//#360,mSupportFocus(true)//#291
		,mOffscreenWidth(0),mOffscreenHeight(0)
		,mPreparingOffscreen(false)
		,mFontSize(9.0), mAntiAliasing(true)
		,mOffscreenMode(false), mPaintDC(NULL)
{
	//
	sAliveUserPaneArray.Add(this);
	sAliveUserPaneArray_HWnd.Add(0);
	//
	for( AIndex i = 0; i < 8; i++ )
	{
		mHDefaultFont[i] = NULL;
	}
}

//デストラクタ
CUserPane::~CUserPane()
{
	//ここにくるときはDestroyUserPane()ですでにウインドウ削除済みでなくてはならない。
	if( mHWnd != 0 )
	{
		_ACError("",this);
	}
	//デストラクタでDestroyWindow()等は行わない。DestroyUserPane()を呼ぶこと。
	//デストラクタの実行はイベントトランザクションの最後になるので、ここでUI処理を行うと、
	//削除シーケンスが崩れる（APICB_ControlSubProc()がコールされ、ControlIDが見つからない）
	//
	AIndex	index = sAliveUserPaneArray.Find(this);
	if( index != kIndex_Invalid )
	{
		sAliveUserPaneArray.Delete1(index);
		sAliveUserPaneArray_HWnd.Delete1(index);
	}
}

//AWindow取得
AWindow&	CUserPane::GetAWin()
{
	return mWindowImp.GetAWin();
}

#pragma mark ===========================

#pragma mark ---コントロール情報取得

//自分のControlID取得
AControlID	CUserPane::GetControlID() const
{
	return mControlID;
}

//コントロールの位置取得
void	CUserPane::GetControlPosition( AWindowPoint& outPoint ) const
{
	GetWinConst().GetControlPosition(GetControlID(),outPoint);
}

//コントロール全体のRect取得
void	CUserPane::GetControlLocalFrameRect( ALocalRect& outRect ) const
{
	outRect.left 	= 0;
	outRect.top 	= 0;
	outRect.right 	= GetWinConst().GetControlWidth(GetControlID());
	outRect.bottom 	= GetWinConst().GetControlHeight(GetControlID());
}

#pragma mark ===========================

#pragma mark --- ウインドウ制御

//Wnd生成
void	CUserPane::CreateUserPane( const HWND inParent, const AControlID inControlID, const APoint& inPoint, 
			const ANumber inWidth, const ANumber inHeight, const ABool inFront )
{
	mControlID = inControlID;
	mHWnd = ::CreateWindowW(L"userpane",NULL,WS_CHILDWINDOW|WS_VISIBLE,
			inPoint.x,inPoint.y,inWidth,inHeight,
			inParent,
			(HMENU)inControlID,//子ウインドウID
			CAppImp::GetHInstance(),NULL);
	DWORD	err = GetLastError();
	if( mHWnd == 0 )
	{
		_ACThrow("CreateUserPane() failed",this);
	}
	if( inFront == true )
	{
		::SetWindowPos(mHWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	else
	{
		::SetWindowPos(mHWnd,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	
	//HWND登録
	AIndex	index = sAliveUserPaneArray.Find(this);
	if( index == kIndex_Invalid )
	{
		_ACThrow("",this);
	}
	sAliveUserPaneArray_HWnd.Set(index,mHWnd);
}

void	CUserPane::DestroyUserPane()
{
	//
	if( mDrawLinePen != NULL )
	{
		::DeleteObject(mDrawLinePen);
		mDrawLinePen = NULL;
	}
	//DropTarget削除
	if( mDropTarget != NULL )
	{
		//Drag&Drop登録削除
		::RevokeDragDrop(mHWnd);
		//CDropTarget削除（参照回数減算）
		mDropTarget->Release();
		//
		mDropTarget = NULL;
	}
	//Offscreen削除
	DisposeOffscreen();
	//
	for( AIndex i = 0; i < 8; i++ )
	{
		if( mHDefaultFont[i] != NULL )
		{
			::DeleteObject(mHDefaultFont[i]);
			mHDefaultFont[i] = NULL;
		}
	}
	//WND削除
	//WM_DESTROYからDestroyUserPane()をコールするようにしたのでここはコメントアウト::DestroyWindow(mHWnd);
	mHWnd = 0;
}

void	CUserPane::EnableDrop( const AArray<AScrapType>& inScrapType )
{
	if( mHWnd == 0 )   {_ACError("",this);return;}
	if( mDropTarget == NULL )
	{
		mDropTarget = new CDropTarget(*this);
		::RegisterDragDrop(mHWnd,mDropTarget);
	}
	for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
	{
		mDropTarget->AddAcceptableType(inScrapType.Get(i));
	}
}

//
void	CUserPane::SetMouseTrackingMode( const ABool inSet )
{
	if( inSet == true )
	{
		::SetCapture(mHWnd);
		mMouseTrackingMode = true;
	}
	else
	{
		::ReleaseCapture();
		mMouseTrackingMode = false;
	}
}

//
void	CUserPane::EnableMouseLeaveEvent()
{
	mMouseLeaveEventEnable = true;
}

//
void	CUserPane::RegisterUserPaneWindow()
{
	WNDCLASSEXW	wndclass;
	wndclass.cbSize			= sizeof(WNDCLASSEXW);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc	= CUserPane::STATIC_APICB_WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= CAppImp::GetHInstance();
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= ::LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground	= NULL;//(HBRUSH)(COLOR_WINDOW+1);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= L"userpane";
	wndclass.hIconSm		= NULL;
	
	if( RegisterClassExW(&wndclass) == false )
	{
		_ACError("RegisterClass() failed",NULL);
	}
}

#pragma mark ===========================

#pragma mark ---メニューコマンド処理

//メニューコマンド処理
//virtual, public
ABool	CUserPane::DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	//デフォルト動作：処理無し
	return false;
}

void	CUserPane::UpdateMenu()
{
	//デフォルト動作：処理無し
	return;
}

#pragma mark ===========================

#pragma mark ---コントロールフォーカス処理

//フォーカスに設定
//virtual, public
void	CUserPane::SetFocus() 
{
	//デフォルト動作：処理無し
}

//フォーカス解除
//virtual, public
void	CUserPane::ResetFocus()
{
	//デフォルト動作：処理無し
}

#pragma mark ===========================

#pragma mark --- コントロール属性設定インターフェイス

//
void	CUserPane::SetEnableControl( const ABool inEnable )
{
	if( IsControlEnabled() == inEnable )   return;//#530
	::EnableWindow(mHWnd,inEnable);
}

//
ABool	CUserPane::IsControlEnabled() const
{
	return (::IsWindowEnabled(mHWnd)==TRUE);
}

//
void	CUserPane::SetShowControl( const ABool inShow )
{
	if( inShow == true )
	{
		::ShowWindow(mHWnd,SW_SHOWNORMAL);
		//ここでUpdateWindow()しないと、途中で白色（灰色）描画発生する場合がある。この後の何らかのタイミングでoffscreen用意前にウインドウ描画発生するため？
		::UpdateWindow(mHWnd);
	}
	else
	{
		::ShowWindow(mHWnd,SW_HIDE);
	}
}

//
ABool	CUserPane::IsControlVisible() const
{
	return (::IsWindowVisible(mHWnd)==TRUE);
}

//
ABool	CUserPane::IsControlActive() const
{
	return (GetWinConst().IsActive());
}

void	CUserPane::SetDefaultCursor( const ACursorType inCursorType )
{
	/*この方法ではクラス全てのカーソルが変わってしまうので処理内容変更
	switch(inCursorType)
	{
	  case kCursorType_IBeam:
		{
			::SetClassLong(mHWnd,GCL_HCURSOR,(LONG)::LoadCursor(NULL,MAKEINTRESOURCE(IDC_IBEAM)));
			break;
		}
	  case kCursorType_Arrow:
	  default:
		{
			::SetClassLong(mHWnd,GCL_HCURSOR,(LONG)::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
			break;
		}
	}*/
	mDefaultCursorType = inCursorType;
}

//#282
/**
Controlサイズ設定
*/
void	CUserPane::SetControlSize( const ANumber inWidth, const ANumber inHeight )
{
	GetWin().SetControlSize(GetControlID(),inWidth,inHeight);
}

//#282
/**
Control位置設定
*/
void	CUserPane::SetControlPosition( const AWindowPoint& inWindowPoint )
{
	GetWin().SetControlPosition(GetControlID(),inWindowPoint);
}

//#455
/**
Binding設定
*/
void	CUserPane::SetControlBindings( const ABool inLeft, const ABool inTop, const ABool inRight, const ABool inBottom )
{
	GetWin().SetControlBindings(GetControlID(),inLeft,inTop,inRight,inBottom);
}

#pragma mark ===========================

#pragma mark --- 

void	CUserPane::TellChangedToWindow()
{
	GetWin().CB_UserPaneValueChanged(GetControlID());
}

#pragma mark ===========================

#pragma mark ---Offscreen

/*
描画は常にまずOffscreenに対して行われる。
【Offscreenへの描画】
①最初にOffscreenを作成したとき
mUpdateRectをframe全体にしてDoDraw()がコールされる
②Scroll
mUpdateRectをScrollによって新たに描画すべき範囲にしてDoDraw()がコールされる

なお、DoDraw()からAbs内のEVT_DoDraw()がコールされ、その中からDrawText()等の描画ルーチンがコールされる。

OSからのWM_PAINTイベントを受信したとき、OffscreenからウインドウへのBitBlt転送が行われる。
BitBlt転送はUserPaneフレーム全体に対して行う
ウインドウへの直接描画は、基本的には、上記BitBltだけである。
*/

//Offscreen作成・描画
void	CUserPane::MakeOffscreen( HDC inHDC )
{
	if( mOffscreenMode == false )   return;
	//char	str[256];
	//sprintf(str,"MakeOffscreen()-s hwnd:%X\n",mHWnd);
	//OutputDebugStringA(str);
	
	if( mOffscreenDC != NULL )
	{
		DisposeOffscreen();
	}
	//フレーム取得
	ALocalRect	localFrame;
	GetControlLocalFrameRect(localFrame);
	//Offscreen生成
	mOffscreenDC = ::CreateCompatibleDC(inHDC);
	mOffscreenWidth = localFrame.right-localFrame.left;
	mOffscreenHeight = localFrame.bottom-localFrame.top;
	mOffscreenBitmap = ::CreateCompatibleBitmap(inHDC,mOffscreenWidth,mOffscreenHeight);
	::SelectObject(mOffscreenDC,mOffscreenBitmap);
	//消去
	::SelectObject(mOffscreenDC,::GetStockObject(WHITE_PEN));
	::PatBlt(mOffscreenDC,localFrame.left,localFrame.top,localFrame.right,localFrame.bottom,PATCOPY);
	//Offscreenへの描画
	mUpdateRect = localFrame;
	DoDraw();
	
	//sprintf(str,"MakeOffscreen()-e hwnd:%X\n",mHWnd);
	//OutputDebugStringA(str);
}

/**
Offscreen作成
このメソッドでは描画はせず、WM_PAINTで描画される
*/
void	CUserPane::PrepareOffscreen()
{
	if( mOffscreenMode == false )   return;
	if( mOffscreenDC != NULL )   return;
	
	HDC	hdc = ::GetDC(GetHWnd());
	//フレーム取得
	ALocalRect	localFrame;
	GetControlLocalFrameRect(localFrame);
	//Offscreen生成
	mOffscreenDC = ::CreateCompatibleDC(hdc);
	mOffscreenWidth = localFrame.right-localFrame.left;
	mOffscreenHeight = localFrame.bottom-localFrame.top;
	mOffscreenBitmap = ::CreateCompatibleBitmap(hdc,mOffscreenWidth,mOffscreenHeight);
	::SelectObject(mOffscreenDC,mOffscreenBitmap);
	//消去
	::SelectObject(mOffscreenDC,::GetStockObject(WHITE_PEN));
	::PatBlt(mOffscreenDC,localFrame.left,localFrame.top,localFrame.right,localFrame.bottom,PATCOPY);
	//
	::ReleaseDC(GetHWnd(),hdc);
	//
	mPreparingOffscreen = true;
}

//Offscreen削除
void	CUserPane::DisposeOffscreen()
{
	if( mOffscreenDC != NULL )
	{
		::DeleteObject(mOffscreenBitmap);
		::DeleteDC(mOffscreenDC);
		mOffscreenDC = NULL;
		mOffscreenWidth = 0;
		mOffscreenHeight = 0;
	}
}

void	CUserPane::TransferOffscreen()
{
	if( IsControlVisible() == false )   return;
	if( mInWMPaint == true )   return;//WM_PAINT内で転送されるため
	if( mOffscreenMode == false )   return;
	//Offscreenをウインドウへ転写する（mUpdateRectを対象）
	RECT	rect;
	if( true )
	{
		rect.left		= mUpdateRect.left;
		rect.top		= mUpdateRect.top;
		rect.right		= mUpdateRect.right;
		rect.bottom		= mUpdateRect.bottom;
	}
	else
	{
		//デバッグ用（常に全領域をウインドウへ転写）
		rect.left = 0;
		rect.top = 0;
		rect.right = mOffscreenWidth;
		rect.bottom = mOffscreenHeight;
	}
	::InvalidateRect(mHWnd,&rect,FALSE);
	//::UpdateWindow(mHWnd);これをここで実行するとたとえば補完入力で削除→入力の過程が見える（ちらつく）イベントトランザクション実行後に一気にpaintすべきなのでここでUpdateWindow()を実行しないほうが正解っぽい
}

/**
*/
void	CUserPane::SetOffscreenMode( const ABool inOffscreenMode )
{
	mOffscreenMode = inOffscreenMode;
}

#pragma mark ===========================

#pragma mark ---描画属性設定

//テキストデフォルト属性設定
void	CUserPane::SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, 
		const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing )
{
	//どれかのパラメータが違うときだけ設定してフォントキャッシュ消去 #450
	if( mFontName.Compare(inFontName) == false || mFontSize != inFontSize || 
				mColor != inColor || mStyle != inStyle || mAntiAliasing != inAntiAliasing )
	{
		//
		mFontName.SetText(inFontName);
		mFontSize = inFontSize;
		mColor = inColor;
		mStyle = inStyle;
		mAntiAliasing = inAntiAliasing;
		//フォントキャッシュ消去
		for( AIndex i = 0; i < 8; i++ )
		{
			if( mHDefaultFont[i] != NULL )
			{
				::DeleteObject(mHDefaultFont[i]);
				mHDefaultFont[i] = NULL;
			}
		}
	}
}
void	CUserPane::SetDefaultTextProperty( const AColor& inColor, const ATextStyle inStyle )
{
	mColor = inColor;
	mStyle = inStyle;
}

/**
*/
HFONT	CUserPane::GetHFont( const ATextStyle inStyle ) const
{
	if( inStyle < 0 || inStyle >= 8 )  {_ACError("",NULL);return NULL;}
	
	//すでに取得済みならそれを返す
	if( mHDefaultFont[inStyle] != NULL )   return mHDefaultFont[inStyle];
	
	//未取得なら取得して返す
	HDC	hdc = ::GetDC(mHWnd);
	if( hdc == NULL )   {_ACError("",NULL);return NULL;}
	mHDefaultFont[inStyle] = CWindowImp::CreateFont_(hdc,mFontName,mFontSize,inStyle,mAntiAliasing);
	::ReleaseDC(mHWnd,hdc);
	return mHDefaultFont[inStyle];
}

#pragma mark ===========================

#pragma mark ---描画ルーチン

//テキスト表示
void	CUserPane::DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inText,mColor,mStyle,inJustification);
}

void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, const AJustification inJustification )
{
	DrawText(inDrawRect,inClipRect,inText,mColor,mStyle,inJustification);
}

void	CUserPane::DrawText( const ALocalRect& inDrawRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inText,inColor,inStyle,inJustification);
}

void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification )
{
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(inText);
	textDrawData.UTF16DrawText.ConvertFromUTF8ToAPIUnicode();
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(inColor);
	textDrawData.attrStyle.Add(inStyle);
	DrawText(inDrawRect,inClipRect,textDrawData,inJustification);
}

void	CUserPane::DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inTextDrawData,inJustification);
}

void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
		CTextDrawData& inTextDrawData, const AJustification inJustification )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	::SelectObject(hdc,GetHFont(0));
	::SetBkMode(hdc,TRANSPARENT);
	
	ANumber	textWidth = 1;
	ANumber	x = inDrawRect.left;
	ANumber	y = inDrawRect.top;//#450
	{
		AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
		
		SIZE	textsize;
		::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),utf16textptr.GetByteCount()/sizeof(AUTF16Char),&textsize);
		textWidth = textsize.cx;
		TEXTMETRIC	tm = {0};//#450
		::GetTextMetrics(hdc,&tm);//#450
		{
			switch(inJustification)
			{
			  case kJustification_Right:
				{
					x = inDrawRect.right - textWidth;
					break;
				}
			  case kJustification_RightCenter://#450
				{
					x = inDrawRect.right - textWidth;
					y = inDrawRect.top + (inDrawRect.bottom-inDrawRect.top)/2 - tm.tmHeight/2;
					break;
				}
			  case kJustification_Center:
				{
					x = (inDrawRect.left+inDrawRect.right)/2 - textWidth/2;
					break;
				}
			  case kJustification_LeftTruncated://#315
				{
					if( textWidth > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - textWidth;
					}
					break;
				}
			  case kJustification_CenterTruncated://#315
				{
					if( textWidth > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - textWidth;
					}
					else
					{
						x = (inDrawRect.left+inDrawRect.right)/2 - textWidth/2;
					}
					break;
				}
			}
		}
		
		if( inTextDrawData.drawSelection == true )
		{
			ALocalRect	rect = inDrawRect;
			//SIZE	ts;
			if( inTextDrawData.selectionStart != kIndex_Invalid )
			{
				//::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),inTextDrawData.selectionStart,&ts);
				//rect.left = inDrawRect.left + ts.cx;
				rect.left = inDrawRect.left +
						GetImageXByTextPosition(inTextDrawData,
						inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(inTextDrawData.selectionStart));
			}
			if( inTextDrawData.selectionEnd != kIndex_Invalid )
			{
				//::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),inTextDrawData.selectionEnd,&ts);
				//rect.right = inDrawRect.left + ts.cx;
				rect.right = inDrawRect.left +
						GetImageXByTextPosition(inTextDrawData,
						inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(inTextDrawData.selectionEnd));
			}
			PaintRect(rect,inTextDrawData.selectionColor,0.5);
		}
		
		//clips
		RECT	cliprect;
		cliprect.left		= inClipRect.left;
		cliprect.top		= inClipRect.top;
		cliprect.right		= inClipRect.right;
		cliprect.bottom		= inClipRect.bottom;
		RECT	updaterect;
		updaterect.left		= mUpdateRect.left;
		updaterect.top		= mUpdateRect.top;
		updaterect.right	= mUpdateRect.right;
		updaterect.bottom	= mUpdateRect.bottom;
		RECT	totalclip;
		::IntersectRect(&totalclip,&cliprect,&updaterect);
		//HRGN	hrgn = ::CreateRectRgn(inClipRect.left,inClipRect.top,inClipRect.right,inClipRect.bottom);
		HRGN	hrgn = ::CreateRectRgn(totalclip.left,totalclip.top,totalclip.right,totalclip.bottom);
		::SelectClipRgn(hdc,hrgn);
		
		::SetTextAlign(hdc,TA_UPDATECP);
		::MoveToEx(hdc,x,/*#450 inDrawRect.top*/y,NULL);
		if( inTextDrawData.attrPos.GetItemCount() > 0 )
		{
			//attrPos, curPos, lenはUnicode文字単位
			//UTF16DrawTextはAText(バイト単位)
			AIndex	curPos = 0;//#435 attrPosの順番が入れ替わったときの防止のため、curPosでどこまで表示したかを管理する
			for( AIndex i = 0; i < inTextDrawData.attrPos.GetItemCount(); i++ )
			{
				//lenに、処理するRunLengthの、UniCharでの長さを格納する。
				AItemCount	len;
				if( i+1 < inTextDrawData.attrPos.GetItemCount() )
				{
					len = inTextDrawData.attrPos.Get(i+1) - curPos;//#435 inTextDrawData.attrPos.Get(i);
				}
				else
				{
					//forループ最後でテキストの最後までを必ず表示することを保証するため
					len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - curPos;//#435 inTextDrawData.attrPos.Get(i);
				}
				//lenがテキストの長さよりも長くならないように補正 #435
				if( curPos + len > inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) )
				{
					len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - curPos;
				}
				//
				if( len <= 0 )   continue;
				
				AColor	color = inTextDrawData.attrColor.Get(i);
				::SetTextColor(hdc,color);
				ATextStyle	style = (inTextDrawData.attrStyle.Get(i)&0x07);
				::SelectObject(hdc,GetHFont(style));
/*				switch(style)
				{
				  case kTextStyle_Bold:
					{
						::SelectObject(hdc,mHDefaultFont_Bold);
						break;
					}
				  case kTextStyle_Normal:
				  default:
					{
						::SelectObject(hdc,mHDefaultFont_Normal);
						break;
					}
				}*/
				::TextOutW(hdc,0,0,
						(LPCWSTR)(utf16textptr.GetPtr()+/*#435 inTextDrawData.attrPos.Get(i)*/
						curPos*sizeof(AUTF16Char)),len);
				
				//curPos移動 #435
				curPos += len;
			}
		}
		else
		{
			::SetTextColor(hdc,mColor);
			::TextOutW(hdc,0,0,(LPCWSTR)(utf16textptr.GetPtr()),utf16textptr.GetByteCount()/sizeof(AUTF16Char));
		}
		
		::DeleteObject(hrgn);
		::SelectClipRgn(hdc,NULL);
	}
}

void	CUserPane::DrawIcon( const ALocalRect& inRect, const AIconID inIconID, const ABool inEnabledColor, const ABool inDefaultSize ) const
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	HICON	hicon = CWindowImp::GetHIconByIconID(inIconID);
	if( inEnabledColor == false )
	{
		HICON	hi = CWindowImp::GetHIconByIconID(inIconID,true);
		if( hi != NULL )   hicon = hi;
	}
	UINT	diFlags = DI_NORMAL;
	//win::DrawIconEx(hdc,inRect.left,inRect.top,hicon,0,0,0,NULL,diFlags);
	::DrawIconEx(hdc,inRect.left,inRect.top,hicon,16,16,0,NULL,diFlags);//win 16*16のサイズで表示
	/*上記で元のアイコンのサイズそのままで表示できる・・・はず
	if( inDefaultSize == true )
	{
		diFlags |= DI_DEFAULTSIZE;
	}
	::DrawIconEx(hdc,inRect.left,inRect.top,hicon,inRect.right-inRect.left,inRect.bottom-inRect.top,0,NULL,diFlags);
	*/
}

void	CUserPane::DrawIconFromFile( const ALocalRect& inRect, const AFileAcc& inFile, const ABool inEnabledColor ) const
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	//★暫定 
	HICON	hicon = NULL;
	if( inFile.IsFolder() == false )
	{
		hicon = CWindowImp::GetHIconByIconID(kIconID_Abs_Document);
	}
	else
	{
		hicon = CWindowImp::GetHIconByIconID(kIconID_Abs_Folder);
	}
	UINT	diFlags = DI_NORMAL;
	//win ::DrawIconEx(hdc,inRect.left,inRect.top,hicon,0,0,0,NULL,diFlags);
	::DrawIconEx(hdc,inRect.left,inRect.top,hicon,16,16,0,NULL,diFlags);//win 16*16のサイズで表示
}

//#379
/**
多角形描画
*/
void	CUserPane::PaintPoly( const AArray<ALocalPoint>& inPath, const AColor& inColor, const AFloatNumber inAlpha )
{
	if( inPath.GetItemCount() == 0 )   return;
	
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	//
	HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
	if( hcliprgn == NULL )   return;
	HBRUSH	hbr = ::CreateSolidBrush(inColor);
	if( hbr == NULL )   {_ACError("",NULL);return;}
	::SelectObject(hdc,hbr);
	::SelectClipRgn(hdc,hcliprgn);
	
	//
	AItemCount	pointCount = inPath.GetItemCount();
	POINT*	pointArray = (POINT*)malloc((pointCount+1)*sizeof(POINT));
	if( pointArray == NULL )   {_ACThrow("",this);}
	for( AIndex i = 0; i < pointCount; i++ )
	{
		pointArray[i].x = inPath.Get(i).x;
		pointArray[i].y = inPath.Get(i).y;
	}
	pointArray[pointCount].x = inPath.Get(0).x;
	pointArray[pointCount].y = inPath.Get(0).y;
	//
	//アルファ未対応 if( inAlpha == 1.0 )
	{
		::Polygon(hdc,pointArray,pointCount);
	}
	/*アルファ未対応
	{
		//アルファ（透過）の場合
		//オフスクリーンビットマップに指定色べた塗りして、AlphaBlendでアルファ重ね塗りする
		//
		ANumber	w = 500;
		ANumber	h = 2000;
		//
		HDC	alphahdc = ::CreateCompatibleDC(hdc);
		HBITMAP alphahbitmap = ::CreateCompatibleBitmap(hdc,w,h);
		::SelectObject(alphahdc,alphahbitmap);
		//
		RECT	r;
		r.left		= 0;
		r.top		= 0;
		r.right		= w;
		r.bottom	= h;
		HBRUSH	backhbr = ::CreateSolidBrush(kColor_Black);
		if( backhbr == NULL )   {_ACError("",NULL);return;}
		::FillRect(alphahdc,&r,backhbr);
		//
		::SelectObject(alphahdc,hbr);
		::Polygon(alphahdc,pointArray,pointCount);
		//
		BLENDFUNCTION	ftn;
		ftn.BlendOp = AC_SRC_OVER;
		ftn.BlendFlags = 0;
		ftn.SourceConstantAlpha = 256*inAlpha;
		ftn.AlphaFormat = AC_SRC_ALPHA;
		::AlphaBlend(hdc,0,0,w,h,alphahdc,0,0,w,h,ftn);
		//
		::DeleteDC(alphahdc);
		::DeleteObject(alphahbitmap);
		::DeleteObject(backhbr);
	}
	*/
	//
	::free(pointArray);
	//
	::SelectClipRgn(hdc,NULL);
	::DeleteObject(hcliprgn);
	::DeleteObject(hbr);
}

/**
Rect塗りつぶし
*/
void	CUserPane::PaintRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha ) const
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
	if( hcliprgn == NULL )   return;
	HBRUSH	hbr = ::CreateSolidBrush(inColor);
	if( hbr == NULL )   {_ACError("",NULL);return;}
	::SelectClipRgn(hdc,hcliprgn);
	RECT	rect;
	rect.left		= inRect.left;
	rect.top		= inRect.top;
	rect.right		= inRect.right;
	rect.bottom		= inRect.bottom;
	
	if( inAlpha == 1.0 )
	{
		::FillRect(hdc,&rect,hbr);
	}
	else
	{
		//アルファ（透過）の場合
		//オフスクリーンビットマップに指定色べた塗りして、AlphaBlendでアルファ重ね塗りする
		//
		ANumber	w = rect.right-rect.left;
		ANumber	h = rect.bottom-rect.top;
		//
		HDC	alphahdc = ::CreateCompatibleDC(hdc);
		HBITMAP alphahbitmap = ::CreateCompatibleBitmap(hdc,w,h);
		::SelectObject(alphahdc,alphahbitmap);
		//
		RECT	r;
		r.left		= 0;
		r.top		= 0;
		r.right		= w;
		r.bottom	= h;
		::FillRect(alphahdc,&r,hbr);
		//
		BLENDFUNCTION	ftn;
		ftn.BlendOp = AC_SRC_OVER;
		ftn.BlendFlags = 0;
		ftn.SourceConstantAlpha = 256*inAlpha;
		ftn.AlphaFormat = 0;
		::AlphaBlend(hdc,rect.left,rect.top,w,h,alphahdc,0,0,w,h,ftn);
		//
		::DeleteDC(alphahdc);
		::DeleteObject(alphahbitmap);
	}
	::SelectClipRgn(hdc,NULL);
	::DeleteObject(hcliprgn);
	::DeleteObject(hbr);
}

//#634
/**
垂直方向グラデーションでPaintRect
*/
void	CUserPane::PaintRectWithVerticalGradient( const ALocalRect& inRect, 
		const AColor& inTopColor, const AColor& inBottomColor, const AFloatNumber inAlpha, const ABool inDrawUsingLines )
{
	if( inDrawUsingLines == true )
	{
		//線を複数書くことで実現する
		//ただし行数が多いと処理時間がかかる
		AFloatNumber	percent = 0;
		AFloatNumber	h = (inRect.bottom-inRect.top-1);
		AFloatNumber	percentInc = 100.0/h;
		for( ANumber y = inRect.top; y < inRect.bottom; y++ )
		{
			AColor	color = AColorWrapper::GetGradientColor(inTopColor,inBottomColor,percent);
			ALocalPoint	spt = {inRect.left,y};
			ALocalPoint	ept = {inRect.right-1,y};
			DrawLine(spt,ept,color,inAlpha);
			percent += percentInc;
		}
	}
	else
	{
		AColor	color = AColorWrapper::GetGradientColor(inTopColor,inBottomColor,50.0);
		PaintRect(inRect,color,inAlpha);
		//対応必要
	}
}

//#634
/**
水平方向グラデーションでPaintRect
*/
void	CUserPane::PaintRectWithHorizontalGradient( const ALocalRect& inRect, 
		const AColor& inTopColor, const AColor& inBottomColor, const AFloatNumber inAlpha )
{
	AColor	color = AColorWrapper::GetGradientColor(inTopColor,inBottomColor,50.0);
	PaintRect(inRect,color,inAlpha);
	//対応必要
}

/**
ウインドウを背景色／透明色で消去
（Overlayウインドウの場合はRectを透明色で塗りつぶし）
*/
void	CUserPane::EraseRect( const ALocalRect& inRect )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	if( GetWin().IsOverlayWindow() == true )
	{
		PaintRect(inRect,GetWin().GetOverlayBackgroundColor(),1.0);
	}
	else
	{
		HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
		if( hcliprgn == NULL )   return;
		::SelectClipRgn(hdc,hcliprgn);
		RECT	rect;
		rect.left		= inRect.left;
		rect.top		= inRect.top;
		rect.right		= inRect.right;
		rect.bottom		= inRect.bottom;
		
		//ウインドウ背景ブラシで消去
		::FillRect(hdc,&rect,GetWin().GetBackgroundBlash());
		
		::SelectClipRgn(hdc,NULL);
		::DeleteObject(hcliprgn);
	}
}

void	CUserPane::FrameRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	HBRUSH	hbr = ::CreateSolidBrush(inColor);
	if( hbr == NULL )   {_ACError("",NULL);return;}
	HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
	if( hcliprgn == NULL )   {_ACError("",NULL);return;}
	::SelectClipRgn(hdc,hcliprgn);
	RECT	rect;
	rect.left		= inRect.left;
	rect.top		= inRect.top;
	rect.right		= inRect.right;
	rect.bottom		= inRect.bottom;
	::FrameRect(hdc,&rect,hbr);
	::SelectClipRgn(hdc,NULL);
	::DeleteObject(hcliprgn);
	::DeleteObject(hbr);
	//★暫定 アルファ未対応
}

/**
*/
void	CUserPane::DrawFocusFrame( const ALocalRect& inRect )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
	if( hcliprgn == NULL )   {_ACError("",NULL);return;}
	::SelectClipRgn(hdc,hcliprgn);
	RECT	rect;
	rect.left		= inRect.left;
	rect.top		= inRect.top;
	rect.right		= inRect.right;
	rect.bottom		= inRect.bottom;
	
	//::DrawFocusRect(hdc,&rect);これは点線枠になるので違う
	//
	AColor	focuscolor;
	AColorWrapper::GetHighlightColor(focuscolor);
	ALocalRect	localrect = inRect;
	
	localrect.left++;
	localrect.top++;
	localrect.right--;
	localrect.bottom--;
	
	FrameRect(localrect,focuscolor);
	
	::SelectClipRgn(hdc,NULL);
	::DeleteObject(hcliprgn);
}

void	CUserPane::DrawBackgroundImage( const ALocalRect& inEraseRect, const AImageRect& inImageFrameRect, 
		const ANumber inLeftOffset, const AIndex inBackgroundImageIndex, const AFloatNumber inAlpha )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	//
	RECT	eraserect;
	eraserect.left		= inEraseRect.left;
	eraserect.top		= inEraseRect.top;
	eraserect.right		= inEraseRect.right;
	eraserect.bottom	= inEraseRect.bottom;
	//mUpdateRectとeraserectとの重複部分でclipする
	RECT	updaterect;
	updaterect.left		= mUpdateRect.left;
	updaterect.top		= mUpdateRect.top;
	updaterect.right	= mUpdateRect.right;
	updaterect.bottom	= mUpdateRect.bottom;
	RECT	cliprect;
	::IntersectRect(&cliprect,&eraserect,&updaterect);
	HRGN	hcliprgn = ::CreateRectRgn(mUpdateRect.left,mUpdateRect.top,mUpdateRect.right,mUpdateRect.bottom);
	if( hcliprgn == NULL )   return;
	::SelectClipRgn(hdc,hcliprgn);
	//
	HBITMAP	hbitmap = CWindowImp::GetBackgoundBitmap(inBackgroundImageIndex);
	BITMAP	bmp;
	::GetObject(hbitmap,sizeof(BITMAP),&bmp);
	HDC	bitmaphdc = ::CreateCompatibleDC(hdc);
	::SelectObject(bitmaphdc,hbitmap);
	//
	ANumber	picwidth = bmp.bmWidth;
	ANumber	picheight = bmp.bmHeight;
	if( picwidth == 0 )   picwidth = 1;
	//
	ANumber	startx = inImageFrameRect.left/picwidth;
	ANumber	starty = inImageFrameRect.top/picheight;
	for( ANumber x = startx; x < inImageFrameRect.right + inLeftOffset; x += picwidth )
	{
		for( ANumber y = starty; y < inImageFrameRect.bottom; y += picheight )
		{
			//Imageを貼る位置をImage座標系で取得
			AImageRect	pictimage;
			pictimage.left		= x - inLeftOffset;
			pictimage.top		= y;
			pictimage.right		= x - inLeftOffset + picwidth;
			pictimage.bottom	= y + picheight;
			//Local座標系に変換
			RECT	rect;
			rect.left		= pictimage.left		- inImageFrameRect.left;
			rect.top		= pictimage.top			- inImageFrameRect.top;
			rect.right		= pictimage.right		- inImageFrameRect.left;
			rect.bottom		= pictimage.bottom		- inImageFrameRect.top;
			//
			RECT	tmp;
			if( ::IntersectRect(&tmp,&eraserect,&rect) )
			{
				::BitBlt(hdc,rect.left,rect.top,picwidth,picheight,bitmaphdc,0,0,SRCCOPY);
			}
		}
	}
	::DeleteDC(bitmaphdc);
	::SelectClipRgn(hdc,NULL);
	::DeleteObject(hcliprgn);
	if( true )
	{
		PaintRect(inEraseRect,kColor_White,inAlpha);
	}
}

void	CUserPane::DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
		const AFloatNumber inAlpha, const AFloatNumber inLineDash, const AFloatNumber inLineWidth )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	HPEN	hpen = NULL;
	if( inLineDash == 0.0 )
	{
		hpen = ::CreatePen(PS_SOLID,static_cast<int>(inLineWidth),inColor);
	}
	else
	{
		//★暫定
		hpen = ::CreatePen(PS_DOT,1,inColor);
	} 
	if( hpen == NULL )   {_ACError("",NULL);return;}
	::SetBkMode(hdc,TRANSPARENT);
	::SelectObject(hdc,hpen);
	::MoveToEx(hdc,inStartPoint.x,inStartPoint.y,NULL);
	::LineTo(hdc,inEndPoint.x,inEndPoint.y);
	::DeleteObject(hpen);
	//★暫定　アルファ未対応
}

void	CUserPane::SetDrawLineMode( const AColor& inColor, const AFloatNumber inAlpha, 
		const AFloatNumber inLineDash, const AFloatNumber inLineWidth )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	if( mDrawLinePen != NULL )   ::DeleteObject(mDrawLinePen);
	mDrawLinePen = NULL;
	
	if( inLineDash == 0.0 )
	{
		mDrawLinePen = ::CreatePen(PS_SOLID,static_cast<int>(inLineWidth),inColor);
	}
	else
	{
		//★暫定
		mDrawLinePen = ::CreatePen(PS_DOT,1,inColor);
	} 
	if( mDrawLinePen == NULL )   {_ACError("",NULL);return;}
	::SetBkMode(hdc,TRANSPARENT);
	::SelectObject(hdc,mDrawLinePen);
	//★暫定　アルファ未対応
}

void	CUserPane::RestoreDrawLineMode()
{
	if( mDrawLinePen != NULL )   ::DeleteObject(mDrawLinePen);
	mDrawLinePen = NULL;
}

void	CUserPane::DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = mPaintDC;
	}
	else
	{
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return;}
	
	::MoveToEx(hdc,inStartPoint.x,inStartPoint.y,NULL);
	::LineTo(hdc,inEndPoint.x,inEndPoint.y);
}

void	CUserPane::DrawXorCaretLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const ABool inClipToFrame, 
		const ABool inDash, const ANumber inPenSize )
{
	APoint	spt,ept;
	HWND	hwnd = mHWnd;
	if( inClipToFrame == false )
	{
		//UserPane内でclipしない＝UserPane外へも描画する場合
		//Windowsでは、Wndがコントロールごとなので、外へ描画するときは、トップウインドウに描画する必要がある。
		
		//HWnd
		hwnd = GetWin().GetHWnd();
		
		//ウインドウ座標変換
		AWindowPoint	wspt, wept;
		GetWin().GetWindowPointFromControlLocalPoint(GetControlID(),inStartPoint,wspt);
		GetWin().GetWindowPointFromControlLocalPoint(GetControlID(),inEndPoint,wept);
		spt.x = wspt.x;
		spt.y = wspt.y;
		ept.x = wept.x;
		ept.y = wept.y;
	}
	else
	{
		//UserPane内でclipする場合
		//Wnd外へは描画されないので、SelectClipRgnは不要
		
		//
		spt.x = inStartPoint.x;
		spt.y = inStartPoint.y;
		ept.x = inEndPoint.x;
		ept.y = inEndPoint.y;
	}
	/*HRGN	hrgn = NULL;
	if( inClipToFrame == true )
	{
	ALocalRect	localFrame;
	GetControlLocalFrameRect(localFrame);
	hrgn = ::CreateRectRgn(localFrame.left,localFrame.top,localFrame.right,localFrame.bottom);
	::SelectClipRgn(hdc,hrgn);
	}*/
	
	//HDC取得
	HDC	hdc = ::GetDC(hwnd);
	if( hdc == NULL )   {_ACError("",NULL);return;}
	
	HPEN	hpen = NULL;
	if( inDash == false )
	{
		hpen = ::CreatePen(PS_SOLID,inPenSize,kColor_White);
	}
	else
	{
		//★暫定
		hpen = ::CreatePen(PS_SOLID,inPenSize,kColor_Gray30Percent);
		//hpen = ::CreatePen(PS_DOT,1,kColor_Black);
	} 
	if( hpen == NULL )   {_ACError("",NULL);return;}//解説：「hpenが取得できないような状況ではまともに動かない（故障かメモリ不足）→まともに動かない状態で細かいリーク対策するのは無意味」がポリシー。できることは、データを緊急保存して、ユーザーに危険状態を通知することだけ。
	::SetBkMode(hdc,TRANSPARENT);
	::SetROP2(hdc,R2_XORPEN);
	::SelectObject(hdc,hpen);
	::MoveToEx(hdc,spt.x,spt.y,NULL);
	::LineTo(hdc,ept.x,ept.y);
	::DeleteObject(hpen);
	::SetROP2(hdc,R2_COPYPEN);
	
	/*if( hrgn != NULL )
	{
	::DeleteObject(hrgn);
	::SelectClipRgn(hdc,NULL);
	}*/
	
	::ReleaseDC(hwnd,hdc);
}

void	CUserPane::DrawXorCaretRect( const ALocalRect& inLocalRect, const ABool inClipToFrame )
{
	ARect	rect;
	HWND	hwnd = mHWnd;
	if( inClipToFrame == false )
	{
		//UserPane内でclipしない＝UserPane外へも描画する場合
		//Windowsでは、Wndがコントロールごとなので、外へ描画するときは、トップウインドウに描画する必要がある。
		
		//HWnd
		hwnd = GetWin().GetHWnd();
		
		//ウインドウ座標変換
		AWindowRect	wrect;
		GetWin().GetWindowRectFromControlLocalRect(GetControlID(),inLocalRect,wrect);
		rect.left		= wrect.left;
		rect.top		= wrect.top;
		rect.right		= wrect.right;
		rect.bottom		= wrect.bottom;
	}
	else
	{
		//UserPane内でclipする場合
		//Wnd外へは描画されないので、SelectClipRgnは不要
		
		//
		rect.left		= inLocalRect.left;
		rect.top		= inLocalRect.top;
		rect.right		= inLocalRect.right;
		rect.bottom		= inLocalRect.bottom;
	}
	
	HDC	hdc = ::GetDC(hwnd);
	if( hdc == NULL )   {_ACError("",NULL);return;}
	
	HRGN	hrgn = NULL;
	if( inClipToFrame == true )
	{
		ALocalRect	localFrame;
		GetControlLocalFrameRect(localFrame);
		hrgn = ::CreateRectRgn(localFrame.left,localFrame.top,localFrame.right,localFrame.bottom);
		::SelectClipRgn(hdc,hrgn);
	}
	
	HPEN	hpen = ::CreatePen(PS_SOLID,1,kColor_White);
	if( hpen == NULL )   {_ACError("",NULL);return;}
	::SetBkMode(hdc,TRANSPARENT);
	::SetROP2(hdc,R2_XORPEN);
	::SelectObject(hdc,hpen);
	::MoveToEx(hdc,rect.left,rect.top,NULL);
	::LineTo(hdc,rect.right,rect.top);
	::LineTo(hdc,rect.right,rect.bottom);
	::LineTo(hdc,rect.left,rect.bottom);
	::LineTo(hdc,rect.left,rect.top);
	::DeleteObject(hpen);
	::SetROP2(hdc,R2_COPYPEN);
	
	if( hrgn != NULL )
	{
		::DeleteObject(hrgn);
		::SelectClipRgn(hdc,NULL);
	}
	
	::ReleaseDC(hwnd,hdc);
}

#pragma mark ===========================

#pragma mark ---Refresh

void	CUserPane::RefreshRect( const ALocalRect& inLocalRect )
{
	ALocalRect	localFrameRect;
	GetControlLocalFrameRect(localFrameRect);
	//R0108高速化 フレームサイズトリミングの結果、localRect.top > localRect.bottom となると、DoDraw時のUpdateRegion判定で、全範囲対象になってしまう？ので
	//RefreshRectがフレーム外のときは何もしないようにする
	if( inLocalRect.bottom < localFrameRect.top )   return;
	if( inLocalRect.top > localFrameRect.bottom )   return;
	if( inLocalRect.right < localFrameRect.left )   return;
	if( inLocalRect.left > localFrameRect.right )   return;
	//指定領域を描画（MacOSXではOSへUpdateを指示(HIViewRender())してOS経由で描画していたが、Windows版はOffscreenがCUserPane管理なので直接描画）
	if( mOffscreenMode == true )
	{
		//
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		//
		mUpdateRect = inLocalRect;
		DoDraw();
		//WM_PAINTイベントを発生させて、Offscreenをウインドウへ転写する
		/* EVT_DoDraw()内からTransferOffscreen()を呼ぶ
		RECT	rect;
		rect.left		= inLocalRect.left;
		rect.top		= inLocalRect.top;
		rect.right		= inLocalRect.right;
		rect.bottom		= inLocalRect.bottom;
		::InvalidateRect(mHWnd,&rect,FALSE);
		::UpdateWindow(mHWnd);
		*/
	}
	else
	{
		RECT	rect;
		rect.left		= inLocalRect.left;
		rect.top		= inLocalRect.top;
		rect.right		= inLocalRect.right;
		rect.bottom		= inLocalRect.bottom;
		::InvalidateRect(mHWnd,&rect,FALSE);
		//::UpdateWindow(mHWnd);
	}
}

void	CUserPane::RefreshControl()
{
	ALocalRect	rect;
	GetControlLocalFrameRect(rect);
	RefreshRect(rect);
}

/**
指定rectについてOffscreenから実画面へFlushする
*/
void	CUserPane::FlushRect( const ALocalRect& inLocalRect )
{
	RECT	rect;
	rect.left		= inLocalRect.left;
	rect.top		= inLocalRect.top;
	rect.right		= inLocalRect.right;
	rect.bottom		= inLocalRect.bottom;
	::InvalidateRect(mHWnd,&rect,FALSE);
	//UpdateWindow()からWM_PAINTが処理される（WM_PAINT処理後、UpdateWindow()が帰ってくる）
	::UpdateWindow(mHWnd);
}

/**
コントロール全体をOffscreenから実画面へFlushする
*/
void	CUserPane::FlushControl()
{
	ALocalRect	rect;
	GetControlLocalFrameRect(rect);
	FlushRect(rect);
}

#pragma mark ===========================

#pragma mark ---描画用情報取得

//テキストの位置から描画時のX座標を得る
ANumber	CUserPane::GetImageXByTextPosition( CTextDrawData& inTextDrawData, const AIndex inTextPosition )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = ::GetDC(GetHWnd());
	}
	else
	{
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return 0;}
	
	//フォント設定
	::SelectObject(hdc,GetHFont(0));
	::SetBkMode(hdc,TRANSPARENT);
	::SetTextAlign(hdc,TA_UPDATECP);
	//Unicode文字offset取得
	AItemCount	offset = 0;
	if( inTextPosition < inTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount() )
	{
		offset = inTextDrawData.OriginalTextArray_UnicodeOffset.Get(inTextPosition);
	}
	else
	{
		offset = inTextDrawData.OriginalTextArray_UnicodeOffset.Get(inTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	}
	ANumber	textWidth = 0;
	{
		AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
		if( inTextDrawData.attrPos.GetItemCount() > 0 )
		{
			//attrPos, curPos, lenはUnicode文字単位
			//UTF16DrawTextはAText(バイト単位)
			AIndex	curPos = 0;//#435 attrPosの順番が入れ替わったときの防止のため、curPosでどこまで表示したかを管理する
			for( AIndex i = 0; i < inTextDrawData.attrPos.GetItemCount(); i++ )
			{
				//終了条件：offsetまでの文字幅（の和）を計算する
				if( curPos >= offset )   break;
				
				//lenに、処理するRunLengthの、UniCharでの長さを格納する。
				AItemCount	len;
				if( i+1 < inTextDrawData.attrPos.GetItemCount() )
				{
					len = inTextDrawData.attrPos.Get(i+1) - curPos;//#435 inTextDrawData.attrPos.Get(i);
				}
				else
				{
					//forループ最後でテキストの最後までを必ず表示することを保証するため
					len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - curPos;//#435 inTextDrawData.attrPos.Get(i);
				}
				//lenがテキストの長さよりも長くならないように補正 #435
				if( curPos + len > inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) )
				{
					len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - curPos;
				}
				//
				if( len <= 0 )   continue;
				
				//次の位置がoffset以降ならoffset位置までの文字幅を計算するようにする
				if( curPos+len >= offset )   len = offset-curPos;
				
				//style指定
				ATextStyle	style = (inTextDrawData.attrStyle.Get(i)&0x07);
				::SelectObject(hdc,GetHFont(style));
				//文字幅取得(curPosからlenの長さ分）
				SIZE	textsize = {1,1};
				::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()+curPos*sizeof(AUTF16Char)),len,&textsize);
				textWidth += textsize.cx;
				//curPos移動 #435
				curPos += len;
			}
		}
		else
		{
			SIZE	textsize = {1,1};
			::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),offset,&textsize);
			textWidth = textsize.cx;
		}
	}
	//
	if( mOffscreenMode == false )
	{
		::ReleaseDC(GetHWnd(),hdc);
	}
	return textWidth;
}

//描画時のX座標からテキストの位置を得る
AIndex	CUserPane::GetTextPositionByImageX( CTextDrawData& inTextDrawData, const ANumber inImageX )
{
	//AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
	AItemCount	count = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char);
	if( count == 0 )   return 0;
	
	ANumber	prevw = 0;
	for( AIndex i = 1; i <= count; i++ )
	{
		ANumber w = GetImageXByTextPosition(inTextDrawData,inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(i));
		if( w > inImageX )
		{
			if( (prevw+w)/2 < inImageX )
			{
				return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(i);
			}
			else
			{
				return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(i-1);
			}
		}
		prevw = w;
	}
	return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(count);
	/*
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = ::GetDC(GetHWnd());
	}
	else
	{
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return 0;}
	
	//フォント設定
	::SelectObject(hdc,GetHFont(0));
	//
	AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
	AItemCount	count = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char);
	if( count == 0 )   return 0;
	
	ANumber	prevw = 0;
	for( AIndex i = 1; i <= count; i++ )
	{
		SIZE	textsize = {1,1};
		::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),i,&textsize);
		ANumber	w = textsize.cx;
		if( w > inImageX )
		{
			if( (prevw+w)/2 < inImageX )
			{
				return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(i);
			}
			else
			{
				return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(i-1);
			}
		}
		prevw = w;
	}
	//
	if( mOffscreenMode == false )
	{
		::ReleaseDC(GetHWnd(),hdc);
	}
	return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(count);
	*/
}

AFloatNumber	CUserPane::GetDrawTextWidth( const AText& inText )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = ::GetDC(GetHWnd());
	}
	else
	{
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return 0;}
	
	//フォント設定
	::SelectObject(hdc,GetHFont(mStyle));
	//
	ANumber	textWidth = 1;
	AText	utf16text;
	utf16text.SetText(inText);
	utf16text.ConvertFromUTF8ToAPIUnicode();
	{
		AStTextPtr	utf16textptr(utf16text,0,utf16text.GetItemCount());
		SIZE	textsize = {1,1};
		::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),static_cast<int>(utf16textptr.GetByteCount()/sizeof(AUTF16Char)),&textsize);
		textWidth = textsize.cx;
	}
	//
	if( mOffscreenMode == false )
	{
		::ReleaseDC(GetHWnd(),hdc);
	}
	return textWidth;
}

AFloatNumber	CUserPane::GetDrawTextWidth( CTextDrawData& inTextDrawData )
{
	HDC	hdc = NULL;
	if( mOffscreenMode == false )
	{
		hdc = ::GetDC(GetHWnd());
	}
	else
	{
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		hdc = mOffscreenDC;
	}
	if( hdc == NULL ) {_ACError("",this);return 0;}
	
	//フォント設定
	::SelectObject(hdc,GetHFont(mStyle));
	//
	ANumber	textWidth = 1;
	{
		AStTextPtr	utf16textptr(inTextDrawData.UTF16DrawText,0,inTextDrawData.UTF16DrawText.GetItemCount());
		SIZE	textsize = {1,1};
		::GetTextExtentPoint32W(hdc,(LPCWSTR)(utf16textptr.GetPtr()),static_cast<int>(utf16textptr.GetByteCount()/sizeof(AUTF16Char)),&textsize);
		textWidth = textsize.cx;
	}
	//
	if( mOffscreenMode == false )
	{
		::ReleaseDC(GetHWnd(),hdc);
	}
	return textWidth;
}

//Drawすべき領域内かどうかの判定
//mUpdateRectはCUserPaneが自分で設定する、Offscreen内の描画更新すべき領域。OSのWM_PAINTイベントの描画領域とは関係ない。
ABool	CUserPane::IsRectInDrawUpdateRegion( const ALocalRect& inLocalRect ) const
{
	if(	mUpdateRect.right < inLocalRect.left  ||
		mUpdateRect.left  > inLocalRect.right ||
		mUpdateRect.bottom< inLocalRect.top   ||
		mUpdateRect.top   > inLocalRect.bottom )
	{
		return false;
	}
	else
	{
		return true;
	}
}

//
void	CUserPane::GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent ) const
{
	HDC	hdc = ::GetDC(mHWnd);
	if( hdc == NULL )   {_ACError("",NULL);return;}
	
	::SelectObject(hdc,GetHFont(0));
	TEXTMETRIC	tm;
	::GetTextMetrics(hdc,&tm);
	outLineHeight = tm.tmHeight;
	outLineAscent = tm.tmAscent;
	
	::ReleaseDC(mHWnd,hdc);
}

#pragma mark ===========================

#pragma mark ---スクロール

//Pane全体をスクロールする
void	CUserPane::Scroll( const ANumber inDeltaX, const ANumber inDeltaY )
{
	if( inDeltaX == 0 && inDeltaY == 0 )   return;

	ALocalRect	frame;
	GetControlLocalFrameRect(frame);
	
	//
	RECT	framerect, invalRect;
	framerect.left		= frame.left;
	framerect.top		= frame.top;
	framerect.right		= frame.right;
	framerect.bottom	= frame.bottom;
	if( mOffscreenMode == true )
	{
		if( mOffscreenDC == NULL )
		{
			PrepareOffscreen();
		}
		::ScrollDC(mOffscreenDC,inDeltaX,inDeltaY,&framerect,NULL,NULL,&invalRect);
		//実際のウインドウ画面もスクロール
		HDC	hdc = ::GetDC(mHWnd);
		if( hdc != NULL )
		{
			RECT	updaterect;
			::ScrollDC(hdc,inDeltaX,inDeltaY,&framerect,NULL,NULL,&updaterect);
			::BitBlt(hdc,updaterect.left,updaterect.top,
					updaterect.right-updaterect.left,updaterect.bottom-updaterect.top,
					mOffscreenDC,updaterect.left,updaterect.top,SRCCOPY);
			::ReleaseDC(mHWnd,hdc);
		}
		else _ACError("",NULL);
		
		::SelectObject(mOffscreenDC,::GetStockObject(WHITE_PEN));
		::PatBlt(mOffscreenDC,invalRect.left,invalRect.top,invalRect.right-invalRect.left,invalRect.bottom-invalRect.top,PATCOPY);//★
		
		//スクロールで新たに描画すべき場所を描画
		mUpdateRect.left	= invalRect.left;
		mUpdateRect.top		= invalRect.top;
		mUpdateRect.right	= invalRect.right;
		mUpdateRect.bottom	= invalRect.bottom;
		DoDraw();
		
		//WM_PAINTイベントを発生させて、Offscreen全てをウインドウへ転写する
		/* EVT_DoDraw()内からTransferOffscreen()を呼ぶ
		::InvalidateRect(mHWnd,&framerect,FALSE);
		::UpdateWindow(mHWnd);*/
	}
	else
	{
		mPaintDC = ::GetDC(mHWnd);
		if( mPaintDC == NULL )   {_ACError("",this);return;}
		//
		RECT	updaterect;
		::ScrollDC(mPaintDC,inDeltaX,inDeltaY,&framerect,NULL,NULL,&invalRect);
		//
		mUpdateRect.left	= invalRect.left;
		mUpdateRect.top		= invalRect.top;
		mUpdateRect.right	= invalRect.right;
		mUpdateRect.bottom	= invalRect.bottom;
		DoDraw();
		//
		::ReleaseDC(mHWnd,mPaintDC);
	}
}

#pragma mark ===========================

#pragma mark ---Drag

void	CUserPane::DragText( const ALocalPoint& inMousePoint, const AArray<ALocalRect>& inDragRect, const AArray<AScrapType>& inScrapType, const ATextArray& inData )
{
	IDataObject*	pdataobject = new CDataObject();
	for( AIndex index = 0; index < inScrapType.GetItemCount(); index++ )
	{
		AScrapType	scrapType = inScrapType.Get(index);
		//★Unicode以外のテキスト対応検討（ここでレガシー変換して格納？）
		/*#688
		switch(scrapType)
		{
		  case kScrapType_UnicodeText:
		{
		*/
		FORMATETC	fmt;
		fmt.cfFormat = scrapType;
		fmt.ptd = NULL;
		fmt.dwAspect = DVASPECT_CONTENT;
		fmt.lindex = -1;
		fmt.tymed = TYMED_HGLOBAL;
		STGMEDIUM	stg;
		stg.tymed = TYMED_HGLOBAL;
		{
			//テキストエンコーディング／改行コード変換（この関数にきたときにはUTF-8）#688
			AText	utf16Text;
			utf16Text.SetText(inData.GetTextConst(index));
			utf16Text.ConvertFromUTF8ToUTF16();//#688 
			for( AIndex pos = 0; pos < utf16Text.GetItemCount(); pos += sizeof(AUTF16Char) )
			{
				AUTF16Char	ch = utf16Text.GetUTF16CharFromUTF16Text(pos);
				if( ch == 0x000D )
				{
					utf16Text.InsertUTF16Char(pos+sizeof(AUTF16Char),0x000A);
					pos += sizeof(AUTF16Char);
				}
			}
			//設定
			AStTextPtr	textptr(utf16Text,0,utf16Text.GetItemCount());
			stg.hGlobal = ::GlobalAlloc(GHND|GMEM_SHARE,textptr.GetByteCount()+sizeof(AUTF16Char));//文字列はNULL終端される
			AUCharPtr	ptr = (AUCharPtr)::GlobalLock(stg.hGlobal);
			for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
			{
				ptr[i] = (textptr.GetPtr())[i];
			}
			::GlobalUnlock(stg.hGlobal);
		}
		stg.pUnkForRelease = NULL;
		pdataobject->SetData(&fmt,&stg,TRUE);//所有権はCDataObjectへ移動
			/*#688
				break;
			}
		  case kScrapType_Text:
			  {
				FORMATETC	fmt;
				fmt.cfFormat = scrapType;
				fmt.ptd = NULL;
				fmt.dwAspect = DVASPECT_CONTENT;
				fmt.lindex = -1;
				fmt.tymed = TYMED_HGLOBAL;
				STGMEDIUM	stg;
				stg.tymed = TYMED_HGLOBAL;
				{
					//改行コード変換
					AText	text;
					text.SetText(inData.GetTextConst(index));
					for( AIndex pos = 0; pos < text.GetItemCount(); pos++ )
					{
						AUChar	ch = text.Get(pos);
						if( ch == kUChar_CR )
						{
							text.Insert1(pos+1,kUChar_LF);
							pos++;
						}
					}
					//設定
					AStTextPtr	textptr(text,0,text.GetItemCount());
					stg.hGlobal = ::GlobalAlloc(GHND|GMEM_SHARE,textptr.GetByteCount()+sizeof(AUTF16Char));//文字列はNULL終端される
					AUCharPtr	ptr = (AUCharPtr)::GlobalLock(stg.hGlobal);
					for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
					{
						ptr[i] = (textptr.GetPtr())[i];
					}
					::GlobalUnlock(stg.hGlobal);
				}
				stg.pUnkForRelease = NULL;
				pdataobject->SetData(&fmt,&stg,TRUE);//所有権はCDataObjectへ移動
				break;
			}
		  default:
			  {
				FORMATETC	fmt;
				fmt.cfFormat = scrapType;
				fmt.ptd = NULL;
				fmt.dwAspect = DVASPECT_CONTENT;
				fmt.lindex = -1;
				fmt.tymed = TYMED_HGLOBAL;
				STGMEDIUM	stg;
				stg.tymed = TYMED_HGLOBAL;
				{
					//設定
					AStTextPtr	textptr(inData.GetTextConst(index),0,inData.GetTextConst(index).GetItemCount());
					stg.hGlobal = ::GlobalAlloc(GHND|GMEM_SHARE,textptr.GetByteCount()+sizeof(AUTF16Char));//文字列はNULL終端される
					AUCharPtr	ptr = (AUCharPtr)::GlobalLock(stg.hGlobal);
					for( AIndex i = 0; i < textptr.GetByteCount(); i++ )
					{
						ptr[i] = (textptr.GetPtr())[i];
					}
					::GlobalUnlock(stg.hGlobal);
				}
				stg.pUnkForRelease = NULL;
				pdataobject->SetData(&fmt,&stg,TRUE);//所有権はCDataObjectへ移動
				break;
			  }
		}
	*/
	}
	IDropSource*	pdropsource = new CDropSource();
	//
	DWORD	pdweffect = 0;
	::DoDragDrop(pdataobject,pdropsource,DROPEFFECT_COPY|DROPEFFECT_MOVE,&pdweffect);
	//オブジェクト解放（参照回数減算→delte）
	pdataobject->Release();
	pdropsource->Release();
}

ABool	CUserPane::DragEnter( IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	//継承クラスで実装
	return true;
}

ABool	CUserPane::DragOver( DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	//継承クラスで実装
	return true;
}

ABool	CUserPane::DragLeave()
{
	//継承クラスで実装
	return true;
}

ABool	CUserPane::Drop( IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	//継承クラスで実装
	return true;
}

#pragma mark ===========================

#pragma mark --- カーソル

/**
カーソルを（次回マウスを動かすまで）消す
現在のカーソル位置がウインドウ内の場合のみ適用される。
ウインドウのタイトルバー上などにあった場合に、再度表示するタイミングがないため。
*/
void	CUserPane::ObscureCursor()
{
	POINT	pt;
	::GetCursorPos(&pt);
	ARect	rect;
	GetWin().GetWindowBounds(rect);
	if( pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom )
	{
		ACursorWrapper::ObscureCursor();
	}
}

#pragma mark ===========================

#pragma mark --- APIコールバック

ABool	CUserPane::APICB_WndProc( UINT message, WPARAM wParam, LPARAM lParam, LRESULT& outResult )
{
	outResult = 0;
	ABool	handled = false;
	switch(message)
	{
		//コマンド実行
	  case WM_COMMAND:
		{
			if( lParam == 0 && mRedirectTextInput == true )
			{
				HWND	hwnd = AApplication::GetApplication().NVI_GetWindowByID(mRedirectWindowID).NVI_GetWindowRef();
						//win NVM_GetImp().GetHWnd();
				::PostMessage(hwnd, message, wParam, lParam);
				break;
			}
			//コントロールメッセージをルートウインドウへ送信
			::PostMessage(::GetAncestor(mHWnd,GA_ROOT), message, wParam, lParam);
			handled = true;
			break;
		}
		//ウインドウ描画
	  case WM_PAINT:
		{
			//OutputDebugStringA("StartWM_PAINTproc\n");
			mInWMPaint = true;
			AControlID	controlID = GetControlID();
			//Offscreenをコピーする（フレーム全体をコピー）
			//Offscreenが描画済みの場合は、ここからはDoDraw()はコールされない。
			ALocalRect	frame;
			GetControlLocalFrameRect(frame);
			PAINTSTRUCT ps;
			mPaintDC = ::BeginPaint(mHWnd,&ps);
			if( mPaintDC == NULL )   {_ACError("",NULL);break;}
			if( mOffscreenMode == true )
			{
				if( mOffscreenDC == NULL )
				{
					MakeOffscreen(mPaintDC);
				}
				else if( mPreparingOffscreen == true )
				{
					//Offscreenへの描画
					ALocalRect	localFrame;
					GetControlLocalFrameRect(localFrame);
					mUpdateRect = localFrame;
					DoDraw();
					//Prepare状態解除
					mPreparingOffscreen = false;
				}
				//char	str[256];
				//sprintf(str,"WM_PAINT(UserPane) hwnd:%X left:%d right:%d \n",mHWnd,ps.rcPaint.left,ps.rcPaint.right);
				//OutputDebugStringA(str);
				
				RECT	framerect;
				framerect.left		= frame.left;
				framerect.top		= frame.top;
				framerect.right		= frame.right;
				framerect.bottom	= frame.bottom;
				//::BitBlt(hdc,framerect.left,framerect.top,framerect.right-framerect.left,framerect.bottom-framerect.top,
				//		mOffscreenDC,framerect.left,framerect.top,SRCCOPY);
				DoDrawPreProcess();
				
				//最初に消去（）
				/*HBRUSH	hbr = ::CreateSolidBrush(GetWin().GetOverlayBackgroundColor());
				if( hbr == NULL )   {_ACError("",NULL);break;}
				::FillRect(hdc,&framerect,hbr);
				::DeleteObject(hbr);
				*/
				::BitBlt(mPaintDC,ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right-ps.rcPaint.left,ps.rcPaint.bottom-ps.rcPaint.top,
						mOffscreenDC,ps.rcPaint.left,ps.rcPaint.top,SRCCOPY);
				DoDrawPostProcess();
			}
			else
			{
				ALocalRect	localFrame;
				GetControlLocalFrameRect(localFrame);
				mUpdateRect = localFrame;
				DoDrawPreProcess();
				DoDraw();
				DoDrawPostProcess();
			}
			::EndPaint(mHWnd,&ps);
			mInWMPaint = false;
			handled = true;
			//OutputDebugStringA("EndWM_PAINTproc\n");
			break;
		}
	  case WM_TIMER:
		{
			switch(wParam)
			{
			  case kTimer_ShowTooltip:
				{
					//タイマー削除
					::KillTimer(GetHWnd(),kTimer_ShowTooltip);
					//ツールチップ表示
					ShowTooltip();
					break;
				}
			}
			break;
		}
		//
	  case WM_DESTROY:
		{
			DestroyUserPane();
			break;
		}
		//マウスクリック
	  case WM_LBUTTONDOWN:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint取得
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			//OverlayWindowへのリダイレクト実行試行（もし実行したならtrueで返る）
			if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
			{
				handled = true;
				break;
			}
			//
			if( DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,false) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_RBUTTONDOWN:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint取得
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			//OverlayWindowへのリダイレクト実行試行（もし実行したならtrueで返る）
			if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
			{
				handled = true;
				break;
			}
			//
			if( DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,true) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_LBUTTONDBLCLK:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint取得
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			//OverlayWindowへのリダイレクト実行試行（もし実行したならtrueで返る）
			if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
			{
				handled = true;
				break;
			}
			//
			if( DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),2,false) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_RBUTTONDBLCLK:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint取得
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			//OverlayWindowへのリダイレクト実行試行（もし実行したならtrueで返る）
			if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
			{
				handled = true;
				break;
			}
			//
			if( DoMouseDown(localPoint,AKeyWrapper::GetEventKeyModifiers(),2,true) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_LBUTTONUP:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint取得
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			if( mMouseTrackingMode == false )
			{
				//OverlayWindowへのリダイレクト実行試行（もし実行したならtrueで返る）
				if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
				{
					handled = true;
					break;
				}
			}
			//
			if( DoMouseUp(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,false) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
	  case WM_RBUTTONUP:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint取得
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			if( mMouseTrackingMode == false )
			{
				//OverlayWindowへのリダイレクト実行試行（もし実行したならtrueで返る）
				if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
				{
					handled = true;
					break;
				}
			}
			//
			if( DoMouseUp(localPoint,AKeyWrapper::GetEventKeyModifiers(),1,true) == true )
			{
				handled = true;
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			break;
		}
		//マウス移動
	  case WM_MOUSEMOVE:
		{
			ALocalPoint	localPoint;
			localPoint.x = GET_X_LPARAM(lParam);
			localPoint.y = GET_Y_LPARAM(lParam);
			//globalPoint取得
			AGlobalPoint	globalPoint;
			GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),localPoint,globalPoint);
			if( mMouseTrackingMode == false )
			{
				//OverlayWindowへのリダイレクト実行試行（もし実行したならtrueで返る）
				if( GetWin().RedirectToOverlayWindow(message,wParam,lParam,globalPoint) == true )
				{
					handled = true;
					break;
				}
			}
			//
			if( DoMouseMoved(localPoint,AKeyWrapper::GetEventKeyModifiers()) == true )
			{
				handled = true;
			}
			//
			//ACursorWrapper::ShowCursor();
			break;
		}
		//マウスがclient領域からはずれた
	  case WM_MOUSELEAVE:
		{
			DoMouseLeft();
			ACursorWrapper::ShowCursor();
			break;
		}
		//マウスホイールはCWindowImpに来る。
		//文字入力
	  case WM_CHAR:
		{
			AOSKeyEvent	keyevent;
			keyevent.message = message;
			keyevent.wParam = wParam;
			keyevent.lParam = lParam;
			//#688
			AKeyBindKey	keyBindKey = AKeyWrapper::GetKeyBindKey(keyevent);
			AModifierKeys	modifiers = AKeyWrapper::GetEventKeyModifiers(keyevent);
			//
			AText	text;
			AUTF16Char	ch = wParam;
			//制御文字チェック
			if( ch >= 0x00 && ch <= 0x1F )
			{
				ABool	canhandle = false;
				switch(ch)
				{
				  case kUChar_Tab:
				  case kUChar_Escape:
				  case '\r':
				  case 8:
					/*方向キーはWM_CHARがこない
				  case 0x1C:
				  case 0x1D:
				  case 0x1E:
				  case 0x1F:
					*/
					{
						canhandle = true;
						break;
					}
				}
				if( canhandle == false )   break;
			}
			//
			if( ch == kUChar_Backslash )
			{
				if( GetWin().IsInputBackslashByYenKeyMode() == false )
				{
					ch = 0x00A5;
				}
			}
			//text設定
			text.InsertFromUTF16TextPtr(0,&ch,sizeof(AUTF16Char));
			ABool	updatemenu = false;
			if( mRedirectTextInput == true )
			{
				if( AApplication::GetApplication().NVI_GetWindowByID(mRedirectWindowID).
							EVT_DoTextInput(mRedirectControlID,text,/*#688 keyevent*/keyBindKey,modifiers,updatemenu) == true )
				{
					::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
					handled = true;
					AApplication::GetApplication().NVI_UpdateMenu();
					break;
				}
			}
			else
			{
				if( GetAWin().EVT_DoTextInput(GetControlID(),text,/*#688 keyevent*/keyBindKey,modifiers,updatemenu) == true )
				{
					::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
					handled = true;
					AApplication::GetApplication().NVI_UpdateMenu();
					break;
				}
			}
			break;
		}
		//キー入力
		//WM_KEYDOWNのほうがrawイベント。
		//一部キーだけ、この後に、WM_CHARがくる。WM_CHARがくるこないは、WM_KEYDOWNの処理の仕方は影響しない。
		//（WM_KEYDOWNの返り値を0にして、DefWindowProc()をコールしない場合でも、WM_CHARは来る。）
	  case WM_KEYDOWN:
		{
			AOSKeyEvent	keyevent;
			keyevent.message = message;
			keyevent.wParam = wParam;
			keyevent.lParam = lParam;
			//メニューショートカット（Windowsの場合、メニューにショートカットを登録できない（表示のみ）。Acceleratorも固定。よって、動的メニューのショートカットキーはソフト制御する。）
			ANumber	shortcut = 0;
			switch(wParam)
			{
			  case 0x41:
			  case 0x42:
			  case 0x43:
			  case 0x44:
			  case 0x45:
			  case 0x46:
			  case 0x47:
			  case 0x48:
			  case 0x49:
			  case 0x4A:
			  case 0x4B:
			  case 0x4C:
			  case 0x4D:
			  case 0x4E:
			  case 0x4F:
			  case 0x50:
			  case 0x51:
			  case 0x52:
			  case 0x53:
			  case 0x54:
			  case 0x55:
			  case 0x56:
			  case 0x57:
			  case 0x58:
			  case 0x59:
			  case 0x5A:
				{
					shortcut = wParam;
					break;
				}
			  case '0':
			  case '1':
			  case '2':
			  case '3':
			  case '4':
			  case '5':
			  case '6':
			  case '7':
			  case '8':
			  case '9':
				{
					shortcut = wParam;
					break;
				}
			  case VK_OEM_1://0xBA
				{
					shortcut = ':';
					break;
				}
			  case VK_OEM_PLUS://0xBB
				{
					shortcut = ';';
					break;
				}
			  case VK_OEM_MINUS://0xBD
				{
					shortcut = '-';
					break;
				}
			  case VK_OEM_7://0xDE
				{
					shortcut = '^';
					break;
				}
			  case VK_OEM_5://0xDC
				{
					shortcut = '\\';
					break;
				}
			  case VK_OEM_3://0xC0
				{
					shortcut = '@';
					break;
				}
			  case VK_OEM_4://0xDB
				{
					shortcut = '[';
					break;
				}
			  case VK_OEM_6://0xDD
				{
					shortcut = ']';
					break;
				}
			  case VK_OEM_COMMA://0xBC
				{
					shortcut = ',';
					break;
				}
			  case VK_OEM_PERIOD://0xBE
				{
					shortcut = '.';
					break;
				}
			  case VK_OEM_2://0xBF
				{
					shortcut = '/';
					break;
				}
			}
			if( shortcut != 0 )
			{
				//動的メニューのショートカット検索
				AModifierKeys	modifiers = AKeyWrapper::GetEventKeyModifiers(keyevent);
				AMenuShortcutModifierKeys	menumodifiers = AKeyWrapper::MakeMenuShortcutModifierKeys(
											(AKeyWrapper::IsControlKeyPressed(modifiers) == true),
											(AKeyWrapper::IsCommandKeyPressed(modifiers) == true),
											(AKeyWrapper::IsShiftKeyPressed(modifiers) == true));
				AMenuItemID	menuItemID = 0;
				AText	actiontext;
				if( ADynamicMenu::FindShortcut(shortcut,menumodifiers,menuItemID,actiontext) == true )
				{
					//Winで実行試行
					if( GetAWin().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
					{
						AApplication::GetApplication().NVI_UpdateMenu();
						::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
						break;
					}
					//Appで実行試行
					else if( AApplication::GetApplication().EVT_DoMenuItemSelected(menuItemID,actiontext,0) == true )
					{
						AApplication::GetApplication().NVI_UpdateMenu();
						::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
						break;
					}
				}
			}
			//
			if( AKeyWrapper::GetKeyBindKey(keyevent) != kKeyBindKey_Invalid )
			{
				AText	text;
				//テキスト設定
				//カーソルキーはWM_CHARにこないので、keyeventではなくtextで処理するviewに対して(AView_EditBox等）、textを設定する必要がある。
				//deleteキーも同様
				switch(wParam)
				{
				  case VK_UP:
					{
						text.Add(0x1E);
						break;
					}
				  case VK_DOWN:
					{
						text.Add(0x1F);
						break;
					}
				  case VK_LEFT:
					{
						text.Add(0x1C);
						break;
					}
				  case VK_RIGHT:
					{
						text.Add(0x1D);
						break;
					}
				  case VK_DELETE:
					{
						text.Add(0x7F);
						break;
					}
				}
				//
				ABool	updatemenu = false;
				if( mRedirectTextInput == true )
				{
					if( AApplication::GetApplication().NVI_GetWindowByID(mRedirectWindowID).
								EVT_DoTextInput(mRedirectControlID,text,/*#688 keyevent*/keyBindKey,modifiers,updatemenu) == true )
					{
						::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
						handled = true;
						AApplication::GetApplication().NVI_UpdateMenu();
					}
					else
					{
						handled = false;
					}
				}
				else
				{
					if( GetAWin().EVT_DoTextInput(GetControlID(),text,/*#688 keyevent*/keyBindKey,modifiers,updatemenu) == true )
					{
						::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
						handled = true;
						AApplication::GetApplication().NVI_UpdateMenu();
					}
					else
					{
						handled = false;
					}
				}
			}
			//handleした場合、WM_CHAR実行しないようにする
			if( handled == true )
			{
				CAppImp::DisableNextTranslateMessage();
			}
			break;
		}
		//IME
	  case WM_IME_COMPOSITION:
		{
			char	buf[4096];
			HIMC	hImc = ::ImmGetContext(mHWnd);
			AText	comp;
			//if( (lParam&GCS_COMPSTR) != 0 )
			{
				LONG	bytecount = ::ImmGetCompositionStringW(hImc,GCS_COMPSTR,buf,4096);
				comp.InsertFromUTF16TextPtr(0,(AUTF16CharPtr)buf,bytecount);
			}
			AText	fix;
			//if( (lParam&GCS_RESULTSTR) != 0 )
			{
				LONG	bytecount = ::ImmGetCompositionStringW(hImc,GCS_RESULTSTR,buf,4096);
				fix.InsertFromUTF16TextPtr(0,(AUTF16CharPtr)buf,bytecount);
			}
			AText	inputUTF8Text;
			inputUTF8Text.AddText(fix);
			inputUTF8Text.AddText(comp);
			AItemCount	fixlen = fix.GetItemCount();
			comp.GetUTF16TextIndexInfo(mCurrentInlineInputUTF16PosIndexArray,mCurrentInlineInputUTF8PosIndexArray);
			AArray<AIndex>	hiliteLineStyleIndex;
			AArray<AIndex>	hiliteStartPos;
			AArray<AIndex>	hiliteEndPos;
			AArray<AColor>	hiliteColor;
			{
				LONG	clause[1024];
				LONG	clausebytecount = ::ImmGetCompositionStringW(hImc,GCS_COMPCLAUSE,clause,1024*sizeof(clause));
				char	attr[4096];
				LONG	attrbytecount = ::ImmGetCompositionStringW(hImc,GCS_COMPATTR,attr,4096);
				AItemCount	inputing = false;
				AItemCount	clausecount = clausebytecount/sizeof(LONG);
				for( AIndex i = 0; i < clausecount-1; i++ )
				{
					AIndex	spos = clause[i];
					AIndex	epos = clause[i+1];
					if( spos >= 4096 || epos >= 4096 )   break;
					AIndex	style = 0;
					switch(attr[spos])
					{
					  case ATTR_INPUT:
						{
							style = 1;
							inputing = true;
							break;
						}
					  case ATTR_TARGET_CONVERTED:
						{
							style = 0;
							break;
						}
					  case ATTR_CONVERTED:
						{
							style = 2;
							break;
						}
					  case ATTR_TARGET_NOTCONVERTED:
						{
							style = 3;
							break;
						}
					  default:
						{
							style = 4;
							break;
						}
					}
					/*
					AText	t;
					t.SetFormattedCstring("(%d)",style);
					t.OutputToStderr();
					*/
					hiliteLineStyleIndex.Add(style);
					hiliteStartPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(spos)+fixlen);
					hiliteEndPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(epos)+fixlen);
					//ここの+fixlenについて：
					//Windowsの場合、変換後すぐに次の文字入力した場合に、fixlen=前回の変換文字の長さ、
					//spos,epos=今回の変換開始からのオフセットで来る。
					//Macの場合と処理を合わせるために、ここで調整する
				}
				//キャレット位置設定
				if( inputing == true )
				{
					LONG	utf16pos = ::ImmGetCompositionStringW(hImc,GCS_CURSORPOS,NULL,0);
					/*
					AText	t;
					t.SetFormattedCstring("%X %d %d %d\n",lParam,wParam,utf16pos,
							::ImmGetCompositionStringW(hImc,GCS_DELTASTART,NULL,0));
					t.OutputToStderr();
					*/
					if( utf16pos < mCurrentInlineInputUTF8PosIndexArray.GetItemCount() )
					{
						hiliteLineStyleIndex.Add(kIndex_Invalid);
						hiliteStartPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(utf16pos)+fixlen);
						hiliteEndPos.Add(mCurrentInlineInputUTF8PosIndexArray.Get(utf16pos)+fixlen);
					}
				}
			}
			::ImmReleaseContext(mHWnd,hImc);
			ABool	updateMenu = false;
			if( GetAWin().EVT_DoInlineInput(GetControlID(),inputUTF8Text,fixlen,
							hiliteLineStyleIndex,hiliteColor,hiliteStartPos,hiliteEndPos,0,updateMenu) == true )
			{
				::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
			}
			if( updateMenu == true )
			{
				AApplication::GetApplication().NVI_UpdateMenu();
			}
			handled = true;
			break;
		}
	  case WM_IME_STARTCOMPOSITION:
		{
			//★暫定 タイミング調査必要
			ALocalPoint	localPoint;
			if( GetAWin().EVT_DoInlineInputOffsetToPos(GetControlID(),0,localPoint) == true )
			{
				HIMC	hImc = ::ImmGetContext(mHWnd);
				CANDIDATEFORM	cf;
				cf.dwIndex = 0;//★暫定 要調査
				cf.dwStyle = CFS_CANDIDATEPOS;
				cf.ptCurrentPos.x = localPoint.x;
				cf.ptCurrentPos.y = localPoint.y+3;
				::ImmSetCandidateWindow(hImc,&cf);
				::ImmReleaseContext(mHWnd,hImc);
			}
			handled = true;
			break;
		}
	  case WM_IME_ENDCOMPOSITION:
		{
			handled = true;
			break;
		}
		//IME文字
		/*case WM_IME_CHAR:
		{
		AOSKeyEvent	keyevent;
		keyevent.message = message;
		keyevent.wParam = wParam;
		keyevent.lParam = lParam;
		AText	text;
		AUTF16Char	ch = wParam;
		text.InsertFromUTF16TextPtr(0,&ch,sizeof(AUTF16Char));
		ABool	updatemenu = false;
		if( GetAWin().EVT_DoTextInput(GetControlID(),text,keyevent,updatemenu) == true )
		{
		handled = true;
		AApplication::GetApplication().NVI_UpdateMenu();
		break;
		}
		break;
		}*/
		//
	  case WM_SIZE:
		{
			//debug
			//char	str[256];
			//sprintf(str,"WM_SIZE(UserPane) hwnd:%X wParam:%d x:%d y:%d\n",
			//		mHWnd,wParam,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
			//OutputDebugStringA(str);
			if( mOffscreenMode == true )
			{
				//新規サイズ取得
				ANumber	newWidth =  GET_X_LPARAM(lParam);
				ANumber	newHeight = GET_Y_LPARAM(lParam);
				if( newWidth == 0 )   break;
				if( newHeight == 0 )   break;
				//現在のoffscreenのサイズと同じなら何もしない
				if( newWidth == mOffscreenWidth && newHeight == mOffscreenHeight )   break;
				
				//
				DoControlBoundsChanged();
				//新規サイズでoffscreen再作成
				if( mPreparingOffscreen == true )
				{
					DisposeOffscreen();
					PrepareOffscreen();
				}
				else
				{
					HDC	hdc = ::GetDC(mHWnd);
					if( hdc == NULL )   {_ACError("",NULL);return false;}
					if( mOffscreenDC != NULL )
					{
						if( IsControlVisible() == true )
						{
							MakeOffscreen(hdc);
						}
						else
						{
							//非表示中ならOffscreenを削除。→次回WM_PAINTで更新。
							//MakeOffscreen()はOffscreenを作成しなおす（EVT_DoDraw()が呼ばれる）が、
							//EVT_DoDraw()はコントロール表示中以外は何もしないので。
							DisposeOffscreen();
						}
					}
					::ReleaseDC(mHWnd,hdc);
				}
			}
			break;
		}
	  case WM_HSCROLL:
		{
			::PostMessage(::GetAncestor(mHWnd,GA_ROOT), message, wParam, lParam);
			return 0L;
			break;
		}
	  case WM_VSCROLL:
		{
			::PostMessage(::GetAncestor(mHWnd,GA_ROOT), message, wParam, lParam);
			return 0L;
			break;
		}
	  case WM_SETCURSOR:
		{
			/*Tracking Tooltip対応後、AView_Listでカーソルがちらつく現象が発生したので、ここはコメントアウト。
			WM_SETCURSORはhandled=trueで返さないと、テキストエリアでちらつく
			::ACursorWrapper::SetCursor(mDefaultCursorType);
			*/
			handled = true;
			break;
		}
	  case WM_KILLFOCUS:
		{
			FixInlineInput();
			ACursorWrapper::ShowCursor();
			break;
		}
	  default:
		{
			handled = false;
			break;
		}
	}
	return handled;
}

#pragma mark ===========================

#pragma mark --- IME

/**
IMEの強制確定
*/
void	CUserPane::FixInlineInput()
{
	//AView内の未確定テキストをすべて確定させる
	AArray<AIndex>	hiliteLineStyleIndex;
	AArray<AIndex>	hiliteStartPos;
	AArray<AIndex>	hiliteEndPos;
	ABool	updateMenu = false;
	if( GetAWin().EVT_DoInlineInput(GetControlID(),GetEmptyText(),kIndex_Invalid,
					hiliteLineStyleIndex,hiliteStartPos,hiliteEndPos,updateMenu) == true )
	{
		//::UpdateWindow(mHWnd);//これがないとキーボード入力速度（リピート時等）＜処理時間のときに、描画できなくなる
	}
	if( updateMenu == true )
	{
		AApplication::GetApplication().NVI_UpdateMenu();
	}
	//OS側のIME入力をキャンセル
	HIMC	hImc = ::ImmGetContext(mHWnd);
	::ImmNotifyIME(hImc,NI_COMPOSITIONSTR,CPS_CANCEL,0);
	::ImmReleaseContext(mHWnd,hImc);
}

#pragma mark ===========================

#pragma mark --- APIコールバック(static)

LRESULT CALLBACK CUserPane::STATIC_APICB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(W-PRC)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return 0;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	
	//hWndに対応するCWindowImpオブジェクトを検索
	AIndex	index = sAliveUserPaneArray_HWnd.Find(hWnd);
	if( index == kIndex_Invalid )
	{
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	//インスタンスのコールバックメソッドを呼び出す
	LRESULT	result = 0;
	try
	{
		if( (sAliveUserPaneArray.Get(index))->APICB_WndProc(message,wParam,lParam,result) == false )
		{
			result = ::DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_WndProc() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[W-PRC]");
	return result;
}


#pragma mark ===========================
#pragma mark [クラス]CDropTarget
#pragma mark ===========================
//COM Drag&Drop実装クラス （ドラッグ先オブジェクト）
//参考：http://hp.vector.co.jp/authors/VA016117/oledragdrop2.html 

CDropTarget::CDropTarget( CUserPane& inUserPane ): mRefCount(1), mUserPane(inUserPane)
{
}

CDropTarget::~CDropTarget()
{
}

HRESULT __stdcall CDropTarget::QueryInterface( const IID& iid, void** ppv )
{
	HRESULT hr;
	
	if( iid == IID_IDropTarget || iid == IID_IUnknown )
	{
		hr = S_OK;
		*ppv = (void*)this;
		AddRef();
	}
	else
	{
		hr = E_NOINTERFACE;
		*ppv = 0;
	}
	return hr;
}

ULONG __stdcall CDropTarget::AddRef()
{
	::InterlockedIncrement(&mRefCount);
	return (ULONG)mRefCount;
}

ULONG __stdcall CDropTarget::Release()
{
	ULONG ret = (ULONG)::InterlockedDecrement(&mRefCount);
	if( ret == 0 )
	{
		delete this;
	}
	return (ULONG)mRefCount;
}

HRESULT __stdcall CDropTarget::DragEnter( IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	//受信可能データが含まれているかどうかのチェック
	if( ExistAcceptableData(pDataObject) == false )
	{
		*pdwEffect = DROPEFFECT_NONE;
		return E_UNEXPECTED;
	}
	
	//
	if( mUserPane.DragEnter(pDataObject,grfKeyState,ptl,pdwEffect) == true )
	{
		return S_OK;
	}
	else
	{
		return E_UNEXPECTED;
	}
}

HRESULT __stdcall CDropTarget::DragOver( DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	if( mUserPane.DragOver(grfKeyState,ptl,pdwEffect) == true )
	{
		return S_OK;
	}
	else
	{
		return E_UNEXPECTED;
	}
}

HRESULT __stdcall CDropTarget::DragLeave()
{
	if( mUserPane.DragLeave() == true )
	{
		return S_OK;
	}
	else
	{
		return E_UNEXPECTED;
	}
}

HRESULT __stdcall CDropTarget::Drop( IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
	//受信可能データが含まれているかどうかのチェック
	if( ExistAcceptableData(pDataObject) == false )
	{
		*pdwEffect = DROPEFFECT_NONE;
		return E_UNEXPECTED;
	}
	
	//
	if( mUserPane.Drop(pDataObject,grfKeyState,pt,pdwEffect) == true )
	{
		return S_OK;
	}
	else
	{
		return E_UNEXPECTED;
	}
}

void	CDropTarget::AddAcceptableType( const AScrapType inAcceptableType )
{
	mAcceptableType.Add(inAcceptableType);
}

ABool	CDropTarget::ExistAcceptableData( IDataObject* inDataObject ) const
{
	for( AIndex i = 0; i < mAcceptableType.GetItemCount(); i++ )
	{
		FORMATETC	fmt;
		fmt.cfFormat = mAcceptableType.Get(i);
		fmt.ptd = NULL;
		fmt.dwAspect = DVASPECT_CONTENT;
		fmt.lindex = -1;
		fmt.tymed = TYMED_HGLOBAL;
		if( inDataObject->QueryGetData(&fmt) == S_OK )
		{
			return true;
		}
	}
	return false;
}

#pragma mark ===========================
#pragma mark [クラス]CDropSource
#pragma mark ===========================
//COM Drag&Drop実装クラス （ドラッグ元オブジェクト）
//参考：http://hp.vector.co.jp/authors/VA016117/oledragdrop2.html 

CDropSource::CDropSource() : mRefCount(1)
{
}

CDropSource::~CDropSource()
{
}

HRESULT __stdcall CDropSource::QueryInterface( const IID& iid, void** ppv )
{
	HRESULT hr;
	
	if( iid == IID_IDropSource || iid == IID_IUnknown )
	{
		hr = S_OK;
		*ppv = (void*)this;
		AddRef();
	}
	else
	{
		hr = E_NOINTERFACE;
		*ppv = 0;
	}
	return hr;
}

ULONG __stdcall CDropSource::AddRef()
{
	::InterlockedIncrement(&mRefCount);
	return (ULONG)mRefCount;
}

ULONG __stdcall CDropSource::Release()
{
	ULONG ret = (ULONG)::InterlockedDecrement(&mRefCount);
	if( ret == 0 )
	{
		delete this;
	}
	return (ULONG)mRefCount;
}

HRESULT __stdcall CDropSource::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	if( fEscapePressed == TRUE )
	{
		return DRAGDROP_S_CANCEL;
	}
	
	if( (grfKeyState & (MK_LBUTTON | MK_RBUTTON)) == 0 )
	{
		return DRAGDROP_S_DROP;
	}
	return S_OK;
}

HRESULT __stdcall CDropSource::GiveFeedback( DWORD dwEffect )
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

#pragma mark ===========================
#pragma mark [クラス]CDataObject
#pragma mark ===========================
//COM Drag&Drop実装クラス　（データ内容）

CDataObject::CDataObject() : mRefCount(1),mDataCount(0)
{
}

CDataObject::~CDataObject()
{
	for( AIndex i = 0; i < mDataCount; i++ )
	{
		::ReleaseStgMedium(&(mMedium[i]));
	}
}

HRESULT __stdcall CDataObject::QueryInterface( const IID& iid, void** ppv )
{
	HRESULT hr;
	
	if( iid == IID_IDataObject || iid == IID_IUnknown )
	{
		hr = S_OK;
		*ppv = (void*)this;
		AddRef();
	}
	else
	{
		hr = E_NOINTERFACE;
		*ppv = 0;
	}
	return hr;
}

ULONG __stdcall CDataObject::AddRef()
{
	::InterlockedIncrement(&mRefCount);
	return (ULONG)mRefCount;
}

ULONG __stdcall CDataObject::Release()
{
	ULONG ret = (ULONG)::InterlockedDecrement(&mRefCount);
	if(ret == 0)
	{
		delete this;
	}
	return (ULONG)mRefCount;
}

//指定フォーマットのデータ取得
HRESULT __stdcall	CDataObject::GetData( FORMATETC *pFormatetc, STGMEDIUM *pMedium )
{
	if( pFormatetc == NULL || pMedium == NULL ) return E_INVALIDARG;
	
	if( (pFormatetc->dwAspect&DVASPECT_CONTENT) == 0 )   return DV_E_DVASPECT;
	
	for( AIndex i = 0; i < mDataCount; i++ )
	{
		if( pFormatetc->cfFormat == mFormat[i].cfFormat &&
					(pFormatetc->tymed & mFormat[i].tymed) != 0 )
		{
			if( DuplicateMedium(pMedium,&(mFormat[i]),&(mMedium[i])) == false )
			{
				return E_OUTOFMEMORY;
			}
			else
			{
				return S_OK;
			}
		}
	}
	
	return DV_E_FORMATETC;
}

//指定フォーマットのデータ存在チェック
HRESULT __stdcall	CDataObject::QueryGetData( FORMATETC *pFormatetc )
{
	if( pFormatetc == NULL ) return E_INVALIDARG;
	
	if( (pFormatetc->dwAspect&DVASPECT_CONTENT) == 0 )   return DV_E_DVASPECT;
	
	for( AIndex i = 0; i < mDataCount; i++ )
	{
		if( pFormatetc->cfFormat == mFormat[i].cfFormat &&
					(pFormatetc->tymed & mFormat[i].tymed) != 0 )
		{
			return S_OK;
		}
	}
	
	return DV_E_FORMATETC;
}

//
HRESULT __stdcall	CDataObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppenumFormatetc )
{
	return S_OK;
}

//データ追加
HRESULT __stdcall	CDataObject::SetData( FORMATETC *pFormatetc, STGMEDIUM *pMedium, BOOL fRelease )
{
	//引数チェック
	if( pFormatetc == NULL || pMedium == NULL ) return E_INVALIDARG;
	
	//データ数チェック
	if( mDataCount >= kDataCountMax )   return E_OUTOFMEMORY;
	
	if( fRelease == TRUE )
	{
		//所有権は本インスタンスに移動
		mFormat[mDataCount] = *pFormatetc;
		mMedium[mDataCount] = *pMedium;
	}
	else
	{
		//引数のmediumはコール側が所有権保持、本インスタンスへコピー
		mFormat[mDataCount] = *pFormatetc;
		DuplicateMedium(&(mMedium[mDataCount]),pFormatetc,pMedium);
	}
	mDataCount++;
	
	return S_OK;
}

HRESULT __stdcall	CDataObject::GetDataHere( FORMATETC *pFormatetc, STGMEDIUM *pMedium )
{
	return E_NOTIMPL;
}

HRESULT __stdcall	CDataObject::GetCanonicalFormatEtc( FORMATETC *pFormatetcIn, FORMATETC *pFormatetcOut )
{
	return E_NOTIMPL;
}

HRESULT __stdcall	CDataObject::DAdvise( FORMATETC *pFormatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection )
{
    return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT __stdcall	CDataObject::DUnadvise( DWORD dwConnection )
{
    return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT __stdcall	CDataObject::EnumDAdvise( IEnumSTATDATA **ppEnumAdvise )
{
    return OLE_E_ADVISENOTSUPPORTED;
}

//mediumデータコピー
ABool	CDataObject::DuplicateMedium( STGMEDIUM *pdest, const FORMATETC* pFormatetc, const STGMEDIUM *pMedium )
{
	switch(pMedium->tymed)
	{
	  case TYMED_HGLOBAL:
		{
			pdest->hGlobal = (HGLOBAL)::OleDuplicateData(pMedium->hGlobal, pFormatetc->cfFormat, (UINT)NULL);
			break;
		}
	  case TYMED_GDI:
		{
			pdest->hBitmap = (HBITMAP)::OleDuplicateData(pMedium->hBitmap, pFormatetc->cfFormat, (UINT)NULL);
			break;
		}
	  case TYMED_MFPICT:
		{
			pdest->hMetaFilePict = (HMETAFILEPICT)::OleDuplicateData(pMedium->hMetaFilePict, pFormatetc->cfFormat, (UINT)NULL);
			break;
		}
	  case TYMED_ENHMF:
		{
			pdest->hEnhMetaFile = (HENHMETAFILE)::OleDuplicateData(pMedium->hEnhMetaFile, pFormatetc->cfFormat, (UINT)NULL);
			break;
		}
	  case TYMED_FILE:
		{
			pdest->lpszFileName = (LPOLESTR)::OleDuplicateData(pMedium->lpszFileName, pFormatetc->cfFormat, (UINT)NULL);
			break;
		}
	  case TYMED_NULL:
		{
			break;
		}
	  case TYMED_ISTREAM:
	  case TYMED_ISTORAGE:
	  default:
		{
			return false;
			break;
		}
	}
	pdest->tymed = pMedium->tymed;
	pdest->pUnkForRelease = pMedium->pUnkForRelease;
	if( pMedium->pUnkForRelease != NULL )   pMedium->pUnkForRelease->AddRef();
	return true;
}





