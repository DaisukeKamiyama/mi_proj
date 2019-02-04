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

AView

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AView
#pragma mark ===========================
//View

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView::AView( const AWindowID inWindowID, const AControlID inID, const AImagePoint& inOriginOfLocalFrame ) 
		: AObjectArrayItem(/*#199 inParent*/&(AApplication::GetApplication())), /*#199mWindow(inWindow),*/
		mWindowID(inWindowID), mControlID(inID), 
		mControlID_VScrollBar(kControlID_Invalid), mControlID_HScrollBar(kControlID_Invalid),
		mImageWidth(100), mImageHeight(100), mHScrollBarUnit(1), mVScrollBarUnit(1), mHScrollBarPageUnit(1), mVScrollBarPageUnit(1),
		/*#688 mFocusActive(false),*/ mOriginOfLocalFrame(inOriginOfLocalFrame)
		,mMouseWheelEventRedirectViewID(kObjectID_Invalid)
		,mBorrowerView(false)//#92
		,mAutomaticSelectBySwitchFocus(false)//#135
		,mCatchReturn(false)//#137
		,mCatchTab(false)//#137
		,mInputBackslashByYenKeyMode(false)//#182
		,mDataType(kDataType_Unknown)//#182
		,mViewImp(AApplication::GetApplication().NVI_GetWindowByID(inWindowID).NVI_GetViewImp(inID))//#271
		,mWindow(AApplication::GetApplication().NVI_GetWindowByID(inWindowID))//#271
		,mFrameViewControlID(kControlID_Invalid), mFrameLeftMargin(0), mFrameRightMargin(0), mFrameTopMargin(0), mFrameBottomMargin(0)//#135
		,mVScrollBarControlID(kControlID_Invalid),mHScrollBarControlID(kControlID_Invalid)//#135
		,mVScrollBarWidth(15), mHScrollBarHeight(15)
		,mMouseTrackingMode(false)//#688
,mImageYMinMargin(0), mImageYMaxMargin(0)//#621
,mDefaultCursor(kCursorType_Arrow)//#725
,mBackgroundTransparency(1.0), mBackgroundActiveColor(kColor_White), mBackgroundDeactiveColor(kColor_White)//#725
,mEnableHelpTag(true)
,mRefreshing(false)//#1034
,mContextMenuItemID(0)//#1380
{
}

//デストラクタ
AView::~AView()
{
}

//#92
/**
AObjectArrayItemをAObjectArrayから削除した直後にコールされる。
（デストラクタはGC時なので、タイミング制御できない。よって、各種削除処理はDoDeleted()で行うべき。）
*/
void	AView::DoDeleted()
{
	//FrameView削除 #135
	if( ExistFrameView() == true )
	{
		DeleteFrameView();
	}
	//
	NVIDO_DoDeleted();
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
*/
ABool	AView::EVT_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	//#390
	if( AWindow::STATIC_NVI_GetTextInputRedirect_WindowID() != kObjectID_Invalid && 
				AWindow::STATIC_NVI_GetTextInputRedirect_WindowID() != mWindowID )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(AWindow::STATIC_NVI_GetTextInputRedirect_WindowID()).
		NVI_GetViewByControlID(AWindow::STATIC_NVI_GetTextInputRedirect_ControlID()).
		EVT_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys);
	}
	//
	return EVTDO_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys);
}

/**
*/
void	AView::EVT_UpdateMenu()
{
	//#688 終了処理中にUpdateMenuしないようにする
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return;
	
	//#390
	if( AWindow::STATIC_NVI_GetTextInputRedirect_WindowID() != kObjectID_Invalid && 
				AWindow::STATIC_NVI_GetTextInputRedirect_WindowID() != mWindowID )
	{
		AApplication::GetApplication().NVI_GetWindowByID(AWindow::STATIC_NVI_GetTextInputRedirect_WindowID()).
				NVI_GetViewByControlID(AWindow::STATIC_NVI_GetTextInputRedirect_ControlID()).EVT_UpdateMenu();
		return;
	}
	//
	EVTDO_UpdateMenu();
}

/**
描画
非表示中はDoDraw()実行しない。
*/
void	AView::EVT_DoDraw()
{
	//#1034
	//リフレッシュ中（draw待ち）フラグOFF
	//NVMC_RefreshRect(), NVMC_RefreshControl()でONにされる。キャレット描画時に、context flushの要否を判断するため使用する。
	mRefreshing = false;
	//win 非表示中でもOffscreenへの描画可能にする if( NVI_IsVisible() == false )   return;//#0 非表示中はdrawする必要がない
	EVTDO_DrawPreProcess();
	EVTDO_DoDraw();
	NVMC_TransferOffscreen();
	EVTDO_DrawPostProcess();
}

//スクロールバー操作
void	AView::EVT_DoScrollBarAction( const ABool inV, const AScrollBarPart inPart )
{
	if( inV )
	{
		switch(inPart)
		{
		  case kScrollBarPart_UpButton:
			{
				if( mViewImp.GetVerticalMode() == false )
				{
					NVI_Scroll(0,-NVI_GetVScrollBarUnit());
				}
				else
				{
					//縦書きの場合は逆方向 #558
					NVI_Scroll(0,NVI_GetVScrollBarUnit());
				}
				break;
			}
		  case kScrollBarPart_DownButton:
			{
				if( mViewImp.GetVerticalMode() == false )
				{
					NVI_Scroll(0,NVI_GetVScrollBarUnit());
				}
				else
				{
					//縦書きの場合は逆方向 #558
					NVI_Scroll(0,-NVI_GetVScrollBarUnit());
				}
				break;
			}
		  case kScrollBarPart_PageUpButton:
			{
				if( mViewImp.GetVerticalMode() == false )
				{
					NVI_Scroll(0,-NVI_GetVScrollBarPageUnit());
				}
				else
				{
					//縦書きの場合は逆方向 #558
					NVI_Scroll(0,NVI_GetVScrollBarPageUnit());
				}
				break;
			}
		  case kScrollBarPart_PageDownButton:
			{
				if( mViewImp.GetVerticalMode() == false )
				{
					NVI_Scroll(0,NVI_GetVScrollBarPageUnit());
				}
				else
				{
					//縦書きの場合は逆方向 #558
					NVI_Scroll(0,-NVI_GetVScrollBarPageUnit());
				}
				break;
			}
		  case kScrollBarPart_ThumbTracking:
			{
				AImagePoint	pt;
				NVI_GetOriginOfLocalFrame(pt);
				if( mViewImp.GetVerticalMode() == false )
				{
					pt.y = NVM_GetWindow().NVI_GetControlNumber(mControlID_VScrollBar);
				}
				else
				{
					//縦書きの場合は逆方向 #558
					pt.y = mImageHeight-NVI_GetViewHeight()-NVM_GetWindow().NVI_GetControlNumber(mControlID_VScrollBar);
				}
				NVI_ScrollTo(pt,true,true,kScrollTrigger_ScrollBar);//#1031
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	else
	{
		switch(inPart)
		{
		  case kScrollBarPart_UpButton:
			{
				NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
				break;
			}
		  case kScrollBarPart_DownButton:
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				break;
			}
		  case kScrollBarPart_PageUpButton:
			{
				NVI_Scroll(-NVI_GetHScrollBarPageUnit(),0);
				break;
			}
		  case kScrollBarPart_PageDownButton:
			{
				NVI_Scroll(NVI_GetHScrollBarPageUnit(),0);
				break;
			}
		  case kScrollBarPart_ThumbTracking:
			{
				AImagePoint	pt;
				NVI_GetOriginOfLocalFrame(pt);
				pt.x = NVM_GetWindow().NVI_GetControlNumber(mControlID_HScrollBar);
				NVI_ScrollTo(pt,true,true,kScrollTrigger_ScrollBar);//#1031
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
}

//
void	AView::EVT_DoControlBoundsChanged()
{
	//
	mHScrollBarPageUnit = NVI_GetViewWidth()  - NVI_GetHScrollBarUnit();
	mVScrollBarPageUnit = NVI_GetViewHeight() - NVI_GetVScrollBarUnit();
	//
	EVTDO_DoControlBoundsChanged();
}

/**
マウスWheel時のOSイベントコールバック
*/
void	AView::EVT_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
		const ALocalPoint inLocalPoint )
{
	//イベントRedirectが指定されている場合は、Redirect先ViewのEVT_メソッドをコールする
	if( mMouseWheelEventRedirectViewID != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetViewByID(mMouseWheelEventRedirectViewID).
		EVT_DoMouseWheel(inDeltaX,inDeltaY,inModifierKeys,inLocalPoint);
		return;
	}
	//
	EVTDO_DoMouseWheel(inDeltaX,inDeltaY,inModifierKeys,inLocalPoint);
}

//#1380
/**
右クリックメニュークリック時のデフォルト処理
*/
ABool	AView::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( mContextMenuItemID > 0 )
	{
		AImagePoint	clickImagePoint = {0};
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		if( NVM_IsImagePointInViewRect(clickImagePoint) == true )
		{
			AGlobalPoint	globalPoint = {0};
			NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
			NVMC_ShowContextMenu(mContextMenuItemID,globalPoint);
		}
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---表示制御
//#137

/**
Show/Hide
*/
void	AView::NVI_SetShow( const ABool inShow )
{
	NVMC_SetShowControl(inShow);
	NVIDO_SetShow(inShow);
	//FrameViewのShow/Hideを設定 #135
	if( ExistFrameView() == true )
	{
		GetFrameView().NVI_SetShow(inShow);
	}
}

/**
Enable/Disable
*/
void	AView::NVI_SetEnabled( const ABool inEnable )
{
	//#530
	if( NVMC_IsControlEnabled() == inEnable )   return;
	//
	NVMC_SetEnable(inEnable);
	NVIDO_SetEnabled(inEnable);
	//FrameViewのEnable/Disableを設定
	if( ExistFrameView() == true )
	{
		GetFrameView().NVI_SetEnabled(inEnable);
	}
	NVMC_RefreshControl();//win
}

#pragma mark ===========================

#pragma mark ---スクロール

//スクロール
void	AView::NVI_Scroll( const ANumber inDeltaX, const ANumber inDeltaY, const ABool inRedraw, const ABool inConstrainToImageSize,
						   const AScrollTrigger inScrollTrigger )//#1031
{
	if( inDeltaX == 0 && inDeltaY == 0 )   return;
	ScrollCore(inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize,inScrollTrigger);//#1031
}

void	AView::ScrollCore( const ANumber inDeltaX, const ANumber inDeltaY, const ABool inRedraw, const ABool inConstrainToImageSize,
						   const AScrollTrigger inScrollTrigger )//#1031
{
	//fprintf(stderr,"%16X ScrollCore() - Start\n",(int)(GetObjectID().val));
	
	if( inRedraw == true )
	{
		//#688
		//スクロール前に、今までの描画すべき領域を、今すぐ描画(EVT_DoDraw())する
		NVMC_ExecuteDoDrawImmediately();
	}
	
	//スクロール前処理
	NVIDO_ScrollPreProcess(inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize,inScrollTrigger);//#138 #1031
	
	AImagePoint	newOrigin = mOriginOfLocalFrame;
	newOrigin.x += inDeltaX;
	if( inConstrainToImageSize == true )
	{
		//イメージサイズ外にスクロールしないように補正
		if( newOrigin.x > mImageWidth - NVI_GetViewWidth() )//#558 NVM_GetWindowConst().NVI_GetControlWidth(mControlID)→NVI_GetViewWidth()
		{
			newOrigin.x = mImageWidth - NVI_GetViewWidth();//#558 NVM_GetWindowConst().NVI_GetControlWidth(mControlID)→NVI_GetViewWidth()
		}
		if( newOrigin.x < 0 )
		{
			newOrigin.x = 0;
		}
	}
	newOrigin.y += inDeltaY;
	if( inConstrainToImageSize == true )
	{
		//イメージサイズ外にスクロールしないように補正
		if( newOrigin.y > mImageHeight + mImageYMaxMargin - NVI_GetViewHeight() )//#621 #558 NVM_GetWindowConst().NVI_GetControlHeight(mControlID)→NVI_GetViewHeight()
		{
			newOrigin.y = mImageHeight + mImageYMaxMargin - NVI_GetViewHeight();//#621 #558 NVM_GetWindowConst().NVI_GetControlHeight(mControlID)→NVI_GetViewHeight()
		}
		if( newOrigin.y < -mImageYMinMargin )//#621
		{
			newOrigin.y = -mImageYMinMargin;//#621
		}
	}
	ANumber	deltaX = newOrigin.x - mOriginOfLocalFrame.x;
	ANumber	deltaY = newOrigin.y - mOriginOfLocalFrame.y;
	mOriginOfLocalFrame = newOrigin;
	if( inRedraw == true )
	{
		NVMC_Scroll(-deltaX,-deltaY);
	}
	
	//スクロールバーの現在値設定
	if( mControlID_HScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetControlNumber(mControlID_HScrollBar,mOriginOfLocalFrame.x);
	}
	if( mControlID_VScrollBar != kControlID_Invalid )
	{
		if( mViewImp.GetVerticalMode() == false )
		{
			NVM_GetWindow().NVI_SetControlNumber(mControlID_VScrollBar,mOriginOfLocalFrame.y);
		}
		else
		{
			//縦書きモード時は右側を0にする #558
			NVM_GetWindow().NVI_SetControlNumber(mControlID_VScrollBar,mImageHeight-NVI_GetViewHeight()-mOriginOfLocalFrame.y);
		}
	}
	
	//AWindowオブジェクトへViewがスクロールされたことを伝える
	//#1031 NVIDO_ScrollPostProcess()へ移動 NVM_GetWindow().OWICB_ViewScrolled(mControlID,inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize);
	
	//スクロール後処理
	NVIDO_ScrollPostProcess(inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize,inScrollTrigger);//#138 #1031
	
	//fprintf(stderr,"%16X ScrollCore() - End\n",(int)(GetObjectID().val));
}

//スクロール
void	AView::NVI_ScrollTo( const AImagePoint& inNewOrigin, const ABool inRedraw, const ABool inConstrainToImageSize,
							 const AScrollTrigger inScrollTrigger )//#1031
{
	NVI_Scroll(inNewOrigin.x-mOriginOfLocalFrame.x,inNewOrigin.y-mOriginOfLocalFrame.y,inRedraw,inConstrainToImageSize,inScrollTrigger);//#1031
}

/**
スクロール位置をImage sizeの範囲外になっていたら、調整
*/
void	AView::NVI_ConstrainScrollToImageSize( const ABool inRedraw )
{
	ScrollCore(0,0,inRedraw,true);
}

void	AView::NVI_SetScrollBarControlID( const AControlID inHScrollBar, const AControlID inVScrollBar )
{
	if( mViewImp.GetVerticalMode() == false )
	{
		mControlID_HScrollBar = inHScrollBar;
		mControlID_VScrollBar = inVScrollBar;
	}
	else
	{
		//縦書きモード時はH,V入れ替え #558
		mControlID_HScrollBar = inVScrollBar;
		mControlID_VScrollBar = inHScrollBar;
	}
	//Scrollbar値表示更新 #558
	NVI_UpdateScrollBar();
}

/**
スクロールバーunit設定
*/
void	AView::NVI_SetScrollBarUnit( const ANumber inHScrollUnit, const ANumber inVScrollUnit, 
		const ANumber inHScrollPageUnit, const ANumber inVScrollPageUnit )
{
	mHScrollBarUnit = inHScrollUnit;
	mVScrollBarUnit = inVScrollUnit;
	mHScrollBarPageUnit = inHScrollPageUnit;
	mVScrollBarPageUnit = inVScrollPageUnit;
	//#725
	//page unit設定
	if( mControlID_VScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetScrollBarPageUnit(mControlID_VScrollBar,mVScrollBarPageUnit);
	}
	if( mControlID_HScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetScrollBarPageUnit(mControlID_HScrollBar,mHScrollBarPageUnit);
	}
}


#pragma mark ===========================

#pragma mark ---フォーカス制御

/**
FocusがActivateされたときにコールされる
*/
void	AView::EVT_DoViewFocusActivated()
{
	//フォーカス設定
	//#688 mFocusActive = true;
	EVTDO_DoViewFocusActivated();
	//FrameViewのFocus描画用状態を設定 #135
	if( ExistFrameView() == true )
	{
		GetFrameView().SPI_SetFocused(true);
	}
	//#688
	//mAutomaticSelectBySwitchFocusがtrueなら、フォーカス状態になったときに全選択する
	//（Cocoa対応でtabキーによるfocus制御はOS任せにしたので、ここで捕まえる必要がある。）
	//#688 CWindowImp::APICB_CocoaViewBecomeFirstResponder()にて、タブキーによる移動の場合のみ実行するようにした。 NVI_AutomaticSelectBySwitchFocus();
}

/**
FocusがDeactivateされたときにコールされる
*/
void	AView::EVT_DoViewFocusDeactivated()
{
	//#688 mFocusActive = false;
	EVTDO_DoViewFocusDeactivated();
	//FrameViewのFocus描画用状態を設定 #135
	if( ExistFrameView() == true )
	{
		GetFrameView().SPI_SetFocused(false);
	}
}

//#312
/**
ウインドウのlatentなfocusかどうかを返す
*/
ABool	AView::NVI_IsLatentFocus() const
{
	return (NVI_GetControlID() == NVM_GetWindowConst().NVI_GetLatentFocus());
}

/**
ビューがFocusされているかどうかを返す
*/
ABool	AView::NVI_IsFocusActive() const
{
	return (/*#688 mFocusActive*/NVI_GetControlID() == NVM_GetWindowConst().NVI_GetCurrentFocus() &&
		NVM_GetWindowConst().NVI_IsWindowFocusActive());
}

//#312
/**
タブ内のlatentなfocusかどうかを返す
*/
ABool	AView::NVI_IsFocusInTab() const
{
	return NVM_GetWindowConst().NVI_IsFocusInTab(NVI_GetControlID());
}

#pragma mark ===========================

#pragma mark ---位置・サイズ

/**
位置設定（付属処理も行う。通常はNVMC_SetPosition()ではなくこちらを呼び出す。）
*/
void	AView::NVI_SetPosition( const AWindowPoint& inWindowPoint )
{
	if( ExistFrameView() == false )
	{
		//FrameViewが存在しない場合
		//位置設定
		NVMC_SetControlPosition(inWindowPoint);//#182
	}
	else
	{
		//FrameViewが存在する場合
		/*
		//Frame分だけ位置をずらす
		AWindowPoint	insetPoint;
		insetPoint.x = inWindowPoint.x + mFrameLeftMargin;
		insetPoint.y = inWindowPoint.y + mFrameTopMargin;
		NVMC_SetControlPosition(insetPoint);
		//Frameサイズ変更
		UpdateFrameViewBounds();
		*/
		//このview自体はframe viewの両端にbindするようにする
		NVMC_SetControlBindings(true,true,true,true,false,false);
		//frame viewのサイズ取得
		ANumber	frameWidth = NVI_GetWidth() + mFrameLeftMargin + mFrameRightMargin;
		ANumber	frameHeight = NVI_GetHeight() + mFrameTopMargin + mFrameBottomMargin;
		//Frameの位置・サイズ変更
		GetFrameView().SPI_SetRealFramePositionAndSize(inWindowPoint,frameWidth,frameHeight);
	}
	//派生クラス処理
	NVIDO_SetPosition(inWindowPoint);
}

/**
サイズ設定（付属処理も行う。通常はNVI_SetSize()ではなくこちらを呼び出す。）
*/
void	AView::NVI_SetSize( const ANumber inWidth, const ANumber inHeight )
{
	if( ExistFrameView() == false )
	{
		//FrameViewが存在しない場合
		//サイズ設定
		NVMC_SetControlSize(inWidth,inHeight);//#182
	}
	else
	{
		//FrameViewが存在する場合
		/*
		//Frame分だけサイズを小さくする
		ANumber	insetWidth = inWidth - mFrameLeftMargin - mFrameRightMargin;
		ANumber	insetHeight = inHeight - mFrameTopMargin - mFrameBottomMargin;
		NVMC_SetControlSize(insetWidth,insetHeight);
		//Frameサイズ変更
		UpdateFrameViewBounds();
		*/
		//このview自体はframe viewの両端にbindするようにする
		NVMC_SetControlBindings(true,true,true,true,false,false);
		//frame viewの位置取得
		AWindowPoint	editBoxPoint = NVI_GetPosition();
		AWindowPoint	framePoint = {0};
		framePoint.x = editBoxPoint.x - mFrameLeftMargin;
		framePoint.y = editBoxPoint.y - mFrameTopMargin;
		//Frameの位置・サイズ変更
		GetFrameView().SPI_SetRealFramePositionAndSize(framePoint,inWidth,inHeight);
		//このview自体の位置設定
		NVMC_SetControlSize(inWidth - mFrameLeftMargin - mFrameRightMargin,inHeight - mFrameTopMargin - mFrameBottomMargin);
	}
	//派生クラス処理
	NVIDO_SetSize(inWidth,inHeight);
	//スクロールバー最大最小現在値更新#688
	NVI_UpdateScrollBar();
}

//位置取得
AWindowPoint	AView::NVI_GetPosition() const
{
	AWindowPoint	wpt;
	NVM_GetWindowConst().NVI_GetControlPosition(NVI_GetControlID(),wpt);
	return wpt;
}

//幅取得
ANumber	AView::NVI_GetWidth() const
{
	return NVM_GetWindowConst().NVI_GetControlWidth(NVI_GetControlID());
}

//高さ取得
ANumber	AView::NVI_GetHeight() const
{
	return NVM_GetWindowConst().NVI_GetControlHeight(NVI_GetControlID());
}

//#455
/**
*/
void	AView::NVI_SetControlBindings( const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
		const ABool inWidthFixed, const ABool inHeightFixed )
{
	if( ExistFrameView() == true )
	{
		//Frame viewの方にBind設定する
		GetFrameView().NVMC_SetControlBindings(inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
		//自身はフレームビューの左右上下にbindingする。（サイズはフレキシブル）
		NVMC_SetControlBindings(true,true,true,true,false,false);
	}
	else
	{
		NVMC_SetControlBindings(inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
	}
}

#pragma mark ===========================

#pragma mark ---

void	AView::NVI_Refresh()
{
	if( NVMC_IsControlVisible() == true )
	{
		NVMC_RefreshControl();
	}
}

ABool	AView::NVI_IsVisible() const
{
	return NVMC_IsControlVisible();
}

//#92
/**
Viewを最前面に持ってくる
*/
void	AView::NVI_RevealView( const ABool inSelectWindow, const ABool inSwitchFocus )//#291 #492
{
	//ウインドウを選択
	if( inSelectWindow == true )//#291
	{
		NVM_GetWindow().NVI_SelectWindow();
	}
	//Tabを選択
	if( NVI_IsVisible() == false )//#212 このViewを非表示中ならTab選択
	{
		NVM_GetWindow().NVI_SelectTabByIncludingControlID(NVI_GetControlID());
	}
	//フォーカス設定 #313
	if( inSwitchFocus == true )//#492
	{
		NVM_GetWindow().NVI_SwitchFocusTo(NVI_GetControlID());
	}
}

/**
このViewを包含するWindowをTryCloseする（BorrowerViewでない場合のみ）
*/
void	AView::NVI_TryCloseWindow()
{
	if( mBorrowerView == false )
	{
		NVM_GetWindow().NVI_TryClose();
	}
}

/**
このViewを包含するWindowをExecuteCloseする（BorrowerViewでない場合のみ）
*/
void	AView::NVI_ExecuteCloseWindow()
{
	if( mBorrowerView == false )
	{
		NVM_GetWindow().NVI_ExecuteClose();
	}
}

/**
このViewを包含するWindowを閉じるのを禁止する（BorrowerViewでない場合のみ）
*/
void	AView::NVI_SetInhibitCloseWindow( const ABool inInhibit )
{
	if( mBorrowerView == false )
	{
		NVM_GetWindow().NVI_SetInhibitClose(inInhibit);
	}
}

/**
このViewを包含するWindowのタイトルバーを更新する（BorrowerViewでない場合のみ）
*/
void	AView::NVI_UpdateWindowTitleBar()
{
	NVM_GetWindow().NVI_UpdateTitleBar();
}

//#717
/**
Frame Rectをglobal座標で取得
*/
void	AView::NVI_GetGlobalViewRect( AGlobalRect& outGlobalRect ) const
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	NVM_GetWindowConst().NVI_GetGlobalRectFromControlLocalRect(NVI_GetControlID(),frameRect,outGlobalRect);
}

#pragma mark ===========================

#pragma mark <汎化メソッド>

#pragma mark ===========================

#pragma mark ---Imageサイズ

//Imageサイズ設定
void	AView::NVM_SetImageSize( const ANumber inWidth, const ANumber inHeight )
{
	//変化無しなら何もせず終了
	if( mImageWidth == inWidth && mImageHeight == inHeight )
	{
		return;
	}
	//
	mImageWidth = inWidth;
	mImageHeight = inHeight;
	
	//#519 メソッド化
	NVI_UpdateScrollBar();
}

//#519
/**
スクロールバーの最大最小・現在値設定
*/
void	AView::NVI_UpdateScrollBar()
{
	//スクロールバーの最大最小値設定
	if( mControlID_VScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetMinMax(mControlID_VScrollBar,0,mImageHeight-NVI_GetViewHeight());//#558 NVM_GetWindowConst().NVI_GetControlHeight(mControlID)→NVI_GetViewHeight()
	}
	if( mControlID_HScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetMinMax(mControlID_HScrollBar,0,mImageWidth-NVI_GetViewWidth());//#558 NVM_GetWindowConst().NVI_GetControlWidth(mControlID)→NVI_GetViewWidth()
	}
	//スクロールバーの現在値設定
	if( mControlID_HScrollBar != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_SetControlNumber(mControlID_HScrollBar,mOriginOfLocalFrame.x);
	}
	if( mControlID_VScrollBar != kControlID_Invalid )
	{
		if( mViewImp.GetVerticalMode() == false )
		{
			NVM_GetWindow().NVI_SetControlNumber(mControlID_VScrollBar,mOriginOfLocalFrame.y);
		}
		else
		{
			//縦書きモード時は右側を0にする #558
			NVM_GetWindow().NVI_SetControlNumber(mControlID_VScrollBar,mImageHeight-NVI_GetViewHeight()-mOriginOfLocalFrame.y);
		}
	}
}

//#688
/**
リサイズ中かどうかを判定
*/
ABool	AView::NVI_IsInLiveResize() const
{
	return ((mViewImp.IsInLiveResize()==true)||(NVM_GetWindowConst().NVI_GetVirtualLiveResizing()==true));
}

#pragma mark ===========================

#pragma mark ---イベントRedirect

//#135
/**
MouseWheelイベントのRedirect先を設定する

MouseWheelイベントは、ここで設定したViewIDへ転送される
*/
void	AView::NVI_SetMouseWheelEventRedirectViewID( const AViewID inViewID )
{
	mMouseWheelEventRedirectViewID = inViewID;
	NVIDO_SetMouseWheelEventRedirectViewID(inViewID);
	//FrameViewも同様に設定
	if( ExistFrameView() == true )
	{
		GetFrameView().NVI_SetMouseWheelEventRedirectViewID(inViewID);
	}
}

#pragma mark ===========================

#pragma mark ---Frame View
//#135

/**
FrameViewを生成
*/
void	AView::NVI_CreateFrameView( const ANumber inLeftMargin, const ANumber inRightMargin, const ANumber inTopMargin, const ANumber inBottomMargin )
{
	//Frameとのあいだの余白サイズを設定
	mFrameLeftMargin 	= inLeftMargin;
	mFrameRightMargin 	= inRightMargin;
	mFrameTopMargin 	= inTopMargin;
	mFrameBottomMargin 	= inBottomMargin;
	//このViewの元の位置・サイズを取得し、このViewのサイズは縮小する。
	AWindowPoint	origPoint = NVI_GetPosition();
	ANumber	origWidth = NVI_GetWidth();
	ANumber	origHeight = NVI_GetHeight();
	AWindowPoint	insetPoint;
	insetPoint.x = origPoint.x + mFrameLeftMargin;
	insetPoint.y = origPoint.y + mFrameTopMargin;
	ANumber	insetWidth = origWidth - mFrameLeftMargin - mFrameRightMargin;
	ANumber	insetHeight = origHeight - mFrameTopMargin - mFrameBottomMargin;
	NVMC_SetControlPosition(insetPoint);
	NVMC_SetControlSize(insetWidth,insetHeight);
	//EditBoxが存在するTabControlのIndexを取得
	AIndex	tabControlIndex = NVM_GetWindow().NVI_GetControlEmbeddedTabControlIndex(NVI_GetControlID());
	//Frame生成
	mFrameViewControlID = NVM_GetWindow().NVM_AssignDynamicControlID();//#271
	AFrameViewFactory	frameViewFactory(NVM_GetWindowID(),mFrameViewControlID,NVI_GetControlID());//#92
	NVM_GetWindow().NVI_CreateView(mFrameViewControlID,origPoint,origWidth,origHeight,kControlID_Invalid,NVI_GetControlID(),false,frameViewFactory,tabControlIndex,true);//#92 #137 最後のfalse->true
	GetFrameView().SPI_SetRealFramePositionAndSize(origPoint,origWidth,origHeight);//#135
	NVM_GetWindow().NVI_EmbedControl(mFrameViewControlID,NVI_GetControlID());//#182
}

/*#688
//FrameViewの位置・サイズを更新
void	AView::UpdateFrameViewBounds()
{
	//EditBoxの位置・サイズを取得し、Frameの位置・サイズを計算する
	AWindowPoint	editBoxPoint = NVI_GetPosition();
	ANumber	editBoxWidth = NVI_GetWidth();
	ANumber	editBoxHeight = NVI_GetHeight();
	AWindowPoint	framePoint;
	framePoint.x = editBoxPoint.x - mFrameLeftMargin;//#135
	framePoint.y = editBoxPoint.y - mFrameTopMargin;//#135
	ANumber	frameWidth = editBoxWidth + mFrameLeftMargin + mFrameRightMargin;//#135
	ANumber	frameHeight = editBoxHeight + mFrameTopMargin + mFrameBottomMargin;//#135
	//Frameの位置・サイズ変更
	GetFrameView().SPI_SetRealFramePositionAndSize(framePoint,frameWidth,frameHeight);//#135
	//EditBoxの位置も変更されるので、ここで元の位置（WindowPoint）を再設定する #390
	NVMC_SetControlPosition(editBoxPoint);
}
*/

//FrameViewを削除
void	AView::DeleteFrameView()
{
	if( NVM_GetWindow().NVI_ExistView(mFrameViewControlID) == true )//#137 NVI_DeleteTabAndView()により先に削除されている場合があるのでチェック追加
	{
		NVM_GetWindow().NVI_DeleteControl(mFrameViewControlID);//win NVI_DeleteView()はmTabDataArray_ControlIDsからの削除等を行わないのでNG
	}
	mFrameViewControlID = kControlID_Invalid;
}

/**
FrameViewオブジェクト取得
*/
AView_Frame&	AView::GetFrameView()
{
	return dynamic_cast<AView_Frame&>(NVM_GetWindow().NVI_GetViewByControlID(mFrameViewControlID));
}

#pragma mark ===========================

#pragma mark ---ScrollBar
//#135

/**
VScrollBar生成
*/
void	AView::NVI_CreateVScrollBar( const ANumber inWidth, const ABool inHasHScrollbar )
{
	//
	mVScrollBarWidth = inWidth;
	//このViewの元の位置・サイズを取得し、このViewのサイズは縮小する。
	AWindowPoint	origPoint = NVI_GetPosition();
	ANumber	origWidth = NVI_GetWidth();
	ANumber	origHeight = NVI_GetHeight();
	ANumber	insetWidth = origWidth - mVScrollBarWidth;
	ANumber	insetHeight = origHeight;
	NVMC_SetControlSize(insetWidth,insetHeight);
	//EditBoxが存在するTabControlのIndexを取得
	AIndex	tabControlIndex = NVM_GetWindow().NVI_GetControlEmbeddedTabControlIndex(NVI_GetControlID());
	//ScrollBar生成
	mVScrollBarControlID = NVM_GetWindow().NVM_AssignDynamicControlID();
	if( ExistFrameView() == true )
	{
		//
		AWindowRect	frameRealBounds;
		GetFrameView().SPI_GetRealFrameBounds(frameRealBounds);
		//
		AWindowPoint	pt;
		pt.x = frameRealBounds.right - mVScrollBarWidth -1;
		pt.y = frameRealBounds.top +1;
		ANumber	h = frameRealBounds.bottom-frameRealBounds.top-2;
		if( inHasHScrollbar == true )
		{
			h -= 15;
		}
		NVM_GetWindow().NVI_CreateScrollBar(mVScrollBarControlID,pt,mVScrollBarWidth,h,tabControlIndex);
		NVM_GetWindow().NVI_EmbedControl(mFrameViewControlID,mVScrollBarControlID);
	}
	else
	{
		AWindowPoint	pt;
		pt.x = origPoint.x + insetWidth;
		pt.y = origPoint.y;
		NVM_GetWindow().NVI_CreateScrollBar(mVScrollBarControlID,pt,mVScrollBarWidth,insetHeight,tabControlIndex);
	}
	//
	NVM_GetWindow().NVI_SetControlBindings(mVScrollBarControlID,false,true,true,true,true,false);//#455 #688
	//
	mControlID_VScrollBar = mVScrollBarControlID;
}

/**
HScrollBar生成
*/
void	AView::NVI_CreateHScrollBar( const ANumber inHeight, const ABool inHasVScrollbar )
{
	//
	mHScrollBarHeight = inHeight;
	//このViewの元の位置・サイズを取得し、このViewのサイズは縮小する。
	AWindowPoint	origPoint = NVI_GetPosition();
	ANumber	origWidth = NVI_GetWidth();
	ANumber	origHeight = NVI_GetHeight();
	ANumber	insetWidth = origWidth;
	ANumber	insetHeight = origHeight - mHScrollBarHeight;
	NVMC_SetControlSize(insetWidth,insetHeight);
	//EditBoxが存在するTabControlのIndexを取得
	AIndex	tabControlIndex = NVM_GetWindow().NVI_GetControlEmbeddedTabControlIndex(NVI_GetControlID());
	//ScrollBar生成
	mHScrollBarControlID = NVM_GetWindow().NVM_AssignDynamicControlID();
	if( ExistFrameView() == true )
	{
		//
		AWindowRect	frameRealBounds;
		GetFrameView().SPI_GetRealFrameBounds(frameRealBounds);
		//
		AWindowPoint	pt;
		pt.x = frameRealBounds.left +1;
		pt.y = frameRealBounds.bottom - mHScrollBarHeight -1;
		ANumber	w = frameRealBounds.right-frameRealBounds.left-2;
		if( inHasVScrollbar == true )
		{
			w -= 15;
		}
		NVM_GetWindow().NVI_CreateScrollBar(mHScrollBarControlID,pt,w,mHScrollBarHeight,tabControlIndex);
		NVM_GetWindow().NVI_EmbedControl(mFrameViewControlID,mHScrollBarControlID);
	}
	else
	{
		AWindowPoint	pt;
		pt.x = origPoint.x;
		pt.y = origPoint.y + insetHeight;
		NVM_GetWindow().NVI_CreateScrollBar(mHScrollBarControlID,pt,insetWidth,mHScrollBarHeight,tabControlIndex);
	}
	//
	NVM_GetWindow().NVI_SetControlBindings(mHScrollBarControlID,true,false,true,true,false,true);//#455 #688
	//
	mControlID_HScrollBar = mHScrollBarControlID;
}


#pragma mark ===========================

#pragma mark ---マウスクリックTrackingモード設定
//#688

/**
マウスクリックTrackingモードを設定する
*/
void	AView::NVM_SetMouseTrackingMode( const ABool inSet )
{
	//メンバ変数設定（trueのときのみEVTDO_DoMouseTracking()/EVT_DoMouseTrackEnd()を実行する）
	mMouseTrackingMode = inSet;
	//ViewImpにモード設定
	mViewImp.SetMouseTrackingMode(inSet);
}

#pragma mark ===========================

#pragma mark ---テキスト描画
//#725

/**
最後の方を...で省略したテキストを取得
*/
void	AView::NVI_GetEllipsisTextWithFixedLastCharacters( const AText& inText, CTextDrawData& inTextDrawData, const ANumber inWidth,
		const AItemCount inLastCharCount, AText& outText, const ABool inEllipsisRepeatChar ) 
{
	//省略前のテキスト表示幅を取得
	ANumber	totalWidth = NVMC_GetDrawTextWidth(inTextDrawData);
	//元々inWidthに収まっていれば、そのままのテキストを格納して終了。
	if( totalWidth < inWidth )
	{
		outText.SetText(inText);
		return;
	}
	//...の表示幅を取得
	AText	ellipsisText("...");
	ANumber	ellipsisTextWidth = NVMC_GetDrawTextWidth(ellipsisText);
	//最後からinLastCharCount文字の位置を取得
	AIndex	lastCharsStartIndex = inText.GetItemCount() - inLastCharCount;
	if( lastCharsStartIndex < 0 )   lastCharsStartIndex = 0;
	lastCharsStartIndex = inText.GetCurrentCharPos(lastCharsStartIndex);
	//最後からinLastCharCount文字分の表示幅を取得
	ANumber	lastCharsWidth = totalWidth - NVMC_GetImageXByTextPosition(inTextDrawData,lastCharsStartIndex);
	//inWidthから...以降の部分を除いた表示幅を計算
	ANumber	remainTextWidth = inWidth - 3 - ellipsisTextWidth - lastCharsWidth;
	if( inEllipsisRepeatChar == true )
	{
		//------------------繰り返し文字を省略する場合------------------
		//繰り返し文字があるかどうかを判定し、あればその部分も省略する。
		//（繰り返し文字がなければ、通常処理を行う）
		
		//inLastCharCount文字の位置までのテキストを取得
		AText	text;
		inText.GetText(0,lastCharsStartIndex,text);
		//繰り返し文字の位置を取得
		AUChar	prevCh = 0;
		AItemCount	repeatCount = 0;
		AIndex	repeatSpos = kIndex_Invalid;
		AIndex	repeatEpos = kIndex_Invalid;
		for( AIndex p = 0; p < text.GetItemCount(); p++ )
		{
			AUChar	ch = text.Get(p);
			if( prevCh == ch )
			{
				//前の文字と同じ場合
				//繰り返し開始位置を記憶
				if( repeatSpos == kIndex_Invalid )
				{
					repeatSpos = p-1;
				}
				//繰り返し数インクリメント
				repeatCount++;
			}
			else
			{
				//前の文字と違う場合
				if( repeatCount >= 6 )
				{
					//繰り返し終了時に6文字以上の場合は、終了位置を設定して、ループ終了
					repeatEpos = p;
					break;
				}
				//繰り返しリセット
				repeatSpos = kIndex_Invalid;
				repeatCount = 0;
			}
			//前回の文字を記憶
			prevCh = ch;
		}
		if( repeatEpos != kIndex_Invalid )
		{
			//------------------繰り返し発見した場合------------------
			
			//繰り返し部分削除
			text.Delete(repeatSpos+1,repeatEpos-repeatSpos-2);
			//...挿入
			text.InsertCstring(repeatSpos+1,"...");
			//「inWidthから...以降の部分を除いた表示幅」内に表示できるテキストを取得
			NVI_GetTextInWidth(text,remainTextWidth,outText);
			//上につなげて「最後の部分のテキスト」を順にoutTextに格納
			outText.AddCstring("...");
			AText	lastText;
			inText.GetText(lastCharsStartIndex,inText.GetItemCount()-lastCharsStartIndex,lastText);
			outText.AddText(lastText);
			return;
		}
	}
	//inWidthから...以降の部分を除いた表示幅に表示できる文字数を取得
	AIndex	remainTextEndIndex = NVMC_GetTextPositionByImageX(inTextDrawData,remainTextWidth);
	//「inWidthから...以降の部分を除いた表示幅に表示できる文字数分のテキスト」「...」「最後の部分のテキスト」を順にoutTextに格納
	inText.GetText(0,remainTextEndIndex,outText);
	outText.AddCstring("...");
	AText	lastText;
	inText.GetText(lastCharsStartIndex,inText.GetItemCount()-lastCharsStartIndex,lastText);
	outText.AddText(lastText);
}

/**
最後の方を...で省略したテキストを取得
*/
void	AView::NVI_GetEllipsisTextWithFixedLastCharacters( const AText& inText, const ANumber inWidth,
														  const AItemCount inLastCharCount, AText& outText, 
														  const ABool inEllipsisRepeatChar ) 
{
	CTextDrawData	textDrawData(false);
	textDrawData.MakeTextDrawDataWithoutStyle(inText);
	NVI_GetEllipsisTextWithFixedLastCharacters(inText,textDrawData,inWidth,inLastCharCount,outText,inEllipsisRepeatChar);
}

/**
最初の方を...で省略したテキストを取得
*/
void	AView::NVI_GetEllipsisTextWithFixedFirstCharacters( const AText& inText, CTextDrawData& inTextDrawData, const ANumber inWidth,
		const AItemCount inFirstCharCount, AText& outText ) 
{
	//省略前のテキスト表示幅を取得
	ANumber	totalWidth = NVMC_GetDrawTextWidth(inTextDrawData);
	//元々inWidthに収まっていれば、そのままのテキストを格納して終了。
	if( totalWidth < inWidth )
	{
		outText.SetText(inText);
		return;
	}
	//...の表示幅を取得
	AText	ellipsisText("...");
	ANumber	ellipsisTextWidth = NVMC_GetDrawTextWidth(ellipsisText);
	//最初からinFirstCharCount文字の位置を取得
	AIndex	firstCharsEndIndex = inFirstCharCount;
	if( firstCharsEndIndex >= inText.GetItemCount() )   firstCharsEndIndex = inText.GetItemCount();
	firstCharsEndIndex = inText.GetCurrentCharPos(firstCharsEndIndex);
	//最初からinFirstCharCount文字分の表示幅を取得
	ANumber	firstCharsWidth = NVMC_GetImageXByTextPosition(inTextDrawData,firstCharsEndIndex);
	//inWidthから...以前の部分を除いた表示幅を計算
	ANumber	remainTextWidth = inWidth -3 - ellipsisTextWidth - firstCharsWidth;
	//inWidthから...以前の部分を除いた表示幅に表示できる文字数を取得
	AIndex	remainTextStartIndex = NVMC_GetTextPositionByImageX(inTextDrawData,totalWidth-remainTextWidth);
	//inWidthから...以前の部分を除いた表示幅に表示できる文字数分のテキスト、...、最後の部分のテキストを順にoutTextに格納
	inText.GetText(0,firstCharsEndIndex,outText);
	outText.AddCstring("...");
	AText	remainText;
	inText.GetText(remainTextStartIndex,inText.GetItemCount()-remainTextStartIndex,remainText);
	outText.AddText(remainText);
}

/**
最初の方を...で省略したテキストを取得
*/
void	AView::NVI_GetEllipsisTextWithFixedFirstCharacters( const AText& inText, const ANumber inWidth,
		const AItemCount inLastCharCount, AText& outText ) 
{
	CTextDrawData	textDrawData(false);
	textDrawData.MakeTextDrawDataWithoutStyle(inText);
	NVI_GetEllipsisTextWithFixedFirstCharacters(inText,textDrawData,inWidth,inLastCharCount,outText);
}

/**
指定幅までのテキストを取得
*/
void	AView::NVI_GetTextInWidth( const AText& inText, const ANumber inWidth, AText& outText ) 
{
	CTextDrawData	textDrawData(false);
	textDrawData.MakeTextDrawDataWithoutStyle(inText);
	AIndex	index = NVMC_GetTextPositionByImageX(textDrawData,inWidth);
	inText.GetText(0,index,outText);
}

/**
指定rect内に複数行（ワードラップ）にテキスト表示する
*/
void	AView::NVI_DrawTextMultiLine( const ALocalRect& inLocalRect, const AText& inText,
		const AText& inFontName, const AFloatNumber inFontSize, 
		const AColor inColor, const ATextStyle inStyle, const ABool inAntiAlias )
{
	NVMC_SetDefaultTextProperty(inFontName,inFontSize,inColor,inStyle,true);
	ANumber	fontheight = 9 ,fontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
	//ワードラップ計算
	AArray<AIndex>	lineStartArray;
	AArray<AItemCount>	lengthArray;
	//inText.CalcParagraphBreaks(lineStartArray,lengthArray);
	CWindowImp::CalcLineBreaks(inText,inFontName,inFontSize,inAntiAlias,inLocalRect.right-inLocalRect.left,false,
							   lineStartArray,lengthArray);
	//各行ごとの処理
	for( AIndex lineIndex = 0; lineIndex < lineStartArray.GetItemCount(); lineIndex++ )
	{
		//行データ取得
		AIndex	start 	= lineStartArray.Get(lineIndex);
		AItemCount	len = lengthArray.Get(lineIndex);
		AText	text;
		inText.GetText(start,len,text);
		//行rect取得
		ALocalRect	rect = inLocalRect;
		rect.top		+= lineIndex*fontheight;
		rect.bottom		+= rect.top + fontheight;
		//行描画
		NVMC_DrawText(rect,text,kJustification_Left);
	}
}

