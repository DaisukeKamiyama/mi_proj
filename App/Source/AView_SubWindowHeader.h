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

AView_SubWindowHeader
#723

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AView_SubWindowHeader

/**
SubWindowヘッダ
*/
class AView_SubWindowHeader : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_SubWindowHeader( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_SubWindowHeader();
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
  private:
	void					NVIDO_SetText( const AText& inText );
	
	//データ
  private:
	AText								mText;
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_SubWindowHeader;}
};

