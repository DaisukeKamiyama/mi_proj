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

AWindow_MultiFileFindProgress

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AFileAcc;

#pragma mark ===========================
#pragma mark [クラス]AWindow_MultiFileFindProgress
//ジャンプリストウインドウのクラス

class AWindow_MultiFileFindProgress : public AWindow
{
	//コンストラクタ
  public:
	AWindow_MultiFileFindProgress();
	
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	
	
	//<インターフェイス>
	
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//プログレス表示制御
  public:
	void					SPI_SetProgressText( const AText& inText );
	void					SPI_SetProgress( const short inPercent );
	
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_MultiFileFindProgress; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_MultiFileFindProgress"; }
};

