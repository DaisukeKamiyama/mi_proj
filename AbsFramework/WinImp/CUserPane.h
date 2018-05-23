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

#pragma once

#include "../AbsBase/ABase.h"
#include "CWindowImp.h"
#include "CTextDrawData.h"
#include <windows.h>
#include <objidl.h>
#include <commctrl.h>

class AWindow;
class CDropTarget;

#pragma mark ===========================
#pragma mark [クラス]CUserPane
/**
UserPane実装クラス
*/
class CUserPane: public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	CUserPane( AObjectArrayItem* inParent, CWindowImp& inWindow );
	virtual ~CUserPane();
	
	//AWindow取得
  public:
	AWindow&	GetAWin();
	
	//コントロール情報取得
  public:
	AControlID	GetControlID() const;
	void	GetControlPosition( AWindowPoint& outPoint ) const;
	void	GetControlLocalFrameRect( ALocalRect& outRect ) const;
	void	GetControlWindowFrameRect( AWindowRect& outRect ) const;
	
	//イベントハンドラ
  public:
	virtual ABool	DoDraw() = 0;
	virtual void	DoDrawPreProcess() {}
	virtual void	DoDrawPostProcess() {}
	virtual ABool	DoMouseDown( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
		const ANumber inClickCount, const ABool inRightButton ) { return false; }
	virtual ABool	DoMouseUp( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
		const ANumber inClickCount, const ABool inRightButton ) { return false; }
	virtual ABool	DoMouseMoved( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers ) { return false; }
	virtual ABool	DoMouseWheel( const AFloatNumber inDeltaY, const AModifierKeys inModifiers, const ALocalPoint inLocalPoint )  { return false; }
	virtual void	DoWindowActivated() {}
	virtual void	DoWindowDeactivated() {}
	virtual void	DoControlBoundsChanged() {}
	virtual void	DoMouseLeft() {}
	
	//ウインドウ制御
  public:
	void	CreateUserPane( const HWND inParent, const AControlID inControlID, const APoint& inPoint, 
			const ANumber inWidth, const ANumber inHeight, const ABool inFront );
	virtual void	DestroyUserPane();
	void	SetMouseTrackingMode( const ABool inSet );
	void	EnableMouseLeaveEvent();
	static void	RegisterUserPaneWindow();
	ABool	IsMouseLeaveEventTracking() const { return mMouseLeaveEventTracking; }
  protected:
	ABool	mMouseTrackingMode;
	ABool	mMouseLeaveEventEnable;
	ABool	mMouseLeaveEventTracking;
	
	//メニューコマンド処理
  public:
	virtual ABool	DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	virtual void	UpdateMenu();
	
	//コントロール操作インターフェイス
  public:
	virtual void	SetFocus();
	virtual void	ResetFocus();
	/*#360
	ABool					GetSupportFocus() const { return mSupportFocus; }
	void					SetSupportFocus( const ABool inSupportFocus ) { mSupportFocus = inSupportFocus; }
  private:
	ABool								mSupportFocus;
	*/
	
	//コントロール属性設定インターフェイス
  public:
	virtual void	SetEnableControl( const ABool inEnable );
	virtual ABool	IsControlEnabled() const;
	virtual void	SetShowControl( const ABool inShow );
	virtual ABool	IsControlVisible() const;
	virtual ABool	IsControlActive() const;
	virtual void	SetTextFont( const AText& inFontName, const AFontSize inFontSize ) {_AError("not implemented",this);}
	virtual void	SetTextStyle( const ABool inBold ) {_AError("not implemented",this);}
	virtual void	SetIcon( const AIconID inIconID, 
					const ABool inDrawDefaultSize, const ANumber inLeftOffset, const ANumber inTopOffset ) {}
	void					SetDefaultCursor( const ACursorType inCursorType );
	void					SetControlSize( const ANumber inWidth, const ANumber inHeight );//#282
	void					SetControlPosition( const AWindowPoint& inWindowPoint );//#282
	void					SetControlBindings( const ABool inLeft, const ABool inTop, const ABool inRight, const ABool inBottom );
  private:
	ACursorType					mDefaultCursorType;
	
	//コントロール値Set/Getインターフェイス（以下のうち必要なものだけをサブクラスで実装）
  public:
	virtual void	GetText( AText& outText ) const {_AError("not implemented",this);}
	virtual void	SetText( const AText& inText ) {_AError("not implemented",this);}
	virtual void	AddText( const AText& inText ) {_AError("not implemented",this);}
	//virtual void	InputText( const AText& inText ) {_AError("not implemented",this);}
	virtual void	SetBool( const ABool inBool ) {_AError("not implemented",this);}
	virtual void	GetBool( ABool& outBool ) const  {_AError("not implemented",this);}
	virtual void	GetColor( AColor& outColor ) const {_AError("not implemented",this);}
	virtual void	SetColor( const AColor& inColor ) {_AError("not implemented",this);}
	virtual void	GetNumber( ANumber& outNumber ) const {_AError("not implemented",this);}
	virtual void	SetNumber( const ANumber inNumber ) {_AError("not implemented",this);}
	virtual void	GetFloatNumber( AFloatNumber& outNumber ) const {_AError("not implemented",this);}
	virtual void	SetFloatNumber( const AFloatNumber inNumber ) {_AError("not implemented",this);}
	virtual void	SetEmptyValue() {_AError("not implemented",this);}
	virtual void	SetTextJustification( const AJustification inJustification ) {_AError("not implemented",this);}
	virtual void	SetToggleEnable( const ABool inToggleEnable ) {_AError("not implemented",this);}
	virtual void	ResetUndoStack() {}
	
	//コントロール内選択インターフェイス
  public:
	virtual void	SetSelection( const AIndex inStart, const AIndex inEnd ) {}
	virtual void	SetSelectionAll() {}
	
	//描画属性設定 
  public:
	void	SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing );
	void	SetDefaultTextProperty( const AColor& inColor, const ATextStyle inStyle );
	HFONT					GetHFont( const ATextStyle inStyle ) const;
	//APIコールバック
  public:
	ABool	APICB_WndProc( UINT message, WPARAM wParam, LPARAM lParam, LRESULT& outResult );
  private:
	mutable HFONT						mHDefaultFont[8];
  protected:
	AText								mFontName;
	AFloatNumber						mFontSize;
	AColor								mColor;
	ATextStyle							mStyle;
	ABool								mAntiAliasing;
	
	//描画ルーチン
  public:
	void					DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification = kJustification_Left );
	void					DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, const AJustification inJustification = kJustification_Left );
	void					DrawText( const ALocalRect& inDrawRect, 
							const AText& inText, const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification = kJustification_Left );
	void					DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
							const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification = kJustification_Left );
	void					DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification = kJustification_Left );
	void					DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, CTextDrawData& inTextDrawData, 
							const AJustification inJustification = kJustification_Left );
	void					DrawIcon( const ALocalRect& inRect, const AIconID inIconID, const ABool inEnabledColor, const ABool inDefaultSize = false ) const;
	void					DrawIconFromFile( const ALocalRect& inRect, const AFileAcc& inFile, const ABool inEnabledColor ) const;
	void					PaintPoly( const AArray<ALocalPoint>& inPath, const AColor& inColor, const AFloatNumber inAlpha );//#379
	void					PaintRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha = 1.0 ) const;
	void					PaintRectWithVerticalGradient( const ALocalRect& inRect, 
							const AColor& inTopColor, const AColor& inBottomColor, const AFloatNumber inAlpha, const ABool inDrawUsingLines );//#634
	void					PaintRectWithHorizontalGradient( const ALocalRect& inRect, 
							const AColor& inTopColor, const AColor& inBottomColor, const AFloatNumber inAlpha );//#634
	void					EraseRect( const ALocalRect& inRect );
	void					FrameRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha = 1.0 );
	void					DrawFocusFrame( const ALocalRect& inRect );
	void					DrawBackgroundImage( const ALocalRect& inDrawRect, const AImageRect& inImageFrameRect, 
							const ANumber inLeftOffset, const AIndex inBackgroundImageIndex, const AFloatNumber inAlpha );
	void					DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
							const AFloatNumber inAlpha = 1.0, const AFloatNumber inLineDash = 0.0, const AFloatNumber inLineWidth = 1.0 );
	void					SetDrawLineMode( const AColor& inColor, const AFloatNumber inAlpha = 1.0, 
							const AFloatNumber inLineDash = 0.0, const AFloatNumber inLineWidth = 1.0 );//B0000
	void					RestoreDrawLineMode();//B0000
	void					DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint );//B0000
	void					DrawXorCaretLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const ABool inClipToFrame, 
							const ABool inDash = false, const ANumber inPenSize = 1 );
	void					DrawXorCaretRect( const ALocalRect& inLocalRect, const ABool inClipToFrame );
	void					RedrawImmediately() {}
  private:
	HPEN								mDrawLinePen;
	
	//Refresh
  public:
	void					RefreshRect( const ALocalRect& inLocalRect );
	void					RefreshControl();
	void					FlushRect( const ALocalRect& inLocalRect );
	void					FlushControl();
	
	//Offscreen
  public:
	void					PrepareOffscreen();
	void					TransferOffscreen();
  protected:
	HDC									mOffscreenDC;
	ANumber								mOffscreenWidth;
	ANumber								mOffscreenHeight;
  private:
	void					MakeOffscreen( HDC inHDC );
	void					DisposeOffscreen();
	HBITMAP								mOffscreenBitmap;
	ALocalRect							mUpdateRect;
	ABool								mInWMPaint;
	ABool								mPreparingOffscreen;
	
	//Offscreenモード／直接描画モード
  public:
	void					SetOffscreenMode( const ABool inOffscreenMode );
  private:
	ABool								mOffscreenMode;
	HDC									mPaintDC;
	
	//描画用情報取得
  public:
	ANumber					GetImageXByTextPosition( CTextDrawData& inTextDrawData, const AIndex inTextPosition );
	ANumber					GetImageXByUnicodeOffset( CTextDrawData& inTextDrawData, const AIndex inUnicodeOffset );
	AIndex					GetTextPositionByImageX( CTextDrawData& inTextDrawData, const ANumber inImageX );
	AFloatNumber			GetDrawTextWidth( const AText& inText );
	AFloatNumber			GetDrawTextWidth( CTextDrawData& inTextDrawData );
	ABool					IsRectInDrawUpdateRegion( const ALocalRect& inLocalRect ) const;
	void					GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent ) const;
	//スクロール
  public:
	void					Scroll( const ANumber inDeltaX, const ANumber inDeltaY );
	
	//Drag
  public:
	void					EnableDrop( const AArray<AScrapType>& inScrapType );
	void					Drag( const ALocalPoint& inMousePoint, const AArray<ALocalRect>& inDragRect, const AArray<AScrapType>& inScrapType, const ATextArray& inData );
	virtual ABool			DragEnter( IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect );
	virtual ABool			DragOver( DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect );
	virtual ABool			DragLeave();
	virtual ABool			Drop( IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect );
  private:
	CDropTarget*						mDropTarget;
	
	//カーソル
  public:
	void					ObscureCursor();
	
	//Tooltip
  public:
	virtual void			EnableTrackingToopTip( const ABool inShowToolTipOnlyWhenNarrow = true ) {}
	virtual void			SetFixedTextTooltip( const AText& inText, const AHelpTagSide inTagSide ) {}
  private:
	virtual void			ShowTooltip() {}
	
	//Redirect
  public:
	void					SetRedirectTextInput( const AWindowID inWindowID, const AControlID inControlID )
	{
		mRedirectTextInput = true;
		mRedirectWindowID = inWindowID;
		mRedirectControlID = inControlID;
	}
  private:
	ABool								mRedirectTextInput;
	AWindowID							mRedirectWindowID;
	AControlID							mRedirectControlID;
	
  public:
	virtual void			TellChangedToWindow();
	
	//Timer
  protected:
	static const UINT_PTR				kTimer_ShowTooltip = 1;
	
	//IME
  public:
	void					FixInlineInput();
	
	//APIコールバック(static)
  private:
	static LRESULT CALLBACK STATIC_APICB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	
  public:
	CWindowImp&				GetWin() { return mWindowImp; }
	CWindowImp&				GetWinConst() const  { return mWindowImp; }
	CWindowImp&							mWindowImp;
	
  public:
	HWND					GetHWnd() const { return mHWnd; }
  private:
	HWND								mHWnd;
	AControlID							mControlID;
	
  private:
	static AArray<CUserPane*>			sAliveUserPaneArray;
	static AArray<HWND>					sAliveUserPaneArray_HWnd;
	AArray<AIndex>						mCurrentInlineInputUTF8PosIndexArray;
	AArray<AIndex>						mCurrentInlineInputUTF16PosIndexArray;
	
	//自動Test用
  public:
	ANumber					GetDrawTextCount() const { return mDrawTextCount; }
	void					ClearDrawTextCount() { mDrawTextCount = 0; }
  private:
	ANumber								mDrawTextCount;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "CUserPane"; }
	
};

typedef AAbstractFactoryForObjectArray<CUserPane>	CUserPaneFactory;

//OLE Drag&Drop

#pragma mark ===========================
#pragma mark [クラス]CDropTarget

class CDropTarget : public IDropTarget
{
  public:
	CDropTarget( CUserPane& inUserPane );
	~CDropTarget();
	
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	ULONG __stdcall AddRef(void);
	ULONG __stdcall Release(void);
	
	HRESULT __stdcall DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect  );
	HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect);
	HRESULT __stdcall DragLeave();
	HRESULT __stdcall Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	
	void	AddAcceptableType( const AScrapType inAcceptableType );
	ABool	ExistAcceptableData( IDataObject* inDataObject ) const;
	
  private:
	CUserPane&	mUserPane;
	LONG	mRefCount;
	AArray<AScrapType>	mAcceptableType;
};

#pragma mark ===========================
#pragma mark [クラス]CDropSource

class CDropSource : public IDropSource
{
  public:
	CDropSource();
	~CDropSource();
	
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	ULONG __stdcall AddRef(void);
	ULONG __stdcall Release(void);
	
	HRESULT __stdcall QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
	HRESULT __stdcall GiveFeedback(DWORD dwEffect);
	
  private:
	LONG	mRefCount;
};

#pragma mark ===========================
#pragma mark [クラス]CDataObject

class CDataObject : public IDataObject
{
  public:
	CDataObject();
	~CDataObject();
	
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	ULONG __stdcall AddRef(void);
	ULONG __stdcall Release(void);
	
	HRESULT __stdcall GetData(FORMATETC *pFormatetc, STGMEDIUM *pmedium);
	HRESULT __stdcall GetDataHere(FORMATETC *pFormatetc, STGMEDIUM *pmedium);
	HRESULT __stdcall QueryGetData(FORMATETC *pFormatetc);
	HRESULT __stdcall GetCanonicalFormatEtc(FORMATETC *pFormatetcIn, FORMATETC *pFormatetcInOut);
	HRESULT __stdcall SetData(FORMATETC *pFormatetc, STGMEDIUM *pMedium, BOOL fRelease);
	HRESULT __stdcall EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatetc);
	HRESULT __stdcall DAdvise(FORMATETC *pFormatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
	HRESULT __stdcall DUnadvise(DWORD dwConnection);
	HRESULT __stdcall EnumDAdvise(IEnumSTATDATA **ppenumAdvise);
	
  private:
	LONG	mRefCount;
	static const AItemCount	kDataCountMax = 32;
	AItemCount	mDataCount;
	FORMATETC	mFormat[kDataCountMax];
	STGMEDIUM	mMedium[kDataCountMax];
	
	static ABool	DuplicateMedium( STGMEDIUM *pdest, const FORMATETC* pFormatetc, const STGMEDIUM *pMedium );
};
