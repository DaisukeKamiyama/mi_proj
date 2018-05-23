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

AWindow_TraceLog

*/

#include "stdafx.h"

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AWindow_TraceLog.h"
#include "AAppPrefDB.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_TraceLog
#pragma mark ===========================
//トレースログウインドウ

#pragma mark --- コンストラクタ／デストラクタ
//コンストラクタ
AWindow_TraceLog::AWindow_TraceLog():AWindow(/*#175 NULL*/)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kTraceLogWindowPosition);
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_TraceLog::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/TraceLog");
	//#688 NVI_RegisterTextEditPane(1,true,true,false,false);
	NVI_CreateEditBoxView(1,true,true,false);//#688
	AText	fontname;//win
	AFontWrapper::GetAppDefaultFontName(fontname);//win #375
	NVI_SetControlTextFont(1,/*win AFontWrapper::GetLogFont()*/fontname,9);
}

#pragma mark ===========================

#pragma mark ---ログ追加

//ログ追加
void	AWindow_TraceLog::SPI_AddLog( const AText& inText )
{
	AText	text;
	text.SetText(inText);
#if IMPLEMENTATION_FOR_WINDOWS
	text.ReplaceCstring("\r","\r\n");
#endif
	mText.AddText(text);//#695 内部データに格納
	if( NVI_IsWindowVisible() == true )//#695 高速化のためウインドウ表示中の場合のみコントロール内にテキスト追加
	{
		NVI_AddControlText(1,text);
	}
}

//#695
/**
ウインドウ表示時処理
*/
void	AWindow_TraceLog::NVIDO_Show()
{
	//内部データをコントロールに設定
	NVI_SetControlText(1,mText);
}

