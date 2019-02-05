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

AWindow_About

*/

#pragma once

#include "../../AbsFramework/Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_About
//Aboutウインドウ
class AWindow_About : public AWindow
{
	//コンストラクタ
  public:
	AWindow_About();
	
	//<イベント処理>
	
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					NVMDO_UpdateControlStatus();//#1384
	
	//<インターフェイス>
	
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	
	//コントロールID
	static const AControlID	kVersion = 1;
	static const AControlID	kButton_WebSite = 10;
	static const AControlID	kUserID = 30;//#1384
	
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_About; }
};

