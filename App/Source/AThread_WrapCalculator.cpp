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

AThread_WrapCalculator
#699

*/

#include "stdafx.h"

#include "AThread_WrapCalculator.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AThread_WrapCalculator
#pragma mark ===========================
//マルチファイル検索実行

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AThread_WrapCalculator::AThread_WrapCalculator( AObjectArrayItem* inParent ) : AThread(inParent), mTextInfo(this),
		mTextDocumentID(kObjectID_Invalid), 
		mFontSize(9.0), mAntiAliasing(true), mTabWidth(4), mIndentWidth(4), mWrapMode(kWrapMode_NoWrap),
		mWrapLetterCount(80), mViewWidth(800), mCountAs2Letters(false)
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/*
スレッド動作
1. 行折り返し計算必要なときにスレッド起動(SPNVI_Run)
2. 一定量(kCalculateLimitLineCount)完了したら、メインスレッドへイベント通知してwait
3. メインスレッドは終了分の反映をしたら、スレッドのwait解除
4. 2,3を、全てのテキストの計算が終了するまで継続

mTextInfo等の各データは、
mWaitStateがtrueのとき（スレッドがwait状態）は、メインスレッドから設定・参照、
mWaitStateがfalseのとき（スレッドのwait状態解除）は、このスレッドから設定・参照する。
*/

/**
スレッドメインルーチン
*/
void	AThread_WrapCalculator::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_WrapCalculator::NVIDO_ThreadMain started.",this);
	try
	{
		while( NVM_IsAborted() == false )//NVI_AbortThread()がコールされていたらスレッド終了
		{
			//pause解除まで待つ
			NVM_WaitForUnpaused();
			//Abortされていたら終了（折り返し計算完了時、メインスレッドからNVI_AbortThread()+Unpause()により終了される。）
			if( NVM_IsAborted() == true )
			{
				break;
			}
			//行折り返し計算実行
			//（全て終了、または、kCalculatorThreadPauseLineCount行終了したら、関数から戻ってくる。
			//第二引数がfalseなのでEOF空行は追加されない。）
			AItemCount	completedTextLen = mTextInfo.CalcLineInfoAll(mText,false,kWrapCalculatorThreadPauseLineCount,
						mFontName,mFontSize,mAntiAliasing,mTabWidth,mIndentWidth,mWrapMode,mWrapLetterCount,
						mViewWidth,mCountAs2Letters);
			//Wrap計算終了分のテキストは削除。次回mTextの最初から計算する。
			mText.Delete(0,completedTextLen);
			//スレッドをpause状態にする
			NVI_Pause();
			//メインスレッドへ今回分終了を通知
			//ABase::PostToMainInternalEventQueue()直後にメインスレッド側処理が全て実行される可能性もある。
			//この場合、ContinueWrapCalculator()のコールまでが全て実行されるので、NVI_Pause()はこの前で実行することで
			//実行順がNVI_Unpause()(=ContinueWrapCalculator())→NVI_Pause()となってしまわないようにするなどの考慮をしている。
			//逆に、メインスレッド側処理のほうがゆっくりの場合もあるが、この場合は、NVM_WaitForUnpaused()で待っている間に、
			//NVI_Unpause()(=ContinueWrapCalculator())がコールされる。
			AObjectIDArray	objectIDArray;
			objectIDArray.Add(mTextDocumentID);
			ABase::PostToMainInternalEventQueue(kInternalEvent_WrapCalculatorPaused,GetObjectID(),0,GetEmptyText(),objectIDArray);
			//スレッド終了は、メインスレッドで判断する。
			//折り返し計算完了時（CopyFromCalculatorResult()により終了と判断されたとき）は、
			//NVI_AbortThread()+Unpause()によりスレッド終了する。
		}
	}
	catch(...)
	{
		_ACError("AThread_WrapCalculator::NVIDO_ThreadMain() caught exception.",NULL);
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_WrapCalculator::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---スレッド設定・起動

/**
データ初期化
*/
void	AThread_WrapCalculator::SPI_Init( const ADocumentID inTextDocumentID )
{
	//ドキュメントIDを設定
	mTextDocumentID = inTextDocumentID;
}

/**
mText設定
StartWrapCalculator()からコールされる。（行送り計算開始時に一回設定される。）
*/
void	AThread_WrapCalculator::SPI_SetText( const AText& inText, const AIndex inIndex, const AItemCount inCount )
{
	//mText設定
	inText.GetText(inIndex,inCount,mText);
}

/**
Wrapパラメータ設定
*/
void	AThread_WrapCalculator::SPI_SetWrapParameters( 
		const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
		const AItemCount inTabWidth, const AItemCount inIndentWidth, 
		const AWrapMode inWrapMode, const AItemCount inWrapLetterCount, 
		const ANumber inViewWidth, const ABool inCountAs2Letters )
{
	mFontName = inFontName;
	mFontSize = inFontSize;
	mAntiAliasing = inAntiAliasing;
	mTabWidth = inTabWidth;
	mIndentWidth = inIndentWidth;
	mWrapMode = inWrapMode;
	mWrapLetterCount = inWrapLetterCount;
	mViewWidth = inViewWidth;
	mCountAs2Letters = inCountAs2Letters;
}

#pragma mark ===========================

#pragma mark ---スレッド終了後処理

/**
スレッド内データ削除
*/
void	AThread_WrapCalculator::ClearData()
{
	//行折り返し計算完了時にデータを削除（メモリ節約）
	//ペースト時等に再度計算実行するのでSPI_Init()で設定したデータは削除しない
	mText.DeleteAll();
	mTextInfo.DeleteLineInfoAll();
}

