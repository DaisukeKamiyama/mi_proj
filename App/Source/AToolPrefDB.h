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

#pragma once

#include "../../AbsFramework/AbsBase/ABase.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AToolPrefDB
//モード設定データ
class AToolPrefDB : public ADataBase
{
	//コンストラクタ／デストラクタ
  public:
	AToolPrefDB();
	~AToolPrefDB();
	
	//#427
  public:
	static void	GetLanguagePrefIDArray( AArray<APrefID>& outArray );
	
  public:
	const static APrefID	kToolArray_FileName						= 100;
	const static APrefID	kToolArray_Enabled						= 101;
	const static APrefID	kToolArray_Shortcut						= 102;
	const static APrefID	kToolArray_ShortcutModifiers_ControlKey	= 103;
	const static APrefID	kToolArray_ShortcutModifiers_ShiftKey	= 104;
	const static APrefID	kToolArray_Grayout						= 105;
	const static APrefID	kToolArray_EnabledForMacroBar			= 106;
	const static APrefID	kVersionText							= 107;
	
	const static APrefID	kToolArray_JapaneseName					= 500;
	const static APrefID	kToolArray_EnglishName					= 501;
	const static APrefID	kToolArray_FrenchName					= 502;
	const static APrefID	kToolArray_GermanName					= 503;
	const static APrefID	kToolArray_SpanishName					= 504;
	const static APrefID	kToolArray_ItalianName					= 505;
	const static APrefID	kToolArray_DutchName					= 506;
	const static APrefID	kToolArray_SwedishName					= 507;
	const static APrefID	kToolArray_NorwegianName				= 508;
	const static APrefID	kToolArray_DanishName					= 509;
	const static APrefID	kToolArray_FinnishName					= 510;
	const static APrefID	kToolArray_PortugueseName				= 511;
	const static APrefID	kToolArray_SimplifiedChineseName		= 512;
	const static APrefID	kToolArray_TraditionalChineseName		= 513;
	const static APrefID	kToolArray_KoreanName					= 514;
	const static APrefID	kToolArray_PolishName					= 515;
	const static APrefID	kToolArray_PortuguesePortugalName		= 516;
	const static APrefID	kToolArray_RussianName					= 517;
};


