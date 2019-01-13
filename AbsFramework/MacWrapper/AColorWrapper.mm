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

#include "AColorWrapper.h"

//B0000 高速化
AColor	AColorWrapper::sHighlightColor;

//B0000 高速化
/**
初期化 AApplication::AApplication()からコールされる
*/
void	AColorWrapper::Init()
{
	OSStatus	err = noErr;
	
	/*#1034
	Collection	collect = ::NewCollection();
	err = ::GetTheme(collect);
	if( err != noErr )   _ACErrorNum("GetTheme() returned error: ",err,NULL);
	SInt32	size = sizeof(RGBColor);
	err = ::GetCollectionItem(collect,kThemeHighlightColorTag,0,&size,&sHighlightColor);
	if( err != noErr )   _ACErrorNum("GetCollectionItem() returned error: ",err,NULL);
	::DisposeCollection(collect);
	*/
	NSColor*	selectedColor = [NSColor selectedTextBackgroundColor];
	CGFloat	r = 0, g = 0, b = 0, a = 0;
	[[selectedColor colorUsingColorSpaceName:NSDeviceRGBColorSpace] getRed:&r green:&g blue:&b alpha:&a];
	sHighlightColor.red = r*65535;
	sHighlightColor.green = g*65535;
	sHighlightColor.blue = b*65535;
}

/**
選択色取得
*/
void	AColorWrapper::GetHighlightColor( AColor& outColor)
{
	/* B0000 高速化 Collection	collect = ::NewCollection();
	::GetTheme(collect);
	SInt32	size = sizeof(RGBColor);
	::GetCollectionItem(collect,kThemeHighlightColorTag,0,&size,&outColor);
	::DisposeCollection(collect);*/
	outColor = sHighlightColor;
}

//#1316
/**
テキスト選択色取得
システム環境設定の変更を即反映させたいので、staticメンバー変数にキャッシュする方法は今回は使わない（処理速度に問題があれば再検討）
*/
AColor	AColorWrapper::GetHighlightColor()
{
    CGFloat    r = 0, g = 0, b = 0, a = 0;
	[[[NSColor selectedTextBackgroundColor] colorUsingColorSpaceName:NSDeviceRGBColorSpace] getRed:&r green:&g blue:&b alpha:&a];
	AColor	color = {r*65535,g*65535,b*65535};
	return color;
}

//#1316
/**
コントロールアクセント色取得
macOS10.14+の場合は「アクセントカラー」、10.14未満の場合はテキスト選択色と同じ。
システム環境設定の変更を即反映させたいので、staticメンバー変数にキャッシュする方法は今回は使わない（処理速度に問題があれば再検討）
*/
AColor	AColorWrapper::GetControlAccentColor()
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_14 )
	{
		CGFloat    r = 0, g = 0, b = 0, a = 0;
		[[[NSColor controlAccentColor] colorUsingColorSpaceName:NSDeviceRGBColorSpace] getRed:&r green:&g blue:&b alpha:&a];
		AColor	color = {r*65535,g*65535,b*65535};
		return color;
	}
	else
	{
		return AColorWrapper::GetHighlightColor();
	}
}

//#1034
#if 0 
/**
リスト選択色取得
*/
void	AColorWrapper::GetListHighlightColor( AColor& outColor)
{
	OSStatus	err = noErr;
	
	err = ::GetThemeBrushAsColor(kThemeBrushAlternatePrimaryHighlightColor,65536,true,&outColor);
	if( err != noErr )   _ACErrorNum("GetThemeBrushAsColor() returned error: ",err,NULL);
	outColor = kColor_Blue;
}

/**
Deactivated時のリスト選択色取得
*/
void	AColorWrapper::GetListHighlightColorDeactivated( AColor& outColor)
{
	OSStatus	err = noErr;
	
	err = ::GetThemeBrushAsColor(kThemeBrushSecondaryHighlightColor,65536,true,&outColor);
	if( err != noErr )   _ACErrorNum("GetThemeBrushAsColor() returned error: ",err,NULL);
	outColor = kColor_Blue;
}
#endif

/**
色比較
*/
ABool	AColorWrapper::CompareColor( const AColor& inColor1, const AColor& inColor2 )
{
	return (inColor1.red==inColor2.red && inColor1.green==inColor2.green && inColor1.blue==inColor2.blue);
}

/**
白っぽい色を黒に変換
*/
ABool	AColorWrapper::ChangeBlackIfWhite( AColor &ioColor )
{
	if( ioColor.red > 60000 && ioColor.green > 60000 && ioColor.blue > 60000 )
	{
		ioColor.red = ioColor.green = ioColor.blue = 0;
		return true;
	}
	else
	{
		return false;
	}
}

/**
色からHTML形式テキスト取得
*/
void	AColorWrapper::GetHTMLFormatColorText( const AColor inColor, AText& outText )
{
	outText.DeleteAll();
	outText.AddFormattedCstring("%02X",inColor.red/256);
	outText.AddFormattedCstring("%02X",inColor.green/256);
	outText.AddFormattedCstring("%02X",inColor.blue/256);
}

/**
HTML形式テキストから色を取得
*/
AColor	AColorWrapper::GetColorByHTMLFormatColor( const AText& inText )
{
	AColor	result = kColor_Black;
	if( inText.GetItemCount() == 6 )
	{
		/*#0 FFを65535に変換するように計算式変更
		result.red		= (inText.GetIntegerByHexChar(0) * 16 + inText.GetIntegerByHexChar(1))*256;
		result.green	= (inText.GetIntegerByHexChar(2) * 16 + inText.GetIntegerByHexChar(3))*256;
		result.blue		= (inText.GetIntegerByHexChar(4) * 16 + inText.GetIntegerByHexChar(5))*256;
		*/
		result.red		= (inText.GetIntegerByHexChar(0) * 16 + inText.GetIntegerByHexChar(1))*257;
		result.green	= (inText.GetIntegerByHexChar(2) * 16 + inText.GetIntegerByHexChar(3))*257;
		result.blue		= (inText.GetIntegerByHexChar(4) * 16 + inText.GetIntegerByHexChar(5))*257;
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
諧調色取得
*/
AColor	AColorWrapper::GetGradientColor( const AColor& inStartColor, const AColor& inEndColor,
		const AFloatNumber inPercent )
{
	AFloatNumber	red = inEndColor.red - inStartColor.red;
	red *= inPercent;
	red /= 100;
	AFloatNumber	green = inEndColor.green - inStartColor.green;
	green *= inPercent;
	green /= 100;
	AFloatNumber	blue = inEndColor.blue - inStartColor.blue;
	blue *= inPercent;
	blue /= 100;
	AColor	result = inStartColor;
	result.red += red;
	result.green += green;
	result.blue += blue;
	return result;
}

/* UInt16 GetDepth(void)
{
	GDHandle theDevice;
	CGrafPtr port;
	
	GetGWorld(&port, &theDevice);
	PixMapHandle screenPMapH = GetGWorldPixMap((GWorldPtr)port);
	return GetPixDepth(screenPMapH);
}*/

//#634
/**
RGB->HSV変換
*/
void	AColorWrapper::GetHSVFromRGBColor( const AColor inColor, AFloatNumber& outH, AFloatNumber& outS, AFloatNumber& outV )
{
	outH = 0;
	outS = 0;
	outV = 0;
	//0～1.0に正規化
	AFloatNumber	red = inColor.red;
	red /= 65535;
	AFloatNumber	green = inColor.green;
	green /= 65535;
	AFloatNumber	blue = inColor.blue;
	blue /= 65535;
	
	//Min/Max取得
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
	
	//計算
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
HSV->RGB変換
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
	ANumber	hi = floor(h/60.0);
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
	//AColorへ変換
	AColor	result = kColor_White;
	result.red = red*65535;
	result.green = green*65535;
	result.blue = blue*65535;
	return result;
}

//#634
/**
HSVの増減(H)・倍率(S, V)で色変更
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

//#1142
/**
アルファブレンディング
*/
AColor	AColorWrapper::GetAlpheBlend( const AColor& inBackgroundColor, const AColor& inColor, const AFloatNumber inAlpha )
{
	AColor	result = inColor;
	result.red		= inBackgroundColor.red*(1-inAlpha)		+ inColor.red*inAlpha;
	result.green	= inBackgroundColor.green*(1-inAlpha)	+ inColor.green*inAlpha;
	result.blue		= inBackgroundColor.blue*(1-inAlpha)	+ inColor.blue*inAlpha;
	return result;
}

