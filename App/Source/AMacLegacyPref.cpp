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

AMacLegacyPref

*/

#include "AMacLegacyPref.h"
#include "AAppPrefDB.h"
#include "AModePrefDB.h"
#include "ADocPrefDB.h"
//#1034 #include "AApp.h"

//#1034
//バイトにシビアな処理が多いので、64bit対応はせず、LoadTool_LegacyShortcutData()以外は別アプリ（modeprefconverter）で実行する。
//modeprefconverterとはソースを共有する。modeprefconverterのみMODEPREFCONVERTERwo#defineしている。

//#1034
//modeprefconverter用処理
#ifdef MODEPREFCONVERTER
enum EPrefVersion {
	prefVersion_Less20b5 = 0,
	prefVersion_20b6,
	prefVersion_20b7,
	prefVersion_20b9,
	prefVersion_20b10,
	prefVersion_21b1,
	prefVersion_21b3,
	prefVersion_21b5,
	prefVersion_21b8,
	prefVersion_21b9,
	prefVersion_21b13,
	prefVersion_213,//B0034
	prefVersion_215, //R0043
	prefVersion_216b1,
	prefVersion_216b3,
	prefVersion_216b5,
	prefVersion_217b1,
	prefVersion_217b6
};

EPrefVersion	GetPrefVersion( short inRRefNum );
void	WritePrefVersion( short inRRefNum );
Handle	Get1Resource( short inRRefNum, ResType inResType, short inID );
void	CopyPstring( Str255 dst, const Str255 src );
ABool	ComparePstring( const Str255 a, const Str255 b );

//AppPrefの移行はバージョン3では対応しない
#if 0
#pragma mark ---AppPref

//旧MacOSリソースタイプのデータ読み込み
void	AAppPrefDB::LoadMacLegacy()
{
	AFileAcc	preffile;
	AFileWrapper::GetAppPrefFile(preffile);
	FSRef	ref;
	preffile.GetFSRef(ref);
	short	rRefNum = ::FSOpenResFile(&ref,fsRdPerm);
	if( rRefNum != -1 )
	{
		LoadMacLegacy_LegacyPref(rRefNum);
		LoadMacLegacy_MultiFileFind_RecentlyUsedFolder(rRefNum);
		//#259 LoadMacLegacy_RecentlyOpenedFile(rRefNum);
		LoadMacLegacy_FolderList(rRefNum);
		LoadMacLegacy_FTPFolder(rRefNum);
		LoadMacLegacy_LaunchApp(rRefNum);
		LoadMacLegacy_FTPServer(rRefNum);
		LoadMacLegacy_FindMemory(rRefNum);
		
		::CloseResFile(rRefNum);
	}
	LoadMacLegacy_TextEncoding();
	LoadMacLegacy_Kinsoku();
}

void	AAppPrefDB::LoadMacLegacy_LegacyPref( short inResRefNum )
{
	SMacLegacyAppPref	pref;
	
	ABool	exist = false;
	try
	{
		//初期値設定
		pref.CMMouseButton = true;
		pref.CMMouseButtonTick = 60;
		pref.OBSOLETE_browser = 'MSIE';
		pref.OBSOLETE_browserEventClass = 'GURL';
		pref.OBSOLETE_browserEventID = 'GURL';
		pref.OBSOLETE_appearance = appearance_normal;
		pref.OBSOLETE_useOpenRecent = true;
		pref.printColored = false;
		RGBColor	color;
		color.red = color.green = color.blue = 0;
		pref.indexWinNormalColor = color;
		color.red = color.green = color.blue = 65535;
		pref.indexWinBackColor = color;
		color.green = color.blue = 0;
		pref.indexWinCommentColor = color;
		pref.indexWinGroupColor = color;
		color.red = color.green = 0;
		color.blue = 65535;
		pref.indexWinURLColor = color;
		pref.OBSOLETE_useNS = true;
		pref.OBSOLETE_undoLevel = 0;
		pref.bunsetsuSelect = true;
		pref.windowShortcut = false;
		pref.cmdNModeName[0] = 0;
		pref.OBSOLETE_useInternetConfigBrowser = true;
		pref.useShellScript = true;
		pref.stdOutModeName[0] = 0;
		pref.lockWhileEdit = true;
		pref.printBlackForWhite = false;
		pref.OBSOLETE_OSXDock = OSXDock_Bottom;
		pref.indexWindowFont = 0;//#1012 ::GetAppFont();
		pref.indexWindowFontSize = 9;
		pref.indexWindowWidth = 500;
		pref.indexWindowHeight = 300;
		pref.indexWindowSeparateLine = 40;
		pref.inlineInputColorLine = false;
		pref.findDefaultIgnoreCase = true;
		pref.findDefaultWholeWord = false;
		pref.findDefaultAimai = false;
		pref.findDefaultRegExp = false;
		pref.findDefaultLoop = false;
		pref.findDefaultAllowReturn = false;
		pref.findDefaultRecursive = true;
		pref.findDefaultDisplayPosition = false;
		pref.findFontSizeNum = 4;//B0398 10;
		pref.findDialogPosition.h = -1;
		pref.findDialogPosition.v = -1;
		pref.multiFileFindDialogPosition.h = -1;
		pref.multiFileFindDialogPosition.v = -1;
		CopyPstring(pref.findDefaultFileFilter,"\p.+");
		pref.wheelScrollPercent = 20;//B0091
		pref.wheelScrollPercentCmd = 100;//B0091
		pref.alphaWindowMode = false;
		pref.alphaWindowPercent1 = 85;
		pref.alphaWindowPercent2 = 50;
		pref.OBSOLETE_alphaWindowPercent3 = 100;
		pref.OBSOLETE_alphaWindowPercent4 = 100;
		pref.OBSOLETE_alphaWindowPercent5 = 100;
		pref.singleWindowMode = false;
		pref.OBSOLETE_openNewWindow = false;//B0233
		CopyPstring(pref.OBSOLETE_openNewWindowModeName,"\p");//B0233
		pref.jumpListOffsetX = 16;//R0047
		pref.jumpListOffsetY = 32;//R0047
		pref.jumpListAlphaPercent = 90;//R0047 R0100
		pref.OBSOLETE_jumpListFontNumDummy = 0;//R0047
		pref.OBSOLETE_jumpListFontSizeDummy = 9;//R0047
		pref.jumpListSizeX = 250;//R0047
		pref.jumpListSizeY = 500;//R0047
		pref.displayWindowList = false;//R0003-2
		pref.windowListOffsetX = 100;//R0072 -150;//R0003
		pref.windowListOffsetY = 100;//R0072 320;//R0003
		pref.windowListAlphaPercent = 90;//R0003 R0100
		pref.OBSOLETE_windowListFontNumDummy = 0;//R0003
		pref.OBSOLETE_windowListFontSizeDummy = 9;//R0003
		pref.windowListSizeX = 150;//R0003
		pref.windowListSizeY = 300;//R0003
		pref.OBSOLETE_windowListColor.red = pref.OBSOLETE_windowListColor.green = pref.OBSOLETE_windowListColor.blue = 65535;//R0003
		pref.OBSOLETE_jumpListColor.red = pref.OBSOLETE_jumpListColor.green = pref.OBSOLETE_jumpListColor.blue = 65535;//R0047
		pref.OBSOLETE_windowListTextColor.red = pref.OBSOLETE_windowListTextColor.green = pref.OBSOLETE_windowListTextColor.blue = 65535;//R0003
		pref.OBSOLETE_jumpListTextColor.red = pref.OBSOLETE_jumpListTextColor.green = pref.OBSOLETE_jumpListTextColor.blue = 65535;//R0047
		pref.OBSOLETE_jumpListFixed = false;//R0066
		pref.OBSOLETE_windowListFixed = false;//R0066
		pref.jumpListPosition = windowPosition_Right;//R0003-2
		pref.OBSOLETE_windowListPosition = windowPosition_Right;//R0003-2
		pref.singleWindowBounds.left = 100;//R0077
		pref.singleWindowBounds.right = 400;//R0077
		pref.singleWindowBounds.top = 50;//R0077
		pref.singleWindowBounds.bottom = 450;//R0077
		pref.windowListMode = windowListMode_RecentFile;//R0079
		
		//R-ub
		LoadMacLegacy_ConvertEndianToBig(pref);//まずいったんBigEndianにする
		
		//各バージョンに従ったサイズ分を読み込み
		EPrefVersion	ver = GetPrefVersion(inResRefNum);
		switch(ver) 
		{
		  case prefVersion_217b6:
		  case prefVersion_217b1:
		  case prefVersion_216b5:
			{
				SAppPref217b1	**res = (SAppPref217b1**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) 
				{
					exist = true;
					/*long	a = sizeof(SAppPref217b1);
					long	b = sizeof(pref);*/
					for( long i = 0; i < sizeof(SAppPref217b1); i++ ) 
					{
						*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		  case prefVersion_216b3://R0061
			{
				SAppPref216b3	**res = (SAppPref216b3**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) 
				{
					exist = true;
					if( GetHandleSize((Handle)res) == sizeof(SAppPref216b3) ) 
					{
						for( long i = 0; i < sizeof(SAppPref216b3); i++ ) 
						{
							*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
						}
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		  case prefVersion_216b1://B0091
			{
				SAppPref216b1	**res = (SAppPref216b1**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) 
				{
					exist = true;
					if( GetHandleSize((Handle)res) == sizeof(SAppPref216b1) ) 
					{
						for( long i = 0; i < sizeof(SAppPref216b1); i++ ) 
						{
							*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
						}
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		  case prefVersion_215://R0043
		  case prefVersion_213://B0034
		  case prefVersion_21b13:
			{
				SAppPref21b13	**res = (SAppPref21b13**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) 
				{
					exist = true;
					if( GetHandleSize((Handle)res) == sizeof(SAppPref21b13) ) 
					{
						for( long i = 0; i < sizeof(SAppPref21b13); i++ ) 
						{
							*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
						}
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		  case prefVersion_21b9:
			{
				SAppPref21b9	**res = (SAppPref21b9**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) 
				{
					exist = true;
					if( GetHandleSize((Handle)res) == sizeof(SAppPref21b9) ) 
					{
						for( long i = 0; i < sizeof(SAppPref21b9); i++ ) 
						{
							*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
						}
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		  case prefVersion_21b8:
			{
				SAppPref21b8	**res = (SAppPref21b8**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) 
				{
					exist = true;
					if( GetHandleSize((Handle)res) == sizeof(SAppPref21b8) ) 
					{
						for( long i = 0; i < sizeof(SAppPref21b8); i++ ) 
						{
							*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
						}
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		  case prefVersion_21b5:
		  case prefVersion_21b3:
		  case prefVersion_21b1:
			{
				SAppPref21b5	**res = (SAppPref21b5**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) {
					exist = true;
					if( GetHandleSize((Handle)res) == sizeof(SAppPref21b5) ) {
						for( long i = 0; i < sizeof(SAppPref21b5); i++ ) {
							*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
						}
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		  case prefVersion_20b9:
			{
				SAppPref20b9	**res = (SAppPref20b9**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) {
					exist = true;
					if( GetHandleSize((Handle)res) == sizeof(SAppPref20b9) ) {
						for( long i = 0; i < sizeof(SAppPref20b9); i++ ) {
							*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
						}
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		  case prefVersion_20b7:
			{
				SAppPref20b7	**res = (SAppPref20b7**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) {
					exist = true;
					if( GetHandleSize((Handle)res) == sizeof(SAppPref20b7) ) {
						for( long i = 0; i < sizeof(SAppPref20b7); i++ ) {
							*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
						}
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		  case prefVersion_20b6:
			{
				SAppPref20b6	**res = (SAppPref20b6**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) {
					exist = true;
					if( GetHandleSize((Handle)res) == sizeof(SAppPref20b6) ) {
						for( long i = 0; i < sizeof(SAppPref20b6); i++ ) {
							*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
						}
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		  case prefVersion_Less20b5:
			{
				SAppPref20b5	**res = (SAppPref20b5**)Get1Resource(inResRefNum,'MMKP',1000);
				if( res != NULL ) {
					exist = true;
					if( GetHandleSize((Handle)res) == sizeof(SAppPref20b5) ) {
						for( long i = 0; i < sizeof(SAppPref20b5); i++ ) {
							*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
						}
					}
					ReleaseResource((Handle)res);
				}
				break;
			}
		}
		
		//R-ub
		LoadMacLegacy_ConvertEndianToHost(pref);
		
		//R-nib
		AFloatNumber	fontsize = 12;//B0398
		switch(pref.findFontSizeNum)
		{
		  case 1:
			fontsize = 9;
			break;
		  case 2:
			fontsize = 10;
			break;
		  case 3:
			fontsize = 12;
			break;
		  case 4:
			fontsize = 14;
			break;
		}
		SetData_FloatNumber(kFind_FontSize,fontsize);
		if( exist )
		{
			//APrefデータベースへのSetData
			AText	text;
			APoint	point;
			ARect	rect;
			ANumber	num;
			SetData_Bool(kCMMouseButton,				pref.CMMouseButton);
			SetData_Number(kCMMouseButtonTick,			pref.CMMouseButtonTick);
			SetData_Color(kIndexWinNormalColor,			pref.indexWinNormalColor);
			SetData_Color(kIndexWinBackColor,			pref.indexWinBackColor);
			SetData_Color(kIndexWinCommentColor,		pref.indexWinCommentColor);
			SetData_Color(kIndexWinURLColor,			pref.indexWinURLColor);
			SetData_Color(kIndexWinGroupColor,			pref.indexWinGroupColor);
			SetData_Bool(kPrintColored,					pref.printColored);
			SetData_Bool(kBunsetsuSelect,				pref.bunsetsuSelect);
			SetData_Bool(kWindowShortcut,				pref.windowShortcut);
			text.SetPstring(pref.cmdNModeName);
			text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			SetData_Text(kCmdNModeName,					text);
			SetData_Bool(kUseShellScript,				pref.useShellScript);
			text.SetPstring(pref.stdOutModeName);
			text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			SetData_Text(kStdOutModeName,				text);
			SetData_Bool(kLockWhileEdit,				pref.lockWhileEdit);
			SetData_Bool(kPrintBlackForWhite,			pref.printBlackForWhite);
			SetData_FloatNumber(kIndexWindowFontSize,	pref.indexWindowFontSize);
			SetData_Number(kIndexWindowWidth,			pref.indexWindowWidth);
			SetData_Number(kIndexWindowHeight,			pref.indexWindowHeight);
			SetData_Number(kIndexWindowSeparateLine,	pref.indexWindowSeparateLine);
			SetData_Bool(kInlineInputColorLine,			pref.inlineInputColorLine);
			SetData_Bool(kFindDefaultIgnoreCase,		pref.findDefaultIgnoreCase);
			SetData_Bool(kFindDefaultWholeWord,			pref.findDefaultWholeWord);
			SetData_Bool(kFindDefaultAimai,				pref.findDefaultAimai);
			SetData_Bool(kFindDefaultRegExp,			pref.findDefaultRegExp);
			SetData_Bool(kFindDefaultLoop,				pref.findDefaultLoop);
			SetData_Bool(kFindDefaultAllowReturn,		pref.findDefaultAllowReturn);
			SetData_Bool(kFindDefaultRecursive,			pref.findDefaultRecursive);
			SetData_Bool(kFindDefaultDisplayPosition,	pref.findDefaultDisplayPosition);
			text.SetPstring(pref.findDefaultFileFilter);
			text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			SetData_Text(kFindDefaultFileFilter,		text);
			//B0398 SetData_FloatNumber(kFind_FontSize,			pref.findFontSizeNum);
			point.x = pref.findDialogPosition.h;
			point.y = pref.findDialogPosition.v;
			SetData_Point(kFindWindowPosition,point);
			point.x = pref.multiFileFindDialogPosition.h;
			point.y = pref.multiFileFindDialogPosition.v;
			SetData_Point(kMultiFileFindWindowPosition,point);
			SetData_Number(kWheelScrollPercent,			pref.wheelScrollPercent);
			SetData_Number(kWheelScrollPercentCmd,		pref.wheelScrollPercentCmd);
			SetData_Bool(kAlphaWindowMode,				pref.alphaWindowMode);
			SetData_Number(kAlphaWindowPercent1,		pref.alphaWindowPercent1);
			SetData_Number(kAlphaWindowPercent2,		pref.alphaWindowPercent2);
			SetData_Bool(kCreateTabInsteadOfCreateWindow,				pref.singleWindowMode);
			SetData_Number(kJumpListOffsetX,			pref.jumpListOffsetX);
			SetData_Number(kJumpListOffsetY,			pref.jumpListOffsetY);
			SetData_Number(kJumpListAlphaPercent,		pref.jumpListAlphaPercent);
			SetData_Number(kJumpListSizeX,				pref.jumpListSizeX);
			SetData_Number(kJumpListSizeY,				pref.jumpListSizeY);
			SetData_Number(kWindowListOffsetX,			pref.windowListOffsetX);
			SetData_Number(kWindowListOffsetY,			pref.windowListOffsetY);
			SetData_Number(kWindowListAlphaPercent,		pref.windowListAlphaPercent);
			SetData_Number(kWindowListSizeX,			pref.windowListSizeX);
			SetData_Number(kWindowListSizeY,			pref.windowListSizeY);
			SetData_Bool(kDisplayWindowList,			pref.displayWindowList);
			num = kJumpListPosition_Right;
			if( pref.jumpListPosition==windowPosition_Left )   num = kJumpListPosition_Left;
			SetData_Number(kJumpListPosition,			num);
			rect.left = pref.singleWindowBounds.left;
			rect.right = pref.singleWindowBounds.right;
			rect.top = pref.singleWindowBounds.top;
			rect.bottom = pref.singleWindowBounds.bottom;
			SetData_Rect(kSingleWindowBounds,			rect);
			SetData_Number(kWindowListMode,				static_cast<ANumber>(pref.windowListMode));
		}
	}
	catch(...)
	{
		_ACError("AAppPrefDB::LoadMacLegacy_LegacyPref() caught exception.",NULL);//#199
	}
}

void	AAppPrefDB::LoadMacLegacy_ConvertEndianToHost( SMacLegacyAppPref& ioPref )
{
	//Booleanは1バイトなので変換の必要なし（boolは4バイト）、enumも1バイト
	AUniversalBinaryUtility::SwapBigToHost(ioPref.CMMouseButtonTick);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.indexWinNormalColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.indexWinBackColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.indexWinCommentColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.indexWinURLColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.indexWinGroupColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_browser);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_browserEventClass);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_browserEventID);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_undoLevel);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.indexWindowFont);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.indexWindowFontSize);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.indexWindowWidth);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.indexWindowHeight);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.indexWindowSeparateLine);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.findFontSizeNum);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.wheelScrollPercent);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.wheelScrollPercentCmd);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.alphaWindowPercent1);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.alphaWindowPercent2);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_alphaWindowPercent3);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_alphaWindowPercent4);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_alphaWindowPercent5);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.jumpListOffsetX);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.jumpListOffsetY);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.jumpListAlphaPercent);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_jumpListFontNumDummy);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_jumpListFontSizeDummy);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.jumpListSizeX);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.jumpListSizeY);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.windowListOffsetX);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.windowListOffsetY);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.windowListAlphaPercent);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_windowListFontNumDummy);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_windowListFontSizeDummy);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.windowListSizeX);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.windowListSizeY);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_windowListColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_jumpListColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_windowListTextColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.OBSOLETE_jumpListTextColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.singleWindowBounds);
}

void	AAppPrefDB::LoadMacLegacy_ConvertEndianToBig( SMacLegacyAppPref& ioPref )
{
	//Booleanは1バイトなので変換の必要なし（boolは4バイト）、enumも1バイト
	AUniversalBinaryUtility::SwapHostToBig(ioPref.CMMouseButtonTick);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.indexWinNormalColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.indexWinBackColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.indexWinCommentColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.indexWinURLColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.indexWinGroupColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_browser);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_browserEventClass);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_browserEventID);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_undoLevel);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.indexWindowFont);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.indexWindowFontSize);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.indexWindowWidth);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.indexWindowHeight);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.indexWindowSeparateLine);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.findFontSizeNum);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.wheelScrollPercent);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.wheelScrollPercentCmd);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.alphaWindowPercent1);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.alphaWindowPercent2);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_alphaWindowPercent3);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_alphaWindowPercent4);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_alphaWindowPercent5);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.jumpListOffsetX);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.jumpListOffsetY);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.jumpListAlphaPercent);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_jumpListFontNumDummy);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_jumpListFontSizeDummy);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.jumpListSizeX);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.jumpListSizeY);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.windowListOffsetX);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.windowListOffsetY);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.windowListAlphaPercent);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_windowListFontNumDummy);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_windowListFontSizeDummy);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.windowListSizeX);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.windowListSizeY);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_windowListColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_jumpListColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_windowListTextColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.OBSOLETE_jumpListTextColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.singleWindowBounds);
}

//マルチファイル検索 フォルダーストック（旧フォーマット）読み込み
void	AAppPrefDB::LoadMacLegacy_MultiFileFind_RecentlyUsedFolder( short inResRefNum )
{
	Handle	res = Get1Resource(inResRefNum,'MMFS',3000);
	if( res != NULL ) 
	{
		Int32	count = ::GetHandleSize(res)/sizeof(FSSpec);
		for( Int32 number = 0; number < count; number++ ) 
		{
			FSSpec	f;
			f = ((FSSpec*)(*res))[number];
			AUniversalBinaryUtility::SwapBigToHost(f);
			FSSpec	folder;
			if( ::FSMakeFSSpec(f.vRefNum,f.parID,f.name,&folder) == noErr )
			{
				AFileAcc	file;
				file.SpecifyByFSSpec(folder);
				AText	text;
				file.GetPath(text,kFilePathType_1);//B0389 ここはデータ互換性のため旧タイプのまま。
				text.AddPathDelimiter(AFileAcc::GetPathDelimiter(kFilePathType_1));//win 080713
				GetData_TextArrayRef(AAppPrefDB::kMultiFileFind_RecentlyUsedFolder).Add(text);
			}
		}
		::ReleaseResource(res);
	}
}

/*
#259 SpecifyByFSSpec()にバイナリのFSSpecを直接渡しているのは危険なので、旧データの読み込みはやめる
void	AAppPrefDB::LoadMacLegacy_RecentlyOpenedFile( short inResRefNum )
{
	SRecentlyOpenedResource	**res = (SRecentlyOpenedResource**)Get1Resource(inResRefNum,'ROPN',1000);
	if( res != NULL ) 
	{
		if( GetHandleSize((Handle)res) == sizeof(SRecentlyOpenedResource) ) 
		{
			short	count = (**res).count;
			AUniversalBinaryUtility::SwapBigToHost(count);
			
			for( Int32 i = 0; i < count; i++ ) 
			{
				if( i >= 48 )   break;
				
				//R-ub
				FSSpec	file = (**res).file[i];
				AUniversalBinaryUtility::SwapBigToHost(file.parID);
				AUniversalBinaryUtility::SwapBigToHost(file.vRefNum);
				
				//R-abs CText	path;
				//R-abs UFolder::GetFullPathNameUTF8(file,false,&path);
				//R-abs mRecentlyOpenedFile.AddText(path);
				
				AFileAcc	fileacc;
				fileacc.SpecifyByFSSpec(file);
				AText	text;
				fileacc.GetPath(text,kFilePathType_1);//B0389 ここはデータ互換性のため旧タイプのまま。
				GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kRecentlyOpenedFile).Add(text);
			}
		}
		ReleaseResource((Handle)res);
	}
}
*/

void	AAppPrefDB::LoadMacLegacy_LaunchApp( short inResRefNum )
{
	Int32	**rescounth = (Int32**)Get1Resource(inResRefNum,'LAPP',500);
	if( rescounth != NULL )
	{
		Int32	rescount = **rescounth;
		AUniversalBinaryUtility::SwapBigToHost(rescount);
		
		SMacLegacyLaunchApp	**resh = (SMacLegacyLaunchApp**)Get1Resource(inResRefNum,'LAPP',1000);
		if( resh != NULL )
		{
			for( Int32 i = 0; i < rescount; i++ ) 
			{
				SMacLegacyLaunchApp	app = (*resh)[i];
				AUniversalBinaryUtility::SwapBigToHost(app.creator);
				AUniversalBinaryUtility::SwapBigToHost(app.type);
				AUniversalBinaryUtility::SwapBigToHost(app.launchCreator);
				AUniversalBinaryUtility::SwapBigToHost(app.launchFileSpec);
				
				AText	text;
				text.SetPstring(app.extension);
				GetData_TextArrayRef(AAppPrefDB::kLaunchApp_Extension).Add(text);
				//AUtil::GetATextFromOSType(app.creator,text);
				text.SetFromOSType(app.creator);
				GetData_TextArrayRef(AAppPrefDB::kLaunchApp_Creator).Add(text);
				//AUtil::GetATextFromOSType(app.type,text);
				text.SetFromOSType(app.type);
				GetData_TextArrayRef(AAppPrefDB::kLaunchApp_Type).Add(text);
				
				text.DeleteAll();
				if( app.launchFileSpec.name[0] != 0 )
				{
					FSSpec	fsspec;//#259
					::FSMakeFSSpec(app.launchFileSpec.vRefNum,app.launchFileSpec.parID,app.launchFileSpec.name,&fsspec);//#259
					AFileAcc	file;
					file.SpecifyByFSSpec(fsspec);//#259
					file.GetPath(text);
				}
				GetData_TextArrayRef(AAppPrefDB::kLaunchApp_App).Add(text);
				
				GetData_BoolArrayRef(AAppPrefDB::kLaunchApp_CheckExtension).Add(app.checkExtension==true);
				GetData_BoolArrayRef(AAppPrefDB::kLaunchApp_CheckCreator).Add(app.checkCreator==true);
				GetData_BoolArrayRef(AAppPrefDB::kLaunchApp_CheckType).Add(app.checkType==true);
				GetData_BoolArrayRef(AAppPrefDB::kLaunchApp_ShiftKey).Add(app.shiftKey==true);
			}
			ReleaseResource((Handle)resh);
		}
		ReleaseResource((Handle)rescounth);
	}
}

void	AAppPrefDB::LoadMacLegacy_FindMemory( short inResRefNum )
{
	short	rescount = 0;
	SMacLegacyFindMemoryHeader	**res = (SMacLegacyFindMemoryHeader**)Get1Resource(inResRefNum,'FMEM',1000);
	if( res != NULL ) 
	{
		if( GetHandleSize((Handle)res) == sizeof(SMacLegacyFindMemoryHeader) ) 
		{
			rescount = (**res).findMemoryCount;
			
			//R-ub
			AUniversalBinaryUtility::SwapBigToHost(rescount);
		}
		ReleaseResource((Handle)res);
	}
	if( rescount != 0 )
	{
		//R-uni 
		Boolean	added = false;
		for( Int32 i = 0; i < rescount; i++ ) 
		{
			SMacLegacyFindMemory	**res = (SMacLegacyFindMemory**)Get1Resource(inResRefNum,'FMEM',3000+i);
			if( res != NULL ) 
			{
				HLock((Handle)res);
				
				AText	text;
				text.SetPstring((**res).name);
				GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kFindMemoryName,text);
				text.SetPstring((**res).findString);
				GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kFindMemoryFindText,text);
				text.SetPstring((**res).replaceString);
				GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kFindMemoryReplaceText,text);
				GetApp().GetAppPref().Add_BoolArray(AAppPrefDB::kFindMemoryIgnoreCase,(**res).ignoreCase);
				GetApp().GetAppPref().Add_BoolArray(AAppPrefDB::kFindMemoryWholeWord,(**res).wholeWord);
				GetApp().GetAppPref().Add_BoolArray(AAppPrefDB::kFindMemoryAimai,(**res).aimai);
				GetApp().GetAppPref().Add_BoolArray(AAppPrefDB::kFindMemoryRegExp,(**res).regExp);
				GetApp().GetAppPref().Add_BoolArray(AAppPrefDB::kFindMemoryLoop,(**res).loop);
				
				added = true;
				HUnlock((Handle)res);
				ReleaseResource((Handle)res);
			}
		}
		
		if( ! added )//R-uni 3000番台が見つからない場合は旧Macコード用リソースを検索し、Macコード→UTF8変換を行う
		{
			for( Int32 i = 0; i < rescount; i++ ) 
			{
				SMacLegacyFindMemory	**res = (SMacLegacyFindMemory**)Get1Resource(inResRefNum,'FMEM',2000+i);
				if( res != NULL ) 
				{
					HLock((Handle)res);
					
					//R-uni
					AText	fontname;//win
					AFontWrapper::GetDialogDefaultFontName(fontname);//win #375
					TextEncoding	tec = ATextEncodingWrapper::GetSJISTextEncoding();//#688 GetLegacyTextEncodingFromFont(/*win ::GetAppFont()*/fontname);
					//#307 TextEncoding	tmp;
					SMacLegacyFindMemory	resc =**res;
					AText	text1(resc.name);
					//B0400 AUtil::ConvertToUTF8CRAnyway(text1,tec,tmp);
					GetApp().NVI_ConvertToUTF8CRAnyway(text1,tec,false);//B0400 #182 #307
					text1.GetPstring(resc.name,255);
					AText	text2(resc.findString);
					//B0400 AUtil::ConvertToUTF8CRAnyway(text2,tec,tmp);
					GetApp().NVI_ConvertToUTF8CRAnyway(text2,tec,false);//B0400 #182 #307
					text2.GetPstring(resc.findString,255);
					AText	text3(resc.replaceString);
					//B0400 AUtil::ConvertToUTF8CRAnyway(text3,tec,tmp);
					GetApp().NVI_ConvertToUTF8CRAnyway(text3,tec,false);//B0400 #182 #307
					text3.GetPstring(resc.replaceString,255);
					
					AText	text;
					text.SetPstring(resc.name);
					GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kFindMemoryName,text);
					text.SetPstring(resc.findString);
					GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kFindMemoryFindText,text);
					text.SetPstring(resc.replaceString);
					GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kFindMemoryReplaceText,text);
					GetApp().GetAppPref().Add_BoolArray(AAppPrefDB::kFindMemoryIgnoreCase,resc.ignoreCase);
					GetApp().GetAppPref().Add_BoolArray(AAppPrefDB::kFindMemoryWholeWord,resc.wholeWord);
					GetApp().GetAppPref().Add_BoolArray(AAppPrefDB::kFindMemoryAimai,resc.aimai);
					GetApp().GetAppPref().Add_BoolArray(AAppPrefDB::kFindMemoryRegExp,resc.regExp);
					GetApp().GetAppPref().Add_BoolArray(AAppPrefDB::kFindMemoryLoop,resc.loop);
					
					HUnlock((Handle)res);
					ReleaseResource((Handle)res);
				}
			}
		}
	}
}

void	AAppPrefDB::LoadMacLegacy_FolderList( short inResRefNum )
{
	for( short i = 0; i < 100; i++ )
	{
		Handle	res = Get1Resource(inResRefNum,'FOLD',1000+i);
		if( res == NULL )   break;
		long	len = ::GetHandleSize(res);
		AText	text;
		::HLock(res);
		text.SetFromTextPtr(*res,len);
		::HUnlock(res);
		text.ReplaceChar(':','/');
		Add_TextArray(AAppPrefDB::kFolderListMenu,text);
		::ReleaseResource(res);
	}
}

void	AAppPrefDB::LoadMacLegacy_FTPFolder( short inResRefNum )
{
	for( short i = 0; i < 100; i++ )
	{
		Handle	res = Get1Resource(inResRefNum,'FTPF',1000+i);
		if( res == NULL )   break;
		SLegacyFTPFolderResHeader	**folderRes = (SLegacyFTPFolderResHeader**)res;
		short	depth = (*folderRes)->level;
		AUniversalBinaryUtility::SwapBigToHost(depth);
		GetData_NumberArrayRef(AAppPrefDB::kFTPFolder_Depth).Add(depth);
		
		long	len = GetHandleSize(res)-sizeof(SLegacyFTPFolderResHeader);
		AText	server, user, path;
		long	pos = sizeof(SLegacyFTPFolderResHeader);
		
		for( long i = 0; i < len; i++ )
		{
			if( (*res)[pos] == 0 )   break;
			server.Add((*res)[pos]);
			pos++;
		}
		pos++;
		
		for( long i = 0; i < len; i++ )
		{
			if( (*res)[pos] == 0 )   break;
			user.Add((*res)[pos]);
			pos++;
		}
		pos++;
		
		for( long i = 0; i < len; i++ )
		{
			if( (*res)[pos] == 0 )   break;
			path.Add((*res)[pos]);
			pos++;
		}
		pos++;
		
		GetData_TextArrayRef(AAppPrefDB::kFTPFolder_Server).Add(server);
		GetData_TextArrayRef(AAppPrefDB::kFTPFolder_User).Add(user);
		GetData_TextArrayRef(AAppPrefDB::kFTPFolder_Path).Add(path);
		ReleaseResource(res);
	}
}

void	AAppPrefDB::LoadMacLegacy_FTPServer( short inResRefNum )
{
	for( short i = 0; i < 100; i++ )
	{
		Handle	res = Get1Resource(inResRefNum,'FTPS',1000+i);
		if( res == NULL )   break;
		SMacLegacyFTPServer	pref = *((SMacLegacyFTPServer*)(*res));
		AUniversalBinaryUtility::SwapBigToHost(pref.version);
		AUniversalBinaryUtility::SwapBigToHost(pref.port);
		AUniversalBinaryUtility::SwapBigToHost(pref.backupFolder.dirID);
		AUniversalBinaryUtility::SwapBigToHost(pref.backupFolder.vRefNum);
		
		if( pref.version < 2 )
		{
			pref.proxyUser[0] = 0;
		}
		if( pref.version < 3 )
		{
			pref.backup = false;
		}
		pref.version = 3;
		
		AText	text;
		text.SetPstring(pref.server);
		GetData_TextArrayRef(AAppPrefDB::kFTPAccount_Server).Add(text);
		text.SetPstring(pref.user);
		GetData_TextArrayRef(AAppPrefDB::kFTPAccount_User).Add(text);
		GetData_BoolArrayRef(AAppPrefDB::kFTPAccount_Passive).Add(pref.PASV);
		text.SetPstring(pref.proxyServer);
		GetData_TextArrayRef(AAppPrefDB::kFTPAccount_ProxyServer).Add(text);
		text.SetPstring(pref.rootPath);
		GetData_TextArrayRef(AAppPrefDB::kFTPAccount_RootPath).Add(text);
		text.SetPstring(pref.httpURL);
		GetData_TextArrayRef(AAppPrefDB::kFTPAccount_HTTPURL).Add(text);
		GetData_NumberArrayRef(AAppPrefDB::kFTPAccount_Port).Add(pref.port);
		ANumber	proxymode = kProxyMode_NoProxy;
		switch( pref.proxyMode )
		{
		  case proxyMode_NoProxy:
		  case proxyMode_URLAccess:
			{
				proxymode = kProxyMode_NoProxy;
				break;
			}
		  case proxyMode_FTPProxy1:
			{
				proxymode = kProxyMode_FTPProxy1;
				break;
			}
		  case proxyMode_FTPProxy2:
			{
				proxymode = kProxyMode_FTPProxy2;
				break;
			}
		}
		GetData_NumberArrayRef(AAppPrefDB::kFTPAccount_ProxyMode).Add(proxymode);
		text.SetPstring(pref.proxyUser);
		GetData_TextArrayRef(AAppPrefDB::kFTPAccount_ProxyUser).Add(text);
		GetData_BoolArrayRef(AAppPrefDB::kFTPAccount_Backup).Add(pref.backup);
		FSSpec	fsspec;
		::FSMakeFSSpec(pref.backupFolder.vRefNum,pref.backupFolder.dirID,"\p",&fsspec);
		AFileAcc	file;
		file.SpecifyByFSSpec(fsspec);
		file.GetPath(text);
		GetData_TextArrayRef(AAppPrefDB::kFTPAccount_BackupFolderPath).Add(text);
		Add_BoolArray(kFTPAccount_BinaryMode,false);
		
		ReleaseResource(res);
	}
}

void	AAppPrefDB::LoadMacLegacy_Kinsoku()
{
	AFileAcc	preffolder;
	AFileWrapper::GetAppPrefFolder(preffolder);
	AFileAcc	file;
	file.SpecifyChild(preffolder,"Kinsoku.txt");
	if( file.Exist() == false )   return;
	AText	text;
	file.ReadTo(text);
	text.ConvertToUTF8FromUTF16();
	SetData_Text(kKinsokuLetters,text);
}

void	AAppPrefDB::LoadMacLegacy_TextEncoding()
{
	//Preferencesフォルダ内miフォルダの下の"TextEncodings.txt"から１行ずつ読み出して
	//当システムで利用可能ならば、kTextEncodingMenuListに格納する。
	AFileAcc	preffolder;
	AFileWrapper::GetAppPrefFolder(preffolder);
	AFileAcc	file;
	file.SpecifyChild(preffolder,"TextEncodings.txt");
	AText	text;
	file.ReadTo(text);
	if( text.GetLength() > 0 )
	{
		GetData_TextArrayRef(kTextEncodingMenu).DeleteAll();
		for( AIndex pos = 0; pos < text.GetLength(); )
		{
			AText	line;
			text.GetLine(pos,line);
			if( ATextEncodingWrapper::CheckTextEncodingAvailability(line) == true )
			{
				GetData_TextArrayRef(kTextEncodingMenu).Add(line);
			}
		}
	}
}

void	AAppPrefDB::WriteMacLegacy()
{
	AFileAcc	preffile;
	AFileWrapper::GetAppPrefFile(preffile);
	FSRef	ref;
	preffile.GetFSRef(ref);
	short	rRefNum = ::FSOpenResFile(&ref,fsWrPerm);
	if( rRefNum != -1 )
	{
		//WriteMacLegacy_FTPFolder(rRefNum);
		
		//#259
		{
			Handle	res;
			res = Get1Resource(rRefNum,'ROPN',1000);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
		}
		
		WritePrefVersion(rRefNum);
		::CloseResFile(rRefNum);
	}
}

/*void	AAppPrefDB::WriteMacLegacy_FTPFolder( short inResRefNum )
{
	for( short i = 0; i < 100; i++ )
	{
		Handle	res = Get1Resource(inResRefNum,'FTPF',1000+i);
		if( res != NULL ) 
		{
			::RemoveResource(res);
			::DisposeHandle(res);
		}
		if( i < mFTPFolder_Server.GetItemCount() )
		{
			res = NewHandle(10);
			if( res != NULL )
			{
				AText	server, user, path;
				mFTPFolder_Server.Get(i,server);
				mFTPFolder_User.Get(i,user);
				mFTPFolder_Path.Get(i,path);
				long	len = server.GetLength()+user.GetLength()+path.GetLength();
				::SetHandleSize(res,len+sizeof(SLegacyFTPFolderResHeader)+3);
				long	pos = sizeof(SLegacyFTPFolderResHeader);
				for( long i = 0; i < server.GetLength(); i++ )
				{
					(*res)[pos] = server.Get(i);
					pos++;
				}
				(*res)[pos] = 0;
				pos++;
				for( long i = 0; i < user.GetLength(); i++ )
				{
					(*res)[pos] = user.Get(i);
					pos++;
				}
				(*res)[pos] = 0;
				pos++;
				for( long i = 0; i < path.GetLength(); i++ )
				{
					(*res)[pos] = path.Get(i);
					pos++;
				}
				(*res)[pos] = 0;
				pos++;
				SLegacyFTPFolderResHeader **folderRes = (SLegacyFTPFolderResHeader**)res;
				(*folderRes)->level = mFTPFolder_Depth.Get(i);
				::HLock((Handle)folderRes);
				AUniversalBinaryUtility::SwapBigToHost((*folderRes)->level);
				::HUnlock((Handle)folderRes);
				
				AddResource(res,'FTPF',1000+i,"\p");
				WriteResource(res);
				ReleaseResource(res);
			}
		}
	}
}*/


#endif

#pragma mark ---ModePref

//旧MacOSリソースタイプのデータ読み込み
void	AModePrefDB::LoadMacLegacy()
{
	//モードファイル取得 #920
	AText	modeRawName;
	GetModeRawName(modeRawName);
	AFileAcc	modeFile;
	modeFile.SpecifyChild(mModeFolder,modeRawName);
	if( modeFile.Exist() == false )
	{
		return;
	}
	//
	AText	text;
	modeFile.ReadTo(text);
	FSRef	ref;
	modeFile.GetFSRef(ref);
	short	rRefNum = ::FSOpenResFile(&ref,fsRdPerm);
	if( rRefNum != -1 )
	{
		LoadMacLegacy_Pref(rRefNum);
		LoadMacLegacy_DragDrop(rRefNum);
		LoadMacLegacy_Correspond(rRefNum);
		LoadMacLegacy_Indent(rRefNum);
		LoadMacLegacy_JumpSetup(rRefNum);
		LoadMacLegacy_KeyBind(rRefNum);
		LoadMacLegacy_Keyword(rRefNum,text);
		
		::CloseResFile(rRefNum);
	}
}

//#844
#if 0
void	AModePrefDB::WriteMacLegacy()
{
	AText	dftext;
	FSRef	ref;
	mModeFile.GetFSRef(ref);
	short	rRefNum = ::FSOpenResFile(&ref,fsWrPerm);
	if( rRefNum != -1 )
	{
		WriteMacLegacy_Pref(rRefNum);
		WriteMacLegacy_DragDrop(rRefNum);
		WriteMacLegacy_Correspond(rRefNum);
		WriteMacLegacy_Indent(rRefNum);
		WriteMacLegacy_JumpSetup(rRefNum);
		WriteMacLegacy_KeyBind(rRefNum);
		WriteMacLegacy_Keyword(rRefNum,dftext);
		
		WritePrefVersion(rRefNum);
		::CloseResFile(rRefNum);
	}
	mModeFile.WriteFile(dftext);
}
#endif

void	AModePrefDB::LoadMacLegacy_KeyBind( short inResRefNum )
{
	short	rescount = 0;
	SLegacyKeyBindHeader	**res = (SLegacyKeyBindHeader**)Get1Resource(inResRefNum,'KYBD',1000);
	if( res != NULL ) 
	{
		if( GetHandleSize((Handle)res) == sizeof(SLegacyKeyBindHeader) ) 
		{
			rescount = (**res).keyBindCount;
			//R-ub
			AUniversalBinaryUtility::SwapBigToHost(rescount);
		}
		::ReleaseResource((Handle)res);
	}
	if( rescount != 0 ) 
	{
		for( Int32 i = 0; i < rescount; i++ ) 
		{
			Boolean	utf8 = true;//R-uni
			Handle	res = NULL;
			res = Get1Resource(inResRefNum,'KYBD',3000+i);//R-uni
			if( res == NULL )//R-uni
			{
				utf8 = false;
				res = Get1Resource(inResRefNum,'KYBD',2000+i);
			}
			if( res != NULL ) 
			{
				SLegacyKeyBind	keyBind;
				keyBind.controlKey = ((SLegacyKeyBind*)(*res))->controlKey;
				keyBind.optionKey = ((SLegacyKeyBind*)(*res))->optionKey;
				keyBind.commandKey = ((SLegacyKeyBind*)(*res))->commandKey;
				keyBind.shiftKey = ((SLegacyKeyBind*)(*res))->shiftKey;
				keyBind.keyCode = ((SLegacyKeyBind*)(*res))->keyCode;
				//R-ub
				AUniversalBinaryUtility::SwapBigToHost(keyBind.keyCode);
					
				keyBind.action = ((SLegacyKeyBind*)(*res))->action;
				
				AText	text;
				if( keyBind.action == keyAction_string ) 
				{
					Int32	stringsize = GetHandleSize(res)-sizeof(SLegacyKeyBind);
					for( Int32 k = 0; k < stringsize; k++ ) 
					{
						text.Add(*((unsigned char*)(*res)+sizeof(SLegacyKeyBind)+k));
					}
					if( !utf8 )//R-uni
					{
						ATextEncoding	tec;
						AReturnCode	returnCode;
						ATextEncodingFallbackType	fallbackType;
						ABool	notMixed = true;//#995
						/*#1034
						GetApp().SPI_ConvertToUTF8CR(text,ATextEncodingWrapper::GetSJISTextEncoding(),false,
													 true,false,returnCode_CR,tec,returnCode,fallbackType,notMixed);//B0400 #182 #307
						*/
						//テキストエンコーディング変換 #1034 AAppを使用せず、ATextで実行する
						ABool	fallbackUsed = false;
						text.ConvertToUTF8CR(ATextEncodingWrapper::GetSJISTextEncoding(),true,fallbackUsed);
					}
				}
				
				AKeyBindKey	key = AKeyWrapper::GetKeyBindKeyFromKeyCode(keyBind.keyCode);
				if( key != kKeyBindKey_Invalid )
				{
					Add_NumberArray(kKeyBind_Key,key);
					Add_BoolArray(kKeyBind_ControlKey,keyBind.controlKey);
					Add_BoolArray(kKeyBind_OptionKey,keyBind.optionKey);
					Add_BoolArray(kKeyBind_CommandKey,keyBind.commandKey);
					Add_BoolArray(kKeyBind_ShiftKey,keyBind.shiftKey);
					Add_NumberArray(kKeyBind_Action,keyBind.action);
					Add_TextArray(kKeyBind_Text,text);
				}
			}
			::ReleaseResource(res);
		}
	}
}

//#844
#if 0
void	AModePrefDB::WriteMacLegacy_KeyBind( short inResRefNum )
{
	short	legacyKeyCodeTable[] =
	{
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
				0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
				'\r','\t',' ',0x1B,3,8,127,0x1E,0x1F,0x1C,0x1D,':',';','-','^','\\','@','[',']',',','.','/'
			};
	SLegacyKeyBindHeader	**res = (SLegacyKeyBindHeader**)Get1Resource(inResRefNum,'KYBD',1000);
	if( res != NULL ) 
	{
		::RemoveResource((Handle)res);
		::DisposeHandle((Handle)res);
	}
	res = (SLegacyKeyBindHeader**)NewHandle(sizeof(SLegacyKeyBindHeader));
	if( res != NULL ) 
	{
		(**res).keyBindCount = GetItemCount_NumberArray(kKeyBind_Key);
		//R-ub
		::HLock((Handle)res);
		AUniversalBinaryUtility::SwapHostToBig((**res).keyBindCount);
		::HUnlock((Handle)res);
		
		::AddResource((Handle)res,'KYBD',1000,"\p");
		::WriteResource((Handle)res);
		::ReleaseResource((Handle)res);
	}
	for( AIndex i = 0; i < GetItemCount_NumberArray(kKeyBind_Key); i++ )
	{
		if( i > 999 )   break;
		SLegacyKeyBind**	res = (SLegacyKeyBind**)Get1Resource(inResRefNum,'KYBD',3000+i);
		if( res != NULL )
		{
			::RemoveResource((Handle)res);
			::DisposeHandle((Handle)res);
		}
		res = (SLegacyKeyBind**)Get1Resource(inResRefNum,'KYBD',2000+i);//SJIS版
		if( res != NULL )
		{
			::RemoveResource((Handle)res);
			::DisposeHandle((Handle)res);
		}
		ANumber	key = GetData_NumberArray(kKeyBind_Key,i);
		if( key > 62 || key < 0 )
		{
			//2.1.8a7時点でのキーバインド可能なキーの数は62個。63以降は、それ以降のバージョンから追加されたキーなので変換保存不要。
			continue;
		}
		ANumber	action = GetData_NumberArray(kKeyBind_Action,i);
		AText	actionText;
		GetData_TextArray(kKeyBind_Text,i,actionText);
		
		ByteCount	ressize = sizeof(SLegacyKeyBind);
		if( action == keyAction_string )
		{
			ressize += actionText.GetLength();
		}
		res = (SLegacyKeyBind**)::NewHandle(ressize);
		if( res != NULL )
		{
			(**res).controlKey 	= GetData_BoolArray(kKeyBind_ControlKey,i);
			(**res).optionKey 	= GetData_BoolArray(kKeyBind_OptionKey,i);
			(**res).commandKey 	= GetData_BoolArray(kKeyBind_CommandKey,i);
			(**res).shiftKey 	= GetData_BoolArray(kKeyBind_ShiftKey,i);
			(**res).keyCode 	= legacyKeyCodeTable[key];
			(**res).action 		= static_cast<EKeyAction>(action);//singed long(AIndex)->int(enum)のキャスト
			if( action == keyAction_string )
			{
				for( Int32 l = 0; l < actionText.GetLength(); l++ ) 
				{
					(*((unsigned char*)(*res)+sizeof(SLegacyKeyBind)+l)) = actionText.Get(l);
				}
			}
			::HLock((Handle)res);
			AUniversalBinaryUtility::SwapHostToBig((**res).keyCode);
			::HUnlock((Handle)res);
			
			::AddResource((Handle)res,'KYBD',3000+i,"\p");
			::WriteResource((Handle)res);
			::ReleaseResource((Handle)res);
		}
		//SJIS版保存
		AText	actionTextSJIS;
		actionTextSJIS.SetText(actionText);
		actionTextSJIS.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
		ressize = sizeof(SLegacyKeyBind);
		if( action == keyAction_string )
		{
			ressize += actionTextSJIS.GetLength();
		}
		res = (SLegacyKeyBind**)::NewHandle(ressize);
		if( res != NULL )
		{
			(**res).controlKey 	= GetData_BoolArray(kKeyBind_ControlKey,i);
			(**res).optionKey 	= GetData_BoolArray(kKeyBind_OptionKey,i);
			(**res).commandKey 	= GetData_BoolArray(kKeyBind_CommandKey,i);
			(**res).shiftKey 	= GetData_BoolArray(kKeyBind_ShiftKey,i);
			(**res).keyCode 	= legacyKeyCodeTable[key];
			(**res).action 		= static_cast<EKeyAction>(action);//singed long(AIndex)->int(enum)のキャスト
			if( action == keyAction_string )
			{
				for( Int32 l = 0; l < actionTextSJIS.GetLength(); l++ ) 
				{
					(*((unsigned char*)(*res)+sizeof(SLegacyKeyBind)+l)) = actionTextSJIS.Get(l);
				}
			}
			::HLock((Handle)res);
			AUniversalBinaryUtility::SwapHostToBig((**res).keyCode);
			::HUnlock((Handle)res);
			
			::AddResource((Handle)res,'KYBD',2000+i,"\p");
			::WriteResource((Handle)res);
			::ReleaseResource((Handle)res);
		}
	}
}
#endif

void	AModePrefDB::LoadMacLegacy_JumpSetup( short inResRefNum )
{
	short	rescount = 0;
	SLegacyMdsLinePrefHeader	**res = (SLegacyMdsLinePrefHeader**)Get1Resource(inResRefNum,'JUMP',1000);
	if( res != NULL ) 
	{
		if( GetHandleSize((Handle)res) == sizeof(SLegacyMdsLinePrefHeader) ) {
			rescount = (**res).MdsLinePrefCount;
			//R-ub
			AUniversalBinaryUtility::SwapBigToHost(rescount);
		}
		::ReleaseResource((Handle)res);
	}
	if( rescount != 0 ) 
	{
		for( short i = 0; i < rescount; i++ ) {
			//R-uni UTF-8版は4000番台 B0128
			SLegacyMdsLinePref	**res = (SLegacyMdsLinePref**)Get1Resource(inResRefNum,'JUMP',4000+i);
			if( res != NULL ) 
			{
				SLegacyMdsLinePref	pref = **res;
				//R-ub
				AUniversalBinaryUtility::SwapBigToHost(pref.lineColor);
				AUniversalBinaryUtility::SwapBigToHost(pref.selectionGroup);
				AUniversalBinaryUtility::SwapBigToHost(pref.keywordGroup);
				AUniversalBinaryUtility::SwapBigToHost(pref.keywordColor);
				
				AText	text;
				text.SetPstring(pref.name);
				text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				Add_TextArray(kJumpSetup_Name,text);
				text.SetPstring(pref.regexp);
				Add_TextArray(kJumpSetup_RegExp,text);
				text.SetPstring(pref.menustring);
				Add_TextArray(kJumpSetup_MenuText,text);
				Add_NumberArray(kJumpSetup_SelectionGroup,pref.selectionGroup);
				Add_NumberArray(kJumpSetup_KeywordGroup,pref.keywordGroup);
				Add_BoolArray(kJumpSetup_ColorizeLine,pref.isColorLine);
				Add_ColorArray(kJumpSetup_LineColor,pref.lineColor);
				Add_ColorArray(kJumpSetup_KeywordColor,pref.keywordColor);
				Add_BoolArray(kJumpSetup_RegisterToMenu,(pref.menustring[0]!=0));
				Add_BoolArray(kJumpSetup_RegisterAsKeyword,(pref.keywordGroup!=0));
				
				::ReleaseResource((Handle)res);
			}
			else
			{
				SLegacyMdsLinePref	**res = (SLegacyMdsLinePref**)Get1Resource(inResRefNum,'JUMP',2000+i);
				if( res != NULL ) 
				{
					SLegacyMdsLinePref	pref = **res;
					//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.lineColor);
					AUniversalBinaryUtility::SwapBigToHost(pref.selectionGroup);
					AUniversalBinaryUtility::SwapBigToHost(pref.keywordGroup);
					AUniversalBinaryUtility::SwapBigToHost(pref.keywordColor);
					
					AText	text;
					text.SetPstring(pref.name);
					text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
					Add_TextArray(kJumpSetup_Name,text);
					text.SetPstring(pref.regexp);
					text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
					Add_TextArray(kJumpSetup_RegExp,text);
					text.SetPstring(pref.menustring);
					text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
					Add_TextArray(kJumpSetup_MenuText,text);
					Add_NumberArray(kJumpSetup_SelectionGroup,pref.selectionGroup);
					Add_NumberArray(kJumpSetup_KeywordGroup,pref.keywordGroup);
					Add_BoolArray(kJumpSetup_ColorizeLine,pref.isColorLine);
					Add_ColorArray(kJumpSetup_LineColor,pref.lineColor);
					Add_ColorArray(kJumpSetup_KeywordColor,pref.keywordColor);
					Add_BoolArray(kJumpSetup_RegisterToMenu,(pref.menustring[0]!=0));
					Add_BoolArray(kJumpSetup_RegisterAsKeyword,(pref.keywordGroup!=0));
					
					::ReleaseResource((Handle)res);
				}
			}
		}
	}
}

//#844
#if 0
void	AModePrefDB::WriteMacLegacy_JumpSetup( short inResRefNum )
{
	SLegacyMdsLinePrefHeader	**res = (SLegacyMdsLinePrefHeader**)Get1Resource(inResRefNum,'JUMP',1000);
	if( res != NULL ) 
	{
		RemoveResource((Handle)res);
		DisposeHandle((Handle)res);
	}
	res = (SLegacyMdsLinePrefHeader**)NewHandle(sizeof(SLegacyMdsLinePrefHeader));
	if( res != NULL ) 
	{
		(**res).MdsLinePrefCount = GetItemCount_TextArray(kJumpSetup_Name);
		
		//R-ub
		::HLock((Handle)res);
		AUniversalBinaryUtility::SwapHostToBig((**res).MdsLinePrefCount);
		::HUnlock((Handle)res);
		
		AddResource((Handle)res,'JUMP',1000,"\p");
		WriteResource((Handle)res);
		ReleaseResource((Handle)res);
	}
	for( AIndex i = 0; i < GetItemCount_TextArray(kJumpSetup_Name); i++ ) 
	{
		SLegacyMdsLinePref	**res = (SLegacyMdsLinePref**)Get1Resource(inResRefNum,'JUMP',4000+i);
		if( res != NULL ) 
		{
			RemoveResource((Handle)res);
			DisposeHandle((Handle)res);
		}
		res = (SLegacyMdsLinePref**)Get1Resource(inResRefNum,'JUMP',2000+i);
		if( res != NULL ) 
		{
			RemoveResource((Handle)res);
			DisposeHandle((Handle)res);
		}
		
		SLegacyMdsLinePref	pref;
		pref.local = false;
		pref.localBorder = false;
		pref.info[0] = 0;
		AText	text;
		GetData_TextArray(kJumpSetup_Name,i,text);
		text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
		text.LimitLengthAsSJIS(0,31);
		text.GetPstring(pref.name,31);
		GetData_TextArray(kJumpSetup_RegExp,i,text);
		text.LimitLength(0,255);
		text.GetPstring(pref.regexp,255);
		GetData_TextArray(kJumpSetup_MenuText,i,text);
		text.LimitLength(0,31);
		text.GetPstring(pref.menustring,31);
		pref.selectionGroup = GetData_NumberArray(kJumpSetup_SelectionGroup,i);
		pref.keywordGroup = GetData_NumberArray(kJumpSetup_KeywordGroup,i);
		pref.isColorLine = GetData_BoolArray(kJumpSetup_ColorizeLine,i);
		GetData_ColorArray(kJumpSetup_LineColor,i,pref.lineColor);
		GetData_ColorArray(kJumpSetup_KeywordColor,i,pref.keywordColor);
		if( GetData_BoolArray(kJumpSetup_RegisterToMenu,i) == false )
		{
			pref.menustring[0] = 0;
		}
		if( GetData_BoolArray(kJumpSetup_RegisterAsKeyword,i) == false )
		{
			pref.keywordGroup = 0;
		}
		
		//UTF-8版を4000番台に保存
		res = (SLegacyMdsLinePref**)NewHandle(sizeof(SLegacyMdsLinePref));
		if( res != NULL ) 
		{
			**res = pref;
			
			//R-ub
			::HLock((Handle)res);
			AUniversalBinaryUtility::SwapHostToBig((**res).lineColor);
			AUniversalBinaryUtility::SwapHostToBig((**res).selectionGroup);
			AUniversalBinaryUtility::SwapHostToBig((**res).keywordGroup);
			AUniversalBinaryUtility::SwapHostToBig((**res).keywordColor);
			::HUnlock((Handle)res);
			
			AddResource((Handle)res,'JUMP',4000+i,"\p");
			WriteResource((Handle)res);
			ReleaseResource((Handle)res);
		}
		
		//SJIS版を2000番台に保存
		GetData_TextArray(kJumpSetup_Name,i,text);
		text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
		text.LimitLengthAsSJIS(0,31);
		text.GetPstring(pref.name,31);
		
		GetData_TextArray(kJumpSetup_RegExp,i,text);
		text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
		text.LimitLengthAsSJIS(0,255);
		text.GetPstring(pref.regexp,255);
		
		GetData_TextArray(kJumpSetup_MenuText,i,text);
		text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
		text.LimitLengthAsSJIS(0,31);
		text.GetPstring(pref.menustring,31);
		
		res = (SLegacyMdsLinePref**)NewHandle(sizeof(SLegacyMdsLinePref));
		if( res != NULL ) 
		{
			**res = pref;
			
			//R-ub
			::HLock((Handle)res);
			AUniversalBinaryUtility::SwapHostToBig((**res).lineColor);
			AUniversalBinaryUtility::SwapHostToBig((**res).selectionGroup);
			AUniversalBinaryUtility::SwapHostToBig((**res).keywordGroup);
			AUniversalBinaryUtility::SwapHostToBig((**res).keywordColor);
			::HUnlock((Handle)res);
			
			AddResource((Handle)res,'JUMP',2000+i,"\p");
			WriteResource((Handle)res);
			ReleaseResource((Handle)res);
		}
	}
}
#endif

void	AModePrefDB::LoadMacLegacy_Indent( short inResRefNum )
{
	SLegacyIndentPrefHeader	**res = (SLegacyIndentPrefHeader**)Get1Resource(inResRefNum,'IDNT',1000);
	short	rescount = 0;
	if( res != NULL ) 
	{
		if( GetHandleSize((Handle)res) == sizeof(SLegacyIndentPrefHeader) ) 
		{
			rescount = (**res).IndentPrefCount;
			//R-ub
			AUniversalBinaryUtility::SwapBigToHost(rescount);
		}
		ReleaseResource((Handle)res);
	}
	if( rescount != 0 ) 
	{
		for( short i = 0; i < rescount; i++ ) 
		{
			SLegacyIndentPref	**res = (SLegacyIndentPref**)Get1Resource(inResRefNum,'IDNT',2000+i);
			if( res != NULL ) 
			{
				SLegacyIndentPref	pref = **res;
				//R-ub
				AUniversalBinaryUtility::SwapBigToHost(pref.currentIndent);
				AUniversalBinaryUtility::SwapBigToHost(pref.nextIndent);
				
				AText	text;
				text.SetPstring(pref.regexp);
				Add_TextArray(kIndent_RegExp,text);
				Add_NumberArray(kIndent_OffsetCurrentLine,pref.currentIndent);
				Add_NumberArray(kIndent_OffsetNextLine,pref.nextIndent);
				
				::ReleaseResource((Handle)res);
			}
		}
	}
}

//#844
#if 0
void	AModePrefDB::WriteMacLegacy_Indent( short inResRefNum )
{
	SLegacyIndentPrefHeader	**res = (SLegacyIndentPrefHeader**)Get1Resource(inResRefNum,'IDNT',1000);
	if( res != NULL ) 
	{
		RemoveResource((Handle)res);
		DisposeHandle((Handle)res);
	}
	res = (SLegacyIndentPrefHeader**)NewHandle(sizeof(SLegacyIndentPrefHeader));
	if( res != NULL ) 
	{
		(**res).IndentPrefCount = GetItemCount_TextArray(kIndent_RegExp);
		
		//R-ub
		::HLock((Handle)res);
		AUniversalBinaryUtility::SwapHostToBig((**res).IndentPrefCount);
		::HUnlock((Handle)res);
		
		AddResource((Handle)res,'IDNT',1000,"\p");
		WriteResource((Handle)res);
		ReleaseResource((Handle)res);
	}
	for( AIndex i = 0; i <  GetItemCount_TextArray(kIndent_RegExp); i++ ) 
	{
		SLegacyIndentPref	**res = (SLegacyIndentPref**)Get1Resource(inResRefNum,'IDNT',2000+i);
		if( res != NULL ) 
		{
			RemoveResource((Handle)res);
			DisposeHandle((Handle)res);
		}
		res = (SLegacyIndentPref**)NewHandle(sizeof(SLegacyIndentPref));
		if( res != NULL ) 
		{
			SLegacyIndentPref	pref;
			AText	text;
			GetData_TextArray(kIndent_RegExp,i,text);
			text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			text.LimitLengthAsSJIS(0,63);
			text.GetPstring(pref.regexp,63);
			pref.currentIndent = GetData_NumberArray(kIndent_OffsetCurrentLine,i);
			pref.nextIndent = GetData_NumberArray(kIndent_OffsetNextLine,i);
			**res = pref;
			
			//R-ub
			::HLock((Handle)res);
			AUniversalBinaryUtility::SwapHostToBig((**res).currentIndent);
			AUniversalBinaryUtility::SwapHostToBig((**res).nextIndent);
			::HUnlock((Handle)res);
			
			AddResource((Handle)res,'IDNT',2000+i,"\p");
			WriteResource((Handle)res);
			ReleaseResource((Handle)res);
		}
	}
}
#endif

void	AModePrefDB::LoadMacLegacy_Correspond( short inResRefNum )
{
	short	rescount = 0;
	SLegacyTaioPrefHeader	**res = (SLegacyTaioPrefHeader**)Get1Resource(inResRefNum,'TAIO',1000);
	if( res != NULL ) 
	{
		if( GetHandleSize((Handle)res) == sizeof(SLegacyTaioPrefHeader) ) 
		{
			rescount = (**res).count;
			//R-ub
			AUniversalBinaryUtility::SwapBigToHost(rescount);
		}
		::ReleaseResource((Handle)res);
	}
	if( rescount != 0 ) 
	{
		for( short i = 0; i < rescount; i++ ) 
		{
			SLegacyTaioPref	**res = (SLegacyTaioPref**)Get1Resource(inResRefNum,'TAIO',2000+i);
			if( res != NULL ) 
			{
				AText	text;
				text.SetPstring((**res).start);
				Add_TextArray(kCorrespond_StartText,text);
				text.SetPstring((**res).end);
				Add_TextArray(kCorrespond_EndText,text);
				
				::ReleaseResource((Handle)res);
			}
		}
	}
}

//#844
#if 0
void	AModePrefDB::WriteMacLegacy_Correspond( short inResRefNum )
{
	SLegacyTaioPrefHeader	**res = (SLegacyTaioPrefHeader**)Get1Resource(inResRefNum,'TAIO',1000);
	if( res != NULL ) 
	{
		RemoveResource((Handle)res);
		DisposeHandle((Handle)res);
	}
	res = (SLegacyTaioPrefHeader**)NewHandle(sizeof(SLegacyTaioPrefHeader));
	if( res != NULL ) 
	{
		(**res).count = GetItemCount_TextArray(kCorrespond_StartText);
		
		//R-ub
		::HLock((Handle)res);
		AUniversalBinaryUtility::SwapHostToBig((**res).count);
		::HUnlock((Handle)res);
		
		AddResource((Handle)res,'TAIO',1000,"\p");
		WriteResource((Handle)res);
		ReleaseResource((Handle)res);
	}
	for( AIndex i = 0; i < GetItemCount_TextArray(kCorrespond_StartText); i++ ) 
	{
		SLegacyTaioPref	**res = (SLegacyTaioPref**)Get1Resource(inResRefNum,'TAIO',2000+i);
		if( res != NULL ) 
		{
			RemoveResource((Handle)res);
			DisposeHandle((Handle)res);
		}
		res = (SLegacyTaioPref**)NewHandle(sizeof(SLegacyTaioPref));
		if( res != NULL ) 
		{
			AText	text;
			GetData_TextArray(kCorrespond_StartText,i,text);
			text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			text.LimitLengthAsSJIS(0,63);
			text.GetPstring((**res).start,63);
			
			GetData_TextArray(kCorrespond_EndText,i,text);
			text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			text.LimitLengthAsSJIS(0,63);
			text.GetPstring((**res).end,63);
			
			AddResource((Handle)res,'TAIO',2000+i,"\p");
			WriteResource((Handle)res);
			ReleaseResource((Handle)res);
		}
	}
}
#endif

void	AModePrefDB::LoadMacLegacy_DragDrop( short inResRefNum )
{
	short	rescount = 0;
	SLegacyDragDropPrefHeader	**res = (SLegacyDragDropPrefHeader**)Get1Resource(inResRefNum,'DROP',1000);
	if( res != NULL ) 
	{
		if( GetHandleSize((Handle)res) == sizeof(SLegacyDragDropPrefHeader) ) 
		{
			rescount = (**res).dragDropPrefCount;
			//R-ub
			AUniversalBinaryUtility::SwapBigToHost(rescount);
		}
		ReleaseResource((Handle)res);
	}
	if( rescount != 0 ) 
	{
		for( short i = 0; i < rescount; i++ ) 
		{
			//R-uni UTF-8は3000番台
			SLegacyDragDropPref	**res = (SLegacyDragDropPref**)Get1Resource(inResRefNum,'DROP',3000+i);
			if( res != NULL ) 
			{
				SLegacyDragDropPref	pref = **res;
				
				AText	text;
				text.SetPstring(pref.suffix);
				Add_TextArray(kDragDrop_Suffix,text);
				text.SetPstring(pref.text);
				Add_TextArray(kDragDrop_Text,text);
				
				ReleaseResource((Handle)res);
			}
			else
			{
				res = (SLegacyDragDropPref**)Get1Resource(inResRefNum,'DROP',2000+i);
				if( res != NULL ) 
				{
					SLegacyDragDropPref	pref = **res;
					AText	text;
					text.SetPstring(pref.suffix);
					Add_TextArray(kDragDrop_Suffix,text);
					text.SetPstring(pref.text);
					text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
					Add_TextArray(kDragDrop_Text,text);
					
					ReleaseResource((Handle)res);
				}
			}
		}
	}
}

//#844
#if 0
void	AModePrefDB::WriteMacLegacy_DragDrop( short inResRefNum )
{
	SLegacyDragDropPrefHeader	**res = (SLegacyDragDropPrefHeader**)Get1Resource(inResRefNum,'DROP',1000);
	if( res != NULL ) 
	{
		RemoveResource((Handle)res);
		DisposeHandle((Handle)res);
	}
	res = (SLegacyDragDropPrefHeader**)NewHandle(sizeof(SLegacyDragDropPrefHeader));
	if( res != NULL ) 
	{
		(**res).dragDropPrefCount = GetItemCount_TextArray(kDragDrop_Suffix);
		
		//R-ub
		::HLock((Handle)res);
		AUniversalBinaryUtility::SwapHostToBig((**res).dragDropPrefCount);
		::HUnlock((Handle)res);
		
		AddResource((Handle)res,'DROP',1000,"\p");
		WriteResource((Handle)res);
		ReleaseResource((Handle)res);
	}
	for( AIndex i = 0; i < GetItemCount_TextArray(kDragDrop_Suffix); i++ ) 
	{
		//UTF8 3000番台
		SLegacyDragDropPref	**res = (SLegacyDragDropPref**)Get1Resource(inResRefNum,'DROP',3000+i);
		if( res != NULL ) 
		{
			RemoveResource((Handle)res);
			DisposeHandle((Handle)res);
		}
		res = (SLegacyDragDropPref**)NewHandle(sizeof(SLegacyDragDropPref));
		if( res != NULL ) 
		{
			SLegacyDragDropPref	pref;
			
			AText	text;
			GetData_TextArray(kDragDrop_Suffix,i,text);
			text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			text.LimitLengthAsSJIS(0,31);
			text.GetPstring(pref.suffix,31);
			
			GetData_TextArray(kDragDrop_Text,i,text);
			text.LimitLength(0,255);
			text.GetPstring(pref.text,255);
			
			**res = pref;
			AddResource((Handle)res,'DROP',3000+i,"\p");
			WriteResource((Handle)res);
			ReleaseResource((Handle)res);
		}
		//SJIS 2000番台
		res = (SLegacyDragDropPref**)Get1Resource(inResRefNum,'DROP',2000+i);
		if( res != NULL ) 
		{
			RemoveResource((Handle)res);
			DisposeHandle((Handle)res);
		}
		res = (SLegacyDragDropPref**)NewHandle(sizeof(SLegacyDragDropPref));
		if( res != NULL ) 
		{
			SLegacyDragDropPref	pref;
			
			AText	text;
			GetData_TextArray(kDragDrop_Suffix,i,text);
			text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			text.LimitLengthAsSJIS(0,31);
			text.GetPstring(pref.suffix,31);
			
			GetData_TextArray(kDragDrop_Text,i,text);
			text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			text.LimitLengthAsSJIS(0,255);
			text.GetPstring(pref.text,255);
			
			**res = pref;
			AddResource((Handle)res,'DROP',2000+i,"\p");
			WriteResource((Handle)res);
			ReleaseResource((Handle)res);
		}
	}
	
}
#endif

void	AModePrefDB::LoadMacLegacy_GetPrefDefault( SMacLegacyModePref& outPref )
{
	outPref.font = 0;//#1012 ::GetAppFont();
	outPref.size = 9;
	outPref.windowwidth = 400;
	outPref.windowheight = 200;
	outPref.tabwidth = 4;
	outPref.kanjicode = kanjiCode_MacCode;
	outPref.returncode = returnCode_CR;
	outPref.wrapmode = wrapMode_NoWrap;
	outPref.creator = 'MMKE';
	outPref.printForm.usePrintFont = true;
	outPref.printForm.printPageNumber = true;
	outPref.printForm.printFileName = true;
	outPref.printForm.printLineNumber = true;
	outPref.printForm.printFont =0;//#1012 ::GetAppFont();
	outPref.printForm.printSize = 9;
	outPref.printForm.pageFont = 0;//#1012 ::GetAppFont();
	outPref.printForm.pageSize = 9;
	outPref.printForm.pagePosition = printPosition_RightTop;
	outPref.printForm.fileFont = 0;//#1012 ::GetAppFont();
	outPref.printForm.fileSize = 9;
	outPref.printForm.lineFont = 0;//#1012 ::GetAppFont();
	outPref.printForm.printSeparateLine = true;
	outPref.printForm.wrapMode = wrapMode_WordWrap;
	outPref.printForm.betweenLine = 0;
	/*TPrint	**dpr = UPrintingMgr::GetDefaultPrintRecord();
	if( dpr != NULL ) {
		outPref.printRecord = **dpr;
	}*/
	outPref.backfilename[0] = 0;
	outPref.background = false;
	outPref.displayEachLineNumber = true;
	outPref.displayEachParagraphNumber = false;
	outPref.displayCurrentLineNumber = true;
	outPref.displayCurrentParagraphNumber = false;
	outPref.displayHandFreeScroll = true;
	outPref.displayInfoTag = true;
	outPref.kakkoTaio = true;
	outPref.insideKakkoSelection = true;
	outPref.mimikakiWrap = false;
	outPref.jumpScroll = false;
	outPref.jumpScrollStart = 5;
	outPref.jumpScrollLines = 3;
	outPref.forceRoman = false;
	outPref.useDragDrop = true;
	outPref.newDocumentDialog = false;
	outPref.displayCurrentPosition = true;
	outPref.displaySpecialCharacter = false;
	outPref.LCWrap = false;
	outPref.LCWrapCount = 80;
	outPref.displayRuler = false;
	outPref.rulerScale = rulerScale_Tab;
	outPref.rulerLetter = ' ';
	outPref.tripleClick2 = false;
	outPref.tripleClick2String[0] = 0;
	outPref.betweenLine = 0;
	outPref.forceMimi = false;
	outPref.keyBind = false;
	outPref.smartDrag = false;
    CopyPstring(outPref.alphabetString,"\pABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
	outPref.headAlphabetString[0] = 0;
	outPref.tailAlphabetString[0] = 0;
	CopyPstring(outPref.rulerString,"\p ");
	RGBColor	color;
	color.red = color.green = color.blue = 50000;
	outPref.specialCharacterColor = color;
	CopyPstring(outPref.commentStart,"\p");
	CopyPstring(outPref.commentEnd,"\p");
	CopyPstring(outPref.commentLine,"\p");
	CopyPstring(outPref.literalStart1,"\p");
	CopyPstring(outPref.literalEnd1,"\p");
	CopyPstring(outPref.literalStart2,"\p");
	CopyPstring(outPref.literalEnd2,"\p");
	CopyPstring(outPref.literalEscape,"\p");
	CopyPstring(outPref.otherStart1,"\p");
	CopyPstring(outPref.otherEnd1,"\p");
	CopyPstring(outPref.otherStart2,"\p");
	CopyPstring(outPref.otherEnd2,"\p");
	color.red = 65535;
	color.green = color.blue = 0;
	outPref.commentColor = color;
	color.red = color.green = color.blue = 30000;
	outPref.literalColor = color;
	for( short i = 0; i < 128; i++ )   outPref.indentChar[i] = 0;
	outPref.recognizeC = false;
	outPref.indentC = false;
	color.red = color.green = color.blue = 0;
	outPref.prefC.recognizeGlobal = true;
	outPref.prefC.recognizeLocal = true;
	outPref.prefC.recognizeExternal = true;
	outPref.prefC.useIdentifierResource = true;
	outPref.prefC.displayInfoTag = true;
	outPref.prefC.useTRTemplate = false;
	outPref.prefC.displayLineNumber = true;
	outPref.prefC.displayFunction = true;
	outPref.prefC.displayEachLineNumber = false;
	outPref.prefC.normalIndent = 4;
	outPref.prefC.labelIndent = -2;
	outPref.prefC.fukubunIndent = 8;
	outPref.prefC.functionColor = color;
	outPref.prefC.externalFunctionColor = color;
	outPref.prefC.classColor = color;
	outPref.prefC.globalVariableColor = color;
	outPref.prefC.localVariableColor = color;
	outPref.prefC.externalVariableColor = color;
	outPref.prefC.commentColor = color;
	outPref.prefC.literalColor = color;
	outPref.prefC.enumtypeColor = color;
	outPref.prefC.enumColor = color;
	outPref.prefC.typedefColor = color;
	outPref.prefC.structColor = color;
	outPref.prefC.unionColor = color;
	outPref.prefC.defineColor = color;
	outPref.displayToolBar = true;
	outPref.countReturn = false;
	outPref.countSpace = true;
	outPref.countReserve1 = false;
	outPref.countReserve2 = false;
	outPref.countReserve3 = false;
	outPref.countReserve4 = false;
	outPref.backImagePath[0] = 0;
	outPref.shadowText = false;
	outPref.displayZenkakuSpace = false;
	outPref.attachDOSEOF = false;
	outPref.displayOpenWell = true;
	outPref.spacedTab = false;  
	outPref.masume = false;
	outPref.normalColor.red = outPref.normalColor.green = outPref.normalColor.blue = 0;
	outPref.backColor.red = outPref.backColor.green = outPref.backColor.blue = 65535;
	outPref.saveWindowPosition = false;
	outPref.texCompiler.vRefNum = outPref.texCompiler.parID = 0;
	CopyPstring(outPref.texCompiler.name,"\p");//B0089
	outPref.texPreviewer.vRefNum = outPref.texPreviewer.parID = 0;
	outPref.attachUTF8BOM = false;//B0034
	outPref.dontSaveResource = false;
	outPref.saveYenAsBackslash = false;//B0014,B0035 040215デフォルトはfalseに変更
	//2.1.4
	outPref.OBSOLETE_internalEncoding = kTextEncodingShiftJIS_X0213_00;//R0043
	outPref.useUnicode = false;//R0043
	outPref.useDefaultEncoding = true;
	outPref.defaultEncoding = kanjiCode_MacCode;
	outPref.displaySpace = false;//R0038
	outPref.displayTab = false;//R0027
	outPref.displayReturn = false;
	//2.1.6b1
	CopyPstring(outPref.texCompilerPath,"\p/usr/local/bin/platex");
	outPref.useMiWordSelect = false;//B0159
	outPref.charsetRecognize = true;
	outPref.sniffRecognize = true;
	outPref.correctEncoding = false;//R0089
	outPref.checkCharsetAfterSave = true;
	//2.1.6b3
	outPref.defalutOpenJumpList = false;//R0047
	//2.1.7b1
	outPref.changeYenToBackslash = true;//R-uni
	outPref.antiAliasMinSize = 0;//R-uni//B0124
	outPref.defaultTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();//R-uni
	outPref.useLegacySniff = true;//R-uni
	outPref.useBOMSniff = true;//R-uni
	outPref.displayYenFor5CWhenJIS = false;//R-uni
	outPref.save5CForYenWhenJIS = true;//R-uni
	CopyPstring(outPref.rulerStringUTF8,"\p ");//R-uni
}

void	AModePrefDB::LoadMacLegacy_Pref( short inResRefNum )
{
	SMacLegacyModePref	pref;
	LoadMacLegacy_GetPrefDefault(pref);
	
	//まずBigEndianへ
	LoadMacLegacy_ConvertPrefEndianToBig(pref);
	
	EPrefVersion ver = GetPrefVersion(inResRefNum);
	switch(ver) 
	{
	  case prefVersion_217b6:
	  case prefVersion_217b1:
		{
			/*R0128 SMacLegacyModePref	**res = (SMacLegacyModePref**)Get1Resource(rRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
			pref = **res;
			ReleaseResource((Handle)res);
			}
			break;*/
			SMacLegacyModePref217	**res = (SMacLegacyModePref217**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
				for( long i = 0; i < sizeof(SMacLegacyModePref217); i++ ) 
				{
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			break;
		}
	  case prefVersion_216b5:
	  case prefVersion_216b3://R0061
		{
			SMacLegacyModePref216b5	**res = (SMacLegacyModePref216b5**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
				for( long i = 0; i < sizeof(SMacLegacyModePref216b5); i++ ) 
				{
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			break;
		}
	  case prefVersion_216b1://B0089
		{
			SMacLegacyModePref216b1	**res = (SMacLegacyModePref216b1**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
				for( long i = 0; i < sizeof(SMacLegacyModePref216b1); i++ ) 
				{
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			break;
		}
	  case prefVersion_215://R0043
		{
			SMacLegacyModePref215	**res = (SMacLegacyModePref215**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
				for( long i = 0; i < sizeof(SMacLegacyModePref215); i++ ) 
				{
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			break;
		}
	  case prefVersion_213://B0034
		{
			SMacLegacyModePref213	**res = (SMacLegacyModePref213**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
				for( long i = 0; i < sizeof(SMacLegacyModePref213); i++ ) 
				{
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			break;
		}
	  case prefVersion_21b13:
		{
			SMacLegacyModePref21b13	**res = (SMacLegacyModePref21b13**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
				for( long i = 0; i < sizeof(SMacLegacyModePref21b13); i++ ) 
				{
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			break;
		}
	  case prefVersion_21b9:
		{
			SMacLegacyModePref21b9	**res = (SMacLegacyModePref21b9**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
				for( long i = 0; i < sizeof(SMacLegacyModePref21b9); i++ ) 
				{
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			break;
		}
	  case prefVersion_21b8:
		{
			SMacLegacyModePref21b8	**res = (SMacLegacyModePref21b8**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
				for( long i = 0; i < sizeof(SMacLegacyModePref21b8); i++ ) 
				{
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			pref.attachDOSEOF = false;
			break;
		}
	  case prefVersion_21b5:
		{
			SMacLegacyModePref21b5	**res = (SMacLegacyModePref21b5**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
				for( long i = 0; i < sizeof(SMacLegacyModePref21b5); i++ ) 
				{
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			pref.attachDOSEOF = false;
			break;
		}
	  case prefVersion_21b3:
		{
			SMacLegacyModePref21b3	**res = (SMacLegacyModePref21b3**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) 
			{
				for( long i = 0; i < sizeof(SMacLegacyModePref21b3); i++ ) 
				{
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			pref.attachDOSEOF = false;
			break;
		}
	  case prefVersion_21b1:
		{
			SMacLegacyModePref21b2	**res = (SMacLegacyModePref21b2**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) {
				for( long i = 0; i < sizeof(SMacLegacyModePref21b2); i++ ) {
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				ReleaseResource((Handle)res);
			}
			CopyPstring(pref.otherStart1,"\p");
			pref.attachDOSEOF = false;
			break;
		}
	  case prefVersion_20b9:
		{
			SMacLegacyModePref20b9	**res = (SMacLegacyModePref20b9**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) {
				//B0145 if( GetHandleSize((Handle)res) == sizeof(SMacLegacyModePref20b9) ) {
				for( long i = 0; i < sizeof(SMacLegacyModePref20b9); i++ ) {
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				//B0145 }
				ReleaseResource((Handle)res);
			}
			break;
		}
	  case prefVersion_20b7:
		{
			SMacLegacyModePref20b7	**res = (SMacLegacyModePref20b7**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) {
				//B0145 if( GetHandleSize((Handle)res) == sizeof(SMacLegacyModePref20b7) ) {
				for( long i = 0; i < sizeof(SMacLegacyModePref20b7); i++ ) {
					*(((Ptr)(&pref))+i) = *(((Ptr)(*res))+i);
				}
				//B0145 }
				pref.rulerString[1] = pref.rulerLetter;
				ReleaseResource((Handle)res);
			}
			break;
		}
	  case prefVersion_20b6:
		{
			SMacLegacyModePref20b6	**res = (SMacLegacyModePref20b6**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) {
				//B0145 if( GetHandleSize((Handle)res) == sizeof(SMacLegacyModePref20b6) ) {
				pref.font = (**res).font;
				pref.size = (**res).size;
				pref.windowwidth = (**res).windowwidth;
				pref.windowheight = (**res).windowheight;
				pref.tabwidth = (**res).tabwidth;
				pref.kanjicode = (**res).kanjicode;
				pref.returncode = (**res).returncode;
				pref.wrapmode = (**res).wrapmode;
				pref.creator = (**res).creator;
				pref.printForm.usePrintFont = (**res).printForm.usePrintFont;
				pref.printForm.printPageNumber = (**res).printForm.printPageNumber;
				pref.printForm.printFileName = (**res).printForm.printFileName;
				pref.printForm.printLineNumber = (**res).printForm.printLineNumber;
				pref.printForm.printFont = (**res).printForm.printFont;
				pref.printForm.printSize = (**res).printForm.printSize;
				pref.printForm.pageFont = (**res).printForm.pageFont;
				pref.printForm.pageSize = (**res).printForm.pageSize;
				pref.printForm.pagePosition = (**res).printForm.pagePosition;
				pref.printForm.fileFont = (**res).printForm.fileFont;
				pref.printForm.fileSize = (**res).printForm.fileSize;
				pref.printForm.lineFont = (**res).printForm.lineFont;
				pref.printForm.printSeparateLine = (**res).printForm.printSeparateLine;
				pref.printForm.wrapMode = (**res).printForm.wrapMode;
				/*TPrint	**dpr = UPrintingMgr::GetDefaultPrintRecord();
				if( dpr != NULL ) {
				pref.printRecord = **dpr;
				}*/
				CopyPstring(pref.backfilename,(**res).backfilename);
				pref.background = (**res).background;
				pref.displayEachLineNumber = (**res).displayEachLineNumber;
				pref.displayEachParagraphNumber = (**res).displayEachParagraphNumber;
				pref.displayCurrentLineNumber = (**res).displayCurrentLineNumber;
				pref.displayCurrentParagraphNumber = (**res).displayCurrentParagraphNumber;
				pref.displayHandFreeScroll = (**res).displayHandFreeScroll;
				pref.displayInfoTag = (**res).displayInfoTag;
				pref.kakkoTaio = (**res).kakkoTaio;
				pref.insideKakkoSelection = (**res).insideKakkoSelection;
				pref.mimikakiWrap = (**res).mimikakiWrap;
				pref.jumpScroll = (**res).jumpScroll;
				pref.jumpScrollStart = (**res).jumpScrollStart;
				pref.jumpScrollLines = (**res).jumpScrollLines;
				pref.forceRoman = (**res).forceRoman;
				pref.useDragDrop = (**res).useDragDrop;
				pref.newDocumentDialog = (**res).newDocumentDialog;
				pref.displayCurrentPosition = (**res).displayCurrentPosition;
				pref.displaySpecialCharacter = (**res).displaySpecialCharacter;
				pref.LCWrap = (**res).LCWrap;
				pref.LCWrapCount = (**res).LCWrapCount;
				pref.displayRuler = (**res).displayRuler;
				pref.rulerScale = (**res).rulerScale;
				pref.rulerLetter = (**res).rulerLetter;
				pref.tripleClick2 = (**res).tripleClick2;
				CopyPstring(pref.tripleClick2String,(**res).tripleClick2String);
				pref.betweenLine = 0;
				pref.printForm.betweenLine = 0;
				pref.forceMimi = false;
				CopyPstring(pref.alphabetString,"\pABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
				pref.headAlphabetString[0] = 0;
				pref.keyBind = false;
				pref.rulerString[1] = pref.rulerLetter;
				//B0145 }
				ReleaseResource((Handle)res);
			}
			break;
		}			
	  case prefVersion_Less20b5:
		{
			SMacLegacyModePref20b5	**res = (SMacLegacyModePref20b5**)Get1Resource(inResRefNum,'MMKP',1000);
			if( res != NULL ) {
				//B0145 if( GetHandleSize((Handle)res) == sizeof(SMacLegacyModePref20b5) ) {
				pref.font = (**res).font;
				pref.size = (**res).size;
				pref.windowwidth = (**res).windowwidth;
				pref.windowheight = (**res).windowheight;
				pref.tabwidth = (**res).tabwidth;
				pref.kanjicode = (**res).kanjicode;
				pref.returncode = (**res).returncode;
				pref.wrapmode = (**res).wrapmode;
				pref.creator = (**res).creator;
				pref.printForm.usePrintFont = (**res).printForm.usePrintFont;
				pref.printForm.printPageNumber = (**res).printForm.printPageNumber;
				pref.printForm.printFileName = (**res).printForm.printFileName;
				pref.printForm.printLineNumber = (**res).printForm.printLineNumber;
				pref.printForm.printFont = (**res).printForm.printFont;
				pref.printForm.printSize = (**res).printForm.printSize;
				pref.printForm.pageFont = (**res).printForm.pageFont;
				pref.printForm.pageSize = (**res).printForm.pageSize;
				pref.printForm.pagePosition = (**res).printForm.pagePosition;
				pref.printForm.fileFont = (**res).printForm.fileFont;
				pref.printForm.fileSize = (**res).printForm.fileSize;
				pref.printForm.lineFont = (**res).printForm.lineFont;
				pref.printForm.printSeparateLine = (**res).printForm.printSeparateLine;
				pref.printForm.wrapMode = (**res).printForm.wrapMode;
				/*TPrint	**dpr = UPrintingMgr::GetDefaultPrintRecord();
				if( dpr != NULL ) {
				pref.printRecord = **dpr;
				}*/
				CopyPstring(pref.backfilename,(**res).backfilename);
				pref.background = (**res).background;
				pref.displayEachLineNumber = (**res).displayEachLineNumber;
				pref.displayEachParagraphNumber = (**res).displayEachParagraphNumber;
				pref.displayCurrentLineNumber = (**res).displayCurrentLineNumber;
				pref.displayCurrentParagraphNumber = (**res).displayCurrentParagraphNumber;
				pref.displayHandFreeScroll = (**res).displayHandFreeScroll;
				pref.displayInfoTag = (**res).displayInfoTag;
				pref.kakkoTaio = (**res).kakkoTaio;
				pref.insideKakkoSelection = (**res).insideKakkoSelection;
				pref.mimikakiWrap = (**res).mimikakiWrap;
				pref.jumpScroll = (**res).jumpScroll;
				pref.jumpScrollStart = (**res).jumpScrollStart;
				pref.jumpScrollLines = (**res).jumpScrollLines;
				pref.forceRoman = (**res).forceRoman;
				pref.useDragDrop = (**res).useDragDrop;
				pref.newDocumentDialog = (**res).newDocumentDialog;
				pref.displayCurrentPosition = (**res).displayCurrentPosition;
				pref.displaySpecialCharacter = (**res).displaySpecialCharacter;
				pref.LCWrap = false;
				pref.LCWrapCount = 80;
				pref.displayRuler = false;
				pref.rulerScale = rulerScale_Tab;
				pref.rulerLetter = ' ';
				pref.tripleClick2 = false;
				pref.tripleClick2String[0] = 0;
				pref.betweenLine = 0;
				pref.printForm.betweenLine = 0;
				pref.forceMimi = false;
				pref.alphabetString[0] = 0;
				pref.headAlphabetString[0] = 0;
				pref.keyBind = false;
				pref.rulerString[1] = pref.rulerLetter;
				//B0145 }
				ReleaseResource((Handle)res);
			}
			break;
		}
	  default:
		{
			//R-abs UClassicAlert::DoAlert("\pCannot decide version of mode file","\p");//B0145
			fprintf(stderr,"Cannot decide version of mode file.\n");
		}
		break;
	}
	
	//R-ub
	LoadMacLegacy_ConvertPrefEndianToHost(pref);
			
	if( ver <= prefVersion_213 )//R0027
	{
		pref.displayReturn = pref.displaySpecialCharacter;
		pref.displayTab = pref.displaySpecialCharacter;
	}
	if( ver <= prefVersion_215 )//B0089
	{
		if( pref.texCompiler.vRefNum == 0 && pref.texCompiler.parID == 0 )
		{
			CopyPstring(pref.texCompilerPath,pref.texCompiler.name);
		}
		else
		{
			CopyPstring(pref.texCompilerPath,"\p");
		}
	}
	//R-uni
	if( ver < prefVersion_217b1 )
	{
		AText	text;
		text.SetPstring(pref.rulerString);
		text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
		text.GetPstring(pref.rulerStringUTF8,255);
		
		//B0132
		pref.defaultTextEncoding = AMacLegacyPrefUtil::GetTextEncodingFromKanjiCode(pref.kanjicode,pref.font);
	}
	if( pref.kanjicode == kanjiCode_MacRoman || pref.kanjicode == kanjiCode_MacCyrillic || 
			pref.kanjicode == kanjiCode_MacGreek || pref.kanjicode == kanjiCode_MacTurkish )
	{
		pref.kanjicode = kanjiCode_MacCode;
	}
	//R-uni Mode設定のデフォルトデータが設定メニュー中に存在するとは限らないので存在しなければUTF8に強制設定
	//#844 常にUTF8にする if( GetApp().GetAppPref().ExistTextEncodingMenu(pref.defaultTextEncoding) == true )
	{
		pref.defaultTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
	}
	
	//R-uni
	if( pref.wrapmode == wrapMode_LetterWrap )
	{
		pref.wrapmode = wrapMode_WordWrap;
	}
	if( pref.printForm.wrapMode == wrapMode_LetterWrap )
	{
		pref.printForm.wrapMode = wrapMode_WordWrap;
	}
	
	//R-uni
	pref.displayOpenWell = true;
	
	/* 設定フェール・セーフ */
	Str255	str;
	//#1012 ::GetFontName(pref.font,str);
	str[0] = 0;//#1012
	if( str[0] == 0 )   pref.font = 0;
	if( pref.size < 3 )   pref.size = 3;
	if( pref.size > 255 )   pref.size = 255;
	if( pref.windowwidth < 400 )   pref.windowwidth = 400;
	if( pref.windowwidth > 3000 )   pref.windowwidth = 640;
	if( pref.windowheight < 150 )   pref.windowheight = 150;
	if( pref.windowheight > 2000 )   pref.windowheight = 600;	
	if( pref.jumpScrollLines > 100 )   pref.jumpScrollLines = 3;
	if( pref.jumpScrollLines < 1 )   pref.jumpScrollLines = 1;
	if( pref.LCWrapCount < 5 )   pref.LCWrapCount = 5;
	if( pref.betweenLine > 100 )   pref.betweenLine = 0;
	
	pref.displayHandFreeScroll = false;
	
	//B0278 文字数Wrapの文字数は見た目に合わせるように変更→旧バージョンの場合は-2する。（旧Verの42＝新Verの40）
	pref.LCWrapCount -= 2;//この時点でのデータは旧リソースから読んだ値なので-2する。
	//新バージョンのデータはXMLベースなので、LoadFromPreFileで読み込まれる。（XMLデータが存在すれば、新keyの値で上書きされる）
	
	SetData_Bool(kDefaultEnableAntiAlias,			(pref.antiAliasMinSize>=0));
	SetData_Bool(kDisplayBackgroundImage,			pref.background);
	SetData_Bool(kDisplayInformationHeader,			pref.displayInfoTag);
	SetData_Bool(kDisplayToolbar,					pref.displayToolBar);
	SetData_Bool(kDisplayPositionButton,			pref.displayCurrentPosition);
	SetData_Bool(kDisplayJumpList,					pref.defalutOpenJumpList);
	SetData_Bool(kDisplayRuler,						pref.displayRuler);
	SetData_Bool(kRulerScaleWithTabLetter,			(pref.rulerScale==rulerScale_Tab));
	SetData_Bool(kDisplayReturnCode,				pref.displayReturn);
	SetData_Bool(kDisplayTabCode,					pref.displayTab);
	SetData_Bool(kDisplaySpaceCode,					pref.displaySpace);
	SetData_Bool(kDisplayZenkakuSpaceCode,			pref.displayZenkakuSpace);
	SetData_Bool(kDisplayGenkoyoshi,				pref.masume);
	SetData_Bool(kCheckBrace,						pref.kakkoTaio);
	SetData_Bool(kSelectInsideBraceByDoubleClick,	pref.insideKakkoSelection);
	SetData_Bool(kSelectInsideByQuadClick,			pref.tripleClick2);
	SetData_Bool(kKinsokuBuraasge,					pref.mimikakiWrap);
	SetData_Bool(kInputSpacesByTabKey,				pref.spacedTab);
	SetData_Bool(kAddRemoveSpaceByDragDrop,			pref.smartDrag);
	SetData_Bool(kInputBackslashByYenKey,			pref.changeYenToBackslash);
	SetData_Bool(kDontSaveResouceFork,				pref.dontSaveResource);
	SetData_Bool(kSaveUTF8BOM,						pref.attachUTF8BOM);
	SetData_Bool(kCheckCharsetWhenSave,				pref.checkCharsetAfterSave);
	SetData_Bool(kSaveDOSEOF,						pref.attachDOSEOF);
	SetData_Bool(kSaveWindowPosition,				pref.saveWindowPosition);
	SetData_Bool(kSniffTextEncoding_BOM,			pref.useBOMSniff);
	SetData_Bool(kSniffTextEncoding_Charset,		pref.charsetRecognize);
	SetData_Bool(kSniffTextEncoding_Legacy,			pref.useLegacySniff);
	SetData_Bool(kShowDialogWhenTextEncodingNotDecided,pref.correctEncoding);
	SetData_Bool(kApplyCharsForWordToDoubleClick,	pref.useMiWordSelect);
	SetData_Bool(kUseCIndent,						pref.indentC);
	SetData_Bool(kUseCSyntaxColor,					pref.recognizeC);
	
	SetData_Bool(kDisplayLineNumberButton,			(pref.displayCurrentLineNumber||pref.displayCurrentParagraphNumber));
	SetData_Bool(kDisplayLineNumberButton_AsParagraph,pref.displayCurrentParagraphNumber);
	SetData_Bool(kDisplayEachLineNumber,			(pref.displayEachLineNumber||pref.displayEachParagraphNumber));
	SetData_Bool(kDisplayEachLineNumber_AsParagraph,pref.displayEachParagraphNumber);
	
	SetData_Bool(kDefaultPrintOption_UsePrintFont,	pref.printForm.usePrintFont);
	SetData_Bool(kDefaultPrintOption_PrintPageNumber,pref.printForm.printPageNumber);
	SetData_Bool(kDefaultPrintOption_PrintFileName,	pref.printForm.printFileName);
	SetData_Bool(kDefaultPrintOption_PrintLineNumber,pref.printForm.printLineNumber);
	SetData_Bool(kDefaultPrintOption_PrintSeparateLine,pref.printForm.printSeparateLine);
	SetData_Bool(kDefaultPrintOption_ForceWordWrap,	(pref.printForm.wrapMode == wrapMode_WordWrap));
	
	SetData_Color(kLetterColor,						pref.normalColor);
	SetData_Color(kBackgroundColor,					pref.backColor);
	SetData_Color(kSpecialCodeColor,				pref.specialCharacterColor);
	SetData_Color(kCommentColor,					pref.commentColor);
	SetData_Color(kLiteralColor,					pref.literalColor);
	
	SetData_Color(kCSyntaxColor_Function,			pref.prefC.functionColor);
	SetData_Color(kCSyntaxColor_ExternalFunction,	pref.prefC.externalFunctionColor);
	SetData_Color(kCSyntaxColor_Class,				pref.prefC.classColor);
	SetData_Color(kCSyntaxColor_GlobalVariable,		pref.prefC.globalVariableColor);
	SetData_Color(kCSyntaxColor_LocalVariable,		pref.prefC.localVariableColor);
	SetData_Color(kCSyntaxColor_ExternalVariable,	pref.prefC.externalVariableColor);
	SetData_Color(kCSyntaxColor_EnumType,			pref.prefC.enumtypeColor);
	SetData_Color(kCSyntaxColor_Enum,				pref.prefC.enumColor);
	SetData_Color(kCSyntaxColor_Typedef,			pref.prefC.typedefColor);
	SetData_Color(kCSyntaxColor_Struct,				pref.prefC.structColor);
	SetData_Color(kCSyntaxColor_Union,				pref.prefC.unionColor);
	SetData_Color(kCSyntaxColor_Define,				pref.prefC.defineColor);
	
	AText	text;
	text.SetPstring(pref.rulerStringUTF8);
	SetData_Text(kRulerBaseLetter,					text);
	text.SetPstring(pref.tripleClick2String);
	SetData_Text(kQuadClickText,					text);
	text.SetPstring(pref.commentStart);
	
	SetData_Text(kCommentStart,						text);
	text.SetPstring(pref.commentEnd);
	SetData_Text(kCommentEnd,						text);
	text.SetPstring(pref.commentLine);
	SetData_Text(kCommentLineStart,					text);
	text.SetPstring(pref.literalStart1);
	SetData_Text(kLiteralStart1,					text);
	text.SetPstring(pref.literalEnd1);
	SetData_Text(kLiteralEnd1,						text);
	text.SetPstring(pref.literalStart2);
	SetData_Text(kLiteralStart2,					text);
	text.SetPstring(pref.literalEnd2);
	SetData_Text(kLiteralEnd2,						text);
	text.SetPstring(pref.literalEscape);
	SetData_Text(kEscapeString,						text);
	text.SetPstring(pref.otherStart1);
	SetData_Text(kOtherStart1,						text);
	text.SetPstring(pref.otherEnd1);
	SetData_Text(kOtherEnd1,						text);
	text.SetPstring(pref.otherStart2);
	SetData_Text(kOtherStart2,						text);
	text.SetPstring(pref.otherEnd2);
	SetData_Text(kOtherEnd2,						text);
	
	text.SetPstring(pref.alphabetString);
	SetData_Text(kCharsForWord,						text);
	text.SetPstring(pref.headAlphabetString);
	SetData_Text(kCharsForHeadOfWord,				text);
	text.SetPstring(pref.tailAlphabetString);
	SetData_Text(kCharsForTailOfWord,				text);
	
	text.SetPstring(pref.backImagePath);
	text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	SetData_Text(kBackgroundImageFile,				text);
	
	text.SetPstring(pref.texCompilerPath);
	SetData_Text(kTeXCompiler,						text);
	FSSpec	fsspec;//#259
	::FSMakeFSSpec(pref.texPreviewer.vRefNum,pref.texPreviewer.parID,pref.texPreviewer.name,&fsspec);
	AFileAcc	file;
	file.SpecifyByFSSpec(fsspec);
	file.GetPath(text);
	SetData_Text(kTeXPreviewer,						text);
	
	//#688 /*win SetData_Font*/SetData_Number(/*win kDefaultFont*/kDefaultFont_compat,pref.font);
	SetData_FloatNumber(kDefaultFontSize,			pref.size);
	SetData_Number(kDefaultTabWidth,				pref.tabwidth);
	SetData_Number(kDefaultWindowWidth,				pref.windowwidth);
	SetData_Number(kDefaultWindowHeight,			pref.windowheight);
	
	//#688 /*win SetData_Font*/SetData_Number(kDefaultPrintOption_PrintFont_compat,pref.printForm.printFont);
	SetData_FloatNumber(kDefaultPrintOption_PrintFontSize,		pref.printForm.printSize);
	//#688 /*win SetData_Font*/SetData_Number(kDefaultPrintOption_PageNumberFont_compat,pref.printForm.pageFont);
	SetData_FloatNumber(kDefaultPrintOption_PageNumberFontSize,	pref.printForm.pageSize);
	//#688 /*win SetData_Font*/SetData_Number(kDefaultPrintOption_FileNameFont_compat,pref.printForm.fileFont);
	SetData_FloatNumber(kDefaultPrintOption_FileNameFontSize,	pref.printForm.fileSize);
	//#688 /*win SetData_Font*/SetData_Number(kDefaultPrintOption_LineNumberFont_compat,pref.printForm.lineFont);
	SetData_Number(kDefaultPrintOption_LineMargin,				pref.printForm.betweenLine);
	
	SetData_Number(kLineMargin,						pref.betweenLine);
	
	SetData_Number(kCIndent_Normal,					pref.prefC.normalIndent);
	SetData_Number(kCIndent_Fukubun,				pref.prefC.fukubunIndent);
	SetData_Number(kCIndent_Label,					pref.prefC.labelIndent);
	
	if( ATextEncodingWrapper::GetTextEncodingName(pref.defaultTextEncoding,text) == true )
	{
		SetData_Text(kDefaultTextEncoding,text);
	}
	
	//AUtil::GetATextFromOSType(pref.creator,text);
	text.SetFromOSType(pref.creator);
	SetData_Text(kDefaultCreator,text);
	
	//行送りの設定
	//旧データ(EWrapMode)と新データ(AWrapMode)では値の定義が異なるので、下記変換を行う。
	if( pref.wrapmode == wrapMode_NoWrap )
	{
		SetData_Number(kDefaultWrapMode,static_cast<ANumber>(kWrapMode_NoWrap));
	}
	else
	{
		if( pref.LCWrap == true )
		{
			SetData_Number(kDefaultWrapMode,static_cast<ANumber>(kWrapMode_WordWrapByLetterCount));
		}
		else
		{
			SetData_Number(kDefaultWrapMode,static_cast<ANumber>(kWrapMode_WordWrap));
		}
	}
	SetData_Number(kDefaultWrapMode_LetterCount,pref.LCWrapCount);
	
	SetData_Number(kDefaultReturnCode,static_cast<ANumber>(pref.returncode));
	
	//B0398 indentChar設定引き継ぎ
	AText	indentText;
	for( AUChar ch = 0; ch < 128; ch++ )
	{
		if( pref.indentChar[ch] == true )
		{
			indentText.Add(ch);
		}
	}
	SetData_Text(kIndentExeLetters,indentText);
}

//#844
#if 0
//
void	AModePrefDB::WriteMacLegacy_Pref( short inResRefNum )
{
	SMacLegacyModePref	pref;
	LoadMacLegacy_GetPrefDefault(pref);
	
	
	//#844 if( GetData_Bool(kDefaultEnableAntiAlias) == true )
	//#844 {
	pref.antiAliasMinSize = 0;
	/*#844
	}
	else
	{
		pref.antiAliasMinSize = -1;
	}
	*/
	pref.background 				= GetData_Bool(kDisplayBackgroundImage);
	pref.displayInfoTag 			= GetData_Bool(kDisplayInformationHeader);
	pref.displayToolBar 			= GetData_Bool(kDisplayToolbar);
	pref.displayCurrentPosition	= GetData_Bool(kDisplayPositionButton);
	pref.defalutOpenJumpList		= GetData_Bool(kDisplayJumpList);
	pref.displayRuler				= GetData_Bool(kDisplayRuler);
	pref.rulerScale				= ((GetData_Bool(kRulerScaleWithTabLetter)==true)?rulerScale_Tab:rulerScale_5);
	pref.displayReturn				= GetData_Bool(kDisplayReturnCode);
	pref.displayTab				= GetData_Bool(kDisplayTabCode);
	pref.displaySpace				= GetData_Bool(kDisplaySpaceCode);
	pref.displayZenkakuSpace		= GetData_Bool(kDisplayZenkakuSpaceCode);
	pref.masume					= GetData_Bool(kDisplayGenkoyoshi);
	pref.kakkoTaio					= GetData_Bool(kCheckBrace);
	pref.insideKakkoSelection		= GetData_Bool(kSelectInsideBraceByDoubleClick);
	pref.tripleClick2				= GetData_Bool(kSelectInsideByQuadClick);
	pref.mimikakiWrap				= GetData_Bool(kKinsokuBuraasge);
	pref.spacedTab					= GetData_Bool(kInputSpacesByTabKey);
	pref.smartDrag					= GetData_Bool(kAddRemoveSpaceByDragDrop);
	pref.changeYenToBackslash		= GetData_Bool(kInputBackslashByYenKey);
	pref.dontSaveResource			= GetData_Bool(kDontSaveResouceFork);
	pref.attachUTF8BOM				= GetData_Bool(kSaveUTF8BOM);
	pref.checkCharsetAfterSave		= GetData_Bool(kCheckCharsetWhenSave);
	pref.attachDOSEOF				= GetData_Bool(kSaveDOSEOF);
	pref.saveWindowPosition		= GetData_Bool(kSaveWindowPosition);
	pref.useBOMSniff				= GetData_Bool(kSniffTextEncoding_BOM);
	pref.charsetRecognize			= GetData_Bool(kSniffTextEncoding_Charset);
	pref.useLegacySniff			= GetData_Bool(kSniffTextEncoding_Legacy);
	pref.correctEncoding			= GetData_Bool(kShowDialogWhenTextEncodingNotDecided);
	pref.useMiWordSelect			= GetData_Bool(kApplyCharsForWordToDoubleClick);
	pref.indentC					= GetData_Bool(kUseCIndent);
	pref.recognizeC				= GetData_Bool(kUseCSyntaxColor);
	
	pref.displayCurrentLineNumber = 
			(GetData_Bool(kDisplayLineNumberButton)==true && GetData_Bool(kDisplayLineNumberButton_AsParagraph)==false);
	pref.displayCurrentParagraphNumber = 
			(GetData_Bool(kDisplayLineNumberButton)==true && GetData_Bool(kDisplayLineNumberButton_AsParagraph)==true);
	pref.displayEachLineNumber = 
			(GetData_Bool(kDisplayEachLineNumber)==true && GetData_Bool(kDisplayEachLineNumber_AsParagraph)==false);
	pref.displayEachParagraphNumber = 
			(GetData_Bool(kDisplayEachLineNumber)==true && GetData_Bool(kDisplayEachLineNumber_AsParagraph)==true);
	
	pref.printForm.usePrintFont	= GetData_Bool(kDefaultPrintOption_UsePrintFont);
	pref.printForm.printPageNumber	= GetData_Bool(kDefaultPrintOption_PrintPageNumber);
	pref.printForm.printFileName	= GetData_Bool(kDefaultPrintOption_PrintFileName);
	pref.printForm.printLineNumber	= GetData_Bool(kDefaultPrintOption_PrintLineNumber);
	pref.printForm.printSeparateLine= GetData_Bool(kDefaultPrintOption_PrintSeparateLine);
	pref.printForm.wrapMode		= 
			((GetData_Bool(kDefaultPrintOption_ForceWordWrap)==true)?wrapMode_WordWrap:wrapMode_NoWrap);
	
	GetData_Color(kLetterColor,						pref.normalColor);
	GetData_Color(kBackgroundColor,					pref.backColor);
	GetData_Color(kSpecialCodeColor,				pref.specialCharacterColor);
	GetData_Color(kCommentColor,					pref.commentColor);
	GetData_Color(kLiteralColor,					pref.literalColor);
	
	GetData_Color(kCSyntaxColor_Function,			pref.prefC.functionColor);
	GetData_Color(kCSyntaxColor_ExternalFunction,	pref.prefC.externalFunctionColor);
	GetData_Color(kCSyntaxColor_Class,				pref.prefC.classColor);
	GetData_Color(kCSyntaxColor_GlobalVariable,		pref.prefC.globalVariableColor);
	GetData_Color(kCSyntaxColor_LocalVariable,		pref.prefC.localVariableColor);
	GetData_Color(kCSyntaxColor_ExternalVariable,	pref.prefC.externalVariableColor);
	GetData_Color(kCSyntaxColor_EnumType,			pref.prefC.enumtypeColor);
	GetData_Color(kCSyntaxColor_Enum,				pref.prefC.enumColor);
	GetData_Color(kCSyntaxColor_Typedef,			pref.prefC.typedefColor);
	GetData_Color(kCSyntaxColor_Struct,				pref.prefC.structColor);
	GetData_Color(kCSyntaxColor_Union,				pref.prefC.unionColor);
	GetData_Color(kCSyntaxColor_Define,				pref.prefC.defineColor);
	
	AText	text;
	GetData_Text(kRulerBaseLetter,					text);
	text.LimitLength(0,255);
	text.GetPstring(pref.rulerStringUTF8,255);
	
	GetData_Text(kQuadClickText,					text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,255);
	text.GetPstring(pref.tripleClick2String,255);
	
	GetData_Text(kCommentStart,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.commentStart,31);
	
	GetData_Text(kCommentEnd,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.commentEnd,31);
	
	GetData_Text(kCommentLineStart,					text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.commentLine,31);
	
	GetData_Text(kLiteralStart1,					text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.literalStart1,31);
	
	GetData_Text(kLiteralEnd1,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.literalEnd1,31);
	
	GetData_Text(kLiteralStart2,					text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.literalStart2,31);
	
	GetData_Text(kLiteralEnd2,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.literalEnd2,31);
	
	GetData_Text(kEscapeString,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.literalEscape,31);
	
	GetData_Text(kOtherStart1,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.otherStart1,31);
	
	GetData_Text(kOtherEnd1,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.otherEnd1,31);
	
	GetData_Text(kOtherStart2,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.otherStart2,31);
	
	GetData_Text(kOtherEnd2,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,31);
	text.GetPstring(pref.otherEnd2,31);
	
	GetData_Text(kCharsForWord,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,255);
	text.GetPstring(pref.alphabetString,255);
	
	GetData_Text(kCharsForHeadOfWord,				text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,255);
	text.GetPstring(pref.headAlphabetString,255);
	
	GetData_Text(kCharsForTailOfWord,				text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,255);
	text.GetPstring(pref.tailAlphabetString,255);
	
	GetData_Text(kTeXCompiler,						text);
	text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	text.LimitLengthAsSJIS(0,255);
	text.GetPstring(pref.texCompilerPath,255);
	
	pref.font					= 0;//#688 /*win GetData_Font*/GetData_Number(/*win kDefaultFont*/kDefaultFont_compat);
	pref.size					= GetData_FloatNumber(kDefaultFontSize);
	pref.tabwidth				= GetData_Number(kDefaultTabWidth);
	pref.windowwidth			= 800;//#844 GetData_Number(kDefaultWindowWidth);
	pref.windowheight			= 900;//#844 GetData_Number(kDefaultWindowHeight);
	
	pref.printForm.printFont	= 0;//#688 /*win GetData_Font*/GetData_Number(kDefaultPrintOption_PrintFont_compat);
	pref.printForm.printSize	= GetData_FloatNumber(kDefaultPrintOption_PrintFontSize);
	if( pref.printForm.printSize > 24 )   pref.printForm.printSize = 24;
	if( pref.printForm.printSize != 6 && pref.printForm.printSize != 8 && pref.printForm.printSize != 9 && pref.printForm.printSize != 10 &&
				pref.printForm.printSize != 12 && pref.printForm.printSize != 14 && pref.printForm.printSize != 18 && pref.printForm.printSize != 24 )
	{
		pref.printForm.printSize = 12;
	}
	pref.printForm.pageFont	= 0;//#688 /*win GetData_Font*/GetData_Number(kDefaultPrintOption_PageNumberFont_compat);
	pref.printForm.pageSize	= GetData_FloatNumber(kDefaultPrintOption_PageNumberFontSize);
	if( pref.printForm.pageSize > 14 )   pref.printForm.pageSize = 14;
	if( pref.printForm.pageSize != 6 && pref.printForm.pageSize != 8 && pref.printForm.pageSize != 9 && pref.printForm.pageSize != 10 &&
				pref.printForm.pageSize != 12 && pref.printForm.pageSize != 14 )
	{
		pref.printForm.pageSize = 12;
	}
	pref.printForm.fileFont	= 0;//#688 /*win GetData_Font*/GetData_Number(kDefaultPrintOption_FileNameFont_compat);
	pref.printForm.fileSize	= GetData_FloatNumber(kDefaultPrintOption_FileNameFontSize);
	if( pref.printForm.fileSize > 14 )   pref.printForm.fileSize = 14;
	if( pref.printForm.fileSize != 6 && pref.printForm.fileSize != 8 && pref.printForm.fileSize != 9 && pref.printForm.fileSize != 10 &&
				pref.printForm.fileSize != 12 && pref.printForm.fileSize != 14 )
	{
		pref.printForm.fileSize = 12;
	}
	pref.printForm.lineFont	= 0;//#688 /*win GetData_Font*/GetData_Number(kDefaultPrintOption_LineNumberFont_compat);
	pref.printForm.betweenLine	= GetData_Number(kDefaultPrintOption_LineMargin);
	
	pref.betweenLine			= GetData_Number(kLineMargin);
	
	pref.prefC.normalIndent	= GetData_Number(kCIndent_Normal);
	pref.prefC.fukubunIndent	= GetData_Number(kCIndent_Fukubun);
	pref.prefC.labelIndent		= GetData_Number(kCIndent_Label);
	
	GetData_Text(kDefaultTextEncoding,text);
	ATextEncodingWrapper::GetTextEncodingFromName(text,pref.defaultTextEncoding);
	
	switch(static_cast<AWrapMode>(GetData_Number(kDefaultWrapMode)))
	{
	  case kWrapMode_NoWrap:
		{
			pref.wrapmode = wrapMode_NoWrap;
			pref.LCWrap = false;
			break;
		}
	  case kWrapMode_WordWrap:
		{
			pref.wrapmode = wrapMode_WordWrap;
			pref.LCWrap = false;
			break;
		}
	  case kWrapMode_WordWrapByLetterCount:
		{
			pref.wrapmode = wrapMode_WordWrap;
			pref.LCWrap = true;
			break;
		}
	}
	pref.LCWrapCount = GetData_Number(kDefaultWrapMode_LetterCount);
	
	pref.returncode = static_cast<EReturnCode>(GetData_Number(kDefaultReturnCode));
	
	GetData_Text(kDefaultCreator,text);
	pref.creator = text.GetOSTypeFromText();//AUtil::GetOSTypeFromAText(text);
	
	SMacLegacyModePref	**res = (SMacLegacyModePref**)Get1Resource(inResRefNum,'MMKP',1000);
	if( res != NULL ) 
	{
		::RemoveResource((Handle)res);
		::DisposeHandle((Handle)res);
	}
	res = (SMacLegacyModePref**)NewHandle(sizeof(SMacLegacyModePref));
	if( res != NULL ) 
	{
		//B0278 ここで書き込むのは旧Ver用のデータなので+2して保存（旧Verの42＝新Verの40）
		pref.LCWrapCount += 2;
		//R-ub
		LoadMacLegacy_ConvertPrefEndianToBig(pref);
		
		**res = pref;
		
		//R-ub
		LoadMacLegacy_ConvertPrefEndianToHost(pref);
		//B0278 元に戻す(-2)
		pref.LCWrapCount -= 2;
		
		AddResource((Handle)res,'MMKP',1000,"\pMIMIKAKI Mode Pref");
		WriteResource((Handle)res);
		ReleaseResource((Handle)res);
	}
}
#endif

//
void	AModePrefDB::LoadMacLegacy_Keyword( short inResRefNum, const AText& inText )
{
	//R0210 AModePrefDB::LoadSuffix()より移動
	{
		DeleteAll_TextArray(AModePrefDB::kSuffix);
		SMacLegacyTextArrayPosition	**suffixpos = NULL;
		suffixpos = (SMacLegacyTextArrayPosition**)Get1Resource(inResRefNum,'MMCW',1500);
		if( suffixpos != NULL )
		{
			Int32	suffixcount = ::GetHandleSize((Handle)suffixpos)/sizeof(SMacLegacyTextArrayPosition);
			for( Int32 number = 0; number < suffixcount; number++ ) 
			{
				AText	suffix;
				SMacLegacyTextArrayPosition	s = (*suffixpos)[number];
				AUniversalBinaryUtility::SwapBigToHost(s.position);
				AUniversalBinaryUtility::SwapBigToHost(s.length);
				inText.GetText(s.position,s.length,suffix);
				if( suffix.GetLength() == 0 )   suffix.SetCstring("   ");
				Add_TextArray(AModePrefDB::kSuffix,suffix);
			}
		}
	}
	
	SMacLegacyCategoryResource**categoryres = NULL;
	RGBColor	**colorres = NULL;
	SMacLegacyKeywordPref**	keywordPrefH = (SMacLegacyKeywordPref**)Get1Resource(inResRefNum,'MMKC',3000);
	Boolean	newStruct = ( keywordPrefH != NULL );
	short	categoryResCount = 0;
	if( newStruct )
	{
		categoryResCount = (*keywordPrefH)->categoryCount;
		//R-ub
		AUniversalBinaryUtility::SwapBigToHost(categoryResCount);
	}
	else
	{
		colorres = (RGBColor**)Get1Resource(inResRefNum,'MMKC',1100);
		if( colorres ==  NULL ) 
		{
			colorres = (RGBColor**)Get1Resource(inResRefNum,'MMKC',1000);
		}
		
		if( colorres != NULL )
		{
			RGBColor	normalColor = (*colorres)[0];
			RGBColor	backgroundColor = (*colorres)[1];
			//R-ub
			AUniversalBinaryUtility::SwapBigToHost(normalColor);
			AUniversalBinaryUtility::SwapBigToHost(backgroundColor);
			//
			SetData_Color(kLetterColor,normalColor);
			SetData_Color(kBackgroundColor,backgroundColor);
		}
		categoryres = (SMacLegacyCategoryResource**)Get1Resource(inResRefNum,'MMCW',1100);
		if( categoryres == NULL ) 
		{
			categoryres = (SMacLegacyCategoryResource**)Get1Resource(inResRefNum,'MMCW',1000);
		}
		if( categoryres != NULL )
		{
			categoryResCount =  GetHandleSize((Handle)categoryres) / sizeof(SMacLegacyCategoryResource);
		}
	}
	if( categoryResCount > 0 ) 
	{
		for( AIndex cat = 0; cat < categoryResCount; cat++ ) 
		{
			if( newStruct == false )
			{
				if( (*categoryres)[cat].exist == false )   continue;
			}
			
			AIndex categoryIndex = mLegacyKeywordTempDataArray.AddNewObject();
			AText	name;
			name.SetCstring("category");
			Add_TextArray(kAdditionalCategory_Name,name);
			Add_ColorArray(kAdditionalCategory_Color,kColor_Blue);
			Add_BoolArray(kAdditionalCategory_Bold,false);
			Add_BoolArray(kAdditionalCategory_RegExp,false);
			Add_BoolArray(kAdditionalCategory_IgnoreCase,false);
			Add_BoolArray(kAdditionalCategory_Underline,false);//R0195
			Add_BoolArray(kAdditionalCategory_Italic,false);//R0195
			Add_BoolArray(kAdditionalCategory_PriorToOtherColor,false);//R0195
			Add_TextArray(kAdditionalCategory_Keywords,GetEmptyText());//#427
			
			Int32	keywordCount = 0;
			if( newStruct )
			{
				SMacLegacyCategoryPref**	categoryPrefH = (SMacLegacyCategoryPref**)Get1Resource(inResRefNum,'MMKC',4000+cat);
				if( categoryPrefH != NULL )
				{
					keywordCount = (*categoryPrefH)->count;
					//R-ub
					AUniversalBinaryUtility::SwapBigToHost(keywordCount);
					
					RGBColor color = (*categoryPrefH)->color;
					//R-ub
					AUniversalBinaryUtility::SwapBigToHost(color);
					SetData_ColorArray(kAdditionalCategory_Color,categoryIndex,color);
					
					SetData_BoolArray(kAdditionalCategory_Bold,categoryIndex, ((*categoryPrefH)->bold == true) );
					SetData_BoolArray(kAdditionalCategory_IgnoreCase,categoryIndex,((*categoryPrefH)->ignoreCase));
					SetData_BoolArray(kAdditionalCategory_RegExp,categoryIndex,(*categoryPrefH)->regExp);
					AText	name;
					name.SetPstring((*categoryPrefH)->name);
					name.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
					SetData_TextArray(kAdditionalCategory_Name,categoryIndex,name);
					::ReleaseResource((Handle)categoryPrefH);
				}
			}
			else
			{
				keywordCount = (*categoryres)[cat].count;
				//R-ub
				AUniversalBinaryUtility::SwapBigToHost(keywordCount);
				
				RGBColor	color = (*colorres)[2+cat];//MMKC#1000にすべてのカテゴリーの色を格納
				//R-ub
				AUniversalBinaryUtility::SwapBigToHost(color);
				SetData_ColorArray(kAdditionalCategory_Color,categoryIndex,color);
				
				AText	name;
				name.SetPstring((*categoryres)[cat].name);
				name.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				SetData_TextArray(kAdditionalCategory_Name,categoryIndex,name);
			}
			SMacLegacyTextArrayPosition	**wordpos = NULL;
			wordpos = (SMacLegacyTextArrayPosition**)Get1Resource(inResRefNum,'MMCW',2000+cat);
			if( wordpos != NULL && GetHandleSize((Handle)wordpos) == sizeof(SMacLegacyTextArrayPosition)*keywordCount ) 
			{
				for( AIndex i = 0; i < keywordCount; i++ )
				{
					Int32	pos = (*wordpos)[i].position;
					Int32	len = (*wordpos)[i].length;
					AUniversalBinaryUtility::SwapBigToHost(pos);
					AUniversalBinaryUtility::SwapBigToHost(len);
					AText	keyword;
					inText.GetText(pos,len,keyword);
					keyword.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
					mLegacyKeywordTempDataArray.GetObject(categoryIndex).Add(keyword);
				}
			}
			if( wordpos != NULL )   ::ReleaseResource((Handle)wordpos);
		}
	}
	if( categoryres != NULL )   ::ReleaseResource((Handle)categoryres);
	if( colorres != NULL )   ::ReleaseResource((Handle)colorres);
	if( keywordPrefH != NULL )   ::ReleaseResource((Handle)keywordPrefH);
}

//#844
#if 0
void	AModePrefDB::WriteMacLegacy_Keyword( short inResRefNum, AText& outText )
{
	//拡張子
	{
		SMacLegacyTextArrayPosition	**res = (SMacLegacyTextArrayPosition**)Get1Resource(inResRefNum,'MMCW',1500);
		if( res != NULL ) 
		{
			::RemoveResource((Handle)res);
			::DisposeHandle((Handle)res);
		}
		//
		AItemCount	count = GetItemCount_Array(AModePrefDB::kSuffix);
		res = (SMacLegacyTextArrayPosition**)::NewHandle(sizeof(SMacLegacyTextArrayPosition)*count);
		if( res != NULL )
		{
			for( AIndex i = 0; i < count; i++ )
			{
				AText	suffix;
				GetData_TextArray(AModePrefDB::kSuffix,i,suffix);
				Int32	spos = outText.GetItemCount();
				suffix.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				outText.AddText(suffix);
				Int32	epos = outText.GetItemCount();
				(*res)[i].position = spos;
				(*res)[i].length = epos-spos;
				::HLock((Handle)res);
				AUniversalBinaryUtility::SwapHostToBig((*res)[i].position);
				AUniversalBinaryUtility::SwapHostToBig((*res)[i].length);
				::HUnlock((Handle)res);
			}
			::AddResource((Handle)res,'MMCW',1500,"\p");
			::WriteResource((Handle)res);
			::ReleaseResource((Handle)res);
		}
	}
	//キーワード
	{
		{
			SMacLegacyCategoryResource	**res = (SMacLegacyCategoryResource**)Get1Resource(inResRefNum,'MMCW',1100);
			if( res != NULL ) 
			{
				::RemoveResource((Handle)res);
				::DisposeHandle((Handle)res);
			}
		}
		{
			RGBColor	**res = (RGBColor**)Get1Resource(inResRefNum,'MMKC',1100);
			if( res != NULL ) 
			{
				::RemoveResource((Handle)res);
				::DisposeHandle((Handle)res);
			}
		}
		{
			SMacLegacyKeywordPref	**res = (SMacLegacyKeywordPref**)Get1Resource(inResRefNum,'MMKC',3000);
			if( res != NULL ) 
			{
				::RemoveResource((Handle)res);
				::DisposeHandle((Handle)res);
			}
		}
		//書き込み
		AItemCount	categoryCount = GetItemCount_Array(kAdditionalCategory_Name);
		SMacLegacyKeywordPref**	res = (SMacLegacyKeywordPref**)::NewHandle(sizeof(SMacLegacyKeywordPref));
		if( res != NULL )
		{
			(*res)->keywordPrefVersion = 1;
			(*res)->categoryCount = categoryCount;
			
			::HLock((Handle)res);
			AUniversalBinaryUtility::SwapHostToBig((*res)->keywordPrefVersion);
			AUniversalBinaryUtility::SwapHostToBig((*res)->categoryCount);
			::HUnlock((Handle)res);
			
			::AddResource((Handle)res,'MMKC',3000,"\pKeywords Pref ver1");
			::WriteResource((Handle)res);
			::ReleaseResource((Handle)res);
		}
		for( AIndex cat = 0; cat < categoryCount; cat++ )
		{
			SMacLegacyTextArrayPosition**	posres = (SMacLegacyTextArrayPosition**)Get1Resource(inResRefNum,'MMCW',2000+cat);
			if( posres != NULL )
			{
				::RemoveResource((Handle)posres);
				::DisposeHandle((Handle)posres);
			}
			AItemCount	keywordCount = mLegacyKeywordTempDataArray.GetObject(cat).GetItemCount();
			if( keywordCount == 0 )
			{
				//0個で保存すると旧バージョンで起動できない問題の回避策
				posres = (SMacLegacyTextArrayPosition**)::NewHandle(sizeof(SMacLegacyTextArrayPosition));
				AText	keyword("***noword***");
				Int32	spos = outText.GetItemCount();
				keyword.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				outText.AddText(keyword);
				Int32	epos = outText.GetItemCount();
				(*posres)[0].position = spos;
				(*posres)[0].length = epos-spos;
				::HLock((Handle)posres);
				AUniversalBinaryUtility::SwapHostToBig((*posres)[0].position);
				AUniversalBinaryUtility::SwapHostToBig((*posres)[0].length);
				::HUnlock((Handle)posres);
				keywordCount = 1;
			}
			else
			{
				posres = (SMacLegacyTextArrayPosition**)::NewHandle(sizeof(SMacLegacyTextArrayPosition)*keywordCount);
				for( AIndex i = 0; i < keywordCount; i++ )
				{
					AText	keyword;
					mLegacyKeywordTempDataArray.GetObject(cat).Get(i,keyword);
					Int32	spos = outText.GetItemCount();
					keyword.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
					outText.AddText(keyword);
					Int32	epos = outText.GetItemCount();
					(*posres)[i].position = spos;
					(*posres)[i].length = epos-spos;
					::HLock((Handle)posres);
					AUniversalBinaryUtility::SwapHostToBig((*posres)[i].position);
					AUniversalBinaryUtility::SwapHostToBig((*posres)[i].length);
					::HUnlock((Handle)posres);
				}
			}
			::AddResource((Handle)posres,'MMCW',2000+cat,"\p");
			::WriteResource((Handle)posres);
			::ReleaseResource((Handle)posres);
			//category pref
			SMacLegacyCategoryPref**	res = (SMacLegacyCategoryPref**)::Get1Resource(inResRefNum,'MMKC',4000+cat);
			if( res != NULL )
			{
				::RemoveResource((Handle)res);
				::DisposeHandle((Handle)res);
			}
			res = (SMacLegacyCategoryPref**)::NewHandle(sizeof(SMacLegacyCategoryPref));
			(*res)->categoryPrefVersion = 1;
			(*res)->count = keywordCount;
			AColor	color;
			GetData_ColorArray(kAdditionalCategory_Color,cat,color);
			(*res)->color = color;
			(*res)->bold = GetData_BoolArray(kAdditionalCategory_Bold,cat);
			(*res)->ignoreCase = GetData_BoolArray(kAdditionalCategory_IgnoreCase,cat);
			(*res)->isAuto = false;
			(*res)->regExp = GetData_BoolArray(kAdditionalCategory_RegExp,cat);
			AText	nametext;
			GetData_TextArray(kAdditionalCategory_Name,cat,nametext);
			nametext.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			nametext.LimitLengthAsSJIS(0,255);
			nametext.GetPstring((*res)->name,255);
			::HLock((Handle)res);
			AUniversalBinaryUtility::SwapHostToBig((*res)->categoryPrefVersion);
			AUniversalBinaryUtility::SwapHostToBig((*res)->count);
			AUniversalBinaryUtility::SwapHostToBig((*res)->color);
			::HUnlock((Handle)res);
			
			::AddResource((Handle)res,'MMKC',4000+cat,"\pCategoryPref Ver1");
			::WriteResource((Handle)res);
			::ReleaseResource((Handle)res);
		}
	}
#if 0
	CText	outtext;
	short	rRefNum;
	rRefNum = ::FSpOpenResFile(&mFileSpec,fsCurPerm);
	if( rRefNum == -1 )   return;
	
	STextArrayPosition	**suffixres;
	suffixres = (STextArrayPosition**)UResource::Get1Resource(rRefNum,'MMCW',1500);
	if( suffixres != NULL ) {
		::RemoveResource((Handle)suffixres);
		::DisposeHandle((Handle)suffixres);
		suffixres = NULL;
	}
	//R-abs CTextArray	*suffixArray = CModeData::GetSuffixArrayByID(mModeID);
	ATextArray*	suffixArray = gApp->GetModePref().GetData_TextArrayRef(mModeID,AModePref::kSuffixArray);
	if( suffixArray != NULL && suffixArray->GetItemCount() != 0 ) {
		suffixres = (STextArrayPosition**)::NewHandle(sizeof(STextArrayPosition)*suffixArray->GetItemCount());
		if( suffixres == NULL ) {
			CloseResFile(rRefNum);
			return;
		}
		for( Int32 number = 0; number < suffixArray->GetItemCount(); number++ ) {
			Str255	str;
			//R-abs suffixArray->GetString(number,str);
			AText	suffix;
			suffixArray->Get(number,suffix);
			AUtil::GetPstringFromAText(suffix,str,255);
			
			Int32	spos,epos;
			spos = outtext.mLength;
			outtext.AddPstring(str);
			epos = outtext.mLength;
			(*suffixres)[number].position = spos;
			(*suffixres)[number].length = epos-spos;
			
			//R-ub
			::HLock((Handle)suffixres);
			UUB::SwapHostToBig((*suffixres)[number].position);
			UUB::SwapHostToBig((*suffixres)[number].length);
			::HUnlock((Handle)suffixres);
		}
		AddResource((Handle)suffixres,'MMCW',1500,"\p");
		WriteResource((Handle)suffixres);
		ReleaseResource((Handle)suffixres);
	}
	
	SCategoryResource	**categoryres;
	categoryres = (SCategoryResource**)UResource::Get1Resource(rRefNum,'MMCW',1100);
	if( categoryres != NULL ) {
		RemoveResource((Handle)categoryres);
		DisposeHandle((Handle)categoryres);
		categoryres = NULL;
	}
	RGBColor	**colorres;
	colorres = (RGBColor**)UResource::Get1Resource(rRefNum,'MMKC',1100);
	if( colorres != NULL ) {
		RemoveResource((Handle)colorres);
		DisposeHandle((Handle)colorres);
		colorres = NULL;
	}
	
	// Keywords Pref
	SKeywordPref**	keywordPrefH = (SKeywordPref**)UResource::Get1Resource(rRefNum,'MMKC',3000);
	if( keywordPrefH != NULL )
	{
		RemoveResource((Handle)keywordPrefH);
		DisposeHandle((Handle)keywordPrefH);
	}
	keywordPrefH = (SKeywordPref**)NewHandle(sizeof(SKeywordPref));
	if( keywordPrefH != NULL )
	{
		(*keywordPrefH)->keywordPrefVersion = 1;
		(*keywordPrefH)->categoryCount = mCategoryCount;
		
		//R-ub
		::HLock((Handle)keywordPrefH);
		UUB::SwapHostToBig((*keywordPrefH)->keywordPrefVersion);
		UUB::SwapHostToBig((*keywordPrefH)->categoryCount);
		::HUnlock((Handle)keywordPrefH);
		
		AddResource((Handle)keywordPrefH,'MMKC',3000,"\pKeywords Pref ver1");
		WriteResource((Handle)keywordPrefH);
		ReleaseResource((Handle)keywordPrefH);
	}
	
	LListIterator	iterator(mCategory,iterate_FromStart);
	iterator.ResetTo(iterate_FromStart);
	for( short cat = 0; cat < mCategoryCount; cat++ )
	{
		CCategory	*category;
		if( !iterator.Next(&category) ) 
		{
			break;
		}
		if( category->mWordStorage->mItemCount != 0 ) 
		{
			// Keywords Position Array
			STextArrayPosition	**positionres;
			positionres = (STextArrayPosition**)UResource::Get1Resource(rRefNum,'MMCW',2000+cat);
			if( positionres != NULL ) 
			{
				RemoveResource((Handle)positionres);
				DisposeHandle((Handle)positionres);
				positionres = NULL;
			}
			positionres = (STextArrayPosition**)NewHandle(sizeof(STextArrayPosition)*
					category->mWordStorage->mItemCount);
			if( positionres == NULL ) 
			{
				CloseResFile(rRefNum);
				return;
			}
			Int32	offset = outtext.mLength;
			category->mWordStorage->mStorage->Lock();
			outtext.Insert(outtext.mLength,(unsigned char*)*(category->mWordStorage->mStorage->mStorageH),
					category->mWordStorage->mStorage->mLength);
			category->mWordStorage->mStorage->Unlock();
			category->mWordStorage->mPosition->Lock();
			STextArrayPosition	*posP;
			posP = (STextArrayPosition*)*(category->mWordStorage->mPosition->mStorageH);
			for( Int32 number = 0; number < category->mWordStorage->mItemCount; number++ )
			{
				(*positionres)[number].position = posP[number].position+offset;
				(*positionres)[number].length = posP[number].length;
				//R-ub
				::HLock((Handle)positionres);
				UUB::SwapHostToBig((*positionres)[number].position);
				UUB::SwapHostToBig((*positionres)[number].length);
				::HUnlock((Handle)positionres);
			}
			category->mWordStorage->mPosition->Unlock();
			AddResource((Handle)positionres,'MMCW',2000+cat,"\p");
			WriteResource((Handle)positionres);
			ReleaseResource((Handle)positionres);
			
			// Category Pref
			SCategoryPref	**categoryPrefH;
			categoryPrefH = (SCategoryPref**)UResource::Get1Resource(rRefNum,'MMKC',4000+cat);
			if( categoryPrefH != NULL ) 
			{
				RemoveResource((Handle)categoryPrefH);
				DisposeHandle((Handle)categoryPrefH);
			}
			categoryPrefH = (SCategoryPref**)NewHandle(sizeof(SCategoryPref));
			if( categoryPrefH == NULL ) 
			{
				CloseResFile(rRefNum);
				return;
			}
			(*categoryPrefH)->categoryPrefVersion = 1;
			(*categoryPrefH)->count = category->mWordStorage->mItemCount;
			(*categoryPrefH)->color = category->mColor;
			(*categoryPrefH)->bold = category->mBold;
			(*categoryPrefH)->ignoreCase = category->mIgnoreCase;
			(*categoryPrefH)->isAuto = category->mAuto;
			(*categoryPrefH)->regExp = category->mRegExp;
			UPstring::Copy((*categoryPrefH)->name,category->mName);
			//R-ub
			::HLock((Handle)categoryPrefH);
			UUB::SwapHostToBig((*categoryPrefH)->categoryPrefVersion);
			UUB::SwapHostToBig((*categoryPrefH)->count);
			UUB::SwapHostToBig((*categoryPrefH)->color);
			::HUnlock((Handle)categoryPrefH);
			
			AddResource((Handle)categoryPrefH,'MMKC',4000+cat,"\pCategoryPref Ver1");
			WriteResource((Handle)categoryPrefH);
			ReleaseResource((Handle)categoryPrefH);
		}
	}
	
	//モードファイルの下位互換性のため
	{
		short	categoryrescount = mCategoryCount;
		if( categoryrescount < 8 )   categoryrescount = 8;
		categoryres = (SCategoryResource**)NewHandle(sizeof(SCategoryResource)*categoryrescount);
		colorres = (RGBColor**)NewHandle(sizeof(RGBColor)*(categoryrescount+2));
		(*colorres)[0] = CModeData::GetModeDataByID(mModeID)->mPref.normalColor;
		(*colorres)[1] = CModeData::GetModeDataByID(mModeID)->mPref.backColor;
		//R-ub
		::HLock((Handle)colorres);
		UUB::SwapHostToBig((*colorres)[0]);
		UUB::SwapHostToBig((*colorres)[1]);
		::HUnlock((Handle)colorres);
		
		if( categoryres == NULL || colorres == NULL ) {
			CloseResFile(rRefNum);
			return;
		}
		LListIterator	iterator(mCategory,iterate_FromStart);
		for( short cat = 0; cat < categoryrescount; cat++ ) {
			CCategory	*category;
			if( !iterator.Next(&category) ) {
				for( ; cat < categoryrescount; cat++ ) {
					(*categoryres)[cat].exist = false;
					(*colorres)[2+cat] = CModeData::GetModeDataByID(mModeID)->mPref.normalColor;
					//R-ub
					::HLock((Handle)colorres);
					UUB::SwapHostToBig((*colorres)[2+cat]);
					::HUnlock((Handle)colorres);
				}
				break;
			}
			if( category->mWordStorage->mItemCount == 0 ) {
				(*categoryres)[cat].exist = false;
				(*colorres)[2+cat] = category->mColor;
				//R-ub
				::HLock((Handle)colorres);
				UUB::SwapHostToBig((*colorres)[2+cat]);
				::HUnlock((Handle)colorres);
			}
			else {
				(*categoryres)[cat].exist = true;
				(*colorres)[2+cat] = category->mColor;
				(*categoryres)[cat].count = category->mWordStorage->mItemCount;
				(*categoryres)[cat].kind = 0;
				HLock((Handle)categoryres);
				UPstring::CopyLimited((*categoryres)[cat].name,category->mName,31);
				HUnlock((Handle)categoryres);
				//R-ub
				::HLock((Handle)colorres);
				UUB::SwapHostToBig((*colorres)[2+cat]);
				::HUnlock((Handle)colorres);
				::HLock((Handle)categoryres);
				UUB::SwapHostToBig((*categoryres)[cat].count);
				::HUnlock((Handle)categoryres);
			}
		}
		AddResource((Handle)categoryres,'MMCW',1100,"\p");
		WriteResource((Handle)categoryres);
		ReleaseResource((Handle)categoryres);
		AddResource((Handle)colorres,'MMKC',1100,"\p");
		WriteResource((Handle)colorres);
		ReleaseResource((Handle)colorres);
	}
	
	::CloseResFile(rRefNum);
	
	// Keywords Text
	short	fref;
	FSpOpenDF(&mFileSpec,fsWrPerm,&fref);
	outtext.Lock();
	long	count = outtext.mLength;
	FSWrite(fref,&count,*(outtext.mTextH));
	SetEOF(fref,count);
	outtext.Unlock();
	FSClose(fref);
#endif
}
#endif

void AModePrefDB::LoadMacLegacy_ConvertPrefEndianToHost( SMacLegacyModePref& ioPref )
{
	//Booleanは1バイトなので変換の必要なし（boolは4バイト）、enumも1バイト
	AUniversalBinaryUtility::SwapBigToHost(ioPref.font);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.size);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.windowwidth);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.windowheight);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.tabwidth);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.creator);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.printForm.printFont);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.printForm.printSize);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.printForm.pageFont);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.printForm.pageSize);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.printForm.fileFont);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.printForm.fileSize);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.printForm.lineFont);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.printForm.betweenLine);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.jumpScrollStart);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.jumpScrollLines);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.specialCharacterColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.LCWrapCount);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.betweenLine);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.commentColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.literalColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.normalIndent);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.labelIndent);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.fukubunIndent);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.functionColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.externalFunctionColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.classColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.globalVariableColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.localVariableColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.externalVariableColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.commentColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.literalColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.enumtypeColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.enumColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.structColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.unionColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.defineColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.prefC.typedefColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.normalColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.backColor);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.texCompiler);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.texPreviewer);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.antiAliasMinSize);
	AUniversalBinaryUtility::SwapBigToHost(ioPref.legacyCopyTextEncoding);
}

void AModePrefDB::LoadMacLegacy_ConvertPrefEndianToBig( SMacLegacyModePref& ioPref )
{
	//Booleanは1バイトなので変換の必要なし（boolは4バイト）、enumも1バイト
	AUniversalBinaryUtility::SwapHostToBig(ioPref.font);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.size);	AUniversalBinaryUtility::SwapHostToBig(ioPref.windowwidth);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.windowheight);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.tabwidth);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.creator);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.printForm.printFont);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.printForm.printSize);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.printForm.pageFont);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.printForm.pageSize);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.printForm.fileFont);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.printForm.fileSize);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.printForm.lineFont);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.printForm.betweenLine);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.jumpScrollStart);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.jumpScrollLines);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.specialCharacterColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.LCWrapCount);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.betweenLine);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.commentColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.literalColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.normalIndent);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.labelIndent);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.fukubunIndent);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.functionColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.externalFunctionColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.classColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.globalVariableColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.localVariableColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.externalVariableColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.commentColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.literalColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.enumtypeColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.enumColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.structColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.unionColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.defineColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.prefC.typedefColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.normalColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.backColor);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.texCompiler);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.texPreviewer);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.antiAliasMinSize);
	AUniversalBinaryUtility::SwapHostToBig(ioPref.legacyCopyTextEncoding);
}
#endif

//#1034
//mi本体用処理
#ifndef MODEPREFCONVERTER
// リソースTCMDからショートカットキー情報を読み出す
//（バージョン2.0のファイル名に/～をつける方式には対応しない。ファイル名の制限ができてしまうため。）
void	AModePrefDB::LoadTool_LegacyShortcutData( const AFileAcc& inToolFile, 
		ANumber& outShortcutKey, ABool& outShortcutShiftKey, ABool& outShortcutControlKey, ABool& outShortcutOptionKey )
{
	outShortcutShiftKey = false;
	outShortcutControlKey = false;
	outShortcutOptionKey = false;
	SLegacyCommandKeyInfo	cmdInfo;
	cmdInfo.key = 0;
	AText	buffer;
	if( inToolFile.ExistResource('TCMD',1000) == true )
	{
		if( inToolFile.ReadResouceTo(buffer,'TCMD',1000) == true )
		{
			AStTextPtr	bufptr(buffer,0,buffer.GetItemCount());
			cmdInfo = *((SLegacyCommandKeyInfo*)(bufptr.GetPtr()));
			AUniversalBinaryUtility::SwapBigToHost(cmdInfo.version);
			AUniversalBinaryUtility::SwapBigToHost(cmdInfo.key);
		}
	}
	outShortcutKey = cmdInfo.key;
	if( outShortcutKey != 0 )
	{
		outShortcutShiftKey = cmdInfo.shiftKey;
		outShortcutControlKey = cmdInfo.controlKey;
		outShortcutOptionKey = cmdInfo.optionKey;
	}
}
#endif

#if 0 
//#844
#if 0
void	AModePrefDB::WriteTool_LegacyShortcutData( const AFileAcc& inToolFile, 
		const ANumber inShortcutKey, const ABool inShortcutShiftKey, const ABool inShortcutControlKey )
{
	FSRef	ref;
	inToolFile.GetFSRef(ref);
	{
		HFSUniStr255	forkName;
		::FSGetResourceForkName(&forkName);
		::FSCreateResourceFork(&ref,forkName.length,forkName.unicode,0);
	}
	short	rRefNum = ::FSOpenResFile(&ref,fsWrPerm);
	if( rRefNum != -1 )
	{
		SLegacyCommandKeyInfo	**res = (SLegacyCommandKeyInfo**)Get1Resource(rRefNum,'TCMD',1000);
		if( res != NULL ) 
		{
			::RemoveResource((Handle)res);
			::DisposeHandle((Handle)res);
		}
		res = (SLegacyCommandKeyInfo**)NewHandle(sizeof(SLegacyCommandKeyInfo));
		if( res != NULL ) 
		{
			SLegacyCommandKeyInfo	cmdInfo;
			cmdInfo.key = inShortcutKey;
			cmdInfo.shiftKey = inShortcutShiftKey;
			cmdInfo.controlKey = inShortcutControlKey;
			cmdInfo.optionKey = false;
			cmdInfo.version = 1;
			AUniversalBinaryUtility::SwapHostToBig(cmdInfo.version);
			AUniversalBinaryUtility::SwapHostToBig(cmdInfo.key);
			
			**res = cmdInfo;
			::AddResource((Handle)res,'TCMD',1000,"\p");
			::WriteResource((Handle)res);
			::ReleaseResource((Handle)res);
		}
		::CloseResFile(rRefNum);
	}
}
#endif

#pragma mark ---DocPref

//#902
#if 0
void	ADocPrefDB::LoadMacLegacy( const AFileAcc& inFile, APrintPagePrefData& outPrintPref )
{
	FSRef	ref;
	inFile.GetFSRef(ref);
	short	rRefNum = ::FSOpenResFile(&ref,fsRdPerm);
	if( rRefNum != -1 )
	{
		//Print
		Handle resprint = Get1Resource(rRefNum,'MMKP',6000);//Print Settings
		if( resprint != NULL )
		{
			::DetachResource(resprint);
			outPrintPref.SetPrintSettingsHandle(resprint);
		}
		resprint = Get1Resource(rRefNum,'MMKP',6001);//Page Format
		if( resprint != NULL )
		{
			::DetachResource(resprint);
			outPrintPref.SetPageFormatHandle(resprint);
		}
#if 0
		//
		Handle	res = Get1Resource(rRefNum,'MMKP',3000);
		if( res != NULL )
		{
			SLegacyDocPref	pref = *((SLegacyDocPref*)(*res));
			::ReleaseResource(res);
			AText	text;
			
			AUniversalBinaryUtility::SwapBigToHost(pref.docPrefVersion);
			AUniversalBinaryUtility::SwapBigToHost(pref.font);
			AUniversalBinaryUtility::SwapBigToHost(pref.size);
			AUniversalBinaryUtility::SwapBigToHost(pref.caretPosition);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.selectPosition);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.tabwidth);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.windowwidth);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.windowheight);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.printForm.printFont);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.printForm.printSize);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.printForm.pageFont);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.printForm.pageSize);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.printForm.fileFont);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.printForm.fileSize);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.printForm.lineFont);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.printForm.betweenLine);//R-ub
			AUniversalBinaryUtility::SwapBigToHost(pref.LCWrapCount);//R-ub
			//#688 /*win SetData_Font*/SetData_Number(/*win kTextFont*/kTextFont_compat,pref.font);
			SetData_FloatNumber(kTextFontSize,					pref.size);
			SetData_Bool(kCaretMode,							pref.caretMode);
			SetData_Number(kCaretPointX,						pref.caretPosition.h);
			SetData_Number(kCaretPointY,						pref.caretPosition.v);
			SetData_Number(kSelectPointX,						pref.selectPosition.h);
			SetData_Number(kSelectPointY,						pref.selectPosition.v);
			//kanjiCode+fontでTextEncodingを決めるのは確実でないので自動認識のほうに任せる
			//ATextEncodingWrapper::GetTextEncodingName(ATextEncodingWrapper::GetTextEncodingFromKanjiCode(pref.kanjiCode,pref.font),text);
			//SetData_Text(kTextEncodingName,						text);
			SetData_Number(kReturnCode,							pref.returnCode);
			SetData_Number(kOldWrapMode,						pref.wrapMode);
			SetData_Number(kTabWidth,							pref.tabwidth);
			SetData_Number(kWindowWidth,						pref.windowwidth);
			SetData_Number(kWindowHeight,						pref.windowheight);
			
			SetData_Bool(kPrintOption_UsePrintFont,				pref.printForm.usePrintFont);
			SetData_Bool(kPrintOption_PrintPageNumber,			pref.printForm.printPageNumber);
			SetData_Bool(kPrintOption_PrintFileName,			pref.printForm.printFileName);
			SetData_Bool(kPrintOption_PrintLineNumber,			pref.printForm.printLineNumber);
			SetData_Bool(kPrintOption_PrintSeparateLine,		pref.printForm.printSeparateLine);
			SetData_Number(kPrintOption_OldWrapMode,			pref.printForm.wrapMode);
			//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_PrintFont*/kPrintOption_PrintFont_compat,pref.printForm.printFont);
			SetData_FloatNumber(kPrintOption_PrintFontSize,		pref.printForm.printSize);
			//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_PageNumberFont*/kPrintOption_PageNumberFont_compat,pref.printForm.pageFont);
			SetData_FloatNumber(kPrintOption_PageNumberFontSize,pref.printForm.pageSize);
			//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_FileNameFont*/kPrintOption_FileNameFont_compat,pref.printForm.fileFont);
			SetData_FloatNumber(kPrintOption_FileNameFontSize,	pref.printForm.fileSize);
			//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_LineNumberFont*/kPrintOption_LineNumberFont_compat,pref.printForm.lineFont);
			SetData_Number(kPrintOption_LineMargin,				pref.printForm.betweenLine);
			
			text.SetPstring(pref.modeString);
			text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			SetData_Text(kModeName,								text);
			SetData_Bool(kOldWrapModeLCWrap,					pref.LCWrap);
			SetData_Number(kOldLCWrapLetterCount,				pref.LCWrapCount);
			
			if( pref.docPrefVersion >= 2 )
			{
				AUniversalBinaryUtility::SwapBigToHost(pref.windowPosition.h);
				AUniversalBinaryUtility::SwapBigToHost(pref.windowPosition.v);
				APoint	pt;
				pt.x = pref.windowPosition.h;
				pt.y = pref.windowPosition.v;
				SetData_Point(kWindowPosition,					pt);
			}
			if( pref.docPrefVersion >= 3 )
			{
				SetData_Number(kAntiAliasMode,					pref.antiAliasMode);
			}
		}
		else
		{
			Handle	r = Get1Resource(rRefNum,'MMKP',2001);
			if( r != NULL ) 
			{ 
				//B0133 リソースのサイズでフォーマット判定
				if( GetHandleSize(r) == sizeof(SLegacyDocPref21b3) ) 
				{
					SLegacyDocPref21b3	pref = *((SLegacyDocPref21b3*)(*r));
					::ReleaseResource(r);
					AText	text;
					
					AUniversalBinaryUtility::SwapBigToHost(pref.font);
					AUniversalBinaryUtility::SwapBigToHost(pref.size);
					AUniversalBinaryUtility::SwapBigToHost(pref.caretPosition);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.selectPosition);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.tabwidth);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.windowwidth);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.windowheight);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.printFont);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.printSize);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.pageFont);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.pageSize);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.fileFont);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.fileSize);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.lineFont);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.betweenLine);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.LCWrapCount);//R-ub
					//#688 SetData_Number(/*win kTextFont*/kTextFont_compat,	pref.font);
					SetData_FloatNumber(kTextFontSize,					pref.size);
					SetData_Bool(kCaretMode,							pref.caretMode);
					SetData_Number(kCaretPointX,						pref.caretPosition.h);
					SetData_Number(kCaretPointY,						pref.caretPosition.v);
					SetData_Number(kSelectPointX,						pref.selectPosition.h);
					SetData_Number(kSelectPointY,						pref.selectPosition.v);
					//kanjiCode+fontでTextEncodingを決めるのは確実でないので自動認識のほうに任せる
					//ATextEncodingWrapper::GetTextEncodingName(ATextEncodingWrapper::GetTextEncodingFromKanjiCode(pref.kanjiCode,pref.font),text);
					//SetData_Text(kTextEncodingName,						text);
					SetData_Number(kReturnCode,							pref.returnCode);
					SetData_Number(kOldWrapMode,						pref.wrapMode);
					SetData_Number(kTabWidth,							pref.tabwidth);
					SetData_Number(kWindowWidth,						pref.windowwidth);
					SetData_Number(kWindowHeight,						pref.windowheight);
					
					SetData_Bool(kPrintOption_UsePrintFont,				pref.printForm.usePrintFont);
					SetData_Bool(kPrintOption_PrintPageNumber,			pref.printForm.printPageNumber);
					SetData_Bool(kPrintOption_PrintFileName,			pref.printForm.printFileName);
					SetData_Bool(kPrintOption_PrintLineNumber,			pref.printForm.printLineNumber);
					SetData_Bool(kPrintOption_PrintSeparateLine,		pref.printForm.printSeparateLine);
					SetData_Number(kPrintOption_OldWrapMode,			pref.printForm.wrapMode);
					//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_PrintFont*/kPrintOption_PrintFont_compat,pref.printForm.printFont);
					SetData_FloatNumber(kPrintOption_PrintFontSize,		pref.printForm.printSize);
					//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_PageNumberFont*/kPrintOption_PageNumberFont_compat,pref.printForm.pageFont);
					SetData_FloatNumber(kPrintOption_PageNumberFontSize,pref.printForm.pageSize);
					//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_FileNameFont*/kPrintOption_FileNameFont_compat,pref.printForm.fileFont);
					SetData_FloatNumber(kPrintOption_FileNameFontSize,	pref.printForm.fileSize);
					//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_LineNumberFont*/kPrintOption_LineNumberFont_compat,pref.printForm.lineFont);
					SetData_Number(kPrintOption_LineMargin,				pref.printForm.betweenLine);
					
					text.SetPstring(pref.modeString);
					text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
					SetData_Text(kModeName,								text);
					SetData_Bool(kOldWrapModeLCWrap,					pref.LCWrap);
					SetData_Number(kOldLCWrapLetterCount,				pref.LCWrapCount);
				}
				else if( GetHandleSize(r) == sizeof(SLegacyDocPref20b6) ) 
				{
					SLegacyDocPref20b6	pref = *((SLegacyDocPref20b6*)(*r));
					::ReleaseResource(r);
					AText	text;
					
					AUniversalBinaryUtility::SwapBigToHost(pref.font);
					AUniversalBinaryUtility::SwapBigToHost(pref.size);
					AUniversalBinaryUtility::SwapBigToHost(pref.caretPosition);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.selectPosition);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.tabwidth);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.windowwidth);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.windowheight);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.printFont);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.printSize);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.pageFont);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.pageSize);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.fileFont);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.fileSize);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.printForm.lineFont);//R-ub
					AUniversalBinaryUtility::SwapBigToHost(pref.LCWrapCount);//R-ub
					//#688 SetData_Number(/*win kTextFont*/kTextFont_compat,	pref.font);
					SetData_FloatNumber(kTextFontSize,					pref.size);
					SetData_Bool(kCaretMode,							pref.caretMode);
					SetData_Number(kCaretPointX,						pref.caretPosition.h);
					SetData_Number(kCaretPointY,						pref.caretPosition.v);
					SetData_Number(kSelectPointX,						pref.selectPosition.h);
					SetData_Number(kSelectPointY,						pref.selectPosition.v);
					//kanjiCode+fontでTextEncodingを決めるのは確実でないので自動認識のほうに任せる
					//ATextEncodingWrapper::GetTextEncodingName(ATextEncodingWrapper::GetTextEncodingFromKanjiCode(pref.kanjiCode,pref.font),text);
					//SetData_Text(kTextEncodingName,						text);
					SetData_Number(kReturnCode,							pref.returnCode);
					SetData_Number(kOldWrapMode,						pref.wrapMode);
					SetData_Number(kTabWidth,							pref.tabwidth);
					SetData_Number(kWindowWidth,						pref.windowwidth);
					SetData_Number(kWindowHeight,						pref.windowheight);
					
					SetData_Bool(kPrintOption_UsePrintFont,				pref.printForm.usePrintFont);
					SetData_Bool(kPrintOption_PrintPageNumber,			pref.printForm.printPageNumber);
					SetData_Bool(kPrintOption_PrintFileName,			pref.printForm.printFileName);
					SetData_Bool(kPrintOption_PrintLineNumber,			pref.printForm.printLineNumber);
					SetData_Bool(kPrintOption_PrintSeparateLine,		pref.printForm.printSeparateLine);
					SetData_Number(kPrintOption_OldWrapMode,			pref.printForm.wrapMode);
					//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_PrintFont*/kPrintOption_PrintFont_compat,pref.printForm.printFont);
					SetData_FloatNumber(kPrintOption_PrintFontSize,		pref.printForm.printSize);
					//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_PageNumberFont*/kPrintOption_PageNumberFont_compat,pref.printForm.pageFont);
					SetData_FloatNumber(kPrintOption_PageNumberFontSize,pref.printForm.pageSize);
					//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_FileNameFont*/kPrintOption_FileNameFont_compat,pref.printForm.fileFont);
					SetData_FloatNumber(kPrintOption_FileNameFontSize,	pref.printForm.fileSize);
					//#688 /*win SetData_Font*/SetData_Number(/*win kPrintOption_LineNumberFont*/kPrintOption_LineNumberFont_compat,pref.printForm.lineFont);
					
					text.SetPstring(pref.modeString);
					text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
					SetData_Text(kModeName,								text);
					SetData_Bool(kOldWrapModeLCWrap,					pref.LCWrap);
					SetData_Number(kOldLCWrapLetterCount,				pref.LCWrapCount);
				}
			}
		}
		Handle	tecres = Get1Resource(rRefNum,'EncN',1000);
		if( tecres != NULL )
		{
			AText	text;
			::HLock((Handle)tecres);
			text.SetFromTextPtr(*tecres,::GetHandleSize((Handle)tecres));
			::HUnlock((Handle)tecres);
			::ReleaseResource(tecres);
			SetData_Text(kTextEncodingName,text);
		}
#endif
		::CloseResFile(rRefNum);
	}
}
#endif

void	ADocPrefDB::RemoveMacLegacyResouces( const AFileAcc& inFile )
{
	FSRef	ref;
	inFile.GetFSRef(ref);
	short	rRefNum = ::FSOpenResFile(&ref,fsWrPerm);
	if( rRefNum != -1 )
	{
		Handle	res;
		res = Get1Resource(rRefNum,'MMKP',2001);
		if( res != NULL ) 
		{
			::RemoveResource(res);
			::DisposeHandle(res);
		}
		res = Get1Resource(rRefNum,'MMKP',3000);
		if( res != NULL ) 
		{
			::RemoveResource(res);
			::DisposeHandle(res);
		}
		res = Get1Resource(rRefNum,'EncN',1000);
		if( res != NULL ) 
		{
			::RemoveResource(res);
			::DisposeHandle(res);
		}
		res = Get1Resource(rRefNum,'MMKE',1);
		if( res != NULL ) 
		{
			::RemoveResource(res);
			::DisposeHandle(res);
		}
		res = Get1Resource(rRefNum,'MMKP',6000);
		if( res != NULL ) 
		{
		  ::RemoveResource(res);
		  ::DisposeHandle(res);
		}
		res = Get1Resource(rRefNum,'MMKP',6001);
		if( res != NULL ) 
		{
		  ::RemoveResource(res);
		  ::DisposeHandle(res);
		}
		
		::CloseResFile(rRefNum);//#239
	}
}

//#902
#if 0
void	ADocPrefDB::WriteMacLegacy( const AFileAcc& inFile, APrintPagePrefData& inPrintPref )
{
	AText	modename;
	GetData_Text(kModeName,modename);
	AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeName(modename);
	if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
	FSRef	ref;
	inFile.GetFSRef(ref);
	{
		HFSUniStr255	forkName;
		::FSGetResourceForkName(&forkName);
		::FSCreateResourceFork(&ref,forkName.length,forkName.unicode,0);
	}
	short	rRefNum = ::FSOpenResFile(&ref,fsWrPerm);
	if( rRefNum != -1 )
	{
		//Print
		{
			Handle	res = Get1Resource(rRefNum,'MMKP',6000);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			res = Get1Resource(rRefNum,'MMKP',6001);
			if( res != NULL ) 
			{
				::RemoveResource(res);
				::DisposeHandle(res);
			}
			Handle	printsettingsH = inPrintPref.TakePrintSettingsHandle();
			Handle	pageformatH = inPrintPref.TakePageFormatHandle();
			if( printsettingsH != NULL )
			{
				::AddResource(printsettingsH,'MMKP',6000,"\pmi Print Settings");
				::WriteResource(printsettingsH);
				::ReleaseResource(printsettingsH);
			} 
			if( pageformatH != NULL )
			{
				::AddResource(pageformatH,'MMKP',6001,"\pmi Page Format");
				::WriteResource(pageformatH);
				::ReleaseResource(pageformatH);
			}
		}
	}
  ::CloseResFile(rRefNum);
}
#endif
#endif

//#1034
//modeprefconverter用処理
//int16_tだけSwapBigToHost()を実装
#ifndef MODEPREFCONVERTER
//#1034
void AUniversalBinaryUtility::SwapBigToHost( int16_t &ioValue )
{
	ioValue = ::CFSwapInt16BigToHost(ioValue);
}

//#1034
void AUniversalBinaryUtility::SwapHostToBig( int16_t &ioValue )
{
	ioValue = ::CFSwapInt16HostToBig(ioValue);
}

//mi本体用処理
#else

#pragma mark ---AMacLegacyPrefUtil

//旧kanjiCodeからTextEncodingへの変換
ATextEncoding AMacLegacyPrefUtil::GetTextEncodingFromKanjiCode( EKanjiCode inKanjiCode, short inFont )
{
	TextEncoding	result;
	switch(inKanjiCode)
	{
	  case kanjiCode_MacCode:
	  case kanjiCode_MacRoman:
	  case kanjiCode_MacCyrillic:
	  case kanjiCode_MacGreek:
	  case kanjiCode_MacTurkish:
		/* kanjiCodeが用いられるのは旧Pref使用した場合のみであるので、font情報もあるはず。
		よって旧Mac内部コードのkanjiCodeだった場合は、fontによって、上記を区別する。 */
		switch(::FontToScript(inFont))
		{
		  case smJapanese:
			ATextEncodingWrapper::GetTextEncodingFromName("Shift_JIS",result);
			break;
		  case smGreek:
			ATextEncodingWrapper::GetTextEncodingFromName("x-mac-greek",result);
			break;
		  case smCyrillic:
			ATextEncodingWrapper::GetTextEncodingFromName("x-mac-cyrillic",result);
			break;
		  /*case smTurkish:
			ATextEncodingWrapper::GetTextEncodingFromName("x-mac-turkish",result);
			break;*/
		  default:
			ATextEncodingWrapper::GetTextEncodingFromName("x-mac-roman",result);
			break;
		}
		break;
	  case kanjiCode_Jis:
		ATextEncodingWrapper::GetTextEncodingFromName("ISO-2022-JP",result);//B0000 \ISO-2022-JPとなっていたのを修正(最初の\を削除)
		break;
	  case kanjiCode_Euc:
		ATextEncodingWrapper::GetTextEncodingFromName("EUC-JP",result);
		break;
	  case kanjiCode_ISO88591:
		ATextEncodingWrapper::GetTextEncodingFromName("ISO-8859-1",result);
		break;
	  case kanjiCode_ISO88595:
		ATextEncodingWrapper::GetTextEncodingFromName("ISO-8859-5",result);
		break;
	  case kanjiCode_KOI8R:
		ATextEncodingWrapper::GetTextEncodingFromName("KOI8-R",result);
		break;
	  case kanjiCode_Windows1251:
		ATextEncodingWrapper::GetTextEncodingFromName("windows-1251",result);
		break;
	  case kanjiCode_ISO88597:
		ATextEncodingWrapper::GetTextEncodingFromName("ISO-8859-7",result);
		break;
	  case kanjiCode_Windows1253:
		ATextEncodingWrapper::GetTextEncodingFromName("windows-1253",result);
		break;
	  case kanjiCode_ISO88599:
		ATextEncodingWrapper::GetTextEncodingFromName("ISO-8859-9",result);
		break;
	  case kanjiCode_Unicode_UCS2:
		ATextEncodingWrapper::GetTextEncodingFromName("UTF-16",result);
		break;
	  case kanjiCode_Unicode_UTF8:
	  default:
		ATextEncodingWrapper::GetTextEncodingFromName("UTF-8",result);
		break;
	}
	return result;
}

//TextEncodingから旧KanjiCodeへの変換（互換性のため）
EKanjiCode AMacLegacyPrefUtil::GetKanjiCodeFromTextEncoding( const ATextEncoding& inTextEncoding )
{
	EKanjiCode result = kanjiCode_MacCode;
	TextEncodingBase	tecbase = ::GetTextEncodingBase(inTextEncoding);
	switch(tecbase)
	{
	  case kTextEncodingShiftJIS:
	  case kTextEncodingMacGreek:
	  case kTextEncodingMacCyrillic:
	  case kTextEncodingMacTurkish:
	  case kTextEncodingMacRoman:
		result = kanjiCode_MacCode;
		break;
	  case kTextEncodingISO_2022_JP:
		result = kanjiCode_Jis;
		break;
	  case kTextEncodingEUC_JP:
		result = kanjiCode_Euc;
		break;
	  case kTextEncodingISOLatin1:
		result = kanjiCode_ISO88591;
		break;
	  case kTextEncodingISOLatinCyrillic:
		result = kanjiCode_ISO88595;
		break;
	  case kTextEncodingKOI8_R:
		result = kanjiCode_KOI8R;
		break;
	  case kTextEncodingWindowsCyrillic:
		result = kanjiCode_Windows1251;
		break;
	  case kTextEncodingISOLatinGreek:
		result = kanjiCode_ISO88597;
		break;
	  case kTextEncodingWindowsGreek:
		result = kanjiCode_Windows1253;
		break;
	  case kTextEncodingISOLatin5:
		result = kanjiCode_ISO88599;
		break;
	  case kTextEncodingUnicodeDefault:
		switch( ::GetTextEncodingFormat(inTextEncoding)  )
		{
		  case kUnicode16BitFormat:
			result = kanjiCode_Unicode_UCS2;
			break;
		  default:
			result = kanjiCode_Unicode_UTF8;
			break;
		}
		break;
	}
	return result;
}

#pragma mark --- UniversalBinary

void AUniversalBinaryUtility::SwapBigToHost( RGBColor &ioValue )
{
	SwapBigToHost(ioValue.red);
	SwapBigToHost(ioValue.green);
	SwapBigToHost(ioValue.blue);
}

void AUniversalBinaryUtility::SwapBigToHost( Rect &ioValue )
{
	SwapBigToHost(ioValue.top);
	SwapBigToHost(ioValue.left);
	SwapBigToHost(ioValue.bottom);
	SwapBigToHost(ioValue.right);
}

void AUniversalBinaryUtility::SwapBigToHost( FSSpec &ioValue )
{
	SwapBigToHost(ioValue.parID);
	SwapBigToHost(ioValue.vRefNum);
}

/* #1034
void AUniversalBinaryUtility::SwapBigToHost( SPoint32 &ioValue )
{
	SwapBigToHost(ioValue.h);
	SwapBigToHost(ioValue.v);
}
*/

void AUniversalBinaryUtility::SwapBigToHost( UInt16 &ioValue )
{
	ioValue = ::CFSwapInt16BigToHost(ioValue);
}

void AUniversalBinaryUtility::SwapBigToHost( SInt16 &ioValue )
{
	ioValue = ::CFSwapInt16BigToHost(ioValue);
}

void AUniversalBinaryUtility::SwapBigToHost( UInt32 &ioValue )
{
	ioValue = ::CFSwapInt32BigToHost(ioValue);
}

void AUniversalBinaryUtility::SwapBigToHost( SInt32 &ioValue )
{
	ioValue = ::CFSwapInt32BigToHost(ioValue);
}

void AUniversalBinaryUtility::SwapBigToHost( Point &ioValue )
{
	SwapBigToHost(ioValue.h);
	SwapBigToHost(ioValue.v);
}


void AUniversalBinaryUtility::SwapHostToBig( RGBColor &ioValue )
{
	SwapHostToBig(ioValue.red);
	SwapHostToBig(ioValue.green);
	SwapHostToBig(ioValue.blue);
}

void AUniversalBinaryUtility::SwapHostToBig( Rect &ioValue )
{
	SwapHostToBig(ioValue.top);
	SwapHostToBig(ioValue.left);
	SwapHostToBig(ioValue.bottom);
	SwapHostToBig(ioValue.right);
}

void AUniversalBinaryUtility::SwapHostToBig( FSSpec &ioValue )
{
	SwapHostToBig(ioValue.parID);
	SwapHostToBig(ioValue.vRefNum);
}

/* #1034
void AUniversalBinaryUtility::SwapHostToBig( SPoint32 &ioValue )
{
	SwapHostToBig(ioValue.h);
	SwapHostToBig(ioValue.v);
}
*/

void AUniversalBinaryUtility::SwapHostToBig( SInt16 &ioValue )
{
	ioValue = ::CFSwapInt16HostToBig(ioValue);
}

void AUniversalBinaryUtility::SwapHostToBig( UInt16 &ioValue )
{
	ioValue = ::CFSwapInt16HostToBig(ioValue);
}

void AUniversalBinaryUtility::SwapHostToBig( SInt32 &ioValue )
{
	ioValue = ::CFSwapInt32HostToBig(ioValue);
}

void AUniversalBinaryUtility::SwapHostToBig( UInt32 &ioValue )
{
	ioValue = ::CFSwapInt32HostToBig(ioValue);
}

void AUniversalBinaryUtility::SwapHostToBig( Point &ioValue )
{
	SwapHostToBig(ioValue.h);
	SwapHostToBig(ioValue.v);
}

EPrefVersion GetPrefVersion( short inRRefNum )
{
	Handle	h = Get1Resource(inRRefNum,'vers',1);
	if( h != NULL )
	{
		VersRec	versRec = *((VersRecPtr)(*h));
		switch(versRec.numericVersion.majorRev)
		{
		  case 2:
		  default:
			switch(versRec.numericVersion.minorAndBugRev)
			{
			  case 0x10://2.1
				switch(versRec.numericVersion.stage)
				{
				  case finalStage:
					return prefVersion_21b13;
					break;
				  case betaStage:
				  default:
					switch(versRec.numericVersion.nonRelRev)
					{
					  case 1://2.1b1
					  case 2://2.1b2
						return prefVersion_21b1;
						break;
					  case 3://2.1b3
						return prefVersion_21b3;
						break;
					  case 5://2.1b5
					  case 6://2.1b6
					  case 7://2.1b7
						return prefVersion_21b5;
						break;
					  case 8://2.1b8
						return prefVersion_21b8;
						break;
					  case 9://2.1b9
					  case 10://2.1b10
					  case 0x10:
					  case 0x11:
					  case 0x12:
						return prefVersion_21b9;
						break;
					  case 0x13:
						return prefVersion_21b13;
						break;
					  default:
						return prefVersion_21b13;
						break;
					}
				}
			  case 0x11://2.1.1
			  case 0x12://2.1.2
				return prefVersion_21b13;//B0034
				break;
			  case 0x13://2.1.3
			  case 0x14://2.1.4
				return prefVersion_213;//B0034
				break;
			  case 0x15://2.1.5
				return prefVersion_215;
				break;
			  case 0x16://2.1.6
				switch(versRec.numericVersion.stage)
				{
				  case betaStage:
					switch(versRec.numericVersion.nonRelRev)
					{
					  case 1://2.1.6b1
					  case 2://2.1.6b2
						return prefVersion_216b1;
						break;
					  case 3://2.1.6b3
					  case 4://2.1.6b4
						return prefVersion_216b3;
						break;
					  case 5://2.1.6b5
					  case 6://2.1.6b6
					  case 7://2.1.6b7
					  default:
						return prefVersion_216b5;
						break;
					}
					break;
				}
				return prefVersion_216b5;
				break;
			  case 0x17://2.1.7
				switch(versRec.numericVersion.stage)
				{
				  case betaStage:
					switch(versRec.numericVersion.nonRelRev)
					{
					  case 1://2.1.7b1
					  case 2://2.1.7b2
					  case 3://2.1.7b3
					  case 4://2.1.7b4
					  case 5://2.1.7b5
						return prefVersion_217b1;
						break;
					  case 6://2.1.7b6
					  case 7://2.1.7b7
					  default:
						return prefVersion_217b6;
						break;
					}
					break;
				}
				return prefVersion_217b6;
				break;
			  case 0x18://2.1.8
			  case 0x19://2.1.9
			  default:
				return prefVersion_217b6;
				break;
			}
		}
	}
	
	h = Get1Resource(inRRefNum,'MMPV',1000);
	if( h == NULL ) {
		return prefVersion_Less20b5;
	}
	Str255	str;
	str[0] = 5;
	for( short i = 0; i < 5; i++ ) {
	    str[i+1] = *(*h+i);
	}
	ReleaseResource(h);
	if( ComparePstring(str,"\p201  ") ) {
	    return prefVersion_21b1;
	}
	else if( ComparePstring(str,"\p20b9 ") || ComparePstring(str,"\p20b10") ) {
	    return prefVersion_20b9;
	}
	else if( ComparePstring(str,"\p20b7 ") ) {
	    return prefVersion_20b7;
	}
    else if( ComparePstring(str,"\p20b6 ") ) {
	    return prefVersion_20b6;
	}
	else {
		return prefVersion_Less20b5;
	}
}

void	WritePrefVersion( short inRRefNum )
{
	//'VERS"によるバージョン情報はもう使用しないので、下位互換性のため、
	//'VERS'を使用するバージョンの最終(2.1.7b6)を保存する
	//#2は使用していないので、#1だけ保存
	VersRec	versRec;
	AMemoryWrapper::Fill0(&versRec,sizeof(versRec));
	versRec.numericVersion.majorRev = 2;
	versRec.numericVersion.minorAndBugRev = 0x17;
	versRec.numericVersion.stage = betaStage;
	versRec.numericVersion.nonRelRev = 6;
	versRec.countryCode = verJapan;
	
	Handle	h2 = Get1Resource(inRRefNum,'vers',2);
	if( h2 != NULL ) 
	{
		::RemoveResource(h2);
		::DisposeHandle(h2);
	}
	Handle	h1 = Get1Resource(inRRefNum,'vers',1);
	if( h1 != NULL ) 
	{
		::RemoveResource(h1);
		::DisposeHandle(h1);
	}
	h1 = ::NewHandle(sizeof(VersRec));
	if( h1 != NULL )
	{
		*((VersRecPtr)(*h1)) = versRec;
		::AddResource(h1,'vers',1,"\p");
		::WriteResource(h1);
		::ReleaseResource(h1);
	}
}

Handle Get1Resource( short inRRefNum, ResType inResType, short inID )
{
	::UseResFile(inRRefNum);
	return ::Get1Resource(inResType,inID);
}

void	CopyPstring( Str255 dst, const Str255 src )
{
	short	i;
	
	*dst = *src;
	for( i = 1; i <= *src; i++ ) {
		*(dst+i) = *(src+i);
	}
}

ABool	ComparePstring( const Str255 a, const Str255 b )
{
	short	i;
	
	if( *a != *b )   return false;
	for( i = 1; i <= *a; i++ ) {
		if( *(a+i) != *(b+i) )   return false;
	}
	return true;
}
#endif




