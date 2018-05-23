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

AWindow_AddToolButton

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AWindow_Text;

#pragma mark ===========================
#pragma mark [クラス]AWindow_AddToolButton
//環境設定ウインドウ
class AWindow_AddToolButton: public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_AddToolButton(/*#199  AWindow& inWindow */const AWindowID inParentWindowID);
	virtual ~AWindow_AddToolButton();
	
	//<関連オブジェクト取得>
  private:
	ADataBase&				NVMDO_GetDB();
	//#199 AWindow&						mWindow;
	AModeIndex						mModeIndex;
	ABool							mEditWithTextWindow;
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
	//#724 void					AddStandardIcon();//#232
	//#724 void					UpdateIconList();//#232
	void					EVTDO_DoCloseButton();//win
	
	//<インターフェイス>
	
	//ウインドウ制御
  public:
	void					SPI_SetMode( const AModeIndex inModeIndex, const ABool inEditWithTextWindow,
							const AText& inInitialTitle, const AText& inToolText );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//#724
	//ツール内容テキスト
  private:
	AText								mToolText;
	
	//<汎化メソッド特化部分>
	
  private:
	void					NVMDO_UpdateControlStatus();
	
	/*#724
	AIconID								mCurrentIconIndexOffset;
	AFileAcc							mFolder;
	
	//#232
  private:
	ATextArray							mStandardToolButtonArray_Title;
	ATextArray							mStandardToolButtonArray_Content;
	AArray<AIconID>						mStandardToolButtonArray_IconID;
	*/
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_AddToolButton; }
};

