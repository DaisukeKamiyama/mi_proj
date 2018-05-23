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

ABase

*/

#include "stdafx.h"

#include "ABase.h"
#include "../Wrapper.h"
#include "APurgeManager.h"

ABase*	gBase = NULL;

#pragma mark ===========================
#pragma mark [クラス]ABase
#pragma mark ===========================
//必ず最初に生成されるべきクラス

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
ABase::ABase( const ABool& inUseTraceLog ) 
	: mUseTraceLog(inUseTraceLog)/*#344 メモリープール機構削除（仮想メモリ環境では使用される状況がまずありえないので）, 
	mLevel1MemoryPool(AMemoryWrapper::Malloc(kByteCount_MemoryPoolLevel1Size)), 
	mLevel2MemoryPool(AMemoryWrapper::Malloc(kByteCount_MemoryPoolLevel2Size))*/
{
	//gBase設定
	gBase = this;
	
	//テキストエンコーディングラッパー初期化
	ATextEncodingWrapper::Init();
	
	//#693
	//#941 必要時に生成するようにする new APurgeManager();
	
	//フォントラッパー初期化 win
	//AApplication::AApplication()へ移動（LocalizedTextを使用するため) AFontWrapper::Init();
	
	mMainThreadID = AThreadWrapper::GetCurrentThreadID();
	
	_AInfo("ABase init completed",NULL);
}

/**
デストラクタ
*/
ABase::~ABase()
{
	gBase = NULL;
}

/**
現在MainThread内かどうかを判定する
*/
ABool	ABase::InMainThread() const
{
	return (mMainThreadID == AThreadWrapper::GetCurrentThreadID());
}

#pragma mark ===========================

#pragma mark ---メイン内部イベントキューPost/Dequeue

/**
キューからイベント取り出し
*/
ABool	ABase::DequeueFromMainInternalEventQueue( AInternalEventType& outType, AObjectID& outSenderObjectID, 
		ANumber& outNumber, AText& outText, AObjectIDArray& outObjectIDArray )
{
	if( gBase != NULL )
	{
		return gBase->mMainInternalEventQueue.Dequeue(outType,outSenderObjectID,outNumber,outText,outObjectIDArray);
	}
	return false;
}

/**
キュー内のイベント項目数取得
*/
AItemCount	ABase::GetMainInternalEventQueueCount()
{
	if( gBase != NULL )
	{
		return gBase->mMainInternalEventQueue.GetQueueCount();
	}
	return false;
}

/**
イベント送信
*/
void	ABase::PostToMainInternalEventQueue( const AInternalEventType& inType, const AObjectID inSenderObjectID,
		const ANumber& inNumber, const AText& inText, const AObjectIDArray& inObjectIDArray )
{
	if( gBase != NULL )
	{
		gBase->mMainInternalEventQueue.Post(inType,inSenderObjectID,inNumber,inText,inObjectIDArray);
	}
}

//#698
/**
指定イベントをキューから削除
*/
void	ABase::RemoveEventFromMainInternalEventQueue( const AInternalEventType inType, const AObjectID inSenderObjectID )
{
	if( gBase != NULL )
	{
		gBase->mMainInternalEventQueue.RemoveEvent(inType,inSenderObjectID);
	}
}

//#1056
/**
キュー内に指定タイプのイベントが存在するかどうかを返す
*/
ABool	ABase::ExistEventInInternalEventQueue( const AInternalEventType inType )
{
	if( gBase != NULL )
	{
		return gBase->mMainInternalEventQueue.ExistEventInInternalEventQueue(inType);
	}
	return false;
}


#pragma mark ===========================

#pragma mark ---空Arrayの取得

const AText				kEmptyText;
const ATextArray		kEmptyTextArray;
const ABoolArray		kEmptyBoolArray;
const ANumberArray		kEmptyNumberArray;
const AColorArray		kEmptyColorArray;
const AObjectIDArray	kEmptyObjectIDArray;
const AHashNumberArray	kEmptyHashNumberArray;

/**
空Text取得
*/
const AText&	GetEmptyText()
{
	return kEmptyText;
}

/**
空TextArray取得
*/
const ATextArray&	GetEmptyTextArray()
{
	return kEmptyTextArray;
}

/**
空BoolArray取得
*/
const ABoolArray&	GetEmptyBoolArray()
{
	return kEmptyBoolArray;
}

/**
空NumberArray取得
*/
const ANumberArray&	GetEmptyNumberArray()
{
	return kEmptyNumberArray;
}

/**
空ColorArray取得
*/
const AColorArray&	GetEmptyColorArray()
{
	return kEmptyColorArray;
}

/**
空ObjectIDArray取得
*/
const AObjectIDArray&	GetEmptyObjectIDArray()
{
	return kEmptyObjectIDArray;
}

/**
空HashNumberArray取得
*/
const AHashNumberArray&	GetEmptyHashNumberArray()
{
	return kEmptyHashNumberArray;
}

#pragma mark ===========================

#pragma mark ---ANumberユーティリティ
//#688

/**
最小値を取得
*/
ANumber	GetMinValue( const ANumber inNumber1, const ANumber inNumber2 )
{
	if( inNumber1 < inNumber2 )
	{
		return inNumber1;
	}
	else
	{
		return inNumber2;
	}
}

/**
最大値を取得
*/
ANumber	GetMaxValue( const ANumber inNumber1, const ANumber inNumber2 )
{
	if( inNumber1 > inNumber2 )
	{
		return inNumber1;
	}
	else
	{
		return inNumber2;
	}
}


#pragma mark ===========================

#pragma mark ---APointユーティリティ

/**
Point順序比較
*/
void	OrderPoint( const APoint& inSelectTextPoint1, const APoint& inSelectTextPoint2, APoint& outStartTextPoint, APoint& outEndTextPoint ) 
{
	if( inSelectTextPoint1.y < inSelectTextPoint2.y ) 
	{
		outStartTextPoint = inSelectTextPoint1;
		outEndTextPoint = inSelectTextPoint2;
	}
	else if( inSelectTextPoint1.y == inSelectTextPoint2.y )
	{
		if( inSelectTextPoint1.x < inSelectTextPoint2.x )
		{
			outStartTextPoint = inSelectTextPoint1;
			outEndTextPoint = inSelectTextPoint2;
		}
		else
		{
			outStartTextPoint = inSelectTextPoint2;
			outEndTextPoint = inSelectTextPoint1;
		}
	}
	else
	{
		outStartTextPoint = inSelectTextPoint2;
		outEndTextPoint = inSelectTextPoint1;
	}
}

//R0108
/**
AImagePoint順序比較
*/
void	OrderImagePoint( const AImagePoint& inSelectTextPoint1, const AImagePoint& inSelectTextPoint2, AImagePoint& outStartTextPoint, AImagePoint& outEndTextPoint ) 
{
	if( inSelectTextPoint1.y < inSelectTextPoint2.y ) 
	{
		outStartTextPoint = inSelectTextPoint1;
		outEndTextPoint = inSelectTextPoint2;
	}
	else if( inSelectTextPoint1.y == inSelectTextPoint2.y )
	{
		if( inSelectTextPoint1.x < inSelectTextPoint2.x )
		{
			outStartTextPoint = inSelectTextPoint1;
			outEndTextPoint = inSelectTextPoint2;
		}
		else
		{
			outStartTextPoint = inSelectTextPoint2;
			outEndTextPoint = inSelectTextPoint1;
		}
	}
	else
	{
		outStartTextPoint = inSelectTextPoint2;
		outEndTextPoint = inSelectTextPoint1;
	}
}

//win 080618
/**
ATextPoint順序比較
*/
void	OrderTextPoint( const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2, 
		ATextPoint& outStartTextPoint, ATextPoint& outEndTextPoint ) 
{
	if( inSelectTextPoint1.y < inSelectTextPoint2.y ) 
	{
		outStartTextPoint = inSelectTextPoint1;
		outEndTextPoint = inSelectTextPoint2;
	}
	else if( inSelectTextPoint1.y == inSelectTextPoint2.y )
	{
		if( inSelectTextPoint1.x < inSelectTextPoint2.x )
		{
			outStartTextPoint = inSelectTextPoint1;
			outEndTextPoint = inSelectTextPoint2;
		}
		else
		{
			outStartTextPoint = inSelectTextPoint2;
			outEndTextPoint = inSelectTextPoint1;
		}
	}
	else
	{
		outStartTextPoint = inSelectTextPoint2;
		outEndTextPoint = inSelectTextPoint1;
	}
}

#pragma mark ===========================

#pragma mark ---ALocalRectユーティリティ
//#688

/**
２つのLocalRectの重なり部分を取得
*/
ABool	GetIntersectRect( const ALocalRect& inRect1, const ALocalRect& inRect2, ALocalRect& outIntersectRect )
{
	outIntersectRect.left		= 0;
	outIntersectRect.top		= 0;
	outIntersectRect.right		= 0;
	outIntersectRect.bottom		= 0;
	//X方向の重なり部分を計算
	ANumber	maxLeft = GetMaxValue(inRect1.left,inRect2.left);
	ANumber	minRight = GetMinValue(inRect1.right,inRect2.right);
	if( minRight <= maxLeft )
	{
		//重なっていない
		return false;
	}
	//Y方向の重なり部分を計算
	ANumber	maxTop = GetMaxValue(inRect1.top,inRect2.top);
	ANumber	minBottom = GetMinValue(inRect1.bottom,inRect2.bottom);
	if( minBottom <= maxTop )
	{
		//重なっていない
		return false;
	}
	//結果
	outIntersectRect.left		= maxLeft;
	outIntersectRect.top		= maxTop;
	outIntersectRect.right		= minRight;
	outIntersectRect.bottom		= minBottom;
	return true;
}

//#688
/**
LocalPointがLoaclRect内にあるかどうかを判定
*/
ABool	IsPointInRect( const ALocalPoint inPoint, const ALocalRect& inRect )
{
	if( inPoint.x >= inRect.left && inPoint.x <= inRect.right && 
				inPoint.y >= inRect.top && inPoint.y <= inRect.bottom )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//#850
/**
ドラッグ開始を判定（最初のクリック時のポイントからの差分が大きくなったらtrueを返す）
*/
ABool	IsDragStart( const ALocalPoint& inCurrentPoint, const ALocalPoint& inOriginalPoint )
{
	if( abs(inCurrentPoint.x - inOriginalPoint.x) > 8 || abs(inCurrentPoint.y - inOriginalPoint.y) > 8 )
	{
		return true;
	}
	else
	{
		return false;
	}
}


