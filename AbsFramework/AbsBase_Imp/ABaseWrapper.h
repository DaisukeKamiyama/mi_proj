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

ABaseWrapper.h
基本OSコールラッパー

*/

#pragma once

#include "../AbsBase/ABaseTypes.h"

#if IMPLEMENTATION_FOR_MACOSX
#include <malloc/malloc.h>
#pragma mark ===========================
#pragma mark MacOSX用

#pragma mark ===========================
#pragma mark [クラス]ACstringWrapper
/**
C文字列処理ラッパー
*/
class ACstringWrapper
{
  private:
	friend class AText;
	
	/**
	C文字列長取得
	*/
	static AByteCount GetLength( AConstCharPtr inString )
	{
		for( AByteCount i = 0; i < 0x7FFFFFFF; i++ )
		{
			if( inString[i] == 0 )
			{
				return i;
			}
		}
		return 0x7FFFFFFF;
	}
	
	//#1034 va_listを都度設定するようにするため、AText側で直接実行するようにした
#if 0
	/**
	C文字列sprintf後の長さを取得
	*/
	static AByteCount GetFormattedCstringLength( const AConstCharPtr inFormat, va_list inArgList )
	{
		char	dummy;
		return vsnprintf(&dummy,0,inFormat,inArgList);//サイズが無制限だったとした場合のサイズが返る（最後のNULL文字は含まない）
	}
	
	/**
	C文字列sprintf
	*/
	static void	Sprintf( const ACharPtr ioTextPtr, const AByteCount inTextBufferSize, const AConstCharPtr inFormat, va_list inArgList )
	{
		vsnprintf(ioTextPtr,inTextBufferSize,inFormat,inArgList);
	}
#endif
};

#pragma mark ===========================
#pragma mark [クラス]AMemoryWrapper
/**
メモリー管理ラッパー
*/
class AMemoryWrapper
{
  public:
	/**
	メモリ割り当て
	*/
	static void*	Malloc( AByteCount inByteCount )
	{
		return ::malloc(inByteCount);
	}
	
	/**
	メモリ解放
	*/
	static void	Free( void* inPtr )
	{
		::free(inPtr);
	}
	
	/**
	メモリ再割り当て
	*/
	static void*	Realloc( void* inPtr, AByteCount inByteCount )
	{
		return ::realloc(inPtr,inByteCount);
	}
	
	/**
	メモリ内容コピー
	*/
	static void	Memmove( void* inDst, const void* inSrc, AByteCount inByteCount )
	{
		::memmove(inDst,inSrc,inByteCount);
	}
	
	/**
	メモリ内容比較
	*/
	static ABool	Memcmp( const void* inA, const void* inB, const AByteCount inByteCount )
	{
		/*#1275 memcmpの方が速い（多数回実行テスト結果：memcmp：3.07s 自作:4.40s）のでmemcmpにする。）
		//memcmpでも良いかもしれない。（memcmpのロジックの中身が分からないので、速度的懸念から、自作にしている。）
		const unsigned char* ptra = reinterpret_cast<const unsigned char*>(inA);
		const unsigned char* ptrb = reinterpret_cast<const unsigned char*>(inB);
		for( AByteCount i = 0; i < inByteCount; i++ )
		{
			if( ptra[i] != ptrb[i] )   return false;
		}
		return true;
		*/
		return (::memcmp(inA,inB,inByteCount)==0);
	}
	
	/**
	メモリ内容を0で埋める
	*/
	static void	Fill0( void* inPtr, AByteCount inByteCount )
	{
		unsigned char* ptr = reinterpret_cast<unsigned char*>(inPtr);
		for( AByteCount i = 0; i < inByteCount; i++ )
		{
			ptr[i] = 0;
		}
	}
	
	/**
	メモリ割り当てサイズ取得（padding含む）
	*/
	static AByteCount	AllocatedSize( const void* inPtr )
	{
		return ::malloc_size(inPtr);
	}
};

#pragma mark ===========================
#pragma mark [クラス]AEnvWrapper

//OSバージョン
static long	gOSVersion = 0;

/**
OS環境情報取得ラッパー
*/
class AEnvWrapper
{
  public:
	/**
	OSバージョン取得
	*/
	static AOSVersion	GetOSVersion()
	{
		/*#0 高速化
		long	response;
		::Gestalt(gestaltSystemVersion,&response);
		return response;
		*/
		if( gOSVersion == 0 )
		{
			//OSバージョン未初期化なら取得
			//#1034 ::Gestalt(gestaltSystemVersion,&gOSVersion);
			//#1079 gOSVersion = NSAppKitVersionNumber;
			if( [[NSProcessInfo processInfo] respondsToSelector:@selector(operatingSystemVersion)] ) 
			{
				NSOperatingSystemVersion	osver = [[NSProcessInfo processInfo] operatingSystemVersion];
				gOSVersion = osver.minorVersion;
			}
			else
			{
				if( floor(NSAppKitVersionNumber) >= NSAppKitVersionNumber10_12 )//#1190 #1262 #1263
				{
					gOSVersion = kOSVersion_MacOSX_10_12;
				}
				else if( floor(NSAppKitVersionNumber) >= NSAppKitVersionNumber10_11 )//#1190
				{
					gOSVersion = kOSVersion_MacOSX_10_11;
				}
				else if( NSAppKitVersionNumber >= NSAppKitVersionNumber10_10 )//#1190
				{
					gOSVersion = kOSVersion_MacOSX_10_10;
				}
				else if( NSAppKitVersionNumber >= NSAppKitVersionNumber10_9 )//#1190
				{
					gOSVersion = kOSVersion_MacOSX_10_9;
				}
				else if( NSAppKitVersionNumber >= NSAppKitVersionNumber10_8 )
				{
					gOSVersion = kOSVersion_MacOSX_10_8;
				}
				else if( NSAppKitVersionNumber >= NSAppKitVersionNumber10_7 )
				{
					gOSVersion = kOSVersion_MacOSX_10_7;
				}
				else if( NSAppKitVersionNumber >= NSAppKitVersionNumber10_6 )
				{
					gOSVersion = kOSVersion_MacOSX_10_6;
				}
				else if( NSAppKitVersionNumber >= NSAppKitVersionNumber10_5 )
				{
					gOSVersion = kOSVersion_MacOSX_10_5;
				}
				else 
				{
					gOSVersion = kOSVersion_MacOSX_10_4;
				}
			}
		}
		return gOSVersion;
	}
};

#endif

#if IMPLEMENTATION_FOR_WINDOWS

#pragma mark ===========================
#pragma mark Windows用

#include <stdio.h>

#pragma mark ===========================
#pragma mark [クラス]ACstringWrapper
/**
C文字列処理ラッパー
*/
class ACstringWrapper
{
  private:
	friend class AText;
	
	static AByteCount GetLength( AConstCharPtr inString )
	{
		for( AByteCount i = 0; i < 0x7FFFFFFF; i++ )
		{
			if( inString[i] == 0 )
			{
				return i;
			}
		}
		return 0x7FFFFFFF;
	}
	
	static AByteCount GetFormattedCstringLength( const AConstCharPtr inFormat, va_list inArgList )
	{
		// ★4095文字まで
		char	dummy[4096];
		vsnprintf_s(dummy,4095,_TRUNCATE,inFormat,inArgList);
		return GetLength(dummy);
	}
	
	static void	Sprintf( const ACharPtr ioTextPtr, const AByteCount inTextBufferSize, const AConstCharPtr inFormat, va_list inArgList )
	{
		vsnprintf_s(ioTextPtr,inTextBufferSize,_TRUNCATE,inFormat,inArgList);
	}
	
};

#pragma mark ===========================
#pragma mark [クラス]AMemoryWrapper
/**
メモリー管理ラッパー
*/
class AMemoryWrapper
{
  public:
	static void*	Malloc( AByteCount inByteCount )
	{
		return ::malloc(inByteCount);
	}
	
	static void	Free( void* inPtr )
	{
		::free(inPtr);
	}
	
	static void*	Realloc( void* inPtr, AByteCount inByteCount )
	{
		return ::realloc(inPtr,inByteCount);
	}
	
	static void	Memmove( void* inDst, const void* inSrc, AByteCount inByteCount )
	{
		::memmove(inDst,inSrc,inByteCount);
	}
	
	static ABool	Memcmp( const void* inA, const void* inB, const AByteCount inByteCount )
	{
		//memcmpでも良いかもしれない。（memcmpのロジックの中身が分からないので、速度的懸念から、自作にしている。）
		const unsigned char* ptra = reinterpret_cast<const unsigned char*>(inA);
		const unsigned char* ptrb = reinterpret_cast<const unsigned char*>(inB);
		for( AByteCount i = 0; i < inByteCount; i++ )
		{
			if( ptra[i] != ptrb[i] )   return false;
		}
		return true;
	}
	
	static void	Fill0( void* inPtr, AByteCount inByteCount )
	{
		unsigned char* ptr = reinterpret_cast<unsigned char*>(inPtr);
		for( AByteCount i = 0; i < inByteCount; i++ )
		{
			ptr[i] = 0;
		}
	}
	
	/**
	メモリ割り当てサイズ取得（padding含む）
	*/
	static AByteCount	AllocatedSize( const void* inPtr )
	{
		return ::_msize((void*)inPtr);
	}
};

#pragma mark ===========================
#pragma mark [クラス]AEnvWrapper
/**
OS環境情報取得ラッパー
*/
class AEnvWrapper
{
  public:
	/**
	OSバージョン取得
	*/
	static AOSVersion	GetOSVersion()
	{
		OSVERSIONINFO	osVer = {0};
		osVer.dwOSVersionInfoSize = sizeof(osVer);
		::GetVersionEx(&osVer);
		return ((osVer.dwMajorVersion)*0x100)+((osVer.dwMinorVersion)*0x10);
	}
};

#endif
