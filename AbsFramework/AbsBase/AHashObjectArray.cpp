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

AHashObjectArray

*/

#include "stdafx.h"

#include "AHashObjectArray.h"
#include "AText.h"

#pragma mark ===========================
#pragma mark [クラス]AHashObjectArray<>
#pragma mark ===========================
#pragma mark ---ハッシュ関数
//ATextArray
template<> unsigned long	AHashObjectArray<AText,AText>::GetHashValue( const AText& inData, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	AItemCount	targetItemCount = inTargetItemCount;
	if( targetItemCount == kIndex_Invalid )
	{
		targetItemCount = inData.GetItemCount();
	}
	/*win 080726 if( targetItemCount == 0 )
	{
		return 12345;
	}
	else if( targetItemCount == 1 )
	{
		return inData.Get(inTargetIndex)*111;
	}
	else if( targetItemCount == 2 )
	{
		return inData.Get(inTargetIndex)*111 + inData.Get(inTargetIndex+1)*222;
	}
	else 
	{
		return inData.Get(inTargetIndex)*111 + inData.Get(inTargetIndex+1)*222 + inData.Get(inTargetIndex+2)*333 + targetItemCount*999;
	}*/
	if( targetItemCount == 0 )
	{
		return 1234567;
	}
	else if( targetItemCount == 1 )
	{
		return inData.Get(inTargetIndex)*67890123;
	}
	else if( targetItemCount == 2 )
	{
		return inData.Get(inTargetIndex)*78901234 + inData.Get(inTargetIndex+1)*891012345;
	}
	else 
	{
		return inData.Get(inTargetIndex)*8901234 + inData.Get(inTargetIndex+1)*9012345 
			+ inData.Get(inTargetIndex+targetItemCount-2)*1234567 + inData.Get(inTargetIndex+targetItemCount-1) + targetItemCount*4567;
	}
}


