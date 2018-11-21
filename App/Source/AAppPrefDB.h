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

AAppPrefDB

*/

#pragma once

#include "../../AbsFramework/AbsBase/ADataBase.h"
#include "ATypes.h"
#if IMPLEMENTATION_FOR_MACOSX
#include "AMacLegacyPref.h"
#endif
#ifndef MODEPREFCONVERTER
#import <Sparkle/SUUpdater.h>
#endif

/**
候補キータイプ
*/
enum AAbbrevKeyType
{
	kAbbrevKeyType_KeyBind = 0,
	kAbbrevKeyType_ShiftSpace = 1,
	kAbbrevKeyType_ShiftTab = 2,
	kAbbrevKeyType_Tab = 3,
	kAbbrevKeyType_Escape = 4,
	kAbbrevKeyType_ShiftEscape = 5,
};

/**
app pref
*/
class AAppPrefDB : public ADataBase
{
  public:
	AAppPrefDB();
	void	Setup();
	//#844 void	SetupAfterModeInit();
	void	SaveToFile();
	
	ABool	GetFTPPassword( AText& inServer, AText& inUser, AText& outPassword );
	
	/*#844
	void	SetDefaultTextEncodingMenu( AConstCharPtr inString );
	void	SetDefaultTextEncodingMenu();
	const ATextArray&	GetTextEncodingMenuFixedTextArrayRef() const;
	ABool	ExistTextEncodingMenu( const ATextEncoding& inTextEncoding ) const;
	ATextEncoding	GetTextEncodingFromMenuItemIndex( AIndex inIndex ) const;
	AIndex	GetMenuItemIndexFromTextEncoding( const ATextEncoding& inTextEncoding ) const;
	*/
	
	//FTP
	AIndex	FindFTPAccountPrefIndex( const AText& inServer, const AText& inUser ) const;
	void	GetHTTPURLFromFTPURL( const AText& inFTPURL, AText& outHTTPURL ) const;
	//ABool	GetInternetPassword( AText& inServer, AText& inUser, AText& outPassword ) const;
	//void	SetInternetPassword( AText& inServer, AText& inUser, AText& inPassword ) const;
	
	//#725
	//サイドバーデフォルト
  public:
	void	SetDefaultRightSideBarArray();
	void	SetDefaultLeftSideBarArray();
	
	//#844
	//TextEncoding
  public:
	const ATextArray&		GetTextEncodingNameArray() const { return mTextEncodingNameArray; }
	const ATextArray&		GetTextEncodingDisplayNameArray() const { return mTextEncodingDisplayNameArray; }
	ABool					IsTextEncodingAvailable( const AText& inTextEncodingName ) const;
	void					ConvertToNormalizedTextEncodingName( AText& ioName ) const;//#1040
  private:
	void					InitTextEncoding();
	ATextArray				mTextEncodingNameArray;
	ATextArray				mTextEncodingDisplayNameArray;
	AHashArray<ATextEncoding>	mTextEncodingArray;//#1040
	
	//#1102
	//自動更新
  public:
	void					SPI_UpdateSparkleUpdateLevel();
	
	//
  public:
	//AAppPrefDBからデータをGet/Setするときのkey
	//AWindow_AppPrefの各コントロールのControlIDを兼ねている。
	//番号は101番以降それぞれ重複しない番号をとる。ウインドウの各部品のIDと連動している。
	const static APrefID	kDisplayControlCode					= 101;
	const static APrefID	kCountAs2Letters					= 102;
	const static APrefID	kInsertLineMarginForAntiAlias		= 103;
	const static APrefID	kBunsetsuSelect						= 107;
	const static APrefID	kCMMouseButton						= 108;
	const static APrefID	kCMMouseButtonTick					= 109;
	const static APrefID	kWheelScrollPercent					= 110;
	const static APrefID	kWheelScrollPercentCmd				= 111;
	const static APrefID	kFind_FontSize						= 115;
	const static APrefID	kFindDefaultIgnoreCase				= 116;
	const static APrefID	kFindDefaultWholeWord				= 117;
	const static APrefID	kFindDefaultIgnoreBackslashYen		= 118;
	const static APrefID	kFindDefaultAimai					= 119;
	const static APrefID	kFindDefaultRegExp					= 120;
	const static APrefID	kFindDefaultAllowReturn				= 121;
	const static APrefID	kFindDefaultLoop					= 122;
	const static APrefID	kFindDefaultRecursive				= 123;
	const static APrefID	kDisplayFindHighlight				= 124;
	const static APrefID	kJumpListPosition					= 129;
	const static APrefID	kPrintColored						= 141;
	const static APrefID	kPrintBlackForWhite					= 142;
	const static APrefID	kLockWhileEdit						= 143;
	const static APrefID	kUseShellScript						= 144;
	const static APrefID	kStdOutModeName						= 145;
	const static APrefID	kUserID								= 147;
	const static APrefID	kKinsokuLetters						= 148;
	const static APrefID	kDisableHorizontalWheelScroll			= 149;
	const static APrefID	kCmdNModeName						= 150;
	const static APrefID	kFindDefaultDisplayPosition			= 151;
	const static APrefID	kThanks								= 163;
	const static APrefID	kFTPAccount_User					= 164;
	const static APrefID	kRecentlyOpenedFileComment			= 166;
	const static APrefID	kRecentlyOpenedFileCommentPath		= 167;
	const static APrefID	kTabToWindowPositionOffset			= 211;
	const static APrefID	kTexCompilerPath					= 212;
	const static APrefID	kTexPreviewerPath 					= 213;
	const static APrefID	kFindDefaultOnlyVisibleFile			= 218;//B0313
	const static APrefID	kFindDefaultOnlyTextFile			= 219;//B0313
	const static APrefID	kBigCaret							= 221;//R0184
	const static APrefID	kUseSCM								= 226;//R0006
	//#379 const static APrefID	kCompareWithLatestWhenOpen			= 229;//R0006
	//#379 const static APrefID	kCompareWithLatestWhenSave			= 230;//R0006
	const static APrefID	kCVSPath							= 231;//R0006
	const static APrefID	kDiffColor_Changed					= 232;//R0006
	const static APrefID	kDiffColor_Added					= 233;//R0006
	const static APrefID	kDiffColor_Deleted					= 234;//R0006
	const static APrefID	kFindDialogFontName					= 241;//win
	const static APrefID	kFileListDontDisplayInvisibleFile	= 243;//win 080702
	const static APrefID	kSelectWordByContextMenu			= 244;//B0432
	const static APrefID	kDisplayFindHighlight2				= 246;//R0244
	const static APrefID	kSVNPath							= 248;//R0247
	const static APrefID	kDiffPath							= 249;//R0247
	const static APrefID	kDiffOption							= 250;//R0247
	const static APrefID	kDisplayFindResultInTextWindow		= 252;//#92
	const static APrefID	kFindShortcutIsFocusSearchBox		= 254;//#137
	const static APrefID	kCheckEncodingError_KishuIzon		= 255;//#208
	const static APrefID	kCheckEncodingError_HankakuKana		= 256;//#208
	const static APrefID	kCandidateWindowKeyBind				= 257;//#204
	const static APrefID	kSelectTextWindowAfterFind			= 260;//#269
	const static APrefID	kPrintEachParagraphAlways			= 292;//#334
	//#379 const static APrefID	kUseExternalDiffForSubversion		= 293;//#335
	const static APrefID	kDisplayAmazonButton				= 294;//amazon
	const static APrefID	kDevTool_DmgMaker_Version			= 297;//#436
	const static APrefID	kDevTool_DmgMaker_Folder			= 298;//#436
	const static APrefID	kDevTool_DmgMaker_Master			= 299;//#436
	const static APrefID	kFindDefaultIgnoreSpaces			= 300;//#165
	const static APrefID	kFuzzyFind_NFKD						= 301;//#166
	const static APrefID	kFuzzyFind_KanaType					= 302;//#166
	const static APrefID	kFuzzyFind_Onbiki					= 303;//#166
	const static APrefID	kFuzzyFind_KanaYuragi				= 304;//#166
	const static APrefID	kLaunguage							= 305;//win
	//#450 const static APrefID	kLineFolding						= 306;//#450
	const static APrefID	kCloseAllTabsWhenCloseWindow		= 308;//#510
	const static APrefID	kReopenFile							= 309;//#513
	const static APrefID	kShowDiffTargetWithWordWrap			= 310;//#379
	//#653 const static APrefID	kDontShowEditByOhterAppDialog		= 314;//#545
	const static APrefID	kInfoPaneSideBarMode				= 315;//#634
	const static APrefID	kInfoPaneSideBarModeColor			= 316;//#634
	const static APrefID	kSaveSubPaneModeIntoDocument		= 317;//#668
	//アンチエイリアス
	const static APrefID	kEnableAntiAlias					= 401;//#844
	//デフォルトウインドウサイズ
	const static APrefID	kDefaultWindowWidth					= 402;//#844
	const static APrefID	kDefaultWindowHeight				= 403;//#844
	//印刷オプション
	const static APrefID	kPrintOption_PrintFontName			= 404;//#844
	const static APrefID	kPrintOption_PrintFontSize			= 405;//#844
	const static APrefID	kPrintOption_PageNumberFontName		= 406;//#844
	const static APrefID	kPrintOption_PageNumberFontSize		= 407;//#844
	const static APrefID	kPrintOption_FileNameFontName		= 408;//#844
	const static APrefID	kPrintOption_FileNameFontSize		= 409;//#844
	const static APrefID	kPrintOption_LineNumberFontName		= 410;//#844
	const static APrefID	kPrintOption_LineNumberFontSize		= 411;//#844
	const static APrefID	kPrintOption_LineMargin				= 412;//#844
	const static APrefID	kPrintOption_UsePrintFont			= 413;//#844
	const static APrefID	kPrintOption_PrintPageNumber		= 414;//#844
	const static APrefID	kPrintOption_PrintFileName			= 415;//#844
	const static APrefID	kPrintOption_PrintLineNumber		= 416;//#844
	const static APrefID	kPrintOption_PrintSeparateLine		= 417;//#844
	const static APrefID	kPrintOption_ForceWordWrap			= 418;//#844
	//印刷余白
	const static APrefID	kPrintMargin_Left					= 423;//R0242
	const static APrefID	kPrintMargin_Right					= 424;//R0242
	const static APrefID	kPrintMargin_Top					= 425;//R0242
	const static APrefID	kPrintMargin_Bottom					= 426;//R0242
	//キャレット・現在行表示
	const static APrefID	kDrawVerticalLineAsCaret			= 427;//#844
	const static APrefID	kDrawLineForCurrentLine				= 428;//#844
	const static APrefID	kCurrentLineColor					= 429;//#844
	//候補ポップアップ表示 #717
	const static APrefID	kPopupCandidateBelowInputText		= 430;
	const static APrefID	kPopupCandidateNearInputText		= 431;
	//unicode decomp #863
	const static APrefID	kDisplayEachCanonicalDecompChar		= 432;
	//const static APrefID	kFindDistinguishCanonicalCompDecomp	= 433;
	//現在行表示 #844
	const static APrefID	kPaintLineForCurrentLine			= 434;
	//キーワード表示ポップアップ
	const static APrefID	kPopupIdInfo						= 435;
	//禁則 #844 mode prefから移動
	const static APrefID	kKinsokuBuraasge					= 436;
	//自動バックアップ#81
	const static APrefID	kEnableAutoBackup					= 437;
	const static APrefID	kAutoBackupRootFolder				= 438;
	//バイナリファイル定義#723
	const static APrefID	kBinaryFileDefinition				= 439;
	//現在単語ハイライト表示
	const static APrefID	kDisplayCurrentWordHighlight		= 441;
	//キーワード情報ポップアップをしばらくしたら閉じる
	const static APrefID	kHideIdInfoPopupByIdle				= 442;
	//ステータスバー表示
	const static APrefID	kDisplayStatusBar					= 443;
	//編集時下余白#621
	const static APrefID	kBottomScrollAt25Percent			= 444;
	//
	const static APrefID	kCreateTabInsteadOfCreateWindow		= 445;
	//
	const static APrefID	kAbbrevInputByArrowKeyAfterPopup	= 446;
	//
	const static APrefID	kIdInfoAtNoteText					= 447;
	//#764
	const static APrefID	kUseGuessTextEncoding				= 448;
	//#830
	const static APrefID	kUseFallbackForTextEncodingConversion= 449;
	const static APrefID	kShowDialogWhenTextEncodingFallback	= 450;
	//#222
	const static APrefID	kSelectParagraphByTripleClick		= 451;
	const static APrefID	kSelectLineByClickLineNumber		= 452;
	//レイアウト #457
	const static APrefID	kShowHScrollBar						= 453;
	const static APrefID	kShowVScrollBar						= 454;
	const static APrefID	kShowMacroBar						= 455;
	/*#1160
	//候補キー
	const static APrefID	kAbbrevNextKey						= 456;
	const static APrefID	kAbbrevPrevKey						= 457;
	*/
	//TeX
	const static APrefID	kTexTypesetPath 					= 458;
	const static APrefID	kTexDviToPdfPath 					= 459;
	//skin
	const static APrefID	kSkinName							= 460;
	//ドット単位スクロール #891
	const static APrefID	kUseWheelScrollFloatValue			= 461;
	//Gitパス
	const static APrefID	kGitPath							= 462;
	//キャレットローカル固定 #1031
	const static APrefID	kFixCaretLocalPoint					= 463;
	//コピー／JIS保存時のCanonicalComp変換 #1044
	const static APrefID	kAutoConvertToCanonicalComp			= 464;
	//Sparkle自動更新レベル #1102
	const static APrefID	kSparkleUpdateLevel					= 465;
	//ウインドウタイトルフルパス表示 #1182
	const static APrefID	kDisplayFullPathTitleBar			= 466;
	//テキストの上であってもIビームカーソルにしない設定 #1208
	const static APrefID	kInhibitIbeamCursor					= 467;
	//左右余白 #1186
	const static APrefID	kDisplayLeftRightMargin				= 468;
	//左右余白テキスト幅 #1186
	const static APrefID	kTextWidthForLeftRightMargin		= 469;
	//半透明ウインドウ#1255
	const static APrefID	kAlphaWindowModeV3					= 470;
	const static APrefID	kAlphaWindowPercent1V3				= 471;
	const static APrefID	kAlphaWindowPercent2V3				= 472;
	//タブに親フォルダ名表示 #1334
	const static APrefID	kTabShowParentFolder				= 473;
	//検索結果ポップアップしないオプション #1322
	const static APrefID	kDontShowFindResultPopup			= 474;
	//タブ幅 #1349
	const static APrefID	kTabWidth							= 475;
	
	//AWindow_AppPrefでは設定項目が無いもの（ウインドウ位置等）
	//番号は1001番以降それぞれ重複しない番号をとる。（この番号は他の箇所には影響しない）
	const static APrefID	kPreferenceWindowPosition			= 1001;
	const static APrefID	kRecentlyOpenedFile_old				= 1002;//v2.1でのデータ
	const static APrefID	kMultiFileFindProgressPosition		= 1003;
	const static APrefID	kTextEncodingMenuFixedTextArray		= 1004;
	const static APrefID	kMultiFileFindWindowPosition		= 1005;
	const static APrefID	kMultiFileFind_RecentlyUsedFindString	= 1006;
	const static APrefID	kMultiFileFind_RecentlyUsedFileFilter	= 1007;
	const static APrefID	kMultiFileFind_RecentlyUsedFolder	= 1008;
	const static APrefID	kModePrefWindowPosition				= 1009;
	const static APrefID	kFindMemoryWindow_Width				= 1010;
	const static APrefID	kFindMemoryWindow_Height			= 1011;
	const static APrefID	kFTPFolderWindowPosition			= 1013;
	const static APrefID	kFolderListWindowPosition			= 1014;
	const static APrefID	kFTPFolder_Server					= 1015;
	const static APrefID	kFTPFolder_User						= 1016;
	const static APrefID	kFTPFolder_Path						= 1017;
	const static APrefID	kFTPFolder_Depth					= 1018;
	const static APrefID	kFTPLogWindowPosition				= 1019;
	const static APrefID	kFTPProgressPosition				= 1020;
	const static APrefID	kTraceLogWindowPosition				= 1021;
	const static APrefID	kIndexWindowWidth					= 1022;
	const static APrefID	kIndexWindowHeight					= 1023;
	const static APrefID	kIndexWindowSeparateLine			= 1024;
	const static APrefID	kFindDefaultFileFilter				= 1025;
	const static APrefID	kJumpListOffsetX					= 1027;
	const static APrefID	kJumpListOffsetY					= 1028;
	const static APrefID	kJumpListSizeX						= 1029;
	const static APrefID	kJumpListSizeY						= 1030;
	const static APrefID	kWindowListOffsetX					= 1031;
	const static APrefID	kWindowListOffsetY					= 1032;
	const static APrefID	kWindowListSizeX					= 1033;
	const static APrefID	kWindowListSizeY					= 1034;
	const static APrefID	kDisplayWindowList					= 1035;
	const static APrefID	kSingleWindowBounds					= 1036;
	const static APrefID	kWindowListMode						= 1037;
	const static APrefID	kFindMemory_DefaultVisible			= 1038;
	const static APrefID	kFindMemory_DecresingOrder			= 1039;
	const static APrefID	kFindMemory_NumberWidth				= 1040;
	const static APrefID	kFindMemory_NameWidth				= 1041;
	const static APrefID	kFindMemory_FindStringWidth			= 1042;
	const static APrefID	kFindMemory_ReplaceStringWidth		= 1043;
	const static APrefID	kFindMemory_OffsetFromFindWindow	= 1044;
	const static APrefID	kFind_WindowWidth					= 1047;
	const static APrefID	kJumpListParagraphColumnWidth		= 1048;
	const static APrefID	kFTPAccount_Server					= 1049;
	const static APrefID	kFTPAccount_Passive					= 1050;
	const static APrefID	kFTPAccount_ProxyServer				= 1052;
	const static APrefID	kFTPAccount_RootPath				= 1053;
	const static APrefID	kFTPAccount_HTTPURL					= 1054;
	const static APrefID	kFTPAccount_Port					= 1055;
	const static APrefID	kFTPAccount_ProxyMode				= 1056;
	const static APrefID	kFTPAccount_ProxyUser				= 1057;
	const static APrefID	kFindWindowPosition					= 1060;
	const static APrefID	kMultiFileFindStringCache			= 1061;
	const static APrefID	kMultiFileFindFolderStock			= 1062;
	const static APrefID	kFTPAccount_BinaryMode				= 1063;
	const static APrefID	kFolderListMenu						= 1064;
	const static APrefID	kFindStringCache					= 1065;
	const static APrefID	kReplaceStringCache					= 1066;
	const static APrefID	kFindMemoryName						= 1067;
	const static APrefID	kFindMemoryFindText					= 1068;
	const static APrefID	kFindMemoryReplaceText				= 1069;
	const static APrefID	kFindMemoryIgnoreCase				= 1070;
	const static APrefID	kFindMemoryWholeWord				= 1071;
	const static APrefID	kFindMemoryAimai					= 1072;
	const static APrefID	kFindMemoryRegExp					= 1073;
	const static APrefID	kFindMemoryLoop						= 1074;
	const static APrefID	kJumpListParagraphWidth				= 1075;
	const static APrefID	kFileListPosition					= 1076;
	const static APrefID	kIndexWindowPosition				= 1077;
	const static APrefID	kSameProjectFolderPath				= 1078;
	const static APrefID	kSameProjectFolderModeIndex			= 1079;
	const static APrefID	kSameFolderPath						= 1080;
	const static APrefID	kTextCountWindowPosition			= 1081;
	const static APrefID	kTextCountConditionLetterCount		= 1082;
	const static APrefID	kTextCountConditionLineCount		= 1083;
	const static APrefID	kTextCountCountAs2Letters			= 1084;
	const static APrefID	kMultiFileFind_ActiveFolderKind		= 1085;
	const static APrefID	kTextCountCountSpaceTab				= 1086;
	const static APrefID	kTextCountCountReturn				= 1087;
	const static APrefID	kMultiFileFind_UseFileFilter		= 1092;//B0414
	//#361 const static APrefID	kFTPAccount_Password				= 1095;//win 080802
	const static APrefID	kIdInfoWindowPosition				= 1096;//RC2
	const static APrefID	kIdInfoWindowWidth					= 1097;//RC2
	const static APrefID	kIdInfoWindowHeight					= 1098;//RC2
	const static APrefID	kDisplayIdInfoWindow				= 1099;//RC2
	const static APrefID	kCandidateWindowWidth				= 1100;//RC2
	const static APrefID	kCandidateWindowHeight				= 1101;//RC2
	const static APrefID	kKeyToolListWindowWidth				= 1102;//R0073
	const static APrefID	kKeyToolListWindowHeight			= 1103;//R0073
	const static APrefID	kFusenListWindowPosition			= 1104;//#138
	const static APrefID	kFusenListWindowWidth				= 1105;//#138
	const static APrefID	kFusenListWindowHeight				= 1106;//#138
	const static APrefID	kDisplayFusenList					= 1107;//#138
	const static APrefID	kFindResultViewHeight				= 1108;//#92
	const static APrefID	kSubPaneDisplayed					= 1109;//#212
	const static APrefID	kSubPaneWidth						= 1110;//#212
	const static APrefID	kInfoPaneDisplayed					= 1112;//#291
	const static APrefID	kInfoPaneWidth						= 1113;//#291
	const static APrefID	kKeyRecordWindowPosition			= 1119;//#390
	const static APrefID	kKeyRecordWindowWidth				= 1120;//#390
	const static APrefID	kKeyRecordWindowHeight				= 1121;//#390
	const static APrefID	kKeyRecordRecentlyPlay				= 1122;//#390
	const static APrefID	kMultiFileFindFileFilterKind		= 1123;//#437
	const static APrefID	kSingleWindowBounds_UnzoomedBounds	= 1124;//win
	const static APrefID	kSingleWindowBounds_Zoomed			= 1125;//win
	const static APrefID	kFullScreenMode						= 1126;//#404
	const static APrefID	kCommitWindowWidth					= 1127;//#455
	const static APrefID	kCommitWindowHeight					= 1128;//#455
	const static APrefID	kCommitAdditionalArgs				= 1129;//#455
	const static APrefID	kCommitRecentMessage				= 1130;//#455
	const static APrefID	kSCMCommitWindowPosition			= 1131;//#455
	const static APrefID	kJumpListHistory					= 1132;//#454
	const static APrefID	kJumpListHistory_Path				= 1133;//#454
	const static APrefID	kDiffWindowPosition					= 1134;//#379
	const static APrefID	kDiffSourceFolder_Source			= 1135;//#379
	const static APrefID	kDiffSourceFolder_Working			= 1136;//#379
	const static APrefID	kDiffSourceFolder_Enable			= 1137;//#379
	const static APrefID	kDiffSourceFile						= 1138;//#379
	const static APrefID	kDiffWorkingFile					= 1139;//#379
	const static APrefID	kReopenFile_Path					= 1140;//#513
	const static APrefID	kReopenFile_Selected				= 1141;//#513
	const static APrefID	kReopenFile_IsMainWindow			= 1142;//#513
	const static APrefID	kDirectiveEnabledDefine				= 1143;//#467
	const static APrefID	kDirectiveEnabledDefineValue		= 1144;//#467
	const static APrefID	kEnabledDefinesWindowPosition		= 1145;//#467
	const static APrefID	kLuaConsoleWindowPosition			= 1146;//#567
	const static APrefID	kLuaConsoleWindowWidth				= 1147;//#567
	const static APrefID	kLuaConsoleWindowHeight				= 1148;//#567
	const static APrefID	kDisplayKeyRecordWindow				= 1149;//#679
	const static APrefID	kSubWindowsFontName					= 1150;//#725
	const static APrefID	kSubWindowsFontSize					= 1151;//#725
	const static APrefID	kSubWindowsAlpha_Floating			= 1153;//#725
	const static APrefID	kSubWindowsAlpha_Popup				= 1154;//#725
	const static APrefID	kDontRememberAnyHistory				= 1155;//#861
	const static APrefID	kFTPAccount_ConnectionType			= 1156;//#361
	const static APrefID	kRecentCompletionWordArray			= 1157;//#717
	const static APrefID	kFloatingSubWindowArray_Type		= 1158;//#725
	const static APrefID	kFloatingSubWindowArray_Parameter	= 1159;//#725
	const static APrefID	kFloatingSubWindowArray_Width		= 1160;//#725
	const static APrefID	kFloatingSubWindowArray_Height		= 1161;//#725
	const static APrefID	kFloatingSubWindowArray_X			= 1162;//#725
	const static APrefID	kFloatingSubWindowArray_Y			= 1163;//#725
	const static APrefID	kFindWindow_FindOptionsExpanded		= 1164;//#725
	const static APrefID	kFindWindow_ReplaceExpanded			= 1165;//#725
	const static APrefID	kFindWindow_AdvancedSearchExpanded	= 1166;//#725
	const static APrefID	kFindWindow_FindMemoryExpanded		= 1167;//#725
	const static APrefID	kReopenFile_WindowIndex				= 1168;//#850
	const static APrefID	kReopenFile_TabZOrder				= 1169;//#850
	const static APrefID	kReopenFile_TabDisplayIndex			= 1170;//#850
	const static APrefID	kReopenFile_IsRemoteFile			= 1171;//#850
	const static APrefID	kReopenFile_WindowWidth				= 1172;//#850
	const static APrefID	kReopenFile_WindowHeight			= 1173;//#850
	const static APrefID	kReopenFile_WindowX					= 1174;//#850
	const static APrefID	kReopenFile_WindowY					= 1175;//#850
	const static APrefID	kTextMarkerArray_Text				= 1176;//#750
	const static APrefID	kTextMarkerArray_Title				= 1177;//#750
	const static APrefID	kCallGrafHistory_FilePath			= 1178;//#723
	const static APrefID	kCallGrafHistory_ClassName			= 1179;//#723
	const static APrefID	kCallGrafHistory_FunctionName		= 1180;//#723
	const static APrefID	kCallGrafHistory_StartIndex			= 1181;//#723
	const static APrefID	kCallGrafHistory_EndIndex			= 1182;//#723
	const static APrefID	kCallGrafHistory_ModeName			= 1183;//#723
	const static APrefID	kCallGrafHistory_EdittedFlag		= 1184;//#723
	const static APrefID	kLeftSideBarDisplayed				= 1185;//#725
	const static APrefID	kLeftSideBarWidth					= 1186;//#725
	const static APrefID	kRightSideBarDisplayed				= 1187;//#725
	const static APrefID	kRightSideBarWidth					= 1188;//#725
	const static APrefID	kFileListCollapseFolder				= 1189;//#892
	const static APrefID	kFindWindow_WindowWidth				= 1190;//#725
	const static APrefID	kNewDocumentWindowPoint				= 1191;//#725
	const static APrefID	kMultiFileFindWindow_FindOptionsExpanded= 1192;//#872
	const static APrefID	kMultiFileFindWindow_WindowWidth	= 1193;//#725
	const static APrefID	kMultiFileFind_FolderPathText		= 1195;//#65
	const static APrefID	kMultiFileFind_FileFilterText		= 1196;//#65
	const static APrefID	kMultiFileFind_BackupFolder			= 1197;//#65
	const static APrefID	kPreviewerMultiplier				= 1198;//#734
	const static APrefID	kProhibitPopup						= 1199;//#725
	const static APrefID	kShowFloatingJumpList				= 1200;//#725
	const static APrefID	kReopenFile_DisplayLeftSideBar		= 1201;//#850
	const static APrefID	kReopenFile_DisplayRightSideBar		= 1202;//#850
	const static APrefID	kReopenFile_DisplaySubText			= 1203;//#850
	const static APrefID	kAddNewModeWindowPosition			= 1204;
	const static APrefID	kProjectFolderSetupPosition			= 1205;
	const static APrefID	kTextMarkerCurrentGroupIndex		= 1206;
	const static APrefID	kFloatingSubWindowArray_CurrentPath	= 1207;//#725
	const static APrefID	kReopenFile_ShownInSubText			= 1208;
	const static APrefID	kMultiFileFindWindow_AdvancedSearchExpanded= 1209;
	const static APrefID	kFTPAccount_UsePrivateKey			= 1210;//#1231
	const static APrefID	kFTPAccount_PrivateKeyFilePath		= 1211;//#1231
	const static APrefID	kFTPAccount_DontCheckServerCert		= 1212;//#1231
	//AWindow_AppPrefに設定項目はあるが、対応するControlIDが無いもの（ListViewのColumn等）
	const static APrefID	kLeftSideBarArray_Type				= 2010;//#725
	const static APrefID	kLeftSideBarArray_Height			= 2011;//#725
	const static APrefID	kLeftSideBarArray_Parameter			= 2012;//#725
	const static APrefID	kRightSideBarArray_Type				= 2013;//#725
	const static APrefID	kRightSideBarArray_Height			= 2014;//#725
	const static APrefID	kRightSideBarArray_Parameter		= 2015;//#725
	const static APrefID	kSubTextColumnWidth					= 2016;//#725
	const static APrefID	kSubTextColumnDisplayed				= 2017;//#725
	const static APrefID	kLeftSideBarArray_Collapsed			= 2018;//#725
	const static APrefID	kRightSideBarArray_Collapsed		= 2019;//#725
	const static APrefID	kRecentlyOpenedFile					= 2020;//#910
	const static APrefID	kRecentlyOpenedFile_Pin				= 2021;//#394
	const static APrefID	kLeftSideBarArray_CurrentPath		= 2022;//#725
	const static APrefID	kRightSideBarArray_CurrentPath		= 2023;//#725
	const static APrefID	kFTPAccount_UseDefaultPort			= 2024;//#193
	const static APrefID	kFTPAccount_DisplayName				= 2025;//#193
	
	//JumpListPosition
	const static ANumber	kJumpListPosition_Left		= 0;
	const static ANumber	kJumpListPosition_Right		= 1;
	
	//FTPAccount_ProxyMode
	const static ANumber	kProxyMode_NoProxy			= 0;
	const static ANumber	kProxyMode_FTPProxy1		= 1;
	const static ANumber	kProxyMode_FTPProxy2		= 2;
	
	//#844
	//バージョン3.0でobsoleteにした設定
  private:
	//
	const static APrefID	kAppDefaultFont						= 295;//#375
	//
	const static APrefID	kFileListFontName					= 238;//win
	const static APrefID	kJumpListFontName					= 239;//win
	const static APrefID	kIndexWindowFontName				= 240;//win
	const static APrefID	kJumpListFontSize					= 128;
	const static APrefID	kFileListFontSize					= 131;
	const static APrefID	kIndexWindowFontSize				= 134;
	const static APrefID	kJumpListAlphaPercent				= 152;
	const static APrefID	kWindowListAlphaPercent				= 132;
	const static APrefID	kIdInfoWindowAlphaPercent			= 242;//RC2
	//
	const static APrefID	kChangeScrollSpeedForMouseDrag		= 165;
	//
	const static APrefID	kCreateNewWindowAutomatic			= 209;
	//
	const static APrefID	kTextWindowTabWidth					= 1088;//R0177
	const static APrefID	kTextWindowTabHeight				= 1089;//R0177
	const static APrefID	kTextWindowTabFontSize				= 1090;//R0177
	const static APrefID	kMultipleRowTab						= 210;
	const static APrefID	kShowTabCloseButton					= 307;//#164
	//
	const static APrefID	kDisplayLeftsideConcatViewButton 	= 216;
	//
	const static APrefID	kInfoPaneArray_Type					= 1115;//#291
	const static APrefID	kInfoPaneArray_Height				= 1116;//#291
	const static APrefID	kSubPaneArray_Type					= 1117;//#291
	const static APrefID	kSubPaneArray_Height				= 1118;//#291
	const static APrefID	kOverlayWindowsAlpha				= 291;//#291
	//
	const static APrefID	kDisplaySearchBox					= 253;//#137
	//
	const static APrefID	kCommandMIsMultiFileFind			= 113;
	//
	const static APrefID	kDontAddRecentlyUsedFile			= 222;//R0193
	const static APrefID	kDontRememberRecentFindString		= 296;//#352
	//
	const static APrefID	kOpenWithNoWrapWhenBigFile			= 224;//R0208
	const static APrefID	kOpenAlertWhenBigFile				= 225;//R0208
	//
	const static APrefID	kInlineInputColorLine				= 140;
	//
	const static APrefID	kReopenFileLimit					= 311;//#513
	const static APrefID	kReopenFileLimitNumber				= 312;//#513
	//
	const static APrefID	kFilePathDisplayType				= 223;//B0389
	//
	const static APrefID	kDiffColor_JumpList					= 235;//R0006
	//
	const static APrefID	kDisplaySCMStateInFileList			= 227;//R0006
	const static APrefID	kDisplaySCMStateInFileList_Color	= 228;//R0006
	const static APrefID	kDisplaySCMStateInFileList_NotEntriedColor	= 236;//R0006
	const static APrefID	kDisplaySCMStateInFileList_OthersColor	= 237;//R0006
	//
	const static APrefID	kLegacyEncodingForCopyPaste			= 251;//#189
	//
	const static APrefID	kIndexWinNormalColor				= 135;
	const static APrefID	kIndexWinBackColor					= 136;
	const static APrefID	kIndexWinCommentColor				= 137;
	const static APrefID	kIndexWinURLColor					= 138;
	const static APrefID	kIndexWinGroupColor					= 139;
	//
	const static APrefID	kAlphaWindowMode					= 104;
	const static APrefID	kAlphaWindowPercent1				= 105;
	const static APrefID	kAlphaWindowPercent2				= 106;
	//
	const static APrefID	kDisplayKeyToolList					= 247;//R0073
	//
	const static APrefID	kSaveAsDefaultFolderIsOriginalFileFolder	= 217;
	//
	const static APrefID	kLaunchApp_CheckExtension			= 2001;
	const static APrefID	kLaunchApp_Extension				= 2002;
	const static APrefID	kLaunchApp_CheckCreator				= 2003;
	const static APrefID	kLaunchApp_Creator					= 2004;
	const static APrefID	kLaunchApp_CheckType				= 2005;
	const static APrefID	kLaunchApp_Type						= 2006;
	const static APrefID	kLaunchApp_ShiftKey					= 2007;
	const static APrefID	kLaunchApp_App						= 2008;
	//#898
	const static APrefID	kRecentlyOpenedFile_DocPrefPath		= 1094;//win 080710
	//#899
	const static APrefID	kSubPaneMode						= 1111;//#212
	//#907
	const static APrefID	kFindHighlightColor					= 125;
	const static APrefID	kFindHighlightColor2				= 245;//R0244
	const static APrefID	kEnableSelectionColorForFind		= 258;//#262
	const static APrefID	kSelectionColorForFind				= 259;//#262
	//
	const static APrefID	kFTPAccount_Backup					= 1058;
	const static APrefID	kFTPAccount_BackupFolderPath		= 1059;
	const static APrefID	kFTPAccount_BackupWithDateTime		= 1091;//R0207
	//
	const static APrefID	kWindowShortcut						= 112;
	//
	const static APrefID	kTextEncodingMenu					= 2009;
	//#844 SJISロスレスフォールバックはdrop
	const static APrefID	kUseSJISLosslessFallback			= 313;//#473
	
  public:
	//最近開いたファイル
	void	AddNewRecentlyOpenedFileComment();
	void	DeleteRecentlyOpenedFileComment( AIndex inNumber );
	//B0389 ABool	GetLastOpenedFile( AFileAcc& outFileAcc );
	
	//禁則
  public:
	ABool	IsKinsokuLetter( const AText& inText );
	void	UpdateKinsokuLettersHash();
	
	//ファイルパス
  public:
	void	GetFilePathForDisplay( const AFileAcc& inFile, AText& outPath ) const;//B0389
	
	//バイナリファイル判定
  public:
	ABool	IsBinaryFile( const AFileAcc& inFile ) const;
	void	UpdateBinaryFileRegExp();
  private:
	mutable AThreadMutex	mBinaryFileRegExpMutex;
	mutable ARegExp	mBinaryFileRegExp;
	
  public:
	ABool	LaunchAppWithFile( const AFileAcc& inFile, const AModifierKeys inModifierKeys );
	
  private:
	ANumber	ConstrainWidthHeight( ANumber inNumber );
	
	//#1034
#if 0
#if IMPLEMENTATION_FOR_MACOSX
	//旧Macリソースタイプの設定読み込み、変換
	void	LoadMacLegacy();
	void	LoadMacLegacy_LegacyPref( short inResRefNum );
	void	LoadMacLegacy_MultiFileFind_RecentlyUsedFolder( short inResRefNum );
	//#259 void	LoadMacLegacy_RecentlyOpenedFile( short inResRefNum );
	void	LoadMacLegacy_FTPFolder( short inResRefNum );
	void	LoadMacLegacy_LaunchApp( short inResRefNum );
	void	LoadMacLegacy_Kinsoku();
	void	LoadMacLegacy_FTPServer( short inResRefNum );
	void	LoadMacLegacy_TextEncoding();
	void	LoadMacLegacy_ConvertEndianToHost( SMacLegacyAppPref& ioPref );
	void	LoadMacLegacy_ConvertEndianToBig( SMacLegacyAppPref& ioPref );
	void	LoadMacLegacy_FolderList( short inResRefNum );
	void	LoadMacLegacy_FindMemory( short inResRefNum );
	
	void	WriteMacLegacy();
	void	WriteMacLegacy_FTPFolder( short inResRefNum );
#endif
#endif
	
	//データ
	//#844 ATextArray	mTextEncodingMenuFixedTextArray;
	AHashTextArray	mKinsokuLettersHash;
	AThreadMutex	mKinsokuLettersHashMutex;//#890
	//B0413 AHashTextArray	mRecentlyOpenedFileDisplayText;//R0153
	
	AObjectArray<ANumberArray>	mKeyBindDefault;
	AObjectArray<ATextArray>	mKeyBindDefaultText;//win
	AObjectArray<ABoolArray>	mKeyBindChangeable;
  public:
	ABool	GetData_DefaultKeyBindAction( const AKeyBindKey& inKey, const AModifierKeys inModiferKeys, AKeyBindAction& outAction, AText& outText );
	void	Create_DefaultKeyBindTable();
	
};
