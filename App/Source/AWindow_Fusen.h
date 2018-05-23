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

AWindow_Fusen

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "AView_Text.h"


#pragma mark ===========================
#pragma mark [クラス]AWindow_Fusen
//インデックスウインドウのクラス

class AWindow_Fusen : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_Fusen( const AViewID inTextViewID,
			const AObjectID inProjectObjectID, const AText& inRelativePath, 
			const AText& inModuleName, const ANumber inOffset );
	~AWindow_Fusen();
	
	//付箋データ識別主キー
  private:
	AObjectID							mProjectObjectID;
	AText								mRelativePath;
	AText								mModuleName;
	ANumber								mOffset;
	
	
	//<関連オブジェクト取得>
  public:
	AView_Text&				GetTextView();
  private:
	AViewID								mTextViewID;
	
	//<イベント処理>
	
	//イベントコールバック
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	void					UpdateViewBounds();
	
	//<インターフェイス>
  public:
	void					SPI_EditFusen();
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//ウインドウ制御
  public:
  private:
	
	//<View>
  private:
	AControlID							mEditBoxViewControlID;
	
	//最小サイズ
  private:
	const static ANumber				kMinWidth = 96;
	const static ANumber				kMinHeight = 32;
	const static ANumber				kEditWindowWidthDelta = 8;
	const static ANumber				kEditWindowHeightDelta = 8;

	
	//ウインドウ情報設定／取得
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_Fusen; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_Fusen"; }
};

#pragma mark ===========================
#pragma mark [クラス]AFusenWindowFactory
class AFusenWindowFactory : public AWindowFactory
{
  public:
	AFusenWindowFactory( const AViewID inTextViewID,
			const AObjectID inProjectObjectID, const AText& inRelativePath, 
			const AText& inModuleName, const ANumber inOffset )
	{
		mTextViewID = inTextViewID;
		mProjectObjectID = inProjectObjectID;
		mRelativePath.SetText(inRelativePath);
		mModuleName.SetText(inModuleName);
		mOffset = inOffset;
	}
	AWindow_Fusen*	Create() 
	{
		return new AWindow_Fusen(mTextViewID,mProjectObjectID,mRelativePath,mModuleName,mOffset);
	}
  private:
	AViewID								mTextViewID;
	AObjectID							mProjectObjectID;
	AText								mRelativePath;
	AText								mModuleName;
	ANumber								mOffset;
};
