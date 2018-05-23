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

#include "stdafx.h"

#include "AWindow_ModeExecutableAlert.h"
#include "AApp.h"

const AControlID	kFileList			= 5200;
const AControlID	kReport				= 5201;
const AControlID	kImportButton		= 5001;
const AControlID	kCancelButton		= 5002;
//const AControlID	kReportButton		= 5003;
 
#pragma mark ===========================
#pragma mark [クラス]AWindow_ModeExecutableAlert
#pragma mark ===========================
//モード追加ウインドウ

#pragma mark --- コンストラクタ／デストラクタ
//コンストラクタ
AWindow_ModeExecutableAlert::AWindow_ModeExecutableAlert()
		:AWindow(), mModeMode(false), mToolMode(false), mConvertToAutoUpdate(true)//#427
,mModeIndex(kIndex_Invalid),mToolMenuObjectID(kObjectID_Invalid),mRowIndex(kIndex_Invalid)
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

ABool	AWindow_ModeExecutableAlert::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	switch(inID)
	{
	  case kImportButton:
		{
			if( mModeMode == true )
			{
				//モードファイル
				AModeIndex	modeIndex = GetApp().SPI_AddNewModeImportFromFolder(mModeName,mImportFile,true,mConvertToAutoUpdate);//R0000 security #427
				if( modeIndex != kIndex_Invalid )
				{
					GetApp().SPI_ShowModePrefWindow(modeIndex);
				}
			}
			else if( mToolMode == true )
			{
				//ツール
				if( mImportFile.IsFolder() == false )
				{
					mImportFile.CopyFileTo(mDstFile,true);
				}
				else
				{
					mImportFile.CopyFolderTo(mDstFile,true,true);
				}
				GetApp().SPI_GetModePrefDB(mModeIndex).InsertToolItem(mToolMenuObjectID,mRowIndex,mDstFile,true,true,false);//#427
				GetApp().SPI_GetModePrefWindow(mModeIndex).NVI_UpdateProperty();
			}
			NVI_StopModalSession();
			//NVI_Close();
			break;
		}
	  case kCancelButton:
		{
			NVI_StopModalSession();
			//NVI_Close();
			break;
		}
			/*#688
	  case kReportButton:
		{
			AObjectID docID = GetApp().SPNVI_CreateNewTextDocument(kStandardModeIndex);
			if( docID == kObjectID_Invalid )   break;//win
			GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,mReport);
			break;
		}
			 */
	  default:
		{
			return false;
		}
	}
	return true;
}

//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_ModeExecutableAlert::EVTDO_DoCloseButton()
{
	NVI_StopModalSession();
	//NVI_Close();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_ModeExecutableAlert::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/ModeExecutableAlert");
	//#135 NVI_CreateEditBoxView(kFileList,false,true,false,true);
	NVI_CreateEditBoxView(kFileList,true,true,true);
	NVI_GetEditBoxView(kFileList).NVI_SetTextFont(GetEmptyText(),9.0);
	NVI_GetEditBoxView(kFileList).SPI_SetReadOnly();
	//#135 NVI_CreateEditBoxView(kReport,false,true,false,true);
	//#688 NVI_CreateEditBoxView(kReport,true,false,true);
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);//#375
	//#688 NVI_SetControlTextFont(kFileList,/*win AFontWrapper::GetFixWidthFont()*/fontname,10);
	//#688 NVI_SetControlTextFont(kReport,/*win AFontWrapper::GetFixWidthFont()*/fontname,10);
	NVI_RegisterToFocusGroup(kCancelButton,true);//#353
	NVI_RegisterToFocusGroup(kImportButton,true);//#353
	//#688 NVI_RegisterHelpAnchor(90001,"importalert");
	mImportFile.Unspecify();
}

void	AWindow_ModeExecutableAlert::SPI_Set( const AText& inModeName, 
		const AFileAcc& inImportFile, const AText& inFileList, const AText& inReport, const ABool inConvertToAutoUpdate )//#427
{
	mImportFile.CopyFrom(inImportFile);
	mModeName.SetText(inModeName);
	NVI_SetControlText(kFileList,inFileList);
	NVI_SetControlText(kReport,inReport);
	mReport.SetText(inReport);
	mModeMode = true;
	mToolMode = false;
	mConvertToAutoUpdate = inConvertToAutoUpdate;//#427
}

void	AWindow_ModeExecutableAlert::SPI_Set_Tool( const AModeIndex inModeIndex, const AObjectID inToolMenuObjectID, const AIndex inRowIndex, 
		const AFileAcc& inImportFile, const AFileAcc& inDstFile, const AText& inFileList, const AText& inReport )
{
	GetApp().SPI_GetModePrefWindow(inModeIndex).NVI_CreateAndShow();
	mImportFile.CopyFrom(inImportFile);
	mDstFile.CopyFrom(inDstFile);
	mModeIndex = inModeIndex;
	mToolMenuObjectID = inToolMenuObjectID;
	mRowIndex = inRowIndex;
	NVI_SetControlText(kFileList,inFileList);
	NVI_SetControlText(kReport,inReport);
	mReport.SetText(inReport);
	mModeMode = false;
	mToolMode = true;
}

/*
void	AWindow_ModeExecutableAlert::SPI_Set_Toolbar( const AModeIndex inModeIndex, const AIndex inRowIndex, 
		const AFileAcc& inImportFile, const AFileAcc& inDstFile, const AText& inFileList, const AText& inReport )
{
	GetApp().SPI_GetModePrefWindow(inModeIndex).NVI_CreateAndShow();
	mImportFile.CopyFrom(inImportFile);
	mDstFile.CopyFrom(inDstFile);
	mModeIndex = inModeIndex;
	mRowIndex = inRowIndex;
	NVI_SetControlText(kFileList,inFileList);
	NVI_SetControlText(kReport,inReport);
	mReport.SetText(inReport);
	mModeMode = false;
	mToolMode = false;
	mToolbarMode = true;
}
*/
