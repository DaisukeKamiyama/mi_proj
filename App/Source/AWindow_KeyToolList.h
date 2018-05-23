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

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AView_KeyToolList;
class AView_SubWindowTitlebar;

#pragma mark ===========================
#pragma mark [クラス]AWindow_KeyToolList
//情報ウインドウ

class AWindow_KeyToolList : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_KeyToolList();
	~AWindow_KeyToolList();
	
	//<関連オブジェクト取得>
  private:
	AView_KeyToolList&	GetKeyToolListView();
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
	
  public:
	void					SPI_SetInfo( const AWindowID inTextWindowID, const AControlID inTextViewControlID,
										const AGlobalPoint inPoint, const ANumber inLineHeight, 
										const ATextArray& inTextArray, const ABoolArray& inIsUserToolArray );
  private:
	void					UpdateViewBounds();
	
  private:
	AControlID							mKeyToolListViewControlID;
	AControlID							mVScrollBarControlID;
	
	//上方表示／下方表示どちらを優先かを記憶
	ABool								mPreferBelow;
	
	static const ANumber				kVScrollBarWidth = 12;
	
	
	//#725
	/** ウインドウの通常時（collapseしていない時）の最小高さ取得 */
  public:
	ANumber					SPI_GetSubWindowMinHeight() const;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_KeyToolList; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_KeyToolList"; }
};

