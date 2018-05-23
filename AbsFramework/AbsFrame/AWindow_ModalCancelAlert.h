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

AWindow_ModalCancelAlert

*/

#pragma once

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_ModalCancelAlert
/**
ModalAlertウインドウ
*/
class AWindow_ModalCancelAlert : public AWindow
{
	//コンストラクタ
  public:
	AWindow_ModalCancelAlert();
	~AWindow_ModalCancelAlert();
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();
	
	//<インターフェイス>
	//ウインドウ制御
  public:
	ABool					SPNVI_Show( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonText, const AText& inCancelButtonText );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	ABool								mCanceled;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_ModalAlert; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_ModalCancelAlert"; }
};

