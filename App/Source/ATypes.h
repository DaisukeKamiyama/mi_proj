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

ATypes.h
アプリケーション個別の定義

*/

#pragma once

#include "../../AbsFramework/AbsBase/ABase.h"
#include "../../AbsFramework/AbsFrame/AbsFrame.h"

/*AMacLegacyPref.hへ移動struct FolderSpec
{
	short	vRefNum;
	long	dirID;
};
*/
typedef AReturnCode EReturnCode;

enum AKeyFilterType
{
	A_KeyFilter_Number = 0
};

enum AMenuID
{
	kMenuID_New =1,
	kMenuID_RecentlyOpenedFile,
	kMenuID_ContextMenu_TextNotSelected,
	kMenuID_ContextMenu_TextSelected,
	kMenuID_ContextMenu_IndexWindow,
	kMenuID_ContextMenu_InfoTag,
	kMenuID_ContextMenu_Transliterate
};

enum AWrapMode
{
	kWrapMode_NoWrap = 0,
	kWrapMode_WordWrap,
	kWrapMode_WordWrapByLetterCount,
	//#1113
	kWrapMode_LetterWrap,
	kWrapMode_LetterWrapByLetterCount,
};

enum AAntiAliasMode
{
	kAntiAliasMode_Default =0,
	kAntiAliasMode_On,
	kAntiAliasMode_Off
};

enum ADiffType
{
	kDiffType_None = 0,
	kDiffType_Added,
	kDiffType_Deleted,
	kDiffType_Changed,
	kDiffType_OutOfDiffRange,
};


#if IMPLEMENTATION_FOR_MACOSX

const AMenuItemID	kMenuItemID_AppPref 					= 1101;//'pref';
const AMenuItemID	kMenuItemID_Commit 						= 'CVSc';
const AMenuItemID	kMenuItemID_Add 						= 'CVSa';
const AMenuItemID	kMenuItemID_CorrectEncoding 			= 1104;//'CEnc';
const AMenuItemID	kMenuItemID_FindEncodingProblem 		= 1105;//'FiEP';
const AMenuItemID	kMenuItemID_FindMisspell				= 1106;//'FiMS';//R0199
const AMenuItemID	kMenuItemID_MoveLine 					= 1107;//'MvLn';
const AMenuItemID	kMenuItemID_ShowJumpList 				= 1108;//'SJpL';
const AMenuItemID	kMenuItemID_ShowFileList 				= 1109;//'SFiL';
const AMenuItemID	kMenuItemID_ShowFusenList 				= 1110;//'SFuL';//#138
const AMenuItemID	kMenuItemID_FontSizeOther 				= 'FSOt';
const AMenuItemID	kMenuItemID_WrapMode_NoWrap 			= 1112;//'WrNo';
const AMenuItemID	kMenuItemID_WrapMode_WordWrap 			= 1113;//'WrWo';
const AMenuItemID	kMenuItemID_WrapMode_WordWrapByLetterCount = 1114;//'WrLC';
const AMenuItemID	kMenuItemID_WrapMode_RootMenu			= 1115;//'_WrN';
const AMenuItemID	kMenuItemID_ShowDocPropertyWindow 		= 1116;//'DocP';
const AMenuItemID	kMenuItemID_About 						= 1117;//'abou';
const AMenuItemID	kMenuItemID_ModePref 					= 1118;//'ModP';
const AMenuItemID	kMenuItemID_IconManager 				= 'TBIM';
const AMenuItemID	kMenuItemID_MutiFileFind 				= 1120;//'MMFi';
const AMenuItemID	kMenuItemID_FTPFolderSetup 				= 1121;//'FtpS';
const AMenuItemID	kMenuItemID_FTPFolder 					= 1122;//'FtpF';
const AMenuItemID	kMenuItemID_NewFTPFile 					= 'FtpN';
const AMenuItemID	kMenuItemID_FolderList 					= 1124;//'FolL';
const AMenuItemID	kMenuItemID_FolderListSetup 			= 1125;//'FoLS';
//const AMenuItemID	kMenuItemID_FolderListRootItem = 'FoLR';
const AMenuItemID	kMenuItemID_FTPLog 						= 1126;//'FtLg';
const AMenuItemID	kMenuItemID_TraceLog 					= 'TrLg';
const AMenuItemID	kMenuItemID_Tool_StandardMode 			= 1128;//'TooS';
const AMenuItemID	kMenuItemID_Tool 						= 1129;//'Tool';
const AMenuItemID	kMenuItemID_SameFolder 					= 1130;//'SmFo';
const AMenuItemID	kMenuItemID_SameProject 				= 1131;//'SmPj';
const AMenuItemID	kMenuItemID_RefreshProjectFolders		= 1132;//'RPrj';//#331
const AMenuItemID	kMenuItemID_NewTextDocument 			= 1133;//'NewT';
const AMenuItemID	kMenuItemID_NewIndexWindow 				= 'NewI';
const AMenuItemID	kMenuItemID_NewWithTemplate 			= 1135;//'NwTm';
//const AMenuItemID	kMenuItemID_New = 'New ';
//ABaseTypes.hに移動const AMenuItemID	kMenuItemID_InputBackslashYen = 'BYen';
const AMenuItemID	kMenuItemID_WindowMenu 					= 1136;//'WnMn';
const AMenuItemID	kMenuItemID_SwitchWindow 				= 1137;//'SwWi';
const AMenuItemID	kMenuItemID_SwitchWindowPrev 			= 1138;//'SwWP';//#552
const AMenuItemID	kMenuItemID_TabWindowMode 				= 1139;//'TabW';
const AMenuItemID	kMenuItemID_AllWindowToTab 				= 1140;//'AllT';
const AMenuItemID	kMenuItemID_WindowToTab					= 1141;//'WinT';
const AMenuItemID	kMenuItemID_TabToWindow 				= 1142;//'TWin';
const AMenuItemID	kMenuItemID_IndexPrevious 				= 1143;//'IdxP';
const AMenuItemID	kMenuItemID_IndexNext 					= 1144;//'IdxN';
const AMenuItemID	kMenuItemID_TextCount 					= 1145;//'Coun';
const AMenuItemID	kMenuItemID_MoveToDefinition 			= 1146;//'MvDf';
const AMenuItemID	kMenuItemID_MoveToDefinitionRootMenu	= 1147;//'_MvD';
const AMenuItemID	kMenuItemID_ReRecognize 				= 1148;//'ReRe';
const AMenuItemID	kMenuItemID_SetProjectFolder 			= 1149;//'SPrj';
const AMenuItemID	kMenuItemID_CorrectSpell				= 'CSpl';//R0199
const AMenuItemID	kMenuItemID_WrapMode_LetterWrap 		= 1150;//#1113
const AMenuItemID	kMenuItemID_WrapMode_LetterWrapByLetterCount = 1151;//#1113

const AMenuItemID	kMenuItemID_OpenModePrefFolder 			= 'OpMF';
const AMenuItemID	kMenuItemID_Open 						= 1202;//'open';
//ABaseTypes.hに移動const AMenuItemID	kMenuItemID_Close = 'clos';
const AMenuItemID	kMenuItemID_OpenInvisible 				= 1203;//'OpIv';
const AMenuItemID	kMenuItemID_OpenSelected 				= 1204;//'OpSl';
const AMenuItemID	kMenuItemID_Open_RecentlyOpenedFile 	= 1205;//'ORct';
const AMenuItemID	kMenuItemID_OpenImportFile 				= 1206;//'OImp';
const AMenuItemID	kMenuItemID_Delete_RecentlyOpenedFile 	= 1207;//'DRct';
const AMenuItemID	kMenuItemID_Delete_NotFoundRecentlyOpenedFile= 1208;//'DNRc';//#480
//#182 ABaseTypes.hに移動　const AMenuItemID	kMenuItemID_Quit 						= 'quit';
const AMenuItemID	kMenuItemID_Font 						= 'font';
const AMenuItemID	kMenuItemID_FontRootMenu				= '_fon';
const AMenuItemID	kMenuItemID_FontSize 					= 'FSiz';
const AMenuItemID	kMenuItemID_Jump 						= 1212;//'Jump';
const AMenuItemID	kMenuItemID_JumpExportToText			= 1213;//'ExJm';
const AMenuItemID	kMenuItemID_JumpNextItem				= 1214;//'NxJm';
const AMenuItemID	kMenuItemID_JumpPreviousItem			= 1215;//'PrJm';
const AMenuItemID	kMenuItemID_SetMode 					= 1216;//'SMod';
const AMenuItemID	kMenuItemID_SetModeRootMenu				= 1217;//'_SMo';
const AMenuItemID	kMenuItemID_SetTextEncoding 			= 1218;//'STEC';
const AMenuItemID	kMenuItemID_SetTextEncodingRootMenu		= 1219;//'_STE';
const AMenuItemID	kMenuItemID_Transliterate_ToLower 		= 1220;//'TlLw';
const AMenuItemID	kMenuItemID_Transliterate_ToUpper 		= 1221;//'TlUp';
const AMenuItemID	kMenuItemID_Transliterate 				= 1222;//'TrsL';
const AMenuItemID	kMenuItemID_TransliterateRootMenu		= 1223;//'_Trs';
const AMenuItemID	kMenuItemID_Minimize 					= 1224;//'mini';
const AMenuItemID	kMenuItemID_ReturnCode_CR 				= 1225;//'RtCR';
const AMenuItemID	kMenuItemID_ReturnCode_CRLF 			= 1226;//'RtCL';
const AMenuItemID	kMenuItemID_ReturnCode_LF 				= 1227;//'RtLF';
const AMenuItemID	kMenuItemID_ReturnCode_RootMenu			= 1228;//'_RtC';
const AMenuItemID	kMenuItemID_ShowHideCrossCaret 			= 1229;//'CrCa';
const AMenuItemID	kMenuItemID_SplitView					= 1230;//'SpVw';
const AMenuItemID	kMenuItemID_ConcatView					= 1231;//'CtVw';
const AMenuItemID	kMenuItemID_DisplaySubPane				= 1232;//'SubP';//#212
const AMenuItemID	kMenuItemID_DisplayInfoPane				= 1233;//'InfP';//#291
const AMenuItemID	kMenuItemID_SetupPane					= 1234;//'StPa';//#566
const AMenuItemID	kMenuItemID_CopyFilePath				= 1235;//'CpPt';//#564
const AMenuItemID	kMenuItemID_OpenLuaConsole				= 1236;//'OpLu';//#567
const AMenuItemID	kMenuItemID_SaveTabSet					= 1237;//#1050
//テキスト編集
const AMenuItemID	kMenuItemID_Save 						= 2001;//'Save';
const AMenuItemID	kMenuItemID_SaveAs						= 2002;//'SvAs';
const AMenuItemID	kMenuItemID_AdditionalCopy				= 2003;//'CopA';
const AMenuItemID	kMenuItemID_Indent						= 2004;//'Inde';
const AMenuItemID	kMenuItemID_ShiftLeft					= 2005;//'ShLe';
const AMenuItemID	kMenuItemID_ShiftRight					= 2006;//'ShRi';
const AMenuItemID	kMenuItemID_Bookmark					= 2007;//'BkMk';
const AMenuItemID	kMenuItemID_MoveToBookmark				= 2008;//'MvBM';
const AMenuItemID	kMenuItemID_CorrespondText				= 2009;//'Corr';
const AMenuItemID	kMenuItemID_SelectBetweenCorrespondText	= 2010;//'CorS';//#359
const AMenuItemID	kMenuItemID_KeyToolMode					= 2011;//'KyTl';
const AMenuItemID	kMenuItemID_KukeiSelectionMode			= 2012;//'Kuke';
const AMenuItemID	kMenuItemID_KukeiPaste					= 2013;//'KuPa';
const AMenuItemID	kMenuItemID_SaveFTPAlias				= 2014;//'SvFA';
const AMenuItemID	kMenuItemID_InsertLineEndAtWrap			= 2015;//'ILEW';
//検索
const AMenuItemID	kMenuItemID_Find						= 2101;//'find';
const AMenuItemID	kMenuItemID_FindNext					= 2102;//'FdNx';
const AMenuItemID	kMenuItemID_FindPrevious				= 2103;//'FdPr';
const AMenuItemID	kMenuItemID_FindFromFirst				= 2104;//'Fd1s';
const AMenuItemID	kMenuItemID_FindList					= 2015;//'FdLt';
const AMenuItemID	kMenuItemID_ReplaceSelectedTextOnly		= 2106;//'Rplc';
const AMenuItemID	kMenuItemID_ReplaceAndFind				= 2107;//'RpFn';
const AMenuItemID	kMenuItemID_ReplaceNext					= 2108;//'RpNx';
const AMenuItemID	kMenuItemID_ReplaceAfterCaret			= 2109;//'RpAC';
const AMenuItemID	kMenuItemID_ReplaceInSelection			= 2110;//'RpIS';
const AMenuItemID	kMenuItemID_ReplaceAll					= 2112;//'RpAl';
const AMenuItemID	kMenuItemID_SetFindText					= 2113;//'StFd';
const AMenuItemID	kMenuItemID_SetReplaceText				= 2114;//'StRp';
const AMenuItemID	kMenuItemID_EraseFindHighlight			= 2115;//'ErFH';
const AMenuItemID	kMenuItemID_FindExtractMatchedText		= 2116;
//印刷
const AMenuItemID	kMenuItemID_PageSetup					= 2201;//'PSet';
const AMenuItemID	kMenuItemID_Print						= 2202;//'Prnt';
const AMenuItemID	kMenuItemID_ShowPrintOptionWindow		= 2203;//'PrOp';
const AMenuItemID	kMenuItemID_PrintRange					= 2204;//'PrtR';//#524
//SCM R0006
const AMenuItemID	kMenuItemID_SCMCompareWithLatest		= 2301;//'SCMC';
const AMenuItemID	kMenuItemID_SCMNextDiff					= 2302;//'SCMN';
const AMenuItemID	kMenuItemID_SCMPrevDiff					= 2303;//'SCMP';
const AMenuItemID	kMenuItemID_SCMUpdateStatusAll			= 2304;//'SCMU';
//RC1
const AMenuItemID	kMenuItemID_ArrowToDef					= 'ArrD';
//RC2
const AMenuItemID	kMenuItemID_ShowIdInfo					= 'SIdI';
//#146
const AMenuItemID	kMenuItemID_NavigatePrev				= 2601;//'NavP';
const AMenuItemID	kMenuItemID_NavigateNext				= 2602;//'NavN';
//#232
const AMenuItemID	kMenuItemID_ToolBar_Folder				= 2701;//'TBFo';
const AMenuItemID	kMenuItemID_ToolBar_EditButton			= 2702;//'EdTB';
const AMenuItemID	kMenuItemID_ToolBar_InvalidButton		= 2703;//'IvTB';
const AMenuItemID	kMenuItemID_ToolBar_DisplayAllButton	= 2704;//'DsTB';
const AMenuItemID	kMenuItemID_ToolBar_DeleteButton		= 2705;//'DlTB';
const AMenuItemID	kMenuItemID_ToolBar_UpdateDisplay		= 'UdTB';
const AMenuItemID	kMenuItemID_ToolBar_ChangeTitle			= 2706;//#1344
//#238
const AMenuItemID	kMenuItemID_IdInfoHistory				= 'IIHs';
//#212
const AMenuItemID	kMenuItemID_SubTextPaneMenu				= 'STPM';
//#357
const AMenuItemID	kMenuItemID_SwitchToNextTab				= 3001;//'SwNT';
const AMenuItemID	kMenuItemID_SwitchToPrevTab				= 3002;//'SwPT';
//#390
const AMenuItemID	kMenuItemID_KeyRecordWindow				= 3101;//'KyRc';
const AMenuItemID	kMenuItemID_KeyRecordStart				= 3102;//'KyRR';
const AMenuItemID	kMenuItemID_KeyRecordStop				= 3103;//'KyRS';
const AMenuItemID	kMenuItemID_KeyRecordPlay				= 3104;//'KyRP';
const AMenuItemID	kMenuItemID_KeyRecordRecentlyPlay		= 'KRRP';
//#539
const AMenuItemID	kMenuItemID_Help						= 3201;//'help';
const AMenuItemID	kMenuItemID_ReadMeFirst					= 3202;//#1351
//#404
const AMenuItemID	kMenuItemID_FullScreenMode				= 3301;//'SwFS';
//#455
const AMenuItemID	kMenuItemID_SCMCommit					= 3401;//'SCCm';
//#454
const AMenuItemID	kMenuItemID_JumpListHistory				= 'JmLH';
//#478
const AMenuItemID	kMenuItemID_FigMode_None				= 'FigN';
const AMenuItemID	kMenuItemID_FigMode_Line				= 'FigL';
//#379
/*#899
const AMenuItemID	kMenuItemID_SubPaneMode_Manual			= 3701;//'SPMn';
const AMenuItemID	kMenuItemID_SubPaneMode_PrevTab			= 3702;//'SPPT';
const AMenuItemID	kMenuItemID_SubPaneMode_SameTab			= 3703;//'SPST';
const AMenuItemID	kMenuItemID_CompareFiles				= 'SPCF';
*/
//#467
const AMenuItemID	kMenuItemID_EnableDefines				= 3801;//'EDef';
//#606
const AMenuItemID	kMenuItemID_SwitchHandToolMode			= 3901;//'SHTM';
//#619
const AMenuItemID	kMenuItemID_HideAllModeCreatorToolbarButton = 4001;
const AMenuItemID	kMenuItemID_HideAllUserToolbarButton		= 4002;
const AMenuItemID	kMenuItemID_AddToolbarButton				= 4003;
//#638
const AMenuItemID	kMenuItemID_SwitchAutoIndentMode		 	= 4101;
//#450
const AMenuItemID	kMenuItemID_Folding_Collapse						= 4201;
const AMenuItemID	kMenuItemID_Folding_Expand							= 4202;
const AMenuItemID	kMenuItemID_Folding_CollapseAllAtCurrentLevel		= 4203;
const AMenuItemID	kMenuItemID_Folding_ExpandAllAtCurrentLevel			= 4204;
const AMenuItemID	kMenuItemID_Folding_CollapseAllHeaders				= 4205;
const AMenuItemID	kMenuItemID_Folding_CollapseAllHeadersWithNoChange	= 4206;
const AMenuItemID	kMenuItemID_Folding_ExpandAll						= 4207;
const AMenuItemID	kMenuItemID_Folding_GoToParent						= 4208;
const AMenuItemID	kMenuItemID_Folding_GoToNext						= 4209;
const AMenuItemID	kMenuItemID_Folding_GoToPrev						= 4210;
const AMenuItemID	kMenuItemID_Folding_GoToNextAtSameLevel				= 4211;
const AMenuItemID	kMenuItemID_Folding_GoToPrevAtSameLevel				= 4212;
const AMenuItemID	kMenuItemID_Folding_SelectFoldingRange				= 4213;
const AMenuItemID	kMenuItemID_Folding_SetToDefault					= 4214;
//
const AMenuItemID	kMenuItemID_DisplaySubTextPane				= 4301;
//#868
const AMenuItemID	kMenuItemID_UseModeFont						= 4401;
const AMenuItemID	kMenuItemID_UseDocumentFont					= 4402;
//#737
const AMenuItemID	kMenuItemID_CompareLeftRightSelected		= 4501;
const AMenuItemID	kMenuItemID_CompareSelectionAndClipboard	= 4502;
//#878
const AMenuItemID	kMenuItemID_SwitchFIFOClipboardMode			= 4601;
//
const AMenuItemID	kMenuItemID_CompareWithAutoBackup_Monthly	= 4701;
const AMenuItemID	kMenuItemID_CompareWithAutoBackup_Daily		= 4702;
const AMenuItemID	kMenuItemID_CompareWithAutoBackup_Hourly	= 4703;
const AMenuItemID	kMenuItemID_CompareWithAutoBackup_Minutely	= 4704;
const AMenuItemID	kMenuItemID_CompareWithDailyBackupByDefault	= 4705;
const AMenuItemID	kMenuItemID_SetupAutoBackup					= 4706;
const AMenuItemID	kMenuItemID_CompareWithAutoBackup_Title		= 4707;
const AMenuItemID	kMenuItemID_NoCompare						= 4708;
const AMenuItemID	kMenuItemID_CompareWithFile					= 4709;
const AMenuItemID	kMenuItemID_CompareWithAutoBackup			= 4710;
//#798
const AMenuItemID	kMenuItemID_SearchInHeaderList				= 4801;
const AMenuItemID	kMenuItemID_SearchInProjectFolder			= 4802;
const AMenuItemID	kMenuItemID_SearchInSameFolder				= 4803;
const AMenuItemID	kMenuItemID_SearchInRecentlyOpenFiles		= 4804;
const AMenuItemID	kMenuItemID_SearchInKeywordList				= 4805;
//#850
const AMenuItemID	kMenuItemID_OpenDocumentUsingTab			= 4901;
const AMenuItemID	kMenuItemID_CollectAllWindowsToTabs			= 4902;
//#725
const AMenuItemID	kMenuItemID_AddRecentlyOpenFileListInRightSideBar	= 5001;
const AMenuItemID	kMenuItemID_AddSameFolderFileListInRightSideBar		= 5002;
const AMenuItemID	kMenuItemID_AddSameProjectFileListInRightSideBar	= 5003;
const AMenuItemID	kMenuItemID_AddRemoteFileListInRightSideBar			= 5004;
const AMenuItemID	kMenuItemID_AddJumpListInRightSideBar				= 5005;
const AMenuItemID	kMenuItemID_AddDocInfoInRightSideBar				= 5006;
const AMenuItemID	kMenuItemID_AddTextMarkerInRightSideBar				= 5007;
const AMenuItemID	kMenuItemID_AddCandidateListInRightSideBar			= 5008;
const AMenuItemID	kMenuItemID_AddToolListInRightSideBar				= 5009;
const AMenuItemID	kMenuItemID_AddIdInfoListInRightSideBar				= 5010;
const AMenuItemID	kMenuItemID_AddCallGrefInRightSideBar				= 5011;
const AMenuItemID	kMenuItemID_AddPreviewInRightSideBar				= 5012;
const AMenuItemID	kMenuItemID_AddWordsListInRightSideBar				= 5013;
const AMenuItemID	kMenuItemID_AddKeyRecordInRightSideBar				= 5014;
//#725
const AMenuItemID	kMenuItemID_AddRecentlyOpenFileListInLeftSideBar	= 5051;
const AMenuItemID	kMenuItemID_AddSameFolderFileListInLeftSideBar		= 5052;
const AMenuItemID	kMenuItemID_AddSameProjectFileListInLeftSideBar		= 5053;
const AMenuItemID	kMenuItemID_AddRemoteFileListInLeftSideBar			= 5054;
const AMenuItemID	kMenuItemID_AddJumpListInLeftSideBar				= 5055;
const AMenuItemID	kMenuItemID_AddDocInfoInLeftSideBar					= 5056;
const AMenuItemID	kMenuItemID_AddTextMarkerInLeftSideBar				= 5057;
const AMenuItemID	kMenuItemID_AddCandidateListInLeftSideBar			= 5058;
const AMenuItemID	kMenuItemID_AddToolListInLeftSideBar				= 5059;
const AMenuItemID	kMenuItemID_AddIdInfoListInLeftSideBar				= 5060;
const AMenuItemID	kMenuItemID_AddCallGrefInLeftSideBar				= 5061;
const AMenuItemID	kMenuItemID_AddPreviewInLeftSideBar					= 5062;
const AMenuItemID	kMenuItemID_AddWordsListInLeftSideBar				= 5063;
const AMenuItemID	kMenuItemID_AddKeyRecordInLeftSideBar				= 5064;
//#750
const AMenuItemID	kMenuItemID_TextMarkerSelectGroup			= 5101;
//
const AMenuItemID	kMenuItemID_JumpList_CollapseAllForLevel	= 5201;
const AMenuItemID	kMenuItemID_JumpList_ExpandAll				= 5202;
//
const AMenuItemID	kMenuItemID_FileList_ProjectModeFileter_AllFiles	= 5301;
const AMenuItemID	kMenuItemID_FileList_ProjectModeFileter_SameMode	= 5302;
const AMenuItemID	kMenuItemID_FileList_ProjectModeFileter_DiffFiles	= 5303;
//
const AMenuItemID	kMenuItemID_Percent									= 5401;
//#725
const AMenuItemID	kMenuItemID_AddRecentlyOpenFileListInFloating		= 5601;
const AMenuItemID	kMenuItemID_AddSameFolderFileListInFloating			= 5602;
const AMenuItemID	kMenuItemID_AddSameProjectFileListInFloating		= 5603;
const AMenuItemID	kMenuItemID_AddRemoteFileListInFloating				= 5604;
const AMenuItemID	kMenuItemID_AddJumpListInFloating					= 5605;
const AMenuItemID	kMenuItemID_AddDocInfoInFloating					= 5606;
const AMenuItemID	kMenuItemID_AddTextMarkerInFloating					= 5607;
const AMenuItemID	kMenuItemID_AddCandidateListInFloating				= 5608;
const AMenuItemID	kMenuItemID_AddToolListInFloating					= 5609;
const AMenuItemID	kMenuItemID_AddIdInfoListInFloating					= 5610;
const AMenuItemID	kMenuItemID_AddCallGrefInFloating					= 5611;
const AMenuItemID	kMenuItemID_AddPreviewInFloating					= 5612;
const AMenuItemID	kMenuItemID_AddWordsListInFloating					= 5613;
const AMenuItemID	kMenuItemID_AddKeyRecordInFloating					= 5614;
//#688
const AMenuItemID	kMenuItemID_ShowHideToolbar							= 5701;
const AMenuItemID	kMenuItemID_CustomizeToolbar						= 5702;
//#65
const AMenuItemID	kMenuItemID_MultiFileReplace						= 5801;
//#717
const AMenuItemID	kMenuItemID_CandidateSearchMode_Normal				= 5901;
const AMenuItemID	kMenuItemID_CandidateSearchMode_Partial				= 5902;
//#844
const AMenuItemID	kMenuItemID_AddNewMode								= 6001;
//
const AMenuItemID	kMenuItemID_FixRecentlyOpenedItem					= 6101;
//#793
const AMenuItemID	kMenuItemID_CompareLeftFileAndRightFile				= 6201;
//
const AMenuItemID	kMenuItemID_BringAllToFront							= 6301;
const AMenuItemID	kMenuItemID_Zoom									= 6302;
//
const AMenuItemID	kMenuItemID_CurrentModePref		 					= 6401;
//
const AMenuItemID	kMenuItemID_CloseAllTabWithoutFlag					= 6501;
const AMenuItemID	kMenuItemID_SetDocumentFlag							= 6502;
//
const AMenuItemID	kMenuItemID_ClearAllLock							= 6601;
//
const AMenuItemID	kMenuItemID_FindDialog								= 6701;
//
const AMenuItemID	kMenuItemID_PasteAtEndOfEachLine					= 6801;
//
const AMenuItemID	kMenuItemID_HideMacrosAfterThis						= 6901;
const AMenuItemID	kMenuItemID_CloseTabsAfterThis						= 6902;
const AMenuItemID	kMenuItemID_DeleteMacrosAfterThis					= 6903;
//
const AMenuItemID	kMenuItemID_RevealMacroWithFinder					= 7001;
const AMenuItemID	kMenuItemID_ReconfigMacro							= 7002;
//
const AMenuItemID	kMenuItemID_Compare									= 7101;
//
const AMenuItemID	kMenuItemID_ShowHideAllFloatingWindow				= 7201;
const AMenuItemID	kMenuItemID_InhibitPopup							= 7202;
const AMenuItemID	kMenuItemID_AddKeyRecordInFloating_AndStart			= 7203;
//フォントサイズ一時変更 #966
const AMenuItemID	kMenuItemID_FontSize60								= 7301;
const AMenuItemID	kMenuItemID_FontSize80								= 7302;
const AMenuItemID	kMenuItemID_FontSize90								= 7303;
const AMenuItemID	kMenuItemID_FontSize95								= 7304;
const AMenuItemID	kMenuItemID_FontSize100								= 7305;
const AMenuItemID	kMenuItemID_FontSize105								= 7306;
const AMenuItemID	kMenuItemID_FontSize110								= 7307;
const AMenuItemID	kMenuItemID_FontSize120								= 7308;
const AMenuItemID	kMenuItemID_FontSize140								= 7309;
const AMenuItemID	kMenuItemID_FontSize160								= 7310;
const AMenuItemID	kMenuItemID_FontSize180								= 7311;
const AMenuItemID	kMenuItemID_FontSize240								= 7312;
const AMenuItemID	kMenuItemID_RevertFontSize							= 7399;
//
const AMenuItemID	kMenuItemID_SortTabs								= 7400;//#1062
//Speech #851
const AMenuItemID	kMenuItemID_StartSpeaking							= 7401;
const AMenuItemID	kMenuItemID_StopSpeaking							= 7402;

const AMenuItemID	kMenuItemID_CM_MultiFileFind			= 8001;//'C_MF';
const AMenuItemID	kMenuItemID_CM_FindNext					= 8002;//'C_FN';
const AMenuItemID	kMenuItemID_CM_FindPrevious				= 8003;//'C_FP';
const AMenuItemID	kMenuItemID_CM_Find						= 8004;//'C_FD';
const AMenuItemID	kMenuItemID_CM_SelectAll				= 8005;//'C_SA';
const AMenuItemID	kMenuItemID_CM_Paste					= 8006;//'C_Pa';
const AMenuItemID	kMenuItemID_CM_Copy						= 8007;//'C_Co';
const AMenuItemID	kMenuItemID_CM_Cut						= 8008;//'C_Cu';
const AMenuItemID	kMenuItemID_CM_Clear					= 8009;//'C_Cl';
const AMenuItemID	kMenuItemID_CM_DetachTab_Left			= 8010;//'C_Dl';
const AMenuItemID	kMenuItemID_CM_DetachTab_Right			= 8011;//'C_Dr';
const AMenuItemID	kMenuItemID_CM_DetachTab_Bottom			= 'C_Db';
const AMenuItemID	kMenuItemID_CM_CloseTab					= 8013;//'C_CT';
const AMenuItemID	kMenuItemID_CM_DisplayInSubTextPane		= 8014;//'C_Sb';//#333
const AMenuItemID	kMenuItemID_CM_ClearItemFromRecentOpen	= 8015;//'C_C1';//R0186
const AMenuItemID	kMenuItemID_CM_ClearNotFoundItemsFromRecentOpen	= 8016;//'C_C2';//R0186
const AMenuItemID	kMenuItemID_CM_ClearAllItemsFromRecentOpen= 8017;//'C_C3';//#480
const AMenuItemID	kMenuItemID_CM_Dictionary				= 8018;//'C_Di';//R0231
const AMenuItemID	kMenuItemID_CM_IdInfo					= 8019;//'C_Id';//RC2
const AMenuItemID	kMenuItemID_CM_ExternalComment			= 8020;//'C_EC';//RC3
const AMenuItemID	kMenuItemID_CM_IndexView				= 8021;//'IdEx';//#465
const AMenuItemID	kMenuItemID_CM_PrintRange				= 8022;//'C_PS';//#524
const AMenuItemID	kMenuItemID_CM_OpenDefineValueWindow	= 8023;//'C_DA';//#467

const AMenuItemID kWindowCommand_OK = 'bok ';
const AMenuItemID kWindowCommand_Cancel = 'bcan';
const AMenuItemID kWindowCommand_ControlValueChanged = 'chgd';
const AMenuItemID kWindowCommand_ControlValueChangedA = 'chgA';

//デバッグ
const AMenuItemID	kMenuItemID_HashStatics					= 'Hash';//#271
const AMenuItemID	kMenuItemID_DumpObjectID				= 'obID';
const AMenuItemID	kMenuItemID_UnitTest					= 'UTST';
const AMenuItemID	kMenuItemID_UnitTest2					= 9901;
const AMenuItemID	kMenuItemID_SDFDebugMode				= 'SDFD';
//#350 const AMenuItemID	kMenuItemID_ModePrefXML					= 'MXML';
const AMenuItemID	kMenuItemID_RegExpTest					= 'RTST';
const AMenuItemID	kMenuItemID_LowMemoryTest				= 'LMTS';
const AMenuItemID	kMenuItemID_CompareFolder				= 'CmpF';

#endif

#if IMPLEMENTATION_FOR_WINDOWS
const AMenuItemID	kMenuItemID_AppPref 					= IDM_AppPref;
const AMenuItemID	kMenuItemID_Commit 						= 'CVSc';
const AMenuItemID	kMenuItemID_Add 						= 'CVSa';
const AMenuItemID	kMenuItemID_CorrectEncoding 			= IDM_CorrectEncoding;
const AMenuItemID	kMenuItemID_FindEncodingProblem 		= IDM_FindEncodingProblem;
const AMenuItemID	kMenuItemID_FindMisspell				= IDM_FindMisspell;
const AMenuItemID	kMenuItemID_MoveLine 					= IDM_MoveLine;
const AMenuItemID	kMenuItemID_ShowJumpList 				= IDM_ShowJumpList;
const AMenuItemID	kMenuItemID_ShowFileList 				= IDM_ShowFileList;
const AMenuItemID	kMenuItemID_ShowFusenList 				= 'SFuL';
const AMenuItemID	kMenuItemID_FontSizeOther 				= IDM_FontSizeOther;
const AMenuItemID	kMenuItemID_WrapMode_NoWrap 			= IDM_WrapMode_NoWrap;
const AMenuItemID	kMenuItemID_WrapMode_WordWrap 			= IDM_WrapMode_WordWrap;
const AMenuItemID	kMenuItemID_WrapMode_WordWrapByLetterCount = IDM_WrapMode_WordWrapByLetterCount;
const AMenuItemID	kMenuItemID_WrapMode_RootMenu			= '_WrN';
const AMenuItemID	kMenuItemID_ShowDocPropertyWindow 		= IDM_ShowDocPropertyWindow;
const AMenuItemID	kMenuItemID_About 						= IDM_About;
const AMenuItemID	kMenuItemID_ModePref 					= IDM_ModePref;
const AMenuItemID	kMenuItemID_IconManager 				= 'TBIM';
const AMenuItemID	kMenuItemID_MutiFileFind 				= IDM_MultiFileFind;
const AMenuItemID	kMenuItemID_FTPFolderSetup 				= IDM_FTPFolderSetup;
const AMenuItemID	kMenuItemID_FTPFolder 					= IDM_FTPFolder;
const AMenuItemID	kMenuItemID_NewFTPFile 					= 'FtpN';
const AMenuItemID	kMenuItemID_FolderList 					= IDM_FolderList;
const AMenuItemID	kMenuItemID_FolderListSetup 			= IDM_FolderListSetup;
//const AMenuItemID	kMenuItemID_FolderListRootItem = 'FoLR';
const AMenuItemID	kMenuItemID_FTPLog 						= IDM_FTPLog;
const AMenuItemID	kMenuItemID_TraceLog 					= IDM_TraceLog;
const AMenuItemID	kMenuItemID_Tool_StandardMode 			= IDM_Tool_StandardMode;
const AMenuItemID	kMenuItemID_Tool 						= IDM_Tool;
const AMenuItemID	kMenuItemID_SameFolder 					= IDM_SameFolder;
const AMenuItemID	kMenuItemID_SameProject 				= IDM_SameProject;
const AMenuItemID	kMenuItemID_RefreshProjectFolders		= IDM_RefreshProjectFolders;//#331
const AMenuItemID	kMenuItemID_NewTextDocument 			= IDM_NewTextDocument;
const AMenuItemID	kMenuItemID_NewIndexWindow 				= 'NewI';
const AMenuItemID	kMenuItemID_NewWithTemplate 			= IDM_NewWithTemplate;
//const AMenuItemID	kMenuItemID_New = 'New ';
//ABaseTypes.hに移動const AMenuItemID	kMenuItemID_InputBackslashYen = 'BYen';
const AMenuItemID	kMenuItemID_WindowMenu 					= IDM_WindowMenu;
const AMenuItemID	kMenuItemID_SwitchWindow 				= IDM_SwitchWindow;
const AMenuItemID	kMenuItemID_SwitchWindowPrev 			= IDM_SwitchPrevWindow;//#552
const AMenuItemID	kMenuItemID_TabWindowMode 				= IDM_TabWindowMode;
const AMenuItemID	kMenuItemID_AllWindowToTab 				= IDM_AllWindowToTab;
const AMenuItemID	kMenuItemID_WindowToTab					= IDM_WindowToTab;
const AMenuItemID	kMenuItemID_TabToWindow 				= IDM_TabToWindow;
const AMenuItemID	kMenuItemID_IndexPrevious 				= IDM_IndexPrevious;
const AMenuItemID	kMenuItemID_IndexNext 					= IDM_IndexNext;
const AMenuItemID	kMenuItemID_TextCount 					= IDM_TextCount;
const AMenuItemID	kMenuItemID_MoveToDefinition 			= IDM_MoveToDefinition;
const AMenuItemID	kMenuItemID_MoveToDefinitionRootMenu	= '_MvD';
const AMenuItemID	kMenuItemID_ReRecognize 				= IDM_ReRecognize;
const AMenuItemID	kMenuItemID_SetProjectFolder 			= IDM_SetProjectFolder;
const AMenuItemID	kMenuItemID_CorrectSpell				= 'CSpl';//R0199

const AMenuItemID	kMenuItemID_OpenModePrefFolder 			= 'OpMF';
const AMenuItemID	kMenuItemID_Open 						= IDM_OPEN;
//ABaseTypes.hに移動const AMenuItemID	kMenuItemID_Close = 'clos';
const AMenuItemID	kMenuItemID_OpenInvisible 				= IDM_OpenInvisible;
const AMenuItemID	kMenuItemID_OpenSelected 				= IDM_OpenSelected;
const AMenuItemID	kMenuItemID_Open_RecentlyOpenedFile 	= IDM_Open_RecentlyOpenedFile;
const AMenuItemID	kMenuItemID_OpenImportFile 				= IDM_OpenImportFile;
const AMenuItemID	kMenuItemID_Delete_RecentlyOpenedFile 	= IDM_Delete_RecentlyOpenedFile;
const AMenuItemID	kMenuItemID_Delete_NotFoundRecentlyOpenedFile= 'DNRc';//#480
//#182 ABaseTypes.hに移動const AMenuItemID	kMenuItemID_Quit 						= IDM_Quit;
const AMenuItemID	kMenuItemID_Font 						= IDM_FONT;
const AMenuItemID	kMenuItemID_FontRootMenu				= '_fon';
const AMenuItemID	kMenuItemID_FontSize 					= IDM_FontSize;
const AMenuItemID	kMenuItemID_FontSize60					= IDM_FontSize60;
const AMenuItemID	kMenuItemID_FontSize80					= IDM_FontSize80;
const AMenuItemID	kMenuItemID_FontSize90					= IDM_FontSize90;
const AMenuItemID	kMenuItemID_FontSize95					= IDM_FontSize95;
const AMenuItemID	kMenuItemID_FontSize100					= IDM_FontSize100;
const AMenuItemID	kMenuItemID_FontSize105					= IDM_FontSize105;
const AMenuItemID	kMenuItemID_FontSize110					= IDM_FontSize110;
const AMenuItemID	kMenuItemID_FontSize120					= IDM_FontSize120;
const AMenuItemID	kMenuItemID_FontSize140					= IDM_FontSize140;
const AMenuItemID	kMenuItemID_FontSize160					= IDM_FontSize160;
const AMenuItemID	kMenuItemID_FontSize180					= IDM_FontSize180;
const AMenuItemID	kMenuItemID_FontSize240					= IDM_FontSize240;
const AMenuItemID	kMenuItemID_Jump 						= IDM_Jump;
const AMenuItemID	kMenuItemID_JumpExportToText			= IDM_JumpExportToText;
const AMenuItemID	kMenuItemID_JumpNextItem				= IDM_JumpNextItem;
const AMenuItemID	kMenuItemID_JumpPreviousItem			= IDM_JumpPreviousItem;
const AMenuItemID	kMenuItemID_SetMode 					= IDM_SetMode;
const AMenuItemID	kMenuItemID_SetModeRootMenu				= '_SMo';
const AMenuItemID	kMenuItemID_SetTextEncoding 			= IDM_SetTextEncoding;
const AMenuItemID	kMenuItemID_SetTextEncodingRootMenu		= '_STE';
const AMenuItemID	kMenuItemID_Transliterate_ToLower 		= IDM_Transliterate_ToLower;
const AMenuItemID	kMenuItemID_Transliterate_ToUpper 		= IDM_Transliterate_ToUpper;
const AMenuItemID	kMenuItemID_Transliterate 				= IDM_Transliterate;
const AMenuItemID	kMenuItemID_TransliterateRootMenu		= '_Trs';
const AMenuItemID	kMenuItemID_Minimize					= IDM_Minimize;
const AMenuItemID	kMenuItemID_ReturnCode_CR 				= IDM_ReturnCode_CR;
const AMenuItemID	kMenuItemID_ReturnCode_CRLF 			= IDM_ReturnCode_CRLF;
const AMenuItemID	kMenuItemID_ReturnCode_LF 				= IDM_ReturnCode_LF;
const AMenuItemID	kMenuItemID_ReturnCode_RootMenu			= '_RtC';
const AMenuItemID	kMenuItemID_ShowHideCrossCaret 			= IDM_ShowHideCrossCaret;
const AMenuItemID	kMenuItemID_SplitView					= IDM_SplitView;
const AMenuItemID	kMenuItemID_ConcatView					= IDM_ConcatView;
const AMenuItemID	kMenuItemID_DisplaySubPane				= IDM_ShowHideLeftSideBarColumn;//#212
const AMenuItemID	kMenuItemID_DisplayInfoPane				= IDM_ShowHideRightSideBarColumn;//#291
const AMenuItemID	kMenuItemID_SetupPane					= IDM_SetupPane;//#566
const AMenuItemID	kMenuItemID_CopyFilePath				= IDM_CopyFilePath;//#564
const AMenuItemID	kMenuItemID_OpenLuaConsole				= IDM_LuaConsole;//#567
//テキスト編集
//ABaseTypesへ移動const AMenuItemID	kMenuItemID_Undo 						= 'Undo';
//ABaseTypesへ移動const AMenuItemID	kMenuItemID_Redo 						= 'Redo';
const AMenuItemID	kMenuItemID_Save 						= IDM_Save;
const AMenuItemID	kMenuItemID_SaveAs						= IDM_SaveAs;
//ABaseTypesへ移動const AMenuItemID	kMenuItemID_Copy 						= 'Copy';
//ABaseTypesへ移動const AMenuItemID	kMenuItemID_Cut 						= 'Cut ';
//ABaseTypesへ移動const AMenuItemID	kMenuItemID_Paste 						= 'Past';
const AMenuItemID	kMenuItemID_AdditionalCopy				= IDM_AdditionalCopy;
//ABaseTypesへ移動const AMenuItemID	kMenuItemID_SelectAll					= 'SAll';
//ABaseTypesへ移動const AMenuItemID	kMenuItemID_Clear						= 'Cler';
const AMenuItemID	kMenuItemID_Indent						= IDM_Indent;
const AMenuItemID	kMenuItemID_ShiftLeft					= IDM_ShiftLeft;
const AMenuItemID	kMenuItemID_ShiftRight					= IDM_ShiftRight;
const AMenuItemID	kMenuItemID_Bookmark					= IDM_Bookmark;
const AMenuItemID	kMenuItemID_MoveToBookmark				= IDM_MoveToBookmark;
const AMenuItemID	kMenuItemID_CorrespondText				= IDM_CorrespondText;
const AMenuItemID	kMenuItemID_SelectBetweenCorrespondText	= IDM_SelectBetweenCorrespondText;//#359
const AMenuItemID	kMenuItemID_KeyToolMode					= IDM_KeyToolMode;
const AMenuItemID	kMenuItemID_KukeiSelectionMode			= IDM_KukeiSelectionMode;
const AMenuItemID	kMenuItemID_KukeiPaste					= IDM_KukeiPaste;
const AMenuItemID	kMenuItemID_SaveFTPAlias				= IDM_SaveFTPAlias;
const AMenuItemID	kMenuItemID_InsertLineEndAtWrap			= IDM_InsertLineEndAtWrap;
//検索
const AMenuItemID	kMenuItemID_Find						= IDM_Find;
const AMenuItemID	kMenuItemID_FindNext					= IDM_FindNext;
const AMenuItemID	kMenuItemID_FindPrevious				= IDM_FindPrevious;
const AMenuItemID	kMenuItemID_FindFromFirst				= IDM_FindFromFirst;
const AMenuItemID	kMenuItemID_FindList					= IDM_FindList;
const AMenuItemID	kMenuItemID_ReplaceSelectedTextOnly		= IDM_Replace;
const AMenuItemID	kMenuItemID_ReplaceAndFind				= IDM_ReplaceAndFind;
const AMenuItemID	kMenuItemID_ReplaceNext					= IDM_ReplaceNext;
const AMenuItemID	kMenuItemID_ReplaceAfterCaret			= IDM_ReplaceAfterCaret;
const AMenuItemID	kMenuItemID_ReplaceInSelection			= IDM_ReplaceInSelection;
const AMenuItemID	kMenuItemID_ReplaceAll					= IDM_ReplaceAll;
const AMenuItemID	kMenuItemID_SetFindText					= IDM_SetFindText;
const AMenuItemID	kMenuItemID_SetReplaceText				= IDM_SetReplaceText;
const AMenuItemID	kMenuItemID_EraseFindHighlight			= IDM_EraseFindHighlight;
//印刷
const AMenuItemID	kMenuItemID_PageSetup					= IDM_PageSetup;
const AMenuItemID	kMenuItemID_Print						= IDM_Print;
const AMenuItemID	kMenuItemID_PrintRange					= IDM_PrintRange;//#524
const AMenuItemID	kMenuItemID_ShowPrintOptionWindow		= IDM_ShowPrintOptionWindow;
//SCM R0006
const AMenuItemID	kMenuItemID_SCMCompareWithLatest		= IDM_SCMCompareWithLatest;
const AMenuItemID	kMenuItemID_SCMNextDiff					= IDM_SCMNextDiff;
const AMenuItemID	kMenuItemID_SCMPrevDiff					= IDM_SCMPrevDiff;
const AMenuItemID	kMenuItemID_SCMUpdateStatusAll			= IDM_SCMUpdateStatusAll;
//RC1
const AMenuItemID	kMenuItemID_ArrowToDef					= IDM_ArrowToDef;
//RC2
const AMenuItemID	kMenuItemID_ShowIdInfo					= IDM_ShowIdIno;
//#146
const AMenuItemID	kMenuItemID_NavigatePrev				= IDM_NavigatePrev;
const AMenuItemID	kMenuItemID_NavigateNext				= IDM_NavigateNext;
//#232
const AMenuItemID	kMenuItemID_ToolBar_Folder				= 'TBFo';
const AMenuItemID	kMenuItemID_ToolBar_EditButton			= IDM_CM_EditToolButton;
const AMenuItemID	kMenuItemID_ToolBar_InvalidButton		= IDM_CM_HideToolButton;
const AMenuItemID	kMenuItemID_ToolBar_DisplayAllButton	= IDM_CM_DisplayAllToolButton;
const AMenuItemID	kMenuItemID_ToolBar_DeleteButton		= IDM_CM_DeleteToolButton;
const AMenuItemID	kMenuItemID_ToolBar_UpdateDisplay		= IDM_CM_UpdateToolButton;
//#238
const AMenuItemID	kMenuItemID_IdInfoHistory				= 'IIHs';
//#212
const AMenuItemID	kMenuItemID_SubTextPaneMenu				= 'STPM';
//#357
const AMenuItemID	kMenuItemID_SwitchToNextTab				= IDM_SwitchToNextTab;
const AMenuItemID	kMenuItemID_SwitchToPrevTab				= IDM_SwitchToPrevTab;
//#390
const AMenuItemID	kMenuItemID_KeyRecordWindow				= IDM_KeyRecordWindow;
const AMenuItemID	kMenuItemID_KeyRecordStart				= IDM_KeyRecordStart;
const AMenuItemID	kMenuItemID_KeyRecordStop				= IDM_KeyRecordStop;
const AMenuItemID	kMenuItemID_KeyRecordPlay				= IDM_KeyRecordPlay;
const AMenuItemID	kMenuItemID_KeyRecordRecentlyPlay		= 'KRRP';
//win
const AMenuItemID	kMenuItemID_Help						= IDM_Help;
//#404
const AMenuItemID	kMenuItemID_FullScreenMode				= IDM_FullScreenMode;
//#455
const AMenuItemID	kMenuItemID_SCMCommit					= IDM_SCMCommit;
//#454
const AMenuItemID	kMenuItemID_JumpListHistory				= 'JmLH';
//#478
const AMenuItemID	kMenuItemID_FigMode_None				= IDM_FigMode_None;
const AMenuItemID	kMenuItemID_FigMode_Line				= IDM_FigMode_Line;
//#379
/*#899
const AMenuItemID	kMenuItemID_SubPaneMode_Manual			= IDM_SubPaneMode_Manual;
const AMenuItemID	kMenuItemID_SubPaneMode_PrevTab			= IDM_SubPaneMode_PrevTab;
const AMenuItemID	kMenuItemID_SubPaneMode_SameTab			= IDM_SubPaneMode_SameTab;
const AMenuItemID	kMenuItemID_CompareFiles				= IDM_CompareFiles;
*/
//#467
const AMenuItemID	kMenuItemID_EnableDefines				= IDM_DefineValues;
//#606
const AMenuItemID	kMenuItemID_SwitchHandToolMode			= 'SHTM';
//#619
const AMenuItemID	kMenuItemID_HideAllModeCreatorToolbarButton = 2001;
const AMenuItemID	kMenuItemID_HideAllUserToolbarButton 	= 2002;
const AMenuItemID	kMenuItemID_AddToolbarButton		 	= 2003;
//#638
const AMenuItemID	kMenuItemID_SwitchAutoIndentMode	 	= 2004;


//デバッグ
const AMenuItemID	kMenuItemID_HashStatics					= 'Hash';//#271
const AMenuItemID	kMenuItemID_DumpObjectID				= IDM_DumpObjectID;
const AMenuItemID	kMenuItemID_UnitTest					= IDM_UnitTest;
const AMenuItemID	kMenuItemID_UnitTest2					= IDM_UnitTest2;
const AMenuItemID	kMenuItemID_SDFDebugMode				= IDM_SDFDebugMode;
//#350 const AMenuItemID	kMenuItemID_ModePrefXML					= IDM_ModePrefXML;
const AMenuItemID	kMenuItemID_RegExpTest					= IDM_RegExpTest;
const AMenuItemID	kMenuItemID_LowMemoryTest				= IDM_LowMemoryTest;
const AMenuItemID	kMenuItemID_CompareFolder				= IDM_CompareFolder;

const AMenuItemID	kMenuItemID_CM_MultiFileFind			= IDM_CM_MultiFileFind;
const AMenuItemID	kMenuItemID_CM_FindNext					= IDM_CM_FindNext;
const AMenuItemID	kMenuItemID_CM_FindPrevious				= IDM_CM_FindPrevious;
const AMenuItemID	kMenuItemID_CM_Find						= IDM_CM_Find;
const AMenuItemID	kMenuItemID_CM_SelectAll				= IDM_CM_SelectAll;
const AMenuItemID	kMenuItemID_CM_Paste					= IDM_CM_Paste;
const AMenuItemID	kMenuItemID_CM_Copy						= IDM_CM_Copy;
const AMenuItemID	kMenuItemID_CM_Cut						= IDM_CM_Cut;
const AMenuItemID	kMenuItemID_CM_Clear					= IDM_CM_Clear;
const AMenuItemID	kMenuItemID_CM_DetachTab_Left			= IDM_CM_DetachTab_Left;
const AMenuItemID	kMenuItemID_CM_DetachTab_Right			= IDM_CM_DetachTab_Right;
const AMenuItemID	kMenuItemID_CM_DetachTab_Bottom			= 'C_Db';
const AMenuItemID	kMenuItemID_CM_CloseTab					= IDM_CM_CloseTab;
const AMenuItemID	kMenuItemID_CM_DisplayInSubTextPane		= IDM_CM_DisplayInSubTextPane;//#333
const AMenuItemID	kMenuItemID_CM_ClearItemFromRecentOpen	= IDM_CM_ClearItemFromRecentOpen;//R0186
const AMenuItemID	kMenuItemID_CM_ClearNotFoundItemsFromRecentOpen	= IDM_CM_ClearNotFoundItemsFromRecentOpen;//R0186
const AMenuItemID	kMenuItemID_CM_ClearAllItemsFromRecentOpen= 'C_C3';//#480
const AMenuItemID	kMenuItemID_CM_Dictionary				= IDM_CM_Dictionary;//R0231
const AMenuItemID	kMenuItemID_CM_Google					= IDM_CM_Google;//R0231
const AMenuItemID	kMenuItemID_CM_IdInfo					= IDM_CM_IdInfo;//RC2
const AMenuItemID	kMenuItemID_CM_ExternalComment			= IDM_CM_ExternalComment;//RC3
const AMenuItemID	kMenuItemID_CM_IndexView				= IDM_IndexViewExport;//#465
const AMenuItemID	kMenuItemID_CM_PrintRange				= IDM_CM_PrintSelected;//#524
const AMenuItemID	kMenuItemID_CM_OpenDefineValueWindow	= IDM_CM_DefineValue;//#467

const AMenuItemID kWindowCommand_OK = 'bok ';
const AMenuItemID kWindowCommand_Cancel = 'bcan';
const AMenuItemID kWindowCommand_ControlValueChanged = 'chgd';
const AMenuItemID kWindowCommand_ControlValueChangedA = 'chgA';
#endif

//ABaseTypes.hへ移動typedef ANumber AContextMenuID;


const AContextMenuID	kContextMenuID_TextSelected			= 101;
const AContextMenuID	kContextMenuID_TextUnselected		= 102;
//#688 const AContextMenuID	kContextMenuID_IndexWindow			= 103;
const AContextMenuID	kContextMenuID_Tab					= 104;
const AContextMenuID	kContextMenuID_RecentOpen			= 105;//R0186
const AContextMenuID	kContextMenuID_TextEncoding			= 106;//#232
const AContextMenuID	kContextMenuID_ReturnCode			= 107;//#232
const AContextMenuID	kContextMenuID_WrapMode				= 108;//#232
const AContextMenuID	kContextMenuID_Mode					= 109;//#232
const AContextMenuID	kContextMenuID_ToolButton			= 110;//#232
const AContextMenuID	kContextMenuID_IdInfoHistory		= 111;//#238
const AContextMenuID	kContextMenuID_FileList				= 112;//#442
const AContextMenuID	kContextMenuID_JumpList				= 113;//#442
const AContextMenuID	kContextMenuID_IdInfo				= 114;//#442
const AContextMenuID	kContextMenuID_IndexView			= 115;//#465
const AContextMenuID	kContextMenuID_JumpListHistory		= 116;//#454
const AContextMenuID	kContextMenuID_ToolBar				= 117;//#619
const AContextMenuID	kContextMenuID_RightSideBarPref		= 118;//#725
const AContextMenuID	kContextMenuID_LeftSideBarPref		= 119;//#725
const AContextMenuID	kContextMenuID_SameProjectHeader	= 120;//#725
const AContextMenuID	kContextMenuID_Percent				= 121;//#734
const AContextMenuID	kContextMenuID_SubWindows			= 122;//#725
//#0 未使用のためコメントアウト const AContextMenuID	kContextMenuID_Diff					= 123;//#112
const AContextMenuID	kContextMenuID_CandidateSearchMode	= 124;//#717

//#725
const AControlID	kControlID_Toolbar_TextEncoding					= 995001;
const AControlID	kControlID_Toolbar_ReturnCode					= 995002;
const AControlID	kControlID_Toolbar_WrapMode						= 995003;
const AControlID	kControlID_Toolbar_Mode							= 995004;
//#0 未使用のためコメントアウト const AControlID	kControlID_Toolbar_Diff							= 995005;
//
const AControlID	kControlID_Toolbar_SearchField					= 995101;
//
const AControlID	kControlID_Toolbar_ButtonStart					= 995011;
const AControlID	kControlID_Toolbar_ModePref						= 995011;
const AControlID	kControlID_Toolbar_Pref							= 995012;
const AControlID	kControlID_Toolbar_NewDocument					= 995013;
const AControlID	kControlID_Toolbar_MultiFileFind				= 995014;
const AControlID	kControlID_Toolbar_FindNext						= 995015;
const AControlID	kControlID_Toolbar_FindPrevious					= 995016;
const AControlID	kControlID_Toolbar_FindHighlight				= 995017;
const AControlID	kControlID_Toolbar_GoPrevious					= 995018;
const AControlID	kControlID_Toolbar_GoNext						= 995019;
const AControlID	kControlID_Toolbar_KeywordInfo					= 995020;
const AControlID	kControlID_Toolbar_Commit						= 995021;
const AControlID	kControlID_Toolbar_LeftSideBar					= 995022;
const AControlID	kControlID_Toolbar_2Screens						= 995023;
const AControlID	kControlID_Toolbar_SplitView					= 995024;
const AControlID	kControlID_Toolbar_RightSideBar					= 995025;
const AControlID	kControlID_Toolbar_FullScreen					= 995026;
const AControlID	kControlID_Toolbar_SubWindows					= 995027;
const AControlID	kControlID_Toolbar_AddMacroBar					= 995028;
const AControlID	kControlID_Toolbar_KeyRecordStart				= 995029;
const AControlID	kControlID_Toolbar_KeyRecordStop				= 995030;
const AControlID	kControlID_Toolbar_KeyRecordPlay				= 995031;
const AControlID	kControlID_Toolbar_ProhibitPopup				= 995032;
const AControlID	kControlID_Toolbar_MultiFileFindExecute			= 995033;
const AControlID	kControlID_Toolbar_ShowHideFloatingWindows		= 995034;
const AControlID	kControlID_Toolbar_ButtonEnd					= 995034;

//ABaseTypes.hへ移動const UInt32	kCommandID_UpdateCommandStatus			= 'UdCS';

const AInternalEventType	kInternalEvent_FTPProgress_Hide 						= 101;
const AInternalEventType	kInternalEvent_FTPProgress_SetProgress					= 102;
const AInternalEventType	kInternalEvent_FTPProgress_SetProgressText				= 103;
const AInternalEventType	kInternalEvent_FTPProgress_Error						= 104;
const AInternalEventType	kInternalEvent_FTPOpen									= 105;
const AInternalEventType	kInternalEvent_FTPLog									= 106;
const AInternalEventType	kInternalEvent_FTPFolder								= 108;
const AInternalEventType	kInternalEvent_MultiFileFindResult						= 109;
const AInternalEventType	kInternalEvent_MultiFileFind_Completed 					= 110;
const AInternalEventType	kInternalEvent_MultiFileFindProgress_SetProgress		= 111;
const AInternalEventType	kInternalEvent_MultiFileFindProgress_SetProgressText	= 112;
const AInternalEventType	kInternalEvent_ImportFileRecognizeCompleted				= 113;
const AInternalEventType	kInternalEvent_ChildProcessCompleted					= 114;
const AInternalEventType	kInternalEvent_ProjectFolderLoaderResult				= 115;//#402
const AInternalEventType	kInternalEvent_ReopenFile								= 116;//#682
const AInternalEventType	kInternalEvent_RevealDocument							= 117;//#678
const AInternalEventType	kInternalEvent_SyntaxRecognizerPaused					= 118;//#698
const AInternalEventType	kInternalEvent_WrapCalculatorPaused						= 119;//#699
const AInternalEventType	kInternalEvent_CandidateFinderPaused					= 120;//#699
//#193 const AInternalEventType	kInternalEvent_RemoteConnectionResponseReceived				= 121;//#361
const AInternalEventType	kInternalEvent_WordsListFinderPausedForCallGraf			= 122;//#723
const AInternalEventType	kInternalEvent_IdInfoFinderPaused						= 123;//#853
const AInternalEventType	kInternalEvent_FTPDirectoryResult						= 124;//#361
const AInternalEventType	kInternalEvent_ImportFileRecognizeProgress				= 125;//#725
const AInternalEventType	kInternalEvent_WordsListFinderPausedForWordsList		= 126;//#723
//const AInternalEventType	kInternalEvent_ImportFileRecognizePartialCompleted		= 127;
const AInternalEventType	kInternalEvent_StartUp									= 128;
const AInternalEventType	kInternalEvent_SaveResultDialog_ForcedToUTF8			= 129;
const AInternalEventType	kInternalEvent_SaveResultDialog_CharsetError			= 130;
const AInternalEventType	kInternalEvent_SameFolderLoaderCompleted				= 131;
const AInternalEventType	kInternalEvent_CSVKeywordLoaderCompleted				= 132;
const AInternalEventType	kInternalEvent_DocumentFileScreenCompleted				= 133;
const AInternalEventType	kInternalEvent_DocumentFileScreenError					= 134;//#962
const AInternalEventType	kInternalEvent_AppActivated								= 135;//#974
const AInternalEventType	kInternalEvent_AppleEventOpenDoc						= 136;//#1006

//#688 キーバインド動作をABaseTypes.hへ移動

//#688 typedef EKeyAction AKeyBindAction;

//ドキュメントタイプ
const ADocumentType	kDocumentType_Text 					= 9101;
const ADocumentType	kDocumentType_IndexWindow 			= 9102;
const ADocumentType	kDocumentType_FTPAlias 				= 9103;
const ADocumentType	kDocumentType_ModePref 				= 9104;
const ADocumentType	kDocumentType_TabSet				= 9105;//#1050

//ウインドウタイプ
const AWindowType	kWindowType_Text 					= 101;
//#725 const AWindowType	kWindowType_Index 					= 102;
const AWindowType	kWindowType_Fusen 					= 103;//#138
const AWindowType	kWindowType_FusenEdit 				= 104;//#138
const AWindowType	kWindowType_SetProjectFolder 		= 105;
//#858 const AWindowType	kWindowType_ProjectFolderAlert 		= 106;
const AWindowType	kWindowType_FusenList		 		= 107;
const AWindowType	kWindowType_NewFTPFile		 		= 108;
const AWindowType	kWindowType_TextCount		 		= 109;
const AWindowType	kWindowType_ModePref		 		= 110;
const AWindowType	kWindowType_Find		 			= 111;
const AWindowType	kWindowType_MultiFileFind 			= 112;
const AWindowType	kWindowType_AppPref		 			= 113;
const AWindowType	kWindowType_FileList	 			= 114;
const AWindowType	kWindowType_IdInfo		 			= 115;
const AWindowType	kWindowType_CandidateList 			= 116;
const AWindowType	kWindowType_FTPFolder	 			= 117;
const AWindowType	kWindowType_FolderList	 			= 118;
const AWindowType	kWindowType_KeyToolList	 			= 119;
const AWindowType	kWindowType_FTPLog		 			= 120;
const AWindowType	kWindowType_TraceLog		 		= 121;
const AWindowType	kWindowType_BigFileAlert	 		= 122;
const AWindowType	kWindowType_MultiFileFindProgress	= 123;
const AWindowType	kWindowType_FTPProgress	 			= 124;
const AWindowType	kWindowType_About					= 125;
const AWindowType	kWindowType_TextSheet_MoveLine		= 126;
const AWindowType	kWindowType_TextSheet_FontSize		= 127;
const AWindowType	kWindowType_TextSheet_WrapLetterCount= 128;
const AWindowType	kWindowType_TextSheet_CorrectEncoding= 129;
const AWindowType	kWindowType_TextSheet_PrintOption	= 130;
const AWindowType	kWindowType_TextSheet_Property		= 131;
const AWindowType	kWindowType_TextSheet_DropWarning	= 132;
const AWindowType	kWindowType_AddToolButton			= 133;
const AWindowType	kWindowType_EditByOtherApp			= 134;
const AWindowType	kWindowType_JumpList				= 135;
const AWindowType	kWindowType_AddNewMode				= 136;
const AWindowType	kWindowType_ChangeToolButtonIcon	= 137;
const AWindowType	kWindowType_ModeExecutableAlert		= 138;
const AWindowType	kWindowType_CompareFolder			= 139;
const AWindowType	kWindowType_FindMemory				= 140;
const AWindowType	kWindowType_KeyRecord				= 141;//#390
const AWindowType	kWindowType_DevTools				= 142;//#436
const AWindowType	kWindowType_TextSheet_SCMCommit		= 143;//#455
const AWindowType	kWindowType_DiffInfo		 		= 144;//#379
const AWindowType	kWindowType_Diff		 			= 145;//#379
const AWindowType	kWindowType_EnabledDefines 			= 146;//#467
const AWindowType	kWindowType_CallGraf	 			= 147;//#723
const AWindowType	kWindowType_Previewer	 			= 148;//#734
const AWindowType	kWindowType_TextMarker				= 149;//#750
const AWindowType	kWindowType_ClipboardHistory		= 150;//#152
const AWindowType	kWindowType_DocInfo					= 151;//#142
const AWindowType	kWindowType_Ruler					= 152;//#725
const AWindowType	kWindowType_SubWindowList			= 153;//#725
const AWindowType	kWindowType_KeyActionList			= 154;//#738
const AWindowType	kWindowType_EditProgress			= 155;//#846
const AWindowType	kWindowType_SubTextShadow			= 156;//#725
const AWindowType	kWindowType_WordsList				= 157;//#723
const AWindowType	kWindowType_Notification			= 158;//#725
const AWindowType	kWindowType_FindResult				= 159;//#725
const AWindowType	kWindowType_ChangeToolButton		= 160;//#1344

//ビュータイプ
const AViewType		kViewType_Text 						= 5101;
const AViewType		kViewType_LineNumber 				= 5102;
const AViewType		kViewType_Separator 				= 5103;
const AViewType		kViewType_TabSelector 				= 5104;
const AViewType		kViewType_Index 					= 5105;
const AViewType		kViewType_Info 						= 5106;
const AViewType		kViewType_InfoHeader 				= 5107;
const AViewType		kViewType_Ruler 					= 5108;
const AViewType		kViewType_IdInfo 					= 5109;//RC2
const AViewType		kViewType_CandidateList 			= 5110;//RC2
const AViewType		kViewType_KeyToolList 				= 5111;//R0073
const AViewType		kViewType_Fusen 					= 5112;//#138
const AViewType		kViewType_List_FusenList 			= 5113;//#138
const AViewType		kViewType_FindResultSeparator		= 5114;//#92
const AViewType		kViewType_Toolbar	 				= 5115;
const AViewType		kViewType_DiffInfo 					= 5116;//#379
const AViewType		kViewType_CallGraf 					= 5117;//#723
const AViewType		kViewType_DocInfo					= 5119;//#142
const AViewType		kViewType_SubWindowsList			= 5120;//#725
const AViewType		kViewType_SubWindowsSeparator		= 5121;//#725
const AViewType		kViewType_SubWindowsTitlebar		= 5122;//#725
const AViewType		kViewType_CallGrafHeader			= 5123;//#723
const AViewType		kViewType_SubWindowBackground		= 5124;//#725
const AViewType		kViewType_Footer					= 5125;//#725
const AViewType		kViewType_FooterSeparator			= 5126;//#725
const AViewType		kViewType_SubWindowHeader			= 5127;//#723
const AViewType		kViewType_SubTextShadow				= 5128;//#725
const AViewType		kViewType_WordsList					= 5129;//#723
const AViewType		kViewType_Notification				= 5130;//#725
const AViewType		kViewType_TextViewHSeparator		= 5131;//#725
const AViewType		kViewType_SubWindowStatusBar		= 5132;//#725
const AViewType		kViewType_SubWindowSizeBox			= 5133;//#725

//#427
const ACurlTransactionType	kCurlTransactionType_AutoUpdateRevision		= 1001;
const ACurlTransactionType	kCurlTransactionType_AutoUpdateZip			= 1002;
const ACurlTransactionType	kCurlTransactionType_AddModeFromWeb_Revision	= 1003;
const ACurlTransactionType	kCurlTransactionType_AddModeFromWeb_Zip			= 1004;
const ACurlTransactionType	kCurlTransactionType_LoadCSVForKeyword			= 1005;//#796


const AItemCount	kRecentlyOpendFileMax = 500;

/*ABaseTypes.hに移動
struct ATextPoint
{
	AIndex	x;
	AIndex	y;
};

typedef AIndex	ATextIndex;
*/

//ABaseTypes.h に移動#define hex2asc(x) (((x)>=0xa)?((x)-0xa+'A'):((x)+'0'))


const ATextArrayMenuID	kTextArrayMenuID_TextEncoding = 1;
const ATextArrayMenuID	kTextArrayMenuID_EnabledMode = 2;//#914
const ATextArrayMenuID	kTextArrayMenuID_ReturnCode = 3;
const ATextArrayMenuID	kTextArrayMenuID_WrapMode = 4;
const ATextArrayMenuID	kTextArrayMenuID_DefaultMode = 5;//#463
//#844 const ATextArrayMenuID	kTextArrayMenuID_ModeUpdateURL = 6;//#427
const ATextArrayMenuID	kTextArrayMenuID_FileFilterPreset = 7;//#617
const ATextArrayMenuID	kTextArrayMenuID_RegExpList = 8;//#801
const ATextArrayMenuID	kTextArrayMenuID_TextMarker = 9;//#750
const ATextArrayMenuID	kTextArrayMenuID_ColorScheme = 10;//#889
const ATextArrayMenuID	kTextArrayMenuID_AllMode = 11;//#914
const ATextArrayMenuID	kTextArrayMenuID_Pref = 12;
const ATextArrayMenuID	kTextArrayMenuID_Skin = 13;
const ATextArrayMenuID	kTextArrayMenuID_AccessPlugIn = 14;//#193

#if IMPLEMENTATION_FOR_MACOSX
//AppleEvent
enum
{
	keyCharacterCode = 'chac'
};
#endif

//==================ImageID==================
//
const AImageID	kImageID_iconSwlFlag					= 102;
//#1316 const AImageID	kImageID_iconSwlPin						= 103;
const AImageID	kImageID_iconSwlParentPath				= 104;
/*#1316
const AImageID	kImageID_SubWindowTitlebar_Floating0	= 110;
const AImageID	kImageID_SubWindowTitlebar_Floating1	= 111;
const AImageID	kImageID_SubWindowTitlebar_Floating2	= 112;
const AImageID	kImageID_SubWindowTitlebar_Overlay0		= 120;
const AImageID	kImageID_SubWindowTitlebar_Overlay1		= 121;
const AImageID	kImageID_SubWindowTitlebar_Overlay2		= 122;
*/

//
const AImageID	kImageID_barSwCloseButton_1				= 130;
const AImageID	kImageID_barSwCloseButton_p				= 131;
const AImageID	kImageID_barSwCloseButton_h				= 132;

/*#1316
//
const AImageID	kImageID_Tab0					= 200;
const AImageID	kImageID_Tab1					= 201;
const AImageID	kImageID_Tab2					= 202;
const AImageID	kImageID_Tab3					= 203;
const AImageID	kImageID_Tab4					= 204;
const AImageID	kImageID_Tab5					= 205;
const AImageID	kImageID_Tab6					= 206;
const AImageID	kImageID_Tab7					= 207;
const AImageID	kImageID_Tab8					= 208;

//
const AImageID	kImageID_tabFront_1				= 220;
const AImageID	kImageID_tabFront_2				= 221;
const AImageID	kImageID_tabFront_3				= 222;

//
const AImageID	kImageID_tabFront_h_1			= 223;
const AImageID	kImageID_tabFront_h_2			= 224;
const AImageID	kImageID_tabFront_h_3			= 225;

//
const AImageID	kImageID_tabNext_1				= 226;
const AImageID	kImageID_tabNext_2				= 227;
const AImageID	kImageID_tabNext_3				= 228;

//
const AImageID	kImageID_tabNext_h_1			= 229;
const AImageID	kImageID_tabNext_h_2			= 230;
const AImageID	kImageID_tabNext_h_3			= 231;

//
const AImageID	kImageID_tabOther_h_1			= 232;
const AImageID	kImageID_tabOther_h_2			= 233;
const AImageID	kImageID_tabOther_h_3			= 234;
*/

//
const AImageID	kImageID_tabDot					= 235;

//
const AImageID	kImageID_iconPnSeparateVertical	= 236;

//
const AImageID	kImageID_iconPnLeft				= 237;
const AImageID	kImageID_iconPnRight			= 238;
const AImageID	kImageID_iconPnOption			= 239;
const AImageID	kImageID_iconPnHandle			= 240;

/*#1316
//
const AImageID	kImageID_bg32MacroBar_1			= 250;
const AImageID	kImageID_bg32MacroBar_2			= 251;
const AImageID	kImageID_bg32MacroBar_3			= 252;
const AImageID	kImageID_bg32MacroBar_4			= 253;
const AImageID	kImageID_bg32MacroBar_5			= 254;
const AImageID	kImageID_bg32MacroBar_6			= 255;
const AImageID	kImageID_bg32MacroBar_7			= 256;
const AImageID	kImageID_bg32MacroBar_8			= 257;
const AImageID	kImageID_bg32MacroBar_9			= 258;
*/
const AImageID	kImageID_RectButton0			= 300;
const AImageID	kImageID_RectButton1			= 301;
const AImageID	kImageID_RectButton2			= 302;

//
const AImageID	kImageID_btnFldngArrowRight		= 320;
const AImageID	kImageID_btnFldngArrowRight_h	= 321;
const AImageID	kImageID_btnFldngArrowDown		= 322;
const AImageID	kImageID_btnFldngArrowDown_h	= 323;
const AImageID	kImageID_btnFldngArrowEnd		= 324;
const AImageID	kImageID_btnFldngArrowEnd_h		= 325;

//
const AImageID	kImageID_iconSwlDocument		= 330;
const AImageID	kImageID_iconSwlFolder			= 331;

//
const AImageID	kImageID_btnEditorClose			= 340;
const AImageID	kImageID_btnEditorClose_h		= 341;

//
const AImageID	kImageID_btnNaviRight			= 350;
const AImageID	kImageID_btnNaviLeft			= 351;

//
const AImageID	kImageID_iconTbCircleCursorButton = 360;

//
const AImageID	kImageID_iconSwUnlock = 370;
const AImageID	kImageID_iconSwLock = 371;
const AImageID	kImageID_iconSwSearchList = 372;
const AImageID	kImageID_iconSwWordReferrer = 373;
const AImageID	kImageID_iconSwTool = 374;

//
const AImageID	kImageID_iconTbSeparateVertical = 380;
const AImageID	kImageID_iconTbKeyRecord = 381;

//サイドバータイトルアイコン
const AImageID	kImageID_iconSwBox_ProjectFolder	= 1101;
const AImageID	kImageID_iconSwBox_RecentlyOpened	= 1102;
const AImageID	kImageID_iconSwBox_SameFolder		= 1103;
const AImageID	kImageID_iconSwBox_Remote			= 1104;
const AImageID	kImageID_iconSwTree					= 1105;
const AImageID	kImageID_iconSwList					= 1106;
const AImageID	kImageID_iconSwMarkerPen			= 1107;
const AImageID	kImageID_iconSwPreview				= 1108;
const AImageID	kImageID_iconSwKeyWord				= 1109;
const AImageID	kImageID_iconSwTextCount			= 1110;
const AImageID	kImageID_iconSwHome					= 1111;

//メインウインドウ内各アイコン
const AImageID	kImageID_iconSwCompare				= 1201;
const AImageID	kImageID_iconSwReplace				= 1202;
const AImageID	kImageID_iconSwShow					= 1203;
const AImageID	kImageID_iconSwDocument				= 1204;

const AImageID	kImageID_testblack				= 1205;

//ファイルリストサブウインドウ機能切り替えボタン
const AImageID	kImageID_swSwitchFunction			= 1301;//#1368

//ABase.h に移動const AIconID	kIconID_NoIcon					= 101;
const AIconID	kIconID_Mi_Batsu				= 101;
const AIconID	kIconID_Mi_FileList_RecentlyOpened = 102;
const AIconID	kIconID_Mi_FileList_SameFolder	= 103;
const AIconID	kIconID_Mi_FileList_Window		= 104;
const AIconID	kIconID_Mi_FileList_Project		= 105;
const AIconID	kIconID_Mi_BuiltinToolButton	= 106;
//const AIconID	kIconID_Mi_ArrowRight			= 107;
//const AIconID	kIconID_Mi_ArrowDown			= 108;
const AIconID	kIconID_Mi_Plus					= 109;
const AIconID	kIconID_Mi_Gear					= 110;
const AIconID	kIconID_Mi_SplitView			= 111;
const AIconID	kIconID_Mi_ConcatView			= 112;
const AIconID	kIconID_Mi_Close				= 113;
const AIconID	kIconID_Mi_Find					= 114;//#137
const AIconID	kIconID_Mi_Sync					= 115;//#212
const AIconID	kIconID_Mi_Swap					= 116;//#212
const AIconID	kIconID_Amazon					= 117;//amazon
const AIconID	kIconID_Play					= 118;//#390
const AIconID	kIconID_Record					= 119;//#390
const AIconID	kIconID_Stop					= 120;//#390
const AIconID	kIconID_Pause					= 121;//#390
const AIconID	kIconID_RemoveCross				= 122;//#390
const AIconID	kIconID_Mi_Diff					= 123;//#379
const AIconID	kIconID_Mi_Close_White1			= 124;//#164
const AIconID	kIconID_Mi_Close_White2			= 125;//#164
const AIconID	kIconID_Mi_VSeparator			= 126;//#634
const AIconID	kIconID_Mi_InfoPane				= 127;//#634
const AIconID	kIconID_Mi_SubPane				= 128;//#634

const AIconID	kIconID_Tool_Save				= 1001;//#232
const AIconID	kIconID_Tool_Find				= 1002;//#232
const AIconID	kIconID_Tool_FindPrev			= 1003;//#232
const AIconID	kIconID_Tool_FindNext			= 1004;//#232
const AIconID	kIconID_Tool_Undo				= 1005;//#232
const AIconID	kIconID_Tool_Redo				= 1006;//#232
const AIconID	kIconID_Tool_FindInFolder		= 1007;//#232
const AIconID	kIconID_Tool_Indent				= 1008;//#232
const AIconID	kIconID_Tool_Stamp				= 1009;//#232
const AIconID	kIconID_Tool_NavPrev			= 1010;//#146
const AIconID	kIconID_Tool_NavNext			= 1011;//#146
//const AIconID	kIconID_Tool_Folder				= 1012;//#232
const AIconID	kIconID_Tool_Highlight			= 1013;//#232
const AIconID	kIconID_Tool_TreePrev			= 1014;//#232
const AIconID	kIconID_Tool_TreeNext			= 1015;//#232
const AIconID	kIconID_Tool_Info				= 1016;//#232
const AIconID	kIconID_Tool_Table				= 1017;//#232
const AIconID	kIconID_Tool_Guide				= 1018;//#232
const AIconID	kIconID_Tool_Gear				= 1019;//#232
const AIconID	kIconID_Tool_Comment			= 1020;//#232
const AIconID	kIconID_Tool_CommentLine		= 1021;//#232
const AIconID	kIconID_Tool_Comment_Html		= 1022;//#232
const AIconID	kIconID_Tool_Comment_C			= 1023;//#232
const AIconID	kIconID_Tool_CommentLine_C		= 1024;//#232
const AIconID	kIconID_Tool_Clock				= 1025;//#232
const AIconID	kIconID_Tool_Color				= 1026;//#232
const AIconID	kIconID_Tool_OrderedList		= 1027;//#232
const AIconID	kIconID_Tool_UnorderedList		= 1028;//#232
const AIconID	kIconID_Tool_Home				= 1029;//#232
const AIconID	kIconID_Tool_Image				= 1030;//#232
const AIconID	kIconID_Tool_Key				= 1031;//#232
const AIconID	kIconID_Tool_Leaf				= 1032;//#232
const AIconID	kIconID_Tool_Light				= 1033;//#232
const AIconID	kIconID_Tool_Minus				= 1034;//#232
const AIconID	kIconID_Tool_Plus				= 1035;//#232
const AIconID	kIconID_Tool_Report				= 1036;//#232
const AIconID	kIconID_Tool_Script				= 1037;//#232
const AIconID	kIconID_Tool_SpellCheck			= 1038;//#232
const AIconID	kIconID_Tool_StickyNote			= 1039;//#232
const AIconID	kIconID_Tool_Wand				= 1040;//#232
const AIconID	kIconID_Tool_Doc				= 1041;//#248
const AIconID	kIconID_Tool_Docs				= 1042;//#248
const AIconID	kIconID_Tool_DocText			= 1043;//#248
const AIconID	kIconID_Tool_PageNext			= 1044;//#248
const AIconID	kIconID_Tool_PagePrev			= 1045;//#248
const AIconID	kIconID_Tool_SubPane			= 1046;//#340
const AIconID	kIconID_Tool_FontSize			= 1047;//#354
const AIconID	kIconID_Tool_FullScreen			= 1048;//#404
const AIconID	kIconID_Tool_Commit				= 1049;//#455
const AIconID	kIconID_Tool_Up					= 1050;//#455
const AIconID	kIconID_Tool_Down				= 1051;//#455
const AIconID	kIconID_Tool_Diff				= 1052;//#379
const AIconID	kIconID_Tool_DocumentText		= 1053;//win
const AIconID	kIconID_Tool_ProjectFolder		= 1054;//win
const AIconID	kIconID_Tool_Printer			= 1055;//win
const AIconID	kIconID_Tool_Tab				= 1056;//win
const AIconID	kIconID_Tool_Checkboxes			= 1057;//win
const AIconID	kIconID_Tool_Property			= 1058;//win
const AIconID	kIconID_Tool_Kanji				= 1059;//win
const AIconID	kIconID_Sort					= 1060;//win
const AIconID	kIconID_Tool_DocumentTree		= 1061;//win
const AIconID	kIconID_Tool_DropdownList		= 1062;//win
const AIconID	kIconID_Tool_NavNext2			= 1063;//win
const AIconID	kIconID_Tool_Balance			= 1064;//win
const AIconID	kIconID_Tool_SplitView			= 1065;//win
const AIconID	kIconID_Tool_Selection			= 1066;//win
const AIconID	kIconID_Tool_Question			= 1067;//win
const AIconID	kIconID_Tool_ProjectFolderStamp	= 1068;//win
const AIconID	kIconID_Tool_OpenProjectFolder	= 1069;//win
const AIconID	kIconID_Tool_DockLeft			= 1070;//win
const AIconID	kIconID_Tool_ArrowCurveUp		= 1071;//win
const AIconID	kIconID_Tool_Hand				= 1072;//#606

const AModeIndex	kStandardModeIndex = 0;

enum AToolCommandID
{
	tc_NOP = 0,
	tc_CARET,//
	tc_MOVECARET_HERE,//
	tc_SELECTED,//
	tc_PASTE,//
	tc_FILENAME,//
	tc_PATH,
	tc_FULL_PATH,
	tc_DATE,//
	tc_DATE_SHORT,//
	tc_DATE_LONG,//
	tc_DATE_6,//
	tc_WAIT,//
	tc_WAIT_ACTIVATE,
	tc_ASCOMPILE,
	tc_COMPILE_APPLESCRIPT,
	tc_ASRUN,
	tc_RUN_APPLESCRIPT,
	tc_TOOL,
	tc_AUTOTAB,
	tc_LAUNCH,
	tc_DROP,
	tc_SENDSELECTED,
	tc_SEND_SELECTED,
	tc_SENDSELECTEDR,
	tc_SEND_SELECTED_REPLACE,
	tc_SENDSELECTEDN,
	tc_SEND_SELECTED_NEWDOCUMENT,
	tc_TIME,//
	tc_CLOSEDROP,
	tc_CLOSE_AND_DROP,
	tc_IMAGEWIDTH,
	tc_IMAGEHEIGHT,
	tc_IMAGEHALFWIDTH,//#1173
	tc_IMAGEHALFHEIGHT,//#1173
	tc_SAVE,//
	tc_SAVEAS,//
	tc_NEWDOCUMENT,//
	tc_OPEN_DIALOG,//
	tc_CLOSE,//
	tc_CHANGEMODE,//
	tc_CHANGEFONT,//
	tc_CHANGESIZE,//
	tc_CHANGETO_SHIFTJIS,//
	tc_CHANGETO_JIS,//
	tc_CHANGETO_EUC,//
	tc_CHANGETO_UCS2,//
	tc_CHANGETO_UTF8,//
	tc_CHANGETO_ISO88591,//
	tc_CHANGETO_CR,//
	tc_CHANGETO_LF,//
	tc_CHANGETO_CRLF,//
	tc_CHANGETO_NOWRAP,//
	tc_CHANGETO_WORDWRAP,//
	tc_CHANGETO_LETTERWRAP,//
	tc_CHANGETAB,//
	tc_CHANGECREATOR,//
	tc_CHANGETYPE,//
	tc_FIND_REGEXP_NEXT,//
	tc_MULTIFILEFIND_REGEXP,
	tc_REPLACE_REGEXP_NEXT,//
	tc_REPLACE_REGEXP_SELECTED,//
	tc_REPLACE_REGEXP_AFTERCARET,//
	tc_REPLACE_REGEXP_ALL,//
	tc_INDENT,//
	tc_SHIFT_LEFT,//
	tc_SHIFT_RIGHT,//
	tc_ABBREV_NEXT,//
	tc_ABBREV_PREV,//
	tc_DELETE_NEXT,//
	tc_DELETE_PREV,//
	tc_DELETE_ENDOFLINE,//
	tc_DELETE_SELECTED,//
	tc_MOVECARET_UP,//
	tc_MOVECARET_DOWN,//
	tc_MOVECARET_LEFT,//
	tc_MOVECARET_RIGHT,//
	tc_MOVECARET_HOME,//
	tc_MOVECARET_END,//
	tc_MOVECARET_ENDOFLINE,//
	tc_MOVECARET_STARTOFLINE,//
	tc_MOVECARET_PREVWORD,//
	tc_MOVECARET_NEXTWORD,//
	tc_MOVECARET_LINE,//
	tc_MOVECARET_PARAGRAPH,//
	tc_MOVECARET_COLUMN,//
	tc_SELECT_UP,//
	tc_SELECT_DOWN,//
	tc_SELECT_LEFT,//
	tc_SELECT_RIGHT,//
	tc_SELECT_HOME,//
	tc_SELECT_END,//
	tc_SELECT_ENDOFLINE,//
	tc_SELECT_STARTOFLINE,//
	tc_SELECT_PREVWORD,//
	tc_SELECT_NEXTWORD,//
	tc_SELECT_ALL,//
	tc_SCROLL_UP,//
	tc_SCROLL_DOWN,//
	tc_SCROLL_LEFT,//
	tc_SCROLL_RIGHT,//
	tc_SCROLL_HOME,//
	tc_SCROLL_END,//
	tc_SCROLL_PREVPAGE,//
	tc_SCROLL_NEXTPAGE,//
	tc_SCROLL_MOVECARET_PREVPAGE,//
	tc_SCROLL_MOVECARET_NEXTPAGE,//
	tc_UNDO,//
	tc_CUT,//
	tc_COPY,//
	tc_CUT_ENDOFLINE,//
	tc_KEYTOOL,
	tc_RETURN,//
	tc_INDENT_RETURN,//
	tc_TAB,//
	tc_INDENT_TAB,//
	tc_SPACE,//
	tc_URL,
	tc_TITLE,
	tc_REDO,//
	tc_MULTIFILEFIND_DIALOG,//
	tc_FIND_DIALOG,//
	tc_FIND_REGEXP_PREV,//
	tc_FIND_SELECTED_NEXT,//
	tc_FIND_SELECTED_PREV,//
	tc_MULTIFILEFIND_SELECTED,//
	tc_OPEN_SELECTED,
	tc_COMMAND,
	tc_KEY,
	tc_OPEN,
	tc_YEAR,
	tc_YEAR_2,
	tc_YEAR_4,
	tc_MONTH,
	tc_MONTH_2,
	tc_DAY,
	tc_DAY_2,
	tc_SELECT_WHOLE_PARAGRAPH,
	tc_SELECT_WHOLE_LINE,
	tc_CORRESPONDENCE,
	tc_RECENTER,
	tc_OPEN_URL,
	tc_OPEN_FILE,
	tc_SELECTMODE,
	tc_CUT_STARTOFLINE,
	tc_SELECT_BRACE,
	tc_SAMELETTER_UP,
	tc_DEBUG1,
	tc_BROWSE,
	tc_DEBUGINFO,
	tc_DEBUG2,
	tc_DUPLICATE_LINE,
	tc_INSERT_LINE,
	tc_COMPILE_TEX,
	tc_PREVIEW_TEX,
	tc_COMPILEPREVIEW_TEX,
	tc_DIRECTORY,
	tc_FILENAME_NOSUFFIX,
	tc_SWITCH_WINDOW,
	tc_MOVECARET_ENDOFPARAGRAPH,
	tc_MOVECARET_STARTOFPARAGRAPH,
	tc_DAYOFTHEWEEK,
	tc_GT3,//B0012
	tc_LT3,//B0012
	tc_GT1,
	tc_LT1,
	tc_MOVECARET_PREVWORD_LINESTOP,//R0052
	tc_MOVECARET_NEXTWORD_LINESTOP,//R0052
	tc_DIR_LIST,//R0012
	tc_SELECT_PREVWORD_LINESTOP,//R0063
	tc_SELECT_NEXTWORD_LINESTOP,//R0063
	tc_FIND_REGEXP_FROMFIRST,//R0075
	tc_CHANGETO,//R-uni
	tc_MODIFYTO,//R-uni
	tc_SWAPLINE,//R-uni
	tc_SWAPLETTER,//#1154
	tc_UNITTEST,//R-abs
	tc_MULTIFILEFIND_SELECTED_INPROJECT,//
	tc_SWITCH_NEXTSPLITVIEW,
	tc_SWITCH_PREVSPLITVIEW,
	tc_SPLITVIEW,
	tc_CLOSE_SPLITVIEW,
	tc_SHOW_DEFINITION,
	tc_COPY_WITHHTMLCOLOR,//R0015
	tc_NUMBER_REGEXP_SELECTED,//R0189
	tc_TRANSLITERATE_SELECTED,//R0017
	tc_TRANSLITERATE_REGEXP_SELECTED,//R0017
	tc_DUMP_UNICODE_CHARCODE,//For debug
	tc_DELETE_ENDOFPARAGRAPH,//R0213
	tc_CUT_ENDOFPARAGRAPH,//R0213
	tc_ADDITIONAL_CUT_ENDOFPARAGRAPH,//R0213
	tc_ADDITIONAL_CUT,//R0213
	tc_ADDITIONAL_COPY,//R0213
	tc_TOOLINDENT,//R0000
	tc_CONCATALL_INPROJECT,//R0000
	tc_COMPARE_WITH_LATEST,//R0006
	tc_MOVECARET_NEXTDIFF,//R0006
	tc_MOVECARET_PREVDIFF,//R0006
	tc_ARROWTODEF,//RC1
	tc_IDINFO,//RC2
	tc_SHOWHIDE_CROSSCARET,//R0185
	tc_HOUR,//R0116
	tc_HOUR_2,//R0116
	tc_HOUR12,//R0116
	tc_HOUR12_2,//R0116
	tc_AMPM,//R0116
	tc_MINUTE,//R0116
	tc_MINUTE_2,//R0116
	tc_SECOND,//R0116
	tc_SECOND_2,//R0116
	tc_DICT_POPUP,//R0231
	tc_DICT,//R0231
	tc_FIX_IDINFO,//RC2
	tc_EDIT_MYCOMMENT,//RC3
	tc_NAV_PREV,//#146
	tc_NAV_NEXT,//#146
	tc_TOOLBARICON,//#232
	tc_HIGHLIGHT_SELECTED,//#232
	tc_HIGHLIGHT_CLEAR,//#232
	tc_INDEXWINDOW_NEXT,//#232
    tc_INDEXWINDOW_PREV,//#232
	tc_SELECT_WHOLE_PARAGRAPH_NOLINEEND,//#223
	tc_DELETE_WHOLE_PARAGRAPH,//#124
	tc_NEWDOCUMENT_MOVETARGET,//#56
	tc_NEWDOCUMENT_SAMEMODE,//#56
	tc_NEWDOCUMENT_SAMEMODE_MOVETARGET,//#56
	tc_SELECT_NEXTHEADER,//#150
	tc_SELECT_PREVHEADER,//#150
	tc_SELECTED_PLACEHOLDER,//#110
	tc_SETVAR_SELECTED,//#50
	tc_GETVAR,//#50
	tc_TAB_BYSPACE,//#249
	tc_INDENT_BYSPACE_TAB,//#249
	tc_INDENT_BYSPACE_TAB_BYSPACE,//#249
	tc_INDENT_BYSPACE,//#249
	tc_INDENT_BYSPACE_RETURN,//#249
	tc_INDENT_ONLYNEWLINE_RETURN,//#249
	tc_INDENT_ONLYNEWLINE_BYSPACE_RETURN,//#249
	tc_CHANGEINDENT,//#251
	tc_SHOWHIDE_SUBPANECOLUMN,//#212
	tc_SHOWHIDE_INFOPANECOLUMN,//#291
	tc_CHANGEFONT_MODEDEFAULT,//#354
	tc_REVERTFONT,//#354
	tc_SWITCH_NEXTTAB,//#357
	tc_SWITCH_PREVTAB,//#357
	tc_URL_NOURLENCODE,//#363
	tc_KEYRECORD_START,//#390
	tc_KEYRECORD_START_WITHWINDOW,//#390
	tc_KEYRECORD_STOP,//#390
	tc_KEYRECORD_PLAY,//#390
	tc_TABTOWINDOW,//#389
	tc_WINDOWTOTAB,//#389
	tc_SELECT_CORRESPONDENCE,//#359
	tc_ADDITIONAL_COPY_TEXT,//#425
	tc_FIND_NEXT,//win
	tc_FIND_PREV,//win
	tc_SETFINDTEXT_SELECTED,//win
	tc_SHOW_PRINTDIALOG,//win
	tc_SHOW_FINDDIALOG,//win
	tc_SHOW_MULTIFINDDIALOG,//win
	tc_FULLSCREEN,//#404
	tc_SCM_COMMIT,//#455
	tc_DIFF_REPOSITORY,//#379
	tc_DAYOFWEEK,//#537
	tc_DAYOFWEEK_LONG,//#537
	tc_SWITCH_PREVWINDOW,//#552
	tc_HANDTOOL,//#606
	tc_SHOW_LINEDIALOG,//#594
	tc_AUTOINDENTMODE,//#638
	tc_DEBUG_CHECKLINEINFO,//#699
	tc_FIND_TEXT_NEXT,//#887
	tc_FIND_TEXT_PREV,//#887
	tc_FIND_TEXT_FROMFIRST,//#887
	tc_REPLACE_TEXT_NEXT,//#887
	tc_REPLACE_TEXT_INSELECTION,//#887
	tc_REPLACE_TEXT_AFTERCARET,//#887
	tc_REPLACE_TEXT_ALL,//#887
	tc_REPLACE_TEXT_SELECTEDTEXTONLY,//#887
	tc_PREVIEW_EXECJS_SELECTED,//#734
	tc_DELETE_STARTOFLINE,//#913
	tc_DELETE_STARTOFPARAGRAPH,//#913
	tc_SELECT_STARTOFPARAGRAPH,//#913
	tc_SELECT_ENDOFPARAGRAPH,//#913
	tc_SETMARK,//#913
	tc_SWAPMARK,//#913
	tc_SELECT_MARK,//#913
	tc_DELETE_MARK,//#913
	tc_DELETE_PREV_DECOMP,//#913
	tc_YANK,//#913
	tc_SELECT_WORD,//#913
	tc_MACRO1,//#724
	tc_HINT,
	tc_FIND_EXTRACT,
	tc_BROWSE_PANDOC,//#1286
	tc_COUNT_OF_TOOLCOMMAND
};

enum AToolbarItemType
{
	kToolbarItemType_File = 0,
	kToolbarItemType_AddToolButton,
	kToolbarItemType_ModePref,
	kToolbarItemType_TextEncoding,
	kToolbarItemType_ReturnCode,
	kToolbarItemType_WrapMode,
	kToolbarItemType_Mode,
	kToolbarItemType_Folder,
	kToolbarItemType_MacroTitle,
};

//ABaseTypes.hへ移動#define Macro_IsAlphabet(ch) (( (ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_' ))

enum AScopeType
{
	kScopeType_Local = 0,
	kScopeType_Global,
	kScopeType_Import,
	kScopeType_Import_Near,
	kScopeType_ModeSystemHeader,
	kScopeType_ModeKeyword,
};

//#212
/**
サブペイン表示モード
*/
/*#899
enum ASubPaneMode
{
	kSubPaneMode_Manual = 0,
	kSubPaneMode_SameDocument,
	kSubPaneMode_PreviousDocument
};
*/

//#291 #725
/**
情報ペイン種類
*/
enum ASubWindowType
{
	kSubWindowType_None = -1,
	kSubWindowType_JumpList = 0,
	kSubWindowType_IdInfo = 1,
	kSubWindowType_FileList = 2,
	kSubWindowType_SubTextPane = 3,
	kSubWindowType_CandidateList = 4,
	kSubWindowType_CallGraf = 5,
	kSubWindowType_Previewer = 6,
	kSubWindowType_TextMarker = 7,
	kSubWindowType_ClipboardHistory = 8,
	kSubWindowType_DocInfo = 9,
	kSubWindowType_KeyToolList = 10,
	kSubWindowType_WordsList = 11,
	kSubWindowType_FindResult = 12,
	kSubWindowType_KeyRecord = 13,
};//追加時は最後に追加していくこと。AppPrefに番号が保存されるため。

//#725
/**
*/
enum ASubWindowLocationType
{
	kSubWindowLocationType_None = -1,
	kSubWindowLocationType_LeftSideBar = 0,
	kSubWindowLocationType_RightSideBar,
	kSubWindowLocationType_Floating,
	kSubWindowLocationType_Popup,
	kSubWindowLocationType_PopupMouseOver,
};

//#450
/**
おりたたみレベル
*/
typedef short int AFoldingLevel;
const AFoldingLevel	kFoldingLevel_None = 0;
const AFoldingLevel	kFoldingLevel_Header 			= 1;
const AFoldingLevel	kFoldingLevel_BlockStart 		= 2;
const AFoldingLevel	kFoldingLevel_BlockEnd 			= 4;
const AFoldingLevel	kFoldingLevel_CommentStart 		= 8;
const AFoldingLevel	kFoldingLevel_CommentEnd 		= 16;
const AFoldingLevel	kFoldingLevel_IfStart 			= 32;
const AFoldingLevel	kFoldingLevel_IfEnd 			= 64;

//#650
enum AIndentTriggerType
{
	kIndentTriggerType_IndentFeature = 0,
	kIndentTriggerType_IndentKey,
	kIndentTriggerType_ReturnKey_CurrentLine,
	kIndentTriggerType_ReturnKey_NewLine,
	kIndentTriggerType_AutoIndentCharacter
};

//#645
enum AUpdateViewBoundsTriggerType
{
	kUpdateViewBoundsTriggerType_Other = 0,
	kUpdateViewBoundsTriggerType_InfoPaneHSeparator,
	kUpdateViewBoundsTriggerType_InfoPaneVSeparator,
	kUpdateViewBoundsTriggerType_InfoPaneVSeparator_Completed,
	kUpdateViewBoundsTriggerType_SubPaneHSeparator,
	kUpdateViewBoundsTriggerType_ShowLeftSideBar,
	kUpdateViewBoundsTriggerType_HideLeftSideBar,
	kUpdateViewBoundsTriggerType_WindowBoudnsChanged,
	kUpdateViewBoundsTriggerType_TextViewHSeparator,
	kUpdateViewBoundsTriggerType_SubTextColumnVSeparator,
};

//#853
typedef ANumber AKeywordSearchOption;
const AKeywordSearchOption kKeywordSearchOption_None = 0;
const AKeywordSearchOption kKeywordSearchOption_Leading = 1;
const AKeywordSearchOption kKeywordSearchOption_Partial = 2;
const AKeywordSearchOption kKeywordSearchOption_IgnoreCases = 4;
const AKeywordSearchOption kKeywordSearchOption_IncludeInfoText = 8;
const AKeywordSearchOption kKeywordSearchOption_CompareParentKeyword = 16;
const AKeywordSearchOption kKeywordSearchOption_InhibitDuplicatedKeyword = 32;
const AKeywordSearchOption kKeywordSearchOption_OnlyEnabledCategoryForCurrentState = 64;

//#364
/**
ScrapType
*/
#if IMPLEMENTATION_FOR_MACOSX
const AScrapType	kScrapType_TabSelector = 'TabS';
const AScrapType	kScrapType_SubWindowTitlebar = 'SubW';
const AScrapType	kScrapType_CallGrafItem = 'CGIt';
const AScrapType	kScrapType_IdInfoItem = 'IdIt';
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AScrapType	kScrapType_TabSelector = CF_PRIVATEFIRST+0x40;
const AScrapType	kScrapType_SubWindowTitlebar = CF_PRIVATEFIRST+0x41;
const AScrapType	kScrapType_CallGrafItem = CF_PRIVATEFIRST+0x42;
const AScrapType	kScrapType_IdInfoItem = CF_PRIVATEFIRST+0x43;
//+0x3FまでをAbsFrameに割り当て。+0x40～をアプリに割り当て
#endif

//#698
enum ATimedExecutionType
{
	kTimedExecutionType_TransitNextDocumentToWrapCalculatingState
};

/**
AdjustScrollタイプ
*/
enum AAdjustScrollType
{
	kAdjustScrollType_Top = 0,
	kAdjustScrollType_Center,
};

/**
インデント文字タイプ
*/
enum AIndentCharacterType
{
	kIndentCharacterType_Unknown = 0,
	kIndentCharacterType_Tab,
	kIndentCharacterType_2Spaces,
	kIndentCharacterType_4Spaces,
	kIndentCharacterType_6Spaces,
	kIndentCharacterType_8Spaces,
};

/**
テキストロードタイプ
*/
enum ALoadTextPurposeType
{
	kLoadTextPurposeType_LoadDocument,
	kLoadTextPurposeType_LoadDocument_ForMultiFileReplace,
	kLoadTextPurposeType_CreateDocumentFromTemplateFile,
	kLoadTextPurposeType_ScreenModeExecutableItem,
	kLoadTextPurposeType_TexCompileResult,
	kLoadTextPurposeType_ImportFileRecognize,
	kLoadTextPurposeType_ToolOrderFile,
	kLoadTextPurposeType_WordsListFinder,
	kLoadTextPurposeType_Tool,
	kLoadTextPurposeType_ConcatAllTool,
	kLoadTextPurposeType_ImportKeybind,
	kLoadTextPurposeType_ImportDragDrop,
	kLoadTextPurposeType_ImportCorrespond,
	kLoadTextPurposeType_ImportRegExpImport,
	kLoadTextPurposeType_MultiFileFinder_Find,
	kLoadTextPurposeType_MultiFileFinder_Replace,
	kLoadTextPurposeType_KeywordCSVFile,
	kLoadTextPurposeType_BatchReplace,
	kLoadTextPurposeType_ASIndexDoc,
	kLoadTextPurposeType_UnitTest,
};

//ヒントテキストで実際に挿入する文字
const AUChar	kHintTextChar = '?';

//=====バッファサイズ=====

//#733
//検索文字列バッファの最大文字長
const AItemCount	kBufferSize_TextFinder_UCS4FindText = 4096;

//==================各種制限値 #695==================

//開けるファイルサイズの最大
//100MB
const AByteCount	kLimit_OpenTextFileSize = 100*1024*1024;

//開けるファイルサイズ合計の最大
const AByteCount	kLimit_TotalOpenTextFileSize = 200*1024*1024;

//各種検索文字の最長（UTF-8でのバイト数）
//kBufferSize_TextFinder_UCS4FindTextに由来する制限。
//UTF8で2048バイトなのでUCS4では2048文字未満。ただし、decomp化すると、少し増える。
//（4096文字を超えないことを保証することはできない）
const AItemCount	kLimit_FindTextLength = 2048;

//「単語」の最大長
const AItemCount	kLimit_WordLength = 4096;

//ハイライト最大長
const AItemCount	kLimit_HighlightTargetTextLength = 4096;

//AImportFileRecognizer::NVIDO_ThreadMain()にて、Importファイルで文法認識を行う最大のファイルデータサイズ
//同じプロジェクト内の対象ファイルに大きすぎるファイルがあると、その文法解析のみにAImportFileRecognizerの処理を費やされてしまうので、
//下記の値以上のファイルは文法認識対象外とする。
//制限値：10MB
const AItemCount	kLimit_ImportFileRecognizer_MaxByteCountToRecognize = 10*1024*1024;

//AWindow_Text::EVTDO_DoServiceCopyにて、ServiceCopyイベントに対し、テキストを返す最大の行数
//テキストが大きすぎると、メニュー選択時等にスローオペレーションになるので、この行数以上の場合は、空のテキストを返す。
//制限値：10万行
const AItemCount	kLimit_ServiceCopy_MaxLineCount = 100000;

//ADocument_Text::AddToJumpMenu()にて、ジャンプメニューに追加するメニュー項目の最大数
//制限値：500項目
const AItemCount	kLimit_MaxJumpMenuItemCount = 500;

//同じフォルダメニュー最大数
const AItemCount	kLimit_SameFolderMenuItemCount = 500;

//関連するファイルメニュー最大数
const AItemCount	kLimit_ImportFileMenuItemCount = 500;

//ADocument_Text::SPI_GetTextCount()にて、テキストカウント時に文法認識実行する最大行数
//制限値：10万行
const AItemCount	kLimit_TextCount_MaxLineCountForSyntaxRecognize = 100000;

//ATextInfo::ExecuteRecognizeSyntax()にて、認識完了行でも状態一致の条件を満たすまで認識を継続する行数のmax値。
//下記の行数より多く認識済み行を認識したら、文法認識は条件にかかわらず終了する。
//制限値：1000行
const AItemCount	kLimit_ExecuteRecognizeSyntax_MaxLineCountAfterRecognizedFlagTrue = 1000;

//ATextInfo::GetTextDrawDataWithoutStyle()にて、一行に表示する文字のバイト数最大
//バイト数でkLimit_Max1LineDrawByteCounts以上となる最初の文字までで取得打ちきりとなる
//制限値：10000バイト
const AItemCount	kLimit_Max1LineDrawByteCounts = 10000;

//ATextInfo::PrepareRecognizeSyntaxContext()にて、文法認識を開始する行を決定するときに、戻る最大の行数
//状態がstableである行を検索して戻るが、下記行数より多くは戻らない
//制限値：64行
const AItemCount	kLimit_RecognizeSyntax_MinLineCountToGoBackRecognizeStart = 8;
const AItemCount	kLimit_RecognizeSyntax_MaxLineCountToGoBackRecognizeStart = 64;

//Global識別子の最大数（ATextInfo::AddLineGlobalIdentifier()）
//制限値：50000個
const AItemCount	kLimit_MaxGlobalIdentifiers = 50000;

//Local識別子の最大数（ATextInfo::AddLineLocalIdentifier()）
//制限値：50000個
const AItemCount	kLimit_MaxLocalIdentifiers = 50000;

//BlockStartDataの最大数（ATextInfo::AddBlockStartData()）
//制限値：500000個//★
const AItemCount	kLimit_MaxBlockStartData = 500000;

//ADocument_Text::SPI_CalcIndentCount()で、SyntaxDefinitionインデントの場合に、戻る行数の最大
//制限値：32行//★
const AItemCount	kLimit_SyntaxDefinitionIndent_MaxLineCountToGoBack = 32;

//AView_Text::EVTDO_DoDraw()にて、特殊文字（タブ等）描画に対応する行内posの最大値
//この値以降のx位置は、特殊文字描画は行わない
//制限値：10000バイト
const AItemCount	kLimit_TextViewDoDraw_MaxXPosToDrawSpecialCharacters = 10000;

//AView_Text::EVTDO_DoInlineInputGetSelectedText()にて、OSヘ返すテキストの最大長（LimitLength()によるUTF8文字位置補正あり）
//制限値：10000バイト
const AItemCount	kLimit_InlineInputGetSelectedText_MaxByteCount = 10000;

//ADocument_Text::SPI_GetBraceSelection()にて、引数inLimitSearchRangeにtrueを指定した場合の検索範囲
//制限値：100行
const AItemCount	kLimit_BraseSelectionSearchRange = 100;

//AIdentifierList::SearchKeyword()にて、キーワード検索の結果最大個数
const AItemCount	kLimit_KeywordSearchResultCount = 1024;

//補完候補リストの最大個数
const AItemCount	kLimit_CandidateListItemCount = 200;

//コールグラフの履歴最大個数
const AItemCount	kLimit_CallGrafLevel0ItemCount = 128;

//AView_Text::EVTDO_DoDrawにて、ハイライト検索を行う範囲（段落最初からのバイト数）
//制限値：10000バイト
const AItemCount	kLimit_TextHighlightFindRange = 10000;

//AView_IdInfo::SPI_SetIdInfo()にて、IdInfoのinfo textの最大表示バイト数
//制限値：800バイト
const AItemCount	kLimit_IdInfo_InfoTextLength = 800;

//検索結果でのグループ最大個数
const AItemCount	kLimit_IndexGroupMax = 16;

//ワードリストの結果表示最大個数
const AItemCount	kLimit_WordsListResultCount = 1000;

//補完候補履歴の記憶最大数
const AItemCount	kLimit_MaxRecentCompletionWordArray = 256;

//マクロタイトル最大長
const AItemCount	kLimit_MacroTitleLength = 15;

//各種閾値 #699

//ADocument_Text::AdjustLineInfo()にて、メインスレッドで直接ではなくスレッドで行計算する行数。
//下記の値以上の行数が追加されたとき、スレッドで行計算する。
//閾値：20000行
const AItemCount	kThreashold_LineCountToUseWrapCalculatorThread = 20000;

//ADocument_Text::StartSyntaxRecognizerByEdit()にて、メインスレッドで直接ではなくスレッドで文法認識する行数。
//スレッド未動作中、下記の値以上の行数が追加されたとき、スレッドで文法認識する。
//閾値：1行
//常にスレッドで認識するようにするconst AItemCount	kThreashold_LineCountToUseSyntaxRecognizerThreadByEdit = 1;

//AApp::SPI_CheckContinueingEditProgressModalSession()にて、progressウインドウを最初に表示するcount数
//閾値：500回
const AItemCount	kThreshold_EditProgressContinueCount = 500;

//各種設定値 #699

//ADocument_Text::SPI_Init_TransitToWrapCalculatingState()にて、メインスレッド内で行計算する行数
//設定値：20000行
const AItemCount	kMainThreadWrapCalculateLineCount = 20000;

//行計算スレッドが一度に行計算する行数
//この行数分だけ完了したら、メインスレッドへ通知して、このスレッドはpause状態になる。
const AItemCount	kWrapCalculatorThreadPauseLineCount = 10000;

//文法認識スレッドで一度に文法認識する行数
//この行数分だけ完了したら、メインスレッドへ通知して、このスレッドはpause状態になる。
const AItemCount	kSyntaxRecognizerThreadPauseLineCount = 10000;

//==================動作オプション==================
const ABool	kOption_CallGraf_DontUpdateHistoryWhenItemSelected = true;
const ABool	kOption_CallGraf_FixFirstHistoryItem = false;

//==================動作データ==================

//AdjustScroll時の、余分にスクロールする行数（この行数分だけ次にスクロール調整が発生するまでの行数の余裕がある） #939
const AItemCount	kAdjustScrollOffsetDefault = 5;

//==================描画データ==================

//候補リスト下部ステータスバー高さ
const ANumber	kHeight_CandidateListStatusBar = 18;

//候補リストフレーム
const AFloatNumber	kRadius_PopupCandidateList = 3.0;//#1079 5.0→3.0
const AFloatNumber	kLineWidth_PopupCandidateList = 0.5;

//ツールリストフレーム
const AFloatNumber	kRadius_PopupKeyToolList = 3.0;//#1079 5.0→3.0
const AFloatNumber	kLineWidth_PopupKeyToolList = 0.5;

