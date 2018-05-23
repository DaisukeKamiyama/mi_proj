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

AWindow_EnabledDefines
#467

*/

#include "stdafx.h"

#include "AWindow_EnabledDefines.h"
#include "AApp.h"

//
const AControlID		kDefineList						= 100;
const AControlID		kDefineEditBox		 			= 101;
const AControlID		kValueEditBox					= 102;
const AControlID		kAddButton						= 103;
const AControlID		kDeleteButton					= 104;
const AControlID		kUpdateDocumentButton			= 105;

const AIndex			kColumn_DefineList = 0;

#pragma mark ===========================
#pragma mark [クラス]AWindow_EnabledDefines
#pragma mark ===========================
//検索ウインドウ

#pragma mark --- コンストラクタ／デストラクタ

//コンストラクタ
AWindow_EnabledDefines::AWindow_EnabledDefines():AWindow()
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kEnabledDefinesWindowPosition);
	//
	NVI_AddToRotateArray();
	//
	NVI_AddToTimerActionWindowArray();
}

//デストラクタ
AWindow_EnabledDefines::~AWindow_EnabledDefines()
{
}

/**
削除時処理
*/
void	AWindow_EnabledDefines::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
クリック時処理
*/
ABool	AWindow_EnabledDefines::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kAddButton:
		{
			//defineエディットボックスからテキスト取得（スペース削除）
			AText	text;
			NVI_GetControlText(kDefineEditBox,text);
			AText	defineText;
			text.GetTextWithoutSpaces(defineText);
			//define値エディットボックスからテキスト取得（スペース削除）
			NVI_GetControlText(kValueEditBox,text);
			AText	valueText;
			text.GetTextWithoutSpaces(valueText);
			//すでに存在するdefineなら値変更、そうでなければ新規追加
			AIndex	index = GetApp().NVI_GetAppPrefDB().Find_TextArray(AAppPrefDB::kDirectiveEnabledDefine,defineText);
			if( index == kIndex_Invalid )
			{
				GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kDirectiveEnabledDefine,defineText);
				GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,valueText);
			}
			else
			{
				GetApp().NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kDirectiveEnabledDefine,index,defineText);
				GetApp().NVI_GetAppPrefDB().SetData_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,index,valueText);
			}
			UpdateList();
			break;
		}
	  case kDeleteButton:
		{
			//選択を取得
			AArray<AIndex>	selectedDBIndexArray;
			NVI_GetListView(kDefineList).SPI_GetSelectedDBIndexArray(selectedDBIndexArray);
			//順番に並べたあとで、下から削除
			selectedDBIndexArray.Sort(true);
			for( AIndex i = selectedDBIndexArray.GetItemCount()-1; i >= 0; i-- )
			{
				GetApp().NVI_GetAppPrefDB().Delete1_TextArray(AAppPrefDB::kDirectiveEnabledDefine,selectedDBIndexArray.Get(i));
				GetApp().NVI_GetAppPrefDB().Delete1_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,selectedDBIndexArray.Get(i));
			}
			UpdateList();
			break;
		}
	  case kDefineList:
		{
			//選択を取得
			AIndex	sel = NVI_GetListView(kDefineList).SPI_GetSelectedDBIndex();
			if( sel != kIndex_Invalid )
			{
				//テキストボックス更新
				AText	text;
				GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefine,sel,text);
				NVI_SetControlText(kDefineEditBox,text);
				GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,sel,text);
				NVI_SetControlText(kValueEditBox,text);
			}
			break;
		}
	  case kUpdateDocumentButton:
		{
			//文法再認識
			GetApp().SPI_ReRecognizeSyntaxAll(kIndex_Invalid);
			break;
		}
	}
	return result;
}

//
/**
コントロール値変更時処理
*/
ABool	AWindow_EnabledDefines::EVTDO_ValueChanged( const AControlID inControlID )
{
	ABool	result = true;
	switch(inControlID)
	{
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

/**
コントロール状態（Enable/Disable等）を更新
NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()からコールされる
クリックやアクティベートでコールされるので、あまり重い処理をNVMDO_UpdateControlStatus()には入れないこと。
*/
void	AWindow_EnabledDefines::NVMDO_UpdateControlStatus()
{
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

/**
ウインドウ生成
*/
void AWindow_EnabledDefines::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/EnabledDefines");
	
	NVI_CreateListViewFromResource(kDefineList);
	NVI_GetListView(kDefineList).SPI_CreateFrame();
	NVI_GetListView(kDefineList).NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_Invalid);
	NVI_GetListView(kDefineList).SPI_RegisterColumn_Text(kColumn_DefineList,0,"",false);
	NVI_GetListView(kDefineList).SPI_SetEnableDragDrop(true);
	NVI_GetListView(kDefineList).SPI_SetRowMargin(5);
	NVI_GetListView(kDefineList).SPI_SetMultiSelectEnabled(true);
	
	NVI_CreateEditBoxView(kDefineEditBox);
	NVI_CreateEditBoxView(kValueEditBox);
	
	NVI_SetDefaultOKButton(kAddButton);
	
	UpdateList();
}

/**
リスト更新
*/
void	AWindow_EnabledDefines::UpdateList()
{
	//リストのテキスト生成
	ATextArray	textArray;
	for( AIndex i = 0; i < GetApp().NVI_GetAppPrefDB().GetItemCount_Array(AAppPrefDB::kDirectiveEnabledDefine); i++ )
	{
		AText	defineText;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefine,i,defineText);
		AText	valueText;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,i,valueText);
		AText	text;
		text.AddText(defineText);
		if( valueText.GetItemCount() > 0 )
		{
			text.AddCstring(" = ");
			text.AddText(valueText);
		}
		textArray.Add(text);
	}
	//リスト更新
	//#1012 NVI_GetListView(kDefineList).SPI_SetEnableIcon(false);
	NVI_GetListView(kDefineList).SPI_BeginSetTable();
	NVI_GetListView(kDefineList).SPI_SetColumn_Text(kColumn_DefineList,textArray);
	NVI_GetListView(kDefineList).SPI_EndSetTable();
}

/**
defineエディットボックスにテキスト設定
*/
void	AWindow_EnabledDefines::SPI_SetDefineEditBox( const AText& inText )
{
	//スペース系文字を削除
	AText	defineText;
	inText.GetTextWithoutSpaces(defineText);
	//存在するdefineなら値を取得、そうでなければ1をデフォルトとして設定
	AIndex	index = GetApp().NVI_GetAppPrefDB().Find_TextArray(AAppPrefDB::kDirectiveEnabledDefine,defineText);
	AText	valueText("1");
	if( index != kIndex_Invalid )
	{
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,index,valueText);
	}
	NVI_SetControlText(kDefineEditBox,defineText);
	NVI_SetControlText(kValueEditBox,valueText);
	
	//テキストボックスを全選択してフォーカス移動
	NVI_GetEditBoxView(kValueEditBox).NVI_SetSelectAll();
	NVI_SwitchFocusTo(kValueEditBox);
}
