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

AView_Button

*/
//#232

#include "stdafx.h"

#include "../Frame.h"

//アイコンとテキストの間のスペース
const ANumber	kSpaceBetweenIconAndText = 2;

#pragma mark ===========================
#pragma mark [クラス]AView_Button
#pragma mark ===========================
/**
枠表示のためのView
*/

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ

@param inWindowID 親ウインドウのID
@param inID ViewのControlID
*/
AView_Button::AView_Button( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mIconImageID(kImageID_Invalid), mMenuID(kContextMenuID_Invalid), mToggleMode(false), mToggleOn(false),mContextMenuID(kContextMenuID_Invalid),
		mMouseHover(false), mMouseDown(false), mIconLeftOffset(4), mIconTopOffset(4),mIconWidth(16),mIconHeight(16),
		/*#688 mMouseTrackByLoop(true), */
		mTextJustification(kJustification_Left), mFontHeight(9), mFontAscent(7), mEnableDrag(false)
		,mTransparency(1.0), mAlwaysActiveColors(false)//#291
		,mFrameColor(kColor_Gray80Percent)//amazon
		,mMouseTrackResultIsDrag(false)//win
		,mColor(kColor_Gray10Percent),mColorDeactive(kColor_Gray60Percent)
,mButtonViewType(kButtonViewType_None)//kButtonViewType_Rect20)//#634
,mButtonBehaviorType(kButtonBehaviorType_Normal)//#634
,mPreviousMouseGlobalPoint(kGlobalPoint_00)//#634
,mButtonValidXRangeStart(-1),mButtonValidXRangeEnd(-1)//#634
,mHelpTagSide(kHelpTagSide_Default)//#661
,mFontSize(9.0), mTextStyle(kTextStyle_Normal)//#724
,mDisplayingContextMenu(false)//#724
,mDropShadowColor(kColor_White)//#724
,mEllipsisMode(kEllipsisMode_FixedLastCharacters),mEllipsisCharacterCount(5)//#725
,mDragging(false)//#850
,mTemporaryInvisible(false)//#724
,mMenuItemID(0)//#688
,mHoverIconImageID(kImageID_Invalid)
,mToggleOnIconImageID(kImageID_Invalid)
,mOvalHoverColor(kColor_Gray50Percent)
{
	NVMC_SetOffscreenMode(true);//win
}
/**
デストラクタ
*/
AView_Button::~AView_Button()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_Button::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
	/*#852
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);//#375
	SPI_SetTextProperty(defaultfontname,9.0,kJustification_Left);
	*/
	/*#688
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
}

//#852
/**
未初期化なら初期化する（高速化のため、本当に描画される直前でSPI_SetTextProperty()を実行する）
*/
void	AView_Button::InitTextPropertyIfNotInited()
{
	if( mFontName.GetItemCount() > 0 )   return;
	
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);//#375
	SPI_SetTextProperty(defaultfontname,mFontSize,mTextJustification,mTextStyle,mColor,mColorDeactive);
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_Button::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
マウスボタン押下時のOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
@param inClickCout クリック回数
*/
ABool	AView_Button::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//コントロールdisableなら何もしない
	if( NVMC_IsControlEnabled() == false )
	{
		return false;
	}
	
	//#634 ボタン有効範囲
	if( mButtonValidXRangeStart != -1 && mButtonValidXRangeEnd != -1 )
	{
		if( inLocalPoint.x < mButtonValidXRangeStart || inLocalPoint.x > mButtonValidXRangeEnd )
		{
			return false;
		}
	}
	
	//#634 VSeparatorモード
	if( mButtonBehaviorType == kButtonBehaviorType_VSeparator )
	{
		return DoMouseDown_VSeparator(inLocalPoint,inModifierKeys,inClickCount);
	}
	
	ALocalRect	frame;
	NVM_GetLocalViewRect(frame);
	//#256 win下から移動
	mMouseDown = true;
	
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	
	//TrackingMode設定
	mMouseTrackResultIsDrag = false;
	NVM_SetMouseTrackingMode(true);
	NVI_Refresh();//win
	return true;
	
	//#688 }
	/*#688
	//
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	mMouseTrackResultIsDrag = false;
	if( (mMenuID == kIndex_Invalid && mMenuTextArray.GetItemCount() == 0) || (AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true) )//#276
	{
		while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
		{
			ALocalPoint	mouseLocalPoint;
			NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
			if(	mouseLocalPoint.x >= frame.left && mouseLocalPoint.x <= frame.right &&
				mouseLocalPoint.y >= frame.top && mouseLocalPoint.y <= frame.bottom )
			{
				mMouseDown = true;
			}
			else
			{
				mMouseDown = false;
				if( mEnableDrag == true )
				{
					mMouseTrackResultIsDrag = true;
					NVI_Refresh();
					break;
				}
			}
			NVI_Refresh();
		}
	}
	if( mMouseTrackResultIsDrag == true )
	{
		StartDrag(inLocalPoint,inModifierKeys);//win
		return true;
	}
	//
	MouseDown(inModifierKeys);
	return true;
	*/
}

//win
void	AView_Button::StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mMouseTrackResultIsDrag == true )
	{
		ALocalRect	frame;
		NVM_GetLocalViewRect(frame);
		//Drag領域設定
		AArray<ALocalRect>	dragRect;
		dragRect.Add(frame);
		//Scrap設定
		AArray<AScrapType>	scrapType;
		ATextArray	data;
		scrapType.Add(kScrapType_ButtonView);
		AUniqID	viewUniqID = AApplication::GetApplication().NVI_GetViewUniqID(GetObjectID());//#693
		AText	text;
		text.AddNumber(viewUniqID.val);//#693
		data.Add(text);
		//Drag実行
		//#850 NVMC_DragText(inLocalPoint,dragRect,scrapType,data);
		AWindowRect	wrect = {0};
		NVM_GetWindow().NVI_GetWindowRectFromControlLocalRect(NVI_GetControlID(),frame,wrect);
		mDragging = true;
		if( MVMC_DragTextWithWindowImage(inLocalPoint,scrapType,data,wrect,wrect.right-wrect.left,wrect.bottom-wrect.top) == false )
		{
			NVM_GetWindow().OWICB_ButtonViewDragCanceled(NVI_GetControlID());
		}
		mDragging = false;
	}
}

/**
マウスボタンダウン時処理
*/
void	AView_Button::MouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalRect	frame;
	NVM_GetLocalViewRect(frame);
	
	//Toggle
	if( mMouseDown == true && mToggleMode == true )
	{
		mToggleOn = !mToggleOn;
	}
	//ボタンのControlID保存（WindowのEVTDO_DoMenuItemSelected()でどのボタンが押されたかを判定するため）
	NVM_GetWindow().NVI_SetLastClickedButtonControlID(NVI_GetControlID());
	//メニュー形式ボタンの場合、メニューを表示する
	//メニュークリック後、WindowのEVTDO_DoMenuItemSelected()に渡される
	//【メモ】メニュークリック後までShowContextMenu()は戻ってこないので、ここで選択メニュー項目をしてEVT_Clicked()で処理する方式もあったのだが、
	//WindowのEVTDO_DoMenuItemSelected()で処理する方が通常メニューと処理共通化できて良いかと思ったため。
	//ShowContextMenu()をちょっと改造すれば上記の方式をサポートすることは可能。
	if( mMouseDown == true && mMenuID != kIndex_Invalid )
	{
		//TextArrayMenu連動メニューの場合
		//ContextMenu設定（MenuItemIDとチェックマーク）
		if( mMenuItemID != kMenuItemID_Invalid )
		{
			AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuMenuItemID(mMenuID,mMenuItemID);
		}
		AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuCheckMark(mMenuID,mText);
		//ContextMenu表示
		ALocalPoint	localPoint;
		localPoint.x = 0;
		localPoint.y = frame.bottom+5;
		AGlobalPoint	globalPoint;
		NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),localPoint,globalPoint);
		//#724
		mDisplayingContextMenu = true;
		NVI_Refresh();
		//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(mMenuID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
		NVMC_ShowContextMenu(mMenuID,globalPoint);//#688
		mMouseDown = false;
		//#724
		mDisplayingContextMenu = false;
		//コンテキストメニュー処理によりコントロールが削除されている可能性があるので、存在チェックを行う
		if( NVM_GetWindow().NVI_ExistControl(NVI_GetControlID()) == true )
		{
			//描画更新
			NVI_Refresh();
			//ウインドウ内のクリック時処理実行
			NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
		}
		return;
	}
	if( mMouseDown == true && mMenuTextArray.GetItemCount() > 0 )
	{
		//固定メニュー（mMenuTextArrayで指定）の場合
		//ContextMenu設定（TextArrayとMeuItemID）
		AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuTextArray(kContextMenuID_ButtonView,mMenuTextArray,mActionTextArray);
		if( mMenuItemID != kMenuItemID_Invalid )
		{
			AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuMenuItemID(kContextMenuID_ButtonView,mMenuItemID);
		}
		AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuCheckMark(kContextMenuID_ButtonView,mText);
		//ContextMenu表示
		ALocalPoint	localPoint;
		localPoint.x = 0;
		localPoint.y = frame.bottom+5;
		AGlobalPoint	globalPoint;
		NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),localPoint,globalPoint);
		//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_ButtonView,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
		NVMC_ShowContextMenu(kContextMenuID_ButtonView,globalPoint);//#688
		mMouseDown = false;
		//コンテキストメニュー処理によりコントロールが削除されている可能性があるので、存在チェックを行う
		if( NVM_GetWindow().NVI_ExistControl(NVI_GetControlID()) == true )
		{
			//ウインドウ内のクリック時処理実行
			NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
		}
		return;
	}
	//
	ABool	mousedown = mMouseDown;
	//先に表示更新（Clickイベントでボタンが無くなる可能性があるから）
	mMouseDown = false;
	NVI_Refresh();
	//Clickイベント実行
	if( mousedown == true )
	{
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
	}
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
/**
マウスボタン押下中TrackingのOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_Button::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//#634 VSeparatorモード
	if( mButtonBehaviorType == kButtonBehaviorType_VSeparator )
	{
		DoMouseTrack_VSeparator(inLocalPoint);
		return;
	}
	
	ALocalRect	frame;
	NVM_GetLocalViewRect(frame);
	if(	inLocalPoint.x >= frame.left && inLocalPoint.x <= frame.right &&
		inLocalPoint.y >= frame.top && inLocalPoint.y <= frame.bottom )
	{
		mMouseDown = true;
	}
	else
	{
		mMouseDown = false;
		//
		if( mEnableDrag == true )
		{
			mMouseTrackResultIsDrag = true;
			//ドラッグ開始
			StartDrag(inLocalPoint,inModifierKeys);
			//TrackingMode解除
			NVM_SetMouseTrackingMode(false);
		}
	}
	//Drag処理によりコントロール削除されている可能性があるので、存在チェックしてから、refreshする #724
	if( NVM_GetWindow().NVI_ExistControl(NVI_GetControlID()) == true )
	{
		NVI_Refresh();
	}
}

/**
マウスボタン押下中Tracking終了時のOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_Button::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//#634 VSeparatorモード
	if( mButtonBehaviorType == kButtonBehaviorType_VSeparator )
	{
		DoMouseTrackEnd_VSeparator(inLocalPoint);
		return;
	}
	
	NVM_SetMouseTrackingMode(false);
	MouseDown(inLocalPoint,inModifierKeys);
}
//#688 #endif

//
const ANumber	kLeftPadding_TextWithOvalHover = 10;
const ANumber	kRightPadding_TextWithOvalHover = 10;
const ANumber	kTopPadding_TextWithOvalHover = 2;
const ANumber	kBottomPadding_TextWithOvalHover = 2;

//
const ANumber	kWidth_MenuTriangle = 8;
//


//描画要求時のコールバック(AView用)
void	AView_Button::EVTDO_DoDraw()
{
	//未初期化なら初期化
	InitTextPropertyIfNotInited();
	
	ALocalRect	frame;
	NVM_GetLocalViewRect(frame);
	//（オーバーレイの場合は）透明色で全体消去 win
	NVMC_EraseRect(frame);
	
	ABool	active = (NVM_GetWindow().NVI_IsWindowActive() == true || NVM_GetWindow().NVI_IsFloating() == true);
	if( mAlwaysActiveColors == true )//#291
	{
		active = true;
	}
	if( NVMC_IsControlEnabled() == false )//#390
	{
		active = false;
	}
	if( (mTextStyle&kTextStyle_DropShadow) != 0 )
	{
		NVMC_SetDropShadowColor(mDropShadowColor);
	}
	
	//==================Ovalホバー付きテキストタイプボタン==================
	//Ovalホバー付きテキストボタン専用処理
	if( mButtonViewType == kButtonViewType_TextWithOvalHover || 
				mButtonViewType == kButtonViewType_TextWithOvalHoverWithFixedWidth ||
				mButtonViewType == kButtonViewType_TextWithNoHoverWithFixedWidth )
	{
		//------------------テキスト描画領域計算------------------
		ALocalRect	textrect = frame;
		textrect.left	= frame.left + kLeftPadding_TextWithOvalHover -1;
		textrect.top	= (frame.top+frame.bottom)/2 - (mFontHeight+1)/2;
		textrect.right	= frame.right - kRightPadding_TextWithOvalHover +1;
		textrect.bottom	= (frame.top+frame.bottom)/2 + (mFontHeight+1)/2;
		
		//文字色
		AColor	lettercolor = mColor;
		if( active == false )
		{
			lettercolor = mColorDeactive;
		}
		
		//------------------メニュー下三角形描画判定------------------
		ABool	showMenuTriangle = false;
		if( mMenuID != kIndex_Invalid || mMenuTextArray.GetItemCount() > 0 )
		{
			showMenuTriangle = true;
			//テキスト描画領域変更
			textrect.right -= kWidth_MenuTriangle;
		}
		
		//------------------ホバー描画------------------
		if( mButtonViewType == kButtonViewType_TextWithOvalHover || 
					mButtonViewType == kButtonViewType_TextWithOvalHoverWithFixedWidth )
		{
			//ホバー中、または、コンテキストメニュー表示中に、ホバー表示する
			if( mMouseHover == true || mDisplayingContextMenu == true || mToggleOn == true )
			{
				//描画rect取得
				ALocalRect	ovalrect = {0};
				GetOvalRect(ovalrect);
				//ホバー描画
				NVMC_PaintRoundedRect(ovalrect,mOvalHoverColor,mOvalHoverColor,kGradientType_None,1.0,1.0,
									  kNumber_MaxNumber,true,true,true,true);
				//ドロップシャドウ色設定
				if( active == true )
				{
					NVMC_SetDropShadowColor(kColor_Gray30Percent);
				}
				else
				{
					NVMC_SetDropShadowColor(kColor_White);
				}
				//文字色設定
				lettercolor = kColor_White;
			}
			/*#
			else
			{
			//ホバー中でない場合の文字色設定
			if( active == false )
			{
			lettercolor = mColorDeactive;
			}
			}
			*/
		}
		
		//------------------アイコン描画------------------
		//アイコンイメージが存在していたら、アイコンを描画する
		AImageID	iconImageID = mIconImageID;
		if( mToggleOn == true && mToggleOnIconImageID != kImageID_Invalid )
		{
			iconImageID = mToggleOnIconImageID;
		}
		if( iconImageID != kImageID_Invalid )
		{
			//アイコン描画
			ALocalPoint	pt = {0};
			pt.x = textrect.left;
			pt.y = (textrect.top+textrect.bottom)/2 - (NVMC_GetImageHeight(iconImageID))/2;
			NVMC_DrawImage(iconImageID,pt);
			//テキスト描画領域変更
			textrect.left += NVMC_GetImageWidth(iconImageID) + kSpaceBetweenIconAndText;
		}
		
		//テキスト取得
		AText	text;
		text.SetText(mText);
		//Canonical Compへ変換（ファイル名由来データの場合、濁点等が分離している場合があり、そのままだと分離表示されるため）
		text.ConvertToCanonicalComp();
		//ellipsisテキスト取得
		switch( mEllipsisMode )
		{
		  case kEllipsisMode_FixedFirstCharacters:
			{
				NVI_GetEllipsisTextWithFixedFirstCharacters(mText,textrect.right-textrect.left,mEllipsisCharacterCount,text);
				break;
			}
		  case kEllipsisMode_FixedLastCharacters:
			{
				NVI_GetEllipsisTextWithFixedLastCharacters(mText,textrect.right-textrect.left,mEllipsisCharacterCount,text);
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		//ヘルプタグ設定
		NVI_SetEnableHelpTag(mText.Compare(text)==false);
		//一時的非表示のときは表示しない
		if( mTemporaryInvisible == false )
		{
			//------------------テキスト描画------------------
			NVMC_DrawText(textrect,text,lettercolor,mTextStyle,mTextJustification);
			//NVMC_FrameRect(textrect,kColor_Red);
		}
		
		//------------------メニュー下三角形の描画------------------
		if( showMenuTriangle == true )
		{
			ANumber	drawntextwidth = NVMC_GetDrawTextWidth(text);
			//描画
			ALocalPoint	pt = {textrect.left + drawntextwidth +5 , (textrect.top+textrect.bottom)/2 - 8};
			NVMC_DrawImage(kImageID_iconSwTriangles,pt);
		}
		
		return;
	}
	
	//#724 #725
	//==================通常ボタン（ホバーボタン以外）==================
	switch(mButtonViewType)
	{
	  case kButtonViewType_Rect20:
		{
			ALocalPoint	pt = {frame.left,frame.top};
			if( mMouseDown == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20Rect_p_1,kImageID_btn20Rect_p_2,kImageID_btn20Rect_p_3,pt,frame.right-frame.left);
			}
			else if( mMouseHover == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20Rect_h_1,kImageID_btn20Rect_h_2,kImageID_btn20Rect_h_3,pt,frame.right-frame.left);
			}
			else
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20Rect_1,kImageID_btn20Rect_2,kImageID_btn20Rect_3,pt,frame.right-frame.left);
			}
			break;
		}
	  case kButtonViewType_RoundedRect20:
		{
			ALocalPoint	pt = {frame.left,frame.top};
			if( mMouseDown == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20RoundedRect_p_1,kImageID_btn20RoundedRect_p_2,kImageID_btn20RoundedRect_p_3,pt,frame.right-frame.left);
			}
			else if( mMouseHover == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20RoundedRect_h_1,kImageID_btn20RoundedRect_h_2,kImageID_btn20RoundedRect_h_3,pt,frame.right-frame.left);
			}
			else
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn20RoundedRect_1,kImageID_btn20RoundedRect_2,kImageID_btn20RoundedRect_3,pt,frame.right-frame.left);
			}
			break;
		}
	  case kButtonViewType_Rect16:
		{
			ALocalPoint	pt = {frame.left,frame.top};
			if( mMouseDown == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn16Rect_p_1,kImageID_btn16Rect_p_2,kImageID_btn16Rect_p_3,pt,frame.right-frame.left);
			}
			else if( mMouseHover == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn16Rect_h_1,kImageID_btn16Rect_h_2,kImageID_btn16Rect_h_3,pt,frame.right-frame.left);
			}
			else
			{
				NVMC_DrawImageFlexibleWidth(kImageID_btn16Rect_1,kImageID_btn16Rect_2,kImageID_btn16Rect_3,pt,frame.right-frame.left);
			}
			break;
		}
	  case kButtonViewType_Rect16Footer:
		{
			ALocalPoint	pt = {frame.left,frame.top};
			if( mMouseDown == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_panel16Footer_1,kImageID_panel16Footer_2,kImageID_panel16Footer_3,pt,frame.right-frame.left);
			}
			else if( mMouseHover == true )
			{
				NVMC_DrawImageFlexibleWidth(kImageID_panel16Footer_1,kImageID_panel16Footer_2,kImageID_panel16Footer_3,pt,frame.right-frame.left);
			}
			else
			{
				NVMC_DrawImageFlexibleWidth(kImageID_panel16Footer_1,kImageID_panel16Footer_2,kImageID_panel16Footer_3,pt,frame.right-frame.left);
			}
			break;
		}
	  case kButtonViewType_ScrollBar:
		{
			ALocalPoint	pt = {frame.left,frame.top};
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
			{
				if( mMouseDown == true )
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_107,pt);
				}
				else if( mMouseHover == true )
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_107,pt);
				}
				else
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_107,pt);
				}
			}
			else
			{
				if( mMouseDown == true )
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_106,pt);
				}
				else if( mMouseHover == true )
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_106,pt);
				}
				else
				{
					NVMC_DrawImage(kImageID_frameScrlbarTop_106,pt);
				}
			}
			break;
		}
	  case kButtonViewType_NoFrame:
	  case kButtonViewType_NoFrameWithTextHover:
		{
			//処理無し
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
#if 0
	//
	if( active == true )
	{
		if( mMouseHover == false || mToggleOn == true )
		{
			//通常状態 
			if( mToggleOn == false )
			{
				//#597
				/*
				//ボタン上半分表示
				ALocalRect	f1 = frame;
				f1.bottom = f1.top + (frame.bottom-frame.top)/2;
				NVMC_PaintRect(f1,kColor_Gray94Percent,mTransparency);//#291
				//ボタン下半分gradient表示
				ALocalRect	f2 = frame;
				f2.top = f1.bottom;
				AColor	startColor = kColor_Gray93Percent;
				AColor	endColor = kColor_Gray90Percent;
				NVMC_PaintRectWithVerticalGradient(f2,startColor,endColor,mTransparency,true);
				*/
				{
					ALocalPoint	pt = {frame.left,frame.top};
					NVMC_DrawImageFlexibleWidth(300,301,302/*kImageID_RectButton*/,pt,frame.right-frame.left);
				}
				//ボタン外周描画
				/*
				switch(mButtonViewType)
				{
				  case kButtonViewType_Rect:
					{
						NVMC_FrameRect(frame,mFrameColor);//amazon kColor_Gray80Percent);//#291 kColor_Gray70Percent);
						break;
					}
					//#634
				  case kButtonViewType_RoundedCorner:
					{
						ALocalPoint	spt, ept;
						spt.x = frame.left+1;
						spt.y = frame.top;
						ept.x = frame.right-1;
						ept.y = frame.top;
						NVMC_DrawLine(spt,ept,mFrameColor);
						spt.x = frame.left+1;
						spt.y = frame.bottom-1;
						ept.x = frame.right-1;
						ept.y = frame.bottom-1;
						NVMC_DrawLine(spt,ept,mFrameColor);
						spt.x = frame.left;
						spt.y = frame.top;
						ept.x = frame.left;
						ept.y = frame.bottom-2;
						NVMC_DrawLine(spt,ept,mFrameColor);
						spt.x = frame.right-1;
						spt.y = frame.top;
						ept.x = frame.right-1;
						ept.y = frame.bottom-2;
						NVMC_DrawLine(spt,ept,mFrameColor);
						break;
					}
				}
				
				ALocalPoint	spt, ept;
				spt.x = frame.left+1;
				spt.y = frame.top+1;
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_White);
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_White);
				spt.x = frame.right-2;
				spt.y = frame.bottom-2;
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
				*/
			}
			else
			{
				/*win トグルon時の描画変更
				NVMC_PaintRect(frame,kColor_Gray80Percent,mTransparency);//#291
				NVMC_FrameRect(frame,mFrameColor);//amazon kColor_Gray80Percent);//#291 kColor_Gray70Percent);
				ALocalPoint	spt, ept;
				spt.x = frame.left+1;
				spt.y = frame.top+1;
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_Gray70Percent);
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_Gray70Percent);
				spt.x = frame.right-2;
				spt.y = frame.bottom-2;
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_White);
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_White);
				*/
				AColor	hilightColor;
				AColorWrapper::GetHighlightColor(hilightColor);
				NVMC_PaintRect(frame,kColor_Gray92Percent);
				NVMC_PaintRect(frame,hilightColor,0.7);
				NVMC_FrameRect(frame,hilightColor);
				ALocalPoint	spt, ept;
				spt.x = frame.left+1;
				spt.y = frame.top+1;
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_White);
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_White);
				spt.x = frame.right-2;
				spt.y = frame.bottom-2;
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
			}
		}
		else
		{
			//マウスオーバー
			if( mMouseDown == true )
			{
				//マウスダウン中のマウスオーバー 
				AColor	hilightColor;
				AColorWrapper::GetHighlightColor(hilightColor);
				NVMC_PaintRect(frame,kColor_Gray92Percent);
				NVMC_PaintRect(frame,hilightColor,0.7);
				NVMC_FrameRect(frame,hilightColor);
				ALocalPoint	spt, ept;
				spt.x = frame.left+1;
				spt.y = frame.top+1;
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_White);
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_White);
				spt.x = frame.right-2;
				spt.y = frame.bottom-2;
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
			}
			else
			{
				//通常のマウスオーバー 
				AColor	hilightColor;
				AColorWrapper::GetHighlightColor(hilightColor);
				NVMC_PaintRect(frame,kColor_Gray92Percent);
				NVMC_PaintRect(frame,hilightColor,0.2);
				NVMC_FrameRect(frame,hilightColor);
				ALocalPoint	spt, ept;
				spt.x = frame.left+1;
				spt.y = frame.top+1;
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_White);
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_White);
				spt.x = frame.right-2;
				spt.y = frame.bottom-2;
				ept.x = frame.right-2;
				ept.y = frame.top+1;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
				ept.x = frame.left+1;
				ept.y = frame.bottom-2;
				NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
			}
		}
	}
	//ウインドウがdeactive
	else
	{
		NVMC_PaintRect(frame,kColor_Gray92Percent);
		NVMC_FrameRect(frame,kColor_Gray80Percent);
		ALocalPoint	spt, ept;
		spt.x = frame.left+1;
		spt.y = frame.top+1;
		ept.x = frame.left+1;
		ept.y = frame.bottom-2;
		NVMC_DrawLine(spt,ept,kColor_White);
		ept.x = frame.right-2;
		ept.y = frame.top+1;
		NVMC_DrawLine(spt,ept,kColor_White);
		spt.x = frame.right-2;
		spt.y = frame.bottom-2;
		ept.x = frame.right-2;
		ept.y = frame.top+1;
		NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
		ept.x = frame.left+1;
		ept.y = frame.bottom-2;
		NVMC_DrawLine(spt,ept,kColor_Gray90Percent);
	}
#endif
	
	//------------------描画領域計算------------------
	ALocalRect	drawrect = frame;
	drawrect.left += 3;
	drawrect.top = (frame.top+frame.bottom)/2 - mFontHeight/2 -1;
	drawrect.right -= 3;
	drawrect.bottom = (frame.top+frame.bottom)/2 + mFontHeight/2 +1;
	
	//------------------アイコン表示幅取得------------------
	ANumber	iconImageWidth = 0;
	if( mIconImageID != kImageID_Invalid )
	{
		iconImageWidth = mIconWidth+4;
	}
	
	//------------------メニュー▼表示幅取得------------------
	ANumber	menuTriangleWidth = 0;
	if( mMenuID != kIndex_Invalid || mMenuTextArray.GetItemCount() > 0 )
	{
		menuTriangleWidth = kWidth_MenuTriangle + 4;
	}
	
	//------------------センタリングなら描画領域を中央寄せにする------------------
	switch(mTextJustification)
	{
	  case kJustification_Center:
	  case kJustification_CenterTruncated:
		{
			ANumber	textwidth = NVMC_GetDrawTextWidth(mText);
			if( drawrect.right - drawrect.left > textwidth + iconImageWidth + menuTriangleWidth )
			{
				ANumber	m = ((drawrect.left+iconImageWidth)+(drawrect.right-menuTriangleWidth))/2;
				drawrect.left	= m - textwidth/2 - iconImageWidth -2;
				drawrect.right	= m + textwidth/2 + menuTriangleWidth +2;
				//NVMC_FrameRect(drawrect,kColor_Black);
			}
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	
	//------------------アイコン描画------------------
	if( iconImageWidth > 0 )
	{
		switch(mTextJustification)
		{
		  case kJustification_Center:
		  case kJustification_CenterTruncated:
			{
				//
				ALocalPoint	pt = {drawrect.left,mIconTopOffset};
				if( mMouseHover == true && mHoverIconImageID != kImageID_Invalid )
				{
					NVMC_DrawImage(mHoverIconImageID,pt);
				}
				else
				{
					NVMC_DrawImage(mIconImageID,pt);
				}
				break;
			}
		  default:
			{
				//
				ALocalPoint	pt = {mIconLeftOffset,mIconTopOffset};
				if( mIconLeftOffset < 0 )
				{
					pt.x = frame.right+mIconLeftOffset;
				}
				if( mMouseHover == true && mHoverIconImageID != kImageID_Invalid )
				{
					NVMC_DrawImage(mHoverIconImageID,pt);
				}
				else
				{
					NVMC_DrawImage(mIconImageID,pt);
				}
				break;
			}
		}
	}
	//------------------Menu用▼描画------------------
	if( menuTriangleWidth > 0 )
	{
		ALocalPoint	pt = {drawrect.right - kWidth_MenuTriangle,(drawrect.top+drawrect.bottom)/2 - 7};
		NVMC_DrawImage(kImageID_iconSwTriangles,pt);
	}
	//------------------テキスト描画------------------
	if( mText.GetItemCount() > 0 )
	{
		//
		ALocalRect	textrect = drawrect;
		textrect.left += iconImageWidth;
		textrect.right -= menuTriangleWidth;
		//テキスト取得
		AText	text;
		text.SetText(mText);
		//Canonical Compへ変換（ファイル名由来データの場合、濁点等が分離している場合があり、そのままだと分離表示されるため）
		text.ConvertToCanonicalComp();
		//ellipsisテキスト取得
		switch( mEllipsisMode )
		{
		  case kEllipsisMode_FixedFirstCharacters:
			{
				NVI_GetEllipsisTextWithFixedFirstCharacters(mText,textrect.right-textrect.left,mEllipsisCharacterCount,text);
				break;
			}
		  case kEllipsisMode_FixedLastCharacters:
			{
				NVI_GetEllipsisTextWithFixedLastCharacters(mText,textrect.right-textrect.left,mEllipsisCharacterCount,text);
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		//色取得
		AColor	color = mColor;
		//
		if( mButtonViewType == kButtonViewType_NoFrameWithTextHover )
		{
			if( mMouseHover == true )
			{
				//AColorWrapper::GetHighlightColor(color);
				color = kColor_Blue;
			}
		}
		
		if( active == false )
		{
			color = mColorDeactive;
		}
		
		//
		switch(mButtonViewType)
		{
		  case kButtonViewType_Rect16:
			{
				if( active == true )
				{
					color = kColor_Gray10Percent;
				}
				else
				{
					color = kColor_Gray60Percent;
				}
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		
		//テキストスタイル取得（active時はドロップシャドウ）
		ATextStyle	style = kTextStyle_DropShadow;
		if( active == false )
		{
			style = kTextStyle_Normal;
		}
		
		//テキスト描画
		NVMC_DrawText(textrect,text,color,style,mTextJustification);
	}
	//fprintf(stderr,"AView_Button::EVTDO_DoDraw()");
}

/**
Ovalボタンの場合のOval領域取得
*/
void	AView_Button::GetOvalRect( ALocalRect& outOvalRect ) const
{
	ALocalRect	frame = {0};
	NVM_GetLocalViewRect(frame);
	
	//半円部分の幅合計
	const ANumber	kWidth_OvalWidth = kLeftPadding_TextWithOvalHover + kRightPadding_TextWithOvalHover;
	
	//テキスト表示幅取得
	ANumber	textwidth = NVMC_GetDrawTextWidth(mText);
	
	//メニュー▼表示判定
	ABool	showMenuTriangle = false;
	if( mMenuID != kIndex_Invalid || mMenuTextArray.GetItemCount() > 0 )
	{
		showMenuTriangle = true;
	}
	
	//
	outOvalRect = frame;
	//
	outOvalRect.top += 1;
	outOvalRect.bottom -= 1;
	//固定幅タイプの場合は、テキスト幅に合わせて幅設定
	if( mButtonViewType == kButtonViewType_TextWithOvalHoverWithFixedWidth ||
				mButtonViewType == kButtonViewType_TextWithNoHoverWithFixedWidth )
	{
		//メニュー下三角形部分の幅取得
		ANumber	iconsWidth = 0;
		if( showMenuTriangle == true )
		{
			iconsWidth = kWidth_MenuTriangle;
		}
		if( mIconImageID != kImageID_Invalid )
		{
			//アイコン表示幅
			iconsWidth += mIconWidth;
		}
		//テキスト幅＋メニュー下三角形幅よりも、ovalrect幅が大きいときは、テキスト幅に合わせて幅設定する
		if( outOvalRect.right - outOvalRect.left > textwidth + iconsWidth + kWidth_OvalWidth )
		{
			//ovalrect設定
			outOvalRect.right = outOvalRect.left + textwidth + iconsWidth + kWidth_OvalWidth;
		}
	}
}

/**
マウス移動時のOSイベントコールバック

@param inLocalPoint マウスカーソルの場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
ABool	AView_Button::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( NVMC_IsControlEnabled() == true )
	{
		//#634 ボタン有効範囲
		if( mButtonValidXRangeStart != -1 && mButtonValidXRangeEnd != -1 )
		{
			if( inLocalPoint.x < mButtonValidXRangeStart || inLocalPoint.x > mButtonValidXRangeEnd )
			{
				return false;
			}
		}
		
		mMouseHover = true;
		NVI_Refresh();
	}
	return true;
}

/**
マウスがView外へ移動した時のOSイベントコールバック

@param inLocalPoint マウスカーソルの場所（コントロールローカル座標）
*/
void	AView_Button::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	mMouseHover = false;
	NVI_Refresh();
}

/**
右クリック押下時のOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
@param inClickCout クリック回数
*/
ABool	AView_Button::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( mContextMenuID == kIndex_Invalid )   return false;
	mMouseHover = false;
	NVI_Refresh();
	//ボタンのControlID保存（WindowのEVTDO_DoMenuItemSelected()でどのボタンが押されたかを判定するため）
	NVM_GetWindow().NVI_SetLastClickedButtonControlID(NVI_GetControlID());
	//ContextMenu Enable/Disable設定
	for( AIndex i = 0; i < mContextMenuEnableArray.GetItemCount(); i++ )
	{
		AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuEnableMenuItem(mContextMenuID,i,mContextMenuEnableArray.Get(i));
	}
	//ContextMenu表示
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	//#724
	mDisplayingContextMenu = true;
	NVI_Refresh();
	//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(mContextMenuID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	NVMC_ShowContextMenu(mContextMenuID,globalPoint);//#688
	//#724
	mDisplayingContextMenu = false;
	//コンテキストメニューの実行の結果、コントロールが削除されている可能性があるので、チェックする。
	if( NVM_GetWindow().NVI_ExistControl(NVI_GetControlID()) == true )
	{
		NVI_Refresh();
	}
	//
	return true;
}

//#379
/**
ヘルプタグ
*/
ABool	AView_Button::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	//Ovalボタン、フレーム無しボタンのときはヘルプタグは表示しない
	switch(mButtonViewType)
	{
	  case kButtonViewType_TextWithOvalHover:
	  case kButtonViewType_NoFrame:
	  case kButtonViewType_NoFrameWithTextHover:
		{
			return false;
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//#661 SPI_SetHelpTag()で設定されたテキストがあるときは、そちら優先
	if( mHelpTagText1.GetItemCount() > 0 )
	{
		outText1.SetText(mHelpTagText1);
		outText2.SetText(mHelpTagText2);
		outTagSide = mHelpTagSide;
	}
	else
	{
		outText1.SetText(mText);
		outText2.SetText(mText);
		outTagSide = kHelpTagSide_Default;//#643
	}
	NVM_GetLocalViewRect(outRect);
	return true;
}

//#634
/**
マウスカーソルタイプ
*/
ACursorType	AView_Button::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//ボタン有効範囲
	if( mButtonValidXRangeStart != -1 && mButtonValidXRangeEnd != -1 )
	{
		if( inLocalPoint.x < mButtonValidXRangeStart || inLocalPoint.x > mButtonValidXRangeEnd )
		{
			return kCursorType_Arrow;
		}
	}
	
	//
	ACursorType	cursorType = kCursorType_Arrow;
	switch(mButtonBehaviorType)
	{
	  case kButtonBehaviorType_VSeparator:
		{
			cursorType = kCursorType_ResizeLeftRight;
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return cursorType;
}

/**
*/
void	AView_Button::NVIDO_SetShow( const ABool inShow )
{
	//mMouseHover = false;
}

//win
/**
*/
void	AView_Button::NVIDO_SetBool( const ABool inBool )
{
	if( mToggleMode == true )
	{
		mToggleOn = inBool;
		NVI_Refresh();
	}
}

//win
/**
*/
ABool	AView_Button::NVIDO_GetBool() const
{
	return mToggleOn;
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

/**
アイコン設定
*/
void	AView_Button::SPI_SetIcon( const AImageID inIconImageID, 
								  const ANumber inLeftOffset, const ANumber inTopOffset,
								  const ANumber inWidth, const ANumber inHeight, const ABool inRefresh, 
								  const AImageID inHoverIconImageID,//#530
								  const AImageID inToggleOnImageID )
{
	mIconImageID = inIconImageID;
	mHoverIconImageID = inHoverIconImageID;
	mToggleOnIconImageID = inToggleOnImageID;
	mIconLeftOffset = inLeftOffset;
	mIconTopOffset = inTopOffset;
	mIconWidth = inWidth;
	mIconHeight = inHeight;
	if( inRefresh == true )//#530
	{
		NVI_Refresh();
	}
}

/**
アイコンImage設定
*/
void	AView_Button::SPI_SetIconImageID( AImageID inIconImageID, 
										 const AImageID inHoverIconImageID,
										 const AImageID inToggleOnImageID )
{
	mIconImageID = inIconImageID;
	mHoverIconImageID = inHoverIconImageID;
	mToggleOnIconImageID = inToggleOnImageID;
	mIconLeftOffset = 0;
	mIconTopOffset = 0;
	mIconWidth = NVMC_GetImageWidth(mIconImageID);
	mIconHeight = NVMC_GetImageHeight(mIconImageID);
}

/**
テキスト属性設定
*/
void	AView_Button::SPI_SetTextProperty( const AText& inFontName, const AFloatNumber inFontSize, const AJustification inJustification, 
		const ATextStyle inTextStyle,//#724
		const AColor inColor, const AColor inColorDeactive, const ABool inRefresh )//#530
{
	mFontName.SetText(inFontName);
	mFontSize = inFontSize;
#if IMPLEMENTATION_FOR_WINDOWS
	mFontSize *= 0.8;
	if( mFontSize < 7.8 )   mFontSize = 7.8;
#endif
	mTextStyle = inTextStyle;//#724
	mColor = inColor;
	mColorDeactive = inColorDeactive;
	NVMC_SetDefaultTextProperty(inFontName,mFontSize,inColor,mTextStyle,true);
	NVMC_GetMetricsForDefaultTextProperty(mFontHeight,mFontAscent);
	mTextJustification = inJustification;
	//#724
	if( mButtonViewType == kButtonViewType_TextWithOvalHover )
	{
		SPI_SetWidthToFitTextWidth();
	}
	//
	if( inRefresh == true )//#530
	{
		NVI_Refresh();
	}
}

//#724
/**
テキスト幅に合わせてボタン幅設定
*/
void	AView_Button::SPI_SetWidthToFitTextWidth( const ANumber inMaxWidth )
{
	ANumber	width = NVMC_GetDrawTextWidth(mText)+kLeftPadding_TextWithOvalHover+kRightPadding_TextWithOvalHover;
	if( mMenuID != kIndex_Invalid || mMenuTextArray.GetItemCount() > 0 )
	{
		width += kWidth_MenuTriangle;
	}
	if( mIconImageID != kImageID_Invalid )
	{
		//アイコン表示幅
		width += NVMC_GetImageWidth(mIconImageID) + kSpaceBetweenIconAndText;
	}
	if( width > inMaxWidth )
	{
		width = inMaxWidth;
	}
	NVI_SetSize(width,NVI_GetHeight());
}

/**
テキスト設定
*/
void	AView_Button::NVIDO_SetText( const AText& inText )
{
	if( mText.Compare(inText) == false )
	{
		mText.SetText(inText);
		
		NVI_Refresh();
	}
}

//#379
/**
テキスト取得
*/
void	AView_Button::NVIDO_GetText( AText& outText ) const
{
	outText.SetText(mText);
}

/**
テキスト設定
*/
void	AView_Button::SPI_SetTextFromMenuIndex( const AIndex inIndex )
{
	if( mMenuID == kIndex_Invalid )   {_ACError("",this);return;}
	AText	text;
	AApplication::GetApplication().NVI_GetMenuManager().GetContextMenuItemText(mMenuID,inIndex,text);
	NVI_SetText(text);
}

/**
メニュー設定（ContextMenuを本クラス外で設定する場合用）

ContextMenuにTextArrayMenuを設定している場合などに使う
*/
void	AView_Button::SPI_SetMenu( const AContextMenuID inContextMenuID, const AMenuItemID inMenuItemID )
{
	mMenuID = inContextMenuID;
	mMenuItemID = inMenuItemID;
}

/**
メニュー設定（TextArrayを設定する場合用）

固定のTextArray
*/
void	AView_Button::SPI_SetMenuTextArray( const ATextArray& inTextArray, const ATextArray& inActionTextArray, const AMenuItemID inMenuItemID )
{
	mMenuTextArray.SetFromTextArray(inTextArray);
	mActionTextArray.SetFromTextArray(inActionTextArray);
	mMenuItemID = inMenuItemID;
}

/**
ボタンをトグルモードにする
*/
void	AView_Button::SPI_SetToggleMode( const ABool inToggleMode )
{
	mToggleMode = inToggleMode;
	mToggleOn = false;
	NVI_Refresh();
}

/**
ボタンの現在のトグル状態を取得する
*/
void	AView_Button::SPI_SetToogleOn( const ABool inToggleOn )
{
	if( mToggleOn == inToggleOn )   return;//#695
	mToggleOn = inToggleOn;
	NVI_Refresh();
}

/**
ボタンをトグルOnの状態に設定する
*/
ABool	AView_Button::SPI_GetToggleOn() const
{
	return mToggleOn;
}

/**
右クリックメニュー設定
*/
void	AView_Button::SPI_SetContextMenu( const AContextMenuID inContextMenuID )
{
	mContextMenuID = inContextMenuID;
	mContextMenuEnableArray.DeleteAll();
	for( AIndex i = 0; i < AApplication::GetApplication().NVI_GetMenuManager().GetContextMenuItemCount(inContextMenuID); i++ )
	{
		mContextMenuEnableArray.Add(true);
	}
}

/**
コンテキストメニューのEnable/Disableを設定する
*/
void	AView_Button::SPI_SetContextMenuEnableItem( const AIndex inIndex, const ABool inEnable )
{
	mContextMenuEnableArray.Set(inIndex,inEnable);
}

//#634
/**
ボタン動作タイプ設定
*/
void	AView_Button::SPI_SetButtonBehaviorType( const AButtonBehaviorType inButtonType )
{
	mButtonBehaviorType = inButtonType;
}

//#634
/**
ボタン有効範囲設定
*/
void	AView_Button::SPI_SetButtonValidXRange( const ANumber inStartX, const ANumber inEndX )
{
	mButtonValidXRangeStart = inStartX;
	mButtonValidXRangeEnd = inEndX;
}

//#661
/**
ヘルプタグ設定
*/
void	AView_Button::SPI_SetHelpTag( const AText& inText1, const AText& inText2, const AHelpTagSide inTagSide )
{
	switch(mButtonViewType)
	{
	  case kButtonViewType_TextWithOvalHover:
	  case kButtonViewType_NoFrame:
	  case kButtonViewType_NoFrameWithTextHover:
		{
			return;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	mHelpTagText1.SetText(inText1);
	mHelpTagText2.SetText(inText2);
	mHelpTagSide = inTagSide;
	NVM_GetWindow().NVI_EnableHelpTagCallback(NVI_GetControlID(),false);
}

//#724
/**
ボタンviewタイプ設定
*/
void	AView_Button::SPI_SetButtonViewType( const AButtonViewType inType )
{
	mButtonViewType = inType;
	switch(mButtonViewType)
	{
	  case kButtonViewType_TextWithOvalHover:
	  case kButtonViewType_NoFrame:
	  case kButtonViewType_NoFrameWithTextHover:
		{
			mHelpTagText1.DeleteAll();
			mHelpTagText2.DeleteAll();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//
	NVI_Refresh();
}

#pragma mark ===========================

#pragma mark --- VSeprator type
//#634

/**
VSeparatorタイプ　マウスダウン時動作
*/
ABool	AView_Button::DoMouseDown_VSeparator( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//前回マウス位置の保存
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,mPreviousMouseGlobalPoint);
	
	//Windowsの場合は、ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
	//#688 if( mMouseTrackByLoop == false )
	{
		//TrackingMode設定
		NVM_SetMouseTrackingMode(true);
		return false;
	}
	/*#688
	//マウスボタンが放されるまでループ
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//ローカル座標に変換
		ALocalPoint	mouseLocalPoint;
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		//
		DoMouseTrack_VSeparator(mouseLocalPoint);
	}
	//完了通知 #409
	NVM_GetWindow().NVI_SeparatorMoved(NVM_GetWindowID(),NVI_GetControlID(),0,true);
	return false;
	*/
}

/**
VSeparatorタイプ　マウストラック時動作
*/
void	AView_Button::DoMouseTrack_VSeparator( const ALocalPoint& inLocalPoint )
{
	//グローバル座標に変換
	AGlobalPoint	mouseGlobalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,mouseGlobalPoint);
	//前回マウス位置と同じなら何もせずループ継続
	if( mouseGlobalPoint.x == mPreviousMouseGlobalPoint.x && mouseGlobalPoint.y == mPreviousMouseGlobalPoint.y )
	{
		return;
	}
	//TargetWindowへSeparator移動を通知
	ANumber	widthDelta = mouseGlobalPoint.x-mPreviousMouseGlobalPoint.x;
	ANumber	resultDelta = NVM_GetWindow().NVI_SeparatorMoved(NVM_GetWindowID(),NVI_GetControlID(),widthDelta,false);//#409
	mouseGlobalPoint.x = mPreviousMouseGlobalPoint.x + resultDelta;
	//前回マウス位置の保存
	mPreviousMouseGlobalPoint = mouseGlobalPoint;
}

/**
VSeparatorタイプ　マウストラック終了時動作
*/
void	AView_Button::DoMouseTrackEnd_VSeparator( const ALocalPoint& inLocalPoint )
{
	//完了通知 #409
	NVM_GetWindow().NVI_SeparatorMoved(NVM_GetWindowID(),NVI_GetControlID(),0,true);
	//TrackingMode解除（DoMouseTrackEnd()の前にしないと、DoMouseTrackEnd()でSeparatorコントロールが削除されてしまうので）
	NVM_SetMouseTrackingMode(false);
}


