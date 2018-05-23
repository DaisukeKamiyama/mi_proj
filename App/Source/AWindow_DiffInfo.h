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

AWindow_DiffInfo

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AView_DiffInfo;

#pragma mark ===========================
#pragma mark [クラス]AWindow_DiffInfo
//情報ウインドウ

class AWindow_DiffInfo : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_DiffInfo();
	~AWindow_DiffInfo();
	
	//<イベント処理>
	
	//イベントコールバック
  private:
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	
	//<インターフェイス>
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	AView_DiffInfo&			GetDiffInfoView();
	
	//TextWindowへのリンク
  public:
	void					SPI_SetTextWindowID( const AWindowID inTextWindowID );
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_DiffInfo; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_DiffInfo"; }
};

