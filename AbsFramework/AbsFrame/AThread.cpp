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
#pragma mark [�N���X]AThread
#pragma mark ===========================
//

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AThread::AThread( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent), mThreadRef(NULL), mAborted(false), mWakeFlag(false), mSleepFlag(false)
,mIsThreadWorking(false)//#525
,mPauseState(false)//#699
{
}
//�f�X�g���N�^
AThread::~AThread()
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���s�Ǘ�

#if IMPLEMENTATION_FOR_MACOSX

//�X���b�h�N��
void	AThread::NVI_Run( const ABool inPauseState )
{
	//#699 �X���b�h���쒆�̃X���b�h�N�����̓G���[�i�������X���b�h�N���͂���j
	if( mIsThreadWorking == true )
	{
		_ACError("",NULL);
	}
	//
	mAborted = false;
	mPauseState = inPauseState;//#699
	//win mThreadRef = AThreadWrapper::CreateThread(threadmain,this);
	mIsThreadWorking = true;//#525 mThreadRef�ɒl���i�[����O�ɃX���b�h�I�����邱�Ƃ�����̂ŁA�t���O���ɗ��Ă�
	pthread_create(&mThreadRef,NULL,threadmain,this);
}

extern void*	AllocAutoreleasePool();
extern void	ReleaseAutoreleasePool( void* inPool );

//�X���b�h���C�����[�`���istatic�j
void*	AThread::threadmain( void* vptr )
{
	AThread*	obj = (AThread*)vptr;
#if IMPLEMENTATION_FOR_MACOSX
	//Objective-C�ptry�u���b�N�iNVIDO_ThreadMain()����@throw���ꂽ�Ƃ��̃A�v�������I���h�~
	@try 
	{
#endif
		try//NVIDO_ThreadMain()����throw���ꂽ�Ƃ��ɋ����I������\��������
		{
			//#688 �X���b�h�pautorelease pool����
			void*	pool = AllocAutoreleasePool();
			//�񓯊��L�����Z���\�ɂ���
			//�����삵�Ă��Ȃ��H
			int	oldtype = 0;
			int err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&oldtype);
			int	oldstate = 0;
			err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&oldstate);
			//�X���b�h���C���������s
			obj->NVIDO_ThreadMain();
			//#688 �X���b�h�pautorelease pool���
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

//�X���b�h�N��
void	AThread::NVI_Run( const ABool inPauseState )
{
	//#699 �X���b�h���쒆�̃X���b�h�N�����̓G���[�i�������X���b�h�N���͂���j
	if( mIsThreadWorking == true )
	{
		_ACError("",NULL);
	}
	//
	mAborted = false;
	mPauseState = inPauseState;//#699
	//
	AThreadID	threadId = 0;
	mIsThreadWorking = true;//#525 mThreadRef�ɒl���i�[����O�ɃX���b�h�I�����邱�Ƃ�����̂ŁA�t���O���ɗ��Ă�
	mThreadRef = (HANDLE)_beginthreadex(NULL,0,threadmain,this,0,&threadId);
}

//�X���b�h���C�����[�`���istatic�j
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
�X���b�h�񓯊��L�����Z��(kill)
������A�L�����Z���|�C���g�������I�ɂȂ���cancel�ł��Ȃ��B
*/
void	AThread::NVI_Kill()
{
	AThreadRef	threadRef = mThreadRef;
	pthread_cancel(threadRef);
	//�I���҂����Ȃ��i����A�L�����Z���|�C���g�������I�ɂȂ���cancel�ł��Ȃ��̂ŁA�������Ȃ��\��������B�jAThreadWrapper::WaitThreadEnd(threadRef);
	//
	mIsThreadWorking = false;
	mThreadRef = NULL;
}

//�X���b�h�O����X���b�h���������f������
void	AThread::NVI_AbortThread()
{
	mAborted = true;
	NVIDO_AbortThread();
	NVI_WakeTrigger();
}

//�X���b�h�O����X���b�h���������f�����A�I���҂�
void	AThread::NVI_WaitAbortThread()
{
	if( NVI_IsThreadWorking() == false )   return;
	
	AThreadRef	threadRef = mThreadRef;
	NVI_SetSleepFlag(false);
	NVI_WakeTrigger();
	NVI_AbortThread();
	//Pause���Ȃ�pause�����ipause�������Ȃ���abort����ӏ��֍s���Ȃ��\��������j#717
	NVI_UnpauseIfPaused();
	//�X���b�h�I���҂�
	AThreadWrapper::WaitThreadEnd(threadRef);
}

void	AThread::NVI_WaitThreadEnd()
{
	if( NVI_IsThreadWorking() == false )   return;
	
	AThreadRef	threadRef = mThreadRef;
	AThreadWrapper::WaitThreadEnd(threadRef);
}

//�X���b�h���������f���ꂽ���ǂ�����Ԃ�
ABool	AThread::NVM_IsAborted()
{
	return mAborted;
}

//�X���b�h���s�����ǂ�����Ԃ�
ABool	AThread::NVI_IsThreadWorking() const
{
	//#525 mThreadRef�ւ̒l�i�[�O�ɃX���b�h�I������\�������邽�� return (mThreadRef!=NULL);
	return mIsThreadWorking;//#525
}

//NVI_WakeTrigger()�ŋN�������܂ŃX���[�v����
//�imWakeFlag��true�ɂȂ�܂Ńu���b�N��Ԃɂ���j
void	AThread::NVM_Sleep()
{
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	mWakeFlag = false;
	while( mWakeFlag == false )
	{
		//mWakeFlagCondition�ɃV�O�i����������܂Ńu���b�N�A���A�u���b�N����mutex�̓A�����b�N�����
		mWakeFlagCondition.WaitForCondition(mWakeFlagMutex);
	}
	mWakeFlag = false;
}

//NVI_WakeTrigger()�ŋN�������܂ŃX���[�v����
//�imWakeFlag��true�ɂȂ�܂Ńu���b�N��Ԃɂ���j
void	AThread::NVM_SleepWithTimer( const float inTimerSecond )//#1483 ANumber��float
{
	if( NVI_IsThreadWorking() == false )   return;
	AStMutexLocker	mutexlocker(mWakeFlagMutex);
	mWakeFlag = false;
	{
		//mWakeFlagCondition�ɃV�O�i����������܂Ńu���b�N�A���A�u���b�N����mutex�̓A�����b�N�����
		//�������A�^�C�}�[���������炻���Ńu���b�N����
		mWakeFlagCondition.WaitForConditionWithTimer(mWakeFlagMutex,inTimerSecond);
	}
	mWakeFlag = false;
}

//�X���[�v��Ԃ̃X���b�h���N����
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
�X���b�h���s��pause��Ԃɂ���
�t���O�ݒ�݂̂��s���B���ۂɈꎞ��~����ɂ̓X���b�h����NVM_WaitForUnpaused()�����s����K�v������B
*/
void	AThread::NVI_Pause()
{
	AStMutexLocker	mutexlocker(mPauseStateMutex);
	//pause����pause�̓G���[
	if( mPauseState == true )
	{
		_ACError("NVI_Pause() is called while unpaused.",NULL);
	}
	//pause��Ԃɐݒ�
	mPauseState = true;
}

//#699
/**
�X���b�h���s��unpause��Ԃɂ���
�t���O��ݒ肵�A�X���b�h�փV�O�i�����M����B����ɂ��NVM_WaitForUnpaused()�ł̑҂������������B
*/
void	AThread::NVI_Unpause()
{
	AStMutexLocker	mutexlocker(mPauseStateMutex);
	//pause���łȂ��Ƃ���unpause�̓G���[
	if( mPauseState == false )
	{
		_ACError("NVI_Unpause() is called while paused.",NULL);
	}
	//unpause��Ԃɐݒ�
	mPauseState = false;
	//�V�O�i�����M
	mPauseStateCondition.SignalWithCondition();
}

//#717
/**
pause��ԂȂ�pause��������
*/
void	AThread::NVI_UnpauseIfPaused()
{
	AStMutexLocker	mutexlocker(mPauseStateMutex);
	//pause���łȂ��Ƃ��͉������Ȃ�
	if( mPauseState == false )
	{
		return;
	}
	//unpause��Ԃɐݒ�
	mPauseState = false;
	//�V�O�i�����M
	mPauseStateCondition.SignalWithCondition();
}

//#699
/**
�X���b�h����unpause�����̂�҂�
*/
void	AThread::NVM_WaitForUnpaused()
{
	while( NVM_IsAborted() == false )//NVI_AbortThread()����Ă����烊�^�[��
	{
		AStMutexLocker	mutexlocker(mPauseStateMutex);
		//wait��ԉ������ꂽ�烊�^�[��
		if( mPauseState == false )
		{
			return;
		}
		//�����ϐ�mPauseStateCondition����V�O�i�����M�����܂�wait����
		//�V�O�i�����M��(NVI_Unpause())�E��M���i���̊֐��j��mPauseStateMutex�����b�N���Ă���̂ŁA
		//��L��mPauseState�����wait����܂ł̊ԁi�܂肱�̃R�����g�ʒu�̃^�C�~���O�j�ɒl�ύX�E�V�O�i�����M���������삷�邱�Ƃ͂Ȃ��A
		//�l�ύX(NVI_Unpause())�ς݂Ȃ̂�WaitForConditionWithTimer()���^�C�}�[�����܂Ŕ����Ă��Ȃ��Ƃ������Ƃ͔������Ȃ��B
		//��condition wait����mPauseStateMutex�̓A�����b�N����Ă���
		//Abort���肵�����̂Łi�{�i�v�҂���O�̂��ߖh�����߁j�A�^�C�}�[�t�����g���Ă���B
		mPauseStateCondition.WaitForConditionWithTimer(mPauseStateMutex,3);
	}
}



