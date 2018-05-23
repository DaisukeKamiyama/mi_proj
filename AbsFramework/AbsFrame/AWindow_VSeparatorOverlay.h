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

AWindow_VSeparatorOverlay

*/

#pragma once

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_VSeparatorOverlay
/**
VSperator表示オーバーレイウインドウ
*/
class AWindow_VSeparatorOverlay : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_VSeparatorOverlay();
	~AWindow_VSeparatorOverlay();
	
	//<イベント処理>
	
	//イベントコールバック
  private:
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	
	//<インターフェイス>
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	AControlID				NVIDO_GetMainControlID() const { return kVSperatorViewControlID; }//#602
  private:
	void					UpdateViewBounds();
	
	//Drag通知先ウインドウ設定
  public:
	void					SPI_SetTargetWindowID( const AWindowID inTargetWindowID );
	
	//Separator位置設定
  public:
	void					SPI_SetLinePosition( const ASeparatorLinePosition inLinePosition );
	
	/*#688
	//背景透過率設定 #634
  public:
	void					SPI_SetTransparency( const AFloatNumber inTransparency );
	*/
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_VSeparatorOverlay; }
	
	//#602
  private:
	static const AControlID				kVSperatorViewControlID = 101;
};

