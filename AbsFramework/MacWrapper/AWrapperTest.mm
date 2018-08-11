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

AWrapperTest

★注意：このソースコードはUTF-8 BOM無しで保存されています。ソース内にUTF-8文字列を使用しているため。
Windows Visual Studio ではUTF-8 BOM無しのソースをコンパイルすると正しくコンパイル出来ないことがあるので、
このソースコードをWindowsでコンパイルするときは何らかの対応が必要です。
（gcc 4.0ではUTF-8 BOM有りをコンパイルできないため、UTF-8 BOM無しにせざるをえませんでした。）

*/

#include "../Wrapper.h"

//extern const char* kAllSJISChars;

extern void	MakeAllUnicodeCharText( AText& outText );
extern void	MakeAllSJISCharText( AText& outSJISText );

ABool	WrapperComponentUnitTest()
{
	ABool	result = true;
	try
	{
		//AFileAccテスト #1090
		{
			AFileAcc	a;
			if( a.UnitTest() == false )   _ACThrow("",NULL);
		}
		
		//AScrapWrapper
		_AInfo("AScrapWrapper",NULL);
		{
			AText	osaka("Osaka");
			/*win AFont	osakaFont;
			AFontWrapper::GetFontByName(osaka,osakaFont);*/
			AText	text;
			text.SetCstring("森鷗外");
			AScrapWrapper::SetClipboardTextScrap(text,true,false, false);//#688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(osaka),true);
			AScrapWrapper::GetClipboardTextScrap(text);//#688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(osaka),false);
			if( text.Compare("森鷗外") == false )   _ACThrow("",NULL);
			//
			//検討必要：GetClipboardTextScrapの最後の引数(inConvertToCanonicalComp)をtrueにすると、canonicalcompに変換される。例：U+2000→U+2002
			AText	origText;
			MakeAllUnicodeCharText(origText);
			text.SetText(origText);
			origText.ConvertReturnCodeToCR();//B0439
			AScrapWrapper::SetClipboardTextScrap(text,true,false, false);//#688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(osaka),true);
			AScrapWrapper::GetClipboardTextScrap(text);//#688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(osaka),false);
			if( text.Compare(origText) == false )   _ACThrow("",NULL);
			/*#688
			//非Unicodeアプリ→miへのSJISテキストコピーのテスト B0366
			MakeAllSJISCharText(origText);
			text.SetText(origText);
			AScrapWrapper::SetClipboardScrapData(kScrapType_Text,text,true);
			AScrapWrapper::GetClipboardTextScrap(text,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(osaka),true);
			origText.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			if( text.Compare(origText) == false )   _ACThrow("",NULL);
			//mi→非Unicodeアプリ
			text.SetText(origText);
			AScrapWrapper::SetClipboardTextScrap(text,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(osaka),true);
			AScrapWrapper::GetClipboardScrapData(kScrapType_Text,text);
			text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			if( text.Compare(origText) == false )   _ACThrow("",NULL);
			//B0377
			text.SetCstring("test");
			text.ConvertFromUTF8(ATextEncodingWrapper::GetUTF16BETextEncoding());
			text.Insert1(0,0xFE);
			text.Insert1(1,0xFF);
			AScrapWrapper::SetClipboardScrapData('ut16',text,true);
			AScrapWrapper::GetClipboardTextScrap(text,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(osaka),true);
			if( text.Compare("test") == false )   _ACThrow("",NULL);
			//B0377
			text.SetCstring("森鷗外");
			text.ConvertFromUTF8(ATextEncodingWrapper::GetUTF16BETextEncoding());
			AScrapWrapper::SetClipboardScrapData('ut16',text,true);
			AScrapWrapper::GetClipboardTextScrap(text,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(osaka),true);
			if( text.Compare("森鷗外") == false )   _ACThrow("",NULL);
			*/
		}
		
		//LaunchWrapper B0387
		{
			AAppAcc	appAcc(NULL,NULL);
			AText	text;
			text.SetCstring("mi(a7)");
			appAcc.SetFromToolCommandText(text);
			AFileAcc	file;
			file.Specify("/Applications/mi(a7)/mi(a7).app");
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			text.SetCstring("mi(a7)/mi(a7).app");
			appAcc.SetFromToolCommandText(text);
			file.Specify("/Applications/mi(a7)/mi(a7).app");
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			text.SetCstring("mi(a7)/mi(a7)");
			appAcc.SetFromToolCommandText(text);
			file.Specify("/Applications/mi(a7)/mi(a7).app");
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			text.SetCstring("mi(a7)/mi(a7)");
			appAcc.SetFromToolCommandText(text);
			file.Specify("/Applications/mi(a7)/mi(a7).app");
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			text.SetCstring("Safari");
			appAcc.SetFromToolCommandText(text);
			file.Specify("/Applications/Safari.app");
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			text.SetCstring("TextEdit.app");
			appAcc.SetFromToolCommandText(text);
			file.Specify("/Applications/TextEdit.app");
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			file.GetPath(text,kFilePathType_1);
			appAcc.SetFromToolCommandText(text);
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			text.Delete(text.GetItemCount()-4,4);
			appAcc.SetFromToolCommandText(text);
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			file.GetPath(text,kFilePathType_2);
			appAcc.SetFromToolCommandText(text);
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			text.Delete(text.GetItemCount()-4,4);
			appAcc.SetFromToolCommandText(text);
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			file.GetPath(text,kFilePathType_Default);
			appAcc.SetFromToolCommandText(text);
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
			text.Delete(text.GetItemCount()-4,4);
			appAcc.SetFromToolCommandText(text);
			if( appAcc.GetFile().Compare(file) == false )   _ACThrow("",NULL);
			
		}
		//#1034
		{
			AText	helvetica("Helvetica");
			if( AFontWrapper::IsFontEnabled(helvetica) == false )   _ACThrow("",NULL);
			AText	ngfont("xxxxxxxxx");
			if( AFontWrapper::IsFontEnabled(ngfont) == true )   _ACThrow("",NULL);
		}
	}
	catch(...)
	{
		result = false;
	}
	return result;
}

