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

ABaseTypes
基本データ型定義

*/

#pragma once

#ifdef _WINDOWS
#include "../Main_Win/miedit.h"
#endif
#ifdef IMPLEMENTATION_FOR_WINDOWS
#include "Winsock2.h"
#include <objidl.h>
#endif
#import <Cocoa/Cocoa.h>

#pragma mark ===========================
#pragma mark ---基本型/const値定義

//ABool（ブール値）
typedef uint8_t ABool;//#695 Memmove等処理高速化のため bool→unsigned char

//ANumber（符号付き32ビット整数）
//#1034 typedef int32_t	ANumber;
typedef int64_t	ANumber;
//const値
//#1034 const ANumber	kNumber_MaxNumber =  0x7FFFFFFF;
//#1034 const ANumber	kNumber_MinNumber = -0x7FFFFFFF;//win 080618-0x80000000;
const ANumber	kNumber_MaxNumber =  0x7FFFFFFFFFFFFFFF;
const ANumber	kNumber_MinNumber = -0x7FFFFFFFFFFFFFFF;//win 080618-0x80000000;

//ANumber64bit（符号付き64ビット整数）#427
typedef int64_t ANumber64bit;

/*
//
#if IMPLEMENTATION_FOR_MACOSX
#define kInvalidMemoryPointer ((void*)(0xFFFFFFFF))
#else IMPLEMENTATION_FOR_WINDOWS
#define kInvalidMemoryPointer ((void*)(0xFFFFFFFF))
#endif
*/

typedef uint16_t AUInt16;
typedef uint8_t AUInt8;
typedef uint32_t AUInt32;

//AFloatNumber（浮動小数点数）
#if IMPLEMENTATION_FOR_MACOSX
typedef Float32	AFloatNumber;
#else 
typedef double	AFloatNumber;
#endif

//AUChar（符号無し1バイト文字・UTF-8の構成バイト）
typedef unsigned char AUChar;
//const値
const AUChar	kUChar_CR ='\r';
const AUChar	kUChar_LF = 10;
const AUChar	kUChar_LineEnd = kUChar_CR;
const AUChar	kUChar_Tab = '\t';
const AUChar	kUChar_Space = ' ';
const AUChar	kUChar_Slash = '/';
const AUChar	kUChar_Colon = ':';
const AUChar	kUChar_DOSEOF = 0x1A;
const AUChar	kUChar_Period = '.';
const AUChar	kUChar_DoubleQuotation = '\"';
const AUChar	kUChar_SingleQuotation = '\'';
const AUChar	kUChar_Backslash = '\\';
const AUChar	kUChar_Minus = '-';
const AUChar	kUChar_Comma = ',';
const AUChar	kUChar_Escape = 0x1B;
const AUChar	kUChar_Left = 0x1C;
const AUChar	kUChar_Right = 0x1D;
const AUChar	kUChar_Up = 0x1E;
const AUChar	kUChar_Down = 0x1F;
const AUChar	kUChar_Enter = 3;
const unsigned char A_CHAR_CR = 13;
const unsigned char A_CHAR_LF = 10;
const unsigned char A_CHAR_LineEnd = A_CHAR_CR;//Normalized LineEnd
const unsigned char A_CHAR_PATHDELIMITER = '/';
const unsigned char A_CHAR_SPACE = ' ';
const unsigned char A_CHAR_TAB = '\t';
const unsigned char A_CHAR_PERIOD = '.';
//ポインタ
typedef unsigned char* AUCharPtr;
typedef const unsigned char* AConstUCharPtr;

//AChar（C文字列の文字　C文字列（"xxxxx"）を扱うときのみ使用する。）
typedef char AChar;
//ポインタ
typedef const char* AConstCharPtr;
typedef const char* AConstCstringPtr;
typedef char* ACharPtr;
typedef char* ACstringPtr;

//UCS-4 #415
typedef AUInt32 AUCS4Char;

#pragma mark ---特定用途の型/const値定義

//バイト数
//[単位]AByteCount:byte
typedef size_t	AByteCount;

//配列型データの添字（インデックス）
//配列のデータ数がn個のとき0～n-1が有効な添字。ただし、それ以外の値を別の意味でとりうる。
typedef ANumber	AIndex;
//const値
const AIndex kIndex_Invalid = -1;

//配列の要素の個数（マイナスの値をとりうる。たとえば、追加個数がマイナス＝減らすという意味に使う）
//[単位]AItemCount:item
//[単位]byte = item * sizeof()
typedef ANumber	AItemCount;

/*#693
//ObjectID（AObjectArray<>の要素を一意に指定する不変のID）
//#216 typedef ANumber AObjectID;
//#216 const AObjectID	kObjectID_Invalid = -1;
struct AObjectID
{
	ANumber	val;
	bool operator==(AObjectID inObjectID) const { return (val==inObjectID.val); }
	bool operator!=(AObjectID inObjectID) const { return (val!=inObjectID.val); }
};
const AObjectID	kObjectID_Invalid = {-1};
*/

//ADataBaseにおいてデータを指定するためのID
typedef ANumber	ADataID;
const ADataID kDataID_Invalid = -1;

#pragma mark ===========================
#if IMPLEMENTATION_FOR_MACOSX
#pragma mark ---MacOSXデータ型/const値定義

//#380 typedef unsigned long ADateTime;
typedef CFAbsoluteTime ADateTime;

//AColor
typedef RGBColor	AColor;
const AColor kColor_Black 			= {0,0,0};
const AColor kColor_White 			= {65535,65535,65535};
const AColor kColor_Red 			= {65535,0,0};
const AColor kColor_Green 			= {0,65535,0};
const AColor kColor_DarkGreen 		= {0,30000,0};
const AColor kColor_DarkGreen2 		= {0,50000,0};
const AColor kColor_Blue 			= {0,0,65535};
const AColor kColor_DarkBlue 		= {0,0,50000};
const AColor kColor_LightBlue 		= {0xAD*0xAD,0xD8*0xD8,0xE6*0xE6};
const AColor kColor_LightPink 		= {0xFF*0xFF,0xB6*0xB6,0xC1*0xC1};
const AColor kColor_GreenYellow		= {0xAD*0xAD,0xFF*0xFF,0x2F*0x2F};
const AColor kColor_Pink			= {65535,0,50000};
const AColor kColor_Yellow 			= {65535,65535,0};
const AColor kColor_DeepYellow		= {65535,0xF0*0xF0,0};
const AColor kColor_Gold 			= {65535,0xD7*0xD7,0};
const AColor kColor_DarkGray		= {15000,15000,15000};
const AColor kColor_Gray 			= {30000,30000,30000};
const AColor kColor_LightGray 		= {50000,50000,50000};
const AColor kColor_Gray10Percent	= {6554,6554,6554};
const AColor kColor_Gray20Percent	= {13107,13107,13107};
const AColor kColor_Gray30Percent	= {19661,19661,19661};
const AColor kColor_Gray40Percent	= {26214,26214,26214};
const AColor kColor_Gray50Percent	= {32768,32768,32768};
const AColor kColor_Gray60Percent	= {39322,39322,39322};
const AColor kColor_Gray70Percent	= {45875,45875,45875};
const AColor kColor_Gray75Percent	= {49151,49151,49151};
const AColor kColor_Gray80Percent	= {52429,52429,52429};
const AColor kColor_Gray85Percent	= {55705,55705,55705};
const AColor kColor_Gray87Percent	= {57015,57015,57015};
const AColor kColor_Gray90Percent	= {58982,58982,58982};
const AColor kColor_Gray91Percent	= {59637,59637,59637};
const AColor kColor_Gray92Percent	= {60293,60293,60293};
const AColor kColor_Gray93Percent	= {60949,60949,60949};
const AColor kColor_Gray94Percent	= {61604,61604,61604};
const AColor kColor_Gray95Percent	= {62259,62259,62259};
const AColor kColor_Gray97Percent	= {63570,63570,63570};
const AColor kColor_Gray98Percent	= {64224,64224,64224};
const AColor kColor_Gray99Percent	= {64879,64879,64879};
const AColor kColor_Bisque			= {0xFF*0xFF,0xE4*0xE4,0xC4*0xC4};
const AColor kColor_LightYellow		= {0xFF*0xFF,0xFF*0xFF,0xE0*0xE0};
const AColor kColor_Khaki			= {0xF0*0xF0,0xE6*0xE6,0x8C*0x8C};
const AColor kColor_LightGreen		= {0x90*0x90,0xEE*0xEE,0x90*0x90};
const AColor kColor_Moccasin		= {0xFF*0xFF,0xE4*0xE4,0xB5*0xB5};
const AColor kColor_Honeydew		= {0xF0*0xF0,0xFF*0xFF,0xF0*0xF0};
const AColor kColor_Orange			= {0xFF*0xFF,0xA5*0xA5,0x00*0x00};
const AColor kColor_Lavender		= {0xE6*0xE6,0xE6*0xE6,0xFA*0xFA};

#elif IMPLEMENTATION_FOR_WINDOWS

typedef HWND	AWindowRef;
typedef COLORREF	AColor;

const AColor kColor_Black 			= RGB(0,0,0);
const AColor kColor_White 			= RGB(255,255,255);
const AColor kColor_Red 			= RGB(255,0,0);
const AColor kColor_Green 			= RGB(0,255,0);
const AColor kColor_DarkGreen 		= RGB(0,117,0);
const AColor kColor_DarkGreen2 		= RGB(0,195,0);
const AColor kColor_Blue 			= RGB(0,0,255);
const AColor kColor_DarkBlue		= RGB(0,0,100);
const AColor kColor_LightBlue 		= RGB(0xAD,0xD8,0xE6);
const AColor kColor_LightPink 		= RGB(0xFF,0xB6,0xC1);
const AColor kColor_GreenYellow		= RGB(0xAD,0xFF,0x2F);
const AColor kColor_Yellow 			= RGB(0xFF,0xFF,0);
const AColor kColor_DeepYellow		= RGB(0xFF,0xF0,0);
const AColor kColor_Gold 			= RGB(0xFF,0x80,0);
const AColor kColor_DarkGray		= RGB(59,59,59);
const AColor kColor_Gray 			= RGB(117,117,117);
const AColor kColor_LightGray 		= RGB(195,195,195);
const AColor kColor_Gray10Percent	= RGB(25,25,25);
const AColor kColor_Gray20Percent	= RGB(51,51,51);
const AColor kColor_Gray30Percent	= RGB(77,77,77);
const AColor kColor_Gray40Percent	= RGB(102,102,102);
const AColor kColor_Gray50Percent	= RGB(127,127,127);
const AColor kColor_Gray60Percent	= RGB(153,153,153);
const AColor kColor_Gray70Percent	= RGB(179,179,179);
const AColor kColor_Gray80Percent	= RGB(204,204,204);
const AColor kColor_Gray85Percent	= RGB(221,221,221);
const AColor kColor_Gray90Percent	= RGB(238,238,238);
const AColor kColor_Gray91Percent	= RGB(240,240,240);
const AColor kColor_Gray92Percent	= RGB(242,242,242);
const AColor kColor_Gray93Percent	= RGB(244,244,244);
const AColor kColor_Gray94Percent	= RGB(246,246,246);
const AColor kColor_Gray95Percent	= RGB(248,248,248);
const AColor kColor_Gray97Percent	= RGB(252,252,252);
const AColor kColor_Pink			= RGB(255,0,200);
const AColor kColor_Bisque			= RGB(0xFF,0xE4,0xC4);
const AColor kColor_LightYellow		= RGB(0xFF,0xFF,0xE0);
const AColor kColor_Khaki			= RGB(0xF0,0xE6,0x8C);
const AColor kColor_LightGreen		= RGB(0x90,0xEE,0x90);
const AColor kColor_Moccasin		= RGB(0xFF,0xE4,0xB5);
const AColor kColor_Honeydew		= RGB(0xF0,0xFF,0xF0);
const AColor kColor_Orange			= RGB(0xFF,0xA5,0x00);
const AColor kColor_Lavender		= RGB(0xE6,0xE6,0xFA);

#endif


//#688 typedef unsigned short AModeID;
typedef AIndex AModeID;//#688
typedef ANumber	AControlID;
typedef ANumber APrefID;
//#688 typedef int ATabID;

typedef ANumber AModeIndex;
const AModeIndex kModeIndex_StandardMode = 0;

const AControlID	kControlID_Invalid = -1;
const AControlID	kControlID_TabControl = 65534;//#353

typedef AIndex	AIconID;
const AIconID	kIconID_NoIcon = 0;//B0000 101;

//#688
/**
ImageID
*/
typedef AIndex	AImageID;
const AImageID	kImageID_Invalid = -1;

//
#if IMPLEMENTATION_FOR_MACOSX

//#1275 typedef WindowRef	AWindowRef;
typedef NSInteger	AWindowRef;//#1275

//#688 typedef ANumber ALineNumber;

//#688 typedef short	AFontSize;
//#688 typedef short	ATabWidth;

//#1034 typedef MenuRef	ACarbonMenuRef;//#688
//#688 typedef MenuItemIndex AMenuItemIndex;
//#688 typedef UInt16 AMenuItemCount;
typedef void*	AMenuRef;//#688

//#688 typedef short	AFontNumber;//win
//#688 typedef short	AFontSize;
//#688 typedef UInt32 AMenuItemID;
typedef uint32_t	AMenuItemID;//#688
const AMenuItemID	kMenuItemID_Invalid = -1;//#232
const AMenuItemID	kMenuItemID_Close						= 1010;//'clos';
const AMenuItemID	kMenuItemID_Quit 						= 1011;//'quit';//#182
const AMenuItemID	kMenuItemID_HideThisApplication			= 1012;
const AMenuItemID	kMenuItemID_HideOtherApplications		= 1013;
const AMenuItemID	kMenuItemID_ShowAllApplications			= 1014;
//#688 typedef DataBrowserPropertyID ALinkViewColumnID;
const AMenuItemID	kMenuItemID_CheckForUpdates				= 1015;//#1102

typedef UniChar AUTF16Char;
typedef UniChar*	AUTF16CharPtr;

//#688 typedef HIViewRef AControlRef;

//#688 typedef CFStringRef ALocalizedTextKey;

//#1040 typedef TextEncoding ATextEncoding;
typedef CFStringEncoding ATextEncoding;

typedef in_addr_t AIPAddress;
typedef in_port_t APortNumber;
typedef int ASocketDescriptor;
const ASocketDescriptor	kSocketDescriptor_Invalid = -1;

struct AFileAttribute
{
	OSType	type;
	OSType	creator;
};


typedef UInt32	AModifierKeys;
//#688 typedef UInt8	AMenuShortcutModifierKeys;
typedef uint32_t	AMenuShortcutModifierKeys;
/*#688 Mac/Win共通箇所に移動
const AModifierKeys	kModifierKeysMask_None	 	= 0;
const AModifierKeys	kModifierKeysMask_Shift 	= 1;
const AModifierKeys	kModifierKeysMask_Control 	= 2;
const AModifierKeys	kModifierKeysMask_Option 	= 4;
const AModifierKeys	kModifierKeysMask_Command 	= 8;
*/

/*#688
typedef EventRef	AOSKeyEvent;
const AOSKeyEvent	kOSKeyEventNull = NULL;//UnitTest用
*/

//#688
const int16_t kNSEvent_InternalEvent = 1;

//#688 typedef ScrapRef	AScrapRef;
typedef void*	AScrapRef;//#688
//#688 typedef DragRef	ADragRef;
typedef void*	ADragRef;//#688
//#688 typedef ScrapFlavorType AScrapType;
typedef AIndex AScrapType;//#688

//#688 AScrapWrapper.hから移動
const AScrapType	kScrapType_UnicodeText = 0;//#688 'utxt';
//#688 const AScrapType	kScrapType_Text = 1;//#688 'TEXT';
const AScrapType	kScrapType_File = 2;//#688 'hfs ';
const AScrapType	kScrapType_URL = 3;//#688

//#1034 typedef OSErr	AOSError;
enum AFileError 
{
	kFileError_General = 0,
	kFileError_DiskFull
};

const OSType kOSTypeNULL = 0;

//OSバージョン
/*#1034
typedef long	AOSVersion;
const AOSVersion	kOSVersion_MacOSX_10_7 = 0x1070;
const AOSVersion	kOSVersion_MacOSX_10_6 = 0x1060;
const AOSVersion	kOSVersion_MacOSX_10_5 = 0x1050;
const AOSVersion	kOSVersion_MacOSX_10_4 = 0x1040;
*/
typedef int32_t AOSVersion;
const AOSVersion	kOSVersion_MacOSX_10_12 = 12;//#1190
const AOSVersion	kOSVersion_MacOSX_10_11 = 11;//#1190
const AOSVersion	kOSVersion_MacOSX_10_10 = 10;
const AOSVersion	kOSVersion_MacOSX_10_9 = 9;
const AOSVersion	kOSVersion_MacOSX_10_8 = 8;
const AOSVersion	kOSVersion_MacOSX_10_7 = 7;
const AOSVersion	kOSVersion_MacOSX_10_6 = 6;
const AOSVersion	kOSVersion_MacOSX_10_5 = 5;
const AOSVersion	kOSVersion_MacOSX_10_4 = 4;

const AMenuItemID	kMenuItemID_Undo 						= 1001;//'Undo';
const AMenuItemID	kMenuItemID_Redo 						= 1002;//'Redo';
const AMenuItemID	kCommandID_UpdateCommandStatus			= 1003;//'UdCS';
const AMenuItemID	kMenuItemID_Copy 						= 1004;//'Copy';
const AMenuItemID	kMenuItemID_Cut 						= 1005;//'Cut ';
const AMenuItemID	kMenuItemID_Paste 						= 1006;//'Past';
const AMenuItemID	kMenuItemID_SelectAll					= 1007;//'SAll';
const AMenuItemID	kMenuItemID_Clear						= 1008;//'Cler';
const AMenuItemID	kMenuItemID_InputBackslashYen			= 1009;//'BYen';

//#1034 typedef NavDialogRef	AFileOpenDialogRef;//R0198


#elif IMPLEMENTATION_FOR_WINDOWS

typedef __time64_t ADateTime;

typedef HWND	AWindowRef;
typedef wchar_t AUTF16Char;
typedef wchar_t*	AUTF16CharPtr;
typedef UINT ATextEncoding;
typedef DWORD	AOSError;
struct AFileAttribute
{
	int	tmp;
};
//#688 typedef short	AFontSize;
typedef HMENU	AMenuRef;
typedef AIndex AMenuItemIndex;

typedef UINT AMenuItemID;
const AMenuItemID	kMenuItemID_Invalid = -1;//#232
const AMenuItemID	kMenuItemID_Close 						= IDM_CLOSE;
const AMenuItemID	kMenuItemID_Quit 						= IDM_Quit;//#182

typedef wchar_t	AUTF16Char;
typedef wchar_t*	AUTF16CharPtr;

typedef WORD	AModifierKeys;
/*#688 Mac/Win共通箇所に移動
const WORD	kModifierKeysMask_Shift 	= 1;
const WORD	kModifierKeysMask_Control 	= 2;
const WORD	kModifierKeysMask_Option 	= 4;
const WORD	kModifierKeysMask_Command 	= 8;
*/
//Macの場合はメニューに直接SetMenuItemModifiers()でショートカットを設定するための引数としてAModifierKeysと分けていたが、
//Windowsの場合は、メニューにショートカット設定できない（メニューは表示のみ・AcceralatorかWM_KEYDOWでソフト処理する必要がある）ので、
//AModifierKeysと同じにする。
/*#688
typedef AModifierKeys	AMenuShortcutModifierKeys;
const WORD	kMenuShoftcutModifierKeysMask_Shift 	= 1;
const WORD	kMenuShoftcutModifierKeysMask_Control 	= 2;
const WORD	kMenuShoftcutModifierKeysMask_Alt	 	= 8;
*/
//
struct AOSKeyEvent
{
	UINT	message;
	WPARAM	wParam;
	LPARAM	lParam;
};

//
typedef UINT	AScrapType;

//OS Version
typedef long	AOSVersion;
const AOSVersion	kOSVersion_Windows2000 			= 0x0500;//Windows2000
const AOSVersion	kOSVersion_WindowsXP 			= 0x0510;//WindowsXP
const AOSVersion	kOSVersion_WindowsServer2003	= 0x0520;//WindowsXP64bit, Server2003, Server2003R2
const AOSVersion	kOSVersion_WindowsVista		 	= 0x0600;//WindowsVista, Server2008
const AOSVersion	kOSVersion_Windows7				= 0x0610;//Windows7, Server2008R2

//★型確認
typedef UINT	AFileOpenDialogRef;

typedef IDataObject* ADragRef;

//#688 const UINT	kTickMsec = 100;

typedef u_long	AIPAddress;
typedef unsigned short APortNumber;
typedef SOCKET ASocketDescriptor;
const ASocketDescriptor	kSocketDescriptor_Invalid = -1;

//ユーザー定義メッセージ
//+32からはじめる
//#688 直接実行に変更 #define	WM_APP_UPDATEMENU (WM_APP+32)
#define	WM_APP_INTERNALEVENT (WM_APP+33)
#define	WM_APP_MOUSEWHEEL (WM_APP+34)
#define WM_APP_DROPFROMANOTHER (WM_APP+35)

const AMenuItemID	kMenuItemID_Undo 						= IDM_Undo;
const AMenuItemID	kMenuItemID_Redo 						= IDM_Redo;
const AMenuItemID	kMenuItemID_Copy 						= IDM_Copy;
const AMenuItemID	kMenuItemID_Cut 						= IDM_Cut;
const AMenuItemID	kMenuItemID_Paste 						= IDM_Paste;
const AMenuItemID	kMenuItemID_SelectAll					= IDM_SelectAll;
const AMenuItemID	kMenuItemID_Clear						= IDM_Clear;
const AMenuItemID	kMenuItemID_InputBackslashYen			= IDM_InputBackslashYen;

const int	kPathMaxLength = 4096;

#endif

//#232 typedef ANumber AContextMenuID;

class APoint
{
  public:
	ANumber x;
	ANumber y;
};

const APoint kPoint_00 = {0,0};

class ARect
{
  public:
	ANumber left;
	ANumber top;
	ANumber right;
	ANumber bottom;
};

enum AReturnCode 
{
	returnCode_CR = 0,
	returnCode_CRLF,
	returnCode_LF
};

//#189 const ANumber	kLaunguage_Japanese = 0;
//#305
enum ALanguage
{
	kLanguage_Japanese = 0,//ja
	kLanguage_English = 1,//en
	kLanguage_French = 2,//fr
	kLanguage_German = 3,//de
	kLanguage_Spanish = 4,//es
	kLanguage_Italian = 5,//it
	kLanguage_Dutch = 6,//nl
	kLanguage_Swedish = 7,//sv
	kLanguage_Norwegian = 8,//nb
	kLanguage_Danish = 9,//da
	kLanguage_Finnish = 10,//fi
	kLanguage_Portuguese = 11,//pt
	kLanguage_SimplifiedChinese = 12,//zh_CN
	kLanguage_TraditionalChinese = 13,//zh_TW
	kLanguage_Korean = 14,//ko
	kLanguage_Polish = 15,//pl
	kLanguage_PortuguesePortugal = 16,//pt_PT
	kLanguage_Russian = 17,//ru
	kLanguage_Count = 18
};

//ハッシュ配列用定義
const AIndex kIndex_HashNoData = -2;
const AIndex kIndex_HashDeleted = -1;
const AItemCount kItemCount_HashMaxSize = 0x7FFFFFFF;

typedef ANumber	AInternalEventType;

const AInternalEventType	kInternalEvent_NOP = 0;
const AInternalEventType	kInternalEvent_TraceLog = 1;
const AInternalEventType	kInternalEvent_Quit				= 2;//win


//R0195
typedef ANumber ATextStyle;
const ATextStyle	kTextStyle_Normal = 0;
const ATextStyle	kTextStyle_Bold = 1;//win
const ATextStyle	kTextStyle_Underline = 2;//win
const ATextStyle	kTextStyle_Italic = 4;//win
const ATextStyle	kTextStyle_DropShadow = 8;//win

#define hex2asc(x) (((x)>=0xa)?((x)-0xa+'A'):((x)+'0'))

//ATypes.hから移動
struct ATextPoint
{
	AIndex	x;
	AIndex	y;
};
const ATextPoint	kTextPoint_00 = {0,0};//#699
const ATextPoint	kTextPoint_Invalid = {kIndex_Invalid,kIndex_Invalid};//#699

typedef AIndex	ATextIndex;

//ATypes.hから移動
#define Macro_IsAlphabet(ch) (( (ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_' ))

//AChildProcessManager.hから移動
#if IMPLEMENTATION_FOR_MACOSX
typedef pid_t	APID;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
typedef DWORD APID;
#endif

//統計データ #271
//AHashArray
extern AItemCount	gStatics_AHashArray_MakeHashCount;
extern AItemCount	gStatics_AHashArray_FindCount;
extern AItemCount	gStatics_AHashArray_FirstDataNull;
extern AItemCount	gStatics_AHashArray_FirstDataHit;
extern AItemCount	gStatics_AHashArray_CompareCount;
//AHashObjectArray
extern AItemCount	gStatics_AHashObjectArray_MakeHashCount;
//AObjectArrayItem
extern AItemCount	gStatics_AObjectArrayItem_MakeHashCount;
extern AItemCount	gStatics_AObjectArrayItem_FindCount;
extern AItemCount	gStatics_AObjectArrayItem_FirstDataNull;
extern AItemCount	gStatics_AObjectArrayItem_FirstDataHit;
extern AItemCount	gStatics_AObjectArrayItem_CompareCount;

//#693
/**
AUniqID
AUniqIDArrayによって生成される一意ID
*/
struct AUniqID
{
	ANumber	val;
	bool operator==(AUniqID inUniqID) const { return (val==inUniqID.val); }
	bool operator!=(AUniqID inUniqID) const { return (val!=inUniqID.val); }
};
const AUniqID	kUniqID_Invalid = {-1};

//#688
//291ms/5 (58.2ms) に設定
const double	kTickMsec = 291.0/5;

//#361
/**
パイプ
*/
typedef int APipe;

//#724
/**
AGradientType
*/
enum AGradientType
{
	kGradientType_None,
	kGradientType_Horizontal,
	kGradientType_Vertical,
};

//#688
const AModifierKeys	kModifierKeysMask_None	 	= 0;
const AModifierKeys	kModifierKeysMask_Control 	= 1;
const AModifierKeys	kModifierKeysMask_Option 	= 2;
const AModifierKeys	kModifierKeysMask_Command 	= 4;
const AModifierKeys	kModifierKeysMask_Shift 	= 8;

//#1004
const AItemCount kMenuTextMax = 4096;

//#1031
typedef ANumber AScrollTrigger;
const AScrollTrigger	kScrollTrigger_General = 0;
const AScrollTrigger	kScrollTrigger_ScrollBar = 1;
const AScrollTrigger	kScrollTrigger_Wheel = 2;
const AScrollTrigger	kScrollTrigger_ScrollKey = 3;


