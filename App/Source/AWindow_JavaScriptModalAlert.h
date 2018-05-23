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

AWindow_JavaScriptModalAlert

*/

#pragma once

#include "../../AbsFramework/Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_JavaScriptModalAlert
/**
ModalAlertウインドウ
*/
class AWindow_JavaScriptModalAlert : public AWindow
{
	//コンストラクタ
  public:
	AWindow_JavaScriptModalAlert();
	~AWindow_JavaScriptModalAlert();
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();
	
	//<インターフェイス>
	//ウインドウ制御
  public:
	void					SPNVI_Show( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, ABool& outAborted );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	ABool								mAborted;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_ModalAlert; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_JavaScriptModalAlert"; }
};

