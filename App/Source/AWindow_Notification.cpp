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

AWindow_Notification
#725

*/

#include "stdafx.h"

#include "AWindow_Notification.h"
#include "AView_Notification.h"
#include "AApp.h"

//Notification viewのcontrol id
const AControlID	kControlID_ListView = 101;

#pragma mark ===========================
#pragma mark [クラス]AWindow_Notification
#pragma mark ===========================
/*
各種メッセージ表示用ポップアップ
各text view毎にAWindow_Notificationポップアップウインドウを持ち、そのウインドウ内にAView_Notificationがある
*/

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_Notification::AWindow_Notification():AWindow()
{
	//タイマーenable
	NVI_AddToTimerActionWindowArray();
}
/**
デストラクタ
*/
AWindow_Notification::~AWindow_Notification()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

/**
ListView(Notification view)取得
*/
AView_Notification&	AWindow_Notification::GetListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Notification,kViewType_Notification,kControlID_ListView);
}
const AView_Notification&	AWindow_Notification::GetListViewConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_Notification,kViewType_Notification,kControlID_ListView);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
メニュー選択時処理
*/
ABool	AWindow_Notification::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
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
void	AWindow_Notification::EVTDO_UpdateMenu()
{
}

//コントロールのクリック時のコールバック
ABool	AWindow_Notification::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	}
	return result;
}

//ウインドウサイズ変更通知時のコールバック
void	AWindow_Notification::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	UpdateViewBounds();
}

/**
ウインドウの閉じるボタン
*/
void	AWindow_Notification::EVTDO_DoCloseButton()
{
}

/**
tickタイマー処理
*/
void	AWindow_Notification::EVTDO_DoTickTimerAction()
{
	GetListView().SPI_DoTickTimer();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_Notification::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
	
	//Notification view生成
	AWindowPoint	pt = {0,0};
	AViewDefaultFactory<AView_Notification>	infoViewFactory(GetObjectID(),kControlID_ListView);
	NVM_CreateView(kControlID_ListView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,infoViewFactory);
}

//新規タブ生成
//引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
void	AWindow_Notification::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
}

//Hide()時等にウインドウのデータを保存する
void	AWindow_Notification::NVIDO_Hide()
{
	GetListView().SPI_DoWindowHide();
}

//
void	AWindow_Notification::NVIDO_UpdateProperty()
{
}

/**
view bounds更新
*/
void	AWindow_Notification::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	//背景描画view配置
	AWindowPoint	pt = {0};
	NVI_SetControlPosition(kControlID_ListView,pt);
	NVI_SetControlSize(kControlID_ListView,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top);
	//背景描画view表示
	NVI_SetShowControl(kControlID_ListView,true);
}


