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

AThread_JavaScriptExecuter
#904

*/

#include "stdafx.h"

#include "AThread_JavaScriptExecuter.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AThread_JavaScriptExecuter
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AThread_JavaScriptExecuter::AThread_JavaScriptExecuter( AObjectArrayItem* inParent ) : AThread(inParent), mIsCompleted(false)
{
}

/**
デストラクタ
*/
AThread_JavaScriptExecuter::~AThread_JavaScriptExecuter()
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/**
スレッドメインルーチン
*/
void	AThread_JavaScriptExecuter::NVIDO_ThreadMain()
{
	//JavaScript実行
	GetApp().NVI_DoJavaScript(mScript,mFolder);
	//完了フラグをON
	mIsCompleted = true;
	//メインスレッドを起こす→直後に完了フラグ判定が実行される
	GetApp().NVI_WakeTrigger();
}

#pragma mark ===========================

#pragma mark ---

/**
JavaScript実行
*/
ABool	AThread_JavaScriptExecuter::SPNVI_Run_ExecuteJavaScript( const AText& inScriptText, const AFileAcc& inFolder )
{
	//スレッド実行中なら何もしない
	if( NVI_IsThreadWorking() == true )
	{
		return false;
	}
	
	//要求JavaScript設定
	mScript.SetText(inScriptText);
	mFolder = inFolder;
	
	//完了フラグOFF
	mIsCompleted = false;
	
	//スレッド実行
	NVI_Run();
	return true;
}

