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

AView_DiffInfo
#379

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AView_DiffInfo

class AView_DiffInfo : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_DiffInfo( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_DiffInfo();
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) { return false; }
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, 
							const ALocalPoint inLocalPoint ) {}
	
	//TextWindowへのリンク
  public:
	void					SPI_SetTextWindowID( const AWindowID inTextWindowID )
	{ mTextWindowID = inTextWindowID; }
  private:
	AWindowID										mTextWindowID;
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_DiffInfo;}
};

