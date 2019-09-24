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

#pragma once

#include "../AbsBase/ABase.h"
#include "../AbsBase_Imp/AThreadWrapper.h"

#pragma mark ===========================
#pragma mark [クラス]AThread
/**
スレッド管理クラス
*/
class AThread : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	AThread( AObjectArrayItem* inParent );
	virtual ~AThread();
	
	//<インターフェイス>
	
	//スレッド実行管理
  public:
	ABool					NVI_IsThreadWorking() const;
	void					NVI_Run( const ABool inPauseState = false );//#699
	void					NVI_Kill();//#904
	void					NVI_AbortThread();
	void					NVI_WaitAbortThread();
	void					NVI_WaitThreadEnd();
	void					NVI_WakeTrigger();
	void					NVI_SetSleepFlag( const ABool inMakeSleep ) { mSleepFlag = inMakeSleep; }
	ABool					NVI_GetSleepFlag() const { return mSleepFlag; }
  protected:
	void					NVM_Sleep();
	void					NVM_SleepWithTimer( const float inTimerSecond );//#1483 ANumber→float
	ABool					NVM_IsAborted();
	ABool								mAborted;
  private:
	virtual void			NVIDO_AbortThread() {}
	virtual void			NVIDO_ThreadMain() = 0;
	AThreadRef							mThreadRef;
	ABool								mWakeFlag;
	AThreadCondition					mWakeFlagCondition;
	AThreadMutex						mWakeFlagMutex;
	ABool								mSleepFlag;
	ABool								mIsThreadWorking;//#525
	
	//Pause機構 #699
  public:
	void					NVI_Pause();
	void					NVI_Unpause();
	void					NVI_UnpauseIfPaused();//#717
	void					NVM_WaitForUnpaused();
  private:
	ABool								mPauseState;
	AThreadMutex						mPauseStateMutex;
	AThreadCondition					mPauseStateCondition;
	
#if IMPLEMENTATION_FOR_MACOSX
	static void*			threadmain( void* vptr );
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	static unsigned int __stdcall		threadmain( void* vptr );
#endif
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AThread"; }
	
};

typedef AAbstractFactoryForObjectArray<AThread>	AThreadFactory;


