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

CPaneForAView (Windows)

*/

#include "stdafx.h"

#include "CPaneForAView.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]CPaneForAView
#pragma mark ===========================
//AView用コントロール

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
CPaneForAView::CPaneForAView( AObjectArrayItem* inParent, CWindowImp& inWindow ) : CUserPane(inParent,inWindow)
		,mShowTooltipTimerWorking(false)
		,mEnableTrackingToolTip(false)
		,mHTooltip(NULL), mTooltipActive(false)
		,mShowToolTipOnlyWhenNarrow(true)//#507
		,mFixedTextTooltip_TagSide(kHelpTagSide_Default)//#643
{
	//DoMouseMovedの移動チェック用
	mPreviousMousePoint.x = -1;
	mPreviousMousePoint.y = -1;
}

//デストラクタ
CPaneForAView::~CPaneForAView()
{
}

void	CPaneForAView::DestroyUserPane()
{
	//
	if( mHTooltip != NULL )
	{
		::DestroyWindow(mHTooltip);
		mHTooltip = NULL;
	}
	//
	CUserPane::DestroyUserPane();
}

#pragma mark ===========================

#pragma mark ---イベントハンドラ

//描画
ABool	CPaneForAView::DoDraw()
{
	GetAWin().EVT_DoDraw(GetControlID());
	return true;
}

void	CPaneForAView::DoDrawPreProcess()
{
	GetAWin().EVT_DrawPreProcess(GetControlID());
}

void	CPaneForAView::DoDrawPostProcess()
{
	GetAWin().EVT_DrawPostProcess(GetControlID());
}

//マウスホイール
ABool	CPaneForAView::DoMouseWheel( const AFloatNumber inDeltaY, const AModifierKeys inModifiers, const ALocalPoint inLocalPoint )
{
//	if( IsControlVisible() == false )   return true;
	
	//（AWindowを経由して）対応するAViewのイベントハンドラを実行 
	GetAWin().EVT_DoMouseWheel(GetControlID(),0,inDeltaY,inModifiers,inLocalPoint);
	//表示更新（WM_PAINT発生）
	::UpdateWindow(GetHWnd());
	//Tooltip制御
	if( GetEnableTrackingToolTip() == true )
	{
		//Tooltip更新
		UpdateTooltip(inLocalPoint);
	}
	return true;
}

//マウスクリックに対する処理  
ABool	CPaneForAView::DoMouseDown( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
		const ANumber inClickCount, const ABool inRightButton )
{
//付箋紙内をクリックするとここがfalseになる。いずれにしてもこの判定は不要と思われるのでコメントアウト。	if( IsControlVisible() == false )   return true;
	
	//右クリックなら、EVT_DoContextMenu()をコールしてみる 
	if( inRightButton == true )
	{
		if( GetAWin().EVT_DoContextMenu(GetControlID(),inLocalPoint,inModifiers,inClickCount) == true )
		{
			return true;
		}
	}
	//（AWindowを経由して）対応するAViewのEVT_DoMouseDown()を実行 
	if( GetAWin().EVT_DoMouseDown(GetControlID(),inLocalPoint,inModifiers,inClickCount) == false )
	{
		//
		if( inClickCount == 1 )
		{
			GetAWin().EVT_Clicked(GetControlID(),inModifiers);
		}
		else if( inClickCount == 2 )
		{
			GetAWin().EVT_Clicked(GetControlID(),inModifiers);//B0303 ウインドウアクティベートクリックが１回目になってしまい、EVT_Clicked()を呼べないことがあるので
			GetAWin().EVT_DoubleClicked(GetControlID());
		}
		//char	str[256];
		//sprintf(str,"DoMouseDown:%d \n",GetControlID());
		//OutputDebugStringA(str);
	}
	//（AWindowを経由して）対応するAViewのEVT_GetCursorType()を実行  
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(GetControlID(),inLocalPoint,inModifiers));
	//表示更新（WM_PAINT発生）
	::UpdateWindow(GetHWnd());
	return true;
}

ABool	CPaneForAView::DoMouseMoved( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers )
{
	if( IsControlVisible() == false )   return true;
	
	//移動チェック（TTF_TRACKを設定したTooltipを表示しているときは、マウスを移動していなくてもコールされる）
	if( mPreviousMousePoint.x == inLocalPoint.x && mPreviousMousePoint.y == inLocalPoint.y )
	{
		//前回とマウス位置同じなら何もせずリターン
		return true;
	}
	mPreviousMousePoint.x = inLocalPoint.x;
	mPreviousMousePoint.y = inLocalPoint.y;
	
	//MouseTracking
	if( mMouseTrackingMode == true )
	{
		GetAWin().EVT_DoMouseTracking(GetControlID(),inLocalPoint,inModifiers);
		AApplication::GetApplication().NVI_UpdateMenu();
	}
	//自UserPane以外の他の全てのWindowの全てのUserPaneにDoMouseLeft()する。（tracking中なら）
	GetWin().TryMouseLeaveForAllWindowsExceptFor(GetControlID());
	//
	ABool	result = GetAWin().EVT_DoMouseMoved(GetControlID(),inLocalPoint,inModifiers);
	//（AWindowを経由して）対応するAViewのEVT_GetCursorType()を実行 
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(GetControlID(),inLocalPoint,inModifiers));
	//
	if( mMouseLeaveEventEnable == true && mMouseLeaveEventTracking == false )
	{
		mMouseLeaveEventTracking = true;
		//
		HWND	trackHWnd = GetHWnd();
		if( GetWin().IsOverlayWindow() == true )
		{
			//OverlayWindowの場合は親ウインドウ内でtrackする。（Overlayウインドウはマウス透過なので、即leaveしてしまう）
			trackHWnd = GetWin().GetParentWindowHWnd();
		}
		//
		TRACKMOUSEEVENT	trackmouse;
		trackmouse.cbSize = sizeof(trackmouse);
		trackmouse.dwFlags = TME_LEAVE;
		trackmouse.hwndTrack = trackHWnd;
		trackmouse.dwHoverTime = HOVER_DEFAULT;
		//TrackMouseEvent()自体はすぐに抜ける。
		//次回、マウスがウインドウ外へ移動すると、一度だけ、WM_MOUSELEAVEがそのウインドウへ送信される
		//オーバーレイの場合、透明部分はウインドウ外と判定される。
		//OutputDebugStringA("BeforeTrackMouseEvent\n");
		::TrackMouseEvent(&trackmouse);
		//OutputDebugStringA("AfterTrackMouseEvent\n");
	}
	//マウスカーソル表示
	ACursorWrapper::ShowCursor();
	//表示更新（WM_PAINT発生）
	::UpdateWindow(GetHWnd());
	//Tooltip制御
	if( GetEnableTrackingToolTip() == true )
	{
		//Tooltip更新
		UpdateTooltip(inLocalPoint);
	}
	return result;
}

void	CPaneForAView::DoMouseLeft()
{
	if( IsControlVisible() == false )   return;
	ALocalPoint	localPoint = {0,0};
	//
	GetAWin().EVT_DoMouseLeft(GetControlID(),localPoint);
	//Tooltip制御
	if( GetEnableTrackingToolTip() == true )
	{
		//Tooltipをdeactivateする
		HideTooltip();
	}
	//移動チェックメンバー変数クリア
	mPreviousMousePoint.x = -1;
	mPreviousMousePoint.y = -1;
	//
	mMouseLeaveEventTracking = false;
}

void	CPaneForAView::UpdateTooltip( const ALocalPoint inLocalPoint )
{
	//テキスト取得
	ABool	show = true;
	AText	text;
	AText	text1, text2;
	ALocalRect	localRect;
	AHelpTagSide	tagSide = kHelpTagSide_Default;//#643
	if( mFixedTextTooltip.GetItemCount() > 0 )
	{
		text.SetText(mFixedTextTooltip);
		text2.SetText(text);
		GetControlLocalFrameRect(localRect);
	}
	else
	{
		GetAWin().EVT_DoGetHelpTag(GetControlID(),inLocalPoint,text1,text2,localRect,tagSide);
		text.SetText(text2);
		if( mShowToolTipOnlyWhenNarrow == true )//#507
		{
			ANumber	width = static_cast<ANumber>(GetDrawTextWidth(text));
			if( width < localRect.right-localRect.left )
			{
				show = false;
			}
		}
	}
	if( show == false )
	{
		HideTooltip();
	}
	else
	{
		//
		if( mCurrentTooltipText.Compare(text) == false ||
					mCurrentTooltipRect.left != localRect.left ||
					mCurrentTooltipRect.right != localRect.right ||
					mCurrentTooltipRect.top != localRect.top ||
					mCurrentTooltipRect.bottom != localRect.bottom )
		{
			//ツールチップウインドウ生成
			if( mHTooltip == NULL )
			{
				mHTooltip = ::CreateWindowEx(0,TOOLTIPS_CLASS,NULL,TTS_ALWAYSTIP|TTS_NOPREFIX,
						CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
						GetHWnd(),NULL,CAppImp::GetHInstance(),NULL);
				//Tooltip設定
				//TTF_TRACK: Trackingモード
				//TTF_ABSOLUTE: 設定しない場合コントロール外で表示
				RECT	rect;
				::GetClientRect(GetHWnd(),&rect);
				mTooltipToolinfo.cbSize = sizeof(mTooltipToolinfo);
				mTooltipToolinfo.hwnd		= GetHWnd();
				mTooltipToolinfo.rect		= rect;
				mTooltipToolinfo.hinst		= CAppImp::GetHResourceInstance();//satDLL
				mTooltipToolinfo.uFlags	= TTF_IDISHWND|TTF_TRACK|TTF_ABSOLUTE;
				mTooltipToolinfo.uId		= (UINT_PTR)GetHWnd();
				mTooltipToolinfo.lpszText	= L"";
				::SendMessageW(mHTooltip,TTM_ADDTOOLW,0,(LPARAM)&mTooltipToolinfo);
				mTooltipActive = false;
			}
			
			//更新チェック用データ設定
			mCurrentTooltipText.SetText(text);
			mCurrentTooltipRect = localRect;
			//まず非表示にする（タイマーで表示させるため）
			::SendMessageW(mHTooltip,TTM_TRACKACTIVATE,(WPARAM)FALSE,(LPARAM)&mTooltipToolinfo);
			//表示位置取得
			//POINT	pt = {localRect.left,localRect.bottom+3};
			POINT	pt = { inLocalPoint.x+32, localRect.bottom+3};
			//#643
			switch(tagSide)
			{
			  case kHelpTagSide_Right:
				{
					pt.x = localRect.right + 3;
					pt.y = localRect.bottom + 3;
					break;
				}
			}
			//
			::ClientToScreen(GetHWnd(),&pt);
			//最大幅設定
			AGlobalRect	screenBounds;
			CWindowImp::GetAvailableWindowPositioningBounds(screenBounds);
			::SendMessageW(mHTooltip,TTM_SETMAXTIPWIDTH,(WPARAM)0,
					(LPARAM)screenBounds.right-pt.x);
			//フォント設定（UserPaneのデフォルトフォントを設定）
			if( mFixedTextTooltip.GetItemCount() == 0 )//固定文字列ツールチップのときはシステムデフォルトのフォントを使用する
			{
				::SendMessageW(mHTooltip,WM_SETFONT,(WPARAM)GetHFont(0),(LPARAM)FALSE);
			}
			//Tooltipテキスト更新
			AStCreateWcharStringFromAText	str(text2);
			mTooltipToolinfo.lpszText = (wchar_t*)str.GetPtr();
			::SendMessageW(mHTooltip,TTM_SETTOOLINFO,(WPARAM)0,(LPARAM)&mTooltipToolinfo);
			mTooltipToolinfo.lpszText = L"";
			//Tooltip位置設定
			::SendMessageW(mHTooltip,TTM_TRACKPOSITION,0,(LPARAM)MAKELONG(pt.x,pt.y));
			//表示タイマー起動
			::SetTimer(GetHWnd(),kTimer_ShowTooltip,750,NULL);
			mShowTooltipTimerWorking = true;
		}
	}
}

void	CPaneForAView::ShowTooltip()
{
	//if( mTooltipActive == true )   return;
	
	mTooltipToolinfo.lpszText = L"";//念のため
	::SendMessageW(mHTooltip,TTM_TRACKACTIVATE,(WPARAM)TRUE,(LPARAM)&mTooltipToolinfo);
	mTooltipActive = true;
}

void	CPaneForAView::HideTooltip()
{
	//if( mTooltipActive == false )   return;
	
	mTooltipToolinfo.lpszText = L"";//念のため
	::SendMessageW(mHTooltip,TTM_TRACKACTIVATE,(WPARAM)FALSE,(LPARAM)&mTooltipToolinfo);
	mTooltipActive = false;
	//表示タイマー解除
	if( mShowTooltipTimerWorking == true )
	{
		//タイマー削除
		::KillTimer(GetHWnd(),kTimer_ShowTooltip);
		mShowTooltipTimerWorking = false;
	}
	//更新チェック用データクリア
	mCurrentTooltipText.DeleteAll();
	mCurrentTooltipRect.left	= 0;
	mCurrentTooltipRect.top		= 0;
	mCurrentTooltipRect.right	= 0;
	mCurrentTooltipRect.bottom	= 0;
	//ツールチップウインドウ削除
	if( mHTooltip != NULL )
	{
		::DestroyWindow(mHTooltip);
		mHTooltip = NULL;
	}
}

//マウスクリックに対する処理  
ABool	CPaneForAView::DoMouseUp( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
		const ANumber inClickCount, const ABool inRightButton )
{
	if( IsControlVisible() == false )   return true;
	
	//MouseTracking
	if( mMouseTrackingMode == true )
	{
		GetAWin().EVT_DoMouseTrackEnd(GetControlID(),inLocalPoint,inModifiers);
	}
	//表示更新（WM_PAINT発生）
	::UpdateWindow(GetHWnd());
	return true;
}

void	CPaneForAView::DoControlBoundsChanged()
{
	GetAWin().EVT_DoControlBoundsChanged(GetControlID());
}

ABool	CPaneForAView::DragEnter( IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	POINT	pt;
	pt.x = ptl.x;
	pt.y = ptl.y;
	::ScreenToClient(GetHWnd(),&pt);
	ALocalPoint	localPoint;
	localPoint.x = pt.x;
	localPoint.y = pt.y;
	AModifierKeys	modifiers = 0;
	if( (grfKeyState&MK_SHIFT)==0 && (grfKeyState&MK_CONTROL)==0 && ((*pdwEffect)&DROPEFFECT_MOVE)!= 0 )
	{
		*pdwEffect = DROPEFFECT_MOVE;
	}
	else if( (grfKeyState&MK_SHIFT) != 0 )
	{
		modifiers |= kModifierKeysMask_Shift;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else if( (grfKeyState&MK_CONTROL) != 0 )
	{
		modifiers |= kModifierKeysMask_Control;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else
	{
		*pdwEffect = DROPEFFECT_COPY;
	}
	GetAWin().EVT_DoDragEnter(GetControlID(),NULL,localPoint,modifiers);
	return true;
}

ABool	CPaneForAView::DragOver( DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	POINT	pt;
	pt.x = ptl.x;
	pt.y = ptl.y;
	::ScreenToClient(GetHWnd(),&pt);
	ALocalPoint	localPoint;
	localPoint.x = pt.x;
	localPoint.y = pt.y;
	AModifierKeys	modifiers = 0;
	if( (grfKeyState&MK_SHIFT)==0 && (grfKeyState&MK_CONTROL)==0 && ((*pdwEffect)&DROPEFFECT_MOVE)!= 0 )
	{
		*pdwEffect = DROPEFFECT_MOVE;
	}
	else if( (grfKeyState&MK_SHIFT) != 0 )
	{
		modifiers |= kModifierKeysMask_Shift;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else if( (grfKeyState&MK_CONTROL) != 0 )
	{
		modifiers |= kModifierKeysMask_Control;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else
	{
		*pdwEffect = DROPEFFECT_COPY;
	}
	GetAWin().EVT_DoDragTracking(GetControlID(),NULL,localPoint,modifiers);
	return true;
}

ABool	CPaneForAView::DragLeave()
{
	ALocalPoint	localPoint;
	localPoint.x = 0;
	localPoint.y = 0;
	AModifierKeys	modifiers = 0;
	GetAWin().EVT_DoDragLeave(GetControlID(),NULL,localPoint,modifiers);
	return true;
}

ABool	CPaneForAView::Drop( IDataObject* pDataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect )
{
	ADragRef	dummy = 0;
	POINT	pt;
	pt.x = ptl.x;
	pt.y = ptl.y;
	::ScreenToClient(GetHWnd(),&pt);
	ALocalPoint	localPoint;
	localPoint.x = pt.x;
	localPoint.y = pt.y;
	AModifierKeys	modifiers = 0;
	if( (grfKeyState&MK_SHIFT)==0 && (grfKeyState&MK_CONTROL)==0 && ((*pdwEffect)&DROPEFFECT_MOVE)!= 0 )
	{
		*pdwEffect = DROPEFFECT_MOVE;
	}
	else if( (grfKeyState&MK_SHIFT) != 0 )
	{
		modifiers |= kModifierKeysMask_Shift;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else if( (grfKeyState&MK_CONTROL) != 0 )
	{
		modifiers |= kModifierKeysMask_Control;
		*pdwEffect = DROPEFFECT_COPY;
	}
	else
	{
		*pdwEffect = DROPEFFECT_COPY;
	}
	GetAWin().EVT_DoDragReceive(GetControlID(),pDataObject,localPoint,modifiers);
	return true;
}

#pragma mark ===========================

#pragma mark ---Tooltip

//#507
/**
*/
void	CPaneForAView::EnableTrackingToopTip( const ABool inShowToolTipOnlyWhenNarrow )
{
	mEnableTrackingToolTip = true;
	mShowToolTipOnlyWhenNarrow = inShowToolTipOnlyWhenNarrow;
}

void	CPaneForAView::SetFixedTextTooltip( const AText& inText, const AHelpTagSide inTagSide )//#643
{
	EnableTrackingToopTip();
	mFixedTextTooltip.SetText(inText);
	mFixedTextTooltip_TagSide = inTagSide;//#643
}

#pragma mark ===========================

#pragma mark ---コントロールフォーカス処理

//#135
/**
フォーカス設定時にCWindowImpからコールされる
*/
void	CPaneForAView::SetFocus()
{
	GetAWin().EVT_DoViewFocusActivated(GetControlID());
}

//#135
/**
フォーカス解除
*/
void	CPaneForAView::ResetFocus()
{
	GetAWin().EVT_DoViewFocusDeactivated(GetControlID());
}

