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

ARegExp
正規表現検索／置換
(C) 2008 Daisuke Kamiyama, All Rights Reserved.

★注意：このソースコードはUTF-8 BOM無しで保存されています。ソース内にUTF-8文字列を使用しているため。
Windows Visual Studio ではUTF-8 BOM無しのソースをコンパイルすると正しくコンパイル出来ないことがあるので、
このソースコードをWindowsでコンパイルするときは何らかの対応が必要です。
（gcc 4.0ではUTF-8 BOM有りをコンパイルできないため、UTF-8 BOM無しにせざるをえませんでした。）

*/

#include "stdafx.h"

#include "ARegExp.h"

#pragma mark テスト

#if IMPLEMENTATION_FOR_MACOSX
//検索（単体テスト用）
// inTargetText: 検索を行うテキスト（最初からinTargetEndPosまでを検索します）
// outFoundText: 検索後マッチした文字列($0)
// outStartPos: 検索にマッチした文字列の開始位置
ABool	TestFind( ARegExp& ioRegExp, AText& inTargetText, AIndex inTargetStartPos, AIndex inTargetEndPos, 
		AText& outFoundText, AIndex& outFoundStartPos )
{
	ABool	result = false;
	if( inTargetEndPos > inTargetText.GetLength() )   inTargetEndPos = inTargetText.GetLength();
	for( AIndex pos = inTargetStartPos; pos <= inTargetEndPos; pos = inTargetText.GetNextCharPos(pos) )
	{
		ioRegExp.InitGroup();
		AIndex	svPos = pos;
		if( ioRegExp.Match(inTargetText,pos,inTargetEndPos) == true )
		{
			AText	repin;
			repin.AddCstring("$0");
			ioRegExp.ChangeReplaceText(inTargetText,repin,outFoundText);
			outFoundStartPos = svPos;
			result = true;
			break;
		}
		if( pos == inTargetEndPos )   break;
	}
	return result;
}

// 検索マッチ文字列をチェック
void	TestRegExp1( ABool& ioResult, const char* inTarget, AIndex inTargetStartPos, AIndex inTargetEndPos, const char* inRegExp, 
					 const char* inExpect, ABool inIsFoundExpected )
{
	ARegExp	regexp;
	
	AText	targettext;
	targettext.SetCstring(inTarget);
	AText	regexptext;
	regexptext.SetCstring(inRegExp);
	AText	expecttext;
	expecttext.SetCstring(inExpect);
	regexp.SetRE(regexptext);
	AText	foundtext;
	AIndex	foundpos;
	if( inIsFoundExpected )
	{
		if( TestFind(regexp,targettext,inTargetStartPos,inTargetEndPos,foundtext,foundpos) == true )
		{
			if( foundtext.Compare(expecttext) == false )
			{
				ioResult = false;
			}
		}
		else ioResult = false;
	}
	else
	{
		if( TestFind(regexp,targettext,inTargetStartPos,inTargetEndPos,foundtext,foundpos) == true )
		{
			ioResult = false;
		}
	}
}

// 検索マッチ文字列、検索マッチ位置をチェック
void	TestRegExp2( ABool& ioResult, const char* inTarget, AIndex inTargetStartPos, AIndex inTargetEndPos, const char* inRegExp, 
					 const char* inExpect, AIndex inExpectPos, ABool inIsFoundExpected )
{
	ARegExp	regexp;
	
	AText	targettext;
	targettext.SetCstring(inTarget);
	AText	regexptext;
	regexptext.SetCstring(inRegExp);
	AText	expecttext;
	expecttext.SetCstring(inExpect);
	regexp.SetRE(regexptext);
	AText	foundtext;
	AIndex	foundpos;
	if( inIsFoundExpected )
	{
		if( TestFind(regexp,targettext,inTargetStartPos,inTargetEndPos,foundtext,foundpos) == true )
		{
			if( foundtext.Compare(expecttext) == false || foundpos != inExpectPos )
			{
				ioResult = false;
			}
		}
		else ioResult = false;
	}
	else
	{
		if( TestFind(regexp,targettext,inTargetStartPos,inTargetEndPos,foundtext,foundpos) == true )
		{
			ioResult= false;
		}
	}
}

// 検索置換結果チェック
void	TestRegExp3( ABool& ioResult, const char* inTarget, const char* inRegExp, const char* inReplace, const char* inExpect, ABool inPerlTest )
{
	ARegExp	regexp;
	
	AText	targettext;
	targettext.SetCstring(inTarget);
	AText	regexptext;
	regexptext.SetCstring(inRegExp);
	AText	replacetext;
	replacetext.SetCstring(inReplace);
	regexp.SetRE(regexptext);
	AText	text;
	text.SetText(targettext);
	AIndex	foundpos;
	AText	foundtext;
	if( TestFind(regexp,targettext,0,targettext.GetItemCount(),foundtext,foundpos) == true )
	{
		AText	t;
		regexp.ChangeReplaceText(targettext,replacetext,t);
		text.Delete(foundpos,foundtext.GetItemCount());
		text.InsertText(foundpos,t);
	}
	if( text.Compare(inExpect) == false )   ioResult = false;
	
	if( inPerlTest == false )   return;
	
	//perl
	AFileAcc	testfolder;
	testfolder.SpecifyAsUnitTestFolder();
	
	AFileAcc	perlfile;
	perlfile.SpecifyChild(testfolder,"xxxxxregexpperltestxxxxx.pl");
	perlfile.CreateFile();
	AText	perltext;
	perltext.SetCstring("$_=\"");
	perltext.AddCstring(inTarget);
	perltext.AddCstring("\";s/");
	AText	escapedslash("\\/");//B0000 \/→\\/
	AText	r;
	r.SetCstring(inRegExp);
	r.ReplaceChar('/',escapedslash);
	perltext.AddText(r);
	perltext.AddCstring("/");
	r.SetCstring(inReplace);
	r.ReplaceChar('/',escapedslash);
	perltext.AddText(r);
	perltext.AddCstring("/;print;");
	perlfile.WriteFile(perltext);
	AFileAcc	resultfile;
	resultfile.SpecifyChild(testfolder,"xxxxxregexpperltestxxxxxresult");
	AText	commandtext("perl ");
	AText	path;
	perlfile.GetPath(path);
	commandtext.AddText(path);
	commandtext.AddCstring(" >");
	resultfile.GetPath(path);
	commandtext.AddText(path);
	{
		AStCreateCstringFromAText	cstr(commandtext);
		system(cstr.GetPtr());
	}
	AText	resulttext;
	resultfile.ReadTo(resulttext);
	if( resulttext.Compare(text) == false )   ioResult = false;
}

ABool	ABaseFunction::TestRegExp()
{
	ABool	result = true;
	
	_AInfo("RegExp Test 1",NULL); 
	TestRegExp3(result,"Hello World Hello world","Hello","Good","Good World Hello world",true);
	if( result == false )   _AInfo("NG",NULL);
	//
	_AInfo("RegExp Test 2",NULL);
	TestRegExp1(result,"ABCDE A123E AAAAA",0,99999999,"A[0-9]+E","A123E",true);
	if( result == false )   _AInfo("NG",NULL);
	
	//B0218 正規表現の|がマルチバイト文字で正常に動作しない
	_AInfo("RegExp Test 3",NULL);
	TestRegExp1(result,"あいうえお\rテ先生スト\r森鷗外先生\r",0,99999999,".+(様|先生|殿)$","森鷗外先生",true);
	if( result == false )   _AInfo("NG",NULL);
	
	//B0047 正規表現"$"で置換時、最終行の置換文字挿入位置が間違っている
	_AInfo("RegExp Test 4",NULL);
	AText	b0047;
	b0047.SetCstring("ああああb");
	TestRegExp2(result,"ああああb",0,99999999,"$","",b0047.GetLength(),true);
	if( result == false )   _AInfo("NG",NULL);
	
	//B0050 選択範囲内で正規表現検索を行った際、選択範囲内でテキストが終了しているものとして、行末、単語区切り、後続指定が判定される
	_AInfo("RegExp Test 5",NULL);
	TestRegExp1(result,"aaa\rbbb\r",0,2,"$","",false);
	TestRegExp1(result,"aaa\rbbb\r",0,2,"\b","",false);
	TestRegExp1(result,"aaa\rbbb\r",0,2,"aa(?=a$)","aa",true);
	if( result == false )   _AInfo("NG",NULL);
	
	//B0056 行全体を選択時、次の行の先頭が正規表現^にヒットしてしまう(2.1.4b4)
	_AInfo("RegExp Test 6",NULL);
	TestRegExp1(result,"aaa\rbbb",1,4,"^","",false);
	TestRegExp2(result,"aaa\rbbb",0,4,"^","",0,true);
	TestRegExp2(result,"aaa\rbbb",1,5,"^","",4,true);
	if( result == false )   _AInfo("NG",NULL);
	
	//B0117 新規ファイルに文字「a」一文字を入力した状態で、正規表現.*(?=a)で前方検索をかけると永遠に終わらない
	//正規表現部分の不具合では無かったが、テストパターンとして。
	_AInfo("RegExp Test 8",NULL);
	TestRegExp2(result,"a",0,999999999,".*(?=a)","",0,true);
	if( result == false )   _AInfo("NG",NULL);
	
	//2.1.7/2.1.8でずっとかかえていたバグ
	_AInfo("RegExp Test 9",NULL);
	//a+
	TestRegExp1(result,"テストaaaaa+++++ AAAAA",0,99999999,"a+","aaaaa",true);
	TestRegExp1(result,"テスト+++++ AAAAA",0,99999999,"a+","aaaaa",false);
	TestRegExp3(result,"テストaaaaa+++++ AAAAA","a+","XXX","テストXXX+++++ AAAAA",true);
	TestRegExp3(result,"テストあああああ+++++ AAAAA","あ+","XXX","テストXXX+++++ AAAAA",false);
	TestRegExp3(result,"テストあああああ+++++ AAAAA","(あ)+","XXX","テストXXX+++++ AAAAA",true);
	//a+?
	TestRegExp3(result,"テストaaaaa+++++ AAAAA","a+?","XXX","テストXXXaaaa+++++ AAAAA",true);
	if( result == false )   _AInfo("NG",NULL);
	
	//B0397　0文字一致繰り返しで無限ループ
	_AInfo("RegExp Test 10",NULL);
	TestRegExp3(result,"test();\rtest",";($)*","XXX","test()XXX\rtest",true);
	TestRegExp3(result,"A\rB\rC\r","A(.*$)+C","XXX","A\rB\rC\r",true);
	TestRegExp3(result,"A\rB\rC\r","A(.*\r)+C","XXX","XXX\r",true);
	TestRegExp3(result,"test();\rtest",";($)*?a","XXX","test();\rtest",true);
	TestRegExp3(result,"test();\rtest",";($)*?\r","XXX","test()XXXtest",true);
	if( result == false )   _AInfo("NG",NULL);
	
	//
	TestRegExp3(result,".....","[.]+","XXX","XXX",true);
	//[]内の.はピリオド文字扱いとなる。Perlと同じ。
	TestRegExp3(result,"AA","[.]+","XXX","AA",true);
	if( result == false )   _AInfo("NG",NULL);
	
	//
	_AInfo("RegExp Test 11",NULL);
	TestRegExp3(result,"\r<tr><td title=><a href=>A</a></td>\r<td></td>\r</tr>\r",
			"<tr>\\s*?<td.*?><a.*?>\\s*?([\\w\\-]+?)</a>[^貘]*?</tr>","$1","\rA\r",false);
	if( result == false )   _AInfo("NG",NULL);
	
	//\\B[0-9]+\\B B0422 \B単語境界以外
	_AInfo("RegExp Test 12",NULL);
	TestRegExp3(result,"a 1234 c","\\B[0-9]+\\B","XXX","a 1XXX4 c",true);
	TestRegExp3(result,"a a23z c","\\B[0-9]+\\B","XXX","a aXXXz c",true);
	TestRegExp3(result,"a(a234)c","\\B[0-9]+\\B","XXX","a(aXXX4)c",true);
	TestRegExp3(result,"a(abcd)c","\\B[a-z]+\\B","XXX","a(aXXXd)c",true);
	if( result == false )   _AInfo("NG",NULL);
	
	//\b
	TestRegExp3(result,"a 1234 c","\\b[0-9]+\\b","XXX","a XXX c",true);
	TestRegExp3(result,"a a23z c","\\b[0-9]+\\b","XXX","a a23z c",true);
	TestRegExp3(result,"a(a234)c","\\b[0-9]+\\b","XXX","a(a234)c",true);
	TestRegExp3(result,"a(abcd)c","\\b[a-z]+\\b","XXX","XXX(abcd)c",true);
	if( result == false )   _AInfo("NG",NULL);
	
	//B0440
	//変更前のOKパターン TestRegExp3(result,"line1\r","^.+\\r^","XXX","XXX",false);
	//変更前のOKパターン TestRegExp3(result,"line1\rline2\r","^.+\\r^.+\\r^","XXX","XXX",false);
	TestRegExp3(result,"line1\r","^.+\\r^","XXX","line1\r",false);
	TestRegExp3(result,"line1\rline2\r","^.+\\r^.+\\r^","XXX","line1\rline2\r",false);
	TestRegExp3(result,"line1\rline2\r","^.+\\r^.+\\r","XXX","XXX",false);
	if( result == false )   _AInfo("NG",NULL);
	
	return result;
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	ABaseFunction::TestRegExp()
{
	//UTF-8 file cannot be handled correctly with VS2008
	return true;
}
#endif
