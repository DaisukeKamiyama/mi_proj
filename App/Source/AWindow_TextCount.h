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

AWindow_TextCount

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AWindow_Text;

#pragma mark ===========================
#pragma mark [クラス]ATextCountData
//

class ATextCountData
{
  public:
	ABool				countAs2Letters;
	AItemCount			conditionLetterCount;
	ABool				countSpaceTab;
	ABool				countReturn;
	
  public:
	AItemCount			letterCount;
	AItemCount			commentLetterCount;
	AItemCount			paragraphCount;
	
	AItemCount			letterCountInSelect;
	AItemCount			commentLetterCountInSelect;
	AItemCount			paragraphCountInSelect;
	
	AItemCount			lineCountWithCondition;
	
};


#pragma mark ===========================
#pragma mark [クラス]AWindow_TextCount
//

class AWindow_TextCount : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_TextCount();
	~AWindow_TextCount();
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	
	//<インターフェイス>
	
	//ウインドウ制御
  public:
	void					SPI_Update();
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Hide();
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TextCount; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TextCount"; }
};




