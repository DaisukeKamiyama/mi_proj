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

AToolPref
#305
*/

#include "stdafx.h"

#include "AToolPrefDB.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AToolPrefDB
#pragma mark ===========================
//ModePrefDB

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AToolPrefDB::AToolPrefDB(): ADataBase(NULL)
{
	//				データID									ファイル用Key							初期値			(最小,最大)			(最小,最大)
	CreateData_Text(kVersionText,								"VersionText",						"");
	
	CreateTableStart();
	CreateData_TextArray(kToolArray_FileName,					"ToolArray_FileName",				"");
	CreateData_BoolArray(kToolArray_Enabled,					"ToolArray_Enabled",				true);
	CreateData_NumberArray(kToolArray_Shortcut,					"ToolArray_Shortcut",				0,			0,65535);//#695 ,false);
	CreateData_BoolArray(kToolArray_ShortcutModifiers_ControlKey,"ToolArray_ShortcutModifiers_ControlKey",false);
	CreateData_BoolArray(kToolArray_ShortcutModifiers_ShiftKey,	"ToolArray_ShortcutModifiers_ShiftKey",false);
	CreateData_BoolArray(kToolArray_Grayout,					"ToolArray_Grayout",				false);
	CreateData_BoolArray(kToolArray_EnabledForMacroBar,			"ToolArray_EnabledForMacroBar",		true);
	
	CreateData_TextArray(kToolArray_JapaneseName,				"ToolArray_JapaneseName",			"");
	CreateData_TextArray(kToolArray_EnglishName,				"ToolArray_EnglishName",			"");
	CreateData_TextArray(kToolArray_FrenchName,					"ToolArray_FrenchName",				"");
	CreateData_TextArray(kToolArray_GermanName,					"ToolArray_GermanName",				"");
	CreateData_TextArray(kToolArray_SpanishName,				"ToolArray_SpanishName",			"");
	CreateData_TextArray(kToolArray_ItalianName,				"ToolArray_ItalianName",			"");
	CreateData_TextArray(kToolArray_DutchName,					"ToolArray_DutchName",				"");
	CreateData_TextArray(kToolArray_SwedishName,				"ToolArray_SwedishName",			"");
	CreateData_TextArray(kToolArray_NorwegianName,				"ToolArray_NorwegianName",			"");
	CreateData_TextArray(kToolArray_DanishName,					"ToolArray_DanishName",				"");
	CreateData_TextArray(kToolArray_FinnishName,				"ToolArray_FinnishName",			"");
	CreateData_TextArray(kToolArray_PortugueseName,				"ToolArray_PortugueseName",			"");
	CreateData_TextArray(kToolArray_SimplifiedChineseName,		"ToolArray_SimplifiedChineseName",	"");
	CreateData_TextArray(kToolArray_TraditionalChineseName,		"ToolArray_TraditionalChineseName",	"");
	CreateData_TextArray(kToolArray_KoreanName,					"ToolArray_KoreanName",				"");
	CreateData_TextArray(kToolArray_PolishName,					"ToolArray_PolishName",				"");
	CreateData_TextArray(kToolArray_PortuguesePortugalName,		"ToolArray_PortuguesePortugalName",	"");
	CreateData_TextArray(kToolArray_RussianName,				"ToolArray_RussianName",			"");
	CreateTableEnd();
}
//デストラクタ
AToolPrefDB::~AToolPrefDB()
{
}

//#427
/**
多言語ツール名称のPrefIDを取得
*/
void	AToolPrefDB::GetLanguagePrefIDArray( AArray<APrefID>& outArray )
{
	outArray.DeleteAll();
	for( int i = 500; i <= 517; i++ )
	{
		outArray.Add(i);
	}
	/*なぜかkToolArray_JapaneseName等がリンクエラーになるので上記のように直値を使う
	outArray.Add(kToolArray_JapaneseName);
	outArray.Add(kToolArray_EnglishName);
	outArray.Add(kToolArray_FrenchName);
	outArray.Add(kToolArray_GermanName);
	outArray.Add(kToolArray_SpanishName);
	outArray.Add(kToolArray_ItalianName);
	outArray.Add(kToolArray_DutchName);
	outArray.Add(kToolArray_SwedishName);
	outArray.Add(kToolArray_NorwegianName);
	outArray.Add(kToolArray_DanishName);
	outArray.Add(kToolArray_FinnishName);
	outArray.Add(kToolArray_PortugueseName);
	outArray.Add(kToolArray_SimplifiedChineseName);
	outArray.Add(kToolArray_TraditionalChineseName);
	outArray.Add(kToolArray_KoreanName);
	outArray.Add(kToolArray_PolishName);
	outArray.Add(kToolArray_PortuguesePortugalName);
	outArray.Add(kToolArray_RussianName);
	*/
}

