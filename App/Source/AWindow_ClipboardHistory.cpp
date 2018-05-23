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

AWindow_ClipboardHistory
#152

*/

#include "stdafx.h"

#include "AWindow_ClipboardHistory.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"

//クリップボード履歴list view
const AControlID	kControlID_ListView = 101;

//クリップボード履歴list view 列
const ADataID			kColumn_Text				= 0;

//スクロールバー
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kWidth_VScroll = 12;

//#725
//SubWindow用タイトルバーview
const AControlID	kControlID_TitleBar = 901;

//#725
//SubWindow用セパレータview
const AControlID	kControlID_Separator = 902;

#pragma mark ===========================
#pragma mark [クラス]AWindow_ClipboardHistory
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_ClipboardHistory::AWindow_ClipboardHistory():AWindow()
{
}
/**
デストラクタ
*/
AWindow_ClipboardHistory::~AWindow_ClipboardHistory()
{
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_ClipboardHistory::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 10;
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_ClipboardHistory::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_ClipboardHistory::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	return result;
}

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_ClipboardHistory::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}


#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_ClipboardHistory::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ生成
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
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
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,true,false,true,false);
			break;
		}
	}
	
	//SubWindow用タイトルバー生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_ClipboardHistory");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwList);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//SubWindow用セパレータ生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//リストview生成
	{
		AWindowPoint	pt = {0,0};
		AListViewFactory	listViewFactory(GetObjectID(),kControlID_ListView);
		NVM_CreateView(kControlID_ListView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,listViewFactory);
		AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(true);
		NVI_GetListView(kControlID_ListView).SPI_SetBackgroundColor(
					GetApp().SPI_GetSubWindowBackgroundColor(true),
					GetApp().SPI_GetSubWindowBackgroundColor(false));
		NVI_GetListView(kControlID_ListView).SPI_RegisterColumn_Text(kColumn_Text,0,"",false);
		NVI_GetListView(kControlID_ListView).SPI_SetEnableCursorRow();
		AText	fontname;
		AFloatNumber	fontsize = 9.0;
		GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
		NVI_GetListView(kControlID_ListView).SPI_SetTextDrawProperty(fontname,fontsize,kColor_Black);
		NVI_GetListView(kControlID_ListView).SPI_SetWheelScrollPercent(
					GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
					GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));
		NVI_SetControlBindings(kControlID_ListView,true,true,true,true,false,false);
	}
	
	//Vスクロールバー生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateScrollBar(kControlID_VScrollBar,pt,kWidth_VScroll,kWidth_VScroll*2);
		NVI_GetListView(kControlID_ListView).NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
		NVI_SetShowControl(kControlID_VScrollBar,true);
		NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	}
}

/**
新規タブ生成
引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_ClipboardHistory::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	UpdateViewBounds();
}

/**
Hide()時等にウインドウのデータを保存する
*/
void	AWindow_ClipboardHistory::NVIDO_Hide()
{
}

/**
*/
void	AWindow_ClipboardHistory::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	NVI_RefreshWindow();
}

/**
クリップボード履歴リストview更新
*/
void	AWindow_ClipboardHistory::SPI_UpdateTable()
{
	NVI_GetListView(kControlID_ListView).SPI_BeginSetTable();
	NVI_GetListView(kControlID_ListView).SPI_SetColumn_Text(kColumn_Text,GetApp().SPI_GetClipboardHistory());
	NVI_GetListView(kControlID_ListView).SPI_EndSetTable();
	NVI_RefreshWindow();
}

/**
View配置更新
*/
void	AWindow_ClipboardHistory::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//タイルバー、セパレータ高さ取得
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	
	//タイトルバー、セパレータ表示フラグ
	ABool	showTitleBar = false;
	ABool	showSeparator = false;
	//list viewサイズ
	AWindowPoint	listViewPoint = {0,0};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top;
	//スクロールバー幅
	ANumber	vscrollbarWidth = kWidth_VScroll;
	//レイアウト計算
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//サイドバーの場合
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//タイトルバー、セパレータ表示フラグ
			showTitleBar = true;
			showSeparator = true;
			//list viewサイズ
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar + height_Separator;
			break;
		}
		//フローティングの場合
	  default:
		{
			//タイトルバー、セパレータ表示フラグ
			showTitleBar = true;
			//list viewサイズ
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;
			break;
		}
	}
	
	//タイトルバー配置
	if( showTitleBar == true )
	{
		//タイトルバー配置
		AWindowPoint	pt = {0,0};
		NVI_SetControlPosition(kControlID_TitleBar,pt);
		NVI_SetControlSize(kControlID_TitleBar,windowBounds.right - windowBounds.left,height_Titlebar);
		//タイトルバー表示
		NVI_SetShowControl(kControlID_TitleBar,true);
	}
	else
	{
		//タイトルバー非表示
		NVI_SetShowControl(kControlID_TitleBar,false);
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
	
	//list view配置
	{
		NVI_GetListView(kControlID_ListView).NVI_SetPosition(listViewPoint);
		NVI_GetListView(kControlID_ListView).NVI_SetSize(listViewWidth - vscrollbarWidth,listViewHeight);
	}
	
	
	//Vスクロールバー配置
	if( vscrollbarWidth > 0 )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(kControlID_VScrollBar,pt);
		NVI_SetControlSize(kControlID_VScrollBar,vscrollbarWidth,listViewHeight);
		NVI_SetShowControl(kControlID_VScrollBar,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_VScrollBar,false);
	}
}

