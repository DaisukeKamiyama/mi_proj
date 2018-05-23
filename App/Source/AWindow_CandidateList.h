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

AWindow_CandidateList

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_SubWindowTitlebar.h"
#include "AThread_CandidateFinder.h"

class AView_CandidateList;

#pragma mark ===========================
#pragma mark [クラス]AWindow_CandidateList
//情報ウインドウ

class AWindow_CandidateList : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_CandidateList();
	~AWindow_CandidateList();
	
	//<関連オブジェクト取得>
  public:
	AView_CandidateList&	SPI_GetCandidateListView() { return GetCandidateListView(); }
	const AView_CandidateList&	SPI_GetCandidateListViewConst() const { return GetCandidateListViewConst(); }
  private:
	AView_CandidateList&	GetCandidateListView();
	const AView_CandidateList&	GetCandidateListViewConst() const;
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
	ANumber					SPI_GetMaxWindowHeight() const;
	void					SPI_UpdateStatusBar();
	void					SPI_SetInfo( const AModeIndex inModeIndex,
							const AWindowID inTextWindowID, const AControlID inTextViewControlID,
							//#717 const AGlobalPoint inPoint, const ANumber inLineHeight, 
							const ATextArray& inIdTextArray, const ATextArray& inInfoTextArray,
							const AArray<AIndex>& inCategoryIndexArray, const AArray<AScopeType>& inScopeTypeArray, //#717 const AObjectArray<AFileAcc>& inFileArray );
							const ATextArray& inFilePathArray, const AArray<ACandidatePriority>& inPriorityArray,
							const ATextArray& inParentKeywordArray,
							const AArray<AItemCount>& inMatchedCountArray );
	void					SPI_SetIndex( const AIndex inIndex );
  private:
	void					UpdateViewBounds();
	AText								mAbbrevKeyText;
	AModeIndex							mCurrentModeIndexForAbbrevKeyText;
	
	//#725
	/** ウインドウの通常時（collapseしていない時）の最小高さ取得 */
  public:
	ANumber					SPI_GetSubWindowMinHeight() const;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_CandidateList; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_CandidateList"; }
};

