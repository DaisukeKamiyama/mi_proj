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

AWindow_ModeExecutableAlert
security
*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AWindow_ModePref;

#pragma mark ===========================
#pragma mark [クラス]AWindow_ModeExecutableAlert
//モード追加ウインドウ
class AWindow_ModeExecutableAlert : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_ModeExecutableAlert();
  private:
	//#199 AWindow_ModePref&					mModePrefWindow;
	AWindowID							mModePrefWindowID;//#199
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoCloseButton();//win
	
	//<インターフェイス>
	
	//ウインドウ制御
  public:
	void					SPI_Set( const AText& inModeName, const AFileAcc& inImportFile, 
							const AText& inFileList, const AText& inReport, const ABool inConvertToAutoUpdate );//#427
	void					SPI_Set_Tool( const AModeIndex inModeIndex, const AObjectID inToolMenuObjectID, const AIndex inRowIndex, 
							const AFileAcc& inImportFile, const AFileAcc& inDstFile, const AText& inFileList, const AText& inReport );
	/*void					SPI_Set_Toolbar( const AModeIndex inModeIndex, const AIndex inRowIndex, 
							const AFileAcc& inImportFile, const AFileAcc& inDstFile, const AText& inFileList, const AText& inReport );*/
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
  private:
	AFileAcc							mImportFile;
	AFileAcc							mDstFile;
	AText								mModeName;
	AModeIndex							mModeIndex;
	AObjectID							mToolMenuObjectID;
	AIndex								mRowIndex;
	AText								mReport;
	ABool								mModeMode;
	ABool								mToolMode;
	ABool								mConvertToAutoUpdate;//#427
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_ModeExecutableAlert; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_ModeExecutableAlert"; }
};



