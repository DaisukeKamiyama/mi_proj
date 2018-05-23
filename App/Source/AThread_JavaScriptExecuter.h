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

#pragma once

#include "../../AbsFramework/Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AThread_JavaScriptExecuter
/**
AThread_JavaScriptExecuterスレッドオブジェクト
*/
class AThread_JavaScriptExecuter : public AThread
{
	//コンストラクタ
  public:
	AThread_JavaScriptExecuter( AObjectArrayItem* inParent );
	~AThread_JavaScriptExecuter();
	
	//スレッドメインルーチン
  private:
	void	NVIDO_ThreadMain();
	
	//JavaScript実行
  public:
	ABool	SPNVI_Run_ExecuteJavaScript( const AText& inScriptText, const AFileAcc& inFolder );
	
	//完了フラグ
  public:
	ABool	SPI_IsCompleted() const { return mIsCompleted; }
  private:
	ABool		mIsCompleted;
	
	//データ
  private:
	AText		mScript;
	AFileAcc	mFolder;
};


#pragma mark ===========================
#pragma mark [クラス]AThread_JavaScriptExecuterFactory
//生成Factory
class AThread_JavaScriptExecuterFactory : public AThreadFactory
{
  public:
	AThread_JavaScriptExecuterFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AThread_JavaScriptExecuter*	Create() 
	{
		return new AThread_JavaScriptExecuter(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};
