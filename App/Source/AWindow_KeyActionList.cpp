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

AWindow_KeyActionList
#738

*/

#include "stdafx.h"

#include "AWindow_KeyActionList.h"
#include "AView_DocInfo.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"

//#725
//ListView
const AControlID	kControlID_ListView = 101;


#pragma mark ===========================
#pragma mark [クラス]AWindow_KeyActionList
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_KeyActionList::AWindow_KeyActionList():AWindow()
{
}
/**
デストラクタ
*/
AWindow_KeyActionList::~AWindow_KeyActionList()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_KeyActionList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	return result;
}

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_KeyActionList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}


#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_KeyActionList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//
	NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_Bottom,true,false,false,false,false,true,true);
	
	//ListView生成
	{
		AWindowPoint	pt = {0,0};
		AListViewFactory	listViewFactory(GetObjectID(),kControlID_ListView);
		NVM_CreateView(kControlID_ListView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,listViewFactory);
		NVI_SetControlBindings(kControlID_ListView,true,true,true,true,false,false);
		
		//
		AFileAcc	folder;
		GetApp().SPI_GetLocalizedDataFolder(folder);
		AFileAcc	file;
		file.SpecifyChild(folder,"keyactions.csv");
		AText	text;
		file.ReadTo(text);
		//
		ACSVFile	csvFile(text,ATextEncodingWrapper::GetUTF8TextEncoding());
		ATextArray	titleArray,toolCommandArray;
		ANumberArray	outlineLevelArray, actionNumberArray;
		csvFile.GetColumn(0,outlineLevelArray);
		csvFile.GetColumn(1,titleArray);
		csvFile.GetColumn(2,actionNumberArray);
		csvFile.GetColumn(3,toolCommandArray);
		
		//
		NVI_GetListView(kControlID_ListView).SPI_RegisterColumn_Text(0,0,"",false);
		NVI_GetListView(kControlID_ListView).SPI_BeginSetTable();
		NVI_GetListView(kControlID_ListView).SPI_SetColumn_Text(0,titleArray);
		NVI_GetListView(kControlID_ListView).SPI_SetTable_OutlineFoldingLevel(outlineLevelArray);
		NVI_GetListView(kControlID_ListView).SPI_EndSetTable();
	}
	
}

/**
新規タブ生成
引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_KeyActionList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
}

/**
Hide()時等にウインドウのデータを保存する
*/
void	AWindow_KeyActionList::NVIDO_Hide()
{
}

/**
*/
void	AWindow_KeyActionList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	NVI_RefreshWindow();
}

/**
*/
void	AWindow_KeyActionList::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//list viewサイズ
	AWindowPoint	listViewPoint = {0,0};
	NVI_SetControlPosition(kControlID_ListView,listViewPoint);
	NVI_SetControlSize(kControlID_ListView,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top);
}




