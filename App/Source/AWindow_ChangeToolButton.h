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

AWindow_ChangeToolButton

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AWindow_Text;

#pragma mark ===========================
#pragma mark [クラス]AWindow_ChangeToolButton
//環境設定ウインドウ
class AWindow_ChangeToolButton: public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_ChangeToolButton(const AWindowID inParentWindowID);
	virtual ~AWindow_ChangeToolButton();
	
	//<関連オブジェクト取得>
  private:
	AWindow&				GetParentWindow();
	AWindowID							mParentWindowID;
	
	//データ
  private:
	AModeIndex							mModeIndex;
	AFileAcc							mFile;
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_TextInputted( const AControlID inID );
	void					EVTDO_DoCloseButton();//win
	
	//<インターフェイス>
	
	//ウインドウ制御
  public:
	void					SPI_SetMode( const AModeIndex inModeIndex, const AFileAcc& inFile );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//<汎化メソッド特化部分>
	
  private:
	void					NVMDO_UpdateControlStatus();
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_ChangeToolButton; }
};

