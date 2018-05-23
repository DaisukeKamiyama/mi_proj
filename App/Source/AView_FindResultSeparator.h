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

AView_FindResultSeparator
#92

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AWindow_Text;

#pragma mark ===========================
#pragma mark [クラス]AView_FindResultSeparator

class AView_FindResultSeparator : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_FindResultSeparator( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_FindResultSeparator();
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();
	
	//<関連オブジェクト取得>
  private:
	AWindow_Text&			GetTextWindow();
	
	//<インターフェイス>
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_FindResultSeparator;}
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys ) {return kCursorType_ResizeUpDown;}
	
	void					DoMouseTrack( const ALocalPoint& inLocalPoint );
	void					DoMouseTrackEnd();
//#688 #if IMPLEMENTATION_FOR_WINDOWS
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
//#688 #endif

	AMouseTrackingMode		mMouseTrackingMode;
	//#688 ABool					mMouseTrackByLoop;
	ALocalPoint				mPreviousMouseLocalPoint;
	ALocalPoint				mDragLineStart;
	ALocalPoint				mDragLineEnd;
	ABool					mHover;
};
 
