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

AView_Footer

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AView_Footer

/**
テキストウインドウのフッター
*/
class AView_Footer : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_Footer( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_Footer();
  private:
	void					NVIDO_Init();//#92
	void					NVIDO_DoDeleted();//#92
	
	//<イベント処理>
  private:
	void					EVTDO_DoDraw();
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	ABool					EVTDO_DoTextInput( const AText& inText, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}
	
	//<インターフェイス>
  public:
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_Footer;}
	
};

/*
#pragma mark ===========================
#pragma mark [クラス]AView_FooterSeparator

class AView_FooterSeparator : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_FooterSeparator( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_FooterSeparator();
  private:
	void					NVIDO_Init();//#92
	void					NVIDO_DoDeleted();//#92
	
	//<イベント処理>
  private:
	void					EVTDO_DoDraw();
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}
	
	//<インターフェイス>
  public:
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_FooterSeparator;}
	
};
*/

