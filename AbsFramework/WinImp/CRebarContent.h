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

CRebarContent

*/

#pragma once

#include "CUserPane.h"

#pragma mark ===========================
#pragma mark [クラス]CRebarContent
/**
Rebarコントロール実装クラス
*/
class CRebarContent: public CUserPane
{
	//コンストラクタ／デストラクタ
  public:
	CRebarContent( AObjectArrayItem* inParent, CWindowImp& inWindow );
	~CRebarContent();
	
	//イベントハンドラ
  public:
	ABool	DoDraw();
	ABool	DoMouseDown( const ALocalPoint inLocalPoint, const AModifierKeys inModifiers, 
		const ANumber inClickCount, const ABool inRightButton );
	
};


#pragma mark ===========================
#pragma mark [クラス]CRebarContentFactory
//ColorPickerをObjectArrayで生成するためのクラス
class CRebarContentFactory : public CUserPaneFactory
{
  public:
	CRebarContentFactory( AObjectArrayItem* inParent, CWindowImp& inWindow )
			: mParent(inParent), mWindow(inWindow)
	{
	}
	CRebarContent*	Create() 
	{
		return new CRebarContent(mParent,mWindow);
	}
  private:
	AObjectArrayItem* mParent;
	CWindowImp&	mWindow;
};


