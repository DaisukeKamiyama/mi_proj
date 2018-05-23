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

#include "stdafx.h"

#include "AColorWrapper.h"

/**
������ AApplication::AApplication()����R�[�������
*/
void	AColorWrapper::Init()
{
}

/**
�I��F�擾
*/
void	AColorWrapper::GetHighlightColor( AColor& outColor)
{
	outColor = ::GetSysColor(COLOR_HIGHLIGHT);//�yOS�݊����zWindows2000�ȍ~
}

/**
���X�g�I��F�擾
*/
void	AColorWrapper::GetListHighlightColor( AColor& outColor)
{
	outColor = ::GetSysColor(COLOR_HIGHLIGHT);//�yOS�݊����zWindows2000�ȍ~
}

/**
Deactivated���̃��X�g�I��F�擾
*/
void	AColorWrapper::GetListHighlightColorDeactivated( AColor& outColor)
{
	outColor = kColor_Gray;
}

/**
�F��r
*/
ABool	AColorWrapper::CompareColor( const AColor& inColor1, const AColor& inColor2 )
{
	return (inColor1==inColor2);
}

/**
�����ۂ��F�����ɕϊ�
*/
ABool	AColorWrapper::ChangeBlackIfWhite( AColor &ioColor )
{
	if( GetRValue(ioColor) > 234 && GetGValue(ioColor) > 234 && GetBValue(ioColor) > 234 )
	{
		ioColor = kColor_Black;
		return true;
	}
	else
	{
		return false;
	}
}

/**
�F����HTML�`���e�L�X�g�擾
*/
void	AColorWrapper::GetHTMLFormatColorText( const AColor inColor, AText& outText )
{
	outText.DeleteAll();
	outText.AddFormattedCstring("%02X",GetRValue(inColor));
	outText.AddFormattedCstring("%02X",GetGValue(inColor));
	outText.AddFormattedCstring("%02X",GetBValue(inColor));
}

/**
HTML�`���e�L�X�g����F���擾
*/
AColor	AColorWrapper::GetColorByHTMLFormatColor( const AText& inText )
{
	AColor	result = kColor_Black;
	if( inText.GetItemCount() == 6 )
	{
		result = RGB(inText.GetIntegerByHexChar(0) * 16 + inText.GetIntegerByHexChar(1),
				inText.GetIntegerByHexChar(2) * 16 + inText.GetIntegerByHexChar(3),
				inText.GetIntegerByHexChar(4) * 16 + inText.GetIntegerByHexChar(5));
	}
	return result;
}
AColor	AColorWrapper::GetColorByHTMLFormatColor( const char* inTextPtr )
{
	AText	text(inTextPtr);
	return GetColorByHTMLFormatColor(text);
}

//#597
/**
�~���F�擾
*/
AColor	AColorWrapper::GetGradientColor( const AColor& inStartColor, const AColor& inEndColor,
		const AFloatNumber inPercent )
{
	AFloatNumber	red = (GetRValue(inEndColor) - GetRValue(inStartColor));
	red *= inPercent;
	red /= 100;
	AFloatNumber	green = (GetGValue(inEndColor) - GetGValue(inStartColor));
	green *= inPercent;
	green /= 100;
	AFloatNumber	blue = (GetBValue(inEndColor) - GetBValue(inStartColor));
	blue *= inPercent;
	blue /= 100;
	return RGB(GetRValue(inStartColor)+red,GetGValue(inStartColor)+green,GetBValue(inStartColor)+blue);
}

//#634
/**
RGB->HSV�ϊ�
*/
void	AColorWrapper::GetHSVFromRGBColor( const AColor inColor, AFloatNumber& outH, AFloatNumber& outS, AFloatNumber& outV )
{
	outH = 0;
	outS = 0;
	outV = 0;
	//0�`1.0�ɐ��K��
	AFloatNumber	red = GetRValue(inColor);
	red /= 255;
	AFloatNumber	green = GetGValue(inColor);
	green /= 255;
	AFloatNumber	blue = GetBValue(inColor);
	blue /= 255;
	
	//Min/Max�擾
	AFloatNumber	max = red;
	AFloatNumber	min = red;
	if( green > max )
	{
		max = green;
	}
	if( green < min )
	{
		min = green;
	}
	if( blue > max )
	{
		max = blue;
	}
	if( blue < min )
	{
		min = blue;
	}
	
	//�v�Z
	if( max != min )
	{
		if( max == red )
		{
			//if MAX=R
			outH = 60.0 * (green-blue) / (max-min);
		}
		else if( max == green )
		{
			//if MAX=G
			outH = 60.0 * (blue-red) / (max-min) + 120.0;
		}
		else if( max == blue )
		{
			//if MAX=B
			outH = 60.0 * (red-green) / (max-min) + 240.0;
		}
	}
	outS = (max-min) / (max);
	outV = max;
}

//#634
/**
HSV->RGB�ϊ�
*/
AColor	AColorWrapper::GetRGBColorFromHSV( const AFloatNumber inH, const AFloatNumber inS, const AFloatNumber inV )
{
	AFloatNumber	h = inH;
	AFloatNumber	s = inS;
	AFloatNumber	v = inV;
	if( h > 360.0 )   h = 360.0;
	if( h < 0.0 )   h = 0.0;
	if( s > 1.0 )   s = 1.0;
	if( s < 0.0 )   s = 0.0;
	if( v > 1.0 )   v = 1.0;
	if( v < 0.0 )   v = 0.0;
	//
	ANumber	hi = /*floor*/(h/60.0);
	hi = hi%6;
	AFloatNumber	f = h/60.0 - hi;
	AFloatNumber	p = v * (1.0 - s);
	AFloatNumber	q = v * (1.0 - f*s);
	AFloatNumber	t = v * (1.0 - (1.0-f)*s);
	AFloatNumber	red = 0, green = 0, blue = 0;
	switch(hi)
	{
	  case 0:
		{
			red = v;
			green = t;
			blue = p;
			break;
		}
	  case 1:
		{
			red = q;
			green = v;
			blue = p;
			break;
		}
	  case 2:
		{
			red = p;
			green = v;
			blue = t;
			break;
		}
	  case 3:
		{
			red = p;
			green = q;
			blue = v;
			break;
		}
	  case 4:
		{
			red = t;
			green = p;
			blue = v;
			break;
		}
	  case 5:
		{
			red = v;
			green = p;
			blue = q;
			break;
		}
	  default:
		{
			_ACError("",NULL);
			break;
		}
	}
	//AColor�֕ϊ�
	AColor	result = kColor_White;
	return RGB(red*255,green*255,blue*255);
}

//#634
/**
HSV�̑���(H)�E�{��(S, V)�ŐF�ύX
*/
AColor	AColorWrapper::ChangeHSV( const AColor inColor, 
		const AFloatNumber inDeltaH, const AFloatNumber inMultiplyS, const AFloatNumber inMultiplyV )
{
	AFloatNumber	h = 0, s = 0, v = 0;
	GetHSVFromRGBColor(inColor,h,s,v);
	h += inDeltaH;
	if( h > 360.0 )   h = 360.0;
	if( h < 0.0 )   h = 0.0;
	s *= inMultiplyS;
	if( s > 1.0 )   s = 1.0;
	if( s < 0.0 )   s = 0.0;
	v *= inMultiplyV;
	if( v > 1.0 )   v = 1.0;
	if( v < 0.0 )   v = 0.0;
	return GetRGBColorFromHSV(h,s,v);
}

