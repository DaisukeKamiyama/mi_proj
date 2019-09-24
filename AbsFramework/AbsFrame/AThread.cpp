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

AThread

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AThread
#pragma mark ===========================
//

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AThread::AThread( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent), mThreadRef(NULL), mAborted(false), mWakeFlag(false), mSleepFlag(false)
,mIsThreadWorking(false)//#525
,mPauseState(false)//#699
{
}
//デストラクタ
AThread::~AThread()
{
}

#pragma mark ===========================

#pragma mark ---スレッド実行管理

#if IMPLEMENTATION_FOR_MACOSX

//スレッド起動
void	AThread::NVI_Run( const ABool inPauseState )
{
	//#699 スレッド動作中のスレッド起動時はエラー（ただしスレッド起動はする）
	if( mIsThreadWorking == true )
	{
		_ACError("",NULL);
	}
	//
	mAborted = false;
	mPauseState = inPauseState;//#699
	//win mThreadRef = AThreadWrapper::CreateThread(threadmain,this);
	mIsThreadWorking = true;//#525 mThreadRefに値を格納する前にスレッド終了することがあるので、フラグを先に立てる
	pthread_create(&mThreadRef,NULL,threadmain,this);
}

extern void*	AllocAutoreleasePool();
extern void	ReleaseAutoreleasePool( void* inPool );

//スレッドメインルーチン（static）
void*	AThread::threadmain( void* vptr )
{
	AThread*	obj = (AThread*)vptr;
#if IMPLEMENTATION_FOR_MACOSX
	//Objective-C用tryブロック（NVIDO_ThreadMain()内で@throwされたときのアプリ強制終了防止
	@try 
	{
#endif
		try//NVIDO_ThreadMain()内でthrowされたときに強制終了する可能性がある
		{
			//#688 スレッド用autorelease pool生成
			void*	pool = AllocAutoreleasePool();
			//非同期キャンセル可能にする
			//★動作していない？
			int	oldtype = 0;
			int err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&oldtype);
			int	oldstate = 0;
			err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&oldstate);
			//スレッドメイン処理実行
			obj->NVIDO_ThreadMain();
			//#688 スレッド用autorelease pool解放
			ReleaseAutoreleasePool(pool);
		}
		catch(...)
		{
		}
#if IMPLEMENTATION_FOR_MACOSX
	}
	@catch(NSException *theException)
	{
		_ACError("AThread::threadmain() @catch",NULL); 
	}
#endif
	obj->mIsThreadWorking = false;//#525
	obj->mThreadRef = NULL;
	return NULL;
}

#endif

#if IMPLEMENTATION_FOR_WINDOWS

//スレッド起動
void	AThread::NVI_Run( const ABool inPauseState )
{
	//#699 スレッド動作中のスレッド起動時はエラー（ただしスレッド起動はする）
	if( mIsThreadWorking == true )
	{
		_ACError("",NULL);
	}
	//
	mAborted = false;
	mPauseState = inPauseState;//#699
	//
	AThreadID	threadId = 0;
	mIsThreadWorking = true;//#525 mThreadRefに値を格納する前にスレッド終了することがあるので、フラグを先に立てる
	mThreadRef = (HANDLE)_beginthreadex(NULL,0,threadmain,this,0,&threadId);
}

//スレッドメインルーチン（static）
unsigned int __stdcall	AThread::threadmain( void* vptr )
{
	AThread*	obj = (AThread*)vptr;
	try
	{
		obj->NVIDO_ThreadMain();
	}
	catch(...)
	{
	}
	obj->mIsThreadWorking = false;//#525
	obj->mThreadRef = NULL;
	return 0;
}

#endif


//#904
/**
スレッド非同期キャンセル(kill)
★現状、キャンセルポイントが明示的にないとcancelできない。
*/
void	AThread::NVI_Kill()
{
	AThreadRef	threadRef = mThreadRef;
	pthread_cancel(threadRef);
	//終了待ちしない（現状、キャンセルポイントが明示的にないとcancelできないので、完了しない可能性がある。）AThreadWrapper::WaitThreadEnd(threadRef);
	//
	mIsThreadWorking = false;
	mThreadRef = NULL;
}

//スレッド外からスレッドを強制中断させる
void	AThread::NVI_AbortThread()
{
	mAborted = true;
	NVIDO_AbortThread();
	NVI_WakeTrigger();
}

//スレッド外からスレッドを強制中断させ、終了待ち
void	AThread::NVI_WaitAbortThread()
{
	if( NVI_IsThreadWorking() == false )   return;
	
	AThreadRef	threadRef = mThreadRef;
	NVI_SetSleepFlag(false);
	NVI_WakeTrigger();
	NVI_AbortThread();
	//Pause中ならpause解除（pause解除しないとabort判定箇所へ行かない可能性がある）#717
	NVI_UnpauseIfPaused();
	//スレッド終了待ち
	AThreadWrapper::WaitThreadEnd(threadRef);
}

void	AThread::NVI_WaitThreadEnd()
{
	if( NVI_IsThreadWorking() == false )   return;
	
	AThreadRef	threadRef = mThreadRef;
	AThreadWrapper::WaitThreadEnd(threadRef);
}

//スレッドが強制中断されたかどうかを返す
ABool	AThread::NVM_IsAborted()
{
	return mAborted;
}

//スレッド実行中かどうかを返す
ABool	AThread::NVI_IsThreadWorking() const
{
	//#525 mThreadRefへの値格納前にスレッド終了する可能性があるため return (mThreadRef!=NULL);
	return mIsThreadWorking;//#525
}

//NVI_WakeTrigger()で起こされるまでスリープする
//（mWakeFlagがtrueになるまでブロック状態にする）
void	AThread::NVM_Sleep()
{
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	mWakeFlag = false;
	while( mWakeFlag == false )
	{
		//mWakeFlagConditionにシグナルが送られるまでブロック、かつ、ブロック中はmutexはアンロックされる
		mWakeFlagCondition.WaitForCondition(mWakeFlagMutex);
	}
	mWakeFlag = false;
}

//NVI_WakeTrigger()で起こされるまでスリープする
//（mWakeFlagがtrueになるまでブロック状態にする）
void	AThread::NVM_SleepWithTimer( const float inTimerSecond )//#1483 ANumber→float
{
	if( NVI_IsThreadWorking() == false )   return;
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	mWakeFlag = false;
	{
		//mWakeFlagConditionにシグナルが送られるまでブロック、かつ、ブロック中はmutexはアンロックされる
		//ただし、タイマー満了したらそこでブロック解除
		mWakeFlagCondition.WaitForConditionWithTimer(mWakeFlagMutex,inTimerSecond);
	}
	mWakeFlag = false;
}

//スリープ状態のスレッドを起こす
void	AThread::NVI_WakeTrigger()
{
	if( mSleepFlag == true )   return;
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	if( mWakeFlag == false )
	{
		mWakeFlag = true;
		mWakeFlagCondition.SignalWithCondition();
	}
}

//#699
/**
スレッド実行をpause状態にする
フラグ設定のみを行う。実際に一時停止するにはスレッド側でNVM_WaitForUnpaused()を実行する必要がある。
*/
void	AThread::NVI_Pause()
{
	AStMutexLocker	mutexlocker(mPauseStateMutex);
	//pause中のpauseはエラー
	if( mPauseState == true )
	{
		_ACError("NVI_Pause() is called while unpaused.",NULL);
	}
	//pause状態に設定
	mPauseState = true;
}

//#699
/**
スレッド実行をunpause状態にする
フラグを設定し、スレッドへシグナル送信する。それによりNVM_WaitForUnpaused()での待ちが解除される。
*/
void	AThread::NVI_Unpause()
{
	AStMutexLocker	mutexlocker(mPauseStateMutex);
	//pause中でないときのunpauseはエラー
	if( mPauseState == false )
	{
		_ACError("NVI_Unpause() is called while paused.",NULL);
	}
	//unpause状態に設定
	mPauseState = false;
	//シグナル送信
	mPauseStateCondition.SignalWithCondition();
}

//#717
/**
pause状態ならpause解除する
*/
void	AThread::NVI_UnpauseIfPaused()
{
	AStMutexLocker	mutexlocker(mPauseStateMutex);
	//pause中でないときは何もしない
	if( mPauseState == false )
	{
		return;
	}
	//unpause状態に設定
	mPauseState = false;
	//シグナル送信
	mPauseStateCondition.SignalWithCondition();
}

//#699
/**
スレッド側でunpauseされるのを待つ
*/
void	AThread::NVM_WaitForUnpaused()
{
	while( NVM_IsAborted() == false )//NVI_AbortThread()されていたらリターン
	{
		AStMutexLocker	mutexlocker(mPauseStateMutex);
		//wait状態解除されたらリターン
		if( mPauseState == false )
		{
			return;
		}
		//条件変数mPauseStateConditionからシグナル送信されるまでwaitする
		//シグナル送信側(NVI_Unpause())・受信側（この関数）でmPauseStateMutexをロックしているので、
		//上記のmPauseState判定とwaitするまでの間（つまりこのコメント位置のタイミング）に値変更・シグナル送信処理が動作することはなく、
		//値変更(NVI_Unpause())済みなのにWaitForConditionWithTimer()がタイマー満了まで抜けてこないということは発生しない。
		//※condition wait中はmPauseStateMutexはアンロックされている
		//Abort判定したいので（＋永久待ちを念のため防ぐため）、タイマー付きを使っている。
		mPauseStateCondition.WaitForConditionWithTimer(mPauseStateMutex,3);
	}
}



