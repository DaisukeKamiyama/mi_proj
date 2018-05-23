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

AWindow_FusenEdit

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AView_Text;

#pragma mark ===========================
#pragma mark [クラス]AWindow_FusenEdit
/**
付箋紙編集ウインドウのクラス
*/
class AWindow_FusenEdit : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_FusenEdit();
	~AWindow_FusenEdit();
	
	//<関連オブジェクト取得>
  public:
  private:
	AView_Text&				GetTextView();
	AViewID								mTextViewID;
	
	//付箋データ識別主キー
  private:
	AObjectID							mProjectObjectID;
	AText								mRelativePath;
	AText								mModuleName;
	ANumber								mOffset;
	
	//<イベント処理>
	
	//イベントコールバック
  private:
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	void					EVTDO_WindowDeactivated();
	void					EVTDO_DoCloseButton();
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	
	//<インターフェイス>
  public:
	void					SPNVI_Show( const AObjectID inProjectObjectID, const AText& inRelativePath, 
							const AText& inModuleName, const ANumber inOffset,
							const ARect& inDisplayBounds,
							const AViewID inTextViewID );
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Hide();
	void					ApplyData();
	
	//ウインドウ制御
  public:
  private:
	void					UpdateViewBounds();
	
	//<内包View>
  private:
	AControlID							mEditBoxViewControlID;
	
	//<表示データ>
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_FusenEdit; }
	
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_FusenEdit"; }
};

