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

AView_TabSelector

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AView_TabSelector

class AView_TabSelector : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_TabSelector( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_TabSelector();
  private:
	void					NVIDO_Init();//#92
	void					NVIDO_DoDeleted();//#92
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					MouseClick( const ALocalPoint& inLocalPoint );//#164
	ABool					EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}//win 080706
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	AIndex								mContextMenuSelectedTabIndex;
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//B0000 080810
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );//B0000 080810
	ABool					EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;//R0240
	void					EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation );//#364
	void					EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#364
	void					EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#364
	void					EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//win
	void					ClearHover();//B0000 080810
	void					StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#364
	void					GetTabRect( const ALocalPoint& inLocalPoint, ALocalRect& outTabRect );//#364
	AIndex					FindDragIndex( const ALocalPoint& inLocalPoint ) const;//#364
	void					XorDragLine( const AIndex inDragIndex );//#364
	void					GetCloseButtonRect( const AIndex inTabDisplayIndex, ALocalRect& outRect ) const;//#164
	void					UpdateDragging( const ALocalPoint& inLocalPoint );//#850
	AIndex								mHoverIndex;//B0000 080810
	ABool								mMouseTrackResultIsDrag;//#364
	ALocalPoint							mMouseTrackStartLocalPoint;//#364
	ABool								mMouseDown;//#364
	ABool								mMouseClickCount;//#832
	//#688 ABool								mMouseTrackByLoop;//#364
	//#850 ABool								mDragMode;//#364
	//#850 AIndex								mDragCurrentIndex;//#364
	//#850 AIndex								mDragOriginalDisplayIndex;//#364
	ABool								mHoverCloseButton;//#164
	ABool								mDragging;//#575
	AIndex								mDraggingCurrentDisplayIndex;//#850
	ABool								mDraggingTabShouldHide;//#850
	//#364
//#688 #if IMPLEMENTATION_FOR_WINDOWS
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
//#688 #endif
	
	//<インターフェイス>
	public:
	ANumber					SPI_GetHeight( const ANumber inViewWidth ) const;
	void					SPI_UpdateProperty();//R0177
  private:
	AItemCount				GetTabRowCount( const ANumber inViewWidth ) const;
	AItemCount				GetTabCountInOneRow( const ANumber inViewWidth ) const;
	
	//情報取得
  public:
	AIndex					SPI_GetContextMenuSelectedTabIndex() { return mContextMenuSelectedTabIndex; }
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_TabSelector;}
	
	/*#164
	//R0228
  public:
	AIndex					SPI_GetCurrentCloseButtonIndex() const;
	AControlID				SPI_GetTabCloseButtonControlID() const { return kTabCloseButtonControlID; }
  private:
	AIndex								mCurrentCloseButtonIndex;
	const static  AControlID			kTabCloseButtonControlID = 99;
	ABool								mEnableTabCloseButton;
	*/
	
	//
  private:
	ANumber								mTabRowHeight;//R0177
	ANumber								mTabColumnWidth;//R0177
	AFloatNumber						mFontSize;//R0177
	//win AFont								mFont;//R0177
	AText								mFontName;//win
	ANumber								mFontHeight;//R0177
	ANumber								mFontAscent;//R1077
	
	
	//#498
  public:
	void					SPI_DoTabCreated( const AIndex inTabIndex );
	void					SPI_DoTabDeleted( const AIndex inTabIndex );
	AIndex					SPI_GetDisplayTabIndex( const AIndex inTabIndex ) const
	{ return mDisplayOrderArray.Find(inTabIndex); }//#513
	AIndex					SPI_GetTabIndexFromDisplayIndex( const AIndex inDisplayIndex ) const//#559
	{
		if( inDisplayIndex < 0 )   return kIndex_Invalid;
		if( inDisplayIndex >= mDisplayOrderArray.GetItemCount() )   return kIndex_Invalid;
		return mDisplayOrderArray.Get(inDisplayIndex);
	}
	void					SPI_SetDisplayTabIndex( const AIndex inTabIndex, const AIndex inDisplayTabIndex );
	AItemCount				SPI_GetDisplayTabCount() const { return mDisplayOrderArray.GetItemCount(); }//#559
	void					SPI_SetDisplayOrderArray( const AArray<AIndex>& inTabIndexArray );//#1062
  private:
	//void					UpdateMouseTrackingRect();//#688
	AHashArray<AIndex>					mDisplayOrderArray;//配列順：表示順、値：データ上のindex
	
	//R0240
	const static ANumber				kTextLeftMargin = 12;//3;
	const static ANumber				kTextRightMargin = 12;//3;
	const static ANumber				kTextBottomMargin = 5;//3;
	const static ANumber				kTextEllipseMargin = 12;
};

//#199 ATabSelectorViewFactoryは削除。AViewDefaultFactory<>へ統合。

