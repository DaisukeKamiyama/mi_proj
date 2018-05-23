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

AWindow_IdInfo

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_SubWindowTitlebar.h"

class AView_IdInfo;

#pragma mark ===========================
#pragma mark [クラス]AWindow_IdInfo
//情報ウインドウ

class AWindow_IdInfo : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_IdInfo();
	~AWindow_IdInfo();
	
	//<関連オブジェクト取得>
  public:
	AView_IdInfo&			SPI_GetIdInfoView();//#238
  private:
	AView_IdInfo&			GetIdInfoView();
	const AView_IdInfo&		GetIdInfoViewConst() const;
	AView_SubWindowTitlebar&	GetTitlebarViewByControlID( const AControlID inControlID );//#725
	
	//<イベント処理>
	
	//イベントコールバック
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	//#725 void					EVTDO_DoCloseButton();//#291
	void					EVTDO_TextInputted( const AControlID inID );
	ABool					NVIDO_ViewReturnKeyPressed( const AControlID inControlID );
	ABool					NVIDO_ViewEscapeKeyPressed( const AControlID inControlID );//#798
	ABool					NVIDO_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );
	ABool					NVIDO_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );
	void					EVTDO_DoCloseButton();
	void					EVTDO_DoTickTimerAction();
	
	//<インターフェイス>
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	void					NVIDO_Hide();
	void					NVIDO_UpdateProperty();
	
  public:
	void					SPI_SwitchFocusToSearchBox();//#798
	ANumber					SPI_GetMaxWindowHeight() const;
	/*#238
	void					SPI_SetInfo( const AText& inIdText, const AModeIndex inModeIndex, 
							const ATextArray& inInfoTextArray, const AObjectArray<AFileAcc>& inFileArray,
							const ATextArray& inCommentTextArray, const AArray<AIndex>& inCategoryIndexArray,
							const AArray<AScopeType>& inScopeTypeArray,
							const AArray<AIndex>& inStartIndexArray, const AArray<AIndex>& inEndIndexArray,
							const ATextArray& inParentKeywordArray );
	void					SPI_SetArgIndex( const AIndex inIndex );
	*/
	//#725 void					SPI_FixDisplay();
	//#725 void					SPI_UpdateHistoryButton();//#238
	//#238 const AText&			SPI_GetIdText() const;
	//#725 void					SPI_UpdateOverlayWindowAlpha();//#291
	//#725 void					SPI_SetSideBarMode( const ABool inSideBarMode, const AColor inSideBarColor );//#634
  private:
	ABool					EVTDO_DoMouseMoved( const AWindowPoint& inWindowPoint, const AModifierKeys inModifierKeys );
	void					UpdateViewBounds();
	//#238 ABool								mFixDisplay;
	
  private:
	AControlID							mInfoViewControlID;
	AControlID							mVScrollBarControlID;
	
	static const ANumber				kVScrollBarWidth = 11;
	
	
	//#725
	/** ウインドウの通常時（collapseしていない時）の最小高さ取得 */
  public:
	ANumber					SPI_GetSubWindowMinHeight() const;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_IdInfo; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_IdInfo"; }
};

