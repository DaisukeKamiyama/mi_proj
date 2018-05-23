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
�X���b�h���b�p�[�N���X
*/
class AThreadWrapper
{
	//�X���b�h
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
	//�����ϐ�
  public:
	static void			InitCondition( AThreadConditionImp& inCond );
	static void			WaitForCondition( AThreadConditionImp& inCond, AThreadMutexImp& inMutex );
	static ABool		WaitForConditionWithTimer( AThreadConditionImp& inCond, AThreadMutexImp& inMutex, const ANumber inTimerSecond );//B0314
	static void			SignalWithCondition( AThreadConditionImp& inCond );
	static void			DeleteCondition( AThreadConditionImp& inCond );

};


#pragma mark ---�X���b�h

#if IMPLEMENTATION_FOR_MACOSX

/*win inline AThreadRef	AThreadWrapper::CreateThread( void *(*inStartFunc)(void*), void* inArg )
{
	AThreadRef	threadID;
	pthread_create(&threadID,NULL,inStartFunc,inArg);
	return threadID;
}*/

/**
���݂̃X���b�h�ԍ��擾
*/
inline AThreadID	AThreadWrapper::GetCurrentThreadID()
{
	return pthread_self();
}

/**
�X���b�h�I����҂�
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
Mutex������
*/
inline void	AThreadWrapper::InitMutex( AThreadMutexImp& inMutex )
{
	pthread_mutex_init(&inMutex,NULL);
}

/**
Mutex�����b�N
*/
inline void	AThreadWrapper::LockMutex( AThreadMutexImp& inMutex )
{
	pthread_mutex_lock(&inMutex);
}

/**
Mutex�����b�N�ł��邩�ǂ������擾����
*/
inline ABool	AThreadWrapper::TryLockMutex( AThreadMutexImp& inMutex )
{
	return (pthread_mutex_trylock(&inMutex)==0);
}

/**
Mutex���A�����b�N
*/
inline void	AThreadWrapper::UnlockMutex( AThreadMutexImp& inMutex )
{
	pthread_mutex_unlock(&inMutex);
}

/**
Mutex���폜
*/
inline void	AThreadWrapper::DeleteMutex( AThreadMutexImp& inMutex )
{
	//��������
}

#pragma mark ---�����ϐ�

/**
�����ϐ�������
*/
inline void	AThreadWrapper::InitCondition( AThreadConditionImp& inCond )
{
	pthread_cond_init(&inCond,NULL);
}

/**
�����ϐ��҂�
*/
inline void	AThreadWrapper::WaitForCondition( AThreadConditionImp& inCond, AThreadMutexImp& inMutex )
{
	pthread_cond_wait(&inCond,&inMutex);
}

/**
�����ϐ��҂��i�^�C�}�[�t���j
*/
inline ABool	AThreadWrapper::WaitForConditionWithTimer( AThreadConditionImp& inCond, AThreadMutexImp& inMutex, const ANumber inTimerSecond )
{
	struct	timeval	tv;
	struct	timespec	ts;
	if( gettimeofday(&tv,NULL) < 0 )   {_ACError("",NULL);return false;}
	ts.tv_sec = tv.tv_sec + inTimerSecond;
	ts.tv_nsec = tv.tv_usec * 1000;
	int err = pthread_cond_timedwait(&inCond,&inMutex,&ts);//inMutex���A�����b�N���ău���b�N���߂�Ƃ��ɍēx���b�N
	if( err != 0 )   return false;//B0314
	return true;//B0314
}

/**
�����ϐ��ύX
*/
inline void	AThreadWrapper::SignalWithCondition( AThreadConditionImp& inCond )
{
	pthread_cond_signal(&inCond);
}

/**
�����ϐ��폜
*/
inline void	AThreadWrapper::DeleteCondition( AThreadConditionImp& inCond )
{
	//��������
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

#pragma mark ---�����ϐ�

inline void	AThreadWrapper::InitCondition( AThreadConditionImp& inCond )
{
	//lpEventAttributes: NULL
	//bManualReset: FALSE (�������Z�b�g)
	//bInitialState: FALSE (�f�t�H���g=��V�O�i�����)
	//lpName: NULL
	inCond = ::CreateEvent(NULL,FALSE,FALSE,NULL);
}

inline void	AThreadWrapper::WaitForCondition( AThreadConditionImp& inCond, AThreadMutexImp& inMutex )
{
	//pthread_cond_signal�ƃC���^�[�t�F�C�X�����킹�邽�߁Amutex���g���Ă���B
	//pthread_cond_signal�ɂ�mutex���K�v�ȗ��R��
	//�uUNIX�l�b�g���[�N�v���O���~���O�v��23.8�ߎQ�ƁB
	//���ۂ�Win32API�ł͕K�v�Ȃ��Ǝv����B
	//�iSetEvent()�́AWaitForSingleObject()���Ă���X���b�h���Ȃ��Ă�Event���V�O�i����Ԃɂ��邽�߁A
	//�l�`�F�b�N�ƃV�O�i���҂��̊ԂŁA�l�ݒ聕�V�O�i�����M���������Ă��A
	//WaitForSingleObject()���i�v��wait���邱�Ƃ͂Ȃ��j
	::LeaveCriticalSection(&inMutex);
	::WaitForSingleObject(inCond,INFINITE);
	::EnterCriticalSection(&inMutex);
}

inline ABool	AThreadWrapper::WaitForConditionWithTimer( AThreadConditionImp& inCond, AThreadMutexImp& inMutex, const ANumber inTimerSecond )
{
	//pthread_cond_signal�ƃC���^�[�t�F�C�X�����킹�邽�߁Amutex���g���Ă���B
	::LeaveCriticalSection(&inMutex);
	DWORD	res =  ::WaitForSingleObject(inCond,inTimerSecond*1000);
	::EnterCriticalSection(&inMutex);
	if( res != WAIT_OBJECT_0 )   return true;
	else return false;
}

inline void	AThreadWrapper::SignalWithCondition( AThreadConditionImp& inCond )
{
	::SetEvent(inCond);
	//���������Z�b�g�̈Ӗ��m�F�K�v�B
	//WaitForSingleObject()���甲������V�O�i����ԉ����̗����ō����Ă�H
}

inline void	AThreadWrapper::DeleteCondition( AThreadConditionImp& inCond )
{
	::CloseHandle(inCond);
}

#endif
