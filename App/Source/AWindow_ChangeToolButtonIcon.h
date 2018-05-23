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

AWindow_ChangeToolButtonIcon

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AWindow_Text;

#pragma mark ===========================
#pragma mark [クラス]AWindow_ChangeToolButtonIcon
//環境設定ウインドウ
class AWindow_ChangeToolButtonIcon: public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_ChangeToolButtonIcon( /*#199 AWindow& inWindow*/const AWindowID inParentWindowID );
	virtual ~AWindow_ChangeToolButtonIcon();
	
	//<関連オブジェクト取得>
  private:
	ADataBase&				NVMDO_GetDB();
	//#199 AWindow&						mWindow;
	AWindow&				GetParentWindow();
	AWindowID							mParentWindowID;
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	ABool					EVTDO_ValueChanged( const AControlID inID );
	ABool					EVTDO_DoubleClicked( const AControlID inID );
	void					EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile );
	void					EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder );
	void					EVTDO_TextInputted( const AControlID inID );
	void					EVTDO_DoScrollBarAction( const AControlID inID, const AScrollBarPart inPart );
	void					EVTDO_DoCloseButton();//win
	
	//<インターフェイス>
	
	//ウインドウ制御
  public:
	void					SPI_SetFile( const AFileAcc& inFile, const AModeIndex inModeIndex );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//<汎化メソッド特化部分>
	
  private:
	void					NVMDO_UpdateControlStatus();
	
	AFileAcc							mFile;
	AIndex								mCurrentIconIndexOffset;
	AIndex								mModeIndex;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_ChangeToolButtonIcon; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_ChangeToolButtonIcon"; }
};

