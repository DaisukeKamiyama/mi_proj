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

AWindow_Previewer
#734

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_SubWindowTitlebar.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_Previewer

/**
*/
class AWindow_Previewer : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_Previewer();
	~AWindow_Previewer();
	
	//<関連オブジェクト取得>
  public:
	AView_SubWindowTitlebar&	GetTitlebarViewByControlID( const AControlID inControlID );//#725
	
	//<イベント処理>
	
	//イベントコールバック
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	void					EVTDO_DoCloseButton();
	
	//<インターフェイス>
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	void					NVIDO_Hide();
	void					NVIDO_UpdateProperty();
	
	//表示要求
  public:
	void					SPI_RequestLoadURL( const AText& inURL );
	void					SPI_RequestReload();
  private:
	AText								mRequestedURL;
	AText								mDisplayedURLWhenLoaded;
	
  public:
	void					SPI_ExecuteJavaScript( const AText& inText );
	void					SPI_SetLetterSize( const ANumber inPercent );
	
	//
  private:
	void					UpdateViewBounds();
	
	//#725
	/** ウインドウの通常時（collapseしていない時）の最小高さ取得 */
  public:
	ANumber					SPI_GetSubWindowMinHeight() const;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_Previewer; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_Previewer"; }
};

