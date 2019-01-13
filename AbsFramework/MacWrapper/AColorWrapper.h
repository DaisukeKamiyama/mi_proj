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

AColorWrapper

*/

#pragma once

#include "../AbsBase/ABase.h"

/**
ColorÉâÉbÉpÅ[
*/
class AColorWrapper
{
  public:
	static void	Init();//B0000 çÇë¨âª
	static void	GetHighlightColor( AColor& outColor);
	static AColor	GetHighlightColor();//#1316
	static AColor	GetControlAccentColor();//#1316
	//#1034 static void	GetListHighlightColor( AColor& outColor);
	//#1034 static void	GetListHighlightColorDeactivated( AColor& outColor);
	static ABool	CompareColor( const AColor& inColor1, const AColor& inColor2 );
	static ABool	ChangeBlackIfWhite( AColor &ioColor );
	static void	GetHTMLFormatColorText( const AColor inColor, AText& outText );
	static AColor	GetColorByHTMLFormatColor( const AText& inText );
	static AColor	GetColorByHTMLFormatColor( const char* inTextPtr );
	static AColor	GetGradientColor( const AColor& inStartColor, const AColor& inEndColor,
				const AFloatNumber inPercent );//#597
	static void	GetHSVFromRGBColor( const AColor inColor, AFloatNumber& outH, AFloatNumber& outS, AFloatNumber& outV );//#634
	static AColor	GetRGBColorFromHSV( const AFloatNumber inH, const AFloatNumber inS, const AFloatNumber inV );//#634
	static AColor	ChangeHSV( const AColor inColor, 
					const AFloatNumber inDeltaH, const AFloatNumber inMultiplyS, const AFloatNumber inMultiplyV );//#634
	static AColor	GetAlpheBlend( const AColor& inBackgroundColor, const AColor& inColor, const AFloatNumber inAlpha );//#1142
  private:
	static AColor	sHighlightColor;//B0000 çÇë¨âª
};


