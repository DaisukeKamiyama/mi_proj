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

ATest

*/

//抽象クラスのテスト用

#include "stdafx.h"

#include "ATest.h"
//#include "AModePrefDB.h"
#include "AApp.h"

ATest::ATest()
{
}

void ATest::RunTest( AText& outText )
{
	bool resultOK = true;
	
	//ATextArray
	/*{
		ATextArray	a;
		if( a.Test(outText) == false )   resultOK = false;
	}*/
	
	//ATextHashArray
	/*{
		ATextHashArray	a(128);
		if( a.Test(outText) == false )   resultOK = false;
	}*/
	
	//ASyntax
	/*★{
		ASyntax	a;
		if( a.Test(outText) == false )   resultOK = false;
	}*/
	
	//AFileAcc
	{
		AFileAcc	a;
		if( a.UnitTest(outText) == false )   resultOK = false;
	}
	
	//AModeData
	{
		AFileAcc	tmp;
		tmp.Specify("/");
		AFileAcc	testfolder;
		testfolder.SpecifyUniqChildFile(tmp,"unittest_mode");
		testfolder.CreateFolder();
		AFileAcc	testmode;
		testmode.SpecifyChild(testfolder,"testmode");
		testmode.CreateFile();
		if( testmode.Exist() == false )   resultOK = false;
		else
		{
			//★AModeID modeID = GetApp().SPI_GetModePref().AddMode(testmode);
			//★if( GetApp().SPI_GetModePrefDB(modeID).UnitTest(outText) == false )  resultOK = false;
		}
	}
	
	if( resultOK == true )
	{
		outText.AddCstring("ZTest: All tests OK!\n");
	}
	else
	{
		outText.AddCstring("ZTest: There are at least one NG!\n");
	}
}




