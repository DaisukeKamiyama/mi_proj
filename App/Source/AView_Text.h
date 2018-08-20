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

#pragma once

//#include "../AbsBase/ABase.h"
#include "../../AbsFramework/Frame.h"
#include "ATextUndoer.h"
#include "ATextFinder.h"
#include "AThread_CandidateFinder.h"

class ADocument_Text;
class AWindow_Text;
class CPaneForAView;
class ADocPrefDB;
class AModePrefDB;
class AView_LineNumber;//#450
class AWindow_IdInfo;
class AWindow_Notification;

//#478
enum AFigMode
{
	kFigMode_None = 0,
	kFigMode_Line
};

//#450
//折りたたみexpand/collapse指示データ
enum AExpandCollapseType
{
	kExpandCollapseType_Switch = 0,
	kExpandCollapseType_Collapse,
	kExpandCollapseType_Expand,
	kExpandCollapseType_ForceExpand,//foldinglevelに関わらず常に指定行以降のcollapsed状態を解除する
};

//#867
//キーアイドル時を待ってする処理
typedef AIndex AKeyIdleWorkType;
const AKeyIdleWorkType	kKeyIdleWorkType_None = 0;
const AKeyIdleWorkType	kKeyIdleWorkType_UpdateLocalRange = 1;
const AKeyIdleWorkType	kKeyIdleWorkType_MouseClick = 2;

//#688
//手のひらツールモード
const AMouseTrackingMode	kMouseTrackingMode_TenohiraTool = 128;

#pragma mark ===========================
#pragma mark [クラス]AView_Text

class AView_Text : public AView
{
	friend class AView_LineNumber;
	
	//コンストラクタ、デストラクタ
  public:
	AView_Text( /*#199 AObjectArrayItem* inParent, AWindow& inWindow,*/
			const AWindowID inWindowID, const AControlID inID, 
				/*#695 const ADocumentID inTextDocumentID*/
				ADocument_Text& inTextDocument, 
				const AWindowID inTextWindowID, //#379
				//#699 const ABool inCaretMode, 
				/*#699const ATextPoint& inCaretTextPoint, const ATextPoint& inSelectTextPoint, */const AImagePoint& inOriginOfLocalFrame,
			const ABool inCrossCaretMode );//B0345
	virtual ~AView_Text();
	ANumber					SPI_GetTextViewWidth() const;//#379
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();//#138
	//#695 AObjectID	mTextDocumentID;
	ADocument_Text&			mTextDocumentRef;//#695
	//高速化のため
	AText					mZenkakuSpaceText;
	ANumber					mZenkakuSpaceWidth;
	AText					mLineEndDisplayText;
	AText					mLineEndDisplayText_UTF16;
	ANumber					mACharWidth;//#1186
	
	//<関連オブジェクト取得>
  public:
	ADocument_Text&			SPI_GetTextDocument() { return mTextDocumentRef/*#695 GetTextDocument()*/; }//#212
	ADocument_Text&			GetTextDocument() { return mTextDocumentRef; }//#695
	const ADocument_Text&	SPI_GetTextDocumentConst() const { return mTextDocumentRef; }//#737
	const ADocument_Text&	GetTextDocumentConst() const { return mTextDocumentRef; }//#695
	ADocPrefDB&				GetDocPrefDB();
	AModePrefDB&			GetModePrefDB();
	void					SPI_SetLineNumberView( const AViewID inLineNumberViewID );//#450
	AView_LineNumber&		SPI_GetLineNumberView() { return GetLineNumberView(); }//#450
	void					SPI_FindNext();//win
	void					SPI_FindPrev();//win
  private:
	AView_LineNumber&		GetLineNumberView();//#450
	AViewID					mLineNumberViewID;//#450
	
	//<イベント処理>
  public:
	void					SPI_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
												const ABool inClickedAtLineNumberArea );
	ABool					SPI_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount,
											const ABool inClickedAtLineNumberArea );//#809
  private:
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					OpenSelected( const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	ABool					EVTDO_DoInlineInput( const AText& inText, const AItemCount inFixLen,
							const AArray<AIndex>& inHiliteLineStyleIndex, 
							const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos, 
							const AItemCount inReplaceLengthInUTF16,//#688
							ABool& outUpdateMenu );
	ABool					EVTDO_DoInlineInputOffsetToPos( const AIndex inStartOffset, const AIndex inEndOffset, ALocalRect& outRect );//#1305
	ABool					EVTDO_DoInlineInputPosToOffset( const ALocalPoint& inPos, AIndex& outOffset );
	ABool					EVTDO_DoInlineInputGetSelectedText( AText& outText );
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu );
	ABool					EVTDO_DoKeyDown( /*#688 const AUChar inChar*/ const AText& inText, const AModifierKeys inModifierKeys );//#1080
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//R0246
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
//#688 #if IMPLEMENTATION_FOR_WINDOWS
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					DoMouseTrack( const AModifierKeys inModifierKeys );
//#688 #endif
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	ABool					EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_QuickLook( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#1026
	void					EVTDO_DoDraw();
	void					EVTDO_DrawPreProcess();//win
	void					EVTDO_DrawPostProcess();//win
	void					EVTDO_PreProcess_SetShowControl( const ABool inVisible );//#138
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint );//win 080706
	void					EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation );
	void					EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					DeleteSelect( const AUndoTag inUndoActionTag );
	void					TextInput( const AUndoTag inUndoTag, const AText& inText );
	void					KukeiInput( const AText& inText );
	ABool					KeyBindAction( const AKeyBindKey inKeyBindKey, const AModifierKeys inModiferKeys, 
										  const AKeyBindAction inKeyBindAction,
										  const AText& inTextForInsertTextAction,
										  ABool& outUpdateMenu );//win
	void					ArrowKey( const AKeyBindAction inAction );
	AKeyBindAction			ConvertArrowKeyForVerticalMode( const AKeyBindAction inAction );//#558
	void					ArrowKeySelect( const AKeyBindAction inAction );
	void					ArrowKeyEdge( const AKeyBindAction inAction, const ABool inSelect, const ABool inParagraphEdge );
	void					ArrowKeyWord( const AKeyBindAction inAction, const ABool inSelect, const ABool inLineStop );
	void					GetSelectedText( AText& outText ) const;
	void					DoMouseDownSingleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
												   const ABool inInsideLineNumber );
	void					DoMouseDownDoubleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					DoMouseDownTripleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
												   const ABool inInsideLineNumber );
	void					DoMouseDownQuadClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					DoMouseDownTenohira( const ALocalPoint& inFirstLocalPoint , const AModifierKeys inModifierKeys );//#606
	void					DoIndent( /*#650 const ABool inUseCurrentParagraphIndentForRegExp, */const AUndoTag inUndoTag, 
							const ABool inInputSpaces, const AIndentTriggerType inIndentTriggerType );//B0363 B0381 #249 #639 #650
	void					DoIndent( /*#650 const ABool inUseCurrentParagraphIndentForRegExp, */const AUndoTag inUndoTag, 
							ATextPoint& ioStartTextPoint, ATextPoint& ioEndTextPoint, const ABool inInputSpaces,
							const AIndentTriggerType inIndentTriggerType );//B0381 #249 #639 #650
	void					DeleteNextChar();
	void					DeletePreviousChar( const ABool inIncludingDecomp = true );
	void					DeleteToLineEnd();
	void					DeleteToParagraphEnd();//R0213
	void					DeleteCurrentParagraph();//#124
	void					DeleteToLineStart();//#913
	void					DeleteToParagraphStart();//#913
	void					CutToLineEnd();
	void					CutToParagraphEnd( const ABool inAdditional );//R0213
	void					CutToLineStart();
	void					DeleteToMark();//#913
	void					Yank();//#913
	void					UndoRedo( const ABool inUndo );
	void					Cut( const ABool inAdditional );//R0213
	void					Copy( const ABool inAdditional );//R0213
	void					EnqueueFIFOClipboard( const AText& inText );
	void					DequeueFIFOClipboard( AText& outText );
	void					CopyWithHTMLColor();//R0015
	void					Paste();
	void					PasteKukei();
	void					SwitchFIFOClipboardMode();//#878
	void					PasteAtEndOfEachLine();//#929
	ATextIndex				GetIndentStart( const ATextPoint& inTextPoint ) const;
	void					HomeKey();
	void					EndKey();
	void					PageupKey( const ABool inWithCaret );
	void					PagedownKey( const ABool inWithCaret );
	void					Recenter();
	void					SwapLine();
	void					SwapLetter();//#1154
	void					EVTDO_DoTickTimer();
	ANumber					DragScrollV( const ANumber inMouseOffset, const AModifierKeys inModifierKeys );
	ABool					JudgeContextMenuByLeftClickLongTime( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );//#688
	//#717 void					HideCandidateWindow();//RC2
	void					IndentTab( const ABool inInputSpacesForIndent, const ABool inInputSpacesForTab );//#249
	void					Indent( const ABool inInputSpacesForIndent );//#255
	void					AutoTab( const ABool inInputSpaces );//B0381 #249
	void					SelectBrace();
	ABool					DoubleClickBrace( ATextPoint& ioStart, ATextPoint& ioEnd );//#688
	void					ShiftRightLeft( const ABool inRight, const ABool inInputSpaces, const ABool inIsTabKey = false );//#249 #1188
	void					InputCorrespondence();
	void					SelectBetweenCorrespondTexts();//#359
	void					SameLetterUp();
	void					DuplicateLine();
	void					InsertEmptyLine();
	//#725 InfoHeader廃止void					SetInfoHeader( const AText& inKeyword );
	//#853 void					SetIdInfo( const AText& inKeyword );//RC2 #725
	void					InputTab( const ABool inInputSpaces );//#249
	void					NextDiff();//R0006
	void					PrevDiff();//R0006
	void					DiffWithRepository();//#379
	//#853 void					IdInfo();//RC2
	void					IdInfoAuto();//RC2
	void					IdInfoAutoArg();//RC2
	void					IdInfoHighlightArg( const AIndex inTextIndex );//#853
	void					GetIdInfoArgIndex( const AIndex inTextIndex, AIndex& outFuncPos, AIndex& outArgIndex,
							AIndex& outArgStartTextIndex, AIndex& outArgEndTextIndex ) const;//#853
	void					AdjustPopupWindowsPosition();
	void					SelectWord();//B0432
	//#858 void					ExternalComment();//RC3
#if IMPLEMENTATION_FOR_MACOSX
	void					DictionaryPopup();//R0231
	void					Dictionary();//R0231
#endif
	ABool					IsHandToolMode() const;//#606
	
	void					DeleteAndInsertTextToDocument( const AText& inText );
	void					InsertTextToDocument( const ATextIndex inInsertIndex, const AText& inText, const ABool inDontRedraw = false );//#846
	void					InsertTextToDocument( const ATextPoint& inInsertPoint, const AText& inText, const ABool inDontRedraw = false );//#846
	void					DeleteTextFromDocument( const ATextIndex inStart, const ATextIndex inEnd, 
												   const ABool inDontRedraw = false,
												   const ABool inStoreToKillBuffer = true );
	void					DeleteTextFromDocument( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, 
												   const ABool inDontRedraw = false,
												   const ABool inStoreToKillBuffer = true );
	void					DeleteTextFromDocument( const ABool inStoreToKillBuffer = true );
  private:
	void					DoMouseTrack_SingleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );
	void					DoMouseTrackEnd_SingleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );//#478
	void					DoMouseTrack_DoubleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );
	void					DoMouseTrack_TripleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );
	void					DoMouseTrack_QuadClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );
	void					DoMouseTrackEnd_SingleClickInSelect( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys );
	void					StartKeyToolMode();
	void					EndKeyToolMode();//R0073
	ABool					KeyTool( const AText& inText );
	void					UpdateKeyToolListWindow();
	void					RestoreFromBraceCheck();
	void					XorDragCaret();
	void					XorWordDragDrop();
	void					Jump( const AUniqID inJumpIdentifier );//#146
	void					JumpNext();//#150
	void					JumpPrev();//#150
	void					SetFindText();//win
	ABool					IsMouseInFoldingCollapsedLine( const ALocalPoint& inLocalPoint ) ;
	void					NVIDO_SetShow( const ABool inShow );//#725
	void					NVIDO_SetPosition( const AWindowPoint& inWindowPoint );//#725
	void					NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight );//#725
	void					TriggerSyntaxRecognize();
	void					RemoveHintText( const ATextIndex inTextIndex );
	void					Transliterate( const AText& inTitle );//#1035
	void					TransliterateToUpper();//#1035
	void					TransliterateToLower();//#1035
	/*#233 テスト
	void					XorHoverCaret( const ATextPoint& inHoverCaret );//#233
	ATextPoint				mHoverCaretTextPoint;//#233
	*/
	ABool					mDragDrawn;
	ABool					mDragWordDragDrop;
	ATextPoint				mDragCaretTextPoint;
	ATextPoint				mDragSelectTextPoint;
	//#717 ATextArray				mLastAbbrevKeywordArray;
	//#717 ATextArray				mLastAbbrevInfoTextArray;//RC2
	//#717 AArray<AIndex>			mLastAbbrevCategoryIndexArray;//RC2
	//#717 AArray<AScopeType>		mLastAbbrevScopeTypeArray;//RC2
	//#717 AObjectArray<AFileAcc>	mLastAbbrevFileArray;//RC2
	//#717 ATextArray				mLastAbbrevFilePathArray;//#717
	//#717 AItemCount				mAbbrevCount;//RC2
	//#717 AIndex					mLastAbbrevKeywordIndex;
	ABool					mDisplayingBraceCheck;
	ANumber					mBraceCheckStartTickCount;
	ATextPoint				mBraceCheckSavedCaretTextPoint;
	AImagePoint				mBraceCheckSavedOrigin;
	ABool					mDragging;//B0347
	AMouseTrackingMode		mMouseTrackingMode;//win
	//#688 ABool					mMouseTrackByLoop;//win
	AImagePoint				mMouseTrackStartImagePoint;//win
	ALocalPoint				mMouseTenohiraPreviousLocalPoint;//#688
	AImagePoint				mMouseTrackImagePoint;//win
	ATextPoint				mFirstSelectStart;//win
	ATextPoint				mFirstSelectEnd;//win
	AIndex					mClickTextIndex;//win
	AIndex					mClickTextLineIndex;//win
	//選択範囲内シングルクリック後に開始位置からマウスが動いたのでドラッグすべきであることを保持するフラグ
	ABool					mMouseTrackResultIsDrag;//win
	AImagePoint				mMouseDownImagePoint;//#478
	unsigned long long		mMouseDownTick;//#688
	//キーワード情報引数ハイライト
	ABool					mIdInfoCurrentArgHighlighted;
	ATextPoint				mIdInfoCurrentArgStartTextPoint;
	ATextPoint				mIdInfoCurrentArgEndTextPoint;
	//#81
	ANumber					mAutoSaveUnsavedCheckTickCount;
	AIndex					mCurrentCaretStateColorSlotIndex;
	AModifierKeys			mCurrentMouseTrackModifierKeys;//#795
	ABool					mCurrentMouseTrackInsideLineNumber;
  public:
	void					SPI_GetDrawLineRange( AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const;//#823
	
	//#867
	//キーアイドル時処理
  private:
	void					TriggerKeyIdleWork( const AKeyIdleWorkType inWorkType, const ANumber inTimerValue );
	void					ResetKeyIdleWorkTimer( const ANumber inTimerValue );
	AKeyIdleWorkType					mKeyIdleWorkType;
	ANumber								mKeyIdleWorkTimer;
	ANumber								mCandidateRequestTimer;
	
	
	//#142
	//テキストカウント
  public:
	void					SPI_UpdateTextCountWindows() { UpdateTextCountWindows(); }//#1149
  private:
	void					UpdateTextCountWindows();
	ATextArray							mCurrentHeaderPathArray_HeaderTextArray;
	AArray<AIndex>						mCurrentHeaderPathArray_HeaderParagraphIndexArray;
	AIndex								mCurrentHeaderPathJumpMenuItemIndex;
	
	//#455
  public:
	void					SPI_NextDiff() {NextDiff();}
	void					SPI_PrevDiff() {PrevDiff();}
	
	//#262
  private:
	ABool					mSelectionByFind;
	
	//R0073
  public:
	void					SPI_KeyTool( const AUChar inKeyChar );
	
	//ツール
  public:
	void					SPI_DoTool( const AFileAcc& inFile, const AModifierKeys inModifierKeys, const ABool inDontShowDropWarning,
							const ANumber inDepth = 1 );//B0405
	void					SPI_DoTool_Text( const AText& inText, const ABool inDontShowDropWarning, const ANumber inDepth = 1 );//B0405
  private:
	void					DoTool_ShellScript( const AFileAcc& inFile, const AText& inText, const ABool inBinaryExecute );//#862
	void					DoTool_AppleScript( const AFileAcc& inFile );
	void					InsertToolText( const AText& inText, const ANumber inDepth );
	ABool					InsertToolText_Command( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
												   const AToolCommandID inToolCommandID, AText& outText,
												   AText& outHintText );
	void					InsertToolText_Command_FIND( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
			const AToolCommandID inToolCommandID, AText& outText );
	void					InsertToolText_Command_DATE( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
			const AToolCommandID inToolCommandID, AText& outText );
	void					InsertToolText_Command_MOVECARET( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
			const AToolCommandID inToolCommandID, AText& outText );
	void					InsertToolText_Command_SELECT( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
			const AToolCommandID inToolCommandID, AText& outText );
	void					InsertToolText_Command_SCROLL( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
			const AToolCommandID inToolCommandID, AText& outText );
	void					InsertToolText_Command_CHANGE( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
			const AToolCommandID inToolCommandID, AText& outText );
	void					InsertToolText_Command_EDIT( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
			const AToolCommandID inToolCommandID, AText& outText );
	void					InsertToolText_Command_FILE( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
			const AToolCommandID inToolCommandID, AText& outText );
	void					InsertToolText_Command_LAUNCH( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
														   const AToolCommandID inToolCommandID, AText& outText );
	void					Browse( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
									const AToolCommandID inToolCommandID, AText& outText, AFileAcc& inFile );//#1286
	void					ToolCommand_ReadFindArg1( const AText& inText, ATextIndex& ioPos, AText& outArg );
	void					ToolCommand_ReadArg1( const AText& inText, ATextIndex& ioPos, AText& outArg );
	void					ToolCommand_ReadFindArg2( const AText& inText, ATextIndex& ioPos, AText& outArg1, AText& outArg2 );
	void					ToolCommand_ReadQuotedText( const AText& inText, ATextIndex& ioPos, AText& outText, const AUChar inTerminateChar = '\"' );//#273
	ABool								mToolData_CaretReserved;
	AText								mToolData_SelectedText;
	ATextIndex							mToolData_CaretTextIndex;
	ATextIndex							mToolData_SelectTextIndex;
	ABool								mToolData_CaretMode;
	ABool								mToolData_UndoTagInserted;
	ABool								mToolData_CloseFlag;
	ABool								mToolData_DropWarningFlag;
	ABool								mToolData_DropWarningNextStartPos;
	ABool								mToolData_DontShowDropWarning;
	ABool								mToolData_IndentFlag;
	ABool								mToolData_IndentFlag_InputSpaces;
	ABool								mToolData_IndentFlag_OnlyNewLine;
	ABool								mToolData_ToolIndentMode;//R0000
	ABool					MakeDropText( const AFileAcc& inFile, const AText& inURL, const AText& inTitle, AText& outText, AIndex& outCaretOffset );//#91
	void					ParseFindOption( const AText& inText, ATextIndex& ioPos, AFindParameter& ioParameter );//#5
	
	//#50
  private:
	static AHashTextArray				sToolData_VarNameArray;
	static ATextArray					sToolData_VarValueArray;
	
	//<インターフェイス>
	//Update
  public:
	void					SPI_UpdateText( const AIndex inStartLineIndex, const AIndex inEndLineIndex,
							const AItemCount inInsertedLineCount, const ABool inParagraphCountChanged );//#450
	void					SPI_RefreshTextView();//#450
	void					SPI_RefreshLines( const AIndex inStartLineIndex, const AIndex inEndLineIndex );//#695
	void					SPI_RefreshLine( const AIndex inLineIndex );//#496
	void					SPI_UpdateTextDrawProperty();
	void					SPI_UpdateImageSize();
	void					SPI_UpdateScrollBarUnit();
	void					SPI_DoWrapCalculated();//#699
	void					SPI_DoWrapCalculating();//#699
	void					SPI_UpdateWindowTitleBarText();
  private:
	void					CancelReservedSelection();//#699
	
	//スクロール
  public:
	void					SPI_AdjustScroll( const AAdjustScrollType inType );
	void					SPI_AdjustScroll_Center();
	void					SPI_AdjustScroll_Top();//#454
	void					SPI_AdjustScroll_Center( const AIndex inStartLineIndex, const AIndex inEndLineIndex, 
							const ANumber inX );//#512
	void					NVIDO_ScrollPreProcess( const ANumber inDeltaX, const ANumber inDeltaY,
													const ABool inRedraw, const ABool inConstrainToImageSize,
													const AScrollTrigger inScrollTrigger );//#138 #1031
	void					NVIDO_ScrollPostProcess( const ANumber inDeltaX, const ANumber inDeltaY,
													 const ABool inRedraw, const ABool inConstrainToImageSize,
													 const AScrollTrigger inScrollTrigger );//#138 #1031
	AIndex					SPI_GetViewStartParagraphIndex() const;//#379
	AIndex					SPI_GetViewEndParagraphIndex() const;//#379
	void					SPI_ScrollToParagraph( const AIndex inStartParagraphIndex, const ANumber inOffset );//#379
	AIndex					SPI_GetOriginParagraph() const;//#450
	void					SPI_ScrollToLine( const AIndex inStartLineIndex );//#450
	AIndex					SPI_GetOriginLineIndex() const;
  private:
	void					AdjustScroll( const ATextPoint& inTextPoint, const AIndex inOffset = kAdjustScrollOffsetDefault );//B0434 #939
	void					AdjustScroll_Center( const ATextPoint& inTextPoint, const ABool inRedraw = true );
	void					AdjustScroll_Top( const ATextPoint& inTextPoint );
	ABool								mDisableAdjustDiffDisplayScrollAtScrollPostProcess;//#611
	
	//フォーカス
  private:
	void					EVTDO_DoViewFocusActivated();
	void					EVTDO_DoViewFocusDeactivated();
	
	//検索
  public:
	ABool					SPI_FindNext( const AFindParameter& inParam );
	ABool					SPI_FindPrevious( const AFindParameter& inParam );
	ABool					SPI_FindFromFirst( const AFindParameter& inParam );
	ABool					SPI_FindFromFirst();//#724
	void					SPI_FindList( const AFindParameter& inParam, const ABool inExtractMatchedText );
	ABool					SPI_ReplaceNext( const AFindParameter& inParam );
	AItemCount				SPI_ReplaceAfterCaret( const AFindParameter& inParam );
	AItemCount				SPI_ReplaceInSelection( const AFindParameter& inParam, const ABool inDontShowResultNotification = false );
	AItemCount				SPI_ReplaceAll( const AFindParameter& inParam );
	ABool					SPI_ReplaceSelectedTextOnly( const AFindParameter& inParam );
	ABool					SPI_ReplaceAndFind( const AFindParameter& inParam );
	ABool					SPI_CompareFindParamAndSelectedText( const AFindParameter& inParam );
	void					SPI_FindSelectedNext();//#724
	void					SPI_FindSelectedPrevious();//#724
	void					SPI_BatchReplace( const AFindParameter& inParam, const AText& inBatchText );//#935
  private:
	ABool					FindNext( const AFindParameter& inParam );
	ABool					ReplaceNext( const AFindParameter& inParam );
	ABool					FindForward( const AFindParameter& inParam, const AIndex inStart, const AIndex inEnd );
	ABool					FindBackward( const AFindParameter& inParam, const AIndex inStart, const AIndex inEnd );
	ABool					Replace( const AFindParameter& inParam, const ATextIndex inStart, const ATextIndex inEnd, AItemCount& outReplacedCount )
	{ AItemCount offset = 0; return Replace(inParam,inStart,inEnd,outReplacedCount,offset); }
	ABool					Replace( const AFindParameter& inParam, const ATextIndex inStart, const ATextIndex inEnd, 
							AItemCount& outReplacedCount, AItemCount& outOffset );//#245
	void					ShowReplaceResultNotification( const ABool inNotAborted, const ANumber inReplacedCount,
							const AText& inFindText, const AText& inReplaceText );//#725
	void					ShowFindResultNotification( const ABool inFoundInNext, const ABool inFoundInPrev,
							const AText& inFindText );//#725
	
	//補完 #717
  public:
	void					SPI_DoCandidateFinderPaused();
	void					SPI_RequestCandidateFromParentKeyword( const ATextArray& inParentWord );
  private:
	ABool					InputAbbrev( const ABool inNext );//#1160
	ABool					GetCurrentWordForAbbrev( AIndex& outWordSpos, AIndex& outWordEpos ) const;
	void					FindCandidateRealtime();
	ABool					RequestCandidateFinder();
	void					RequestCandidateFinder( const AText& inWord, const ATextArray& inParentWord, const ATextIndex inTextIndex );
	void					RequestCandidateFinderIfHintExist();
	void					ClearAbbrevInputMode( const ABool inHidePopupAlways, const ABool inDecideCompletion );
	ABool					GetParentKeyword( const AIndex inTextIndex, AText& outParentKeyword ) const;
	/** 
	候補入力中状態フラグ 
	@note 候補入力中状態フラグ （補完キー入力を行ったらtrueになる。リアルタイム候補表示中のみの場合はfalseのまま。）
	*/
	ABool								mAbbrevInputMode;
	/**
	*/
	ABool								mAbbrevCandidateState_Requesting;
	ABool								mAbbrevCandidateState_Displayed;
	AText								mAbbrevCurrentRequestWord;
	AIndex								mAbbrevRequestInputKeyCount;
	AIndex								mAbbrevCurrentCandidateIndex;
	ATextArray							mAbbrevCandidateArray_Keyword;
	ATextArray							mAbbrevCandidateArray_InfoText;
	ATextArray							mAbbrevCandidateArray_CompletionText;
	AArray<AIndex>						mAbbrevCandidateArray_CategoryIndex;
	AArray<AScopeType>					mAbbrevCandidateArray_ScopeType;
	ATextArray							mAbbrevCandidateArray_FilePath;
	AArray<ACandidatePriority>			mAbbrevCandidateArray_Priority;
	AArray<AItemCount>					mAbbrevCandidateArray_MatchedCount;
	ATextArray							mAbbrevCandidateArray_ParentKeyword;
	AText								mLastAbbrevInput_Word;
	AText								mLastAbbrevInput_CompletionText;
	ABool								mCandidateRequestValid;
	ABool								mSuppressRequestCandidateFinder;
	
	//キーワード情報 #853
  public:
  private:
	void					DisplayIdInfo( const ABool inDisplayAlways, const AIndex inTextIndex, const AIndex inArgIndex );
	void					RequestIdInfo( const AText& inWord, const ATextArray& inParentWord, const AIndex inArgIndex,
										  const ATextPoint& inTextPoint );
	
	//現在単語マーカー #823
  private:
	void					SetCurrentWordHighlight();
	void					UpdateCurrentWordHighlight( const AText& inWord );
	
	//ワーズリスト #723
  private:
	void					RequestWordsList();
	
	//diff
  public:
	void					SPI_SetSelectByDiffTargetDocumentSelection( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint );
	
	//KeyRecord
  public:
	void					SPI_PlayKeyRecord();
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_Text;}
	
	//
  public:
	void					SPI_MoveToLine( const AIndex inLineIndex, const ABool inParagraphMode );
	void					SPI_DoJumpListSelected( const AIndex inIndex );
	void					SPI_SavePreProcess();
	void					SPI_SelectText( const AIndex inStartIndex, const AItemCount inLength, const ABool inByFind = false );
	void					SPI_SetCandidateIndex( const AIndex inIndex );//RC2
	
  public:
	void					SPI_SetBackgroundImageOffset( const ANumber inOffset ) { mBackgroundImageOffset = inOffset; }
  private:
	ANumber								mBackgroundImageOffset;
	
	//
  public:
	void					SPI_SetTextWindowID( const AWindowID inWindowID ) { mTextWindowID = inWindowID; }
	AWindowID				SPI_GetTextWindowID() const { return mTextWindowID; }//#379
  private:
	AObjectID							mTextWindowID;
	
	/*#858
	//#138
	//付箋紙
  public:
	void					SPI_DeleteFusenWindow( const AWindowID inFusenWindowID );
	void					SPI_RepositionFusenWindow( const AWindowID inFusenWindowID );
	void					SPI_EditFusenWindowContent( const AWindowID inFusenWindowID );
	void					SPI_UpdateFusen( const ABool inUpdateAll );
	void					SPI_UpdateFusenVisible();
	void					SPI_GetFusenModuleAndOffset( const APoint inFusenEditWindowPos, AText& outModuleName, ANumber& outOffset );
  private:
	void					EditOrNewFusen( const AIndex inLineIndex );
	AArray<AIndex>						mFusenArray_CommentIndex;
	AArray<AObjectID>	mFusenArray_WindowID;
	AArray<AWindowPoint>				mFusenArray_WindowOffset;
	*/
	//<内部モジュール>
	
	//Image空間処理
  public:
	
	//テキスト描画用データ取得
  public:
	AIndex					GetLineIndexByImageY( const ANumber inImageY ) const;
	ANumber					GetImageYByLineIndex( const AIndex inLineIndex ) const;
	//#450ANumber					GetLineHeightWithMargin() const;
	//#450 ANumber					GetLineHeightWithoutMargin() const;
	ANumber					GetLineHeightWithMargin( const AIndex inLineIndex ) const;//#450
	ANumber					GetLineHeightWithoutMargin( const AIndex inLineIndex ) const;//#450
	ANumber					GetLineAscent( const AIndex inLineIndex ) const;
	void					GetLineImageRect( const AIndex inLineIndex, AImageRect& outLineImageRect ) const;
	void					GetLineImageRect( const AIndex inStartLineIndex, const AIndex inEndLineIndex, AImageRect& outLineImageRect ) const;
	ANumber					SPI_GetImageHeight() const { return NVM_GetImageHeight(); }//#450
	AIndex					SPI_GetLocalYByLineIndex( const AIndex inLineIndex ) const;//#379
	void					SPI_GetImagePointFromTextPoint( const ATextPoint& inTextPoint, AImagePoint& outImagePoint )
	{ GetImagePointFromTextPoint(inTextPoint,outImagePoint); }
  private:
	void					SetLineDefaultTextProperty( const AIndex inLineIndex );//#450
	void					GetImagePointFromTextPoint( const ATextPoint& inTextPoint, AImagePoint& outImagePoint );
	void					GetTextPointFromImagePoint( const AImagePoint& inImagePoint, ATextPoint& outTextPoint );
	ANumber					GetLineHeightPlusMargin() const { return mLineHeight+mLineMargin; }
	//キャレット制御
  public:
	ABool					SPI_IsCaretMode() const {return IsCaretMode();}
	void					SPI_GetImageSelect( AImagePoint& outStartPoint, AImagePoint& outEndPoint );
	void					SPI_DoLineInfoChangeStart();
	void					SPI_DoLineInfoChangeEnd();
	void					SetOldArrowImageX( const ANumber inImageX );
	ANumber					GetOldArrowImageX() const;
	void					SPI_CorrectCaretIfInvalid();//#413
	void					SPI_RefreshCaret() { HideCaret(true,true);ShowCaret(); }//#844
  private:
	void					SetCaretTextPoint( const ATextPoint& inTextPoint );
	void					UpdateCaretImagePoint();//#450
	void					SetLastCaretLocalPoint();//#1031
	void					ShowCaret();
	void					HideCaret( const ABool inHideCrossCaretH, const ABool inHideCrossCaretV );
	void					ShowCaretReserve();//win
	void					ShowCaretIfReserved();//win
	void					BlinkCaret();
	void					DrawXorCaret( const ABool inFlush );//R0184 #1034
	ABool					IsCaretMode() const;
	void					GetCaretTextPoint( ATextPoint& outTextPoint ) const { outTextPoint = mCaretTextPoint; }
	void					GetSelectTextPoint( ATextPoint& outTextPoint ) const { outTextPoint = mSelectTextPoint; }
	void					GetSelect( AIndex& outStartIndex, AIndex& outEndIndex ) const;
	void					GetCaretDrawData( ALocalPoint& outStartPoint, ALocalPoint& outEndPoint ) const;
  private:
	const ATextPoint&		GetCaretTextPoint() const { return mCaretTextPoint; }//#231 mCaretTextPoint直接設定を検出するために、mCaretTextPointは読み込みであっても直接アクセスしないようにする
	const ATextPoint&		GetSelectTextPoint() const { return mSelectTextPoint; }//#231 mSelectTextPoint直接設定を検出するために、mSelectTextPointは読み込みであっても直接アクセスしないようにする
	void					TempHideCaret();
	void					RestoreTempHideCaret();
	ANumber								mTempCaretHideCount;
	ABool								mCaretMode;
	ATextPoint							mCaretTextPoint;
	ATextPoint							mSelectTextPoint;
	AImagePoint							mCaretImagePoint;//Image座標でのキャレット位置はmCaretTextPoint設定時(SetCaretTextPoint())に同時設定される（処理時間の問題）
	AImagePoint							mSelectImagePoint;//R0108 高速化 ルーラーから呼ばれるSPI_GetImageSelect()がけっこう時間を食っているため
	ANumber								mCaretTickCount;
	ABool								mCaretBlinkStateVisible;
	ANumber								mOldArrowImageX;
	//ABool								mOldArrowImageXValid;
	ABool								mForceSelectMode;
	ATextIndex							mSavedStartTextIndex;
	ATextIndex							mSavedEndTextIndex;
	ABool								mKeyToolMode;
	AObjectID							mKeyToolMenuObjectID_AppTool;
	AIndex								mKeyToolMenuStartIndex_AppTool;
	AIndex								mKeyToolMenuEndIndex_AppTool;
	AObjectID							mKeyToolMenuObjectID_UserTool;
	AIndex								mKeyToolMenuStartIndex_UserTool;
	AIndex								mKeyToolMenuEndIndex_UserTool;
	ALocalPoint							mLastCaretLocalPoint;//#1031
	
	//選択制御
  public:
	ABool					SPI_GetSelect( ATextPoint& outStartPoint, ATextPoint& outEndPoint ) const;
	void					SPI_SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inByFind = false );
	void					SPI_SetSelect( const AIndex inStart, const AIndex inEnd, const ABool inByFind = false );
	void					SPI_GetSelect( AArray<ATextIndex>& outStart, AArray<ATextIndex>& outEnd ) const;
	void					SPI_GetSelect( AIndex& outStartIndex, AIndex& outEndIndex ) const;//B0000
	void					SPI_GetSelectedText( AText& outText ) const;
	void					SPI_SetSelectWithParagraphIndex( 
							const AIndex inStartParagraphIndex, const AIndex inStartOffset, 
							const AIndex inEndParagraphIndex, const AIndex inEndOffset );//#699
	void					SPI_GetSelectGlobalRect( AGlobalRect& outRect );
	void					SPI_GetCurrentWordGlobalRect( AGlobalRect& outRect );
	void					SPI_InitSelect();//#699
  private:
	void					SetSelectTextPoint( const ATextPoint& inTextPoint, const ABool inByFind = false );//#262
	void					SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inByFind = false );//#262
	void					SetSelect( const AIndex inStart, const AIndex inEnd, const ABool inByFind = false );//#262
	ABool					IsInSelect( const AImagePoint& inImagePoint, const ABool inIncludeBoundary );
	AIndex								mReservedSelection_StartTextIndex;//#699
	AIndex								mReservedSelection_EndTextIndex;//#699
	ATextPoint							mReservedSelection_StartTextPoint;//#699
	ATextPoint							mReservedSelection_EndTextPoint;//#699
	ABool								mReservedSelection_IsSelectWithParagraph;//#699
	
	//
  public:
	void					SPI_EditPreProcess( const ATextIndex inTextIndex, const AItemCount inInsertedCount );
	void					SPI_EditPostProcess( const ATextIndex inTextIndex, const AItemCount inInsertedCount,
							const ABool inUpdateCaretImagePoint );//#846
  private:
	ATextIndex							mSavedStartTextIndexForEditPrePostProcess;
	ATextIndex							mSavedEndTextIndexForEditPrePostProcess;
	
	//#913
  private:
	void					SelectWholeLine();
	void					SelectWholeParagraph();
	void					SetMark();
	void					SwapMark();
	void					SelectToMark();
	ATextPoint							mMarkStartTextPoint;
	ATextPoint							mMarkEndTextPoint;
	
	//
  public:
	void					SPI_DoListViewHoverUpdated( const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex );
	
#if IMPLEMENTATION_FOR_MACOSX
	//
  public:
	void					SPI_ServicePaste( const AScrapRef inScrapRef );//B0370
#endif
	
	//矩形選択
  private:
	void					SetKukeiSelect( const AImagePoint& inImagePoint1, const AImagePoint& inImagePoint2 );
	void					SetKukeiSelect( const AArray<ATextIndex>& inStartArray, const AArray<ATextIndex>& inEndArray );//#1035
	void					GetKukeiContainRange( ATextPoint& outStart, ATextPoint& outEnd ) const;
	ABool								mKukeiSelectionMode;
	ABool								mKukeiSelected;
	AArray<ATextPoint>					mKukeiSelected_Start;
	AArray<ATextPoint>					mKukeiSelected_End;
	
	//クロスキャレット
  public:
	void					SPI_ShowHideCrossCaret();
	ABool					SPI_IsCrossCaretMode() const { return mCrossCaretMode; }
  private:
	void					XorCrossCaretH();
	void					XorCrossCaretV();
	ABool								mCrossCaretMode;
	ABool								mCrossCaretHVisible;
	ABool								mCrossCaretVVisible;
	
  private:
	ANumber								mLineHeight;
	ANumber								mLineAscent;
	ANumber								mLineMargin;
	
	//Inline Inputデータ
  private:
	AIndex								mInlineInputNotFixedTextPos;
	AItemCount							mInlineInputNotFixedTextLen;
	AArray<AIndex>						mHiliteLineStyleIndex;
	AArray<AIndex>						mHiliteStartPos;
	AArray<AIndex>						mHiliteEndPos;
	
	//印刷
  public:
  private:
	ABool					IsPrinting() {return mPrinting;}
	ABool								mPrinting;
	
	//#450 行image情報（折りたたみ・行ごと高さ対応）
  public:
	void					SPI_InitLineImageInfo();
	ANumber					SPI_UpdateLineImageInfo( const AIndex inStartLineIndex, const AItemCount inInsertedLineCount,
							const AIndex inUpdateStartLineIndex );
	ABool					SPI_IsCollapsedLine( const AIndex inLineIndex ) const;
	void					SPI_ExpandCollapse( const AIndex inLineIndex, const AExpandCollapseType inExpandCollapseType,
							const ABool inRedraw );
	ABool					SPI_GetFoldingCollapsed( const AIndex inLineIndex ) const;
	void					SPI_ExpandCollapseAtCurrentLine( const AIndex inLineIndex, const ABool inCollapse );
	void					SPI_ExpandCollapseAllAtCurrentLevel( const AIndex inLineIndex, const ABool inCollapse, const ABool inByClickFoldingMark );
	void					SPI_GetFoldingCollapsedLines( ANumberArray& outCollapsedLines );
	void					SPI_CollapseLines( const ANumberArray& inCollapsedLines, const ABool inApplyDefaultFoldingSetting,
											  const ABool inExpandOtherLines = false );
	AIndex					SPI_FindFoldingEndLineIndex( const AIndex inStartLineIndex ) const;
	AIndex					SPI_FindFoldingStartLineIndex( const AIndex inEndLineIndex ) const;
	ABool					SPI_IsFoldingStart( const AFoldingLevel inFoldingLevel ) const;
	ABool					SPI_IsFoldingEnd( const AFoldingLevel inFoldingLevel ) const;
	ABool					SPI_IsLineCollapsed( const AIndex inLineIndex ) const;
	void					SPI_RevealCollapsedLine( const AIndex inLineIndex );
	void					SPI_SetFoldingToDefault();
	void					SPI_UpdateLineImageInfoAllByCollapsedParameter() { UpdateLineImageInfoAllByCollapsedParameter(); }
  private:
	AIndex					FindParentHeader( const AIndex inLineIndex ) const;
	AIndex					FindNextHeader( const AIndex inLineIndex ) const;
	AIndex					FindPrevHeader( const AIndex inLineIndex ) const;
	AIndex					FindSameOrHigherLevelNextHeader( const AIndex inLineIndex ) const;
	AIndex					FindSameOrHigherLevelPrevHeader( const AIndex inLineIndex ) const;
	void					CollapseAllHeadersWithMNoChange();
	void					ExpandAllFoldings();
	void					CollapseAllHeaders();
	AIndex					FindCurrentFoldingStartLineIndex( const AIndex inCurrentLineIndex ) const;
	void					ExpandCollapseAllAtCurrentLevel( const AIndex inLineIndex, const ABool inCollapse );
	void					SetFoldingToDefaultCore( const ABool inExpandForNotMatched );
	void					UpdateLineImageInfoAllByCollapsedParameter();
	//#493一旦機能drop ANumber					CalcLineHeight( const AIndex inLineIndex ) const;
	AArray<ANumber>						mLineImageInfo_Height;
	AArray<ANumber>						mLineImageInfo_ImageY;
	AArray<ABool>						mLineImageInfo_Collapsed;
	
	//#478 図形モード
  private:
	void					FigMode_DrawLine( const AImagePoint& inStartImagePoint, const AImagePoint& inEndImagePoint );
	void					FigMode_DrawLine( const ANumber inStartTextX, const ANumber inEndTextX, const ANumber inTextY );
	AFigMode							mFigMode;
	
	//Notificationウインドウ
  public:
	void					SPI_ShowNotificationPopupWindow( const ABool inShow );
	AWindow_Notification&	SPI_GetPopupNotificationWindow();
	void					SPI_ShowGeneralNotificationPopupWindow( const AText& inTitle, const AText& inMessage );//#1117
  private:
	void					UpdateNotificationPopupWindowBounds( const ABool inAlwaysUpdate = false );
	AWindowID							mPopupNotificationWindowID;
	
  private:
	static const ANumber				kHScrollBarUnit = 30;
	static const ANumber				kDragDropScrollRange = 24;
	
	//Viewオブジェクト取得 #92
  public:
	static AView_Text&			GetTextViewByViewID( const AViewID inViewID )
	{
		MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_Text,kViewType_Text,inViewID);
	}
	static const AView_Text&	GetTextViewConstByViewID( const AViewID inViewID ) 
	{
		MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_VIEWID(AView_Text,kViewType_Text,inViewID);
	}
	
	//#1186
  public:
	ANumber						GetACharWidth() const { return mACharWidth; }
	
	//自動テスト用 #210
  public:
	void					SPI_KeyBindAction( const AKeyBindKey inKeyBindKey, const AModifierKeys inModiferKeys, ABool& outUpdateMenu )
	{ KeyBindAction(inKeyBindKey,inModiferKeys,keyAction_NOP,GetEmptyText(),outUpdateMenu); }
};


#pragma mark ===========================
#pragma mark [クラス]ATextViewFactory
class ATextViewFactory : public AViewFactory
{
  public:
	ATextViewFactory( /*#199 AObjectArrayItem* inParent, AWindow& inWindow,*/
			const AWindowID inWindowID, const AControlID inID, 
				/*#695 const ADocumentID inTextDocumentID*/ADocument_Text& inTextDocument, 
				const AWindowID inTextWindowID, //#379
				//#699 const ABool inCaretMode, 
				//#699 const ATextPoint& inCaretTextPoint, const ATextPoint& inSelectTextPoint, 
				const AImagePoint& inOriginOfLocalFrame,
			const ABool inCrossCaretMode )//B0345
	: /*#199 mParent(inParent), mWindow(inWindow)*/mWindowID(inWindowID), mControlID(inID), //#695mTextDocumentID(inTextDocumentID), 
	mTextDocument(inTextDocument),
			/*#699 mCaretMode(inCaretMode), mCaretTextPoint(inCaretTextPoint), mSelectTextPoint(inSelectTextPoint), */mOriginOfLocalFrame(inOriginOfLocalFrame),
			mCrossCaretMode(inCrossCaretMode)//B0345
			,mTextWindowID(inTextWindowID)//#379
	{
	}
	AView_Text*	Create() 
	{
		return new AView_Text(mWindowID,mControlID,mTextDocument/*#695 ID*/,mTextWindowID,
		/*#699 mCaretMode,mCaretTextPoint,mSelectTextPoint,*/mOriginOfLocalFrame,mCrossCaretMode);//#379
	}
  private:
	//#199 AObjectArrayItem* mParent;
	//#199 AWindow&	mWindow;
	AWindowID							mWindowID;
	AControlID							mControlID;
	//#695 ADocumentID							mTextDocumentID;
	ADocument_Text&						mTextDocument;//#695
	AWindowID							mTextWindowID;//#379
	//#699 ABool								mCaretMode;
	//#699 ATextPoint							mCaretTextPoint;
	//#699 ATextPoint							mSelectTextPoint;
	AImagePoint							mOriginOfLocalFrame;
	ABool								mCrossCaretMode;
};

