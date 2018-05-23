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

AView_TextPanel

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AView_TextPanel

/**
static Text表示のためのView
*/
class AView_TextPanel : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_TextPanel( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_TextPanel();
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
	
	//<インターフェイス>
  public:
	
	//
  private:
	void					NVIDO_SetTextFont( const AText& inFontName, const AFloatNumber inFontSize );
	void					NVIDO_SetText( const AText& inText );
	void					NVIDO_SetTextProperty( const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification );
  private:
	AText								mFontName;
	AFloatNumber						mFontSize;
	AText								mText;
	AColor								mTextColor;
	ATextStyle							mTextStyle;
	AJustification						mTextJustification;
	
	//<データ>
  private:
	
	//Object Type
  private:
	AViewType				NVIDO_GetViewType() const { return kViewType_TextPanel; }
};
