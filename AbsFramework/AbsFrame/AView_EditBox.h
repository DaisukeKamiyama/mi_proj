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

#pragma once

#include "../AbsBase/ABase.h"
#include "AView.h"
#include "ADocument_EditBox.h"
#include "AView_Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AView_EditBox
/**
EditBox�pView�N���X
*/
class AView_EditBox : public AView
{
	//�R���X�g���N�^�A�f�X�g���N�^
  public:
	AView_EditBox( const AWindowID inWindowID, const AControlID inID );//#182
	virtual ~AView_EditBox();
  private:
	void					NVIDO_DoDeleted();//#92
	void					NVIDO_Init();
	ADocumentID							mDocumentID;
	AContextMenuID						mContextMenuID;//#135
	
	//<�֘A�I�u�W�F�N�g�擾>
  private:
	ADocument_EditBox&					GetTextDocument();
	const ADocument_EditBox&			GetTextDocumentConst() const;
	
	//<�C�x���g����>
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
//#688 #if IMPLEMENTATION_FOR_WINDOWS
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
//#688 #endif
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint );//win 080706
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );//#725
	ABool					EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );//#135
	void					EVTDO_DoTickTimer();
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu );
	void					UndoRedo( const ABool inUndo );
	void					Cut( const ABool inAdditional );
	void					Copy( const ABool inAdditional );
	void					Paste();
	void					TextInput( const AEditBoxUndoTag inUndoActionTag, const AText& inText );
	void					DeleteText();
	void					InsertText( const ATextPoint& inPoint, const AText& inText );
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					DeleteAndInsertTextToDocument( const AText& inText );
	void					InsertTextToDocument( const ATextIndex inInsertIndex, const AText& inText );
	void					InsertTextToDocument( const ATextPoint& inInsertPoint, const AText& inText );
	void					DeleteTextFromDocument( const ATextIndex inStart, const ATextIndex inEnd, const ABool inDontRedraw = false );
	void					DeleteTextFromDocument( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inDontRedraw = false );
	void					DeleteTextFromDocument();
	void					EVTDO_DrawPreProcess();
	void					EVTDO_DrawPostProcess();
	ABool					EVTDO_DoInlineInput( const AText& inText, const AItemCount inFixLen,
							const AArray<AIndex>& inHiliteLineStyleIndex, const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
							const AItemCount inReplaceLengthInUTF16,//#688
							ABool& outUpdateMenu );
	ABool					EVTDO_DoInlineInputOffsetToPos( const AIndex inStartOffset, const AIndex inEndOffset, ALocalRect& outRect );//#1305
	ABool					EVTDO_DoInlineInputPosToOffset( const ALocalPoint& inPos, AIndex& outOffset );
	ABool					EVTDO_DoInlineInputGetSelectedText( AText& outText );
	void					Refresh();
	void					UpdateBackgroundColor();//#725
	void					ArrowKey( const AUChar inChar );
	void					ArrowKeySelect( const AUChar inChar );
	void					ArrowKeyWord( const AUChar inChar, const ABool inSelect, const ABool inLineStop );
	void					ArrowKeyEdge( const AUChar inChar, const ABool inSelect );
	void					SetOldArrowImageX( const ANumber inImageX );
	ANumber					GetOldArrowImageX() const;
	void					DeleteNextChar();
	void					DeletePreviousChar();
	void					DeleteSelect( const AEditBoxUndoTag inUndoActionTag );
	void					DoMouseTrack_SingleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );
	void					DoMouseTrack_DoubleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );
	void					DoMouseTrack_TripleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );
	void					DoMouseDownSingleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					DoMouseDownDoubleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					DoMouseDownTripleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					DoMouseTrackEnd_SingleClickInSelect( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );
	ANumber					DragScrollV( const ANumber inMouseOffset, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
							ABool& outIsCopyOperation );
	void					EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
    void					XorDragCaret();
    void					XorWordDragDrop();
	void					NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight );
	ABool								mDragging;//B0347
	ABool								mDragDrawn;
	ABool								mDragWordDragDrop;
	ATextPoint							mDragCaretTextPoint;
	ATextPoint							mDragSelectTextPoint;
	static const ANumber				kDragDropScrollRange = 24;
	static const ANumber				kHScrollBarUnit = 30;
	AItemCount							mTempCaretHideCount;
	
	//<�C���^�[�t�F�C�X>
	
	//�e�L�X�g�ݒ�E�擾
  public:
	void					SPI_DeleteAndInsertText( const AText& inText );//#801
	void					SPI_SetSelect00AfterSetText( const ABool inSetSelect00 ) { mSetSelect00AfterSetText = inSetSelect00; }
  private:
	void					NVIDO_SetText( const AText& inText );
	void					NVIDO_AddText( const AText& inText );//#182
	void					NVIDO_GetText( AText& outText ) const;
	void					NVIDO_SetNumber( const ANumber inNumber );
	ANumber					NVIDO_GetNumber() const;
	void					NVIDO_SetFloatNumber( const AFloatNumber inNumber );
	AFloatNumber			NVIDO_GetFloatNumber() const;
	ABool								mSetSelect00AfterSetText;
	
	//�e�L�X�g�t�H���g�ݒ�E�擾
  public:
	void					SPI_SetTextFontSize( const AFloatNumber inFontSize );//#349
  private:
	void					NVIDO_SetTextFont( const AText& inFontName, const AFloatNumber inFontSize );//#182
	
	//�����ݒ�
  public:
	void					SPI_SetTextDrawProperty( const AText& inFontName, const AFloatNumber inFontSize );//#1316 , const AColor inColor, const AColor inColorDeactive );
	//#1316 void					SPI_SetTextColor( const AColor inColor, const AColor inColorDeactive );//#677
	//#1316 void					SPI_SetBackgroundColor( const AColor inBackColor, const AColor inFrameColor, const AColor inFrameColorDeactive );
	void					SPI_SetEnableEdiit( const ABool inEnable );
	void					SPI_UpdateScrollBarUnit();//#135
	void					SPI_SetLineStyle( const AArray<ATextStyle>& inStyleArray, const AArray<AColor>& inColorArray );//#455
	void					SPI_SetCenterizeWhen1Line() { mCenterizeWhen1Line = true; }//#894
  private:
	void					UpdateImageYMinMarginFor1LineCentering();//#894
	ABool					mEnableEdit;
	AArray<ATextStyle>		mLineStyleArray_Style;//#455
	AArray<AColor>			mLineStyleArray_Color;//#455
	ABool					mCenterizeWhen1Line;//#894
	
	//�L�����b�g����
  public:
	void					SPI_ArrowKeyEdge( const AUChar inChar, const ABool inSelect );
  private:
	ABool					IsCaretMode() const;
	void					SetCaretTextPoint( const ATextPoint& inTextPoint );
	void					GetCaretDrawData( ALocalPoint& outStartPoint, ALocalPoint& outEndPoint ) const;
    void					DrawXorCaret( const ABool inFlush );//#1034
	void					ShowCaret();
	void					HideCaret();
	void					ShowCaretReserve();//#688
	void					ShowCaretIfReserved();//#688
	void					TempHideCaret();
	void					RestoreTempHideCaret();
	void					BlinkCaret();
	void					XorCrossCaret();
	void					GetCaretTextPoint( ATextPoint& outTextPoint ) const { outTextPoint = mCaretTextPoint; }
	void					GetSelectTextPoint( ATextPoint& outTextPoint ) const { outTextPoint = mSelectTextPoint; }
	
	//�I�𐧌�
  public:
	ABool					SPI_GetSelect( ATextPoint& outStartPoint, ATextPoint& outEndPoint ) const;
	ABool					SPI_GetSelect( ATextIndex& outStart, ATextIndex& outEnd ) const;
	void					SPI_SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint );
	void					SPI_SetSelect( const AIndex inStart, const AIndex inEnd );
	//#135 void					SPI_SetSelectAll();
  private:
	void					SetSelectTextPoint( const ATextPoint& inTextPoint );
	void					SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint );
	void					SetSelect( const AIndex inStart, const AIndex inEnd );
	void					GetSelectedText( AText& outText ) const;
	ABool					IsInSelect( const AImagePoint& inImagePoint, const ABool inIncludeBoundary );
	void					NVIDO_SetSelectAll();//#135
	
	//���W�ϊ�
  public:
	ANumber					SPI_GetLineHeightWithMargin() const { return GetLineHeightWithMargin(); }//#798
  private:
	void					GetImagePointFromTextPoint( const ATextPoint& inTextPoint, AImagePoint& outImagePoint );
	void					GetTextPointFromImagePoint( const AImagePoint& inImagePoint, ATextPoint& outTextPoint );
	AIndex					GetLineIndexByImageY( const ANumber inImageY ) const;
	ANumber					GetImageYByLineIndex( const AIndex inLineIndex ) const;
	ANumber					GetLineHeightWithMargin() const;
	ANumber					GetLineHeightWithoutMargin() const;
	void					GetLineImageRect( const AIndex inLineIndex, AImageRect& outLineImageRect ) const;
	void					GetLineImageRect( const AIndex inStartLineIndex, const AIndex inEndLineIndex, AImageRect& outLineImageRect ) const;
	
	//�X�N���[��
  public:
	void					SPI_UpdateImageSize();
	void					SPI_AdjustScroll() { AdjustScroll(); }
  private:
	void					NVIDO_ScrollPreProcess( const ANumber inDeltaX, const ANumber inDeltaY, 
													const ABool inRedraw, const ABool inConstrainToImageSize,
													const AScrollTrigger inScrollTrigger );//#138 #1031
	void					NVIDO_ScrollPostProcess( const ANumber inDeltaX, const ANumber inDeltaY,
													 const ABool inRedraw, const ABool inConstrainToImageSize,
													 const AScrollTrigger inScrollTrigger );//#138 #1031
	void					AdjustScroll( const ATextPoint& inTextPoint, const AIndex inOffset = 2 );
	void					AdjustScroll();//#427
	
	//�t�H�[�J�X #137
  private:
	void					EVTDO_DoViewFocusActivated();
	void					EVTDO_DoViewFocusDeactivated();
	
	//Undo�����N���A #135
  private:
	void					NVIDO_ClearUndoInfo();
	
	//�T�[�`�L�����Z���{�^�� #725
  private:
	void					GetSearchCancelButtonRect( ALocalRect& outRect ) const;
	ABool					MouseIsInSearchCancelButtonRect( const ALocalPoint inMousePoint ) const;
	ABool					IsSearchCancelButtonDisplayed() const;
	ABool								mHoveringSearchCancelButton;
	ABool								mPressingSearchCancelButton;
	ABool								mTrackingPressSearchCancelButton;
	
	//���擾 #138
  public:
	ANumber					SPI_GetMaxDisplayWidth() const;
	
	/*#135 AView�ֈړ�
	//�]�� #138
  public:
	ANumber					SPI_GetLeftMargin() const { return kLeftMargin; }
	ANumber					SPI_GetTopMargin() const { return kTopMargin; }
	ANumber					SPI_GetRightMargin() const { return kRightMargin; }
	ANumber					SPI_GetBottomMargin() const { return kBottomMargin; }
  private:
	static const ANumber				kLeftMargin = 4;
	static const ANumber				kTopMargin = 2;
	static const ANumber				kRightMargin = 4;
	static const ANumber				kBottomMargin = 2;
	*/
	
	//ListView�A�g
  public:
	void					SPI_SetListViewID( const AControlID inID );
  private:
	AControlID							mListViewID;
	
	/*#135 AView�ֈړ�
	//�C�x���gRedirect
  private:
	void					NVIDO_SetMouseWheelEventRedirectViewID( const AViewID inViewID )
	{ SPI_GetFrameView().NVI_SetMouseWheelEventRedirectViewID(inViewID); }
	
	//Frame View
  public:
	AView_Frame&			SPI_GetFrameView();
  private:
	void					NVIDO_SetPosition( const AWindowPoint& inWindowPoint );
	void					NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight );
	void					CreateFrameView();
	void					UpdateFrameView();
	void					DeleteFrameView();
	*/
	
  private:
	const ATextPoint&		GetCaretTextPoint() const { return mCaretTextPoint; }//#231 mCaretTextPoint���ڐݒ�����o���邽�߂ɁAmCaretTextPoint�͓ǂݍ��݂ł����Ă����ڃA�N�Z�X���Ȃ��悤�ɂ���
	const ATextPoint&		GetSelectTextPoint() const { return mSelectTextPoint; }//#231 mSelectTextPoint���ڐݒ�����o���邽�߂ɁAmSelectTextPoint�͓ǂݍ��݂ł����Ă����ڃA�N�Z�X���Ȃ��悤�ɂ���
	AText								mFontName;
	AFloatNumber						mFontSize;
	//#1316 AColor								mColor;
	ANumber								mLineHeight;
	ANumber								mLineAscent;
	ABool								mCaretMode;
	ATextPoint							mCaretTextPoint;
	ATextPoint							mSelectTextPoint;
	AImagePoint							mCaretImagePoint;
	AImagePoint							mSelectImagePoint;
	ANumber								mCaretTickCount;
	ABool								mCaretBlinkStateVisible;
	ANumber								mOldArrowImageX;
	AMouseTrackingMode					mMouseTrackingMode;//win
	//#688 ABool								mMouseTrackByLoop;//win
	ABool								mMouseTrackResultIsDrag;//win
	AImagePoint							mMouseTrackStartImagePoint;//win
	ATextPoint							mFirstSelectStart;//win
	ATextPoint							mFirstSelectEnd;//win
	AIndex								mClickTextIndex;//win
	AIndex								mClickTextLineIndex;//win
	//#1316 AColor								mBackgroundColor;
	//#1316 AColor								mFrameColor;
	//#1316 AColor								mColorDeactive;
	//#1316 AColor								mFrameColorDeactive;
	//#135 AViewID								mFrameViewID;
	
	//#688
  public:
	void					SPI_SetReadOnly() { mReadOnly = true; NVI_SetAutomaticSelectBySwitchFocus(false); }
  private:
	ABool								mReadOnly;
	
	//#261
  public:
	void					SPI_SetSetmAdjustScrollXOffset( const ANumber inRight, const ANumber inLeft )
	{ mAdjustScrollXOffsetRight = inRight; mAdjustScrollXOffsetLeft = inLeft; }
  private:
	ANumber											mAdjustScrollXOffsetRight;
	ANumber											mAdjustScrollXOffsetLeft;
	
	//Inline Input�f�[�^
  private:
	AIndex								mInlineInputNotFixedTextPos;
	AItemCount							mInlineInputNotFixedTextLen;
	AArray<AIndex>						mHiliteLineStyleIndex;
	AArray<AIndex>						mHiliteStartPos;
	AArray<AIndex>						mHiliteEndPos;
	
	AArray<AColor>						mInlineInputHiliteColorArray;
	
	//#798
	//�t�H�[�J�Xring�`��ېݒ�
  public:
	void					SPI_SetEnableFocusRing( const ABool inEnableFocusRing )
	{
		if( NVI_ExistFrameView() == true )
		{
			NVI_GetFrameView().SPI_SetEnableFocusRing(inEnableFocusRing);
		}
	}
	
	//#798
	//�e�L�X�g����̂Ƃ��̕`�敶���A�w�i�F�ݒ�
  public:
	void					SPI_SetTextForEmptyState( const AText& inText, const AText& inShortcutText )
	{ mTextForEmptyState.SetText(inText); mTextForEmptyState_Shortcut.SetText(inShortcutText); }
	/*#1316
	void					SPI_SetBackgroundColorForEmptyState( const AColor& inActiveColor, const AColor& inDeactiveColor )
	{ 
		mEnableBackgroundColorForEmptyState = true;
		mBackgroundActiveColorForEmptyState = inActiveColor;
		mBackgroundDeactiveColorForEmptyState = inDeactiveColor;
		UpdateBackgroundColor();
	}
	*/
  private:
	AText								mTextForEmptyState;
	AText								mTextForEmptyState_Shortcut;
	/*#1316
	ABool								mEnableBackgroundColorForEmptyState;
	AColor								mBackgroundActiveColorForEmptyState;
	AColor								mBackgroundDeactiveColorForEmptyState;
	*/
	
	//�����x #291
  public:
	void					SPI_SetTransparency( const AFloatNumber inTransparency ) { mTransparency = inTransparency; }
  private:
	AFloatNumber						mTransparency;
	
	//EditBox�^�C�v
  public:
	void					SPI_SetEditBoxType( const AEditBoxType inEditBoxType ) { mEditBoxType = inEditBoxType; }
  private:
	AEditBoxType						mEditBoxType;
	
	//���s�R�[�h�\��
  public:
	void					SPI_SetDrawLineEnd( const ABool inDraw ) { mDrawLineEnd = inDraw; }
  private:
	ABool								mDrawLineEnd;
	AText								mLineEndDisplayText;
	AText								mLineEndDisplayText_UTF16;
	
	//View�I�u�W�F�N�g�擾
  public:
	static AView_EditBox&		GetEditBoxViewByViewID( const AViewID inViewID )
	{
		MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_EditBox,kViewType_EditBox,inViewID);
	}
	static const AView_EditBox&	GetEditBoxViewConstByViewID( const AViewID inViewID ) 
	{
		MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_VIEWID(AView_EditBox,kViewType_EditBox,inViewID);
	}
	
	//static�f�t�H���g�t�H���g win
	//AApplication::AApplication()����R�[�������
  public:
	static void					SPI_SetDefaultFont( const AText& inDefaultFontName, const AFloatNumber inDefaultFontSize )
	{
		sDefaultFontName.SetText(inDefaultFontName);
		sDefaultFontSize = inDefaultFontSize;
	}
  private:
	static AText						sDefaultFontName;
	static AFloatNumber					sDefaultFontSize;
	
	//Object Type
  private:
	AViewType				NVIDO_GetViewType() const { return kViewType_EditBox; }
};


#pragma mark ===========================
#pragma mark [�N���X]AEditBoxViewFactory
/**
AView_EditBox�����pAbstructFactory
*/
class AEditBoxViewFactory : public AViewFactory
{
  public:
	AEditBoxViewFactory( const AWindowID inWindowID, const AControlID inID )
			: mWindowID(inWindowID), mControlID(inID)
	{
	}
	AView_EditBox*	Create() 
	{
		return new AView_EditBox(mWindowID,mControlID);
	}
  private:
	AWindowID							mWindowID;
	AControlID							mControlID;
};

