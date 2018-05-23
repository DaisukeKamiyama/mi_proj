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

AView_Button

*/
//#232

#pragma once

#include "../AbsBase/ABase.h"
#include "AView.h"

//#634
/**
ボタン表示タイプ
*/
enum AButtonViewType
{
	kButtonViewType_None = -1,
	kButtonViewType_Rect20 = 0,
	kButtonViewType_RoundedRect20,
	kButtonViewType_Rect16,
	kButtonViewType_Rect16Footer,
	kButtonViewType_TextWithOvalHover,
	kButtonViewType_TextWithOvalHoverWithFixedWidth,
	kButtonViewType_TextWithNoHoverWithFixedWidth,
	kButtonViewType_NoFrame,
	kButtonViewType_NoFrameWithTextHover,
	kButtonViewType_ScrollBar,
};

//#634
/**
ボタン動作タイプ
*/
enum AButtonBehaviorType
{
	kButtonBehaviorType_Normal = 0,
	kButtonBehaviorType_VSeparator
};

//#725
/**
Ellipsisタイプ
*/
enum AEllipsisMode
{
	kEllipsisMode_Normal = 0,
	kEllipsisMode_FixedFirstCharacters,
	kEllipsisMode_FixedLastCharacters,
};

#pragma mark ===========================
#pragma mark [クラス]AView_Button

/**
枠表示のためのView
*/
class AView_Button : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_Button( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_Button();
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();
	void					InitTextPropertyIfNotInited();//#852
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}
	void					EVTDO_DoDraw();
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );
	ABool					EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	ABool					EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;//#379
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#634
	void					MouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					NVIDO_SetShow( const ABool inShow );
	void					NVIDO_SetBool( const ABool inBool );//win
	ABool					NVIDO_GetBool() const;//win
	void					StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//win
//#if IMPLEMENTATION_FOR_WINDOWS
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
//#endif
	void					GetOvalRect( ALocalRect& outOvalRect ) const;
	
	//<インターフェイス>
  public:
	void					SPI_SetIcon( AImageID inIconImageID, const ANumber inLeftOffset = 0, const ANumber inTopOffset = 0,
										const ANumber inWidth = 16, const ANumber inHeight = 16,
										const ABool inRefresh = false, 
										const AImageID inHoverIconImageID = kImageID_Invalid,//#530
										const AImageID inToggleOnImageID = kImageID_Invalid );
	void					SPI_SetIconImageID( AImageID inIconImageID, 
											   const AImageID inHoverIconImageID = kImageID_Invalid,//#530
											   const AImageID inToggleOnImageID = kImageID_Invalid );
	void					SPI_SetTextProperty( const AText& inFontName, const AFloatNumber inFontSize, const AJustification inJustification, 
							const ATextStyle inTextStyle,//#724
							const AColor inColor = kColor_Gray30Percent, const AColor inColorDeactive = kColor_Gray60Percent,
							const ABool inRefresh = false );//#530
	void					SPI_SetWidthToFitTextWidth( const ANumber inMaxWidth = kNumber_MaxNumber );//#724
	void					SPI_SetTextFromMenuIndex( const AIndex inIndex );
	void					SPI_SetMenu( const AContextMenuID inContextMenuID, const AMenuItemID inMenuItemID );
	void					SPI_SetMenuTextArray( const ATextArray& inTextArray, const ATextArray& inActionTextArray, const AMenuItemID inMenuItemID );
	void					SPI_SetToggleMode( const ABool inToggleMode );
	void					SPI_SetToogleOn( const ABool inToggleOn );
	ABool					SPI_GetToggleOn() const;
	void					SPI_SetContextMenu( const AContextMenuID inContextMenuID );
	void					SPI_SetContextMenuEnableItem( const AIndex inIndex, const ABool inEnable );
	void					SPI_SetEnableDrag( const ABool inEnableDrag ) { mEnableDrag = inEnableDrag; }
	void					SPI_SetButtonBehaviorType( const AButtonBehaviorType inButtonType );//#634
	void					SPI_SetButtonValidXRange( const ANumber inStartX, const ANumber inEndX );//#634
	void					SPI_SetButtonViewType( const AButtonViewType inType );//#724
	void					SPI_SetDropShadowColor( const AColor inColor ) { mDropShadowColor = inColor; }//#724
	void					SPI_SetLetterColor( const AColor inColor, const AColor inDeactiveColor ) { mColor = inColor; mColorDeactive = inDeactiveColor; }
	void					SPI_SetEllipsisMode( const AEllipsisMode inEllipsisMode, const ANumber inCharacterCount )
	{ mEllipsisMode = inEllipsisMode; mEllipsisCharacterCount = inCharacterCount; }//#725
  private:
	void					NVIDO_SetText( const AText& inText );//#379
	void					NVIDO_GetText( AText& outText ) const;//#379
	AEllipsisMode						mEllipsisMode;//#725
	AItemCount							mEllipsisCharacterCount;//#725
	
	//色設定 #291
  public:
	void					SPI_SetAlwaysActiveColors( const ABool inBool ) { mAlwaysActiveColors = inBool; }
	void					SPI_SetOvalHoverColor( const AColor inColor ) { mOvalHoverColor = inColor; }
  private:
	ABool								mAlwaysActiveColors;
	AColor								mDropShadowColor;
	AColor								mOvalHoverColor;
	
	//amazon
  public:
	void					SPI_SetFrameColor( const AColor& inColor ) { mFrameColor = inColor; }
  private:
	AColor								mFrameColor;
	
	//ヘルプタグ設定#661
  public:
	void					SPI_SetHelpTag( const AText& inText1, const AText& inText2, const AHelpTagSide inTagSide = kHelpTagSide_Default );
  private:
	AText								mHelpTagText1;
	AText								mHelpTagText2;
	AHelpTagSide						mHelpTagSide;
	
	//透明度 #291
  public:
	void					SPI_SetTransparency( const AFloatNumber inTransparency ) { mTransparency = inTransparency; }
  private:
	AFloatNumber						mTransparency;
	
	//VSeparator #634
  public:
	ABool					DoMouseDown_VSeparator( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					DoMouseTrack_VSeparator( const ALocalPoint& inLocalPoint );
	void					DoMouseTrackEnd_VSeparator( const ALocalPoint& inLocalPoint );
	
	//#724
  public:
	void					SPI_SetTemporaryInvisible( const ABool inInvisible )
	{ mTemporaryInvisible = inInvisible; NVI_Refresh(); }
  private:
	ABool								mTemporaryInvisible;//#724
	
	//<データ>
  private:
	//#688 ABool								mMouseTrackByLoop;
	AImageID							mIconImageID;
	AImageID							mToggleOnIconImageID;
	AImageID							mHoverIconImageID;
	ABool								mMouseHover;
	ABool								mToggleOn;
	ABool								mToggleMode;
	ABool								mMouseDown;
	ABool								mDisplayingContextMenu;//#724
	ANumber								mIconLeftOffset;
	ANumber								mIconTopOffset;
	ANumber								mIconWidth;
	ANumber								mIconHeight;
	ANumber								mFontHeight;
	ANumber								mFontAscent;
	AJustification						mTextJustification;
	AText								mText;
	AText								mFontName;
	AFloatNumber						mFontSize;
	ATextStyle							mTextStyle;//#724
	AColor								mColor;
	AColor								mColorDeactive;
	AContextMenuID						mMenuID;
	ATextArray							mMenuTextArray;
	ATextArray							mActionTextArray;
	AMenuItemID							mMenuItemID;
	AContextMenuID						mContextMenuID;
	ABoolArray							mContextMenuEnableArray;
	ABool								mEnableDrag;
	ABool								mMouseTrackResultIsDrag;
	AButtonViewType						mButtonViewType;//#634
	AButtonBehaviorType					mButtonBehaviorType;//#634
	AGlobalPoint						mPreviousMouseGlobalPoint;//#634
	ANumber								mButtonValidXRangeStart;//#634
	ANumber								mButtonValidXRangeEnd;//#634
	ABool								mDragging;//#850
	
	//Object Type
  private:
	AViewType				NVIDO_GetViewType() const { return kViewType_Button; }
};

