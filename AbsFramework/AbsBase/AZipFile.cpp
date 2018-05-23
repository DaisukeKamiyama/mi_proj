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
#pragma mark [�N���X]AZipFile
#pragma mark ===========================
//

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AZipFile::AZipFile( const AFileAcc& inZipFile )
{
	mFile = inZipFile;
}

/**
�f�X�g���N�^
*/
AZipFile::~AZipFile()
{
}

#pragma mark ===========================
#pragma mark ---��

/**
���k
*/
ABool	AZipFile::Zip( const AFileAcc& inFolder )
{
	//zip�t�@�C���̃p�X�擾
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
��
*/
ABool	AZipFile::Unzip( const AFileAcc& inDstFolder )
{
	//zip�t�@�C���̃p�X�擾
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
	
	//zip�t�@�C���I�[�v��
	AStCreateCstringFromAText	zipPathcstr(path);
	unzFile	zipFile = unzOpen(zipPathcstr.GetPtr());
	if( zipFile == NULL )   return false;
	ABool	zipByMacosx = false;//MacOSX�̏ꍇ�A�t�@�C������UTF-8���k�����t���O�������Ȃ��̂ŁB__MACOSX�t�H���_���݂ɂ�茩������B
	//�t�@�C�����Ƃ̃��[�v
	while(true)
	{
		unz_file_info fileInfo = {0};
		char	pathcstr[4096];
		if( unzGetCurrentFileInfo(zipFile,&fileInfo,pathcstr,4096,NULL,0,NULL,0) == UNZ_OK )
		{
			AText	path;
			path.SetCstring(pathcstr);
			//�������Ɗm����MacOSX�ł̈��k������������@������΁A�u��������
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
			//�t�@�C�� or �t�H���_
			if( path.GetLastChar() == '/' )
			{
				//�f�B���N�g���̏ꍇ
				//�f�B���N�g������
				AFileAcc	folder;
				folder.SpecifyChild(inDstFolder,path);
				if( folder.CreateFolderRecursive() == false )
				{
					//���łɃt�H���_�����݂���ꍇ�ȂǁB�Ȃ̂ŁAresult=false�ɂ͂��Ȃ��B
				}
			}
			else
			{
				//�t�@�C���̏ꍇ
				//zip���t�@�C���I�[�v��
				if( unzOpenCurrentFile(zipFile) != UNZ_OK )
				{
					result = false;
					break;
				}
				//
				AFileAcc	file;
				file.SpecifyChild(inDstFolder,path);
				//�e�t�H���_����
				AFileAcc	parentFolder;
				parentFolder.SpecifyParent(file);
				parentFolder.CreateFolderRecursive();
				//�t�@�C������
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
				//�t�@�C����������
				file.WriteFile(data);
				//zip���t�@�C���N���[�Y
				unzCloseCurrentFile(zipFile);
			}
		}
		//���̃t�@�C���E�t�H���_
		if( unzGoToNextFile(zipFile) == UNZ_END_OF_LIST_OF_FILE )   break;
	}
	//zip�t�@�C���N���[�Y
	unzClose(zipFile);
	return result;
#endif
}


