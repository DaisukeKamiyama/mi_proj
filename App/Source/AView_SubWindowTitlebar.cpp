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

AView_SubWindowTitlebar

*/

#include "stdafx.h"

#include "AView_SubWindowTitlebar.h"
#include "AApp.h"


//
const ANumber	kWidth_LeftMargin_Overlay = 4;
const ANumber	kWidth_FoldingTriangleIcon = 15;
//
const ANumber	kWidth_CloseButton = 19;

#pragma mark ===========================
#pragma mark [クラス]AView_SubWindowTitlebar
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ

@param inWindowID 親ウインドウのID
@param inID ViewのControlID
*/
AView_SubWindowTitlebar::AView_SubWindowTitlebar( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), /*mDragReceiving(false),*/ mMouseTrackStartLocalPoint(kLocalPoint_00)
,mDraggingOverlayWindow(false),mTitleIconImageID(kImageID_Invalid)
,mMouseHoverCloseButton(false),mMouseHoverFoldingButton(false)
,mMousePressingFoldingButton(false),mMousePressingCloseButton(false)
{
}

/**
デストラクタ
*/
AView_SubWindowTitlebar::~AView_SubWindowTitlebar()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_SubWindowTitlebar::NVIDO_Init()
{
	/*
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_SubWindowTitlebar);
	NVMC_EnableDrop(scrapTypeArray);
	*/
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_SubWindowTitlebar::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//描画要求時のコールバック(AView用)
void	AView_SubWindowTitlebar::EVTDO_DoDraw()
{
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	
	//---------------タイトルバーイメージ描画---------------
	ALocalPoint	pt = {0,0};
	if( IsOverlay() == true )
	{
		//---------------オーバーレイ時---------------
		
		//タイトルバーImage描画
		NVMC_DrawImageFlexibleWidth(kImageID_SubWindowTitlebar_Overlay0,kImageID_SubWindowTitlebar_Overlay1,kImageID_SubWindowTitlebar_Overlay2,
					pt,localFrameRect.right-localFrameRect.left);
		
		//折りたたみ状態取得
		ABool	collapsed = false;
		AWindowID	textWindowID = NVM_GetWindow().NVI_GetOverlayParentWindowID();
		if( textWindowID != kObjectID_Invalid )
		{
			collapsed = GetApp().SPI_GetTextWindowByID(textWindowID).SPI_IsCollapsedSubWindow(NVM_GetWindow().GetObjectID());
		}
		
		//---------------折りたたみイメージ描画---------------
		{
			ALocalPoint	pt = {0};
			pt.x = localFrameRect.left+kWidth_LeftMargin_Overlay;
			pt.y = localFrameRect.top+2;
			if( collapsed == true )
			{
				//折りたたみ時トライアングルアイコン
				if( mMousePressingFoldingButton == true )
				{
					//押下時
					NVMC_DrawImage(kImageID_barSwCursorRight_p,pt);
				}
				else if( mMouseHoverFoldingButton == false )
				{
					//通常時
					NVMC_DrawImage(kImageID_barSwCursorRight,pt);
				}
				else
				{
					//ホバー時
					NVMC_DrawImage(kImageID_barSwCursorRight_h,pt);
				}
			}
			else
			{
				//展開時トライアングルアイコン
				if( mMousePressingFoldingButton == true )
				{
					//押下時
					NVMC_DrawImage(kImageID_barSwCursorDown_p,pt);
				}
				else if( mMouseHoverFoldingButton == false )
				{
					//通常時
					NVMC_DrawImage(kImageID_barSwCursorDown,pt);
				}
				else
				{
					//ホバー時
					NVMC_DrawImage(kImageID_barSwCursorDown_h,pt);
				}
			}
		}
	}
	else
	{
		//---------------フローティング／ポップアップ時---------------
		
		NVMC_DrawImageFlexibleWidth(kImageID_SubWindowTitlebar_Floating0,kImageID_SubWindowTitlebar_Floating1,kImageID_SubWindowTitlebar_Floating2,
					pt,localFrameRect.right-localFrameRect.left);
		
		//---------------クローズボタンイメージ描画---------------
		{
			ALocalPoint	pt = {0};
			pt.x = localFrameRect.left+2;
			pt.y = localFrameRect.top+2;
			if( mMousePressingCloseButton == true )
			{
				//押下時
				NVMC_DrawImage(kImageID_barSwCloseButton_p,pt);
			}
			else if( mMouseHoverCloseButton == false )
			{
				//通常時
				NVMC_DrawImage(kImageID_barSwCloseButton_1,pt);
			}
			else
			{
				//ホバー時
				NVMC_DrawImage(kImageID_barSwCloseButton_h,pt);
			}
		}
	}
	
	//フォント設定（ダイアログ用標準フォント）
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);
	NVMC_SetDefaultTextProperty(fontname,9.0,kColor_Gray90Percent,kTextStyle_Bold,true);
	
	//タイトルバーテキスト描画
	ALocalRect	rect =	localFrameRect;
	rect.top = 3;
	rect.bottom = 17;
	AColor	color = GetApp().SPI_GetSubWindowTitlebarTextColor();
	NVMC_SetDropShadowColor(GetApp().SPI_GetSubWindowTitlebarTextDropShadowColor());
	AJustification	justification = kJustification_Center;
	ABool	titleJustificationLeft = (IsOverlay() == true );
	if( titleJustificationLeft == true )
	{
		rect.left = 38;
		justification = kJustification_Left;
	}
	NVMC_DrawText(rect,mTitleText,color,kTextStyle_Bold|kTextStyle_DropShadow,justification);
	
	//タイトルバーアイコン描画
	if( mTitleIconImageID != kImageID_Invalid )
	{
		ALocalPoint	pt = {0};
		pt.x = localFrameRect.left + kWidth_LeftMargin_Overlay + kWidth_FoldingTriangleIcon;
		pt.y = localFrameRect.top+3;
		if( titleJustificationLeft == false )
		{
			pt.x = (localFrameRect.left+localFrameRect.right)/2-NVMC_GetDrawTextWidth(mTitleText)/2 - 18;
		}
		NVMC_DrawImage(mTitleIconImageID,pt);
	}
}

/**
マウスボタンダウン時処理
*/
ABool	AView_SubWindowTitlebar::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//tracking開始時マウス位置記憶
	mMouseTrackStartLocalPoint = inLocalPoint;
	
	//クローズボタン／折りたたみアイコンクリック判定
	mMousePressingFoldingButton = false;
	mMousePressingCloseButton = false;
	if( IsPointInCloseButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInCloseButton(inLocalPoint) == true )
		{
			mMousePressingCloseButton = true;
		}
		NVI_Refresh();
	}
	if( IsPointInFoldingButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInFoldingButton(inLocalPoint) == true )
		{
			mMousePressingFoldingButton = true;
		}
		NVI_Refresh();
	}
	
	//Mouse tracking開始
	NVM_SetMouseTrackingMode(true);
	NVI_Refresh();
	return true;
}

/**
マウスボタン押下中TrackingのOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_SubWindowTitlebar::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//クローズボタン／折りたたみアイコンクリック判定
	mMousePressingFoldingButton = false;
	mMousePressingCloseButton = false;
	if( IsPointInCloseButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInCloseButton(inLocalPoint) == true )
		{
			mMousePressingCloseButton = true;
		}
		NVI_Refresh();
		return;
	}
	if( IsPointInFoldingButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInFoldingButton(inLocalPoint) == true )
		{
			mMousePressingFoldingButton = true;
		}
		NVI_Refresh();
		return;
	}
	
	//==================タイトルバーDrag==================
	//tracking開始時と違うマウス位置になったらdrag開始
	if( inLocalPoint.x != mMouseTrackStartLocalPoint.x || inLocalPoint.y != mMouseTrackStartLocalPoint.y )
	{
		//DragしたフラグON
		if( mDraggingOverlayWindow == false )
		{
			mDraggingOverlayWindow = true;
		}
	}
	//ウインドウDrag移動・サイドバーDock
	if( mDraggingOverlayWindow == true )
	{
		//マウス位置取得
		AGlobalPoint	mouseGlobalPoint = {0};
		AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
		//ウインドウbounds取得
		ARect	rect = {0};
		NVM_GetWindow().NVI_GetWindowBounds(rect);
		ANumber	w = rect.right - rect.left;
		ANumber	h = rect.bottom - rect.top;
		//マウス位置に応じた新規ウインドウ左上ポイントを計算し、スクリーン内に補正
		APoint	pt = {0};
		pt.x = mouseGlobalPoint.x - mMouseTrackStartLocalPoint.x;
		pt.y = mouseGlobalPoint.y - mMouseTrackStartLocalPoint.y;
		NVM_GetWindow().NVI_ConstrainWindowPosition(true,pt);
		//新規ウインドウbounds取得
		rect.left = pt.x;
		rect.top = pt.y;
		rect.right = rect.left + w;
		rect.bottom = rect.top + h;
		//フローティングウインドウ間の吸着補正
		GetApp().SPI_DockSubWindowsToSubWindows(rect);
		//ウインドウbounds設定
		NVM_GetWindow().NVI_SetWindowBounds(rect);
		//最前面のテキストウインドウのサブウインドウdock処理
		AWindowID	textWindowID = NVM_GetWindow().NVI_GetOverlayParentWindowID();
		if( textWindowID == kObjectID_Invalid )
		{
			textWindowID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
		}
		if( textWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(textWindowID).SPI_DragSubWindow(NVM_GetWindow().GetObjectID(),mouseGlobalPoint);
		}
	}
	NVI_Refresh();
}

/**
マウスボタン押下中Tracking終了時のOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_SubWindowTitlebar::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//クローズボタン／折りたたみアイコンクリック判定
	mMousePressingFoldingButton = false;
	mMousePressingCloseButton = false;
	NVI_Refresh();
	//
	if( IsPointInCloseButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInCloseButton(inLocalPoint) == true )
		{
			NVM_GetWindow().EVT_DoCloseButton();
		}
		return;
	}
	if( IsPointInFoldingButton(mMouseTrackStartLocalPoint) == true )
	{
		if( IsPointInFoldingButton(inLocalPoint) == true )
		{
			AWindowID	textWindowID = NVM_GetWindow().NVI_GetOverlayParentWindowID();
			if( textWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(textWindowID).SPI_ExpandCollapseSubWindow(NVM_GetWindow().GetObjectID());
				return;
			}
		}
		return;
	}
	
	//最前面のテキストウインドウのサブウインドウdock完了処理
	AWindowID	textWindowID = NVM_GetWindow().NVI_GetOverlayParentWindowID();
	if( textWindowID == kObjectID_Invalid )
	{
		textWindowID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
	}
	if( textWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(textWindowID).SPI_EndDragSubWindow(NVM_GetWindow().GetObjectID());
	}
	//Dragしなかった場合は、ウインドウにクリックを通知
	if( mDraggingOverlayWindow == false )
	{
		//ウインドウにクリックを通知
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
	}
	//DragしたフラグOFF
	mDraggingOverlayWindow = false;
	
	//Tracking解除
	NVM_SetMouseTrackingMode(false);
}

/**
指定ポイントがクリックボタン内かどうかの判定
*/
ABool	AView_SubWindowTitlebar::IsPointInCloseButton( const ALocalPoint& inLocalPoint ) const
{
	if( IsOverlay() == true )
	{
		return false;
	}
	//
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	return (inLocalPoint.x >= localFrameRect.left && inLocalPoint.x <= localFrameRect.left + kWidth_CloseButton &&
		inLocalPoint.y >= localFrameRect.top && inLocalPoint.y <= localFrameRect.bottom );
}

/**
指定ポイントが折りたたみマーク内かどうかの判定
*/
ABool	AView_SubWindowTitlebar::IsPointInFoldingButton( const ALocalPoint& inLocalPoint ) const
{
	if( IsOverlay() == false )
	{
		return false;
	}
	//
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	return (inLocalPoint.x >= localFrameRect.left && inLocalPoint.x <= localFrameRect.left + kWidth_LeftMargin_Overlay+kWidth_FoldingTriangleIcon &&
		inLocalPoint.y >= localFrameRect.top && inLocalPoint.y <= localFrameRect.bottom );
}

/**
マウス移動時のOSイベントコールバック

@param inLocalPoint マウスカーソルの場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
ABool	AView_SubWindowTitlebar::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//クローズボタン／折りたたみアイコンホバー判定
	mMouseHoverCloseButton = false;
	mMouseHoverFoldingButton = false;
	if( IsPointInCloseButton(inLocalPoint) == true )
	{
		mMouseHoverCloseButton = true;
	}
	if( IsPointInFoldingButton(inLocalPoint) == true )
	{
		mMouseHoverFoldingButton = true;
	}
	NVI_Refresh();
	return true;
}

/**
マウスがView外へ移動した時のOSイベントコールバック

@param inLocalPoint マウスカーソルの場所（コントロールローカル座標）
*/
void	AView_SubWindowTitlebar::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	//ホバー解除
	mMouseHoverCloseButton = false;
	mMouseHoverFoldingButton = false;
	NVI_Refresh();
}

#if 0
/**
Drag&Drop処理（Drag中）
*/
void	AView_SubWindowTitlebar::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
{
}

/**
Drag&Drop処理（DragがViewに入った）
*/
void	AView_SubWindowTitlebar::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_SubWindowTitlebar) == true )
	{
		mDragReceiving = true;
		NVI_Refresh();
	}
}

/**
Drag&Drop処理（DragがViewから出た）
*/
void	AView_SubWindowTitlebar::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_SubWindowTitlebar) == true )
	{
		mDragReceiving = false;
		NVI_Refresh();
	}
}

/**
Drag&Drop処理（Drop）
*/
void	AView_SubWindowTitlebar::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_SubWindowTitlebar) == true )
	{
		AText	text;
		AScrapWrapper::GetDragData(inDragRef,kScrapType_SubWindowTitlebar,text);
		
		//
		ASubWindowLocationType	subWindowLocationType = kSubWindowLocationType_None;
		AIndex	subWindowLocationIndex = kIndex_Invalid;
		GetApp().SPI_GetSubWindowProperty(NVM_GetWindow().GetObjectID(),subWindowLocationType,subWindowLocationIndex);
		//
		ANumber	num = text.GetNumber();
		ASubWindowLocationType	droppedType = (ASubWindowLocationType)(num/100);
		AIndex	droppedIndex = (num%100);
		if( droppedIndex == 99 )
		{
			GetApp().SPI_AddOverlaySubWindow(subWindowLocationType,subWindowLocationIndex,(ASubWindowType)(num/100));
		}
		else
		{
			GetApp().SPI_MoveOverlaySubWindow(droppedType,droppedIndex,subWindowLocationType,subWindowLocationIndex);
		}
	}
}
#endif
/**
カーソルタイプ取得
*/
ACursorType	AView_SubWindowTitlebar::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	return kCursorType_Arrow;
}

/**
*/
ABool	AView_SubWindowTitlebar::IsOverlay() const
{
	return (NVM_GetWindowConst().NVI_GetOverlayParentWindowID() != kObjectID_Invalid);
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

/**
タイトルバーテキストとアイコン設定
*/
void	AView_SubWindowTitlebar::SPI_SetTitleTextAndIconImageID( const AText& inText, const AImageID inIconImageID )
{
	mTitleText.SetText(inText);
	mTitleIconImageID = inIconImageID;
	NVI_Refresh();
}


#pragma mark ===========================
#pragma mark [クラス]AView_SubWindowBackground
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ

@param inWindowID 親ウインドウのID
@param inID ViewのControlID
*/
AView_SubWindowBackground::AView_SubWindowBackground( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID)
{
}

/**
デストラクタ
*/
AView_SubWindowBackground::~AView_SubWindowBackground()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_SubWindowBackground::NVIDO_Init()
{
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_SubWindowBackground::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//描画要求時のコールバック(AView用)
void	AView_SubWindowBackground::EVTDO_DoDraw()
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	//==================ロケーションタイプ毎の処理==================
	ASubWindowLocationType	locationType = GetApp().SPI_GetSubWindowLocationType(NVM_GetWindowConst().GetObjectID());
	switch(locationType)
	{
		//ポップアップ用描画
	  case kSubWindowLocationType_Popup:
		{
			//==================色取得==================
			
			//描画色設定
			AColor	letterColor = kColor_Black;
			AColor	dropShadowColor = kColor_White;
			AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
			GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
			
			//
			switch( GetApp().SPI_GetSubWindowType(NVM_GetWindow().GetObjectID()) )
			{
				//補完候補リストの場合、rounded rectを描画
			  case kSubWindowType_CandidateList:
				{
					//色・α取得
					AColor	backgroundColor = kColor_White;
					AFloatNumber	backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha()+0.1;
					//描画rect取得（frameから1pixel insetする）
					ALocalRect	rect = localFrameRect;
					rect.top		+= 1;
					rect.left		+= 1;
					rect.right		-= 1;
					rect.bottom		-= kHeight_CandidateListStatusBar-1;
					//描画
					NVMC_PaintRoundedRect(rect,
										  //kColor_White,kColor_Gray95Percent
										  boxBaseColor1,boxBaseColor2,
										  kGradientType_Vertical,backgroundAlpha,backgroundAlpha,
										  kRadius_PopupCandidateList,true,true,false,false);
					NVMC_FrameRoundedRect(rect,
										  //kColor_Gray20Percent
										  //kColor_Gray50Percent,
										  letterColor,
										  0.5,kRadius_PopupCandidateList,true,true,false,false,
										  true,true,true,false,kLineWidth_PopupCandidateList);
					break;
				}
				//
			  case kSubWindowType_KeyToolList:
				{
					//色・α取得
					AColor	backgroundColor = kColor_White;
					AFloatNumber	backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha() +0.1;
					//描画rect取得（frameから1pixel insetする）
					ALocalRect	rect = localFrameRect;
					rect.top		+= 1;
					rect.left		+= 1;
					rect.right		-= 1;
					rect.bottom		-= 1;
					//描画
					NVMC_PaintRoundedRect(rect,
										  //kColor_White,kColor_Gray95Percent,
										  boxBaseColor1,boxBaseColor2,
										  kGradientType_Vertical,backgroundAlpha,backgroundAlpha,	
										  kRadius_PopupKeyToolList,true,true,true,true);
					NVMC_FrameRoundedRect(rect,
										  //kColor_Gray20Percent,
										  //kColor_Gray50Percent,
										  letterColor,
										  0.5,kRadius_PopupKeyToolList,true,true,true,true,
										  true,true,true,true,kLineWidth_PopupKeyToolList);
					break;
				}
				//それ以外の場合
			  default:
				{
					AColor	color = kColor_Gray80Percent;
					NVMC_FrameRect(localFrameRect,color);
					break;
				}
			}
			break;
		}
		//フローティングウインドウ
	  case kSubWindowLocationType_Floating:
		{
			//フローティングウインドウ背景色・α取得
			AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
			AFloatNumber	backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			//全体に左下、右下R=5.0のrounded rectを描画
			NVMC_PaintRoundedRect(localFrameRect,
								  backgroundColor,backgroundColor,kGradientType_Horizontal,backgroundAlpha,backgroundAlpha,
								  5.0,false,false,true,true);
			break;
		}
		//右サイドバー
	  case kSubWindowLocationType_RightSideBar:
		{
			//全体をサブウインドウ背景色でペイント
			AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
			NVMC_PaintRect(localFrameRect,backgroundColor);
			//右端以外をサイドバーフレーム色で描画
			AColor	color = GetApp().SPI_GetSideBarFrameColor();
			ABool	drawBottom = !(GetApp().SPI_IsSubWindowSideBarBottom(NVM_GetWindow().GetObjectID()));
			NVMC_FrameRect(localFrameRect,color,1.0,true,true,false,drawBottom,1.0);
			break;
		}
		//左サイドバー
	  case kSubWindowLocationType_LeftSideBar:
		{
			//全体をサブウインドウ背景色でペイント
			AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
			NVMC_PaintRect(localFrameRect,backgroundColor);
			//左端以外をサイドバーフレーム色で描画
			AColor	color = GetApp().SPI_GetSideBarFrameColor();
			ABool	drawBottom = !(GetApp().SPI_IsSubWindowSideBarBottom(NVM_GetWindow().GetObjectID()));
			NVMC_FrameRect(localFrameRect,color,1.0,false,true,true,drawBottom,1.0);
			break;
		}
	}
	/*
	//
	ALocalRect	r = localFrameRect;
	r.bottom += 3;
	NVMC_FrameRect(r,color);
	//
	ALocalPoint	spt = {localFrameRect.left,localFrameRect.bottom-2};
	ALocalPoint	ept = {localFrameRect.right,localFrameRect.bottom-2};
	NVMC_DrawLine(spt,ept,color);
	*/
}



