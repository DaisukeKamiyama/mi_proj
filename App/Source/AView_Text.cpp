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

AView_Text

*/
/*
AView_Text�݌v�|���V�[

�EAView_Text�������A��Ƀh�L�������g�����݂���B�i�h�L�������g�����݂��Ȃ���Ԃ�View�𓮍삳���邱�Ƃ͌��X�l������Ă��Ȃ����߁j(110323)

*/

#include "stdafx.h"

#include "AView_Text.h"
#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AApp.h"
//#include "../Wrapper.h"
//#include "../Imp.h"
//#include "AUtil.h"
#include "ADocument_IndexWindow.h"
//#858 #include "AWindow_Fusen.h"//#138
//#858 #include "AWindow_FusenEdit.h"//#138
#include "AView_IdInfo.h"//#238
#include "AView_LineNumber.h"//#450
#include "AView_CallGraf.h"
#include "AWindow_Notification.h"
#include "AView_Notification.h"

#if IMPLEMENTATION_FOR_MACOSX
const ANumber kBraceCheckTimerCount = 5;//#688 20;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const ANumber kBraceCheckTimerCount = 3;
#endif

//#456 �h���b�O�X�N���[�����s����������ʏ�[���[�������������ɂ���
const ANumber	kDragScrollMargin = 8;

//#867
//idle work timer�^�C�}�[�l
const ANumber	kKeyIdleWorkTimerValue = 30;
const ANumber	kMouseClikcIdleWorkTimerValue = 3;

#pragma mark ===========================
#pragma mark [�N���X]AView_Text
#pragma mark ===========================
//�e�L�X�g�\��View
//�e�E�C���h�E��AWindow�ł���Ηǂ��AAWindow_Text�ȊO�ł��g�p��

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_Text::AView_Text( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/
			const AWindowID inWindowID, const AControlID inID, 
			/*#695 const ADocumentID inTextDocumentID*/ADocument_Text& inTextDocument, 
			const AWindowID inTextWindowID, //#379
			/*#699 const ABool inCaretMode, */
			/*#699 const ATextPoint& inCaretTextPoint, const ATextPoint& inSelectTextPoint, */const AImagePoint& inOriginOfLocalFrame,
			const ABool inCrossCaretMode )//B0345
: AView(/*#199 inParent,inWindow*/inWindowID,inID,inOriginOfLocalFrame), //#695 mTextDocumentID(inTextDocumentID),
		mCaretMode(/*#699 inCaretMode*/false), 
		mCaretTextPoint(/*#699 inCaretTextPoint*/kTextPoint_00), 
		mSelectTextPoint(/*#699 inSelectTextPoint*/kTextPoint_00), 
		mCaretImagePoint(kImagePoint_00),mSelectImagePoint(kImagePoint_00),//#831
		mCaretTickCount(0), mCaretBlinkStateVisible(false), mInlineInputNotFixedTextLen(0), 
		mOldArrowImageX(0), mForceSelectMode(false), mDragDrawn(false), mDragWordDragDrop(false), mPrinting(false), mBackgroundImageOffset(0),
		/*#717 mLastAbbrevKeywordIndex(kIndex_Invalid), */mLineMargin(0), /*#379mTextWindowID(kObjectID_Invalid),*/ mDisplayingBraceCheck(false), mKeyToolMode(false),
		mKukeiSelectionMode(false), mKukeiSelected(false), mCrossCaretMode(true/*inCrossCaretMode*/), mCrossCaretHVisible(false), mCrossCaretVVisible(false), mDragging(false),
		mMouseTrackingMode(kMouseTrackingMode_None), /*#688 mMouseTrackByLoop(true),*/ mTempCaretHideCount(0)
		,mSelectionByFind(false)//#262
		,mToolData_DontShowDropWarning(false)//#308 ���������̌������ǂ����͕s������
		,mFigMode(kFigMode_None)//#478
		,mLineNumberViewID(kObjectID_Invalid)//#450
		,mTextWindowID(inTextWindowID)//#379
		,mDisableAdjustDiffDisplayScrollAtScrollPostProcess(false)//#611
,mTextDocumentRef(inTextDocument)//#695
,mLineImageInfo_Height(this,kLineInfoInitialAllocation,kLineInfoAllocationStep)//#695
,mLineImageInfo_ImageY(this,kLineInfoInitialAllocation,kLineInfoAllocationStep)//#695
,mLineImageInfo_Collapsed(this,kLineInfoInitialAllocation,kLineInfoAllocationStep)//#695
,mReservedSelection_StartTextIndex(kIndex_Invalid),mReservedSelection_EndTextIndex(kIndex_Invalid)//#699
,mReservedSelection_StartTextPoint(kTextPoint_Invalid)//#699
,mReservedSelection_EndTextPoint(kTextPoint_Invalid)//#699
,mReservedSelection_IsSelectWithParagraph(false)//#699
,mAbbrevInputMode(false),mAbbrevCurrentCandidateIndex(kIndex_Invalid),mAbbrevRequestInputKeyCount(0)//#717
,mIdInfoCurrentArgHighlighted(false),mIdInfoCurrentArgStartTextPoint(kTextPoint_00),mIdInfoCurrentArgEndTextPoint(kTextPoint_00)//#853
,mKeyIdleWorkType(kKeyIdleWorkType_None),mKeyIdleWorkTimer(0)//#867
,mAutoSaveUnsavedCheckTickCount(0)//#81
,mPopupNotificationWindowID(kObjectID_Invalid)//#725
,mCurrentHeaderPathJumpMenuItemIndex(kIndex_Invalid)//#906
,mCurrentCaretStateColorSlotIndex(kIndex_Invalid)
,mMouseDownTick(0)
,mKeyToolMenuObjectID_AppTool(kObjectID_Invalid),mKeyToolMenuStartIndex_AppTool(kIndex_Invalid),mKeyToolMenuEndIndex_AppTool(kIndex_Invalid)
,mKeyToolMenuObjectID_UserTool(kObjectID_Invalid),mKeyToolMenuStartIndex_UserTool(kIndex_Invalid),mKeyToolMenuEndIndex_UserTool(kIndex_Invalid)
,mSuppressRequestCandidateFinder(false)
,mAbbrevCandidateState_Requesting(false),mAbbrevCandidateState_Displayed(false)
,mCurrentMouseTrackModifierKeys(0)//#795
,mCurrentMouseTrackInsideLineNumber(false)
,mCandidateRequestTimer(0)
,mLastCaretLocalPoint(kLocalPoint_00)//#1031
,mACharWidth(9)//#1186
,mZenkakuAWidth(18)//#1385
,mBraceHighlighted(false),mBraceHighlightStartTextPoint1(kTextPoint_00),mBraceHighlightEndTextPoint1(kTextPoint_00),mBraceHighlightStartTextPoint2(kTextPoint_00),mBraceHighlightEndTextPoint2(kTextPoint_00)//#1406
{
	NVMC_SetOffscreenMode(true);//win
	//#92 ��������NVIDO_Init()�ֈړ�
	
	//�I��ݒ�i�h�L�������g�̍s�܂�Ԃ��v�Z�������Ȃ�I��\��ɂȂ�j#699
	//text view�������ɂ͑I��ݒ肵�Ȃ��B#699
	//#699 SetSelect(inCaretTextPoint,inSelectTextPoint);
	
	//view��s�����ɐݒ� #1090
	NVMC_SetOpaque(true);
	
	//#913
	//�}�[�N������
	mMarkStartTextPoint.x = mMarkStartTextPoint.y = -1;
	mMarkEndTextPoint.x = mMarkEndTextPoint.y = -1;
	//�������ۂ��̂��l�������X�N���[�������Ȃ��悤�ɐݒ�itext view�ł͕K�v�Ȃ��̂Łj #947
	NVI_SetAdjustScrollForRoundedCorner(false);
	
	//�T�[�r�X���j���[�ېݒ� #1309
	NVI_SetServiceMenuAvailable();
}

//�f�X�g���N�^
AView_Text::~AView_Text()
{
	//#92 NVIDO_DoDeleted()�ֈړ� HideCandidateWindow();
}

//#138
void	AView_Text::NVIDO_DoDeleted()
{
	//#725
	//�|�b�v�A�b�vnotification�E�C���h�E�폜
	if( mPopupNotificationWindowID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteWindow(mPopupNotificationWindowID);
	}
	/*#858
	//#138
	//�tⳎ��E�C���h�E��S�č폜
	for( AIndex i = 0; i < mFusenArray_WindowID.GetItemCount(); i++ )
	{
		GetApp().NVI_DeleteWindow(mFusenArray_WindowID.Get(i));
	}
	mFusenArray_WindowID.DeleteAll();
	*/
	//Document�I�u�W�F�N�g��View�o�^������
	GetTextDocument().NVI_UnregisterView(GetObjectID());//#92 SPI_UnregisterTextViewID(GetObjectID());
	//#92
	//�⊮���͏�ԉ���
	ClearAbbrevInputMode(true,false);
}

//win 080709
void	AView_Text::NVIDO_Init()
{
	mZenkakuSpaceText.SetLocalizedText("Text_ZenkakuSpace");
	mLineEndDisplayText.SetLocalizedText("ReturnCodeText");
	mLineEndDisplayText_UTF16.SetText(mLineEndDisplayText);//B0000
	mLineEndDisplayText_UTF16.ConvertFromUTF8ToUTF16();//B0000
	
	//#699 SPI_UpdateTextDrawProperty();
	//#699 SPI_InitLineImageInfo();//#450
	GetTextDocument().NVI_RegisterView(GetObjectID());//#379
	
	//�c�������[�h�ݒ� #558
	NVI_SetVerticalMode(GetTextDocumentConst().SPI_GetVerticalMode());
	
	//
	SPI_UpdateTextDrawProperty();
	SPI_InitLineImageInfo();//#450
	
	//Document�I�u�W�F�N�g��View��o�^
	//#379 GetTextDocument().NVI_RegisterView(GetObjectID());//#92 SPI_RegisterTextViewID(GetObjectID());//#138
	
	/*#831 �������BAView_Text�������ɂ�caret point��ݒ肵�Ȃ��悤�ɂ����̂ŁA�����ł�image point�擾�͕K�v�Ȃ��B
	GetImagePointFromTextPoint(GetCaretTextPoint(),mCaretImagePoint);
	//R0108
	GetImagePointFromTextPoint(GetSelectTextPoint(),mSelectImagePoint);
	*/
	//win
	NVI_SetDefaultCursor(kCursorType_IBeam);
	/*#688
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
	//#236 NVMC_EnableDrop(kScrapType_UnicodeText);
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_UnicodeText);
	scrapTypeArray.Add(kScrapType_File);//win
	scrapTypeArray.Add(kScrapType_TabSelector);//#859
	scrapTypeArray.Add(kScrapType_IdInfoItem);
	scrapTypeArray.Add(kScrapType_CallGrafItem);
	NVMC_EnableDrop(scrapTypeArray);
	
	//caret point������
	SetCaretTextPoint(kTextPoint_00);
}

//#379
/**
TextView�̕��i�E�C���h�E���s�܂�Ԃ��v�Z�p�j�擾
*/
ANumber	AView_Text::SPI_GetTextViewWidth() const
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		//InfoPange�\�������l�������r���[����Ԃ�
		return GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetTextViewWidth(/*#695 mTextDocumentID*/mTextDocumentRef.GetObjectID());
	}
	else
	{
		return NVI_GetViewWidth();
	}
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

/*#695
//TextDocument�擾
ADocument_Text&	AView_Text::GetTextDocument()
{
	MACRO_RETURN_DOCUMENT_DYNAMIC_CAST(ADocument_Text,kDocumentType_Text,mTextDocumentID);//#199
	//#199 return GetApp().SPI_GetTextDocumentByID(mTextDocumentID);
}
//TextDocument�擾
const ADocument_Text&	AView_Text::GetTextDocumentConst() const
{
	MACRO_RETURN_CONSTDOCUMENT_DYNAMIC_CAST(ADocument_Text,kDocumentType_Text,mTextDocumentID);//#199
	//#199 return GetApp().SPI_GetTextDocumentConstByID(mTextDocumentID);
}
*/
//DocPrefDB�擾
ADocPrefDB&	AView_Text::GetDocPrefDB()
{
	return GetTextDocument().GetDocPrefDB();
}

//ModePrefDB�擾
AModePrefDB&	AView_Text::GetModePrefDB()
{
	return GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex());
}

//#450
/**
*/
void	AView_Text::SPI_SetLineNumberView( const AViewID inLineNumberViewID )
{
	mLineNumberViewID = inLineNumberViewID;
}

//#450
/**
*/
AView_LineNumber&	AView_Text::GetLineNumberView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_LineNumber,kViewType_LineNumber,mLineNumberViewID);
}

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//���j���[�I��������
ABool	AView_Text::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	RestoreFromBraceCheck();
	//�⊮���͏�ԉ���
	ClearAbbrevInputMode(true,false);//RC2
	//
	ABool	result = true;
	switch(inMenuItemID)
	{
		//Undo
	  case kMenuItemID_Undo:
		{
			UndoRedo(true);
			break;
		}
		//Redo
	  case kMenuItemID_Redo:
		{
			UndoRedo(false);
			break;
		}
		//Save
	  case kMenuItemID_Save:
		{
			GetTextDocument().SPI_Save(false);
			break;
		}
		//SaveAs
	  case kMenuItemID_SaveAs:
		{
			GetTextDocument().SPI_Save(true);
			break;
		}
		//��� #524 AWindow_Text����ړ�
	  case kMenuItemID_Print:
		{
			GetTextDocument().SPI_Print();
			break;
		}
		//�I��͈͂���� #524
	  case kMenuItemID_PrintRange:
	  case kMenuItemID_CM_PrintRange:
		{
			ATextPoint	spt = {0,0}, ept = {0,0};
			SPI_GetSelect(spt,ept);
			AIndex	start = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y);
			AIndex	end = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ept.y);
			if( ept.x != 0 )
			{
				end++;
			}
			GetTextDocument().SPI_PrintWithRange(start,end);
			break;
		}
		//�R�s�[
	  case kMenuItemID_Copy:
	  case kMenuItemID_CM_Copy:
		{
			//#900
			//�L�[�L�^���Ȃ�L�^
			GetApp().SPI_RecordKeybindAction(keyAction_Copy,GetEmptyText());
			//
			Copy(false);
			break;
		}
		//�ǉ��R�s�[
	  case kMenuItemID_AdditionalCopy:
		{
			//#900
			//�L�[�L�^���Ȃ�L�^
			GetApp().SPI_RecordKeybindAction(keyAction_additionalcopy,GetEmptyText());
			//
			AText	scrapText;
			AScrapWrapper::GetClipboardTextScrap(scrapText);//#688 ,GetTextDocument().SPI_GetPreferLegacyTextEncoding(),true);
			AText	text;
			GetSelectedText(text);
			scrapText.AddText(text);
			AScrapWrapper::SetClipboardTextScrap(scrapText,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocument().SPI_GetPreferLegacyTextEncoding(),true);
			break;
		}
		//�J�b�g
	  case kMenuItemID_Cut:
	  case kMenuItemID_CM_Cut:
		{
			//#900
			//�L�[�L�^���Ȃ�L�^
			GetApp().SPI_RecordKeybindAction(keyAction_Cut,GetEmptyText());
			//
			Cut(false);
			break;
		}
		//�y�[�X�g
	  case kMenuItemID_Paste:
	  case kMenuItemID_CM_Paste:
		{
			//#900
			//�L�[�L�^���Ȃ�L�^
			GetApp().SPI_RecordKeybindAction(keyAction_Paste,GetEmptyText());
			//
			Paste();
			break;
		}
		//#929
	  case kMenuItemID_PasteAtEndOfEachLine:
		{
			PasteAtEndOfEachLine();
			break;
		}
		//�폜
	  case kMenuItemID_Clear:
	  case kMenuItemID_CM_Clear:
		{
			DeleteSelect(undoTag_DELKey);
			break;
		}
		//�S�Ă�I��
	  case kMenuItemID_SelectAll:
	  case kMenuItemID_CM_SelectAll:
		{
			SetSelect(0,GetTextDocumentConst().SPI_GetTextLength());
			break;
		}
		//#878
		//FIFO�N���b�v�{�[�h���[�h
	  case kMenuItemID_SwitchFIFOClipboardMode:
		{
			SwitchFIFOClipboardMode();
			break;
		}
		//�C���f���g
	  case kMenuItemID_Indent:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			DoIndent(/*#650 false,*/undoTag_Indent,inputSpaces,kIndentTriggerType_IndentFeature);//#249 #639 #650
			break;
		}
		//�E�փV�t�g
	  case kMenuItemID_ShiftRight:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(true,inputSpaces);
			break;
		}
		//���փV�t�g
	  case kMenuItemID_ShiftLeft:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(false,inputSpaces);
			break;
		}
		//�o�b�N�X���b�V������
	  case kMenuItemID_InputBackslashYen:
		{
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey) == true )
			{
				AText	yen;
				yen.SetLocalizedText("Text_Yen");
				TextInput(undoTag_Typing,yen);
			}
			else
			{
				AText	backslash;
				backslash.Add(kUChar_Backslash);
				TextInput(undoTag_Typing,backslash);
			}
			break;
		}
		
		//��������
	  case kMenuItemID_FindNext:
		{
			SPI_FindNext();
			result = true;
			break;
		}
		//�O������
	  case kMenuItemID_FindPrevious:
		{
			SPI_FindPrev();
			result = true;
			break;
		}
		//�ŏ����猟��
	  case kMenuItemID_FindFromFirst:
		{
			SPI_FindFromFirst();//#724
			result = true;
			break;
		}
		//�u��
	  case kMenuItemID_ReplaceSelectedTextOnly:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceSelectedTextOnly(param);
			result = true;
			break;
		}
		//�u��
	  case kMenuItemID_ReplaceAndFind:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceAndFind(param);
			result = true;
			break;
		}
		//����u��
	  case kMenuItemID_ReplaceNext:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceNext(param);
			break;
		}
		//�L�����b�g�ȍ~��u��
	  case kMenuItemID_ReplaceAfterCaret:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceAfterCaret(param);
			break;
		}
		//�I��͈͓���u��
	  case kMenuItemID_ReplaceInSelection:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceInSelection(param);
			break;
		}
		//�S�Ēu��
	  case kMenuItemID_ReplaceAll:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceAll(param);
			break;
		}
		//�I�𕶎��������������ɓo�^
	  case kMenuItemID_SetFindText:
		{
			SetFindText();
			break;
		}
		//�I�𕶎����u��������ɓo�^
	  case kMenuItemID_SetReplaceText:
		{
			if( IsCaretMode() == true )   break;
			AText	text;
			GetSelectedText(text);
			GetApp().SPI_SetReplaceText(text);
			break;
		}
		//�����n�C���C�g����
	  case kMenuItemID_EraseFindHighlight:
		{
			GetTextDocument().SPI_ClearFindHighlight();
			break;
		}
		//�W�����v
	  case kMenuItemID_Jump:
		{
			AUniqID	identifierObjectID;//#216 = inDynamicMenuActionText.GetNumber();
			identifierObjectID.val = inDynamicMenuActionText.GetNumber();//#216
			Jump(identifierObjectID);
			break;
		}
		//
	  case kMenuItemID_JumpNextItem:
		{
			JumpNext();//#150
			break;
		}
		//
	  case kMenuItemID_JumpPreviousItem:
		{
			JumpPrev();//#150
			break;
		}
		
		//�I�𕔕����J��
	  case kMenuItemID_OpenSelected:
		{
			OpenSelected(inModifierKeys);
			break;
		}
		//�c�[��
	  case kMenuItemID_Tool:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			SPI_DoTool(file,inModifierKeys,false);
			break;
		}
		//�c�[��
	  case kMenuItemID_Tool_StandardMode:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			SPI_DoTool(file,inModifierKeys,false);
			break;
		}
		//������
	  case kMenuItemID_Bookmark:
		{
			ATextPoint	spt, ept;
			SPI_GetSelect(spt,ept);
			GetTextDocument().SPI_SetBookmarkLineIndex(spt.y);
			NVM_GetWindow().NVI_RefreshWindow();
			break;
		}
		//������Ɉړ�
	  case kMenuItemID_MoveToBookmark:
		{
			SPI_MoveToLine(GetTextDocument().SPI_GetBookmarkLineIndex(),false);
			break;
		}
		//�Ή����������
	  case kMenuItemID_CorrespondText:
		{
			InputCorrespondence();
			break;
		}
		//#359
	  case kMenuItemID_SelectBetweenCorrespondText:
		{
			SelectBetweenCorrespondTexts();
			break;
		}
		//�c�[�����̓��[�h
	  case kMenuItemID_KeyToolMode:
		{
			StartKeyToolMode();
			break;
		}
		//��`�I�����[�h
	  case kMenuItemID_KukeiSelectionMode:
		{
			if( mKukeiSelectionMode == true )
			{
				if( IsCaretMode() == false && mKukeiSelected == true && mKukeiSelected_Start.GetItemCount() > 0 )
				{
					ATextPoint	spt, ept;
					GetKukeiContainRange(spt,ept);
					SetSelect(spt,ept);
				}
				mKukeiSelected_Start.DeleteAll();
				mKukeiSelected_End.DeleteAll();
				mKukeiSelected = false;
				mKukeiSelectionMode = false;
			}
			else
			{
				mKukeiSelected_Start.DeleteAll();
				mKukeiSelected_End.DeleteAll();
				mKukeiSelected = false;
				mKukeiSelectionMode = true;
				if( IsCaretMode() == false )
				{
					ATextPoint	spt, ept;
					SPI_GetSelect(spt,ept);
					AImagePoint	imagespt, imageept;
					GetImagePointFromTextPoint(spt,imagespt);
					GetImagePointFromTextPoint(ept,imageept);
					SetKukeiSelect(imagespt,imageept);
/*					
					for( AIndex lineIndex = spt.y; lineIndex <= ept.y; lineIndex++ )
					{
						AImagePoint	ispt = imagespt;
						ispt.y = GetImageYByLineIndex(lineIndex);
						AImagePoint	iept = imageept;
						iept.y = ispt.y;
						ATextPoint	start, end;
						GetTextPointFromImagePoint(ispt,start);
						GetTextPointFromImagePoint(iept,end);
						mKukeiSelected_Start.Add(start);
						mKukeiSelected_End.Add(end);
					}
					mKukeiSelected = true;
					mCaretMode = false;
					NVI_Refresh();*/
				}
			}
			break;
		}
		/*#800
		//��`�y�[�X�g
	  case kMenuItemID_KukeiPaste:
		{
			PasteKukei();
			break;
		}
		*/
		//�s�܂�Ԃ��ʒu�ŉ��s
	  case kMenuItemID_InsertLineEndAtWrap:
		{
			ATextPoint	spt, ept;
			SPI_GetSelect(spt,ept);
			if( spt.y != ept.y )
			{
				GetTextDocument().SPI_InsertLineEndAtWrap(spt.y,ept.y);
			}
			break;
		}
		//Transliterate
	  case kMenuItemID_Transliterate:
		{
			Transliterate(inDynamicMenuActionText);
			/*#1035
			ATextIndex	spos,epos;
			GetSelect(spos,epos);
			AText	origtext, text;
			GetSelectedText(origtext);
			GetApp().SPI_Transliterate(inDynamicMenuActionText,origtext,text);
			TextInput(undoTag_Transliterate,text);
			SetSelect(spos,spos+text.GetItemCount());
			*/
			break;
		}
	  case kMenuItemID_Transliterate_ToLower:
		{
			TransliterateToLower();
			/*#1035
			ATextIndex	spos,epos;
			GetSelect(spos,epos);
			AText	text;
			GetSelectedText(text);
			text.ChangeCaseLower();
			TextInput(undoTag_Transliterate,text);
			SetSelect(spos,spos+text.GetItemCount());
			*/
			break;
		}
	  case kMenuItemID_Transliterate_ToUpper:
		{
			TransliterateToUpper();
			/*#1035
			ATextIndex	spos,epos;
			GetSelect(spos,epos);
			AText	text;
			GetSelectedText(text);
			text.ChangeCaseUpper();
			TextInput(undoTag_Transliterate,text);
			SetSelect(spos,spos+text.GetItemCount());
			*/
			break;
		}
		//��`�ꏊ
	  case kMenuItemID_MoveToDefinition:
		{
			//���݂̈ʒu��Navigate�o�^���� #146
			GetApp().SPI_RegisterNavigationPosition();
			//
			AIndex	index = inDynamicMenuActionText.GetNumber();
			ATextIndex	spos, epos;
			ABool	import;
			AFileAcc	file;
			GetTextDocument().SPI_GetCurrentIdentifierDefinitionRange(index,spos,epos,import,file);
			if( import == false )
			{
				if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
				{
					if( mTextWindowID != kObjectID_Invalid )
					{
						GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SplitView(false);
					}
				}
				SetSelect(spos,epos);
				AdjustScroll_Top(GetCaretTextPoint());
			}
			else
			{
				//#725 GetApp().SPI_SelectTextDocument(file,spos,epos-spos);
				AText	path;
				file.GetPath(path);
				GetApp().SPI_OpenOrRevealTextDocument(path,spos,epos,kObjectID_Invalid,false,kAdjustScrollType_Center);
			}
			break;
		}
		//���@�ĔF��
	  case kMenuItemID_ReRecognize:
		{
			GetTextDocument().SPI_ReRecognizeSyntax();
			break;
		}
		//R0199
	  case kMenuItemID_CorrectSpell:
		{
			if( GetTextDocument().NVI_IsReadOnly() == true )   break;
			if( IsCaretMode() == true )
			{
				SelectWord();
			}
			TextInput(undoTag_Typing,inDynamicMenuActionText);
			break;
		}
		//R0006
	  case kMenuItemID_SCMCompareWithLatest:
		{
			DiffWithRepository();//#379
			break;
		}
	  case kMenuItemID_SCMNextDiff:
		{
			NextDiff();
			break;
		}
	  case kMenuItemID_SCMPrevDiff:
		{
			PrevDiff();
			break;
		}
		//#390
	  case kMenuItemID_KeyRecordStart:
		{
			GetApp().SPI_ShowKeyRecordWindow();
			GetApp().SPI_StartRecord();
			break;
		}
	  case kMenuItemID_KeyRecordStop:
		{
			GetApp().SPI_StopRecord();
			break;
		}
	  case kMenuItemID_KeyRecordPlay:
		{
			SPI_PlayKeyRecord();
			break;
		}
		//#478
	  case kMenuItemID_FigMode_None:
		{
			mFigMode = kFigMode_None;
			break;
		}
	  case kMenuItemID_FigMode_Line:
		{
			mFigMode = kFigMode_Line;
			break;
		}
		//#450
		//�܂肽���݁E�W�J
	  case kMenuItemID_Folding_Collapse:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			SPI_ExpandCollapseAtCurrentLine(spt.y,true);
			break;
		}
	  case kMenuItemID_Folding_Expand:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			SPI_ExpandCollapseAtCurrentLine(spt.y,false);
			break;
		}
	  case kMenuItemID_Folding_CollapseAllAtCurrentLevel:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			SPI_ExpandCollapseAllAtCurrentLevel(spt.y,true,false);
			break;
		}
	  case kMenuItemID_Folding_ExpandAllAtCurrentLevel:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			SPI_ExpandCollapseAllAtCurrentLevel(spt.y,false,false);
			break;
		}
	  case kMenuItemID_Folding_CollapseAllHeaders:
		{
			CollapseAllHeaders();
			break;
		}
	  case kMenuItemID_Folding_CollapseAllHeadersWithNoChange:
		{
			CollapseAllHeadersWithMNoChange();
			break;
		}
	  case kMenuItemID_Folding_ExpandAll:
		{
			ExpandAllFoldings();
			break;
		}
	  case kMenuItemID_Folding_SelectFoldingRange:
		{
			//������
			break;
		}
	  case kMenuItemID_Folding_SetToDefault:
		{
			SPI_SetFoldingToDefault();
			break;
		}
		//���o���ړ�
	  case kMenuItemID_Folding_GoToParent:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			spt.y = FindParentHeader(spt.y);
			ept.y = spt.y +1;
			spt.x = spt.x = 0;
			if( spt.y != kIndex_Invalid )
			{
				SPI_SetSelect(spt,ept);
				SPI_AdjustScroll_Center();
			}
			break;
		}
	  case kMenuItemID_Folding_GoToNext:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			spt.y = FindNextHeader(spt.y);
			ept.y = spt.y +1;
			spt.x = ept.x = 0;
			if( spt.y != kIndex_Invalid )
			{
				SPI_SetSelect(spt,ept);
				SPI_AdjustScroll_Center();
			}
			break;
		}
	  case kMenuItemID_Folding_GoToPrev:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			spt.y = FindPrevHeader(spt.y);
			ept.y = spt.y +1;
			spt.x = ept.x = 0;
			if( spt.y != kIndex_Invalid )
			{
				SPI_SetSelect(spt,ept);
				SPI_AdjustScroll_Center();
			}
			break;
		}
	  case kMenuItemID_Folding_GoToNextAtSameLevel:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			spt.y = FindSameOrHigherLevelNextHeader(spt.y);
			ept.y = spt.y +1;
			spt.x = ept.x = 0;
			if( spt.y != kIndex_Invalid )
			{
				SPI_SetSelect(spt,ept);
				SPI_AdjustScroll_Center();
			}
			break;
		}
	  case kMenuItemID_Folding_GoToPrevAtSameLevel:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			spt.y = FindSameOrHigherLevelPrevHeader(spt.y);
			ept.y = spt.y +1;
			spt.x = ept.x = 0;
			if( spt.y != kIndex_Invalid )
			{
				SPI_SetSelect(spt,ept);
				SPI_AdjustScroll_Center();
			}
			break;
		}
		
		//�R���e�N�X�g���j���[
		//�}���`�t�@�C������
	  case kMenuItemID_CM_MultiFileFind:
		{
			AText	text;
			GetSelectedText(text);
			GetApp().SPI_ShowMultiFileFindWindow(text);
			break;
		}
		//
	  case kMenuItemID_CM_Find:
		{
			AText	text;
			GetSelectedText(text);
			GetApp().SPI_ShowFindWindow(text);
			break;
		}
		//
	  case kMenuItemID_CM_FindNext:
		{
			//Find�p�����[�^�ݒ�
			AFindParameter	param;
			GetSelectedText(param.findText);
			param.regExp = true;
			param.loop = false;
			//�������s
			SPI_FindNext(param);
			break;
		}
		//
	  case kMenuItemID_CM_FindPrevious:
		{
			//Find�p�����[�^�ݒ�
			AFindParameter	param;
			GetSelectedText(param.findText);
			param.regExp = true;
			param.loop = false;
			//�������s
			SPI_FindPrevious(param);
			break;
		}
		//
	  case kMenuItemID_CM_IdInfo:
		{
			//B0432
			if( IsCaretMode() == true )
			{
				SelectWord();
			}
			
			//#291 GetApp().SPI_GetIdInfoWindow().NVI_Show(false);
			/*#853
			GetApp().SPI_ShowHideIdInfoWindow(true);//#291
			AText	text;
			GetSelectedText(text);
			SetIdInfo(text);
			*/
			DisplayIdInfo(true,kIndex_Invalid,kIndex_Invalid);//#853
			break;
		}
#if IMPLEMENTATION_FOR_MACOSX
		//R0231
	  case kMenuItemID_CM_Dictionary:
		{
			//B0432
			if( IsCaretMode() == true )
			{
				SelectWord();
			}
			
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
			{
				DictionaryPopup();
			}
			else
			{
				Dictionary();
			}
			break;
		}
#endif
		/*#858
		//RC3
	  case kMenuItemID_CM_ExternalComment:
		{
			ExternalComment();
			break;
		}
		*/
		//#467 define��`�ݒ�E�C���h�E���J��
	  case kMenuItemID_CM_OpenDefineValueWindow:
		{
			AText	text;
			GetSelectedText(text);
			GetApp().SPI_GetEnabledDefinesWindow().SPI_SetDefineEditBox(text);
			GetApp().SPI_GetEnabledDefinesWindow().NVI_Show();
			break;
		}
		//RC1
	  case kMenuItemID_ArrowToDef:
		{
			GetTextDocument().SPI_SetDrawArrowToDef(!GetTextDocumentConst().SPI_GetDrawArrowToDef());
			NVM_GetWindow().NVI_RefreshWindow();
			break;
		}
		//#564
	  case kMenuItemID_CopyFilePath:
		{
			if( GetTextDocumentConst().NVI_IsFileSpecified() == true )
			{
				AFileAcc	file;
				GetTextDocument().NVI_GetFile(file);
				AText	path;
				file.GetPath(path);
				AScrapWrapper::SetClipboardTextScrap(path,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), false);//#1300
			}
			break;
		}
		//�I��͈͂ƃN���b�v�{�[�h���r
	  case kMenuItemID_CompareSelectionAndClipboard:
		{
			//�N���b�v�{�[�h�擾
			AText	text;
			AScrapWrapper::GetClipboardTextScrap(text);
			//�I��͈͎擾
			AIndex	start = kIndex_Invalid;
			AIndex	end = kIndex_Invalid;
			SPI_GetSelect(start,end);
			//�h�L�������g�ɔ�r���[�h�ݒ�
			GetTextDocument().SPI_SetDiffMode_TextWithRange(text,start,end,kCompareMode_Clipboard);
			//�E�C���h�E���r���[�h�ɐݒ�
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(true);
			}
			break;
		}
		//#112
		//�t�@�C���Ɣ�r
	  case kMenuItemID_CompareWithFile:
		{
			//�t�@�C���I���_�C�A���O�\��
			//�f�t�H���g�t�@�C���F�h�L�������g�̃t�@�C��
			AFileAcc	defaultFile;
			GetTextDocument().NVI_GetFile(defaultFile);
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_CompareFile");
			NVM_GetWindow().NVI_ShowChooseFileWindow(defaultFile,message,AWindow_Text::kVirtualControlID_CompareFileChoosen);
			break;
		}
		//#81
		//��r�Ȃ�
	  case kMenuItemID_NoCompare:
		{
			//�h�L�������g�ɔ�r���[�h�ݒ�
			GetTextDocument().SPI_SetDiffMode_None();
			//�E�C���h�E�̔�r���[�h����
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(false);
			}
			break;
		}
		//�o�b�N�A�b�v�̍��ځi���I���j���[���ځj��I����
	  case kMenuItemID_CompareWithAutoBackup_Monthly:
		{
			//��r��t�@�C���擾
			AFileAcc	diffTargetFile;
			if( GetTextDocument().SPI_GetAutoBackupMonthlyFile(inDynamicMenuActionText,diffTargetFile) == true )
			{
				//�h�L�������g�ɔ�r���[�h�ݒ�
				GetTextDocument().SPI_SetDiffMode_File(diffTargetFile,kCompareMode_AutoBackupMonthly);
				//�E�C���h�E���r���[�h�ɐݒ�
				if( mTextWindowID != kObjectID_Invalid )
				{
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(true);
				}
			}
			break;
		}
	  case kMenuItemID_CompareWithAutoBackup_Daily:
		{
			//��r��t�@�C���擾
			AFileAcc	diffTargetFile;
			if( GetTextDocument().SPI_GetAutoBackupDailyFile(inDynamicMenuActionText,diffTargetFile) == true )
			{
				//�h�L�������g�ɔ�r���[�h�ݒ�
				GetTextDocument().SPI_SetDiffMode_File(diffTargetFile,kCompareMode_AutoBackupDaily);
				//�E�C���h�E���r���[�h�ɐݒ�
				if( mTextWindowID != kObjectID_Invalid )
				{
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(true);
				}
			}
			break;
		}
	  case kMenuItemID_CompareWithAutoBackup_Hourly:
		{
			//��r��t�@�C���擾
			AFileAcc	diffTargetFile;
			if( GetTextDocument().SPI_GetAutoBackupHourlyFile(inDynamicMenuActionText,diffTargetFile) == true )
			{
				//�h�L�������g�ɔ�r���[�h�ݒ�
				GetTextDocument().SPI_SetDiffMode_File(diffTargetFile,kCompareMode_AutoBackupHourly);
				//�E�C���h�E���r���[�h�ɐݒ�
				if( mTextWindowID != kObjectID_Invalid )
				{
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(true);
				}
			}
			break;
		}
	  case kMenuItemID_CompareWithAutoBackup_Minutely:
		{
			//��r��t�@�C���擾
			AFileAcc	diffTargetFile;
			if( GetTextDocument().SPI_GetAutoBackupMinutelyFile(inDynamicMenuActionText,diffTargetFile) == true )
			{
				//�h�L�������g�ɔ�r���[�h�ݒ�
				GetTextDocument().SPI_SetDiffMode_File(diffTargetFile,kCompareMode_AutoBackupMinutely);
				//�E�C���h�E���r���[�h�ɐݒ�
				if( mTextWindowID != kObjectID_Invalid )
				{
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(true);
				}
			}
			break;
		}
	  case kMenuItemID_CompareWithDailyBackupByDefault:
		{
			//������
			break;
		}
		//��r�i��r���[�h���肩���j
	  case kMenuItemID_Compare:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).
						SPI_SetDiffDisplayMode(!(GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetDiffDisplayMode()));
			}
			break;
		}
		//Speech #851
	  case kMenuItemID_StartSpeaking:
		{
			AText	text;
			GetSelectedText(text);
			//�I�𒆂łȂ��ꍇ�͂��ׂăX�s�[�` #1313
			if( text.GetItemCount() == 0 )
			{
				GetTextDocumentConst().SPI_GetText(text);
			}
			//
			GetApp().NVI_SpeakText(text);
			break;
		}
	  case kMenuItemID_StopSpeaking:
		{
			GetApp().NVI_StopSpeaking();
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

//
void	AView_Text::OpenSelected( const AModifierKeys inModifierKeys )
{
	AText	path;
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	GetTextDocumentConst().SPI_GetTextForOpenSelected(spt,ept,path);
	//
	AIndex	foundpos;
	if( path.FindCstring(0,"://",foundpos) == true )
	{
		ALaunchWrapper::OpenURL(path);
		return;
	}
	//
	if( GetTextDocumentConst().SPI_IsRemoteFileMode() == true )
	{
		//
		if( path.GetItemCount() > 0 )
		{
			AText	url;
			GetTextDocumentConst().SPI_GetRemoteFileURL(url);
			url.CatFTPURLPathText(path);
			GetApp().SPI_RequestOpenFileToRemoteConnection(url,false);//#361
		}
	}
	else 
	{
		//B0389 OK �t���p�X�ŊJ��
		AFileAcc	fullpathfile;
		fullpathfile.Specify(path);
		if( fullpathfile.Exist() == true && fullpathfile.IsFolder() == false )
		{
			GetApp().GetAppPref().LaunchAppWithFile(fullpathfile,inModifierKeys);
			return;
		}
		
		if( GetTextDocumentConst().NVI_IsFileSpecified() == true )
		{
			//
			AFileAcc	docfile;
			GetTextDocumentConst().NVI_GetFile(docfile);
			AFileAcc	folder;
			folder.SpecifyParent(docfile);
			//
			AFileAcc	file;
			file.SpecifyChild(folder,path);//B0389 OK ,kFilePathType_1);kFilePathType_Default�ł�../.���߂ł���̂Ŗ��Ȃ��B
			if( file.Exist() == true && file.IsFolder() == false )
			{
				GetApp().GetAppPref().LaunchAppWithFile(file,inModifierKeys);
				return;
			}
			//
			AText	text;
			docfile.GetPath(text);
			AObjectArray<AFileAcc>	candidate;
			GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetOpenSelectedFileCandidate(text,candidate);
			if( candidate.GetItemCount() > 0 )
			{
				//GetApp().SPNVI_CreateDocumentFromLocalFile(candidate.GetObject(0));
				GetApp().GetAppPref().LaunchAppWithFile(candidate.GetObject(0),inModifierKeys);
			}
			//R0000 �ŏ��̃C���|�[�g�t�@�C�����J��
			else
			{
				if( GetTextDocumentConst().SPI_GetImportFileArray().GetItemCount() > 0 )
				{
					GetApp().GetAppPref().LaunchAppWithFile(GetTextDocumentConst().SPI_GetImportFileArray().GetObjectConst(0),inModifierKeys);
				}
			}
		}
	}
}

//
void	AView_Text::EVTDO_UpdateMenu()
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetAskingSaveChanges() == true )   return;
		if( GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_IsPrintMode() == true )   return;
	}
	
	ABool	b;
	AText	text;
	
	//Undo
	b = GetTextDocument().SPI_GetUndoText(text);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Undo,b);
	GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_Undo,text);
	//Redo
	b = GetTextDocument().SPI_GetRedoText(text);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Redo,b);
	GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_Redo,text);
	//Save
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Save,GetTextDocument().NVI_IsDirty());
	//SaveAs
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SaveAs,true);
	//��� #524
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Print,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_PrintRange,(IsCaretMode()==false));
	//�R�s�[
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Copy,(IsCaretMode()==false));
	//�ǉ��R�s�[
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_AdditionalCopy,(IsCaretMode()==false)&&
				(GetApp().SPI_IsFIFOClipboardMode()==false));//#878 FIFO�N���b�v�{�[�h���[�h���͒ǉ��R�s�[�s��
	//�J�b�g
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Cut,((IsCaretMode()==false)&&(GetTextDocument().NVI_IsReadOnly()==false)));
	//�y�[�X�g
	if( GetApp().SPI_IsFIFOClipboardMode() == false )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Paste,
					((GetTextDocument().NVI_IsReadOnly()==false)&&(AScrapWrapper::ExistClipboardTextScrapData()==true)));
		//#929
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_PasteAtEndOfEachLine,
														((GetTextDocument().NVI_IsReadOnly()==false)&&(AScrapWrapper::ExistClipboardTextScrapData()==true)));
	}
	//#878 FIFO�N���b�v���[�h���̃y�[�X�g
	else
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Paste,
					((GetTextDocument().NVI_IsReadOnly()==false)&&
						(GetApp().SPI_GetFIFOClipboardCount()>0||AScrapWrapper::ExistClipboardTextScrapData()==true)));
		//#929
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_PasteAtEndOfEachLine,false);
	}
	//�폜
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Clear,((IsCaretMode()==false)&&(GetTextDocument().NVI_IsReadOnly()==false)));
	//�S�Ă�I��
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SelectAll,true);
	//�C���f���g
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Indent,(GetTextDocument().NVI_IsReadOnly()==false));
	//�E�V�t�g
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShiftRight,(GetTextDocument().NVI_IsReadOnly()==false));
	//���V�t�g
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShiftLeft,(GetTextDocument().NVI_IsReadOnly()==false));
	//�o�b�N�X���b�V������
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_InputBackslashYen,(GetTextDocumentConst().SPI_IsJISTextEncoding() == false));//#940
		AText	text;
		if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey) == true )
		{
			text.SetLocalizedText("InputBackslash_InputYen");
		}
		else
		{
			text.SetLocalizedText("InputBackslash_InputBackslash");
#if IMPLEMENTATION_FOR_WINDOWS
			AText	t("Backslash");
			text.ReplaceChar(kUChar_Backslash,t);
#endif
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_InputBackslashYen,text);
	}
	
	//����
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	b = (param.findText.GetItemCount()>0);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindNext,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindPrevious,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindFromFirst,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindList,b);
	b = ((GetTextDocumentConst().NVI_IsReadOnly()==false) && (IsCaretMode() == false) && (param.findText.GetItemCount()>0));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceSelectedTextOnly,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceAndFind,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceInSelection,b);
	b = ((GetTextDocumentConst().NVI_IsReadOnly()==false) && (param.findText.GetItemCount()>0));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceNext,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceAfterCaret,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceAll,b);
	b = (IsCaretMode()==false);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetFindText,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetReplaceText,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_EraseFindHighlight,GetTextDocument().SPI_IsFindHighlightSet());
	
	//�I�𕔕����J��
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_OpenSelected,true);
	//������
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Bookmark,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_MoveToBookmark,(GetTextDocument().SPI_GetBookmarkLineIndex()!=kIndex_Invalid));
	//�Ή����������
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CorrespondText,(GetTextDocument().NVI_IsReadOnly()==false));
	//#359
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SelectBetweenCorrespondText,true);
	//�c�[�����̓��[�h
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KeyToolMode,true);
	//�c�[��
	//#129 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Tool,true);
	//��`�I�����[�h
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KukeiSelectionMode,true);
		AText	text;
		if( mKukeiSelectionMode == true )
		{
			text.SetLocalizedText("RectSelect_Off");
		}
		else
		{
			text.SetLocalizedText("RectSelect_On");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_KukeiSelectionMode,text);
	}
	
	/*#800
	//��`�y�[�X�g
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KukeiPaste,
			((GetTextDocument().NVI_IsReadOnly()==false)&&(AScrapWrapper::ExistClipboardTextScrapData()==true)));
			*/
	//�s�܂�Ԃ��ʒu�ŉ��s
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_InsertLineEndAtWrap,(GetTextDocument().NVI_IsReadOnly()==false));
	//Transliterate
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Transliterate,(GetTextDocument().NVI_IsReadOnly()==false)&&(IsCaretMode()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Transliterate_ToLower,(GetTextDocument().NVI_IsReadOnly()==false)&&(IsCaretMode()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Transliterate_ToUpper,(GetTextDocument().NVI_IsReadOnly()==false)&&(IsCaretMode()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_TransliterateRootMenu,(GetTextDocument().NVI_IsReadOnly()==false)&&(IsCaretMode()==false));
	
	//�W�����v��
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_JumpNextItem,true);
	//�W�����v�O
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_JumpPreviousItem,true);
	//
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Jump,true);
	
	//���@�ĔF��
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReRecognize,true);
	
	//#129 Enable/Disable�X�V
	//#129 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Tool_StandardMode,true);
	//#129 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Tool,true);
	GetApp().NVI_GetMenuManager().UpdateDynamicMenuEnableMenuItem(kMenuItemID_Tool_StandardMode);//#129
	GetApp().NVI_GetMenuManager().UpdateDynamicMenuEnableMenuItem(kMenuItemID_Tool);//#129
	
	//RC1
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ArrowToDef,true);
	
	//#390
	ABool	keyRecording = GetApp().SPI_IsKeyRecording();
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KeyRecordStart,(keyRecording==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KeyRecordStop,(keyRecording==true));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KeyRecordPlay,true);
	
	//#478
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FigMode_None,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FigMode_Line,true);
	
	//#564
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CopyFilePath,
				(GetTextDocumentConst().NVI_IsFileSpecified()==true));
	
	//#450
	//�܂肽���݊֘A
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_Collapse,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_Expand,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_CollapseAllAtCurrentLevel,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_ExpandAllAtCurrentLevel,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_CollapseAllHeaders,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_CollapseAllHeadersWithNoChange,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_ExpandAll,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_GoToParent,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_GoToNext,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_GoToPrev,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_GoToNextAtSameLevel,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_GoToPrevAtSameLevel,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_SelectFoldingRange,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_SetToDefault,true);
	
	//#878
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchFIFOClipboardMode,true);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SwitchFIFOClipboardMode,GetApp().SPI_IsFIFOClipboardMode());
	
	//------------------��r�֘A------------------
	//R0006
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
	{
		AFileAcc	f;//#379
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SCMCompareWithLatest,
					GetTextDocumentConst().SPI_CanDiffWithRepositiory() &&
					((GetTextDocumentConst().SPI_IsRemoteFileMode()==false)&&(GetTextDocumentConst().NVI_IsFileSpecified()==true)));
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SCMNextDiff,true);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SCMPrevDiff,true);
	}
	
	//
	ABool	wrapCalculateCompleted = (GetTextDocumentConst().SPI_IsWrapCalculating()==false);
	//#737
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareSelectionAndClipboard,
													((AScrapWrapper::ExistClipboardTextScrapData()==true)&&wrapCalculateCompleted));
	//#81
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_NoCompare,wrapCalculateCompleted);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithFile,wrapCalculateCompleted);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithAutoBackup,
													(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kEnableAutoBackup)&&
													 wrapCalculateCompleted));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithAutoBackup_Monthly,wrapCalculateCompleted);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithAutoBackup_Daily,wrapCalculateCompleted);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithAutoBackup_Hourly,wrapCalculateCompleted);
	//GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithDailyBackupByDefault,true);
	//
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_NoCompare,false);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SCMCompareWithLatest,false);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithFile,false);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareSelectionAndClipboard,false);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithAutoBackup,false);
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Minutely,GetEmptyText());
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Hourly,GetEmptyText());
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Daily,GetEmptyText());
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Monthly,GetEmptyText());
	AText	filename;
	switch(GetTextDocumentConst().SPI_GetCompareMode(filename))
	{
	  case kCompareMode_NoCompare:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_NoCompare,true);
			break;
		}
	  case kCompareMode_Repository:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SCMCompareWithLatest,true);
			break;
		}
	  case kCompareMode_File:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithFile,true);
			break;
		}
	  case kCompareMode_Clipboard:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareSelectionAndClipboard,true);
			break;
		}
	  case kCompareMode_AutoBackupMinutely:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithAutoBackup,true);
			GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Minutely,filename);
			break;
		}
	  case kCompareMode_AutoBackupHourly:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithAutoBackup,true);
			GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Hourly,filename);
			break;
		}
	  case kCompareMode_AutoBackupDaily:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithAutoBackup,true);
			GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Daily,filename);
			break;
		}
	  case kCompareMode_AutoBackupMonthly:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithAutoBackup,true);
			GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Monthly,filename);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	//��r
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Compare,(GetTextDocumentConst().SPI_GetDiffTargetDocumentID() != kObjectID_Invalid));
	//Speech #851
	/*#1313
	AText	selectedText;
	GetSelectedText(selectedText);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_StartSpeaking,(selectedText.GetItemCount()>0));
	*/
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_StartSpeaking,true);//#1313
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_StopSpeaking,GetApp().NVI_IsSpeaking());
}

//R0246
ABool	AView_Text::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	/*#688
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_HideHelpTag();
	}
	*/
	/*#233 �e�X�g
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	mouseTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,mouseTextPoint);
	if( mHoverCaretTextPoint.x != mouseTextPoint.x || mHoverCaretTextPoint.y != mouseTextPoint.y )
	{
		XorHoverCaret(mHoverCaretTextPoint);
		//
		mHoverCaretTextPoint = mouseTextPoint;
		XorHoverCaret(mHoverCaretTextPoint);
	}
	*/
	
	//#853
	//�L�[���[�h��� �����n�C���C�g
	AImagePoint	hoverImagePoint = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,hoverImagePoint);
	ATextPoint	hoverTextPoint = {0};
	GetTextPointFromImagePoint(hoverImagePoint,hoverTextPoint);
	IdInfoHighlightArg(GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(hoverTextPoint));
	
	return false;
}

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_Text::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	return SPI_DoMouseDown(inLocalPoint,inModifierKeys,inClickCount,false);
}
ABool	AView_Text::SPI_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount,
									const ABool inClickedAtLineNumberArea )
{
	RestoreFromBraceCheck();
	//�⊮���͏�ԉ���
	ClearAbbrevInputMode(true,false);//RC2
	//
	EndKeyToolMode();//R0073
	
	//�I��\������� #699
	CancelReservedSelection();
	
	//fprintf(stderr,"EVTDO_DoMouseDown() ");
	//��View��Active�łȂ��ꍇ�́AWindow�փt�H�[�J�X�̈ړ����˗�����
	if( NVI_IsFocusActive() == false )
	{
		NVM_GetWindow().NVI_SwitchFocusTo(NVI_GetControlID());
	}
	
	if( inClickCount == 1 )
	{
		DoMouseDownSingleClick(inLocalPoint,inModifierKeys,inClickedAtLineNumberArea);
	}
	else if( inClickCount == 2 )
	{
		DoMouseDownDoubleClick(inLocalPoint,inModifierKeys);
	}
	else if( inClickCount == 3 )
	{
		DoMouseDownTripleClick(inLocalPoint,inModifierKeys,inClickedAtLineNumberArea);
	}
	else if( inClickCount == 4 && 
	GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kSelectInsideByQuadClick) == true )
	{
		DoMouseDownQuadClick(inLocalPoint,inModifierKeys);
	}
	
	//win Caret������΍�i�r���ŕ`�悹���ɁA�����ł܂Ƃ߂ĕ\���j
	//Caret�\���\�񂪂���Ε\������
	ShowCaretIfReserved();
	
	return false;
}

//�}�E�X�{�^���������̃R�[���o�b�N(AView�p)
ABool	AView_Text::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	RestoreFromBraceCheck();
	//�⊮���͏�ԉ���
	ClearAbbrevInputMode(true,false);//RC2
	//
	DoContextMenu(inLocalPoint,inModifierKeys);
	return true;
}

//�}�E�X�N���b�N�i�V���O���N���b�N�j
void	AView_Text::DoMouseDownSingleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
										   const ABool inInsideLineNumber )
{
	//#606 ��̂Ђ�c�[��
	if( IsHandToolMode() == true )
	{
		DoMouseDownTenohira(inLocalPoint,inModifierKeys);
		return;
	}
	
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	//�s�ԍ��N���b�N�̏ꍇ�́Aimage�̍��[���N���b�N�������Ƃɂ���
	if( inInsideLineNumber == true )
	{
		clickImagePoint.x = 0;
	}
	
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	
	//#478 �ŏ��̃N���b�N�ʒu���L��
	mMouseDownImagePoint = clickImagePoint;
	mMouseDownTick = GetApp().SPI_GetAppElapsedTick();
	
	//�I��͈͓��N���b�N�iDrag�܂��̓L�����b�g�ݒ�j
	if( IsInSelect(clickImagePoint,true) == true )
	{
		//#688 if( mMouseTrackByLoop == false )
		//#688{
		//TrackingMode�ݒ�
		mMouseTrackingMode = kMouseTrackingMode_SingleClickInSelect;
		mMouseTrackStartImagePoint = clickImagePoint;
		mMouseTrackImagePoint = clickImagePoint;
		mMouseTrackResultIsDrag = false;
		NVM_SetMouseTrackingMode(true);
		return;
		//#688}
		/*#688 
		//�ŏ��̃}�E�X�ʒu�擾�i���̎��_�Ń}�E�X�{�^����������Ă����烊�^�[���BTrackMouseDown()�Ń}�E�X�{�^�������[�X�̃C�x���g���擾������A���͂������Ȃ��BB0260�j
		AWindowPoint	mouseWindowPoint;
		AModifierKeys	modifiers;
		if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
		
		AWindowPoint	firstMouseWindowPoint = mouseWindowPoint;
		mMouseTrackResultIsDrag = false;
		while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
		{
			if( firstMouseWindowPoint.x != mouseWindowPoint.x || firstMouseWindowPoint.y != mouseWindowPoint.y )
			{
				//�}�E�X�{�^���������ςȂ��Ń}�E�X�ʒu�������ł��ړ�������Drag
				mMouseTrackResultIsDrag = true;
				break;
			}
		}
		
		//Image���W�ɕϊ�
		ALocalPoint	mouseLocalPoint;
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		AImagePoint	mouseImagePoint;
		NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
		
		//Drag�J�n�܂��̓L�����b�g�ݒ�
		DoMouseTrackEnd_SingleClickInSelect(mouseImagePoint,0);
		
		return;
		*/
	}
	
	//�L�����b�g�ݒ�
	if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true )
	{
		SetSelectTextPoint(clickTextPoint);
	}
	else
	{
		SetCaretTextPoint(clickTextPoint);
		ShowCaret();
		/*#725 InfoHeader�p�~
		//R0214
		if( false )
		{
			ATextIndex	pos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			AText	word;
			ATextIndex	spos = pos, epos = pos;
			GetTextDocument().SPI_FindWord(pos,spos,epos);
			GetTextDocument().SPI_GetText(spos,epos,word);
			SetInfoHeader(word);
		}
		*/
	}
	
	//�}�E�X�h���b�O�ɂ��I��
	
	//#688 if( mMouseTrackByLoop == false )
	//#688{
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
	mMouseTrackImagePoint = clickImagePoint;
	NVM_SetMouseTrackingMode(true);
	return;
	//#688}
	/*#688
	//�ŏ��̃}�E�X�ʒu�擾�i���̎��_�Ń}�E�X�{�^����������Ă����烊�^�[���BTrackMouseDown()�Ń}�E�X�{�^�������[�X�̃C�x���g���擾������A���͂������Ȃ��BB0260�j
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
	
	//Image���W�ɕϊ�
	ALocalPoint	mouseLocalPoint;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
	//�O��}�E�X�ʒu�̕ۑ�
	AImagePoint	previousImagePoint = mouseImagePoint;
	//
	ATickCount	firsttickcount = ATimerWrapper::GetTickCount();
	ABool	sameposasfirst = true;
	//�}�E�X�{�^�����������܂Ń��[�v
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//Image���W�ɕϊ�
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
		//CM
		if( sameposasfirst == true )
		{
			if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCMMouseButton) == true &&
			ATimerWrapper::GetTickCount() > firsttickcount + GetApp().GetAppPref().GetData_Number(AAppPrefDB::kCMMouseButtonTick) )
			{
				DoContextMenu(mouseLocalPoint,inModifierKeys);
				return;
			}
		}
		//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
		if( mouseImagePoint.x == previousImagePoint.x && mouseImagePoint.y == previousImagePoint.y )
		{
			continue;
		}
		sameposasfirst = false;
		
		//Mouse Track
		DoMouseTrack_SingleClick(mouseImagePoint,modifiers);
		
		//�O��}�E�X�ʒu�̕ۑ�
		previousImagePoint = mouseImagePoint;
	}
	DoMouseTrackEnd_SingleClick(mouseImagePoint,modifiers);
	*/
}

//Mouse Track
void	AView_Text::DoMouseTrack_SingleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//�\���t���[����Image���W�Ŏ擾
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(Y����)
		if( mouseImagePoint.y < (frameImageRect.top+kDragScrollMargin) )//#456
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.top+kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > (frameImageRect.bottom-kDragScrollMargin) )//#456
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.bottom-kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.bottom - 1;
			}
		}
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(X����)
		if( mouseImagePoint.x < frameImageRect.left )
		{
			if( frameImageRect.left > 0 )
			{
				NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.left - NVI_GetHScrollBarUnit() + 1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.left;
			}
		}
		else if( mouseImagePoint.x > frameImageRect.right )
		{
			if( frameImageRect.right < NVM_GetImageWidth() )
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.right + NVI_GetHScrollBarUnit() -1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.right;
			}
		}
		
		switch(mFigMode)
		{
		  case kFigMode_None:
			{
				//�e�L�X�g�I��
				if( mKukeiSelectionMode == false && 
				   (((inModifierKeys&kModifierKeysMask_Option)==0)/*&&((inModifierKeys&kModifierKeysMask_Shift)==0)*/) )//#795
				{
					ATextPoint	selectTextPoint;
					GetTextPointFromImagePoint(mouseImagePoint,selectTextPoint);
					SetSelectTextPoint(selectTextPoint);
				}
				else
				{
					SetKukeiSelect(mCaretImagePoint,mouseImagePoint);
				}
				break;
			}
			//#478
		  case kFigMode_Line:
			{
				
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
}

//#478
/**
*/
void	AView_Text::DoMouseTrackEnd_SingleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	switch(mFigMode)
	{
	  case kFigMode_None:
		{
			//���@�F���ƃ��[�J���͈͐ݒ�
			TriggerSyntaxRecognize();
			/*#853 �N���b�N�͂��̒P��̃L�[���[�h���\���Ƃ���B�������̓z�o�[�ŕ\���B
			EVTDO_DoMouseTrackEnd()�ŏ�������B
			//���ʂ����E�C���h�E�����A���_�[���C���\��
			if( GetApp().SPI_GetIdInfoWindow().NVI_IsWindowVisible() == true )
			{
				IdInfoAutoArg();
			}
			*/
			break;
		}
	  case kFigMode_Line:
		{
			FigMode_DrawLine(mMouseDownImagePoint,inMouseImagePoint);
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//�I��͈͓��N���b�N��̃}�E�X�g���b�N�I���i�}�E�X�{�^��Up�܂��̓h���b�O�J�n�j
void	AView_Text::DoMouseTrackEnd_SingleClickInSelect( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	ALocalPoint	localPoint;
	NVM_GetLocalPointFromImagePoint(inMouseImagePoint,localPoint);
	ATextPoint	textPoint;
	GetTextPointFromImagePoint(inMouseImagePoint,textPoint);
	if( mMouseTrackResultIsDrag == true )
	{
		//�}�E�X�{�^���������Ƀ}�E�X�𓮂������ꍇ
		//Drag�̈�ݒ�
		AArray<ALocalRect>	dragRect;
		if( mKukeiSelected == false )
		{
			//�ʏ�I���h���b�O�̏ꍇ�i��`�h���b�O�ł͂Ȃ��ꍇ�j
			ATextPoint	spt, ept;
			SPI_GetSelect(spt,ept);
			AImageRect	viewImageRect;
			NVM_GetImageViewRect(viewImageRect);
			if( spt.y < GetLineIndexByImageY(viewImageRect.top) )   spt.y = GetLineIndexByImageY(viewImageRect.top);
			if( ept.y > GetLineIndexByImageY(viewImageRect.bottom) )   ept.y = GetLineIndexByImageY(viewImageRect.bottom);
			AIndex	sx = spt.x;
			for( AIndex y = spt.y; y <= ept.y; y++ )
			{
				AIndex	ex = 0;
				if( y == ept.y )
				{
					ex = ept.x;
				}
				ATextPoint	s, e;
				s.x = sx;
				s.y = y;
				e.x = ex;
				e.y = y;
				AImagePoint	imageStartPoint, imageEndPoint;
				GetImagePointFromTextPoint(s,imageStartPoint);
				GetImagePointFromTextPoint(e,imageEndPoint);
				AImageRect	imageRect;
				imageRect.left		= imageStartPoint.x;
				imageRect.top		= imageStartPoint.y;
				imageRect.right		= imageEndPoint.x;
				imageRect.bottom	= imageEndPoint.y + GetLineHeightWithMargin(y);//#450 GetLineHeightWithMargin();
				if( y != ept.y )
				{
					imageRect.right = viewImageRect.right;
				}
				ALocalRect	localRect;
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				dragRect.Add(localRect);
				sx = 0;
			}
		}
		else
		{
			//��`�I���h���b�O�̏ꍇ
			for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
			{
				ATextPoint	spt = mKukeiSelected_Start.Get(i);
				ATextPoint	ept = mKukeiSelected_End.Get(i);
				AImagePoint	imageStartPoint, imageEndPoint;
				GetImagePointFromTextPoint(spt,imageStartPoint);
				GetImagePointFromTextPoint(ept,imageEndPoint);
				AImageRect	imageRect;
				imageRect.left		= imageStartPoint.x;
				imageRect.top		= imageStartPoint.y;
				imageRect.right		= imageEndPoint.x;
				imageRect.bottom	= imageEndPoint.y + GetLineHeightWithMargin(ept.y);//#450 GetLineHeightWithMargin();
				ALocalRect	localRect;
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				dragRect.Add(localRect);
			}
		}
		//Scrap�ݒ�
		AArray<AScrapType>	scrapType;
		ATextArray	data;
		//�I�𒆂̃e�L�X�g�擾
		AText	text;
		GetSelectedText(text);
		//#688
		//Scrap type�w��
		scrapType.Add(kScrapType_UnicodeText);
		data.Add(text);
		/*#688
		//UTF16�ł�Scrap�ǉ�
		AText	utf16text;
		utf16text.SetText(text);
		utf16text.ConvertFromUTF8ToUTF16();
		scrapType.Add(kScrapType_UnicodeText);
		data.Add(utf16text);
		//���K�V�[�e�L�X�g�G���R�[�f�B���O�ł�Scrap�ǉ�
		AText	legacyText;
		legacyText.SetText(text);
		legacyText.ConvertFromUTF8(GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding());//win
		//win ATextEncodingWrapper::GetLegacyTextEncodingFromFont(GetTextDocumentConst().SPI_GetFont()));//B0337
		scrapType.Add(kScrapType_Text);
		data.Add(legacyText);
		*/
		//Drag���s
		mDragging = true;//B0347
		NVMC_DragText(localPoint,dragRect,scrapType,data);
		mDragging = false;//B0347
		//�S�~������F���Ή�����
	}
	else
	{
		//�L�����b�g�ݒ�
		SetCaretTextPoint(textPoint);
		ShowCaret();
	}
}

//�}�E�X�N���b�N�i�_�u���N���b�N�j
void	AView_Text::DoMouseDownDoubleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	
	//�P��I��
	mClickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
	ATextIndex	wordStart, wordEnd;
	GetTextDocument().SPI_FindWord(mClickTextIndex,wordStart,wordEnd);
	GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,mFirstSelectStart,true);
	GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,mFirstSelectEnd,false);
	//���ʑI��
	ABool	braceSelected = DoubleClickBrace(mFirstSelectStart,mFirstSelectEnd);//#688
	//
	SetSelect(mFirstSelectStart,mFirstSelectEnd);
	//#688
	//�_�u���N���b�N�ɂ�銇�ʑI�����̓h���b�O�I�������A�����ŏI��
	if( braceSelected == true )
	{
		return;
	}
	
	/*
	//#450
	//�܂肽���ݔg�����N���b�N�Ȃ�s�W�J
	if( IsMouseInFoldingCollapsedLine(inLocalPoint) == true )
	{
		SPI_ExpandCollapse(clickTextPoint.y,kExpandCollapseType_Expand,true);
		return;
	}
	*/
	
	//�P��擾
	AText	word;
	GetTextDocument().SPI_GetText(wordStart,wordEnd,word);
	/*#725 InfoHeader�p�~
	if( GetApp().SPI_GetIdInfoWindow().NVI_IsWindowVisible() == true )//RC2
	{
		SetIdInfo(word);//RC2
	}
	else
	{
		SetInfoHeader(word);
	}
	*/
	//��`�ꏊ���j���[�X�V
	GetTextDocument().SPI_UpdateCurrentIdentifierDefinitionList(word);
	
	//�}�E�X�h���b�O�ɂ��I��
	
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_DoubleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
	mMouseTrackImagePoint = clickImagePoint;
	NVM_SetMouseTrackingMode(true);
	return;
	//#688}
	/*#688
	//�ŏ��̃}�E�X�ʒu�擾�i���̎��_�Ń}�E�X�{�^����������Ă����烊�^�[���BTrackMouseDown()�Ń}�E�X�{�^�������[�X�̃C�x���g���擾������A���͂������Ȃ��BB0260�j
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
	//Image���W�ɕϊ�
	ALocalPoint	mouseLocalPoint;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
	//�O��}�E�X�ʒu�̕ۑ�
	AImagePoint	previousImagePoint = mouseImagePoint;
	//�}�E�X�{�^�����������܂Ń��[�v
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//Image���W�ɕϊ�
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
		//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
		if( mouseImagePoint.x == previousImagePoint.x && mouseImagePoint.y == previousImagePoint.y )
		{
			continue;
		}
		
		//Mouse Track
		DoMouseTrack_DoubleClick(mouseImagePoint,modifiers);
		
		//�O��}�E�X�ʒu�̕ۑ�
		previousImagePoint = mouseImagePoint;
	}
	*/
}

//Mouse Track (�_�u���N���b�N)
void	AView_Text::DoMouseTrack_DoubleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//�\���t���[����Image���W�Ŏ擾
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(Y����)
		if( mouseImagePoint.y < (frameImageRect.top+kDragScrollMargin) )//#456
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.top+kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > (frameImageRect.bottom-kDragScrollMargin) )//#456
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.bottom-kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.bottom - 1;
			}
		}
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(X����)
		if( mouseImagePoint.x < frameImageRect.left )
		{
			if( frameImageRect.left > 0 )
			{
				NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.left - NVI_GetHScrollBarUnit() + 1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.left;
			}
		}
		else if( mouseImagePoint.x > frameImageRect.right )
		{
			if( frameImageRect.right < NVM_GetImageWidth() )
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.right + NVI_GetHScrollBarUnit() -1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.right;
			}
		}
		
		//Drag�ɂ��P��I��
		ATextPoint	selectTextPoint;
		GetTextPointFromImagePoint(mouseImagePoint,selectTextPoint);
		ATextIndex	selectTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(selectTextPoint);
		ATextIndex	wordStart, wordEnd;
		GetTextDocument().SPI_FindWord(selectTextIndex,wordStart,wordEnd);
		ATextPoint	wordStartPoint, wordEndPoint;
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,wordStartPoint,true);
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,wordEndPoint,false);
		if( wordEnd <= mClickTextIndex )
		{
			if( GetCaretTextPoint().x != mFirstSelectEnd.x || GetCaretTextPoint().y != mFirstSelectEnd.y )
			{
				SetCaretTextPoint(mFirstSelectEnd);
			}
			SetSelectTextPoint(wordStartPoint);
		}
		else
		{
			if( GetCaretTextPoint().x != mFirstSelectStart.x || GetCaretTextPoint().y != mFirstSelectStart.y )
			{
				SetCaretTextPoint(mFirstSelectStart);
			}
			SetSelectTextPoint(wordEndPoint);
		}
	}
}

//�}�E�X�N���b�N�iTriple�N���b�N�j
void	AView_Text::DoMouseDownTripleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
										   const ABool inInsideLineNumber )
{
	//
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectParagraphByTripleClick) == false || 
	   inInsideLineNumber == true )
	{
		//�s�I��
		mFirstSelectStart.x = 0;
		mFirstSelectStart.y = clickTextPoint.y;
		if( clickTextPoint.y+1 >= GetTextDocumentConst().SPI_GetLineCount() )
		{
			mFirstSelectEnd.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(clickTextPoint.y);
			mFirstSelectEnd.y = clickTextPoint.y;
		}
		else
		{
			mFirstSelectEnd.x = 0;
			mFirstSelectEnd.y = clickTextPoint.y+1;
		}
	}
	else //#222
	{
		//�i���I��
		AIndex	paraIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(clickTextPoint.y);
		mFirstSelectStart.x = 0;
		mFirstSelectStart.y = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(paraIndex);
		if( paraIndex+1 >= GetTextDocumentConst().SPI_GetParagraphCount() )
		{
			mFirstSelectEnd.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(GetTextDocumentConst().SPI_GetLineCount()-1);
			mFirstSelectEnd.y = GetTextDocumentConst().SPI_GetLineCount()-1;
		}
		else
		{
			mFirstSelectEnd.x = 0;
			mFirstSelectEnd.y = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(paraIndex+1);
		}
	}
	SetSelect(mFirstSelectStart,mFirstSelectEnd);
	mClickTextLineIndex = clickTextPoint.y;
	
	//�}�E�X�h���b�O�ɂ��I��
	
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_TripleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
	mMouseTrackImagePoint = clickImagePoint;
	NVM_SetMouseTrackingMode(true);
	return;
	//#688 }
	/*#688
	//�ŏ��̃}�E�X�ʒu�擾
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers);
	//Image���W�ɕϊ�
	ALocalPoint	mouseLocalPoint;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
	//�O��}�E�X�ʒu�̕ۑ�
	AImagePoint	previousImagePoint = mouseImagePoint;
	//�}�E�X�{�^�����������܂Ń��[�v
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//Image���W�ɕϊ�
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
		//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
		if( mouseImagePoint.x == previousImagePoint.x && mouseImagePoint.y == previousImagePoint.y )
		{
			continue;
		}
		
		//Mouse Track
		DoMouseTrack_TripleClick(mouseImagePoint,modifiers);
		
		//�O��}�E�X�ʒu�̕ۑ�
		previousImagePoint = mouseImagePoint;
	}
	*/
}

//Mouse Track (�g���v���N���b�N)
void	AView_Text::DoMouseTrack_TripleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//�\���t���[����Image���W�Ŏ擾
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(Y����)
		if( mouseImagePoint.y < (frameImageRect.top+kDragScrollMargin) )//#456
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.top+kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > (frameImageRect.bottom-kDragScrollMargin) )//#456
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.bottom-kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.bottom - 1;
			}
		}
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(X����)
		if( mouseImagePoint.x < frameImageRect.left )
		{
			if( frameImageRect.left > 0 )
			{
				NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.left - NVI_GetHScrollBarUnit() + 1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.left;
			}
		}
		else if( mouseImagePoint.x > frameImageRect.right )
		{
			if( frameImageRect.right < NVM_GetImageWidth() )
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.right + NVI_GetHScrollBarUnit() -1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.right;
			}
		}
		
		//Drag�ɂ��s�I��
		ATextPoint	selectTextPoint;
		GetTextPointFromImagePoint(mouseImagePoint,selectTextPoint);
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectParagraphByTripleClick) == false || 
		   mCurrentMouseTrackInsideLineNumber == true )
		{
			//==================�s�I��==================
			if( selectTextPoint.y < mClickTextLineIndex )
			{
				ATextPoint	pt;
				pt.x = 0;
				pt.y = selectTextPoint.y;
				if( GetCaretTextPoint().x != mFirstSelectEnd.x || GetCaretTextPoint().y != mFirstSelectEnd.y )
				{
					SetCaretTextPoint(mFirstSelectEnd);
				}
				SetSelectTextPoint(pt);
			}
			else
			{
				ATextPoint	pt;
				if( selectTextPoint.y+1 >= GetTextDocumentConst().SPI_GetLineCount() )
				{
					pt.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(selectTextPoint.y);
					pt.y = selectTextPoint.y;
				}
				else
				{
					pt.x = 0;
					pt.y = selectTextPoint.y+1;
				}
				if( GetCaretTextPoint().x != mFirstSelectStart.x || GetCaretTextPoint().y != mFirstSelectStart.y )
				{
					SetCaretTextPoint(mFirstSelectStart);
				}
				SetSelectTextPoint(pt);
			}
		}
		else //#222
		{
			//==================�i���I��==================
			//�i���J�n�ʒu�A�I���ʒu�擾
			ATextPoint	start = {0}, end = {0};
			AIndex	paraIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(selectTextPoint.y);
			start.x = 0;
			start.y = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(paraIndex);
			if( paraIndex+1 >= GetTextDocumentConst().SPI_GetParagraphCount() )
			{
				end.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(GetTextDocumentConst().SPI_GetLineCount()-1);
				end.y = GetTextDocumentConst().SPI_GetLineCount()-1;
			}
			else
			{
				end.x = 0;
				end.y = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(paraIndex+1);
			}
			//�O�����փh���b�O�̏ꍇ
			if( selectTextPoint.y < mClickTextLineIndex )
			{
				if( GetCaretTextPoint().x != mFirstSelectEnd.x || GetCaretTextPoint().y != mFirstSelectEnd.y )
				{
					SetCaretTextPoint(mFirstSelectEnd);
				}
				SetSelectTextPoint(start);
			}
			//������փh���b�O�̏ꍇ
			else
			{
				if( GetCaretTextPoint().x != mFirstSelectStart.x || GetCaretTextPoint().y != mFirstSelectStart.y )
				{
					SetCaretTextPoint(mFirstSelectStart);
				}
				SetSelectTextPoint(end);
			}
		}
	}
}

//�}�E�X�N���b�N�i4��N���b�N�j
void	AView_Text::DoMouseDownQuadClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	
	//quad click�I��
	mClickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
	ATextIndex	wordStart, wordEnd;
	GetTextDocument().SPI_FindQuadWord(mClickTextIndex,wordStart,wordEnd);
	GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,mFirstSelectStart,true);
	GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,mFirstSelectEnd,false);
	/*#688 4��N���b�N���̊��ʑI�𖳂�
	//���ʑI��
	DoubleClickBrace(mFirstSelectStart,mFirstSelectEnd);
	*/
	//
	SetSelect(mFirstSelectStart,mFirstSelectEnd);
	
	return;
	/*#688 4��N���b�N���̃}�E�X�h���b�O�͕s�v�Ǝv����̂ŃR�����g�A�E�g
	//
	AText	word;
	GetTextDocument().SPI_GetText(wordStart,wordEnd,word);
	if( GetApp().SPI_GetIdInfoWindow().NVI_IsWindowVisible() == true )//RC2
	{
		SetIdInfo(word);//RC2
	}
	else
	{
		SetInfoHeader(word);
	}
	
	//�}�E�X�h���b�O�ɂ��I��
	
	//Windows�̏ꍇ�́A�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_QuadClick;
	mMouseTrackStartImagePoint = clickImagePoint;
	mMouseTrackImagePoint = clickImagePoint;
	NVM_SetMouseTrackingMode(true);
	return;
	*/
	//#688}
	/*#688
	//�ŏ��̃}�E�X�ʒu�擾�i���̎��_�Ń}�E�X�{�^����������Ă����烊�^�[���BTrackMouseDown()�Ń}�E�X�{�^�������[�X�̃C�x���g���擾������A���͂������Ȃ��BB0260�j
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
	//Image���W�ɕϊ�
	ALocalPoint	mouseLocalPoint;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
	//�O��}�E�X�ʒu�̕ۑ�
	AImagePoint	previousImagePoint = mouseImagePoint;
	//�}�E�X�{�^�����������܂Ń��[�v
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//Image���W�ɕϊ�
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
		//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
		if( mouseImagePoint.x == previousImagePoint.x && mouseImagePoint.y == previousImagePoint.y )
		{
			continue;
		}
		
		//Mouse Track
		DoMouseTrack_QuadClick(mouseImagePoint,modifiers);
		
		//�O��}�E�X�ʒu�̕ۑ�
		previousImagePoint = mouseImagePoint;
	}
	*/
}

//Mouse Track (4��N���b�N)
void	AView_Text::DoMouseTrack_QuadClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//�\���t���[����Image���W�Ŏ擾
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(Y����)
		if( mouseImagePoint.y < (frameImageRect.top+kDragScrollMargin) )//#456
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.top+kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > (frameImageRect.bottom-kDragScrollMargin) )//#456
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.bottom-kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.bottom - 1;
			}
		}
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(X����)
		if( mouseImagePoint.x < frameImageRect.left )
		{
			if( frameImageRect.left > 0 )
			{
				NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.left - NVI_GetHScrollBarUnit() + 1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.left;
			}
		}
		else if( mouseImagePoint.x > frameImageRect.right )
		{
			if( frameImageRect.right < NVM_GetImageWidth() )
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.right + NVI_GetHScrollBarUnit() -1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.right;
			}
		}
		
		//Drag�ɂ��P��I��
		ATextPoint	selectTextPoint;
		GetTextPointFromImagePoint(mouseImagePoint,selectTextPoint);
		ATextIndex	selectTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(selectTextPoint);
		ATextIndex	wordStart, wordEnd;
		GetTextDocument().SPI_FindQuadWord(selectTextIndex,wordStart,wordEnd);
		ATextPoint	wordStartPoint, wordEndPoint;
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,wordStartPoint,true);
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,wordEndPoint,false);
		if( wordEnd <= mClickTextIndex )
		{
			if( GetCaretTextPoint().x != mFirstSelectEnd.x || GetCaretTextPoint().y != mFirstSelectEnd.y )
			{
				SetCaretTextPoint(mFirstSelectEnd);
			}
			SetSelectTextPoint(wordStartPoint);
		}
		else
		{
			if( GetCaretTextPoint().x != mFirstSelectStart.x || GetCaretTextPoint().y != mFirstSelectStart.y )
			{
				SetCaretTextPoint(mFirstSelectStart);
			}
			SetSelectTextPoint(wordEndPoint);
		}
	}
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_Text::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	SPI_DoMouseTracking(inLocalPoint,inModifierKeys,false);
}
void	AView_Text::SPI_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
										  const ABool inClickedAtLineNumberArea )
{
	//#688
	//�Ă̂Ђ�c�[��
	if( mMouseTrackingMode == kMouseTrackingMode_TenohiraTool )
	{
		//�ω��擾
		ANumber	deltaX = inLocalPoint.x - mMouseTenohiraPreviousLocalPoint.x;
		ANumber	deltaY = inLocalPoint.y - mMouseTenohiraPreviousLocalPoint.y;
		
		//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
		if( deltaX == 0 && deltaY == 0 )
		{
			return;
		}
		
		//�X�N���[��
		NVI_Scroll(-deltaX,-deltaY,true);
		
		//previousLocalPoint�ɂ͂P�O�̃}�E�X�ʒu���L��
		mMouseTenohiraPreviousLocalPoint = inLocalPoint;
		
		return;
	}
	//
	AImagePoint	imagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagePoint);
	if( imagePoint.x == mMouseTrackImagePoint.x && imagePoint.y == mMouseTrackImagePoint.y )   return;
	
	//#795
	//modifier���L��
	mCurrentMouseTrackModifierKeys = inModifierKeys;
	//�s�ԍ������N���b�N�������ǂ������L��
	mCurrentMouseTrackInsideLineNumber = inClickedAtLineNumberArea;
	//image point���L��
	mMouseTrackImagePoint = imagePoint;
	DoMouseTrack(inModifierKeys);
}

void	AView_Text::DoMouseTrack( const AModifierKeys inModifierKeys )
{
	//#688
	//�������ɂ��R���e�L�X�g���j���[���s�B�i���s������track�����j
	if( JudgeContextMenuByLeftClickLongTime(mMouseTrackImagePoint,inModifierKeys) == true )
	{
		mMouseTrackingMode = kMouseTrackingMode_None;
		NVM_SetMouseTrackingMode(false);
		return;
	}
	
	//
	ALocalPoint	originalLocalPoint;
	NVM_GetLocalPointFromImagePoint(mMouseTrackImagePoint,originalLocalPoint);
	switch(mMouseTrackingMode)
	{
	  case kMouseTrackingMode_SingleClick:
		{
			//Mouse Track
			DoMouseTrack_SingleClick(mMouseTrackImagePoint,inModifierKeys);
			break;
		}
	  case kMouseTrackingMode_SingleClickInSelect:
		{
			if( mMouseTrackImagePoint.x != mMouseTrackStartImagePoint.x || mMouseTrackImagePoint.y != mMouseTrackStartImagePoint.y )
			{
				mMouseTrackResultIsDrag = true;
				//TrackingMode���� #688 �ʒu��DoMouseTrackEnd_SingleClickInSelect()�̉�����ړ�
				mMouseTrackingMode = kMouseTrackingMode_None;
				NVM_SetMouseTrackingMode(false);
				//
				DoMouseTrackEnd_SingleClickInSelect(mMouseTrackImagePoint,inModifierKeys);
			}
			break;
		}
	  case kMouseTrackingMode_DoubleClick:
		{
			//Mouse Track
			DoMouseTrack_DoubleClick(mMouseTrackImagePoint,inModifierKeys);
			break;
		}
	  case kMouseTrackingMode_TripleClick:
		{
			//Mouse Track
			DoMouseTrack_TripleClick(mMouseTrackImagePoint,inModifierKeys);
			break;
		}
	  case kMouseTrackingMode_QuadClick:
		{
			//Mouse Track
			DoMouseTrack_QuadClick(mMouseTrackImagePoint,inModifierKeys);
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
	NVM_GetImagePointFromLocalPoint(originalLocalPoint,mMouseTrackImagePoint);
}

//Mouse Tracking�I���i�}�E�X�{�^��Up�j
void	AView_Text::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	//
	if( mMouseTrackingMode == kMouseTrackingMode_SingleClickInSelect )
	{
		DoMouseTrackEnd_SingleClickInSelect(mouseImagePoint,inModifierKeys);
	}
	//
	if( mMouseTrackingMode == kMouseTrackingMode_SingleClick )
	{
		DoMouseTrackEnd_SingleClick(mouseImagePoint,inModifierKeys);
	}
	//#823
	/*idle work�^�C�}�[�ɂ����s
	//���ݒP��n�C���C�g
	SetCurrentWordHighlight();
	//#853
	//�L�[���[�h���
	DisplayIdInfo(false,kIndex_Invalid,kIndex_Invalid);
	//#723
	//���[�Y���X�g�֌����v��
	RequestWordsList();
	*/
	//idle work timer�N��
	TriggerKeyIdleWork(kKeyIdleWorkType_MouseClick,kMouseClikcIdleWorkTimerValue);
	
	//TrackingMode����
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}
//#688 #endif

//#688
/**
�}�E�X�N���b�N����������
*/
ABool	AView_Text::JudgeContextMenuByLeftClickLongTime( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	//
	if( inMouseImagePoint.x == mMouseDownImagePoint.x && inMouseImagePoint.y == mMouseDownImagePoint.y )
	{
		//elapsed tick��58.2ms���Ƃ�+1�Amouse button tick�ݒ��16.7ms�P�ʁB
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCMMouseButton) == true &&
					GetApp().SPI_GetAppElapsedTick() > mMouseDownTick + GetApp().GetAppPref().GetData_Number(AAppPrefDB::kCMMouseButtonTick)/3.5 )
		{
			//�R���e�L�X�g���j���[���s
			ALocalPoint	mouseLocalPoint = {0};
			NVM_GetLocalPointFromImagePoint(inMouseImagePoint,mouseLocalPoint);
			DoContextMenu(mouseLocalPoint,inModifierKeys);
			//
			mMouseDownImagePoint.x = mMouseDownImagePoint.y = 0;
			mMouseDownTick = 0;
			return true;
		}
	}
	return false;
}

//B0260
ANumber	AView_Text::DragScrollV( const ANumber inMouseOffset, const AModifierKeys inModifierKeys )
{
	ANumber	delta = GetLineHeightPlusMargin();//#450 GetLineHeightWithMargin();
	if( true /*#844 GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kChangeScrollSpeedForMouseDrag) == true*/ )
	{
		ANumber	div = 30;
		ANumber	mouseOffset = inMouseOffset;
		if( mouseOffset > 3*div )  mouseOffset = 3*div;
		if( mouseOffset < -3*div )  mouseOffset = -3*div;
		delta *= inMouseOffset;
		delta /= div;
	}
	else
	{
		if( inMouseOffset < 0 )   delta *= -1;
	}
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) )   delta *= 5;
	
	NVI_Scroll(0,delta);
	return delta;
}

//#1026
/**
QuickLook�C�x���g�i�����Œ��ׂ�j
*/
void	AView_Text::EVTDO_QuickLook( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mCaretMode == false )
	{
		//�e�L�X�g�I�𒆂́A���̑I�𕶎���̎�����\��
		DictionaryPopup();
	}
	else
	{
		//�e�L�X�g��I�����Ă��Ȃ��Ƃ��́A�N���b�N�ʒu�̒P���I�����āA���̒P��̎�����\��
		
		//�N���b�N�ʒu�̃e�L�X�g�ʒu�擾
		AImagePoint	clickImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		ATextPoint	clickTextPoint;
		GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
		
		//�P��I��
		ATextIndex	clickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
		ATextIndex	wordStart, wordEnd;
		GetTextDocument().SPI_FindWord(clickTextIndex,wordStart,wordEnd);
		ATextPoint	spt, ept;
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,spt,true);
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,ept,false);
		AText	text;
		GetTextDocument().SPI_GetText(wordStart,wordEnd,text);
		
		//�P��J�n�ʒu�̕\���ʒu���擾
		AImagePoint	ipt;
		GetImagePointFromTextPoint(spt,ipt);
		ALocalPoint	pt;
		NVM_GetLocalPointFromImagePoint(ipt,pt);
		if( NVI_GetVerticalMode() == false )
		{
			//����������ascent�����v���X����
			pt.y += GetLineAscent(spt.y);
		}
		else
		{
			//�c�������͍s���������v���X����
			pt.y += GetLineHeightWithoutMargin(spt.y);
		}
		//�����\��
		NVMC_ShowDictionary(text,pt);
	}
}

//�R���e�L�X�g���j���[
void	AView_Text::DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	const	AItemCount	kLimitLength = 15;
	
	//#688
	//�T�u���j���[detach���ۑ��ϐ�
	AArray<AMenuRef>	subMenuArray;
	AArray<AMenuRef>	oldParentMenuArray;
	AArray<AIndex>	oldParentMenuItemIndexArray;
	//
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	globalPoint.y += GetLineHeightPlusMargin();//#450 GetLineHeightWithoutMargin();
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	if( IsInSelect(mouseImagePoint,true) == false && GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kSelectWordByContextMenu) == false )//B0432
	{
		//R0231
		const AIndex	kJumpToDef	= 0;
		//const AIndex	kIdInfo		= 1;
		//const AIndex	kDictionary	= 3;
		const AIndex	kCorrectSpell = 4;
		//const AIndex	kSelectAll	= 6;
		const AIndex	kPaste 		= 7;
		const AIndex	kTool		= 9;
		const AIndex	kRelatedFile= 11;
		
#if IMPLEMENTATION_FOR_WINDOWS
		//���Ή�����
		GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextUnselected,3,false);
		GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextUnselected,4,false);
		GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextUnselected,9,false);
#endif
		
		//�L�����b�g�ݒ�A�P��擾
		AImagePoint	clickImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		ATextPoint	clickTextPoint;
		GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
		SetSelect(clickTextPoint,clickTextPoint);//B0432
		ATextIndex	textpos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
		ATextIndex	spos,epos;
		GetTextDocument().SPI_FindWord(textpos,spos,epos);
		AText	selectedtext;
		GetTextDocumentConst().SPI_GetText(spos,epos,selectedtext);
		//�u��`�ꏊ�Ɉړ��v
		GetTextDocument().SPI_UpdateCurrentIdentifierDefinitionList(selectedtext);
		GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextUnselected,kJumpToDef,
					GetApp().NVI_GetMenuManager().GetDynamicMenuObjectIDByMenuItemID(kMenuItemID_MoveToDefinition),
					subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
		//�u�y�[�X�g�v
		AText	text;
		text.SetLocalizedText("CMText_Paste");
		AText	pastetext;
		AScrapWrapper::GetClipboardTextScrap(pastetext);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
		pastetext.ReplaceChar(kUChar_LineEnd,kUChar_Space);//win
		pastetext.ReplaceChar(kUChar_Tab,kUChar_Space);//win
		//B0000 �E�N���b�N�ŗ������莞�Ԃ���������̑΍�
		if( pastetext.GetItemCount() >= kLimitLength )
		{
			pastetext.LimitLength(0,kLimitLength-3);
			pastetext.AddCstring("...");
		}
		if( pastetext.CheckUTF8Text(pastetext.GetItemCount()) == false )   pastetext.DeleteAll();
		
		//�u�c�[���v
		text.ReplaceParamText('0',pastetext);
		GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextUnselected,/*R0231 1*/kPaste,text);
		GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextUnselected,/*R0231 3*/kTool,
					GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetToolMenuRootObjectID(),
					subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
		//�u�֘A����t�@�C���v
		GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextUnselected,/*R0231 5*/kRelatedFile,
					GetApp().NVI_GetMenuManager().GetDynamicMenuObjectIDByMenuItemID(kMenuItemID_OpenImportFile),
					subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
#if IMPLEMENTATION_FOR_MACOSX
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
		{
			ATextArray	candidateArray;
			ASpellCheckerWrapper::GuessWord(selectedtext,candidateArray,
					GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Number(AModePrefDB::kSpellCheckLanguage));
			//���݂̃��j���[���ڂ��폜
			AObjectID	correctSpellMenuObjectID = GetApp().SPI_GetCorrectSpellMenuObjectID();
			GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByObjectID(correctSpellMenuObjectID);
			//
			for( AIndex i = 0; i < candidateArray.GetItemCount(); i++ )
			{
				AText	menutext;
				candidateArray.Get(i,menutext);
				GetApp().NVI_GetMenuManager().AddDynamicMenuItemByObjectID(correctSpellMenuObjectID,menutext,menutext,kObjectID_Invalid,NULL,0,false);
			}
			GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextUnselected,
						kCorrectSpell,correctSpellMenuObjectID,
						subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
		}
#endif
		/*#858
		{
			//#138
			//�u�tⳎ��v
			AIndex	commentIndex = GetTextDocument().SPI_FindExternalCommentIndexByLineIndex(clickTextPoint.y);
			if( commentIndex == kIndex_Invalid )
			{
				text.SetLocalizedText("CMText_Fusen_New");
			}
			else
			{
				text.SetLocalizedText("CMText_Fusen_Edit");
			}
			GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextUnselected,kExternalComment,text);
		}
		*/
		//�R���e�L�X�g���j���[�\��
		//B0000
		//#688 GetApp().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_TextUnselected,globalPoint,NVM_GetWindow().NVI_GetWindowRef());//win 080712
		NVMC_ShowContextMenu(kContextMenuID_TextUnselected,globalPoint);//#688
		
		//#688 return;
	}
	else
	{
		if( IsInSelect(mouseImagePoint,true) == false )
		{
			//�P��I��
			AImagePoint	clickImagePoint;
			NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
			ATextPoint	clickTextPoint;
			GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
			ATextIndex	textpos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			ATextIndex	spos,epos;
			GetTextDocument().SPI_FindWord(textpos,spos,epos);
			SetSelect(spos,epos);
		}
		{
			//R0231
			const AIndex	kJumpToDef	= 0;
			//const AIndex	kIdInfo		= 1;
			//const AIndex	kDefineValue= 2;
			//const AIndex	kDictionary	= 4;
			const AIndex	kCorrectSpell = 5;
			//const AIndex	kCopy 		= 7;
			//const AIndex	kCut 		= 8;
			const AIndex	kPaste		= 9;
			//const AIndex	kClear		= 10;
			const AIndex	kFind		= 12;
			//const AIndex	kMFFind		= 13;
			const AIndex	kNext		= 14;
			const AIndex	kPrev		= 15;
			const AIndex	kTransliterate= 17;
			const AIndex	kTool		= 18;
			const AIndex	kRelatedFile= 20;
			//�@�\����const AIndex	kCorrectSpell=14;
			//const AIndex	kPrintRange	= 23;//#524
			
#if IMPLEMENTATION_FOR_WINDOWS
			//���Ή�����
			GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextSelected,3,false);
			GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextSelected,4,false);
			GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextSelected,16,false);
#endif
			
			//�u�y�[�X�g�v
			AText	text;
			text.SetLocalizedText("CMText_Paste");
			AText	pastetext;
			AScrapWrapper::GetClipboardTextScrap(pastetext);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
			pastetext.ReplaceChar(kUChar_LineEnd,kUChar_Space);//win
			pastetext.ReplaceChar(kUChar_Tab,kUChar_Space);//win
			//B0000 �E�N���b�N�ŗ������莞�Ԃ���������̑΍�
			if( pastetext.GetItemCount() >= kLimitLength )
			{
				pastetext.LimitLength(0,kLimitLength-3);
				pastetext.AddCstring("...");
			}
			if( pastetext.CheckUTF8Text(pastetext.GetItemCount()) == false )   pastetext.DeleteAll();
			
			text.ReplaceParamText('0',pastetext);
			GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextSelected,/*R0231 2*/kPaste,text);
			//�u�����v
			text.SetLocalizedText("CMText_Find");
			AText	selectedtext;
			GetSelectedText(selectedtext);
			//B0000 
			AText	selectedtext1;
			selectedtext1.SetText(selectedtext);
			selectedtext1.ReplaceChar(kUChar_LineEnd,kUChar_Space);//win
			selectedtext1.ReplaceChar(kUChar_Tab,kUChar_Space);//win
			if( selectedtext1.GetItemCount() >= kLimitLength )
			{
				selectedtext1.LimitLength(0,kLimitLength-3);
				selectedtext1.AddCstring("...");
			}
			if( selectedtext1.CheckUTF8Text(selectedtext.GetItemCount()) == false )   selectedtext1.DeleteAll();
			
			text.ReplaceParamText('0',selectedtext1);
			GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextSelected,/*R0231 5*/kFind,text);
			//�u���������v
			text.SetLocalizedText("CMText_FindNext");
			text.ReplaceParamText('0',selectedtext1);
			GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextSelected,/*R0231 7*/kNext,text);
			//�u�O�������v
			text.SetLocalizedText("CMText_FindPrev");
			text.ReplaceParamText('0',selectedtext1);
			GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextSelected,/*R0231 8*/kPrev,text);
			//�u�c�[���v
			GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextSelected,/*R0231 12*/kTool,
						GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetToolMenuRootObjectID(),
						subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
			//�u������ϊ��v
			GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextSelected,/*R0231 11*/kTransliterate,
						GetApp().NVI_GetMenuManager().GetDynamicMenuObjectIDByMenuItemID(kMenuItemID_Transliterate),
						subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
			//�u��`�ꏊ�Ɉړ��v
			GetTextDocument().SPI_UpdateCurrentIdentifierDefinitionList(selectedtext);
			GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextSelected,/*R0231 10*/kJumpToDef,
						GetApp().NVI_GetMenuManager().GetDynamicMenuObjectIDByMenuItemID(kMenuItemID_MoveToDefinition),
						subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
			//�u�֘A����t�@�C���v
			GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextSelected,/*R0231 14*/kRelatedFile,
						GetApp().NVI_GetMenuManager().GetDynamicMenuObjectIDByMenuItemID(kMenuItemID_OpenImportFile),
						subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
			//R0199
#if IMPLEMENTATION_FOR_MACOSX
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
			{
				ATextArray	candidateArray;
				ASpellCheckerWrapper::GuessWord(selectedtext,candidateArray,
							GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Number(AModePrefDB::kSpellCheckLanguage));
				//���݂̃��j���[���ڂ��폜
				AObjectID	correctSpellMenuObjectID = GetApp().SPI_GetCorrectSpellMenuObjectID();
				GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByObjectID(correctSpellMenuObjectID);
				//
				for( AIndex i = 0; i < candidateArray.GetItemCount(); i++ )
				{
					AText	menutext;
					candidateArray.Get(i,menutext);
					GetApp().NVI_GetMenuManager().AddDynamicMenuItemByObjectID(correctSpellMenuObjectID,menutext,menutext,kObjectID_Invalid,NULL,0,false);
				}
				GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextSelected,kCorrectSpell,correctSpellMenuObjectID,
							subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
			}
#endif
			/*#858
			{
				//#138
				//�u�tⳎ��v
				ATextPoint	spt, ept;
				SPI_GetSelect(spt,ept);
				AIndex	commentIndex = GetTextDocument().SPI_FindExternalCommentIndexByLineIndex(spt.y);
				if( commentIndex == kIndex_Invalid )
				{
					text.SetLocalizedText("CMText_Fusen_New");
				}
				else
				{
					text.SetLocalizedText("CMText_Fusen_Edit");
				}
				GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextSelected,kExternalComment,text);
			}
			*/
			//�R���e�L�X�g���j���[�\��
			//B0000
			//#688 GetApp().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_TextSelected,globalPoint,NVM_GetWindow().NVI_GetWindowRef());//win 080712
			NVMC_ShowContextMenu(kContextMenuID_TextSelected,globalPoint);//#688
		}
	}
	//#688
	//�T�u���j���[�����i���̃��j���[��attach����j
	for( AIndex i = 0; i < subMenuArray.GetItemCount(); i++ )
	{
		//�R���e�L�X�g���j���[����detach
		AMenuRef	menuRef = NULL;
		AIndex	menuItemIndex = kIndex_Invalid;
		AMenuWrapper::DetachSubMenu(subMenuArray.Get(i),menuRef,menuItemIndex);
		//���̃��j���[��attach
		if( oldParentMenuArray.Get(i) != NULL )
		{
			AMenuWrapper::SetSubMenu(oldParentMenuArray.Get(i),oldParentMenuItemIndexArray.Get(i),subMenuArray.Get(i));
		}
	}
}

//#606 ��̂Ђ�c�[��
/**
��̂Ђ�c�[�����[�h���ǂ����𔻒�
*/
ABool	AView_Text::IsHandToolMode() const
{
	//���݂̎�̂Ђ�c�[�����[�h��Ԃ��擾
	ABool	handtoolmode = GetApp().SPI_GetHandToolMode();
	//�R�}���h�L�[�����Ȃ甽�]
	if( AKeyWrapper::IsCommandKeyPressed() == true )
	{
		handtoolmode = !handtoolmode;
	}
	return handtoolmode;
}

//�J�[�\��
ACursorType	AView_Text::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//��̂Ђ�c�[�����[�h�Ȃ�Open hand�J�[�\�� #606
	/*
	if( IsHandToolMode() == true )
	{
		return kCursorType_OpenHand;
	}
	*/
	//
	if( IsCaretMode() == false )
	{
		AImagePoint	mouseImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
		ATextPoint	mouseTextPoint;
		GetTextPointFromImagePoint(mouseImagePoint,mouseTextPoint);
		if( IsInSelect(mouseImagePoint,true) == true )
		{
			if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || 
						AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true ||
						AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win
			{
				return kCursorType_ArrowCopy;
			}
			else
			{
				return kCursorType_Arrow;
			}
		}
	}
	/*
	//#450
	//�܂肽���ݔg���ʒu�Ȃ�㉺���T�C�Y�J�[�\��
	if( IsMouseInFoldingCollapsedLine(inLocalPoint) == true )
	{
		return kCursorType_ResizeUpDown;
	}
	*/
	//�e�L�X�g�̏�ł����Ă�I�r�[���J�[�\���ɂ��Ȃ��ݒ� #1208
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kInhibitIbeamCursor) == true )
	{
		return kCursorType_Arrow;
	}
	//�c�������[�h�Ȃ�}�E�X�J�[�\�����c�����p�ɕύX #558
	if( NVI_GetVerticalMode() == true )
	{
		return kCursorType_IBeamForVertical;
	}
	//
	return kCursorType_IBeam;
}

/**
�܂肽���ݔg���ʒu���ǂ����𔻒�
*/
ABool	AView_Text::IsMouseInFoldingCollapsedLine( const ALocalPoint& inLocalPoint ) 
{
	AImagePoint	mouseImagePoint = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	ATextPoint	textPoint = {0};
	GetTextPointFromImagePoint(mouseImagePoint,textPoint);
	if( SPI_IsCollapsedLine(textPoint.y) == true )
	{
		//�܂肽���ݍs�̉�3pt�Ȃ�true��Ԃ�
		AImageRect	lineImageRect = {0};
		GetLineImageRect(textPoint.y,lineImageRect);
		if( mouseImagePoint.y >= lineImageRect.bottom-3 && mouseImagePoint.y <= lineImageRect.bottom )
		{
			return true;
		}
	}
	return false;
}

//#606
/**
�}�E�X�N���b�N�i��̂Ђ�c�[���j������
*/
void	AView_Text::DoMouseDownTenohira( const ALocalPoint& inFirstLocalPoint , const AModifierKeys inModifierKeys )
{
	//��̂Ђ�i���ށj�J�[�\���ݒ�
	ACursorWrapper::SetCursor(kCursorType_ClosedHand);
	
	//#688
	//��̂Ђ�c�[���h���b�O�J�n
	mMouseTrackingMode = kMouseTrackingMode_TenohiraTool;
	mMouseTenohiraPreviousLocalPoint = inFirstLocalPoint;
	NVM_SetMouseTrackingMode(true);
	return;
	
	/*#688
	//�ŏ��̃}�E�X�ʒu�擾�i���̎��_�Ń}�E�X�{�^����������Ă����烊�^�[���BTrackMouseDown()�Ń}�E�X�{�^�������[�X�̃C�x���g���擾������A���͂������Ȃ��BB0260�j
	AWindowPoint	mouseWindowPoint = {0,0};
	AModifierKeys	modifiers = 0;
	if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
	
	//previousLocalPoint�ɂ͂P�O�̃}�E�X�ʒu���L��
	ALocalPoint	previousLocalPoint = {0,0};
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,previousLocalPoint);
	
	//�}�E�X�{�^�����������܂Ń��[�v
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//�}�E�X�ʒu�擾
		ALocalPoint	localPoint = {0,0};
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,localPoint);
		
		//�ω��擾
		ANumber	deltaX = localPoint.x - previousLocalPoint.x;
		ANumber	deltaY = localPoint.y - previousLocalPoint.y;
		
		//�O��}�E�X�ʒu�Ɠ����Ȃ牽���������[�v�p��
		if( deltaX == 0 && deltaY == 0 )
		{
			continue;
		}
		
		//�X�N���[��
		NVI_Scroll(-deltaX,-deltaY,true);
		
		//previousLocalPoint�ɂ͂P�O�̃}�E�X�ʒu���L��
		previousLocalPoint = localPoint;
	}
	
	//�J�[�\���߂�
	ACursorWrapper::SetCursor(kCursorType_Arrow);
	*/
}

//#823
/**
���݂̕`��͈͂��擾
*/
void	AView_Text::SPI_GetDrawLineRange( AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const
{
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	outStartLineIndex = GetLineIndexByImageY(imageFrameRect.top);
	outEndLineIndex = GetLineIndexByImageY(imageFrameRect.bottom);
}

//�`��f�[�^
const AFloatNumber	kFindHighlightAlpha = 0.8;
const AFloatNumber	kIdInfoArgHighlightAlpha = 0.3;
const AFloatNumber	kCurrentWordHighlightAlpha = 0.3;
const AFloatNumber	kTextMarkerAlpha = 0.8;

//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_Text::EVTDO_DoDraw()
{
	//win EVTDO_DrawPreProcess()�ֈړ� TempHideCaret();
	
	//��\�����͉������Ȃ� win
	if( NVI_IsVisible() == false )   return;
	
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//#621
	ANumber	imageTop = imageFrameRect.top;
	if( imageTop < 0 )
	{
		imageTop = 0;
	}
	//
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	AIndex	startLineIndex = GetLineIndexByImageY(imageTop);
	//#450 AIndex	endLineIndex = imageFrameRect.bottom/GetLineHeightWithMargin();//���݂��Ȃ��s�ɂ��E�C���h�E���͔w�i��\�������邽��GetLineIndexByImageY(rect.bottom);
	/*AText	title;
	document.NVI_GetTitle(title);
	title.OutputToStderr();
	fprintf(stderr,"-start:%d \n",startLineIndex);*/
	//
	const ADocument_Text&	document = GetTextDocumentConst();
	const AModeIndex	modeIndex = document.SPI_GetModeIndex();
	const AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(modeIndex);
	//const AAppPrefDB&	appPrefDB = GetApp().GetAppPref();
	ABool	drawTab = ((modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayTabCode)==true)&&IsPrinting()==false);
	ABool	drawSpace = ((modePrefDB.GetModeData_Bool(AModePrefDB::kDisplaySpaceCode)==true)&&IsPrinting()==false);
	ABool	drawZenkakuSpace = ((modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayZenkakuSpaceCode)==true)&&IsPrinting()==false);
	ABool	drawMasume = (modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayGenkoyoshi) == true);
	ABool	drawLineEndCode = (modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayReturnCode) == true);
	AWrapMode	wrapMode = document.SPI_GetWrapMode();
	AItemCount	wrapLetterCount = document.SPI_GetWrapLetterCount();
	//�w�i�`��i�w�i��localFrame�S�́i�̂���UpdateRegion���j����C�ɕ`���B�j
	AIndex	backgroundImageIndex = modePrefDB.GetBackgroundImageIndex();
	if( modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayBackgroundImage) == true && backgroundImageIndex != kIndex_Invalid )
	{
		AFloatNumber	alpha = modePrefDB.GetModeData_Number(AModePrefDB::kBackgroundImageTransparency);
		alpha /= 100;
		NVMC_DrawBackgroundImage(localFrameRect,imageFrameRect,mBackgroundImageOffset,backgroundImageIndex,alpha);
	}
	else
	{
		AColor	color;
		modePrefDB.GetModeData_Color(AModePrefDB::kBackgroundColor,color);
		NVMC_PaintRect(localFrameRect,color,1.0);
	}
	//�C���[�W�͈͂����O�͔w�i�F�ŃN���A����
	if( imageFrameRect.top < 0 )
	{
		AImageRect	minusAreaImageRect = imageFrameRect;
		minusAreaImageRect.bottom = 0;
		ALocalRect	minusAreaLocalRect = {0};
		NVM_GetLocalRectFromImageRect(minusAreaImageRect,minusAreaLocalRect);
		AColor	color = kColor_White;
		modePrefDB.GetModeData_Color(AModePrefDB::kBackgroundColor,color);
		NVMC_PaintRect(minusAreaLocalRect,color,1.0);
	}
	//
	/*
	ALocalPoint	shadowpt = {localFrameRect.right-22,localFrameRect.top};
	NVMC_DrawImageFlexibleHeight(kImageID_SubTextShadow,kImageID_SubTextShadow,kImageID_SubTextShadow,shadowpt,localFrameRect.bottom-localFrameRect.top);
	*/
	//�I��F�擾
	AColor	selectionColor;
	//#1316 if( NVI_IsFocusActive() == true )
	{
		AColorWrapper::GetHighlightColor(selectionColor);
	}
	/*#1316
	else
	{
		selectionColor = kColor_Gray90Percent;
	}
	*/
	//�I��F�s�����x�擾
	AFloatNumber	selectionOpacity = GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Number(AModePrefDB::kSelectionOpacity);
	selectionOpacity /= 100.0;
	//R0006 diff�F�擾
	AColor	diffcolor_added, diffcolor_changed, diffcolor_deleted;
	GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Changed,diffcolor_changed);
	//#379 Diff�Ώۃh�L�������g���͍폜�F�ƒǉ��F����ւ�
	//AFloatNumber	diffDelAlpha = 0.15;
	if( GetTextDocumentConst().SPI_IsDiffTargetDocument() == false )
	{
		GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Added,diffcolor_added);
		GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Deleted,diffcolor_deleted);
	}
	else
	{
		GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Deleted,diffcolor_added);
		GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Added,diffcolor_deleted);
		//diffDelAlpha = 0.1;
	}
	//diff�\�����l�擾
	AFloatNumber	diffAlpha = modePrefDB.GetModeData_Number(AModePrefDB::kDiffColorOpacity);
	diffAlpha /= 300;//0.0�`0.33�ɂ���
	//�Z���ݒ肪�傫���قǍʓx�������āA���x���グ��
	AFloatNumber	diffSChange = 1.0 - diffAlpha*1.5;
	AFloatNumber	diffVChange = 1.0 + diffAlpha*2.0;
	diffcolor_added = AColorWrapper::ChangeHSV(diffcolor_added,0.0,diffSChange,diffVChange);
	diffcolor_deleted = AColorWrapper::ChangeHSV(diffcolor_deleted,0.0,diffSChange,diffVChange);
	diffcolor_changed = AColorWrapper::ChangeHSV(diffcolor_changed,0.0,diffSChange,diffVChange);
	//�E�C���h�E��diff mode���ǂ������擾
	ABool	windowIsDiffMode = false;
	if( mTextWindowID != kObjectID_Invalid )
	{
		if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetDiffDisplayMode() == true )
		{
			windowIsDiffMode = true;
		}
	}
	//#450
	AText	fontname;
	//#868 GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,fontname);
	GetTextDocumentConst().SPI_GetFontName(fontname);//#868
	AColor	lettercolor;
	GetModePrefDB().GetModeData_Color(AModePrefDB::kLetterColor,lettercolor);
	AFloatNumber	normalfontsize = GetTextDocumentConst().SPI_GetFontSize();//#868
	ABool	isAntiAlias = GetTextDocument().SPI_IsAntiAlias();
	//#1142
	//���䕶���F�擾
	AColor	spcolor = kColor_Gray50Percent;
	GetModePrefDB().GetModeData_Color(AModePrefDB::kControlCodeColor,spcolor);
	//�t�H���g�ݒ�
	NVMC_SetDefaultTextProperty(fontname,normalfontsize,lettercolor,kTextStyle_Normal,isAntiAlias);
	
	//initial�n��Ԃ̂Ƃ��͕`�悵�Ȃ�
	if( GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		/*
		NVMC_SetDefaultTextProperty(fontname,12.0,lettercolor,kTextStyle_Normal,isAntiAlias);
		ALocalRect	rect = {0};
		rect.left		= 10;
		rect.top		= 10;
		rect.right		= 1000;
		rect.bottom		= 80;
		AText	loading;
		loading.SetLocalizedText("TextView_FileLoading");
		NVMC_DrawText(rect,loading);
		*/
		return;
	}
	
	AIndex	lineCount = document.SPI_GetLineCount();//#450
	
	//�}�X�ڕ`��
	//#1132 �s���Ȃ������ɂ��`�悷��悤�C��
	if( drawMasume == true )
	{
		//#1316
		AColor	masumeColor = kColor_Gray85Percent;//#1316 80%��85%
		if( AApplication::GetApplication().NVI_IsDarkMode() == true )
		{
			masumeColor = kColor_Gray30Percent;
		}
		//�s���Ƃ̃��[�v�i�ŏI�s�ȍ~1000�s�܂Ń��[�v�B�������Aframe�O�ɏo����break�j
		for( AIndex lineIndex = startLineIndex; lineIndex < lineCount + 1000; lineIndex++ )
		{
			//
			AImageRect	lineImageRect = {0};
			if( lineIndex < lineCount )
			{
				//�simage rect�擾
				GetLineImageRect(lineIndex,lineImageRect);
			}
			else
			{
				//�s���Ȃ������́A�ŏI�s�̈ʒu�ƍ�������A�simage rect���쐬����B
				GetLineImageRect(lineCount-1,lineImageRect);
				ANumber	lineHeight = GetImageYByLineIndex(lineCount) - GetImageYByLineIndex(lineCount-1);
				lineImageRect.top		+= lineHeight*(lineIndex-lineCount);
				lineImageRect.bottom	+= lineHeight*(lineIndex-lineCount);
			}
			//�I������i���\�����悤�Ƃ��Ă���s��imagerect��frame�O��������I��
			if( lineImageRect.top > imageFrameRect.bottom )   break;
			//Update�̈攻��
			ALocalRect	lineLocalRect = {0};
			NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
			if( NVMC_IsRectInDrawUpdateRegion(lineLocalRect) == false )   continue;
			
			AIndex	start 	= imageFrameRect.left  / mZenkakuAWidth;//#1385 mZenkakuSpaceWidth;
			AIndex	end 	= imageFrameRect.right / mZenkakuAWidth;//#1385 mZenkakuSpaceWidth;
			if( wrapMode == kWrapMode_WordWrapByLetterCount || wrapMode == kWrapMode_LetterWrapByLetterCount )//#1113
			{
				end = wrapLetterCount/2;
			}
			ALocalPoint	spt, ept;
			AImagePoint	imagespt, imageept;
			for( AIndex i = start; i <= end; i++ )
			{
				imagespt.x = (ANumber)(mZenkakuAWidth * i);//#1385 mZenkakuSpaceWidth * i;
				imagespt.y = lineImageRect.top;//win 080618
				imageept.x = (ANumber)(mZenkakuAWidth * i);//#1385 mZenkakuSpaceWidth * i;
				imageept.y = imagespt.y + mLineHeight -1;//win 080618
				NVM_GetLocalPointFromImagePoint(imagespt,spt);
				NVM_GetLocalPointFromImagePoint(imageept,ept);
				NVMC_DrawLine(spt,ept,masumeColor);
			}
			//
			imagespt.x = lineImageRect.left;
			imagespt.y = lineImageRect.top;//win 080618
			imageept.x = (ANumber)(mZenkakuAWidth * end);//#1385 mZenkakuSpaceWidth * end;
			imageept.y = lineImageRect.top;//win 080618
			NVM_GetLocalPointFromImagePoint(imagespt,spt);
			NVM_GetLocalPointFromImagePoint(imageept,ept);
			NVMC_DrawLine(spt,ept,masumeColor);
			//
			imagespt.y += mLineHeight;
			imageept.y += mLineHeight;
			NVM_GetLocalPointFromImagePoint(imagespt,spt);
			NVM_GetLocalPointFromImagePoint(imageept,ept);
			NVMC_DrawLine(spt,ept,masumeColor);
		}
	}
	
	//���@�p�[�g�^�O�ƁA���@�`�F�b�N���[�j���O�̔z�F #1316
	AColor	warningTagBackgroundStart = kColor_White;
	AColor	warningTagBackgroundEnd = kColor_Gray95Percent;
	AColor	warningTagFrame = kColor_Gray30Percent;
	AColor	warningTagLetterColor = kColor_Gray30Percent;
	if( GetApp().NVI_IsDarkMode() == true )
	{
		warningTagBackgroundStart = AColorWrapper::GetColorByHTMLFormatColor("2C2C2C");
		warningTagBackgroundEnd = AColorWrapper::GetColorByHTMLFormatColor("282828");
		warningTagFrame = AColorWrapper::GetColorByHTMLFormatColor("909090");
		warningTagLetterColor = AColorWrapper::GetColorByHTMLFormatColor("A0A0A0");
	}
	
	//�e�s�`��
	for( AIndex lineIndex = startLineIndex; lineIndex < lineCount/*#450endLineIndex*/; lineIndex++ )//#450
	{
		AImageRect	lineImageRect;
		GetLineImageRect(lineIndex,lineImageRect);
		//#450 ��\���s����
		if( lineImageRect.top == lineImageRect.bottom )   continue;
		//#450 �I������i���\�����悤�Ƃ��Ă���s��imagerect��frame�O��������I��
		if( lineImageRect.top > imageFrameRect.bottom )   break;
		//
		ALocalRect	lineLocalRect;
		NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
		//#688
		//�`��E�[�̓t���[���E�[�܂łɂ���B�i����gradient�\���̂��߁{���̑��������������ɂȂ邩������Ȃ��j
		lineLocalRect.right = localFrameRect.right;
		//Update�̈攻��
		if( NVMC_IsRectInDrawUpdateRegion(lineLocalRect) == false )   continue;
		
		//==================���@�p�[�g�̕\��==================
		{
			//���ݍs�̕��@�p�[�gindex�擾
			ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
			if( syntaxDefinition.GetSyntaxPartCount() >= 2 )
			{
				AIndex	stateIndex = GetTextDocumentConst().SPI_GetStateIndexByLineIndex(lineIndex);
				AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
				if( syntaxPartIndex != kIndex_Invalid && GetTextDocumentConst().SPI_GetLineRecognized(lineIndex) == true )
				{
					//���@�p�[�g��؂���\������
					ABool	syntaxPartShouldShown = true;
					if( lineIndex > 0 )
					{
						if( GetTextDocumentConst().SPI_GetLineRecognized(lineIndex-1) == false )
						{
							//���@���F���Ȃ�K����؂�\�����Ȃ�
							syntaxPartShouldShown = false;
						}
						else
						{
							//�O�s�̕��@�p�[�g�ƁA���ݍs�̕��@�p�[�g�̕������\�����̂��������������؂�\�����Ȃ�
							AIndex	prevLineStateIndex = GetTextDocumentConst().SPI_GetStateIndexByLineIndex(lineIndex-1);
							AIndex	prevLineSyntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(prevLineStateIndex).GetStateSyntaxPartIndex();
							AText	prevLineSyntaxSeparatorName, currentLineSyntaxSeparatorName;
							if( prevLineSyntaxPartIndex != kIndex_Invalid )
							{
								syntaxDefinition.GetSyntaxPartSeparatorName(prevLineSyntaxPartIndex,prevLineSyntaxSeparatorName);
							}
							if( syntaxPartIndex != kIndex_Invalid )
							{
								syntaxDefinition.GetSyntaxPartSeparatorName(syntaxPartIndex,currentLineSyntaxSeparatorName);
							}
							if( prevLineSyntaxSeparatorName.Compare(currentLineSyntaxSeparatorName) == true )
							{
								syntaxPartShouldShown = false;
							}
						}
					}
					//
					if( syntaxPartShouldShown == true )
					{
						//#1316
						AColor	separatorColor = AColorWrapper::GetColorByHTMLFormatColor("f4edbb");
						if( GetApp().NVI_IsDarkMode() == true )
						{
							separatorColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
						}
						//���@�p�[�g��؂�`��
						ALocalRect	separatorRect = lineLocalRect;
						separatorRect.bottom = separatorRect.top+2;
						NVMC_PaintRectWithVerticalGradient(separatorRect,separatorColor,separatorColor,0.7,0.0);//#1316 ��=0.8��0.7
						//���@�p�[�g���擾
						AText	syntaxName;
						syntaxDefinition.GetSyntaxPartName(syntaxPartIndex,syntaxName);
						NVMC_SetDefaultTextProperty(fontname,6.0,warningTagLetterColor,kTextStyle_Normal,true);//#1316
						//���@�p�[�g���t���[���`��
						ANumber	w = NVMC_GetDrawTextWidth(syntaxName);
						ALocalRect	textFrameRect = lineLocalRect;
						textFrameRect.left		= textFrameRect.right - w - 8;
						textFrameRect.bottom	= textFrameRect.top + 10;
						NVMC_PaintRoundedRect(textFrameRect,warningTagBackgroundStart,warningTagBackgroundEnd,kGradientType_Vertical,0.7,0.7,//#1316
											  3,false,false,true,false);
						NVMC_FrameRoundedRect(textFrameRect,warningTagFrame,1.0,5,false,false,true,false);//#1316
						//���@�p�[�g���`��
						ALocalRect	textDrawRect = textFrameRect;
						textDrawRect.left		+= 3;
						textDrawRect.top		+= 1;
						textDrawRect.right		-= 3;
						textDrawRect.bottom		-= 1;
						NVMC_DrawText(textDrawRect,syntaxName,kJustification_Center);
						
						//�ʏ�̃t�H���g�ɖ߂�
						NVMC_SetDefaultTextProperty(fontname,normalfontsize,lettercolor,kTextStyle_Normal,isAntiAlias);
					}
				}
			}
		}
		
		//#844
		//���ݍs�`��
		{
			ATextPoint	spt = {0,0}, ept = {0,0};
			SPI_GetSelect(spt,ept);
			AColor	color = kColor_LightGray;
			GetApp().NVI_GetAppPrefDB().GetData_Color(AAppPrefDB::kCurrentLineColor,color);
			ABool	currentLine = ((spt.y==ept.y)&&(lineIndex==spt.y));
			if( currentLine == true )
			{
				//���ݍs�ipaint�̏ꍇ�j
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPaintLineForCurrentLine) == true )
				{
					NVMC_PaintRect(lineLocalRect,color,0.3);
				}
				//#496 ���ݍs�i�����̏ꍇ�j
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDrawLineForCurrentLine) == true )//#844
				{
					ALocalRect	r = lineLocalRect;
					r.top = r.bottom - 1;
					NVMC_PaintRect(r,color,0.7);
				}
			}
		}
		
		/*drop
		//#450
		AFloatNumber	fontsize = normalfontsize * document.SPI_GetLineFontSizeMultiply(lineIndex);
		NVMC_SetDefaultTextProperty(fontname,fontsize,lettercolor,kTextStyle_Normal,isAntiAlias);
		*/
		
		//fprintf(stderr,"DRaw:%d ",lineIndex);
		//char	str[256];
		//sprintf(str,"Draw:%d \n",lineIndex);
		//OutputDebugStringA(str);
		
		//
		CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
		ATextPoint	caretPoint, selectPoint;
		GetCaretTextPoint(caretPoint);
		GetSelectTextPoint(selectPoint);
		ABool	selection = (IsCaretMode()==false);
		if( mKukeiSelected == true )
		{
			selection = false;
			for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
			{
				ATextPoint	spt = mKukeiSelected_Start.Get(i);
				if( spt.y == lineIndex )
				{
					ATextPoint	ept = mKukeiSelected_End.Get(i);
					caretPoint = spt;
					selectPoint = ept;
					selection = true;
					break;
				}
			}
		}
		
		//live resize���Ȃ�style�Ȃ���draw�݂̂��s���B�i�������j #688
		if( NVI_IsInLiveResize() == true )
		{
			document.SPI_GetTextDrawDataWithoutStyle(lineIndex,textDrawData);
			NVMC_DrawText(lineLocalRect,textDrawData);
			continue;
		}
		
		AAnalyzeDrawData	analyzeDrawData;//RC1
		document.SPI_GetTextDrawDataWithStyle(lineIndex,textDrawData,selection,caretPoint,selectPoint,analyzeDrawData,GetObjectID());//RC1
		
		//�w�i�F�擾
		AColor	backgroundColor = kColor_White;//#1316
		modePrefDB.GetModeData_Color(AModePrefDB::kBackgroundColor,backgroundColor);//#1316
		
		//
		ATextIndex	lineStart = document.SPI_GetLineStart(lineIndex);
		ATextIndex	lineEnd = lineStart + document.SPI_GetLineLengthWithoutLineEnd(lineIndex);
		
		//�O�s�J�n�ʒu�擾
		ATextIndex	prevLineStart = lineStart;
		if( lineIndex-1 >= 0 )
		{
			prevLineStart = document.SPI_GetLineStart(lineIndex-1);
		}
		//���s�I���ʒu�擾
		ATextIndex	nextLineEnd = lineEnd;
		if( lineIndex+1 < document.SPI_GetLineCount() )
		{
			nextLineEnd = document.SPI_GetLineStart(lineIndex+1) 
					+ document.SPI_GetLineLengthWithoutLineEnd(lineIndex+1);
		}
		
		//==================���@�`�F�b�J�[���[�j���O�̕\��==================
		//#992
		if( GetTextDocumentConst().SPI_IsParagraphStartLine(lineIndex) == true )
		{
			//�i���ԍ��擾
			AIndex	paraIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(lineIndex);
			//���@�`�F�b�J�[���[�j���O�f�[�^�擾
			ANumberArray	colIndexArray;
			ATextArray	titleArray,detailArray;
			AColorArray	colorArray;
			ABoolArray	displayInTextViewArray;
			GetTextDocumentConst().SPI_GetCheckerPluginWarnings(paraIndex,colIndexArray,titleArray,detailArray,colorArray,displayInTextViewArray);
			//���[�j���O�f�[�^��1�ȏ゠�邩�ǂ����̔���
			if( colIndexArray.GetItemCount() > 0 )
			{
				//TextView���ɕ\�����邩�ǂ����̃t���O�i1�ł�TextView�ɕ\�����ׂ����[�j���O�f�[�^������΁A�t���OON�ɂ���j
				ABool	displayInTextView = false;
				for( AIndex i = 0; i < displayInTextViewArray.GetItemCount(); i++ )
				{
					if( displayInTextViewArray.Get(i) == true )
					{
						/*
						�����Ή��icolIndex��text position�ւ̕ϊ������K�v�B�\���f�U�C�������K�v�B
						if( colIndexArray.Get(i) >= 0 )
						{
							//
							AImagePoint	colImagePoint = {0};
							colImagePoint.x = NVMC_GetImageXByTextPosition(textDrawData,colIndexArray.Get(i));
							colImagePoint.y = GetImageYByLineIndex(lineIndex)+1;
							ALocalPoint	spt = {0}, ept = {0};
							NVM_GetLocalPointFromImagePoint(colImagePoint,spt);
							ept = spt;
							ept.y += 2;
							NVMC_DrawLine(spt,ept,kColor_Gray50Percent,0.5);
						}
						*/
						//�t���OON
						displayInTextView = true;
					}
				}
				//TextView���ɕ\�����邩�ǂ����̔���
				if( displayInTextView == true )
				{
					//���[�j���O�e�L�X�g�擾�i�ŏ��̃��[�j���O�̃f�[�^���擾�j
					AText	warningTitle;
					if( titleArray.GetItemCount() > 0 )
					{
						titleArray.Get(0,warningTitle);
					}
					//�{���̉E�[�̈ʒu���擾
					AImagePoint	imagePoint = {0};
					imagePoint.x = NVMC_GetImageXByTextPosition(textDrawData,lineEnd-lineStart);
					imagePoint.y = GetImageYByLineIndex(lineIndex)+1;
					ALocalPoint	localPoint = {0};
					NVM_GetLocalPointFromImagePoint(imagePoint,localPoint);
					//���[�j���O�\���̈�\���擾
					ANumber	maxWidthForWarning = localFrameRect.right - localPoint.x - 32;
					//�\���\���ǂ����̔���
					if( maxWidthForWarning > 16 )
					{
						//���[�j���O�\���̈�͉�ʂ�1/5���ő�Ƃ���
						if( maxWidthForWarning > (lineLocalRect.right - lineLocalRect.left)/5 )
						{
							maxWidthForWarning = (lineLocalRect.right - lineLocalRect.left)/5;
						}
						//�t�H���g�ݒ�
						AFloatNumber	fs = 9.5;//#0 9.0��9.5
						if( fs > normalfontsize )   fs = normalfontsize;
						AText	f("Helvetica");
						NVMC_SetDefaultTextProperty(f,fs,warningTagLetterColor,kTextStyle_Normal,true);//#1316
						//�e�L�X�g�\�����擾
						ANumber	w = NVMC_GetDrawTextWidth(warningTitle);
						//�e�L�X�g�\���������[�j���O�\���̈�𒴂���ꍇ��...�\���ɂ���
						AText	warningTitleEllipsis;
						if( w > maxWidthForWarning )
						{
							w = maxWidthForWarning;
							NVI_GetEllipsisTextWithFixedLastCharacters(warningTitle,w,0,warningTitleEllipsis,true);
						}
						else
						{
							warningTitleEllipsis.SetText(warningTitle);
						}
						//���[�j���O�\���̈�v�Z
						ALocalRect	textFrameRect = lineLocalRect;
						textFrameRect.left = textFrameRect.right - w - 16;
						textFrameRect.right -= 2;
						textFrameRect.top++;
						if( textFrameRect.bottom - textFrameRect.top > 16 )//#0 14��16
						{
							textFrameRect.bottom	= textFrameRect.top + 16;//#0 14��16
						}
						//�g�`��
						NVMC_PaintRoundedRect(textFrameRect,warningTagBackgroundStart,warningTagBackgroundEnd,kGradientType_Vertical,0.7,0.7,//#1316
											  3,true,true,true,true);
						NVMC_FrameRoundedRect(textFrameRect,warningTagFrame,1.0,5,true,true,true,true);//#1316
						//�e�L�X�g�`��
						ALocalRect	textDrawRect = textFrameRect;
						textDrawRect.left		+= 6;
						textDrawRect.top		+= 1;
						textDrawRect.right		-= 8;
						textDrawRect.bottom		-= 1;
						NVMC_DrawText(textDrawRect,warningTitleEllipsis,kJustification_Left);
						
						//�ʏ�̃t�H���g�ɖ߂�
						NVMC_SetDefaultTextProperty(fontname,normalfontsize,lettercolor,kTextStyle_Normal,isAntiAlias);
					}
				}
			}
		}
		//==================diff�\��==================
		//���ݍs��diff�^�C�v�擾
		ADiffType	difftype = GetTextDocumentConst().SPI_GetDiffTypeByIncludingLineIndex(lineIndex);
		/*
		//#790
		ADiffType	difftype = kDiffType_None;
		AIndex	diffIndex = GetTextDocumentConst().SPI_GetDiffIndexByIncludingLineIndex(lineIndex);
		if( diffIndex != kIndex_Invalid )
		{
			difftype = GetTextDocumentConst().SPI_GetDiffTypeByDiffIndex(diffIndex);
		}
		
		//
		if( mTextWindowID != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsMainSubDiffMode(NVI_GetControlID()) == true )
			{
				diffIndex = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_MainSubDiff_GetDiffIndexByIncludingLineIndex(
							NVI_GetControlID(),lineIndex,difftype);
			}
		}
		*/
		//#790
		//diff�p�[�g�g���\���L���擾
		ABool	diffPartStartLine = false;
		ABool	diffPartEndLine = false;
		AFloatNumber	diffPartFrameLineAlphaMultiply = 3.0;//#1394 ANumber��AFloatNumber
		AFloatNumber	diffLetterChangedLetterMultiply = 0.5;//�ύX�����搔 ����not changed����̑����� #1316 1.0��0.5
		AFloatNumber	diffLetterChangedLinesMultiply = 0.0;//�ύX�s�搔 ����not changed����̑����� #1316 0.3��0.0
		AFloatNumber	diffLetterNotChangedLetterMultiply = 0.4;//�ύX�Ȃ������搔 #1316 
		if( GetApp().NVI_IsDarkMode() == false )
		{
			//���C�g���[�h���̓_�[�N���[�h�������Z���߁B
			diffLetterChangedLetterMultiply += 0.2;
			diffLetterChangedLinesMultiply += 0.3;
		}
		
		//#1316 ��r���[�h�łȂ��ꍇ���㉺���͕\������ if( windowIsDiffMode == true )
		{
			//�J�n���\������
			if( lineIndex-1 >= 0 )
			{
				if( GetTextDocumentConst().SPI_GetDiffTypeByIncludingLineIndex(lineIndex-1) != difftype )
				{
					diffPartStartLine = true;
				}
			}
			else
			{
				diffPartStartLine = true;
			}
			//�I�����\������
			if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
			{
				if( GetTextDocumentConst().SPI_GetDiffTypeByIncludingLineIndex(lineIndex+1) != difftype )
				{
					diffPartEndLine = true;
				}
			}
			else
			{
				diffPartEndLine = true;
			}
			/*
			if( diffPartStartLine == true || diffPartEndLine == true )
			{
				AIndex	lineDiffIndex = GetTextDocumentConst().SPI_GetDiffIndexByIncludingLineIndex(lineIndex);
				ATextPoint	spt = {0,0}, ept = {0,0};
				SPI_GetSelect(spt,ept);
				AIndex	selDiffIndex = GetTextDocumentConst().SPI_GetDiffIndexByIncludingLineIndex(spt.y);
				if( lineDiffIndex == selDiffIndex )
				{
					diffPartFrameLineAlphaMultiply = 8;
				}
			}
			*/
			//#1316
			//��r���[�h�łȂ��ꍇ�A�㉺���͏��������\������
			if( windowIsDiffMode == false )
			{
				if( GetApp().NVI_IsDarkMode() == false )
				{
					//#1394 diffPartFrameLineAlphaMultiply *= 0.5;
					diffPartFrameLineAlphaMultiply = 1.0;//#1394 1��1.0�ŕύX�Ȃ�
				}
				else
				{
					//#1394 diffPartFrameLineAlphaMultiply *= 0.7;
					diffPartFrameLineAlphaMultiply = 1.5;//#1394 2��1.5
				}
			}
		}
		//
		if( difftype != kDiffType_None )
		{
			/*�`��X�V���K�v�ɂȂ�̂ŁA�Ƃ肠�����R�����g�A�E�g
			//#790
			AIndex	diffStartLineIndex = GetTextDocumentConst().SPI_GetDiffStartLineIndexByDiffIndex(diffIndex);
			AIndex	diffEndLineIndex = GetTextDocumentConst().SPI_GetDiffEndLineIndexByDiffIndex(diffIndex);
			AFloatNumber	alphaMul = 1.0;
			if( caretPoint.y >= diffStartLineIndex && caretPoint.y <= diffEndLineIndex )
			{
				alphaMul = 2.0;
			}
			*/
			//
			switch(difftype)
			{
			  case kDiffType_Added:
				{
					AColor	color2 = AColorWrapper::ChangeHSV(diffcolor_added,0,1.6,1.0);
					//NVMC_PaintRectWithHorizontalGradient(lineLocalRect,diffcolor_added,color2,0.08,0.08);
					NVMC_PaintRect(lineLocalRect,diffcolor_added,diffAlpha*0.5);//#1316 �ǉ��s�̐F�͏����������� ��1.0��0.5
					//�g���`��
					//#1316 if( windowIsDiffMode == true )
					{
						if( diffPartStartLine == true )
						{
							ALocalRect	frameLine = lineLocalRect;
							frameLine.bottom = frameLine.top + 1;
							NVMC_PaintRect(frameLine,diffcolor_added,diffAlpha*diffPartFrameLineAlphaMultiply);
						}
						if( diffPartEndLine == true )
						{
							ALocalRect	frameLine = lineLocalRect;
							frameLine.top = frameLine.bottom - 1;
							NVMC_PaintRect(frameLine,diffcolor_added,diffAlpha*diffPartFrameLineAlphaMultiply);
						}
					}
					break;
				}
			  case kDiffType_Deleted:
				{
					if( GetTextDocumentConst().SPI_GetParagraphStartTextIndexFromLineIndex(lineIndex) ==
								GetTextDocumentConst().SPI_GetLineStart(lineIndex ) )//#379 �i���J�n�s�̂�
					{
						ALocalRect	aboverect = lineLocalRect;
						aboverect.bottom = aboverect.top + 3;
						NVMC_PaintRect(aboverect,diffcolor_deleted,diffAlpha);
						//�g���`��
						//#1316 if( windowIsDiffMode == true )
						{
							if( diffPartStartLine == true )
							{
								ALocalRect	frameLine = aboverect;
								frameLine.bottom = frameLine.top + 1;
								NVMC_PaintRect(frameLine,diffcolor_deleted,diffAlpha*diffPartFrameLineAlphaMultiply);
							}
							if( diffPartEndLine == true )
							{
								ALocalRect	frameLine = aboverect;
								frameLine.top = frameLine.bottom - 1;
								NVMC_PaintRect(frameLine,diffcolor_deleted,diffAlpha*diffPartFrameLineAlphaMultiply);
							}
						}
					}
					break;
				}
			  case kDiffType_Changed:
				{
					//NVMC_PaintRect(lineLocalRect,diffcolor_changed,0.1);
					//AColor	color2 = AColorWrapper::ChangeHSV(diffcolor_changed,0,1.6,1.0);
					//NVMC_PaintRectWithHorizontalGradient(lineLocalRect,diffcolor_changed,color2,0.08,0.08);
					NVMC_PaintRect(lineLocalRect,diffcolor_changed,diffAlpha*diffLetterNotChangedLetterMultiply);
					
					//�������ƍ����`�� #379
					AArray<AIndex>	charDiffStartArray, charDiffEndArray;
					GetTextDocument().SPI_GetCharDiffInfo(lineIndex,charDiffStartArray,charDiffEndArray);
					for( AIndex i = 0; i < charDiffStartArray.GetItemCount(); i++ )
					{
						//
						ATextIndex	charDiffStart = charDiffStartArray.Get(i);
						ATextIndex	charDiffEnd = charDiffEndArray.Get(i);
						if( charDiffEnd <= lineStart || charDiffStart > lineEnd )   continue;
						//
						AImageRect	imageRect = lineImageRect;
						if( charDiffStart > lineStart && charDiffStart <= lineEnd )
						{
							imageRect.left = NVMC_GetImageXByTextPosition(textDrawData,charDiffStart-lineStart);
						}
						if( charDiffEnd > lineStart && charDiffEnd <= lineEnd )
						{
							imageRect.right	= NVMC_GetImageXByTextPosition(textDrawData,charDiffEnd-lineStart);
						}
						ALocalRect	localRect;
						NVM_GetLocalRectFromImageRect(imageRect,localRect);
						if( localFrameRect.right < localRect.left )   break;//win ����ȏ�͕`��s�v�Ȃ̂�
						NVMC_PaintRect(localRect,diffcolor_changed,diffAlpha*diffLetterChangedLetterMultiply);
						//�E�C���h�E�����E��r���̏ꍇ�́A�������̍����������l�p�ň͂�
						if( windowIsDiffMode == true )
						{
							NVMC_FrameRect(localRect,diffcolor_changed,diffAlpha*diffPartFrameLineAlphaMultiply*10);//#1316 ��������r�͈͖̔͂ڗ����������̂ŁA���̘g����10�{��alpha�ɂ���B�i���̘g����ڗ�������ƌ��Â炭�Ȃ�̂ŁA�������g�������ɂ���B�j
						}
						//
						if( localRect.left == localRect.right )
						{
							ALocalPoint	spt = {localRect.left,localRect.top-1};
							ALocalPoint	ept = {localRect.left,localRect.bottom-1};
							NVMC_DrawLine(spt,ept,diffcolor_changed,0.5,0.0,2.0);
						}
						/*
						{
							ALocalPoint	spt = {localRect.right,localRect.top};
							ALocalPoint	ept = {localRect.right,localRect.bottom};
							NVMC_DrawLine(spt,ept,diffcolor_changed,0.5);
						}
						*/
					}
					if( charDiffStartArray.GetItemCount() == 0 )
					{
						NVMC_PaintRect(lineLocalRect,diffcolor_changed,diffAlpha*diffLetterChangedLinesMultiply);
					}
					//�g���`��
					//#1316 if( windowIsDiffMode == true )
					{
						if( diffPartStartLine == true )
						{
							ALocalRect	frameLine = lineLocalRect;
							frameLine.bottom = frameLine.top + 1;
							NVMC_PaintRect(frameLine,diffcolor_changed,diffAlpha*diffPartFrameLineAlphaMultiply);
						}
						if( diffPartEndLine == true )
						{
							ALocalRect	frameLine = lineLocalRect;
							frameLine.top = frameLine.bottom - 1;
							NVMC_PaintRect(frameLine,diffcolor_changed,diffAlpha*diffPartFrameLineAlphaMultiply);
						}
					}
					break;
				}
				//�I��͈͔�r���̔�r�͈͊O
			  case kDiffType_OutOfDiffRange:
				{
					NVMC_PaintRect(lineLocalRect,kColor_Gray,diffAlpha*3);
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
		}
		
		//
		if( lineIndex >= document.SPI_GetLineCount() )   continue;
		
		//#450
		//�܂肽���ݔg���\��
		if( SPI_GetFoldingCollapsed(lineIndex) == true )
		{
			ALocalPoint	foldingLineSpt, foldingLineEpt;
			foldingLineSpt.x = lineLocalRect.left;
			foldingLineSpt.y = lineLocalRect.bottom - 3;
			foldingLineEpt.x = lineLocalRect.right;
			foldingLineEpt.y = lineLocalRect.bottom - 3;
			//NVMC_DrawLine(foldingLineSpt,foldingLineEpt,kColor_Orange,0.8,3.0);
			foldingLineSpt.x += 3;
			foldingLineSpt.y++;
			foldingLineEpt.y++;
			NVMC_DrawLine(foldingLineSpt,foldingLineEpt,kColor_Orange,0.7,3.0);
			foldingLineSpt.x -= 3;
			foldingLineSpt.y++;
			foldingLineEpt.y++;
			NVMC_DrawLine(foldingLineSpt,foldingLineEpt,kColor_Orange,0.7,3.0);
		}
		
		//#9 ���s�R�[�h�`��̈ʒu�������n�C���C�g�`��ォ��ړ�
		//���s�R�[�h�`�� B0000 ������
		//#889 AColor	spcolor;
		//#889 modePrefDB.GetData_Color(AModePrefDB::kSpecialCodeColor,spcolor);
		if( document.SPI_GetLineExistLineEnd(lineIndex) == true && drawLineEndCode == true && IsPrinting() == false )
		{
			textDrawData.AddText(mLineEndDisplayText,mLineEndDisplayText_UTF16,spcolor);//#1142 kColor_Gray);//#889 spcolor);//#532
		}
		//==================����R�[�h�ԕ\��==================
		/*#473 textDrawData����controlCodeStartUTF8Index/controlCodeEndUTF8Index�ɏ]���悤�ɕύX
		if( drawControlCode == true )
		{
			for( AIndex pos = lineStart; pos < lineEnd; pos = document.SPI_GetNextCharTextIndex(pos) )
			{
				AUChar	ch = document.SPI_GetTextChar(pos);
				if( ch < 0x20 && ch != kUChar_Tab && ch != kUChar_CR )
				{
					AImageRect	imageRect = lineImageRect;
					imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart) + 1;
					imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart+1) - 1;
					imageRect.top++;
					imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);//#450 GetLineHeightWithoutMargin();
					ALocalRect	localRect;
					NVM_GetLocalRectFromImageRect(imageRect,localRect);
					if( localFrameRect.right < localRect.left )   break;//win ����ȏ�͕`��s�v�Ȃ̂�
					NVMC_PaintRect(localRect,kColor_Red,0.8);
				}
			}
		}
		*/
		//#473 textDrawData����controlCodeStartUTF8Index/controlCodeEndUTF8Index�ɏ]���Đ���R�[�h�ԕ\��
		AItemCount	controlCodeCount = textDrawData.controlCodeStartUTF8Index.GetItemCount();
		for( AIndex i = 0; i < controlCodeCount; i++ )
		{
			//�J�n�E�I���ʒu�擾
			AIndex	s = textDrawData.controlCodeStartUTF8Index.Get(i);
			AIndex	e = textDrawData.controlCodeEndUTF8Index.Get(i);
			//�ԕ\���̈�v�Z
			AImageRect	imageRect = lineImageRect;
			imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,s) + 1;
			imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,e) - 1;
			imageRect.top++;
			imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
			ALocalRect	localRect;
			NVM_GetLocalRectFromImageRect(imageRect,localRect);
			if( localFrameRect.right < localRect.left )   break;
			//�`��
			NVMC_PaintRoundedRect(localRect,kColor_Red,kColor_Red,kGradientType_None,0.8,0.8,3,true,true,true,true);
			//�t���[���O�ɂȂ����烋�[�v�I���i�������̂��߁j
			if( imageRect.left > imageFrameRect.right )
			{
				break;
			}
		}
		
		//==================�q���g�\��==================
		AHashNumberArray	hintTextStartUTF8IndexHashArray;
		AItemCount	hintTextCount = textDrawData.hintTextStartUTF8Index.GetItemCount();
		/*#1316 �������B�s���ƂɎ擾����K�v���Ȃ��̂ŁB
		AColor	selectionColor = kColor_Black;
		AColorWrapper::GetHighlightColor(selectionColor);
		*/
		for( AIndex i = 0; i < hintTextCount; i++ )
		{
			//
			AIndex	s = textDrawData.hintTextStartUTF8Index.Get(i);
			hintTextStartUTF8IndexHashArray.Add(s);
			AIndex	e = textDrawData.hintTextEndUTF8Index.Get(i);
			//�ԕ\���̈�v�Z
			AImageRect	imageRect = lineImageRect;
			imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,s) + 1;
			imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,e) - 1;
			imageRect.top++;
			imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
			ALocalRect	localRect;
			NVM_GetLocalRectFromImageRect(imageRect,localRect);
			if( localFrameRect.right < localRect.left )   break;
			//�`��
			NVMC_PaintRoundedRect(localRect,selectionColor,selectionColor,kGradientType_None,0.5,0.5,999,true,true,true,true);
			NVMC_FrameRoundedRect(localRect,kColor_Blue,1.0,999,true,true,true,true);
			//�t���[���O�ɂȂ����烋�[�v�I���i�������̂��߁j
			if( imageRect.left > imageFrameRect.right )
			{
				break;
			}
		}
		
        //�i���J�n�ʒu�E�I���ʒu�擾
        AIndex    paragraphStartLineIndex = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(lineIndex);
        //AIndex    paragraphEndLineIndex = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(lineIndex);
        ATextIndex    paraStart = GetTextDocumentConst().SPI_GetLineStart(paragraphStartLineIndex);
        ATextIndex    paraEnd = GetTextDocumentConst().SPI_GetNextLineEndCharIndex(paraStart);//�܂�Ԃ��v�Z�Ώۍs�̏ꍇ�A1�s�ɂ�������̕���������̂ŁA���s�R�[�h�܂łɌ��肷��
        
		//#1316 �����ʒu�������n�C���C�g�̌ォ��O�ֈړ��B
		//#750
		//==================���ݒP��n�C���C�g==================
		{
			AColor	color = kColor_Yellow;
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kCurrentWordHighlightColor,color);//#844
			//GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor,color);
			//B0358 �i���S�̂Ō�������悤�ɏC��
			//�i�����Ō������邪�A�O�s�̍ŏ����玟�s�̍Ō�܂ł��ő�͈͂Ƃ���
			AIndex	start = paraStart;
			AIndex	end = paraEnd;
			if( start < prevLineStart )
			{
				//�O�s�̍ŏ����猟������
				start = prevLineStart;
			}
			if( end > nextLineEnd )
			{
				//���s�̍Ō�܂ł���������
				end = nextLineEnd;
			}
			for( ATextIndex pos = start; pos < end; )
			{
				//#695 �i������������ꍇ�ɏ������Ԃ����肷����̂�h�~
				//if( pos > paraStart + kLimit_TextHighlightFindRange )   break;
				//
				ATextIndex	spos, epos;
				if( GetTextDocument().SPI_FindForCurrentWordHighlight(pos,end,spos,epos) == false )   break;
				if( epos > spos )
				{
					pos = epos;
					if( spos < lineStart )   spos = lineStart;
					if( epos > lineEnd )   epos = lineEnd;
					if( epos < lineStart )   epos = lineStart;//�s�v���Ǝv�����O�̂���
					if( spos > lineEnd )   spos = lineEnd;//�s�v���Ǝv�����O�̂���
					if( spos < epos )
					{
						AImageRect	imageRect = lineImageRect;
						imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,spos-lineStart);
						imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,epos-lineStart);
						imageRect.top;
						imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
						ALocalRect	localRect;
						NVM_GetLocalRectFromImageRect(imageRect,localRect);
						//�_�[�N���[�h���ǂ����̔��� #1316
						if( GetApp().NVI_IsDarkMode() == false )
						{
							NVMC_PaintRect(localRect,color,selectionOpacity/2.0);
						}
						else
						{
							//�_�[�N���[�h�̏ꍇ�́A�h��Ԃ��𔖂����� #1316
							NVMC_PaintRect(localRect,color,selectionOpacity*0.5);
							NVMC_FrameRect(localRect,color,0.05);
						}
						//�t���[���O�ɂȂ����烋�[�v�I���i�������̂��߁j
						if( imageRect.left > imageFrameRect.right )
						{
							break;
						}
					}
				}
				else
				{
					//spos==epos�̏ꍇ�i���K�\�������ł͂��肤��j
					pos += GetTextDocument().SPI_GetNextCharTextIndex(pos);
				}
			}
		}
		
		//==================�����n�C���C�g�i�O���v�j==================
		
		//�����n�C���C�g2 R0244
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFindHighlight2) == true )
		{
			//R0244
			AColor	color;
			//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor2,color);
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kFindHighlightColorPrev,color);//#844
			//B0358 �i���S�̂Ō�������悤�ɏC��
			for( ATextIndex pos = paraStart; pos < paraEnd; )
			{
				//#695 �i������������ꍇ�ɏ������Ԃ����肷����̂�h�~
				if( pos > paraStart + kLimit_TextHighlightFindRange )   break;
				//
				ATextIndex	spos, epos;
				if( GetTextDocument().SPI_FindForHighlight2(pos,paraEnd,spos,epos) == false )   break;
				if( epos > spos )
				{
					pos = epos;
					if( spos < lineStart )   spos = lineStart;
					if( epos > lineEnd )   epos = lineEnd;
					if( epos < lineStart )   epos = lineStart;//�s�v���Ǝv�����O�̂���
					if( spos > lineEnd )   spos = lineEnd;//�s�v���Ǝv�����O�̂���
					if( spos < epos )
					{
						AImageRect	imageRect = lineImageRect;
						imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,spos-lineStart);
						imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,epos-lineStart);
						imageRect.top;
						imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);//#450 GetLineHeightWithoutMargin();
						ALocalRect	localRect;
						NVM_GetLocalRectFromImageRect(imageRect,localRect);
						/*R0244 AColor	color;
						GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor,color);*/
						//�_�[�N���[�h���ǂ����̔��� #1316
						if( GetApp().NVI_IsDarkMode() == false )
						{
							NVMC_PaintRect(localRect,color,selectionOpacity);
						}
						else
						{
							//�_�[�N���[�h�̏ꍇ�́A�h��Ԃ��𔖂����� #1316
							NVMC_PaintRect(localRect,backgroundColor,1.0);//���̓h��Ԃ����L�����Z�����邽�߂܂��w�i�F�œh��
							NVMC_PaintRect(localRect,color,selectionOpacity*0.5);
							NVMC_FrameRect(localRect,color,0.05);
						}
						//�t���[���O�ɂȂ����烋�[�v�I���i�������̂��߁j
						if( imageRect.left > imageFrameRect.right )
						{
							break;
						}
					}
				}
				else
				{
					//spos==epos�̏ꍇ�i���K�\�������ł͂��肤��j
					pos += GetTextDocument().SPI_GetNextCharTextIndex(pos);
				}
			}
		}
		//==================�����n�C���C�g==================
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFindHighlight) == true )
		{
			//R0244
			AColor	color;
			//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor,color);
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kFindHighlightColor,color);//#844
			//B0358 �i���S�̂Ō�������悤�ɏC��
			for( ATextIndex pos = paraStart; pos < paraEnd; )
			{
				//#695 �i������������ꍇ�ɏ������Ԃ����肷����̂�h�~
				if( pos > paraStart + kLimit_TextHighlightFindRange )   break;
				//
				ATextIndex	spos, epos;
				if( GetTextDocument().SPI_FindForHighlight(pos,paraEnd,spos,epos) == false )   break;
				if( epos > spos )
				{
					pos = epos;
					if( spos < lineStart )   spos = lineStart;
					if( epos > lineEnd )   epos = lineEnd;
					if( epos < lineStart )   epos = lineStart;//�s�v���Ǝv�����O�̂���
					if( spos > lineEnd )   spos = lineEnd;//�s�v���Ǝv�����O�̂���
					if( spos < epos )
					{
						AImageRect	imageRect = lineImageRect;
						imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,spos-lineStart);
						imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,epos-lineStart);
						imageRect.top;
						imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);//#450 GetLineHeightWithoutMargin();
						ALocalRect	localRect;
						NVM_GetLocalRectFromImageRect(imageRect,localRect);
						/*R0244 AColor	color;
						GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor,color);*/
						//�_�[�N���[�h���ǂ����̔��� #1316
						if( GetApp().NVI_IsDarkMode() == false )
						{
							NVMC_PaintRect(localRect,color,selectionOpacity);
						}
						else
						{
							//�_�[�N���[�h�̏ꍇ�́A�h��Ԃ��𔖂����� #1316
							NVMC_PaintRect(localRect,backgroundColor,1.0);//���̓h��Ԃ����L�����Z�����邽�߂܂��w�i�F�œh��
							NVMC_PaintRect(localRect,color,selectionOpacity*0.5);
							NVMC_FrameRect(localRect,color,0.05);
						}
						//�t���[���O�ɂȂ����烋�[�v�I���i�������̂��߁j
						if( imageRect.left > imageFrameRect.right )
						{
							break;
						}
					}
				}
				else
				{
					//spos==epos�̏ꍇ�i���K�\�������ł͂��肤��j
					pos += GetTextDocument().SPI_GetNextCharTextIndex(pos);
				}
			}
		}
		//#853
		//==================�L�[���[�h�������n�C���C�g==================
		if( mIdInfoCurrentArgHighlighted == true )
		{
			//�n�C���C�g���ǂ����̔���
			if( mIdInfoCurrentArgStartTextPoint.y == lineIndex && mIdInfoCurrentArgEndTextPoint.y == lineIndex )
			{
				//�n�C���C�g�`��
				AImageRect	imageRect = lineImageRect;
				imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,mIdInfoCurrentArgStartTextPoint.x);
				imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,mIdInfoCurrentArgEndTextPoint.x);
				imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				AColor	color = kColor_Yellow;
				GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kCurrentWordHighlightColor,color);//#844
				NVMC_PaintRect(localRect,color,selectionOpacity/2.0);
			}
		}
		//#1406
		//==================���ʃn�C���C�g==================
		if( mBraceHighlighted == true )
		{
			//�J�n���ʃn�C���C�g
			if( mBraceHighlightStartTextPoint1.y == lineIndex && mBraceHighlightEndTextPoint1.y == lineIndex )
			{
				//�n�C���C�g�`��
				AImageRect	imageRect = lineImageRect;
				imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,mBraceHighlightStartTextPoint1.x);
				imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,mBraceHighlightEndTextPoint1.x);
				imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				AColor	color = kColor_Yellow;
				GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kFirstSelectionColor,color);
				NVMC_PaintRect(localRect,color,selectionOpacity);
			}
			//�I�����ʃn�C���C�g
			if( mBraceHighlightStartTextPoint2.y == lineIndex && mBraceHighlightEndTextPoint2.y == lineIndex )
			{
				//�n�C���C�g�`��
				AImageRect	imageRect = lineImageRect;
				imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,mBraceHighlightStartTextPoint2.x);
				imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,mBraceHighlightEndTextPoint2.x);
				imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				AColor	color = kColor_Yellow;
				GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kFirstSelectionColor,color);
				NVMC_PaintRect(localRect,color,selectionOpacity);
			}
		}
		//#750
		//==================�e�L�X�g�n�C���C�g==================
		{
			AColor	color = kColor_Bisque;
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kTextMarkerHightlightColor,color);//#844
			//GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor,color);
			//B0358 �i���S�̂Ō�������悤�ɏC��
			//�i�����Ō������邪�A�O�s�̍ŏ����玟�s�̍Ō�܂ł��ő�͈͂Ƃ���
			AIndex	start = paraStart;
			AIndex	end = paraEnd;
			if( start < prevLineStart )
			{
				//�O�s�̍ŏ����猟������
				start = prevLineStart;
			}
			if( end > nextLineEnd )
			{
				//���s�̍Ō�܂ł���������
				end = nextLineEnd;
			}
			for( ATextIndex pos = start; pos < end; )
			{
				//#695 �i������������ꍇ�ɏ������Ԃ����肷����̂�h�~
				//if( pos > paraStart + kLimit_TextHighlightFindRange )   break;
				//
				ATextIndex	spos, epos;
				if( GetTextDocument().SPI_FindForTextMarker(pos,end,spos,epos) == false )   break;
				if( epos > spos )
				{
					pos = epos;
					if( spos < lineStart )   spos = lineStart;
					if( epos > lineEnd )   epos = lineEnd;
					if( epos < lineStart )   epos = lineStart;//�s�v���Ǝv�����O�̂���
					if( spos > lineEnd )   spos = lineEnd;//�s�v���Ǝv�����O�̂���
					if( spos < epos )
					{
						AImageRect	imageRect = lineImageRect;
						imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,spos-lineStart);
						imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,epos-lineStart);
						imageRect.top;
						imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
						ALocalRect	localRect;
						NVM_GetLocalRectFromImageRect(imageRect,localRect);
						NVMC_PaintRect(localRect,color,selectionOpacity);
						//�t���[���O�ɂȂ����烋�[�v�I���i�������̂��߁j
						if( imageRect.left > imageFrameRect.right )
						{
							break;
						}
					}
				}
				else
				{
					//spos==epos�̏ꍇ�i���K�\�������ł͂��肤��j
					pos += GetTextDocument().SPI_GetNextCharTextIndex(pos);
				}
			}
		}
		//#9 ���s�R�[�h�`��̈ʒu�������n�C���C�g�`��O�ֈړ�
		
		//#913
		//==================�}�[�N==================
		{
			//�}�[�N�J�n�ʒu�`��
			if( mMarkStartTextPoint.y == lineIndex )
			{
				AImageRect	imageRect = lineImageRect;
				imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,mMarkStartTextPoint.x) - 1;
				imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,mMarkStartTextPoint.x) + 1;
				imageRect.top;
				imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				NVMC_PaintRoundedRect(localRect,kColor_Red,kColor_Red,kGradientType_None,0.8,0.8,2,true,false,true,false);
			}
			//�}�[�N�I���ʒu�`��
			if( mMarkEndTextPoint.y == lineIndex )
			{
				AImageRect	imageRect = lineImageRect;
				imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,mMarkEndTextPoint.x) - 1;
				imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,mMarkEndTextPoint.x) + 1;
				imageRect.top;
				imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				NVMC_PaintRoundedRect(localRect,kColor_Red,kColor_Red,kGradientType_None,0.8,0.8,2,false,true,false,true);
			}
		}
		
		//==================�e�L�X�g�`��==================
		textDrawData.selectionColor = AColorWrapper::ChangeHSV(selectionColor,0,1,1);
		textDrawData.selectionAlpha = selectionOpacity;
		textDrawData.selectionBackgroundColor = backgroundColor;//#1316
		//#262
		//��������I��͈͕\��
		if( /*#844 GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kEnableSelectionColorForFind) == true &&*/
					mSelectionByFind == true )
		{
			//#844 GetApp().NVI_GetAppPrefDB().GetData_Color(AAppPrefDB::kSelectionColorForFind,textDrawData.selectionColor);
			modePrefDB.GetModeData_Color(AModePrefDB::kFirstSelectionColor,textDrawData.selectionColor);
			//�_�[�N���[�h�̏ꍇ�̓�=1.0�ɂ��ĕ����F���]������ #1316
			if( GetApp().NVI_IsDarkMode() == true )
			{
				if( textDrawData.drawSelection == true )
				{
					textDrawData.selectionAlpha = 1.0;
					textDrawData.AddAttributeWithUnicodeOffset(textDrawData.selectionStart,textDrawData.selectionEnd,backgroundColor);
				}
			}
		}
		//�e�L�X�g�`����s
		/*B0000 ������ ANumber	textWidth = */NVMC_DrawText(lineLocalRect,textDrawData);
		
		//==================���ꕶ���\��==================
		NVMC_SetDrawLineMode(spcolor);//#1142 lettercolor,0.3);//#889 spcolor);//B0000 ������
		AItemCount	spaceToIndentSpaceCount = GetTextDocumentConst().SPI_GetIndentWidth();//#117
		ABool   startSpaceToIndent = modePrefDB.GetData_Bool(AModePrefDB::kStartSpaceToIndent);//#117
		for( AIndex pos = lineStart; pos < lineEnd; pos = document.SPI_GetNextCharTextIndex(pos) )
		{
			//�����l�ȍ~�͕`�悵�Ȃ�#695
			if( pos > lineStart+kLimit_TextViewDoDraw_MaxXPosToDrawSpecialCharacters )
			{
				break;
			}
			//#117
			if( startSpaceToIndent == true )
			{
				if( textDrawData.startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex.Find(pos-lineStart) != kIndex_Invalid )
				{
					if( drawTab == true )
					{
						AImagePoint	imageStartPoint, imageEndPoint;
						imageStartPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart);
						imageStartPoint.y = GetImageYByLineIndex(lineIndex) + GetLineAscent(lineIndex)/*#450 mLineAscent*/ +1;
						imageEndPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart+spaceToIndentSpaceCount) - 2;
						imageEndPoint.y = imageStartPoint.y;
						ALocalPoint	spt, ept;
						NVM_GetLocalPointFromImagePoint(imageStartPoint,spt);
						NVM_GetLocalPointFromImagePoint(imageEndPoint,ept);
						NVMC_DrawLine(spt,ept);
						spt.x = ept.x;
						spt.y = ept.y -3;
						NVMC_DrawLine(spt,ept);
						//�t���[���O�ɂȂ����烋�[�v�I���i�������̂��߁j
						if( imageStartPoint.x > imageFrameRect.right )
						{
							break;
						}
					}
					pos += spaceToIndentSpaceCount - 1;
					continue;
				}
			}
			//
			AUChar	ch = document.SPI_GetTextChar(pos);
			//���s���������炻��ȍ~�͕`�悵�Ȃ�#699
			if( ch == kUChar_LineEnd )
			{
				break;
			}
			//
			switch(ch)
			{
				//�^�u�`��
			  case kUChar_Tab:
				{
					if( drawTab == true )
					{
						AImagePoint	imageStartPoint, imageEndPoint;
						imageStartPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart);
						imageStartPoint.y = GetImageYByLineIndex(lineIndex) + GetLineAscent(lineIndex)/*#450 mLineAscent*/ +1;
						imageEndPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart+1) - 2;
						imageEndPoint.y = imageStartPoint.y;
						ALocalPoint	spt, ept;
						NVM_GetLocalPointFromImagePoint(imageStartPoint,spt);
						NVM_GetLocalPointFromImagePoint(imageEndPoint,ept);
						NVMC_DrawLine(spt,ept);//B0000,spcolor);
						spt.x = ept.x - 1;//mLineAscent/5;
#if IMPLEMENTATION_FOR_WINDOWS
						spt.x--;
#endif
						spt.y = ept.y - 2;//mLineAscent/4;
						NVMC_DrawLine(spt,ept);//B0000,spcolor);
						//�t���[���O�ɂȂ����烋�[�v�I���i�������̂��߁j
						if( imageStartPoint.x > imageFrameRect.right )
						{
							break;
						}
					}
					break;
				}
				//���p�X�y�[�X�`��
			  case kUChar_Space:
				{
					if( drawSpace == true )
					{
						AImagePoint	imageStartPoint, imageEndPoint; 
						imageStartPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart);
						imageStartPoint.y = GetImageYByLineIndex(lineIndex) + GetLineAscent(lineIndex)/*#450 mLineAscent*/ +1;
						imageEndPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart+1) - 2;
						imageEndPoint.y = imageStartPoint.y;
						ALocalPoint	spt, ept;
						NVM_GetLocalPointFromImagePoint(imageStartPoint,spt);
						NVM_GetLocalPointFromImagePoint(imageEndPoint,ept);
						NVMC_DrawLine(spt,ept);//B0000,spcolor);
						ALocalPoint	pt;
						pt.x = spt.x;
						pt.y = spt.y - 2;
						NVMC_DrawLine(spt,pt);//B0000,spcolor);
						pt.x = ept.x;
						pt.y = ept.y - 2;
						NVMC_DrawLine(ept,pt);//B0000,spcolor);
						//�t���[���O�ɂȂ����烋�[�v�I���i�������̂��߁j
						if( imageStartPoint.x > imageFrameRect.right )
						{
							break;
						}
					}
					break;
				}
				//�S�p�X�y�[�X�`��
			  case 0xE3:
				{
					if( drawZenkakuSpace == true )
					{
						if( pos+2 < lineEnd )
						{
							if( document.SPI_GetTextChar(pos+1) == 0x80 && document.SPI_GetTextChar(pos+2) == 0x80 )
							{
								//#450
								AFloatNumber	z = mZenkakuSpaceWidth;
								z *= mLineImageInfo_Height.Get(lineIndex);
								z /= (mLineHeight+mLineMargin);
								//
								AImageRect	imageRect = lineImageRect;
								imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart) + z/5;
								imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart+3) - z/5;
								imageRect.top		= GetImageYByLineIndex(lineIndex) + GetLineAscent(lineIndex)/*#450 mLineAscent*/ - z*4/5;
								imageRect.bottom	= GetImageYByLineIndex(lineIndex) + GetLineAscent(lineIndex)/*#450 mLineAscent*/ +1;
								ALocalRect	localRect;
								NVM_GetLocalRectFromImageRect(imageRect,localRect);
								NVMC_FrameRect(localRect,spcolor,1.0);//#1142 lettercolor,0.3);//#889 spcolor,1.0);
								//�t���[���O�ɂȂ����烋�[�v�I���i�������̂��߁j
								if( imageRect.left > imageFrameRect.right )
								{
									break;
								}
							}
						}
					}
					break;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
		}
		NVMC_RestoreDrawLineMode();//B0000 ������
		//==================InlineInput �n�C���C�g�����\��==================
		if( document.SPI_GetDisableInlineInputHilite() == false )//#187
		{
			for( AIndex index = 0; index < mHiliteLineStyleIndex.GetItemCount(); index++ )
			{
				AIndex	hiliteLineStyleIndex = mHiliteLineStyleIndex.Get(index);
				if( hiliteLineStyleIndex == kIndex_Invalid )   continue;
				/*#836
				AColor	color;
				GetApp().SPI_GetInlineInputHiliteColor(hiliteLineStyleIndex,color);
				*/
				//#836
				//�w�i�����̉��������₷���悤�ɂ���B
				AColor	color = lettercolor;
				AFloatNumber	lineWidth = 2.0;//#1018
				//AFloatNumber	h = 0, s = 0, v = 0;
				//AColorWrapper::GetHSVFromRGBColor(lettercolor,h,s,v);
				switch( hiliteLineStyleIndex )
				{
					//v2.1��gray���������́i�����Cocoa�ł͖��g�p��������Ȃ��j
				  case 0:
				  case 1:
					{
						color = kColor_Gray;
						lineWidth = 1.0;//#1018
						break;
					}
					//v2.1��60% gray����������
				  case 2:
					{
						color = kColor_Gray60Percent;
						lineWidth = 1.0;//#1018
						break;
					}
					//v2.1��black����������
				  default:
					{
						//���������ilettercolor�̂܂܁j
						break;
					}
				}
				//
				ATextPoint	spt, ept;
				document.SPI_GetTextPointFromTextIndex(mHiliteStartPos.Get(index),spt);
				document.SPI_GetTextPointFromTextIndex(mHiliteEndPos.Get(index),ept);
				if( lineIndex >= spt.y && lineIndex <= ept.y )
				{
					ATextPoint	lineTextStart, lineTextEnd;
					if( lineIndex == spt.y )
					{
						lineTextStart.x = spt.x;
					}
					else
					{
						lineTextStart.x = 0;
					}
					lineTextStart.y = lineIndex;
					if( lineIndex == ept.y )
					{
						lineTextEnd.x = ept.x;
					}
					else
					{
						lineTextEnd.x = document.SPI_GetLineLengthWithoutLineEnd(lineIndex);
					}
					lineTextEnd.y = lineIndex;
					AImagePoint	lineImageStart, lineImageEnd;
					GetImagePointFromTextPoint(lineTextStart,lineImageStart);
					GetImagePointFromTextPoint(lineTextEnd,lineImageEnd);
					ALocalPoint	lineLocalStart, lineLocalEnd;
					NVM_GetLocalPointFromImagePoint(lineImageStart,lineLocalStart);
					NVM_GetLocalPointFromImagePoint(lineImageEnd,lineLocalEnd);
					lineLocalStart.y += GetLineHeightWithoutMargin(lineIndex) -2;//#450 GetLineHeightWithoutMargin() -1;
					lineLocalEnd.y += GetLineHeightWithoutMargin(lineIndex) -2;//#450 GetLineHeightWithoutMargin() -1;
					lineLocalStart.x += 2;//#1018
					lineLocalEnd.x -= 2;//#185
					if( lineLocalEnd.x < lineLocalStart.x )   lineLocalEnd.x = lineLocalStart.x;//#185
					NVMC_DrawLine(lineLocalStart,lineLocalEnd,color,1.0,0.0,lineWidth);
				}
			}
		}
	}
	//win�@EVTDO_DrawPostProcess()�ֈړ� RestoreTempHideCaret();
	
	//RC1
	const AAnalyzeDrawData&	localAnalyzeDrawData = document.SPI_GetCurrentLocalIdentifierAnalyzeDrawData();
	if( document.SPI_GetDrawArrowToDef() == true )
	{
		for( AIndex i = 0; i < localAnalyzeDrawData.defStartArrowToDef.GetItemCount(); i++ )
		{
			ATextPoint	tdefspt, tdefept, trefspt, trefept;
			tdefspt = localAnalyzeDrawData.defStartArrowToDef.Get(i);
			tdefept = localAnalyzeDrawData.defEndArrowToDef.Get(i);
			trefspt = localAnalyzeDrawData.refStartArrowToDef.Get(i);
			trefept = localAnalyzeDrawData.refEndArrowToDef.Get(i);
			AImagePoint	idefspt, idefept, irefspt, irefept;
			GetImagePointFromTextPoint(tdefspt,idefspt);
			GetImagePointFromTextPoint(tdefept,idefept);
			GetImagePointFromTextPoint(trefspt,irefspt);
			GetImagePointFromTextPoint(trefept,irefept);
			ALocalPoint	defspt, defept, refspt, refept;
			NVM_GetLocalPointFromImagePoint(idefspt,defspt);
			NVM_GetLocalPointFromImagePoint(idefept,defept);
			NVM_GetLocalPointFromImagePoint(irefspt,refspt);
			NVM_GetLocalPointFromImagePoint(irefept,refept);
			defspt.y += GetLineHeightWithoutMargin(tdefspt.y);//#450 GetLineHeightWithoutMargin();
			defept.y += GetLineHeightWithoutMargin(tdefspt.y);//#450 GetLineHeightWithoutMargin();
			refspt.y += GetLineHeightWithoutMargin(trefspt.y);//#450 GetLineHeightWithoutMargin();
			refept.y += GetLineHeightWithoutMargin(trefspt.y);//#450 GetLineHeightWithoutMargin();
			NVMC_DrawLine(defspt,defept,kColor_Red,0.5);
			NVMC_DrawLine(refspt,refept,kColor_Red,0.5,1.0);
			ALocalPoint	spt, ept;
			ept.x = (defspt.x+defept.x)/2;
			ept.y = defspt.y;
			spt.x = (refspt.x+refept.x)/2;
			spt.y = refspt.y;
			NVMC_DrawLine(spt,ept,kColor_Red,0.5,1.0);
		}
	}
	
	//�h���b�O���̃h���b�O��L�����b�g�`�� #1332 XorDragCaret()�̓��e�������Ɉړ��B�}�E�X�h���b�O����lockFocus�`�攽�f����Ȃ��悤�Ȃ̂ŁAdrawRect()���[�g�ŕ`�悷��B
	if( mDragDrawn == true )
	{
		AImagePoint	caretImageStart, caretImageEnd;
		GetImagePointFromTextPoint(mDragCaretTextPoint,caretImageStart);
		caretImageEnd = caretImageStart;
		caretImageEnd.y += GetLineHeightWithMargin(mDragCaretTextPoint.y);//#450 GetLineHeightWithMargin();
		ALocalPoint	caretLocalStart, caretLocalEnd;
		NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
		NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
		//�L�����b�g�F�擾�i�����F�Ɠ����ɂ���j
		AColor	modeLetterColor = kColor_Black;
		GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Color(AModePrefDB::kLetterColor,modeLetterColor);
		//�L�����b�g��������
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kBigCaret) == false )//#722
		{
			//����1
			NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,true,false,1);//#1034 ���flush����
		}
		//#722 drag���̃L�����b�g������ʏ�Ɠ����ɂ���
		else
		{
			//����2
			caretLocalStart.y -= 1;
			caretLocalEnd.y += 1;
			NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,true,false,2);//#1034 ���flush����
		}
	}
	//�h���b�O���̃��[�h�h���b�O��L�����b�g�`�� #1332 XorDragCaret()�̓��e�������Ɉړ��B�}�E�X�h���b�O����lockFocus�`�攽�f����Ȃ��悤�Ȃ̂ŁAdrawRect()���[�g�ŕ`�悷��B
	if( mDragWordDragDrop == true )
	{
		///�L�����b�g�F�擾�i�����F�Ɠ����ɂ���j
		AColor	modeLetterColor = kColor_Black;
	  GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetModeData_Color(AModePrefDB::kLetterColor,modeLetterColor);
		//
		AImageRect	imageRect;
		GetLineImageRect(mDragCaretTextPoint.y,imageRect);
		AImagePoint	caretImagePoint, selectImagePoint;
		GetImagePointFromTextPoint(mDragCaretTextPoint,caretImagePoint);
		GetImagePointFromTextPoint(mDragSelectTextPoint,selectImagePoint);
		imageRect.left	= caretImagePoint.x;
		imageRect.right	= selectImagePoint.x;
		ALocalRect	localRect;
		NVM_GetLocalRectFromImageRect(imageRect,localRect);
		NVMC_DrawXorCaretRect(localRect,true,true);//#1034 ���flush����
	}
}

//win
//�`��O����
void	AView_Text::EVTDO_DrawPreProcess()
{
	TempHideCaret();
}

//win
//�`��㏈��
void	AView_Text::EVTDO_DrawPostProcess()
{
	RestoreTempHideCaret();
	
	//#138
	//�tⳎ�
	//#858 SPI_UpdateFusen(false);
}

//#138
//�\���^��\���؂�ւ�
void	AView_Text::EVTDO_PreProcess_SetShowControl( const ABool inVisible )
{
	//�tⳎ�
	//#858 SPI_UpdateFusenVisible();
}

//�}�E�X�z�C�[��
void	AView_Text::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
										const ALocalPoint inLocalPoint )//win 080706
{
	//fprintf(stderr,"%16X EVTDO_DoMouseWheel() - Start\n",(int)(GetObjectID().val));
	
	//#306 Option+�z�C�[���Ńt�H���g�T�C�Y�ύX
	if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true )
	{
		//���݂̃T�C�Y�擾
		AFloatNumber	fontsize = GetTextDocument().SPI_GetFontSize();
		//0.5�P�ʂŕύX
		/* #966 0.5�P�ʂɂȂ�Ȃ������C��
		ANumber	delta1 = inDeltaY*5;
		AFloatNumber	delta2 = delta1;
		delta2 = delta2/10;
		fontsize += delta2;
		*/
		if( inDeltaY > 0 )
		{
			fontsize += 0.5;
		}
		else if( inDeltaY < 0 )
		{
			fontsize -= 0.5;
		}
		//�T�C�Y�ݒ�
		GetTextDocument().SPI_SetTemporaryFontSize(fontsize);//#966 �ꎞ�t�H���g�T�C�Y�ύX�֕ύX
		//���j���[�X�V
		GetApp().NVI_UpdateMenu();
		return;
	}
	
	//���������z�C�[���֎~�Ȃ���deltaX��0�ɂ���
	ANumber	deltaX = inDeltaX;
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisableHorizontalWheelScroll) == true )
	{
		deltaX = 0;
	}
	//�X�N���[������ݒ肩��擾����
	ANumber	scrollPercent;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true 
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win 080702
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd);
	}
	else
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent);
	}
	//�X�N���[�����s
	NVI_Scroll(deltaX*NVI_GetHScrollBarUnit()*scrollPercent/100,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100,
			   true,true,kScrollTrigger_Wheel);
	
	//fprintf(stderr,"%16X EVTDO_DoMouseWheel() - End\n",(int)(GetObjectID().val));
}

//
ABool	AView_Text::EVTDO_DoInlineInput( const AText& inText, const AItemCount inFixLen,
		const AArray<AIndex>& inHiliteLineStyleIndex, const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
		const AItemCount inReplaceLengthInUTF16,//#688
		ABool& outUpdateMenu )
{
	/*
	���ȃ��[�h����control+option+@,[,]����͂����0x1D�������͂�������΍􂵂悤�Ƃ������A
	inFixLen��0�ɂȂ��Ă��Ȃ��i�ϊ�����ԂɂȂ��Ă���j�B�߂�ǂ��̂łƂ肠�����΍􂵂Ȃ��B
	AModifierKeys modifierKeys = AKeyWrapper::GetCurrentModifierKeys();
	if( mInlineInputNotFixedTextLen == 0 && inFixLen == 1 && inText.GetItemCount() == 1 )
	{
		AUChar	ch = inText.Get(0);
		if( ch >= 0x01 && ch < 0x20 && ch != 0x09 && ch != 0x0D && modifierKeys != 0 )
		{
			ABool	updateMenu = false;
			if( EVTDO_DoTextInput(inText,kKeyBindKey_Invalid,modifierKeys,keyAction_NOP,updateMenu) == true )
			{
				return true;
			}
		}
	}
	*/
	
	//#867
	//Idle work�^�C�}�[�N���A
	ResetKeyIdleWorkTimer(kKeyIdleWorkTimerValue);
	
	RestoreFromBraceCheck();
	//�⊮���͏�ԉ���
	ClearAbbrevInputMode(true,false);//RC2
	//
	if( KeyTool(inText) == true )   return true;
	if( GetTextDocumentConst().NVI_IsReadOnly() == true )   return false;
	//B0000
	//win ���ֈړ� ACursorWrapper::ObscureCursor();
	
	//#688
	//tick timer�����O�ɕ������͂��ꂽ�ꍇ�ɁA������reserve���ꂽcaret�`������s����B
	//�i�����ŕ`�悵�Ȃ��ƃI�[�g���s�[�g�����Ƃ����A�L�����b�g�`�悪�x��Č�����j
	ShowCaretIfReserved();
	
	//���Xdirty�Ȃ烁�j���[��Ԃ͕ς��Ȃ�
	if( GetTextDocumentConst().NVI_IsDirty() == true )
	{
		outUpdateMenu = false;
	}
	else
	{
		outUpdateMenu = true;
	}
	
	//InlineInput�n�C���C�g�����̏����폜�iDeleteTextFromDocument()��Draw����������̂ŁA�����Ŗ������������Ȃ��悤�j
	mHiliteLineStyleIndex.DeleteAll();
	mHiliteStartPos.DeleteAll();
	mHiliteEndPos.DeleteAll();
	
	//�����m��̏ꍇ win 
	if( inFixLen == kIndex_Invalid )
	{
		mInlineInputNotFixedTextLen = 0;
		NVMC_RefreshControl();
		return true;
	}
	
	//Undo�A�N�V�����^�O�L�^�i�O��InlineInput����̑����ł͂Ȃ��ꍇ�̂�Undo�A�N�V�����^�O�L�^�j
	if( mInlineInputNotFixedTextLen == 0 )
	{
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Typing);
	}
	
	//#1298
	//���̓f�[�^�O����
	//�EkInputBackslashByYenKey��true�̏ꍇ��AJIS�n�̏ꍇ�́A���p���iU+00A5�j���o�b�N�X���b�V���iU+005C�j�֕ϊ�����
	//�܂��AfixLen��A�n�C���C�g�ʒu��ϊ��ɉ����Ă��炷�B
	//�iADocument_Text::SPI_InsertText()�ɂĕϊ����s���Ă������AfixLen��A�n�C���C�g�ʒu�����炷�K�v������̂ŁA�����ł��炩���ߏ������Ă������Ƃɂ���B
	AText	insertText;
	insertText.SetText(inText);
	AItemCount	fixLen = inFixLen;
	AArray<AIndex>	hiliteLineStyleIndex, hiliteStartPos, hiliteEndPos;
	hiliteLineStyleIndex.SetFromObject(inHiliteLineStyleIndex);
	hiliteStartPos.SetFromObject(inHiliteStartPos);
	hiliteEndPos.SetFromObject(inHiliteEndPos);
	
	//���͕�����ꕶ�����Ƃ̃��[�v
	for( AIndex pos = 0; pos < insertText.GetItemCount(); pos += insertText.GetNextCharPos(pos) )
	{
		//���p���iU+00A5�j���ǂ����̔���
		if( insertText.GetUTF8ByteCount(pos) == 2 )
		{
			AUChar	ch1 = insertText.Get(pos);
			AUChar	ch2 = insertText.Get(pos+1);
			if( ch1 == 0xC2 && ch2 == 0xA5 )
			{
				//kInputBackslashByYenKey��true�A�܂��́AJIS�n�̏ꍇ�́A���p���iU+00A5�j���o�b�N�X���b�V���iU+005C�j�֕ϊ�����
				if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey) == true || GetTextDocument().SPI_IsJISTextEncoding() == true )
				{
					//���p���iU+00A5�j���o�b�N�X���b�V���iU+005C�j�֕ϊ�
					insertText.Delete(pos,2);
					insertText.Insert1(pos,kUChar_Backslash);
					//fixLen��A�n�C���C�g�ʒu���A�ϊ��ʒu�������ɂ���ꍇ�́A���炷�B
					if( pos < fixLen )
					{
						fixLen--;
					}
					for( AIndex i = 0; i < hiliteStartPos.GetItemCount(); i++ )
					{
						if( pos < hiliteStartPos.Get(i) )
						{
							hiliteStartPos.Set(i,hiliteStartPos.Get(i)-1);
						}
						if( pos < hiliteEndPos.Get(i) )
						{
							hiliteEndPos.Set(i,hiliteEndPos.Get(i)-1);
						}
					}
				}
			}
		}
	}
	
	AIndex	insertionTextIndex;
	AIndex	inlineInputFirstPos;
	if( mInlineInputNotFixedTextLen > 0 )
	{
		//�O��InlineInput����̑����̏ꍇ
		//�O�񖢊m�蕪�͍폜����
		//DeleteTextFromDocument(mInlineInputNotFixedTextPos,mInlineInputNotFixedTextPos+mInlineInputNotFixedTextLen);
		//�O�񖢊m�蕔���̃e�L�X�g���擾����
		AText	text;
		GetTextDocumentConst().SPI_GetText(mInlineInputNotFixedTextPos,mInlineInputNotFixedTextPos+mInlineInputNotFixedTextLen,text);
		//�O�񖢊m��e�L�X�g�ƁA����e�L�X�g���r���A�ŏ����牽������v���Ă��邩���擾����
		AIndex	samecount = 0;
		for( ; samecount < text.GetItemCount(); samecount++ )
		{
			if( samecount >= insertText.GetItemCount() )   break;
			if( text.Get(samecount) != insertText.Get(samecount) )   break;
		}
		samecount = text.GetCurrentCharPos(samecount);
		//�폜�͈͌v�Z
		AIndex	spos = mInlineInputNotFixedTextPos + samecount;
		AIndex	epos = mInlineInputNotFixedTextPos+mInlineInputNotFixedTextLen;
		if( spos < epos )
		{
			GetTextDocument().SPI_DeleteText(spos,epos,false,false);//(insertText.GetItemCount()>0));//080928 �o�O����̂łЂƂ܂����ɖ߂��i�s�܂�Ԃ��L��ŁA�����������́��폜�ŁA�s������ꍇ�Ɏ��̍s�̕\�����X�V����Ȃ��j,(insertText.GetItemCount()>0));//B0000 080922������ �}�������񂪂����redraw���Ȃ�
		}
		//�}���ʒu�͑O�񖢊m�蕔���̂����폜���Ȃ����������̍Ōォ��
		insertionTextIndex = mInlineInputNotFixedTextPos + samecount;
		inlineInputFirstPos = mInlineInputNotFixedTextPos;
		//
		mInlineInputNotFixedTextPos = mInlineInputNotFixedTextPos+fixLen;
		mInlineInputNotFixedTextLen = insertText.GetItemCount() - fixLen;
		//
		insertText.Delete(0,samecount);
	}
	else
	{
		//InlineInput�ŏ�����̏ꍇ
		ATextPoint	insertionPoint;
		if( IsCaretMode() == true )
		{
			//#688
			//�p��2�x�����Ή�
			if( inReplaceLengthInUTF16 > 0 )
			{
				AIndex	spos = 0, epos = 0;
				SPI_GetSelect(spos,epos);
				for( AIndex i = 0; i < inReplaceLengthInUTF16; i++ )
				{
					spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos,false);
				}
				DeleteTextFromDocument(spos,epos,false,false);
			}
			//
			GetCaretTextPoint(insertionPoint);
		}
		else
		{
			ATextPoint	ept;
			SPI_GetSelect(insertionPoint,ept);
			DeleteTextFromDocument(insertionPoint,ept,false,false);
			GetCaretTextPoint(insertionPoint);//B0349 �폜�̌��ʁA�L�����b�g��TextPoint�͕ς�邱�Ƃ�����B�i�s�܂�Ԃ�����̏ꍇ�j
		}
		insertionTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(insertionPoint);
		inlineInputFirstPos = GetTextDocument().SPI_GetTextIndexFromTextPoint(insertionPoint);
		//
		mInlineInputNotFixedTextPos = insertionTextIndex+fixLen;
		mInlineInputNotFixedTextLen = insertText.GetItemCount() - fixLen;
	}
	
	if( mInlineInputNotFixedTextLen > 0 )//B0351 AquaSKK�΍�@�S�m��Ȃ�n�C���C�g�������͑S�폜��Ԃɂ���
	{
		//InlineInput�n�C���C�g�����̏���ۑ�
		mHiliteLineStyleIndex.SetFromObject(hiliteLineStyleIndex);
		mHiliteStartPos.SetFromObject(hiliteStartPos);
		mHiliteEndPos.SetFromObject(hiliteEndPos);
		//mHiliteStartPos, mHiliteEndPos�̓e�L�X�g�S�̂̃C���f�b�N�X�ɂ��Ă���
		for( AIndex i = 0; i < mHiliteLineStyleIndex.GetItemCount(); i++ )
		{
			mHiliteStartPos.Set(i,mHiliteStartPos.Get(i)+inlineInputFirstPos);
			mHiliteEndPos.Set(i,mHiliteEndPos.Get(i)+inlineInputFirstPos);
		}
	}
	
	/*#1298
	//B0332
	if( insertText.GetItemCount() >= 2 )
	{
		AUChar	ch1 = insertText.Get(0);
		AUChar	ch2 = insertText.Get(1);
		if( ch1 == 0xC2 && ch2 == 0xA5 )
		{
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey) == true )
			{
				insertText.Delete(0,2);
				insertText.Insert1(0,kUChar_Backslash);
				//B0380 Fix������0�̏ꍇ�́A�o�C�g�������炵��������NotFixed�̕����̂͂��Ȃ̂ŁAmInlineInputNotFixedTextLen��1byte�����炷
				if( fixLen == 0 )
				{
					mInlineInputNotFixedTextLen--;
				}
			}
		}
	}
	*/
	
	//�e�L�X�g�}���ʒu�Ƀq���g�e�L�X�g������΍폜����
	RemoveHintText(insertionTextIndex);
	//�e�L�X�g�}��
	//InsertTextToDocument(insertionTextIndex,insertText);
	GetTextDocument().SPI_InsertText(insertionTextIndex,insertText);
	
	//�L�����b�g�ݒ�
	ATextPoint	caretPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(insertionTextIndex+insertText.GetItemCount(),caretPoint);
	
	//Hilite���ɃL�����b�g��񂪊܂܂�Ă���ꍇ�͂���ɏ]��
	if( mInlineInputNotFixedTextLen > 0 )//B0351 AquaSKK�΍�@�S�m��Ȃ�L�����b�g�͓��͕�����Ō�̂܂܂ɂ��Ă���
	{
		for( AIndex i = 0; i < hiliteLineStyleIndex.GetItemCount(); i++ )
		{
			if( hiliteLineStyleIndex.Get(i) == kIndex_Invalid )
			{
				GetTextDocument().SPI_GetTextPointFromTextIndex(inlineInputFirstPos+hiliteStartPos.Get(i),caretPoint);
			}
		}
	}
	
	//
	SetCaretTextPoint(caretPoint);
	AdjustScroll(GetCaretTextPoint());
	
	//B0000 �L�[���͂ɑ΂��锽�����x���グ�邽�߂ɁA�����ň�U�A���ۂ̉�ʂւ̉�ʍX�V���s��
	//#688 NVMC_ExecuteDoDrawImmediately();
	//#688 NVMC_RedrawImmediately();
	
	//
	ATextPoint	insertionTextPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(insertionTextIndex,insertionTextPoint);
	ATextPoint	oldNotFixedStartTextPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(mInlineInputNotFixedTextPos-fixLen,oldNotFixedStartTextPoint);
	if( oldNotFixedStartTextPoint.y < insertionTextPoint.y )
	{
		//
		AImageRect	imageLineRect;
		GetLineImageRect(oldNotFixedStartTextPoint.y,insertionTextPoint.y,imageLineRect);
		ALocalRect	localLineRect;
		NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
		//�`��͈͂�view�̉E�[�܂łɂ���
		ALocalRect	localFrameRect = {0};
		NVM_GetLocalViewRect(localFrameRect);
		if( localLineRect.right < localFrameRect.right )
		{
			localLineRect.right = localFrameRect.right;
		}
		//
		NVMC_RefreshRect(localLineRect);
	}
	
	//#390 �L�[�}�N���L�^
	if( fixLen > 0 && GetApp().SPI_IsKeyRecording() == true )
	{
		AText	text;
		GetTextDocumentConst().SPI_GetText(mInlineInputNotFixedTextPos-fixLen,mInlineInputNotFixedTextPos,text);
		GetApp().SPI_RecordKeyText(text);
	}
	
	//win
	//win ACursorWrapper::ObscureCursor();
	NVMC_ObscureCursor();
	
	//win Caret������΍�i�r���ŕ`�悹���ɁA�����ł܂Ƃ߂ĕ\���j
	//Caret�\���\�񂪂���Ε\������
	//#1093 ShowCaretIfReserved();
	
	return true;
}

/**
�C�����C�����́@�e�L�X�g�͈́����[�J�����W
#1305 
*/
ABool	AView_Text::EVTDO_DoInlineInputOffsetToPos( const AIndex inStartOffset, const AIndex inEndOffset, ALocalRect& outRect )
{
	//�e�L�X�g�͈͎擾
	ATextPoint	startTextPoint = {0}, endTextPoint = {0};
	if( mInlineInputNotFixedTextLen > 0 )
	{
		GetTextDocument().SPI_GetTextPointFromTextIndex(mInlineInputNotFixedTextPos+inStartOffset,startTextPoint,true);
		GetTextDocument().SPI_GetTextPointFromTextIndex(mInlineInputNotFixedTextPos+inEndOffset,endTextPoint,true);
	}
	else
	{
		GetCaretTextPoint(startTextPoint);
		GetCaretTextPoint(endTextPoint);
	}
	//�C���[�W���W�擾
	AImagePoint	startImagePoint = {0}, endImagePoint = {0};
	GetImagePointFromTextPoint(startTextPoint,startImagePoint);
	GetImagePointFromTextPoint(endTextPoint,endImagePoint);
	//�����s�ɂ킽��ꍇ�͏o��Rect�̉E�[�́A�ŏ��̍s�̉E�[�ɂ���
	if( endTextPoint.y > startTextPoint.y )
	{
		ATextPoint	startTextPoint2 = startTextPoint;
		startTextPoint2.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(startTextPoint.y);
		AImagePoint	startImagePoint2 = {0};
		GetImagePointFromTextPoint(startTextPoint2,startImagePoint2);
		endImagePoint.x = startImagePoint2.x;
	}
	//���[�J�����W�擾
	AImageRect	imageRect = {0};
	imageRect.left		= startImagePoint.x;
	imageRect.top		= startImagePoint.y;
	imageRect.right		= endImagePoint.x;
	imageRect.bottom	= endImagePoint.y + GetLineHeightWithoutMargin(endTextPoint.y);
	NVM_GetLocalRectFromImageRect(imageRect,outRect);
	return true;
}

ABool	AView_Text::EVTDO_DoInlineInputPosToOffset( const ALocalPoint& inPos, AIndex& outOffset )
{
	AImagePoint	imagePoint;
	NVM_GetImagePointFromLocalPoint(inPos,imagePoint);
	ATextPoint	textPoint;
	GetTextPointFromImagePoint(imagePoint,textPoint);
	ATextIndex	textIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(textPoint);
	if( mInlineInputNotFixedTextLen > 0 )
	{
		outOffset = textIndex - mInlineInputNotFixedTextPos;
	}
	else
	{
		outOffset = 0;
	}
	return true;
}

ABool	AView_Text::EVTDO_DoInlineInputGetSelectedText( AText& outText )
{
	GetSelectedText(outText);
	outText.LimitLength(0,kLimit_InlineInputGetSelectedText_MaxByteCount);//#695 10000 byte�����x�ɂ���
	return true;
}

void	AView_Text::SPI_GetSelectedText( AText& outText ) const
{
	GetSelectedText(outText);
}

void	AView_Text::GetSelectedText( AText& outText ) const
{
	outText.DeleteAll();
	if( IsCaretMode() == false )
	{
		if( mKukeiSelected == false )
		{
			ATextPoint	caretPoint, selectPoint;
			GetCaretTextPoint(caretPoint);
			GetSelectTextPoint(selectPoint);
			GetTextDocumentConst().SPI_GetText(caretPoint,selectPoint,outText);
		}
		else
		{
			for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
			{
				ATextPoint	spt, ept;
				spt = mKukeiSelected_Start.Get(i);
				ept = mKukeiSelected_End.Get(i);
				AText	text;
				GetTextDocumentConst().SPI_GetText(spt,ept,text);
				outText.AddText(text);
				outText.Add(kUChar_LineEnd);
			}
		}
	}
	//B0330 TEST
	/*else
	{
		ATextIndex	spos, epos;
		GetSelect(spos,epos);
		AIndex pos = spos-1;
		for( ; pos > 0; pos-- )
		{
			AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(pos);
			if( (ch>='A'&&ch<='Z') || (ch>='a'&&ch<='z') || ch == '-' )
			{
				continue;
			}
			else
			{
				pos++;
				break;
			}
		}
		if( pos < spos )
		{
			GetTextDocumentConst().SPI_GetText(pos,spos,outText);
		}
	}*/
}

ABool	AView_Text::EVTDO_DoKeyDown( /*#688 const AUChar inChar*/ const AText& inText, const AModifierKeys inModifierKeys )//#1080
{
	/*#688 
	AText	text;
	text.Add(inChar);
	*/
	if( KeyTool(inText) == true )   return true;
	//#1416
	//Control+Q�́Aemacs�ł����Ƃ����quoted-insert����Ɋ��蓖�Ă��Ă���Ǝv����B
	//�iSystem/Library/Frameworks/AppKit.framework/Resources/StandardKeyBinding.dict�ɂ͋L�q���Ȃ����AmacOS�̃f�t�H���g����Ƃ��đg�ݍ��܂�Ă�����̂Ǝv����B�j
	//quoted-insert�͎��ɓ��͂������������̂܂܁iControl+A�Ȃ�0x01�̂܂܁j���͂���Ƃ������삾���Ami�ł́A����R�[�h�̓��͂�AView_Text::TextInput()�ŗ}�����Ă���(�֘A�F#1080)�̂ŁAquoted-insert�͓��삵�Ȃ��B
	//���̂��߁A#1416�̑΍�Ƃ��āAControl+Q�̂݁A�����ŕ߂܂��ď������ACocoaUserPaneView.mm/keydown�ŁAinterpretKeyEvents�𓮍삳���Ȃ��悤�ɂ���B
	//���̃L�[�������ŕ߂܂���ƁAOS�̃L�[�o�C���h���e���擾�ł��Ȃ��̂ŁA�uOS�̃L�[�o�C���h����̍�����ݒ肷��v�ݒ��ON�ɂ����ꍇ�̓��삪�ł��Ȃ��B���̂��߁A���̃L�[�͏]���ʂ�AinterpretKeyEvents�����s���A���ꂼ��deleteBackward���o�R��EVTDO_DoTextInput()���R�[������悤�ɂ���B
	//Control+���p�����A�X���b�V���ABS�L�[�������ŏ������邱�Ƃɂ���(#1252)
	if( inText.GetItemCount() > 0 )
	{
		//���͕���(UCS4)�擾
		AUCS4Char ch = 0;
		inText.Convert1CharToUCS4(0, ch);
		if( inModifierKeys == kModifierKeysMask_Control )
		{
			//�L�[�擾
			AKeyBindKey	key = kKeyBindKey_Invalid;
			switch(ch)
			{
			  case 0x11://Control+Q
				{
					key = kKeyBindKey_Q;
					break;
				}
			  case '0':
				{
					key = kKeyBindKey_0;
					break;
				}
			  case '1':
				{
					key = kKeyBindKey_1;
					break;
				}
			  case '2':
				{
					key = kKeyBindKey_2;
					break;
				}
			  case '3':
				{
					key = kKeyBindKey_3;
					break;
				}
			  case '4':
				{
					key = kKeyBindKey_4;
					break;
				}
			  case '5':
				{
					key = kKeyBindKey_5;
					break;
				}
			  case '6':
				{
					key = kKeyBindKey_6;
					break;
				}
			  case '7':
				{
					key = kKeyBindKey_7;
					break;
				}
			  case '8':
				{
					key = kKeyBindKey_8;
					break;
				}
			  case '9':
				{
					key = kKeyBindKey_9;
					break;
				}
			  case '/':
				{
					key = kKeyBindKey_Slash;
					break;
				}
			  case 0x7F:
				{
					key = kKeyBindKey_BS;
					break;
				}
			}
			if( key != kKeyBindKey_Invalid )
			{
				//�L�[���͏����i�L�[�o�C���h�j
				ABool	updateMenu = false;
				ABool	result = EVTDO_DoTextInput(inText,key,kModifierKeysMask_Control,keyAction_NOP,updateMenu);
				if( updateMenu == true )
				{
					AApplication::GetApplication().NVI_UpdateMenu();
				}
				return result;
			}
		}
	}
	//
	return false;
}

//
ABool	AView_Text::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	//#688
	//���j���[�X�V�t���O������
	outUpdateMenu = true;
	
	//#867
	//Idle work�^�C�}�[�N���A
	ResetKeyIdleWorkTimer(kKeyIdleWorkTimerValue);
	
	//#688
	//tick timer�����O�ɕ������͂��ꂽ�ꍇ�ɁA������reserve���ꂽcaret�`������s����B
	//�i�����ŕ`�悵�Ȃ��ƃI�[�g���s�[�g�����Ƃ����A�L�����b�g�`�悪�x��Č�����j
	ShowCaretIfReserved();
	
	//
	ABool	result = true;//win �����������ǂ�����Ԃ�
	
	RestoreFromBraceCheck();
	if( KeyTool(inText) == true )   return true;//win Win��EVTDO_DoKeyDown()��ʂ�Ȃ��̂ŁB
	AKeyBindKey	keyBindKey = inKeyBindKey;//#688 AKeyWrapper::GetKeyBindKey(inOSKeyEvent);
	AModifierKeys	modifiers = inModifierKeys;//#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent);
	
	//�L�[�o�C���h���m��̏ꍇ�A���A1�����݂̂ŁA0x01-0x1F�̂Ƃ��́A���ꂼ��ɉ������L�[�o�C���h�L�[��ݒ肷��
	//control+shift+[���������őΉ����邽�߁B #1017
	//�ΏǗÖ@�I�ł͂��邪�A�ǂ����������Ȃ���΁A����R�[�h���͂ɂȂ��Ă��܂������Ȃ̂ŁA�Ή����Ȃ����͂����ق���
	//���̂���L�[�o�C���h������B
	if( inText.GetItemCount() == 1 && inKeyBindAction == keyAction_NOP && keyBindKey == kKeyBindKey_Invalid )
	{
		AUChar	ch = inText.Get(0);
		if( ch >= 0x01 && ch < 0x20 && ch != 0x09 && ch != 0x0D && inModifierKeys != 0 )
		{
			//0x01:A
			//0x1A:Z
			//0x1B:@
			//0x1C:]
			//0x1D:[
			//0x1E:�s��
			//0x1F:-
			switch(ch)
			{
			  default:
				{
					keyBindKey = (AKeyBindKey)(kKeyBindKey_A+ch-0x01);
					break;
				}
			  case 0x1B:
				{
					keyBindKey = kKeyBindKey_Atmark;
					break;
				}
			  case 0x1C:
				{
					keyBindKey = kKeyBindKey_BracketEnd;
					break;
				}
			  case 0x1D:
				{
					keyBindKey = kKeyBindKey_BracketStart;
					break;
				}
			  case 0x1E:
				{
					//�����Ȃ�
					break;
				}
			  case 0x1F:
				{
					keyBindKey = kKeyBindKey_Minus;
					break;
				}
			}
		}
	}
	
	//#204 �⊮���̓E�C���h�E�\�����̃L�[����
	//#717 if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCandidateWindowKeyBind) == true )
	{
		//#717 if( GetApp().SPI_GetCandidateListWindow().NVI_IsWindowVisible() == true )
		if( mAbbrevInputMode == true )//#717
		{
			switch(keyBindKey)
			{
			  case kKeyBindKey_Escape:
				{
					//���݂�Undo�A�N�V�������⊮���́A���A���̃A�N�V�����^�O�̌�Ƀe�L�X�g�}���폜�����݂����ꍇ�AUndo�����s
					//�i�A�N�V�����^�O�̌�Ƀe�L�X�g�}���폜�����݂��Ȃ��ꍇ��Undo���s����ƁA���̑O�̃A�N�V������Undo���Ă��܂����߁Adangling���f�K�v�j
					if( GetTextDocument().SPI_GetCurrentUndoActionTag() == undoTag_AbbrevInput &&
								GetTextDocumentConst().SPI_IsCurrentUndoActionDangling() == false )//#314
					{
						UndoRedo(true);
					}
					//�⊮���͏�ԉ��� #717
					ClearAbbrevInputMode(true,false);
					//win ACursorWrapper::ObscureCursor();
					NVMC_ObscureCursor();
					return true;
				}
			  case kKeyBindKey_Up:
				{
					InputAbbrev(false);
					//win ACursorWrapper::ObscureCursor();
					NVMC_ObscureCursor();
					return true;
				}
			  case kKeyBindKey_Down:
				{
					InputAbbrev(true);
					//win ACursorWrapper::ObscureCursor();
					NVMC_ObscureCursor();
					return true;
				}
			  case kKeyBindKey_Return:
				{
					//�⊮���͏�ԉ��� #717
					ClearAbbrevInputMode(true,true);
					//win ACursorWrapper::ObscureCursor();
					NVMC_ObscureCursor();
					return true;
				}
			  default:
				{
					//�����Ȃ�
					break;
				}
			}
		}
	}
	//�⊮�|�b�v�A�b�v��ɏ�ɏ㉺�L�[�ŕ⊮�I������ꍇ�̏���
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAbbrevInputByArrowKeyAfterPopup) == true )
	{
		if( mTextWindowID != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsPopupCandidateWindowVisible() == true )
			{
				switch(keyBindKey)
				{
				  case kKeyBindKey_Up:
					{
						InputAbbrev(false);
						NVMC_ObscureCursor();
						return true;
					}
				  case kKeyBindKey_Down:
					{
						InputAbbrev(true);
						NVMC_ObscureCursor();
						return true;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
			}
		}
	}
	
	/*#1160
	//==================���L�[�ݒ�ɂ�������==================
	//���L�[�ݒ�ɂ������́i���̌��j
	{
		//���L�[�ݒ�ɑΉ�����L�[��������Ă��邩�ǂ������`�F�b�N
		ABool	abbrevKey = false;
		switch( GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kAbbrevNextKey) )
		{
		  case kAbbrevKeyType_ShiftSpace:
			{
				if( keyBindKey == kKeyBindKey_Space && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_ShiftTab:
			{
				if( keyBindKey == kKeyBindKey_Tab && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_Tab:
			{
				if( keyBindKey == kKeyBindKey_Tab && modifiers == kModifierKeysMask_None )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_Escape:
			{
				if( keyBindKey == kKeyBindKey_Escape && modifiers == kModifierKeysMask_None )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_ShiftEscape:
			{
				if( keyBindKey == kKeyBindKey_Escape && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		//���L�[��������Ă�����A��⏈��
		if( abbrevKey == true )
		{
			//��₪�������ꍇ�A���^�[������
			if( InputAbbrev(true) == true )
			{
				NVMC_ObscureCursor();
				return true;
			}
		}
	}
	
	//���L�[�ݒ�ɂ������́i�O�̌��j
	{
		//���L�[�ݒ�ɑΉ�����L�[��������Ă��邩�ǂ������`�F�b�N
		ABool	abbrevKey = false;
		switch( GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kAbbrevPrevKey) )
		{
		  case kAbbrevKeyType_ShiftSpace:
			{
				if( keyBindKey == kKeyBindKey_Space && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_ShiftTab:
			{
				if( keyBindKey == kKeyBindKey_Tab && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_Tab:
			{
				if( keyBindKey == kKeyBindKey_Tab && modifiers == kModifierKeysMask_None )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_Escape:
			{
				if( keyBindKey == kKeyBindKey_Escape && modifiers == kModifierKeysMask_None )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_ShiftEscape:
			{
				if( keyBindKey == kKeyBindKey_Escape && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		//���L�[��������Ă�����A��⏈��
		if( abbrevKey == true )
		{
			//��₪�������ꍇ�A���^�[������
			if( InputAbbrev(false) == true )
			{
				NVMC_ObscureCursor();
				return true;
			}
		}
	}
	*/
	
	AKeyBindAction	action;
	AText	actiontext;
	if( keyBindKey != kKeyBindKey_Invalid )
	{
		if( GetApp().GetAppPref().GetData_DefaultKeyBindAction(keyBindKey,modifiers,action,actiontext) == false )
		{
			keyBindKey = kKeyBindKey_Invalid;
		}
		//RC2
		if( action != keyAction_abbrevprev && action != keyAction_abbrevnext )
		{
			//�⊮���͏�ԉ��� #717
			ClearAbbrevInputMode(false,false);//RC2
		}
	}
	if( keyBindKey == kKeyBindKey_Invalid )
	{
		if( inText.GetItemCount() == 0 )//win
		{
			result = false;
		}
		else
		{
			//�⊮���͏�ԉ��� #717
			ClearAbbrevInputMode(false,false);//RC2
			if( AKeyWrapper::IsCommandKeyPressed(modifiers) != true )//�Y�����j���[�R�}���h���g�p�s�\�ȏꍇ�̃R�}���h�V���[�g�J�b�g�L�[�̏ꍇ�A�������͂Ƃ��đ����Ă���
			{
				AText	yen;
				yen.SetLocalizedText("Text_Yen");
				if( yen.Compare(inText) == true )
				{
					//
					AText	backslash;
					backslash.Add(kUChar_Backslash);
					if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey) == true )
					{
						TextInput(undoTag_Typing,backslash);
					}
					else
					{
						TextInput(undoTag_Typing,inText);
					}
				}
				else
				{
                    //==================�ʏ�e�L�X�g���́i�L�[�o�C���h�s�L�[�j==================
					if( GetTextDocument().NVI_IsDirty() == true && mCaretMode == true )   outUpdateMenu = false;
					TextInput(undoTag_Typing,inText);
					
					//#390 �L�[�}�N���L�^
					GetApp().SPI_RecordKeyText(inText);
				}
			}
			result = true;
		}
	}
	else
	{
        //==================�L�[�o�C���h�������s==================
		result = KeyBindAction(keyBindKey,modifiers,inKeyBindAction,inText,outUpdateMenu);//win
	}
	
	//win ACursorWrapper::ObscureCursor();
	NVMC_ObscureCursor();
	
	//win Caret������΍�i�r���ŕ`�悹���ɁA�����ł܂Ƃ߂ĕ\���j
	//Caret�\���\�񂪂���Ε\������
	//#688 ShowCaretIfReserved();
	
	return result;
}

void	AView_Text::DeleteSelect( const AUndoTag inUndoActionTag )
{
	if( IsCaretMode() == true )   return;
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(inUndoActionTag);
	//�I��͈͍폜���s
	DeleteTextFromDocument();
}

void	AView_Text::TextInput( const AUndoTag inUndoActionTag, const AText& inText )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	//0x20�����̕����͓��͂��Ȃ� #1080
	if( inText.GetItemCount() == 1 )
	{
		AUChar	ch = inText.Get(0);
		if( ch < 0x20 && ch != 0x09 && ch != 0x0D )
		{
			return;
		}
	}
	
	//Undo�A�N�V�����^�O�L�^
	if( inUndoActionTag != undoTag_NOP )//B0381
	{
		GetTextDocument().SPI_RecordUndoActionTag(inUndoActionTag);
	}
	
	//#478
	//�e�[�u���ҏW���[�h
	ABool	tableMode = false;
	AArray<AIndex>	surrondingTableRowArray_LineIndex;
	AArray<ABool>	surrondingTableRowArray_IsRuledLine;
	AArray<AIndex>	surrondingTableRowArray_ColumnEndX;
	if( false )
	{
		//�͂�ł���e�[�u���̏����擾
		tableMode = GetTextDocumentConst().SPI_GetSurroundingTable(mCaretTextPoint,
					surrondingTableRowArray_LineIndex,surrondingTableRowArray_IsRuledLine,surrondingTableRowArray_ColumnEndX);
	}
	
	//�I��\�����#699
	CancelReservedSelection();
	
	//�I��͈͍폜�A�e�L�X�g�}��
	DeleteAndInsertTextToDocument(inText);
	
	//#478
	//�e�[�u���ҏW���[�h
	if( tableMode == true )
	{
		//�e�[�u���Ɉ͂܂�Ă���ꍇ
		
		//�e�[�u���e�s���̏���
		for( AIndex index = 0; index < surrondingTableRowArray_LineIndex.GetItemCount(); index++ )
		{
			//�sindex�擾
			AIndex	lineIndex = surrondingTableRowArray_LineIndex.Get(index);
			//�L�����b�g�s�͉������Ȃ�
			if( lineIndex == mCaretTextPoint.y )
			{
				continue;
			}
			//�r���s�̏���
			if( surrondingTableRowArray_IsRuledLine.Get(index) == true )
			{
				//-��}��
				AIndex	x = surrondingTableRowArray_ColumnEndX.Get(index);
				ATextPoint	tpt = {x,lineIndex};
				AText	t("-");
				GetTextDocument().SPI_InsertText(tpt,t);
			}
			//�r���łȂ��s�̏���
			else
			{
				//�X�y�[�X��}��
				AIndex	x = surrondingTableRowArray_ColumnEndX.Get(index);
				ATextPoint	tpt = {x,lineIndex};
				AText	t(" ");
				GetTextDocument().SPI_InsertText(tpt,t);
			}
		}
	}
	
	//#717
	//���A���^�C���⊮���v��
	if( inUndoActionTag == undoTag_Typing )
	{
		//FindCandidateRealtime();
		mCandidateRequestTimer = 9;
	}
	
	//�C���f���g���s�����ɂ�鎩���C���f���g���s
	//�i�C���f���g�^�C�v�����@��`�C���f���g�A���K�\���C���f���g�̏ꍇ�̂݁j
	if( inText.GetItemCount() == 1 )
	{
		if( (GetTextDocumentConst().SPI_GetIndentType() == kIndentType_SDFIndent || 
				GetTextDocumentConst().SPI_GetIndentType() == kIndentType_RegExpIndent ) && 
				GetApp().SPI_GetAutoIndentMode() == true )//#638
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			AText	text;
			GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Text(AModePrefDB::kIndentExeLetters,text);
			if( text.IsCharContained(inText.Get(0)) == true )
			{
				DoIndent(undoTag_NOP,inputSpaces,kIndentTriggerType_AutoIndentCharacter);//#249 #639 #650 ���K�\����v�̏ꍇ�̂݃C���f���g�K�p
			}
		}
	}
	
	//
	if( inText.GetItemCount() == 1 && 
				GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kCheckBrace) == true )
	{
		ATextPoint	pt;
		ABool	checkOK;
		if( GetTextDocument().SPI_CheckBrace(GetCaretTextPoint(),pt,checkOK) == true )
		{
			if( checkOK == true )
			{
				NVI_GetOriginOfLocalFrame(mBraceCheckSavedOrigin);
				mBraceCheckSavedCaretTextPoint = GetCaretTextPoint();
				ATextPoint	ept = pt;
				ept.x++;
				SetSelect(pt,ept,(GetApp().NVI_IsDarkMode()==true));//#1316 �_�[�N���[�h�̏ꍇ�͔��]�n�C���C�g���g��
				//#396 AdjustScroll_Center(pt);
				AdjustScroll(pt);//#396
				mBraceCheckStartTickCount = kBraceCheckTimerCount;//win 080713 20;
				mDisplayingBraceCheck = true;
			}
			else
			{
				ASoundWrapper::Beep();
			}
		}
	}
	//RC2
	if( inText.GetItemCount() == 1 )
	{
		//#853 if( GetApp().SPI_GetIdInfoWindow().NVI_IsWindowVisible() )
		{
			AUChar	ch = inText.Get(0);
			//�L�[���[�h�\�������̌�Ȃ�A�L�[���[�h���\��
			if( GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetSyntaxDefinition().IsIdInfoAutoDisplayChar(ch) == true )
			{
				IdInfoAuto();
			}
			//�����f���~�^�̌�Ȃ�A�����n�C���C�g
			if( GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetSyntaxDefinition().IsIdInfoDelimiterChar(ch) == true )
			{
				IdInfoAutoArg();
			}
		}
	}
	//R0214
	/*�w�b�_���e�������ɕς���Ă��܂��̂ŕԂ��Ďg���Â炢
	ATextIndex	pos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
	if( pos > 0 )
	{
		if( inText.GetItemCount() == 1 && GetTextDocumentConst().SPI_IsWordAlphabet(pos-1) )
		{
			pos--;
			AText	word;
			ATextIndex	spos = pos, epos = pos;
			GetTextDocument().SPI_FindWord(pos,spos,epos);
			GetTextDocument().SPI_GetText(spos,epos,word);
			SetInfoHeader(word);
		}
	}*/
}

void	AView_Text::KukeiInput( const AText& inText )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	//�I��͈͍폜
	DeleteTextFromDocument();
	//
	mKukeiSelected_Start.DeleteAll();
	mKukeiSelected_End.DeleteAll();
	//
	AImagePoint	imagePoint;
	GetImagePointFromTextPoint(GetCaretTextPoint(),imagePoint);
	HideCaret(true,true);
	mKukeiSelected = true;
	mCaretMode = false;
	mSelectTextPoint = GetCaretTextPoint();//�O�̂���
	AIndex	lineIndex = GetCaretTextPoint().y;
	for( AIndex pos = 0; pos < inText.GetItemCount();  )
	{
		AText	lineText;
		inText.GetLine(pos,lineText);
		AImagePoint	ipt;
		ipt.x = imagePoint.x;
		ipt.y = GetImageYByLineIndex(lineIndex);
		ATextPoint	textpoint;
		GetTextPointFromImagePoint(ipt,textpoint);
		GetTextDocument().SPI_InsertText(textpoint,lineText);
		//
		mKukeiSelected_Start.Add(textpoint);
		ATextPoint	end = textpoint;
		end.x += lineText.GetItemCount();
		mKukeiSelected_End.Add(end);
		//
		lineIndex++;
		if( lineIndex >= GetTextDocumentConst().SPI_GetLineCount() )
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			GetTextDocument().SPI_InsertText(GetTextDocumentConst().SPI_GetTextLength(),text);
		}
	}
	NVI_Refresh();
}

//�C���f���g���s
void	AView_Text::DoIndent( /*#650 const ABool inUseCurrentParagraphIndentForRegExp, */const AUndoTag inUndoTag, 
		const ABool inInputSpaces, const AIndentTriggerType inIndentTriggerType )//B0363 B0381 #249 #639 #650
{
	ABool	svCaretMode = mCaretMode;
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	ATextPoint	pt = spt;
	pt.x = 0;
	SetCaretTextPoint(pt);
	DoIndent(/*#650 inUseCurrentParagraphIndentForRegExp,*/inUndoTag,spt,ept,inInputSpaces,inIndentTriggerType);//#249 #639 #650
	if( spt.y >= GetTextDocumentConst().SPI_GetLineCount() )   spt.y = GetTextDocumentConst().SPI_GetLineCount()-1;
	if( ept.y >= GetTextDocumentConst().SPI_GetLineCount() )   ept.y = GetTextDocumentConst().SPI_GetLineCount()-1;
	//���X���L�����b�g��Ԃ̏ꍇ�A�C���f���g��ɑI����ԂɂȂ�Ȃ��悤�ɁA�␳����
	//�iDoIndent()�̑I���I���ʒu�␳�������s���S�Ȃ��߂����A����͂ЂƂ܂����̑΍�ɂ���j
	if( svCaretMode == true )
	{
		//spt, ept�̂����A�O�̕����L�����b�g�ʒu�ɐݒ肷��
		if( GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt) < GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ept) )
		{
			ept = spt;
		}
		else
		{
			spt = ept;
		}
	}
	//
	SetSelect(spt,ept);
}
//B0381
void	AView_Text::DoIndent( /*#650 const ABool inUseCurrentParagraphIndentForRegExp, */const AUndoTag inUndoTag, 
		ATextPoint& ioStartTextPoint, ATextPoint& ioEndTextPoint, const ABool inInputSpaces,
		const AIndentTriggerType inIndentTriggerType )//#249 #639 #650
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	//Undo�A�N�V�����^�O�L�^
	if( inUndoTag != undoTag_NOP )//B0381
	{
		GetTextDocument().SPI_RecordUndoActionTag(inUndoTag);
	}
	
	//#639
	ABool	indent0ForEmptyLine = false;
	if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kIndent0ForEmptyLine) == true )
	{
		switch(inIndentTriggerType)
		{
		  case kIndentTriggerType_IndentFeature:
		  case kIndentTriggerType_ReturnKey_CurrentLine:
			{
				indent0ForEmptyLine = true;
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
	
	/*R0208 GetCurrentParagraphStartLineIndex()���d��
	AIndex	startParagraphIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ioStartTextPoint.y);
	AIndex	startParagraphStartLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(startParagraphIndex);
	AIndex	endParagraphIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ioEndTextPoint.y)+1;
	AIndex	endParagraphStartLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(endParagraphIndex);*/
	AIndex	startParagraphStartLineIndex = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(ioStartTextPoint.y);
	//B0000 �s�S�̂�I�����ăC���f���g���A�C���f���g���������ꍇ�A���̍s�܂ŃC���f���g�ΏۂƂȂ�����C��
	AIndex	endlineindex = ioEndTextPoint.y;
	if( ioEndTextPoint.x == 0 && ioEndTextPoint.y > 0 && ioEndTextPoint.y > ioStartTextPoint.y )   endlineindex--;
	AIndex	endParagraphStartLineIndex = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(endlineindex);
	
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_Indent,true,false,true);
	try
	{
		//#649 spt�Œ�����i�I���ł���Ax==0����n�܂�j
		ABool	fixStartPoint = ((ComparePoint(ioStartTextPoint,ioEndTextPoint) == false) && (ioStartTextPoint.x == 0));
		//
		AIndex	prevIndentCount = kIndex_Invalid;
		for( AIndex lineIndex = startParagraphStartLineIndex; lineIndex < endParagraphStartLineIndex; lineIndex++ )
		{
			//���[�_���Z�b�V�����p������
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Indent,0,true,
																	  lineIndex-startParagraphStartLineIndex,
																	  endParagraphStartLineIndex-startParagraphStartLineIndex) == false )
			{
				break;
			}
			if( lineIndex >= GetTextDocumentConst().SPI_GetLineCount() )   break;//��
			//�i���J�n�s�ȊO�ł̓C���f���g�������s��Ȃ�
			if( GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(lineIndex) != lineIndex )   continue;
			//
			AItemCount	nextIndentCount;
			AItemCount	indentCount = GetTextDocument().SPI_CalcIndentCount(lineIndex,prevIndentCount,nextIndentCount,
																			//#650 inUseCurrentParagraphIndentForRegExp);//B0363
																			inIndentTriggerType);//#650
			prevIndentCount = nextIndentCount;
			if( prevIndentCount < 0 )   prevIndentCount = 0;
			//#639 ��s�Ȃ�C���f���g0�ɂ���
			if( indent0ForEmptyLine == true )
			{
				//���݂̃p���O���t�̃e�L�X�g�擾
				AText	paraText;
				GetTextDocumentConst().SPI_GetParagraphTextByLineIndex(lineIndex,paraText);
				//�󔒂݂̂��ǂ����𔻒�
				ABool	onlySpace = true;
				AItemCount	len = paraText.GetItemCount();
				for( AIndex p = 0; p < len; p++ )
				{
					AUChar	ch = paraText.Get(p);
					if( ch != kUChar_Space && ch != kUChar_Tab && ch != kUChar_LineEnd )
					{
						onlySpace = false;
						break;
					}
				}
				//�󔒂݂̂Ȃ�C���f���g��0�ɐݒ�
				if( onlySpace == true )
				{
					indentCount = 0;
				}
			}
			//�I��͈͂��炵�p�f�[�^�L���i�C���f���g���s�O�f�[�^�L���j #996
			AIndex	currentParaIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(lineIndex);
			AIndex	startParaIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ioStartTextPoint.y);
			AIndex	startParaStartTextIndex = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(startParaIndex);
			AIndex	startOffset = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ioStartTextPoint) - startParaStartTextIndex;
			AIndex	endParaIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ioEndTextPoint.y);
			AIndex	endParaStartTextIndex = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(endParaIndex);
			AIndex	endOffset = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ioEndTextPoint) - endParaStartTextIndex;
			//
			ATextIndex	svEndTextIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ioEndTextPoint);
			//AItemCount	svLineCount = GetTextDocumentConst().SPI_GetLineCount();
			//�C���f���g���s
			AItemCount	insertedCount = GetTextDocument().SPI_Indent(lineIndex,indentCount,inInputSpaces,true);//#249
			//AItemCount	insertedLineCount = GetTextDocumentConst().SPI_GetLineCount() - svLineCount;
			//fprintf(stderr,"%d ",lineIndex);
			/*#649 ���̔��肾��x==0���畡���s�I�����Aept�̈ʒu����������Ȃ���肪����B
		if( ioStartTextPoint.x == 0 && ioEndTextPoint.y > ioStartTextPoint.y )//B0416 �����s��I�𒆂Ɂu�E�V�t�g�v���s�����ꍇ�A�I��͈͒��ŏ��̍s�̍s���^�u�������A�I��͈͂���O��Ă��܂�
		{
			//�����Ȃ�
		}
		else
		{
		*/
			//���ݏ������̒i�����A�I��͈͂̊J�n�i���ł���΁A�I��͈͂̊J�n�����炷�@#996 �s�P�ʂ̏�������i���P�ʂ̏����֕ύX
			//�I���J�n�ʒu�����炷
			if( startParaIndex == currentParaIndex && fixStartPoint == false )//#649
			{
				startOffset += insertedCount;
				if( startOffset < 0 )
				{
					startOffset = 0;
				}
				if( startOffset > GetTextDocument().SPI_GetParagraphLengthWithoutLineEnd(startParaIndex) )
				{
					startOffset = GetTextDocument().SPI_GetParagraphLengthWithoutLineEnd(startParaIndex);
				}
				GetTextDocument().SPI_GetTextPointFromTextIndex(startParaStartTextIndex+startOffset,ioStartTextPoint);
			}
			//�I���I���ʒu�����炷
			if( endParaIndex == currentParaIndex )
			{
				endOffset += insertedCount;
				if( endOffset < 0 )
				{
					endOffset = 0;
				}
				if( endOffset > GetTextDocument().SPI_GetParagraphLengthWithoutLineEnd(endParaIndex) )
				{
					endOffset = GetTextDocument().SPI_GetParagraphLengthWithoutLineEnd(endParaIndex);
				}
				GetTextDocument().SPI_GetTextPointFromTextIndex(endParaStartTextIndex+endOffset,ioEndTextPoint);
			}
			//#649}
			//�I���I���ʒu������̃C���f���g�s����̏ꍇ�A�I���I���ʒu��␳����
			if( endParaIndex > currentParaIndex )
			{
				GetTextDocumentConst().SPI_GetTextPointFromTextIndex(svEndTextIndex + insertedCount,ioEndTextPoint);
				//
				AIndex	endlineindex = ioEndTextPoint.y;
				if( ioEndTextPoint.x == 0 && ioEndTextPoint.y > 0 && ioEndTextPoint.y > ioStartTextPoint.y )   endlineindex--;
				endParagraphStartLineIndex = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(endlineindex);
			}
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	//�e�L�X�g���E�C���h�E�����X�V�i�������̂��߁A��̏�����SPI_Indent()�̈����ɃT�u�E�C���h�E�`�悵�Ȃ��悤�ɐݒ肵�Ă���̂ŁA�����ł܂Ƃ߂ĕ`��j
	GetTextDocument().SPI_DeferredRefreshSubWindowsAfterInsertDeleteText();
}

//�V�t�g
void	AView_Text::ShiftRightLeft( const ABool inRight, const ABool inInputSpaces, const ABool inIsTabKey )//#249 #1188
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Shift);
	
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	//#649 spt�Œ�����i�I���ł���Ax==0����n�܂�j
	ABool	fixStartPoint = ((ComparePoint(spt,ept) == false) && (spt.x == 0));
	//
	ATextPoint	pt = spt;
	pt.x = 0;
	SetCaretTextPoint(pt);
	AIndex	startParagraphIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y);
	AIndex	startParagraphStartLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(startParagraphIndex);
	AIndex	endParagraphIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ept.y);//B0334+1;
	if( (spt.x == ept.x && spt.y == ept.y) || ept.x > 0 )
	{
		endParagraphIndex++;
	}
	AIndex	endParagraphStartLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(endParagraphIndex);
	for( AIndex lineIndex = startParagraphStartLineIndex; lineIndex < endParagraphStartLineIndex; lineIndex++ )
	{
		if( lineIndex >= GetTextDocumentConst().SPI_GetLineCount() )   break;//��
		if( GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(lineIndex) != lineIndex )   continue;
		AItemCount	indentCount = GetTextDocumentConst().SPI_GetCurrentIndentCount(lineIndex);
		if( inRight == true )
		{
			//#1188 �u��s�̏ꍇ�C���f���g�ʂ��O�ɂ���v��ON�ɂ��Ă���ꍇ�A��s�Ȃ�E�V�t�g���Ȃ�
			if( GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(lineIndex) == 0 &&
				GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kIndent0ForEmptyLine) == true &&
				inIsTabKey == false )
			{
				continue;
			}
			//#1407 indentCount += GetTextDocumentConst().SPI_GetIndentWidth();
			//�C���f���g���{�P #1407 �C���f���g�ʒu�ɋz������悤�ɂ���
			indentCount /= GetTextDocumentConst().SPI_GetIndentWidth();
			indentCount += 1;
			indentCount *= GetTextDocumentConst().SPI_GetIndentWidth();
		}
		else
		{
			//#1407 indentCount -= GetTextDocumentConst().SPI_GetIndentWidth();
			//�C���f���g���|�P #1407 �C���f���g�ʒu�ɋz������悤�ɂ���
            indentCount /= GetTextDocumentConst().SPI_GetIndentWidth();
            indentCount -= 1;
            indentCount *= GetTextDocumentConst().SPI_GetIndentWidth();
		}
		AItemCount	insertedCount = GetTextDocument().SPI_Indent(lineIndex,indentCount,inInputSpaces,true);//#249
		/*#649
		if( spt.x == 0 && ept.y > spt.y )//B0416
		{
			//�����Ȃ�
		}
		else
		{
		*/
		if( spt.y == lineIndex && fixStartPoint == false )//#649
		{
			spt.x += insertedCount;
			if( spt.x < 0 )   spt.x = 0;
			if( spt.x > GetTextDocument().SPI_GetLineLengthWithoutLineEnd(spt.y) )   spt.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(spt.y);
		}
		//#649 }//#581
		if( ept.y == lineIndex )
		{
			ept.x += insertedCount;
			if( ept.x < 0 )   ept.x = 0;
			if( ept.x > GetTextDocument().SPI_GetLineLengthWithoutLineEnd(ept.y) )   ept.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(ept.y);
		}
		//#581}
	}
	//�e�L�X�g���E�C���h�E�����X�V�i�������̂��߁A��̏�����SPI_Indent()�̈����ɃT�u�E�C���h�E�`�悵�Ȃ��悤�ɐݒ肵�Ă���̂ŁA�����ł܂Ƃ߂ĕ`��j
	GetTextDocument().SPI_DeferredRefreshSubWindowsAfterInsertDeleteText();
	//
	SetSelect(spt,ept);
}

void	AView_Text::DeleteNextChar()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( IsCaretMode() == false )
	{
		DeleteSelect(undoTag_DELKey);
	}
	else
	{
		AIndex	start = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
		AIndex	end = GetTextDocumentConst().SPI_GetNextCharTextIndex(start);
		if( end > GetTextDocumentConst().SPI_GetTextLength() )   return;
		//Undo�A�N�V�����^�O�L�^
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_DELKey);
		//
		DeleteTextFromDocument(start,end,false,false);
	}
	
	//���A���^�C���⊮��⌟�� #717
	//FindCandidateRealtime();
	mCandidateRequestTimer = 9;
	
	//�q���g�e�L�X�g�����݂��Ă�����⊮�����v��
	RequestCandidateFinderIfHintExist();
}

void	AView_Text::DeletePreviousChar( const ABool inIncludingDecomp )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( IsCaretMode() == false )
	{
		DeleteSelect(undoTag_DELKey);
	}
	else
	{
		//���V�t�g�ɂ��邩�A�P�����폜�ɂ��邩�̔���
		//���V�t�g�ݒ�ON���s���Ȃ�A���V�t�g�ɂ���
		ABool	shiftLeft = false;
		if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kShiftleftByBackspace) == true )
		{
			ATextIndex	pos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
			if(	GetIndentStart(GetCaretTextPoint()) >= pos && 
			   GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(GetCaretTextPoint().y) == GetCaretTextPoint().y &&
			   GetCaretTextPoint().x > 0 )
			{
				shiftLeft = true;
			}
		}
		//�����C���f���gOFF���͍��V�t�g����
		if( GetApp().SPI_GetAutoIndentMode() == false )
		{
			shiftLeft = false;
		}
		//���V�t�g
		if( shiftLeft == true )
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(false,inputSpaces);
		}
		//�O�ꕶ���폜
		else
		{
			//
			AIndex	end = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
			AIndex	start = GetTextDocumentConst().SPI_GetPrevCharTextIndex(end,inIncludingDecomp);
			if( start < 0 )   return;
			//Undo�A�N�V�����^�O�L�^
			GetTextDocument().SPI_RecordUndoActionTag(undoTag_BSKey);
			//
			DeleteTextFromDocument(start,end,false,false);
		}
	}
	
	//���A���^�C���⊮��⌟�� #717
	//FindCandidateRealtime();
	mCandidateRequestTimer = 9;
}

void	AView_Text::DeleteToLineEnd()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	end.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(end.y);
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
	//�e�L�X�g�폜
	DeleteTextFromDocument(start,end);
}

//R0213
void	AView_Text::DeleteToParagraphEnd()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	end.y = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(end.y)-1;
	end.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(end.y);
	//�i���̍Ō�̏ꍇ�͉��s�R�[�h���폜����
	ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(start);
	ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(end);
	if( spos == epos )
	{
		if( epos >= GetTextDocumentConst().SPI_GetTextLength() )   return;
		epos++;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,start);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,end);
	}
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
	//�e�L�X�g�폜
	DeleteTextFromDocument(start,end);
}

//#913
/**
�s���܂ł��폜
*/
void	AView_Text::DeleteToLineStart()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	if( start.x == 0 && end.x == 0 && start.y == end.y )
	{
		//#1207 �s���̏ꍇ�͑O�̕������폜
		//Undo�A�N�V�����^�O�L�^
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
		//�O�̕������폜
		DeletePreviousChar();
	}
	else
	{
		start.x = 0;
		//Undo�A�N�V�����^�O�L�^
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
		//�e�L�X�g�폜
		DeleteTextFromDocument(start,end);
	}
}

//#913
/**
�i�����܂ł��폜
*/
void	AView_Text::DeleteToParagraphStart()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	start.y = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(start.y);
	start.x = 0;
	//�i���̍ŏ��̏ꍇ�͒��O�̉��s�R�[�h���폜����
	ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(start);
	ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(end);
	if( spos == epos )
	{
		if( spos == 0 )   return;
		spos--;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,start);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,end);
	}
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
	//�e�L�X�g�폜
	DeleteTextFromDocument(start,end);
}

//#124
void	AView_Text::DeleteCurrentParagraph()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( spt.x != ept.x || spt.y != ept.y )//B0327 �����ǉ��i��s�ŃL�����b�g��Ԃ̏ꍇ��ept.y--���Ȃ��悤�ɂ���j
	{
		if( ept.x == 0 && ept.y > 0 )   ept.y--;
	}
	ATextIndex	spos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y));
	ATextIndex	epos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ept.y)+1);
	if( spos < epos )
	{
		ATextPoint	start, end;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,start);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,end);
		//Undo�A�N�V�����^�O�L�^
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
		//�e�L�X�g�폜
		DeleteTextFromDocument(start,end);
	}
}

void	AView_Text::CutToLineEnd()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	end.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(end.y);
	//Scrap�ۑ�
	AText	text;
	GetTextDocumentConst().SPI_GetText(start,end,text);
	if( text.GetItemCount() == 0 )   return;//B0000
AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Cut);
	//�e�L�X�g�폜
	DeleteTextFromDocument(start,end);
}

//R0213
void	AView_Text::CutToParagraphEnd( const ABool inAdditional )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	end.y = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(end.y)-1;
	end.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(end.y);
	//�i���̍Ō�̏ꍇ�͉��s�R�[�h���폜����
	ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(start);
	ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(end);
	if( spos == epos )
	{
		if( epos >= GetTextDocumentConst().SPI_GetTextLength() )   return;
		epos++;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,start);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,end);
	}
	//Scrap�ۑ�
	AText	text;
	GetTextDocumentConst().SPI_GetText(start,end,text);
	if( text.GetItemCount() == 0 )   return;//B0000
	if( inAdditional == true )
	{
		AText	scrapText;
		AScrapWrapper::GetClipboardTextScrap(scrapText);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
		text.InsertText(0,scrapText);
	}
AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Cut);
	//�e�L�X�g�폜
	DeleteTextFromDocument(start,end);
}

void	AView_Text::CutToLineStart()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	start.x = 0;
	//Scrap�ۑ�
	AText	text;
	GetTextDocumentConst().SPI_GetText(start,end,text);
	if( text.GetItemCount() == 0 )   return;//B0000
AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Cut);
	//�e�L�X�g�폜
	DeleteTextFromDocument(start,end);
}

//#913
/**
�}�[�N�܂ł��폜
*/
void	AView_Text::DeleteToMark()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( GetTextDocumentConst().SPI_IsTextPointValid(mMarkStartTextPoint) == true &&
	   GetTextDocumentConst().SPI_IsTextPointValid(mMarkEndTextPoint) == true )
	{
		//Undo�A�N�V�����^�O�L�^
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
		//�e�L�X�g�폜
		ATextPoint	spt = {0}, ept = {0};
		SPI_GetSelect(spt,ept);
		ATextPoint	dummy = {0};
		ADocument_Text::OrderTextPoint(mMarkStartTextPoint,spt,spt,dummy);
		ADocument_Text::OrderTextPoint(mMarkEndTextPoint,ept,dummy,ept);
		DeleteTextFromDocument(spt,ept);
	}
	//�}�[�N����
	mMarkStartTextPoint.x = mMarkStartTextPoint.y = -1;
	mMarkEndTextPoint.x = mMarkEndTextPoint.y = -1;
}

//#913
/**
Yank
*/
void	AView_Text::Yank()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//�L���o�b�t�@����f�[�^�擾
	AText	text;
	GetApp().SPI_GetKillBuffer(text);
	//�e�L�X�g����
	TextInput(undoTag_Typing,text);
}

//IndentStart(�s���̃^�u�A�X�y�[�X���I������ʒu)���擾����
ATextIndex	AView_Text::GetIndentStart( const ATextPoint& inTextPoint ) const
{
	//#318
	AText	zenkakuSpaceText;
	zenkakuSpaceText.SetZenkakuSpace();
	ABool	zenkakuSpaceEnable = (GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == true &&
			GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kZenkakuSpaceIndent) == true);
	//
	AIndex	paraStartLineIndex = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(inTextPoint.y);
	ATextIndex	paraStartTextIndex = GetTextDocumentConst().SPI_GetLineStart(paraStartLineIndex);
	ATextIndex	lineLength = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(paraStartLineIndex);
	for( ATextIndex pos = paraStartTextIndex; pos < paraStartTextIndex+lineLength; pos = GetTextDocumentConst().SPI_GetNextCharTextIndex(pos) )
	{
		AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(pos);
		if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
		else
		{
			//#318
			if( zenkakuSpaceEnable == true )
			{
				AText	text;
				GetTextDocumentConst().SPI_GetText1CharText(pos,text);
				if( text.Compare(zenkakuSpaceText) == true )
				{
					continue;
				}
			}
			return pos;
		}
	}
	return paraStartTextIndex+lineLength;
}

ABool	AView_Text::KeyBindAction( const AKeyBindKey inKeyBindKey, const AModifierKeys inModiferKeys, 
								  const AKeyBindAction inKeyBindAction,
                                  const AText& inTextForInsertTextAction,
								  ABool& outUpdateMenu )
{
	ABool	result = true;//win
	//���[�h�ݒ肩��A�L�[�ɑΉ�����L�[�A�N�V�������擾
	AKeyBindAction	action = keyAction_NOP;
	AText	actionText;
	ABool	isDefault = false, isAvailable = false;
	GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).
			GetData_KeyBindAction(inKeyBindKey,inModiferKeys,action,actionText,isDefault,isAvailable);
	
	//#481
	//OS(AppKit)�L�[�o�C���h����̍�����ݒ肷��ꍇ�́A�L�[�o�C���h�ݒ肪�����i�f�t�H���g�j�ꍇ�́A
	//OS����n���ꂽ�A�N�V�����iinKeyBindAction�j���g�p����B
	if( GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).
       GetData_Bool(AModePrefDB::kKeyBindDefaultIsOSKeyBind) == true && isDefault == true )
	{
		//OS����n���ꂽ�A�N�V������ݒ�
		action = inKeyBindAction;
		//OS�̎��̃A�N�V���������s���邽�߂ɁA�Ԃ�l��false��Ԃ�
		result = false;
	}
	//OS����n���ꂽ������}���A�N�V����(insertText:)�̏ꍇ�́AOS����n���ꂽ�e�L�X�g��actionText�ɐݒ�i���ŃL�[�}�N���ɋL�������j
	if( action == keyAction_InsertText )
	{
		actionText.SetText(inTextForInsertTextAction);
	}
	
	//#1336
	//esc�L�[�A���A�A�N�V�������Ȃ��ꍇ�A�t���[�e�B���O�������ʃE�C���h�E�ƃL�[���[�h���|�b�v�A�b�v��hide����B
	if( inKeyBindKey == kKeyBindKey_Escape && action == keyAction_NOP )
	{
		//�L�[���[�h���|�b�v�A�b�v��hide
		GetApp().SPI_HideFloatingIdInfoWindow();
		//�������ʂ�hide
		if( GetApp().SPI_IsMultiFileFindWorking() == false )
		{
			AWindowID	winID = GetApp().SPI_GetFloatingFindResultWindowID();
			if( winID != kObjectID_Invalid )
			{
				GetApp().SPI_GetFindResultWindow(winID).NVI_Hide();
				return true;
			}
		}
	}
	
	//#390 �L�[�}�N���L�^
	GetApp().SPI_RecordKeybindAction(action,actionText);
	
	//���j���[�X�V���邩�ǂ����̔���̂��߂Ɍ��݂̏�Ԃ��L��
	ABool	svDirty = GetTextDocument().NVI_IsDirty();
	ABool	svCaretMode = mCaretMode;
	//
	ABool	select = (mForceSelectMode==true);//#388 �����ŃL�[���肵�Ă͂����Ȃ��i�L�[�o�C���h�ݒ�ɏ]���ׂ��j((AKeyWrapper::IsShiftKeyPressed(inModiferKeys)==true) || (mForceSelectMode==true));
	ABool	edge = false;//#388 �����ŃL�[���肵�Ă͂����Ȃ��i�L�[�o�C���h�ݒ�ɏ]���ׂ��jAKeyWrapper::IsCommandKeyPressed(inModiferKeys);
	switch(action)
	{
		//#481
		//OS����n���ꂽ������}���A�N�V����
	  case keyAction_InsertText:
		{
			TextInput(undoTag_Typing,actionText);
			break;
		}
        //
	  case keyAction_string:
		{
			SPI_DoTool_Text(actionText,true);
			break;
		}
	  case keyAction_return:
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			TextInput(undoTag_Typing,text);
			//GetTextDocument().SPI_RecognizeSyntaxIfDirty();
			//#905 GetTextDocument().SPI_UpdateImportFileDataIfDirty();
			//#905
			//���@�F���ƃ��[�J���͈͐ݒ�
			//TriggerSyntaxRecognize();
			break;
		}
	  case keyAction_indentreturn:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			AText	text;
			text.Add(kUChar_LineEnd);
			TextInput(undoTag_Typing,text);
			//B0381 ���s�}���s�̃C���f���g���s�i���s���͌�łȂ��ƁA���Ƃ��΁A</p>�̑O�ŉ��s�����ꍇ�ȂǁA���ʂ��������Ȃ�Ȃ��B�j
			/*R0000 SDF�ł����s�}���s�̃C���f���g���s���Ė��Ȃ��͂����C���f���g���s���ׂ��B�c�[���R�}���h��</p><<<INDENT-RETURN�̏ꍇ�ȂǁB
			ABool	useSyntaxDef = 
					(GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseSyntaxDefinitionIndent) == true &&
			GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseBuiltinSyntaxDefinitionFile) == true);*/
			if( GetApp().SPI_GetAutoIndentMode() == true )//#638
			{
				if( GetTextDocumentConst().SPI_GetIndentType() == kIndentType_SDFIndent || 
					GetTextDocumentConst().SPI_GetIndentType() == kIndentType_RegExpIndent /*B0381 �C���f���gN/A�Ȃ���s�}���s�̃C���f���g�͂��Ȃ�*/ )
				{
					if( GetCaretTextPoint().y > 0 ) 
					{
						ATextPoint	spt, ept;
						spt = GetCaretTextPoint();
						spt.y--;
						ept = spt;
						DoIndent(undoTag_NOP,spt,ept,inputSpaces,kIndentTriggerType_ReturnKey_CurrentLine);//#249 #639 #650
					}
				}
				DoIndent(undoTag_NOP,inputSpaces,kIndentTriggerType_ReturnKey_NewLine);//B0381 undoTag��NOP�� #249 #639 #650
			}
			//#905
			//���@�F���ƃ��[�J���͈͐ݒ�
			//TriggerSyntaxRecognize();
			break;
		}
	  case keyAction_indent_byspace_return://#249
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			TextInput(undoTag_Typing,text);
			if( GetApp().SPI_GetAutoIndentMode() == true )//#638
			{
				if( GetTextDocumentConst().SPI_GetIndentType() == kIndentType_SDFIndent || 
					GetTextDocumentConst().SPI_GetIndentType() == kIndentType_RegExpIndent  )
				{
					if( GetCaretTextPoint().y > 0 ) 
					{
						ATextPoint	spt, ept;
						spt = GetCaretTextPoint();
						spt.y--;
						ept = spt;
						DoIndent(/*#650 false,*/undoTag_NOP,spt,ept,true,kIndentTriggerType_ReturnKey_CurrentLine);//#249 #639 #650
					}
				}
				
				DoIndent(/*#650 false,*/undoTag_NOP,true,kIndentTriggerType_ReturnKey_NewLine);//#249 #639
			}
			//#905
			//���@�F���ƃ��[�J���͈͐ݒ�
			//TriggerSyntaxRecognize();
			break;
		}
	  case keyAction_indent_onlynewline_return://#249
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			AText	text;
			text.Add(kUChar_LineEnd);
			TextInput(undoTag_Typing,text);
			
			if( GetApp().SPI_GetAutoIndentMode() == true )//#638
			{
				DoIndent(/*#650 false,*/undoTag_NOP,inputSpaces,kIndentTriggerType_ReturnKey_NewLine);//#249 #639 #650
			}
			//#905
			//���@�F���ƃ��[�J���͈͐ݒ�
			//TriggerSyntaxRecognize();
			break;
		}
	  case keyAction_indent_onlynewline_byspace_return://#249
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			TextInput(undoTag_Typing,text);
			
			if( GetApp().SPI_GetAutoIndentMode() == true )//#638
			{
				DoIndent(/*#650 false,*/undoTag_NOP,true,kIndentTriggerType_ReturnKey_NewLine);//#249 #639 #650
			}
			//#905
			//���@�F���ƃ��[�J���͈͐ݒ�
			//TriggerSyntaxRecognize();
			break;
		}
		//�^�u
	  case keyAction_tab:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			InputTab(inputSpaces);//#249
			break;
		}
	  case keyAction_tab_byspace://#249
		{
			InputTab(true);
			break;
		}
		//�C���f���g�^�^�u
	  case keyAction_indenttab:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			if( spt.y == ept.y )
			{
				IndentTab(inputSpaces,inputSpaces);//#249
			}
			//#815
			else
			{
				//�����s�I�����͍��E�V�t�g
				ShiftRightLeft((inModiferKeys==kModifierKeysMask_None),inputSpaces,true);//#1188
			}
			break;
		}
	  case keyAction_indent_byspace_tab://#249
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);
			IndentTab(true,inputSpaces);
			break;
		}
	  case keyAction_indent_byspace_tab_byspace://#249
		{
			IndentTab(true,true);
			break;
		}
		//�C���f���g
	  case keyAction_indent://#249
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			//#255 DoIndent(false,undoTag_NOP,inputSpaces);
			Indent(inputSpaces);//#255
			break;
		}
	  case keyAction_indent_byspace://#249
		{
			//#255 DoIndent(false,undoTag_NOP,true);
			Indent(true);//#255
			break;
		}
		//
	  case keyAction_space:
		{
			AText	text;
			text.Add(kUChar_Space);
			TextInput(undoTag_Typing,text);
			break;
		}
	  case keyAction_abbrevnext:
		{
			//#1160
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseUserDefinedSDF) == false &&
				GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetKeywordListItemCount() == 0 )
			{
				//�L�[���[�h�������ꍇ�́A�L�[���͕��������̂܂ܓ��͂���iShift+space�Ȃǁj
				TextInput(undoTag_Typing,inTextForInsertTextAction);
				break;
			}
			//
			InputAbbrev(true);
			break;
		}
	  case keyAction_abbrevprev:
		{
			//#1160
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseUserDefinedSDF) == false &&
				GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetKeywordListItemCount() == 0 )
			{
				//�L�[���[�h�������ꍇ�́A�L�[���͕��������̂܂ܓ��͂���iShift+space�Ȃǁj
				TextInput(undoTag_Typing,inTextForInsertTextAction);
				break;
			}
			//
			InputAbbrev(false);
			break;
		}
	  case keyAction_deletenext:
		{
			DeleteNextChar();
			break;
		}
	  case keyAction_deleteprev:
		{
			DeletePreviousChar();
			break;
		}
	  case keyAction_deleteBackwardByDecomposingPreviousCharacter://#913
		{
			DeletePreviousChar(false);
			break;
		}
	  case keyAction_deletelineend:
		{
			DeleteToLineEnd();
			break;
		}
	  case keyAction_cutlineend:
		{
			outUpdateMenu = true;
			CutToLineEnd();
			break;
		}
	  case keyAction_cutlinestart:
		{
			outUpdateMenu = true;
			CutToLineStart();
			break;
		}
	  case keyAction_deleteparagraphend://R0213
		{
			DeleteToParagraphEnd();
			break;
		}
	  case keyAction_cutparagraphend://R0213
		{
			CutToParagraphEnd(false);
			break;
		}
		//#913
	  case keyAction_deletelinestart:
		{
			DeleteToLineStart();
			break;
		}
		//#913
	  case keyAction_deleteparagraphstart:
		{
			DeleteToParagraphStart();
			break;
		}
		//#913
	  case keyAction_deleteToMark:
		{
			DeleteToMark();
			break;
		}
		//#913
	  case keyAction_yank:
		{
			Yank();
			break;
		}
		//
	  case keyAction_additionalcutparagraphend://R0213
		{
			CutToParagraphEnd(true);
			break;
		}
	  case keyAction_caretup:
	  case keyAction_caretdown:
	  case keyAction_caretleft:
	  case keyAction_caretright:
		{
			if( select == true && edge == true )
			{
				ArrowKeyEdge(action,true,false);
			}
			else if( edge == true )
			{
				ArrowKeyEdge(action,false,false);
			}
			else if( select == true )
			{
				ArrowKeySelect(action);
			}
			else
			{
				ArrowKey(action);
			}
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_pageup:
		{
			PageupKey(false);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_pagedown:
		{
			PagedownKey(false);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_pageupwithcaret:
		{
			PageupKey(true);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_pagedownwithcaret:
		{
			PagedownKey(true);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_caretlinestart:
		{
			ArrowKeyEdge(keyAction_caretleft,select,false);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_caretlineend:
		{
			ArrowKeyEdge(keyAction_caretright,select,false);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_caretparagraphstart:
		{
			ArrowKeyEdge(keyAction_caretleft,select,true);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_caretparagraphend:
		{
			ArrowKeyEdge(keyAction_caretright,select,true);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_selectlinestart:
		{
			ArrowKeyEdge(keyAction_caretleft,true,false);
			break;
		}
	  case keyAction_selectlineend:
		{
			ArrowKeyEdge(keyAction_caretright,true,false);
			break;
		}
		//#913
	  case keyAction_selectparagraphstart:
		{
			ArrowKeyEdge(keyAction_caretleft,true,true);
			break;
		}
		//#913
	  case keyAction_selectparagraphend:
		{
			ArrowKeyEdge(keyAction_caretright,true,true);
			break;
		}
		//#913
	  case keyAction_selectLine:
		{
			SelectWholeLine();
			break;
		}
		//#913
	  case keyAction_selectParagraph:
		{
			SelectWholeParagraph();
			break;
		}
		//#913
	  case keyAction_selectWord:
		{
			SelectWord();
			break;
		}
		//#913
	  case keyAction_selectAll:
		{
			SetSelect(0,GetTextDocumentConst().SPI_GetTextLength());
			break;
		}
		//#815
	  case keyAction_shiftleft:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(false,inputSpaces);
			break;
		}
	  case keyAction_shiftright:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(true,inputSpaces);
			break;
		}
		//
	  case keyAction_home:
		{
			HomeKey();
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_end:
		{
			EndKey();
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_recenter:
		{
			Recenter();
			break;
		}
	  case keyAction_correspondence:
		{
			InputCorrespondence();
			break;
		}
	  case keyAction_carethome:
		{
			ArrowKeyEdge(keyAction_caretup,select,false);
			break;
		}
	  case keyAction_caretend:
		{
			ArrowKeyEdge(keyAction_caretdown,select,false);
			break;
		}
	  case keyAction_selecthome:
		{
			ArrowKeyEdge(keyAction_caretup,true,false);
			break;
		}
	  case keyAction_selectend:
		{
			ArrowKeyEdge(keyAction_caretdown,true,false);
			break;
		}
	  case keyAction_selectbrace:
		{
			SelectBrace();
			break;
		}
	  case keyAction_previousword:
		{
			ArrowKeyWord(keyAction_caretleft,select,false);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_nextword:
		{
			ArrowKeyWord(keyAction_caretright,select,false);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_previousword_linestop:
		{
			ArrowKeyWord(keyAction_caretleft,select,true);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_nextword_linestop:
		{
			ArrowKeyWord(keyAction_caretright,select,true);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_selectpreviousword_linestop:
		{
			ArrowKeyWord(keyAction_caretleft,true,true);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_selectnextword_linestop:
		{
			ArrowKeyWord(keyAction_caretright,true,true);
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_selectleft:
		{
			ArrowKeySelect(keyAction_caretleft);
			break;
		}
	  case keyAction_selectright:
		{
			ArrowKeySelect(keyAction_caretright);
			break;
		}
	  case keyAction_selectup:
		{
			ArrowKeySelect(keyAction_caretup);
			break;
		}
	  case keyAction_selectdown:
		{
			ArrowKeySelect(keyAction_caretdown);
			break;
		}
	  case keyAction_selectnextword:
		{
			ArrowKeyWord(keyAction_caretright,true,false);
			break;
		}
	  case keyAction_selectprevword:
		{
			ArrowKeyWord(keyAction_caretleft,true,false);
			break;
		}
		//#913
	  case keyAction_setMark:
		{
			SetMark();
			break;
		}
		//#913
	  case keyAction_swapWithMark:
		{
			SwapMark();
			break;
		}
		//#913
	  case keyAction_selectToMark:
		{
			SelectToMark();
			break;
		}
		//
	  case keyAction_Undo:
		{
			outUpdateMenu = true;
			UndoRedo(true);
			break;
		}
	  case keyAction_Redo:
		{
			outUpdateMenu = true;
			UndoRedo(false);
			break;
		}
	  case keyAction_Cut:
		{
			outUpdateMenu = true;
			Cut(false);
			break;
		}
	  case keyAction_Copy:
		{
			outUpdateMenu = true;
			Copy(false);
			break;
		}
	  case keyAction_Paste:
		{
			Paste();
			break;
		}
	  case keyAction_additionalcopy://R0213
		{
			outUpdateMenu = true;
			Copy(true);
			break;
		}
	  case keyAction_additionalcut://R0213
		{
			outUpdateMenu = true;
			Cut(true);
			break;
		}
	  case keyAction_sameletterup:
		{
			SameLetterUp();
			break;
		}
	  case keyAction_duplicateline:
		{
			DuplicateLine();
			break;
		}
	  case keyAction_insertline:
		{
			InsertEmptyLine();
			break;
		}
	  case keyAction_selectmode:
		{
			mForceSelectMode = !mForceSelectMode;
			break;
		}
	  case keyAction_scrollleft:
		{
			NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
			break;
		}
	  case keyAction_scrollright:
		{
			NVI_Scroll(NVI_GetHScrollBarUnit(),0);
			break;
		}
	  case keyAction_scrollup:
		{
			NVI_Scroll(0,-NVI_GetVScrollBarUnit(),true,true,kScrollTrigger_ScrollKey);//#1031
			break;
		}
	  case keyAction_scrolldown:
		{
			NVI_Scroll(0,NVI_GetVScrollBarUnit(),true,true,kScrollTrigger_ScrollKey);//#1031
			break;
		}
		//R0006
	  case keyAction_prevdiff:
		{
			PrevDiff();
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_nextdiff:
		{
			NextDiff();
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
		//RC2
	  case keyAction_idinfo:
		{
			//�L�[���[�h���
			//#853 IdInfo();
			DisplayIdInfo(true,kIndex_Invalid,kIndex_Invalid);//#853
			break;
		}
		//#124
	  case keyAction_deletewholeparagraph:
		{
			DeleteCurrentParagraph();
			break;
		}
		//#150
	  case keyAction_nextheader:
		{
			JumpNext();
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_prevheader:
		{
			JumpPrev();
			//�L�����b�g�\�� #688
			ShowCaretIfReserved();
			break;
		}
		//#357
	  case keyAction_switch_nexttab:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SwitchTabNext();
			}
			break;
		}
	  case keyAction_switch_prevtab:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SwitchTabPrev();
			}
			break;
		}
		//#594
	  case keyAction_linedialog:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowMoveLineWindow();
			}
			break;
		}
		//#638
	  case keyAction_autoindentmode:
		{
			GetApp().SPI_SetAutoIndentMode(!GetApp().SPI_GetAutoIndentMode());
			break;
		}
		//#724
	  case keyAction_macro1:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(0);
			}
			break;
		}
	  case keyAction_macro2:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(1);
			}
			break;
		}
	  case keyAction_macro3:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(2);
			}
			break;
		}
	  case keyAction_macro4:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(3);
			}
			break;
		}
	  case keyAction_macro5:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(4);
			}
			break;
		}
	  case keyAction_macro6:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(5);
			}
			break;
		}
	  case keyAction_macro7:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(6);
			}
			break;
		}
	  case keyAction_macro8:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(7);
			}
			break;
		}
	  case keyAction_macro9:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(8);
			}
			break;
		}
		//#1154
	  case keyAction_swapLetter:
		{
			SwapLetter();
			break;
		}
		//#1397
	  case keyAction_findnext:
		{
            SPI_FindNext();
			break;
		}
	  case keyAction_findprev:
		{
			SPI_FindPrev();
			break;
		}
		//#1399
	  case keyAction_caretprevparagraphstart:
		{
			ArrowKeyNextPrevEdge(keyAction_caretup,false);
			break;
		}
	  case keyAction_caretnextparagraphend:
		{
			ArrowKeyNextPrevEdge(keyAction_caretdown,false);
			break;
		}
	  case keyAction_selectprevparagraphstart:
		{
			ArrowKeyNextPrevEdge(keyAction_caretup,true);
			break;
		}
	  case keyAction_selectnextparagraphend:
		{
			ArrowKeyNextPrevEdge(keyAction_caretdown,true);
			break;
		}
		//
	  default:
		{
			result = false;//win
			break;
		}
	}
	//��ԕω���������΃��j���[�X�V���Ȃ�
	if( svDirty == GetTextDocument().NVI_IsDirty() && svCaretMode == mCaretMode )
	{
		outUpdateMenu = false;
	}
	return result;//win
}

//#558
/**
�c�������[�h�p ���L�[�̕ϊ�
*/
AKeyBindAction	AView_Text::ConvertArrowKeyForVerticalMode( const AKeyBindAction inAction )
{
	AKeyBindAction	action = inAction;
	switch(inAction)
	{
	  case keyAction_caretleft:
		{
			action = keyAction_caretdown;
			break;
		}
	  case keyAction_caretright:
		{
			action = keyAction_caretup;
			break;
		}
	  case keyAction_caretup:
		{
			action = keyAction_caretleft;
			break;
		}
	  case keyAction_caretdown:
		{
			action = keyAction_caretright;
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return action;
}

//���L�[�i�m�[�}���j
void	AView_Text::ArrowKey( const AKeyBindAction inAction )
{
	//�I��\�����#699
	CancelReservedSelection();
	//�⊮���͏�ԉ��� #717
	ClearAbbrevInputMode(true,false);
	
	//#558
	//�c�������[�h�Ȃ���L�[��ϊ�����
	AKeyBindAction	action = inAction;
	if( NVI_GetVerticalMode() == true )
	{
		action = ConvertArrowKeyForVerticalMode(inAction);
	}
	
	//ArrowKeySelect�Ɠ����A���S���Y������_���H�����K�v
	ATextPoint	caretTextPoint;
	AImagePoint	imagePoint;
	switch(action)
	{
		//��
	  case keyAction_caretleft:
		{
			if( IsCaretMode() == true )
			{
				GetCaretTextPoint(caretTextPoint);
				/*B0000 if( caretTextPoint.x == 0 )
				{
					if( caretTextPoint.y == 0 )
					{
						caretTextPoint.x = 0;
					}
					else
					{
						caretTextPoint.y--;
						caretTextPoint.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(caretTextPoint.y);
					}
				}
				else
				{
					ATextIndex	index = GetTextDocument().SPI_GetTextIndexFromTextPoint(caretTextPoint);
					index = GetTextDocument().SPI_GetPrevCharTextIndex(index);
					GetTextDocument().SPI_GetTextPointFromTextIndex(index,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);*/
				//B0000 �s�܂�Ԃ��̍s���̏ꍇ�A�s���P�����ڂ̌�́��L�[�ōs���ړ��A�s���ł́��L�[�őO�s�̍Ō�̕����̑O�ֈړ�����悤�ɏC��
				ATextIndex	index = GetTextDocument().SPI_GetTextIndexFromTextPoint(caretTextPoint);
				if( index == 0 )
				{
					//��������
				}
				else
				{
					index = GetTextDocument().SPI_GetPrevCharTextIndex(index);
					GetTextDocument().SPI_GetTextPointFromTextIndex(index,caretTextPoint,true);
					//#450
					//�ړ���text point���܂肽���ݒ��̍s�Ȃ�A�܂肽���܂�Ă��Ȃ��s�̍s���܂Ŗ߂�
					if( mLineImageInfo_Height.Get(caretTextPoint.y) == 0 )
					{
						for( ; caretTextPoint.y > 0; caretTextPoint.y-- )
						{
							if( mLineImageInfo_Height.Get(caretTextPoint.y) > 0 )
							{
								break;
							}
						}
						//�s��
						caretTextPoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(caretTextPoint.y);
					}
					//�L�����b�g�ݒ�
					SetCaretTextPoint(caretTextPoint);
				}
			}
			else
			{
				ATextPoint	pt1, pt2;
				ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt1);
			}
			break;
		}
		//�E
	  case keyAction_caretright:
		{
			if( IsCaretMode() == true )
			{
				GetCaretTextPoint(caretTextPoint);
				/*B0000 if( caretTextPoint.x >= GetTextDocument().SPI_GetLineLengthWithoutLineEnd(caretTextPoint.y) )
				{
					if( caretTextPoint.y >= GetTextDocument().SPI_GetLineCount()-1 )
					{
						caretTextPoint.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(caretTextPoint.y);
					}
					else
					{
						caretTextPoint.y++;
						caretTextPoint.x = 0;
					}
				}
				else
				{
					ATextIndex	index = GetTextDocument().SPI_GetTextIndexFromTextPoint(caretTextPoint);
					index = GetTextDocument().SPI_GetNextCharTextIndex(index);
					GetTextDocument().SPI_GetTextPointFromTextIndex(index,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);*/
				//B0000 �s�܂�Ԃ��̍s���̏ꍇ�A�s���P�����O�̌�́��L�[�ōs���ړ��A�s���ł́��L�[�Ŏ��s�̍s���P�����ڂ̌�ֈړ�����悤�ɏC��(TextEdit�ɍ��킹��j
				ATextIndex	index = GetTextDocument().SPI_GetTextIndexFromTextPoint(caretTextPoint);
				if( index >= GetTextDocumentConst().SPI_GetTextLength() )
				{
					//��������
				}
				else
				{
					index = GetTextDocument().SPI_GetNextCharTextIndex(index);
					GetTextDocument().SPI_GetTextPointFromTextIndex(index,caretTextPoint,true);
					//#450
					//�ړ���text point���܂肽���ݒ��̍s�Ȃ�A�܂肽���܂�Ă��Ȃ��s�̍s���܂Ői��
					if( mLineImageInfo_Height.Get(caretTextPoint.y) == 0 )
					{
						for( ; caretTextPoint.y < GetTextDocument().SPI_GetLineCount()-1; caretTextPoint.y++ )
						{
							if( mLineImageInfo_Height.Get(caretTextPoint.y) > 0 )
							{
								break;
							}
						}
						//�s��
						caretTextPoint.x = 0;
					}
					//�L�����b�g�ݒ�
					SetCaretTextPoint(caretTextPoint);
				}
			}
			else
			{
				ATextPoint	pt1, pt2;
				ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt2);
			}
			break;
		}
		//��
	  case keyAction_caretup:
		{
			if( IsCaretMode() == true )
			{
				ANumber	svOldArrowImageX = GetOldArrowImageX();
				GetCaretTextPoint(caretTextPoint);
				if( caretTextPoint.y == 0 )
				{
					caretTextPoint.x = 0;
				}
				else
				{
					caretTextPoint.y--;
					//#450
					//�ړ���text point���܂肽���ݒ��̍s�Ȃ�A�܂肽���܂�Ă��Ȃ��s�܂Ŗ߂�
					for( ; caretTextPoint.y > 0; caretTextPoint.y-- )
					{
						if( mLineImageInfo_Height.Get(caretTextPoint.y) > 0 )
						{
							break;
						}
					}
					//
					imagePoint.y = GetImageYByLineIndex(caretTextPoint.y)+2;//#450 caretTextPoint.y * GetLineHeightWithMargin() +2;
					imagePoint.x = GetOldArrowImageX();
					GetTextPointFromImagePoint(imagePoint,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);
				SetOldArrowImageX(svOldArrowImageX);
			}
			else
			{
				ATextPoint	pt1, pt2;
				ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt1);
			}
			break;
		}
		//��
	  case keyAction_caretdown:
		{
			if( IsCaretMode() == true )
			{
				ANumber	svOldArrowImageX = GetOldArrowImageX();
				GetCaretTextPoint(caretTextPoint);
				if( caretTextPoint.y >= GetTextDocument().SPI_GetLineCount()-1 )
				{
					caretTextPoint.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(caretTextPoint.y);
				}
				else
				{
					caretTextPoint.y++;
					//
					imagePoint.y = GetImageYByLineIndex(caretTextPoint.y)+2;//#450 caretTextPoint.y * GetLineHeightWithMargin() +2;
					imagePoint.x = GetOldArrowImageX();
					GetTextPointFromImagePoint(imagePoint,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);
				SetOldArrowImageX(svOldArrowImageX);
			}
			else
			{
				ATextPoint	pt1, pt2;
				ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt2);
			}
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
	//�X�N���[���ʒu�␳
	AdjustScroll(GetCaretTextPoint());
}

//���L�[�i�I���j
void	AView_Text::ArrowKeySelect( const AKeyBindAction inAction )
{
	//�⊮���͏�ԉ��� #717
	ClearAbbrevInputMode(true,false);
	
	//#558
	//�c�������[�h�Ȃ���L�[��ϊ�����
	AKeyBindAction	action = inAction;
	if( NVI_GetVerticalMode() == true )
	{
		action = ConvertArrowKeyForVerticalMode(inAction);
	}
	
	ATextPoint	selectPoint = GetCaretTextPoint();
	if( IsCaretMode() == false )
	{
		selectPoint = GetSelectTextPoint();
	}
	//#108
	//��`�I�𒆂͋�`�͈͂̍Ō�̃|�C���g����J�n����
	if( mCaretMode == false && mKukeiSelectionMode == true )
	{
		selectPoint = GetSelectTextPoint();
		if( mKukeiSelected_End.GetItemCount() > 0 )
		{
			selectPoint = mKukeiSelected_End.Get(mKukeiSelected_End.GetItemCount()-1);
		}
	}
	//
	ATextIndex	selectPos = GetTextDocument().SPI_GetTextIndexFromTextPoint(selectPoint);
	switch(action)
	{
		//��
	  case keyAction_caretleft:
		{
			selectPos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(selectPos);
			GetTextDocument().SPI_GetTextPointFromTextIndex(selectPos,selectPoint,true);//B0000
			//#450
			//�ړ���text point���܂肽���ݒ��̍s�Ȃ�A�܂肽���܂�Ă��Ȃ��s�̍s���܂Ŗ߂�
			if( mLineImageInfo_Height.Get(selectPoint.y) == 0 )
			{
				for( ; selectPoint.y > 0; selectPoint.y-- )
				{
					if( mLineImageInfo_Height.Get(selectPoint.y) > 0 )
					{
						break;
					}
				}
				//�s��
				selectPoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(selectPoint.y);
			}
			break;
		}
		//�E
	  case keyAction_caretright:
		{
			selectPos = GetTextDocumentConst().SPI_GetNextCharTextIndex(selectPos);
			GetTextDocument().SPI_GetTextPointFromTextIndex(selectPos,selectPoint,true);//B0000
			//#450
			//�ړ���text point���܂肽���ݒ��̍s�Ȃ�A�܂肽���܂�Ă��Ȃ��s�̍s���܂Ői��
			if( mLineImageInfo_Height.Get(selectPoint.y) == 0 )
			{
				for( ; selectPoint.y < GetTextDocument().SPI_GetLineCount()-1; selectPoint.y++ )
				{
					if( mLineImageInfo_Height.Get(selectPoint.y) > 0 )
					{
						break;
					}
				}
				//�s��
				selectPoint.x = 0;
			}
			break;
		}
		//��
	  case keyAction_caretup:
		{
			//B0362
			if( selectPoint.y == 0 )
			{
				selectPoint.x = 0;
			}
			else
			{
				AImagePoint	imagePoint;
				GetImagePointFromTextPoint(selectPoint,imagePoint);
				//#450
				//�ړ���text point���܂肽���ݒ��̍s�Ȃ�A�܂肽���܂�Ă��Ȃ��s�܂Ŗ߂�
				for( ; selectPoint.y-1 > 0 ; selectPoint.y-- )
				{
					if( mLineImageInfo_Height.Get(selectPoint.y-1) > 0 )
					{
						break;
					}
				}
				//
				imagePoint.y -= GetLineHeightWithMargin(selectPoint.y-1);//#450 GetLineHeightWithMargin();
				GetTextPointFromImagePoint(imagePoint,selectPoint);
			}
			break;
		}
		//��
	  case keyAction_caretdown:
		{
			//B0362
			if( selectPoint.y >= GetTextDocument().SPI_GetLineCount()-1 )
			{
				selectPoint.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(selectPoint.y);
			}
			else
			{
				AImagePoint	imagePoint;
				GetImagePointFromTextPoint(selectPoint,imagePoint);
				//
				imagePoint.y += GetLineHeightWithMargin(selectPoint.y) + 2;//#450 GetLineHeightWithMargin();
				GetTextPointFromImagePoint(imagePoint,selectPoint);
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	if( mCaretMode == true || mKukeiSelectionMode == false )
	{
		SetSelect(GetCaretTextPoint(),selectPoint);
	}
	else //#108
	{
		//��`�I�𒆂̏ꍇ
		AImagePoint	spt = mCaretImagePoint;
		if( mKukeiSelected_Start.GetItemCount() > 0 )
		{
			GetImagePointFromTextPoint(mKukeiSelected_Start.Get(0),spt);
		}
		AImagePoint	ept = {0};
		GetImagePointFromTextPoint(selectPoint,ept);
		SetKukeiSelect(spt,ept);
	}
	
	//�X�N���[���ʒu�␳
	AdjustScroll(GetSelectTextPoint(),0);//B0434
}

//���L�[�i�P��ړ��j
void	AView_Text::ArrowKeyWord( const AKeyBindAction inAction, const ABool inSelect, const ABool inLineStop )
{
	//�⊮���͏�ԉ��� #717
	ClearAbbrevInputMode(true,false);
	
	//#558
	//�c�������[�h�Ȃ���L�[��ϊ�����
	AKeyBindAction	action = inAction;
	if( NVI_GetVerticalMode() == true )
	{
		action = ConvertArrowKeyForVerticalMode(inAction);
	}
	
	ATextPoint	textpoint = GetCaretTextPoint();
	if( IsCaretMode() == false )
	{
		if( inSelect == false )
		{
			ArrowKey(inAction);
			return;
		}
		textpoint = GetSelectTextPoint();
	}
	ATextIndex	textpos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(textpoint);
	switch(action)
	{
		//��
	  case keyAction_caretleft:
		{
			if( textpos == 0 )   break;
			textpos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(textpos);
			ATextIndex	spos,epos;
			GetTextDocument().SPI_FindWord(textpos,spos,epos);
			while( spos > 0 )
			{
				if( inLineStop == true && GetTextDocumentConst().SPI_GetTextChar(spos-1) == kUChar_LineEnd )   break;
				if( GetTextDocumentConst().SPI_IsAsciiAlphabet(spos) == true || GetTextDocumentConst().SPI_IsUTF8Multibyte(spos) == true )   break;
				spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
				GetTextDocument().SPI_FindWord(spos,spos,epos);
			}
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,textpoint,true);//B0000
			//#450
			//�ړ���text point���܂肽���ݒ��̍s�Ȃ�A�܂肽���܂�Ă��Ȃ��s�̍s���܂Ŗ߂�
			if( mLineImageInfo_Height.Get(textpoint.y) == 0 )
			{
				for( ; textpoint.y > 0; textpoint.y-- )
				{
					if( mLineImageInfo_Height.Get(textpoint.y) > 0 )
					{
						break;
					}
				}
				//�s��
				textpoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y);
			}
			//
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//�E
	  case keyAction_caretright:
		{
			ATextIndex	spos,epos;
			GetTextDocument().SPI_FindWord(textpos,spos,epos);
			while( epos < GetTextDocumentConst().SPI_GetTextLength() )
			{
				if( inLineStop == true && GetTextDocumentConst().SPI_GetTextChar(epos) == kUChar_LineEnd )   break;
				if( GetTextDocumentConst().SPI_IsAsciiAlphabet(epos-1) == true || GetTextDocumentConst().SPI_IsUTF8Multibyte(epos-1) == true )   break;//B0000
				GetTextDocument().SPI_FindWord(epos,spos,epos);
			}
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,textpoint,true);//B0000
			//#450
			//�ړ���text point���܂肽���ݒ��̍s�Ȃ�A�܂肽���܂�Ă��Ȃ��s�̍s���܂Ői��
			if( mLineImageInfo_Height.Get(textpoint.y) == 0 )
			{
				for( ; textpoint.y < GetTextDocument().SPI_GetLineCount()-1; textpoint.y++ )
				{
					if( mLineImageInfo_Height.Get(textpoint.y) > 0 )
					{
						break;
					}
				}
				//�s��
				textpoint.x = 0;
			}
			//
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//��A��
	  case keyAction_caretup:
	  case keyAction_caretdown:
		{
			if( inSelect == true )
			{
				ArrowKeySelect(inAction);
			}
			else
			{
				ArrowKey(inAction);
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	
	//�X�N���[���ʒu�␳
	textpoint = GetCaretTextPoint();
	if( inSelect == true )   textpoint = GetSelectTextPoint();
	AdjustScroll(textpoint);
}

//���L�[�i�[�܂ňړ��j
void	AView_Text::ArrowKeyEdge( const AKeyBindAction inAction, const ABool inSelect, const ABool inParagraphEdge )
{
	//�⊮���͏�ԉ��� #717
	ClearAbbrevInputMode(true,false);
	
	//#558
	//�c�������[�h�Ȃ���L�[��ϊ�����
	AKeyBindAction	action = inAction;
	if( NVI_GetVerticalMode() == true )
	{
		action = ConvertArrowKeyForVerticalMode(inAction);
	}
	
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	ATextPoint	textpoint;
	switch(action)
	{
		//��
	  case keyAction_caretleft:
		{
			textpoint = start;
			if( inParagraphEdge == true )
			{
				textpoint.y = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(start.y);
			}
			textpoint.x = 0;
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//�E
	  case keyAction_caretright:
		{
			textpoint = end;
			if( inParagraphEdge == true )
			{
				textpoint.y = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(start.y)-1;
			}
			textpoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y);
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//��
	  case keyAction_caretup:
		{
			textpoint.x = 0;
			textpoint.y = 0;
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//��
	  case keyAction_caretdown:
		{
			textpoint.y = GetTextDocumentConst().SPI_GetLineCount()-1;
			textpoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y);
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	
	//�X�N���[���ʒu�␳
	textpoint = GetCaretTextPoint();
	if( inSelect == true )   textpoint = GetSelectTextPoint();
	AdjustScroll(textpoint);
}

//#1399
/**
�O�i�����A���i�����ֈړ��^�I��
*/
void	AView_Text::ArrowKeyNextPrevEdge( const AKeyBindAction inAction, const ABool inSelect )
{
	//�⊮���͏�ԉ��� #717
	ClearAbbrevInputMode(true,false);
	
	//#558
	//�c�������[�h�Ȃ���L�[��ϊ�����
	AKeyBindAction	action = inAction;
	if( NVI_GetVerticalMode() == true )
	{
		action = ConvertArrowKeyForVerticalMode(inAction);
	}
	
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	ATextPoint	textpoint;
	switch(action)
	{
		//��
	  case keyAction_caretup:
		{
			//�I�𒆂̏ꍇ��select point����ړ�����i�������A�L�����b�g�ړ��̏ꍇ��start����ړ�����j
			if( IsCaretMode() == false && inSelect == true )
			{
				GetSelectTextPoint(textpoint);
			}
			else
			{
				textpoint = start;
			}
			//���ݍ��[�̏ꍇ�́A�O�̍s�̍��[�ֈړ�����
			if( textpoint.x == 0 && textpoint.y > 0 )
			{
				textpoint.y--;
			}
			//���ݒi���̍ŏ��ֈړ�����
			textpoint.y = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(textpoint.y);
			textpoint.x = 0;
			//�L�����b�g�ݒ�^�Z���N�g�ݒ�
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//��
	  case keyAction_caretdown:
		{
			//�I�𒆂̏ꍇ��select point����ړ�����i�������A�L�����b�g�ړ��̏ꍇ��end����ړ�����j
			if( IsCaretMode() == false && inSelect == true )
			{
				GetSelectTextPoint(textpoint);
			}
			else
			{
				textpoint = end;
			}
			//���݉E�[�̏ꍇ�́A���̍s�ֈړ�����
			if( textpoint.x == GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y) &&
				textpoint.y+1 < GetTextDocumentConst().SPI_GetLineCount() )
			{
				textpoint.y++;
			}
			//���̒i���̂P�O�̍s�̉E�[�i�܂茻�ݒi���̍Ō�j�ֈړ�����
			textpoint.y = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(textpoint.y)-1;
			textpoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y);
			//�L�����b�g�ݒ�^�Z���N�g�ݒ�
			if( inSelect == true )
			{
				//���̍s�̍ŏ��ֈړ�����i�L�����b�g�ړ��Ɠ��삪�قȂ��Ă��蓝�ꐫ���Ȃ����AOS�̕W�����삪�����Ȃ��Ă���̂ō��킹��B�j
				if( textpoint.y + 1 < GetTextDocumentConst().SPI_GetLineCount() )
				{
					textpoint.x = 0;
					textpoint.y++;
				}
				//
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	
	//�X�N���[���ʒu�␳
	textpoint = GetCaretTextPoint();
	if( inSelect == true )   textpoint = GetSelectTextPoint();
	AdjustScroll(textpoint);
}

void	AView_Text::HomeKey()
{
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

void	AView_Text::EndKey()
{
	AImagePoint	pt;
	pt.x = 0;
	pt.y = NVM_GetImageHeight();//NVI_Scroll�̒��ŕ␳�����
	NVI_ScrollTo(pt);
}

void	AView_Text::PageupKey( const ABool inWithCaret )
{
	NVI_Scroll(0,-NVI_GetVScrollBarPageUnit());
	if( inWithCaret == true && NVM_IsImagePointInViewRect(mCaretImagePoint) == false )
	{
		AImagePoint	origin;
		NVI_GetOriginOfLocalFrame(origin);
		AImagePoint	imagePoint;
		imagePoint.x = mCaretImagePoint.x;
		imagePoint.y = origin.y + NVI_GetViewHeight()/2;
		ATextPoint	caretTextPoint;
		GetTextPointFromImagePoint(imagePoint,caretTextPoint);
		SetCaretTextPoint(caretTextPoint);
	}
	NVM_GetWindow().NVI_UpdateWindow();//#420
}

void	AView_Text::PagedownKey( const ABool inWithCaret )
{
	NVI_Scroll(0,NVI_GetVScrollBarPageUnit());
	if( inWithCaret == true && NVM_IsImagePointInViewRect(mCaretImagePoint) == false )
	{
		AImagePoint	origin;
		NVI_GetOriginOfLocalFrame(origin);
		AImagePoint	imagePoint;
		imagePoint.x = mCaretImagePoint.x;
		imagePoint.y = origin.y + NVI_GetViewHeight()/2;
		ATextPoint	caretTextPoint;
		GetTextPointFromImagePoint(imagePoint,caretTextPoint);
		SetCaretTextPoint(caretTextPoint);
	}
	NVM_GetWindow().NVI_UpdateWindow();//#420
}

void	AView_Text::Recenter()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	AImagePoint	imagespt;
	GetImagePointFromTextPoint(spt,imagespt);
	ANumber	viewHeight = NVI_GetViewHeight();
	AImagePoint	pt;
	pt.x = 0;
	pt.y = imagespt.y - viewHeight/2;
	NVI_ScrollTo(pt);
}

void	AView_Text::UndoRedo( const ABool inUndo )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	/*#182
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_FixInlineInput();//B0079
	}
	*/
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());//#688
	//
	AIndex	selectTextIndex;
	AItemCount	selectTextLength;
	if( inUndo == true )
	{
		AText	text;
		if( GetTextDocumentConst().SPI_GetUndoText(text) == false )   return;
		GetTextDocument().SPI_Undo(selectTextIndex,selectTextLength);
	}
	else
	{
		AText	text;
		if( GetTextDocumentConst().SPI_GetRedoText(text) == false )   return;
		GetTextDocument().SPI_Redo(selectTextIndex,selectTextLength);
	}
	if( selectTextIndex == kIndex_Invalid )   return;
	ATextPoint	caretPoint, selectPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(selectTextIndex,caretPoint);
	SetCaretTextPoint(caretPoint);
	if( selectTextLength > 0 )
	{
		GetTextDocument().SPI_GetTextPointFromTextIndex(selectTextIndex+selectTextLength,selectPoint);
		SetSelectTextPoint(selectPoint);
	}
	AdjustScroll_Center(GetCaretTextPoint());
}

void	AView_Text::Cut( const ABool inAdditional )//R0213
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	AText	text;
	GetSelectedText(text);
	if( text.GetItemCount() == 0 )   return;//#1400
	if( inAdditional == true )//R0213
	{
		AText	scrapText;
		AScrapWrapper::GetClipboardTextScrap(scrapText);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);
		text.InsertText(0,scrapText);
	}
	//#878
	//FIFO�N���b�v�{�[�h���[�h�Ȃ�FIFO�ɃL���[
	if( GetApp().SPI_IsFIFOClipboardMode() == true )
	{
		EnqueueFIFOClipboard(text);
	}
	//�N���b�v�{�[�h�ɐݒ�
	AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	DeleteSelect(undoTag_Cut);
}

void	AView_Text::Copy( const ABool inAdditional )//R0213
{
	AText	text;
	GetSelectedText(text);
	if( text.GetItemCount() == 0 )   return;
	//#150
	//�N���b�v�{�[�h�����ɒǉ�
	GetApp().SPI_AddClipboardHistory(text);
	//
	if( inAdditional == true )//R0213
	{
		AText	scrapText;
		AScrapWrapper::GetClipboardTextScrap(scrapText);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
		text.InsertText(0,scrapText);
	}
	//#878
	//FIFO�N���b�v�{�[�h���[�h�Ȃ�FIFO�ɃL���[
	if( GetApp().SPI_IsFIFOClipboardMode() == true )
	{
		EnqueueFIFOClipboard(text);
	}
	//�N���b�v�{�[�h�ɐݒ�
	AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win 
}

/**
FIFO�N���b�v�{�[�h�ɒǉ�
*/
void	AView_Text::EnqueueFIFOClipboard( const AText& inText )
{
	//FIFO�ɂ�
	GetApp().SPI_EnqueueFIFOClipboard(inText);
	//notification�\��
	AText	firsttext;
	GetApp().SPI_GetFirstTextInFIFOClipboard(firsttext);
	AText	infotext;
	infotext.SetLocalizedText("Notification_CurrentPaste");
	infotext.ReplaceParamText('0',firsttext);
	AText	notificationTitle;
	notificationTitle.SetLocalizedText("Notification_FIFOCopy");
	if( mTextWindowID != kObjectID_Invalid )
	{
		SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_General(notificationTitle,infotext,GetEmptyText());
		SPI_ShowNotificationPopupWindow(true);
	}
}

/**
FIFO�N���b�v�{�[�h������o��
*/
void	AView_Text::DequeueFIFOClipboard( AText& outText )
{
	if( GetApp().SPI_GetFIFOClipboardCount() > 0 )
	{
		//FIFO�L���[����e�L�X�g�擾
		GetApp().SPI_DequeueFIFOClipboard(outText);
		//�L���[����擾�����e�L�X�g���N���b�v�{�[�h�ɐݒ�
		AScrapWrapper::SetClipboardTextScrap(outText,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300
	}
	else
	{
		//�N���b�v�{�[�h����e�L�X�g�擾
		AScrapWrapper::GetClipboardTextScrap(outText);
	}
	//notification�\��
	AText	firsttext;
	if( GetApp().SPI_GetFIFOClipboardCount() > 0 )
	{
		GetApp().SPI_GetFirstTextInFIFOClipboard(firsttext);
	}
	else
	{
		AScrapWrapper::GetClipboardTextScrap(firsttext);
	}
	AText	infotext;
	infotext.SetLocalizedText("Notification_CurrentPaste");
	infotext.ReplaceParamText('0',firsttext);
	AText	notificationTitle;
	notificationTitle.SetLocalizedText("Notification_FIFOCopy");
	if( mTextWindowID != kObjectID_Invalid )
	{
		SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_General(notificationTitle,infotext,GetEmptyText());
		SPI_ShowNotificationPopupWindow(true);
	}
}

//R0015 B0364
void	AView_Text::CopyWithHTMLColor()
{
	AText	text;
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	spt.x = 0;
	if( ept.x != 0 )
	{
		ept.y++;
		ept.x = 0;
	}
	SetSelect(spt,ept);
	text.AddCstring("<pre class=\"\"><code>");
	for( AIndex lineIndex = spt.y; lineIndex < ept.y; lineIndex++ )
	{
		CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
		ATextPoint	tmp1, tmp2;
		AAnalyzeDrawData	tmp3;//RC1
		GetTextDocumentConst().SPI_GetTextDrawDataWithStyle(lineIndex,textDrawData,false,tmp1,tmp2,tmp3,GetObjectID());
		AText	t;
		t.SetText(textDrawData.UTF16DrawText);
		t.ConvertToUTF8FromUTF16();
		AIndex	unicodeCharIndex = 0;
		ABool	first = true;
		for( AIndex i = 0; i < t.GetItemCount(); i = t.GetNextCharPos(i) )
		{
			AIndex	attrIndex = kIndex_Invalid;
			for( AIndex j = 0; j < textDrawData.attrPos.GetItemCount(); j++ )
			{
				if( textDrawData.attrPos.Get(j) == unicodeCharIndex )
				{
					attrIndex = j;
				}
			}
			if( attrIndex != kIndex_Invalid )
			{
				if( first == false )
				{
					AText	addtext;
					addtext.SetCstring("</span>");
					t.InsertText(i,addtext);
					i += addtext.GetItemCount();
				}
				AText	colortext;
				AColorWrapper::GetHTMLFormatColorText(textDrawData.attrColor.Get(attrIndex),colortext);
				AText	addtext;
				addtext.SetCstring("<span style=\"color:#");
				addtext.AddText(colortext);
				addtext.AddCstring("\">");
				t.InsertText(i,addtext);
				i += addtext.GetItemCount();
				first = false;
			}
			//<>����������
			AUChar	ch = t.Get(i);
			if( ch == '<' )
			{
				AText	ht;
				ht.SetCstring("&lt;");
				t.Delete1(i);
				t.InsertText(i,ht);
				i += ht.GetItemCount()-1;
			}
			else if( ch == '>' )
			{
				AText	ht;
				ht.SetCstring("&gt;");
				t.Delete1(i);
				t.InsertText(i,ht);
				i += ht.GetItemCount()-1;
			}
			else if( ch == '&' )
			{
				AText	ht;
				ht.SetCstring("&amp;");
				t.Delete1(i);
				t.InsertText(i,ht);
				i += ht.GetItemCount()-1;
			}
			//
			unicodeCharIndex++;
		}
		if( first == false )
		{
			AText	addtext;
			addtext.SetCstring("</span>");
			t.AddText(addtext);
		}
		text.AddText(t);
		text.Add(kUChar_LineEnd);
	}
	text.AddCstring("</code></pre>");
	AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
}

void	AView_Text::Paste()
{
	//#800
	//��`���[�h���Ȃ��`�y�[�X�g����
	if( mKukeiSelectionMode == true )
	{
		PasteKukei();
		return;
	}
	//
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	AText	text;
	//#878
	//FIFO�N���b�v���[�h�Ȃ�f�L���[���āA�N���b�v�{�[�h�ɐݒ�
	if( GetApp().SPI_IsFIFOClipboardMode() == true )
	{
		DequeueFIFOClipboard(text);
	}
	else
	{
		//�N���b�v�{�[�h����e�L�X�g�擾
		AScrapWrapper::GetClipboardTextScrap(text);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	}
	ATextPoint	spt1 = {0,0}, ept1 = {0,0};//#646
	SPI_GetSelect(spt1,ept1);//#646
	TextInput(undoTag_Paste,text);
	//�s�܂�Ԃ��X���b�h���쒆����
	if( GetTextDocumentConst().SPI_IsWrapCalculating() == false )//#699 �s�܂�Ԃ��X���b�h���삵���ꍇ�́A�y�[�X�g���̎����C���f���g�͂��Ȃ�
	{
		//�����C���f���gON�̏ꍇ�̂݃y�[�X�g�������C���f���g���s
		if( GetApp().SPI_GetAutoIndentMode() == true )
		{
			//#646
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kIndentWhenPaste) == true )
			{
				//�C���f���g���s
				ATextPoint	spt2 = {0,0}, ept2 = {0,0};
				SPI_GetSelect(spt2,ept2);
				ATextPoint	spt = spt1, ept = ept2;
				ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);
				DoIndent(undoTag_NOP,spt,ept,inputSpaces,kIndentTriggerType_IndentFeature);
				if( ept.y >= GetTextDocumentConst().SPI_GetLineCount() )   ept.y = GetTextDocumentConst().SPI_GetLineCount()-1;
				SetSelect(ept,ept);
			}
		}
	}
	//#905
	//���@�F���ƃ��[�J���͈͐ݒ�
	TriggerSyntaxRecognize();
}

void	AView_Text::PasteKukei()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	AText	text;
	AScrapWrapper::GetClipboardTextScrap(text);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Paste);
	//��`����
	KukeiInput(text);
	//#905
	//���@�F���ƃ��[�J���͈͐ݒ�
	TriggerSyntaxRecognize();
}

/**
�N���b�v�{�[�hFIFO���[�h�ؑ�
*/
void	AView_Text::SwitchFIFOClipboardMode()
{
	//FIFO���[�h�ؑ�
	GetApp().SPI_SetFIFIOClipboardMode(!(GetApp().SPI_IsFIFOClipboardMode()));
	
	//FIFO���[�hON�ɕύX�����ꍇ�A�ŏ��Ɍ��݂̃N���b�v�{�[�h�̓��e��FIFO�ɐς�
	if( GetApp().SPI_IsFIFOClipboardMode() == true )
	{
		//�e�L�X�g���N���b�v�{�[�h����擾
		AText	text;
		AScrapWrapper::GetClipboardTextScrap(text);
		if( text.GetItemCount() > 0 )
		{
			//�ŏ��ɃL���[�ɒǉ�
			EnqueueFIFOClipboard(text);
		}
	}
}

//#929
/**
�e�i���̍Ō�Ƀy�[�X�g
*/
void	AView_Text::PasteAtEndOfEachLine()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	//�N���b�v�{�[�h�e�L�X�g�擾
	AText	text;
	AScrapWrapper::GetClipboardTextScrap(text);
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Paste);
	//
	AText	lineText;
	AIndex	pos = 0;
	//�J�n�i���擾
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetSelect(spt,ept);
	AIndex	paraIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y);
	//�e�i�����ƃ��[�v
	AItemCount	paraCount = GetTextDocumentConst().SPI_GetParagraphCount();
	for( ; paraIndex < paraCount; paraIndex++ )
	{
		//�y�[�X�g�e�L�X�g�S�ăy�[�X�g�ς݂Ȃ�I��
		if( pos >= text.GetItemCount() )
		{
			break;
		}
		//�y�[�X�g�e�L�X�g����i���擾
		text.GetLine(pos,lineText);
		//�i���Ō�ɑ}��
		AIndex	paraStartLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(paraIndex);
		AIndex	paraEndLineIndex = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(paraStartLineIndex) -1;
		AIndex	textIndex = GetTextDocumentConst().SPI_GetLineStart(paraEndLineIndex) +
				GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(paraEndLineIndex);
		GetTextDocument().SPI_InsertText(textIndex,lineText);
	}
	//���@�F���ƃ��[�J���͈͐ݒ�
	TriggerSyntaxRecognize();
}

void	AView_Text::InputTab( const ABool inInputSpaces )//#249
{
	//#249 if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == false )
	if( inInputSpaces == false )
	{
		AText	text;
		text.Add(kUChar_Tab);
		TextInput(undoTag_Typing,text);
	}
	else
	{
		CTextDrawData	textdrawdata(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
		GetTextDocumentConst().SPI_GetTextDrawDataWithoutStyle(GetCaretTextPoint().y,textdrawdata);
		AItemCount	tabletters = textdrawdata.OriginalTextArray_TabLetters.Get(GetCaretTextPoint().x);
		AText	text;
		text.Add(kUChar_Space);
		tabletters++;
		while( (tabletters%GetTextDocumentConst().SPI_GetTabWidth()) != 0 )
		{
			text.Add(kUChar_Space);
			tabletters++;
		}
		//#318
		if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == true &&
					GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kZenkakuSpaceIndent) == true )
		{
			//
			AText	zenkakuSpaceText;
			zenkakuSpaceText.SetZenkakuSpace();
			//
			AText	t;
			//
			AItemCount	zenkakuSpaceCount = text.GetItemCount()/2;
			AItemCount	spaceCount = text.GetItemCount()-zenkakuSpaceCount*2;
			for( AIndex i = 0; i < zenkakuSpaceCount; i++ )
			{
				t.AddText(zenkakuSpaceText);
			}
			for( AIndex i = 0; i < spaceCount; i++ )
			{
				t.Add(kUChar_Space);
			}
			//
			TextInput(undoTag_Typing,t);
		}
		else
		{
			TextInput(undoTag_Typing,text);
		}
	}
}

/*
B0381
�^�u�E�C���f���g�d�l�E�݌v�܂Ƃ�
2008.1.1

�y�ݒ�z
�ȉ��̂Q�̃��[�h�ݒ肪����B
�@���K�\���C���f���g�L��
�A���@�C���f���gEnable/Disable�i�{���@��`�g�pEnable/Disable�j
���ӓ_
�E�A�̐ݒ肪Enable�̏ꍇ�́A�����炪�D�悳��A�@�͖����ƂȂ�B
�EADocument_Text::SPI_GetIndentAvailable()�́A�@���A���L���̏ꍇtrue�ƂȂ�B
�ȉ��A�u�C���f���g�ݒ薳���v�Ƃ́A�@���A�������ł��邱�Ƃ������B

�y�C���f���g�̎d�l�z
�u�C���f���g�v�̌v�Z���@(DoIndent(), SPI_CalcIndentCount())
�@A ���@�C���f���gEnable�̏ꍇ
�@�@�K���Ȓi���iGlobal���ʎq�����݂���i���j�܂Ŗ߂��Ă������當�@��`�ɏ]���ăC���f���g�v�Z
�@B ���@�C���f���gDisable�̏ꍇ
�@�@B-1 ���K�\���C���f���g�L��̏ꍇ
�@�@�@�O�̒i���̌��݂̃C���f���g�ʂ���Ƃ��āA�O�̒i���ƌ��ݒi���̓��e�����ꂼ�ꐳ�K�\���Ɣ�r���āA�C���f���g�v�Z
�@�@B-2 �C���f���g�ݒ薳��
�@�@�@���ݒi���̃C���f���g��0�̏ꍇ�̂݁A�O�̒i���̃C���f���g�ʂɍ��킹��

��SPI_CalcIndentCount()�̈���inUseCurrentParagraphIndentForRegExp��true�ɂ����ꍇ�́A���ݒi���̌��݂̃C���f���g����Ɍv�Z���邪�A
�g�p���Ȃ����Ƃɂ����B�i�C���f���g���^�[���̏ꍇ�̉��s��}�������i���ɂ��āA�����K�p���邱�Ƃɂ��A�t���[�ȃC���f���g�Ƃ̋�����}�낤�Ƃ������A
���K�\���C���f���g�̋@�\�Ƃ��܂��K�����Ȃ��B

�y�e�@�\�̎d�l�z
�^�u�E�C���f���g�֘A�e�@�\�̎d�l
�@�c�[���R�}���hINDENT-TAB�A�L�[�o�C���h����keyAction_indenttab
�@A �C���f���g�ݒ薳���̏ꍇ
�@�@��Ɂu�^�u�v�}��
�@B �C���f���g�ݒ�L��̏ꍇ
�@�@B-1 �L�����b�g���[�h�̏ꍇ
�@�@�@�u�^�u�v�}��
�@�@B-2 �L�����b�g���[�h�ȊO�̏ꍇ
�@�@�@B-2-1 �i���̓r���̏ꍇ
�@�@�@�@�u�^�u�v�}��
�@�@�@B-2-2 �i���̂͂��߂̏ꍇ
�@�@�@�@�C���f���g�ʂ��{���̃C���f���g�ʂ������Ȃ���΃C���f���g�A�����łȂ���΁u�^�u�v�}��
�v����ɁA�u�C���f���g�ݒ肪����A���A�L�����b�g���{���̃C���f���g�������ɂ���΃C���f���g�A����ȊO�́A�^�u�v

�A�c�[���R�}���hAUTOTAB
�ݒ�Ɋւ�炸��ɑO�̒i���̃C���f���g�ʂɍ��킹��

�B�c�[���R�}���hINDENT�A���j���[����C���f���g�A���@�C���f���g���s�����̓���
�C���f���g

�C�c�[���R�}���hINDENT-RETURN�A�L�[�o�C���h����keyAction_indentreturn
���s�}���{���s�}���s�Ǝ��s�̃C���f���g

*/
//�c�[���R�}���hINDENT-TAB�i�^�u�L�[�̃f�t�H���g�L�[�o�C���h����j
void	AView_Text::IndentTab( const ABool inInputSpacesForIndent, const ABool inInputSpacesForTab )//#249
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	/*#912 kIndentTabIsAlwaysTabIfIndentOff�͍폜�BV3�ł̓C���f���g�^�C�v���u�����C���f���g���Ȃ��v�ɐݒ肷�邱�ƂŁA
	�u�C���f���g�^�^�u�v�L�[����Ƀ^�u����Ƃ��邱�Ƃ��ł���B�i�����ɂ͍s���̏ꍇ�͏�Ɂu�E�V�t�g�v����ƂȂ�B�j
	//#281 AModePrefDB::kIndentTabIsAlwaysTabIfIndentOff��true�̏ꍇ�́u�C���f���g�^�^�u�v�͏�Ƀ^�u����Ƃ���
	//�i#255�ŃR�����g�A�E�g�������A��������Ɠ��삪�ς���Ă��܂��̂ŁA�ݒ�ɂ��ύX�ł���悤�ɂ����B�f�t�H���g�͊����i2.1.8�Ɠ����j����j
	if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kIndentTabIsAlwaysTabIfIndentOff) == true )
	{
		//B0381 ���@�C���f���g�����K�\���C���f���g�������̏ꍇ�́A�^�u
		//�R�����g�F2.1.13b2�ɂă��[�h�ݒ�̃��C�A�E�g�ύX���s�������A���K�\���C���f���g���g�p����ꍇ�Ő��K�\���ݒ肪�����Ƃ����A
		//���L�̑ΏۂƂȂ�̂ŁA���C�A�E�g�ǂ���̓���ɂȂ��Ă��Ȃ��B�����A�]������ƕς������Ȃ��Ƃ������Ƃ�����̂ŁA
		//�Ƃ肠��������ʂ�ɂ��Ă����B
		if( GetTextDocument().SPI_GetIndentAvailable() == false )
		{
			InputTab(inInputSpacesForTab);//#249
			return;
		}
	}
	*/
	if( IsCaretMode() == false )
	{
		//�y�I�𒆂̏ꍇ�z�^�u
		InputTab(inInputSpacesForTab);//#249
	}
	else
	{
		//�y�L�����b�g�\�����̏ꍇ�z�s���Ȃ�΃C���f���g�A�s���ȊO�̏ꍇ�̓^�u
		ATextIndex	pos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
		if(	GetIndentStart(GetCaretTextPoint()) < pos || 
			GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(GetCaretTextPoint().y) != GetCaretTextPoint().y )
		{
			//�y�s���ł͂Ȃ��ꍇ�z�^�u
			InputTab(inInputSpacesForTab);//#249
		}
		else
		{
			Indent(inInputSpacesForIndent);//#255
		}
	}
}

//#255
/**
�^�u�L�[���ɂ��C���f���g
*/
void	AView_Text::Indent( const ABool inInputSpacesForIndent )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	/*B0381 if( inAlwaysIndentIfLineStart == false )
	{*/
	//�y�s���̏ꍇ�z�L�����b�g�ʒu���C���f���g�ʒu��荶�Ȃ�C���f���g�A����ȏ�Ȃ�E�V�t�g(#255)
	CTextDrawData	textdrawdata(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	GetTextDocumentConst().SPI_GetTextDrawDataWithoutStyle(GetCaretTextPoint().y,textdrawdata);
	AItemCount	tabletters = textdrawdata.OriginalTextArray_TabLetters.Get(GetCaretTextPoint().x);
	AItemCount	tmp;
	if( tabletters < GetTextDocument().SPI_CalcIndentCount(GetCaretTextPoint().y,kIndex_Invalid,tmp,/*#650false*/kIndentTriggerType_IndentKey) &&
				GetApp().SPI_GetAutoIndentMode() == true )//#638
	{
		DoIndent(/*#650 false,*/undoTag_Indent,inInputSpacesForIndent,kIndentTriggerType_IndentKey);//#249
	}
	else
	{
		//#255 InputTab(inInputSpacesForTab);//#249
		ShiftRightLeft(true,inInputSpacesForIndent, true);//#255 #1188
	}
	/*B0381 }
	else
	{
	DoIndent(false);
	}*/
	//�C���f���g�ʒu�փL�����b�g�ړ�
	AIndex pos = GetIndentStart(GetCaretTextPoint());
	ATextPoint	textpoint;
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(pos,textpoint);
	SetCaretTextPoint(textpoint);
}

//B0381
//�c�[���R�}���hAUTOTAB�i��ɑO�̍s�̃C���f���g�ʂɍ��킹��j�ςȋ@�\�����C���f���g���s����Ȃ�INDENT������̂ŁA�ߋ��̓���ɍ��킹��
void	AView_Text::AutoTab( const ABool inInputSpaces )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	AIndex	currentLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(
			GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(GetCaretTextPoint().y));
	AItemCount	indentCount = 0;
	if( currentLineIndex > 0 )
	{
		AIndex	prevLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(
				GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(currentLineIndex-1));
		indentCount = GetTextDocumentConst().SPI_GetCurrentIndentCount(prevLineIndex);
	}
	AItemCount	insertedCount = GetTextDocument().SPI_Indent(currentLineIndex,indentCount,inInputSpaces,false);//#249
	ATextPoint	pt = GetCaretTextPoint();
	pt.x += insertedCount;
	if( pt.x < 0 )   pt.x = 0;
	if( pt.x > GetTextDocument().SPI_GetLineLengthWithoutLineEnd(pt.y) )   pt.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(pt.y);
	SetCaretTextPoint(pt);
}

void	AView_Text::InputCorrespondence()
{
	if( GetTextDocumentConst().NVI_IsReadOnly() == true )   return;
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	ATextPoint	hitspt, hitept;//R0000
	AText	text;
	if( GetTextDocumentConst().SPI_GetCorrespondText(spt,text,hitspt,hitept) == true )
	{
		TextInput(undoTag_InputCorrespond,text);
		//R0000
		/*NVI_GetOriginOfLocalFrame(mBraceCheckSavedOrigin);
		mBraceCheckSavedCaretTextPoint = GetCaretTextPoint();
		SetSelect(hitspt,hitept);
		AdjustScroll_Center(hitspt);
		mBraceCheckStartTickCount = 50;
		mDisplayingBraceCheck = true;*/
		AText	line, para;
		line.SetFormattedCstring("%d",hitspt.y+1);
		para.SetFormattedCstring("%d",GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(hitspt.y)+1);
		AText	infotext;
		infotext.SetLocalizedText("Correspnd_StartLine");
		infotext.ReplaceParamText('0',line);
		infotext.ReplaceParamText('1',para);
		AText	notificationTitle;
		notificationTitle.SetLocalizedText("Notification_Correspond");
		if( mTextWindowID != kObjectID_Invalid )
		{
			SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
					SPI_SetNotification_General(notificationTitle,infotext,GetEmptyText());
			SPI_ShowNotificationPopupWindow(true);
		}
		
		//�C���f���g���s�����ɂ�鎩���C���f���g���s
		//�i�C���f���g�^�C�v�����@��`�C���f���g�A���K�\���C���f���g�̏ꍇ�̂݁j
		//#213 �Ή���������͎��A�C���f���g���s�������܂܂�Ă�����A�C���f���g���s
		if( (GetTextDocumentConst().SPI_GetIndentType() == kIndentType_SDFIndent || 
				GetTextDocumentConst().SPI_GetIndentType() == kIndentType_RegExpIndent ) && 
				GetApp().SPI_GetAutoIndentMode() == true )//#638
		{
			AText	indentletters;
			GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Text(AModePrefDB::kIndentExeLetters,indentletters);
			for( AIndex i = 0; i < indentletters.GetItemCount(); i++ )
			{
				if( text.IsCharContained(indentletters.Get(i)) == true )
				{
					ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
					DoIndent(/*#650 false,*/undoTag_NOP,inputSpaces,kIndentTriggerType_AutoIndentCharacter);//#249 #639 #650 ���K�\����v�̏ꍇ�̂݃C���f���g�K�p
					break;
				}
			}
		}
	}
}

//#359
/**
*/
void	AView_Text::SelectBetweenCorrespondTexts()
{
	ATextPoint	cspt, cept;
	SPI_GetSelect(cspt,cept);
	ATextPoint	spt,ept;
	if( GetTextDocumentConst().SPI_GetCorrespondenceSelection(cspt,cept,spt,ept) == false )
	{
		//#419 �Ή����Ƃ�Ă��Ȃ��ꍇ
		ASoundWrapper::Beep();
	}
	SetSelect(spt,ept);
}

void	AView_Text::SameLetterUp()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( spt.y == 0 )   return;
	ATextPoint	pt;
	pt.x = spt.x;
	pt.y = spt.y-1;
	ATextIndex	pos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(pt);
	AText	ch;
	GetTextDocumentConst().SPI_GetText1CharText(pos,ch);
	TextInput(undoTag_Typing,ch);
}

void	AView_Text::DuplicateLine()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	AIndex	currentpara = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y);
	ATextIndex	spos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(currentpara);
	ATextIndex	epos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(currentpara+1);
	AText	text;
	GetTextDocumentConst().SPI_GetText(spos,epos,text);
	ATextIndex	pos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(currentpara);
	ATextPoint	textpoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(pos,textpoint);
	SetCaretTextPoint(textpoint);
	TextInput(undoTag_Typing,text);
}

void	AView_Text::InsertEmptyLine()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	AIndex	currentpara = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y);
	ATextIndex	pos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(currentpara);
	ATextPoint	textpoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(pos,textpoint);
	SetCaretTextPoint(textpoint);
	AText	text;
	text.Add(kUChar_LineEnd);
	TextInput(undoTag_Typing,text);
}

#if 0
//RC2
//���ʎq���E�C���h�E��inKeyword�ɑΉ����鎯�ʎq�̏���ݒ�
void	AView_Text::SetIdInfo( const AText& inKeyword )
{
	AImagePoint	ipt;
	GetImagePointFromTextPoint(GetCaretTextPoint(),ipt);
	ALocalPoint	lpt;
	NVM_GetLocalPointFromImagePoint(ipt,lpt);
	lpt.x = 16;
	AGlobalPoint	gpt;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lpt,gpt);
	ARect	rect = {gpt.x+500,gpt.y-200,gpt.x+800,gpt.y-30};
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	AGlobalRect	grect = {0};
	NVM_GetWindow().NVI_GetGlobalRectFromControlLocalRect(NVI_GetControlID(),frameRect,grect);
	rect.right = grect.right - 5;
	rect.left = rect.right - 300;
	GetApp().SPI_GetPopupIdInfoWindow().NVI_SetWindowBounds(rect);
	GetApp().SPI_GetPopupIdInfoWindow().NVI_Show(false);
	
	//#725if( mTextWindowID != kObjectID_Invalid )
	{
		//#725 GetApp().SPI_GetIdInfoView().SPI_SetIdInfo(inKeyword,/*#695 mTextDocumentID*/mTextDocumentRef.GetObjectID());
		ABool	idExist = GetApp().SPI_UpdateIdInfoWindows(inKeyword,mTextDocumentRef.GetObjectID(),mTextWindowID);//#725
		if( idExist == false )
		{
			GetApp().SPI_GetPopupIdInfoWindow().NVI_Hide();
		}
	}
}
#endif

//
void	AView_Text::SelectBrace()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	ATextPoint	origspt = spt;
	ATextPoint	origept = ept;
	if( GetTextDocumentConst().SPI_GetBraceSelection(spt,ept) == true )
	{
		//Brace���v�Z�����݂̑I��͈͂Ɠ����ꍇ�͂P�����L���čēx���s
		if( spt.x == origspt.x && spt.y == origspt.y && ept.x == origept.x && ept.y == origept.y )
		{
			ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
			ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ept);
			if( spos > 0 )   spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
			if( epos < GetTextDocumentConst().SPI_GetTextLength()-1 )   epos = GetTextDocumentConst().SPI_GetNextCharTextIndex(epos);
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,spt);
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,ept);
			if( GetTextDocumentConst().SPI_GetBraceSelection(spt,ept) == true )
			{
				SetSelect(spt,ept);
			}
		}
		else
		{
			SetSelect(spt,ept);
		}
	}
}

//
ABool	AView_Text::DoubleClickBrace( ATextPoint& ioStart, ATextPoint& ioEnd )//#688
{
	//#844 if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSelectInsideBraceByDoubleClick) == false )   return false; 
	ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ioStart);
	ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ioEnd);
	if( epos-spos != 1 )   return false;
	if( GetTextDocumentConst().SPI_IsCodeChar(ioStart) == false )   return false;
	AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(spos);
	ATextPoint	spt, ept;
	if( ch == '{' || ch == '(' || ch == '[' )
		{
			spt = ioEnd;
		}
	else if( ch == '}' || ch == ')' || ch == ']' )
	{
		spt = ioStart;
	}
	else return false;
	ept = spt;
	GetTextDocumentConst().SPI_GetBraceSelection(spt,ept);
	//#1357
	//���ʂ��܂ސݒ�ON�Ȃ�A���ʂ��܂߂�
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectBraceItselfByDoubleClick) == true )
	{
		ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
		ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ept);
		spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
		epos = GetTextDocumentConst().SPI_GetNextCharTextIndex(epos);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,spt,false);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,ept,false);
	}
	//
	ioStart = spt;
	ioEnd = ept;
	return true;
}

//#1406
/**
���ʃn�C���C�g
*/
void	AView_Text::HighlightBrace()
{
	//���[�h�ݒ�OFF�Ȃ�n�C���C�g�������ďI��
	if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kHighlightBrace) == false )
	{
		SetHighlightBraceVisible(false);
		return;
	}
	
	ATextPoint	spt = {0}, ept = {0};
	
	//�L�����b�g�ʒu�̑O�̕����i�����ʕ����ʒu�j���擾
	ATextIndex	caretPos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
	ATextIndex	kakkoPos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(caretPos);
	if( kakkoPos < 0 || kakkoPos >= GetTextDocumentConst().SPI_GetTextLength() )
	{
		SetHighlightBraceVisible(false);
		return;
	}
	AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(kakkoPos);
	//���ʊJ�n�����Ȃ�spt, ept���L�����b�g�ʒu�ɐݒ�
	if( ch == '{' || ch == '(' || ch == '[' )
	{
		spt = ept = GetTextDocumentConst().SPI_GetTextPointFromTextIndex(caretPos);
	}
	//���ʏI�������Ȃ�spt, ept�����ʕ����ʒu�ɐݒ�
	else if( ch == '}' || ch == ')' || ch == ']' )
	{
		spt = ept = GetTextDocumentConst().SPI_GetTextPointFromTextIndex(kakkoPos);
	}
	//����ȊO�̕����Ȃ�n�C���C�g�������ďI��
	else
	{
		SetHighlightBraceVisible(false);
		return;
	}
	//���ʕ����ʒu���R�[�h�łȂ���΃n�C���C�g�������ďI��
	ATextPoint	kakkoPoint = GetTextDocumentConst().SPI_GetTextPointFromTextIndex(kakkoPos);
	if( GetTextDocumentConst().SPI_IsCodeChar(kakkoPoint) == false )
	{
		SetHighlightBraceVisible(false);
		return;
	}
	
	//���ʔ͈͎擾
	if( GetTextDocumentConst().SPI_GetBraceSelection(spt, ept) == true )
	{
		//�J�n���ʃn�C���C�g�͈͐ݒ�
		mBraceHighlightStartTextPoint1 = GetTextDocumentConst().SPI_GetPrevCharTextPoint(spt);
		mBraceHighlightEndTextPoint1 = spt;
		//�I�����ʃn�C���C�g�͈͐ݒ�
		mBraceHighlightStartTextPoint2 = ept;
		mBraceHighlightEndTextPoint2 = GetTextDocumentConst().SPI_GetNextCharTextPoint(ept);
		//�n�C���C�g�\��
		SetHighlightBraceVisible(true);
		return;
	}
	//���ʔ͈͂��擾�ł��Ȃ��Ƃ��A�o�����X���Ƃ�Ă��Ȃ��Ƃ����̓n�C���C�g�������ďI��
	SetHighlightBraceVisible(false);
}

//#1406
/**
���ʃn�C���C�g��\���E��������
*/
void	AView_Text::SetHighlightBraceVisible( const ABool inVisible )
{
	if( mBraceHighlighted != inVisible )
	{
		mBraceHighlighted = inVisible;
		//�n�C���C�g����i���Ă����j�s���ĕ`��
		SPI_RefreshLines(mBraceHighlightStartTextPoint1.y, mBraceHighlightEndTextPoint1.y);
		SPI_RefreshLines(mBraceHighlightStartTextPoint2.y, mBraceHighlightEndTextPoint2.y);
	}
}


//
void	AView_Text::EVTDO_DoTickTimer()
{
	//#606
	//��̂Ђ�c�[�����[�h�p�}�E�X�J�[�\���ݒ�
	if( IsHandToolMode() == true )
	{
		if( mMouseTrackingMode == kMouseTrackingMode_TenohiraTool )
		{
			//��̂Ђ�h���b�O��
			if( ACursorWrapper::GetCurrentCursorType() != kCursorType_ClosedHand )
			{
				ACursorWrapper::SetCursor(kCursorType_ClosedHand);
			}
		}
		else
		{
			//�ʏ펞
			//�R�}���h�L�[���N���b�N���Ď�̂Ђ�ɂȂ�̂���a��������̂ŁA�R�����g�A�E�g
			/*
			if( ACursorWrapper::GetCurrentCursorType() != kCursorType_OpenHand )
			{
				ACursorWrapper::SetCursor(kCursorType_OpenHand);
			}
			*/
		}
	}
	else
	{
		if( ACursorWrapper::GetCurrentCursorType() == kCursorType_OpenHand )
		{
			ACursorWrapper::SetCursor(kCursorType_Arrow);
		}
	}
	//Caret�\���\�񂪂���Ε\������
	//������DoDraw()�̍Ō�Ŏ��s���Ă������A���s���ɃL�����b�g�`��̂ق�����s���Č�����̂ŁAtick timer���A����сAtext input�C�x���g���s�̍ŏ��ōs���悤�ɂ����B
	ShowCaretIfReserved();
	//
	mCaretTickCount++;
	if( mCaretTickCount >= ATimerWrapper::GetCaretTime() )//#688 >����>=�֕ύX
	{
		if( NVI_IsFocusActive() == true )
		{
			//�L�����b�g�u�����N
			BlinkCaret();
		}
		else
		{
			//�t�H�[�J�X���Ȃ��Ƃ��̓L�����b�g��\�� #688
			HideCaret(false,false);
		}
	}
	if( mDisplayingBraceCheck == true )
	{
		mBraceCheckStartTickCount--;
		if( mBraceCheckStartTickCount == 0 )
		{
			RestoreFromBraceCheck();
		}
	}
//#688 #if IMPLEMENTATION_FOR_WINDOWS
	//win 080713
	//�X�N���[���h���b�O���Ƀ}�E�X���~�߂Ă��X�N���[������悤�ɂ���
	if( mMouseTrackingMode == kMouseTrackingMode_SingleClick ||
				mMouseTrackingMode == kMouseTrackingMode_DoubleClick || //#688
				mMouseTrackingMode == kMouseTrackingMode_TripleClick || //#688
				mMouseTrackingMode == kMouseTrackingMode_QuadClick )//#688
	{
		DoMouseTrack(mCurrentMouseTrackModifierKeys);
	}
	//#688 #endif
	
	//
	if( mKeyIdleWorkTimer > 0 )
	{
		//
		mKeyIdleWorkTimer--;
		//
		if( mKeyIdleWorkTimer == 0 )
		{
			//
			if( (mKeyIdleWorkType&kKeyIdleWorkType_UpdateLocalRange) != 0 )
			{
				GetTextDocument().SPI_SetCurrentLocalTextPoint(GetCaretTextPoint(),GetObjectID());
			}
			//�}�E�X�N���b�N��ɂ��^�C�}�[�o�ߏ���
			if( (mKeyIdleWorkType&kKeyIdleWorkType_MouseClick) != 0 )
			{
				//�L�[���[�h���
				DisplayIdInfo(false,kIndex_Invalid,kIndex_Invalid);
				//���[�Y���X�g�֌����v��
				RequestWordsList();
			}
			//���ݒP��n�C���C�g�ݒ�
			SetCurrentWordHighlight();
			//#905
			//���@�F���ƃ��[�J���͈͐ݒ�
			TriggerSyntaxRecognize();
		}
	}
	else
	{
		//
		mKeyIdleWorkType = kKeyIdleWorkType_None;
		mKeyIdleWorkTimer = 0;
	}
	
	//���A���^�C���⊮���\��
	if( mCandidateRequestTimer > 0 )
	{
		//�J�E���g�_�E��
		mCandidateRequestTimer--;
		//0�Ȃ���s
		if( mCandidateRequestTimer == 0 )
		{
			//�⊮���v��
			FindCandidateRealtime();
		}
	}
	
	//#81
	//��1�����Ƃɖ��ۑ��f�[�^�`�F�b�N
	mAutoSaveUnsavedCheckTickCount++;
	if( mAutoSaveUnsavedCheckTickCount > 1030 )
	{
		GetTextDocument().SPI_AutoSave_Unsaved();
		mAutoSaveUnsavedCheckTickCount = 0;
	}
}

//#867
/**
*/
void	AView_Text::TriggerKeyIdleWork( const AKeyIdleWorkType inWorkType, const ANumber inTimerValue )
{
	mKeyIdleWorkType |= inWorkType;
	mKeyIdleWorkTimer = inTimerValue;
}

/**
*/
void	AView_Text::ResetKeyIdleWorkTimer( const ANumber inTimerValue )
{
	if( mKeyIdleWorkTimer > 0 )
	{
		mKeyIdleWorkTimer = inTimerValue;
	}
}

//
void	AView_Text::RestoreFromBraceCheck()
{
	if( mDisplayingBraceCheck == true )
	{
		SetCaretTextPoint(mBraceCheckSavedCaretTextPoint);
		NVI_ScrollTo(mBraceCheckSavedOrigin);
		mDisplayingBraceCheck = false;
	}
}

//�h�L�������g�փe�L�X�g�}��
void	AView_Text::DeleteAndInsertTextToDocument( const AText& inText )
{
	if( IsCaretMode() == false )
	{
		DeleteTextFromDocument();
	}
	ATextPoint	insertionPoint = GetCaretTextPoint();
	//�e�L�X�g�}��
	InsertTextToDocument(insertionPoint,inText);
}

//�h�L�������g�փe�L�X�g�}��
void	AView_Text::InsertTextToDocument( const ATextIndex inInsertIndex, const AText& inText, const ABool inDontRedraw )//#846
{
	ATextPoint	insertPoint = {0,0};
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inInsertIndex,insertPoint);
	InsertTextToDocument(insertPoint,inText,inDontRedraw);
}
void	AView_Text::InsertTextToDocument( const ATextPoint& inInsertPoint, const AText& inText, const ABool inDontRedraw )//#846
{
	if( GetTextDocumentConst().NVI_IsReadOnly() == true )   return;
	//
	//�L�����b�g��������
	
	//
	ATextIndex	insertIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(inInsertPoint);//#695
	//�e�L�X�g�}���ʒu�Ƀq���g�e�L�X�g������΍폜����
	//�������A�⊮���͒��̏ꍇ�́A�폜���Ȃ��i�q���g�e�L�X�g���A�����Ă���΂����ɁA�⊮�̎��̌��ɕϊ����鎞�ɁA
	//���̃q���g�e�L�X�g���폜�����̂�h�����߁B�⊮���̓L�[���������Ƃ���InputAbbrev()���ň�xRemoveHintText()�����s�ς݁j
	if( mAbbrevInputMode == false )
	{
		RemoveHintText(insertIndex);
	}
	//
	AText	insertText(inText);
	/*
	//�������������[�h
	ABool	wakachiMode = true;
	if( wakachiMode == true )
	{
		if( insertIndex > 0 && insertText.GetItemCount() > 0 )
		{
			AIndex	prevCharIndex = GetTextDocumentConst().SPI_GetPrevCharTextIndex(insertIndex);
			AUCS4Char	prevChar = GetTextDocumentConst().SPI_GetText1UCS4Char(prevCharIndex);
			AUCS4Char	firstChar = 0;
			insertText.Convert1CharToUCS4(0,firstChar);
			if( AApplication::GetApplication().NVI_GetUnicodeData().IsWideWidth(prevChar) == true && 
				AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabet(firstChar) == true )
			{
				insertText.InsertCstring(0," ");
			}
		}
		if( insertIndex < GetTextDocumentConst().SPI_GetTextLength() && insertText.GetItemCount() > 0 )
		{
			AIndex	nextCharIndex = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(insertIndex);
			AUCS4Char	nextChar = GetTextDocumentConst().SPI_GetText1UCS4Char(nextCharIndex);
			AUCS4Char	lastChar = 0;
			insertText.Convert1CharToUCS4(insertText.GetPrevCharPos(insertText.GetItemCount()-1),lastChar);
			if( AApplication::GetApplication().NVI_GetUnicodeData().IsWideWidth(nextChar) == true && 
				AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabet(lastChar) == true )
			{
				insertText.AddCstring(" ");
			}
		}
	}
	*/
	//
	AItemCount	insertedLineCount = GetTextDocument().SPI_InsertText(/*#695 inInsertIndex*/inInsertPoint,insertText);
	//�L�����b�g�ݒ�
	/*#695
	ATextPoint	pt;
	GetTextDocument().SPI_GetTextPointFromTextIndex(insertIndex+insertText.GetItemCount(),pt);
	SetCaretTextPoint(pt);
	*/
	//#846 �u�������Aredraw���Ȃ��悤�ɂ���
	if( inDontRedraw == false )
	{
		//�}���e�L�X�g�̒���ɃL�����b�g�ړ��i�������X���b�h�ōs�v�Z���́ASetSelect()���ňړ����U�[�u�����j#695
		SetSelect(insertIndex+insertText.GetItemCount(),insertIndex+insertText.GetItemCount());
		//
		AdjustScroll(GetCaretTextPoint());
	}
	//#567 LuaConsole �R�}���h���s���� #1170
	if( GetTextDocumentConst().SPI_GetLuaConsoleMode() == true && insertedLineCount > 0 )
	{
		AIndex	promptTextIndex = GetTextDocumentConst().SPI_GetLuaPromptTextIndex();
		if( insertIndex >= promptTextIndex )
		{
			AText	luaCommand;
			GetTextDocument().SPI_GetText(promptTextIndex,insertIndex+insertText.GetItemCount(),luaCommand);
			AText	errorText;
			if( GetApp().NVI_Lua_dostring(luaCommand,errorText) != 0 )
			{
				GetTextDocument().SPI_Lua_Output(errorText);
			}
			GetTextDocument().SPI_Lua_Prompt();
		}
	}
	
	if( insertedLineCount > 0 )
	{
		//�⊮���͏�ԉ��� #717
		ClearAbbrevInputMode(true,false);
	}
}

//�h�L�������g����e�L�X�g�폜
void	AView_Text::DeleteTextFromDocument( const ATextIndex inStart, const ATextIndex inEnd, const ABool inDontRedraw,
											const ABool inStoreToKillBuffer )
{
	if( GetTextDocumentConst().NVI_IsReadOnly() == true )   return;
	
	//#913
	//inStoreToKillBuffer��true�̏ꍇ�́A�L���o�b�t�@�[�ɕۑ�����
	if( inStoreToKillBuffer == true )
	{
		ATextPoint	spt = {0};
		GetTextDocument().SPI_GetTextPointFromTextIndex(inStart,spt);
		AText	deletedText;
		GetTextDocumentConst().SPI_GetText(inStart,inEnd,deletedText);
		GetApp().SPI_AddToKillBuffer(GetTextDocumentConst().GetObjectID(),spt,deletedText);
	}
	
	//
	GetTextDocument().SPI_DeleteText(inStart,inEnd,false,inDontRedraw);
	//#846 �u�������Aredraw���Ȃ��悤�ɂ���
	if( inDontRedraw == false )
	{
		//�L�����b�g�ݒ�
		ATextPoint	pt;
		GetTextDocument().SPI_GetTextPointFromTextIndex(inStart,pt);
		SetCaretTextPoint(pt);
		//
		AdjustScroll(GetCaretTextPoint());
	}
	
}
void	AView_Text::DeleteTextFromDocument( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inDontRedraw,
										   const ABool inStoreToKillBuffer )
{
	DeleteTextFromDocument(GetTextDocument().SPI_GetTextIndexFromTextPoint(inStartPoint),GetTextDocument().SPI_GetTextIndexFromTextPoint(inEndPoint),
			inDontRedraw,inStoreToKillBuffer);
}
//�h�L�������g����e�L�X�g�폜�i�I��͈́j
void	AView_Text::DeleteTextFromDocument( const ABool inStoreToKillBuffer )
{
	if( GetTextDocumentConst().NVI_IsReadOnly() == true )   return;
	if( mKukeiSelected == true )
	{
		for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
		{
			ATextPoint	spt = mKukeiSelected_Start.Get(i);
			ATextPoint	ept = mKukeiSelected_End.Get(i);
			AIndex	start = GetTextDocument().SPI_GetTextIndexFromTextPoint(spt);
			AIndex	end = GetTextDocument().SPI_GetTextIndexFromTextPoint(ept);
			GetTextDocument().SPI_DeleteText(start,end);
		}
		//�L�����b�g�ݒ�
		SetCaretTextPoint(GetCaretTextPoint());
		//
		AdjustScroll(GetCaretTextPoint());
	}
	else
	{
		ATextPoint	spt, ept;
		SPI_GetSelect(spt,ept);
		DeleteTextFromDocument(spt,ept,false,inStoreToKillBuffer);
	}
}

/**
�q���g�e�L�X�g������΍폜����
*/
void	AView_Text::RemoveHintText( const ATextIndex inTextIndex )
{
	if( GetTextDocumentConst().SPI_FindHintIndexFromTextIndex(inTextIndex) != kIndex_Invalid )
	{
		if( GetTextDocumentConst().SPI_GetTextChar(inTextIndex) == kHintTextChar )
		{
			GetTextDocument().SPI_DeleteText(inTextIndex,inTextIndex+1);
		}
	}
}

//Drag&Drop�����iDrag���j
//#1034 XOR�`�揈������CGContextFlush�����s����悤�ɂ����̂ŁAdrag���ɂ�����悤�ɂȂ����̂ŁA����������ꍇ�̂ݕ`�悷��悤�ɂ���
void	AView_Text::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )//#688
{
	//�\���t���[����Image���W�Ŏ擾
	AImageRect	frameImageRect;
	NVM_GetImageViewRect(frameImageRect);
	//
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	//scroll
	{
		//Windows���̏ꍇ�A�C�x���g�h���u���Ńh���b�O�X�N���[��������
		if( clickImagePoint.y < frameImageRect.top+kDragDropScrollRange && 
					frameImageRect.top > 0 )
		{
			//��񂾂��X�N���[�����A�����Ń��^�[��
			DragScrollV(clickImagePoint.y-(frameImageRect.top+kDragDropScrollRange),inModifierKeys);
			return;
		}
		else if( clickImagePoint.y > frameImageRect.bottom-kDragDropScrollRange && 
					frameImageRect.bottom < NVM_GetImageHeight() )
		{
			//��񂾂��X�N���[�����A�����Ń��^�[��
			DragScrollV(clickImagePoint.y-(frameImageRect.bottom-kDragDropScrollRange),inModifierKeys);
			return;
		}
	}
	//
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	if( IsInSelect(clickImagePoint,false) == false )
	{
		// -------------------- �I��͈͊O�փh���b�O�����ꍇ --------------------
		
		//�V�KDrag�L�����b�g���i�[�p�ϐ� #1034
		ATextPoint	newDragCaretTextPoint = {0}, newDragSelectTextPoint = {0};
		ABool	newDragWordDragDrop = false;
		
		//�P��u������Drag&Drop
		if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true && mKukeiSelected == false )
		{
			ATextIndex	clickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			ATextIndex	wordStart, wordEnd;
			GetTextDocument().SPI_FindWord(clickTextIndex,wordStart,wordEnd);
			if( wordStart != wordEnd )
			{
				//Drag�L�����b�g�ʒu�擾
				GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,newDragCaretTextPoint,true);
				GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,newDragSelectTextPoint,false);
				//�P��u���������[�h�ݒ�
				newDragWordDragDrop = true;
			}
			else
			{
				//Drag�L�����b�g�ʒu�擾
				newDragCaretTextPoint = clickTextPoint;
				newDragSelectTextPoint = mDragSelectTextPoint;
			}
		}
		//�ʏ�Drag&Drop
		else
		{
			//Drag�L�����b�g�ʒu�擾
			newDragCaretTextPoint = clickTextPoint;
			newDragSelectTextPoint = mDragSelectTextPoint;
		}
		//Drag�L�����b�g���ɍ��������邩�ǂ����̔��� #1034
		//����������΁A�`����s��
		if( newDragWordDragDrop != mDragWordDragDrop || 
			newDragCaretTextPoint.x != mDragCaretTextPoint.x || newDragCaretTextPoint.y != mDragCaretTextPoint.y ||
			newDragSelectTextPoint.x != mDragSelectTextPoint.x || newDragSelectTextPoint.y != mDragSelectTextPoint.y )
		{
			// -------------------- Drag�L�����b�g���� --------------------
			if( mDragDrawn == true )
			{
				//Erase
				XorDragCaret();
				mDragDrawn = false;
			}
			if( mDragWordDragDrop == true )
			{
				//Erase
				XorWordDragDrop();
				mDragWordDragDrop = false;
			}
			// -------------------- Drag�L�����b�g���ݒ� --------------------
			mDragWordDragDrop = newDragWordDragDrop;
			mDragCaretTextPoint = newDragCaretTextPoint;
			mDragSelectTextPoint = newDragSelectTextPoint;
			// -------------------- Drag�L�����b�g�`�� --------------------
			if( mDragWordDragDrop == true )
			{
				//Draw
				mDragWordDragDrop = false;
				XorWordDragDrop();
				mDragWordDragDrop = true;
			}
			else
			{
				//Draw
				mDragDrawn = false;
				XorDragCaret();
				mDragDrawn = true;
			}
		}
	}
	else
	{
		// -------------------- �I��͈͓��փh���b�O�����ꍇ --------------------
		//Drag�L�����b�g������
		if( mDragDrawn == true )
		{
			//Erase
			XorDragCaret();
			mDragDrawn = false;
		}
		if( mDragWordDragDrop == true )
		{
			//Erase
			XorWordDragDrop();
			mDragWordDragDrop = false;
		}
	}
	//#859
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_TabSelector) == true )
	{
		if( mTextWindowID != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsSubTextView(NVI_GetControlID()) == true )
			{
				//#688 ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
				outIsCopyOperation = true;
			}
		}
	}
	//
	else if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true ||
				AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true      ||//win 080709
				mDragging == false )//B0347
	{
		//#688 ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
		outIsCopyOperation = true;
	}
	else
	{
		//#688 ACursorWrapper::SetCursor(kCursorType_Arrow);
		outIsCopyOperation = false;
	}
}

//Drag&Drop�����iDrag��View�ɓ������j
void	AView_Text::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	GetTextPointFromImagePoint(clickImagePoint,mDragCaretTextPoint);
	if( IsInSelect(clickImagePoint,false) == false )
	{
		//Draw
		mDragDrawn = false;
		XorDragCaret();
		mDragDrawn = true;
	}
}

//Drag&Drop�����iDrag��View����o���j
void	AView_Text::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mDragDrawn == true )
	{
		//Erase
		XorDragCaret();
		mDragDrawn = false;
	}
	if( mDragWordDragDrop == true )
	{
		//Erase
		XorWordDragDrop();
		mDragWordDragDrop = false;
	}
}

//Drag&Drop�����iDrop�j
void	AView_Text::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
	
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	//DragCaret����
	if( mDragDrawn == true )
	{
		//Erase
		XorDragCaret();
		mDragDrawn = false;
	}
	if( mDragWordDragDrop == true )
	{
		//Erase
		XorWordDragDrop();
		mDragWordDragDrop = false;
	}
	if( IsInSelect(clickImagePoint,false) == true )   return;
	
	/*#182
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_FixInlineInput();//B0079
	}
	*/
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());//#688
	
	//URL Drop
	AText	url, title;
	if( AScrapWrapper::GetURLDragData(inDragRef,url,title) == true )
	{
		//DragCaret�ʒu���L�����b�g�ʒu�Ƃ��Đݒ�
		SetCaretTextPoint(clickTextPoint);
		//
		AText	text;
		AIndex	caretIndex = kIndex_Invalid;
		AFileAcc	tmp;
		MakeDropText(tmp,url,title,text,caretIndex);
		TextInput(undoTag_DragDrop,text);
		if( caretIndex != kIndex_Invalid )
		{
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			textindex += caretIndex;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textindex,clickTextPoint);
			SetCaretTextPoint(clickTextPoint);
		}
		return;
	}
	
	//#859
	//�^�u�����drag&drop
	AText	docUniqIDText;
	AScrapWrapper::GetDragData(inDragRef,kScrapType_TabSelector,docUniqIDText);//tab index�擾
	if( docUniqIDText.GetItemCount() > 0 && mTextWindowID != kObjectID_Invalid )
	{
		AUniqID	uniqID = kUniqID_Invalid;//#850
		uniqID.val = docUniqIDText.GetNumber();
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(uniqID);//#850
		//
		if( NVM_GetWindow().NVI_GetTabIndexByDocumentID(docID) == kIndex_Invalid )
		{
			//-----------------�h���b�O���̃h�L�������g�����̃E�C���h�E���ŕ\������Ă��Ȃ��ꍇ-----------------
			
			//�h���b�O���̃^�u�����̃E�C���h�E�ֈړ�����
			
			//�h���b�O���̃^�u�̃E�C���h�E�A�^�u���擾
			AWindowID	srcWindowID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			AIndex	srcTabIndex = GetApp().NVI_GetWindowByID(srcWindowID).NVI_GetTabIndexByDocumentID(docID);
			//�h���b�O��փ^�u�R�s�[
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPNVI_CopyCreateTab(srcWindowID, srcTabIndex);
		}
		else
		{
			//-----------------�h���b�O���̃h�L�������g�����̃E�C���h�E���ŕ\������Ă���ꍇ-----------------
			
			//�T�u�e�L�X�g�ւ�drop�̏ꍇ�̂ݏ�������B
			if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsSubTextView(NVI_GetControlID()) == true )
			{
				if( docID != kObjectID_Invalid )
				{
					//#850
					AIndex	tabIndex = NVM_GetWindow().NVI_GetTabIndexByDocumentID(docID);
					//�T�u�e�L�X�g�֕\��
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DisplayTabInSubText(tabIndex,false);
				}
			}
		}
	}
	
	/*IdInfo��ʃr���[�Ƀh���b�O�����炻�̃r���[�ŊJ���@�\���ЂƂ܂�drop
	//#853
	//�L�[���[�h��񂩂��drag&drop
	AText	openPosText;
	AScrapWrapper::GetDragData(inDragRef,kScrapType_IdInfoItem,openPosText);
	if( openPosText.GetItemCount() > 0 && mTextWindowID != kObjectID_Invalid )
	{
		ATextArray	ta;
		ta.ExplodeFromText(openPosText,kUChar_Tab);
		AText	filepath, sposText, eposText;
		ta.Get(0,filepath);
		ta.Get(1,sposText);
		ta.Get(2,eposText);
		ANumber	spos = sposText.GetNumber();
		ANumber	epos = eposText.GetNumber();
		GetApp().SPI_OpenOrRevealTextDocument(filepath,spos,epos,
					mTextWindowID,GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsSubTextView(NVI_GetControlID()),
					kAdjustScrollType_Top);
	}
	*/
	
	AText	text;
	AScrapWrapper::GetTextDragData(inDragRef,text);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding());//win
	if( text.GetItemCount() > 0 )//B0348
	{
		//
		if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true && mKukeiSelected == false )//B0347 if�u���b�N��������ւ��ishift�D��j
		{
			ATextIndex	clickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			ATextIndex	wordStart, wordEnd;
			GetTextDocument().SPI_FindWord(clickTextIndex,wordStart,wordEnd);
			ATextPoint	wordStartPoint, wordEndPoint;
			GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,wordStartPoint,true);
			GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,wordEndPoint,false);
			//
			SetSelect(wordStartPoint,wordEndPoint);
			//
			TextInput(undoTag_DragDrop,text);
			//
			ATextPoint	ept;
			GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart+text.GetItemCount(),ept,false);
			//#231 ���ł�wordStartPoint�͐������Ȃ��\��������i�s�܂�Ԃ�����̏ꍇ�A�����}����́A�}��������J�n�ʒu���O�s�Ɉړ�����j
			ATextPoint	spt;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(wordStart,spt);
			SetSelect(/*#231 wordStartPoint*/spt,ept);
			
		}
		else if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || 
					AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true ||//win 080709
					mDragging == false )//B0347 ���r���[��Drag���łȂ���Ώ�ɃR�s�[����
		{
			ABool	svKukeiSelected = mKukeiSelected;
			//DragCaret�ʒu���L�����b�g�ʒu�Ƃ��Đݒ�
			SetCaretTextPoint(clickTextPoint);
			//#231
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			//
			if( svKukeiSelected == false )
			{
				TextInput(undoTag_DragDrop,text);
				//
				ATextPoint	ept = GetCaretTextPoint();
				//#231 ���ł�clickTextPoint�͐������Ȃ��\��������i�s�܂�Ԃ�����̏ꍇ�A�����}����́A�}��������J�n�ʒu���O�s�Ɉړ�����j
				ATextPoint	spt;
				GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textindex,spt);
				SetSelect(/*#231 clickTextPoint*/spt,ept);
			}
			else
			{
				//Undo�A�N�V�����^�O�L�^
				GetTextDocument().SPI_RecordUndoActionTag(undoTag_DragDrop);
				//
				KukeiInput(text);
			}
		}
		else
		{
			//�ړ�Drag
			ABool	svKukeiSelected = mKukeiSelected;
			//�ړ���TextIndex��ۑ�
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			//
			ATextIndex	spos, epos;
			GetSelect(spos,epos);
			
			//==================Samrt drag==================
			//#888 �A���t�@�x�b�g���菈���C��
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kAddRemoveSpaceByDragDrop) == true &&
						text.GetItemCount() > 0 && //#888
						mKukeiSelected == false )
			{
				//==================�h���b�v��̃X�y�[�X����==================
				
				//�ړ���̑O��̕������A���t�@�x�b�g�Ȃ�Adrop���镶����ɃX�y�[�X������ǉ�
				if( textindex > 0 )
				{
					//------------------�}������e�L�X�g�̍ŏ����A���t�@�x�b�g�A���A�h���b�v��̑O�̕������A���t�@�x�b�g�Ȃ�A�}���e�L�X�g�̍ŏ��ɃX�y�[�X�ǉ�------------------
					
					//�}���e�L�X�g�̍ŏ��̕����擾
					AUCS4Char	ucs4char = 0;
					text.Convert1CharToUCS4(0,ucs4char);
					//�}���e�L�X�g�̍ŏ��̕����̃A���t�@�x�b�g����
					if( unicodeData.IsAlphabetOrNumber(ucs4char) == true )
					{
						//�h���b�v��̑O�̕������擾
						ATextIndex	prevtextindex = GetTextDocumentConst().SPI_GetPrevCharTextIndex(textindex);
						ucs4char = GetTextDocumentConst().SPI_GetText1UCS4Char(prevtextindex);
						//�h���b�v��̑O�̕����̃A���t�@�x�b�g����
						if( unicodeData.IsAlphabetOrNumber(ucs4char) == true )
						{
							text.Insert1(0,kUChar_Space);
						}
					}
				}
				if( textindex < GetTextDocumentConst().SPI_GetTextLength() )
				{
					//------------------�}������e�L�X�g�̍Ōオ�A���t�@�x�b�g�A���A�h���b�v��̌�̕������A���t�@�x�b�g�Ȃ�A�}���e�L�X�g�̍ŏ��ɃX�y�[�X�ǉ�------------------
					
					//�}���e�L�X�g�̍Ō�̕����擾
					AUCS4Char	ucs4char = 0;
					text.Convert1CharToUCS4(text.GetPrevCharPos(text.GetItemCount()),ucs4char);
					//�}���e�L�X�g�̍Ō�̕����̃A���t�@�x�b�g����
					if( unicodeData.IsAlphabetOrNumber(ucs4char) == true )
					{
						//�h���b�v��̌�̕������擾
						ucs4char = GetTextDocumentConst().SPI_GetText1UCS4Char(textindex);
						//�h���b�v��̌�̕����̃A���t�@�x�b�g����
						if( unicodeData.IsAlphabetOrNumber(ucs4char) == true )
						{
							//�X�y�[�X�ǉ�
							text.Add(kUChar_Space);
						}
					}
				}
				
				//==================�h���b�O���̃X�y�[�X�����i�O��̃X�y�[�X�폜�j==================
				ABool	smart1 = false, smart2 = false;
				ABool	del1 = false, del2 = false;
				if( spos >= 1 )
				{
					//�h���b�O���e�L�X�g�̒��O�̕������擾
					AUCS4Char	ucs4char = 0;
					ATextIndex	prevtextindex = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
					ucs4char = GetTextDocumentConst().SPI_GetText1UCS4Char(prevtextindex);
					//�h���b�O���e�L�X�g�̒��O�̕����̃A���t�@�x�b�g����
					if( unicodeData.IsAlphabetOrNumber(ucs4char) == false )
					{
						smart1 = true;
						if( GetTextDocumentConst().SPI_GetTextChar(spos-1) == kUChar_Space )   del1 = true;
					}
				}
				if( epos < GetTextDocumentConst().SPI_GetTextLength() )
				{
					//�h���b�O���e�L�X�g�̒���̕������擾
					AUCS4Char	ucs4char = 0;
					ucs4char = GetTextDocumentConst().SPI_GetText1UCS4Char(epos);
					//�h���b�O���e�L�X�g�̒���̕����̃A���t�@�x�b�g����
					if( unicodeData.IsAlphabetOrNumber(ucs4char) == false )
					{
						smart2 = true;
						if( GetTextDocumentConst().SPI_GetTextChar(epos) == kUChar_Space )   del2 = true;
					}
				}
				if( smart1 == true && smart2 == true )
				{
					if( del1 == true )   spos--;
					else if( del2 == true )   epos++;
					SetSelect(spos,epos);
				}
			}
			//�폜���TextIndex���v�Z�i�ړ��悪�A���݂̑I��͈͈ȍ~�̏ꍇ�A�폜����Ƃ��̕��A�O�Ɉړ����Ȃ��Ă͂����Ȃ��j
			ATextIndex	newtextindex = textindex;
			if( textindex >= epos )
			{
				newtextindex = textindex - (epos-spos);
			}
			//Undo�A�N�V�����^�O�L�^
			GetTextDocument().SPI_RecordUndoActionTag(undoTag_DragDrop);
			//�폜
			DeleteTextFromDocument(false);
			//DragCaret�ʒu���L�����b�g�ʒu�Ƃ��Đݒ�
			ATextPoint	textpoint;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(newtextindex,textpoint);
			SetCaretTextPoint(textpoint);
			//
			if( svKukeiSelected == false )
			{
				//�e�L�X�g�}��
				InsertTextToDocument(textpoint,text);
				//
				ATextPoint	ept = GetCaretTextPoint();
				//#231 ���ł�textpoint�͐������Ȃ��\��������i�s�܂�Ԃ�����̏ꍇ�A�����}����́A�}��������J�n�ʒu���O�s�Ɉړ�����j
				ATextPoint	spt;
				GetTextDocumentConst().SPI_GetTextPointFromTextIndex(newtextindex,spt);
				SetSelect(/*#231 textpoint*/spt,ept);
			}
			else
			{
				//
				KukeiInput(text);
			}
		}
	}
	//B0348 �t�@�C��Drop����͍Ō�ɂ���itext clipping�t�@�C����Drop�����ꍇ�A�e�L�X�g�������Ă���̂ł�����D��j
	else
	{
		//�t�@�C��Drop
		//#384 AFileAcc	file;
		//#384 if( AScrapWrapper::GetFileDragData(inDragRef,file) == true )
		AObjectArray<AFileAcc>	fileArray;//#384
		AScrapWrapper::GetFileDragData(inDragRef,fileArray);//#384
		if( fileArray.GetItemCount() > 0 )//#384
		{
			if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || 
						AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win 080709
			{
				//#384 �����̃t�@�C���𓯎��ɊJ��
				for( AIndex i = 0; i < fileArray.GetItemCount(); i++ )
				{
					GetApp().SPNVI_CreateDocumentFromLocalFile(fileArray.GetObjectConst(i));
				}
				return;
			}
			else
			{
				//DragCaret�ʒu���L�����b�g�ʒu�Ƃ��Đݒ�
				SetCaretTextPoint(clickTextPoint);
				//
				AIndex	lastCaretTextIndex = kIndex_Invalid;//#788
				//#384 �p�X�}�����͍ŏ��̃t�@�C����Ώ�
				//#788 �t�@�C��Drag&Drop�̃p�X�}�����A�����t�@�C���ɑΉ�����B
				for( AIndex i = 0; i < fileArray.GetItemCount(); i++ )
				{
					AFileAcc	file;
					file.CopyFrom(fileArray.GetObjectConst(i));//#788 0));
					
					//
					AText	text;
					AIndex	caretIndex = kIndex_Invalid;
					/*#329 
					if( MakeDropText(file,GetEmptyText(),GetEmptyText(),text,caretIndex) == false &&
					GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Number(AModePrefDB::kDragDrop_ActionForNoSuffix) == 0 )
					*/
					if( MakeDropText(file,GetEmptyText(),GetEmptyText(),text,caretIndex) == false )
					{
						switch( GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Number(AModePrefDB::kDragDrop_ActionForNoSuffix) )
						{
						  case 0://�t�@�C�����J��
							{
								//#91
								//#384 �����̃t�@�C���𓯎��ɊJ��
								//#788 for( AIndex i = 0; i < fileArray.GetItemCount(); i++ )
								//#788 {
								GetApp().SPNVI_CreateDocumentFromLocalFile(file);//#788 fileArray.GetObjectConst(i));
								//#788 }
								//#788 return;
								continue;//#788
							}
						  case 1://��΃p�X
							{
								file.GetPath(text);
								break;
							}
						  case 2://���΃p�X
							{
								AFileAcc	docFile;
								if( GetTextDocumentConst().NVI_GetFile(docFile) == true )
								{
									file.GetPartialPath(docFile,text);
								}
								else
								{
									file.GetPath(text);
								}
								break;
							}
						  default:
							{
								//�����Ȃ�
								break;
							}
						}
					}
					//#788
					AIndex	caretTextIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
					//
					TextInput(undoTag_DragDrop,text);
					//
					if( caretIndex != kIndex_Invalid )
					{
						/*#788
						ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
						textindex += caretIndex;
						GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textindex,clickTextPoint);
						SetCaretTextPoint(clickTextPoint);
						*/
						lastCaretTextIndex = caretTextIndex + caretIndex;
					}
				}
				//#788
				if( lastCaretTextIndex != kIndex_Invalid )
				{
					ATextPoint	caretTextPoint = kTextPoint_00;
					GetTextDocumentConst().SPI_GetTextPointFromTextIndex(lastCaretTextIndex,caretTextPoint);
					SetCaretTextPoint(caretTextPoint);
				}
			}
		}
  }
	//#905
	//���@�F���ƃ��[�J���͈͐ݒ�
	TriggerSyntaxRecognize();
}

/**
�h���b�O���̃h���b�O��L�����b�g�`��
*/
void	AView_Text::XorDragCaret()
{
	//��ʕ`��w�� #1332 �}�E�X�h���b�O����lockFocus�`�攽�f����Ȃ��悤�Ȃ̂ŁAdrawRect()���[�g�ŕ`�悷��B
	NVMC_RefreshControl();
	/*#1332
	AImagePoint	caretImageStart, caretImageEnd;
	GetImagePointFromTextPoint(mDragCaretTextPoint,caretImageStart);
	caretImageEnd = caretImageStart;
	caretImageEnd.y += GetLineHeightWithMargin(mDragCaretTextPoint.y);//#450 GetLineHeightWithMargin();
	ALocalPoint	caretLocalStart, caretLocalEnd;
	NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
	NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
	//�L�����b�g�F�擾�i�����F�Ɠ����ɂ���j
	AColor	modeLetterColor = kColor_Black;
	GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Color(AModePrefDB::kLetterColor,modeLetterColor);
	//�L�����b�g��������
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kBigCaret) == false )//#722
	{
		//����1
		NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,true,false,1);//#1034 ���flush����
	}
	//#722 drag���̃L�����b�g������ʏ�Ɠ����ɂ���
	else
	{
		//����2
		caretLocalStart.y -= 1;
		caretLocalEnd.y += 1;
		NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,true,false,2);//#1034 ���flush����
	}
	*/
}

/**
�h���b�O���̃��[�h�h���b�O��L�����b�g�`��
*/
void	AView_Text::XorWordDragDrop()
{
	//��ʕ`��w�� #1332 �}�E�X�h���b�O����lockFocus�`�攽�f����Ȃ��悤�Ȃ̂ŁAdrawRect()���[�g�ŕ`�悷��B
	NVMC_RefreshControl();
	/*#1332
	///�L�����b�g�F�擾�i�����F�Ɠ����ɂ���j
	AColor	modeLetterColor = kColor_Black;
	GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetModeData_Color(AModePrefDB::kLetterColor,modeLetterColor);
	//
	AImageRect	imageRect;
	GetLineImageRect(mDragCaretTextPoint.y,imageRect);
	AImagePoint	caretImagePoint, selectImagePoint;
	GetImagePointFromTextPoint(mDragCaretTextPoint,caretImagePoint);
	GetImagePointFromTextPoint(mDragSelectTextPoint,selectImagePoint);
	imageRect.left	= caretImagePoint.x;
	imageRect.right	= selectImagePoint.x;
	ALocalRect	localRect;
	NVM_GetLocalRectFromImageRect(imageRect,localRect);
	NVMC_DrawXorCaretRect(localRect,true,true);//#1034 ���flush����
	*/
}

/**
�c�[�����̓��[�h�J�n�i�R�}���h�{Y�����������j
*/
void	AView_Text::StartKeyToolMode()
{
	mKeyToolMode = true;
	//�L�����b�g�ʒu�̕��@�p�[�g�擾
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
	AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(mCaretTextPoint);
	AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
	//���[�U�[�c�[����menu object id�ƊJ�n�ʒu�A�I���ʒu���擾
	GetModePrefDB().GetKeyToolStartMenu(syntaxPartIndex,true,mKeyToolMenuObjectID_UserTool,
										mKeyToolMenuStartIndex_UserTool,mKeyToolMenuEndIndex_UserTool);
	//�A�v���g�ݍ��݃c�[����menu object id�ƊJ�n�ʒu�A�I���ʒu���擾
	GetModePrefDB().GetKeyToolStartMenu(syntaxPartIndex,false,mKeyToolMenuObjectID_AppTool,
										mKeyToolMenuStartIndex_AppTool,mKeyToolMenuEndIndex_AppTool);
	//�L�[�c�[�����X�g�E�C���h�E�X�V
	UpdateKeyToolListWindow();
}

/**
�c�[�����̓��[�h�ł̃L�[���͏���
*/
ABool	AView_Text::KeyTool( const AText& inText )
{
	if( mKeyToolMode == false )   return false;
	if( inText.GetItemCount() == 0 )   return false;
	
	//==================���̓L�[�擾==================
	AText	t;
	t.SetText(inText);
	t.ChangeCaseUpper();
	AUChar	keych = t.Get(0);
	
	//==================���[�U�[�c�[������==================
	ABool	foundFileInUserTool = false;
	ABool	foundFolderInUserTool = false;
	AFileAcc	userToolFile;
	if( mKeyToolMenuObjectID_UserTool != kObjectID_Invalid )
	{
		//
		for( AIndex i = mKeyToolMenuStartIndex_UserTool; i < mKeyToolMenuEndIndex_UserTool; i++ )
		{
			//���j���[�e�L�X�g�̍ŏ��̈ꕶ�����擾
			AText	menutext;
			GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mKeyToolMenuObjectID_UserTool,i,menutext);
			if( menutext.GetItemCount() < 2 )   continue;
			AText	t;
			t.SetText(menutext);
			t.ChangeCaseUpper();
			AUChar	ch = t.Get(0);
			AUChar	ch2 = t.Get(1);
			if( ch == keych && ch2 == kUChar_Space )
			{
				//------------------���̓L�[�ƈ�v���Ă����ꍇ------------------
				AObjectID	subMenuObjectID = GetApp().NVI_GetMenuManager().
							GetDynamicMenuItemSubMenuObjectIDByObjectID(mKeyToolMenuObjectID_UserTool,i);
				if( subMenuObjectID != kObjectID_Invalid )
				{
					//------------------�T�u���j���[�L��------------------
					//�t�H���_�����A���[�v�I��
					foundFolderInUserTool = true;
					mKeyToolMenuObjectID_UserTool = subMenuObjectID;
					mKeyToolMenuStartIndex_UserTool = 0;
					mKeyToolMenuEndIndex_UserTool = GetApp().NVI_GetMenuManager().
							GetDynamicMenuItemCountByObjectID(mKeyToolMenuObjectID_UserTool);
					break;
				}
				else
				{
					//------------------�T�u���j���[����------------------
					//�t�@�C�������A���[�v�I��
					foundFileInUserTool = true;
					AText	actiontext;
					GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(mKeyToolMenuObjectID_UserTool,i,actiontext);
					userToolFile.Specify(actiontext);
					break;
				}
			}
		}
	}
	if( foundFolderInUserTool == false )
	{
		mKeyToolMenuObjectID_UserTool = kObjectID_Invalid;
	}
	//==================�A�v�����c�[������==================
	ABool	foundFileInAppTool = false;
	ABool	foundFolderInAppTool = false;
	AFileAcc	appToolFile;
	if( mKeyToolMenuObjectID_AppTool != kObjectID_Invalid )
	{
		//
		for( AIndex i = mKeyToolMenuStartIndex_AppTool; i < mKeyToolMenuEndIndex_AppTool; i++ )
		{
			//���j���[�e�L�X�g�̍ŏ��̈ꕶ�����擾
			AText	menutext;
			GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mKeyToolMenuObjectID_AppTool,i,menutext);
			if( menutext.GetItemCount() < 2 )   continue;
			AText	t;
			t.SetText(menutext);
			t.ChangeCaseUpper();
			AUChar	ch = t.Get(0);
			AUChar	ch2 = t.Get(1);
			if( ch == keych && ch2 == kUChar_Space )
			{
				//------------------���̓L�[�ƈ�v���Ă����ꍇ------------------
				AObjectID	subMenuObjectID = GetApp().NVI_GetMenuManager().
							GetDynamicMenuItemSubMenuObjectIDByObjectID(mKeyToolMenuObjectID_AppTool,i);
				if( subMenuObjectID != kObjectID_Invalid )
				{
					//------------------�T�u���j���[�L��------------------
					//�t�H���_�����A���[�v�I��
					foundFolderInAppTool = true;
					mKeyToolMenuObjectID_AppTool = subMenuObjectID;
					mKeyToolMenuStartIndex_AppTool = 0;
					mKeyToolMenuEndIndex_AppTool = GetApp().NVI_GetMenuManager().
							GetDynamicMenuItemCountByObjectID(mKeyToolMenuObjectID_AppTool);
					break;
				}
				else
				{
					//------------------�T�u���j���[����------------------
					//�t�@�C�������A���[�v�I��
					foundFileInAppTool = true;
					AText	actiontext;
					GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(mKeyToolMenuObjectID_AppTool,i,actiontext);
					appToolFile.Specify(actiontext);
					break;
				}
			}
		}
	}
	if( foundFolderInAppTool == false )
	{
		mKeyToolMenuObjectID_AppTool = kObjectID_Invalid;
	}
	
	//==================���[�U�[�c�[���I��������==================
	if( foundFileInUserTool == true )
	{
		SPI_DoTool(userToolFile,0,false);
		EndKeyToolMode();
		return true;
	}
	//==================�A�v�����g�ݍ��݃c�[���I��������==================
	if( foundFileInAppTool == true )
	{
		SPI_DoTool(appToolFile,0,false);
		EndKeyToolMode();
		return true;
	}
	//==================�t�H���_�I��������==================
	if( foundFolderInUserTool == true || foundFolderInAppTool == true )
	{
		UpdateKeyToolListWindow();
		return true;
	}
	//==================�ǂ�ɂ���v���Ȃ������ꍇ==================
	EndKeyToolMode();
	return true;//#499 ���̓G���[���̕�����TextInput���Ȃ�false;
}

//R0073
void	AView_Text::EndKeyToolMode()
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetPopupKeyToolListWindow().NVI_Hide();
	}
	mKeyToolMode = false;
}

//
void	AView_Text::UpdateKeyToolListWindow()
{
	//�L�[���̓��[�h �I�����X�g�쐬
	AHashTextArray	textArray;
	ABoolArray	isUserToolArray;
	//==================�A�v�����c�[������쐬==================
	if( mKeyToolMenuObjectID_AppTool != kObjectID_Invalid )
	{
		for( AIndex i = mKeyToolMenuStartIndex_AppTool; i < mKeyToolMenuEndIndex_AppTool; i++ )
		{
			//���j���[�e�L�X�g�̍ŏ��̈ꕶ���擾
			AText	menutext;
			GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mKeyToolMenuObjectID_AppTool,i,menutext);
			if( menutext.GetItemCount() < 2 )   continue;
			AText	t;
			t.SetText(menutext);
			t.ChangeCaseUpper();
			AUChar	ch = t.Get(0);
			AUChar	ch2 = t.Get(1);
			AText	text;
			if( ch >= 0x80 || ch2 != kUChar_Space )
			{
				//�A�X�L�[�����{�X�y�[�X�ȊO�͉������Ȃ�
				continue;
			}
			else
			{
				//���X�g�ɒǉ�
				text.Add(ch);
				text.Add(kUChar_Colon);
				menutext.Delete1(0);
				text.AddText(menutext);
			}
			//�T�u���j���[�L��Ȃ�E�O�p�����ǉ�
			if( GetApp().NVI_GetMenuManager().GetDynamicMenuItemSubMenuObjectIDByObjectID(mKeyToolMenuObjectID_AppTool,i) != kObjectID_Invalid )
			{
				text.AddCstring(" ");
				AText	existSubMenuText;
				existSubMenuText.SetLocalizedText("ExistSubMenu");
				text.AddText(existSubMenuText);
			}
			//���ڒǉ�
			textArray.Add(text);
			isUserToolArray.Add(false);
		}
	}
	/*
	if( textArray.GetItemCount() > 0 )
	{
		AText	separatorText("---");
		textArray.Add(separatorText);
	}
	*/
	//==================���[�U�[�c�[������쐬==================
	if( mKeyToolMenuObjectID_UserTool != kObjectID_Invalid )
	{
		for( AIndex i = mKeyToolMenuStartIndex_UserTool; i < mKeyToolMenuEndIndex_UserTool; i++ )
		{
			//���j���[�e�L�X�g�̍ŏ��̈ꕶ���擾
			AText	menutext;
			GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mKeyToolMenuObjectID_UserTool,i,menutext);
			if( menutext.GetItemCount() < 2 )   continue;
			AText	t;
			t.SetText(menutext);
			t.ChangeCaseUpper();
			AUChar	ch = t.Get(0);
			AUChar	ch2 = t.Get(1);
			AText	text;
			if( ch >= 0x80 || ch2 != kUChar_Space )
			{
				//�A�X�L�[�����{�X�y�[�X�ȊO�͉������Ȃ�
				continue;
			}
			else
			{
				//���X�g�ɒǉ�
				text.Add(ch);
				text.Add(kUChar_Colon);
				menutext.Delete1(0);
				text.AddText(menutext);
			}
			//�T�u���j���[�L��Ȃ�E�O�p�����ǉ�
			if( GetApp().NVI_GetMenuManager().GetDynamicMenuItemSubMenuObjectIDByObjectID(mKeyToolMenuObjectID_UserTool,i) != kObjectID_Invalid )
			{
				text.AddCstring(" ");
				AText	existSubMenuText;
				existSubMenuText.SetLocalizedText("ExistSubMenu");
				text.AddText(existSubMenuText);
			}
			//
			AIndex	foundIndex = textArray.Find(text);
			if( foundIndex != kIndex_Invalid )
			{
				isUserToolArray.Set(foundIndex,true);
			}
			else
			{
				textArray.Add(text);
				isUserToolArray.Add(true);
			}
		}
	}
	//==================�E�C���h�E�\���ʒu����==================
	AImagePoint	ipt;
	GetImagePointFromTextPoint(GetCaretTextPoint(),ipt);
	ALocalPoint	lpt;
	NVM_GetLocalPointFromImagePoint(ipt,lpt);
	lpt.x = 16;
	AGlobalPoint	gpt;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lpt,gpt);
	//==================�|�b�v�A�b�v�E�C���h�E�\��==================
	if( mTextWindowID != kObjectID_Invalid )
	{
		//�|�b�v�A�b�v�E�C���h�E�������Ȃ琶������
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetPopupKeyToolListWindow();
	}
	//==================���X�g�f�[�^�ݒ�==================
	ATextArray	ta;
	ta.SetFromTextArray(textArray);
	GetApp().SPI_SetKeyToolListInfo(mTextWindowID,NVI_GetControlID(),
									gpt,GetLineHeightWithMargin(GetCaretTextPoint().y),ta,isUserToolArray);
	//==================keytool�E�C���h�E�\��==================
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetPopupKeyToolListWindow().NVI_Show(false);
	}
}

//R0073
void	AView_Text::SPI_KeyTool( const AUChar inKeyChar )
{
	AText	text;
	text.Add(inKeyChar);
	KeyTool(text);
}

//#379
/**
*/
void	AView_Text::DiffWithRepository()
{
	//�h�L�������g��Diff���[�h�ݒ�
	ADiffTargetMode	oldDiffTargetMode = GetTextDocument().SPI_GetDiffMode();
	if( oldDiffTargetMode != kDiffTargetMode_RepositoryLatest )
	{
		GetTextDocument().SPI_SetDiffMode_Repository();
	}
	//�T�u�y�C��Diff�\���؂�ւ�
	if( mTextWindowID != kObjectID_Invalid )
	{
		/*#725 diff�ł́A�T�C�h�o�[�\���͐��䂵�Ȃ��B
		//�T�u�y�C���\��
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowHideLeftSideBarColumn(true);
		*/
		//Diff�\���ɂ��邩�ǂ����̔���
		ABool	showDiffDisplay = false;
		if( oldDiffTargetMode != kDiffTargetMode_RepositoryLatest ||
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetDiffDisplayMode() == false )
		{
			//Diff�Ώۂ̐؂�ւ��A�܂��́A����Diff�\�����Ă��Ȃ��ꍇ�́ADiff�\���֐؂�ւ�
			showDiffDisplay = true;
		}
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(showDiffDisplay);
	}
}

void	AView_Text::NextDiff()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	/*#512
	ATextPoint	pt;
	pt.x = 0;
	pt.y = GetTextDocumentConst().SPI_GetNextDiffLineIndex(spt.y);
	*/
	AIndex	start = kIndex_Invalid, end = kIndex_Invalid;//#512
	GetTextDocumentConst().SPI_GetNextDiffLineIndex(spt.y,start,end);//#512
	if( /*#512pt.y*/start != kIndex_Invalid )
	{
		ATextPoint	pt;
		pt.x = 0;
		pt.y = start;
		//#688 ���ֈړ��iSetCaretTextPoint()���ŃT�u�e�L�X�g�̃X�N���[���ʒu�����������Ă��܂��̂Łj SetCaretTextPoint(pt);
		//SPI_AdjustScroll_Center()����NVIDO_ScrollPostProcess()�o�R��SPI_AdjustDiffDisplayScroll()
		//��currentDiffIndex=kIndex_Invalid�ŃR�[������A�T�u�y�C�����X�N���[�������B
		//�����āArefresh�O�ɍēx���L�ŃX�N���[������̂ŁA����ɕ`��X�V����Ȃ��B
		//���̖��΍�̂��߁A�t���O���������āANVIDO_ScrollPostProcess()�o�R��SPI_AdjustDiffDisplayScroll()
		//���Ă΂Ȃ��悤�ɂ���B#611
		mDisableAdjustDiffDisplayScrollAtScrollPostProcess = true;//#611
		SPI_AdjustScroll_Center(start,end,0);//#512 pt);
		mDisableAdjustDiffDisplayScrollAtScrollPostProcess = false;//#611
		/*#688
		//#379 Diff���h�L�������g���ADiff Part��������悤�ɃX�N���[������
		if( mTextWindowID != kObjectID_Invalid )
		{
			AIndex	currentDiffIndex = GetTextDocumentConst().SPI_GetDiffIndexByIncludingLineIndex(pt.y);
			GetApp().SPI_GetTextWindowByID(mTextWindowID).
					SPI_AdjustDiffDisplayScroll(mTextDocumentRef.GetObjectID(),NVI_GetControlID(),currentDiffIndex);
		}
		*/
		//�L�����b�g�ݒ�i�{�T�u�e�L�X�g�̃X�N���[���ʒu�����j
		SetCaretTextPoint(pt);
		AdjustScroll_Center(mCaretTextPoint);//�܂肽���݂�����ꍇ�ɁA��ł͐�����adjust����Ȃ����Ƃ����邽��
	}
}

void	AView_Text::PrevDiff()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	/*#512
	ATextPoint	pt;
	pt.x = 0;
	pt.y = GetTextDocumentConst().SPI_GetPrevDiffLineIndex(spt.y);
	*/
	AIndex	start = kIndex_Invalid, end = kIndex_Invalid;//#512
	GetTextDocumentConst().SPI_GetPrevDiffLineIndex(spt.y,start,end);//#512
	if( /*#512pt.y*/start != kIndex_Invalid )
	{
		ATextPoint	pt;
		pt.x = 0;
		pt.y = start;
		//#688 ���ֈړ��iSetCaretTextPoint()���ŃT�u�e�L�X�g�̃X�N���[���ʒu�����������Ă��܂��̂ŁjSetCaretTextPoint(pt);
		mDisableAdjustDiffDisplayScrollAtScrollPostProcess = true;//#611
		SPI_AdjustScroll_Center(start,end,0);//#512 pt); 
		mDisableAdjustDiffDisplayScrollAtScrollPostProcess = false;//#611
		/*#688
		//#379 Diff���h�L�������g���ADiff Part��������悤�ɃX�N���[������
		if( mTextWindowID != kObjectID_Invalid )
		{
			AIndex	currentDiffIndex = GetTextDocumentConst().SPI_GetDiffIndexByIncludingLineIndex(pt.y);
			GetApp().SPI_GetTextWindowByID(mTextWindowID).
					SPI_AdjustDiffDisplayScroll(mTextDocumentRef.GetObjectID(),NVI_GetControlID(),currentDiffIndex);
		}
		*/
		//�L�����b�g�ݒ�i�{�T�u�e�L�X�g�̃X�N���[���ʒu�����j
		SetCaretTextPoint(pt);
		AdjustScroll_Center(mCaretTextPoint);//�܂肽���݂�����ꍇ�ɁA��ł͐�����adjust����Ȃ����Ƃ����邽��
	}
}

/*#725
//RC2
//���ʎq���\���i���݂̃L�����b�g�ʒu���ӂ̒P��̎��ʎq����\������j
void	AView_Text::IdInfo()
{
	//#291 GetApp().SPI_GetIdInfoWindow().NVI_Show(false);
	GetApp().SPI_ShowHideIdInfoWindow(true);//#291
	AIndex	spos, epos;
	GetSelect(spos,epos);
	AText	text;
	if( spos == epos )
	{
		GetTextDocument().SPI_FindWord(spos,spos,epos);
	}
	GetTextDocumentConst().SPI_GetText(spos,epos,text);
	SetIdInfo(text);
}
*/

#if IMPLEMENTATION_FOR_MACOSX
//R0231
//�����Œ��ׂ�i�|�b�v�A�b�v�\���j
void	AView_Text::DictionaryPopup()
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		AText	text;
		GetSelectedText(text);
		ATextPoint	spt,ept;
		SPI_GetSelect(spt,ept);
		AImagePoint	ipt;
		GetImagePointFromTextPoint(spt,ipt);
		ALocalPoint	pt;
		NVM_GetLocalPointFromImagePoint(ipt,pt);
		if( NVI_GetVerticalMode() == false )
		{
			//����������ascent�����v���X����
			pt.y += GetLineAscent(spt.y);//#450 mLineAscent;
		}
		else
		{
			//�c�������͍s���������v���X����
			pt.y += GetLineHeightWithoutMargin(spt.y);
		}
		NVMC_ShowDictionary(text,pt);
	}
}

//R0231
//�����Œ��ׂ�i�����A�v���ŕ\���j
void	AView_Text::Dictionary()
{
	AText	text;
	GetSelectedText(text);
	text.ConvertToURLEscape();
	text.InsertCstring(0,"dict:///");
	ALaunchWrapper::OpenURL(text);
}
#endif

#if 0
//RC2
//�L�[���͂ɂ�鎩��IdInfo�\��
void	AView_Text::IdInfoAuto()
{
	//�L�����b�g�ʒu�擾
	AIndex	spos, epos;
	GetSelect(spos,epos);
	//�L�����b�g�ʒu���O�ŃA���t�@�x�b�g�����݂���ʒu���擾����
	for( spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos); spos > 0; spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos) )
	{
		if( GetTextDocumentConst().SPI_IsAsciiAlphabet(spos) == true )
		{
			break;
		}
	}
	//�P��ʒu�擾
	GetTextDocument().SPI_FindWord(spos,spos,epos);
	//��L�Ŏ擾�����P��ɂ��āA���ʎq���\��
	AText	text;
	GetTextDocumentConst().SPI_GetText(spos,epos,text);
	SetIdInfo(text);
	GetApp().SPI_GetIdInfoView().SPI_SetArgIndex(0);
}

//RC2
void	AView_Text::IdInfoAutoArg()
{
	//��
	/*#725
	
	//#225 ���ʎq���E�C���h�E��\���Ȃ牽�����Ȃ�
	if( GetApp().SPI_GetIdInfoWindow().NVI_IsWindowVisible() == false )   return;
	
	const ADocument_Text& document = GetTextDocumentConst();
	//�P��
	AIndex	argindex = 0;
	//�����I���ʒu�擾
	AIndex	localRangeStartLineIndex = document.SPI_GetCurrentLocalIdentifierStartLineIndex();
	if( localRangeStartLineIndex == kIndex_Invalid )   return;
	//�L�����b�g�ʒu�擾
	AIndex	spos, epos;
	GetSelect(spos,epos);
	ATextIndex	localRangeStart = document.SPI_GetLineStart(localRangeStartLineIndex);
	//#695
	if( localRangeStart < spos - 10000 )
	{
		localRangeStart = document.SPI_GetCurrentCharTextIndex(spos-10000);
	}
	//�L�����b�g�ʒu���O��'('������������
	const ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetSyntaxDefinition();
	if( syntaxDefinition.GetIdInfoAutoDisplayCharCount() == 0 )   return;//#225 IsIdInfoAutoDisplayChar�����݂��Ȃ��Ȃ炷���ɏI���i���ꂪ�Ȃ��ƍŏ��܂�break���Ȃ��j
	AIndex	level = 0;
	for( spos = document.SPI_GetPrevCharTextIndex(spos); spos > localRangeStart; spos = document.SPI_GetPrevCharTextIndex(spos) )
	{
		AUChar	ch = document.SPI_GetTextChar(spos);
		//level��0�̂Ƃ��̂�
		if( level == 0 )
		{
			//IsIdInfoAutoDisplayChar�iC����̏ꍇ��")"�j�Ȃ�����P�O�ɖ߂��ďI��
			if( syntaxDefinition.IsIdInfoAutoDisplayChar(ch) == true )
			{
				spos = document.SPI_GetPrevCharTextIndex(spos);
				break;
			}
			//IsIdInfoDelimiterChar�iC����̏ꍇ��","�j�Ȃ�argindex��+1���Čp��
			if( syntaxDefinition.IsIdInfoDelimiterChar(ch) == true )
			{
				argindex++;
			}
		}
		//()���𖳎����邽�߂�level���㉺
		if( syntaxDefinition.IsIdInfoStartChar(ch) == true )   level--;
		if( syntaxDefinition.IsIdInfoEndChar(ch) == true )   level++;
	}
	//���ʎq�擾
	document.SPI_FindWord(spos,spos,epos);
	AText	text;
	//���ݕ\�����̎��ʎq�Ɠ����Ȃ�A�������X�V
	document.SPI_GetText(spos,epos,text);
	if( GetApp().SPI_GetIdInfoView().SPI_GetCurrentIdText().Compare(text) == true )
	{
		//�����C���f�b�N�X�ݒ�
		GetApp().SPI_GetIdInfoView().SPI_SetArgIndex(argindex);
	}
	*/
}
#endif

//B0432
//�P��I��
void	AView_Text::SelectWord()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	ATextIndex	textpos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
	ATextIndex	spos,epos;
	GetTextDocument().SPI_FindWord(textpos,spos,epos);
	SetSelect(spos,epos);
}

//#858
#if 0
//RC3
//�}�C�R�����g�@�\�@
void	AView_Text::ExternalComment()
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		//�v���W�F�N�g�t�H���_���ݒ�̏ꍇ�̓��[�j���O���o���ďI��
		if( GetTextDocumentConst().SPI_GetProjectObjectID() == kObjectID_Invalid )
		{
			//�f�t�H���g�t�H���_���擾
			AFileAcc	defaultFolder;
			GetTextDocumentConst().SPI_GetParentFolder(defaultFolder);
			//���[�j���O
			GetApp().SPI_ShowProjectFolderAlertWindow(defaultFolder);
			return;
		}
		//���݂̑I��͈͎擾
		ATextPoint	spt, ept;
		SPI_GetSelect(spt,ept);
		//#138 �V�K�tⳎ��^�܂��͊����tⳎ��ҏW
		EditOrNewFusen(spt.y);
		/*
		//���݈ʒu�̃��W���[���ƃI�t�Z�b�g���擾
		AText	moduleName;
		AIndex	identifierLineIndex = kIndex_Invalid;
		GetTextDocument().SPI_GetModuleNameAndOffsetByLineIndex(spt.y,moduleName,identifierLineIndex);
		//�}�C�R�����g�_�C�A���O�\��
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowExternalCommentWindow(moduleName,spt.y-identifierLineIndex);
		*/
	}
}
#endif

//#146
/**
�W�����v
*/
void	AView_Text::Jump( const AUniqID inJumpIdentifier )
{
	//���݂̈ʒu��Navigate�o�^����
	GetApp().SPI_RegisterNavigationPosition();
	//�W�����v
	ATextPoint	spt,ept;
	GetTextDocument().SPI_GetGlobalIdentifierRange(inJumpIdentifier,spt,ept);
	AdjustScroll_Top(spt);
	SetSelect(spt,ept,true);
	//#130 �A�E�g���C�� �e�X�g�i�w�茩�o���͈͈ȊO���\���j
	/*
	AIndex	jumpIndex = GetTextDocumentConst().SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	ANumber	imageY = 0;
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		//�����EImageY�ݒ�
		ANumber	lineHeight = CalcLineHeight(lineIndex);
		if( GetTextDocumentConst().SPI_GetJumpMenuItemIndexByLineIndex(lineIndex) != jumpIndex )
		{
			lineHeight = 0;
		}
		mLineImageInfo_Height.Set(lineIndex,lineHeight);
		mLineImageInfo_ImageY.Set(lineIndex,imageY);
		imageY += lineHeight;
	}
	mLineImageInfo_Height.Set(lineCount,0);
	mLineImageInfo_ImageY.Set(lineCount,imageY);
	//ImageSize�X�V
	SPI_UpdateImageSize();
	NVI_Refresh();
	*/
}

//#150
/**
���փW�����v
*/
void	AView_Text::JumpNext()
{
	AIndex menuItemIndex = GetTextDocument().SPI_GetJumpMenuItemIndexByLineIndex(GetCaretTextPoint().y);
	if( menuItemIndex+1 >= GetTextDocument().SPI_GetJumpMenuTextArray().GetItemCount() )   return;
	AUniqID	identifierObjectID = GetTextDocument().SPI_GetJumpMenuIdentifierUniqID(menuItemIndex+1);
	Jump(identifierObjectID);
}

//#150
/**
�O�փW�����v
*/
void	AView_Text::JumpPrev()
{
	AIndex menuItemIndex = GetTextDocument().SPI_GetJumpMenuItemIndexByLineIndex(GetCaretTextPoint().y);
	if( menuItemIndex-1 < 0 )   return;
	AUniqID	identifierObjectID = GetTextDocument().SPI_GetJumpMenuIdentifierUniqID(menuItemIndex-1);
	Jump(identifierObjectID);
}

//win
/**
�I�𕶎���ݒ�
*/
void	AView_Text::SetFindText()
{
	if( IsCaretMode() == true )   return;
	AText	text;
	GetSelectedText(text);
	GetApp().SPI_SetFindText(text);
	//
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	GetTextDocument().SPI_SetFindHighlight(param);
	//�����{�b�N�X�ɂ��ݒ� #137
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetSearchBoxText(text);
	}
}

/*#233 �e�X�g
void	AView_Text::XorHoverCaret( const ATextPoint& inHoverCaret )
{
	AImagePoint	caretImageStart, caretImageEnd;
	GetImagePointFromTextPoint(inHoverCaret,caretImageStart);
	caretImageEnd = caretImageStart;
	caretImageEnd.y += GetLineHeightWithMargin();
	ALocalPoint	caretLocalStart, caretLocalEnd;
	NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
	NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
	NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,false,2);
}
*/

//#725
/**
NVI_SetShow()�R�[�����̏���
*/
void	AView_Text::NVIDO_SetShow( const ABool inShow )
{
	if( mPopupNotificationWindowID == kObjectID_Invalid )
	{
		return;
	}
	if( SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_GetItemCount() == 0 )
	{
		return;
	}
	
	//notification �E�C���h�E��\���^��\��
	SPI_ShowNotificationPopupWindow(inShow);
}

//#725
/**
NVI_SetPosition()�R�[�����̏���
*/
void	AView_Text::NVIDO_SetPosition( const AWindowPoint& inWindowPoint )
{
	if( mPopupNotificationWindowID == kObjectID_Invalid )
	{
		return;
	}
	if( SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_GetItemCount() == 0 )
	{
		return;
	}
	
	//notification�E�C���h�E��bounds�X�V
	UpdateNotificationPopupWindowBounds();
}

//#725
/**
NVI_SetSize()�R�[�����̏���
*/
void	AView_Text::NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight )
{
	if( mPopupNotificationWindowID == kObjectID_Invalid )
	{
		return;
	}
	if( SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_GetItemCount() == 0 )
	{
		return;
	}
	
	//notification�E�C���h�E��bounds�X�V
	UpdateNotificationPopupWindowBounds();
}

//#905
/**
���@�F�����s�g���K�[
*/
void	AView_Text::TriggerSyntaxRecognize()
{
	//���@�F�����s�iunrecognized�ȍs���T�[�`���đS�āi�X���b�h�Łj�F������j
	GetTextDocument().SPI_RecognizeSyntaxUnrecognizedLines();
	//���[�J�����ʎq
	GetTextDocument().SPI_SetCurrentLocalTextPoint(GetCaretTextPoint(),GetObjectID());
}

//�e�X�g�p������
//����������������������������������
//�ǂǂǂǂǂǊ����ǂǂǂǂǂǂǂ�
//����������������������������������
//aaa����������������������������

//#1035
/**
������ϊ�
*/
void	AView_Text::Transliterate( const AText& inTitle )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( mKukeiSelected == false )
	{
		//�ʏ�I����
		
		//�I��͈͎擾
		ATextIndex	spos,epos;
		GetSelect(spos,epos);
		//�I��͈̓e�L�X�g�擾
		AText	origtext, text;
		GetSelectedText(origtext);
		//������ϊ����s
		GetApp().SPI_Transliterate(inTitle,origtext,text);
		//�e�L�X�g�u��
		TextInput(undoTag_Transliterate,text);
		//�I��͈͍X�V
		SetSelect(spos,spos+text.GetItemCount());
	}
	else
	{
		//��`�I����
		
		//undo�^�O
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Transliterate);
		//��`�I��͈͎擾
		AArray<ATextIndex>	sposArray, eposArray;
		SPI_GetSelect(sposArray,eposArray);
		AItemCount	totalOffset = 0;
		for( AIndex i = 0; i < sposArray.GetItemCount(); i++ )
		{
			//�I��͈͎擾
			ATextIndex	spos = sposArray.Get(i)+totalOffset;
			ATextIndex	epos = eposArray.Get(i)+totalOffset;
			//�I��͈̓e�L�X�g�擾
			AText	origtext, text;
			GetTextDocumentConst().SPI_GetText(spos,epos,origtext);
			//������ϊ����s
			GetApp().SPI_Transliterate(inTitle,origtext,text);
			//�e�L�X�g�u��
			DeleteTextFromDocument(spos,epos);
			InsertTextToDocument(spos,text);
			//�I�t�Z�b�g�X�V
			totalOffset += text.GetItemCount() - origtext.GetItemCount();
			//�I��͈͍X�V
			sposArray.Set(i,spos);
			eposArray.Set(i,spos+text.GetItemCount());
		}
		//��`�I��͈͍X�V
		SetKukeiSelect(sposArray,eposArray);
	}
}

//#1035
/**
������ϊ��i���������j
*/
void	AView_Text::TransliterateToLower()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( mKukeiSelected == false )
	{
		//�ʏ�I����
		
		//�I��͈͎擾
		ATextIndex	spos,epos;
		GetSelect(spos,epos);
		//�I��͈̓e�L�X�g�擾
		AText	text;
		GetSelectedText(text);
		//������ϊ����s
		text.ChangeCaseLower();
		//�e�L�X�g�u��
		TextInput(undoTag_Transliterate,text);
		//�I��͈͍X�V
		SetSelect(spos,spos+text.GetItemCount());
	}
	else
	{
		//��`�I����
		
		//undo�^�O
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Transliterate);
		//��`�I��͈͎擾
		AArray<ATextIndex>	sposArray, eposArray;
		SPI_GetSelect(sposArray,eposArray);
		AItemCount	totalOffset = 0;
		for( AIndex i = 0; i < sposArray.GetItemCount(); i++ )
		{
			//�I��͈͎擾
			ATextIndex	spos = sposArray.Get(i)+totalOffset;
			ATextIndex	epos = eposArray.Get(i)+totalOffset;
			//�I��͈̓e�L�X�g�擾
			AText	origtext, text;
			GetTextDocumentConst().SPI_GetText(spos,epos,origtext);
			text.SetText(origtext);
			//������ϊ����s
			text.ChangeCaseLower();
			//�e�L�X�g�u��
			DeleteTextFromDocument(spos,epos);
			InsertTextToDocument(spos,text);
			//�I�t�Z�b�g�X�V
			totalOffset += text.GetItemCount() - origtext.GetItemCount();
			//�I��͈͍X�V
			sposArray.Set(i,spos);
			eposArray.Set(i,spos+text.GetItemCount());
		}
		//��`�I��͈͍X�V
		SetKukeiSelect(sposArray,eposArray);
	}
}

//#1035
/**
������ϊ��i�啶�����j
*/
void	AView_Text::TransliterateToUpper()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( mKukeiSelected == false )
	{
		//�ʏ�I����
		
		//�I��͈͎擾
		ATextIndex	spos,epos;
		GetSelect(spos,epos);
		//�I��͈̓e�L�X�g�擾
		AText	text;
		GetSelectedText(text);
		//������ϊ����s
		text.ChangeCaseUpper();
		//�e�L�X�g�u��
		TextInput(undoTag_Transliterate,text);
		//�I��͈͍X�V
		SetSelect(spos,spos+text.GetItemCount());
	}
	else
	{
		//��`�I����
		
		//undo�^�O
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Transliterate);
		//��`�I��͈͎擾
		AArray<ATextIndex>	sposArray, eposArray;
		SPI_GetSelect(sposArray,eposArray);
		AItemCount	totalOffset = 0;
		for( AIndex i = 0; i < sposArray.GetItemCount(); i++ )
		{
			//�I��͈͎擾
			ATextIndex	spos = sposArray.Get(i)+totalOffset;
			ATextIndex	epos = eposArray.Get(i)+totalOffset;
			//�I��͈̓e�L�X�g�擾
			AText	origtext, text;
			GetTextDocumentConst().SPI_GetText(spos,epos,origtext);
			text.SetText(origtext);
			//������ϊ����s
			text.ChangeCaseUpper();
			//�e�L�X�g�u��
			DeleteTextFromDocument(spos,epos);
			InsertTextToDocument(spos,text);
			//�I�t�Z�b�g�X�V
			totalOffset += text.GetItemCount() - origtext.GetItemCount();
			//�I��͈͍X�V
			sposArray.Set(i,spos);
			eposArray.Set(i,spos+text.GetItemCount());
		}
		//��`�I��͈͍X�V
		SetKukeiSelect(sposArray,eposArray);
	}
}

//#1154
/**
�L�����b�g�̍��E�̕�������ւ�
*/
void	AView_Text::SwapLetter()
{
	//
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//�I�𒆈ȊO�̏ꍇ�͉����������^�[��
	if( IsCaretMode() == false )   return;
	//�L�����b�g�ʒu�擾
	ATextPoint	spt,ept;
	SPI_GetSelect(spt,ept);
	ATextIndex	startIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
	//�L�����b�g�ʒu���h�L�������g�̍ŏ��܂��͍Ō�̏ꍇ�͉������Ȃ�
	if( startIndex == 0 || startIndex >= GetTextDocumentConst().SPI_GetTextLength() )   return;
	//�O�̕����̈ʒu���擾
	ATextIndex	prevIndex = GetTextDocumentConst().SPI_GetPrevCharTextIndex(startIndex);
	//�O�̕������擾
	AText	swapText;
	GetTextDocumentConst().SPI_GetText(prevIndex,startIndex,swapText);
	//���̕����̈ʒu���擾
	ATextIndex	nextIndex = GetTextDocumentConst().SPI_GetNextCharTextIndex(startIndex);
	ATextPoint	nextpt;
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(nextIndex,nextpt);
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
	//���̕����̈ʒu�ցA�O�̕�����}��
	InsertTextToDocument(nextIndex,swapText);
	//�O�̕������폜�i�O�s�͌��֍폜���Ă���̂Ńe�L�X�g�̈ʒu�͕ω����Ă��Ȃ��j
	DeleteTextFromDocument(prevIndex,startIndex);
	//�L�����b�g�ʒu�␳�B�i�폜�����J�n�ʒu�̎��̕����ʒu���擾�j
	ATextIndex	caretTextIndex = GetTextDocumentConst().SPI_GetNextCharTextIndex(prevIndex);
	ATextPoint	caretpt;
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(caretTextIndex,caretpt);
	SetCaretTextPoint(caretpt);
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---Update

//
/**
�\���X�V
*/
void	AView_Text::SPI_UpdateText( const AIndex inStartLineIndex, const AIndex inEndLineIndex,
		const AItemCount inInsertedLineCount, const ABool inParagraphCountChanged )//#450
{
	//�w�i�`�悠��̏ꍇ�A�S�ĕ`�悹����𓾂Ȃ��̂ŁA�X�N���[���ł̍s�}���E�폜�`��͂��Ȃ�
	
	//
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//
	AImageRect	imageLineRect;
	GetLineImageRect(inStartLineIndex,inEndLineIndex,imageLineRect);
	ALocalRect	localLineRect;
	NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
	//�s�}���E�s�폜�ŁA�X�V�J�n�s(inStartLineIndex)���A�t���[�����O�������ꍇ�́A�}���s������redraw�Ȃ��X�N���[������i�\���X�V��RefreshRect()�ōs���j
	if( inInsertedLineCount != 0 && imageLineRect.top < imageFrameRect.top )
	{
		NVI_Scroll(0,inInsertedLineCount*GetLineHeightPlusMargin()/*#450 GetLineHeightWithMargin()*/,false);
	}
	//�X�V�ŏI�s���A�e�L�X�g�S�̂̍ŏI�s�A���A�s�폜�ɂ����̂ł������ꍇ�́A�r���[�̍ŉ����܂ōX�V
	if( inEndLineIndex >= GetTextDocumentConst().SPI_GetLineCount() && inInsertedLineCount < 0 )
	{
		localLineRect.bottom = localFrameRect.bottom;
	}
	//�X�V�͈͂�view�̉E�[�܂łɂ���
	if( localLineRect.right < localFrameRect.right )
	{
		localLineRect.right = localFrameRect.right;
	}
	//
	//�X�V
	NVMC_RefreshRect(localLineRect);
	//#450 �s�ԍ��r���[�\���X�V
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().SPI_UpdateText(inStartLineIndex,inEndLineIndex,inInsertedLineCount,inParagraphCountChanged);
	}
}

//#450
/**
�\���X�V
*/
void	AView_Text::SPI_RefreshTextView()
{
	if( NVI_IsVisible() == false )   return;//#530
	//�e�L�X�g�r���[�\���X�V
	NVI_Refresh();
	//�s�ԍ��r���[�\���X�V
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().NVI_Refresh();
	}
	//DiffInfoWindow�\���X�V #379
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_RefreshDiffInfoWindow();
	}
}

//#695
/**
�\���X�V
*/
void	AView_Text::SPI_RefreshLines( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	//
	if( NVI_IsVisible() == false )   return;//#530
	
	//�w��s�͈͎̔擾���ARefreshRect����
	AImageRect	imageLineRect = {0,0,0,0};
	GetLineImageRect(inStartLineIndex,inEndLineIndex,imageLineRect);
	ALocalRect	localLineRect = {0,0,0,0};
	NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
	//�X�V�͈͂�view�̉E�[�܂łɂ���
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	if( localLineRect.right < localFrameRect.right )
	{
		localLineRect.right = localFrameRect.right;
	}
	//
	NVMC_RefreshRect(localLineRect);
	//�s�ԍ��y�C���̕`��X�V
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().SPI_RefreshLines(inStartLineIndex,inEndLineIndex);
	}
	//DiffInfoWindow�\���X�V #379
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_RefreshDiffInfoWindow();
	}
}

//#496
/**
�w��s�̕`��X�V
*/
void	AView_Text::SPI_RefreshLine( const AIndex inLineIndex )
{
	//�w��s�͈͎̔擾���ARefreshRect����
	AImageRect	imageLineRect;
	GetLineImageRect(inLineIndex,imageLineRect);
	ALocalRect	localLineRect;
	NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
	//�X�V�͈͂�view�̉E�[�܂łɂ���
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	if( localLineRect.right < localFrameRect.right )
	{
		localLineRect.right = localFrameRect.right;
	}
	//
	NVMC_RefreshRect(localLineRect);
	//�s�ԍ��y�C���̕`��X�V
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().SPI_RefreshLine(inLineIndex);
	}
}

//
void	AView_Text::SPI_UpdateImageSize()
{
	//#450 LineImageInfo���쐬�Ȃ牽���������^�[��
	if( mLineImageInfo_ImageY.GetItemCount() == 0 )   return;
	
	ANumber	width = 20000;//�� 10000��20000 #1069
	ANumber	height = mLineImageInfo_ImageY.Get(mLineImageInfo_ImageY.GetItemCount()-1);//#450 GetTextDocument().SPI_GetLineCount() * GetLineHeightWithMargin();
	if( (GetTextDocument().SPI_GetWrapMode() != kWrapMode_NoWrap) && 
		(GetTextDocument().SPI_GetWrapMode() != kWrapMode_WordWrapByLetterCount) &&
		(GetTextDocument().SPI_GetWrapMode() != kWrapMode_LetterWrapByLetterCount) ) //#1113
	{
		width = NVI_GetViewWidth();
		//#531 �T�u�y�C���ōs�܂�Ԃ��h�L�������g�����E�X�N���[���ł���悤�ɂ��邽��
		if( mTextWindowID != kObjectID_Invalid )
		{
			width = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetTextViewWidth(/*#695 mTextDocumentID*/mTextDocumentRef.GetObjectID());
		}
	}
	//#612 ������wrap�̂Ƃ��́AView�̕���"W"�̕������w�蕶�������Ƃ���B�i�������E�C���h�E�����͏��������Ȃ��j
	if( GetTextDocumentConst().SPI_GetWrapMode() == kWrapMode_WordWrapByLetterCount ||
		GetTextDocumentConst().SPI_GetWrapMode() == kWrapMode_LetterWrapByLetterCount ) //#1113
	{
		AText	w("a");//#1186 ������w��a�ɕύX�B�i�c�������e�\������View�����L���Ȃ肷���āA�X�N���[���o�[���\������Ă��܂������C�����邽�߁B�j
		width = static_cast<ANumber>(NVMC_GetDrawTextWidth(w) * GetTextDocumentConst().SPI_GetWrapLetterCount());
		ANumber	textviewwidth = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetTextViewWidth(/*#695 mTextDocumentID*/mTextDocumentRef.GetObjectID());
		if( width < textviewwidth )
		{
			width = textviewwidth;
		}
	}
	//
	NVM_SetImageSize(width,height);
	//#379 Document����̍X�V�C�x���g��TextView��LineNumber�ւ��ʒm����悤�ɂ���
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().SPI_UpdateImageSize();
	}
	//�s�܂�Ԃ��X���b�hpaused���ɃX�N���[���o�[�����X�V���邽�߂ɁAScrollbar unit���X�V #688
	SPI_UpdateScrollBarUnit();
}

//
void	AView_Text::SPI_UpdateScrollBarUnit()
{
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(kHScrollBarUnit,GetLineHeightPlusMargin()//#450 GetLineHeightWithMargin()
			,rect.right-rect.left-kHScrollBarUnit*5,rect.bottom-rect.top-/*#450 GetLineHeightWithMargin()*/GetLineHeightPlusMargin()*5);
}

//�t�H���g����Text�`��v���p�e�B���ύX���ꂽ�ꍇ�̏���
void	AView_Text::SPI_UpdateTextDrawProperty()
{
	//win
	AText	fontname;
	//#868 GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,fontname);
	GetTextDocumentConst().SPI_GetFontName(fontname);//#868
	
	AColor	color;
	GetModePrefDB().GetModeData_Color(AModePrefDB::kLetterColor,color);
	NVMC_SetDefaultTextProperty(
			fontname,//win
			GetTextDocumentConst().SPI_GetFontSize(),
			color,
			kTextStyle_Normal,
			GetTextDocument().SPI_IsAntiAlias());
	NVMC_GetMetricsForDefaultTextProperty(mLineHeight,mLineAscent);
	mLineMargin = GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetModeData_Number(AModePrefDB::kLineMargin);
	/*B0000 if( GetTextDocument().SPI_IsAntiAlias() == true && GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kInsertLineMarginForAntiAlias) == true )
	{
		mLineMargin++;
		mLineHeight++;
	}*/
	//
	SPI_UpdateScrollBarUnit();
	//LineHeight���ύX���ꂽ�̂�ImageSize���ύX�K�v
	SPI_UpdateImageSize();
	mZenkakuSpaceWidth = static_cast<ANumber>(NVMC_GetDrawTextWidth(mZenkakuSpaceText));
	//a�̕��������L�� #1186
	AText	achar("a");
	mACharWidth = static_cast<ANumber>(NVMC_GetDrawTextWidth(achar));
	//�u���v�̕��������L�� #1385 �}�X�ڕ\���ɂ͑S�p�X�y�[�X����Osaka�t�H���g���ŕs���m�ɂȂ�̂Łu���v���g���B
	AText	zenkakuachar;
	zenkakuachar.AddFromUCS4Char(0x3042);
	mZenkakuAWidth = NVMC_GetDrawTextWidth(zenkakuachar);
}

//#699
/**
�s�܂�Ԃ��v�Z�X���b�h���쒆�ipause���j����
*/
void	AView_Text::SPI_DoWrapCalculating()
{
	//�s�ԍ��{�^���̕\���X�V
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_UpdateLineNumberButton(NVI_GetControlID());
	}
}

//#699
/**
�s�܂�Ԃ��v�Z�X���b�h�v�Z����������
*/
void	AView_Text::SPI_DoWrapCalculated()
{
	/*
	//����AdjustScroll_Center()�ɂāAview�̃T�C�Y���󂾂�adjust���ꂸ�A�܂��A����������ƕςȈʒu��adjust����Ă��܂��̂ŁA
	//�\���ȃT�C�Y���Ƃ��Ă����B�i������300�ɂ��Ă����B����ŏォ��150�̂�����ɃL�����b�g������悤�ɂȂ�j
	if( NVI_GetWidth() == 0 )
	{
		NVI_SetSize(10000,300);
	}
	*/
	
	//�I��\����s
	if( mReservedSelection_StartTextIndex != kIndex_Invalid )
	{
		//�I��
		SetSelect(mReservedSelection_StartTextIndex,mReservedSelection_EndTextIndex);
		//�X�N���[������
		AdjustScroll_Center(mCaretTextPoint);
	}
	if( mReservedSelection_StartTextPoint.y != kIndex_Invalid )
	{
		if( mReservedSelection_IsSelectWithParagraph == true )
		{
			//�i��index�ɂ��I��\��
			AIndex	spos = GetTextDocumentConst().
					SPI_GetParagraphStartTextIndex(mReservedSelection_StartTextPoint.y) +
					mReservedSelection_StartTextPoint.x;
			AIndex	epos = GetTextDocumentConst().
					SPI_GetParagraphStartTextIndex(mReservedSelection_EndTextPoint.y) +
					mReservedSelection_EndTextPoint.x;
			SetSelect(spos,epos);
		}
		else
		{
			//text point�␳
			GetTextDocumentConst().SPI_CorrectTextPoint(mReservedSelection_StartTextPoint);
			GetTextDocumentConst().SPI_CorrectTextPoint(mReservedSelection_EndTextPoint);
			//�I��
			SetSelect(mReservedSelection_StartTextPoint,mReservedSelection_EndTextPoint);
		}
		//�X�N���[������
		AdjustScroll_Center(mCaretTextPoint);
	}
}

//#699
/**
�I��\�����
*/
void	AView_Text::CancelReservedSelection()
{
	mReservedSelection_StartTextIndex = kIndex_Invalid;
	mReservedSelection_EndTextIndex = kIndex_Invalid;
	mReservedSelection_StartTextPoint = kTextPoint_Invalid;
	mReservedSelection_EndTextPoint = kTextPoint_Invalid;
	mReservedSelection_IsSelectWithParagraph = false;
}

/**
�E�C���h�E�^�C�g���o�[�e�L�X�g�X�V
*/
void	AView_Text::SPI_UpdateWindowTitleBarText()
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_UpdateWindowTitleBarText();
	}
}

#pragma mark ===========================

#pragma mark ---�X�N���[��

//
void	AView_Text::NVIDO_ScrollPreProcess( const ANumber inDeltaX, const ANumber inDeltaY,
											const ABool inRedraw, const ABool inConstrainToImageSize,
											const AScrollTrigger inScrollTrigger )//#138 #1031
{
	//fprintf(stderr,"%16X NVIDO_ScrollPreProcess() - Start\n",(int)(GetObjectID().val));
	
//	TempHideCaret();
	
	//#138 �tⳎ��ҏW�E�C���h�E��Hide
	//#858 GetApp().SPI_GetFusenEditWindow().NVI_Hide(); 
	
	//#717
	//�⊮���͏�ԉ���
	ClearAbbrevInputMode(true,false);
	
	//fprintf(stderr,"%16X NVIDO_ScrollPreProcess() - End\n",(int)(GetObjectID().val));
}

//
void	AView_Text::NVIDO_ScrollPostProcess( const ANumber inDeltaX, const ANumber inDeltaY,
											 const ABool inRedraw, const ABool inConstrainToImageSize,
											 const AScrollTrigger inScrollTrigger )//#138 #1031
{
	//fprintf(stderr,"%16X NVIDO_ScrollPostProcess() - Start\n",(int)(GetObjectID().val));
	
	//X�����X�N���[���̂Ƃ��͑S�̍ĕ`�悳����
	//�i����p�[�g�\�����������̑΍�B�ǂ���X�����X�N���[���̏ꍇ�͑S�s�ĕ`��ɂȂ�̂ŁA�p�t�H�[�}���X�ւ̉e���͖����j#1042
	if( inDeltaX != 0 && inRedraw == true )
	{
		NVI_Refresh();
	}
	
	//#1031
	//�L�����b�g�̃��[�J���ʒu���Œ�
	//�X�N���[���o�[�܂��̓z�C�[���ɂ��X�N���[���̏ꍇ�A�L�����b�g�ʒu��O��ݒ肵���L�����b�g�̃��[�J�����W�ɌŒ肷��
	//���̃^�C�~���O�Ŏ��s����ƁA�X�N���[�����s�ƁA���ݍs�ړ��������ɕ`�悳���̂ł�����������Ȃ��B
	//�iSPI_ViewScrolled()������Ŏ��s���Ă��܂��ƁA���̒���NVMC_ExecuteDoDrawImmediately()�ŉ�ʃt���b�V�����������Ă���H�̂ł������������
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFixCaretLocalPoint) == true &&
		GetTextDocumentConst().SPI_IsDiffTargetDocument() == false ) //��r���r���[�ł̓L�����b�g�Œ肵�Ȃ�
	{
		switch(inScrollTrigger)
		{
		  case kScrollTrigger_ScrollBar:
		  case kScrollTrigger_Wheel:
		  case kScrollTrigger_ScrollKey:
			{
				//�O��ݒ肵���L�����b�g�̃��[�J�����W����L�����b�g�ݒ�i�X�N���[����Ȃ̂ŁA�L�����b�g�ړ�����B�j
				//�u�O��ݒ肵���L�����b�g�̃��[�J�����W�v�͍X�V���Ȃ�
				AImagePoint	ipt = {0};
				NVM_GetImagePointFromLocalPoint(mLastCaretLocalPoint,ipt);
				ATextPoint	textpt = {0};
				GetTextPointFromImagePoint(ipt,textpt);
				ALocalPoint	lpt = mLastCaretLocalPoint;
				SPI_SetSelect(textpt,textpt);
				mLastCaretLocalPoint = lpt;
				//DoDraw+��ʃt���b�V��
				NVMC_ExecuteDoDrawImmediately();
			}
			break;
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
	}
	
//	RestoreTempHideCaret();
	
	//#1031 AView::ScrollCore()����ړ�
	//AWindow�I�u�W�F�N�g��View���X�N���[�����ꂽ���Ƃ�`����
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ViewScrolled(NVI_GetControlID(),inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize);
	}
	
	if( mDisableAdjustDiffDisplayScrollAtScrollPostProcess == false )//#611
	{
		//#379
		//DiffView�𓯊��X�N���[������
		if( mTextWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_AdjustDiffDisplayScroll(/*#695 mTextDocumentID*/mTextDocumentRef.GetObjectID(),NVI_GetControlID(),kIndex_Invalid);
		}
	}
	//�|�b�v�A�b�v�E�C���h�E�̈ʒu�X�V
	AdjustPopupWindowsPosition();
	
	//fprintf(stderr,"%16X NVIDO_ScrollPostProcess() - End\n",(int)(GetObjectID().val));
}

//#379
/**
View�ŕ\�����Ă���ŏ��̒i�����擾����
*/
AIndex	AView_Text::SPI_GetViewStartParagraphIndex() const
{
	//ImageFrame�擾
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//�i���擾
	return GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(GetLineIndexByImageY(imageFrameRect.top));
}

//#379
/**
View�ŕ\�����Ă���Ō�̒i�����擾����
*/
AIndex	AView_Text::SPI_GetViewEndParagraphIndex() const
{
	//ImageFrame�擾
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//�i���擾
	return GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(GetLineIndexByImageY(imageFrameRect.bottom));
}

//#379
/**
�w��i���փX�N���[������
*/
void	AView_Text::SPI_ScrollToParagraph( const AIndex inStartParagraphIndex, const ANumber inOffset )
{
	AIndex	lineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(inStartParagraphIndex);
	AImagePoint	originImagePt = {0,0};
	originImagePt.y = GetImageYByLineIndex(lineIndex) + inOffset;
	if( originImagePt.y < 0 )   originImagePt.y = 0;
	NVI_ScrollTo(originImagePt);
}

//#450
/**
���݂̃X�N���[���ʒu�̊J�n�i�����擾
*/
AIndex	AView_Text::SPI_GetOriginParagraph() const
{
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	AIndex	lineIndex = GetLineIndexByImageY(imageFrameRect.top);
	return GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(lineIndex);
}

//#450
/**
�w��s�փX�N���[��
*/
void	AView_Text::SPI_ScrollToLine( const AIndex inStartLineIndex )
{
	AImagePoint	originImagePt = {0,0};
	originImagePt.y = GetImageYByLineIndex(inStartLineIndex);
	if( originImagePt.y < 0 )   originImagePt.y = 0;
	NVI_ScrollTo(originImagePt);
}

//#450
/**
view�t���[���̊J�n�s���擾
*/
AIndex	AView_Text::SPI_GetOriginLineIndex() const
{
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	AIndex	lineIndex = GetLineIndexByImageY(imageFrameRect.top);
	return lineIndex;
}

//
void	AView_Text::AdjustScroll( const ATextPoint& inTextPoint, const AIndex inOffset )//B0434
{
	//���݂̃t���[�����擾
	AImageRect	oldFrameImageRect;
	NVM_GetImageViewRect(oldFrameImageRect);
	//�t���[������Ȃ牽�����Ȃ� win
	if( oldFrameImageRect.left == oldFrameImageRect.right ||
				oldFrameImageRect.top == oldFrameImageRect.bottom )   return;
	//
	AImagePoint	newTopLeft;
	newTopLeft.x = oldFrameImageRect.left;
	newTopLeft.y = oldFrameImageRect.top;
	
	//�ʏ펞�̍s�̍����擾
	ANumber	normalLineHeight = mLineHeight+mLineMargin;
	//��ԉ���view���ǂ������擾
	ABool	isBottomView = false;
	if( mTextWindowID != kObjectID_Invalid )
	{
		isBottomView = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetIsBottomTextView(NVI_GetControlID());
	}
	//�����]���s�����v�Z
	ANumber	bottomMarginLineCount = 0;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kBottomScrollAt25Percent) == true &&
	   isBottomView == true )
	{
		ALocalRect	frameLocalRect = {0};
		NVM_GetLocalViewRect(frameLocalRect);
		bottomMarginLineCount = (frameLocalRect.bottom-frameLocalRect.top)/normalLineHeight/4;
	}
	//�㕔�E�����ҏW�}�[�W�����擾�i�����ҏW�}�[�W���ɂ͏�Ōv�Z���������]���s���𑫂��j
	ANumber	topEditMargin = inOffset;
	ANumber	bottomEditMargin = inOffset+1;
	bottomEditMargin += bottomMarginLineCount;
	
	
	//Y�����␳
	ATextPoint	pt = inTextPoint;
	//�w��|�C���g��image Y�̈ʒu��frame�����O�̎��͕␳
	if( GetImageYByLineIndex(pt.y) < oldFrameImageRect.top )
	{
		//�w��|�C���g��image Y���㕔�ҏW�}�[�W���̗]���̌�ɕ\�������悤�ɒ���
		newTopLeft.y = GetImageYByLineIndex(pt.y);
		newTopLeft.y -= topEditMargin*normalLineHeight;//B0434 2);
	}
	else if( GetImageYByLineIndex(pt.y+1) + bottomMarginLineCount*normalLineHeight >= oldFrameImageRect.bottom )
	{
		//�w��|�C���g��image Y�������ҏW�}�[�W���̗]���̑O�ɕ\�������悤�ɒ���
		//#450 ���肽���ݑΉ��ɂ��A�E�L�ł͓r���ɂ��肽���݂��L�����ꍇ�ɐ������Ȃ��̂ŉ��L�ɏC���B newTopLeft.y = GetImageYByLineIndex(pt.y-(endLineIndex-startLineIndex)+inOffset);//B0434 2);
		//pt.y+inOffset�s�ڂ�y�ʒu���ŉ��s�ɂȂ�悤�ȍ���origin���v�Z
		newTopLeft.y = GetImageYByLineIndex(pt.y+1);
		newTopLeft.y += bottomEditMargin*normalLineHeight;
		newTopLeft.y -= (oldFrameImageRect.bottom-oldFrameImageRect.top);
		if( newTopLeft.y < 0 )   newTopLeft.y = 0;
		//frame�̊J�n���s�̓r���ɂ��Ȃ��悤�ɕ␳����B
		AIndex	newTopLineIndex = GetLineIndexByImageY(newTopLeft.y);
		newTopLeft.y = GetImageYByLineIndex(newTopLineIndex);
	}
	
	//X�����␳
	AImagePoint	imagePoint;
	GetImagePointFromTextPoint(pt,imagePoint);
	if( imagePoint.x > oldFrameImageRect.right-5 )//#652
	{
		ANumber	delta = (imagePoint.x-oldFrameImageRect.right)/100+1;
		delta *= 100;
		newTopLeft.x = oldFrameImageRect.left + delta;
	}
	else if( imagePoint.x < oldFrameImageRect.left+5 )//#652
	{
		ANumber	delta = (oldFrameImageRect.left-imagePoint.x)/100+1;
		delta *= 100;
		newTopLeft.x = oldFrameImageRect.left -= delta;
	}
	if( newTopLeft.x < 0 )   newTopLeft.x = 0;
	//#531 �T�u�y�C���ōs�܂�Ԃ��h�L�������g�����E�X�N���[���ł���悤�ɂ��邽�� if( GetTextDocument().SPI_GetWrapMode() != kWrapMode_NoWrap )   newTopLeft.x = 0;
	
	//�X�N���[��
	NVI_ScrollTo(newTopLeft);
	
	//#1031
	//�L�����b�g�̃��[�J���ʒu���L��
	SetLastCaretLocalPoint();
}

void	AView_Text::SPI_AdjustScroll_Center()
{
	AdjustScroll_Center(GetCaretTextPoint());
}

//#454
/**
*/
void	AView_Text::SPI_AdjustScroll_Top()
{
	AdjustScroll_Top(GetCaretTextPoint());
}

/**
�^�C�v�w��AdjustScroll
*/
void	AView_Text::SPI_AdjustScroll( const AAdjustScrollType inType )
{
	switch(inType)
	{
	  case kAdjustScrollType_Top:
		{
			SPI_AdjustScroll_Top();
			break;
		}
	  case kAdjustScrollType_Center:
		{
			SPI_AdjustScroll_Center();
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

//#512
/**
�w��͈͂�AdjustScroll����
*/
void	AView_Text::SPI_AdjustScroll_Center( const AIndex inStartLineIndex, const AIndex inEndLineIndex, const ANumber inX )
{
	//���݂̃t���[�����擾
	AImageRect	oldFrameImageRect = {0,0,0,0};
	NVM_GetImageViewRect(oldFrameImageRect);
	//�t���[������Ȃ牽�����Ȃ� win
	if( oldFrameImageRect.left == oldFrameImageRect.right ||
				oldFrameImageRect.top == oldFrameImageRect.bottom )   return;
	//
	AImagePoint	newTopLeft = {0,0};
	newTopLeft.x = inX;
	newTopLeft.y = oldFrameImageRect.top;
	
	//�t���[���̊J�n�s�A�I���s���擾
	AIndex	frameStartLineIndex = GetLineIndexByImageY(oldFrameImageRect.top);
	AIndex	frameEndLineIndex = GetLineIndexByImageY(oldFrameImageRect.bottom);
	
	//1. �S�̕\���ł���Ȃ�A���Ԓn�_��Adjust_Center����
	if( frameEndLineIndex - frameStartLineIndex > inEndLineIndex - inStartLineIndex )
	{
		//Y�����␳
		if( inStartLineIndex <= frameStartLineIndex || inEndLineIndex >= frameEndLineIndex-1 )
		{
			AIndex	newStartLineIndex = (inStartLineIndex+inEndLineIndex)/2 - (frameEndLineIndex-frameStartLineIndex)/2;
			if( newStartLineIndex < 0 )   newStartLineIndex = 0;
			newTopLeft.y = GetImageYByLineIndex(newStartLineIndex);
		}
	
	}
	//2. �S�̕\���ł��Ȃ��Ȃ�A�ŏ��̍s��AdjustScroll_Top����
	else
	{
		//Y�����␳
		newTopLeft.y = GetImageYByLineIndex(inStartLineIndex);
	}
	
	//�X�N���[��
	NVI_ScrollTo(newTopLeft);
	
	//#1031
	//�L�����b�g�̃��[�J���ʒu���L��
	SetLastCaretLocalPoint();
}

//�L�����b�g�ʒu�����݂̃t���[���O�̏ꍇ�A�L�����b�g�ʒu���t���[�������t�߂ɗ���悤�ɃX�N���[���␳����B
void	AView_Text::AdjustScroll_Center( const ATextPoint& inTextPoint, const ABool inRedraw )
{
	//���݂̃t���[�����擾
	AImageRect	oldFrameImageRect;
	NVM_GetImageViewRect(oldFrameImageRect);
	//�t���[������Ȃ牽�����Ȃ� win
	if( oldFrameImageRect.left == oldFrameImageRect.right ||
				oldFrameImageRect.top == oldFrameImageRect.bottom )   return;
	//
	AImagePoint	newTopLeft;
	newTopLeft.x = oldFrameImageRect.left;
	newTopLeft.y = oldFrameImageRect.top;
	
	//�t���[���̊J�n�s�A�I���s���擾
	AIndex	startLineIndex = GetLineIndexByImageY(oldFrameImageRect.top);
	AIndex	endLineIndex = GetLineIndexByImageY(oldFrameImageRect.bottom);
	
	//Y�����␳
	if( inTextPoint.y <= startLineIndex || inTextPoint.y >= endLineIndex-1 )
	{
		/*#450 ���肽���ݑΉ��ɂ��A�E�L�ł͓r���ɂ��肽���݂��L�����ꍇ�ɐ������Ȃ��̂ŉ��L�ɏC���BAIndex	newStartLineIndex = GetCaretTextPoint().y - (endLineIndex-startLineIndex)/2;
		if( newStartLineIndex < 0 )   newStartLineIndex = 0;
		newTopLeft.y = GetImageYByLineIndex(newStartLineIndex);//#450 newStartLineIndex * GetLineHeightWithMargin();
		*/
		//inTextPoint.y�s�ڂ�y�ʒu���t���[�������ɂȂ�悤�ȍ���origin���v�Z
		newTopLeft.y = GetImageYByLineIndex(inTextPoint.y) - (oldFrameImageRect.bottom-oldFrameImageRect.top)/2;
		if( newTopLeft.y < 0 )   newTopLeft.y = 0;
		//frame�̊J�n���s�̓r���ɂ��Ȃ��悤�ɕ␳����B
		AIndex	newTopLineIndex = GetLineIndexByImageY(newTopLeft.y);
		newTopLeft.y = GetImageYByLineIndex(newTopLineIndex);
	}
	
	//X�����␳
	ATextPoint	selSpt, selEpt;
	SPI_GetSelect(selSpt,selEpt);
	if( selSpt.y == selEpt.y )
	{
		AImagePoint	imagePoint;
		GetImagePointFromTextPoint(selSpt,imagePoint);
		if( (imagePoint.x < oldFrameImageRect.left) || (oldFrameImageRect.right-10 < imagePoint.x) )
		{
			newTopLeft.x = imagePoint.x - (oldFrameImageRect.right-oldFrameImageRect.left)/2;
		}
	}
	if( newTopLeft.x < 0 )   newTopLeft.x = 0;
	
	//�X�N���[��
	NVI_ScrollTo(newTopLeft,inRedraw);
	
	//#1031
	//�L�����b�g�̃��[�J���ʒu���L��
	SetLastCaretLocalPoint();
}

//�L�����b�g�ʒu�����݂̃t���[���O�̏ꍇ�A�L�����b�g�ʒu���t���[���ŏ㕔�ɗ���悤�ɃX�N���[���␳����B
void	AView_Text::AdjustScroll_Top( const ATextPoint& inTextPoint )
{
	//���݂̃t���[�����擾
	AImageRect	oldFrameImageRect;
	NVM_GetImageViewRect(oldFrameImageRect);
	//�t���[������Ȃ牽�����Ȃ� win
	if( oldFrameImageRect.left == oldFrameImageRect.right ||
				oldFrameImageRect.top == oldFrameImageRect.bottom )   return;
	//
	AImagePoint	newTopLeft;
	newTopLeft.x = oldFrameImageRect.left;
	newTopLeft.y = oldFrameImageRect.top;
	
	//�t���[���̊J�n�s�A�I���s���擾
	AIndex	startLineIndex = GetLineIndexByImageY(oldFrameImageRect.top);
	AIndex	endLineIndex = GetLineIndexByImageY(oldFrameImageRect.bottom);
	
	//Y�����␳
	if( inTextPoint.y <= startLineIndex || inTextPoint.y >= endLineIndex )
	{
		newTopLeft.y = GetImageYByLineIndex(inTextPoint.y);//#450 inTextPoint.y * GetLineHeightWithMargin();
	}
	
	//X��0
	newTopLeft.x = 0;
	
	//�X�N���[��
	NVI_ScrollTo(newTopLeft);
	
	//#1031
	//�L�����b�g�̃��[�J���ʒu���L��
	SetLastCaretLocalPoint();
}

#pragma mark ===========================

#pragma mark ---�t�H�[�J�X

void	AView_Text::EVTDO_DoViewFocusActivated()
{
	/*#688 ���ꂪ����ƃ^�u�ؑ֎��ɐ�ɃL�����b�g���\������Ă��܂��B�܂��A���ɕK�v�Ƃ͎v���Ȃ��̂ŁA�폜����B
	if( IsCaretMode() == true )
	{
		ShowCaret();
	}
	*/
	/*������
	else
	{
		NVI_Refresh();
	}
	*/
	//#138
	//#858 SPI_UpdateFusen(false); 
	//#631
	//�c�[�����j���[�X�V
	GetApp().SPI_UpdateToolMenu(GetTextDocumentConst().SPI_GetModeIndex());
	
	//#725
	//���o�����X�g�^�C�g���X�V
	/*
	if( mTextWindowID != kObjectID_Invalid )
	{
		AText	text;
		//if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsSubTextView(NVI_GetControlID()) == false )
		{
			text.SetLocalizedText("SubWindowTitle_JumpList");
		}
		*
		else
		{
			text.SetLocalizedText("SubWindowTitle_JumpList_SubText");
		}
		*
		AText	title;
		NVM_GetWindow().NVI_GetTitle(NVM_GetWindow().NVI_GetCurrentTabIndex(),title);
		text.AddCstring("  (");
		text.AddText(title);
		text.AddCstring(")");
		GetApp().SPI_SetJumpListWindowTitle(mTextWindowID,text);
	}
	*/
	
	//#912
	//�����C���f���g�����ݒ�
	//�h�L�������g�Ő�������Ă���C���f���g�����^�C�v�����[�h�ݒ�ɐݒ肷��
	GetTextDocument().SPI_GuessIndentCharacterType();//#1003 �������܂߂Ď��s����B�iSPI_UpdateIndentCharacterTypeModePref()�����ŃR�[�������j
	//#1003 GetTextDocument().SPI_UpdateIndentCharacterTypeModePref();
}

void	AView_Text::EVTDO_DoViewFocusDeactivated()
{
	if( IsCaretMode() == true )
	{
		HideCaret(false,false);
	}
	/*������
	else
	{
		NVI_Refresh();
	}
	*/
	//#138
	//#858 SPI_UpdateFusen(false);
	//#81
	//���ۑ��f�[�^�̕ۑ��g���K�[
	GetTextDocument().SPI_AutoSave_Unsaved();
}

#pragma mark ===========================

#pragma mark ---�����E�u��

/**
���������i�C���^�[�t�F�C�X�j
*/
void	AView_Text::SPI_FindNext()
{
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	SPI_FindNext(param);
}
ABool	AView_Text::SPI_FindNext( const AFindParameter& inParam )
{
	//#1397
	//���������񂪋�̂Ƃ��͉����������^�[��
	if( inParam.findText.GetItemCount() == 0 )
	{
		return false;
	}
	
	//�L�[�L�^
	GetApp().SPI_RecordFindNext(inParam);
	
	//�ŋߎg���������E�u��������ɒǉ�
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	
	//�����n�C���C�g�ݒ�
	GetTextDocument().SPI_SetFindHighlight(inParam);
	
	//
	return FindNext(inParam);
}

/**
���������i�R�A�����j
*/
ABool	AView_Text::FindNext( const AFindParameter& inParam )
{
	//�s�܂�Ԃ��v�Z���͌����s�Ƃ��� #1117
	ADocumentInitState	docState = SPI_GetTextDocument().SPI_GetDocumentInitState();
	if( docState != kDocumentInitState_SyntaxRecognizing && docState != kDocumentInitState_Completed )
	{
		AText	title, message;
		title.SetLocalizedText("Notification_FindResult_LineCalc_Title");
		message.SetLocalizedText("Notification_FindResult_LineCalc");
		SPI_ShowGeneralNotificationPopupWindow(title,message);
		return false;
	}
	
	AIndex	selectSpos, selectEpos;
	GetSelect(selectSpos,selectEpos);
	
	AIndex	start = selectEpos;
	AIndex	end = GetTextDocumentConst().SPI_GetTextLength();
	
	//�Ԃ�l������
	ABool	result = false;
	//
	if( FindForward(inParam,start,end) == true )
	{
		result = true;
	}
	else
	{
		if( inParam.loop == true )
		{
			start = 0;
			end = selectEpos;
			if( FindForward(inParam,start,end) == true )
			{
				result = true;
			}
			else
			{
				ASoundWrapper::Beep();
				result = false;
			}
		}
		else
		{
			ASoundWrapper::Beep();
			result = false;
		}
	}
	//������Ȃ��ꍇ�́Anotification�\��
	if( result == false )
	{
		//#846
		//���[�_���Z�b�V����
		AStEditProgressModalSession	modalSession(kEditProgressType_Find,true,false,true);
		try
		{
			//�O�����Ɍ���
			AIndex	resultStart = 0, resultEnd = 0;
			ABool	modalSessionAborted = false;
			ABool	foundPrev = GetTextDocument().SPI_Find(inParam,true,0,selectSpos,resultStart,resultEnd,true,modalSessionAborted);
			ShowFindResultNotification(false,foundPrev,inParam.findText);
		}
		catch(...)
		{
			_ACError("",this);
		}
	}
	//
	return result;
}

/**
�ŏ����猟���i�C���^�[�t�F�C�X�{�R�A�����j
*/
ABool	AView_Text::SPI_FindFromFirst()
{
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	return SPI_FindFromFirst(param);
}
ABool	AView_Text::SPI_FindFromFirst( const AFindParameter& inParam )
{
	//#1397
	//���������񂪋�̂Ƃ��͉����������^�[��
	if( inParam.findText.GetItemCount() == 0 )
	{
		return false;
	}
	
	//�L�[�L�^
	GetApp().SPI_RecordFindFromFirst(inParam);
	
	//�ŋߎg���������E�u��������ɒǉ�
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	
	//�����n�C���C�g�ݒ�
	GetTextDocument().SPI_SetFindHighlight(inParam);
	
	if( FindForward(inParam,0,GetTextDocumentConst().SPI_GetTextLength()) == true )
	{
		return true;
	}
	else
	{
		ASoundWrapper::Beep();
		return false;
	}
}

/**
�O�������i�C���^�[�t�F�C�X�{�R�A�����j
*/
void	AView_Text::SPI_FindPrev()
{
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	SPI_FindPrevious(param);
}
ABool	AView_Text::SPI_FindPrevious( const AFindParameter& inParam )
{
	//#1397
	//���������񂪋�̂Ƃ��͉����������^�[��
	if( inParam.findText.GetItemCount() == 0 )
	{
		return false;
	}
	
	//�s�܂�Ԃ��v�Z���͌����s�Ƃ��� #1117
	ADocumentInitState	docState = SPI_GetTextDocument().SPI_GetDocumentInitState();
	if( docState != kDocumentInitState_SyntaxRecognizing && docState != kDocumentInitState_Completed )
	{
		AText	title, message;
		title.SetLocalizedText("Notification_FindResult_LineCalc_Title");
		message.SetLocalizedText("Notification_FindResult_LineCalc");
		SPI_ShowGeneralNotificationPopupWindow(title,message);
		return false;
	}
	
	//�L�[�L�^
	GetApp().SPI_RecordFindPrevious(inParam);
	
	//�ŋߎg���������E�u��������ɒǉ�
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	
	//�����n�C���C�g�ݒ�
	GetTextDocument().SPI_SetFindHighlight(inParam);
	
	AIndex	selectSpos, selectEpos;
	GetSelect(selectSpos,selectEpos);
	
	AIndex	start = 0;
	AIndex	end = selectSpos;
	
	//�Ԃ�l������
	ABool	result = false;
	//
	if( FindBackward(inParam,start,end) == true )
	{
		result = true;
	}
	else
	{
		if( inParam.loop == true )
		{
			start = selectSpos;
			end = GetTextDocumentConst().SPI_GetTextLength();
			if( FindBackward(inParam,start,end) == true )
			{
				result = true;
			}
			else
			{
				ASoundWrapper::Beep();
				result = false;
			}
		}
		else
		{
			ASoundWrapper::Beep();
			result = false;
		}
	}
	//������Ȃ��ꍇ�́Anotification�\��
	if( result == false )
	{
		//#846
		//���[�_���Z�b�V����
		AStEditProgressModalSession	modalSession(kEditProgressType_Find,true,false,true);
		try
		{
			//�������Ɍ���
			AIndex	resultStart = 0, resultEnd = 0;
			ABool	modalSessionAborted = false;
			ABool	foundNext = GetTextDocument().SPI_Find(inParam,false,selectEpos,GetTextDocumentConst().SPI_GetTextLength(),
														   resultStart,resultEnd,true,modalSessionAborted);
			ShowFindResultNotification(foundNext,false,inParam.findText);
		}
		catch(...)
		{
			_ACError("",this);
		}
	}
	//
	return result;
}

const AItemCount	kPreHitTextLength = 100;
const AItemCount	kPostHitTextLength = 100;

/**
�������X�g�i�C���^�[�t�F�C�X�{�R�A�����j
*/
void	AView_Text::SPI_FindList( const AFindParameter& inParam, const ABool inExtractMatchedText )
{
	//�ŋߎg���������E�u��������ɒǉ�
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	
	//�����n�C���C�g�ݒ�
	GetTextDocument().SPI_SetFindHighlight(inParam);
	
	//���ʕ\���E�C���h�E�\��
	AText	wintitle;
	wintitle.SetLocalizedText("IndexWindow_Title_FindResult");
	AText	grouptitle1, grouptitle2;
	grouptitle1.SetLocalizedText("FindResult_FindResult1");
	AText	datetime;
	ADateTimeWrapper::GetDateTimeText(datetime);
	grouptitle1.ReplaceParamText('0',datetime);
	grouptitle2.SetLocalizedText("FindResult_FindResult2");
	grouptitle2.ReplaceParamText('0',inParam.findText);
	AText	regexp;
	if( inParam.regExp == true )
	{
		regexp.SetLocalizedText("FindResult_RegExp");
	}
	grouptitle2.ReplaceParamText('1',regexp);
	//#92 ADocumentID	docID = GetApp().SPNVI_SelectOrCreateIndexWindowDocument(wintitle);
	//#92 �������ʂ��e�L�X�g�E�C���h�E���ɕ\������ݒ�ǉ�
	ADocumentID	docID = kObjectID_Invalid;
	//#725 if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFindResultInTextWindow) == false || mTextWindowID == kObjectID_Invalid )
	{
		//�ʃE�C���h�E�ɕ\��
		docID = GetApp().SPI_GetOrCreateFindResultWindowDocument();
	}
	/*#725
	else
	{
		//�e�L�X�g�E�C���h�E���ɕ\��
		docID = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetOrCreateFindResultDocument();
	}
	*/
	AFileAcc	basefolder;//#465
	GetTextDocumentConst().NVI_GetParentFolder(basefolder);//#465
	GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_InsertGroup(0,grouptitle1,grouptitle2,basefolder);//#465
	
	//
	AText	returntext;
	returntext.SetLocalizedText("ReturnCodeText");
	AFileAcc	file;
	GetTextDocumentConst().NVI_GetFile(file);
	AText	filepath;
	if( GetTextDocumentConst().NVI_IsFileSpecified() == true )//#221
	{
		file.GetPath(filepath);
	}
	//#221 �V�K�h�L�������g�̏ꍇ�́A�t�@�C���p�X�Ƀ^�u�R�[�h+DocumentID���i�[���邱�Ƃɂ���
	else
	{
		filepath.Add(kUChar_Tab);
		filepath.AddNumber(GetApp().NVI_GetDocumentUniqID(GetTextDocumentConst().GetObjectID()).val);//#693
	}
	
	//#937
	AText	extractedText;
	
	//#846
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_FindList,true,false,true);
	ABool	aborted = false;
	AItemCount	hitcount = 0;//R0238
	try
	{
		//
		AItemCount	len = GetTextDocumentConst().SPI_GetTextLength();
		for( ATextIndex textpos = 0; textpos < len;  )
		{
			//���[�_���Z�b�V�����p������
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_FindList,hitcount,true,textpos,len) == false )
			{
				//�L�����Z����
				aborted = true;
				break;
			}
			AIndex	resultStart, resultEnd;
			ABool	modalSessionAborted = false;
			if( GetTextDocument().SPI_Find(inParam,false,textpos,len,resultStart,resultEnd,true,modalSessionAborted) == true )
			{
				//��v�e�L�X�g���o
				if( inExtractMatchedText == true )
				{
					AText	t;
					if( inParam.regExp == true && inParam.replaceText.GetItemCount() > 0 )
					{
						GetTextDocumentConst().SPI_ChangeReplaceText(inParam.replaceText,t);
					}
					else
					{
						GetTextDocumentConst().SPI_GetText(resultStart,resultEnd,t);
					}
					extractedText.AddText(t);
				}
				//
				AText	hit, prehit, posthit, positiontext;
				GetTextDocumentConst().SPI_GetText(resultStart,resultEnd,hit);
				ATextIndex	pos = resultStart - kPreHitTextLength;
				if( pos < 0 )   pos = 0;
				pos = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(pos);
				GetTextDocumentConst().SPI_GetText(pos,resultStart,prehit);//B0350
				pos = resultEnd + kPostHitTextLength;
				if( pos >= len )   pos = len;
				pos = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(pos);
				GetTextDocumentConst().SPI_GetText(resultEnd,pos,posthit);
				hit.ReplaceChar(kUChar_CR,returntext);
				prehit.ReplaceChar(kUChar_CR,returntext);
				posthit.ReplaceChar(kUChar_CR,returntext);
				//�i��Index���擾
				ATextPoint	textpoint;
				GetTextDocumentConst().SPI_GetTextPointFromTextIndex(resultStart,textpoint);
				AIndex	paragraphIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(textpoint.y);
				//�ꏊ���擾
				GetTextDocumentConst().SPI_GetPositionTextByLineIndex(textpoint.y,positiontext);
				//#465
				AText	paratext;
				GetTextDocumentConst().SPI_GetParagraphTextByLineIndex(textpoint.y,paratext);
				//
				GetApp().SPI_GetIndexWindowDocumentByID(docID).
						SPI_AddItem_TextPosition(0,filepath,hit,prehit,posthit,positiontext,
												 paratext,//#465
												 resultStart,resultEnd-resultStart,paragraphIndex,GetEmptyText());
				if( textpos < resultEnd )//B0117
				{
					textpos = resultEnd;
				}
				else
				{
					textpos = GetTextDocumentConst().SPI_GetNextCharTextIndex(textpos);
				}
				hitcount++;//R0238
			}
			else
			{
				//�L�����Z���A�܂��́A�����s��v��
				//�L�����Z�����́AmodalSessionAborted��true�������Ă���
				aborted = modalSessionAborted;
				break;
			}
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	//R0238
	AText	text;
	if( aborted == false )
	{
		text.SetLocalizedText("FindList_ResultCount");
	}
	else
	{
		text.SetLocalizedText("FindList_Aborted");
	}
	AText	numtext;
	numtext.AddNumber(hitcount);
	text.ReplaceParamText('0',numtext);
	AText	filecount("1");
	text.ReplaceParamText('1',filecount);
	AText	f,prehit,posthit,positiontext;
	GetApp().SPI_GetIndexWindowDocumentByID(docID).
			SPI_AddItem_TextPosition(0,f,text,prehit,posthit,positiontext,
			GetEmptyText(),//#465
									 0,0,-1,GetEmptyText());
	//���o�e�L�X�g��V�K�h�L�������g�ɕ\��
	if( inExtractMatchedText == true )
	{
		if( extractedText.GetItemCount() > 0 )
		{
			ADocumentID docID = GetApp().SPNVI_CreateNewTextDocument(kStandardModeIndex);
			if( docID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,extractedText);
			}
		}
	}
}

/**
����u���i�C���^�[�t�F�C�X�j
*/
ABool	AView_Text::SPI_ReplaceNext( const AFindParameter& inParam )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return false;//B0000
	
	//�L�[�L�^ #887
	GetApp().SPI_RecordReplace(inParam,kReplaceRangeType_Next);
	
	//�ŋߎg���������E�u��������ɒǉ�
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	GetApp().SPI_AddRecentlyUsedReplaceText(inParam);
	
	//�����n�C���C�g�ݒ�
	GetTextDocument().SPI_SetFindHighlight(inParam);
	
	//�R�A����
	return ReplaceNext(inParam);
}

/**
����u���i�R�A�����j
*/
ABool	AView_Text::ReplaceNext( const AFindParameter& inParam )
{
	if( FindNext(inParam) == true )
	{
		AText	text;
		if( inParam.regExp == true )
		{
			GetTextDocumentConst().SPI_ChangeReplaceText(inParam.replaceText,text);
		}
		else
		{
			text.SetText(inParam.replaceText);
		}
		TextInput(undoTag_Replace1,text);
		return true;
	}
	else
	{
		return false;
	}
}

/**
�L�����b�g�ȍ~��u���i�C���^�[�t�F�C�X�j
*/
AItemCount	AView_Text::SPI_ReplaceAfterCaret( const AFindParameter& inParam )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return 0;//B0000
	
	//�L�[�L�^ #887
	GetApp().SPI_RecordReplace(inParam,kReplaceRangeType_AfterCaret);
	
	//�ŋߎg���������E�u��������ɒǉ�
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	GetApp().SPI_AddRecentlyUsedReplaceText(inParam);
	
	//�����n�C���C�g�ݒ�
	GetTextDocument().SPI_ClearFindHighlight();
	
	//
	ATextIndex	spos, epos;
	GetSelect(spos,epos);
	AItemCount	replacedCount;
	ABool	notaborted = Replace(inParam,epos,GetTextDocumentConst().SPI_GetTextLength(),replacedCount);
	//�u�����ʕ\�� #725
	ShowReplaceResultNotification(notaborted,replacedCount,inParam.findText,inParam.replaceText);
	
	return replacedCount;
}

/**
�I��͈͓���u���i�C���^�[�t�F�C�X�j
*/
AItemCount	AView_Text::SPI_ReplaceInSelection( const AFindParameter& inParam, const ABool inDontShowResultNotification )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return 0;//B0000
	
	//�L�[�L�^ #887
	GetApp().SPI_RecordReplace(inParam,kReplaceRangeType_InSelection);
	
	//�ŋߎg���������E�u��������ɒǉ�
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	GetApp().SPI_AddRecentlyUsedReplaceText(inParam);
	
	//�����n�C���C�g�ݒ�
	GetTextDocument().SPI_ClearFindHighlight();
	
	//
	if( mKukeiSelected == false )//#245
	{
		ATextIndex	spos, epos;
		GetSelect(spos,epos);
		AItemCount	replacedCount;
		ABool	notaborted = Replace(inParam,spos,epos,replacedCount);
		//�u�����ʕ\�� #725
		if( inDontShowResultNotification == false )
		{
			ShowReplaceResultNotification(notaborted,replacedCount,inParam.findText,inParam.replaceText);
		}
		if( notaborted == false )
		{
			return -1;
		}
		else
		{
			return replacedCount;
		}
	}
	else//#245 ��`�I�𒆂̏ꍇ�i���L�����͋�`�I�𒆂łȂ��Ă�����OK�����A�e���x�������邽�߂ɏ]���������g�����Ƃɂ����j
	{
		AArray<ATextIndex>	sposArray, eposArray;
		SPI_GetSelect(sposArray,eposArray);
		ABool	notaborted = true;
		AItemCount	totalRepleacedCount = 0;
		AItemCount	totalOffset = 0;
		for( AIndex i = 0; i < sposArray.GetItemCount(); i++ )
		{
			AItemCount	rc = 0;
			AItemCount	of = 0;
			notaborted = Replace(inParam,sposArray.Get(i)+totalOffset,eposArray.Get(i)+totalOffset,rc,of);
			totalRepleacedCount += rc;
			totalOffset += of;
			//�L�����Z������break
			if( notaborted == false )
			{
				break;
			}
		}
		//�u�����ʕ\�� #725
		if( inDontShowResultNotification == false )
		{
			ShowReplaceResultNotification(notaborted,totalRepleacedCount,inParam.findText,inParam.replaceText);
		}
		if( notaborted == false )
		{
			return -1;
		}
		else
		{
			return totalRepleacedCount;
		}
	}
}

/**
�S�Ă�u���i�C���^�[�t�F�C�X�j
*/
AItemCount	AView_Text::SPI_ReplaceAll( const AFindParameter& inParam )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return 0;//B0000
	
	//�L�[�L�^ #887
	GetApp().SPI_RecordReplace(inParam,kReplaceRangeType_All);
	
	//�ŋߎg���������E�u��������ɒǉ�
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	GetApp().SPI_AddRecentlyUsedReplaceText(inParam);
	
	//�����n�C���C�g�ݒ�
	GetTextDocument().SPI_ClearFindHighlight();
	
	//
	AItemCount	replacedCount;
	ABool	notaborted = Replace(inParam,0,GetTextDocumentConst().SPI_GetTextLength(),replacedCount);
	//�u�����ʕ\�� #725
	ShowReplaceResultNotification(notaborted,replacedCount,inParam.findText,inParam.replaceText);
	return replacedCount;
}

/**
�I���e�L�X�g�̂ݒu��
*/
ABool	AView_Text::SPI_ReplaceSelectedTextOnly( const AFindParameter& inParam )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return false;//B0000
	
	//�L�[�L�^ #887
	GetApp().SPI_RecordReplace(inParam,kReplaceRangeType_ReplaceSelectedTextOnly);
	
	//�ŋߎg���������E�u��������ɒǉ�
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	GetApp().SPI_AddRecentlyUsedReplaceText(inParam);
	
	//�����n�C���C�g�ݒ�
	GetTextDocument().SPI_ClearFindHighlight();
	
	//
	if( SPI_CompareFindParamAndSelectedText(inParam) == true )
	{
		ATextPoint	spt, ept;
		SPI_GetSelect(spt,ept);
		SetCaretTextPoint(spt);
		return ReplaceNext(inParam);
	}
	return false;
}

/**
�u���㎟������
*/
ABool	AView_Text::SPI_ReplaceAndFind( const AFindParameter& inParam )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return false;//B0000
	
	//�u�I���e�L�X�g�̂ݒu���v�{�u���������v�����s����B
	//�L�[�L�^�͂��ꂼ��̊֐����Ŏ��s�B
	
	//
	if( SPI_ReplaceSelectedTextOnly(inParam) == true )
	{
		return SPI_FindNext(inParam);
	}
	return false;
}

//#935
/**
�ꊇ�u��
*/
void	AView_Text::SPI_BatchReplace( const AFindParameter& inParam, const AText& inBatchText )
{
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_BatchReplace,true,false,true);
	//�p�����[�^�擾
	AFindParameter	param;
	param.Set(inParam);
	//�ꊇ�u���e�L�X�g�̍��ځi2�i����1���ځj���Ƃ̃��[�v
	AItemCount	len = inBatchText.GetItemCount();
	for( AIndex pos = 0; pos < len; )
	{
		//���[�_���Z�b�V�����p������
		if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_BatchReplace,0,true,pos,len) == false )
		{
			break;
		}
		
		//�����e�L�X�g�A�u���e�L�X�g�擾
		inBatchText.GetLine(pos,param.findText);
		inBatchText.GetLine(pos,param.replaceText);
		//�u�����s
		if( SPI_ReplaceInSelection(param,true) < 0 )
		{
			break;
		}
	}
}

//#725
/**
�u������notification�\��
*/
void	AView_Text::ShowReplaceResultNotification( const ABool inNotAborted, const ANumber inReplacedCount,
												  const AText& inFindText, const AText& inReplaceText )
{
	//�������ʕ\���I�v�V������OFF�Ȃ牽���������^�[�� #1322
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowFindResultPopup) == false )
	{
		return;
	}
	
	//
	if( mTextWindowID != kObjectID_Invalid )
	{
		SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_ReplaceResult(inNotAborted,inReplacedCount,inFindText,inReplaceText);
		SPI_ShowNotificationPopupWindow(true);
	}
}

//#725
/**
��������notification�\��
*/
void	AView_Text::ShowFindResultNotification( const ABool inFoundInNext, const ABool inFoundInPrev,
											   const AText& inFindText )
{
	//�������ʕ\���I�v�V������OFF�Ȃ牽���������^�[�� #1322
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowFindResultPopup) == false )
	{
		return;
	}
	
	//
	if( mTextWindowID != kObjectID_Invalid )
	{
		SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_FindResult(inFoundInNext,inFoundInPrev,inFindText);
		SPI_ShowNotificationPopupWindow(true);
	}
}

//�����e�L�X�g�ƁA�I�𒆂̓��e���r����i���ݑI�𒆂̓��e��������v������true��Ԃ��j
ABool	AView_Text::SPI_CompareFindParamAndSelectedText( const AFindParameter& inParam )
{
	ATextIndex	selStart, selEnd, resultStart, resultEnd;
	GetSelect(selStart,selEnd);
	if( selStart == selEnd )   return false;
	ABool	modalSessionAborted = false;
	if( GetTextDocument().SPI_Find(inParam,false,selStart,selEnd,resultStart,resultEnd,false,modalSessionAborted) == true )
	{
		return ((selStart==resultStart)&&(selEnd==resultEnd));
	}
	return false;
}

//inStart����inEnd�͈͓̔��Ō������A�ŏ��Ɍ����������̂�I������
ABool	AView_Text::FindForward( const AFindParameter& inParam, const AIndex inStart, const AIndex inEnd )
{
	AIndex	resultStart, resultEnd;
	//#846
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_Find,true,false,true);
	ABool	result = false;
	try
	{
		//
		ABool	modalSessionAborted = false;
		if( GetTextDocument().SPI_Find(inParam,false,inStart,inEnd,resultStart,resultEnd,true,modalSessionAborted) == true )
		{
			SetSelect(resultStart,resultEnd,true);//#262
			AdjustScroll_Center(GetCaretTextPoint());
			result = true;
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	return result;
}

//inStart����inEnd�͈͓̔��Ō������A�Ō�Ɍ����������̂�I������
ABool	AView_Text::FindBackward( const AFindParameter& inParam, const AIndex inStart, const AIndex inEnd )
{
	AIndex	resultStart, resultEnd;
	//#846
	//���[�_���Z�b�V����
	AStEditProgressModalSession	modalSession(kEditProgressType_Find,true,false,true);
	ABool	result = false;
	try
	{
		//
		ABool	modalSessionAborted = false;
		if( GetTextDocument().SPI_Find(inParam,true,inStart,inEnd,resultStart,resultEnd,true,modalSessionAborted) == true )
		{
			SetSelect(resultStart,resultEnd,true);//#262
			AdjustScroll_Center(GetCaretTextPoint());
			result = true;
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	return result;
	/*#789
	AIndex	resultStart, resultEnd;
	if( GetTextDocument().SPI_Find(inParam,inStart,inEnd,resultStart,resultEnd) == false )
	{
		return false;
	}
	else
	{
		AIndex	finalResultStart = resultStart;
		AIndex	finalResultEnd = resultEnd;
		AIndex	start = resultEnd;
		AIndex	end = inEnd;
		while( GetTextDocument().SPI_Find(inParam,start,end,resultStart,resultEnd) == true )
		{
			finalResultStart = resultStart;
			finalResultEnd = resultEnd;
			if( start == resultEnd )//B0117 �������[�v�h�~
			{
				resultEnd = GetTextDocumentConst().SPI_GetNextCharTextIndex(resultEnd);
				if( resultEnd >= GetTextDocumentConst().SPI_GetTextLength() )   break;
			}
			start = resultEnd;
		}
		SetSelect(finalResultStart,finalResultEnd,true);//#262
		AdjustScroll_Center(GetCaretTextPoint());
		return true;
	}
	*/
}

//�w��͈͓��u��
ABool	AView_Text::Replace( const AFindParameter& inParam, const ATextIndex inStart, const ATextIndex inEnd, AItemCount& outReplacedCount,
		AItemCount& outOffset )//#245
{
	//#65
	//�I���擾
	ATextIndex	selectSpos = 0, selectEpos = 0;
	GetSelect(selectSpos,selectEpos);
	//�u�����s
	ABool	result = GetTextDocument().SPI_ReplaceText(inParam,inStart,inEnd,outReplacedCount,outOffset,selectSpos,selectEpos);
	
	/*#65
	outOffset = 0;//#245
	outReplacedCount = 0;
	
	ATextIndex	selectSpos, selectEpos;
	GetSelect(selectSpos,selectEpos);
	
	ATextIndex	startPos = inStart;
	ATextIndex	endPos = inEnd;
	
	//�����K�v �I�������AHideCaret()�K�v�H
	
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Replace);
	
	//�P�ڂ̌���
	ATextIndex	replaceStart1, replaceEnd1;
	if( GetTextDocument().SPI_Find(inParam,false,startPos,endPos,replaceStart1,replaceEnd1) == false )
	{
		return;
	}
	
	//�u��������쐬
	AText	replaceText1;
	if( inParam.regExp == true )
	{
		GetTextDocumentConst().SPI_ChangeReplaceText(inParam.replaceText,replaceText1);
	}
	else
	{
		replaceText1.SetText(inParam.replaceText);
	}
	//���̌����J�n�ʒu
	if( replaceStart1 == replaceEnd1 )
	{
		if( replaceEnd1 >= endPos )
		{
			startPos = endPos+1;//���܂ł�������I��点��
		}
		else
		{
			startPos = GetTextDocumentConst().SPI_GetNextCharTextIndex(replaceEnd1);
		}
	}
	else
	{
		startPos = replaceEnd1;
	}
	
	//�Q�ڈȍ~�̌���
	AText	replaceText2;
	while(true) 
	{
		//���̌���
		ABool	found = false;
		ATextIndex	replaceStart2, replaceEnd2;
		if( startPos <= endPos )
		{
			found = GetTextDocument().SPI_Find(inParam,false,startPos,endPos,replaceStart2,replaceEnd2);
			//���̌����J�n�ʒu
			if( replaceStart2 == replaceEnd2 )
			{
				if( replaceEnd2 >= endPos )
				{
					startPos = endPos+1;//���܂ł�������I��点��
				}
				else
				{
					startPos = GetTextDocumentConst().SPI_GetNextCharTextIndex(replaceEnd2);
				}
			}
			else
			{
				startPos = replaceEnd2;
			}
		}
		else
		{
			replaceStart2 = replaceEnd2 = startPos;
		}
		
		//B0182 �܂�replaceText2�Ɏ��̒u����������쐬���ĕۑ�����
		//���̒u��������̍쐬
		replaceText2.DeleteAll();
		if( found == true )
		{
			if( inParam.regExp == true )
			{
				GetTextDocumentConst().SPI_ChangeReplaceText(inParam.replaceText,replaceText2);
			}
			else
			{
				replaceText2.SetText(inParam.replaceText);
			}
		}
		
		//�P�O�Ɍ������������̒u��
		DeleteTextFromDocument(replaceStart1,replaceEnd1,true);//dont redraw
		if( replaceText1.GetItemCount() > 0 )
		{
			InsertTextToDocument(replaceStart1,replaceText1,true);//#846
		}
		outReplacedCount++;
		//�u���ɂ�錟���͈͂̈ړ�
		AItemCount	offset = replaceText1.GetItemCount() - (replaceEnd1-replaceStart1);
		startPos += offset;
		endPos += offset;
		outOffset += offset;//#245
		
		if( replaceStart1 < selectSpos ) 	selectSpos += offset;
		if( replaceEnd1 <= selectEpos )  	selectEpos += offset;
		
		//replaceStart1/replaceEnd1�̍X�V
		replaceStart1 	= replaceStart2 + offset;
		replaceEnd1 	= replaceEnd2 + offset;
		
		if( found == false )   break;
		
		//���̒u���������replaceText1�ɃR�s�[
		replaceText1.SetText(replaceText2);
	}
	*/
	//#212 GetTextDocument().SPI_RefreshWindow();
	GetTextDocument().SPI_RefreshTextViews();
	//���X�̑I��͈͂�I��������
	SetSelect(selectSpos,selectEpos);
	
	return result;
}

//#724
/**
�u���������v�c�[���o�[���ڃN���b�N������
*/
void	AView_Text::SPI_FindSelectedNext()
{
	//�I���e�L�X�g�擾�i�L�����b�g���́A�P���I���j
	AText	text;
	GetSelectedText(text);
	if( mCaretMode == true )
	{
		ATextIndex	wordStart = 0, wordEnd = 0;
		GetTextDocumentConst().SPI_FindWord(GetTextDocument().SPI_GetTextIndexFromTextPoint(mCaretTextPoint),wordStart,wordEnd);
		GetTextDocumentConst().SPI_GetText(wordStart,wordEnd,text);
	}
	//Find�p�����[�^�ݒ�
	AFindParameter	param;
	param.findText.SetText(text);
	param.regExp = false;
	param.loop = false;
	//�������s
	SPI_FindNext(param);
}

//#724
/**
�u�O�������v�c�[���o�[���ڃN���b�N������
*/
void	AView_Text::SPI_FindSelectedPrevious()
{
	//�I���e�L�X�g�擾�i�L�����b�g���́A�P���I���j
	AText	text;
	GetSelectedText(text);
	if( mCaretMode == true )
	{
		ATextIndex	wordStart = 0, wordEnd = 0;
		GetTextDocumentConst().SPI_FindWord(GetTextDocument().SPI_GetTextIndexFromTextPoint(mCaretTextPoint),wordStart,wordEnd);
		GetTextDocumentConst().SPI_GetText(wordStart,wordEnd,text);
	}
	//Find�p�����[�^�ݒ�
	AFindParameter	param;
	param.findText.SetText(text);
	param.regExp = false;
	param.loop = false;
	//�������s
	SPI_FindPrevious(param);
}

#pragma mark ===========================

#pragma mark ---�⊮
//#717

/**
���A���^�C���⊮�������s
@note �����𖞂����ꍇ�̂݌������s����B
*/
void	AView_Text::FindCandidateRealtime()
{
	//�����͒���Ԃ̂Ƃ��́A���A���^�C����⌟���͂��Ȃ��i�V���Ȍ�⌟���͂��Ȃ��j
	if( mAbbrevInputMode == true )
	{
		return;
	}
	
	//���݂̃L�����b�g�ʒu�擾
	ATextIndex	textIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
	if( textIndex == 0 )   return;
	//���ݒP��擾
	ATextIndex	wordSpos = 0, wordEpos = 0;
	ATextIndex	prevCharPos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(textIndex);
	ABool	isAlphabet = GetTextDocumentConst().SPI_FindWordForKeyword(prevCharPos,wordSpos,wordEpos);
	AText	word;
	GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
	
	//==================�⊮�ĕϊ�==================
	//�Ō�ɕ⊮���͂Ŋm�肵���P��ƁA���ݒP�ꂪ�������́A�Ō�̕⊮���͏�Ԃ��p������i����A�V���Ȍ�⌟���͂��Ȃ��j
	if( mLastAbbrevInput_Word.GetItemCount() > 0 && mLastAbbrevInput_Word.Compare(word) == true )
	{
		mAbbrevInputMode = true;
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(true,mAbbrevInputMode);
		return;
	}
	
	//==================�⊮�����v�����邩�ǂ����̔���==================
	//��⌟�����邩�ǂ����̃t���O
	ABool	requestCandidateFinder = false;
	//�P�ꂪ2�����ȏ�Ȃ猟�����s
	if( wordEpos - wordSpos >= 2 && isAlphabet == true )
	{
		requestCandidateFinder = true;
	}
	//
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
	//�N���X�����̌�Ȃ猟�����s
	if( requestCandidateFinder == false )
	{
		//�N���X����(::��)�̌ォ�ǂ����𔻒f
		AItemCount	classlen = 0;
		if( syntaxDefinition.IsAfterClassString(GetTextDocumentConst().SPI_GetTextConst(),textIndex,classlen) == true )
		{
			requestCandidateFinder = true;
		}
		if( syntaxDefinition.IsAfterInstanceString(GetTextDocumentConst().SPI_GetTextConst(),textIndex,classlen) == true )
		{
			requestCandidateFinder = true;
		}
	}
	//��Ԃ��u�N���X��v�ł���΁A�������s
	if( requestCandidateFinder == false )
	{
		ATextPoint	textpt = {0};
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textIndex,textpt);
		AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(textpt);
		if( syntaxDefinition.GetSyntaxDefinitionState(stateIndex).IsAfterClassState() == true )
		{
			requestCandidateFinder = true;
		}
	}
	//==================��⌟���X���b�h���s==================
	if( requestCandidateFinder == true )
	{
		//fprintf(stderr,"RequestCandidateFinder(1.TextInput) ");
		RequestCandidateFinder();
	}
	//==================��⌟���X���b�h���s���Ȃ�==================
	else if( isAlphabet == false )
	{
		//�A���t�@�x�b�g�ȊO���͎��̓|�b�v�A�b�v����
		if( mTextWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(false,false);
		}
	}
}

/**
�⊮���p���ݒP��擾
*/
ABool	AView_Text::GetCurrentWordForAbbrev( AIndex& outWordSpos, AIndex& outWordEpos ) const
{
	//���ʏ�����
	outWordSpos = 0;
	outWordEpos = 0;
	//���݂̃L�����b�g�ʒu�擾
	ATextIndex	textIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
	//�h�L�������g�ŏ��Ȃ牽�����Ȃ�
	if( textIndex == 0 )   return false;
	//�P��擾
	ATextIndex	prevCharPos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(textIndex);
	ABool	isWord = GetTextDocumentConst().SPI_FindWordForKeyword(prevCharPos,outWordSpos,outWordEpos);
	//�P��łȂ��ꍇ�́A���ʂ̓L�����b�g�ʒu�ɂ���B
	if( isWord == false )
	{
		outWordSpos = outWordEpos = textIndex;
	}
	return true;
}

/**
�e�L�X�g����w��ʒu�̐e�L�[���[�h�擾
*/
ABool	AView_Text::GetParentKeyword( const AIndex inTextIndex, AText& outParentKeyword ) const
{
	//���ʏ�����
	outParentKeyword.DeleteAll();
	//
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
	//text point�擾
	ATextPoint	textpt = {0};
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inTextIndex,textpt);
	//��Ԏ擾
	AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(textpt);
	ABool	isAfterClassState = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).IsAfterClassState();
	/*
	if( isAfterClassState == true )
	{
		syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetAfterClassState_ParentKeyword(outParentKeyword);
		if( outParentKeyword.GetItemCount() > 0 )
		{
			//�L�[���[�h���X���b�h�Ɍ����v��
			GetApp().SPI_GetIdInfoFinderThread().SetRequestData(GetTextDocumentConst().GetObjectID(),
																kObjectID_Invalid,mTextWindowID,GetObjectID(),
																outParentKeyword,GetEmptyText(),0,GetEmptyText(),
																stateIndex);
			//�X���b�hunpause
			GetApp().SPI_GetIdInfoFinderThread().NVI_UnpauseIfPaused();
			return;
		}
	}
	*/
	AItemCount	classlen = 0;
	//==================�C���X�^���X�����̌�Ȃ璼�O�̃L�[���[�h�̃^�C�v��e�L�[���[�h�ɐݒ�==================
	ABool	afterInstanceString = false;
	if( isAfterClassState == false )
	{
		//�C���X�^���X����(.��)�̌ォ�ǂ����𔻒f
		afterInstanceString = syntaxDefinition.IsAfterInstanceString(GetTextDocumentConst().SPI_GetTextConst(),inTextIndex,classlen);
	}
	else
	{
		//��Ԃ��u�N���X��v�Ȃ��ɃC���X�^���X�����̌�Ƃ��Ĉ���
		afterInstanceString = true;
		classlen = 1;
	}
	if( afterInstanceString == true )
	{
		//�C���X�^���X�����̌�Ȃ�A�C���X�^���X�ϐ���type��parent keyword�Ƃ��Ď擾
		ATextIndex	p = inTextIndex-classlen-1;
		if( p >= 0 )
		{
			//GetApp().�Ȃǂ̏ꍇ�A()�̑O�̎��ʎq��Ώۂɂ���B
			AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(p);
			if( ch == ')' || ch == ']' || ch == '}' )
			{
				ATextPoint	sp, ep;
				GetTextDocumentConst().SPI_GetTextPointFromTextIndex(p,sp);
				ep = sp;
				GetTextDocumentConst().SPI_GetBraceSelection(sp,ep,true);
				ATextIndex	pp = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(sp) -2;
				if( pp >= 0 )   p = pp;
			}
			AText	instancetext;
			AIndex	s,e;
			GetTextDocumentConst().SPI_FindWordForKeyword(p,s,e);
			GetTextDocumentConst().SPI_GetText(s,e,instancetext);
			if( GetTextDocumentConst().SPI_GetTypeOfKeyword(instancetext,outParentKeyword,GetObjectID()) == true )
			{
				return true;
			}
		}
	}
	//==================�N���X�����̌�Ȃ璼�O�̃L�[���[�h��e�L�[���[�h�ɐݒ�==================
	ABool	afterClassString = false;
	if( isAfterClassState == false )
	{
		//�N���X����(::��)�̌ォ�ǂ����𔻒f
		afterClassString = syntaxDefinition.IsAfterClassString(GetTextDocumentConst().SPI_GetTextConst(),inTextIndex,classlen);
	}
	else
	{
		//��Ԃ��u�N���X��v�Ȃ��ɃN���X�����̌�Ƃ��Ĉ���
		afterClassString = true;
		classlen = 1;
	}
	if( afterClassString == true )
	{
		//�N���X�����̌�Ȃ�A�N���X��parent keyword�Ƃ��Ď擾
		ATextIndex	p = inTextIndex-classlen-1;
		if( p >= 0 )
		{
			AIndex	s,e;
			GetTextDocumentConst().SPI_FindWordForKeyword(p,s,e);//���Ƃ��΁AAText::�̂��ƂɃL�����b�g������ꍇ�At�̑O����word��������
			GetTextDocumentConst().SPI_GetText(s,e,outParentKeyword);
			return true;
		}
	}
	//==================���[�J���J�n���ʎq�̐e�L�[���[�h�i���T�^�I�ɂ͊֐��̃N���X�j���擾==================
	GetTextDocumentConst().SPI_GetCurrentLocalStartIdentifierParentKeyword(textpt.y,outParentKeyword);
	return false;
}

/**
�⊮��⌟���X���b�h�Ɍ����v��
*/
ABool	AView_Text::RequestCandidateFinder()
{
	//���ݒP��擾
	ATextIndex	wordSpos = 0, wordEpos = 0;
	if( GetCurrentWordForAbbrev(wordSpos,wordEpos) == false )
	{
		//���ݒP����擾�ł��Ȃ����́A���|�b�v�A�b�v�E�C���h�E���\���ɂ��ďI��
		if( mTextWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(false,false);
		}
		return false;
	}
	AText	word;
	GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
	
	//parent keyword�擾
	AText	parentKeyword;
	ABool	afterParentKeyword = GetParentKeyword(wordSpos,parentKeyword);
	//�������Ōp���N���X��parentKeywordArray�ɓ��ꂽ�����B�B�B
	ATextArray	parentKeywordArray;
	if( parentKeyword.GetItemCount() > 0 )
	{
		parentKeywordArray.Add(parentKeyword);
	}
	
	//�����v��
	if( afterParentKeyword == true || word.GetItemCount() > 0 )
	{
		//�e�L�[���[�h�̌�A�܂��́A�L�����b�g�ʒu�ɒP�ꂪ����ꍇ�̂݁A���v������
		RequestCandidateFinder(word,parentKeywordArray,wordEpos);
		return true;
	}
	else
	{
		return false;
	}
}
void	AView_Text::RequestCandidateFinder( const AText& inWord, const ATextArray& inParentWord, const ATextIndex inTextIndex )
{
	//suppress���Ȃ牽�����Ȃ�
	if( mSuppressRequestCandidateFinder == true )
	{
		return;
	}
	
	//��⌟�������e�L�X�g���L��
	mAbbrevCurrentRequestWord.SetText(inWord);
	
	//��Ԑݒ�i��ԁF�v�����j
	mAbbrevCandidateState_Requesting = true;
	mAbbrevCandidateState_Displayed = false;
	
	//�����v���f�[�^�ݒ�
	ATextPoint	textpt = {0};
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inTextIndex,textpt);
	AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(textpt);
	GetApp().SPI_GetCandidateFinderThread().SetRequestData(GetTextDocumentConst().GetObjectID(),GetObjectID(),
				mAbbrevCurrentRequestWord,inParentWord,stateIndex);
	//�X���b�hunpause
	GetApp().SPI_GetCandidateFinderThread().NVI_UnpauseIfPaused();
	
	//�V���Ȍ�⌟�����s�����̂ŁA�Ō�̕⊮���͊m��P�����������
	mLastAbbrevInput_Word.DeleteAll();
	mLastAbbrevInput_CompletionText.DeleteAll();
}

/**
�L�����b�g�ʒu�Ƀq���g�e�L�X�g�����݂��Ă�����q���g�e�L�X�g��e�L�[���[�h�Ƃ��Č�⌟���v������
*/
void	AView_Text::RequestCandidateFinderIfHintExist()
{
	//��ԁF�v�����A�\�����ł͂Ȃ��ꍇ�̂݌�⌟���v������
	if( mAbbrevCandidateState_Requesting == false && mAbbrevCandidateState_Displayed == false )
	{
		//�L�����b�g�ʒu�̃q���g�e�L�X�g�擾
		ATextIndex	textIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
		AText	hintText;
		GetTextDocumentConst().SPI_GetHintText(textIndex,hintText);
		//�q���g�e�L�X�g�̔��p�X�y�[�X�O�܂ł�e�L�[���[�h�Ƃ��Ďg�p����
		AIndex	pos = 0;
		if( hintText.FindCstring(0," ",pos) == true )
		{
			hintText.DeleteAfter(pos);
		}
		//�q���g�e�L�X�g���݂���΁A��⌟�����s
		if( hintText.GetItemCount() > 0 )
		{
			//�e�L�[���[�h�ɐݒ�
			ATextArray	parentWordArray;
			if( hintText.GetItemCount() > 0 )
			{
				parentWordArray.Add(hintText);
			}
			//��⌟���v��
			//fprintf(stderr,"RequestCandidateFinder(4.HintText) ");
			RequestCandidateFinder(GetEmptyText(),parentWordArray,textIndex);
		}
	}
}

/**
IdInfo�\�����ɁAIdInfo�̎��ʎq�̐e�L�[���[�h�ɑΉ�����L�[���[�h����\������ire-completion�j
*/
void	AView_Text::SPI_RequestCandidateFromParentKeyword( const ATextArray& inParentWord )
{
	//��ԁF�v�����A�\�����ł͂Ȃ��ꍇ�̂݌�⌟���v������
	if( mAbbrevCandidateState_Requesting == false && mAbbrevCandidateState_Displayed == false )
	{
		//SDF��re-completion�\��ԂƂ��Ē�`����Ă��邩���擾
		ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
		AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(mCaretTextPoint);
		if( syntaxDefinition.GetSyntaxDefinitionState(stateIndex).IsEnableRecompletion() == true )
		{
			//��łȂ�parent word�̂݁AparentWordArray�Ɋi�[
			ATextArray	parentWordArray;
			for( AIndex i = 0; i < inParentWord.GetItemCount(); i++ )
			{
				AText	parentWord;
				inParentWord.Get(i,parentWord);
				if( parentWord.GetItemCount() > 0 )
				{
					parentWordArray.Add(parentWord);
				}
			}
			//parentWordArray������Ό�⌟���v������
			if( parentWordArray.GetItemCount() > 0 )
			{
				ATextIndex	textIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
				//fprintf(stderr,"RequestCandidateFinder(3.IdInfoPopup(Parent Keyword match)) ");
				RequestCandidateFinder(GetEmptyText(),parentWordArray,textIndex);
			}
		}
	}
}

/**
�⊮���X���b�h���ʏ���
*/
void	AView_Text::SPI_DoCandidateFinderPaused()
{
	//�v����word�擾
	AText	word;
	ATextArray	parentword;
	GetApp().SPI_GetCandidateFinderThread().GetWordForResult(word,parentword);
	//���݂̌��index������
	mAbbrevCurrentCandidateIndex = kIndex_Invalid;
	//�Ō�̕ϊ����e�L�X�g������
	mLastAbbrevInput_Word.DeleteAll();
	mLastAbbrevInput_CompletionText.DeleteAll();
	//�������ʎ擾
	mAbbrevCandidateArray_Keyword.DeleteAll();
	mAbbrevCandidateArray_InfoText.DeleteAll();
	mAbbrevCandidateArray_CompletionText.DeleteAll();
	mAbbrevCandidateArray_CategoryIndex.DeleteAll();
	mAbbrevCandidateArray_ScopeType.DeleteAll();
	mAbbrevCandidateArray_FilePath.DeleteAll();
	mAbbrevCandidateArray_Priority.DeleteAll();
	mAbbrevCandidateArray_MatchedCount.DeleteAll();
	mAbbrevCandidateArray_ParentKeyword.DeleteAll();
	GetApp().SPI_GetCandidateFinderThread().
			GetResult(
					  mAbbrevCandidateArray_Keyword,mAbbrevCandidateArray_InfoText,mAbbrevCandidateArray_CategoryIndex,
					  mAbbrevCandidateArray_ScopeType,mAbbrevCandidateArray_FilePath,mAbbrevCandidateArray_Priority,
					  mAbbrevCandidateArray_ParentKeyword,mAbbrevCandidateArray_CompletionText,
					  mAbbrevCandidateArray_MatchedCount);
	//�������ʗL���ɂ�鏈������
	if( mAbbrevCandidateArray_Keyword.GetItemCount() >0 && mAbbrevCandidateState_Requesting == true )
	{
		//==================���L��==================
		
		//��⌋�ʂ�⊮��⃊�X�g�ɕ\��
		GetApp().SPI_UpdateCandidateListWindows(GetTextDocumentConst().SPI_GetModeIndex(),
					NVM_GetWindow().GetObjectID(),NVI_GetControlID(),
					mAbbrevCandidateArray_Keyword,mAbbrevCandidateArray_InfoText,mAbbrevCandidateArray_CategoryIndex,
					mAbbrevCandidateArray_ScopeType,mAbbrevCandidateArray_FilePath,mAbbrevCandidateArray_Priority,
					mAbbrevCandidateArray_ParentKeyword,mAbbrevCandidateArray_MatchedCount);
		
		if( mAbbrevInputMode == true )
		{
			//==================�����͒���Ԃ̏ꍇ==================
			//�ŏ��^�Ō�̌����e�L�X�g�ɓ���
			
			//���ݒP��͈͎擾
			ATextIndex	wordSpos = 0, wordEpos = 0;
			if( GetCurrentWordForAbbrev(wordSpos,wordEpos) == true )
			{
				AText	word;
				GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
				
				//�⊮���̓L�[���������񐔂ɏ]���āA���݂̍���index��ݒ�
				if( mAbbrevRequestInputKeyCount >= 0 )
				{
					//�v���X�E0�̏ꍇ
					mAbbrevCurrentCandidateIndex = mAbbrevRequestInputKeyCount;
					if( mAbbrevCurrentCandidateIndex >= mAbbrevCandidateArray_Keyword.GetItemCount() )
					{
						mAbbrevCurrentCandidateIndex = mAbbrevCandidateArray_Keyword.GetItemCount()-1;
					}
				}
				else
				{
					//�}�C�i�X�̏ꍇ
					mAbbrevCurrentCandidateIndex = mAbbrevCandidateArray_Keyword.GetItemCount() + mAbbrevRequestInputKeyCount;
					if( mAbbrevCurrentCandidateIndex < 0 )
					{
						mAbbrevCurrentCandidateIndex = 0;
					}
				}
				
				//��⍀�ڂ�keyword�擾
				AText	keyword;
				mAbbrevCandidateArray_Keyword.Get(mAbbrevCurrentCandidateIndex,keyword);
				
				if( keyword.Compare(word) == false )
				{
					//Undo�A�N�V�����^�O�L�^
					GetTextDocument().SPI_RecordUndoActionTag(undoTag_AbbrevInput);
					
					//���ݒP��I��
					SetSelect(wordSpos,wordEpos);
					//�I��͈͍폜�A�e�L�X�g�}��
					DeleteAndInsertTextToDocument(keyword);
				}
				
				//�Ō�ɕ⊮�����P����L��
				mLastAbbrevInput_Word.SetText(keyword);
				mAbbrevCandidateArray_CompletionText.Get(mAbbrevCurrentCandidateIndex,mLastAbbrevInput_CompletionText);
				
				//#750
				//���ݒP��n�C���C�g
				//�i�⊮�I���ԈႢ�h�~�ɂȂ�B�j
				UpdateCurrentWordHighlight(keyword);
			}
		}
		else
		{
			mAbbrevCurrentCandidateIndex = kIndex_Invalid;
		}
		
		//==================
		if( mTextWindowID != kObjectID_Invalid )
		{
			//���|�b�v�A�b�v�E�C���h�E�\��
			//��₪1�̂݁A���A�v�����̃��[�h�Ɠ����P��݂̂Ȃ�|�b�v�A�b�v��\��
			ABool	shouldPopup = true;
			if( mAbbrevCandidateArray_Keyword.GetItemCount() == 1 )
			{
				if( word.Compare(mAbbrevCandidateArray_Keyword.GetTextConst(0)) == true )
				{
					shouldPopup = false;
					//------------------�L�[���[�h����v��------------------
					RequestIdInfo(word,parentword,0,mCaretTextPoint);
				}
			}
			//���|�b�v�A�b�v�E�C���h�E�\���E��\��
			if( shouldPopup == true )
			{
				//���|�b�v�A�b�v�E�C���h�E�\��
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(true,mAbbrevInputMode);
			}
			else
			{
				//���|�b�v�A�b�v�E�C���h�E��\��
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(false,mAbbrevInputMode);
			}
		}
		
		//���E�C���h�E�̍��ڂ�I��
		GetApp().SPI_SetCandidateListWindowsSelectedIndex(NVM_GetWindow().GetObjectID(),mAbbrevCurrentCandidateIndex);
		//��ԁF�\����
		mAbbrevCandidateState_Requesting = false;
		mAbbrevCandidateState_Displayed = true;
	}
	else
	{
		//==================���Ȃ�==================
		
		//�����͏�ԉ���
		mAbbrevInputMode = false;
		
		if( mTextWindowID != kObjectID_Invalid )
		{
			//���E�C���h�E�̍��ڃN���b�N�ɂ����͂𖳌��ɂ���
			GetApp().SPI_InvalidateCandidateListWindowClick(mTextWindowID);
			//���|�b�v�A�b�v�E�C���h�E��\��
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(false,mAbbrevInputMode);
		}
		//��ԁF�A�C�h��
		mAbbrevCandidateState_Requesting = false;
		mAbbrevCandidateState_Displayed = false;
	}
}

/**
�⊮�L�[���͎�����
*/
ABool	AView_Text::InputAbbrev( const ABool inNext )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return false;
	
	//���݂̈ʒu�̒P��擾
	ATextIndex	wordSpos = 0, wordEpos = 0;
	if( GetCurrentWordForAbbrev(wordSpos,wordEpos) == false )   return false;
	AText	word;
	GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
	
	//==================�⊮�ĕϊ�==================
	//�Ō�ɕ⊮���͂Ŋm�肵���P��ƁA���ݒP�ꂪ�������́A�Ō�̕⊮���͏�Ԃ��p������
	if( mLastAbbrevInput_Word.GetItemCount() > 0 && mLastAbbrevInput_Word.Compare(word) == true )
	{
		mAbbrevInputMode = true;
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(true,mAbbrevInputMode);
	}
	
	if( mAbbrevInputMode == false )
	{
		//==================�����͒��t���OOFF�̏ꍇ==================
		//��⌟�������{����B
		//�����͒��t���O��ON�ɂ��Č�⌟���X���b�h�ɗv���𑗂�B
		//��⌟���X���b�h���牞�����������Ă����Ƃ��ɁA�����͒��t���O��ON�Ȃ̂ŁA�ŏ��̌�₪�h�L�������g�ɓ��͂����
		
		//���L�[���O�L�[�����L���B
		if( inNext == true )
		{
			mAbbrevRequestInputKeyCount = 0;
		}
		else
		{
			mAbbrevRequestInputKeyCount = -1;
		}
		//�e�L�X�g�}���ʒu�Ƀq���g�e�L�X�g������΍폜����
		ATextIndex	caretIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
		RemoveHintText(caretIndex);
		//�����͏�ԃt���OON�ɂ���
		mAbbrevInputMode = true;
		//��⌟�����s
		//fprintf(stderr,"RequestCandidateFinder(2.Abbrev key input) ");
		return RequestCandidateFinder();
	}
	else
	{
		//==================�����͒��t���OON�̏ꍇ==================
		
		if( mAbbrevCandidateState_Requesting == true )
		{
			//==================��⌟���v�����̏ꍇ==================
			//��⌟���X���b�h�v�����ɐ�ɕ⊮�L�[����͂��ꂽ�ꍇ�A�܂����f�[�^���o���Ă��Ȃ��̂ŁA���̏ꍇ�͉������Ȃ��B
			//�L�[���L�^
			if( inNext == true )
			{
				mAbbrevRequestInputKeyCount++;
			}
			else
			{
				mAbbrevRequestInputKeyCount--;
			}
		}
		else
		{
			//==================��⌟��������̏ꍇ==================
			//���L�[
			if( inNext == true )
			{
				//���index����
				mAbbrevCurrentCandidateIndex++;
				//�Ō�܂ł�������ŏ��ɖ߂�
				if( mAbbrevCurrentCandidateIndex >= mAbbrevCandidateArray_Keyword.GetItemCount() )
				{
					mAbbrevCurrentCandidateIndex = 0;
				}
			}
			//�O�L�[
			else
			{
				//���index�O��
				mAbbrevCurrentCandidateIndex--;
				//�ŏ��܂ł�������Ō�Ɉړ�
				if( mAbbrevCurrentCandidateIndex < 0 )
				{
					mAbbrevCurrentCandidateIndex = mAbbrevCandidateArray_Keyword.GetItemCount()-1;
				}
			}
			
			//�L�[���[�h�擾
			AText	keyword;
			mAbbrevCandidateArray_Keyword.Get(mAbbrevCurrentCandidateIndex,keyword);
			
			if( keyword.Compare(word) == false )
			{
				//Undo�A�N�V�����^�O�L�^
				if( GetTextDocument().SPI_GetCurrentUndoActionTag() != undoTag_AbbrevInput )
				{
					GetTextDocument().SPI_RecordUndoActionTag(undoTag_AbbrevInput);
				}
				
				//���ݒP��I��
				ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
				ATextIndex	spos = epos - mLastAbbrevInput_Word.GetItemCount();
				if( spos < 0 )   spos = 0;
				if( epos < 0 )   epos = 0;
				SetSelect(spos,epos);
				//�I��͈͍폜�A�e�L�X�g�}��
				DeleteAndInsertTextToDocument(keyword);
			}
			
			//�Ō�ɕ⊮�����P����L��
			mLastAbbrevInput_Word.SetText(keyword);
			mAbbrevCandidateArray_CompletionText.Get(mAbbrevCurrentCandidateIndex,mLastAbbrevInput_CompletionText);
			
			//#823
			//���ݒP��n�C���C�g
			//�i�⊮�I���ԈႢ�h�~�ɂȂ�B�j
			UpdateCurrentWordHighlight(keyword);
			
			//���E�C���h�E�̑I��index�X�V
			GetApp().SPI_SetCandidateListWindowsSelectedIndex(NVM_GetWindow().GetObjectID(),mAbbrevCurrentCandidateIndex);
		}
		return true;
	}
}

/**
�ȗ����͂̌���Index��TextView�̊O���i�Ⴆ�Ό��E�C���h�E�j���猈�߂�i�e�L�X�g�����̌�╶���ɓ���ւ��j
*/
void	AView_Text::SPI_SetCandidateIndex( const AIndex inIndex )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	//���index�ݒ�
	mAbbrevCurrentCandidateIndex = inIndex;
	//�L�[���[�h�擾
	AText	keyword;
	mAbbrevCandidateArray_Keyword.Get(mAbbrevCurrentCandidateIndex,keyword);
	
	//Undo�A�N�V�����^�O�L�^
	if( GetTextDocument().SPI_GetCurrentUndoActionTag() != undoTag_AbbrevInput )
	{
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_AbbrevInput);
	}
	
	//���݂̈ʒu�̒P��擾
	ATextIndex	wordSpos = 0, wordEpos = 0;
	if( GetCurrentWordForAbbrev(wordSpos,wordEpos) == false )   return;
	//���ݒP��I��
	SetSelect(wordSpos,wordEpos);
	//�I��͈͍폜�A�e�L�X�g�}��
	DeleteAndInsertTextToDocument(keyword);
	
	//�Ō�ɕ⊮�����P����L��
	mLastAbbrevInput_Word.SetText(keyword);
	mAbbrevCandidateArray_CompletionText.Get(mAbbrevCurrentCandidateIndex,mLastAbbrevInput_CompletionText);
}

/**
�����͏�ԉ����i�⊮�m��j
*/
void	AView_Text::ClearAbbrevInputMode( const ABool inHidePopupAlways, const ABool inDecideCompletion )
{
	mSuppressRequestCandidateFinder = true;
	
	//�����͒��ł���΁A����ŕ⊮�m��Ƃ������ƂȂ̂ŁA�⊮�����ɒǉ�
	if( mAbbrevInputMode == true )
	{
		/*
		ATextIndex	wordSpos = 0, wordEpos = 0;
		GetCurrentWordForAbbrev(wordSpos,wordEpos);
		AText	word;
		GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
		*/
		GetApp().SPI_AddToRecentCompletionWord(mLastAbbrevInput_Word);//word);
		//�L�[���[�h���\��
		IdInfoAuto();
		
		//�⊮�m�莞�e�L�X�g�}��
		if( inDecideCompletion == true && mLastAbbrevInput_CompletionText.GetItemCount() > 0 )
		{
			//���ݒP��I��
			ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
			ATextIndex	spos = epos - mLastAbbrevInput_Word.GetItemCount();
			if( spos < 0 )   spos = 0;
			if( epos < 0 )   epos = 0;
			SetSelect(spos,epos);
			//�I��͈͍폜�A�e�L�X�g�}��
			DeleteTextFromDocument();
			//�m�莞�e�L�X�g�}��
			AText	insText;
			insText.SetText(mLastAbbrevInput_CompletionText);
			SPI_DoTool_Text(insText,true);
		}
	}
	
	//
	if( mAbbrevInputMode == true || inHidePopupAlways == true )
	{
		if( mTextWindowID != kObjectID_Invalid )
		{
			//���E�C���h�E�̍��ڃN���b�N�𖳌�������
			GetApp().SPI_InvalidateCandidateListWindowClick(mTextWindowID);
			//���|�b�v�A�b�v�E�C���h�E���\��
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(false,mAbbrevInputMode);
		}
	}
	
	//�����͏�ԉ���
	mAbbrevInputMode = false;
	//��ԁF�A�C�h��
	mAbbrevCandidateState_Requesting = false;
	mAbbrevCandidateState_Displayed = false;
	
	//
	mSuppressRequestCandidateFinder = false;
	//�⊮�m�莞�́A�q���g�e�L�X�g���݂��Ă�����A�q���g�e�L�X�g��e�L�[���[�h�Ƃ����⌟�����s
	if( inDecideCompletion == true )
	{
		RequestCandidateFinderIfHintExist();
	}
}

#pragma mark ===========================

#pragma mark ---�L�[���[�h���
//#853

/**
�L�[���[�h���\��
*/
void	AView_Text::DisplayIdInfo( const ABool inDisplayAlways, const AIndex inTextIndex, const AIndex inArgIndex )
{
	//�P��ʒu�擾
	AIndex	wordSpos = 0, wordEpos = 0;
	if( inTextIndex == kIndex_Invalid )
	{
		//���ݑI���擾
		GetSelect(wordSpos,wordEpos);
		//�I��͈͖����Ȃ猻�ݒP����擾
		if( wordSpos == wordEpos )
		{
			if( GetTextDocumentConst().SPI_FindWordForKeyword(wordSpos,wordSpos,wordEpos) == false )
			{
				return;
			}
		}
	}
	else
	{
		//�w��text index�̎���̒P����擾
		if( GetTextDocumentConst().SPI_FindWordForKeyword(inTextIndex,wordSpos,wordEpos) == false )
		{
			return;
		}
	}
	//�P��擾
	AText	word;
	GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
	//parent keyword�擾
	AText	parentKeyword;
	GetParentKeyword(wordSpos,parentKeyword);
	//�������Ōp���N���X��parentKeywordArray�ɓ��ꂽ�����B�B�B
	ATextArray	parentKeywordArray;
	if( parentKeyword.GetItemCount() > 0 )
	{
		parentKeywordArray.Add(parentKeyword);
	}
	//id info�����v��
	ATextPoint	textPoint = {0};
	GetTextDocument().SPI_GetTextPointFromTextIndex(wordSpos,textPoint);
	RequestIdInfo(word,parentKeywordArray,inArgIndex,textPoint);
}

/**
id info�����v��
*/
void	AView_Text::RequestIdInfo( const AText& inWord, const ATextArray& inParentWord, const AIndex inArgIndex,
								  const ATextPoint& inTextPoint )
{
	//���݂̏ꏊ��funcId���擾
	AText	currentFuncFuncname, currentFuncClassname;
	ATextPoint	currentFuncMenuidentifierSpt = {0}, currentFuncMenuidentifierEpt = {0};
	AIndex	categoryIndex = kIndex_Invalid;
	GetTextDocument().SPI_GetLocalStartIdentifierDataByLineIndex(inTextPoint.y,currentFuncFuncname,currentFuncClassname,
																 categoryIndex,currentFuncMenuidentifierSpt,currentFuncMenuidentifierEpt);
	AText	filepath;
	GetTextDocument().NVI_GetFilePath(filepath);
	AText	callerFuncIdText;
	AView_CallGraf::SPI_GetFuncIdText(filepath,currentFuncClassname,currentFuncFuncname,callerFuncIdText);
	//�L�[���[�h���X���b�h�Ɍ����v��
	AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(inTextPoint);
	GetApp().SPI_GetIdInfoFinderThread().SetRequestData(GetTextDocumentConst().GetObjectID(),
														kObjectID_Invalid,mTextWindowID,GetObjectID(),
														inWord,inParentWord,inArgIndex,callerFuncIdText,
														stateIndex);
	//�X���b�hunpause
	GetApp().SPI_GetIdInfoFinderThread().NVI_UnpauseIfPaused();
}

/**
�L�[����"("���ɂ�鎩��IdInfo�\��
*/
void	AView_Text::IdInfoAuto()
{
	//�L�����b�g�ʒu�擾
	AIndex	spos, epos;
	GetSelect(spos,epos);
	//�L�����b�g�ʒu���O�ŃA���t�@�x�b�g�����݂���ʒu���擾����
	for( spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos); spos > 0; spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos) )
	{
		if( GetTextDocumentConst().SPI_IsAsciiAlphabet(spos) == true )
		{
			break;
		}
		//#955
		//�}���`�o�C�g�̏ꍇ�̓��^�[��
		AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(spos);
		if( IsUTF8Multibyte(ch) == true )
		{
			return;
		}
	}
	//�L�[���[�h���\���i����index=0�j
	DisplayIdInfo(false,spos,0);
}

/**
�L�[����","���ɂ�鎩��IdInfo�\��
*/
void	AView_Text::IdInfoAutoArg()
{
	//�L�����b�g�ʒu�擾
	AIndex	spos = 0, epos = 0;
	GetSelect(spos,epos);
	//����index�擾
	AIndex	funcPos = kIndex_Invalid;
	AIndex	argIndex = kIndex_Invalid;
	AIndex	argSpos = 0, argEpos = 0;
	GetIdInfoArgIndex(spos,funcPos,argIndex,argSpos,argEpos);
	//�L�[���[�h���\���i�����w��j
	DisplayIdInfo(false,funcPos,argIndex);
}

/**
*/
void	AView_Text::IdInfoHighlightArg( const AIndex inTextIndex )
{
	AIndex	funcPos = 0;
	AIndex	argIndex = 0;
	AIndex	argSpos = kIndex_Invalid, argEpos = kIndex_Invalid;
	GetIdInfoArgIndex(inTextIndex,funcPos,argIndex,argSpos,argEpos);
	//
	AIndex	wordSpos = 0, wordEpos = 0;
	GetTextDocumentConst().SPI_FindWordForKeyword(funcPos,wordSpos,wordEpos);
	//
	AText	funcText;
	GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,funcText);
	//
	if( GetApp().SPI_HighlightIdInfoArgIndex(NVM_GetWindow().GetObjectID(),funcText,argIndex) == true &&
				argSpos != kIndex_Invalid && argEpos != kIndex_Invalid )
	{
		if( mIdInfoCurrentArgHighlighted == true )
		{
			mIdInfoCurrentArgHighlighted = false;
			SPI_RefreshLines(mIdInfoCurrentArgStartTextPoint.y,mIdInfoCurrentArgStartTextPoint.y+1);
		}
		mIdInfoCurrentArgHighlighted = true;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(argSpos,mIdInfoCurrentArgStartTextPoint);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(argEpos,mIdInfoCurrentArgEndTextPoint);
		SPI_RefreshLines(mIdInfoCurrentArgStartTextPoint.y,mIdInfoCurrentArgStartTextPoint.y+1);
	}
	else
	{
		if( mIdInfoCurrentArgHighlighted == true )
		{
			mIdInfoCurrentArgHighlighted = false;
			SPI_RefreshLines(mIdInfoCurrentArgStartTextPoint.y,mIdInfoCurrentArgStartTextPoint.y+1);
		}
	}
}

/**
�L�[���[�h���p ����index, �����ʒu(text index)�擾
*/
void	AView_Text::GetIdInfoArgIndex( const AIndex inTextIndex, AIndex& outFuncPos, AIndex& outArgIndex,
		AIndex& outArgStartTextIndex, AIndex& outArgEndTextIndex ) const
{
	const ADocument_Text& document = GetTextDocumentConst();
	//���ʏ�����
	outFuncPos = inTextIndex;
	outArgIndex = 0;
	outArgStartTextIndex = kIndex_Invalid;
	outArgEndTextIndex = kIndex_Invalid;
	//syntax def�擾
	const ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
	//�����J�n���������݂��Ȃ��Ȃ炷���ɏI���i���ꂪ�Ȃ��ƍŏ��܂�break���Ȃ��j
	if( syntaxDefinition.GetIdInfoArgStartCharCount() == 0 )   return;
	//�����J�nindex�擾
	AIndex	searchStartIndex = inTextIndex-1000;
	if( searchStartIndex < 0 )
	{
		searchStartIndex = 0;
	}
	//�����I��index�擾
	AIndex	searchEndIndex = inTextIndex+1000;
	if( searchEndIndex >= GetTextDocumentConst().SPI_GetTextLength() )
	{
		searchEndIndex = GetTextDocumentConst().SPI_GetTextLength();
	}
	//�L�����b�g�ʒu���O��'('������������
	AIndex	level = 0;
	for( outFuncPos = document.SPI_GetPrevCharTextIndex(inTextIndex); 
				outFuncPos > searchStartIndex; 
				outFuncPos = document.SPI_GetPrevCharTextIndex(outFuncPos) )
	{
		AUChar	ch = document.SPI_GetTextChar(outFuncPos);
		//level��0�̂Ƃ��̂�
		if( level == 0 )
		{
			//IsIdInfoStartChar�iC����̏ꍇ��"("�j�Ȃ�����P�O�ɖ߂��ďI��
			if( syntaxDefinition.IsIdInfoStartChar(ch) == true )
			{
				//�����J�n�ʒu���ݒ�̏ꍇ�́A�����J�n�ʒu�Ƃ��Đݒ�
				if( outArgStartTextIndex == kIndex_Invalid )
				{
					outArgStartTextIndex = document.SPI_GetNextCharTextIndex(outFuncPos);
				}
				//�֐��ʒu�ݒ�A�����I��
				outFuncPos = document.SPI_GetPrevCharTextIndex(outFuncPos);
				break;
			}
			//IsIdInfoDelimiterChar�iC����̏ꍇ��","�j�Ȃ�argindex��+1���Čp��
			if( syntaxDefinition.IsIdInfoDelimiterChar(ch) == true )
			{
				//�����J�n�ʒu���ݒ�̏ꍇ�́A�����J�n�ʒu�Ƃ��Đݒ�
				if( outArgStartTextIndex == kIndex_Invalid )
				{
					outArgStartTextIndex = document.SPI_GetNextCharTextIndex(outFuncPos);
				}
				//����index�C���N�������g
				outArgIndex++;
			}
		}
		//()���𖳎����邽�߂�level���㉺
		if( syntaxDefinition.IsIdInfoStartChar(ch) == true )   level--;
		if( syntaxDefinition.IsIdInfoEndChar(ch) == true )   level++;
	}
	//�L�����b�g�ʒu�����','������������
	level = 0;
	for( AIndex pos = inTextIndex; 
				pos < searchEndIndex; 
				pos = document.SPI_GetNextCharTextIndex(pos) )
	{
		AUChar	ch = document.SPI_GetTextChar(pos);
		//level��0�̂Ƃ��̂�
		if( level == 0 )
		{
			//')', ','�Ȃ�A�����I���ʒu�ݒ�
			if( syntaxDefinition.IsIdInfoEndChar(ch) == true || syntaxDefinition.IsIdInfoDelimiterChar(ch) == true )
			{
				outArgEndTextIndex = pos;
				break;
			}
		}
		//()���𖳎����邽�߂�level���㉺
		if( syntaxDefinition.IsIdInfoStartChar(ch) == true )   level++;
		if( syntaxDefinition.IsIdInfoEndChar(ch) == true )   level--;
	}
}

/**
�|�b�v�A�b�v�E�C���h�E�ʒu����
*/
void	AView_Text::AdjustPopupWindowsPosition()
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_AdjustPopupWindowsPosition();
	}
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�J�E���g
//#142

/**
�e�L�X�g�J�E���g�E�C���h�E�\���X�V�i�I��͈̓J�E���g�A���ݕ����j
*/
void	AView_Text::UpdateTextCountWindows()
{
	//doc info�E�C���h�E��������Ή������Ȃ�
	if( GetApp().SPI_DocInfoWindowExist(NVM_GetWindow().GetObjectID()) == false )
	{
		return;
	}
	
	//�I��͈͂̕��������J�E���g
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetSelect(spt,ept);
	AItemCount	selectedCharCount = 0, selectedWordCount = 0, paragraphCount = 0;
	GetTextDocumentConst().SPI_GetWordCount(spt,ept,selectedCharCount,selectedWordCount,paragraphCount);
	//#1123
	//��`�I�𒆂͒i�����Ƃ��āA��`�s����\������
	if( mKukeiSelected == true )
	{
		paragraphCount = mKukeiSelected_Start.GetItemCount();
	}
	AIndex	startIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
	//�P��6�����Ȃ�I���e�L�X�g�F�Ƃ��Ď擾
	AText	selectedText;
	AIndex	s = 0, e = 0;
	GetTextDocumentConst().SPI_FindWordForKeyword(startIndex,s,e);
	if( e-s < 256 )
	{
		GetTextDocumentConst().SPI_GetText(s,e,selectedText);
	}
	//�e�L�X�g�J�E���g�E�C���h�E�X�V
	GetApp().SPI_UpdateDocInfoWindows_SelectedCount(NVM_GetWindow().GetObjectID(),
				selectedCharCount, selectedWordCount, paragraphCount,selectedText);
	//sdf debug
	AText	debugtext;
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )
	{
		AText	stateText,pushedStateText;
		AIndex	stateIndex = 0, pushedStateIndex = 0;
		GetTextDocumentConst().SPI_GetCurrentStateIndexAndName(GetCaretTextPoint(),
					stateIndex,pushedStateIndex,stateText,pushedStateText);
		AText	t;
		t.SetFormattedCstring("State:%d(",stateIndex);
		t.AddText(stateText);
		t.AddFormattedCstring(")   PushedState:%d(",pushedStateIndex);
		t.AddText(pushedStateText);
		AItemCount	indentCount = GetTextDocumentConst().SPI_GetTextInfoIndentCount(GetCaretTextPoint().y);
		t.AddFormattedCstring(")   Indent:%d \r",indentCount);
		t.AddFormattedCstring("Directive Level:%d ",GetTextDocument().ForDebug_GetTextInfo().GetDirectiveLevel(GetCaretTextPoint().y));
		debugtext.SetText(t);
	}
	//���ݕ����擾
	AUCS4Char	ch = 0, decomp = 0;
	GetTextDocumentConst().SPI_GetText1UCS4Char(startIndex,ch,decomp);
	//#906
	//���o���p�X�\��
	//�O��擾���̌��o��index���擾���A�Ⴄ�ꍇ�̂݌��o���p�X�X�V
	AIndex	jumpMenuItemIndex = GetTextDocumentConst().SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
	if( jumpMenuItemIndex != mCurrentHeaderPathJumpMenuItemIndex )
	{
		//���o���p�X�擾
		mCurrentHeaderPathJumpMenuItemIndex = jumpMenuItemIndex;
		GetTextDocumentConst().SPI_GetHeaderPathArray(spt.y,mCurrentHeaderPathArray_HeaderTextArray,mCurrentHeaderPathArray_HeaderParagraphIndexArray);
	}
	//���ݕ�����doc info�E�C���h�E�ɐݒ�
	GetApp().SPI_UpdateDocInfoWindows_CurrentChar(NVM_GetWindow().GetObjectID(),ch,decomp,
				mCurrentHeaderPathArray_HeaderTextArray,mCurrentHeaderPathArray_HeaderParagraphIndexArray,debugtext);
}


#pragma mark ===========================

#pragma mark ---���ݒP��}�[�J�[
//#823

/**
���ݒP��n�C���C�g�ݒ�
*/
void	AView_Text::SetCurrentWordHighlight()
{
	//���ݒP��n�C���C�g���Ȃ��ݒ�Ȃ�A���ݒP��n�C���C�g�N���A�݂̂��āA�I��
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayCurrentWordHighlight) == false )
	{
		GetTextDocument().SPI_ClearCurrentWordForHighlight(true);
		return;
	}
	
	//�I��͈͎擾
	AIndex	selSpos = 0, selEpos = 0;
	GetSelect(selSpos,selEpos);
	//�P��
	AText	word;
	//
	if( selSpos != selEpos )
	{
		//�I�𕶎��񂪑��݂���ꍇ
		
		//�I��͈͊J�n�ʒu���ӂ̒P��͈͎擾
		AIndex	spos1 = 0, epos1 = 0;
		GetTextDocumentConst().SPI_FindWordForKeyword(selSpos,spos1,epos1);
		//�I��͈͏I���ʒu���ӂ̒P��͈͎擾
		AIndex	spos2 = 0, epos2 = 0;
		GetTextDocumentConst().SPI_FindWordForKeyword(GetTextDocumentConst().SPI_GetPrevCharTextIndex(selEpos),spos2,epos2);
		
		//��L��2�P��̊J�n�A�I���܂ł̃e�L�X�g���擾
		GetTextDocumentConst().SPI_GetText(spos1,epos2,word);
	}
	else
	{
		//�I�𕶎��񂪑��݂��Ȃ��ꍇ
		
		//���ݒP��擾
		ATextIndex	spos = 0, epos = 0;
		if( GetTextDocumentConst().SPI_FindWordForKeyword(selSpos,spos,epos) == true )
		{
			GetTextDocumentConst().SPI_GetText(spos,epos,word);
		}
	}
	//�P����n�C���C�g�Ƃ��Đݒ�
	UpdateCurrentWordHighlight(word);
	
}

/**
���ݒP��n�C���C�g�X�V
*/
void	AView_Text::UpdateCurrentWordHighlight( const AText& inWord )
{
	//�h�L�������g�Ɍ��ݒP��ݒ�
	GetTextDocument().SPI_SetCurrentWordForHighlight(inWord,true,true);
	//���C���^�T�u�A�����Е��̌��ݒP��ݒ�
	if( mTextWindowID != kObjectID_Invalid )
	{
		ADocumentID	mainDocID = GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_GetCurrentDocumentID();
		ADocumentID	subDocID = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetCurrentSubTextPaneDocumentID();
		if( mainDocID == GetTextDocument().GetObjectID() && subDocID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(subDocID).SPI_SetCurrentWordForHighlight(inWord,true,true);
		}
		else if( mainDocID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(mainDocID).SPI_SetCurrentWordForHighlight(inWord,true,true);
		}
	}
}

#pragma mark ===========================

#pragma mark ---���[�Y���X�g
//#723

/**
���[�h���X�g�E�C���h�E�֒P�ꌟ����v��
*/
void	AView_Text::RequestWordsList()
{
	//�I��͈͎擾
	AIndex	selSpos = 0, selEpos = 0;
	GetSelect(selSpos,selEpos);
	//�P��
	AText	word;
	//���ݒP��擾
	ATextIndex	spos = 0, epos = 0;
	if( GetTextDocumentConst().SPI_FindWordForKeyword(selSpos,spos,epos) == true )
	{
		//���ݒP�ꂪ�A���t�@�x�b�g�̏ꍇ�A���[�Y���X�g��v������
		
		//�P��擾
		GetTextDocumentConst().SPI_GetText(spos,epos,word);
		
		//���[�Y���X�g�E�C���h�E�ɒP�ꌟ����v��
		AFileAcc	folder;
		GetTextDocumentConst().SPI_GetProjectFolder(folder);
		if( folder.IsSpecified() == false )
		{
			//�v���W�F�N�g�t�H���_���ݒ�Ȃ�A�e�t�H���_���擾
			//���[�h���X�g�����X���b�h����SPI_GetImportFileDataIDArrayForWordsList()���R�[�����ꂽ�Ƃ���
			//�v���W�F�N�g�t�H���_�łȂ���΁A���̃t�H���_���ċA�I�Ɍ�������B
			GetTextDocumentConst().SPI_GetParentFolder(folder);
		}
		GetApp().SPI_RequestWordsList(mTextWindowID,folder,GetTextDocumentConst().SPI_GetModeIndex(),word);//���[�Y���X�g�E�C���h�E���Ȃ���Ή������Ȃ�
	}
}

#pragma mark ===========================

#pragma mark ---diff

/**
diff���葤�h�L�������g�̑I���ʒu�ɍ��킹�āA�����̑I���ʒu��ݒ�
*/
void	AView_Text::SPI_SetSelectByDiffTargetDocumentSelection( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint )
{
	ATextPoint	spt = {0}, ept = {0};
	if( GetTextDocumentConst().SPI_GetCorrespondingTextPointByDiffTargetTextPoint(inStartPoint,spt) == true &&
				GetTextDocumentConst().SPI_GetCorrespondingTextPointByDiffTargetTextPoint(inEndPoint,ept) == true )
	{
		SPI_SetSelect(spt,ept);
		ShowCaret();
	}
}

#pragma mark ===========================

#pragma mark ---KeyRecord

/**
�L�[�L�^�Đ�
*/
void	AView_Text::SPI_PlayKeyRecord()
{
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
	//�c�[�����s
	AText	text;
	GetApp().SPI_GetRecordedText(text);
	InsertToolText(text,1);
}

#pragma mark ===========================

//
void	AView_Text::SPI_MoveToLine( const AIndex inLineIndex, const ABool inParagraphMode )
{
	AIndex	lineIndex = inLineIndex;
	if( inParagraphMode == true )
	{
		lineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(inLineIndex);
	}
	ATextPoint	spt, ept;
	if( lineIndex >= GetTextDocumentConst().SPI_GetLineCount()-1 )//#224 �ŏI�s�̏����Y��C��
	{
		spt.x = 0;
		spt.y = GetTextDocumentConst().SPI_GetLineCount()-1;
		ept.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(GetTextDocumentConst().SPI_GetLineCount()-1);
		ept.y = GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	else
	{
		spt.x = 0;
		spt.y = lineIndex;
		ept.x = 0;
		ept.y = lineIndex+1;
	}
	SPI_SetSelect(spt,ept);
	SPI_AdjustScroll_Center();
}

void	AView_Text::SPI_DoJumpListSelected( const AIndex inIndex )
{
	AUniqID	identifierObjectID = GetTextDocumentConst().SPI_GetJumpMenuIdentifierUniqID(inIndex);
	/*#146
	ATextPoint	spt,ept;
	GetTextDocument().SPI_GetGlobalIdentifierRange(identifierObjectID,spt,ept);
	AdjustScroll_Top(spt);
	SetSelect(spt,ept);
	*/
	Jump(identifierObjectID);
	NVM_GetWindow().NVI_SelectWindow();//win 080714
}

//�h�L�������g��Save�O����(ADocument_Text��SPI_Save()����R�[�������)
void	AView_Text::SPI_SavePreProcess()
{
	//Caret, Select����DB�֕ۑ�
	GetTextDocument().GetDocPrefDB().SetData_Bool(ADocPrefDB::kCaretMode,			mCaretMode);
	/*B0425 y��i���ԍ��ɂ���悤�ɕύX
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kCaretPointX,		GetCaretTextPoint().x);
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kCaretPointY,		GetCaretTextPoint().y);
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointX,		GetSelectTextPoint().x);
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointY,		GetSelectTextPoint().y);
	*/
	ATextIndex	carettextindex	= GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
	AIndex	caretparaindex		= GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(GetCaretTextPoint().y);
	ATextIndex	caretparastart	= GetTextDocumentConst().SPI_GetParagraphStartTextIndex(caretparaindex);
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kCaretPointX,		carettextindex-caretparastart);
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kCaretPointY,		caretparaindex);
	if( mCaretMode == true )
	{
		GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointX,		carettextindex-caretparastart);
		GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointY,		caretparaindex);
	}
	else
	{
		ATextIndex	selecttextindex	= GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetSelectTextPoint());
		AIndex	selectparaindex		= GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(GetSelectTextPoint().y);
		ATextIndex	selectparastart	= GetTextDocumentConst().SPI_GetParagraphStartTextIndex(selectparaindex);
		GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointX,		selecttextindex-/*#260selectparaindex*/selectparastart);
		GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointY,		selectparaindex);
	}
}

void	AView_Text::SPI_SelectText( const AIndex inStartIndex, const AItemCount inLength, const ABool inByFind )
{
	ATextIndex	spos, epos;
	spos = inStartIndex;
	epos = spos + inLength;
	SetSelect(spos,epos,inByFind);
	AdjustScroll_Center(GetCaretTextPoint());
}

#if IMPLEMENTATION_FOR_MACOSX
//B0370
void	AView_Text::SPI_ServicePaste( const AScrapRef inScrapRef )
{
	AText	text;
	AScrapWrapper::GetTextScrap(inScrapRef,text);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	TextInput(undoTag_Paste,text);
}
#endif

#pragma mark ===========================

#pragma mark ---<�������W���[��>

#pragma mark ===========================

#pragma mark ---�e�L�X�g�`��p�f�[�^�擾

/*#450
//�s�̍����擾
ANumber	AView_Text::GetLineHeightWithMargin() const
{
	return mLineHeight + mLineMargin;
}
ANumber	AView_Text::GetLineHeightWithoutMargin() const
{
	return mLineHeight;
}
*/
//#450
/**
*/
ANumber	AView_Text::GetLineHeightWithMargin( const AIndex inLineIndex ) const
{
	if( inLineIndex >= mLineImageInfo_Height.GetItemCount() )
	{
		return mLineHeight + mLineMargin;
	}
	else
	{
		return mLineImageInfo_Height.Get(inLineIndex);
	}
}
ANumber	AView_Text::GetLineHeightWithoutMargin( const AIndex inLineIndex ) const
{
	if( inLineIndex >= mLineImageInfo_Height.GetItemCount() )
	{
		return mLineHeight;
	}
	else
	{
		AFloatNumber	h = mLineImageInfo_Height.Get(inLineIndex);
		h *= mLineHeight;
		h /= (mLineHeight+mLineMargin);
		return h;
	}
}
ANumber	AView_Text::GetLineAscent( const AIndex inLineIndex ) const
{
	if( inLineIndex >= mLineImageInfo_Height.GetItemCount() )
	{
		return mLineAscent;
	}
	else
	{
		AFloatNumber	h = mLineImageInfo_Height.Get(inLineIndex);
		h *= mLineAscent;
		h /= (mLineHeight+mLineMargin);
		return h;
	} 
} 

//
/**
ImageY����e�L�X�g�s�ԍ��擾�i�e�s���Ƃ�ImageY�z�񂩂�2���@�Ō����j
*/
AIndex	AView_Text::GetLineIndexByImageY( const ANumber inImageY ) const
{
	if( mLineImageInfo_ImageY.GetItemCount() == 0 )   return 0;
	
	//#450 �s�܂�Ԃ��Ή�
	//
	if( inImageY > mLineImageInfo_ImageY.Get(mLineImageInfo_ImageY.GetItemCount()-1) || inImageY < 0 )
	{
		//_ACError("image y invalid",this);
		return GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	//2���@
	AIndex	startLineIndex = 0;
	AIndex	endLineIndex = mLineImageInfo_ImageY.GetItemCount();
	AIndex	lineIndex = (startLineIndex+endLineIndex)/2;
	AItemCount	lineCount = mLineImageInfo_ImageY.GetItemCount();
	const ADocument_Text&	document = GetTextDocumentConst();
	for( AIndex i = 0; i < lineCount*2; i++ )//�������[�v�h�~�i�����Ƃ��s����̃��[�v�ŏI���͂��j
	{
		ANumber	imageY = mLineImageInfo_ImageY.Get(lineIndex);
		ANumber	height = mLineImageInfo_Height.Get(lineIndex);
		if( inImageY >= imageY && inImageY <= imageY+height ) 
		{
			if( lineIndex < 0 )
			{
				lineIndex = 0;
			}
			if( lineIndex >= document.SPI_GetLineCount() )
			{
				lineIndex = document.SPI_GetLineCount()-1;
			}
			//���傤�ǎ��̍s�Ƃ̋��E��̏ꍇ�͎��̍s��Ԃ��i2���@�̓s����(?)�A���inImageY <= imageY+height�̕s������<�ɕς���ƈ�v�s���������Ȃ��j
			if( inImageY == imageY+height )
			{
				lineIndex++;
			}
			//�ŏI�s�ȍ~�ɂȂ�����␳
			if( lineIndex >= document.SPI_GetLineCount() )
			{
				lineIndex = document.SPI_GetLineCount()-1;
			}
			//
			return lineIndex;
		}
		if( inImageY < imageY )
		{
			endLineIndex = lineIndex;
			lineIndex = (startLineIndex+endLineIndex)/2;
		}
		else if( inImageY > imageY+height )
		{
			startLineIndex = lineIndex+1;
			lineIndex = (startLineIndex+endLineIndex)/2;
		}
		if( startLineIndex+1 >= endLineIndex )
		{
			endLineIndex = startLineIndex+1;
			lineIndex = startLineIndex;
		}
	}
	_ACError("cannot find image y",this);
	return kIndex_Invalid;
	/*
	AIndex	lineIndex = inImageY/GetLineHeightWithMargin();
	if( lineIndex < 0 )
	{
		lineIndex = 0;
	}
	if( lineIndex >= GetTextDocumentConst().SPI_GetLineCount() )
	{
		lineIndex = GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	return lineIndex;
	*/
}

//
ANumber	AView_Text::GetImageYByLineIndex( const AIndex inLineIndex ) const
{
	if( mLineImageInfo_ImageY.GetItemCount() == 0 )   return 0;
	//#450 �s�܂�Ԃ��Ή�
	if( inLineIndex < 0 )
	{
		return 0;
	}
	if( inLineIndex >= mLineImageInfo_ImageY.GetItemCount() )
	{
		return mLineImageInfo_ImageY.Get(mLineImageInfo_ImageY.GetItemCount()-1);
	}
	return mLineImageInfo_ImageY.Get(inLineIndex);
	/*#450
	return inLineIndex * GetLineHeightWithMargin();
	*/
}

//
void	AView_Text::GetLineImageRect( const AIndex inLineIndex, AImageRect& outLineImageRect ) const
{
	outLineImageRect.left	= 0;
	outLineImageRect.right	= NVM_GetImageWidth();
	outLineImageRect.top	= GetImageYByLineIndex(inLineIndex);//#450 inLineIndex*GetLineHeightWithMargin();
	outLineImageRect.bottom	= GetImageYByLineIndex(inLineIndex+1);//#450 (inLineIndex+1)*GetLineHeightWithMargin();
}

//
void	AView_Text::GetLineImageRect( const AIndex inStartLineIndex, const AIndex inEndLineIndex, AImageRect& outLineImageRect ) const
{
	outLineImageRect.left	= 0;
	outLineImageRect.right	= NVM_GetImageWidth();
	outLineImageRect.top	= GetImageYByLineIndex(inStartLineIndex);//#450 inStartLineIndex*GetLineHeightWithMargin();
	outLineImageRect.bottom	= GetImageYByLineIndex(inEndLineIndex);//#450 inEndLineIndex*GetLineHeightWithMargin();
}

//
//����const
void	AView_Text::GetImagePointFromTextPoint( const ATextPoint& inTextPoint, AImagePoint& outImagePoint )
{
	CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	ANumber	y = inTextPoint.y;
	if( y >= GetTextDocumentConst().SPI_GetLineCount() )
	{
		y = GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	//#470 GetTextDocument().SPI_GetTextDrawDataWithoutStyle(y,textDrawData);
	AAnalyzeDrawData	analyzeDrawData;
	ATextPoint	caretPoint, selectPoint;
	GetTextDocument().SPI_GetTextDrawDataWithStyle(y,textDrawData,false,caretPoint,selectPoint,analyzeDrawData,GetObjectID());
	
	//#450
	SetLineDefaultTextProperty(inTextPoint.y);
	
	//
	outImagePoint.x = NVMC_GetImageXByTextPosition(textDrawData,inTextPoint.x);
	outImagePoint.y = GetImageYByLineIndex(inTextPoint.y);//#450
	/*#450
	outImagePoint.y = y * GetLineHeightWithMargin();
	*/
} 

//
//����const
void	AView_Text::GetTextPointFromImagePoint( const AImagePoint& inImagePoint, ATextPoint& outTextPoint )
{
	if( inImagePoint.y > NVM_GetImageHeight()/*#450 GetLineHeightWithMargin() * GetTextDocumentConst().SPI_GetLineCount()*/ )
	{
		outTextPoint.y = GetTextDocumentConst().SPI_GetLineCount()-1;
		outTextPoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(outTextPoint.y);
		return;
	}
	//�e�L�X�gY���W�v�Z
	outTextPoint.y = GetLineIndexByImageY(inImagePoint.y);
	
	//#450
	SetLineDefaultTextProperty(outTextPoint.y);
	
	//�Ώۍs�`��f�[�^�擾
	CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	//#470 GetTextDocument().SPI_GetTextDrawDataWithoutStyle(outTextPoint.y,textDrawData);
	AAnalyzeDrawData	analyzeDrawData;
	ATextPoint	caretPoint, selectPoint;
	GetTextDocument().SPI_GetTextDrawDataWithStyle(outTextPoint.y,textDrawData,false,caretPoint,selectPoint,analyzeDrawData,GetObjectID());
	//
	outTextPoint.x = NVMC_GetTextPositionByImageX(textDrawData,inImagePoint.x);
}

//#450
/**
�s�̃f�t�H���gtextproperty��ݒ�
*/
void	AView_Text::SetLineDefaultTextProperty( const AIndex inLineIndex )
{
	/* �s���Ƃ�multiply�@�\������̂��߁A��U�R�����g�A�E�g�i�������̂��߁jGetImagePointFromTextPoint(), GetTextPointFromImagePoint()����R�[������Ă���
	AText	fontname;
	//#868 GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,fontname);
	GetTextDocumentConst().SPI_GetFontName(fontname);//#868
	AColor	lettercolor;
	GetModePrefDB().GetData_Color(AModePrefDB::kLetterColor,lettercolor);
	ABool	isAntiAlias = GetTextDocument().SPI_IsAntiAlias();
	AFloatNumber	fontsize = GetTextDocumentConst().SPI_GetFontSize()
					* GetTextDocumentConst().SPI_GetLineFontSizeMultiply(inLineIndex);
	NVMC_SetDefaultTextProperty(fontname,fontsize,lettercolor,kTextStyle_Normal,isAntiAlias);
	*/
}

//#379
/**
�s�ԍ�����LocalY���擾
*/
AIndex	AView_Text::SPI_GetLocalYByLineIndex( const AIndex inLineIndex ) const
{
	AImagePoint	ipt = {0,0};
	ipt.y = GetImageYByLineIndex(inLineIndex);
	ALocalPoint	lpt = {0,0};
	NVM_GetLocalPointFromImagePoint(ipt,lpt);
	return lpt.y;
}

#pragma mark ===========================

#pragma mark ---�L�����b�g����

//�L�����b�g���[�h���ǂ������擾
ABool	AView_Text::IsCaretMode() const
{
	return mCaretMode;
}

//
void	AView_Text::SetCaretTextPoint( const ATextPoint& inTextPoint )
{
	//#496 �s�`��X�V�̂��߂Ɍ��ݍs�̈ʒu���L�����Ă���
	ATextPoint	svCaretPoint = mCaretTextPoint;
	//#262
	mSelectionByFind = false;
	//
	if( IsCaretMode() == true )
	{
		//win
		if( inTextPoint.x == mCaretTextPoint.x && inTextPoint.y == mCaretTextPoint.y )
		{
			return;
		}
		//
		HideCaret(inTextPoint.y != mCaretTextPoint.y,true);
	}
	else
	{
		mCaretMode = true;
		AImageRect	imageRect;
		if( mKukeiSelected == true )
		{
			ATextPoint	spt, ept;
			GetKukeiContainRange(spt,ept);
			GetLineImageRect(spt.y,ept.y+1,imageRect);
			mKukeiSelected_Start.DeleteAll();
			mKukeiSelected_End.DeleteAll();
			mKukeiSelected = false;
		}
		else
		{
			ATextPoint	pt1, pt2;
			ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
			GetLineImageRect(pt1.y,pt2.y+1,imageRect);
		}
		ALocalRect	localRect;
		NVM_GetLocalRectFromImageRect(imageRect,localRect);
		//view�E�[�܂ł�`��
		ALocalRect	localFrameRect = {0};
		NVM_GetLocalViewRect(localFrameRect);
		if( localRect.right < localFrameRect.right )
		{
			localRect.right = localFrameRect.right;
		}
		//
		NVMC_RefreshRect(localRect);
	}
	//#231 �}���`�o�C�g�����̓r���ɂȂ�Ȃ��悤�ɕ␳
	ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(inTextPoint);
	ATextIndex	adjustedindex = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(textindex);
	if( textindex == adjustedindex )
	{
		//�␳����
		mCaretTextPoint = inTextPoint;
	}
	else
	{
		//�␳����
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(adjustedindex,mCaretTextPoint);
	}
	//#571 �G���[�ʒu�␳
	if( mCaretTextPoint.y < 0 )
	{
		mCaretTextPoint.y = 0;
	}
	if( mCaretTextPoint.y >= GetTextDocumentConst().SPI_GetLineCount() )
	{
		mCaretTextPoint.y = GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	if( mCaretTextPoint.x < 0 )
	{
		mCaretTextPoint.x = 0;
	}
	if( mCaretTextPoint.x > GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(mCaretTextPoint.y) )
	{
		mCaretTextPoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(mCaretTextPoint.y);
	}
	
	GetImagePointFromTextPoint(GetCaretTextPoint(),mCaretImagePoint);
	if( NVI_IsFocusActive() == true )
	{
		//win Caret������΍��i�r���ŕ`�悹���ɁA�����ł܂Ƃ߂ĕ\���j ShowCaret();
		//Caret�\���\�񂷂�
		ShowCaretReserve();//win
	}
	
	//#867
	//���[�J���͈͍X�V�g���K�[
	TriggerKeyIdleWork(kKeyIdleWorkType_UpdateLocalRange,kKeyIdleWorkTimerValue);
	
	//
	SetOldArrowImageX(mCaretImagePoint.x);
	
	/*#844
	//#496 ���ݍs�����`�悠��̏ꍇ�A�s�`��X�V
	if( GetModePrefDB().GetData_Bool(AModePrefDB::kShowUnderlineAtCurrentLine) == true )
	*/
	if( svCaretPoint.y != mCaretTextPoint.y )
	{
		SPI_RefreshLine(svCaretPoint.y);
		SPI_RefreshLine(mCaretTextPoint.y);
	}
	/*
	*/
	//#688
	//TextInputClient��selected range�������iInlineInput���������ǂ����̔��f�͊֐����Ŏ��{�j
	NVMC_ResetSelectedRangeForTextInput();
	
	//#450
	//�ݒ肵���L�����b�g�ʒu���܂肽���ݓ��Ȃ炻�̍s��܂肽���݉�������
	if( SPI_IsLineCollapsed(mCaretTextPoint.y) == true )
	{
		SPI_RevealCollapsedLine(mCaretTextPoint.y);
	}
	
	//#142
	//�e�L�X�g�J�E���g�i�I��͈́j�X�V
	UpdateTextCountWindows();
	
	//�I��ύX��JavaScript�v���O�C�����s #994
	//#994 GetTextDocumentConst().SPI_ExecuteEventListener("onSelectionChanged",GetEmptyText());
	
	//#750
	//���ݒP��n�C���C�g����
	//GetTextDocument().SPI_ClearCurrentWordForHighlight(true);
	
	//�|�b�v�A�b�v�E�C���h�E�ʒu����
	AdjustPopupWindowsPosition();
	
	//�L�����b�g�ʒu�̃J���[�X���b�gindex�ɕϊ����������ꍇ�́A���@�F�����g���K�[����
	AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(mCaretTextPoint);
	AIndex	colorSlotIndex = GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetColorSlotIndexByStateIndex(stateIndex);
	if( colorSlotIndex != mCurrentCaretStateColorSlotIndex )
	{
		//#905
		//���@�F���ƃ��[�J���͈͐ݒ�
		TriggerSyntaxRecognize();
		//�L�����b�g�ʒu�J���[�X���b�g�L��
		mCurrentCaretStateColorSlotIndex = colorSlotIndex;
	}
	
	//�W�����v���X�g�X�V�̂��߁A�I���X�V���E�C���h�E�֒ʒm
	NVM_GetWindow().OWICB_SelectionChanged(NVI_GetControlID());
	
	//�L�����b�g�ʒu�Ƀq���g�e�L�X�g������΁A�����e�L�[���[�h�Ƃ��Č�⌟��
	RequestCandidateFinderIfHintExist();
	
	//#1031
	//�L�����b�g�̃��[�J���ʒu���L��
	SetLastCaretLocalPoint();
	
	//���ʃn�C���C�g #1406
	HighlightBrace();
}

//#1031
/**
�L�����b�g�̃��[�J���ʒu���L��
*/
void	AView_Text::SetLastCaretLocalPoint()
{
	AImagePoint	ipt = {0};
	GetImagePointFromTextPoint(mCaretTextPoint,ipt);
	NVM_GetLocalPointFromImagePoint(ipt,mLastCaretLocalPoint);
}

//#450
/**
*/
void	AView_Text::UpdateCaretImagePoint()
{
	if( IsCaretMode() == true )
	{
		HideCaret(true,true);
		
		GetImagePointFromTextPoint(GetCaretTextPoint(),mCaretImagePoint);
		SetOldArrowImageX(mCaretImagePoint.x);
		//SPI_EditPostProcess()����R�[�����ꂽ�Ƃ��ɁA���HideCaret()���Ă��܂��̂ŁA
		//���̂��Ƃ�SetCaretPoint()��showcaret���ꂸ�Adelete�L�[���̒���ɃL�����b�g�\������Ȃ����̑΍�B#690
		ShowCaretReserve();
	}
}

//
void	AView_Text::SetOldArrowImageX( const ANumber inImageX )
{
	mOldArrowImageX = inImageX;
}

ANumber	AView_Text::GetOldArrowImageX() const
{
	return mOldArrowImageX;
}

//���݂̃L�����b�g�`��f�[�^�擾
//�Ԃ�l�F�L�����b�g�����݂�ViewRect�Ɋ܂܂�邩�ǂ���
void	AView_Text::GetCaretDrawData( ALocalPoint& outStartPoint, ALocalPoint& outEndPoint ) const
{
	AImagePoint	imageSpt = mCaretImagePoint;
	AImagePoint	imageEpt = imageSpt;
	imageEpt.y += GetLineHeightWithMargin(mCaretTextPoint.y)-1;//#450 GetLineHeightWithoutMargin()-1;
	NVM_GetLocalPointFromImagePoint(imageSpt,outStartPoint);
	NVM_GetLocalPointFromImagePoint(imageEpt,outEndPoint);
	//return (NVM_IsImagePointInViewRect(imageSpt) == true || NVM_IsImagePointInViewRect(imageEpt) == true);
}

//R0184
/**
�L�����b�g�`��
*/
void	AView_Text::DrawXorCaret( const ABool inFlush )//#1034
{
	ALocalPoint	spt,ept;
	GetCaretDrawData(spt,ept);
	//�L�����b�g�F�i�������F�j�擾
	AColor	modeLetterColor = kColor_Black;
	GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetModeData_Color(AModePrefDB::kLetterColor,modeLetterColor);
	//�L�����b�g����
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kBigCaret) == false )
	{
		NVMC_DrawXorCaretLine(spt,ept,true,inFlush,false,1);//#688 #1034
	}
	else
	{
		spt.y -= 1;
		ept.y += 1;
		if( spt.x > 0 )
		{
			//#186 spt.x -= 1;
			//#186 ept.x -= 1;
		}
		//
		NVMC_DrawXorCaretLine(spt,ept,true,inFlush,false,2);//#688 #1034
	}
	//#448
	/*
	if( mTextWindowID != kObjectID_Invalid )
	{
		if( GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_GetInlineInputMode() == true )
		{
			spt = ept;
			spt.x += 2;
			NVMC_DrawXorCaretLine(spt,ept,true,false);
		}
	}
	*/
}

//�L�����b�g�\��
void	AView_Text::ShowCaret()
{
	//���initial�n�̂Ƃ��͉����������^�[��
	if( GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		return;
	}
	
	if( IsCaretMode() == true )
	{
		//===============�ʏ�L�����b�g===============
		if( NVI_IsFocusActive() == true )
		{
			if( mCaretBlinkStateVisible == false )
			{
				DrawXorCaret(mRefreshing == false);//#1034 �e�L�X�g�`��\�肪�Ȃ��ꍇ�̂�flush����
			}
			mCaretTickCount = 0;
			mCaretBlinkStateVisible = true;
		}
		
		//===============�N���X�L�����b�gH===============
		{
			if( mCrossCaretMode == true && mCrossCaretHVisible == false )
			{
				XorCrossCaretH();
				mCrossCaretHVisible = true;
			}
		}
		//===============�N���X�L�����b�gV===============
		{
			if( mCrossCaretMode == true && mCrossCaretVVisible == false )
			{
				XorCrossCaretV();
				mCrossCaretVVisible = true;
			}
		}
	}
}

//�L�����b�g��\��
void	AView_Text::HideCaret( const ABool inHideCrossCaretH, const ABool inHideCrossCaretV )
{
	//fprintf(stderr,"HideCaret()");
	if( IsCaretMode() == true )
	{
		//===============�ʏ�L�����b�g===============
		{
			if( mCaretBlinkStateVisible == true )
			{
				DrawXorCaret(false);//#1034 ���flush���Ȃ�
			}
			mCaretTickCount = 0;
			mCaretBlinkStateVisible = false;
		}
		//===============�N���X�L�����b�gH===============
		if( mCrossCaretMode == true && mCrossCaretHVisible == true && inHideCrossCaretH == true )
		{
			XorCrossCaretH();
			mCrossCaretHVisible = false;
		}
		//===============�N���X�L�����b�gV===============
		if( mCrossCaretMode == true && mCrossCaretVVisible == true && inHideCrossCaretV == true )
		{
			XorCrossCaretV();
			mCrossCaretVVisible = false;
		}
	}
}

/**
Caret�\���\��
Windows�ł�Caret������΍�B�iWindows�̏ꍇ�AShowCaret()���R�[������Ƃ��̎��_�ŕ`�悳���̂ŁA�����r���̉ӏ���Caret�`�悳���B
���ۂɂ͂����ŕ`�悹���AShowCaretIfReserved()�R�[�����A�܂��́A���񑦃^�C���A�E�g���ɕ`�悷��
*/
void	AView_Text::ShowCaretReserve()
{
	//fprintf(stderr,"ShowCaretReserve()");
	if( IsCaretMode() == true )
	{
		//������Hide����ƕ������͂̂��тɃL�����b�g��������Ă��܂��B
		//������Hide�����AShowCaretIfReserved()�ŁA�O��caret��hide(DrawXorCaretLine()���Ŏ��s�����)�{����show�ɂ��邱�ƂŁA
		//��������Ȃ��悤�ɂ���B
		if( mCaretBlinkStateVisible == true )
		{
			DrawXorCaret(false);//#1034 ���flush���Ȃ�
		}
		//#1034
		//�e�L�X�g�`��\�肪�Ȃ��ꍇ�F����ShowCaretIfReserved()���s���ɃL�����b�g�`����s��
		if( mRefreshing == false )
		{
			mCaretTickCount = 99999;
		}
		//�e�L�X�g�`��\�肪����ꍇ�Ftick timer2�񔭐���ɃL�����b�g�`����s��
		else
		{
			mCaretTickCount = ATimerWrapper::GetCaretTime()-2;
		}
		mCaretBlinkStateVisible = false;
		/*
		if( mCrossCaretMode == true && mCrossCaretVisible == true )
		{
			XorCrossCaret(false);
			mCrossCaretVisible = false;
		}
		*/
	}
}

/**
Caret�\���iCaret�\���\�񎞁j
*/
void	AView_Text::ShowCaretIfReserved()
{
	if( mCaretTickCount >= 99999 )
	{
		ShowCaret();
	}
}

/**
�L�����b�g�ꎞ��\��
@note DoDraw()�O�A����сA�X�N���[�����s�O��NVIDO_ScrollPreProcess()����R�[�������
*/
void	AView_Text::TempHideCaret()
{
	//#688
	//�t�H�[�J�X�����Ƃ��͉������Ȃ�
	/*cross caret h���t�H�[�J�X���Ȃ�view�ł��\���������̂ŃR�����g�A�E�g
	if( NVI_IsFocusActive() == false )
	{
		return;
	}
	*/
	
	mTempCaretHideCount++;
	if( mTempCaretHideCount != 1 )   return;
	if( IsCaretMode() == true && NVI_IsFocusActive() == true )
	{
		if( mCaretBlinkStateVisible == true )
		{
			DrawXorCaret(false);//#1034 ���flush���Ȃ�
			//fprintf(stderr,"TEMPHIDE ");
		}
		
		if( mCrossCaretMode == true && mCrossCaretHVisible == true )
		{
			XorCrossCaretH();
		}
		if( mCrossCaretMode == true && mCrossCaretVVisible == true )
		{
			XorCrossCaretV();
		}
		
	}
}

/**
�L�����b�g�ꎞ��\������
@note DoDraw()��A����сA�X�N���[�����s���NVIDO_ScrollPostProcess()����R�[�������
*/
void	AView_Text::RestoreTempHideCaret()
{
	//#688
	//�t�H�[�J�X�����Ƃ��͉������Ȃ�
	/*cross caret h���t�H�[�J�X���Ȃ�view�ł��\���������̂ŃR�����g�A�E�g
	if( NVI_IsFocusActive() == false )
	{
		return;
	}
	*/
	
	mTempCaretHideCount--;
	if( mTempCaretHideCount != 0 )   return;
	if( IsCaretMode() == true )
	{
		if( mCaretBlinkStateVisible == true && NVI_IsFocusActive() == true )
		{
			DrawXorCaret(false);//#1034 ���flush���Ȃ�
			//fprintf(stderr,"RESTORETEMPHIDE ");
		}
		
		//
		if( mCrossCaretMode == true && mCrossCaretHVisible == true )
		{
			XorCrossCaretH();
		}
		if( mCrossCaretMode == true && mCrossCaretVVisible == true )
		{
			XorCrossCaretV();
		}
	}
}

//�L�����b�g�u�����N�i�L�����b�g�^�C�}�[���ɌĂ΂��j
void	AView_Text::BlinkCaret()
{
	//���initial�n�̂Ƃ��͉����������^�[��
	if( GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		return;
	}
	
	//fprintf(stderr,"BlinkCaret()");
	if( IsCaretMode() == true && NVI_IsFocusActive() == true )
	{
		/*R0184 ALocalPoint	spt,ept;
		GetCaretDrawData(spt,ept);
		NVMC_DrawXorCaretLine(spt,ept,true);*/
		DrawXorCaret(true);//#1034 ���flush����
		mCaretBlinkStateVisible = !mCaretBlinkStateVisible;
		//fprintf(stderr,"BLINK ");
	}
	mCaretTickCount = 0;
}

/**
*/
void	AView_Text::XorCrossCaretH()
{
	return;
	/*
	//
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayCaretWithHorizontalLine) == true ||
				GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayCrossCaret) == true )
	{
		//
		ALocalPoint	cspt,cept;
		GetCaretDrawData(cspt,cept);
		ALocalPoint	spt = cspt;
		ALocalPoint	ept = cept;
		ALocalRect	localFrame;
		NVM_GetLocalViewRect(localFrame);
		spt = cspt;
		ept = cept;
		spt.x = localFrame.left;
		ept.x = localFrame.right;
		spt.y = ept.y;
		spt.y--;
		ept.y--;
		//
		AImagePoint	ipt = {0};
		NVI_GetOriginOfLocalFrame(ipt);
		if( (ipt.x%2) == 1 )
		{
			spt.x--;
		}
		//
		NVMC_DrawXorCaretLine(spt,ept,true,true,1);
		//
		GetLineNumberView().SPI_RefreshLine(mCaretTextPoint.y);
	}
	*/
}

/**
�������L�����b�g
*/
void	AView_Text::XorCrossCaretV()
{
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDrawVerticalLineAsCaret) == true )
	{
		//�L�����b�g�ʒu�f�[�^�擾
		ALocalPoint	cspt,cept;
		GetCaretDrawData(cspt,cept);
		ALocalPoint	spt = cspt;
		ALocalPoint	ept = cept;
		ALocalRect	localFrame;
		NVM_GetLocalViewRect(localFrame);
		//������
		spt.y = localFrame.top;
		ept.y = localFrame.bottom;
		//
		AImagePoint	ipt = {0};
		NVI_GetOriginOfLocalFrame(ipt);
		if( (ipt.y%2) == 1 )
		{
			spt.y--;
		}
		//�L�����b�g�`��
		if( AEnvWrapper::GetOSVersion() <= kOSVersion_MacOSX_10_13 )//#1409 macOS 10.13�ȑO��alpha�w�肵�Ȃ��悤�ɂ���
		{
			//macOS 10.13�ȑO�̏ꍇ�Falpha�w��Ȃ��A�_�� #1409
			NVMC_DrawXorCaretLine(spt,ept,true,false,true,1);//#688 #1034 ���flush���Ȃ�
		}
		else
		{
			//macOS 10.14�ȍ~�̏ꍇ�Falpha�w�肠��
			NVMC_DrawXorCaretLine(spt,ept,true,false,false,1,0.3);//#688 #1034 ���flush���Ȃ� #1398
		}
	}
}

//
void	AView_Text::SPI_ShowHideCrossCaret()
{
	//if( NVI_IsFocusActive() == true )
	{
		if( IsCaretMode() == true )
		{
			HideCaret(true,true);
		}
	}
	mCrossCaretMode = (mCrossCaretMode==false);
	//if( NVI_IsFocusActive() == true )
	{
		if( IsCaretMode() == true )
		{
			ShowCaret();
		}
	}
}

//
ABool	AView_Text::SPI_GetSelect( ATextPoint& outStartPoint, ATextPoint& outEndPoint ) const
{
	if( IsCaretMode() == false )
	{
		ATextPoint	caretPoint, selectPoint;
		GetCaretTextPoint(caretPoint);
		GetSelectTextPoint(selectPoint);
		ADocument_Text::OrderTextPoint(caretPoint,selectPoint,outStartPoint,outEndPoint);
	}
	else
	{
		GetCaretTextPoint(outStartPoint);
		GetCaretTextPoint(outEndPoint);
	}
	return IsCaretMode();
}

void	AView_Text::SPI_GetSelect( AArray<ATextIndex>& outStart, AArray<ATextIndex>& outEnd ) const
{
	outStart.DeleteAll();
	outEnd.DeleteAll();
	if( IsCaretMode() == false )
	{
		if( mKukeiSelected == false )
		{
			ATextIndex	start, end;
			GetSelect(start,end);
			outStart.Add(start);
			outEnd.Add(end);
		}
		else
		{
			for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
			{
				ATextIndex	start = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mKukeiSelected_Start.Get(i));
				ATextIndex	end = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mKukeiSelected_End.Get(i));
				outStart.Add(start);
				outEnd.Add(end);
			}
		}
	}
	else
	{
		ATextIndex	start, end;
		GetSelect(start,end);
		outStart.Add(start);
		outEnd.Add(end);
	}
}

//B0000
//�I��͈͎擾
void	AView_Text::SPI_GetSelect( AIndex& outStartIndex, AIndex& outEndIndex ) const
{
	GetSelect(outStartIndex,outEndIndex);
}

//
void	AView_Text::GetSelect( AIndex& outStartIndex, AIndex& outEndIndex ) const
{
	ATextPoint	spt,ept;
	SPI_GetSelect(spt,ept);
	outStartIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
	outEndIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ept);
}

//
void	AView_Text::SPI_GetImageSelect( AImagePoint& outStartPoint, AImagePoint& outEndPoint )
{
	/*R0108 ATextPoint	spt,ept;
	SPI_GetSelect(spt,ept);
	GetImagePointFromTextPoint(spt,outStartPoint);
	GetImagePointFromTextPoint(ept,outEndPoint);*/
	if( mCaretMode == true )//B0391
	{
		outStartPoint =  outEndPoint = mCaretImagePoint;
	}
	else
	{
		OrderImagePoint(mCaretImagePoint,mSelectImagePoint,outStartPoint,outEndPoint);
	}
}

//
void	AView_Text::SPI_DoLineInfoChangeStart()
{
	GetSelect(mSavedStartTextIndex,mSavedEndTextIndex);
}

//
void	AView_Text::SPI_DoLineInfoChangeEnd()
{
	SetSelect(mSavedStartTextIndex,mSavedEndTextIndex);
}

//#413
/**
�L�����b�g�ʒu�␳
*/
void	AView_Text::SPI_CorrectCaretIfInvalid()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( GetTextDocumentConst().SPI_IsTextPointValid(spt) == false ||
				GetTextDocumentConst().SPI_IsTextPointValid(ept) == false )
	{
		spt.x = spt.y = 0;
		SPI_SetSelect(spt,spt);
	}
}

/**
�I���ʒu��global point�擾
*/
void	AView_Text::SPI_GetSelectGlobalRect( AGlobalRect& outRect ) 
{
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetSelect(spt,ept);
	AImagePoint	ispt = {0}, iept = {0};
	GetImagePointFromTextPoint(spt,ispt);
	GetImagePointFromTextPoint(ept,iept);
	ALocalPoint	lspt = {0}, lept = {0};
	NVM_GetLocalPointFromImagePoint(ispt,lspt);
	NVM_GetLocalPointFromImagePoint(iept,lept);
	AGlobalPoint	gspt = {0}, gept = {0};
	NVM_GetWindowConst().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lspt,gspt);
	NVM_GetWindowConst().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lept,gept);
	outRect.left		= gspt.x;
	outRect.top			= gspt.y;
	outRect.right		= gept.x;
	outRect.bottom		= gept.y + GetLineHeightWithMargin(ept.y);
}

/**
���ݒP���global point�擾
*/
void	AView_Text::SPI_GetCurrentWordGlobalRect( AGlobalRect& outRect ) 
{
	ATextIndex	wordSpos = 0, wordEpos = 0;
	GetCurrentWordForAbbrev(wordSpos,wordEpos);
	ATextPoint	spt = {0}, ept = {0};
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(wordSpos,spt);
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(wordEpos,ept);
	AImagePoint	ispt = {0}, iept = {0};
	GetImagePointFromTextPoint(spt,ispt);
	GetImagePointFromTextPoint(ept,iept);
	ALocalPoint	lspt = {0}, lept = {0};
	NVM_GetLocalPointFromImagePoint(ispt,lspt);
	NVM_GetLocalPointFromImagePoint(iept,lept);
	AGlobalPoint	gspt = {0}, gept = {0};
	NVM_GetWindowConst().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lspt,gspt);
	NVM_GetWindowConst().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lept,gept);
	outRect.left		= gspt.x;
	outRect.top			= gspt.y;
	outRect.right		= gept.x;
	outRect.bottom		= gept.y + GetLineHeightWithMargin(ept.y);
}

#pragma mark ===========================

#pragma mark ---�I�𐧌�

void	AView_Text::SetSelectTextPoint( const ATextPoint& inTextPoint, const ABool inByFind )
{
	//#262
	mSelectionByFind = inByFind;
	
	//inTextPoint���L�����b�g�ʒu�Ɠ����Ȃ�A�L�����b�g�Ƃ��Đݒ�
	if( inTextPoint.x == GetCaretTextPoint().x && inTextPoint.y == GetCaretTextPoint().y )
	{
		SetCaretTextPoint(inTextPoint);
		return;
	}
	
	if( mKukeiSelected == true )
	{
		mKukeiSelected_Start.DeleteAll();
		mKukeiSelected_End.DeleteAll();
		mKukeiSelected = false;
	}
	AImageRect	refreshImageRect;
	if( IsCaretMode() == true )
	{
		HideCaret(true,true);
		mCaretMode = false;
		//#231 �}���`�o�C�g�����̓r���ɂȂ�Ȃ��悤�ɕ␳
		ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(inTextPoint);
		ATextIndex	adjustedindex = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(textindex);
		if( textindex == adjustedindex )
		{
			//�␳����
			mSelectTextPoint = inTextPoint;
		}
		else
		{
			//�␳����
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(adjustedindex,mSelectTextPoint);
		}
		
		ATextPoint	pt1, pt2;
		ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
		GetLineImageRect(pt1.y,pt2.y+1,refreshImageRect);
	}
	else
	{
		ATextPoint	pt1, pt2;
		ADocument_Text::OrderTextPoint(GetSelectTextPoint(),inTextPoint,pt1,pt2);
		//#231 �}���`�o�C�g�����̓r���ɂȂ�Ȃ��悤�ɕ␳
		ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(inTextPoint);
		ATextIndex	adjustedindex = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(textindex);
		if( textindex == adjustedindex )
		{
			//�␳����
			mSelectTextPoint = inTextPoint;
		}
		else
		{
			//�␳����
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(adjustedindex,mSelectTextPoint);
		}
		
		GetLineImageRect(pt1.y,pt2.y+1,refreshImageRect);
	}
	//#571 �G���[�ʒu�␳
	if( mSelectTextPoint.y < 0 )
	{
		mSelectTextPoint.y = 0;
	}
	if( mSelectTextPoint.y >= GetTextDocumentConst().SPI_GetLineCount() )
	{
		mSelectTextPoint.y = GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	if( mSelectTextPoint.x < 0 )
	{
		mSelectTextPoint.x = 0;
	}
	if( mSelectTextPoint.x > GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(mSelectTextPoint.y) )
	{
		mSelectTextPoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(mSelectTextPoint.y);
	}
	//R0108 �������̂���
	GetImagePointFromTextPoint(GetSelectTextPoint(),mSelectImagePoint);
	
	ALocalRect	localRect;
	NVM_GetLocalRectFromImageRect(refreshImageRect,localRect);
	//view�E�[�܂ł�`��
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	if( localRect.right < localFrameRect.right )
	{
		localRect.right = localFrameRect.right;
	}
	//
	NVMC_RefreshRect(localRect);
	/*#844
	//#496 ���ݍs�����`�悠��̏ꍇ�A�s�`��X�V
	if( GetModePrefDB().GetData_Bool(AModePrefDB::kShowUnderlineAtCurrentLine) == true && mLineNumberViewID != kObjectID_Invalid )
	*/
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().SPI_RefreshLine(mCaretTextPoint.y);
	}
	/*
	*/
	//#688
	//TextInputClient��selected range�������iInlineInput���������ǂ����̔��f�͊֐����Ŏ��{�j
	NVMC_ResetSelectedRangeForTextInput();
	//#142
	//�I��͈̓e�L�X�g�J�E���g�X�V
	UpdateTextCountWindows();
	
	//�I��ύX��JavaScript�v���O�C�����s #994
	//#994 GetTextDocumentConst().SPI_ExecuteEventListener("onSelectionChanged",GetEmptyText());
	
	//#750
	//���ݒP��n�C���C�g����
	//GetTextDocument().SPI_ClearCurrentWordForHighlight(true);
	
	//�W�����v���X�g�X�V�̂��߁A�I���X�V���E�C���h�E�֒ʒm
	NVM_GetWindow().OWICB_SelectionChanged(NVI_GetControlID());
	
	//���ʃn�C���C�g���� #1406
	SetHighlightBraceVisible(false);
}

//
void	AView_Text::SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inByFind )//#262
{
	//�s�܂�Ԃ��v�Z���͑I��\�񂵂ă��^�[��#699
	if( GetTextDocumentConst().SPI_IsWrapCalculating() == true )
	{
		mReservedSelection_StartTextIndex = kIndex_Invalid;
		mReservedSelection_EndTextIndex = kIndex_Invalid;
		mReservedSelection_StartTextPoint = inStartPoint;
		mReservedSelection_EndTextPoint = inEndPoint;
		mReservedSelection_IsSelectWithParagraph = false;
		return;
	}
	//
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( spt.x == inStartPoint.x && spt.y == inStartPoint.y && ept.x == inEndPoint.x && ept.y == inEndPoint.y )
	{
		mSelectionByFind = inByFind;
		return;
	}
	if( (GetCaretTextPoint().x == inStartPoint.x && GetCaretTextPoint().y == inStartPoint.y) == false ||
		(inStartPoint.x == inEndPoint.x && inStartPoint.y == inEndPoint.y) )//B0434
	{
		SetCaretTextPoint(inStartPoint);
	}
	if( inStartPoint.x == inEndPoint.x && inStartPoint.y == inEndPoint.y )   return;
	SetSelectTextPoint(inEndPoint,inByFind);//#262
}
void	AView_Text::SetSelect( const AIndex inStart, const AIndex inEnd, const ABool inByFind )//#262
{
	//�s�܂�Ԃ��v�Z���͑I��\�񂵂ă��^�[��#699
	if( GetTextDocumentConst().SPI_IsWrapCalculating() == true )
	{
		mReservedSelection_StartTextIndex = inStart;
		mReservedSelection_EndTextIndex = inEnd;
		mReservedSelection_StartTextPoint.x = mReservedSelection_StartTextPoint.y = kIndex_Invalid;
		mReservedSelection_EndTextPoint.x = mReservedSelection_EndTextPoint.y = kIndex_Invalid;
		mReservedSelection_IsSelectWithParagraph = false;
		return;
	}
	//
	ATextPoint	spt, ept;
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inStart,spt);
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inEnd,ept);
	SetSelect(spt,ept,inByFind);//#262
}
void	AView_Text::SPI_SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inByFind )
{
	SetSelect(inStartPoint,inEndPoint,inByFind);
}
void	AView_Text::SPI_SetSelect( const AIndex inStart, const AIndex inEnd, const ABool inByFind )
{
	SetSelect(inStart,inEnd,inByFind);
}

//#699
/**
�I��������������
@note �s�܂�Ԃ��v�Z���ł����s����i�I��\�񂵂Ȃ��j
*/
void	AView_Text::SPI_InitSelect()
{
	SetCaretTextPoint(kTextPoint_00);
}

//#699
/**
�i��index�őI��
*/
void	AView_Text::SPI_SetSelectWithParagraphIndex( 
		const AIndex inStartParagraphIndex, const AIndex inStartOffset, 
		const AIndex inEndParagraphIndex, const AIndex inEndOffset )
{
	//�s�܂�Ԃ��v�Z���͑I��\�񂵂ă��^�[��#699
	if( GetTextDocumentConst().SPI_IsWrapCalculating() == true )
	{
		mReservedSelection_StartTextIndex = kIndex_Invalid;
		mReservedSelection_EndTextIndex = kIndex_Invalid;
		mReservedSelection_StartTextPoint.x = inStartOffset;
		mReservedSelection_StartTextPoint.y = inStartParagraphIndex;
		mReservedSelection_EndTextPoint.x = inEndOffset;
		mReservedSelection_EndTextPoint.y = inEndParagraphIndex;
		mReservedSelection_IsSelectWithParagraph = true;
		return;
	}
	
	//�I�����s
	AIndex	spos = GetTextDocumentConst().
			SPI_GetParagraphStartTextIndex(inStartParagraphIndex) + inStartOffset;
	AIndex	epos = GetTextDocumentConst().
			SPI_GetParagraphStartTextIndex(inEndParagraphIndex) + inEndOffset;
	SetSelect(spos,epos);
}

//�w��TextPoint���A�I��͈͓��ɂ��邩�ǂ���
//inIncludeBoundary: ���E���܂߂邩�ǂ���
ABool	AView_Text::IsInSelect( const AImagePoint& inImagePoint, const ABool inIncludeBoundary )
{
	if( IsCaretMode() == true )   return false;
	ATextPoint	textpoint;
	GetTextPointFromImagePoint(inImagePoint,textpoint);
	/* ATextPoint	lineend = textpoint;
	lineend.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y);
	AImagePoint	lineendImagePoint;
	GetImagePointFromTextPoint(lineend,lineendImagePoint);
	if( inImagePoint.x < 0 || inImagePoint.x > lineendImagePoint.x+1 )   return false;*/
	if( mKukeiSelected == false )
	{
		ATextPoint	spt, ept;
		SPI_GetSelect(spt,ept);
		AImagePoint	imagespt, imageept;
		GetImagePointFromTextPoint(spt,imagespt);
		GetImagePointFromTextPoint(ept,imageept);
		//spt.y == ept.y�̏ꍇ
		if( spt.y == ept.y )
		{
			if( textpoint.y == spt.y )
			{
				/* if( inIncludeBoundary == true )
				{
					return (textpoint.x >= spt.x && textpoint.x <= ept.x);
				}
				else
				{
					return (textpoint.x > spt.x && textpoint.x < ept.x);
				}*/
				return ( inImagePoint.x >= imagespt.x && inImagePoint.x <= imageept.x );
			}
			else
			{
				return false;
			}
		}
		//�ȍ~�Aspt.y < ept.y�̏ꍇ
		else if( textpoint.y == spt.y )
		{
			/* if( inIncludeBoundary == true )
			{
				return (textpoint.x >= spt.x);
			}
			else
			{
				return (textpoint.x > spt.x);
			}*/
			if( inIncludeBoundary == true )
			{
				return ( inImagePoint.x >= imagespt.x );
			}
			else
			{
				return ( inImagePoint.x > imagespt.x );
			}
		}
		else if( textpoint.y == ept.y )
		{
			/* if( inIncludeBoundary == true )
			{
				return (textpoint.x <= ept.x);
			}
			else
			{
				return (textpoint.x < ept.x);
			}*/
			return ( inImagePoint.x < imageept.x );
		}
		else if( textpoint.y > spt.y && textpoint.y < ept.y )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
		{
			if( mKukeiSelected_Start.Get(i).y == textpoint.y )
			{
				if( inIncludeBoundary == true )
				{
					return ( textpoint.x >= mKukeiSelected_Start.Get(i).x && textpoint.x <= mKukeiSelected_End.Get(i).x);
				}
				else
				{
					return ( textpoint.x > mKukeiSelected_Start.Get(i).x && textpoint.x < mKukeiSelected_End.Get(i).x );
				}
			}
		}
		return false;
	}
}

//
void	AView_Text::SetKukeiSelect( const AImagePoint& inImagePoint1, const AImagePoint& inImagePoint2 )
{
	//����`�I���̍s�͈͂��擾���A����`�I���f�[�^���폜
	AIndex	oldStartLineIndex = GetTextDocumentConst().SPI_GetLineCount();
	AIndex	oldEndLineIndex = 0;
	if( mKukeiSelected == true )
	{
		ATextPoint	spt, ept;
		GetKukeiContainRange(spt,ept);
		oldStartLineIndex = spt.y;
		oldEndLineIndex = ept.y;
	}
	mKukeiSelected_Start.DeleteAll();
	mKukeiSelected_End.DeleteAll();
	//
	ATextPoint	pt1, pt2;
	GetTextPointFromImagePoint(inImagePoint1,pt1);
	GetTextPointFromImagePoint(inImagePoint2,pt2);
	//
	AIndex	startLineIndex = pt1.y;
	AIndex	endLineIndex = pt2.y;
	if( startLineIndex == endLineIndex )
	{
		//�����s�Ȃ�ʏ�̑I���ŏ\���Ȃ̂ł�����ŁB�i�L�����b�g���[�h�ւ̕ύX��������Łj
		SPI_SetSelect(pt1,pt2);
		//�`��X�V
		SPI_RefreshLines(oldStartLineIndex,oldEndLineIndex+1);
		return;
	}
	if( endLineIndex < startLineIndex )
	{
		AIndex	tmp = endLineIndex;
		endLineIndex = startLineIndex;
		startLineIndex = tmp;
	}
	//
	if( IsCaretMode() == true )
	{
		HideCaret(true,true);
		mCaretMode = false;
	}
	//
	ANumber	startImageX = inImagePoint1.x;
	ANumber	endImageX = inImagePoint2.x;
	if( endImageX < startImageX )
	{
		ANumber	tmp = endImageX;
		endImageX = startImageX;
		startImageX = tmp;
	}
	//
	for( AIndex lineIndex = startLineIndex; lineIndex <= endLineIndex; lineIndex++ )
	{
		AImagePoint	ispt;
		ispt.x = startImageX;
		ispt.y = GetImageYByLineIndex(lineIndex);
		AImagePoint	iept;
		iept.x = endImageX;
		iept.y = ispt.y;
		ATextPoint	start, end;
		GetTextPointFromImagePoint(ispt,start);
		GetTextPointFromImagePoint(iept,end);
		mKukeiSelected_Start.Add(start);
		mKukeiSelected_End.Add(end);
	}
	mKukeiSelected = true;
	mCaretMode = false;
	mSelectTextPoint = GetCaretTextPoint();//�O�̂���
	//
	AIndex	drawStartLineIndex = startLineIndex;
	if( drawStartLineIndex > oldStartLineIndex )
	{
		drawStartLineIndex = oldStartLineIndex;
	}
	AIndex	drawEndLineIndex = endLineIndex;
	if( drawEndLineIndex < oldEndLineIndex )
	{
		drawEndLineIndex = oldEndLineIndex;
	}
	AImageRect	refreshImageRect;
	GetLineImageRect(drawStartLineIndex,drawEndLineIndex+1,refreshImageRect);
	ALocalRect	localRect;
	NVM_GetLocalRectFromImageRect(refreshImageRect,localRect);
	//view�E�[�܂ł�`��X�V
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	if( localRect.right < localFrameRect.right )
	{
		localRect.right = localFrameRect.right;
	}
	//#1123
	//�e�L�X�g���X�V
	UpdateTextCountWindows();
	//
	NVMC_RefreshRect(localRect);
}

//#1035
/**
��`�I��ݒ�
*/
void	AView_Text::SetKukeiSelect( const AArray<ATextIndex>& inStartArray, const AArray<ATextIndex>& inEndArray )
{
	//��`�I��S�폜
	mKukeiSelected_Start.DeleteAll();
	mKukeiSelected_End.DeleteAll();
	//�ݒ�
	for( AIndex i = 0; i < inStartArray.GetItemCount(); i++ )
	{
		ATextPoint	spt = {0};
		ATextPoint	ept = {0};
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inStartArray.Get(i),spt);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inEndArray.Get(i),ept);
		mKukeiSelected_Start.Add(spt);
		mKukeiSelected_End.Add(ept);
	}
	//��`�I�����[�h�ݒ�
	mKukeiSelected = true;
	mCaretMode = false;
	mSelectTextPoint = GetCaretTextPoint();//�O�̂���
	//#1123
	//�e�L�X�g���X�V
	UpdateTextCountWindows();
	//�`��X�V
	SPI_RefreshTextView();
}

void	AView_Text::GetKukeiContainRange( ATextPoint& outStart, ATextPoint& outEnd ) const
{
	outStart.x = 0;
	outStart.y = GetTextDocumentConst().SPI_GetLineCount();
	outEnd.x = 0;
	outEnd.y = 0;
	for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
	{
		if( mKukeiSelected_Start.Get(i).y < outStart.y )
		{
			outStart = mKukeiSelected_Start.Get(i);
		}
		if( mKukeiSelected_End.Get(i).y > outEnd.y )
		{
			outEnd = mKukeiSelected_End.Get(i);
		}
	}
}

void	AView_Text::SPI_EditPreProcess( const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	GetSelect(mSavedStartTextIndexForEditPrePostProcess,mSavedEndTextIndexForEditPrePostProcess);
	if( inInsertedCount > 0 )
	{
		if( mSavedStartTextIndexForEditPrePostProcess > inTextIndex )
		{
			mSavedStartTextIndexForEditPrePostProcess += inInsertedCount;
			mSavedEndTextIndexForEditPrePostProcess += inInsertedCount;
		}
		else if( mSavedEndTextIndexForEditPrePostProcess > inTextIndex )
		{
			mSavedEndTextIndexForEditPrePostProcess += inInsertedCount;
		}
	}
	if( inInsertedCount < 0 )
	{
		if( mSavedStartTextIndexForEditPrePostProcess > inTextIndex-inInsertedCount )
		{
			mSavedStartTextIndexForEditPrePostProcess += inInsertedCount;
			mSavedEndTextIndexForEditPrePostProcess += inInsertedCount;
		}
		else if( mSavedStartTextIndexForEditPrePostProcess > inTextIndex )
		{
			mSavedStartTextIndexForEditPrePostProcess = inTextIndex;
			mSavedEndTextIndexForEditPrePostProcess = inTextIndex;
		}
		else if( mSavedEndTextIndexForEditPrePostProcess > inTextIndex )
		{
			mSavedStartTextIndexForEditPrePostProcess = inTextIndex;
			mSavedEndTextIndexForEditPrePostProcess = inTextIndex;
		}
	}
}

void	AView_Text::SPI_EditPostProcess( const ATextIndex inTextIndex, const AItemCount inInsertedCount,
		const ABool inUpdateCaretImagePoint )//#846
{
	SetSelect(mSavedStartTextIndexForEditPrePostProcess,mSavedEndTextIndexForEditPrePostProcess);//�� #695
	/*�����Ă��L�����b�g�`��ʒu���X�V����Ȃ������Ȃ̂ŁA���x�D��ɂ����ق����ǂ�
	if( inUpdateCaretImagePoint == true )//#846
	{
		UpdateCaretImagePoint();//�L�����b�g�`��ʒu�␳ #0 �t�H���g�T�C�Y�ύX�����A�����ŃL�����b�g�`��ʒu��␳����
	}
	*/
}

//#913
/**
�i���S�̂�I��
*/
void	AView_Text::SelectWholeParagraph()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( spt.x != ept.x || spt.y != ept.y )//B0327 �����ǉ��i��s�ŃL�����b�g��Ԃ̏ꍇ��ept.y--���Ȃ��悤�ɂ���j
	{
		if( ept.x == 0 && ept.y > 0 )   ept.y--;
	}
	ATextIndex	spos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y));
	ATextIndex	epos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ept.y)+1);
	SetSelect(spos,epos);
}

//#913
/**
�s�S�̂�I��
*/
void	AView_Text::SelectWholeLine()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( ept.x == 0 && ept.y > 0 && ept.y > spt.y )   ept.y--;
	spt.x = 0;
	ept.x = 0;
	ept.y++;
	//B0361
	if( ept.y >= GetTextDocumentConst().SPI_GetLineCount() )
	{
		ept.y = GetTextDocumentConst().SPI_GetLineCount()-1;
		ept.x = GetTextDocumentConst().SPI_GetLineLengthWithLineEnd(ept.y);
	}
	
	SetSelect(spt,ept);
}

//#913
/**
�}�[�N��ݒ�
*/
void	AView_Text::SetMark()
{
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetSelect(spt,ept);
	AIndex	oldMarkStartLineIndex = mMarkStartTextPoint.y;
	AIndex	oldMarkEndLineIndex = mMarkEndTextPoint.y;
	mMarkStartTextPoint = spt;
	mMarkEndTextPoint = ept;
	SPI_RefreshLine(oldMarkStartLineIndex);
	SPI_RefreshLine(oldMarkEndLineIndex);
	SPI_RefreshLine(mMarkStartTextPoint.y);
	SPI_RefreshLine(mMarkEndTextPoint.y);
}

//#913
/**
�}�[�N�ƑI��͈͂�����
*/
void	AView_Text::SwapMark()
{
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetSelect(spt,ept);
	if( GetTextDocumentConst().SPI_IsTextPointValid(mMarkStartTextPoint) == true &&
	   GetTextDocumentConst().SPI_IsTextPointValid(mMarkEndTextPoint) == true )
	{
		SPI_SetSelect(mMarkStartTextPoint,mMarkEndTextPoint);
	}
	AIndex	oldMarkStartLineIndex = mMarkStartTextPoint.y;
	AIndex	oldMarkEndLineIndex = mMarkEndTextPoint.y;
	mMarkStartTextPoint = spt;
	mMarkEndTextPoint = ept;
	SPI_RefreshLine(oldMarkStartLineIndex);
	SPI_RefreshLine(oldMarkEndLineIndex);
	SPI_RefreshLine(mMarkStartTextPoint.y);
	SPI_RefreshLine(mMarkEndTextPoint.y);
}

//#913
/**
�}�[�N�܂ł�I��
*/
void	AView_Text::SelectToMark()
{
	if( GetTextDocumentConst().SPI_IsTextPointValid(mMarkStartTextPoint) == true &&
	   GetTextDocumentConst().SPI_IsTextPointValid(mMarkEndTextPoint) == true )
	{
		ATextPoint	spt = {0}, ept = {0};
		SPI_GetSelect(spt,ept);
		ATextPoint	dummy = {0};
		ADocument_Text::OrderTextPoint(mMarkStartTextPoint,spt,spt,dummy);
		ADocument_Text::OrderTextPoint(mMarkEndTextPoint,ept,dummy,ept);
		SetSelect(spt,ept);
	}
}

//#858
#if 0
#pragma mark ===========================

#pragma mark ---�tⳎ�

//#138
/**
�tⳎ��\���X�V

DrawPostProcess����A�tⳎ��f�[�^�X�V���Ƀh�L�������g�̃��\�b�h����R�[�������
View�ɂ́A���ݕ\�����̕����̂݁A�f�[�^�����݂���B�X�N���[���A�E�g�����ƕtⳎ��E�C���h�E�͍폜�����B�r���[��Hide����Ă��A�tⳎ��E�C���h�E�͍폜�����B
*/
void	AView_Text::SPI_UpdateFusen( const ABool inUpdateAll )
{
	//�tⳎ��f�[�^�X�V�����́A�ŏ��ɕtⳎ���S�č폜����B
	if( inUpdateAll == true )
	{
		//�tⳎ���S�č폜����B
		while( mFusenArray_WindowID.GetItemCount() > 0 )
		{
			SPI_DeleteFusenWindow(mFusenArray_WindowID.Get(0));
		}
	}
	//TextView��bounds�擾
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	//�\���ς݃t���O�쐬
	ABoolArray	shownFlagArray;
	for( AIndex i = 0; i < mFusenArray_CommentIndex.GetItemCount(); i++ )
	{
		shownFlagArray.Add(false);
	}
	//TextView�̕\���͈͍s���擾
	AIndex	startLineIndex = GetLineIndexByImageY(imageFrameRect.top);
	AIndex	endLineIndex = GetLineIndexByImageY(imageFrameRect.bottom);//#450 imageFrameRect.bottom/GetLineHeightWithMargin();
	for( AIndex lineIndex = startLineIndex; lineIndex <= endLineIndex; lineIndex++ )
	{
		//�s�ɑΉ�����R�����gIndex�擾
		AIndex	commentIndex = GetTextDocument().SPI_FindExternalCommentIndexByLineIndex(lineIndex);
		if( commentIndex != kIndex_Invalid )
		{
			//TextView�����z���Index���擾
			AIndex	index = mFusenArray_CommentIndex.Find(commentIndex);
			if( index == kIndex_Invalid )
			{
				//AView_Text���̔z��Ƀf�[�^�����̏ꍇ
				
				//AWindow_Fusen����
				AFusenWindowFactory	windowfactory(GetObjectID(),
									GetTextDocumentConst().SPI_GetProjectObjectID(),
									GetTextDocumentConst().SPI_GetProjectRelativePath(),
									GetTextDocumentConst().SPI_GetExternalCommentModuleName(commentIndex),
									GetTextDocumentConst().SPI_GetExternalCommentOffset(commentIndex));
				AObjectID	winID = GetApp().NVI_CreateWindow(windowfactory);
				GetApp().NVI_GetWindowByID(winID).NVI_Create(kObjectID_Invalid);
				GetApp().NVI_GetWindowByID(winID).NVI_SetAsStickyWindow(NVM_GetWindow().GetObjectID());
				//�����z��Ƀf�[�^�ǉ�
				mFusenArray_CommentIndex.Add(commentIndex);
				mFusenArray_WindowID.Add(winID);
				AWindowPoint	windowPoint = {0,0};
				mFusenArray_WindowOffset.Add(windowPoint);
				//�tⳎ��̈ʒu�ݒ�
				SPI_RepositionFusenWindow(winID);
				//�\��
				if( NVI_IsVisible() == true && NVI_IsFocusActive() == true )
				{
					GetApp().NVI_GetWindowByID(winID).NVI_Show(false);
				}
				else
				{
					GetApp().NVI_GetWindowByID(winID).NVI_Hide();
				}
				//�\���ς݃t���OOn
				shownFlagArray.Add(true);
			}
			else
			{
				//AView_Text���̔z��Ƀf�[�^�L��̏ꍇ
				
				//Sticky�E�C���h�E�̈ʒu�␳
				SPI_RepositionFusenWindow(mFusenArray_WindowID.Get(index));
				//�\��
				if( NVI_IsVisible() == true && NVI_IsFocusActive() == true )
				{
					GetApp().NVI_GetWindowByID(mFusenArray_WindowID.Get(index)).NVI_Show(false);
				}
				else
				{
					GetApp().NVI_GetWindowByID(mFusenArray_WindowID.Get(index)).NVI_Hide();
				}
				//�\���ς݃t���OOn
				shownFlagArray.Set(index,true);
			}
		}
	}
	//�\�����Ȃ�����Sticky�E�C���h�E�͍폜����
	for( AIndex i = 0; i < shownFlagArray.GetItemCount(); )
	{
		if( shownFlagArray.Get(i) == false )
		{
			shownFlagArray.Delete1(i);
			SPI_DeleteFusenWindow(mFusenArray_WindowID.Get(i));
		}
		else
		{
			i++;
		}
	}
}

//#138
/**
�tⳎ���Show/Hide�X�V
*/
void	AView_Text::SPI_UpdateFusenVisible()
{
	for( AIndex i = 0; i < mFusenArray_CommentIndex.GetItemCount(); i++ )
	{
		if( NVI_IsVisible() == true )
		{
			GetApp().NVI_GetWindowByID(mFusenArray_WindowID.Get(i)).NVI_Show();
		}
		else
		{
			GetApp().NVI_GetWindowByID(mFusenArray_WindowID.Get(i)).NVI_Hide();
		}
	}
}

/**
�tⳎ��폜
*/
void	AView_Text::SPI_DeleteFusenWindow( const AWindowID inFusenWindowID )
{
	//AView_Text�������z��폜
	AIndex index = mFusenArray_WindowID.Find(inFusenWindowID);
	if( index == kIndex_Invalid )   { _ACError("Fusen not found",this); return; }
	//�����z�񂩂�폜
	mFusenArray_CommentIndex.Delete1(index);
	mFusenArray_WindowID.Delete1(index);
	mFusenArray_WindowOffset.Delete1(index);
	//�tⳃE�C���h�E�폜
	GetApp().NVI_DeleteWindow(inFusenWindowID);
}

/**
�tⳎ��ʒu�����݂�TextView�̃X�N���[���ʒu�ɍ��킹��

@param inFusenWindowID �tⳎ���WindowID
*/
void	AView_Text::SPI_RepositionFusenWindow( const AWindowID inFusenWindowID )
{
	AIndex index = mFusenArray_WindowID.Find(inFusenWindowID);
	if( index == kIndex_Invalid )   { _ACError("Fusen not found",this); return; }
	
	//TextView��bounds�擾
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	//�tⳎ��ʒu�v�Z
	ALocalPoint	localPoint;
	AWindowPoint	windowPoint;
	localPoint.x = localFrameRect.right - GetApp().NVI_GetWindowByID(mFusenArray_WindowID.Get(index)).NVI_GetWindowWidth() + 24;
	localPoint.y = GetImageYByLineIndex(GetTextDocumentConst().SPI_GetExternalCommentLineIndex(mFusenArray_CommentIndex.Get(index))) - imageFrameRect.top;
	NVM_GetWindow().NVI_GetWindowPointFromControlLocalPoint(NVI_GetControlID(),localPoint,windowPoint);
	//�����z��ɕۑ����Ă������ʒu����ύX������΁A�ʒu�ړ�
	AWindowPoint	oldwpt = mFusenArray_WindowOffset.Get(index);
	if( oldwpt.x != windowPoint.x || oldwpt.y != windowPoint.y )
	{
		GetApp().NVI_GetWindowByID(NVM_GetWindow().GetObjectID()).NVI_SetChildStickyWindowOffset(inFusenWindowID,windowPoint);
		mFusenArray_WindowOffset.Set(index,windowPoint);
	}
}

/**
�tⳎ��ʒu����A���W���[�����E�I�t�Z�b�g���擾

�tⳎ��ҏW�E�C���h�E�̈ʒu�𓮂������Ƃ��ɁA�tⳎ��ҏW�E�C���h�E����A���W���[�����E�I�t�Z�b�g��m�邽�߂Ɏg�p�����B
@param inFusenEditWindowPos �tⳎ��ҏW�E�C���h�E�̈ʒu
@param outModuleName ���W���[����
@param outOffset ���W���[��������̃I�t�Z�b�g
*/
void	AView_Text::SPI_GetFusenModuleAndOffset( const APoint inFusenEditWindowPos, AText& outModuleName, ANumber& outOffset )
{
	//�e�L�X�g�E�C���h�E�ʒu�擾
	APoint	docWinPos;
	NVM_GetWindow().NVI_GetWindowPosition(docWinPos);
	//���Έʒu�擾
	AWindowPoint	wpt;
	wpt.x = inFusenEditWindowPos.x - docWinPos.x;
	wpt.y = inFusenEditWindowPos.y - docWinPos.y;
	//TextView��LocalPoint�֕ϊ���ImagePoint�֕ϊ���TextPoint�֕ϊ�
	ALocalPoint	lpt;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),wpt,lpt);
	AImagePoint	ipt;
	NVM_GetImagePointFromLocalPoint(lpt,ipt);
	ATextPoint	tpt;
	GetTextPointFromImagePoint(ipt,tpt);
	//�s�ԍ����烂�W���[�����E�I�t�Z�b�g�擾
	AText	newModuleName;
	//ANumber	newOffset = 0;
	GetTextDocumentConst().SPI_GetModuleNameAndOffsetByLineIndex(tpt.y,outModuleName,outOffset);
}

/**
�V�K�tⳎ��^�܂��͊����tⳎ��ҏW

@param inLineIndex �s�ԍ�
*/
void	AView_Text::EditOrNewFusen( const AIndex inLineIndex )
{
	//�Y���s�̃R�����gIndex�擾
	AIndex	commentIndex = GetTextDocument().SPI_FindExternalCommentIndexByLineIndex(inLineIndex);
	if( commentIndex == kIndex_Invalid )
	{
		//�R�����g�����݂��Ă��Ȃ���ΐV�K�tⳎ��f�[�^�쐬
		
		//�s�ԍ����烂�W���[�����E�I�t�Z�b�g�擾
		AText	moduleName;
		ANumber	offset = 0;
		GetTextDocumentConst().SPI_GetModuleNameAndOffsetByLineIndex(inLineIndex,moduleName,offset);
		//�V�K�tⳃf�[�^�ǉ�
		AText	comment;
		GetApp().SPI_SetExternalCommentToProjectDB(GetTextDocumentConst().SPI_GetProjectObjectID(),
				GetTextDocumentConst().SPI_GetProjectRelativePath(),
				moduleName,offset,comment);
		//�Y���s�̃R�����gIndex�擾
		commentIndex = GetTextDocument().SPI_FindExternalCommentIndexByLineIndex(inLineIndex);
		if( commentIndex == kIndex_Invalid )
		{
			_ACError("",this);
			return;
		}
	}
	//�tⳎ��\���E�C���h�E��winID���擾���A�ҏW���w��
	AWindowID	fusenWindowID = mFusenArray_WindowID.Get(mFusenArray_CommentIndex.Find(commentIndex));
	GetApp().SPI_GetFusenWindow(fusenWindowID).SPI_EditFusen();
}
#endif

/**
���o�����X�g�z�o�[�X�V������
*/
void	AView_Text::SPI_DoListViewHoverUpdated( const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex )
{
	/*�����r��
	AIndex	hoverStartLineIndex = kIndex_Invalid, hoverEndLineIndex = kIndex_Invalid;
	if( inHoverStartDBIndex != kIndex_Invalid )
	{
		AUniqID	identifier = GetTextDocumentConst().SPI_GetJumpMenuIdentifierUniqID(inHoverStartDBIndex);
		ATextPoint	spt = {0}, ept = {0};
		GetTextDocumentConst().SPI_GetGlobalIdentifierRange(identifier,spt,ept);
		hoverStartLineIndex = spt.y;
	}
	if( inHoverEndDBIndex != kIndex_Invalid )
	{
		AUniqID	identifier = GetTextDocumentConst().SPI_GetJumpMenuIdentifierUniqID(inHoverEndDBIndex);
		ATextPoint	spt = {0}, ept = {0};
		GetTextDocumentConst().SPI_GetGlobalIdentifierRange(identifier,spt,ept);
		hoverEndLineIndex = ept.y;
	}
	GetLineNumberView().SPI_DoListViewHoverUpdated(hoverStartLineIndex,hoverEndLineIndex);
	*/
}

#pragma mark ===========================

#pragma mark ---�simage���i�܂肽���݁E�s���ƍ����Ή��j
//#450

/**
�s���ƍ����Eimage�ʒu��񏉊���
*/
void	AView_Text::SPI_InitLineImageInfo()
{
	//mLineHeight���̍X�V
	SPI_UpdateTextDrawProperty();
	//
	mLineImageInfo_Height.DeleteAll();
	mLineImageInfo_ImageY.DeleteAll();
	mLineImageInfo_Collapsed.DeleteAll();
	
	//#695 �s���g�厞�̍Ċ��蓖�đ����ʂ̐ݒ�
	AItemCount	lineReallocateStep = GetTextDocumentConst().SPI_GetLineReallocateStep();
	mLineImageInfo_Height.SetReallocateStep(lineReallocateStep);
	mLineImageInfo_ImageY.SetReallocateStep(lineReallocateStep);
	mLineImageInfo_Collapsed.SetReallocateStep(lineReallocateStep);
	
	//#695
	ANumber	lineHeightPlusMargin = GetLineHeightPlusMargin();
	
	//
	ANumber	imageY = 0;
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	{
		//LineImageInfo�f�[�^Reserve#695
		mLineImageInfo_Height.Reserve(0,lineCount);
		mLineImageInfo_ImageY.Reserve(0,lineCount);
		mLineImageInfo_Collapsed.Reserve(0,lineCount);
		//�|�C���^�擾
		AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
		ANumber*	p_height = arrayptr_height.GetPtr();
		AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
		ANumber*	p_imageY = arrayptr_imageY.GetPtr();
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			ANumber	lineHeight = lineHeightPlusMargin;//#695 CalcLineHeight(lineIndex);
			/*#493��U�@�\drop
			if( foldingLevelRef.Get(lineIndex) == kFoldingLevel_Header )
			{
			lineHeight *= multiplyRef.Get(lineIndex);
			lineHeight /= 100;
			}
			*/
			/*#695
			mLineImageInfo_Height.Add(lineHeight);
			mLineImageInfo_ImageY.Add(imageY);
			mLineImageInfo_Collapsed.Add(false);
			*/
			p_height[lineIndex] = lineHeight;
			p_imageY[lineIndex] = imageY;
			p_collapsed[lineIndex] = false;
			//
			imageY += lineHeight;
		}
	}
	//
	mLineImageInfo_Height.Add(0);
	mLineImageInfo_ImageY.Add(imageY);
	mLineImageInfo_Collapsed.Add(false);
	//
	SPI_UpdateImageSize();
}

/*#493��U�@�\drop 
**
�e�s�̍������v�Z
*
ANumber	AView_Text::CalcLineHeight( const AIndex inLineIndex ) const
{
	ANumber	lineHeight = (GetLineHeightPlusMargin())
			* GetTextDocumentConst().SPI_GetLineFontSizeMultiply(inLineIndex);
	return lineHeight;
}
*/

/**
�s���ƍ����Eimage�ʒu���X�V
@param inStartLineIndex �ҏW�J�n�sindex
@param inInsertedLineCount �ǉ��s��
@param inUpdateStartLineIndex �s���X�V�J�nindex
*/
ANumber	AView_Text::SPI_UpdateLineImageInfo( const AIndex inStartLineIndex, const AItemCount inInsertedLineCount,
		const AIndex inUpdateStartLineIndex )
{
	//�s�}���E�폜���A�}���E�폜�J�n�s��collapsed�t���O��ON�Ȃ�A���炩����expand����
	//�i�}�����F�J�n�s�ȍ~�ɍs�}������̂ŁAcollapse�������Ă����Ȃ��ƁAcollapse�t���OON�̌�ɍs�ǉ����Ă��܂��B
	// �폜���F�폜�J�n�s��mLineImageInfo_xxx�͍폜�����A����ȍ~�̍폜�s��mLineImageInfo_xxx���폜�����̂ŁA
	// �폜�J�n�s��collapse�������Ă����Ȃ��ƁAcollapse�\�����c���Ă��܂��B�icollpase����Ă����s�͍폜�����B�j
	if( inInsertedLineCount != 0 )
	{
		if( mLineImageInfo_Collapsed.Get(inStartLineIndex) == true )
		{
			SPI_ExpandCollapse(inStartLineIndex,kExpandCollapseType_ForceExpand,false);
		}
	}
	
	//#695
	ANumber	lineHeightPlusMargin = GetLineHeightPlusMargin();
	
	//#695 �s���g�厞�̍Ċ��蓖�đ����ʂ̐ݒ�
	AItemCount	lineReallocateStep = GetTextDocumentConst().SPI_GetLineReallocateStep();
	mLineImageInfo_Height.SetReallocateStep(lineReallocateStep);
	mLineImageInfo_ImageY.SetReallocateStep(lineReallocateStep);
	mLineImageInfo_Collapsed.SetReallocateStep(lineReallocateStep);
	
	//inUpdateStartLineIndex�`inStartLineIndex�̍X�V
	ANumber	imageY = mLineImageInfo_ImageY.Get(inUpdateStartLineIndex);
	AIndex	lineIndex = inUpdateStartLineIndex;
	{
		//�|�C���^�擾
		AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
		ANumber*	p_height = arrayptr_height.GetPtr();
		AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
		ANumber*	p_imageY = arrayptr_imageY.GetPtr();
		for( ; lineIndex <= inStartLineIndex; lineIndex++ )
		{
			ANumber	lineHeight = lineHeightPlusMargin;//#695 CalcLineHeight(lineIndex);
			/*#493��U�@�\drop
			if( foldingLevelRef.Get(lineIndex) == kFoldingLevel_Header )
			{
			lineHeight *= multiplyRef.Get(lineIndex);
			lineHeight /= 100;
			}
			*/
			//
			//���X����0�i��collapse���j�Ȃ獂��0�̂܂܂ɂ���
			//if( mLineImageInfo_Height.Get(lineIndex) == 0 )
			if( p_height[lineIndex] == 0 )
			{
				lineHeight = 0;
			}
			//
			//mLineImageInfo_Height.Set(lineIndex,lineHeight);
			//mLineImageInfo_ImageY.Set(lineIndex,imageY);
			p_height[lineIndex] = lineHeight;
			p_imageY[lineIndex] = imageY;
			imageY += lineHeight;
		}
	}
	//�s�폜
	if( inInsertedLineCount < 0 )
	{
		mLineImageInfo_Height.Delete(lineIndex,-inInsertedLineCount);
		mLineImageInfo_ImageY.Delete(lineIndex,-inInsertedLineCount);
		mLineImageInfo_Collapsed.Delete(lineIndex,-inInsertedLineCount);
	}
	//�s�ǉ�
	if( inInsertedLineCount > 0 )
	{
		//LineImageInfo�f�[�^Reserve#695
		mLineImageInfo_Height.Reserve(lineIndex,inInsertedLineCount);
		mLineImageInfo_ImageY.Reserve(lineIndex,inInsertedLineCount);
		mLineImageInfo_Collapsed.Reserve(lineIndex,inInsertedLineCount);
		//�|�C���^�擾
		AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
		ANumber*	p_height = arrayptr_height.GetPtr();
		AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
		ANumber*	p_imageY = arrayptr_imageY.GetPtr();
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		for( AIndex i = 0; i < inInsertedLineCount; i++ )
		{
			ANumber	lineHeight = lineHeightPlusMargin;//#695 CalcLineHeight(lineIndex);
			/*#493��U�@�\drop
			if( foldingLevelRef.Get(lineIndex) == kFoldingLevel_Header )
			{
				lineHeight *= multiplyRef.Get(lineIndex);
				lineHeight /= 100;
			}
			 */
			/*#695
			mLineImageInfo_Height.Insert1(lineIndex,lineHeight);
			mLineImageInfo_ImageY.Insert1(lineIndex,imageY);
			mLineImageInfo_Collapsed.Insert1(lineIndex,false);
			*/
			p_height[lineIndex] = lineHeight;
			p_imageY[lineIndex] = imageY;
			p_collapsed[lineIndex] = false;
			//
			imageY += lineHeight;
			lineIndex++;
		}
	}
	//
	//�폜�E�ǉ��s��肠�Ƃ�imageY�����炵�Ă����i���ꂪ����΂����̂݁j
	ANumber	delta = imageY - mLineImageInfo_ImageY.Get(lineIndex);
	if( delta != 0 )
	{
		AStArrayPtr<ANumber>	arrayptr(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());//#695
		ANumber*	p = arrayptr.GetPtr();
		AItemCount	itemCount = mLineImageInfo_ImageY.GetItemCount();//#695
		for( ; lineIndex < itemCount; lineIndex++ )
		{
			//#695 mLineImageInfo_ImageY.Set(lineIndex,mLineImageInfo_ImageY.Get(lineIndex)+delta);
			p[lineIndex] += delta;
		}
	}
	//�s�폜�̏ꍇ�A�폜�����s�̂Ȃ��ɁAcollapsed�J�n�s���܂܂�Ă���\��������A�폜�ςݍs�̎��̍s�ȍ~��collapsed����Ă���\��������̂ŁA�������������
	if( inInsertedLineCount < 0 )
	{
		if( mLineImageInfo_Height.Get(inStartLineIndex+1) == 0 )
		{
			SPI_ExpandCollapse(inStartLineIndex,kExpandCollapseType_ForceExpand,false);
		}
	}
	//imagesize�̍X�V
	SPI_UpdateImageSize();
	//�폜�E�ǉ��s��肠�Ƃ�imageY�ɂ��ꂪ��������true��Ԃ�
	return delta;
}

/**
�܂肽���܂ꂽ�s���ǂ������擾
*/
ABool	AView_Text::SPI_IsCollapsedLine( const AIndex inLineIndex ) const
{
	return mLineImageInfo_Collapsed.Get(inLineIndex);
}

/**
�܂肽����expand/collapse
@note inLineIndex�͐܂肽���݊J�n�s�A�܂��́A�I���s
*/
void	AView_Text::SPI_ExpandCollapse( const AIndex inLineIndex, const AExpandCollapseType inExpandCollapseType,
		const ABool inRedraw )
{
	//�N���b�N�s�̌��X��image y���L��
	ANumber	originalImageY = GetImageYByLineIndex(inLineIndex);
	
	//�܂肽���݊J�n�s�^�I���s
	AIndex	foldingStartLineIndex = inLineIndex;
	AIndex	foldingEndLineIndex = inLineIndex;
	//�N���b�N�s�̐܂肽���݃��x���擾
	AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(inLineIndex);
	ABool	isFoldingStart = SPI_IsFoldingStart(foldingLevel);
	ABool	isFoldingEnd = SPI_IsFoldingEnd(foldingLevel);
	//�w��s���܂肽���݊J�n�ł��I���ł��Ȃ��A���A�����܂肽���݉����̏ꍇ�A�w��s����ōŏ���collapsed��Ԃł͂Ȃ��Ȃ�s���I���s�ɂ���
	if( (isFoldingStart == false && isFoldingEnd == false ) || inExpandCollapseType == kExpandCollapseType_ForceExpand )
	{
		if( inLineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
		{
			if( mLineImageInfo_Height.Get(inLineIndex+1) == 0 )
			{
				//�ŏ���collapsed��Ԃł͂Ȃ��Ȃ�s���I���s�ɂ���
				foldingEndLineIndex = inLineIndex+1;
				for( ; foldingEndLineIndex < GetTextDocumentConst().SPI_GetLineCount(); foldingEndLineIndex++ )
				{
					if( mLineImageInfo_Height.Get(foldingEndLineIndex) > 0 )
					{
						break;
					}
				}
			}
		}
	}
	//�܂肽���݊J�n�s�N���b�N�̏ꍇ�A�܂肽���ݏI���s���擾
	else if( isFoldingStart == true )
	{
		foldingEndLineIndex = SPI_FindFoldingEndLineIndex(inLineIndex);
	}
	//�܂肽���ݏI���s�N���b�N�̏ꍇ�A�܂肽���݊J�n�s���擾
	else if( isFoldingEnd == true )
	{
		foldingStartLineIndex = SPI_FindFoldingStartLineIndex(inLineIndex);
	}
	
	//�J�n�s�^�I���s�����Ɏ��s�����ꍇ�͉����������^�[��
	if( foldingStartLineIndex == foldingEndLineIndex )
	{
		//�w��s���܂肽���ݏ�ԂƂȂ��Ă���ꍇ�́A�ُ��ԂȂ̂ŁA�܂肽���ݏ�Ԃ���������B
		if( mLineImageInfo_Collapsed.Get(inLineIndex) == true )
		{
			mLineImageInfo_Collapsed.Set(inLineIndex,false);
			//textview, linenumber view, diff view��\���X�V
			SPI_RefreshTextView();
		}
		//
		return;
	}
	
	//�s���擾
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	//�܂肽���݊J�n�ʒu�̎��̍s����lineIndex�J�n
	AIndex lineIndex = foldingStartLineIndex+1;
	if( lineIndex >= lineCount )
	{
		return;
	}
	
	//imageY
	ANumber	imageY = mLineImageInfo_ImageY.Get(lineIndex);
	
	//collapse���邩expand���邩������
	ABool	collapse = false;
	switch(inExpandCollapseType)
	{
	  case kExpandCollapseType_Switch:
		{
			collapse = !(mLineImageInfo_Collapsed.Get(foldingStartLineIndex));
			break;
		}
	  case kExpandCollapseType_Collapse:
		{
			collapse = true;
			break;
		}
	  case kExpandCollapseType_Expand:
	  case kExpandCollapseType_ForceExpand:
		{
			collapse = false;
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	
	//#695
	ANumber	lineHeightPlusMargin = GetLineHeightPlusMargin();
	
	{
		//�|�C���^�擾
		AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
		ANumber*	p_height = arrayptr_height.GetPtr();
		AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
		ANumber*	p_imageY = arrayptr_imageY.GetPtr();
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		if( collapse == false )
		{
			//Expand����ꍇ
			
			//�f�[�^�ݒ�
			//mLineImageInfo_Collapsed.Set(foldingStartLineIndex,false);
			p_collapsed[foldingStartLineIndex] = false;
			//�܂肽���ݏI���s�܂ł̍s�����𕜌�����
			for( ; lineIndex < foldingEndLineIndex; lineIndex++ )
			{
				//�����EImageY�ݒ�
				ANumber	lineHeight = lineHeightPlusMargin;//#493��U�@�\drop CalcLineHeight(lineIndex);
				//mLineImageInfo_Height.Set(lineIndex,lineHeight);
				//mLineImageInfo_ImageY.Set(lineIndex,imageY);
				p_height[lineIndex] = lineHeight;
				p_imageY[lineIndex] = imageY;
				imageY += lineHeight;
				
				//Expand���������Collapsed���������炻�̕����͍���0�̂܂܂ɂ���
				//if( mLineImageInfo_Collapsed.Get(lineIndex) == true )
				if( p_collapsed[lineIndex] == true )
				{
					//�Ή�����܂肽���ݏI���s���擾
					AIndex	callapseEndLineIndex = SPI_FindFoldingEndLineIndex(lineIndex);
					
					//Collapse�}�[�N�̎��̍s����ACollapse�}�[�N�̍s�̃C���f���g�Ɠ����C���f���g�ʂ̍s�܂ŁA����0�ݒ肷��
					if( lineIndex+1 < lineCount && callapseEndLineIndex > lineIndex )
					{
						lineIndex++;
						for( ; lineIndex < callapseEndLineIndex; lineIndex++ )
						{
							//�����EImageY�ݒ�
							//mLineImageInfo_Height.Set(lineIndex,0);
							//mLineImageInfo_ImageY.Set(lineIndex,imageY);
							p_height[lineIndex] = 0;
							p_imageY[lineIndex] = imageY;
						}
						lineIndex--;
						/*
						//Collapse�}�[�N�̍s�̃C���f���g�Ɠ����C���f���g�ʂɂȂ����s�̍����ݒ�
						if( lineIndex < lineCount )
						{
							ANumber	lineHeight = lineHeightPlusMargin;//#493��U�@�\drop CalcLineHeight(lineIndex);
							//mLineImageInfo_Height.Set(lineIndex,lineHeight);
							//mLineImageInfo_ImageY.Set(lineIndex,imageY);
							p_height[lineIndex] = lineHeight;
							p_imageY[lineIndex] = imageY;
							imageY += lineHeight;
						}
						else
						{
							break;
						}*/
					}
					else
					{
						//�܂肽���ݏI���s�𐳂����������Ȃ��ꍇ�́A
						//�܂肽���ݏ�ԂƂȂ��Ă��邱�Ƃ��ُ�Ȃ̂ŁA�܂肽���ݏ�Ԃ���������B
						p_collapsed[lineIndex] = false;
					}
				}
			}
		}
		else
		{
			//Collapse����ꍇ
			
			//�f�[�^�ݒ�
			//mLineImageInfo_Collapsed.Set(foldingStartLineIndex,true);
			p_collapsed[foldingStartLineIndex] = true;
			//�܂肽���ݏI���s�܂ł̍s������0�ɂ���
			for( ; lineIndex < foldingEndLineIndex; lineIndex++ )
			{
				//�����EImageY�ݒ�
				//mLineImageInfo_Height.Set(lineIndex,0);
				//mLineImageInfo_ImageY.Set(lineIndex,imageY);
				p_height[lineIndex] = 0;
				p_imageY[lineIndex] = imageY;
			}
		}
		
		//�ȍ~�̍s��imageY�����炷
		ANumber	delta = imageY - mLineImageInfo_ImageY.Get(lineIndex);
		if( delta != 0 )
		{
			//ImageY�ݒ�
			for( ; lineIndex < mLineImageInfo_ImageY.GetItemCount(); lineIndex++ )
			{
				//mLineImageInfo_ImageY.Set(lineIndex,mLineImageInfo_ImageY.Get(lineIndex)+delta);
				p_imageY[lineIndex] += delta;
			}
		}
	}
	//ImageSize�X�V
	SPI_UpdateImageSize();
	
	if( inRedraw == true )
	{
		//�L�����b�g�E�I���ʒu�␳
		if( mLineImageInfo_Collapsed.Get(foldingStartLineIndex) == true )
		{
			//Collapse�̏ꍇ
			ATextPoint	spt,ept;
			SPI_GetSelect(spt,ept);
			if( (spt.y > foldingStartLineIndex && spt.y < foldingEndLineIndex) ||
						(ept.y > foldingStartLineIndex && ept.y < foldingEndLineIndex) )
			{
				spt.x = 0;
				spt.y = foldingStartLineIndex;
				SetCaretTextPoint(spt);
				//AdjustScroll(spt);
			}
			else
			{
				if( IsCaretMode() == true )
				{
					//�L�����b�gdraw�ʒu�X�V�̂���
					UpdateCaretImagePoint();
				}
			}
		}
		else
		{
			//Expand�̏ꍇ
			if( IsCaretMode() == true )
			{
				//�L�����b�gdraw�ʒu�X�V�̂���
				UpdateCaretImagePoint();
			}
		}
		
		//�N���b�N�s�̃X�N���[���ʒu���Œ肷��
		NVI_Scroll(0,GetImageYByLineIndex(inLineIndex)-originalImageY,false);
		//image y�ɍ���������΁Aredraw
		//if( delta != 0 )
		{
			//textview, linenumber view, diff view��\���X�V
			SPI_RefreshTextView();
		}
	}
}

/**
�܂肽���݊J�n���ǂ����𔻒f
*/
ABool	AView_Text::SPI_IsFoldingStart( const AFoldingLevel inFoldingLevel ) const
{
	ABool	result = false;
	if( (inFoldingLevel&kFoldingLevel_Header) != 0 || (inFoldingLevel&kFoldingLevel_BlockStart) != 0 ||
				(inFoldingLevel&kFoldingLevel_CommentStart) != 0 || (inFoldingLevel&kFoldingLevel_IfStart) != 0 )
	{
		result = true;
	}
	return result;
}

/**
�܂肽���ݏI�����ǂ����𔻒f
*/
ABool	AView_Text::SPI_IsFoldingEnd( const AFoldingLevel inFoldingLevel ) const
{
	ABool	result = false;
	if( (inFoldingLevel&kFoldingLevel_BlockEnd) != 0 || (inFoldingLevel&kFoldingLevel_CommentEnd) != 0 ||
				(inFoldingLevel&kFoldingLevel_IfEnd) != 0 )
	{
		result = true;
	}
	return result;
}

/**
�w��s�i�܂肽���݊J�n�j�ɑ΂���܂肽���ݏI���s������
*/
AIndex	AView_Text::SPI_FindFoldingEndLineIndex( const AIndex inStartLineIndex ) const
{
	//�w��s�̐܂肽���݃��x���擾
	AFoldingLevel	startFoldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(inStartLineIndex);
	//�܂肽���݊J�n�łȂ���Ή��������Ƀ��^�[��
	if( SPI_IsFoldingStart(startFoldingLevel) == false )   return inStartLineIndex;
	
	//�܂肽���݊J�n�s�̃C���f���g���x�����擾
	AItemCount	indentCount = GetTextDocumentConst().SPI_GetTextInfoIndentCount(inStartLineIndex);
	//�܂肽���݊J�n�s�̃A�E�g���C�����x�����擾
	AIndex	startOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inStartLineIndex);
	//�܂肽���݊J�n�s��directive���x�����擾
	AIndex	startDirectiveLevel = GetTextDocumentConst().SPI_GetDirectiveLevel(inStartLineIndex);
	
	//�s���擾
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	//�܂肽���݊J�n�s�̎��̍s����n�߂āA�v���X�����ɐ܂肽���ݏI���s������
	AIndex lineIndex = inStartLineIndex+1;
	for( ; lineIndex < lineCount; lineIndex++ )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//==================���o��==================
		if( (startFoldingLevel&kFoldingLevel_Header) != 0 )
		{
			//�J�n�s�̌��o���A�E�g���C�����x���Ɠ������x���܂��͏�ʂ̌��o���Ȃ�I���Ƃ���
			if( (foldingLevel&kFoldingLevel_Header) != 0 )
			{
				if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) <= startOutlineLevel)
				{
					return lineIndex;
				}
			}
			//���̍s�̃C���f���g���x�����J�n�s�̃C���f���g���x�������������Ȃ�����I���Ƃ���
			if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
			{
				if( indentCount > GetTextDocumentConst().SPI_GetTextInfoIndentCount(lineIndex+1) )
				{
					return lineIndex;
				}
			}
			//
			if( lineIndex == lineCount-1 )
			{
				return lineIndex;
			}
		}
		//==================�u���b�N==================
		if( (startFoldingLevel&kFoldingLevel_BlockStart) != 0 )
		{
			//���肽���݊J�n�s�Ɠ����C���f���g���x���A���Ablockend�Ȃ�I���Ƃ���
			if( (foldingLevel&kFoldingLevel_BlockEnd) != 0 )
			{
				if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
				{
					if( indentCount == GetTextDocumentConst().SPI_GetTextInfoIndentCount(lineIndex+1) )
					{
						return lineIndex;
					}
				}
			}
		}
		//==================#if==================
		if( (startFoldingLevel&kFoldingLevel_IfStart) != 0 )
		{
			if( (foldingLevel&kFoldingLevel_IfEnd) != 0 )
			{
				//���̍s��directive���x�����J�n�s��directive���x���Ɠ����Ȃ�I���Ƃ���
				if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
				{
					if( startDirectiveLevel == GetTextDocumentConst().SPI_GetDirectiveLevel(lineIndex+1) )
					{
						return lineIndex;
					}
				}
			}
		}
		//==================�R�����g==================
		if( (startFoldingLevel&kFoldingLevel_CommentStart) != 0 )
		{
			if( (foldingLevel&kFoldingLevel_CommentEnd) != 0 )
			{
				return lineIndex;
			}
		}
	}
	return inStartLineIndex;
}

/**
�w��s�i�܂肽���ݏI���j�ɑ΂���܂肽���݊J�n�s������
*/
AIndex	AView_Text::SPI_FindFoldingStartLineIndex( const AIndex inEndLineIndex ) const
{
	//�w��s�̐܂肽���݃��x���擾
	AFoldingLevel	endFoldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(inEndLineIndex);
	//�܂肽���ݏI���łȂ���Ή��������Ƀ��^�[��
	if( SPI_IsFoldingEnd(endFoldingLevel) == false )   return inEndLineIndex;
	
	//�܂肽���ݏI���s�̃C���f���g���x�����擾
	AItemCount	indentCount = GetTextDocumentConst().SPI_GetTextInfoIndentCount(inEndLineIndex);
	//�܂肽���ݏI���s�̃A�E�g���C�����x�����擾
	AIndex	endOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inEndLineIndex);
	//�܂肽���ݏI���s��directive���x�����擾
	AIndex	endDirectiveLevel = GetTextDocumentConst().SPI_GetDirectiveLevel(inEndLineIndex);
	
	//�܂肽���ݏI���s�̑O�̍s����n�߂āA�}�C�i�X�����ɐ܂肽���݊J�n�s������
	AIndex lineIndex = inEndLineIndex-1;
	for( ; lineIndex > 0; lineIndex-- )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//==================���o��==================
		if( (endFoldingLevel&kFoldingLevel_Header) != 0 )
		{
			//�J�n�s�̌��o���A�E�g���C�����x���Ɠ������x���܂��͏�ʂ̌��o���Ȃ�I���Ƃ���
			if( (foldingLevel&kFoldingLevel_Header) != 0 )
			{
				if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) <= endOutlineLevel )
				{
					return lineIndex;
				}
			}
		}
		//==================�u���b�N==================
		if( (endFoldingLevel&kFoldingLevel_BlockEnd) != 0 )
		{
			//�܂肽���ݏI���s�Ɠ����C���f���g���x���A���Ablock start�Ȃ�I���Ƃ���
			if( (foldingLevel&kFoldingLevel_BlockStart) != 0 )
			{
				if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
				{
					if( indentCount == GetTextDocumentConst().SPI_GetTextInfoIndentCount(lineIndex+1) )
					{
						return lineIndex;
					}
				}
			}
		}
		//==================#endif==================
		if( (endFoldingLevel&kFoldingLevel_IfEnd) != 0 )
		{
			if( (foldingLevel&kFoldingLevel_IfStart) != 0 )
			{
				//���̍s��directive���x�����I���s��directive���x���Ɠ����Ȃ�I���Ƃ���
				if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
				{
					if( endDirectiveLevel == GetTextDocumentConst().SPI_GetDirectiveLevel(lineIndex+1) )
					{
						return lineIndex;
					}
				}
			}
		}
		//==================�R�����g==================
		if( (endFoldingLevel&kFoldingLevel_CommentEnd) != 0 )
		{
			if( (foldingLevel&kFoldingLevel_CommentStart) != 0 )
			{
				return lineIndex;
			}
		}
	}
	return inEndLineIndex;
}

/**
�܂肽���ݏ�Ԏ擾
*/
ABool	AView_Text::SPI_GetFoldingCollapsed( const AIndex inLineIndex ) const
{
	return mLineImageInfo_Collapsed.Get(inLineIndex);
}

/**
*/
void	AView_Text::CollapseAllHeaders()
{
	{
		//�|�C���^�擾
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			//�w�b�_�s�Ȃ�Acollapse
			if( (GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)&kFoldingLevel_Header) != 0 )
			{
				p_collapsed[lineIndex] = true;
			}
		}
	}
	//�S�Ă̍sImageY,�������Acollapsed�p�����[�^�ɏ]���čX�V
	UpdateLineImageInfoAllByCollapsedParameter();
}

/**
SCM�ύX���܂܂Ȃ��֐���collapse
*/
void	AView_Text::CollapseAllHeadersWithMNoChange()
{
	{
		//�|�C���^�擾
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			//�w�b�_�s�A���ASCM�ύX���܂�ł��Ȃ���΁Acollapse
			if( (GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)&kFoldingLevel_Header) != 0 )
			{
				AIndex	endLineIndex = SPI_FindFoldingEndLineIndex(lineIndex);
				if( GetTextDocumentConst().SPI_DiffExistInRange(lineIndex,endLineIndex) == false )
				{
					p_collapsed[lineIndex] = true;
				}
			}
		}
	}
	//�S�Ă̍sImageY,�������Acollapsed�p�����[�^�ɏ]���čX�V
	UpdateLineImageInfoAllByCollapsedParameter();
}

/**
�S��expand
*/
void	AView_Text::ExpandAllFoldings()
{
	ANumber	lineHeight = GetLineHeightPlusMargin();
	ANumber	imageY = 0;
	//�|�C���^�擾
	AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
	ANumber*	p_height = arrayptr_height.GetPtr();
	AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
	ANumber*	p_imageY = arrayptr_imageY.GetPtr();
	AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
	ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
	//
	AItemCount	itemCount = mLineImageInfo_ImageY.GetItemCount();
	for( AIndex lineIndex = 0; lineIndex < itemCount; lineIndex++ )
	{
		p_height[lineIndex] = lineHeight;
		p_imageY[lineIndex] = imageY;
		p_collapsed[lineIndex] = false;
		//
		imageY += lineHeight;
	}
	//ImageSize�X�V
	SPI_UpdateImageSize();
	//textview, linenumber view, diff view��\���X�V
	SPI_RefreshTextView();
}

/**
�w��s������folding�J�n�ʒu���擾
*/
AIndex	AView_Text::FindCurrentFoldingStartLineIndex( const AIndex inCurrentLineIndex ) const
{
	//�w��s��folding�J�n�ʒu�Ȃ�w��s��Ԃ�
	AFoldingLevel	currentLineFoldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(inCurrentLineIndex);
	if( (currentLineFoldingLevel&kFoldingLevel_Header) != 0 ||  (currentLineFoldingLevel&kFoldingLevel_BlockStart) != 0 )
	{
		return inCurrentLineIndex;
	}
	
	//���ݍs�̃C���f���g���x�����擾
	AItemCount	currentLineIndentCount = GetTextDocumentConst().SPI_GetTextInfoIndentCount(inCurrentLineIndex);
	//���ݍs�̃A�E�g���C�����x�����擾
	AIndex	currentLineOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inCurrentLineIndex);
	
	//�O�̍s�ȑO�ŁA�ŏ��Ƀ��x���������܂��͏������Ȃ�folding�J�n�ʒu���擾�i�������u���b�N�̏ꍇ�́A�����C���f���g���x���͏��O�j
	for( AIndex lineIndex = inCurrentLineIndex-1; lineIndex >= 0; lineIndex-- )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//���o��
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) <= currentLineOutlineLevel )
			{
				return lineIndex;
			}
		}
		//�u���b�N
		else if( (foldingLevel&kFoldingLevel_BlockStart) != 0 )
		{
			if( GetTextDocumentConst().SPI_GetTextInfoIndentCount(lineIndex) < currentLineIndentCount )
			{
				return lineIndex;
			}
		}
	}
	return kIndex_Invalid;
}

/**
�w��s���܂�folding��expand/collapse
@note inLineIndex�͐܂肽���݊J�n�s�E�I���s�ȊO�ł��ǂ�
*/
void	AView_Text::SPI_ExpandCollapseAtCurrentLine( const AIndex inLineIndex, const ABool inCollapse )
{
	//�w��s������folding�J�n�ʒu���擾
	AIndex	foldingStartLineIndex = FindCurrentFoldingStartLineIndex(inLineIndex);
	if( foldingStartLineIndex == kIndex_Invalid )
	{
		//folding�J�n�s�Ȃ�
		return;
	}
	//
	if( inCollapse == true )
	{
		SPI_ExpandCollapse(foldingStartLineIndex,kExpandCollapseType_Collapse,true);
	}
	else
	{
		SPI_ExpandCollapse(foldingStartLineIndex,kExpandCollapseType_Expand,true);
	}
}

/**
�w��s��folding level�Ɠ���folding��S��expand/collapse
@note inLineIndex�͐܂肽���݊J�n�s�E�I���s�ȊO�ł��ǂ�
*/
void	AView_Text::SPI_ExpandCollapseAllAtCurrentLevel( const AIndex inLineIndex, const ABool inCollapse, const ABool inByClickFoldingMark )
{
	//�w��s������folding�J�n�ʒu���擾
	AIndex	foldingStartLineIndex = FindCurrentFoldingStartLineIndex(inLineIndex);
	if( foldingStartLineIndex == kIndex_Invalid )
	{
		//folding�J�n�s�Ȃ�
		return;
	}
	if( inByClickFoldingMark == true )
	{
		//�s�ԍ��G���A�̐܂肽���݃}�[�N�N���b�N�ɂ��ꍇ�́A�R�����g���ΏۂƂ���
		if( (GetTextDocumentConst().SPI_GetFoldingLevel(inLineIndex)&kFoldingLevel_CommentStart) != 0 )
		{
			foldingStartLineIndex = inLineIndex;
		}
		//
		if( foldingStartLineIndex != inLineIndex )
		{
			//�s�ԍ��G���A�̐܂肽���݃}�[�N�N���b�N�ɂ��ꍇ�́A�N���b�N�s���J�n�s�ɂȂ�Ȃ�����A�ʏ��expand/collapse���s���B
			SPI_ExpandCollapse(inLineIndex,kExpandCollapseType_Switch,true);
			return;
		}
	}
	//
	AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(foldingStartLineIndex);
	if( (foldingLevel&kFoldingLevel_Header) != 0 )
	{
		//==================�������x���̌��o���܂肽����==================
		
		//
		AItemCount	outlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(foldingStartLineIndex);
		//�|�C���^�擾
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			if( (GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)&kFoldingLevel_Header) != 0 )
			{
				if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) == outlineLevel )
				{
					p_collapsed[lineIndex] = inCollapse;
				}
			}
		}
		//�S�Ă̍sImageY,�������Acollapsed�p�����[�^�ɏ]���čX�V
		UpdateLineImageInfoAllByCollapsedParameter();
	}
	else if( (foldingLevel&kFoldingLevel_BlockStart) != 0 )
	{
		//==================�����C���f���g���x���̃u���b�N�܂肽����==================
		
		//
		AItemCount	indentCount = GetTextDocumentConst().SPI_GetTextInfoIndentCount(foldingStartLineIndex);
		//�|�C���^�擾
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		/*���[�J���͈͓��ɂ��悤�Ǝv�������A�S�͈͂ɕύX�i������̂ق����g�����肪�ǂ��B����HTML�̏ꍇ�j
		AIndex	localStartLineIndex = 0, localEndLineIndex = 0;
		GetTextDocumentConst().SPI_GetLocalRangeForLineIndex(foldingStartLineIndex,localStartLineIndex,localEndLineIndex);
		//
		if( localStartLineIndex != kIndex_Invalid && localEndLineIndex != kIndex_Invalid )
		*/
		{
			//for( AIndex lineIndex = localStartLineIndex; lineIndex < localEndLineIndex; lineIndex++ )
			AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
			for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
			{
				if( (GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)&kFoldingLevel_BlockStart) != 0 )
				{
					if( GetTextDocumentConst().SPI_GetTextInfoIndentCount(lineIndex) == indentCount )
					{
						p_collapsed[lineIndex] = inCollapse;
					}
				}
			}
			//�S�Ă̍sImageY,�������Acollapsed�p�����[�^�ɏ]���čX�V
			UpdateLineImageInfoAllByCollapsedParameter();
		}
	}
	else if( (foldingLevel&kFoldingLevel_CommentStart) != 0 )
	{
		//==================���[�J���͈͓��̃R�����g��S�Đ܂肽����==================
		
		//�|�C���^�擾
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		AIndex	localStartLineIndex = 0, localEndLineIndex = 0;
		GetTextDocumentConst().SPI_GetLocalRangeForLineIndex(foldingStartLineIndex,localStartLineIndex,localEndLineIndex);
		//
		if( localStartLineIndex != kIndex_Invalid && localEndLineIndex != kIndex_Invalid )
		{
			//
			for( AIndex lineIndex = localStartLineIndex; lineIndex < localEndLineIndex; lineIndex++ )
			{
				if( (GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)&kFoldingLevel_CommentStart) != 0 )
				{
					p_collapsed[lineIndex] = inCollapse;
				}
			}
			//�S�Ă̍sImageY,�������Acollapsed�p�����[�^�ɏ]���čX�V
			UpdateLineImageInfoAllByCollapsedParameter();
		}
	}
}

/**
collapse���ꂽ�s�̃��X�g���擾
*/
void	AView_Text::SPI_GetFoldingCollapsedLines( ANumberArray& outCollapsedLines )
{
	//�|�C���^�擾
	AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
	ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
	//
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		if( p_collapsed[lineIndex] == true )
		{
			outCollapsedLines.Add(lineIndex);
		}
	}
}

/**
�S�Ă̍sImageY,�������Acollapsed�p�����[�^�ɏ]���čX�V
*/
void	AView_Text::UpdateLineImageInfoAllByCollapsedParameter()
{
	//�܂肽���ݓK�p�O�̊J�n�sindex���擾�i�܂肽���݌�A�J�n�s���ω����Ȃ��悤�ɃX�N���[�����s���邽�߁j
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	AIndex	oldStartLineIndex = GetLineIndexByImageY(imageFrameRect.top);
	
	//
	ANumber	lineHeight = GetLineHeightPlusMargin();
	ANumber	imageY = 0;
	//�|�C���^�擾
	AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
	ANumber*	p_height = arrayptr_height.GetPtr();
	AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
	ANumber*	p_imageY = arrayptr_imageY.GetPtr();
	AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
	ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
	//
	AItemCount	itemCount = mLineImageInfo_ImageY.GetItemCount();
	for( AIndex lineIndex = 0; lineIndex < itemCount; lineIndex++ )
	{
		//�����EImageY�ݒ�
		p_height[lineIndex] = lineHeight;
		p_imageY[lineIndex] = imageY;
		imageY += lineHeight;
		
		//
		if( p_collapsed[lineIndex] == true )
		{
			//�Ή�����܂肽���ݏI���s���擾
			AIndex	callapseEndLineIndex = SPI_FindFoldingEndLineIndex(lineIndex);
			
			//Collapse�}�[�N�̎��̍s����ACollapse�}�[�N�̍s�̃C���f���g�Ɠ����C���f���g�ʂ̍s�܂ŁA����0�ݒ肷��
			if( lineIndex+1 < itemCount && callapseEndLineIndex > lineIndex )
			{
				lineIndex++;
				for( ; lineIndex < callapseEndLineIndex; lineIndex++ )
				{
					//�����EImageY�ݒ�
					p_height[lineIndex] = 0;
					p_imageY[lineIndex] = imageY;
				}
				lineIndex--;
				/*
				//Collapse�}�[�N�̍s�̃C���f���g�Ɠ����C���f���g�ʂɂȂ����s�̍����ݒ�
				if( lineIndex < itemCount )
				{
					p_height[lineIndex] = lineHeight;
					p_imageY[lineIndex] = imageY;
					imageY += lineHeight;
				}
				else
				{
					break;
				}
				*/
			}
			else
			{
				//�܂肽���ݏI���s�𐳂����������Ȃ��ꍇ�́A
				//�܂肽���ݏ�ԂƂȂ��Ă��邱�Ƃ��ُ�Ȃ̂ŁA�܂肽���ݏ�Ԃ���������B
				p_collapsed[lineIndex] = false;
			}
		}
	}
	//ImageSize�X�V
	SPI_UpdateImageSize();
	//�܂肽���ݑO�̊J�n�s�ʒu�փX�N���[��
	AImagePoint	origin = {0};
	origin.x = imageFrameRect.left;
	origin.y = GetImageYByLineIndex(oldStartLineIndex);
	NVI_ScrollTo(origin,false);
	//�L�����b�gimage point�X�V
	UpdateCaretImagePoint();
	//textview, linenumber view, diff view��\���X�V
	SPI_RefreshTextView();
}

/**
�s�̃��X�g�ɏ]���čs��collapse
*/
void	AView_Text::SPI_CollapseLines( const ANumberArray& inCollapsedLines, const ABool inApplyDefaultFoldingSetting,
									 const ABool inExpandOtherLines )
{
	{
		//�|�C���^�擾
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		
		//==================inExpandOtherLines��true�Ȃ�܂��S�Ă̍s��expand==================
		if( inExpandOtherLines == true )
		{
			AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
			for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
			{
				p_collapsed[lineIndex] = false;
			}
		}
		
		//==================���X�g�Ɏw�肳�ꂽ�s��collapse==================
		//
		AItemCount	itemCount = inCollapsedLines.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AIndex	lineIndex = inCollapsedLines.Get(i);
			if( lineIndex >= 0 && lineIndex < GetTextDocumentConst().SPI_GetLineCount() )
			{
				p_collapsed[lineIndex] = true;
			}
		}
	}
	
	//==================�f�t�H���g�܂肽���݂�K�p==================
	//
	if( inApplyDefaultFoldingSetting == true && 
				GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseDefaultFoldingSetting) == true )
	{
		SetFoldingToDefaultCore(false);
	}
	
	//�S�Ă̍sImageY,�������Acollapsed�p�����[�^�ɏ]���čX�V
	UpdateLineImageInfoAllByCollapsedParameter();
	
	//==================�����L�����b�g�ʒu���܂肽���܂�Ă��܂�����A�L�����b�g�ʒu�݂̂͐܂肽���݉�������==================
	if( SPI_IsLineCollapsed(mCaretTextPoint.y) == true )
	{
		SPI_RevealCollapsedLine(mCaretTextPoint.y);
	}
}

/**
�܂肽���݂��f�t�H���g�ɂ���
*/
void	AView_Text::SPI_SetFoldingToDefault()
{
	//�f�t�H���g�܂肽���ݓK�p
	SetFoldingToDefaultCore(true);
	
	//�S�Ă̍sImageY,�������Acollapsed�p�����[�^�ɏ]���čX�V
	UpdateLineImageInfoAllByCollapsedParameter();
}

/**
���[�h�ݒ�ɂ��f�t�H���g�܂肽���ݓK�p
*/
void	AView_Text::SetFoldingToDefaultCore( const ABool inExpandForNotMatched )
{
	//�|�C���^�擾
	AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
	ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
	
	//�f�t�H���g�Ő܂肽���ލs�̊J�n�e�L�X�g�ݒ���擾
	AText	settingText;
	GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Text(AModePrefDB::kDefaultFoldingLineText,settingText);
	ATextArray	settingTextArray;
	settingTextArray.ExplodeFromText(settingText,kUChar_LineEnd);
	AItemCount	settingTextArrayItemCount = settingTextArray.GetItemCount();
	if( settingTextArrayItemCount > 0 )
	{
		//�X�y�[�X�E�^�u����菜��
		for( AIndex i = 0; i < settingTextArrayItemCount; i++ )
		{
			AText	t;
			settingTextArray.Get(i,t);
			t.RemoveSpaceTab();
			settingTextArray.Set(i,t);
		}
		//�ݒ�f�[�^�Ɉ�v����s������collapse����B
		AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			if( inExpandForNotMatched == true )
			{
				//��v���Ȃ��s�͐܂肽���݉���
				p_collapsed[lineIndex] = false;
			}
			//
			if( SPI_IsFoldingStart(GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)) == true )
			{
				AText	linetext;
				GetTextDocumentConst().SPI_GetLineText(lineIndex,linetext);
				linetext.RemoveSpaceTab();
				//�ݒ�f�[�^���ږ��̃��[�v
				for( AIndex i = 0; i < settingTextArrayItemCount; i++ )
				{
					//�ݒ�e�L�X�g�̒����ŁA�ݒ�e�L�X�g�ƍs�e�L�X�g���r����i�s�e�L�X�g�̒����̕����Z����΁A��r���Ȃ��Bout of range�ɂȂ�Ȃ��悤�ɂ��邽�߁A���A��r�͐�΂Ɉ�v���Ȃ��̂ŁB�j
					AItemCount	l = settingTextArray.GetTextLen(i);
					if( l > 0 && l <= linetext.GetItemCount() )
					{
						if( settingTextArray.Compare(i,linetext,0,l) == true )
						{
							//��v������܂肽����
							p_collapsed[lineIndex] = true;
							break;
						}
					}
				}
			}
		}
	}
}

/**
�w��s���܂肽���܂�Ă���ꍇ�ɁA�w��s���\�������悤�ɐ܂肽���݉�������i�������ʉӏ���\������ꍇ�Ȃǁj
*/
void	AView_Text::SPI_RevealCollapsedLine( const AIndex inLineIndex )
{
	//collapse���ꂽ�s�łȂ���Ή������Ȃ�
	if( mLineImageInfo_Height.Get(inLineIndex) > 0 )
	{
		return;
	}
	//�s�������̂ڂ��āAcollpase�t���OON�̍s����������B
	//�����W�b�N���P���� reveal�������s�̑O�ɁA�Ɨ������u���b�N�������Ă��ꂪcollapse����Ă���A���A����ɑS�̂�collapse�̏ꍇ
	for( AIndex lineIndex = inLineIndex-1; lineIndex >= 0; lineIndex-- )
	{
		if( mLineImageInfo_Collapsed.Get(lineIndex) == true )
		{
			if( mLineImageInfo_Height.Get(lineIndex) == 0 )
			{
				//collapse�t���OON�A���A���̍s��collapsed�Ȃ�A���̍s��collpase�t���O��OFF�ɂ��āA����ɂ����̂ڂ�
				mLineImageInfo_Collapsed.Set(lineIndex,false);
			}
			else
			{
				//collapse�t���OON�A���A���̍s��collapsed�łȂ���΁A���̍s����expand�����s����B
				SPI_ExpandCollapse(lineIndex,kExpandCollapseType_Expand,true);
				break;
			}
		}
	}
}

/**
�s���܂肽���܂�Ă��邩�ǂ������擾
*/
ABool	AView_Text::SPI_IsLineCollapsed( const AIndex inLineIndex ) const
{
	return (mLineImageInfo_Height.Get(inLineIndex)==0);
}

/**
�e���o���̍s���擾
*/
AIndex	AView_Text::FindParentHeader( const AIndex inLineIndex ) const
{
	//���ݍs�̃A�E�g���C�����x�����擾
	AIndex	currentLineOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inLineIndex);
	
	for( AIndex lineIndex = inLineIndex-1; lineIndex >= 0; lineIndex-- )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//���o��
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) < currentLineOutlineLevel )
			{
				return lineIndex;
			}
		}
	}
	return kIndex_Invalid;
}

/**
���̌��o���s���擾
*/
AIndex	AView_Text::FindNextHeader( const AIndex inLineIndex ) const
{
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	for( AIndex lineIndex = inLineIndex+1; lineIndex < lineCount; lineIndex++ )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//���o��
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			return lineIndex;
		}
	}
	return kIndex_Invalid;
}

/**
�O�̌��o���s���擾
*/
AIndex	AView_Text::FindPrevHeader( const AIndex inLineIndex ) const
{
	for( AIndex lineIndex = inLineIndex-1; lineIndex >= 0; lineIndex-- )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//���o��
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			return lineIndex;
		}
	}
	return kIndex_Invalid;
}

/**
�������x���̎��̌��o�����擾
*/
AIndex	AView_Text::FindSameOrHigherLevelNextHeader( const AIndex inLineIndex ) const
{
	//���ݍs�̃A�E�g���C�����x�����擾
	AIndex	currentLineOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inLineIndex);
	
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	for( AIndex lineIndex = inLineIndex+1; lineIndex < lineCount; lineIndex++ )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//���o��
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) <= currentLineOutlineLevel )
			{
				return lineIndex;
			}
		}
	}
	return kIndex_Invalid;
}

/**
�������x���̑O�̌��o�����擾
*/
AIndex	AView_Text::FindSameOrHigherLevelPrevHeader( const AIndex inLineIndex ) const
{
	//���ݍs�̃A�E�g���C�����x�����擾
	AIndex	currentLineOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inLineIndex);
	
	for( AIndex lineIndex = inLineIndex-1; lineIndex >= 0; lineIndex-- )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//���o��
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) <= currentLineOutlineLevel )
			{
				return lineIndex;
			}
		}
	}
	return kIndex_Invalid;
}

#pragma mark ===========================

#pragma mark ---�}�`���[�h
//#478

/**
*/
void	AView_Text::FigMode_DrawLine( const AImagePoint& inStartImagePoint, const AImagePoint& inEndImagePoint )
{
	AText	space(" ");
	ANumber	spacewidth = static_cast<ANumber>(NVMC_GetDrawTextWidth(space));
	FigMode_DrawLine(inStartImagePoint.x/spacewidth,inEndImagePoint.x/spacewidth,GetLineIndexByImageY(inStartImagePoint.y));
}

/**
*/
void	AView_Text::FigMode_DrawLine( const ANumber inStartTextX, const ANumber inEndTextX, const ANumber inTextY )
{
	CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	AAnalyzeDrawData	analyzeDrawData;
	ATextPoint	caretPoint, selectPoint;
	GetTextDocument().SPI_GetTextDrawDataWithStyle(inTextY,textDrawData,false,caretPoint,selectPoint,analyzeDrawData,GetObjectID());
	
	//
	AIndex	lineLength = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(inTextY);
	//
	ANumber	spaceInsert = 0;
	ANumber	startOffset = inStartTextX;
	ANumber	deleteEnd = inEndTextX;
	if( inEndTextX <= textDrawData.UTF16DrawText.GetItemCount() )
	{
		//
		startOffset = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(inStartTextX);
		deleteEnd = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(inEndTextX);
	}
	else if( inStartTextX <= textDrawData.UTF16DrawText.GetItemCount() )
	{
		//
		startOffset = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(inStartTextX);
		deleteEnd = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(textDrawData.UTF16DrawText.GetItemCount()-1);
	}
	else
	{
		//
		spaceInsert = inStartTextX-lineLength;
		//
		startOffset = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(textDrawData.UTF16DrawText.GetItemCount()-1) + spaceInsert;
		deleteEnd = startOffset;
	}
	
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Typing);
	//�폜
	ATextPoint	deletespt;
	deletespt.x = startOffset;
	deletespt.y = inTextY;
	ATextPoint	deleteept;
	deleteept.x = deleteEnd;
	deleteept.y = inTextY;
	if( startOffset < deleteEnd )
	{
		GetTextDocument().SPI_DeleteText(deletespt,deleteept);
	}
	//
	ATextPoint	pt = deletespt;
	//�X�y�[�X�}��
	AText	spacetext;
	for( AIndex i = 0; i < spaceInsert; i++ )
	{
		spacetext.Add(kUChar_Space);
	}
	GetTextDocument().SPI_InsertText(pt,spacetext);
	pt.x += spacetext.GetItemCount();
	//���}��
	AText	linetext;
	for( AIndex i = inStartTextX; i < inEndTextX; i++ )
	{
		linetext.Add('-');
	}
	GetTextDocument().SPI_InsertText(pt,linetext);
}

#pragma mark ===========================

#pragma mark ---Notification�E�C���h�E
//#725

/**
Notification�|�b�v�A�b�v�E�C���h�E��\���^��\��
*/
void	AView_Text::SPI_ShowNotificationPopupWindow( const ABool inShow )
{
	if( inShow == true )
	{
		//notification �|�b�v�A�b�v�E�C���h�E��bounds�X�V
		UpdateNotificationPopupWindowBounds(true);
		
		//�\��
		SPI_GetPopupNotificationWindow().NVI_Show(false);
	}
	else
	{
		//��\��
		SPI_GetPopupNotificationWindow().NVI_Hide();
	}
}

/**
Notification�E�C���h�E�擾
*/
AWindow_Notification&	AView_Text::SPI_GetPopupNotificationWindow()
{
	//�������Ȃ琶��
	if( mPopupNotificationWindowID == kObjectID_Invalid )
	{
		AWindowDefaultFactory<AWindow_Notification>	factory;
		mPopupNotificationWindowID = GetApp().NVI_CreateWindow(factory);
		GetApp().NVI_GetWindowByID(mPopupNotificationWindowID).NVI_ChangeToOverlay(NVM_GetWindow().GetObjectID(),true);
		SPI_GetPopupNotificationWindow().NVI_Create(kObjectID_Invalid);
	}
	//Notification�E�C���h�E�擾
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Notification,kWindowType_Notification,mPopupNotificationWindowID);
}

/**
notification�E�C���h�E��bounds�X�V
*/
void	AView_Text::UpdateNotificationPopupWindowBounds( const ABool inAlwaysUpdate )
{
	//text view�S�̂�global rect�擾
	AGlobalRect	viewGlobalRect = {0};
	NVI_GetGlobalViewRect(viewGlobalRect);
	
	if( inAlwaysUpdate == false )
	{
		//notification�\�����łȂ���΁A�������Ȃ�
		if( mPopupNotificationWindowID == kObjectID_Invalid )
		{
			return;
		}
		if( SPI_GetPopupNotificationWindow().NVI_IsWindowVisible() == false )
		{
			return;
		}
	}
	
	//==================Notification�E�C���h�E�z�u==================
	
	const ANumber	kNotificationWindowRightMargin = 2;
	const ANumber	kNotificationWindowTopMargin = 3;
	
	ARect	windowRect = {0};
	if( NVI_GetVerticalMode() == false )
	{
		windowRect.right	= viewGlobalRect.right - kNotificationWindowRightMargin;
		windowRect.left		= windowRect.right - 320;
		windowRect.top		= viewGlobalRect.top + kNotificationWindowTopMargin;
		windowRect.bottom	= windowRect.top +
							SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_GetWindowHeight();
	}
	else
	{
		//�c�������[�h���͍s�ԍ��G���A�̉E��ɕ\������ #1387
		GetLineNumberView().NVI_GetGlobalViewRect(viewGlobalRect);
		
		windowRect.right	= viewGlobalRect.right - kNotificationWindowRightMargin;
		windowRect.left		= windowRect.right - 320;
		windowRect.top		= viewGlobalRect.top + kNotificationWindowTopMargin;
		windowRect.bottom	= windowRect.top +
							SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_GetWindowHeight();
	}
	
	SPI_GetPopupNotificationWindow().NVI_SetWindowBounds(windowRect);
}

//#1117
/**
�ėp�|�b�v�A�b�v�ʒm�\��
*/
void	AView_Text::SPI_ShowGeneralNotificationPopupWindow( const AText& inTitle, const AText& inMessage )
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_General(inTitle,inMessage,GetEmptyText());
		SPI_ShowNotificationPopupWindow(true);
	}
}
