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

AInternalEvent.cp

*/

#include "stdafx.h"

#include "ABase.h"
#include "../Wrapper.h"
#include "../Imp.h"

AInternalEventQueue::AInternalEventQueue( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent)
{
}

ABool	AInternalEventQueue::Dequeue( AInternalEventType& outType, AObjectID& outSenderObjectID, 
		ANumber& outNumber, AText& outText, AObjectIDArray& outObjectIDArray )
{
	AStMutexLocker	mutexlocker(mQueueMutex);
	ABool	result = false;
	outType = kInternalEvent_NOP;
	if( mQueue_Event.GetItemCount() > 0 )
	{
		outType 			= mQueue_Event.Get(0);
		outSenderObjectID	= mQueue_SenderObject.Get(0);
		outNumber 			= mQueue_Number.Get(0);
		outText.			SetText(mQueue_Text.GetObjectConst(0));
		outObjectIDArray.	SetFromObject(mQueue_ObjectIDArray.GetObjectConst(0));
		
		mQueue_Event.		Delete1(0);
		mQueue_SenderObject.Delete1(0);
		mQueue_Number.		Delete1(0);
		mQueue_Text.		Delete1Object(0);
		mQueue_ObjectIDArray.Delete1Object(0);
		
		result = true;
	}
	return result;
}

AItemCount	AInternalEventQueue::GetQueueCount() const
{
	AStMutexLocker	mutexlocker(mQueueMutex);
	return mQueue_Event.GetItemCount();
}

#if IMPLEMENTATION_FOR_MACOSX
extern void PostInternalEvent();
#endif

void	AInternalEventQueue::Post( const AInternalEventType& inType, const AObjectID inSenderObjectID, 
		const ANumber& inNumber, const AText& inText, const AObjectIDArray& inObjectIDArray )
{
	AStMutexLocker	mutexlocker(mQueueMutex);
	mQueue_Event.Add(inType);
	mQueue_SenderObject.Add(inSenderObjectID);
	mQueue_Number.Add(inNumber);
	mQueue_Text.GetObject(mQueue_Text.AddNewObject()).SetText(inText);
	mQueue_ObjectIDArray.GetObject(mQueue_ObjectIDArray.AddNewObject()).SetFromObject(inObjectIDArray);
	
#if IMPLEMENTATION_FOR_MACOSX
	/*#688 
	EventRef	event;
	::CreateEvent(NULL,'user','user',0,kEventAttributeUserEvent,&event);
	::PostEventToQueue(::GetMainEventQueue(),event,kEventPriorityStandard);
	*/
	//NSApplicationDefinedイベントを送信
	PostInternalEvent();
#else
	::PostMessage(CAppImp::GetMessageOnlyWindow(),WM_APP_INTERNALEVENT,0,0);
#endif
}

//#699
/**
キューから指定イベントを削除（タイプ、送信元ObjectIDが一致するもの）
*/
void	AInternalEventQueue::RemoveEvent( const AInternalEventType inType, const AObjectID inSenderObjectID )
{
	AStMutexLocker	mutexlocker(mQueueMutex);
	for( AIndex i = 0; i < mQueue_Event.GetItemCount(); )
	{
		if( mQueue_Event.Get(i) == inType && mQueue_SenderObject.Get(i) == inSenderObjectID )
		{
			//一致するものを削除
			mQueue_Event.Delete1(i);
			mQueue_SenderObject.Delete1(i);
			mQueue_Number.Delete1(i);
			mQueue_Text.Delete1Object(i);
			mQueue_ObjectIDArray.Delete1Object(i);
		}
		else
		{
			i++;
		}
	}
}

//#1056
/**
キュー内に指定タイプのイベントが存在するかどうかを返す
*/
ABool	AInternalEventQueue::ExistEventInInternalEventQueue( const AInternalEventType inType )
{
	return (mQueue_Event.Find(inType)!=kIndex_Invalid);
}

