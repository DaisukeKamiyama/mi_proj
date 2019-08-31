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

AWindow

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "AApplication.h"
#include "../Imp.h"
#include "../Wrapper.h"

#include "AView.h"

//#205
enum ATableViewType
{
	kTableViewType_ListView = 0,
	kTableViewType_OSControl
};

//#205
//#688 #if IMPLEMENTATION_FOR_MACOSX
//#688 const ATableViewType kDefaultTableViewType = kTableViewType_OSControl;//Mac OS Xの場合、デフォルトはDataBrowserを使う
//#688 #else
const ATableViewType kDefaultTableViewType = kTableViewType_ListView;
//#688 #endif

class AView;
typedef AAbstractFactoryForObjectArray<AView>	AViewFactory;
class AView_List;
//#205 class AView_ListFrame;
class AView_EditBox;
class AView_Button;
class AView_VSeparator;
class AWindow_VSeparatorOverlay;
class AView_HSeparator;
class AWindow_HSeparatorOverlay;
class AWindow_ButtonOverlay;
class AView_Plain;//#634

#pragma mark ===========================
#pragma mark [クラス]AWindow
/**
各種ウインドウの基底クラス
*/
class AWindow : public AObjectArrayItem
{
	//コンストラクタ、デストラクタ
  public:
	AWindow(/*#175 AObjectArrayItem* inParent */);
	virtual ~AWindow();
	void					DoDeleted();//#138 #92
  private:
	virtual void			NVIDO_DoDeleted() {}//#92
	
	//<関連オブジェクト取得>
	/*#182
  public://暫定
	CWindowImp&				NVI_GetWindowImp() {return mWindowImp;}
	CWindowImp&				NVM_GetImp() {return mWindowImp;}
	const CWindowImp&		NVM_GetImpConst() const {return mWindowImp;}
	*/
  public:
	CWindowImp&				NVI_GetWindowImp() { return mWindowImp; }//#1034
  protected:
	CWindowImp&				GetImp() {return mWindowImp;}
	const CWindowImp&		GetImpConst() const {return mWindowImp;}
  private:
	CWindowImp							mWindowImp;
	
	
	//<イベント処理>
	
  public:
	ABool					EVT_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVT_UpdateMenu();
	void					EVT_TextInputted( const AControlID inID );
	ABool					EVT_ValueChanged( const AControlID inID );
	void					EVT_ToolbarItemAdded( const AControlID inID );//#688
	ABool					EVT_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	ABool					EVT_DoubleClicked( const AControlID inID );
	void					EVT_WindowActivated();
	void					EVT_WindowDeactivated();
	void					EVT_WindowFocusActivated();//#688
	void					EVT_WindowFocusDeactivated();//#688
	void					EVT_WindowCollapsed() {EVTDO_WindowCollapsed();}
	void					EVT_DoCloseButton();
	void					EVT_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder );
	void					EVT_FileChoosen( const AControlID inControlID, const AFileAcc& inFile );
	void					EVT_AskSaveChangesReply( const AObjectID inDocumentID, const AAskSaveChangesResult inAskSaveChangesReply );
	void					EVT_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText );
	void					EVT_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );//win 080618
	void					EVT_WindowResizeCompleted();
	void					EVT_WindowMinimizeRestored();//win
	void					EVT_WindowMinimized();//win
	void					EVT_TableViewStateChanged( const AControlID inID );
#if IMPLEMENTATION_FOR_MACOSX
	ABool					EVT_DoServiceGetTypes( AArray<AScrapType>& outCopyTypes, AArray<AScrapType>& outPasteTypes )//B0370
	{ return EVTDO_DoServiceGetTypes(outCopyTypes,outPasteTypes); }
	ABool					EVT_DoServiceCopy( const AScrapRef inScrapRef )//B0370
	{ return EVTDO_DoServiceCopy(inScrapRef); }
	ABool					EVT_DoServicePaste( const AScrapRef inScrapRef )//B0370
	{ return EVTDO_DoServicePaste(inScrapRef); }
#endif
	void					EVT_DoFileDropped( const AControlID inControlID, const AFileAcc& inFile )//R0000
							{ return EVTDO_DoFileDropped(inControlID,inFile); }
	void					EVT_DoViewFocusActivated( const AControlID inID );//#135
	void					EVT_DoViewFocusDeactivated( const AControlID inID );//#135
	ABool					EVT_ArrowKey( const AControlID inID, const AUTF16Char inChar );//#353
	//View行き
	ABool					EVT_DoInlineInput( const AControlID inID, const AText& inText, const AItemCount inFixLen,
							const AArray<AIndex>& inHiliteLineStyleIndex, 
							const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
							const AItemCount inReplaceLengthInUTF16,//#688
							ABool& outUpdateMenu );
	ABool					EVT_DoInlineInputOffsetToPos( const AControlID inID, const AIndex inStartOffset, const AIndex inEndOffset, ALocalRect& outRect  );//#1305
	ABool					EVT_DoInlineInputPosToOffset( const AControlID inID, const ALocalPoint& inPos, AIndex& outOffset );
	ABool					EVT_DoInlineInputGetSelectedText( const AControlID inID, AText& outText );
	ABool					EVT_DoTextInput( const AControlID inID, const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction, 
							ABool& outUpdateMenu );
	ABool					EVT_DoKeyDown( const AControlID inID, /*#688 const AUChar inChar*/ const AText& inText, const AModifierKeys inModifierKeys );//#1080
	ABool					EVT_DoMouseDown( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	ABool					EVT_DoContextMenu( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	ABool					EVT_DoMouseMoved( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	ABool					EVT_DoMouseMoved( const AWindowPoint& inWindowPoint, const AModifierKeys inModifierKeys );
	void					EVT_DoMouseLeft( const AControlID inID, const ALocalPoint& inLocalPoint );//win 080712
	ACursorType				EVT_GetCursorType( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVT_DoDraw( const AControlID inID );
	void					EVT_DrawPreProcess( const AControlID inID );//win 080712
	void					EVT_DrawPostProcess( const AControlID inID );//win 080712
	void					EVT_PreProcess_SetShowControl( const AControlID inID, const ABool inVisible );//#138
	void					EVT_DoMouseWheel( const AControlID inID, const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, 
							const ALocalPoint inLocalPoint );//win 080706
	void					EVT_DoScrollBarAction( const AControlID inID, const AScrollBarPart inPart );
	void					EVT_DoControlBoundsChanged( const AControlID inID );
	void					EVT_DoDragTracking( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
							ABool& outIsCopyOperation );
	ABool					EVT_DoGetHelpTag( const AControlID inID, const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide );//R0240
	void					EVT_DoDragEnter( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVT_DoDragLeave( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVT_DoDragReceive( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVT_DoMouseTracking( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVT_DoMouseTrackEnd( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVT_DoTickTimer();
	void					EVT_DoWindowBeforeSelected() { EVTDO_DoWindowBeforeSelected(); }//#240
	void					EVT_QuickLook( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#1026
  private:
	virtual ABool			EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys ) {return false;}
	virtual void			EVTDO_UpdateMenu() {}
	virtual void			EVTDO_TextInputted( const AControlID inID ) {}
	virtual ABool			EVTDO_ValueChanged( const AControlID inID ) {return false;}
	virtual ABool			EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys ) {return false;}
	virtual ABool			EVTDO_DoubleClicked( const AControlID inID ) {return false;}
	virtual void			EVTDO_WindowActivated() {}
	virtual void			EVTDO_WindowDeactivated() {}
	virtual void			EVTDO_WindowFocusActivated() {}//#688
	virtual void			EVTDO_WindowFocusDeactivated() {}//#688
	virtual void			EVTDO_WindowCollapsed() {}
	virtual void			EVTDO_DoCloseButton() {NVI_Hide();}
	virtual void			EVTDO_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder ) {}
	virtual void			EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile ) {}
	virtual void			EVTDO_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText ) {}
	virtual void			EVTDO_AskSaveChangesReply( const AObjectID inDocumentID, const AAskSaveChangesResult inAskSaveChangesReply ) {}
	virtual void			EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds ) {}
	virtual void			EVTDO_WindowResizeCompleted( const ABool inResized ) {}
	virtual void			EVTDO_TableViewStateChanged( const AControlID inID ) {}
	virtual void			EVTDO_DoScrollBarAction( const AControlID inID, const AScrollBarPart inPart ) {}
	virtual void			EVTDO_WindowMinimizeRestored() {}//win
	virtual void			EVTDO_WindowMinimized() {}//win
#if IMPLEMENTATION_FOR_MACOSX
	virtual ABool			EVTDO_DoServiceGetTypes( AArray<AScrapType>& outCopyTypes, AArray<AScrapType>& outPasteTypes ) { return false; }//B0370
	virtual ABool			EVTDO_DoServiceCopy( const AScrapRef inScrapRef ) { return false; }//B0370
	virtual ABool			EVTDO_DoServicePaste( const AScrapRef inScrapRef ) { return false; }//B0370
#endif
	virtual void			EVTDO_DoFileDropped( const AControlID inControlID, const AFileAcc& inFile ) {} //R0000
	virtual void			EVTDO_DoViewFocusActivated( const AControlID inID ) {}//#135
	virtual void			EVTDO_DoViewFocusDeactivated( const AControlID inID ) {}//#135
	virtual void			EVTDO_DoWindowBeforeSelected() {}//#240
	virtual ABool			EVTDO_DoGetHelpTag( const AControlID inID, const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) { return false; }//#602
	virtual ABool			EVTDO_DoMouseMoved( const AWindowPoint& inWindowPoint, const AModifierKeys inModifierKeys ) { return false; }
	virtual void			EVTDO_ToolbarItemAdded( const AControlID inID ) {}//#688
	virtual void			EVTDO_DoTickTimerAction() {}//
	ANumber								mCurrentWindowWidth;//#688
	ANumber								mCurrentWindowHeight;//#688
	
	//#1448
	//コンテクストメニューを開いたウインドウ、コントロールを記憶
  public:
  private:
	static AWindowID					sCurrentContextMenu_WindowID;
	static AControlID					sCurrentContextMenu_ControlID;
	
	//Redirect #390
  public:
	static void				STATIC_NVI_SetTextInputRedirect( const AWindowID inWindowID, const AControlID inControlID );
	static AWindowID		STATIC_NVI_GetTextInputRedirect_WindowID() { return sTextInputRedirect_WindowID; }
	static AControlID		STATIC_NVI_GetTextInputRedirect_ControlID() { return sTextInputRedirect_ControlID; }
  private:
	static AWindowID					sTextInputRedirect_WindowID;
	static AControlID					sTextInputRedirect_ControlID;
	
	//<インターフェイス>
	
	//ウインドウ制御
  public:
	void					NVI_Create( const ADocumentID inDocumentID );
	void					NVI_Show( const ABool inSelect = true );
	void					NVI_CreateAndShow( const ABool inSelect = true );
	void					NVI_Hide();
	void					NVI_Close();
	void					NVI_UpdateProperty();
	void					NVI_SelectWindow();
	void					NVI_SendBehind( const AWindowID inWindowID );
	void					NVI_SendAbove( const AWindowID inWindowID );//#688
	void					NVI_ReorderOverlayChildWindows();//#688
	void					NVI_DeleteControl( const AControlID inControlID );
	void					NVI_RefreshWindow();
	void					NVI_SetWindowBounds( const ARect& inBounds, const ABool inAnimate = false );//#688
	void					NVI_GetWindowBounds( ARect& outBounds ) const;
	void					NVI_SetWindowPosition( const APoint& inPoint );
	void					NVI_GetWindowPosition( APoint& outPoint ) const;
	void					NVI_SetWindowSize( const ANumber inWidth, const ANumber inHeight );
	void					NVI_SetWindowWidth( const ANumber inWidth );//#182
	void					NVI_SetWindowHeight( const ANumber inHeight );//#182
	ANumber					NVI_GetWindowWidth() const;
	ANumber					NVI_GetWindowHeight() const;
	void					NVI_CollapseWindow( const ABool inCollapse );//B0395
	ABool					NVI_IsVisibleMode() const { return mVisibleMode; }//B0404 NVI_IsWindowVisible()は実際に表示中の場合のみtrue（アプリが前面に無いときのフローティングウインドウはfalseになってしまう）
	void					NVI_TryClose( const AIndex inTabIndex = kIndex_Invalid );//#92 #164
	void					NVI_ExecuteClose();//#92
	ABool					NVI_AskSaveForAllTabs();
	void					NVI_SetInhibitClose( const ABool inInhibitClose ) { mInhibitClose = inInhibitClose; }//#92
	void					NVI_SetWindowStyleToSheet( const AWindowID inParentWindowID );//#182
	void					NVI_CreateToolbar( const AIndex inType ) { mWindowImp.CreateToolbar(inType); }
	//#688 void					NVI_SetWindowStyleToDrawer( const AWindowID inParentWindowID );//#182
	//win void					NVI_SetFloating();//#182
	void					NVI_ConstrainWindowPosition( const ABool inAllowPartial );//#182 #370
	void					NVI_ConstrainWindowPosition( const ABool inAllowPartial, APoint& ioPoint ) const;//#688
	void					NVI_ConstrainWindowSize();//#385
#if IMPLEMENTATION_FOR_WINDOWS
	void					NVI_CreateSizeBox( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight );//#182
	void					NVI_ShowFileOpenWindow( const AText& inFilter, const ABool inShowInvisibleFile );//#182
#endif
	CPaneForAView&			NVI_GetViewImp( const AControlID inControlID );//#182
	void					NVI_ShowIfNotOverlayMode();//#291
	void					NVI_HideIfNotOverlayMode();//#291
	void					NVI_UpdateWindow();//#409
	void					NVI_Zoom();//win
	void					NVI_SetFullScreenMode( const ABool inFullScreenMode );//#404
	ABool					NVI_GetFullScreenMode() const;//#404
	void					NVI_SetWindowMinimumSize( const ANumber inWidth, const ANumber inHeight );//#505
	void					NVI_SetWindowMaximumSize( const ANumber inWidth, const ANumber inHeight );//#505
	AControlID				NVI_GetMainControlID() const { return NVIDO_GetMainControlID(); }//#602
	void					NVI_UpdateHelpTag( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
											  const ABool inUpdateAlways );
   protected:
	void					NVM_SetWindowPositionDataID( ADataID inDataID ) {mWindowPositionDataID = inDataID;}
	void					NVM_CreateWindow( AConstCharPtr inWindowName, 
							const AWindowClass inWindowClass = kWindowClass_Document,//win
							const ABool inTabWindow = false, const AItemCount inTabCount = 0 );//#182
	void					NVM_CreateWindow( const AWindowClass inWindowClass = kWindowClass_Document,//#182
							const AOverlayWindowLayer inOverlayWindowLayer = kOverlayWindowLayer_None,//#688
							const ABool inHasCloseButton = true, const ABool inHasCollapseButton = true, //#219
							const ABool inHasZoomButton = true, const ABool inHasGrowBox = true,//#219
											 const ABool inIgnoreClick = false, const ABool inHasShadow = true, const ABool inHasTitlebar = true );//#379 #688
 private:
	virtual void			NVIDO_Create( const ADocumentID inDocumentID ) = 0;
	virtual void			NVIDO_Show() {}
	virtual void			NVIDO_Hide() {}
	virtual void			NVIDO_Close() {}
	virtual void			NVIDO_UpdateProperty() {}
	virtual void			NVIDO_TryClose( const AIndex inTabIndex ) {} //#92 #164
	virtual void			NVIDO_ExecuteClose() {} //#92
	virtual ABool			NVIDO_AskSaveForAllTabs() { return true; }
	virtual AControlID		NVIDO_GetMainControlID() const { return kControlID_Invalid; }//#602
	ADataID								mWindowPositionDataID;
	ABool								mVisibleMode;//B0404
	ABool								mInhibitClose;//#92
	
	//リサイズ中フラグ
  public:
	void					NVI_SetVirtualLiveResizing( const ABool inResizing ) { mVirtualLiveResizing = inResizing; }
	ABool					NVI_GetVirtualLiveResizing() const { return mVirtualLiveResizing; }
  private:
	ABool								mVirtualLiveResizing;
	
	//#850
	//初期ウインドウサイズ
  public:
	void					NVI_SetInitialWindowBounds( const ARect& inBounds ) { mInitialWindowBoundsIsSet = true; mInitialWindowBounds = inBounds; }
  protected:
	ARect								mInitialWindowBounds;
	ABool								mInitialWindowBoundsIsSet;
	
	//タブ制御
  public:
	void					NVI_CreateTab( const ADocumentID inDocumentID, const ABool inSelectableTab = true );//#379
	void					NVI_DeleteTabAndView( const AIndex inTabIndex );
	void					NVI_SelectTab( const AIndex inTabIndex/*B0415, const ABool inForRedraw = false*/ );
	void					NVI_SetTabZOrder( const AIndex inTabIndex, const AIndex inZOrder );
	AIndex					NVI_GetTabZOrder( const AIndex inTabIndex ) const;
	void					NVI_SelectTabByDocumentID( const ADocumentID inDocumentID );
	void					NVI_SelectTabByIncludingControlID( const AControlID inControlID );//#92
	AItemCount				NVI_GetTabCount() const {return mTabDataArray_ControlIDs.GetItemCount();}
	AItemCount				NVI_GetSelectableTabCount() const { return mTabZOrder.GetItemCount(); }//#379
	AIndex					NVI_GetCurrentTabIndex() const {return mCurrentTabIndex;}
	ADocumentID				NVI_GetDocumentIDByTabIndex( const AIndex inTabIndex ) const;
	AIndex					NVI_GetTabIndexByDocumentID( const AObjectID inDocumentID ) const;
	AIndex					NVI_GetMostBackTabIndex() const;
	AIndex					NVI_GetTabIndexByZOrderIndex( const AIndex inZOrderIndex ) const;
	ABool					NVI_GetTabSelectable( const AIndex inTabIndex ) const;//#379
	ABool					NVI_GetNextDisplayTab( AIndex& ioIndex, const ABool inLoop ) const { return NVIDO_GetNextDisplayTab(ioIndex,inLoop); }//#559
	ABool					NVI_GetPrevDisplayTab( AIndex& ioIndex, const ABool inLoop ) const { return NVIDO_GetPrevDisplayTab(ioIndex,inLoop); }//#559
	AIndex					NVI_GetFirstDisplayTab() const;//#559
	AIndex					NVI_GetLastDisplayTab() const;//#559
  protected:
	void					NVM_RegisterViewInTab( const AIndex inTabIndex, const AControlID inControlID );
	void					NVM_UnregisterViewInTab( const AIndex inTabIndex, const AControlID inControlID );
	void					NVI_SetLatentVisible( const AIndex inTabIndex, const AControlID inControlID, const ABool inLatentVisible );//#212
	void					NVM_DoTabActivated( const AIndex inTabIndex, const ABool inTabSwitched );//win 080712
	void					NVM_DoTabDeactivated( const AIndex inTabIndex );
	void					NVM_DoTabActivating( const AIndex inTabIndex );//win
	void					NVM_SetTabDocumentID( const AIndex inTabIndex, const ADocumentID inDocID )
	{ mTabDataArray_DocumentID.Set(inTabIndex,inDocID); }//#688
  private:
	virtual void			NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID ) { outInitialFocusControlID = kControlID_Invalid; };
	virtual void			NVIDO_DeleteTab( const AIndex inTabIndex ) {}
	virtual void			NVIDO_TabDeleted() {}//#663
	virtual void			NVIDO_SelectTab( const AIndex inTabIndex ) {}//#725
	AObjectArray< AArray<AControlID> >	mTabDataArray_ControlIDs;
	AArray<ADocumentID>					mTabDataArray_DocumentID;
	AArray<AControlID>					mTabDataArray_LatentFocus;
	AObjectArray< AArray<ABool> >		mTabDataArray_LatentVisible;
	AArray<AIndex>						mTabDataArray_mWindowMenuShortcut;
	ABoolArray							mTabDataArray_TabSelectable;//#379
	AIndex								mCurrentTabIndex;
	AArray<AIndex>						mTabZOrder;
	virtual void			NVMDO_DoTabActivating( const AIndex inTabIndex ) {}//win
	virtual void			NVMDO_DoTabActivated( const AIndex inTabIndex, const ABool inTabSwitched ) {}//win 080712
	virtual void			NVMDO_DoTabDeactivated( const AIndex inTabIndex ) {}
	virtual ABool			NVIDO_GetNextDisplayTab( AIndex& ioIndex, const ABool inLoop ) const;//#559
	virtual ABool			NVIDO_GetPrevDisplayTab( AIndex& ioIndex, const ABool inLoop ) const;//#559
	
	//ウインドウ情報設定／取得
  public:
	AWindowType				NVI_GetWindowType() const { return NVIDO_GetWindowType(); }
	ABool					NVI_IsWindowVisible() const;
	ABool					NVI_IsWindowCreated() const;
	ABool					NVI_IsWindowActive() const;
	ABool					NVI_IsWindowFocusActive() const;//#688
	ABool					NVI_HasCloseButton() const;
	ABool					NVI_HasTitleBar() const;
	ABool					NVI_IsFloating() const;
	ABool					NVI_IsModal() const;
	ABool					NVI_IsResizable() const;
	ABool					NVI_IsZoomable() const;
	ABool					NVI_IsZoomed() const;
	ABool					NVI_IsDocumentWindow() const;//#673
	void					NVI_GetWindowBoundsInStandardState( ARect& outBounds ) const;//win
	AWindowRef				NVI_GetWindowRef() const;
	ADocumentID				NVI_GetCurrentDocumentID() const;
	ABool					NVI_IsWindowCollapsed() const;//B0395
	ABool					NVI_IsPrintMode() const;//#182
	void					NVI_AddToRotateArray();//#175
	void					NVI_AddToTimerActionWindowArray();
	void					NVI_AddToAllCloseTargetWindowArray();//#199
	void					NVI_SetWindowTransparency( const AFloatNumber inTransparency );//#182
	void					NVI_SetModified( const ABool inModified );//#182
	void					NVI_SetInputBackslashByYenKeyMode( const ABool inMode );//#182
	void					NVI_SelectTabControl( const AIndex inIndex );//#182
	void					NVI_PaintRect( const AWindowRect& inRect, const AColor& inColor );//win
	void					NVI_SetCloseByEscapeKey( const ABool inCloseEscapeKey ) { mWindowImp.SetCloseByEscapeKey(inCloseEscapeKey); }//#986
  private:
	virtual AWindowType		NVIDO_GetWindowType() const { return kWindowType_Invalid; }
	virtual ABool			NVIDO_IsDocumentWindow() const { return false; }//#688
	
	//Control制御
  public:
	void					NVI_SetShowControl( const AControlID inControlID, const ABool inShow );
	void					NVI_SetShowControlIfCurrentTab( const AIndex inTabIndex, const AControlID inControlID, const ABool inShow );//#246
	void					NVI_SetControlPosition( const AControlID inControlID, const AWindowPoint& inPoint );//#246
	void					NVI_SetControlPositionIfCurrentTab( const AIndex inTabIndex, const AControlID inControlID, const AWindowPoint& inPoint );//#246
	void					NVI_SetControlSize( const AControlID inControlID, const ANumber inWidth, const ANumber inHeight );//#246
	void					NVI_SetControlSizeIfCurrentTab( const AIndex inTabIndex, const AControlID inControlID, const ANumber inWidth, const ANumber inHeight );//#246
	void					NVI_SetControlSelectAll( const AControlID inControlID );//#135
	void					NVI_SetControlEnable( const AControlID inControlID, const ABool inEnable );//#182
	void					NVI_SetCatchReturn( const AControlID inControlID, const ABool inCatch );//#182
	void					NVI_SetCatchTab( const AControlID inControlID, const ABool inCatch );//#182
	void					NVI_GetControlPosition( const AControlID inID, AWindowPoint& outPoint ) const;//#182
	ANumber					NVI_GetControlWidth( const AControlID inID ) const;//#182
	ANumber					NVI_GetControlHeight( const AControlID inID ) const;//#182
	ABool					NVI_IsControlVisible( const AControlID inControlID ) const;//#182
	ABool					NVI_IsControlEnable( const AControlID inControlID ) const;//#182
	AIndex					NVI_GetControlEmbeddedTabControlIndex( const AControlID inID ) const;//#182
	void					NVI_SetAutomaticSelectBySwitchFocus( const AControlID inControlID, const ABool inSelect );//#182
	void					NVI_SetControlIcon( const AControlID inControlID, const AIconID inIconID,
							const ABool inDefaultSize = false, const ANumber inLeftOffset = 4, const ANumber inTopOffset = 4 );//#182
	void					NVI_SetMenuItemsFromTextArray( const AControlID inID, const ATextArray& inTextArray );//#182
	void					NVI_InsertMenuItem( const AControlID inID, const AIndex inMenuItemIndex, const AText& inText );//win
	void					NVI_DeleteMenuItem( const AControlID inID, const AIndex inMenuItemIndex );//win
	void					NVI_DrawControl( const AControlID inID );//#182
	void					NVI_EmbedControl( const AControlID inParent, const AControlID inChild );//#182
	void					NVI_SetMinMax( const AControlID inControlID, const ANumber inMin, const ANumber inMax );//#182
	void					NVI_RegisterFontMenu( const AControlID inID, const ABool inEnableDefaultFontItem = false );//#182 #375
	void					NVI_RegisterTextArrayMenu( const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID );//#182
	void					NVI_UnregisterTextArrayMenu( const AControlID inControlID );//#182
	void					NVI_SetHelpTag( const AControlID inID, const AText& inText, const AHelpTagSide inTagSide = kHelpTagSide_Default );//#182 #634
	void					NVI_EnableHelpTagCallback( const AControlID inID, const ABool inShowOnlyWhenNarrow = true );//#182 #507
	void					NVI_HideHelpTag() const;//#182
	void					NVI_ClearControlUndoInfo( const AControlID inID );//#182
	void					NVI_SetControlZOrderToTop( const AControlID inID );//#291
	void					NVI_SetControlBindings( const AControlID inControlID,
							const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
							const ABool inWidthFixed, const ABool inHeightFixed );//#291 #688
	void					NVI_DisableEventCatch( const AControlID inControlID );//win
	void					NVI_EnableFileDrop( const AControlID inControlID );//#565
	ABool					NVI_ExistControl( const AControlID inControlID ) const
	{ return GetImpConst().ExistControl(inControlID); }//#688
	void					NVI_SetScrollBarPageUnit( const AControlID inControlID, const ANumber inUnit )
	{ GetImp().SetScrollBarPageUnit(inControlID,inUnit); }//#725
	void					NVI_SetControlMenu( const AControlID inControlID, const AMenuRef inMenuRef )
	{ GetImp().SetControlMenu(inControlID,inMenuRef); }//#725
	void					NVI_ShowControlFontPanel( const AControlID inControlID )
	{ GetImp().ShowControlFontPanel(inControlID); }//#688
	void					NVI_AutomaticSelectBySwitchFocus( const AControlID inControlID );
  private:
	ABool					IsView( const AControlID inID ) const { return (mViewControlIDArray.Find(inID) != kIndex_Invalid); }//#182
	AArray<AControlID>					mToolTipTextArray_ControlID;//#688
	ATextArray							mToolTipTextArray_ToolTipText;//#688
	AArray<AHelpTagSide>				mToolTipTextArray_HelpTagSide;//#688
	
	//サイズボックスターゲットウインドウ
  public:
	void					NVI_SetSizeBoxTargetWindowID( const AWindowID inWindowID ) { mSizeBoxTargetWindowID = inWindowID; }
	AWindowID				NVI_GetSizeBoxTargetWindowID() const { return mSizeBoxTargetWindowID; }
  private:
	AWindowID							mSizeBoxTargetWindowID;
	
	//座標変換 #182
  public:
	void					NVI_GetWindowPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AWindowPoint& inWindowPoint ) const;
	void					NVI_GetWindowRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AWindowRect& outWindowRect ) const;
	void					NVI_GetControlLocalPointFromWindowPoint( const AControlID inID, const AWindowPoint& inWindowPoint, ALocalPoint& outLocalPoint ) const;
	void					NVI_GetControlLocalRectFromWindowRect( const AControlID inID, const AWindowRect& inWindowRect, ALocalRect& outLocalRect ) const;
	void					NVI_GetGlobalPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AGlobalPoint& outGlobalPoint ) const;
	void					NVI_GetGlobalRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AGlobalRect& outGlobalRect ) const;
	
	//#182
  public:
	void					NVI_DragWindow( const AControlID inControlID, const ALocalPoint& inMouseLocalPoint );
	
	//StickyWindow制御 #138
  public:
	void					NVI_SetAsStickyWindow( const AWindowID inParent );//子側からコール
	void					NVI_RegisterChildStickyWindow( const AWindowID inChild );
	void					NVI_UnregisterChildStickyWindow( const AWindowID inChild );
	void					NVI_SetChildStickyWindowOffset( const AWindowID inChild, const AWindowPoint& inOffset );
  private:
	void					UpdateStickyWindowOffset();
	AArray<AWindowID>					mChildStickyWindowArray_WindowID;
	AArray<AWindowPoint>				mChildStickyWindowArray_Offset;
	AWindowID							mParentWindowIDForStickyWindow;
	
	//オーバーレイウインドウ制御 #291
  public:
	void					NVI_ChangeToOverlay( const AWindowID inParentWindowID, const ABool inTransparent );//win
	void					NVI_ChangeToFloating();
	void					NVI_SetOffsetOfOverlayWindow( const AWindowID inParentWindowID, const AWindowPoint& inOffset );
	void					NVI_SetOffsetOfOverlayWindowAndSize( const AWindowID inParentWindowID, 
							const AWindowPoint& inOffset, const ANumber inWidth, const ANumber inHeight, const ABool inAnimate = false );
	void					NVI_SendOverlayWindowBehind( const AWindowID inWindowID );//#320
	void					NVI_SendOverlayWindowAbove( const AWindowID inWindowID );//#688
	ABool					NVI_GetOverlayMode() const { return mOverlayMode; }
	void					NVI_SelectWindowForSpacesProblemCountermeasure();//#422
	AWindowID				NVI_GetOverlayParentWindowID() const { return mOverlayParentWindowID; }//#725
  protected:
	ABool					NVM_GetOverlayMode() const { return mOverlayMode; }
  private:
	void					SetOverlayMode( const ABool inOverlayMode );
	ABool								mOverlayMode;
	AWindowID							mOverlayParentWindowID;
	ABool								mOverlayTransparent;
	AWindowPoint						mOverlayCurrentOffset;//#379
	ANumber								mOverlayCurrentWidth;//#379
	ANumber								mOverlayCurrentHeight;//#379
	
	//子オーバーレイウインドウ管理 #725
  public:
	void					NVM_RegisterOverlayWindow( const AWindowID inWindowID, const AIndex inTabIndex, const ABool inLatentVisible );
	void					NVM_UnregisterOverlayWindow( const AWindowID inWindowID );
	void					NVI_SetOverlayWindowVisibility( const AWindowID inWindowID, const ABool inShow );
  private:
	AArray<AWindowID>					mChildOverlayWindowArray_WindowID;
	AArray<AIndex>						mChildOverlayWindowArray_TabIndex;
	AArray<ABool>						mChildOverlayWindowArray_LatentVisible;
	
	//ウインドウタイトル設定／取得
  public:
	void					NVI_SetDefaultTitle( const AText& inText );
	void					NVI_GetTitle( const AIndex inTabIndex, AText& outTitle ) const;
	void					NVI_GetCurrentTitle( AText& outTitle ) const;
	void					NVI_UpdateTitleBar() { NVIDO_UpdateTitleBar(); }//#699
	void					NVI_SetFixTitle( const ABool inFix ) { mFixTitle = inFix; }
	void					NVI_SetWindowFile( const AFileAcc& inFile )
	{ GetImp().SetWindowFile(inFile); }
  private:
	virtual void			NVIDO_UpdateTitleBar();//#699
	AText								mWindowDefaultTitle;
	ABool								mFixTitle;//win 080726
	
	//ウインドウショートカット
  public:
	AIndex					NVI_GetWindowMenuShortcut( const AIndex inTabIndex ) const {return mTabDataArray_mWindowMenuShortcut.Get(inTabIndex);}
	void					NVI_SetWindowMenuShortcut( const AIndex inTabIndex, const AIndex inWindowMenuShortcut ) { mTabDataArray_mWindowMenuShortcut.Set(inTabIndex,inWindowMenuShortcut); }
	
	//共通ショートカットキー用設定
  public:
	void					NVI_SetDefaultCancelButton( const AControlID inControlID );//#135
	void					NVI_SetDefaultOKButton( const AControlID inControlID );//#135
  private:
	AControlID							mDefaultCancelButton;//#135
	AControlID							mDefaultOKButton;//#135
	
	
	//<所有クラス(AView)生成／削除／取得>
  public:
	/*#271 AViewID					NVI_CreateView( AViewFactory& ioFactory, 
							const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight, const ABool inSupportFocus, 
							const AIndex inTabControlIndex = kIndex_Invalid, const ABool inFront = true );//#92*/
	//win void					NVI_DeleteViewByViewID( const AViewID inViewID );//#92
	
	AView&					NVI_GetViewByControlID( const AControlID inControlID );
	const AView&			NVI_GetViewConstByControlID( const AControlID inControlID ) const;
	ABool					NVI_ExistView( const AControlID inControlID ) const { return IsView(inControlID); }//win 080618
	AViewID					NVI_CreateView( const AControlID inControlID, const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight, 
							const AControlID inParentControlID, const AControlID inSiblingControlID,//#688
							const ABool inSupportFocus, AViewFactory& inFactory, const AIndex inTabIndex = kIndex_Invalid, const ABool inFront = true )
	{ return NVM_CreateView(inControlID,inPosition,inWidth,inHeight,inParentControlID,inSiblingControlID,inSupportFocus,inFactory,inTabIndex,inFront); }//#138
	/*win void					NVI_DeleteView( const AControlID inControlID )
	{ NVM_DeleteView(inControlID); }*/
	AControlID				NVM_AssignDynamicControlID();
  protected:
	AViewID					NVM_CreateView( const AControlID inControlID, const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight, //#199
							const AControlID inParentControlID, const AControlID inSiblingControlID,//#688
							const ABool inSupportFocus, AViewFactory& inFactory, const AIndex inTabControlIndex = kIndex_Invalid, const ABool inFront = true );//win 080618
	AViewID					NVM_CreateView( const AControlID inControlID, const ABool inSupportFocus, AViewFactory& inFactory );//#199
	//win void					NVM_DeleteView( const AControlID inControlID );
	//#243 動的ControlIDは10000～固定にするvoid					NVM_SetDynamicControlIDRange( const AControlID inStart, const AControlID inEnd );
  private:
	void					DeleteView( const AControlID inControlID );//win
	//#199 AObjectArray<AView>					mViewArray;
	AArray<AViewID>						mViewIDArray;//#199
	AHashArray<AControlID>				mViewControlIDArray;//#645 高速化
	AControlID							mNextDynamicControlID;
	AControlID							mDynamicControlIDRangeStart;
	AControlID							mDynamicControlIDRangeEnd;
	static const AControlID				kDefaultDynamicControlIDRangeStart = 10000;//#199
	ABool					ExistView( const AControlID inControlID );
	
	//フォーカス制御
  public:
	void					NVI_RegisterToFocusGroup( const AControlID inFocusControlID, 
							const ABool inOnly2ndPattern = false, const ABool inGroup = false );//#135 #353
	void					NVI_UnregisterFromFocusGroup( const AControlID inFocusControlID );//#135
	void					NVI_SwitchFocusTo( const AControlID inFocusControlID, const ABool inReverseSearch = false );
	void					NVI_SwitchFocusToFirst();
	AControlID				NVI_GetCurrentFocus() const;
	AControlID				NVI_GetFocusInTab( const AIndex inTabIndex ) const;
	AViewID					NVI_GetCurrentFocusViewID() const;//#567
	void					NVI_SwitchFocusToNext();//#135
	void					NVI_SwitchFocusToPrev();//#135
	AControlID				NVI_GetLatentFocus() const { return mLatentFocus; }//#312
	ABool					NVI_IsFocusInTab( const AControlID inControlID ) const;//#312
	const AArray<AControlID>&	GetFocusGroup() const //#353
	{
		/*#688
		if( sIsFocusGroup2ndPattern == true )
		{
			return mFocusGroup2nd;
		}
		else
		*/
		{
			return mFocusGroup;
		}
	}
	const ABoolArray&	GetFocusGroup_IsGroup() const //#353
	{
		/*#688
		if( sIsFocusGroup2ndPattern == true )
		{
			return mFocusGroup2nd_IsGroup;
		}
		else
		*/
		{
			return mFocusGroup_IsGroup;
		}
	}
	ABool					NVI_GetControlSupportFocus( const AControlID inControlID ) const;//#360
  private:
	void					UpdateFocusInTab( const AControlID inControlID );//#135
	//#135 virtual void			NVIDO_SwitchFocusTo( const AControlID inFocusControlID ) {}
	//#135 AControlID							mCurrentFocus;
	AArray<AControlID>					mFocusGroup;//#135
	ABoolArray							mFocusGroup_IsGroup;//#353
	//#688 AArray<AControlID>					mFocusGroup2nd;//#353
	//#688 ABoolArray							mFocusGroup2nd_IsGroup;//#353
	AControlID							mLatentFocus;//#135
	//#688 static ABool						sIsFocusGroup2ndPattern;//#353
	AControlID				GetNextFocusControlID( const AControlID inControlID ) const;//#353
	AControlID				GetPreviousFocusControlID( const AControlID inControlID ) const;//#353
	ABool					IsFocusableControl( const AControlID inControlID ) const;//#353
	
	//各種Control/Menu等生成／登録 #182
  public:
	/*#688
	void					NVI_RegisterTextEditPane( const AControlID inID, const ABool inWrapMode = false , 
	const ABool inVScrollbar = false, const ABool inHScrollBar = false, const ABool inHasFrame = true );
	*/
	void					NVI_RegisterColorPickerPane( const AControlID inID );
	void					NVI_RegisterHelpAnchor( const AControlID inID, AConstCstringPtr inAnchor );//#539
	void					NVI_RegisterRadioGroup();
	void					NVI_AddToLastRegisteredRadioGroup( const AControlID inID );
	void					NVI_CreateScrollBar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex = kIndex_Invalid );//#182
	void					NVI_RegisterScrollBar( const AControlID inID );
	void					NVI_CreateRoundedRectButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex = kIndex_Invalid )
	{ GetImp().CreateRoundedRectButton(inID,inPoint,inWidth,inHeight,inTabIndex); }//#688
	void					NVI_CreateProgressIndicator( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex = kIndex_Invalid )
	{ GetImp().CreateProgressIndicator(inID,inPoint,inWidth,inHeight,inTabIndex); }//#688
  private:
	void					UpdateRadioGroup( const AControlID inControlID );//#349
	AObjectArray< AArray<AControlID> >	mRadioGroup;//#349
	AArray<AControlID>					mRadioGroup_All;//#349 検索用
	AHashArray<AControlID>				mHelpAnchorArray_ControlID;//#539
	ATextArray							mHelpAnchorArray_Anchor;//#539
	
	//WebView #734
  public:
	void					NVI_CreateWebView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight );
	void					NVI_LoadURLToWebView( const AControlID inID, const AText& inURL );
	void					NVI_ReloadWebView( const AControlID inID );
	void					NVI_GetCurrentWebViewURL( const AControlID inID, AText& outURL ) const;
	void					NVI_ExecuteJavaScriptInWebView( const AControlID inID, const AText& inText );
	void					NVI_SetWebViewFontMultiplier( const AControlID inID, const AFloatNumber inMultiplier );
	
	//ファイル／フォルダ選択
  public:
	void					NVI_ShowChooseFolderWindow( const AFileAcc& inDefaultFolder, const AText& inMessage,
							const AControlID inVirtualControlID, const ABool inSheet );//#182 #551
	void					NVI_ShowChooseFileWindow( const AFileAcc& inDefaultFile, const AText& inMessage,
							const AControlID inVirtualControlID, const ABool inOnlyApp = false, 
							const ABool inSheet = true );//#182
	void					NVI_ShowChooseFileWindowWithFileFilter( const AFileAcc& inDefaultFile, const AControlID inVirtualControlID, const ABool inSheet,
							const AFileAttribute inFilterAttribute );//#182
	void					NVI_ShowSaveWindow( const AText& inFileName, const ADocumentID inDocumentID, 
							const AFileAttribute inFileAttr, const AText& inFilter, const AText& inRefText, const AFileAcc& inDefaultFolder );//#182
  private:
	ABool								mAskSaveChanges_IsModal;
	AAskSaveChangesResult				mAskSaveChanges_Result;
	
	//子ウインドウ
  public:
	void					NVI_ShowChildWindow_OK( const AText& inMessage1, const AText& inMessage2, const ABool inLongDialog = false );//#182
	void					NVI_CloseChildWindow_OK();//#427
	void					NVI_SetChildWindow_OK_EnableOKButton( const ABool inEnable );//#427
	void					NVI_SetChildWindow_OK_SetText( const AText& inMessage1, const AText& inMessage2 );//#427
	void					NVI_DoChildWindowClosed_OK();//#291
	void					NVI_ShowChildWindow_OKCancel( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
							const AControlID inOKButtonVirtualControlID );//#182
	void					NVI_DoChildWindowClosed_OKCancel();//#291
	void					NVI_ShowChildWindow_OKCancelDiscard( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
							const AControlID inOKButtonVirtualControlID, const AControlID inDiscardButtonVirtualControlID );//#182
	void					NVI_DoChildWindowClosed_OKCancelDiscard();//#291
	AAskSaveChangesResult	NVI_ShowAskSaveChangesWindow( const AText& inFileName, const AObjectID inDocumentID,
														  const ABool inModal = false );//#182
	void					NVI_DoChildWindowClosed_SaveCancelDiscard();//#1034
	void					NVI_CloseChildWindow();//#182
	ABool					NVI_IsChildWindowVisible() const;//B0411
	//#291 ABool					NVI_ChildWindowVisible() const;//#182
	void					NVI_RegisterChildWindow( const AWindowID inChildWindowID );//#694
	void					NVI_UnregisterChildWindow( const AWindowID inChildWindowID );//#694
  private:
	virtual ABool			NVIDO_IsChildWindowVisible() const { return false; }
	AWindowID							mOKWindow;//#291
	AWindowID							mOKCancelWindow;//#291
	AWindowID							mOKCancelDiscardWindow;//#291
	AWindowID							mSaveCancelDiscardWindow;//#1034
	AArray<AObjectID>					mChildWindowIDArray;//#694
	
	//<所有クラス(AView)インターフェイス>
	
	//処理通知コールバック
  public:
	/*#1031
	void					OWICB_ViewScrolled( const AControlID inControlID, const ANumber inDeltaX, const ANumber inDeltaY,
											  const ABool inRedraw, const ABool inConstrainToImageSize );
	*/
	void					OWICB_SelectionChanged( const AControlID inControlID );
	void					OWICB_ListViewRowMoved( const AControlID inControlID, const AIndex inOldIndex, const AIndex inNewIndex );
	//{ NVIDO_ListViewRowMoved(inControlID,inOldIndex,inNewIndex); }
	void					OWICB_ListViewFileDropped( const AControlID inControlID, const AIndex inRowIndex, const AFileAcc& inFile )
	{ NVIDO_ListViewFileDropped(inControlID,inRowIndex,inFile); }
  private:
	/*#1031
	virtual void			NVIDO_ViewScrolled( const AControlID inControlID, const ANumber inDeltaX, const ANumber inDeltaY,
											   const ABool inRedraw, const ABool inConstrainToImageSize ) {}
	*/
	virtual void			NVIDO_SelectionChanged( const AControlID inControlID ) {}
	virtual void			NVIDO_ListViewRowMoved( const AControlID inControlID, const AIndex inOldIndex, const AIndex inNewIndex ) {}
	virtual void			NVIDO_ListViewFileDropped( const AControlID inControlID, const AIndex inRowIndex, const AFileAcc& inFile ) {}
	//View内でのキー押下共通処理
  public:
	void					OWICB_ViewReturnKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );//#137 #135
	void					OWICB_ViewTabKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );//#137 #135
	void					OWICB_ViewEscapeKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );//#137 #135
	ABool					OWICB_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );//#798
	ABool					OWICB_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers );//#798
	//#892
	void					OWICB_ListViewExpandedCollapsed( const AControlID inControlID );
	//
	void					OWICB_ButtonViewDragCanceled( const AControlID inControlID );
	void					OWICB_ListViewHoverUpdated( const AControlID inControlID, 
													   const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex );
  private:
	virtual ABool			NVIDO_ViewReturnKeyPressed( const AControlID inControlID ) { return false; }//#137 #135
	virtual ABool			NVIDO_ViewTabKeyPressed( const AControlID inControlID ) { return false; }//#137 #135
	virtual ABool			NVIDO_ViewEscapeKeyPressed( const AControlID inControlID ) { return false; }//#137 #135
	virtual ABool			NVIDO_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers ) { return false; }//#798
	virtual ABool			NVIDO_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers ) { return false; }//#798
	virtual void			NVIDO_ListViewExpandedCollapsed( const AControlID inControlID ) {}//#892
	//
	virtual void			NVIDO_ButtonViewDragCanceled( const AControlID inControlID ) {}
	virtual void			NVIDO_ListViewHoverUpdated( const AControlID inControlID, 
													   const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex ) {}
	
	//コントロールの値設定／取得 #135 #182
  public:
	void					NVI_SetControlText( const AControlID inControlID, const AText& inText );
	void					NVI_AddControlText( const AControlID inControlID, const AText& inText );//#182
	void					NVI_GetControlText( const AControlID inControlID, AText& outText ) const;
	void					NVI_SetControlNumber( const AControlID inControlID, const ANumber inNumber );
	ANumber					NVI_GetControlNumber( const AControlID inControlID ) const;
	void					NVI_GetControlNumber( const AControlID inControlID, ANumber& outNumber ) const;
	void					NVI_SetControlBool( const AControlID inControlID, const ABool inBool );
	void					NVI_GetControlBool( const AControlID inControlID, ABool& outBool ) const;
	ABool					NVI_GetControlBool( const AControlID inControlID ) const;
	void					NVI_SetControlColor( const AControlID inControlID, const AColor& inColor );
	void					NVI_GetControlColor( const AControlID inControlID, AColor& outColor ) const;
	void					NVI_SetControlFloatNumber( const AControlID inControlID, const AFloatNumber inNumber );
	void					NVI_GetControlFloatNumber( const AControlID inControlID, AFloatNumber& outNumber ) const;
	void					NVI_SetControlTextFont( const AControlID inControlID, const AText& inFontName, const AFloatNumber inFontSize );
	void					NVI_SetProgressIndicatorProgress( const AIndex inProgressIndex, const ANumber inProgress, const ABool inShow = true );//#688
	//void					NVI_SetMouseTrackingRect( const AControlID inControlID, const AArray<ALocalRect>& inLocalRectArray );//#688
	void					NVI_SetControlFont( const AControlID inID, const AText& inFontName, const AFloatNumber inFontSize );//#688
	void					NVI_GetControlFont( const AControlID inID, AText& outFontName, AFloatNumber& outFontSize ) const;//#688
	
	//インライン入力
  public:
	void					NVI_FixInlineInput( const AControlID inControlID );
	ABool					NVI_GetInlineInputMode() { return mWindowImp.GetInlineInputMode(); }//#448
	
	//ユーティリティ
  public:
	static void				NVI_GetCurrentMouseLocation( AGlobalPoint& outGlobalPoint )//#725
	{ CWindowImp::GetCurrentMouseLocation(outGlobalPoint); }
	//<汎化メソッド>
	
	//コントロール状態の更新
  protected:
	void					NVM_UpdateControlStatus();
  private:
	virtual void			NVMDO_UpdateControlStatus() {};
	
	//連動DBの取得
  protected:
	ADataBase&				NVM_GetDB() { return NVMDO_GetDB(); }
  private:
	virtual ADataBase&		NVMDO_GetDB() { _ACError("No DB",this); return GetDummyDB();}
	
	//DB連動データの登録
  protected:
	void					NVM_RegisterDBData( const ADataID inDataID, const ABool inNotifyDataChanged, 
												const AControlID inEnablerControlID = kControlID_Invalid, const AControlID inDisablerControlID = kControlID_Invalid,
												const ABool inReverseBool = false );
	void					NVM_RegisterDBData_( const AControlID inControlID, const ADataID inDataID, const ABool inNotifyDataChanged, 
												 const AControlID inEnablerControlID, const AControlID inDisablerControlID,
												 const ABool inReverseBool );
	
	//データタイプ設定（DB連動コントロール用） #182
  protected:
	void					NVM_SetControlDataType( const AControlID inControlID, const ADataType inDataType );
	
	//テーブル選択
  public:
	void					NVM_SelectTableRow( const AControlID inControlID, const AIndex inTableRowDBIndex );
	ADataID					NVI_GetTableViewSelectionColumnID( const AControlID inTableControlID ) const;//#205
	AIndex					NVI_GetTableViewSelectionDBIndex( const AControlID inTableControlID ) const;//#205
	void					SPI_GetTableViewSelectionDBIndexArray( const AControlID inTableControlID, AArray<AIndex>& outDBIndexArray ) const;//#625
  private:
	void					SetTableViewSelectionByDBIndex( const AControlID inControlID, const AIndex inTableRowDBIndex );//#205
	
	//テーブル編集 #205
  public:
	void					NVI_StartTableColumnEditMode( const AControlID inTableControlID, const ADataID inColumnID );
	
	//DB連動テーブルデータの登録
  public:
	void					NVM_RegisterDBTable( const AControlID inTableControlID, const ADataID inFirstDataID,
							const AControlID inAddRowControlID, const AControlID inDeleteRowControlID, const AControlID inDeleteRowOKControlID,
							const AControlID inUpID, const AControlID inDownID, const AControlID inTopID, const AControlID inBottomID,
							const AControlID inContentGroupControlID, const ABool inNotifyDataChanged,
							const ABool inFrame = false, const ABool inHeader = false, 
							const ABool inVScrollBar = false, const ABool inHScrollBar = false ,//win 080618
							const ATableViewType inTableViewType = kDefaultTableViewType );
	void					NVM_RegisterDBTableColumn( const AControlID inTableControlID, const ADataID inColumnID, const ADataID inDataID,
							const AControlID inEditButtonControlID, const ANumber inColumnWidth, const AConstCharPtr inTitle, const ABool inEditable );//win 080618
	void					NVM_RegisterDBTableColumn( const AControlID inTableControlID, const ADataID inColumnID, const ADataType inDataType,
							const AControlID inEditButtonControlID, const ANumber inColumnWidth, const AConstCharPtr inTitle, const ABool inEditable );//win 080618
  private:
	ATableViewType			GetTableViewType( const AControlID inTableControlID ) const;
	
	//DB連動テーブルView表示データの設定 #205
  public:
	void					NVI_BeginSetTable( const AControlID inTableControlID );
	void					NVI_SetTableColumn_Bool( const AControlID inTableControlID, const ADataID inColumnID, const ABoolArray& inArray );
	void					NVI_SetTableColumn_Number( const AControlID inTableControlID, const ADataID inColumnID, const ANumberArray& inArray );
	void					NVI_SetTableColumn_Text( const AControlID inTableControlID, const ADataID inColumnID, const ATextArray& inArray );
	void					NVI_SetTableColumn_Color( const AControlID inTableControlID, const ADataID inColumnID, const AColorArray& inArray );
	//#1012 void					NVI_SetTableColumn_Icon( const AControlID inTableControlID, const ADataID inColumnID, const ANumberArray& inArray );
	void					NVI_EndSetTable( const AControlID inTableControlID );
	void					NVM_SetInhibit0LengthForTable( const AControlID inTableControlID, const ABool inInhibit0Length );//#205
	void					NVM_SetInhibit0LengthForTable( const AControlID inTableControlID, const ABool inInhibit0Length, const AText& in0LengthText );//#1428
	
	//DB連動テーブルView表示データの取得 #205
  public:
	const ABoolArray&		NVI_GetTableColumn_Bool( const AControlID inTableControlID, const ADataID inColumnID );
	const ANumberArray&		NVI_GetTableColumn_Number( const AControlID inTableControlID, const ADataID inColumnID );
	const ATextArray&		NVI_GetTableColumn_Text( const AControlID inTableControlID, const ADataID inColumnID );
	const AColorArray&		NVI_GetTableColumn_Color( const AControlID inTableControlID, const ADataID inColumnID );
	
	//DB連動テーブルView列の幅設定／取得
  public:
	void					NVI_SetTableColumnWidth( const AControlID inTableControlID, const ADataID inColumnID, const ANumber inWidth );
	ANumber					NVI_GetTableColumnWidth( const AControlID inTableControlID, const ADataID inColumnID ) const;
	
	//ContentView管理
  public:
	void					NVI_SaveContentView( const AControlID inTableControlID ) { NVM_SaveContentView(inTableControlID); }
  protected:
	void					NVM_RegisterDBTableContentView( const AControlID inTableControlID, const AControlID inContentControlID, const ADataID inDataID,
															const ABool inReverseBool = false );//#1032
	void					NVM_RegisterDBTableSpecializedContentView( const AControlID inTableControlID, const AControlID inContentControlID );//B0406
	void					NVM_SaveContentView( const AControlID inTableControlID );//B0406 旧名称NVM_SetDBTableDataFromContentView()からメソッド名変更
  private:
	void					OpenContentView( const AControlID inTableControlID );
	//B0406 virtual void			NVMDO_SetDBTableDataFromContentView( const AControlID inTableControlID, const AControlID inContentControlID, const AIndex inRowIndex ) {}
	virtual void			NVMDO_SaveSpecializedContentView( const AControlID inTableControlID, const AIndex inRowIndex ) {}//B0406 旧NVMDO_SetDBTableDataFromContentViewに相当
	virtual void			NVMDO_OpenSpecializedContentView( const AControlID inTableControlID, const AIndex inRowIndex ) {}//B0406 
	AObjectArray<AHashNumberArray>		mDBTable_ContentIDArray;
	AObjectArray<AHashNumberArray>		mDBTable_ContentDataIDArray;
	AObjectArray<ABoolArray>			mDBTable_ContentReverseBool;//#1032
	AObjectArray<AHashNumberArray>		mDBTable_SpecializedContentIDArray;//B0406
	AArray<AIndex>						mDBTable_CurrentContentViewRowIndex;//B0406
	AArray<AObjectID>					mDBTable_CurrentContentViewDBID;//B0406
	
	//DB連動テーブル個別Update
  protected:
	void					NVM_UpdateDBTableView( const AControlID inTableControlID );
	
	//DB連動データが変更されたことを派生クラスへ通知するためのメソッド
  private:
	virtual void			NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
							const AControlID inTriggerControlID/*B0406*/ ) {}
	virtual void			NVMDO_NotifyDataChangedForSave() {}//#1239
	
	//<内部モジュール>
	
	//DB連動データあり？
  private:
	ABool					IsDBDataTableExist();
	
	//DB連動データのUpdate
  private:
	void					UpdateDBDataAndTable();
	void					UpdateDBControlStatus();
  private:
	virtual void			NVMDO_UpdateTableView( const AControlID inTableControlID, const ADataID inColumnID ) {}
	//B0406 virtual void			NVMDO_UpdateTableContentView( const AControlID inTableControlID, const AControlID inContentControlID ) {}
	
	//DB連動データのクリック時処理
  private:
	ABool					DoClicked_DBTable( const AControlID inControlID );
	
	//DB連動データの値変更時処理
  public:
	void					NVI_DoValueChanged( const AControlID inControlID );
  private:
	ABool					DoValueChanged_DBData( const AControlID inControlID );
	ABool					DoValueChanged_DBTable( const AControlID inTableControlID );
	ABool					DoValueChanged_DBTableContentView( const AControlID inControlID );
	
	/*#625 NVMDO_NotifyDataChanged()と役割重複のため、こちらは削除
  public:
	void					NVI_AddTableRow( const AControlID inControlID ) { NVIDO_AddTableRow(inControlID); }
	void					NVI_RemoveTableRow( const AControlID inControlID, const AIndex inRowIndex ) {NVIDO_RemoveTableRow(inControlID,inRowIndex);}
	void					NVI_MoveTableRow( const AControlID inControlID, const AIndex inDeleteItemIndex, const AIndex inNewItemIndex ) 
							{NVIDO_MoveTableRow(inControlID,inDeleteItemIndex,inNewItemIndex);}//R0183
  private:
	virtual void			NVIDO_AddTableRow( const AControlID inControlID ) {}
	virtual void			NVIDO_RemoveTableRow( const AControlID inControlID, const AIndex inRowIndex ) {}
	virtual void			NVIDO_MoveTableRow( const AControlID inControlID, const AIndex inDeleteItemIndex, const AIndex inNewItemIndex ) {}//R0183
	*/
	//DB連動データ
  private:
	AHashArray<AControlID>				mDBData_ControlID;
	AHashArray<ADataID>					mDBData_DataID;
	AArray<ABool>						mDBData_NotifyDataChanged;
	AHashArray<AControlID>				mDBData_EnablerControlID;
	AHashArray<AControlID>				mDBData_DisablerControlID;
	AArray<ABool>						mDBData_ReverseBool;
	
	AHashArray<AControlID>				mDBTable_ControlID;
	AArray<ADataID>						mDBTable_FirstDataID;
	AHashArray<AControlID>				mDBTable_AddRow_ControlID;
	AHashArray<AControlID>				mDBTable_DeleteRow_ControlID;
	AHashArray<AControlID>				mDBTable_DeleteRowOK_ControlID;
	AHashArray<AControlID>				mDBTable_Up_ControlID;
	AHashArray<AControlID>				mDBTable_Down_ControlID;
	AHashArray<AControlID>				mDBTable_Top_ControlID;
	AHashArray<AControlID>				mDBTable_Bottom_ControlID;
	AObjectArray<ANumberArray>			mDBTable_ColumnIDArray;
	AObjectArray<ANumberArray>			mDBTable_DataIDArray;
	AObjectArray< AArray</*#216 AObjectID*/AControlID> >	mDBTable_ColumnEditButtonIDArray;
	AHashArray<AControlID>				mDBTable_ContentGroupControlID;
	AArray<ABool>						mDBTable_NotifyDataChanged;
	AArray<AIndex>						mDBTable_CurrentSelect;
	AArray<ATableViewType>				mDBTable_ListViewType;//#205
	
  public:
	void					NVM_StoreToDBAll();
	
	//ListView
  public:
	//#205 void					NVI_CreateListView( const AControlID inListViewControlID, const AControlID inListViewFrameControlID,
	//#205 						const AControlID inScrollBarControlID );
	//#205 void					NVI_CreateListView( const AControlID inListViewControlID, const ABool inFrame, const ABool inHeader, 
	//#205						const ABool inVScrollBar, const ABool inHScrollBar );//win 080618
	void					NVI_CreateListViewFromResource( const AControlID inListViewControlID );//#205
	AView_List&				NVI_GetListView( const AControlID inListViewControlID );
	const AView_List&		NVI_GetListViewConst( const AControlID inListViewControlID ) const;
	//#205 AView_ListFrame&		NVI_GetListFrameView( const AControlID inListHeaderViewControlID );//win 080618
  private:
	//#205 AArray<AControlID>					mListViewControlIDArray;
	//#205 AArray<AControlID>					mListFrameViewControlIDArray;
	
	//EditView  win 080618
  public:
	AViewID					NVI_CreateEditBoxView( const AControlID inEditViewControlID, 
							const ABool inHasVScrollbar = false, const ABool inHasHScrollBar = false , const ABool inHasFrame = true, const AEditBoxType inEditBoxType = kEditBoxType_Normal );
	AViewID					NVI_CreateEditBoxView( const AControlID inEditViewControlID, 
							const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight, 
							const AControlID inParentControlID,//#688
							const AIndex inTabControlIndex, const ABool inFront,
							const ABool inHasVScrollbar = false, const ABool inHasHScrollBar = false , const ABool inHasFrame = true, const AEditBoxType inEditBoxType = kEditBoxType_Normal );//#182
	void					NVI_DeleteEditBoxView( const AControlID inEditViewControlID );
	AView_EditBox&			NVI_GetEditBoxView( const AControlID inEditViewControlID );
	const AView_EditBox&	NVI_GetEditBoxViewConst( const AControlID inEditViewControlID ) const;
  private:
	//#199 AArray<AControlID>					mEditBoxViewControlIDArray;
	
	//ButtonView #232
  public:
	AViewID					NVI_CreateButtonView( const AControlID inControlID, 
							const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight,
							const AControlID inParentControlID );//#688
	AViewID					NVI_CreateButtonView( const AControlID inControlID );//#427
	AView_Button&			NVI_GetButtonViewByControlID( const AControlID inButtonControlID );
	void					NVI_SetLastClickedButtonControlID( const AControlID inLastClickedButtonControlID );
	AControlID				NVI_GetLastClickedButtonControlID() const;
  private:
	AControlID							mLastClickedButtonControlID;
	
	//PlainView #634
  public:
	AViewID					NVI_CreatePlainView( const AControlID inControlID, const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight );
	AView_Plain&			NVI_GetPlainViewByControlID( const AControlID inControlID );
	void					NVI_SetPlainViewColor( const AControlID inControlID, const AColor inColor, const AColor inColorDeactive, 
												  const ABool inLeftBottomRounded, const ABool inRightBottomRounded,
												  const ABool inForLeftSideBarSeparator, const ABool inForRightSideBarSeparator, const AColor inSeparatorLineColor );
	
	//Separator #291
  public:
	AWindow_VSeparatorOverlay&	NVI_GetVSeparatorWindow( const AWindowID inSeparatorOverlayWindow );
	AWindow_HSeparatorOverlay&	NVI_GetHSeparatorWindow( const AWindowID inSeparatorOverlayWindow );
	void					NVI_CreateVSeparatorView( const AControlID inControlID, const ASeparatorLinePosition inSeparatorLinePosition,
							const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight );
	AView_VSeparator&		NVI_GetVSeparatorViewByControlID( const AControlID inControlID );
	void					NVI_CreateHSeparatorView( const AControlID inControlID, const ASeparatorLinePosition inSeparatorLinePosition,
							const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight );
	AView_HSeparator&		NVI_GetHSeparatorViewByControlID( const AControlID inControlID );
	ANumber					NVI_SeparatorMoved( const AWindowID inTargetWindowID, const AControlID inID, 
							const ANumber inDelta, const ABool inCompleted );//#409
	void					NVI_SeparatorDoubleClicked( const AWindowID inTargetWindowID, const AControlID inID );
  private:
	virtual ANumber			NVIDO_SeparatorMoved( const AWindowID inSeparatorWindowID, const AControlID inID,
												 const ANumber inDelta, const ABool inCompleted ) { return inDelta; }//#409
	virtual void			NVIDO_SeparatorDoubleClicked( const AWindowID inTargetWindowID, const AControlID inID ) {}
	
	//ButtonOverlayWindow win
  public:
	AWindow_ButtonOverlay&	NVI_GetButtonWindow( const AWindowID inButtonOverlayWindow );
	
	//#846
	//modal処理
  public:
	void					NVI_StartModalSession();
	ABool					NVI_CheckContinueingModalSession();
	void					NVI_EndModalSession();
	void					NVI_StopModalSession();
	void					NVI_RunModalWindow();
	static ABool			NVI_IsInModal() { return sInModal; }//#946
  private:
	static ABool						sInModal;//#946
	
	//#872
	//ウインドウサイズ
  public:
	void					NVI_OptimizeWindowBounds( const AControlID inBottomMarker, const AControlID inRightMarker )
	{ GetImp().OptimizeWindowBounds(inBottomMarker,inRightMarker); }
	void					NVI_GetAvailableWindowBoundsInSameScreen( AGlobalRect& outRect )
	{ GetImp().GetAvailableWindowBoundsInSameScreen(outRect); }
	//#725
  public:
	static void				NVI_GetAvailableWindowBoundsInMainScreen( AGlobalRect& outRect ) 
	{ CWindowImp::GetAvailableWindowBoundsInMainScreen(outRect); }
	
	//#688
	//ツールバー
  public:
	void					NVI_ShowHideToolbar()
	{ GetImp().ShowHideToolbar(); }
	void					NVI_ShowToolbarCustomize()
	{ GetImp().ShowToolbarCustomize(); }
	ABool					NVI_IsToolBarShown()
	{ return GetImpConst().IsToolBarShown(); }
	
	//#688
  public:
	ABool					NVI_IsFullScreen() const
	{ return GetImpConst().IsFullScreen(); }
	
	//#688
	//フォントパネル
  public:
	void					NVI_ShowFontPanel( const AControlID inVirtualControlID, const AText& inFontName, const AFloatNumber inFontSize )
	{ GetImp().ShowFontPanel(inVirtualControlID,inFontName,inFontSize); }
	
	//staticデータ初期化 #353
  public:
	static void				STATIC_Init();
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AWindow"; }
	
};

typedef AAbstractFactoryForObjectArray<AWindow>	AWindowFactory;

#pragma mark ===========================
#pragma mark [クラス]AWindowDefaultFactory
template<class T> class AWindowDefaultFactory : public AWindowFactory
{
  public:
	AWindowDefaultFactory<T>()
	{
	}
	T*	Create() 
	{
		return new T();
	}
};

#pragma mark ===========================
#pragma mark [クラス]AChildWindowFactory
template<class T> class AChildWindowFactory : public AWindowFactory
{
  public:
	AChildWindowFactory<T>( const AWindowID inParentWindowID ) : mParentWindowID(inParentWindowID)
	{
	}
	T*	Create() 
	{
		return new T(mParentWindowID);
	}
  private:
	AWindowID							mParentWindowID;
};


