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

AZipFile

*/

#include "stdafx.h"

#include "../Frame.h"
#include "../../zlib/unzip.h"

#pragma mark ===========================
#pragma mark [クラス]AZipFile
#pragma mark ===========================
//

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AZipFile::AZipFile( const AFileAcc& inZipFile )
{
	mFile = inZipFile;
}

/**
デストラクタ
*/
AZipFile::~AZipFile()
{
}

#pragma mark ===========================
#pragma mark ---解凍

/**
圧縮
*/
ABool	AZipFile::Zip( const AFileAcc& inFolder )
{
	//zipファイルのパス取得
	AText	path;
	mFile.GetPath(path);
	
#if IMPLEMENTATION_FOR_MACOSX
	{
		AFileAcc	parentFolder;
		parentFolder.SpecifyParent(inFolder);
		AText	parentFolderPath;
		parentFolder.GetPath(parentFolderPath);
		AStCreateCstringFromAText	dirPath(parentFolderPath);
		chdir(dirPath.GetPtr());
	}
	{
		AText	folderName;
		inFolder.GetFilename(folderName);
		AText	command;
		command.SetCstring("zip -r \"");
		command.AddText(path);
		command.AddCstring("\" \"");
		command.AddText(folderName);
		command.AddCstring("\"");
		AStCreateCstringFromAText	cstr(command);
		system(cstr.GetPtr());
	}
	return true;
#else
	return false;
#endif
}

/**
解凍
*/
ABool	AZipFile::Unzip( const AFileAcc& inDstFolder )
{
	//zipファイルのパス取得
	AText	path;
	mFile.GetPath(path);
	
#if IMPLEMENTATION_FOR_MACOSX
	AText	dstFolderPath;
	inDstFolder.GetPath(dstFolderPath);
	AText	command;
	command.SetCstring("unzip \"");
	command.AddText(path);
	command.AddCstring("\" -d \"");
	command.AddText(dstFolderPath);
	command.AddCstring("\"");
	AStCreateCstringFromAText	cstr(command);
	system(cstr.GetPtr());
	return true;
#else
	//
	ABool	result = true;
	
	//zipファイルオープン
	AStCreateCstringFromAText	zipPathcstr(path);
	unzFile	zipFile = unzOpen(zipPathcstr.GetPtr());
	if( zipFile == NULL )   return false;
	ABool	zipByMacosx = false;//MacOSXの場合、ファイル名がUTF-8圧縮だがフラグが立たないので。__MACOSXフォルダ存在により見分ける。
	//ファイルごとのループ
	while(true)
	{
		unz_file_info fileInfo = {0};
		char	pathcstr[4096];
		if( unzGetCurrentFileInfo(zipFile,&fileInfo,pathcstr,4096,NULL,0,NULL,0) == UNZ_OK )
		{
			AText	path;
			path.SetCstring(pathcstr);
			//★もっと確実にMacOSXでの圧縮を見分ける方法があれば、置き換える
			if( path.Compare("__MACOSX/") == true )
			{
				zipByMacosx = true;
			}
			//
			if( ((fileInfo.flag & (0x1<<12)) == 0) && (zipByMacosx == false) )//Language encoding flag http://www.pkware.com/documents/casestudies/APPNOTE.TXT
			{
				ATextEncoding	osdefaulttec = ATextEncodingWrapper::GetOSDefaultTextEncoding();
				path.ConvertToUTF8(osdefaulttec);
			}
			//ファイル or フォルダ
			if( path.GetLastChar() == '/' )
			{
				//ディレクトリの場合
				//ディレクトリ生成
				AFileAcc	folder;
				folder.SpecifyChild(inDstFolder,path);
				if( folder.CreateFolderRecursive() == false )
				{
					//すでにフォルダが存在する場合など。なので、result=falseにはしない。
				}
			}
			else
			{
				//ファイルの場合
				//zip内ファイルオープン
				if( unzOpenCurrentFile(zipFile) != UNZ_OK )
				{
					result = false;
					break;
				}
				//
				AFileAcc	file;
				file.SpecifyChild(inDstFolder,path);
				//親フォルダ生成
				AFileAcc	parentFolder;
				parentFolder.SpecifyParent(file);
				parentFolder.CreateFolderRecursive();
				//ファイル生成
				file.CreateFile();
				//
				AText	data;
				AUChar buf[8192];
				while(true)
				{
					AItemCount len = unzReadCurrentFile(zipFile,buf,sizeof(buf));
					if( len == 0 )   break;
					data.AddFromTextPtr(buf,len);
				}
				//ファイル書き込み
				file.WriteFile(data);
				//zip内ファイルクローズ
				unzCloseCurrentFile(zipFile);
			}
		}
		//次のファイル・フォルダ
		if( unzGoToNextFile(zipFile) == UNZ_END_OF_LIST_OF_FILE )   break;
	}
	//zipファイルクローズ
	unzClose(zipFile);
	return result;
#endif
}


