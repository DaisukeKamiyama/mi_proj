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

AWindow_BigFileAlert
R0208
*/

#include "stdafx.h"

#include "AWindow_BigFileAlert.h"
#include "AApp.h"

const AControlID	kOK = 1;
const AControlID	kCancel = 2;
const AControlID	kText = 3;
const AControlID	kFileList = 4;

#pragma mark ===========================
#pragma mark [クラス]AWindow_BigFileAlert
#pragma mark ===========================

//コンストラクタ
AWindow_BigFileAlert::AWindow_BigFileAlert():AWindow(/*#175NULL*/)
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//コントロールのクリック時のコールバック
ABool	AWindow_BigFileAlert::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kOK:
		{
			NVI_Close();//B0410 SPNVI_CreateDocumentFromLocalFile()の前に移動（シート表示中は開いたドキュメントを前面に持ってこないようにしたため）
			//B0419 GetApp().SPNVI_CreateDocumentFromLocalFile(mFile,GetEmptyText(),false);
			//B0419 複数のサイズの大きいファイルを開こうとしたときに１つしか開けない問題の対策
			while( mFileArray.GetItemCount() > 0 )
			{
				GetApp().SPNVI_CreateDocumentFromLocalFile(mFileArray.GetObjectConst(0),GetEmptyText(),kObjectID_Invalid,false);
				mFileArray.Delete1Object(0);
			}
			result = true;
			break;
		}
	  case kCancel:
		{
			NVI_Close();
			mFileArray.DeleteAll();//B0419
			result = true;
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
void	AWindow_BigFileAlert::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/BigFileAlert");
	NVI_SetDefaultOKButton(kOK);//B0452 Cancel->OK
	NVI_SetDefaultCancelButton(kCancel);
	NVI_RegisterToFocusGroup(kCancel,true);//#353
	NVI_RegisterToFocusGroup(kOK,true);//#353
}

void	AWindow_BigFileAlert::SPI_SetFile( const AFileAcc& inFile )
{
	//B0419 mFile.CopyFrom(inFile);
	mFileArray.GetObject(mFileArray.AddNewObject()).CopyFrom(inFile);
	
	//B0419 AText	text;
	//B0419 NVI_GetControlText(kText,text);
	AText	t;
	for( AIndex i = 0; i < mFileArray.GetItemCount(); i++ )
	{
		AText	text;
		text.SetLocalizedText("BigFileAlert_File");
		AText	filename;
		mFileArray.GetObjectConst(i).GetFilename(filename);
		AItemCount	filesize = mFileArray.GetObjectConst(i).GetFileSize();
		AText	filesizetext;
		filesizetext.SetFormattedCstring("%d",filesize);
		text.ReplaceParamText('0',filename);
		text.ReplaceParamText('1',filesizetext);
		t.AddText(text);
	}
	NVI_SetControlText(kFileList,t);
}



