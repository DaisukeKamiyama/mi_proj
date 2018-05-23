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

AWindow_FusenList
#138 AWindow_JumpListをベースとして付箋紙リスト用に新規生成

*/

#include "stdafx.h"

#include "AWindow_FusenList.h"
#include "AApp.h"
#include "AView_List_FusenList.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_FusenList
#pragma mark ===========================
//付箋紙リストウインドウのクラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_FusenList::AWindow_FusenList()
	:AWindow(), mListViewControlID(kControlID_Invalid), mVScrollBarControlID(kControlID_Invalid)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kFusenListWindowPosition);
}
//デストラクタ
AWindow_FusenList::~AWindow_FusenList()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

/**
ListView(付箋紙リスト)オブジェクト取得
*/
AView_List_FusenList&	AWindow_FusenList::SPI_GetListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_List_FusenList,kViewType_List_FusenList,mListViewControlID);
}
const AView_List_FusenList&	AWindow_FusenList::SPI_GetListViewConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_List_FusenList,kViewType_List_FusenList,mListViewControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//ウインドウサイズ変更通知時のコールバック
void	AWindow_FusenList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	//コントロールの位置・サイズ更新
	UpdateViewBounds(kIndex_Invalid);
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_FusenList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ実体生成
	NVM_CreateWindow("main/FusenList");
	
	//サイズ設定
	NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFusenListWindowWidth));
	NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFusenListWindowHeight));
	
	/*win
	//フローティング設定
	NVI_SetFloating();
	*/
}

//新規タブ生成
//引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
void	AWindow_FusenList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {0,0};
	
	//ListView生成
	mListViewControlID = NVM_AssignDynamicControlID();
	AViewDefaultFactory<AView_List_FusenList>	fusenListViewFactory(GetObjectID(),mListViewControlID);
	NVM_CreateView(mListViewControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,fusenListViewFactory);
	
	//Vスクロールバー生成、ListViewにリンクを設定
	mVScrollBarControlID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(mVScrollBarControlID,pt,kVScrollBarWidth,kVScrollBarWidth*2);
	SPI_GetListView().NVI_SetScrollBarControlID(kControlID_Invalid,mVScrollBarControlID);
	
	//コントロールの位置・サイズ更新
	UpdateViewBounds(inTabIndex);
	
	//
	outInitialFocusControlID = mListViewControlID;
}

//Hide()時等にウインドウのデータを保存する
void	AWindow_FusenList::NVIDO_Hide()
{
	//ウインドウ幅の保存
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kFusenListWindowWidth,NVI_GetWindowWidth());
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kFusenListWindowHeight,NVI_GetWindowHeight());
}

/**
プロパティデータを反映する
*/
void	AWindow_FusenList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	NVI_SetWindowTransparency((static_cast<AFloatNumber>(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kSubWindowsAlpha_Floating)))/100);
}

#pragma mark ===========================

#pragma mark ---

/**
コントロールの位置・サイズ更新
*/
void	AWindow_FusenList::UpdateViewBounds( const AIndex inTabIndex )
{
	//ウインドウbounds取得
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	
	//FileListView bounds設定
	AWindowPoint	pt;
	pt.x = 0;
	pt.y = kButtonHeight;
	ANumber	viewwidth = windowBounds.right - windowBounds.left -kVScrollBarWidth +1;
	ANumber	viewheight = windowBounds.bottom - windowBounds.top;
	NVI_SetControlPosition(mListViewControlID,pt);
	NVI_SetControlSize(mListViewControlID,viewwidth,viewheight-kButtonHeight);
	//スクロールバーbounds設定
	pt.x = windowBounds.right - windowBounds.left -kVScrollBarWidth +1;
	pt.y = kButtonHeight;
	viewwidth = kVScrollBarWidth;
	viewheight = windowBounds.bottom - windowBounds.top - kDragBoxHeight +1;
	NVI_SetControlPosition(mVScrollBarControlID,pt);
	NVI_SetControlSize(mVScrollBarControlID,viewwidth,viewheight-kButtonHeight);
}


