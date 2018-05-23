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

AView_CallGrafHeader
#723

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AView_CallGrafHeader

/**
CallGrafヘッダ
*/
class AView_CallGrafHeader : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_CallGrafHeader( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_CallGrafHeader();
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();
	
	//<関連オブジェクト取得>
  public:
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) { return false; }
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint ) {}
	
	//<インターフェイス>
  public:
	void					SPI_SetCurrentStatus( const AIndex inStartLevel, const ATextArray& inCurrentWordArray );
	
	//データ
  private:
	AIndex								mStartLevel;
	ATextArray							mCurrentWordArray;
	
  public:
	//1行の高さ
	static const ANumber						kLineHeight = 18;
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_CallGrafHeader;}
};

