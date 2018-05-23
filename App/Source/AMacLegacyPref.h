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

#pragma once
#include "../../AbsFramework/AbsBase/ABase.h"
#include "ATypes.h"

#ifndef MODEPREFCONVERTER
//mi本体用処理 #1034
struct SLegacyCommandKeyInfo
{
	/*#1034 short*/int16_t	version;
	/*#1034 short*/int16_t	key;
	Boolean	shiftKey;
	Boolean	controlKey;
	Boolean	optionKey;
};

#else
//modeprefconverter用処理 #1034

//ATypes.hから移動
struct FolderSpec
{
	short	vRefNum;
	long	dirID;
};

typedef SInt32 Int32;
typedef SInt16 Int16;

struct	SPoint32	{
	SInt32	h;
	SInt32	v;
};

struct SMacLegacyTextArrayPosition {
	Int32 position;
	Int32 length;
};


enum EAntiAliasMode
{
	antiAliasMode_Default =0,
	antiAliasMode_On,
	antiAliasMode_Off
};

enum EWindowListMode
{
	windowListMode_Window = 0,
	windowListMode_SameFolder,
	windowListMode_RecentFile
};

enum EWrapMode 
{
	wrapMode_NoWrap = 0,
	wrapMode_WordWrap,
	wrapMode_LetterWrap //既に未使用（「文字数指定」かどうかは別の設定を使用する）
};

enum EOSXDock
{
	OSXDock_Bottom = 0,
	OSXDock_Left,
	OSXDock_Right,
	OSXDock_None
};

enum EAbbrevKey {
	abbrevKey_ESC = 0,
	abbrevKey_Space
};

enum EAppearance {
	appearance_normal = 0,
	appearance_kaleido
};

enum EWindowPosition
{
	windowPosition_Left = 0,
	windowPosition_Right
};

enum EProxyMode {
	proxyMode_NoProxy = 0,
	proxyMode_FTPProxy1,//USER remoteuser@remotehost->PASS remotepass
	proxyMode_URLAccess,
//	proxyMode_HTTPProxy,
	proxyMode_FTPProxy2//USER proxyuser->PASS proxypass->SITE remotehost->USER remoteuser->PASS remotepass
};

struct TPrint
{
	char	dummy[120];
};

enum EKanjiCode {
	kanjiCode_Unknown = -1,
	kanjiCode_MacCode = 0,
	kanjiCode_Jis,
	kanjiCode_Euc,
	kanjiCode_ISO88591,
	kanjiCode_MacRoman,
	kanjiCode_Unicode_UCS2,
	kanjiCode_Unicode_UTF8,
	kanjiCode_MacCyrillic,
	kanjiCode_ISO88595,
	kanjiCode_KOI8R,
	kanjiCode_Windows1251,
	kanjiCode_MacGreek,
	kanjiCode_ISO88597,
	kanjiCode_Windows1253,
	kanjiCode_MacTurkish,
	kanjiCode_ISO88599
};

//B0211
struct SRecentlyOpenedResource {
	short	count;
	FSSpec	file[48];
};


class AMacLegacyPrefUtil
{
  public:
	static ATextEncoding GetTextEncodingFromKanjiCode( EKanjiCode inKanjiCode, short inFont );
	static EKanjiCode GetKanjiCodeFromTextEncoding( const ATextEncoding& inTextEncoding );
};

struct SMacLegacyAppPref 
{
	Boolean		OBSOLETE_useContextualMenu;
	Boolean		CMMouseButton;//OK
	short		CMMouseButtonTick;//OK
	Boolean		OBSOLETE_useInlineInput;
	EAbbrevKey	OBSOLETE_abbrevKey;
	Boolean		OBSOLETE_independentFindString;
	Boolean		OBSOLETE_activeScroll;
	Boolean		OBSOLETE_useTEC;
	RGBColor	indexWinNormalColor;//OK
	RGBColor	indexWinBackColor;//OK
	RGBColor	indexWinCommentColor;//OK
	RGBColor	indexWinURLColor;//OK
	RGBColor	indexWinGroupColor;//OK
	OSType		OBSOLETE_browser;
	OSType		OBSOLETE_browserEventClass;
	OSType		OBSOLETE_browserEventID;
	Boolean		OBSOLETE_useNS;
	long		OBSOLETE_undoLevel;
	EAppearance	OBSOLETE_appearance;
	Boolean		OBSOLETE_useOpenRecent;
	Boolean		printColored;//OK
	Boolean		bunsetsuSelect;//OK
	Boolean		windowShortcut;//OK
	Str31		cmdNModeName;//OK
	Boolean		OBSOLETE_useInternetConfigBrowser;
	Boolean 	dummy1;
	Boolean		useShellScript;//OK
	Str31		stdOutModeName;//OK
	Boolean		lockWhileEdit;//OK
	long		dummy2;
	Boolean		printBlackForWhite;//OK
	EOSXDock	OBSOLETE_OSXDock;
	short		indexWindowFont;//OK
	short		indexWindowFontSize;//OK
	short		indexWindowWidth;//OK
	short		indexWindowHeight;//OK
	short		indexWindowSeparateLine;//OK
	Boolean		inlineInputColorLine;//OK
	Boolean		findDefaultIgnoreCase;//OK
	Boolean		findDefaultWholeWord;//OK
	Boolean		findDefaultAimai;//R-nib findDefaultIgnoreZenHan; //OK
	Boolean		findDefaultRegExp;//OK
	Boolean		findDefaultLoop;//OK
	Boolean		findDefaultAllowReturn;//OK
	Boolean		findDefaultRecursive;//OK
	Boolean		findDefaultDisplayPosition;//OK
	Str255		findDefaultFileFilter;//OK
	short		findFontSizeNum;//OK
	Point		findDialogPosition;//OK
	Point		multiFileFindDialogPosition;//OK
	short		dummy3;
	short		wheelScrollPercent;//OK
	short		wheelScrollPercentCmd;//OK
	short		dummy4;//R0061
	Boolean		alphaWindowMode;//R0061 //OK
	short		alphaWindowPercent1;//R0061 //OK
	short		alphaWindowPercent2;//R0061 //OK
	short		OBSOLETE_alphaWindowPercent3;//R0061
	short		OBSOLETE_alphaWindowPercent4;//R0061
	short		OBSOLETE_alphaWindowPercent5;//R0061
	Boolean		singleWindowMode;//OK
	Boolean		OBSOLETE_openNewWindow;//B0233
	Str31		OBSOLETE_openNewWindowModeName;//B0233
	short		jumpListOffsetX;//R0047 //OK
	short		jumpListOffsetY;//R0047 //OK
	short		jumpListAlphaPercent;//R0047 //OK
	short		OBSOLETE_jumpListFontNumDummy;//R0047
	short		OBSOLETE_jumpListFontSizeDummy;//R0047
	Boolean		OBSOLETE_jumpListRsv;//R0047
	short		jumpListSizeX;//R0047 //OK
	short		jumpListSizeY;//R0047 //OK
	short		windowListOffsetX;//R0003 //OK
	short		windowListOffsetY;//R0003 //OK
	short		windowListAlphaPercent;//R0003 //OK
	short		OBSOLETE_windowListFontNumDummy;//R0003
	short		OBSOLETE_windowListFontSizeDummy;//R0003
	short		windowListSizeX;//R0003 //OK
	short		windowListSizeY;//R0003 //OK
	RGBColor	OBSOLETE_windowListColor;//R0003
	RGBColor	OBSOLETE_jumpListColor;//R0003
	Boolean		displayWindowList;//OK
	RGBColor	OBSOLETE_windowListTextColor;//R0003 
	RGBColor	OBSOLETE_jumpListTextColor;//R0003
	short		dummy5;//R0003-2
	EWindowPosition	jumpListPosition;//R0003-2 //OK
	EWindowPosition	OBSOLETE_windowListPosition;//R0003-2
	Boolean		OBSOLETE_jumpListFixed;//R0066
	Boolean		OBSOLETE_windowListFixed;//R0066
	Rect		singleWindowBounds;//R0077 //OK
	EWindowListMode	windowListMode;//R0079
};

struct SAppPref217b1
{
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
	EAbbrevKey	abbrevKey;
	Boolean	independentFindString;
	Boolean	activeScroll;
	Boolean	useTEC;
	RGBColor	indexWinNormalColor;
	RGBColor	indexWinBackColor;
	RGBColor	indexWinCommentColor;
	RGBColor	indexWinURLColor;
	RGBColor	indexWinGroupColor;
	OSType	browser;
	OSType	browserEventClass;
	OSType	browserEventID;
	Boolean	useNS;
	long	undoLevel;
	EAppearance	appearance;
	Boolean	useOpenRecent;
	Boolean	printColored;
	Boolean	bunsetsuSelect;
	Boolean	windowShortcut;
	Str31	cmdNModeName;
	Boolean	useInternetConfigBrowser;
	Boolean dummy1;
	Boolean	useShellScript;
	Str31	stdOutModeName;
	Boolean	lockWhileEdit;
	long dummy2;
	Boolean	printBlackForWhite;
	EOSXDock	OSXDock;
	short	indexWindowFont;
	short	indexWindowFontSize;
	short	indexWindowWidth;
	short	indexWindowHeight;
	short	indexWindowSeparateLine;
	Boolean	inlineInputColorLine;
	Boolean	findDefaultIgnoreCase;
	Boolean	findDefaultWholeWord;
	Boolean	findDefaultIgnoreZenHan;
	Boolean	findDefaultRegExp;
	Boolean	findDefaultLoop;
	Boolean	findDefaultAllowReturn;
	Boolean	findDefaultRecursive;
	Boolean	findDefaultDisplayPosition;
	Str255	findDefaultFileFilter;
	short	findFontSizeNum;
	Point	findDialogPosition;
	Point	multiFileFindDialogPosition;
	short	dummy3;
	short	wheelScrollPercent;
	short	wheelScrollPercentCmd;
	short	dummy4;//R0061
	Boolean	alphaWindowMode;//R0061
	short	alphaWindowPercent1;//R0061
	short	alphaWindowPercent2;//R0061
	short	alphaWindowPercent3;//R0061
	short	alphaWindowPercent4;//R0061
	short	alphaWindowPercent5;//R0061
	Boolean	singleWindowMode;
	Boolean	openNewWindow;
	Str31	openNewWindowModeName;
	short	jumpListOffsetX;//R0047
	short	jumpListOffsetY;//R0047
	short	jumpListAlphaPercent;//R0047
	short	jumpListFontNum;//R0047
	short	jumpListFontSize;//R0047
	Boolean	jumpListRsv;//R0047
	short	jumpListSizeX;//R0047
	short	jumpListSizeY;//R0047
	short	windowListOffsetX;//R0003
	short	windowListOffsetY;//R0003
	short	windowListAlphaPercent;//R0003
	short	windowListFontNum;//R0003
	short	windowListFontSize;//R0003
	short	windowListSizeX;//R0003
	short	windowListSizeY;//R0003
	RGBColor	windowListColor;//R0003
	RGBColor	jumpListColor;//R0003
	Boolean	displayWindowList;
	RGBColor	windowListTextColor;//R0003
	RGBColor	jumpListTextColor;//R0003
	short	dummy5;//R0003-2
	EWindowPosition	jumpListPosition;//R0003-2
	EWindowPosition	windowListPosition;//R0003-2
	Boolean	jumpListFixed;//R0066
	Boolean	windowListFixed;//R0066
	Rect	singleWindowBounds;//R0077
	EWindowListMode	windowListMode;//R0079
};

struct SAppPref216b3 
{
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
	EAbbrevKey	abbrevKey;
	Boolean	independentFindString;
	Boolean	activeScroll;
	Boolean	useTEC;
	RGBColor	indexWinNormalColor;
	RGBColor	indexWinBackColor;
	RGBColor	indexWinCommentColor;
	RGBColor	indexWinURLColor;
	RGBColor	indexWinGroupColor;
	OSType	browser;
	OSType	browserEventClass;
	OSType	browserEventID;
	Boolean	useNS;
	long	undoLevel;
	EAppearance	appearance;
	Boolean	useOpenRecent;
	Boolean	printColored;
	Boolean	bunsetsuSelect;
	Boolean	windowShortcut;
	Str31	cmdNModeName;
	Boolean	useInternetConfigBrowser;
	Boolean dummy1;
	Boolean	useShellScript;
	Str31	stdOutModeName;
	Boolean	lockWhileEdit;
	long dummy2;
	Boolean	printBlackForWhite;
	EOSXDock	OSXDock;
	short	indexWindowFont;
	short	indexWindowFontSize;
	short	indexWindowWidth;
	short	indexWindowHeight;
	short	indexWindowSeparateLine;
	Boolean	inlineInputColorLine;
	Boolean	findDefaultIgnoreCase;
	Boolean	findDefaultWholeWord;
	Boolean	findDefaultIgnoreZenHan;
	Boolean	findDefaultRegExp;
	Boolean	findDefaultLoop;
	Boolean	findDefaultAllowReturn;
	Boolean	findDefaultRecursive;
	Boolean	findDefaultDisplayPosition;
	Str255	findDefaultFileFilter;
	short	findFontSizeNum;
	Point	findDialogPosition;
	Point	multiFileFindDialogPosition;
	short	dummy3;
	short	wheelScrollPercent;
	short	wheelScrollPercentCmd;
	short	dummy4;//R0061
	Boolean	alphaWindowMode;//R0061
	short	alphaWindowPercent1;//R0061
	short	alphaWindowPercent2;//R0061
	short	alphaWindowPercent3;//R0061
	short	alphaWindowPercent4;//R0061
	short	alphaWindowPercent5;//R0061
	Boolean	singleWindowMode;
	Boolean	openNewWindow;
	Str31	openNewWindowModeName;
	short	jumpListOffsetX;//R0047
	short	jumpListOffsetY;//R0047
	short	jumpListAlphaPercent;//R0047
	short	jumpListFontNum;//R0047
	short	jumpListFontSize;//R0047
	Boolean	jumpListRsv;//R0047
	short	jumpListSizeX;//R0047
	short	jumpListSizeY;//R0047
	short	windowListOffsetX;//R0003
	short	windowListOffsetY;//R0003
	short	windowListAlphaPercent;//R0003
	short	windowListFontNum;//R0003
	short	windowListFontSize;//R0003
	short	windowListSizeX;//R0003
	short	windowListSizeY;//R0003
	RGBColor	windowListColor;//R0003
	RGBColor	jumpListColor;//R0003
	Boolean	displayWindowList;
	RGBColor	windowListTextColor;//R0003
	RGBColor	jumpListTextColor;//R0003
};

struct SAppPref216b1//R0061
{
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
	EAbbrevKey	abbrevKey;
	Boolean	independentFindString;
	Boolean	activeScroll;
	Boolean	useTEC;
	RGBColor	indexWinNormalColor;
	RGBColor	indexWinBackColor;
	RGBColor	indexWinCommentColor;
	RGBColor	indexWinURLColor;
	RGBColor	indexWinGroupColor;
	OSType	browser;
	OSType	browserEventClass;
	OSType	browserEventID;
	Boolean	useNS;
	long	undoLevel;
	EAppearance	appearance;
	Boolean	useOpenRecent;
	Boolean	printColored;
	Boolean	bunsetsuSelect;
	Boolean	windowShortcut;
	Str31	cmdNModeName;
	Boolean	useInternetConfigBrowser;
	Boolean dummy1;
	Boolean	useShellScript;
	Str31	stdOutModeName;
	Boolean	lockWhileEdit;
	long dummy2;
	Boolean	printBlackForWhite;
	EOSXDock	OSXDock;
	short	indexWindowFont;
	short	indexWindowFontSize;
	short	indexWindowWidth;
	short	indexWindowHeight;
	short	indexWindowSeparateLine;
	Boolean	inlineInputColorLine;
	Boolean	findDefaultIgnoreCase;
	Boolean	findDefaultWholeWord;
	Boolean	findDefaultIgnoreZenHan;
	Boolean	findDefaultRegExp;
	Boolean	findDefaultLoop;
	Boolean	findDefaultAllowReturn;
	Boolean	findDefaultRecursive;
	Boolean	findDefaultDisplayPosition;
	Str255	findDefaultFileFilter;
	short	findFontSizeNum;
	Point	findDialogPosition;
	Point	multiFileFindDialogPosition;
	short	dummy3;
	short	wheelScrollPercent;
	short	wheelScrollPercentCmd;
};

//B0091
struct SAppPref21b13
{
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
	EAbbrevKey	abbrevKey;
	Boolean	independentFindString;
	Boolean	activeScroll;
	Boolean	useTEC;
	RGBColor	indexWinNormalColor;
	RGBColor	indexWinBackColor;
	RGBColor	indexWinCommentColor;
	RGBColor	indexWinURLColor;
	RGBColor	indexWinGroupColor;
	OSType	browser;
	OSType	browserEventClass;
	OSType	browserEventID;
	Boolean	useNS;
	long	undoLevel;
	EAppearance	appearance;
	Boolean	useOpenRecent;
	Boolean	printColored;
	Boolean	bunsetsuSelect;
	Boolean	windowShortcut;
	Str31	cmdNModeName;
	Boolean	useInternetConfigBrowser;
	Boolean dummy1;
	Boolean	useShellScript;
	Str31	stdOutModeName;
	Boolean	lockWhileEdit;
	long dummy2;
	Boolean	printBlackForWhite;
	EOSXDock	OSXDock;
	short	indexWindowFont;
	short	indexWindowFontSize;
	short	indexWindowWidth;
	short	indexWindowHeight;
	short	indexWindowSeparateLine;
	Boolean	inlineInputColorLine;
	Boolean	findDefaultIgnoreCase;
	Boolean	findDefaultWholeWord;
	Boolean	findDefaultIgnoreZenHan;
	Boolean	findDefaultRegExp;
	Boolean	findDefaultLoop;
	Boolean	findDefaultAllowReturn;
	Boolean	findDefaultRecursive;
	Boolean	findDefaultDisplayPosition;
	Str255	findDefaultFileFilter;
	short	findFontSizeNum;
	Point	findDialogPosition;
	Point	multiFileFindDialogPosition;
};

struct SAppPref21b9
{
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
	EAbbrevKey	abbrevKey;
	Boolean	independentFindString;
	Boolean	activeScroll;
	Boolean	useTEC;
	RGBColor	indexWinNormalColor;
	RGBColor	indexWinBackColor;
	RGBColor	indexWinCommentColor;
	RGBColor	indexWinURLColor;
	RGBColor	indexWinGroupColor;
	OSType	browser;
	OSType	browserEventClass;
	OSType	browserEventID;
	Boolean	useNS;
	long	undoLevel;
	EAppearance	appearance;
	Boolean	useOpenRecent;
	Boolean	printColored;
	Boolean	bunsetsuSelect;
	Boolean	windowShortcut;
	Str31	cmdNModeName;
	Boolean	useInternetConfigBrowser;
	Boolean dummy1;
	Boolean	useShellScript;
	Str31	stdOutModeName;
	Boolean	lockWhileEdit;
};

struct SAppPref21b8
{
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
	EAbbrevKey	abbrevKey;
	Boolean	independentFindString;
	Boolean	activeScroll;
	Boolean	useTEC;
	RGBColor	indexWinNormalColor;
	RGBColor	indexWinBackColor;
	RGBColor	indexWinCommentColor;
	RGBColor	indexWinURLColor;
	RGBColor	indexWinGroupColor;
	OSType	browser;
	OSType	browserEventClass;
	OSType	browserEventID;
	Boolean	useNS;
	long	undoLevel;
	EAppearance	appearance;
	Boolean	useOpenRecent;
	Boolean	printColored;
	Boolean	bunsetsuSelect;
	Boolean	windowShortcut;
	Str31	cmdNModeName;
	Boolean	useInternetConfigBrowser;
};

struct SAppPref21b5
{
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
	EAbbrevKey	abbrevKey;
	Boolean	independentFindString;
	Boolean	activeScroll;
	Boolean	useTEC;
	RGBColor	indexWinNormalColor;
	RGBColor	indexWinBackColor;
	RGBColor	indexWinCommentColor;
	RGBColor	indexWinURLColor;
	RGBColor	indexWinGroupColor;
	OSType	browser;
	OSType	browserEventClass;
	OSType	browserEventID;
	Boolean	useNS;
	long	undoLevel;
	EAppearance	appearance;
	Boolean	useOpenRecent;
	Boolean	printColored;
};

struct SAppPref20b5 {
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
};

struct SAppPref20b6 {
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
	EAbbrevKey	abbrevKey;
};

struct SAppPref20b7 {
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
	EAbbrevKey	abbrevKey;
	Boolean	independentFindString;
	Boolean	activeScroll;
	Boolean	useTEC;
};

struct SAppPref20b9 {
	Boolean	useContextualMenu;
	Boolean	CMMouseButton;
	short	CMMouseButtonTick;
	Boolean	useInlineInput;
	EAbbrevKey	abbrevKey;
	Boolean	independentFindString;
	Boolean	activeScroll;
	Boolean	useTEC;
	RGBColor	indexWinNormalColor;
	RGBColor	indexWinBackColor;
	RGBColor	indexWinCommentColor;
	RGBColor	indexWinURLColor;
	RGBColor	indexWinGroupColor;
	OSType	browser;
	OSType	browserEventClass;
	OSType	browserEventID;
};

struct SPrefC20b5 {
	Boolean	recognizeGlobal;
	Boolean	recognizeLocal;
	Boolean	recognizeExternal;
	Boolean	useIdentifierResource;
	Boolean	displayInfoTag;
	Boolean	useTRTemplate;
	Boolean	displayLineNumber;
	Boolean	displayFunction;
	Boolean	displayEachLineNumber;
	short	normalIndent;
	short	labelIndent;
	short	fukubunIndent;
	RGBColor	functionColor;
	RGBColor	externalFunctionColor;
	RGBColor	classColor;
	RGBColor	globalVariableColor;
	RGBColor	localVariableColor;
	RGBColor	externalVariableColor;
	RGBColor	commentColor;
	RGBColor	literalColor;
	RGBColor	enumtypeColor;
	RGBColor	enumColor;
	RGBColor	structColor;
	RGBColor	unionColor;
	RGBColor	defineColor;
	RGBColor	typedefColor;
};//same as 2.0b6
	
struct SPrefTex20b5 {
	short	indent;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	displayLineNumber;
	Boolean	displayPosition;
};//same as 2.0b6

struct SPrefHTML20b5 {
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	displayLineNumber;
	Boolean	displayPosition;
};//same as 2.0b6

struct SPrefMds20b5 {
	Str255	mdsString;
};//same as 2.0b6

// 2.1b7以前のリソースデータ構造
struct SMacLegacyCategoryResource {
	Boolean	exist;
	Int32	count;
	unsigned char	kind;//obsolete
	Str31	name;
};

//2.1b8より使用
struct SMacLegacyCategoryPref
{//Version 1
	short	categoryPrefVersion;
	Int32	count;
	RGBColor	color;
	Boolean	bold;
	Boolean	ignoreCase;
	Boolean	isAuto;
	Boolean	regExp;
	Str255	name;
};

struct SMacLegacyKeywordPref
{//Version 1
	short	keywordPrefVersion;
	short	categoryCount;
};

struct SMacLegacyLaunchApp 
{
	Boolean	checkCreator;
	Boolean	checkType;
	Boolean	checkExtension;
	Boolean	shiftKey;
	OSType	creator;
	OSType	type;
	Str31	extension;
	OSType	launchCreator;
	FSSpec	launchFileSpec;
};

struct SMacLegacyFTPServer
{
	short	version;
	Str255	server;
	Str255	user;
	Boolean	PASV;
	Boolean	proxy;
	Str255	proxyServer;
	Str255	rootPath;
	Str255	httpURL;
	short	port;
	Str255	localBackupPath;
	EProxyMode	proxyMode;
	// version 2
	long dummy1;
	Str255	proxyUser;
	// version 3
	long dummy2;
	Boolean	backup;
	FolderSpec	backupFolder;
};

struct SLegacyFTPFolderResHeader
{
	short	level;
	short	dummy[7];
};

struct SMacLegacyFindMemoryHeader 
{
	short	findMemoryCount;
};

struct SMacLegacyFindMemory
{
	Str255	name;
	Str255	findString;
	Str255	replaceString;
	Boolean ignoreCase;
	Boolean	wholeWord;
	Boolean	aimai;//R-uni
	Boolean	regExp;
	Boolean	loop;
	Boolean	dummy;
};

struct SLegacyDragDropPrefHeader 
{
	short	dragDropPrefCount;
};

struct SLegacyDragDropPref 
{
	Str31	suffix;
	Str255	text;
};

struct SLegacyTaioPrefHeader 
{
	short	count;
};

struct SLegacyTaioPref 
{
	Str63	start;
	Str63	end;
};

struct SLegacyIndentPrefHeader 
{
	short	IndentPrefCount;
};

struct SLegacyIndentPref
{
	Str63	regexp;
	short	currentIndent;
	short	nextIndent;
	short	reserved[8];
};

struct SLegacyMdsLinePrefHeader 
{
	short	MdsLinePrefCount;
};

struct SLegacyMdsLinePref
{
	Str31	name;
	Str255	regexp;
	Str31	menustring;
	Boolean	isColorLine;
	RGBColor	lineColor;
	short	selectionGroup;
	short	keywordGroup;
	RGBColor	keywordColor;
	Boolean	local;
	Boolean	localBorder;
	Str255	info;
};

struct SLegacyKeyBindHeader 
{
	short	keyBindCount;
};

//#688
//互換性用定義
//2.1まではtypedef EKeyAction AKeyBindAction, enum EKeyAction{...}と定義していたが、
//v3でtypedef AIndex AKeyBindActionに変更
enum EKeyAction 
{
	dummy = 0
};

struct SLegacyKeyBind 
{
	Boolean	controlKey;
	Boolean	optionKey;
	Boolean	commandKey;
	Boolean	shiftKey;
	short	keyCode;
	EKeyAction  action;
};

enum ELegacyPrintPosition {
	printPosition_LeftTop = 0,
	printPosition_Top,
	printPosition_RightTop,
	printPosition_TopEvenOdd,
	printPosition_LeftBottom,
	printPosition_Bottom,
	printPosition_RightBottom,
	printPosition_BottomEvenOdd
};

struct SLegacyPrintFormPref {
	Boolean	usePrintFont;
	Boolean	printPageNumber;
	Boolean	printFileName;
	Boolean	printLineNumber;
	ELegacyPrintPosition	pagePosition;
	short	printFont,printSize;
	short	pageFont,pageSize;
	short	fileFont,fileSize;
	short	lineFont;//,lineSize;
	Boolean	printSeparateLine;
	EWrapMode	wrapMode;
	Int16	betweenLine;
};

struct SLegacyPrintFormPref20b6 {
	Boolean	usePrintFont;
	Boolean	printPageNumber;
	Boolean	printFileName;
	Boolean	printLineNumber;
	ELegacyPrintPosition	pagePosition;
	short	printFont,printSize;
	short	pageFont,pageSize;
	short	fileFont,fileSize;
	short	lineFont;//,lineSize;
	Boolean	printSeparateLine;
	EWrapMode	wrapMode;
};

enum ERulerScale {
	rulerScale_Tab = 0,
	rulerScale_5 = 5
};

struct SPrefC {
	Boolean	recognizeGlobal;
	Boolean	recognizeLocal;
	Boolean	recognizeExternal;
	Boolean	useIdentifierResource;
	Boolean	displayInfoTag;
	Boolean	useTRTemplate;
	Boolean	displayLineNumber;
	Boolean	displayFunction;
	Boolean	displayEachLineNumber;
	short	normalIndent;
	short	labelIndent;
	short	fukubunIndent;
	RGBColor	functionColor;
	RGBColor	externalFunctionColor;
	RGBColor	classColor;
	RGBColor	globalVariableColor;
	RGBColor	localVariableColor;
	RGBColor	externalVariableColor;
	RGBColor	commentColor;
	RGBColor	literalColor;
	RGBColor	enumtypeColor;
	RGBColor	enumColor;
	RGBColor	structColor;
	RGBColor	unionColor;
	RGBColor	defineColor;
	RGBColor	typedefColor;
};

struct SMacLegacyModePref {//2.1.8以降
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	OBSOLETE_printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
	short	dum1;
	Str255	backImagePath;
	Boolean	shadowText;
	Boolean	displayZenkakuSpace;
	RGBColor	normalColor;
	RGBColor	backColor;
	Boolean	attachDOSEOF;
	Boolean	displayOpenWell;
	short dum2;
	Boolean	spacedTab;
	Boolean	masume;
	short dum3;
	Boolean	saveWindowPosition;
	FSSpec	texCompiler;
	FSSpec	texPreviewer;
	short	dum4;//設定追加時は必ずダミーshortを入れること 
	Boolean	attachUTF8BOM;//B0034
	Boolean	dontSaveResource;
	Boolean	saveYenAsBackslash;//B0014
	short	dum5;
	//2.1.5
	CFStringEncoding	OBSOLETE_internalEncoding;//R0043
	Boolean	useUnicode;//R0043
	Boolean	useDefaultEncoding;//R0043
	EKanjiCode	defaultEncoding;//R0043
	Boolean	displaySpace;//R0038
	Boolean	displayTab;//R0027
	Boolean	displayReturn;//R0027
	short	dum6;//設定追加時は必ずダミーshortを入れること 
	//2.1.6b1
	Str255	texCompilerPath;//B0089
	Boolean	useMiWordSelect;//R0036
	Boolean	charsetRecognize;//R0045
	Boolean	correctEncoding;//R0045
	Boolean	checkCharsetAfterSave;//R0045
	Boolean	sniffRecognize;//R0045
	short	dum7;
	//2.1.6b3
	Boolean	defalutOpenJumpList;//R0047
	short	dum8;
	//2.1.7b1
	Boolean	changeYenToBackslash;//R-uni
	short	antiAliasMinSize;//R-uni
	TextEncoding defaultTextEncoding;//R-uni
	Boolean	useLegacySniff;//R-uni
	Boolean	useBOMSniff;//R-uni
	Str255	rulerStringUTF8;//R-uni
	TextEncoding legacyCopyTextEncoding;//R-uni
	Boolean	displayYenFor5CWhenJIS;//R-uni
	Boolean	save5CForYenWhenJIS;//R-uni
	short	dum9;
	//2.1.8
	//R-abs Boolean	UseRegExpCommentStart;//R0128
	//R-abs Boolean	UseRegExpCommentEnd;//R0128
};

struct SMacLegacyModePref217 {//R0128
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	OBSOLETE_printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
	short	dum1;
	Str255	backImagePath;
	Boolean	shadowText;
	Boolean	displayZenkakuSpace;
	RGBColor	normalColor;
	RGBColor	backColor;
	Boolean	attachDOSEOF;
	Boolean	displayOpenWell;
	short dum2;
	Boolean	spacedTab;
	Boolean	masume;
	short dum3;
	Boolean	saveWindowPosition;
	FSSpec	texCompiler;
	FSSpec	texPreviewer;
	short	dum4;//設定追加時は必ずダミーshortを入れること 
	Boolean	attachUTF8BOM;//B0034
	Boolean	dontSaveResource;
	Boolean	saveYenAsBackslash;//B0014
	short	dum5;
	//2.1.5
	CFStringEncoding	OBSOLETE_internalEncoding;//R0043
	Boolean	useUnicode;//R0043
	Boolean	useDefaultEncoding;//R0043
	EKanjiCode	defaultEncoding;//R0043
	Boolean	displaySpace;//R0038
	Boolean	displayTab;//R0027
	Boolean	displayReturn;//R0027
	short	dum6;//設定追加時は必ずダミーshortを入れること 
	//2.1.6b1
	Str255	texCompilerPath;//B0089
	Boolean	useMiWordSelect;//R0036
	Boolean	charsetRecognize;//R0045
	Boolean	correctEncoding;//R0045
	Boolean	checkCharsetAfterSave;//R0045
	Boolean	sniffRecognize;//R0045
	short	dum7;
	//2.1.6b3
	Boolean	defalutOpenJumpList;//R0047
	short	dum8;
	//2.1.7b1
	Boolean	changeYenToBackslash;//R-uni
	short	antiAliasMinSize;//R-uni
	TextEncoding defaultTextEncoding;//R-uni
	Boolean	useLegacySniff;//R-uni
	Boolean	useBOMSniff;//R-uni
	Str255	rulerStringUTF8;//R-uni
	TextEncoding legacyCopyTextEncoding;//R-uni
	Boolean	displayYenFor5CWhenJIS;//R-uni
	Boolean	save5CForYenWhenJIS;//R-uni
};

struct SMacLegacyModePref216b5 {//2.1.6b1
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
	short	dum1;
	Str255	backImagePath;
	Boolean	shadowText;
	Boolean	displayZenkakuSpace;
	RGBColor	normalColor;
	RGBColor	backColor;
	Boolean	attachDOSEOF;
	Boolean	displayOpenWell;
	short dum2;
	Boolean	spacedTab;
	Boolean	masume;
	short dum3;
	Boolean	saveWindowPosition;
	FSSpec	texCompiler;
	FSSpec	texPreviewer;
	short	dum4;//設定追加時は必ずダミーshortを入れること 
	Boolean	attachUTF8BOM;//B0034
	Boolean	dontSaveResource;
	Boolean	saveYenAsBackslash;//B0014
	short	dum5;
	//2.1.5
	CFStringEncoding	internalEncoding;//R0043
	Boolean	useUnicode;//R0043
	Boolean	useDefaultEncoding;//R0043
	EKanjiCode	defaultEncoding;//R0043
	Boolean	displaySpace;//R0038
	Boolean	displayTab;//R0027
	Boolean	displayReturn;//R0027
	short	dum6;//設定追加時は必ずダミーshortを入れること 
	//2.1.6b1
	Str255	texCompilerPath;//B0089
	Boolean	useMiWordSelect;//R0036
	Boolean	charsetRecognize;//R0045
	Boolean	correctEncoding;//R0045
	Boolean	checkCharsetAfterSave;//R0045
	Boolean	sniffRecognize;//R0045
	short	dum7;
	//2.1.6b3
	Boolean	defalutOpenJumpList;//R0047
};

struct SMacLegacyModePref216b1 {//2.1.6b1
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
	short	dum1;
	Str255	backImagePath;
	Boolean	shadowText;
	Boolean	displayZenkakuSpace;
	RGBColor	normalColor;
	RGBColor	backColor;
	Boolean	attachDOSEOF;
	Boolean	displayOpenWell;
	short dum2;
	Boolean	spacedTab;
	Boolean	masume;
	short dum3;
	Boolean	saveWindowPosition;
	FSSpec	texCompiler;
	FSSpec	texPreviewer;
	short	dum4;//設定追加時は必ずダミーshortを入れること 
	Boolean	attachUTF8BOM;//B0034
	Boolean	dontSaveResource;
	Boolean	saveYenAsBackslash;//B0014
	short	dum5;
	//2.1.5
	CFStringEncoding	internalEncoding;//R0043
	Boolean	useUnicode;//R0043
	Boolean	useDefaultEncoding;//R0043
	EKanjiCode	defaultEncoding;//R0043
	Boolean	displaySpace;//R0038
	Boolean	displayTab;//R0027
	Boolean	displayReturn;//R0027
	short	dum6;//設定追加時は必ずダミーshortを入れること 
	//2.1.6b1
	Str255	texCompilerPath;//B0089
	Boolean	useMiWordSelect;//R0036
	Boolean	charsetRecognize;//R0045
	Boolean	correctEncoding;//R0045
	Boolean	checkCharsetAfterSave;//R0045
	Boolean	sniffRecognize;//R0045
};

struct SMacLegacyModePref215 {//2.1.5
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
	short	dum1;
	Str255	backImagePath;
	Boolean	shadowText;
	Boolean	displayZenkakuSpace;
	RGBColor	normalColor;
	RGBColor	backColor;
	Boolean	attachDOSEOF;
	Boolean	displayOpenWell;
	short dum2;
	Boolean	spacedTab;
	Boolean	masume;
	short dum3;
	Boolean	saveWindowPosition;
	FSSpec	texCompiler;
	FSSpec	texPreviewer;
	short	dum4;//設定追加時は必ずダミーshortを入れること 
	Boolean	attachUTF8BOM;//B0034
	Boolean	dontSaveResource;
	Boolean	saveYenAsBackslash;//B0014
	short	dum5;
	//2.1.5
	CFStringEncoding	internalEncoding;//R0043
	Boolean	useUnicode;//R0043
	Boolean	useDefaultEncoding;//R0043
	EKanjiCode	defaultEncoding;//R0043
	Boolean	displaySpace;//R0038
	Boolean	displayTab;//R0027
	Boolean	displayReturn;//R0027
};

struct SMacLegacyModePref213 {//2.1.3//R0043
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
	short	dum1;
	Str255	backImagePath;
	Boolean	shadowText;
	Boolean	displayZenkakuSpace;
	RGBColor	normalColor;
	RGBColor	backColor;
	Boolean	attachDOSEOF;
	Boolean	displayOpenWell;
	short dum2;
	Boolean	spacedTab;
	Boolean	masume;
	short dum3;
	Boolean	saveWindowPosition;
	FSSpec	texCompiler;
	FSSpec	texPreviewer;
	short	dum4;//設定追加時は必ずダミーshortを入れること 
	Boolean	attachUTF8BOM;//B0034
	Boolean	dontSaveResource;
	Boolean	saveYenAsBackslash;//B0014
};

struct SMacLegacyModePref21b13 {//2.1b13 //B0034
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
	short	dum1;
	Str255	backImagePath;
	Boolean	shadowText;
	Boolean	displayZenkakuSpace;
	RGBColor	normalColor;
	RGBColor	backColor;
	Boolean	attachDOSEOF;
	Boolean	displayOpenWell;
	short dum2;
	Boolean	spacedTab;
	Boolean	masume;
	short dum3;
	Boolean	saveWindowPosition;
	FSSpec	texCompiler;
	FSSpec	texPreviewer;
};

struct SMacLegacyModePref21b9 {//2.1b9
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
	short	dum1;
	Str255	backImagePath;
	Boolean	shadowText;
	Boolean	displayZenkakuSpace;
	RGBColor	normalColor;
	RGBColor	backColor;
	Boolean	attachDOSEOF;
	Boolean	displayOpenWell;
	short dum2;
	Boolean	spacedTab;
	Boolean	masume;
};

struct SMacLegacyModePref21b8 {//2.1b8
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
	short	dum1;
	Str255	backImagePath;
	Boolean	shadowText;
	Boolean	displayZenkakuSpace;
	RGBColor	normalColor;
	RGBColor	backColor;
	Boolean	attachDOSEOF;
	Boolean	displayOpenWell;
};

struct SMacLegacyModePref21b5{//2.1b5
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
	short	dum1;
	Str255	backImagePath;
	Boolean	shadowText;
	Boolean	reserved;
};

struct SMacLegacyModePref21b3 {//2.1b3
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
	Str31	otherStart1;
	Str31	otherEnd1;
	Str31	otherStart2;
	Str31	otherEnd2;
	Boolean	countReturn;
	Boolean	countSpace;
	Boolean	countReserve1;
	Boolean	countReserve2;
	Boolean	countReserve3;
	Boolean	countReserve4;
};

struct SMacLegacyModePref21b2 {//2.1b2
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
	Str31	commentStart;
	Str31	commentEnd;
	Str31	commentLine;
	Str31	literalStart1;
	Str31	literalEnd1;
	Str31	literalStart2;
	Str31	literalEnd2;
	Str31	literalEscape;
	RGBColor	commentColor;
	RGBColor	literalColor;
	Boolean	indentChar[128];
	Boolean	recognizeC;
	Boolean	indentC;
	SPrefC	prefC;
	Boolean	displayToolBar;
};

struct SMacLegacyModePref20b9 {//20b9
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
	Str255	tailAlphabetString;
	Boolean	smartDrag;
	Str255	rulerString;
};
struct SMacLegacyModePref20b7 {//20b7
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
	Int16	betweenLine;
	Boolean	forceMimi;
	Str255	alphabetString;
	Str255	headAlphabetString;
	Boolean	keyBind;
};

struct SMacLegacyModePref20b6 {
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref20b6	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
	Boolean	LCWrap;
	short	LCWrapCount;
	Boolean	displayRuler;
	ERulerScale	rulerScale;
	char	rulerLetter;
	Boolean	tripleClick2;
	Str255	tripleClick2String;
};

struct SMacLegacyModePref20b5 {
	short	font;
	short	size;
	short	windowwidth;
	short	windowheight;
	short	tabwidth;
	EKanjiCode	kanjicode;
	EReturnCode	returncode;
	EWrapMode	wrapmode;
	OSType	creator;
	SLegacyPrintFormPref20b6	printForm;
	TPrint	printRecord;
	Str31	backfilename;
	Boolean	background;
	Boolean	displayEachLineNumber;
	Boolean	displayEachParagraphNumber;
	Boolean	displayCurrentLineNumber;
	Boolean	displayCurrentParagraphNumber;
	Boolean	displayHandFreeScroll;
	Boolean	displayInfoTag;
	Boolean	displayCurrentPosition;
	Boolean	newDocumentDialog;
	Boolean	kakkoTaio;
	Boolean	insideKakkoSelection;
	Boolean	mimikakiWrap;
	Boolean	jumpScroll;
	Int16	jumpScrollStart;
	Int16	jumpScrollLines;
	Boolean	forceRoman;
	Boolean	useDragDrop;
	Boolean	displaySpecialCharacter;
	RGBColor	specialCharacterColor;
};

#pragma mark ---DocPref構造体

struct SLegacyDocPref 
{
	short	docPrefVersion;
	short		font;
	short		size;
	Boolean		caretMode;
	SPoint32	caretPosition;
	SPoint32	selectPosition;
	EKanjiCode	kanjiCode;
	EReturnCode	returnCode;
	EWrapMode	wrapMode;
	Int16	tabwidth;
	Int16	windowwidth;
	Int16	windowheight;
	SLegacyPrintFormPref	printForm;
	TPrint	OBSOLETE_printRecord;
	Str31	modeString;
	Boolean	LCWrap;
	short	LCWrapCount;
	/* 2.1b13 */
	Point	windowPosition;
	// 2.1.7b8
	short	dummy;
	EAntiAliasMode	antiAliasMode;	//R0105
	Int16	OBSOLETE_pixelTabWidth;	//R0008
};

struct SLegacyDocPref21b3 {
	short		font;
	short		size;
	Boolean		caretMode;
	SPoint32	caretPosition;
	SPoint32	selectPosition;
	EKanjiCode	kanjiCode;
	EReturnCode	returnCode;
	EWrapMode	wrapMode;
	Int16	tabwidth;
	Int16	windowwidth;
	Int16	windowheight;
	SLegacyPrintFormPref	printForm;
	TPrint	printRecord;
	Str31	modeString;
	Boolean	LCWrap;
	short	LCWrapCount;
};

struct SLegacyDocPref20b6 {
	short		font;
	short		size;
	Boolean		caretMode;
	SPoint32	caretPosition;
	SPoint32	selectPosition;
	EKanjiCode	kanjiCode;
	EReturnCode	returnCode;
	EWrapMode	wrapMode;
	Int16	tabwidth;
	Int16	windowwidth;
	Int16	windowheight;
	SLegacyPrintFormPref20b6	printForm;
	TPrint	printRecord;
	Str31	modeString;
	Boolean	LCWrap;
	short	LCWrapCount;
};

#endif
class AUniversalBinaryUtility
{
  public:
#ifndef MODEPREFCONVERTER
	//mi本体用処理 #1034
	static void	SwapBigToHost( int16_t &ioValue );//#1034
	static void	SwapHostToBig( int16_t &ioValue );//#1034
#else
	//modeprefconverter用処理 #1034
	static void	SwapBigToHost( RGBColor &ioValue );
	static void	SwapBigToHost( Rect &ioValue );
	static void	SwapBigToHost( FSSpec &ioValue );
	//#1034 static void	SwapBigToHost( SPoint32 &ioValue );
	static void	SwapBigToHost( UInt16 &ioValue );
	static void	SwapBigToHost( SInt16 &ioValue );
	static void	SwapBigToHost( UInt32 &ioValue );
	static void	SwapBigToHost( SInt32 &ioValue );
	static void	SwapBigToHost( Point &ioValue );
	
	static void	SwapHostToBig( RGBColor &ioValue );
	static void	SwapHostToBig( Rect &ioValue );
	static void	SwapHostToBig( FSSpec &ioValue );
	//#1034 static void	SwapHostToBig( SPoint32 &ioValue );
	static void	SwapHostToBig( UInt16 &ioValue );
	static void	SwapHostToBig( SInt16 &ioValue );
	static void	SwapHostToBig( UInt32 &ioValue );
	static void	SwapHostToBig( SInt32 &ioValue );
	static void	SwapHostToBig( Point &ioValue );
#endif
};

