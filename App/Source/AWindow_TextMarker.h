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

AWindow_TextMarker
#750

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_SubWindowTitlebar.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_TextMarker

/**
TextMarker
*/
class AWindow_TextMarker : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_TextMarker();
	~AWindow_TextMarker();
	
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
	void					EVTDO_TextInputted( const AControlID inID );
	ABool					EVTDO_ValueChanged( const AControlID inID );
	ABool					NVIDO_ViewReturnKeyPressed( const AControlID inControlID );
	ABool					NVIDO_ViewEscapeKeyPressed( const AControlID inControlID );
	void					ApplyMarkerTextToDocument();
	void					EVTDO_DoCloseButton();
	void					ClearGroupNameEditMode( const ABool inApply );
	
	//<インターフェイス>
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	void					NVIDO_Hide();
	void					NVIDO_UpdateProperty();
	
	//
  public:
  private:
	void					UpdateViewBounds();
	ABool								mGroupNameEditMode;
	
	//#725
	/** ウインドウの通常時（collapseしていない時）の最小高さ取得 */
  public:
	ANumber					SPI_GetSubWindowMinHeight() const;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TextMarker; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextMarker"; }
};

