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

AProjectPrefDB
RC3
*/

#include "stdafx.h"

#include "AProjectPrefDB.h"
#include "AApp.h"

//
AProjectPrefDB::AProjectPrefDB( AObjectArrayItem* inParent, const AText& inProjectFilePath ) : ADataBase(inParent)
{
	mProjectFilePath.SetText(inProjectFilePath);
	
	//				データID									ファイル用Key							初期値			(最小,最大)			(最小,最大)
	CreateTableStart();
	CreateData_TextArray(kExternalComment_PathAndModuleName,"ExternalComment_PathAndModuleName","");
	CreateData_TextArray(kExternalComment_Comment,			"ExternalComment_Comment",			"");
	CreateTableEnd();
	
}

//
void	AProjectPrefDB::Load()
{
	AFileAcc	file;
	file.Specify(mProjectFilePath);
	LoadFromPrefTextFile(file);
}

//
void	AProjectPrefDB::Save()
{
	AFileAcc	file;
	file.Specify(mProjectFilePath);
	WriteToPrefTextFile(file);
}

//#138
/**
プロジェクトフォルダのパス取得
*/
void	AProjectPrefDB::GetProjectFolderPath( AText& outFolderPath ) const
{
	AFileAcc	file;
	file.Specify(mProjectFilePath);
	AFileAcc	folder;
	folder.SpecifyParent(file);
	folder.GetPath(outFolderPath);
}
