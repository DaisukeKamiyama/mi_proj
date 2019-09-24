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

AWindow_Find

*/

#include "stdafx.h"

#include "AWindow_Find.h"
#include "AApp.h"
//#include "AUtil.h"
#include "AView_Text.h"

const AControlID		kPopup_RecentlyUsedFindText		= 101;
const AControlID		kText_FindText		 			= 102;
const AControlID		kButton_FindList	 			= 103;
const AControlID		kButton_FindFromFirst 			= 104;
const AControlID		kButton_FindPrevious		 	= 105;
const AControlID		kButton_FindNext		 		= 106;
const AControlID		kPopup_RecentlyUsedReplaceText	= 107;
const AControlID		kText_ReplaceText				= 108;
const AControlID		kRadio_ReplaceNext				= 109;
const AControlID		kRadio_ReplaceAfterCaret		= 110;
const AControlID		kRadio_ReplaceInSelection		= 111;
const AControlID		kRadio_ReplaceAll	 			= 112;
const AControlID		kButton_ReplaceAndFind			= 113;
const AControlID		kButton_Replace	 				= 114;
const AControlID		kCheckBox_IgnoreCase 			= 115;
const AControlID		kCheckBox_WholeWord 			= 116;
const AControlID		kCheckBox_Aimai 				= 117;
const AControlID		kCheckBox_BackslashYen 			= 118;
const AControlID		kCheckBox_RegExp 				= 119;
//#872 const AControlID		kCheckBox_AllowReturnTab 		= 120;
const AControlID		kCheckBox_Loop					= 121;
const AControlID		kButton_FindMemory				= 122;
const AControlID		kCheckBox_IgnoreSpaces			= 123;//#165
const AControlID		kButton_SetupFuzzySearch		= 124;//#166
const AControlID		kText_IncrementalNumReplaceStart= 125;//#872
const AControlID		kButton_IncrementalNumReplace	= 126;//#872
const AControlID		kStaticText_BottomMarker		= 129;//#872
const AControlID		kTriangle_FindOptions			= 130;//#872
const AControlID		kTriangle_Replace				= 131;//#872
//drop const AControlID		kTriangle_AddToMacro			= 132;//#872
const AControlID		kTriangle_FindMemory			= 133;//#872
const AControlID		kTriangleText_FindOptions		= 134;//#872
const AControlID		kTriangleText_Replace			= 135;//#872
const AControlID		kTriangleText_AdvancedReplace	= 136;//#872
const AControlID		kTriangleText_FindMemory		= 137;//#872
const AControlID		kPopup_RegExpList				= 138;
const AControlID		kTriangle_AdvancedSearch		= 139;//#872
const AControlID		kTriangleText_AdvancedSearch	= 140;//#872

//
//drop const AControlID		kButton_AddMacro_Find			= 301;
const AControlID		kButton_AddMacro_Replace		= 302;
const AControlID		kButton_BatchReplace			= 303;
const AControlID		kButton_ExtractMatchedText		= 304;

const AControlID		kButton_FindMemoryAdd			= 201;
const AControlID		kButton_FindMemoryRemove		= 202;
/*#872
const AControlID		kButton_FindMemoryUp			= 203;
const AControlID		kButton_FindMemoryDown			= 204;
const AControlID		kButton_FindMemoryTop			= 205;
const AControlID		kButton_FindMemoryBottom		= 206;
*/
const AControlID		kTable_FindMemory				= 207;
const AControlID		kButton_FindMemoryRemoveOK		= 208;
//#872 const AControlID		kButton_FindMemoryClose			= 209;


//======================�e�[�u����======================
const ADataID			kColumn_Index					= 0;
const ADataID			kColumn_Name					= 1;
const ADataID			kColumn_FindText				= 2;
const ADataID			kColumn_ReplaceText				= 3;
const ADataID			kColumn_IgnoreCase				= 4;
const ADataID			kColumn_WholeWord				= 5;
const ADataID			kColumn_Aimai					= 6;
const ADataID			kColumn_RegExp					= 7;
const ADataID			kColumn_Loop					= 8;



#pragma mark ===========================
#pragma mark [�N���X]AWindow_Find
#pragma mark ===========================
//�����E�C���h�E

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

//�R���X�g���N�^
AWindow_Find::AWindow_Find():AWindow(/*#175NULL*/)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kFindWindowPosition);
	//#175
	NVI_AddToRotateArray();
	//#135
	NVI_AddToTimerActionWindowArray();
}

//�f�X�g���N�^
AWindow_Find::~AWindow_Find()
{
	/*#92 NVIDO_DoDeleted() �ֈړ�
	*#199 mFindMemoryWindow*SPI_GetFindMemoryWindow().NVI_Close();
	GetApp().NVI_DeleteWindow(mFindMemoryWindowID);
	*/
}

//#92
/**
*/
void	AWindow_Find::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//�f�[�^�x�[�X�擾�iAPrefWindow����R�[�������j
ADataBase&	AWindow_Find::NVMDO_GetDB()
{
	return GetApp().GetAppPref();
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//
ABool	AWindow_Find::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	//#701 �����E�C���h�E�������J�����Ƃ��ɓ������z�f�X�N�g�b�v���ɃE�C���h�E�\�������悤��Hide�ł͂Ȃ���UClose����
	if( inMenuItemID == kMenuItemID_Close )
	{
		NVI_Close();
		return true;
	}
	//
	return Execute(inMenuItemID);
}

//
void	AWindow_Find::EVTDO_UpdateMenu()
{
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		AText	findText;
		//#135 NVI_GetControlText(kText_FindText,findText);
		NVI_GetControlText(kText_FindText,findText);//#135
		if( findText.GetItemCount() > 0 )
		{
			GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindNext,true);
			GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindPrevious,true);
			GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindFromFirst,true);
			GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindList,true);
			if( GetApp().NVI_GetDocumentByID(docID).NVI_IsReadOnly() == false )
			{
				GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceNext,true);
				GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceAfterCaret,true);
				GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceAll,true);
				AWindowID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
				if( winID != kObjectID_Invalid )
				{
					if( GetApp().SPI_GetTextWindowByID(winID).SPI_IsCaretMode() == false )
					{
						GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceSelectedTextOnly,true);
						GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceAndFind,true);
						GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceInSelection,true);
					}
				}
			}
		}
	}
}

//
ABool	AWindow_Find::EVTDO_ValueChanged( const AControlID inControlID )
{
	//���݂�UI�̓��e��AApp���̃f�[�^�֔��f #693
	UpdateData_CurrentFindParam();
	//
	ABool	result = false;
	switch(inControlID)
	{
	  case kPopup_RecentlyUsedFindText:
		{
			AIndex	index = NVI_GetControlNumber(kPopup_RecentlyUsedFindText);
			if( index-1 >= mRecentlyUsedFindTextArray.GetItemCount() )//#901 GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kFindStringCache) )
			{
				//���j���[������
				GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kFindStringCache);
				MakeRecentlyUsedFindTextMenu();//win
			}
			else if( index > 0 )//B0000 �u�ŋߌ�������������v�͌���������ɐݒ肵�Ȃ�
			{
				//����������ɃR�s�[
				AText	text;
				NVI_GetControlText(kPopup_RecentlyUsedFindText,text);
				//#135 NVI_SetControlText(kText_FindText,text);
				NVI_SetControlText(kText_FindText,text);//#135
			}
			NVI_SetControlNumber(kPopup_RecentlyUsedFindText,0);
			result = true;
			break;
		}
	  case kPopup_RecentlyUsedReplaceText:
		{
			AIndex	index = NVI_GetControlNumber(kPopup_RecentlyUsedReplaceText);
			if( index-1 >= mRecentlyUsedReplaceTextArray.GetItemCount() )//#901 GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kReplaceStringCache) )
			{
				//���j���[������
				GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kReplaceStringCache);
				MakeRecentlyUsedReplaceTextMenu();//win
			}
			else if( index > 0 )//B0000 �u�ŋߌ�������������v�͌���������ɐݒ肵�Ȃ�
			{
				//����������ɃR�s�[
				AText	text;
				NVI_GetControlText(kPopup_RecentlyUsedReplaceText,text);
				//#135 NVI_SetControlText(kText_ReplaceText,text);
				NVI_SetControlText(kText_ReplaceText,text);
			}
			NVI_SetControlNumber(kPopup_RecentlyUsedReplaceText,0);
			result = true;
			break;
		}
		//���K�\���T���v�����X�g
	  case kPopup_RegExpList:
		{
			//�I�����j���[����index�擾
			AIndex	index = NVI_GetControlNumber(kPopup_RegExpList);
			//���j���[���ڂɑΉ����鐳�K�\���e�L�X�g�擾
			AText	regexp;
			GetApp().SPI_GetRegExpListItem(index,regexp);
			//����������ɐݒ�
			NVI_GetEditBoxView(kText_FindText).SPI_DeleteAndInsertText(regexp);
			//���j���[���ڂ̑I����0�Ԗڂɖ߂�
			NVI_SetControlNumber(kPopup_RegExpList,0);
			//���K�\���`�F�b�N�{�b�N�XON
			NVI_SetControlBool(kCheckBox_RegExp,true);
			NVM_UpdateControlStatus();
			result = true;
			break;
		}
	}
	return result;
}

//
ABool	AWindow_Find::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	//���݂�UI�̓��e��AApp���̃f�[�^�֔��f #693
	UpdateData_CurrentFindParam();
	//
	ABool	result = false;
	/*#872
	switch(inID)
	{
	  case kButton_FindMemory:
		{
			if( SPI_GetFindMemoryWindow().NVI_IsWindowVisible() == true )
			{
				SPI_GetFindMemoryWindow().NVI_Hide();
			}
			else
			{
				SPI_GetFindMemoryWindow().NVI_Show();
			}
			NVI_SelectWindow();
			result = true;
			break;
		}
	}
	if( result == true )   return true;
	*/
	
	switch(inID)
	{
	  case kButton_FindNext:
		{
			result = Execute(kMenuItemID_FindNext);
			break;
		}
	  case kButton_FindPrevious:
		{
			result = Execute(kMenuItemID_FindPrevious);
			break;
		}
	  case kButton_FindFromFirst:
		{
			result = Execute(kMenuItemID_FindFromFirst);
			break;
		}
	  case kButton_FindList:
		{
			result = Execute(kMenuItemID_FindList);
			break;
		}
		//#937
		//��v�e�L�X�g���o
	  case kButton_ExtractMatchedText:
		{
			result = Execute(kMenuItemID_FindExtractMatchedText);
			break;
		}
		//
	  case kButton_Replace:
		{
			if( NVI_GetControlBool(kRadio_ReplaceNext) == true )
			{
				result = Execute(kMenuItemID_ReplaceNext);
			}
			else if( NVI_GetControlBool(kRadio_ReplaceAfterCaret) == true )
			{
				result = Execute(kMenuItemID_ReplaceAfterCaret);
			}
			else if( NVI_GetControlBool(kRadio_ReplaceInSelection) == true )
			{
				result = Execute(kMenuItemID_ReplaceInSelection);
			}
			else if( NVI_GetControlBool(kRadio_ReplaceAll) == true )
			{
				result = Execute(kMenuItemID_ReplaceAll);
			}
			break;
		}
	  case kButton_ReplaceAndFind:
		{
			result = Execute(kMenuItemID_ReplaceAndFind);
			break;
		}
		//#166
	  case kButton_SetupFuzzySearch:
		{
			GetApp().SPI_GetAppPrefWindow().NVI_CreateAndShow();
			GetApp().SPI_GetAppPrefWindow().NVI_SelectTabControl(3);
			GetApp().SPI_GetAppPrefWindow().NVI_RefreshWindow();
			GetApp().SPI_GetAppPrefWindow().NVI_SwitchFocusToFirst();
			break;
		}
		//#872
	  case kTriangle_FindOptions:
	  case kTriangle_Replace:
	  //drop case kTriangle_AddToMacro:
	  case kTriangle_FindMemory:
	  case kTriangle_AdvancedSearch:
		{
			NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			break;
		}
	  case kTriangleText_FindOptions:
		{
			NVI_SetControlBool(kTriangle_FindOptions,!(NVI_GetControlBool(kTriangle_FindOptions)));
			NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			break;
		}
	  case kTriangleText_Replace:
		{
			NVI_SetControlBool(kTriangle_Replace,!(NVI_GetControlBool(kTriangle_Replace)));
			NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			break;
		}
	  case kTriangleText_FindMemory:
		{
			NVI_SetControlBool(kTriangle_FindMemory,!(NVI_GetControlBool(kTriangle_FindMemory)));
			NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			break;
		}
	  case kTriangleText_AdvancedSearch:
		{
			NVI_SetControlBool(kTriangle_AdvancedSearch,!(NVI_GetControlBool(kTriangle_AdvancedSearch)));
			NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			break;
		}
		/*drop
	  case kTriangleText_AdvancedReplace:
		{
			NVI_SetControlBool(kTriangle_AddToMacro,!(NVI_GetControlBool(kTriangle_AddToMacro)));
			NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
			break;
		}
		//#887
		//drop
	  case kButton_AddMacro_Find:
		{
			AObjectID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID == kObjectID_Invalid )   break;
			
			//�����p�c�[���R�}���h�擾
			AText	text;
			GetApp().SPI_GetFindParamToolCommand(text);
			
			//�}�N���^�C�g������
			AFindParameter	findParam;
			GetApp().SPI_GetFindParam(findParam);
			AText	title;
			ADocumentID	docID = GetApp().SPI_GetTextWindowByID(winID).NVI_GetCurrentDocumentID();
			AIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex();
			title.SetLocalizedText("MacroDefaultName_Find");
			title.AddCstring("\"");
			title.AddText(findParam.findText);
			title.AddCstring("\"");
			GetApp().SPI_GetModePrefDB(modeIndex).GetUniqToolbarName(title);
			
			//�}�N���ǉ��E�C���h�E�\��
			GetApp().SPI_GetTextWindowByID(winID).SPI_ShowAddToolButtonWindow(title,text);
			break;
		}
		*/
		//#887
	  case kButton_AddMacro_Replace:
		{
			AObjectID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID == kObjectID_Invalid )   break;
			
			AFindParameter	findParam;
			GetApp().SPI_GetFindParam(findParam);
			
			//�u���p�c�[���R�}���h�擾
			AText	text;
			if( NVI_GetControlBool(kRadio_ReplaceNext) == true )
			{
				GetApp().SPI_GetReplaceParamToolCommand(findParam,kReplaceRangeType_Next,text);
			}
			else if( NVI_GetControlBool(kRadio_ReplaceAfterCaret) == true )
			{
				GetApp().SPI_GetReplaceParamToolCommand(findParam,kReplaceRangeType_AfterCaret,text);
			}
			else if( NVI_GetControlBool(kRadio_ReplaceInSelection) == true )
			{
				GetApp().SPI_GetReplaceParamToolCommand(findParam,kReplaceRangeType_InSelection,text);
			}
			else if( NVI_GetControlBool(kRadio_ReplaceAll) == true )
			{
				GetApp().SPI_GetReplaceParamToolCommand(findParam,kReplaceRangeType_All,text);
			}
			
			//�}�N���^�C�g������
			AText	title;
			ADocumentID	docID = GetApp().SPI_GetTextWindowByID(winID).NVI_GetCurrentDocumentID();
			AIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex();
			title.SetLocalizedText("MacroDefaultName_Replace");
			title.AddCstring("\"");
			title.AddText(findParam.findText);
			title.AddCstring("->");
			title.AddText(findParam.replaceText);
			title.AddCstring("\"");
			GetApp().SPI_GetModePrefDB(modeIndex).GetUniqToolbarName(title);
			
			//�}�N���ǉ��E�C���h�E�\��
			GetApp().SPI_GetTextWindowByID(winID).SPI_ShowAddToolButtonWindow(title,text,false);
			break;
		}
		//#935
		//�ꊇ�u��
	  case kButton_BatchReplace:
		{
			//�t�@�C���I���_�C�A���O�\��
			AFileAcc	defaultFile;
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_BatchReplace");
			NVI_ShowChooseFileWindow(defaultFile,message,kButton_BatchReplace);
			break;
		}
	}
	return result;
}

//B0317
ABool	AWindow_Find::CheckRegExp( const AFindParameter& inFindParam )
{
	if( inFindParam.regExp == true )
	{
		ARegExp	regexpcheck;
		if( regexpcheck.SetRE(inFindParam.findText) == false )
		{
			AText	message1, message2;
			message1.SetLocalizedText("RegExpError1");
			message2.SetLocalizedText("RegExpError2");
			message2.ReplaceParamText('0',inFindParam.findText);
			NVI_SelectWindow();
			NVI_ShowChildWindow_OK(message1,message2);
			return false;
		}
	}
	return true;
}

ABool	AWindow_Find::Execute( const AMenuItemID inMenuItemID )
{
	//���݂�UI�̓��e��AApp���̃f�[�^�֔��f #693
	//�i�����͕s�v��������Ȃ����A�O�̂��߁B�j
	UpdateData_CurrentFindParam();
	//
	AObjectID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
	if( winID == kObjectID_Invalid )   return false;
	
	ABool	result = false;
	AFindParameter	findParam;
	GetApp().SPI_GetFindParam(findParam);
	switch(inMenuItemID)
	{
	  case kMenuItemID_FindNext:
		{
			if( CheckRegExp(findParam) == false )   break;//B0317
			if( GetApp().SPI_GetTextWindowByID(winID).SPI_FindNext(findParam) == true )
			{
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectTextWindowAfterFind) == true )//#269
				{
					GetApp().SPI_GetTextWindowByID(winID).NVI_SelectWindow();
				}
			}
			result = true;
			break;
		}
	  case kMenuItemID_FindPrevious:
		{
			if( CheckRegExp(findParam) == false )   break;//B0317
			if( GetApp().SPI_GetTextWindowByID(winID).SPI_FindPrevious(findParam) == true )
			{
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectTextWindowAfterFind) == true )//#269
				{
					GetApp().SPI_GetTextWindowByID(winID).NVI_SelectWindow();
				}
			}
			result = true;
			break;
		}
	  case kMenuItemID_FindFromFirst:
		{
			if( CheckRegExp(findParam) == false )   break;//B0317
			if( GetApp().SPI_GetTextWindowByID(winID).SPI_FindFromFirst(findParam) == true )
			{
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectTextWindowAfterFind) == true )//#269
				{
					GetApp().SPI_GetTextWindowByID(winID).NVI_SelectWindow();
				}
			}
			result = true;
			break;
		}
	  case kMenuItemID_FindList:
		{
			if( CheckRegExp(findParam) == false )   break;//B0317
			GetApp().SPI_GetTextWindowByID(winID).SPI_FindList(findParam,false);
			result = true;
			break;
		}
		//#937
		//��v�e�L�X�g���o
	  case kMenuItemID_FindExtractMatchedText:
		{
			if( CheckRegExp(findParam) == false )   break;//B0317
			GetApp().SPI_GetTextWindowByID(winID).SPI_FindList(findParam,true);
			result = true;
			break;
		}
		//
	  case kMenuItemID_ReplaceNext:
		{
			if( CheckRegExp(findParam) == false )   break;//B0317
			if( GetApp().SPI_GetTextWindowByID(winID).SPI_ReplaceNext(findParam) == true )
			{
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectTextWindowAfterFind) == true )//#269
				{
					GetApp().SPI_GetTextWindowByID(winID).NVI_SelectWindow();
				}
			}
			result = true;
			break;
		}
	  case kMenuItemID_ReplaceAfterCaret:
		{
			if( CheckRegExp(findParam) == false )   break;//B0317
			//�L�����b�g�ȍ~�u�����s
			GetApp().SPI_GetTextWindowByID(winID).SPI_ReplaceAfterCaret(findParam);
			//#725 SPI_ReplaceAfterCaret()���Ō��ʕ\������ SPI_ShowReplaceResultSheet(num);
			result = true;
			break;
		}
	  case kMenuItemID_ReplaceInSelection:
		{
			if( CheckRegExp(findParam) == false )   break;//B0317
			//�I����u�����s
			GetApp().SPI_GetTextWindowByID(winID).SPI_ReplaceInSelection(findParam);
			//#725 SPI_ReplaceAfterCaret()���Ō��ʕ\������ SPI_ShowReplaceResultSheet(num);
			result = true;
			break;
		}
	  case kMenuItemID_ReplaceAll:
		{
			if( CheckRegExp(findParam) == false )   break;//B0317
			//�S�u�����s
			GetApp().SPI_GetTextWindowByID(winID).SPI_ReplaceAll(findParam);
			//#725 SPI_ReplaceAfterCaret()���Ō��ʕ\������ SPI_ShowReplaceResultSheet(num);
			result = true;
			break;
		}
	  case kMenuItemID_ReplaceAndFind:
		{
			if( CheckRegExp(findParam) == false )   break;//B0317
			if( GetApp().SPI_GetTextWindowByID(winID).SPI_ReplaceAndFind(findParam) == true )
			{
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectTextWindowAfterFind) == true )//#269
				{
					GetApp().SPI_GetTextWindowByID(winID).NVI_SelectWindow();
				}
			}
			result = true;
			break;
		}
	}
	return result;
}

void	AWindow_Find::EVTDO_TextInputted( const AControlID inID )
{
	NVM_UpdateControlStatus();
}

//WindowActivated���̃R�[���o�b�N
void	AWindow_Find::EVTDO_WindowActivated()
{
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID != kObjectID_Invalid )
	{
		NVI_SetInputBackslashByYenKeyMode(GetApp().SPI_GetModePrefDB(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex()).
					GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey));
	}
	else
	{
		NVI_SetInputBackslashByYenKeyMode(false);
	}
}

//WindowDeactivated���̃R�[���o�b�N
void	AWindow_Find::EVTDO_WindowDeactivated()
{
	/*if( mFindMemoryWindow.IsShown() == true )
	{
		mFindMemoryWindow.Hide();
	}*/
}

//#701
/**
�N���[�Y�{�^���N���b�N������
*/
void	AWindow_Find::EVTDO_DoCloseButton()
{
	//�����E�C���h�E�������J�����Ƃ��ɓ������z�f�X�N�g�b�v���ɃE�C���h�E�\�������悤��Hide�ł͂Ȃ���UClose����
	NVI_Close();
}

/**
�E�C���h�E�ʒu�ύX�ʒm���̃R�[���o�b�N
*/
void	AWindow_Find::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
}

//#935
/**
�t�@�C���I��������
*/
void	AWindow_Find::EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile )
{
	switch(inControlID)
	{
		//�ꊇ�u��
	  case kButton_BatchReplace:
		{
			AObjectID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				//�����p�����[�^�擾
				AFindParameter	findParam;
				GetApp().SPI_GetFindParam(findParam);
				//�ꊇ�u�����s
				AText	text;
				GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_BatchReplace,inFile,text);
				GetApp().SPI_GetTextWindowByID(winID).SPI_BatchReplace(findParam,text);
			}
			break;
		}
	}
}
#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�E�C���h�E����

//�E�C���h�E����
void AWindow_Find::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/Find");
	NVI_CreateEditBoxView(kText_FindText);
	NVI_SetControlBindings(kText_FindText,true,true,true,false,false,true);
	NVI_GetEditBoxView(kText_FindText).SPI_SetCenterizeWhen1Line();
	/*#872
	NVI_RegisterToFocusGroup(kButton_FindList,true);//#353
	NVI_RegisterToFocusGroup(kButton_FindFromFirst,true);//#353
	NVI_RegisterToFocusGroup(kButton_FindPrevious,true);//#353
	NVI_RegisterToFocusGroup(kButton_FindNext,true);//#353
	NVI_RegisterToFocusGroup(kPopup_RecentlyUsedReplaceText,true);//#353
	*/
	NVI_CreateEditBoxView(kText_ReplaceText);
	NVI_SetControlBindings(kText_ReplaceText,true,true,true,false,false,true);
	NVI_GetEditBoxView(kText_ReplaceText).SPI_SetCenterizeWhen1Line();
	NVI_RegisterRadioGroup();
	NVI_AddToLastRegisteredRadioGroup(kRadio_ReplaceNext);
	NVI_AddToLastRegisteredRadioGroup(kRadio_ReplaceAfterCaret);
	NVI_AddToLastRegisteredRadioGroup(kRadio_ReplaceInSelection);
	NVI_AddToLastRegisteredRadioGroup(kRadio_ReplaceAll);
	/*#872
	NVI_RegisterToFocusGroup(kButton_ReplaceAndFind,true);//#353
	NVI_RegisterToFocusGroup(kButton_Replace,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_IgnoreCase,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_WholeWord,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_IgnoreSpaces,true);//#165
	NVI_RegisterToFocusGroup(kCheckBox_BackslashYen,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_RegExp,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_AllowReturnTab,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_Loop,true);//#353
	NVI_RegisterToFocusGroup(kCheckBox_Aimai,true);//#353
	NVI_RegisterToFocusGroup(kButton_FindMemory,true);//#353
	NVI_RegisterToFocusGroup(kPopup_RecentlyUsedFindText,true);//#353
	*/
	NVI_SetDefaultOKButton(kButton_FindNext);
	
	//NVI_SetControlBindings(kText_FindText,false,true,true,true,false,true);
	
	//�e�[�u���ݒ�
	NVM_RegisterDBTable(kTable_FindMemory,AAppPrefDB::kFindMemoryName,
			kButton_FindMemoryAdd,kButton_FindMemoryRemove,kButton_FindMemoryRemoveOK,
			kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,
			kControlID_Invalid,true,true,true,true,true);
	NVM_RegisterDBTableColumn(	kTable_FindMemory,kColumn_Index,			kDataType_TextArray,
								kControlID_Invalid,						50,"",false);//win 080618
	NVM_RegisterDBTableColumn(	kTable_FindMemory,kColumn_Name,			AAppPrefDB::kFindMemoryName,
								kControlID_Invalid,						120,"FindMemory_ListViewTitle1",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_FindMemory,kColumn_FindText,		AAppPrefDB::kFindMemoryFindText,
								kControlID_Invalid,						160,"FindMemory_ListViewTitle2",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_FindMemory,kColumn_ReplaceText,	AAppPrefDB::kFindMemoryReplaceText,
								kControlID_Invalid,						160,"FindMemory_ListViewTitle3",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_FindMemory,kColumn_IgnoreCase,		AAppPrefDB::kFindMemoryIgnoreCase,
								kControlID_Invalid,						35,"FindMemory_ListViewTitle4",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_FindMemory,kColumn_WholeWord,		AAppPrefDB::kFindMemoryWholeWord,
								kControlID_Invalid,						35,"FindMemory_ListViewTitle5",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_FindMemory,kColumn_Aimai,			AAppPrefDB::kFindMemoryAimai,
								kControlID_Invalid,						35,"FindMemory_ListViewTitle6",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_FindMemory,kColumn_RegExp,			AAppPrefDB::kFindMemoryRegExp,
								kControlID_Invalid,						35,"FindMemory_ListViewTitle7",true);//win 080618
	NVM_RegisterDBTableColumn(	kTable_FindMemory,kColumn_Loop,			AAppPrefDB::kFindMemoryLoop,
								kControlID_Invalid,						35,"FindMemory_ListViewTitle8",true);//win 080618
	//�e�[�u���̕�
	NVI_SetTableColumnWidth(kTable_FindMemory,kColumn_Index,			GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFindMemory_NumberWidth));//#205
	NVI_SetTableColumnWidth(kTable_FindMemory,kColumn_Name,				GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFindMemory_NameWidth));//#205
	NVI_SetTableColumnWidth(kTable_FindMemory,kColumn_FindText,			GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFindMemory_FindStringWidth));//#205
	NVI_SetTableColumnWidth(kTable_FindMemory,kColumn_ReplaceText,		GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFindMemory_ReplaceStringWidth));//#205
	//
	NVI_GetListView(kTable_FindMemory).SPI_SetControlBindings(true,true,true,false,false,true);
	//�E�C���h�E��
	//B0000 NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFind_WindowWidth));
	
	/*#693
	//�����I�v�V�����f�t�H���g�ݒ�
	NVI_SetControlBool(kCheckBox_IgnoreCase,			GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreCase));
	NVI_SetControlBool(kCheckBox_WholeWord,			GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultWholeWord));
	NVI_SetControlBool(kCheckBox_Aimai,				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultAimai));
	NVI_SetControlBool(kCheckBox_BackslashYen,		(GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreBackslashYen)==false));
	NVI_SetControlBool(kCheckBox_RegExp,				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultRegExp));
	//#872 NVI_SetControlBool(kCheckBox_AllowReturnTab,		GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultAllowReturn));
	NVI_SetControlBool(kCheckBox_Loop,				GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultLoop));
	NVI_SetControlBool(kCheckBox_IgnoreSpaces,		GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kFindDefaultIgnoreSpaces));//#165
	*/
	
	//���K�\���T���v�����X�g�ݒ�
	NVI_RegisterTextArrayMenu(kPopup_RegExpList,kTextArrayMenuID_RegExpList);
	
	//AApp����FindParam�擾���āAUI�֔��f #693
	SPI_UpdateUI_CurrentFindParam();
	
	//
	MakeRecentlyUsedFindTextMenu();
	MakeRecentlyUsedReplaceTextMenu();
	
	//win
	NVI_SetControlBool(kRadio_ReplaceNext,true);
	
	//#872
	NVI_SetControlBool(kTriangle_FindOptions,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFindWindow_FindOptionsExpanded));
	NVI_SetControlBool(kTriangle_Replace,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFindWindow_ReplaceExpanded));
	//drop NVI_SetControlBool(kTriangle_AddToMacro,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFindWindow_AddToMacroCollapsed));
	NVI_SetControlBool(kTriangle_FindMemory,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFindWindow_FindMemoryExpanded));
	NVI_SetControlBool(kTriangle_AdvancedSearch,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFindWindow_AdvancedSearchExpanded));
	NVI_OptimizeWindowBounds(kStaticText_BottomMarker,kControlID_Invalid);
	
	//
	const ANumber kWindowMinimumWidth = 280;
	const ANumber kWindowMaximumWidth = 100000;
	
	ARect	bounds = {0};
	NVI_GetWindowBounds(bounds);
	NVI_SetWindowMinimumSize(kWindowMinimumWidth,bounds.bottom-bounds.top);
	NVI_SetWindowMaximumSize(kWindowMaximumWidth,bounds.bottom-bounds.top);
	
	//
	NVI_SetWindowWidth(GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kFindWindow_WindowWidth));
	
	//#1373
	NVI_RegisterHelpAnchor(90001,"find.htm#option");
}

//Hide()�����ɃE�C���h�E�̃f�[�^��ۑ�����
void	AWindow_Find::NVIDO_Hide()
{
	//win �ŋߎg�����������AppPrefDB�֕ۑ�
	GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kFindStringCache);
	for( AIndex i = 0; i < mRecentlyUsedFindTextArray.GetItemCount(); i++ )
	{
		if( i >= 128 )   break;//#693
		GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kFindStringCache,
											mRecentlyUsedFindTextArray.GetTextConst(i));
	}
	GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kReplaceStringCache);
	for( AIndex i = 0; i < mRecentlyUsedReplaceTextArray.GetItemCount(); i++ )
	{
		if( i >= 128 )   break;//#693
		GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kReplaceStringCache,
											mRecentlyUsedReplaceTextArray.GetTextConst(i));
	}
	//#688
	//����(close)�Ƃ��ɁA���݂�UI�̓��e��App���̃f�[�^�ɕۑ�����
	UpdateData_CurrentFindParam();
	
	//�e�[�u���̕��ۑ�
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kFindMemory_NumberWidth,			NVI_GetTableColumnWidth(kTable_FindMemory,kColumn_Index));//#205
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kFindMemory_NameWidth,				NVI_GetTableColumnWidth(kTable_FindMemory,kColumn_Name));//#205
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kFindMemory_FindStringWidth,		NVI_GetTableColumnWidth(kTable_FindMemory,kColumn_FindText));//#205
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kFindMemory_ReplaceStringWidth,	NVI_GetTableColumnWidth(kTable_FindMemory,kColumn_ReplaceText));//#205
	
	//#872
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kFindWindow_FindOptionsExpanded,NVI_GetControlBool(kTriangle_FindOptions));
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kFindWindow_ReplaceExpanded,NVI_GetControlBool(kTriangle_Replace));
	//drop GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kFindWindow_AddToMacroCollapsed,NVI_GetControlBool(kTriangle_AddToMacro));
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kFindWindow_FindMemoryExpanded,NVI_GetControlBool(kTriangle_FindMemory));
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kFindWindow_AdvancedSearchExpanded,NVI_GetControlBool(kTriangle_AdvancedSearch));
	
	//
	GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kFindWindow_WindowWidth,NVI_GetWindowWidth());
}

void	AWindow_Find::NVIDO_Close()
{
}

void	AWindow_Find::NVIDO_UpdateProperty()
{
	//win
	AText	fontname;
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kFindDialogFontName,fontname);
	
	//
	/*#135
	NVI_SetControlTextFont(kText_FindText,
			//win GetApp().GetAppPref().GetData_Font(AAppPrefDB::kFindDialogFont),
			fontname,
			static_cast<short>(GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFind_FontSize)));
	NVI_SetControlTextFont(kText_ReplaceText,
			//win GetApp().GetAppPref().GetData_Font(AAppPrefDB::kFindDialogFont),
			fontname,
			static_cast<short>(GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFind_FontSize)));
	*/
	/*#688
	NVI_GetEditBoxView(kText_FindText).SPI_SetTextDrawProperty(fontname,GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFind_FontSize),
			kColor_Black,kColor_Gray);//#135
	NVI_GetEditBoxView(kText_ReplaceText).SPI_SetTextDrawProperty(fontname,GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFind_FontSize),
			kColor_Black,kColor_Gray);//#135
	*/
	NVI_SetControlTextFont(kText_FindText,fontname,GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFind_FontSize));
	NVI_SetControlTextFont(kText_ReplaceText,fontname,GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kFind_FontSize));
}

//
void	AWindow_Find::NVMDO_UpdateControlStatus()
{
	ADocumentID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	if( docID == kObjectID_Invalid )
	{
		NVI_SetControlEnable(kButton_FindNext,false);
		NVI_SetControlEnable(kButton_FindPrevious,false);
		NVI_SetControlEnable(kButton_FindFromFirst,false);
		NVI_SetControlEnable(kButton_FindList,false);
		NVI_SetControlEnable(kButton_Replace,false);
		NVI_SetControlEnable(kButton_ReplaceAndFind,false);
		NVI_SetControlEnable(kButton_AddMacro_Replace,false);
	}
	else
	{
		AText	text;
		//#135 NVI_GetControlText(kText_FindText,text);
		NVI_GetControlText(kText_FindText,text);//#135
		ABool	findtextOK = (text.GetLength() > 0);
		NVI_SetControlEnable(kButton_FindNext,findtextOK);
		NVI_SetControlEnable(kButton_FindPrevious,findtextOK);
		NVI_SetControlEnable(kButton_FindFromFirst,findtextOK);
		NVI_SetControlEnable(kButton_FindList,findtextOK);
		NVI_SetControlEnable(kButton_ReplaceAndFind,findtextOK);
		NVI_SetControlEnable(kButton_Replace,findtextOK);
		NVI_SetControlEnable(kButton_AddMacro_Replace,findtextOK);
		
		if( GetApp().SPI_GetTextDocumentByID(docID).NVI_IsReadOnly() )
		{
			NVI_SetControlEnable(kButton_ReplaceAndFind,false);
			NVI_SetControlEnable(kButton_Replace,false);
		}
		
		//�u�I��͈͓��v���W�I�{�^��
		AWindowID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
		if( winID != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetTextWindowByID(winID).SPI_IsCaretMode() == false )
			{
				NVI_SetControlEnable(kRadio_ReplaceInSelection,true);
			}
			else
			{
				NVI_SetControlEnable(kRadio_ReplaceInSelection,false);
				if( NVI_GetControlBool(kRadio_ReplaceInSelection) == true )
				{
					NVI_SetControlBool(kRadio_ReplaceInSelection,false);
					NVI_SetControlBool(kRadio_ReplaceNext,true);
				}
				NVI_SetControlEnable(kButton_ReplaceAndFind,false);
			}
		}
	}
	
	
	//#930 NVI_SetControlEnable(kCheckBox_IgnoreCase,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_WholeWord,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_Aimai,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_BackslashYen,(NVI_GetControlBool(kCheckBox_RegExp)==false));
	NVI_SetControlEnable(kCheckBox_IgnoreSpaces,(NVI_GetControlBool(kCheckBox_RegExp)==false));//#165
	
	/*#872
	NVI_SetCatchReturn(kText_FindText,NVI_GetControlBool(kCheckBox_AllowReturnTab));//#135
	NVI_SetCatchTab(kText_FindText,NVI_GetControlBool(kCheckBox_AllowReturnTab));//#135
	NVI_SetCatchReturn(kText_ReplaceText,NVI_GetControlBool(kCheckBox_AllowReturnTab));//#135
	NVI_SetCatchTab(kText_ReplaceText,NVI_GetControlBool(kCheckBox_AllowReturnTab));//#135
	*/
	
	//�u���������{�^���̍X�V�́A������v����Ɏ��Ԃ�������̂ōs��Ȃ�
	
	//�u���{�^���̃e�L�X�g
	if( NVI_GetControlBool(kRadio_ReplaceNext) == true )
	{
		AText	text;
		text.SetLocalizedText("FindWindow_ReplaceNext");
		NVI_SetControlText(kButton_Replace,text);
	}
	else if( NVI_GetControlBool(kRadio_ReplaceAfterCaret) == true )
	{
		AText	text;
		text.SetLocalizedText("FindWindow_ReplaceAfterCaret");
		NVI_SetControlText(kButton_Replace,text);
	}
	else if( NVI_GetControlBool(kRadio_ReplaceInSelection) == true )
	{
		AText	text;
		text.SetLocalizedText("FindWindow_ReplaceInSelection");
		NVI_SetControlText(kButton_Replace,text);
	}
	else if( NVI_GetControlBool(kRadio_ReplaceAll) == true )
	{
		AText	text;
		text.SetLocalizedText("FindWindow_ReplaceAll");
		NVI_SetControlText(kButton_Replace,text);
	}
}

#pragma mark ===========================

#pragma mark ---�������ݒ�^�擾

/*#693
void	AWindow_Find::SPI_SetFindText( const AText& inText )
{
	//#135 NVI_SetControlText(kText_FindText,inText);
	NVI_SetControlText(kText_FindText,inText);
}

void	AWindow_Find::SPI_SetReplaceText( const AText& inText )
{
	//#135 NVI_SetControlText(kText_ReplaceText,inText);
	NVI_SetControlText(kText_ReplaceText,inText);
}
*/

/**
���݂̌����p�����[�^�f�[�^��UI�֔��f����
*/
void	AWindow_Find::SPI_UpdateUI_CurrentFindParam()
{
	//�E�C���h�E�������Ȃ牽�����Ȃ�
	if( NVI_IsWindowCreated() == false )
	{
		return;
	}
	//�����I�v�V�����擾
	AFindParameter	findParam;
	GetApp().SPI_GetFindParam(findParam);
	//UI�֔��f
	NVI_SetControlText(kText_FindText,			findParam.findText);
	NVI_SetControlText(kText_ReplaceText,		findParam.replaceText);
	NVI_SetControlBool(kCheckBox_IgnoreCase,	findParam.ignoreCase);
	NVI_SetControlBool(kCheckBox_WholeWord,		findParam.wholeWord);
	NVI_SetControlBool(kCheckBox_Aimai,			findParam.fuzzy);
	NVI_SetControlBool(kCheckBox_BackslashYen,	(findParam.ignoreBackslashYen==false));
	NVI_SetControlBool(kCheckBox_RegExp,		findParam.regExp);
	NVI_SetControlBool(kCheckBox_Loop,			findParam.loop);
	NVI_SetControlBool(kCheckBox_IgnoreSpaces,	findParam.ignoreSpaces);
}

/**
���݂�UI���e�������p�����[�^�f�[�^�֐ݒ肷��
*/
void	AWindow_Find::UpdateData_CurrentFindParam() const
{
	//UI����f�[�^�擾
	AFindParameter	findParam;
	AText	text;
	NVI_GetControlText(kText_FindText,text);
	findParam.findText.SetText(text);
	NVI_GetControlText(kText_ReplaceText,text);
	findParam.replaceText.SetText(text);
	findParam.ignoreCase 		= NVI_GetControlBool(kCheckBox_IgnoreCase);
	findParam.wholeWord 		= NVI_GetControlBool(kCheckBox_WholeWord);
	findParam.fuzzy				= NVI_GetControlBool(kCheckBox_Aimai);
	findParam.regExp 			= NVI_GetControlBool(kCheckBox_RegExp);
	findParam.loop 				= NVI_GetControlBool(kCheckBox_Loop);
	findParam.ignoreBackslashYen = (NVI_GetControlBool(kCheckBox_BackslashYen)==false);
	findParam.ignoreSpaces		= NVI_GetControlBool(kCheckBox_IgnoreSpaces);
	//�����I�v�V�����f�[�^�ݒ�
	GetApp().SPI_SetFindParam(findParam);
}

void	AWindow_Find::SPI_ShowReplaceResultSheet( const AItemCount inReplacedCount )
{
	AText	message1;
	message1.SetLocalizedText("ReplaceResult_Message1");
	AText	text;
	text.SetFormattedCstring("%d",inReplacedCount);
	message1.ReplaceParamText('0',text);
	NVI_ShowChildWindow_OK(message1,GetEmptyText());
}

#pragma mark ===========================

#pragma mark <�������W���[��>

#pragma mark ===========================

#pragma mark ---�u�ŋߎg����������v

void	AWindow_Find::MakeRecentlyUsedFindTextMenu()//win
{
	ATextArray	textArray;
	textArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kFindStringCache));
	AText	text;
	text.SetLocalizedText("FindCacheMenu_Find");
	textArray.Insert1(0,text);
	text.SetLocalizedText("RecentlyMenus_DeleteAll");
	textArray.Add(text);
	NVI_SetMenuItemsFromTextArray(kPopup_RecentlyUsedFindText,textArray);
	//#1481
	NVI_SetControlNumber(kPopup_RecentlyUsedFindText,0);
	
	//win
	mRecentlyUsedFindTextArray.DeleteAll();//#901
	const ATextArray&	findStringCacheTextArray = 
			GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kFindStringCache);
	for( AIndex i = 0; i < findStringCacheTextArray.GetItemCount(); i++ )
	{
		mRecentlyUsedFindTextArray.Add(findStringCacheTextArray.GetTextConst(i));
	}
}

void	AWindow_Find::MakeRecentlyUsedReplaceTextMenu()//win
{
	ATextArray	textArray;
	textArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kReplaceStringCache));
	AText	text;
	text.SetLocalizedText("FindCacheMenu_Replace");
	textArray.Insert1(0,text);
	text.SetLocalizedText("RecentlyMenus_DeleteAll");
	textArray.Add(text);
	NVI_SetMenuItemsFromTextArray(kPopup_RecentlyUsedReplaceText,textArray);
	//#1481
	NVI_SetControlNumber(kPopup_RecentlyUsedReplaceText,0);
	
	//win
	mRecentlyUsedReplaceTextArray.DeleteAll();//#901
	const ATextArray&	replaceStringCacheTextArray = 
			GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kReplaceStringCache);
	for( AIndex i = 0; i < replaceStringCacheTextArray.GetItemCount(); i++ )
	{
		mRecentlyUsedReplaceTextArray.Add(replaceStringCacheTextArray.GetTextConst(i));
	}
}


void	AWindow_Find::SPI_AddRecentlyUsedFindText( const AFindParameter& inFindParam )
{
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )   return;//#352
	
	//���łɍŋߎg����������Ɋ܂܂�Ă��邩�ǂ��������炩���ߌ���
	AIndex	foundindex = mRecentlyUsedFindTextArray.Find(inFindParam.findText);
	//��ԍŏ��ɒǉ�
	mRecentlyUsedFindTextArray.Insert1(0,inFindParam.findText);
	//�E�C���h�E�����ς݂Ȃ�A�|�b�v�A�b�v���j���[�ɂ��ǉ�
	if( NVI_IsWindowCreated() == true )
	{
		NVI_InsertMenuItem(kPopup_RecentlyUsedFindText,1,inFindParam.findText);
	}
	if( foundindex != kIndex_Invalid )
	{
		//�ŋߎg����������Ɋ܂܂�Ă����ꍇ�A���f�[�^������
		foundindex++;
		mRecentlyUsedFindTextArray.Delete1(foundindex);
		//�E�C���h�E�����ς݂Ȃ�A�|�b�v�A�b�v���j���[������폜
		if( NVI_IsWindowCreated() == true )
		{
			NVI_DeleteMenuItem(kPopup_RecentlyUsedFindText,1+foundindex);
		}
	}
	else if( mRecentlyUsedFindTextArray.GetItemCount() >= 128 )
	{
		//�ŋߎg����������Ɋ܂܂�Ă��Ȃ������ꍇ�A�V�K�ǉ����A128�ȏ�Ȃ�Ō������
		mRecentlyUsedFindTextArray.Delete1(127);
		//�E�C���h�E�����ς݂Ȃ�A�|�b�v�A�b�v���j���[������폜
		if( NVI_IsWindowCreated() == true )
		{
			NVI_DeleteMenuItem(kPopup_RecentlyUsedFindText,128);
		}
	}
	//#1481
	NVI_SetControlNumber(kPopup_RecentlyUsedFindText,0);
}

void	AWindow_Find::SPI_AddRecentlyUsedReplaceText( const AFindParameter& inFindParam )
{
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )   return;//#352
	
	//���łɍŋߎg����������Ɋ܂܂�Ă��邩�ǂ��������炩���ߌ���
	AIndex	foundindex = mRecentlyUsedReplaceTextArray.Find(inFindParam.replaceText);
	//��ԍŏ��ɒǉ�
	mRecentlyUsedReplaceTextArray.Insert1(0,inFindParam.replaceText);
	//�E�C���h�E�����ς݂Ȃ�A�|�b�v�A�b�v���j���[�ɂ��ǉ�
	if( NVI_IsWindowCreated() == true )
	{
		NVI_InsertMenuItem(kPopup_RecentlyUsedReplaceText,1,inFindParam.replaceText);
	}
	if( foundindex != kIndex_Invalid )
	{
		//�ŋߎg����������Ɋ܂܂�Ă����ꍇ�A���f�[�^������
		foundindex++;
		mRecentlyUsedReplaceTextArray.Delete1(foundindex);
		//�E�C���h�E�����ς݂Ȃ�A�|�b�v�A�b�v���j���[������폜
		if( NVI_IsWindowCreated() == true )
		{
			NVI_DeleteMenuItem(kPopup_RecentlyUsedReplaceText,1+foundindex);
		}
	}
	else if( mRecentlyUsedReplaceTextArray.GetItemCount() >= 128 )
	{
		//�ŋߎg����������Ɋ܂܂�Ă��Ȃ������ꍇ�A�V�K�ǉ����A128�ȏ�Ȃ�Ō������
		mRecentlyUsedReplaceTextArray.Delete1(127);
		//�E�C���h�E�����ς݂Ȃ�A�|�b�v�A�b�v���j���[������폜
		if( NVI_IsWindowCreated() == true )
		{
			NVI_DeleteMenuItem(kPopup_RecentlyUsedReplaceText,128);
		}
	}
	//#1481
	NVI_SetControlNumber(kPopup_RecentlyUsedReplaceText,0);
}

#pragma mark ===========================

#pragma mark ---�����������[

void	AWindow_Find::SPI_SetFromFindMemory( const AIndex inMemoryIndex )
{
	AText	text;
	GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFindMemoryFindText,inMemoryIndex,text);
	//#135 NVI_SetControlText(kText_FindText,text);
	NVI_SetControlText(kText_FindText,text);//#135
	GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kFindMemoryReplaceText,inMemoryIndex,text);
	//#135 NVI_SetControlText(kText_ReplaceText,text);
	NVI_SetControlText(kText_ReplaceText,text);//#135
	NVI_SetControlBool(kCheckBox_IgnoreCase,GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFindMemoryIgnoreCase	,inMemoryIndex));
	NVI_SetControlBool(kCheckBox_WholeWord	,GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFindMemoryWholeWord	,inMemoryIndex));
	NVI_SetControlBool(kCheckBox_Aimai		,GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFindMemoryAimai		,inMemoryIndex));
	NVI_SetControlBool(kCheckBox_RegExp	,GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFindMemoryRegExp		,inMemoryIndex));
	NVI_SetControlBool(kCheckBox_Loop		,GetApp().GetAppPref().GetData_BoolArray(AAppPrefDB::kFindMemoryLoop		,inMemoryIndex));
	NVM_UpdateControlStatus();
}

void	AWindow_Find::SPI_CopyToFindMemory( const AIndex inMemoryIndex )
{
	AText	text;
	//#135 NVI_GetControlText(kText_FindText,text);
	NVI_GetControlText(kText_FindText,text);//#135
	GetApp().GetAppPref().SetData_TextArray(AAppPrefDB::kFindMemoryFindText,inMemoryIndex,text);
	//#135 NVI_GetControlText(kText_ReplaceText,text);
	NVI_GetControlText(kText_ReplaceText,text);//#135
	GetApp().GetAppPref().SetData_TextArray(AAppPrefDB::kFindMemoryReplaceText,inMemoryIndex,text);
	GetApp().GetAppPref().SetData_BoolArray(AAppPrefDB::kFindMemoryIgnoreCase,	inMemoryIndex, NVI_GetControlBool(kCheckBox_IgnoreCase));
	GetApp().GetAppPref().SetData_BoolArray(AAppPrefDB::kFindMemoryWholeWord,	inMemoryIndex, NVI_GetControlBool(kCheckBox_WholeWord));
	GetApp().GetAppPref().SetData_BoolArray(AAppPrefDB::kFindMemoryAimai,		inMemoryIndex, NVI_GetControlBool(kCheckBox_Aimai));
	GetApp().GetAppPref().SetData_BoolArray(AAppPrefDB::kFindMemoryRegExp,		inMemoryIndex, NVI_GetControlBool(kCheckBox_RegExp));
	GetApp().GetAppPref().SetData_BoolArray(AAppPrefDB::kFindMemoryLoop,		inMemoryIndex, NVI_GetControlBool(kCheckBox_Loop));
}

void	AWindow_Find::NVMDO_UpdateTableView( const AControlID inTableControlID, const ADataID inColumnID )
{
	switch(inTableControlID)
	{
	  case kTable_FindMemory:
		{
			switch(inColumnID)
			{
			  case kColumn_Index:
				{
					ATextArray	textArray;
					for( AIndex i = 0; i < GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kFindMemoryName) ; i++ )
					{
						AText	text;
						text.SetFormattedCstring("%d",i+1);
						textArray.Add(text);
					}
					//�e�[�u���ɐݒ�(BeginSetTable/EndSetTable��AWindow���Ŏ��s�����)
					NVI_SetTableColumn_Text(kTable_FindMemory,kColumn_Index,textArray);//#205
					break;
				}
			}
			break;
		}
	}
}

//
void	AWindow_Find::NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
		const AControlID inTriggerControlID )
{
	switch(inControlID)
	{
	  case kTable_FindMemory:
		{
			if( inModificationType == kModificationType_RowAdded && inTriggerControlID == kButton_FindMemoryAdd )//#286
			{
				//�����E�C���h�E����̃R�s�[
				SPI_CopyToFindMemory(inIndex);
				NVM_UpdateDBTableView(kTable_FindMemory);
			}
			break;
		}
	}
}

ABool	AWindow_Find::EVTDO_DoubleClicked( const AControlID inID )
{
	ABool	result = false;
	switch(inID)
	{
	  case kTable_FindMemory:
		{
			AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(kTable_FindMemory);
			if( rowIndex != kIndex_Invalid )
			{
				ADataID	columnID = NVI_GetTableViewSelectionColumnID(kTable_FindMemory);
				{
					SPI_SetFromFindMemory(rowIndex);
					result = true;
				}
			}
			break;
		}
	}
	return result;
}

