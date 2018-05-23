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

#include "stdafx.h"

#include "AWindow_FolderList.h"
#include "AApp.h"
//#include "AUtil.h"

	
const AControlID		kTable = 100;
const AControlID		kButton_Add = 101;
const AControlID		kButton_Remove = 102;
const AControlID		kButton_RemoveOK = 109;
const AControlID	kButton_Up			= 200;//R0183
const AControlID	kButton_Down		= 201;//R0183
const AControlID	kButton_Top			= 202;//R0183
const AControlID	kButton_Bottom		= 203;//R0183

const AIndex		kColumn_Folder = 0;

#pragma mark ===========================
#pragma mark [クラス]AWindow_FolderList
#pragma mark ===========================
//フォルダリスト設定ウインドウ

#pragma mark --- コンストラクタ／デストラクタ
//コンストラクタ
AWindow_FolderList::AWindow_FolderList():AWindow(/*#175NULL*/)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kFolderListWindowPosition);
	//#175
	NVI_AddToRotateArray();
}

//デストラクタ
AWindow_FolderList::~AWindow_FolderList()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

ADataBase&	AWindow_FolderList::NVMDO_GetDB()
{
	return GetApp().GetAppPref();
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

void	AWindow_FolderList::EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder )
{
	//win AIndex	index = NVI_GetControlNumber(kTable);
	//#398 AIndex	index = NVI_GetListView(kTable).SPI_GetSelectedDBIndex();//win
	AIndex	index = NVI_GetTableViewSelectionDBIndex(kTable);//#398
	if( index != kIndex_Invalid )
	{
		AText	path;
		//B0389 inFolder.GetPath(path,kFilePathType_1);
		GetApp().GetAppPref().GetFilePathForDisplay(inFolder,path);//B0389
		NVM_GetDB().SetData_TextArray(AAppPrefDB::kFolderListMenu,index,path);
		NVM_UpdateDBTableView(kTable);
		GetApp().SPI_UpdateFolderListMenu(kModificationType_ValueChanged,index);
	}
}

//B0423
ABool	AWindow_FolderList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kButton_Add:
		{
			GetApp().GetAppPref().AddRow_Table(AAppPrefDB::kFolderListMenu);
			NVM_UpdateDBTableView(kTable);
			AIndex	index = GetApp().GetAppPref().GetRowCount_Table(AAppPrefDB::kFolderListMenu)-1;
			NVMDO_NotifyDataChanged(kTable,kModificationType_RowAdded,index,kControlID_Invalid);
			//最終行を選択
			NVM_SelectTableRow(kTable,index);
			NVM_UpdateControlStatus();
			//#551 
			AFileAcc	defaultfolder;
			AIndex	selected = NVI_GetTableViewSelectionDBIndex(kTable);
			if( selected != kIndex_Invalid )
			{
				AText	path;
				GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFolderListMenu,selected,path);
				defaultfolder.Specify(path);
			}
			//フォルダ選択
			AText	message;
			NVI_ShowChooseFolderWindow(defaultfolder,message,kTable,true);//#551
			break;
		}
	}
	return result;
}

//B0423
void	AWindow_FolderList::EVTDO_DoFileDropped( const AControlID inControlID, const AFileAcc& inFile )
{
	switch(inControlID)
	{
	  case kTable:
		{
			GetApp().GetAppPref().AddRow_Table(AAppPrefDB::kFolderListMenu);
			NVM_UpdateDBTableView(kTable);
			AIndex	index = GetApp().GetAppPref().GetRowCount_Table(AAppPrefDB::kFolderListMenu)-1;
			NVMDO_NotifyDataChanged(kTable,kModificationType_RowAdded,index,kControlID_Invalid);
			//最終行を選択
			NVM_SelectTableRow(kTable,index);
			NVM_UpdateControlStatus();
			//追加
			AFileAcc	folder;
			if( inFile.IsFolder() == true )
			{
				folder.CopyFrom(inFile);
			}
			else
			{
				folder.SpecifyParent(inFile);
			}
			AText	path;
			GetApp().GetAppPref().GetFilePathForDisplay(folder,path);
			NVM_GetDB().SetData_TextArray(AAppPrefDB::kFolderListMenu,index,path);
			NVM_UpdateDBTableView(kTable);
			GetApp().SPI_UpdateFolderListMenu(kModificationType_ValueChanged,index);
			break;
		}
	}
}

ABool	AWindow_FolderList::EVTDO_DoubleClicked( const AControlID inID )
{
	ABool	result = true;
	switch(inID)
	{
	  case kTable:
		{
			//#551 
			AFileAcc	defaultfolder;
			AIndex	selected = NVI_GetTableViewSelectionDBIndex(kTable);
			if( selected != kIndex_Invalid )
			{
				AText	path;
				GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kFolderListMenu,selected,path);
				defaultfolder.Specify(path);
			}
			//
			AText	message;
			NVI_ShowChooseFolderWindow(defaultfolder,message,kTable,true);//#551
			break;
		}
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_FolderList::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/FolderList");
	
	NVM_RegisterDBTable(kTable,AAppPrefDB::kFolderListMenu,
			/*B0423 kButton_Add*/kControlID_Invalid,kButton_Remove,kButton_RemoveOK,
			kButton_Up,kButton_Down,kButton_Top,kButton_Bottom,//R0183
			kControlID_Invalid,true,
				true,false,true,false,kTableViewType_ListView);//win 080618 #688
	NVM_RegisterDBTableColumn(	kTable,kColumn_Folder,			AAppPrefDB::kFolderListMenu,
								kControlID_Invalid,0,"",false);//win 080618
	
	NVI_RegisterToFocusGroup(kTable,true);//#353
	NVI_RegisterToFocusGroup(kButton_Add,true);//#353
	NVI_RegisterToFocusGroup(kButton_Up,true);//#353
	NVI_RegisterToFocusGroup(kButton_Down,true);//#353
	NVI_RegisterToFocusGroup(kButton_Top,true);//#353
	NVI_RegisterToFocusGroup(kButton_Bottom,true);//#353
	NVI_RegisterToFocusGroup(kButton_Remove,true);//#353
	
	NVI_EnableFileDrop(kTable);//#565
}

#pragma mark ===========================

#pragma mark <汎化メソッド特化部分>

#pragma mark ===========================

#pragma mark ---DB連動データが変更されたことを派生クラスへ通知するためのメソッド

void	AWindow_FolderList::NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
		const AControlID inTriggerControlID/*B0406*/ )
{
	GetApp().SPI_UpdateFolderListMenu(inModificationType,inIndex);
}

