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

ASecurityWrapper

*/

#include "ASecurityWrapper.h"
#include <Security/Security.h>

/**
InternetópPasswordéÊìæ
*/
ABool	ASecurityWrapper::GetInternetPassword( const AText& inServer, const AText& inUser, AText& outPassword )
{
	outPassword.DeleteAll();
	AStTextPtr	pserver(inServer,0,inServer.GetItemCount()), puser(inUser,0,inUser.GetItemCount());
	void*	passwordData = NULL;
	UInt32	passwordLen = 0;
	if( ::SecKeychainFindInternetPassword(NULL,
			pserver.GetByteCount(),(char*)(pserver.GetPtr()),0,NULL,
			puser.GetByteCount(),(char*)(puser.GetPtr()),0,NULL,0,
			0,0,&passwordLen,&passwordData,NULL) == noErr )
	{
		if( passwordData != NULL )
		{
			outPassword.SetFromTextPtr((char*)passwordData,passwordLen);
			::SecKeychainItemFreeContent(NULL,passwordData);
			return true;
		}
	}
	return false;
}

/**
InternetópPasswordê›íË
*/
void	ASecurityWrapper::SetInternetPassword( const AText& inServer, const AText& inUser, const AText& inPassword )
{
	AStTextPtr	pserver(inServer,0,inServer.GetItemCount()), puser(inUser,0,inUser.GetItemCount()), ppassword(inPassword,0,inPassword.GetItemCount());
	SecKeychainItemRef	itemRef;
	if( ::SecKeychainFindInternetPassword(NULL,
			pserver.GetByteCount(),(char*)(pserver.GetPtr()),0,NULL,
			puser.GetByteCount(),(char*)(puser.GetPtr()),0,NULL,0,
			0,0,NULL,NULL,&itemRef) == noErr )
	{
		::SecKeychainItemDelete(itemRef);
	}
	OSStatus status = ::SecKeychainAddInternetPassword(NULL,
			pserver.GetByteCount(),(char*)(pserver.GetPtr()),0,NULL,
			puser.GetByteCount(),(char*)(puser.GetPtr()),0,NULL,0,
			0,0,ppassword.GetByteCount(),ppassword.GetPtr(),NULL);
	if( status != noErr )   _AError("password cannot be added",NULL);
}

