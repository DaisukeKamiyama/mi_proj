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

AWindow_CompareFolder

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_CompareFolder
//

class AWindow_CompareFolder : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_CompareFolder();
	~AWindow_CompareFolder();
	
	//<イベント処理>
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_DoFileDropped( const AControlID inControlID, const AFileAcc& inFile );
	
	//<インターフェイス>
	
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					Compare( const AFileAcc& inFile1, const AFileAcc& inFile2 );
	void					ExistCheckRecursive( const AFileAcc& inBase, const AFileAcc& inTarget, AText& outText );
	void					ContentCheckRecursive( const AFileAcc& inBase, const AFileAcc& inTarget, AText& outText );
	void					DirListResursive( const AFileAcc& inFolder, const ANumber inDepth, AText& outText );
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_CompareFolder; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_CompareFolder"; }
};




