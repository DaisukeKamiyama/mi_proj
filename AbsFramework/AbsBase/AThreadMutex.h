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

AThreadMutex

*/

#pragma once

#include "../AbsBase_Imp/AThreadWrapper.h"

class AThreadCondition;

#pragma mark ===========================
#pragma mark [クラス]AThreadMutex
/**
スレッドMutex
*/
class AThreadMutex
{
	friend class AThreadCondition;
	//コンストラクタ
  public:
	AThreadMutex()
	{
		AThreadWrapper::InitMutex(mMutex);
	}
	//win
	~AThreadMutex()
	{
		AThreadWrapper::DeleteMutex(mMutex);
	}
	
	//Lock/Unlock
  public:
	void	Lock()
	{
		AThreadWrapper::LockMutex(mMutex);
	}
	void	Unlock()
	{
		AThreadWrapper::UnlockMutex(mMutex);
	}
	
	//TryLock（Lock出来なければwaitせず、falseを返す）#723
	ABool	TryLock()
	{
		return AThreadWrapper::TryLockMutex(mMutex);
	}
	
  private:
	AThreadMutexImp	mMutex;
};

#pragma mark ===========================
#pragma mark [クラス]AStMutexLocker
/**
スレッドMutexスタックロッククラス
*/
class AStMutexLocker
{
  public:
	AStMutexLocker( AThreadMutex& inMutex, const ABool inLock = true ) : mMutex(inMutex), mIsLocked(false)//#598
	{
		//#598 引数inLockによりオブジェクト生成してもLockしない選択ができるように変更。
		if( inLock == true )
		{
			Lock();
		}
	}
	~AStMutexLocker()
	{
		if( mIsLocked == true )//#598
		{
			mMutex.Unlock();
		}
	}
	//#598
	void	Lock()
	{
		if( mIsLocked == false )
		{
			mMutex.Lock();
			mIsLocked = true;
		}
	}
	void	Unlock()
	{
		if( mIsLocked == true )
		{
			mMutex.Unlock();
			mIsLocked = false;
		}
	}
	//
  private:
	AThreadMutex&	mMutex;
	ABool	mIsLocked;//#598
};

#pragma mark ===========================
#pragma mark [クラス]AThreadCondition
/**
スレッド条件変数
*/
class AThreadCondition
{
	//コンストラクタ
  public:
	AThreadCondition()
	{
		AThreadWrapper::InitCondition(mCondition);
	}
	//win
	~AThreadCondition()
	{
		AThreadWrapper::DeleteCondition(mCondition);
	}
	
	//wait/signal
  public:
	void	WaitForCondition( AThreadMutex& inMutex )
	{
		AThreadWrapper::WaitForCondition(mCondition,inMutex.mMutex);
	}
	ABool	WaitForConditionWithTimer( AThreadMutex& inMutex, const float inTimerSecond )//#1483 ANumber→float
	{
		return AThreadWrapper::WaitForConditionWithTimer(mCondition,inMutex.mMutex,inTimerSecond);
	}
	void	SignalWithCondition()
	{
		AThreadWrapper::SignalWithCondition(mCondition);
	}
	
  private:
	AThreadConditionImp	mCondition;
};

