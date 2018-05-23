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

AView_DocInfo
#142

*/

#pragma once

#include "../../AbsFramework/Wrapper.h"
#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AView_DocInfo

/**
テキストカウントView
*/
class AView_DocInfo : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_DocInfo( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_DocInfo();
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText,
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount ) {return false;}
	void					EVTDO_DoDraw();
	
	//<インターフェイス>
	
	//DocInfo更新
  public:
	void					SPI_UpdateDocInfo_TotalCount( const AItemCount inTotalCharCount,
														 const AItemCount inTotalWordCount,
														 const AItemCount inTotalParagraphCount );
	void					SPI_UpdateDocInfo_SelectedCount( const AItemCount inSelectedCharCount,
															const AItemCount inSelectedWordCount,
															const AItemCount inSelectedParagraphCount,
															const AText& inSelectedText );
	void					SPI_UpdateDocInfo_CurrentChar( const AUCS4Char inChar, 
														  const AUCS4Char inCanonicalDecomp, 
														  const ATextArray& inHeaderTextArray, 
														  const AArray<AIndex>& inHeaderParagraphIndexArray, 
														  const AText& inDebugText );
	void					SPI_UpdateDocInfo_PluginText( const AText& inPluginText );
	
	//<データ>
  private:
	//テキスト情報
	AText								mTotalCharCountText;
	AText								mTotalWordCountText;
	AText								mTotalParagraphCountText;
	AText								mSelectedCharCountText;
	AText								mSelectedWordCountText;
	AText								mSelectedParagraphCountText;
	AUCS4Char							mCurrentChar;
	AUCS4Char							mCurrentCanonicalDecomp;
	AText								mDebugText;
	AText								mPluginText;
	//現在文字
	AText								mCurrentCharText;
	//見出しリスト
	ATextArray							mHeaderTextArray;
	AArray<AIndex>						mHeaderParagraphIndexArray;
	//選択テキスト色
	AText								mSelectedColorText;
	AColor								mSelectedColor;
	//選択テキスト数値
	AText								mSelectedNumberText;
	//テンプレートテキスト
	AText								mTemplateText_Total1;
	AText								mTemplateText_Total2;
	AText								mTemplateText_Total3;
	AText								mTemplateText_Total4;
	AText								mTemplateText_Selected1;
	AText								mTemplateText_Selected2;
	AText								mTemplateText_Selected3;
	AText								mTemplateText_Selected4;
	AText								mTemplateText_CurrentCharReturnCode;
	AText								mTemplateText_CurrentChar;
	AText								mTemplateText_HeaderPath;
	AText								mTemplateText_HeaderPathParagraph;
	AText								mTemplateText_HeaderPathTriangle;
	AText								mTemplateText_SelectedTextColor;
	AText								mTemplateText_SelectedTextNumber;
	
	//Object Type
  private:
	AViewType				NVIDO_GetViewType() const { return kViewType_DocInfo; }
};
