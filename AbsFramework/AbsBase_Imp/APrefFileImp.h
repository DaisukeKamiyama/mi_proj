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

AAPrefFileImp

*/

#pragma once
#include "../AbsBase/ABase.h"

/**
設定ファイルImpクラス
*/
class APrefFileImp
{
  public:
	APrefFileImp();
	~APrefFileImp();
	
#if IMPLEMENTATION_FOR_MACOSX
	ABool	Load( const AFileAcc& inFile, const ResType& inResType = NULL );
	void	Write( const AFileAcc& inFile/*#1034 , const ResType& inResType = NULL*/ );
#else
	ABool	Load( const AFileAcc& inFile );
	void	Write( const AFileAcc& inFile );
#endif
	//Bool
	ABool	GetData( const AText& inKey, ABool& outData );
	void	SetData( const AText& inKey, const ABool& inData );
	//Number
	ABool	GetData( const AText& inKey, ANumber& outData );
	void	SetData( const AText& inKey, const ANumber& inData );
	//FloatNumber
	ABool	GetData( const AText& inKey, AFloatNumber& outData );
	void	SetData( const AText& inKey, const AFloatNumber& inData );
	//Text
	ABool	GetData( const AText& inKey, AText& outData );
	void	SetData( const AText& inKey, const AText& inData );
	//Point
	ABool	GetData( const AText& inKey, APoint& outData );
	void	SetData( const AText& inKey, const APoint& inData );
	//Rect
	ABool	GetData( const AText& inKey, ARect& outData );
	void	SetData( const AText& inKey, const ARect& inData );
	//Font
	/*win ABool	GetData( const AText& inKey, AFont& outData );
	void	SetData( const AText& inKey, const AFont& inData );*/
	//Color
	ABool	GetData( const AText& inKey, AColor& outData );
	void	SetData( const AText& inKey, const AColor& inData );
	//TextArray
	ABool	GetData( const AText& inKey, ATextArray& outData );
	void	SetData( const AText& inKey, const ATextArray& inData );
	//BoolArray
	ABool	GetData( const AText& inKey, ABoolArray& outData );
	void	SetData( const AText& inKey, const ABoolArray& inData );
	//NumberArray
	ABool	GetData( const AText& inKey, ANumberArray& outData );
	void	SetData( const AText& inKey, const ANumberArray& inData );
	//ColorArray
	ABool	GetData( const AText& inKey, AColorArray& outData );
	void	SetData( const AText& inKey, const AColorArray& inData );
	
#if IMPLEMENTATION_FOR_MACOSX
  private:
	CFMutableDictionaryRef	mDictionary;
#endif
};






