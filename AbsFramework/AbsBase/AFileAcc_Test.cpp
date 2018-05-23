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

AFileAcc
ファイル（フォルダ）アクセスクラス

★注意：このソースコードはUTF-8 BOM無しで保存されています。ソース内にUTF-8文字列を使用しているため。
Windows Visual Studio ではUTF-8 BOM無しのソースをコンパイルすると正しくコンパイル出来ないことがあるので、
このソースコードをWindowsでコンパイルするときは何らかの対応が必要です。
（gcc 4.0ではUTF-8 BOM有りをコンパイルできないため、UTF-8 BOM無しにせざるをえませんでした。）

*/

#include "stdafx.h"

#include "ABase.h"
#include "AFileWrapper.h"
#include "../AbsBase/Cocoa.h"

#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::UnitTest()
{
	bool	result = true;
	
	AFileAcc	tmp;
	AFileWrapper::GetDocumentsFolder(tmp);
	AFileAcc	testfolder;
	testfolder.SpecifyUniqChildFile(tmp,"FileAccUnitTest");
	testfolder.CreateFolder();
	AFileAcc	file1;
	file1.SpecifyChild(testfolder,"テスト1");
	file1.CreateFile();
	if( file1.Exist() == false )   result = false;
	
	AFileAcc	file2;
	file2.SpecifySister(file1,"△◕Ṹἥ£ㇷげヹヺ゙ジシ゛ゲポㄗ︻︼█☃☄ἄ");
	file2.CreateFile();
	if( file2.Exist() == false )   result = false;
	AText	file2name;
	file2.GetFilename(file2name,false);
	AFileAcc	file2_;
	file2_.SpecifyChild(testfolder,file2name);
	if( file2.Compare(file2_) == false )   result = false;
	if( file2_.Exist() == false )   result = false;
	AText	displaypath2;
	file2.GetPath(displaypath2,kFilePathType_1);
	file2.Specify(displaypath2,kFilePathType_1);
	if( file2.Exist() == false )   result = false;
	if( file2_.Compare(file2) == false )   result = false;
	
	AFileAcc	file3,parent;
	parent.SpecifyParent(file2);
	if( parent.Compare(testfolder) == false )   result = false;
	file3.SpecifyChild(parent,"あああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああああ");
	file3.CreateFile();
	if( file3.Exist() == false )   result = false;
	AText	filename3new;
	filename3new.AddCstring("△◕Ṹἥ£ㇷげヹヺ゙ジシ゛ゲポㄗ︻︼█☃☄ἄ△◕Ṹἥ£ㇷげヹヺ゙ジシ゛ゲポㄗ︻︼█☃☄ἄ△◕Ṹἥ£ㇷげヹヺ゙ジシ゛ゲポㄗ︻︼█☃☄ἄ△◕Ṹἥ£ㇷげヹヺ゙ジシ゛ゲポㄗ︻︼█☃☄ἄ");
	file3.Rename(filename3new);
	AFileAcc	file3_;
	file3_.SpecifyChild(testfolder,filename3new);
	if( file3_.Exist() == false )   result = false;
	if( file3_.Compare(file3) == false )   result = false;
	
	AFileAcc	folder4;
	folder4.SpecifyChild(testfolder,"フォルダ４");
	AFileAcc	file4_1;
	file4_1.SpecifyChild(folder4,"テスト森鷗外");
	folder4.CreateFolder();
	file4_1.CreateFile();
	AFileAcc	folder5;
	folder5.SpecifyChild(testfolder,"フォルダ５");
	folder4.CopyFolderTo(folder5,false,false);
	AFileAcc	file5_1;
	file5_1.SpecifyChild(folder5,"テスト森鷗外");
	if( folder5.Exist() == false )   result = false;
	if( folder5.IsFolder() == false )   result = false;
	//#1090 decomp文字列でもExist(), IsFolder()が正常動作することを確認する（fileExistsAtPathにはそのまま渡すようにしたので）
	AFileAcc	folder5decomp;
	folder5decomp.SpecifyChild(testfolder,"フォルダ５");//decomp
	if( folder5decomp.Exist() == false )   result = false;
	if( folder5decomp.IsFolder() == false )   result = false;
	//
	if( file5_1.Exist() == false )   result = false;
	file5_1.WriteFile(filename3new);
	AFileAcc	file5_2;
	file5_2.SpecifyChild(folder5,"あいうえお");
	file5_1.CopyFileTo(file5_2,false);
	AText	file5_2text;
	file5_2.ReadTo(file5_2text);
	if( file5_2text.Compare(filename3new) == false )   result = false;
    if( folder5.IsFolder() == false )   result = false;
    if( file5_2.IsFolder() == true )   result = false;
	{
		//★#1168 MBP MacOSX10.10だと、次のチェックでファイルが消えていないことになるので、removeFileAtPathにとりあえず置き換え。file5_2.DeleteFile();
		AText	file5_2_path;
		file5_2.GetPath(file5_2_path);
		AStCreateNSStringFromAText	pathstr(file5_2_path);
		[[NSFileManager defaultManager] removeFileAtPath:pathstr.GetNSString() handler:nil];
	}
	//
    if( file5_2.Exist() )   result = false;
	//#1034 file1.WriteResourceFork(filename3new);
	file1.CopyFileTo(file5_2,false);
    if( file5_2.Exist() == false )   result = false;
	file5_2.ReadTo(file5_2text);
    if( file5_2text.GetLength() > 0 )   result = false;
	/*#1034
	file5_2.ReadTo(file5_2text,true);
	if( file5_2text.Compare(filename3new) == false )   result = false;
	*/
	
	AObjectArray<AFileAcc>	children;
	testfolder.GetChildren(children);
	if( children.GetItemCount() != 5 )   result = false;
	ABool	found = false;
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		if( children.GetObject(i).Compare(file1) )   found = true;
	}
	if( found == false )   result = false;
	
	//テストフォルダ削除
	testfolder.DeleteFileOrFolderRecursive();
	
	return result;
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::UnitTest()
{
	_ACError("not implemented",NULL);
	return false;
}
#endif





