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

AHashArray

*/

#include "stdafx.h"

#include "AHashArray.h"

#pragma mark ===========================
#pragma mark [クラス]AHashArray<>
#pragma mark ===========================
#pragma mark ---各データ型ごとのハッシュ関数

template<> unsigned long	AHashArray<ANumber>::GetHashValue( const ANumber inData ) const
{
	return static_cast<unsigned long>(inData);
}

template<> unsigned long	AHashArray<unsigned long>::GetHashValue( const unsigned long inData ) const
{
	return static_cast<unsigned long>(inData);
}

//win
template<> unsigned long	AHashArray<unsigned int>::GetHashValue( const unsigned int inData ) const
{
	return static_cast<unsigned long>(inData);
}

template<> unsigned long	AHashArray<AMenuRef>::GetHashValue( const AMenuRef inData ) const
{
	return reinterpret_cast<unsigned long>(inData);
}

typedef const AArrayAbstract* ConstAArrayAbstractPtr;
template<> unsigned long	AHashArray<ConstAArrayAbstractPtr>::GetHashValue( const ConstAArrayAbstractPtr inData ) const
{
	return reinterpret_cast<unsigned long>(inData);
}

//#423
template<> unsigned long	AHashArray<AObjectID>::GetHashValue( const AObjectID inData ) const
{
	return reinterpret_cast<unsigned long>(inData.val);//#693
}

//#1290
template<> unsigned long	AHashArray<AWindowRef>::GetHashValue( const AWindowRef inData ) const
{
	return reinterpret_cast<unsigned long>(inData);
}

//#693
template<> unsigned long	AHashArray<AUChar>::GetHashValue( const AUChar inData ) const
{
	return static_cast<unsigned long>(inData);
}

//#693
template<> unsigned long	AHashArray<AUniqID>::GetHashValue( const AUniqID inData ) const
{
	return static_cast<unsigned long>(inData.val*2);//ハッシュの再構成トリガーの閾値が2倍なので、*2にした
}

//#693
template<> unsigned long	AHashArray<const void*>::GetHashValue( const void* inData ) const
{
	return reinterpret_cast<unsigned long>(inData);
}

