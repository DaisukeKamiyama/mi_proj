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

AView_EditBox

*/

#include "stdafx.h"

#include "../Frame.h"

AText			AView_EditBox::sDefaultFontName;//AApplication::AApplication()�ɂ�SPI_SetDefaultFont()�o�R�Őݒ肳���
AFloatNumber	AView_EditBox::sDefaultFontSize = 12.0;

#pragma mark ===========================
#pragma mark [�N���X]AView_EditBox
#pragma mark ===========================
//�C���f�b�N�X�E�C���h�E�̃��C��View

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AView_EditBox::AView_EditBox( const AWindowID inWindowID, const AControlID inID ) //#182
: AView(inWindowID,inID), mCaretMode(true), mCaretBlinkStateVisible(false), mFontSize(12.0), mColor(kColor_Black), 
		mOldArrowImageX(0), mListViewID(kControlID_Invalid), mInlineInputNotFixedTextLen(0),
		mMouseTrackingMode(kMouseTrackingMode_None)//#688 , mMouseTrackByLoop(true)
		,mBackgroundColor(kColor_White),mFrameColor(kColor_Gray),mColorDeactive(kColor_Gray70Percent),mFrameColorDeactive(kColor_Gray50Percent)
		/*#135 ,mFrameViewID(kObjectID_Invalid)*/, mEnableEdit(true)
		,mAdjustScrollXOffsetRight(30), mAdjustScrollXOffsetLeft(50)//#261
		,mDragging(false), mDragDrawn(false), mDragWordDragDrop(false)//#135
		,mContextMenuID(kContextMenuID_EditBox)//#135
		,mEnableBackgroundColorForEmptyState(false), mBackgroundActiveColorForEmptyState(kColor_White), mBackgroundDeactiveColorForEmptyState(kColor_White)//#798
,mTransparency(1.0), mEditBoxType(kEditBoxType_Normal)
, mHoveringSearchCancelButton(false), mPressingSearchCancelButton(false), mTrackingPressSearchCancelButton(false)
,mDrawLineEnd(true), mCenterizeWhen1Line(false)//#894
,mReadOnly(false)//#688
,mSetSelect00AfterSetText(false)
,mTempCaretHideCount(0)
{
	NVMC_SetOffscreenMode(true);//win
	//#92 ��������NVIDO_Init()�ֈړ�
}

/**
�������iView�쐬����ɕK���R�[�������j
*/
void	AView_EditBox::NVIDO_Init()
{
	//
	AEditBoxDocumentFactory	factory(this);
	mDocumentID = AApplication::GetApplication().NVI_CreateDocument(factory);
	GetTextDocument().NVI_RegisterView(GetObjectID());//#379 ��{���[���Ƃ���Document�ɂ�View��o�^�B�o�^������Document�폜�B
	
	//���s�R�[�h�e�L�X�g�擾
	mLineEndDisplayText.SetLocalizedText("ReturnCodeText");
	mLineEndDisplayText_UTF16.SetText(mLineEndDisplayText);
	mLineEndDisplayText_UTF16.ConvertFromUTF8ToUTF16();
	
	//
	//win AFontWrapper::GetDefaultFontName(mFontName);
	ATextPoint	pt = {0,0};
	SetCaretTextPoint(pt);
	NVI_SetDefaultCursor(kCursorType_IBeam);
	mInlineInputHiliteColorArray.Add(kColor_Black);
	mInlineInputHiliteColorArray.Add(kColor_LightGray);
	mInlineInputHiliteColorArray.Add(kColor_Gray);
	mInlineInputHiliteColorArray.Add(kColor_Black);
	mInlineInputHiliteColorArray.Add(kColor_Black);
	mInlineInputHiliteColorArray.Add(kColor_Black);
	
	//#688
	//�t�H�[�J�X��
	NVMC_SetAcceptFocus(true);
	
	/*#688
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
	//#135 CreateFrameView();
	//#137 �f�t�H���gTextDrawProperty�ݒ�
	/*win
	AText	defaultfontname;
	AFontWrapper::GetDefaultFontName(defaultfontname);
	*/
	//win SPI_SetTextDrawProperty(windefaultfontname,14.0,kColor_Black,kColor_Gray70Percent);//#182 kColor_Gray20Percent,kColor_Gray20Percent);
	SPI_SetTextDrawProperty(sDefaultFontName,sDefaultFontSize,kColor_Black,kColor_Gray70Percent);//win
	//#135
	NVI_SetAutomaticSelectBySwitchFocus(true);
	//#137 �f�t�H���g�ł�Return/Tab��EditBox�ł͏������Ȃ�
	NVI_SetCatchReturn(false);
	NVI_SetCatchTab(false);
	//#135
	SPI_UpdateImageSize();
	//Drag&Drop�Ή� #135 
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_UnicodeText);
	NVMC_EnableDrop(scrapTypeArray);
	//#725
	NVMC_EnableMouseLeaveEvent();
}

//�f�X�g���N�^
AView_EditBox::~AView_EditBox()
{
	/*#92 NVIDO_DoDeleted()�ֈړ�
	//#138
	DeleteFrameView();
	//
	if( mListViewID != kControlID_Invalid )
	{
		AText	text;
		GetTextDocumentConst().SPI_GetText(text);
		NVM_GetWindow().NVI_GetListView(mListViewID).SPI_DoEditBoxDeleted(text);
	}
	AApplication::GetApplication().NVI_DeleteDocument(mDocumentID);
	*/
}

//#92
/**
�폜�������i�폜���ɕK���R�[�������j
�f�X�g���N�^��GarbageCollection���ɃR�[�������̂ŁA��{�I�ɂ͂�����ō폜�������s������
*/
void	AView_EditBox::NVIDO_DoDeleted()
{
	//#138
	//#135 DeleteFrameView();
	//
	if( mListViewID != kControlID_Invalid )
	{
		AText	text;
		GetTextDocumentConst().SPI_GetText(text);
		NVM_GetWindow().NVI_GetListView(mListViewID).SPI_DoEditBoxDeleted(text);
	}
	//#379 AApplication::GetApplication().NVI_DeleteDocument(mDocumentID);
	GetTextDocument().NVI_UnregisterView(GetObjectID());//#379 ��{���[���Ƃ���Document�ɂ�View��o�^�B�o�^������Document�폜�B
}

#pragma mark ===========================

#pragma mark <�֘A�I�u�W�F�N�g�擾>

#pragma mark ===========================

//
ADocument_EditBox&	AView_EditBox::GetTextDocument()
{
	MACRO_RETURN_DOCUMENT_DYNAMIC_CAST(ADocument_EditBox,kDocumentType_EditBox,mDocumentID);
	/*#199
	if( AApplication::GetApplication().NVI_GetDocumentTypeByID(mDocumentID) != kDocumentType_EditBox )
	{
		_ACThrow("document type different",this);
	}
	return dynamic_cast<ADocument_EditBox&>(AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID));
	*/
}
//
const ADocument_EditBox&	AView_EditBox::GetTextDocumentConst() const
{
	MACRO_RETURN_CONSTDOCUMENT_DYNAMIC_CAST(ADocument_EditBox,kDocumentType_EditBox,mDocumentID);
	/*#199
	if( AApplication::GetApplication().NVI_GetDocumentTypeByID(mDocumentID) != kDocumentType_EditBox )
	{
		_ACThrow("document type different",this);
	}
	return dynamic_cast<const ADocument_EditBox&>(AApplication::GetApplication().NVI_GetDocumentConstByID(mDocumentID));
	*/
}

/*#135
//#199
AView_Frame&	AView_EditBox::SPI_GetFrameView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_Frame,kViewType_Frame,mFrameViewID);
}
*/

#pragma mark ===========================

#pragma mark <�C�x���g����>

#pragma mark ===========================

//���j���[�I��������
ABool	AView_EditBox::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	ABool	touch = false;//#135
	switch(inMenuItemID)
	{
		//Undo
	  case kMenuItemID_Undo:
		{
			UndoRedo(true);
			touch = true;//#135
			break;
		}
		//Redo
	  case kMenuItemID_Redo:
		{
			UndoRedo(false);
			touch = true;//#135
			break;
		}
		//�R�s�[
	  case kMenuItemID_Copy:
	  //case kMenuItemID_CM_Copy:
		{
			Copy(false);
			break;
		}
		//�J�b�g
	  case kMenuItemID_Cut:
	  //case kMenuItemID_CM_Cut:
		{
			Cut(false);
			touch = true;//#135
			break;
		}
		//�y�[�X�g
	  case kMenuItemID_Paste:
	  //case kMenuItemID_CM_Paste:
		{
			Paste();
			touch = true;//#135
			break;
		}
		//�폜
	  case kMenuItemID_Clear:
	  //case kMenuItemID_CM_Clear:
		{
			DeleteSelect(kEditBoxUndoTag_DELKey);
			touch = true;//#135
			break;
		}
		//�S�Ă�I��
	  case kMenuItemID_SelectAll:
	  //case kMenuItemID_CM_SelectAll:
		{
			NVI_SetSelectAll();
			break;
		}
		//�o�b�N�X���b�V������ #137
	  case kMenuItemID_InputBackslashYen:
		{
			if( NVI_GetInputBackslashByYenKeyMode() == true )
			{
				AText	yen;
				yen.SetLocalizedText("Text_Yen");
				TextInput(kEditBoxUndoTag_Typing,yen);
			}
			else
			{
				AText	backslash;
				backslash.Add(kUChar_Backslash);
				TextInput(kEditBoxUndoTag_Typing,backslash);
			}
			touch = true;//#135
			break;
		}
	  default:
		{
			result = false;
			break;
		}
	}
	//#135
	if( touch == true )
	{
		NVM_GetWindow().EVT_TextInputted(NVI_GetControlID());
	}
	//�w�i�F�X�V
	UpdateBackgroundColor();
	//�C���[�W�T�C�Y�X�V
	SPI_UpdateImageSize();
	return result;
}

//
void	AView_EditBox::EVTDO_UpdateMenu()
{
	ABool	b;
	AText	text;
	
	//Undo
	b = GetTextDocument().SPI_GetUndoText(text);
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Undo,b);
	AApplication::GetApplication().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_Undo,text);
	//Redo
	b = GetTextDocument().SPI_GetRedoText(text);
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Redo,b);
	AApplication::GetApplication().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_Redo,text);
	//�R�s�[
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Copy,(IsCaretMode()==false));
	//�J�b�g
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Cut,((IsCaretMode()==false)&&(GetTextDocument().NVI_IsReadOnly()==false)));
	//�y�[�X�g
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Paste,
			((GetTextDocument().NVI_IsReadOnly()==false)&&(AScrapWrapper::ExistClipboardTextScrapData()==true)));
	//�폜
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Clear,((IsCaretMode()==false)&&(GetTextDocument().NVI_IsReadOnly()==false)));
	//�S�Ă�I��
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SelectAll,true);
	//�o�b�N�X���b�V������
	{
		AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_InputBackslashYen,true);
		AText	text;
		if( NVI_GetInputBackslashByYenKeyMode() == true )
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
		AApplication::GetApplication().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_InputBackslashYen,text);
	}
}


//�`��v�����̃R�[���o�b�N(AView�p)
void	AView_EditBox::EVTDO_DoDraw()
{
	ADocument_EditBox&	document = GetTextDocument();
	
	//�w�i�`��
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	if( NVI_GetFrameViewControlID() == kControlID_Invalid )
	{
		NVMC_PaintRect(localFrameRect,mBackgroundColor,mTransparency);
	}
	
	//�����F�擾
	AColor	color = mColor;
	//
	if( NVMC_IsControlEnabled() == false )
	{
		color = kColor_Gray;
	}
	
	/*
	if( NVM_GetWindow().NVI_IsWindowActive() == false )
	{
		color = mColorDeactive;
	}
	if( NVI_GetEnabled() == false )//#135
	{
		color = mColorDeactive;
	}
	*/
	
	//#798
	//�e�L�X�g����̏ꍇ�̔w�i�`��
	if( GetTextDocumentConst().SPI_GetTextLength() == 0 && mEnableBackgroundColorForEmptyState == true )
	{
		AColor	backgroundColor = mBackgroundActiveColorForEmptyState;
		if( NVM_GetWindow().NVI_IsWindowActive() == false )
		{
			backgroundColor = mBackgroundDeactiveColorForEmptyState;
		}
		NVMC_PaintRect(localFrameRect,backgroundColor,mTransparency);
	}
	//�e�L�X�g����̏ꍇ�̃e�L�X�g�`��
	if( GetTextDocumentConst().SPI_GetTextLength() == 0 && mTextForEmptyState.GetItemCount() > 0 )
	{
		//�e�L�X�g����̏ꍇ�̃e�L�X�g�`��
		NVMC_DrawText(localFrameRect,mTextForEmptyState,kColor_Gray60Percent,kTextStyle_Normal);
		//�V���[�g�J�b�g�����\��
		ALocalRect	r = localFrameRect;
		r.left = localFrameRect.right - NVMC_GetDrawTextWidth(mTextForEmptyState_Shortcut) - 3;
		NVMC_DrawText(r,mTextForEmptyState_Shortcut,kColor_Gray60Percent,kTextStyle_Normal);
	}
	
	//�����L�����Z���{�^���\��
	if( IsSearchCancelButtonDisplayed() == true )
	{
		ALocalRect	rect = {0};
		GetSearchCancelButtonRect(rect);
		ALocalPoint	pt = {rect.left,rect.top};
		AImageID	buttonImageID = kImageID_iconSwCancel;
		if( mPressingSearchCancelButton == true )
		{
			buttonImageID = kImageID_iconSwCancel_p;
		}
		else if( mHoveringSearchCancelButton == true )
		{
			buttonImageID = kImageID_iconSwCancel_h;
		}
		NVMC_DrawImage(buttonImageID,pt);
	}
	
	//1�s���Ƃɕ`��
	AItemCount	lineCount = GetTextDocument().SPI_GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		AImageRect	lineImageRect;
		GetLineImageRect(lineIndex,lineImageRect);
		ALocalRect	lineLocalRect;
		NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
		if( NVMC_IsRectInDrawUpdateRegion(lineLocalRect) == false )   continue;
		
		//
		//#455 NVMC_SetDefaultTextProperty(mFontName,mFontSize,color,kTextStyle_Normal,true);
		//#455 
		ATextStyle	style = kTextStyle_Normal;
		if( mLineStyleArray_Style.GetItemCount() > lineIndex )
		{
			style = mLineStyleArray_Style.Get(lineIndex);
			color = mLineStyleArray_Color.Get(lineIndex);
		}
		NVMC_SetDefaultTextProperty(mFontName,mFontSize,color,style,true);
		//
		CTextDrawData	textDrawData(false);
		GetTextDocument().SPI_GetTextDrawData(lineIndex,textDrawData);
		
		//�I��F�擾
		AColor	selectionColor;
		if( NVI_IsFocusActive() == true )
		{
			AColorWrapper::GetHighlightColor(selectionColor);
		}
		else
		{
			selectionColor = kColor_Gray90Percent;
		}
		textDrawData.selectionColor = selectionColor;
		//�I��\��
		textDrawData.drawSelection = false;
		if( mCaretMode == false )
		{
			//�ق���View�ł̃e�L�X�g�ҏW�𔽉f������ꍇ���A�ǂ����Ă��I��͈͂��C���[�K���ɂȂ邱�Ƃ����肤��̂ŁA
			//�C���[�K���̏ꍇ�͑I��͈͕\���͂����Ȃ��悤�ɂ���B
			//if( IsTextPointValid(inSelectTextPoint1) == true && IsTextPointValid(inSelectTextPoint2) == true )
			{
				ATextPoint	spt, ept;
				OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),spt,ept);
				if( spt.y == lineIndex && ept.y == lineIndex )
				{
					textDrawData.drawSelection = true;
					textDrawData.selectionStart = textDrawData.OriginalTextArray_UnicodeOffset.Get(spt.x);
					textDrawData.selectionEnd = textDrawData.OriginalTextArray_UnicodeOffset.Get(ept.x);
				}
				else if( spt.y < lineIndex && ept.y == lineIndex )
				{
					textDrawData.drawSelection = true;
					textDrawData.selectionStart = kIndex_Invalid;
					textDrawData.selectionEnd = textDrawData.OriginalTextArray_UnicodeOffset.Get(ept.x);
				}
				else if( spt.y < lineIndex && ept.y > lineIndex )
				{
					textDrawData.drawSelection = true;
					textDrawData.selectionStart = textDrawData.OriginalTextArray_UnicodeOffset.Get(0);
					textDrawData.selectionEnd = kIndex_Invalid;
				}
				else if( spt.y == lineIndex && ept.y > lineIndex )
				{
					textDrawData.drawSelection = true;
					textDrawData.selectionStart = textDrawData.OriginalTextArray_UnicodeOffset.Get(spt.x);
					textDrawData.selectionEnd = kIndex_Invalid;
				}
			}
		}
		
		//���s�R�[�h�e�L�X�g�ǉ�
		if( mDrawLineEnd == true && document.SPI_ExistLineEnd(lineIndex) == true )
		{
			textDrawData.AddText(mLineEndDisplayText,mLineEndDisplayText_UTF16,kColor_Gray);
		}
		
		//�e�L�X�g�`��
		AImageRect	imageDrawRect;
		GetLineImageRect(lineIndex,imageDrawRect);
		ALocalRect	localDrawRect = localFrameRect;
		NVM_GetLocalRectFromImageRect(imageDrawRect,localDrawRect);
		/*
		ANumber	yoffset = 0;
		if( lineCount == 1 )
		{
			yoffset = ((localFrameRect.bottom - localFrameRect.top ) - (localDrawRect.bottom - localDrawRect.top))/2;
			localDrawRect.top += yoffset;
			localDrawRect.bottom += yoffset;
		}
		*/
		//
		NVMC_DrawText(localDrawRect,textDrawData);
		
		//InlineInput �n�C���C�g�����\��
		for( AIndex index = 0; index < mHiliteLineStyleIndex.GetItemCount(); index++ )
		{
			AIndex	hiliteLineStyleIndex = mHiliteLineStyleIndex.Get(index);
			if( hiliteLineStyleIndex == kIndex_Invalid )   continue;
			AColor	color = mInlineInputHiliteColorArray.Get(hiliteLineStyleIndex);
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
				lineLocalStart.y += GetLineHeightWithoutMargin()-1;//win 080618
				lineLocalEnd.y += GetLineHeightWithoutMargin()-1;//win 080618
				lineLocalEnd.x -= 2;//#185
				if( lineLocalEnd.x < lineLocalStart.x )   lineLocalEnd.x = lineLocalStart.x;//#185
				NVMC_DrawLine(lineLocalStart,lineLocalEnd,color);
			}
		}
	}
}

/**
�}�E�X�{�^����������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�{�^�������̏ꏊ�i�R���g���[�����[�J�����W�j
@param inModifierKeys ModifierKeys�̏��
@param inClickCout �N���b�N��
*/
ABool	AView_EditBox::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//�t���[�����N���b�N���́A(-1,-1)���Z�b�g����Ă����ɗ���̂ŁA�S�̑I�����ă��^�[���B
	if( inLocalPoint.x == -1 && inLocalPoint.y == -1 )
	{
		NVI_SetSelectAll();
		return true;
	}
	
	//�ҏW�s�Ȃ牽�����Ȃ��ŏI��
	//#455 if( mEnableEdit == false )   return false;
	
	//��View��Active�łȂ��ꍇ�́AWindow�փt�H�[�J�X�̈ړ����˗�����
	if( NVI_IsFocusActive() == false && NVMC_IsControlEnabled() == true )
	{
		ABool	svSelectAllAutomatic = NVI_GetAutomaticSelectBySwitchFocus();
		NVI_SetAutomaticSelectBySwitchFocus(false);
		//�t�H�[�J�X�ݒ�
		NVM_GetWindow().NVI_SwitchFocusTo(NVI_GetControlID());
		//
		NVI_SetAutomaticSelectBySwitchFocus(svSelectAllAutomatic);
	}
	
	if( inClickCount == 1 )
	{
		DoMouseDownSingleClick(inLocalPoint,inModifierKeys);
	}
	else if( inClickCount == 2 )
	{
		DoMouseDownDoubleClick(inLocalPoint,inModifierKeys);
	}
	else if( inClickCount == 3 )
	{
		DoMouseDownTripleClick(inLocalPoint,inModifierKeys);
	}
	return false;
}

/**
�}�E�X��View�O�ֈړ���������OS�C�x���g�R�[���o�b�N

@param inLocalPoint �}�E�X�J�[�\���̏ꏊ�i�R���g���[�����[�J�����W�j
*/
void	AView_EditBox::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	if( mHoveringSearchCancelButton == true )
	{
		mHoveringSearchCancelButton = false;
		NVI_Refresh();
	}
}

//�}�E�X�N���b�N�i�V���O���N���b�N�j
void	AView_EditBox::DoMouseDownSingleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	
	//#725
	//�����L�����Z���{�^��mouse down�������imouse up���ɃN���b�N���������s�j
	if( MouseIsInSearchCancelButtonRect(inLocalPoint) == true )
	{
		mMouseTrackingMode = kMouseTrackingMode_SingleClick;
		mTrackingPressSearchCancelButton = true;
		mPressingSearchCancelButton = true;
		NVI_Refresh();
		NVM_SetMouseTrackingMode(true);
		return;
	}
	
	//�I��͈͓��N���b�N�iDrag�܂��̓L�����b�g�ݒ�j
	if( IsInSelect(clickImagePoint,true) == true )
	{
		//Windows�̏ꍇ�́A�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
		//#688 if( mMouseTrackByLoop == false )
		//#688 {
		//TrackingMode�ݒ�
		mMouseTrackingMode = kMouseTrackingMode_SingleClickInSelect;
		mMouseTrackStartImagePoint = clickImagePoint;
		mMouseTrackResultIsDrag = false;
		NVM_SetMouseTrackingMode(true);
		return;
		//#688 }
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
		//#688 SetCaretTextPoint()����ShowCaretReserve()�����s���Ă���̂ŕs�v ShowCaret();
	}
	
	//�}�E�X�h���b�O�ɂ��I��
	
	//Windows�̏ꍇ�́A�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
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
	//ATickCount	firsttickcount = ATimerWrapper::GetTickCount();
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
	*/
}

//Mouse Track
void	AView_EditBox::DoMouseTrack_SingleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//�\���t���[����Image���W�Ŏ擾
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(Y����)
		if( mouseImagePoint.y < frameImageRect.top )
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + DragScrollV(mouseImagePoint.y-frameImageRect.top,inModifierKeys);
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > frameImageRect.bottom )
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + DragScrollV(mouseImagePoint.y-frameImageRect.bottom,inModifierKeys);
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
			//#400 ���ݍs�̒[�܂ł������E�X�N���[���o���Ȃ��悤�ɂ���B
			//���ݍs�̕\�������擾
			ATextPoint	pt;
			GetTextPointFromImagePoint(inMouseImagePoint,pt);
			pt.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(pt.y);
			AImagePoint	ipt;
			GetImagePointFromTextPoint(pt,ipt);
			ANumber	imageWidth = ipt.x;
			//
			if( frameImageRect.right < /*#400 NVM_GetImageWidth()*/imageWidth )
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.right + NVI_GetHScrollBarUnit() -1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.right;
			}
		}
		
		ATextPoint	selectTextPoint;
		GetTextPointFromImagePoint(mouseImagePoint,selectTextPoint);
		SetSelectTextPoint(selectTextPoint);
	}
}

//
void	AView_EditBox::DoMouseTrackEnd_SingleClickInSelect( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	ALocalPoint	localPoint;
	NVM_GetLocalPointFromImagePoint(inMouseImagePoint,localPoint);
	ATextPoint	textPoint;
	GetTextPointFromImagePoint(inMouseImagePoint,textPoint);
	//#135 Drag&Drop�Ή�
	if( mMouseTrackResultIsDrag == true )
	{
		//�}�E�X�{�^���������Ƀ}�E�X�𓮂������ꍇ
		//Drag�̈�ݒ�
		AArray<ALocalRect>	dragRect;
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
			imageRect.bottom	= imageEndPoint.y + GetLineHeightWithMargin();
			if( y != ept.y )
			{
				imageRect.right = viewImageRect.right;
			}
			ALocalRect	localRect;
			NVM_GetLocalRectFromImageRect(imageRect,localRect);
			dragRect.Add(localRect);
			sx = 0;
		}
		//Scrap�ݒ�
		AArray<AScrapType>	scrapType;
		ATextArray	data;
		//�I�𒆂̃e�L�X�g�擾
		AText	text;
		GetSelectedText(text);
		//#688
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
		legacyText.ConvertFromUTF8(ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName));//win
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
		//#688 SetCaretTextPoint()����ShowCaretReserve()�����s���Ă���̂ŕs�v ShowCaret();
	}
}

//�}�E�X�N���b�N�i�_�u���N���b�N�j
void	AView_EditBox::DoMouseDownDoubleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
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
	//
	SetSelect(mFirstSelectStart,mFirstSelectEnd);
	
	//�}�E�X�h���b�O�ɂ��I��
	
	//Windows�̏ꍇ�́A�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_DoubleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
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
void	AView_EditBox::DoMouseTrack_DoubleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//�\���t���[����Image���W�Ŏ擾
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(Y����)
		if( mouseImagePoint.y < frameImageRect.top )
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + DragScrollV(mouseImagePoint.y-frameImageRect.top,inModifierKeys);
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > frameImageRect.bottom )
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + DragScrollV(mouseImagePoint.y-frameImageRect.bottom,inModifierKeys);
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
			//#400 ���ݍs�̒[�܂ł������E�X�N���[���o���Ȃ��悤�ɂ���B
			//���ݍs�̕\�������擾
			ATextPoint	pt;
			GetTextPointFromImagePoint(inMouseImagePoint,pt);
			pt.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(pt.y);
			AImagePoint	ipt;
			GetImagePointFromTextPoint(pt,ipt);
			ANumber	imageWidth = ipt.x;
			//
			if( frameImageRect.right < /*#400 NVM_GetImageWidth()*/imageWidth )
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
void	AView_EditBox::DoMouseDownTripleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
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
	SetSelect(mFirstSelectStart,mFirstSelectEnd);
	mClickTextLineIndex = clickTextPoint.y;
	
	//�}�E�X�h���b�O�ɂ��I��
	
	//Windows�̏ꍇ�́A�����ň�U�C�x���g�R�[���o�b�N�𔲂���BMouseTrack�͕ʂ̃C�x���g�ōs���B
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode�ݒ�
	mMouseTrackingMode = kMouseTrackingMode_TripleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
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
void	AView_EditBox::DoMouseTrack_TripleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//�\���t���[����Image���W�Ŏ擾
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//�\���t���[���O�Ƀ}�E�X�ړ������Ƃ��̓X�N���[������(Y����)
		if( mouseImagePoint.y < frameImageRect.top )
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + DragScrollV(mouseImagePoint.y-frameImageRect.top,inModifierKeys);
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > frameImageRect.bottom )
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + DragScrollV(mouseImagePoint.y-frameImageRect.bottom,inModifierKeys);
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
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_EditBox::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�����L�����Z���{�^��tracking����
	if( mTrackingPressSearchCancelButton == true )
	{
		if( MouseIsInSearchCancelButtonRect(inLocalPoint) == true )
		{
			mPressingSearchCancelButton = true;
		}
		else
		{
			mPressingSearchCancelButton = false;
		}
		NVI_Refresh();
		return;
	}
	
	//
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	switch(mMouseTrackingMode)
	{
	  case kMouseTrackingMode_SingleClick:
		{
			//Mouse Track
			DoMouseTrack_SingleClick(mouseImagePoint,inModifierKeys);
			break;
		}
	  case kMouseTrackingMode_SingleClickInSelect:
		{
			if( mouseImagePoint.x != mMouseTrackStartImagePoint.x || mouseImagePoint.y != mMouseTrackStartImagePoint.y )
			{
				mMouseTrackResultIsDrag = true;
				//
				DoMouseTrackEnd_SingleClickInSelect(mouseImagePoint,inModifierKeys);
				//TrackingMode����
				mMouseTrackingMode = kMouseTrackingMode_None;
				NVM_SetMouseTrackingMode(false);
			}
			break;
		}
	  case kMouseTrackingMode_DoubleClick:
		{
			//Mouse Track
			DoMouseTrack_DoubleClick(mouseImagePoint,inModifierKeys);
			break;
		}
	  case kMouseTrackingMode_TripleClick:
		{
			//Mouse Track
			DoMouseTrack_TripleClick(mouseImagePoint,inModifierKeys);
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
}

//Mouse Tracking�I���i�}�E�X�{�^��Up�j
void	AView_EditBox::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�����L�����Z���{�^���N���b�N������
	if( mTrackingPressSearchCancelButton == true )
	{
		if( MouseIsInSearchCancelButtonRect(inLocalPoint) == true )
		{
			NVI_SetText(GetEmptyText());
			NVM_GetWindow().EVT_TextInputted(NVI_GetControlID());
		}
		mPressingSearchCancelButton = false;
		mTrackingPressSearchCancelButton = false;
		NVI_Refresh();
		//
		UpdateBackgroundColor();
		return;
	}
	
	//
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	//
	if( mMouseTrackingMode == kMouseTrackingMode_SingleClickInSelect )
	{
		DoMouseTrackEnd_SingleClickInSelect(mouseImagePoint,inModifierKeys);
	}
	//TrackingMode����
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}
//#688 #endif

//B0260
ANumber	AView_EditBox::DragScrollV( const ANumber inMouseOffset, const AModifierKeys inModifierKeys )
{
	ANumber	delta = GetLineHeightWithMargin();
	if( true )
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

//
void	AView_EditBox::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
											const ALocalPoint inLocalPoint )//win 080706
{
	//�X�N���[����
	ANumber	scrollPercent;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true  || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
	{
		scrollPercent = 300;
	}
	else
	{
		scrollPercent = 100;
	}
	//�X�N���[�����s
	NVI_Scroll(0,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100);
}

//
ABool	AView_EditBox::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�����L�����Z���{�^���z�o�[����
	ABool	oldHoveringSearchCancelButton = mHoveringSearchCancelButton;
	if( MouseIsInSearchCancelButtonRect(inLocalPoint) == true )
	{
		mHoveringSearchCancelButton = true;
	}
	else
	{
		mHoveringSearchCancelButton = false;
	}
	if( oldHoveringSearchCancelButton != mHoveringSearchCancelButton )
	{
		NVI_Refresh();
	}
	return true;
}

//#135
/**
�}�E�X�{�^���E�N���b�N���̃R�[���o�b�N
*/
ABool	AView_EditBox::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( mContextMenuID == kIndex_Invalid )   return false;
	//
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	globalPoint.y += GetLineHeightWithoutMargin();
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	//
	const AIndex	kCut = 0;
	const AIndex	kCopy = 1;
	const AIndex	kPaste = 2;
	//
	ABool	isInSelect = IsInSelect(mouseImagePoint,true);
	AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuEnableMenuItem(mContextMenuID,kCut,
				mEnableEdit&&NVM_GetWindow().NVI_IsControlEnable(NVI_GetControlID())&&isInSelect);
	AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuEnableMenuItem(mContextMenuID,kCopy,isInSelect);
	AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuEnableMenuItem(mContextMenuID,kPaste,
				mEnableEdit&&NVM_GetWindow().NVI_IsControlEnable(NVI_GetControlID())&&(AScrapWrapper::ExistClipboardTextScrapData()==true));
	//�R���e�L�X�g���j���[�\��
	//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(mContextMenuID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	NVMC_ShowContextMenu(mContextMenuID,globalPoint);//#688
	return true;
}

//�J�[�\��
ACursorType	AView_EditBox::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//�����L�����Z���{�^�����͖��J�[�\��
	if( MouseIsInSearchCancelButtonRect(inLocalPoint) == true )
	{
		return kCursorType_Arrow;
	}
	
	//�I��͈͓��Ȃ���J�[�\��
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
						AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
			{
				return kCursorType_ArrowCopy;
			}
			else
			{
				return kCursorType_Arrow;
			}
		}
	}
	
	//��L�ȊO��I�r�[��
	return kCursorType_IBeam;
}

//
void	AView_EditBox::EVTDO_DoTickTimer()
{
	//
	mCaretTickCount++;
	if( mCaretTickCount >= ATimerWrapper::GetCaretTime() )//#688 >����>=�֕ύX
	{
		if( NVI_IsFocusActive() == true )
		{
			BlinkCaret();
		}
		else
		{
			HideCaret();
		}
	}
}

//
ABool	AView_EditBox::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	//���j���[�X�V�t���OOFF�ɐݒ� #688
	outUpdateMenu = false;
	
	//�ҏW�s�Ȃ牽�����Ȃ��ŏI��
	if( mEnableEdit == false )   return false;
	
	//�R���g���[��disable�Ȃ�e�L�X�g���͕s��
	if( NVMC_IsControlEnabled() == false )
	{
		return false;
	}
	
	//#688
	//tick timer�����O�ɕ������͂��ꂽ�ꍇ�ɁA������reserve���ꂽcaret�`������s����B
	//�i�����ŕ`�悵�Ȃ��ƃI�[�g���s�[�g�����Ƃ����A�L�����b�g�`�悪�x��Č�����j
	ShowCaretIfReserved();
	
	//���j���[�X�V�t���OON�ɐݒ� #688
	outUpdateMenu = true;
	
	ABool	handled = false;
	ABool	touch = false;//#135
	if( inText.GetItemCount() >0 )//#688 == 1 )
	{
		AModifierKeys	modifiers = inModifierKeys;//#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent);
		ABool	shift = AKeyWrapper::IsShiftKeyPressed(modifiers);
		ABool	option = AKeyWrapper::IsOptionKeyPressed(modifiers);
		ABool	cmd = AKeyWrapper::IsCommandKeyPressed(modifiers);
		ABool	ctrl = AKeyWrapper::IsControlKeyPressed(modifiers);//#688
		AUChar	ch = inText.Get(0);
		if( mListViewID != kControlID_Invalid )
		{
			if( NVM_GetWindow().NVI_GetListView(mListViewID).SPI_EditBoxInput(ch) == true )
			{
				return true;
			}
		}
		switch(/*#688 ch*/inKeyBindKey)
		{
			//arrow
		  case kKeyBindKey_Up://#688 0x1C:
		  case kKeyBindKey_Down://#688 0x1D:
		  case kKeyBindKey_Right://#688 0x1E:
		  case kKeyBindKey_Left://#688 0x1F:
			{
				//#798
				//�㉺�L�[�̏ꍇ�̓E�C���h�E�֒ʒm�i���s�����ꍇ��break�j
				if( inKeyBindKey == kKeyBindKey_Up )
				{
					if( NVM_GetWindow().OWICB_ViewArrowUpKeyPressed(NVI_GetControlID(),modifiers) == true )
					{
						handled = true;
						break;
					}
				}
				if( inKeyBindKey == kKeyBindKey_Down )
				{
					if( NVM_GetWindow().OWICB_ViewArrowDownKeyPressed(NVI_GetControlID(),modifiers) == true )
					{
						handled = true;
						break;
					}
				}
				//#688
				//char�ɕϊ�
				AUChar	ch = 0;
				switch(inKeyBindKey)
				{
				  case kKeyBindKey_Left:
					{
						ch = 0x1C;
						break;
					}
				  case kKeyBindKey_Right:
					{
						ch = 0x1D;
						break;
					}
				  case kKeyBindKey_Up:
					{
						ch = 0x1E;
						break;
					}
				  case kKeyBindKey_Down:
					{
						ch = 0x1F;
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
				//
				if( option == true )
				{
					ArrowKeyWord(ch,shift,true);
				}
				else if( cmd == true )
				{
					ArrowKeyEdge(ch,shift);
				}
				else if( shift == true )
				{
					ArrowKeySelect(ch);
				}
				else
				{
					ArrowKey(ch);
				}
				handled = true;
				break;
			}
		  case kKeyBindKey_BS://#688 0x08:
			{
				DeletePreviousChar();
				touch = true;//#135
				handled = true;
				break;
			}
		  case kKeyBindKey_DEL://#688 0x7F:
			{
				DeleteNextChar();
				touch = true;//#135
				handled = true;
				break;
			}
		  case kKeyBindKey_Period://#688 '.'://#135
			{
				if( cmd == true )
				{
					NVM_GetWindow().OWICB_ViewEscapeKeyPressed(NVI_GetControlID(),modifiers);
					handled = true;
				}
				break;
			}
		  case kKeyBindKey_Return://#688 '\r'://ReturnKey
		  case kKeyBindKey_Enter://#688 3://EnterKey
			{
				if( NVI_GetCatchReturn() == true || option == true || ctrl == true || cmd == true )//#688 option+return�͏�ɉ��s���͂ɂȂ�悤�ύX
				{
					//------------------���s�R�[�h���͂̏ꍇ------------------
					AText	text;
					text.Add(kUChar_LineEnd);
					TextInput(kEditBoxUndoTag_Typing,text);
					touch = true;//#135
				}
				else
				{
					//------------------���s�R�[�h���͂��Ȃ��ꍇ------------------
					//�i�����_�C�A���O�ł̌������s�Ȃǂ��s���B���s�R�[�h���͂����ꍇ�́A���s����Ȃ��j
					NVM_GetWindow().OWICB_ViewReturnKeyPressed(NVI_GetControlID(),modifiers);//#135
				}
				handled = true;
				break;
			}
		  case kKeyBindKey_Tab://#688 '\t':
			{
				if( NVI_GetCatchTab() == true || option == true || ctrl == true || cmd == true )//#688 option+tab�͏�Ƀ^�u���͂ɂȂ�悤�ύX
				{
					//------------------�^�u�R�[�h���͂̏ꍇ------------------
					AText	text;
					text.Add('\t');
					TextInput(kEditBoxUndoTag_Typing,text);
					touch = true;//#135
				}
				else
				{
					//------------------�^�u�R�[�h���͂��Ȃ��ꍇ------------------
					//�i�����_�C�A���O�ł̌������s�Ȃǂ��s���B�^�u�R�[�h���͂����ꍇ�́A���s����Ȃ��j
					NVM_GetWindow().OWICB_ViewTabKeyPressed(NVI_GetControlID(),modifiers);//#135
				}
				handled = true;
				break;
			}
		  case kKeyBindKey_Escape://#688 0x1B://#137
			{
				NVM_GetWindow().OWICB_ViewEscapeKeyPressed(NVI_GetControlID(),modifiers);//#135
				handled = true;
				break;
			}
		  default:
			{
				//�����Ȃ�
				break;
			}
		}
		if( ch <= 0x1F )   handled = true;
	}
	//#137
	AText	yen;
	yen.SetLocalizedText("Text_Yen");
	if( yen.Compare(inText) == true )
	{
		AText	backslash;
		backslash.Add(kUChar_Backslash);
		if( NVI_GetInputBackslashByYenKeyMode() == true )
		{
			TextInput(kEditBoxUndoTag_Typing,backslash);
			touch = true;//#135
			handled = true;
		}
	}
	//#137
	//�Y�����j���[�R�}���h���g�p�s�\�ȏꍇ�̃R�}���h�V���[�g�J�b�g�L�[�̏ꍇ�A�������͂Ƃ��đ����Ă���
	//�̂ŁA����͕������͂Ƃ��Ĉ���Ȃ��悤�ɂ���
	if( AKeyWrapper::IsCommandKeyPressed(/*#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent)*/inModifierKeys) == true )
	{
		handled = true;
	}
	
	//#135 �L�[�t�B���^�[
	switch(NVI_GetDataType())
	{
	  case kDataType_Number:
		{
			for( AIndex i = 0; i < inText.GetItemCount(); i++ )
			{
				AUChar ch = inText.Get(i);
				if( (ch >= '0' && ch <= '9') || ch == '-' )   continue;
				else 
				{
					handled = true;
					break;
				}
			}
			break;
		}
	  case kDataType_FloatNumber:
		{
			for( AIndex i = 0; i < inText.GetItemCount(); i++ )
			{
				AUChar ch = inText.Get(i);
				if( (ch >= '0' && ch <= '9') || ch == '-' || ch == '.' )   continue;
				else 
				{
					handled = true;
					break;
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
	//
	if( handled == false )
	{
		if( inText.GetItemCount() > 0 )//win
		{
			TextInput(kEditBoxUndoTag_Typing,inText);
			touch = true;//#135
			handled = true;
		}
		else//win
		{
			handled = false;
		}
	}
	//#0 SPI_UpdateImageSize();
	//win ACursorWrapper::ObscureCursor();
	//#135
	if( touch == true )
	{
		SPI_UpdateImageSize();
		NVM_GetWindow().EVT_TextInputted(NVI_GetControlID());
	}
	return handled;//win
}

//���L�[�i�m�[�}���j
void	AView_EditBox::ArrowKey( const AUChar inChar )
{
	//ArrowKeySelect�Ɠ����A���S���Y������_���H�����K�v
	ATextPoint	caretTextPoint;
	AImagePoint	imagePoint;
	switch(inChar)
	{
		//��
	  case 0x1C:
		{
			if( IsCaretMode() == true )
			{
				GetCaretTextPoint(caretTextPoint);
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
					SetCaretTextPoint(caretTextPoint);
				}
			}
			else
			{
				ATextPoint	pt1, pt2;
				OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt1);
			}
			break;
		}
		//�E
	  case 0x1D:
		{
			if( IsCaretMode() == true )
			{
				GetCaretTextPoint(caretTextPoint);
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
					SetCaretTextPoint(caretTextPoint);
				}
			}
			else
			{
				ATextPoint	pt1, pt2;
				OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt2);
			}
			break;
		}
		//��
	  case 0x1E:
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
					imagePoint.y = caretTextPoint.y * GetLineHeightWithMargin() +2;
					imagePoint.x = GetOldArrowImageX();
					GetTextPointFromImagePoint(imagePoint,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);
				SetOldArrowImageX(svOldArrowImageX);
			}
			else
			{
				ATextPoint	pt1, pt2;
				OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt1);
			}
			break;
		}
		//��
	  case 0x1F:
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
					imagePoint.y = caretTextPoint.y * GetLineHeightWithMargin() +2;
					imagePoint.x = GetOldArrowImageX();
					GetTextPointFromImagePoint(imagePoint,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);
				SetOldArrowImageX(svOldArrowImageX);
			}
			else
			{
				ATextPoint	pt1, pt2;
				OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
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
void	AView_EditBox::ArrowKeySelect( const AUChar inChar )
{
	ATextPoint	selectPoint = GetCaretTextPoint();
	if( IsCaretMode() == false )
	{
		selectPoint = GetSelectTextPoint();
	}
	ATextIndex	selectPos = GetTextDocument().SPI_GetTextIndexFromTextPoint(selectPoint);
	switch(inChar)
	{
		//��
	  case 0x1C:
		{
			selectPos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(selectPos);
			GetTextDocument().SPI_GetTextPointFromTextIndex(selectPos,selectPoint,true);//B0000
			break;
		}
		//�E
	  case 0x1D:
		{
			selectPos = GetTextDocumentConst().SPI_GetNextCharTextIndex(selectPos);
			GetTextDocument().SPI_GetTextPointFromTextIndex(selectPos,selectPoint,true);//B0000
			break;
		}
		//��
	  case 0x1E:
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
				imagePoint.y -= GetLineHeightWithMargin();
				GetTextPointFromImagePoint(imagePoint,selectPoint);
			}
			break;
		}
		//��
	  case 0x1F:
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
				imagePoint.y += GetLineHeightWithMargin();
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
	SetSelect(GetCaretTextPoint(),selectPoint);
	
	//�X�N���[���ʒu�␳
	AdjustScroll(GetSelectTextPoint());
}

//���L�[�i�P��ړ��j
void	AView_EditBox::ArrowKeyWord( const AUChar inChar, const ABool inSelect, const ABool inLineStop )
{
	ATextPoint	textpoint = GetCaretTextPoint();
	if( IsCaretMode() == false )
	{
		if( inSelect == false )
		{
			ArrowKey(inChar);
			return;
		}
		textpoint = GetSelectTextPoint();
	}
	ATextIndex	textpos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(textpoint);
	switch(inChar)
	{
		//��
	  case 0x1C:
		{
			if( textpos == 0 )   break;
			textpos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(textpos);
			ATextIndex	spos,epos;
			GetTextDocumentConst().SPI_FindWord(textpos,spos,epos);
			while( spos > 0 )
			{
				if( inLineStop == true && GetTextDocumentConst().SPI_GetTextChar(spos-1) == kUChar_LineEnd )   break;
				if( GetTextDocumentConst().SPI_IsAsciiAlphabet(spos) == true || GetTextDocumentConst().SPI_IsUTF8Multibyte(spos) == true )   break;
				spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
				GetTextDocumentConst().SPI_FindWord(spos,spos,epos);
			}
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,textpoint,true);//B0000
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
	  case 0x1D:
		{
			ATextIndex	spos,epos;
			GetTextDocumentConst().SPI_FindWord(textpos,spos,epos);
			while( epos < GetTextDocumentConst().SPI_GetTextLength() )
			{
				if( inLineStop == true && GetTextDocumentConst().SPI_GetTextChar(epos) == kUChar_LineEnd )   break;
				if( GetTextDocumentConst().SPI_IsAsciiAlphabet(epos-1) == true || GetTextDocumentConst().SPI_IsUTF8Multibyte(epos-1) == true )   break;//B0000
				GetTextDocumentConst().SPI_FindWord(epos,spos,epos);
			}
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,textpoint,true);//B0000
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
	  case 0x1E:
	  case 0x1F:
		{
			if( inSelect == true )
			{
				ArrowKeySelect(inChar);
			}
			else
			{
				ArrowKey(inChar);
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
void	AView_EditBox::ArrowKeyEdge( const AUChar inChar, const ABool inSelect )
{
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	ATextPoint	textpoint;
	switch(inChar)
	{
		//��
	  case 0x1C:
		{
			textpoint = start;
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
	  case 0x1D:
		{
			textpoint = end;
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
	  case 0x1E:
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
	  case 0x1F:
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

//
void	AView_EditBox::SetOldArrowImageX( const ANumber inImageX )
{
	mOldArrowImageX = inImageX;
}

ANumber	AView_EditBox::GetOldArrowImageX() const
{
	return mOldArrowImageX;
}

//
void	AView_EditBox::DeleteNextChar()
{
	if( IsCaretMode() == false )
	{
		DeleteSelect(kEditBoxUndoTag_DELKey);
	}
	else
	{
		AIndex	start = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
		AIndex	end = GetTextDocumentConst().SPI_GetNextCharTextIndex(start);
		if( end > GetTextDocumentConst().SPI_GetTextLength() )   return;
		//Undo�A�N�V�����^�O�L�^
		GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_DELKey);
		//
		DeleteTextFromDocument(start,end);
	}
}

//
void	AView_EditBox::DeletePreviousChar()
{
	if( IsCaretMode() == false )
	{
		DeleteSelect(kEditBoxUndoTag_BSKey);
	}
	else
	{
		AIndex	end = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
		AIndex	start = GetTextDocumentConst().SPI_GetPrevCharTextIndex(end);
		if( start < 0 )   return;
		//Undo�A�N�V�����^�O�L�^
		GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_BSKey);
		//
		DeleteTextFromDocument(start,end);
	}
}

//
void	AView_EditBox::DeleteSelect( const AEditBoxUndoTag inUndoActionTag )
{
	if( IsCaretMode() == true )   return;
	//Undo�A�N�V�����^�O�L�^
	GetTextDocument().SPI_RecordUndoActionTag(inUndoActionTag);
	//�I��͈͍폜���s
	DeleteTextFromDocument();
}

void	AView_EditBox::UndoRedo( const ABool inUndo )
{
	//
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());
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
	ATextPoint	caretPoint, selectPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(selectTextIndex,caretPoint);
	SetCaretTextPoint(caretPoint);
	if( selectTextLength > 0 )
	{
		GetTextDocument().SPI_GetTextPointFromTextIndex(selectTextIndex+selectTextLength,selectPoint);
		SetSelectTextPoint(selectPoint);
	}
	AdjustScroll(GetCaretTextPoint());
	//
	Refresh();
}

//
void	AView_EditBox::Cut( const ABool inAdditional )//R0213
{
	AText	text;
	GetSelectedText(text);
	if( inAdditional == true )//R0213
	{
		AText	scrapText;
		AScrapWrapper::GetClipboardTextScrap(scrapText);//#688,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName),true);
		text.InsertText(0,scrapText);
	}
	AScrapWrapper::SetClipboardTextScrap(text,true,true);//#1044 #688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName),true);//win
	DeleteSelect(kEditBoxUndoTag_Cut);
}

//
void	AView_EditBox::Copy( const ABool inAdditional )//R0213
{
	AText	text;
	GetSelectedText(text);
	if( text.GetItemCount() == 0 )   return;
	if( inAdditional == true )//R0213
	{
		AText	scrapText;
		AScrapWrapper::GetClipboardTextScrap(scrapText);//#688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName),true);//win
		text.InsertText(0,scrapText);
	}
	AScrapWrapper::SetClipboardTextScrap(text,true,true);//#1044 #688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName),true);//win 
}

void	AView_EditBox::Paste()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	AText	text;
	AScrapWrapper::GetClipboardTextScrap(text);//#688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName),true);//win
	TextInput(kEditBoxUndoTag_Paste,text);
}

//
void	AView_EditBox::TextInput( const AEditBoxUndoTag inUndoActionTag, const AText& inText )
{
	//Undo�A�N�V�����^�O�L�^
	if( inUndoActionTag != kEditBoxUndoTag_NOP )//B0381
	{
		GetTextDocument().SPI_RecordUndoActionTag(inUndoActionTag);
	}
	
	//�I��͈͍폜�A�e�L�X�g�}��
	DeleteAndInsertTextToDocument(inText);
	
}

//�h�L�������g�փe�L�X�g�}��
void	AView_EditBox::DeleteAndInsertTextToDocument( const AText& inText )
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
void	AView_EditBox::InsertTextToDocument( const ATextIndex inInsertIndex, const AText& inText )
{
	//
	GetTextDocument().SPI_InsertText(inInsertIndex,inText);
	//
	SPI_UpdateImageSize();
	//�L�����b�g�ݒ�
	ATextPoint	pt;
	GetTextDocument().SPI_GetTextPointFromTextIndex(inInsertIndex+inText.GetItemCount(),pt);
	SetCaretTextPoint(pt);
	//
	AdjustScroll(GetCaretTextPoint());
	//#725
	//�w�i�F�X�V�i��̏ꍇ�ɔw�i�F���ς��ꍇ�ɑΉ��j
	UpdateBackgroundColor();
	//
	Refresh();
}
void	AView_EditBox::InsertTextToDocument( const ATextPoint& inInsertPoint, const AText& inText )
{
	InsertTextToDocument(GetTextDocument().SPI_GetTextIndexFromTextPoint(inInsertPoint),inText);
}

//�h�L�������g����e�L�X�g�폜
void	AView_EditBox::DeleteTextFromDocument( const ATextIndex inStart, const ATextIndex inEnd, const ABool inDontRedraw )
{
	//
	GetTextDocument().SPI_DeleteText(inStart,inEnd);
	//
	SPI_UpdateImageSize();
	//�L�����b�g�ݒ�
	ATextPoint	pt;
	GetTextDocument().SPI_GetTextPointFromTextIndex(inStart,pt);
	SetCaretTextPoint(pt);
	//
	AdjustScroll(GetCaretTextPoint());
	//#725
	//�w�i�F�X�V�i��̏ꍇ�ɔw�i�F���ς��ꍇ�ɑΉ��j
	UpdateBackgroundColor();
	//
	Refresh();
}
void	AView_EditBox::DeleteTextFromDocument( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inDontRedraw )
{
	DeleteTextFromDocument(GetTextDocument().SPI_GetTextIndexFromTextPoint(inStartPoint),GetTextDocument().SPI_GetTextIndexFromTextPoint(inEndPoint),
			inDontRedraw);
}
//�h�L�������g����e�L�X�g�폜�i�I��͈́j
void	AView_EditBox::DeleteTextFromDocument()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	DeleteTextFromDocument(spt,ept);
}

//
ABool	AView_EditBox::EVTDO_DoInlineInput( const AText& inText, const AItemCount inFixLen,
		const AArray<AIndex>& inHiliteLineStyleIndex, const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
		const AItemCount inReplaceLengthInUTF16,//#688
		ABool& outUpdateMenu )
{
	//�ҏW�s�Ȃ牽�����Ȃ��ŏI��
	if( mEnableEdit == false )   return false;
	
	//�R���g���[��disable�Ȃ�e�L�X�g���͕s��
	if( NVMC_IsControlEnabled() == false )
	{
		return false;
	}
	
	//���j���[��ԍX�V
	outUpdateMenu = true;//#135
	
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
		GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_Typing);
	}
	
	AText	insertText;
	insertText.SetText(inText);
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
			GetTextDocument().SPI_DeleteText(spos,epos);
		}
		//�}���ʒu�͑O�񖢊m�蕔���̂����폜���Ȃ����������̍Ōォ��
		insertionTextIndex = mInlineInputNotFixedTextPos + samecount;
		inlineInputFirstPos = mInlineInputNotFixedTextPos;
		//
		mInlineInputNotFixedTextPos = mInlineInputNotFixedTextPos+inFixLen;
		mInlineInputNotFixedTextLen = inText.GetItemCount() - inFixLen;
		//
		insertText.Delete(0,samecount);
	}
	else
	{
		//InlineInput�ŏ�����̏ꍇ
		ATextPoint	insertionPoint;
		if( IsCaretMode() == true )
		{
			//#688 �p��2�x�����Ή��B���݂̑I���ʒu����w�蕶�������O�܂ł��폜����B
			if( inReplaceLengthInUTF16 > 0 )
			{
				AIndex	spos = 0, epos = 0;
				SPI_GetSelect(spos,epos);
				for( AIndex i = 0; i < inReplaceLengthInUTF16; i++ )
				{
					spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
				}
				DeleteTextFromDocument(spos,epos);
			}
			//
			GetCaretTextPoint(insertionPoint);
		}
		else
		{
			ATextPoint	ept;
			SPI_GetSelect(insertionPoint,ept);
			DeleteTextFromDocument(insertionPoint,ept);
			GetCaretTextPoint(insertionPoint);//B0349 �폜�̌��ʁA�L�����b�g��TextPoint�͕ς�邱�Ƃ�����B�i�s�܂�Ԃ�����̏ꍇ�j
		}
		insertionTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(insertionPoint);
		inlineInputFirstPos = GetTextDocument().SPI_GetTextIndexFromTextPoint(insertionPoint);
		//
		mInlineInputNotFixedTextPos = insertionTextIndex+inFixLen;
		mInlineInputNotFixedTextLen = inText.GetItemCount() - inFixLen;
	}
	
	if( mInlineInputNotFixedTextLen > 0 )//B0351 AquaSKK�΍�@�S�m��Ȃ�n�C���C�g�������͑S�폜��Ԃɂ���
	{
		//InlineInput�n�C���C�g�����̏���ۑ�
		mHiliteLineStyleIndex.SetFromObject(inHiliteLineStyleIndex);
		mHiliteStartPos.SetFromObject(inHiliteStartPos);
		mHiliteEndPos.SetFromObject(inHiliteEndPos);
		//mHiliteStartPos, mHiliteEndPos�̓e�L�X�g�S�̂̃C���f�b�N�X�ɂ��Ă���
		for( AIndex i = 0; i < mHiliteLineStyleIndex.GetItemCount(); i++ )
		{
			mHiliteStartPos.Set(i,mHiliteStartPos.Get(i)+inlineInputFirstPos);
			mHiliteEndPos.Set(i,mHiliteEndPos.Get(i)+inlineInputFirstPos);
		}
	}
	
	//B0332
	if( insertText.GetItemCount() >= 2 )
	{
		AUChar	ch1 = insertText.Get(0);
		AUChar	ch2 = insertText.Get(1);
		if( ch1 == 0xC2 && ch2 == 0xA5 )
		{
			if( NVI_GetInputBackslashByYenKeyMode() == true )
			{
				insertText.Delete(0,2);
				insertText.Insert1(0,kUChar_Backslash);
				//B0380 Fix������0�̏ꍇ�́A�o�C�g�������炵��������NotFixed�̕����̂͂��Ȃ̂ŁAmInlineInputNotFixedTextLen��1byte�����炷
				if( inFixLen == 0 )
				{
					mInlineInputNotFixedTextLen--;
				}
			}
		}
	}
	
	//�e�L�X�g�}��
	//InsertTextToDocument(insertionTextIndex,inText);
	GetTextDocument().SPI_InsertText(insertionTextIndex,insertText);
	
	//�L�����b�g�ݒ�
	ATextPoint	caretPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(insertionTextIndex+insertText.GetItemCount(),caretPoint);
	
	//Hilite���ɃL�����b�g��񂪊܂܂�Ă���ꍇ�͂���ɏ]��
	if( mInlineInputNotFixedTextLen > 0 )//B0351 AquaSKK�΍�@�S�m��Ȃ�L�����b�g�͓��͕�����Ō�̂܂܂ɂ��Ă���
	{
		for( AIndex i = 0; i < inHiliteLineStyleIndex.GetItemCount(); i++ )
		{
			if( inHiliteLineStyleIndex.Get(i) == kIndex_Invalid )
			{
				GetTextDocument().SPI_GetTextPointFromTextIndex(inlineInputFirstPos+inHiliteStartPos.Get(i),caretPoint);
			}
		}
	}
	
	//
	SetCaretTextPoint(caretPoint);
	AdjustScroll(GetCaretTextPoint());
	
	//
	ATextPoint	insertionTextPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(insertionTextIndex,insertionTextPoint);
	ATextPoint	oldNotFixedStartTextPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(mInlineInputNotFixedTextPos-inFixLen,oldNotFixedStartTextPoint);
	if( oldNotFixedStartTextPoint.y < insertionTextPoint.y )
	{
		//
		AImageRect	imageLineRect;
		GetLineImageRect(oldNotFixedStartTextPoint.y,insertionTextPoint.y,imageLineRect);
		ALocalRect	localLineRect;
		NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
		NVMC_RefreshRect(localLineRect);
	}
	//#725
	//�w�i�F�X�V�i��̏ꍇ�ɔw�i�F���ς��ꍇ�ɑΉ��j
	UpdateBackgroundColor();
	
	//
	Refresh();
	
	//#135
	NVM_GetWindow().EVT_TextInputted(NVI_GetControlID());
	
	//win Caret������΍�i�r���ŕ`�悹���ɁA�����ł܂Ƃ߂ĕ\���j
	//Caret�\���\�񂪂���Ε\������
	//�L�����b�g�c��΍� ShowCaretIfReserved();
	
	return true;
}

ABool	AView_EditBox::EVTDO_DoInlineInputOffsetToPos( const AIndex inOffset, ALocalPoint& outPos )
{
	ATextPoint	textPoint;
	if( mInlineInputNotFixedTextLen > 0 )
	{
		GetTextDocument().SPI_GetTextPointFromTextIndex(mInlineInputNotFixedTextPos+inOffset,textPoint);
	}
	else
	{
		GetCaretTextPoint(textPoint);
	}
	AImagePoint	imagePoint;
	GetImagePointFromTextPoint(textPoint,imagePoint);
	imagePoint.y += GetLineHeightWithoutMargin();
	NVM_GetLocalPointFromImagePoint(imagePoint,outPos);
	return true;
}

ABool	AView_EditBox::EVTDO_DoInlineInputPosToOffset( const ALocalPoint& inPos, AIndex& outOffset )
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

ABool	AView_EditBox::EVTDO_DoInlineInputGetSelectedText( AText& outText )
{
	GetSelectedText(outText);//#373
	return true;
}

//
void	AView_EditBox::EVTDO_DrawPreProcess()
{
	TempHideCaret();
}

//
void	AView_EditBox::EVTDO_DrawPostProcess()
{
	RestoreTempHideCaret();
}

//Drag&Drop�����iDrag���j
void	AView_EditBox::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
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
	//�\���t���[����Image���W�Ŏ擾
	AImageRect	frameImageRect;
	NVM_GetImageViewRect(frameImageRect);
	//
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	//scroll
	/*#688 
	if( mMouseTrackByLoop == true )
	{
		if( clickImagePoint.y < frameImageRect.top+kDragDropScrollRange && clickImagePoint.y > frameImageRect.top && frameImageRect.top > 0 )
		{
			AImagePoint	mouseImagePoint = clickImagePoint;
			AWindowPoint	mouseWindowPoint;
			AModifierKeys	modifiers;
			while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
			{
				//Image���W�ɕϊ�
				ALocalPoint	mouseLocalPoint;
				NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
				NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
				//
				NVM_GetImageViewRect(frameImageRect);
				//
				if( frameImageRect.top <= 0 )   break;
				//
				if( mouseImagePoint.y < frameImageRect.top+kDragDropScrollRange && mouseImagePoint.y > frameImageRect.top )
				{
					mouseImagePoint.y = frameImageRect.top+kDragDropScrollRange + 
							DragScrollV(mouseImagePoint.y-(frameImageRect.top+kDragDropScrollRange),inModifierKeys);
				}
				else
				{
					break;
				}
			}
			clickImagePoint = mouseImagePoint;
		}
		else if( clickImagePoint.y > frameImageRect.bottom-kDragDropScrollRange && clickImagePoint.y < frameImageRect.bottom &&
					frameImageRect.bottom < NVM_GetImageHeight() )
		{
			AImagePoint	mouseImagePoint = clickImagePoint;
			AWindowPoint	mouseWindowPoint;
			AModifierKeys	modifiers;
			while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
			{
				//Image���W�ɕϊ�
				ALocalPoint	mouseLocalPoint;
				NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
				NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
				//
				NVM_GetImageViewRect(frameImageRect);
				//
				if( frameImageRect.bottom >= NVM_GetImageHeight() )   break;
				//
				if( mouseImagePoint.y > frameImageRect.bottom-kDragDropScrollRange && mouseImagePoint.y < frameImageRect.bottom )
				{
					mouseImagePoint.y = frameImageRect.bottom-kDragDropScrollRange + 
							DragScrollV(mouseImagePoint.y-(frameImageRect.bottom-kDragDropScrollRange),inModifierKeys);
				}
				else
				{
					break;
				}
			}
			clickImagePoint = mouseImagePoint;
		}
	}
	else
	*/
	//688{
	//�C�x���g�h���u���Ńh���b�O�X�N���[��������
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
	//#688 }
	//
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	if( IsInSelect(clickImagePoint,false) == false )
	{
		//�P��u������Drag&Drop
		if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true )
		{
			ATextIndex	clickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			ATextIndex	wordStart, wordEnd;
			GetTextDocument().SPI_FindWord(clickTextIndex,wordStart,wordEnd);
			if( wordStart != wordEnd )
			{
				GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,mDragCaretTextPoint,true);
				GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,mDragSelectTextPoint,false);
				//Draw
				XorWordDragDrop();
				mDragWordDragDrop = true;
			}
			else
			{
				mDragCaretTextPoint = clickTextPoint;
				//Draw
				XorDragCaret();
				mDragDrawn = true;
			}
		}
		//�ʏ�Drag&Drop
		else
		{
			mDragCaretTextPoint = clickTextPoint;
			//Draw
			XorDragCaret();
			mDragDrawn = true;
		}
	}
	if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true ||
				AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true ||//win 080709
				mDragging == false )//B0347
	{
		//#688 ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
		outIsCopyOperation = true;//#688
	}
	else
	{
		//#688 ACursorWrapper::SetCursor(kCursorType_Arrow);
		outIsCopyOperation = false;//#688
	}
}

//Drag&Drop�����iDrag��View�ɓ������j
void	AView_EditBox::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	GetTextPointFromImagePoint(clickImagePoint,mDragCaretTextPoint);
	if( IsInSelect(clickImagePoint,false) == false )
	{
		//Draw
		XorDragCaret();
		mDragDrawn = true;
	}
}

//Drag&Drop�����iDrag��View����o���j
void	AView_EditBox::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
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
void	AView_EditBox::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
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
	
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());//B0079
	
	//URL Drop
	AText	url, title;
	if( AScrapWrapper::GetURLDragData(inDragRef,url,title) == true )
	{
		//DragCaret�ʒu���L�����b�g�ʒu�Ƃ��Đݒ�
		SetCaretTextPoint(clickTextPoint);
		//
		AText	text;
		AIndex	caretIndex = kIndex_Invalid;
		text.SetText(url);
		text.Add(kUChar_LineEnd);
		text.AddText(title);
		TextInput(kEditBoxUndoTag_DragDrop,text);
		if( caretIndex != kIndex_Invalid )
		{
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			textindex += caretIndex;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textindex,clickTextPoint);
			SetCaretTextPoint(clickTextPoint);
		}
		return;
	}
	
	AText	text;
	AScrapWrapper::GetTextDragData(inDragRef,text);//#688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName));//win
	if( text.GetItemCount() > 0 )//B0348
	{
		//
		if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true )//B0347 if�u���b�N��������ւ��ishift�D��j
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
			TextInput(kEditBoxUndoTag_DragDrop,text);
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
			//DragCaret�ʒu���L�����b�g�ʒu�Ƃ��Đݒ�
			SetCaretTextPoint(clickTextPoint);
			//#231
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			//
			TextInput(kEditBoxUndoTag_DragDrop,text);
			//
			ATextPoint	ept = GetCaretTextPoint();
			//#231 ���ł�clickTextPoint�͐������Ȃ��\��������i�s�܂�Ԃ�����̏ꍇ�A�����}����́A�}��������J�n�ʒu���O�s�Ɉړ�����j
			ATextPoint	spt;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textindex,spt);
			SetSelect(/*#231 clickTextPoint*/spt,ept);
		}
		else
		{
			//�ړ���TextIndex��ۑ�
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			//
			ATextIndex	spos, epos;
			SPI_GetSelect(spos,epos);
			
			//�폜���TextIndex���v�Z�i�ړ��悪�A���݂̑I��͈͈ȍ~�̏ꍇ�A�폜����Ƃ��̕��A�O�Ɉړ����Ȃ��Ă͂����Ȃ��j
			ATextIndex	newtextindex = textindex;
			if( textindex >= epos )
			{
				newtextindex = textindex - (epos-spos);
			}
			//Undo�A�N�V�����^�O�L�^
			GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_DragDrop);
			//�폜
			DeleteTextFromDocument();
			//DragCaret�ʒu���L�����b�g�ʒu�Ƃ��Đݒ�
			ATextPoint	textpoint;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(newtextindex,textpoint);
			SetCaretTextPoint(textpoint);
			//
			//�e�L�X�g�}��
			InsertTextToDocument(textpoint,text);
			//
			ATextPoint	ept = GetCaretTextPoint();
			//#231 ���ł�textpoint�͐������Ȃ��\��������i�s�܂�Ԃ�����̏ꍇ�A�����}����́A�}��������J�n�ʒu���O�s�Ɉړ�����j
			ATextPoint	spt;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(newtextindex,spt);
			SetSelect(/*#231 textpoint*/spt,ept);
		}
	}
}

/**
�h���b�O���̃h���b�O��L�����b�g�`��
*/
void	AView_EditBox::XorDragCaret()
{
	AImagePoint	caretImageStart, caretImageEnd;
	GetImagePointFromTextPoint(mDragCaretTextPoint,caretImageStart);
	caretImageEnd = caretImageStart;
	caretImageEnd.y += GetLineHeightWithMargin();
	ALocalPoint	caretLocalStart, caretLocalEnd;
	NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
	NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
	NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,true);//#1034 ���flush����
}

/**
�h���b�O���̃��[�h�h���b�O��L�����b�g�`��
*/
void	AView_EditBox::XorWordDragDrop()
{
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

void	AView_EditBox::Refresh()
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	NVMC_RefreshRect(localFrameRect);
}

//#725
/**
�w�i�F�X�V�i�e�L�X�g��̏ꍇ�ɔw�i�F������鏈���Ή��j
*/
void	AView_EditBox::UpdateBackgroundColor()
{
	//�i���g�̔w�i�F��DoDraw()�őΉ��j
	
	//�t���[�������݂��Ă�����A�t���[���̔w�i�F�ݒ�
	if( NVI_ExistFrameView() == true )
	{
		if( GetTextDocumentConst().SPI_GetTextLength() > 0 )
		{
			//�e�L�X�g���݂���ꍇ
			NVI_GetFrameView().SPI_SetColor(mBackgroundColor,mFrameColor,mFrameColorDeactive,mTransparency);
		}
		else
		{
			//�e�L�X�g����̏ꍇ
			NVI_GetFrameView().SPI_SetColor(mBackgroundActiveColorForEmptyState,mFrameColor,mFrameColorDeactive,mTransparency);
		}
		//�`��X�V
		NVI_GetFrameView().NVI_Refresh();
	}
}

/**
�T�C�Y�ύX������
*/
void	AView_EditBox::NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight )
{
	AdjustScroll();
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�e�L�X�g�ݒ�E�擾

//
/**
�e�L�X�g�ݒ�
*/
void	AView_EditBox::NVIDO_SetText( const AText& inText )
{
	HideCaret();
	//�܂��C�����C���C���v�b�g�͊m�肳����i�A�v�����̃e�L�X�g��ύX����̂ŁA���̑O��TSM���Ɠ���������K�v�L��j
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());
	//
	GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_Typing);
	//
	AText	text;
	text.SetText(inText);
	text.ConvertLineEndToCR();
	GetTextDocument().SPI_DeleteText(0,GetTextDocument().SPI_GetTextLength());
	GetTextDocument().SPI_InsertText(0,text);
	if( mSetSelect00AfterSetText == true )
	{
		SetSelect(0,0);
	}
	else
	{
		SetSelect(text.GetItemCount(),text.GetItemCount());//#242
	}
	SPI_UpdateImageSize();
	AdjustScroll();//#427
	UpdateBackgroundColor();//#798
	Refresh();
}

//#182
/**
�e�L�X�g�ǉ�
*/
void	AView_EditBox::NVIDO_AddText( const AText& inText )
{
	HideCaret();
	//�܂��C�����C���C���v�b�g�͊m�肳����i�A�v�����̃e�L�X�g��ύX����̂ŁA���̑O��TSM���Ɠ���������K�v�L��j
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());
	//
	GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_Typing);
	//
	AText	text;
	text.SetText(inText);
	text.ConvertLineEndToCR();
	GetTextDocument().SPI_InsertText(GetTextDocument().SPI_GetTextLength(),text);
	SPI_UpdateImageSize();
	AdjustScroll();//#427
	Refresh();
}

//#801
/**
���݂̑I��͈͍폜���ăe�L�X�g�ǉ�
*/
void	AView_EditBox::SPI_DeleteAndInsertText( const AText& inText )
{
	DeleteAndInsertTextToDocument(inText);
}

//
/**
�e�L�X�g�擾
*/
void	AView_EditBox::NVIDO_GetText( AText& outText ) const
{
	GetTextDocumentConst().SPI_GetText(outText);
}

//#182
/**
���l�ݒ�
*/
void	AView_EditBox::NVIDO_SetNumber( const ANumber inNumber )
{
	AText	text;
	text.SetNumber(inNumber);
	NVI_SetText(text);
}

//#182
/**
���l�擾
*/
ANumber	AView_EditBox::NVIDO_GetNumber() const
{
	AText	text;
	NVI_GetText(text);
	return text.GetNumber();
}

//#182
/**
Float���l�ݒ�
*/
void	AView_EditBox::NVIDO_SetFloatNumber( const AFloatNumber inNumber )
{
	AText	text;
	text.SetFormattedCstring("%g",inNumber);
	NVI_SetText(text);
}

//#182
/**
Float���l�擾
*/
AFloatNumber	AView_EditBox::NVIDO_GetFloatNumber() const
{
	AText	text;
	NVI_GetText(text);
	return text.GetFloatNumber();
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�t�H���g�ݒ�E�擾
//#182

/**
�e�L�X�g�t�H���g�ݒ�
*/
void	AView_EditBox::NVIDO_SetTextFont( const AText& inFontName, const AFloatNumber inFontSize )
{
	SPI_SetTextDrawProperty(inFontName,inFontSize,mColor,mColorDeactive);
}

//#349
/**
�t�H���g�T�C�Y�ݒ�
*/
void	AView_EditBox::SPI_SetTextFontSize( const AFloatNumber inFontSize )
{
	AText	fontname(mFontName);
	SPI_SetTextDrawProperty(fontname,inFontSize,mColor,mColorDeactive);
}

#pragma mark ===========================

#pragma mark ---�����ݒ�

//�t�H���g����Text�`��v���p�e�B���ύX���ꂽ�ꍇ�̏���
void	AView_EditBox::SPI_SetTextDrawProperty( const AText& inFontName, const AFloatNumber inFontSize, const AColor inColor, const AColor inColorDeactive )
{
	mFontName.SetText(inFontName);
	mFontSize = inFontSize;
	/*
#if IMPLEMENTATION_FOR_WINDOWS
	mFontSize *= 0.8;
	if( mFontSize < 7.8 )   mFontSize = 7.8;
#endif
	*/
	mColor = inColor;
	mColorDeactive = inColorDeactive;
	NVMC_SetDefaultTextProperty(inFontName,inFontSize,inColor,kTextStyle_Normal,true);
	NVMC_GetMetricsForDefaultTextProperty(mLineHeight,mLineAscent);
	//#894
	//�s����1�s�݂̂̏ꍇ�A�㉺�����Z���^�[�\������悤��ImageY�}�[�W���^�X�N���[���ʒu�𒲐�
	UpdateImageYMinMarginFor1LineCentering();
	//
	NVI_Refresh();
	SPI_UpdateScrollBarUnit();
}

//#677
/**
�e�L�X�g�F�ݒ�
*/
void	AView_EditBox::SPI_SetTextColor( const AColor inColor, const AColor inColorDeactive )
{
	mColor = inColor;
	mColorDeactive = inColorDeactive;
	NVMC_SetDefaultTextProperty(mFontName,mFontSize,inColor,kTextStyle_Normal,true);
	NVMC_GetMetricsForDefaultTextProperty(mLineHeight,mLineAscent);
	//#894
	//�s����1�s�݂̂̏ꍇ�A�㉺�����Z���^�[�\������悤��ImageY�}�[�W���^�X�N���[���ʒu�𒲐�
	UpdateImageYMinMarginFor1LineCentering();
	//
	NVI_Refresh();
}

//#894
/**
�s����1�s�݂̂̏ꍇ�A�㉺�����Z���^�[�\������悤��ImageY�}�[�W���^�X�N���[���ʒu�𒲐�
*/
void	AView_EditBox::UpdateImageYMinMarginFor1LineCentering()
{
	if( GetTextDocumentConst().SPI_GetLineCount() == 1 && mCenterizeWhen1Line == true )
	{
		//------------------1�s�݂̂̏ꍇ------------------
		//�C���[�W�T�C�Y�X�V
		SPI_UpdateImageSize();
		
		//�}�[�W���v�Z
		ALocalRect	localFrameRect = {0};
		NVM_GetLocalViewRect(localFrameRect);
		ANumber	margin = ((localFrameRect.bottom-localFrameRect.top)-GetLineHeightWithMargin())/2;
		if( margin > 0 )
		{
			NVI_SetImageYMargin(margin,0);
		}
		
		//�Z���^�[�ɂȂ�悤�ɃX�N���[������
		AImagePoint	origin = {0,-mImageYMinMargin};
		NVI_ScrollTo(origin);
	}
	else
	{
		//------------------�����s�̏ꍇ------------------
		//ImageY�}�[�W���Ȃ�
		NVI_SetImageYMargin(0,0);
	}
}

//#135
/**
Scroll�EPageUp/Down�P�ʐݒ�
*/
void	AView_EditBox::SPI_UpdateScrollBarUnit()
{
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(kHScrollBarUnit,GetLineHeightWithMargin(),rect.right-rect.left-kHScrollBarUnit*5,rect.bottom-rect.top-GetLineHeightWithMargin()*5);
}

//�w�i�F���ݒ�
void	AView_EditBox::SPI_SetBackgroundColor( const AColor inBackColor, const AColor inFrameColor, const AColor inFrameColorDeactive )
{
	//
	mBackgroundColor = inBackColor;
	mFrameColor = inFrameColor;
	mFrameColorDeactive = inFrameColorDeactive;
	//
	//#135 SPI_GetFrameView().SPI_SetColor(inBackColor,inFrameColor,inFrameColorDeactive);
	/*#199 dynamic_cast<AView_Frame&>(NVM_GetWindow().NVI_GetViewByControlID(mFrameViewControlID)).SPI_SetColor(inBackColor,inFrameColor,inFrameColorDeactive);
	*/
	NVI_SetFrameViewColor(inBackColor,inFrameColor,inFrameColorDeactive);
}

/**
�ҏW�^�s�ݒ�
*/
void	AView_EditBox::SPI_SetEnableEdiit( const ABool inEnable )
{
	mEnableEdit = inEnable;
	if( mEnableEdit == false )
	{
		HideCaret();
	}
}

//#455
/**
�s�X�^�C���ݒ�
*/
void	AView_EditBox::SPI_SetLineStyle( const AArray<ATextStyle>& inStyleArray, const AArray<AColor>& inColorArray )
{
	mLineStyleArray_Style.DeleteAll();
	mLineStyleArray_Color.DeleteAll();
	for( AIndex i = 0; i < inStyleArray.GetItemCount(); i++ )
	{
		mLineStyleArray_Style.Add(inStyleArray.Get(i));
		mLineStyleArray_Color.Add(inColorArray.Get(i));
	}
}

#pragma mark ===========================

#pragma mark ---�L�����b�g����

//#390
/**
�R�}���h�{���L�[simulate
*/
void	AView_EditBox::SPI_ArrowKeyEdge( const AUChar inChar, const ABool inSelect )
{
	ArrowKeyEdge(inChar,inSelect);
}

//�L�����b�g���[�h���ǂ������擾
ABool	AView_EditBox::IsCaretMode() const
{
	return mCaretMode;
}

//
void	AView_EditBox::SetCaretTextPoint( const ATextPoint& inTextPoint )
{
	if( IsCaretMode() == true )
	{
		HideCaret();
	}
	else
	{
		mCaretMode = true;
		Refresh();
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
	
	GetImagePointFromTextPoint(GetCaretTextPoint(),mCaretImagePoint);
	
	//
	SetOldArrowImageX(mCaretImagePoint.x);
	
	if( NVI_IsFocusActive() == true )
	{
		//#688 ShowCaret();
		//Caret�\���\�񂷂�
		ShowCaretReserve();//win
	}
	
	//#688
	//TextInputClient��selected range�������iInlineInput���������ǂ����̔��f�͊֐����Ŏ��{�j
	NVMC_ResetSelectedRangeForTextInput();
}

//���݂̃L�����b�g�`��f�[�^�擾
//�Ԃ�l�F�L�����b�g�����݂�ViewRect�Ɋ܂܂�邩�ǂ���
void	AView_EditBox::GetCaretDrawData( ALocalPoint& outStartPoint, ALocalPoint& outEndPoint ) const
{
	AImagePoint	imageSpt = mCaretImagePoint;
	AImagePoint	imageEpt = imageSpt;
	imageEpt.y += mLineHeight-1;
	NVM_GetLocalPointFromImagePoint(imageSpt,outStartPoint);
	NVM_GetLocalPointFromImagePoint(imageEpt,outEndPoint);
}

//R0184
/**
�L�����b�g�`��
*/
void	AView_EditBox::DrawXorCaret( const ABool inFlush )//#1034
{
	if( mReadOnly == true )
	{
		return;
	}
	
	
	//fprintf(stderr,"DrawXorCaret()");
	ALocalPoint	spt,ept;
	GetCaretDrawData(spt,ept);
	//�L�����b�g����
	spt.y -= 1;
	ept.y += 1;
	//
	NVMC_DrawXorCaretLine(spt,ept,true,inFlush,false,2);//#688
}

//�L�����b�g�\��
void	AView_EditBox::ShowCaret()
{
	if( mReadOnly == true )
	{
		return;
	}
	
	//fprintf(stderr,"ShowCaret()");
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
	}
}

//�L�����b�g��\��
void	AView_EditBox::HideCaret()
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
	}
}

/**
Caret�\���\��
Windows�ł�Caret������΍�B�iWindows�̏ꍇ�AShowCaret()���R�[������Ƃ��̎��_�ŕ`�悳���̂ŁA�����r���̉ӏ���Caret�`�悳���B
���ۂɂ͂����ŕ`�悹���AShowCaretIfReserved()�R�[�����A�܂��́A���񑦃^�C���A�E�g���ɕ`�悷��
*/
void	AView_EditBox::ShowCaretReserve()
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
	}
}

/**
Caret�\���iCaret�\���\�񎞁j
*/
void	AView_EditBox::ShowCaretIfReserved()
{
	if( mCaretTickCount >= 99999 )
	{
		ShowCaret();
	}
}

//�L�����b�g�ꎞ��\��
void	AView_EditBox::TempHideCaret()
{
	mTempCaretHideCount++;
	if( mTempCaretHideCount != 1 )   return;
	if( IsCaretMode() == true && NVI_IsFocusActive() == true )
	{
		if( mCaretBlinkStateVisible == true )
		{
			DrawXorCaret(false);//#1034 ���flush���Ȃ�
		}
	}
}

//�L�����b�g�ꎞ��\������
void	AView_EditBox::RestoreTempHideCaret()
{
	mTempCaretHideCount--;
	if( mTempCaretHideCount != 0 )   return;
	if( IsCaretMode() == true )
	{
		if( mCaretBlinkStateVisible == true && NVI_IsFocusActive() == true )
		{
			DrawXorCaret(false);//#1034 ���flush���Ȃ�
		}
	}
}

//�L�����b�g�u�����N�i�L�����b�g�^�C�}�[���ɌĂ΂��j
void	AView_EditBox::BlinkCaret()
{
	if( IsCaretMode() == true && NVI_IsFocusActive() == true )
	{
		DrawXorCaret(true);//#1034 ���flush����
		mCaretBlinkStateVisible = !mCaretBlinkStateVisible;
	}
	mCaretTickCount = 0;
}

//
void	AView_EditBox::XorCrossCaret()
{
	ALocalPoint	cspt,cept;
	GetCaretDrawData(cspt,cept);
	ALocalPoint	spt = cspt;
	ALocalPoint	ept = cept;
	ALocalRect	localFrame;
	NVM_GetLocalViewRect(localFrame);
	spt.y = localFrame.top;
	ept.y = localFrame.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,false,true);//#1034 #1034 ���flush���Ȃ�
	spt = cspt;
	ept = cept;
	spt.x = localFrame.left;
	ept.x = localFrame.right;
	spt.y = ept.y;
	NVMC_DrawXorCaretLine(spt,ept,true,false,true);//#1034 #1034 ���flush���Ȃ�
}

#pragma mark ===========================

#pragma mark ---�I�𐧌�

void	AView_EditBox::SetSelectTextPoint( const ATextPoint& inTextPoint )
{
	//inTextPoint���L�����b�g�ʒu�Ɠ����Ȃ�A�L�����b�g�Ƃ��Đݒ�
	if( inTextPoint.x == GetCaretTextPoint().x && inTextPoint.y == GetCaretTextPoint().y )
	{
		SetCaretTextPoint(inTextPoint);
		return;
	}
	
	if( IsCaretMode() == true )
	{
		HideCaret();
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
		OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
	}
	else
	{
		ATextPoint	pt1, pt2;
		OrderTextPoint(GetSelectTextPoint(),inTextPoint,pt1,pt2);
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
		
	}
	//R0108 �������̂���
	GetImagePointFromTextPoint(GetSelectTextPoint(),mSelectImagePoint);
	
	//#688
	//TextInputClient��selected range�������iInlineInput���������ǂ����̔��f�͊֐����Ŏ��{�j
	NVMC_ResetSelectedRangeForTextInput();
	
	Refresh();
}

//
ABool	AView_EditBox::SPI_GetSelect( ATextPoint& outStartPoint, ATextPoint& outEndPoint ) const
{
	if( IsCaretMode() == false )
	{
		OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),outStartPoint,outEndPoint);
	}
	else
	{
		outStartPoint = outEndPoint = GetCaretTextPoint();
	}
	return IsCaretMode();
}

//
ABool	AView_EditBox::SPI_GetSelect( ATextIndex& outStart, ATextIndex& outEnd ) const
{
	ATextPoint	spt, ept;
	ABool	result = SPI_GetSelect(spt,ept);
	outStart = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
	outEnd = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ept);
	return result;
}

//
void	AView_EditBox::SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint )
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( spt.x == inStartPoint.x && spt.y == inStartPoint.y && ept.x == inEndPoint.x && ept.y == inEndPoint.y )
	{
		return;
	}
	SetCaretTextPoint(inStartPoint);
	if( inStartPoint.x == inEndPoint.x && inStartPoint.y == inEndPoint.y )   return;
	SetSelectTextPoint(inEndPoint);
}
void	AView_EditBox::SetSelect( const AIndex inStart, const AIndex inEnd )
{
	ATextPoint	spt, ept;
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inStart,spt);
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inEnd,ept);
	SetSelect(spt,ept);
}
void	AView_EditBox::SPI_SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint )
{
	SetSelect(inStartPoint,inEndPoint);
}
void	AView_EditBox::SPI_SetSelect( const AIndex inStart, const AIndex inEnd )
{
	SetSelect(inStart,inEnd);
}

//#135
/**
�S�I��
*/
void	AView_EditBox::NVIDO_SetSelectAll()
{
	SetSelect(0,GetTextDocumentConst().SPI_GetTextLength());
	//�X�N���[���ʒu #135
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

//
void	AView_EditBox::GetSelectedText( AText& outText ) const
{
	outText.DeleteAll();
	if( IsCaretMode() == false )
	{
		ATextPoint	caretPoint, selectPoint;
		GetCaretTextPoint(caretPoint);
		GetSelectTextPoint(selectPoint);
		GetTextDocumentConst().SPI_GetText(caretPoint,selectPoint,outText);
	}
}

//�w��TextPoint���A�I��͈͓��ɂ��邩�ǂ���
//inIncludeBoundary: ���E���܂߂邩�ǂ���
ABool	AView_EditBox::IsInSelect( const AImagePoint& inImagePoint, const ABool inIncludeBoundary )
{
	if( IsCaretMode() == true )   return false;
	ATextPoint	textpoint;
	GetTextPointFromImagePoint(inImagePoint,textpoint);
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

#pragma mark ===========================

#pragma mark ---���W�ϊ�

//
void	AView_EditBox::GetImagePointFromTextPoint( const ATextPoint& inTextPoint, AImagePoint& outImagePoint ) 
{
	CTextDrawData	textDrawData(false);
	GetTextDocument().SPI_GetTextDrawData(inTextPoint.y,textDrawData);
	
	outImagePoint.x = NVMC_GetImageXByTextPosition(textDrawData,inTextPoint.x);
	outImagePoint.y = GetImageYByLineIndex(inTextPoint.y);
}

//
void	AView_EditBox::GetTextPointFromImagePoint( const AImagePoint& inImagePoint, ATextPoint& outTextPoint ) 
{
	//�e�L�X�gY���W�v�Z
	outTextPoint.y = GetLineIndexByImageY(inImagePoint.y);
	//�Ώۍs�`��f�[�^�擾
	CTextDrawData	textDrawData(false);
	GetTextDocument().SPI_GetTextDrawData(outTextPoint.y,textDrawData);
	//
	outTextPoint.x = NVMC_GetTextPositionByImageX(textDrawData,inImagePoint.x);
}

//
AIndex	AView_EditBox::GetLineIndexByImageY( const ANumber inImageY ) const
{
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
}

//
ANumber	AView_EditBox::GetImageYByLineIndex( const AIndex inLineIndex ) const
{
	return inLineIndex * GetLineHeightWithMargin();
}

//
void	AView_EditBox::GetLineImageRect( const AIndex inLineIndex, AImageRect& outLineImageRect ) const
{
	outLineImageRect.left	= 0;
	outLineImageRect.right	= NVM_GetImageWidth();
	outLineImageRect.top	= inLineIndex*GetLineHeightWithMargin();
	outLineImageRect.bottom	= (inLineIndex+1)*GetLineHeightWithMargin();
}

//
void	AView_EditBox::GetLineImageRect( const AIndex inStartLineIndex, const AIndex inEndLineIndex, AImageRect& outLineImageRect ) const
{
	outLineImageRect.left	= 0;
	outLineImageRect.right	= NVM_GetImageWidth();
	outLineImageRect.top	= inStartLineIndex*GetLineHeightWithMargin();
	outLineImageRect.bottom	= inEndLineIndex*GetLineHeightWithMargin();
}

//�s�̍����擾
ANumber	AView_EditBox::GetLineHeightWithMargin() const
{
	return mLineHeight;
}
ANumber	AView_EditBox::GetLineHeightWithoutMargin() const
{
	return mLineHeight;
}

#pragma mark ===========================

#pragma mark ---�X�N���[��

//
void	AView_EditBox::NVIDO_ScrollPreProcess( const ANumber inDeltaX, const ANumber inDeltaY, 
											   const ABool inRedraw, const ABool inConstrainToImageSize,
											   const AScrollTrigger inScrollTrigger )//#138 #1031
{
	//TempHideCaret();
}

//
void	AView_EditBox::NVIDO_ScrollPostProcess( const ANumber inDeltaX, const ANumber inDeltaY,
												const ABool inRedraw, const ABool inConstrainToImageSize,
												const AScrollTrigger inScrollTrigger )//#138 #1031
{
	//RestoreTempHideCaret();
}

//
void	AView_EditBox::AdjustScroll( const ATextPoint& inTextPoint, const AIndex inOffset )
{
	//���݂̃t���[�����擾
	AImageRect	oldFrameImageRect;
	NVM_GetImageViewRect(oldFrameImageRect);
	AImagePoint	newTopLeft;
	newTopLeft.x = oldFrameImageRect.left;
	newTopLeft.y = oldFrameImageRect.top;
	
	//�C���[�W�T�C�Y����̂Ƃ��́A���(0,0)�ɃX�N���[���iedit box�������㓙�ɁA�ُ�ʒu�ɃX�N���[�������̂�h�����߁j
	if( oldFrameImageRect.bottom - oldFrameImageRect.top <= 0 || oldFrameImageRect.right - oldFrameImageRect.left <= 0 )
	{
		AImagePoint	pt = {0,0};
		NVI_ScrollTo(pt);
		return;
	}
	
	//�t���[���̊J�n�s�A�I���s���擾
	AIndex	startLineIndex = GetLineIndexByImageY(oldFrameImageRect.top+GetLineHeightWithMargin()-1);
	AIndex	endLineIndex = GetLineIndexByImageY(oldFrameImageRect.bottom);
	
	//Y�����␳
	ATextPoint	pt = inTextPoint;
	if( pt.y < startLineIndex )
	{
		AIndex	lineIndex = pt.y-inOffset;//#137
		if( lineIndex < 0 )   lineIndex = 0;//#137
		newTopLeft.y = GetImageYByLineIndex(lineIndex);
	}
	else if( pt.y >= endLineIndex )
	{
		AIndex	lineIndex = pt.y-(endLineIndex-startLineIndex)+inOffset;//#137
		if( lineIndex >= GetTextDocument().SPI_GetLineCount() )   lineIndex = GetTextDocument().SPI_GetLineCount()-1;//#137
		newTopLeft.y = GetImageYByLineIndex(lineIndex);
	}
	
	//X�����␳
	AImagePoint	imagePoint;
	GetImagePointFromTextPoint(pt,imagePoint);
	if( imagePoint.x > oldFrameImageRect.right )
	{
		ANumber	delta = (imagePoint.x-oldFrameImageRect.right)/mAdjustScrollXOffsetRight+1;//#261
		delta *= mAdjustScrollXOffsetRight;//#261
		newTopLeft.x = oldFrameImageRect.left + delta;
	}
	else if( imagePoint.x < oldFrameImageRect.left )
	{
		ANumber	delta = (oldFrameImageRect.left-imagePoint.x)/mAdjustScrollXOffsetLeft+1;//#261
		delta *= mAdjustScrollXOffsetLeft;//#261
		newTopLeft.x = oldFrameImageRect.left -= delta;
	}
	if( newTopLeft.x < 0 )   newTopLeft.x = 0;
	
	//�X�N���[��
	NVI_ScrollTo(newTopLeft);
}

//#427
/**
�X�N���[���ʒu�����݂̑I��͈͂�������悤�ɒ���
*/
void	AView_EditBox::AdjustScroll()
{
	ATextPoint	spt = {0,0}, ept = {0,0};
	SPI_GetSelect(spt,ept);
	AdjustScroll(spt);
}

//
void	AView_EditBox::SPI_UpdateImageSize()
{
	ANumber	width = 10000;
	ANumber	height = GetTextDocument().SPI_GetLineCount() * GetLineHeightWithMargin();
	NVM_SetImageSize(width,height);
}

#pragma mark ===========================

#pragma mark ---�t�H�[�J�X
//#137

void	AView_EditBox::EVTDO_DoViewFocusActivated()
{
	if( IsCaretMode() == true )
	{
		ShowCaret();
	}
	else
	{
		NVI_Refresh();
	}
}

void	AView_EditBox::EVTDO_DoViewFocusDeactivated()
{
	if( IsCaretMode() == true )
	{
		HideCaret();
	}
	else
	{
		NVI_Refresh();
	}
	//�t�H�[�J�X�������͑I������ #135
	SetSelect(GetCaretTextPoint(),GetCaretTextPoint());
	//#391
	//������ValueChanged�����s����ƃL�[�o�C���h��Control�L�[��Off/On�����Ƃ��ɕ����񂪍폜������肪�����������l�q��
	NVM_GetWindow().EVT_ValueChanged(NVI_GetControlID());
	//�ҏW���[�h�I��
	if( mListViewID != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_GetListView(mListViewID).SPI_EndEditMode();
	}
}

//#135
/**
Undo�����N���A
*/
void	AView_EditBox::NVIDO_ClearUndoInfo()
{
	GetTextDocument().SPI_ClearUndoInfo();
}

#pragma mark ===========================

#pragma mark ---�T�[�`�L�����Z���{�^��

/**
�����L�����Z���{�^��rect�擾
*/
void	AView_EditBox::GetSearchCancelButtonRect( ALocalRect& outRect ) const
{
	const ANumber	kCancelButtonOffset = 16;
	NVM_GetLocalViewRect(outRect);
	outRect.left = outRect.right - kCancelButtonOffset;
}

/**
�w��|�C���g�������L�����Z���{�^��rect�����ǂ������擾
*/
ABool	AView_EditBox::MouseIsInSearchCancelButtonRect( const ALocalPoint inMousePoint ) const
{
	//�����L�����Z���{�^����\���Ȃ���false
	if( IsSearchCancelButtonDisplayed() == false )
	{
		return false;
	}
	//�����L�����Z���{�^��rect�����ǂ����𔻒�
	ALocalRect	rect = {0};
	GetSearchCancelButtonRect(rect);
	if( inMousePoint.x >= rect.left && inMousePoint.x <= rect.right &&
				inMousePoint.y >= rect.top && inMousePoint.y <= rect.bottom )
	{
		return true;
	}
	return false;
}

/**
�����L�����Z���{�^���\�������ǂ����𔻒�
*/
ABool	AView_EditBox::IsSearchCancelButtonDisplayed() const
{
	if( mEditBoxType == kEditBoxType_FilterBox && GetTextDocumentConst().SPI_GetTextLength() > 0 )
	{
		return true;
	}
	return false;
}

#pragma mark ===========================

#pragma mark ---���擾

//#138
/**
�s�̍ő啝���擾����B

@return ���݂̃e�L�X�g�̊e�s�̕\�����̂����A�ő�̂���
*/
ANumber	AView_EditBox::SPI_GetMaxDisplayWidth() const
{
	ANumber	width = 0;
	for( AIndex lineIndex = 0; lineIndex < GetTextDocumentConst().SPI_GetLineCount(); lineIndex++ )
	{
		CTextDrawData	textDrawData(false);
		GetTextDocumentConst().SPI_GetTextDrawData(lineIndex,textDrawData);
		ANumber	w = static_cast<ANumber>(NVMC_GetDrawTextWidth(textDrawData));
		if( w > width )   width = w;
	}
	return width;
}

#pragma mark ===========================

#pragma mark ---ListView�A�g

void	AView_EditBox::SPI_SetListViewID( const AControlID inID )
{
	mListViewID = inID;
}

