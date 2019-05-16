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

#include "stdafx.h"

#include "AModePrefDB.h"
#include "AApp.h"

const  AControlID	kMenu_ModeSelect					= 101;
const AControlID	kButton_CurrentWindowSize			= 113;
const  AControlID	kButton_BackgroundImageFile			= 132;
const  AControlID	kText_KeywordList					= 171;
const  AControlID	kTable_KeywordCategory				= 172;
//const  AControlID	kColor_KeywordColor					= 173;
//const  AControlID	kCheck_KeywordBold					= 174;
const  AControlID	kCheck_KeywordIgnoreCase			= 175;
const  AControlID	kCheck_KeywordRegExp				= 176;
const  AControlID	kButton_KeywordSave					= 177;
const  AControlID	kButton_KeywordSort					= 178;
const  AControlID	kButton_KeywordAddCategory			= 179;
const  AControlID	kButton_KeywordRemoveCategory		= 180;
const  AControlID	kGroup_KeywordCategorySetup			= 200;
const  AControlID	kButton_KeywordRemoveCategoryOK		= 201;

const  AControlID	kTable_JumpSetup					= 202;
const  AControlID	kText_JumpSetup_RegExp				= 203;
const  AControlID	kCheck_JumpSetup_RegisterToMenu		= 204;
const  AControlID	kText_JumpSetup_MenuText			= 205;
const  AControlID	kText_JumpSetup_SelectionGroup		= 206;
const  AControlID	kCheck_JumpSetup_ColorizeLine		= 207;
const  AControlID	kColor_JumpSetup_LineColor			= 208;
const  AControlID	kCheck_JumpSetup_RegisterAsKeyword	= 209;
//const  AControlID	kColor_JumpSetup_KeywordColor		= 210;
const  AControlID	kText_JumpSetup_KeywordGroup		= 211;
const  AControlID	kButton_JumpSetup_Add				= 212;
const  AControlID	kButton_JumpSetup_Remove			= 213;
const  AControlID	kGroup_JumpSetup					= 225;
const  AControlID	kButton_JumpSetup_RemoveOK			= 226;

const  AControlID	kTable_Suffix						= 214;
const  AControlID	kButton_SuffixAdd					= 215;
const  AControlID	kButton_SuffixRemove				= 216;
const  AControlID	kButton_SuffixRemoveOK				= 275;

const  AControlID	kTable_DragDrop					= 227;
const  AControlID	kButton_DragDropAdd				= 228;
const  AControlID	kButton_DragDrapRemove			= 229;
const  AControlID	kText_DragDrapText				= 230;
const  AControlID	kButton_DragDrapRemoveOK		= 231;
const  AControlID	kGroup_DragDrop					= 234;

const  AControlID	kTable_Correspond				= 219;
const  AControlID	kButton_CorrespondAdd			= 220;
const  AControlID	kButton_CorrespondRemove		= 221;
const  AControlID	kButton_CorrespondRemoveOK		= 232;

const  AControlID	kTable_Indent					= 222;
const  AControlID	kButton_IndentAdd				= 223;
const  AControlID	kButton_IndentRemove			= 224;
const  AControlID	kButton_IndentRemoveOK			= 233;

const  AControlID	kCheck_Keybind_ControlKey		= 235;
const  AControlID	kCheck_Keybind_OptionKey		= 236;
const  AControlID	kCheck_Keybind_CommandKey		= 237;
const  AControlID	kCheck_Keybind_ShiftKey			= 238;
const  AControlID	kTable_Keybind					= 239;
const  AControlID	kPopup_KeybindAction			= 240;
const  AControlID	kText_KeybindActionText			= 241;
const  AControlID	kGroupKeybindItem				= 242;
const  AControlID	kButton_Keybind_Import			= 262;
const  AControlID	kButton_Keybind_Export			= 263;

const  AControlID	kToolbarMode					= 265;
const  AControlID	kButton_ToolAdd					= 267;
const  AControlID	kButton_ToolRemove				= 268;
const  AControlID	kButton_Tool_Display			= 269;
const  AControlID	kText_Tool						= 270;
const  AControlID	kText_Tool_Shortcut				= 271;
const  AControlID	kCheck_Tool_ShortcutControlKey	= 272;
const  AControlID	kCheck_Tool_ShortcutShiftKey	= 273;
const  AControlID	kGroup_Tool						= 274;
													//275 reserved

const  AControlID	kButton_UpdateSyntaxDefinition	= 276;

//#205 const  AControlID	kListViewFrame_StateColor		= 277;
//#868 const  AControlID	kListView_StateColor			= 278;
//#868 const  AControlID	kVScrollBar_StateColor			= 279;

//#205 const  AControlID	kListViewFrame_CategoryColor	= 280;
//#868 const  AControlID	kListView_CategoryColor			= 281;
//#868 const  AControlID	kVScrollBar_CategoryColor		= 282;

const  AControlID	kStateColor						= 283;
const  AControlID	kCategoryColor					= 284;
const  AControlID	kCategoryImportColor			= 288;
const  AControlID	kCategoryLocalColor				= 289;

//#205 const  AControlID	kListViewFrame_Tool				= 285;
const  AControlID	kListView_Tool					= 286;
//#688 const  AControlID	kVScrollBar_Tool				= 287;

const  AControlID	kStateContentGroup				= 290;
const  AControlID	kCategoryContentGroup			= 291;

const  AControlID	kButton_UpdateJumpSetup			= 292;

const  AControlID	kButton_ToolParentFolder		= 293;
const  AControlID	kButton_ToolPath				= 294;
const  AControlID	kText_ToolName					= 295;
const  AControlID	kButton_ToolSave				= 296;
const  AControlID	kToolMode						= 297;
const  AControlID	kButton_ToolRemoveOK			= 298;
const  AControlID	kButton_ChangeToolButtonIcon	= 299;
const  AControlID	kButton_AddToolFolder			= 300;

const  AControlID	kButton_AddNewMode				= 500;
const  AControlID	kButton_RevealModeFolder		= 501;
const  AControlID	kButton_RevealToolFolder		= 502;
const  AControlID	kButton_RevealToolbarFolder		= 503;
const  AControlID	kButton_ReconfigWithToolFolder	= 504;
const  AControlID	kButton_EditSuffixTable			= 505;
const  AControlID	kButton_KeyBindTextOK			= 506;
const  AControlID	kButton_KeywordCategoryEdit		= 507;
const  AControlID	kButton_JumpSetupNameEdit		= 508;
const  AControlID	kButton_DragDropSuffixEdit		= 509;
const  AControlID	kButton_CorrStartEdit			= 510;
const  AControlID	kButton_CorrEndEdit				= 511;
const  AControlID	kButton_IndentRegExpEdit		= 512;
const  AControlID	kButton_IndentCurLineEdit		= 513;
const  AControlID	kButton_IndentNextLineEdit		= 514;
const  AControlID	kButton_NewFile					= 515;
//const  AControlID	kCheck_KeywordUnderline				= 516;//R0195
//const  AControlID	kCheck_KeywordItalic				= 517;//R0195
const  AControlID	kCheck_KeywordPriorToOtherColor		= 518;//R0195
const  AControlID	kCheck_SyntaxCategoryBold			= 519;//R0195
const  AControlID	kCheck_SyntaxCategoryItalic			= 520;//R0195
const  AControlID	kCheck_SyntaxCategoryUnderline		= 521;//R0195
const  AControlID	kText_JumpSetup_Anchor				= 522;//R0212
const  AControlID	kButton_DragDropImport				= 523;//R0000
const  AControlID	kButton_DragDropExport				= 524;//R0000
const  AControlID	kButton_CorrespondImport			= 525;//R0000
const  AControlID	kButton_CorrespondExport			= 526;//R0000
const  AControlID	kButton_RegExpIndentImport			= 527;//R0000
const  AControlID	kButton_RegExpIndentExport			= 528;//R0000
const  AControlID	kTable_ModeSelectFilePath			= 529;//R0210
const  AControlID	kButton_ModeSelectAdd				= 530;//R0210
const  AControlID	kButton_ModeSelectRemove			= 531;//R0210
const  AControlID	kButton_ModeSelectRemoveOK			= 532;//R0210
const  AControlID	kButton_EditModeSelectFilePath		= 533;//R0210
const  AControlID	kButton_KeywordCategory_Up			= 534;//R0183
const  AControlID	kButton_KeywordCategory_Down		= 535;//R0183
const  AControlID	kButton_KeywordCategory_Top			= 536;//R0183
const  AControlID	kButton_KeywordCategory_Bottom		= 537;//R0183
const  AControlID	kButton_JumpSetup_Up				= 538;//R0183
const  AControlID	kButton_JumpSetup_Down				= 539;//R0183
const  AControlID	kButton_JumpSetup_Top				= 540;//R0183
const  AControlID	kButton_JumpSetup_Bottom			= 541;//R0183
const  AControlID	kButton_Keybind_ImportAdd			= 542;//R0000
const  AControlID	kButton_Suffix_Up					= 543;//R0000
const  AControlID	kButton_Suffix_Down					= 544;//R0000
const  AControlID	kButton_Suffix_Top					= 545;//R0000
const  AControlID	kButton_Suffix_Bottom				= 546;//R0000
const  AControlID	kButton_KeywordDisable				= 547;//R0000
const  AControlID	kButton_SyntaxDefColorsImport		= 548;//R0000
const  AControlID	kButton_SyntaxDefColorsExport		= 549;//R0000
const  AControlID	kButton_KeywordsImport				= 550;//R0068
const  AControlID	kButton_KeywordsExport				= 551;//R0068
const  AControlID	kButton_ModeBackup					= 552;//R0000
const  AControlID	kButton_RevertToLaunchBackup		= 553;//R0000
const  AControlID	kButton_RevertToLaunchBackupOK		= 554;//R0000
const  AControlID	kButton_SetToDefault				= 555;//R0000
//const  AControlID	kButton_TextPrefTestExport			= 556;//R0000
//const  AControlID	kButton_TextPrefTestImport			= 557;//R0000
const  AControlID	kButton_KeywordRevert				= 558;//#8
const  AControlID	kButton_ToolRevert					= 559;//#8
const  AControlID	kButton_Tool_Grayout				= 560;//#129
//const  AControlID	kCheck_Tool_UseMultiLanguageName	= 561;//#305
const  AControlID	kPopup_Tool_NameLanguage			= 562;//#305
const  AControlID	kText_Tool_MultiLanguageName		= 563;//#305
const  AControlID	kPopup_ModeNameLanguage				= 564;//#305
const  AControlID	kText_ModeMultiLanguageName			= 565;//#305
const  AControlID	kButton_SelectUserDefinedSDFFile	= 566;//#349
const  AControlID	kButton_UpdateSyntaxDefinition2		= 567;//#349
const  AControlID	kButton_SDFDebugMode				= 568;//#349
const  AControlID	kButton_OpenModeUpdatePage			= 569;//#426
const  AControlID	kText_JumpSetup_DisplayMultiply		= 570;//#450
const  AControlID	kLabel_JumpSetup_DisplayMultiply1	= 571;//#450
const  AControlID	kLabel_JumpSetup_DisplayMultiply2	= 572;//#450
const  AControlID	kLabel_JumpSetup_DisplayMultiply3	= 573;//#450
const  AControlID	kLabel_JumpSetup_DisplayMultiply4	= 574;//#450
const  AControlID	kLabel_JumpSetup_DisplayMultiply5	= 575;//#450
const  AControlID	kToolCaption1						= 577;//#427
const  AControlID	kToolCaption2						= 578;//#427
const  AControlID	kToolCaption3						= 579;//#427
const  AControlID	kToolCaption4						= 580;//#427
const  AControlID	kModeUpateButton					= 581;//#427
const  AControlID	kModeUpateButtonOK					= 582;//#427
const  AControlID	kAutoUpdateInfoText					= 586;//#427
const  AControlID	kModeFileName						= 588;//#427
const  AControlID	kJumpSetupCatpion1					= 589;//#427
const  AControlID	kJumpSetupCatpion2					= 590;//#427
const  AControlID	kJumpSetupCatpion3					= 591;//#427
const  AControlID	kJumpSetupCatpion4					= 592;//#427
//#844 const  AControlID	kModeUpdateURLMenu					= 593;//#427
const  AControlID	kCheck_JumpSetup_ColorizeRegExpGroup		= 594;//#603
const  AControlID	kText_JumpSetup_ColorizeRegExpGroup_Number	= 595;//#603
const  AControlID	kColor_JumpSetup_ColorizeRegExpGroup_Color	= 596;//#603
const  AControlID	kJumpSetupCatpion5							= 597;//#603
const  AControlID	kButton_CreateModeUpdateDistribution		= 598;//#427
const  AControlID	kKeywordCaption1							= 599;//#427
const  AControlID	kKeywordCaption2							= 600;//#427
const  AControlID	kNoIndentBehaviourRadioButton				= 601;//#683

//======================== �u���[�h�v �^�u ========================
const AControlID	kButton_OpenNormalMode_ModeTab				= 7003;
const AControlID	kTable_Plugin								= 7004;//#994

//======================== �u�\���v �^�u ========================
//�w�b�_�t�@�C���ֈړ�const AControlID	kFontControl_DefaultFont					= 7101;
const AControlID	kButton_DefaultFontPanel					= 7102;
const AControlID	kButton_OpenNormalMode_DisplayTab			= 7103;
//const AControlID	kButton_CopyAllModeFromNormalMode_DisplayTab	= 7104;
const AControlID	kRadio_DisplayBackslash						= 7105;//#940

//======================== �u�ҏW�v �^�u ========================
const AControlID	kButton_OpenNormalMode_EditTab				= 7201;
//const AControlID	kButton_CopyAllModeFromNormalMode_EditTab	= 7202;

//======================== �u�L�[���[�h�v �^�u ========================
const AControlID	kRadio_Keyword_NotUseCSVFile				= 7301;
const AControlID	kRadio_Keyword_UseCSVFile					= 7302;
const AControlID	kEditBox_Keyword_CSVPath					= 7303;
const AControlID	kEditBox_Keyword_KeywordColumn				= 7304;
const AControlID	kEditBox_Keyword_ExplanationColumn			= 7305;
const AControlID	kEditBox_Keyword_ParentKeywordColumn		= 7306;
const AControlID	kButton_Keyword_UpdateDocument				= 7307;
const AControlID	kButton_Keyword_SelectCSVFile				= 7308;
const AControlID	kEditBox_Keyword_TypeTextColumn				= 7309;
const AControlID	kEditBox_Keyword_ColorSlotColumn			= 7310;
const AControlID	kPopup_Keyword_ColorSlot					= 7311;
const AControlID	kPopup_Keyword_ValidSyntaxPart				= 7312;
const AControlID	kButton_OpenCSVFile							= 7313;
const AControlID	kStaticText_CategoryKind					= 7314;
const AControlID	kEditBox_Keyword_CompletionTextColumn		= 7315;
const AControlID	kEditBox_Keyword_URLColumn					= 7316;

//======================== �u���o���v �^�u ========================
const AControlID	kText_JumpSetup_OutlineLevel				= 7405;
const AControlID	kRadio_JumpSetup_RegExp						= 7406;
const AControlID	kRadio_JumpSetup_SimplerSetting				= 7407;
const AControlID	kText_JumpSetup_LineStartText				= 7408;
const AControlID	kCheckbox_JumpSetup_RegisterToMenu2			= 7409;
const AControlID	kPopup_JumpSetup_KeywordColorSlot			= 7410;
const AControlID	kCheck_JumpSetup_Enable						= 7411;

//======================== �u���@�v �^�u ========================
const AControlID	kRadio_Syntax_UseSimpleSDF					= 7501;
const AControlID	kColor_Syntax_Slot1							= 7511;
const AControlID	kCheck_Syntax_Slot1Bold						= 7512;
const AControlID	kCheck_Syntax_Slot1Italic					= 7513;
const AControlID	kCheck_Syntax_Slot1Underline				= 7514;

//======================== �u�C���f���g�v �^�u ========================
const AControlID	kRadio_Indent_SameAsPreviousLine			= 7701;
const AControlID	kRadio_Indent_InputTabCodeForIndent			= 7702;

//======================== �u�L�[�o�C���h�v �^�u ========================
const AControlID	kRadio_KeyBind_DefaultIsAppKeyBind			= 7801;

//======================== �u�c�[���v �^�u ========================
const AControlID	kButton_Tool_OpenToolFile					= 7901;
const AControlID	kStaticText_ToolKind						= 7902;
const AControlID	kButton_Tool_CopyToUserTool					= 7903;

//======================== �u�ڍׁv �^�u ========================
const AControlID	kButton_ModeUpdate_Application				= 8001;

//======================== �u�F�v �^�u ========================
const AControlID	kButton_OpenNormalMode_ColorsTab			= 8101;
const AControlID	kButton_ApplyColorScheme					= 8102;
const AControlID	kButton_ExportColorScheme					= 8103;
const AControlID	kButton_RevealColorSchemeFolder				= 8104;
const AControlID	kButton_UpdateColorSchemeMenu				= 8105;

//State ListView
const APrefID		kColumn_StateName				= 0;
//Category ListView
const APrefID		kColumn_CategoryName			= 0;
//Tool ListView
const APrefID		kColumn_ToolName				= 0;

//�e�[�u���r���[ColumnID
const ADataID	kColumn_Category					= 0;


const ADataID		kColumn_DragDrop_Suffix			= 0;

const ADataID		kColumn_Correspond_Start		= 0;
const ADataID		kColumn_Correspond_End			= 1;

const ADataID		kColumn_Indent_RegExp				= 0;
const ADataID		kColumn_Indent_OffsetCurrentLine	= 1;
const ADataID		kColumn_Indent_OffsetNextLine		= 2;

const ADataID		kColumn_JumpSetup_Name				= 0;

const ADataID		kColumn_Suffix_Suffix				= 0;

const ADataID		kColumn_KeyBind_Key					= 0;
const ADataID		kColumn_KeyBind_Action				= 1;

//R0210
const ADataID		kColumn_First						= 0;

//#994
//�v���O�C���e�[�u���e��ID
const ADataID		kColumnID_Plugin_Enabled			= 0;
const ADataID		kColumnID_Plugin_Name				= 1;
const ADataID		kColumnID_Plugin_Options			= 2;

#pragma mark ===========================
#pragma mark [�N���X]AWindow_ModePref
#pragma mark ===========================
//���[�h�ݒ�E�C���h�E

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AWindow_ModePref::AWindow_ModePref():AWindow(/*#175NULL*/), /*B0406 mKeywordTextDirty(false), mCurrentKeywordCategoryIndex(kIndex_Invalid),*/ 
		mCurrentToolMenuObjectID(kObjectID_Invalid), /*#199 mAddNewModeWindow(*this),*/ //#868 mToolbarMode(false),
		/*B0406 mCurrentToolSelectIndex(kIndex_Invalid),*/
		/*#199 mAddToolButtonWindow(*this), mChangeToolButtonIconWindow(*this), mModeExecutableAlertWindow(*this),*/
		mKeywordEditManager(*this), mToolEditManager(*this)//B0406
	 ,mCurrentToolNameLanguage(kLanguage_Japanese)//#305
		,mCurrentModeNameLanguage(kLanguage_Japanese)//#305
,mCurrentKeyBindContentViewModifiers(0)//#688
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kModePrefWindowPosition);
	
	mModeIndex = kStandardModeIndex;
	
	for( AIndex i = 0; i < kKeyBindKeyItemCount; i++ )
	{
		AText	text;
		text.SetLocalizedText("Keybind_key",i);
		mKeyBindKeyText.Add(text);
	}
	
	/*#844
	//#199
	//AddToolButtonWindow
	AChildWindowFactory<AWindow_AddToolButton>	addToolButtonWindowFactory(GetObjectID());
	mAddToolButtonWindowID = GetApp().NVI_CreateWindow(addToolButtonWindowFactory);
	
	//AddNewModeWindow
	AChildWindowFactory<AWindow_AddNewMode>	addNewModeWindowFactory(GetObjectID());
	mAddNewModeWindowID = GetApp().NVI_CreateWindow(addNewModeWindowFactory);
	
	//ChangeToolButtonIconWindow
	AChildWindowFactory<AWindow_ChangeToolButtonIcon>	changeToolButtonIconWindowFactory(GetObjectID());
	mChangeToolButtonIconWindowID = GetApp().NVI_CreateWindow(changeToolButtonIconWindowFactory);
	*/
	
	//B0000 �L�[�o�C���h����\�����ԕύX
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
    //
	mKeybindMenuOrder.Add((AKeyBindAction)0);//����Ȃ�
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//
	mKeybindMenuOrder.Add((AKeyBindAction)1);//������}��
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//���s�E�^�u�E�C���f���g�E�X�y�[�X
	mKeybindMenuOrder.Add((AKeyBindAction)2);//���s�i�C���f���g�����j
	mKeybindMenuOrder.Add((AKeyBindAction)3);//���s�i���ݍs�ƐV�K�s�ŃC���f���g�j
	mKeybindMenuOrder.Add((AKeyBindAction)77);//���s�i�V�K�s�ł̂݃C���f���g�j #249
	mKeybindMenuOrder.Add((AKeyBindAction)5);//�C���f���g�^�^�u�i�s���Ȃ�C���f���g �s���ȊO�Ȃ�^�u�j
	mKeybindMenuOrder.Add((AKeyBindAction)4);//�^�u
	mKeybindMenuOrder.Add((AKeyBindAction)6);//�X�y�[�X
	mKeybindMenuOrder.Add((AKeyBindAction)-1);//#249
	//����
	mKeybindMenuOrder.Add((AKeyBindAction)122);//�������� #1397
	mKeybindMenuOrder.Add((AKeyBindAction)123);//�O������ #1397
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//�ȗ�����
	mKeybindMenuOrder.Add((AKeyBindAction)7);//�ȗ����́i���j
	mKeybindMenuOrder.Add((AKeyBindAction)8);//�ȗ����́i�O�j
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//�폜
	mKeybindMenuOrder.Add((AKeyBindAction)10);//�폜�i�O�P�����j
	mKeybindMenuOrder.Add((AKeyBindAction)9);//�폜�i���P�����j
	mKeybindMenuOrder.Add((AKeyBindAction)89);//�폜�i�s���܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)11);//�폜�i�s���܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)90);//�폜�i�i�����܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)62);//�폜�i�i�����܂ŁE�i�����Ȃ���s�R�[�h�폜�j R0213 
	mKeybindMenuOrder.Add((AKeyBindAction)70);//�폜�i�i���S�́j #124 
	mKeybindMenuOrder.Add((AKeyBindAction)91);//�폜�i�}�[�N�ʒu�܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)88);//�폜�i�O�P�����EDecomposition �������j
	mKeybindMenuOrder.Add((AKeyBindAction)51);//�J�b�g�i�s���܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)40);//�J�b�g�i�s���܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)63);//�J�b�g�i�i�����܂ŁE�i�����Ȃ���s�R�[�h�J�b�g�j R0213
	mKeybindMenuOrder.Add((AKeyBindAction)37);//�J�b�g
	mKeybindMenuOrder.Add((AKeyBindAction)64);//�ǉ��J�b�g�i�i�����܂ŁE�i�����Ȃ���s�R�[�h�J�b�g�j R0213
	mKeybindMenuOrder.Add((AKeyBindAction)65);//�ǉ��J�b�g R0213
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//�L�����b�g�ړ�
	mKeybindMenuOrder.Add((AKeyBindAction)12);//�L�����b�g�ړ��i��j
	mKeybindMenuOrder.Add((AKeyBindAction)13);//�L�����b�g�ړ��i���j
	mKeybindMenuOrder.Add((AKeyBindAction)14);//�L�����b�g�ړ��i���j
	mKeybindMenuOrder.Add((AKeyBindAction)15);//�L�����b�g�ړ��i�E�j
	mKeybindMenuOrder.Add((AKeyBindAction)32);//�L�����b�g�ړ��i�O�̒P��J�n�ʒu�j
	mKeybindMenuOrder.Add((AKeyBindAction)33);//�L�����b�g�ړ��i���̒P��I���ʒu�j
	mKeybindMenuOrder.Add((AKeyBindAction)56);//�L�����b�g�ړ��i�O�̒P��J�n�ʒu�E�s����~����j
	mKeybindMenuOrder.Add((AKeyBindAction)57);//�L�����b�g�ړ��i���̒P��I���ʒu�E�s����~����j
	mKeybindMenuOrder.Add((AKeyBindAction)18);//�L�����b�g�ړ��i�s���j
	mKeybindMenuOrder.Add((AKeyBindAction)19);//�L�����b�g�ړ��i�s���j
	mKeybindMenuOrder.Add((AKeyBindAction)60);//�L�����b�g�ړ��i�i�����j
	mKeybindMenuOrder.Add((AKeyBindAction)61);//�L�����b�g�ړ��i�i�����j
	mKeybindMenuOrder.Add((AKeyBindAction)34);//�L�����b�g�ړ��i�O�y�[�W�j
	mKeybindMenuOrder.Add((AKeyBindAction)35);//�L�����b�g�ړ��i���y�[�W�j
	mKeybindMenuOrder.Add((AKeyBindAction)16);//�L�����b�g�ړ��i�h�L�������g�̍ŏ��j
	mKeybindMenuOrder.Add((AKeyBindAction)17);//�L�����b�g�ړ��i�h�L�������g�̍Ō�j
	mKeybindMenuOrder.Add((AKeyBindAction)124);//�L�����b�g�ړ��i�O�̒i�����j#1399
	mKeybindMenuOrder.Add((AKeyBindAction)125);//�L�����b�g�ړ��i���̒i�����j#1399
	mKeybindMenuOrder.Add((AKeyBindAction)67);//R0006
	mKeybindMenuOrder.Add((AKeyBindAction)68);//R0006
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//�I��
	mKeybindMenuOrder.Add((AKeyBindAction)20);//�I���i��j
	mKeybindMenuOrder.Add((AKeyBindAction)21);//�I���i���j
	mKeybindMenuOrder.Add((AKeyBindAction)22);//�I���i���j
	mKeybindMenuOrder.Add((AKeyBindAction)23);//�I���i�E�j
	mKeybindMenuOrder.Add((AKeyBindAction)43);//�I���i�O�̒P��J�n�ʒu�܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)42);//�I���i���̒P��I���ʒu�܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)58);//�I���i�O�̒P��J�n�ʒu�܂ŁE�s����~����j
	mKeybindMenuOrder.Add((AKeyBindAction)59);//�I���i���̒P��I���ʒu�܂ŁE�s����~����j
	mKeybindMenuOrder.Add((AKeyBindAction)46);//�I���i�h�L�������g�̍ŏ��܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)47);//�I���i�h�L�������g�̍Ō�܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)97);//�I���i�i�����܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)109);//�I���i�i�����܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)48);//�I���i�s���܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)49);//�I���i�s���܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)50);//�I�����[�h
	mKeybindMenuOrder.Add((AKeyBindAction)52);//���ʓ���I��
	mKeybindMenuOrder.Add((AKeyBindAction)100);//���ׂĂ�I��
	mKeybindMenuOrder.Add((AKeyBindAction)101);//�I���i�s�S�́j
	mKeybindMenuOrder.Add((AKeyBindAction)102);//�I���i�i���S�́j
	mKeybindMenuOrder.Add((AKeyBindAction)103);//�I���i�}�[�N�ʒu�܂Łj
	mKeybindMenuOrder.Add((AKeyBindAction)104);//�I���i�P��S�́j
	mKeybindMenuOrder.Add((AKeyBindAction)126);//�I���i�O�̒i�����܂Łj#1399
	mKeybindMenuOrder.Add((AKeyBindAction)127);//�I���i���̒i�����܂Łj#1399
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//�}�[�N
	mKeybindMenuOrder.Add((AKeyBindAction)105);//�}�[�N�ݒ�
	mKeybindMenuOrder.Add((AKeyBindAction)106);//�}�[�N�ƑI��͈͂��X���b�v
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//�X�N���[��
	mKeybindMenuOrder.Add((AKeyBindAction)28);//�X�N���[���i��j
	mKeybindMenuOrder.Add((AKeyBindAction)29);//�X�N���[���i���j
	mKeybindMenuOrder.Add((AKeyBindAction)30);//�X�N���[���i�E�j
	mKeybindMenuOrder.Add((AKeyBindAction)31);//�X�N���[���i���j
	mKeybindMenuOrder.Add((AKeyBindAction)24);//�X�N���[���i�O�y�[�W�j
	mKeybindMenuOrder.Add((AKeyBindAction)25);//�X�N���[���i���y�[�W�j
	mKeybindMenuOrder.Add((AKeyBindAction)26);//�X�N���[���i�h�L�������g�̍ŏ��j
	mKeybindMenuOrder.Add((AKeyBindAction)27);//�X�N���[���i�h�L�������g�̍Ō�j
	mKeybindMenuOrder.Add((AKeyBindAction)45);//�L�����b�g�ʒu���E�C���h�E�����ɂȂ�悤�ɃX�N���[��
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//�}�N���o�[
	mKeybindMenuOrder.Add((AKeyBindAction)110);//�}�N��1
	mKeybindMenuOrder.Add((AKeyBindAction)111);//�}�N��2
	mKeybindMenuOrder.Add((AKeyBindAction)112);//�}�N��3
	mKeybindMenuOrder.Add((AKeyBindAction)113);//�}�N��4
	mKeybindMenuOrder.Add((AKeyBindAction)114);//�}�N��5
	mKeybindMenuOrder.Add((AKeyBindAction)115);//�}�N��6
	mKeybindMenuOrder.Add((AKeyBindAction)116);//�}�N��7
	mKeybindMenuOrder.Add((AKeyBindAction)117);//�}�N��8
	mKeybindMenuOrder.Add((AKeyBindAction)118);//�}�N��9
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//���̑�
	mKeybindMenuOrder.Add((AKeyBindAction)36);//������ (Undo)
	mKeybindMenuOrder.Add((AKeyBindAction)41);//��蒼�� (Redo)
	mKeybindMenuOrder.Add((AKeyBindAction)38);//�R�s�[
	mKeybindMenuOrder.Add((AKeyBindAction)66);//�ǉ��R�s�[ R0213
	mKeybindMenuOrder.Add((AKeyBindAction)39);//�y�[�X�g
	mKeybindMenuOrder.Add((AKeyBindAction)119);//���V�t�g #1154
	mKeybindMenuOrder.Add((AKeyBindAction)120);//�E�V�t�g #1154
	mKeybindMenuOrder.Add((AKeyBindAction)121);//���E�̕��������ւ� #1154
	mKeybindMenuOrder.Add((AKeyBindAction)108);//yank
	mKeybindMenuOrder.Add((AKeyBindAction)44);//�Ή����������
	mKeybindMenuOrder.Add((AKeyBindAction)53);//��̍s�Ɠ�������
	mKeybindMenuOrder.Add((AKeyBindAction)54);//�s�𕡐�
	mKeybindMenuOrder.Add((AKeyBindAction)55);//�󔒍s��ǉ�
	mKeybindMenuOrder.Add((AKeyBindAction)69);//���ʎq���\�� RC2
	mKeybindMenuOrder.Add((AKeyBindAction)79);//���̌��o�� #150
	mKeybindMenuOrder.Add((AKeyBindAction)80);//�O�̌��o�� #150
	mKeybindMenuOrder.Add((AKeyBindAction)81);//���̃^�u��I�� #357
	mKeybindMenuOrder.Add((AKeyBindAction)82);//�O�̃^�u��I�� #357
	mKeybindMenuOrder.Add((AKeyBindAction)83);//�s�^�i���ړ��_�C�A���O�\�� #594
	mKeybindMenuOrder.Add((AKeyBindAction)84);//�����C���f���g ON/OFF �؂�ւ� #638
	mKeybindMenuOrder.Add((AKeyBindAction)-1);
	//
	mKeybindMenuOrder.Add((AKeyBindAction)76);//���s�i���ݍs�ƐV�K�s�ŃC���f���g�s���p�X�y�[�X�t�j #249
	mKeybindMenuOrder.Add((AKeyBindAction)78);//���s�i�V�K�s�ł̂݃C���f���g�s���p�X�y�[�X�t�j #249
	mKeybindMenuOrder.Add((AKeyBindAction)72);//�^�u�s���p�X�y�[�X�t #249 
	mKeybindMenuOrder.Add((AKeyBindAction)71);//�C���f���g #249 
	mKeybindMenuOrder.Add((AKeyBindAction)75);//�C���f���g�s���p�X�y�[�X�t #249 
	mKeybindMenuOrder.Add((AKeyBindAction)73);//�C���f���g�^�^�u�i�s���Ȃ�C���f���g�s���p�X�y�[�X�t �s���ȊO�Ȃ�^�u�j #249 
	mKeybindMenuOrder.Add((AKeyBindAction)74);//�C���f���g�^�^�u�i�s���Ȃ�C���f���g�s���p�X�y�[�X�t �s���ȊO�Ȃ�^�u�s���p�X�y�[�X�t�j #249 
	
	/*
	AItemCount	countcheck = 0;
	for( AIndex i = 0; i < mKeybindMenuOrder.GetItemCount(); i++ )
	{
		if( mKeybindMenuOrder.Get(i) != -1 )   countcheck++;
	}
	if( countcheck != keyActionItemCount )   _ACError("mKeybindMenuOrder count error",this);
	*/
	//�u�f�t�H���g�̓���v�̃��j���[���ڒǉ�
	AText   t;
	t.SetLocalizedText("KeyBindAction_Default");
	mKeyBindActionMenuText.Add(t);
    //�e���j���[���ڒǉ�
	for( AIndex i = 1; i < mKeybindMenuOrder.GetItemCount(); i++ )
	{
        //mKeybindMenuOrder�̒l��-1�̏ꍇ�̓��j���[���ڂƂ��ăZ�p���[�^��}��
		if( mKeybindMenuOrder.Get(i) == -1 )//B0000 �L�[�o�C���h����\�����ԕύX
		{
			AText	text;
			text.SetCstring("-");
			mKeyBindActionMenuText.Add(text);
		}
		//mKeybindMenuOrder�̒l��-1�ł͂Ȃ��ꍇ�̓��j���[���ڂƂ��Ă��̃A�N�V�����̃e�L�X�g��}��
		else
		{
			AText	text;
			text.SetLocalizedText("Keybind_action",mKeybindMenuOrder.Get(i));//B0000 �L�[�o�C���h����\�����ԕύX
			mKeyBindActionMenuText.Add(text);
		}
	}
	//#175
	NVI_AddToRotateArray();
	//#138
	NVI_AddToTimerActionWindowArray();
}

/**
�f�X�g���N�^
*/
AWindow_ModePref::~AWindow_ModePref()
{
}

/**
ObjectArray����̍폜���ɒ��ڃR�[������郁�\�b�h�i�f�X�g���N�^�̓K�[�x�[�W�R���N�V�������ɃR�[�������j
*/
void	AWindow_ModePref::NVIDO_DoDeleted()
{
	//#199
	//#844 GetApp().NVI_DeleteWindow(mAddToolButtonWindowID);
	//#844 GetApp().NVI_DeleteWindow(mAddNewModeWindowID);
	//#844 GetApp().NVI_DeleteWindow(mChangeToolButtonIconWindowID);
	//#844 GetApp().NVI_DeleteWindow(mModeExecutableAlertWindowID);
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//
ADataBase&	AWindow_ModePref::NVMDO_GetDB()
{
	return GetApp().SPI_GetModePrefDB(mModeIndex);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//���j���[�I�����̃R�[���o�b�N�i�������s������true��Ԃ��j
ABool	AWindow_ModePref::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			//B0303
			if( TryCloseCurrentMode() == false )
			{
				return true;
			}
			//NVI_Hide();
			NVI_Close();
			return true;
		}
		/*#350
	  case kMenuItemID_ModePrefXML:
		{
			AText	text;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetXMLFile(text);
			text.ConvertLineEndToCR();
			ADocumentID docID = GetApp().SPNVI_CreateNewTextDocument(kStandardModeIndex);
			GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
			return true;
		}
		*/
		//R0000
	  case kMenuItemID_Save:
		{
			if( NVI_IsControlVisible(kButton_ToolSave) == true && NVI_IsControlEnable(kButton_ToolSave) == true )
			{
				EVTDO_Clicked(kButton_ToolSave,0);
			}
			else if( NVI_IsControlVisible(kButton_KeywordSave) == true && NVI_IsControlEnable(kButton_KeywordSave) == true )
			{
				EVTDO_Clicked(kButton_KeywordSave,0);
			}
			NVM_UpdateControlStatus();
			return true;
		}
	}
	return false;
}

void	AWindow_ModePref::EVTDO_UpdateMenu()
{
	//#350 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ModePrefXML,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,true);
	//R0000
	if( NVI_IsControlVisible(kButton_ToolSave) == true && NVI_IsControlEnable(kButton_ToolSave) == true )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Save,true);
	}
	if( NVI_IsControlVisible(kButton_KeywordSave) == true && NVI_IsControlEnable(kButton_KeywordSave) == true )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Save,true);
	}
}

//�E�C���h�E�̕���{�^��
void	AWindow_ModePref::EVTDO_DoCloseButton()
{
	//B0303
	if( TryCloseCurrentMode() == false )
	{
		return;
	}
	//NVI_Hide();
	NVI_Close();
}

//B0303
ABool	AWindow_ModePref::TryCloseCurrentMode()
{
	/*B0406 if( mKeywordTextDirty == true )
	{
		//
		ShowSaveKeywordAlertSheet();
		return false;
	}*/
	if( mKeywordEditManager.TryClose() == false )//B0406
	{
		return false;
	}
	else if( /*B0406 SwitchToolContent()*/mToolEditManager.TryClose() == false )//B0303
	{
		return false;
	}
	//
	NVM_StoreToDBAll();
	//�c�[�����X�g�I������
	SelectToolListRow(kIndex_Invalid);
	//�L�[���[�h�J�e�S���[�I������
	NVM_SelectTableRow(kTable_KeywordCategory,kIndex_Invalid);
	//B0406 mCurrentKeywordCategoryIndex = kIndex_Invalid;
	//�e��e�[�u���I������
	NVM_SelectTableRow(kTable_JumpSetup,kIndex_Invalid);
	NVM_SelectTableRow(kTable_Suffix,kIndex_Invalid);
	NVM_SelectTableRow(kTable_ModeSelectFilePath,kIndex_Invalid);//R0210
	NVM_SelectTableRow(kTable_DragDrop,kIndex_Invalid);
	NVM_SelectTableRow(kTable_Correspond,kIndex_Invalid);
	NVM_SelectTableRow(kTable_Indent,kIndex_Invalid);
	NVM_SelectTableRow(kTable_Keybind,kIndex_Invalid);
	/*#868 
	//B0000
	NVI_GetListView(kListView_StateColor).SPI_SetSelect(kIndex_Invalid);
	NVI_GetListView(kListView_CategoryColor).SPI_SetSelect(kIndex_Invalid);
	*/
	
	//�]���̃��[�h�̓��e���t�@�C���ɕۑ�
	GetApp().SPI_GetModePrefDB(mModeIndex).SaveToFile();
	//����Ƃ��Ƀc�[������U�ŏ�w�ɖ߂�
	if( mToolMenuObjectIDArray.GetItemCount() > 0 )
	{
		mCurrentToolMenuObjectID = mToolMenuObjectIDArray.Get(0);
	}
	mToolMenuObjectIDArray.DeleteAll();
	mToolMenuPathArray.DeleteAll();
	mToolMenuDisplayPathArray.DeleteAll();
	NVI_UpdateProperty();
	return true;
}

void	AWindow_ModePref::EVTDO_TextInputted( const AControlID inID )
{
	switch(inID)
	{
	  case kText_KeywordList:
		{
			/*B0406 mKeywordTextDirty = true;
			NVI_SetModified(true);*/
			mKeywordEditManager.SetDirty(true);
			NVM_UpdateControlStatus();
			break;
		}
	  case kText_Tool:
		{
			//B0406 SetToolTextDirty(true);//B0320
			mToolEditManager.SetDirty(true);
			NVM_UpdateControlStatus();
			break;
		}
	  case kText_Tool_Shortcut:
		{
			//B0406 ApplyToolShortcut();
			mToolEditManager.SaveShortcut();
			break;
		}
	}
}

/*B0406
//B0320
void	AWindow_ModePref::SetToolTextDirty( const ABool inDirty )
{
	mToolTextDirty = inDirty;
	NVM_UpdateControlStatus();
	NVI_SetModified(inDirty);
	NVI_GetListView(kListView_Tool).SPI_SetEnableDragDrop(inDirty==false);
}
*/

void	AWindow_ModePref::EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile )
{
	switch(inControlID)
	{
	  case kButton_BackgroundImageFile:
		{
			AText	path;
			inFile.GetPath(path);//B0389 OK ,kFilePathType_1);
			NVI_SetControlText(AModePrefDB::kBackgroundImageFile,path);
			GetApp().SPI_GetModePrefDB(mModeIndex).SetData_Text(AModePrefDB::kBackgroundImageFile,path);
			GetApp().SPI_GetModePrefDB(mModeIndex).UpdateBackgroundImage();
			NVM_UpdateControlStatus();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//�L�[�o�C���h�C���|�[�g
	  case kButton_Keybind_Import:
	  case kButton_Keybind_ImportAdd://B0000
		{
			NVM_SelectTableRow(kTable_Keybind,kIndex_Invalid);//B0406
			//R0000 ���f�[�^��ۑ����S�~���ړ�
			{
				AText	text;
				GetApp().SPI_GetModePrefDB(mModeIndex).ExportKeyBindToText(text);
				MakeOldDataCopyInTrash("Keybind_OldDataFileName",text);
			}
			
			AText	text;
			GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_ImportKeybind,inFile,text);
			GetApp().SPI_GetModePrefDB(mModeIndex).ImportKeyBindFromText(text,(inControlID==kButton_Keybind_ImportAdd));//B0000
			NVM_UpdateDBTableView(kTable_Keybind);
			//B0000 ��������Ȃ��Ɠ��상�j���[�̒l�����̂܂܂ɂȂ遨�s�ړ��ŋ����상�j���[�̒l���f�ƂȂ��Ă��܂�
			NVI_UpdateProperty();
			break;
		}
		//DragDrop�C���|�[�g R0000
	  case kButton_DragDropImport:
		{
			NVM_SelectTableRow(kTable_DragDrop,kIndex_Invalid);//B0406
			//���f�[�^��ۑ����S�~���ړ�
			{
				AText	text;
				GetApp().SPI_GetModePrefDB(mModeIndex).ExportDragDropToText(text);
				MakeOldDataCopyInTrash("DragDrop_OldDataFileName",text);
			}
			
			AText	text;
			GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_ImportDragDrop,inFile,text);
			GetApp().SPI_GetModePrefDB(mModeIndex).ImportDragDropFromText(text);
			NVM_UpdateDBTableView(kTable_DragDrop);
			NVI_UpdateProperty();
			break;
		}
		//�Ή�������C���|�[�g R0000
	  case kButton_CorrespondImport:
		{
			NVM_SelectTableRow(kTable_Correspond,kIndex_Invalid);//B0406
			//���f�[�^��ۑ����S�~���ړ�
			{
				AText	text;
				GetApp().SPI_GetModePrefDB(mModeIndex).ExportCorrespond(text);
				MakeOldDataCopyInTrash("Correspond_OldDataFileName",text);
			}
			
			AText	text;
			GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_ImportCorrespond,inFile,text);
			GetApp().SPI_GetModePrefDB(mModeIndex).ImportCorrespond(text);
			NVM_UpdateDBTableView(kTable_Correspond);
			NVI_UpdateProperty();
			break;
		}
		//���K�\���C���f���g�C���|�[�g R0000
	  case kButton_RegExpIndentImport:
		{
			NVM_SelectTableRow(kTable_Indent,kIndex_Invalid);//B0406
			//���f�[�^��ۑ����S�~���ړ�
			{
				AText	text;
				GetApp().SPI_GetModePrefDB(mModeIndex).ExportRegExpIndent(text);
				MakeOldDataCopyInTrash("RegExpIndent_OldDataFileName",text);
			}
			
			AText	text;
			GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_ImportRegExpImport,inFile,text);
			GetApp().SPI_GetModePrefDB(mModeIndex).ImportRegExpIndent(text);
			NVM_UpdateDBTableView(kTable_Indent);
			NVI_UpdateProperty();
			break;
		}
		/*#868
		//���@��`�X�^�C���C���|�[�g R0000
	  case kButton_SyntaxDefColorsImport:
		{
			NVI_GetListView(kListView_StateColor).SPI_SetSelect(kIndex_Invalid);//B0406
			NVI_GetListView(kListView_CategoryColor).SPI_SetSelect(kIndex_Invalid);//B0406
			//���f�[�^��ۑ����S�~���ړ�
			{
				AText	text;
				GetApp().SPI_GetModePrefDB(mModeIndex).ExportSyntaxDefinitionColors(text);
				MakeOldDataCopyInTrash("SyntaxDefColors_OldDataFileName",text);
			}
			
			AText	text;
			GetApp().SPI_GetTextFromFile(inFile,text);
			GetApp().SPI_GetModePrefDB(mModeIndex).ImportSyntaxDefinitionColors(text);
			NVI_GetListView(kListView_StateColor).SPI_SetSelect(kIndex_Invalid);
			NVI_GetListView(kListView_CategoryColor).SPI_SetSelect(kIndex_Invalid);
			NVI_UpdateProperty();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		*/
		/*#844
		//�L�[���[�h�C���|�[�g R0068
	  case kButton_KeywordsImport:
		{
			mKeywordEditManager.TryClose(true);
			//���f�[�^��ۑ����S�~���ړ�
			{
				AText	text;
				GetApp().SPI_GetModePrefDB(mModeIndex).ExportKeywords(text);
				MakeOldDataCopyInTrash("Keywords_OldDataFileName",text);
			}
			
			AText	text;
			GetApp().SPI_GetTextFromFile(inFile,text);
			GetApp().SPI_GetModePrefDB(mModeIndex).ImportKeywords(text);
			NVM_UpdateDBTableView(kTable_KeywordCategory);
			NVI_UpdateProperty();
			//
			//�R�����g�FKeyword RegExp�̍X�V��ImportKeywords()���Ŏ��s�ς݁iUpdateUserKeywordCategoryAndKeyword()�o�R�j
			GetApp().NVI_RefreshAllWindow();//#427
			//
			//mKeywordTextDirty = false;
			//NVI_SetModified(false);
			break;
		}
		*/
		//#349
	  case kButton_SelectUserDefinedSDFFile:
		{
			//�p�X�擾
			AText	path;
			inFile.GetPath(path);
			//���[�h�t�H���_���̃t�@�C���̏ꍇ�A���΃p�X��ۑ�����
			AFileAcc	modeFolder;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetModeFolder(modeFolder);
			AText	modeFolderPath;
			modeFolder.GetPath(modeFolderPath);
			//�w��t�@�C���̂͂��߂̕������A���[�h�t�H���_�p�X���ǂ����𒲂ׂ�
			if( modeFolderPath.GetItemCount() < path.GetItemCount() )
			{
				if( modeFolderPath.Compare(path,0,modeFolderPath.GetItemCount()) == true )
				{
					//���[�h�t�H���_����̑��΃p�X���擾
					inFile.GetPartialPath(modeFolder,path);
				}
			}
			//�p�X�ۑ�
			NVI_SetControlText(AModePrefDB::kUserDefinedSDFFilePath,path);
			GetApp().SPI_GetModePrefDB(mModeIndex).SetData_Text(AModePrefDB::kUserDefinedSDFFilePath,path);
			break;
		}
		//#791
		//�L�[���[�h�pCSV�t�@�C���I����
	  case kButton_Keyword_SelectCSVFile:
		{
			//�p�X�擾
			AText	path;
			inFile.GetPath(path);
			//���[�h��data�t�H���_���̃t�@�C���̏ꍇ�A���΃p�X��ۑ�����
			AFileAcc	dataFolder;
			//#954 GetApp().SPI_GetModePrefDB(mModeIndex).GetDataFolder(dataFolder);
			AFileAcc	modeFolder;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetModeFolder(modeFolder);
			dataFolder.SpecifyChild(modeFolder,"keyword");
			//data�t�H���_�z���̃t�@�C�����ǂ����𔻒�
			if( inFile.IsChildOfFolder(dataFolder) == true )
			{
				//���[�h�t�H���_����̑��΃p�X���擾
				inFile.GetPartialPath(dataFolder,path);
			}
			//CSV�t�@�C���p�X���R���g���[���ɐݒ�
			NVI_SetControlText(kEditBox_Keyword_CSVPath,path);
			break;
		}
	}
}

ABool	AWindow_ModePref::EVTDO_ValueChanged( const AControlID inControlID )
{
	ADataBase& modepref = GetApp().SPI_GetModePrefDB(mModeIndex);
	
	ABool	result = true;
	switch(inControlID)
	{
		//#725
		//�f�t�H���g�t�H���g�ݒ莞����
	  case kFontControl_DefaultFont:
		{
			//�R���g���[������t�H���g�擾
			AText	fontname;
			AFloatNumber	fontsize = 9.0;
			NVI_GetControlFont(kFontControl_DefaultFont,fontname,fontsize);
			//DB�Ƀf�[�^�ݒ�
			modepref.SetData_Text(AModePrefDB::kDefaultFontName,fontname);
			modepref.SetData_FloatNumber(AModePrefDB::kDefaultFontSize,fontsize);
			//�t�H���g�ݒ���e�h�L�������g�ɔ��f
			GetApp().SPI_TextFontSetInModeIsUpdatedAll(mModeIndex);
			//���ݒ�̕��̃R���g���[�����X�V #1373
			if( GetApp().SPI_IsAppPrefWindowCreated() == true )
			{
				GetApp().SPI_GetAppPrefWindow().
						NVI_SetControlFont(AWindow_AppPref::kFontControl_NormalFont,fontname,fontsize);
			}
			break;
		}
		/*#868
		//
	  case kStateColor:
		{
			AIndex sel = NVI_GetListView(kListView_StateColor).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			//ColorPicker�R���g���[���̒l�ύX�𔽉f
			//sel�ɑΉ������Ԗ����擾
			ATextArray	nameArray,displayNameArray;
			AArray<AColor>	colorArray;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetStateColorArray(nameArray,colorArray,displayNameArray);
			AText	name;
			nameArray.Get(sel,name);
			//�F��ColorPicker�R���g���[������擾
			AColor	color;
			NVI_GetControlColor(kStateColor,color);
			//��ԐF�ύX�����[�h�ݒ�ɔ��f����
			GetApp().SPI_GetModePrefDB(mModeIndex).SetSyntaxDefinitionStateColor(name,color);
			GetApp().NVI_RefreshAllWindow();
			break;
		}
	  case kCategoryColor:
		{
			AIndex sel = NVI_GetListView(kListView_CategoryColor).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			//
			AText	categoryName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetCategoryNameArray().Get(sel,categoryName);
			AColor	color;
			NVI_GetControlColor(kCategoryColor,color);
			GetApp().SPI_GetModePrefDB(mModeIndex).SetSyntaxDefinitionCategoryColor(categoryName,color);
			GetApp().NVI_RefreshAllWindow();
			break;
		}
	  case kCategoryImportColor:
		{
			AIndex sel = NVI_GetListView(kListView_CategoryColor).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			//
			AText	categoryName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetCategoryNameArray().Get(sel,categoryName);
			AColor	color;
			NVI_GetControlColor(kCategoryImportColor,color);
			GetApp().SPI_GetModePrefDB(mModeIndex).SetSyntaxDefinitionCategoryImportColor(categoryName,color);
			GetApp().NVI_RefreshAllWindow();
			break;
		}
	  case kCategoryLocalColor:
		{
			AIndex sel = NVI_GetListView(kListView_CategoryColor).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			//
			AText	categoryName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetCategoryNameArray().Get(sel,categoryName);
			AColor	color;
			NVI_GetControlColor(kCategoryLocalColor,color);
			GetApp().SPI_GetModePrefDB(mModeIndex).SetSyntaxDefinitionCategoryLocalColor(categoryName,color);
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//R0195
	  case kCheck_SyntaxCategoryBold:
		{
			AIndex sel = NVI_GetListView(kListView_CategoryColor).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			//
			AText	categoryName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetCategoryNameArray().Get(sel,categoryName);
			GetApp().SPI_GetModePrefDB(mModeIndex).SetSyntaxDefinitionCategoryBold(categoryName,NVI_GetControlBool(kCheck_SyntaxCategoryBold));
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//R0195
	  case kCheck_SyntaxCategoryItalic:
		{
			AIndex sel = NVI_GetListView(kListView_CategoryColor).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			//
			AText	categoryName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetCategoryNameArray().Get(sel,categoryName);
			GetApp().SPI_GetModePrefDB(mModeIndex).SetSyntaxDefinitionCategoryItalic(categoryName,NVI_GetControlBool(kCheck_SyntaxCategoryItalic));
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//R0195
	  case kCheck_SyntaxCategoryUnderline:
		{
			AIndex sel = NVI_GetListView(kListView_CategoryColor).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			//
			AText	categoryName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetCategoryNameArray().Get(sel,categoryName);
			GetApp().SPI_GetModePrefDB(mModeIndex).SetSyntaxDefinitionCategoryUnderline(categoryName,NVI_GetControlBool(kCheck_SyntaxCategoryUnderline));
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		*/
	  case kPopup_KeybindAction:
		{
			if( mKeybindMenuOrder.Get(NVI_GetControlNumber(kPopup_KeybindAction)) == keyAction_string )
			{
				NVI_SetControlEnable(kText_KeybindActionText,true);
				NVI_SetControlEnable(kButton_KeyBindTextOK,true);
				NVI_SwitchFocusTo(kText_KeybindActionText);
			}
			result = false;
			break;
		}
	  case kText_ToolName:
		{
			//B0406 ApplyToolName();
			mToolEditManager.SaveToolName();
			break;
		}
		//#305 ������I��
	  case kPopup_Tool_NameLanguage:
		{
			//���݂̌���ł̃f�[�^��ModePrefDB�֕ۑ�
			SaveMultiLanguageToolName();
			//mCurrentToolNameLanguage���X�V
			mCurrentToolNameLanguage = GetLaunguageFromMenuItemIndex(NVI_GetControlNumber(kPopup_Tool_NameLanguage));
			//�e�L�X�g�{�b�N�X�X�V
			DisplayMultiLanguageToolName();
			break;
		}
	  case kPopup_ModeNameLanguage:
		{
			mCurrentModeNameLanguage = GetLaunguageFromMenuItemIndex(NVI_GetControlNumber(kPopup_ModeNameLanguage));
			DisplayMultiLanguageModeName();
			break;
		}
		//#305 ������c�[������ModePrefDB�֕ۑ�
	  case kText_Tool_MultiLanguageName:
		{
			SaveMultiLanguageToolName();
			break;
		}
	  case kText_ModeMultiLanguageName:
		{
			SaveMultiLanguageModeName();
			break;
		}
		//#427 ���[�h���i���[�h�t�@�C�����j�ύX
	  case kModeFileName:
		{
			//���݂̃��[�h�t�@�C�����擾
			AText	currentModename;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetModeRawName(currentModename);
			//UI�Őݒ肵�����[�h�����擾
			AText	modename;
			NVI_GetControlText(kModeFileName,modename);
			//�ύX���Ȃ����break
			if( modename.Compare(currentModename) == true )   break;
			//���[�h���ύX���s
			if( GetApp().SPI_GetModePrefDB(mModeIndex).ChangeModeFileName(modename) == false )
			{
				//���[�h���ύX�ł��Ȃ������ꍇ�́AUI�Ɍ��̃��[�h����ݒ�
				GetApp().SPI_GetModePrefDB(mModeIndex).GetModeRawName(modename);
				NVI_SetControlText(kModeFileName,modename);
			}
			break;
		}
		/*#844
		//#427
	  case kModeUpdateURLMenu:
		{
			AText	url;
			GetApp().SPI_GetModeUpdateURL(NVI_GetControlNumber(kModeUpdateURLMenu),url);
			NVI_SetControlText(AModePrefDB::kModeUpdateURL,url);
			GetApp().SPI_GetModePrefDB(mModeIndex).SetData_Text(AModePrefDB::kModeUpdateURL,url);
			break;
		}
		*/
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

//R0000
void	AWindow_ModePref::MakeOldDataCopyInTrash( const AConstCharPtr inFileName, const AText& inExportText )
{
	AFileAcc	appPrefFolder;
	AFileWrapper::GetAppPrefFolder(appPrefFolder);
	AText	oldDataFileName;
	oldDataFileName.SetLocalizedText(inFileName);
	oldDataFileName.Add('_');
	AText	modeName;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeDisplayName(modeName);
	oldDataFileName.AddText(modeName);
	AText	datetimetext;
	ADateTimeWrapper::GetDateTimeTextForFileName(datetimetext);
	oldDataFileName.AddText(datetimetext);
	AFileAcc	oldDataFile;
	oldDataFile.SpecifyUniqChildFile(appPrefFolder,oldDataFileName);
	oldDataFile.CreateFile();
	oldDataFile.WriteFile(inExportText);
	ALaunchWrapper::MoveToTrash(oldDataFile);
}

ABool	AWindow_ModePref::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inID)
	{
		/*#844
	  case kButton_CurrentWindowSize:
		{
			AWindowID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				ARect	rect;
				GetApp().NVI_GetWindowByID(winID).NVI_GetWindowBounds(rect);
				NVI_SetControlNumber(AModePrefDB::kDefaultWindowWidth,rect.right-rect.left);
				NVI_SetControlNumber(AModePrefDB::kDefaultWindowHeight,rect.bottom-rect.top);
				NVI_DoValueChanged(AModePrefDB::kDefaultWindowWidth);
				NVI_DoValueChanged(AModePrefDB::kDefaultWindowHeight);
			}
			break;
		}
		*/
		//#868
		//�W�����[�h�́u���[�h�v�^�u���J��
	  case kButton_OpenNormalMode_ModeTab:
		{
			GetApp().SPI_ShowModePrefWindow(0);
			GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(0);
			break;
		}
		//#868
		//�W�����[�h�́u�\���v�^�u���J��
	  case kButton_OpenNormalMode_DisplayTab:
		{
			GetApp().SPI_ShowModePrefWindow(0);
			GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(1);
			break;
		}
		//#868
		//�W�����[�h�́u�ҏW�v�^�u���J��
	  case kButton_OpenNormalMode_EditTab:
		{
			GetApp().SPI_ShowModePrefWindow(0);
			GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(2);
			break;
		}
		//#868
		//�W�����[�h�́u�F�v�^�u���J��
	  case kButton_OpenNormalMode_ColorsTab:
		{
			GetApp().SPI_ShowModePrefWindow(0);
			GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(11);
			break;
		}
		//#868
		//���[�h����t�@�C���p�X�ǉ��{�^��
	  case kButton_ModeSelectAdd:
		{
			AFileAcc	defaultfolder;
			GetApp().SPI_GetMostFrontDocumentParentFolder(defaultfolder);
			AText	message;
			message.SetLocalizedText("ChooseFolderMessage_ModeSelectFilePath");
			NVI_ShowChooseFolderWindow(defaultfolder,message,kButton_ModeSelectAdd,true);
			break;
		}
		/*#868
	  case kMenu_ModeSelect:
		{
			//B0303
			AText	modeName;
			NVI_GetControlText(kMenu_ModeSelect,modeName);
			AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeName(modeName);
			if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
			if( SwitchMode(modeIndex) == false )
			{
				//�L�[���[�h���ۑ��E�c�[�����ۑ��̏ꍇ�͌��̃��[�h�I���ɖ߂�
				AText	modeName;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetModeName(modeName);
				NVI_SetControlText(kMenu_ModeSelect,modeName);
			}
			break;
		}
		*/
	  case kButton_BackgroundImageFile:
		{
			AText	path;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kBackgroundImageFile,path);
			AFileAcc	defaultFile;
			defaultFile.SpecifyWithAnyFilePathType(path);//B0389 OK,kFilePathType_1);
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_ModePrefBackgroundImageFile");
			NVI_ShowChooseFileWindow(defaultFile,message,kButton_BackgroundImageFile);
			break;
		}
		//
	  case kButton_KeywordSave:
		{
			mKeywordEditManager.Save();
			//�L�[���[�h���X�g(AIdentifierList)�̍X�V
			GetApp().SPI_GetModePrefDB(mModeIndex).UpdateUserKeywordCategoryAndKeyword();//#890
			GetApp().NVI_RefreshAllWindow();//#427
			break;
		}
		//#8
	  case kButton_KeywordRevert:
		{
			mKeywordEditManager.Revert();
			break;
		}
	  case kButton_KeywordSort:
		{
			mKeywordEditManager.Sort();
			break;
		}
		//
	  case kTable_KeywordCategory:
		{
			//�L�[���[�h���ۑ��̏ꍇ�͕ۑ��_�C�A���O���o���āA���̃J�e�S���[�ɖ߂�
			/*B0406 if( mKeywordTextDirty == true )
			{
				NVI_SetControlNumber(kTable_KeywordCategory,mCurrentKeywordCategoryIndex);
				ShowSaveKeywordAlertSheet();
			}
			else*/
			if( mKeywordEditManager.TryClose() == true )
			{
				//TryClose()�̂ق��ōs�I���͏C���ς�
				result = false;
			}
			break;
		}
		//B0000
	  case kButton_KeywordAddCategory:
	  case kButton_KeywordCategory_Up:
	  case kButton_KeywordCategory_Down:
	  case kButton_KeywordCategory_Top:
	  case kButton_KeywordCategory_Bottom:
		{
			//�L�[���[�h���ۑ��̏ꍇ�͕ۑ��_�C�A���O���o��
			/*B0406 if( mKeywordTextDirty == true )
			{
				ShowSaveKeywordAlertSheet();
			}
			else*/
			if( mKeywordEditManager.TryClose() == true )
			{
				result = false;//AWindow�ɏ��������邽��
			}
			break;
		}
		//B0406
	  case kButton_KeywordRemoveCategoryOK:
		{
			/*B0406 mKeywordTextDirty = false;
			NVI_SetModified(false);*/
			mKeywordEditManager.TryClose(true);
			result = false;//AWindow�ɏ��������邽��
			break;
		}
		//���@���f
		//#349
	  case kButton_UpdateSyntaxDefinition2:
		{
			//���ꂪ�Ȃ��ƁA���f�{�^���N���b�N��A�ݒ肪���ɖ߂��Ă��܂��̂ŁE�E�E#391
			EVT_ValueChanged(AModePrefDB::kSystemHeaderFiles);
			EVT_ValueChanged(AModePrefDB::kUserDefinedSDFFilePath);
			
			/*#868
			//
			NVI_GetListView(kListView_StateColor).SPI_SetSelect(kIndex_Invalid);
			NVI_GetListView(kListView_CategoryColor).SPI_SetSelect(kIndex_Invalid);
			*/
			//#890 LoadOrWaitLoadComplete()���Ŏ��s GetApp().SPI_GetImportFileManager().DeleteAllAndStopRecognizer();
			
			//���[�h�ݒ胊���[�h
			GetApp().SPI_GetModePrefDB(mModeIndex).SaveToFile();//���܂ł̐ݒ���e��ۑ�
			GetApp().SPI_GetModePrefDB(mModeIndex).LoadOrWaitLoadComplete();//���[�h
			//�c�[�����X�g�̃f�[�^������
			mCurrentToolMenuObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuRootObjectID();
			mToolMenuObjectIDArray.DeleteAll();
			mToolMenuPathArray.DeleteAll();
			mToolMenuDisplayPathArray.DeleteAll();
			//
			const ASyntaxDefinition&	syntaxDef = GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition();
			if( syntaxDef.GetParseResult() == false )
			{
				AIndex	errorLineIndex = syntaxDef.GetErrorLineNumber();
				AText	mes1,mes2;
				mes1.SetLocalizedText("SDFError1");
				mes2.SetLocalizedText("SDFError2");
				AText	lineNumber;
				lineNumber.SetNumber(errorLineIndex+1);
				mes2.ReplaceParamText('0',lineNumber);
				AText	errorType;
				syntaxDef.GetErrorText(errorType);
				mes2.ReplaceParamText('1',errorType);
				NVI_ShowChildWindow_OK(mes1,mes2);
			}
			GetApp().NVI_UpdateAll();
			UpdateValidSyntaxPartMenu();//���@�p�[�g���j���[�X�V
			NVI_UpdateProperty();
			NVM_UpdateControlStatus();//#997
			break;
		}
		//#349
	  case kButton_SelectUserDefinedSDFFile:
		{
			AText	path;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kUserDefinedSDFFilePath,path);
			AFileAcc	defaultFile;
			defaultFile.SpecifyWithAnyFilePathType(path);
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_SelectUserSDF");
			NVI_ShowChooseFileWindow(defaultFile,message,kButton_SelectUserDefinedSDFFile);
			break;
		}
		
		//�L�[�o�C���h modifier�`�F�b�N�{�b�N�X�N���b�N��
	  case kCheck_Keybind_ControlKey:
	  case kCheck_Keybind_OptionKey:
	  case kCheck_Keybind_CommandKey:
	  case kCheck_Keybind_ShiftKey:
		{
			//#688
			//���݂̑}���������ۑ�����i�I�[�v�������Ƃ���control�L�[���̏�Ԃ֕ۑ������j
			NVM_SaveContentView(kTable_Keybind);
			//
			NVM_UpdateDBTableView(kTable_Keybind);
			NVI_UpdateProperty();
			break;
		}
		//
	  case kButton_KeyBindTextOK:
		{
			NVM_SaveContentView(kTable_Keybind);
			break;
		}
		/*#868
	  case kListView_StateColor:
		{
			AIndex sel = NVI_GetListView(kListView_StateColor).SPI_GetSelectedDBIndex();
			//
			if( sel == kIndex_Invalid )   break;
			//���X�g�I�����ꂽ�̂�ColorPicker�R���g���[���ɐF�𔽉f
			//sel�ɑΉ������Ԗ����擾
			ATextArray	nameArray, displayNameArray;
			AArray<AColor>	colorArray;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetStateColorArray(nameArray,colorArray,displayNameArray);
			AText	name;
			nameArray.Get(sel,name);
			//��Ԗ��ɑΉ�����F���擾
			AColor	color;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinitionStateColorByName(name,color);
			NVI_SetControlColor(kStateColor,color);
			break;
		}
	  case kListView_CategoryColor:
		{
			AIndex sel = NVI_GetListView(kListView_CategoryColor).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			//
			AColor	color, importcolor, localcolor;
			AText	categoryName;
			ATextStyle	menustyle;
			ABool	styleBold, styleItalic, styleUnderline;//R0195
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetCategoryNameArray().Get(sel,categoryName);
			GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinitionCategoryColorByName(categoryName,color,importcolor,localcolor,menustyle,
					styleBold,styleItalic,styleUnderline);//R0195
			NVI_SetControlColor(kCategoryColor,color);
			NVI_SetControlColor(kCategoryImportColor,importcolor);
			NVI_SetControlColor(kCategoryLocalColor,localcolor);
			NVI_SetControlBool(kCheck_SyntaxCategoryBold,styleBold);//R0195
			NVI_SetControlBool(kCheck_SyntaxCategoryItalic,styleItalic);//R0195
			NVI_SetControlBool(kCheck_SyntaxCategoryUnderline,styleUnderline);//R0195
			break;
		}
		*/
		//�c�[��
	  case kListView_Tool:
		{
			//�c�[�����ۑ��̏ꍇ�͕ۑ��_�C�A���O���o���āA���̃c�[���ɖ߂� B0303
			/*B0406
			if( SwitchToolContent() == false )
			{
				SelectToolListRow(mCurrentToolSelectIndex);
				break;
			}
			
			AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
			mCurrentToolSelectIndex = sel;//B0303
			UpdateToolContent();//B0321
			*/
			if( mToolEditManager.TryClose() == false )
			{
				//TryClose()�̂ق��ōs�I���͏C���ς�
				break;
			}
			DoToolListRowSelected();
			break;
		}
	  case kCheck_Tool_ShortcutControlKey:
	  case kCheck_Tool_ShortcutShiftKey:
		{
			/*B0406 AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			if( mToolbarMode == false )
			{
				//
				AText	path;
				GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(mCurrentToolMenuObjectID,sel,path);
				AFileAcc	file;
				file.Specify(path);
				//
				AText	shortcuttext;
				NVI_GetControlText(kText_Tool_Shortcut,shortcuttext);
				ANumber	shortcut = NULL;
				if( shortcuttext.GetItemCount() > 0 )
				{
					shortcut = shortcuttext.Get(0);
				}
				ABool	controlKey = NVI_GetControlBool(kCheck_Tool_ShortcutControlKey);
				ABool	shiftKey = NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey);
				AMenuShortcutModifierKeys	modifiers = AKeyWrapper::MakeMenuShortcutModifierKeys(controlKey,false,shiftKey);
				GetApp().NVI_GetMenuManager().SetDynamicMenuItemShortcutByObjectID(mCurrentToolMenuObjectID,sel,shortcut,modifiers);
				//
				GetApp().SPI_GetModePrefDB(mModeIndex).WriteTool_LegacyShortcutData(file,shortcut,shiftKey,controlKey);
			}*/
			mToolEditManager.SaveShortcut();
			break;
		}
		//
	  case kButton_ToolParentFolder:
		{
			//#868 if( mToolbarMode == false )
			{
				if( mToolMenuObjectIDArray.GetItemCount() > 0 )
				{
					//�c�[���t�H���_�擾
					AFileAcc	folder;
					GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFolderFromMenuObjectID(mCurrentToolMenuObjectID,folder);
					//B0303 
					if( /*B0406 SwitchToolContent()*/mToolEditManager.TryClose() == false )
					{
						break;
					}
					//B0303 ���X�g�I������
					SelectToolListRow(kIndex_Invalid);
					//�e�t�H���_�ֈړ�
					mCurrentToolMenuObjectID = mToolMenuObjectIDArray.Get(mToolMenuObjectIDArray.GetItemCount()-1);
					mToolMenuObjectIDArray.Delete1(mToolMenuObjectIDArray.GetItemCount()-1);
					mToolMenuPathArray.Delete1(mToolMenuPathArray.GetItemCount()-1);
					mToolMenuDisplayPathArray.Delete1(mToolMenuDisplayPathArray.GetItemCount()-1);
					NVI_UpdateProperty();
					//�t�H���_��I��
					AIndex	arrayIndex = kIndex_Invalid, itemIndex = kIndex_Invalid;
					GetApp().SPI_GetModePrefDB(mModeIndex).FindToolIndexByFile(folder,arrayIndex,itemIndex);
					SelectToolListRow(itemIndex);
				}
			}
			break;
		}
		//
	  case kButton_ToolSave:
		{
			/*B0406
			AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			AText	text;
			NVI_GetControlText(kText_Tool,text);
			text.ConvertLineEndToLF();
			//�c�[��
			if( mToolbarMode == false )
			{
				AText	path;
				GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(mCurrentToolMenuObjectID,sel,path);
				AFileAcc	file;
				file.Specify(path);
				//B0310 file.WriteFile(text);
				GetApp().SPI_WriteTextFile(text,file);//B0310
			}
			//�c�[���o�[
			else
			{
				AToolbarItemType	type;
				AFileAcc	file;
				AIconID	iconID;
				AText	name;
				ABool	enabled;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarItem(sel,type,file,iconID,name,enabled);
				//B0310 file.WriteFile(text);
				GetApp().SPI_WriteTextFile(text,file);//B0310
			}
			SetToolTextDirty(false);//B0320
			NVI_SetModified(false);
			*/
			mToolEditManager.SaveText();
			break;
		}
		//#8
	  case kButton_ToolRevert:
		{
			mToolEditManager.RevertText();
			break;
		}
#if 0
		//
	  case kToolbarMode:
		{
			//B0303 
			if( /*B0406 SwitchToolContent()*/mToolEditManager.TryClose() == false )
			{
				//�c�[�����[�h�ɖ߂�
				NVI_SetControlBool(kToolMode,true);
				break;
			}
			//�c�[���o�[���[�h�֕ύX
			mToolbarMode = true;
			SelectToolListRow(kIndex_Invalid);
			NVI_UpdateProperty();
			break;
		}
		//
	  case kToolMode:
		{
			//B0303 
			if( /*SwitchToolContent()*/mToolEditManager.TryClose() == false )
			{
				//�c�[���o�[���[�h�ɖ߂�
				NVI_SetControlBool(kToolbarMode,true);//B0406
				break;
			}
			//�c�[�����[�h�֕ύX
			mToolbarMode = false;
			SelectToolListRow(kIndex_Invalid);
			NVI_UpdateProperty();
			break;
		}
#endif
		//�c�[���L���^�����`�F�b�N�{�b�N�X
	  case kButton_Tool_Display:
		{
			//#447 AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
			//#447 if( sel == kIndex_Invalid )   break;
			AArray<AIndex>	selectedDBIndexArray;//#447
			NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndexArray(selectedDBIndexArray);//#447
			if( selectedDBIndexArray.GetItemCount() == 0 )   break;//#447
			/*#868 if( mToolbarMode == true )
			{
				for( AIndex i = 0; i < selectedDBIndexArray.GetItemCount(); i++ )//#447
				{
					AIndex	sel = selectedDBIndexArray.Get(i);//#447
					GetApp().SPI_GetModePrefDB(mModeIndex).SetEnableToolbarItem(sel,NVI_GetControlBool(kButton_Tool_Display));
				}
				//#232 NVI_UpdateProperty();
			}
			else//R0173
			*/
			{
				for( AIndex i = 0; i < selectedDBIndexArray.GetItemCount(); i++ )//#447
				{
					AIndex	sel = selectedDBIndexArray.Get(i);//#447
					GetApp().SPI_GetModePrefDB(mModeIndex).SetEnableToolItem(mCurrentToolMenuObjectID,sel,NVI_GetControlBool(kButton_Tool_Display));
				}
			}
			break;
		}
		//#129
		//�c�[���O���[�\���`�F�b�N�{�b�N�X
	  case kButton_Tool_Grayout:
		{
			//#447 AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
			//#447 if( sel == kIndex_Invalid )   break;
			AArray<AIndex>	selectedDBIndexArray;//#447
			NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndexArray(selectedDBIndexArray);//#447
			if( selectedDBIndexArray.GetItemCount() == 0 )   break;//#447
			//#868 if( mToolbarMode == false )
			{
				for( AIndex i = 0; i < selectedDBIndexArray.GetItemCount(); i++ )//#447
				{
					AIndex	sel = selectedDBIndexArray.Get(i);//#447
					GetApp().SPI_GetModePrefDB(mModeIndex).SetGrayoutToolItem(mCurrentToolMenuObjectID,sel,
							NVI_GetControlBool(kButton_Tool_Grayout));
				}
			}
			break;
		}
		//�c�[���폜�{�^��
	  case kButton_ToolRemove:
		{
			AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("ModePref_DeleteToolRemove1");
			mes2.SetLocalizedText("ModePref_DeleteToolRemove2");
			mes3.SetLocalizedText("ModePref_DeleteToolRemove3");
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kButton_ToolRemoveOK);
			break;
		}
		//�c�[���폜OK�{�^��
	  case kButton_ToolRemoveOK:
		{
			//#237 AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
			//#237 if( sel == kIndex_Invalid )   break;
			AArray<AIndex>	selectedDBIndexArray;//#237
			NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndexArray(selectedDBIndexArray);//#237
			if( selectedDBIndexArray.GetItemCount() == 0 )   break;//#237
			//B0406 ����
			mToolEditManager.TryClose(true);
			//
			//#868 if( mToolbarMode == false )
			{
				/*B0406 R0173//
				AText	path;
				GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(mCurrentToolMenuObjectID,sel,path);
				AFileAcc	file;
				file.Specify(path);
				AFileWrapper::MoveToTrash(file);
				//
				GetApp().NVI_GetMenuManager().DeleteDynamicMenuItemByObjectID(mCurrentToolMenuObjectID,sel);*/
				//#237 GetApp().SPI_GetModePrefDB(mModeIndex).DeleteToolItemAndFile(mCurrentToolMenuObjectID,sel);
				//#237 �����폜
				while( selectedDBIndexArray.GetItemCount() > 0 )
				{
					AIndex	index = selectedDBIndexArray.Get(0);
					//StaticText���ځE�����X�V���ڂ͍폜�ΏۊO #427
					if( GetApp().SPI_GetModePrefDB(mModeIndex).GetToolIsStaticText(mCurrentToolMenuObjectID,index) == true ||
								GetApp().SPI_GetModePrefDB(mModeIndex).GetToolAutoUpdateFlag(mCurrentToolMenuObjectID,index) == true )
					{
						selectedDBIndexArray.Delete1(0);
						continue;
					}
					//
					GetApp().SPI_GetModePrefDB(mModeIndex).DeleteToolItemAndFile(mCurrentToolMenuObjectID,index);
					selectedDBIndexArray.Delete1(0);
					for( AIndex i = 0; i < selectedDBIndexArray.GetItemCount(); i++ )
					{
						if( selectedDBIndexArray.Get(i) >= index )
						{
							selectedDBIndexArray.Set(i,selectedDBIndexArray.Get(i)-1);
						}
					}
				}
			}
#if 0
			else
			{
				//#237 GetApp().SPI_GetModePrefDB(mModeIndex).DeleteToolbarItem(sel);
				//#237 �����폜
				while( selectedDBIndexArray.GetItemCount() > 0 )
				{
					AIndex	index = selectedDBIndexArray.Get(0);
					//�����X�V���ڂ͍폜�ΏۊO #427
					if( GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarAutoUpdateFlag(index) == true )
					{
						selectedDBIndexArray.Delete1(0);
						continue;
					}
					//
					GetApp().SPI_GetModePrefDB(mModeIndex).DeleteToolbarItem(index);
					selectedDBIndexArray.Delete1(0);
					for( AIndex i = 0; i < selectedDBIndexArray.GetItemCount(); i++ )
					{
						if( selectedDBIndexArray.Get(i) >= index )
						{
							selectedDBIndexArray.Set(i,selectedDBIndexArray.Get(i)-1);
						}
					}
				}
			}
#endif
			SelectToolListRow(kIndex_Invalid);
			NVI_UpdateProperty();
			//B0406
			//B0406 SetToolTextDirty(false);
			break;
		}
		//
	  case kButton_ToolAdd:
		{
			//B0321
			if( /*B0406 SwitchToolContent()*/mToolEditManager.TryClose() == false )
			{
				break;
			}
			
			//#868 if( mToolbarMode == false )
			{
				AText	text;
				mToolMenuPathArray.Implode('/',text);
				AFileAcc	toolfolder;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFolder(false,toolfolder);
				AFileAcc	folder;
				folder.SpecifyChild(toolfolder,text);
				AText	untitled;
				untitled.SetLocalizedText("UntitledDocumentName");
				AFileAcc	newFile;
				newFile.SpecifyUniqChildFile(folder,untitled);
				newFile.CreateFile();
				//win newFile.SetCreatorType('MMKE','TEXT');
				AFileAttribute	attr;
				GetApp().SPI_GetToolFileAttribute(attr);
				newFile.SetFileAttribute(attr);
				//�c�[���i���[�U�[�c�[���j�ǉ�
				AIndex	insertIndex = GetApp().SPI_GetModePrefDB(mModeIndex).GetUserToolInsertIndex(mCurrentToolMenuObjectID);
				GetApp().SPI_GetModePrefDB(mModeIndex).
						InsertToolItem(mCurrentToolMenuObjectID,insertIndex,newFile,true,true,false);//B0321
				NVI_UpdateProperty();
				//B0321
				SelectToolListRow(insertIndex);
				//B0406 UpdateToolContent();
			}
#if 0
			else
			{
				/*#199 mAddToolButtonWindow*/SPI_GetAddToolButtonWindow().NVI_CreateAndShow();
				/*#199 mAddToolButtonWindow*/SPI_GetAddToolButtonWindow().SPI_SetMode(mModeIndex,false);
				/*#199 mAddToolButtonWindow*/SPI_GetAddToolButtonWindow().NVI_SwitchFocusToFirst();
			}
#endif
			break;
		}
	  case kButton_AddToolFolder:
		{
			//B0321
			if( /*B0406 SwitchToolContent()*/mToolEditManager.TryClose() == false )
			{
				break;
			}
			
			//#868 if( mToolbarMode == false )
			{
				AText	text;
				mToolMenuPathArray.Implode('/',text);
				AFileAcc	toolfolder;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFolder(false,toolfolder);
				AFileAcc	folder;
				folder.SpecifyChild(toolfolder,text);
				AText	untitled;
				untitled.SetLocalizedText("UntitledDocumentName");
				AFileAcc	newFile;
				newFile.SpecifyUniqChildFile(folder,untitled);
				newFile.CreateFolder();
				//�t�H���_�ǉ�
				AIndex	insertIndex = GetApp().SPI_GetModePrefDB(mModeIndex).
						GetUserToolInsertIndex(mCurrentToolMenuObjectID);
				GetApp().SPI_GetModePrefDB(mModeIndex).
						InsertToolItem(mCurrentToolMenuObjectID,insertIndex,newFile,true,true,false);//B0321
				NVI_UpdateProperty();
				//B0321
				SelectToolListRow(insertIndex);
				//B0406 UpdateToolContent();
			}
			break;
		}
#if 0
	  case kButton_ChangeToolButtonIcon:
		{
			if( mToolbarMode == true )
			{
				AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
				if( sel == kIndex_Invalid )   break;
				AFileAcc	file;
				AToolbarItemType	type;
				//AIconID	iconID;
				AText	name;
				ABool	enabled;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarItem(sel,type,file,name,enabled);
				/*#199 mChangeToolButtonIconWindow*/SPI_GetChangeToolButtonIconWindow().NVI_CreateAndShow();
				/*#199 mChangeToolButtonIconWindow*/SPI_GetChangeToolButtonIconWindow().SPI_SetFile(file,mModeIndex);
				/*#199 mChangeToolButtonIconWindow*/SPI_GetChangeToolButtonIconWindow().NVI_SwitchFocusToFirst();
			}
			break;
		}
#endif
		//���o���ݒ蔽�f
	  case kButton_UpdateJumpSetup:
		{
			NVM_SaveContentView(kTable_JumpSetup);
			GetApp().SPI_GetModePrefDB(mModeIndex).SaveToFile();//���܂ł̐ݒ�f�[�^��ۑ�
			GetApp().SPI_GetModePrefDB(mModeIndex).LoadOrWaitLoadComplete();//���[�h
			//�c�[�����X�g�f�[�^������
			mCurrentToolMenuObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuRootObjectID();
			mToolMenuObjectIDArray.DeleteAll();
			mToolMenuPathArray.DeleteAll();
			mToolMenuDisplayPathArray.DeleteAll();
			break;
		}
		/*#683 �N���b�N���̍X�V�͕s�v�i�l�ύX����NVMDO_NotifyDataChanged()���R�[�������j
	  case kTable_Indent:
		{
			GetApp().SPI_UpdateIndentRegExpAll(mModeIndex);
			break;
		}
		*/
		//�J���[�X�L�[���f�[�^�G�N�X�|�[�g
	  case kButton_ExportColorScheme:
		{
			//���[�U�[�J���[�X�L�[���t�H���_�擾
			AFileAcc	userSchemeFolder;
			GetApp().SPI_GetUserColorSchemeFolder(userSchemeFolder);
			//�ۑ��_�C�A���O�\��
			AText	name;
			name.SetLocalizedText("Colors_ExportFileName");
			AFileAttribute	attr;
			GetApp().SPI_GetExportFileAttribute(attr);
			AText	kind;
			kind.SetCstring("colorscheme");
			AText	filter;
			GetApp().SPI_GetDefaultFileFilter(filter);
			NVI_ShowSaveWindow(name,kObjectID_Invalid,attr,filter,kind,userSchemeFolder);
			break;
		}
		//�L�[�o�C���h�C���|�[�g
	  case kButton_Keybind_Import:
		{
			AFileAcc	appFile;
			AFileWrapper::GetAppFile(appFile);
			AFileAcc	defaultFile;
			defaultFile.SpecifySister(appFile,"keybind");
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_ImportKeybind");
			NVI_ShowChooseFileWindow(defaultFile,message,kButton_Keybind_Import);
			break;
		}
		//�L�[�o�C���h�G�N�X�|�[�g
	  case kButton_Keybind_Export:
		{
			AText	name;
			name.SetLocalizedText("Keybind_ExportFileName");
			name.Add('_');
			AText	modeName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetModeDisplayName(modeName);
			name.AddText(modeName);
			AFileAttribute	attr;
			GetApp().SPI_GetExportFileAttribute(attr);
			AFileAcc	defaultfolder;
			AFileWrapper::GetDesktopFolder(defaultfolder);
			AText	kind;
			kind.SetCstring("keybindexport");
			AText	filter;//win 080709
			GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
			NVI_ShowSaveWindow(name,kObjectID_Invalid,attr,filter,kind,defaultfolder);
			break;
		}
		//DragDrop�C���|�[�g
	  case kButton_DragDropImport:
		{
			AFileAcc	appFile;
			AFileWrapper::GetAppFile(appFile);
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_ImportDragDrop");
			NVI_ShowChooseFileWindow(appFile,message,kButton_DragDropImport);
			break;
		}
		//DragDrop�G�N�X�|�[�g
	  case kButton_DragDropExport:
		{
			AText	name;
			name.SetLocalizedText("DragDrop_ExportFileName");
			name.Add('_');
			AText	modeName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetModeDisplayName(modeName);
			name.AddText(modeName);
			AFileAttribute	attr;
			//win AFileWrapper::GetDefaultFileAttribute(attr);
			//win attr.creator = 'MMKE';
			//win attr.type = 'TEXT';
			GetApp().SPI_GetExportFileAttribute(attr);
			AFileAcc	defaultfolder;
			AFileWrapper::GetDesktopFolder(defaultfolder);
			AText	kind;
			kind.SetCstring("dragdropexport");
			AText	filter;//win 080709
			GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
			NVI_ShowSaveWindow(name,kObjectID_Invalid,attr,filter,kind,defaultfolder);
			break;
		}
		//�Ή�������C���|�[�g
	  case kButton_CorrespondImport:
		{
			AFileAcc	appFile;
			AFileWrapper::GetAppFile(appFile);
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_CorrespondDragDrop");
			NVI_ShowChooseFileWindow(appFile,message,kButton_CorrespondImport);
			break;
		}
		//�Ή�������G�N�X�|�[�g
	  case kButton_CorrespondExport:
		{
			AText	name;
			name.SetLocalizedText("Correspond_ExportFileName");
			name.Add('_');
			AText	modeName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetModeDisplayName(modeName);
			name.AddText(modeName);
			AFileAttribute	attr;
			//win AFileWrapper::GetDefaultFileAttribute(attr);
			//win attr.creator = 'MMKE';
			//win attr.type = 'TEXT';
			GetApp().SPI_GetExportFileAttribute(attr);
			AFileAcc	defaultfolder;
			AFileWrapper::GetDesktopFolder(defaultfolder);
			AText	kind;
			kind.SetCstring("correspondexport");
			AText	filter;//win 080709
			GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
			NVI_ShowSaveWindow(name,kObjectID_Invalid,attr,filter,kind,defaultfolder);
			break;
		}
		//���K�\���C���f���g�C���|�[�g
	  case kButton_RegExpIndentImport:
		{
			AFileAcc	appFile;
			AFileWrapper::GetAppFile(appFile);
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_RegExpIndentDragDrop");
			NVI_ShowChooseFileWindow(appFile,message,kButton_RegExpIndentImport);
			break;
		}
		//���K�\���C���f���g�G�N�X�|�[�g
	  case kButton_RegExpIndentExport:
		{
			AText	name;
			name.SetLocalizedText("RegExpIndent_ExportFileName");
			name.Add('_');
			AText	modeName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetModeDisplayName(modeName);
			name.AddText(modeName);
			AFileAttribute	attr;
			//win AFileWrapper::GetDefaultFileAttribute(attr);
			//win attr.creator = 'MMKE';
			//win attr.type = 'TEXT';
			GetApp().SPI_GetExportFileAttribute(attr);
			AFileAcc	defaultfolder;
			AFileWrapper::GetDesktopFolder(defaultfolder);
			AText	kind;
			kind.SetCstring("regexpindentexport");
			AText	filter;//win 080709
			GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
			NVI_ShowSaveWindow(name,kObjectID_Invalid,attr,filter,kind,defaultfolder);
			break;
		}
		/*#844
		//
	  case kButton_AddNewMode:
		{
			//B0303
			if( TryCloseCurrentMode() == false )   break;
			
			SPI_GetAddNewModeWindow().NVI_CreateAndShow();
			SPI_GetAddNewModeWindow().NVI_SwitchFocusToFirst();
			break;
		}
		*/
		//
	  case kButton_RevealModeFolder:
		{
			AFileAcc	folder;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetModeFolder(folder);
			ALaunchWrapper::Reveal(folder);
			break;
		}
		//
	  case kButton_RevealToolFolder:
		{
			AFileAcc	folder;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFolder(false,folder);
			ALaunchWrapper::Reveal(folder);
			break;
		}
		//
	  case kButton_RevealToolbarFolder:
		{
			AFileAcc	folder;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(/*#844 false,*/folder);
			ALaunchWrapper::Reveal(folder);
			break;
		}
		//
	  case kButton_ReconfigWithToolFolder:
		{
			//B0303
			if( /*B0406 SwitchToolContent()*/mToolEditManager.TryClose() == false )   break;
			//B0303 ���X�g�I������
			SelectToolListRow(kIndex_Invalid);
			
			GetApp().SPI_GetModePrefDB(mModeIndex).LoadTool();
			mCurrentToolMenuObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuRootObjectID();
			mToolMenuObjectIDArray.DeleteAll();
			mToolMenuPathArray.DeleteAll();
			mToolMenuDisplayPathArray.DeleteAll();
			//GetApp().SPI_GetModePrefDB(mModeIndex).LoadToolbar();
			NVI_UpdateProperty();
			break;
		}
		//ColorScheme�t�H���_���J��
	  case kButton_RevealColorSchemeFolder:
		{
			AFileAcc	userSchemeFolder;
			GetApp().SPI_GetUserColorSchemeFolder(userSchemeFolder);
			ALaunchWrapper::Reveal(userSchemeFolder);
			break;
		}
		//ColorScheme�t�H���_���j���[�X�V
	  case kButton_UpdateColorSchemeMenu:
		{
			GetApp().SPI_UpdateColorSchemeMenu();
			break;
		}
		//
	  case kButton_NewFile:
		{
			GetApp().SPNVI_CreateNewTextDocument(mModeIndex);
			break;
		}
		//
	  case kButton_SyntaxDefColorsImport:
		{
			AFileAcc	appFile;
			AFileWrapper::GetAppFile(appFile);
			AText	message;
			NVI_ShowChooseFileWindow(appFile,message,kButton_SyntaxDefColorsImport);
			break;
		}
		//
	  case kButton_SyntaxDefColorsExport:
		{
			AText	name;
			name.SetLocalizedText("SyntaxDefColors_ExportFileName");
			name.Add('_');
			AText	modeName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetModeDisplayName(modeName);
			name.AddText(modeName);
			AFileAttribute	attr;
			//win AFileWrapper::GetDefaultFileAttribute(attr);
			//win attr.creator = 'MMKE';
			//win attr.type = 'TEXT';
			GetApp().SPI_GetExportFileAttribute(attr);
			AFileAcc	defaultfolder;
			AFileWrapper::GetDesktopFolder(defaultfolder);
			AText	kind;
			kind.SetCstring("sytandefcolorsexport");
			AText	filter;//win 080709
			GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
			NVI_ShowSaveWindow(name,kObjectID_Invalid,attr,filter,kind,defaultfolder);
			break;
		}
		//R0068
	  case kButton_KeywordsImport:
		{
			AFileAcc	appFile;
			AFileWrapper::GetAppFile(appFile);
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_ImportKeyword");
			NVI_ShowChooseFileWindow(appFile,message,kButton_KeywordsImport);
			break;
		}
		/*#844
		//R0068
	  case kButton_KeywordsExport:
		{
			AText	name;
			name.SetLocalizedText("Keywords_ExportFileName");
			name.Add('_');
			AText	modeName;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetModeName(modeName);
			name.AddText(modeName);
			AFileAttribute	attr;
			//win AFileWrapper::GetDefaultFileAttribute(attr);
			//win attr.creator = 'MMKE';
			//win attr.type = 'TEXT';
			GetApp().SPI_GetExportFileAttribute(attr);
			AFileAcc	defaultfolder;
			AFileWrapper::GetDesktopFolder(defaultfolder);
			AText	kind;
			kind.SetCstring("keywordsexport");
			AText	filter;//win 080709
			GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
			NVI_ShowSaveWindow(name,kObjectID_Invalid,attr,filter,kind,defaultfolder);
			break;
		}
		*/
		//R0000
	  case kButton_ModeBackup:
		{
			GetApp().SPI_GetModePrefDB(mModeIndex).ModeBackup(true);//#427 SaveModeBackup();
			break;
		}
		/*#844
	  case kButton_RevertToLaunchBackup:
		{
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("ModePref_Revert1");
			mes2.SetLocalizedText("ModePref_Revert2");
			mes3.SetLocalizedText("ModePref_Revert3");
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kButton_RevertToLaunchBackupOK);
			break;
		}
	  case kButton_RevertToLaunchBackupOK:
		{
			mKeywordEditManager.TryClose(true);
			if( GetApp().SPI_GetModePrefDB(mModeIndex).RevertToLaunchBackup() == true )
			{
				GetApp().SPI_ReSetModeAll(mModeIndex);//#349
				NVI_UpdateProperty();
			}
			else
			{
				AText	message1, message2;
				message1.SetLocalizedText("ModePref_RevertError1");
				message2.SetLocalizedText("ModePref_RevertError2");
				NVI_ShowChildWindow_OK(message1,message2);
			}
			break;
		}
		*/
		/*
		//
	  case kButton_TextPrefTestExport:
		{
			AFileAcc	folder;
			AFileWrapper::GetDesktopFolder(folder);
			AFileAcc	file;
			file.SpecifyChild(folder,"test.txt");
			GetApp().SPI_GetModePrefDB(mModeIndex).WriteToPrefTextFile(file);
			break;
		}
		//
	  case kButton_TextPrefTestImport:
		{
			AFileAcc	folder;
			AFileWrapper::GetDesktopFolder(folder);
			AFileAcc	file;
			file.SpecifyChild(folder,"test.txt");
			GetApp().SPI_GetModePrefDB(mModeIndex).LoadFromPrefTextFile(file);
			GetApp().SPI_RecalcAll(mModeIndex);
			NVI_UpdateProperty();
			break;
		}
		*/
		//#349
	  case kButton_SDFDebugMode:
		{
			GetApp().SPI_SetSDFDebugMode(NVI_GetControlBool(kButton_SDFDebugMode));
			break;
		}
		//#426
	  case kButton_OpenModeUpdatePage:
		{
			AText	url;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kModeUpdateURL,url);
			ALaunchWrapper::OpenURL(url);
			break;
		}
		//#427
	  case kModeUpateButton:
		{
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("ModePref_ModeUpdate1");
			mes2.SetLocalizedText("ModePref_ModeUpdate2");
			mes3.SetLocalizedText("ModePref_ModeUpdate3");
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kModeUpateButtonOK);
			break;
		}
		//#427
	  case kModeUpateButtonOK:
		{
			GetApp().SPI_ModeUpdate(mModeIndex,true);
			//���[�h�X�V���_�C�A���O�\��
			AText	message1, message2;
			message1.SetLocalizedText("ModePref_AutoUpdateNow1");
			message2.SetLocalizedText("ModePref_AutoUpdateNow2");
			NVI_SelectWindow();
			NVI_ShowChildWindow_OK(message1,message2);
			NVI_SetChildWindow_OK_EnableOKButton(false);
			break;
		}
		//#427 �z�z�t�@�C������
	  case kButton_CreateModeUpdateDistribution:
		{
			AFileAcc	defaultfolder;
			AFileWrapper::GetDesktopFolder(defaultfolder);
			AText	message;
			message.SetLocalizedText("");
			NVI_ShowChooseFolderWindow(defaultfolder,message,kButton_CreateModeUpdateDistribution,true);
			break;
		}
		//�L�[���[�hCSV�t�@�C���g�p���邵�Ȃ����W�I�{�^���N���b�N������
	  case kRadio_Keyword_NotUseCSVFile:
	  case kRadio_Keyword_UseCSVFile:
		{
			//CSV�g�p���ǂ�����ۑ�
			mKeywordEditManager.SaveUseCSV();
			//�L�[���[�h���X�g(AIdentifierList)�̍X�V
			GetApp().SPI_GetModePrefDB(mModeIndex).UpdateUserKeywordCategoryAndKeyword();//#890
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//�L�[���[�h�^�u�̍X�V�{�^��
	  case kButton_Keyword_UpdateDocument:
		{
			//
			NVM_SaveContentView(kTable_KeywordCategory);
			//�X�V�����i�ۑ��{�^���Ɠ����j
			mKeywordEditManager.Save();
			//�L�[���[�h���X�g(AIdentifierList)�̍X�V
			GetApp().SPI_GetModePrefDB(mModeIndex).UpdateUserKeywordCategoryAndKeyword();//#890
			GetApp().NVI_RefreshAllWindow();//#427
			break;
		}
		//CSV�t�@�C���I���{�^��
	  case kButton_Keyword_SelectCSVFile:
		{
			//���݂�CSV�t�@�C���p�X�擾
			AText	filepath;
			NVI_GetControlText(kEditBox_Keyword_CSVPath,filepath);
			AFileAcc	file;
			file.Specify(filepath);
			//�f�t�H���g�t�H���_�擾
			AFileAcc	folder;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetDataFolder(folder);//���݂�CSV�t�@�C���p�X�̃t�@�C�������݂��Ă��Ȃ����data�t�H���_
			if( file.Exist() == true )
			{
				folder.SpecifyParent(file);
			}
			//���b�Z�[�W�擾
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_KeywordCSVFile");
			//�t�@�C���I���E�C���h�E�\��
			NVI_ShowChooseFileWindow(folder,message,kButton_Keyword_SelectCSVFile);
			break;
		}
		//CSV�t�@�C�����J���{�^��
	  case kButton_OpenCSVFile:
		{
			//���݂�CSV�t�@�C���p�X�擾
			AText	filepath;
			NVI_GetControlText(kEditBox_Keyword_CSVPath,filepath);
			//�t�@�C�����擾
			AFileAcc	file;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetKeywordCSVFileFromPath(filepath,file);
			//�t�@�C�����J��
			ADocumentID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file);
			if( docID != kObjectID_Invalid && NVI_IsControlEnable(kEditBox_Keyword_CSVPath) == false )
			{
				GetApp().SPI_GetTextDocumentByID(docID).NVI_SetReadOnly(true);
			}
			break;
		}
		//�c�[���t�@�C�����J���{�^�� #868
	  case kButton_Tool_OpenToolFile:
		{
			//�c�[���t�@�C�����J���imi�܂��͑��A�v���j
			mToolEditManager.OpenToolWithApp();
			break;
		}
		//�A�v���g�ݍ��݃c�[�������[�U�[�c�[���ɃR�s�[
	  case kButton_Tool_CopyToUserTool:
		{
			//�I�����ڂ��擾
			AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
			if( sel == kIndex_Invalid )   break;
			
			//�I�����ڂ��A�v���g�ݍ��݃c�[���łȂ���Ή������Ȃ�
			ABool	autoupdate = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolAutoUpdateFlag(mCurrentToolMenuObjectID,sel);
			if( autoupdate == false )   break;
			
			//�c�[���ҏW��ʂ����
			if( mToolEditManager.TryClose() == false )   break;
			
			//���X�g�I������
			SelectToolListRow(kIndex_Invalid);
			
			//�c�[���t�@�C���擾
			AFileAcc	appTool;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFile(mCurrentToolMenuObjectID,sel,appTool);
			//�c�[�����擾
			AText	toolname;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetToolDisplayName(mCurrentToolMenuObjectID,sel,toolname);
			//�V���[�g�J�b�g�擾
			ANumber	shortcut = 0;
			AMenuShortcutModifierKeys	modifiers = 0;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetToolShortcut(mCurrentToolMenuObjectID,sel,shortcut,modifiers);
			
			//���[�U�[�c�[���t�H���_�擾
			AFileAcc	userToolFolder;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFolder(false,userToolFolder);
			//�I�����ڂ́A���[�g����̑��΃p�X���擾
			AText	relativePath;
			mToolMenuDisplayPathArray.Implode('/',relativePath);
			//���[�U�[�c�[���t�H���_���ɁA�������΃p�X�̃c�[���t�@�C�����쐬
			AFileAcc	userToolParentFolder;
			userToolParentFolder.SpecifyChild(userToolFolder,relativePath);
			AFileAcc	userTool;
			userTool.SpecifyChild(userToolParentFolder,toolname);
			if( userTool.Exist() == true )
			{
				//------------------���ɓ����c�[���t�@�C�������݂���ꍇ------------------
				//��
				
			}
			else
			{
				//------------------�����c�[���t�@�C���������݂̏ꍇ------------------
				//�t�H���_�𐶐�
				userToolParentFolder.CreateFolderRecursive();
				//�A�v���g�ݍ��݃c�[�������[�U�[�c�[���ɃR�s�[
				appTool.CopyFileTo(userTool,false);
				
				//�t�H���_����č\��
				GetApp().SPI_GetModePrefDB(mModeIndex).LoadTool();
				
				//�V���[�g�J�b�g�����[�U�[�c�[�����Ɉړ�
				GetApp().SPI_GetModePrefDB(mModeIndex).SetToolShortcut(appTool,0,0);
				GetApp().SPI_GetModePrefDB(mModeIndex).SetToolShortcut(userTool,shortcut,modifiers);
				
				//�Ώۃ��[�U�[�c�[���ɑ΂��ăc�[�����X�g�f�[�^���X�V
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuObjectIDFromFile(userTool,mCurrentToolMenuObjectID,mToolMenuObjectIDArray);
				mToolMenuPathArray.SetFromTextArray(mToolMenuDisplayPathArray);
				//�\���X�V
				NVI_UpdateProperty();
				
				//�c�[���ҏW���Open
				AIndex	arrayIndex = kIndex_Invalid;
				AIndex	toolIndex = kIndex_Invalid;
				GetApp().SPI_GetModePrefDB(mModeIndex).FindToolIndexByFile(userTool,arrayIndex,toolIndex);
				NVI_GetListView(kListView_Tool).SPI_SetSelectByDBIndex(toolIndex,true);
				mToolEditManager.Open(mModeIndex,mCurrentToolMenuObjectID,toolIndex);
				//�\���X�V
				NVI_UpdateProperty();
			}
			break;
		}
		/*test �L�[�o�C���h�ꗗ�E�C���h�E
	  case 999007:
		{
			GetApp().SPI_GetKeyActionListWindow().NVI_Hide();
			break;
		}
	  case 999008:
		{
			GetApp().SPI_GetKeyActionListWindow().NVI_Show();
			ARect	windowBounds = {100,100,300,800};
			GetApp().SPI_GetKeyActionListWindow().NVI_SetWindowBounds(windowBounds);
			break;
		}
		*/
		//
	  case kButton_ApplyColorScheme:
		{
			//���݂̐F��ۑ�
			GetApp().SPI_GetModePrefDB(mModeIndex).SaveLastColors();
			/*#1316
			//�J���[�X�L�[��db�X�V
			GetApp().SPI_GetModePrefDB(mModeIndex).UpdateColorScheme();
			//mode pref db�̊e�F���J���[�X�L�[������X�V
			GetApp().SPI_GetModePrefDB(mModeIndex).ApplyFromColorScheme();
			*/
			//�J���[�X�L�[�����烂�[�h�ݒ�F�փR�s�[ #1316
			AText	schemename;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kColorSchemeName,schemename);
			GetApp().SPI_GetModePrefDB(mModeIndex).ApplyFromColorScheme(schemename);
			//�F�X�V������
			UpdateColors();
			//�\���X�V
			NVI_UpdateProperty();
			break;
		}
		//
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

//#427
/**
�t�H���_�I��������
*/
void	AWindow_ModePref::EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder )
{
	switch(inControlID)
	{
	  case kButton_CreateModeUpdateDistribution:
		{
			GetApp().SPI_GetModePrefDB(mModeIndex).CreateModeUpdateDistribution(inFolder);
			break;
		}
		//#868
		//���[�h����t�@�C���p�X�I��������
	  case kButton_ModeSelectAdd:
		{
			//�p�X�擾
			AText	path;
			inFolder.GetPath(path);
			//DB�ɒǉ�
			GetApp().SPI_GetModePrefDB(mModeIndex).Add_TextArray(AModePrefDB::kModeSelectFilePath,path);
			//�e�[�u���X�V
			NVM_UpdateDBTableView(kTable_ModeSelectFilePath);
			break;
		}
	}
}

//B0303
//�i���[�U�[�N���b�N�ł͂Ȃ��j��������List�̍s��I������
void	AWindow_ModePref::SelectToolListRow( const AIndex inIndex )
{
	NVI_GetListView(kListView_Tool).SPI_SetSelect(inIndex);
	NVI_GetListView(kListView_Tool).SPI_AdjustScroll();
	DoToolListRowSelected();
}

//B0406
//List�s�I�����̏����iEVTDO_Clicked()���[�g�ƁASelectToolListRow()���[�g�j
void	AWindow_ModePref::DoToolListRowSelected()
{
	//�܂�����
	mToolEditManager.TryClose(true);
	//#237 �����I���̏ꍇ�͒��g�ݒ�Ȃ�
	AArray<AIndex>	selectedDBIndexArray;
	NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndexArray(selectedDBIndexArray);
	if( selectedDBIndexArray.GetItemCount() >= 2 )
	{
		//�����I���̏ꍇ�̏���
		
		//#447 NVI_SetControlEnable(kGroup_Tool,false);
		//#447
		AIndex	sel = selectedDBIndexArray.Get(0);
		/*#427 �O���C�A�E�g�����SPI_UpdateToolGrayout()�ł܂Ƃ߂�
		//�O���C�A�E�g����
		NVI_SetControlEnable(kGroup_Tool,true);
		NVI_SetControlEnable(kText_Tool,false);
		NVI_SetControlEnable(kText_Tool_Shortcut,false);
		NVI_SetControlEnable(kPopup_Tool_NameLanguage,false);
		NVI_SetControlEnable(kText_Tool_MultiLanguageName,false);
		NVI_SetControlEnable(kCheck_Tool_ShortcutControlKey,false);
		NVI_SetControlEnable(kCheck_Tool_ShortcutShiftKey,false);
		NVI_SetControlEnable(kButton_ChangeToolButtonIcon,false);
		NVI_SetControlEnable(kToolCaption1,false);//#427
		NVI_SetControlEnable(kToolCaption2,false);//#427
		NVI_SetControlEnable(kToolCaption3,false);//#427
		NVI_SetControlEnable(kToolCaption4,false);//#427
		*/
		ABool	enableGrayoutCheckbox = true;//#427
		ABool	enableDisplayCheckbox = true;//#427
		//#868 if( mToolbarMode == false )
		{
			//�c�[��
			
			//�L���`�F�b�N�{�b�N�X�̐ݒ�
			NVI_SetControlBool(kButton_Tool_Display,
					GetApp().SPI_GetModePrefDB(mModeIndex).IsToolEnabled(mCurrentToolMenuObjectID,sel));
			//#427 NVI_SetControlEnable(kButton_Tool_Display,true);
			//�O���C�A�E�g�`�F�b�N�{�b�N�X�̐ݒ�
			NVI_SetControlBool(kButton_Tool_Grayout,
					GetApp().SPI_GetModePrefDB(mModeIndex).IsToolGrayout(mCurrentToolMenuObjectID,sel));
			//#427 NVI_SetControlEnable(kButton_Tool_Grayout,true);
		}
		/*#868
		else
		{
			//�c�[���o�[
			
			AToolbarItemType	type;
			AFileAcc	file;
			//AIconID	iconID;
			AText	name;
			ABool	enabled;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarItem(sel,type,file,name,enabled);
			//�L���`�F�b�N�{�b�N�X�̐ݒ�
			NVI_SetControlBool(kButton_Tool_Display,enabled);
			//#427 NVI_SetControlEnable(kButton_Tool_Display,true);
			enableGrayoutCheckbox = false;//#427
		}
		*/
		//�O���C�A�E�g���� #427
		SPI_UpdateToolContentViewControlGrayout(false,false,false,enableGrayoutCheckbox,enableDisplayCheckbox,false);
		return;
	}
	//
	AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
	//mCurrentToolSelectIndex = sel;//B0303
	if( sel == kIndex_Invalid )
	{
		return;
	}
	//Open
	mToolEditManager.Open(mModeIndex,/*#868 mToolbarMode,*/mCurrentToolMenuObjectID,sel);
	/*#182
	AFileAcc	file;
	if( mToolbarMode == false )
	{
		//�c�[����ContentView�֊J��
		GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFile(mCurrentToolMenuObjectID,sel,file);
		mToolEditManager.Open(mModeIndex,false,file);
		//�L���`�F�b�N�{�b�N�X�̐ݒ�i���ꂾ���́AToolEditManager�Ǘ��O�j
		NVI_SetControlBool(kButton_Tool_Display,GetApp().SPI_GetModePrefDB(mModeIndex).IsToolEnabled(mCurrentToolMenuObjectID,sel));
		NVI_SetControlEnable(kButton_Tool_Display,true);
		//#129
		NVI_SetControlBool(kButton_Tool_Grayout,GetApp().SPI_GetModePrefDB(mModeIndex).IsToolGrayout(mCurrentToolMenuObjectID,sel));
		NVI_SetControlEnable(kButton_Tool_Grayout,true);
	}
	else
	{
		//�c�[���o�[��ContentView�֊J��
		AToolbarItemType	type;
		AFileAcc	file;
		AIconID	iconID;
		AText	name;
		ABool	enabled;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarItem(sel,type,file,iconID,name,enabled);
		switch(type)
		{
		  case kToolbarItemType_File:
		  case kToolbarItemType_Folder:
			{
				mToolEditManager.Open(mModeIndex,true,file);
				break;
			}
		  default:
			{
				//�L���`�F�b�N�{�b�N�X�����ݒ�ł���悤�ɂ���
				NVI_SetControlEnable(kGroup_Tool,true);
				NVI_SetControlText(kText_ToolName,name);
				NVI_SetControlText(kText_Tool,GetEmptyText());
				NVI_SetControlText(kText_Tool_Shortcut,GetEmptyText());
				NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,false);
				NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,false);
				NVI_SetControlBool(kCheck_AutoUpdate_Tool,false);//#427
				NVI_SetControlEnable(kText_ToolName,false);
				NVI_SetControlEnable(kText_Tool,false);
				NVI_SetControlEnable(kText_Tool_Shortcut,false);
				NVI_SetControlEnable(kPopup_Tool_NameLanguage,false);//#447
				NVI_SetControlEnable(kText_Tool_MultiLanguageName,false);//#305
				NVI_SetControlEnable(kCheck_Tool_ShortcutControlKey,false);
				NVI_SetControlEnable(kCheck_Tool_ShortcutShiftKey,false);
				NVI_SetControlEnable(kButton_ChangeToolButtonIcon,false);
				NVI_SetControlEnable(kCheck_AutoUpdate_Tool,false);//#427
				break;
			}
		}
		//�L���`�F�b�N�{�b�N�X�̐ݒ�i���ꂾ���́AToolEditManager�Ǘ��O�j
		NVI_SetControlBool(kButton_Tool_Display,enabled);
		NVI_SetControlEnable(kButton_Tool_Display,true);
		//#129
		NVI_SetControlBool(kButton_Tool_Grayout,false);
		NVI_SetControlEnable(kButton_Tool_Grayout,false);
	}
	*/
	//#305 ������c�[�������e�L�X�g�{�b�N�X�֕\��
	DisplayMultiLanguageToolName();
}

//
ABool	AWindow_ModePref::EVTDO_DoubleClicked( const AControlID inID )
{
	ABool	result = true;
	switch(inID)
	{
		//�c�[��
	  case kListView_Tool:
		{
			//#868 if( mToolbarMode == false )
			{
				AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
				if( sel != kIndex_Invalid )
				{
					//
					/*R0173 AText	path;
					GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(mCurrentToolMenuObjectID,sel,path);
					AFileAcc	file;
					file.Specify(path);*/
					AFileAcc	file;
					GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFile(mCurrentToolMenuObjectID,sel,file);
					//
					if( file.IsFolder() == true )
					{
						//==================�t�H���_���ֈړ�==================
						//�c�[���\�����擾
						AText	tooldisplayname;
						GetApp().SPI_GetModePrefDB(mModeIndex).GetToolDisplayName(mCurrentToolMenuObjectID,sel,tooldisplayname);
						
						//
						AObjectID	subMenuID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolSubMenuID(mCurrentToolMenuObjectID,sel);
						if( subMenuID != kObjectID_Invalid )
						{
							SelectToolListRow(kIndex_Invalid);
							mToolMenuObjectIDArray.Add(mCurrentToolMenuObjectID);
							mCurrentToolMenuObjectID = subMenuID;
							AText	filename;
							file.GetFilename(filename);
							mToolMenuPathArray.Add(filename);
							mToolMenuDisplayPathArray.Add(tooldisplayname);
							NVI_UpdateProperty();
						}
					}
					//#721
					//�_�u���N���b�N�Ńc�[���t�@�C�����J��
					else
					{
						//==================�c�[���t�@�C�����J���imi�܂��͑��A�v���j==================
						mToolEditManager.OpenToolWithApp();
					}
				}
			}
			break;
		}
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

//���[�h�ؑ�
void	AWindow_ModePref::SPI_SelectMode( const AModeIndex inModeIndex )
{
	//B0303
	//#868 SwitchMode(inModeIndex);
	_ACError("",NULL);
}

//B0303
ABool	AWindow_ModePref::SwitchMode( const AModeIndex inModeIndex )
{
	/*#868
	if( mModeIndex == inModeIndex )   return true;
	if( TryCloseCurrentMode() == false )   return false;
	//
	mModeIndex = inModeIndex;
	NVI_SetControlNumber(kMenu_ModeSelect,mModeIndex);
	mCurrentToolMenuObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuRootObjectID();
	NVI_UpdateProperty();
	//���[�h�ݒ�E�C���h�E�̕\��������邱�Ƃ�������̑΍�B
	NVI_RefreshWindow();
	return true;
	*/
	_ACError("",NULL);
	return false;
}

void	AWindow_ModePref::ShowSaveKeywordAlertSheet()
{
	AText	mes1, mes2, mes3;
	mes1.SetLocalizedText("ModePref_SaveKeywordSheet1");
	mes2.SetLocalizedText("ModePref_SaveKeywordSheet2");
	mes3.SetLocalizedText("ModePref_SaveKeywordSheet3");
	NVI_ShowChildWindow_OKCancelDiscard(mes1,mes2,mes3,kButton_KeywordSave,kButton_KeywordRevert);//#8
	NVI_SelectWindow();//B0303
}

void	AWindow_ModePref::ShowSaveToolAlertSheet()
{
	AText	mes1, mes2, mes3;
	mes1.SetLocalizedText("ModePref_SaveToolSheet1");
	mes2.SetLocalizedText("ModePref_SaveToolSheet2");
	mes3.SetLocalizedText("ModePref_SaveToolSheet3");
	NVI_ShowChildWindow_OKCancelDiscard(mes1,mes2,mes3,kButton_ToolSave,kButton_ToolRevert);//#8
	NVI_SelectWindow();//B0303
}

//�ۑ��_�C�A���O������M����
void	AWindow_ModePref::EVTDO_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText )
{
	if( inRefText.Compare("keybindexport") == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(mModeIndex).ExportKeyBindToText(text);
		inFile.WriteFile(text);
	}
	else if( inRefText.Compare("dragdropexport") == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(mModeIndex).ExportDragDropToText(text);
		inFile.WriteFile(text);
	}
	else if( inRefText.Compare("correspondexport") == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(mModeIndex).ExportCorrespond(text);
		inFile.WriteFile(text);
	}
	else if( inRefText.Compare("regexpindentexport") == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(mModeIndex).ExportRegExpIndent(text);
		inFile.WriteFile(text);
	}
	else if( inRefText.Compare("colorscheme") == true )
	{
		GetApp().SPI_GetModePrefDB(mModeIndex).ExportColors(inFile);
	}
	/*#844
	//R0068
	else if( inRefText.Compare("keywordsexport") == true )
	{
		AText	text;
		GetApp().SPI_GetModePrefDB(mModeIndex).ExportKeywords(text);
		inFile.WriteFile(text);
	}
	*/
}

void	AWindow_ModePref::EVTDO_WindowDeactivated()
{
	NVM_SaveContentView(kTable_Keybind);
}

#pragma mark ===========================

#pragma mark ---������

//#305
/**
�c�[���̑�����^�C�g����ModePrefDB�֕ۑ��A���j���[�X�V�AToolPrefDB��������
*/
void	AWindow_ModePref::SaveMultiLanguageToolName()
{
	//���ݑI�𒆂̃c�[����Index���擾
	AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
	if( sel == kIndex_Invalid )   return;
	//#868 if( mToolbarMode == false )
	{
		//ModePrefDB�֕ۑ�
		AText	multiLangName;
		NVI_GetControlText(kText_Tool_MultiLanguageName,multiLangName);
		GetApp().SPI_GetModePrefDB(mModeIndex).SetToolMultiLanguageName(mCurrentToolMenuObjectID,sel,mCurrentToolNameLanguage,multiLangName);
	}
	/*#868
	else
	{
		//ModePrefDB�֕ۑ�
		AText	multiLangName;
		NVI_GetControlText(kText_Tool_MultiLanguageName,multiLangName);
		GetApp().SPI_GetModePrefDB(mModeIndex).SetToolbarMultiLanguageName(sel,mCurrentToolNameLanguage,multiLangName);
	}
	*/
	//���X�g�X�V
	NVI_UpdateProperty();
}

//#305
/**
�c�[���̑�����^�C�g�����e�L�X�g�{�b�N�X�֕\��
*/
void	AWindow_ModePref::DisplayMultiLanguageToolName()
{
	//���ݑI�𒆂̃c�[����Index���擾
	AIndex sel = NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex();
	if( sel == kIndex_Invalid )   return;
	//#868 if( mToolbarMode == false )
	{
		//ModePrefDB����擾
		AText	multiLangName;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMultiLanguageName(mCurrentToolMenuObjectID,sel,mCurrentToolNameLanguage,multiLangName);
		NVI_SetControlText(kText_Tool_MultiLanguageName,multiLangName);
	}
	/*#868
	else
	{
		//ModePrefDB����擾
		AText	multiLangName;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarMultiLanguageName(sel,mCurrentToolNameLanguage,multiLangName);
		NVI_SetControlText(kText_Tool_MultiLanguageName,multiLangName);
	}
	*/
}

//#305
/**
*/
void	AWindow_ModePref::SaveMultiLanguageModeName()
{
	//ModePrefDB�֕ۑ�
	AText	multiLangName;
	NVI_GetControlText(kText_ModeMultiLanguageName,multiLangName);
	GetApp().SPI_GetModePrefDB(mModeIndex).SetModeMultiLanguageName(mCurrentModeNameLanguage,multiLangName);
}

//#305
/**
���[�h�̑�����^�C�g�����e�L�X�g�{�b�N�X�֕\��
*/
void	AWindow_ModePref::DisplayMultiLanguageModeName()
{
	//ModePrefDB����擾
	AText	multiLangName;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeMultiLanguageName(mCurrentModeNameLanguage,multiLangName);
	NVI_SetControlText(kText_ModeMultiLanguageName,multiLangName);
}

//#305
/**
���j���[����index���猾����擾
*/
ALanguage	AWindow_ModePref::GetLaunguageFromMenuItemIndex( const AIndex inIndex ) const
{
	switch(inIndex)
	{
	  case 0:
		{
			return kLanguage_Japanese;
			break;
		}
	  case 1:
		{
			return kLanguage_English;
			break;
		}
	  case 2:
		{
			return kLanguage_French;
			break;
		}
	  case 3:
		{
			return kLanguage_German;
			break;
		}
	  case 4:
		{
			return kLanguage_Spanish;
			break;
		}
	  case 5:
		{
			return kLanguage_Italian;
			break;
		}
	  case 6:
		{
			return kLanguage_Dutch;
			break;
		}
	  case 7:
		{
			return kLanguage_Swedish;
			break;
		}
	  case 8:
		{
			return kLanguage_Norwegian;
			break;
		}
	  case 9:
		{
			return kLanguage_Danish;
			break;
		}
	  case 10:
		{
			return kLanguage_Finnish;
			break;
		}
	  case 11:
		{
			return kLanguage_Portuguese;
			break;
		}
	  case 12:
		{
			return kLanguage_SimplifiedChinese;
			break;
		}
	  case 13:
		{
			return kLanguage_TraditionalChinese;
			break;
		}
	  case 14:
		{
			return kLanguage_Korean;
			break;
		}
	  case 15:
		{
			return kLanguage_Polish;
			break;
		}
	  case 16:
		{
			return kLanguage_PortuguesePortugal;
			break;
		}
	  case 17:
		{
			return kLanguage_Russian;
			break;
		}
	  default:
		{
			return kLanguage_English;
			break;
		}
	}
}

//#305
/**
���ꂩ�烁�j���[����index���擾
*/
AIndex	AWindow_ModePref::GetMenuItemIndexFromLanguage( const ALanguage inLanguage ) const
{
	switch(inLanguage)
	{
	  case kLanguage_Japanese:
		{
			return 0;
			break;
		}
	  case kLanguage_English:
		{
			return 1;
			break;
		}
	  case kLanguage_French:
		{
			return 2;
			break;
		}
	  case kLanguage_German:
		{
			return 3;
			break;
		}
	  case kLanguage_Spanish:
		{
			return 4;
			break;
		}
	  case kLanguage_Italian:
		{
			return 5;
			break;
		}
	  case kLanguage_Dutch:
		{
			return 6;
			break;
		}
	  case kLanguage_Swedish:
		{
			return 7;
			break;
		}
	  case kLanguage_Norwegian:
		{
			return 8;
			break;
		}
	  case kLanguage_Danish:
		{
			return 9;
			break;
		}
	  case kLanguage_Finnish:
		{
			return 10;
			break;
		}
	  case kLanguage_Portuguese:
		{
			return 11;
			break;
		}
	  case kLanguage_SimplifiedChinese:
		{
			return 12;
			break;
		}
	  case kLanguage_TraditionalChinese:
		{
			return 13;
			break;
		}
	  case kLanguage_Korean:
		{
			return 14;
			break;
		}
	  case kLanguage_Polish:
		{
			return 15;
			break;
		}
	  case kLanguage_PortuguesePortugal:
		{
			return 16;
			break;
		}
	  case kLanguage_Russian:
		{
			return 17;
			break;
		}
	  default:
		{
			return 1;
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E�����iAWindow::Show()����R�[�������j
void AWindow_ModePref::NVIDO_Create( const ADocumentID inDocumentID )
{
	//#402 �N�����o�b�N�A�b�v�iAApp::InitModePref()���珈���^�C�~���O�ړ��j
	//#920 GetApp().SPI_CopyToLaunchBackup();
	
	//
	AText	appdefaultfontname;//#375
	AFontWrapper::GetAppDefaultFontName(appdefaultfontname);//#375
	
	//==================�E�C���h�E����==================
	NVM_CreateWindow("modepref/ModePref",kWindowClass_Document,true,13);//#117
	//�c�[���o�[����
	NVI_CreateToolbar(2);
	
	const ADataBase& modepref = GetApp().SPI_GetModePrefDB(mModeIndex);
	
	//���[�h�ݒ�E�C���h�E�^�C�g���ݒ�
	AText	wintitle;
	wintitle.SetLocalizedText("ModeSettingTitle");
	AText	modename;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeDisplayName(modename);
	wintitle.ReplaceParamText('0',modename);
	NVI_SetDefaultTitle(wintitle);
	
	//======================== �u���[�h�v �^�u ========================
	
	//���[�h�ݒ�L���`�F�b�N�{�b�N�X
	NVM_RegisterDBData(AModePrefDB::kEnableMode,						true);
	
	//�g���q�e�[�u���̐ݒ�
	NVM_RegisterDBTable(kTable_Suffix,AModePrefDB::kSuffix,
				kButton_SuffixAdd,kButton_SuffixRemove,kButton_SuffixRemoveOK,
				kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
				kControlID_Invalid,true,true,true,true,false);
	NVM_RegisterDBTableColumn(	kTable_Suffix,kColumn_Suffix_Suffix,					AModePrefDB::kSuffix,
							  kButton_EditSuffixTable,			0,"ModePref_Suffix_ListViewTitle0",true);
	//�g���q�Ȃ������� #1428
	AText	noSuffix;
	noSuffix.SetLocalizedText("ModePrefNoSuffix");
	NVM_SetInhibit0LengthForTable(kTable_Suffix,false,noSuffix);
	
	//�t�H���g���u�W���v���[�h�Ɠ����ɂ���ݒ�
	NVM_RegisterDBData(AModePrefDB::kSameAsNormalMode_Font,				true);
	
	//�e�L�X�g�t�H���g
	{
		AText	text;
		modepref.GetData_Text(AModePrefDB::kDefaultFontName,text);
		NVI_SetControlFont(kFontControl_DefaultFont,text,modepref.GetData_FloatNumber(AModePrefDB::kDefaultFontSize));
	}
	
	//���[�h���
	NVI_CreateEditBoxView(AModePrefDB::kModeComment,true,false,true);//#180
	NVI_GetEditBoxView(AModePrefDB::kModeComment).
    SPI_SetTextDrawProperty(appdefaultfontname,9.0);//#1316 ,kColor_Black,kColor_Black);//#180
	NVI_GetEditBoxView(AModePrefDB::kModeComment).NVI_SetCatchReturn(true);//#180
	NVI_GetEditBoxView(AModePrefDB::kModeComment).NVI_SetCatchTab(true);//#180
	NVI_GetEditBoxView(AModePrefDB::kModeComment).NVI_SetAutomaticSelectBySwitchFocus(false);
	NVI_GetEditBoxView(AModePrefDB::kModeComment).SPI_SetSelect00AfterSetText(true);
	NVM_RegisterDBData(AModePrefDB::kModeComment,						false);
	
	//#994
	//�v���O�C��
	NVM_RegisterDBTable(kTable_Plugin,AModePrefDB::kPluginData_Enabled,
			kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
			kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
			kControlID_Invalid,true,true,true,true,false);
	NVM_RegisterDBTableColumn(	kTable_Plugin,kColumnID_Plugin_Enabled,				AModePrefDB::kPluginData_Enabled,
								kControlID_Invalid,					32,"",true);
	NVM_RegisterDBTableColumn(	kTable_Plugin,kColumnID_Plugin_Name,				kDataType_TextArray,
								kControlID_Invalid,					256,"",false);
	NVM_RegisterDBTableColumn(	kTable_Plugin,kColumnID_Plugin_Options,				AModePrefDB::kPluginData_Options,
								kControlID_Invalid,					-1,"",true);
	NVM_SetInhibit0LengthForTable(kTable_Plugin,false);
	
	//======================== �u�F�v �^�u ========================
	
	//�F���u�W���v���[�h�Ɠ����ɂ���ݒ�
	NVM_RegisterDBData(AModePrefDB::kSameAsNormalMode_Colors,			true);
	
	//�J���[�X�L�[�� #889
	//#889 NVM_RegisterDBData(AModePrefDB::kUseColorScheme,					true);
	NVI_RegisterTextArrayMenu(AModePrefDB::kColorSchemeName,kTextArrayMenuID_ColorScheme);
	NVM_RegisterDBData(AModePrefDB::kColorSchemeName,					true);
	
	//�����F�E�w�i�F
	NVM_RegisterDBData(AModePrefDB::kLetterColor,						true);
	NVM_RegisterDBData(AModePrefDB::kBackgroundColor,					true);
	NVM_RegisterDBData(AModePrefDB::kControlCodeColor,					true);//#1142
	
	//#844
	//�e�X���b�g�F
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot0_Color,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot1_Color,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot2_Color,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot3_Color,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot4_Color,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot5_Color,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot6_Color,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot7_Color,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlotComment_Color,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlotLiteral_Color,		true);
	
	//#844
	//�e�X���b�g�{�[���h
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot0_Bold,				true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot1_Bold,				true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot2_Bold,				true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot3_Bold,				true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot4_Bold,				true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot5_Bold,				true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot6_Bold,				true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot7_Bold,				true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlotComment_Bold,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlotLiteral_Bold,		true);
	
	//#844
	//�e�X���b�g�C�^���b�N
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot0_Italic,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot1_Italic,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot2_Italic,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot3_Italic,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot4_Italic,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot5_Italic,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot6_Italic,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot7_Italic,			true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlotComment_Italic,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlotLiteral_Italic,		true);
	
	//#844
	//�e�X���b�g�A���_�[���C��
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot0_Underline,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot1_Underline,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot2_Underline,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot3_Underline,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot4_Underline,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot5_Underline,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot6_Underline,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlot7_Underline,		true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlotComment_Underline,	true);
	NVM_RegisterDBData(AModePrefDB::kSyntaxColorSlotLiteral_Underline,	true);
	
	//�C���|�[�g�E���[�J���F
	NVM_RegisterDBData(AModePrefDB::kDarkenImportLightenLocal,			true);
	
	//�e�n�C���C�g�F
	NVM_RegisterDBData(AModePrefDB::kFindHighlightColor,				true);
	NVM_RegisterDBData(AModePrefDB::kFindHighlightColorPrev,			true);
	NVM_RegisterDBData(AModePrefDB::kCurrentWordHighlightColor,			true);
	NVM_RegisterDBData(AModePrefDB::kTextMarkerHightlightColor,			true);
	NVM_RegisterDBData(AModePrefDB::kFirstSelectionColor,				true);
	
	//diff�F�s�����x
	NVM_RegisterDBData(AModePrefDB::kDiffColorOpacity,					true);
	
	//�I��F�����x
	NVM_RegisterDBData(AModePrefDB::kSelectionOpacity,					true);
	
	//======================== �u�\���v �^�u ========================
	
	//------------------�e�L�X�g�\��------------------
	
	//TextDisplay���u�W���v���[�h�Ɠ����ɂ���ݒ�
	NVM_RegisterDBData(AModePrefDB::kSameAsNormalMode_TextDisplay,		true);
	
	//�s��
	NVM_RegisterDBData(AModePrefDB::kLineMargin,						true);
	
	//���䕶���\��
	NVM_RegisterDBData(AModePrefDB::kDisplayReturnCode,					true);
	NVM_RegisterDBData(AModePrefDB::kDisplayTabCode,					true);
	NVM_RegisterDBData(AModePrefDB::kDisplaySpaceCode,					true);
	NVM_RegisterDBData(AModePrefDB::kDisplayZenkakuSpaceCode,			true);
	
	//���^�o�b�N�X���b�V�� #940
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(AModePrefDB::kConvert5CToA5WhenOpenJIS);
	NVI_AddToLastRegisteredRadioGroup(kRadio_DisplayBackslash);
	
	//JIS�n�t�@�C�����J�������o�b�N�X���b�V���𔼊p���Ƃ��ĕ\��
	NVM_RegisterDBData(AModePrefDB::kConvert5CToA5WhenOpenJIS,			true);
	
	//�c�������[�h #558
	NVM_RegisterDBData(AModePrefDB::kVerticalMode,						true);
	
	//���ʃn�C���C�g #1406
	NVM_RegisterDBData(AModePrefDB::kHighlightBrace,					false);
	
	//------------------�w�i�\��------------------
	
	//�e�L�X�g�\�����u�W���v���[�h�Ɠ����ɂ���ݒ�
	NVM_RegisterDBData(AModePrefDB::kSameAsNormalMode_BackgroundDisplay,true);
	
	//�w�i�摜�E�s��
	//�t�@�C���p�X��NVMDO_UpdateControlStatus()�ɂĐݒ�
	NVM_RegisterDBData(AModePrefDB::kDisplayBackgroundImage,			true);
	NVM_RegisterDBData(AModePrefDB::kBackgroundImageFile,				true);
	NVM_RegisterDBData(AModePrefDB::kBackgroundImageTransparency,		true,AModePrefDB::kDisplayBackgroundImage);
	NVM_RegisterDBData(AModePrefDB::kDisplayGenkoyoshi,					true);
	
	//------------------���C�A�E�g------------------
	
	//�e�L�X�g���C�A�E�g���u�W���v���[�h�Ɠ����ɂ���ݒ�
	NVM_RegisterDBData(AModePrefDB::kSameAsNormalMode_TextLayout,		true);
	
	//���[���[�E�}�N���o�[�E�s�ԍ��\��
	NVM_RegisterDBData(AModePrefDB::kDisplayRuler,						true);
	NVM_RegisterDBData(AModePrefDB::kRulerBaseLetter,					true);
	NVM_RegisterDBData(AModePrefDB::kRulerScaleWithTabLetter,			true);
	//#457 NVM_RegisterDBData(AModePrefDB::kDisplayToolbar,					true);
	NVM_RegisterDBData(AModePrefDB::kDisplayEachLineNumber,				true);
	NVM_RegisterDBData(AModePrefDB::kDisplayEachLineNumber_AsParagraph,	true);
	
	//======================== �u�ҏW�v �^�u ========================
	
	//------------------�e�L�X�g�̃v���p�e�B------------------
	
	//�f�t�H���g�ݒ���u�W���v���[�h�Ɠ����ɂ���ݒ�
	NVM_RegisterDBData(AModePrefDB::kSameAsNormalMode_TextProperty,	false);
	
	//�f�t�H���g�e�L�X�g�G���R�[�f�B���O�E���s�R�[�h�E�s�܂�Ԃ�
	NVI_RegisterTextArrayMenu(AModePrefDB::kDefaultTextEncoding,kTextArrayMenuID_TextEncoding);
	NVM_RegisterDBData(AModePrefDB::kDefaultTextEncoding,				false);
	NVM_RegisterDBData(AModePrefDB::kDefaultReturnCode,					false);
	NVM_RegisterDBData(AModePrefDB::kDefaultWrapMode,					false);
	NVM_RegisterDBData(AModePrefDB::kDefaultWrapMode_LetterCount,		false);
	NVM_RegisterDBData(AModePrefDB::kSaveUTF8BOM,						false);
	
	//------------------���͐ݒ�------------------
	
	//���͐ݒ���u�W���v���[�h�Ɠ����ɂ���ݒ�
	NVM_RegisterDBData(AModePrefDB::kSameAsNormalMode_InputtSettings,	false);
	
	//���ʑΉ��E4��N���b�N�E���L�[
	NVM_RegisterDBData(AModePrefDB::kCheckBrace,						false);
	NVM_RegisterDBData(AModePrefDB::kSelectInsideByQuadClick,			false);
	NVM_RegisterDBData(AModePrefDB::kQuadClickText,						false);
	NVM_RegisterDBData(AModePrefDB::kInputBackslashByYenKey,			false);
	NVM_RegisterDBData(AModePrefDB::kAddRemoveSpaceByDragDrop,			false);
	NVM_RegisterDBData(AModePrefDB::kApplyCharsForWordToDoubleClick,	false);
	
	//------------------�X�y���`�F�b�N------------------
	
	//�X�y���`�F�b�N�ݒ���u�W���v���[�h�Ɠ����ɂ���ݒ�
	NVM_RegisterDBData(AModePrefDB::kSameAsNormalMode_SpellCheck,		true);
	
	//�X�y���`�F�b�N
	NVM_RegisterDBData(AModePrefDB::kDisplaySpellCheck,					true);
	NVM_RegisterDBData(AModePrefDB::kSpellCheckLanguage,				true);
	/*#688
#if IMPLEMENTATION_FOR_MACOSX
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_4 ) 
	{
		NVI_SetControlEnable(AModePrefDB::kDisplaySpellCheck,false);
		NVI_SetControlEnable(AModePrefDB::kSpellCheckLanguage,false);
	}
#endif
	*/
	
	//======================== �u�L�[���[�h�v �^�u ========================
	
	//�L�[���[�h�ݒ���@���W�I�{�^���O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_Keyword_NotUseCSVFile);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Keyword_UseCSVFile);
	
	//�L�[���[�h�J�e�S���[�e�[�u��
	NVM_RegisterDBTable(kTable_KeywordCategory,AModePrefDB::kAdditionalCategory_Name,
				kButton_KeywordAddCategory,kButton_KeywordRemoveCategory,kButton_KeywordRemoveCategoryOK,
				kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
				kControlID_Invalid,true,true,true,true,false);
	NVM_RegisterDBTableColumn(	kTable_KeywordCategory,kColumn_Category,					AModePrefDB::kAdditionalCategory_Name,
								kButton_KeywordCategoryEdit,	0,"ModePref_KeywordCategory_ListViewTitle0",true);//win 080618
	//B0406 NVM_RegisterDBTableContentView(kTable_KeywordCategory,/*B0406 kText_KeywordList*/kControlID_Invalid,kDataID_Invalid);
	NVM_RegisterDBTableSpecializedContentView(kTable_KeywordCategory,kText_KeywordList);
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kCheck_KeywordIgnoreCase,		AModePrefDB::kAdditionalCategory_IgnoreCase);
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kCheck_KeywordRegExp,			AModePrefDB::kAdditionalCategory_RegExp);
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kCheck_KeywordPriorToOtherColor,AModePrefDB::kAdditionalCategory_PriorToOtherColor);//R0195
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kButton_KeywordDisable,		AModePrefDB::kAdditionalCategory_Disabled,true);//R0000 #1032
	NVM_RegisterDBTableSpecializedContentView(kTable_KeywordCategory,kRadio_Keyword_UseCSVFile);//#791
	NVM_RegisterDBTableSpecializedContentView(kTable_KeywordCategory,kRadio_Keyword_NotUseCSVFile);//#791
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kEditBox_Keyword_CSVPath,		AModePrefDB::kAdditionalCategory_CSVPath);//#791
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kEditBox_Keyword_KeywordColumn,AModePrefDB::kAdditionalCategory_KeywordColumn);//#791
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kEditBox_Keyword_ExplanationColumn,AModePrefDB::kAdditionalCategory_ExplanationColumn);//#791
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kEditBox_Keyword_TypeTextColumn,AModePrefDB::kAdditionalCategory_TypeTextColumn);//#791
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kEditBox_Keyword_ParentKeywordColumn,AModePrefDB::kAdditionalCategory_ParentKeywordColumn);//#791
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kEditBox_Keyword_ColorSlotColumn,AModePrefDB::kAdditionalCategory_ColorSlotColumn);//#791
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kEditBox_Keyword_CompletionTextColumn,AModePrefDB::kAdditionalCategory_CompletionTextColumn);//#791
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kEditBox_Keyword_URLColumn,AModePrefDB::kAdditionalCategory_URLColumn);//#791
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kPopup_Keyword_ColorSlot,		AModePrefDB::kAdditionalCategory_ColorSlot);//#844
	NVM_RegisterDBTableContentView(kTable_KeywordCategory,kPopup_Keyword_ValidSyntaxPart,AModePrefDB::kAdditionalCategory_ValidSyntaxPart);//#844
	
	//�L�[���[�h�G�f�B�b�g�{�b�N�X
	NVI_CreateEditBoxView(kText_KeywordList,true,false);//#135
	NVI_SetControlEnable(kText_KeywordList,false);//#135
	NVI_SetCatchReturn(kText_KeywordList,true);
	NVI_SetAutomaticSelectBySwitchFocus(kText_KeywordList,false);
	NVI_SetControlTextFont(kText_KeywordList,/*win AFontWrapper::GetFixWidthFont()*/appdefaultfontname,11);//R0000 #375
	
	//���@�p�[�g���j���[�X�V
	UpdateValidSyntaxPartMenu();
	
	//======================== �u���o���v �^�u ========================
	
	//���o�����@���W�I�{�^���O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_JumpSetup_RegExp);
	NVI_AddToLastRegisteredRadioGroup(kRadio_JumpSetup_SimplerSetting);
	
	//���o���e�[�u��
	NVM_RegisterDBTable(kTable_JumpSetup,AModePrefDB::kJumpSetup_Name,
				kButton_JumpSetup_Add,kButton_JumpSetup_Remove,kButton_JumpSetup_RemoveOK,
				kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
				kControlID_Invalid,true,true,false,true,false);
	NVM_RegisterDBTableColumn(	kTable_JumpSetup,kColumn_JumpSetup_Name,				AModePrefDB::kJumpSetup_Name,
								kButton_JumpSetupNameEdit,		0,"",true);//win 080618
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kCheck_JumpSetup_Enable,		AModePrefDB::kJumpSetup_Enable);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kText_JumpSetup_RegExp,			AModePrefDB::kJumpSetup_RegExp);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kCheck_JumpSetup_RegisterToMenu,AModePrefDB::kJumpSetup_RegisterToMenu);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kText_JumpSetup_MenuText,		AModePrefDB::kJumpSetup_MenuText);
	//#844 NVM_RegisterDBTableContentView(kTable_JumpSetup,kText_JumpSetup_SelectionGroup,	AModePrefDB::kJumpSetup_SelectionGroup);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kCheck_JumpSetup_ColorizeLine,	AModePrefDB::kJumpSetup_ColorizeLine);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kColor_JumpSetup_LineColor,		AModePrefDB::kJumpSetup_LineColor);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kCheck_JumpSetup_RegisterAsKeyword,AModePrefDB::kJumpSetup_RegisterAsKeyword);
	//NVM_RegisterDBTableContentView(kTable_JumpSetup,kColor_JumpSetup_KeywordColor,	AModePrefDB::kJumpSetup_KeywordColor);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kPopup_JumpSetup_KeywordColorSlot,	AModePrefDB::kJumpSetup_KeywordColorSlot);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kText_JumpSetup_KeywordGroup,	AModePrefDB::kJumpSetup_KeywordGroup);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kText_JumpSetup_OutlineLevel,	AModePrefDB::kJumpSetup_OutlineLevel);
	NVM_RegisterDBTableSpecializedContentView(kTable_JumpSetup,kRadio_JumpSetup_RegExp);
	NVM_RegisterDBTableSpecializedContentView(kTable_JumpSetup,kRadio_JumpSetup_SimplerSetting);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kText_JumpSetup_LineStartText,	AModePrefDB::kJumpSetup_LineStartText);
	NVM_RegisterDBTableContentView(kTable_JumpSetup,kCheckbox_JumpSetup_RegisterToMenu2,	AModePrefDB::kJumpSetup_RegisterToMenu2);
	//NVM_RegisterDBTableContentView(kTable_JumpSetup,kCheck_JumpSetup_ColorizeRegExpGroup,		AModePrefDB::kJumpSetup_ColorizeRegExpGroup);//#603
	//NVM_RegisterDBTableContentView(kTable_JumpSetup,kText_JumpSetup_ColorizeRegExpGroup_Number,	AModePrefDB::kJumpSetup_ColorizeRegExpGroup_Number);//#603
	//NVM_RegisterDBTableContentView(kTable_JumpSetup,kColor_JumpSetup_ColorizeRegExpGroup_Color,	AModePrefDB::kJumpSetup_ColorizeRegExpGroup_Color);//#603
	
	
	//======================== �u���@�v �^�u ========================
	
	//���@�ݒ���@���W�I�{�^���O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVI_AddToLastRegisteredRadioGroup(AModePrefDB::kUseUserDefinedSDF);
	NVI_AddToLastRegisteredRadioGroup(kRadio_Syntax_UseSimpleSDF);
	
	//���@�ݒ���@
	NVM_RegisterDBData(AModePrefDB::kUseBuiltinSyntaxDefinitionFile,	true);
	NVM_RegisterDBData(AModePrefDB::kUseUserDefinedSDF,					false);//,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	
	//--------------- �g�ݍ���SDF�g�p���ݒ� ---------------
	
	//���@��`
	NVI_SetMenuItemsFromTextArray(AModePrefDB::kBuiltinSyntaxDefinitionName,GetApp().SPI_GetBuiltinSDFNameArray());
	NVM_RegisterDBData(AModePrefDB::kBuiltinSyntaxDefinitionName,		true);//#844 ,AModePrefDB::kUseEmbeddedSDF);//#349 AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	
	//���[�U�[SDF
	NVM_RegisterDBData(AModePrefDB::kUserDefinedSDFFilePath,			false,AModePrefDB::kUseUserDefinedSDF);
	
	//�f�t�H���g���@�p�[�g
	NVM_RegisterDBData(AModePrefDB::kDefaultSyntaxPart,					false);
	
	//�C���|�[�g�t�@�C��
	NVI_CreateEditBoxView(AModePrefDB::kSystemHeaderFiles,true,true,true);//#253
    NVI_GetEditBoxView(AModePrefDB::kSystemHeaderFiles).SPI_SetTextDrawProperty(appdefaultfontname,9.0);//#1316 ,kColor_Black,kColor_Black);//#253 #375
	NVI_GetEditBoxView(AModePrefDB::kSystemHeaderFiles).NVI_SetCatchReturn(true);//#253
	NVM_RegisterDBData(AModePrefDB::kImportSystemHeaderFiles,			false);
	NVM_RegisterDBData(AModePrefDB::kSystemHeaderFiles,					false,AModePrefDB::kImportSystemHeaderFiles);//#253
	
	//�f�B���N�e�B�u
	NVM_RegisterDBData(AModePrefDB::kRecognizeConditionalCompileDirective,false);//#467
	NVM_RegisterDBData(AModePrefDB::kConditionalCompileDisabledColor,	true,AModePrefDB::kRecognizeConditionalCompileDirective);//#467
	
	//--------------- �ȈՐݒ�g�p���ݒ� ---------------
	
	//
	//NVM_RegisterDBData(AModePrefDB::kCommentColor,						false,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	//NVM_RegisterDBData(AModePrefDB::kLiteralColor,						false,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kUseRegExpCommentStart,				false);//,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kCommentStart,						false);//,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kCommentEnd,						false);//,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kCommentLineStart,					false);//,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kLiteralStart1,						false);//,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kLiteralEnd1,						false);//,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kLiteralStart2,						false);//,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kLiteralEnd2,						false);//,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kEscapeString,						false);//,kControlID_Invalid,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	
	//�L�[���[�h�\������
	NVM_RegisterDBData(AModePrefDB::kCharsForWord,						true);
	NVM_RegisterDBData(AModePrefDB::kCharsForHeadOfWord,				true);
	NVM_RegisterDBData(AModePrefDB::kCharsForTailOfWord,				true);
	
	//�u���b�N�J�n�A�I�����K�\�� #972
	NVM_RegisterDBData(AModePrefDB::kIncIndentRegExp,					false);
	NVM_RegisterDBData(AModePrefDB::kDecIndentRegExp,					false);
	
	//======================== �u�C���f���g�v �^�u ========================
	
	//�C���f���g�����ݒ�
	NVM_RegisterDBData(AModePrefDB::kIndentAutoDetect,					false);//#912
	
	//�C���f���g��
	NVM_RegisterDBData(AModePrefDB::kIndentWidth,						false);
	
	//�^�u�\����
	NVM_RegisterDBData(AModePrefDB::kDefaultTabWidth,					true);
	
	//
	NVM_RegisterDBData(AModePrefDB::kShiftleftByBackspace,				false);
	
	//�C���f���g�����^�u�^�X�y�[�X���W�I�{�^���O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_Indent_InputTabCodeForIndent);
	NVI_AddToLastRegisteredRadioGroup(AModePrefDB::kInputSpacesByTabKey);
	
	//�C���f���g�����^�u�^�X�y�[�X
	NVM_RegisterDBData(AModePrefDB::kInputSpacesByTabKey,				false);
	
	//�C���f���g�I�v�V����
	NVM_RegisterDBData(AModePrefDB::kIndent0ForEmptyLine,				false);
	NVM_RegisterDBData(AModePrefDB::kIndentWhenPaste,					false);
	NVM_RegisterDBData(AModePrefDB::kIndentExeLetters,					false);//B0392,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	
	//�C���f���g�v�Z���@���W�I�{�^���O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(AModePrefDB::kNoIndent);//#912
	NVI_AddToLastRegisteredRadioGroup(kRadio_Indent_SameAsPreviousLine);
	NVI_AddToLastRegisteredRadioGroup(AModePrefDB::kUseSyntaxDefinitionIndent);//#683
	NVI_AddToLastRegisteredRadioGroup(AModePrefDB::kUseRegExpIndent);//#683
	
	//�C���f���g�v�Z���@
	NVM_RegisterDBData(AModePrefDB::kNoIndent,							false);//#912
	NVM_RegisterDBData(AModePrefDB::kUseSyntaxDefinitionIndent,			false);//,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kUseRegExpIndent,					false);//B0392
	
	//�ʂ̃C���f���g
	NVM_RegisterDBData(AModePrefDB::kIndentOnTheWay,					false);//,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kIndentForLabel,					false);//,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kIndentOnTheWaySecondIndent,		false);//,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kIndentOnTheWay_ParenthesisStartPos,false);//,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	NVM_RegisterDBData(AModePrefDB::kIndentOnTheWay_SingleColon,		false);//,AModePrefDB::kUseBuiltinSyntaxDefinitionFile);
	
	//���K�\���C���f���g�e�[�u���̐ݒ�
	NVM_RegisterDBTable(kTable_Indent,AModePrefDB::kIndent_RegExp,
				kButton_IndentAdd,kButton_IndentRemove,kButton_IndentRemoveOK,
				kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
				kControlID_Invalid,true,true,true,true,false);//win 080618
	NVM_RegisterDBTableColumn(	kTable_Indent,kColumn_Indent_RegExp,				AModePrefDB::kIndent_RegExp,
								kButton_IndentRegExpEdit,			480,"ModePref_Indent_ListViewTitle0",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_Indent,kColumn_Indent_OffsetCurrentLine,	AModePrefDB::kIndent_OffsetCurrentLine,
								kButton_IndentCurLineEdit,			60,"ModePref_Indent_ListViewTitle1",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_Indent,kColumn_Indent_OffsetNextLine,		AModePrefDB::kIndent_OffsetNextLine,
								kButton_IndentNextLineEdit,			0,"ModePref_Indent_ListViewTitle2",true);//win 080618
	
	//======================== �u�@�\�v �^�u ========================
	
	//------------------�܂肽����------------------
	NVM_RegisterDBData(AModePrefDB::kUseLineFolding,					true);
	NVI_CreateEditBoxView(AModePrefDB::kDefaultFoldingLineText,true,true);
	NVI_SetCatchTab(AModePrefDB::kDefaultFoldingLineText,true);
	NVI_SetCatchReturn(AModePrefDB::kDefaultFoldingLineText,true);
	NVI_GetEditBoxView(AModePrefDB::kDefaultFoldingLineText).NVI_SetAutomaticSelectBySwitchFocus(false);
	NVI_SetControlTextFont(AModePrefDB::kDefaultFoldingLineText,appdefaultfontname,11);
	NVM_RegisterDBData(AModePrefDB::kUseDefaultFoldingSetting,			false);
	NVM_RegisterDBData(AModePrefDB::kDefaultFoldingLineText,			false);
	
	//------------------Drag&Drop�e�[�u���̐ݒ�------------------
	NVM_RegisterDBTable(kTable_DragDrop,AModePrefDB::kDragDrop_Suffix,
				kButton_DragDropAdd,kButton_DragDrapRemove,kButton_DragDrapRemoveOK,
				kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
				kControlID_Invalid,false,true,true,true,false);
	NVM_RegisterDBTableColumn(	kTable_DragDrop,kColumn_DragDrop_Suffix,			AModePrefDB::kDragDrop_Suffix,
								kButton_DragDropSuffixEdit,			0,"ModePref_DragDrop_ListViewTitle0",true);
	NVM_RegisterDBTableContentView(kTable_DragDrop,kText_DragDrapText,			AModePrefDB::kDragDrop_Text);
	
	NVI_CreateEditBoxView(kText_DragDrapText);
	NVI_SetCatchTab(kText_DragDrapText,true);
	NVI_SetCatchReturn(kText_DragDrapText,true);
	NVI_SetControlTextFont(kText_DragDrapText,appdefaultfontname,11);
	
	//�T�t�B�N�X�s��v������
	NVM_RegisterDBData(AModePrefDB::kDragDrop_ActionForNoSuffix,		false);
	
	//------------------�Ή�������e�[�u���̐ݒ�------------------
	NVM_RegisterDBTable(kTable_Correspond,AModePrefDB::kCorrespond_StartText,
			kButton_CorrespondAdd,kButton_CorrespondRemove,kButton_CorrespondRemoveOK,
			kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
			kControlID_Invalid,false,true,true,true,false);
	NVM_RegisterDBTableColumn(	kTable_Correspond,kColumn_Correspond_Start,		AModePrefDB::kCorrespond_StartText,
								kButton_CorrStartEdit,				240,"ModePref_Correspond_ListViewTitle0",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_Correspond,kColumn_Correspond_End,			AModePrefDB::kCorrespond_EndText,
								kButton_CorrEndEdit,				0,"ModePref_Correspond_ListViewTitle1",true);//win 080618
	NVM_SetInhibit0LengthForTable(kTable_Correspond,false);//#11
	
	//
	NVM_RegisterDBData(AModePrefDB::kEnableSDFCorrespond,					false);
	
	//======================== �u�L�[�o�C���h�v �^�u ========================
	
	//�L�[�o�C���h�x�[�X���W�I�{�^���O���[�v
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_KeyBind_DefaultIsAppKeyBind);
	NVI_AddToLastRegisteredRadioGroup(AModePrefDB::kKeyBindDefaultIsNormalMode);
	NVI_AddToLastRegisteredRadioGroup(AModePrefDB::kKeyBindDefaultIsOSKeyBind);
	//�L�[�o�C���h�x�[�X�ݒ�
	NVM_RegisterDBData(AModePrefDB::kKeyBindDefaultIsNormalMode,				true);
	NVM_RegisterDBData(AModePrefDB::kKeyBindDefaultIsOSKeyBind,					true);
	
	//�L�[�o�C���h�e�[�u���̐ݒ�
	NVM_RegisterDBTable(kTable_Keybind,kDataID_Invalid,
			kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
			kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
			kControlID_Invalid,false,true,true,true,false);
	NVM_RegisterDBTableColumn(	kTable_Keybind,kColumn_KeyBind_Key,					kDataType_TextArray,
								kControlID_Invalid,					80,"ModePref_KeyBind_ListViewTitle0",false);//win 080618
	NVM_RegisterDBTableColumn(	kTable_Keybind,kColumn_KeyBind_Action,				kDataType_TextArray,
								kControlID_Invalid,					0,"ModePref_KeyBind_ListViewTitle1",false);//win 080618
	NVM_RegisterDBTableSpecializedContentView(kTable_Keybind,kPopup_KeybindAction);
	NVM_RegisterDBTableSpecializedContentView(kTable_Keybind,kText_KeybindActionText);
	
	//�L�[�o�C���h���상�j���[
	NVI_SetMenuItemsFromTextArray(kPopup_KeybindAction,mKeyBindActionMenuText);
	
	//�L�[�o�C���h����}��������
	NVI_CreateEditBoxView(kText_KeybindActionText);
	NVI_SetCatchTab(kText_KeybindActionText,true);
	NVI_SetCatchReturn(kText_KeybindActionText,true);
	NVI_SetControlTextFont(kText_KeybindActionText,appdefaultfontname,11);
	
	//======================== �u�c�[���v �^�u ========================
	
	//�c�[�����X�g�r���[
	NVI_CreateListViewFromResource(kListView_Tool);//#205
	NVI_GetListView(kListView_Tool).SPI_CreateFrame(false,true,false);//#205 #688
	//#688 NVI_GetListView(kListView_Tool).NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_Invalid);//#688 ,kVScrollBar_Tool);//#205
	NVI_GetListView(kListView_Tool).SPI_RegisterColumn_Text(kColumn_ToolName,0,"",false);
	NVI_GetListView(kListView_Tool).SPI_SetEnableDragDrop(true);
	NVI_GetListView(kListView_Tool).SPI_SetRowMargin(5);
	NVI_GetListView(kListView_Tool).SPI_SetMultiSelectEnabled(true);//#237
	NVI_EnableHelpTagCallback(kListView_Tool);//#427
	
	//�c�[���}��������
	NVI_CreateEditBoxView(kText_Tool,true,true,true,kEditBoxType_Normal);
	NVI_SetCatchTab(kText_Tool,true);
	NVI_SetCatchReturn(kText_Tool,true);
	NVI_SetControlTextFont(kText_Tool,appdefaultfontname,11);
	NVI_GetEditBoxView(kText_Tool).NVI_SetAutomaticSelectBySwitchFocus(false);
	
	//�c�[�����j���[�̃��[�g�̃I�u�W�F�N�gID�����݂̃c�[�����j���[�I�u�W�F�N�g�Ƃ��Đݒ�
	mCurrentToolMenuObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuRootObjectID();//B0303
	
	//#305
	//�c�[���������ꃁ�j���[�̃f�t�H���g��ݒ�
	mCurrentToolNameLanguage = GetApp().NVI_GetLanguage();
	NVI_SetControlNumber(kPopup_Tool_NameLanguage,GetMenuItemIndexFromLanguage(mCurrentToolNameLanguage));
	
	//#427 ToolContentView�̃R���g���[�����O���C�A�E�g�ɂ��A��ɐݒ�
	SPI_UpdateToolContentViewControlGrayout(false,false,false,false,false,false);
	SPI_ClearToolContentViewControl();
	
	//======================== �u�ڍׁv �^�u ========================
	
	//------------------�ڍ�------------------
	
	//charset�Ƃ̈�v���`�F�b�N
	NVM_RegisterDBData(AModePrefDB::kCheckCharsetWhenSave,				false);
	
#if IMPLEMENTATION_FOR_MACOSX
	//�N���G�[�^�R�[�h�֘A
	NVM_RegisterDBData(AModePrefDB::kSetDefaultCreator,					false);
	NVM_RegisterDBData(AModePrefDB::kDefaultCreator,					false,AModePrefDB::kSetDefaultCreator);
	NVM_RegisterDBData(AModePrefDB::kForceMiTextFile,					false);
#endif
	
	//�S�p�X�y�[�X�C���f���g
	NVM_RegisterDBData(AModePrefDB::kZenkakuSpaceIndent,				false);//#318
	
	//�ŏ��̃X�y�[�X�������^�u�\�����ŕ\��
	NVM_RegisterDBData(AModePrefDB::kStartSpaceToIndent,                false);
	
	//���[�h�I���t�@�C���p�X�e�[�u���̐ݒ� R0210
	NVM_RegisterDBTable(kTable_ModeSelectFilePath,AModePrefDB::kModeSelectFilePath,
						kControlID_Invalid,kButton_ModeSelectRemove,kButton_ModeSelectRemoveOK,
						kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
						kControlID_Invalid,false,true,true,true,false);
	NVM_RegisterDBTableColumn(	kTable_ModeSelectFilePath,kColumn_First,				AModePrefDB::kModeSelectFilePath,
							  kButton_EditModeSelectFilePath,		0,"ModePref_ModeSelectFilePath_ListViewTitle0",true);//win 080618
	
	//------------------���[�h�Ǘ�------------------
	
	//���[�h�������ꃁ�j���[�̃f�t�H���g��ݒ�
	mCurrentModeNameLanguage = GetApp().NVI_GetLanguage();
	NVI_SetControlNumber(kPopup_ModeNameLanguage,GetMenuItemIndexFromLanguage(mCurrentModeNameLanguage));
	DisplayMultiLanguageModeName();//#305
	
	//------------------���[�h�X�V------------------
	//�X�V���@���W�I�{�^��
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kButton_ModeUpdate_Application);
	NVI_AddToLastRegisteredRadioGroup(AModePrefDB::kEnableModeUpdateFromWeb);
	
	//
	NVI_RegisterTextArrayMenu(AModePrefDB::kModeUpdateSourceModeName,kTextArrayMenuID_DefaultMode);
	NVM_RegisterDBData(AModePrefDB::kModeUpdateSourceModeName,			true);
	
	//
	NVM_RegisterDBData(AModePrefDB::kEnableModeUpdateFromWeb,			false);
	
	//���[�h�X�VURL
	NVM_RegisterDBData(AModePrefDB::kModeUpdateURL,						false);
    
	//���[�h�X�VURL���j���[
	//#844 NVI_RegisterTextArrayMenu(kModeUpdateURLMenu,kTextArrayMenuID_ModeUpdateURL);
	
#if IMPLEMENTATION_FOR_MACOSX
	//#427
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_4 )
	{
		NVI_SetControlEnable(kModeUpateButton,false);
	}
#endif
	
	//======================== ���̑����� ========================
	//�ŏ��̃^�u�R���g���[����I��
	NVI_SelectTabControl(0);
	
	//�w���v�{�^���o�^
	/*
	NVI_RegisterHelpAnchor(90001,"pref.htm#modepref_default");
	NVI_RegisterHelpAnchor(90002,"pref.htm#modepref_hyouji");
	NVI_RegisterHelpAnchor(90003,"pref.htm#modepref_henshu");
	NVI_RegisterHelpAnchor(90004,"pref.htm#modepref_hiraku");
	NVI_RegisterHelpAnchor(90005,"pref.htm#modepref_hozon");
	NVI_RegisterHelpAnchor(90006,"pref.htm#modepref_bunpou");
	NVI_RegisterHelpAnchor(90007,"pref.htm#modepref_keyword");
	NVI_RegisterHelpAnchor(90008,"pref.htm#modepref_midashi");
	NVI_RegisterHelpAnchor(90009,"pref.htm#modepref_nyuryokushien");
	NVI_RegisterHelpAnchor(90010,"pref.htm#modepref_keybind");
	NVI_RegisterHelpAnchor(90011,"pref.htm#modepref_tool");
	NVI_RegisterHelpAnchor(90012,"pref.htm#modepref_indent");
	NVI_RegisterHelpAnchor(90013,"pref.htm#modepref_kanri");
	*/
	NVI_RegisterHelpAnchor(90001,"topic.htm#yokutsukau");
	NVI_RegisterHelpAnchor(90002,"kihon.htm#mode");
	NVI_RegisterHelpAnchor(90003,"topic.htm#register_keyword");
	NVI_RegisterHelpAnchor(90004,"topic.htm#filedragdrop");
	
	//�^�C�g���o�[�Ƀ��[�h�t�H���_�ݒ�
	AFileAcc	modeFolder;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeFolder(modeFolder);
	GetApp().SPI_GetModePrefWindow(mModeIndex).NVI_SetWindowFile(modeFolder);
	
	//
	NVI_UpdateProperty();//B0303
}

/*B0303 void	AWindow_ModePref::SPNVI_Show( const AModeIndex inModeIndex )
{
	SPI_SelectMode(inModeIndex);
	mCurrentToolMenuObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuRootObjectID();
	NVI_Show();
	NVI_UpdateProperty();
}*/

void	AWindow_ModePref::NVMDO_UpdateControlStatus()
{
	//AWindow�W�������itable���ڂ̍폜�{�^��enable/disable���j���I����ɂ����ɗ���
	
	//�W�����[�h�̏ꍇ�A�u�W�����[�h���J���v�{�^����\�����Ȃ�
	{
		NVI_SetControlEnable(kButton_OpenNormalMode_ModeTab,(mModeIndex!=kStandardModeIndex));
		NVI_SetControlEnable(kButton_OpenNormalMode_DisplayTab,(mModeIndex!=kStandardModeIndex));
		NVI_SetControlEnable(kButton_OpenNormalMode_EditTab,(mModeIndex!=kStandardModeIndex));
		NVI_SetControlEnable(kButton_OpenNormalMode_ColorsTab,(mModeIndex!=kStandardModeIndex));
	}
	//�W�����[�h�̏ꍇ�A�u�W�����[�h�Ɠ����v�`�F�b�N�{�b�N�X��disable�ɂ���
	{
		NVI_SetControlEnable(AModePrefDB::kSameAsNormalMode_Font,(mModeIndex!=kStandardModeIndex));
		NVI_SetControlEnable(AModePrefDB::kSameAsNormalMode_Colors,(mModeIndex!=kStandardModeIndex));
		NVI_SetControlEnable(AModePrefDB::kSameAsNormalMode_TextDisplay,(mModeIndex!=kStandardModeIndex));
		NVI_SetControlEnable(AModePrefDB::kSameAsNormalMode_BackgroundDisplay,(mModeIndex!=kStandardModeIndex));
		NVI_SetControlEnable(AModePrefDB::kSameAsNormalMode_TextLayout,(mModeIndex!=kStandardModeIndex));
		NVI_SetControlEnable(AModePrefDB::kSameAsNormalMode_TextProperty,(mModeIndex!=kStandardModeIndex));
		NVI_SetControlEnable(AModePrefDB::kSameAsNormalMode_InputtSettings,(mModeIndex!=kStandardModeIndex));
		NVI_SetControlEnable(AModePrefDB::kSameAsNormalMode_SpellCheck,(mModeIndex!=kStandardModeIndex));
	}
	
	//======================== �u���[�h�v �^�u ========================
	
	/*
	{
		ABool	enableSuffix = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kEnableMode);
		NVI_SetControlEnable(kTable_Suffix,enableSuffix);
		if( enableSuffix == false )
		{
			NVI_SetControlEnable(kButton_SuffixAdd,false);
			NVI_SetControlEnable(kButton_SuffixRemove,false);
			NVI_SetControlEnable(kButton_EditSuffixTable,false);
		}
		else
		{
			NVI_SetControlEnable(kButton_SuffixAdd,true);
		}
	}
	*/
	
	//�u�W�����[�h�Ɠ����ݒ���g�p����v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	sameAsNormal = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSameAsNormalMode_Font);
		NVI_SetControlEnable(kFontControl_DefaultFont,!sameAsNormal);
		NVI_SetControlEnable(kButton_DefaultFontPanel,!sameAsNormal);
	}
	
	//
	{
		NVI_SetControlEnable(AModePrefDB::kEnableMode,(mModeIndex!=kStandardModeIndex));
	}
	
	//==================�u�F�v�^�u==================
	
	//�u�W�����[�h�Ɠ����ݒ���g�p����v�`�F�b�N�{�b�N�X�A����сA�u�J���[�X�L�[���v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	useColorScheme = false;//#889 GetApp().SPI_GetModePrefDB(mModeIndex).GetModeData_Bool(AModePrefDB::kUseColorScheme);
		ABool	sameAsNormal = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSameAsNormalMode_Colors);
		//�J���[�X�L�[��
		//#889 NVI_SetControlEnable(AModePrefDB::kUseColorScheme,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kColorSchemeName,(!sameAsNormal));//#889 (!sameAsNormal)&&useColorScheme);
		NVI_SetControlEnable(kButton_ApplyColorScheme,(!sameAsNormal));
		NVI_SetControlEnable(kButton_ExportColorScheme,(!sameAsNormal));
		NVI_SetControlEnable(kButton_RevealColorSchemeFolder,(!sameAsNormal));
		NVI_SetControlEnable(kButton_UpdateColorSchemeMenu,(!sameAsNormal));
		//�����F�A�w�i�F
		NVI_SetControlEnable(AModePrefDB::kLetterColor,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kBackgroundColor,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kControlCodeColor,(!sameAsNormal)&&(!useColorScheme));//#1142
		//�J���[�X���b�g�@�F
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot0_Color,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot1_Color,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot2_Color,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot3_Color,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot4_Color,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot5_Color,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot6_Color,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot7_Color,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlotComment_Color,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlotLiteral_Color,(!sameAsNormal)&&(!useColorScheme));
		//�J���[�X���b�g�@�{�[���h
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot0_Bold,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot1_Bold,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot2_Bold,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot3_Bold,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot4_Bold,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot5_Bold,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot6_Bold,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot7_Bold,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlotComment_Bold,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlotLiteral_Bold,!sameAsNormal);
		//�J���[�X���b�g�@�C�^���b�N
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot0_Italic,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot1_Italic,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot2_Italic,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot3_Italic,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot4_Italic,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot5_Italic,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot6_Italic,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot7_Italic,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlotComment_Italic,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlotLiteral_Italic,!sameAsNormal);
		//�J���[�X���b�g�@�A���_�[���C��
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot0_Underline,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot1_Underline,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot2_Underline,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot3_Underline,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot4_Underline,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot5_Underline,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot6_Underline,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlot7_Underline,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlotComment_Underline,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSyntaxColorSlotLiteral_Underline,!sameAsNormal);
		//�C���|�[�g�E���[�J���F
		NVI_SetControlEnable(AModePrefDB::kDarkenImportLightenLocal,!sameAsNormal);
		//�n�C���C�g�F
		NVI_SetControlEnable(AModePrefDB::kFindHighlightColor,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kFindHighlightColorPrev,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kCurrentWordHighlightColor,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kTextMarkerHightlightColor,(!sameAsNormal)&&(!useColorScheme));
		NVI_SetControlEnable(AModePrefDB::kFirstSelectionColor,(!sameAsNormal)&&(!useColorScheme));
		//diff�F�s�����x
		NVI_SetControlEnable(AModePrefDB::kDiffColorOpacity,(!sameAsNormal)&&(!useColorScheme));
		//�I��F�s�����x
		NVI_SetControlEnable(AModePrefDB::kSelectionOpacity,(!sameAsNormal)&&(!useColorScheme));
	}
	
	//======================== �u�\���v �^�u ========================
	
	//�u�W�����[�h�Ɠ����ݒ���g�p����v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	sameAsNormal = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSameAsNormalMode_TextDisplay);
		//#912 NVI_SetControlEnable(AModePrefDB::kDefaultTabWidth,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kLineMargin,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kDisplayReturnCode,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kDisplayTabCode,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kDisplaySpaceCode,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kDisplayZenkakuSpaceCode,!sameAsNormal);
	}
	
	//�u�W�����[�h�Ɠ����ݒ���g�p����v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	sameAsNormal = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSameAsNormalMode_BackgroundDisplay);
		ABool	displayBackground = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kDisplayBackgroundImage);
		NVI_SetControlEnable(AModePrefDB::kDisplayBackgroundImage,!sameAsNormal);
		NVI_SetControlEnable(kButton_BackgroundImageFile,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kBackgroundImageFile,(!sameAsNormal)&&displayBackground);
		NVI_SetControlEnable(AModePrefDB::kBackgroundImageTransparency,(!sameAsNormal)&&displayBackground);
		NVI_SetControlEnable(AModePrefDB::kDisplayGenkoyoshi,!sameAsNormal);
	}
	
	//�u�W�����[�h�Ɠ����ݒ���g�p����v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	sameAsNormal = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSameAsNormalMode_TextLayout);
		ABool	displayRuler = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kDisplayRuler);
		NVI_SetControlEnable(AModePrefDB::kDisplayRuler,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kRulerBaseLetter,(!sameAsNormal)&&displayRuler);
		NVI_SetControlEnable(AModePrefDB::kRulerScaleWithTabLetter,(!sameAsNormal)&&displayRuler);
		//#457 NVI_SetControlEnable(AModePrefDB::kDisplayToolbar,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kDisplayEachLineNumber,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kDisplayEachLineNumber_AsParagraph,!sameAsNormal);
	}
	
	//
	{
		NVI_SetControlEnable(kButton_BackgroundImageFile,
							 GetApp().SPI_GetModePrefDB(mModeIndex).GetModeData_Bool(AModePrefDB::kDisplayBackgroundImage));
	}
	
	//======================== �u�ҏW�v �^�u ========================
	
	//�u�W�����[�h�Ɠ����ݒ���g�p����v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	sameAsNormal = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSameAsNormalMode_TextProperty);
		NVI_SetControlEnable(AModePrefDB::kDefaultTextEncoding,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kDefaultReturnCode,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kDefaultWrapMode,!sameAsNormal);
		ABool	wrapMode = NVI_GetControlNumber(AModePrefDB::kDefaultWrapMode);//#1113
		NVI_SetControlEnable(AModePrefDB::kDefaultWrapMode_LetterCount,(!sameAsNormal)&&(wrapMode == 2 || wrapMode == 4));//#1113
		NVI_SetControlEnable(AModePrefDB::kSaveUTF8BOM,!sameAsNormal);
	}
	
	//�u�W�����[�h�Ɠ����ݒ���g�p����v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	sameAsNormal = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSameAsNormalMode_InputtSettings);
		ABool	useQuadClick = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSelectInsideByQuadClick);
		NVI_SetControlEnable(AModePrefDB::kCheckBrace,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSelectInsideByQuadClick,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kQuadClickText,(!sameAsNormal)&&useQuadClick);
		NVI_SetControlEnable(AModePrefDB::kInputBackslashByYenKey,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kAddRemoveSpaceByDragDrop,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kApplyCharsForWordToDoubleClick,!sameAsNormal);
	}
	
	//�u�W�����[�h�Ɠ����ݒ���g�p����v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	sameAsNormal = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kSameAsNormalMode_SpellCheck);
		NVI_SetControlEnable(AModePrefDB::kDisplaySpellCheck,!sameAsNormal);
		NVI_SetControlEnable(AModePrefDB::kSpellCheckLanguage,!sameAsNormal);
	}
	
	//======================== �u���o���v �^�u ========================
	
	//���o��ContentView���̃R���g���[��Enable/Disable
	SPI_UpdateJumpSetupContentViewControlGrayout();
	
	//======================== �u�L�[���[�h�v �^�u ========================
	
	//�L�[���[�hContentView���̃R���g���[��Enable/Disable
	SPI_UpdateKeywordContentViewControlGrayout();
	
	//======================== �u���@�v �^�u ========================
	
	//�u��ɃC���|�[�g�t�@�C�������̃t�H���_����ǂݍ��ށv�`�F�b�N�{�b�N�X�ɂ��Enable/Disable
	{
		ABool	shouldEnabled = NVI_GetControlBool(AModePrefDB::kImportSystemHeaderFiles);
		NVI_SetControlEnable(AModePrefDB::kSystemHeaderFiles,shouldEnabled);
	}
	
	//
	if( NVI_GetControlBool(AModePrefDB::kUseBuiltinSyntaxDefinitionFile) == false &&
		NVI_GetControlBool(AModePrefDB::kUseUserDefinedSDF) == false )
	{
		NVI_SetControlBool(kRadio_Syntax_UseSimpleSDF,true);
	}
	
	//==================�u�C���f���g�v�^�u==================
	
	//�u�C���f���g�������h�L�������g�ɍ��킹�Ď����I�ɐݒ肷��v�`�F�b�N�{�b�N�X�ɂ��Enable/Disable
	{
		ABool	autodetectIndentType = NVI_GetControlBool(AModePrefDB::kIndentAutoDetect);
		NVI_SetControlEnable(kRadio_Indent_InputTabCodeForIndent,!autodetectIndentType);
		NVI_SetControlEnable(AModePrefDB::kInputSpacesByTabKey,!autodetectIndentType);
		NVI_SetControlEnable(AModePrefDB::kIndentWidth,!autodetectIndentType);
		
	}
	
	//
	if( NVI_GetControlBool(AModePrefDB::kNoIndent) == false &&
		NVI_GetControlBool(AModePrefDB::kUseSyntaxDefinitionIndent) == false &&
		NVI_GetControlBool(AModePrefDB::kUseRegExpIndent) == false )
	{
		NVI_SetControlBool(kRadio_Indent_SameAsPreviousLine,true);
	}
	
	//#997
	//SDF�ɂĎ����C���f���g�����ɐݒ肳��Ă���ꍇ�͊֘A�ݒ��disable�ɂ���
	{
		ABool	b = (GetApp().SPI_GetModePrefDB(mModeIndex).GetDisableAutoIndent()==false);
		NVI_SetControlEnable(AModePrefDB::kUseSyntaxDefinitionIndent,b);
		NVI_SetControlEnable(AModePrefDB::kIndentWhenPaste,b);
	}
	
	//==================�u�L�[�o�C���h�v�^�u==================
	
	{
        //�W�����[�h�̏ꍇ�́u�W�����[�h�̃L�[�o�C���h����̍�����ݒ肷��v�`�F�b�N�{�b�N�X��disable
		NVI_SetControlEnable(AModePrefDB::kKeyBindDefaultIsNormalMode,(mModeIndex!=kStandardModeIndex));
	}
    
	//======================== �u�c�[���v �^�u ========================
	
	{
		//�c�[���̌��݃p�X�\��
		AText	text;
		mToolMenuDisplayPathArray.Implode('/',text);
		text.Insert1(0,'/');
		NVI_SetControlText(kButton_ToolPath,text);
		
		//�e�t�H���_�ړ��{�^��Enable/Disable
		NVI_SetControlEnable(kButton_ToolParentFolder,(mToolMenuObjectIDArray.GetItemCount()>0));
		
		//�u�ۑ��v�{�^��Enable/Disable
		NVI_SetControlEnable(kButton_ToolSave,mToolEditManager.IsDirty());
		
		//------------------�����X�V�c�[���̏ꍇ�̈ꕔ�R���g���[��disable------------------
		//------------------�V�K���ځE�V�K�t�H���_�{�^��------------------
		ABool	enableNewToolButton = true;
		ABool	enableNewToolFolderButton = true;
		//AutoUpdate�̃t�H���_�̏ꍇ�A�V�K���ځE�V�K�t�H���_�͎g�p�s��
		//����ȊO�́A�V�K���ځE�V�K�t�H���_�{�^���g�p��
		if( GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFolderIsAutoUpdate(mCurrentToolMenuObjectID) == true )
		{
			enableNewToolButton = false;
			enableNewToolFolderButton = false;
		}
		NVI_SetControlEnable(kButton_ToolAdd,enableNewToolButton);
		NVI_SetControlEnable(kButton_AddToolFolder,enableNewToolFolderButton);
		//------------------�폜�{�^��------------------
		ABool	enableRemoveToolButton = true;
		//�c�[�����ځi�����X�V�c�[���ȊO�j��I�𒆂͍폜�{�^����enable
		AArray<AIndex>	selectedDBIndexArray;
		NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndexArray(selectedDBIndexArray);
		for( AIndex i = 0; i < selectedDBIndexArray.GetItemCount(); i++ )
		{
			AIndex	selectedDBIndex = selectedDBIndexArray.Get(i);
			//�����X�V�{�^�����I����Ɋ܂܂�Ă�����폜�s��
			if( GetApp().SPI_GetModePrefDB(mModeIndex).GetToolAutoUpdateFlag(mCurrentToolMenuObjectID,selectedDBIndex) == true )
			{
				enableRemoveToolButton = false;
				break;
			}
			//StaticText���I����Ɋ܂܂�Ă�����폜�s��
			if( GetApp().SPI_GetModePrefDB(mModeIndex).GetToolIsStaticText(mCurrentToolMenuObjectID,selectedDBIndex) == true )
			{
				enableRemoveToolButton = false;
				break;
			}
		}
		NVI_SetControlEnable(kButton_ToolRemove,enableRemoveToolButton);
	}
	
	//======================== �u�ڍׁv �^�u ========================
	
	//���[�h�X�V�{�^��#0
#if IMPLEMENTATION_FOR_MACOSX
  if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
#endif
	{
		AText	url;
		NVI_GetControlText(AModePrefDB::kModeUpdateURL,url);
		NVI_SetControlEnable(kModeUpateButton,(url.GetItemCount()>0));
	}
	
	//���[�h��
	//�W�����[�h�͕ύX�s��
	{
		NVI_SetControlEnable(kPopup_ModeNameLanguage,!(mModeIndex == kStandardModeIndex));
		NVI_SetControlEnable(kText_ModeMultiLanguageName,!(mModeIndex == kStandardModeIndex));
		NVI_SetControlEnable(kModeFileName,!(mModeIndex == kStandardModeIndex));
	}
	
	//#427 �����X�V���X�V
	{
		//���݂̃��r�W�����\��
		AText	text;
		text.SetLocalizedText("ModePref_AutoUpdateRevision");
		//���r�W�����ԍ��擾�A�\���e�L�X�g�ɐݒ�
		AText	revisionText;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kAutoUpdateRevisionText,revisionText);
		ANumber64bit	revisionNumber = revisionText.GetNumber64bit();
		AText	revision;
		if( revisionNumber == 0 )
		{
			revision.SetLocalizedText("ModePref_AutoUpdate_NotYetUpdated");
		}
		else
		{
			revision.SetNumber64bit(revisionNumber);
		}
		text.ReplaceParamText('1',revision);
		//�ŏI�X�V�����擾�A�\���e�L�X�g�ɐݒ�
		AText	datetime;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Text(AModePrefDB::kLastAutoUpdateDateTime,datetime);
		if( datetime.GetItemCount() == 0 )
		{
			datetime.SetLocalizedText("ModePref_AutoUpdate_NotYetUpdated");
		}
		text.ReplaceParamText('0',datetime);
		//�e�L�X�g�ݒ�
		NVI_SetControlText(kAutoUpdateInfoText,text);
		NVI_SetControlEnable(kAutoUpdateInfoText,true);
	}
	
	//Web���玩���X�V
	{
		ABool	enableWebUpdate = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kEnableModeUpdateFromWeb);
		NVI_SetControlEnable(kAutoUpdateInfoText,enableWebUpdate);
		NVI_SetControlEnable(kModeUpateButton,enableWebUpdate);
		NVI_SetControlEnable(AModePrefDB::kModeUpdateURL,enableWebUpdate);
		NVI_SetControlEnable(kButton_OpenModeUpdatePage,enableWebUpdate);
	}
}

/**
�e�[�u���̊e��X�V�iUI���X�V�j
*/
void	AWindow_ModePref::NVMDO_UpdateTableView( const AControlID inTableControlID, const ADataID inColumnID )
{
	switch(inTableControlID)
	{
	  case kTable_Keybind:
		{
			switch(inColumnID)
			{
                //------------------�L�[�o�C���h�̃L�[��------------------
			  case kColumn_KeyBind_Key:
				{
					//�e�[�u���ɐݒ�(BeginSetTable/EndSetTable��AWindow���Ŏ��s�����)
					NVI_SetTableColumn_Text(kTable_Keybind,kColumn_KeyBind_Key,mKeyBindKeyText);//#205
					break;
				}
                //------------------�L�[�o�C���h�̃A�N�V������------------------
			  case kColumn_KeyBind_Action:
				{
                    //�e�s�̃e�L�X�g�A�F���i�[����ϐ�
					ATextArray	textArray;
					AArray<AColor>	colorArray;//#382
                    //modifier key�̃`�F�b�N�{�b�N�X��Ԏ擾
					ABool	controlKey = NVI_GetControlBool(kCheck_Keybind_ControlKey);
					ABool	optionKey = NVI_GetControlBool(kCheck_Keybind_OptionKey);
					ABool	commandKey = NVI_GetControlBool(kCheck_Keybind_CommandKey);
					ABool	shiftKey = NVI_GetControlBool(kCheck_Keybind_ShiftKey);
					AModifierKeys	modifiers = AKeyWrapper::MakeModifierKeys(controlKey,optionKey,commandKey,shiftKey);
                    //�s���Ƃ̃��[�v
					for( AIndex i = 0; i < kKeyBindKeyItemCount; i++  )
					{
                        //�L�[�ɑΉ�����A�N�V�����擾
						AKeyBindAction	action = keyAction_NOP;
						AText	insertText;
						ABool	keybindDefault = false;//#382
						ABool	keybindBindAvailable = false;//#382
						GetApp().SPI_GetModePrefDB(mModeIndex).
                                GetData_KeyBindAction(static_cast<AKeyBindKey>(i),modifiers,
                                                      action,insertText,keybindDefault,keybindBindAvailable);
                        //OS�L�[�o�C���h�x�[�X���ǂ������擾
						ABool   defaultIsOSKeyBind = GetApp().SPI_GetModePrefDB(mModeIndex).
                                GetData_Bool(AModePrefDB::kKeyBindDefaultIsOSKeyBind);
                        //�L�[�o�C���h�۔���
						if( keybindBindAvailable == true )//#382
						{
                            //==================�L�[�o�C���h�̃L�[�̏ꍇ==================
							if( keybindDefault == true && defaultIsOSKeyBind == true )
							{
                                //------------------�L�[�o�C���h���얢�ݒ�i�f�t�H���g�j����OS�L�[�o�C���h�x�[�X�̏ꍇ------------------
                                //�uOS�̃f�t�H���g�̓���v��\���iOS�̃L�[�o�C���h�ݒ���擾������@��������Ȃ����߁j
								AText   t;
								t.SetLocalizedText("KeyBindAction_OSDefault");
								textArray.Add(t);
								colorArray.Add(kColor_List_Normal);//#1316 kColor_Black);
							}
                            else
							{
                                //------------------�L�[�o�C���h����ݒ�L��A�܂��́AOS�L�[�o�C���h�x�[�X�ł͂Ȃ��ꍇ------------------
                                //�L�[�o�C���h�A�N�V�������擾
								AText	actionText;
								actionText.SetLocalizedText("Keybind_action",action);
								//������}���̏ꍇ�́u������}�� : �`�v��\��
								if( action == keyAction_string )
								{
									actionText.AddCstring(" : ");
									actionText.AddText(insertText);
								}
								textArray.Add(actionText);
								//�f�t�H���g�ȊO�̃L�[�o�C���h��Ԏ��ɂ��� #382
								if( keybindDefault == true )
								{
									//�L�[�o�C���h����ݒ薳���̏ꍇ�͍��F�\��
									colorArray.Add(kColor_List_Normal);//#1316 kColor_Black);
								}
								else
								{
                                    //�L�[�o�C���h����ݒ�L��̏ꍇ�͐ԐF�\��
									colorArray.Add(kColor_Red);
								}
							}
						}
						else
						{
                            //==================�L�[�o�C���h�s�̃L�[�̏ꍇ==================
							AText	text;
							text.SetLocalizedText("Keybind_NoChangable");
							textArray.Add(text);
							//#382 �ύX�s�Ȃ�D�F
							colorArray.Add(kColor_Gray);
						}
					}
					//�e�[�u���ɐݒ�(BeginSetTable/EndSetTable��AWindow���Ŏ��s�����)
					NVI_SetTableColumn_Text(kTable_Keybind,kColumn_KeyBind_Action,textArray);//#205
					NVI_GetListView(kTable_Keybind).SPI_SetTable_Color(colorArray);//#382
					break;
				}
			}
		}
		//#427 �����X�V���ڂ���ɂ���
	  case kTable_KeywordCategory:
		{
			switch(inColumnID)
			{
			  case kColumn_Category:
				{
					AColorArray	colorArray, backgroundColorArray;
					ABoolArray	rowEditableArray;
					AArray<ATextStyle>	styleArray;//#1316
					for( AIndex i = 0; 
								i <  GetApp().SPI_GetModePrefDB(mModeIndex).GetItemCount_Array(AModePrefDB::kAdditionalCategory_Name);
								i++ )
					{
						AColor	color = kColor_List_Normal;//#1316 kColor_Black;
						AColor	backgroundColor = kColor_White;
						ATextStyle	style = kTextStyle_Normal;//#1316
						ABool	rowEditable = true;
						if( GetApp().SPI_GetModePrefDB(mModeIndex).
									GetData_BoolArray(AModePrefDB::kAdditionalCategory_AutoUpdate,i) == true )
						{
							//#1316 color = kColor_Blue;
							//#1316 backgroundColor = kColor_Gray92Percent;
							style = kTextStyle_Italic;//#1316
							rowEditable = false;
						}
						colorArray.Add(color);
						backgroundColorArray.Add(backgroundColor);
						styleArray.Add(style);//#1316
						rowEditableArray.Add(rowEditable);
					}
					NVI_GetListView(kTable_KeywordCategory).SPI_SetTable_Color(colorArray);
					NVI_GetListView(kTable_KeywordCategory).SPI_SetTable_BackgroundColor(backgroundColorArray);
					NVI_GetListView(kTable_KeywordCategory).SPI_SetTable_TextStyle(styleArray);//#1316
					NVI_GetListView(kTable_KeywordCategory).SPI_SetTable_RowEditable(rowEditableArray);
					break;
				}
			}
			break;
		}
		//#994
		//�v���O�C���e�[�u��
	  case kTable_Plugin:
		{
			switch(inColumnID)
			{
			  case kColumnID_Plugin_Name:
				{
					ATextArray	nameArray;
					for( AIndex i = 0;
						 i < GetApp().SPI_GetModePrefDB(mModeIndex).GetItemCount_Array(AModePrefDB::kPluginData_Id);
						 i++ )
					{
						//�v���O�C��ID�擾
						AText	pluginID;
						GetApp().SPI_GetModePrefDB(mModeIndex).GetData_TextArray(AModePrefDB::kPluginData_Id,i,pluginID);
						//�v���O�C���\�����擾
						AText	displayName;
						if( GetApp().SPI_GetModePrefDB(mModeIndex).GetPluginDisplayName(pluginID,displayName) == false )
						{
							//�v���O�C���\�����������ꍇ�̓v���O�C�������擾
							GetApp().SPI_GetModePrefDB(mModeIndex).GetData_TextArray(AModePrefDB::kPluginData_Name,i,displayName);
						}
						nameArray.Add(displayName);
					}
					//�v���O�C������Ƀf�[�^�ݒ�
					NVI_GetListView(kTable_Plugin).SPI_SetColumn_Text(inColumnID,nameArray);
					break;
				}
			}
			break;
		}
		/*#844
		//#427 �����X�V���ڂ���ɂ���
	  case kTable_JumpSetup:
		{
			switch(inColumnID)
			{
			  case kColumn_JumpSetup_Name:
				{
					AColorArray	colorArray, backgroundColorArray;
					ABoolArray	rowEditableArray;
					for( AIndex i = 0; 
								i <  GetApp().SPI_GetModePrefDB(mModeIndex).GetItemCount_Array(AModePrefDB::kJumpSetup_Name);
								i++ )
					{
						AColor	color = kColor_Black;
						AColor	backgroundColor = kColor_White;
						ABool	rowEditable = true;
						if( GetApp().SPI_GetModePrefDB(mModeIndex).
									GetData_BoolArray(AModePrefDB::kJumpSetup_AutoUpdate,i) == true )
						{
							color = kColor_Blue;
							backgroundColor = kColor_Gray92Percent;
							rowEditable = false;
						}
						colorArray.Add(color);
						backgroundColorArray.Add(backgroundColor);
						rowEditableArray.Add(rowEditable);
					}
					NVI_GetListView(kTable_JumpSetup).SPI_SetTable_Color(colorArray);
					NVI_GetListView(kTable_JumpSetup).SPI_SetTable_BackgroundColor(backgroundColorArray);
					NVI_GetListView(kTable_JumpSetup).SPI_SetTable_RowEditable(rowEditableArray);
					break;
				}
			}
			break;
		}
		*/
	}
	
}


void	AWindow_ModePref::NVIDO_UpdateProperty()
{
	//#427 ���[�h��
	AText	modename;
	GetApp().SPI_GetModePrefDB(mModeIndex).GetModeRawName(modename);
	NVI_SetControlText(kModeFileName,modename);
	//#305
	DisplayMultiLanguageModeName();
	//
	if( NVI_IsWindowVisible() == false )   return;
	/*#868
	{
		//SDF��ԃ��X�g��ݒ�
		ATextArray	nameArray, displaynameArray;
		AArray<AColor>	colorArray;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetStateColorArray(nameArray,colorArray,displaynameArray);
		NVI_GetListView(kListView_StateColor).SPI_BeginSetTable();
		NVI_GetListView(kListView_StateColor).SPI_SetColumn_Text(kColumn_StateName,displaynameArray);
		NVI_GetListView(kListView_StateColor).SPI_EndSetTable();
	}
	{
		ATextArray	nameArray;
		nameArray.AddFromTextArray(GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().GetCategoryNameArray());
		NVI_GetListView(kListView_CategoryColor).SPI_BeginSetTable();
		NVI_GetListView(kListView_CategoryColor).SPI_SetColumn_Text(kColumn_CategoryName,nameArray);
		NVI_GetListView(kListView_CategoryColor).SPI_EndSetTable();
	}
	*/
	//�C���f���g�������W�I�{�^���X�V
	if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == false )
	{
		NVI_SetControlBool(kRadio_Indent_InputTabCodeForIndent,true);
	}
	//
	{
		//#868 if( mToolbarMode == false )
		{
			//R0173 AItemCount	count = GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(mCurrentToolMenuObjectID);
			AItemCount	count = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolItemCount(mCurrentToolMenuObjectID);//R0173
			ATextArray	textArray;
			ATextArray	pathArray;
			AColorArray	backgroundColorArray;//#427
			AColorArray	colorArray;//#427
			AArray<ATextStyle>	styleArray;//#1316
			for( AIndex i = 0; i < count; i++ )
			{
				/*R0173 AText	text;
				GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mCurrentToolMenuObjectID,i,text);
				textArray.Add(text);
				GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(mCurrentToolMenuObjectID,i,text);
				pathArray.Add(text);*/
				AFileAcc	file;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFile(mCurrentToolMenuObjectID,i,file);
				AText	filename, path;
				file.GetFilename(filename);
				file.GetNormalizedPath(path);
				//#305 textArray.Add(filename);
				AText	toolname;//#305
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolDisplayName(mCurrentToolMenuObjectID,i,toolname);//#305
				textArray.Add(toolname);//#305
				pathArray.Add(path);
				//#427 �����X�V�c�[���͕����F�E�w�i�F��ς���
				AColor	backgroundColor = kColor_White;
				AColor	color = kColor_List_Normal;//#1316 kColor_Black;
				ATextStyle	style = kTextStyle_Normal;//#1316
				if( GetApp().SPI_GetModePrefDB(mModeIndex).GetToolAutoUpdateFlag(mCurrentToolMenuObjectID,i) == true )
				{
					//#1316 backgroundColor = kColor_Gray92Percent;
					//#1316 color = kColor_List_Blue;
					style = kTextStyle_Italic;//#1316
				}
				if( GetApp().SPI_GetModePrefDB(mModeIndex).GetToolIsStaticText(mCurrentToolMenuObjectID,i) == true )
				{
					color = kColor_Gray50Percent;
				}
				backgroundColorArray.Add(backgroundColor);
				colorArray.Add(color);
				styleArray.Add(style);//#1316
			}
			NVI_GetListView(kListView_Tool).SPI_SetEnableFileIcon(true);
			//#1012 NVI_GetListView(kListView_Tool).SPI_SetEnableIcon(false);
			NVI_GetListView(kListView_Tool).SPI_BeginSetTable();
			NVI_GetListView(kListView_Tool).SPI_SetColumn_Text(kColumn_ToolName,textArray);
			NVI_GetListView(kListView_Tool).SPI_SetTable_File(pathArray);
			NVI_GetListView(kListView_Tool).SPI_SetTable_BackgroundColor(backgroundColorArray);//#427 �w�i�F�ݒ�
			NVI_GetListView(kListView_Tool).SPI_SetTable_Color(colorArray);//#427 �����F�ݒ�
			NVI_GetListView(kListView_Tool).SPI_SetTable_TextStyle(styleArray);//#1316
			NVI_GetListView(kListView_Tool).SPI_EndSetTable();
		}
#if 0
		else
		{
			AItemCount	count = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarItemCount();
			ATextArray	textArray;
			AArray<AIconID>	iconArray;
			AColorArray	backgroundColorArray;//#427
			AColorArray	colorArray;//#427
			for( AIndex i = 0; i < count; i++ )
			{
				AToolbarItemType	type;
				AFileAcc	file;
				//AIconID	iconID;
				AText	name;
				ABool	enabled;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarItem(i,type,file,name,enabled);
				/*
				//�t�@�C���̒��g��TOOLBARICON()�R�}���h������΁A�������D�悷��
				AIconID	iconID2 = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarIconFromToolContent(file);
				if( iconID2 != kIconID_NoIcon )
				{
					iconID = iconID2;
				}
				 */
				//
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarDisplayName(i,name);//#305
				textArray.Add(name);
				//iconArray.Add(iconID);
				//#427 �����X�V�c�[���͕����F�E�w�i�F��ς���
				AColor	backgroundColor = kColor_White;
				AColor	color = kColor_List_Normal;//#1316 kColor_Black;
				if( GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarAutoUpdateFlag(i) == true )
				{
					backgroundColor = kColor_Gray92Percent;
					color = kColor_Blue;
				}
				backgroundColorArray.Add(backgroundColor);
				colorArray.Add(color);
			}
			NVI_GetListView(kListView_Tool).SPI_SetEnableFileIcon(false);
			NVI_GetListView(kListView_Tool).SPI_SetEnableIcon(true);
			NVI_GetListView(kListView_Tool).SPI_BeginSetTable();
			NVI_GetListView(kListView_Tool).SPI_SetColumn_Text(kColumn_ToolName,textArray);
			//NVI_GetListView(kListView_Tool).SPI_SetTable_Icon(iconArray);
			NVI_GetListView(kListView_Tool).SPI_SetTable_BackgroundColor(backgroundColorArray);//#427 �w�i�F�ݒ�
			NVI_GetListView(kListView_Tool).SPI_SetTable_Color(colorArray);//#427 �����F�ݒ�
			NVI_GetListView(kListView_Tool).SPI_EndSetTable();
		}
#endif
	}
}

void	AWindow_ModePref::NVIDO_Hide()
{
	//�]���̃��[�h�̓��e���t�@�C���ɕۑ�
	GetApp().SPI_GetModePrefDB(mModeIndex).SaveToFile();
}

#pragma mark ===========================

#pragma mark ---�R���g���[����Ԑ���

//#427
/**
�c�[��ContentView�̃R���g���[��Enable/Disable����
*/
void	AWindow_ModePref::SPI_UpdateToolContentViewControlGrayout( 
		const ABool inEnableToolName, const ABool inEnableToolContent,
		const ABool inEnableShortcut, const ABool inEnableGrayout, const ABool inEnableDisplay, 
		const ABool inEnableCopyToUserTool )
{
	//�O���[�v�S��
	//NVI_SetControlEnable(kGroup_Tool,true);
	
	//�c�[����
	NVI_SetControlEnable(kText_ToolName,inEnableToolName);
	NVI_SetControlEnable(kPopup_Tool_NameLanguage,inEnableToolName);
	NVI_SetControlEnable(kText_Tool_MultiLanguageName,inEnableToolName);
	//NVI_SetControlEnable(kToolCaption1,inEnableToolName);
	//NVI_SetControlEnable(kToolCaption2,inEnableToolName);
	
	//�c�[�����e
	NVI_SetControlEnable(kText_Tool,inEnableToolContent);
	//NVI_SetControlEnable(kToolCaption3,inEnableToolContent);
	
	//�V���[�g�J�b�g
	NVI_SetControlEnable(kText_Tool_Shortcut,inEnableShortcut);
	NVI_SetControlEnable(kCheck_Tool_ShortcutControlKey,inEnableShortcut);
	NVI_SetControlEnable(kCheck_Tool_ShortcutShiftKey,inEnableShortcut);
	//NVI_SetControlEnable(kToolCaption4,inEnableShortcut);
	
	//�\���ݒ�
	NVI_SetControlEnable(kButton_Tool_Display,inEnableDisplay);
	
	//�O���C�A�E�g�ݒ�
	NVI_SetControlEnable(kButton_Tool_Grayout,inEnableGrayout);
	
	//�c�[���t�@�C�����J���{�^��
	NVI_SetControlEnable(kButton_Tool_OpenToolFile,inEnableShortcut);
	
	//
	NVI_SetControlEnable(kButton_Tool_CopyToUserTool,inEnableCopyToUserTool);
	
	//�A�C�R��
	//NVI_SetControlEnable(kButton_ChangeToolButtonIcon,inEnableIcon);
}

//#427
/**
�c�[��ContentView�̃R���g���[�����e��S�N���A
*/
void	AWindow_ModePref::SPI_ClearToolContentViewControl()
{
	//��ݒ�
	//NVI_SetControlEnable(kGroup_Tool,false);
	NVI_SetControlText(kText_ToolName,GetEmptyText());
	NVI_SetControlText(kText_Tool,GetEmptyText());
	NVI_SetControlText(kText_Tool_Shortcut,GetEmptyText());
	NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,false);
	NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,false);
	NVI_SetControlBool(kButton_Tool_Display,false);
	NVI_SetControlBool(kButton_Tool_Grayout,false);
	NVI_SetControlText(kText_Tool_MultiLanguageName,GetEmptyText());
}

//#427
/**
�L�[���[�h�J�e�S��ContentView�̃R���g���[��Enable/Disable����
*/
void	AWindow_ModePref::SPI_UpdateKeywordContentViewControlGrayout()
{
	//���݃J�e�S���[�I������A���A�����X�V�̍��ڂłȂ��ꍇ�̂݁AContentView�̊e�R���g���[����enable�ɂ���
	ABool	enable = false;
	AIndex	index = NVI_GetTableViewSelectionDBIndex(kTable_KeywordCategory);//#625 �����I���͖��I������ NVI_GetListView(kTable_KeywordCategory).SPI_GetSelectedDBIndex();
	if( index != kIndex_Invalid )
	{
		if( GetApp().SPI_GetModePrefDB(mModeIndex).
					GetData_BoolArray(AModePrefDB::kAdditionalCategory_AutoUpdate,index) == false )
		{
			enable = true;
		}
	}
	//�e�R���g���[��Enable/Disable
	NVI_SetControlEnable(kText_KeywordList,enable);
	NVI_SetControlEnable(kButton_KeywordSave,enable);
	NVI_SetControlEnable(kButton_KeywordSort,enable);
	NVI_SetControlEnable(kCheck_KeywordPriorToOtherColor,enable);
	NVI_SetControlEnable(kCheck_KeywordIgnoreCase,enable);
	NVI_SetControlEnable(kCheck_KeywordRegExp,enable);
	NVI_SetControlEnable(kPopup_Keyword_ColorSlot,enable);
	NVI_SetControlEnable(kRadio_Keyword_NotUseCSVFile,enable);
	NVI_SetControlEnable(kRadio_Keyword_UseCSVFile,enable);
	NVI_SetControlEnable(kEditBox_Keyword_CSVPath,enable);
	NVI_SetControlEnable(kEditBox_Keyword_KeywordColumn,enable);
	NVI_SetControlEnable(kEditBox_Keyword_ExplanationColumn,enable);
	NVI_SetControlEnable(kEditBox_Keyword_TypeTextColumn,enable);
	NVI_SetControlEnable(kEditBox_Keyword_ParentKeywordColumn,enable);
	NVI_SetControlEnable(kEditBox_Keyword_ColorSlotColumn,enable);
	NVI_SetControlEnable(kEditBox_Keyword_CompletionTextColumn,enable);
	NVI_SetControlEnable(kEditBox_Keyword_URLColumn,enable);
	NVI_SetControlEnable(kButton_Keyword_SelectCSVFile,enable);
	NVI_SetControlEnable(kPopup_Keyword_ValidSyntaxPart,enable);
	NVI_SetControlEnable(kButton_KeywordRemoveCategory,enable);
	NVI_SetControlEnable(kButton_KeywordCategoryEdit,enable);
	//���I����Ԃ̏ꍇ�́A���W�I�{�^�����G�f�B�b�g�{�b�N�X���g�����ɐݒ肷��
	if( index == kIndex_Invalid )
	{
		NVI_SetControlBool(kRadio_Keyword_NotUseCSVFile,true);
	}
}

//#427
/**
���o��ContentView�̃R���g���[��Enable/Disable����
*/
void	AWindow_ModePref::SPI_UpdateJumpSetupContentViewControlGrayout()
{
	//���ڑI�𒆂ł���A���݂̑I�����ڂ����[�U�[�ҏW���ڂȂ�R���g���[��enable�ɂ���
	ABool	enable = false;
	AIndex	selIndex = NVI_GetTableViewSelectionDBIndex(kTable_JumpSetup);//#625 �����I���͖��I������NVI_GetListView(kTable_JumpSetup).SPI_GetSelectedDBIndex();
	if( selIndex != kIndex_Invalid )
	{
		/*#844
		if( GetApp().SPI_GetModePrefDB(mModeIndex).
					GetData_BoolArray(AModePrefDB::kJumpSetup_AutoUpdate,selIndex) == false )
					*/
		{
			enable = true;
		}
	}
	//�e�R���g���[��Enable/Disable�ݒ�
	NVI_SetControlEnable(kText_JumpSetup_RegExp,enable);
	NVI_SetControlEnable(kCheck_JumpSetup_RegisterToMenu,enable);
	//#844 NVI_SetControlEnable(kText_JumpSetup_SelectionGroup,enable);
	NVI_SetControlEnable(kCheck_JumpSetup_ColorizeLine,enable);
	NVI_SetControlEnable(kCheck_JumpSetup_RegisterAsKeyword,enable);
	NVI_SetControlEnable(kText_JumpSetup_OutlineLevel,enable);
	NVI_SetControlEnable(kRadio_JumpSetup_SimplerSetting,enable);
	NVI_SetControlEnable(kRadio_JumpSetup_RegExp,enable);
	NVI_SetControlEnable(kText_JumpSetup_LineStartText,enable);
	NVI_SetControlEnable(kCheckbox_JumpSetup_RegisterToMenu2,enable);
	//���I����Ԃ̏ꍇ�́A���W�I�{�^���𐳋K�\�����g�����ɐݒ肷��
	if( selIndex == kIndex_Invalid )
	{
		NVI_SetControlBool(kRadio_JumpSetup_RegExp,true);
	}
	//�u���o�����X�g�ƌ��o�����j���[�ɕ\������v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	shouldEnabled = ( enable && 
								 NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) == true &&
								 NVI_IsControlEnable(kCheck_JumpSetup_RegisterToMenu) == true );
		NVI_SetControlEnable(kText_JumpSetup_MenuText,shouldEnabled);
	}
	//�u���o���s�ɐF������v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	shouldEnabled = ( enable && 
								 NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) == true &&
								 NVI_IsControlEnable(kCheck_JumpSetup_ColorizeLine) == true );
		NVI_SetControlEnable(kColor_JumpSetup_LineColor,shouldEnabled);
	}
	//�u���K�\���O���[�v�Ɉ�v����P����L�[���[�h�ɂ���v�`�F�b�N�{�b�N�X�ɂ��e�R���g���[��Enable/Disable
	{
		ABool	shouldEnabled = ( enable && 
								 NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) == true &&
								 NVI_IsControlEnable(kCheck_JumpSetup_RegisterAsKeyword) == true );
		NVI_SetControlEnable(kPopup_JumpSetup_KeywordColorSlot,shouldEnabled);
		NVI_SetControlEnable(kText_JumpSetup_KeywordGroup,shouldEnabled);
	}
	//NVI_SetControlBool(kRadio_JumpSetup_RegExp,true);
	//#868 NVI_SetControlEnable(kText_JumpSetup_DisplayMultiply,enable);
	//#868 NVI_SetControlEnable(kJumpSetupCatpion1,enable);
	//#868 NVI_SetControlEnable(kJumpSetupCatpion2,enable);
	//#868 NVI_SetControlEnable(kJumpSetupCatpion3,enable);
	//#868 NVI_SetControlEnable(kJumpSetupCatpion4,enable);
	//#868 NVI_SetControlEnable(kJumpSetupCatpion5,enable);//#603
	//NVI_SetControlEnable(kCheck_JumpSetup_ColorizeRegExpGroup,enable);//#603
	//NVI_SetControlEnable(kText_JumpSetup_ColorizeRegExpGroup_Number,enable);//#603
}

/**
�L�[���[�h�^�u�̕��@�p�[�g���j���[���X�V
*/
void	AWindow_ModePref::UpdateValidSyntaxPartMenu()
{
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition();
	const AHashTextArray&	syntaxPartNameArray = syntaxDefinition.GetSyntaxPartTextArray();
	ATextArray	validSyntaxPartArray, defaultSyntaxPartArray;
	AText	t;
	t.SetLocalizedText("ValidSyntaxPartAll");
	validSyntaxPartArray.Add(t);
	t.SetLocalizedText("DefaultSyntaxPartNotDirected");
	defaultSyntaxPartArray.Add(t);
	for( AIndex i = 0; i < syntaxPartNameArray.GetItemCount(); i++ )
	{
		AText	text;
		syntaxDefinition.GetSyntaxPartName(i,text);
		validSyntaxPartArray.Add(text);
		defaultSyntaxPartArray.Add(text);
	}
	NVI_SetMenuItemsFromTextArray(kPopup_Keyword_ValidSyntaxPart,validSyntaxPartArray);
	//
	NVI_SetMenuItemsFromTextArray(AModePrefDB::kDefaultSyntaxPart,defaultSyntaxPartArray);
}

#pragma mark ===========================

#pragma mark ---ContentView �Ǘ�

/**
ContentView���e��DB�֕ۑ�
*/
void	AWindow_ModePref::NVMDO_SaveSpecializedContentView( const AControlID inTableControlID, const AIndex inRowIndex )
{
	switch(inTableControlID)
	{
	  case kTable_Keybind:
		{
			/*switch(inContentControlID)
			{
			case kPopup_KeybindAction:
			case kText_KeybindActionText:
			{*/
			/*#688 �ʂ�key�ւ̒lsave�֎~
			ABool	controlKey = NVI_GetControlBool(kCheck_Keybind_ControlKey);
			ABool	optionKey = NVI_GetControlBool(kCheck_Keybind_OptionKey);
			ABool	commandKey = NVI_GetControlBool(kCheck_Keybind_CommandKey);
			ABool	shiftKey = NVI_GetControlBool(kCheck_Keybind_ShiftKey);
			AModifierKeys	modifiers = AKeyWrapper::MakeModifierKeys(controlKey,optionKey,commandKey,shiftKey);
			*/
			//�A�N�V�����I���|�b�v�A�b�v���j���[�̓��e�擾
			ABool	changed = false;
			AIndex  keybindActionPopupIndex = NVI_GetControlNumber(kPopup_KeybindAction);
			if( keybindActionPopupIndex == 0 )
			{
                //------------------�L�[�o�C���h�ݒ薳���i�f�t�H���g�j��I�������ꍇ------------------
                //���[�h�ݒ�̊Y���L�[�o�C���h�ݒ���폜����
				changed = GetApp().SPI_GetModePrefDB(mModeIndex).
                        Remove_KeybindAction(static_cast<AKeyBindKey>(inRowIndex),mCurrentKeyBindContentViewModifiers);
			}
            else
			{
                //------------------�L�[�o�C���h��I�������ꍇ------------------
				AKeyBindAction	action = mKeybindMenuOrder.Get(keybindActionPopupIndex);
				//�A�N�V�����}��������擾
				AText	insertText;
				NVI_GetControlText(kText_KeybindActionText,insertText);
				//DB�ݒ�
				changed = GetApp().SPI_GetModePrefDB(mModeIndex).
                        SetData_KeybindAction(static_cast<AKeyBindKey>(inRowIndex),mCurrentKeyBindContentViewModifiers,
                                              action,insertText);
				NVI_SetControlEnable(kText_KeybindActionText,(action==keyAction_string));
			}
			/*
			//�}�N���o�[�X�V�i//���}�N���o�[�ɃV���[�g�J�b�g�\������ꍇ�̓R�����g�����j
			if( changed == true )
			{
				GetApp().SPI_UpdateMacroBarItemContentAll();
			}
			*/
			break;
			/*	}
			}
			break;*/
		}
		//���o���ݒ�
	  case kTable_JumpSetup:
		{
			if( inRowIndex != kIndex_Invalid )
			{
				ABool	simplerSetting = NVI_GetControlBool(kRadio_JumpSetup_SimplerSetting);
				GetApp().SPI_GetModePrefDB(mModeIndex).SetData_BoolArray(AModePrefDB::kJumpSetup_SimplerSetting,inRowIndex,simplerSetting);
			}
			break;
		}
	}
}

/**
ContentView���e��W�J����
*/
void	AWindow_ModePref::NVMDO_OpenSpecializedContentView( const AControlID inTableControlID, const AIndex inRowIndex )
{
	switch(inTableControlID)
	{
        //�L�[�o�C���h
	  case kTable_Keybind:
		{
			if( inRowIndex != kIndex_Invalid )
			{
                //------------------�e�[�u���s�I�������ꍇ------------------
                //modifier�L�[�̃`�F�b�N�{�b�N�X�̒l���擾
				ABool	controlKey = NVI_GetControlBool(kCheck_Keybind_ControlKey);
				ABool	optionKey = NVI_GetControlBool(kCheck_Keybind_OptionKey);
				ABool	commandKey = NVI_GetControlBool(kCheck_Keybind_CommandKey);
				ABool	shiftKey = NVI_GetControlBool(kCheck_Keybind_ShiftKey);
                //modifier�L�[�̌��ݏ�Ԃ��L��
				mCurrentKeyBindContentViewModifiers = AKeyWrapper::MakeModifierKeys(controlKey,optionKey,commandKey,shiftKey);//#688 �ʂ�key�ւ̒lsave�֎~
                //�I�������L�[�̃A�N�V�������擾
				AKeyBindAction	action = keyAction_NOP;
				AText	insertText;
				ABool	keybindDefault = false;//#382
				ABool	keybindBindAvailable = false;//#382
				GetApp().SPI_GetModePrefDB(mModeIndex).
                        GetData_KeyBindAction(static_cast<AKeyBindKey>(inRowIndex),mCurrentKeyBindContentViewModifiers,
                                              action,insertText,keybindDefault,keybindBindAvailable);
				if( keybindBindAvailable == true )//#382
				{
                    //==================�L�[�o�C���h�̃L�[�̏ꍇ==================
                    if( keybindDefault == true )
					{
                        //------------------�L�[�o�C���h�ݒ薳���i�f�t�H���g�j�̏ꍇ------------------
                        //�|�b�v�A�b�v���j���[�Ɂu�f�t�H���g�̓���v��\������
						NVI_SetControlEnable(kPopup_KeybindAction,true);
						NVI_SetControlNumber(kPopup_KeybindAction,0);
                        //���j���[�A�G�f�B�b�g�{�b�N�X��disable
						NVI_SetControlEnable(kText_KeybindActionText,false);
						NVI_SetControlEnable(kButton_KeyBindTextOK,false);
						NVI_SetControlText(kText_KeybindActionText,GetEmptyText());
					}
                    else
					{
						//------------------�L�[�o�C���h�ݒ�L��̏ꍇ------------------
                        //�|�b�v�A�b�v���j���[�ɃA�N�V�����ɑΉ����鍀�ڂ�\������
						NVI_SetControlEnable(kPopup_KeybindAction,true);
						NVI_SetControlNumber(kPopup_KeybindAction,mKeybindMenuOrder.Find(action));//B0000
                        
						NVI_SetControlEnable(kText_KeybindActionText,(action==keyAction_string));
						NVI_SetControlEnable(kButton_KeyBindTextOK,(action==keyAction_string));
						if( action == keyAction_string )
						{
							NVI_SwitchFocusTo(kText_KeybindActionText);
						}
						NVI_SetControlText(kText_KeybindActionText,insertText);
					}
				}
				else
				{
                    //==================�L�[�o�C���h�s�̃L�[�̏ꍇ==================
                    //�|�b�v�A�b�v���j���[�Ɂu���얳���v��\�����Adisable�ɂ���
					NVI_SetControlEnable(kPopup_KeybindAction,false);
					NVI_SetControlNumber(kPopup_KeybindAction,mKeybindMenuOrder.Find(keyAction_NOP));//B0000
					
					NVI_SetControlEnable(kText_KeybindActionText,false);
					NVI_SetControlEnable(kButton_KeyBindTextOK,false);
					NVI_SetControlText(kText_KeybindActionText,GetEmptyText());
				}
			}
			else//B0406
			{
                //------------------�e�[�u���s�I�����������ꍇ------------------
				NVI_SetControlEnable(kPopup_KeybindAction,false);
				NVI_SetControlNumber(kPopup_KeybindAction,mKeybindMenuOrder.Find(keyAction_NOP));//B0000
				
				NVI_SetControlEnable(kText_KeybindActionText,false);
				NVI_SetControlEnable(kButton_KeyBindTextOK,false);
				NVI_SetControlText(kText_KeybindActionText,GetEmptyText());
			}
			break;
		}
		//�L�[���[�h�J�e�S���[�I���E�I������
	  case kTable_KeywordCategory:
		{
			if( inRowIndex == kIndex_Invalid )
			{
				//�I�������̏ꍇ���������R�[������Ă���Ƃ��͂��łɕۑ��m�F���݂Ȃ̂ŁA�����ۑ����[�h�ŁAContentView�����
				mKeywordEditManager.TryClose(true);
			}
			else
			{
				//�I���̏ꍇ��ContentView���J��
				/*#427
				//�L�[���[�h�J�e�S���[��ObjectID�擾
				AObjectID	categoryObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(inRowIndex);
				*/
				//
				mKeywordEditManager.Open(mModeIndex,inRowIndex);//#427 categoryObjectID);//B0406 NVI_GetControlNumber(kTable_KeywordCategory));
			}
			break;
		}
		//���o��
	  case kTable_JumpSetup:
		{
			if( inRowIndex != kIndex_Invalid )
			{
				ABool	simplerSetting = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_SimplerSetting,inRowIndex);
				if( simplerSetting == true )
				{
					NVI_SetControlBool(kRadio_JumpSetup_SimplerSetting,true);
				}
				else
				{
					NVI_SetControlBool(kRadio_JumpSetup_RegExp,true);
				}
			}
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark ---�����X�V

//#427
/**
�����X�V�J�n������
*/
ABool	AWindow_ModePref::SPI_StartAutoUpdate()
{
	//�c�[��close
	if( mToolEditManager.TryClose() == false )   return false;
	
	//���X�g�I������
	SelectToolListRow(kIndex_Invalid);
	
	//�c�[���I������
	mCurrentToolMenuObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuRootObjectID();
	mToolMenuObjectIDArray.DeleteAll();
	mToolMenuPathArray.DeleteAll();
	mToolMenuDisplayPathArray.DeleteAll();
	
	//�\���X�V
	NVI_UpdateProperty();
	
	//�J�n�\��Ԃ�
	return true;
}

//#427
/**
�����X�V�I��������
*/
void	AWindow_ModePref::SPI_EndAutoUpdate()
{
	//���[�h�X�V���_�C�A���O�̃e�L�X�g�X�V�i�I���\���ɂ���j
	NVI_SetChildWindow_OK_EnableOKButton(true);
	AText	message1, message2;
	message1.SetLocalizedText("ModePref_AutoUpdateEnd1");
	message2.SetLocalizedText("ModePref_AutoUpdateEnd2");
	NVI_SetChildWindow_OK_SetText(message1,message2);
	
	//
	NVI_UpdateProperty();
}

//#427
/**
�����X�V�G���[�\��
*/
void	AWindow_ModePref::SPI_ShowAutoUpdateError()
{
	//�X�V���_�C�A���O�����
	NVI_CloseChildWindow_OK();
	
	//
	AText	message1, message2;
	message1.SetLocalizedText("ModePref_AutoUpdateError1");
	message2.SetLocalizedText("ModePref_AutoUpdateError2");
	NVI_SelectWindow();
	NVI_ShowChildWindow_OK(message1,message2);
}

#pragma mark ===========================

#pragma mark <�ĉ����\�b�h��������>

#pragma mark ===========================

void	AWindow_ModePref::NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
		const AControlID inTriggerControlID/*B0406*/ )
{
	switch(inControlID)
	{
		//#844 case AModePrefDB::kDefaultEnableAntiAlias:
	  case AModePrefDB::kControlCodeColor://#1142
	  case AModePrefDB::kBackgroundColor:
	  case AModePrefDB::kSpecialCodeColor:
	  case AModePrefDB::kDisplayBackgroundImage:
	  case AModePrefDB::kBackgroundImageTransparency:
	  case AModePrefDB::kDisplayReturnCode:
	  case AModePrefDB::kDisplayTabCode:
	  case AModePrefDB::kDisplaySpaceCode:
	  case AModePrefDB::kDisplayZenkakuSpaceCode:
	  case AModePrefDB::kDisplayGenkoyoshi:
	  case AModePrefDB::kDisplaySpellCheck://R0199
	  case AModePrefDB::kConditionalCompileDisabledColor://#467
	  case AModePrefDB::kShowUnderlineAtCurrentLine://#496
	  case AModePrefDB::kCurrentLineUnderlineColor://#496
	  case AModePrefDB::kDefaultTabWidth://#868
	  case AModePrefDB::kDefaultFontName://#868
	  case AModePrefDB::kDefaultFontSize://#868
	  case AModePrefDB::kConvert5CToA5WhenOpenJIS://#940
		{
			GetApp().NVI_RefreshAllWindow();
			break;
		}
	  case AModePrefDB::kSameAsNormalMode_BackgroundDisplay://#868
		{
			GetApp().SPI_GetModePrefDB(mModeIndex).UpdateBackgroundImage();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
	  case AModePrefDB::kSameAsNormalMode_TextLayout://#868
		{
			GetApp().SPI_UpdateTextWindowViewBoundsAll();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
	  case AModePrefDB::kVerticalMode://#558
		{
			//�s�����Čv�Z
			for( AObjectID docID = GetApp().NVI_GetFirstDocumentID(kDocumentType_Text); docID != kObjectID_Invalid; docID = GetApp().NVI_GetNextDocumentID(docID) )
			{
				if( GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex() == mModeIndex )
				{
					GetApp().SPI_GetTextDocumentByID(docID).SPI_DoWrapRelatedDataChanged(true);
				}
			}
			//���C�A�E�g�X�V
			GetApp().SPI_UpdateTextWindowViewBoundsAll();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
	  case AModePrefDB::kLetterColor:
	  case AModePrefDB::kCharsForWord:
	  case AModePrefDB::kCharsForHeadOfWord:
	  case AModePrefDB::kCharsForTailOfWord:
		{
			//
			GetApp().SPI_GetModePrefDB(mModeIndex).UpdateUserKeywordCategoryAndKeyword();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
	  case AModePrefDB::kFindHighlightColor:
	  case AModePrefDB::kFindHighlightColorPrev:
	  case AModePrefDB::kCurrentWordHighlightColor:
	  case AModePrefDB::kTextMarkerHightlightColor:
	  case AModePrefDB::kFirstSelectionColor:
	  case AModePrefDB::kDiffColorOpacity:
	  case AModePrefDB::kSelectionOpacity:
		{
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		/*
		//#493
	  case AModePrefDB::kSDFHeaderDisplayMultiply:
		{
			GetApp().SPI_UpdateJumpSetupAll(mModeIndex);
			break;
		}
		*/
	  case AModePrefDB::kSameAsNormalMode_TextDisplay://#868
	  case AModePrefDB::kLineMargin:
		{
			//#637 GetApp().SPI_UpdateTextDrawPropertyAll(mModeIndex);
			GetApp().SPI_UpdateLineImageInfo(mModeIndex);//#637
			break;
		}
		/*#844 
	  case AModePrefDB::kKinsokuBuraasge:
		{
			GetApp().SPI_RecalcWordWrapAll(mModeIndex);
			break;
		}
		*/
		
		//#844 case AModePrefDB::kDisplayInformationHeader:
		//#844 case AModePrefDB::kDisplayPositionButton:
	  case AModePrefDB::kDisplayRuler:
		//#457 case AModePrefDB::kDisplayToolbar:
		//#844 case AModePrefDB::kDisplayLineNumberButton:
		//#844 case AModePrefDB::kDisplayLineNumberButton_AsParagraph:
	  case AModePrefDB::kDisplayEachLineNumber:
	  case AModePrefDB::kDisplayEachLineNumber_AsParagraph:
	  case AModePrefDB::kRulerScaleWithTabLetter:
	  case AModePrefDB::kRulerBaseLetter:
		{
			GetApp().SPI_UpdateTextWindowViewBoundsAll();
			GetApp().NVI_RefreshAllWindow();
			break;
		}
	  case kTable_KeywordCategory:
		{
			if( inTriggerControlID == kText_KeywordList ||
						inTriggerControlID == kEditBox_Keyword_CSVPath ||
						inTriggerControlID == kEditBox_Keyword_KeywordColumn ||
						inTriggerControlID == kEditBox_Keyword_ExplanationColumn ||
						inTriggerControlID == kEditBox_Keyword_ParentKeywordColumn )
			{
				//B0406 ���X�g�I�����Ƀe�L�X�g�t�H�[�J�X�ړ��Ŗ���R�[������Ă��܂����߁B�����̕ύX���f�́u�ۑ��v�{�^�����ōs���B
				break;
			}
			switch(inModificationType)
			{
			  case kModificationType_TransactionCompleted_AddNew:
				{
					//�J�e�S���[�������j�[�N�ɂ���
					AIndex	index = GetApp().SPI_GetModePrefDB(mModeIndex).GetItemCount_Array(AModePrefDB::kAdditionalCategory_Name)-1;
					AText	name;
					GetApp().SPI_GetModePrefDB(mModeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Name,index,name);
					AText	uniqname;
					GetApp().SPI_GetModePrefDB(mModeIndex).GetUniqCategoryName(name,uniqname);
					GetApp().SPI_GetModePrefDB(mModeIndex).SetData_TextArray(AModePrefDB::kAdditionalCategory_Name,index,uniqname);
					//�L�[���[�h���X�g�X�V
					GetApp().SPI_GetModePrefDB(mModeIndex).UpdateUserKeywordCategoryAndKeyword();//#890
					GetApp().NVI_RefreshAllWindow();
					//�e�[�u���\���X�V
					NVM_UpdateDBTableView(kTable_KeywordCategory);
					break;
				}
			  case kModificationType_TransactionCompleted_DeleteRows:
			  case kModificationType_TransactionCompleted_MoveRow:
			  case kModificationType_ValueChanged:
				{
					//�L�[���[�h���X�g(AIdentifierList)�̍X�V
					GetApp().SPI_GetModePrefDB(mModeIndex).UpdateUserKeywordCategoryAndKeyword();//#890
					GetApp().NVI_RefreshAllWindow();
					break;
				}
			  default:
				{
					//��������
					//#625 �������̂���kModificationType_RowDeleted���ł�UpdateUserKeywordCategoryAndKeyword()���s���Ȃ��悤�ɂ���
					break;
				}
			}
			break;
		}
		/*#890 ���o���ݒ�͍X�V�{�^�����������Ƃ��Ƀ����o�[�ϐ��ɃR�s�[���Atextinfo�͂��̃����o�[�ϐ����Q�Ƃ���悤�ɕύX
		//#625 NVIDO_RemoveTableRow()��p�~����NotifyDataChanged�֓���
	  case kTable_JumpSetup:
		{
			switch(inModificationType)
			{
			  case kModificationType_TransactionCompleted_DeleteRows:
				{
					//#0 ���o���ݒ荀�ڂ�DB����폜�����̂ŁA�u���f�v�{�^���N���b�N���Ɠ����悤�ɔ��f���Ă����Ȃ��ƃG���[�ɂȂ�
					GetApp().SPI_GetModePrefDB(mModeIndex).MakeCategoryArrayAndKeywordList();
					GetApp().SPI_UpdateJumpSetupAll(mModeIndex);
					break;
				}
			  default:
				{
					//��������
					break;
				}
			}
			break;
		}
		*/
		/*#868
		//
	  case AModePrefDB::kDefaultWrapMode:
		{
			NVM_UpdateControlStatus();
			break;
		}
		*/
	  case kTable_Suffix:
		{
			GetApp().SPI_GetModePrefDB(mModeIndex).UpdateSuffixHash();
			break;
		}
		//�v���O�C���e�[�u��Enable/Disable�ؑ֎�
	  case kTable_Plugin:
		{
			//�v���O�C����S�čă��[�h
			GetApp().SPI_GetModePrefDB(mModeIndex).LoadPlugins();
			break;
		}
		//B0000
	  case kTable_Indent:
		{
			//#683 GetApp().SPI_UpdateIndentRegExpAll(mModeIndex);
			GetApp().SPI_UpdateRegExpForAllDocuments(mModeIndex);//#683
			break;
		}
		/*#844
		//RC2
	  case AModePrefDB::kIdInfoWindowDisplayComment:
		{
			GetApp().SPI_GetIdInfoWindow().NVI_UpdateProperty();
			break;
		}
		*/
		//R0199
	  case AModePrefDB::kSpellCheckLanguage:
	  case AModePrefDB::kSameAsNormalMode_SpellCheck:
		{
			GetApp().SPI_DeleteSpellCheckCache(mModeIndex);
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		/*#844
		//#427 �����X�V�E���[�U�[�c�[���L�������؂�ւ����f
	  case AModePrefDB::kEnableAutoUpdateTool:
	  case AModePrefDB::kEnableUserTool:
		{
			SelectToolListRow(kIndex_Invalid);
			GetApp().SPI_GetModePrefDB(mModeIndex).LoadTool();
			mCurrentToolMenuObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuRootObjectID();
			mToolMenuObjectIDArray.DeleteAll();
			mToolMenuPathArray.DeleteAll();
			NVI_UpdateProperty();
			break;
		}
		//#427 �����X�V�E���[�U�[�c�[���o�[�L�������؂�ւ����f
	  case AModePrefDB::kEnableAutoUpdateToolbar:
	  case AModePrefDB::kEnableUserToolbar:
		{
			SelectToolListRow(kIndex_Invalid);
			mCurrentToolMenuObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuRootObjectID();
			mToolMenuObjectIDArray.DeleteAll();
			mToolMenuPathArray.DeleteAll();
			GetApp().SPI_GetModePrefDB(mModeIndex).LoadToolbar();
			NVI_UpdateProperty();
			break;
		}
		//#427 �L�[���[�h�̗L�������؂�ւ����f
	  case AModePrefDB::kEnableAutoUpdateKeyword:
	  case AModePrefDB::kEnableUserKeyword:
		{
			//���[�h�ݒ�ēǂݍ���
			GetApp().SPI_GetModePrefDB(mModeIndex).SaveToFile();//��U�t�@�C���ۑ��B�iReload()�Ńt�@�C�����烍�[�h����̂Łj
			GetApp().SPI_GetModePrefDB(mModeIndex).LoadOrWaitLoadComplete();
			GetApp().NVI_UpdateAll();
			break;
		}
		//#427 ���o���̗L�������؂�ւ����f
	  case AModePrefDB::kEnableAutoUpdateJumpSetup:
	  case AModePrefDB::kEnableUserJumpSetup:
		{
			//���[�h�ݒ�ēǂݍ���
			GetApp().SPI_GetModePrefDB(mModeIndex).SaveToFile();//��U�t�@�C���ۑ��B�iReload()�Ńt�@�C�����烍�[�h����̂Łj
			GetApp().SPI_GetModePrefDB(mModeIndex).LoadOrWaitLoadComplete();
			GetApp().NVI_UpdateAll();
			break;
		}
		*/
	  case AModePrefDB::kUseLineFolding:
		{
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//#889
		//�F�X���b�g
	  case AModePrefDB::kSyntaxColorSlot0_Color:
	  case AModePrefDB::kSyntaxColorSlot0_Bold:
	  case AModePrefDB::kSyntaxColorSlot0_Italic:
	  case AModePrefDB::kSyntaxColorSlot0_Underline:
	  case AModePrefDB::kSyntaxColorSlot1_Color:
	  case AModePrefDB::kSyntaxColorSlot1_Bold:
	  case AModePrefDB::kSyntaxColorSlot1_Italic:
	  case AModePrefDB::kSyntaxColorSlot1_Underline:
	  case AModePrefDB::kSyntaxColorSlot2_Color:
	  case AModePrefDB::kSyntaxColorSlot2_Bold:
	  case AModePrefDB::kSyntaxColorSlot2_Italic:
	  case AModePrefDB::kSyntaxColorSlot2_Underline:
	  case AModePrefDB::kSyntaxColorSlot3_Color:
	  case AModePrefDB::kSyntaxColorSlot3_Bold:
	  case AModePrefDB::kSyntaxColorSlot3_Italic:
	  case AModePrefDB::kSyntaxColorSlot3_Underline:
	  case AModePrefDB::kSyntaxColorSlot4_Color:
	  case AModePrefDB::kSyntaxColorSlot4_Bold:
	  case AModePrefDB::kSyntaxColorSlot4_Italic:
	  case AModePrefDB::kSyntaxColorSlot4_Underline:
	  case AModePrefDB::kSyntaxColorSlot5_Color:
	  case AModePrefDB::kSyntaxColorSlot5_Bold:
	  case AModePrefDB::kSyntaxColorSlot5_Italic:
	  case AModePrefDB::kSyntaxColorSlot5_Underline:
	  case AModePrefDB::kSyntaxColorSlot6_Color:
	  case AModePrefDB::kSyntaxColorSlot6_Bold:
	  case AModePrefDB::kSyntaxColorSlot6_Italic:
	  case AModePrefDB::kSyntaxColorSlot6_Underline:
	  case AModePrefDB::kSyntaxColorSlot7_Color:
	  case AModePrefDB::kSyntaxColorSlot7_Bold:
	  case AModePrefDB::kSyntaxColorSlot7_Italic:
	  case AModePrefDB::kSyntaxColorSlot7_Underline:
	  case AModePrefDB::kSyntaxColorSlotComment_Color:
	  case AModePrefDB::kSyntaxColorSlotComment_Bold:
	  case AModePrefDB::kSyntaxColorSlotComment_Italic:
	  case AModePrefDB::kSyntaxColorSlotComment_Underline:
	  case AModePrefDB::kSyntaxColorSlotLiteral_Color:
	  case AModePrefDB::kSyntaxColorSlotLiteral_Bold:
	  case AModePrefDB::kSyntaxColorSlotLiteral_Italic:
	  case AModePrefDB::kSyntaxColorSlotLiteral_Underline:
		//#889 case AModePrefDB::kUseColorScheme:
	  case AModePrefDB::kColorSchemeName:
	  case AModePrefDB::kDarkenImportLightenLocal:
	  case AModePrefDB::kSameAsNormalMode_Colors://#868
		{
			UpdateColors();
			break;
		}
		//�t�H���g
	  case AModePrefDB::kSameAsNormalMode_Font://#868
		{
			GetApp().SPI_TextFontSetInModeIsUpdatedAll(mModeIndex);
			GetApp().NVI_RefreshAllWindow();
			break;
		}
		//�L�[�o�C���h�x�[�X�I�����W�I�{�^��
	  case kRadio_KeyBind_DefaultIsAppKeyBind:
	  case AModePrefDB::kKeyBindDefaultIsNormalMode:
	  case AModePrefDB::kKeyBindDefaultIsOSKeyBind:
		{
			//���݂̑}���������ۑ�����i�I�[�v�������Ƃ���control�L�[���̏�Ԃ֕ۑ������j
			NVM_SaveContentView(kTable_Keybind);
			//
			NVM_UpdateDBTableView(kTable_Keybind);
			break;
		}
		//���[�henable/disable
	  case AModePrefDB::kEnableMode:
		{
			//�����v���W�F�N�g�̃f�[�^�X�V�i�����郂�[�h���ς��̂Łj
			GetApp().SPI_UpdateAllSameProjectData();
			//���[�h���j���[�X�V
			GetApp().SPI_UpdateModeMenu();
			//���[�hsuffix�n�b�V���X�V
			GetApp().SPI_UpdateModeSuffixArray(mModeIndex);
			break;
		}
		//�A�v�����g�ݍ���default���[�h����̍X�V �X�V�����[�h�I��
	  case AModePrefDB::kModeUpdateSourceModeName:
		{
			GetApp().SPI_GetModePrefDB(mModeIndex).SaveToFile();//���܂ł̐ݒ�f�[�^��ۑ�
			GetApp().SPI_GetModePrefDB(mModeIndex).LoadOrWaitLoadComplete();//���[�h
			//�c�[�����X�g�f�[�^������
			mCurrentToolMenuObjectID = GetApp().SPI_GetModePrefDB(mModeIndex).GetToolMenuRootObjectID();
			mToolMenuObjectIDArray.DeleteAll();
			mToolMenuPathArray.DeleteAll();
			mToolMenuDisplayPathArray.DeleteAll();
			break;
		}
	  default:
		{
			//��������
			break;
		}
	}
}

//#1239
/**
�t�@�C���֕ۑ�
*/
void	AWindow_ModePref::NVMDO_NotifyDataChangedForSave()
{
	GetApp().SPI_GetModePrefDB(mModeIndex).SaveToFile();
}

/**
�F�X�V������
*/
void	AWindow_ModePref::UpdateColors()
{
	if( mModeIndex == kStandardModeIndex )
	{
		//------------------�W�����[�h�̏ꍇ�̓��[�h�ς݂̑S���[�h�̐F�����X�V------------------
		for( AIndex i = 0; i < GetApp().SPI_GetModeCount(); i++ )
		{
			if( GetApp().SPI_GetModePrefDB(i,false).IsLoaded() == true )
			{
				//#1316 GetApp().SPI_GetModePrefDB(i).UpdateColorScheme();
				GetApp().SPI_GetModePrefDB(i).UpdateUserKeywordCategoryColor();
				GetApp().SPI_GetModePrefDB(i).UpdateSyntaxDefinitionStateColorArray();
				GetApp().SPI_GetModePrefDB(i).UpdateSyntaxDefinitionCategoryColor();
			}
		}
	}
	else
	{
		//------------------�W�����[�h�ȊO�̏ꍇ�͂��̃��[�h�̐F�����X�V------------------
		//#1316 GetApp().SPI_GetModePrefDB(mModeIndex).UpdateColorScheme();
		GetApp().SPI_GetModePrefDB(mModeIndex).UpdateUserKeywordCategoryColor();
		GetApp().SPI_GetModePrefDB(mModeIndex).UpdateSyntaxDefinitionStateColorArray();
		GetApp().SPI_GetModePrefDB(mModeIndex).UpdateSyntaxDefinitionCategoryColor();
	}
	//�`��X�V
	GetApp().NVI_RefreshAllWindow();
}

#pragma mark ===========================

#pragma mark ---������

void	AWindow_ModePref::NVIDO_ListViewRowMoved( const AControlID inControlID, const AIndex inOldIndex, const AIndex inNewIndex )
{
	switch(inControlID)
	{
	  case kListView_Tool:
		{
			//#868 if( mToolbarMode == false )
			{
				//
				/*R0173 ATextArray	actiontextarray;
				NVI_GetListView(kListView_Tool).SPI_GetTable_File(actiontextarray);
				GetApp().SPI_GetModePrefDB(mModeIndex).MoveToolItem(mCurrentToolMenuObjectID,actiontextarray);*/
				GetApp().SPI_GetModePrefDB(mModeIndex).MoveToolItem(mCurrentToolMenuObjectID,inOldIndex,inNewIndex);
			}
			/*#868
			else
			{
				GetApp().SPI_GetModePrefDB(mModeIndex).MoveToolbarItem(inOldIndex,inNewIndex);
			}
			*/
			break;
		}
	}
}

void	AWindow_ModePref::NVIDO_ListViewFileDropped( const AControlID inControlID, const AIndex inRowIndex, const AFileAcc& inFile )
{
	switch(inControlID)
	{
		//#868
		//���[�h����t�@�C���p�X�ւ̃t�@�C���h���b�v
	  case kTable_ModeSelectFilePath:
		{
			AText	path;
			inFile.GetPath(path);
			GetApp().SPI_GetModePrefDB(mModeIndex).Insert1_TextArray(AModePrefDB::kModeSelectFilePath,inRowIndex,path);
			NVM_UpdateDBTableView(kTable_ModeSelectFilePath);
			break;
		}
		//
	  case kListView_Tool:
		{
			//#868 if( mToolbarMode == false )
			{
				AText	text;
				mToolMenuPathArray.Implode('/',text);
				AFileAcc	toolfolder;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolFolder(false,toolfolder);
				AFileAcc	folder;
				folder.SpecifyChild(toolfolder,text);
				AText	filename;
				inFile.GetFilename(filename);
				AFileAcc	newFile;
				newFile.SpecifyUniqChildFile(folder,filename);
				
				//R0000 security
				AText	filelist, report;
				if( GetApp().SPI_ScreenModeExecutable(inFile,filelist,report) == true )
				{
					GetApp().SPI_ShowModeExecutableAlertWindow_Tool(mModeIndex,mCurrentToolMenuObjectID,inRowIndex,inFile,newFile,filelist,report);
					break;
				}
				
				if( inFile.IsFolder() == false )
				{
					inFile.CopyFileTo(newFile,true);
				}
				else
				{
					inFile.CopyFolderTo(newFile,true,true);
				}
				GetApp().SPI_GetModePrefDB(mModeIndex).InsertToolItem(mCurrentToolMenuObjectID,inRowIndex,newFile,true,true,false);
				NVI_UpdateProperty();
			}
			/*#868
			else
			{
				AFileAcc	toolbarFolder;
				GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(false,toolbarFolder);
				AText	filename;
				inFile.GetFilename(filename);
				AFileAcc	newFile;
				newFile.SpecifyUniqChildFile(toolbarFolder,filename);
				
				//R0000 security
				AText	filelist, report;
				if( GetApp().SPI_ScreenModeExecutable(inFile,filelist,report) == true )
				{
					SPI_ShowModeExecutableAlertWindow_Toolbar(mModeIndex,inRowIndex,inFile,newFile,filelist,report);
					break;
				}
				
				if( inFile.IsFolder() == false )
				{
					inFile.CopyFileTo(newFile,true);
					GetApp().SPI_GetModePrefDB(mModeIndex).InsertToolbarItem(inRowIndex,kToolbarItemType_File,newFile,false);//#427
				}
				else
				{
					inFile.CopyFolderTo(newFile,true,true);
					//R0137
					if( newFile.IsBundleFolder() == true )
					{
						GetApp().SPI_GetModePrefDB(mModeIndex).InsertToolbarItem(inRowIndex,kToolbarItemType_File,newFile,false);//#427
					}
					else
					GetApp().SPI_GetModePrefDB(mModeIndex).InsertToolbarItem(inRowIndex,kToolbarItemType_Folder,newFile,false);//#427
				}
				NVI_UpdateProperty();
			}
			*/
			break;
		}
	}
}

void	AWindow_ModePref::TestRecursive( AFileAcc& inOld, AFileAcc& inNew, AText& outText )
{
	AObjectArray<AFileAcc>	old_children;
	inOld.GetChildren(old_children);
	for( AIndex i = 0; i < old_children.GetItemCount(); i++ )
	{
		AFileAcc	old_child;
		old_child.CopyFrom(old_children.GetObject(i));
		AText	filename;
		old_child.GetFilename(filename,false);
		AFileAcc	new_child;
		new_child.SpecifyChild(inNew,filename);
		if( old_child.IsFolder() )
		{
			TestRecursive(old_child,new_child,outText);
		}
		else
		{
			AText	old_text, new_text;
			old_child.ReadTo(old_text);
			new_child.ReadTo(new_text);
			if( old_text.Compare(new_text) == false )
			{
				AText	path;
				old_child.GetPath(path,kFilePathType_1);
				outText.AddText(path);
				outText.Add(13);
			}
		}
	}
}

void	AWindow_ModePref::Test()
{
	AFileAcc	o, n;
	AText	on, nn;
	on.SetCstring("/cmp/old");
	nn.SetCstring("/cmp/new");
	o.Specify(on);
	n.Specify(nn);
	AText	text;
	TestRecursive(o,n,text);
	NVI_SetControlText(kText_JumpSetup_RegExp,text);
}

ABool	AWindow_ModePref::AttemptQuit()
{
	if( NVI_IsWindowVisible() == true )//B0343 TryCloseCurrentMode()�̂Ȃ���ModePref�ۑ����s���Ă��邪�A��x���E�C���h�E���J���Ă��Ȃ���Ԃł�������Ƃ܂���
	{
		//B0303
		if( TryCloseCurrentMode() == false )   return false;
	}
	//
	NVI_Close();
	
	return true;
}

//B0406
/*
�d�v�ȃ|�C���g�F
TextEdit View�ւ̊i�[�E�ǂݏo�����ꌳ������B
�ʂ̏ꏊ����́ATextEdit View�ւ̊i�[�E�ǂݏo�����s��Ȃ����Ƃɂ���āA
TextEdit View�̓��e�ƁA���݂̃��[�h�E�J�e�S���[�i�ݒ�f�[�^�̊i�[�E�ǂݍ��ݐ�j������Ȃ����Ƃ�ۏ؂���B
�f�[�^�̊i�[��i�߂���j�́AObjectID�ŋL������BObjectID�ł���΁A�J�e�S���[�̍폜��Index�̈ړ��������Ă��A�K������TextArray�ɖ߂���邱�Ƃ��ۏ؂����B
�u�J�����Ƃ���֕ۑ�����v��ۏ؂���
*/
#pragma mark ===========================
#pragma mark [�N���X]AKeywordEditManager
#pragma mark ===========================
//�L�[���[�h����ContentView�Ǘ�
AKeywordEditManager::AKeywordEditManager( AWindow_ModePref& inModePrefWindow ) 
: mDirty(false), mCurrentModeIndex(kIndex_Invalid), //#427 mCurrentCategoryObjectID(kObjectID_Invalid), 
		mModePrefWindow(inModePrefWindow), mOpened(false)
,mCurrentCategoryIndex(kIndex_Invalid)//#427
{
}
AKeywordEditManager::~AKeywordEditManager()
{
}

//�J��
void	AKeywordEditManager::Open( const AIndex inModeIndex, const AIndex inCategoryIndex )//#427 const AObjectID inCategoryObjectID )
{
	//inCategoryObjectID��Invalid�̂Ƃ��i�e�[�u���I�������j�́Aclose�����i�����ۑ��j
	//#427if( inCategoryObjectID == kObjectID_Invalid )
	if( inCategoryIndex == kIndex_Invalid )//#427
	{
		TryClose(true);
		return;
	}
	
	//����Open�̏ꍇ
	if( mOpened == true )
	{
		if( inModeIndex == mCurrentModeIndex && inCategoryIndex == mCurrentCategoryIndex )//#427 inCategoryObjectID == mCurrentCategoryObjectID )
		{
			//���ɓ����f�[�^��Open�ς݂Ȃ炻�̂܂�return
			return;
		}
		else
		{
			//�ʂ̃f�[�^��Open�ς݂Ȃ����i�����ۑ��j
			TryClose(true);
		}
	}
	
	//���݂̃��[�h�E�J�e�S���[���X�V����
	mCurrentModeIndex = inModeIndex;
	//#427 mCurrentCategoryObjectID = inCategoryObjectID;
	mCurrentCategoryIndex = inCategoryIndex;//#427
	//
	mOpened = true;
	
	//�f�[�^��ContentView�ɕ\������
	AText	text;
	//#427 GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetAdditionalKeywordForModePrefWindow(mCurrentCategoryObjectID,text);
	GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,
				mCurrentCategoryIndex,text);//#427
	mModePrefWindow.NVI_SetControlText(kText_KeywordList,text);
	//#427 mModePrefWindow.NVI_SetControlEnable(kText_KeywordList,true);
	mModePrefWindow.SPI_UpdateKeywordContentViewControlGrayout();//#427
	
	//#135 
	mModePrefWindow.NVI_ClearControlUndoInfo(kText_KeywordList);
	
	//#791
	//CSV�g�p�ݒ�ɂ�蕪��
	if( GetApp().SPI_GetModePrefDB(mCurrentModeIndex).
				GetData_BoolArray(AModePrefDB::kAdditionalCategory_UseCSV,mCurrentCategoryIndex) == true )
	{
		//------------------CSV�L�[���[�h------------------
		mModePrefWindow.NVI_SetControlBool(kRadio_Keyword_UseCSVFile,true);
		mModePrefWindow.NVI_SwitchFocusTo(kEditBox_Keyword_CSVPath);
	}
	else
	{
		//------------------�ʏ�L�[���[�h------------------
		mModePrefWindow.NVI_SetControlBool(kRadio_Keyword_NotUseCSVFile,true);
		mModePrefWindow.NVI_SwitchFocusTo(kText_KeywordList);
	}
	
	//#868
	//�����X�V���ǂ����ɂ���Đ������̐ؑ�
	//#1373 AText	categoryKindText;
	if( GetApp().SPI_GetModePrefDB(mCurrentModeIndex).
	   GetData_BoolArray(AModePrefDB::kAdditionalCategory_AutoUpdate,mCurrentCategoryIndex) == true )
	{
		//#1373 categoryKindText.SetLocalizedText("ModePref_KeywordKind_Embed");
		mModePrefWindow.NVI_SetShowControl(kStaticText_CategoryKind,true);
	}
	else
	{
		//#1373 categoryKindText.SetLocalizedText("ModePref_KeywordKind_User");
		mModePrefWindow.NVI_SetShowControl(kStaticText_CategoryKind,false);
	}
	//#1373 mModePrefWindow.NVI_SetControlText(kStaticText_CategoryKind,categoryKindText);
}

//����
ABool	AKeywordEditManager::TryClose( const ABool inForceSave )
{
	//Open���Ă��Ȃ���΂��̂܂�return
	if( mOpened == false )   return true;
	
	//CSV�t�@�C�����Acontent���e��DB�ɕۑ����� #688
	mModePrefWindow.NVI_SaveContentView(kTable_KeywordCategory);
	
	//���ۑ��Ȃ�ۑ��_�C�A���O�\��
	if( mDirty == true )
	{
		if( inForceSave == false )
		{
			//���݂�ContentView�̓��e�ɁA�e�[�u���̑I���s�̂ق������킹��
			/*427 
			AIndex	categoryIndex = GetApp().SPI_GetModePrefDB(mCurrentModeIndex).
					GetAdditionalKeywordCategoryIndexByObjectID(mCurrentCategoryObjectID);
			mModePrefWindow.NVI_SetControlNumber(kTable_KeywordCategory,categoryIndex);
			*/
			mModePrefWindow.NVI_GetListView(kTable_KeywordCategory).SPI_SetSelectByDBIndex(mCurrentCategoryIndex);
			//�ۑ��_�C�A���O�\��
			mModePrefWindow.ShowSaveKeywordAlertSheet();
			return false;
		}
		else
		{
			Save();
		}
	}
	
	//#791
	//CSV�ݒ��DB�ɕۑ�
	SaveUseCSV();
	
	//���݂̃��[�h�E�J�e�S���[��Invalid�ɂ���i������Ԃ֑J�ځj
	mCurrentModeIndex = kIndex_Invalid;
	//#427 mCurrentCategoryObjectID = kObjectID_Invalid;
	mCurrentCategoryIndex = kIndex_Invalid;//#427
	//
	mOpened = false;
	
	//#1373
	mModePrefWindow.NVI_SetShowControl(kStaticText_CategoryKind,false);
	
	//��f�[�^��ContentView�ɕ\������
	mModePrefWindow.NVI_SetControlText(kText_KeywordList,GetEmptyText());
	//#427 mModePrefWindow.NVI_SetControlEnable(kText_KeywordList,false);
	mModePrefWindow.SPI_UpdateKeywordContentViewControlGrayout();//#427
	
	return true;
}

//�ۑ�
void	AKeywordEditManager::Save()
{
	if( mOpened == true )
	{
		//View����f�[�^�擾
		AText	text;
		mModePrefWindow.NVI_GetControlText(kText_KeywordList,text);
		//ModePrefDB�Ƀf�[�^�ݒ�
		//#427 GetApp().SPI_GetModePrefDB(mCurrentModeIndex).SetAdditionalKeywordFromModePrefWindow(mCurrentCategoryObjectID,text);
		GetApp().SPI_GetModePrefDB(mCurrentModeIndex).SetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,
					mCurrentCategoryIndex,text);//#427
	}
	//Dirty����
	SetDirty(false);
}

//#791
/**
CSV�ݒ�̃��W�I�{�^���̒l��DB�ɕۑ�
*/
void	AKeywordEditManager::SaveUseCSV()
{
	if( mModePrefWindow.NVI_GetControlBool(kRadio_Keyword_UseCSVFile) == true )
	{
		GetApp().SPI_GetModePrefDB(mCurrentModeIndex).
				SetData_BoolArray(AModePrefDB::kAdditionalCategory_UseCSV,mCurrentCategoryIndex,true);
	}
	else
	{
		GetApp().SPI_GetModePrefDB(mCurrentModeIndex).
				SetData_BoolArray(AModePrefDB::kAdditionalCategory_UseCSV,mCurrentCategoryIndex,false);
	}
}

//#8 
//�L�[���[�h��Revert����
void	AKeywordEditManager::Revert()
{
	if( mOpened == true )
	{
		//�f�[�^��ContentView�ɕ\������
		AText	text;
		//#427 GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetAdditionalKeywordForModePrefWindow(mCurrentCategoryObjectID,text);
		GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,
					mCurrentCategoryIndex,text);//#427
		mModePrefWindow.NVI_SetControlText(kText_KeywordList,text);
		//#427 mModePrefWindow.NVI_SetControlEnable(kText_KeywordList,true);
		mModePrefWindow.NVI_SwitchFocusTo(kText_KeywordList);
	}
	//Dirty����
	SetDirty(false);
}

void	AKeywordEditManager::SetDirty( const ABool inDirty )
{
	//Dirty�ݒ�
	mDirty = inDirty;
	//�E�C���h�E��Dirty�\���ݒ�
	mModePrefWindow.NVI_SetModified(mDirty);
	//#427
	mModePrefWindow.SPI_UpdateKeywordContentViewControlGrayout();
}

void	AKeywordEditManager::Sort()
{
	//TextView�̓��e���擾�A�\�[�g�A�ݒ�
	AText	text;
	mModePrefWindow.NVI_GetControlText(kText_KeywordList,text);
	ATextArray	textArray;
	text.Explode(kUChar_LineEnd,textArray);
	textArray.Sort(true);
	while( textArray.GetItemCount() > 0 )
	{
		if( textArray.GetTextConst(0).GetItemCount() == 0 )   textArray.Delete1(0);
		else break;
	}
	textArray.Implode(kUChar_LineEnd,text);
	mModePrefWindow.NVI_SetControlText(kText_KeywordList,text);
	//
	SetDirty(true);
}

#pragma mark ===========================
#pragma mark [�N���X]AToolEditManager
#pragma mark ===========================
//Tool�ҏWContentView�Ǘ�
AToolEditManager::AToolEditManager( AWindow_ModePref& inModePrefWindow ) 
: mModePrefWindow(inModePrefWindow), mDirty(false), mOpened(false) //#868 mCurrentIsToobar(false)
,mCurrentToolMenuObjectID(kObjectID_Invalid), mCurrentToolIndex(kIndex_Invalid)//#427
,mCurrentModeIndex(kIndex_Invalid)//#427
{
}

AToolEditManager::~AToolEditManager()
{
}

//
void	AToolEditManager::Open( const AModeIndex inModeIndex, //#868 const ABool inToolbar, //#427 const AFileAcc& inFile )
			const AObjectID inToolMenuObjectID, const AIndex inToolIndex )//#427
{
	//����Open�̏ꍇ�̏���
	if( mOpened == true )
	{
		if( inModeIndex == mCurrentModeIndex && //#868 inToolbar == mCurrentIsToobar && //#427 && inFile.Compare(mCurrentFile) == true )
					inToolMenuObjectID == mCurrentToolMenuObjectID && inToolIndex == mCurrentToolIndex )
		{
			//���ɓ����f�[�^��Open�ς݂Ȃ炻�̂܂�return
			return;
		}
		else
		{
			//�ʂ̃f�[�^��Open�ς݂Ȃ����i�����ۑ��j
			TryClose(true);
		}
	}
	
	//�c�[���f�[�^�ǂݍ��݁AmCurrentFile�ݒ�A�c�[���o�[�̃^�C�v����
	//#427 �R�[�����̏�����������Ɉړ��i���t�@�N�^�����O�j
	ABool	toolEnabled = true;
	ABool	toolGrayout = false;
	ABool	autoupdate = false;
	ANumber	shortcut = 0;
	AMenuShortcutModifierKeys	modifiers = 0;
	//#868 if( inToolbar == false )
	{
		//�c�[���t�@�C���擾
		GetApp().SPI_GetModePrefDB(inModeIndex).GetToolFile(inToolMenuObjectID,inToolIndex,mCurrentFile);
		
		//�e��f�[�^�ǂݍ���
		autoupdate = GetApp().SPI_GetModePrefDB(inModeIndex).GetToolAutoUpdateFlag(inToolMenuObjectID,inToolIndex);
		toolEnabled = GetApp().SPI_GetModePrefDB(inModeIndex).IsToolEnabled(inToolMenuObjectID,inToolIndex);
		toolGrayout = GetApp().SPI_GetModePrefDB(inModeIndex).IsToolGrayout(inToolMenuObjectID,inToolIndex);
		GetApp().SPI_GetModePrefDB(inModeIndex).GetToolShortcut(inToolMenuObjectID,inToolIndex,shortcut,modifiers);
		
		//StaticText�̏ꍇ�A�S�ăO���C�A�E�g #427
		if( GetApp().SPI_GetModePrefDB(inModeIndex).GetToolIsStaticText(inToolMenuObjectID,inToolIndex) == true )
		{
			mModePrefWindow.SPI_UpdateToolContentViewControlGrayout(false,false,false,false,false,false);
			return;
		}
	}
#if 0
	else
	{
		//�c�[���o�[�t�@�C���擾
		AFileAcc	file;
		AToolbarItemType	type = kToolbarItemType_File;
		//AIconID	iconID = kIconID_NoIcon;
		AText	name;
		GetApp().SPI_GetModePrefDB(inModeIndex).GetToolbarItem(inToolIndex,type,file,name,toolEnabled);
		
		//�e��f�[�^�ǂݍ���
		autoupdate = GetApp().SPI_GetModePrefDB(inModeIndex).GetToolbarAutoUpdateFlag(inToolIndex);
		
		//�t�@�C���E�t�H���_�ȊO�̃c�[���o�[���ځi�g�ݍ��݃c�[���o�[�{�^�����j�̏ꍇ�A
		//�L���`�F�b�N�{�b�N�X�����ݒ�ł���悤�ɂ���
		if( type != kToolbarItemType_File && type != kToolbarItemType_Folder )
		{
			//�L���`�F�b�N�{�b�N�X�����ݒ�ł���悤�ɂ���
			//#427 mModePrefWindow.NVI_SetControlEnable(kGroup_Tool,true);
			mModePrefWindow.NVI_SetControlText(kText_ToolName,name);
			mModePrefWindow.NVI_SetControlText(kText_Tool,GetEmptyText());
			mModePrefWindow.NVI_SetControlText(kText_Tool_Shortcut,GetEmptyText());
			mModePrefWindow.NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,false);
			mModePrefWindow.NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,false);
			//�O���C�A�E�g����
			/*#427
			mModePrefWindow.NVI_SetControlEnable(kText_ToolName,false);
			mModePrefWindow.NVI_SetControlEnable(kText_Tool,false);
			mModePrefWindow.NVI_SetControlEnable(kText_Tool_Shortcut,false);
			mModePrefWindow.NVI_SetControlEnable(kPopup_Tool_NameLanguage,false);//#447
			mModePrefWindow.NVI_SetControlEnable(kText_Tool_MultiLanguageName,false);//#305
			mModePrefWindow.NVI_SetControlEnable(kCheck_Tool_ShortcutControlKey,false);
			mModePrefWindow.NVI_SetControlEnable(kCheck_Tool_ShortcutShiftKey,false);
			mModePrefWindow.NVI_SetControlEnable(kButton_ChangeToolButtonIcon,false);
			mModePrefWindow.NVI_SetControlEnable(kToolCaption1,false);//#427
			mModePrefWindow.NVI_SetControlEnable(kToolCaption2,false);//#427
			mModePrefWindow.NVI_SetControlEnable(kToolCaption3,false);//#427
			mModePrefWindow.NVI_SetControlEnable(kToolCaption4,false);//#427
			*/
			mModePrefWindow.SPI_UpdateToolContentViewControlGrayout(false,false,false,false,true,false);//#427
			return;
		}
		
		//
		mCurrentFile = file;
	}
#endif
	
	//���݂̃��[�h�A�t�@�C�����X�V
	mCurrentModeIndex = inModeIndex;
	//#868 mCurrentIsToobar = inToolbar;
	//#427 mCurrentFile.CopyFrom(inFile);
	mCurrentToolMenuObjectID = inToolMenuObjectID;//#427
	mCurrentToolIndex = inToolIndex;//#427
	
	//
	mOpened = true;
	
	//ContentView�X�V
	
	/*#427 �O���C�A�E�g����͂܂Ƃ߂�
	//Caption�����O���C�A�E�g����
	mModePrefWindow.NVI_SetControlEnable(kToolCaption1,true);//#427
	mModePrefWindow.NVI_SetControlEnable(kToolCaption2,true);//#427
	mModePrefWindow.NVI_SetControlEnable(kToolCaption3,true);//#427
	mModePrefWindow.NVI_SetControlEnable(kToolCaption4,true);//#427
	*/
	
	//�L���^�����`�F�b�N�{�b�N�X
	mModePrefWindow.NVI_SetControlBool(kButton_Tool_Display,toolEnabled);
	//#427 mModePrefWindow.NVI_SetControlEnable(kButton_Tool_Display,true);
	
	//�O���C�A�E�g�`�F�b�N�{�b�N�X
	mModePrefWindow.NVI_SetControlBool(kButton_Tool_Grayout,toolGrayout);
	//#427 mModePrefWindow.NVI_SetControlEnable(kButton_Tool_Grayout,true);
	
	/*#427 �O���C�A�E�g����͂܂Ƃ߂�
	//�O���[�v
	mModePrefWindow.NVI_SetControlEnable(kGroup_Tool,true);
	*/
	
	//�c�[����
	//#427 mModePrefWindow.NVI_SetControlEnable(kText_ToolName,true);
	AText	filename;
	mCurrentFile.GetFilename(filename);
	mModePrefWindow.NVI_SetControlText(kText_ToolName,filename);
	/*#688
	if( CWindowImp::STATIC_GetIsFocusGroup2ndPattern() == false )//#353 �t���L�[�{�[�h�t�H�[�J�X�̏ꍇ�̓t�H�[�J�X�ړ����Ȃ�
	{
		mModePrefWindow.NVI_SwitchFocusTo(kText_ToolName);
	}
	 */
	//#427 mModePrefWindow.NVI_SetControlEnable(kText_Tool_MultiLanguageName,true);//#305
	//#427 mModePrefWindow.NVI_SetControlEnable(kPopup_Tool_NameLanguage,true);//#447
	//
	if( mCurrentFile.IsFolder() == true )
	{
		//�t�H���_
		/*#427 �O���C�A�E�g����͂܂Ƃ߂�
		//�O���C�A�E�g����
		mModePrefWindow.NVI_SetControlEnable(kText_Tool,false);
		mModePrefWindow.NVI_SetControlEnable(kText_Tool_Shortcut,false);
		mModePrefWindow.NVI_SetControlEnable(kCheck_Tool_ShortcutControlKey,false);
		mModePrefWindow.NVI_SetControlEnable(kCheck_Tool_ShortcutShiftKey,false);
		mModePrefWindow.NVI_SetControlEnable(kButton_ChangeToolButtonIcon,false);
		mModePrefWindow.NVI_SetControlEnable(kToolCaption3,false);//#427
		mModePrefWindow.NVI_SetControlEnable(kToolCaption4,false);//#427
		*/
		//
		mModePrefWindow.NVI_SetControlText(kText_Tool,GetEmptyText());
		mModePrefWindow.NVI_SetControlText(kText_Tool_Shortcut,GetEmptyText());//B0321
		mModePrefWindow.NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,false);
		mModePrefWindow.NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,false);
		
		//#427 �O���C�A�E�g����i�t�H���_�j
		if( autoupdate == true )
		{
			//�����X�V�c�[���i�t�H���_�j
			mModePrefWindow.SPI_UpdateToolContentViewControlGrayout(false,false,false,false,true,false);
		}
		else
		{
			//���[�U�[�ҏW�c�[���i�t�H���_�j
			mModePrefWindow.SPI_UpdateToolContentViewControlGrayout(true,false,false,true,true,false);
		}
	}
	else
	{
		//�t�@�C��
		//
		//#427 mModePrefWindow.NVI_SetControlEnable(kText_Tool,true);
		//#868 if( mCurrentIsToobar == false )
		{
			//�c�[��
			/*#427
			//
			mModePrefWindow.NVI_SetControlEnable(kText_Tool_Shortcut,true);
			mModePrefWindow.NVI_SetControlEnable(kCheck_Tool_ShortcutControlKey,true);
			mModePrefWindow.NVI_SetControlEnable(kCheck_Tool_ShortcutShiftKey,true);
			mModePrefWindow.NVI_SetControlEnable(kButton_ChangeToolButtonIcon,false);
			*/
			//�V���[�g�J�b�g
			/*#427��ֈړ�
			ANumber	shortcut = 0;
			AMenuShortcutModifierKeys	modifiers = 0;
			GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetToolShortcut(mCurrentFile,shortcut,modifiers);
			*/
			if( shortcut != 0 )
			{
				AText	shortcuttext;
				shortcuttext.Add(shortcut);
				mModePrefWindow.NVI_SetControlText(kText_Tool_Shortcut,shortcuttext);
				mModePrefWindow.NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,AKeyWrapper::IsControlKeyOnMenuShortcut(modifiers));
				mModePrefWindow.NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,AKeyWrapper::IsShiftKeyOnMenuShortcut(modifiers));
			}
			else
			{
				mModePrefWindow.NVI_SetControlText(kText_Tool_Shortcut,GetEmptyText());
				mModePrefWindow.NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,false);
				mModePrefWindow.NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,false);
			}
			//���e
			Open_ToolText();
		}
#if 0
		else
		{
			//�c�[���o�[
			/*#427
			//
			mModePrefWindow.NVI_SetControlEnable(kText_Tool_Shortcut,false);
			mModePrefWindow.NVI_SetControlEnable(kCheck_Tool_ShortcutControlKey,false);
			mModePrefWindow.NVI_SetControlEnable(kCheck_Tool_ShortcutShiftKey,false);
			*/
			mModePrefWindow.NVI_SetControlText(kText_Tool_Shortcut,GetEmptyText());
			//#427 mModePrefWindow.NVI_SetControlEnable(kButton_ChangeToolButtonIcon,true);
			//���e
			Open_ToolText();
		}
#endif
		
		//#427 �O���C�A�E�g����i�t�@�C���j
		if( autoupdate == true )
		{
			//�����X�V�c�[���i�t�@�C���j
			mModePrefWindow.SPI_UpdateToolContentViewControlGrayout(false,false,true,false,true,true);
		}
		else
		{
			//���[�U�[�ҏW�c�[���i�t�@�C���j
			ABool	enableContent = true;
#if IMPLEMENTATION_FOR_MACOSX
			if( mCurrentFile.IsAppleScriptFile() == true )
			{
				enableContent = false;
			}
#endif
			mModePrefWindow.SPI_UpdateToolContentViewControlGrayout(true,enableContent,true,
						(mCurrentIsToobar==false),true,false);
		}
	}
	mModePrefWindow.NVI_ClearControlUndoInfo(kText_Tool);//B0323
	
	//�����X�V���ǂ����ɂ���Đ������ؑ�
	//#1373 AText	toolKindText;
	if( autoupdate == true )
	{
		//#1373 toolKindText.SetLocalizedText("ModePref_ToolKind_Embed");
		mModePrefWindow.NVI_SetShowControl(kStaticText_ToolKind,true);
		mModePrefWindow.NVI_SetShowControl(kButton_Tool_CopyToUserTool,true);
	}
	else
	{
		//#1373 toolKindText.SetLocalizedText("ModePref_ToolKind_User");
		mModePrefWindow.NVI_SetShowControl(kStaticText_ToolKind,false);
		mModePrefWindow.NVI_SetShowControl(kButton_Tool_CopyToUserTool,false);
	}
	//#1373 mModePrefWindow.NVI_SetControlText(kStaticText_ToolKind,toolKindText);
}

//�c�[�����e�X�V
void	AToolEditManager::Open_ToolText()
{
#if IMPLEMENTATION_FOR_MACOSX
	/*B0000 OSType	creator, type;
	mCurrentFile.GetCreatorType(creator,type);*/
	if( /*B0000 type == 'osas'*/mCurrentFile.IsAppleScriptFile() == true )
	{
		AText	text;
		GetApp().SPI_GetAppleScriptSource(mCurrentFile,text);
		mModePrefWindow.NVI_SetControlText(kText_Tool,text);
		//#427 mModePrefWindow.NVI_SetControlEnable(kText_Tool,false);
	}
	else
#endif
	{
		AText	text;
		GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_Tool,mCurrentFile,text);
		mModePrefWindow.NVI_SetControlText(kText_Tool,text);
		//#427 mModePrefWindow.NVI_SetControlEnable(kText_Tool,true);
	}
}


ABool	AToolEditManager::TryClose( const ABool inForceSave )
{
	//Open���Ă��Ȃ���΂��̂܂�return
	if( mOpened == false )   return true;
	
	//���ۑ��Ȃ�ۑ��_�C�A���O�\��
	if( mDirty == true )
	{
		if( inForceSave == false )
		{
			//���݂�ContentView�̓��e�ɁA�e�[�u���̑I���s�̂ق������킹��
			//#868 if( mCurrentIsToobar == false )
			{
				//�c�[��
				//#216 AObjectID	menuObjectID = kObjectID_Invalid;
				AIndex	arrayIndex = kIndex_Invalid;//#216
				AIndex	itemIndex = kIndex_Invalid;
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).FindToolIndexByFile(mCurrentFile,/*#216 menuObjectID*/arrayIndex,itemIndex);
				mModePrefWindow.NVI_GetListView(kListView_Tool).SPI_SetSelect(itemIndex);
				mModePrefWindow.NVI_GetListView(kListView_Tool).SPI_AdjustScroll();
			}
			/*#868
			else
			{
				//�c�[���o�[
				AIndex	itemIndex = kIndex_Invalid;
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).FindToolbarIndexByFile(mCurrentFile,itemIndex);
				mModePrefWindow.NVI_GetListView(kListView_Tool).SPI_SetSelect(itemIndex);
				mModePrefWindow.NVI_GetListView(kListView_Tool).SPI_AdjustScroll();
			}
			*/
			//�ۑ��_�C�A���O�\��
			mModePrefWindow.ShowSaveToolAlertSheet();
			return false;
		}
		else
		{
			SaveText();
		}
	}
	
	//���݂̃��[�h�A�t�@�C�����X�V
	mCurrentModeIndex = kIndex_Invalid;
	mCurrentFile.Unspecify();
	//
	mOpened = false;
	
	//#1373
	mModePrefWindow.NVI_SetShowControl(kStaticText_ToolKind,false);
	mModePrefWindow.NVI_SetShowControl(kButton_Tool_CopyToUserTool,false);
	
	//#427 ToolContentView�̃R���g���[�����O���C�A�E�g�ɂ��A��ɐݒ�
	mModePrefWindow.SPI_UpdateToolContentViewControlGrayout(false,false,false,false,false,false);
	mModePrefWindow.SPI_ClearToolContentViewControl();
	//
	return true;
}

void	AToolEditManager::SaveText()
{
	AText	text;
	mModePrefWindow.NVI_GetControlText(kText_Tool,text);
	text.ConvertLineEndToLF();
	GetApp().SPI_WriteTextFile(text,mCurrentFile);//B0310
	SetDirty(false);
}

//#8
//Tool�̓��e��Revert
void	AToolEditManager::RevertText()
{
	Open_ToolText();
	SetDirty(false);
}

void	AToolEditManager::SaveShortcut()
{
	//��Open�Ȃ�I��
	if( mOpened == false )
	{
		return;
	}
	
	//�c�[�����[�h�̏ꍇ�̂ݎ��s
	//#868 if( mCurrentIsToobar == false )
	{
		//�V���[�g�J�b�g�����擾�i���������啶���ɂ���j
		AText	shortcuttext;
		mModePrefWindow.NVI_GetControlText(kText_Tool_Shortcut,shortcuttext);
		ANumber	shortcut = 0;
		if( shortcuttext.GetItemCount() > 0 )
		{
			shortcut = shortcuttext.Get(0);
			if( shortcut >= 'a' && shortcut <= 'z' )
			{
				shortcut -= 'a'-'A';
			}
			shortcuttext.DeleteAll();
			shortcuttext.Add(shortcut);
			mModePrefWindow.NVI_SetControlText(kText_Tool_Shortcut,shortcuttext);
		}
		//modifier�L�[�擾
		ABool	controlKey = mModePrefWindow.NVI_GetControlBool(kCheck_Tool_ShortcutControlKey);
		ABool	shiftKey = mModePrefWindow.NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey);
		AMenuShortcutModifierKeys	modifiers = AKeyWrapper::MakeMenuShortcutModifierKeys(controlKey,false,shiftKey);
		//�f�[�^�ݒ�
		GetApp().SPI_GetModePrefDB(mCurrentModeIndex).SetToolShortcut(mCurrentFile,shortcut,modifiers);
	}
}

void	AToolEditManager::SaveToolName()
{
	//��Open�Ȃ�I��
	if( mOpened == false )
	{
		return;
	}
	
	//�c�[�����擾
	AText	newName;
	mModePrefWindow.NVI_GetControlText(kText_ToolName,newName);
	/*
	//#0 �t�@�C�����Ɏg���Ȃ��������X�y�[�X�ɕϊ�
	newName.ReplaceChar(kUChar_Slash,kUChar_Space);
	newName.ReplaceChar('\\',kUChar_Space);
	mModePrefWindow.NVI_SetControlText(kText_ToolName,newName);
	*/
	//
	ABool	result = false;
	AFileAcc	newfile;
	//#868 if( mCurrentIsToobar == false )
	{
		//�c�[��
		result = GetApp().SPI_GetModePrefDB(mCurrentModeIndex).SetToolName(mCurrentFile,newName,newfile);
	}
	/*#868
	else
	{
		//�c�[���o�[
		result = GetApp().SPI_GetModePrefDB(mCurrentModeIndex).SetToolbarName(mCurrentFile,newName,newfile);
	}
	*/
	if( result == true )
	{
		//���̕ύX�����Ȃ�mCurrentFile���X�V
		mCurrentFile.CopyFrom(newfile);
	}
	else
	{
		//���̕ύX���s�Ȃ�ContentView�̓��e�����̃t�@�C�����ɖ߂�
		AText	oldname;
		mCurrentFile.GetFilename(oldname);
		mModePrefWindow.NVI_SetControlText(kText_ToolName,oldname);
		return;
	}
	//
	mModePrefWindow.NVI_UpdateProperty();
}

void	AToolEditManager::SetDirty( const ABool inDirty )
{
	mDirty = inDirty;
	//mModePrefWindow.NVM_UpdateControlStatus();
	mModePrefWindow.NVI_SetModified(mDirty);
	mModePrefWindow.NVI_GetListView(kListView_Tool).SPI_SetEnableDragDrop(mDirty==false);
}

//#868
/**
���݂̃c�[���t�@�C����mi�܂��͑��A�v���ŊJ��
*/
void	AToolEditManager::OpenToolWithApp()
{
	//�t�@�C�����J��
	GetApp().GetAppPref().LaunchAppWithFile(mCurrentFile,0);
	//�����X�V�c�[���̏ꍇ�͓ǂݍ��ݐ�p�ɂ���
	ABool	autoupdate = GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetToolAutoUpdateFlag(mCurrentToolMenuObjectID,mCurrentToolIndex);
	ADocumentID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,mCurrentFile);
	if( docID != kObjectID_Invalid && autoupdate == true )
	{
		GetApp().SPI_GetTextDocumentByID(docID).NVI_SetReadOnly(true);
	}
}


