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

AWindow_AddNewMode

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AWindow_ModePref;

#pragma mark ===========================
#pragma mark [クラス]AWindow_AddNewMode
//モード追加ウインドウ
class AWindow_AddNewMode : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_AddNewMode();
  private:
	
	//<関連オブジェクト取得>
  public:
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_TextInputted( const AControlID inID );
	void					EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder );
	ABool					EVTDO_ValueChanged( const AControlID inControlID );//R0000
	void					EVTDO_DoCloseButton();//win

	//<インターフェイス>
	
	//ウインドウ制御
  public:
	void					SPI_SetImportFolder( const AFileAcc& inImportFolder );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVMDO_UpdateControlStatus();
	ABool					ModeNameValid();
	
	
  private:
	AFileAcc							mImportFolder;//#920
	//#844 ATextArray							mDefaultModeFolderNameArray;//#463
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_AddNewMode; }
};



