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

AInternalEvent.h

*/

#pragma once

#include "AObjectArrayItem.h"
/**
内部イベントキュー
スレッド間送受信に使用可能
*/
class AInternalEventQueue : public AObjectArrayItem
{
  public:
	AInternalEventQueue( AObjectArrayItem* inParent = NULL );
	
	ABool	Dequeue( AInternalEventType& outType, AObjectID& outSenderObjectID, 
			ANumber& outNumber, AText& outText, AObjectIDArray& outObjectIDArray );
	AItemCount	GetQueueCount() const;
	void	Post( const AInternalEventType& inType, const AObjectID inSenderObjectID,
			const ANumber& inNumber, const AText& inText, const AObjectIDArray& inObjectIDArray );
	void	RemoveEvent( const AInternalEventType inType, const AObjectID inSenderObjectID );
	ABool	ExistEventInInternalEventQueue( const AInternalEventType inType );//#1056
  private:
	AArray<AInternalEventType>		mQueue_Event;
	AArray<AObjectID>				mQueue_SenderObject;
	AArray<ANumber>					mQueue_Number;
	AObjectArray<AText>				mQueue_Text;
	AObjectArray<AObjectIDArray>	mQueue_ObjectIDArray;
	
	mutable AThreadMutex	mQueueMutex;
	
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AInternalEventQueue"; }
	
};
