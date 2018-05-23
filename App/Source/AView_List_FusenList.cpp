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

AView_List_FusenList
#138

*/

#include "stdafx.h"

#include "AView_List_FusenList.h"
#include "AApp.h"

const ADataID			kColumn_LineIndex				= 0;
const ADataID			kColumn_ModuleName				= 1;
const ADataID			kColumn_Comment					= 2;

#pragma mark ===========================
#pragma mark [クラス]AView_List_FusenList
#pragma mark ===========================
//情報ウインドウのメインView

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AView_List_FusenList::AView_List_FusenList( const AWindowID inWindowID, const AControlID inID  ) 
	: AView_List(inWindowID,inID), mProjectObjectID(kObjectID_Invalid)
{
}

/**
初期化
*/
void	AView_List_FusenList::NVIDO_ListInit()
{
	//カーソル設定
	NVI_SetDefaultCursor(kCursorType_Arrow);
	//フォント等設定
	AText	fontname;
	AFontWrapper::GetAppDefaultFontName(fontname);//#375
	SPI_SetTextDrawProperty(fontname,10.0,kColor_Black);
	SPI_SetWheelScrollPercent(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
			GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));
	//リスト設定
	SPI_RegisterColumn_Number(kColumn_LineIndex,0,"",false);
	SPI_RegisterColumn_Text(kColumn_ModuleName,250,"",false);
	SPI_RegisterColumn_Text(kColumn_Comment,0,"",false);
	SPI_SetEnableCursorRow();
}

/**
デストラクタ
*/
AView_List_FusenList::~AView_List_FusenList()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//描画要求時のコールバック(AView用)
void	AView_List_FusenList::EVTDO_ListClicked( const AIndex inClickedRowIndex, const AIndex inClickedRowDBIndex, const ANumber inClickCount )
{
	if( inClickedRowDBIndex == kIndex_Invalid )   return;
	
	GetApp().SPI_RevealExternalComment(mProjectObjectID,mRelativePath,
			mModuleNameArray.GetTextConst(inClickedRowDBIndex),mOffsetArray.Get(inClickedRowDBIndex));
	
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---

/**
アクティブドキュメント変更

表示すべき対象のドキュメントが変更されたときにコールされる。
@param inProjectObjectID アクティブになったドキュメントのProjectObjectID
@param inRelativePath アクティブになったドキュメントのプロジェクト相対パス
*/
void	AView_List_FusenList::SPI_ChangeActiveDocument( const AObjectID inProjectObjectID, const AText& inRelativePath )
{
	//現在のアクティブドキュメントと同じなら何もしない
	if( inProjectObjectID == mProjectObjectID && inRelativePath.Compare(mRelativePath) == true )   return;
	
	//アクティブドキュメント更新
	mProjectObjectID = inProjectObjectID;
	mRelativePath.SetText(inRelativePath);
	//テーブル更新
	UpdateTable();
}

/**
リスト更新

付箋紙データが更新された場合などにコールされる
@param inProjectObjectID 更新対象ドキュメントのProjectObjectID
@param inRelativePath 更新対象ドキュメントのプロジェクト相対パス
*/
void	AView_List_FusenList::SPI_UpdateTable( const AObjectID inProjectObjectID, const AText& inRelativePath )
{
	//更新対象ドキュメントが、現在のアクティブドキュメントと同じ場合のみテーブル更新
	if( inProjectObjectID == mProjectObjectID && inRelativePath.Compare(mRelativePath) == true )
	{
		UpdateTable();
	}
}

/**
リスト更新（内部モジュール）
*/
void	AView_List_FusenList::UpdateTable()
{
	//付箋紙データ取得
	
	//DocID取得
	ADocumentID	docID = GetApp().SPI_GetDocumentIDByProjectObjectIDAndPath(mProjectObjectID,mRelativePath,false);//該当なしなら新規doc作成しない
	if( docID == kObjectID_Invalid )   return;
	
	//ドキュメントからデータ取得
	ATextArray	commentArray;
	ANumberArray	lineIndexArray;
	GetApp().SPI_GetTextDocumentByID(docID).SPI_GetExternalCommentArray(mModuleNameArray,mOffsetArray,commentArray,lineIndexArray);
	
	//"モジュール名＋オフセット"の文字列配列を生成
	ATextArray	posArray;
	for( AIndex i = 0; i < mModuleNameArray.GetItemCount(); i++ )
	{
		AText	text;
		text.SetText(mModuleNameArray.GetTextConst(i));
		text.AddCstring(" + ");
		text.AddNumber(mOffsetArray.Get(i));
		posArray.Add(text);
	}
	
	//テーブル更新
	SPI_SetSelect(kIndex_Invalid);
	SPI_BeginSetTable();
	SPI_SetColumn_Number(kColumn_LineIndex,lineIndexArray);
	SPI_SetColumn_Text(kColumn_ModuleName,posArray);
	SPI_SetColumn_Text(kColumn_Comment,commentArray);
	SPI_EndSetTable();
	SPI_Sort(kColumn_LineIndex,true);
	
	//表示更新
	NVI_Refresh();
}

