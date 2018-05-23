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

AWindow_TraceLog

*/

#pragma once

#include "../../AbsFramework/Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_TraceLog
//トレースログウインドウ
class AWindow_TraceLog : public AWindow
{
	//コンストラクタ
  public:
	AWindow_TraceLog();
  private:
	AText								mText;//#695
	
	//<インターフェイス>
	
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Show();//#695
	
	//ログ追加
  public:
	void					SPI_AddLog( const AText& inText );
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_TraceLog; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_TraceLog"; }
};

