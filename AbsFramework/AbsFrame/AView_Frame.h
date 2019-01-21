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

AView_Frame

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "AView.h"

#pragma mark ===========================
#pragma mark [クラス]AView_Frame

/**
枠表示のためのView
*/
class AView_Frame : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_Frame( const AWindowID inWindowID, const AControlID inID, const AControlID inContentControlID );
	virtual ~AView_Frame();
  private:
	void					NVIDO_Init();//#92
	void					NVIDO_DoDeleted();//#92
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}
	void					EVTDO_DoDraw();
	
	//<インターフェイス>
  public:
	/*#1316
	void					SPI_SetColor( const AColor inBackColor, const AColor inFrameColor, const AColor inFrameColorDeactive,
							const AFloatNumber inTransparency = 1.0 );//#688
							*/
	void					SPI_SetExistText( const ABool inExist );//#1316
	//#1316 void					SPI_SetFrameBackgroundColor( const ABool inEnableFrameBackgroundColor, const AColor inColor );//win
	void					SPI_SetFocused( const ABool inFocused );//#135
	void					SPI_GetRealFrameBounds( AWindowRect& outRect ) const;//#135
	void					SPI_SetRealFramePositionAndSize( const AWindowPoint& inRealFramePoint, 
							const ANumber inRealFrameWidth, const ANumber inRealFrameHeight );//#135
	static const ANumber				kFocusWidth = 3;
	//#135 static ANumber			SPI_GetFocusFrameWidth() { return kFocusWidth; }//#135
	void					SPI_SetEnableFocusRing( const ABool inEnableFocusRing ) { mEnableFocusRing = inEnableFocusRing; }//#798
	
	
	//#725
	//フレームのタイプ
  public:
	void					SPI_SetFrameViewType( const AFrameViewType inType ) { mFrameViewType = inType; }
  private:
	AFrameViewType						mFrameViewType;
	
	//<データ>
  private:
	AControlID							mContentControlID;
	/*#1316
	AColor								mBackgroundColor;
	AColor								mFrameColor;
	AColor								mFrameColorDeactive;
	*/
	ABool								mFocused;
	ABool								mEnableFocusRing;//#798
	//#1316 AFloatNumber						mBackgroundTransparency;//#688
	/*#1316
	//フレーム部分背景色 win
	ABool								mEnableFrameBackgroundColor;
	AColor								mFrameBackgroundColor;
	*/
	ABool								mExistText;//#1316
	
	//Object Type
  private:
	AViewType				NVIDO_GetViewType() const { return kViewType_Frame; }
};

#pragma mark ===========================
#pragma mark [クラス]AFrameViewFactory
class AFrameViewFactory : public AViewFactory
{
  public:
	AFrameViewFactory( const AWindowID inWindowID, const AControlID inID, const AControlID inContentControlID )
	: mWindowID(inWindowID), mControlID(inID), mContentControlID(inContentControlID)//#0
	{
	}
	AView_Frame*	Create() 
	{
		return new AView_Frame(mWindowID,mControlID,mContentControlID);
	}
  private:
	AWindowID							mWindowID;
	AControlID							mControlID;
	AControlID							mContentControlID;
};

