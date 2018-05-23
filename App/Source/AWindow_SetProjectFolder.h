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

AWindow_SetProjectFolder

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AWindow_Text;

#pragma mark ===========================
#pragma mark [クラス]AWindow_SetProjectFolder
//

class AWindow_SetProjectFolder : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_SetProjectFolder();
	~AWindow_SetProjectFolder();
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	void					EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder );
	
	//<インターフェイス>
	
	//ウインドウ制御
  public:
	void					SPI_SetFolder( const AFileAcc& inDefaultFolder );//RC3
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Hide();
	void					NVIDO_Show();
	void					NVMDO_UpdateControlStatus();
	
	//AFileAcc							mFolder;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_SetProjectFolder; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_SetProjectFolder"; }
};




