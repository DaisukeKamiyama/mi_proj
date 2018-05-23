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

AWindow_KeyToolList
R0073
*/

#include "stdafx.h"

#include "AWindow_KeyToolList.h"
#include "AView_KeyToolList.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowSizeBox.h"

const AFloatNumber	kFontSize = 9.0;

//#725
//List view, scrollbar
const AControlID	kControlID_ListView = 101;
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kVScrollBarWidth = 11;

//#725
//サイズボックス
const AControlID	kControlID_SizeBox = 903;
const ANumber	kWidth_SizeBox = 15;
const ANumber	kHeight_SizeBox = 15;

//#725
//SubWindow用タイトルバーview
const AControlID	kControlID_TitleBar = 901;

//#725
//SubWindow用セパレータview
const AControlID	kControlID_Separator = 902;

//#725
//背景色描画用view
const AControlID	kControlID_Background = 905;

#pragma mark ===========================
#pragma mark [クラス]AWindow_KeyToolList
#pragma mark ===========================
//情報ウインドウのクラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_KeyToolList::AWindow_KeyToolList():AWindow(/*#175NULL*/)
,mPreferBelow(true)
{
	//#138 デフォルト範囲(10000～)を使う限り設定不要 NVM_SetDynamicControlIDRange(kDynamicControlIDStart,kDynamicControlIDEnd);
}
//デストラクタ
AWindow_KeyToolList::~AWindow_KeyToolList()
{
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_KeyToolList::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 10;
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

//TextView取得
AView_KeyToolList&	AWindow_KeyToolList::GetKeyToolListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_KeyToolList,kViewType_KeyToolList,mKeyToolListViewControlID);
	/*#199
	if( NVI_GetViewByControlID(mKeyToolListViewControlID).NVI_GetViewType() != kViewType_KeyToolList )   _ACThrow("",this);
	return dynamic_cast<AView_KeyToolList&>(NVI_GetViewByControlID(mKeyToolListViewControlID));
	*/
}

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_KeyToolList::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//コントロールのクリック時のコールバック
ABool	AWindow_KeyToolList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	return result;
}

//ウインドウサイズ変更通知時のコールバック
void	AWindow_KeyToolList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

/**
ウインドウの閉じるボタン
*/
void	AWindow_KeyToolList::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}


#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

/**
メニュー選択時処理
*/
ABool	AWindow_KeyToolList::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			GetApp().SPI_CloseSubWindow(GetObjectID());
			result = true;
			break;
		}
		//その他のmenu item IDの場合、このクラスで処理せず、次のコマンド対象で処理する
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

/**
メニュー更新
*/
void	AWindow_KeyToolList::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

//ウインドウ生成
void	AWindow_KeyToolList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ生成
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//ポップアップ
	  case kSubWindowLocationType_Popup:
		{
			//
			//※シャドウはfalse。シャドウを入れると、背景色アルファが薄い場合に、文字にシャドウがついて汚くなる。
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
			//サイズ設定
			NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kKeyToolListWindowWidth));
			NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kKeyToolListWindowHeight));
			break;
		}
		//サイドバー
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Bottom,false,false,false,false,false,false,false);
			break;
		}
		//フローティング
	  default:
		{
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,false,false,true,false);
			break;
		}
	}
	
	//背景色描画用view生成
	{
		AViewDefaultFactory<AView_SubWindowBackground>	viewFactory(GetObjectID(),kControlID_Background);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Background,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Background,true);
		NVI_SetControlBindings(kControlID_Background,true,true,true,true,false,false);
	}
	
	//SubWindow用タイトルバー生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_ToolList");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwTool);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//SizeBox生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
}

//新規タブ生成
//引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
void	AWindow_KeyToolList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//listview生成
	{
		AWindowPoint	pt = {0,0}; 
		mKeyToolListViewControlID = NVM_AssignDynamicControlID();
		AViewDefaultFactory<AView_KeyToolList>	keyToolListViewFactory(GetObjectID(),mKeyToolListViewControlID);//#199
		NVM_CreateView(mKeyToolListViewControlID,pt,500,500,kControlID_Invalid,kControlID_Invalid,false,keyToolListViewFactory);
		NVI_SetControlBindings(mKeyToolListViewControlID,true,true,true,true,false,false);
	}
	//Vスクロールバー生成
	{
		AWindowPoint	pt = {0,0}; 
		mVScrollBarControlID = NVM_AssignDynamicControlID();
		NVI_CreateScrollBar(mVScrollBarControlID,pt,kVScrollBarWidth,kVScrollBarWidth*2);
		GetKeyToolListView().NVI_SetScrollBarControlID(kControlID_Invalid,mVScrollBarControlID);
		NVI_SetControlBindings(mVScrollBarControlID,false,true,true,true,true,false);
	}
	//SubWindow用セパレータ生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,mKeyToolListViewControlID,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	//
	UpdateViewBounds();
	//windows用
	NVI_RegisterToFocusGroup(mKeyToolListViewControlID);
	NVI_SwitchFocusTo(mKeyToolListViewControlID);
}

//Hide()時等にウインドウのデータを保存する
void	AWindow_KeyToolList::NVIDO_Hide()
{
	//ウインドウ幅の保存
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kKeyToolListWindowWidth,NVI_GetWindowWidth());
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kKeyToolListWindowHeight,NVI_GetWindowHeight());
	//次回表示時に下方表示優先にする
	mPreferBelow = true;
}

//
void	AWindow_KeyToolList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	NVI_SetWindowTransparency(0.9);
	UpdateViewBounds();
	NVI_RefreshWindow();
}

//
void	AWindow_KeyToolList::SPI_SetInfo( const AWindowID inTextWindowID, const AControlID inTextViewControlID,
										 const AGlobalPoint inPoint, const ANumber inLineHeight, 
										 const ATextArray& inTextArray, const ABoolArray& inIsUserToolArray )
{
	//リスト設定
	GetKeyToolListView().SPI_SetInfo(inTextWindowID,inTextViewControlID,inTextArray,inIsUserToolArray);
	//ポップアップ時は、ウインドウサイズに合わせて（上のSPI_SetInfo()で項目数に合わせてウインドウサイズ変更されている）、
	//ウインドウ位置を調整
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Popup )
	{
		ANumber	h = NVI_GetWindowHeight();
		AGlobalRect	screenbounds;
		NVI_GetAvailableWindowBoundsInSameScreen(screenbounds);
		APoint	pt;
		if( mPreferBelow == true )
		{
			//==================下方表示優先の場合==================
			//下方に位置設定
			pt.x = inPoint.x;
			pt.y = inPoint.y + inLineHeight + 32;//タイトルバー分だけ多め
			if( pt.y + h > screenbounds.bottom )
			{
				//ウインドウbottomがスクリーン外になるときは、上に表示する
				pt.y = inPoint.y - 16- h;
				//上方表示優先にする
				mPreferBelow = false;
			}
		}
		else
		{
			//==================上方表示優先の場合==================
			//上方に位置設定
			pt.x = inPoint.x;
			pt.y = inPoint.y - 16- h;
			if( pt.y < screenbounds.top )
			{
				//ウインドウtopがスクリーン外になるときは、下に表示する
				pt.y = inPoint.y + inLineHeight + 32;//タイトルバー分だけ多め
				//下方表示優先にする
				mPreferBelow = true;
			}
		}
		NVI_SetWindowPosition(pt);
	}
}

//
void	AWindow_KeyToolList::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//タイルバー、セパレータ高さ取得
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	
	//サイドバーの最後の項目かどうかを取得
	ABool	isSideBarBottom = GetApp().SPI_IsSubWindowSideBarBottom(GetObjectID());
	
	//背景色描画view調整
	ANumber	leftMargin = 1;
	ANumber	rightMargin = 1;
	ANumber	bottomMargin = 1;
	ANumber	topMargin = 0;
	AWindowPoint	backgroundPt = {0,height_Titlebar};
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			leftMargin = 0;
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			rightMargin = 0;
			break;
		}
	  case kSubWindowLocationType_Floating:
		{
			leftMargin = 0;
			rightMargin = 0;
			bottomMargin = 0;
			break;
		}
	  default:
		{
			//ポップアップの場合
			leftMargin = 0;
			rightMargin = 0;
			bottomMargin = 0;
			backgroundPt.y = 0;
			topMargin = 5;
			break;
		}
	}
	if( isSideBarBottom == true )
	{
		bottomMargin = 0;
	}
	
	//タイトルバー、セパレータ表示フラグ
	ABool	showTitleBar = false;
	ABool	showSeparator = false;
	ABool	showSizeBox = false;
	ABool	showBackground = false;
	//list viewサイズ
	AWindowPoint	listViewPoint = {leftMargin,topMargin};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;
	//vscrollbar幅
	ANumber	vscrollbarWidth = kVScrollBarWidth;
	//右サイドバーの最終項目の場合、かつ、ステータスバー未表示時以外はサイズボックスの対象は自ウインドウ
	NVI_SetSizeBoxTargetWindowID(GetObjectID());
	//レイアウト計算
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//ポップアップの場合
	  case kSubWindowLocationType_Popup:
		{
			//background viewにrounded rectを表示するので、その内部におさまるように、list viewを少し小さくする
			showBackground = true;
			listViewPoint.y += 4;
			listViewPoint.x += 5;
			listViewWidth -= 10;
			listViewHeight -= 8;
			//scroll barは表示しない
			vscrollbarWidth = 0;
			//サイズボックスは表示する
			showSizeBox = true;
			break;
		}
		//サイドバーの場合
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//タイトルバー、セパレータ表示フラグ
			showTitleBar = true;
			showSeparator = true;
			showBackground = true;
			//list viewサイズ
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;// + height_Separator;
			/*
			if( isSideBarBottom == true )
			{
				showSeparator = false;
				//右サイドバーの最終項目の場合、かつ、ステータスバー未表示時はテキストウインドウサイズ変更用のサイズボックスを表示する
				if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_RightSideBar &&
							GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
				{
					showSizeBox = true;
					//サイズボックス対象をテキストウインドウにする
					NVI_SetSizeBoxTargetWindowID(NVI_GetOverlayParentWindowID());
				}
			}
			*/
			break;
		}
		//フローティングの場合
	  default:
		{
			//タイトルバー、セパレータ表示フラグ
			showTitleBar = true;
			showBackground = true;
			//list viewサイズ
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;
			//
			showSizeBox = true;
			break;
		}
	}
	//
	if( listViewHeight < 0 )   listViewHeight = 0;
	AWindowPoint	footerPoint = {leftMargin,listViewPoint.y+listViewHeight};
	ANumber	footerWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	
	//Vスクロールバー設定
	ANumber	vscrollbarHeight = listViewHeight;
	if( showSizeBox == true )
	{
		vscrollbarHeight -= kHeight_SizeBox;
	}
	
	//タイトルバー配置
	if( showTitleBar == true )
	{
		//タイトルバー配置
		AWindowPoint	pt = {-1+leftMargin,0};
		NVI_SetControlPosition(kControlID_TitleBar,pt);
		NVI_SetControlSize(kControlID_TitleBar,windowBounds.right - windowBounds.left +2-leftMargin-rightMargin,height_Titlebar);
		//タイトルバー表示
		NVI_SetShowControl(kControlID_TitleBar,true);
	}
	else
	{
		//タイトルバー非表示
		NVI_SetShowControl(kControlID_TitleBar,false);
	}
	
	//背景描画view配置
	if( showBackground == true )
	{
		//背景描画view配置
		NVI_SetControlPosition(kControlID_Background,backgroundPt);
		NVI_SetControlSize(kControlID_Background,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top - backgroundPt.y);
		//背景描画view表示
		NVI_SetShowControl(kControlID_Background,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_Background,false);
	}
	
	//セパレータ配置
	if( showSeparator == true )
	{
		//セパレータ配置
		AWindowPoint	pt = {0,windowBounds.bottom-windowBounds.top-height_Separator};
		NVI_SetControlPosition(kControlID_Separator,pt);
		NVI_SetControlSize(kControlID_Separator,windowBounds.right-windowBounds.left,height_Separator);
		//セパレータ表示
		NVI_SetShowControl(kControlID_Separator,true);
	}
	else
	{
		//セパレータ非表示
		NVI_SetShowControl(kControlID_Separator,false);
	}
	
	//ListView配置・表示
	{
		NVI_SetControlPosition(mKeyToolListViewControlID,listViewPoint);
		NVI_SetControlSize(mKeyToolListViewControlID,listViewWidth - vscrollbarWidth,listViewHeight);
		NVI_SetShowControl(mKeyToolListViewControlID,true);
	}
	
	//Vスクロールバー配置
	if( vscrollbarWidth > 0 && NVI_IsControlEnable(mVScrollBarControlID) == true  )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(mVScrollBarControlID,pt);
		NVI_SetControlSize(mVScrollBarControlID,vscrollbarWidth,vscrollbarHeight);
		NVI_SetShowControl(mVScrollBarControlID,true);
	}
	else
	{
		NVI_SetShowControl(mVScrollBarControlID,false);
	}
	
	//サイズボックス配置
	if( showSizeBox == true )
	{
		AWindowPoint	pt = {windowBounds.right-windowBounds.left-kWidth_SizeBox - rightMargin,
		windowBounds.bottom-windowBounds.top-kHeight_SizeBox - bottomMargin};
		NVI_SetControlPosition(kControlID_SizeBox,pt);
		NVI_SetControlSize(kControlID_SizeBox,kWidth_SizeBox,kHeight_SizeBox);
		NVI_SetShowControl(kControlID_SizeBox,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_SizeBox,false);
	}
	
	//
	GetKeyToolListView().SPI_UpdateDrawProperty();
}

