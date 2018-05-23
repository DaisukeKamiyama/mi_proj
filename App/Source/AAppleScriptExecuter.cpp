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

AAppleScriptExecuter

*/

#include "stdafx.h"

#include "AAppleScriptExecuter.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AAppleScriptExecuter
#pragma mark ===========================
//

/*

当初、スタックフレーム内にAAppオブジェクトを生成していたことにより、スタックオーバー？で、OSADoScriptFileが-2706エラーでどうしても
実行できないため、スレッド実行を試したが、OSADoScriptFileは動作するものの、スレッド内でAppleEvent受信してしまい、うまくいかなかった。
AAppをヒープ生成にしたら、メインスレッドでOSADoScriptFileが動作するようになったので、スレッド起動はやめて直接実行にした。
将来的にスレッド実行を試みるかもしれないので枠組みは残す。
*/

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AAppleScriptExecuter::AAppleScriptExecuter( AObjectArrayItem* inParent ) : AThread(inParent), mExecuteMode(false), mGetSourceMode(false)
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

void	AAppleScriptExecuter::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AAppleScriptExecuter::NVIDO_ThreadMain started.",this);
	try
	{
		OSErr	err = noErr;
		if( mExecuteMode == true )
		{
			FSRef	fsref;
			mFile.GetFSRef(fsref);
			AEDesc	result;
			err = ::OSADoScriptFile(CAppImp::GetScriptingComponent(),&fsref,kOSANullScript,typeChar,kOSAModeNull,&result);
		}
		if( mGetSourceMode == true )
		{
			GetSource();
		}
	}
	catch(...)
	{
		_ACError("AAppleScriptExecuter::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	mExecuteMode = false;
	mGetSourceMode = false;
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AAppleScriptExecuter::NVIDO_ThreadMain ended.",this);
}

void	AAppleScriptExecuter::SPI_GetSource( const AFileAcc& inFile, AText& outText )
{
	mFile.CopyFrom(inFile);
	mGetSourceMode = true;
	//NVI_Run();
	//NVI_WaitThreadEnd();
	NVIDO_ThreadMain();
	outText.SetText(mSourceText);
}

void	AAppleScriptExecuter::GetSource()
{
	mSourceText.DeleteAll();
	
	AText	data;
	// scpt128
	/*B0000 リソース無しThrow発生回避 if( mFile.ReadResouceTo(data,'scpt',128) == false )
	{
		//data fork
		mFile.ReadTo(data);
	}*/
	if( mFile.ExistResource('scpt',128) == true )
	{
		// scpt128
		mFile.ReadResouceTo(data,'scpt',128);
	}
	if( data.GetItemCount() == 0 )
	{
		//data fork
		mFile.ReadTo(data);
	}
	
	if( data.GetItemCount() == 0 )   return;
	
	
	AStTextPtr	ptr(data,0,data.GetItemCount());
	AEDesc	script;
	::AECreateDesc(typeNull,NULL,0,&script);
	OSErr	err;
	{
		err = ::AECreateDesc(typeOSAGenericStorage,(Ptr)(ptr.GetPtr()),ptr.GetByteCount(),&script);
	}
	OSAID	scriptid;
	if( err == noErr ) 
	{
		err = ::OSALoad(CAppImp::GetScriptingComponent(),&script,kOSAModeNull,&scriptid);
	}
	AEDesc	compiledText;
	::AECreateDesc(typeNull,NULL,0,&compiledText);
	if( err == noErr ) 
	{
		err = ::OSAGetSource(CAppImp::GetScriptingComponent(),scriptid,typeText,&compiledText);
	}
	
	//
	CAEDescReader	desc(&compiledText);
	desc.GetText(mSourceText);
	
	::AEDisposeDesc(&compiledText);
	::AEDisposeDesc(&script);
}

void	AAppleScriptExecuter::SPI_Execute( const AFileAcc& inFile )
{
	//if( NVI_IsThreadWorking() == true )   NVI_WaitThreadEnd();
	mFile.CopyFrom(inFile);
	mExecuteMode = true;
	//NVI_Run();
	NVIDO_ThreadMain();
	//NVI_WaitThreadEnd();
}



