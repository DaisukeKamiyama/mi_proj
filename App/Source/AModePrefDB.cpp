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
//modeprefconverter�ł͎g�p���Ȃ��ӏ� #1034
#include "AApp.h"
#if IMPLEMENTATION_FOR_MACOSX
#include <unistd.h>
#endif
#include "AToolPrefDB.h"//#305
#include "ACSVKeywordLoader.h"
#endif

#pragma mark ===========================
#pragma mark [�N���X]AModePrefDB
#pragma mark ===========================
//ModePrefDB

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AModePrefDB::AModePrefDB( AObjectArrayItem* inParent )
: ADataBase(inParent)
#ifndef MODEPREFCONVERTER
//modeprefconverter�ł͎g�p���Ȃ��ӏ� #1034
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
//modeprefconverter�ł͎g�p���Ȃ��ӏ� #1034
	//�L�[���[�h�T�[�`���A���e�L�X�g����łȂ���΁A�d���`�F�b�N���Ȃ��悤�ɂ���
	mKeywordList.SetDontCheckDuplicateIfInfoTextExists();
#endif
	
	/*#688
	ANumber	oldDefaultFontNumber = 0;
#if IMPLEMENTATION_FOR_MACOSX
	oldDefaultFontNumber = AFontWrapper::GetDefaultFont();
#endif
	//���f�[�^
	CreateData_Number(kDefaultFont_compat,							"DefaultFont",							oldDefaultFontNumber,kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kDefaultPrintOption_PrintFont_compat,			"DefaultPrintOption_PrintFont",			oldDefaultFontNumber,kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kDefaultPrintOption_PageNumberFont_compat,	"DefaultPrintOption_PageNumberFont",	oldDefaultFontNumber,kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kDefaultPrintOption_FileNameFont_compat,		"DefaultPrintOption_FileNameFont",		oldDefaultFontNumber,kNumber_MinNumber,kNumber_MaxNumber);//win
	CreateData_Number(kDefaultPrintOption_LineNumberFont_compat,	"DefaultPrintOption_LineNumberFont",	oldDefaultFontNumber,kNumber_MinNumber,kNumber_MaxNumber);//win
	*/
	//				�f�[�^ID								�t�@�C���pKey							�����l			(�ŏ�,�ő�)			(�ŏ�,�ő�)
	CreateData_Bool(kUseRegExpCommentStart,					"UseRegExpCommentStart",			false);
	CreateData_Bool(kUseRegExpCommentEnd,					"UseRegExpCommentEnd",				false);
	CreateData_Number(kBackgroundImageTransparency,			"BackgroundImageTransparency",		85,				0,100);
	CreateData_Number(kWrapWithLetterCount_LetterCount,		"WrapWithLetterCount LetterCount",	80,				2,9999);
	CreateData_Bool(kConvert5CToA5WhenOpenJIS,				"Convert5CToA5WhenOpenJIS",			false);
	
	//				�f�[�^ID								�t�@�C���pKey							�����l			(�ŏ�,�ő�)			(�ŏ�,�ő�)
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
	CreateData_Bool(kInputBackslashByYenKey,				"InputBackslashByYenKey",			false);//#917 �f�t�H���gfalse�ɕύX
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
	
	//���@��`�֘A
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
	CreateData_Text(kDefaultFontNameOld,					"DefaultFontName",					"Monaco");//#375 ���o�[�W�����݊����̂��߁B���ۂɂ�kDefaultFontName���g���B
	CreateData_FloatNumber(kDefaultFontSize,				"DefaultFontSize",					9.0,			6.0,256.0);//B0398 �ŏ��l�ύX
	CreateData_Number(kDefaultTabWidth,						"DefaultTabWidth",					4,				1,128);
	CreateData_Number(kIndentWidth,							"IndentWidth",						4,				1,128);
	CreateData_Number(kDefaultWindowWidth,					"DefaultWindowWidth",				900,			50,5000);
	CreateData_Number(kDefaultWindowHeight,					"DefaultWindowHeight",				800,			50,5000);
	
	//win CreateData_Font(kDefaultPrintOption_PrintFont,					"DefaultPrintOption_PrintFont",			AFontWrapper::GetDefaultFont());
	CreateData_Text(kDefaultPrintOption_PrintFontName,				"DefaultPrintOption_PrintFontNameUni",	"default");//#375 defaultfont);
	CreateData_Text(kDefaultPrintOption_PrintFontNameOld,			"DefaultPrintOption_PrintFontName",		"Monaco");//#375 ���o�[�W�����݊����̂��߁B
	CreateData_FloatNumber(kDefaultPrintOption_PrintFontSize,		"DefaultPrintOption_PrintFontSize",		12.0,			3.0,256.0);
	//win CreateData_Font(kDefaultPrintOption_PageNumberFont,				"DefaultPrintOption_PageNumberFont",	AFontWrapper::GetDefaultFont());
	CreateData_Text(kDefaultPrintOption_PageNumberFontName,			"DefaultPrintOption_PageNumberFontNameUni","default");//#375 defaultfont);
	CreateData_Text(kDefaultPrintOption_PageNumberFontNameOld,		"DefaultPrintOption_PageNumberFontName","Monaco");//#375 ���o�[�W�����݊����̂��߁B
	CreateData_FloatNumber(kDefaultPrintOption_PageNumberFontSize,	"DefaultPrintOption_PageNumberFontSize",12.0,			3.0,256.0);
	//win CreateData_Font(kDefaultPrintOption_FileNameFont,				"DefaultPrintOption_FileNameFont",		AFontWrapper::GetDefaultFont());
	CreateData_Text(kDefaultPrintOption_FileNameFontName,			"DefaultPrintOption_FileNameFontNameUni","default");//#375 defaultfont);
	CreateData_Text(kDefaultPrintOption_FileNameFontNameOld,		"DefaultPrintOption_FileNameFontName",	"Monaco");//#375 ���o�[�W�����݊����̂��߁B
	CreateData_FloatNumber(kDefaultPrintOption_FileNameFontSize,	"DefaultPrintOption_FileNameFontSize",	12.0,			3.0,256.0);
	//win CreateData_Font(kDefaultPrintOption_LineNumberFont,				"DefaultPrintOption_LineNumberFont",	AFontWrapper::GetDefaultFont());
	CreateData_Text(kDefaultPrintOption_LineNumberFontName,			"DefaultPrintOption_LineNumberFontNameUni","default");//#375 defaultfont);
	CreateData_Text(kDefaultPrintOption_LineNumberFontNameOld,		"DefaultPrintOption_LineNumberFontName","Monaco");//#375 ���o�[�W�����݊����̂��߁B
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
	
	CreateData_Text(kIndentExeLetters,						"IndentExeLetters",					"");//B0000�����l�ύX
	
	CreateData_Bool(kSetDefaultCreator,						"SetDefaultCreator",				true);//B0000�����l�ύX #226
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
	
	//#305 ���ꖈ���[�h��
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
	
	//Web����̎����X�V
	CreateData_Bool(kEnableModeUpdateFromWeb,				"EnableModeUpdateFromWeb",			false);
	
	//�Ή���������͂�SDF���g��
	CreateData_Bool(kEnableSDFCorrespond,					"EnableSDFCorrespond",				true);
	
	//�A�v�����g�ݍ��݃��[�h�̍X�V�����[�h
	CreateData_Text(kModeUpdateSourceModeName,				"ModeUpdateSourceModeName",			"");
	
	//�A�v�����g�ݍ��݃��[�h����̍X�V���́A�A�v���X�V�����L��
	CreateData_Text(kAutoUpdateFromAppDateTimeText,			"AutoUpdateFromAppDateTimeText",	"");
	
	//diff�\���s�����x
	CreateData_Number(kDiffColorOpacity,					"DiffColorOpacity",					30,				1,100);
	
	//�I��F�s�����x
	CreateData_Number(kSelectionOpacity,					"SelectionOpacity",					80,				1,100);
	
	//�f�t�H���g���@�p�[�g
	CreateData_Text(kDefaultSyntaxPart,						"DefaultSyntaxPart",				"");
	
	//�s���o�b�N�X�y�[�X�͍��V�t�g�ɂ���
	CreateData_Bool(kShiftleftByBackspace,					"ShiftleftByBackspace",				true);
	
	//�c�������[�h #558
	CreateData_Bool(kVerticalMode,							"VerticalMode",						false);
	
	//���ʃn�C���C�g #1406
	CreateData_Bool(kHighlightBrace,						"HighlightBrace",					false);
	
	//Flexible�^�u #1421
	CreateData_Bool(kEnableFlexibleTabPositions,			"EnableFlexibleTabPositions",		false);
	CreateData_Text(kFlexibleTabPositions,					"FlexibleTabPositions",				"8,8,8,8,8,8,8,8,8,8");
	
	//�v���O�C�� #994
	CreateTableStart();
	CreateData_TextArray(kPluginData_Id,					"PluginData_Id",					"");
	CreateData_TextArray(kPluginData_Name,					"PluginData_Name",					"");
	CreateData_TextArray(kPluginData_Options,				"PluginData_Options",				"");
	CreateData_BoolArray(kPluginData_Enabled,				"PluginData_Enabled",				false);
	CreateTableEnd();
	
	//�v���O�C���\���� #994
	CreateTableStart();
	CreateData_TextArray(kPluginDisplayName_Id_Language,	"PluginDisplayName_Id_Language",	"");
	CreateData_TextArray(kPluginDisplayName_DisplayName,	"PluginDisplayName_DisplayName",	"");
	CreateTableEnd();
	
	//#
	//CreateData_Bool(997,									"testtest997",					true);
	//CreateData_TextArray(996,								"testtest996",					"996");
	//�J�e�S���[
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
	CreateData_TextArray(kJumpSetup_MenuText,				"JumpSetup_MenuText",				"$0");//B0000�����l�ύX
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
//�f�X�g���N�^
AModePrefDB::~AModePrefDB()
{
	//SaveToFile();
}

//#427
/**
���[�h�t�@�C�����擾
*/
void	AModePrefDB::GetModeRawName( AText& outName ) const
{
	AText	modename;
	mModeFolder.GetFilename(outName);
}

/**
���[�h�t�H���_�擾
*/
void	AModePrefDB::GetModeFolder( AFileAcc& outFolder ) const
{
	outFolder = mModeFolder;
}

//#350
/**
data�t�H���_�擾
*/
void	AModePrefDB::GetDataFolder( AFileAcc& outFolder ) const
{
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	outFolder.SpecifyChild(modefolder,"data");
}

//#350
/**
keywords.txt�ǂݍ���
*/
void	AModePrefDB::LoadLegacyKeywordTextIntoEmptyCatgory( const AText& inDataText, AModePrefDB& ioDB )//#427
{
	//�J�e�S���̂����A�L�[���[�htext����A���Aautoupdate�łȂ��J�e�S������������ #427
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
		//#427 categoryName����v����J�e�S����shouldbeAddedArray��true�Ȃ�A�L�[���[�h��ǉ�����B
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
//modeprefconverter�ł͎g�p���Ȃ��ӏ� #1034
//#305
/**
���[�h������^�C�g���擾
*/
void	AModePrefDB::GetModeMultiLanguageName( const ALanguage inLanguage, AText& outName ) const
{
	//���[�h������^�C�g���擾
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
			//�����Ȃ�
			break;
		}
	}
}

//#305
/**
���[�h������^�C�g���ݒ�
*/
void	AModePrefDB::SetModeMultiLanguageName( const ALanguage inLanguage, const AText& inName )
{
	//���[�h������^�C�g���ݒ�
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
			//�����Ȃ�
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark ---������

//������
//�I�u�W�F�N�g�쐬����AApp����R�[�������
void	AModePrefDB::InitMode( const AModeIndex& inModeID, const AFileAcc& inModeFolder )
{
	mModeID = inModeID;
	//#920 mModeFile = inModeFile;
	mModeFolder = inModeFolder;//#920
	
	//#427 CreateData_XXX()�̓R���X�g���N�^�ֈړ�
	
	//#852 InitLoad();//R0000
	
	//#852
	//data/suffix.mi������΁Asuffix.mi����ǂݍ��ށB�i�K�v�ŏ����̃f�[�^�݂̂��i�[����Ă���j
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	AFileAcc	suffixPrefFile;
	suffixPrefFile.SpecifyChild(dataFolder,"suffix.mi");
	if( suffixPrefFile.Exist() == true )
	{
		//suffix.mi����ǂݍ���
		LoadFromPrefTextFile(suffixPrefFile);
	}
	else
	{
		//�S�f�[�^�ǂݍ���
		LoadDB();
		//suffix.mi��ۑ��i����N�����̍������̂��߁j #1275 �������Bsuffix.mi����ǂݍ��܂Ȃ������ꍇ�����ۑ�����悤�ɏC���B
		SaveSuffixDB();
	}
	//suffix�n�b�V�����X�V
	UpdateSuffixHash();
}

//R0000
/**
���[�h�i��ɕK�v�ȃf�[�^�̃��[�h�B�N�����ɓǂݍ��܂��j
*/
void	AModePrefDB::LoadDB()//#852
{
	//#852
	//�S�f�[�^�폜
	DeleteAllModeData();
	
	// ------------ ModePreferences.mi�ǂݍ��� ------------
	
	//data�t�H���_�擾
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	//�o�[�W����3�p�ݒ�t�@�C��
	AFileAcc	preffile;
	preffile.SpecifyChild(dataFolder,"ModePreferences.mi");
	//�o�[�W����2.1�p�ݒ�t�@�C��
	AFileAcc	v2prefFile;
	v2prefFile.SpecifyChild(dataFolder,"modepref.mi");
	if( preffile.Exist() == true )
	{
		//==================�o�[�W����3�p�ݒ�t�@�C������ǂݍ���==================
		
		//#1419
		// ------------ ���[�hdefault�f�[�^����ǂݍ��� ------------
		//mi�A�b�v�O���[�h��ɁA���[�U�[�f�[�^�ɑ��݂��Ȃ��f�[�^���A���[�hdefault�f�[�^����ǂݍ��ނ悤�ɂ��邽�߁B
		
		//�u�R�s�[�����[�h���v�ݒ���擾���邽�߂Ɉ�U���[�U�[�f�[�^��ǂݍ��ށiGetAutoUpdateFolder()�Łu�R�s�[�����[�h���v�ݒ�f�[�^���K�v�j
		LoadFromPrefTextFile(preffile);
		
		//�u�R�s�[�����[�h�v����f�[�^�����[�h����
		AFileAcc	defaultFolder;
		GetAutoUpdateFolder(defaultFolder);
		AFileAcc	defaultPrefFile;
		defaultPrefFile.SpecifyChild(defaultFolder,"data/ModePreferences.mi");
		LoadFromPrefTextFile(defaultPrefFile);
		
		// ------------ ���[�U�[�f�[�^����ǂݍ��� ------------
		LoadFromPrefTextFile(preffile);
	}
	else if( v2prefFile.Exist() == true )
	{
		//==================�o�[�W����2.1.11�`2.1.13�p�ݒ�t�@�C������ǂݍ���==================
		//data/modepref.mi������Γǂݍ��݁i2.1.11�ȍ~�j
		//�i���̏ꍇ�AMacLegacy(2.1.8a7�ȑO)���AMMKE(2.1.8�`2.1.10)���ǂݍ��܂Ȃ��B�������̂��߁B�j
		LoadFromPrefTextFile(v2prefFile);
		//keywords.txt����ǂݍ��� #427
		//2.1.13b1���L�[���[�h��modeprefDB���Ɋi�[����悤�ɂ����B
		//�J�e�S���̃L�[���[�h�f�[�^(kAdditionalCategory_Keywords)����̃J�e�S���̂݁A
		//������keywords.txt����kAdditionalCategory_Keywords�փL�[���[�h�ǂݍ��݂���B
		//�i�����ł̓��[�U�[�쐬�L�[���[�h�̂݁B�����X�V�̂ق��͎����X�Vmodepref.mi�ǂݍ��ݎ�
		//�܂�A����autoupdateDB.LoadFromPrefTextFile()���s����ɁAkeywords.txt�ǂݍ��ށB�j
		AFileAcc	keywordTextFile;
		keywordTextFile.SpecifyChild(dataFolder,"keywords.txt");
		AText	keywordText;
		keywordTextFile.ReadTo(keywordText);
		if( keywordText.GetItemCount() > 0 )
		{
			//LoadLegacyKeywordTextIntoEmptyCatgory()�͋�̃J�e�S���ɑ΂��Ă̂݁Akeyword.txt����L�[���[�h��ǂݍ���
			LoadLegacyKeywordTextIntoEmptyCatgory(keywordText,*this);
		}
	}
	else
	{
		/*#1034
		//==================�o�[�W����2.1.10�ȑO�p�ݒ�t�@�C������ǂݍ���==================
#if IMPLEMENTATION_FOR_MACOSX
		//------------------���`���f�[�^�ǂݍ���(2.1.8a7�ȑO)------------------
		try
		{
			LoadMacLegacy();
		}
		catch(...)
		{
			//�t�@�C���̔j�󓙂ɂ��G���[
			_ACError("AModePrefDB::LoadDB() caught exception.",NULL);//#199
		}
		//LoadMacLegacy()�ɂ���ēǂݍ���2.1.8a7�ȑO�`���L�[���[�h��modeprefDB�փR�s�[����#427
		for( AIndex categoryIndex = 0; 
					categoryIndex < mLegacyKeywordTempDataArray.GetItemCount();
					categoryIndex++ )
		{
			AText	text;
			mLegacyKeywordTempDataArray.GetObjectConst(categoryIndex).Implode(kUChar_LineEnd,text);
			SetData_TextArray(kAdditionalCategory_Keywords,categoryIndex,text);
		}
		
		//------------------���`���f�[�^�ǂݍ���(2.1.8�`2.1.10)------------------
		//LoadFromPrefFile()�́A�t�@�C�������݂��Ă���΁A���f�[�^�͍폜����B�i��L�œǂ񂾃L�[���[�h���S�č폜�j
		AText	modeRawName;
		GetModeRawName(modeRawName);
		AFileAcc	legacyModeFile;
		legacyModeFile.SpecifyChild(mModeFolder,modeRawName);
		if( legacyModeFile.Exist() == true )
		{
			LoadFromPrefFile(legacyModeFile,'MMKE');
			//MMKE-2�̃L�[���[�h�f�[�^�ǂݍ��� #427
			AText	keywordText;
			legacyModeFile.ReadResouceTo(keywordText,'MMKE',2);
			if( keywordText.GetItemCount() > 0 )
			{
				//LoadLegacyKeywordTextIntoEmptyCatgory()�͋�̃J�e�S���ɑ΂��Ă̂݁Akeyword.txt����L�[���[�h��ǂݍ���
				LoadLegacyKeywordTextIntoEmptyCatgory(keywordText,*this);
			}
		}
#endif
		*/
		/*#1355 Xcode10�ȍ~��32bit�r���h���ł��Ȃ��Ȃ�����Catalina��32bit�N���ł��Ȃ��Ȃ����̂ŁA���\�[�X�t�H�[�N�`���̃��[�h�f�[�^�̃R���o�[�g�͑Ή����Ȃ��B
		//#1034
		//���`���f�[�^��version3�`����convert����
		//�imodeprefconverter�����s�j
		ATextArray	argArray;
		AText	command;
		//modeprefconverter�R�}���h�̃p�X���擾
		AFileAcc	appFile;
		AFileWrapper::GetAppFile(appFile);
		AFileAcc	converterFile;
		converterFile.SpecifyChild(appFile,"Contents/MacOS/mi_modeprefconverter");
		converterFile.GetNormalizedPath(command);
		argArray.Add(command);
		//���[�h�t�H���_�̃p�X���擾
		AFileAcc	modeFolder;
		GetModeFolder(modeFolder);
		AText	modefolderPath;
		modeFolder.GetNormalizedPath(modefolderPath);
		argArray.Add(modefolderPath);
		//���s
		AText	resulttext;
		GetApp().SPI_GetChildProcessManager().ExecuteGeneralSyncCommand(command,argArray,modefolderPath,resulttext);
		//���[�h
		LoadFromPrefTextFile(preffile);
		*/
	}
	
	//B0000
	CorrectTable_MakeColumnUnique(kSyntaxDefinitionCategory_Name,kSyntaxDefinitionCategory_Name);
	
	//#868
	//�W�����[�h�̏ꍇ�́A�u�W�����[�h�Ɠ����v�ݒ�͑S��false
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
	
	/*#1275 �A�v�����̃��[�h�t�@�C������R�s�[����̂ł͂Ȃ��A�A�v�����̃��[�h�t�@�C���𒼐ڎQ�Ƃ���悤�ɂ���B
	�A�v���X�V��ŏ��������s����邾�������AHTML���[�h�̏ꍇ�����������Ԃ�����i5�b���x�j�̂ŁA���P����B
	//==================�A�v���P�[�V�������̃��[�h�t�@�C������̎����X�V==================
	if( GetData_Bool(AModePrefDB::kEnableModeUpdateFromWeb) == false )
	{
		//���݋N�����Ă���A�v���̍X�V�����擾
		AText	thisAppModifiedDateTimeText;
		GetApp().SPI_GetAppModifiedDataTimeText(thisAppModifiedDateTimeText);
		//���[�h�f�[�^�ɋL������Ă���Ō�̃A�v���̍X�V�����擾
		AText	lastUpdateFromAppDateTimeText;
		GetData_Text(kAutoUpdateFromAppDateTimeText,lastUpdateFromAppDateTimeText);
		//��̂Q���Ⴄ�ꍇ�́A�A�v�����g�ݍ���default�t�H���_����R�s�[
		if( thisAppModifiedDateTimeText.Compare(lastUpdateFromAppDateTimeText) == false )
		{
			//���[�hdefault�t�H���_���擾
			AFileAcc	appModeRootFolder;
			GetApp().SPI_GetModeDefaultFolder(appModeRootFolder);
			//�R�s�[�����[�h�����擾
			AText	modename;
			GetData_Text(AModePrefDB::kModeUpdateSourceModeName,modename);
			if( modename.GetItemCount() == 0 )
			{
				//�R�s�[�����[�h������̏ꍇ�i�����j���[�̍ŏ��̍��ڂ��I������Ă����ꍇ�j�A�������[�h���̃��[�h���R�s�[���ɂ���
				GetModeRawName(modename);
			}
			//�R�s�[���t�H���_�擾
			AFileAcc	appModeFolder;
			appModeFolder.SpecifyChild(appModeRootFolder,modename);
			//�R�s�[���t�H���_�����݂��Ă���΁A�����autoupdate�t�H���_�ɃR�s�[����
			if( appModeFolder.Exist() == true )
			{
				CopyToAutoUpdateFolder(appModeFolder);
			}
			//�R�s�[���t�H���_�����݂��Ă��Ȃ���΁Aautoupdate�t�H���_�͍폜����
			else
			{
				AFileAcc	dstAutoupdateFolder;
				GetAutoUpdateFolder(dstAutoupdateFolder);
				dstAutoupdateFolder.DeleteFileOrFolderRecursive();
			}
			//����̃A�v���̍X�V�������L��
			SetData_Text(kAutoUpdateFromAppDateTimeText,thisAppModifiedDateTimeText);
		}
	}
	*/
	
	//#427 
	//==================�����X�V�f�[�^�ǂݍ���==================
	//------------------autoupdate/data/ModePreferences.mi�t�@�C������autoupdateDB�֓ǂݍ���------------------
	AFileAcc	autoUpdateFolder;
	GetAutoUpdateFolder(autoUpdateFolder);
	AFileAcc	autoUpdateModePrefFile;
	autoUpdateModePrefFile.SpecifyChild(autoUpdateFolder,"data/ModePreferences.mi");
	if( autoUpdateModePrefFile.Exist() == true )
	{
		//�����X�VModePrefDB�擾
		AModePrefDB	autoupdateDB(this);
		autoupdateDB.LoadFromPrefTextFile(autoUpdateModePrefFile);
		//------------------autoupdate_userdata/ModePreferences.mi����autoupdateUserDataDB�֓ǂݍ���------------------
		//�����X�VModePrefDB userdata�擾 #624
		AFileAcc	autoUpdateUserDataModePrefFile;
		GetAutoUpdateUserDataFolder("ModePreferences.mi",autoUpdateUserDataModePrefFile);
		ABool	userDataDBExist = autoUpdateUserDataModePrefFile.Exist();
		AModePrefDB	autoupdateUserDataDB(this);
		if( userDataDBExist == true )
		{
			autoupdateUserDataDB.LoadFromPrefTextFile(autoUpdateUserDataModePrefFile);
		}
		/*#844
		//keywords.txt�ǂݍ���
		AFileAcc	keywordTextFile;
		keywordTextFile.SpecifyChild(autoUpdateFolder,"data/keywords.txt");
		AText	keywordText;
		keywordTextFile.ReadTo(keywordText);
		if( keywordText.GetItemCount() > 0 )
		{
			//LoadLegacyKeywordTextIntoEmptyCatgory()�͋�̃J�e�S���ɑ΂��Ă̂݁Akeyword.txt����L�[���[�h��ǂݍ���
			LoadLegacyKeywordTextIntoEmptyCatgory(keywordText,autoupdateDB);
		}
		*/
		
		//���o���f�[�^�}�[�W
		//#844 MergeAutoUpdate_JumpSetup(autoupdateDB,userDataDBExist,autoupdateUserDataDB);//#624
		
		//�L�[���[�h�J�e�S���[�f�[�^�}�[�W
		MergeAutoUpdate_KeywordCategory(autoupdateDB,userDataDBExist,autoupdateUserDataDB);//#624
	}
	
	//========================�L�[���[�h�����t�@�C���ǂݍ��� #427========================
	LoadKeywordsExplanation();
	
	//==================�f�[�^�␳==================
	//�g�p�o���Ȃ��t�H���g�̏ꍇ��default�t�H���g�ɕ␳����
	AdjustFontName(kDefaultFontName);
	
	//�C���f���g�^�C�v�ݒ�̕␳�i�ǂꂩ�P������true�ɂ���B2.1�ł͏����I��SDF��RegExp���O�s �Ƃ����D�揇�������̂ŁA����ɍ��킹�� #912
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
	//����R�[�h�F���ݒ�̏ꍇ�i�o�[�W����3.0.0b9�ȑO�̃f�[�^�̏ꍇ�j�́A�w�i�F�ɕ����F��30%�Ńu�����h�����F��ݒ�
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
	//C���ꕶ�@�F�������f�[�^����ϊ�
	//kUseBuiltinSyntaxDefinitionFile�����ݒ�i���[�h�t�@�C���ɍ��ږ����j�ŁAkUseCSyntaxColor��true�Ȃ�C++�r���g�C�����@��`�t�@�C����ݒ�
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
	
	//2.1.8a7�`2.1.10�̃o�[�W�����̃t�H���g���ǂݍ���
	//������O�̓t�H���g�ԍ��Ȃ̂ŁAv3�ł͈����p���Ή����Ȃ��B
	//#375
	if( IsStillDefault(kDefaultFontName) == true && IsStillDefault(kDefaultFontNameOld) == false )
	{
		AText	fontname;
		GetData_Text(kDefaultFontNameOld,fontname);
		SetData_Text(kDefaultFontName,fontname);
	}
	
	//Flexible�^�u�ʒu�ݒ�parse #1421
	UpdateFlexibleTabPositions();
	
	//==================
	//modepref.mi/keywords.txt�ǂݍ��݊����t���OON
	mDBLoaded = true;//#852
}

//#427
/**
���o���ݒ�ɂ��ă��[�h���������X�V���ڂƃ��[�U�[�쐬���ڂ��}�[�W����
*/
/*#844
void	AModePrefDB::MergeAutoUpdate_JumpSetup( AModePrefDB& inAutoUpdateDB, 
			const ABool inAutoUpdateUserDataDBExist, const AModePrefDB& inAutoUpdateUserDataDB )
{
	// ========== AutoUpdateUserDataDB����AutoUpdateDB�ցA���[�U�[���ݒ肵���F�f�[�^���R�s�[���� ========== 
	if( inAutoUpdateUserDataDBExist == true )
	{
		for( AIndex autoUpdateDBIndex = 0; 
					autoUpdateDBIndex < inAutoUpdateDB.GetItemCount_Array(kJumpSetup_Name); 
					autoUpdateDBIndex++ )
		{
			//UserDataDB������ݒ薼�������A���AAutoUpdate�t���OON���ڂ�����
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
				//�ݒ薼���������ڂ֐F�f�[�^�������㏑��
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
	
	// ========== �{DB�̂ق���AutoUpdate�t���O�t���̍��ڂ�S�폜���� ========== 
	for( AIndex thisDBIndex = GetItemCount_Array(kJumpSetup_Name)-1; 
				thisDBIndex >= 0; 
				thisDBIndex-- )
	{
		if( GetData_BoolArray(kJumpSetup_AutoUpdate,thisDBIndex) == true )
		{
			//AutoUpdate�t���OON�Ȃ�폜����
			DeleteRow_Table(kJumpSetup_Name,thisDBIndex);
		}
	}
	
	// ========== �L�[���[�h�̗L�������؂�ւ���Enable�̏ꍇ�̂݁AAutoUpdate���ڂ�{DB�փR�s�[���� ========== 
	if( GetData_Bool(AModePrefDB::kEnableAutoUpdateJumpSetup) == true )
	{
		//AutoUpdateDB����A�{DB�փf�[�^�R�s�[���AAutoUpdate�t���O��ON�ɂ���
		for( AIndex autoUpdateDBIndex = 0; 
					autoUpdateDBIndex < inAutoUpdateDB.GetItemCount_Array(kJumpSetup_Name); 
					autoUpdateDBIndex++ )
		{
			AIndex	thisDBIndex = autoUpdateDBIndex;//AutoUpdateDB�Ɠ���index�ɑ}��
			//�{DB�֍��ڒǉ��iAutoUpdateDB����R�s�[�j
			InsertRowFromOtherDB_Table(kJumpSetup_Name,thisDBIndex,inAutoUpdateDB,autoUpdateDBIndex);
			//AutoUpdate�t���OON
			SetData_BoolArray(kJumpSetup_AutoUpdate,thisDBIndex,true);
		}
	}
}
*/

//#427
/**
�L�[���[�h�ɂ��ă��[�h���������X�V���ڂƃ��[�U�[�쐬���ڂ��}�[�W����
*/
void	AModePrefDB::MergeAutoUpdate_KeywordCategory( AModePrefDB& inAutoUpdateDB,
			const ABool inAutoUpdateUserDataDBExist, const AModePrefDB& inAutoUpdateUserDataDB )//#624
{
	// ========== AutoUpdateUserDataDB����AutoUpdateDB�ցA���[�U�[���ݒ肵���F�f�[�^���R�s�[���� ========== 
	if( inAutoUpdateUserDataDBExist == true )
	{
		//AutoUpdateDB���̊e�J�e�S�����̃��[�v
		for( AIndex autoUpdateDBIndex = 0; 
					autoUpdateDBIndex < inAutoUpdateDB.GetItemCount_Array(kAdditionalCategory_Name); 
					autoUpdateDBIndex++ )
		{
			//UserDataDB������ݒ薼�������A���AAutoUpdate�t���OON���ڂ�����
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
				//�ݒ薼���������ڂ֐F�f�[�^���������㏑��
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
	
	// ========== �{DB�̂ق���AutoUpdate�t���O�t���̍��ڂ�S�폜���� ========== 
	for( AIndex thisDBIndex = GetItemCount_Array(kAdditionalCategory_Name)-1; 
				thisDBIndex >= 0; 
				thisDBIndex-- )
	{
		if( GetData_BoolArray(kAdditionalCategory_AutoUpdate,thisDBIndex) == true )
		{
			//AutoUpdate�t���OON�Ȃ�폜����
			DeleteRow_Table(kAdditionalCategory_Name,thisDBIndex);
		}
	}
	
	// ========== AutoUpdate���ڂ�{DB�փR�s�[���� ========== 
	//#844 if( GetData_Bool(AModePrefDB::kEnableAutoUpdateKeyword) == true )
	{
		//AutoUpdateDB����A�{DB�փf�[�^�R�s�[���AAutoUpdate�t���O��ON�ɂ���
		for( AIndex autoUpdateDBIndex = 0; 
					autoUpdateDBIndex < inAutoUpdateDB.GetItemCount_Array(kAdditionalCategory_Name); 
					autoUpdateDBIndex++ )
		{
			AIndex	thisDBIndex = autoUpdateDBIndex;//AutoUpdateDB�Ɠ���index�ɑ}��
			//�{DB�֍��ڒǉ��iAutoUpdateDB����R�s�[�j
			InsertRowFromOtherDB_Table(kAdditionalCategory_Name,thisDBIndex,inAutoUpdateDB,autoUpdateDBIndex);
			//AutoUpdate�t���OON
			SetData_BoolArray(kAdditionalCategory_AutoUpdate,thisDBIndex,true);
		}
	}
}

#pragma mark ===========================

#pragma mark ---���[�h�^�ۑ�

/**
���[�h�i�g�p���ɕK�v�ȃf�[�^�̃��[�h�j
@note inInitialLoad��SPI_GetModePrefDB() or InitModePref()�ȊO����̃R�[���i�������[�h�j��false�ɂ���B�܂��Afalse�̏ꍇ�A�X���b�h�Z�[�t�ł͂Ȃ��B
inInitialLoad=true�F���񃍁[�h�p�B���̏ꍇ�AImportFileRecognizer�X���b�h���̒�~�����͍s��Ȃ��B�X���b�h������R�[���B
inInitialLoad=false�F�����[�h�p�BImportFileRecognizer�X���b�h���̒�~�������s���Ă���A���[�h�������s���B�X���b�h����̃R�[���͕s�B
*/
void	AModePrefDB::LoadOrWaitLoadComplete( const ABool inInitialLoad )
{
	//�X���b�h��~����
	if( inInitialLoad == false )
	{
		//==================�����[�h���p�����i�X���b�h�Z�[�t�łȂ��j==================
		
		//#725
		//�e�T�u�E�C���h�E�̃��[�h�֘A�f�[�^���N���A����
		GetApp().SPI_ClearSubWindowsModeRelatedData(mModeID);
		
		//���[�h����v����h�L�������g���̃��[�J�����ʎq�f�[�^���폜
		for( AObjectID docID = GetApp().NVI_GetFirstDocumentID(kDocumentType_Text); 
					docID != kObjectID_Invalid; 
					docID = GetApp().NVI_GetNextDocumentID(docID) )
		{
			if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == mModeID )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_DeleteAllLocalIdentifiers();
			}
		}
		
		//ImportFileRecognizer�X���b�h��~
		GetApp().SPI_GetImportFileManager().ClearRecognizedDataAndStopRecognizer(mModeID);
		
		//���@�F���X���b�h��~
		GetApp().SPI_StopSyntaxRecognizerThreadAll(mModeID);
		
		//MultiFileFinder�̕��@�F����~
		GetApp().SPI_GetMultiFileFinder().SPI_AbortRecognizeSyntax();
	}
	
	{
		//���񃍁[�h���p�̔r������B���̐������Q�ƁB�i�����[�h�́A�X���b�h����̓R�[������Ȃ��j
		AStMutexLocker	locker(mLoadMutex);//#890
		
		//���̊֐������s���ɁA�ʃX���b�h����mode data�ւ̃A�N�Z�X�����������ꍇ�A�i�����񃍁[�h�������ɔ��������ꍇ�j
		//�P�D���[�h�ς݃t���OOFF�ƂȂ��Ă���̂ŁA���̕ʃX���b�h��������̊֐����R�[�������B
		//�Q�DmLoadMutex�ɂ��r�����䂳���̂ŁA���[�h�����܂ŏ��locker�ő҂������B
		//�R�D���̔���ɂ�胍�[�h�ς݂Ɣ��肳���̂ŁA�����Ƀ��^�[������B
		if( mLoaded == true && inInitialLoad == true )
		{
			return;
		}
		
		//#852
		//DB�̃��[�h
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
		//2.1.10�ȑO�̃o�[�W��������̃f�[�^�����p���p
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
		
		//2.1.8a6�ȑO�̃o�[�W��������̃f�[�^�����p���p
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
		//#207 �ۑ����f�t�H���g�g���q�̃f�t�H���g�ݒ�
		if( IsStillDefault(kDefaultSuffixForSaveText) == true )
		{
			if( GetItemCount_Array(kSuffix) > 0 )
			{
				//�ŏ��̊g���q�擾
				AText	text;
				GetData_TextArray(kSuffix,0,text);
				//�ݒ�
				SetData_Text(kDefaultSuffixForSaveText,text);
			}
		}
		*/
		
		//========================�e�f�[�^�̍X�V========================
		
		//�J���[�X�L�[��DB�̓ǂݍ���
		//#1316 UpdateColorScheme();
		
		//�A���t�@�x�b�g�e�[�u���X�V
		UpdateIsAlphabetTable();
		
		//���o���f�[�^�X�V�i���[�h�ݒ��ʕύX���ɂ��璼�ڃf�[�^�������݂��Ȃ��悤�ɁA���@�F�������͕ϐ����g���čs���Ă���j #890
		UpdateJumpSetup();
		
		//���@��`�ݒ�
		UpdateSyntaxDefinition();
		
		//�L�[���[�h���X�g�쐬
		MakeCategoryArrayAndKeywordList();
		
		//���@�F���F�e�[�u���X�V
		UpdateSyntaxDefinitionStateColorArray();
		
		//#382 �f�t�H���g�L�[�o�C���h�Ɠ����L�[�o�C���h�ݒ���폜����
		//#912 RemoveKeyBindsSameAsDefault();
		
		//�w�i�摜�ǂݍ���
		UpdateBackgroundImage();
		
		//���[�h�ς݃t���OON
		mLoaded = true;
	}
	
	//========================�V�X�e���w�b�_�ǂݍ���======================== #253
	UpdateSystemHeaderFileData();
	
	//========================�c�[���ǂݍ���========================
	LoadTool();
	
	//========================�c�[���o�[�ǂݍ���========================
	LoadToolbar();
	
	//==================�v���O�C���ǂݍ���==================
	LoadPlugins();
	
	//==================�v���r���[�v���O�C���ǂݍ���==================
	LoadPreviewPlugin();
	
	if( inInitialLoad == false )
	{
		//==================�����[�h���p�����i�X���b�h�Z�[�t�łȂ��j==================
		
		//#683 �e�h�L�������g��TextInfo����RegExp�I�u�W�F�N�g���X�V����
		//�i�������[�h���́A�܂�TextInfo��SetMode�����h�L�������g���Ȃ��̂ŁA�R�[������K�v�������j
		GetApp().SPI_UpdateRegExpForAllDocuments(mModeID);
		
		//�C���|�[�g�t�@�C���̔F�����ĊJ
		GetApp().SPI_GetImportFileManager().RestartRecognizer(mModeID);
		
		//���̃��[�h�̃h�L�������g�̕��@�F�����ēx�s��
		GetApp().SPI_ReRecognizeSyntaxAll(mModeID);
	}
}

/**
���@��`�̍X�V
*/
void	AModePrefDB::UpdateSyntaxDefinition()
{
	//���@��`�I�u�W�F�N�g�̃f�[�^��S�폜
	mSyntaxDefinition.DeleteAll();
	
	//SDF�t�@�C������ݒ�
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
			//��΃p�X�Ńg���C
			sdfFile.Specify(path);
			if( sdfFile.Exist() == false )
			{
				//autoupdate�t�H���_����̑��΃p�X�Ńg���C#427
				AFileAcc	autoupdateFolder;
				GetAutoUpdateFolder(autoupdateFolder);
				sdfFile.SpecifyChild(autoupdateFolder,path);
				if( sdfFile.Exist() == false )
				{
					//���[�h�t�H���_����̑��΃p�X�Ńg���C
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
			//SDF�t�@�C�������݂��Ȃ��ꍇ�A�_�~�[��syntaxDef���쐬
			AText	t;
			mSyntaxDefinition.MakeSimpleDefinition(t,t,t,t,t,t,t,t,false,t,t);
			//#907
			mSDFIsSimpleDefinition = true;
		}
	}
	//�ȈՕ��@�ݒ肩��ϊ����Đݒ�
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
		//�u���b�N�J�n�A�I�����K�\���擾 #972
		AText	incIndentRegExp, decIndentRegExp;
		GetData_Text(kIncIndentRegExp,incIndentRegExp);
		GetData_Text(kDecIndentRegExp,decIndentRegExp);
		//���@��`�ݒ�
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
	
	//SDF�ɂĎ����C���f���gdisable�̏ꍇ�́A�֘A�ݒ�������I��false�ɂ��� #997 Python�̏ꍇ�Ɏ����C���f���g���֎~���邽�߁iPython�͎蓮�C���f���g�O��Ȃ̂Ŏ����C���f���g����Ǝg���Â炢�j
	if( GetDisableAutoIndent() == true )
	{
		SetData_Bool(kUseSyntaxDefinitionIndent,false);
		SetData_Bool(kIndentWhenPaste,false);
	}
}

//#253
/**
�V�X�e���w�b�_�t�@�C��
*/
void	AModePrefDB::UpdateSystemHeaderFileData()
{
	//�V�X�e���w�b�_�t�@�C���f�[�^�S�폜
	DeleteAllImportFileData();
	//�V�X�e���w�b�_�t�@�C���ݒ�ɋL�q���ꂽ�e�t�H���_��ǂݍ���
	if( GetData_Bool(AModePrefDB::kImportSystemHeaderFiles) == true )
	{
		AText	text;
		GetData_Text(AModePrefDB::kSystemHeaderFiles,text);
		//�L�[���[�h�t�@�C��(.import)�Ŏw�肵���w�b�_�t�@�C���t�H���_��ǉ� #1065
		text.Add(kUChar_LF);
		text.AddText(mImportFilePathsByKeywordCategory);
		//
		for( AIndex pos = 0; pos < text.GetItemCount();  )
		{
			AText	line;
			text.GetLine(pos,line);
			AFileAcc	folder;
			folder.Specify(line);
			folder.ResolveAlias();//�G�C���A�X���� #1065
			if( folder.Exist() == true )//#1065
			{
				if( folder.IsFolder() == true )//���T�u�t�H���_�͌������Ă��Ȃ��H
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
							//�C���|�[�g�����t�@�C���̃p�X���L��
							AText	path;
							file.GetNormalizedPath(path);
							mSystemHeaderFileArray_NormalizedPath.Add(path);
						}
					}
				}
				else
				{
					mSystemHeaderFileArray.GetObject(mSystemHeaderFileArray.AddNewObject()).CopyFrom(folder);
					//�C���|�[�g�����t�@�C���̃p�X���L��
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
			//�V�X�e���w�b�_�t�@�C���ւ̃����N�f�[�^�擾�i����͂�����Ή�͗v���j
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
�w�b�_�t�@�C���̉�͊����ʒm�BAImportFileManager����ʒm�����B
*/
void	AModePrefDB::DoImportFileRecognized()
{
	UpdateSystemHeaderFileData();
}

//#253
/**
�w��w�b�_�t�@�C���ɑΉ����鎯�ʂ������N�f�[�^��S�č폜����B
�w�b�_�t�@�C���ۑ�����AImportFileManager::DoFileSaved()����R�[�������B
*/
ABool	AModePrefDB::DeleteImportFileData( const AObjectID inImportFileDataObjectID, const AFileAcc& inFile )
{
	//mutex lock #717
	AStMutexLocker	locker(mSystemHeaderIdentifierLinkListMutex);
	
	//inImportFileDataObjectID�ɑΉ����鎯�ʎq�����N�f�[�^���폜
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

//�w�i�摜�ǂݍ���
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
�t�@�C���֕ۑ��i�ΏہFModePreferences.mi�j
*/
void	AModePrefDB::SaveToFile()
{
	//#427 if( IsLoaded() == false )   return;
	// ModePreferences.mi�̓ǂݍ��ݍς݃`�F�b�N�i���ǂݍ��ݕۑ��h�~�j #427
	if( mDBLoaded == false )//#852
	{
		_ACThrow("mode SaveToFile() before loaded.",this);
	}
	
	// --------------- modepref.mi�̕ۑ� ---------------
	/*#688
#if IMPLEMENTATION_FOR_MACOSX
	//win
	//���f�[�^�փR�s�[
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
	//�t�@�C����������
#if IMPLEMENTATION_FOR_MACOSX
	WriteToPrefFile(mModeFile,'MMKE');
#else
	WriteToPrefTextFile(mModeFile);
#endif
	*/
	
	//#624
	// ========== UserDataDB�Ƃ��ĕۑ� ========== 
	//�����X�V���ڂ��܂߂đS�f�[�^�����̂܂�autoupdate_userdata�t�H���_�ɕۑ��B
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
	// ========== JumpSetup��AutoUpdate�t���O�t���̍��ڂ�S�폜���� ========== 
	for( AIndex thisDBIndex = GetItemCount_Array(kJumpSetup_Name)-1; 
				thisDBIndex >= 0; 
				thisDBIndex-- )
	{
		if( GetData_BoolArray(kJumpSetup_AutoUpdate,thisDBIndex) == true )
		{
			//AutoUpdate�t���OON�Ȃ�폜����
			DeleteRow_Table(kJumpSetup_Name,thisDBIndex);
		}
	}
	*/
	// ========== KeywordCategory��AutoUpdate�t���O�t���̍��ڂ�S�폜���� ========== 
	for( AIndex thisDBIndex = GetItemCount_Array(kAdditionalCategory_Name)-1; 
				thisDBIndex >= 0; 
				thisDBIndex-- )
	{
		if( GetData_BoolArray(kAdditionalCategory_AutoUpdate,thisDBIndex) == true )
		{
			//AutoUpdate�t���OON�Ȃ�폜����
			DeleteRow_Table(kAdditionalCategory_Name,thisDBIndex);
		}
	}
	
	// ========== data�t�H���_��ModePrefDB��ۑ� ========== 
	// JumpSetup, Keyword�̎����X�V���ڂ͍폜����Ă���B
	
	//data�t�H���_�擾�E����
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	dataFolder.CreateFolder();
	
	//ModePref.mi��������
	AFileAcc	prefFile;
	prefFile.SpecifyChild(dataFolder,"ModePreferences.mi");
	prefFile.CreateFile();
	WriteToPrefTextFile(prefFile);
	
	//#624
	// ========== UserDataDB����ǂݍ��݁i��ō폜�������̕����̂��߁j ========== 
	LoadFromPrefTextFile(userDataDBFile);
	
	//#844
#if 0
	// --------------- keywords.txt�̕ۑ� ---------------
	
	/*#350
	//�L�[���[�h�������݁i�b��j
	WriteAdditionalKeywords();
	*/
	//�L�[���[�h�ݒ�f�[�^�쐬
	AText	keywordDataText;
	MakeUserKeywordDataText(keywordDataText);
	//Keyword.txt��������
	AFileAcc	keywordFile;
	keywordFile.SpecifyChild(dataFolder,"keywords.txt");
	keywordFile.CreateFile();
	keywordFile.WriteFile(keywordDataText);
	
#if IMPLEMENTATION_FOR_MACOSX
	//���`���f�[�^��������
	WriteToPrefFile(mModeFile,'MMKE');//#350
	mModeFile.WriteResouceFile(keywordDataText,'MMKE',2);//#350
	//WriteMacLegacy()�p��mLegacyKeywordTempDataArray���쐬 #427
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
	//suffix.mi�ۑ�
	SaveSuffixDB();
}

//#852
/**
suffix.mi�ۑ��B�N���������̂��߁A�K�v�ŏ����̃f�[�^��ۑ��B�N�����̓ǂݍ��݂͂��̃t�@�C����ǂݍ��ށi�S�Ẵ��[�h�Łj�B
*/
void	AModePrefDB::SaveSuffixDB()
{
	ADataBase	suffixDB;
	//�f�[�^����
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
	//�f�[�^���R�s�[
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
	//suffix.mi�t�@�C��
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	AFileAcc	suffixPrefFile;
	suffixPrefFile.SpecifyChild(dataFolder,"suffix.mi");
	suffixPrefFile.CreateFile();
	//��������
	suffixDB.WriteToPrefTextFile(suffixPrefFile);
}

/*#350
//MMKE,2�ɃL�[���[�h��������
//InfoText�Ή�����ɂ́A���[�h�ݒ�_�C�A���O����ύX����K�v������̂ŁA�b���
void	AModePrefDB::WriteAdditionalKeywords()
{
#if IMPLEMENTATION_FOR_MACOSX
	AText	text;
	for( AIndex i = 0; i < mAdditionalCategoryArray_KeywordArray.GetItemCount(); i++ )
	{
		AText	categoryName;
		GetData_TextArray(kAdditionalCategory_Name,i,categoryName);
		//R0068 kAdditionalCategory_Name�̂ق���ϊ����Ȃ���ΈӖ��Ȃ�categoryName.ReplaceChar('[','(');
		//R0068 kAdditionalCategory_Name�̂ق���ϊ����Ȃ���ΈӖ��Ȃ�categoryName.ReplaceChar(']',')');
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
�L�[���[�h�ۑ��f�[�^�쐬
*/
/*#844
void	AModePrefDB::MakeUserKeywordDataText( AText& outDataText ) const
{
	outDataText.DeleteAll();
	for( AIndex categoryIndex = 0; 
				categoryIndex < GetItemCount_Array(kAdditionalCategory_Name);//#427 mAdditionalCategoryArray_KeywordArray.GetItemCount(); 
				categoryIndex++ )
	{
		//#427 �����X�V�L�[���[�h��keyword.txt�ɏ����o���Ȃ��i�����[�U�[�ҏW���̂ݍ쐬����j
		if( GetData_BoolArray(kAdditionalCategory_AutoUpdate,categoryIndex) == true )
		{
			continue;
		}
		//#427 �L�[���[�h�e�L�X�g��textarray�ɓW�J
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
//MMKE,2����L�[���[�h�ǂݍ���
//�b��
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

/*#427 ModeBackup()�ɓ���
//R0000
void	AModePrefDB::SaveModeBackup()
{
#if IMPLEMENTATION_FOR_MACOSX
	//���݂̃f�[�^��S�ăt�@�C���ۑ�
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
		//�o�b�N�A�b�v�t�@�C�����݂��Ȃ�
		return false;
	}
	
	//�C���|�[�g�t�@�C���F���X���b�h���~���� #0
	//GetApp().SPI_WaitStopImportFileRecognizer(mModeID);
	
	//
	AFileAcc	modefile;
	GetModeFile(modefile);
	//���݂̃f�[�^��S�ăt�@�C���ۑ����ăS�~���ֈړ�
	SaveToFile();
	AFileWrapper::MoveToTrash(modefile);
	
	//�o�b�N�A�b�v�t�@�C�����R�s�[
	backupfile.CopyFileTo(modefile,true);
	
	//#350
	AText	backupfilepath;
	backupfile.GetPath(backupfilepath);
	//modepref.mi��Revert
	AFileAcc	srcPrefFile;
	srcPrefFile.SpecifySister(modefile,"data/modepref.mi");
	AFileAcc	dstPrefFile;
	AText	backupfilepath_pref(backupfilepath);
	backupfilepath_pref.AddCstring("_modepref");
	dstPrefFile.Specify(backupfilepath_pref);
	AFileWrapper::MoveToTrash(srcPrefFile);
	dstPrefFile.CopyFileTo(srcPrefFile,true);
	//keywords.txt���o�b�N�A�b�v
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
�O���[�h�f�[�^�폜
*/
void	AModePrefDB::DeleteAllModeData()
{
	//mKeywordList�S�폜
	{
		//#717
		AStMutexLocker	locker(mKeywordListMutex);
		//�S���ʎq�폜
		mKeywordList.DeleteAllIdentifiers();//#641
		//mUserKeywordIdentifierUniqIDArray.DeleteAll();//#890
	}
	//�V�X�e���w�b�_�t�@�C���f�[�^�S�폜
	DeleteAllImportFileData();
	//�J�e�S���f�[�^�S�폜
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
		mCategoryDisabledStateArray.DeleteAll();//R0000 �J�e�S���[��
		mCategoryDisabledStateArrayAllFalse = true;//R0000 �J�e�S���[��
	}
	mAdditionalCategoryStartIndex = 0;
	//���K�\���L�[���[�h�S�폜
	mKeywordRegExp_Text.DeleteAll();
	mKeywordRegExp_CategoryIndex.DeleteAll();
	//�����f�[�^�z��S�폜
	mKeywordExplanationTextArray_Keyword.DeleteAll();
	mKeywordExplanationTextArray_CategoryName.DeleteAll();
	mKeywordExplanationTextArray_Explanation.DeleteAll();
	//
	//#427 mAdditionalCategoryArray_KeywordArray.DeleteAll();
	mLegacyKeywordTempDataArray.DeleteAll();//#427
	//���@��`�F�f�[�^�S�폜
	mSyntaxDefinitionStateColorArray.DeleteAll();
	mSyntaxDefinitionStateColorArray_Dark.DeleteAll();//#1316
	mSyntaxDefinitionStateColorValidArray.DeleteAll();
	mSyntaxDefinitionStateArray_TextStyle.DeleteAll();//#844
	//suffix�n�b�V���S�폜
	{
		AStMutexLocker	locker(mSuffixHashArrayMutex);
		mSuffixHashArray.DeleteAll();
	}
	//���@��`�f�[�^�S�폜
	mSyntaxDefinition.DeleteAll();
	//�w�i�摜index�폜
	mBackgroundImageIndex = kIndex_Invalid;
	//���o���ݒ�o�b�t�@�폜
	DeleteJumpSetupAll();
	
	//DB�̃f�[�^��S�ăf�t�H���g�ɐݒ肷��
	SetAllDataToDefault();
}

//#427
/**
���[�h�����X�V

autoupdate�t�H���_�̒u�������ƁA���[�h�f�[�^�ēǍ����s��
*/
void	AModePrefDB::AutoUpdate( const AFileAcc& inSrcModeFolder )
{
	//���[�h�ݒ�E�C���h�E�̎����X�V�J�n������
	if( GetApp().SPI_GetModePrefWindow(mModeID).SPI_StartAutoUpdate() == false )   return;
	
	//���[�h�t�@�C���ۑ�
	SaveToFile();
	
	//autoupdate�t�H���_�փR�s�[
	CopyToAutoUpdateFolder(inSrcModeFolder);
	
	//���[�h�ݒ�ēǍ�
	LoadOrWaitLoadComplete();
	
	//�S�Ẵh�L�������g�ɂ��āA���[�h�̍Đݒ���s��
	GetApp().SPI_ReSetModeAll(mModeID);
	
	//revision�ݒ�imLoadingRevision��AApp::ModeUpdate_ResultRevisionCheck()�Őݒ肳���j
	AText	revisionText;
	revisionText.SetNumber64bit(mLoadingRevision);
	SetData_Text(AModePrefDB::kAutoUpdateRevisionText,revisionText);
	
	//�X�V�����ݒ�
	AText	datetime;
	ADateTimeWrapper::GetShortDateText(datetime);
	datetime.AddCstring(" ");
	AText	t;
	ADateTimeWrapper::GetTimeText(t);
	datetime.AddText(t);
	SetData_Text(AModePrefDB::kLastAutoUpdateDateTime,datetime);
	
	//���[�h�ݒ�E�C���h�E�̎����X�V�I��������
	GetApp().SPI_GetModePrefWindow(mModeID).SPI_EndAutoUpdate();
}

/**
�w��t�H���_����autoupdate�t�H���_�փR�s�[
*/
void	AModePrefDB::CopyToAutoUpdateFolder( const AFileAcc& inSrcModeFolder )
{
	//autoupdate�t�H���_�X�V
	//���݂�autoupdate�t�H���_��S�폜
	AFileAcc	dstAutoupdateFolder;
	GetAutoUpdateFolder(dstAutoupdateFolder);
	if( dstAutoupdateFolder.Exist() == true )
	{
		dstAutoupdateFolder.DeleteFileOrFolderRecursive();
	}
	//�w��t�H���_���܂邲��autoupdate�t�H���_�փR�s�[
	dstAutoupdateFolder.CreateFolderRecursive();
	inSrcModeFolder.CopyFolderTo(dstAutoupdateFolder,false,false);//�㏑���A�t�H���_�����łɑ��݂��Ă��Ă��ǉ��R�s�[
}

//#427
/**
���[�h�����X�V�p�z�z�f�[�^����
*/
void	AModePrefDB::CreateModeUpdateDistribution( const AFileAcc& inFolder )
{
	//���[�h�t�@�C�����擾
	AText	modefilename;
	GetModeRawName(modefilename);
	//���[�h�t�H���_�擾
	AFileAcc	modeFolder;
	GetModeFolder(modeFolder);
	//�z�z�f�[�^�쐬�p�e���|�����t�H���_����
	AFileAcc	tempFolder;
	GetApp().SPI_GetTempFolder(tempFolder);
	AFileAcc	tempWorkFolder;
	tempWorkFolder.SpecifyUniqChildFile(tempFolder,"modeUpdateDistribution");
	tempWorkFolder.CreateFolderRecursive();
	AFileAcc	tempModeFolder;
	tempModeFolder.SpecifyChild(tempWorkFolder,modefilename);
	//���[�h�t�H���_�̓��e��S�ăe���|�����t�H���_�փR�s�[
	modeFolder.CopyFolderTo(tempModeFolder,true,true);
	//�e���|�����t�H���_����autoupdate, autoupdate_userdata�t�H���_���폜
	AFileAcc	tempModeAutoUpdateFolder;
	tempModeAutoUpdateFolder.SpecifyChild(tempModeFolder,"autoupdate");
	tempModeAutoUpdateFolder.DeleteFileOrFolderRecursive();
	AFileAcc	tempModeAutoUpdateFolderUserData;
	tempModeAutoUpdateFolderUserData.SpecifyChild(tempModeFolder,"autoupdate_userdata");
	tempModeAutoUpdateFolderUserData.DeleteFileOrFolderRecursive();
	
	//�����e�L�X�g����
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
	//update.txt�t�@�C������
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
	//zip�t�@�C������
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
���[�h�ݒ�o�b�N�A�b�v
*/
void	AModePrefDB::ModeBackup( const ABool inReveal )
{
	//modebackup�t�H���_�擾
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	AFileAcc	backupRootFolder;
	backupRootFolder.SpecifyChild(appPrefFolder,"modebackup");
	backupRootFolder.CreateFolder();
	
	//���[�h���̃t�H���_��modebackup�t�H���_�ɐ�������
	AText	modename;
	GetModeRawName(modename);
	AFileAcc	backupModeFolder;
	backupModeFolder.SpecifyChild(backupRootFolder,modename);
	backupModeFolder.CreateFolder();
	
	//��������o�b�N�A�b�v��t�H���_������
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
	
	//�t�H���_�R�s�[
	AFileAcc	modeFolder;
	GetModeFolder(modeFolder);
	modeFolder.CopyFolderTo(folder,true,true);
	
	//�t�H���_Reveal
	if( inReveal == true )
	{
		ALaunchWrapper::Reveal(folder);
	}
}

//#889
/**
�J���[�X�L�[��DB�X�V
*/
/*#1316 AColorSchemeDB::Load()�ֈړ�
void	AModePrefDB::UpdateColorScheme()
{
	//���݂̃X�L�[�����擾
	AText	schemename;
	GetModeData_Text(kColorSchemeName,schemename);
	//�t�@�C���^�C�v
	AColorSchemeType	type = kColorSchemeType_CSV;
	//csv, epf�t�@�C���̃t�@�C�����擾
	AText	csvschemename, epfschemename;
	csvschemename.SetText(schemename);
	csvschemename.AddCstring(".csv");
	epfschemename.SetText(schemename);
	epfschemename.AddCstring(".epf");
	//���[�U�[�J���[�X�L�[���t�H���_�擾
	AFileAcc	userSchemeFolder;
	GetApp().SPI_GetUserColorSchemeFolder(userSchemeFolder);
	//�J���[�X�L�[���t�@�C���擾
	AFileAcc	file;
	file.SpecifyChild(userSchemeFolder,csvschemename);
	//csv�t�@�C�������݂��Ȃ��ꍇ��epf�t�@�C��������
	if( file.Exist() == false )
	{
		file.SpecifyChild(userSchemeFolder,epfschemename);
		if( file.Exist() == true )
		{
			//epf�t�@�C�������݂��Ă�����t�@�C���^�C�v��epf�ɂ���
			type = kColorSchemeType_EPF;
		}
	}
	//���[�U�[�J���[�X�L�[���t�H���_�ɑΏۖ��̃t�@�C�������݂��Ă��Ȃ���΁A�A�v���J���[�X�L�[���t�H���_����T��
	if( file.Exist() == false )
	{
		//�A�v���P�[�V�����̃J���[�X�L�[���t�H���_�擾
		AFileAcc	appSchemeFolder;
		GetApp().SPI_GetAppColorSchemeFolder(appSchemeFolder);
		//
		file.SpecifyChild(appSchemeFolder,csvschemename);
		//�t�@�C�������݂��Ă��Ȃ���΃f�t�H���g�̃t�@�C�����擾
		if( file.Exist() == false )
		{
			file.SpecifyChild(appSchemeFolder,"mi-default.csv");
		}
	}
	//�J���[�X�L�[��DB������
	mColorSchemeDB.SetAllDataToDefault();
	//�J���[�X�L�[��DB�֓ǂݍ���
	mColorSchemeDB.LoadFromColorSchemeFile(file,type);
}
*/

//#889
/**
���݂̐F��LastOverwrittenColors�X�L�[���t�@�C���ɕۑ�
*/
void	AModePrefDB::SaveLastColors()
{
	//���[�U�[�J���[�X�L�[���t�H���_�擾
	AFileAcc	userSchemeFolder;
	GetApp().SPI_GetUserColorSchemeFolder(userSchemeFolder);
	//LastOverwrittenColors�J���[�X�L�[���t�@�C���擾
	AFileAcc	file;
	file.SpecifyChild(userSchemeFolder,"LastOverwrittenColors.csv");
	//�t�@�C���ɃG�N�X�|�[�g
	ExportColors(file);
	//�J���[�X�L�[�����j���[�X�V
	GetApp().SPI_UpdateColorSchemeMenu();
}

/**
�F�G�N�X�|�[�g
*/
void	AModePrefDB::ExportColors( const AFileAcc& inFile )
{
	//CSV�f�[�^����
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
���[�h���L�����ǂ������擾
*/
ABool	AModePrefDB::IsModeEnabled() const
{
	return GetData_Bool(kEnableMode);
}

#pragma mark ===========================

#pragma mark ---�f�[�^�擾�I�[�o�[���C�h

/**
�w��f�[�^���ڂɂ��āA�W�����[�h�Ɠ����ݒ�ɂ��ׂ����ǂ������擾
*/
ABool	AModePrefDB::IsSameAsNormal( const ADataID inID ) const
{
	ABool	sameAsNormal = false;
	switch(inID)
	{
		//�t�H���g
	  case kDefaultFontName:
	  case kDefaultFontSize:
		{
			if( GetData_Bool(kSameAsNormalMode_Font) == true )
			{
				sameAsNormal = true;
			}
			break;
		}
		//�F
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
		//�e�L�X�g�\��
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
		//�w�i�\��
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
		//���C�A�E�g�i���[���[�E�}�N���o�[�E�s�ԍ��\���j
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
		//�f�t�H���g�e�L�X�g�G���R�[�f�B���O�E���s�R�[�h�E�s�܂�Ԃ�
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
		//���ʑΉ��E4��N���b�N�E���L�[
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
		//�X�y���`�F�b�N
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
			//�����Ȃ�
			break;
		}
	}
	return sameAsNormal;
}

/**
�f�[�^�擾�I�[�o�[���C�h
*/
ABool	AModePrefDB::GetModeData_Bool( ADataID inID ) const
{
	//���ݒ�J���[�X�L�[�����g�p����ꍇ�́AAppPref����f�[�^���擾���� #1316
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
�f�[�^�擾�I�[�o�[���C�h
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
�f�[�^�擾�I�[�o�[���C�h
*/
ANumber	AModePrefDB::GetModeData_Number( ADataID inID ) const
{
	//���ݒ�J���[�X�L�[�����g�p����ꍇ�́AAppPref����f�[�^���擾���� #1316
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
�f�[�^�擾�I�[�o�[���C�h
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
�f�[�^�擾�I�[�o�[���C�h
*/
void	AModePrefDB::GetModeData_Color( ADataID inID, AColor& outData ) const
{
	GetModeData_Color(inID, outData, GetApp().NVI_IsDarkMode());
}
void	AModePrefDB::GetModeData_Color( ADataID inID, AColor& outData, const ABool inDarkMode ) const//#1316
{
	//���ݒ�J���[�X�L�[�����g�p����ꍇ�́AAppPref����f�[�^���擾���� #1316
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
		//==================�W�����[�h�A�܂��́A�W�����[�h�Ɠ����ݒ���g���ݒ�=NO�̏ꍇ==================
		//���̃��[�h�̃f�[�^���g��
		
		//�f�[�^�擾
		ADataBase::GetData_Color(inID,outData);
		
		/*#889 
		//�J���[�X�L�[�����g�p����ݒ�ON���A�J���[�X�L�[���f�[�^������΁A���̐F�ŏ㏑������B
		//�i���̃��[�h�̃J���[�X�L�[���j
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
		//==================�W�����[�h�ł͂Ȃ��A���A�W�����[�h�Ɠ����ݒ���g���ݒ�=YES�̏ꍇ==================
		//�W�����[�h�̃f�[�^���g��
		GetApp().SPI_GetModePrefDB(0).GetModeData_Color(inID,outData,inDarkMode);//#1316
	}
}

//#1316 
/**
�J���[�X�L�[���̐F�����[�h�ݒ�F�ɓK�p����i���[�h�ݒ�́u�J���[�X�L�[����K�p�v�{�^���N���b�N���̏����j
*/
void	AModePrefDB::ApplyFromColorScheme( const AText& inSchemeName )
{
	//AColorSchemeDB�Ƀ��[�h����
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
	//����R�[�h�����F
	if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kControlCode) == false )
	{
		//�J���[�X�L�[���ɐ���R�[�h�F�̐ݒ肪����΁A���̐F��ݒ�
		colorSchemeDB.GetColor(AColorSchemeDB::kControlCode,color);
		SetData_Color(kControlCodeColor,color);
	}
	else if( colorSchemeDB.IsStillDefault(AColorSchemeDB::kLetter) == false )
	{
		//�J���[�X�L�[���Ő���R�[�h�F���ݒ�A���A�����F�̐ݒ肪����ꍇ�́A�w�i�F�ɕ����F��30%�Ńu�����h�����F��ݒ�
		AColor	backgroundColor = kColor_White;
		GetData_Color(kBackgroundColor,backgroundColor);
		AColor	letterColor = kColor_Black;
		GetData_Color(kLetterColor,letterColor);
		AColor	controlCodeColor = AColorWrapper::GetAlpheBlend(backgroundColor,letterColor,0.3);
		SetData_Color(kControlCodeColor,controlCodeColor);
	}
}

#pragma mark ===========================

#pragma mark ---�L�[���[�h�\������

void	AModePrefDB::UpdateIsAlphabetTable()
{
	//
	//mIsContinuousAlphabet: false�Ȃ炱�̕����̒��O���P���؂�ɂȂ�
	//mIsTailAlphabet: false�Ȃ炱�̕����̒��オ�P���؂�ɂȂ�
	
	//�A���t�@�x�b�g�e�[�u���̍X�V
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

#pragma mark ---�v���r���[�v���O�C��

/**
�v���r���[�v���O�C�����[�h
*/
void	AModePrefDB::LoadPreviewPlugin()
{
	//�v���r���[�v���O�C��������
	mPreviewPluginText.DeleteAll();
	mPreviewPluginExist = false;
	//�v���r���[�v���O�C�������݂��Ă���΁A�ǂݍ���
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
�v���r���[�v���O�C���擾
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
�v���O�C�����[�h
*/
void	AModePrefDB::LoadPlugins()
{
	for( AIndex i = 0; i < 2; i++ )
	{
		//�v���O�C���t�H���_�擾
		AFileAcc	pluginsFolder;
		GetPluginsFolder((i==0),pluginsFolder);
		pluginsFolder.CreateFolder();
		//�e�v���O�C���t�@�C�����Ƃ̃��[�v
		AObjectArray<AFileAcc>	pluginFileArray;
		pluginsFolder.GetChildren(pluginFileArray);
		for( AIndex j = 0; j < pluginFileArray.GetItemCount(); j++ )
		{
			//�t�@�C���擾
			AFileAcc	file = pluginFileArray.GetObjectConst(j);
			if( file.IsFolder() == false )
			{
				//�v���O�C���e�L�X�g�ǂݍ���
				AText	pluginText;
				file.ReadTo(pluginText);
				//�v���O�C�����[�h�iJS���s�j
				GetApp().SPI_LoadPlugin(mModeID,pluginText,pluginsFolder);
			}
		}
	}
}

//#1421
/**
Flexible�^�u�ʒu�ݒ�parse
*/
void	AModePrefDB::UpdateFlexibleTabPositions()
{
	//�^�u�ʒu�z�񏉊���
	mFlexibleTabPositions.DeleteAll();
	//Flexible�^�uEnable�łȂ���ΏI��
	if( GetData_Bool(kEnableFlexibleTabPositions) == false )
	{
		return;
	}
	//�e�L�X�g�擾
	AText	tabPositionsText;
	GetData_Text(kFlexibleTabPositions, tabPositionsText);
	//CSV�I�u�W�F�N�g����
	ACSVFile	csv(tabPositionsText,ATextEncodingWrapper::GetUTF8TextEncoding(),1);
	for( AIndex i = 0; i < 256; i++ )
	{
		//��擾
		ATextArray	textArray;
		if( csv.GetColumn(i,textArray) == false )
		{
			//�񂪂Ȃ��Ȃ�����I��
			break;
		}
		//�ŏ��̍s�̐��l���擾
		ANumber	number = 0;
		if( textArray.GetItemCount() > 0 )
		{
			AText	t;
			textArray.Get(0,t);
			AIndex	p = 0;
			t.SkipTabSpace(p,t.GetItemCount());
			t.ParseIntegerNumber(p, number, false);
		}
		//�z��ɒǉ�
		mFlexibleTabPositions.Add(number);
	}
}

/**
�v���O�C���o�^�i�v���O�C����JS����R�[�������j
*/
void	AModePrefDB::RegisterPlugin( const AText& inPluginID, const AText& inPluginName, 
									 const ABool inDefaultEnabled, const AText& inDefaultOptions )
{
	//�o�^�ς݂Ȃ牽�����Ȃ�
	AIndex	index = Find_TextArray(kPluginData_Id,inPluginID);
	if( index != kIndex_Invalid )
	{
		return;
	}
	//�o�^
	Add_TextArray(kPluginData_Id,inPluginID);
	Add_TextArray(kPluginData_Name,inPluginName);
	Add_BoolArray(kPluginData_Enabled,inDefaultEnabled);
	Add_TextArray(kPluginData_Options,inDefaultOptions);
}

/**
�v���O�C���\�����o�^�i�v���O�C����JS����R�[�������j
*/
void	AModePrefDB::SetPluginDisplayName( const AText& inPluginID, const AText& inLangName, const AText& inDisplayName )
{
	//�v���O�C��ID, ���ꖼ���^�u�łȂ������̂��L�[�Ƃ���
	AText	id_lang;
	id_lang.SetText(inPluginID);
	id_lang.AddCstring("\t");
	id_lang.AddText(inLangName);
	//�o�^�ς݂Ȃ�폜
	AIndex	index = Find_TextArray(kPluginDisplayName_Id_Language,id_lang);
	if( index != kIndex_Invalid )
	{
		DeleteRow_Table(kPluginDisplayName_Id_Language,index);
	}
	//�o�^
	Add_TextArray(kPluginDisplayName_Id_Language,id_lang);
	Add_TextArray(kPluginDisplayName_DisplayName,inDisplayName);
}

/**
�v���O�C���\�����擾
*/
ABool	AModePrefDB::GetPluginDisplayName( const AText& inPluginID, AText& outDisplayName ) const
{
	//���݊��̌���擾
	AText	langName;
	GetApp().NVI_GetLanguageName(langName);
	//�v���O�C��ID, ���݌�����^�u�łȂ������̂��L�[�Ƃ���
	AText	id_lang;
	id_lang.SetText(inPluginID);
	id_lang.AddCstring("\t");
	id_lang.AddText(langName);
	//�v���O�C���\�����擾
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
�v���O�C����Enable���ǂ������擾
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
�v���O�C���I�v�V�����擾
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

#pragma mark ---�c�[���i��{����j

/*
�y�f�[�^�\���z
mToolMenuRootObjectID: ���[�g���j���[��MenuObjectID
mToolMenuArray_MenuObjectID: ���[�g���j���[����ъe�T�u���j���[��MenuObjectID���i�[�����z��

mToolItemArray_xxx: �c�[���̊e���ڂ��i�[�����񎟌��z��B�ȉ���5������B
mToolItemArray_File, mToolItemArray_SubMenuObjectID, mToolItemArray_Enabled, mToolItemArray_Shortcut, mToolItemArray_Modifiers
�O���̔z���index�́AmToolMenuArray_MenuObjectID��index�ɑΉ�����B
�����̔z���index�́A���j���[���̏��ԁB�i�������A���j���[�ɕ\������Ȃ��i�L���łȂ��j���ڂ��z����Ɋ܂܂��B�j

�y�T�v�z
LoadTool()�ɂ��A�����f�[�^���\�z�A���j���[���\������B
���j���[�̍\����UpdateToolMenu()�ɂ����s�����B���j���[�\�������MenuObjectID�͂��炩���ߌ��܂��Ă���B�L�����ǂ����̓����f�[�^���Q�Ƃ��Ȃ���\�z�����B


*/

/**
Load�i���[�g���j���[�z���S�Ă��i�ēx�j�ǂݍ��݁j
*/
void	AModePrefDB::LoadTool()
{
	//���Xrealize�ς݂������ꍇ�́Arealize����
	ABool	realized = false;
	if( mToolMenuRootObjectID != kObjectID_Invalid )
	{
		realized = GetApp().NVI_GetMenuManager().GetDynamicMenuRealizedByObjectID(mToolMenuRootObjectID);
	}
	if( realized == true )
	{
		GetApp().NVI_GetMenuManager().UnrealizeDynamicMenu(mToolMenuRootObjectID);
	}
	
	//���[�g���j���[�f�[�^����
	
	//#305 ������ړ�
	//���[�g�t�H���_�쐬
	AFileAcc	toolRootFolder;
	GetToolFolder(false,toolRootFolder);//#427 ���[�U�[��`�̂ق��̃c�[�����[�g�t�H���_
	toolRootFolder.CreateFolder();
	//
	if( mToolMenuRootObjectID != kObjectID_Invalid )
	{
		//���Ƀ��[�g���j���[�쐬�ς݂̏ꍇ�́A���[�g���j���[�̊e���ڂ��폜
		while( GetToolItemCount(mToolMenuRootObjectID) > 0 )
		{
			DeleteToolItem(mToolMenuRootObjectID,0,false,false);
		}
		UpdateToolMenu(mToolMenuRootObjectID);
	}
	else
	{
		//���[�g���j���[���쐬�̏ꍇ�́A�V�K�쐬
		AMenuItemID	menuItemID = kMenuItemID_Tool;
		if( mModeID == kStandardModeIndex )
		{
			menuItemID = kMenuItemID_Tool_StandardMode;
		}
		mToolMenuRootObjectID = GetApp().NVI_GetMenuManager().CreateDynamicMenu(menuItemID,true);
		mToolMenuArray_MenuObjectID.Add(mToolMenuRootObjectID);
		mToolMenuArray_Folder.GetObject(mToolMenuArray_Folder.AddNewObject()).CopyFrom(toolRootFolder);//#305
		mToolMenuArray_AutoUpdateFolder.Add(false);//#427 ���[�g�t�H���_�̓��[�U�[�ҏW��
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
	/*#305 ��ֈړ�
	//���[�g�t�H���_�쐬
	AFileAcc	toolRootFolder;
	GetToolFolder(toolRootFolder);
	toolRootFolder.CreateFolder();
	*/
	
	
	//���[�h�\�����擾
	AText	modeDisplayName;
	GetModeDisplayName(modeDisplayName);
	
	//���[�g�̃��[�U�[�c�[���A�A�v���g�ݍ��݃c�[���̈ʒu������
	mToolMenuRoot_UserToolStartIndex = mToolMenuRoot_UserToolEndIndex = 0;
	mToolMenuRoot_AppToolStartIndex = mToolMenuRoot_AppToolEndIndex = 0;
	
	//==================���[�U�[�c�[��==================
	//���[�U�[�c�[���\���L���Ȃ烆�[�U�[�c�[����ʏ�t�H���_���烍�[�h
	//#844 if( GetData_Bool(kEnableUserTool) == true )
	//���[�U�[�c�[���J�n�ʒu�擾
	mToolMenuRoot_UserToolStartIndex = GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mToolMenuRootObjectID);//GetToolItemCount(mToolMenuRootObjectID);
	{
		//#427 �u���[�U�[�c�[���i���[�h�F�j�v�Ƃ������j���[���ڂ�ǉ�
		AText	staticText;
		staticText.SetLocalizedText("ToolMenuText_UserTool");
		staticText.ReplaceParamText('0',modeDisplayName);
		InsertStaticTextToolItem(mToolMenuRootObjectID,GetToolItemCount(mToolMenuRootObjectID),
								 staticText,false,false,false);
		//�c�[���ǂݍ��݁i���[�U�[��`�j
		LoadToolRecursive(toolRootFolder,mToolMenuRootObjectID,false);//#427
	}
	//���[�U�[�c�[���I���ʒu�擾
	mToolMenuRoot_UserToolEndIndex = GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mToolMenuRootObjectID);//GetToolItemCount(mToolMenuRootObjectID);
	
	//��؂���ǉ�
	AText	sepatext("-");
	InsertStaticTextToolItem(mToolMenuRootObjectID,GetToolItemCount(mToolMenuRootObjectID),
							 sepatext,false,false,true);
	
	//==================�A�v�����c�[��==================
	//#427 
	//�����X�V�c�[���\���L���Ȃ玩���X�V�c�[����autoupdate�t�H���_���烍�[�h
	//#844 if( GetData_Bool(kEnableAutoUpdateTool) == true )
	//�A�v���g�ݍ��݃c�[���J�n�ʒu�擾
	mToolMenuRoot_AppToolStartIndex = GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mToolMenuRootObjectID) +1;//GetToolItemCount(mToolMenuRootObjectID);
	{
		//autoupdate�t�H���_����tool�t�H���_�擾
		AFileAcc	autoUpdateToolRootFolder;
		GetToolFolder(true,autoUpdateToolRootFolder);
		//autoupdate�t�H���_����tool�t�H���_�̑��ݔ���
		if( autoUpdateToolRootFolder.Exist() == true )
		{
			//�u�A�v�����c�[���i���[�h�F�j�v�Ƃ������j���[���ڂ�ǉ�
			AText	staticText;
			staticText.SetLocalizedText("ToolMenuText_AutoUpdateTool");
			staticText.ReplaceParamText('0',modeDisplayName);
			InsertStaticTextToolItem(mToolMenuRootObjectID,GetToolItemCount(mToolMenuRootObjectID),
									 staticText,false,false,true);
			//�����X�V�c�[�������[�h����
			LoadToolRecursive(autoUpdateToolRootFolder,mToolMenuRootObjectID,true);
		}
	}
	//�A�v���g�ݍ��݃c�[���I���ʒu�擾
	mToolMenuRoot_AppToolEndIndex = GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mToolMenuRootObjectID);//GetToolItemCount(mToolMenuRootObjectID);
	
	//���Xrealize�ς݂������ꍇ�́A���j���[realize
	if( realized == true )
	{
		GetApp().NVI_GetMenuManager().RealizeDynamicMenu(mToolMenuRootObjectID);
	}
}
//�e���j���[�i���[�g���j���[�E�T�u���j���[�j���Ƃ�Load
void	AModePrefDB::LoadToolRecursive( const AFileAcc& inFolder, const AObjectID inDynamicMenuObjectID,
		const ABool inAutoUpdate )//#427
{
	AFileAcc	toolPrefFile;
	toolPrefFile.SpecifyChild(inFolder,"toolpref.mi");
	if( toolPrefFile.Exist() == true )
	{
		//toolpref.mi�t�@�C������ǂݍ���
		
		//
		AIndex	toolItemArrayIndex = mToolMenuArray_MenuObjectID.Find(inDynamicMenuObjectID);
		if( toolItemArrayIndex == kIndex_Invalid )
		{
			_ACThrow("invalid menu objectID",NULL);
		}
		
		//toolpref.mi�t�@�C������ǂݍ���
		AToolPrefDB	toolPrefDB;
		if( inAutoUpdate == false )
		{
			//���[�U�[�ҏW�c�[���̏ꍇ
			//�c�[���t�@�C���Ɠ����t�H���_��toolpref.mi��ǂݍ���
			if( toolPrefFile.Exist() == true )
			{
				toolPrefDB.LoadFromPrefTextFile(toolPrefFile);
			}
		}
		else
		{
			//#427
			//�����X�V�c�[���̏ꍇ�Aautoupdate�t�H���_����toolpref.mi��
			//autoupdate_userdata�t�H���_���̑Ή�����t�H���_��toolpref.mi�Ƃ��}�[�W����B
			
			//�܂��Aautoupdate_userdata�t�H���_���̑Ή�����t�H���_��toolpref.mi��ǂݍ���
			//�i���݂��Ȃ���Γǂݍ��܂Ȃ��BtoolPrefDB�͋�ƂȂ�B�j
			AFileAcc	userDataToolPrefFile;
			GetToolPrefFile(toolItemArrayIndex,true,userDataToolPrefFile);
			if( userDataToolPrefFile.Exist() == true )
			{
				toolPrefDB.LoadFromPrefTextFile(userDataToolPrefFile);
			}
			//autoupdate�t�H���_����toolpref.mi��ǂݍ���
			AToolPrefDB	autoUpdateToolPrefDB;
			if( toolPrefFile.Exist() == true )
			{
				autoUpdateToolPrefDB.LoadFromPrefTextFile(toolPrefFile);
			}
			//autoupdate�t�H���_����toolpref.mi�̊e���ڂ��ƂɃ��[�v
			for( AIndex autoupdateIndex = 0; 
						autoupdateIndex < autoUpdateToolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName);
						autoupdateIndex++ )
			{
				AText	filename;
				autoUpdateToolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FileName,autoupdateIndex,filename);
				//AutoUpdate���̃c�[���Ɠ����t�@�C�����̃c�[����UserData�����猟��
				AIndex	foundIndex = toolPrefDB.Find_TextArray(AToolPrefDB::kToolArray_FileName,filename);
				if( foundIndex == kIndex_Invalid )
				{
					//UserData���ɑ��݂��Ȃ������ꍇ�AUserData�ւ��̂܂ܒǉ�����
					toolPrefDB.InsertRowFromOtherDB_Table(AToolPrefDB::kToolArray_FileName,
								toolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName),
								autoUpdateToolPrefDB,autoupdateIndex);
				}
				else
				{
					//UserData���ɑ��݂����ꍇ�A������c�[����������UserData�փR�s�[����
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
		
		//�t�H���_�̒��̃t�@�C�����擾���Achildren�֊i�[
		AObjectArray<AFileAcc>	children;
		inFolder.GetChildren(children);
		//�t�H���_�̒��̃t�@�C���ɂ��Ă̒ǉ��ς݃t���O�A�t�@�C����Hash
		AHashTextArray	childrenFilesName;
		ABoolArray	childrenFilesAddedFlag;
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			AText	filename;
			children.GetObject(i).GetFilename(filename);
			childrenFilesName.Add(filename);
			childrenFilesAddedFlag.Add(false);
		}
		
		//ToolPref�̊e���ڂ�ǉ�
		AItemCount	prefItemCount = toolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName);
		for( AIndex index = 0; index < prefItemCount; index++ )
		{
			//�c�[���ǉ�
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
			//toolpref.mi�ɋL�q���ꂽ���ڂ̃t�@�C�������݂��Ă��Ȃ��ꍇ�A
			//���̍��ڂ͖�������B
			//�i2.1.13b1���d�l�ύX�B�����X�V���A�c�[�����폜���ꂽ�ꍇ�Aautoupdate_userdata�t�H���_�̂ق���
			//���ڂ������c��\��������B���̏ꍇ�A�c�[����ǉ����ׂ��ł͂Ȃ��B
			//�܂��A���[�U�[�쐬�c�[���ɂ����Ă��A��̃c�[����ǉ����郁���b�g���Ȃ��B�j#427
			if( file.Exist() == false )//#427   file.CreateFile();
			{
				//���̍��ڂ͖������Ď���
				continue;
			}
			//�c�[���ǉ��BtoolIndex�ցA�ǉ������c�[����index������iindex�Ƃ͕K��������v���Ȃ��B
			//���[�g�̏ꍇ�͎����X�V�ƃ��[�U�[��`��A�����ē���邽�߁B�j�B
			AIndex	toolIndex = AddToolItem(inDynamicMenuObjectID,file,false,false,inAutoUpdate);//#427
			//�ݒ�
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
			//������^�C�g���ݒ�
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
			
			//ToolPref�̃t�@�C�����ɑΉ�����t�@�C�������݂�����ǉ��ς݃t���OOn
			AIndex	childrenFilesIndex = childrenFilesName.Find(filename);
			if( childrenFilesIndex != kIndex_Invalid )
			{
				childrenFilesAddedFlag.Set(childrenFilesIndex,true);
			}
		}
		
		//���ǉ��iToolPref�ɑ��݂��Ȃ����́j�̃t�@�C����ǉ�
		//Finder�⋌�o�[�W�����Ńt�@�C������ύX�����ꍇ�ɖ��ǉ��ɂȂ�B
		//�i������^�C�g�����̃f�[�^�������Ȃ����߂ɁAToolPref�̃f�[�^���������Ă����j
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			//�ǉ��ς݂͑ΏۊO
			if( childrenFilesAddedFlag.Get(i) == true )   continue;
			//�t�@�C�����擾
			AFileAcc	file = children.GetObject(i);
			AText	filename;
			file.GetFilename(filename);
			//�ΏۊO�t�@�C��
			if( filename.GetLength() == 0 )   continue;
			if( filename.Get(0) == '.' )   continue;
			if( filename.Compare("order") == true )   continue;
			if( filename.Compare("Icon\r") == true )   continue;
			if( filename.Compare("toolpref.mi") == true )   continue;
			//if( filename.Compare("_script") == true )   continue;
			//�c�[���ǉ�
			AddToolItem(inDynamicMenuObjectID,file,false,false,inAutoUpdate);//#427
		}
		
		//���j���[�X�V
		UpdateToolMenu(inDynamicMenuObjectID);
	}
	//#844 ��.order/order�t�@�C���ǂݍ��ݏ����폜
#if IMPLEMENTATION_FOR_MACOSX
	//���o�[�W�����ō쐬�������[�h�f�[�^�Ƃ̌݊����̂��߂̏����i�o�[�W����3.0.0b1�ō폜���Ă������A�����p���ł��Ȃ��Ƃ����s��񍐂��������̂ŁA��͂蕜������B#949�j
	//�����X�V�f�[�^��toolpref.mi�K�{��O��Ƃ���B�itoolpref.mi�������o�[�W�����͊��ɂ��Ȃ�Â��̂ŁB�j
	else
	{
		//���o�[�W����order�t�@�C������ǂݍ���
		
		//�t�H���_�̒��̃t�@�C�����擾���Achildren�֊i�[
		AObjectArray<AFileAcc>	children;
		inFolder.GetChildren(children);
		
		//"order"�t�@�C���ɏ]���ď��ԓ���ւ�
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
		AIndex	pos = 0;//Text��pos
		AIndex	n = 0;//���݂̏���
		while( pos < orderText.GetLength() )
		{
			//order�t�@�C������line�ւP�s�擾
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
			//children���������āAline�ƈ�v����t�@�C��������������A
			//children��n�Ԗڂ̈ʒu�ցA������ړ�����B
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
		//���j���[�̍\�z
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			//�t�@�C�����擾
			AFileAcc	file = children.GetObject(i);
			AText	filename;
			file.GetFilename(filename);
			//�ΏۊO�t�@�C��
			if( filename.GetLength() == 0 )   continue;
			if( filename.Get(0) == '.' )   continue;
			if( filename.Compare("order") == true )   continue;
			if( filename.Compare("Icon\r") == true )   continue;
			if( filename.Compare("toolpref.mi") == true )   continue;//#305
			//�c�[���ǉ�
			AddToolItem(inDynamicMenuObjectID,file,false,false,inAutoUpdate);//#427
			//R0173
			mToolItemArray_Enabled.GetObject(toolItemArrayIndex).Set(GetToolItemCount(inDynamicMenuObjectID)-1,enableArray.Get(i));
			//#129
			mToolItemArray_Grayout.GetObject(toolItemArrayIndex).Set(GetToolItemCount(inDynamicMenuObjectID)-1,grayoutArray.Get(i));
			//#305 �V���[�g�J�b�g�ݒ�iInsertToolItem()����ړ��BTCMD���\�[�X����ǂݏo���j
			//�i�o�[�W����3.0.0b1�ō폜���Ă������A�����p���ł��Ȃ��Ƃ����s��񍐂��������̂ŁA��͂蕜������B#949�j
			ANumber	shortcut = NULL;
			ABool	shiftKey = false;
			ABool	controlKey = false;
			ABool	optionKey =false;
			LoadTool_LegacyShortcutData(file,shortcut,shiftKey,controlKey,optionKey);
			AMenuShortcutModifierKeys	modifiers = AKeyWrapper::MakeMenuShortcutModifierKeys(controlKey,optionKey,shiftKey);
			mToolItemArray_Shortcut.GetObject(toolItemArrayIndex).Set(GetToolItemCount(inDynamicMenuObjectID)-1,shortcut);
			mToolItemArray_Modifiers.GetObject(toolItemArrayIndex).Set(GetToolItemCount(inDynamicMenuObjectID)-1,modifiers);
		}
		//���j���[�X�V
		UpdateToolMenu(inDynamicMenuObjectID);
		//order�t�@�C���������� #949
		WriteToolOrderFile(inDynamicMenuObjectID);
	}
#endif
}

//�c�[�����ڒǉ��i�Ō�̍��ڂƂ��Ēǉ��j
AIndex	AModePrefDB::AddToolItem( const AObjectID inMenuObjectID, const AFileAcc& inFile, 
		const ABool inWriteOrderFile, const ABool inUpdateMenu, const ABool inAutoUpdate )//B0321 #427
{
	AIndex	index = GetToolItemCount(inMenuObjectID);
	InsertToolItem(inMenuObjectID,index,inFile,inWriteOrderFile,inUpdateMenu,inAutoUpdate);
	return index;
}

/**
�c�[�����ڒǉ�
toolpref.mi����̃f�[�^�ǂݍ��݂͂��Ȃ��Bshortcut�f�[�^���͋�̂܂܂ɂȂ�B
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
	//menutext, actiontext�擾
	AText	menutext, actiontext;
	inFile.GetFilename(menutext);
	//�������̂���GetPath()�֕ύXinFile.GetNormalizedPath(actiontext);
	inFile.GetPath(actiontext);
	//menutext����g���q������
	AText	suffix;
	menutext.GetSuffix(suffix);
	if( suffix.GetLength() > 0 )
	{
		menutext.Delete(menutext.GetLength()-suffix.GetLength(),suffix.GetLength());
	}
	//�t�H���_�^�t�@�C������
	if( inFile.IsFolder() == true && inFile.IsBundleFolder() == false )//R0137
	{
		//�t�H���_�[�Ȃ�T�u���j���[�𐶐�
		AMenuItemID	menuItemID = kMenuItemID_Tool;
		if( mModeID == kStandardModeIndex )
		{
			menuItemID = kMenuItemID_Tool_StandardMode;
		}
		AObjectID	subMenuObjectID = GetApp().NVI_GetMenuManager().CreateDynamicMenu(menuItemID,false);
		//R0173 ���݂̃t�H���_��array�ɍ��ڑ}��
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
		//R0173 �V�Karray�ǉ�
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
		//�T�u�t�H���_�ǂݍ���
		LoadToolRecursive(inFile,subMenuObjectID,inAutoUpdate);//�T�u�t�H���_�̎����X�V�t���O�͐e�������p��
	}
	else
	{
		//�t�@�C���Ȃ�A�R�}���h�V���[�g�J�b�g�ݒ��ǂݏo�����̂��A���j���[�ɍ��ڒǉ��B
		/*#305 LoadToolRecursive()�̋��f�[�^�ǂݍ��݂̉ӏ��Ɉړ�
		ANumber	shortcut = NULL;
		ABool	shiftKey = false;
		ABool	controlKey = false;
		ABool	optionKey =false;
#if IMPLEMENTATION_FOR_MACOSX
		LoadTool_LegacyShortcutData(inFile,shortcut,shiftKey,controlKey,optionKey);
#endif
		AMenuShortcutModifierKeys	modifiers = AKeyWrapper::MakeMenuShortcutModifierKeys(controlKey,optionKey,shiftKey);
		*/
		//R0173 ���݂̃t�H���_��array�ɍ��ڑ}��
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
	//order�t�@�C����������
	if( inWriteOrderFile == true )
	{
		WriteToolOrderFile(inMenuObjectID);
	}
	//
	if( inUpdateMenu == true )
	{
		UpdateToolMenu(inMenuObjectID);
	}
	//���[�g�̏ꍇ�A���[�U�[�c�[���J�n�ʒu�������炷
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
StaticText�c�[���i���j���[�ɐ����������\�����邽�߁j��}��
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
	//�c�[���f�[�^
	mToolItemArray_File.GetObject(arrayIndex).InsertNew1Object(inItemIndex);
	mToolItemArray_SubMenuObjectID.GetObject(arrayIndex).Insert1(inItemIndex,kObjectID_Invalid);
	mToolItemArray_Enabled.GetObject(arrayIndex).Insert1(inItemIndex,true);
	mToolItemArray_Shortcut.GetObject(arrayIndex).Insert1(inItemIndex,0);
	mToolItemArray_Modifiers.GetObject(arrayIndex).Insert1(inItemIndex,0);
	mToolItemArray_Grayout.GetObject(arrayIndex).Insert1(inItemIndex,true);
	mToolItemArray_AutoUpdate.GetObject(arrayIndex).Insert1(inItemIndex,inAutoUpdate);
	mToolItemArray_StaticText.GetObject(arrayIndex).Insert1(inItemIndex,inStaticText);
	//���ꖈ�^�C�g��
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
	//order�t�@�C����������
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
//�c�[�����ڍ폜�i�t�@�C���E�t�H���_�͍폜���Ȃ��j
void	AModePrefDB::DeleteToolItem( const AObjectID inMenuObjectID, const AIndex inItemIndex,
		const ABool inWriteOrderFile, const ABool inUpdateMenu )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//submenu�����݂���ꍇ�̓T�u���j���[�폜
	AObjectID	subMenuID = mToolItemArray_SubMenuObjectID.GetObjectConst(arrayIndex).Get(inItemIndex);
	if( subMenuID != kObjectID_Invalid )
	{
		//�T�u���j���[���̊e���ڍ폜
		while( GetToolItemCount(subMenuID) > 0 )
		{
			DeleteToolItem(subMenuID,0,false,false);
		}
		//�T�u���j���[��array�폜
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
	//�c�[�����ڃf�[�^�폜
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
	//order�t�@�C����������
	if( inWriteOrderFile == true )
	{
		WriteToolOrderFile(inMenuObjectID);
	}
	//
	if( inUpdateMenu == true )
	{
		UpdateToolMenu(inMenuObjectID);
	}
	//���[�g�̏ꍇ�A���[�U�[�c�[���J�n�ʒu�������炷
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
//�c�[�����ڐ��擾
AItemCount	AModePrefDB::GetToolItemCount( const AObjectID inMenuObjectID ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	return mToolItemArray_File.GetObjectConst(arrayIndex).GetItemCount();
}

//�t�@�C������Index���������� B0406 R0173
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
�w��c�[���t�@�C���̃��j���[object ID�A����сA���[�g����̃��j���[object ID�z��i���[�g��index 0�A�e���j���[���Ō�̍��ځj��Ԃ�
*/
ABool	AModePrefDB::GetToolMenuObjectIDFromFile( const AFileAcc& inFile, 
												 AObjectID& outMenuObjectID, AArray<AObjectID>& outMenuObjectIDArray ) const
{
	//���ʏ�����
	outMenuObjectID = kObjectID_Invalid;
	outMenuObjectIDArray.DeleteAll();
	//�w��c�[���t�@�C�����烁�j���[object ID���擾
	AIndex	arrayIndex = kIndex_Invalid;
	AIndex	itemIndex = kIndex_Invalid;
	if( FindToolIndexByFile(inFile,arrayIndex,itemIndex) == false )
	{
		//�c�[���t�@�C����������Ȃ���΃��^�[��
		return false;
	}
	//���j���[object ID�擾
	outMenuObjectID = mToolMenuArray_MenuObjectID.Get(arrayIndex);
	
	//�w��c�[���t�@�C���̐e�t�H���_���擾
	AFileAcc	folder;
	folder.SpecifyParent(inFile);
	//�t�H���_����A���j���[object ID���擾�i�e���������ǂ��āA������Ȃ��Ȃ�܂ŌJ��Ԃ��j
	while( FindToolIndexByFile(folder,arrayIndex,itemIndex) == true )
	{
		//���ʂɒǉ�
		outMenuObjectIDArray.Insert1(0,mToolMenuArray_MenuObjectID.Get(arrayIndex));
		//�e�t�H���_�擾
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
//�c�[�����j���[�\��
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
			menutext.DeleteAfter(menutext.GetSuffixStartPos());//B0000 ���j���[�\���͊g���q�ȍ~�폜
			mToolItemArray_File.GetObject(arrayIndex).GetObject(i).GetNormalizedPath(actiontext);
			//#305 ���j���[�\��������擾�@�D�揇�ʁF���݂̌��ꁨ�p�ꁨ�t�@�C����
			GetToolDisplayName(inMenuObjectID,i,menutext);
			//���j���[���ڒǉ�
			AMenuShortcutModifierKeys	shortcutmod = mToolItemArray_Modifiers.GetObject(arrayIndex).Get(i);//win
#if IMPLEMENTATION_FOR_WINDOWS
			//Windows�ł̍����F
			//Control�`�F�b�N�{�b�N�X��Alt�`�F�b�N�{�b�N�X�ɕύX
			//Shift�`�F�b�N�{�b�N�X��Shift�`�F�b�N�{�b�N�X�ɕύX
			//Ctrl�͏�ɕK�v�Ƃ���
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
//ToolItemIndex���烁�j���[��Index���擾�ienabled�łȂ����ڂ��������C���f�b�N�X�j
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
���[�U�[�c�[���}���ʒu���擾
*/
AIndex	AModePrefDB::GetUserToolInsertIndex( const AObjectID inMenuObjectID ) const
{
	if( inMenuObjectID == mToolMenuRootObjectID )
	{
		//���[�g�̏ꍇ�͋L�����Ă����ʒu
		return mToolMenuRoot_UserToolEndIndex;
	}
	else
	{
		//���[�g�ȊO�̏ꍇ�͍Ō�
		return GetToolItemCount(inMenuObjectID);
	}
}

/**
key tool�̊J�n���̃��j���[���擾
*/
void	AModePrefDB::GetKeyToolStartMenu( const AIndex inSyntaxPartIndex, const ABool inUserTool,
										 AObjectID& outMenuObjectID, 
										 AIndex& outStartIndex, AIndex& outEndIndex ) const
{
	/*
	//���[�g�擾
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(mToolMenuRootObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	*/
	//inUserTool�ɉ����āA�A�v���g�ݍ��݁A���[�U�[�����ꂩ�̈ʒu���擾
	AIndex	startIndex = mToolMenuRoot_AppToolStartIndex;
	AIndex	endIndex = mToolMenuRoot_AppToolEndIndex;
	if( inUserTool == true )
	{
		startIndex = mToolMenuRoot_UserToolStartIndex;
		endIndex = mToolMenuRoot_UserToolEndIndex;
	}
	//inSyntaxPartIndex�i���@�p�[�g�j�̎w�肪����΁A���̕��@�p�[�g���̃t�H���_����J�n����
	if( inSyntaxPartIndex != kIndex_Invalid )
	{
		//���@�p�[�g���擾
		AText	syntaxPartName;
		mSyntaxDefinition.GetSyntaxPartSeparatorName(inSyntaxPartIndex,syntaxPartName);
		//���[�g���j���[�̊e���ږ��̃��[�v
		for( AIndex i = startIndex; i < endIndex; i++ )
		{
			AText	filename;
			//mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(i).GetFilename(filename);
			GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mToolMenuRootObjectID,i,filename);
			if( filename.Compare(syntaxPartName) == true )
			{
				//------------------�p�[�g���Ɠ������ڂ��������ꍇ------------------
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
	//���@�p�[�g�̎w�肪�����ꍇ�A�������́A���̕��@�p�[�g���̃t�H���_�������ꍇ�́A���[�g����J�n
	outMenuObjectID = mToolMenuRootObjectID;
	outStartIndex = startIndex;
	outEndIndex = endIndex;
}

#pragma mark ---�c�[���i���j���[�ւ�Attach/Detach�j

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

#pragma mark ---�c�[���i�ݒ�j

//�t�@�C���ƂƂ��Ƀc�[�����ڍ폜 R0173
void	AModePrefDB::DeleteToolItemAndFile( const AObjectID inMenuObjectID, const AIndex inItemIndex )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//�S�~����
	AFileAcc	file;
	file.CopyFrom(mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(inItemIndex));
	ALaunchWrapper::MoveToTrash(file);
	//�c�[�����ڍ폜
	DeleteToolItem(inMenuObjectID,inItemIndex,true,true);
	//ToolPref�t�@�C���������� #305
	WriteToolOrderFile(inMenuObjectID);
}

//�c�[�����̕ύX
ABool	AModePrefDB::SetToolName( const AFileAcc& inFile, const AText& inNewName, AFileAcc& outNewFile )
{
	outNewFile.CopyFrom(inFile);
	//�t�@�C������Index������
	AIndex	arrayIndex = kIndex_Invalid;
	AIndex	itemIndex = kIndex_Invalid;
	if( FindToolIndexByFile(inFile,arrayIndex,itemIndex) == false )
	{
		_ACError("invalid tool file",NULL);
		return false;
	}
	AObjectID	menuObjectID = mToolMenuArray_MenuObjectID.Get(arrayIndex);
	//B0328 �t�@�C�������ς��Ȃ��Ƃ��͏I��
	AText	filename;
	inFile.GetFilename(filename);
	if( filename.Compare(inNewName) == true )   return false;
	//�t�@�C�����ύX
	if( outNewFile.Rename(inNewName) == false )   return false;
	/*#305 Delete����Insert���ƁA�e����f�[�^�������Ă��܂����߁iInsert�ł́Atoolpref.mi�̓��e���ǂݍ��܂�Ȃ��j
	//���ڂ��폜���Ēǉ�
	DeleteToolItem(menuObjectID,itemIndex,false,false);
	InsertToolItem(menuObjectID,itemIndex,outNewFile,true,true);
	*/
	
	//�t�@�C�����f�[�^�ύX
	mToolItemArray_File.GetObject(arrayIndex).GetObject(itemIndex).CopyFrom(outNewFile);
	//���j���[�X�V
	UpdateToolMenu(menuObjectID);
	//ToolPref�t�@�C���������� #305
	WriteToolOrderFile(menuObjectID);
	
	/*#427 �]���̃R�[�h�͖��̕ύX����c�[���Ɠ����K�w�̃c�[����S�폜���āA�ēǂݍ��݂��Ă����B
	����A���[�g�̏ꍇ�A�����X�V�^���[�U�[�c�[�������݂���̂ŁA���̃R�[�h��NG�B
	���[�g�̏ꍇ�͑S�̂��ēxLoad����悤�ɕύX�B
	//�ύX�����t�@�C���̃t�H���_�ȉ���S�čēǍ��i�T�u�t�H���_�̃p�X�����ύX����邽�߁j
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

//�V���[�g�J�b�g�ݒ� B0406
void	AModePrefDB::SetToolShortcut( const AFileAcc& inFile, const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers )
{
	//�t�@�C������Index������
	AIndex	arrayIndex = kIndex_Invalid;
	AIndex	itemIndex = kIndex_Invalid;
	if( FindToolIndexByFile(inFile,arrayIndex,itemIndex) == false )
	{
		_ACError("invalid tool file",NULL);
		return;
	}
	AObjectID	menuObjectID = mToolMenuArray_MenuObjectID.Get(arrayIndex);
	//�f�[�^�ݒ�
	mToolItemArray_Shortcut.GetObject(arrayIndex).Set(itemIndex,inShortcut);
	mToolItemArray_Modifiers.GetObject(arrayIndex).Set(itemIndex,inModifiers);
	//�t�@�C���ɐݒ�
	/*#305
#if IMPLEMENTATION_FOR_MACOSX
	WriteTool_LegacyShortcutData(inFile,inShortcut,
			AKeyWrapper::IsShiftKeyOnMenuShortcut(inModifiers),AKeyWrapper::IsControlKeyOnMenuShortcut(inModifiers));
#endif
	*/
	//ToolPref�t�@�C���������� #305
	WriteToolOrderFile(menuObjectID);
	//���j���[�X�V
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
	//order�t�@�C����������
	WriteToolOrderFile(inMenuObjectID);
	//���j���[�X�V
	UpdateToolMenu(inMenuObjectID);
	//ToolPref�t�@�C���������� #305
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
	//order�t�@�C����������
	WriteToolOrderFile(inMenuObjectID);
	//���j���[�X�V
	UpdateToolMenu(inMenuObjectID);
}

void	AModePrefDB::MoveToolItem( const AObjectID inMenuObjectID, const AIndex inOldIndex, const AIndex inNewIndex )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//�f�[�^�ړ�
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
	//order�t�@�C����������
	WriteToolOrderFile(inMenuObjectID);
	//���j���[�X�V
	UpdateToolMenu(inMenuObjectID);
}

//#305
/**
�c�[��������^�C�g���ݒ�
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
			//�����Ȃ�
			break;
		}
	}
	//order�t�@�C����������
	WriteToolOrderFile(inMenuObjectID);
	//���j���[�X�V
	UpdateToolMenu(inMenuObjectID);
}

#pragma mark ---�c�[���i�擾�j

//MenuObjectID��Index����t�@�C�����擾 B0406
void	AModePrefDB::GetToolFile( const AObjectID inMenuObjectID, const AIndex inItemIndex, AFileAcc& outFile ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	outFile.CopyFrom(mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(inItemIndex));
}

//�V���[�g�J�b�g�擾
void	AModePrefDB::GetToolShortcut( const AObjectID inMenuObjectID, const AIndex inItemIndex, 
			ANumber& outShortcut, AMenuShortcutModifierKeys& outModifiers ) const //#427 �������ق��Ɠ���
{
	outShortcut = 0;
	outModifiers = 0;
	//Index������
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//�擾
	outShortcut = mToolItemArray_Shortcut.GetObjectConst(arrayIndex).Get(inItemIndex);
	outModifiers =mToolItemArray_Modifiers.GetObjectConst(arrayIndex).Get(inItemIndex);
}

//MenuObjectID��Index����T�u���j���[�擾 R0173
AObjectID	AModePrefDB::GetToolSubMenuID( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	return mToolItemArray_SubMenuObjectID.GetObjectConst(arrayIndex).Get(inItemIndex);
}

//enable�擾 R0173
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
�c�[��������^�C�g���擾
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
			//�����Ȃ�
			break;
		}
	}
}

//#305
/**
�c�[���\�����O�擾
�D�揇�ʁF���݂̌��ꁨ�p�ꁨ�t�@�C����
*/
void	AModePrefDB::GetToolDisplayName( const AObjectID inMenuObjectID, const AIndex inItemIndex, AText& outName ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//#427 staticText������΂�����D��
	AText	staticText;
	mToolItemArray_StaticText.GetObjectConst(arrayIndex).Get(inItemIndex,staticText);
	if( staticText.GetItemCount() > 0 )
	{
		outName.SetText(staticText);
		return;
	}
	
	//�t�@�C����
	mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(inItemIndex).GetFilename(outName);
	//#444 �g���q�폜
	AText	suffix;
	outName.GetSuffix(suffix);
	if( suffix.GetLength() > 0 )
	{
		outName.Delete(outName.GetLength()-suffix.GetLength(),suffix.GetLength());
	}
	//�����ꖼ�O
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
�����X�V�t���O�擾
*/
ABool	AModePrefDB::GetToolAutoUpdateFlag( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//�擾
	return mToolItemArray_AutoUpdate.GetObjectConst(arrayIndex).Get(inItemIndex);
}

//#427
/**
�c�[����StaticText���ǂ�������
*/
ABool	AModePrefDB::GetToolIsStaticText( const AObjectID inMenuObjectID, const AIndex inItemIndex ) const
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	//�擾
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
	//�擾
	return mToolMenuArray_AutoUpdateFolder.Get(arrayIndex);
}

#pragma mark ---�c�[���iorder�t�@�C���j

void	AModePrefDB::WriteToolOrderFile( const AObjectID inMenuObjectID )
{
	AIndex	arrayIndex = mToolMenuArray_MenuObjectID.Find(inMenuObjectID);
	if( arrayIndex == kIndex_Invalid )
	{
		_ACThrow("invalid menu objectID",NULL);
	}
	/*#427 order�t�@�C���͕ۑ����Ȃ��悤�ɂ���i�ݒ�t�@�C������������͍̂�������̂Łj
	AItemCount	count = mToolItemArray_File.GetObjectConst(arrayIndex).GetItemCount();
	//order�t�@�C����������
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
		��order�t�@�C���͕ۑ����Ȃ��悤�ɂ���i���܂�ݒ�t�@�C������������̂���������̂Łj
		AFileAcc	orderFileSJIS;
		orderFileSJIS.SpecifySister(f,"order");
		orderText.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
		orderFileSJIS.CreateFile();//B0000
		orderFileSJIS.WriteFile(orderText);
		*
	}
	*/
	/*#1275 ���[�g�ȊO�̃��j���[�ł�autoupdate/user���ꂼ���toolpref.mi��ۑ����悤�Ƃ��Ă����̂ŁA�C������B
	//#427 AutoUpdate�̃c�[���ƁAUser��`�̃c�[�����ꂼ��toolpref.mi�ɕۑ����邽�߂̃��[�v
	for( AIndex j = 0; j <= 1; j++ )
	{
		//���̃��[�v��autoUpdate���ǂ�����ݒ�
		ABool	autoUpdate = false;
		if( j == 0 )
		{
			autoUpdate = true;
		}
	*/
	//���[�g���j���[���ǂ����̔��� #1275
	if( mToolMenuArray_MenuObjectID.Get(arrayIndex) == mToolMenuRootObjectID )
	{
		//���[�g���j���[�̏ꍇ�́Aautoupdate/user������toolpref.mi��ۑ�
		WriteToolOrderFile(arrayIndex,true);
		WriteToolOrderFile(arrayIndex,false);
	}
	else
	{
		//���[�g���j���[�ȊO�̏ꍇ�́A���j���[��autoupdate�t���O�ɏ]���āAautoupdate/user�ǂ��炩��toolpref.mi��ۑ�
		WriteToolOrderFile(arrayIndex,mToolMenuArray_AutoUpdateFolder.Get(arrayIndex));
	}
}
void	AModePrefDB::WriteToolOrderFile( const AIndex arrayIndex, const ABool autoUpdate )
{
	//#305
	//ToolPrefDB��������
	AToolPrefDB	toolPrefDB;
	for( AIndex i = 0; i < mToolItemArray_File.GetObjectConst(arrayIndex).GetItemCount(); i++ )
	{
		//#427 �����X�V�^���[�U�[�ҏW�ǂ��炩�݂̂����ꂼ��toolPrefDB�֏�������
		if( mToolItemArray_AutoUpdate.GetObjectConst(arrayIndex).Get(i) != autoUpdate )
		{
			continue;
		}
		//#427 statictext�c�[����toolpref�ɏ��������Ȃ�
		AText	staticText;
		mToolItemArray_StaticText.GetObjectConst(arrayIndex).Get(i,staticText);
		if( staticText.GetItemCount() > 0 )
		{
			continue;
		}
		//
		AText	filename;
		mToolItemArray_File.GetObjectConst(arrayIndex).GetObjectConst(i).GetFilename(filename,false);
		//�ݒ�
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
		//������^�C�g��
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
	//toolpref.mi�t�@�C���擾
	//autoupdate�̏ꍇ�́A"autoupdate_userdata"�t�H���_���̑Ή�����t�H���_��toolpref.mi���w�肷��B#427
	AFileAcc	toolPrefFile;
	//#427 toolPrefFile.SpecifyChild(mToolMenuArray_Folder.GetObjectConst(arrayIndex),"toolpref.mi");
	GetToolPrefFile(arrayIndex,autoUpdate,toolPrefFile);//#427
	//
	toolPrefDB.WriteToPrefTextFile(toolPrefFile);
}

//#427
/**
ToolPref�t�@�C����FileAcc���擾
@note ���[�g���j���[�̏ꍇ�̓��[�U�[��`�c�[���Ǝ����X�V�c�[�������݂��Ă���̂�inAutoUpdate�t���O�łǂ����toolpref.mi���擾���邩��I������B
���[�g���j���[�ȊO�̏ꍇ��inAutoUpdate�t���O�ɂ�����炸���j���[�����[�U�[��`�c�[���A�����X�V�c�[���ǂ���Ȃ̂��ɏ]���B
*/
void	AModePrefDB::GetToolPrefFile( const AIndex inToolMenuArrayIndex, const ABool inAutoUpdateForRootMenu, AFileAcc& outToolPrefFile ) const
{
	//���j���[��autoupdate�t���O�擾
	ABool	autoUpdate = mToolMenuArray_AutoUpdateFolder.Get(inToolMenuArrayIndex);
	//���[�g���j���[���ǂ������擾
	ABool	rootMenu = (mToolMenuArray_MenuObjectID.Get(inToolMenuArrayIndex) == mToolMenuRootObjectID);
	//���[�g���j���[�Ȃ�Aautoupdate�t���O�͈����ɏ]��
	if( rootMenu == true )
	{
		autoUpdate = inAutoUpdateForRootMenu;
	}
	//autoupdate���ǂ����̕���
	if( autoUpdate == false )
	{
		//���[�U�[��`�c�[���̏ꍇ
		
		//�c�[���̂���t�H���_��toolpref.mi
		outToolPrefFile.SpecifyChild(mToolMenuArray_Folder.GetObjectConst(inToolMenuArrayIndex),"toolpref.mi");
	}
	else
	{
		//�����X�V�c�[���̏ꍇ
		
		//"tool"�̌�̕����p�X�擾
		
		//�����X�V�t�H���_��tool�t�H���_�i��(mode name)/autoupdate/tool/�j���擾
		AFileAcc	toolRootFolder;
		GetToolFolder(true,toolRootFolder);
		//�w�胁�j���[�ɑΉ�����t�H���_�́Atool�t�H���_�����΃p�X���擾
		AText	partialpath;
		if( rootMenu == true )
		{
			//��������
			//���[�g�̏ꍇ�́A���΃p�X�͋�Ƃ���B
			//mToolMenuArray_Folder�́A���[�U�[�c�[���̂ق���tool�t�H���_�ɂȂ��Ă���̂ŁAGetPartialPath()�����s����Ƃ��������Ȃ�
		}
		else
		{
			//�w�胁�j���[�ɑΉ�����t�H���_�́Atool�t�H���_����̑��΃p�X���擾
			mToolMenuArray_Folder.GetObjectConst(inToolMenuArrayIndex).GetPartialPath(toolRootFolder,partialpath);
		}
		//"autoupdate_userdata"�t�H���_���̑Ή�����t�H���_��toolpref.mi���擾
		AFileAcc	autoUpdateUserDataToolRootFolder;
		GetAutoUpdateUserDataFolder("tool",autoUpdateUserDataToolRootFolder);
		AFileAcc	folder;
		folder.SpecifyChild(autoUpdateUserDataToolRootFolder,partialpath);
		folder.CreateFolderRecursive();//�ċA�I�Ƀt�H���_����
		//
		outToolPrefFile.SpecifyChild(folder,"toolpref.mi");
	}
}

#pragma mark ===========================

#pragma mark ---�c�[���o�[

void	AModePrefDB::LoadToolbar()
{
	/*#427 ���ֈړ�
	AFileAcc	toolbarFolder;
	GetToolbarFolder(toolbarFolder);
	
	//�t�H���_�̒��̃t�@�C�����擾���Achildren�֊i�[
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
			if( CWindowImp::IsDynamicIcon(iconID) == true )//#232 Icon��DynamicIcon�łȂ��ꍇ�iGetToolbarIconFromToolContent()�Ŏ擾�����A�C�R���j���͂���
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
	
	//�����X�V�c�[���o�[�ƁA���[�U�[�ҏW�ҏW�c�[���o�[���ꂼ��L���^�����ݒ�ɏ]���ă��[�h���s
	//#844 �c�[���o�[�̓��[�U�[�ҏW�c�[���o�[�i�}�N���o�[�j�݂̂ɂ���Bfor( AIndex j = 0; j <= 1; j++ )
	{
		/*#844
		//j=0: AutoUpdate�c�[���o�[�Aj=1: ���[�U�[�c�[���o�[
		ABool	autoUpdate = false;
		if( j == 0 )
		{
			autoUpdate = true;
		}
		
		//�����X�V�c�[���o�[���ڃ��[�h�L������
		if( autoUpdate == true && GetData_Bool(kEnableAutoUpdateToolbar) == false )
		{
			continue;
		}
		
		//�����X�V�c�[���o�[���ڃ��[�h�L������
		if( autoUpdate == false && GetData_Bool(kEnableUserToolbar) == false )
		{
			continue;
		}
		*/
		//�c�[���o�[�t�H���_�擾
		AFileAcc	toolbarFolder;
		GetToolbarFolder(/*#844 autoUpdate,*/toolbarFolder);
		toolbarFolder.CreateFolderRecursive();
		
		/*#844
		//�c�[���o�[�t�H���_�����݂��Ă��Ȃ���Ή������Ȃ�
		if( toolbarFolder.Exist() == false )
		{
			continue;
		}
		*/
		
		//�t�H���_�̒��̃t�@�C�����擾���Achildren�֊i�[
		AObjectArray<AFileAcc>	children;
		toolbarFolder.GetChildren(children);
		
		//
		AFileAcc	toolPrefFile;
		toolPrefFile.SpecifyChild(toolbarFolder,"toolpref.mi");
		if( toolPrefFile.Exist() == true )
		{
			//toolpref.mi�t�@�C������ǂݍ���
			
			AToolPrefDB	toolPrefDB;
			//#844 if( autoUpdate == false )
			{
				//���[�U�[�ҏW�c�[���̏ꍇ
				//�c�[���t�@�C���Ɠ����t�H���_��toolpref.mi��ǂݍ���
				toolPrefDB.LoadFromPrefTextFile(toolPrefFile);
			}
			/*#844 
			else
			{
				//#427
				//�����X�V�c�[���̏ꍇ�Aautoupdate�t�H���_����toolpref.mi��
				//autoupdate_userdata�t�H���_���̑Ή�����t�H���_��toolpref.mi�Ƃ��}�[�W����B�i�c�[�������L�[�Ƀ}�[�W�j
				
				//�܂��Aautoupdate_userdata�t�H���_���̑Ή�����t�H���_��toolpref.mi��ǂݍ���
				//�i���݂��Ȃ���Γǂݍ��܂Ȃ��j
				AFileAcc	userDataToolPrefFile;
				GetToolbarPrefFile(true,userDataToolPrefFile);
				if( userDataToolPrefFile.Exist() == true )
				{
					toolPrefDB.LoadFromPrefTextFile(userDataToolPrefFile);
				}
				//autoupdate�t�H���_����toolpref.mi��ǂݍ���
				AToolPrefDB	autoUpdateToolPrefDB;
				autoUpdateToolPrefDB.LoadFromPrefTextFile(toolPrefFile);
				//autoupdate�t�H���_����toolpref.mi�̊e���ڂ��ƂɃ��[�v
				for( AIndex autoupdateIndex = 0; 
							autoupdateIndex < autoUpdateToolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName);
							autoupdateIndex++ )
				{
					AText	filename;
					autoUpdateToolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FileName,autoupdateIndex,filename);
					//AutoUpdate���̃c�[���Ɠ����t�@�C�����̃c�[����UserData�����猟��
					AIndex	foundIndex = toolPrefDB.Find_TextArray(AToolPrefDB::kToolArray_FileName,filename);
					if( foundIndex == kIndex_Invalid )
					{
						//UserData���ɑ��݂��Ȃ������ꍇ�AUserData�֒ǉ�����
						toolPrefDB.InsertRowFromOtherDB_Table(AToolPrefDB::kToolArray_FileName,
									toolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName),
									autoUpdateToolPrefDB,autoupdateIndex);
					}
					else
					{
						//UserData���ɑ��݂����ꍇ�A������c�[�����O�̂݃R�s�[
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
			
			//==================V2����̈����p��������==================
			//V2�ł̕W���Y�t�c�[���o�[�̓f�t�H���g�Ŕ�\���ɂ���
			//�iV2��toolpref.mi�Ή��o�[�W�����Ń��[�U�[���쐬�����c�[���o�[���ڂ̂݁A�\�����邱�Ƃɂ���j
			
			//tool pref���ɐݒ肳�ꂽ�o�[�W�����e�L�X�g���擾
			AText	versionText;
			toolPrefDB.GetData_Text(AToolPrefDB::kVersionText,versionText);
			//kVersionText����̂Ƃ����o�[�W����3�ŏ������񂾂��Ƃ̖����f�[�^
			if( versionText.GetItemCount() == 0 )
			{
				AItemCount	prefItemCount = toolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName);
				for( AIndex index = 0; index < prefItemCount; index++ )
				{
					//�o�[�W����2.1�ł̕W���Y�t�c�[���o�[���Ɠ����ꍇ�͍폜����B
					//�o�[�W����3�ŐV�K�ǉ������f�[�^�ŕ\���^��\�����Ă���̂ŁA�o�[�W����2.1��ł̕\���ɂ͉e�����Ȃ�
					AText	name;
					toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FileName,index,name);
					if( GetApp().SPI_IsV2ToolbarItemNameToHide(name) == true )
					{
						toolPrefDB.SetData_BoolArray(AToolPrefDB::kToolArray_EnabledForMacroBar,index,false);
					}
				}
			}
			
			//==================
			
			//�t�H���_�̒��̃t�@�C�����擾���Achildren�֊i�[
			AObjectArray<AFileAcc>	children;
			toolbarFolder.GetChildren(children);
			//�t�H���_�̒��̃t�@�C���ɂ��Ă̒ǉ��ς݃t���O�A�t�@�C����Hash
			AHashTextArray	childrenFilesName;
			ABoolArray	childrenFilesAddedFlag;
			for( AIndex i = 0; i < children.GetItemCount(); i++ )
			{
				AText	filename;
				children.GetObject(i).GetFilename(filename);
				childrenFilesName.Add(filename);
				childrenFilesAddedFlag.Add(false);
			}
			
			//ToolPref�̊e���ڂ�ǉ�
			AItemCount	prefItemCount = toolPrefDB.GetItemCount_Array(AToolPrefDB::kToolArray_FileName);
			for( AIndex index = 0; index < prefItemCount; index++ )
			{
				//�c�[���ǉ�
				AText	filename;
				toolPrefDB.GetData_TextArray(AToolPrefDB::kToolArray_FileName,index,filename);
				//#427 
				//toolpref.mi�ɋL�q���ꂽ���ڂ̃t�@�C�������݂��Ă��Ȃ��ꍇ�A
				//���̍��ڂ͖�������B
				//�i2.1.13b1���d�l�ύX�B�����X�V���A�c�[�����폜���ꂽ�ꍇ�Aautoupdate_userdata�t�H���_�̂ق���
				//���ڂ������c��\��������B���̏ꍇ�A�c�[����ǉ����ׂ��ł͂Ȃ��B�܂��A�]���ɂ����Ă��A��̃c�[����ǉ����郁���b�g���Ȃ��B�j#427
				AFileAcc	file;
				file.SpecifyChild(toolbarFolder,filename);
				AToolbarItemType	type = GetToolbarTypeFromFile(file);
				if( file.Exist() == false && 
							(type == kToolbarItemType_File || type == kToolbarItemType_Folder ) )//#427   file.CreateFile();
				{
					//���̍��ڂ͖������Ď���
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
				//������^�C�g���ݒ�
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
				
				//ToolPref�̃t�@�C�����ɑΉ�����t�@�C�������݂�����ǉ��ς݃t���OOn
				AIndex	childrenFilesIndex = childrenFilesName.Find(filename);
				if( childrenFilesIndex != kIndex_Invalid )
				{
					childrenFilesAddedFlag.Set(childrenFilesIndex,true);
				}
			}
			
			//���ǉ��iToolPref�ɑ��݂��Ȃ����́j�̃t�@�C����ǉ�
			//Finder�⋌�o�[�W�����Ńt�@�C������ύX�����ꍇ�ɖ��ǉ��ɂȂ�B
			//�i������^�C�g�����̃f�[�^�������Ȃ����߂ɁAToolPref�̃f�[�^���������Ă����j
			//�������A��\���ɂ���B
			for( AIndex i = 0; i < children.GetItemCount(); i++ )
			{
				//�ǉ��ς݂͑ΏۊO
				if( childrenFilesAddedFlag.Get(i) == true )   continue;
				//�t�@�C�����擾
				AFileAcc	file = children.GetObject(i);
				AText	filename;
				file.GetFilename(filename);
				//�ΏۊO�t�@�C��
				if( filename.GetLength() == 0 )   continue;
				if( filename.Get(0) == '.' )   continue;
				if( filename.Compare("order") == true )   continue;
				if( filename.Compare("Icon\r") == true )   continue;
				if( filename.Compare("toolpref.mi") == true )   continue;
				//�A�C�R���t�@�C���͑ΏۊO win
				AText	suffix;
				filename.GetSuffix(suffix);
				if( suffix.Compare(".ico") == true )   continue;
				if( suffix.Compare(".icns") == true )   continue;//#232
				//�c�[���ǉ�
				AIndex	index = mToolbarItem_Type.Add(kToolbarItemType_File);
				mToolbarItem_File.AddNewObject();
				//#724 mToolbarItem_IconID.Add(kIconID_NoIcon);
				mToolbarItem_Enabled.Add(false);//#724 toolpref.mi�ɓo�^����Ă��Ȃ��c�[���o�[���ڂ͔�\���ɂ���
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
				//������^�C�g���ݒ�
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
		//���o�[�W�����ō쐬�������[�h�f�[�^�Ƃ̌݊����̂��߂̏���
		//�����X�V�f�[�^��toolpref.mi�K�{��O��Ƃ���B�itoolpref.mi�������o�[�W�����͊��ɂ��Ȃ�Â��̂ŁB�j
		else
		{
			//order�t�@�C���L�ڏ��ɓo�^
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
				//order�t�@�C������line�ւP�s�擾
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
					mToolbarItem_Enabled.Add(false);//#724 toolpref.mi�̂Ȃ��o�[�W�����̃c�[���o�[���ڂ͔�\���ɂ���
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
						mToolbarItem_Enabled.Add(false);//#724 toolpref.mi�̂Ȃ��o�[�W�����̃c�[���o�[���ڂ͔�\���ɂ���
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
			
			//order�t�@�C���ɋL�ڂ���Ă��Ȃ��A�c��̃t�@�C����o�^
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
				//�A�C�R���t�@�C���͑ΏۊO win
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
				mToolbarItem_Enabled.Add(false);//#724 toolpref.mi�̂Ȃ��o�[�W�����̃c�[���o�[���ڂ͔�\���ɂ���
				//#844 mToolbarItem_AutoUpdate.Add(false);//#427
			}
			
			//������^�C�g���ݒ� #305
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
�c�[���o�[�t�@�C���i�t�@�C�����́j����^�C�v���擾
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
�c�[�����e�L�X�g����Toolbar�A�C�R���w��
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
			//�����Ȃ�
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
			//�����Ȃ�
			break;
		}
	}
	//#724 mToolbarItem_IconID.Insert1(inItemIndex,iconID);
	mToolbarItem_Enabled.Insert1(inItemIndex,true);
	//#844 mToolbarItem_AutoUpdate.Insert1(inItemIndex,inAutoUpdate);//#427
	//������^�C�g���ݒ�
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
			//�����Ȃ�
			break;
		}
	}
	mToolbarItem_Type.Delete1(inIndex);
	mToolbarItem_File.Delete1Object(inIndex);
	mToolbarItem_FilenameArray.Delete1Object(inIndex);
	//#724 mToolbarItem_IconID.Delete1(inIndex);
	mToolbarItem_Enabled.Delete1(inIndex);
	//#844 mToolbarItem_AutoUpdate.Delete1(inIndex);//#427
	//������^�C�g���폜
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
	/* #427 order�t�@�C���͕ۑ����Ȃ��悤�ɂ���i�ݒ�t�@�C������������͍̂�������̂Łj
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
	��order�t�@�C���͕ۑ����Ȃ��悤�ɂ���i���܂�ݒ�t�@�C������������̂���������̂Łj
	AFileAcc	orderFileSJIS;
	orderFileSJIS.SpecifyChild(toolbarfolder,"order");
	orderText.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	orderFileSJIS.CreateFile();//B0000
	orderFileSJIS.WriteFile(orderText);
	*/
	//#427 AutoUpdate�̃c�[���ƁAUser��`�̃c�[�����ꂼ��toolpref.mi�ɕۑ����邽�߂̃��[�v
	//#844 for( AIndex j = 0; j <= 1; j++ )
	{
		/*#844
		//j=0:AutoUpdate�̃c�[���o�[���ځAj=1:���[�U�[�쐬�̃c�[���o�[����
		ABool	autoUpdate = false;
		if( j == 0 )
		{
			autoUpdate = true;
		}
		*/
		//
		//#305
		//ToolPrefDB��������
		AToolPrefDB	toolPrefDB;
		//�o�[�W�����e�L�X�g��������
		AText	versionText;
		GetApp().NVI_GetVersion(versionText);
		toolPrefDB.SetData_Text(AToolPrefDB::kVersionText,versionText);
		//
		for( AIndex i = 0; i < mToolbarItem_Type.GetItemCount(); i++ )
		{
			/*#844
			//#427 �����X�V�^���[�U�[�ҏW�ǂ��炩
			if( mToolbarItem_AutoUpdate.Get(i) != autoUpdate )
			{
				continue;
			}
			*/
			//
			AText	filename;
			mToolbarItem_File.GetObject(i).GetFilename(filename);
			//�ݒ�
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
					//�����Ȃ�
					break;
				}
			}
			toolPrefDB.Add_TextArray(AToolPrefDB::kToolArray_FileName,filename);
			toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_EnabledForMacroBar,mToolbarItem_Enabled.Get(i));
			toolPrefDB.Add_NumberArray(AToolPrefDB::kToolArray_Shortcut,0);
			toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_ShortcutModifiers_ControlKey,false);
			toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_ShortcutModifiers_ShiftKey,false);
			toolPrefDB.Add_BoolArray(AToolPrefDB::kToolArray_Grayout,false);
			//������^�C�g��
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
		//toolpref.mi�t�@�C���擾
		//autoupdate�̏ꍇ�́A"autoupdate_userdata"�t�H���_���̑Ή�����t�H���_��toolpref.mi���w�肷��B#427
		AFileAcc	toolPrefFile;
		//#427 toolPrefFile.SpecifyChild(toolbarfolder,"toolpref.mi");
		GetToolbarPrefFile(/*#844 autoUpdate,*/toolPrefFile);//#427
		//
		toolPrefDB.WriteToPrefTextFile(toolPrefFile);
	}
}

//#427
/**
Toolbar��toolpref.mi�t�@�C����FileAcc�擾
*/
void	AModePrefDB::GetToolbarPrefFile( /*#844 const ABool inAutoUpdate,*/ AFileAcc& outToolPrefFile ) const
{
	//#844 if( inAutoUpdate == false )
	{
		//���[�U�[��`�c�[���̏ꍇ
		
		//�c�[���o�[�t�H���_�擾
		AFileAcc	toolbarFolder;
		GetToolbarFolder(/*#844 false,*/toolbarFolder);
		//�c�[���o�[�̂���t�H���_��toolpref.mi
		outToolPrefFile.SpecifyChild(toolbarFolder,"toolpref.mi");
	}
	/*#844
	else
	{
		//�����X�V�c�[���̏ꍇ
		
		//"autoupdate_userdata"�t�H���_���̑Ή�����t�H���_��toolpref.mi���擾
		AFileAcc	folder;
		GetAutoUpdateUserDataFolder("toolbar",folder);
		folder.CreateFolderRecursive();//�ċA�I�Ƀt�H���_����
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
	//������^�C�g��
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

//�c�[���o�[���̕ύX
//B0406 ABool	AModePrefDB::RenameToolbarItem( const AIndex inIndex, const AText& inNewName )
ABool	AModePrefDB::SetToolbarName( const AFileAcc& inFile, const AText& inNewName, AFileAcc& outNewFile )
{
	outNewFile.CopyFrom(inFile);
	//�t�@�C������MenuObjectID��Index������
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
	//�t�@�C�����ύX
	if( outNewFile.Rename(inNewName) == false )   return false;
	//�����f�[�^�ύX
	mToolbarItem_File.GetObject(itemIndex).CopyFrom(outNewFile);
	//order�t�@�C�������o��
	WriteToolbarItemOrderFile();
	//�S�c�[���o�[�X�V
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
	//#724 GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();//#232
	return true;
}

//�t�@�C������Index���������� B0406
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
�c�[���o�[���ڂ̕\���E��\����ݒ肷��

�e�L�X�g�E�C���h�E�E���[�h�ݒ�E�C���h�E�̕\�����X�V�����
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
�S�Ẵc�[���o�[���ڂ�\����Ԃɂ���

�e�L�X�g�E�C���h�E�E���[�h�ݒ�E�C���h�E�̕\�����X�V�����
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
�����X�V�E���[�U�[�쐬���w�肵�āA�S�Ẵc�[���o�[���ڂ�Enable/Disable�ɂ���
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
�c�[��������^�C�g���擾
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
			//�����Ȃ�
			break;
		}
	}
}

//#305
/**
�c�[���o�[�\�����擾
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
�c�[��������^�C�g���ݒ�
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
			//�����Ȃ�
			break;
		}
	}
	WriteToolbarItemOrderFile();
	GetApp().SPI_RemakeToolButtonsAll(mModeID);
	//#724 GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();
}

//#427
/**
�c�[���o�[�����X�V�t���O�擾
*/
/*#844
ABool	AModePrefDB::GetToolbarAutoUpdateFlag( const AIndex inItemIndex )
{
	return mToolbarItem_AutoUpdate.Get(inItemIndex);
}
*/

//#724
/**
���Əd�����Ȃ��c�[���o�[���ږ����擾
*/
void	AModePrefDB::GetUniqToolbarName( AText& ioName ) const
{
	//�c�[���o�[�t�H���_�擾
	AFileAcc	toolbarFolder;
	GetToolbarFolder(toolbarFolder);
	//���j�[�N�ȃt�@�C�����̃t�@�C���擾
	AFileAcc	file;
	file.SpecifyUniqChildFile(toolbarFolder,ioName);
	//�t�@�C�����擾
	file.GetFilename(ioName);
}

/**
�w��c�[�����A�c�[���e�L�X�g�ŁA�}�N���o�[���ڐ���
*/
void	AModePrefDB::CreateNewToolbar( const AText& inToolName, const AText& inToolText )
{
	//���s(CR, LF)���X�y�[�X�ɕϊ�
	AText	toolname(inToolName);
	toolname.ReplaceChar(kUChar_LineEnd,kUChar_Space);
	toolname.ReplaceChar(kUChar_LF,kUChar_Space);
	toolname.ReplaceChar(kUChar_CR,kUChar_Space);
	//�c�[���t�@�C�������A�e�L�X�g��������
	AFileAcc	toolbarFolder;
	GetApp().SPI_GetModePrefDB(mModeID).GetToolbarFolder(toolbarFolder);
	AFileAcc	file;
	file.SpecifyChild(toolbarFolder,toolname);
	file.CreateFile();
	file.WriteFile(inToolText);
	//�t�@�C��attr��������
	AFileAttribute	attr;
	GetApp().SPI_GetToolFileAttribute(attr);
	file.SetFileAttribute(attr);
	//�c�[���o�[���ڒǉ�
	GetApp().SPI_GetModePrefDB(mModeID).AddToolbarItem(kToolbarItemType_File,file,false);
	//���[�h�ݒ�X�V
	GetApp().SPI_GetModePrefWindow(mModeID).NVI_UpdateProperty();
}

/**
��\���}�N���̃��X�g���擾
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

#pragma mark ---�L�[���[�h

/**
�J�e�S���A����сA�L�[���[�h���X�g�쐬
*/
void	AModePrefDB::MakeCategoryArrayAndKeywordList()
{
	{
		//#717
		//�r������
		AStMutexLocker	locker(mKeywordListMutex);
		//�L�[���[�h���X�g�S�폜
		mKeywordList.DeleteAllIdentifiers();//#641
		//mUserKeywordIdentifierUniqIDArray.DeleteAll();//#890
	}
	//�J�e�S���f�[�^�S�폜
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
		mCategoryDisabledStateArray.DeleteAll();//R0000 �J�e�S���[��
		mCategoryDisabledStateArrayAllFalse = true;//R0000 �J�e�S���[��
		
		//R0000 �J�e�S���[�۔z��ݒ�̗v�f����state�̐������ݒ肷��
		for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
		{
			mCategoryDisabledStateArray.AddNewObject();
		}
	}
	mKeywordRegExp_Text.DeleteAll();
	mKeywordRegExp_CategoryIndex.DeleteAll();
	
	//==================SDF����ǉ��i�����ʎq�̃J�e�S���j==================
	/*#844 SDF�̌Œ�L�[���[�h�͔p�~�BSDF�̃J�e�S���[�͎��ʎq�݂̂Ɏg�p����B
	//���@��`�t�@�C������L�[���[�h�擾
	{
		//#717
		AStMutexLocker	locker(mKeywordListMutex);
		mSyntaxDefinition.GetKeywordData(mKeywordList);
	}
	*/
	//���@��`�t�@�C������J�e�S���[���擾
	mCategoryArray_Name.SetFromTextArray(mSyntaxDefinition.GetCategoryNameArray());
	//�J�e�S���[�F�ݒ�i���[�h�ݒ�D��A���ݒ�Ȃ當�@��`�t�@�C���̃f�t�H���g�J���[�j
	for( AIndex i = 0; i < mCategoryArray_Name.GetItemCount(); i++ )
	{
		AText	name;
		mCategoryArray_Name.Get(i,name);
		AColor	color, importcolor, localcolor;
		AColor	color_dark, importcolor_dark, localcolor_dark;//#1316
		ATextStyle	menustyle = kTextStyle_Normal;
		//#844 ABool	styleBold, styleItalic, styleUnderline;
		ATextStyle	style = kTextStyle_Normal;
		//SDF��`�J�e�S���̐F�E�X�^�C�����擾�i�FSDF�ɋL�q���ꂽColorSlot�ɏ]���Ď擾�j
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
		//R0000 �J�e�S���[�۔z�� �S�Ă�state�ɁAcategory��ǉ�
		{
			AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
			for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
			{
				//�S�Ă�state��enable
				//���ʎq�͑S�Ă̕��@�p�[�g���ŗL���ɂ���
				//HTML��id��JavaScript���ŗ��p�����肷�邽�߁B�����I�ɂ͐ݒ�Ő؂�ւ�����悤�ɂ����ق����ǂ���������Ȃ��B
				mCategoryDisabledStateArray.GetObject(stateIndex).Add(false);
			}
		}
	}
	
	//==================���o������ǉ�==================
	
	//�J�e�S�����X�g���́A���o���J�e�S���̊J�n�ʒu���L��
	mJumpSetupCategoryStartIndex = mCategoryArray_Name.GetItemCount();
	
	//�e���o���ݒ育�Ƃ̃��[�v
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
		//R0000 �J�e�S���[�۔z�� �S�Ă�state�ɁAcategory��ǉ�
		{
			AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
			for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
			{
				//�S�Ă�state��enable
				mCategoryDisabledStateArray.GetObject(stateIndex).Add(false);
			}
		}
	}
	
	//==================���[�U�[�ǉ�Keyword��ǉ�==================
	
	//�J�e�S�����X�g���́A���[�U�[�L�[���[�h�̃J�e�S���̊J�n�ʒu���L��
	mAdditionalCategoryStartIndex = mCategoryArray_Name.GetItemCount();
	
	//���[�U�[�L�[���[�h���J�e�S�����X�g�ɒǉ��A�L�[���[�h���X�g�̍쐬
	AddNormalKeywordCategoryAndKeyword();
	
	//==================SDF��StateAttribute_DisableAllCategory���w�肳�ꂽstate�͑S�Ă�category��disable�ɂ���==================
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
���[�U�[�L�[���[�h�̍X�V���̏����i�J�e�S�����X�g���̃L�[���[�h�̃v���p�e�B�̍X�V�����Ȃǁj
*/
void	AModePrefDB::UpdateUserKeywordCategoryAndKeyword()
{
	//�A���t�@�x�b�g�e�[�u���X�V
	UpdateIsAlphabetTable();
	
	//#725
	//�T�u�E�C���h�E���̃��[�h�Ɋ֘A����f�[�^�̃N���A
	GetApp().SPI_ClearSubWindowsModeRelatedData(mModeID);
	
	{
		//#717
		AStMutexLocker	locker(mKeywordListMutex);
		//�o�^�����L�[���[�h��S�ăL�[���[�h���X�g����폜
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
	//�J�e�S�����X�g���̃��[�U�[�L�[���[�h�̃f�[�^�폜
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
	//�J�e�S���ۂ̃��[�U�[�L�[���[�h�̃f�[�^�폜
	{
		AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
		AItemCount	statecount = mSyntaxDefinition.GetStateCount();
		for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
		{
			mCategoryDisabledStateArray.GetObject(stateIndex).DeleteAfter(mAdditionalCategoryStartIndex);
		}
	}
	//���K�\���L�[���[�h�̃f�[�^�폜
	mKeywordRegExp_Text.DeleteAll();
	mKeywordRegExp_CategoryIndex.DeleteAll();
	
	//���[�U�[�L�[���[�h�f�[�^�ǉ�
	AddNormalKeywordCategoryAndKeyword();
	
	//#683 �e�h�L�������g��TextInfo����RegExp�I�u�W�F�N�g���X�V����
	GetApp().SPI_UpdateRegExpForAllDocuments(mModeID);
	
	//==================SDF��StateAttribute_DisableAllCategory���w�肳�ꂽstate�͑S�Ă�category��disable�ɂ���==================
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
���[�U�[�L�[���[�h���J�e�S�����X�g�ɒǉ��A����сA�L�[���[�h���X�g�̍쐬
*/
void	AModePrefDB::AddNormalKeywordCategoryAndKeyword()
{
	//�L�[���[�h�t�@�C��(.import)�Ŏw�肵���w�b�_�t�@�C���t�H���_�i�[�����o�ϐ����N���A
	mImportFilePathsByKeywordCategory.DeleteAll();
	
	//state���擾
	AItemCount	statecount = mSyntaxDefinition.GetStateCount();
	//���[�U�[�L�[���[�h�e�J�e�S�����̃��[�v
	for( AIndex i = 0; 
				i < GetItemCount_Array(kAdditionalCategory_Keywords);//#427 mAdditionalCategoryArray_KeywordArray.GetItemCount(); 
				i++ )
	{
		//#427 �L�[���[�h�e�L�X�g��textarray�ɓW�J
		ATextArray	keywordArray;
		keywordArray.ExplodeFromText(GetData_TextArrayRef(kAdditionalCategory_Keywords).GetTextConst(i),
									 kUChar_LineEnd);
		
		//========================�J�e�S�����X�g�ɒǉ�========================
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
		
		//========================�J�e�S���ۂɒǉ�========================
		//R0000 �J�e�S���[�۔z�� �S�Ă�state�ɁAcategory��ǉ�
		//�u�����ɂ���v�`�F�b�N�{�b�N�X�̒l���擾
		ABool	alldisable = GetData_BoolArray(kAdditionalCategory_Disabled,i);
		//�L�����@�p�[�g���擾
		AText	syntaxPartText;
		GetData_TextArray(kAdditionalCategory_ValidSyntaxPart,i,syntaxPartText);
		AIndex	syntaxPartIndex = mSyntaxDefinition.GetSyntaxPartIndexFromName(syntaxPartText);
		//�estate�ɂ��Ẵ��[�v
		for( AIndex stateIndex = 0; stateIndex < statecount; stateIndex++ )
		{
			ABool	disabled = false;
			if( syntaxPartIndex != kIndex_Invalid )
			{
				//�L�����@�p�[�g�̎w�肪����Ƃ��́A���̕��@�p�[�g�̃J�e�S���̂�enable�ɂ���
				if( mSyntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex() == syntaxPartIndex )
				{
					//���݂�state�̕��@�p�[�g���A�ݒ肳�ꂽ���@�p�[�g�ƈ�v�����enable
					disabled = false;
				}
				else
				{
					//���݂�state�̕��@�p�[�g���A�ݒ肳�ꂽ���@�p�[�g�ƈ�v���Ȃ����disable
					disabled = true;
				}
			}
			//�u�����ɂ���v��ON�̂Ƃ��͑S�J�e�S����disable�ɂ���
			if( alldisable == true )
			{
				disabled = true;
			}
			{
				AStMutexLocker	locker(mCategoryDisabledStateArrayMutex);
				//�P�ł��L���Ȃ��̂�����΁AmCategoryDisabledStateArrayAllFalse�t���O��OFF�ɂ���
				if( disabled == true )
				{
					mCategoryDisabledStateArrayAllFalse = false;
				}
				//�J�e�S���ۂɒǉ�
				mCategoryDisabledStateArray.GetObject(stateIndex).Add(disabled);
			}
		}
		
		//========================�L�[���[�h���X�g�ɒǉ�========================
		//�L�[���[�h�J�e�S�����L���̏ꍇ�̂݃L�[���[�h�ǉ� #713
		if( GetData_BoolArray(kAdditionalCategory_Disabled,i) == false )
		{
			//------------------CSV�L�[���[�h------------------
			if( GetData_BoolArray(kAdditionalCategory_UseCSV,i) == true )
			{
				AText	modeName;
				GetModeRawName(modeName);
				AText	csvpath;
				GetData_TextArray(kAdditionalCategory_CSVPath,i,csvpath);
				if( csvpath.GetItemCount() > 0 )
				{
					//------------------�����[�g�t�@�C���̏ꍇ------------------
					if( csvpath.CompareMin("http://") == true && csvpath.GetItemCount() > 7 )
					{
						//http�t�@�C�����[�h�v��
						//�p�X�̗�F"http://proj.mimikaki.net/mi/report/1009?format=csv&USER=anonymous"
						GetApp().SPI_LoadCSVForKeyword(modeName,csvpath,categoryIndex);
					}
					//------------------���[�J���t�@�C���̏ꍇ------------------
					else
					{
						//�ݒ�p�X����CSV�t�@�C���������i�D�揇�ʂɏ]���āj
						AFileAcc	file;
						GetKeywordCSVFileFromPath(csvpath,file);
						//�t�@�C�������݂��Ă�����A���̃t�@�C������L�[���[�h���L�[���[�h���X�g�ɒǉ�
						if( file.Exist() == true )
						{
							//�g���q�擾
							AText	path;
							file.GetPath(path);
							AText	suffix;
							path.GetSuffix(suffix);
							//�g���q�ɂ�鏈������
							if( suffix.Compare(".import") == true )
							{
								//�L�[���[�h�t�@�C��(.import)�Ŏw�肵���w�b�_�t�@�C���t�H���_���L�q�����t�@�C�� #1065
								AText	text;
								file.ReadTo(text);
								text.Add(kUChar_LF);
								mImportFilePathsByKeywordCategory.AddText(text);
							}
							else
							{
								//CSV�t�@�C��
								AText	csvtext;
								//file.ReadTo(csvtext);
								GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_KeywordCSVFile,file,csvtext);
								StartAddCSVKeywordThread(csvtext,categoryIndex);
							}
						}
					}
				}
			}
			//------------------�ʏ�L�[���[�h------------------
			else
			{
				AItemCount	keywordCount = keywordArray.GetItemCount();//#427
				if( GetData_BoolArray(kAdditionalCategory_RegExp,i) == false )
				{
					for( AIndex j = 0; j < keywordCount; j++ )
					{
						const AText&	keyword = keywordArray.GetTextConst(j);
						//#147 �L�[���[�h�����������A���݂����InfoText�ɐݒ�
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
							//�L�[���[�h���X�g�ɃL�[���[�h�ǉ�
							//�i�ʏ�L�[���[�h�̏ꍇ�A���ʎq�ʃJ���[�X���b�g�w��͂Ȃ��B�J���[�X���b�g�̓J�e�S���Őݒ�B�j
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
					//------------------���K�\���L�[���[�h------------------
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
CSV�L�[���[�h�ǂݍ��݃X���b�h�J�n*
*/
void	AModePrefDB::StartAddCSVKeywordThread( const AText& inResultText, const AIndex inCategoryIndex )
{
	ACSVKeywordLoaderFactory	factory(NULL);
	AObjectID	threadObjectID = GetApp().NVI_CreateThread(factory);
	(dynamic_cast<ACSVKeywordLoader&>(GetApp().NVI_GetThreadByID(threadObjectID))).SPNVI_Run(mModeID,inResultText,inCategoryIndex);
}

//#791
/**
CSV�e�L�X�g����L�[���[�h���L�[���[�h���X�g�ɒǉ�����
*/
void	AModePrefDB::AddCategoryKeywordListFromCSV( const AText& inResultText, const AIndex inCategoryIndex, ABool& inAbortFlag )
{
	//�r���J�e�S�����폜�����Ȃǂ��āA�J�e�S���������Ȃ�A�܂��́A�����CSV�L�[���[�h�Ŗ����Ȃ����ꍇ�́A�����������^�[��
	//�iURL����擾�̏ꍇ�A�񓯊��Ȃ̂ŁA�i��m���ł͂��邪�j�A�r���ŃJ�e�S���폜���ꂽ�肵����j
	//�i���܂��ܕʂ́ACSV���g���Ă���J�e�S���ɓ����Ă��܂����Ƃ͂��肤�邪�A����N�����ɂ͂����ƌ��ɖ߂�̂ŁA�Ή����Ȃ��j
	AIndex	additionalCategoryIndex = inCategoryIndex - mAdditionalCategoryStartIndex;
	if( additionalCategoryIndex < 0 || additionalCategoryIndex >= GetItemCount_BoolArray(kAdditionalCategory_UseCSV) )
	{
		return;
	}
	if( GetData_BoolArray(kAdditionalCategory_UseCSV,additionalCategoryIndex) == false )
	{
		return;
	}
	
	//CSV�L�[���[�h�J�e�S���̊e�p�����[�^���擾
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
	//CSV�t�@�C����́A�e�J�����擾
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
	//�e�L�[���[�h���Ƃ̏���
	for( AIndex index = 0; index < keywordArray.GetItemCount(); index++ )
	{
		//�X���b�h��abort���ꂽ�烋�[�v���f
		if( inAbortFlag == true )
		{
			break;
		}
		
		AText	keyword, infotext, typetext, parentKeyword , completionText, url;
		keywordArray.Get(index,keyword);
		//�L�[���[�h�e�L�X�g����Ȃ牽�����Ȃ�
		if( keyword.GetItemCount() == 0 )
		{
			continue;
		}
		//�e�e�L�X�g�擾
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
		//�J���[�X���b�g�擾
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
		//�L�[���[�h���X�g�ɃL�[���[�h�ǉ�
		{
			//#717
			AStMutexLocker	locker(mKeywordListMutex);
			//�L�[���[�h�ǉ�
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
���@��`�̊e�J�e�S���F�X�V
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
		//SDF��`�J�e�S���̐F�E�X�^�C�����擾�i�FSDF�ɋL�q���ꂽColorSlot�ɏ]���Ď擾�j
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
���[�U�[�L�[���[�h�̊e�J�e�S���F�X�V
*/
void	AModePrefDB::UpdateUserKeywordCategoryColor()
{
	//���[�U�[�L�[���[�h�J�e�S�������[�v
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
�J�e�S���ێ擾
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

//�L�[���[�h����
ABool	AModePrefDB::FindKeyword( const AText& inKeyword, AColor& outColor, ATextStyle& outStyle, ABool& outPriorToOtherColor,
		const AIndex inStateIndex )//R0195 R0000 �J�e�S���[��
{
	//�L�[���[�h���X�g����L�[���[�h����
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
	
	//�L�[���[�h���̃J���[�X���b�g�w�肪����΂������D��
	if( colorSlot != kIndex_Invalid )
	{
		//IdentifierList��ColorSlot���ݒ肳��Ă���ꍇ�i��CSV�L�[���[�h�j�A
		//�������D�悷��B
		GetColorSlotData(colorSlot,outColor,outStyle,GetApp().NVI_IsDarkMode());
	}
	else
	{
		//IdentifierList��ColorSlot���ݒ肳��Ă��Ȃ��ꍇ�i��CSV�L�[���[�h�ȊO�j�A
		//�J�e�S���ɐݒ肳�ꂽ�F�E�X�^�C�����擾
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
�J�e�S�����擾
*/
AItemCount	AModePrefDB::GetCategoryCount() const
{
	return mCategoryArray_Name.GetItemCount();
}

/**
CSV�t�@�C���ݒ�p�X����i�D�揇�ʂɏ]���āj�t�@�C������������
*/
void	AModePrefDB::GetKeywordCSVFileFromPath( const AText& inPath, AFileAcc& outFile ) const
{
	//�D�揇��
	//1.��΃p�X
	//2.keyword/�i�e����j�t�H���_�i�����[�U�[�p�j
	//3.keyword�t�H���_�i�����[�U�[�p�j
	//4.autoupdate/keyword/�i�e����j�t�H���_
	//5.autoupdate/keyword/�t�H���_
	
	//1.��΃p�X
	outFile.Specify(inPath);
	if( outFile.Exist() == false )
	{
		//keyword�t�H���_�擾
		AFileAcc	modefolder;
		GetModeFolder(modefolder);
		AFileAcc	keywordFolder;
		keywordFolder.SpecifyChild(modefolder,"keyword");
		//�e����t�H���_�擾
		AText	langText;
		GetApp().NVI_GetLanguageName(langText);
		AFileAcc	langFolder;
		langFolder.SpecifyChild(keywordFolder,langText);
		//2.keyword/�i�e����j�t�H���_�i�����[�U�[�p�j
		outFile.SpecifyChild(langFolder,inPath);
		if( outFile.Exist() == false )
		{
			//3.keyword�t�H���_�i�����[�U�[�p�j
			outFile.SpecifyChild(keywordFolder,inPath);
			if( outFile.Exist() == false )
			{
				//auto update�t�H���_�擾
				AFileAcc	autoUpdateFolder;
				GetAutoUpdateFolder(autoUpdateFolder);
				//autoupdate/keyword�t�H���_�擾
				AFileAcc	autoUpdateKeywordFolder;
				autoUpdateKeywordFolder.SpecifyChild(autoUpdateFolder,"keyword");
				//autoupdate/keyword/�e����t�H���_�擾
				AFileAcc	autoupdateKeywordLangFolder;
				autoupdateKeywordLangFolder.SpecifyChild(autoUpdateKeywordFolder,langText);
				//4.autoupdate/keyword/�i�e����j�t�H���_
				outFile.SpecifyChild(autoupdateKeywordLangFolder,inPath);
				if( outFile.Exist() == false )
				{
					//5.autoupdate/keyword/�t�H���_
					outFile.SpecifyChild(autoUpdateKeywordFolder,inPath);
				}
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---�L�[���[�h����
//#147

//#427
/**
�L�[���[�h�����f�[�^���[�h
*/
void	AModePrefDB::LoadKeywordsExplanation()
{
	//data�t�H���_�擾
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	//�����f�[�^�z��S�폜
	mKeywordExplanationTextArray_Keyword.DeleteAll();
	mKeywordExplanationTextArray_CategoryName.DeleteAll();
	mKeywordExplanationTextArray_Explanation.DeleteAll();
	//�L�[���[�h�����t�@�C���ǂݍ���
	/*#844 autoupdate�z����keywords_explanation.txt�͎g�p���Ȃ�
	//AutoUpdate�f�[�^ #427
	AFileAcc	autoUpdateFolder;
	GetAutoUpdateFolder(autoUpdateFolder);
	//autoupdate/data/keywords_explanation.txt�t�@�C��
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
	//���[�U�[�f�[�^
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
�L�[���[�h�����t�@�C��Parse
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
		//���̎��_��pos�͍s��
		//���̉��s+[�܂ł�����Ƃ���
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
			//�����o�^
			mKeywordExplanationTextArray_Keyword.Add(keyword);
			mKeywordExplanationTextArray_CategoryName.Add(categoryName);
			mKeywordExplanationTextArray_Explanation.Add(explanation);
		}
	}
}

#pragma mark ===========================

#pragma mark ---�L�[���[�h��񌟍�

//#853
/**
�V�X�e���w�b�_����e��L�[���[�h�����X���b�h�p�����L�[���[�h��������(Import���ʎq)
@note �n�b�V�����g��Ȃ��̂Œᑬ�Ȃ��߁A�X���b�h�g�p���K�{�����A���낢��Ȍ������@���ł���Bthread-safe
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
	//�r������
	AStMutexLocker	locker(mSystemHeaderIdentifierLinkListMutex);
	//�L�[���[�h����
	return mSystemHeaderIdentifierLinkList.SearchKeywordImport(inWord,inParentWord,inOption,inCurrentStateIndex,
		outKeywordArray,outParentKeywordArray,
		outTypeTextArray,outInfoTextArray,outCommentTextArray,
		outCompletionTextArray,outURLArray,
		outCategoryIndexArray,outColorSlotIndexArray,
		outStartIndexArray,outEndIndexArray,outScopeArray,outFilePathArray,outMatchedCountArray,inAbort);
}

//#853
/**
�L�[���[�h���X�g����e��L�[���[�h�����X���b�h�p�����L�[���[�h��������(Import���ʎq)
@note �n�b�V�����g��Ȃ��̂Œᑬ�Ȃ��߁A�X���b�h�g�p���K�{�����A���낢��Ȍ������@���ł���Bthread-safe
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
	//�r������
	AStMutexLocker	locker(mKeywordListMutex);
	//���ʂ̊J�n�ʒu�L��
	AIndex	startIndex = outKeywordArray.GetItemCount();
	//�L�[���[�h����
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
	//�L�[���[�h���X�g����̌����̏ꍇ�AoutScopeArray, outFilePathArray�ɂ͊i�[����Ȃ��̂ŁA���̌��ʂƓ����������A�ǉ�����B
	for( AIndex i = startIndex; i < outKeywordArray.GetItemCount(); i++ )
	{
		outScopeArray.Add(kScopeType_ModeKeyword);
		outFilePathArray.Add(GetEmptyText());
	}
	return result;
}

/**
�L�[���[�h��IdInfo���擾
*/
void	AModePrefDB::FindKeywordIdInfo( const AText& inKeyword, 
			ATextArray& outExplanationArray, AArray<AIndex>& outCategoryIndexArray, ATextArray& outParentKeywordArray ) const
{
	/*
	//�L�[���[�h����v������̂����ׂČ���
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
	//�r������
	AStMutexLocker	locker(mKeywordListMutex);
	
	//�L�[���[�h�ɑΉ����鎯�ʎq��S�Ď擾
	AArray<AUniqID>	UniqIDArray;
	mKeywordList.GetIdentifierListByKeyword(inKeyword,UniqIDArray);
	//�e���ʎq���ɏ���
	for( AIndex i = 0; i < UniqIDArray.GetItemCount(); i++ )
	{
		//���ʎqUniqID�擾
		AUniqID	identifierID = UniqIDArray.Get(i);
		//�e���擾
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

//�J�e�S���[�F�擾�i���[�h�ݒ�D��A���ݒ�Ȃ當�@��`�t�@�C���̃f�t�H���g�J���[�j
void	AModePrefDB::GetSyntaxDefinitionCategoryColorByName( const AText& inName, 
		AColor& outColor, AColor& outImportColor, AColor& outLocalColor, ATextStyle& outMenuTextStyle, 
		ATextStyle& outTextStyle, const ABool inDarkMode ) const //#844 #1316
{
	/*#844 �]����SDF�J�e�S���F�f�[�^�͎g�p���Ȃ��B�J���[�X���b�g���g�p�B
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
	//�J���[�X���b�g�̐F���擾
	AIndex	colorSlot = mSyntaxDefinition.GetCategoryColorSlot().Get(index);
	GetColorSlotData(colorSlot,outColor,outTextStyle,inDarkMode);//#1316
	if( GetModeData_Bool(AModePrefDB::kDarkenImportLightenLocal) == true )
	{
		outImportColor = AColorWrapper::ChangeHSV(outColor,0,1.0,0.85);//�C���|�[�g�͏����Z���߂̐F #1316 0.8��0.85 �Â��ƃ_�[�N���[�h�Ō��Â炭�Ȃ�̂Ō��ʂ��������炷�B
		outLocalColor = AColorWrapper::ChangeHSV(outColor,0,1.0,1.3);//���[�J���͏�������߂̐F
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

//state���Ƃ̐F����AModePrefDB���̃����o�[�ϐ�(mSyntaxDefinitionStateColorArray, mSyntaxDefinitionStateColorValidArray)�Ɋi�[����
//�e�L�X�g�`�掞�́AmSyntaxDefinitionStateColorArray, mSyntaxDefinitionStateColorValidArray�̂ق����g�p����B
void	AModePrefDB::UpdateSyntaxDefinitionStateColorArray()
{
	//���@��`state���̐F�e�[�u���X�V
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
			//------------------�J���[�X���b�g�w��̗L��state�̏ꍇ------------------
			//#844 GetSyntaxDefinitionStateColorByName(mSyntaxDefinition.GetStateName(index),color);
			//�J���[�X���b�g�̐F�擾
			ATextStyle	textstyle = kTextStyle_Normal;
			GetColorSlotData(colorSlot,color,textstyle,false);
			GetColorSlotData(colorSlot,color_dark,textstyle,true);//#1316
			//�e�[�u���ɒǉ�
			mSyntaxDefinitionStateColorArray.Add(color);
			mSyntaxDefinitionStateColorArray_Dark.Add(color_dark);
			mSyntaxDefinitionStateColorValidArray.Add(true);
			mSyntaxDefinitionStateArray_TextStyle.Add(textstyle);//#844
		}
		else
		{
			//------------------�J���[�X���b�g�w��̖���state�̏ꍇ------------------
			//�����F���e�[�u���ɒǉ�
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
�J���[�X���b�g�̐F�E�X�^�C���擾
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
			GetModeData_Color(kLetterColor,outColor,inDarkMode);//#1388 CategoryAttribute_ColorSlot���w��̏ꍇ�͕W�������F�ɂ���B
			break;
		}
	}
	//�e�L�X�g�X�^�C���ݒ�
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

//state�ɑΉ�����F���擾����
//�imSyntaxDefinitionStateColorArray, mSyntaxDefinitionStateColorValidArray����擾����j
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

/*R0210 AModePrefDB::LoadMacLegacy_Keyword()�Ɉړ�
void	AModePrefDB::LoadSuffix()
{
	DeleteAll_TextArray(AModePrefDB::kSuffix);
	
	//�b��
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
suffix�n�b�V���e�[�u���X�V
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
	//�O���[�o����suffix�n�b�V�����X�V
	GetApp().SPI_UpdateModeSuffixArray(mModeID);
}

/**
���̃��[�h��suffix�����t�@�C�������ǂ����𔻒�
@note thread-safe
*/
ABool	AModePrefDB::IsThisModeSuffix( const AText& inFilename ) const
{
	//#868
	//���̃��[�h��disable�Ȃ���false
	if( GetData_Bool(kEnableMode) == false )   return false;
	//���̃��[�h��suffix�̂����ꂩ�ƈ�v���邩�ǂ����𔻒�
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
���[�h���擾
�D�揇�ʁF���݂̌���ł̖��́��t�@�C����
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
���[�h�t�@�C�����ύX
*/
ABool	AModePrefDB::ChangeModeFileName( const AText& inName )
{
	//�󕶎���ւ̕ύX�͕s��
	if( inName.GetItemCount() == 0 )   return false;
	//�����t�@�C�����̑��݃`�F�b�N
	AFileAcc	newModeFolderForCheck;
	newModeFolderForCheck.SpecifySister(mModeFolder,inName);
	if( newModeFolderForCheck.Exist() == true )
	{
		return false;
	}
	//�t�H���_���ύX�iRename()��AFileAcc�̃p�X���X�V�����j
	mModeFolder.Rename(inName);
	//���[�h�t�@�C�����ύX������
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

#pragma mark ---���o���ݒ�

/**
���o�������p���K�\�����擾
*/
void	AModePrefDB::SetJumpSetupRegExp( AObjectArray<ARegExp>& outRegExpArray ) const
{
	//���ʑS�폜
	outRegExpArray.DeleteAll();
	//�e���o���ݒ育�Ƃ̃��[�v
	AItemCount	itemCount = mJumpSetup_RegExpText.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//���K�\��������擾
		AText	regexpText;
		//------------------�ȒP�ݒ�̏ꍇ------------------
		if( mJumpSetup_SimplerSetting.Get(i) == true )
		{
			//�J�n������擾
			AText	text;
			mJumpSetup_LineStartText.Get(i,text);
			if( text.GetItemCount() > 0 )
			{
				AItemCount	len = text.GetItemCount();
				AText	exceptChars;
				//�e���o���ݒ薈�̃��[�v
				for( AIndex j = 0; j < itemCount; j++ )
				{
					if( mJumpSetup_SimplerSetting.Get(j) == true )
					{
						//�ȒP�ݒ���g�����o���ݒ�̊J�n��������擾�A������̂ق��������񒷂������A������̕�����͈̔͂œ���������Ȃ�A
						//�����̂���ʒu�̕�����exceptChars�ɒǉ�����B
						AText	t;
						mJumpSetup_LineStartText.Get(j,t);
						if( t.CompareMin(text) == true && t.GetItemCount() > len )
						{
							AText	ch;
							t.Get1UTF8Char(len,ch);
							//�����̂���ʒu�̕�����exceptChars�ɒǉ�
							exceptChars.AddText(ch);
						}
					}
				}
				//���K�\���p�ɃG�X�P�[�v
				text.ConvertToRegExpEscapedText();
				exceptChars.ConvertToRegExpEscapedText();
				//���K�\���e�L�X�g����
				regexpText.SetCstring("[ \t]*(");//�ŏ��̃^�u�X�y�[�X�̗L���͖������Ĉ�v����B���j���[�ɓo�^����̂�$1�̕����B
				regexpText.AddText(text);
				if( exceptChars.GetItemCount() > 0 )
				{
					//���̊ȒP���o���ݒ�Ɉ�v���Ȃ��悤�ɁA���̕����Ƃ��đΏۊO�Ƃ��镶�����w��
					regexpText.AddCstring("[^");
					regexpText.AddText(exceptChars);
					regexpText.AddCstring("]");
				}
				//
				regexpText.AddCstring(".+)");
			}
		}
		//------------------���K�\���ݒ�̏ꍇ------------------
		else
		{
			//���K�\���e�L�X�g�擾
			mJumpSetup_RegExpText.Get(i,regexpText);
		}
		//���K�\���I�u�W�F�N�g�擾�A���K�\���ݒ�
		AIndex	regExpIndex = outRegExpArray.AddNewObject();
		if( regexpText.GetItemCount() > 0 && mJumpSetup_Enable.Get(i) == true )
		{
			outRegExpArray.GetObject(regExpIndex).SetRE(regexpText);
		}
	}
}

/**
���o���ݒ�e�[�u�����X�V
�i���[�h�ݒ�̌��o���ݒ�ύX���ɒ��ڕ��@�F������Q�Ƃ��Ă���f�[�^��ύX���Ȃ��悤�ɂ��邽�߁A�o�b�t�@���������Ă���j
*/
void	AModePrefDB::UpdateJumpSetup()
{
	//�r������
	AStMutexLocker	locker(mJumpSetupMutex);
	
	//�e�f�[�^�R�s�[
	mJumpSetup_Enable.SetFromObject(GetData_BoolArrayRef(kJumpSetup_Enable));
	mJumpSetup_RegExpText.SetFromTextArray(GetData_TextArrayRef(kJumpSetup_RegExp));
	mJumpSetup_LineStartText.SetFromTextArray(GetData_TextArrayRef(kJumpSetup_LineStartText));
	mJumpSetup_DisplayMultiply.SetFromObject(GetData_ConstNumberArrayRef(kJumpSetup_DisplayMultiply));
	mJumpSetup_ColorizeLine.SetFromObject(GetData_ConstBoolArrayRef(kJumpSetup_ColorizeLine));
	mJumpSetup_LineColor.SetFromObject(GetData_ConstColorArrayRef(kJumpSetup_LineColor));
	//�ȒP�ݒ肩���K�\���ݒ肩�ɂ���āA���ꂼ��̃��j���[�o�^�ݒ肩��A���j���[�o�^�ݒ��ݒ�
	mJumpSetup_RegisterToMenu.DeleteAll();
	mJumpSetup_MenuText.DeleteAll();
	AItemCount	itemCount = GetItemCount_BoolArray(kJumpSetup_RegisterToMenu);
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//�ȒP�ݒ�̏ꍇ
		if( GetData_BoolArray(kJumpSetup_SimplerSetting,i) == true )
		{
			mJumpSetup_RegisterToMenu.Add(GetData_BoolArray(kJumpSetup_RegisterToMenu2,i));
			AText	menutext("$1");
			mJumpSetup_MenuText.Add(menutext);
		}
		//���K�\���ݒ�̏ꍇ
		else
		{
			mJumpSetup_RegisterToMenu.Add(GetData_BoolArray(kJumpSetup_RegisterToMenu,i));
			AText	menutext;
			GetData_TextArray(kJumpSetup_MenuText,i,menutext);
			mJumpSetup_MenuText.Add(menutext);
		}
	}
	//�e�f�[�^�R�s�[
	mJumpSetup_RegisterAsKeyword.SetFromObject(GetData_ConstBoolArrayRef(kJumpSetup_RegisterAsKeyword));
	mJumpSetup_KeywordGroup.SetFromObject(GetData_ConstNumberArrayRef(kJumpSetup_KeywordGroup));
	mJumpSetup_ColorizeRegExpGroup.SetFromObject(GetData_ConstBoolArrayRef(kJumpSetup_ColorizeRegExpGroup));
	mJumpSetup_ColorizeRegExpGroup_Number.SetFromObject(GetData_ConstNumberArrayRef(kJumpSetup_ColorizeRegExpGroup_Number));
	mJumpSetup_ColorizeRegExpGroup_Color.SetFromObject(GetData_ConstColorArrayRef(kJumpSetup_ColorizeRegExpGroup_Color));
	mJumpSetup_OutlineLevel.SetFromObject(GetData_ConstNumberArrayRef(kJumpSetup_OutlineLevel));
	mJumpSetup_SimplerSetting.SetFromObject(GetData_ConstBoolArrayRef(kJumpSetup_SimplerSetting));
}

/**
���o���e�[�u���S�폜
*/
void	AModePrefDB::DeleteJumpSetupAll()
{
	//�r������
	AStMutexLocker	locker(mJumpSetupMutex);
	//�S�폜
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
���o���ݒ�p�����[�^�擾
*/
ANumber	AModePrefDB::GetJumpSetup_DisplayMultiply( const AIndex inIndex ) const
{
	return mJumpSetup_DisplayMultiply.Get(inIndex);
}

/**
���o���ݒ�p�����[�^�擾
*/
ABool	AModePrefDB::GetJumpSetup_ColorizeLine( const AIndex inIndex ) const
{
	return mJumpSetup_ColorizeLine.Get(inIndex);
}

/**
���o���ݒ�p�����[�^�擾
*/
AColor	AModePrefDB::GetJumpSetup_LineColor( const AIndex inIndex ) const
{
	return mJumpSetup_LineColor.Get(inIndex);
}

/**
���o���ݒ�p�����[�^�擾
*/
ABool	AModePrefDB::GetJumpSetup_RegisterToMenu( const AIndex inIndex ) const
{
	return mJumpSetup_RegisterToMenu.Get(inIndex);
}

/**
���o���ݒ�p�����[�^�擾
*/
void	AModePrefDB::GetJumpSetup_MenuText( const AIndex inIndex, AText& outMenuText ) const
{
	mJumpSetup_MenuText.Get(inIndex,outMenuText);
}

/**
���o���ݒ�p�����[�^�擾
*/
ABool	AModePrefDB::GetJumpSetup_RegisterAsKeyword( const AIndex inIndex ) const
{
	return mJumpSetup_RegisterAsKeyword.Get(inIndex);
}

/**
���o���ݒ�p�����[�^�擾
*/
ANumber	AModePrefDB::GetJumpSetup_KeywordGroup( const AIndex inIndex ) const
{
	return mJumpSetup_KeywordGroup.Get(inIndex);
}

/**
���o���ݒ�p�����[�^�擾
*/
ABool	AModePrefDB::GetJumpSetup_ColorizeRegExpGroup( const AIndex inIndex ) const
{
	return mJumpSetup_ColorizeRegExpGroup.Get(inIndex);
}

/**
���o���ݒ�p�����[�^�擾
*/
ANumber	AModePrefDB::GetJumpSetup_ColorizeRegExpGroup_Number( const AIndex inIndex ) const
{
	return mJumpSetup_ColorizeRegExpGroup_Number.Get(inIndex);
}

/**
���o���ݒ�p�����[�^�擾
*/
AColor	AModePrefDB::GetJumpSetup_ColorizeRegExpGroup_Color( const AIndex inIndex ) const
{
	return mJumpSetup_ColorizeRegExpGroup_Color.Get(inIndex);
}

/**
���o���ݒ�p�����[�^�擾
*/
AIndex	AModePrefDB::GetJumpSetupCategoryIndex( const AIndex inIndex ) const 
{
	return mJumpSetupCategoryStartIndex+inIndex;
}

/**
���o���ݒ�p�����[�^�擾
*/
ANumber	AModePrefDB::GetJumpSetupOutlineLevel( const AIndex inIndex ) const
{
	return mJumpSetup_OutlineLevel.Get(inIndex);
}

#pragma mark ===========================

#pragma mark ---�L�[�o�C���h


/**
�L�[�o�C���h�擾
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
        //==================�W�����[�h����̍�����ݒ肷��ꍇ==================
        //�W�����[�h�̃L�[�o�C���h�f�[�^���擾
        GetApp().SPI_GetModePrefDB(kStandardModeIndex).GetData_KeyBindAction(inKey,inModiferKeys,
																			 outAction,outText,outDefault,outBindAvailable);
        //�L�[�o�C���h�s�̏ꍇ�͂����Ń��^�[���i���̃��[�h�Őݒ肵�Ă���L�[�o�C���h�ݒ�͎擾���Ȃ��j
        if( outBindAvailable == false )
		{
			return;
		}
	}
    else
	{
		//==================OS�f�t�H���g�L�[�o�C���h����̍�����ݒ肷��ꍇ==================
        //�iOS�f�t�H���g�L�[�o�C���h����̍����̏ꍇ�ł��A�L�[�o�C���h�۔���̂��߂ɁA���L�̏������s���B
        //�f�t�H���g������擾���Ă��邪�A�R�[�����ł�outDefault��true�̏ꍇ��outAction�͎g�p���Ă��Ȃ��B�j
        //==================mi�̃f�t�H���g�L�[�o�C���h����̍�����ݒ肷��ꍇ==================
		//�f�t�H���g�L�[�o�C���h�擾�{�L�[�o�C���h�۔���
		if( GetApp().GetAppPref().GetData_DefaultKeyBindAction(inKey,inModiferKeys,outAction,outText) == false )
		{
			//�L�[�o�C���h�s�̏ꍇ�͂����Ń��^�[���i���̃��[�h�Őݒ肵�Ă���L�[�o�C���h�ݒ�͎擾���Ȃ��j
			outDefault = true;
			outBindAvailable = false;
			return;
		}
	}
    
    //==================���̃��[�h�Őݒ肵�Ă���L�[�o�C���h�ݒ���������A�L��΂����Ԃ�==================
	//�L�[�o�C���h�f�[�^�����v������̂���������B
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
�L�[�o�C���h�擾�i�f�t�H���g���ǂ����E�L�[�o�C���h�ύX�ۂ̕Ԃ�l�Ȃ��o�[�W�����j
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
�L�[�o�C���h�ݒ�
*/
ABool	AModePrefDB::SetData_KeybindAction( const AKeyBindKey& inKey, const AModifierKeys inModiferKeys, const AKeyBindAction& inAction, const AText& inText )
{
	AKeyBindAction	defaultAction;
	AText	defaultText;
	//�f�t�H���g�L�[�o�C���h�擾�{�L�[�o�C���h�۔���
	if( GetApp().GetAppPref().GetData_DefaultKeyBindAction(inKey,inModiferKeys,defaultAction,defaultText) == false )
	{
		//�L�[�o�C���h�s��
		return false;
	}
	else
	{
		//�L�[�o�C���h�f�[�^�����v������̂���������B
		for( AIndex index = 0; index < GetItemCount_NumberArray(kKeyBind_Key); index++ )
		{
			if( GetData_NumberArray(kKeyBind_Key,index) == inKey )
			{
				if( GetData_BoolArray(kKeyBind_ControlKey,index) 		== AKeyWrapper::IsControlKeyPressed(inModiferKeys) &&
						GetData_BoolArray(kKeyBind_OptionKey,index) 	== AKeyWrapper::IsOptionKeyPressed(inModiferKeys) &&
						GetData_BoolArray(kKeyBind_CommandKey,index) 	== AKeyWrapper::IsCommandKeyPressed(inModiferKeys) &&
						GetData_BoolArray(kKeyBind_ShiftKey,index) 		== AKeyWrapper::IsShiftKeyPressed(inModiferKeys) )
				{
					//�ύX�������ǂ����̃t���O�i�Ԃ�l�j
					ABool	changed = false;
					//���݂̃L�[�o�C���h���擾
					AKeyBindAction	oldAction = GetData_NumberArray(kKeyBind_Action,index);
					AText	oldActionText;
					GetData_TextArray(kKeyBind_Text,index,oldActionText);
					//#481 if( defaultAction != inAction || defaultText.Compare(inText) == false )//win
					//�ύX������ΐݒ�
					if( oldAction != inAction || oldActionText.Compare(inText) == false )
					{
						SetData_NumberArray(kKeyBind_Action,index,inAction);
						SetData_TextArray(kKeyBind_Text,index,inText);
						changed = true;
					}
					/*#481 �f�t�H���g�̒l�Ɋւ�炸�A��Ƀf�[�^��ݒ肷��悤�ɕύX�B�x�[�X�ƂȂ�L�[�o�C���h���I���\�ɂȂ������߁B
					else
					{
						//�f�t�H���g�L�[�o�C���h�Ɠ����ɂȂ��Ă��܂����ꍇ�A�f�[�^�폜
						DeleteRow_Table(kKeyBind_Key,index);
					}
                    */
					return changed;
				}
			}
		}
		//�����f�[�^�Ɉ�v������̂������ꍇ�A
		//�f�t�H���g�L�[�o�C���h�ƈقȂ�ꍇ�̂݁A�f�[�^�ǉ�
		//#481 �f�t�H���g�̒l�Ɋւ�炸�A��Ƀf�[�^��ݒ肷��悤�ɕύX�B�x�[�X�ƂȂ�L�[�o�C���h���I���\�ɂȂ������߁B
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
�L�[�o�C���h�폜
*/
ABool	AModePrefDB::Remove_KeybindAction( const AKeyBindKey& inKey, const AModifierKeys inModiferKeys )
{
	ABool	deleted = false;
	AKeyBindAction	defaultAction;
	AText	defaultText;
	//�L�[�o�C���h�f�[�^�����v������̂���������B
	for( AIndex index = 0; index < GetItemCount_NumberArray(kKeyBind_Key); index++ )
	{
		if( GetData_NumberArray(kKeyBind_Key,index) == inKey )
		{
			if( GetData_BoolArray(kKeyBind_ControlKey,index) 		== AKeyWrapper::IsControlKeyPressed(inModiferKeys) &&
					GetData_BoolArray(kKeyBind_OptionKey,index) 	== AKeyWrapper::IsOptionKeyPressed(inModiferKeys) &&
					GetData_BoolArray(kKeyBind_CommandKey,index) 	== AKeyWrapper::IsCommandKeyPressed(inModiferKeys) &&
					GetData_BoolArray(kKeyBind_ShiftKey,index) 		== AKeyWrapper::IsShiftKeyPressed(inModiferKeys) )
			{
				//�f�[�^�폜
				DeleteRow_Table(kKeyBind_Key,index);
				deleted = true;
			}
		}
	}
	return deleted;
}

/**
�L�[�o�C���h���삩��A���̓��������L�[������
@note ����x��
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
�L�[�o�C���h���삩��A���̓��������L�[�̕\����������擾
*/
ABool	AModePrefDB::GetKeyTextFromKeybindAction( const AKeyBindAction inAction, AText& outKeyText ) const
{
	outKeyText.DeleteAll();
	//���삩��L�[������
	AKeyBindKey key = kKeyBindKey_A;
	AModifierKeys	modifierKeys;
	if( FindKeyFromKeybindAction(inAction,key,modifierKeys) == true )
	{
		//modifier�\���������ݒ�
		outKeyText.AddModifiersKeyText(modifierKeys);
		//modifier�\�������񂪂����"+"��ǉ� #1379
		if( outKeyText.GetItemCount() > 0 )
		{
			outKeyText.AddCstring("+");
		}
		//�L�[�̕\���������ǉ�
		AText	t;
		t.SetLocalizedText("Keybind_key",key);
		outKeyText.AddText(t);
		return true;
	}
	return false;
}

//#382
/**
�f�t�H���g�Ɠ����L�[�o�C���h�f�[�^���폜����
*/
/*#481
void	AModePrefDB::RemoveKeyBindsSameAsDefault()
{
	for( AIndex index = 0; index < GetItemCount_NumberArray(kKeyBind_Key); )
	{
		//�L�[�o�C���h�ݒ�擾
		AKeyBindKey	key = (AKeyBindKey)GetData_NumberArray(kKeyBind_Key,index);
		AModifierKeys	modifiers = AKeyWrapper::MakeModifierKeys(
				GetData_BoolArray(kKeyBind_ControlKey,index),
				GetData_BoolArray(kKeyBind_OptionKey,index),
				GetData_BoolArray(kKeyBind_CommandKey,index),
						GetData_BoolArray(kKeyBind_ShiftKey,index));
		//�Ή�����f�t�H���g�L�[�o�C���h���擾
		AKeyBindAction	defaultAction;
		AText	defaultText;
		GetApp().GetAppPref().GetData_DefaultKeyBindAction(key,modifiers,defaultAction,defaultText);
		if( defaultAction == GetData_NumberArray(kKeyBind_Action,index) &&
					GetData_TextArrayRef(kKeyBind_Text).GetTextConst(index).Compare(defaultText) == true )
		{
			//�����Ȃ�폜
			DeleteRow_Table(kKeyBind_Key,index);
		}
		else
		{
			//�Ⴄ�Ȃ玟��
			index++;
		}
	}
}
*/

//R0000
void	AModePrefDB::ImportRegExpIndent( const AText& inText )
{
	//�S�폜
	while( GetItemCount_Array(kIndent_RegExp) > 0 )
	{
		DeleteRow_Table(kIndent_RegExp,0);
	}
	
	//�e�L�X�g�f�[�^���
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
	//#683 �e�h�L�������g��TextInfo����RegExp�I�u�W�F�N�g���X�V����
	GetApp().SPI_UpdateRegExpForAllDocuments(mModeID);
}

//R0000
void	AModePrefDB::ExportRegExpIndent( AText& outText ) const
{
	outText.DeleteAll();
	//�G�N�X�|�[�g�̃e�L�X�g�f�[�^�쐬
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
	//�S�폜
	while( GetItemCount_Array(kCorrespond_StartText) > 0 )
	{
		DeleteRow_Table(kCorrespond_StartText,0);
	}
	
	//�e�L�X�g�f�[�^���
	for( AIndex pos = 0; pos < inText.GetItemCount(); )
	{
		AText	start, end;
		inText.GetLine(pos,start);
		inText.GetLine(pos,end);
		//�f�[�^�ǉ�
		Add_TextArray(kCorrespond_StartText,start);
		Add_TextArray(kCorrespond_EndText,end);
	}
}

//R0000
void	AModePrefDB::ExportCorrespond( AText& outText ) const
{
	outText.DeleteAll();
	//�G�N�X�|�[�g�̃e�L�X�g�f�[�^�쐬
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
//Drag&Drop�}��������̃C���|�[�g
void	AModePrefDB::ImportDragDropFromText( const AText& inText )
{
	//�S�폜
	while( GetItemCount_Array(kDragDrop_Suffix) > 0 )
	{
		DeleteRow_Table(kDragDrop_Suffix,0);
	}
	
	//�e�L�X�g�f�[�^���
	for( AIndex pos = 0; pos < inText.GetItemCount(); )
	{
		AText	suffix;
		AText	inserttext;
		for( ; pos < inText.GetItemCount(); )
		{
			AText	line;
			inText.GetLine(pos,line);
			if( line.GetItemCount() < 4 )   continue;//3����(***�̂�)���_��
			if( (line.Get(0) == '*' && line.Get(1) == '*' && line.Get(2) == '*') == false )   continue;
			line.GetText(3,line.GetItemCount()-3,suffix);
			//***xxx�̎��̍s�̍ŏ�=pos
			AIndex	textspos = pos;
			for( ; pos < inText.GetItemCount(); pos++ )
			{
				if( pos+3 < inText.GetItemCount() )
				{
					//�s����3����*������ꍇ�A������1���R�[�h�I��
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
			inText.GetText(textspos,pos-1-textspos,inserttext);//�Ō�̉��s�R�[�h�͊܂܂Ȃ�
			//�s����****��***�ɒu������
			inserttext.ReplaceCstring("\r****","\r***");
			//�f�[�^�ǉ�
			Add_TextArray(kDragDrop_Suffix,suffix);
			Add_TextArray(kDragDrop_Text,inserttext);
		}
	}
}

//R0000
//Drag&Drop�}��������̃G�N�X�|�[�g
void	AModePrefDB::ExportDragDropToText( AText& outText ) const
{
	outText.DeleteAll();
	//�G�N�X�|�[�g�̃e�L�X�g�f�[�^�쐬
	for( AIndex index = 0; index < GetItemCount_Array(kDragDrop_Suffix); index++ )
	{
		//***�����R�[�h�J�n�̈�
		outText.AddCstring("***");
		//�f�[�^�擾
		AText	suffix, inserttext;
		GetData_TextArray(kDragDrop_Suffix,index,suffix);
		GetData_TextArray(kDragDrop_Text,index,inserttext);
		//�s����***��****�ɕϊ�����B�C���|�[�g���ɖ߂��B
		inserttext.ReplaceCstring("\r***","\r****");
		//
		outText.AddText(suffix);
		outText.Add(kUChar_LineEnd);
		outText.AddText(inserttext);
		outText.Add(kUChar_LineEnd);//Drag&Drop�̂ق��͍Ō�ɉ��s�R�[�h�L��
	}
	outText.AddCstring("***");
}

void	AModePrefDB::ImportKeyBindFromText( const AText& inText, const ABool inAdditional )//B0000
{
	if( inAdditional == false )
	{
		//B0000 �C���|�[�g���͋��f�[�^��S�폜����B�������Ȃ��ƁA�L�[�Ƀf�t�H���g�ȊO�̓�������蓖�ĂĂ����Ƃ��ɁA
		//���̃L�[���f�t�H���g�ł���f�[�^���C���|�[�g�����Ă��A���̃L�[���f�t�H���g�ɂȂ�Ȃ�
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
					//B0000 �L�[�o�C���h���u������}���v�ɂ���2�s�ڈȍ~�̍s����***�������ƌ�F�������
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
				//B0000 �s����****��***�ɒu������
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
						//�����Ȃ�
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
				//�����Ȃ�
				break;
			}
		}
		//text
		outText.Add(kUChar_Colon);
		if( action == keyAction_string )
		{
			//B0000 �L�[�o�C���h���u������}���v�ɂ���2�s�ڈȍ~�̍s����***�������ƌ�F�������
			//�s����***��****�ɂ���B�C���|�[�g���ɕϊ��B
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
	outText.AddCstring("***");//B0000 ***�����Ă����΁A�Ō�̉��s�R�[�h���L�[�o�C���h�̑}��������Ƃ��ĔF������邱�Ƃ͂Ȃ�
}

#if 0
//R0068
void	AModePrefDB::ImportKeywords( const AText& inText )
{
	/*#427
	//�f�[�^�S�폜
	while( GetItemCount_TextArray(kAdditionalCategory_Name) > 0 )
	{
		DeleteRow_Table(kAdditionalCategory_Name,0);
	}
	mAdditionalCategoryArray_KeywordArray.DeleteAll();
	*/
	//AutoUpdate�̃J�e�S���ȊO���폜 #427
	for( AIndex index = GetItemCount_TextArray(kAdditionalCategory_Name)-1;
				index >= 0;
				index-- )
	{
		if( GetData_BoolArray(kAdditionalCategory_AutoUpdate,index) == false )
		{
			DeleteRow_Table(kAdditionalCategory_Name,index);
		}
	}
	
	//���
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
		//�����X�V�f�[�^��Export���Ȃ� #427
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
		//#427 �L�[���[�h�e�L�X�g��textarray�ɓW�J
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

/*#844 keywords.txt�ւ̕ۑ������Ȃ����Ƃɂ������ߕs�v�B
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

#pragma mark ---������

//#427
/**
���[�h�������ꖼ�̂�S�폜
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
�W�����[�h�̖��O���擾
*/
void	AModePrefDB::SetNormalModeNames()
{
	RemoveMultiLanguageModeNames();
	AText	text;
	text.SetLocalizedText("NormalModeName_Japanese");
	SetData_Text(kJapaneseName,text);
}

#pragma mark ===========================

#pragma mark ---�e��t�H���_�擾

//#994
/**
plugins�t�H���_�擾
*/
void	AModePrefDB::GetPluginsFolder( const ABool inAutoUpdate, AFileAcc& outFolder ) const
{
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	/*#1275 autoupdate�t�H���_��GetAutoUpdateFolder()�Ŏ擾
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
tool�t�H���_�擾
*/
void	AModePrefDB::GetToolFolder( const ABool inAutoUpdate, AFileAcc& outFolder ) const//#427
{
	AFileAcc	modefolder;
	GetModeFolder(modefolder);
	/*#1275 autoupdate�t�H���_��GetAutoUpdateFolder()�Ŏ擾
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
toolbar�t�H���_�擾
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
AutoUpdate�t�H���_�擾
*/
void	AModePrefDB::GetAutoUpdateFolder( AFileAcc& outFolder ) const
{
	//Web����X�V�ȊO�i���A�v������default���g�p�j���ǂ����𔻒�
	//#1275 Web����X�V�ȊO�i���A�v������default���g�p�j�̏ꍇ�́Aautoupdate�ɃR�s�[�����A�A�v������default��autoupdate�t�H���_�Ƃ݂Ȃ��B
	//#1374 if( GetData_Bool(AModePrefDB::kEnableModeUpdateFromWeb) == false )
	{
		//�A�v������default���g�p����ꍇ
		
		//���[�hdefault�t�H���_���擾
		AFileAcc	appModeRootFolder;
		GetApp().SPI_GetModeDefaultFolder(appModeRootFolder);
		//�R�s�[�����[�h�����擾
		AText	modename;
		GetData_Text(AModePrefDB::kModeUpdateSourceModeName,modename);
		if( modename.GetItemCount() == 0 )
		{
			//�R�s�[�����[�h������̏ꍇ�i�����j���[�̍ŏ��̍��ڂ��I������Ă����ꍇ�j�A�������[�h���̃��[�h���R�s�[���ɂ���
			GetModeRawName(modename);
		}
		//�R�s�[���t�H���_�擾
		outFolder.SpecifyChild(appModeRootFolder,modename);
	}
	/*#1374
	else
	{
		//Web����X�V�̏ꍇ
		AFileAcc	modefolder;
		GetModeFolder(modefolder);
		outFolder.SpecifyChild(modefolder,"autoupdate");
	}
	*/
}

//#427
/**
autoupdate_userdata�t�H���_�擾
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
�V�X�e���w�b�_�t�@�C�����ǂ�����Ԃ�
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
	//�e�X�g�p���[�h�f�[�^�̍쐬
	// �����ɗ�����Ԃ�"/unittest_mode/testmode"�Ƃ�����t�@�C�����ł��Ă���B
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
	
	//���[�h
	LoadOrWaitLoadComplete();
	
	//�`�F�b�N
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
//modeprefconverter�ł̂ݎg�p����ӏ� #1034

/**
2.1.10�ȑO�̃f�[�^��ǂݍ��݁A�o�[�W����3�`���ŕۑ�����B�imodeprefconverter�Ŏg�p�j
*/
void	AModePrefDB::LoadMacLegacyAndSave( const AModeIndex& inModeID, const AFileAcc& inModeFolder )
{
	mModeID = inModeID;
	mModeFolder = inModeFolder;//#920
	
	//------------------���`���f�[�^�ǂݍ���(2.1.8a7�ȑO)------------------
	try
	{
		LoadMacLegacy();
	}
	catch(...)
	{
		//�t�@�C���̔j�󓙂ɂ��G���[
		_ACError("AModePrefDB::LoadDB() caught exception.",NULL);//#199
	}
	//LoadMacLegacy()�ɂ���ēǂݍ���2.1.8a7�ȑO�`���L�[���[�h��modeprefDB�փR�s�[����#427
	for( AIndex categoryIndex = 0; 
		 categoryIndex < mLegacyKeywordTempDataArray.GetItemCount();
		 categoryIndex++ )
	{
		AText	text;
		mLegacyKeywordTempDataArray.GetObjectConst(categoryIndex).Implode(kUChar_LineEnd,text);
		SetData_TextArray(kAdditionalCategory_Keywords,categoryIndex,text);
	}
	
	//------------------���`���f�[�^�ǂݍ���(2.1.8�`2.1.10)------------------
	//LoadFromPrefFile()�́A�t�@�C�������݂��Ă���΁A���f�[�^�͍폜����B�i��L�œǂ񂾃L�[���[�h���S�č폜�j
	AText	modeRawName;
	GetModeRawName(modeRawName);
	AFileAcc	legacyModeFile;
	legacyModeFile.SpecifyChild(mModeFolder,modeRawName);
	if( legacyModeFile.Exist() == true )
	{
		LoadFromPrefFile(legacyModeFile,'MMKE');
		//MMKE-2�̃L�[���[�h�f�[�^�ǂݍ��� #427
		AText	keywordText;
		legacyModeFile.ReadResouceTo(keywordText,'MMKE',2);
		if( keywordText.GetItemCount() > 0 )
		{
			//LoadLegacyKeywordTextIntoEmptyCatgory()�͋�̃J�e�S���ɑ΂��Ă̂݁Akeyword.txt����L�[���[�h��ǂݍ���
			LoadLegacyKeywordTextIntoEmptyCatgory(keywordText,*this);
		}
	}
	
	//data�t�H���_�擾�E����
	AFileAcc	dataFolder;
	GetDataFolder(dataFolder);
	dataFolder.CreateFolder();
	
	//ModePref.mi��������
	AFileAcc	prefFile;
	prefFile.SpecifyChild(dataFolder,"ModePreferences.mi");
	prefFile.CreateFile();
	WriteToPrefTextFile(prefFile);
}

#endif
