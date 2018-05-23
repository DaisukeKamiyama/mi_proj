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

AView_Index

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class ADocument_IndexWindow;

#pragma mark ===========================
#pragma mark [クラス]AView_Index

class AView_Index : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_Index( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID, const AObjectID inIndexDocumentID );
	virtual ~AView_Index();
  private:
	void					NVIDO_Init();//#92
	void					NVIDO_DoDeleted();//#92
	AObjectID							mIndexDocumentID;
	//#199 AWindow&	mWindow;
	ANumber								mFileColumnWidth;
	//#725
	ABool								mOneColumnMode;
	
	//<関連オブジェクト取得>
  public:
	ADocument_IndexWindow&	GetIndexDocument();
	const ADocument_IndexWindow&	GetIndexDocumentConst() const;
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint );//win 080706
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					DragChangeColumnWidth( const ALocalPoint inLocalPoint );//win
//#688 #if IMPLEMENTATION_FOR_WINDOWS
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//win
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//win
//#688 #endif
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu );
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#92
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );//#92
	void					UpdateCursorRow( const ALocalPoint& inLocalPoint );//#92
	void					ClearCursorRow();//#92
	void					NVMDO_DoViewDeactivated();//#92
	void					NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight );
	ABool					EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, 
							const AModifierKeys inModifierKeys, const ANumber inClickCount );//#465
	ABool					EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;//#541
	AIndex								mCursorRowDisplayIndex;//#92
	//#688 ABool								mMouseTrackByLoop;//win
	AMouseTrackingMode					mMouseTrackingMode;//win
	ALocalPoint							mMouseTrackStartPoint;//win
	ALocalPoint							mMouseTrackPoint;//win
	
	//<インターフェイス>
  public:
	void					SPI_UpdateRows( const AIndex inStartRowIndex, const AIndex inEndRowIndex );
	void					SPI_UpdateImageSize();
	void					SPI_UpdateTextDrawProperty();
	void					SPI_UpdateScrollBarUnit();
	void					SPI_ScrollToHome();
	void					SPI_SetOnlyFileColumnWidth( const ANumber inNewWidth );
	void					SPI_SetInfoColumnWidth( const ANumber inNewWidth );
	void					SPI_SetFileColumnWidth( const ANumber inNewWidth );
	void					SPI_AdjustScroll();
	void					SPI_RefreshRow( const AIndex inDisplayRowIndex );
  private:
	ANumber					GetRowHeight() const;
	void					GetRowImageRect( const AIndex inStartRowIndex, const AIndex inEndRowIndex, AImageRect& outRowImageRect ) const;
	
	//ViewType取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_Index;}
	
	//
  public:
	void					SPI_OpenFromRow( const AIndex inRowIndex ) const;
	void					SPI_SelectNextRow();
	void					SPI_SelectPreviousRow();
	void					SPI_SelectRow( const AIndex inDisplayRowIndex );
	
	//
  public:
	void					SPI_GetColumnWidth( ANumber& outFileColumnWidth, ANumber& outInfoColumnWidth ) const;
	
	//#92
  public:
	void					SPI_TryCloseWindow();
	
	//#92
  public:
	void					SPI_ShowSaveWindow();
	
	//#465
  public:
	AIndex					SPI_GetContextMenuSelectedRowDBIndex() const;
	void					SPI_ExportToNewDocument( const AIndex inRowIndex ) const;
  private:
	AIndex								mContextMenuSelectedRowIndex;
	
	//
  private:
	ANumber								mLineHeight;
	ANumber								mAscent;
	AIndex								mSelectedRowIndex;
	
	static const ANumber				kHScrollBarUnit = 30;
	static const ANumber				kColumnWidthChangeAreaWidth = 4;
	
	//Viewオブジェクト取得 #92
  public:
	static AView_Index&			GetIndexViewByViewID( const AViewID inViewID )
	{
		MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_Index,kViewType_Index,inViewID);
	}
	static const AView_Index&	GetIndexViewConstByViewID( const AViewID inViewID ) 
	{
		MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_VIEWID(AView_Index,kViewType_Index,inViewID);
	}
};


#pragma mark ===========================
#pragma mark [クラス]AIndexViewFactory
class AIndexViewFactory : public AViewFactory
{
  public:
	AIndexViewFactory( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID, const AObjectID inIndexDocumentID )
			: /*#199 mParent(inParent), mWindow(inWindow)*/mWindowID(inWindowID), mControlID(inID), mIndexDocumentID(inIndexDocumentID)
	{
	}
	AView_Index*	Create() 
	{
		return new AView_Index(/*#199 mParent,mWindow*/mWindowID,mControlID,mIndexDocumentID);
	}
  private:
	//#199 AObjectArrayItem* mParent;
	//#199 AWindow&	mWindow;
	AWindowID							mWindowID;//#199
	AControlID							mControlID;
	AObjectID							mIndexDocumentID;
};

