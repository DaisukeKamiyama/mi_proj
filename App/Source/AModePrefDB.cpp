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

AModePref

*/

#include "AModePrefDB.h"
#ifndef MODEPREFCONVERTER
//modeprefconverterでは使用しない箇所 #1034
#include "AApp.h"
#if IMPLEMENTATION_FOR_MACOSX
#include <unistd.h>
#endif
#include "AToolPrefDB.h"//#305
#include "ACSVKeywordLoader.h"
#endif

#pragma mark ===========================
#pragma mark [クラス]AModePrefDB
#pragma mark ===========================
//ModePrefDB

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AModePrefDB::AModePrefDB( AObjectArrayItem* inParent )
: ADataBase(inParent)
#ifndef MODEPREFCONVERTER
//modeprefconverterでは使用しない箇所 #1034
, mLoaded(false), 
		mBackgroundImageIndex(kIndex_Invalid), mJumpSetupCategoryStartIndex(0), 
		mToolMenuRootObjectID(kObjectID_Invalid), mKeywordList(this)
//#427,mAdditionalCategoryArray_KeywordArray(NULL,true)//#417
		,mForceAutoUpdate(false),mLoadingRevision(0)//#427
,mDBLoaded(false)//#427 #852
,mAdditionalCategoryStartIndex(0)//#791
,mSDFIsSimpleDefinition(false)//#907
,mToolMenuRoot_AppToolStartIndex(kIndex_Invalid)
,mToolMenuRoot_AppToolEndIndex(kIndex_Invalid)
,mToolMenuRoot_UserToolStartIndex(kIndex_Invalid)
,mToolMenuRoot_UserToolEndIndex(kIndex_Invalid)
,mPreviewPluginExist(false)
#endif
{
#ifndef MODEPREFCONVERTER
//modeprefconverterでは使用しない箇所 #1034
	//キーワードサーチ時、情報テキストが空でなければ、重複チェックしないようにする
	mKeywordList.SetDontCheckDuplicateIfInfoTextExists();
#endif
	
	/*#688
	ANumber	oldDefaultFontNumber = 0;
#if IMPLEMENTATION_FOR_MACOSX
	oldDefaultFontNumber = AFontWrapper::GetDefaultFont();
#endif
	//旧データ
	CreateData_Number(kDefaultFont_compat,							"DefaultFont",							oldDefaultFontNumber,kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kDefaultPrintOption_PrintFont_compat,			"DefaultPrintOption_PrintFont",			oldDefaultFontNumber,kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kDefaultPrintOption_PageNumberFont_compat,	"DefaultPrintOption_PageNumberFont",	oldDefaultFontNumber,kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kDefaultPrintOption_FileNameFont_compat,		"DefaultPrintOption_FileNameFont",		oldDefaultFontNumber,kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kDefaultPrintOption_LineNumberFont_compat,	"DefaultPrintOption_LineNumberFont",	oldDefaultFontNumber,kNumber_MinNumber,kNumber_MaxNumber);//win
	*/
	//				データID								ファイル用Key							初期値			(最小,最大)			(最小,最大)
	CreateData_Bool(kUseRegExpCommentStart,					"UseRegExpCommentStart",			false);
	CreateData_Bool(kUseRegExpCommentEnd,					"UseRegExpCommentEnd",				false);
	CreateData_Number(kBackgroundImageTransparency,			"BackgroundImageTransparency",		85,				0,100);
	CreateData_Number(kWrapWithLetterCount_LetterCount,		"WrapWithLetterCount LetterCount",	80,				2,9999);
	CreateData_Bool(kConvert5CToA5WhenOpenJIS,				"Convert5CToA5WhenOpenJIS",			false);
	
	//				データID								ファイル用Key							初期値			(最小,最大)			(最小,最大)
	CreateData_Bool(kDefaultEnableAntiAlias,				"DefaultEnableAntiAlias",			true);
	CreateData_Bool(kDisplayBackgroundImage,				"DisplayBackgroundImage",			false);
	CreateData_Bool(kDisplayInformationHeader,				"DisplayInformationHeader",			true);
	CreateData_Bool(kDisplayToolbar,						"DisplayToolbar",					true);
	CreateData_Bool(kDisplayPositionButton,					"DisplayPositionButton",			true);
	CreateData_Bool(kDisplayJumpList,						"DisplayJumpList",					false);
	CreateData_Bool(kDisplayRuler,							"DisplayRuler",						false);
	CreateData_Bool(kRulerScaleWithTabLetter,				"RulerScaleWithTabLetter",			false);
	CreateData_Bool(kDisplayReturnCode,						"DisplayReturnCode",				true);
	CreateData_Bool(kDisplayTabCode,						"DisplayTabCode",					true);
	CreateData_Bool(kDisplaySpaceCode,						"DisplaySpaceCode",					false);
	CreateData_Bool(kDisplayZenkakuSpaceCode,				"DisplayZenkakuSpaceCode",			false);
	CreateData_Bool(kDisplayGenkoyoshi,						"DisplayGenkoyoshi",				false);
	CreateData_Bool(kCheckBrace,							"CheckBrace",						true);
	CreateData_Bool(kSelectInsideBraceByDoubleClick,		"SelectInsideBraceByDoubleClick",	true);
	CreateData_Bool(kSelectInsideByQuadClick,				"SelectInsideByQuadClick",			false);
	CreateData_Bool(kKinsokuBuraasge,						"KinsokuBuraasge",					true);
	CreateData_Bool(kInputSpacesByTabKey,					"InputSpacesByTabKey",				false);
	CreateData_Bool(kAddRemoveSpaceByDragDrop,				"AddRemoveSpaceByDragDrop",			false);
	CreateData_Bool(kInputBackslashByYenKey,				"InputBackslashByYenKey",			false);//#917 デフォルトfalseに変更
	CreateData_Bool(kDontSaveResouceFork,					"DontSaveResouceFork",				true);
	CreateData_Bool(kSaveUTF8BOM,							"SaveUTF8BOM",						false);
	CreateData_Bool(kCheckCharsetWhenSave,					"CheckCharsetWhenSave",				true);
	CreateData_Bool(kSaveDOSEOF,							"SaveDOSEOF",						false);
	CreateData_Bool(kSaveWindowPosition,					"SaveWindowPosition",				true);
	CreateData_Bool(kSniffTextEncoding_BOM,					"SniffTextEncoding_BOM",			true);
	CreateData_Bool(kSniffTextEncoding_Charset,				"SniffTextEncoding_Charset",		true);
	CreateData_Bool(kSniffTextEncoding_Legacy,				"SniffTextEncoding_Legacy",			true);
	CreateData_Bool(kShowDialogWhenTextEncodingNotDecided,	"ShowDialogWhenTextEncodingNotDecided",true);
	CreateData_Bool(kApplyCharsForWordToDoubleClick,		"ApplyCharsForWordToDoubleClick",	false);
	CreateData_Bool(kUseCIndent,							"UseCIndent",						false);
	CreateData_Bool(kUseCSyntaxColor,						"UseCSyntaxColor",					false);
	
	//文法定義関連
	//#349 CreateData_Bool(kUseSyntaxDefinitionFile,				"UseSyntaxDefinitionFile",			false);
	CreateData_Bool(kUseBuiltinSyntaxDefinitionFile,		"UseBuiltinSyntaxDefinitionFile",	false);
	CreateData_Text(kBuiltinSyntaxDefinitionName,			"BuiltinSyntaxDefinitionName",		"C/C++");
	
	CreateData_Bool(kDisplayLineNumberButton,				"DisplayLineNumberButton",			true);
	CreateData_Bool(kDisplayLineNumberButton_AsParagraph,	"DisplayLineNumberButton_AsParagraph",true);
	CreateData_Bool(kDisplayEachLineNumber,					"DisplayEachLineNumber",			true);
	CreateData_Bool(kDisplayEachLineNumber_AsParagraph,		"DisplayEachLineNumber_AsParagraph",true);
	
	CreateData_Bool(kDefaultPrintOption_UsePrintFont,		"DefaultPrintOption_UsePrintFont",	false);
	CreateData_Bool(kDefaultPrintOption_PrintPageNumber,	"DefaultPrintOption_PrintPageNumber",true);
	CreateData_Bool(kDefaultPrintOption_PrintFileName,		"DefaultPrintOption_PrintFileName",	true);
	CreateData_Bool(kDefaultPrintOption_PrintLineNumber,	"DefaultPrintOption_PrintLineNumber",true);
	CreateData_Bool(kDefaultPrintOption_PrintSeparateLine,	"DefaultPrintOption_PrintSeparateLine",true);
	CreateData_Bool(kDefaultPrintOption_ForceWordWrap,		"DefaultPrintOption_ForceWordWrap",	true);
	
	CreateData_Color(kLetterColor,							"LetterColor",						kColor_Black);
	CreateData_Color(kControlCodeColor,						"ControlCodeColor",					kColor_Gray50Percent);//#1142
	CreateData_Color(kBackgroundColor,						"BackgroundColor",					kColor_White);
	CreateData_Color(kSpecialCodeColor,						"SpecialCodeColor",					kColor_Gray80Percent);
	CreateData_Color(kCommentColor,							"CommentColor",						kColor_Red);
	CreateData_Color(kLiteralColor,							"LiteralColor",						kColor_Gray);
	
	CreateData_Color(kCSyntaxColor_Function,				"CSyntaxColor_Function",			kColor_Green);
	CreateData_Color(kCSyntaxColor_ExternalFunction,		"CSyntaxColor_ExternalFunction",	kColor_Green);
	CreateData_Color(kCSyntaxColor_Class,					"CSyntaxColor_Class",				kColor_Yellow);
	CreateData_Color(kCSyntaxColor_GlobalVariable,			"CSyntaxColor_GlobalVariable",		kColor_Blue);
	CreateData_Color(kCSyntaxColor_LocalVariable,			"CSyntaxColor_LocalVariable",		kColor_Blue);
	CreateData_Color(kCSyntaxColor_ExternalVariable,		"CSyntaxColor_ExternalVariable",	kColor_Blue);
	CreateData_Color(kCSyntaxColor_EnumType,				"CSyntaxColor_EnumType",			kColor_Yellow);
	CreateData_Color(kCSyntaxColor_Enum,					"CSyntaxColor_Enum",				kColor_Yellow);
	CreateData_Color(kCSyntaxColor_Typedef,					"CSyntaxColor_Typedef",				kColor_Yellow);
	CreateData_Color(kCSyntaxColor_Struct,					"CSyntaxColor_Struct",				kColor_Yellow);
	CreateData_Color(kCSyntaxColor_Union,					"CSyntaxColor_Union",				kColor_Yellow);
	CreateData_Color(kCSyntaxColor_Define,					"CSyntaxColor_Define",				kColor_Gray);
	
	CreateData_Text(kRulerBaseLetter,						"RulerBaseLetter",					"a");
	CreateData_Text(kQuadClickText,							"QuadClickText",					"");
	CreateData_Text(kCommentStart,							"CommentStart",						"");
	CreateData_Text(kCommentEnd,							"CommentEnd",						"");
	CreateData_Text(kCommentLineStart,						"CommentLineStart",					"");
	CreateData_Text(kLiteralStart1,							"LiteralStart1",					"");
	CreateData_Text(kLiteralEnd1,							"LiteralEnd1",						"");
	CreateData_Text(kLiteralStart2,							"LiteralStart2",					"");
	CreateData_Text(kLiteralEnd2,							"LiteralEnd2",						"");
	CreateData_Text(kEscapeString,							"EscapeString",						"");
	CreateData_Text(kOtherStart1,							"OtherStart1",						"");
	CreateData_Text(kOtherEnd1,								"OtherEnd1",						"");
	CreateData_Text(kOtherStart2,							"OtherStart2",						"");
	CreateData_Text(kOtherEnd2,								"OtherEnd2",						"");
	CreateData_Text(kIncIndentRegExp,						"IncIndentRegExp",					"");//#972
	CreateData_Text(kDecIndentRegExp,						"DecIndentRegExp",					"");//#972
	
	CreateData_Text(kCharsForWord,							"CharsForWord",						"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
	CreateData_Text(kCharsForHeadOfWord,					"CharsForHeadOfWord",				"");
	CreateData_Text(kCharsForTailOfWord,					"CharsForTailOfWord",				"");
	
	CreateData_Text(kTeXCompiler,							"TeXCompiler",						"/usr/local/bin/platex");
	CreateData_Text(kTeXPreviewer,							"TeXPreviewer",						"");
	
	//win CreateData_Font(kDefaultFont,							"DefaultFont",						AFontWrapper::GetDefaultFont());
	CreateData_Text(kDefaultFontName,						"DefaultFontNameUni",				"Monaco");//#844 "default");//#375 defaultfont);
	CreateData_Text(kDefaultFontNameOld,					"DefaultFontName",					"Monaco");//#375 旧バージョン互換性のため。実際にはkDefaultFontNameを使う。
	CreateData_FloatNumber(kDefaultFontSize,				"DefaultFontSize",					9.0,			6.0,256.0);//B0398 最小値変更
	CreateData_Number(kDefaultTabWidth,						"DefaultTabWidth",					4,				1,128);
	CreateData_Number(kIndentWidth,							"IndentWidth",						4,				1,128);
	CreateData_Number(kDefaultWindowWidth,					"DefaultWindowWidth",				900,			50,5000);
	CreateData_Number(kDefaultWindowHeight,					"DefaultWindowHeight",				800,			50,5000);
	
	//win CreateData_Font(kDefaultPrintOption_PrintFont,					"DefaultPrintOption_PrintFont",			AFontWrapper::GetDefaultFont());
	CreateData_Text(kDefaultPrintOption_PrintFontName,				"DefaultPrintOption_PrintFontNameUni",	"default");//#375 defaultfont);
	CreateData_Text(kDefaultPrintOption_PrintFontNameOld,			"DefaultPrintOption_PrintFontName",		"Monaco");//#375 旧バージョン互換性のため。
	CreateData_FloatNumber(kDefaultPrintOption_PrintFontSize,		"DefaultPrintOption_PrintFontSize",		12.0,			3.0,256.0);
	//win CreateData_Font(kDefaultPrintOption_PageNumberFont,				"DefaultPrintOption_PageNumberFont",	AFontWrapper::GetDefaultFont());
	CreateData_Text(kDefaultPrintOption_PageNumberFontName,			"DefaultPrintOption_PageNumberFontNameUni","default");//#375 defaultfont);
	CreateData_Text(kDefaultPrintOption_PageNumberFontNameOld,		"DefaultPrintOption_PageNumberFontName","Monaco");//#375 旧バージョン互換性のため。
	CreateData_FloatNumber(kDefaultPrintOption_PageNumberFontSize,	"DefaultPrintOption_PageNumberFontSize",12.0,			3.0,256.0);
	//win CreateData_Font(kDefaultPrintOption_FileNameFont,				"DefaultPrintOption_FileNameFont",		AFontWrapper::GetDefaultFont());
	CreateData_Text(kDefaultPrintOption_FileNameFontName,			"DefaultPrintOption_FileNameFontNameUni","default");//#375 defaultfont);
	CreateData_Text(kDefaultPrintOption_FileNameFontNameOld,		"DefaultPrintOption_FileNameFontName",	"Monaco");//#375 旧バージョン互換性のため。
	CreateData_FloatNumber(kDefaultPrintOption_FileNameFontSize,	"DefaultPrintOption_FileNameFontSize",	12.0,			3.0,256.0);
	//win CreateData_Font(kDefaultPrintOption_LineNumberFont,				"DefaultPrintOption_LineNumberFont",	AFontWrapper::GetDefaultFont());
	CreateData_Text(kDefaultPrintOption_LineNumberFontName,			"DefaultPrintOption_LineNumberFontNameUni","default");//#375 defaultfont);
	CreateData_Text(kDefaultPrintOption_LineNumberFontNameOld,		"DefaultPrintOption_LineNumberFontName","Monaco");//#375 旧バージョン互換性のため。
	CreateData_Number(kDefaultPrintOption_LineMargin,				"DefaultPrintOption_LineMargin",		0,				0,64);
	
#if IMPLEMENTATION_FOR_MACOSX
	CreateData_Number(kLineMargin,							"LineMargin",						0,			-64,64);//B0446
#else
	CreateData_Number(kLineMargin,							"LineMargin",						1,			-64,64);//win
#endif
	
	CreateData_Number(kCIndent_Normal,						"CIndent_Normal",					4,				-128,128);
	CreateData_Number(kCIndent_Fukubun,						"CIndent_Fukubun",					8,				-128,128);
	CreateData_Number(kCIndent_Label,						"CIndent_Label",					-2,				-128,128);
	
	CreateData_Text(kDefaultTextEncoding,					"kDefaultTextEncoding",				"UTF-8");
	
	CreateData_Number(kDefaultWrapMode,						"DefaultWrapMode",					1,				0,4);//#1113
	CreateData_Number(kDefaultWrapMode_LetterCount,			"DefaultWrapMode_LetterCount",		80,				2,9999);
	
	CreateData_Number(kDefaultReturnCode,					"DefaultReturnCode",				2,				0,2);
	
	CreateData_Text(kBackgroundImageFile,					"BackgroundImageFile",				"");
	
	CreateData_Number(kIndentOnTheWay,						"IndentOnTheWay",					8,				-999,999);
	CreateData_Number(kIndentForLabel,						"IndentForLabel",					-2,				-999,999);
	CreateData_Bool(kIndentOnTheWaySecondIndent,			"IndentOnTheWaySecondIndent",		true);
	CreateData_Bool(kIndentOnTheWay_ParenthesisStartPos,	"IndentOnTheWay_ParenthesisStartPos",true);//#912
	CreateData_Bool(kIndentOnTheWay_SingleColon,			"IndentOnTheWay_SingleColon",		true);//#912
	CreateData_Bool(kUseSyntaxDefinitionIndent,				"UseSyntaxDefinitionIndent",		true);
	//B0392
	CreateData_Bool(kUseRegExpIndent,						"UseRegExpIndent",					false);
	//#912
	CreateData_Bool(kNoIndent,								"NoIndent",							false);
	
	CreateData_Text(kIndentExeLetters,						"IndentExeLetters",					"");//B0000初期値変更
	
	CreateData_Bool(kSetDefaultCreator,						"SetDefaultCreator",				true);//B0000初期値変更 #226
	CreateData_Text(kDefaultCreator,						"kDefaultCreator",					"MMKE");
	CreateData_Bool(kForceMiTextFile,						"ForceMiTextFile",					false);
	
	//R0199
	CreateData_Bool(kDisplaySpellCheck,						"DisplaySpellCheck",				false);
	CreateData_Number(kSpellCheckLanguage,					"SpellCheckLanguage",				0,				0,99999);
	
	//B0390
	CreateData_Bool(kSetDefaultTextEncodingWhenNotDecided,	"SetDefaultTextEncodingWhenNotDecided",	false);
	
	//RC2
	CreateData_Bool(kIdInfoWindowDisplayComment,			"IdInfoWindowDisplayComment",		true);
	CreateData_Bool(kDisplayAbbrevCandidateWindow,			"DisplayAbbrevCandidateWindow",		true);
	CreateData_Number(kHitCountForAbbrevCandidiateWindow,	"HitCountForAbbrevCandidiateWindow",3,				1,128);
	
	//B0230
	CreateData_Bool(kSaveTextEncodingToEA,					"SaveTextEncodingToEA",				true);
	
	//B0185
	CreateData_Bool(kDefaultDisplayCrossCaret,				"DefaultDisplayCrossCaret",			false);
	
	//R0210
	CreateTableStart();
	CreateData_TextArray(kModeSelectFilePath,				"ModeSelectFilePath",				"/");
	CreateTableEnd();
	
	//R0242
	CreateData_FloatNumber(kPrintMargin_Left,				"PrintMargin_Left",					10.0,				0,99999);
	CreateData_FloatNumber(kPrintMargin_Right,				"PrintMargin_Right",				10.0,				0,99999);
	CreateData_FloatNumber(kPrintMargin_Top,				"PrintMargin_Top",					10.0,				0,99999);
	CreateData_FloatNumber(kPrintMargin_Bottom,				"PrintMargin_Bottom",				10.0,				0,99999);
	
	//#91
	CreateData_Number(kDragDrop_ActionForNoSuffix,			"DragDrop_ActionForNoSuffix",		0,					0,2);//#329
	
	//#207
	CreateData_Bool(kUseDefaultSuffixForSave,				"UseDefaultSuffixForSave",			true);
	CreateData_Text(kDefaultSuffixForSaveText,				"DefaultSuffixForSaveText",			"");
	
	//#117
	CreateData_Bool(kStartSpaceToIndent,					"StartSpaceToIndent",				false);
	
	//#281
	CreateData_Bool(kIndentTabIsAlwaysTabIfIndentOff,		"IndentTabIsAlwaysTabIfIndentOff",	true);
	
	//#318
	CreateData_Bool(kZenkakuSpaceIndent,					"ZenkakuSpaceIndent",				false);
	
	//#332
	CreateData_Bool(kSaveDocumentPrefToResource,			"SaveDocumentPrefToResource",		false);
	CreateData_Bool(kSaveDocumentPrefToAppPref,				"SaveDocumentPrefToAppPref",		true);
	
	//#253
	CreateData_Bool(kImportSystemHeaderFiles,				"ImportSystemHeaderFiles",			false);
	CreateData_Text(kSystemHeaderFiles,						"SystemHeaderFiles",				"");
	
	//#349
	CreateData_Bool(kUseEmbeddedSDF,						"UseEmbeddedSDF",					true);
	CreateData_Bool(kUseUserDefinedSDF,						"UseUserDefinedSDF",				false);
	CreateData_Text(kUserDefinedSDFFilePath,						"UserDefinedSDFFilePath",	"");
	
	//#305 言語毎モード名
	CreateData_Text(kJapaneseName,							"JapaneseName",						"");
	CreateData_Text(kEnglishName,							"EnglishName",						"");
	CreateData_Text(kFrenchName,							"FrenchName",						"");
	CreateData_Text(kGermanName,							"GermanName",						"");
	CreateData_Text(kSpanishName,							"SpanishName",						"");
	CreateData_Text(kItalianName,							"ItalianName",						"");
	CreateData_Text(kDutchName,								"DutchName",						"");
	CreateData_Text(kSwedishName,							"SwedishName",						"");
	CreateData_Text(kNorwegianName,							"NorwegianName",					"");
	CreateData_Text(kDanishName,							"DanishName",						"");
	CreateData_Text(kFinnishName,							"FinnishName",						"");
	CreateData_Text(kPortugueseName,						"PortugueseName",					"");
	CreateData_Text(kSimplifiedChineseName,					"SimplifiedChineseName",			"");
	CreateData_Text(kTraditionalChineseName,				"TraditionalChineseName",			"");
	CreateData_Text(kKoreanName,							"KoreanName",						"");
	CreateData_Text(kPolishName,							"PolishName",						"");
	CreateData_Text(kPortuguesePortugalName,				"PortuguesePortugalName",			"");
	CreateData_Text(kRussianName,							"RussianName",						"");
	
	//#180
	CreateData_Text(kModeComment,							"ModeComment",						"");
	//#426
	CreateData_Text(kModeUpdateURL,							"ModeUpdateURL",					"");
	
	//#493
	CreateData_Number(kSDFHeaderDisplayMultiply,			"SDFHeaderDisplayMultiply",			100,			10,1000);
	
	//#467
	CreateData_Bool(kRecognizeConditionalCompileDirective,	"RecognizeConditionalCompileDirective",true);
	CreateData_Color(kConditionalCompileDisabledColor,		"ConditionalCompileDisabledColor",	kColor_LightGray);
	
	//#496
	CreateData_Bool(kShowUnderlineAtCurrentLine,			"ShowUnderlineAtCurrentLine",		false);
	CreateData_Color(kCurrentLineUnderlineColor,			"CurrentLineUnderlineColor",		kColor_LightGray);
	
	//#427
	CreateData_Text(kAutoUpdateRevisionText,				"AutoUpdateRevisionText",			"");
	CreateData_Text(kLastAutoUpdateDateTime,				"LastAutoUpdateDateTime",			"");
	
	//#427
	CreateData_Bool(kEnableAutoUpdateTool,					"EnableAutoUpdateTool",				true);
	CreateData_Bool(kEnableAutoUpdateToolbar,				"EnableAutoUpdateToolbar",			true);
	CreateData_Bool(kEnableAutoUpdateKeyword,				"EnableAutoUpdateKeyword",			true);
	CreateData_Bool(kEnableAutoUpdateJumpSetup,				"EnableAutoUpdateJumpSetup",		true);
	CreateData_Bool(kEnableUserTool,						"EnableUserTool",					true);
	CreateData_Bool(kEnableUserToolbar,						"EnableUserToolbar",				true);
	CreateData_Bool(kEnableUserKeyword,						"EnableUserKeyword",				true);
	CreateData_Bool(kEnableUserJumpSetup,					"EnableUserJumpSetup",				true);
	
	//#639
	CreateData_Bool(kIndent0ForEmptyLine,					"Indent0ForEmptyLine",				false);
	
	//#646
	CreateData_Bool(kIndentWhenPaste,						"IndentWhenPaste",					false);
	
	//#868
	CreateData_Bool(kEnableMode,							"EnableMode",						true);
	
	//#868
	CreateData_Bool(kSameAsNormalMode_Font,					"SameAsNormalMode_Font",			true);
	CreateData_Bool(kSameAsNormalMode_Colors,				"SameAsNormalMode_Colors",			true);
	CreateData_Bool(kSameAsNormalMode_TextDisplay,			"SameAsNormalMode_TextDisplay",		false);
	CreateData_Bool(kSameAsNormalMode_BackgroundDisplay,	"SameAsNormalMode_BackgroundDisplay",true);
	CreateData_Bool(kSameAsNormalMode_TextLayout,			"SameAsNormalMode_TextLayout",		true);
	CreateData_Bool(kSameAsNormalMode_TextProperty,			"SameAsNormalMode_TextProperty",	true);
	CreateData_Bool(kSameAsNormalMode_InputtSettings,		"SameAsNormalMode_InputtSettings",	true);
	CreateData_Bool(kSameAsNormalMode_SpellCheck,			"SameAsNormalMode_SpellCheck",		true);
	
	//#844
	CreateData_Color(kSyntaxColorSlot0_Color,				"SyntaxColorSlot0_Color",			AColorWrapper::GetColorByHTMLFormatColor("0000DD"));
	CreateData_Color(kSyntaxColorSlot1_Color,				"SyntaxColorSlot1_Color",			AColorWrapper::GetColorByHTMLFormatColor("008000"));
	CreateData_Color(kSyntaxColorSlot2_Color,				"SyntaxColorSlot2_Color",			AColorWrapper::GetColorByHTMLFormatColor("2080D0"));
	CreateData_Color(kSyntaxColorSlot3_Color,				"SyntaxColorSlot3_Color",			AColorWrapper::GetColorByHTMLFormatColor("2080D0"));
	CreateData_Color(kSyntaxColorSlot4_Color,				"SyntaxColorSlot4_Color",			AColorWrapper::GetColorByHTMLFormatColor("B09000"));
	CreateData_Color(kSyntaxColorSlot5_Color,				"SyntaxColorSlot5_Color",			AColorWrapper::GetColorByHTMLFormatColor("333333"));
	CreateData_Color(kSyntaxColorSlot6_Color,				"SyntaxColorSlot6_Color",			AColorWrapper::GetColorByHTMLFormatColor("B9A300"));
	CreateData_Color(kSyntaxColorSlot7_Color,				"SyntaxColorSlot7_Color",			AColorWrapper::GetColorByHTMLFormatColor("0000FF"));
	CreateData_Color(kSyntaxColorSlotComment_Color,			"SyntaxColorSlotComment_Color",		AColorWrapper::GetColorByHTMLFormatColor("FF0000"));
	CreateData_Color(kSyntaxColorSlotLiteral_Color,			"SyntaxColorSlotLiteral_Color",		AColorWrapper::GetColorByHTMLFormatColor("555555"));
	
	//#844
	CreateData_Bool(kSyntaxColorSlot0_Bold,					"SyntaxColorSlot0_Bold",			false);
	CreateData_Bool(kSyntaxColorSlot1_Bold,					"SyntaxColorSlot1_Bold",			false);
	CreateData_Bool(kSyntaxColorSlot2_Bold,					"SyntaxColorSlot2_Bold",			false);
	CreateData_Bool(kSyntaxColorSlot3_Bold,					"SyntaxColorSlot3_Bold",			false);
	CreateData_Bool(kSyntaxColorSlot4_Bold,					"SyntaxColorSlot4_Bold",			false);
	CreateData_Bool(kSyntaxColorSlot5_Bold,					"SyntaxColorSlot5_Bold",			false);
	CreateData_Bool(kSyntaxColorSlot6_Bold,					"SyntaxColorSlot6_Bold",			false);
	CreateData_Bool(kSyntaxColorSlot7_Bold,					"SyntaxColorSlot7_Bold",			false);
	CreateData_Bool(kSyntaxColorSlotComment_Bold,			"SyntaxColorSlotComment_Bold",		false);
	CreateData_Bool(kSyntaxColorSlotLiteral_Bold,			"SyntaxColorSlotLiteral_Bold",		false);
	
	//#844
	CreateData_Bool(kSyntaxColorSlot0_Italic,				"SyntaxColorSlot0_Italic",			false);
	CreateData_Bool(kSyntaxColorSlot1_Italic,				"SyntaxColorSlot1_Italic",			false);
	CreateData_Bool(kSyntaxColorSlot2_Italic,				"SyntaxColorSlot2_Italic",			false);
	CreateData_Bool(kSyntaxColorSlot3_Italic,				"SyntaxColorSlot3_Italic",			false);
	CreateData_Bool(kSyntaxColorSlot4_Italic,				"SyntaxColorSlot4_Italic",			false);
	CreateData_Bool(kSyntaxColorSlot5_Italic,				"SyntaxColorSlot5_Italic",			false);
	CreateData_Bool(kSyntaxColorSlot6_Italic,				"SyntaxColorSlot6_Italic",			false);
	CreateData_Bool(kSyntaxColorSlot7_Italic,				"SyntaxColorSlot7_Italic",			false);
	CreateData_Bool(kSyntaxColorSlotComment_Italic,			"SyntaxColorSlotComment_Italic",	false);
	CreateData_Bool(kSyntaxColorSlotLiteral_Italic,			"SyntaxColorSlotLiteral_Italic",	false);
	
	//#844
	CreateData_Bool(kSyntaxColorSlot0_Underline,			"SyntaxColorSlot0_Underline",		false);
	CreateData_Bool(kSyntaxColorSlot1_Underline,			"SyntaxColorSlot1_Underline",		false);
	CreateData_Bool(kSyntaxColorSlot2_Underline,			"SyntaxColorSlot2_Underline",		false);
	CreateData_Bool(kSyntaxColorSlot3_Underline,			"SyntaxColorSlot3_Underline",		false);
	CreateData_Bool(kSyntaxColorSlot4_Underline,			"SyntaxColorSlot4_Underline",		false);
	CreateData_Bool(kSyntaxColorSlot5_Underline,			"SyntaxColorSlot5_Underline",		false);
	CreateData_Bool(kSyntaxColorSlot6_Underline,			"SyntaxColorSlot6_Underline",		false);
	CreateData_Bool(kSyntaxColorSlot7_Underline,			"SyntaxColorSlot7_Underline",		false);
	CreateData_Bool(kSyntaxColorSlotComment_Underline,		"SyntaxColorSlotComment_Underline",	false);
	CreateData_Bool(kSyntaxColorSlotLiteral_Underline,		"SyntaxColorSlotLiteral_Underline",	false);
	
	//#450
	CreateData_Bool(kUseLineFolding,						"UseLineFolding",					true);
	CreateData_Bool(kUseDefaultFoldingSetting,				"UseDefaultFoldingSetting",			true);
	CreateData_Text(kDefaultFoldingLineText,				"DefaultFoldingLineText",			"#if 0");
	
	//#889
	CreateData_Text(kColorSchemeName,						"ColorSchemeName",					"");
	
	//#889
	CreateData_Bool(kDarkenImportLightenLocal,				"DarkenImportLightenLocal",			true);
	
	//
	CreateData_Color(kFindHighlightColor,					"FindHighlightColor",				AColorWrapper::GetColorByHTMLFormatColor("FDE900"));
	CreateData_Color(kFindHighlightColorPrev,				"FindHighlightColorPrev",			AColorWrapper::GetColorByHTMLFormatColor("E6E6FA"));
	CreateData_Color(kCurrentWordHighlightColor,			"CurrentWordHighlightColor",		AColorWrapper::GetColorByHTMLFormatColor("FFFF00"));
	CreateData_Color(kTextMarkerHightlightColor,			"TextMarkerHightlightColor",		AColorWrapper::GetColorByHTMLFormatColor("FF7C66"));
	CreateData_Color(kFirstSelectionColor,					"SelectionColorForFind",			AColorWrapper::GetColorByHTMLFormatColor("FFD700"));
	
	//#912
	CreateData_Bool(kIndentAutoDetect,						"IndentAutoDetect",					false);//#1341
	
	//#481
	CreateData_Bool(kKeyBindDefaultIsNormalMode,			"KeyBindDefaultIsNormalMode",		false);
	CreateData_Bool(kKeyBindDefaultIsOSKeyBind,				"KeyBindDefaultIsOSKeyBind",		false);
	
	//Webからの自動更新
	CreateData_Bool(kEnableModeUpdateFromWeb,				"EnableModeUpdateFromWeb",			false);
	
	//対応文字列入力にSDFを使う
	CreateData_Bool(kEnableSDFCorrespond,					"EnableSDFCorrespond",				true);
	
	//アプリ内組み込みモードの更新元モード
	CreateData_Text(kModeUpdateSourceModeName,				"ModeUpdateSourceModeName",			"");
	
	//アプリ内組み込みモードからの更新時の、アプリ更新日時記憶
	CreateData_Text(kAutoUpdateFromAppDateTimeText,			"AutoUpdateFromAppDateTimeText",	"");
	
	//diff表示不透明度
	CreateData_Number(kDiffColorOpacity,					"DiffColorOpacity",					30,				1,100);
	
	//選択色不透明度
	CreateData_Number(kSelectionOpacity,					"SelectionOpacity",					80,				1,100);
	
	//デフォルト文法パート
	CreateData_Text(kDefaultSyntaxPart,						"DefaultSyntaxPart",				"");
	
	//行頭バックスペースは左シフトにする
	CreateData_Bool(kShiftleftByBackspace,					"ShiftleftByBackspace",				true);
	
	//縦書きモード #558
	CreateData_Bool(kVerticalMode,							"VerticalMode",						false);
	
	//括弧ハイライト #1406
	CreateData_Bool(kHighlightBrace,						"HighlightBrace",					false);
	
	//Flexibleタブ #1421
	CreateData_Bool(kEnableFlexibleTabPositions,			"EnableFlexibleTabPositions",		false);
	CreateData_Text(kFlexibleTabPositions,					"FlexibleTabPositions",				"8,8,8,8,8,8,8,8,8,8");
	
	//プラグイン #994
	CreateTableStart();
	CreateData_TextArray(kPluginData_Id,					"PluginData_Id",					"");
	CreateData_TextArray(kPluginData_Name,					"PluginData_Name",					"");
	CreateData_TextArray(kPluginData_Options,				"PluginData_Options",				"");
	CreateData_BoolArray(kPluginData_Enabled,				"PluginData_Enabled",				false);
	CreateTableEnd();
	
	//プラグイン表示名 #994
	CreateTableStart();
	CreateData_TextArray(kPluginDisplayName_Id_Language,	"PluginDisplayName_Id_Language",	"");
	CreateData_TextArray(kPluginDisplayName_DisplayName,	"PluginDisplayName_DisplayName",	"");
	CreateTableEnd();
	
	//#
	//CreateData_Bool(997,									"testtest997",					true);
	//CreateData_TextArray(996,								"testtest996",					"996");
	//カテゴリー
	CreateTableStart();
	CreateData_TextArray(kAdditionalCategory_Name,			"AdditionalCategory_Name",			"category");
	CreateData_ColorArray(kAdditionalCategory_Color,		"AdditionalCategory_Color",			kColor_Blue);
	CreateData_BoolArray(kAdditionalCategory_Bold,			"AdditionalCategory_Bold",			false);
	CreateData_BoolArray(kAdditionalCategory_RegExp,		"AdditionalCategory_RegExp",		false);
	CreateData_BoolArray(kAdditionalCategory_IgnoreCase,	"AdditionalCategory_IgnoreCase",	false);
	CreateData_BoolArray(kAdditionalCategory_Underline,		"AdditionalCategory_Underline",		false);//R0195
	CreateData_BoolArray(kAdditionalCategory_Italic,		"AdditionalCategory_Italic",		false);//R0195
	CreateData_BoolArray(kAdditionalCategory_PriorToOtherColor,"AdditionalCategory_PriorToOtherColor",false);//R0195
	CreateData_BoolArray(kAdditionalCategory_Disabled,		"AdditionalCategory_Disabled",		false);//R0000
	CreateData_BoolArray(kAdditionalCategory_AutoUpdate,	"AdditionalCategory_AutoUpdate",	false);//#427
	CreateData_TextArray(kAdditionalCategory_Keywords,		"AdditionalCategory_Keywords",			"");//#427
	CreateData_BoolArray(kAdditionalCategory_UseCSV,		"AdditionalCategory_UseCSV",		false);//#791
	CreateData_TextArray(kAdditionalCategory_CSVPath,		"AdditionalCategory_CSVPath",		"");//#791
	CreateData_NumberArray(kAdditionalCategory_KeywordColumn,"AdditionalCategory_KeywordColumn",1,0,999);
	CreateData_NumberArray(kAdditionalCategory_ExplanationColumn,"AdditionalCategory_ExplanationColumn",2,0,999);
	CreateData_NumberArray(kAdditionalCategory_TypeTextColumn,"AdditionalCategory_TypeTextColumn",3,0,999);
	CreateData_NumberArray(kAdditionalCategory_ParentKeywordColumn,"AdditionalCategory_ParentKeywordColumn",4,0,999);
	CreateData_NumberArray(kAdditionalCategory_ColorSlotColumn,"AdditionalCategory_ColorTypeColumn",5,0,999);
	CreateData_NumberArray(kAdditionalCategory_CompletionTextColumn,"AdditionalCategory_CompletionTextColumn",6,0,999);
	CreateData_NumberArray(kAdditionalCategory_URLColumn,	"AdditionalCategory_URLColumn",7,0,999);
	CreateData_NumberArray(kAdditionalCategory_ColorSlot,	"AdditionalCategory_ColorSlot",		0,0,7);
	CreateData_TextArray(kAdditionalCategory_ValidSyntaxPart,"AdditionalCategory_ValidSyntaxPart","");
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_TextArray(kDragDrop_Suffix,					"DragDrop_Suffix",					".suffix");
	CreateData_TextArray(kDragDrop_Text,					"DragDrop_Text",					"<<<PATH>>>");
	CreateTableEnd();
	 
	CreateTableStart();
	CreateData_TextArray(kCorrespond_StartText,				"Correspond_StartText",				"start");
	CreateData_TextArray(kCorrespond_EndText,				"Correspond_EndText",				"end");
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_TextArray(kIndent_RegExp,					"Indent_RegExp",					"regexp");
	CreateData_NumberArray(kIndent_OffsetCurrentLine,		"Indent_OffsetCurrentLine",			0,				-128,128);//#695 ,false);
	CreateData_NumberArray(kIndent_OffsetNextLine,			"Indent_OffsetNextLine",			0,				-128,128);//#695 ,false);
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_TextArray(kJumpSetup_Name,					"JumpSetup_Name",					"name");
	CreateData_TextArray(kJumpSetup_RegExp,					"JumpSetup_RegExp",					"regexp");
	CreateData_TextArray(kJumpSetup_MenuText,				"JumpSetup_MenuText",				"$0");//B0000初期値変更
	CreateData_NumberArray(kJumpSetup_SelectionGroup,		"JumpSetup_SelectionGroup",			0,				0,255);//#695 ,false);
	CreateData_NumberArray(kJumpSetup_KeywordGroup,			"JumpSetup_KeywordGroup",			0,				0,255);//#695 ,false);
	CreateData_NumberArray(kJumpSetup_AnchorGroup,			"JumpSetup_AnchorGroup",			0,				0,255);//#695 ,false);//R0212
	CreateData_BoolArray(kJumpSetup_ColorizeLine,			"JumpSetup_ColorizeLine",			true);
	CreateData_ColorArray(kJumpSetup_LineColor,				"JumpSetup_LineColor",				kColor_DarkGreen);
	CreateData_ColorArray(kJumpSetup_KeywordColor,			"JumpSetup_KeywordColor",			kColor_Red);
	CreateData_BoolArray(kJumpSetup_RegisterToMenu,			"JumpSetup_RegisterToMenu",			false);
	CreateData_BoolArray(kJumpSetup_RegisterAsKeyword,		"JumpSetup_RegisterAsKeyword",		false);
	CreateData_NumberArray(kJumpSetup_DisplayMultiply,		"JumpSetup_DisplayMultiply",		100,			10,1000);//#695 ,false);//#493
	CreateData_BoolArray(kJumpSetup_AutoUpdate,				"JumpSetup_AutoUpdate",				false);//#427
	CreateData_BoolArray(kJumpSetup_ColorizeRegExpGroup,	"JumpSetup_ColorizeRegExpGroup",	false);//#603
	CreateData_NumberArray(kJumpSetup_ColorizeRegExpGroup_Number,"JumpSetup_ColorizeRegExpGroup_Number",0,		0,99);//#695 ,false);//#603
	CreateData_ColorArray(kJumpSetup_ColorizeRegExpGroup_Color,"JumpSetup_ColorizeRegExpGroup_Color",kColor_DarkGreen);//#603
	CreateData_NumberArray(kJumpSetup_OutlineLevel,			"JumpSetup_OutlineLevel",			1,		0,9);//#130
	CreateData_BoolArray(kJumpSetup_SimplerSetting,			"JumpSetup_SimplerSetting",			false);//#130
	CreateData_TextArray(kJumpSetup_LineStartText,			"JumpSetup_LineStartText",			"");//#130
	CreateData_BoolArray(kJumpSetup_RegisterToMenu2,		"JumpSetup_RegisterToMenu2",		true);//#130
	CreateData_NumberArray(kJumpSetup_KeywordColorSlot,		"JumpSetup_KeywordColorSlot",		0,	0,7);//
	CreateData_BoolArray(kJumpSetup_Enable,					"JumpSetup_Enable",					true);
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_TextArray(kSuffix,							"Suffix",							".suffix");
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_NumberArray(kKeyBind_Key,					"KeyBind_Key",						kKeyBindKey_A,	0,9999);//#695 ,false);
	CreateData_BoolArray(kKeyBind_ControlKey,				"KeyBind_ControlKey",				true);
	CreateData_BoolArray(kKeyBind_OptionKey,				"KeyBind_OptionKey",				false);
	CreateData_BoolArray(kKeyBind_CommandKey,				"KeyBind_CommandKey",				false);
	CreateData_BoolArray(kKeyBind_ShiftKey,					"KeyBind_ShiftKey",					false);
	CreateData_NumberArray(kKeyBind_Action,					"KeyBind_Action",					keyAction_NOP,	0,9999);//#695 ,false);
	CreateData_TextArray(kKeyBind_Text,						"KeyBind_Text",						"");
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_TextArray(kSyntaxDefinitionCategory_Name,			"SyntaxDefinitionCategory_Name",	"category");
	CreateData_ColorArray(kSyntaxDefinitionCategory_Color,			"SyntaxDefinitionCategory_Color",	kColor_Red);
	CreateData_ColorArray(kSyntaxDefinitionCategory_ImportColor,	"SyntaxDefinitionCategory_ImportColor",	kColor_Red);
	CreateData_ColorArray(kSyntaxDefinitionCategory_LocalColor,		"SyntaxDefinitionCategory_LocalColor",	kColor_Red);
	CreateData_BoolArray(kSyntaxDefinitionCategory_Bold,			"SyntaxDefinitionCategory_Bold",		false);//R0195
	CreateData_BoolArray(kSyntaxDefinitionCategory_Italic,			"SyntaxDefinitionCategory_Italic",		false);//R0195
	CreateData_BoolArray(kSyntaxDefinitionCategory_Underline,		"SyntaxDefinitionCategory_Underline",	false);//R0195
	CreateTableEnd();
	
	CreateTableStart();
	CreateData_TextArray(kSyntaxDefinitionState_Name,				"SyntaxDefinitionState_Name",	"state");
	CreateData_ColorArray(kSyntaxDefinitionState_Color,				"SyntaxDefinitionState_Color",	kColor_Red);
	CreateTableEnd();
	
}
//デストラクタ
AModePrefDB::~AModePrefDB()
{
	//SaveToFile();
}

//#427
/**
モードファイル名取得
*/
void	AModePrefDB::GetModeRawName( AText& outName ) const
{
	AText	modename;
	mModeFolder.GetFilename(outName);
}

/**
モードフォルダ取得
*/
void	AModePrefDB::GetModeFolder( AFileAcc& outFolder ) const
{
	outFolder = mModeFolder;
}

//#350
/**
dataフォルダ取得
*/
void	AModePrefDB::GetDataFolder( AFileAcc& outFolder ) const
{
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	outFolder.SpecifyChild(modefolder,"data");
}

//#350
/**
keywords.txt読み込み
*/
void	AModePrefDB::LoadLegacyKeywordTextIntoEmptyCatgory( const AText& inDataText, AModePrefDB& ioDB )//#427
{
	//カテゴリのうち、キーワードtextが空、かつ、autoupdateでないカテゴリを検索する #427
	ABoolArray	shouldbeAddedArray;
	for( AIndex categoryIndex = 0;
		 categoryIndex < ioDB.GetItemCount_Array(kAdditionalCategory_Name);
		 categoryIndex++ )
	{
		ABool	shouldbeAdded = false;
		if( ioDB.GetData_TextArrayRef(kAdditionalCategory_Keywords).GetTextConst(categoryIndex).GetItemCount() == 0 &&
			ioDB.GetData_BoolArray(kAdditionalCategory_AutoUpdate,categoryIndex) == false )
		{
			shouldbeAdded = true;
		}
		shouldbeAddedArray.Add(shouldbeAdded);
	}
	//
	AItemCount	itemCount = inDataText.GetItemCount();
	for( AIndex pos = 0; pos < itemCount;  )
	{
		for( ; pos < itemCount; pos++ )
		{
			if( inDataText.Get(pos) == '[' )
			{
				pos++;
				break;
			}
		}
		if( pos >= itemCount )   break;
		AIndex	categoryNameSpos = pos;
		for( ; pos < itemCount; pos++ )
		{
			if( inDataText.Get(pos) == ']' )
			{
				break;
			}
		}
		if( pos >= itemCount )   break;
		AText	categoryName;
		inDataText.GetText(categoryNameSpos,pos-categoryNameSpos,categoryName);
		pos++;
		if( inDataText.SkipTabSpace(pos,itemCount) == false )   break;
		AIndex	keywordSpos = pos;
		for( ; pos < itemCount; pos++ )
		{
			if( inDataText.Get(pos) == kUChar_CR )
			{
				break;
			}
		}
		AText	keyword;
		inDataText.GetText(keywordSpos,pos-keywordSpos,keyword);
		pos++;
		//#427 categoryNameが一致するカテゴリのshouldbeAddedArrayがtrueなら、キーワードを追加する。
		for( AIndex categoryIndex = 0; 
			 categoryIndex < ioDB.GetItemCount_Array(kAdditionalCategory_Name); 
			 categoryIndex++ )
		{
			if( ioDB.GetData_TextArrayRef(kAdditionalCategory_Name).
				GetTextConst(categoryIndex).Compare(categoryName) == true &&
				shouldbeAddedArray.Get(categoryIndex) == true )
			{
				AText	keywords;
				ioDB.GetData_TextArray(kAdditionalCategory_Keywords,categoryIndex,keywords);
				keywords.AddText(keyword);
				keywords.Add(kUChar_LineEnd);
				ioDB.SetData_TextArray(kAdditionalCategory_Keywords,categoryIndex,keywords);
			}
		}
	}
}

#ifndef MODEPREFCONVERTER
//modeprefconverterでは使用しない箇所 #1034
//#305
/**
モード多言語タイトル取得
*/
void	AModePrefDB::GetModeMultiLanguageName( const ALanguage inLanguage, AText& outName ) const
{
	//モード多言語タイトル取得
	switch(inLanguage)
	{
	  case kLanguage_Japanese:
		{
			GetData_Text(kJapaneseName,outName);
			break;
		}
	  case kLanguage_English:
		{
			GetData_Text(kEnglishName,outName);
			break;
		}
	  case kLanguage_French:
		{
			GetData_Text(kFrenchName,outName);
			break;
		}
	  case kLanguage_German:
		{
			GetData_Text(kGermanName,outName);
			break;
		}
	  case kLanguage_Spanish:
		{
			GetData_Text(kSpanishName,outName);
			break;
		}
	  case kLanguage_Italian:
		{
			GetData_Text(kItalianName,outName);
			break;
		}
	  case kLanguage_Dutch:
		{
			GetData_Text(kDutchName,outName);
			break;
		}
	  case kLanguage_Swedish:
		{
			GetData_Text(kSwedishName,outName);
			break;
		}
	  case kLanguage_Norwegian:
		{
			GetData_Text(kNorwegianName,outName);
			break;
		}
	  case kLanguage_Danish:
		{
			GetData_Text(kDanishName,outName);
			break;
		}
	  case kLanguage_Finnish:
		{
			GetData_Text(kFinnishName,outName);
			break;
		}
	  case kLanguage_Portuguese:
		{
			GetData_Text(kPortugueseName,outName);
			break;
		}
	  case kLanguage_SimplifiedChinese:
		{
			GetData_Text(kSimplifiedChineseName,outName);
			break;
		}
	  case kLanguage_TraditionalChinese:
		{
			GetData_Text(kTraditionalChineseName,outName);
			break;
		}
	  case kLanguage_Korean:
		{
			GetData_Text(kKoreanName,outName);
			break;
		}
	  case kLanguage_Polish:
		{
			GetData_Text(kPolishName,outName);
			break;
		}
	  case kLanguage_PortuguesePortugal:
		{
			GetData_Text(kPortuguesePortugalName,outName);
			break;
		}
	  case kLanguage_Russian:
		{
			GetData_Text(kRussianName,outName);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#305
/**
モード多言語タイトル設定
*/
void	AModePrefDB::SetModeMultiLanguageName( const ALanguage inLanguage, const AText& inName )
{
	//モード多言語タイトル設定
	switch(inLanguage)
	{
	  case kLanguage_Japanese:
		{
			SetData_Text(kJapaneseName,inName);
			break;
		}
	  case kLanguage_English:
		{
			SetData_Text(kEnglishName,inName);
			break;
		}
	  case kLanguage_French:
		{
			SetData_Text(kFrenchName,inName);
			break;
		}
	  case kLanguage_German:
		{
			SetData_Text(kGermanName,inName);
			break;
		}
	  case kLanguage_Spanish:
		{
			SetData_Text(kSpanishName,inName);
			break;
		}
	  case kLanguage_Italian:
		{
			SetData_Text(kItalianName,inName);
			break;
		}
	  case kLanguage_Dutch:
		{
			SetData_Text(kDutchName,inName);
			break;
		}
	  case kLanguage_Swedish:
		{
			SetData_Text(kSwedishName,inName);
			break;
		}
	  case kLanguage_Norwegian:
		{
			SetData_Text(kNorwegianName,inName);
			break;
		}
	  case kLanguage_Danish:
		{
			SetData_Text(kDanishName,inName);
			break;
		}
	  case kLanguage_Finnish:
		{
			SetData_Text(kFinnishName,inName);
			break;
		}
	  case kLanguage_Portuguese:
		{
			SetData_Text(kPortugueseName,inName);
			break;
		}
	  case kLanguage_SimplifiedChinese:
		{
			SetData_Text(kSimplifiedChineseName,inName);
			break;
		}
	  case kLanguage_TraditionalChinese:
		{
			SetData_Text(kTraditionalChineseName,inName);
			break;
		}
	  case kLanguage_Korean:
		{
			SetData_Text(kKoreanName,inName);
			break;
		}
	  case kLanguage_Polish:
		{
			SetData_Text(kPolishName,inName);
			break;
		}
	  case kLanguage_PortuguesePortugal:
		{
			SetData_Text(kPortuguesePortugalName,inName);
			break;
		}
	  case kLanguage_Russian:
		{
			SetData_Text(kRussianName,inName);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark ---初期化

//初期化
//オブジェクト作成時にAAppからコールされる
void	AModePrefDB::InitMode( const AModeIndex& inModeID, const AFileAcc& inModeFolder )
{
	mModeID = inModeID;
	//#920 mModeFile = inModeFile;
	mModeFolder = inModeFolder;//#920
	
	//#427 CreateData_XXX()はコンストラクタへ移動
	
	//#852 InitLoad();//R0000
	
	//#852
	//data/suffix.miがあれば、suffix.miから読み込む。（必要最小限のデータのみが格納されている）
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	AFileAcc	suffixPrefFile;
	suffixPrefFile.SpecifyChild(dataFolder,"suffix.mi");
	if( suffixPrefFile.Exist() == true )
	{
		//suffix.miから読み込み
		LoadFromPrefTextFile(suffixPrefFile);
	}
	else
	{
		//全データ読み込み
		LoadDB();
		//suffix.miを保存（次回起動時の高速化のため） #1275 高速化。suffix.miから読み込まなかった場合だけ保存するように修正。
		SaveSuffixDB();
	}
	//suffixハッシュを更新
	UpdateSuffixHash();
}

//R0000
/**
ロード（常に必要なデータのロード。起動時に読み込まれる）
*/
void	AModePrefDB::LoadDB()//#852
{
	//#852
	//全データ削除
	DeleteAllModeData();
	
	// ------------ ModePreferences.mi読み込み ------------
	
	//dataフォルダ取得
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	//バージョン3用設定ファイル
	AFileAcc	preffile;
	preffile.SpecifyChild(dataFolder,"ModePreferences.mi");
	//バージョン2.1用設定ファイル
	AFileAcc	v2prefFile;
	v2prefFile.SpecifyChild(dataFolder,"modepref.mi");
	if( preffile.Exist() == true )
	{
		//==================バージョン3用設定ファイルから読み込み==================
		
		//#1419
		// ------------ モードdefaultデータから読み込み ------------
		//miアップグレード後に、ユーザーデータに存在しないデータを、モードdefaultデータから読み込むようにするため。
		
		//「コピー元モード名」設定を取得するために一旦ユーザーデータを読み込む（GetAutoUpdateFolder()で「コピー元モード名」設定データが必要）
		LoadFromPrefTextFile(preffile);
		
		//「コピー元モード」からデータをロードする
		AFileAcc	defaultFolder;
		GetAutoUpdateFolder(defaultFolder);
		AFileAcc	defaultPrefFile;
		defaultPrefFile.SpecifyChild(defaultFolder,"data/ModePreferences.mi");
		LoadFromPrefTextFile(defaultPrefFile);
		
		// ------------ ユーザーデータから読み込み ------------
		LoadFromPrefTextFile(preffile);
	}
	else if( v2prefFile.Exist() == true )
	{
		//==================バージョン2.1.11～2.1.13用設定ファイルから読み込み==================
		//data/modepref.miがあれば読み込み（2.1.11以降）
		//（この場合、MacLegacy(2.1.8a7以前)も、MMKE(2.1.8～2.1.10)も読み込まない。高速化のため。）
		LoadFromPrefTextFile(v2prefFile);
		//keywords.txtから読み込み #427
		//2.1.13b1よりキーワードはmodeprefDB内に格納するようにした。
		//カテゴリのキーワードデータ(kAdditionalCategory_Keywords)が空のカテゴリのみ、
		//ここでkeywords.txtからkAdditionalCategory_Keywordsへキーワード読み込みする。
		//（ここではユーザー作成キーワードのみ。自動更新のほうは自動更新modepref.mi読み込み時
		//つまり、下のautoupdateDB.LoadFromPrefTextFile()実行直後に、keywords.txt読み込む。）
		AFileAcc	keywordTextFile;
		keywordTextFile.SpecifyChild(dataFolder,"keywords.txt");
		AText	keywordText;
		keywordTextFile.ReadTo(keywordText);
		if( keywordText.GetItemCount() > 0 )
		{
			//LoadLegacyKeywordTextIntoEmptyCatgory()は空のカテゴリに対してのみ、keyword.txtからキーワードを読み込む
			LoadLegacyKeywordTextIntoEmptyCatgory(keywordText,*this);
		}
	}
	else
	{
		/*#1034
		//==================バージョン2.1.10以前用設定ファイルから読み込み==================
#if IMPLEMENTATION_FOR_MACOSX
		//------------------旧形式データ読み込み(2.1.8a7以前)------------------
		try
		{
			LoadMacLegacy();
		}
		catch(...)
		{
			//ファイルの破壊等によるエラー
			_ACError("AModePrefDB::LoadDB() caught exception.",NULL);//#199
		}
		//LoadMacLegacy()によって読み込んだ2.1.8a7以前形式キーワードをmodeprefDBへコピーする#427
		for( AIndex categoryIndex = 0; 
					categoryIndex < mLegacyKeywordTempDataArray.GetItemCount();
					categoryIndex++ )
		{
			AText	text;
			mLegacyKeywordTempDataArray.GetObjectConst(categoryIndex).Implode(kUChar_LineEnd,text);
			SetData_TextArray(kAdditionalCategory_Keywords,categoryIndex,text);
		}
		
		//------------------旧形式データ読み込み(2.1.8～2.1.10)------------------
		//LoadFromPrefFile()は、ファイルが存在していれば、旧データは削除する。（上記で読んだキーワードも全て削除）
		AText	modeRawName;
		GetModeRawName(modeRawName);
		AFileAcc	legacyModeFile;
		legacyModeFile.SpecifyChild(mModeFolder,modeRawName);
		if( legacyModeFile.Exist() == true )
		{
			LoadFromPrefFile(legacyModeFile,'MMKE');
			//MMKE-2のキーワードデータ読み込み #427
			AText	keywordText;
			legacyModeFile.ReadResouceTo(keywordText,'MMKE',2);
			if( keywordText.GetItemCount() > 0 )
			{
				//LoadLegacyKeywordTextIntoEmptyCatgory()は空のカテゴリに対してのみ、keyword.txtからキーワードを読み込む
				LoadLegacyKeywordTextIntoEmptyCatgory(keywordText,*this);
			}
		}
#endif
		*/
		/*#1355 Xcode10以降で32bitビルドができなくなった＆Catalinaで32bit起動できなくなったので、リソースフォーク形式のモードデータのコンバートは対応しない。
		//#1034
		//旧形式データをversion3形式にconvertする
		//（modeprefconverterを実行）
		ATextArray	argArray;
		AText	command;
		//modeprefconverterコマンドのパスを取得
		AFileAcc	appFile;
		AFileWrapper::GetAppFile(appFile);
		AFileAcc	converterFile;
		converterFile.SpecifyChild(appFile,"Contents/MacOS/mi_modeprefconverter");
		converterFile.GetNormalizedPath(command);
		argArray.Add(command);
		//モードフォルダのパスを取得
		AFileAcc	modeFolder;
		GetModeFolder(modeFolder);
		AText	modefolderPath;
		modeFolder.GetNormalizedPath(modefolderPath);
		argArray.Add(modefolderPath);
		//実行
		AText	resulttext;
		GetApp().SPI_GetChildProcessManager().ExecuteGeneralSyncCommand(command,argArray,modefolderPath,resulttext);
		//ロード
		LoadFromPrefTextFile(preffile);
		*/
	}
	
	//B0000
	CorrectTable_MakeColumnUnique(kSyntaxDefinitionCategory_Name,kSyntaxDefinitionCategory_Name);
	
	//#868
	//標準モードの場合は、「標準モードと同じ」設定は全てfalse
	if( mModeID == kStandardModeIndex )
	{
		SetData_Bool(kSameAsNormalMode_Font,false);
		SetData_Bool(kSameAsNormalMode_Colors,false);
		SetData_Bool(kSameAsNormalMode_TextDisplay,false);
		SetData_Bool(kSameAsNormalMode_BackgroundDisplay,false);
		SetData_Bool(kSameAsNormalMode_TextLayout,false);
		SetData_Bool(kSameAsNormalMode_TextProperty,false);
		SetData_Bool(kSameAsNormalMode_InputtSettings,false);
		SetData_Bool(kSameAsNormalMode_SpellCheck,false);
	}
	
	//R0210 
	UpdateSuffixHash();
	
	/*#1275 アプリ内のモードファイルからコピーするのではなく、アプリ内のモードファイルを直接参照するようにする。
	アプリ更新後最初だけ実行されるだけだが、HTMLモードの場合けっこう時間かかる（5秒程度）ので、改善する。
	//==================アプリケーション内のモードファイルからの自動更新==================
	if( GetData_Bool(AModePrefDB::kEnableModeUpdateFromWeb) == false )
	{
		//現在起動しているアプリの更新日時取得
		AText	thisAppModifiedDateTimeText;
		GetApp().SPI_GetAppModifiedDataTimeText(thisAppModifiedDateTimeText);
		//モードデータに記憶されている最後のアプリの更新日時取得
		AText	lastUpdateFromAppDateTimeText;
		GetData_Text(kAutoUpdateFromAppDateTimeText,lastUpdateFromAppDateTimeText);
		//上の２つが違う場合は、アプリ内組み込みdefaultフォルダからコピー
		if( thisAppModifiedDateTimeText.Compare(lastUpdateFromAppDateTimeText) == false )
		{
			//モードdefaultフォルダを取得
			AFileAcc	appModeRootFolder;
			GetApp().SPI_GetModeDefaultFolder(appModeRootFolder);
			//コピー元モード名を取得
			AText	modename;
			GetData_Text(AModePrefDB::kModeUpdateSourceModeName,modename);
			if( modename.GetItemCount() == 0 )
			{
				//コピー元モード名が空の場合（＝メニューの最初の項目が選択されていた場合）、同じモード名のモードをコピー元にする
				GetModeRawName(modename);
			}
			//コピー元フォルダ取得
			AFileAcc	appModeFolder;
			appModeFolder.SpecifyChild(appModeRootFolder,modename);
			//コピー元フォルダが存在していれば、それをautoupdateフォルダにコピーする
			if( appModeFolder.Exist() == true )
			{
				CopyToAutoUpdateFolder(appModeFolder);
			}
			//コピー元フォルダが存在していなければ、autoupdateフォルダは削除する
			else
			{
				AFileAcc	dstAutoupdateFolder;
				GetAutoUpdateFolder(dstAutoupdateFolder);
				dstAutoupdateFolder.DeleteFileOrFolderRecursive();
			}
			//今回のアプリの更新日時を記憶
			SetData_Text(kAutoUpdateFromAppDateTimeText,thisAppModifiedDateTimeText);
		}
	}
	*/
	
	//#427 
	//==================自動更新データ読み込み==================
	//------------------autoupdate/data/ModePreferences.miファイルからautoupdateDBへ読み込み------------------
	AFileAcc	autoUpdateFolder;
	GetAutoUpdateFolder(autoUpdateFolder);
	AFileAcc	autoUpdateModePrefFile;
	autoUpdateModePrefFile.SpecifyChild(autoUpdateFolder,"data/ModePreferences.mi");
	if( autoUpdateModePrefFile.Exist() == true )
	{
		//自動更新ModePrefDB取得
		AModePrefDB	autoupdateDB(this);
		autoupdateDB.LoadFromPrefTextFile(autoUpdateModePrefFile);
		//------------------autoupdate_userdata/ModePreferences.miからautoupdateUserDataDBへ読み込み------------------
		//自動更新ModePrefDB userdata取得 #624
		AFileAcc	autoUpdateUserDataModePrefFile;
		GetAutoUpdateUserDataFolder("ModePreferences.mi",autoUpdateUserDataModePrefFile);
		ABool	userDataDBExist = autoUpdateUserDataModePrefFile.Exist();
		AModePrefDB	autoupdateUserDataDB(this);
		if( userDataDBExist == true )
		{
			autoupdateUserDataDB.LoadFromPrefTextFile(autoUpdateUserDataModePrefFile);
		}
		/*#844
		//keywords.txt読み込み
		AFileAcc	keywordTextFile;
		keywordTextFile.SpecifyChild(autoUpdateFolder,"data/keywords.txt");
		AText	keywordText;
		keywordTextFile.ReadTo(keywordText);
		if( keywordText.GetItemCount() > 0 )
		{
			//LoadLegacyKeywordTextIntoEmptyCatgory()は空のカテゴリに対してのみ、keyword.txtからキーワードを読み込む
			LoadLegacyKeywordTextIntoEmptyCatgory(keywordText,autoupdateDB);
		}
		*/
		
		//見出しデータマージ
		//#844 MergeAutoUpdate_JumpSetup(autoupdateDB,userDataDBExist,autoupdateUserDataDB);//#624
		
		//キーワードカテゴリーデータマージ
		MergeAutoUpdate_KeywordCategory(autoupdateDB,userDataDBExist,autoupdateUserDataDB);//#624
	}
	
	//========================キーワード説明ファイル読み込み #427========================
	LoadKeywordsExplanation();
	
	//==================データ補正==================
	//使用出来ないフォントの場合はdefaultフォントに補正する
	AdjustFontName(kDefaultFontName);
	
	//インデントタイプ設定の補正（どれか１つだけをtrueにする。2.1では処理的にSDF＞RegExp＞前行 という優先順だったので、それに合わせる #912
	if( GetData_Bool(kUseSyntaxDefinitionIndent) == true )
	{
		SetData_Bool(kUseRegExpIndent,false);
		SetData_Bool(kNoIndent,false);
	}
	else if( GetData_Bool(kUseRegExpIndent) == true )
	{
		SetData_Bool(kUseSyntaxDefinitionIndent,false);
		SetData_Bool(kNoIndent,false);
	}
	else if( GetData_Bool(kNoIndent) == true )
	{
		SetData_Bool(kUseSyntaxDefinitionIndent,false);
		SetData_Bool(kUseRegExpIndent,false);
	}
	
	//#1142
	//制御コード色未設定の場合（バージョン3.0.0b9以前のデータの場合）は、背景色に文字色α30%でブレンドした色を設定
	if( IsStillDefault(kControlCodeColor) == true )
	{
		AColor	backgroundColor = kColor_White;
		GetData_Color(kBackgroundColor,backgroundColor);
		AColor	letterColor = kColor_Black;
		GetData_Color(kLetterColor,letterColor);
		AColor	controlCodeColor = AColorWrapper::GetAlpheBlend(backgroundColor,letterColor,0.3);
		SetData_Color(kControlCodeColor,controlCodeColor);
	}
	
	//
	//C言語文法認識を旧データから変換
	//kUseBuiltinSyntaxDefinitionFileが未設定（ロードファイルに項目無し）で、kUseCSyntaxColorがtrueならC++ビルトイン文法定義ファイルを設定
	if( IsStillDefault(kUseBuiltinSyntaxDefinitionFile) == true && GetData_Bool(kUseCSyntaxColor) == true )
	{
		//#349 SetData_Bool(kUseSyntaxDefinitionFile,true);
		SetData_Bool(kUseBuiltinSyntaxDefinitionFile,true);
		AText	name;
		name.SetCstring("C/C++");
		SetData_Text(kBuiltinSyntaxDefinitionName,name);
	}
	if( IsStillDefault(kIndentOnTheWay) == true )
	{
		SetData_Number(kIndentOnTheWay,GetData_Number(kCIndent_Fukubun));
	}
	if( IsStillDefault(kIndentForLabel) == true )
	{
		SetData_Number(kIndentForLabel,GetData_Number(kCIndent_Label));
	}
	if( IsStillDefault(kIndentWidth) == true )
	{
		SetData_Number(kIndentWidth,GetData_Number(kCIndent_Normal));
	}
	
	//2.1.8a7～2.1.10のバージョンのフォント名読み込み
	//それより前はフォント番号なので、v3では引き継ぎ対応しない。
	//#375
	if( IsStillDefault(kDefaultFontName) == true && IsStillDefault(kDefaultFontNameOld) == false )
	{
		AText	fontname;
		GetData_Text(kDefaultFontNameOld,fontname);
		SetData_Text(kDefaultFontName,fontname);
	}
	
	//Flexibleタブ位置設定parse #1421
	UpdateFlexibleTabPositions();
	
	//==================
	//modepref.mi/keywords.txt読み込み完了フラグON
	mDBLoaded = true;//#852
}

//#427
/**
見出し設定についてロードした自動更新項目とユーザー作成項目をマージする
*/
/*#844
void	AModePrefDB::MergeAutoUpdate_JumpSetup( AModePrefDB& inAutoUpdateDB, 
			const ABool inAutoUpdateUserDataDBExist, const AModePrefDB& inAutoUpdateUserDataDB )
{
	// ========== AutoUpdateUserDataDBからAutoUpdateDBへ、ユーザーが設定した色データをコピーする ========== 
	if( inAutoUpdateUserDataDBExist == true )
	{
		for( AIndex autoUpdateDBIndex = 0; 
					autoUpdateDBIndex < inAutoUpdateDB.GetItemCount_Array(kJumpSetup_Name); 
					autoUpdateDBIndex++ )
		{
			//UserDataDB側から設定名が同じ、かつ、AutoUpdateフラグON項目を検索
			AText	name;
			inAutoUpdateDB.GetData_TextArray(kJumpSetup_Name,autoUpdateDBIndex,name);
			AIndex	foundThisDBIndex = kIndex_Invalid;
			for( AIndex thisDBIndex = 0; 
						thisDBIndex < inAutoUpdateUserDataDB.GetItemCount_Array(kJumpSetup_Name); //#624
						thisDBIndex++ )
			{
				if( inAutoUpdateUserDataDB.GetData_BoolArray(kJumpSetup_AutoUpdate,thisDBIndex) == true )//#624
				{
					AText	text;
					inAutoUpdateUserDataDB.GetData_TextArray(kJumpSetup_Name,thisDBIndex,text);//#624
					if( name.Compare(text) == true )
					{
						foundThisDBIndex = thisDBIndex;
						break;
					}
				}
			}
			if( foundThisDBIndex != kIndex_Invalid )
			{
				//設定名が同じ項目へ色データだけを上書き
				AColor	color;
				inAutoUpdateUserDataDB.GetData_ColorArray(kJumpSetup_LineColor,foundThisDBIndex,color);//#624
				inAutoUpdateDB.SetData_ColorArray(kJumpSetup_LineColor,autoUpdateDBIndex,color);
				inAutoUpdateUserDataDB.GetData_ColorArray(kJumpSetup_KeywordColor,foundThisDBIndex,color);//#624
				inAutoUpdateDB.SetData_ColorArray(kJumpSetup_KeywordColor,autoUpdateDBIndex,color);
				inAutoUpdateUserDataDB.GetData_ColorArray(kJumpSetup_ColorizeRegExpGroup_Color,foundThisDBIndex,color);//#603 #624
				inAutoUpdateDB.SetData_ColorArray(kJumpSetup_ColorizeRegExpGroup_Color,autoUpdateDBIndex,color);//#603
			}
		}
	}
	
	// ========== 本DBのほうのAutoUpdateフラグ付きの項目を全削除する ========== 
	for( AIndex thisDBIndex = GetItemCount_Array(kJumpSetup_Name)-1; 
				thisDBIndex >= 0; 
				thisDBIndex-- )
	{
		if( GetData_BoolArray(kJumpSetup_AutoUpdate,thisDBIndex) == true )
		{
			//AutoUpdateフラグONなら削除する
			DeleteRow_Table(kJumpSetup_Name,thisDBIndex);
		}
	}
	
	// ========== キーワードの有効無効切り替えがEnableの場合のみ、AutoUpdate項目を本DBへコピーする ========== 
	if( GetData_Bool(AModePrefDB::kEnableAutoUpdateJumpSetup) == true )
	{
		//AutoUpdateDBから、本DBへデータコピーし、AutoUpdateフラグをONにする
		for( AIndex autoUpdateDBIndex = 0; 
					autoUpdateDBIndex < inAutoUpdateDB.GetItemCount_Array(kJumpSetup_Name); 
					autoUpdateDBIndex++ )
		{
			AIndex	thisDBIndex = autoUpdateDBIndex;//AutoUpdateDBと同じindexに挿入
			//本DBへ項目追加（AutoUpdateDBからコピー）
			InsertRowFromOtherDB_Table(kJumpSetup_Name,thisDBIndex,inAutoUpdateDB,autoUpdateDBIndex);
			//AutoUpdateフラグON
			SetData_BoolArray(kJumpSetup_AutoUpdate,thisDBIndex,true);
		}
	}
}
*/

//#427
/**
キーワードについてロードした自動更新項目とユーザー作成項目をマージする
*/
void	AModePrefDB::MergeAutoUpdate_KeywordCategory( AModePrefDB& inAutoUpdateDB,
			const ABool inAutoUpdateUserDataDBExist, const AModePrefDB& inAutoUpdateUserDataDB )//#624
{
	// ========== AutoUpdateUserDataDBからAutoUpdateDBへ、ユーザーが設定した色データをコピーする ========== 
	if( inAutoUpdateUserDataDBExist == true )
	{
		//AutoUpdateDB内の各カテゴリ毎のループ
		for( AIndex autoUpdateDBIndex = 0; 
					autoUpdateDBIndex < inAutoUpdateDB.GetItemCount_Array(kAdditionalCategory_Name); 
					autoUpdateDBIndex++ )
		{
			//UserDataDB側から設定名が同じ、かつ、AutoUpdateフラグON項目を検索
			AText	name;
			inAutoUpdateDB.GetData_TextArray(kAdditionalCategory_Name,autoUpdateDBIndex,name);
			AIndex	foundUserDBIndex = kIndex_Invalid;
			for( AIndex userDBIndex = 0; 
						userDBIndex < inAutoUpdateUserDataDB.GetItemCount_Array(kAdditionalCategory_Name); //#624
						userDBIndex++ )
			{
				if( inAutoUpdateUserDataDB.GetData_BoolArray(kAdditionalCategory_AutoUpdate,userDBIndex) == true )//#624
				{
					AText	text;
					inAutoUpdateUserDataDB.GetData_TextArray(kAdditionalCategory_Name,userDBIndex,text);//#624
					if( name.Compare(text) == true )
					{
						foundUserDBIndex = userDBIndex;
						break;
					}
				}
			}
			if( foundUserDBIndex != kIndex_Invalid )
			{
				/*#844 
				//設定名が同じ項目へ色データ等だけを上書き
				AColor	color;
				inAutoUpdateUserDataDB.GetData_ColorArray(kAdditionalCategory_Color,foundThisDBIndex,color);//#624
				inAutoUpdateDB.SetData_ColorArray(kAdditionalCategory_Color,autoUpdateDBIndex,color);
				inAutoUpdateDB.SetData_BoolArray(kAdditionalCategory_Bold,autoUpdateDBIndex,
							inAutoUpdateUserDataDB.GetData_BoolArray(kAdditionalCategory_Bold,foundThisDBIndex));//#624
				inAutoUpdateDB.SetData_BoolArray(kAdditionalCategory_Underline,autoUpdateDBIndex,
							inAutoUpdateUserDataDB.GetData_BoolArray(kAdditionalCategory_Underline,foundThisDBIndex));//#624
				inAutoUpdateDB.SetData_BoolArray(kAdditionalCategory_Italic,autoUpdateDBIndex,
							inAutoUpdateUserDataDB.GetData_BoolArray(kAdditionalCategory_Italic,foundThisDBIndex));//#624
							*/
				inAutoUpdateDB.SetData_BoolArray(kAdditionalCategory_Disabled,autoUpdateDBIndex,
							inAutoUpdateUserDataDB.GetData_BoolArray(kAdditionalCategory_Disabled,foundUserDBIndex));//#624
			}
		}
	}
	
	// ========== 本DBのほうのAutoUpdateフラグ付きの項目を全削除する ========== 
	for( AIndex thisDBIndex = GetItemCount_Array(kAdditionalCategory_Name)-1; 
				thisDBIndex >= 0; 
				thisDBIndex-- )
	{
		if( GetData_BoolArray(kAdditionalCategory_AutoUpdate,thisDBIndex) == true )
		{
			//AutoUpdateフラグONなら削除する
			DeleteRow_Table(kAdditionalCategory_Name,thisDBIndex);
		}
	}
	
	// ========== AutoUpdate項目を本DBへコピーする ========== 
	//#844 if( GetData_Bool(AModePrefDB::kEnableAutoUpdateKeyword) == true )
	{
		//AutoUpdateDBから、本DBへデータコピーし、AutoUpdateフラグをONにする
		for( AIndex autoUpdateDBIndex = 0; 
					autoUpdateDBIndex < inAutoUpdateDB.GetItemCount_Array(kAdditionalCategory_Name); 
					autoUpdateDBIndex++ )
		{
			AIndex	thisDBIndex = autoUpdateDBIndex;//AutoUpdateDBと同じindexに挿入
			//本DBへ項目追加（AutoUpdateDBからコピー）
			InsertRowFromOtherDB_Table(kAdditionalCategory_Name,thisDBIndex,inAutoUpdateDB,autoUpdateDBIndex);
			//AutoUpdateフラグON
			SetData_BoolArray(kAdditionalCategory_AutoUpdate,thisDBIndex,true);
		}
	}
}

#pragma mark ===========================

#pragma mark ---ロード／保存

/**
ロード（使用時に必要なデータのロード）
@note inInitialLoadはSPI_GetModePrefDB() or InitModePref()以外からのコール（＝リロード）はfalseにする。また、falseの場合、スレッドセーフではない。
inInitialLoad=true：初回ロード用。この場合、ImportFileRecognizerスレッド等の停止処理は行わない。スレッドからもコール可。
inInitialLoad=false：リロード用。ImportFileRecognizerスレッド等の停止処理を行ってから、ロード処理を行う。スレッドからのコールは不可。
*/
void	AModePrefDB::LoadOrWaitLoadComplete( const ABool inInitialLoad )
{
	//スレッド停止処理
	if( inInitialLoad == false )
	{
		//==================リロード時用処理（スレッドセーフでない）==================
		
		//#725
		//各サブウインドウのモード関連データをクリアする
		GetApp().SPI_ClearSubWindowsModeRelatedData(mModeID);
		
		//モードが一致するドキュメント内のローカル識別子データを削除
		for( AObjectID docID = GetApp().NVI_GetFirstDocumentID(kDocumentType_Text); 
					docID != kObjectID_Invalid; 
					docID = GetApp().NVI_GetNextDocumentID(docID) )
		{
			if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == mModeID )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_DeleteAllLocalIdentifiers();
			}
		}
		
		//ImportFileRecognizerスレッド停止
		GetApp().SPI_GetImportFileManager().ClearRecognizedDataAndStopRecognizer(mModeID);
		
		//文法認識スレッド停止
		GetApp().SPI_StopSyntaxRecognizerThreadAll(mModeID);
		
		//MultiFileFinderの文法認識停止
		GetApp().SPI_GetMultiFileFinder().SPI_AbortRecognizeSyntax();
	}
	
	{
		//初回ロード時用の排他制御。下の説明を参照。（リロードは、スレッドからはコールされない）
		AStMutexLocker	locker(mLoadMutex);//#890
		
		//この関数を実行中に、別スレッドからmode dataへのアクセスが発生した場合、（＝初回ロードが同時に発生した場合）
		//１．ロード済みフラグOFFとなっているので、その別スレッドからもこの関数がコールされる。
		//２．mLoadMutexにより排他制御されるので、ロード完了まで上のlockerで待たされる。
		//３．この判定によりロード済みと判定されるので、すぐにリターンする。
		if( mLoaded == true && inInitialLoad == true )
		{
			return;
		}
		
		//#852
		//DBのロード
		LoadDB();
		
		/*#844
		//#332
		if( IsStillDefault(kSaveDocumentPrefToResource) == true )
		{
			SetData_Bool(kSaveDocumentPrefToResource,!GetData_Bool(kDontSaveResouceFork));
		}
		*/
		
		/*#844
#if IMPLEMENTATION_FOR_MACOSX
		//2.1.10以前のバージョンからのデータ引き継ぎ用
		//#375
		if( IsStillDefault(kDefaultPrintOption_PrintFontName) == true && IsStillDefault(kDefaultPrintOption_PrintFontNameOld) == false )
		{
		AText	fontname;
		GetData_Text(kDefaultPrintOption_PrintFontNameOld,fontname);
		SetData_Text(kDefaultPrintOption_PrintFontName,fontname);
		}
		//#375
		if( IsStillDefault(kDefaultPrintOption_PageNumberFontName) == true && IsStillDefault(kDefaultPrintOption_PageNumberFontNameOld) == false )
		{
		AText	fontname;
		GetData_Text(kDefaultPrintOption_PageNumberFontNameOld,fontname);
		SetData_Text(kDefaultPrintOption_PageNumberFontName,fontname);
		}
		//#375
		if( IsStillDefault(kDefaultPrintOption_FileNameFontName) == true && IsStillDefault(kDefaultPrintOption_FileNameFontNameOld) == false )
		{
		AText	fontname;
		GetData_Text(kDefaultPrintOption_FileNameFontNameOld,fontname);
		SetData_Text(kDefaultPrintOption_FileNameFontName,fontname);
		}
		//#375
		if( IsStillDefault(kDefaultPrintOption_LineNumberFontName) == true && IsStillDefault(kDefaultPrintOption_LineNumberFontNameOld) == false )
		{
		AText	fontname;
		GetData_Text(kDefaultPrintOption_LineNumberFontNameOld,fontname);
		SetData_Text(kDefaultPrintOption_LineNumberFontName,fontname);
		}
		
		//2.1.8a6以前のバージョンからのデータ引き継ぎ用
		//win
		if( IsStillDefault(kDefaultFontName) == true && IsStillDefault(kDefaultFont_compat) == false )
		{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kDefaultFont_compat),fontname) == true )
		{
		SetData_Text(kDefaultFontName,fontname);
		}
		}
		//win
		if( IsStillDefault(kDefaultPrintOption_PrintFontName) == true && IsStillDefault(kDefaultPrintOption_PrintFont_compat) == false )
		{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kDefaultPrintOption_PrintFont_compat),fontname) == true )
		{
		SetData_Text(kDefaultPrintOption_PrintFontName,fontname);
		}
		}
		//win
		if( IsStillDefault(kDefaultPrintOption_PageNumberFontName) == true && IsStillDefault(kDefaultPrintOption_PageNumberFont_compat) == false )
		{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kDefaultPrintOption_PageNumberFont_compat),fontname) == true )
		{
		SetData_Text(kDefaultPrintOption_PageNumberFontName,fontname);
		}
		}
		//win
		if( IsStillDefault(kDefaultPrintOption_FileNameFontName) == true && IsStillDefault(kDefaultPrintOption_FileNameFont_compat) == false )
		{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kDefaultPrintOption_FileNameFont_compat),fontname) == true )
		{
		SetData_Text(kDefaultPrintOption_FileNameFontName,fontname);
		}
		}
		//win
		if( IsStillDefault(kDefaultPrintOption_LineNumberFontName) == true && IsStillDefault(kDefaultPrintOption_LineNumberFont_compat) == false )
		{
		AText	fontname;
		if( AFontWrapper::GetFontName(GetData_Number(kDefaultPrintOption_LineNumberFont_compat),fontname) == true )
		{
		SetData_Text(kDefaultPrintOption_LineNumberFontName,fontname);
		}
		}
#endif
		*/
		/*#844
		//#207 保存時デフォルト拡張子のデフォルト設定
		if( IsStillDefault(kDefaultSuffixForSaveText) == true )
		{
			if( GetItemCount_Array(kSuffix) > 0 )
			{
				//最初の拡張子取得
				AText	text;
				GetData_TextArray(kSuffix,0,text);
				//設定
				SetData_Text(kDefaultSuffixForSaveText,text);
			}
		}
		*/
		
		//========================各データの更新========================
		
		//カラースキームDBの読み込み
		//#1316 UpdateColorScheme();
		
		//アルファベットテーブル更新
		UpdateIsAlphabetTable();
		
		//見出しデータ更新（モード設定画面変更時にから直接データ書き込みしないように、文法認識処理は変数を使って行っている） #890
		UpdateJumpSetup();
		
		//文法定義設定
		UpdateSyntaxDefinition();
		
		//キーワードリスト作成
		MakeCategoryArrayAndKeywordList();
		
		//文法認識色テーブル更新
		UpdateSyntaxDefinitionStateColorArray();
		
		//#382 デフォルトキーバインドと同じキーバインド設定を削除する
		//#912 RemoveKeyBindsSameAsDefault();
		
		//背景画像読み込み
		UpdateBackgroundImage();
		
		//ロード済みフラグON
		mLoaded = true;
	}
	
	//========================システムヘッダ読み込み======================== #253
	UpdateSystemHeaderFileData();
	
	//========================ツール読み込み========================
	LoadTool();
	
	//========================ツールバー読み込み========================
	LoadToolbar();
	
	//==================プラグイン読み込み==================
	LoadPlugins();
	
	//==================プレビュープラグイン読み込み==================
	LoadPreviewPlugin();
	
	if( inInitialLoad == false )
	{
		//==================リロード時用処理（スレッドセーフでない）==================
		
		//#683 各ドキュメントのTextInfo内のRegExpオブジェクトを更新する
		//（初期ロード時は、まだTextInfoにSetModeしたドキュメントがないので、コールする必要が無い）
		GetApp().SPI_UpdateRegExpForAllDocuments(mModeID);
		
		//インポートファイルの認識を再開
		GetApp().SPI_GetImportFileManager().RestartRecognizer(mModeID);
		
		//このモードのドキュメントの文法認識を再度行う
		GetApp().SPI_ReRecognizeSyntaxAll(mModeID);
	}
}

/**
文法定義の更新
*/
void	AModePrefDB::UpdateSyntaxDefinition()
{
	//文法定義オブジェクトのデータを全削除
	mSyntaxDefinition.DeleteAll();
	
	//SDFファイルから設定
	if( /*R0000 GetData_Bool(kUseSyntaxDefinitionFile) == true ||*/ GetData_Bool(kUseBuiltinSyntaxDefinitionFile) == true ||
	   GetData_Bool(kUseUserDefinedSDF) == true )
	{
		AFileAcc	sdfFile;
		//if( GetData_Bool(kUseBuiltinSyntaxDefinitionFile) == true )
		//#844 if( GetData_Bool(kUseEmbeddedSDF) == true )//#349
		if( GetData_Bool(kUseUserDefinedSDF) == false )//#844
		{
			AText	name;
			GetData_Text(kBuiltinSyntaxDefinitionName,name);
			//if( name.Compare("C/C++") == true )
			/*R0000{
				AFileAcc	appFile;
				AFileWrapper::GetAppFile(appFile);
				sdfFile.SpecifyChild(appFile,"Contents/Resources/cpp_jp.sdf");
			}*/
			GetApp().SPI_GetBuiltinSDFFileByName(name,sdfFile);
		}
		//#349
		else
		{
			AText	path;
			GetData_Text(kUserDefinedSDFFilePath,path);
			//絶対パスでトライ
			sdfFile.Specify(path);
			if( sdfFile.Exist() == false )
			{
				//autoupdateフォルダからの相対パスでトライ#427
				AFileAcc	autoupdateFolder;
				GetAutoUpdateFolder(autoupdateFolder);
				sdfFile.SpecifyChild(autoupdateFolder,path);
				if( sdfFile.Exist() == false )
				{
					//モードフォルダからの相対パスでトライ
					AFileAcc	folder;
					GetModeFolder(folder);
					sdfFile.SpecifyChild(folder,path);
				}
			}
		}
		if( sdfFile.Exist() == true )//#349
		{
			if( mSyntaxDefinition.Parse(sdfFile) == false )
			{
				//#349 fprintf(stderr,"parse error");
			}
			//#907
			mSDFIsSimpleDefinition = false;
		}
		else
		{
			//SDFファイルが存在しない場合、ダミーでsyntaxDefを作成
			AText	t;
			mSyntaxDefinition.MakeSimpleDefinition(t,t,t,t,t,t,t,t,false,t,t);
			//#907
			mSDFIsSimpleDefinition = true;
		}
	}
	//簡易文法設定から変換して設定
	else
	{
		//comment
		AText	commentStart, commentEnd;
		GetData_Text(kCommentStart,commentStart);
		GetData_Text(kCommentEnd,commentEnd);
		AText	lineCommentStart;
		GetData_Text(kCommentLineStart,lineCommentStart);
		/*#844
		AColor	commentColor;
		GetData_Color(kCommentColor,commentColor);
		*/
		//literal
		AText	literal1Start, literal1End, literal2Start, literal2End;
		GetData_Text(kLiteralStart1,literal1Start);
		GetData_Text(kLiteralEnd1,literal1End);
		GetData_Text(kLiteralStart2,literal2Start);
		GetData_Text(kLiteralEnd2,literal2End);
		/*#844
		AColor	literalColor;
		GetData_Color(kLiteralColor,literalColor);
		*/
		//other
		/* AText	other1Start, other1End, other2Start, other2End;
		GetData_Text(kOtherStart1,other1Start);
		GetData_Text(kOtherEnd1,other1End);
		GetData_Text(kOtherStart2,other2Start);
		GetData_Text(kOtherEnd2,other2End);*/
		AText	escape;
		GetData_Text(kEscapeString,escape);
		//ブロック開始、終了正規表現取得 #972
		AText	incIndentRegExp, decIndentRegExp;
		GetData_Text(kIncIndentRegExp,incIndentRegExp);
		GetData_Text(kDecIndentRegExp,decIndentRegExp);
		//文法定義設定
		mSyntaxDefinition.MakeSimpleDefinition(commentStart,commentEnd,lineCommentStart,
											   literal1Start,literal1End,literal2Start,literal2End,
											   /* other1Start,other1End,other2Start,other2End,*/
											   /*#844 commentColor,literalColor,*/
											   escape,
											   GetData_Bool(kUseRegExpCommentStart),
											   incIndentRegExp, decIndentRegExp );//#972
		//#907
		mSDFIsSimpleDefinition = true;
	}
	
	//SDFにて自動インデントdisableの場合は、関連設定を強制的にfalseにする #997 Pythonの場合に自動インデントを禁止するため（Pythonは手動インデント前提なので自動インデントすると使いづらい）
	if( GetDisableAutoIndent() == true )
	{
		SetData_Bool(kUseSyntaxDefinitionIndent,false);
		SetData_Bool(kIndentWhenPaste,false);
	}
}

//#253
/**
システムヘッダファイル
*/
void	AModePrefDB::UpdateSystemHeaderFileData()
{
	//システムヘッダファイルデータ全削除
	DeleteAllImportFileData();
	//システムヘッダファイル設定に記述された各フォルダを読み込み
	if( GetData_Bool(AModePrefDB::kImportSystemHeaderFiles) == true )
	{
		AText	text;
		GetData_Text(AModePrefDB::kSystemHeaderFiles,text);
		//キーワードファイル(.import)で指定したヘッダファイルフォルダを追加 #1065
		text.Add(kUChar_LF);
		text.AddText(mImportFilePathsByKeywordCategory);
		//
		for( AIndex pos = 0; pos < text.GetItemCount();  )
		{
			AText	line;
			text.GetLine(pos,line);
			AFileAcc	folder;
			folder.Specify(line);
			folder.ResolveAlias();//エイリアス解決 #1065
			if( folder.Exist() == true )//#1065
			{
				if( folder.IsFolder() == true )//★サブフォルダは検索していない？
				{
					AObjectArray<AFileAcc>	children;
					folder.GetChildren(children);
					for( AIndex i = 0; i < children.GetItemCount(); i++ )
					{
						AFileAcc	file;
						file = children.GetObjectConst(i);
						AText	filename;
						file.GetFilename(filename);
						if( IsThisModeSuffix(filename) == true )
						{
							mSystemHeaderFileArray.GetObject(mSystemHeaderFileArray.AddNewObject()).CopyFrom(file);
							//インポートしたファイルのパスを記憶
							AText	path;
							file.GetNormalizedPath(path);
							mSystemHeaderFileArray_NormalizedPath.Add(path);
						}
					}
				}
				else
				{
					mSystemHeaderFileArray.GetObject(mSystemHeaderFileArray.AddNewObject()).CopyFrom(folder);
					//インポートしたファイルのパスを記憶
					AText	path;
					folder.GetNormalizedPath(path);
					mSystemHeaderFileArray_NormalizedPath.Add(path);
				}
			}
		}
	}
	{
		//mutex lock #717
		AStMutexLocker	locker(mSystemHeaderIdentifierLinkListMutex);
		
		//
		//#423 mSystemHeaderIdentifierLinkList.DeleteAll();
		if( mSystemHeaderFileArray.GetItemCount() > 0 )
		{
			//システムヘッダファイルへのリンクデータ取得（未解析があれば解析要求）
			GetApp().SPI_GetImportFileManager().GetImportIdentifierLinkList(kObjectID_Invalid,
						mSystemHeaderFileArray,mModeID,mSystemHeaderIdentifierLinkList,true);
		}
		else //#423
		{
			mSystemHeaderIdentifierLinkList.DeleteAll();
		}
	}
}

//#253
/**
ヘッダファイルの解析完了通知。AImportFileManagerから通知される。
*/
void	AModePrefDB::DoImportFileRecognized()
{
	UpdateSystemHeaderFileData();
}

//#253
/**
指定ヘッダファイルに対応する識別しリンクデータを全て削除する。
ヘッダファイル保存時にAImportFileManager::DoFileSaved()からコールされる。
*/
ABool	AModePrefDB::DeleteImportFileData( const AObjectID inImportFileDataObjectID, const AFileAcc& inFile )
{
	//mutex lock #717
	AStMutexLocker	locker(mSystemHeaderIdentifierLinkListMutex);
	
	//inImportFileDataObjectIDに対応する識別子リンクデータを削除
	return mSystemHeaderIdentifierLinkList.Delete(inImportFileDataObjectID);
}

//#253
/**
*/
void	AModePrefDB::DeleteAllImportFileData()
{
	//mutex lock #717
	AStMutexLocker	locker(mSystemHeaderIdentifierLinkListMutex);
	
	mSystemHeaderFileArray.DeleteAll();
	mSystemHeaderIdentifierLinkList.DeleteAll();
	mSystemHeaderFileArray_NormalizedPath.DeleteAll();
}

//背景画像読み込み
void	AModePrefDB::UpdateBackgroundImage()
{
	AText	backgroundImagePath;
	GetModeData_Text(kBackgroundImageFile,backgroundImagePath);
	AFileAcc	backgroundImageFile;
	backgroundImageFile.SpecifyWithAnyFilePathType(backgroundImagePath);//B0389 OK,kFilePathType_1);
	mBackgroundImageIndex = CWindowImp::RegisterBackground(backgroundImageFile);
}

ABool	AModePrefDB::IsLoaded()
{
	return mLoaded;
}

/**
ファイルへ保存（対象：ModePreferences.mi）
*/
void	AModePrefDB::SaveToFile()
{
	//#427 if( IsLoaded() == false )   return;
	// ModePreferences.miの読み込み済みチェック（未読み込み保存防止） #427
	if( mDBLoaded == false )//#852
	{
		_ACThrow("mode SaveToFile() before loaded.",this);
	}
	
	// --------------- modepref.miの保存 ---------------
	/*#688
#if IMPLEMENTATION_FOR_MACOSX
	//win
	//旧データへコピー
	AText	text;
	AFontNumber	font;
	//
	GetData_Text(kDefaultFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kDefaultFont_compat,font);
	//
	GetData_Text(kDefaultPrintOption_PrintFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kDefaultPrintOption_PrintFont_compat,font);
	//
	GetData_Text(kDefaultPrintOption_PageNumberFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kDefaultPrintOption_PageNumberFont_compat,font);
	//
	GetData_Text(kDefaultPrintOption_FileNameFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kDefaultPrintOption_FileNameFont_compat,font);
	//
	GetData_Text(kDefaultPrintOption_LineNumberFontName,text);
	AFontWrapper::GetFontByName(text,font);
	SetData_Number(kDefaultPrintOption_LineNumberFont_compat,font);
#endif
	*/
	//R0068
	//#844 NormalizeAdditionalCategoryName();
	
	/*#350
	//ファイル書き込み
#if IMPLEMENTATION_FOR_MACOSX
	WriteToPrefFile(mModeFile,'MMKE');
#else
	WriteToPrefTextFile(mModeFile);
#endif
	*/
	
	//#624
	// ========== UserDataDBとして保存 ========== 
	//自動更新項目も含めて全データをそのままautoupdate_userdataフォルダに保存。
	AFileAcc	modeFolder;
	GetModeFolder(modeFolder);
	AFileAcc	autoupdateUserDataFolder;
	autoupdateUserDataFolder.SpecifyChild(modeFolder,"autoupdate_userdata");
	autoupdateUserDataFolder.CreateFolderRecursive();
	AFileAcc	userDataDBFile;
	userDataDBFile.SpecifyChild(autoupdateUserDataFolder,"ModePreferences.mi");
	userDataDBFile.CreateFile();
	WriteToPrefTextFile(userDataDBFile);
	
	/*#844
	// ========== JumpSetupのAutoUpdateフラグ付きの項目を全削除する ========== 
	for( AIndex thisDBIndex = GetItemCount_Array(kJumpSetup_Name)-1; 
				thisDBIndex >= 0; 
				thisDBIndex-- )
	{
		if( GetData_BoolArray(kJumpSetup_AutoUpdate,thisDBIndex) == true )
		{
			//AutoUpdateフラグONなら削除する
			DeleteRow_Table(kJumpSetup_Name,thisDBIndex);
		}
	}
	*/
	// ========== KeywordCategoryのAutoUpdateフラグ付きの項目を全削除する ========== 
	for( AIndex thisDBIndex = GetItemCount_Array(kAdditionalCategory_Name)-1; 
				thisDBIndex >= 0; 
				thisDBIndex-- )
	{
		if( GetData_BoolArray(kAdditionalCategory_AutoUpdate,thisDBIndex) == true )
		{
			//AutoUpdateフラグONなら削除する
			DeleteRow_Table(kAdditionalCategory_Name,thisDBIndex);
		}
	}
	
	// ========== dataフォルダにModePrefDBを保存 ========== 
	// JumpSetup, Keywordの自動更新項目は削除されている。
	
	//dataフォルダ取得・生成
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	dataFolder.CreateFolder();
	
	//ModePref.mi書き込み
	AFileAcc	prefFile;
	prefFile.SpecifyChild(dataFolder,"ModePreferences.mi");
	prefFile.CreateFile();
	WriteToPrefTextFile(prefFile);
	
	//#624
	// ========== UserDataDBから読み込み（上で削除した分の復元のため） ========== 
	LoadFromPrefTextFile(userDataDBFile);
	
	//#844
#if 0
	// --------------- keywords.txtの保存 ---------------
	
	/*#350
	//キーワード書き込み（暫定）
	WriteAdditionalKeywords();
	*/
	//キーワード設定データ作成
	AText	keywordDataText;
	MakeUserKeywordDataText(keywordDataText);
	//Keyword.txt書き込み
	AFileAcc	keywordFile;
	keywordFile.SpecifyChild(dataFolder,"keywords.txt");
	keywordFile.CreateFile();
	keywordFile.WriteFile(keywordDataText);
	
#if IMPLEMENTATION_FOR_MACOSX
	//旧形式データ書き込み
	WriteToPrefFile(mModeFile,'MMKE');//#350
	mModeFile.WriteResouceFile(keywordDataText,'MMKE',2);//#350
	//WriteMacLegacy()用にmLegacyKeywordTempDataArrayを作成 #427
	mLegacyKeywordTempDataArray.DeleteAll();
	for( AIndex categoryIndex = 0; 
				categoryIndex < GetItemCount_Array(kAdditionalCategory_Keywords);
				categoryIndex++ )
	{
		//
		AText	text;
		GetData_TextArray(kAdditionalCategory_Keywords,categoryIndex,text);
		mLegacyKeywordTempDataArray.AddNewObject();
		mLegacyKeywordTempDataArray.GetObject(categoryIndex).ExplodeFromText(text,kUChar_LineEnd);
	}
	//
	WriteMacLegacy();
#endif
#endif
	
	//#852
	//suffix.mi保存
	SaveSuffixDB();
}

//#852
/**
suffix.mi保存。起動高速化のため、必要最小限のデータを保存。起動時の読み込みはこのファイルを読み込む（全てのモードで）。
*/
void	AModePrefDB::SaveSuffixDB()
{
	ADataBase	suffixDB;
	//データ生成
	suffixDB.CreateData_Text(kJapaneseName,							"JapaneseName",						"");
	suffixDB.CreateData_Text(kEnglishName,							"EnglishName",						"");
	suffixDB.CreateData_Text(kFrenchName,							"FrenchName",						"");
	suffixDB.CreateData_Text(kGermanName,							"GermanName",						"");
	suffixDB.CreateData_Text(kSpanishName,							"SpanishName",						"");
	suffixDB.CreateData_Text(kItalianName,							"ItalianName",						"");
	suffixDB.CreateData_Text(kDutchName,							"DutchName",						"");
	suffixDB.CreateData_Text(kSwedishName,							"SwedishName",						"");
	suffixDB.CreateData_Text(kNorwegianName,						"NorwegianName",					"");
	suffixDB.CreateData_Text(kDanishName,							"DanishName",						"");
	suffixDB.CreateData_Text(kFinnishName,							"FinnishName",						"");
	suffixDB.CreateData_Text(kPortugueseName,						"PortugueseName",					"");
	suffixDB.CreateData_Text(kSimplifiedChineseName,				"SimplifiedChineseName",			"");
	suffixDB.CreateData_Text(kTraditionalChineseName,				"TraditionalChineseName",			"");
	suffixDB.CreateData_Text(kKoreanName,							"KoreanName",						"");
	suffixDB.CreateData_Text(kPolishName,							"PolishName",						"");
	suffixDB.CreateData_Text(kPortuguesePortugalName,				"PortuguesePortugalName",			"");
	suffixDB.CreateData_Text(kRussianName,							"RussianName",						"");
	suffixDB.CreateTableStart();
	suffixDB.CreateData_TextArray(kSuffix,							"Suffix",							".suffix");
	suffixDB.CreateTableEnd();
	suffixDB.CreateTableStart();
	suffixDB.CreateData_TextArray(kModeSelectFilePath,				"ModeSelectFilePath",				"/");
	suffixDB.CreateTableEnd();
	suffixDB.CreateData_Bool(kEnableMode,							"EnableMode",						true);
	//データをコピー
	suffixDB.CopyDataFromOtherDB(kJapaneseName,*this,kJapaneseName);
	suffixDB.CopyDataFromOtherDB(kEnglishName,*this,kEnglishName);
	suffixDB.CopyDataFromOtherDB(kFrenchName,*this,kFrenchName);
	suffixDB.CopyDataFromOtherDB(kGermanName,*this,kGermanName);
	suffixDB.CopyDataFromOtherDB(kSpanishName,*this,kSpanishName);
	suffixDB.CopyDataFromOtherDB(kItalianName,*this,kItalianName);
	suffixDB.CopyDataFromOtherDB(kDutchName,*this,kDutchName);
	suffixDB.CopyDataFromOtherDB(kSwedishName,*this,kSwedishName);
	suffixDB.CopyDataFromOtherDB(kNorwegianName,*this,kNorwegianName);
	suffixDB.CopyDataFromOtherDB(kDanishName,*this,kDanishName);
	suffixDB.CopyDataFromOtherDB(kFinnishName,*this,kFinnishName);
	suffixDB.CopyDataFromOtherDB(kPortugueseName,*this,kPortugueseName);
	suffixDB.CopyDataFromOtherDB(kSimplifiedChineseName,*this,kSimplifiedChineseName);
	suffixDB.CopyDataFromOtherDB(kTraditionalChineseName,*this,kTraditionalChineseName);
	suffixDB.CopyDataFromOtherDB(kKoreanName,*this,kKoreanName);
	suffixDB.CopyDataFromOtherDB(kPolishName,*this,kPolishName);
	suffixDB.CopyDataFromOtherDB(kPortuguesePortugalName,*this,kPortuguesePortugalName);
	suffixDB.CopyDataFromOtherDB(kRussianName,*this,kRussianName);
	suffixDB.CopyDataFromOtherDB(kSuffix,*this,kSuffix);
	suffixDB.CopyDataFromOtherDB(kModeSelectFilePath,*this,kModeSelectFilePath);
	suffixDB.CopyDataFromOtherDB(kEnableMode,*this,kEnableMode);
	//suffix.miファイル
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	AFileAcc	suffixPrefFile;
	suffixPrefFile.SpecifyChild(dataFolder,"suffix.mi");
	suffixPrefFile.CreateFile();
	//書き込み
	suffixDB.WriteToPrefTextFile(suffixPrefFile);
}

/*#350
//MMKE,2にキーワード書き込み
//InfoText対応するには、モード設定ダイアログから変更する必要があるので、暫定策
void	AModePrefDB::WriteAdditionalKeywords()
{
#if IMPLEMENTATION_FOR_MACOSX
	AText	text;
	for( AIndex i = 0; i < mAdditionalCategoryArray_KeywordArray.GetItemCount(); i++ )
	{
		AText	categoryName;
		GetData_TextArray(kAdditionalCategory_Name,i,categoryName);
		//R0068 kAdditionalCategory_Nameのほうを変換しなければ意味なしcategoryName.ReplaceChar('[','(');
		//R0068 kAdditionalCategory_Nameのほうを変換しなければ意味なしcategoryName.ReplaceChar(']',')');
		AItemCount	keywordCount = mAdditionalCategoryArray_KeywordArray.GetObject(i).GetItemCount();
		for( AIndex j = 0; j < keywordCount; j++ )
		{
			text.AddCstring("[");
			text.AddText(categoryName);
			text.AddCstring("]");
			text.AddText(mAdditionalCategoryArray_KeywordArray.GetObject(i).GetTextConst(j));
			text.Add(kUChar_CR);
		}
	}
	mModeFile.WriteResouceFile(text,'MMKE',2);
#endif
}
*/

//#350
/**
キーワード保存データ作成
*/
/*#844
void	AModePrefDB::MakeUserKeywordDataText( AText& outDataText ) const
{
	outDataText.DeleteAll();
	for( AIndex categoryIndex = 0; 
				categoryIndex < GetItemCount_Array(kAdditionalCategory_Name);//#427 mAdditionalCategoryArray_KeywordArray.GetItemCount(); 
				categoryIndex++ )
	{
		//#427 自動更新キーワードはkeyword.txtに書き出さない（＝ユーザー編集分のみ作成する）
		if( GetData_BoolArray(kAdditionalCategory_AutoUpdate,categoryIndex) == true )
		{
			continue;
		}
		//#427 キーワードテキストをtextarrayに展開
		ATextArray	keywordArray;
		keywordArray.ExplodeFromText(GetData_ConstTextArrayRef(kAdditionalCategory_Keywords).GetTextConst(categoryIndex),
					kUChar_LineEnd);
		//
		AText	categoryName;
		GetData_TextArray(kAdditionalCategory_Name,categoryIndex,categoryName);
		//#427 AItemCount	keywordCount = mAdditionalCategoryArray_KeywordArray.GetObjectConst(i).GetItemCount();
		AItemCount	keywordCount = keywordArray.GetItemCount();//#427
		for( AIndex j = 0; j < keywordCount; j++ )
		{
			if( keywordArray.GetTextConst(j).GetItemCount() == 0 )   continue;//#427
			outDataText.AddCstring("[");
			outDataText.AddText(categoryName);
			outDataText.AddCstring("]");
			//#427 outDataText.AddText(mAdditionalCategoryArray_KeywordArray.GetObjectConst(i).GetTextConst(j));
			outDataText.AddText(keywordArray.GetTextConst(j));//#427
			outDataText.Add(kUChar_CR);
		}
	}
}
*/

/*#350
//MMKE,2からキーワード読み込み
//暫定
void	AModePrefDB::LoadAdditionalKeywords()
{
#if IMPLEMENTATION_FOR_MACOSX
	AText	text;
	if( mModeFile.ReadResouceTo(text,'MMKE',2) == false )   return;
	mAdditionalCategoryArray_KeywordArray.DeleteAll();
	for( AIndex i = 0; i < GetItemCount_Array(kAdditionalCategory_Name); i++ )
	{
		mAdditionalCategoryArray_KeywordArray.AddNewObject();
	}
	AItemCount	itemCount = text.GetItemCount();
	for( AIndex pos = 0; pos < itemCount;  )
	{
		for( ; pos < itemCount; pos++ )
		{
			if( text.Get(pos) == '[' )
			{
				pos++;
				break;
			}
		}
		if( pos >= itemCount )   break;
		AIndex	categoryNameSpos = pos;
		for( ; pos < itemCount; pos++ )
		{
			if( text.Get(pos) == ']' )
			{
				break;
			}
		}
		if( pos >= itemCount )   break;
		AText	categoryName;
		text.GetText(categoryNameSpos,pos-categoryNameSpos,categoryName);
		pos++;
		if( text.SkipTabSpace(pos,itemCount) == false )   break;
		AIndex	keywordSpos = pos;
		for( ; pos < itemCount; pos++ )
		{
			if( text.Get(pos) == kUChar_CR )
			{
				break;
			}
		}
		AText	keyword;
		text.GetText(keywordSpos,pos-keywordSpos,keyword);
		pos++;
		AIndex	categoryIndex = Find_TextArray(kAdditionalCategory_Name,categoryName,false);
		if( categoryIndex != kIndex_Invalid )
		{
			mAdditionalCategoryArray_KeywordArray.GetObject(categoryIndex).Add(keyword);
		}
	}
#endif
}
*/

/*#350
void	AModePrefDB::GetXMLFile( AText& outText ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	mModeFile.ReadResouceTo(outText,'MMKE',1);
#endif
}
*/

/*#427 ModeBackup()に統一
//R0000
void	AModePrefDB::SaveModeBackup()
{
#if IMPLEMENTATION_FOR_MACOSX
	//現在のデータを全てファイル保存
	SaveToFile();
	//
	AFileAcc	apppreffolder;
	AFileWrapper::GetAppPrefFolder(apppreffolder);
	AFileAcc	backupfolder;
	backupfolder.SpecifyChild(apppreffolder,"modebackup");
	backupfolder.CreateFolder();
	AText	filename("modebackup_");
	AText	modeName;
	GetModeName(modeName);
	filename.AddText(modeName);
	AText	datetimetext;
	ADateTimeWrapper::GetDateTimeTextForFileName(datetimetext);
	filename.AddText(datetimetext);
	filename.AddCstring(".tgz");
	AFileAcc	file;
	file.SpecifyUniqChildFile(backupfolder,filename);
	AText	path;
	file.GetPath(path);
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	AText	modefoldername;
	modefolder.GetFilename(modefoldername);
	AFileAcc	rootModeFolder;
	rootModeFolder.SpecifyParent(modefolder);
	AText	rootModeFolderPath;
	rootModeFolder.GetNormalizedPath(rootModeFolderPath);
	AText	spacetext(" ");
	AText	espapedspacetext("\\ ");
	//#711 rootModeFolderPath.ReplaceText(spacetext,espapedspacetext);
	path.ReplaceText(spacetext,espapedspacetext);
	{
		AStCreateCstringFromAText	cstr(rootModeFolderPath);
		chdir(cstr.GetPtr());
	}
	{
		AText	command("tar zcvf ");
		command.AddText(path);
		command.Add(' ');
		command.AddText(modefoldername);
		AStCreateCstringFromAText	cstr(command);
		system(cstr.GetPtr());
	}
	ALaunchWrapper::Reveal(file);
#endif
}
*/

/*#844
//R0000
ABool	AModePrefDB::RevertToLaunchBackup()
{
	//
	AFileAcc	apppreffolder;
	AFileWrapper::GetAppPrefFolder(apppreffolder);
	AFileAcc	backupfolder;
	backupfolder.SpecifyChildFile(apppreffolder,"_lmb");
	AText	modename;
	GetModeName(modename);
	AFileAcc	backupfile;
	backupfile.SpecifyChildFile(backupfolder,modename);
	if( backupfile.Exist() == false ) 
	{
		//バックアップファイル存在しない
		return false;
	}
	
	//インポートファイル認識スレッドを停止する #0
	//GetApp().SPI_WaitStopImportFileRecognizer(mModeID);
	
	//
	AFileAcc	modefile;
	GetModeFile(modefile);
	//現在のデータを全てファイル保存してゴミ箱へ移動
	SaveToFile();
	AFileWrapper::MoveToTrash(modefile);
	
	//バックアップファイルをコピー
	backupfile.CopyFileTo(modefile,true);
	
	//#350
	AText	backupfilepath;
	backupfile.GetPath(backupfilepath);
	//modepref.miをRevert
	AFileAcc	srcPrefFile;
	srcPrefFile.SpecifySister(modefile,"data/modepref.mi");
	AFileAcc	dstPrefFile;
	AText	backupfilepath_pref(backupfilepath);
	backupfilepath_pref.AddCstring("_modepref");
	dstPrefFile.Specify(backupfilepath_pref);
	AFileWrapper::MoveToTrash(srcPrefFile);
	dstPrefFile.CopyFileTo(srcPrefFile,true);
	//keywords.txtをバックアップ
	AFileAcc	srcKeywordFile;
	srcKeywordFile.SpecifySister(modefile,"data/keywords.txt");
	AFileAcc	dstKeywordFile;
	AText	backupfilepath_keyword(backupfilepath);
	backupfilepath_keyword.AddCstring("_keywords");
	dstKeywordFile.Specify(backupfilepath_keyword);
	AFileWrapper::MoveToTrash(srcKeywordFile);
	dstKeywordFile.CopyFileTo(srcKeywordFile,true);
	
	//
	//#852 InitLoad();
	LoadOrWaitLoadComplete();
	
	return true;
}
*/
//R0000
/**
前モードデータ削除
*/
void	AModePrefDB::DeleteAllModeData()
{
	//mKeywordList全削除
	{
		//#717
		AStMutexLocker	locker(mKeywordListMutex);
		//全識別子削除
		mKeywordList.DeleteAllIdentifiers();//#641
		//mUserKeywordIdentifierUniqIDArray.DeleteAll();//#890
	}
	//システムヘッダファイルデータ全削除
	DeleteAllImportFileData();
	//カテゴリデータ全削除
	mCategoryArray_Name.DeleteAll();
	mCategoryArray_Color.DeleteAll();
	mCategoryArray_ImportColor.DeleteAll();
	mCategoryArray_LocalColor.DeleteAll();
	mCategoryArray_Color_Dark.DeleteAll();//#1316
	mCategoryArray_ImportColor_Dark.DeleteAll();//#1316
	mCategoryArray_LocalColor_Dark.DeleteAll();//#1316
	mCategoryArray_TextStyle.DeleteAll();
	mCategoryArray_MenuTextStyle.DeleteAll();
	mCategoryArray_PriorToOtherColor.DeleteAll();//R0195
	{
		AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
		mCategoryDisabledStateArray.DeleteAll();//R0000 カテゴリー可否
		mCategoryDisabledStateArrayAllFalse = true;//R0000 カテゴリー可否
	}
	mAdditionalCategoryStartIndex = 0;
	//正規表現キーワード全削除
	mKeywordRegExp_Text.DeleteAll();
	mKeywordRegExp_CategoryIndex.DeleteAll();
	//説明データ配列全削除
	mKeywordExplanationTextArray_Keyword.DeleteAll();
	mKeywordExplanationTextArray_CategoryName.DeleteAll();
	mKeywordExplanationTextArray_Explanation.DeleteAll();
	//
	//#427 mAdditionalCategoryArray_KeywordArray.DeleteAll();
	mLegacyKeywordTempDataArray.DeleteAll();//#427
	//文法定義色データ全削除
	mSyntaxDefinitionStateColorArray.DeleteAll();
	mSyntaxDefinitionStateColorArray_Dark.DeleteAll();//#1316
	mSyntaxDefinitionStateColorValidArray.DeleteAll();
	mSyntaxDefinitionStateArray_TextStyle.DeleteAll();//#844
	//suffixハッシュ全削除
	{
		AStMutexLocker	locker(mSuffixHashArrayMutex);
		mSuffixHashArray.DeleteAll();
	}
	//文法定義データ全削除
	mSyntaxDefinition.DeleteAll();
	//背景画像index削除
	mBackgroundImageIndex = kIndex_Invalid;
	//見出し設定バッファ削除
	DeleteJumpSetupAll();
	
	//DBのデータを全てデフォルトに設定する
	SetAllDataToDefault();
}

//#427
/**
モード自動更新

autoupdateフォルダの置き換えと、モードデータ再読込を行う
*/
void	AModePrefDB::AutoUpdate( const AFileAcc& inSrcModeFolder )
{
	//モード設定ウインドウの自動更新開始時処理
	if( GetApp().SPI_GetModePrefWindow(mModeID).SPI_StartAutoUpdate() == false )   return;
	
	//モードファイル保存
	SaveToFile();
	
	//autoupdateフォルダへコピー
	CopyToAutoUpdateFolder(inSrcModeFolder);
	
	//モード設定再読込
	LoadOrWaitLoadComplete();
	
	//全てのドキュメントについて、モードの再設定を行う
	GetApp().SPI_ReSetModeAll(mModeID);
	
	//revision設定（mLoadingRevisionはAApp::ModeUpdate_ResultRevisionCheck()で設定される）
	AText	revisionText;
	revisionText.SetNumber64bit(mLoadingRevision);
	SetData_Text(AModePrefDB::kAutoUpdateRevisionText,revisionText);
	
	//更新日時設定
	AText	datetime;
	ADateTimeWrapper::GetShortDateText(datetime);
	datetime.AddCstring(" ");
	AText	t;
	ADateTimeWrapper::GetTimeText(t);
	datetime.AddText(t);
	SetData_Text(AModePrefDB::kLastAutoUpdateDateTime,datetime);
	
	//モード設定ウインドウの自動更新終了時処理
	GetApp().SPI_GetModePrefWindow(mModeID).SPI_EndAutoUpdate();
}

/**
指定フォルダからautoupdateフォルダへコピー
*/
void	AModePrefDB::CopyToAutoUpdateFolder( const AFileAcc& inSrcModeFolder )
{
	//autoupdateフォルダ更新
	//現在のautoupdateフォルダを全削除
	AFileAcc	dstAutoupdateFolder;
	GetAutoUpdateFolder(dstAutoupdateFolder);
	if( dstAutoupdateFolder.Exist() == true )
	{
		dstAutoupdateFolder.DeleteFileOrFolderRecursive();
	}
	//指定フォルダをまるごとautoupdateフォルダへコピー
	dstAutoupdateFolder.CreateFolderRecursive();
	inSrcModeFolder.CopyFolderTo(dstAutoupdateFolder,false,false);//上書き可、フォルダがすでに存在していても追加コピー
}

//#427
/**
モード自動更新用配布データ生成
*/
void	AModePrefDB::CreateModeUpdateDistribution( const AFileAcc& inFolder )
{
	//モードファイル名取得
	AText	modefilename;
	GetModeRawName(modefilename);
	//モードフォルダ取得
	AFileAcc	modeFolder;
	GetModeFolder(modeFolder);
	//配布データ作成用テンポラリフォルダ生成
	AFileAcc	tempFolder;
	GetApp().SPI_GetTempFolder(tempFolder);
	AFileAcc	tempWorkFolder;
	tempWorkFolder.SpecifyUniqChildFile(tempFolder,"modeUpdateDistribution");
	tempWorkFolder.CreateFolderRecursive();
	AFileAcc	tempModeFolder;
	tempModeFolder.SpecifyChild(tempWorkFolder,modefilename);
	//モードフォルダの内容を全てテンポラリフォルダへコピー
	modeFolder.CopyFolderTo(tempModeFolder,true,true);
	//テンポラリフォルダ内のautoupdate, autoupdate_userdataフォルダを削除
	AFileAcc	tempModeAutoUpdateFolder;
	tempModeAutoUpdateFolder.SpecifyChild(tempModeFolder,"autoupdate");
	tempModeAutoUpdateFolder.DeleteFileOrFolderRecursive();
	AFileAcc	tempModeAutoUpdateFolderUserData;
	tempModeAutoUpdateFolderUserData.SpecifyChild(tempModeFolder,"autoupdate_userdata");
	tempModeAutoUpdateFolderUserData.DeleteFileOrFolderRecursive();
	
	//日時テキスト生成
	AText	datetime("20");
	AText	t;
	ADateTimeWrapper::Get6LettersDateText(t);
	datetime.AddText(t);
	ADateTimeWrapper::Get2LettersHourText(t);
	datetime.AddText(t);
	ADateTimeWrapper::Get2LettersMinuteText(t);
	datetime.AddText(t);
	AText	foldername("Distribution_");
	foldername.AddText(modefilename);
	foldername.AddText(datetime);
	AFileAcc	distfolder;
	distfolder.SpecifyUniqChildFile(inFolder,foldername);
	distfolder.CreateFolder();
	//update.txtファイル生成
	AText	updateText;
	updateText.AddCstring("name:\"");
	updateText.AddText(modefilename);
	updateText.AddCstring("\"\nrevision:");
	updateText.AddText(datetime);
	updateText.AddCstring("\n");
	AFileAcc	updateTextFile;
	updateTextFile.SpecifyChild(distfolder,"update.txt");
	updateTextFile.CreateFile();
	updateTextFile.WriteFile(updateText);
	//zipファイル生成
	AText	zipFileName;
	zipFileName.AddText(modefilename);
	zipFileName.AddText(datetime);
	zipFileName.AddCstring(".zip");
	AFileAcc	file;
	file.SpecifyChild(distfolder,zipFileName);
	AZipFile	zipFile(file);
	zipFile.Zip(tempModeFolder);
}

//#427
/**
モード設定バックアップ
*/
void	AModePrefDB::ModeBackup( const ABool inReveal )
{
	//modebackupフォルダ取得
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	AFileAcc	backupRootFolder;
	backupRootFolder.SpecifyChild(appPrefFolder,"modebackup");
	backupRootFolder.CreateFolder();
	
	//モード名のフォルダをmodebackupフォルダに生成する
	AText	modename;
	GetModeRawName(modename);
	AFileAcc	backupModeFolder;
	backupModeFolder.SpecifyChild(backupRootFolder,modename);
	backupModeFolder.CreateFolder();
	
	//日時からバックアップ先フォルダ名生成
	AText	foldername;
	AText	t;
	ADateTimeWrapper::Get6LettersDateText(t);
	foldername.AddText(t);
	ADateTimeWrapper::Get2LettersHourText(t);
	foldername.AddText(t);
	ADateTimeWrapper::Get2LettersMinuteText(t);
	foldername.AddText(t);
	AFileAcc	folder;
	folder.SpecifyUniqChildFile(backupModeFolder,foldername);
	
	//フォルダコピー
	AFileAcc	modeFolder;
	GetModeFolder(modeFolder);
	modeFolder.CopyFolderTo(folder,true,true);
	
	//フォルダReveal
	if( inReveal == true )
	{
		ALaunchWrapper::Reveal(folder);
	}
}

//#889
/**
カラースキームDB更新
*/
/*#1316 AColorSchemeDB::Load()へ移動
void	AModePrefDB::UpdateColorScheme()
{
	//現在のスキーム名取得
	AText	schemename;
	GetModeData_Text(kColorSchemeName,schemename);
	//ファイルタイプ
	AColorSchemeType	type = kColorSchemeType_CSV;
	//csv, epfファイルのファイル名取得
	AText	csvschemename, epfschemename;
	csvschemename.SetText(schemename);
	csvschemename.AddCstring(".csv");
	epfschemename.SetText(schemename);
	epfschemename.AddCstring(".epf");
	//ユーザーカラースキームフォルダ取得
	AFileAcc	userSchemeFolder;
	GetApp().SPI_GetUserColorSchemeFolder(userSchemeFolder);
	//カラースキームファイル取得
	AFileAcc	file;
	file.SpecifyChild(userSchemeFolder,csvschemename);
	//csvファイルが存在しない場合はepfファイルを試す
	if( file.Exist() == false )
	{
		file.SpecifyChild(userSchemeFolder,epfschemename);
		if( file.Exist() == true )
		{
			//epfファイルが存在していたらファイルタイプはepfにする
			type = kColorSchemeType_EPF;
		}
	}
	//ユーザーカラースキームフォルダに対象名のファイルが存在していなければ、アプリカラースキームフォルダから探す
	if( file.Exist() == false )
	{
		//アプリケーションのカラースキームフォルダ取得
		AFileAcc	appSchemeFolder;
		GetApp().SPI_GetAppColorSchemeFolder(appSchemeFolder);
		//
		file.SpecifyChild(appSchemeFolder,csvschemename);
		//ファイルが存在していなければデフォルトのファイルを取得
		if( file.Exist() == false )
		{
			file.SpecifyChild(appSchemeFolder,"mi-default.csv");
		}
	}
	//カラースキームDB初期化
	mColorSchemeDB.SetAllDataToDefault();
	//カラースキームDBへ読み込み
	mColorSchemeDB.LoadFromColorSchemeFile(file,type);
}
*/

//#889
/**
現在の色をLastOverwrittenColorsスキームファイルに保存
*/
void	AModePrefDB::SaveLastColors()
{
	//ユーザーカラースキームフォルダ取得
	AFileAcc	userSchemeFolder;
	GetApp().SPI_GetUserColorSchemeFolder(userSchemeFolder);
	//LastOverwrittenColorsカラースキームファイル取得
	AFileAcc	file;
	file.SpecifyChild(userSchemeFolder,"LastOverwrittenColors.csv");
	//ファイルにエクスポート
	ExportColors(file);
	//カラースキームメニュー更新
	GetApp().SPI_UpdateColorSchemeMenu();
}

/**
色エクスポート
*/
void	AModePrefDB::ExportColors( const AFileAcc& inFile )
{
	//CSVデータ生成
	AText	colorText;
	AColor	color = kColor_Black;
	AText	csvData;
	//
	csvData.AddCstring("letter,");
	GetData_Color(kLetterColor,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//#1142
	csvData.AddCstring("\ncontrol-code,");
	GetData_Color(kControlCodeColor,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nbackground,");
	GetData_Color(kBackgroundColor,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\ncomment,");
	GetData_Color(kSyntaxColorSlotComment_Color,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nliteral,");
	GetData_Color(kSyntaxColorSlotLiteral_Color,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nreserved-word,");
	GetData_Color(kSyntaxColorSlot0_Color,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nmethod,");
	GetData_Color(kSyntaxColorSlot1_Color,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nproperty,");
	GetData_Color(kSyntaxColorSlot2_Color,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nvariable,");
	GetData_Color(kSyntaxColorSlot3_Color,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nclass,");
	GetData_Color(kSyntaxColorSlot4_Color,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nmacro,");
	GetData_Color(kSyntaxColorSlot5_Color,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nother1,");
	GetData_Color(kSyntaxColorSlot6_Color,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nother2,");
	GetData_Color(kSyntaxColorSlot7_Color,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nhighlight1,");
	GetData_Color(kFindHighlightColor,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nhighlight2,");
	GetData_Color(kFindHighlightColorPrev,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nhighlight3,");
	GetData_Color(kCurrentWordHighlightColor,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nhighlight4,");
	GetData_Color(kTextMarkerHightlightColor,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\nhighlight5,");
	GetData_Color(kFirstSelectionColor,color);
	AColorWrapper::GetHTMLFormatColorText(color,colorText);
	csvData.AddText(colorText);
	//
	csvData.AddCstring("\ndiff-opacity,");
	csvData.AddNumber(GetData_Number(kDiffColorOpacity));
	//
	csvData.AddCstring("\nselection-opacity,");
	csvData.AddNumber(GetData_Number(kSelectionOpacity));
	//
	inFile.CreateFile();
	inFile.WriteFile(csvData);
}

//#914
/**
モードが有効かどうかを取得
*/
ABool	AModePrefDB::IsModeEnabled() const
{
	return GetData_Bool(kEnableMode);
}

#pragma mark ===========================

#pragma mark ---データ取得オーバーライド

/**
指定データ項目について、標準モードと同じ設定にすべきかどうかを取得
*/
ABool	AModePrefDB::IsSameAsNormal( const ADataID inID ) const
{
	ABool	sameAsNormal = false;
	switch(inID)
	{
		//フォント
	  case kDefaultFontName:
	  case kDefaultFontSize:
		{
			if( GetData_Bool(kSameAsNormalMode_Font) == true )
			{
				sameAsNormal = true;
			}
			break;
		}
		//色
	  case kLetterColor:
	  case kControlCodeColor://#1142
	  case kBackgroundColor:
		//#889 case kUseColorScheme:
	  case kColorSchemeName:
	  case kSyntaxColorSlot0_Color:
	  case kSyntaxColorSlot1_Color:
	  case kSyntaxColorSlot2_Color:
	  case kSyntaxColorSlot3_Color:
	  case kSyntaxColorSlot4_Color:
	  case kSyntaxColorSlot5_Color:
	  case kSyntaxColorSlot6_Color:
	  case kSyntaxColorSlot7_Color:
	  case kSyntaxColorSlotComment_Color:
	  case kSyntaxColorSlotLiteral_Color:
	  case kSyntaxColorSlot0_Bold:
	  case kSyntaxColorSlot1_Bold:
	  case kSyntaxColorSlot2_Bold:
	  case kSyntaxColorSlot3_Bold:
	  case kSyntaxColorSlot4_Bold:
	  case kSyntaxColorSlot5_Bold:
	  case kSyntaxColorSlot6_Bold:
	  case kSyntaxColorSlot7_Bold:
	  case kSyntaxColorSlotComment_Bold:
	  case kSyntaxColorSlotLiteral_Bold:
	  case kSyntaxColorSlot0_Italic:
	  case kSyntaxColorSlot1_Italic:
	  case kSyntaxColorSlot2_Italic:
	  case kSyntaxColorSlot3_Italic:
	  case kSyntaxColorSlot4_Italic:
	  case kSyntaxColorSlot5_Italic:
	  case kSyntaxColorSlot6_Italic:
	  case kSyntaxColorSlot7_Italic:
	  case kSyntaxColorSlotComment_Italic:
	  case kSyntaxColorSlotLiteral_Italic:
	  case kSyntaxColorSlot0_Underline:
	  case kSyntaxColorSlot1_Underline:
	  case kSyntaxColorSlot2_Underline:
	  case kSyntaxColorSlot3_Underline:
	  case kSyntaxColorSlot4_Underline:
	  case kSyntaxColorSlot5_Underline:
	  case kSyntaxColorSlot6_Underline:
	  case kSyntaxColorSlot7_Underline:
	  case kSyntaxColorSlotComment_Underline:
	  case kSyntaxColorSlotLiteral_Underline:
	  case kDarkenImportLightenLocal:
	  case kFindHighlightColor:
	  case kFindHighlightColorPrev:
	  case kCurrentWordHighlightColor:
	  case kTextMarkerHightlightColor:
	  case kFirstSelectionColor:
	  case kDiffColorOpacity:
	  case kSelectionOpacity:
		{
			if( GetData_Bool(kSameAsNormalMode_Colors) == true )
			{
				sameAsNormal = true;
			}
			break;
		}
		//テキスト表示
		//#912 case kDefaultTabWidth:
	  case kLineMargin:
	  case kDisplayReturnCode:
	  case kDisplayTabCode:
	  case kDisplaySpaceCode:
	  case kDisplayZenkakuSpaceCode:
		{
			if( GetData_Bool(kSameAsNormalMode_TextDisplay) == true )
			{
				sameAsNormal = true;
			}
			break;
		}
		//背景表示
	  case kDisplayBackgroundImage:
	  case kBackgroundImageFile:
	  case kBackgroundImageTransparency:
	  case kDisplayGenkoyoshi:
		{
			if( GetData_Bool(kSameAsNormalMode_BackgroundDisplay) == true )
			{
				sameAsNormal = true;
			}
			break;
		}
		//レイアウト（ルーラー・マクロバー・行番号表示）
	  case kDisplayRuler:
	  case kRulerBaseLetter:
	  case kRulerScaleWithTabLetter:
	  case kDisplayToolbar:
	  case kDisplayEachLineNumber:
	  case kDisplayEachLineNumber_AsParagraph:
		{
			if( GetData_Bool(kSameAsNormalMode_TextLayout) == true )
			{
				sameAsNormal = true;
			}
			break;
		}
		//デフォルトテキストエンコーディング・改行コード・行折り返し
	  case kDefaultTextEncoding:
	  case kDefaultReturnCode:
	  case kDefaultWrapMode:
	  case kDefaultWrapMode_LetterCount:
	  case kSaveUTF8BOM:
		{
			if( GetData_Bool(kSameAsNormalMode_TextProperty) == true )
			{
				sameAsNormal = true;
			}
			break;
		}
		//括弧対応・4回クリック・￥キー
	  case kCheckBrace:
	  case kSelectInsideByQuadClick:
	  case kQuadClickText:
	  case kInputBackslashByYenKey:
	  case kAddRemoveSpaceByDragDrop:
	  case kApplyCharsForWordToDoubleClick:
		{
			if( GetData_Bool(kSameAsNormalMode_InputtSettings) == true )
			{
				sameAsNormal = true;
			}
			break;
		}
		//スペルチェック
	  case kDisplaySpellCheck:
	  case kSpellCheckLanguage:
		{
			if( GetData_Bool(kSameAsNormalMode_SpellCheck) == true )
			{
				sameAsNormal = true;
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return sameAsNormal;
}

/**
データ取得オーバーライド
*/
ABool	AModePrefDB::GetModeData_Bool( ADataID inID ) const
{
	//環境設定カラースキームを使用する場合は、AppPrefからデータを取得する #1316
	if( GetApp().GetAppPref().UseAppPrefColorScheme(GetApp().NVI_IsDarkMode()) == true )
	{
		const AColorSchemeDB& colorSchemeDB = GetApp().GetAppPref().GetColorSchemeDB(GetApp().NVI_IsDarkMode());
		switch(inID)
		{
		  case kDarkenImportLightenLocal:
			{
				return true;
			}
		}
	}
	
	//
	if( mModeID == 0 || IsSameAsNormal(inID) == false )
	{
		return ADataBase::GetData_Bool(inID);
	}
	else
	{
		return GetApp().SPI_GetModePrefDB(0).GetData_Bool(inID);
	}
}

/**
データ取得オーバーライド
*/
void	AModePrefDB::GetModeData_Text( ADataID inID, AText& outText ) const
{
	if( mModeID == 0 || IsSameAsNormal(inID) == false )
	{
		ADataBase::GetData_Text(inID,outText);
	}
	else
	{
		GetApp().SPI_GetModePrefDB(0).GetData_Text(inID,outText);
	}
}

/**
データ取得オーバーライド
*/
ANumber	AModePrefDB::GetModeData_Number( ADataID inID ) const
{
	//環境設定カラースキームを使用する場合は、AppPrefからデータを取得する #1316
	if( GetApp().GetAppPref().UseAppPrefColorScheme(GetApp().NVI_IsDarkMode()) == true )
	{
		const AColorSchemeDB& colorSchemeDB = GetApp().GetAppPref().GetColorSchemeDB(GetApp().NVI_IsDarkMode());
		switch(inID)
		{
		  case kSelectionOpacity:
			{
				return colorSchemeDB.GetData_Number(AColorSchemeDB::kSelectionOpacity);
			}
		  case kDiffColorOpacity:
			{
				return colorSchemeDB.GetData_Number(AColorSchemeDB::kDiffOpacity);
			}
		}
	}
	
	//
	if( mModeID == 0 || IsSameAsNormal(inID) == false )
	{
		return ADataBase::GetData_Number(inID);
	}
	else
	{
		return GetApp().SPI_GetModePrefDB(0).GetData_Number(inID);
	}
}

/**
データ取得オーバーライド
*/
AFloatNumber	AModePrefDB::GetModeData_FloatNumber( ADataID inID ) const
{
	if( mModeID == 0 || IsSameAsNormal(inID) == false )
	{
		return ADataBase::GetData_FloatNumber(inID);
	}
	else
	{
		return GetApp().SPI_GetModePrefDB(0).GetData_FloatNumber(inID);
	}
}

/**
データ取得オーバーライド
*/
void	AModePrefDB::GetModeData_Color( ADataID inID, AColor& outData ) const
{
	GetModeData_Color(inID, outData, GetApp().NVI_IsDarkMode());
}
void	AModePrefDB::GetModeData_Color( ADataID inID, AColor& outData, const ABool inDarkMode ) const//#1316
{
	//環境設定カラースキームを使用する場合は、AppPrefからデータを取得する #1316
	if( GetApp().GetAppPref().UseAppPrefColorScheme(inDarkMode) == true )
	{
		const AColorSchemeDB& colorSchemeDB = GetApp().GetAppPref().GetColorSchemeDB(inDarkMode);
		switch(inID)
		{
		  case kLetterColor:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kLetter,outData);
				return;
			}
		  case kBackgroundColor:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kBackground,outData);
				return;
			}
		  case kSyntaxColorSlotComment_Color:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kComment,outData);
				return;
			}
		  case kSyntaxColorSlotLiteral_Color:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kLiteral,outData);
				return;
			}
		  case kSyntaxColorSlot0_Color:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kSlot0,outData);
				return;
			}
		  case kSyntaxColorSlot1_Color:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kSlot1,outData);
				return;
			}
		  case kSyntaxColorSlot2_Color:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kSlot2,outData);
				return;
			}
		  case kSyntaxColorSlot3_Color:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kSlot3,outData);
				return;
			}
		  case kSyntaxColorSlot4_Color:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kSlot4,outData);
				return;
			}
		  case kSyntaxColorSlot5_Color:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kSlot5,outData);
				return;
			}
		  case kSyntaxColorSlot6_Color:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kSlot6,outData);
				return;
			}
		  case kSyntaxColorSlot7_Color:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kSlot7,outData);
				return;
			}
		  case kFindHighlightColor:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kHighlight1,outData);
				return;
			}
		  case kFindHighlightColorPrev:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kHighlight2,outData);
				return;
			}
		  case kCurrentWordHighlightColor:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kHighlight3,outData);
				return;
			}
		  case kTextMarkerHightlightColor:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kHighlight4,outData);
				return;
			}
		  case kFirstSelectionColor:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kHighlight5,outData);
				return;
			}
		  case kControlCodeColor:
			{
				colorSchemeDB.GetData_Color(AColorSchemeDB::kControlCode,outData);
				return;
			}
		}
	}
	
	//
	if( mModeID == 0 || IsSameAsNormal(inID) == false )
	{
		//==================標準モード、または、標準モードと同じ設定を使う設定=NOの場合==================
		//このモードのデータを使う
		
		//データ取得
		ADataBase::GetData_Color(inID,outData);
		
		/*#889 
		//カラースキームを使用する設定ONかつ、カラースキームデータがあれば、その色で上書きする。
		//（このモードのカラースキーム）
		if( GetData_Bool(kUseColorScheme) == true )
		{
			switch(inID)
			{
			  case kLetterColor:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kLetter,outData);
					break;
				}
			  case kBackgroundColor:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kBackground,outData);
					break;
				}
			  case kSyntaxColorSlotComment_Color:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kComment,outData);
					break;
				}
			  case kSyntaxColorSlotLiteral_Color:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kLiteral,outData);
					break;
				}
			  case kSyntaxColorSlot0_Color:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kSlot0,outData);
					break;
				}
			  case kSyntaxColorSlot1_Color:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kSlot1,outData);
					break;
				}
			  case kSyntaxColorSlot2_Color:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kSlot2,outData);
					break;
				}
			  case kSyntaxColorSlot3_Color:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kSlot3,outData);
					break;
				}
			  case kSyntaxColorSlot4_Color:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kSlot4,outData);
					break;
				}
			  case kSyntaxColorSlot5_Color:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kSlot5,outData);
					break;
				}
			  case kSyntaxColorSlot6_Color:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kSlot6,outData);
					break;
				}
			  case kSyntaxColorSlot7_Color:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kSlot7,outData);
					break;
				}
			  case kFindHighlightColor:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kHighlight1,outData);
					break;
				}
			  case kFindHighlightColorPrev:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kHighlight2,outData);
					break;
				}
			  case kCurrentWordHighlightColor:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kHighlight3,outData);
					break;
				}
			  case kTextMarkerHightlightColor:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kHighlight4,outData);
					break;
				}
			  case kFirstSelectionColor:
				{
					mColorSchemeDB.GetColor(AColorSchemeDB::kHighlight5,outData);
					break;
				}
			}
		}
		*/
	}
	else
	{
		//==================標準モードではなく、かつ、標準モードと同じ設定を使う設定=YESの場合==================
		//標準モードのデータを使う
		GetApp().SPI_GetModePrefDB(0).GetModeData_Color(inID,outData,inDarkMode);//#1316
	}
}

//#1316 
/**
カラースキームの色をモード設定色に適用する（モード設定の「カラースキームを適用」ボタンクリック時の処理）
*/
void	AModePrefDB::ApplyFromColorScheme( const AText& inSchemeName )
{
	//AColorSchemeDBにロードする
	AColorSchemeDB	colorSchemeDB;
	colorSchemeDB.Load(inSchemeName);
	
	//
	AColor	color = kColor_Black;
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kLetter) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kLetter,color);
		SetData_Color(kLetterColor,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kBackground) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kBackground,color);
		SetData_Color(kBackgroundColor,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kComment) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kComment,color);
		SetData_Color(kSyntaxColorSlotComment_Color,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kLiteral) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kLiteral,color);
		SetData_Color(kSyntaxColorSlotLiteral_Color,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kSlot0) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kSlot0,color);
		SetData_Color(kSyntaxColorSlot0_Color,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kSlot1) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kSlot1,color);
		SetData_Color(kSyntaxColorSlot1_Color,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kSlot2) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kSlot2,color);
		SetData_Color(kSyntaxColorSlot2_Color,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kSlot3) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kSlot3,color);
		SetData_Color(kSyntaxColorSlot3_Color,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kSlot4) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kSlot4,color);
		SetData_Color(kSyntaxColorSlot4_Color,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kSlot5) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kSlot5,color);
		SetData_Color(kSyntaxColorSlot5_Color,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kSlot6) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kSlot6,color);
		SetData_Color(kSyntaxColorSlot6_Color,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kSlot7) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kSlot7,color);
		SetData_Color(kSyntaxColorSlot7_Color,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kHighlight1) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kHighlight1,color);
		SetData_Color(kFindHighlightColor,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kHighlight2) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kHighlight2,color);
		SetData_Color(kFindHighlightColorPrev,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kHighlight3) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kHighlight3,color);
		SetData_Color(kCurrentWordHighlightColor,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kHighlight4) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kHighlight4,color);
		SetData_Color(kTextMarkerHightlightColor,color);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kHighlight5) == false )
	{
		colorSchemeDB.GetColor(AColorSchemeDB::kHighlight5,color);
		SetData_Color(kFirstSelectionColor,color);
	}
	//
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kDiffOpacity) == false )
	{
		ANumber	num = colorSchemeDB.GetData_Number(AColorSchemeDB::kDiffOpacity);
		SetData_Number(kDiffColorOpacity,num);
	}
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kSelectionOpacity) == false )
	{
		ANumber	num = colorSchemeDB.GetData_Number(AColorSchemeDB::kSelectionOpacity);
		SetData_Number(kSelectionOpacity,num);
	}
	//#1142
	//制御コード文字色
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kControlCode) == false )
	{
		//カラースキームに制御コード色の設定があれば、その色を設定
		colorSchemeDB.GetColor(AColorSchemeDB::kControlCode,color);
		SetData_Color(kControlCodeColor,color);
	}
	else if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kLetter) == false )
	{
		//カラースキームで制御コード色未設定、かつ、文字色の設定がある場合は、背景色に文字色α30%でブレンドした色を設定
		AColor	backgroundColor = kColor_White;
		GetData_Color(kBackgroundColor,backgroundColor);
		AColor	letterColor = kColor_Black;
		GetData_Color(kLetterColor,letterColor);
		AColor	controlCodeColor = AColorWrapper::GetAlpheBlend(backgroundColor,letterColor,0.3);
		SetData_Color(kControlCodeColor,controlCodeColor);
	}
}

#pragma mark ===========================

#pragma mark ---キーワード構成文字

void	AModePrefDB::UpdateIsAlphabetTable()
{
	//
	//mIsContinuousAlphabet: falseならこの文字の直前が単語区切りになる
	//mIsTailAlphabet: falseならこの文字の直後が単語区切りになる
	
	//アルファベットテーブルの更新
	for( AIndex i = 0; i < 256; i++ )
	{
		mIsAlphabet[i] = false;
		mIsContinuousAlphabet[i] = false;
		mIsTailAlphabet[i] = false;
	}
	AText	alphatext;
	GetData_Text(kCharsForWord,alphatext);
	for( AIndex i = 0; i < alphatext.GetItemCount(); i++ )
	{
		mIsAlphabet[alphatext.Get(i)] = true;
		mIsContinuousAlphabet[alphatext.Get(i)] = true;
	}
	//B0444
	AText	headtext;
	GetData_Text(kCharsForHeadOfWord,headtext);
	for( AIndex i = 0; i < headtext.GetItemCount(); i++ )
	{
		mIsAlphabet[headtext.Get(i)] = true;
		mIsContinuousAlphabet[headtext.Get(i)] = false;
	}
	//B0444
	AText	tailtext;
	GetData_Text(kCharsForTailOfWord,tailtext);
	for( AIndex i = 0; i < tailtext.GetItemCount(); i++ )
	{
		mIsAlphabet[tailtext.Get(i)] = true;
		if( headtext.Find(tailtext.Get(i)) == kIndex_Invalid )//B0444 #15 false->kIndex_Invalid
		{
			mIsContinuousAlphabet[tailtext.Get(i)] = true;
		}
		mIsTailAlphabet[tailtext.Get(i)] = true;
	}
}

#pragma mark ===========================

#pragma mark ---プレビュープラグイン

/**
プレビュープラグインロード
*/
void	AModePrefDB::LoadPreviewPlugin()
{
	//プレビュープラグイン初期化
	mPreviewPluginText.DeleteAll();
	mPreviewPluginExist = false;
	//プレビュープラグインが存在していれば、読み込み
	/*#1275
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	AFileAcc	file;
	file.SpecifyChild(modefolder,"autoupdate/data/preview.js");
	*/
	AFileAcc	autoupdateFolder;
	GetAutoUpdateFolder(autoupdateFolder);
    AFileAcc    file;
	file.SpecifyChild(autoupdateFolder,"data/preview.js");
	if( file.Exist() == true )
	{
		file.ReadTo(mPreviewPluginText);
		mPreviewPluginExist = true;
	}
}

/**
プレビュープラグイン取得
*/
ABool	AModePrefDB::GetPreviewPlugin( AText& outPluginText ) const
{
	outPluginText.DeleteAll();
	if( mPreviewPluginExist == true )
	{
		outPluginText.SetText(mPreviewPluginText);
		return true;
	}
	else
	{
		return false;
	}
}

//#994
/**
プラグインロード
*/
void	AModePrefDB::LoadPlugins()
{
	for( AIndex i = 0; i < 2; i++ )
	{
		//プラグインフォルダ取得
		AFileAcc	pluginsFolder;
		GetPluginsFolder((i==0),pluginsFolder);
		pluginsFolder.CreateFolder();
		//各プラグインファイルごとのループ
		AObjectArray<AFileAcc>	pluginFileArray;
		pluginsFolder.GetChildren(pluginFileArray);
		for( AIndex j = 0; j < pluginFileArray.GetItemCount(); j++ )
		{
			//ファイル取得
			AFileAcc	file = pluginFileArray.GetObjectConst(j);
			if( file.IsFolder() == false )
			{
				//プラグインテキスト読み込み
				AText	pluginText;
				file.ReadTo(pluginText);
				//プラグインロード（JS実行）
				GetApp().SPI_LoadPlugin(mModeID,pluginText,pluginsFolder);
			}
		}
	}
}

//#1421
/**
Flexibleタブ位置設定parse
*/
void	AModePrefDB::UpdateFlexibleTabPositions()
{
	//タブ位置配列初期化
	mFlexibleTabPositions.DeleteAll();
	//FlexibleタブEnableでなければ終了
	if( GetData_Bool(kEnableFlexibleTabPositions) == false )
	{
		return;
	}
	//テキスト取得
	AText	tabPositionsText;
	GetData_Text(kFlexibleTabPositions, tabPositionsText);
	//CSVオブジェクト生成
	ACSVFile	csv(tabPositionsText,ATextEncodingWrapper::GetUTF8TextEncoding(),1);
	for( AIndex i = 0; i < 256; i++ )
	{
		//列取得
		ATextArray	textArray;
		if( csv.GetColumn(i,textArray) == false )
		{
			//列がなくなったら終了
			break;
		}
		//最初の行の数値を取得
		ANumber	number = 0;
		if( textArray.GetItemCount() > 0 )
		{
			AText	t;
			textArray.Get(0,t);
			AIndex	p = 0;
			t.SkipTabSpace(p,t.GetItemCount());
			t.ParseIntegerNumber(p, number, false);
		}
		//配列に追加
		mFlexibleTabPositions.Add(number);
	}
}

/**
プラグイン登録（プラグインのJSからコールされる）
*/
void	AModePrefDB::RegisterPlugin( const AText& inPluginID, const AText& inPluginName, 
									 const ABool inDefaultEnabled, const AText& inDefaultOptions )
{
	//登録済みなら何もしない
	AIndex	index = Find_TextArray(kPluginData_Id,inPluginID);
	if( index != kIndex_Invalid )
	{
		return;
	}
	//登録
	Add_TextArray(kPluginData_Id,inPluginID);
	Add_TextArray(kPluginData_Name,inPluginName);
	Add_BoolArray(kPluginData_Enabled,inDefaultEnabled);
	Add_TextArray(kPluginData_Options,inDefaultOptions);
}

/**
プラグイン表示名登録（プラグインのJSからコールされる）
*/
void	AModePrefDB::SetPluginDisplayName( const AText& inPluginID, const AText& inLangName, const AText& inDisplayName )
{
	//プラグインID, 言語名をタブでつないだものをキーとする
	AText	id_lang;
	id_lang.SetText(inPluginID);
	id_lang.AddCstring("\t");
	id_lang.AddText(inLangName);
	//登録済みなら削除
	AIndex	index = Find_TextArray(kPluginDisplayName_Id_Language,id_lang);
	if( index != kIndex_Invalid )
	{
		DeleteRow_Table(kPluginDisplayName_Id_Language,index);
	}
	//登録
	Add_TextArray(kPluginDisplayName_Id_Language,id_lang);
	Add_TextArray(kPluginDisplayName_DisplayName,inDisplayName);
}

/**
プラグイン表示名取得
*/
ABool	AModePrefDB::GetPluginDisplayName( const AText& inPluginID, AText& outDisplayName ) const
{
	//現在環境の言語取得
	AText	langName;
	GetApp().NVI_GetLanguageName(langName);
	//プラグインID, 現在言語をタブでつないだものをキーとする
	AText	id_lang;
	id_lang.SetText(inPluginID);
	id_lang.AddCstring("\t");
	id_lang.AddText(langName);
	//プラグイン表示名取得
	AIndex	index = Find_TextArray(kPluginDisplayName_Id_Language,id_lang);
	if( index != kIndex_Invalid )
	{
		GetData_TextArray(kPluginDisplayName_DisplayName,index,outDisplayName);
		return true;
	}
	else
	{
		return false;
	}
}

/**
プラグインがEnableかどうかを取得
*/
ABool	AModePrefDB::GetPluginEnabled( const AText& inPluginID ) const
{
	AIndex	index = Find_TextArray(kPluginData_Id,inPluginID);
	if( index != kIndex_Invalid )
	{
		return GetData_BoolArray(kPluginData_Enabled,index);
	}
	else
	{
		return false;
	}
}

/**
プラグインオプション取得
*/
void	AModePrefDB::GetPluginOptions( const AText& inPluginID, AText& outOptions ) const
{
	outOptions.DeleteAll();
	AIndex	index = Find_TextArray(kPluginData_Id,inPluginID);
	if( index != kIndex_Invalid )
	{
		GetData_TextArray(kPluginData_Options,index,outOptions);
	}
}

#pragma mark ===========================

#pragma mark ---ツール（基本制御）

/*
【データ構造】
mToolMenuRootObjectID: ルートメニューのMenuObjectID
mToolMenuArray_MenuObjectID: ルートメニューおよび各サブメニューのMenuObjectIDを格納した配列

mToolItemArray_xxx: ツールの各項目を格納した二次元配列。以下の5つがある。
mToolItemArray_File, mToolItemArray_SubMenuObjectID, mToolItemArray_Enabled, mToolItemArray_Shortcut, mToolItemArray_Modifiers
外側の配列のindexは、mToolMenuArray_MenuObjectIDのindexに対応する。
内側の配列のindexは、メニュー内の順番。（ただし、メニューに表示されない（有効でない）項目も配列内に含まれる。）

【概要】
LoadTool()により、内部データを構築、メニューを構成する。
メニューの構成はUpdateToolMenu()により実行される。メニュー構成されるMenuObjectIDはあらかじめ決まっている。有効かどうかの内部データを参照しながら構築される。


*/

/**
Load（ルートメニュー配下全てを（再度）読み込み）
*/
void	AModePrefDB::LoadTool()
{
	//元々realize済みだった場合は、realize解除
	ABool	realized = false;
	if( mToolMenuRootObjectID != kObjectID_Invalid )
	{
		realized = GetApp().NVI_GetMenuManager().GetDynamicMenuRealizedByObjectID(mToolMenuRootObjectID);
	}
	if( realized == true )
	{
		GetApp().NVI_GetMenuManager().UnrealizeDynamicMenu(mToolMenuRootObjectID);
	}
	
	//ルートメニューデータ生成
	
	//#305 下から移動
	//ルートフォルダ作成
	AFileAcc	toolRootFolder;
	GetToolFolder(false,toolRootFolder);//#427 ユーザー定義のほうのツールルートフォルダ
	toolRootFolder.CreateFolder();
	//
	if( mToolMenuRootObjectID != kObjectID_Invalid )
	{
		//既にルートメニュー作成済みの場合は、ルートメニューの各項目を削除
		while( GetToolItemCount(mToolMenuRootObjectID) > 0 )
		{
			DeleteToolItem(mToolMenuRootObjectID,0,false,false);
		}
		UpdateToolMenu(mToolMenuRootObjectID);
	}
	else
	{
		//ルートメニュー未作成の場合は、新規作成
		AMenuItemID	menuItemID = kMenuItemID_Tool;
		if( mModeID == kStandardModeIndex )
		{
			menuItemID = kMenuItemID_Tool_StandardMode;
		}
		mToolMenuRootObjectID = GetApp().NVI_GetMenuManager().CreateDynamicMenu(menuItemID,true);
		mToolMenuArray_MenuObjectID.Add(mToolMenuRootObjectID);
		mToolMenuArray_Folder.GetObject(mToolMenuArray_Folder.AddNewObject()).CopyFrom(toolRootFolder);//#305
		mToolMenuArray_AutoUpdateFolder.Add(false);//#427 ルートフォルダはユーザー編集可
		mToolItemArray_File.AddNewObject();
		mToolItemArray_SubMenuObjectID.AddNewObject();
		mToolItemArray_Enabled.AddNewObject();
		mToolItemArray_Shortcut.AddNewObject();
		mToolItemArray_Modifiers.AddNewObject();
		mToolItemArray_Grayout.AddNewObject();//#129
		mToolItemArray_AutoUpdate.AddNewObject();//#427
		mToolItemArray_StaticText.AddNewObject();//#427
		mToolItemArray_JapaneseName.AddNewObject();//#305
		mToolItemArray_EnglishName.AddNewObject();//#305
		mToolItemArray_FrenchName.AddNewObject();//#305
		mToolItemArray_GermanName.AddNewObject();//#305
		mToolItemArray_SpanishName.AddNewObject();//#305
		mToolItemArray_ItalianName.AddNewObject();//#305
		mToolItemArray_DutchName.AddNewObject();//#305
		mToolItemArray_SwedishName.AddNewObject();//#305
		mToolItemArray_NorwegianName.AddNewObject();//#305
		mToolItemArray_DanishName.AddNewObject();//#305
		mToolItemArray_FinnishName.AddNewObject();//#305
		mToolItemArray_PortugueseName.AddNewObject();//#305
		mToolItemArray_SimplifiedChineseName.AddNewObject();//#305
		mToolItemArray_TraditionalChineseName.AddNewObject();//#305
		mToolItemArray_KoreanName.AddNewObject();//#305
		mToolItemArray_PolishName.AddNewObject();//#305
		mToolItemArray_PortuguesePortugalName.AddNewObject();//#305
		mToolItemArray_RussianName.AddNewObject();//#305
	}
	/*#305 上へ移動
	//ルートフォルダ作成
	AFileAcc	toolRootFolder;
	GetToolFolder(toolRootFolder);
	toolRootFolder.CreateFolder();
	*/
	
	
	//モード表示名取得
	AText	modeDisplayName;
	GetModeDisplayName(modeDisplayName);
	
	//ルートのユーザーツール、アプリ組み込みツールの位置初期化
	mToolMenuRoot_UserToolStartIndex = mToolMenuRoot_UserToolEndIndex = 0;
	mToolMenuRoot_AppToolStartIndex = mToolMenuRoot_AppToolEndIndex = 0;
	
	//==================ユーザーツール==================
	//ユーザーツール表示有効ならユーザーツールを通常フォルダからロード
	//#844 if( GetData_Bool(kEnableUserTool) == true )
	//ユーザーツール開始位置取得
	mToolMenuRoot_UserToolStartIndex = GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mToolMenuRootObjectID);//GetToolItemCount(mToolMenuRootObjectID);
	{
		//#427 「ユーザーツール（モード：）」というメニュー項目を追加
		AText	staticText;
		staticText.SetLocalizedText("ToolMenuText_UserTool");
		staticText.ReplaceParamText('0',modeDisplayName);
		InsertStaticTextToolItem(mToolMenuRootObjectID,GetToolItemCount(mToolMenuRootObjectID),
								 staticText,false,false,false);
		//ツール読み込み（ユーザー定義）
		LoadToolRecursive(toolRootFolder,mToolMenuRootObjectID,false);//#427
	}
	//ユーザーツール終了位置取得
	mToolMenuRoot_UserToolEndIndex = GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mToolMenuRootObjectID);//GetToolItemCount(mToolMenuRootObjectID);
	
	//区切り線追加
	AText	sepatext("-");
	InsertStaticTextToolItem(mToolMenuRootObjectID,GetToolItemCount(mToolMenuRootObjectID),
							 sepatext,false,false,true);
	
	//==================アプリ内ツール==================
	//#427 
	//自動更新ツール表示有効なら自動更新ツールをautoupdateフォルダからロード
	//#844 if( GetData_Bool(kEnableAutoUpdateTool) == true )
	//アプリ組み込みツール開始位置取得
	mToolMenuRoot_AppToolStartIndex = GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mToolMenuRootObjectID) +1;//GetToolItemCount(mToolMenuRootObjectID);
	{
		//autoupdateフォルダ内のtoolフォルダ取得
		AFileAcc	autoUpdateToolRootFolder;
		GetToolFolder(true,autoUpdateToolRootFolder);
		//autoupdateフォルダ内のtoolフォルダの存在判定
		if( autoUpdateToolRootFolder.Exist() == true )
		{
			//「アプリ内ツール（モード：）」というメニュー項目を追加
			AText	staticText;
			staticText.SetLocalizedText("ToolMenuText_AutoUpdateTool");
			staticText.ReplaceParamText('0',modeDisplayName);
			InsertStaticTextToolItem(mToolMenuRootObjectID,GetToolItemCount(mToolMenuRootObjectID),
									 staticText,false,false,true);
			//自動更新ツールをロードする
			LoadToolRecursive(autoUpdateToolRootFolder,mToolMenuRootObjectID,true);
		}
	}
	//アプリ組み込みツール終了位置取得
	mToolMenuRoot_AppToolEndIndex = GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mToolMenuRootObjectID);//GetToolItemCount(mToolMenuRootObjectID);
	
	//元々realize済みだった場合は、メニューrealize
	if( realized == true )
	{
		GetApp().NVI_GetMenuManager().RealizeDynamicMenu(mToolMenuRootObjectID);
	}
}
//各メニュー（ルートメニュー・サブメニュー）ごとのLoad
void	AModePrefDB::LoadToolRecursive( const AFileAcc& inFolder, const AObjectID inDynamicMenuObjectID,
		const ABool inAutoUpdate )//#427
{
	AFileAcc	toolPrefFile;
	toolPrefFile.SpecifyChild(inFolder,"toolpref.mi");
	if( toolPrefFile.Exist() == true )
	{
		//toolpref.miファイルから読み込み
		
		//
		AIndex	toolItemArrayIndex = mToolMenuArray_MenuObjectID.Find(inDynamicMenuObjectID);
		if( toolItemArrayIndex == kIndex_Invalid )
		{
			_ACThrow("invalid menu objectID",NULL);
		}
		
		//toolpref.miファイルから読み込み
		AToolPrefDB	toolPrefDB;
		if( inAutoUpdate == false )
		{
			//ユーザー編集ツールの場合
			//ツールファイルと同じフォルダのtoolpref.miを読み込む
			if( toolPrefFile.Exist() == true )
			{
				toolPrefDB.LoadFromPrefTextFile(toolPrefFile);
			}
		}
		else
		{
			//#427
			//自動更新ツールの場合、autoupdateフォルダ内のtoolpref.miと
			//autoupdate_userdataフォルダ内の対応するフォルダのtoolpref.miとをマージする。
			
			//まず、autoupdate_userdataフォルダ内の対応するフォルダのtoolpref.miを読み込む
			//（存在しなければ読み込まない。toolPrefDBは空となる。）
			AFileAcc	userDataToolPrefFile;
			GetToolPrefFile(toolItemArrayIndex,true,userDataToolPrefFile);
			if( userDataToolPrefFile.Exist() == true )
			{
				toolPrefDB.LoadFromPrefTextFile(userDataToolPrefFile);
			}
			//autoupdateフォルダ内のtoolpref.miを読み込む
			AToolPrefDB	autoUpdateToolPrefDB;
			if( toolPrefFile.Exist() == true )
			{
				autoUpdateToolPrefDB.LoadFromPrefTextFile(toolPrefFile);
			}
			//autoupdateフォルダ内のtoolpref.miの各項目ごとにループ
			for( AIndex autoupdateIndex = 0; 
						autoupdateIndex < autoUpdateToolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName);
						autoupdateIndex++ )
			{
				AText	filename;
				autoUpdateToolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FileName,autoupdateIndex,filename);
				//AutoUpdate側のツールと同じファイル名のツールをUserData側から検索
				AIndex	foundIndex = toolPrefDB.Find_TextArray(AToolPrefDB::kToolArray_FileName,filename);
				if( foundIndex == kIndex_Invalid )
				{
					//UserData側に存在しなかった場合、UserDataへそのまま追加する
					toolPrefDB.InsertRowFromOtherDB_Table(AToolPrefDB::kToolArray_FileName,
								toolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName),
								autoUpdateToolPrefDB,autoupdateIndex);
				}
				else
				{
					//UserData側に存在した場合、多言語ツール名だけをUserDataへコピーする
					AArray<APrefID>	langPrefIDArray;
					AToolPrefDB::GetLanguagePrefIDArray(langPrefIDArray);
					for( AIndex i = 0; i < langPrefIDArray.GetItemCount(); i++ )
					{
						AText	name;
						autoUpdateToolPrefDB.GetData_TextArray(langPrefIDArray.Get(i),autoupdateIndex,name);
						toolPrefDB.SetData_TextArray(langPrefIDArray.Get(i),foundIndex,name);
					}
				}
			}
		}
		
		//フォルダの中のファイルを取得し、childrenへ格納
		AObjectArray<AFileAcc>	children;
		inFolder.GetChildren(children);
		//フォルダの中のファイルについての追加済みフラグ、ファイル名Hash
		AHashTextArray	childrenFilesName;
		ABoolArray	childrenFilesAddedFlag;
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			AText	filename;
			children.GetObject(i).GetFilename(filename);
			childrenFilesName.Add(filename);
			childrenFilesAddedFlag.Add(false);
		}
		
		//ToolPrefの各項目を追加
		AItemCount	prefItemCount = toolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName);
		for( AIndex index = 0; index < prefItemCount; index++ )
		{
			//ツール追加
			AText	filename;
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FileName,index,filename);
			AFileAcc	file;
			file.SpecifyChild(inFolder,filename);
			/*
			if( filename.Compare("_script") == true )
			{
				continue;
			}
			*/
			//toolpref.miに記述された項目のファイルが存在していない場合、
			//その項目は無視する。
			//（2.1.13b1より仕様変更。自動更新時、ツールが削除された場合、autoupdate_userdataフォルダのほうの
			//項目だけが残る可能性がある。この場合、ツールを追加すべきではない。
			//また、ユーザー作成ツールにおいても、空のツールを追加するメリットもない。）#427
			if( file.Exist() == false )//#427   file.CreateFile();
			{
				//この項目は無視して次へ
				continue;
			}
			//ツール追加。toolIndexへ、追加したツールのindexを入れる（indexとは必ずしも一致しない。
			//ルートの場合は自動更新とユーザー定義を連続して入れるため。）。
			AIndex	toolIndex = AddToolItem(inDynamicMenuObjectID,file,false,false,inAutoUpdate);//#427
			//設定
			mToolItemArray_Enabled.GetObject(toolItemArrayIndex).Set(toolIndex,
					toolPrefDB.GetData_BoolArray(AToolPrefDB::kToolArray_Enabled,index));
			mToolItemArray_Shortcut.GetObject(toolItemArrayIndex).Set(toolIndex,
					toolPrefDB.GetData_NumberArray(AToolPrefDB::kToolArray_Shortcut,index));
			AMenuShortcutModifierKeys	modifiers = AKeyWrapper::MakeMenuShortcutModifierKeys(
										toolPrefDB.GetData_BoolArray(AToolPrefDB::kToolArray_ShortcutModifiers_ControlKey,index),
										false,
										toolPrefDB.GetData_BoolArray(AToolPrefDB::kToolArray_ShortcutModifiers_ShiftKey,index));
			mToolItemArray_Modifiers.GetObject(toolItemArrayIndex).Set(toolIndex,modifiers);
			mToolItemArray_Grayout.GetObject(toolItemArrayIndex).Set(toolIndex,
					toolPrefDB.GetData_BoolArray(AToolPrefDB::kToolArray_Grayout,index));
			//他言語タイトル設定
			AText	multiLangTitle;
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_JapaneseName,index,multiLangTitle);
			mToolItemArray_JapaneseName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_EnglishName,index,multiLangTitle);
			mToolItemArray_EnglishName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FrenchName,index,multiLangTitle);
			mToolItemArray_FrenchName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_GermanName,index,multiLangTitle);
			mToolItemArray_GermanName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_SpanishName,index,multiLangTitle);
			mToolItemArray_SpanishName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_ItalianName,index,multiLangTitle);
			mToolItemArray_ItalianName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_DutchName,index,multiLangTitle);
			mToolItemArray_DutchName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_SwedishName,index,multiLangTitle);
			mToolItemArray_SwedishName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_NorwegianName,index,multiLangTitle);
			mToolItemArray_NorwegianName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_DanishName,index,multiLangTitle);
			mToolItemArray_DanishName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FinnishName,index,multiLangTitle);
			mToolItemArray_FinnishName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_PortugueseName,index,multiLangTitle);
			mToolItemArray_PortugueseName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_SimplifiedChineseName,index,multiLangTitle);
			mToolItemArray_SimplifiedChineseName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_TraditionalChineseName,index,multiLangTitle);
			mToolItemArray_TraditionalChineseName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_KoreanName,index,multiLangTitle);
			mToolItemArray_KoreanName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_PolishName,index,multiLangTitle);
			mToolItemArray_PolishName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_PortuguesePortugalName,index,multiLangTitle);
			mToolItemArray_PortuguesePortugalName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_RussianName,index,multiLangTitle);
			mToolItemArray_RussianName.GetObject(toolItemArrayIndex).Set(toolIndex,multiLangTitle);
			
			//ToolPrefのファイル名に対応するファイルが存在したら追加済みフラグOn
			AIndex	childrenFilesIndex = childrenFilesName.Find(filename);
			if( childrenFilesIndex != kIndex_Invalid )
			{
				childrenFilesAddedFlag.Set(childrenFilesIndex,true);
			}
		}
		
		//未追加（ToolPrefに存在しないもの）のファイルを追加
		//Finderや旧バージョンでファイル名を変更した場合に未追加になる。
		//（他言語タイトル等のデータを消さないために、ToolPrefのデータも生かしておく）
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			//追加済みは対象外
			if( childrenFilesAddedFlag.Get(i) == true )   continue;
			//ファイル名取得
			AFileAcc	file = children.GetObject(i);
			AText	filename;
			file.GetFilename(filename);
			//対象外ファイル
			if( filename.GetLength() == 0 )   continue;
			if( filename.Get(0) == '.' )   continue;
			if( filename.Compare("order") == true )   continue;
			if( filename.Compare("Icon\r") == true )   continue;
			if( filename.Compare("toolpref.mi") == true )   continue;
			//if( filename.Compare("_script") == true )   continue;
			//ツール追加
			AddToolItem(inDynamicMenuObjectID,file,false,false,inAutoUpdate);//#427
		}
		
		//メニュー更新
		UpdateToolMenu(inDynamicMenuObjectID);
	}
	//#844 旧.order/orderファイル読み込み処理削除
#if IMPLEMENTATION_FOR_MACOSX
	//旧バージョンで作成したモードデータとの互換性のための処理（バージョン3.0.0b1で削除していたが、引き継ぎできないという不具合報告があったので、やはり復活する。#949）
	//自動更新データはtoolpref.mi必須を前提とする。（toolpref.miが無いバージョンは既にかなり古いので。）
	else
	{
		//旧バージョンorderファイルから読み込み
		
		//フォルダの中のファイルを取得し、childrenへ格納
		AObjectArray<AFileAcc>	children;
		inFolder.GetChildren(children);
		
		//"order"ファイルに従って順番入れ替え
		AText	orderText;
		AFileAcc	orderFile;
		orderFile.SpecifyChild(inFolder,".order");
		if( orderFile.Exist() == false )
		{
			orderFile.SpecifyChild(inFolder,"order");
		}
		GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_ToolOrderFile,orderFile,orderText);
		//R0173
		ABoolArray	enableArray;
		ABoolArray	grayoutArray;//#129
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			enableArray.Add(true);
			grayoutArray.Add(false);//#129
		}
		//
		AIndex	pos = 0;//Textのpos
		AIndex	n = 0;//現在の順番
		while( pos < orderText.GetLength() )
		{
			//orderファイルからlineへ１行取得
			AText	line;
			orderText.GetLine(pos,line);
			//R0173
			ABool	enabled = true;
			ABool	grayout = false;//#129
			if( line.GetItemCount() > 2 )
			{
				if( line.Get(0) == '%' && line.Get(1) == '%' )
				{
					enabled = false;
					line.Delete(0,2);
				}
				//#129
				if( line.Get(0) == '%' && line.Get(1) == ')' )
				{
					grayout = true;
					line.Delete(0,2);
				}
			}
			//childrenを検索して、lineと一致するファイル名があったら、
			//childrenのn番目の位置へ、それを移動する。
			for( AIndex i = 0; i < children.GetItemCount(); i++ )
			{
				AText	filename;
				children.GetObject(i).GetFilename(filename);
				if( filename.Compare(line) == true )
				{
					children.SwapObject(n,i);
					enableArray.Set(n,enabled);//R0173
					grayoutArray.Set(n,grayout);//#129
					break;
				}
			}
			n++;
			if( n >= children.GetItemCount() )   break;
		}
		
		//R0173
		AIndex	toolItemArrayIndex = mToolMenuArray_MenuObjectID.Find(inDynamicMenuObjectID);
		if( toolItemArrayIndex == kIndex_Invalid )
		{
			_ACThrow("invalid menu objectID",NULL);
		}
		//メニューの構築
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			//ファイル名取得
			AFileAcc	file = children.GetObject(i);
			AText	filename;
			file.GetFilename(filename);
			//対象外ファイル
			if( filename.GetLength() == 0 )   continue;
			if( filename.Get(0) == '.' )   continue;
			if( filename.Compare("order") == true )   continue;
			if( filename.Compare("Icon\r") == true )   continue;
			if( filename.Compare("toolpref.mi") == true )   continue;//#305
			//ツール追加
			AddToolItem(inDynamicMenuObjectID,file,false,false,inAutoUpdate);//#427
			//R0173
			mToolItemArray_Enabled.GetObject(toolItemArrayIndex).Set(GetToolItemCount(inDynamicMenuObjectID)-1,enableArray.Get(i));
			//#129
			mToolItemArray_Grayout.GetObject(toolItemArrayIndex).Set(GetToolItemCount(inDynamicMenuObjectID)-1,grayoutArray.Get(i));
			//#305 ショートカット設定（InsertToolItem()から移動。TCMDリソースから読み出し）
			//（バージョン3.0.0b1で削除していたが、引き継ぎできないという不具合報告があったので、やはり復活する。#949）
			ANumber	shortcut = NULL;
			ABool	shiftKey = false;
			ABool	controlKey = false;
			ABool	optionKey =false;
			LoadTool_LegacyShortcutData(file,shortcut,shiftKey,controlKey,optionKey);
			AMenuShortcutModifierKeys	modifiers = AKeyWrapper::MakeMenuShortcutModifierKeys(controlKey,optionKey,shiftKey);
			mToolItemArray_Shortcut.GetObject(toolItemArrayIndex).Set(GetToolItemCount(inDynamicMenuObjectID)-1,shortcut);
			mToolItemArray_Modifiers.GetObject(toolItemArrayIndex).Set(GetToolItemCount(inDynamicMenuObjectID)-1,modifiers);
		}
		//メニュー更新
		UpdateToolMenu(inDynamicMenuObjectID);
		//orderファイル書き込み #949
		WriteToolOrderFile(inDynamicMenuObjectID);
	}
#endif
}

//ツール項目追加（最後の項目として追加）
AIndex	AModePrefDB::AddToolItem( const AObjectID inMenuObjectID, const AFileAcc& inFile, 
		const ABool inWriteOrderFile, const ABool inUpdateMenu, const ABool inAutoUpdate )//B0321 #427
{
	AIndex	index = GetToolItemCount(inMenuObjectID);
	InsertToolItem(inMenuObjectID,index,inFile,inWriteOrderFile,inUpdateMenu,inAutoUpdate);
	return index;
}

/**
ツール項目追加
toolpref.miからのデータ読み込みはしない。shortcutデータ等は空のままになる。
*/
void	AModePrefDB::InsertToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex, const AFileAcc& inFile, 
		const ABool inWriteOrderFile, const ABool inUpdateMenu, const ABool inAutoUpdate )//#427
{
	//R0173
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//menutext, actiontext取得
	AText	menutext, actiontext;
	inFile.GetFilename(menutext);
	//高速化のためGetPath()へ変更inFile.GetNormalizedPath(actiontext);
	inFile.GetPath(actiontext);
	//menutextから拡張子を消去
	AText	suffix;
	menutext.GetSuffix(suffix);
	if( suffix.GetLength() > 0 )
	{
		menutext.Delete(menutext.GetLength()-suffix.GetLength(),suffix.GetLength());
	}
	//フォルダ／ファイル判定
	if( inFile.IsFolder() == true && inFile.IsBundleFolder() == false )//R0137
	{
		//フォルダーならサブメニューを生成
		AMenuItemID	menuItemID = kMenuItemID_Tool;
		if( mModeID == kStandardModeIndex )
		{
			menuItemID = kMenuItemID_Tool_StandardMode;
		}
		AObjectID	subMenuObjectID = GetApp().NVI_GetMenuManager().CreateDynamicMenu(menuItemID,false);
		//R0173 現在のフォルダのarrayに項目挿入
		mToolItemArray_File.GetObject(arrayIndex).InsertNew1Object(inItemIndex);
		mToolItemArray_File.GetObject(arrayIndex).GetObject(inItemIndex).CopyFrom(inFile);
		mToolItemArray_SubMenuObjectID.GetObject(arrayIndex).Insert1(inItemIndex,subMenuObjectID);
		mToolItemArray_Enabled.GetObject(arrayIndex).Insert1(inItemIndex,true);
		mToolItemArray_Shortcut.GetObject(arrayIndex).Insert1(inItemIndex,0);
		mToolItemArray_Modifiers.GetObject(arrayIndex).Insert1(inItemIndex,0);
		mToolItemArray_Grayout.GetObject(arrayIndex).Insert1(inItemIndex,false);//#129
		mToolItemArray_AutoUpdate.GetObject(arrayIndex).Insert1(inItemIndex,inAutoUpdate);//#427
		mToolItemArray_StaticText.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());//#427
		//#305
		mToolItemArray_JapaneseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_EnglishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_FrenchName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_GermanName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_SpanishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_ItalianName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_DutchName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_SwedishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_NorwegianName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_DanishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_FinnishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_PortugueseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_SimplifiedChineseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_TraditionalChineseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_KoreanName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_PolishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_PortuguesePortugalName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_RussianName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		//R0173 新規array追加
		mToolMenuArray_MenuObjectID.Add(subMenuObjectID);
		mToolMenuArray_Folder.GetObject(mToolMenuArray_Folder.AddNewObject()).CopyFrom(inFile);//#305
		mToolMenuArray_AutoUpdateFolder.Add(inAutoUpdate);//#427
		mToolItemArray_File.AddNewObject();
		mToolItemArray_SubMenuObjectID.AddNewObject();
		mToolItemArray_Enabled.AddNewObject();
		mToolItemArray_Shortcut.AddNewObject();
		mToolItemArray_Modifiers.AddNewObject();
		mToolItemArray_Grayout.AddNewObject();//#129
		mToolItemArray_AutoUpdate.AddNewObject();//#427
		mToolItemArray_StaticText.AddNewObject();//#427
		//#305
		mToolItemArray_JapaneseName.AddNewObject();
		mToolItemArray_EnglishName.AddNewObject();
		mToolItemArray_FrenchName.AddNewObject();
		mToolItemArray_GermanName.AddNewObject();
		mToolItemArray_SpanishName.AddNewObject();
		mToolItemArray_ItalianName.AddNewObject();
		mToolItemArray_DutchName.AddNewObject();
		mToolItemArray_SwedishName.AddNewObject();
		mToolItemArray_NorwegianName.AddNewObject();
		mToolItemArray_DanishName.AddNewObject();
		mToolItemArray_FinnishName.AddNewObject();
		mToolItemArray_PortugueseName.AddNewObject();
		mToolItemArray_SimplifiedChineseName.AddNewObject();
		mToolItemArray_TraditionalChineseName.AddNewObject();
		mToolItemArray_KoreanName.AddNewObject();
		mToolItemArray_PolishName.AddNewObject();
		mToolItemArray_PortuguesePortugalName.AddNewObject();
		mToolItemArray_RussianName.AddNewObject();
		//サブフォルダ読み込み
		LoadToolRecursive(inFile,subMenuObjectID,inAutoUpdate);//サブフォルダの自動更新フラグは親を引き継ぎ
	}
	else
	{
		//ファイルなら、コマンドショートカット設定を読み出したのち、メニューに項目追加。
		/*#305 LoadToolRecursive()の旧データ読み込みの箇所に移動
		ANumber	shortcut = NULL;
		ABool	shiftKey = false;
		ABool	controlKey = false;
		ABool	optionKey =false;
#if IMPLEMENTATION_FOR_MACOSX
		LoadTool_LegacyShortcutData(inFile,shortcut,shiftKey,controlKey,optionKey);
#endif
		AMenuShortcutModifierKeys	modifiers = AKeyWrapper::MakeMenuShortcutModifierKeys(controlKey,optionKey,shiftKey);
		*/
		//R0173 現在のフォルダのarrayに項目挿入
		mToolItemArray_File.GetObject(arrayIndex).InsertNew1Object(inItemIndex);
		mToolItemArray_File.GetObject(arrayIndex).GetObject(inItemIndex).CopyFrom(inFile);
		mToolItemArray_SubMenuObjectID.GetObject(arrayIndex).Insert1(inItemIndex,kObjectID_Invalid);
		mToolItemArray_Enabled.GetObject(arrayIndex).Insert1(inItemIndex,true);
		mToolItemArray_Shortcut.GetObject(arrayIndex).Insert1(inItemIndex,0);//#305 shortcut);
		mToolItemArray_Modifiers.GetObject(arrayIndex).Insert1(inItemIndex,0);//#305 modifiers);
		mToolItemArray_Grayout.GetObject(arrayIndex).Insert1(inItemIndex,false);//#129
		mToolItemArray_AutoUpdate.GetObject(arrayIndex).Insert1(inItemIndex,inAutoUpdate);//#427
		mToolItemArray_StaticText.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());//#427
		//#305
		mToolItemArray_JapaneseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_EnglishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_FrenchName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_GermanName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_SpanishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_ItalianName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_DutchName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_SwedishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_NorwegianName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_DanishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_FinnishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_PortugueseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_SimplifiedChineseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_TraditionalChineseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_KoreanName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_PolishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_PortuguesePortugalName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
		mToolItemArray_RussianName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	}
	//orderファイル書き込み
	if( inWriteOrderFile == true )
	{
		WriteToolOrderFile(inMenuObjectID);
	}
	//
	if( inUpdateMenu == true )
	{
		UpdateToolMenu(inMenuObjectID);
	}
	//ルートの場合、ユーザーツール開始位置等をずらす
	if( inMenuObjectID == mToolMenuRootObjectID )
	{
		if( inItemIndex <= mToolMenuRoot_UserToolEndIndex )
		{
			mToolMenuRoot_UserToolEndIndex++;
		}
		if( inItemIndex <= mToolMenuRoot_AppToolStartIndex )
		{
			mToolMenuRoot_AppToolStartIndex++;
		}
		if( inItemIndex <= mToolMenuRoot_AppToolEndIndex )
		{
			mToolMenuRoot_AppToolEndIndex++;
		}
	}
}

//#427
/**
StaticTextツール（メニューに説明文字列を表示するため）を挿入
*/
void	AModePrefDB::InsertStaticTextToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex, 
		const AText& inStaticText,
		const ABool inWriteOrderFile, const ABool inUpdateMenu, const ABool inAutoUpdate )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//ツールデータ
	mToolItemArray_File.GetObject(arrayIndex).InsertNew1Object(inItemIndex);
	mToolItemArray_SubMenuObjectID.GetObject(arrayIndex).Insert1(inItemIndex,kObjectID_Invalid);
	mToolItemArray_Enabled.GetObject(arrayIndex).Insert1(inItemIndex,true);
	mToolItemArray_Shortcut.GetObject(arrayIndex).Insert1(inItemIndex,0);
	mToolItemArray_Modifiers.GetObject(arrayIndex).Insert1(inItemIndex,0);
	mToolItemArray_Grayout.GetObject(arrayIndex).Insert1(inItemIndex,true);
	mToolItemArray_AutoUpdate.GetObject(arrayIndex).Insert1(inItemIndex,inAutoUpdate);
	mToolItemArray_StaticText.GetObject(arrayIndex).Insert1(inItemIndex,inStaticText);
	//言語毎タイトル
	mToolItemArray_JapaneseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_EnglishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_FrenchName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_GermanName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_SpanishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_ItalianName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_DutchName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_SwedishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_NorwegianName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_DanishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_FinnishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_PortugueseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_SimplifiedChineseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_TraditionalChineseName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_KoreanName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_PolishName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_PortuguesePortugalName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	mToolItemArray_RussianName.GetObject(arrayIndex).Insert1(inItemIndex,GetEmptyText());
	//orderファイル書き込み
	if( inWriteOrderFile == true )
	{
		WriteToolOrderFile(inMenuObjectID);
	}
	//
	if( inUpdateMenu == true )
	{
		UpdateToolMenu(inMenuObjectID);
	}
}

//B0406
//ツール項目削除（ファイル・フォルダは削除しない）
void	AModePrefDB::DeleteToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex,
		const ABool inWriteOrderFile, const ABool inUpdateMenu )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//submenuが存在する場合はサブメニュー削除
	AObjectID	subMenuID = mToolItemArray_SubMenuObjectID.GetObjectConst(arrayIndex).Get(inItemIndex);
	if( subMenuID != kObjectID_Invalid )
	{
		//サブメニュー内の各項目削除
		while( GetToolItemCount(subMenuID) > 0 )
		{
			DeleteToolItem(subMenuID,0,false,false);
		}
		//サブメニューのarray削除
		AIndex	subMenuArrayIndex = mToolMenuArray_MenuObjectID.Find(subMenuID);
		mToolMenuArray_MenuObjectID.Delete1(subMenuArrayIndex);
		mToolMenuArray_Folder.Delete1Object(subMenuArrayIndex);//#305
		mToolMenuArray_AutoUpdateFolder.Delete1(subMenuArrayIndex);//#427
		mToolItemArray_File.Delete1Object(subMenuArrayIndex);
		mToolItemArray_SubMenuObjectID.Delete1Object(subMenuArrayIndex);
		mToolItemArray_Enabled.Delete1Object(subMenuArrayIndex);
		mToolItemArray_Shortcut.Delete1Object(subMenuArrayIndex);
		mToolItemArray_Modifiers.Delete1Object(subMenuArrayIndex);
		mToolItemArray_Grayout.Delete1Object(subMenuArrayIndex);//#129
		mToolItemArray_AutoUpdate.Delete1Object(subMenuArrayIndex);//#427
		mToolItemArray_StaticText.Delete1Object(subMenuArrayIndex);//#427
		//#305
		mToolItemArray_JapaneseName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_EnglishName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_FrenchName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_GermanName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_SpanishName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_ItalianName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_DutchName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_SwedishName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_NorwegianName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_DanishName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_FinnishName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_PortugueseName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_SimplifiedChineseName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_TraditionalChineseName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_KoreanName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_PolishName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_PortuguesePortugalName.Delete1Object(subMenuArrayIndex);
		mToolItemArray_RussianName.Delete1Object(subMenuArrayIndex);
	}
	//ツール項目データ削除
	mToolItemArray_File.GetObject(arrayIndex).Delete1Object(inItemIndex);
	mToolItemArray_SubMenuObjectID.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_Enabled.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_Shortcut.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_Modifiers.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_Grayout.GetObject(arrayIndex).Delete1(inItemIndex);//#129
	mToolItemArray_AutoUpdate.GetObject(arrayIndex).Delete1(inItemIndex);//#427
	mToolItemArray_StaticText.GetObject(arrayIndex).Delete1(inItemIndex);//#427
	//#305
	mToolItemArray_JapaneseName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_EnglishName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_FrenchName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_GermanName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_SpanishName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_ItalianName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_DutchName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_SwedishName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_NorwegianName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_DanishName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_FinnishName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_PortugueseName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_SimplifiedChineseName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_TraditionalChineseName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_KoreanName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_PolishName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_PortuguesePortugalName.GetObject(arrayIndex).Delete1(inItemIndex);
	mToolItemArray_RussianName.GetObject(arrayIndex).Delete1(inItemIndex);
	//orderファイル書き込み
	if( inWriteOrderFile == true )
	{
		WriteToolOrderFile(inMenuObjectID);
	}
	//
	if( inUpdateMenu == true )
	{
		UpdateToolMenu(inMenuObjectID);
	}
	//ルートの場合、ユーザーツール開始位置等をずらす
	if( inMenuObjectID == mToolMenuRootObjectID )
	{
		if( inItemIndex <= mToolMenuRoot_UserToolEndIndex )
		{
			mToolMenuRoot_UserToolEndIndex--;
		}
		if( inItemIndex <= mToolMenuRoot_AppToolStartIndex )
		{
			mToolMenuRoot_AppToolStartIndex--;
		}
		if( inItemIndex <= mToolMenuRoot_AppToolEndIndex )
		{
			mToolMenuRoot_AppToolEndIndex--;
		}
	}
}

//R0173
//ツール項目数取得
AItemCount	AModePrefDB::GetToolItemCount( const AObjectID inMenuObjectID ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	return mToolItemArray_File.GetObjectConst(arrayIndex).GetItemCount();
}

//ファイルからIndexを検索する B0406 R0173
ABool	AModePrefDB::FindToolIndexByFile( const AFileAcc& inFile, AIndex& outArrayIndex, AIndex& outItemIndex ) const
{
	outArrayIndex = kIndex_Invalid;
	outItemIndex = kIndex_Invalid;
	AText	path;
	inFile.GetNormalizedPath(path);
	AText	p;
	for( AIndex index = 0; index < mToolItemArray_File.GetItemCount(); index++ )
	{
		AItemCount	itemcount = mToolItemArray_File.GetObjectConst(index).GetItemCount();
		for( AIndex i = 0; i < itemcount; i++ )
		{
			mToolItemArray_File.GetObjectConst(index).GetObjectConst(i).GetNormalizedPath(p);
			if( p.Compare(path) == true )
			{
				outArrayIndex = index;
				outItemIndex = i;
				return true;
			}
		}
	}
	return false;
}

/**
指定ツールファイルのメニューobject ID、および、ルートからのメニューobject ID配列（ルートがindex 0、親メニューが最後の項目）を返す
*/
ABool	AModePrefDB::GetToolMenuObjectIDFromFile( const AFileAcc& inFile, 
												 AObjectID& outMenuObjectID, AArray<AObjectID>& outMenuObjectIDArray ) const
{
	//結果初期化
	outMenuObjectID = kObjectID_Invalid;
	outMenuObjectIDArray.DeleteAll();
	//指定ツールファイルからメニューobject IDを取得
	AIndex	arrayIndex = kIndex_Invalid;
	AIndex	itemIndex = kIndex_Invalid;
	if( FindToolIndexByFile(inFile,arrayIndex,itemIndex) == false )
	{
		//ツールファイルが見つからなければリターン
		return false;
	}
	//メニューobject ID取得
	outMenuObjectID = mToolMenuArray_MenuObjectID.Get(arrayIndex);
	
	//指定ツールファイルの親フォルダを取得
	AFileAcc	folder;
	folder.SpecifyParent(inFile);
	//フォルダから、メニューobject IDを取得（親方向をたどって、見つからなくなるまで繰り返す）
	while( FindToolIndexByFile(folder,arrayIndex,itemIndex) == true )
	{
		//結果に追加
		outMenuObjectIDArray.Insert1(0,mToolMenuArray_MenuObjectID.Get(arrayIndex));
		//親フォルダ取得
		AFileAcc	f;
		f = folder;
		folder.SpecifyParent(f);
	}
	return true;
}

/*
ABool	AModePrefDB::FindToolArrayIndexByFile( const AFileAcc inFolder, AIndex& outArrayIndex ) const
{
	outArrayIndex = kIndex_Invalid;
	//
	AText	path;
	inFolder.GetNormalizedPath(path);
	AText	p;
	for( AIndex arrayIndex = 0; arrayIndex < mToolMenuArray_Folder.GetItemCount(); arrayIndex++ )
	{
		mToolMenuArray_Folder.GetObjectConst(arrayIndex).GetNormalizedPath(p);
		if( p.Compare(path) == true )
		{
			outArrayIndex = arrayIndex;
			return true;
		}
	}
	return false;
}
*/

//R0173
//ツールメニュー構成
void	AModePrefDB::UpdateToolMenu( const AObjectID inMenuObjectID )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByObjectID(inMenuObjectID);
	for( AIndex i = 0; i < mToolItemArray_File.GetObjectConst(arrayIndex).GetItemCount(); i++ )
	{
		if( mToolItemArray_Enabled.GetObject(arrayIndex).Get(i) == true )
		{
			AText	menutext, actiontext;
			mToolItemArray_File.GetObject(arrayIndex).GetObject(i).GetFilename(menutext);
			menutext.DeleteAfter(menutext.GetSuffixStartPos());//B0000 メニュー表示は拡張子以降削除
			mToolItemArray_File.GetObject(arrayIndex).GetObject(i).GetNormalizedPath(actiontext);
			//#305 メニュー表示文字列取得　優先順位：現在の言語→英語→ファイル名
			GetToolDisplayName(inMenuObjectID,i,menutext);
			//メニュー項目追加
			AMenuShortcutModifierKeys	shortcutmod = mToolItemArray_Modifiers.GetObject(arrayIndex).Get(i);//win
#if IMPLEMENTATION_FOR_WINDOWS
			//Windows版の差分：
			//Controlチェックボックス→Altチェックボックスに変更
			//Shiftチェックボックス→Shiftチェックボックスに変更
			//Ctrlは常に必要とする
			if( (shortcutmod&kMenuShoftcutModifierKeysMask_Control) == 0 )
			{
				shortcutmod &= ~kMenuShoftcutModifierKeysMask_Alt;
			}
			else
			{
				shortcutmod |= kMenuShoftcutModifierKeysMask_Alt;
			}
			shortcutmod |= kModifierKeysMask_Control;
#endif
			//#427
			ABool	separator = false;
			if( menutext.CompareMin("-") == true )
			{
				separator = true;
			}
			//
			GetApp().NVI_GetMenuManager().InsertDynamicMenuItemByObjectID(inMenuObjectID,GetToolMenuItemIndex(inMenuObjectID,i),
						menutext,actiontext,mToolItemArray_SubMenuObjectID.GetObject(arrayIndex).Get(i),
						mToolItemArray_Shortcut.GetObject(arrayIndex).Get(i),
						/*win mToolItemArray_Modifiers.GetObject(arrayIndex).Get(i)*/shortcutmod,//#427false,
						separator,//#427
						(mToolItemArray_Grayout.GetObject(arrayIndex).Get(i)==false));//#129
		}
	}
}

//R0173
//ToolItemIndexからメニューのIndexを取得（enabledでない項目を除いたインデックス）
AIndex	AModePrefDB::GetToolMenuItemIndex( const AObjectID inMenuObjectID, const AIndex inToolItemIndex ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	AIndex	menuItemIndex = 0;
	for( AIndex i = 0; i < inToolItemIndex; i++ )
	{
		if( mToolItemArray_Enabled.GetObjectConst(arrayIndex).Get(i) == true )
		{
			menuItemIndex++;
		}
	}
	return menuItemIndex;
}

/**
ユーザーツール挿入位置を取得
*/
AIndex	AModePrefDB::GetUserToolInsertIndex( const AObjectID inMenuObjectID ) const
{
	if( inMenuObjectID == mToolMenuRootObjectID )
	{
		//ルートの場合は記憶していた位置
		return mToolMenuRoot_UserToolEndIndex;
	}
	else
	{
		//ルート以外の場合は最後
		return GetToolItemCount(inMenuObjectID);
	}
}

/**
key toolの開始時のメニューを取得
*/
void	AModePrefDB::GetKeyToolStartMenu( const AIndex inSyntaxPartIndex, const ABool inUserTool,
										 AObjectID& outMenuObjectID, 
										 AIndex& outStartIndex, AIndex& outEndIndex ) const
{
	/*
	//ルート取得
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(mToolMenuRootObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	*/
	//inUserToolに応じて、アプリ組み込み、ユーザーいずれかの位置を取得
	AIndex	startIndex = mToolMenuRoot_AppToolStartIndex;
	AIndex	endIndex = mToolMenuRoot_AppToolEndIndex;
	if( inUserTool == true )
	{
		startIndex = mToolMenuRoot_UserToolStartIndex;
		endIndex = mToolMenuRoot_UserToolEndIndex;
	}
	//inSyntaxPartIndex（文法パート）の指定があれば、その文法パート名のフォルダから開始する
	if( inSyntaxPartIndex != kIndex_Invalid )
	{
		//文法パート名取得
		AText	syntaxPartName;
		mSyntaxDefinition.GetSyntaxPartSeparatorName(inSyntaxPartIndex,syntaxPartName);
		//ルートメニューの各項目毎のループ
		for( AIndex i = startIndex; i < endIndex; i++ )
		{
			AText	filename;
			//mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(i).GetFilename(filename);
			GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mToolMenuRootObjectID,i,filename);
			if( filename.Compare(syntaxPartName) == true )
			{
				//------------------パート名と同じ項目を見つけた場合------------------
				outMenuObjectID = GetApp().NVI_GetMenuManager().
						GetDynamicMenuItemSubMenuObjectIDByObjectID(mToolMenuRootObjectID,i);//mToolItemArray_SubMenuObjectID.GetObjectConst(arrayIndex).Get(i);
				if( outMenuObjectID != kObjectID_Invalid )
				{
					outStartIndex = 0;
					outEndIndex = GetApp().NVI_GetMenuManager().
							GetDynamicMenuItemCountByObjectID(outMenuObjectID);
					return;
				}
			}
		}
	}
	//文法パートの指定が無い場合、もしくは、その文法パート名のフォルダが無い場合は、ルートから開始
	outMenuObjectID = mToolMenuRootObjectID;
	outStartIndex = startIndex;
	outEndIndex = endIndex;
}

#pragma mark ---ツール（メニューへのAttach/Detach）

void	AModePrefDB::AttachToolMenu()
{
	if( mModeID != kStandardModeIndex )
	{
		GetApp().NVI_GetMenuManager().RealizeDynamicMenu(mToolMenuRootObjectID);
	}
}

void	AModePrefDB::DetachToolMenu()
{
	if( mModeID != kStandardModeIndex )
	{
		GetApp().NVI_GetMenuManager().UnrealizeDynamicMenu(mToolMenuRootObjectID);
	}
}

#pragma mark ---ツール（設定）

//ファイルとともにツール項目削除 R0173
void	AModePrefDB::DeleteToolItemAndFile( const AObjectID inMenuObjectID, const AIndex inItemIndex )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//ゴミ箱へ
	AFileAcc	file;
	file.CopyFrom(mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(inItemIndex));
	ALaunchWrapper::MoveToTrash(file);
	//ツール項目削除
	DeleteToolItem(inMenuObjectID,inItemIndex,true,true);
	//ToolPrefファイル書き込み #305
	WriteToolOrderFile(inMenuObjectID);
}

//ツール名称変更
ABool	AModePrefDB::SetToolName( const AFileAcc& inFile, const AText& inNewName, AFileAcc& outNewFile )
{
	outNewFile.CopyFrom(inFile);
	//ファイルからIndexを検索
	AIndex	arrayIndex = kIndex_Invalid;
	AIndex	itemIndex = kIndex_Invalid;
	if( FindToolIndexByFile(inFile,arrayIndex,itemIndex) == false )
	{
		_ACError("invalid tool file",NULL);
		return false;
	}
	AObjectID	menuObjectID = mToolMenuArray_MenuObjectID.Get(arrayIndex);
	//B0328 ファイル名が変わらないときは終了
	AText	filename;
	inFile.GetFilename(filename);
	if( filename.Compare(inNewName) == true )   return false;
	//ファイル名変更
	if( outNewFile.Rename(inNewName) == false )   return false;
	/*#305 DeleteしてInsertだと、各言語データが消えてしまうため（Insertでは、toolpref.miの内容が読み込まれない）
	//項目を削除して追加
	DeleteToolItem(menuObjectID,itemIndex,false,false);
	InsertToolItem(menuObjectID,itemIndex,outNewFile,true,true);
	*/
	
	//ファイル名データ変更
	mToolItemArray_File.GetObject(arrayIndex).GetObject(itemIndex).CopyFrom(outNewFile);
	//メニュー更新
	UpdateToolMenu(menuObjectID);
	//ToolPrefファイル書き込み #305
	WriteToolOrderFile(menuObjectID);
	
	/*#427 従来のコードは名称変更するツールと同じ階層のツールを全削除して、再読み込みしていた。
	今回、ルートの場合、自動更新／ユーザーツールが混在するので、このコードはNG。
	ルートの場合は全体を再度Loadするように変更。
	//変更したファイルのフォルダ以下を全て再読込（サブフォルダのパス名が変更されるため）
	AFileAcc	folder;
	folder.SpecifyParent(outNewFile);
	while( GetToolItemCount(menuObjectID) > 0 )
	{
		DeleteToolItem(menuObjectID,0,false,false);
	}
	LoadToolRecursive(folder,menuObjectID);
	*/
	if( outNewFile.IsFolder() == true )
	{
		if( menuObjectID == mToolMenuRootObjectID )
		{
			LoadTool();
		}
		else
		{
			AFileAcc	folder;
			folder.SpecifyParent(outNewFile);
			while( GetToolItemCount(menuObjectID) > 0 )
			{
				DeleteToolItem(menuObjectID,0,false,false);
			}
			LoadToolRecursive(folder,menuObjectID,false);
		}
	}
	
	return true;
}

//ショートカット設定 B0406
void	AModePrefDB::SetToolShortcut( const AFileAcc& inFile, const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers )
{
	//ファイルからIndexを検索
	AIndex	arrayIndex = kIndex_Invalid;
	AIndex	itemIndex = kIndex_Invalid;
	if( FindToolIndexByFile(inFile,arrayIndex,itemIndex) == false )
	{
		_ACError("invalid tool file",NULL);
		return;
	}
	AObjectID	menuObjectID = mToolMenuArray_MenuObjectID.Get(arrayIndex);
	//データ設定
	mToolItemArray_Shortcut.GetObject(arrayIndex).Set(itemIndex,inShortcut);
	mToolItemArray_Modifiers.GetObject(arrayIndex).Set(itemIndex,inModifiers);
	//ファイルに設定
	/*#305
#if IMPLEMENTATION_FOR_MACOSX
	WriteTool_LegacyShortcutData(inFile,inShortcut,
			AKeyWrapper::IsShiftKeyOnMenuShortcut(inModifiers),AKeyWrapper::IsControlKeyOnMenuShortcut(inModifiers));
#endif
	*/
	//ToolPrefファイル書き込み #305
	WriteToolOrderFile(menuObjectID);
	//メニュー更新
	UpdateToolMenu(menuObjectID);
}

//R0173
void	AModePrefDB::SetEnableToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex, const ABool inEnabled )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//
	mToolItemArray_Enabled.GetObject(arrayIndex).Set(inItemIndex,inEnabled);
	//orderファイル書き込み
	WriteToolOrderFile(inMenuObjectID);
	//メニュー更新
	UpdateToolMenu(inMenuObjectID);
	//ToolPrefファイル書き込み #305
	WriteToolOrderFile(inMenuObjectID);
}

//#129
void	AModePrefDB::SetGrayoutToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex, const ABool inGrayout )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//
	mToolItemArray_Grayout.GetObject(arrayIndex).Set(inItemIndex,inGrayout);
	//orderファイル書き込み
	WriteToolOrderFile(inMenuObjectID);
	//メニュー更新
	UpdateToolMenu(inMenuObjectID);
}

void	AModePrefDB::MoveToolItem( const AObjectID inMenuObjectID, const AIndex inOldIndex, const AIndex inNewIndex )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//データ移動
	mToolItemArray_File.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_SubMenuObjectID.GetObject(arrayIndex).Move(inOldIndex,inNewIndex);
	mToolItemArray_Enabled.GetObject(arrayIndex).Move(inOldIndex,inNewIndex);
	mToolItemArray_Shortcut.GetObject(arrayIndex).Move(inOldIndex,inNewIndex);
	mToolItemArray_Modifiers.GetObject(arrayIndex).Move(inOldIndex,inNewIndex);
	mToolItemArray_Grayout.GetObject(arrayIndex).Move(inOldIndex,inNewIndex);//#129
	mToolItemArray_AutoUpdate.GetObject(arrayIndex).Move(inOldIndex,inNewIndex);//#427
	mToolItemArray_StaticText.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);//#427
	//#305
	mToolItemArray_JapaneseName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_EnglishName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_FrenchName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_GermanName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_SpanishName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_ItalianName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_DutchName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_SwedishName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_NorwegianName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_DanishName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_FinnishName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_PortugueseName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_SimplifiedChineseName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_TraditionalChineseName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_KoreanName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_PolishName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_PortuguesePortugalName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	mToolItemArray_RussianName.GetObject(arrayIndex).MoveObject(inOldIndex,inNewIndex);
	//orderファイル書き込み
	WriteToolOrderFile(inMenuObjectID);
	//メニュー更新
	UpdateToolMenu(inMenuObjectID);
}

//#305
/**
ツール多言語タイトル設定
*/
void	AModePrefDB::SetToolMultiLanguageName( const AObjectID inMenuObjectID, const AIndex inItemIndex, 
		const ALanguage inLanguage, const AText& inName )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//
	switch(inLanguage)
	{
	  case kLanguage_Japanese:
		{
			mToolItemArray_JapaneseName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_English:
		{
			mToolItemArray_EnglishName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_French:
		{
			mToolItemArray_FrenchName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_German:
		{
			mToolItemArray_GermanName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Spanish:
		{
			mToolItemArray_SpanishName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Italian:
		{
			mToolItemArray_ItalianName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Dutch:
		{
			mToolItemArray_DutchName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Swedish:
		{
			mToolItemArray_SwedishName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Norwegian:
		{
			mToolItemArray_NorwegianName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Danish:
		{
			mToolItemArray_DanishName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Finnish:
		{
			mToolItemArray_FinnishName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Portuguese:
		{
			mToolItemArray_PortugueseName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_SimplifiedChinese:
		{
			mToolItemArray_SimplifiedChineseName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_TraditionalChinese:
		{
			mToolItemArray_TraditionalChineseName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Korean:
		{
			mToolItemArray_KoreanName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Polish:
		{
			mToolItemArray_PolishName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_PortuguesePortugal:
		{
			mToolItemArray_PortuguesePortugalName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Russian:
		{
			mToolItemArray_RussianName.GetObject(arrayIndex).Set(inItemIndex,inName);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//orderファイル書き込み
	WriteToolOrderFile(inMenuObjectID);
	//メニュー更新
	UpdateToolMenu(inMenuObjectID);
}

#pragma mark ---ツール（取得）

//MenuObjectIDとIndexからファイルを取得 B0406
void	AModePrefDB::GetToolFile( const AObjectID inMenuObjectID, const AIndex inItemIndex, AFileAcc& outFile ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	outFile.CopyFrom(mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(inItemIndex));
}

//ショートカット取得
void	AModePrefDB::GetToolShortcut( const AObjectID inMenuObjectID, const AIndex inItemIndex, 
			ANumber& outShortcut, AMenuShortcutModifierKeys& outModifiers ) const //#427 引数をほかと統一
{
	outShortcut = 0;
	outModifiers = 0;
	//Indexを検索
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//取得
	outShortcut = mToolItemArray_Shortcut.GetObjectConst(arrayIndex).Get(inItemIndex);
	outModifiers =mToolItemArray_Modifiers.GetObjectConst(arrayIndex).Get(inItemIndex);
}

//MenuObjectIDとIndexからサブメニュー取得 R0173
AObjectID	AModePrefDB::GetToolSubMenuID( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	return mToolItemArray_SubMenuObjectID.GetObjectConst(arrayIndex).Get(inItemIndex);
}

//enable取得 R0173
ABool	AModePrefDB::IsToolEnabled( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	return mToolItemArray_Enabled.GetObjectConst(arrayIndex).Get(inItemIndex);
}

//#129
ABool	AModePrefDB::IsToolGrayout( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	return mToolItemArray_Grayout.GetObjectConst(arrayIndex).Get(inItemIndex);
}

//#305
/**
ツール多言語タイトル取得
*/
void	AModePrefDB::GetToolMultiLanguageName( const AObjectID inMenuObjectID, const AIndex inItemIndex, 
		const ALanguage inLanguage, AText& outName ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//
	switch(inLanguage)
	{
	  case kLanguage_Japanese:
		{
			mToolItemArray_JapaneseName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_English:
		{
			mToolItemArray_EnglishName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_French:
		{
			mToolItemArray_FrenchName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_German:
		{
			mToolItemArray_GermanName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Spanish:
		{
			mToolItemArray_SpanishName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Italian:
		{
			mToolItemArray_ItalianName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Dutch:
		{
			mToolItemArray_DutchName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Swedish:
		{
			mToolItemArray_SwedishName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Norwegian:
		{
			mToolItemArray_NorwegianName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Danish:
		{
			mToolItemArray_DanishName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Finnish:
		{
			mToolItemArray_FinnishName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Portuguese:
		{
			mToolItemArray_PortugueseName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_SimplifiedChinese:
		{
			mToolItemArray_SimplifiedChineseName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_TraditionalChinese:
		{
			mToolItemArray_TraditionalChineseName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Korean:
		{
			mToolItemArray_KoreanName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Polish:
		{
			mToolItemArray_PolishName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_PortuguesePortugal:
		{
			mToolItemArray_PortuguesePortugalName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Russian:
		{
			mToolItemArray_RussianName.GetObjectConst(arrayIndex).Get(inItemIndex,outName);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#305
/**
ツール表示名前取得
優先順位：現在の言語→英語→ファイル名
*/
void	AModePrefDB::GetToolDisplayName( const AObjectID inMenuObjectID, const AIndex inItemIndex, AText& outName ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//#427 staticTextがあればそちら優先
	AText	staticText;
	mToolItemArray_StaticText.GetObjectConst(arrayIndex).Get(inItemIndex,staticText);
	if( staticText.GetItemCount() > 0 )
	{
		outName.SetText(staticText);
		return;
	}
	
	//ファイル名
	mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(inItemIndex).GetFilename(outName);
	//#444 拡張子削除
	AText	suffix;
	outName.GetSuffix(suffix);
	if( suffix.GetLength() > 0 )
	{
		outName.Delete(outName.GetLength()-suffix.GetLength(),suffix.GetLength());
	}
	//多言語名前
	AText	multiLangMenuText;
	GetToolMultiLanguageName(inMenuObjectID,inItemIndex,GetApp().NVI_GetLanguage(),multiLangMenuText);
	if( multiLangMenuText.GetItemCount() > 0 )
	{
		outName.SetText(multiLangMenuText);
	}
	else
	{
		AText	englishMenuText;
		GetToolMultiLanguageName(inMenuObjectID,inItemIndex,kLanguage_English,englishMenuText);
		if( englishMenuText.GetItemCount() > 0 )
		{
			outName.SetText(englishMenuText);
		}
	}
}

//#427
/**
自動更新フラグ取得
*/
ABool	AModePrefDB::GetToolAutoUpdateFlag( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//取得
	return mToolItemArray_AutoUpdate.GetObjectConst(arrayIndex).Get(inItemIndex);
}

//#427
/**
ツールがStaticTextかどうか判定
*/
ABool	AModePrefDB::GetToolIsStaticText( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//取得
	AText	staticText;
	mToolItemArray_StaticText.GetObjectConst(arrayIndex).Get(inItemIndex,staticText);
	return (staticText.GetItemCount()>0);
}

//#427
/**
*/
ABool	AModePrefDB::GetToolFolderIsAutoUpdate( const AObjectID inMenuObjectID ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//取得
	return mToolMenuArray_AutoUpdateFolder.Get(arrayIndex);
}

#pragma mark ---ツール（orderファイル）

void	AModePrefDB::WriteToolOrderFile( const AObjectID inMenuObjectID )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	/*#427 orderファイルは保存しないようにする（設定ファイルが複数あるのは混乱するので）
	AItemCount	count = mToolItemArray_File.GetObjectConst(arrayIndex).GetItemCount();
	//orderファイル書き換え
	if( count > 0 )
	{
		AText	orderText;
		for( AIndex i = 0; i < mToolItemArray_File.GetObjectConst(arrayIndex).GetItemCount(); i++ )
		{
			AText	filename;
			mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(i).GetFilename(filename);
			//R0173
			if( mToolItemArray_Enabled.GetObjectConst(arrayIndex).Get(i) == false )
			{
				filename.InsertCstring(0,"%%");
			}
			//#129
			if( mToolItemArray_Grayout.GetObjectConst(arrayIndex).Get(i) == true )
			{
				filename.InsertCstring(0,"%)");
			}
			
			orderText.AddText(filename);
			orderText.Add(kUChar_LineEnd);
		}
		AFileAcc	f;
		f.CopyFrom(mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(0));
		AFileAcc	orderFile;
		orderFile.SpecifySister(f,".order");
		orderFile.CreateFile();//B0000
		orderFile.WriteFile(orderText);
		
		*#305
		旧orderファイルは保存しないようにする（あまり設定ファイルが多すぎるのも混乱するので）
		AFileAcc	orderFileSJIS;
		orderFileSJIS.SpecifySister(f,"order");
		orderText.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
		orderFileSJIS.CreateFile();//B0000
		orderFileSJIS.WriteFile(orderText);
		*
	}
	*/
	/*#1275 ルート以外のメニューでもautoupdate/userそれぞれのtoolpref.miを保存しようとしていたので、修正する。
	//#427 AutoUpdateのツールと、User定義のツールそれぞれtoolpref.miに保存するためのループ
	for( AIndex j = 0; j <= 1; j++ )
	{
		//このループがautoUpdateかどうかを設定
		ABool	autoUpdate = false;
		if( j == 0 )
		{
			autoUpdate = true;
		}
	*/
	//ルートメニューかどうかの判定 #1275
	if( mToolMenuArray_MenuObjectID.Get(arrayIndex) == mToolMenuRootObjectID )
	{
		//ルートメニューの場合は、autoupdate/user両方のtoolpref.miを保存
		WriteToolOrderFile(arrayIndex,true);
		WriteToolOrderFile(arrayIndex,false);
	}
	else
	{
		//ルートメニュー以外の場合は、メニューのautoupdateフラグに従って、autoupdate/userどちらかのtoolpref.miを保存
		WriteToolOrderFile(arrayIndex,mToolMenuArray_AutoUpdateFolder.Get(arrayIndex));
	}
}
void	AModePrefDB::WriteToolOrderFile( const AIndex arrayIndex, const ABool autoUpdate )
{
	//#305
	//ToolPrefDB書き込み
	AToolPrefDB	toolPrefDB;
	for( AIndex i = 0; i < mToolItemArray_File.GetObjectConst(arrayIndex).GetItemCount(); i++ )
	{
		//#427 自動更新／ユーザー編集どちらかのみをそれぞれtoolPrefDBへ書き込み
		if( mToolItemArray_AutoUpdate.GetObjectConst(arrayIndex).Get(i) != autoUpdate )
		{
			continue;
		}
		//#427 statictextツールはtoolprefに書きださない
		AText	staticText;
		mToolItemArray_StaticText.GetObjectConst(arrayIndex).Get(i,staticText);
		if( staticText.GetItemCount() > 0 )
		{
			continue;
		}
		//
		AText	filename;
		mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(i).GetFilename(filename,false);
		//設定
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_FileName,filename);
		toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_Enabled,
								 mToolItemArray_Enabled.GetObjectConst(arrayIndex).Get(i));
		toolPrefDB.Add_NumberArray(AToolPrefDB::kToolArray_Shortcut,
								   mToolItemArray_Shortcut.GetObjectConst(arrayIndex).Get(i));
		toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_ShortcutModifiers_ControlKey,
								 AKeyWrapper::IsControlKeyOnMenuShortcut(mToolItemArray_Modifiers.GetObjectConst(arrayIndex).Get(i)));
		toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_ShortcutModifiers_ShiftKey,
								 AKeyWrapper::IsShiftKeyOnMenuShortcut(mToolItemArray_Modifiers.GetObjectConst(arrayIndex).Get(i)));
		toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_Grayout,
								 mToolItemArray_Grayout.GetObjectConst(arrayIndex).Get(i));
		//多言語タイトル
		AText	multiLangTitle;
		mToolItemArray_JapaneseName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_JapaneseName,multiLangTitle);
		mToolItemArray_EnglishName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_EnglishName,multiLangTitle);
		mToolItemArray_FrenchName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_FrenchName,multiLangTitle);
		mToolItemArray_GermanName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_GermanName,multiLangTitle);
		mToolItemArray_SpanishName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_SpanishName,multiLangTitle);
		mToolItemArray_ItalianName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_ItalianName,multiLangTitle);
		mToolItemArray_DutchName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_DutchName,multiLangTitle);
		mToolItemArray_SwedishName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_SwedishName,multiLangTitle);
		mToolItemArray_NorwegianName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_NorwegianName,multiLangTitle);
		mToolItemArray_DanishName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_DanishName,multiLangTitle);
		mToolItemArray_FinnishName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_FinnishName,multiLangTitle);
		mToolItemArray_PortugueseName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_PortugueseName,multiLangTitle);
		mToolItemArray_SimplifiedChineseName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_SimplifiedChineseName,multiLangTitle);
		mToolItemArray_TraditionalChineseName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_TraditionalChineseName,multiLangTitle);
		mToolItemArray_KoreanName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_KoreanName,multiLangTitle);
		mToolItemArray_PolishName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_PolishName,multiLangTitle);
		mToolItemArray_PortuguesePortugalName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_PortuguesePortugalName,multiLangTitle);
		mToolItemArray_RussianName.GetObjectConst(arrayIndex).Get(i,multiLangTitle);
		toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_RussianName,multiLangTitle);
	}
	//toolpref.miファイル取得
	//autoupdateの場合は、"autoupdate_userdata"フォルダ内の対応するフォルダのtoolpref.miを指定する。#427
	AFileAcc	toolPrefFile;
	//#427 toolPrefFile.SpecifyChild(mToolMenuArray_Folder.GetObjectConst(arrayIndex),"toolpref.mi");
	GetToolPrefFile(arrayIndex,autoUpdate,toolPrefFile);//#427
	//
	toolPrefDB.WriteToPrefTextFile(toolPrefFile);
}

//#427
/**
ToolPrefファイルのFileAccを取得
@note ルートメニューの場合はユーザー定義ツールと自動更新ツールが混在しているのでinAutoUpdateフラグでどちらのtoolpref.miを取得するかを選択する。
ルートメニュー以外の場合はinAutoUpdateフラグにかかわらずメニューがユーザー定義ツール、自動更新ツールどちらなのかに従う。
*/
void	AModePrefDB::GetToolPrefFile( const AIndex inToolMenuArrayIndex, const ABool inAutoUpdateForRootMenu, AFileAcc& outToolPrefFile ) const
{
	//メニューのautoupdateフラグ取得
	ABool	autoUpdate = mToolMenuArray_AutoUpdateFolder.Get(inToolMenuArrayIndex);
	//ルートメニューかどうかを取得
	ABool	rootMenu = (mToolMenuArray_MenuObjectID.Get(inToolMenuArrayIndex) == mToolMenuRootObjectID);
	//ルートメニューなら、autoupdateフラグは引数に従う
	if( rootMenu == true )
	{
		autoUpdate = inAutoUpdateForRootMenu;
	}
	//autoupdateかどうかの分岐
	if( autoUpdate == false )
	{
		//ユーザー定義ツールの場合
		
		//ツールのあるフォルダのtoolpref.mi
		outToolPrefFile.SpecifyChild(mToolMenuArray_Folder.GetObjectConst(inToolMenuArrayIndex),"toolpref.mi");
	}
	else
	{
		//自動更新ツールの場合
		
		//"tool"の後の部分パス取得
		
		//自動更新フォルダのtoolフォルダ（＝(mode name)/autoupdate/tool/）を取得
		AFileAcc	toolRootFolder;
		GetToolFolder(true,toolRootFolder);
		//指定メニューに対応するフォルダの、toolフォルダ内相対パスを取得
		AText	partialpath;
		if( rootMenu == true )
		{
			//処理無し
			//ルートの場合は、相対パスは空とする。
			//mToolMenuArray_Folderは、ユーザーツールのほうのtoolフォルダになっているので、GetPartialPath()を実行するとおかしくなる
		}
		else
		{
			//指定メニューに対応するフォルダの、toolフォルダからの相対パスを取得
			mToolMenuArray_Folder.GetObjectConst(inToolMenuArrayIndex).GetPartialPath(toolRootFolder,partialpath);
		}
		//"autoupdate_userdata"フォルダ内の対応するフォルダのtoolpref.miを取得
		AFileAcc	autoUpdateUserDataToolRootFolder;
		GetAutoUpdateUserDataFolder("tool",autoUpdateUserDataToolRootFolder);
		AFileAcc	folder;
		folder.SpecifyChild(autoUpdateUserDataToolRootFolder,partialpath);
		folder.CreateFolderRecursive();//再帰的にフォルダ生成
		//
		outToolPrefFile.SpecifyChild(folder,"toolpref.mi");
	}
}

#pragma mark ===========================

#pragma mark ---ツールバー

void	AModePrefDB::LoadToolbar()
{
	/*#427 下へ移動
	AFileAcc	toolbarFolder;
	GetToolbarFolder(toolbarFolder);
	
	//フォルダの中のファイルを取得し、childrenへ格納
	AObjectArray<AFileAcc>	children;
	toolbarFolder.GetChildren(children);
	*/
	/*#724
	//
	for( AIndex i = 0; i < mToolbarItem_IconID.GetItemCount(); i++ )
	{
		if( mToolbarItem_Type.Get(i) == kToolbarItemType_File || mToolbarItem_Type.Get(i) == kToolbarItemType_Folder )
		{
			AIconID	iconID = mToolbarItem_IconID.Get(i);//#232
			if( CWindowImp::IsDynamicIcon(iconID) == true )//#232 IconがDynamicIconでない場合（GetToolbarIconFromToolContent()で取得したアイコン）をはずす
			{
				CWindowImp::UnregisterDynamicIcon(iconID);
			}
		}
	}
	*/
	//#724 mToolbarItem_IconID.DeleteAll();
	mToolbarItem_File.DeleteAll();
	mToolbarItem_Type.DeleteAll();
	mToolbarItem_Enabled.DeleteAll();
	mToolbarItem_FilenameArray.DeleteAll();
	//#844 mToolbarItem_AutoUpdate.DeleteAll();//#427
	//#305
	mToolbarItem_JapaneseName.DeleteAll();
	mToolbarItem_EnglishName.DeleteAll();
	mToolbarItem_FrenchName.DeleteAll();
	mToolbarItem_GermanName.DeleteAll();
	mToolbarItem_SpanishName.DeleteAll();
	mToolbarItem_ItalianName.DeleteAll();
	mToolbarItem_DutchName.DeleteAll();
	mToolbarItem_SwedishName.DeleteAll();
	mToolbarItem_NorwegianName.DeleteAll();
	mToolbarItem_DanishName.DeleteAll();
	mToolbarItem_FinnishName.DeleteAll();
	mToolbarItem_PortugueseName.DeleteAll();
	mToolbarItem_SimplifiedChineseName.DeleteAll();
	mToolbarItem_TraditionalChineseName.DeleteAll();
	mToolbarItem_KoreanName.DeleteAll();
	mToolbarItem_PolishName.DeleteAll();
	mToolbarItem_PortuguesePortugalName.DeleteAll();
	mToolbarItem_RussianName.DeleteAll();
	
	//自動更新ツールバーと、ユーザー編集編集ツールバーそれぞれ有効／無効設定に従ってロード実行
	//#844 ツールバーはユーザー編集ツールバー（マクロバー）のみにする。for( AIndex j = 0; j <= 1; j++ )
	{
		/*#844
		//j=0: AutoUpdateツールバー、j=1: ユーザーツールバー
		ABool	autoUpdate = false;
		if( j == 0 )
		{
			autoUpdate = true;
		}
		
		//自動更新ツールバー項目ロード有無判定
		if( autoUpdate == true && GetData_Bool(kEnableAutoUpdateToolbar) == false )
		{
			continue;
		}
		
		//自動更新ツールバー項目ロード有無判定
		if( autoUpdate == false && GetData_Bool(kEnableUserToolbar) == false )
		{
			continue;
		}
		*/
		//ツールバーフォルダ取得
		AFileAcc	toolbarFolder;
		GetToolbarFolder(/*#844 autoUpdate,*/toolbarFolder);
		toolbarFolder.CreateFolderRecursive();
		
		/*#844
		//ツールバーフォルダが存在していなければ何もしない
		if( toolbarFolder.Exist() == false )
		{
			continue;
		}
		*/
		
		//フォルダの中のファイルを取得し、childrenへ格納
		AObjectArray<AFileAcc>	children;
		toolbarFolder.GetChildren(children);
		
		//
		AFileAcc	toolPrefFile;
		toolPrefFile.SpecifyChild(toolbarFolder,"toolpref.mi");
		if( toolPrefFile.Exist() == true )
		{
			//toolpref.miファイルから読み込み
			
			AToolPrefDB	toolPrefDB;
			//#844 if( autoUpdate == false )
			{
				//ユーザー編集ツールの場合
				//ツールファイルと同じフォルダのtoolpref.miを読み込む
				toolPrefDB.LoadFromPrefTextFile(toolPrefFile);
			}
			/*#844 
			else
			{
				//#427
				//自動更新ツールの場合、autoupdateフォルダ内のtoolpref.miと
				//autoupdate_userdataフォルダ内の対応するフォルダのtoolpref.miとをマージする。（ツール名をキーにマージ）
				
				//まず、autoupdate_userdataフォルダ内の対応するフォルダのtoolpref.miを読み込む
				//（存在しなければ読み込まない）
				AFileAcc	userDataToolPrefFile;
				GetToolbarPrefFile(true,userDataToolPrefFile);
				if( userDataToolPrefFile.Exist() == true )
				{
					toolPrefDB.LoadFromPrefTextFile(userDataToolPrefFile);
				}
				//autoupdateフォルダ内のtoolpref.miを読み込む
				AToolPrefDB	autoUpdateToolPrefDB;
				autoUpdateToolPrefDB.LoadFromPrefTextFile(toolPrefFile);
				//autoupdateフォルダ内のtoolpref.miの各項目ごとにループ
				for( AIndex autoupdateIndex = 0; 
							autoupdateIndex < autoUpdateToolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName);
							autoupdateIndex++ )
				{
					AText	filename;
					autoUpdateToolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FileName,autoupdateIndex,filename);
					//AutoUpdate側のツールと同じファイル名のツールをUserData側から検索
					AIndex	foundIndex = toolPrefDB.Find_TextArray(AToolPrefDB::kToolArray_FileName,filename);
					if( foundIndex == kIndex_Invalid )
					{
						//UserData側に存在しなかった場合、UserDataへ追加する
						toolPrefDB.InsertRowFromOtherDB_Table(AToolPrefDB::kToolArray_FileName,
									toolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName),
									autoUpdateToolPrefDB,autoupdateIndex);
					}
					else
					{
						//UserData側に存在した場合、多言語ツール名前のみコピー
						AArray<APrefID>	langPrefIDArray;
						AToolPrefDB::GetLanguagePrefIDArray(langPrefIDArray);
						for( AIndex i = 0; i < langPrefIDArray.GetItemCount(); i++ )
						{
							AText	name;
							autoUpdateToolPrefDB.GetData_TextArray(langPrefIDArray.Get(i),autoupdateIndex,name);
							toolPrefDB.SetData_TextArray(langPrefIDArray.Get(i),foundIndex,name);
						}
					}
				}
			}
			*/
			
			//==================V2からの引き継ぎ時処理==================
			//V2での標準添付ツールバーはデフォルトで非表示にする
			//（V2のtoolpref.mi対応バージョンでユーザーが作成したツールバー項目のみ、表示することにする）
			
			//tool pref内に設定されたバージョンテキストを取得
			AText	versionText;
			toolPrefDB.GetData_Text(AToolPrefDB::kVersionText,versionText);
			//kVersionTextが空のとき＝バージョン3で書き込んだことの無いデータ
			if( versionText.GetItemCount() == 0 )
			{
				AItemCount	prefItemCount = toolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName);
				for( AIndex index = 0; index < prefItemCount; index++ )
				{
					//バージョン2.1での標準添付ツールバー名と同じ場合は削除する。
					//バージョン3で新規追加したデータで表示／非表示しているので、バージョン2.1上での表示には影響しない
					AText	name;
					toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FileName,index,name);
					if( GetApp().SPI_IsV2ToolbarItemNameToHide(name) == true )
					{
						toolPrefDB.SetData_BoolArray(AToolPrefDB::kToolArray_EnabledForMacroBar,index,false);
					}
				}
			}
			
			//==================
			
			//フォルダの中のファイルを取得し、childrenへ格納
			AObjectArray<AFileAcc>	children;
			toolbarFolder.GetChildren(children);
			//フォルダの中のファイルについての追加済みフラグ、ファイル名Hash
			AHashTextArray	childrenFilesName;
			ABoolArray	childrenFilesAddedFlag;
			for( AIndex i = 0; i < children.GetItemCount(); i++ )
			{
				AText	filename;
				children.GetObject(i).GetFilename(filename);
				childrenFilesName.Add(filename);
				childrenFilesAddedFlag.Add(false);
			}
			
			//ToolPrefの各項目を追加
			AItemCount	prefItemCount = toolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName);
			for( AIndex index = 0; index < prefItemCount; index++ )
			{
				//ツール追加
				AText	filename;
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FileName,index,filename);
				//#427 
				//toolpref.miに記述された項目のファイルが存在していない場合、
				//その項目は無視する。
				//（2.1.13b1より仕様変更。自動更新時、ツールが削除された場合、autoupdate_userdataフォルダのほうの
				//項目だけが残る可能性がある。この場合、ツールを追加すべきではない。また、従来においても、空のツールを追加するメリットもない。）#427
				AFileAcc	file;
				file.SpecifyChild(toolbarFolder,filename);
				AToolbarItemType	type = GetToolbarTypeFromFile(file);
				if( file.Exist() == false && 
							(type == kToolbarItemType_File || type == kToolbarItemType_Folder ) )//#427   file.CreateFile();
				{
					//この項目は無視して次へ
					continue;
				}
				//
				AIndex	toolbarIndex = mToolbarItem_Type.Add(kToolbarItemType_File);//#427
				mToolbarItem_File.AddNewObject();
				//#724 mToolbarItem_IconID.Add(kIconID_NoIcon);
				mToolbarItem_Enabled.Add(toolPrefDB.GetData_BoolArray(AToolPrefDB::kToolArray_EnabledForMacroBar,index));
				mToolbarItem_FilenameArray.AddNewObject();
				//#844 mToolbarItem_AutoUpdate.Add(autoUpdate);//#427
				/*#427
				AFileAcc	file;
				file.SpecifyChild(toolbarFolder,filename);
				AToolbarItemType	type = GetToolbarTypeFromFile(file);
				*/
				if( type != kToolbarItemType_File && type != kToolbarItemType_Folder )
				{
					mToolbarItem_Type.Set(toolbarIndex,type);
					//#724 mToolbarItem_IconID.Set(toolbarIndex,kIconID_Mi_BuiltinToolButton);
				}
				else
				{
					/*#427
					AFileAcc	file;
					file.SpecifyChild(toolbarFolder,filename);
					*/
					if( file.IsFolder() == false || file.IsBundleFolder() == true )
					{
						mToolbarItem_Type.Set(toolbarIndex,kToolbarItemType_File);
					}
					else
					{
						mToolbarItem_Type.Set(toolbarIndex,kToolbarItemType_Folder);
						GetFilenameArray(file,mToolbarItem_FilenameArray.GetObject(toolbarIndex));
					}
					mToolbarItem_File.GetObject(toolbarIndex).CopyFrom(file);
					/*#724 
					AIconID	iconID = CWindowImp::RegisterDynamicIconFromFile(file,true);
					mToolbarItem_IconID.Set(toolbarIndex,iconID);
					*/
				}
				//他言語タイトル設定
				AText	multiLangTitle;
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_JapaneseName,index,multiLangTitle);
				mToolbarItem_JapaneseName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_EnglishName,index,multiLangTitle);
				mToolbarItem_EnglishName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FrenchName,index,multiLangTitle);
				mToolbarItem_FrenchName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_GermanName,index,multiLangTitle);
				mToolbarItem_GermanName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_SpanishName,index,multiLangTitle);
				mToolbarItem_SpanishName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_ItalianName,index,multiLangTitle);
				mToolbarItem_ItalianName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_DutchName,index,multiLangTitle);
				mToolbarItem_DutchName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_SwedishName,index,multiLangTitle);
				mToolbarItem_SwedishName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_NorwegianName,index,multiLangTitle);
				mToolbarItem_NorwegianName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_DanishName,index,multiLangTitle);
				mToolbarItem_DanishName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FinnishName,index,multiLangTitle);
				mToolbarItem_FinnishName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_PortugueseName,index,multiLangTitle);
				mToolbarItem_PortugueseName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_SimplifiedChineseName,index,multiLangTitle);
				mToolbarItem_SimplifiedChineseName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_TraditionalChineseName,index,multiLangTitle);
				mToolbarItem_TraditionalChineseName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_KoreanName,index,multiLangTitle);
				mToolbarItem_KoreanName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_PolishName,index,multiLangTitle);
				mToolbarItem_PolishName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_PortuguesePortugalName,index,multiLangTitle);
				mToolbarItem_PortuguesePortugalName.Add(multiLangTitle);
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_RussianName,index,multiLangTitle);
				mToolbarItem_RussianName.Add(multiLangTitle);
				
				//ToolPrefのファイル名に対応するファイルが存在したら追加済みフラグOn
				AIndex	childrenFilesIndex = childrenFilesName.Find(filename);
				if( childrenFilesIndex != kIndex_Invalid )
				{
					childrenFilesAddedFlag.Set(childrenFilesIndex,true);
				}
			}
			
			//未追加（ToolPrefに存在しないもの）のファイルを追加
			//Finderや旧バージョンでファイル名を変更した場合に未追加になる。
			//（他言語タイトル等のデータを消さないために、ToolPrefのデータも生かしておく）
			//ただし、非表示にする。
			for( AIndex i = 0; i < children.GetItemCount(); i++ )
			{
				//追加済みは対象外
				if( childrenFilesAddedFlag.Get(i) == true )   continue;
				//ファイル名取得
				AFileAcc	file = children.GetObject(i);
				AText	filename;
				file.GetFilename(filename);
				//対象外ファイル
				if( filename.GetLength() == 0 )   continue;
				if( filename.Get(0) == '.' )   continue;
				if( filename.Compare("order") == true )   continue;
				if( filename.Compare("Icon\r") == true )   continue;
				if( filename.Compare("toolpref.mi") == true )   continue;
				//アイコンファイルは対象外 win
				AText	suffix;
				filename.GetSuffix(suffix);
				if( suffix.Compare(".ico") == true )   continue;
				if( suffix.Compare(".icns") == true )   continue;//#232
				//ツール追加
				AIndex	index = mToolbarItem_Type.Add(kToolbarItemType_File);
				mToolbarItem_File.AddNewObject();
				//#724 mToolbarItem_IconID.Add(kIconID_NoIcon);
				mToolbarItem_Enabled.Add(false);//#724 toolpref.miに登録されていないツールバー項目は非表示にする
				mToolbarItem_FilenameArray.AddNewObject();
				//#844 mToolbarItem_AutoUpdate.Add(false);//#427
				if( file.IsFolder() == false || file.IsBundleFolder() == true )
				{
					mToolbarItem_Type.Set(index,kToolbarItemType_File);
				}
				else
				{
					mToolbarItem_Type.Set(index,kToolbarItemType_Folder);
					GetFilenameArray(file,mToolbarItem_FilenameArray.GetObject(index));
				}
				mToolbarItem_File.GetObject(index).CopyFrom(file);
				/*#724 
				AIconID	iconID = CWindowImp::RegisterDynamicIconFromFile(file,true);
				mToolbarItem_IconID.Set(index,iconID);
				*/
				//他言語タイトル設定
				AText	multiLangTitle;
				mToolbarItem_JapaneseName.Add(multiLangTitle);
				mToolbarItem_EnglishName.Add(multiLangTitle);
				mToolbarItem_FrenchName.Add(multiLangTitle);
				mToolbarItem_GermanName.Add(multiLangTitle);
				mToolbarItem_SpanishName.Add(multiLangTitle);
				mToolbarItem_ItalianName.Add(multiLangTitle);
				mToolbarItem_DutchName.Add(multiLangTitle);
				mToolbarItem_SwedishName.Add(multiLangTitle);
				mToolbarItem_NorwegianName.Add(multiLangTitle);
				mToolbarItem_DanishName.Add(multiLangTitle);
				mToolbarItem_FinnishName.Add(multiLangTitle);
				mToolbarItem_PortugueseName.Add(multiLangTitle);
				mToolbarItem_SimplifiedChineseName.Add(multiLangTitle);
				mToolbarItem_TraditionalChineseName.Add(multiLangTitle);
				mToolbarItem_KoreanName.Add(multiLangTitle);
				mToolbarItem_PolishName.Add(multiLangTitle);
				mToolbarItem_PortuguesePortugalName.Add(multiLangTitle);
				mToolbarItem_RussianName.Add(multiLangTitle);
			}
		}
#if IMPLEMENTATION_FOR_MACOSX
		//旧バージョンで作成したモードデータとの互換性のための処理
		//自動更新データはtoolpref.mi必須を前提とする。（toolpref.miが無いバージョンは既にかなり古いので。）
		else
		{
			//orderファイル記載順に登録
			AText	orderText;
			AFileAcc	orderFile;
			orderFile.SpecifyChild(toolbarFolder,".order");
			if( orderFile.Exist() == false )
			{
				orderFile.SpecifyChild(toolbarFolder,"order");
			}
			GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_ToolOrderFile,orderFile,orderText);
			//
			for( AIndex pos = 0; pos < orderText.GetItemCount();  )
			{
				//orderファイルからlineへ１行取得
				AText	line;
				orderText.GetLine(pos,line);
				ABool	enabled = true;
				if( line.GetItemCount() > 2 )
				{
					if( line.Get(0) == '%' && line.Get(1) == '%' )
					{
						enabled = false;
						line.Delete(0,2);
					}
				}
				AFileAcc	file;
				file.SpecifyChild(toolbarFolder,line);
				AToolbarItemType	type = GetToolbarTypeFromFile(file);
				if( type != kToolbarItemType_File && type != kToolbarItemType_Folder )
				{
					mToolbarItem_Enabled.Add(false);//#724 toolpref.miのないバージョンのツールバー項目は非表示にする
					mToolbarItem_FilenameArray.AddNewObject();
					mToolbarItem_Type.Add(type);
					mToolbarItem_File.AddNewObject();
					//#724 mToolbarItem_IconID.Add(kIconID_Mi_BuiltinToolButton);
					//#844 mToolbarItem_AutoUpdate.Add(false);//#427
				}
				//
				else for( AIndex i = 0; i < children.GetItemCount(); i++ )
				{
					if( children.GetObject(i).CompareFilename(line) == true )
					{
						mToolbarItem_Enabled.Add(false);//#724 toolpref.miのないバージョンのツールバー項目は非表示にする
						mToolbarItem_FilenameArray.AddNewObject();
						//#844 mToolbarItem_AutoUpdate.Add(false);//#427
						AFileAcc	file;
						file.CopyFrom(children.GetObject(i));
						children.Delete1Object(i);
						if( file.IsFolder() == false || file.IsBundleFolder() == true )//R0137
						{
							mToolbarItem_Type.Add(kToolbarItemType_File);
						}
						else
						{
							mToolbarItem_Type.Add(kToolbarItemType_Folder);
							GetFilenameArray(file,mToolbarItem_FilenameArray.GetObject(mToolbarItem_FilenameArray.GetItemCount()-1));
						}
						AIndex	index = mToolbarItem_File.AddNewObject();
						mToolbarItem_File.GetObject(index).CopyFrom(file);
						/*#724 
						AIconID	iconID = CWindowImp::RegisterDynamicIconFromFile(file,true);
						mToolbarItem_IconID.Add(iconID);
						*/
						break;
					}
				}
			}
			
			//orderファイルに記載されていない、残りのファイルを登録
			for( AIndex i = 0; i < children.GetItemCount(); i++ )
			{
				AFileAcc	file;
				file.CopyFrom(children.GetObject(i));
				AText	filename;
				file.GetFilename(filename);
				if( filename.GetLength() == 0 )   continue;
				if( filename.Get(0) == '.' )   continue;
				if( filename.Compare("order") == true )   continue;
				if( filename.Compare("Icon\r") == true )   continue;
				//アイコンファイルは対象外 win
				AText	suffix;
				filename.GetSuffix(suffix);
				if( suffix.Compare(".ico") == true )   continue;
				if( suffix.Compare(".icns") == true )   continue;//#232
				//
				mToolbarItem_FilenameArray.AddNewObject();
				if( file.IsFolder() == false || file.IsBundleFolder() == true )//R0137
				{
					mToolbarItem_Type.Add(kToolbarItemType_File);
				}
				else
				{
					mToolbarItem_Type.Add(kToolbarItemType_Folder);
					GetFilenameArray(file,mToolbarItem_FilenameArray.GetObject(mToolbarItem_FilenameArray.GetItemCount()-1));
				}
				AIndex	index = mToolbarItem_File.AddNewObject();
				mToolbarItem_File.GetObject(index).CopyFrom(file);
				//AIconID	iconID = CWindowImp::RegisterDynamicIconFromFile(file,true);
				//#724 mToolbarItem_IconID.Add(iconID);
				mToolbarItem_Enabled.Add(false);//#724 toolpref.miのないバージョンのツールバー項目は非表示にする
				//#844 mToolbarItem_AutoUpdate.Add(false);//#427
			}
			
			//他言語タイトル設定 #305
			for( AIndex i = 0; i < mToolbarItem_Type.GetItemCount(); i++ )
			{
				AText	multiLangTitle;
				mToolbarItem_JapaneseName.Add(multiLangTitle);
				mToolbarItem_EnglishName.Add(multiLangTitle);
				mToolbarItem_FrenchName.Add(multiLangTitle);
				mToolbarItem_GermanName.Add(multiLangTitle);
				mToolbarItem_SpanishName.Add(multiLangTitle);
				mToolbarItem_ItalianName.Add(multiLangTitle);
				mToolbarItem_DutchName.Add(multiLangTitle);
				mToolbarItem_SwedishName.Add(multiLangTitle);
				mToolbarItem_NorwegianName.Add(multiLangTitle);
				mToolbarItem_DanishName.Add(multiLangTitle);
				mToolbarItem_FinnishName.Add(multiLangTitle);
				mToolbarItem_PortugueseName.Add(multiLangTitle);
				mToolbarItem_SimplifiedChineseName.Add(multiLangTitle);
				mToolbarItem_TraditionalChineseName.Add(multiLangTitle);
				mToolbarItem_KoreanName.Add(multiLangTitle);
				mToolbarItem_PolishName.Add(multiLangTitle);
				mToolbarItem_PortuguesePortugalName.Add(multiLangTitle);
				mToolbarItem_RussianName.Add(multiLangTitle);
			}
		}
#endif
	}
	//
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
}

//#427
/**
ツールバーファイル（ファイル名称）からタイプを取得
*/
AToolbarItemType	AModePrefDB::GetToolbarTypeFromFile( const AFileAcc& inFile )
{
	AText	filename;
	inFile.GetFilename(filename);
	if( filename.Compare("BUILTIN01") == true )
	{
		return kToolbarItemType_TextEncoding;
	}
	else if( filename.Compare("BUILTIN02") == true )
	{
		return kToolbarItemType_ReturnCode;
	}
	else if( filename.Compare("BUILTIN03") == true )
	{
		return kToolbarItemType_WrapMode;
	}
	else if( filename.Compare("BUILTIN04") == true )
	{
		return kToolbarItemType_Mode;
	}
	else if( inFile.IsFolder() == true )
	{
		return kToolbarItemType_Folder;
	}
	else
	{
		return kToolbarItemType_File;
	}
}

//#232
/**
ツール内テキストからToolbarアイコン指定
*/
/*#724
AIconID	AModePrefDB::GetToolbarIconFromToolContent( const AFileAcc& inFile ) const
{
	AIconID	iconID = kIconID_NoIcon;
	AText	text;
	inFile.ReadTo(text);
	AIndex	pos = 0;
	if( text.FindCstring(0,"TOOLBARICON",pos) == true )
	{
		AText	token;
		text.GetToken(pos,token);
		text.GetToken(pos,token);
		text.GetToken(pos,token);
		iconID = CWindowImp::GetIconIDByName(token);
	}
	return iconID;
}
*/

void	AModePrefDB::GetFilenameArray( const AFileAcc& inFolder, ATextArray& outTextArray )
{
	outTextArray.DeleteAll();
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		AFileAcc	file;
		file.CopyFrom(children.GetObject(i));
		if( file.IsFolder() == false )
		{
			AText	filename;
			file.GetFilename(filename);
			if( filename.GetItemCount() == 0 )   continue;
			if( filename.Get(0) == '.' )   continue;
			if( filename.Compare("Icon\r") == true )   continue;
			outTextArray.Add(filename);
		}
	}
}

AItemCount	AModePrefDB::GetToolbarItemCount() const
{
	return mToolbarItem_File.GetItemCount();
}

void	AModePrefDB::GetToolbarItem( const AIndex inIndex, AToolbarItemType& outType, AFileAcc& outFile, AText& outName, ABool& outEnabled ) const
{
	outType = mToolbarItem_Type.Get(inIndex);
	outFile.CopyFrom(mToolbarItem_File.GetObjectConst(inIndex));
	//#724 outIconID = mToolbarItem_IconID.Get(inIndex);
	switch(outType)
	{
	  case kToolbarItemType_File:
	  case kToolbarItemType_Folder:
		{
			outFile.GetFilename(outName);
			break;
		}
	  case kToolbarItemType_TextEncoding:
		{
			outName.SetLocalizedText("BuiltinToolButtonName_TextEncoding");
			break;
		}
	  case kToolbarItemType_ReturnCode:
		{
			outName.SetLocalizedText("BuiltinToolButtonName_ReturnCode");
			break;
		}
	  case kToolbarItemType_WrapMode:
		{
			outName.SetLocalizedText("BuiltinToolButtonName_WrapMode");//B0311
			break;
		}
	  case kToolbarItemType_Mode:
		{
			outName.SetLocalizedText("BuiltinToolButtonName_Mode");
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	outEnabled = mToolbarItem_Enabled.Get(inIndex);
}

ATextArray&	AModePrefDB::GetToolbarItemFilenameArray( const AIndex inIndex )
{
	return mToolbarItem_FilenameArray.GetObject(inIndex);
}

AIndex	AModePrefDB::AddToolbarItem( const AToolbarItemType inType, const AFileAcc& inFile, const ABool inAutoUpdate )//B0321 #427
{
	AIndex	index = mToolbarItem_Type.GetItemCount();
	InsertToolbarItem(index,inType,inFile,inAutoUpdate);
	return index;
}

void	AModePrefDB::InsertToolbarItem( const AIndex inItemIndex, const AToolbarItemType inType, const AFileAcc& inFile, const ABool inAutoUpdate )//#427
{
	mToolbarItem_Type.Insert1(inItemIndex,inType);
	mToolbarItem_File.InsertNew1Object(inItemIndex);
	mToolbarItem_File.GetObject(inItemIndex).CopyFrom(inFile);
	AIconID	iconID = kIconID_Mi_BuiltinToolButton;
	mToolbarItem_FilenameArray.InsertNew1Object(inItemIndex);
	switch(inType)
	{
	  case kToolbarItemType_File:
		{
			iconID = CWindowImp::RegisterDynamicIconFromFile(inFile,true);
			break;
		}
	  case kToolbarItemType_Folder:
		{
			iconID = CWindowImp::RegisterDynamicIconFromFile(inFile,true);//#232
			GetFilenameArray(inFile,mToolbarItem_FilenameArray.GetObject(inItemIndex));
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//#724 mToolbarItem_IconID.Insert1(inItemIndex,iconID);
	mToolbarItem_Enabled.Insert1(inItemIndex,true);
	//#844 mToolbarItem_AutoUpdate.Insert1(inItemIndex,inAutoUpdate);//#427
	//他言語タイトル設定
	AText	multiLangTitle;
	mToolbarItem_JapaneseName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_EnglishName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_FrenchName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_GermanName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_SpanishName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_ItalianName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_DutchName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_SwedishName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_NorwegianName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_DanishName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_FinnishName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_PortugueseName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_SimplifiedChineseName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_TraditionalChineseName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_KoreanName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_PolishName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_PortuguesePortugalName.Insert1(inItemIndex,multiLangTitle);
	mToolbarItem_RussianName.Insert1(inItemIndex,multiLangTitle);
	//
	WriteToolbarItemOrderFile();
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
	//#724 GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();//#232
}

void	AModePrefDB::DeleteToolbarItem( const AIndex inIndex )
{
	switch(mToolbarItem_Type.Get(inIndex))
	{
	  case kToolbarItemType_File:
	  case kToolbarItemType_Folder:
		{
			ALaunchWrapper::MoveToTrash(mToolbarItem_File.GetObject(inIndex));
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	mToolbarItem_Type.Delete1(inIndex);
	mToolbarItem_File.Delete1Object(inIndex);
	mToolbarItem_FilenameArray.Delete1Object(inIndex);
	//#724 mToolbarItem_IconID.Delete1(inIndex);
	mToolbarItem_Enabled.Delete1(inIndex);
	//#844 mToolbarItem_AutoUpdate.Delete1(inIndex);//#427
	//他言語タイトル削除
	AText	multiLangTitle;
	mToolbarItem_JapaneseName.Delete1(inIndex);
	mToolbarItem_EnglishName.Delete1(inIndex);
	mToolbarItem_FrenchName.Delete1(inIndex);
	mToolbarItem_GermanName.Delete1(inIndex);
	mToolbarItem_SpanishName.Delete1(inIndex);
	mToolbarItem_ItalianName.Delete1(inIndex);
	mToolbarItem_DutchName.Delete1(inIndex);
	mToolbarItem_SwedishName.Delete1(inIndex);
	mToolbarItem_NorwegianName.Delete1(inIndex);
	mToolbarItem_DanishName.Delete1(inIndex);
	mToolbarItem_FinnishName.Delete1(inIndex);
	mToolbarItem_PortugueseName.Delete1(inIndex);
	mToolbarItem_SimplifiedChineseName.Delete1(inIndex);
	mToolbarItem_TraditionalChineseName.Delete1(inIndex);
	mToolbarItem_KoreanName.Delete1(inIndex);
	mToolbarItem_PolishName.Delete1(inIndex);
	mToolbarItem_PortuguesePortugalName.Delete1(inIndex);
	mToolbarItem_RussianName.Delete1(inIndex);
	//
	WriteToolbarItemOrderFile();
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
	//#724 GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();//#232
}

void	AModePrefDB::WriteToolbarItemOrderFile()
{
	/* #427 orderファイルは保存しないようにする（設定ファイルが複数あるのは混乱するので）
	AText	orderText;
	for( AIndex i = 0; i < mToolbarItem_Type.GetItemCount(); i++ )
	{
		if( mToolbarItem_Enabled.Get(i) == false )
		{
			orderText.AddCstring("%%");
		}
		switch(mToolbarItem_Type.Get(i))
		{
		  case kToolbarItemType_TextEncoding:
			{
				orderText.AddCstring("BUILTIN01");
				break;
			}
		  case kToolbarItemType_ReturnCode:
			{
				orderText.AddCstring("BUILTIN02");
				break;
			}
		  case kToolbarItemType_WrapMode:
			{
				orderText.AddCstring("BUILTIN03");
				break;
			}
		  case kToolbarItemType_Mode:
			{
				orderText.AddCstring("BUILTIN04");
				break;
			}
		  case kToolbarItemType_File:
		  case kToolbarItemType_Folder:
			{
				AText	filename;
				mToolbarItem_File.GetObject(i).GetFilename(filename);
				orderText.AddText(filename);
				break;
			}
		}
		orderText.Add(kUChar_LineEnd);
	}
	
	AFileAcc	toolbarfolder;
	GetToolbarFolder(toolbarfolder);
	//
	AFileAcc	orderFile;
	orderFile.SpecifyChild(toolbarfolder,".order");
	orderFile.CreateFile();//B0000
	orderFile.WriteFile(orderText);
	*/
	/*#305
	旧orderファイルは保存しないようにする（あまり設定ファイルが多すぎるのも混乱するので）
	AFileAcc	orderFileSJIS;
	orderFileSJIS.SpecifyChild(toolbarfolder,"order");
	orderText.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	orderFileSJIS.CreateFile();//B0000
	orderFileSJIS.WriteFile(orderText);
	*/
	//#427 AutoUpdateのツールと、User定義のツールそれぞれtoolpref.miに保存するためのループ
	//#844 for( AIndex j = 0; j <= 1; j++ )
	{
		/*#844
		//j=0:AutoUpdateのツールバー項目、j=1:ユーザー作成のツールバー項目
		ABool	autoUpdate = false;
		if( j == 0 )
		{
			autoUpdate = true;
		}
		*/
		//
		//#305
		//ToolPrefDB書き込み
		AToolPrefDB	toolPrefDB;
		//バージョンテキスト書き込み
		AText	versionText;
		GetApp().NVI_GetVersion(versionText);
		toolPrefDB.SetData_Text(AToolPrefDB::kVersionText,versionText);
		//
		for( AIndex i = 0; i < mToolbarItem_Type.GetItemCount(); i++ )
		{
			/*#844
			//#427 自動更新／ユーザー編集どちらか
			if( mToolbarItem_AutoUpdate.Get(i) != autoUpdate )
			{
				continue;
			}
			*/
			//
			AText	filename;
			mToolbarItem_File.GetObject(i).GetFilename(filename);
			//設定
			switch(mToolbarItem_Type.Get(i))
			{
			  case kToolbarItemType_TextEncoding:
				{
					filename.SetCstring("BUILTIN01");
					break;
				}
			  case kToolbarItemType_ReturnCode:
				{
					filename.SetCstring("BUILTIN02");
					break;
				}
			  case kToolbarItemType_WrapMode:
				{
					filename.SetCstring("BUILTIN03");
					break;
				}
			  case kToolbarItemType_Mode:
				{
					filename.SetCstring("BUILTIN04");
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_FileName,filename);
			toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_EnabledForMacroBar,mToolbarItem_Enabled.Get(i));
			toolPrefDB.Add_NumberArray(AToolPrefDB::kToolArray_Shortcut,0);
			toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_ShortcutModifiers_ControlKey,false);
			toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_ShortcutModifiers_ShiftKey,false);
			toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_Grayout,false);
			//多言語タイトル
			AText	multiLangTitle;
			mToolbarItem_JapaneseName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_JapaneseName,multiLangTitle);
			mToolbarItem_EnglishName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_EnglishName,multiLangTitle);
			mToolbarItem_FrenchName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_FrenchName,multiLangTitle);
			mToolbarItem_GermanName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_GermanName,multiLangTitle);
			mToolbarItem_SpanishName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_SpanishName,multiLangTitle);
			mToolbarItem_ItalianName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_ItalianName,multiLangTitle);
			mToolbarItem_DutchName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_DutchName,multiLangTitle);
			mToolbarItem_SwedishName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_SwedishName,multiLangTitle);
			mToolbarItem_NorwegianName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_NorwegianName,multiLangTitle);
			mToolbarItem_DanishName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_DanishName,multiLangTitle);
			mToolbarItem_FinnishName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_FinnishName,multiLangTitle);
			mToolbarItem_PortugueseName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_PortugueseName,multiLangTitle);
			mToolbarItem_SimplifiedChineseName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_SimplifiedChineseName,multiLangTitle);
			mToolbarItem_TraditionalChineseName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_TraditionalChineseName,multiLangTitle);
			mToolbarItem_KoreanName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_KoreanName,multiLangTitle);
			mToolbarItem_PolishName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_PolishName,multiLangTitle);
			mToolbarItem_PortuguesePortugalName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_PortuguesePortugalName,multiLangTitle);
			mToolbarItem_RussianName.Get(i,multiLangTitle);
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_RussianName,multiLangTitle);
		}
		//toolpref.miファイル取得
		//autoupdateの場合は、"autoupdate_userdata"フォルダ内の対応するフォルダのtoolpref.miを指定する。#427
		AFileAcc	toolPrefFile;
		//#427 toolPrefFile.SpecifyChild(toolbarfolder,"toolpref.mi");
		GetToolbarPrefFile(/*#844 autoUpdate,*/toolPrefFile);//#427
		//
		toolPrefDB.WriteToPrefTextFile(toolPrefFile);
	}
}

//#427
/**
Toolbarのtoolpref.miファイルのFileAcc取得
*/
void	AModePrefDB::GetToolbarPrefFile( /*#844 const ABool inAutoUpdate,*/ AFileAcc& outToolPrefFile ) const
{
	//#844 if( inAutoUpdate == false )
	{
		//ユーザー定義ツールの場合
		
		//ツールバーフォルダ取得
		AFileAcc	toolbarFolder;
		GetToolbarFolder(/*#844 false,*/toolbarFolder);
		//ツールバーのあるフォルダのtoolpref.mi
		outToolPrefFile.SpecifyChild(toolbarFolder,"toolpref.mi");
	}
	/*#844
	else
	{
		//自動更新ツールの場合
		
		//"autoupdate_userdata"フォルダ内の対応するフォルダのtoolpref.miを取得
		AFileAcc	folder;
		GetAutoUpdateUserDataFolder("toolbar",folder);
		folder.CreateFolderRecursive();//再帰的にフォルダ生成
		//
		outToolPrefFile.SpecifyChild(folder,"toolpref.mi");
	}
	*/
}

void	AModePrefDB::MoveToolbarItem( const AIndex inOldIndex, const AIndex inNewIndex )
{
	mToolbarItem_Type.Move(inOldIndex,inNewIndex);
	mToolbarItem_File.MoveObject(inOldIndex,inNewIndex);
	//#724 mToolbarItem_IconID.Move(inOldIndex,inNewIndex);
	mToolbarItem_Enabled.Move(inOldIndex,inNewIndex);
	mToolbarItem_FilenameArray.MoveObject(inOldIndex,inNewIndex);
	//#844 mToolbarItem_AutoUpdate.Move(inOldIndex,inNewIndex);//#427
	//他言語タイトル
	AText	multiLangTitle;
	mToolbarItem_JapaneseName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_EnglishName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_FrenchName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_GermanName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_SpanishName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_ItalianName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_DutchName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_SwedishName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_NorwegianName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_DanishName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_FinnishName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_PortugueseName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_SimplifiedChineseName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_TraditionalChineseName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_KoreanName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_PolishName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_PortuguesePortugalName.MoveObject(inOldIndex,inNewIndex);
	mToolbarItem_RussianName.MoveObject(inOldIndex,inNewIndex);
	//
	WriteToolbarItemOrderFile();
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
	//#724 GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();//#232
}

//ツールバー名称変更
//B0406 ABool	AModePrefDB::RenameToolbarItem( const AIndex inIndex, const AText& inNewName )
ABool	AModePrefDB::SetToolbarName( const AFileAcc& inFile, const AText& inNewName, AFileAcc& outNewFile )
{
	outNewFile.CopyFrom(inFile);
	//ファイルからMenuObjectIDとIndexを検索
	AIndex	itemIndex = kIndex_Invalid;
	if( FindToolbarIndexByFile(inFile,itemIndex) == false )
	{
		_ACError("invalid toolbar file",NULL);
		return false;
	}
	//B0328
	AText	filename;
	inFile.GetFilename(filename);
	if( filename.Compare(inNewName) == true )   return false;
	//ファイル名変更
	if( outNewFile.Rename(inNewName) == false )   return false;
	//内部データ変更
	mToolbarItem_File.GetObject(itemIndex).CopyFrom(outNewFile);
	//orderファイル書き出し
	WriteToolbarItemOrderFile();
	//全ツールバー更新
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
	//#724 GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();//#232
	return true;
}

//ファイルからIndexを検索する B0406
ABool	AModePrefDB::FindToolbarIndexByFile( const AFileAcc& inFile, AIndex& outItemIndex ) const
{
	outItemIndex = kIndex_Invalid;
	AText	path;
	inFile.GetNormalizedPath(path);
	AText	p;
	for( AIndex index = 0; index < mToolbarItem_File.GetItemCount(); index++ )
	{
		AText	p;
		mToolbarItem_File.GetObjectConst(index).GetNormalizedPath(p);
		if( p.Compare(path) == true )
		{
			outItemIndex = index;
			return true;
		}
	}
	return false;
}

/*#724  void	AModePrefDB::ChangeToolbarItemIcon( const AIndex inIndex )
{
	AFileAcc	file;
	file.CopyFrom(mToolbarItem_File.GetObject(inIndex));
	CWindowImp::UnregisterDynamicIcon(mToolbarItem_IconID.Get(inIndex));
	AIconID	iconID = CWindowImp::RegisterDynamicIconFromFile(file);
	mToolbarItem_IconID.Add(iconID);
}*/

/**
ツールバー項目の表示・非表示を設定する

テキストウインドウ・モード設定ウインドウの表示も更新される
*/
void	AModePrefDB::SetEnableToolbarItem( const AIndex inIndex, const ABool inEnable )
{
	mToolbarItem_Enabled.Set(inIndex,inEnable);
	WriteToolbarItemOrderFile();
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
	//#724 GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();//#232
}

//#232
/**
全てのツールバー項目を表示状態にする

テキストウインドウ・モード設定ウインドウの表示も更新される
*/
void	AModePrefDB::SetEnableToolbarItemAll()
{
	AItemCount	itemCount = GetToolbarItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		mToolbarItem_Enabled.Set(i,true);
	}
	WriteToolbarItemOrderFile();
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
	//#724 GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();
}

//#619
/**
自動更新・ユーザー作成を指定して、全てのツールバー項目をEnable/Disableにする
*/
void	AModePrefDB::SetEnableToolbarItemAll( const ABool inEnable/*#844 , const ABool inAutoUpdateItems, const ABool inUserItems*/ )
{
	AItemCount	itemCount = GetToolbarItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		/*#844
		ABool	isTarget = false;
		if( mToolbarItem_AutoUpdate.Get(i) == true )
		{
			if( inAutoUpdateItems == true )
			{
				isTarget = true;
			}
		}
		else
		{
			if( inUserItems == true )
			{
				isTarget = true;
			}
		}
		if( isTarget == true )
		*/
		{
			mToolbarItem_Enabled.Set(i,inEnable);
		}
	}
	WriteToolbarItemOrderFile();
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
	//#724 GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();
}

//#305
/**
ツール多言語タイトル取得
*/
void	AModePrefDB::GetToolbarMultiLanguageName( const AIndex inItemIndex, const ALanguage inLanguage, AText& outName ) const
{
	//
	switch(inLanguage)
	{
	  case kLanguage_Japanese:
		{
			mToolbarItem_JapaneseName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_English:
		{
			mToolbarItem_EnglishName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_French:
		{
			mToolbarItem_FrenchName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_German:
		{
			mToolbarItem_GermanName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Spanish:
		{
			mToolbarItem_SpanishName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Italian:
		{
			mToolbarItem_ItalianName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Dutch:
		{
			mToolbarItem_DutchName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Swedish:
		{
			mToolbarItem_SwedishName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Norwegian:
		{
			mToolbarItem_NorwegianName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Danish:
		{
			mToolbarItem_DanishName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Finnish:
		{
			mToolbarItem_FinnishName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Portuguese:
		{
			mToolbarItem_PortugueseName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_SimplifiedChinese:
		{
			mToolbarItem_SimplifiedChineseName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_TraditionalChinese:
		{
			mToolbarItem_TraditionalChineseName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Korean:
		{
			mToolbarItem_KoreanName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Polish:
		{
			mToolbarItem_PolishName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_PortuguesePortugal:
		{
			mToolbarItem_PortuguesePortugalName.Get(inItemIndex,outName);
			break;
		}
	  case kLanguage_Russian:
		{
			mToolbarItem_RussianName.Get(inItemIndex,outName);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#305
/**
ツールバー表示名取得
*/
void	AModePrefDB::GetToolbarDisplayName( const AIndex inItemIndex, AText& outName ) const
{
	AToolbarItemType	type;
	AFileAcc	file;
	//AIconID	iconID;
	AText	name;
	ABool	enabled;
	GetToolbarItem(inItemIndex,type,file,name,enabled);
	//
	AText	multiLangName;
	GetToolbarMultiLanguageName(inItemIndex,GetApp().NVI_GetLanguage(),multiLangName);
	if( multiLangName.GetItemCount() == 0 )
	{
		outName.SetText(name);
	}
	else
	{
		outName.SetText(multiLangName);
	}
}

//#305
/**
ツール多言語タイトル設定
*/
void	AModePrefDB::SetToolbarMultiLanguageName( const AIndex inItemIndex, const ALanguage inLanguage, const AText& inName )
{
	//
	switch(inLanguage)
	{
	  case kLanguage_Japanese:
		{
			mToolbarItem_JapaneseName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_English:
		{
			mToolbarItem_EnglishName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_French:
		{
			mToolbarItem_FrenchName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_German:
		{
			mToolbarItem_GermanName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Spanish:
		{
			mToolbarItem_SpanishName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Italian:
		{
			mToolbarItem_ItalianName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Dutch:
		{
			mToolbarItem_DutchName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Swedish:
		{
			mToolbarItem_SwedishName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Norwegian:
		{
			mToolbarItem_NorwegianName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Danish:
		{
			mToolbarItem_DanishName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Finnish:
		{
			mToolbarItem_FinnishName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Portuguese:
		{
			mToolbarItem_PortugueseName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_SimplifiedChinese:
		{
			mToolbarItem_SimplifiedChineseName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_TraditionalChinese:
		{
			mToolbarItem_TraditionalChineseName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Korean:
		{
			mToolbarItem_KoreanName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Polish:
		{
			mToolbarItem_PolishName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_PortuguesePortugal:
		{
			mToolbarItem_PortuguesePortugalName.Set(inItemIndex,inName);
			break;
		}
	  case kLanguage_Russian:
		{
			mToolbarItem_RussianName.Set(inItemIndex,inName);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	WriteToolbarItemOrderFile();
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
	//#724 GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();
}

//#427
/**
ツールバー自動更新フラグ取得
*/
/*#844
ABool	AModePrefDB::GetToolbarAutoUpdateFlag( const AIndex inItemIndex )
{
	return mToolbarItem_AutoUpdate.Get(inItemIndex);
}
*/

//#724
/**
他と重複しないツールバー項目名を取得
*/
void	AModePrefDB::GetUniqToolbarName( AText& ioName ) const
{
	//ツールバーフォルダ取得
	AFileAcc	toolbarFolder;
	GetToolbarFolder(toolbarFolder);
	//ユニークなファイル名のファイル取得
	AFileAcc	file;
	file.SpecifyUniqChildFile(toolbarFolder,ioName);
	//ファイル名取得
	file.GetFilename(ioName);
}

/**
指定ツール名、ツールテキストで、マクロバー項目生成
*/
void	AModePrefDB::CreateNewToolbar( const AText& inToolName, const AText& inToolText )
{
	//改行(CR, LF)をスペースに変換
	AText	toolname(inToolName);
	toolname.ReplaceChar(kUChar_LineEnd,kUChar_Space);
	toolname.ReplaceChar(kUChar_LF,kUChar_Space);
	toolname.ReplaceChar(kUChar_CR,kUChar_Space);
	//ツールファイル生成、テキスト書き込み
	AFileAcc	toolbarFolder;
	GetApp().SPI_GetModePrefDB(mModeID).GetToolbarFolder(toolbarFolder);
	AFileAcc	file;
	file.SpecifyChild(toolbarFolder,toolname);
	file.CreateFile();
	file.WriteFile(inToolText);
	//ファイルattr書き込み
	AFileAttribute	attr;
	GetApp().SPI_GetToolFileAttribute(attr);
	file.SetFileAttribute(attr);
	//ツールバー項目追加
	GetApp().SPI_GetModePrefDB(mModeID).AddToolbarItem(kToolbarItemType_File,file,false);
	//モード設定更新
	GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();
}

/**
非表示マクロのリストを取得
*/
void	AModePrefDB::GetHiddenMacroBarItemNameArray( ATextArray& outNameArray ) const
{
	for( AIndex index = 0; index < mToolbarItem_File.GetItemCount(); index++ )
	{
		if( mToolbarItem_Enabled.Get(index) == false )
		{
			AText	name;
			mToolbarItem_File.GetObjectConst(index).GetFilename(name);
			outNameArray.Add(name);
		}
	}
}

#pragma mark ===========================

#pragma mark ---キーワード

/**
カテゴリ、および、キーワードリスト作成
*/
void	AModePrefDB::MakeCategoryArrayAndKeywordList()
{
	{
		//#717
		//排他制御
		AStMutexLocker	locker(mKeywordListMutex);
		//キーワードリスト全削除
		mKeywordList.DeleteAllIdentifiers();//#641
		//mUserKeywordIdentifierUniqIDArray.DeleteAll();//#890
	}
	//カテゴリデータ全削除
	mCategoryArray_Name.DeleteAll();
	mCategoryArray_Color.DeleteAll();
	mCategoryArray_ImportColor.DeleteAll();
	mCategoryArray_LocalColor.DeleteAll();
	mCategoryArray_Color_Dark.DeleteAll();//#1316
	mCategoryArray_ImportColor_Dark.DeleteAll();//#1316
	mCategoryArray_LocalColor_Dark.DeleteAll();//#1316
	mCategoryArray_TextStyle.DeleteAll();
	mCategoryArray_MenuTextStyle.DeleteAll();
	mCategoryArray_PriorToOtherColor.DeleteAll();//R0195
	AItemCount	statecount = mSyntaxDefinition.GetStateCount();
	{
		AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
		mCategoryDisabledStateArray.DeleteAll();//R0000 カテゴリー可否
		mCategoryDisabledStateArrayAllFalse = true;//R0000 カテゴリー可否
		
		//R0000 カテゴリー可否配列設定の要素数をstateの数だけ設定する
		for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
		{
			mCategoryDisabledStateArray.AddNewObject();
		}
	}
	mKeywordRegExp_Text.DeleteAll();
	mKeywordRegExp_CategoryIndex.DeleteAll();
	
	//==================SDFから追加（＝識別子のカテゴリ）==================
	/*#844 SDFの固定キーワードは廃止。SDFのカテゴリーは識別子のみに使用する。
	//文法定義ファイルからキーワード取得
	{
		//#717
		AStMutexLocker	locker(mKeywordListMutex);
		mSyntaxDefinition.GetKeywordData(mKeywordList);
	}
	*/
	//文法定義ファイルからカテゴリー名取得
	mCategoryArray_Name.SetFromTextArray(mSyntaxDefinition.GetCategoryNameArray());
	//カテゴリー色設定（モード設定優先、未設定なら文法定義ファイルのデフォルトカラー）
	for( AIndex i = 0; i < mCategoryArray_Name.GetItemCount(); i++ )
	{
		AText	name;
		mCategoryArray_Name.Get(i,name);
		AColor	color, importcolor, localcolor;
		AColor	color_dark, importcolor_dark, localcolor_dark;//#1316
		ATextStyle	menustyle = kTextStyle_Normal;
		//#844 ABool	styleBold, styleItalic, styleUnderline;
		ATextStyle	style = kTextStyle_Normal;
		//SDF定義カテゴリの色・スタイルを取得（色SDFに記述されたColorSlotに従って取得）
		GetSyntaxDefinitionCategoryColorByName(name,color,importcolor,localcolor,menustyle,style,false);//#844 styleBold,styleItalic,styleUnderline);//R0195
		GetSyntaxDefinitionCategoryColorByName(name,color_dark,importcolor_dark,localcolor_dark,menustyle,style,true);//#1316
		mCategoryArray_Color.Add(color);
		mCategoryArray_Color_Dark.Add(color_dark);//#1316
		/*#844 
		//R0195
		ATextStyle	style = kTextStyle_Normal;
		if( styleBold == true )
		{
			style |= kTextStyle_Bold;
		}
		if( styleUnderline == true )
		{
			style |= kTextStyle_Underline;
		}
		if( styleItalic == true )
		{
			style |= kTextStyle_Italic;
		}
		*/
		mCategoryArray_TextStyle.Add(style);
		mCategoryArray_MenuTextStyle.Add(menustyle);
		mCategoryArray_ImportColor.Add(importcolor);
		mCategoryArray_LocalColor.Add(localcolor);
		mCategoryArray_ImportColor_Dark.Add(importcolor_dark);//#1316
		mCategoryArray_LocalColor_Dark.Add(localcolor_dark);//#1316
		mCategoryArray_PriorToOtherColor.Add(false);//R0195
		//R0000 カテゴリー可否配列 全てのstateに、categoryを追加
		{
			AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
			for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
			{
				//全てのstateでenable
				//識別子は全ての文法パート内で有効にする
				//HTMLのidをJavaScript等で利用したりするため。将来的には設定で切り替えられるようにしたほうが良いかもしれない。
				mCategoryDisabledStateArray.GetObject(stateIndex).Add(false);
			}
		}
	}
	
	//==================見出しから追加==================
	
	//カテゴリリスト内の、見出しカテゴリの開始位置を記憶
	mJumpSetupCategoryStartIndex = mCategoryArray_Name.GetItemCount();
	
	//各見出し設定ごとのループ
	for( AIndex i = 0; i < GetItemCount_TextArray(AModePrefDB::kJumpSetup_RegExp); i++ )
	{
		AText	name;
		GetData_TextArray(kJumpSetup_Name,i,name);
		mCategoryArray_Name.Add(name);
		AColor	color = kColor_Black, color_dark = kColor_Black;//#1316
		ATextStyle	style = kTextStyle_Normal;
		//#889 GetData_ColorArray(kJumpSetup_KeywordColor,i,color);
		AIndex	colorSlot = GetData_NumberArray(AModePrefDB::kJumpSetup_KeywordColorSlot,i);//#889
		GetColorSlotData(colorSlot,color,style,false);//#889
		GetColorSlotData(colorSlot,color_dark,style,true);//#1316
		mCategoryArray_Color.Add(color);
		mCategoryArray_ImportColor.Add(kColor_Black);
		mCategoryArray_LocalColor.Add(kColor_Black);
		mCategoryArray_Color_Dark.Add(color_dark);//#1316
		mCategoryArray_ImportColor_Dark.Add(kColor_Black);//#1316
		mCategoryArray_LocalColor_Dark.Add(kColor_Black);//#1316
		mCategoryArray_TextStyle.Add(style);
		mCategoryArray_MenuTextStyle.Add(kTextStyle_Normal);
		mCategoryArray_PriorToOtherColor.Add(false);//R0195
		//R0000 カテゴリー可否配列 全てのstateに、categoryを追加
		{
			AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
			for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
			{
				//全てのstateでenable
				mCategoryDisabledStateArray.GetObject(stateIndex).Add(false);
			}
		}
	}
	
	//==================ユーザー追加Keywordを追加==================
	
	//カテゴリリスト内の、ユーザーキーワードのカテゴリの開始位置を記憶
	mAdditionalCategoryStartIndex = mCategoryArray_Name.GetItemCount();
	
	//ユーザーキーワードをカテゴリリストに追加、キーワードリストの作成
	AddNormalKeywordCategoryAndKeyword();
	
	//==================SDFでStateAttribute_DisableAllCategoryを指定されたstateは全てのcategoryをdisableにする==================
	{
		AItemCount	statecount = mSyntaxDefinition.GetStateCount();
		for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
		{
			if( mSyntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetAllCategoryDisabled() == true )
			{
				AItemCount	categoryCount = mCategoryDisabledStateArray.GetObject(stateIndex).GetItemCount();
				for( AIndex i = 0; i < categoryCount; i++ )
				{
					mCategoryDisabledStateArray.GetObject(stateIndex).Set(i,true);
				}
			}
		}
	}
}

//#890
/**
ユーザーキーワードの更新時の処理（カテゴリリスト内のキーワードのプロパティの更新処理など）
*/
void	AModePrefDB::UpdateUserKeywordCategoryAndKeyword()
{
	//アルファベットテーブル更新
	UpdateIsAlphabetTable();
	
	//#725
	//サブウインドウ内のモードに関連するデータのクリア
	GetApp().SPI_ClearSubWindowsModeRelatedData(mModeID);
	
	{
		//#717
		AStMutexLocker	locker(mKeywordListMutex);
		//登録したキーワードを全てキーワードリストから削除
		mKeywordList.DeleteAllIdentifiers();
		/*
		AItemCount	itemCount = mUserKeywordIdentifierUniqIDArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			mKeywordList.DeleteIdentifiers(mUserKeywordIdentifierUniqIDArray.Get(i));
		}
		mUserKeywordIdentifierUniqIDArray.DeleteAll();
		*/
	}
	//カテゴリリスト内のユーザーキーワードのデータ削除
	mCategoryArray_Name.DeleteAfter(mAdditionalCategoryStartIndex);
	mCategoryArray_Color.DeleteAfter(mAdditionalCategoryStartIndex);
	mCategoryArray_ImportColor.DeleteAfter(mAdditionalCategoryStartIndex);
	mCategoryArray_LocalColor.DeleteAfter(mAdditionalCategoryStartIndex);
	mCategoryArray_Color_Dark.DeleteAfter(mAdditionalCategoryStartIndex);//#1316
	mCategoryArray_ImportColor_Dark.DeleteAfter(mAdditionalCategoryStartIndex);//#1316
	mCategoryArray_LocalColor_Dark.DeleteAfter(mAdditionalCategoryStartIndex);//#1316
	mCategoryArray_TextStyle.DeleteAfter(mAdditionalCategoryStartIndex);
	mCategoryArray_MenuTextStyle.DeleteAfter(mAdditionalCategoryStartIndex);
	mCategoryArray_PriorToOtherColor.DeleteAfter(mAdditionalCategoryStartIndex);
	//カテゴリ可否のユーザーキーワードのデータ削除
	{
		AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
		AItemCount	statecount = mSyntaxDefinition.GetStateCount();
		for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
		{
			mCategoryDisabledStateArray.GetObject(stateIndex).DeleteAfter(mAdditionalCategoryStartIndex);
		}
	}
	//正規表現キーワードのデータ削除
	mKeywordRegExp_Text.DeleteAll();
	mKeywordRegExp_CategoryIndex.DeleteAll();
	
	//ユーザーキーワードデータ追加
	AddNormalKeywordCategoryAndKeyword();
	
	//#683 各ドキュメントのTextInfo内のRegExpオブジェクトを更新する
	GetApp().SPI_UpdateRegExpForAllDocuments(mModeID);
	
	//==================SDFでStateAttribute_DisableAllCategoryを指定されたstateは全てのcategoryをdisableにする==================
	{
		AItemCount	statecount = mSyntaxDefinition.GetStateCount();
		for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
		{
			if( mSyntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetAllCategoryDisabled() == true )
			{
				AItemCount	categoryCount = mCategoryDisabledStateArray.GetObject(stateIndex).GetItemCount();
				for( AIndex i = 0; i < categoryCount; i++ )
				{
					mCategoryDisabledStateArray.GetObject(stateIndex).Set(i,true);
				}
			}
		}
	}
}

/**
ユーザーキーワードをカテゴリリストに追加、および、キーワードリストの作成
*/
void	AModePrefDB::AddNormalKeywordCategoryAndKeyword()
{
	//キーワードファイル(.import)で指定したヘッダファイルフォルダ格納メンバ変数をクリア
	mImportFilePathsByKeywordCategory.DeleteAll();
	
	//state数取得
	AItemCount	statecount = mSyntaxDefinition.GetStateCount();
	//ユーザーキーワード各カテゴリ毎のループ
	for( AIndex i = 0; 
				i < GetItemCount_Array(kAdditionalCategory_Keywords);//#427 mAdditionalCategoryArray_KeywordArray.GetItemCount(); 
				i++ )
	{
		//#427 キーワードテキストをtextarrayに展開
		ATextArray	keywordArray;
		keywordArray.ExplodeFromText(GetData_TextArrayRef(kAdditionalCategory_Keywords).GetTextConst(i),
									 kUChar_LineEnd);
		
		//========================カテゴリリストに追加========================
		AIndex	categoryIndex = mCategoryArray_Name.GetItemCount();
		AText	name;
		GetData_TextArray(kAdditionalCategory_Name,i,name);
		mCategoryArray_Name.Add(name);
		AColor	color = kColor_Black, color_dark = kColor_Black;//#1316
		ATextStyle	style = kTextStyle_Normal;
		//#889 GetData_ColorArray(kAdditionalCategory_Color,i,color);
		AIndex	colorSlot = GetData_NumberArray(kAdditionalCategory_ColorSlot,i);//#889
		GetColorSlotData(colorSlot,color,style,false);//#889
		GetColorSlotData(colorSlot,color_dark,style,true);//#1316
		mCategoryArray_Color.Add(color);
		mCategoryArray_ImportColor.Add(kColor_Black);
		mCategoryArray_LocalColor.Add(kColor_Black);
		mCategoryArray_Color_Dark.Add(color_dark);//#1316
		mCategoryArray_ImportColor_Dark.Add(kColor_Black);//#1316
		mCategoryArray_LocalColor_Dark.Add(kColor_Black);//#1316
		/*#889
		if( GetData_BoolArray(kAdditionalCategory_Bold,i) == true )
		{
			style |= kTextStyle_Bold;
		}
		if( GetData_BoolArray(kAdditionalCategory_Underline,i) == true )//R0195
		{
			style |= kTextStyle_Underline;
		}
		if( GetData_BoolArray(kAdditionalCategory_Italic,i) == true )//R0195
		{
			style |= kTextStyle_Italic;
		}
		*/
		mCategoryArray_TextStyle.Add(style);
		mCategoryArray_MenuTextStyle.Add(kTextStyle_Normal);
		mCategoryArray_PriorToOtherColor.Add(GetData_BoolArray(kAdditionalCategory_PriorToOtherColor,i));//R0195
		
		//========================カテゴリ可否に追加========================
		//R0000 カテゴリー可否配列 全てのstateに、categoryを追加
		//「無効にする」チェックボックスの値を取得
		ABool	alldisable = GetData_BoolArray(kAdditionalCategory_Disabled,i);
		//有効文法パートを取得
		AText	syntaxPartText;
		GetData_TextArray(kAdditionalCategory_ValidSyntaxPart,i,syntaxPartText);
		AIndex	syntaxPartIndex = mSyntaxDefinition.GetSyntaxPartIndexFromName(syntaxPartText);
		//各stateについてのループ
		for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
		{
			ABool	disabled = false;
			if( syntaxPartIndex != kIndex_Invalid )
			{
				//有効文法パートの指定があるときは、その文法パートのカテゴリのみenableにする
				if( mSyntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex() == syntaxPartIndex )
				{
					//現在のstateの文法パートが、設定された文法パートと一致すればenable
					disabled = false;
				}
				else
				{
					//現在のstateの文法パートが、設定された文法パートと一致しなければdisable
					disabled = true;
				}
			}
			//「無効にする」がONのときは全カテゴリをdisableにする
			if( alldisable == true )
			{
				disabled = true;
			}
			{
				AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
				//１つでも有効なものがあれば、mCategoryDisabledStateArrayAllFalseフラグはOFFにする
				if( disabled == true )
				{
					mCategoryDisabledStateArrayAllFalse = false;
				}
				//カテゴリ可否に追加
				mCategoryDisabledStateArray.GetObject(stateIndex).Add(disabled);
			}
		}
		
		//========================キーワードリストに追加========================
		//キーワードカテゴリが有効の場合のみキーワード追加 #713
		if( GetData_BoolArray(kAdditionalCategory_Disabled,i) == false )
		{
			//------------------CSVキーワード------------------
			if( GetData_BoolArray(kAdditionalCategory_UseCSV,i) == true )
			{
				AText	modeName;
				GetModeRawName(modeName);
				AText	csvpath;
				GetData_TextArray(kAdditionalCategory_CSVPath,i,csvpath);
				if( csvpath.GetItemCount() > 0 )
				{
					//------------------リモートファイルの場合------------------
					if( csvpath.CompareMin("http://") == true && csvpath.GetItemCount() > 7 )
					{
						//httpファイルロード要求
						//パスの例："http://proj.mimikaki.net/mi/report/1009?format=csv&USER=anonymous"
						GetApp().SPI_LoadCSVForKeyword(modeName,csvpath,categoryIndex);
					}
					//------------------ローカルファイルの場合------------------
					else
					{
						//設定パスからCSVファイルを検索（優先順位に従って）
						AFileAcc	file;
						GetKeywordCSVFileFromPath(csvpath,file);
						//ファイルが存在していたら、そのファイルからキーワードをキーワードリストに追加
						if( file.Exist() == true )
						{
							//拡張子取得
							AText	path;
							file.GetPath(path);
							AText	suffix;
							path.GetSuffix(suffix);
							//拡張子による処理分岐
							if( suffix.Compare(".import") == true )
							{
								//キーワードファイル(.import)で指定したヘッダファイルフォルダを記述したファイル #1065
								AText	text;
								file.ReadTo(text);
								text.Add(kUChar_LF);
								mImportFilePathsByKeywordCategory.AddText(text);
							}
							else
							{
								//CSVファイル
								AText	csvtext;
								//file.ReadTo(csvtext);
								GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_KeywordCSVFile,file,csvtext);
								StartAddCSVKeywordThread(csvtext,categoryIndex);
							}
						}
					}
				}
			}
			//------------------通常キーワード------------------
			else
			{
				AItemCount	keywordCount = keywordArray.GetItemCount();//#427
				if( GetData_BoolArray(kAdditionalCategory_RegExp,i) == false )
				{
					for( AIndex j = 0; j < keywordCount; j++ )
					{
						const AText&	keyword = keywordArray.GetTextConst(j);
						//#147 キーワード説明検索し、存在すればInfoTextに設定
						AText	infotext;
						AArray<AIndex>	keywordExplanationIndexArray;
						mKeywordExplanationTextArray_Keyword.FindAll(keyword,0,keyword.GetItemCount(),keywordExplanationIndexArray);
						for( AIndex k = 0; k < keywordExplanationIndexArray.GetItemCount(); k++ )
						{
							if( name.Compare(mKeywordExplanationTextArray_CategoryName.GetTextConst(keywordExplanationIndexArray.Get(k))) == true )
							{
								infotext.SetText(keyword);
								infotext.AddCstring(" : ");
								infotext.AddText(mKeywordExplanationTextArray_Explanation.GetTextConst(keywordExplanationIndexArray.Get(k)));
							}
						}
						{
							//#717
							AStMutexLocker	locker(mKeywordListMutex);
							//キーワードリストにキーワード追加
							//（通常キーワードの場合、識別子個別カラースロット指定はなし。カラースロットはカテゴリで設定。）
							AUniqID	uniqID = mKeywordList.AddIdentifier(kUniqID_Invalid,kUniqID_Invalid,kIdentifierType_AdditionalKeyword,
																		categoryIndex,keyword,GetEmptyText(),infotext,0,0,
																		GetData_BoolArray(kAdditionalCategory_IgnoreCase,i),
																		GetEmptyText(),GetEmptyText(),GetEmptyText(),
																		GetEmptyText(),GetEmptyText(),
																		false,0,kIndex_Invalid);//B0368, RC2 #593
							//mUserKeywordIdentifierUniqIDArray.Add(uniqID);
						}
					}
				}
				else if( keywordCount > 0 )
				{
					//------------------正規表現キーワード------------------
					AText	text;
					//#427 mAdditionalCategoryArray_KeywordArray.GetObject(i).Get(0,text);
					keywordArray.Get(0,text);//#427
					mKeywordRegExp_Text.Add(text);
					mKeywordRegExp_CategoryIndex.Add(categoryIndex);
				}
			}
		}
	}
	
}

/*
CSVキーワード読み込みスレッド開始*
*/
void	AModePrefDB::StartAddCSVKeywordThread( const AText& inResultText, const AIndex inCategoryIndex )
{
	ACSVKeywordLoaderFactory	factory(NULL);
	AObjectID	threadObjectID = GetApp().NVI_CreateThread(factory);
	(dynamic_cast<ACSVKeywordLoader&>(GetApp().NVI_GetThreadByID(threadObjectID))).SPNVI_Run(mModeID,inResultText,inCategoryIndex);
}

//#791
/**
CSVテキストからキーワードをキーワードリストに追加する
*/
void	AModePrefDB::AddCategoryKeywordListFromCSV( const AText& inResultText, const AIndex inCategoryIndex, ABool& inAbortFlag )
{
	//途中カテゴリが削除されるなどして、カテゴリが無くなる、または、ずれてCSVキーワードで無くなった場合は、何もせずリターン
	//（URLから取得の場合、非同期なので、（低確率ではあるが）、途中でカテゴリ削除されたりしうる）
	//（たまたま別の、CSVを使っているカテゴリに入ってしまうことはありうるが、次回起動時にはちゃんと元に戻るので、対応しない）
	AIndex	additionalCategoryIndex = inCategoryIndex - mAdditionalCategoryStartIndex;
	if( additionalCategoryIndex < 0 || additionalCategoryIndex >= GetItemCount_BoolArray(kAdditionalCategory_UseCSV) )
	{
		return;
	}
	if( GetData_BoolArray(kAdditionalCategory_UseCSV,additionalCategoryIndex) == false )
	{
		return;
	}
	
	//CSVキーワードカテゴリの各パラメータを取得
	AText	categoryName;
	mCategoryArray_Name.Get(inCategoryIndex,categoryName);
	AIndex	keywordColumn = GetData_NumberArray(kAdditionalCategory_KeywordColumn,additionalCategoryIndex);
	AIndex	explanationColumn = GetData_NumberArray(kAdditionalCategory_ExplanationColumn,additionalCategoryIndex);
	AIndex	typeTextColumn = GetData_NumberArray(kAdditionalCategory_TypeTextColumn,additionalCategoryIndex);
	AIndex	parentKeywordColumn = GetData_NumberArray(kAdditionalCategory_ParentKeywordColumn,additionalCategoryIndex);
	AIndex	colorSlotColumn = GetData_NumberArray(kAdditionalCategory_ColorSlotColumn,additionalCategoryIndex);
	AIndex	completionTextColumn = GetData_NumberArray(kAdditionalCategory_CompletionTextColumn,additionalCategoryIndex);
	AIndex	urlColumn = GetData_NumberArray(kAdditionalCategory_URLColumn,additionalCategoryIndex);
	ABool	ignoreCase = GetData_BoolArray(kAdditionalCategory_IgnoreCase,additionalCategoryIndex);
	AIndex	defaultColorSlot = GetData_NumberArray(kAdditionalCategory_ColorSlot,additionalCategoryIndex);
	//CSVファイル解析、各カラム取得
	ACSVFile	csvFile(inResultText,ATextEncodingWrapper::GetUTF8TextEncoding());
	ATextArray	keywordArray, infoTextArray, typeTextArray, parentKeywordArray, colorSlotTextArray;
	ATextArray	completionTextArray, urlArray;
	if( keywordColumn >= 1 )
	{
		csvFile.GetColumn(keywordColumn-1,keywordArray);
	}
	if( explanationColumn >= 1 )
	{
		csvFile.GetColumn(explanationColumn-1,infoTextArray);
	}
	if( typeTextColumn >= 1 )
	{
		csvFile.GetColumn(typeTextColumn-1,typeTextArray);
	}
	if( parentKeywordColumn >= 1 )
	{
		csvFile.GetColumn(parentKeywordColumn-1,parentKeywordArray);
	}
	if( colorSlotColumn >= 1 )
	{
		csvFile.GetColumn(colorSlotColumn-1,colorSlotTextArray);
	}
	if( completionTextColumn >= 1 )
	{
		csvFile.GetColumn(completionTextColumn-1,completionTextArray);
	}
	if( urlColumn >= 1 )
	{
		csvFile.GetColumn(urlColumn-1,urlArray);
	}
	//各キーワードごとの処理
	for( AIndex index = 0; index < keywordArray.GetItemCount(); index++ )
	{
		//スレッドがabortされたらループ中断
		if( inAbortFlag == true )
		{
			break;
		}
		
		AText	keyword, infotext, typetext, parentKeyword , completionText, url;
		keywordArray.Get(index,keyword);
		//キーワードテキストが空なら何もしない
		if( keyword.GetItemCount() == 0 )
		{
			continue;
		}
		//各テキスト取得
		if( index < infoTextArray.GetItemCount() )
		{
			infoTextArray.Get(index,infotext);
		}
		if( index < typeTextArray.GetItemCount() )
		{
			typeTextArray.Get(index,typetext);
		}
		if( index < parentKeywordArray.GetItemCount() )
		{
			parentKeywordArray.Get(index,parentKeyword);
		}
		if( index < completionTextArray.GetItemCount() )
		{
			completionTextArray.Get(index,completionText);
		}
		if( index < urlArray.GetItemCount() )
		{
			urlArray.Get(index,url);
		}
		//カラースロット取得
		AIndex	colorSlot = kKeywordColorSlotIndexMin;
		if( index < colorSlotTextArray.GetItemCount() )
		{
			AText	colorSlotText;
			colorSlotTextArray.Get(index,colorSlotText);
			colorSlot = AColorSchemeDB::GetColorSlotIndexFromName(colorSlotText);
			if( colorSlot == kIndex_Invalid )
			{
				colorSlot = defaultColorSlot;
			}
		}
		//キーワードリストにキーワード追加
		{
			//#717
			AStMutexLocker	locker(mKeywordListMutex);
			//キーワード追加
			AUniqID	uniqID = mKeywordList.AddIdentifier(kUniqID_Invalid,kUniqID_Invalid,kIdentifierType_AdditionalKeyword,
														inCategoryIndex,keyword,GetEmptyText(),infotext,0,0,
														ignoreCase,
														GetEmptyText(),parentKeyword,typetext,
														completionText,url,
														false,0,colorSlot);
			//mUserKeywordIdentifierUniqIDArray.Add(uniqID);
		}
	}
}

//#844
/**
文法定義の各カテゴリ色更新
*/
void	AModePrefDB::UpdateSyntaxDefinitionCategoryColor()
{
	AItemCount	itemCount = mSyntaxDefinition.GetCategoryNameArray().GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AText	name;
		mCategoryArray_Name.Get(i,name);
		AColor	color = kColor_Black, importcolor = kColor_Black, localcolor = kColor_Black;
		AColor	color_dark = kColor_Black, importcolor_dark = kColor_Black, localcolor_dark = kColor_Black;//#1316
		ATextStyle	menustyle = kTextStyle_Normal;
		ATextStyle	style = kTextStyle_Normal;
		//SDF定義カテゴリの色・スタイルを取得（色SDFに記述されたColorSlotに従って取得）
		GetSyntaxDefinitionCategoryColorByName(name,color,importcolor,localcolor,menustyle,style,false);
		GetSyntaxDefinitionCategoryColorByName(name,color_dark,importcolor_dark,localcolor_dark,menustyle,style,true);//#1316
		mCategoryArray_Color.Set(i,color);
		mCategoryArray_Color_Dark.Set(i,color_dark);//#1316
		mCategoryArray_TextStyle.Set(i,style);
		mCategoryArray_MenuTextStyle.Set(i,menustyle);
		mCategoryArray_ImportColor.Set(i,importcolor);
		mCategoryArray_LocalColor.Set(i,localcolor);
		mCategoryArray_ImportColor_Dark.Set(i,importcolor_dark);//#1316
		mCategoryArray_LocalColor_Dark.Set(i,localcolor_dark);//#1316
	}
}

//#889
/**
ユーザーキーワードの各カテゴリ色更新
*/
void	AModePrefDB::UpdateUserKeywordCategoryColor()
{
	//ユーザーキーワードカテゴリ毎ループ
	for( AIndex i = 0; 
				i < GetItemCount_Array(kAdditionalCategory_Color);
				i++ )
	{
		AColor	color = kColor_Black, color_dark = kColor_Black;
		ATextStyle	style = kTextStyle_Normal;
		AIndex	colorSlot = GetData_NumberArray(kAdditionalCategory_ColorSlot,i);
		GetColorSlotData(colorSlot,color,style,false);
		GetColorSlotData(colorSlot,color_dark,style,true);//#1316
		mCategoryArray_Color.Set(mAdditionalCategoryStartIndex+i,color);
		mCategoryArray_Color_Dark.Set(mAdditionalCategoryStartIndex+i,color_dark);//#1316
		mCategoryArray_TextStyle.Set(mAdditionalCategoryStartIndex+i,style);
	}
}

//#877
/**
カテゴリ可否取得
*/
ABool	AModePrefDB::GetCategoryDisabled( const AIndex inStateIndex, const AIndex inCategoryIndex ) const
{
	AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
	
	if( mCategoryDisabledStateArrayAllFalse == true )
	{
		return false;
	}
	else
	{
		return mCategoryDisabledStateArray.GetObjectConst(inStateIndex).Get(inCategoryIndex);
	}
}

void	AModePrefDB::GetKeywordRegExp( AObjectArray<ARegExp>& outRegExpArray, AArray<AIndex>& outCategoryIndexArray ) const
{
	outRegExpArray.DeleteAll();
	outCategoryIndexArray.DeleteAll();
	for( AIndex i = 0; i < mKeywordRegExp_Text.GetItemCount(); i++ )
	{
		outRegExpArray.GetObject(outRegExpArray.AddNewObject()).SetRE(mKeywordRegExp_Text.GetTextConst(i));
		outCategoryIndexArray.Add(mKeywordRegExp_CategoryIndex.Get(i));
	}
}

//キーワード検索
ABool	AModePrefDB::FindKeyword( const AText& inKeyword, AColor& outColor, ATextStyle& outStyle, ABool& outPriorToOtherColor,
		const AIndex inStateIndex )//R0195 R0000 カテゴリー可否
{
	//キーワードリストからキーワード検索
	AIndex	colorSlot = kIndex_Invalid;
	AIndex categoryIndex = kIndex_Invalid;
	{
		AStMutexLocker	keywordlistlocker(mKeywordListMutex);
		AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
		categoryIndex = mKeywordList.
				GetCategoryIndexByKeywordText(inKeyword,
											  mCategoryDisabledStateArrayAllFalse,
											  mCategoryDisabledStateArray.GetObjectConst(inStateIndex),colorSlot);
	}
	if( categoryIndex == kIndex_Invalid )   return false;
	
	//キーワード毎のカラースロット指定があればそちらを優先
	if( colorSlot != kIndex_Invalid )
	{
		//IdentifierListにColorSlotが設定されている場合（＝CSVキーワード）、
		//そちらを優先する。
		GetColorSlotData(colorSlot,outColor,outStyle,GetApp().NVI_IsDarkMode());
	}
	else
	{
		//IdentifierListにColorSlotが設定されていない場合（＝CSVキーワード以外）、
		//カテゴリに設定された色・スタイルを取得
		if( GetApp().NVI_IsDarkMode() == false )
		{
			outColor = mCategoryArray_Color.Get(categoryIndex);
		}
		else
		{
			outColor = mCategoryArray_Color_Dark.Get(categoryIndex);
		}
		outStyle = mCategoryArray_TextStyle.Get(categoryIndex);
	}
	//
	outPriorToOtherColor = mCategoryArray_PriorToOtherColor.Get(categoryIndex);//R0195
	return true;
}

//
void	AModePrefDB::GetCategoryColor( const AIndex inCategoryIndex, AColor& outColor ) const
{
	if( GetApp().NVI_IsDarkMode() == false )
	{
		outColor = mCategoryArray_Color.Get(inCategoryIndex);
	}
	else
	{
		outColor = mCategoryArray_Color_Dark.Get(inCategoryIndex);
	}
}

//
ATextStyle	AModePrefDB::GetCategoryTextStyle( const AIndex inCategoryIndex ) const
{
	return mCategoryArray_TextStyle.Get(inCategoryIndex);
}

//
void	AModePrefDB::GetCategoryImportColor( const AIndex inCategoryIndex, AColor& outColor ) const
{
	if( GetApp().NVI_IsDarkMode() == false )
	{
		outColor = mCategoryArray_ImportColor.Get(inCategoryIndex);
	}
	else
	{
		outColor = mCategoryArray_ImportColor_Dark.Get(inCategoryIndex);
	}
}

//
void	AModePrefDB::GetCategoryLocalColor( const AIndex inCategoryIndex, AColor& outColor ) const
{
	if( GetApp().NVI_IsDarkMode() == false )
	{
		outColor = mCategoryArray_LocalColor.Get(inCategoryIndex);
	}
	else
	{
		outColor = mCategoryArray_LocalColor_Dark.Get(inCategoryIndex);
	}
}

//
ATextStyle	AModePrefDB::GetCategoryMenuTextStyle( const AIndex inCategoryIndex ) const
{
	return mCategoryArray_MenuTextStyle.Get(inCategoryIndex);
}

//R0195
ABool	AModePrefDB::GetCategoryPriorToOtherColor( const AIndex inCategoryIndex ) const
{
	return mCategoryArray_PriorToOtherColor.Get(inCategoryIndex);
}

//
void	AModePrefDB::GetCategoryName( const AIndex inCategoryIndex, AText& outName ) const
{
	mCategoryArray_Name.Get(inCategoryIndex,outName);
}

void	AModePrefDB::GetUniqCategoryName( const AText& inName, AText& outName ) const
{
	if( mCategoryArray_Name.Find(inName) == kIndex_Invalid )
	{
		outName.SetText(inName);
		return;
	}
	for( AIndex i = 2; i < 99999; i++ )
	{
		outName.SetText(inName);
		AText	numtext;
		numtext.SetFormattedCstring(" %d",i);
		outName.AddText(numtext);
		if( mCategoryArray_Name.Find(outName) == kIndex_Invalid )   return;
	}
}

//#890
/**
カテゴリ数取得
*/
AItemCount	AModePrefDB::GetCategoryCount() const
{
	return mCategoryArray_Name.GetItemCount();
}

/**
CSVファイル設定パスから（優先順位に従って）ファイルを検索する
*/
void	AModePrefDB::GetKeywordCSVFileFromPath( const AText& inPath, AFileAcc& outFile ) const
{
	//優先順位
	//1.絶対パス
	//2.keyword/（各言語）フォルダ（＝ユーザー用）
	//3.keywordフォルダ（＝ユーザー用）
	//4.autoupdate/keyword/（各言語）フォルダ
	//5.autoupdate/keyword/フォルダ
	
	//1.絶対パス
	outFile.Specify(inPath);
	if( outFile.Exist() == false )
	{
		//keywordフォルダ取得
		AFileAcc	modefolder;
		GetModeFolder(modefolder);
		AFileAcc	keywordFolder;
		keywordFolder.SpecifyChild(modefolder,"keyword");
		//各言語フォルダ取得
		AText	langText;
		GetApp().NVI_GetLanguageName(langText);
		AFileAcc	langFolder;
		langFolder.SpecifyChild(keywordFolder,langText);
		//2.keyword/（各言語）フォルダ（＝ユーザー用）
		outFile.SpecifyChild(langFolder,inPath);
		if( outFile.Exist() == false )
		{
			//3.keywordフォルダ（＝ユーザー用）
			outFile.SpecifyChild(keywordFolder,inPath);
			if( outFile.Exist() == false )
			{
				//auto updateフォルダ取得
				AFileAcc	autoUpdateFolder;
				GetAutoUpdateFolder(autoUpdateFolder);
				//autoupdate/keywordフォルダ取得
				AFileAcc	autoUpdateKeywordFolder;
				autoUpdateKeywordFolder.SpecifyChild(autoUpdateFolder,"keyword");
				//autoupdate/keyword/各言語フォルダ取得
				AFileAcc	autoupdateKeywordLangFolder;
				autoupdateKeywordLangFolder.SpecifyChild(autoUpdateKeywordFolder,langText);
				//4.autoupdate/keyword/（各言語）フォルダ
				outFile.SpecifyChild(autoupdateKeywordLangFolder,inPath);
				if( outFile.Exist() == false )
				{
					//5.autoupdate/keyword/フォルダ
					outFile.SpecifyChild(autoUpdateKeywordFolder,inPath);
				}
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---キーワード説明
//#147

//#427
/**
キーワード説明データロード
*/
void	AModePrefDB::LoadKeywordsExplanation()
{
	//dataフォルダ取得
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	//説明データ配列全削除
	mKeywordExplanationTextArray_Keyword.DeleteAll();
	mKeywordExplanationTextArray_CategoryName.DeleteAll();
	mKeywordExplanationTextArray_Explanation.DeleteAll();
	//キーワード説明ファイル読み込み
	/*#844 autoupdate配下のkeywords_explanation.txtは使用しない
	//AutoUpdateデータ #427
	AFileAcc	autoUpdateFolder;
	GetAutoUpdateFolder(autoUpdateFolder);
	//autoupdate/data/keywords_explanation.txtファイル
	AFileAcc	keywordExplanationFile_AutoUpdate;
	keywordExplanationFile_AutoUpdate.SpecifyChild(autoUpdateFolder,"data/keywords_explanation.txt");//#427
	if( keywordExplanationFile_AutoUpdate.Exist() == true )
	{
		AText	keywordExplanationText;
		keywordExplanationFile_AutoUpdate.ReadTo(keywordExplanationText);
		keywordExplanationText.ConvertLineEndToCR();
		ParseKeywordExplanationText(keywordExplanationText);
	}
	*/
	//ユーザーデータ
	AFileAcc	keywordExplanationFile;
	keywordExplanationFile.SpecifyChild(dataFolder,"keywords_explanation.txt");
	if( keywordExplanationFile.Exist() == true )
	{
		AText	keywordExplanationText;
		keywordExplanationFile.ReadTo(keywordExplanationText);
		keywordExplanationText.ConvertLineEndToCR();
		ParseKeywordExplanationText(keywordExplanationText);
	}
}

/**
キーワード説明ファイルParse
*/
void	AModePrefDB::ParseKeywordExplanationText( const AText& inDataText )
{
	//
	AItemCount	itemCount = inDataText.GetItemCount();
	for( AIndex pos = 0; pos < itemCount;  )
	{
		for( ; pos < itemCount; pos++ )
		{
			if( inDataText.Get(pos) == '[' )
			{
				pos++;
				break;
			}
		}
		if( pos >= itemCount )   break;
		AIndex	categoryNameSpos = pos;
		for( ; pos < itemCount; pos++ )
		{
			if( inDataText.Get(pos) == ']' )
			{
				break;
			}
		}
		if( pos >= itemCount )   break;
		AText	categoryName;
		inDataText.GetText(categoryNameSpos,pos-categoryNameSpos,categoryName);
		pos++;
		if( inDataText.SkipTabSpace(pos,itemCount) == false )   break;
		AIndex	keywordSpos = pos;
		for( ; pos < itemCount; pos++ )
		{
			if( inDataText.Get(pos) == kUChar_CR )
			{
				break;
			}
		}
		AText	keyword;
		inDataText.GetText(keywordSpos,pos-keywordSpos,keyword);
		pos++;
		//この時点でposは行頭
		//次の改行+[までを説明とする
		AIndex	explanationSpos = pos;
		for( ; pos < itemCount; pos++ )
		{
			if( inDataText.Get(pos) == '[' )
			{
				if( inDataText.Get(pos-1) == kUChar_CR )
				break;
			}
		}
		if( explanationSpos < pos )
		{
			AText	explanation;
			inDataText.GetText(explanationSpos,pos-explanationSpos,explanation);
			//説明登録
			mKeywordExplanationTextArray_Keyword.Add(keyword);
			mKeywordExplanationTextArray_CategoryName.Add(categoryName);
			mKeywordExplanationTextArray_Explanation.Add(explanation);
		}
	}
}

#pragma mark ===========================

#pragma mark ---キーワード情報検索

//#853
/**
システムヘッダから各種キーワード検索スレッド用統合キーワード検索処理(Import識別子)
@note ハッシュを使わないので低速なため、スレッド使用が必須だが、いろいろな検索方法ができる。thread-safe
*/
ABool	AModePrefDB::SPI_SearchKeywordSystemHeader( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
												   const AIndex inCurrentStateIndex,
												   AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
												   ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
												   ATextArray& outCommentTextArray,
												   ATextArray& outCompletionTextArray, ATextArray& outURLArray,
												   AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
												   AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
												   AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
												   AArray<AItemCount>& outMatchedCountArray,
												   const ABool& inAbort  ) const
{
	//排他制御
	AStMutexLocker	locker(mSystemHeaderIdentifierLinkListMutex);
	//キーワード検索
	return mSystemHeaderIdentifierLinkList.SearchKeywordImport(inWord,inParentWord,inOption,inCurrentStateIndex,
		outKeywordArray,outParentKeywordArray,
		outTypeTextArray,outInfoTextArray,outCommentTextArray,
		outCompletionTextArray,outURLArray,
		outCategoryIndexArray,outColorSlotIndexArray,
		outStartIndexArray,outEndIndexArray,outScopeArray,outFilePathArray,outMatchedCountArray,inAbort);
}

//#853
/**
キーワードリストから各種キーワード検索スレッド用統合キーワード検索処理(Import識別子)
@note ハッシュを使わないので低速なため、スレッド使用が必須だが、いろいろな検索方法ができる。thread-safe
*/
ABool	AModePrefDB::SPI_SearchKeywordKeywordList( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
												  const AIndex inCurrentStateIndex,
												  AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
												  ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
												  ATextArray& outCommentTextArray,
												  ATextArray& outCompletionTextArray, ATextArray& outURLArray,
												  AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
												  AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
												  AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
												  AArray<AItemCount>& outMatchedCountArray,
												  const ABool& inAbort  ) const
{
	//排他制御
	AStMutexLocker	locker(mKeywordListMutex);
	//結果の開始位置記憶
	AIndex	startIndex = outKeywordArray.GetItemCount();
	//キーワード検索
	AHashArray<AUniqID>	limitLineUniqIDArray;
	AArray<AUniqID>	lineUniqIDArray;
	ABool	result = mKeywordList.SearchKeyword(inWord,inParentWord,inOption,
												mModeID,inCurrentStateIndex,
												limitLineUniqIDArray,
												outKeywordArray,outParentKeywordArray,
												outTypeTextArray,outInfoTextArray,outCommentTextArray,
												outCompletionTextArray,outURLArray,
												outCategoryIndexArray,outColorSlotIndexArray,
												outStartIndexArray,outEndIndexArray,outMatchedCountArray,lineUniqIDArray,inAbort);
	//キーワードリストからの検索の場合、outScopeArray, outFilePathArrayには格納されないので、他の結果と同じ数だけ、追加する。
	for( AIndex i = startIndex; i < outKeywordArray.GetItemCount(); i++ )
	{
		outScopeArray.Add(kScopeType_ModeKeyword);
		outFilePathArray.Add(GetEmptyText());
	}
	return result;
}

/**
キーワードのIdInfoを取得
*/
void	AModePrefDB::FindKeywordIdInfo( const AText& inKeyword, 
			ATextArray& outExplanationArray, AArray<AIndex>& outCategoryIndexArray, ATextArray& outParentKeywordArray ) const
{
	/*
	//キーワードが一致するものをすべて検索
	AArray<AIndex>	indexArray;
	mKeywordExplanationTextArray_Keyword.FindAll(inKeyword,0,inKeyword.GetItemCount(),indexArray);
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		AIndex	categoryIndex = mCategoryArray_Name.Find(mKeywordExplanationTextArray_CategoryName.GetTextConst(indexArray.Get(i)));
		if( categoryIndex != kIndex_Invalid )
		{
			outExplanationArray.Add(mKeywordExplanationTextArray_Explanation.GetTextConst(indexArray.Get(i)));
			outCategoryIndexArray.Add(categoryIndex);
		}
	}
	*/
	//排他制御
	AStMutexLocker	locker(mKeywordListMutex);
	
	//キーワードに対応する識別子を全て取得
	AArray<AUniqID>	UniqIDArray;
	mKeywordList.GetIdentifierListByKeyword(inKeyword,UniqIDArray);
	//各識別子毎に処理
	for( AIndex i = 0; i < UniqIDArray.GetItemCount(); i++ )
	{
		//識別子UniqID取得
		AUniqID	identifierID = UniqIDArray.Get(i);
		//各情報取得
		AText	infotext;
		mKeywordList.GetInfoText(identifierID,infotext);
		AText	parent;
		mKeywordList.GetParentKeywordText(identifierID,parent);
		AIndex	categoryIndex = mKeywordList.GetCategoryIndex(identifierID);
		//
		outExplanationArray.Add(infotext);
		outCategoryIndexArray.Add(categoryIndex);
		outParentKeywordArray.Add(parent);
	}
}

#pragma mark ===========================

#pragma mark ---

//カテゴリー色取得（モード設定優先、未設定なら文法定義ファイルのデフォルトカラー）
void	AModePrefDB::GetSyntaxDefinitionCategoryColorByName( const AText& inName, 
		AColor& outColor, AColor& outImportColor, AColor& outLocalColor, ATextStyle& outMenuTextStyle, 
		ATextStyle& outTextStyle, const ABool inDarkMode ) const //#844 #1316
{
	/*#844 従来のSDFカテゴリ色データは使用しない。カラースロットを使用。
	outBold = false;//R0195
	outItalic = false;//R0195
	outUnderline = false;//R0195
	AIndex	index = Find_TextArray(kSyntaxDefinitionCategory_Name,inName,false);
	if( index != kIndex_Invalid )
	{
		GetData_ColorArray(kSyntaxDefinitionCategory_Color,index,outColor);
		GetData_ColorArray(kSyntaxDefinitionCategory_ImportColor,index,outImportColor);
		GetData_ColorArray(kSyntaxDefinitionCategory_LocalColor,index,outLocalColor);
		outBold 	= GetData_BoolArray(kSyntaxDefinitionCategory_Bold,index);//R0195
		outItalic 	= GetData_BoolArray(kSyntaxDefinitionCategory_Italic,index);//R0195
		outUnderline= GetData_BoolArray(kSyntaxDefinitionCategory_Underline,index);//R0195
	}
	else
	*/
	//{
	AIndex	index = mSyntaxDefinition.GetCategoryNameArray().Find(inName);
	if( index == kIndex_Invalid )   { _ACError("",this); return; }
	/*#844
	outColor = mSyntaxDefinition.GetCategoryDefaultColorArray().Get(index);
	outImportColor = mSyntaxDefinition.GetCategoryDefaultImportColorArray().Get(index);
	outLocalColor = mSyntaxDefinition.GetCategoryDefaultLocalColorArray().Get(index);
	*/
	//カラースロットの色を取得
	AIndex	colorSlot = mSyntaxDefinition.GetCategoryColorSlot().Get(index);
	GetColorSlotData(colorSlot,outColor,outTextStyle,inDarkMode);//#1316
	if( GetModeData_Bool(AModePrefDB::kDarkenImportLightenLocal) == true )
	{
		outImportColor = AColorWrapper::ChangeHSV(outColor,0,1.0,0.85);//インポートは少し濃いめの色 #1316 0.8→0.85 暗いとダークモードで見づらくなるので効果を少し減らす。
		outLocalColor = AColorWrapper::ChangeHSV(outColor,0,1.0,1.3);//ローカルは少し明るめの色
	}
	else
	{
		outImportColor = outColor;
		outLocalColor = outColor;
	}
	//}
	//#844 index = mSyntaxDefinition.GetCategoryNameArray().Find(inName);
	outMenuTextStyle = mSyntaxDefinition.GetCategoryMenuTextStyleArray().Get(index);
}

//stateごとの色情報をAModePrefDB内のメンバー変数(mSyntaxDefinitionStateColorArray, mSyntaxDefinitionStateColorValidArray)に格納する
//テキスト描画時は、mSyntaxDefinitionStateColorArray, mSyntaxDefinitionStateColorValidArrayのほうを使用する。
void	AModePrefDB::UpdateSyntaxDefinitionStateColorArray()
{
	//文法定義state毎の色テーブル更新
	mSyntaxDefinitionStateColorArray.DeleteAll();
	mSyntaxDefinitionStateColorArray_Dark.DeleteAll();//#1316
	mSyntaxDefinitionStateColorValidArray.DeleteAll();
	mSyntaxDefinitionStateArray_TextStyle.DeleteAll();//#844
	for( AIndex index = 0; index < mSyntaxDefinition.GetStateCount(); index++ )
	{
		AColor	color, color_dark;
		//#844 if( mSyntaxDefinition.GetSyntaxDefinitionState(index).GetStateColor(color) == true )
		AIndex	colorSlot = mSyntaxDefinition.GetSyntaxDefinitionState(index).GetColorSlot();
		if( colorSlot != kIndex_Invalid )
		{
			//------------------カラースロット指定の有るstateの場合------------------
			//#844 GetSyntaxDefinitionStateColorByName(mSyntaxDefinition.GetStateName(index),color);
			//カラースロットの色取得
			ATextStyle	textstyle = kTextStyle_Normal;
			GetColorSlotData(colorSlot,color,textstyle,false);
			GetColorSlotData(colorSlot,color_dark,textstyle,true);//#1316
			//テーブルに追加
			mSyntaxDefinitionStateColorArray.Add(color);
			mSyntaxDefinitionStateColorArray_Dark.Add(color_dark);
			mSyntaxDefinitionStateColorValidArray.Add(true);
			mSyntaxDefinitionStateArray_TextStyle.Add(textstyle);//#844
		}
		else
		{
			//------------------カラースロット指定の無いstateの場合------------------
			//文字色をテーブルに追加
			GetModeData_Color(AModePrefDB::kLetterColor,color);
			GetModeData_Color(AModePrefDB::kLetterColor,color_dark);//#1316
			mSyntaxDefinitionStateColorArray.Add(color);
			mSyntaxDefinitionStateColorArray_Dark.Add(color_dark);//#1316
			mSyntaxDefinitionStateColorValidArray.Add(false);
			mSyntaxDefinitionStateArray_TextStyle.Add(kTextStyle_Normal);//#844
		}
	}
}

/**
カラースロットの色・スタイル取得
*/
void	AModePrefDB::GetColorSlotData( const AIndex inSlotIndex, AColor& outColor, ATextStyle& outTextStyle, const ABool inDarkMode ) const//#1316
{
	//
	ABool	isBold = false, isItalic = false, isUnderline = false;
	switch(inSlotIndex)
	{
	  case 0:
		{
			GetModeData_Color(kSyntaxColorSlot0_Color,outColor,inDarkMode);//#1316
			isBold = GetModeData_Bool(kSyntaxColorSlot0_Bold);
			isItalic = GetModeData_Bool(kSyntaxColorSlot0_Italic);
			isUnderline = GetModeData_Bool(kSyntaxColorSlot0_Underline);
			break;
		}
	  case 1:
		{
			GetModeData_Color(kSyntaxColorSlot1_Color,outColor,inDarkMode);//#1316
			isBold = GetModeData_Bool(kSyntaxColorSlot1_Bold);
			isItalic = GetModeData_Bool(kSyntaxColorSlot1_Italic);
			isUnderline = GetModeData_Bool(kSyntaxColorSlot1_Underline);
			break;
		}
	  case 2:
		{
			GetModeData_Color(kSyntaxColorSlot2_Color,outColor,inDarkMode);//#1316
			isBold = GetModeData_Bool(kSyntaxColorSlot2_Bold);
			isItalic = GetModeData_Bool(kSyntaxColorSlot2_Italic);
			isUnderline = GetModeData_Bool(kSyntaxColorSlot2_Underline);
			break;
		}
	  case 3:
		{
			GetModeData_Color(kSyntaxColorSlot3_Color,outColor,inDarkMode);//#1316
			isBold = GetModeData_Bool(kSyntaxColorSlot3_Bold);
			isItalic = GetModeData_Bool(kSyntaxColorSlot3_Italic);
			isUnderline = GetModeData_Bool(kSyntaxColorSlot3_Underline);
			break;
		}
	  case 4:
		{
			GetModeData_Color(kSyntaxColorSlot4_Color,outColor,inDarkMode);//#1316
			isBold = GetModeData_Bool(kSyntaxColorSlot4_Bold);
			isItalic = GetModeData_Bool(kSyntaxColorSlot4_Italic);
			isUnderline = GetModeData_Bool(kSyntaxColorSlot4_Underline);
			break;
		}
	  case 5:
		{
			GetModeData_Color(kSyntaxColorSlot5_Color,outColor,inDarkMode);//#1316
			isBold = GetModeData_Bool(kSyntaxColorSlot5_Bold);
			isItalic = GetModeData_Bool(kSyntaxColorSlot5_Italic);
			isUnderline = GetModeData_Bool(kSyntaxColorSlot5_Underline);
			break;
		}
	  case 6:
		{
			GetModeData_Color(kSyntaxColorSlot6_Color,outColor,inDarkMode);//#1316
			isBold = GetModeData_Bool(kSyntaxColorSlot6_Bold);
			isItalic = GetModeData_Bool(kSyntaxColorSlot6_Italic);
			isUnderline = GetModeData_Bool(kSyntaxColorSlot6_Underline);
			break;
		}
	  case 7:
		{
			GetModeData_Color(kSyntaxColorSlot7_Color,outColor,inDarkMode);//#1316
			isBold = GetModeData_Bool(kSyntaxColorSlot7_Bold);
			isItalic = GetModeData_Bool(kSyntaxColorSlot7_Italic);
			isUnderline = GetModeData_Bool(kSyntaxColorSlot7_Underline);
			break;
		}
	  case 32:
		{
			GetModeData_Color(kSyntaxColorSlotComment_Color,outColor,inDarkMode);//#1316
			isBold = GetModeData_Bool(kSyntaxColorSlotComment_Bold);
			isItalic = GetModeData_Bool(kSyntaxColorSlotComment_Italic);
			isUnderline = GetModeData_Bool(kSyntaxColorSlotComment_Underline);
			break;
		}
	  case 33:
		{
			GetModeData_Color(kSyntaxColorSlotLiteral_Color,outColor,inDarkMode);//#1316
			isBold = GetModeData_Bool(kSyntaxColorSlotLiteral_Bold);
			isItalic = GetModeData_Bool(kSyntaxColorSlotLiteral_Italic);
			isUnderline = GetModeData_Bool(kSyntaxColorSlotLiteral_Underline);
			break;
		}
	  default:
		{
			GetModeData_Color(kLetterColor,outColor,inDarkMode);//#1388 CategoryAttribute_ColorSlot未指定の場合は標準文字色にする。
			break;
		}
	}
	//テキストスタイル設定
	outTextStyle = kTextStyle_Normal;
	if( isBold == true )
	{
		outTextStyle |= kTextStyle_Bold;
	}
	if( isItalic == true )
	{
		outTextStyle |= kTextStyle_Italic;
	}
	if( isUnderline == true )
	{
		outTextStyle |= kTextStyle_Underline;
	}
}

//stateに対応する色を取得する
//（mSyntaxDefinitionStateColorArray, mSyntaxDefinitionStateColorValidArrayから取得する）
void	AModePrefDB::GetLetterColorForState( const AIndex inStateIndex, AColor& outColor, ATextStyle& outStyle, ABool& outStateColorValid ) const
{
	if( GetApp().NVI_IsDarkMode() == false )
	{
		outColor = mSyntaxDefinitionStateColorArray.Get(inStateIndex);
	}
	else
	{
		outColor = mSyntaxDefinitionStateColorArray_Dark.Get(inStateIndex);
	}
	outStateColorValid = mSyntaxDefinitionStateColorValidArray.Get(inStateIndex);
	outStyle = mSyntaxDefinitionStateArray_TextStyle.Get(inStateIndex);//#844
	//#868
	if( outStateColorValid == false )
	{
		GetModeData_Color(AModePrefDB::kLetterColor,outColor);
	}
}

void	AModePrefDB::GetOpenSelectedFileCandidate( const AText& inFilePathText, AObjectArray<AFileAcc>& outCandidate ) const
{
	AText	pathwithoutsuffix;
	pathwithoutsuffix.SetText(inFilePathText);
	for( AIndex i = pathwithoutsuffix.GetItemCount() -1; i > 0 ; i-- )
	{
		if( pathwithoutsuffix.Get(i) == kUChar_Period )
		{
			pathwithoutsuffix.Delete(i,pathwithoutsuffix.GetItemCount()-i);
			break;
		}
	}
	AFileAcc	origFile;
	origFile.Specify(inFilePathText);
	for( AIndex suffixIndex = 0; suffixIndex < GetItemCount_Array(kSuffix); suffixIndex++ )
	{
		AText	suffix;
		GetData_TextArray(kSuffix,suffixIndex,suffix);
		AText	path;
		path.SetText(pathwithoutsuffix);
		path.AddText(suffix);
		AFileAcc	file;
		file.Specify(path);
		if( file.Exist() == true && file.Compare(origFile) == false )
		{
			AIndex	index = outCandidate.AddNewObject();
			outCandidate.GetObject(index).CopyFrom(file);
		}
	}
}

/*R0210 AModePrefDB::LoadMacLegacy_Keyword()に移動
void	AModePrefDB::LoadSuffix()
{
	DeleteAll_TextArray(AModePrefDB::kSuffix);
	
	//暫定
#if IMPLEMENTATION_FOR_MACOSX
	try
	{
		AText	basetext;
		mModeFile.ReadTo(basetext);
		FSSpec	fileSpec;
		mModeFile.GetFSSpec(fileSpec);
		::FSpCreateResFile(&fileSpec,'MMKE','MODE',0);
		short rRefNum = ::FSpOpenResFile(&fileSpec,fsRdPerm);
		if( rRefNum != -1  ) 
		{
			SMacLegacyTextArrayPosition	**suffixpos;
			suffixpos = (SMacLegacyTextArrayPosition**)::Get1Resource('MMCW',1500);
			if( suffixpos != NULL )
			{
				Int32	suffixcount = GetHandleSize((Handle)suffixpos)/sizeof(SMacLegacyTextArrayPosition);
				for( Int32 number = 0; number < suffixcount; number++ ) 
				{
					AText	suffix;
					SMacLegacyTextArrayPosition	s = (*suffixpos)[number];
					AUniversalBinaryUtility::SwapBigToHost(s.position);
					AUniversalBinaryUtility::SwapBigToHost(s.length);
					basetext.GetText(s.position,s.length,suffix);
					if( suffix.GetLength() == 0 )   suffix.SetCstring("   ");
					Add_TextArray(AModePrefDB::kSuffix,suffix);
				}
			}
			::CloseResFile(rRefNum);
		}
	}
	catch(...)
	{
	}
#endif
	
	UpdateSuffixHash();
}
*/

/**
suffixハッシュテーブル更新
@note thread-safe
*/
void	AModePrefDB::UpdateSuffixHash()
{
	AStMutexLocker	locker(mSuffixHashArrayMutex);
	mSuffixHashArray.DeleteAll();
	for( AIndex i = 0; i < GetItemCount_Array(AModePrefDB::kSuffix); i++ )
	{
		AText	text;
		GetData_TextArray(AModePrefDB::kSuffix,i,text);
		mSuffixHashArray.Add(text);
		text.ChangeCaseLower();
		mSuffixHashArray.Add(text);
		text.ChangeCaseUpper();
		mSuffixHashArray.Add(text);
	}
	//グローバルのsuffixハッシュを更新
	GetApp().SPI_UpdateModeSuffixArray(mModeID);
}

/**
このモードのsuffixを持つファイル名かどうかを判定
@note thread-safe
*/
ABool	AModePrefDB::IsThisModeSuffix( const AText& inFilename ) const
{
	//#868
	//このモードがdisableなら常にfalse
	if( GetData_Bool(kEnableMode) == false )   return false;
	//このモードのsuffixのいずれかと一致するかどうかを判定
	AIndex	suffixspos = inFilename.GetSuffixStartPos();
	if( suffixspos == inFilename.GetItemCount() )   return false;
	AStMutexLocker	locker(mSuffixHashArrayMutex);
	if( mSuffixHashArray.Find(inFilename,suffixspos,inFilename.GetItemCount()-suffixspos) != kIndex_Invalid )   return true;
	else return false;
}

//R0210
ABool	AModePrefDB::IsThisModeSelectFilePath( const AText& inFilePath ) const
{
	for( AIndex i = 0; i < GetItemCount_Array(kModeSelectFilePath); i++ )
	{
		const AText&	t = GetData_ConstTextArrayRef(kModeSelectFilePath).GetTextConst(i);
		if( t.CompareMin(inFilePath) == true )
		{
			return true;
		}
	}
	return false;
}

/**
モード名取得
優先順位：現在の言語での名称→ファイル名
*/
void	AModePrefDB::GetModeDisplayName( AText& outName ) const
{
	//#305 mModeFile.GetFilename(outName);
	GetModeMultiLanguageName(GetApp().NVI_GetLanguage(),outName);//#305
	if( outName.GetItemCount() == 0 )
	{
		mModeFolder.GetFilename(outName);
	}
}

//#427
/**
モードファイル名変更
*/
ABool	AModePrefDB::ChangeModeFileName( const AText& inName )
{
	//空文字列への変更は不可
	if( inName.GetItemCount() == 0 )   return false;
	//同じファイル名の存在チェック
	AFileAcc	newModeFolderForCheck;
	newModeFolderForCheck.SpecifySister(mModeFolder,inName);
	if( newModeFolderForCheck.Exist() == true )
	{
		return false;
	}
	//フォルダ名変更（Rename()でAFileAccのパスも更新される）
	mModeFolder.Rename(inName);
	//モードファイル名変更時処理
	GetApp().SPI_ModeNameUpdated(mModeID);
	return true;
}

void	AModePrefDB::SetIndentRegExp( AObjectArray<ARegExp>& outRegExpArray ) const
{
	outRegExpArray.DeleteAll();
	for( AIndex i = 0; i < GetItemCount_TextArray(kIndent_RegExp); i++ )
	{
		AText	text;
		GetData_TextArray(kIndent_RegExp,i,text);
		outRegExpArray.GetObject(outRegExpArray.AddNewObject()).SetRE(text);
	}
}

#pragma mark ===========================

#pragma mark ---見出し設定

/**
見出し検索用正規表現を取得
*/
void	AModePrefDB::SetJumpSetupRegExp( AObjectArray<ARegExp>& outRegExpArray ) const
{
	//結果全削除
	outRegExpArray.DeleteAll();
	//各見出し設定ごとのループ
	AItemCount	itemCount = mJumpSetup_RegExpText.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//正規表現文字列取得
		AText	regexpText;
		//------------------簡単設定の場合------------------
		if( mJumpSetup_SimplerSetting.Get(i) == true )
		{
			//開始文字列取得
			AText	text;
			mJumpSetup_LineStartText.Get(i,text);
			if( text.GetItemCount() > 0 )
			{
				AItemCount	len = text.GetItemCount();
				AText	exceptChars;
				//各見出し設定毎のループ
				for( AIndex j = 0; j < itemCount; j++ )
				{
					if( mJumpSetup_SimplerSetting.Get(j) == true )
					{
						//簡単設定を使う見出し設定の開始文字列を取得、そちらのほうが文字列長が長く、こちらの文字列の範囲で同じ文字列なら、
						//差分のある位置の文字をexceptCharsに追加する。
						AText	t;
						mJumpSetup_LineStartText.Get(j,t);
						if( t.CompareMin(text) == true && t.GetItemCount() > len )
						{
							AText	ch;
							t.Get1UTF8Char(len,ch);
							//差分のある位置の文字をexceptCharsに追加
							exceptChars.AddText(ch);
						}
					}
				}
				//正規表現用にエスケープ
				text.ConvertToRegExpEscapedText();
				exceptChars.ConvertToRegExpEscapedText();
				//正規表現テキスト生成
				regexpText.SetCstring("[ \t]*(");//最初のタブスペースの有無は無視して一致する。メニューに登録するのは$1の部分。
				regexpText.AddText(text);
				if( exceptChars.GetItemCount() > 0 )
				{
					//他の簡単見出し設定に一致しないように、次の文字として対象外とする文字を指定
					regexpText.AddCstring("[^");
					regexpText.AddText(exceptChars);
					regexpText.AddCstring("]");
				}
				//
				regexpText.AddCstring(".+)");
			}
		}
		//------------------正規表現設定の場合------------------
		else
		{
			//正規表現テキスト取得
			mJumpSetup_RegExpText.Get(i,regexpText);
		}
		//正規表現オブジェクト取得、正規表現設定
		AIndex	regExpIndex = outRegExpArray.AddNewObject();
		if( regexpText.GetItemCount() > 0 && mJumpSetup_Enable.Get(i) == true )
		{
			outRegExpArray.GetObject(regExpIndex).SetRE(regexpText);
		}
	}
}

/**
見出し設定テーブルを更新
（モード設定の見出し設定変更時に直接文法認識から参照しているデータを変更しないようにするため、バッファをもうけている）
*/
void	AModePrefDB::UpdateJumpSetup()
{
	//排他制御
	AStMutexLocker	locker(mJumpSetupMutex);
	
	//各データコピー
	mJumpSetup_Enable.SetFromObject(GetData_BoolArrayRef(kJumpSetup_Enable));
	mJumpSetup_RegExpText.SetFromTextArray(GetData_TextArrayRef(kJumpSetup_RegExp));
	mJumpSetup_LineStartText.SetFromTextArray(GetData_TextArrayRef(kJumpSetup_LineStartText));
	mJumpSetup_DisplayMultiply.SetFromObject(GetData_ConstNumberArrayRef(kJumpSetup_DisplayMultiply));
	mJumpSetup_ColorizeLine.SetFromObject(GetData_ConstBoolArrayRef(kJumpSetup_ColorizeLine));
	mJumpSetup_LineColor.SetFromObject(GetData_ConstColorArrayRef(kJumpSetup_LineColor));
	//簡単設定か正規表現設定かによって、それぞれのメニュー登録設定から、メニュー登録設定を設定
	mJumpSetup_RegisterToMenu.DeleteAll();
	mJumpSetup_MenuText.DeleteAll();
	AItemCount	itemCount = GetItemCount_BoolArray(kJumpSetup_RegisterToMenu);
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//簡単設定の場合
		if( GetData_BoolArray(kJumpSetup_SimplerSetting,i) == true )
		{
			mJumpSetup_RegisterToMenu.Add(GetData_BoolArray(kJumpSetup_RegisterToMenu2,i));
			AText	menutext("$1");
			mJumpSetup_MenuText.Add(menutext);
		}
		//正規表現設定の場合
		else
		{
			mJumpSetup_RegisterToMenu.Add(GetData_BoolArray(kJumpSetup_RegisterToMenu,i));
			AText	menutext;
			GetData_TextArray(kJumpSetup_MenuText,i,menutext);
			mJumpSetup_MenuText.Add(menutext);
		}
	}
	//各データコピー
	mJumpSetup_RegisterAsKeyword.SetFromObject(GetData_ConstBoolArrayRef(kJumpSetup_RegisterAsKeyword));
	mJumpSetup_KeywordGroup.SetFromObject(GetData_ConstNumberArrayRef(kJumpSetup_KeywordGroup));
	mJumpSetup_ColorizeRegExpGroup.SetFromObject(GetData_ConstBoolArrayRef(kJumpSetup_ColorizeRegExpGroup));
	mJumpSetup_ColorizeRegExpGroup_Number.SetFromObject(GetData_ConstNumberArrayRef(kJumpSetup_ColorizeRegExpGroup_Number));
	mJumpSetup_ColorizeRegExpGroup_Color.SetFromObject(GetData_ConstColorArrayRef(kJumpSetup_ColorizeRegExpGroup_Color));
	mJumpSetup_OutlineLevel.SetFromObject(GetData_ConstNumberArrayRef(kJumpSetup_OutlineLevel));
	mJumpSetup_SimplerSetting.SetFromObject(GetData_ConstBoolArrayRef(kJumpSetup_SimplerSetting));
}

/**
見出しテーブル全削除
*/
void	AModePrefDB::DeleteJumpSetupAll()
{
	//排他制御
	AStMutexLocker	locker(mJumpSetupMutex);
	//全削除
	mJumpSetup_Enable.DeleteAll();
	mJumpSetup_RegExpText.DeleteAll();
	mJumpSetup_LineStartText.DeleteAll();
	mJumpSetup_DisplayMultiply.DeleteAll();
	mJumpSetup_ColorizeLine.DeleteAll();
	mJumpSetup_LineColor.DeleteAll();
	mJumpSetup_RegisterToMenu.DeleteAll();
	mJumpSetup_MenuText.DeleteAll();
	mJumpSetup_RegisterAsKeyword.DeleteAll();
	mJumpSetup_KeywordGroup.DeleteAll();
	mJumpSetup_ColorizeRegExpGroup.DeleteAll();
	mJumpSetup_ColorizeRegExpGroup_Number.DeleteAll();
	mJumpSetup_ColorizeRegExpGroup_Color.DeleteAll();
	mJumpSetup_OutlineLevel.DeleteAll();
	mJumpSetup_SimplerSetting.DeleteAll();
	mJumpSetupCategoryStartIndex = 0;
	
}

/**
見出し設定パラメータ取得
*/
ANumber	AModePrefDB::GetJumpSetup_DisplayMultiply( const AIndex inIndex ) const
{
	return mJumpSetup_DisplayMultiply.Get(inIndex);
}

/**
見出し設定パラメータ取得
*/
ABool	AModePrefDB::GetJumpSetup_ColorizeLine( const AIndex inIndex ) const
{
	return mJumpSetup_ColorizeLine.Get(inIndex);
}

/**
見出し設定パラメータ取得
*/
AColor	AModePrefDB::GetJumpSetup_LineColor( const AIndex inIndex ) const
{
	return mJumpSetup_LineColor.Get(inIndex);
}

/**
見出し設定パラメータ取得
*/
ABool	AModePrefDB::GetJumpSetup_RegisterToMenu( const AIndex inIndex ) const
{
	return mJumpSetup_RegisterToMenu.Get(inIndex);
}

/**
見出し設定パラメータ取得
*/
void	AModePrefDB::GetJumpSetup_MenuText( const AIndex inIndex, AText& outMenuText ) const
{
	mJumpSetup_MenuText.Get(inIndex,outMenuText);
}

/**
見出し設定パラメータ取得
*/
ABool	AModePrefDB::GetJumpSetup_RegisterAsKeyword( const AIndex inIndex ) const
{
	return mJumpSetup_RegisterAsKeyword.Get(inIndex);
}

/**
見出し設定パラメータ取得
*/
ANumber	AModePrefDB::GetJumpSetup_KeywordGroup( const AIndex inIndex ) const
{
	return mJumpSetup_KeywordGroup.Get(inIndex);
}

/**
見出し設定パラメータ取得
*/
ABool	AModePrefDB::GetJumpSetup_ColorizeRegExpGroup( const AIndex inIndex ) const
{
	return mJumpSetup_ColorizeRegExpGroup.Get(inIndex);
}

/**
見出し設定パラメータ取得
*/
ANumber	AModePrefDB::GetJumpSetup_ColorizeRegExpGroup_Number( const AIndex inIndex ) const
{
	return mJumpSetup_ColorizeRegExpGroup_Number.Get(inIndex);
}

/**
見出し設定パラメータ取得
*/
AColor	AModePrefDB::GetJumpSetup_ColorizeRegExpGroup_Color( const AIndex inIndex ) const
{
	return mJumpSetup_ColorizeRegExpGroup_Color.Get(inIndex);
}

/**
見出し設定パラメータ取得
*/
AIndex	AModePrefDB::GetJumpSetupCategoryIndex( const AIndex inIndex ) const 
{
	return mJumpSetupCategoryStartIndex+inIndex;
}

/**
見出し設定パラメータ取得
*/
ANumber	AModePrefDB::GetJumpSetupOutlineLevel( const AIndex inIndex ) const
{
	return mJumpSetup_OutlineLevel.Get(inIndex);
}

#pragma mark ===========================

#pragma mark ---キーバインド


/**
キーバインド取得
*/
void	AModePrefDB::GetData_KeyBindAction( const AKeyBindKey inKey, const AModifierKeys inModiferKeys, 
			AKeyBindAction& outAction, AText& outText,
			ABool& outDefault, ABool& outBindAvailable ) const
{
	outAction = keyAction_NOP;
	outDefault = false;
	outBindAvailable = false;
	
	if( GetData_Bool(AModePrefDB::kKeyBindDefaultIsNormalMode) == true && mModeID != kStandardModeIndex )
	{
        //==================標準モードからの差分を設定する場合==================
        //標準モードのキーバインドデータを取得
        GetApp().SPI_GetModePrefDB(kStandardModeIndex).GetData_KeyBindAction(inKey,inModiferKeys,
																			 outAction,outText,outDefault,outBindAvailable);
        //キーバインド不可の場合はここでリターン（このモードで設定しているキーバインド設定は取得しない）
        if( outBindAvailable == false )
		{
			return;
		}
	}
    else
	{
		//==================OSデフォルトキーバインドからの差分を設定する場合==================
        //（OSデフォルトキーバインドからの差分の場合でも、キーバインド可否判定のために、下記の処理を行う。
        //デフォルト動作も取得しているが、コール元ではoutDefaultがtrueの場合はoutActionは使用していない。）
        //==================miのデフォルトキーバインドからの差分を設定する場合==================
		//デフォルトキーバインド取得＋キーバインド可否判定
		if( GetApp().GetAppPref().GetData_DefaultKeyBindAction(inKey,inModiferKeys,outAction,outText) == false )
		{
			//キーバインド不可の場合はここでリターン（このモードで設定しているキーバインド設定は取得しない）
			outDefault = true;
			outBindAvailable = false;
			return;
		}
	}
    
    //==================このモードで設定しているキーバインド設定を検索し、有ればそれを返す==================
	//キーバインドデータから一致するものを検索する。
	for( AIndex index = 0; index < GetItemCount_NumberArray(kKeyBind_Key); index++ )
	{
		if( GetData_NumberArray(kKeyBind_Key,index) == inKey )
		{
			if( GetData_BoolArray(kKeyBind_ControlKey,index) 		== AKeyWrapper::IsControlKeyPressed(inModiferKeys) &&
					GetData_BoolArray(kKeyBind_OptionKey,index) 	== AKeyWrapper::IsOptionKeyPressed(inModiferKeys) &&
					GetData_BoolArray(kKeyBind_CommandKey,index) 	== AKeyWrapper::IsCommandKeyPressed(inModiferKeys) &&
					GetData_BoolArray(kKeyBind_ShiftKey,index) 		== AKeyWrapper::IsShiftKeyPressed(inModiferKeys) )
			{
				outAction = static_cast<AKeyBindAction>(GetData_NumberArray(kKeyBind_Action,index));
				GetData_TextArray(kKeyBind_Text,index,outText);
				outDefault = false;
				outBindAvailable = true;
				return;
			}
		}
	}
	outDefault = true;
	outBindAvailable = true;
}

/**
キーバインド取得（デフォルトかどうか・キーバインド変更可否の返り値なしバージョン）
*/
/*#481
void	AModePrefDB::GetData_KeyBindAction( const AKeyBindKey inKey, const AModifierKeys inModiferKeys, 
			AKeyBindAction& outAction, AText& outText )
{
	ABool	b1 = false;
	ABool	b2 = false;
	GetData_KeyBindAction(inKey,inModiferKeys,outAction,outText,b1,b2);
}
*/

/**
キーバインド設定
*/
ABool	AModePrefDB::SetData_KeybindAction( const AKeyBindKey& inKey, const AModifierKeys inModiferKeys, const AKeyBindAction& inAction, const AText& inText )
{
	AKeyBindAction	defaultAction;
	AText	defaultText;
	//デフォルトキーバインド取得＋キーバインド可否判定
	if( GetApp().GetAppPref().GetData_DefaultKeyBindAction(inKey,inModiferKeys,defaultAction,defaultText) == false )
	{
		//キーバインド不可
		return false;
	}
	else
	{
		//キーバインドデータから一致するものを検索する。
		for( AIndex index = 0; index < GetItemCount_NumberArray(kKeyBind_Key); index++ )
		{
			if( GetData_NumberArray(kKeyBind_Key,index) == inKey )
			{
				if( GetData_BoolArray(kKeyBind_ControlKey,index) 		== AKeyWrapper::IsControlKeyPressed(inModiferKeys) &&
						GetData_BoolArray(kKeyBind_OptionKey,index) 	== AKeyWrapper::IsOptionKeyPressed(inModiferKeys) &&
						GetData_BoolArray(kKeyBind_CommandKey,index) 	== AKeyWrapper::IsCommandKeyPressed(inModiferKeys) &&
						GetData_BoolArray(kKeyBind_ShiftKey,index) 		== AKeyWrapper::IsShiftKeyPressed(inModiferKeys) )
				{
					//変更したかどうかのフラグ（返り値）
					ABool	changed = false;
					//現在のキーバインドを取得
					AKeyBindAction	oldAction = GetData_NumberArray(kKeyBind_Action,index);
					AText	oldActionText;
					GetData_TextArray(kKeyBind_Text,index,oldActionText);
					//#481 if( defaultAction != inAction || defaultText.Compare(inText) == false )//win
					//変更があれば設定
					if( oldAction != inAction || oldActionText.Compare(inText) == false )
					{
						SetData_NumberArray(kKeyBind_Action,index,inAction);
						SetData_TextArray(kKeyBind_Text,index,inText);
						changed = true;
					}
					/*#481 デフォルトの値に関わらず、常にデータを設定するように変更。ベースとなるキーバインドが選択可能になったため。
					else
					{
						//デフォルトキーバインドと同じになってしまった場合、データ削除
						DeleteRow_Table(kKeyBind_Key,index);
					}
                    */
					return changed;
				}
			}
		}
		//現存データに一致するものが無い場合、
		//デフォルトキーバインドと異なる場合のみ、データ追加
		//#481 デフォルトの値に関わらず、常にデータを設定するように変更。ベースとなるキーバインドが選択可能になったため。
		//#481 if( defaultAction != inAction || defaultText.Compare(inText) == false )//win
		{
			Add_NumberArray(kKeyBind_Key,inKey);
			Add_BoolArray(kKeyBind_ControlKey,	AKeyWrapper::IsControlKeyPressed(inModiferKeys));
			Add_BoolArray(kKeyBind_OptionKey,	AKeyWrapper::IsOptionKeyPressed(inModiferKeys));
			Add_BoolArray(kKeyBind_CommandKey,	AKeyWrapper::IsCommandKeyPressed(inModiferKeys));
			Add_BoolArray(kKeyBind_ShiftKey,	AKeyWrapper::IsShiftKeyPressed(inModiferKeys));
			Add_NumberArray(kKeyBind_Action,inAction);
			Add_TextArray(kKeyBind_Text,inText);
		}
		return true;
	}
}

//#481
/**
キーバインド削除
*/
ABool	AModePrefDB::Remove_KeybindAction( const AKeyBindKey& inKey, const AModifierKeys inModiferKeys )
{
	ABool	deleted = false;
	AKeyBindAction	defaultAction;
	AText	defaultText;
	//キーバインドデータから一致するものを検索する。
	for( AIndex index = 0; index < GetItemCount_NumberArray(kKeyBind_Key); index++ )
	{
		if( GetData_NumberArray(kKeyBind_Key,index) == inKey )
		{
			if( GetData_BoolArray(kKeyBind_ControlKey,index) 		== AKeyWrapper::IsControlKeyPressed(inModiferKeys) &&
					GetData_BoolArray(kKeyBind_OptionKey,index) 	== AKeyWrapper::IsOptionKeyPressed(inModiferKeys) &&
					GetData_BoolArray(kKeyBind_CommandKey,index) 	== AKeyWrapper::IsCommandKeyPressed(inModiferKeys) &&
					GetData_BoolArray(kKeyBind_ShiftKey,index) 		== AKeyWrapper::IsShiftKeyPressed(inModiferKeys) )
			{
				//データ削除
				DeleteRow_Table(kKeyBind_Key,index);
				deleted = true;
			}
		}
	}
	return deleted;
}

/**
キーバインド動作から、その動作をするキーを検索
@note 動作遅い
*/
ABool	AModePrefDB::FindKeyFromKeybindAction( const AKeyBindAction inAction, AKeyBindKey& outKey, AModifierKeys& outModifierKeys ) const
{
	for( AModifierKeys modifierKeys = 0; modifierKeys < 16; modifierKeys++ )
	{
		for( AIndex key = 0; key < kKeyBindKeyItemCount; key++ )
		{
			AKeyBindAction	action = keyAction_NOP;
			ABool	isDefault = false, isBindAvailable = false;
			AText	actionText;
			GetData_KeyBindAction((AKeyBindKey)key,modifierKeys,action,actionText,isDefault,isBindAvailable);
			if( inAction == action )
			{
				outKey = (AKeyBindKey)key;
				outModifierKeys = modifierKeys;
				return true;
			}
		}
	}
	return false;
}

/**
キーバインド動作から、その動作をするキーの表示文字列を取得
*/
ABool	AModePrefDB::GetKeyTextFromKeybindAction( const AKeyBindAction inAction, AText& outKeyText ) const
{
	outKeyText.DeleteAll();
	//動作からキーを検索
	AKeyBindKey key = kKeyBindKey_A;
	AModifierKeys	modifierKeys;
	if( FindKeyFromKeybindAction(inAction,key,modifierKeys) == true )
	{
		//modifier表示文字列を設定
		outKeyText.AddModifiersKeyText(modifierKeys);
		//modifier表示文字列があれば"+"を追加 #1379
		if( outKeyText.GetItemCount() > 0 )
		{
			outKeyText.AddCstring("+");
		}
		//キーの表示文字列を追加
		AText	t;
		t.SetLocalizedText("Keybind_key",key);
		outKeyText.AddText(t);
		return true;
	}
	return false;
}

//#382
/**
デフォルトと同じキーバインドデータを削除する
*/
/*#481
void	AModePrefDB::RemoveKeyBindsSameAsDefault()
{
	for( AIndex index = 0; index < GetItemCount_NumberArray(kKeyBind_Key); )
	{
		//キーバインド設定取得
		AKeyBindKey	key = (AKeyBindKey)GetData_NumberArray(kKeyBind_Key,index);
		AModifierKeys	modifiers = AKeyWrapper::MakeModifierKeys(
				GetData_BoolArray(kKeyBind_ControlKey,index),
				GetData_BoolArray(kKeyBind_OptionKey,index),
				GetData_BoolArray(kKeyBind_CommandKey,index),
						GetData_BoolArray(kKeyBind_ShiftKey,index));
		//対応するデフォルトキーバインドを取得
		AKeyBindAction	defaultAction;
		AText	defaultText;
		GetApp().GetAppPref().GetData_DefaultKeyBindAction(key,modifiers,defaultAction,defaultText);
		if( defaultAction == GetData_NumberArray(kKeyBind_Action,index) &&
					GetData_TextArrayRef(kKeyBind_Text).GetTextConst(index).Compare(defaultText) == true )
		{
			//同じなら削除
			DeleteRow_Table(kKeyBind_Key,index);
		}
		else
		{
			//違うなら次へ
			index++;
		}
	}
}
*/

//R0000
void	AModePrefDB::ImportRegExpIndent( const AText& inText )
{
	//全削除
	while( GetItemCount_Array(kIndent_RegExp) > 0 )
	{
		DeleteRow_Table(kIndent_RegExp,0);
	}
	
	//テキストデータ解析
	for( AIndex pos = 0; pos < inText.GetItemCount(); )
	{
		AText	text;
		inText.GetLine(pos,text);
		Add_TextArray(kIndent_RegExp,text);
		//
		inText.GetLine(pos,text);
		ANumber	num = 0;
		text.ParseIntegerNumber(num,true);
		Add_NumberArray(kIndent_OffsetCurrentLine,num);
		//
		inText.GetLine(pos,text);
		num = 0;
		text.ParseIntegerNumber(num,true);
		Add_NumberArray(kIndent_OffsetNextLine,num);
	}
	//#683 各ドキュメントのTextInfo内のRegExpオブジェクトを更新する
	GetApp().SPI_UpdateRegExpForAllDocuments(mModeID);
}

//R0000
void	AModePrefDB::ExportRegExpIndent( AText& outText ) const
{
	outText.DeleteAll();
	//エクスポートのテキストデータ作成
	for( AIndex index = 0; index < GetItemCount_Array(kIndent_RegExp); index++ )
	{
		AText	regexp;
		GetData_TextArray(kIndent_RegExp,index,regexp);
		ANumber	current = GetData_NumberArray(kIndent_OffsetCurrentLine,index);
		ANumber	next = GetData_NumberArray(kIndent_OffsetNextLine,index);
		//
		outText.AddText(regexp);
		outText.Add(kUChar_LineEnd);
		outText.AddFormattedCstring("%d\r",current);
		outText.AddFormattedCstring("%d\r",next);
	}
}

//R0000
void	AModePrefDB::ImportCorrespond( const AText& inText )
{
	//全削除
	while( GetItemCount_Array(kCorrespond_StartText) > 0 )
	{
		DeleteRow_Table(kCorrespond_StartText,0);
	}
	
	//テキストデータ解析
	for( AIndex pos = 0; pos < inText.GetItemCount(); )
	{
		AText	start, end;
		inText.GetLine(pos,start);
		inText.GetLine(pos,end);
		//データ追加
		Add_TextArray(kCorrespond_StartText,start);
		Add_TextArray(kCorrespond_EndText,end);
	}
}

//R0000
void	AModePrefDB::ExportCorrespond( AText& outText ) const
{
	outText.DeleteAll();
	//エクスポートのテキストデータ作成
	for( AIndex index = 0; index < GetItemCount_Array(kCorrespond_StartText); index++ )
	{
		AText	start, end;
		GetData_TextArray(kCorrespond_StartText,index,start);
		GetData_TextArray(kCorrespond_EndText,index,end);
		//
		outText.AddText(start);
		outText.Add(kUChar_LineEnd);
		outText.AddText(end);
		outText.Add(kUChar_LineEnd);
	}
}

//R0000
//Drag&Drop挿入文字列のインポート
void	AModePrefDB::ImportDragDropFromText( const AText& inText )
{
	//全削除
	while( GetItemCount_Array(kDragDrop_Suffix) > 0 )
	{
		DeleteRow_Table(kDragDrop_Suffix,0);
	}
	
	//テキストデータ解析
	for( AIndex pos = 0; pos < inText.GetItemCount(); )
	{
		AText	suffix;
		AText	inserttext;
		for( ; pos < inText.GetItemCount(); )
		{
			AText	line;
			inText.GetLine(pos,line);
			if( line.GetItemCount() < 4 )   continue;//3文字(***のみ)もダメ
			if( (line.Get(0) == '*' && line.Get(1) == '*' && line.Get(2) == '*') == false )   continue;
			line.GetText(3,line.GetItemCount()-3,suffix);
			//***xxxの次の行の最初=pos
			AIndex	textspos = pos;
			for( ; pos < inText.GetItemCount(); pos++ )
			{
				if( pos+3 < inText.GetItemCount() )
				{
					//行頭に3つだけ*がある場合、そこで1レコード終了
					AUChar	ch = 0;
					if( pos+4 < inText.GetItemCount() )
					{
						ch = inText.Get(pos+4);
					}
					if( inText.Get(pos) == kUChar_LineEnd && 
								inText.Get(pos+1) == '*' && inText.Get(pos+2) == '*' && inText.Get(pos+3) == '*' && ch != '*' )
					{
						pos++;
						break;
					}
				}
			}
			inText.GetText(textspos,pos-1-textspos,inserttext);//最後の改行コードは含まない
			//行頭の****は***に置き換え
			inserttext.ReplaceCstring("\r****","\r***");
			//データ追加
			Add_TextArray(kDragDrop_Suffix,suffix);
			Add_TextArray(kDragDrop_Text,inserttext);
		}
	}
}

//R0000
//Drag&Drop挿入文字列のエクスポート
void	AModePrefDB::ExportDragDropToText( AText& outText ) const
{
	outText.DeleteAll();
	//エクスポートのテキストデータ作成
	for( AIndex index = 0; index < GetItemCount_Array(kDragDrop_Suffix); index++ )
	{
		//***がレコード開始の印
		outText.AddCstring("***");
		//データ取得
		AText	suffix, inserttext;
		GetData_TextArray(kDragDrop_Suffix,index,suffix);
		GetData_TextArray(kDragDrop_Text,index,inserttext);
		//行頭の***を****に変換する。インポート時に戻す。
		inserttext.ReplaceCstring("\r***","\r****");
		//
		outText.AddText(suffix);
		outText.Add(kUChar_LineEnd);
		outText.AddText(inserttext);
		outText.Add(kUChar_LineEnd);//Drag&Dropのほうは最後に改行コード有り
	}
	outText.AddCstring("***");
}

void	AModePrefDB::ImportKeyBindFromText( const AText& inText, const ABool inAdditional )//B0000
{
	if( inAdditional == false )
	{
		//B0000 インポート時は旧データを全削除する。こうしないと、キーにデフォルト以外の動作を割り当てていたときに、
		//そのキーがデフォルトであるデータをインポートをしても、そのキーがデフォルトにならない
		while( GetItemCount_NumberArray(kKeyBind_Key) > 0 )
		{
			DeleteRow_Table(kKeyBind_Key,0);
		}
	}
	
	for( AIndex pos = 0; pos < inText.GetItemCount(); )
	{
		AKeyBindKey	key = kKeyBindKey_Invalid;
		AModifierKeys	modifiers = 0;
		AKeyBindAction	action = keyAction_NOP;
		AText	actionText;
		for( ; pos < inText.GetItemCount(); )
		{
			AText	token;
			inText.GetToken(pos,token);
			token.ChangeCaseLower();
			if( token.GetItemCount() == 0 )   break;
			AUChar	ch = token.Get(0);
			if( ch == '*' || ch == '+' )   continue;
			else if( ch == kUChar_LineEnd )
			{
				key = kKeyBindKey_Invalid;
				break;
			}
			/*#688#if IMPLEMENTATION_FOR_MACOSX
			else if( token.Compare("command") == true )
			{
				modifiers |= cmdKey;
			}
			else if( token.Compare("control") == true )
			{
				modifiers |= controlKey;
			}
			else if( token.Compare("option") == true )
			{
				modifiers |= optionKey;
			}
			else if( token.Compare("shift") == true )
			{
			modifiers |= shiftKey;
			}
			#endif
			#if IMPLEMENTATION_FOR_WINDOWS
			*/
			else if( token.Compare("command") == true )
			{
				modifiers |= kModifierKeysMask_Command;
			}
			else if( token.Compare("control") == true )
			{
				modifiers |= kModifierKeysMask_Control;
			}
			else if( token.Compare("option") == true )
			{
				modifiers |= kModifierKeysMask_Option;
			}
			else if( token.Compare("shift") == true )
			{
				modifiers |= kModifierKeysMask_Shift;
			}
			//#688 #endif
			else if( ch == ':' )
			{
				actionText.DeleteAll();
				for( ; pos < inText.GetItemCount(); )
				{
					AUChar	ch1 = inText.Get(pos);
					AUChar	ch2 = 0;
					if( pos+1 < inText.GetItemCount() )
					{
						ch2 = inText.Get(pos+1);
					}
					AUChar	ch3 = 0;
					if( pos+2 < inText.GetItemCount() )
					{
						ch3 = inText.Get(pos+2);
					}
					AUChar	ch4 = 0;
					if( pos+3 < inText.GetItemCount() )
					{
						ch4 = inText.Get(pos+3);
					}
					//B0000 キーバインドを「文字列挿入」にして2行目以降の行頭に***を書くと誤認識される
					AUChar	ch5 = 0;
					if( pos+4 < inText.GetItemCount() )
					{
						ch5 = inText.Get(pos+4);
					}
					pos++;
					if( ch1 == kUChar_LineEnd && ch2 == '*' && ch3 == '*' && ch4 == '*' && ch5 != '*' )//B0000
					{
						break;
					}
					actionText.Add(ch1);
				}
				if( actionText.GetItemCount() == 0 )
				{
					action = keyAction_NOP;
					break;
				}
				//B0000 行頭の****は***に置き換え
				actionText.ReplaceCstring("\r****","\r***");
				//
				action = keyAction_string;
				AKeyBindAction	act = keyAction_NOP;
				if( actionText.GetItemCount() >= 3 )
				{
					if( actionText.Get(0) == '<' && actionText.Get(1) == '<' && actionText.Get(2) == '<' )
					{
						AIndex	p = 3;
						AText	tok;
						actionText.GetToken(p,tok);
						tok.ChangeCaseUpper();
						act = GetApp().SPI_GetKeyBindActionFromToolCommandText(tok);
						for( AIndex i = 0; i < 3; i++ )
						{
							actionText.GetToken(p,tok);
							if( tok.GetItemCount() == 0 )
							{
								act = keyAction_NOP;
							}
							else if( tok.Get(0) != '>' )
							{
								act = keyAction_NOP;
							}
						}
						actionText.GetToken(p,tok);
						if( tok.GetItemCount() > 0 )
						{
							act = keyAction_NOP;
						}
						if( act != keyAction_NOP )
						{
							actionText.DeleteAll();
							action = act;
						}
					}
				}
				break;
			}
			else if( token.GetItemCount() == 1 )
			{
				AUChar	ch = token.Get(0);
				if( ch >= 'a' && ch <= 'z' )
				{
					key = static_cast<AKeyBindKey>(kKeyBindKey_A + ch - 'a');
				}
				else if( ch >= 'A' && ch <= 'Z' )
				{
					key = static_cast<AKeyBindKey>(kKeyBindKey_A + ch - 'A');
				}
				else if( ch >= '1' && ch <= '9' )//#1083
				{
					key = static_cast<AKeyBindKey>(kKeyBindKey_1 + ch - '1');
				}
				else switch(ch)
				{
				  case '0'://#1083
					{
						key = kKeyBindKey_0;
						break;
					}
				  case '-':
					{
						key = kKeyBindKey_Minus;
						break;
					}
				  case '^':
					{
						key = kKeyBindKey_Hat;
						break;
					}
				  case '\\':
					{
						key = kKeyBindKey_Backslash;
						break;
					}
				  case '@':
					{
						key = kKeyBindKey_Atmark;
						break;
					}
				  case '[':
					{
						key = kKeyBindKey_BracketStart;
						break;
					}
				  case ';':
					{
						key = kKeyBindKey_Semicolon;
						break;
					}
				  case ']':
					{
						key = kKeyBindKey_BracketEnd;
						break;
					}
				  case ',':
					{
						key = kKeyBindKey_Comma;
						break;
					}
				  case '.':
					{
						key = kKeyBindKey_Period;
						break;
					}
				  case '/':
					{
						key = kKeyBindKey_Slash;
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
			}
			else if( token.Compare("colon") == true )
			{
				key = kKeyBindKey_Colon;
			}
			else if( token.Compare("return") == true )
			{
				key = kKeyBindKey_Return;
			}
			else if( token.Compare("tab") == true )
			{
				key = kKeyBindKey_Tab;
			}
			else if( token.Compare("space") == true )
			{
				key = kKeyBindKey_Space;
			}
			else if( token.Compare("escape") == true || token.Compare("esc") == true )
			{
				key = kKeyBindKey_Escape;
			}
			else if( token.Compare("enter") == true )
			{
				key = kKeyBindKey_Enter;
			}
			else if( token.Compare("bs") == true )
			{
				key = kKeyBindKey_BS;
			}
			else if( token.Compare("delete") == true || token.Compare("del") == true )
			{
				key = kKeyBindKey_DEL;
			}
			else if( token.Compare("arrowup") == true )
			{
				key = kKeyBindKey_Up;
			}
			else if( token.Compare("arrowdown") == true )
			{
				key = kKeyBindKey_Down;
			}
			else if( token.Compare("arrowleft") == true )
			{
				key = kKeyBindKey_Left;
			}
			else if( token.Compare("arrowright") == true )
			{
				key = kKeyBindKey_Right;
			}
			else if( token.Compare("home") == true )
			{
				key = kKeyBindKey_Home;
			}
			else if( token.Compare("end") == true )
			{
				key = kKeyBindKey_End;
			}
			else if( token.Compare("pageup") == true )
			{
				key = kKeyBindKey_PageUp;
			}
			else if( token.Compare("pagedown") == true )
			{
				key = kKeyBindKey_PageDown;
			}
			else if( token.GetItemCount() == 2 )
			{
				if( token.Get(0) == 'f' && (token.Get(1)>='0'&&token.Get(1)<='9') )
				{
					key = static_cast<AKeyBindKey>(kKeyBindKey_F1 + token.Get(1)-'1');
				}
			}
			else if( token.GetItemCount() == 3 )
			{
				if( token.Get(0) == 'f' && token.Get(1) == '1' && (token.Get(2)>='0'&&token.Get(2)<='9') )
				{
					key = static_cast<AKeyBindKey>(kKeyBindKey_F10 + token.Get(2)-'0');
				}
			}
		}
		if( key != kKeyBindKey_Invalid )
		{
			SetData_KeybindAction(key,modifiers,action,actionText);
		}
	}
}

void	AModePrefDB::ExportKeyBindToText( AText& outText ) const
{
	outText.DeleteAll();
	for( AIndex index = 0; index < GetItemCount_NumberArray(kKeyBind_Key); index++ )
	{
		AKeyBindKey	key = static_cast<AKeyBindKey>(GetData_NumberArray(kKeyBind_Key,index));
		AKeyBindAction	action = static_cast<AKeyBindAction>(GetData_NumberArray(kKeyBind_Action,index));
		AText	actionText;
		GetData_TextArray(kKeyBind_Text,index,actionText);
		outText.AddCstring("***");
		//modifiers
		if( GetData_BoolArray(kKeyBind_CommandKey,index) == true )
		{
			outText.AddCstring("command+");
		}
		if( GetData_BoolArray(kKeyBind_ControlKey,index) == true )
		{
			outText.AddCstring("control+");
		}
		if( GetData_BoolArray(kKeyBind_OptionKey,index) == true )
		{
			outText.AddCstring("option+");
		}
		if( GetData_BoolArray(kKeyBind_ShiftKey,index) == true )
		{
			outText.AddCstring("shift+");
		}
		//key
		if( key >= kKeyBindKey_A && key <= kKeyBindKey_Z )
		{
			outText.Add('A'+key-kKeyBindKey_A);
		}
		else if( key >= kKeyBindKey_F1 && key <= kKeyBindKey_F15 )
		{
			AText	text;
			text.SetFormattedCstring("F%d",key-kKeyBindKey_F1+1);
			outText.AddText(text);
		}
		else if( key >= kKeyBindKey_1 && key <= kKeyBindKey_9 )//#915
		{
			outText.Add('1'+key-kKeyBindKey_1);
		}
		else if( key == kKeyBindKey_0 )
		{
			outText.Add('0');
		}
		else switch(key)
		{
		  case kKeyBindKey_Return:
			{
				outText.AddCstring("return");
				break;
			}
		  case kKeyBindKey_Tab:
			{
				outText.AddCstring("tab");
				break;
			}
		  case kKeyBindKey_Space:
			{
				outText.AddCstring("space");
				break;
			}
		  case kKeyBindKey_Escape:
			{
				outText.AddCstring("escape");
				break;
			}
		  case kKeyBindKey_Enter:
			{
				outText.AddCstring("enter");
				break;
			}
		  case kKeyBindKey_BS:
			{
				outText.AddCstring("BS");
				break;
			}
		  case kKeyBindKey_DEL:
			{
				outText.AddCstring("DEL");
				break;
			}
		  case kKeyBindKey_Up:
			{
				outText.AddCstring("arrowup");
				break;
			}
		  case kKeyBindKey_Down:
			{
				outText.AddCstring("arrowdown");
				break;
			}
		  case kKeyBindKey_Left:
			{
				outText.AddCstring("arrowleft");
				break;
			}
		  case kKeyBindKey_Right:
			{
				outText.AddCstring("arrowright");
				break;
			}
		  case kKeyBindKey_Colon:
			{
				outText.AddCstring("colon");
				break;
			}
		  case kKeyBindKey_Semicolon:
			{
				outText.AddCstring(";");
				break;
			}
		  case kKeyBindKey_Minus:
			{
				outText.AddCstring("-");
				break;
			}
		  case kKeyBindKey_Hat:
			{
				outText.AddCstring("^");
				break;
			}
		  case kKeyBindKey_Backslash:
			{
				outText.AddCstring("\\");
				break;
			}
		  case kKeyBindKey_Atmark:
			{
				outText.AddCstring("@");
				break;
			}
		  case kKeyBindKey_BracketStart:
			{
				outText.AddCstring("[");
				break;
			}
		  case kKeyBindKey_BracketEnd:
			{
				outText.AddCstring("]");
				break;
			}
		  case kKeyBindKey_Comma:
			{
				outText.AddCstring(",");
				break;
			}
		  case kKeyBindKey_Period:
			{
				outText.AddCstring(".");
				break;
			}
		  case kKeyBindKey_Slash:
			{
				outText.AddCstring("/");
				break;
			}
		  case kKeyBindKey_Home:
			{
				outText.AddCstring("home");
				break;
			}
		  case kKeyBindKey_End:
			{
				outText.AddCstring("end");
				break;
			}
		  case kKeyBindKey_PageUp:
			{
				outText.AddCstring("pageup");
				break;
			}
		  case kKeyBindKey_PageDown:
			{
				outText.AddCstring("pagedown");
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		//text
		outText.Add(kUChar_Colon);
		if( action == keyAction_string )
		{
			//B0000 キーバインドを「文字列挿入」にして2行目以降の行頭に***を書くと誤認識される
			//行頭の***を****にする。インポート時に変換。
			actionText.ReplaceCstring("\r***","\r****");
			//
			outText.AddText(actionText);
		}
		else if( action != keyAction_NOP )
		{
			AText	text;
			GetApp().SPI_GetToolCommandTextFromKeyBindAction(action,text);
			text.InsertCstring(0,"<<<");
			text.AddCstring(">>>");
			outText.AddText(text);
		}
		outText.Add(kUChar_LF);//#1436
	}
	outText.AddCstring("***");//B0000 ***をつけておけば、最後の改行コードがキーバインドの挿入文字列として認識されることはない
}

#if 0
//R0068
void	AModePrefDB::ImportKeywords( const AText& inText )
{
	/*#427
	//データ全削除
	while( GetItemCount_TextArray(kAdditionalCategory_Name) > 0 )
	{
		DeleteRow_Table(kAdditionalCategory_Name,0);
	}
	mAdditionalCategoryArray_KeywordArray.DeleteAll();
	*/
	//AutoUpdateのカテゴリ以外を削除 #427
	for( AIndex index = GetItemCount_TextArray(kAdditionalCategory_Name)-1;
				index >= 0;
				index-- )
	{
		if( GetData_BoolArray(kAdditionalCategory_AutoUpdate,index) == false )
		{
			DeleteRow_Table(kAdditionalCategory_Name,index);
		}
	}
	
	//解析
	for( AIndex pos = 0; pos < inText.GetItemCount();  )
	{
		AText	lineText;
		inText.GetLine(pos,lineText);
		if( lineText.GetItemCount() == 0 )   continue;
		if( lineText.Get(0) != '[' )   continue;
		
		AIndex	linepos = 0;
		AText	categoryname;
		for( ; linepos < inText.GetItemCount(); linepos++ )
		{
			if( lineText.Get(linepos) == ']' )
			{
				lineText.GetText(1,linepos-1,categoryname);
				linepos++;
				break;
			}
		}
		if( categoryname.GetItemCount() == 0 )   continue;
		
		AIndex	index = AddRow_Table(kAdditionalCategory_Name);//#427
		//#427 AIndex	index = mAdditionalCategoryArray_KeywordArray.AddNewObject();
		SetData_TextArray(kAdditionalCategory_Name,index,categoryname);
		
		while( linepos < lineText.GetItemCount() )
		{
			AText	t;
			lineText.GetToken(linepos,t);
			if( t.Compare("color") == true )
			{
				lineText.GetToken(linepos,t);
				if( t.Compare("(") == false )   break;
				lineText.GetToken(linepos,t);
				if( t.Compare("\"") == false )   break;
				lineText.GetToken(linepos,t);
				AColor	color = AColorWrapper::GetColorByHTMLFormatColor(t);
				SetData_ColorArray(kAdditionalCategory_Color,index,color);
			}
			if( t.Compare("bold") == true )
			{
				SetData_BoolArray(kAdditionalCategory_Bold,index,true);
			}
			if( t.Compare("regexp") == true )
			{
				SetData_BoolArray(kAdditionalCategory_RegExp,index,true);
			}
			if( t.Compare("ignorecase") == true )
			{
				SetData_BoolArray(kAdditionalCategory_IgnoreCase,index,true);
			}
			if( t.Compare("underline") == true )
			{
				SetData_BoolArray(kAdditionalCategory_Underline,index,true);
			}
			if( t.Compare("italic") == true )
			{
				SetData_BoolArray(kAdditionalCategory_Italic,index,true);
			}
			if( t.Compare("prior") == true )
			{
				SetData_BoolArray(kAdditionalCategory_PriorToOtherColor,index,true);
			}
			if( t.Compare("disabled") == true )
			{
				SetData_BoolArray(kAdditionalCategory_Disabled,index,true);
			}
		}
		
		AText	keywordTextToSet;//#427
		while( pos < inText.GetItemCount() )
		{
			AText	keyword;
			inText.GetLine(pos,keyword);
			if( keyword.GetItemCount() == 0 )   break;
			//#427 mAdditionalCategoryArray_KeywordArray.GetObject(index).Add(keyword);
			keywordTextToSet.AddText(keyword);//#427
			keywordTextToSet.Add(kUChar_LineEnd);//#427
		}
		SetData_TextArray(kAdditionalCategory_Keywords,index,keywordTextToSet);//#427
	}
	UpdateUserKeywordCategoryAndKeyword();
}

//R0068
void	AModePrefDB::ExportKeywords( AText& outText ) 
{
	NormalizeAdditionalCategoryName();
	
	outText.DeleteAll();
	
	for( AIndex index = 0; index < GetItemCount_Array(kAdditionalCategory_Name); index++ )
	{
		//自動更新データはExportしない #427
		if( GetData_BoolArray(kAdditionalCategory_AutoUpdate,index) == true )
		{
			continue;
		}
		//
		AText	categoryName;
		GetData_TextArray(kAdditionalCategory_Name,index,categoryName);
		outText.AddCstring("[");
		outText.AddText(categoryName);
		outText.AddCstring("]");
		AColor	color;
		GetData_ColorArray(kAdditionalCategory_Color,index,color);
		AText	colortext;
		AColorWrapper::GetHTMLFormatColorText(color,colortext);
		outText.AddCstring(" color(\"");
		outText.AddText(colortext);
		outText.AddCstring("\") ");
		if( GetData_BoolArray(kAdditionalCategory_Bold,index) == true )
		{
			outText.AddCstring(" bold");
		}
		if( GetData_BoolArray(kAdditionalCategory_RegExp,index) == true )
		{
			outText.AddCstring(" regexp");
		}
		if( GetData_BoolArray(kAdditionalCategory_IgnoreCase,index) == true )
		{
			outText.AddCstring(" ignorecase");
		}
		if( GetData_BoolArray(kAdditionalCategory_Underline,index) == true )
		{
			outText.AddCstring(" underline");
		}
		if( GetData_BoolArray(kAdditionalCategory_Italic,index) == true )
		{
			outText.AddCstring(" italic");
		}
		if( GetData_BoolArray(kAdditionalCategory_PriorToOtherColor,index) == true )
		{
			outText.AddCstring(" prior");
		}
		if( GetData_BoolArray(kAdditionalCategory_Disabled,index) == true )
		{
			outText.AddCstring(" disabled");
		}
		outText.AddCstring("\r");
		//#427 AItemCount	keywordCount = mAdditionalCategoryArray_KeywordArray.GetObjectConst(index).GetItemCount();
		//#427 キーワードテキストをtextarrayに展開
		ATextArray	keywordArray;
		keywordArray.ExplodeFromText(GetData_TextArrayRef(kAdditionalCategory_Keywords).GetTextConst(index),
					kUChar_LineEnd);
		AItemCount	keywordCount = keywordArray.GetItemCount();
		//
		for( AIndex j = 0; j < keywordCount; j++ )
		{
			const AText&	t = keywordArray.GetTextConst(j);//#427 mAdditionalCategoryArray_KeywordArray.GetObjectConst(index).GetTextConst(j);
			if( t.GetItemCount() == 0 )   continue;
			outText.AddText(t);
			outText.Add(kUChar_CR);
		}
		outText.Add(kUChar_CR);
	}
}
#endif

/*#844 keywords.txtへの保存をしないことにしたため不要。
//R0068
void	AModePrefDB::NormalizeAdditionalCategoryName()
{
	AItemCount	count = GetItemCount_Array(kAdditionalCategory_Name);
	for( AIndex index = 0; index < count; index++ )
	{
		AText	name;
		GetData_TextArray(kAdditionalCategory_Name,index,name);
		name.ReplaceChar('[','(');
		name.ReplaceChar(']',')');
		AText	n;
		n.SetText(name);
		n.Insert1(0,'\t');
		SetData_TextArray(kAdditionalCategory_Name,index,n);
		while( Find_TextArray(kAdditionalCategory_Name,name) != kIndex_Invalid )
		{
			name.Add('_');
		}
		SetData_TextArray(kAdditionalCategory_Name,index,name);
	}
}
*/

#pragma mark ===========================

#pragma mark ---多言語

//#427
/**
モード名多言語名称を全削除
*/
void	AModePrefDB::RemoveMultiLanguageModeNames()
{
	AText	text;
	SetData_Text(kJapaneseName,text);
	SetData_Text(kEnglishName,text);
	SetData_Text(kFrenchName,text);
	SetData_Text(kGermanName,text);
	SetData_Text(kSpanishName,text);
	SetData_Text(kItalianName,text);
	SetData_Text(kDutchName,text);
	SetData_Text(kSwedishName,text);
	SetData_Text(kNorwegianName,text);
	SetData_Text(kDanishName,text);
	SetData_Text(kFinnishName,text);
	SetData_Text(kPortugueseName,text);
	SetData_Text(kSimplifiedChineseName,text);
	SetData_Text(kTraditionalChineseName,text);
	SetData_Text(kKoreanName,text);
	SetData_Text(kPolishName,text);
	SetData_Text(kPortuguesePortugalName,text);
	SetData_Text(kRussianName,text);
}

/**
標準モードの名前を取得
*/
void	AModePrefDB::SetNormalModeNames()
{
	RemoveMultiLanguageModeNames();
	AText	text;
	text.SetLocalizedText("NormalModeName_Japanese");
	SetData_Text(kJapaneseName,text);
}

#pragma mark ===========================

#pragma mark ---各種フォルダ取得

//#994
/**
pluginsフォルダ取得
*/
void	AModePrefDB::GetPluginsFolder( const ABool inAutoUpdate, AFileAcc& outFolder ) const
{
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	/*#1275 autoupdateフォルダはGetAutoUpdateFolder()で取得
	if( inAutoUpdate == true )
	{
		outFolder.SpecifyChild(modefolder,"autoupdate/plugins");
	}
	else
	{
		outFolder.SpecifyChild(modefolder,"plugins");
	}
	*/
	if( inAutoUpdate == true )
	{
		GetAutoUpdateFolder(modefolder);
	}
	outFolder.SpecifyChild(modefolder,"plugins");
}

/**
toolフォルダ取得
*/
void	AModePrefDB::GetToolFolder( const ABool inAutoUpdate, AFileAcc& outFolder ) const//#427
{
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	/*#1275 autoupdateフォルダはGetAutoUpdateFolder()で取得
	if( inAutoUpdate == true )//#427
	{
		outFolder.SpecifyChild(modefolder,"autoupdate/tool");
	}
	else
	{
		outFolder.SpecifyChild(modefolder,"tool");
	}
	*/
	if( inAutoUpdate == true )
	{
		GetAutoUpdateFolder(modefolder);
	}
	outFolder.SpecifyChild(modefolder,"tool");
}

/**
toolbarフォルダ取得
*/
void	AModePrefDB::GetToolbarFolder( /*#844 const ABool inAutoUpdate,*/ AFileAcc& outFolder ) const//#427
{
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	/*#844
	if( inAutoUpdate == true )//#427
	{
		outFolder.SpecifyChild(modefolder,"autoupdate/toolbar");
	}
	else
	*/
	{
		outFolder.SpecifyChild(modefolder,"toolbar");
	}
}

//#427
/**
AutoUpdateフォルダ取得
*/
void	AModePrefDB::GetAutoUpdateFolder( AFileAcc& outFolder ) const
{
	//Webから更新以外（＝アプリ内のdefaultを使用）かどうかを判定
	//#1275 Webから更新以外（＝アプリ内のdefaultを使用）の場合は、autoupdateにコピーせず、アプリ内のdefaultをautoupdateフォルダとみなす。
	//#1374 if( GetData_Bool(AModePrefDB::kEnableModeUpdateFromWeb) == false )
	{
		//アプリ内のdefaultを使用する場合
		
		//モードdefaultフォルダを取得
		AFileAcc	appModeRootFolder;
		GetApp().SPI_GetModeDefaultFolder(appModeRootFolder);
		//コピー元モード名を取得
		AText	modename;
		GetData_Text(AModePrefDB::kModeUpdateSourceModeName,modename);
		if( modename.GetItemCount() == 0 )
		{
			//コピー元モード名が空の場合（＝メニューの最初の項目が選択されていた場合）、同じモード名のモードをコピー元にする
			GetModeRawName(modename);
		}
		//コピー元フォルダ取得
		outFolder.SpecifyChild(appModeRootFolder,modename);
	}
	/*#1374
	else
	{
		//Webから更新の場合
		AFileAcc	modefolder;
		GetModeFolder(modefolder);
		outFolder.SpecifyChild(modefolder,"autoupdate");
	}
	*/
}

//#427
/**
autoupdate_userdataフォルダ取得
*/
void	AModePrefDB::GetAutoUpdateUserDataFolder( const AConstCstringPtr inChildPath, AFileAcc& outFolder ) const
{
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	AFileAcc	userDataFolder;
	userDataFolder.SpecifyChild(modefolder,"autoupdate_userdata");
	outFolder.SpecifyChild(userDataFolder,inChildPath);
}

//#467
/**
システムヘッダファイルかどうかを返す
*/
ABool	AModePrefDB::IsSystemHeaderFile( const AFileAcc& inFile ) const
{
	/*
	for( AIndex i =0; i < mSystemHeaderFileArray.GetItemCount(); i++ )
	{
		AText	folderpath;
		mSystemHeaderFileArray.GetObjectConst(i).GetPath(folderpath);
		AText	filepath;
		inFile.GetPath(filepath);
		if( folderpath.CompareMin(filepath) == true )   return true;
	}
	return false;
	*/
	//mutex lock #717
	AStMutexLocker	locker(mSystemHeaderIdentifierLinkListMutex);
	
	AText	path;
	inFile.GetNormalizedPath(path);
	return (mSystemHeaderFileArray_NormalizedPath.Find(path) != kIndex_Invalid);
}

#pragma mark ---UnitTest

ABool	AModePrefDB::UnitTest( AText& ioText )
{
	//テスト用モードデータの作成
	// ここに来た状態で"/unittest_mode/testmode"という空ファイルができている。
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	//tool
	AFileAcc	toolfolder;
	GetToolFolder(false,toolfolder);
	toolfolder.CreateFolder();
	AFileAcc	tool1;
	tool1.SpecifyChild(toolfolder,"tool1");
	tool1.CreateFile();
	AFileAcc	tool2;
	tool2.SpecifyChild(toolfolder,"tool2");
	tool2.CreateFolder();
	AFileAcc	tool3;
	tool3.SpecifyChild(toolfolder,".invisible");
	tool3.CreateFolder();
	AFileAcc	order;
	order.SpecifyChild(toolfolder,"order");
	AText	orderText;
	orderText.SetCstring("tool1\rtool2\r");
	order.WriteFile(orderText);
	AFileAcc	tool2_1;
	tool2_1.SpecifyChild(tool2,"tool2_1.scpt");
	tool2_1.CreateFile();
	AFileAcc	tool2_2;
	tool2_2.SpecifyChild(tool2,"tool2_2");
	tool2_2.CreateFile();
	AFileAcc	order2;
	order2.SpecifyChild(tool2,"order");
	AText	orderText2;
	orderText2.SetCstring("tool2_1.scpt\rtool2_2\r");
	order2.WriteFile(orderText2);
	
	//ロード
	LoadOrWaitLoadComplete();
	
	//チェック
	//tool
	/*AMenuRef	toolmenuref = UMenu::GetToolMenuRef();
	mToolMenuRoot.CopyMenuTo(toolmenuref,true);
	if( AMenuWrapper::GetMenuItemCount(toolmenuref) != 2 )   return false;
	AText	text;
	AMenuWrapper::GetMenuItemText(toolmenuref,0,text);
	if( text.Compare("tool1") == false )   return false;
	AMenuWrapper::GetMenuItemText(toolmenuref,1,text);
	if( text.Compare("tool2") == false )   return false;
	AMenuRef	tool2sub;
	AMenuWrapper::GetSubMenu(toolmenuref,1,tool2sub);
	if( tool2sub == NULL )   return false;
	AMenuWrapper::GetMenuItemText(tool2sub,0,text);
	if( text.Compare("tool2_1") == false )   return false;
	AMenuWrapper::GetMenuItemText(tool2sub,1,text);
	if( text.Compare("tool2_2") == false )   return false;*/
	return true;
}
#else
//modeprefconverterでのみ使用する箇所 #1034

/**
2.1.10以前のデータを読み込み、バージョン3形式で保存する。（modeprefconverterで使用）
*/
void	AModePrefDB::LoadMacLegacyAndSave( const AModeIndex& inModeID, const AFileAcc& inModeFolder )
{
	mModeID = inModeID;
	mModeFolder = inModeFolder;//#920
	
	//------------------旧形式データ読み込み(2.1.8a7以前)------------------
	try
	{
		LoadMacLegacy();
	}
	catch(...)
	{
		//ファイルの破壊等によるエラー
		_ACError("AModePrefDB::LoadDB() caught exception.",NULL);//#199
	}
	//LoadMacLegacy()によって読み込んだ2.1.8a7以前形式キーワードをmodeprefDBへコピーする#427
	for( AIndex categoryIndex = 0; 
		 categoryIndex < mLegacyKeywordTempDataArray.GetItemCount();
		 categoryIndex++ )
	{
		AText	text;
		mLegacyKeywordTempDataArray.GetObjectConst(categoryIndex).Implode(kUChar_LineEnd,text);
		SetData_TextArray(kAdditionalCategory_Keywords,categoryIndex,text);
	}
	
	//------------------旧形式データ読み込み(2.1.8～2.1.10)------------------
	//LoadFromPrefFile()は、ファイルが存在していれば、旧データは削除する。（上記で読んだキーワードも全て削除）
	AText	modeRawName;
	GetModeRawName(modeRawName);
	AFileAcc	legacyModeFile;
	legacyModeFile.SpecifyChild(mModeFolder,modeRawName);
	if( legacyModeFile.Exist() == true )
	{
		LoadFromPrefFile(legacyModeFile,'MMKE');
		//MMKE-2のキーワードデータ読み込み #427
		AText	keywordText;
		legacyModeFile.ReadResouceTo(keywordText,'MMKE',2);
		if( keywordText.GetItemCount() > 0 )
		{
			//LoadLegacyKeywordTextIntoEmptyCatgory()は空のカテゴリに対してのみ、keyword.txtからキーワードを読み込む
			LoadLegacyKeywordTextIntoEmptyCatgory(keywordText,*this);
		}
	}
	
	//dataフォルダ取得・生成
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	dataFolder.CreateFolder();
	
	//ModePref.mi書き込み
	AFileAcc	prefFile;
	prefFile.SpecifyChild(dataFolder,"ModePreferences.mi");
	prefFile.CreateFile();
	WriteToPrefTextFile(prefFile);
}

#endif
