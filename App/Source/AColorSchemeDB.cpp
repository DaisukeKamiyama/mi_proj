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

AColorSchemeDB
#889

*/

#include "stdafx.h"

#include "AColorSchemeDB.h"
#include "AApp.h"

/**
コンストラクタ
*/
AColorSchemeDB::AColorSchemeDB() : ADataBase(NULL)
{
	CreateData_Color(kLetter,		"letter",		kColor_Black);
	CreateData_Color(kControlCode,	"controlCode",	kColor_Gray50Percent);//#1142
	CreateData_Color(kBackground,	"background",	kColor_White);
	CreateData_Color(kComment,		"comment",		kColor_Red);
	CreateData_Color(kLiteral,		"literal",		kColor_Gray20Percent);
	CreateData_Color(kSlot0,		"slot0",		kColor_Black);
	CreateData_Color(kSlot1,		"slot1",		kColor_Black);
	CreateData_Color(kSlot2,		"slot2",		kColor_Black);
	CreateData_Color(kSlot3,		"slot3",		kColor_Black);
	CreateData_Color(kSlot4,		"slot4",		kColor_Black);
	CreateData_Color(kSlot5,		"slot5",		kColor_Black);
	CreateData_Color(kSlot6,		"slot6",		kColor_Black);
	CreateData_Color(kSlot7,		"slot7",		kColor_Black);
	CreateData_Color(kHighlight1,	"Highlight1",	kColor_DeepYellow);
	CreateData_Color(kHighlight2,	"Highlight2",	kColor_Lavender);
	CreateData_Color(kHighlight3,	"Highlight3",	kColor_Yellow);
	CreateData_Color(kHighlight4,	"Highlight4",	kColor_Bisque);
	CreateData_Color(kHighlight5,	"Highlight5",	kColor_Gold);
	CreateData_Number(kDiffOpacity,	"DiffOpacity",	30,				1,100);
	CreateData_Number(kSelectionOpacity,"SelectionOpacity",	80,				1,100);
}

/**
カラースキームファイルから読み込み
*/
void	AColorSchemeDB::LoadFromColorSchemeFile( const AFileAcc& inFile, const AColorSchemeType inType )
{
	switch(inType)
	{
	  case kColorSchemeType_CSV:
		{
			LoadFromCSVFile(inFile);
			break;
		}
	  case kColorSchemeType_EPF:
		{
			LoadFromEPFFile(inFile);
			break;
		}
	}
}

/**
EPFファイルから読み込み
*/
void	AColorSchemeDB::LoadFromEPFFile( const AFileAcc& inFile )
{
	//全データをデフォルトに設定（CSVファイルに未設定の場合に、GetColor()で色を変更しないようにする）
	SetAllDataToDefault();
	//EPFファイル読み込み
	AText	text;
	inFile.ReadTo(text);
	//文字色
	AIndex	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.ui.editors/AbstractTextEditor.Color.Foreground=",pos,true,true) == true )
	{
		SetData_Color(kLetter,ParseFromEqual_EPFFile(text,pos));
	}
	//背景色
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.ui.editors/AbstractTextEditor.Color.Background=",pos,true,true) == true )
	{
		SetData_Color(kBackground,ParseFromEqual_EPFFile(text,pos));
	}
	//コメント
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/java_multi_line_comment=",pos,true,true) == true )
	{
		SetData_Color(kComment,ParseFromEqual_EPFFile(text,pos));
	}
	//リテラル
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/java_string=",pos,true,true) == true )
	{
		SetData_Color(kLiteral,ParseFromEqual_EPFFile(text,pos));
	}
	//reserved-word
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/java_keyword=",pos,true,true) == true )
	{
		SetData_Color(kSlot0,ParseFromEqual_EPFFile(text,pos));
	}
	//method
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/semanticHighlighting.method.color=",pos,true,true) == true )
	{
		SetData_Color(kSlot1,ParseFromEqual_EPFFile(text,pos));
	}
	//property
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/semanticHighlighting.parameterVariable.color=",pos,true,true) == true )
	{
		SetData_Color(kSlot2,ParseFromEqual_EPFFile(text,pos));
	}
	//variable
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/semanticHighlighting.localVariable.color=",pos,true,true) == true )
	{
		SetData_Color(kSlot3,ParseFromEqual_EPFFile(text,pos));
	}
	//class
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/semanticHighlighting.class.color=",pos,true,true) == true )
	{
		SetData_Color(kSlot4,ParseFromEqual_EPFFile(text,pos));
	}
	//macro
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/semanticHighlighting.typeParameter.color=",pos,true,true) == true )
	{
		SetData_Color(kSlot5,ParseFromEqual_EPFFile(text,pos));
	}
	//other1
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/semanticHighlighting.staticField.color=",pos,true,true) == true )
	{
		SetData_Color(kSlot6,ParseFromEqual_EPFFile(text,pos));
	}
	//other2
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/java_doc_default=",pos,true,true) == true )
	{
		SetData_Color(kSlot7,ParseFromEqual_EPFFile(text,pos));
	}
	/*
	//検索ハイライト
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.ui.editors/searchResultIndicationColor=",pos,true,true) == true )
	{
		SetData_Color(kHighlight1,ParseFromEqual_EPFFile(text,pos));
	}
	//検索ハイライト２
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/java_keyword=",pos,true,true) == true )
	{
		SetData_Color(kHighlight2,ParseFromEqual_EPFFile(text,pos));
	}
	//現在単語
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/java_keyword=",pos,true,true) == true )
	{
		SetData_Color(kHighlight3,ParseFromEqual_EPFFile(text,pos));
	}
	//テキストマーカー
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/java_keyword=",pos,true,true) == true )
	{
		SetData_Color(kHighlight4,ParseFromEqual_EPFFile(text,pos));
	}
	//見出し等から選択した時の選択色
	pos = 0;
	if( text.FindCstring(0,"/instance/org.eclipse.jdt.ui/java_keyword=",pos,true,true) == true )
	{
		SetData_Color(kHighlight5,ParseFromEqual_EPFFile(text,pos));
	}
	*/
}

/**
epfファイルの=から後をparseする
*/
AColor	AColorSchemeDB::ParseFromEqual_EPFFile( const AText& inText, const AIndex inPos ) const
{
	AIndex	pos = inPos;
	AText	token;
	inText.GetToken(pos,token);
	if( token.Compare("#") == true )
	{
		inText.GetToken(pos,token);
		return AColorWrapper::GetColorByHTMLFormatColor(token);
	}
	else
	{
		AText	htmlColorText;
		AText	t;
		t.SetFormattedCstring("%02X",token.GetNumber());
		htmlColorText.AddText(t);
		inText.GetToken(pos,token);//,
		inText.GetToken(pos,token);//green
		t.SetFormattedCstring("%02X",token.GetNumber());
		htmlColorText.AddText(t);
		inText.GetToken(pos,token);//,
		inText.GetToken(pos,token);//blue
		t.SetFormattedCstring("%02X",token.GetNumber());
		htmlColorText.AddText(t);
		return AColorWrapper::GetColorByHTMLFormatColor(htmlColorText);
	}
}

/**
CSVファイルから読み込み
*/
void	AColorSchemeDB::LoadFromCSVFile( const AFileAcc& inFile )
{
	//全データをデフォルトに設定（CSVファイルに未設定の場合に、GetColor()で色を変更しないようにする）
	SetAllDataToDefault();
	//CSVファイル読み込み
	AText	text;
	inFile.ReadTo(text);
	//CSVファイルparse
	ACSVFile	csv(text,ATextEncodingWrapper::GetUTF8TextEncoding());
	ATextArray	slotNameArray, colorArray;
	csv.GetColumn(0,slotNameArray);
	csv.GetColumn(1,colorArray);
	//CSVファイル各行ごとのループ
	AItemCount	itemCount = slotNameArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( i >= colorArray.GetItemCount() )
		{
			break;
		}
		//CSVから、スロット名、色取得
		AText	name;
		slotNameArray.Get(i,name);
		AText	colorText;
		colorArray.Get(i,colorText);
		AColor	color =  AColorWrapper::GetColorByHTMLFormatColor(colorText);
		ANumber	num = colorText.GetNumber();
		//スロット名に従ってそれぞれデータ設定
		if( name.Compare("letter") == true )
		{
			SetData_Color(kLetter,color);
		}
		else if( name.Compare("background") == true )
		{
			SetData_Color(kBackground,color);
		}
		//#1142
		else if( name.Compare("control-code") == true )
		{
			SetData_Color(kControlCode,color);
		}
		else switch(GetColorSlotIndexFromName(name))
		{
		  case 0:
			{
				SetData_Color(kSlot0,color);
				break;
			}
		  case 1:
			{
				SetData_Color(kSlot1,color);
				break;
			}
		  case 2:
			{
				SetData_Color(kSlot2,color);
				break;
			}
		  case 3:
			{
				SetData_Color(kSlot3,color);
				break;
			}
		  case 4:
			{
				SetData_Color(kSlot4,color);
				break;
			}
		  case 5:
			{
				SetData_Color(kSlot5,color);
				break;
			}
		  case 6:
			{
				SetData_Color(kSlot6,color);
				break;
			}
		  case 7:
			{
				SetData_Color(kSlot7,color);
				break;
			}
		  case 32:
			{
				SetData_Color(kComment,color);
				break;
			}
		  case 33:
			{
				SetData_Color(kLiteral,color);
				break;
			}
		  case 64:
			{
				SetData_Color(kHighlight1,color);
				break;
			}
		  case 65:
			{
				SetData_Color(kHighlight2,color);
				break;
			}
		  case 66:
			{
				SetData_Color(kHighlight3,color);
				break;
			}
		  case 67:
			{
				SetData_Color(kHighlight4,color);
				break;
			}
		  case 68:
			{
				SetData_Color(kHighlight5,color);
				break;
			}
		  case 96:
			{
				SetData_Number(kDiffOpacity,num);
				break;
			}
		  case 97:
			{
				SetData_Number(kSelectionOpacity,num);
				break;
			}
		}
	}
}

/**
スロット名からスロット番号取得
*/
AIndex	AColorSchemeDB::GetColorSlotIndexFromName( const AText& inName )
{
	if( inName.Compare("comment") == true )
	{
		return 32;
	}
	else if( inName.Compare("literal") == true )
	{
		return 33;
	}
	else if( inName.Compare("reserved-word") == true )
	{
		return 0;
	}
	else if( inName.Compare("method") == true )
	{
		return 1;
	}
	else if( inName.Compare("property") == true )
	{
		return 2;
	}
	else if( inName.Compare("variable") == true )
	{
		return 3;
	}
	else if( inName.Compare("class") == true )
	{
		return 4;
	}
	else if( inName.Compare("macro") == true )
	{
		return 5;
	}
	else if( inName.Compare("other1") == true )
	{
		return 6;
	}
	else if( inName.Compare("other2") == true )
	{
		return 7;
	}
	else if( inName.Compare("highlight1") == true )
	{
		return 64;
	}
	else if( inName.Compare("highlight2") == true )
	{
		return 65;
	}
	else if( inName.Compare("highlight3") == true )
	{
		return 66;
	}
	else if( inName.Compare("highlight4") == true )
	{
		return 67;
	}
	else if( inName.Compare("highlight5") == true )
	{
		return 68;
	}
	else if( inName.Compare("diff-opacity") == true )
	{
		return 96;
	}
	else if( inName.Compare("selection-opacity") == true )
	{
		return 97;
	}
	else
	{
		return kIndex_Invalid;
	}
}

/**
スロットに対おする色取得
*/
void	AColorSchemeDB::GetColor( const APrefID inPrefID, AColor& ioColor ) const
{
	//CSVファイルに色味設定の場合は、ioColorの色を変更せずに、リターン
	if( IsStillDefault(inPrefID) == true )   return;
	//色取得
	GetData_Color(inPrefID,ioColor);
}


