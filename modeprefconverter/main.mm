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

mi_modeprefconverter
main.cpp

*/

#include "../../AbsFramework/Frame.h"
#import <Cocoa/Cocoa.h>
#include "../../AbsFramework/AbsBase/Cocoa.h"
#include "AModePrefDB.h"

int	main( int argc, const char **argv )
{
	//SIGPIPEを無視
	::signal(SIGPIPE, SIG_IGN);
	
	//ABaseオブジェクトを生成する。
	ABase	base(true);
	
	//引数（モードフォルダ）取得
	AText	argv1;
	argv1.AddCstring(argv[1]);
	
	//モードフォルダをfileに格納
	AFileAcc	file;
	file.Specify(argv1);
	
	//AModePrefDBオブジェクト作成
	AModePrefDB	modePrefDB(NULL);
	
	//MacLegacyデータをロードしてバージョン3形式ファイルで保存
	modePrefDB.LoadMacLegacyAndSave(0,file);
}

//アプリケーション名
void	GetApplicationName( AText& outName )
{
	outName.SetCstring("mi_modeprefconverter");
}

void	BackupDataForCriticalError()
{
}

void	EventTransactionPostProcess( const ABool inPeriodical )
{
}

