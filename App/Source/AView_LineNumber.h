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

AView_LineNumber

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AView_Text.h"

class AWindow_Text;
class CPaneForAView;
class ADocPrefDB;
class AModePrefDB;
class ADocument_Text;

#pragma mark ===========================
#pragma mark [クラス]AView_LineNumber
//各行行番号表示ビュー
class AView_LineNumber : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_LineNumber( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID, 
			/*#199 const AControlID inControlID_TextView*/const AViewID inTextViewID, const AObjectID inTextDocumentID, const AImagePoint& inOriginOfLocalFrame );
	virtual ~AView_LineNumber();
  private:
	void					NVIDO_Init();//win
	void					NVIDO_DoDeleted();//#92
	//#199 AControlID							mControlID_TextView;
	AViewID								mTextViewID;//#199
	AObjectID							mTextDocumentID;
	
	
	//<関連オブジェクト取得>
  public:
	ADocument_Text&			GetTextDocument();
	const ADocument_Text&	GetTextDocumentConst() const;
	AView_Text&				GetTextView();
	const AView_Text&		GetTextViewConst() const;
	ADocPrefDB&				GetDocPrefDB();
	AModePrefDB&			GetModePrefDB();
	
	
	//<イベント処理>
  public:
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#688
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#688
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint );//win 080706
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	void					EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
							ABool& outIsCopyOperation );
	void					EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
  private:
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#450
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );//#450
	ABool					EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;//#992
	void					GetFoldingMarkLocalRect( const AIndex inLineIndex, ALocalRect& outRect ) const;//#450
	void					GetLineImageRect( const AIndex inLineIndex, AImageRect& outLineImageRect ) const;//#450
	void					GetLineImageRect( const AView_Text& inTextView, const AIndex inLineIndex, AImageRect& outLineImageRect ) const;
	AIndex								mFoldingMarkMouseHoverLineIndex;//#450
	AIndex								mMouseHoverLineIndex;//#450
	AIndex								mFoldingHoverStart;//#450
	AIndex								mFoldingHoverEnd;//#450
	
	//<インターフェイス>
	//各種更新処理
  public:
	void					SPI_UpdateText( const AIndex inStartLineIndex, const AIndex inEndLineIndex, 
							const AItemCount inInsertedLineCount, const ABool inParagraphCountChanged );
	void					SPI_UpdateTextDrawProperty();
	void					SPI_UpdateImageSize();
	void					SPI_RefreshLine( const AIndex inLineIndex );//#496
	void					SPI_RefreshLines( const AIndex inStartLineIndex, const AIndex inEndLineIndex );//#695
	ANumber					SPI_Get1DigitWidth() const;//#450
	
	//Diff表示
  public:
	void					SPI_GetDiffDrawData( AArray<ADiffType>& outDiffType, AArray<AIndex>& outDiffIndex,
							AArray<ALocalPoint>& outStartLeftPoint, AArray<ALocalPoint>& outStartRightPoint, 
							AArray<ALocalPoint>& outEndLeftPoint, AArray<ALocalPoint>& outEndRightPoint );//#379
	void							SPI_SetDiffDrawEnable() { mDiffDrawEnable = true; }//#379
  private:
	ABool								mDiffDrawEnable;//#379
	
	//
  public:
	void					SPI_DoListViewHoverUpdated( const AIndex inStartLineIndex, const AIndex inEndLineIndex );
	
	//
  public:
	//void					SPI_XorCrossCaretH( const ANumber inCaretLocalY );
	
	//#1186
  public:
	void					SPI_SetLeftMargin( const ANumber inMargin )
	{
		mLeftMargin = inMargin;
	}
  private:
	ANumber								mLeftMargin;
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_LineNumber;}
	
  public:
	static const ANumber				kSeparatorOffset = 3;//行番号テキスト右端～線まで
	static const ANumber				kRightMargin = 5;//線の右側
	
	//#450
  public:
	static const ANumber				kFoldingMarkLeftMargin = 2;
	static const ANumber				kFoldingMarkSize = 11;//奇数にすること
	static const ANumber				kSmallFoldingMarkSize = 11;//奇数にすること
	
	//Viewオブジェクト取得 #199
  public:
	static AView_LineNumber&		GetLineNumberViewByViewID( const AViewID inViewID )
	{
		MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_LineNumber,kViewType_LineNumber,inViewID);
	}
	static const AView_LineNumber&	GetLineNumberViewConstByViewID( const AViewID inViewID ) 
	{
		MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_VIEWID(AView_LineNumber,kViewType_LineNumber,inViewID);
	}
};


#pragma mark ===========================
#pragma mark [クラス]ALineNumberViewFactory
class ALineNumberViewFactory : public AViewFactory
{
  public:
	ALineNumberViewFactory( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID, 
			/*#199 const AControlID inControlID_TextView*/const AViewID inTextViewID, AObjectID inTextDocumentID, const AImagePoint& inOriginOfLocalFrame )
			: /*#199 mWindow(inWindow)*/mWindowID(inWindowID), mControlID(inID),/*#199 mControlID_TextView(inControlID_TextView)*/mTextViewID(inTextViewID), mTextDocumentID(inTextDocumentID), mOriginOfLocalFrame(inOriginOfLocalFrame)
	{
	}
	AView_LineNumber*	Create() 
	{
		return new AView_LineNumber(/*#199 mParent,mWindow*/mWindowID,mControlID,/*#199 mControlID_TextView*/mTextViewID,mTextDocumentID,mOriginOfLocalFrame);
	}
  private:
	//#199 AObjectArrayItem*	mParent;
	//#199 AWindow&	mWindow;
	AWindowID							mWindowID;//#199
	AControlID							mControlID;
	//#199 AControlID							mControlID_TextView;
	AViewID								mTextViewID;//#199
	AObjectID							mTextDocumentID;
	AImagePoint							mOriginOfLocalFrame;
};

//TextView取得
inline AView_Text&	AView_LineNumber::GetTextView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_Text,kViewType_Text,mTextViewID);//#199
	//#199 MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID_V(AView_Text,kViewType_Text,mControlID_TextView);
	//#199 return dynamic_cast<AView_Text&>(NVM_GetWindow().NVI_GetViewByControlID(mControlID_TextView));
}
inline const AView_Text&	AView_LineNumber::GetTextViewConst() const
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_Text,kViewType_Text,mTextViewID);//#199
	//#199 MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID_V(AView_Text,kViewType_Text,mControlID_TextView);
	//#199 return dynamic_cast<AView_Text&>(NVM_GetWindow().NVI_GetViewByControlID(mControlID_TextView));
}


