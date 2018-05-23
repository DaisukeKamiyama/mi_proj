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

AWindow_FolderList

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_FolderList
//フォルダリスト設定ウインドウ
class AWindow_FolderList : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_FolderList();
	~AWindow_FolderList();
	
	//<関連オブジェクト取得>
  private:
	ADataBase&				NVMDO_GetDB();
	
	
	//<イベント処理>
  private:
	void					EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder );
	ABool					EVTDO_DoubleClicked( const AControlID inID );
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );//B0423
	void					EVTDO_DoFileDropped( const AControlID inControlID, const AFileAcc& inFile );//B0423
	
	//<汎化メソッド特化部分>
	
	//DB連動データが変更されたことを派生クラスへ通知するためのメソッド
  private:
	void					NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
							const AControlID inTriggerControlID/*B0406*/ );
	
	
	//<インターフェイス>
	
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_FolderList; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_FolderList"; }
};





