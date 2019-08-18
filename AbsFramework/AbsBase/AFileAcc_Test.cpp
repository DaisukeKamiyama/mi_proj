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
	//#1425 CopyFileTo()のテスト追加（上書きコピーテスト）
	file4_1.CopyFileTo(file5_2,true);//上書き禁止
	file5_2.ReadTo(file5_2text);
	if( file5_2text.Compare(filename3new) == false )   result = false;
	file4_1.CopyFileTo(file5_2,false);//上書き許可
	file5_2.ReadTo(file5_2text);
	if( file5_2text.Compare(GetEmptyText()) == false )   result = false;
	//
	file5_2.DeleteFile();
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
	
	//
	AFileAcc	testfolder2;
	testfolder2.SpecifyUniqChildFile(tmp,"FileAccUnitTest2");
	testfolder.CopyFolderTo(testfolder2, true, true);
	
	//#1425 GetLastModifiedDataTime() 旧API置き換え
	ADateTime	datetime;
	ANumber	datetimeDiff = 0;
	if( file4_1.GetLastModifiedDataTime(datetime) == false )   result = false;
	datetimeDiff = ADateTimeWrapper::GetDateTimeDiff(ADateTimeWrapper::GetCurrentTime(), datetime);
	if( datetimeDiff > 300 )   result = false;
	if( folder4.GetLastModifiedDataTime(datetime) == false )   result = false;
	datetimeDiff = ADateTimeWrapper::GetDateTimeDiff(ADateTimeWrapper::GetCurrentTime(), datetime);
	if( datetimeDiff > 300 )   result = false;
	
	//#1425 GetFileAttribute()/SetFileAttribute() 旧API置き換え
	AFileAttribute	attribute, attr1, attr2, attr3, attr4;
	if( file4_1.GetFileAttribute(attr1) == false )   result = false;
	if( attr1.creator != 0 )   result = false;
	if( attr1.type != 0 )   result = false;
	//
	attribute.creator = 'MMKE';
	attribute.type = 'TEXT';
	if( file4_1.SetFileAttribute(attribute) == false )   result = false;
	if( file4_1.GetFileAttribute(attr2) == false )   result = false;
	if( attr2.creator != 'MMKE' )   result = false;
	if( attr2.type != 'TEXT' )   result = false;
	//
	attribute.creator = 'MMKF';
	attribute.type = 'TEXS';
	if( file4_1.SetFileAttribute(attribute) == false )   result = false;
	if( file4_1.GetFileAttribute(attr3) == false )   result = false;
	if( attr3.creator != 'MMKF' )   result = false;
	if( attr3.type != 'TEXS' )   result = false;
	//
	OSType	creator, type;
	file4_1.GetCreatorType(creator, type);
	if( creator != 'MMKF' )   result = false;
	if( type != 'TEXS' )   result = false;
	//
	attribute.creator = 0;
	attribute.type = 0;
	if( file4_1.SetFileAttribute(attribute) == false )   result = false;
	if( file4_1.GetFileAttribute(attr4) == false )   result = false;
	if( attr4.creator != 0 )   result = false;
	if( attr4.type != 0 )   result = false;
	
	//シンボリックリンクテスト
	AFileAcc	file4_symlink;
	file4_symlink.SpecifyChild(folder4, "symlink");
	[[NSFileManager defaultManager] createSymbolicLinkAtURL:file4_symlink.GetNSURL() withDestinationURL:file4_1.GetNSURL() error:nil];
	//
	if( file4_symlink.IsLink() == false )   result = false;
	/*createSymbolicLinkAtURLではエイリアスにはならない。
	AFileAcc	resolved;
	resolved.ResolveAnyLink(file4_symlink);
	AText	p1, p2;
	file4_1.GetPath(p1);
	resolved.GetPath(p2);
	if( p1.Compare(p2) == false )   result = false;
	*/
	file4_symlink.DeleteFile();
	
	//#1404 ファイル削除テスト
	folder4.DeleteFolder();
	folder4.DeleteFile();
	if( folder4.Exist() == false )   result = false;//ファイル削除されていたらエラー（子ファイルがあるので削除できない）
	file4_1.DeleteFile();
	if( file4_1.Exist() == true )   result = false;
	folder4.DeleteFolder();
	if( folder4.Exist() == true )   result = false;
	//
	folder5.DeleteFolder();
	folder5.DeleteFile();
	if( folder5.Exist() == false )   result = false;//ファイル削除されていたらエラー（子ファイルがあるので削除できない）
	file5_1.DeleteFolder();
	if( file5_1.Exist() == true )   result = false;
	file5_2.DeleteFile();
	if( file5_2.Exist() == true )   result = false;
	folder5.DeleteFile();
	if( folder5.Exist() == true )   result = false;
	//
	file1.DeleteFile();
	if( file1.Exist() == true )   result = false;
	file2.DeleteFile();
	if( file2.Exist() == true )   result = false;
	file3.DeleteFile();
	if( file3.Exist() == true )   result = false;
	
	//テストフォルダ削除
	testfolder.DeleteFileOrFolderRecursive();
	if( testfolder.Exist() == true )   result = false;
	testfolder2.DeleteFileOrFolderRecursive();
	if( testfolder2.Exist() == true )   result = false;
	
	//#1425
	//バンドル関連
	AFileAcc	appBundle;
	AText	appPath("/Applications/mi.app");
	appBundle.Specify(appPath);
	if( appBundle.GetBundleFolderAttribute(attribute) == true )
	{
		if( attribute.type != 'APPL' )   result = false;
		if( attribute.creator != 'MMKE' )   result = false;
	}
	else
	{
		result = false;
	}
	
	//#1467 AppleScriptテストのmitestフォルダをボリュームルートから書類フォルダに移動するのに伴い、フォルダ・ファイルもここで生成しておくことにする。
	{
		AFileAcc	docfolder;
		FSRef	ref;
		::FSFindFolder(kUserDomain,kDocumentsFolderType,true,&ref);
		docfolder.SpecifyByFSRef(ref);
		AFileAcc	mitestFolder;
		mitestFolder.SpecifyChild(docfolder,"mitest");
		mitestFolder.CreateFolder();
		AFileAcc	file1;
		file1.SpecifyChild(mitestFolder, "AppleScript_AutoTest001_file.txt");
		file1.CreateFile();
		AFileAcc	file2;
		file2.SpecifyChild(mitestFolder, "AppleScript_AutoTest002_file.txt");
		file2.CreateFile();
	}
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





