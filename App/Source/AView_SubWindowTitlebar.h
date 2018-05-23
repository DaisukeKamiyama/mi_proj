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

AView_SubWindowTitlebar

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
//#include "AView.h"

#pragma mark ===========================
#pragma mark [クラス]AView_SubWindowTitlebar

/**
サブウインドウタイトルバー
*/
class AView_SubWindowTitlebar : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_SubWindowTitlebar( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_SubWindowTitlebar();
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}
	void					EVTDO_DoDraw();
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	/*
	void					EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
							ABool& outIsCopyOperation );
	void					EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	ABool								mDragReceiving;
	*/
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	ALocalPoint							mMouseTrackStartLocalPoint;
	ABool					IsOverlay() const;
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );
	ABool					IsPointInCloseButton( const ALocalPoint& inLocalPoint ) const;
	ABool					IsPointInFoldingButton( const ALocalPoint& inLocalPoint ) const;
	ABool								mMouseHoverCloseButton;
	ABool								mMouseHoverFoldingButton;
	ABool								mMousePressingCloseButton;
	ABool								mMousePressingFoldingButton;
	
	//
  private:
	ABool								mDraggingOverlayWindow;
	
	//<インターフェイス>
  public:
	void					SPI_SetTitleTextAndIconImageID( const AText& inText, const AImageID inIconImageID );
  private:
	AImageID							mTitleIconImageID;
	
	//<データ>
  private:
	AText								mTitleText;
	
	//Object Type
  private:
	AViewType				NVIDO_GetViewType() const { return kViewType_SubWindowsTitlebar; }
};

#pragma mark ===========================
#pragma mark [クラス]AView_SubWindowBackground

/**
サブウインドウ背景表示のためのView
*/
class AView_SubWindowBackground : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_SubWindowBackground( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_SubWindowBackground();
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount ) {return false;}
	void					EVTDO_DoDraw();
	ABool					IsPointInCloseButton( const ALocalPoint& inLocalPoint ) const;
	ABool					IsPointInFoldingButton( const ALocalPoint& inLocalPoint ) const;
	
	//<インターフェイス>
  public:
	
	//<データ>
  private:
	
	//Object Type
  private:
	AViewType				NVIDO_GetViewType() const { return kViewType_SubWindowBackground; }
};

