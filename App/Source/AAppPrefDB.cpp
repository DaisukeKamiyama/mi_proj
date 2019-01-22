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

#include "stdafx.h"

#include "AAppPrefDB.h"
#include "AApp.h"

#pragma mark [AAppPrefDB]�A�v���̐ݒ�f�[�^�Ǘ��I�u�W�F�N�g

#pragma mark ---Constructor/Destructor/Load/Write/Setup

AAppPrefDB::AAppPrefDB() : ADataBase(NULL)
{
	/*#844
	mTextEncodingMenuFixedTextArray.Add("UTF-8");
	mTextEncodingMenuFixedTextArray.Add("UTF-16");
#if IMPLEMENTATION_FOR_MACOSX
	mTextEncodingMenuFixedTextArray.Add("UTF-16LE");
#endif
	mTextEncodingMenuFixedTextArray.Add("-");
	*/
	/*#305 �W�����[�h���̃��W�b�N��ς���̂ŏ������ȒP�ɂ��邽�߂Ƀf�t�H���g�͐ݒ肵�Ȃ����Ƃɂ���
	AText	standardmodename;
	standardmodename.SetLocalizedText("StandardModeName");
	*/
	//�f�[�^�̓o�^�Ə����l�̐ݒ�
	ANumber	num;
	
	//#725
	AText	fixwidthFont;
	AFontWrapper::GetAppDefaultFontName(fixwidthFont);
	//win
	AText	defaultfont;
	//#0 AFontWrapper::GetDefaultFontName(defaultfont);
	AFontWrapper::GetAppDefaultFontName(defaultfont);
	AText	floatingDefaultFont;
	floatingDefaultFont.SetText(defaultfont);
#if IMPLEMENTATION_FOR_WINDOWS
	if( AEnvWrapper::GetOSVersion() < kOSVersion_WindowsVista )
	{
		floatingDefaultFont.SetCstring("Arial");
	}
#endif
	/*#688
#if IMPLEMENTATION_FOR_MACOSX
	//���f�[�^
	CreateData_Number(kIndexWindowFont_compat,					"indexWindowFont",					AFontWrapper::GetDefaultFont(),kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kFindDialogFont_compat,					"FindDialog Font",					AFontWrapper::GetDefaultFont(),kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kJumpListFont_compat,						"JumpList Font",					AFontWrapper::GetDefaultFont(),kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kFileListFont_compat,						"FileList Font",					AFontWrapper::GetDefaultFont(),kNumber_MinNumber,kNumber_MaxNumber);//win
#endif
	*/
	//				�f�[�^ID									�t�@�C���pKey							�����l			(�ŏ�,�ő�)			(�ŏ�,�ő�)
	CreateData_Point(kFTPLogWindowPosition,						"FTPLogWindowPosition",				100,300,		-5000,5000,	-5000,5000);//B0376
	CreateData_Point(kFTPProgressPosition,						"FTPProgressPosition",				100,100,		-5000,5000,	-5000,5000);//B0376
	CreateData_Point(kTraceLogWindowPosition,					"TraceLogWindowPosition",			100,100,		-5000,5000,	-5000,5000);//B0376
	CreateData_Point(kModePrefWindowPosition,					"ModePrefWindowPosition",			180,180,		-5000,5000,	-5000,5000);//B0376
	CreateData_Point(kFTPFolderWindowPosition,					"FTPFolderWindowPosition",			100,200,		-5000,5000,	-5000,5000);//B0376
	CreateData_Point(kFolderListWindowPosition,					"FolderListWindowPosition",			100,250,		-5000,5000,	-5000,5000);//B0376
	CreateData_Point(kMultiFileFindProgressPosition,			"MultiFileFindProgressPosition",	100,50,			-5000,5000,	-5000,5000);//B0376
	CreateData_Point(kFindWindowPosition,						"FindWindowPosition",				120,120,		-5000,5000,	-5000,5000);//B0376
	CreateData_Point(kIndexWindowPosition,						"IndexWindowPosition",				50,500,			-5000,5000,	-5000,5000);//B0376
	CreateData_Point(kDiffWindowPosition,						"DiffWindowPosition",				200,100,		-5000,5000,	-5000,5000);//#379 #623
	CreateData_Point(kEnabledDefinesWindowPosition,				"EnabledDefinesWindowPosition",		210,150,		-5000,5000,	-5000,5000);//#467
	
	//				�f�[�^ID									�t�@�C���pKey							�����l			(�ŏ�,�ő�)			(�ŏ�,�ő�)
	CreateData_Bool(kCMMouseButton,								"CMMouseButton",					true);
	CreateData_Number(kCMMouseButtonTick,						"CMMouseButtonTick",				60,				10,1000);
	CreateData_Color(kIndexWinNormalColor,						"indexWinNormalColor",				kColor_Black);
	CreateData_Color(kIndexWinBackColor,						"indexWinBackColor",				kColor_White);
	CreateData_Color(kIndexWinCommentColor,						"indexWinCommentColor",				kColor_Red);
	CreateData_Color(kIndexWinURLColor,							"indexWinURLColor",					kColor_Blue);
	CreateData_Color(kIndexWinGroupColor,						"indexWinGroupColor",				kColor_Red);
	CreateData_Bool(kPrintColored,								"printColored",						true);
	CreateData_Bool(kBunsetsuSelect,							"bunsetsuSelect",					false);//B0000 �����l�ύX
	CreateData_Bool(kWindowShortcut,							"windowShortcut",					false);
	CreateData_Text(kCmdNModeName,								"cmdNModeName",						GetEmptyText());//#305 standardmodename);
#if IMPLEMENTATION_FOR_MACOSX
	CreateData_Bool(kUseShellScript,							"useShellScript",					true);
#else
	CreateData_Bool(kUseShellScript,							"useShellScript",					false);
#endif
	CreateData_Text(kStdOutModeName,							"stdOutModeName",					GetEmptyText());//#305 standardmodename);
	CreateData_Bool(kLockWhileEdit,								"lockWhileEdit",					true);
	CreateData_Bool(kPrintBlackForWhite,						"printBlackForWhite",				true);
	//win CreateData_Font(kIndexWindowFont,							"indexWindowFont",					AFontWrapper::GetDefaultFont());
	CreateData_Text(kIndexWindowFontName,						"indexWindowFontName",				defaultfont);//win
#if IMPLEMENTATION_FOR_MACOSX
	CreateData_FloatNumber(kIndexWindowFontSize,				"indexWindowFontSize",				9.0,			6.0,256.0);//B0398 �ŏ��l�ύX #0 #212 �f�t�H���g�l�ύX10.0��9.0
#else
	CreateData_FloatNumber(kIndexWindowFontSize,				"indexWindowFontSize",				8.0,			6.0,256.0);//B0398 �ŏ��l�ύX #0 #212 �f�t�H���g�l�ύX10.0��9.0
#endif
	CreateData_Number(kIndexWindowWidth,						"indexWindowWidth",					500,			50,5000);
	CreateData_Number(kIndexWindowHeight,						"indexWindowHeight",				180,			50,5000);
	CreateData_Number(kIndexWindowSeparateLine,					"indexWindowSeparateLine",			40,				1,99);
	CreateData_Bool(kInlineInputColorLine,						"inlineInputColorLine",				false);
	CreateData_Bool(kFindDefaultIgnoreCase,						"findDefaultIgnoreCase",			true);
	CreateData_Bool(kFindDefaultWholeWord,						"findDefaultWholeWord",				false);
	CreateData_Bool(kFindDefaultAimai,							"findDefaultAimai",					false);//#1352
	CreateData_Bool(kFindDefaultRegExp,							"findDefaultRegExp",				false);
	CreateData_Bool(kFindDefaultIgnoreSpaces,					"FindDefaultIgnoreSpaces",			false);//#165
	CreateData_Bool(kFindDefaultLoop,							"findDefaultLoop",					false);
	CreateData_Bool(kFindDefaultAllowReturn,					"findDefaultAllowReturn",			false);
	CreateData_Bool(kFindDefaultRecursive,						"findDefaultRecursive",				true);
	CreateData_Bool(kFindDefaultDisplayPosition,				"findDefaultDisplayPosition",		true);
#if IMPLEMENTATION_FOR_WINDOWS
	CreateData_Text(kFindDefaultFileFilter,						"findDefaultFileFilter",			"*.*");//#437
#else
	CreateData_Text(kFindDefaultFileFilter,						"findDefaultFileFilter",			".+");
#endif
	CreateData_Point(kMultiFileFindWindowPosition,				"multiFileFindDialogPosition",		140,140,		-5000,5000,	-5000,5000);//B0376
#if IMPLEMENTATION_FOR_WINDOWS
	CreateData_Number(kWheelScrollPercent,						"wheelScrollPercent",				200,			20,9999);
	CreateData_Number(kWheelScrollPercentCmd,					"wheelScrollPercentCmd",			600,			20,9999);
#else
	CreateData_Number(kWheelScrollPercent,						"wheelScrollPercent",				100,			20,9999);
	CreateData_Number(kWheelScrollPercentCmd,					"wheelScrollPercentCmd",			300,			20,9999);
#endif
	CreateData_Bool(kAlphaWindowMode,							"alphaWindowMode",					false);
	CreateData_Number(kAlphaWindowPercent1,						"alphaWindowPercent1",				85,				10,100);
	CreateData_Number(kAlphaWindowPercent2,						"alphaWindowPercent2",				50,				0,100);
	CreateData_Bool(kAlphaWindowModeV3,							"alphaWindowModeV3",				false);//#1255
	CreateData_Number(kAlphaWindowPercent1V3,					"alphaWindowPercent1V3",			85,				10,100);//#1255
	CreateData_Number(kAlphaWindowPercent2V3,					"alphaWindowPercent2V3",			50,				0,100);//#1255
	CreateData_Bool(kCreateTabInsteadOfCreateWindow,			"singleWindowMode",					true);//B0000 �����l�ύX
	CreateData_Number(kJumpListOffsetX,							"jumpListOffsetX",					16,				-5000,5000);
	CreateData_Number(kJumpListOffsetY,							"jumpListOffsetY",					32,				-5000,5000);
	CreateData_Number(kJumpListAlphaPercent,					"jumpListAlphaPercent",				90,				10,100);
	CreateData_Number(kJumpListSizeX,							"jumpListSizeX",					250,			50,5000);
	CreateData_Number(kJumpListSizeY,							"jumpListSizeY",					500,			50,5000);
	CreateData_Number(kWindowListOffsetX,						"windowListOffsetX",				100,			-5000,5000);
	CreateData_Number(kWindowListOffsetY,						"windowListOffsetY",				100,			-5000,5000);
	CreateData_Number(kWindowListAlphaPercent,					"windowListAlphaPercent",			90,				10,100);
	CreateData_Number(kWindowListSizeX,							"windowListSizeX",					250,			50,5000);
	CreateData_Number(kWindowListSizeY,							"windowListSizeY",					300,			50,5000);
	CreateData_Bool(kDisplayWindowList,							"displayWindowList",				true);
	num = kJumpListPosition_Right;
	CreateData_Number(kJumpListPosition,						"jumpListPosition",					num,			0,1);
	CreateData_Rect(kSingleWindowBounds,						"singleWindowBounds",				100,50,930,1050,	-5000,-5000,5000,5000);//#190 #1371 900��930
	CreateData_Number(kWindowListMode,							"windowListMode",					static_cast<ANumber>(/*windowListMode_RecentFile*/2),	0,3);
	
	//				�f�[�^ID									�t�@�C���pKey							�����l			(�ŏ�,�ő�)			(�ŏ�,�ő�)
	//win CreateData_Font(kFindDialogFont,							"FindDialog Font",					AFontWrapper::GetDefaultFont());
	CreateData_Text(kFindDialogFontName,						"FindDialog FontName",				defaultfont);//win
	//win CreateData_Font(kJumpListFont,								"JumpList Font",					AFontWrapper::GetDefaultFont());
	CreateData_Text(kJumpListFontName,							"JumpList FontName",				floatingDefaultFont);//win
#if IMPLEMENTATION_FOR_MACOSX
	CreateData_FloatNumber(kJumpListFontSize,					"JumpList FontSize",				9.0,			6.0,256.0);//B0398 �ŏ��l�ύX #0
#else
	CreateData_FloatNumber(kJumpListFontSize,					"JumpList FontSize",				8.0,			6.0,256.0);//B0398 �ŏ��l�ύX #0
#endif
	//win CreateData_Font(kFileListFont,								"FileList Font",					AFontWrapper::GetDefaultFont());
	CreateData_Text(kFileListFontName,							"FileList FontName",				floatingDefaultFont);//win
#if IMPLEMENTATION_FOR_MACOSX
	CreateData_FloatNumber(kFileListFontSize,					"FileList FontSize",				9.0,			6.0,256.0);//B0398 �ŏ��l�ύX #0
#else
	CreateData_FloatNumber(kFileListFontSize,					"FileList FontSize",				8.0,			6.0,256.0);//B0398 �ŏ��l�ύX #0
#endif
	CreateData_Bool(kCountAs2Letters,							"CountAs2Letters",					true);
	CreateData_Bool(kDisplayControlCode,						"DisplayControlCode",				true);
	CreateData_Bool(kDisplayFindHighlight,						"DisplayFindHighlight",				true);
	CreateData_Bool(kDisplayFindHighlight2,						"DisplayFindHighlight2",			true);//R0244
	CreateData_Color(kFindHighlightColor,						"FindHighlightColor",				kColor_DeepYellow);
	CreateData_Color(kFindHighlightColor2,						"FindHighlightColor2",				kColor_Lavender);//R0244 #0 GreenYellow->Lavender
	CreateData_Bool(kInsertLineMarginForAntiAlias,				"InsertLineMarginForAntiAlias",		true);
	CreateData_Bool(kFindMemory_DefaultVisible,					"FindMemory_DefaultVisible",		false);
	CreateData_Bool(kFindMemory_DecresingOrder,					"FindMemory_DecresingOrder",		false);
	CreateData_Number(kFindMemory_NumberWidth,					"FindMemory_NumberWidth",			50,				10,500);
	CreateData_Number(kFindMemory_NameWidth,					"FindMemory_NameWidth",				170,			10,500);
	CreateData_Number(kFindMemory_FindStringWidth,				"FindMemory_FindStringWidth",		100,			10,500);
	CreateData_Number(kFindMemory_ReplaceStringWidth,			"FindMemory_ReplaceStringWidth",	100,			10,500);
	CreateData_Point(kFindMemory_OffsetFromFindWindow,			"FindMemory_OffsetFromFindWindow",	500,	0,		-5000,5000,		-5000,5000);
	CreateData_Number(kFindMemoryWindow_Width,					"FindMemoryWindow_Width",			436,			436,5000);
	CreateData_Number(kFindMemoryWindow_Height,					"FindMemoryWindow_Height",			406,			406,5000);
	CreateData_Number(kFind_WindowWidth,						"Find_WindowWidth",					408,			408,5000);
	CreateData_FloatNumber(kFind_FontSize,						"Find_FontSize",					12.0,			6.0,256.0);//B0398 �ŏ��l�ύX
	CreateData_Bool(kFindDefaultIgnoreBackslashYen,				"findDefaultIgnoreBackslashYen",	true);
	CreateData_Point(kPreferenceWindowPosition,					"preferenceWindowPosition",			60,160,			-5000,5000,	-5000,5000);//B0376
	CreateData_Bool(kDisableHorizontalWheelScroll,				"DisableHorizontalWheelScroll",		false);//#844 �f�t�H���g�l�ύX�i�܂��V�K�f�t�H���g�l�𔽉f�����邽�߂ɐݒ薼�ύX�j #1209
	CreateData_Number(kJumpListParagraphColumnWidth,			"JumpListParagraphColumnWidth",		50,				10,1000);
	CreateData_Bool(kCommandMIsMultiFileFind,					"CommandMIsMultiFileFind",			false);
	CreateData_Bool(kChangeScrollSpeedForMouseDrag,				"ChangeScrollSpeedForMouseDrag",	false);
	
	CreateData_Number(kJumpListParagraphWidth,					"JumpListParagraphWidth",			50,				50,200);
	CreateData_Point(kFileListPosition,							"FileListPosition",					500,300,		-5000,5000,	-5000,5000);//B0376
	
	CreateData_Text(kSameProjectFolderPath,						"sameProjectFolderPath",			"");
	CreateData_Number(kSameProjectFolderModeIndex,				"sameProjectFolderModeIndex",		0,				0,9999);
	CreateData_Text(kSameFolderPath,							"sameFolderPath",					"");
	
	CreateData_Point(kTextCountWindowPosition,					"TextCountWindowPosition",			80,600,			-5000,5000,	-5000,5000);//B0376
	CreateData_Number(kTextCountConditionLetterCount,			"TextCountConditionLetterCount",	80,				5,9999);
	CreateData_Number(kTextCountConditionLineCount,				"TextCountConditionLineCount",		10,				1,9999);
	CreateData_Number(kTextCountCountAs2Letters,				"TextCountCountAs2Letters",			0,				0,2);
	
	CreateData_Bool(kCreateNewWindowAutomatic,					"CreateNewWindowAutomatic",			true);//B0000 �����l�ύX
	CreateData_Bool(kMultipleRowTab,							"MultipleRowTab",					true);
	
	CreateData_Number(kMultiFileFind_ActiveFolderKind,			"kMultiFileFind_ActiveFolderKind",	0,				0,2);
	CreateData_Number(kTabToWindowPositionOffset,				"TabToWindowPositionOffset",		300,			-9999,9999);
	
	CreateData_Text(kTexCompilerPath,							"TexCompilerPath",					"/usr/local/bin/platex");
	CreateData_Text(kTexPreviewerPath,							"TexPreviewerPath",					"");
	
	CreateData_Bool(kDisplayLeftsideConcatViewButton,			"DisplayLeftsideConcatViewButton",	true);//#0
	
	CreateData_Bool(kTextCountCountSpaceTab,					"TextCountCountSpaceTab",			false);
	CreateData_Bool(kTextCountCountReturn,						"TextCountCountReturn",				false);
	
	CreateData_Bool(kSaveAsDefaultFolderIsOriginalFileFolder,	"SaveAsDefaultFolderIsOriginalFileFolder",	false);
	
	//B0313
	CreateData_Bool(kFindDefaultOnlyVisibleFile,				"FindDefaultOnlyVisibleFile",		true);//#234 �����l�ύX
	CreateData_Bool(kFindDefaultOnlyTextFile,					"FindDefaultOnlyTextFile",			false);
	
	//R0177
	CreateData_FloatNumber(kTextWindowTabWidth,					"kTextWindowTabWidth",				120.0,			10,500);//B0000 �f�t�H���g�u��v�֕ύX
	CreateData_FloatNumber(kTextWindowTabHeight,				"TextWindowTabHeight",				20.0,			5,100);
	CreateData_FloatNumber(kTextWindowTabFontSize,				"TextWindowTabFontSize",			10.0,			3,100);
	
	//R0184
	CreateData_Bool(kBigCaret,									"BigCaret",							true);//#234 �����l�ύX
	
	//R0193
	CreateData_Bool(kDontAddRecentlyUsedFile,					"DontAddRecentlyUsedFile",			false);
	
	//B0389
	CreateData_Number(kFilePathDisplayType,						"FilePathDisplayType",				0,				0,2);
	
	//R0208
	CreateData_Bool(kOpenWithNoWrapWhenBigFile,					"OpenWithNoWrapWhenBigFile",		true);
	CreateData_Bool(kOpenAlertWhenBigFile,						"OpenAlertWhenBigFile",				true);
	
	//B0414
	CreateData_Bool(kMultiFileFind_UseFileFilter,				"MultiFileFind_UseFileFilter",		false);
	
	//win 080702
	CreateData_Bool(kFileListDontDisplayInvisibleFile,			"FileListDontDisplayInvisibleFile",	false);
	
	//RC2
	CreateData_Point(kIdInfoWindowPosition,						"IdInfoWindowPosition",				64,64,		-5000,5000,	-5000,5000);
	CreateData_Number(kIdInfoWindowWidth,						"IdInfoWindowWidth",				250,			50,5000);
	CreateData_Number(kIdInfoWindowHeight,						"IdInfoWindowHeight",				300,			50,5000);
	CreateData_Number(kIdInfoWindowAlphaPercent,				"IdInfoWindowAlphaPercent",			90,			10,100);
	CreateData_Bool(kDisplayIdInfoWindow,						"DisplayIdInfoWindow",				false);
	CreateData_Number(kCandidateWindowWidth,					"CandidateWindowWidth",				500,			50,5000);
	CreateData_Number(kCandidateWindowHeight,					"CandidateWindowHeight",			300,			50,5000);
	
	//B0432
	CreateData_Bool(kSelectWordByContextMenu,					"SelectWordByContextMenu",			false);
	
	//R0006
	CreateData_Bool(kUseSCM,									"UseSCM",							true);//#379 false->true
	CreateData_Bool(kDisplaySCMStateInFileList,					"DisplaySCMStateInFileList",		true);
	CreateData_Color(kDisplaySCMStateInFileList_Color,			"DisplaySCMStateInFileList_Color",	kColor_DarkBlue);
	//#379 CreateData_Bool(kCompareWithLatestWhenOpen,					"CompareWithLatestWhenOpen",		true);
	//#379 CreateData_Bool(kCompareWithLatestWhenSave,					"CompareWithLatestWhenSave",		true);
	CreateData_Text(kCVSPath,									"CVSPath",							"/usr/bin/cvs");
	CreateData_Color(kDiffColor_Changed,						"DiffColor_Changed",				kColor_DarkGreen);
	CreateData_Color(kDiffColor_Added,							"DiffColor_Added",					kColor_Blue);
	CreateData_Color(kDiffColor_Deleted,						"DiffColor_Deleted",				kColor_Red);
	CreateData_Color(kDiffColor_JumpList,						"DiffColor_JumpList",				kColor_Red);
	CreateData_Color(kDisplaySCMStateInFileList_NotEntriedColor,"DisplaySCMStateInFileList_NotEntriedColor",kColor_Gray);
	CreateData_Color(kDisplaySCMStateInFileList_OthersColor,	"DisplaySCMStateInFileList_OthersColor",kColor_Pink);
	
	//R0073
	CreateData_Number(kKeyToolListWindowWidth,					"KeyToolListWindowWidth",			200,			50,5000);
	CreateData_Number(kKeyToolListWindowHeight,					"KeyToolListWindowHeight",			300,			50,5000);
	CreateData_Bool(kDisplayKeyToolList,						"DisplayKeyToolList",				true);
	
	//R0274
#if IMPLEMENTATION_FOR_WINDOWS
	CreateData_Text(kSVNPath,									"SVNPath",							"svn");
	CreateData_Text(kDiffPath,									"DiffPath",							"diff");
	CreateData_Text(kGitPath,									"GitPath",							"git");//#589
#else
	CreateData_Text(kSVNPath,									"SVNPath",							"/usr/bin/svn");
	CreateData_Text(kDiffPath,									"DiffPath",							"/usr/bin/diff");
	CreateData_Text(kGitPath,									"GitPath",							"/usr/bin/git");//#589 #1092
#endif
	CreateData_Text(kDiffOption,								"DiffOption",						"-b");
	
	//#138
	CreateData_Point(kFusenListWindowPosition,					"FusenListWindowPosition",			50,500,			-5000,5000,	-5000,5000);//#138
	CreateData_Number(kFusenListWindowWidth,					"FusenListWindowWidth",				400,			50,5000);
	CreateData_Number(kFusenListWindowHeight,					"FusenListWindowHeight",			150,			50,5000);
	CreateData_Bool(kDisplayFusenList,							"DisplayFusenList",					false);
	
	//#189
	CreateData_Number(kLegacyEncodingForCopyPaste,				"LegacyEncodingForCopyPaste",		1,				0,2);
	
	//#92
	CreateData_Bool(kDisplayFindResultInTextWindow,				"DisplayFindResultInTextWindow",	true);
	CreateData_Number(kFindResultViewHeight,					"FindResultViewHeight",				150,			10,5000);
	
	//#137
	CreateData_Bool(kDisplaySearchBox,							"DisplaySearchBox",					true);
	CreateData_Bool(kFindShortcutIsFocusSearchBox,				"FindShortcutIsFocusSearchBox",		true);
	
	//#208
	CreateData_Bool(kCheckEncodingError_KishuIzon,				"CheckEncodingError_KishuIzon",		true);
	CreateData_Bool(kCheckEncodingError_HankakuKana,			"CheckEncodingError_HankakuKana",	true);
	
	//#204
	CreateData_Bool(kCandidateWindowKeyBind,					"CandidateWindowKeyBind",			true);
	
	//#262
	CreateData_Bool(kEnableSelectionColorForFind,				"EnableSelectionColorForFind",		true);
	CreateData_Color(kSelectionColorForFind,					"SelectionColorForFind",			kColor_Gold);//Yellow->Orange #0
	
	//#269
	CreateData_Bool(kSelectTextWindowAfterFind,					"SelectTextWindowAfterFind",		true);
	
	//#212
	CreateData_Bool(kSubPaneDisplayed,							"SubPaneDisplayed",					false);
	CreateData_Number(kSubPaneWidth,							"SubPaneWidth",						350,			100,5000);
	CreateData_Number(kSubPaneMode,								"SubPaneMode",						0,				0,2);
	
	//#291
	CreateData_Bool(kInfoPaneDisplayed,							"InfoPaneDisplayed",				true);
	CreateData_Number(kInfoPaneWidth,							"InfoPaneWidth",					200,			100,5000);
	
	//#291
	CreateData_Number(kOverlayWindowsAlpha,						"OverlayWindowsAlpha",				70,				10,100);
	
	//#334
	CreateData_Bool(kPrintEachParagraphAlways,					"PrintEachParagraphAlways",			false);
	
	//#335
	/*#379
#if IMPLEMENTATION_FOR_WINDOWS
	CreateData_Bool(kUseExternalDiffForSubversion,				"UseExternalDiffForSubversion",		false);
#else
	CreateData_Bool(kUseExternalDiffForSubversion,				"UseExternalDiffForSubversion",		true);
#endif
	*/
	
	//amazon
	CreateData_Bool(kDisplayAmazonButton,						"DisplayAmazonButton",				true);
	
	//#375
	CreateData_Text(kAppDefaultFont,							"AppDefaultFont",					defaultfont);
	
	//#352
	CreateData_Bool(kDontRememberRecentFindString,				"DontRememberRecentFindString",		false);
	
	//#437
	CreateData_Number(kMultiFileFindFileFilterKind,				"MultiFileFindFileFilterKind",		1,				0,1);//wildcard
	
	//#390
	CreateData_Point(kKeyRecordWindowPosition,					"KeyRecordWindowPosition",			50,50,			-5000,5000,	-5000,5000);//#138
	CreateData_Number(kKeyRecordWindowWidth,					"KeyRecordWindowWidth",				230,			50,5000);
	CreateData_Number(kKeyRecordWindowHeight,					"KeyRecordWindowHeight",			150,			50,5000);
	CreateData_TextArray(kKeyRecordRecentlyPlay,				"KeyRecordRecentlyPlay",			"");
	
	//win
	CreateData_Bool(kSingleWindowBounds_Zoomed,					"SingleWindowBounds_Zoomed",		false);
	CreateData_Rect(kSingleWindowBounds_UnzoomedBounds,			"SingleWindowBounds_UnzoomedBounds",100,50,1100,1050,	-5000,-5000,5000,5000);//#190
	
	//#436
	CreateData_Text(kDevTool_DmgMaker_Version,					"DevTool_DmgMaker_Version",			GetEmptyText());
	CreateData_Text(kDevTool_DmgMaker_Folder,					"DevTool_DmgMaker_Folder",			GetEmptyText());
	CreateData_Text(kDevTool_DmgMaker_Master,					"DevTool_DmgMaker_Master",			GetEmptyText());
	
	//#404
	CreateData_Bool(kFullScreenMode,							"FullScreenMode",					false);
	
	//#166
	CreateData_Bool(kFuzzyFind_NFKD,							"FuzzyFind_NFKD",					true);
	CreateData_Bool(kFuzzyFind_KanaType,						"FuzzyFind_KanaType",				true);
	CreateData_Bool(kFuzzyFind_Onbiki,							"FuzzyFind_Onbiki",					true);
	CreateData_Bool(kFuzzyFind_KanaYuragi,						"FuzzyFind_KanaYuragi",				true);
	
	//#455
	CreateData_Number(kCommitWindowWidth,						"CommitWindowWidth",				940,			50,5000);
	CreateData_Number(kCommitWindowHeight,						"CommitWindowHeight",				350,			50,5000);
	CreateData_Text(kCommitAdditionalArgs,						"CommitAdditionalArgs",				GetEmptyText());
	CreateData_TextArray(kCommitRecentMessage,					"CommitRecentMessage",				"");
	CreateData_Point(kSCMCommitWindowPosition,					"SCMCommitWindowPosition",			50,50,			-5000,5000,	-5000,5000);
	
	//win
	CreateData_Number(kLaunguage,								"Launguage",						0,				0,9999);
	SetData_Number(kLaunguage,GetApp().NVI_GetLanguage());//OS�̌����ݒ�i�t�@�C�����ɐݒ肪�Ȃ���΂��̐ݒ肪�K�p�����j
	
	//#454
	CreateData_TextArray(kJumpListHistory,						"JumpListHistory",					"");
	CreateData_TextArray(kJumpListHistory_Path,					"JumpListHistory_Path",				"");
	
	//#450
	//CreateData_Bool(kLineFolding,								"LineFolding",						false);
	
	//#379
	CreateData_Text(kDiffSourceFolder_Source,					"DiffSourceFolder_Source",			GetEmptyText());
	CreateData_Text(kDiffSourceFolder_Working,					"DiffSourceFolder_Working",			GetEmptyText());
	CreateData_Bool(kDiffSourceFolder_Enable,					"DiffSourceFolder_Enable",			true);
	CreateData_Text(kDiffSourceFile,							"DiffSourceFile",					GetEmptyText());
	CreateData_Text(kDiffWorkingFile,							"DiffWorkingFile",					GetEmptyText());
	
	//#164
	CreateData_Bool(kShowTabCloseButton,						"ShowTabCloseButton",				true);
	//#510
	CreateData_Bool(kCloseAllTabsWhenCloseWindow,				"CloseAllTabsWhenCloseWindow",		false);//#844 �f�t�H���g�l�ύXtrue��false
	
	//#379
	CreateData_Bool(kShowDiffTargetWithWordWrap,				"ShowDiffTargetWithWordWrap",		false);
	
	//#473
	CreateData_Bool(kUseSJISLosslessFallback,					"UseSJISLosslessFallback",			true);
	
	//#545
	//#653 CreateData_Bool(kDontShowEditByOhterAppDialog,				"DontShowEditByOhterAppDialog",		false);
	
	//#567
	CreateData_Point(kLuaConsoleWindowPosition,					"LuaConsoleWindowPosition",			50,50,			-5000,5000,	-5000,5000);//#138
	CreateData_Number(kLuaConsoleWindowWidth,					"LuaConsoleWindowWidth",			600,			50,5000);
	CreateData_Number(kLuaConsoleWindowHeight,					"LuaConsoleWindowHeight",			300,			50,5000);
	
	//#634
	CreateData_Bool(kInfoPaneSideBarMode,						"InfoPaneSideBarMode",				false);
#if IMPLEMENTATION_FOR_MACOSX
	AColor	sideBarColor = {0xf900,0xf900,0xf600};
#elif IMPLEMENTATION_FOR_WINDOWS
	AColor	sideBarColor = RGB(0xf9,0xf9,0xf6);
#endif
	CreateData_Color(kInfoPaneSideBarModeColor,					"InfoPaneSideBarModeColor",			sideBarColor);
	
	//#668
	CreateData_Bool(kSaveSubPaneModeIntoDocument,				"SaveSubPaneModeIntoDocument",		false);
	
	//#679
	CreateData_Bool(kDisplayKeyRecordWindow,					"DisplayKeyRecordWindow",			false);
	
	//#725
	CreateData_Text(kSubWindowsFontName,						"SubWindowsFontName",				defaultfont);
	CreateData_FloatNumber(kSubWindowsFontSize,					"SubWindowsFontSize",				10.0,			6.0,256.0);
	
	//#725
	//���T�C�h�o�[
	CreateData_Bool(kLeftSideBarDisplayed,						"LeftSideBarDisplayed",				false);
	CreateData_Number(kLeftSideBarWidth,						"LeftSideBarWidth",					300,			100,5000);
	
	//#291
	//�E�T�C�h�o�[
	CreateData_Bool(kRightSideBarDisplayed,						"RightSideBarDisplayed",			true);
	CreateData_Number(kRightSideBarWidth,						"RightSideBarWidth",				260,			100,5000);//#1371 230��260
	
	//#725
	//�T�u�e�L�X�g
	CreateData_Number(kSubTextColumnWidth,						"SubTextColumnWidth",				500,			30,9999);
	CreateData_Bool(kSubTextColumnDisplayed,					"SubTextColumnDisplayed",			false);
	
	//#725
	//�T�u�E�C���h�E���ߓx
	CreateData_Number(kSubWindowsAlpha_Floating,				"SubWindowsAlpha_Floating",			100,			10,100);
	CreateData_Number(kSubWindowsAlpha_Popup,					"SubWindowsAlpha_Popup",			85,				10,100);
	
	//#861
	//�����L��
	CreateData_Bool(kDontRememberAnyHistory,					"DontRememberAnyHistory",			false);
	
	//#823
	//���ݒP��n�C���C�g
	CreateData_Bool(kDisplayCurrentWordHighlight,				"DisplayCurrentWordHighlight",		true);
	
	//#844
	//�A���`�G�C���A�X
	CreateData_Bool(kEnableAntiAlias,							"EnableAntiAlias",					true);
	//�f�t�H���g�E�C���h�E�T�C�Y
	CreateData_Number(kDefaultWindowWidth,						"DefaultWindowWidth",				800,			50,5000);
	CreateData_Number(kDefaultWindowHeight,						"DefaultWindowHeight",				800,			50,5000);
	//����I�v�V����
	CreateData_Text(kPrintOption_PrintFontName,					"PrintForm_PrintFontNameUni",		"default");
	CreateData_FloatNumber(kPrintOption_PrintFontSize,			"PrintForm_PrintFontSize",			12.0,			3.0,256.0);
	CreateData_Text(kPrintOption_PageNumberFontName,			"PrintForm_PageFontNameUni",		"default");
	CreateData_FloatNumber(kPrintOption_PageNumberFontSize,		"PrintForm_PageFontSize",			12.0,			3.0,256.0);
	CreateData_Text(kPrintOption_FileNameFontName,				"PrintForm_FileFontNameUni",		"default");
	CreateData_FloatNumber(kPrintOption_FileNameFontSize,		"PrintForm_FileFontSize",			12.0,			3.0,256.0);
	CreateData_Text(kPrintOption_LineNumberFontName,			"PrintForm_LineFontNameUni",		"default");
	CreateData_FloatNumber(kPrintOption_LineNumberFontSize,		"PrintForm_LineNumberFontSize",		12.0,			3.0,256.0);
	CreateData_Number(kPrintOption_LineMargin,					"PrintForm_LineMargin",				0,				0,64);
	CreateData_Bool(kPrintOption_UsePrintFont,					"PrintForm_UsePrintFont",			false);
	CreateData_Bool(kPrintOption_PrintPageNumber,				"PrintForm_PrintPageNumber",		true);
	CreateData_Bool(kPrintOption_PrintFileName,					"PrintForm_PrintFileName",			true);
	CreateData_Bool(kPrintOption_PrintLineNumber,				"PrintForm_PrintLineNumber",		true);
	CreateData_Bool(kPrintOption_PrintSeparateLine,				"PrintForm_PrintSeparateLine",		true);
	CreateData_Bool(kPrintOption_ForceWordWrap,					"PrintForm_ForceWordWrap",			true);
	//����]��
	CreateData_FloatNumber(kPrintMargin_Left,					"PrintMargin_Left",					10.0,				0,99999);
	CreateData_FloatNumber(kPrintMargin_Right,					"PrintMargin_Right",				10.0,				0,99999);
	CreateData_FloatNumber(kPrintMargin_Top,					"PrintMargin_Top",					10.0,				0,99999);
	CreateData_FloatNumber(kPrintMargin_Bottom,					"PrintMargin_Bottom",				10.0,				0,99999);
	
	//#717
	//�⊮��◚��
	CreateData_TextArray(kRecentCompletionWordArray,			"RecentCompletionWordArray",		"");
	
	//#872
	//�����E�C���h�E���
	CreateData_Bool(kFindWindow_FindOptionsExpanded,			"FindWindow_FindOptionsExpanded",				true);
	CreateData_Bool(kFindWindow_FindMemoryExpanded,				"FindWindow_FindMemoryExpanded",				false);
	CreateData_Bool(kFindWindow_ReplaceExpanded,				"FindWindow_ReplaceExpanded",					true);
	CreateData_Bool(kFindWindow_AdvancedSearchExpanded,			"FindWindow_AdvancedSearchExpanded",			false);
	CreateData_Bool(kMultiFileFindWindow_FindOptionsExpanded,	"MultiFileFindWindow_FindOptionsExpanded",		true);
	CreateData_Bool(kMultiFileFindWindow_AdvancedSearchExpanded,"MultiFileFindWindow_AdvancedSearchExpanded",	false);
	
	//#844
	//���ݍs�ƃL�����b�g
	CreateData_Bool(kDrawVerticalLineAsCaret,					"DrawVerticalLineAsCaret",			false);
	CreateData_Bool(kDrawLineForCurrentLine,					"DrawLineForCurrentLine",			true);
	CreateData_Bool(kPaintLineForCurrentLine,					"PaintLineForCurrentLine",			false);
	CreateData_Color(kCurrentLineColor,							"CurrentLineColor",					kColor_Gray50Percent);
	
	//#717
	//���|�b�v�A�b�v�ʒu
	CreateData_Bool(kPopupCandidateBelowInputText,				"PopupCandidateBelowInputText",		false);
	CreateData_Bool(kPopupCandidateNearInputText,				"PopupCandidateNearInputText",		true);
	
	//#863
	//decomposition�����\��
	CreateData_Bool(kDisplayEachCanonicalDecompChar,			"DisplayEachCanonicalDecompChar",	false);
	//CreateData_Bool(kFindDistinguishCanonicalCompDecomp,		"FindDistinguishCanonicalCompDecomp",false);
	
	//#892
	//�t�@�C�����X�g��collapse���
	CreateData_TextArray(kFileListCollapseFolder,				"FileListCollapseFolder",			"");
	
	//#725
	//�����E�C���h�E��
	CreateData_Number(kFindWindow_WindowWidth,					"FindWindow_WindowWidth",			300,			100,5000);
	CreateData_Number(kMultiFileFindWindow_WindowWidth,			"MultiFileFindWindow_WindowWidth",	300,			100,5000);
	
	//#725
	//�V�K�E�C���h�E�ʒu
	CreateData_Point(kNewDocumentWindowPoint,					"NewDocumentWindowPoint",			100,50,			-5000,5000,	-5000,5000);
	
	//#872
	//�}���`�t�@�C�������p�����[�^
	CreateData_Text(kMultiFileFind_FolderPathText,				"MultiFileFind_FolderPathText",		"");
	CreateData_Text(kMultiFileFind_FileFilterText,				"MultiFileFind_FileFilterText",		"");
	CreateData_Text(kMultiFileFind_BackupFolder,				"MultiFileFind_BackupFolder",		"");
	
	//#734
	//�v���r���[���\���{��
	CreateData_Number(kPreviewerMultiplier,						"PreviewerMultiplier",				50,				10,300);
	
	//#725
	//�|�b�v�A�b�v�֎~���
	CreateData_Bool(kProhibitPopup,								"ProhibitPopup",					false);
	
	//#725
	//�W�����v���X�g�\���L��
	CreateData_Bool(kShowFloatingJumpList,						"ShowFloatingJumpList",				false);
	
	//�L�[���[�h���|�b�v�A�b�v
	CreateData_Bool(kPopupIdInfo,								"PopupIdInfo",						true);
	CreateData_Bool(kHideIdInfoPopupByIdle,						"HideIdInfoPopupByIdle",			true);
	
	//#844
	//�֑��Ԃ牺��
	CreateData_Bool(kKinsokuBuraasge,							"KinsokuBuraasge",					true);
	
	//#81
	//�����o�b�N�A�b�v
	CreateData_Bool(kEnableAutoBackup,							"EnableAutoBackup",					false);//#1210 true��false �����o�b�N�A�b�v�f�t�H���gfalse
	CreateData_Text(kAutoBackupRootFolder,						"AutoBackupRootFolder",				"");
	
	//#723
	//��e�L�X�g�t�@�C��
	CreateData_Text(kBinaryFileDefinition,						"BinaryFileDefinition",				"*.o;*.pdf;*.icns;*.xib;*.png;*.ico;*.jpg;*.scpt;*.exe;*.nib;*.gif;*.pbxtree");
	
	//�X�e�[�^�X�o�[�\��
	CreateData_Bool(kDisplayStatusBar,							"DisplayStatusBar",					true);//3.0.0b9 false��true
	
	//#621
	//�ҏW�����]��
	CreateData_Bool(kBottomScrollAt25Percent,					"BottomScrollAt25Percent",			false);
	
	//�V�K���[�h�E�C���h�E�ʒu
	CreateData_Point(kAddNewModeWindowPosition,					"AddNewModeWindowPosition",			80,80,		-5000,5000,	-5000,5000);//B0376
	//�v���W�F�N�g�t�H���_�ݒ�E�C���h�E�ʒu
	CreateData_Point(kProjectFolderSetupPosition,				"ProjectFolderSetupPosition",		90,90,		-5000,5000,	-5000,5000);//B0376
	
	//���݂̃e�L�X�g�}�[�J�[�O���[�vindex
	CreateData_Number(kTextMarkerCurrentGroupIndex,				"TextMarkerCurrentGroupIndex",		0,			0,999);
	
	//#446
	CreateData_Bool(kAbbrevInputByArrowKeyAfterPopup,			"AbbrevInputByArrowKeyAfterPopup",	false);
	
	//#853
	CreateData_Text(kIdInfoAtNoteText,							"IdInfoAtNoteText",					"@caution\r@warning\r@note\r");
	
	//#764
	CreateData_Bool(kUseGuessTextEncoding,						"UseGuessTextEncoding",				true);
	
	//#830
	CreateData_Bool(kUseFallbackForTextEncodingConversion,		"UseFallbackForTextEncodingConversion",true);
	CreateData_Bool(kShowDialogWhenTextEncodingFallback,		"ShowDialogWhenTextEncodingFallback",true);
	
	//#222
	CreateData_Bool(kSelectParagraphByTripleClick,				"SelectParagraphByTripleClick",		false);
	CreateData_Bool(kSelectLineByClickLineNumber,				"SelectLineByClickLineNumber",		false);
	
	//���C�A�E�g #457
	CreateData_Bool(kShowHScrollBar,							"ShowHScrollBar",					true);//3.0.0b9 false��true
	CreateData_Bool(kShowVScrollBar,							"ShowVScrollBar",					true);
	CreateData_Bool(kShowMacroBar,								"ShowMacroBar",						true);
	
	/*#1160
	//�⊮���L�[�i�f�t�H���g�́Ashift+space�Ashift+tab�j
	CreateData_Number(kAbbrevNextKey,							"AbbrevNextKey",					1,				0,kNumber_MaxNumber);
	CreateData_Number(kAbbrevPrevKey,							"AbbrevPrevKey",					2,				0,kNumber_MaxNumber);
	*/
	
	//TeX
	CreateData_Text(kTexTypesetPath,							"TexTypesetPath",					"/usr/texbin/platex -interaction=nonstopmode");
	CreateData_Text(kTexDviToPdfPath,							"TexDviToPdfPath",					"/usr/texbin/dvipdfmx -V 4");
	
	//skin
	CreateData_Text(kSkinName,									"SkinName",							"");
	
	//�h�b�g�P�ʃX�N���[�� #891
	CreateData_Bool(kUseWheelScrollFloatValue,					"UseWheelScrollFloatValue",			false);
	
	//�L�����b�g���[�J���ʒu�Œ� #1031
	CreateData_Bool(kFixCaretLocalPoint,						"FixCaretLocalPoint",				false);
	
	//�R�s�[�^�ۑ�����CanonicalComp�ϊ� #1044
	CreateData_Bool(kAutoConvertToCanonicalComp,				"AutoConvertToCanonicalComp",		false);
	
	//Sparkle�����X�V���x�� #1102
	CreateData_Number(kSparkleUpdateLevel,						"SparkleUpdateLevel",				0,				0,3);
	
	//�E�C���h�E�^�C�g���t���p�X�\�� #1182
	CreateData_Bool(kDisplayFullPathTitleBar,					"DisplayFullPathTitleBar",			true);

	//�e�L�X�g�̏�ł����Ă�I�r�[���J�[�\���ɂ��Ȃ��ݒ� #1208
	CreateData_Bool(kInhibitIbeamCursor,						"InhibitIbeamCursor",				false);
	
	//���E�]�� #1186
	CreateData_Bool(kDisplayLeftRightMargin,					"DisplayLeftRightMargin",			false);
	CreateData_Number(kTextWidthForLeftRightMargin,				"TextWidthForLeftRightMargin",		800,				200,50000);
	
	//�^�u�ɐe�t�H���_���\�� #1334
	CreateData_Bool(kTabShowParentFolder,						"TabShowParentFolder",				false);
	//�^�u�� #1349
	CreateData_Number(kTabWidth,								"kTabWidth",						210,				80,300);//#1371 160��210
	
	//�������ʃ|�b�v�A�b�v #1322
	CreateData_Bool(kShowFindResultPopup,						"ShowFindResultPopup",				true);
	
	//�R�[���O���t
	CreateTableStart();
	CreateData_TextArray(kCallGrafHistory_FilePath,				"CallGrafHistory_FilePath",			"");
	CreateData_TextArray(kCallGrafHistory_ClassName,			"CallGrafHistory_ClassName",		"");
	CreateData_TextArray(kCallGrafHistory_FunctionName,			"CallGrafHistory_FunctionName",		"");
	CreateData_NumberArray(kCallGrafHistory_StartIndex,			"CallGrafHistory_StartIndex",		0,				0,kNumber_MaxNumber);
	CreateData_NumberArray(kCallGrafHistory_EndIndex,			"CallGrafHistory_EndIndex",			0,				0,kNumber_MaxNumber);
	CreateData_TextArray(kCallGrafHistory_ModeName,				"CallGrafHistory_ModeName",			"");
	CreateData_BoolArray(kCallGrafHistory_EdittedFlag,			"CallGrafHistory_EdittedFlag",		false);
	CreateTableEnd();
	
	//#750
	//�e�L�X�g�}�[�J�[
	CreateTableStart();
	CreateData_TextArray(kTextMarkerArray_Text,					"TextMarkerArray_Text",				"");
	CreateData_TextArray(kTextMarkerArray_Title,				"TextMarkerArray_Title",			"");
	CreateTableEnd();
	
	//#725
	//�e�T�u�E�C���h�E�^�C�v�E�ʒu�E�T�C�Y�E�p�����[�^
	CreateTableStart();
	CreateData_NumberArray(kFloatingSubWindowArray_Type,		"FloatingSubWindowArray_Type",			0,				0,9999);
	CreateData_NumberArray(kFloatingSubWindowArray_Parameter,	"FloatingSubWindowArray_Parameter",		0,				0,9999);
	CreateData_NumberArray(kFloatingSubWindowArray_Width,		"FloatingSubWindowArray_Width",			200,			30,5000);
	CreateData_NumberArray(kFloatingSubWindowArray_Height,		"FloatingSubWindowArray_Height",		100,			30,5000);
	CreateData_NumberArray(kFloatingSubWindowArray_X,			"FloatingSubWindowArray_X",				100,			-5000,5000);
	CreateData_NumberArray(kFloatingSubWindowArray_Y,			"FloatingSubWindowArray_Y",				100,			-5000,5000);
	CreateData_TextArray(kFloatingSubWindowArray_CurrentPath,	"FloatingSubWindowArray_CurrentPath",	"");
	CreateTableEnd();
	
	//#513
	CreateData_Bool(kReopenFile,								"ReopenFile",						true);
	CreateData_Bool(kReopenFileLimit,							"ReopenFileLimit",					true);
	CreateData_Number(kReopenFileLimitNumber,					"ReopenFileLimitNumber",			32,				1,9999);
	CreateTableStart();
	CreateData_TextArray(kReopenFile_Path,						"ReopenFile_Path",					"");
	CreateData_BoolArray(kReopenFile_Selected,					"ReopenFile_Selected",				false);
	CreateData_BoolArray(kReopenFile_IsMainWindow,				"ReopenFile_IsMainWindow",			false);
	CreateData_NumberArray(kReopenFile_WindowIndex,				"ReopenFile_WindowIndex",			0,				0,999);//#850
	CreateData_NumberArray(kReopenFile_TabZOrder,				"ReopenFile_TabZOrder",				0,				0,999);//#850
	CreateData_NumberArray(kReopenFile_TabDisplayIndex,			"ReopenFile_TabDisplayIndex",		0,				0,999);//#850
	CreateData_BoolArray(kReopenFile_IsRemoteFile,				"ReopenFile_IsRemoteFile",			false);
	CreateData_NumberArray(kReopenFile_WindowWidth,				"ReopenFile_WindowWidth",			600,			30,5000);
	CreateData_NumberArray(kReopenFile_WindowHeight,			"ReopenFile_WindowHeight",			400,			30,5000);
	CreateData_NumberArray(kReopenFile_WindowX,					"ReopenFile_WindowX",				50,				-5000,5000);
	CreateData_NumberArray(kReopenFile_WindowY,					"ReopenFile_WindowY",				50,				-5000,5000);
	CreateData_BoolArray(kReopenFile_DisplayLeftSideBar,		"ReopenFile_DisplayLeftSideBar",	false);
	CreateData_BoolArray(kReopenFile_DisplayRightSideBar,		"ReopenFile_DisplayRightSideBar",	false);
	CreateData_BoolArray(kReopenFile_DisplaySubText,			"ReopenFile_DisplaySubText",		false);
	CreateData_BoolArray(kReopenFile_ShownInSubText,			"ReopenFile_ShownInSubText",		false);
	CreateTableEnd();
	
	//#467
	CreateTableStart();
	CreateData_TextArray(kDirectiveEnabledDefine,				"DirectiveEnabledDefine",			"");
	CreateData_TextArray(kDirectiveEnabledDefineValue,			"DirectiveEnabledDefineValue",		"");
	CreateTableEnd();
	
	//#291
	CreateTableStart();
	CreateData_NumberArray(kInfoPaneArray_Type,					"InfoPaneArray_Type",				0,				0,3);//#695 ,false);
	CreateData_NumberArray(kInfoPaneArray_Height,				"InfoPaneArray_Height",				200,			20,5000);//#695 ,false);
	CreateTableEnd();
	
	//#291
	CreateTableStart();
	CreateData_NumberArray(kSubPaneArray_Type,					"SubPaneArray_Type",				0,				0,3);//#695 ,false);
	CreateData_NumberArray(kSubPaneArray_Height,				"SubPaneArray_Height",				200,			20,5000);//#695 ,false);
	CreateTableEnd();
	
	//#725
	CreateTableStart();
	CreateData_NumberArray(kLeftSideBarArray_Type,				"LeftSideBarArray_Type",				0,				0,9999);
	CreateData_NumberArray(kLeftSideBarArray_Height,			"LeftSideBarArray_Height",				200000,			20000,500000000);//����1���{ #1345 5000000��500000000
	CreateData_NumberArray(kLeftSideBarArray_Parameter,			"LeftSideBarArray_Parameter",			0,				0,9999);
	CreateData_BoolArray(kLeftSideBarArray_Collapsed,			"LeftSideBarArray_Collapsed",			false);
	CreateData_TextArray(kLeftSideBarArray_CurrentPath,			"LeftSideBarArray_CurrentPath",			"");
	CreateTableEnd();
	
	//#725
	CreateTableStart();
	CreateData_NumberArray(kRightSideBarArray_Type,				"RightSideBarArray_Type",				0,				0,9999);
	CreateData_NumberArray(kRightSideBarArray_Height,			"RightSideBarArray_Height",				200000,			20000,500000000);//����1���{ #1345 5000000��500000000
	CreateData_NumberArray(kRightSideBarArray_Parameter,		"RightSideBarArray_Parameter",			0,				0,9999);
	CreateData_BoolArray(kRightSideBarArray_Collapsed,			"RightSideBarArray_Collapsed",			false);
	CreateData_TextArray(kRightSideBarArray_CurrentPath,		"RightSideBarArray_CurrentPath",		"");
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_TextArray(kFindMemoryName,						"FindMemoryName",					"name");
	CreateData_TextArray(kFindMemoryFindText,					"FindMemoryFindText",				"find");
	CreateData_TextArray(kFindMemoryReplaceText,				"FindMemoryReplaceText",			"replace");
	CreateData_BoolArray(kFindMemoryIgnoreCase,					"FindMemoryIgnoreCase",				true);
	CreateData_BoolArray(kFindMemoryWholeWord,					"FindMemoryWholeWord",				false);
	CreateData_BoolArray(kFindMemoryAimai,						"FindMemoryAimai",					true);
	CreateData_BoolArray(kFindMemoryRegExp,						"FindMemoryRegExp",					false);
	CreateData_BoolArray(kFindMemoryLoop,						"FindMemoryLoop",					false);
	CreateTableEnd();
	
	CreateTableStart();//win 080710
	CreateData_TextArray(kRecentlyOpenedFile_old,				"RecentlyOpenedFile",				"");
	CreateData_TextArray(kRecentlyOpenedFile_DocPrefPath,		"RecentlyOpenedFile_DocPrefPath",	"");//win 080710
	CreateTableEnd();//win 080710
	
	//#910
	//�ŋߊJ�����t�@�C��(v3�p)
	CreateTableStart();
	CreateData_TextArray(kRecentlyOpenedFile,					"RecentlyOpenedFileNew",			"");
	CreateData_BoolArray(kRecentlyOpenedFile_Pin,				"RecentlyOpenedFile_Pin",			false);//#394
	CreateTableEnd();
	
	CreateData_TextArray(kMultiFileFindStringCache,				"MultiFileFindStringCache",			"");
	CreateData_TextArray(kMultiFileFindFolderStock,				"MultiFileFindFolderStock",			"");
	CreateData_TextArray(kMultiFileFind_RecentlyUsedFolder,		"MultiFileFind_RecentlyUsedFolder",	"");
	CreateData_TextArray(kMultiFileFind_RecentlyUsedFindString,	"MultiFileFind_RecentlyUsedFindString","");
	CreateData_TextArray(kMultiFileFind_RecentlyUsedFileFilter,	"MultiFileFind_RecentlyUsedFileFilter","");
	CreateData_TextArray(kFindStringCache,						"FindStringCache",					"");
	CreateData_TextArray(kReplaceStringCache,					"ReplaceStringCache",				"");
	
	CreateTableStart();
	CreateData_TextArray(kRecentlyOpenedFileComment,			"RecentlyOpenedFileComment",		"comment");
	CreateData_TextArray(kRecentlyOpenedFileCommentPath,		"RecentlyOpenedFileCommentPath",	"/");
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_TextArray(kFTPFolder_Server,						"FTPFolder_Server",					"ftp.example.com");
	CreateData_TextArray(kFTPFolder_User,						"FTPFolder_User",					"username");
	CreateData_TextArray(kFTPFolder_Path,						"FTPFolder_Path",					"/");
	CreateData_NumberArray(kFTPFolder_Depth,					"FTPFolder_Depth",					10,				1,999);//#695 ,false);
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_BoolArray(kLaunchApp_CheckExtension,				"LaunchApp_CheckExtension",			true);
	CreateData_BoolArray(kLaunchApp_CheckCreator,				"LaunchApp_CheckCreator",			false);
	CreateData_BoolArray(kLaunchApp_CheckType,					"LaunchApp_CheckType",				false);
	CreateData_BoolArray(kLaunchApp_ShiftKey,					"LaunchApp_ShiftKey",				false);
	CreateData_TextArray(kLaunchApp_Extension,					"LaunchApp_Extension",				".suffix");
	CreateData_TextArray(kLaunchApp_Creator,					"LaunchApp_Creator",				"MMKE",true,4,4);
	CreateData_TextArray(kLaunchApp_Type,						"LaunchApp_Type",					"TEXT",true,4,4);
	CreateData_TextArray(kLaunchApp_App,						"LaunchApp_App",					"");
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_TextArray(kFTPAccount_Server,					"FTPAccount_Server",				"example.com");
	CreateData_TextArray(kFTPAccount_User,						"FTPAccount_User",					"username");
	CreateData_BoolArray(kFTPAccount_Passive,					"FTPAccount_Passive",				true);
	CreateData_TextArray(kFTPAccount_ProxyServer,				"FTPAccount_ProxyServer",			"");
	CreateData_TextArray(kFTPAccount_RootPath,					"FTPAccount_RootPath",				"/");
	CreateData_TextArray(kFTPAccount_HTTPURL,					"FTPAccount_HTTPURL",				"http://www.example.com/");
	CreateData_NumberArray(kFTPAccount_Port,					"FTPAccount_Port",					0,				0,65535);//#695 ,false);
	CreateData_NumberArray(kFTPAccount_ProxyMode,				"FTPAccount_ProxyMode",				kProxyMode_NoProxy,	0,2);//#695 ,false);
	CreateData_TextArray(kFTPAccount_ProxyUser,					"FTPAccount_ProxyUser",				"");
	CreateData_BoolArray(kFTPAccount_Backup,					"FTPAccount_Backup",				false);
	CreateData_TextArray(kFTPAccount_BackupFolderPath,			"FTPAccount_BackupFolderPath",		"/");
	CreateData_BoolArray(kFTPAccount_BinaryMode,				"FTPAccount_BinaryMode",			false);
	CreateData_BoolArray(kFTPAccount_BackupWithDateTime,		"FTPAccount_BackupWithDateTime",	false);//R0207
	CreateData_BoolArray(kFTPAccount_UseDefaultPort,			"FTPAccount_UseDefaultPort",		true);//#193
	CreateData_TextArray(kFTPAccount_DisplayName,				"FTPAccount_DisplayName",			"");//#193
	//#361 CreateData_TextArray(kFTPAccount_Password,					"FTPAccount_Passowrd",				"");//win 080802 Windows�p�b��
	CreateData_TextArray(kFTPAccount_ConnectionType,			"FTPAccount_ConnectionType",		"");//#361
	CreateData_BoolArray(kFTPAccount_UsePrivateKey,				"FTPAccount_UsePrivateKey",			false);//#1231
	CreateData_TextArray(kFTPAccount_PrivateKeyFilePath,		"FTPAccount_PrivateKeyFilePath",	"");//#1231
	CreateData_BoolArray(kFTPAccount_DontCheckServerCert,		"FTPAccount_DontCheckServerCert",	false);//#1231
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_TextArray(kTextEncodingMenu,						"TextEncodingMenu",					"UTF-8");
	CreateTableEnd();
	//#844 SetDefaultTextEncodingMenu();
	
	CreateTableStart();
	CreateData_TextArray(kFolderListMenu,						"FolderListMenu",					"");//B0423 "/"��""
	CreateTableEnd();
	
	AText	kinsokudefault;
	kinsokudefault.SetLocalizedText("Kinsoku_Default");
	CreateData_Text(kKinsokuLetters,"KinsokuLetters",kinsokudefault);
	UpdateKinsokuLettersHash();
	
	//B0000 �e�E�C���h�E�̏����ʒu����ʃT�C�Y�ɍ��킹��
	AGlobalRect	screenbounds;
	AWindow::NVI_GetAvailableWindowBoundsInMainScreen(screenbounds);
	APoint	pt;
	//�t�@�C�����X�g
	pt.x = screenbounds.right - GetData_Number(kWindowListSizeX) -16;
	pt.y = screenbounds.bottom - GetData_Number(kWindowListSizeY) -16;
	SetData_Point(kFileListPosition,pt);
	//�}���`�t�@�C�������v���O���X
	pt.x = screenbounds.left + 32;
	pt.y = screenbounds.bottom - 150;
	SetData_Point(kMultiFileFindProgressPosition,pt);
	//FTP�v���O���X
	pt.x = screenbounds.left + 32;
	pt.y = screenbounds.bottom - 150;
	SetData_Point(kFTPProgressPosition,pt);
	
	//#291 InfoPane�f�t�H���g�z�u
	Add_NumberArray(kInfoPaneArray_Type,0);//JumpList
	Add_NumberArray(kInfoPaneArray_Height,600);
	Add_NumberArray(kInfoPaneArray_Type,2);//FileList
	Add_NumberArray(kInfoPaneArray_Height,400);
	
	//#291 SubPane�f�t�H���g�z�u
	Add_NumberArray(kSubPaneArray_Type,1);//Id Info
	Add_NumberArray(kSubPaneArray_Height,150);
	Add_NumberArray(kSubPaneArray_Type,3);//Sub Text Pane
	Add_NumberArray(kSubPaneArray_Height,700);
}

/*#844
void	AAppPrefDB::SetDefaultTextEncodingMenu( AConstCharPtr inString )
{
	AText	text(inString);
	if( text.Compare("-") == true || ATextEncodingWrapper::CheckTextEncodingAvailability(text) == true )
	{
		GetData_TextArrayRef(kTextEncodingMenu).Add(text);
	}
}

void	AAppPrefDB::SetDefaultTextEncodingMenu()
{
#if IMPLEMENTATION_FOR_MACOSX
	SetDefaultTextEncodingMenu("Shift_JIS");
	SetDefaultTextEncodingMenu("ISO-2022-JP");
	SetDefaultTextEncodingMenu("EUC-JP");
	SetDefaultTextEncodingMenu("x-mac-japanese");//#369
	SetDefaultTextEncodingMenu("CP932");//#369
	SetDefaultTextEncodingMenu("Shift_JISX0213");
//	SetDefaultTextEncodingMenu("ISO-2022-JP-3");UTF8����iso-2022-jp-3�ւ�tec converter�쐬�s��
	SetDefaultTextEncodingMenu("-");
	SetDefaultTextEncodingMenu("ISO-8859-1");
	SetDefaultTextEncodingMenu("ISO-8859-2");
	SetDefaultTextEncodingMenu("ISO-8859-4");
	SetDefaultTextEncodingMenu("ISO-8859-10");
	SetDefaultTextEncodingMenu("ISO-8859-13");
//	SetDefaultTextEncodingMenu("ISO-8859-14");UTF8����iso-8859-14�ւ�tec converter�쐬�s��
	SetDefaultTextEncodingMenu("ISO-8859-15");
	SetDefaultTextEncodingMenu("macintosh");
	SetDefaultTextEncodingMenu("windows-1252"); //iso-8859-1�Ɠ����Ɉ�����H
	SetDefaultTextEncodingMenu("-");
	SetDefaultTextEncodingMenu("ISO-8859-5");
	SetDefaultTextEncodingMenu("KOI8-R");
	SetDefaultTextEncodingMenu("x-mac-cyrillic");
	SetDefaultTextEncodingMenu("windows-1251");
	SetDefaultTextEncodingMenu("-");
	SetDefaultTextEncodingMenu("ISO-8859-7");
	SetDefaultTextEncodingMenu("x-mac-greek");
	SetDefaultTextEncodingMenu("windows-1253");
	SetDefaultTextEncodingMenu("-");
	SetDefaultTextEncodingMenu("ISO-8859-9");
	SetDefaultTextEncodingMenu("x-mac-turkish");
	SetDefaultTextEncodingMenu("windows-1254");
	SetDefaultTextEncodingMenu("-");
	SetDefaultTextEncodingMenu("Big5");
	SetDefaultTextEncodingMenu("-");
//	SetDefaultTextEncodingMenu("GB2312");GBK�Ɠ����H
	SetDefaultTextEncodingMenu("HZ-GB-2312");
	SetDefaultTextEncodingMenu("GB18030");
	SetDefaultTextEncodingMenu("GBK");
#elif IMPLEMENTATION_FOR_WINDOWS
	//jp
	SetDefaultTextEncodingMenu("Shift_JIS");
	SetDefaultTextEncodingMenu("ISO-2022-JP");
	SetDefaultTextEncodingMenu("EUC-JP");
	SetDefaultTextEncodingMenu("-");
	//West Europe
	SetDefaultTextEncodingMenu("ISO-8859-1");
	SetDefaultTextEncodingMenu("ISO-8859-15");
	SetDefaultTextEncodingMenu("windows-1252");
	//���b��@���Ή��H������������SetDefaultTextEncodingMenu("macintosh");
	SetDefaultTextEncodingMenu("-");
	//Central Europe
	SetDefaultTextEncodingMenu("ISO-8859-2");
	SetDefaultTextEncodingMenu("windows-1250");
	SetDefaultTextEncodingMenu("-");
	//Other Europe
	SetDefaultTextEncodingMenu("ISO-8859-4");
	SetDefaultTextEncodingMenu("ISO-8859-13");
	SetDefaultTextEncodingMenu("-");
	//Cyril
	SetDefaultTextEncodingMenu("ISO-8859-5");
	SetDefaultTextEncodingMenu("KOI8-R");
	SetDefaultTextEncodingMenu("windows-1251");
	SetDefaultTextEncodingMenu("x-mac-cyrillic");
	SetDefaultTextEncodingMenu("-");
	//Greek
	SetDefaultTextEncodingMenu("ISO-8859-7");
	SetDefaultTextEncodingMenu("windows-1253");
	//SetDefaultTextEncodingMenu("x-mac-greek");
	SetDefaultTextEncodingMenu("-");
	//Turkish
	SetDefaultTextEncodingMenu("ISO-8859-9");
	SetDefaultTextEncodingMenu("windows-1254");
	//SetDefaultTextEncodingMenu("x-mac-turkish");
	SetDefaultTextEncodingMenu("-");
	//Chinese Traditional
	SetDefaultTextEncodingMenu("Big5");
	SetDefaultTextEncodingMenu("-");
	//Chinese Simplified
	SetDefaultTextEncodingMenu("HZ-GB-2312");
	SetDefaultTextEncodingMenu("GB18030");
#endif
}

const ATextArray&	AAppPrefDB::GetTextEncodingMenuFixedTextArrayRef() const
{
	return mTextEncodingMenuFixedTextArray;
}

ABool	AAppPrefDB::ExistTextEncodingMenu( const ATextEncoding& inTextEncoding ) const
{
	AText	target;
	if( ATextEncodingWrapper::GetTextEncodingName(inTextEncoding,target) == false )   return false;
	
	for( AIndex i = 0; i < GetData_ConstTextArrayRef(kTextEncodingMenu).GetItemCount(); i++ )
	{
		AText	text;
		GetData_ConstTextArrayRef(kTextEncodingMenu).Get(i,text);
		if( text.Compare(target) == true )   return true;
	}
	return false;
}

ATextEncoding	AAppPrefDB::GetTextEncodingFromMenuItemIndex( AIndex inIndex ) const
{
	AItemCount	fixcount = GetTextEncodingMenuFixedTextArrayRef().GetItemCount();
	AText	text;
	if( inIndex < fixcount )
	{
		GetTextEncodingMenuFixedTextArrayRef().Get(inIndex,text);
		ATextEncoding	tec;
		ATextEncodingWrapper::GetTextEncodingFromName(text,tec);
		return tec;
	}
	else
	{
		AIndex	index = inIndex-fixcount;
		GetData_ConstTextArrayRef(kTextEncodingMenu).Get(index,text);
		ATextEncoding	tec;
		ATextEncodingWrapper::GetTextEncodingFromName(text,tec);
		return tec;
	}
}

AIndex	AAppPrefDB::GetMenuItemIndexFromTextEncoding( const ATextEncoding& inTextEncoding ) const
{
	AText	name;
	for( AIndex i = 0; i < mTextEncodingMenuFixedTextArray.GetItemCount(); i++ )
	{
		mTextEncodingMenuFixedTextArray.Get(i,name);
		ATextEncoding	tec;
		ATextEncodingWrapper::GetTextEncodingFromName(name,tec);
		if( inTextEncoding == tec )
		{
			return i;
		}
	}
	for( AIndex i = 0; i < GetData_ConstTextArrayRef(kTextEncodingMenu).GetItemCount(); i++ )
	{
		GetData_ConstTextArrayRef(kTextEncodingMenu).Get(i,name);
		ATextEncoding	tec;
		ATextEncodingWrapper::GetTextEncodingFromName(name,tec);
		if( inTextEncoding == tec )
		{
			return mTextEncodingMenuFixedTextArray.GetItemCount() + i;
		}
	}
	return kIndex_Invalid;
}
*/

/**
Setup
*/
void	AAppPrefDB::Setup()
{
	//�e�L�X�g�G���R�[�f�B���O���X�g������
	InitTextEncoding();
	
	//==================�f�t�H���g�L�[�o�C���h�ݒ�==================
	Create_DefaultKeyBindTable();
	
	/*#844 �o�[�W����2.1.7�ȑO�̃��\�[�X�̃f�[�^(MacLegacy)�͓ǂݍ��܂Ȃ����Ƃɂ���
#if IMPLEMENTATION_FOR_MACOSX
	//��MacOS���\�[�X�^�C�v�̃f�[�^�ǂݍ���
	//��ɋ��^�C�v�f�[�^��ǂݍ���ł����āA���̌�A�V�f�[�^�i�̂������݂�����́j�ŏ㏑������B
	try
	{
		LoadMacLegacy();
	}
	catch(...)
	{
		//�t�@�C���̔j�󓙂ɂ��G���[
		_ACError("AAppPrefDB::Setup() caught exception.",NULL);//#199
	}
#endif
	*/
	
	//------------------�o�[�W����3�p�ݒ�t�@�C�����݃`�F�b�N------------------
	AFileAcc	preffolder;
	AFileWrapper::GetAppPrefFolder(preffolder);
	AFileAcc	preffile;
	preffile.SpecifyChild(preffolder,"AppPreferences.mi");
	if( preffile.Exist() == true )
	{
		//==================�o�[�W����3�p�ݒ�t�@�C�������݂���ꍇ==================
		//�o�[�W����3�ݒ�t�@�C�������݂��Ă���΃o�[�W����3�ݒ�t�@�C������ǂݍ���
		LoadFromPrefTextFile(preffile);
	}
	else
	{
		//==================�o�[�W����3�p�ݒ�t�@�C�������݂��Ȃ��ꍇ==================
		//�o�[�W����2.1�p�ݒ�t�@�C������ǂݍ���
		//PrefFile����f�[�^�ǂݍ���
		AFileAcc	v2preffile;
		v2preffile.SpecifyChild(preffolder,"MimikakiEdit Preferences");
#if IMPLEMENTATION_FOR_MACOSX
		LoadFromPrefFile(v2preffile);
#else
		LoadFromPrefTextFile(v2preffile);
#endif
		
		//==================�o�[�W����2.1�p�f�[�^��3.0�f�[�^�ւ̕ϊ�==================
		//�ŋߊJ�����t�@�C���̃f�[�^�ϊ�
		for( AIndex i = 0; i < GetItemCount_TextArray(kRecentlyOpenedFile_old); i++ )
		{
			AText	path;
			GetData_TextArray(kRecentlyOpenedFile_old,i,path);
			AFileAcc	file;
			file.Specify(path,kFilePathType_1);
			file.GetPath(path);
			if( path.GetItemCount() > 0 )
			{
				Add_TextArray(kRecentlyOpenedFile,path);
				Add_BoolArray(kRecentlyOpenedFile_Pin,false);
			}
		}
	}
	/*#725
#if IMPLEMENTATION_FOR_MACOSX
	//���f�[�^������V�f�[�^���ֈڍs
	//�����A�VID�Ńf�[�^�������l�̂܂܁A���A��ID�Ńf�[�^�������l�ȊO�Ȃ�A��ID�̃f�[�^��VID�փR�s�[����
	if( IsStillDefault(kFileListPosition) == true && 
				IsStillDefault(kWindowListOffsetX) == false && IsStillDefault(kWindowListOffsetY) == false )
	{
		APoint	pt;
		pt.x = GetData_Number(kWindowListOffsetX);
		pt.y = GetData_Number(kWindowListOffsetY);
		SetData_Point(kFileListPosition,pt);
	}
	*/
	/*#688
	//win
	if( IsStillDefault(kFileListFontName) == true && IsStillDefault(kFileListFont_compat) == false )
	{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kFileListFont_compat),fontname) == true )
		{
			SetData_Text(kFileListFontName,fontname);
		}
	}
	if( IsStillDefault(kJumpListFontName) == true && IsStillDefault(kJumpListFont_compat) == false )
	{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kJumpListFont_compat),fontname) == true )
		{
			SetData_Text(kJumpListFontName,fontname);
		}
	}
	if( IsStillDefault(kIndexWindowFontName) == true && IsStillDefault(kIndexWindowFont_compat) == false )
	{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kIndexWindowFont_compat),fontname) == true )
		{
			SetData_Text(kIndexWindowFontName,fontname);
		}
	}
	if( IsStillDefault(kFindDialogFontName) == true && IsStillDefault(kFindDialogFont_compat) == false )
	{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kFindDialogFont_compat),fontname) == true )
		{
			SetData_Text(kFindDialogFontName,fontname);
		}
	}
	 #endif
	*/
	
	//SetupLaunchAppData();
	//SetupFTPAccountData();
	
	/*#844
	//#375
	AText	fontname;
	GetData_Text(kAppDefaultFont,fontname);
	if( fontname.GetItemCount() > 0 )
	{
		AFontWrapper::SetAppDefaultFontName(fontname);
	}
	*/
	
	//====================�o�[�W����3.0�p�f�t�H���g�f�[�^�ݒ�====================
	
	//���T�C�h�o�[��Ԃ�v2�̃T�u�y�C����Ԃ�������p��
	//�ikLeftSideBarDisplayed�Ƀf�[�^�����[�h����Ă��炸�AkSubPaneDisplayed�Ƀf�[�^�����[�h����Ă���ꍇ�̂݁j
	if( IsStillDefault(kLeftSideBarDisplayed) == true && IsStillDefault(kSubPaneDisplayed) == false )
	{
		SetData_Bool(kLeftSideBarDisplayed,GetData_Bool(kSubPaneDisplayed));
		SetData_Number(kLeftSideBarWidth,GetData_Number(kSubPaneWidth));
	}
	//�E�T�C�h�o�[��Ԃ�v2�̏��y�C����Ԃ�������p��
	//�ikRightSideBarDisplayed�Ƀf�[�^�����[�h����Ă��炸�AkInfoPaneDisplayed�Ƀf�[�^�����[�h����Ă���ꍇ�̂݁j
	if( IsStillDefault(kRightSideBarDisplayed) == true && IsStillDefault(kInfoPaneDisplayed) == false )
	{
		SetData_Bool(kRightSideBarDisplayed,GetData_Bool(kInfoPaneDisplayed));
		SetData_Number(kRightSideBarWidth,GetData_Number(kInfoPaneWidth));
	}
	
	//���T�C�h�o�[���ڐ���0�Ȃ�f�t�H���g�f�[�^��ݒ肷��B
	//�i�o�[�W����3.0����N����ɃT�C�h�o�[�S�폜���Ă��A�_�~�[�f�[�^(kSubWindowType_None)��1�ǉ������̂ŁAv3�ň�x�ł��N������ΕK�����ڐ���1�ȏ�ɂȂ�B�j
	if( GetItemCount_NumberArray(kLeftSideBarArray_Type) == 0 )
	{
		SetDefaultLeftSideBarArray();
	}
	//�E�T�C�h�o�[���ڐ���0�Ȃ�f�t�H���g�f�[�^��ݒ肷��B
	//�i�o�[�W����3.0����N����ɃT�C�h�o�[�S�폜���Ă��A�_�~�[�f�[�^(kSubWindowType_None)��1�ǉ������̂ŁAv3�ň�x�ł��N������ΕK�����ڐ���1�ȏ�ɂȂ�B�j
	if( GetItemCount_NumberArray(kRightSideBarArray_Type) == 0 )
	{
		SetDefaultRightSideBarArray();
	}
	//�e�L�X�g�}�[�J�[���ڐ���0�Ȃ�f�t�H���g�f�[�^��1�ǉ�����B
	if( GetItemCount_TextArray(kTextMarkerArray_Text) == 0 )
	{
		AText	title, text;
		title.SetLocalizedText("TextMarkerNewGroup");
		Add_TextArray(kTextMarkerArray_Text,text);
		Add_TextArray(kTextMarkerArray_Title,title);
	}
	
	//�����o�b�N�A�b�v�ۑ���t�H���_�p�X����Ȃ�f�t�H���g��ݒ�
	{
		AText	path;
		GetData_Text(kAutoBackupRootFolder,path);
		if( path.GetItemCount() == 0 )
		{
			AFileAcc	folder;
			GetApp().SPI_GetDefaultAutoBackupRootFolder(folder);
			AText	path;
			folder.GetPath(path);
			SetData_Text(kAutoBackupRootFolder,path);
		}
	}
	
	//==================�f�[�^�␳==================
	//���݂��Ȃ��t�H���g�̏ꍇ�A�f�t�H���g�t�H���g��ݒ肷��
	AdjustFontName(kFindDialogFontName);
	AdjustFontName(kSubWindowsFontName);
	AdjustFontName(kPrintOption_PrintFontName);
	AdjustFontName(kPrintOption_PageNumberFontName);
	AdjustFontName(kPrintOption_FileNameFontName);
	AdjustFontName(kPrintOption_LineNumberFontName);
	
	//#1092
	//svn, diff, git�̃p�X�Ƀt�@�C�����Ȃ��ꍇ�́A�f�t�H���g�ɖ߂�
	AFileAcc	cmdfile;
	AText	cmdpath;
	//svn
	GetData_Text(kSVNPath,cmdpath);
	cmdfile.Specify(cmdpath);
	if( cmdfile.Exist() == false )
	{
		SetDefault_Text(kSVNPath);
	}
	//diff
	GetData_Text(kDiffPath,cmdpath);
	cmdfile.Specify(cmdpath);
	if( cmdfile.Exist() == false )
	{
		SetDefault_Text(kDiffPath);
	}
	//git
	GetData_Text(kGitPath,cmdpath);
	cmdfile.Specify(cmdpath);
	if( cmdfile.Exist() == false )
	{
		SetDefault_Text(kGitPath);
	}
	
	//==================
	//�e�L�X�g�t�@�C������pregexp������
	UpdateBinaryFileRegExp();
	
	//==================�ݒ�l�𔽉f==================
	//�h�b�g�P�ʃX�N���[�� #891
	CAppImp::SetUseWheelScrollFloatValue(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kUseWheelScrollFloatValue));
	
	//Sparkle���x���ɂ��feedURL�ݒ�
	AAppPrefDB::SPI_UpdateSparkleUpdateLevel();
}

//#1102
/**
Sparkle���x���ɂ��feedURL�ݒ�
*/
void	AAppPrefDB::SPI_UpdateSparkleUpdateLevel()
{
#ifndef Target_Mac_OS_X_10_6
#ifdef __LP64__
	//Sparkle���x���ɂ��feedURL�ݒ�
	//�f�o�b�O�p�A�b�v�f�[�g
	if( GetApp().NVI_GetEnableDebugMenu() == true )
	{
		[[SUUpdater sharedUpdater] setFeedURL:[NSURL URLWithString:@"https://www.mimikaki.net/download/appcast_debug.xml"]];
		[[SUUpdater sharedUpdater] setAutomaticallyChecksForUpdates:YES];
		return;
	}
	switch(GetData_Number(AAppPrefDB::kSparkleUpdateLevel))
	{
		//���������[�X�̂�
	  case 0:
		{
			[[SUUpdater sharedUpdater] setFeedURL:[NSURL URLWithString:@"https://www.mimikaki.net/download/appcast.xml"]];
			[[SUUpdater sharedUpdater] setAutomaticallyChecksForUpdates:YES];
			break;
		}
		//���������[�X�ƃ�
	  case 1:
		{
			[[SUUpdater sharedUpdater] setFeedURL:[NSURL URLWithString:@"https://www.mimikaki.net/download/appcast_beta.xml"]];
			[[SUUpdater sharedUpdater] setAutomaticallyChecksForUpdates:YES];
			break;
		}
		//���������[�X�ƃ��ƃ�
	  case 2:
		{
			[[SUUpdater sharedUpdater] setFeedURL:[NSURL URLWithString:@"https://www.mimikaki.net/download/appcast_alpha.xml"]];
			[[SUUpdater sharedUpdater] setAutomaticallyChecksForUpdates:YES];
			break;
		}
		//�����X�V�Ȃ�
	  case 3:
		{
			[[SUUpdater sharedUpdater] setFeedURL:[NSURL URLWithString:@""]];
			[[SUUpdater sharedUpdater] setAutomaticallyChecksForUpdates:NO];
			break;
		}
	}
#endif
#endif
}

/**
�E�T�C�h�o�[�f�t�H���g�ݒ�
*/
void	AAppPrefDB::SetDefaultRightSideBarArray()
{
	//�S�폜
	DeleteAll_NumberArray(kRightSideBarArray_Type);
	DeleteAll_NumberArray(kRightSideBarArray_Height);
	DeleteAll_NumberArray(kRightSideBarArray_Parameter);
	DeleteAll_BoolArray(kRightSideBarArray_Collapsed);
	DeleteAll_TextArray(kRightSideBarArray_CurrentPath);
	//�f�t�H���g�ǉ�
	AText	t;
	//���o�����X�g
	Add_NumberArray(kRightSideBarArray_Type,0);//JumpList
	Add_NumberArray(kRightSideBarArray_Height,3000000);
	Add_NumberArray(kRightSideBarArray_Parameter,0);
	Add_BoolArray(kRightSideBarArray_Collapsed,false);
	Add_TextArray(kRightSideBarArray_CurrentPath,t);
	//�����t�H���_�̃t�@�C��
	Add_NumberArray(kRightSideBarArray_Type,2);//FileList
	Add_NumberArray(kRightSideBarArray_Height,2000000);
	Add_NumberArray(kRightSideBarArray_Parameter,1);
	Add_BoolArray(kRightSideBarArray_Collapsed,false);
	Add_TextArray(kRightSideBarArray_CurrentPath,t);
	//�e�L�X�g���
	Add_NumberArray(kRightSideBarArray_Type,9);//DocInf(TextCount)
	Add_NumberArray(kRightSideBarArray_Height,800000);
	Add_NumberArray(kRightSideBarArray_Parameter,0);
	Add_BoolArray(kRightSideBarArray_Collapsed,false);
	Add_TextArray(kRightSideBarArray_CurrentPath,t);
	/*
	//
	Add_NumberArray(kRightSideBarArray_Type,7);//TextMarker
	Add_NumberArray(kRightSideBarArray_Height,1200000);
	Add_NumberArray(kRightSideBarArray_Parameter,0);
	Add_BoolArray(kRightSideBarArray_Collapsed,false);
	Add_TextArray(kRightSideBarArray_CurrentPath,t);
	*/
	/*
	//
	Add_NumberArray(kRightSideBarArray_Type,8);//ClipboardHistory
	Add_NumberArray(kRightSideBarArray_Height,700000);
	Add_NumberArray(kRightSideBarArray_Parameter,0);
	Add_BoolArray(kRightSideBarArray_Collapsed,false);
	Add_TextArray(kRightSideBarArray_CurrentPath,t);
	*/
	/*
	//
	Add_NumberArray(kRightSideBarArray_Type,2);//FileList
	Add_NumberArray(kRightSideBarArray_Height,2000000);
	Add_NumberArray(kRightSideBarArray_Parameter,0);
	Add_BoolArray(kRightSideBarArray_Collapsed,false);
	Add_TextArray(kRightSideBarArray_CurrentPath,t);
	*/
}

/**
���T�C�h�o�[�f�t�H���g�ݒ�
*/
void	AAppPrefDB::SetDefaultLeftSideBarArray()
{
	//�S�폜
	DeleteAll_NumberArray(kLeftSideBarArray_Type);
	DeleteAll_NumberArray(kLeftSideBarArray_Height);
	DeleteAll_NumberArray(kLeftSideBarArray_Parameter);
	DeleteAll_BoolArray(kLeftSideBarArray_Collapsed);
	DeleteAll_TextArray(kLeftSideBarArray_CurrentPath);
	//�f�t�H���g�ǉ�
	AText	t;
	/*
	//
	Add_NumberArray(kLeftSideBarArray_Type,4);//CandidateList
	Add_NumberArray(kLeftSideBarArray_Height,1500000);
	Add_NumberArray(kLeftSideBarArray_Parameter,0);
	Add_BoolArray(kLeftSideBarArray_Collapsed,false);
	Add_TextArray(kLeftSideBarArray_CurrentPath,t);
	*/
	/*
	//
	Add_NumberArray(kLeftSideBarArray_Type,10);//KeyToolList
	Add_NumberArray(kLeftSideBarArray_Height,1500000);
	Add_NumberArray(kLeftSideBarArray_Parameter,0);
	Add_BoolArray(kLeftSideBarArray_Collapsed,false);
	Add_TextArray(kLeftSideBarArray_CurrentPath,t);
	*/
	/*
	//
	Add_NumberArray(kLeftSideBarArray_Type,1);//IdInfo
	Add_NumberArray(kLeftSideBarArray_Height,2500000);
	Add_NumberArray(kLeftSideBarArray_Parameter,0);
	Add_BoolArray(kLeftSideBarArray_Collapsed,false);
	Add_TextArray(kLeftSideBarArray_CurrentPath,t);
	*/
	/*
	//���[�h���X�g
	Add_NumberArray(kLeftSideBarArray_Type,11);//WordsList
	Add_NumberArray(kLeftSideBarArray_Height,3000000);
	Add_NumberArray(kLeftSideBarArray_Parameter,0);
	Add_BoolArray(kLeftSideBarArray_Collapsed,false);
	Add_TextArray(kLeftSideBarArray_CurrentPath,t);
	//�R�[���O���t
	Add_NumberArray(kLeftSideBarArray_Type,5);//CallGraf
	Add_NumberArray(kLeftSideBarArray_Height,3000000);
	Add_NumberArray(kLeftSideBarArray_Parameter,0);
	Add_BoolArray(kLeftSideBarArray_Collapsed,false);
	Add_TextArray(kLeftSideBarArray_CurrentPath,t);
	*/
	//���o�����X�g
	Add_NumberArray(kLeftSideBarArray_Type,0);//JumpList
	Add_NumberArray(kLeftSideBarArray_Height,3000000);
	Add_NumberArray(kLeftSideBarArray_Parameter,0);
	Add_BoolArray(kLeftSideBarArray_Collapsed,false);
	Add_TextArray(kLeftSideBarArray_CurrentPath,t);
	//�����t�H���_�̃t�@�C��
	Add_NumberArray(kLeftSideBarArray_Type,2);//FileList
	Add_NumberArray(kLeftSideBarArray_Height,2000000);
	Add_NumberArray(kLeftSideBarArray_Parameter,1);
	Add_BoolArray(kLeftSideBarArray_Collapsed,false);
	Add_TextArray(kLeftSideBarArray_CurrentPath,t);
	//�e�L�X�g���
	Add_NumberArray(kLeftSideBarArray_Type,9);//DocInf(TextCount)
	Add_NumberArray(kLeftSideBarArray_Height,800000);
	Add_NumberArray(kLeftSideBarArray_Parameter,0);
	Add_BoolArray(kLeftSideBarArray_Collapsed,false);
	Add_TextArray(kLeftSideBarArray_CurrentPath,t);
	//�L�[�L�^
	Add_NumberArray(kLeftSideBarArray_Type,13);//KeyRecord
	Add_NumberArray(kLeftSideBarArray_Height,1500000);
	Add_NumberArray(kLeftSideBarArray_Parameter,0);
	Add_BoolArray(kLeftSideBarArray_Collapsed,false);
	Add_TextArray(kLeftSideBarArray_CurrentPath,t);
	//�v���r���[��
	Add_NumberArray(kLeftSideBarArray_Type,6);//Previewer
	Add_NumberArray(kLeftSideBarArray_Height,4000000);
	Add_NumberArray(kLeftSideBarArray_Parameter,0);
	Add_BoolArray(kLeftSideBarArray_Collapsed,false);
	Add_TextArray(kLeftSideBarArray_CurrentPath,t);
}

/*#844 
void	AAppPrefDB::SetupAfterModeInit()
{
	//TEX���[�h����R�s�[
	if( IsStillDefault(kTexCompilerPath) == true )
	{
		AText	tex("TEX");
		AIndex modeIndex = GetApp().SPI_FindModeIndexByModeRawName(tex);
		if( modeIndex != kIndex_Invalid )
		{
			AText	text;
			GetApp().SPI_GetModePrefDB(modeIndex).GetData_Text(AModePrefDB::kTeXCompiler,text);
			if( text.GetItemCount() == 0 )
			{
				text.SetCstring("/usr/local/bin/platex");
			}
			SetData_Text(kTexCompilerPath,text);
			GetApp().SPI_GetModePrefDB(modeIndex).GetData_Text(AModePrefDB::kTeXPreviewer,text);
			SetData_Text(kTexPreviewerPath,text);
		}
	}
}
*/

void	AAppPrefDB::SaveToFile()
{
	/*#688
#if IMPLEMENTATION_FOR_MACOSX
	//win
	//���f�[�^���փR�s�[
	AText	text;
	AFontNumber	font;
	//
	GetData_Text(kFileListFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kFileListFont_compat,font);
	//
	GetData_Text(kJumpListFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kJumpListFont_compat,font);
	//
	GetData_Text(kIndexWindowFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kIndexWindowFont_compat,font);
	//
	GetData_Text(kFindDialogFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kFindDialogFont_compat,font);
#endif
	*/
	/*#844
	//PrefFile�փf�[�^��������
	AFileAcc	preffile;
	AFileWrapper::GetAppPrefFile(preffile);
#if IMPLEMENTATION_FOR_MACOSX
	ADataBase::WriteToPrefFile(preffile);
#else
	WriteToPrefTextFile(preffile);
#endif
	
#if IMPLEMENTATION_FOR_MACOSX
	WriteMacLegacy();
#endif
	*/
	
	//v3�p�ݒ�t�@�C���Ƀf�[�^��������
	AFileAcc	preffolder;
	AFileWrapper::GetAppPrefFolder(preffolder);
	AFileAcc	preffile;
	preffile.SpecifyChild(preffolder,"AppPreferences.mi");
	WriteToPrefTextFile(preffile);
}

#pragma mark ---FTP

AIndex	AAppPrefDB::FindFTPAccountPrefIndex( const AText& inServer, const AText& inUser ) const
{
	for( AIndex i = 0; i < GetData_ConstTextArrayRef(kFTPAccount_Server).GetItemCount(); i++ )
	{
		AText	text;
		GetData_ConstTextArrayRef(kFTPAccount_Server).Get(i,text);
		if( text.Compare(inServer) == false )   continue;
		GetData_ConstTextArrayRef(kFTPAccount_User).Get(i,text);
		if( text.Compare(inUser) == false )   continue;
		return i;
	}
	return kIndex_Invalid;
}

void	AAppPrefDB::GetHTTPURLFromFTPURL( const AText& inFTPURL, AText& outHTTPURL ) const
{
	//#0
	outHTTPURL.DeleteAll();
	//
	AText	server, user, path;
	inFTPURL.GetFTPURLServer(server);
	inFTPURL.GetFTPURLUser(user);
	inFTPURL.GetFTPURLPath(path);
	AIndex	index = FindFTPAccountPrefIndex(server,user);
	if( index == kIndex_Invalid )   return;//#0
	AText	rootpath;
	GetData_TextArray(kFTPAccount_RootPath,index,rootpath);
	rootpath.AddPathDelimiter(kUChar_Slash);//win 080713
	GetData_TextArray(kFTPAccount_HTTPURL,index,outHTTPURL);
	outHTTPURL.AddPathDelimiter(kUChar_Slash);//win 080713
	path.Delete(0,rootpath.GetLength());
	outHTTPURL.AddText(path);
}

/*ABool	AAppPrefDB::GetInternetPassword( AText& inServer, AText& inUser, AText& outPassword ) const
{
	outPassword.DeleteAll();
	AStTextPtr	pserver(inServer,0,inServer.GetItemCount()), puser(inUser,0,inUser.GetItemCount());
	void*	passwordData = NULL;
	UInt32	passwordLen = 0;
	if( ::SecKeychainFindInternetPassword(NULL,
			pserver.GetByteCount(),(char*)(pserver.GetPtr()),0,NULL,
			puser.GetByteCount(),(char*)(puser.GetPtr()),0,NULL,0,
			0,0,&passwordLen,&passwordData,NULL) == noErr )
	{
		if( passwordData != NULL )
		{
			outPassword.SetFromTextPtr((char*)passwordData,passwordLen);
			::SecKeychainItemFreeContent(NULL,passwordData);
			return true;
		}
	}
	return false;
}

void	AAppPrefDB::SetInternetPassword( AText& inServer, AText& inUser, AText& inPassword ) const
{
	AStTextPtr	pserver(inServer,0,inServer.GetItemCount()), puser(inUser,0,inUser.GetItemCount()), ppassword(inPassword,0,inPassword.GetItemCount());
	SecKeychainItemRef	itemRef;
	if( ::SecKeychainFindInternetPassword(NULL,
			pserver.GetByteCount(),(char*)(pserver.GetPtr()),0,NULL,
			puser.GetByteCount(),(char*)(puser.GetPtr()),0,NULL,0,
			0,0,NULL,NULL,&itemRef) == noErr )
	{
		::SecKeychainItemDelete(itemRef);
	}
	OSStatus status = ::SecKeychainAddInternetPassword(NULL,
			pserver.GetByteCount(),(char*)(pserver.GetPtr()),0,NULL,
			puser.GetByteCount(),(char*)(puser.GetPtr()),0,NULL,0,
			0,0,ppassword.GetByteCount(),ppassword.GetPtr(),NULL);
	if( status != noErr )   _AError("password cannot be added");
}
*/
#pragma mark ---�ŋߊJ�����t�@�C��

void	AAppPrefDB::AddNewRecentlyOpenedFileComment()
{
	AText	text;
	text.SetCstring("/Path");
	GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kRecentlyOpenedFileCommentPath).Add(text);
	text.SetCstring("Comment");
	GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kRecentlyOpenedFileComment).Add(text);
}

void	AAppPrefDB::DeleteRecentlyOpenedFileComment( AIndex inNumber )
{
	GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kRecentlyOpenedFileCommentPath).Delete1(inNumber);
	GetApp().GetAppPref().GetData_TextArrayRef(AAppPrefDB::kRecentlyOpenedFileComment).Delete1(inNumber);
}

//R0148
/*B0389 ABool	AAppPrefDB::GetLastOpenedFile( AFileAcc& outFileAcc )
{
	if( GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFile).GetItemCount() == 0 )   return false;
	AText	text;
	GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFile).Get(0,text);
	outFileAcc.Specify(text,kFilePathType_1);
	return true;
}*/


ANumber	AAppPrefDB::ConstrainWidthHeight( ANumber inNumber )
{
	if( inNumber > 3000 || inNumber < 0 )   return 300;
	else return inNumber;
}

#pragma mark ---�֑�

void	AAppPrefDB::UpdateKinsokuLettersHash()
{
	//mKinsokuLettersHash�̔r������ #890
	AStMutexLocker	locker(mKinsokuLettersHashMutex);
	//mKinsokuLettersHash���X�V
	mKinsokuLettersHash.DeleteAll();
	AText	text;
	GetData_Text(kKinsokuLetters,text);
	for( AIndex	pos = 0; pos < text.GetLength();  )
	{
		AText	ch;
		text.Get1UTF8Char(pos,ch);
		mKinsokuLettersHash.Add(ch);
	}
}

ABool	AAppPrefDB::IsKinsokuLetter( const AText& inText )
{
	//mKinsokuLettersHash�̔r������ #890
	AStMutexLocker	locker(mKinsokuLettersHashMutex);
	//mKinsokuLettersHash���猟��
	AIndex	index = mKinsokuLettersHash.Find(inText);
	if( index != kIndex_Invalid )   return true;
	else return false;
}

#pragma mark ---LaunchApp

ABool	AAppPrefDB::LaunchAppWithFile( const AFileAcc& inFile, const AModifierKeys inModifierKeys )
{
#if IMPLEMENTATION_FOR_MACOSX
	OSType	creator, type;
	inFile.GetCreatorType(creator,type);
	AText	filename;
	inFile.GetFilename(filename);
	AText	suffix;
	filename.GetSuffix(suffix);
	
	/*#844
	AItemCount	count = GetItemCount_Array(kLaunchApp_CheckExtension);
	for( AIndex i = 0; i < count; i++ )
	{
		//�g���q�̃`�F�b�N
		if( GetData_BoolArray(kLaunchApp_CheckExtension,i) == true )
		{
			if( GetData_ConstTextArrayRef(kLaunchApp_Extension).GetTextConst(i).CompareIgnoreCase(suffix) == false )   continue;
		}
		//�N���G�[�^�̃`�F�b�N
		if( GetData_BoolArray(kLaunchApp_CheckCreator,i) == true )
		{
			AText	text;
			GetData_TextArray(kLaunchApp_Creator,i,text);
			//if( AUtil::GetOSTypeFromAText(text) != creator )   continue;
			if( text.GetOSTypeFromText() != creator )   continue;
		}
		//�^�C�v�̃`�F�b�N
		if( GetData_BoolArray(kLaunchApp_CheckType,i) == true )
		{
			AText	text;
			GetData_TextArray(kLaunchApp_Type,i,text);
			//if( AUtil::GetOSTypeFromAText(text) != creator )   continue;
			if( text.GetOSTypeFromText() != type )   continue;
		}
		//�V�t�g�L�[�̃`�F�b�N
		if( GetData_BoolArray(kLaunchApp_ShiftKey,i) == true )
		{
			if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == false )   continue;
		}
		//��v
		AText	apppath;
		GetData_TextArray(kLaunchApp_App,i,apppath);
		AFileAcc	appfile;
		appfile.Specify(apppath);
		AAppAcc	app(appfile);
		//B0000 �N�����s���͎��̌����g�� return ALaunchWrapper::Drop(app,inFile);
		if( ALaunchWrapper::Drop(app,inFile) == true )   return true;
	}
	*/
	//��v����
	//B0326 ���o�[�W������LaunchAndOpen�̓���ɍ��킹��
	//�^�C�v��TEXT, INDX, ���ݒ�Ȃ�mi�ŊJ��
	//����ȊO�Ȃ�OS�ɊJ������ B0326�ďC���iClassicDB�g�p�ł��Ȃ��Ȃ��Ă�H�j
	
	//���ݒ�́u�@�\�v�^�u�́u��e�L�X�g�t�@�C���v�Ŏw�肵���t�@�C�����ǂ����𔻒� #1287
	ABool isBinaryFile = GetApp().GetAppPref().IsBinaryFile(inFile);
	//mi�ŊJ�����ǂ����𔻒�
	if( type == 'TEXT' || type == 'INDX' || (type == kOSTypeNULL && isBinaryFile == false) || AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true )//#1287 type == kOSTypeNULL )//kOSTypeNULL�̏ꍇ��IsBinaryFile()�Ŕ��肷��B���݂ł́A�قƂ�ǂ̃t�@�C����type null
	{
		//GetApp().SPNVI_CreateDocumentFromLocalFile(inFile);
		AText	path;
		inFile.GetPath(path);
		GetApp().SPI_OpenOrRevealTextDocument(path,kIndex_Invalid,kIndex_Invalid,kObjectID_Invalid,false,kAdjustScrollType_Center);
		return true;
	}
	/*else if( type == 'APPL' )
	{
		AAppAcc	app(inFile);
		return ALaunchWrapper::Launch(app);
	}*/
	else 
	{
		//B0326 AAppAcc	app('APPL',creator);
		//B0326 return ALaunchWrapper::Drop(app,inFile);
		//B0326 OS�ɊJ������i�A�v���P�[�V������OS�Ɍ��߂�����j
		return ALaunchWrapper::Open(inFile);
	}
#else
	//���b��
	GetApp().SPNVI_CreateDocumentFromLocalFile(inFile);
	return false;
#endif
}

#pragma mark ---�L�[�o�C���h�i�f�t�H���g�j

const ANumber kKBDTMMask_NoKey 			= 0x00;
const ANumber kKBDTMMask_ControlKey 	= 0x01;
const ANumber kKBDTMMask_OptionKey 		= 0x02;
const ANumber kKBDTMMask_CommandKey 	= 0x04;
const ANumber kKBDTMMask_ShiftKey 		= 0x08;

void	AAppPrefDB::Create_DefaultKeyBindTable()
{
	mKeyBindDefault.DeleteAll();
	mKeyBindDefaultText.DeleteAll();//win
	mKeyBindChangeable.DeleteAll();
	for( AIndex i = 0; i < 16; i++ )
	{
		AIndex newIndex = mKeyBindDefault.AddNewObject();
		mKeyBindDefaultText.AddNewObject();//win
		mKeyBindChangeable.AddNewObject();
		for( AIndex key = 0; key < kKeyBindKeyItemCount; key++ )
		{
			mKeyBindDefault.GetObject(newIndex).Add(keyAction_NOP);
			mKeyBindDefaultText.GetObject(newIndex).Add(GetEmptyText());//win
			
			mKeyBindChangeable.GetObject(newIndex).Add(true);
			
			//==================�L�[�o�C���h���蓖�ĕs�����ݒ�==================
			if( (key >= kKeyBindKey_A && key <= kKeyBindKey_Z) || (key >= kKeyBindKey_Colon && key <= kKeyBindKey_Slash) ||
			   (key >= kKeyBindKey_1 && key <= kKeyBindKey_0) )
			{
#if IMPLEMENTATION_FOR_MACOSX
				//Mac�̏ꍇ�AA-Z, �L������, 0-9�́ACtrl��������Ă���ꍇ�A���ACmd��������Ă��Ȃ��ꍇ�̂ݗL���Ƃ���
				if( (i&kKBDTMMask_ControlKey) == 0 || (i&kKBDTMMask_CommandKey) != 0 )
				{
					mKeyBindChangeable.GetObject(newIndex).Set(key,false);
				}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
				//Windows�̏ꍇ�AA-Z, �L������, 0-9�́ACtrl, Ctrl+Shift�ƂƂ��ɉ�����A����ȊO��������Ă��Ȃ��ꍇ�̂ݏ�������
				if( (i&kKBDTMMask_ControlKey) == 0 || (i&kKBDTMMask_CommandKey) != 0 || 
							(i&kKBDTMMask_OptionKey) != 0 )
				{
					mKeyBindChangeable.GetObject(newIndex).Set(key,false);
				}
#endif
			}
			else if( key >= kKeyBindKey_F1 && key <= kKeyBindKey_F15 )
			{
				//F1-F15�́ACmd��������Ă��Ȃ��ꍇ�̂ݗL���Ƃ���
				if( (i&kKBDTMMask_CommandKey) != 0 )
				{
					mKeyBindChangeable.GetObject(newIndex).Set(key,false);
				}
			}
			else if( key == kKeyBindKey_Return || key == kKeyBindKey_Tab || key == kKeyBindKey_Space )//#1017
			{
				//���s�A�^�u�A�X�y�[�X�́ACmd��������Ă��Ȃ��ꍇ�̂ݗL���Ƃ���
				if( (i&kKBDTMMask_CommandKey) != 0 )
				{
					mKeyBindChangeable.GetObject(newIndex).Set(key,false);
				}
			}
			else if( key == kKeyBindKey_Escape )
			{
				//ESC�L�[��Cmd+Option��������Ă���ꍇ�͖����Ƃ��� #1017
				if( (i&kKBDTMMask_CommandKey) != 0 && (i&kKBDTMMask_OptionKey) != 0 )
				{
					mKeyBindChangeable.GetObject(newIndex).Set(key,false);
				}
			}
#if IMPLEMENTATION_FOR_WINDOWS
			//Windows�̏ꍇ�AAlt�L�[��Ctrl�Ɠ����ɉ������ꍇ�̂ݗL��
			if( (i&kKBDTMMask_CommandKey) != 0 )
			{
				if( (i&kKBDTMMask_ControlKey) == 0 )
				{
					mKeyBindChangeable.GetObject(newIndex).Set(key,false);
				}
			}
#endif
			
		}
		
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_Return,		keyAction_indentreturn);
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_Tab,			keyAction_indenttab);
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_Space,		keyAction_space);
		/*#844 Espace�L�[�ŕ⊮�͂��Ȃ�
#if IMPLEMENTATION_FOR_MACOSX
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_Escape,		keyAction_abbrevnext);
#endif
		*/
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_Enter,		keyAction_return);
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_BS,			keyAction_deleteprev);
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_DEL,			keyAction_deletenext);
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_Up,			keyAction_caretup);
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_Down,		keyAction_caretdown);
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_Left,		keyAction_caretleft);
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_Right,		keyAction_caretright);
		
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_Home,		keyAction_home);//keyAction_caretlinestart);//#489
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_End,		keyAction_end);//keyAction_caretlineend);//#489
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_PageUp,		keyAction_pageup);
		mKeyBindDefault.GetObject(newIndex).Set(kKeyBindKey_PageDown,	keyAction_pagedown);
	}
	//Modifier�L�[�����Ȃ�
#if IMPLEMENTATION_FOR_MACOSX
	mKeyBindDefault.GetObject(kKBDTMMask_NoKey).							Set(kKeyBindKey_F1,			keyAction_Undo);
	mKeyBindDefault.GetObject(kKBDTMMask_NoKey).							Set(kKeyBindKey_F2,			keyAction_Cut);
	mKeyBindDefault.GetObject(kKBDTMMask_NoKey).							Set(kKeyBindKey_F3,			keyAction_Copy);
	mKeyBindDefault.GetObject(kKBDTMMask_NoKey).							Set(kKeyBindKey_F4,			keyAction_Paste);
#endif
	/* Windows�̃L�[�o�C���h�\�p�^�[��
	Ctrl+x:OK
	Ctrl+Shift+x:OK
	Ctrl+Shift+Alt+x:OK
	Ctrl+Alt+x:OK
	Shift+x:OK
	Alt+x:NG
	Alt+Shift+x:NG
	*/
	//Control�L�[�̂�
#if IMPLEMENTATION_FOR_MACOSX
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_A,			keyAction_home);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_C,			keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_D,			keyAction_end);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_H,			keyAction_deleteprev);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_I,			keyAction_indenttab);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_K,			keyAction_pageup);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_L,			keyAction_pagedown);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_M,			keyAction_indentreturn);
#endif
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Return,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Tab,		keyAction_tab);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Space,		keyAction_space);
	/*#844
#if IMPLEMENTATION_FOR_MACOSX
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Escape,		keyAction_abbrevnext);
#endif
*/
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Enter,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_BS,			keyAction_deleteprev);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_DEL,		keyAction_deletenext);
#if IMPLEMENTATION_FOR_MACOSX
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Up,			keyAction_scrollup);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Down,		keyAction_scrolldown);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Left,		keyAction_scrollleft);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Right,		keyAction_scrollright);
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	//Windows��Ctrl+�������A�ύX�ӏ��ړ��Ɋ��蓖�āB
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Up,			keyAction_prevdiff);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Down,		keyAction_nextdiff);
	//Windows��Ctrl+���@�����A�P��ړ��Ɋ��蓖�Ă�B�P��ړ�����ԗD��x�����B
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Left,		keyAction_previousword_linestop);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_Right,		keyAction_nextword_linestop);
#endif
#if IMPLEMENTATION_FOR_MACOSX
	//Option�L�[�̂�(MacOSX��p)
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_Return,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_Tab,		keyAction_tab);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_Space,		keyAction_space);
	/*#844
#if IMPLEMENTATION_FOR_MACOSX
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_Escape,		keyAction_abbrevnext);
#endif
*/
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_Enter,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_BS,			keyAction_deleteprev);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_DEL,		keyAction_deletenext);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_Up,			keyAction_prevdiff);//R0006
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_Down,		keyAction_nextdiff);//R0006
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_Left,		keyAction_previousword);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey).						Set(kKeyBindKey_Right,		keyAction_nextword);
	//Command�L�[�̂�(MacOSX��p)
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_Return,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_Tab,		keyAction_tab);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_Space,		keyAction_space);
	/*#844
#if IMPLEMENTATION_FOR_MACOSX
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_Escape,		keyAction_abbrevnext);
#endif
*/
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_Enter,		keyAction_return);
    mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_BS,			keyAction_deletelinestart);//#1206
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_DEL,		keyAction_deletenext);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_Up,			keyAction_carethome);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_Down,		keyAction_caretend);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_Left,		keyAction_caretlinestart);//B0319
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey).					Set(kKeyBindKey_Right,		keyAction_caretlineend);//B0319
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	//Windows�̏ꍇ�A�����L�[�ɑ΂��āA�uCommand�L�[�̂݁v�Ɠ����L�[�o�C���h��Ctrl+Alt�Ɋ��蓖�Ă�
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_CommandKey).Set(kKeyBindKey_Up,			keyAction_carethome);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_CommandKey).Set(kKeyBindKey_Down,		keyAction_caretend);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_CommandKey).Set(kKeyBindKey_Left,		keyAction_caretlinestart);//B0319
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_CommandKey).Set(kKeyBindKey_Right,		keyAction_caretlineend);//B0319
#endif
	//Shift�L�[�̂�
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_Return,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_Tab,		keyAction_shiftleft);//#1172 keyAction_abbrevprev);//#1160 keyAction_shiftleft);//#815
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_Space,		keyAction_abbrevnext);//#1160
	/*#844
#if IMPLEMENTATION_FOR_MACOSX
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_Escape,		keyAction_abbrevprev);
#endif
*/
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_Enter,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_BS,			keyAction_deletenext);//#452
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_DEL,		keyAction_deletenext);
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_Up,			keyAction_selectup);
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_Down,		keyAction_selectdown);
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_Left,		keyAction_selectleft);
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).						Set(kKeyBindKey_Right,		keyAction_selectright);
	
	//Control+T: ���E��������ւ�
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).					Set(kKeyBindKey_T,			keyAction_swapLetter);//#1154
	
#if IMPLEMENTATION_FOR_MACOSX
	/*#844
	//Option+Shift(MacOSX��p)
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_Return,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_Tab,		keyAction_abbrevprev);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_Space,		keyAction_abbrevnext);
	*/
	/*#844
#if IMPLEMENTATION_FOR_MACOSX
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_Escape,		keyAction_abbrevprev);
#endif
*/
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_Enter,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_BS,			keyAction_deleteprev);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_DEL,		keyAction_deletenext);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_Up,			keyAction_selectup);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_Down,		keyAction_selectdown);
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_Left,		keyAction_selectprevword);//B0356
	mKeyBindDefault.GetObject(kKBDTMMask_OptionKey+kKBDTMMask_ShiftKey).	Set(kKeyBindKey_Right,		keyAction_selectnextword);//B0356
	//Command+Shift(MacOSX��p)
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Return,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Tab,		keyAction_tab);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Space,		keyAction_space);
	/*#844
#if IMPLEMENTATION_FOR_MACOSX
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Escape,		keyAction_abbrevprev);
#endif
*/
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Enter,		keyAction_return);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_BS,			keyAction_deleteprev);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_DEL,		keyAction_deletenext);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Up,			keyAction_selecthome);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Down,		keyAction_selectend);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Left,		keyAction_selectlinestart);
	mKeyBindDefault.GetObject(kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Right,		keyAction_selectlineend);
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	//Windows�̏ꍇ�A�����L�[�ɑ΂��āA�uCommand+Shift�v�Ɠ����L�[�o�C���h��Ctrl+Alt+Shift�Ɋ��蓖�Ă�
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Up,			keyAction_selecthome);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Down,		keyAction_selectend);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Left,		keyAction_selectlinestart);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_CommandKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Right,		keyAction_selectlineend);
	//Control+Shift �P��ړ��I��
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Left,		keyAction_selectpreviousword_linestop);
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_Right,		keyAction_selectnextword_linestop);
#endif
	//�L�[�{�[�h�V���b�g�J�b�g�ݒ�
	//�L�[�{�[�h�V���[�g�J�b�g�Ƃ̏d�������������ɂ̓A�N�Z�����[�^�ł͂Ȃ�������Őݒ肷��̂��x�X�g�Ɣ��f
#if IMPLEMENTATION_FOR_WINDOWS
	AText	text;
	//additional copy
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_C,keyAction_additionalcopy);
	//close
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_W,keyAction_string);
	text.SetCstring("<<<CLOSE");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_W,text);
	//copy
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_C,keyAction_Copy);
	//correspond text
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_J,keyAction_correspondence);
	//cut
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_X,keyAction_Cut);
	//find
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_F,keyAction_string);
	text.SetCstring("<<<SHOW-FINDDIALOG");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_F,text);
	//highlight-clear
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_F,keyAction_string);
	text.SetCstring("<<<HIGHLIGHT-CLEAR");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_F,text);
	//find next
	mKeyBindDefault.GetObject(0).Set(kKeyBindKey_F3,keyAction_string);
	text.SetCstring("<<<FIND-NEXT");
	mKeyBindDefaultText.GetObject(0).Set(kKeyBindKey_F3,text);
	//find prev
	mKeyBindDefault.GetObject(kKBDTMMask_ShiftKey).Set(kKeyBindKey_F3,keyAction_string);
	text.SetCstring("<<<FIND-PREV");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ShiftKey).Set(kKeyBindKey_F3,text);
	//indent
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_T,keyAction_string);
	text.SetCstring("<<<INDENT");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_T,text);
	//index next
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_F3,keyAction_string);
	text.SetCstring("<<<INDEXWINDOW-NEXT");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_F3,text);
	//find prev
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_F3,keyAction_string);
	text.SetCstring("<<<INDEXWINDOW-PREV");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_F3,text);
	//keytool
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_U,keyAction_string);
	text.SetCstring("<<<KEYTOOL()");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_U,text);
	//multifile find
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_M,keyAction_string);
	text.SetCstring("<<<SHOW-MULTIFILEFINDDIALOG");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_M,text);
	//open selected
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_D,keyAction_string);
	text.SetCstring("<<<OPEN-SELECTED");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_D,text);
	//open
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_O,keyAction_string);
	text.SetCstring("<<<OPEN-DIALOG");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_O,text);
	//paste
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_V,keyAction_Paste);
	//redo
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_Y,keyAction_Redo);
	//save
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_S,keyAction_string);
	text.SetCstring("<<<SAVE");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_S,text);
	//select all
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_A,keyAction_string);
	text.SetCstring("<<<SELECT-ALL");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_A,text);
	//select correspondence
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_J,keyAction_string);
	text.SetCstring("<<<SELECT-CORRESPONDENCE");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey+kKBDTMMask_ShiftKey).Set(kKeyBindKey_J,text);
	//setfindtext
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_E,keyAction_string);
	text.SetCstring("<<<SETFINDTEXT-SELECTED");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_E,text);
	//shift left
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_BracketStart,keyAction_string);
	text.SetCstring("<<<SHIFT-LEFT");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_BracketStart,text);
	//shift right
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_BracketEnd,keyAction_string);
	text.SetCstring("<<<SHIFT-RIGHT");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_BracketEnd,text);
	//undo
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_Z,keyAction_Undo);
	//print
	mKeyBindDefault.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_P,keyAction_string);
	text.SetCstring("<<<SHOW-PRINTDIALOG");
	mKeyBindDefaultText.GetObject(kKBDTMMask_ControlKey).Set(kKeyBindKey_P,text);
#endif
}

//�Ԃ�l�́A�L�[�o�C���h�ύX�\���ǂ���������
ABool	AAppPrefDB::GetData_DefaultKeyBindAction( const AKeyBindKey& inKey, const AModifierKeys inModiferKeys, AKeyBindAction& outAction, AText& outText )
{
	AIndex	index = 0;
	if( AKeyWrapper::IsControlKeyPressed(inModiferKeys) == true )
	{
		index += kKBDTMMask_ControlKey;
	}
	if( AKeyWrapper::IsOptionKeyPressed(inModiferKeys) == true )
	{
		index += kKBDTMMask_OptionKey;
	}
	if( AKeyWrapper::IsCommandKeyPressed(inModiferKeys) == true )
	{
		index += kKBDTMMask_CommandKey;
	}
	if( AKeyWrapper::IsShiftKeyPressed(inModiferKeys) == true )
	{
		index += kKBDTMMask_ShiftKey;
	}
	outAction = static_cast<AKeyBindAction>(mKeyBindDefault.GetObject(index).Get(inKey));
	mKeyBindDefaultText.GetObject(index).Get(inKey,outText);//win
	return mKeyBindChangeable.GetObject(index).Get(inKey);
}

#pragma mark ---�t�@�C���p�X

//B0389
void	AAppPrefDB::GetFilePathForDisplay( const AFileAcc& inFile, AText& outPath ) const
{
	outPath.DeleteAll();
	/*#844 �t�@�C���p�X�\���^�C�v�ݒ�p�~
	switch(GetData_Number(kFilePathDisplayType))
	{
	  case 0:
		{
			inFile.GetNormalizedPath(outPath);//B0000
			break;
		}
	  case 1:
		{
			inFile.GetPath(outPath,kFilePathType_2);
			if( outPath.GetItemCount() > 0 )
			{
				outPath.Delete(0,1);
			}
			break;
		}
	  case 2:
		{
			inFile.GetPath(outPath,kFilePathType_1);
			break;
		}
	  default:
		{
			//��������
			break;
		}
	}
	*/
	
	//�p�X�擾
	inFile.GetPath(outPath);
	//unicode comp�`���ɕϊ�
	outPath.ConvertToCanonicalComp();//B0000
    //�t�H���_�̏ꍇ�͕K���Ō��/��t����
	/*#932
    if( inFile.IsFolder() == true )
    {
		AUChar	delim = AFileAcc::GetPathDelimiter();//#844 (AFilePathType)GetData_Number(kFilePathDisplayType));
        if( outPath.GetItemCount() > 0 )
        {
            if( outPath.Get(outPath.GetItemCount()-1) != delim )
            {
                outPath.Add(delim);
            }
        }
    }
	 */
}

#pragma mark ---�o�C�i���t�@�C������

/**
�o�C�i���t�@�C�����ǂ������擾
*/
ABool	AAppPrefDB::IsBinaryFile( const AFileAcc& inFile ) const
{
	//�r������
	AStMutexLocker	locker(mBinaryFileRegExpMutex);
	//�t�@�C�����擾
	AText	filename;
	inFile.GetFilename(filename);
	//�o�C�i���t�@�C����`�Ɉ�v���邩�ǂ�������
	AIndex	pos = 0;
	if( mBinaryFileRegExp.Match(filename,pos,filename.GetItemCount()) == true )
	{
		return true;
	}
	return false;
}

/**
�o�C�i���t�@�C����`regexp�X�V
*/
void	AAppPrefDB::UpdateBinaryFileRegExp()
{
	//�r������
	AStMutexLocker	locker(mBinaryFileRegExpMutex);
	//���C���h�J�[�h�`�����琳�K�\���ɕϊ�
	AText	wildcard, regexpText;
	GetData_Text(kBinaryFileDefinition,wildcard);
	ARegExp::ConvertToREFromWildCard(wildcard,regexpText);
	//regexp�ݒ�
	mBinaryFileRegExp.SetRE(regexpText,true);
	mBinaryFileRegExp.InitGroup();
}

#pragma mark ---TextEncoding

/**
�e�L�X�g�G���R�[�f�B���O���X�g������
*/
void	AAppPrefDB::InitTextEncoding()
{
	//�A�v���P�[�V�����̃e�L�X�g�G���R�[�f�B���O���X�g�t�@�C�����擾
	AFileAcc	appTextEncodingFile;
	GetApp().SPI_GetAppTextEncodingFile(appTextEncodingFile);
	//�e�L�X�g�G���R�[�f�B���O���X�g�t�@�C����ǂݍ���
	AText	text;
	appTextEncodingFile.ReadTo(text);
	//���[�U�[�̃e�L�X�g�G���R�[�f�B���O���X�g�t�@�C�����擾
	AFileAcc	userTextEncodingFile;
	GetApp().SPI_GetUserTextEncodingFile(userTextEncodingFile);
	//�e�L�X�g�G���R�[�f�B���O���X�g�t�@�C����ǂݍ���
	AText	t;
	userTextEncodingFile.ReadTo(t);
	text.AddText(t);
	//CSV�ǂݍ���
	ACSVFile	csv(text,ATextEncodingWrapper::GetUTF8TextEncoding(),2);
	csv.GetColumn(0,mTextEncodingNameArray);
	csv.GetColumn(1,mTextEncodingDisplayNameArray);
	//���X�g�ɐݒ�
	for( AIndex i = 0; i < mTextEncodingNameArray.GetItemCount(); i++ )
	{
		AText	tecName;
		mTextEncodingNameArray.Get(i,tecName);
		AText	displayName;
		mTextEncodingDisplayNameArray.Get(i,displayName);
		if( displayName.GetItemCount() == 0 )
		{
			mTextEncodingDisplayNameArray.Set(i,tecName);
		}
		//#1040
		//�e�L�X�g�G���R�[�f�B���O�l���i�[
		mTextEncodingArray.Add(ATextEncodingWrapper::GetTextEncodingFromName(tecName));
	}
}

/**
�e�L�X�g�G���R�[�f�B���O���j���[�ɑ��݂��Ă��邩�ǂ������擾
*/
ABool	AAppPrefDB::IsTextEncodingAvailable( const AText& inTextEncodingName ) const
{
	return (mTextEncodingNameArray.Find(inTextEncodingName)!=kIndex_Invalid); 
}

//#1040
/**
�e�L�X�g�G���R�[�f�B���O���̂̃m�[�}���C�Y
�e�L�X�g�G���R�[�f�B���O���j���[�ɑ��݂��Ă���G���R�[�f�B���O�Ȃ�A�啶�������������킹��
*/
void	AAppPrefDB::ConvertToNormalizedTextEncodingName( AText& ioName ) const
{
	//IANA������ATextEncoding�擾
	ATextEncoding	tec;
	if( ATextEncodingWrapper::GetTextEncodingFromName(ioName,tec) == true )
	{
		//���j���[�ɊY��������̂�����΁A���̃e�L�X�g�G���R�[�f�B���O�����擾����
		AIndex	index = mTextEncodingArray.Find(tec);
		if( index != kIndex_Invalid )
		{
			mTextEncodingNameArray.Get(index,ioName);
		}
	}
}



