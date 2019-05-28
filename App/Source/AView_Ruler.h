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

AView_Ruler

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AView_Ruler

class AView_Ruler : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_Ruler( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_Ruler();
  private:
	void					NVIDO_Init();//#92
	void					NVIDO_DoDeleted();//#92
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}//win 080706
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	void					EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation );//#575
	void					EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//win
	void					EVTDO_DrawPreProcess();
	void					EVTDO_DrawPostProcess();
	ABool					EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;//#1241

	//<インターフェイス>
  public:
	void					SPI_UpdateProperty( const ADocumentID inTextDocumentID, const ANumber inLeftMargin, const AItemCount inRulerScaleCount );
	void					SPI_UpdateCaretImageX( const ANumber inCaretImageX, const ANumber inSelectImageX );
	void					SPI_UpdateImageSize();
  private:
	void					XorCaret();
	ALocalRect				GetCaretUpdateRect( const ANumber inCaretImageX ) const;
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_Ruler;}
	
	AFloatNumber						mLetterWidth;
	AFloatNumber						mSpaceWidth;//#1421
	AArray<AIndex>						mFlexibleTabPositions;//#1421
	ANumber								mLeftMargin;
	ANumber								mCaretImageX;
	ANumber								mSelectImageX;
	AItemCount							mRulerScaleCount;
	ABool								mCaretDrawn;
};

//#199 ARulerViewFactoryは削除。AViewDefaultFactory<>へ統合。

