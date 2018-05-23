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

#include "stdafx.h"

#include "AWindow_CompareFolder.h"
#include "AApp.h"

const AControlID	kControlID_Path1 	= 100;
const AControlID	kControlID_Path2 	= 101;
const AControlID	kControlID_OK 		= 1;

#pragma mark ===========================
#pragma mark [クラス]AWindow_CompareFolder
#pragma mark ===========================
//

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_CompareFolder::AWindow_CompareFolder():AWindow(/*#175NULL*/)
{
}
//デストラクタ
AWindow_CompareFolder::~AWindow_CompareFolder()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//コントロールのクリック時のコールバック
ABool	AWindow_CompareFolder::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_OK:
		{
			AFileAcc	file1, file2;
			AText	text;
			NVI_GetControlText(kControlID_Path1,text);
			file1.Specify(text);
			NVI_GetControlText(kControlID_Path2,text);
			file2.Specify(text);
			Compare(file1,file2);
			break;
		}
	}
	return result;
}

void	AWindow_CompareFolder::EVTDO_DoFileDropped( const AControlID inControlID, const AFileAcc& inFile )
{
	switch(inControlID)
	{
	  case kControlID_Path1:
	  case kControlID_Path2:
		{
			AText	text;
			inFile.GetPath(text);
			NVI_SetControlText(inControlID,text);
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_CompareFolder::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/CompareFolder");
	NVI_CreateEditBoxView(100);
	NVI_CreateEditBoxView(101);
	
}

void	AWindow_CompareFolder::Compare( const AFileAcc& inFile1, const AFileAcc& inFile2 )
{
	AText	result;
	AText	text;
	AText	t;
	//存在チェック1
	text.DeleteAll();
	ExistCheckRecursive(inFile1,inFile2,text);
	result.AddCstring("[Files/Folders which only exist in \"");
	inFile1.GetPath(t);
	result.AddText(t);
	result.AddCstring("\" ]\r");
	result.AddText(text);
	result.Add('\r');
	//存在チェック2
	text.DeleteAll();
	ExistCheckRecursive(inFile2,inFile1,text);
	result.AddCstring("[Files/Folders which only exist in \"");
	inFile2.GetPath(t);
	result.AddText(t);
	result.AddCstring("\" ]\r");
	result.AddText(text);
	result.Add('\r');
	//内容チェック
	text.DeleteAll();
	ContentCheckRecursive(inFile1,inFile2,text);
	result.AddCstring("[Files which content is different]\r");
	result.AddText(text);
	result.Add('\r');
	//DirList 1
	text.DeleteAll();
	DirListResursive(inFile1,0,text);
	result.AddCstring("[Directory List of \"");
	inFile1.GetPath(t);
	result.AddText(t);
	result.AddCstring("\" ]\r");
	result.AddText(text);
	result.Add('\r');
	//DirList 2
	text.DeleteAll();
	DirListResursive(inFile2,0,text);
	result.AddCstring("[Directory List of \"");
	inFile2.GetPath(t);
	result.AddText(t);
	result.AddCstring("\" ]\r");
	result.AddText(text);
	result.Add('\r');
	//
	ADocumentID docID = GetApp().SPNVI_CreateNewTextDocument();
	if( docID == kObjectID_Invalid )   return;//win
	GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,result);
}

void	AWindow_CompareFolder::ExistCheckRecursive( const AFileAcc& inBase, const AFileAcc& inTarget, AText& outText )
{
	AObjectArray<AFileAcc>	children;
	inBase.GetChildren(children);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		AFileAcc&	fileb = children.GetObject(i);
		AText	filename;
		fileb.GetRawFilename(filename);
		AFileAcc	filet;
		filet.SpecifyChild(inTarget,filename);
		if( filet.Exist() == false || fileb.IsFolder() != filet.IsFolder() )
		{
			AText	p;
			fileb.GetPath(p);
			outText.AddText(p);
			outText.Add('\r');
		}
		else
		{
			if( fileb.IsFolder() == true )
			{
				if( fileb.IsLink() == false )//#0 エイリアス・シンボリックリンクの場合はとばす
				{
					ExistCheckRecursive(fileb,filet,outText);
				}
			}
			else
			{
				//処理無し
			}
		}
	}
}

void	AWindow_CompareFolder::ContentCheckRecursive( const AFileAcc& inBase, const AFileAcc& inTarget, AText& outText )
{
	AObjectArray<AFileAcc>	children;
	inBase.GetChildren(children);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		AFileAcc&	fileb = children.GetObject(i);
		AText	filename;
		fileb.GetRawFilename(filename);
		AFileAcc	filet;
		filet.SpecifyChild(inTarget,filename);
		if( filet.Exist() == false || fileb.IsFolder() != filet.IsFolder() )
		{
			//処理無し
		}
		else
		{
			if( fileb.IsFolder() == true )
			{
				if( fileb.IsLink() == false )//#0 エイリアス・シンボリックリンクの場合はとばす
				{
					ContentCheckRecursive(fileb,filet,outText);
				}
			}
			else
			{
				AText	textb, textt;
				fileb.ReadTo(textb);
				filet.ReadTo(textt);
				if( textb.Compare(textt) == false )
				{
					AText	p;
					fileb.GetPath(p);
					outText.AddText(p);
					outText.AddCstring("   ");
					filet.GetPath(p);
					outText.AddText(p);
					outText.Add('\r');
				}
			}
		}
	}
}

void	AWindow_CompareFolder::DirListResursive( const AFileAcc& inFolder, const ANumber inDepth, AText& outText )
{
	AText	spaces;
	for( AIndex i = 0; i < inDepth; i++ )
	{
		spaces.Add(' ');
	}
	//
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		AFileAcc	file;
		file.CopyFrom(children.GetObject(i));
		AText	filename;
		file.GetRawFilename(filename);
		outText.AddText(spaces);
		outText.AddText(filename);
		if( file.IsFolder() == true )
		{
			if( file.IsLink() == false )//#0 エイリアス・シンボリックリンクの場合はとばす
			{
				outText.AddCstring("\r");
				DirListResursive(file,inDepth+1,outText);
			}
		}
		else
		{
			AText	command;
			command.SetCstring("/sbin/md5");
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			argArray.Add(filename);
			AText	dirpath;
			inFolder.GetNormalizedPath(dirpath);
			GetApp().SPI_GetChildProcessManager().ExecuteGeneralSyncCommand(command,argArray,dirpath,t);
			outText.AddCstring("   ");
			outText.AddText(t);
			outText.AddCstring("\r");
		}
	}
}


