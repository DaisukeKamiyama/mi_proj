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

AThreadWrapper

*/

#pragma once

#include "../AbsBase/ABaseTypes.h"
#include "../AbsBase/ABaseFunction.h"

#if IMPLEMENTATION_FOR_MACOSX
#define AThreadMutexImp pthread_mutex_t
#define AThreadConditionImp	pthread_cond_t
typedef pthread_t AThreadID;
typedef pthread_t AThreadRef;
#include <pthread.h>
#include <sys/time.h>
#endif

#if IMPLEMENTATION_FOR_WINDOWS
#include <process.h>
#define AThreadMutexImp CRITICAL_SECTION
#define AThreadConditionImp	HANDLE
typedef unsigned int AThreadID;
typedef HANDLE AThreadRef;
#endif

/**
スレッドラッパークラス
*/
class AThreadWrapper
{
	//スレッド
  public:
	//win static AThreadRef	CreateThread( void *(*inStartFunc)(void*), void* inArg );
	static AThreadID	GetCurrentThreadID();
	static void			WaitThreadEnd( const AThreadRef inThreadRef );
	//Mutex
  public:
	static void			InitMutex( AThreadMutexImp& inMutex );
	static void			LockMutex( AThreadMutexImp& inMutex );
	static ABool		TryLockMutex( AThreadMutexImp& inMutex );
	static void			UnlockMutex( AThreadMutexImp& inMutex );
	static void			DeleteMutex( AThreadMutexImp& inMutex );
	//条件変数
  public:
	static void			InitCondition( AThreadConditionImp& inCond );
	static void			WaitForCondition( AThreadConditionImp& inCond, AThreadMutexImp& inMutex );
	static ABool		WaitForConditionWithTimer( AThreadConditionImp& inCond, AThreadMutexImp& inMutex, const ANumber inTimerSecond );//B0314
	static void			SignalWithCondition( AThreadConditionImp& inCond );
	static void			DeleteCondition( AThreadConditionImp& inCond );

};


#pragma mark ---スレッド

#if IMPLEMENTATION_FOR_MACOSX

/*win inline AThreadRef	AThreadWrapper::CreateThread( void *(*inStartFunc)(void*), void* inArg )
{
	AThreadRef	threadID;
	pthread_create(&threadID,NULL,inStartFunc,inArg);
	return threadID;
}*/

/**
現在のスレッド番号取得
*/
inline AThreadID	AThreadWrapper::GetCurrentThreadID()
{
	return pthread_self();
}

/**
スレッド終了を待つ
*/
inline void	AThreadWrapper::WaitThreadEnd( const AThreadRef inThreadRef )
{
	int	err = pthread_join(inThreadRef,NULL);
	if( err != 0 )
	{
		_ACError("AThreadWrapper::WaitThreadEnd() error",NULL);
	}
}

#pragma mark ---Mutex

/**
Mutex初期化
*/
inline void	AThreadWrapper::InitMutex( AThreadMutexImp& inMutex )
{
	pthread_mutex_init(&inMutex,NULL);
}

/**
Mutexをロック
*/
inline void	AThreadWrapper::LockMutex( AThreadMutexImp& inMutex )
{
	pthread_mutex_lock(&inMutex);
}

/**
Mutexをロックできるかどうかを取得する
*/
inline ABool	AThreadWrapper::TryLockMutex( AThreadMutexImp& inMutex )
{
	return (pthread_mutex_trylock(&inMutex)==0);
}

/**
Mutexをアンロック
*/
inline void	AThreadWrapper::UnlockMutex( AThreadMutexImp& inMutex )
{
	pthread_mutex_unlock(&inMutex);
}

/**
Mutexを削除
*/
inline void	AThreadWrapper::DeleteMutex( AThreadMutexImp& inMutex )
{
	//処理無し
}

#pragma mark ---条件変数

/**
条件変数初期化
*/
inline void	AThreadWrapper::InitCondition( AThreadConditionImp& inCond )
{
	pthread_cond_init(&inCond,NULL);
}

/**
条件変数待ち
*/
inline void	AThreadWrapper::WaitForCondition( AThreadConditionImp& inCond, AThreadMutexImp& inMutex )
{
	pthread_cond_wait(&inCond,&inMutex);
}

/**
条件変数待ち（タイマー付き）
*/
inline ABool	AThreadWrapper::WaitForConditionWithTimer( AThreadConditionImp& inCond, AThreadMutexImp& inMutex, const ANumber inTimerSecond )
{
	struct	timeval	tv;
	struct	timespec	ts;
	if( gettimeofday(&tv,NULL) < 0 )   {_ACError("",NULL);return false;}
	ts.tv_sec = tv.tv_sec + inTimerSecond;
	ts.tv_nsec = tv.tv_usec * 1000;
	int err = pthread_cond_timedwait(&inCond,&inMutex,&ts);//inMutexをアンロックしてブロック→戻るときに再度ロック
	if( err != 0 )   return false;//B0314
	return true;//B0314
}

/**
条件変数変更
*/
inline void	AThreadWrapper::SignalWithCondition( AThreadConditionImp& inCond )
{
	pthread_cond_signal(&inCond);
}

/**
条件変数削除
*/
inline void	AThreadWrapper::DeleteCondition( AThreadConditionImp& inCond )
{
	//処理無し
}

#endif

#if IMPLEMENTATION_FOR_WINDOWS

inline AThreadID	AThreadWrapper::GetCurrentThreadID()
{
	return ::GetCurrentThreadId();
}

inline void	AThreadWrapper::WaitThreadEnd( const AThreadRef inThreadRef )
{
	::WaitForSingleObject(inThreadRef,INFINITE);
}

#pragma mark ---Mutex

inline void	AThreadWrapper::InitMutex( AThreadMutexImp& inMutex )
{
	::InitializeCriticalSection(&inMutex);
}

inline void	AThreadWrapper::LockMutex( AThreadMutexImp& inMutex )
{
	::EnterCriticalSection(&inMutex);
}

inline ABool	AThreadWrapper::TryLockMutex( AThreadMutexImp& inMutex )
{
	return (::TryEnterCriticalSection(&inMutex)==TRUE);
}

inline void	AThreadWrapper::UnlockMutex( AThreadMutexImp& inMutex )
{
	::LeaveCriticalSection(&inMutex);
}

inline void	AThreadWrapper::DeleteMutex( AThreadMutexImp& inMutex )
{
	::DeleteCriticalSection(&inMutex);
}

#pragma mark ---条件変数

inline void	AThreadWrapper::InitCondition( AThreadConditionImp& inCond )
{
	//lpEventAttributes: NULL
	//bManualReset: FALSE (自動リセット)
	//bInitialState: FALSE (デフォルト=非シグナル状態)
	//lpName: NULL
	inCond = ::CreateEvent(NULL,FALSE,FALSE,NULL);
}

inline void	AThreadWrapper::WaitForCondition( AThreadConditionImp& inCond, AThreadMutexImp& inMutex )
{
	//pthread_cond_signalとインターフェイスを合わせるため、mutexを使っている。
	//pthread_cond_signalにてmutexが必要な理由は
	//「UNIXネットワークプログラミング」の23.8節参照。
	//実際はWin32APIでは必要ないと思われる。
	//（SetEvent()は、WaitForSingleObject()しているスレッドがなくてもEventをシグナル状態にするため、
	//値チェックとシグナル待ちの間で、値設定＆シグナル送信が発生しても、
	//WaitForSingleObject()が永久にwaitすることはない）
	::LeaveCriticalSection(&inMutex);
	::WaitForSingleObject(inCond,INFINITE);
	::EnterCriticalSection(&inMutex);
}

inline ABool	AThreadWrapper::WaitForConditionWithTimer( AThreadConditionImp& inCond, AThreadMutexImp& inMutex, const ANumber inTimerSecond )
{
	//pthread_cond_signalとインターフェイスを合わせるため、mutexを使っている。
	::LeaveCriticalSection(&inMutex);
	DWORD	res =  ::WaitForSingleObject(inCond,inTimerSecond*1000);
	::EnterCriticalSection(&inMutex);
	if( res != WAIT_OBJECT_0 )   return true;
	else return false;
}

inline void	AThreadWrapper::SignalWithCondition( AThreadConditionImp& inCond )
{
	::SetEvent(inCond);
	//★自動リセットの意味確認必要。
	//WaitForSingleObject()から抜けたらシグナル状態解除の理解で合ってる？
}

inline void	AThreadWrapper::DeleteCondition( AThreadConditionImp& inCond )
{
	::CloseHandle(inCond);
}

#endif
