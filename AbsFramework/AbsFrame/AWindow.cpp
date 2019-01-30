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
各種ウインドウの基底クラス

#175: AWindowオブジェクトには全てObjectIDを割り当てるように修正

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow
#pragma mark ===========================
//ウインドウ基底クラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow::AWindow(/*#175 AObjectArrayItem* inParent */) : AObjectArrayItem(/*#175 inParent*/&(AApplication::GetApplication())), mWindowImp(NULL,*this), mCurrentTabIndex(kIndex_Invalid), 
		mDynamicControlIDRangeStart(/*win kControlID_Invalid*//*10000*/kDefaultDynamicControlIDRangeStart), 
		mDynamicControlIDRangeEnd(/*win 080618 kControlID_Invalid*/kNumber_MaxNumber), 
		mNextDynamicControlID(/*win 080618 kControlID_Invalid*//*10000*/kDefaultDynamicControlIDRangeStart),
		/*#135 mCurrentFocus(kControlID_Invalid),*/ mVisibleMode(false), mFixTitle(false)//win 080726
		,mParentWindowIDForStickyWindow(kObjectID_Invalid)
		,mInhibitClose(false)//#92
		,mLastClickedButtonControlID(kControlID_Invalid)//#232
		,mDefaultCancelButton(kControlID_Invalid)//#135
		,mDefaultOKButton(kControlID_Invalid)//#135
		,mLatentFocus(kControlID_Invalid)//#135
		,mOverlayMode(false), mOverlayParentWindowID(kObjectID_Invalid)//#291
		,mOKWindow(kObjectID_Invalid), mOKCancelWindow(kObjectID_Invalid), mOKCancelDiscardWindow(kObjectID_Invalid)//#291
		,mOverlayCurrentOffset(kWindowPoint_00),mOverlayCurrentWidth(0),mOverlayCurrentHeight(0)//#379
,mInitialWindowBoundsIsSet(false),mInitialWindowBounds(kRect_0000)//#850
,mCurrentWindowWidth(0),mCurrentWindowHeight(0)//#688
,mSizeBoxTargetWindowID(kObjectID_Invalid)
,mVirtualLiveResizing(false)
,mAskSaveChanges_IsModal(false),mAskSaveChanges_Result(kAskSaveChangesResult_None)
,mSaveCancelDiscardWindow(kObjectID_Invalid)//#1034
{
	mWindowPositionDataID = kDataID_Invalid;
}
//デストラクタ
AWindow::~AWindow()
{
	/*#92 DoDeleted()へ移動
	NVI_Close();
	//#175
	//巡回ウインドウから削除
	AApplication::GetApplication().NVI_DeleteFromWindowRotateArray(GetObjectID());
	*/
}

//#138
/**
AObjectArrayItemをAObjectArrayから削除した直後にコールされる。
（デストラクタはGC時なので、タイミング制御できない。よって、各種削除処理はDoDeleted()で行うべき。）
*/
void	AWindow::DoDeleted()
{
	//#138 StickyWindowの場合、親ウインドウのウインドウグループから、自分を削除する
	if( mParentWindowIDForStickyWindow != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mParentWindowIDForStickyWindow).NVI_UnregisterChildStickyWindow(GetObjectID());
		mParentWindowIDForStickyWindow = kObjectID_Invalid;
	}
	//タイマー対象ウインドウから削除
	AApplication::GetApplication().NVI_DeleteFromTimerActionWindowArray(GetObjectID());
	//AllClose対象ウインドウから削除 #199
	AApplication::GetApplication().NVI_DeleteFromAllCloseTargetWindowArray(GetObjectID());
	//ウインドウをHideする
	NVI_Hide();
	//#92 デストラクタから移動
	//
	NVI_Close();
	//#175
	//巡回ウインドウから削除
	AApplication::GetApplication().NVI_DeleteFromWindowRotateArray(GetObjectID());
	//#92
	NVIDO_DoDeleted();
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//メニュー選択時のコールバック（処理を行ったらtrueを返す）
ABool	AWindow::EVT_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
				EVT_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys);
	}
	//
	//現在のfocus viewがAViewである場合は、AViewで実行を試みる
	AControlID	focusViewID = NVI_GetCurrentFocus();
	if( focusViewID != kControlID_Invalid )
	{
		if( IsView(focusViewID) == true )
		{
			if( NVI_GetViewByControlID(focusViewID).NVI_IsFocusActive() == true )//#725
			{
				if( NVI_GetViewByControlID(focusViewID).EVT_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys) == true )
				{
					return true;
				}
			}
		}
	}
	//サブクラスで実行を試みる
	if( EVTDO_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys) == true )
	{
		return true;
	}
	if( inMenuItemID == kMenuItemID_Close )
	{
		NVI_Hide();
		return true;
	}
	
	//#725
	//オーバーレイウインドウの場合、親ウインドウでの実行を試みる
	if( NVI_GetOverlayParentWindowID() != kObjectID_Invalid )
	{
		if( AApplication::GetApplication().NVI_GetWindowByID(NVI_GetOverlayParentWindowID()).
					EVT_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys) == true )
		{
			return true;
		}
	}
	return false;
}

//メニュー項目のアップデート要求時のコールバック
void	AWindow::EVT_UpdateMenu()
{
	//#688 終了処理中にUpdateMenuを実行しないようにする
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return;
	
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).EVT_UpdateMenu();
		return;
	}
	//
	//現在のfocus viewがAViewである場合は、AViewでupdate
	AControlID	focusViewID = NVI_GetCurrentFocus();
	if( focusViewID != kControlID_Invalid )
	{
		if( IsView(focusViewID) == true )
		{
			if( NVI_GetViewByControlID(focusViewID).NVI_IsFocusActive() == true )//#725
			{
				NVI_GetViewByControlID(focusViewID).EVT_UpdateMenu();
			}
		}
	}
	//
	if( NVI_HasCloseButton() == true && NVI_IsChildWindowVisible() == false/*B0411*/ )
	{
		AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,true);
	}
	//サブクラスでのupdate
	EVTDO_UpdateMenu();
	
	//#725
	//オーバーレイウインドウの場合、親ウインドウでもupdate
	if( NVI_GetOverlayParentWindowID() != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(NVI_GetOverlayParentWindowID()).EVT_UpdateMenu();
	}
}

//テキスト入力時のコールバック
void AWindow::EVT_TextInputted( const AControlID inID )
{
	EVTDO_TextInputted(inID);
}

//コントロールのクリック時のコールバック
ABool	AWindow::EVT_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	//ラジオグループ更新
	UpdateRadioGroup(inID);
	
	//#539
	AIndex	helpAnchorIndex = mHelpAnchorArray_ControlID.Find(inID);
	if( helpAnchorIndex != kIndex_Invalid )
	{
		AFileAcc	helpFolder;
		AFileWrapper::GetHelpFolder("ja",helpFolder);
		AText	url;
		helpFolder.GetURL(url);
		if( url.GetItemCount() > 0 )
		{
			if( url.Get(url.GetItemCount()-1) != '/' )
			{
				url.Add('/');
			}
			url.AddText(mHelpAnchorArray_Anchor.GetTextConst(helpAnchorIndex));
			ALaunchWrapper::OpenURLWithDefaultBrowser(url);
		}
		return true;
	}
	//
	ABool	result = false;
	if( EVTDO_Clicked(inID,inModifierKeys) == true )   result = true;
	
	//EVTDO_Clicked内でウインドウを閉じた場合
	if( NVI_IsWindowCreated() == false )   return true;
	
	if( result == false )
	{
		//DB連動処理
		if( IsDBDataTableExist() == true )
		{
			if( DoClicked_DBTable(inID) == true )   result = true;
		}
	}
	
	//#349 ラジオグループ更新
	//#791 上へ移動 UpdateRadioGroup(inID);
	
	//DBデータ保存処理 #1239
	NVMDO_NotifyDataChangedForSave();
	
	//
	NVM_UpdateControlStatus();
	return result;
}

//コントロールのダブルクリック時のコールバック
ABool	AWindow::EVT_DoubleClicked( const AControlID inID )
{
	return EVTDO_DoubleClicked(inID);
}

//コントロール値変更通知時のコールバック（処理を行ったらtrueを返す）
ABool	AWindow::EVT_ValueChanged( const AControlID inID )
{
	ABool	result = false;
	if( EVTDO_ValueChanged(inID) == true )   result = true;
	
	if( result == false )
	{
		//DB連動処理
		if( IsDBDataTableExist() == true )
		{
			if( DoValueChanged_DBData(inID) == true )   result = true;
			if( DoValueChanged_DBTable(inID) == true )   result = true;
			if( DoValueChanged_DBTableContentView(inID) == true )   result = true;
		}
	}
	
	//DBデータ保存処理 #1239
	NVMDO_NotifyDataChangedForSave();
	
	NVM_UpdateControlStatus();
	return result;
}

//#688
/**
ツールバー項目追加／削除時処理（削除時はinID=kControlID_Invalid)
*/
void	AWindow::EVT_ToolbarItemAdded( const AControlID inID )
{
	EVTDO_ToolbarItemAdded(inID);
}

//WindowActivated時のコールバック
void	AWindow::EVT_WindowActivated()
{
	EVTDO_WindowActivated();
	NVM_UpdateControlStatus();
	//Tab Activated
	NVM_DoTabActivated(mCurrentTabIndex,false);//win 080712
	//ウインドウ内の各種Controlの表示を更新する #137
	//#725 処理高速化のため、ここでrefreshはしない。NVI_RefreshWindow();
	//#175
	//ウインドウ巡回順を更新する
	AApplication::GetApplication().NVI_RotateWindowActivated(GetObjectID());
	//アプリケーション共通のWindowアクティベート時処理実行 #959
	AApplication::GetApplication().EVT_WindowActivated();
}

//WindowDeactivated時のコールバック
void	AWindow::EVT_WindowDeactivated()
{
	EVTDO_WindowDeactivated();
	//Tab Deactivated
	if( mCurrentTabIndex != kIndex_Invalid )//win
	{
		NVM_DoTabDeactivated(mCurrentTabIndex);
	}
	//ウインドウ内の各種Controlの表示を更新する #137
	//#725 処理高速化のため、ここでrefreshはしない。NVI_RefreshWindow();
}

//#688
/**
ウインドウフォーカスactivate時処理
*/
void	AWindow::EVT_WindowFocusActivated()
{
	//派生クラス内処理
	EVTDO_WindowFocusActivated();
	//現在のフォーカスviewでのフォーカスactivated処理
	if( mLatentFocus != kControlID_Invalid )
	{
		EVT_DoViewFocusActivated(mLatentFocus);
	}
}

//#688
/**
ウインドウフォーカスdeactivate時処理
*/
void	AWindow::EVT_WindowFocusDeactivated()
{
	//派生クラス内処理
	EVTDO_WindowFocusDeactivated();
	//現在のフォーカスviewでのフォーカスdeactivated処理
	if( mLatentFocus != kControlID_Invalid )
	{
		EVT_DoViewFocusDeactivated(mLatentFocus);
	}
}

//クローズボタンクリック時の動作
void	AWindow::EVT_DoCloseButton()
{
	EVTDO_DoCloseButton();
	
	//DBデータ保存処理 #1239
	NVMDO_NotifyDataChangedForSave();
}

//フォルダ選択ダイアログでフォルダが選択されたときのコールバック
void	AWindow::EVT_FolderChoosen( const AControlID inControlID, const AFileAcc& inFolder )
{
	EVTDO_FolderChoosen(inControlID,inFolder);
	
	//DBデータ保存処理 #1239
	NVMDO_NotifyDataChangedForSave();
}

//ファイル選択ダイアログでファイルが選択されたときのコールバック
void	AWindow::EVT_FileChoosen( const AControlID inControlID, const AFileAcc& inFile )
{
	EVTDO_FileChoosen(inControlID,inFile);
	
	//DBデータ保存処理 #1239
	NVMDO_NotifyDataChangedForSave();
}

//ファイル選択ダイアログでファイルが選択されたときのコールバック
void	AWindow::EVT_AskSaveChangesReply( const AObjectID inDocumentID, const AAskSaveChangesResult inAskSaveChangesReply )
{
	if( mAskSaveChanges_IsModal == true )
	{
		//モーダルダイアログで実行した場合はメンバ変数に記憶
		mAskSaveChanges_Result = inAskSaveChangesReply;
	}
	else
	{
		//シートで実行した場合は派生クラスでの処理実行
		EVTDO_AskSaveChangesReply(inDocumentID,inAskSaveChangesReply);
	}
}

//保存ダイアログでファイルが選択されたときのコールバック
void	AWindow::EVT_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText )
{
	EVTDO_SaveWindowReply(inDocumentID,inFile,inRefText);
}

//ウインドウサイズ変更通知時のコールバック
void	AWindow::EVT_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )//win 080618
{
	if( NVI_IsWindowCreated() == false )   return;
	if( NVI_GetTabCount() == 0 )   return;//win Windowsの場合タブ生成前にWM_SIZEが来る
	
	EVTDO_WindowBoundsChanged(inPrevBounds,inCurrentBounds);
	//win 080618
	//Listフレームやスクロールバーの位置を、ListViewの位置に合わせて更新する（USE_IMP_TABLEVIEWの場合は実際には処理無し）
	/*#205 
	for( AIndex i = 0; i < mListFrameViewControlIDArray.GetItemCount(); i++ )
	{
		NVI_GetListFrameView(mListFrameViewControlIDArray.Get(i)).SPI_UpdateBounds();
	}
	*/
	//#138 StickyWindow位置更新
	UpdateStickyWindowOffset();
}

//ウインドウサイズ変更通知時のコールバック
void	AWindow::EVT_WindowResizeCompleted()
{
	if( NVI_IsWindowCreated() == false )   return;
	if( NVI_GetTabCount() == 0 )   return;//win Windowsの場合タブ生成前にWM_SIZEが来る
	
	//#688
	//前回のサイズと同じかどうかの判定
	ARect	bounds = {0};
	NVI_GetWindowBounds(bounds);
	ABool	resized = ( mCurrentWindowWidth != bounds.right-bounds.left || mCurrentWindowHeight != bounds.bottom-bounds.top);
	//派生クラスでの処理
	EVTDO_WindowResizeCompleted(resized);
	//今回のサイズを記憶
	mCurrentWindowWidth = bounds.right-bounds.left;
	mCurrentWindowHeight = bounds.bottom-bounds.top;
}

//win
/**
ウインドウMinimize時のコールバック
*/
void	AWindow::EVT_WindowMinimized()
{
	EVTDO_WindowMinimized();
}

//win
/**
ウインドウMinimize解除時のコールバック
*/
void	AWindow::EVT_WindowMinimizeRestored()
{
	EVTDO_WindowMinimizeRestored();
}

//TableViewのコラム幅の変更やSort順の変更などの状態変更時のコールバック
void	AWindow::EVT_TableViewStateChanged( const AControlID inID )
{
	EVTDO_TableViewStateChanged(inID);
}

//#135
/**
FocusがActivateされたときにコールされる
*/
void	AWindow::EVT_DoViewFocusActivated( const AControlID inID )
{
	//#312
	mLatentFocus = inID;
	//タブ内のアクティブフォーカスを更新
	UpdateFocusInTab(inID);
	//派生クラスでの処理
	EVTDO_DoViewFocusActivated(inID);
	//Viewでの処理
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoViewFocusActivated();
}

//#135
/**
FocusがDeactivateされたときにコールされる
*/
void	AWindow::EVT_DoViewFocusDeactivated( const AControlID inID )
{
	//派生クラスでの処理
	EVTDO_DoViewFocusDeactivated(inID);
	//Viewでの処理
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoViewFocusDeactivated();
}

//#353
/**
カーソルキー処理
*/
ABool	AWindow::EVT_ArrowKey( const AControlID inID, const AUTF16Char inChar )
{
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	const ABoolArray&	focusGroup_IsGroup = GetFocusGroup_IsGroup();
	
	AIndex	focusIndex = focusGroup.Find(inID);
	if( focusIndex != kIndex_Invalid )
	{
		if( focusGroup_IsGroup.Get(focusIndex) == true )
		{
			switch(inChar)
			{
			  case 0x001C://left
			  case 0x001E://up
				{
					for( AIndex	index = focusIndex-1; index >= 0; index-- )
					{
						if( focusGroup_IsGroup.Get(index) == false )   break;
						
						AControlID	controlID = focusGroup.Get(index);
						if( IsFocusableControl(controlID) == true )
						{
							NVI_SwitchFocusTo(controlID);
							NVI_SetControlBool(controlID,true);
							return true;
							break;
						}
					}
					break;
				}
			  case 0x001D://right
			  case 0x001F://down
				{
					for( AIndex	index = focusIndex+1; index < focusGroup.GetItemCount(); index++ )
					{
						if( focusGroup_IsGroup.Get(index) == false )   break;
						
						AControlID	controlID = focusGroup.Get(index);
						if( IsFocusableControl(controlID) == true )
						{
							NVI_SwitchFocusTo(controlID);
							NVI_SetControlBool(controlID,true);
							return true;
							break;
						}
					}
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
		}
	}
	return false;
}

#pragma mark ---View行き

//Inline Input
ABool	AWindow::EVT_DoInlineInput( const AControlID inID, const AText& inText, const AItemCount inFixLen, 
		const AArray<AIndex>& inHiliteLineStyleIndex, const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
		const AItemCount inReplaceLengthInUTF16,//#688
		ABool& outUpdateMenu )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
		EVT_DoInlineInput(sTextInputRedirect_ControlID,inText,
			inFixLen,inHiliteLineStyleIndex,inHiliteStartPos,inHiliteEndPos,inReplaceLengthInUTF16,outUpdateMenu);
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).
	EVT_DoInlineInput(inText,inFixLen,inHiliteLineStyleIndex,inHiliteStartPos,inHiliteEndPos,inReplaceLengthInUTF16,outUpdateMenu);
}

ABool	AWindow::EVT_DoInlineInputOffsetToPos( const AControlID inID, const AIndex inStartOffset, const AIndex inEndOffset, ALocalRect& outRect )//#1305
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
		EVT_DoInlineInputOffsetToPos(sTextInputRedirect_ControlID,inStartOffset,inEndOffset,outRect);//#1305
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoInlineInputOffsetToPos(inStartOffset,inEndOffset,outRect);//#1305
}

ABool	AWindow::EVT_DoInlineInputPosToOffset( const AControlID inID, const ALocalPoint& inPos, AIndex& outOffset )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
				EVT_DoInlineInputPosToOffset(sTextInputRedirect_ControlID,inPos,outOffset);
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoInlineInputPosToOffset(inPos,outOffset);
}

ABool	AWindow::EVT_DoInlineInputGetSelectedText( const AControlID inID, AText& outText )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
				EVT_DoInlineInputGetSelectedText(sTextInputRedirect_ControlID,outText);
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoInlineInputGetSelectedText(outText);
}

//テキスト入力
ABool	AWindow::EVT_DoTextInput( const AControlID inID, const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction, 
		ABool& outUpdateMenu )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
				EVT_DoTextInput(sTextInputRedirect_ControlID,inText,/*#688 inOSKeyEvent*/inKeyBindKey,inModifierKeys,inKeyBindAction,outUpdateMenu);
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoTextInput(inText,/*#688 inOSKeyEvent*/inKeyBindKey,inModifierKeys,inKeyBindAction,outUpdateMenu);
}

//キー入力(raw)
ABool	AWindow::EVT_DoKeyDown( const AControlID inID, /*#688 const AUChar inChar*/ const AText& inText, const AModifierKeys inModifierKeys )//#1080
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).
				EVT_DoKeyDown(sTextInputRedirect_ControlID,/*#688 inChar*/inText,inModifierKeys);
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoKeyDown(/*#688 inChar*/inText,inModifierKeys);
}

//マウスボタン押下時のコールバック(AView用)
ABool	AWindow::EVT_DoMouseDown( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//#390
	if( sTextInputRedirect_WindowID != kObjectID_Invalid && sTextInputRedirect_WindowID != GetObjectID() )
	{
		AApplication::GetApplication().NVI_GetWindowByID(sTextInputRedirect_WindowID).NVI_SwitchFocusTo(kControlID_Invalid);
		sTextInputRedirect_WindowID = kObjectID_Invalid;
		sTextInputRedirect_ControlID = kControlID_Invalid;
	}
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoMouseDown(inLocalPoint,inModifierKeys,inClickCount);
}

//マウスボタン押下（コンテクストメニュー）時のコールバック(AView用)
ABool	AWindow::EVT_DoContextMenu( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoContextMenu(inLocalPoint,inModifierKeys,inClickCount);
}

//マウス移動時のコールバック(AView用)
ABool	AWindow::EVT_DoMouseMoved( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//現在のマウス位置と異なるときは何もしない
	AGlobalPoint	currentMouseGlobalPoint = {0};
	AWindow::NVI_GetCurrentMouseLocation(currentMouseGlobalPoint);
	AGlobalPoint	gpt = {0};
	NVI_GetGlobalPointFromControlLocalPoint(inID,inLocalPoint,gpt);
	if( gpt.x != currentMouseGlobalPoint.x || gpt.y != currentMouseGlobalPoint.y )
	{
		return true;
	}
	
	//ヘルプタグ更新
	NVI_UpdateHelpTag(inID,inLocalPoint,inModifierKeys,false);
	
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoMouseMoved(inLocalPoint,inModifierKeys);
}

//ツールチップウインドウを表示している対象コントロールを記憶するためのグローバル変数
AGlobalRect		gToolTipControlAreaGlobalRect = {0};
AModifierKeys	gToolTipModifierKeys = kModifierKeysMask_None;

/**
*/
void	AWindow::NVI_UpdateHelpTag( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
								  const ABool inUpdateAlways )
{
	//==================ヘルプタグ表示==================
	//rectは、ヘルプタグ描画位置（デフォルトでコントロールのrectを入れておく。EVT_DoGetHelpTag()で値が設定される）
	ALocalRect	controlAreaLocalRect = {0};
	controlAreaLocalRect.right = NVI_GetControlWidth(inID);
	controlAreaLocalRect.bottom = NVI_GetControlHeight(inID);
	AText	text1, text2;
	//AWindow, AViewオブジェクトから、現在のマウスポインタに対応するヘルプタグ情報を取得
	AHelpTagSide	tagside = kHelpTagSide_Default;//#643
	ABool	showToolTip = EVT_DoGetHelpTag(inID,inLocalPoint,text1,text2,controlAreaLocalRect,tagside);
	if( showToolTip == false )
	{
		//AWindow, AViewオブジェクトからEVT_DoGetHelpTag()でヘルプタグ取得できない場合は、AWindow::NVI_SetHelpTag()により設定されたデータから取得
		AIndex	index = mToolTipTextArray_ControlID.Find(inID);
		if( index != kIndex_Invalid )
		{
			showToolTip = true;
			mToolTipTextArray_ToolTipText.Get(index,text1);
			text2.SetText(text1);
			tagside = mToolTipTextArray_HelpTagSide.Get(index);
		}
	}
	//ビューでhelp tagがdisableの場合は、ヘルプタグ非表示にする
	if( IsView(inID) == true )
	{
		if( NVI_GetViewByControlID(inID).NVI_GetEnableHelpTag() == false )
		{
			showToolTip = false;
		}
	}
	//
	if( showToolTip == true )
	{
		//コントロールのglobal rectを取得
		AGlobalRect	controlAreaGlobalRect = {0};
		GetImp().GetGlobalRectFromControlLocalRect(inID,controlAreaLocalRect,controlAreaGlobalRect);
		//いずれかのmodifierキーを押しているかどうかでテキストを変える
		AText	tooltipText;
		if( inModifierKeys == kModifierKeysMask_None )
		{
			tooltipText.SetText(text1);
		}
		else
		{
			tooltipText.SetText(text2);
		}
		
		if( tooltipText.GetItemCount() > 0 )
		{
			//------------------ヘルプタグ表示する場合------------------
			//ツールチップウインドウにテキスト設定（およびサイズ設定）
			AApplication::GetApplication().NVI_GetToolTipWindow().SPI_SetToolTipText(tooltipText);
			//ツールチップウインドウのサイズ取得
			ANumber	w = AApplication::GetApplication().NVI_GetToolTipWindow().NVI_GetWindowWidth();
			ANumber	h = AApplication::GetApplication().NVI_GetToolTipWindow().NVI_GetWindowHeight();
			//マウス位置取得
			AGlobalPoint	mouseGlobalPoint = {0};
			GetImp().GetGlobalPointFromControlLocalPoint(inID,inLocalPoint,mouseGlobalPoint);
			//対象コントロールが前回と違うかどうかの判定
			if(	gToolTipControlAreaGlobalRect.left != controlAreaGlobalRect.left ||
			   gToolTipControlAreaGlobalRect.right != controlAreaGlobalRect.right ||
			   gToolTipControlAreaGlobalRect.top != controlAreaGlobalRect.top ||
			   gToolTipControlAreaGlobalRect.bottom != controlAreaGlobalRect.bottom ||
			   gToolTipModifierKeys != inModifierKeys ||
			   AApplication::GetApplication().NVI_GetToolTipWindow().NVI_IsWindowVisible() == false ||
			   inUpdateAlways == true )
			{
				//------------------対象コントロールが前回と違うので、ヘルプタグ位置調整------------------
				//スクリーン範囲取得
				AGlobalRect	screenRect = {0};
				NVI_GetAvailableWindowBoundsInSameScreen(screenRect);
				//ツールチップウインドウ位置計算
				ARect	tooltipRect = {0};
				switch(tagside)
				{
				  case kHelpTagSide_Default:
					{
						//コントロールの領域よりも下
						tooltipRect.left		= mouseGlobalPoint.x;
						tooltipRect.top			= controlAreaGlobalRect.bottom + 3;
						tooltipRect.right		= tooltipRect.left + w;
						tooltipRect.bottom		= tooltipRect.top + h;
						//
						APoint	pt = {tooltipRect.right,tooltipRect.bottom};
						if( PointInRect(pt,screenRect) == false )
						{
							//コントロールの領域よりも上
							tooltipRect.left		= mouseGlobalPoint.x;
							tooltipRect.right		= tooltipRect.left + w;
							tooltipRect.bottom		= controlAreaGlobalRect.top - 3;
							tooltipRect.top			= tooltipRect.bottom - h;
						}
						break;
					}
				  case kHelpTagSide_Right:
					{
						//コントロールの領域よりも右
						tooltipRect.left		= controlAreaGlobalRect.right + 3;
						tooltipRect.top			= controlAreaGlobalRect.bottom + 3;
						tooltipRect.right		= tooltipRect.left + w;
						tooltipRect.bottom		= tooltipRect.top + h;
						//
						APoint	pt = {tooltipRect.right,tooltipRect.bottom};
						if( PointInRect(pt,screenRect) == false )
						{
							pt.x = tooltipRect.right;
							pt.y = tooltipRect.top;
							if( PointInRect(pt,screenRect) == true )
							{
								//右上がスクリーン内なら、コントロール領域の上に表示する
								tooltipRect.left		= mouseGlobalPoint.x;
								tooltipRect.right		= tooltipRect.left + w;
								tooltipRect.bottom		= controlAreaGlobalRect.top - 3;
								tooltipRect.top			= tooltipRect.bottom - h;
							}
							else
							{
								//コントロールの領域の左に表示する
								tooltipRect.right		= controlAreaGlobalRect.left - 3;
								tooltipRect.top			= controlAreaGlobalRect.bottom + 3;
								tooltipRect.left		= tooltipRect.right - w;
								tooltipRect.bottom		= tooltipRect.top + h;
							}
						}
						break;
					}
				  case kHelpTagSide_Top:
					{
						tooltipRect.left		= mouseGlobalPoint.x;
						tooltipRect.right		= tooltipRect.left + w;
						tooltipRect.bottom		= controlAreaGlobalRect.top - 3;
						tooltipRect.top			= tooltipRect.bottom - h;
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
				//配置・表示
				AApplication::GetApplication().NVI_GetToolTipWindow().NVI_SetWindowBounds(tooltipRect);
				AApplication::GetApplication().NVI_GetToolTipWindow().NVI_ConstrainWindowPosition(false);//★うまく動作していない
				AApplication::GetApplication().NVI_GetToolTipWindow().NVI_Show(false);
			}
		}
		else
		{
			//------------------ヘルプタグ表示しない場合------------------
			//ヘルプタグ消去
			AApplication::GetApplication().NVI_HideToolTip();
		}
		//今回の対象コントロールを記憶
		gToolTipControlAreaGlobalRect = controlAreaGlobalRect;
		gToolTipModifierKeys = inModifierKeys;
	}
}

//#688
/**
ウインドウ対象としたmouse moved処理
*/
ABool	AWindow::EVT_DoMouseMoved( const AWindowPoint& inWindowPoint, const AModifierKeys inModifierKeys )
{
	return EVTDO_DoMouseMoved(inWindowPoint,inModifierKeys);
}

//マウスが離れた(AView用)win 080712
void	AWindow::EVT_DoMouseLeft( const AControlID inID, const ALocalPoint& inLocalPoint )
{
	//ヘルプタグ消去
	AApplication::GetApplication().NVI_HideToolTip();
	//
	if( IsView(inID) == false )   return;
	return NVI_GetViewByControlID(inID).EVT_DoMouseLeft(inLocalPoint);
}

//マウスボタン押下時のコールバック(AView用)
ACursorType	AWindow::EVT_GetCursorType( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return kCursorType_Arrow;
	return NVI_GetViewByControlID(inID).EVT_GetCursorType(inLocalPoint,inModifierKeys);
}

//描画要求時のコールバック(AView用)
void	AWindow::EVT_DoDraw( const AControlID inID )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoDraw();
}

//win 080712
//描画前処理を実行する。（キャレット一時消去など）
//EVT_DrawPreProcess()は、WindowsのWM_PAINTイベントでの描画時にコールされる。
//なお、NVIの中身処理であるEVTDO_DrawPreProcess()は、このルートのほかに、EVT_DoDraw()からもコールされる。
void	AWindow::EVT_DrawPreProcess( const AControlID inID )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DrawPreProcess();
}

//win 080712
//描画後処理を実行する。（キャレット復元など）
//EVT_DrawPostProcess()は、WindowsのWM_PAINTイベントでの描画時にコールされる。
//なお、NVIの中身処理であるEVTDO_DrawPreProcess()は、このルートのほかに、EVT_DoDraw()からもコールされる。
void	AWindow::EVT_DrawPostProcess( const AControlID inID )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DrawPostProcess();
}

//#138
//Viewの表示／非表示切り替え
void	AWindow::EVT_PreProcess_SetShowControl( const AControlID inID, const ABool inVisible )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_PreProcess_SetShowControl(inVisible);
}

//マウスホイールのコールバック(AView用)
void	AWindow::EVT_DoMouseWheel( const AControlID inID, const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
			const ALocalPoint inLocalPoint )//win 080706
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoMouseWheel(inDeltaX,inDeltaY,inModifierKeys,inLocalPoint);
}

//マウストラック（ドラッグ選択等）のコールバック(AView用)
void	AWindow::EVT_DoMouseTracking( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoMouseTracking(inLocalPoint,inModifierKeys);
}

//マウストラック終了のコールバック(AView用)
void	AWindow::EVT_DoMouseTrackEnd( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoMouseTrackEnd(inLocalPoint,inModifierKeys);
}

//
void	AWindow::EVT_DoControlBoundsChanged( const AControlID inID )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoControlBoundsChanged();
}

//#1026
//QuickLookコールバック(AView用)
void	AWindow::EVT_QuickLook( const AControlID inID, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_QuickLook(inLocalPoint,inModifierKeys);
}

//Drag&Dropコールバック(AView用)
void	AWindow::EVT_DoDragTracking( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoDragTracking(inDragRef,inLocalPoint,inModifierKeys,outIsCopyOperation);
}
void	AWindow::EVT_DoDragEnter( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoDragEnter(inDragRef,inLocalPoint,inModifierKeys);
}
void	AWindow::EVT_DoDragLeave( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoDragLeave(inDragRef,inLocalPoint,inModifierKeys);
}
void	AWindow::EVT_DoDragReceive( const AControlID inID, const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( IsView(inID) == false )   return;
	NVI_GetViewByControlID(inID).EVT_DoDragReceive(inDragRef,inLocalPoint,inModifierKeys);
}

//R240
//ヘルプタグ
ABool	AWindow::EVT_DoGetHelpTag( const AControlID inID, const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide )
{
	//#602 AWindow_でのhelptag設定に対応
	if( EVTDO_DoGetHelpTag(inID,inPoint,outText1,outText2,outRect,outTagSide) == true )   return true;
	//
	if( IsView(inID) == false )   return false;
	return NVI_GetViewByControlID(inID).EVT_DoGetHelpTag(inPoint,outText1,outText2,outRect,outTagSide);
}

//スクロールバー
void	AWindow::EVT_DoScrollBarAction( const AControlID inID, const AScrollBarPart inPart )
{
	/* #199 オブジェクト取得を1回ですませるように変更
	for( AIndex i = 0; i < mViewArray.GetItemCount(); i++ )
	{
		if( inID == mViewArray.GetObject(i).NVI_GetVScrollBarControlID() )
		{
			if( mViewArray.GetObject(i).NVI_IsVisible() == true )
			{
				mViewArray.GetObject(i).EVT_DoScrollBarAction(true,inPart);
			}
		}
		else if( inID == mViewArray.GetObject(i).NVI_GetHScrollBarControlID() )
		{
			if( mViewArray.GetObject(i).NVI_IsVisible() == true )
			{
				mViewArray.GetObject(i).EVT_DoScrollBarAction(false,inPart);
			}
		}
	}
	*/
	//各ViewにリンクしたV, Hスクロールを検索し、ControlIDが一致したら、ViewのScrollBarActionを実行する。
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView&	view = AApplication::GetApplication().NVI_GetViewByID(mViewIDArray.Get(i));
		//垂直スクロールバー
		if( inID == view.NVI_GetVScrollBarControlID() )
		{
			if( view.NVI_IsVisible() == true )
			{
				view.EVT_DoScrollBarAction(true,inPart);
			}
		}
		//水平スクロールバー
		else if( inID == view.NVI_GetHScrollBarControlID() )
		{
			if( view.NVI_IsVisible() == true )
			{
				view.EVT_DoScrollBarAction(false,inPart);
			}
		}
	}
	//派生クラス独自の処理実行
	EVTDO_DoScrollBarAction(inID,inPart);
}

//
void	AWindow::EVT_DoTickTimer()
{
	if( NVI_IsWindowActive() == false )   return;//win 080618
	AControlID	focus = NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		if( IsView(focus) == true )//#135
		{
			NVI_GetViewByControlID(focus).EVT_DoTickTimer();
		}
	}
	//派生クラス内処理
	EVTDO_DoTickTimerAction();
}

//#390
AWindowID	AWindow::sTextInputRedirect_WindowID = kObjectID_Invalid;
AControlID	AWindow::sTextInputRedirect_ControlID = kControlID_Invalid;

//#390
/**
*/
void	AWindow::STATIC_NVI_SetTextInputRedirect( const AWindowID inWindowID, const AControlID inControlID )
{
	sTextInputRedirect_WindowID = inWindowID;
	sTextInputRedirect_ControlID = inControlID;
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御
//ウインドウ生成
void	AWindow::NVI_Create( const ADocumentID inDocumentID )
{
	//ウインドウ未生成なら生成する
	if( NVI_IsWindowCreated() == false )
	{
		//ウインドウ生成、初期値設定
		NVIDO_Create(inDocumentID);
		AText	initialTitle;
		GetImp().GetTitle(initialTitle);
		mWindowDefaultTitle.SetText(initialTitle);
		//最初の（もしくは唯一の）タブ生成
		NVI_CreateTab(inDocumentID);
		//タブ選択
		NVI_SelectTab(0);
		//ウインドウ位置復元
		if( mWindowPositionDataID != kDataID_Invalid && NVM_GetOverlayMode() == false )//#291
		{
			APoint	pt;
			AApplication::GetApplication().NVI_GetAppPrefDB().GetData_Point(mWindowPositionDataID,pt);
			NVI_SetWindowPosition(pt);
			NVI_ConstrainWindowPosition(true);//#370 部分的に隠れてもOKを指定（#370対応以前と同じ）
		}
	}
}

//#182
/**
ウインドウ実体生成
*/
void	AWindow::NVM_CreateWindow( AConstCharPtr inWindowName, 
		const AWindowClass inWindowClass,//win
		const ABool inTabWindow, const AItemCount inTabCount )
{
	GetImp().Create(inWindowName,inWindowClass,inTabWindow,inTabCount);
}

//#182
/**
ウインドウ実体生成（基本的にNVIDO_Create()からコールされる）
*/
void	AWindow::NVM_CreateWindow( const AWindowClass inWindowClass, 
		const AOverlayWindowLayer inOverlayWindowLayer,//#688
		const ABool inHasCloseButton, const ABool inHasCollapseButton, 
		const ABool inHasZoomButton, const ABool inHasGrowBox, //#219
		const ABool inIgnoreClick, const ABool inHasShadow, const ABool inHasTitlebar )//#379 #688
{
	//#291 親ウインドウを設定
	//親ウインドウとの関連づけ方法はOSによって異なるので、Create()内で行う
	AWindowRef	parentWindowRef = NULL;
	if( NVM_GetOverlayMode() == true )
	{
		parentWindowRef = AApplication::GetApplication().NVI_GetWindowByID(mOverlayParentWindowID).NVI_GetWindowRef();
	}
	//生成
	GetImp().Create(inWindowClass,inHasCloseButton,inHasCollapseButton,
				inHasZoomButton,inHasGrowBox,inIgnoreClick,inHasShadow,inHasTitlebar,parentWindowRef,inOverlayWindowLayer);//#291 #379 #688
}

//ウインドウ表示
void	AWindow::NVI_Show( const ABool inSelect )
{
	//ウインドウ未生成ならリターン
	if( NVI_IsWindowCreated() == false )   return;
	
	if( NVI_IsWindowVisible() == false )
	{
		//派生ウインドウごとの処理
		NVIDO_Show();
		//
		NVI_UpdateProperty();
		//タイトルバー更新
		NVI_UpdateTitleBar();
		//ウインドウ表示
		GetImp().Show();
		//
		NVM_UpdateControlStatus();
		//#135
		NVI_SwitchFocusTo(mLatentFocus);
		//
		//#852 NVI_RefreshWindow();//win
	}
	//選択
	if( inSelect == true )
	{
		NVI_SelectWindow();
	}
	mVisibleMode = true;//B0404
}

//ウインドウを生成して表示
//inSelect: ウインドウを選択（最前面）するかどうか
void	AWindow::NVI_CreateAndShow( const ABool inSelect )
{
	//ウインドウ未生成なら生成する
	if( NVI_IsWindowCreated() == false )
	{
		NVI_Create(kObjectID_Invalid);
	}
	NVI_Show(inSelect);
}

//ウインドウ非表示
void	AWindow::NVI_Hide()
{
	//ウインドウ未生成ならリターン
	if( NVI_IsWindowCreated() == false )   return;
	
	if( NVI_IsWindowVisible() == true )
	{
		//ウインドウ位置の保存
		if( mWindowPositionDataID != kDataID_Invalid && NVM_GetOverlayMode() == false )//#291
		{
			APoint	pt;
			NVI_GetWindowPosition(pt);
			AApplication::GetApplication().NVI_GetAppPrefDB().SetData_Point(mWindowPositionDataID,pt);
		}
		
		//派生ウインドウごとの処理
		NVIDO_Hide();
		
		//ウインドウ非表示
		GetImp().Hide();
		
		//DB連動処理
		if( IsDBDataTableExist() == true )
		{
			NVM_StoreToDBAll();
			//DBのデータをファイルへ書き込み
			NVM_GetDB().SaveToFile();
		}
	}
	//B0000 アプリケーションデアクティベート時にコールされると閉じれないので、Floatingウインドウは特殊処理
	//（下記処理をif外の共通処理にしても良い気もするが、影響度が不安なので当面これで・・・）
	else if( NVI_IsFloating() == true )
	{
		//派生ウインドウごとの処理
		NVIDO_Hide();
		
		//ウインドウ非表示
		GetImp().Hide();
	}
	mVisibleMode = false;//B0404
	//ヘルプタグ消去
	AApplication::GetApplication().NVI_HideToolTip();
}

//ウインドウ非表示＋削除
void	AWindow::NVI_Close()
{
	//ウインドウ未生成ならリターン
	if( NVI_IsWindowCreated() == false )   return;
	
	//子ウインドウを全て閉じる #291
	NVI_CloseChildWindow();
	//ウインドウ非表示
	NVI_Hide();
	//タブ/View/Controlの削除 #92
	while( NVI_GetTabCount() > 0 )
	{
		NVI_DeleteTabAndView(0);
	}
	//残りのViewを全て削除する #92
	while( mViewControlIDArray.GetItemCount() > 0 )
	{
		NVI_DeleteControl(mViewControlIDArray.Get(0));//win NVI_DeleteView()はmTabDataArray_ControlIDsからの削除等を行わないのでNG
	}
	//ウインドウ削除
	GetImp().CloseWindow();
	//#135
	mFocusGroup.DeleteAll();
	mFocusGroup_IsGroup.DeleteAll();//#353
	//#688 mFocusGroup2nd.DeleteAll();//#353
	//#688 mFocusGroup2nd_IsGroup.DeleteAll();//#353
	//#349
	mRadioGroup.DeleteAll();
	mRadioGroup_All.DeleteAll();
	//
	NVIDO_Close();
	//タブデータの削除 B0000
	/*#92 mTabDataArray_ControlIDsにセットされたView/Controlを削除すべきなので、NVI_DeleteTabAndView()をコールする方法に変更
	mTabDataArray_ControlIDs.DeleteAll();
	mTabDataArray_mWindowMenuShortcut.DeleteAll();
	mTabDataArray_DocumentID.DeleteAll();
	mTabDataArray_LatentFocus.DeleteAll();
	mTabZOrder.DeleteAll();
	mCurrentTabIndex = kIndex_Invalid;
	*/
	//B0000
	//#135 mCurrentFocus = kControlID_Invalid;
	mLatentFocus = kControlID_Invalid;//#135
}

//ウインドウ選択
void	AWindow::NVI_SelectWindow()
{
	GetImp().Select();
}

/**
ウインドウを背後に移動
@param inWindowID 一つ前のウインドウ。kObjectID_Invalidなら最背面に移動
*/
void	AWindow::NVI_SendBehind( const AWindowID inWindowID )
{
	if( inWindowID == kObjectID_Invalid )
	{
		GetImp().SendBehindAll();//#567
	}
	else
	{
		GetImp().SendBehind(AApplication::GetApplication().NVI_GetWindowByID(inWindowID).GetImp());
	}
}

//#688
/**
ウインドウを前面に移動
@param inWindowID 一つ前のウインドウ。kObjectID_Invalidなら最背面に移動
*/
void	AWindow::NVI_SendAbove( const AWindowID inWindowID )
{
	if( inWindowID == kObjectID_Invalid )
	{
		//最前面に移動
		GetImp().SendAboveAll();
	}
	else
	{
		//inWindowIDの前面に移動
		GetImp().SendAbove(AApplication::GetApplication().NVI_GetWindowByID(inWindowID).GetImp());
	}
}

//#688
/**
子オーバーレイウインドウのz順序をウインドウ生成時に指定したAOverlayWindowLayerに従って再配置
*/
void	AWindow::NVI_ReorderOverlayChildWindows()
{
	GetImp().ReorderOverlayChildWindows();
}

//ウインドウ全体の表示を更新
void	AWindow::NVI_UpdateProperty()
{
	//ウインドウ未生成ならリターン #402
	if( NVI_IsWindowCreated() == false )   return;
	
	if( IsDBDataTableExist() == true )
	{
		UpdateDBDataAndTable();
	}
	NVIDO_UpdateProperty();
	NVM_UpdateControlStatus();
}

//ウインドウリフレッシュ
void	AWindow::NVI_RefreshWindow()
{
	if( NVI_IsWindowCreated() == false )   return;//#0 ウインドウ未生成時はリフレッシュできないので何もせずリターン
	GetImp().RefreshWindow();
}

void	AWindow::NVI_SetWindowBounds( const ARect& inBounds, const ABool inAnimate )//#688
{
	//現在のboundsと同じなら何もせず終了
	ARect	oldBounds = {0};
	GetImp().GetWindowBounds(oldBounds);
	if( inBounds.left == oldBounds.left && inBounds.top == oldBounds.top && 
				inBounds.right == oldBounds.right && inBounds.bottom == oldBounds.bottom )
	{
		return;
	}
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(false);
	}
	//
	GetImp().SetWindowBounds(inBounds,inAnimate);//#688
	//サイズ変更途中でなければ、リサイズ完了処理を実行（AView_SizeBoxクラス等からNVI_SetVirtualLiveResizing()によりサイズ変更実行中設定される）
	if( mVirtualLiveResizing == false )
	{
		EVT_WindowResizeCompleted();//B0415
	}
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(true);
	}
}

void	AWindow::NVI_GetWindowBounds( ARect& outBounds ) const
{
	GetImpConst().GetWindowBounds(outBounds);
}

void	AWindow::NVI_SetWindowPosition( const APoint& inPoint )
{
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(false);
	}
	//
	GetImp().SetWindowPosition(inPoint);
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(true);
	}
}

void	AWindow::NVI_GetWindowPosition( APoint& outPoint ) const
{
	GetImpConst().GetWindowPosition(outPoint);
}

//ウインドウサイズ設定
void	AWindow::NVI_SetWindowSize( const ANumber inWidth, const ANumber inHeight )
{
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(false);
	}
	//
	GetImp().SetWindowSize(inWidth,inHeight);
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(true);
	}
}

//#182
/**
ウインドウ幅設定
*/
void	AWindow::NVI_SetWindowWidth( const ANumber inWidth )
{
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(false);
	}
	//
	GetImp().SetWindowWidth(inWidth);
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(true);
	}
}

//#182
/**
ウインドウ高さ設定
*/
void	AWindow::NVI_SetWindowHeight( const ANumber inHeight )
{
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(false);
	}
	//
	GetImp().SetWindowHeight(inHeight);
	//#291
	if( NVM_GetOverlayMode() == true )
	{
		GetImp().SetOverlayWindowMoveTogether(true);
	}
}

//ウインドウ幅取得
ANumber	AWindow::NVI_GetWindowWidth() const
{
	return GetImpConst().GetWindowWidth();
}

//ウインドウ高さ取得
ANumber	AWindow::NVI_GetWindowHeight() const
{
	return GetImpConst().GetWindowHeight();
}

//B0395
void	AWindow::NVI_CollapseWindow( const ABool inCollapse )
{
	GetImp().CollapseWindow(inCollapse);
}

//#92
/**

*/
void	AWindow::NVI_TryClose( const AIndex inTabIndex )//#164
{
	if( mInhibitClose == true )   return;
	
	NVIDO_TryClose(inTabIndex);//#164
}

/**

*/
void	AWindow::NVI_ExecuteClose()
{
	if( mInhibitClose == true )   return;
	
	NVIDO_ExecuteClose();
}

/**
各タブについて、保存確認ダイアログを表示
*/
ABool	AWindow::NVI_AskSaveForAllTabs()
{
	return NVIDO_AskSaveForAllTabs();
}

//#182
/**
ウインドウをSheetウインドウに設定する
*/
void	AWindow::NVI_SetWindowStyleToSheet( const AWindowID inParentWindowID )
{
	GetImp().SetWindowStyleToSheet(AApplication::GetApplication().NVI_GetWindowByID(inParentWindowID).mWindowImp);
}

//#182
/**
ウインドウをDrawerウインドウに設定する
*/
/*#688
void	AWindow::NVI_SetWindowStyleToDrawer( const AWindowID inParentWindowID )
{
	GetImp().SetWindowStyleToDrawer(AApplication::GetApplication().NVI_GetWindowByID(inParentWindowID).mWindowImp);
}
*/

/*win
//#182
**
ウインドウをフローティングにする
*
void	AWindow::NVI_SetFloating()
{
	GetImp().SetFloating();
}
*/

//#182
/**
ウインドウをデスクトップ内に収まるように配置する
*/
void	AWindow::NVI_ConstrainWindowPosition( const ABool inAllowPartial )//#370
{
	GetImp().ConstrainWindowPosition(inAllowPartial);//#370
}

/**
指定ポイントがデスクトップ内かどうかを判定
*/
void	AWindow::NVI_ConstrainWindowPosition( const ABool inAllowPartial, APoint& ioPoint ) const
{
	GetImpConst().ConstrainWindowPosition(inAllowPartial,ioPoint);
}

//#385
/**
ウインドウをデスクトップ内に収まるようにリサイズする
*/
void	AWindow::NVI_ConstrainWindowSize()
{
	GetImp().ConstrainWindowSize();
}

#if IMPLEMENTATION_FOR_WINDOWS
//#182
/**
サイズボックス生成
*/
void	AWindow::NVI_CreateSizeBox( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	GetImp().CreateSizeBox(inID,inPoint,inWidth,inHeight);
}

//#182
/**
FileOpenDialogを表示する（Windows用）
*/
void	AWindow::NVI_ShowFileOpenWindow( const AText& inFilter, const ABool inShowInvisibleFile )
{
	GetImp().ShowFileOpenWindow(inFilter,inShowInvisibleFile);
}
#endif

//#291
/**
OverlayモードでなければウインドウをShow
*/
void	AWindow::NVI_ShowIfNotOverlayMode()
{
	if( NVM_GetOverlayMode() == false )
	{
		NVI_Show(false);
	}
}

//#291
/**
OverlayモードでなければウインドウをHide
*/
void	AWindow::NVI_HideIfNotOverlayMode()
{
	if( NVM_GetOverlayMode() == false )
	{
		NVI_Hide();
	}
}

//#409
/**
ウインドウを明示的に（即時）更新
*/
void	AWindow::NVI_UpdateWindow()
{
	GetImp().UpdateWindow();
}

//win
/**
*/
void	AWindow::NVI_Zoom()
{
	GetImp().Zoom();
}

//#404
/**
フルスクリーンモード設定
*/
void	AWindow::NVI_SetFullScreenMode( const ABool inFullScreenMode )
{
	GetImp().SetFullScreenMode(inFullScreenMode);
}

//#404
/**
フルスクリーンモード状態取得
*/
ABool	AWindow::NVI_GetFullScreenMode() const
{
	return GetImpConst().GetFullScreenMode();
}

//#505
/**
ウインドウの最小サイズ設定
*/
void	AWindow::NVI_SetWindowMinimumSize( const ANumber inWidth, const ANumber inHeight )
{
	GetImp().SetWindowMinimumSize(inWidth,inHeight);
}

//#505
/**
ウインドウの最大サイズ設定
*/
void	AWindow::NVI_SetWindowMaximumSize( const ANumber inWidth, const ANumber inHeight )
{
	GetImp().SetWindowMaximumSize(inWidth,inHeight);
}

#pragma mark ===========================

#pragma mark ---タブ制御

//タブ生成
void	AWindow::NVI_CreateTab( const ADocumentID inDocumentID, const ABool inSelectableTab )
{
	//#291 ウインドウ未生成なら生成
	if( NVI_IsWindowCreated() == false )
	{
		NVI_Create(inDocumentID);//この中でNVI_CreateTab()がコールされる
		return;
	}
	
	AIndex	tabIndex = mTabDataArray_ControlIDs.AddNewObject();
	mTabDataArray_mWindowMenuShortcut.Add(kIndex_Invalid);
	mTabDataArray_DocumentID.Add(inDocumentID);
	mTabDataArray_LatentFocus.Add(kControlID_Invalid);
	mTabDataArray_LatentVisible.AddNewObject();//#212
	mTabDataArray_TabSelectable.Add(inSelectableTab);//#379
	if( inSelectableTab == true )//#379
	{
		if( mTabZOrder.GetItemCount() > 0 )
		{
			//B0411 最初のタブ以外の場合は、2番目のZorderへタブを挿入する
			mTabZOrder.Insert1(1,tabIndex);
		}
		else
		{
			//最前面にタブを挿入
			mTabZOrder.Insert1(0,tabIndex);
		}
		//現在タブ未設定なら、生成したタブを現在タブに設定する。
		//本来はmCurrentTabIndexは、NVI_SelectTab()に設定されるべきだが、
		//NVIDO_CreateTab()内にてNVI_GetCurrentDocumentID()等を使用するとcritical throw等になってしまうので、
		//ここで現在タブを設定する方がベター。mTabDataArray_XXXは設定済みなので、基本的に問題は無いはず。 #963
		if( mCurrentTabIndex == kIndex_Invalid )
		{
			mCurrentTabIndex = tabIndex;
		}
	}
	//派生クラスごとの処理
	AControlID	initialFocus = kControlID_Invalid;
	NVIDO_CreateTab(tabIndex,initialFocus);
	//B0411 最初のtabでない場合はinvisibleにしておく
	if( mTabDataArray_ControlIDs.GetItemCount() > 1 )
	{
		for( AIndex viewIndex = 0; viewIndex < mTabDataArray_ControlIDs.GetObject(tabIndex).GetItemCount(); viewIndex++ )
		{
			NVI_SetShowControl(mTabDataArray_ControlIDs.GetObject(tabIndex).Get(viewIndex),false);
		}
	}
	//初期フォーカスの設定（生成したタブがSelectTab()されたときにウインドウ内フォーカスになる。）
	mTabDataArray_LatentFocus.Set(tabIndex,initialFocus);
	//最初のタブ生成の場合、initialフォーカスにフォーカス移動
	if( inSelectableTab == true )
	{
		if( mTabZOrder.GetItemCount() == 1 )
		{
			//フォーカス設定
			NVI_SwitchFocusTo(initialFocus);
		}
	}
}

//タブ、および、タブが内包するビューの削除
void	AWindow::NVI_DeleteTabAndView( const AIndex inTabIndex )
{
	//フォーカスを解除する（NVI_SelectTab()までの間に、フォーカスViewを取得されることで、すでに削除したViewを使用することになってしまうのを防ぐため）
	//タブ切り替えになる場合は、NVI_SelectTab()で新しいフォーカスが設定される。
	NVI_SwitchFocusTo(kControlID_Invalid);
	//派生クラスごとの処理
	NVIDO_DeleteTab(inTabIndex);
	//内包ビューの削除
	/*#243 NVI_DeleteControl()内部でmTabDataArray_ControlIDsからの登録を解除するように変更したので、矛盾が生じないよう、一旦ControlIDをローカルに移す
	for( AIndex index = 0; index < mTabDataArray_ControlIDs.GetObject(inTabIndex).GetItemCount(); index++ )
	{
		NVI_DeleteControl(mTabDataArray_ControlIDs.GetObject(inTabIndex).Get(index));
	}
	*/
	//タブ内のコントロールIDを全てcontrolIDArrayへコピーする。（下で実際に削除する）
	AArray<AControlID>	controlIDArray;
	for( AIndex index = 0; index < mTabDataArray_ControlIDs.GetObject(inTabIndex).GetItemCount(); index++ )
	{
		controlIDArray.Add(mTabDataArray_ControlIDs.GetObject(inTabIndex).Get(index));
	}
	/*#688 下へ移動（現在のタブindex、タブデータを削除後の正常な状態にした後で、コントロールの削除を行う）
	for( AIndex i = 0; i < controlIDArray.GetItemCount(); i++ )
	{
		AControlID	controlID = controlIDArray.Get(i);
		//すでにほかのControl削除時に同時削除されている可能性があるので、存在チェックを行ってから削除
		if( mTabDataArray_ControlIDs.GetObjectConst(inTabIndex).Find(controlID) != kIndex_Invalid )
		{
			NVI_DeleteControl(controlID);
		}
	}
	*/
	
	//#725
	//登録オーバーレイウインドウのうち、当該tabに属するものを削除する
	AArray<AWindowID>	deletedOverlayWindowIDArray;
	for( AIndex i = 0; i < mChildOverlayWindowArray_TabIndex.GetItemCount(); i++ )
	{
		AIndex	tabIndex = mChildOverlayWindowArray_TabIndex.Get(i);
		if( tabIndex == inTabIndex )
		{
			//当該tab内なら、そのウインドウを削除する。
			AWindowID	overlayWindowID = mChildOverlayWindowArray_WindowID.Get(i);
			AApplication::GetApplication().NVI_DeleteWindow(overlayWindowID);
			deletedOverlayWindowIDArray.Add(overlayWindowID);
		}
		else if( tabIndex > inTabIndex )
		{
			//当該tab外で、削除tab以降なら、tabIndexをデクリメントする
			mChildOverlayWindowArray_TabIndex.Set(i,tabIndex-1);
		}
	}
	//上で削除したものを子オーバーレイウインドウ登録から登録解除する
	for( AIndex i = 0; i < deletedOverlayWindowIDArray.GetItemCount(); i++ )
	{
		NVM_UnregisterOverlayWindow(deletedOverlayWindowIDArray.Get(i));
	}
	
	//タブデータの削除
	mTabDataArray_ControlIDs.Delete1Object(inTabIndex);
	mTabDataArray_mWindowMenuShortcut.Delete1(inTabIndex);
	mTabDataArray_DocumentID.Delete1(inTabIndex);
	mTabDataArray_LatentFocus.Delete1(inTabIndex);
	mTabDataArray_LatentVisible.Delete1Object(inTabIndex);//#212
	mTabDataArray_TabSelectable.Delete1(inTabIndex);//#379
	AIndex	tabZOrderIndex = mTabZOrder.Find(inTabIndex);
	if( tabZOrderIndex != kIndex_Invalid )//#379
	{
		mTabZOrder.Delete1(tabZOrderIndex);
	}
	//TabZOrder配列について、今回削除したタブ以降のIndexを１つ前にずらす
	for( AIndex i = 0; i < mTabZOrder.GetItemCount(); i++ )
	{
		if( mTabZOrder.Get(i) > inTabIndex )
		{
			mTabZOrder.Set(i,mTabZOrder.Get(i)-1);
		}
	}
	
	//#663 現在アクティブなタブを削除した場合のみ、新たなタブを選択し直す
	if( inTabIndex == mCurrentTabIndex )
	{
		mCurrentTabIndex = kIndex_Invalid;
		
		//タブ選択（ZOrder最前面のタブ）
		if( NVI_GetSelectableTabCount() > 0 )
		{
			NVI_SelectTab(mTabZOrder.Get(0));
		}
	}
	else
	{
		//アクティブでないタブを削除した場合の処理#663
		//mCurrentTabIndexが削除したタブより後なら、インデックス値の補正をする
		if( mCurrentTabIndex > inTabIndex )
		{
			mCurrentTabIndex--;
		}
	}
	//#688 上から移動（現在のタブindex、タブデータを削除後の正常な状態にした後で、コントロールの削除を行う）
	//タブ内のコントロールを削除
	for( AIndex i = 0; i < controlIDArray.GetItemCount(); i++ )
	{
		AControlID	controlID = controlIDArray.Get(i);
		//すでにほかのControl削除時に同時削除されている可能性がある（frame viewの場合、メインview削除時に同時削除される）ので、
		//存在チェックを行ってから削除
		if( GetImp().ExistControl(controlID) == true )
		{
			NVI_DeleteControl(controlID);
		}
	}
	//#663 タブ削除後の各ウインドウ処理
	if( NVI_GetTabCount() > 0 )
	{
		NVIDO_TabDeleted();
	}
}

//ウインドウ選択＋タブ選択
void	AWindow::NVI_SelectTab( const AIndex inTabIndex )
{
	if( inTabIndex == mCurrentTabIndex/*B0415 && inForRedraw == false*/ )   return;
	//現在のTabにDeactivatedを通知
	if( mCurrentTabIndex != kIndex_Invalid )
	{
		NVM_DoTabDeactivated(mCurrentTabIndex);
	}
	//現在Tab更新
	mCurrentTabIndex = inTabIndex;
	//ZOrder更新
	mTabZOrder.Delete1(mTabZOrder.Find(mCurrentTabIndex));
	mTabZOrder.Insert1(0,mCurrentTabIndex);
	//TabがこれからActivateされることを通知 win ShowControl()される前の準備処理(SPI_DoViewActivated()等を行うため)
	NVM_DoTabActivating(inTabIndex);
	//#1091
	//タブ内フォーカスが設定済みなら、それをフォーカスに設定する。（タブ内のViewのうち、最後にフォーカスされていたものが、タブ内フォーカスとして設定されている）
	//（以前はタブViewの表示制御をおこなったのちにフォーカス設定していたが、Mac OS X 10.10では、現在のフォーカスをhideすると、
	//フォーカスが勝手に検索フィールドに移動する？と思われるため、hideする前に、今回フォーカスになるべきviewは先にshowする。）
	AControlID	tabFocus = NVI_GetFocusInTab(mCurrentTabIndex);
	if( tabFocus != kControlID_Invalid )
	{
		//Viewを表示する
		NVI_SetShowControl(tabFocus,true);
		//フォーカスを設定する
		NVI_SwitchFocusTo(tabFocus);
	}
	//タブViewを表示する
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_ControlIDs.GetItemCount(); tabIndex++ )
	{
		for( AIndex viewIndex = 0; viewIndex < mTabDataArray_ControlIDs.GetObject(tabIndex).GetItemCount(); viewIndex++ )
		{
			ABool	visible = (tabIndex==mCurrentTabIndex);
			//#212 LatentVisibleで無い場合は表示しない
			if( mTabDataArray_LatentVisible.GetObjectConst(tabIndex).Get(viewIndex) == false )
			{
				visible = false;
			}
			//表示設定
			NVI_SetShowControl(mTabDataArray_ControlIDs.GetObject(tabIndex).Get(viewIndex),visible);
		}
	}
	//#725
	//登録子オーバーレイウインドウのshow/hide制御
	for( AIndex windowIndex = 0; windowIndex < mChildOverlayWindowArray_WindowID.GetItemCount(); windowIndex++ )
	{
		AIndex	tabIndex = mChildOverlayWindowArray_TabIndex.Get(windowIndex);
		//タブに属さない場合は何もしない。
		if( tabIndex == kIndex_Invalid )
		{
			continue;
		}
		//今回選択されたタブに属するオーバーレイウインドウの場合、かつ、latent visibleの場合、showする。それ以外はhide。
		AWindowID	winID = mChildOverlayWindowArray_WindowID.Get(windowIndex);
		ABool	latentVisible = mChildOverlayWindowArray_LatentVisible.Get(windowIndex);
		if( tabIndex == mCurrentTabIndex && latentVisible == true )
		{
			AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_Show(false);
		}
		else
		{
			AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_Hide();
		}
	}
	//タブ内フォーカスが設定済みなら、それをフォーカスに設定する。（タブ内のViewのうち、最後にフォーカスされていたものが、タブ内フォーカスとして設定されている）
	//（#1091により上でフォーカス設定しているが、再度設定しても副作用はないはずなので、こちらでも念のため設定する）
	tabFocus = NVI_GetFocusInTab(mCurrentTabIndex);
	if( tabFocus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(tabFocus);
	}
	//#725 自動制御以外のviewのshow/hideなどのため、派生クラスで処理を実行
	NVIDO_SelectTab(inTabIndex);
	//新規TabにActivatedを通知
	NVM_DoTabActivated(inTabIndex,true);//win 080712
}

void	AWindow::NVI_SetTabZOrder( const AIndex inTabIndex, const AIndex inZOrder )
{
	if( inZOrder == 0 ) 
	{
		NVI_SelectTab(inTabIndex);
	}
	else
	{
		//ZOrder更新
		AIndex	oldZOrder = mTabZOrder.Find(inTabIndex);
		mTabZOrder.Delete1(oldZOrder);
		mTabZOrder.Insert1(inZOrder,inTabIndex);
		//
		if( oldZOrder == 0 )
		{
			NVI_SelectTab(mTabZOrder.Get(0));
		}
		//
		NVI_RefreshWindow();
	}
}

AIndex	AWindow::NVI_GetTabZOrder( const AIndex inTabIndex ) const
{
	return mTabZOrder.Find(inTabIndex);
}

//TabIndexからドキュメントIDを取得
ADocumentID	AWindow::NVI_GetDocumentIDByTabIndex( const AIndex inTabIndex ) const
{
	return mTabDataArray_DocumentID.Get(inTabIndex);
}

//ドキュメントIDからTabIndexを取得
AIndex	AWindow::NVI_GetTabIndexByDocumentID( const AObjectID inDocumentID ) const
{
	return mTabDataArray_DocumentID.Find(inDocumentID);
}

//
void	AWindow::NVI_SelectTabByDocumentID( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex != kIndex_Invalid )
	{
		NVI_SelectTab(tabIndex);
	}
}

//#92
/**
指定Controlを内包するTabを選択する
*/
void	AWindow::NVI_SelectTabByIncludingControlID( const AControlID inControlID )
{
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_ControlIDs.GetItemCount(); tabIndex++ )
	{
		if( mTabDataArray_ControlIDs.GetObject(tabIndex).Find(inControlID) != kIndex_Invalid )
		{
			NVI_SelectTab(tabIndex);
			return;
		}
	}
}

//TabにView追加
void	AWindow::NVM_RegisterViewInTab( const AIndex inTabIndex, const AControlID inControlID )
{
	mTabDataArray_ControlIDs.GetObject(inTabIndex).Add(inControlID);
	mTabDataArray_LatentVisible.GetObject(inTabIndex).Add(true);//#212
	//ViewにFrameViewが存在する場合はFrameViewも同じTabに格納する #135
	AControlID	frameControlID = kControlID_Invalid;
	if( IsView(inControlID) == true )
	{
		frameControlID = NVI_GetViewByControlID(inControlID).NVI_GetFrameViewControlID();
	}
	if( frameControlID != kControlID_Invalid )
	{
		mTabDataArray_ControlIDs.GetObject(inTabIndex).Add(frameControlID);
		mTabDataArray_LatentVisible.GetObject(inTabIndex).Add(true);//#212
	}
	//win
	if( inTabIndex != mCurrentTabIndex )
	{
		NVI_SetShowControl(inControlID,false);
	}
}

//TabにView追加
void	AWindow::NVM_UnregisterViewInTab( const AIndex inTabIndex, const AControlID inControlID )
{
	AIndex	index = mTabDataArray_ControlIDs.GetObject(inTabIndex).Find(inControlID);
	if( index == kIndex_Invalid )   _ACThrow("not found controlID",this);
	mTabDataArray_ControlIDs.GetObject(inTabIndex).Delete1(index);
	mTabDataArray_LatentVisible.GetObject(inTabIndex).Delete1(index);//#212
}

//#212
/**
Tab表示したときにControl表示するかどうかを設定する(LatentVisible)
*/
void	AWindow::NVI_SetLatentVisible( const AIndex inTabIndex, const AControlID inControlID, const ABool inLatentVisible )
{
	AIndex	index = mTabDataArray_ControlIDs.GetObject(inTabIndex).Find(inControlID);
	if( index == kIndex_Invalid )   _ACThrow("not found controlID",this);
	mTabDataArray_LatentVisible.GetObject(inTabIndex).Set(index,inLatentVisible);
}

//win
/**
Tab Activate準備処理
*/
void	AWindow::NVM_DoTabActivating( const AIndex inTabIndex )
{
	NVMDO_DoTabActivating(inTabIndex);
}

//Tab Activated時の処理
//inTabSwitched: true: タブが切り替わった  false: タブは切り替わらずウインドウがactivateされた
void	AWindow::NVM_DoTabActivated( const AIndex inTabIndex, const ABool inTabSwitched )//win 080712
{
	if( NVI_IsWindowVisible() == true )
	{
		//
		NVMDO_DoTabActivated(inTabIndex,inTabSwitched);//win 080712
		/*#135 DoViewFocusActivated()はNVI_SelectTab()からNVI_SwitchFocusTo()経由で実行するように変更
		//
		AControlID	focus = mTabDataArray_LatentFocus.Get(inTabIndex);
		if( ExistView(focus) == true )
		{
			NVI_GetViewByControlID(focus).NVM_DoViewFocusActivated();
		}
		*/
		//
		NVI_UpdateTitleBar();
	}
}

//Tab Deactivated
void	AWindow::NVM_DoTabDeactivated( const AIndex inTabIndex )
{
	if( NVI_IsWindowVisible() == true )
	{
		//
		NVMDO_DoTabDeactivated(inTabIndex);
		/*#135 DoViewFocusActivated()はNVI_SelectTab()からNVI_SwitchFocusTo()経由で実行するように変更
		//
		AControlID	focus = mTabDataArray_LatentFocus.Get(inTabIndex);
		if( ExistView(focus) == true )
		{
			NVI_GetViewByControlID(focus).NVM_DoViewFocusDeactivated();
		}
		*/
	}
}

//再背面にあるタブのインデックスを取得
AIndex	AWindow::NVI_GetMostBackTabIndex() const
{
	return mTabZOrder.Get(mTabZOrder.GetItemCount()-1);
}

//
AIndex	AWindow::NVI_GetTabIndexByZOrderIndex( const AIndex inZOrderIndex ) const
{
	return mTabZOrder.Get(inZOrderIndex);
}

//#379
/**
タブ選択可能かどうか（表示有無）を設定する
*/
ABool	AWindow::NVI_GetTabSelectable( const AIndex inTabIndex ) const
{
	return mTabDataArray_TabSelectable.Get(inTabIndex);
}

//#559
/**
次の表示タブを取得
*/
ABool	AWindow::NVIDO_GetNextDisplayTab( AIndex& ioIndex, const ABool inLoop ) const
{
	//
	AIndex	tabIndex = ioIndex;
	tabIndex++;
	for( ; tabIndex != ioIndex; tabIndex++ )
	{
		if( tabIndex >= NVI_GetTabCount() )
		{
			if( inLoop == true )
			{
				tabIndex = 0;
			}
			else
			{
				return false;
			}
		}
		if( NVI_GetTabSelectable(tabIndex) == true )   break;
	}
	//
	ABool	changed = (tabIndex != ioIndex);
	ioIndex = tabIndex;
	return changed;
}

//#559
/**
前の表示タブを取得
*/
ABool	AWindow::NVIDO_GetPrevDisplayTab( AIndex& ioIndex, const ABool inLoop ) const
{
	//
	AIndex	tabIndex = ioIndex;
	tabIndex--;
	for( ; tabIndex != ioIndex; tabIndex-- )
	{
		if( tabIndex < 0 )
		{
			if( inLoop == true )
			{
				tabIndex = NVI_GetTabCount()-1;
			}
			else
			{
				return false;
			}
		}
		if( NVI_GetTabSelectable(tabIndex) == true )   break;
	}
	//
	ABool	changed = (tabIndex != ioIndex);
	ioIndex = tabIndex;
	return changed;
}

//#559
/**
最初の表示タブを取得
*/
AIndex	AWindow::NVI_GetFirstDisplayTab() const
{
	AIndex	tabIndex = 0;
	NVI_GetPrevDisplayTab(tabIndex,true);
	NVI_GetNextDisplayTab(tabIndex,true);
	return tabIndex;
}

//#559
/**
最後の表示タブを取得
*/
AIndex	AWindow::NVI_GetLastDisplayTab() const
{
	AIndex	tabIndex = 0;
	NVI_GetPrevDisplayTab(tabIndex,true);
	return tabIndex;
}

#pragma mark ===========================

#pragma mark ---ウインドウ情報設定／取得

//ウインドウ表示中かを取得
ABool	AWindow::NVI_IsWindowVisible() const
{
	if( NVI_IsWindowCreated() == false )   return false;//#402
	return GetImpConst().IsVisible();
}

//ウインドウ作成済みかを取得
ABool	AWindow::NVI_IsWindowCreated() const
{
	return GetImpConst().IsWindowCreated();
}

//ウインドウがアクティブかを取得
ABool	AWindow::NVI_IsWindowActive() const
{
	if( NVI_IsWindowCreated() == false )   return false;//#402
	//#634 オーバーレイモードのときは、親ウインドウのactive状態を返す
	if( NVM_GetOverlayMode() == true )
	{
		return AApplication::GetApplication().NVI_GetWindowByID(mOverlayParentWindowID).NVI_IsWindowActive();
	}
	//
	return GetImpConst().IsActive();
}

/**
ウインドウがFocusアクティブ(Key window)かを取得
*/
ABool	AWindow::NVI_IsWindowFocusActive() const
{
	if( NVI_IsWindowCreated() == false )   return false;//#402
	//
	return GetImpConst().IsFocusActive();
}

//WindowRef取得
AWindowRef	AWindow::NVI_GetWindowRef() const
{
	return GetImpConst().GetWindowRef();
}

//現在のタブに対応するDocumentIDを取得
ADocumentID	AWindow::NVI_GetCurrentDocumentID() const
{
	AIndex	currentTabIndex = NVI_GetCurrentTabIndex();
	if( currentTabIndex == kIndex_Invalid )   return kObjectID_Invalid;//アプリ終了等のタイミングによってはcurrentTabIndexがInvalidの場合がある
	return NVI_GetDocumentIDByTabIndex(currentTabIndex);
}

ABool	AWindow::NVI_HasCloseButton() const
{
	return GetImpConst().HasCloseButton();
}

ABool	AWindow::NVI_HasTitleBar() const
{
	return GetImpConst().HasTitleBar();
}

ABool	AWindow::NVI_IsFloating() const
{
	return GetImpConst().IsFloatingWindow();
}

ABool	AWindow::NVI_IsModal() const
{
	return GetImpConst().IsModalWindow();
}

ABool	AWindow::NVI_IsResizable() const
{
	return GetImpConst().IsResizable();
}

ABool	AWindow::NVI_IsZoomable() const
{
	return GetImpConst().IsZoomable();
}

ABool	AWindow::NVI_IsZoomed() const
{
	return GetImpConst().IsZoomed();
}

//#673
/**
ドキュメントウインドウかどうかを取得
（＝フローティングではない、かつ、オーバーレイではない）
*/
ABool	AWindow::NVI_IsDocumentWindow() const
{
	//派生クラスでドキュメントウインドウと定義されている場合はそちらを優先 #688
	if( NVIDO_IsDocumentWindow() == true )
	{
		return true;
	}
	//フローティングではない、かつ、オーバーレイではないならtrueを返す
	return ((NVI_IsFloating()==false)&&(NVM_GetOverlayMode()==false));
}

//win
/**
*/
void	AWindow::NVI_GetWindowBoundsInStandardState( ARect& outBounds ) const
{
	GetImpConst().GetWindowBoundsInStandardState(outBounds);
}

//B0395
ABool	AWindow::NVI_IsWindowCollapsed() const
{
	return GetImpConst().IsWindowCollapsed();
}

/*
#291 子ウインドウのセクションに移動
//B0411
ABool	AWindow::NVI_IsChildWindowVisible() const
{
	if( GetImpConst().ChildWindowVisible() == true )   return true;
	if( NVI_IsPrintMode() == true )   return true;
	if( NVIDO_IsChildWindowVisible() == true )   return true;
	return false;
}
*/

//#182
/**
印刷中かどうかを取得
*/
ABool	AWindow::NVI_IsPrintMode() const
{
	return GetImpConst().IsPrintMode();
}

//#175
//ウインドウを巡回可能にする
void	AWindow::NVI_AddToRotateArray()
{
	AApplication::GetApplication().NVI_AddToWindowRotateArray(GetObjectID());
}

/**
ウインドウをタイマーアクション対象にする
*/
void	AWindow::NVI_AddToTimerActionWindowArray()
{
	AApplication::GetApplication().NVI_AddToTimerActionWindowArray(GetObjectID());
}

//#199
/**
ウインドウをAllClose対象にする
*/
void	AWindow::NVI_AddToAllCloseTargetWindowArray()
{
	AApplication::GetApplication().NVI_AddToAllCloseTargetWindowArray(GetObjectID());
}

//#182
/**
ウインドウ透明度設定
*/
void	AWindow::NVI_SetWindowTransparency( const AFloatNumber inTransparency )
{
	if( NVM_GetOverlayMode() == false )
	{
		GetImp().SetWindowTransparency(inTransparency);
	}
	else
	{
#if IMPLEMENTATION_FOR_WINDOWS
		//win Windowsの場合、Overlayウインドウはウインドウ全体（backgroundウインドウ）に透明度を設定できるようにする
		//Macの場合は、ViewでPaint時にAlpha描画する
		if( mOverlayTransparent == true )//falseの場合は常に非透過
		{
			GetImp().SetWindowTransparency(inTransparency);
		}
#else
		//#1255
		GetImp().SetWindowTransparency(inTransparency);
#endif
	}
}

//#182
/**
ウインドウDirty設定
*/
void	AWindow::NVI_SetModified( const ABool inModified )
{
	GetImp().SetModified(inModified);
}

//#182
/**
ウインドウ内の全Controlが￥キーをbackslashに変換するどうかを設定
*/
void	AWindow::NVI_SetInputBackslashByYenKeyMode( const ABool inMode )
{
	GetImp().SetInputBackslashByYenKeyMode(inMode);
	for( AIndex i = 0; i < mViewControlIDArray.GetItemCount(); i++ )
	{
		NVI_GetViewByControlID(mViewControlIDArray.Get(i)).NVI_SetInputBackslashByYenKeyMode(inMode);
	}
}

//#182
/**
TabControlの値を設定
*/
void	AWindow::NVI_SelectTabControl( const AIndex inIndex )
{
	GetImp().SetTabControlValue(inIndex);
}

//win
/**
ウインドウの指定領域を塗りつぶす
*/
void	AWindow::NVI_PaintRect( const AWindowRect& inRect, const AColor& inColor )
{
	GetImp().PaintRect(inRect,inColor);
}

#pragma mark ===========================

#pragma mark ---Control制御
//#137

/**
ControlのShow/Hide
*/
void	AWindow::NVI_SetShowControl( const AControlID inControlID, const ABool inShow )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetShowControl(inControlID,inShow);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetShow(inShow);
	}
}

//#246
/**
ContolのShow/Hide（ただし、inTabIndexが現在のタブの場合のみ処理を行う）
*/
void	AWindow::NVI_SetShowControlIfCurrentTab( const AIndex inTabIndex, const AControlID inControlID, const ABool inShow )
{
	if( inTabIndex == NVI_GetCurrentTabIndex() )
	{
		NVI_SetShowControl(inControlID,inShow);
	}
}

//#246
/**
Controlの位置設定
*/
void	AWindow::NVI_SetControlPosition( const AControlID inControlID, const AWindowPoint& inPoint )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetControlPosition(inControlID,inPoint);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetPosition(inPoint);
	}
}

//#246
/**
Controlのサイズ設定
*/
void	AWindow::NVI_SetControlPositionIfCurrentTab( const AIndex inTabIndex, const AControlID inControlID, const AWindowPoint& inPoint )
{
	if( inTabIndex == NVI_GetCurrentTabIndex() )
	{
		NVI_SetControlPosition(inControlID,inPoint);
	}
}

//#246
/**
Controlのサイズ設定
*/
void	AWindow::NVI_SetControlSize( const AControlID inControlID, const ANumber inWidth, const ANumber inHeight )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetControlSize(inControlID,inWidth,inHeight);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetSize(inWidth,inHeight);
	}
}

//#246
/**
Controlのサイズ設定
*/
void	AWindow::NVI_SetControlSizeIfCurrentTab( const AIndex inTabIndex, const AControlID inControlID, const ANumber inWidth, const ANumber inHeight )
{
	if( inTabIndex == NVI_GetCurrentTabIndex() )
	{
		NVI_SetControlSize(inControlID,inWidth,inHeight);
	}
}

//#135
/**
Controlの中身を全選択
*/
void	AWindow::NVI_SetControlSelectAll( const AControlID inControlID )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetSelectionAll(inControlID);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetSelectAll();
	}
}

//#182
/**
コントロールEnable/Disable
*/
void	AWindow::NVI_SetControlEnable( const AControlID inControlID, const ABool inEnable )
{
	//enable/disableに変更が無ければ、何もしない
	if( NVI_IsControlEnable(inControlID) == inEnable )
	{
		return;
	}
	//
	if( IsView(inControlID) == false )
	{
		GetImp().SetEnableControl(inControlID,inEnable);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetEnabled(inEnable);
	}
	//#349
	if( NVI_GetCurrentFocus() == inControlID && inEnable == false )
	{
		NVI_SwitchFocusToNext();
	}
}

//#182
/**
ControlがRerturnキーを処理するかどうかを設定
*/
void	AWindow::NVI_SetCatchReturn( const AControlID inControlID, const ABool inCatch )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetCatchReturn(inControlID,inCatch);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetCatchReturn(inCatch);
	}
}

//#182
/**
ControlがTabキーを処理するかどうかを設定
*/
void	AWindow::NVI_SetCatchTab( const AControlID inControlID, const ABool inCatch )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetCatchTab(inControlID,inCatch);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetCatchTab(inCatch);
	}
}

//#182
/**
Controlの位置取得
*/
void	AWindow::NVI_GetControlPosition( const AControlID inID, AWindowPoint& outPoint ) const
{
	GetImpConst().GetControlPosition(inID,outPoint);
}

//#182
/**
Controlの幅取得
*/
ANumber	AWindow::NVI_GetControlWidth( const AControlID inID ) const
{
	return GetImpConst().GetControlWidth(inID);
}

//#182
/**
Controlの高さ取得
*/
ANumber	AWindow::NVI_GetControlHeight( const AControlID inID ) const
{
	return GetImpConst().GetControlHeight(inID);
}

//#182
/**
ControlがVisibleかどうかを取得
*/
ABool	AWindow::NVI_IsControlVisible( const AControlID inControlID ) const
{
	return GetImpConst().IsControlVisible(inControlID);
}

//#182
/**
ControlがEnableかどうかを取得
*/
ABool	AWindow::NVI_IsControlEnable( const AControlID inControlID ) const
{
	return GetImpConst().IsControlEnabled(inControlID);
}

//#182
/**
Controlが含まれるTabControlのIndexを取得
*/
AIndex	AWindow::NVI_GetControlEmbeddedTabControlIndex( const AControlID inID ) const
{
	return GetImpConst().GetControlEmbeddedTabIndex(inID);
}

//#182
/**
フォーカス移動で全選択するかどうかを設定
*/
void	AWindow::NVI_SetAutomaticSelectBySwitchFocus( const AControlID inControlID, const ABool inSelect )
{
	if( IsView(inControlID) == true )
	{
		NVI_GetViewByControlID(inControlID).NVI_SetAutomaticSelectBySwitchFocus(inSelect);
	}
	else
	{
		GetImp().SetAutoSelectAllBySwitchFocus(inControlID,inSelect);
	}
}

/**
ビューの内容自動選択を実行
*/
void	AWindow::NVI_AutomaticSelectBySwitchFocus( const AControlID inControlID )
{
	if( IsView(inControlID) == true )
	{
		NVI_GetViewByControlID(inControlID).NVI_AutomaticSelectBySwitchFocus();
	}
}

//#182
/**
Controlのアイコンを設定する
*/
void	AWindow::NVI_SetControlIcon( const AControlID inControlID, const AIconID inIconID,
		const ABool inDefaultSize, const ANumber inLeftOffset, const ANumber inTopOffset )
{
	GetImp().SetIcon(inControlID,inIconID,inDefaultSize,inLeftOffset,inTopOffset);
}

//#182
/**
Controlのメニューを設定する
*/
void	AWindow::NVI_SetMenuItemsFromTextArray( const AControlID inID, const ATextArray& inTextArray )
{
	//現在選択中の項目のテキストを記憶 #1316
	AText	text;
	NVI_GetControlText(inID,text);
	//
	GetImp().SetMenuItemsFromTextArray(inID,inTextArray);
	//記憶していたテキストを設定 #1316
	NVI_SetControlText(inID,text);
}

//win
/**
Controlのメニューに項目を追加する
*/
void	AWindow::NVI_InsertMenuItem( const AControlID inID, const AIndex inMenuItemIndex, const AText& inText )
{
	GetImp().InsertMenuItem(inID,inMenuItemIndex,inText);
}

//win
/**
Controlのメニューの項目を削除する
*/
void	AWindow::NVI_DeleteMenuItem( const AControlID inID, const AIndex inMenuItemIndex )
{
	GetImp().DeleteMenuItem(inID,inMenuItemIndex);
}

//#182
/**
Control表示更新
*/
void	AWindow::NVI_DrawControl( const AControlID inID )
{
	GetImp().DrawControl(inID);
}

//#182
/**
Control親子設定
*/
void	AWindow::NVI_EmbedControl( const AControlID inParent, const AControlID inChild )
{
	//ViewにFrameViewが存在する場合はFrameViewのほうをinParentにembedする #135
	AControlID	frameControlID = kControlID_Invalid;
	if( IsView(inChild) == true )
	{
		frameControlID = NVI_GetViewByControlID(inChild).NVI_GetFrameViewControlID();
	}
	if( frameControlID != kControlID_Invalid )
	{
		if( frameControlID != inParent )
		{
			GetImp().EmbedControl(inParent,frameControlID);
		}
		else
		{
			GetImp().EmbedControl(inParent,inChild);
		}
	}
	else
	{
		GetImp().EmbedControl(inParent,inChild);
	}
}

//#182
/**
Controlの最小最大値の設定
*/
void	AWindow::NVI_SetMinMax( const AControlID inControlID, const ANumber inMin, const ANumber inMax )
{
	GetImp().SetMinMax(inControlID,inMin,inMax);
}

//#182
/**
FontMenuの登録
*/
void	AWindow::NVI_RegisterFontMenu( const AControlID inID, const ABool inEnableDefaultFontItem )//#375
{
	GetImp().RegisterFontMenu(inID,inEnableDefaultFontItem);//#375
}

//#182
/**
FontMenuの登録
*/
void	AWindow::NVI_RegisterTextArrayMenu( const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID )
{
	GetImp().RegisterTextArrayMenu(inControlID,inTextArrayMenuID);
}

//#182
/**
FontMenuの登録
*/
void	AWindow::NVI_UnregisterTextArrayMenu( const AControlID inControlID )
{
	GetImp().UnregisterTextArrayMenu(inControlID);
}

//#182
/**
ControlにHelpタグ/Tooltip設定
*/
void	AWindow::NVI_SetHelpTag( const AControlID inID, const AText& inText, const AHelpTagSide inTagSide )//#634
{
	//#688 GetImp().SetHelpTag(inID,inText,inTagSide);//#634
	AIndex	index = mToolTipTextArray_ControlID.Find(inID);
	if( index == kIndex_Invalid )
	{
		mToolTipTextArray_ControlID.Add(inID);
		mToolTipTextArray_ToolTipText.Add(inText);
		mToolTipTextArray_HelpTagSide.Add(inTagSide);
	}
	else
	{
		mToolTipTextArray_ControlID.Set(index,inID);
		mToolTipTextArray_ToolTipText.Set(index,inText);
		mToolTipTextArray_HelpTagSide.Set(index,inTagSide);
	}
}

//#182
/**
ControlのHelpタグマウスオーバー時にCallback関数を呼ぶようにする（エリア毎に表示内容を変える場合用）
*/
void	AWindow::NVI_EnableHelpTagCallback( const AControlID inID, const ABool inShowOnlyWhenNarrow )//#507
{
	//#688 GetImp().EnableHelpTagCallback(inID,inShowOnlyWhenNarrow);//#507
}

//#182
/**
HelpタグをHide
*/
void	AWindow::NVI_HideHelpTag() const
{
	//#688 GetImpConst().HideHelpTag();
	//ツールチップウインドウ非表示
	AApplication::GetApplication().NVI_GetToolTipWindow().NVI_Hide();
}

//#182
/**
Undo情報をクリア
*/
void	AWindow::NVI_ClearControlUndoInfo( const AControlID inControlID )
{
	if( IsView(inControlID) == true )
	{
		NVI_GetViewByControlID(inControlID).NVI_ClearUndoInfo();
	}
	else
	{
		GetImp().ResetUndoStack(inControlID);
	}
}

//#291
/**
コントロールのZ位置をTopに設定する
*/
void	AWindow::NVI_SetControlZOrderToTop( const AControlID inID )
{
	GetImp().SetControlZOrderToTop(inID);
}

//#291
/**
Bindings設定
*/
void	AWindow::NVI_SetControlBindings( const AControlID inControlID,
		const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
		const ABool inWidthFixed, const ABool inHeightFixed )//#688
{
	if( IsView(inControlID) == true )//#455
	{
		NVI_GetViewByControlID(inControlID).NVI_SetControlBindings(
					inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
	}
	else
	{
		GetImp().SetControlBindings(inControlID,
					inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
	}
}

//win
/**
*/
void	AWindow::NVI_DisableEventCatch( const AControlID inControlID )
{
	GetImp().DisableEventCatch(inControlID);
}

//コントロールの削除
void	AWindow::NVI_DeleteControl( const AControlID inControlID )
{
	//#243 タブへの登録（mTabDataArray_ControlIDs）未削除なら、登録削除する
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_ControlIDs.GetItemCount(); tabIndex++ )
	{
		if( mTabDataArray_ControlIDs.GetObject(tabIndex).Find(inControlID) != kIndex_Invalid )
		{
			NVM_UnregisterViewInTab(tabIndex,inControlID);
			break;
		}
	}
	//#135 フォーカスグループから削除     win 下のIsView()判定内から移動
	NVI_UnregisterFromFocusGroup(inControlID);
	//
	if( IsView(inControlID) == false )
	{
		//AViewは存在しない場合
		GetImp().DeleteControl(inControlID);
	}
	else
	{
		//AView存在する場合
		DeleteView(inControlID);
	}
	//LatentFocusクリア
	if( mLatentFocus == inControlID )
	{
		mLatentFocus = kControlID_Invalid;
	}
}

//#565
/**
コントロールをFileDrop可能に設定
*/
void	AWindow::NVI_EnableFileDrop( const AControlID inControlID )
{
	GetImp().EnableFileDrop(inControlID);
}

//#182
/**
ViewImp取得
*/
CPaneForAView&	AWindow::NVI_GetViewImp( const AControlID inControlID )
{
	return dynamic_cast<CPaneForAView&>(GetImp().GetUserPane(inControlID));
}

#pragma mark ===========================

#pragma mark ---座標変換
//#182

/**
LocalPoint→WindowPoint
*/
void	AWindow::NVI_GetWindowPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AWindowPoint& outWindowPoint ) const
{
	GetImpConst().GetWindowPointFromControlLocalPoint(inID,inLocalPoint,outWindowPoint);
}

/**
LocalRect→WindowRect
*/
void	AWindow::NVI_GetWindowRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AWindowRect& outWindowRect ) const
{
	GetImpConst().GetWindowRectFromControlLocalRect(inID,inLocalRect,outWindowRect);
}

/**
WindowPoint→LocalPoint
*/
void	AWindow::NVI_GetControlLocalPointFromWindowPoint( const AControlID inID, const AWindowPoint& inWindowPoint, ALocalPoint& outLocalPoint ) const
{
	GetImpConst().GetControlLocalPointFromWindowPoint(inID,inWindowPoint,outLocalPoint);
}

/**
WindowRect→LocalRect
*/
void	AWindow::NVI_GetControlLocalRectFromWindowRect( const AControlID inID, const AWindowRect& inWindowRect, ALocalRect& outLocalRect ) const
{
	GetImpConst().GetControlLocalRectFromWindowRect(inID,inWindowRect,outLocalRect);
}

/**
LocalPoint→GlobalPoint
*/
void	AWindow::NVI_GetGlobalPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AGlobalPoint& outGlobalPoint ) const
{
	GetImpConst().GetGlobalPointFromControlLocalPoint(inID,inLocalPoint,outGlobalPoint);
}

/**
LocalRect→GlobalRect
*/
void	AWindow::NVI_GetGlobalRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AGlobalRect& outGlobalRect ) const
{
	GetImpConst().GetGlobalRectFromControlLocalRect(inID,inLocalRect,outGlobalRect);
}

//#182
/**
*/
void	AWindow::NVI_DragWindow( const AControlID inControlID, const ALocalPoint& inMouseLocalPoint )
{
	GetImp().DragWindow(inControlID,inMouseLocalPoint);
}

#pragma mark ===========================

#pragma mark ---StickyWindow制御
//#138

//#138
/**
StickyWindowにする（子側からコール）
*/
void	AWindow::NVI_SetAsStickyWindow( const AWindowID inParent )
{
	mParentWindowIDForStickyWindow = inParent;
	//親ウインドウに登録
	AApplication::GetApplication().NVI_GetWindowByID(mParentWindowIDForStickyWindow).NVI_RegisterChildStickyWindow(GetObjectID());
}

//#138
/**
子StickyWindowを登録する
*/
void	AWindow::NVI_RegisterChildStickyWindow( const AWindowID inChild )
{
	//子ウインドウのWindowID, Offsetを登録
	mChildStickyWindowArray_WindowID.Add(inChild);
	AWindowPoint	pt = {0,0};
	mChildStickyWindowArray_Offset.Add(pt);
}

//#138
/**
子StickyWindowの登録を解除する
*/
void	AWindow::NVI_UnregisterChildStickyWindow( const AWindowID inChild )
{
	AIndex	index = mChildStickyWindowArray_WindowID.Find(inChild);
	if( index == kIndex_Invalid )   return;
	
	//子ウインドウの登録を削除
	mChildStickyWindowArray_WindowID.Delete1(index);
	mChildStickyWindowArray_Offset.Delete1(index);
}

//#138
/**
子Stickyウインドウの位置設定

@param inChild 子ウインドウのWindowID
@param inOffset 子ウインドウのオフセット位置（親ウインドウの左上からの相対位置）
*/
void	AWindow::NVI_SetChildStickyWindowOffset( const AWindowID inChild, const AWindowPoint& inOffset )
{
	AIndex	index = mChildStickyWindowArray_WindowID.Find(inChild);
	if( index == kIndex_Invalid )   { _ACError("child sticky window not found",this); return; }
	
	//子ウインドウの位置情報を更新
	mChildStickyWindowArray_Offset.Set(index,inOffset);
	
	//子ウインドウの位置設定
	APoint	gpt;
	NVI_GetWindowPosition(gpt);
	gpt.x += inOffset.x;
	gpt.y += inOffset.y;
	AApplication::GetApplication().NVI_GetWindowByID(inChild).NVI_SetWindowPosition(gpt);
}

//#138
/**
全ての子Stickyウインドウの位置を更新
*/
void	AWindow::UpdateStickyWindowOffset()
{
	//親ウインドウの位置に追随して、全ての子ウインドウを移動
	APoint	parentgpt;
	NVI_GetWindowPosition(parentgpt);
	for( AIndex i = 0; i < mChildStickyWindowArray_WindowID.GetItemCount(); i++ )
	{
		//オフセット位置取得
		AWindowPoint	offset = mChildStickyWindowArray_Offset.Get(i);
		//親ウインドウの左上からの相対位置を取得
		APoint	childgpt = parentgpt;
		childgpt.x += offset.x;
		childgpt.y += offset.y;
		//子ウインドウの位置設定
		AApplication::GetApplication().NVI_GetWindowByID(mChildStickyWindowArray_WindowID.Get(i)).NVI_SetWindowPosition(childgpt);
	}
}

#pragma mark ===========================

#pragma mark ---オーバーレイウインドウ制御
//#291

/**
オーバーレイモードに設定する
*/
void	AWindow::NVI_ChangeToOverlay( const AWindowID inParentWindowID, const ABool inTransparent )//win
{
	//
	mOverlayParentWindowID = inParentWindowID;
	mOverlayTransparent = inTransparent;//win
	//
	mOverlayMode = true;
	//#379
	//親ウインドウ内位置データ初期化
	mOverlayCurrentOffset.x = 0;
	mOverlayCurrentOffset.y = 0;
	mOverlayCurrentWidth = 0;
	mOverlayCurrentHeight = 0;
	//親ウインドウ取得
	AWindowRef	parentWindowRef = NULL;
	if( mOverlayParentWindowID != kObjectID_Invalid )
	{
		parentWindowRef = AApplication::GetApplication().NVI_GetWindowByID(mOverlayParentWindowID).NVI_GetWindowRef();
	}
	//オーバーレイモードに設定
	if( NVI_IsWindowCreated() == true )
	{
		GetImp().ChangeToOverlayWindow(parentWindowRef);
	}
	//ウインドウタイプによってアルファ値が違ったりするので、それを反映させる
	NVI_UpdateProperty();
	/*
	//
	if( NVM_GetOverlayMode() == false )
	{
		//
		SetOverlayMode(true);
		//オーバーレイウインドウ登録はNVI_Createで実行される
	}
	*/
}

/**
オーバーレイモードを解除する
*/
void	AWindow::NVI_ChangeToFloating()
{
	//
	mOverlayParentWindowID = kObjectID_Invalid;
	mOverlayTransparent = false;//win
	//
	mOverlayMode = false;
	//#379
	//親ウインドウ内位置データ初期化
	mOverlayCurrentOffset.x = 0;
	mOverlayCurrentOffset.y = 0;
	mOverlayCurrentWidth = 0;
	mOverlayCurrentHeight = 0;
	//フローティングウインドウモードに設定
	if( NVI_IsWindowCreated() == true )
	{
		GetImp().ChangeToFloatingWindow();
	}
	//ウインドウタイプによってアルファ値が違ったりするので、それを反映させる
	NVI_UpdateProperty();
	/*
	if( NVM_GetOverlayMode() == true )
	{
		SetOverlayMode(false);
		//オーバーレイウインドウ登録はSetOverlayMode()内でのウインドウ削除により自動的に解除される
	}
	*/
}

/**
オーバーレイモードを設定
*/
void	AWindow::SetOverlayMode( const ABool inOverlayMode )
{
	if( mOverlayMode != inOverlayMode )
	{
		//#379
		mOverlayCurrentOffset.x = 0;
		mOverlayCurrentOffset.y = 0;
		mOverlayCurrentWidth = 0;
		mOverlayCurrentHeight = 0;
		//
		if( NVI_IsWindowCreated() == true )
		{
			//Tabデータを記憶
			AArray<ADocumentID>	svTabDataArray_DocumentID;
			for( AIndex i = 0; i < mTabDataArray_DocumentID.GetItemCount(); i++ )
			{
				svTabDataArray_DocumentID.Add(mTabDataArray_DocumentID.Get(i));
			}
			//表示状態を記憶
			//ABool	visible = NVI_IsWindowVisible();
			//ウインドウを閉じる
			NVI_Close();
			//オーバーレイモード変更
			mOverlayMode = inOverlayMode;
			//各Tabを生成
			//NVI_Create()は最初のNVI_CreateTab()内でコールされる
			for( AIndex i = 0; i < svTabDataArray_DocumentID.GetItemCount(); i++ )
			{
				NVI_CreateTab(svTabDataArray_DocumentID.Get(i));
			}
			//表示状態を戻す
			/*if( visible == true )
			{
				NVI_Show();
			}*/
		}
		else
		{
			mOverlayMode = inOverlayMode;
		}
	}
}

/**
オーバーレイウインドウの親ウインドウ内でのオフセット位置を設定
*/
void	AWindow::NVI_SetOffsetOfOverlayWindow( const AWindowID inParentWindowID, const AWindowPoint& inOffset )
{
	if( NVM_GetOverlayMode() == false )   { _ACError("not in OverlayMode",this); return; }
	//#379 処理高速化。前と同じ位置なら何もしない
	if( inOffset.x == mOverlayCurrentOffset.x && inOffset.y == mOverlayCurrentOffset.y )   return;
	//
	GetImp().SetOverlayWindowMoveTogether(false);
	//
	APoint	gpt;
	AApplication::GetApplication().NVI_GetWindowByID(inParentWindowID).NVI_GetWindowPosition(gpt);
	gpt.x += inOffset.x;
	gpt.y += inOffset.y;
	NVI_SetWindowPosition(gpt);
	//
	GetImp().SetOverlayWindowMoveTogether(true);
	//#379
	mOverlayCurrentOffset = inOffset;
}

/**
オーバーレイウインドウの親ウインドウ内でのオフセット位置を設定
*/
void	AWindow::NVI_SetOffsetOfOverlayWindowAndSize( const AWindowID inParentWindowID, 
		const AWindowPoint& inOffset, const ANumber inWidth, const ANumber inHeight, const ABool inAnimate )
{
	if( NVM_GetOverlayMode() == false )   { _ACError("not in OverlayMode",this); return; }
	//
	//#379 処理高速化。前と同じ位置・サイズなら何もしない
	//#688 if( inOffset.x == mOverlayCurrentOffset.x && inOffset.y == mOverlayCurrentOffset.y &&
	//#688inWidth == mOverlayCurrentWidth && inHeight == mOverlayCurrentHeight )   return;
	GetImp().SetOverlayWindowMoveTogether(false);
	//
	APoint	gpt;
	AApplication::GetApplication().NVI_GetWindowByID(inParentWindowID).NVI_GetWindowPosition(gpt);
	gpt.x += inOffset.x;
	gpt.y += inOffset.y;
	ARect	grect;
	grect.left		= gpt.x;
	grect.top		= gpt.y;
	grect.right		= gpt.x + inWidth;
	grect.bottom	= gpt.y + inHeight;
	GetImp().SetWindowBounds(grect,inAnimate);
	//
	GetImp().SetOverlayWindowMoveTogether(true);
	//#379
	mOverlayCurrentOffset = inOffset;
	mOverlayCurrentWidth = inWidth;
	mOverlayCurrentHeight = inHeight;
}

//#320
/**
オーバーレイウインドウを指定ウインドウの背後に移動
*/
void	AWindow::NVI_SendOverlayWindowBehind( const AWindowID inWindowID )
{
	if( NVM_GetOverlayMode() == false )   { _ACError("not in OverlayMode",this); return; }
	//Layer固定解除
	GetImp().SetOverlayLayerTogether(false);
	//背後に移動
	NVI_SendBehind(inWindowID);
	//Layer固定
	GetImp().SetOverlayLayerTogether(true);
}

//#688
/**
オーバーレイウインドウを指定ウインドウの前面に移動
*/
void	AWindow::NVI_SendOverlayWindowAbove( const AWindowID inWindowID )
{
	if( NVM_GetOverlayMode() == false )   { _ACError("not in OverlayMode",this); return; }
	//Layer固定解除
	GetImp().SetOverlayLayerTogether(false);
	//背後に移動
	NVI_SendAbove(inWindowID);
	//Layer固定
	GetImp().SetOverlayLayerTogether(true);
}

//#422
/**
Spaces問題対策用
*/
void	AWindow::NVI_SelectWindowForSpacesProblemCountermeasure()
{
	//Group解除
	GetImp().SetOverlayWindowMoveTogether(false);
	//ウインドウ選択
	NVI_SelectWindow();
	//Group設定
	GetImp().SetOverlayWindowMoveTogether(true);
}

#pragma mark ===========================

#pragma mark ---子オーバーレイウインドウ管理
//#725

/**
子オーバーレイウインドウ登録
*/
void	AWindow::NVM_RegisterOverlayWindow( const AWindowID inWindowID, const AIndex inTabIndex, const ABool inLatentVisible )
{
	//データ追加
	mChildOverlayWindowArray_WindowID.Add(inWindowID);
	mChildOverlayWindowArray_TabIndex.Add(inTabIndex);
	mChildOverlayWindowArray_LatentVisible.Add(inLatentVisible);
	//show/hide制御
	NVI_SetOverlayWindowVisibility(inWindowID,inLatentVisible);
}

//#725
/**
子オーバーレイウインドウ登録解除
*/
void	AWindow::NVM_UnregisterOverlayWindow( const AWindowID inWindowID )
{
	//検索
	AIndex	index = mChildOverlayWindowArray_WindowID.Find(inWindowID);
	if( index == kIndex_Invalid )   _ACThrow("",this);
	
	//データ削除
	mChildOverlayWindowArray_WindowID.Delete1(index);
	mChildOverlayWindowArray_TabIndex.Delete1(index);
	mChildOverlayWindowArray_LatentVisible.Delete1(index);
}

//#725
/**
子オーバーレイウインドウshow/hide
*/
void	AWindow::NVI_SetOverlayWindowVisibility( const AWindowID inWindowID, const ABool inShow )
{
	//検索
	AIndex	index = mChildOverlayWindowArray_WindowID.Find(inWindowID);
	if( index == kIndex_Invalid )   _ACThrow("",this);
	
	//現在の表示タブと一致していて、inShowがtrueなら、showする。それ以外はhide。
	AWindowID	winID = mChildOverlayWindowArray_WindowID.Get(index);
	AIndex	tabIndex = mChildOverlayWindowArray_TabIndex.Get(index);
	if( inShow == true && (tabIndex == kIndex_Invalid || tabIndex == NVI_GetCurrentTabIndex()) )
	{
		AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_Show(false);
	}
	else
	{
		AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_Hide();
	}
	//latent visible状態を設定
	mChildOverlayWindowArray_LatentVisible.Set(index,inShow);
}

#pragma mark ===========================

#pragma mark ---ウインドウタイトル設定／取得

//ウインドウタイトル設定（対応するドキュメントが存在しない場合用）
void	AWindow::NVI_SetDefaultTitle( const AText& inText )
{
	mWindowDefaultTitle.SetText(inText);
	GetImp().SetTitle(inText);
}

//ウインドウタイトル取得
void	AWindow::NVI_GetTitle( const AIndex inTabIndex, AText& outTitle ) const
{
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	if( docID == kObjectID_Invalid )
	{
		outTitle.SetText(mWindowDefaultTitle);
	}
	else
	{
		AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_GetTitle(outTitle);
	}
}

//現在のウインドウタイトル取得
void	AWindow::NVI_GetCurrentTitle( AText& outTitle ) const
{
	NVI_GetTitle(NVI_GetCurrentTabIndex(),outTitle);
}

//タイトルバー表示の更新
void	AWindow::NVIDO_UpdateTitleBar()//#699
{
	//win 080726
	//mFixTitleがtrueのときは最初に決めたタイトルのままにする（ドキュメントタイトルに合わせない）
	if( mFixTitle == true )   return;
	if( NVI_GetCurrentTabIndex() == kIndex_Invalid )   return;//#379
	//
	AText	text;
	NVI_GetTitle(NVI_GetCurrentTabIndex(),text);
	GetImp().SetTitle(text);
	
	ADocumentID	docID = NVI_GetCurrentDocumentID();
	if( docID == kObjectID_Invalid )   return;
	
	//ReadOnly
	if( AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_IsReadOnly() == true )
	{
		AText	text;
		NVI_GetTitle(NVI_GetCurrentTabIndex(),text);
		AText	ro;
		ro.SetLocalizedText("ReadOnly");
		text.AddText(ro);
		GetImp().SetTitle(text);
	}
	
	//ファイル設定済み、かつ、スクリーニング完了済みならウインドウファイル設定
	if( AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_IsFileSpecifiedAndScreened() == true )
	{
		AFileAcc	file;
		AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_GetFile(file);
		GetImp().SetWindowFile(file);
	}
	else
	{
		GetImp().ClearWindowFile();
	}
	NVI_SetModified(AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_IsDirty());
	//#699 NVIDO_UpdateTitleBar();
}

#pragma mark ===========================

#pragma mark 共通ショートカットキー用設定

#pragma mark ===========================

//#135
/**
デフォルトキャンセルボタン設定（ESCキー押下時にClickとみなされるControl）
*/
void	AWindow::NVI_SetDefaultCancelButton( const AControlID inControlID )
{
	mDefaultCancelButton = inControlID;
	GetImp().SetDefaultCancelButton(inControlID);
}

//#135
/**
デフォルトOKボタン設定（retrunキー押下時にClickとみなされるControl）
*/
void	AWindow::NVI_SetDefaultOKButton( const AControlID inControlID )
{
	mDefaultOKButton = inControlID;
	GetImp().SetDefaultOKButton(inControlID);
}

#pragma mark ===========================

#pragma mark <所有クラス(AView)生成／削除／取得>

#pragma mark ===========================

/*#271 View生成時にControlIDが確定していないとmViewImpを設定できないのでこのメソッドによるView生成はしないことにする
//#92
**
View生成（まだリソース上にViewが存在していない場合用。ControlIDはモジュール内で生成する）

*
AViewID	AWindow::NVI_CreateView( AViewFactory& inFactory, 
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight, const ABool inSupportFocus, 
		const AIndex inTabControlIndex *【注意】"TabControl"のIndex*, const ABool inFront )
{
	//ControlID生成
	AControlID	controlID = NVM_AssignDynamicControlID();
	//Viewに対応する実体を生成
	GetImp().CreatePaneForAView(controlID,inPosition,inWidth,inHeight,inSupportFocus,inTabControlIndex,inFront);
	//#135
	if( inSupportFocus == true )
	{
		NVI_RegisterToFocusGroup(controlID);
	}
	//View生成
	AViewID	viewID = AApplication::GetApplication().NVI_CreateView(inFactory);
	mViewIDArray.Add(viewID);
	mViewControlIDArray.Add(controlID);
	//ViewへのControlID設定と、初期化
	AApplication::GetApplication().NVI_GetViewByID(viewID).NVI_SetControlID(controlID);
	AApplication::GetApplication().NVI_GetViewByID(viewID).NVI_Init();
	return viewID;
}
*/

/*win
void	AWindow::NVI_DeleteViewByViewID( const AViewID inViewID )
{
	//ViewIDから検索
	AIndex	viewIndex = mViewIDArray.Find(inViewID);
	if( viewIndex == kIndex_Invalid )   _ACThrow("no view found",this);
	AControlID	controlID = mViewControlIDArray.Get(viewIndex);
	NVM_DeleteView(controlID);//#135
}
*/

//View生成
//まだリソース上にViewが存在していない場合用
//inTabControlIndex: OS APIで生成するタブコントロールのどのタブに入れるか（AWindowのタブではないので注意）
//inFront: 生成したViewを最前面にするかどうか
AViewID	AWindow::NVM_CreateView( const AControlID inControlID, 
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight, 
		const AControlID inParentControlID, const AControlID inSiblingControlID,//#688
		const ABool /*#688inSupportFocus*/inSupportInputMethod, AViewFactory& inFactory,
		const AIndex inTabControlIndex /*【注意】"TabControl"のIndex*/, const ABool inFront )//win 080618
{
	GetImp().CreatePaneForAView(inControlID,inPosition,inWidth,inHeight,inParentControlID,inSiblingControlID,
				inTabControlIndex,inFront,inSupportInputMethod);//#688
	//#135
	if( /*#688 inSupportFocus*/inSupportInputMethod == true )
	{
		NVI_RegisterToFocusGroup(inControlID);
	}
	//win 080707 ViewControlIDArray.Add(inControlID);
	//#199 mViewArray.AddNewObject(inFactory);
	AViewID	viewID = AApplication::GetApplication().NVI_CreateView(inFactory);//#199
	mViewIDArray.Add(viewID);//#199
	//win 080618 mViewControlIDArray.Add(inControlID);上へ移動
	//（AView_xxxのコンストラクタからDoDrawが呼ばれる
	//→mViewControlIDArrayに登録がないとビューのDoDraw()実行されない
	//→Offscreenに描画されない
	//→Windows版ではOffscreenの描画チャンスは最初のWM_PAINTか、明示的Refresh以外ない
	//win 080707
	//更に変更
	//mViewArrayに登録されないとWindowsからのコールバック発生時にview参照が得られない。
	//AView_xxxのinit処理をわけて、mViewArray, mViewControlIDArrayの両方へ登録後にinit実行する
	mViewControlIDArray.Add(inControlID);
	NVI_GetViewByControlID(inControlID).NVI_Init();
	return viewID;//#199
}

//View生成
//既にリソース上にViewが存在している場合用
AViewID	AWindow::NVM_CreateView( const AControlID inControlID, const ABool /*#688inSupportFocus*/inSupportInputMethod, AViewFactory& inFactory )
{
	GetImp().RegisterPaneForAView(inControlID,inSupportInputMethod/*#135,inSupportFocus*/);//#688
	//#135
	if( /*#688 inSupportFocus*/inSupportInputMethod == true )
	{
		NVI_RegisterToFocusGroup(inControlID);
	}
	//#199 mViewArray.AddNewObject(inFactory);
	AViewID	viewID = AApplication::GetApplication().NVI_CreateView(inFactory);//#199
	mViewIDArray.Add(viewID);//#199
	mViewControlIDArray.Add(inControlID);
	NVI_GetViewByControlID(inControlID).NVI_Init();
	return viewID;//#199
}

//View削除
void	AWindow::DeleteView( const AControlID inControlID )
{
	AIndex	viewIndex = mViewControlIDArray.Find(inControlID);
	if( viewIndex == kIndex_Invalid )   _ACThrow("no view found",this);
	//
	//#199 mViewArray.Delete1Object(viewIndex);
	AApplication::GetApplication().NVI_DeleteView(mViewIDArray.Get(viewIndex));//#199
	mViewIDArray.Delete1(viewIndex);//#199
	mViewControlIDArray.Delete1(viewIndex);
	//#135 フォーカスグループから削除
	NVI_UnregisterFromFocusGroup(inControlID);
	//Viewに対応する実体を削除
	GetImp().DeleteControl(inControlID);
	/*#135
	//win 080618
	//フォーカスのあるビューを削除した場合は、フォーカスを移動
	if( mCurrentFocus == inControlID )
	{
		mCurrentFocus = kControlID_Invalid;
		NVI_SwitchFocusToFirst();
	}
	*/
	//LatentFocusクリア
	if( mLatentFocus == inControlID )
	{
		mLatentFocus = kControlID_Invalid;
	}
}

//View取得
AView&	AWindow::NVI_GetViewByControlID( const AControlID inControlID )
{
	AIndex	viewIndex = mViewControlIDArray.Find(inControlID);
	if( viewIndex == kIndex_Invalid )   _ACThrow("no view found",this);
	//#199 return mViewArray.GetObject(viewIndex);
	return AApplication::GetApplication().NVI_GetViewByID(mViewIDArray.Get(viewIndex));//#199
}
const AView&	AWindow::NVI_GetViewConstByControlID( const AControlID inControlID ) const
{
	AIndex	viewIndex = mViewControlIDArray.Find(inControlID);
	if( viewIndex == kIndex_Invalid )   _ACThrow("no view found",this);
	//#199 return mViewArray.GetObjectConst(viewIndex);
	return AApplication::GetApplication().NVI_GetViewConstByID(mViewIDArray.Get(viewIndex));//#199
}

/*#243 動的ControlIDは10000～固定にする
//動的割当ControlIDの範囲を設定
void	AWindow::NVM_SetDynamicControlIDRange( const AControlID inStart, const AControlID inEnd )
{
	mDynamicControlIDRangeStart = inStart;
	mDynamicControlIDRangeEnd = inEnd;
	mNextDynamicControlID = inStart;
}
*/

//動的割当ControlIDを割り当てる
AControlID	AWindow::NVM_AssignDynamicControlID()
{
	if( mDynamicControlIDRangeStart == kControlID_Invalid || mDynamicControlIDRangeEnd == kControlID_Invalid )
	{
		_ACThrow("dynamic control ID range is not set",this);
	}
	while( mViewControlIDArray.Find(mNextDynamicControlID) != kIndex_Invalid )
	{
		mNextDynamicControlID++;
		if( mNextDynamicControlID >= mDynamicControlIDRangeEnd )
		{
			mNextDynamicControlID = mDynamicControlIDRangeStart;
		}
	}
	AControlID	id = mNextDynamicControlID;
	mNextDynamicControlID++;
	return id;
}

/*#182
ABool	AWindow::ExistView( const AControlID inControlID )
{
	if( inControlID == kControlID_Invalid )   return false;
	return ( mViewControlIDArray.Find(inControlID) != kIndex_Invalid );
}
*/

#pragma mark ===========================

#pragma mark ---フォーカス制御

//#135
/**
フォーカスグループを設定
*/
void	AWindow::NVI_RegisterToFocusGroup( const AControlID inFocusControlID, 
		const ABool inOnly2ndPattern, const ABool inGroup )//#353
{
	//タブフォーカス移動1stパターンへ登録
	//#688 if( inOnly2ndPattern == false )//#353 フォーカス2ndパターン
	{
		mFocusGroup.Add(inFocusControlID);
		mFocusGroup_IsGroup.Add(false);
	}
	//タブフォーカス移動の2ndパターンへ登録（ポップアップメニュー・ボタン等もフォーカスあり） #353
	//#688 mFocusGroup2nd.Add(inFocusControlID);
	//#688 mFocusGroup2nd_IsGroup.Add(inGroup);
}

//#135
/**
フォーカスグループから削除
*/
void	AWindow::NVI_UnregisterFromFocusGroup( const AControlID inFocusControlID )
{
	//現在のフォーカスを取得
	AControlID	oldFocus = NVI_GetCurrentFocus();
	//削除
	AIndex	focusindex = mFocusGroup.Find(inFocusControlID);
	if( focusindex != kIndex_Invalid )
	{
		mFocusGroup.Delete1(focusindex);
		mFocusGroup_IsGroup.Delete1(focusindex);
	}
	/*#688
	//2ndパターン削除 #353
	AIndex	focusindex2nd = mFocusGroup2nd.Find(inFocusControlID);
	if( focusindex2nd != kIndex_Invalid )
	{
		mFocusGroup2nd.Delete1(focusindex2nd);
		mFocusGroup2nd_IsGroup.Delete1(focusindex2nd);
	}
	*/
	//フォーカスのあるビューを削除した場合は、フォーカスを解除
	/*#688
	if( inFocusControlID == oldFocus )
	{
		//#688 NVI_SwitchFocusToFirst();
		NVI_SwitchFocusTo(kControlID_Invalid);//#688
	}
	 */
}

//フォーカスを設定する
void	AWindow::NVI_SwitchFocusTo( const AControlID inFocusControlID, const ABool inReverseSearch )//#135 #688
{
	if( inFocusControlID == /*#135 mCurrentFocus*/NVI_GetCurrentFocus() )   return;
	
	//#353
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	
	//#135 
	//アクティブなタブ内でなければ、タブ内フォーカスのみを設定して終了する #212 終了はしない。タブ内フォーカスを設定して処理継続。
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_ControlIDs.GetItemCount(); tabIndex++ )
	{
		AIndex	index = mTabDataArray_ControlIDs.GetObject(tabIndex).Find(inFocusControlID);
		if( index != kIndex_Invalid )
		{
			mTabDataArray_LatentFocus.Set(tabIndex,inFocusControlID);//#212
			break;//#212
		}
	}
	//
	mLatentFocus = inFocusControlID;
	if( NVI_IsWindowVisible() == false )
	{
		return;
	}
	//フォーカス設定
	GetImp().SetFocus(inFocusControlID);
	
	//ビューの内容自動選択を実行
	NVI_AutomaticSelectBySwitchFocus(inFocusControlID);
	return;
	
#if 0
	//
	if( inFocusControlID != kControlID_Invalid )
	{
		AIndex	focusGroupIndex = focusGroup.Find(inFocusControlID);
		if( focusGroupIndex != kIndex_Invalid )
		{
			//inControlIDから順に、Enableかつ同じタブのコントロールが見つかるまで検索し、見つかったらそこへfocusを設定
			for( AIndex i = 0; i < focusGroup.GetItemCount(); i++ )
			{
				AControlID	controlID = focusGroup.Get(focusGroupIndex);
				if( NVI_IsControlEnable(controlID) == true && NVI_IsControlVisible(controlID) == true )
				{
					//フォーカス設定
					GetImp().SetFocus(controlID);
					//
					/*#135 if( IsView(controlID) == true )
					{
						NVI_GetViewByControlID(controlID).NVI_AutomaticSelectBySwitchFocus();
					}
					else
					{
						if( GetImp().IsAutoSelectAllBySwitchFocus(controlID) == true )
						{
							GetImp().SetSelectionAll(controlID);
						}
					}*/
					mLatentFocus = controlID;
					return;
				}
				if( inReverseSearch == false )
				{
					focusGroupIndex++;
					if( focusGroupIndex >= focusGroup.GetItemCount() )
					{
						focusGroupIndex = 0;
					}
				}
				else//B0429 フォーカスを前へ戻す場合は、有効なコントロールを前方向に探す必要がある
				{
					focusGroupIndex--;
					if( focusGroupIndex < 0 )
					{
						focusGroupIndex = focusGroup.GetItemCount()-1;
					}
				}
			}
		}
	}
	//EnableかつVisibleなコントロールがない場合はフォーカスをクリア
	GetImp().ClearFocus();
#endif
}

//#135
/**
フォーカスを最初のコントロールに移動する。
*/
void	AWindow::NVI_SwitchFocusToFirst()
{
	//#353
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	
	//
	NVI_SwitchFocusTo(kControlID_Invalid);
	if( focusGroup.GetItemCount() > 0 )
	{
		NVI_SwitchFocusTo(focusGroup.Get(0));
	}
	/*#688
	//#135
	if( IsView(NVI_GetCurrentFocus()) == true )
	{
		NVI_GetViewByControlID(NVI_GetCurrentFocus()).NVI_AutomaticSelectBySwitchFocus();
	}
	else
	{
		if( GetImp().IsAutoSelectAllBySwitchFocus(NVI_GetCurrentFocus()) == true )
		{
			GetImp().SetSelectionAll(NVI_GetCurrentFocus());
		}
	}
	*/
}

//#135
/**
タブ内のアクティブフォーカスを更新（タブがactivateされたときに、フォーカスされる）
*/
void	AWindow::UpdateFocusInTab( const AControlID inFocusControlID )
{
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_ControlIDs.GetItemCount(); tabIndex++ )
	{
		AIndex	index = mTabDataArray_ControlIDs.GetObject(tabIndex).Find(inFocusControlID);
		if( index != kIndex_Invalid )
		{
			mTabDataArray_LatentFocus.Set(tabIndex,inFocusControlID);
			break;
		}
	}
}

//現在のフォーカスのあるViewを取得
AControlID	AWindow::NVI_GetCurrentFocus() const
{
	//#135 return mCurrentFocus;
	return GetImpConst().GetCurrentFocus();//#135
}

//タブ内フォーカスを取得
AControlID	AWindow::NVI_GetFocusInTab( const AIndex inTabIndex ) const
{
	return mTabDataArray_LatentFocus.Get(inTabIndex);
}

//#567
/**
現在のフォーカスのあるViewのViewIDを取得
*/
AViewID	AWindow::NVI_GetCurrentFocusViewID() const
{
	AControlID	focusControlID = NVI_GetCurrentFocus();
	if( IsView(focusControlID) == true )
	{
		return NVI_GetViewConstByControlID(focusControlID).GetObjectID();
	}
	else
	{
		return kObjectID_Invalid;
	}
}

//#135
/**
次のフォーカスへ移動
*/
void	AWindow::NVI_SwitchFocusToNext()
{
	GetImp().SetFocusNext();
	
	/*#688
	NVI_SwitchFocusTo(GetNextFocusControlID(NVI_GetCurrentFocus()));
	//#135
	if( IsView(NVI_GetCurrentFocus()) == true )
	{
		NVI_GetViewByControlID(NVI_GetCurrentFocus()).NVI_AutomaticSelectBySwitchFocus();
	}
	else
	{
		if( GetImp().IsAutoSelectAllBySwitchFocus(NVI_GetCurrentFocus()) == true )
		{
			GetImp().SetSelectionAll(NVI_GetCurrentFocus());
		}
	}
	*/
	/*#353
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	
	//
	if( focusGroup.GetItemCount() == 0 )   return;
	
	//次のFocus候補をfocusIndexに入れる
	//現在Focus未選択の場合はFocus候補は0
	AIndex	focusIndex = 0;
	if( NVI_GetCurrentFocus() != kControlID_Invalid )
	{
		//現在のFocusのインデックスを検索
		focusIndex = focusGroup.Find(NVI_GetCurrentFocus());
		
		if( focusIndex == kIndex_Invalid )
		{
			//現在のFocusのインデックスが見つからない場合（ふつうはありえない）
			_AError("focus not found",this);
			focusIndex = 0;
		}
		else
		{
			//focusIndexを次に進める
			focusIndex++;
			if( focusIndex >= focusGroup.GetItemCount() )
			{
				focusIndex = 0;
			}
		}
	}
	NVI_SwitchFocusTo(focusGroup.Get(focusIndex));
	*/
}

//#135
/**
次のフォーカスへ移動
*/
void	AWindow::NVI_SwitchFocusToPrev()
{
	GetImp().SetFocusPrev();
	
	/*#688
	NVI_SwitchFocusTo(GetPreviousFocusControlID(NVI_GetCurrentFocus()));
	//#135
	if( IsView(NVI_GetCurrentFocus()) == true )
	{
		NVI_GetViewByControlID(NVI_GetCurrentFocus()).NVI_AutomaticSelectBySwitchFocus();
	}
	else
	{
		if( GetImp().IsAutoSelectAllBySwitchFocus(NVI_GetCurrentFocus()) == true )
		{
			GetImp().SetSelectionAll(NVI_GetCurrentFocus());
		}
	}
	*/
	/*#353
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	
	if( focusGroup.GetItemCount() == 0 )   return;
	
	//前のFocus候補をfocusIndexに入れる
	//現在Focus未選択の場合はFocus候補はmFocusGroup.GetItemCount()-1
	AIndex	focusIndex = focusGroup.GetItemCount()-1;
	if( NVI_GetCurrentFocus() != kControlID_Invalid )
	{
		//現在のFocusのインデックスを検索
		focusIndex = focusGroup.Find(NVI_GetCurrentFocus());
		
		if( focusIndex == kIndex_Invalid )
		{
			//現在のFocusのインデックスが見つからない場合（ふつうはありえない）
			_AError("focus not found",this);
			focusIndex = 0;
		}
		else
		{
			//focusIndexを前へ戻す
			focusIndex--;
			if( focusIndex < 0 )
			{
				focusIndex = focusGroup.GetItemCount()-1;
			}
		}
	}
	NVI_SwitchFocusTo(focusGroup.Get(focusIndex),true);
	*/
}

//#312
/**
指定ControlIDがタブ内フォーカスかどうかを返す
*/
ABool	AWindow::NVI_IsFocusInTab( const AControlID inControlID ) const
{
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray_LatentFocus.GetItemCount(); tabIndex++ )
	{
		if( mTabDataArray_LatentFocus.Get(tabIndex) == inControlID )
		{
			return true;
		}
	}
	return false;
}

//#353
/**
次のフォーカスコントロールを取得
*/
AControlID	AWindow::GetNextFocusControlID( const AControlID inControlID ) const
{
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	const ABoolArray&	focusGroup_IsGroup = GetFocusGroup_IsGroup();
	
	//引数がkControlID_Invalidの場合は最初のコントロールを返す
	if( inControlID == kControlID_Invalid )
	{
		if( focusGroup.GetItemCount() > 0 )
		{
			return focusGroup.Get(0);
		}
		else
		{
			return kControlID_Invalid;
		}
	}
	
	//現在のindexを取得
	AIndex	currentFocusIndex = focusGroup.Find(inControlID);
	if( currentFocusIndex == kIndex_Invalid )   return inControlID;
	
	//現在の場所から始める
	AIndex focusIndex = currentFocusIndex;
	//最初にfocusableなcontrolを見つける
	while( true )
	{
		//次に移動
		//現在位置がグループ内かどうかによって次の位置決め方法を変える
		if( focusGroup_IsGroup.Get(focusIndex) == false )
		{
			//現在位置がグループ内では無ければ、単純に+1
			focusIndex++;
		}
		else
		{
			//現在位置がグループ内なら、次にグループ外になる箇所に移動
			for( ; focusIndex < focusGroup_IsGroup.GetItemCount(); focusIndex++ )
			{
				if( focusGroup_IsGroup.Get(focusIndex) == false )   break;
			}
		}
		//最後になったら0にする
		if( focusIndex == focusGroup.GetItemCount() )
		{
			focusIndex = 0;
		}
		//次に入った場所がグループ内だった場合、trueの場所まで移動
		if( focusGroup_IsGroup.Get(focusIndex) == true )
		{
			while( NVI_GetControlBool(focusGroup.Get(focusIndex)) == false )
			{
				focusIndex++;
				if( focusIndex == focusGroup.GetItemCount() )
				{
					focusIndex = 0;
				}
				//無限ループ防止
				if( focusIndex == currentFocusIndex )    break;
			}
		}
		//focusableならreturn
		AControlID	controlID = focusGroup.Get(focusIndex);
		if( IsFocusableControl(controlID) == true )   return controlID;
		
		//無限ループ防止
		if( focusIndex == currentFocusIndex )    break;
	}
	return inControlID;
}

//#353
/**
前のフォーカスコントロールを取得
*/
AControlID	AWindow::GetPreviousFocusControlID( const AControlID inControlID ) const
{
	const AArray<AControlID>&	focusGroup = GetFocusGroup();
	const ABoolArray&	focusGroup_IsGroup = GetFocusGroup_IsGroup();
	
	//引数がkControlID_Invalidの場合は最後のコントロールを返す
	if( inControlID == kControlID_Invalid )
	{
		if( focusGroup.GetItemCount() > 0 )
		{
			return focusGroup.Get(focusGroup.GetItemCount()-1);
		}
		else
		{
			return kControlID_Invalid;
		}
	}
	
	//現在のindexを取得
	AIndex	currentFocusIndex = focusGroup.Find(inControlID);
	if( currentFocusIndex == kIndex_Invalid )   return inControlID;
	
	//現在の場所から始める
	AIndex focusIndex = currentFocusIndex;
	//最初にfocusableなcontrolを見つける
	while( true )
	{
		//前に移動
		//現在位置がグループ内かどうかによって前の位置決め方法を変える
		if( focusGroup_IsGroup.Get(focusIndex) == false )
		{
			//現在位置がグループ内では無ければ、単純に-1
			focusIndex--;
		}
		else
		{
			//現在位置がグループ内なら、次にグループ外になる箇所に移動
			for( ; focusIndex >= 0; focusIndex-- )
			{
				if( focusGroup_IsGroup.Get(focusIndex) == false )   break;
			}
		}
		//-1になったら最後にする
		if( focusIndex < 0 )
		{
			focusIndex = focusGroup.GetItemCount()-1;
		}
		//次に入った場所がグループ内だった場合、trueの場所まで移動
		if( focusGroup_IsGroup.Get(focusIndex) == true )
		{
			while( NVI_GetControlBool(focusGroup.Get(focusIndex)) == false )
			{
				focusIndex--;
				if( focusIndex < 0 )
				{
					focusIndex = focusGroup.GetItemCount()-1;
				}
				//無限ループ防止
				if( focusIndex == currentFocusIndex )    break;
			}
		}
		//focusableならreturn
		AControlID	controlID = focusGroup.Get(focusIndex);
		if( IsFocusableControl(controlID) == true )   return controlID;
		
		//無限ループ防止
		if( focusIndex == currentFocusIndex )    break;
	}
	return inControlID;
}

//#353
/**
フォーカス可能かどうかを返す
*/
ABool	AWindow::IsFocusableControl( const AControlID inControlID ) const
{
	return ( NVI_IsControlEnable(inControlID) == true && NVI_IsControlVisible(inControlID) == true );
}

//#360
/**
コントロールがFocus対象かどうかを返す
*/
ABool	AWindow::NVI_GetControlSupportFocus( const AControlID inControlID ) const
{
	return (GetFocusGroup().Find(inControlID)!=kIndex_Invalid);
}

#pragma mark ===========================

#pragma mark ---各種Control/Menu等生成／登録
//#182

/**
TextEditPaneの登録
*/
/*#688
void	AWindow::NVI_RegisterTextEditPane( const AControlID inID, const ABool inWrapMode , const ABool inVScrollbar, const ABool inHScrollBar , const ABool inHasFrame )
{
	GetImp().RegisterTextEditPane(inID,inWrapMode,inVScrollbar,inHScrollBar,inHasFrame);
	NVI_RegisterToFocusGroup(inID);
}
*/

/**
ColorPickerの登録
*/
void	AWindow::NVI_RegisterColorPickerPane( const AControlID inID )
{
	GetImp().RegisterColorPickerPane(inID);
}

/**
ヘルプの登録
*/
void	AWindow::NVI_RegisterHelpAnchor( const AControlID inID, AConstCstringPtr inAnchor )
{
	//#539 GetImp().RegisterHelpAnchor(inHelpNumber,inAnchor);
	mHelpAnchorArray_ControlID.Add(inID);
	AText	text;
	text.SetCstring(inAnchor);
	mHelpAnchorArray_Anchor.Add(text);
}

/**
ラジオグループ登録
*/
void	AWindow::NVI_RegisterRadioGroup()
{
	//#349 GetImp().RegisterRadioGroup();
	//#349
	mRadioGroup.AddNewObject();
}

/**
ラジオグループへの追加
*/
void	AWindow::NVI_AddToLastRegisteredRadioGroup( const AControlID inID )
{
	//#349 GetImp().AddToLastRegisteredRadioGroup(inID);
	//#349
	if( mRadioGroup.GetItemCount() == 0 )   return;
	mRadioGroup.GetObject(mRadioGroup.GetItemCount()-1).Add(inID);
	mRadioGroup_All.Add(inID);
	//#353
	NVI_RegisterToFocusGroup(inID,true,true);
}

//#349
/**
ラジオボタングループのUpdate（指定したコントロール以外、falseに設定する）
*/
void	AWindow::UpdateRadioGroup( const AControlID inControlID )
{
	//ラジオグループ以外ならリターン
	if( mRadioGroup_All.Find(inControlID) == kIndex_Invalid )   return;
	
	//指定Controlが含まれるラジオボタングループを検索
	AIndex	foundRadioGroup = kIndex_Invalid;
	AItemCount	radioGroupCount = mRadioGroup.GetItemCount();
	for( AIndex i = 0; i < radioGroupCount; i++ )
	{
		if( mRadioGroup.GetObjectConst(i).Find(inControlID) != kIndex_Invalid )
		{
			foundRadioGroup = i;
			break;
		}
	}
	//ラジオブループが見つからなければリターン
	if( foundRadioGroup == kIndex_Invalid )   return;
	
	//inControlIDがtrueの場合、他のグループ内controlをfalseにする
	if( NVI_GetControlBool(inControlID) == true )
	{
		//
		AItemCount	radioButtonCount = mRadioGroup.GetObject(foundRadioGroup).GetItemCount();
		for( AIndex j = 0; j < radioButtonCount; j++ )
		{
			AControlID	controlID = mRadioGroup.GetObject(foundRadioGroup).Get(j);
			if( controlID != inControlID )
			{
				//他のグループ内controlをfalseにして、データ更新処理
				NVI_SetControlBool(controlID,false);
				DoValueChanged_DBData(controlID);
			}
		}
	}
}

/**
スクロールバー生成
*/
void	AWindow::NVI_CreateScrollBar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex )
{
	GetImp().CreateScrollBar(inID,inPoint,inWidth,inHeight,inTabIndex);
}

/**
スクロールバー登録
*/
void	AWindow::NVI_RegisterScrollBar( const AControlID inID )
{
	GetImp().RegisterScrollBar(inID);
}

#pragma mark ---WebView
//#734

/**
WebView生成
*/
void	AWindow::NVI_CreateWebView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	GetImp().CreateWebView(inID,inPoint,inWidth,inHeight);
}

/**
WebViewにLoad要求
*/
void	AWindow::NVI_LoadURLToWebView( const AControlID inID, const AText& inURL )
{
	GetImp().LoadURLToWebView(inID,inURL);
}

/**
WebViewにReload要求
*/
void	AWindow::NVI_ReloadWebView( const AControlID inID )
{
	GetImp().ReloadWebView(inID);
}

/**
WebViewの現在のURL取得
*/
void	AWindow::NVI_GetCurrentWebViewURL( const AControlID inID, AText& outURL ) const
{
	GetImpConst().GetCurrentWebViewURL(inID,outURL);
}

/**
WebViewでJavaScript実行
*/
void	AWindow::NVI_ExecuteJavaScriptInWebView( const AControlID inID, const AText& inText )
{
	GetImp().ExecuteJavaScriptInWebView(inID,inText);
}

/**
WebViewの表示倍率設定
*/
void	AWindow::NVI_SetWebViewFontMultiplier( const AControlID inID, const AFloatNumber inMultiplier )
{
	GetImp().SetWebViewFontMultiplier(inID,inMultiplier);
}

#pragma mark ===========================

#pragma mark ---ファイル／フォルダ選択
//#182

/**
フォルダ選択
*/
void	AWindow::NVI_ShowChooseFolderWindow( const AFileAcc& inDefaultFolder, const AText& inMessage,
		const AControlID inVirtualControlID, const ABool inSheet )//#551
{
	GetImp().ShowChooseFolderWindow(inDefaultFolder,inMessage,inVirtualControlID,inSheet);//#551
}

/**
ファイル選択
*/
void	AWindow::NVI_ShowChooseFileWindow( const AFileAcc& inDefaultFile, const AText& inMessage,
		const AControlID inVirtualControlID, const ABool inOnlyApp, 
		const ABool inSheet )
{
	GetImp().ShowChooseFileWindow(inDefaultFile,inMessage,inVirtualControlID,inOnlyApp,inSheet);
}

/**
ファイル選択
*/
void	AWindow::NVI_ShowChooseFileWindowWithFileFilter( const AFileAcc& inDefaultFile, const AControlID inVirtualControlID, const ABool inSheet,
		const AFileAttribute inFilterAttribute )
{
	GetImp().ShowChooseFileWindowWithFileFilter(inDefaultFile,inVirtualControlID,inSheet,inFilterAttribute);
}

/**
保存ウインドウ
*/
void	AWindow::NVI_ShowSaveWindow( const AText& inFileName, const ADocumentID inDocumentID, 
		const AFileAttribute inFileAttr, const AText& inFilter, const AText& inRefText, const AFileAcc& inDefaultFolder )
{
	GetImp().ShowSaveWindow(inFileName,inDocumentID,inFileAttr,inFilter,inRefText,inDefaultFolder);
}


#pragma mark ===========================

#pragma mark ---子ウインドウ
//#182

/**
子ウインドウ上にOKダイアログを表示
*/
void	AWindow::NVI_ShowChildWindow_OK( const AText& inMessage1, const AText& inMessage2, const ABool inLongDialog )
{
	//#291 GetImp().ShowChildWindow_OK(inMessage1,inMessage2);
	
	//既に開いている子シートウインドウがあったらリターン
	if( NVI_IsChildWindowVisible() == true )   return;
	
	//OKCancelウインドウ表示
	AWindowDefaultFactory<AWindow_OK>	childWindowFactory;
	mOKWindow = AApplication::GetApplication().NVI_CreateWindow(childWindowFactory);
	(dynamic_cast<AWindow_OK&>(AApplication::GetApplication().NVI_GetWindowByID(mOKWindow))).
			SPNVI_CreateAndShow(GetObjectID(),inMessage1,inMessage2,inLongDialog);
}

//#427
/**
子OKダイアログを閉じる
*/
void	AWindow::NVI_CloseChildWindow_OK()
{
	(dynamic_cast<AWindow_OK&>(AApplication::GetApplication().NVI_GetWindowByID(mOKWindow))).
			NVI_Close();
	//Close完了すると、AWindow_OKからNVI_DoChildWindowClosed_OK()がコールされる。
}

//#427
/**
OK子ウインドウ上のOKボタンのEnable/Disable制御
*/
void	AWindow::NVI_SetChildWindow_OK_EnableOKButton( const ABool inEnable )
{
	(dynamic_cast<AWindow_OK&>(AApplication::GetApplication().NVI_GetWindowByID(mOKWindow))).
			SPI_SetEnableOKButton(inEnable);
}

//#427
/**
OK子ウインドウ上のTextメッセージ設定
*/
void	AWindow::NVI_SetChildWindow_OK_SetText( const AText& inMessage1, const AText& inMessage2 )
{
	(dynamic_cast<AWindow_OK&>(AApplication::GetApplication().NVI_GetWindowByID(mOKWindow))).
			SPI_SetText(inMessage1,inMessage2);
}

/**
OK/Cancel子ウインドウが閉じられたときの処理
*/
void	AWindow::NVI_DoChildWindowClosed_OK()
{
	AApplication::GetApplication().NVI_DeleteWindow(mOKWindow);
	mOKWindow = kObjectID_Invalid;
}

/**
子ウインドウ上にOK/Cancelダイアログを表示
*/
void	AWindow::NVI_ShowChildWindow_OKCancel( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID )
{
	//#291 GetImp().ShowChildWindow_OKCancel(inMessage1,inMessage2,inOKButtonMessage,inOKButtonVirtualControlID);
	
	//既に開いている子シートウインドウがあったらリターン
	if( NVI_IsChildWindowVisible() == true )   return;
	
	//OKCancelウインドウ表示
	AWindowDefaultFactory<AWindow_OKCancel>	childWindowFactory;
	mOKCancelWindow = AApplication::GetApplication().NVI_CreateWindow(childWindowFactory);
	AApplication::GetApplication().NVI_GetWindowByID(mOKCancelWindow).NVI_Create(kObjectID_Invalid);
	(dynamic_cast<AWindow_OKCancel&>(AApplication::GetApplication().NVI_GetWindowByID(mOKCancelWindow))).
			SPNVI_Show(GetObjectID(),inMessage1,inMessage2,inOKButtonMessage,inOKButtonVirtualControlID);
}

/**
OK/Cancel子ウインドウが閉じられたときの処理
*/
void	AWindow::NVI_DoChildWindowClosed_OKCancel()
{
	AApplication::GetApplication().NVI_DeleteWindow(mOKCancelWindow);
	mOKCancelWindow = kObjectID_Invalid;
}

/**
子ウインドウ上にOK/Cancel/Discardダイアログを表示
*/
void	AWindow::NVI_ShowChildWindow_OKCancelDiscard( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID, const AControlID inDiscardButtonVirtualControlID )
{
	//#291 GetImp().ShowChildWindow_OKCancelDiscard(inMessage1,inMessage2,inOKButtonMessage,inOKButtonVirtualControlID,inDiscardButtonVirtualControlID);
	
	//既に開いている子シートウインドウがあったらリターン
	if( NVI_IsChildWindowVisible() == true )   return;
	
	//OKCancelウインドウ表示
	AWindowDefaultFactory<AWindow_OKCancelDiscard>	childWindowFactory;
	mOKCancelDiscardWindow = AApplication::GetApplication().NVI_CreateWindow(childWindowFactory);
	AApplication::GetApplication().NVI_GetWindowByID(mOKCancelDiscardWindow).NVI_Create(kObjectID_Invalid);
	(dynamic_cast<AWindow_OKCancelDiscard&>(AApplication::GetApplication().NVI_GetWindowByID(mOKCancelDiscardWindow))).
			SPNVI_Show(GetObjectID(),inMessage1,inMessage2,inOKButtonMessage,inOKButtonVirtualControlID,inDiscardButtonVirtualControlID);
}

/**
OK/Cancel/Discard子ウインドウが閉じられたときの処理
*/
void	AWindow::NVI_DoChildWindowClosed_OKCancelDiscard()
{
	AApplication::GetApplication().NVI_DeleteWindow(mOKCancelDiscardWindow);
	mOKCancelDiscardWindow = kObjectID_Invalid;
}

//#1034
/**
AskSaveChangesウインドウ
*/
AAskSaveChangesResult	AWindow::NVI_ShowAskSaveChangesWindow( const AText& inFileName, const AObjectID inDocumentID, 
															   const ABool inModal )
{
	//モーダルかどうかを記憶
	mAskSaveChanges_IsModal = inModal;
	//モーダルの場合の結果保存用メンバ変数初期化
	mAskSaveChanges_Result = kAskSaveChangesResult_None;
	
	//SaveCancelDiscardウインドウ表示
	AWindowDefaultFactory<AWindow_SaveCancelDiscard>	childWindowFactory;
	mSaveCancelDiscardWindow = AApplication::GetApplication().NVI_CreateWindow(childWindowFactory);
	AApplication::GetApplication().NVI_GetWindowByID(mSaveCancelDiscardWindow).NVI_Create(kObjectID_Invalid);
	AText	message1, message2;
	message1.SetLocalizedText("SaveCancelDiscard_Message1");
	message1.ReplaceParamText('0',inFileName);
	message2.SetLocalizedText("SaveCancelDiscard_Message2");
	//保存確認ダイアログ表示
	//inModal=falseのときは、すぐに返ってくる。
	//inModal=trueのときは、ボタンをクリックするまでここでブロックする。mAskSaveChanges_Resultに結果が入る。
	(dynamic_cast<AWindow_SaveCancelDiscard&>(AApplication::GetApplication().NVI_GetWindowByID(mSaveCancelDiscardWindow))).
			SPNVI_Show(inDocumentID,GetObjectID(),message1,message2,inModal);
	
	return mAskSaveChanges_Result;
	/*#1034
	//モーダルかどうかを記憶
	mAskSaveChanges_IsModal = inModal;
	//モーダルの場合の結果保存用メンバ変数初期化
	mAskSaveChanges_Result = kAskSaveChangesResult_None;
	//保存確認ダイアログ表示
	//inModal=falseのときは、すぐに返ってくる。返り値はkAskSaveChangesResult_None固定。
	//inModal=trueのときは、ボタンをクリックするまでここでブロックする。返り値に結果を返す。
	GetImp().ShowAskSaveChangesWindow(inFileName,inDocumentID,inModal);
	return mAskSaveChanges_Result;
	*/
}

/**
OK/Cancel/Discard子ウインドウが閉じられたときの処理
*/
void	AWindow::NVI_DoChildWindowClosed_SaveCancelDiscard()
{
	AApplication::GetApplication().NVI_DeleteWindow(mSaveCancelDiscardWindow);
	mSaveCancelDiscardWindow = kObjectID_Invalid;
}

/**
子ウインドウを閉じる
*/
void	AWindow::NVI_CloseChildWindow()
{
	//#291 GetImp().CloseChildWindow();
	//#291
	if( mOKWindow != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mOKWindow).NVI_Close();
	}
	if( mOKCancelWindow != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mOKCancelWindow).NVI_Close();
	}
	if( mOKCancelDiscardWindow != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mOKCancelDiscardWindow).NVI_Close();
	}
	//#1034
	if( mSaveCancelDiscardWindow != kObjectID_Invalid )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mSaveCancelDiscardWindow).NVI_Close();
	}
}

/* #291
NVI_IsChildWindowVisible()に統合
ABool	AWindow::NVI_ChildWindowVisible() const
{
	return GetImpConst().ChildWindowVisible();
}
*/

//B0411
/**
子ウインドウがvisibleかどうかを取得
*/
ABool	AWindow::NVI_IsChildWindowVisible() const
{
	if( GetImpConst().ChildWindowVisible() == true )   return true;
	if( NVI_IsPrintMode() == true )   return true;
	//#694 登録された子ウインドウが表示中かどうかを調べる
	for( AIndex i = 0; i < mChildWindowIDArray.GetItemCount(); i++ )
	{
		AWindowID	winID = mChildWindowIDArray.Get(i);
		if( AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_IsWindowVisible() == true )
		{
			return true;
		}
	}
	//
	if( NVIDO_IsChildWindowVisible() == true )   return true;
	if( mOKWindow != kObjectID_Invalid )   return true;//#291
	if( mOKCancelWindow != kObjectID_Invalid )   return true;//#291
	if( mOKCancelDiscardWindow != kObjectID_Invalid )   return true;//#291
	if( mSaveCancelDiscardWindow != kObjectID_Invalid )   return true;//#1034
	return false;
}

//#694
/**
子ウインドウ登録
*/
void	AWindow::NVI_RegisterChildWindow( const AWindowID inChildWindowID )
{
	mChildWindowIDArray.Add(inChildWindowID);
}

//#694
/**
子ウインドウ登録解除
*/
void	AWindow::NVI_UnregisterChildWindow( const AWindowID inChildWindowID )
{
	AIndex	index = mChildWindowIDArray.Find(inChildWindowID);
	if( index != kIndex_Invalid )
	{
		mChildWindowIDArray.Delete1(index);
	}
}


#pragma mark ===========================

#pragma mark <所有クラス(AView)インターフェイス>

#pragma mark ===========================

#pragma mark ---処理通知コールバック

/*#1031
//Viewがスクロールされた
void	AWindow::OWICB_ViewScrolled( const AControlID inControlID, const ANumber inDeltaX, const ANumber inDeltaY,
									const ABool inRedraw, const ABool inConstrainToImageSize )
{
	NVIDO_ViewScrolled(inControlID,inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize);
}
*/

//View内の選択範囲が変更された
void	AWindow::OWICB_SelectionChanged( const AControlID inControlID )
{
	NVIDO_SelectionChanged(inControlID);
}

#pragma mark ===========================

#pragma mark ---View内でのキー押下共通処理

//#135
/**
EditBoxView/ListView等内でESCキーが押された場合のEditBoxからのコールバック（コマンド+.も含む）
*/
void	AWindow::OWICB_ViewEscapeKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//派生クラスでの処理（返り値trueなら派生クラスの処理だけでreturn）
	if( NVIDO_ViewEscapeKeyPressed(inControlID) == true )
	{
		return;
	}
	//デフォルトキャンセルボタン設定済みなら、デフォルトキャンセルボタンクリック時の動作
	if( mDefaultCancelButton != kControlID_Invalid )
	{
		EVT_Clicked(mDefaultCancelButton,inModifers);
	}
	//上記以外でウインドウに閉じるボタンがある場合は閉じるボタンクリック時の動作
	else if( NVI_HasCloseButton() == true )
	{
		EVT_DoCloseButton();
	}
}

//#135
/**
EditBoxView/ListView等内でreturnキーが押された場合のEditBoxからのコールバック
*/
void	AWindow::OWICB_ViewReturnKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//派生クラスでの処理（返り値trueなら派生クラスの処理だけでreturn）
	if( NVIDO_ViewReturnKeyPressed(inControlID) == true )
	{
		return;
	}
	//EditBoxの値が変更されたことをウインドウ（自分）に通知し、全選択
	if( inControlID != kControlID_Invalid )
	{
		EVT_ValueChanged(inControlID);
		NVI_GetViewByControlID(inControlID).NVI_SetSelectAll();
	}
	//デフォルトOKボタン設定済みなら、デフォルトOKボタンクリック時の動作
	if( mDefaultOKButton != kControlID_Invalid )
	{
		if( NVI_IsControlEnable(mDefaultOKButton) == true )
		{
			EVT_Clicked(mDefaultOKButton,inModifers);
		}
	}
}

//#135
/**
EditBoxView/ListView等内でtabキーが押された場合のEditBoxからのコールバック
*/
void	AWindow::OWICB_ViewTabKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//派生クラスでの処理（返り値trueなら派生クラスの処理だけでreturn）
	if( NVIDO_ViewTabKeyPressed(inControlID) == true )
	{
		return;
	}
	//EditBoxの値が変更されたことをウインドウ（自分）に通知
	if( inControlID != kControlID_Invalid )
	{
		EVT_ValueChanged(inControlID);
	}
	//フォーカス移動
	if( AKeyWrapper::IsShiftKeyPressed(inModifers) == true )
	{
		NVI_SwitchFocusToPrev();
	}
	else
	{
		NVI_SwitchFocusToNext();
	}
}

//#798
/**
Viewでの↑キー押下時処理
*/
ABool	AWindow::OWICB_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//派生クラスでの処理（返り値trueなら派生クラスの処理だけでreturn）
	if( NVIDO_ViewArrowUpKeyPressed(inControlID,inModifers) == true )
	{
		return true;
	}
	//
	return false;
}

//#798
/**
Viewでの↓キー押下時処理
*/
ABool	AWindow::OWICB_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//派生クラスでの処理（返り値trueなら派生クラスの処理だけでreturn）
	if( NVIDO_ViewArrowDownKeyPressed(inControlID,inModifers) == true )
	{
		return true;
	}
	//
	return false;
}

//#892
/**
ListViewでの折りたたみ状態変更時処理
*/
void	AWindow::OWICB_ListViewExpandedCollapsed( const AControlID inControlID )
{
	NVIDO_ListViewExpandedCollapsed(inControlID);
}

/**
ボタンviewドラッグキャンセル時処理
*/
void	AWindow::OWICB_ButtonViewDragCanceled( const AControlID inControlID )
{
	NVIDO_ButtonViewDragCanceled(inControlID);
}

/**
ListViewのホバー範囲更新時処理
*/
void	AWindow::OWICB_ListViewHoverUpdated( const AControlID inControlID, 
											const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex )
{
	NVIDO_ListViewHoverUpdated(inControlID,inHoverStartDBIndex,inHoverEndDBIndex);
}

#pragma mark ===========================

#pragma mark ---ListViewからの処理通知

/**
ListViewでの行移動時処理（ListViewでの行移動を、AWindow内データに反映させる）
@note 選択解除状態であること前提。ListView側で選択解除し、AWindowにも反映させた状態でコールされる。
*/
void	AWindow::OWICB_ListViewRowMoved( const AControlID inTableControlID, const AIndex inOldRowIndex, const AIndex inNewRowIndex )
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	if( tableIndex != kIndex_Invalid )
	{
		//------------------データの移動------------------
		ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
		NVM_GetDB().MoveRow_Table(firstDataID,inOldRowIndex,inNewRowIndex);
		//------------------コントロール表示反映------------------
		NVM_UpdateControlStatus();
		//------------------データ変更通知------------------
		if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
		{
			//メニューの更新等の処理を簡単にするため、deleted, addedだけを使う
			NVMDO_NotifyDataChanged(inTableControlID,kModificationType_RowDeleted,inOldRowIndex,inTableControlID);
			NVMDO_NotifyDataChanged(inTableControlID,kModificationType_RowAdded,inNewRowIndex,inTableControlID);
		}
		//#625 複数選択対応 行移動ボタンによる行移動が完了したことを通知
		NVMDO_NotifyDataChanged(inTableControlID,kModificationType_TransactionCompleted_MoveRow,kIndex_Invalid,inTableControlID);
	}
	//行移動時処理
	NVIDO_ListViewRowMoved(inTableControlID,inOldRowIndex,inNewRowIndex);
}

#pragma mark ===========================

#pragma mark ---コントロールの値設定／取得
//#135 #182

/**
テキスト設定
*/
void	AWindow::NVI_SetControlText( const AControlID inControlID, const AText& inText )
{
	//#0 同じテキストの場合、何もせずにリターン
	AText	text;
	NVI_GetControlText(inControlID,text);
	if( text.Compare(inText) == true )   return;
	//
	if( IsView(inControlID) == false )
	{
		GetImp().SetText(inControlID,inText);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetText(inText);
	}
}

/**
テキスト設定
*/
void	AWindow::NVI_AddControlText( const AControlID inControlID, const AText& inText )
{
	if( IsView(inControlID) == false )
	{
		GetImp().AddText(inControlID,inText);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_AddText(inText);
	}
}

/**
テキスト取得
*/
void	AWindow::NVI_GetControlText( const AControlID inControlID, AText& outText ) const
{
	if( IsView(inControlID) == false )
	{
		GetImpConst().GetText(inControlID,outText);
	}
	else
	{
		NVI_GetViewConstByControlID(inControlID).NVI_GetText(outText);
	}
}

/**
数値設定
*/
void	AWindow::NVI_SetControlNumber( const AControlID inControlID, const ANumber inNumber )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetNumber(inControlID,inNumber);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetNumber(inNumber);
	}
}

/**
数値取得
*/
ANumber	AWindow::NVI_GetControlNumber( const AControlID inControlID ) const
{
	if( IsView(inControlID) == false )
	{
		return GetImpConst().GetNumber(inControlID);
	}
	else
	{
		return NVI_GetViewConstByControlID(inControlID).NVI_GetNumber();
	}
}

/**
数値取得
*/
void	AWindow::NVI_GetControlNumber( const AControlID inControlID, ANumber& outNumber ) const
{
	if( IsView(inControlID) == false )
	{
		outNumber = GetImpConst().GetNumber(inControlID);
	}
	else
	{
		outNumber = NVI_GetViewConstByControlID(inControlID).NVI_GetNumber();
	}
}

/**
Bool設定
*/
void	AWindow::NVI_SetControlBool( const AControlID inControlID, const ABool inBool )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetBool(inControlID,inBool);
		
		//#349 ラジオグループ更新処理
		UpdateRadioGroup(inControlID);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetBool(inBool);
	}
}

/**
Bool取得
*/
void	AWindow::NVI_GetControlBool( const AControlID inControlID, ABool& outBool ) const
{
	if( IsView(inControlID) == false )
	{
		outBool = GetImpConst().GetBool(inControlID);
	}
	else
	{
		outBool = NVI_GetViewConstByControlID(inControlID).NVI_GetBool();
	}
}

/**
Bool取得
*/
ABool	AWindow::NVI_GetControlBool( const AControlID inControlID ) const
{
	if( IsView(inControlID) == false )
	{
		return GetImpConst().GetBool(inControlID);
	}
	else
	{
		return NVI_GetViewConstByControlID(inControlID).NVI_GetBool();
	}
}

/**
Color設定
*/
void	AWindow::NVI_SetControlColor( const AControlID inControlID, const AColor& inColor )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetColor(inControlID,inColor);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetColor(inColor);
	}
}

/**
Color取得
*/
void	AWindow::NVI_GetControlColor( const AControlID inControlID, AColor& outColor ) const
{
	if( IsView(inControlID) == false )
	{
		GetImpConst().GetColor(inControlID,outColor);
	}
	else
	{
		NVI_GetViewConstByControlID(inControlID).NVI_GetColor(outColor);
	}
}

/**
Float数値設定
*/
void	AWindow::NVI_SetControlFloatNumber( const AControlID inControlID, const AFloatNumber inNumber )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetFloatNumber(inControlID,inNumber);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetFloatNumber(inNumber);
	}
}

/**
Float数値取得
*/
void	AWindow::NVI_GetControlFloatNumber( const AControlID inControlID, AFloatNumber& outNumber ) const
{
	if( IsView(inControlID) == false )
	{
		 GetImpConst().GetFloatNumber(inControlID,outNumber);
	}
	else
	{
		outNumber = NVI_GetViewConstByControlID(inControlID).NVI_GetFloatNumber();
	}
}

/**
テキストフォント設定
*/
void	AWindow::NVI_SetControlTextFont( const AControlID inControlID, const AText& inFontName, const AFloatNumber inFontSize )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetTextFont(inControlID,inFontName,inFontSize);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetTextFont(inFontName,inFontSize);
	}
}

//#688
/**
Progress indicatorにprogress値設定
*/
void	AWindow::NVI_SetProgressIndicatorProgress( const AIndex inProgressIndex, const ANumber inProgress, const ABool inShow )
{
	GetImp().SetProgressIndicatorProgress(inProgressIndex,inProgress,inShow);
}

//#688
/*
void	AWindow::NVI_SetMouseTrackingRect( const AControlID inControlID, const AArray<ALocalRect>& inLocalRectArray )
{
	GetImp().SetMouseTrackingRect(inControlID,inLocalRectArray);
}
*/

//#688
/**
controlにfont設定
*/
void	AWindow::NVI_SetControlFont( const AControlID inID, const AText& inFontName, const AFloatNumber inFontSize )
{
	GetImp().SetFont(inID,inFontName,inFontSize);
}

//#688
/**
controlのfont取得
*/
void	AWindow::NVI_GetControlFont( const AControlID inID, AText& outFontName, AFloatNumber& outFontSize ) const
{
	GetImpConst().GetFont(inID,outFontName,outFontSize);
}

#pragma mark ===========================

#pragma mark ---インライン入力
//#182

/**
インライン入力確定
*/
void	AWindow::NVI_FixInlineInput( const AControlID inControlID )
{
	GetImp().FixInlineInput(inControlID);
}

#pragma mark ===========================

#pragma mark ---コントロール状態の更新

//コントロール状態（Enable/Disable等）を更新
//NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()からコールされる
//クリックやアクティベートでコールされるので、あまり重い処理をNVMDO_UpdateControlStatus()には入れないこと。
void	AWindow::NVM_UpdateControlStatus()
{
	if( NVI_IsWindowCreated() == false )   return;//#0 モード設定「文法」でドキュメントに反映ボタンを押すとCError発生する問題を修正
	
	//#427 下へ移動（NVMDO_UpdateControlStatus()でGrayout状態を更新するため。）NVMDO_UpdateControlStatus();
	
	//DB連動処理
	if( IsDBDataTableExist() == true )
	{
		UpdateDBControlStatus();
	}
	
	//#427
	NVMDO_UpdateControlStatus();
}

#pragma mark ===========================

#pragma mark ---DB連動データの登録

//データの登録（ControlIDとDataIDの値が同一の場合用）
//DB上のデータと、UI上のコントロールの対応付け
void	AWindow::NVM_RegisterDBData( const ADataID inDataID, const ABool inNotifyDataChanged, 
		const AControlID inEnablerControlID, const AControlID inDisablerControlID, const ABool inReverseBool )
{
	NVM_RegisterDBData_(inDataID,inDataID,inNotifyDataChanged,inEnablerControlID,inDisablerControlID,inReverseBool);
}

//データの登録
//DB上のデータと、UI上のコントロールの対応付け
//inControlID: UI上のコントロールのCotrolID
//inDataID: DB上のデータのDataID
//inNotifyDataChanged: コントロールの値変更時に、設定変更反映処理等を行うためにNVICB_NotifyDataChanged()をコールするかどうか
//inEnablerControlID: このコントロールをEnable/Disableする他のコントロールのControlID（無い場合はkControlID_Invalidを指定）
void	AWindow::NVM_RegisterDBData_( const AControlID inControlID, const ADataID inDataID, const ABool inNotifyDataChanged, 
		const AControlID inEnablerControlID, const AControlID inDisablerControlID, const ABool inReverseBool )
{
	mDBData_ControlID.Add(inControlID);
	mDBData_DataID.Add(inDataID);
	mDBData_NotifyDataChanged.Add(inNotifyDataChanged);
	mDBData_EnablerControlID.Add(inEnablerControlID);
	mDBData_DisablerControlID.Add(inDisablerControlID);
	mDBData_ReverseBool.Add(inReverseBool);
}

#pragma mark ===========================

#pragma mark ---データタイプ設定（DB連動コントロール用）
//#182

/**
Controlのデータタイプ設定（DB連動コントロール用）
*/
void	AWindow::NVM_SetControlDataType( const AControlID inControlID, const ADataType inDataType )
{
	if( IsView(inControlID) == false )
	{
		GetImp().SetDataType(inControlID,inDataType);
	}
	else
	{
		NVI_GetViewByControlID(inControlID).NVI_SetDataType(inDataType);
	}
}

#pragma mark ===========================

#pragma mark ---テーブル選択

/**
テーブルViewの行を選択する（ContentViewの保存や、現在選択行の更新も行う）

@param inControlID テーブルViewのControlID
@param inTableRowDBIndex 行のDBIndex
*/
void	AWindow::NVM_SelectTableRow( const AControlID inControlID, const AIndex inTableRowDBIndex )
{
	/*#205
#if USE_IMP_TABLEVIEW
	NVI_SetControlNumber(inControlID,inTableRow);
#else
	NVI_GetListView(inControlID).SPI_SetSelectByDBIndex(inTableRow);
#endif
	*/
	//行選択
	SetTableViewSelectionByDBIndex(inControlID,inTableRowDBIndex);
	//ContentViewの保存
	NVM_SaveContentView(inControlID);
	//現在選択行情報の更新
	AIndex	tableIndex = mDBTable_ControlID.Find(inControlID);
	mDBTable_CurrentSelect.Set(tableIndex,inTableRowDBIndex);
}

//#205
/**
テーブルViewの行を選択する（ContentViewの保存や、現在選択行の更新はしない）

@param inControlID テーブルViewのControlID
@param inTableRowDBIndex 行のDBIndex
*/
void	AWindow::SetTableViewSelectionByDBIndex( const AControlID inControlID, const AIndex inTableRowDBIndex )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inControlID))
	{
	  case kTableViewType_OSControl:
		{
			NVI_SetControlNumber(inControlID,inTableRowDBIndex);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inControlID).SPI_SetSelectByDBIndex(inTableRowDBIndex);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#205
/**
DB連動テーブルで現在選択中の行の(DB)Indexを返す（複数選択の時はkIndex_Invalidを返す）
*/
AIndex	AWindow::NVI_GetTableViewSelectionDBIndex( const AControlID inTableControlID ) const
{
	AIndex	rowIndex = kIndex_Invalid;
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			rowIndex = NVI_GetControlNumber(inTableControlID);
			break;
		}
	  case kTableViewType_ListView:
		{
			rowIndex = NVI_GetListViewConst(inTableControlID).SPI_GetSelectedDBIndex();
			//#625 複数選択を可能にする。複数選択の場合は、基本的には（削除ボタン等以外は）、未選択時と同じ扱いにする。
			AArray<AIndex>	DBIndexArray;
			SPI_GetTableViewSelectionDBIndexArray(inTableControlID,DBIndexArray);
			if( DBIndexArray.GetItemCount() >= 2 )
			{
				//未選択扱い
				rowIndex = kIndex_Invalid;
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return rowIndex;
}

//#625
/**
選択項目を取得（複数選択の時は複数の項目を取得する）
*/
void	AWindow::SPI_GetTableViewSelectionDBIndexArray( const AControlID inTableControlID, AArray<AIndex>& outDBIndexArray ) const
{
	outDBIndexArray.DeleteAll();
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			AIndex	rowIndex = NVI_GetControlNumber(inTableControlID);
			if( rowIndex != kIndex_Invalid )
			{
				outDBIndexArray.Add(rowIndex);
			}
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListViewConst(inTableControlID).SPI_GetSelectedDBIndexArray(outDBIndexArray);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#205
/**
DB連動テーブルで現在選択中の列のDataIDを返す
現状、kDataID_Invalid固定、もしくは、最後にクリックした列を返す（列選択未対応）
*/
ADataID	AWindow::NVI_GetTableViewSelectionColumnID( const AControlID inTableControlID ) const
{
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return kDataID_Invalid;
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListViewConst(inTableControlID).SPI_GetLastClickedColumnID();
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return kDataID_Invalid;
}

#pragma mark ===========================

#pragma mark ---テーブル編集
//#205

/**
DB連動テーブルの編集開始（行は現在選択中の行）
*/
void	AWindow::NVI_StartTableColumnEditMode( const AControlID inTableControlID, const ADataID inColumnID )
{
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().EnterColumnEditMode(inTableControlID,inColumnID);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_StartEditMode(inColumnID);
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

#pragma mark ---DB連動テーブルデータの登録

//テーブルの登録
//inTableControlID: テーブルコントロールのControlID
//inFirstDataID: 
void	AWindow::NVM_RegisterDBTable( const AControlID inTableControlID, const ADataID inFirstDataID,
		const AControlID inAddRowControlID, const AControlID inDeleteRowControlID, const AControlID inDeleteRowOKControlID,
		const AControlID inUpID, const AControlID inDownID, const AControlID inTopID, const AControlID inBottomID,
		const AControlID inContentGroupControlID, const ABool inNotifyDataChanged,
		const ABool inFrame, const ABool inHeader, const ABool inVScrollBar, const ABool inHScrollBar, 
		const ATableViewType inTableViewType  )//win 080618
{
	//#205
	//inTableViewTypeによって生成するコントロールを切り替え
	switch(inTableViewType)
	{
	  case kTableViewType_OSControl:
		{
			GetImp().RegisterTableView(inTableControlID,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid,kControlID_Invalid);//R0183
			break;
		}
	  case kTableViewType_ListView:
		{
			//ListView生成
			AListViewFactory	listViewFactory(GetObjectID(),inTableControlID);
			NVM_CreateView(inTableControlID,false,listViewFactory);
			
			//フレーム生成
			if( inFrame == true )
			{
				NVI_GetListView(inTableControlID).SPI_CreateFrame(inHeader,inVScrollBar,inHScrollBar);
			}
			
			//#625 ListViewの複数選択を可能にする
			NVI_GetListView(inTableControlID).SPI_SetMultiSelectEnabled(true);
			//
			NVI_GetListView(inTableControlID).SPI_SetEnableDragDrop(true);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	
	mDBTable_ControlID.Add(inTableControlID);
	mDBTable_FirstDataID.Add(inFirstDataID);
	mDBTable_AddRow_ControlID.Add(inAddRowControlID);
	mDBTable_DeleteRow_ControlID.Add(inDeleteRowControlID);
	mDBTable_DeleteRowOK_ControlID.Add(inDeleteRowOKControlID);
	mDBTable_Up_ControlID.Add(inUpID);
	mDBTable_Down_ControlID.Add(inDownID);
	mDBTable_Top_ControlID.Add(inTopID);
	mDBTable_Bottom_ControlID.Add(inBottomID);
	mDBTable_ColumnIDArray.AddNewObject();
	mDBTable_DataIDArray.AddNewObject();
	mDBTable_ColumnEditButtonIDArray.AddNewObject();
	mDBTable_ContentIDArray.AddNewObject();
	mDBTable_ContentReverseBool.AddNewObject();//#1032
	mDBTable_SpecializedContentIDArray.AddNewObject();//B0406
	mDBTable_ContentDataIDArray.AddNewObject();
	mDBTable_ContentGroupControlID.Add(inContentGroupControlID);
	mDBTable_NotifyDataChanged.Add(inNotifyDataChanged);
	mDBTable_CurrentSelect.Add(kIndex_Invalid);
	mDBTable_CurrentContentViewRowIndex.Add(kIndex_Invalid);//B0406
	mDBTable_CurrentContentViewDBID.Add(kObjectID_Invalid);//B0406
	mDBTable_ListViewType.Add(inTableViewType);//#205
	
	//順番変更ボタン
	/*#688
	if( inUpID != kControlID_Invalid )
	{
		NVI_SetControlIcon(inUpID,kIconID_Abs_RowMoveUp);
	}
	if( inDownID != kControlID_Invalid )
	{
		NVI_SetControlIcon(inDownID,kIconID_Abs_RowMoveDown);
	}
	if( inTopID != kControlID_Invalid )
	{
		NVI_SetControlIcon(inTopID,kIconID_Abs_RowMoveTop);
	}
	if( inBottomID != kControlID_Invalid )
	{
		NVI_SetControlIcon(inBottomID,kIconID_Abs_RowMoveBottom);
	}*/
	
	//contentグループ
	if( inContentGroupControlID != kControlID_Invalid )//win 080707
	{
		NVI_SetControlEnable(inContentGroupControlID,false);
	}
}

//列の登録
void	AWindow::NVM_RegisterDBTableColumn( const AControlID inTableControlID, const ADataID inColumnID, const ADataID inDataID, 
		const AControlID inEditButtonControlID, const ANumber inColumnWidth, const AConstCharPtr inTitle, const ABool inEditable )
{
	//#205 TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().RegisterTableViewColumn(inTableControlID,inColumnID,NVM_GetDB().GetDataType(inDataID));
			break;
		}
	  case kTableViewType_ListView:
		{
			if( inDataID != kDataID_Invalid )
			{
				switch(NVM_GetDB().GetDataType(inDataID))
				{
				  case kDataType_BoolArray:
					{
						NVI_GetListView(inTableControlID).SPI_RegisterColumn_Bool(inColumnID,inColumnWidth,inTitle,inEditable);
						break;
					}
				  case kDataType_NumberArray:
					{
						NVI_GetListView(inTableControlID).SPI_RegisterColumn_Number(inColumnID,inColumnWidth,inTitle,inEditable);
						break;
					}
				  case kDataType_TextArray:
					{
						NVI_GetListView(inTableControlID).SPI_RegisterColumn_Text(inColumnID,inColumnWidth,inTitle,inEditable);
						break;
					}
				  case kDataType_ColorArray:
					{
						//NVI_GetListView(inTableControlID).SPI_RegisterColumn_Color(inColumnID);
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
			}
			break;
		}
	}
	
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	mDBTable_ColumnIDArray.GetObject(tableIndex).Add(inColumnID);
	mDBTable_DataIDArray.GetObject(tableIndex).Add(inDataID);
	mDBTable_ColumnEditButtonIDArray.GetObject(tableIndex).Add(inEditButtonControlID);
}

//列の登録
void	AWindow::NVM_RegisterDBTableColumn( const AControlID inTableControlID, const ADataID inColumnID, const ADataType inDataType,
		const AControlID inEditButtonControlID, const ANumber inColumnWidth, const AConstCharPtr inTitle, const ABool inEditable )
{
	//#205 TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().RegisterTableViewColumn(inTableControlID,inColumnID,inDataType);
			break;
		}
	  case kTableViewType_ListView:
		{
			switch(inDataType)
			{
			  case kDataType_BoolArray:
				{
					NVI_GetListView(inTableControlID).SPI_RegisterColumn_Bool(inColumnID,inColumnWidth,inTitle,inEditable);
					break;
				}
			  case kDataType_NumberArray:
				{
					NVI_GetListView(inTableControlID).SPI_RegisterColumn_Number(inColumnID,inColumnWidth,inTitle,inEditable);
					break;
				}
			  case kDataType_TextArray:
				{
					NVI_GetListView(inTableControlID).SPI_RegisterColumn_Text(inColumnID,inColumnWidth,inTitle,inEditable);
					break;
				}
			  case kDataType_ColorArray:
				{
					//NVI_GetListView(inTableControlID).SPI_RegisterColumn_Color(inColumnID);
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	mDBTable_ColumnIDArray.GetObject(tableIndex).Add(inColumnID);
	mDBTable_DataIDArray.GetObject(tableIndex).Add(kDataID_Invalid);
	mDBTable_ColumnEditButtonIDArray.GetObject(tableIndex).Add(inEditButtonControlID);
}

//#205
/**
TableViewTypeを取得
*/
ATableViewType	AWindow::GetTableViewType( const AControlID inTableControlID ) const
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	return mDBTable_ListViewType.Get(tableIndex);
}

#pragma mark ===========================

#pragma mark --- DB連動テーブルView表示データの設定
//#205

/**
テーブルView表示データ設定開始
*/
void	AWindow::NVI_BeginSetTable( const AControlID inTableControlID )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().BeginSetTable(inTableControlID);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_BeginSetTable();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

/**
テーブルView表示データ設定(Bool)
*/
void	AWindow::NVI_SetTableColumn_Bool( const AControlID inTableControlID, const ADataID inColumnID, const ABoolArray& inArray )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetTable_Bool(inTableControlID,inColumnID,inArray);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumn_Bool(inColumnID,inArray);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

/**
テーブルView表示データ設定(Number)
*/
void	AWindow::NVI_SetTableColumn_Number( const AControlID inTableControlID, const ADataID inColumnID, const ANumberArray& inArray )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetTable_Number(inTableControlID,inColumnID,inArray);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumn_Number(inColumnID,inArray);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

/**
テーブルView表示データ設定(Text)
*/
void	AWindow::NVI_SetTableColumn_Text( const AControlID inTableControlID, const ADataID inColumnID, const ATextArray& inArray )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetTable_Text(inTableControlID,inColumnID,inArray);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumn_Text(inColumnID,inArray);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

/**
テーブルView表示データ設定(Color)
*/
void	AWindow::NVI_SetTableColumn_Color( const AControlID inTableControlID, const ADataID inColumnID, const AColorArray& inArray )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetTable_Color(inTableControlID,inColumnID,inArray);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumn_Color(inColumnID,inArray);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

/**
テーブルView表示データ設定(Icon)
*/
/*#1012
void	AWindow::NVI_SetTableColumn_Icon( const AControlID inTableControlID, const ADataID inColumnID, const ANumberArray& inArray )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetColumnIcon(inTableControlID,inColumnID,inArray);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumnIcon(inColumnID,inArray);
			break;
		}
	}
}
*/

/**
テーブルView表示データ設定終了
*/
void	AWindow::NVI_EndSetTable( const AControlID inTableControlID )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().EndSetTable(inTableControlID);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_EndSetTable();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#205
/**
テーブルへのレングス0のテキスト入力を禁止する
*/
void	AWindow::NVM_SetInhibit0LengthForTable( const AControlID inTableControlID, const ABool inInhibit0Length )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetInhibit0LengthForTable(inTableControlID,inInhibit0Length);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetInhibit0Length(inInhibit0Length);
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

#pragma mark --- DB連動テーブルView表示データの取得
//#205

/**
テーブルView表示データ設定(Bool)
*/
const ABoolArray&	AWindow::NVI_GetTableColumn_Bool( const AControlID inTableControlID, const ADataID inColumnID )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return GetImp().GetColumn_BoolArray(inTableControlID,inColumnID);
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListView(inTableControlID).SPI_GetColumn_Bool(inColumnID);
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return GetEmptyBoolArray();
}

/**
テーブルView表示データ設定(Number)
*/
const ANumberArray&	AWindow::NVI_GetTableColumn_Number( const AControlID inTableControlID, const ADataID inColumnID )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return GetImp().GetColumn_NumberArray(inTableControlID,inColumnID);
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListView(inTableControlID).SPI_GetColumn_Number(inColumnID);
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return GetEmptyNumberArray();
}

/**
テーブルView表示データ設定(Text)
*/
const ATextArray&	AWindow::NVI_GetTableColumn_Text( const AControlID inTableControlID, const ADataID inColumnID )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return GetImp().GetColumn_TextArray(inTableControlID,inColumnID);
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListView(inTableControlID).SPI_GetColumn_Text(inColumnID);
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return GetEmptyTextArray();
}

/**
テーブルView表示データ設定(Color)
*/
const AColorArray&	AWindow::NVI_GetTableColumn_Color( const AControlID inTableControlID, const ADataID inColumnID )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return GetImp().GetColumn_ColorArray(inTableControlID,inColumnID);
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListView(inTableControlID).SPI_GetColumn_Color(inColumnID);
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return GetEmptyColorArray();
}

#pragma mark ===========================

#pragma mark --- DB連動テーブルView列の幅設定／取得
//#205

/**
テーブルView列の幅設定
*/
void	AWindow::NVI_SetTableColumnWidth( const AControlID inTableControlID, const ADataID inColumnID, const ANumber inWidth )
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			GetImp().SetColumnWidth(inTableControlID,inColumnID,inWidth);
			break;
		}
	  case kTableViewType_ListView:
		{
			NVI_GetListView(inTableControlID).SPI_SetColumnWidth(inColumnID,inWidth);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

/**
テーブルView列の幅取得
*/
ANumber	AWindow::NVI_GetTableColumnWidth( const AControlID inTableControlID, const ADataID inColumnID ) const
{
	//TableViewTypeによって処理切り替え
	switch(GetTableViewType(inTableControlID))
	{
	  case kTableViewType_OSControl:
		{
			return GetImpConst().GetColumnWidth(inTableControlID,inColumnID);
		}
	  case kTableViewType_ListView:
		{
			return NVI_GetListViewConst(inTableControlID).SPI_GetColumnWidth(inColumnID);
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return 100;
}

#pragma mark ===========================

#pragma mark --- DB連動「テーブル内容表示View」の登録

//「テーブル内容表示View」の登録
void	AWindow::NVM_RegisterDBTableContentView( const AControlID inTableControlID, const AControlID inContentControlID, const ADataID inDataID,
												 const ABool inReverseBool )//#1032
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	mDBTable_ContentIDArray.GetObject(tableIndex).Add(inContentControlID);
	mDBTable_ContentDataIDArray.GetObject(tableIndex).Add(inDataID);
	mDBTable_ContentReverseBool.GetObject(tableIndex).Add(inReverseBool);//#1032
}

//B0406
//ContentView のコントロールのうち、AWindowが自動管理せず、継承クラスで管理するものを登録する。
//ここで登録したコントロールをAWindowが変更することはない。
//当該コントロールが変更されたときに、対応するtableを見つけて、NVM_SaveContentView()をコールするためだけに使用する。
void	AWindow::NVM_RegisterDBTableSpecializedContentView( const AControlID inTableControlID, const AControlID inContentControlID )
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	mDBTable_SpecializedContentIDArray.GetObject(tableIndex).Add(inContentControlID);
}

#pragma mark ===========================

#pragma mark --- ContentView管理

//「テーブル内容View」の値をテーブルDBへ設定し、テーブル表示更新 B0406 旧名称NVM_SetDBTableDataFromContentView()からメソッド名変更
void	AWindow::NVM_SaveContentView( const AControlID inTableControlID )
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	if( tableIndex == kIndex_Invalid )   return;
	AIndex	rowIndex = mDBTable_CurrentContentViewRowIndex.Get(tableIndex);//B0406 mDBTable_CurrentSelect.Get(tableIndex);
	if( rowIndex == kIndex_Invalid )   return;
	//B0406 Open時のDBと同じDBに保存することを保証するため
	if( NVM_GetDB()./*#693GetDBID()*/GetObjectID() != mDBTable_CurrentContentViewDBID.Get(tableIndex) )
	{
		_ACError("DB changed",this);
		return;
	}
	
	for( AIndex contentIndex = 0; contentIndex < mDBTable_ContentIDArray.GetObject(tableIndex).GetItemCount(); contentIndex++ )
	{
		AControlID	contentControlID = mDBTable_ContentIDArray.GetObject(tableIndex).Get(contentIndex);
		ADataID	dataID = mDBTable_ContentDataIDArray.GetObject(tableIndex).Get(contentIndex);
		ABool	reverseBool = mDBTable_ContentReverseBool.GetObject(tableIndex).Get(contentIndex);//#1032
		if( dataID != kDataID_Invalid )
		{
			switch(NVM_GetDB().GetDataType(dataID))
			{
			  case kDataType_BoolArray:
				{
					ABool	data;
					NVI_GetControlBool(contentControlID,data);
					//#1032
					if( reverseBool == true )
					{
						data = !data;
					}
					//
					NVM_GetDB().SetData_BoolArray(dataID,rowIndex,data);
					break;
				}
			  case kDataType_NumberArray:
				{
					ANumber	data;
					NVI_GetControlNumber(contentControlID,data);
					NVM_GetDB().SetData_NumberArray(dataID,rowIndex,data);
					//データ制約反映
					data = NVM_GetDB().GetData_NumberArray(dataID,rowIndex);
					NVI_SetControlNumber(contentControlID,data);
					break;
				}
			  case kDataType_TextArray:
				{
					AText	data;
					NVI_GetControlText(contentControlID,data);
					NVM_GetDB().SetData_TextArray(dataID,rowIndex,data);
					//データ制約反映
					NVM_GetDB().GetData_TextArray(dataID,rowIndex,data);
					NVI_SetControlText(contentControlID,data);
					break;
				}
			  case kDataType_ColorArray:
				{
					AColor	data;
					NVI_GetControlColor(contentControlID,data);
					NVM_GetDB().SetData_ColorArray(dataID,rowIndex,data);
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
		}
		//B0406 NVMDO_SetDBTableDataFromContentView(inTableControlID,contentControlID,rowIndex);
	}
	NVMDO_SaveSpecializedContentView(inTableControlID,rowIndex);//B0406
	NVM_UpdateDBTableView(inTableControlID);
}

//「テーブル内容View」の表示を、現在のテーブル選択行と、DBのデータに合わせて更新
void	AWindow::OpenContentView( const AControlID inTableControlID )
{
	/*#205
#if USE_IMP_TABLEVIEW
	AIndex	rowIndex = NVI_GetControlNumber(inTableControlID);
#else
	AIndex	rowIndex = NVI_GetListView(inTableControlID).SPI_GetSelectedDBIndex();
	#endif
	*/
	AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(inTableControlID);//#205
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	if( tableIndex != kIndex_Invalid )
	{
		//#391
		//AView_EditBox::EVTDO_DoViewFocusDeactivated()にてEVT_ValueChanged()をコールすることにしたため、
		//下にあるNVI_SetControlEnable()からResetFocus()がコールされ、
		//mDBTable_CurrentContentViewRowIndex, mDBTable_CurrentContentViewDBIDが前の値のまま
		//EVT_ValueChanged()経由でNVM_SaveContentView()がコールされ、
		//変更途中のデータが前のインデックスに保存される問題が発生した。
		//これを防ぐために、先にmDBTable_CurrentContentViewRowIndex, mDBTable_CurrentContentViewDBIDをクリアしておく
		mDBTable_CurrentContentViewRowIndex.Set(tableIndex,kIndex_Invalid);
		mDBTable_CurrentContentViewDBID.Set(tableIndex,kObjectID_Invalid);
		
		//ContentグループのEnable/Disableの設定
		AControlID	contentGroupControlID = mDBTable_ContentGroupControlID.Get(tableIndex);
		if( contentGroupControlID != kControlID_Invalid )
		{
			NVI_SetControlEnable(contentGroupControlID,(rowIndex!=kIndex_Invalid));
		}
		//
		for( AIndex contentItemIndex = 0; contentItemIndex < mDBTable_ContentIDArray.GetObject(tableIndex).GetItemCount(); contentItemIndex++ )
		{
			AControlID	contentControlID = mDBTable_ContentIDArray.GetObject(tableIndex).Get(contentItemIndex);
			if( rowIndex == kIndex_Invalid )
			{
				NVI_SetControlEnable(contentControlID,false);
				//#182 GetImp().SetEmptyValue(contentControlID);
				ADataID	dataID = mDBTable_ContentDataIDArray.GetObject(tableIndex).Get(contentItemIndex);
				if( dataID != kDataID_Invalid )
				{
					switch(NVM_GetDB().GetDataType(dataID))
					{
					  case kDataType_BoolArray:
						{
							NVI_SetControlBool(contentControlID,false);
							break;
						}
					  case kDataType_NumberArray:
						{
							NVI_SetControlNumber(contentControlID,0);
							break;
						}
					  case kDataType_TextArray:
						{
							NVI_SetControlText(contentControlID,GetEmptyText());
							break;
						}
					  case kDataType_ColorArray:
						{
							NVI_SetControlColor(contentControlID,kColor_Gray);
							break;
						}
					  default:
						{
							//処理なし
							break;
						}
					}
				}
			}
			else
			{
				NVI_SetControlEnable(contentControlID,true);
				ADataID	dataID = mDBTable_ContentDataIDArray.GetObject(tableIndex).Get(contentItemIndex);
				ABool	reverseBool = mDBTable_ContentReverseBool.GetObject(tableIndex).Get(contentItemIndex);//#1032
				if( dataID != kDataID_Invalid )
				{
					switch(NVM_GetDB().GetDataType(dataID))
					{
					  case kDataType_BoolArray:
						{
							ABool	data = NVM_GetDB().GetData_BoolArray(dataID,rowIndex);
							//#1032
							if( reverseBool == true )
							{
								data = !data;
							}
							//
							NVI_SetControlBool(contentControlID,data);
							break;
						}
					  case kDataType_NumberArray:
						{
							ANumber	data = NVM_GetDB().GetData_NumberArray(dataID,rowIndex);
							NVI_SetControlNumber(contentControlID,data);
							break;
						}
					  case kDataType_TextArray:
						{
							AText	text;
							NVM_GetDB().GetData_TextArray(dataID,rowIndex,text);
							NVI_SetControlText(contentControlID,text);
							break;
						}
					  case kDataType_ColorArray:
						{
							AColor	data;
							NVM_GetDB().GetData_ColorArray(dataID,rowIndex,data);
							NVI_SetControlColor(contentControlID,data);
							break;
						}
					  default:
						{
							//処理なし
							break;
						}
					}
				}
				//B0406 NVMDO_UpdateTableContentView(inTableControlID,contentControlID);
				NVI_ClearControlUndoInfo(contentControlID);//B0323
				/*#688
				//ContentView内の最初の項目へフォーカス移動
				if( contentItemIndex == 0 && sIsFocusGroup2ndPattern == false )//#353 フルキーボードアクセスの場合はフォーカス移動しない
				{
					NVI_SwitchFocusTo(contentControlID);
				}
				*/
			}
		}
		NVMDO_OpenSpecializedContentView(inTableControlID,rowIndex);//B0406
		//B0406
		mDBTable_CurrentContentViewRowIndex.Set(tableIndex,rowIndex);
		mDBTable_CurrentContentViewDBID.Set(tableIndex,NVM_GetDB()./*#693GetDBID()*/GetObjectID());
	}
}


//
/**
「テーブル内容View」の中のコントロールの値が変更されたときの処理
*/
ABool	AWindow::DoValueChanged_DBTableContentView( const AControlID inControlID )
{
	for( AIndex tableIndex = 0; tableIndex < mDBTable_ControlID.GetItemCount(); tableIndex++ )
	{
		for( AIndex contentIndex = 0; contentIndex < mDBTable_ContentIDArray.GetObject(tableIndex).GetItemCount(); contentIndex++ )
		{
			AControlID	contentControlID = mDBTable_ContentIDArray.GetObject(tableIndex).Get(contentIndex);
			if( inControlID == contentControlID )
			{
				AControlID	tableControlID = mDBTable_ControlID.Get(tableIndex);
				NVM_SaveContentView(tableControlID);
				if( mDBTable_NotifyDataChanged.Get(tableIndex) == true && mDBTable_CurrentSelect.Get(tableIndex) != kIndex_Invalid )
				{
					NVMDO_NotifyDataChanged(tableControlID,kModificationType_ValueChanged,mDBTable_CurrentSelect.Get(tableIndex),inControlID/*B0406*/);
				}
				return true;
			}
		}
		//B0406
		for( AIndex contentIndex = 0; contentIndex < mDBTable_SpecializedContentIDArray.GetObject(tableIndex).GetItemCount(); contentIndex++ )
		{
			AControlID	contentControlID = mDBTable_SpecializedContentIDArray.GetObject(tableIndex).Get(contentIndex);
			if( inControlID == contentControlID )
			{
				AControlID	tableControlID = mDBTable_ControlID.Get(tableIndex);
				NVM_SaveContentView(tableControlID);
				if( mDBTable_NotifyDataChanged.Get(tableIndex) == true && mDBTable_CurrentSelect.Get(tableIndex) != kIndex_Invalid )
				{
					NVMDO_NotifyDataChanged(tableControlID,kModificationType_ValueChanged,mDBTable_CurrentSelect.Get(tableIndex),inControlID/*B0406*/);
				}
				return true;
			}
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark ---DB連動テーブル個別Update

//テーブルの表示をDBのデータに合わせて更新
void	AWindow::NVM_UpdateDBTableView( const AControlID inTableControlID )
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	if( tableIndex != kIndex_Invalid )
	{
		//テーブルデータ設定開始
		NVI_BeginSetTable(inTableControlID);
		//各列ごと
		for( AIndex columnIndex = 0; columnIndex < mDBTable_ColumnIDArray.GetObject(tableIndex).GetItemCount(); columnIndex++ )
		{
			ADataID	columnID = mDBTable_ColumnIDArray.GetObject(tableIndex).Get(columnIndex);
			AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
			ADataID	dataID = mDBTable_DataIDArray.GetObject(tableIndex).Get(columnIndex);
			if( dataID != kDataID_Invalid )
			{
				switch(NVM_GetDB().GetDataType(dataID))
				{
				  case kDataType_BoolArray:	
					{
						NVI_SetTableColumn_Bool(inTableControlID,columnID,NVM_GetDB().GetData_ConstBoolArrayRef(dataID));
						break;
					}
				  case kDataType_NumberArray:
					{
						NVI_SetTableColumn_Number(inTableControlID,columnID,NVM_GetDB().GetData_ConstNumberArrayRef(dataID));
						break;
					}
				  case kDataType_TextArray:
					{
						NVI_SetTableColumn_Text(inTableControlID,columnID,NVM_GetDB().GetData_ConstTextArrayRef(dataID));
						break;
					}
				  case kDataType_ColorArray:
					{
						NVI_SetTableColumn_Color(inTableControlID,columnID,NVM_GetDB().GetData_ConstColorArrayRef(dataID));
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
			}
			//派生クラスでの処理
			NVMDO_UpdateTableView(inTableControlID,columnID);
		}
		//テーブルデータ設定終了
		NVI_EndSetTable(inTableControlID);
/*#205
#if USE_IMP_TABLEVIEW
		GetImp().BeginSetTable(inTableControlID);
		for( AIndex columnIndex = 0; columnIndex < mDBTable_ColumnIDArray.GetObject(tableIndex).GetItemCount(); columnIndex++ )
		{
			ADataID	columnID = mDBTable_ColumnIDArray.GetObject(tableIndex).Get(columnIndex);
			
			AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
			//B0000 forの中で定義済みAIndex	columnIndex = mDBTable_ColumnIDArray.GetObject(tableIndex).Find(columnID);
			ADataID	dataID = mDBTable_DataIDArray.GetObject(tableIndex).Get(columnIndex);
			if( dataID != kDataID_Invalid )
			{
				switch(NVM_GetDB().GetDataType(dataID))
				{
				  case kDataType_BoolArray:
					{
						GetImp().SetTable_Bool(inTableControlID,columnID,NVM_GetDB().GetData_ConstBoolArrayRef(dataID));
						break;
					}
				  case kDataType_NumberArray:
					{
						GetImp().SetTable_Number(inTableControlID,columnID,NVM_GetDB().GetData_ConstNumberArrayRef(dataID));
						break;
					}
				  case kDataType_TextArray:
					{
						GetImp().SetTable_Text(inTableControlID,columnID,NVM_GetDB().GetData_ConstTextArrayRef(dataID));
						break;
					}
				  case kDataType_ColorArray:
					{
						GetImp().SetTable_Color(inTableControlID,columnID,NVM_GetDB().GetData_ConstColorArrayRef(dataID));
						break;
					}
				}
			}
			NVMDO_UpdateTableView(inTableControlID,columnID);
		}
		GetImp().EndSetTable(inTableControlID);
#else
		NVI_GetListView(inTableControlID).SPI_BeginSetTable();
		for( AIndex columnIndex = 0; columnIndex < mDBTable_ColumnIDArray.GetObject(tableIndex).GetItemCount(); columnIndex++ )
		{
			ADataID	columnID = mDBTable_ColumnIDArray.GetObject(tableIndex).Get(columnIndex);
			
			AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
			//B0000 forの中で定義済みAIndex	columnIndex = mDBTable_ColumnIDArray.GetObject(tableIndex).Find(columnID);
			ADataID	dataID = mDBTable_DataIDArray.GetObject(tableIndex).Get(columnIndex);
			if( dataID != kDataID_Invalid )
			{
				switch(NVM_GetDB().GetDataType(dataID))
				{
				  case kDataType_BoolArray:
					{
						NVI_GetListView(inTableControlID).SPI_SetColumn_Bool(columnID,NVM_GetDB().GetData_ConstBoolArrayRef(dataID));
						break;
					}
				  case kDataType_NumberArray:
					{
						NVI_GetListView(inTableControlID).SPI_SetColumn_Number(columnID,NVM_GetDB().GetData_ConstNumberArrayRef(dataID));
						break;
					}
				  case kDataType_TextArray:
					{
						NVI_GetListView(inTableControlID).SPI_SetColumn_Text(columnID,NVM_GetDB().GetData_ConstTextArrayRef(dataID));
						break;
					}
				  case kDataType_ColorArray:
					{
						NVI_GetListView(inTableControlID).SPI_SetColumn_Color(columnID,NVM_GetDB().GetData_ConstColorArrayRef(dataID));
						break;
					}
				}
			}
			NVMDO_UpdateTableView(inTableControlID,columnID);
		}
		NVI_GetListView(inTableControlID).SPI_EndSetTable();
#endif
*/
	}
}

#pragma mark ===========================

#pragma mark <内部モジュール>

#pragma mark ===========================

#pragma mark ---DB連動データあり？

ABool	AWindow::IsDBDataTableExist()
{
	return ((mDBData_ControlID.GetItemCount()>0)||(mDBTable_ControlID.GetItemCount()>0));
}

#pragma mark ===========================

#pragma mark ---DB連動データのUpdate

void	AWindow::UpdateDBDataAndTable()
{
	//登録データの値をDBからコントロールへ反映
	for( AIndex index = 0; index < mDBData_ControlID.GetItemCount(); index++ )
	{
		AControlID	controlID = mDBData_ControlID.Get(index);
		ADataID	dataID = mDBData_DataID.Get(index);
		
		ADataType	datatype = NVM_GetDB().GetDataType(dataID);
		NVM_SetControlDataType(controlID,datatype);
		switch(datatype)
		{
		  case kDataType_Bool:
			{
				if( mDBData_ReverseBool.Get(index) == false )
				{
					NVI_SetControlBool(controlID,NVM_GetDB().GetData_Bool(dataID));
				}
				else
				{
					NVI_SetControlBool(controlID,(NVM_GetDB().GetData_Bool(dataID)==false));
				}
				break;
			}
		  case kDataType_Number:
			{
				NVI_SetControlNumber(controlID,NVM_GetDB().GetData_Number(dataID));
				break;
			}
		  case kDataType_FloatNumber:
			{
				NVI_SetControlFloatNumber(controlID,NVM_GetDB().GetData_FloatNumber(dataID));
				break;
			}
		  case kDataType_Color:
			{
				AColor	color;
				NVM_GetDB().GetData_Color(dataID,color);
				NVI_SetControlColor(controlID,color);
				break;
			}
		  case kDataType_Text:
			{
				AText	text;
				NVM_GetDB().GetData_Text(dataID,text);
				NVI_SetControlText(controlID,text);
				break;
			}
		  /*win case kDataType_Font:
			{
				GetImp().SetFont(controlID,NVM_GetDB().GetData_Font(dataID));//暫定
				break;
			}*/
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	//登録テーブルデータの値をDBからコントロールへ反映
	for( AIndex index = 0; index < mDBTable_ControlID.GetItemCount(); index++ )
	{
		//テーブル表示更新
		NVM_UpdateDBTableView(mDBTable_ControlID.Get(index));
		//「テーブル内容View」表示更新
		OpenContentView(mDBTable_ControlID.Get(index));
	}
	//コントロール状態更新
	UpdateDBControlStatus();
}

//コントロール状態更新
void	AWindow::UpdateDBControlStatus()
{
	for( AIndex	dataIndex = 0; dataIndex < mDBData_DataID.GetItemCount(); dataIndex++ )
	{
		AControlID	enablerControlID = mDBData_EnablerControlID.Get(dataIndex);
		if( enablerControlID != kControlID_Invalid )
		{
			ABool	enabled = NVI_GetControlBool(enablerControlID);//#349
			if( GetImpConst().IsControlEnabled(enablerControlID) == false )   enabled = false;//#349 enablerがgrayout中なら常にgrayoutにする
			NVI_SetControlEnable(/*#243 mDBData_DataID*/mDBData_ControlID.Get(dataIndex),enabled);//#349 NVI_GetControlBool(enablerControlID));
		}
		AControlID	disablerControlID = mDBData_DisablerControlID.Get(dataIndex);
		if( disablerControlID != kControlID_Invalid )
		{
			ABool	enabled = (NVI_GetControlBool(disablerControlID)==false);//#349
			if( GetImpConst().IsControlEnabled(disablerControlID) == false )   enabled = false;//#349 enablerがgrayout中なら常にgrayoutにする
			NVI_SetControlEnable(/*#243 mDBData_DataID*/mDBData_ControlID.Get(dataIndex),enabled);//#349 (NVI_GetControlBool(disablerControlID)==false));
		}
	}
	
	for( AIndex	tableIndex = 0; tableIndex < mDBTable_ControlID.GetItemCount(); tableIndex++ )
	{
		AControlID	tableControlID = mDBTable_ControlID.Get(tableIndex);
		
		/** #205
#if USE_IMP_TABLEVIEW
		AIndex	rowIndex = NVI_GetControlNumber(tableControlID);
#else
		AIndex	rowIndex = NVI_GetListView(tableControlID).SPI_GetSelectedDBIndex();
#endif
		*/
		AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(tableControlID);//#205
		ABool	rowSelected = ( rowIndex != kIndex_Invalid);
		AControlID	id = mDBTable_DeleteRow_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}
		/* win 080703 id = mDBTable_DeleteRowOK_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}*/
		id = mDBTable_Up_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}
		id = mDBTable_Down_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}
		id = mDBTable_Top_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}
		id = mDBTable_Bottom_ControlID.Get(tableIndex);
		if( id != kControlID_Invalid )
		{
			NVI_SetControlEnable(id,rowSelected);
		}
		for( AIndex i = 0; i < mDBTable_ColumnEditButtonIDArray.GetObject(tableIndex).GetItemCount(); i++ )
		{
			id = mDBTable_ColumnEditButtonIDArray.GetObject(tableIndex).Get(i);
			if( id != kControlID_Invalid )
			{
				NVI_SetControlEnable(id,rowSelected);
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---DB連動データのクリック時処理

//テーブルクリック処理
ABool	AWindow::DoClicked_DBTable( const AControlID inControlID )
{
	//テーブル選択
	AIndex	tableIndex = mDBTable_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		NVM_SaveContentView(inControlID);
		//CurrentSelectを設定。複数選択の場合、NVI_GetTableViewSelectionDBIndex()はkIndex_Invalidを返す。
		mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(inControlID));//#205
		//
		OpenContentView(inControlID);
		return true;
	}
	
	//テーブル行追加ボタン
	tableIndex = mDBTable_AddRow_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		//データの追加
		ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
		NVM_GetDB().AddRow_Table(firstDataID);
		//#625 NVMDO_NotifyDataChanged()と役割重複のため、こちらは削除 NVI_AddTableRow(tableID);
		NVM_UpdateDBTableView(tableID);
		//最終行を選択
		SetTableViewSelectionByDBIndex(tableID,NVM_GetDB().GetRowCount_Table(firstDataID)-1);//#205
		mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(tableID));//#205
		//
		OpenContentView(tableID);
		NVM_UpdateControlStatus();
		//データ変更通知
		if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
		{
			NVMDO_NotifyDataChanged(tableID,kModificationType_RowAdded,NVI_GetTableViewSelectionDBIndex(tableID),inControlID);//#205
		}
		//#625 複数選択対応 行追加ボタンによる行追加が完了したことを通知
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_AddNew,kIndex_Invalid,inControlID);
		return true;
	}
	//テーブル行削除ボタン
	tableIndex = mDBTable_DeleteRow_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		AArray<AIndex>	rowIndexArray;//#625
		SPI_GetTableViewSelectionDBIndexArray(tableID,rowIndexArray);//#625
		if( rowIndexArray.GetItemCount() > 0 )//#625
		{
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("ModePref_DeleteGeneral1");
			mes2.SetLocalizedText("ModePref_DeleteGeneral2");
			mes3.SetLocalizedText("ModePref_DeleteGeneral3");
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,mDBTable_DeleteRowOK_ControlID.Get(tableIndex));
		}
		return true;
	}
	//テーブル行削除OKボタン
	tableIndex = mDBTable_DeleteRowOK_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		AArray<AIndex>	rowIndexArray;//#625
		SPI_GetTableViewSelectionDBIndexArray(tableID,rowIndexArray);//#625
		SetTableViewSelectionByDBIndex(tableID,kIndex_Invalid);//選択解除 #625
		rowIndexArray.Sort(false);//DB上の後ろのデータのほうから削除するために降順ソートする#625
		for( AIndex i = 0; i < rowIndexArray.GetItemCount(); i++ )//#625
		{
			AIndex	rowIndex = rowIndexArray.Get(i);//#625
			//データの削除
			ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
			NVM_GetDB().DeleteRow_Table(firstDataID,rowIndex);
			mDBTable_CurrentSelect.Set(tableIndex,kIndex_Invalid);
			NVM_UpdateDBTableView(tableID);
			OpenContentView(tableID);
			NVM_UpdateControlStatus();
			//データ変更通知
			if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
			{
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowDeleted,rowIndex,inControlID/*B0406*/);
			}
		}
		//#625 複数選択対応 全ての選択行の削除が完了したことを通知
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_DeleteRows,kIndex_Invalid,inControlID);
		return true;
	}
	//テーブル行Upボタン R0183
	//#205 //★DBIndexベースでの処理になっているので、ソート表示する場合、表示行ベースに対応必要
	tableIndex = mDBTable_Up_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(tableID);//#205
		if( rowIndex != kIndex_Invalid && rowIndex > 0)
		{
			//データの移動
			ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
			NVM_GetDB().MoveRow_Table(firstDataID,rowIndex,rowIndex-1);
			NVM_UpdateDBTableView(tableID);
			//移動後の行を選択
			SetTableViewSelectionByDBIndex(tableID,rowIndex-1);//#205
			mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(tableID));//#205
			//
			OpenContentView(tableID);
			NVM_UpdateControlStatus();
			//データ変更通知
			if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
			{
				//メニューの更新等の処理を簡単にするため、deleted, addedだけを使う
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowDeleted,rowIndex,inControlID/*B0406*/);
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowAdded,NVI_GetTableViewSelectionDBIndex(tableID),inControlID);//#205
			}
		}
		//#625 複数選択対応 行移動ボタンによる行移動が完了したことを通知
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_MoveRow,kIndex_Invalid,inControlID);
		return true;
	}
	//テーブル行Downボタン R0183
	tableIndex = mDBTable_Down_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
		AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(tableID);//#205
		if( rowIndex != kIndex_Invalid && rowIndex < NVM_GetDB().GetItemCount_Array(firstDataID)-1 )
		{
			//データの移動
			NVM_GetDB().MoveRow_Table(firstDataID,rowIndex,rowIndex+1);
			NVM_UpdateDBTableView(tableID);
			//移動後の行を選択
			SetTableViewSelectionByDBIndex(tableID,rowIndex+1);//#205
			mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(tableID));//#205
			//
			OpenContentView(tableID);
			NVM_UpdateControlStatus();
			//データ変更通知
			if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
			{
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowDeleted,rowIndex,inControlID/*B0406*/);
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowAdded,NVI_GetTableViewSelectionDBIndex(tableID),inControlID);//#205
			}
		}
		//#625 複数選択対応 行移動ボタンによる行移動が完了したことを通知
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_MoveRow,kIndex_Invalid,inControlID);
		return true;
	}
	//テーブル行Topボタン R0183
	tableIndex = mDBTable_Top_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(tableID);//#205
		if( rowIndex != kIndex_Invalid && rowIndex > 0 )
		{
			//データの移動
			ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
			NVM_GetDB().MoveRow_Table(firstDataID,rowIndex,0);
			NVM_UpdateDBTableView(tableID);
			//移動後の行を選択
			SetTableViewSelectionByDBIndex(tableID,0);//#205
			mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(tableID));//#205
			//
			OpenContentView(tableID);
			NVM_UpdateControlStatus();
			//データ変更通知
			if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
			{
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowDeleted,rowIndex,inControlID/*B0406*/);
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowAdded,NVI_GetTableViewSelectionDBIndex(tableID),inControlID);//#205
			}
		}
		//#625 複数選択対応 行移動ボタンによる行移動が完了したことを通知
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_MoveRow,kIndex_Invalid,inControlID);
		return true;
	}
	//テーブル行Bottomボタン R0183
	tableIndex = mDBTable_Bottom_ControlID.Find(inControlID);
	if( tableIndex != kIndex_Invalid )
	{
		AControlID	tableID = mDBTable_ControlID.Get(tableIndex);
		ADataID	firstDataID = mDBTable_FirstDataID.Get(tableIndex);
		AIndex	rowIndex = NVI_GetTableViewSelectionDBIndex(tableID);//#205
		if( rowIndex != kIndex_Invalid && rowIndex < NVM_GetDB().GetItemCount_Array(firstDataID)-1 )
		{
			//データの移動
			NVM_GetDB().MoveRow_Table(firstDataID,rowIndex,NVM_GetDB().GetItemCount_Array(firstDataID)-1);
			NVM_UpdateDBTableView(tableID);
			//移動後の行を選択
			SetTableViewSelectionByDBIndex(tableID,NVM_GetDB().GetItemCount_Array(firstDataID)-1);//#205
			mDBTable_CurrentSelect.Set(tableIndex,NVI_GetTableViewSelectionDBIndex(tableID));//#205
			//
			OpenContentView(tableID);
			NVM_UpdateControlStatus();
			//データ変更通知
			if( mDBTable_NotifyDataChanged.Get(tableIndex) == true )
			{
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowDeleted,rowIndex,inControlID/*B0406*/);
				NVMDO_NotifyDataChanged(tableID,kModificationType_RowAdded,NVI_GetTableViewSelectionDBIndex(tableID),inControlID);//#205
			}
		}
		//#625 複数選択対応 行移動ボタンによる行移動が完了したことを通知
		NVMDO_NotifyDataChanged(tableID,kModificationType_TransactionCompleted_MoveRow,kIndex_Invalid,inControlID);
		return true;
	}
	//編集ボタン
	for( tableIndex = 0; tableIndex < mDBTable_ControlID.GetItemCount(); tableIndex++ )
	{
		AIndex	index = mDBTable_ColumnEditButtonIDArray.GetObject(tableIndex).Find(inControlID);
		if( index != kIndex_Invalid )
		{
			NVI_StartTableColumnEditMode(mDBTable_ControlID.Get(tableIndex),mDBTable_ColumnIDArray.GetObject(tableIndex).Get(index));
			return true;
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark ---DB連動データの値変更時処理

//DBData値変更時処理
ABool	AWindow::DoValueChanged_DBData( const AControlID inControlID )
{
	AIndex	dataIndex = mDBData_ControlID.Find(inControlID);
	if( dataIndex != kIndex_Invalid )
	{
		ABool	changed = false;
		ADataID	dataID = mDBData_DataID.Get(dataIndex);
		switch(NVM_GetDB().GetDataType(dataID))
		{
		  case kDataType_Bool:
			{
				ABool	data;
				NVI_GetControlBool(inControlID,data);
				if( mDBData_ReverseBool.Get(dataIndex) == true )
				{
					data = (data==false);
				}
				if( NVM_GetDB().GetData_Bool(dataID) != data )
				{
					NVM_GetDB().SetData_Bool(dataID,data);
					changed = true;
				}
				break;
			}
		  case kDataType_Number:
			{
				ANumber	data;
				NVI_GetControlNumber(inControlID,data);
				if( NVM_GetDB().GetData_Number(dataID) != data )
				{
					NVM_GetDB().SetData_Number(dataID,data);
					//DB上の値制限の結果をUIへ反映させる
					NVI_SetControlNumber(inControlID,NVM_GetDB().GetData_Number(dataID));
					changed = true;
				}
				break;
			}
		  case kDataType_FloatNumber:
			{
				AFloatNumber	data;
				NVI_GetControlFloatNumber(inControlID,data);
				if( NVM_GetDB().GetData_FloatNumber(dataID) != data )
				{
					NVM_GetDB().SetData_FloatNumber(dataID,data);
					//DB上の値制限の結果をUIへ反映させる
					NVI_SetControlFloatNumber(inControlID,NVM_GetDB().GetData_FloatNumber(dataID));
					changed = true;
				}
				break;
			}
		  case kDataType_Color:
			{
				AColor	data;
				NVI_GetControlColor(inControlID,data);
				AColor	olddata;
				NVM_GetDB().GetData_Color(dataID,olddata);
				if( AColorWrapper::CompareColor(olddata,data) == false )
				{
					NVM_GetDB().SetData_Color(dataID,data);
					changed = true;
				}
				break;
			}
		  case kDataType_Text:
			{
				AText	data;
				NVI_GetControlText(inControlID,data);
				AText	olddata;
				NVM_GetDB().GetData_Text(dataID,olddata);
				if( data.Compare(olddata) == false )
				{
					NVM_GetDB().SetData_Text(dataID,data);
					changed = true;
				}
				break;
			}
		  /*win case kDataType_Font:
			{
				AFont	data;
				GetImp().GetFont(inControlID,data);//暫定
				if( NVM_GetDB().GetData_Font(dataID) != data )
				{
					NVM_GetDB().SetData_Font(dataID,data);
					changed = true;
				}
				break;
			}*/
		  default:
			{
				//処理なし
				break;
			}
		}
		if( mDBData_NotifyDataChanged.Get(dataIndex) == true && changed == true )
		{
			NVMDO_NotifyDataChanged(inControlID,kModificationType_ValueChanged,kIndex_Invalid,inControlID/*B0406*/);
		}
		return true;
	}
	return false;
}

//UI→DB
//テーブル(UI)が書き換えられたとき、コールされる。
//テーブル(UI)からデータ（配列）を取得し、DBへコピーする。その後、DB上から、UIへ逆コピーすることで、DB上の値制限の結果をUIへ反映させる。
ABool	AWindow::DoValueChanged_DBTable( const AControlID inTableControlID )
{
	AIndex	tableIndex = mDBTable_ControlID.Find(inTableControlID);
	if( tableIndex != kIndex_Invalid )
	{
		for( AIndex columnIndex = 0; columnIndex < mDBTable_ColumnIDArray.GetObject(tableIndex).GetItemCount(); columnIndex++ )
		{
			ADataID	columnID = mDBTable_ColumnIDArray.GetObject(tableIndex).Get(columnIndex);
			ADataID	dataID = mDBTable_DataIDArray.GetObject(tableIndex).Get(columnIndex);
			if( dataID == kIndex_Invalid )   continue;
			
			//
			switch(NVM_GetDB().GetDataType(dataID))
			{
			  case kDataType_BoolArray:
				{
					NVM_GetDB().SetData_BoolArray(dataID,NVI_GetTableColumn_Bool(inTableControlID,columnID));
					break;
				}
			  case kDataType_NumberArray:
				{
					NVM_GetDB().SetData_NumberArray(dataID,NVI_GetTableColumn_Number(inTableControlID,columnID));
					break;
				}
			  case kDataType_TextArray:
				{
					NVM_GetDB().SetData_TextArray(dataID,NVI_GetTableColumn_Text(inTableControlID,columnID));
					break;
				}
			  case kDataType_ColorArray:
				{
					NVM_GetDB().SetData_ColorArray(dataID,NVI_GetTableColumn_Color(inTableControlID,columnID));
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			/*#205
#if USE_IMP_TABLEVIEW
			switch(NVM_GetDB().GetDataType(dataID))
			{
			  case kDataType_BoolArray:
				{
					NVM_GetDB().SetData_BoolArray(dataID,GetImp().GetColumn_BoolArray(inTableControlID,columnID));
					break;
				}
			  case kDataType_NumberArray:
				{
					NVM_GetDB().SetData_NumberArray(dataID,GetImp().GetColumn_NumberArray(inTableControlID,columnID));
					break;
				}
			  case kDataType_TextArray:
				{
					NVM_GetDB().SetData_TextArray(dataID,GetImp().GetColumn_TextArray(inTableControlID,columnID));
					break;
				}
			  case kDataType_ColorArray:
				{
					NVM_GetDB().SetData_ColorArray(dataID,GetImp().GetColumn_ColorArray(inTableControlID,columnID));
					break;
				}
			}
#else
			switch(NVM_GetDB().GetDataType(dataID))
			{
			  case kDataType_BoolArray:
				{
					NVM_GetDB().SetData_BoolArray(dataID,NVI_GetListView(inTableControlID).SPI_GetColumn_Bool(columnID));
					break;
				}
			  case kDataType_NumberArray:
				{
					NVM_GetDB().SetData_NumberArray(dataID,NVI_GetListView(inTableControlID).SPI_GetColumn_Number(columnID));
					break;
				}
			  case kDataType_TextArray:
				{
					NVM_GetDB().SetData_TextArray(dataID,NVI_GetListView(inTableControlID).SPI_GetColumn_Text(columnID));
					break;
				}
			  case kDataType_ColorArray:
				{
					NVM_GetDB().SetData_ColorArray(dataID,NVI_GetListView(inTableControlID).SPI_GetColumn_Color(columnID));
					break;
				}
			}
#endif
			*/
		}
		if( mDBTable_NotifyDataChanged.Get(tableIndex) == true && mDBTable_CurrentSelect.Get(tableIndex) != kIndex_Invalid )
		{
			NVMDO_NotifyDataChanged(inTableControlID,kModificationType_ValueChanged,mDBTable_CurrentSelect.Get(tableIndex),inTableControlID/*B0406*/);
		}
		//DB上の値制限の結果をUIへ反映させる
		NVM_UpdateDBTableView(inTableControlID);
		return true;
	}
	return false;
}
void	AWindow::NVI_DoValueChanged( const AControlID inID )
{
	if( IsDBDataTableExist() == true )
	{
		if( DoValueChanged_DBData(inID) == true )   return;
		if( DoValueChanged_DBTable(inID) == true )   return;
		if( DoValueChanged_DBTableContentView(inID) == true )   return;
	}
}

void	AWindow::NVM_StoreToDBAll()
{
	for( AIndex i = 0; i < mDBData_ControlID.GetItemCount(); i++ )
	{
		NVI_DoValueChanged(mDBData_ControlID.Get(i));
	}
	for( AIndex i = 0; i < mDBTable_ContentIDArray.GetItemCount(); i++ )
	{
		for( AIndex j = 0; j < mDBTable_ContentIDArray.GetObject(i).GetItemCount(); j++ )
		{
			NVI_DoValueChanged(mDBTable_ContentIDArray.GetObject(i).Get(j));
		}
	}
	//★mDBTable_SpecializedContentIDArrayも必要？
	//そもそもContentViewのコントロール一つ一つに対してDoValueChanged実行するのはおかしい（もったいない）？
	for( AIndex i = 0; i < mDBTable_ControlID.GetItemCount(); i++ )
	{
		NVI_DoValueChanged(mDBTable_ControlID.Get(i));
	}
}

#pragma mark ===========================

#pragma mark ---ListView

//ListView生成（フレーム、スクロールバーがリソース上に定義されている場合用）
//inListViewControlID: ListViewのControlID
//inListViewFrameControlID: フレームのControlID（フレーム無しの場合はkControlID_Invalid)
//inScrollBarControlID: VScrollBarのControlID（VScrollBar無しの場合はkControlID_Invalid)
/*#205
void	AWindow::NVI_CreateListView( const AControlID inListViewControlID, const AControlID inListViewFrameControlID,
		const AControlID inScrollBarControlID )
{
	if( inListViewFrameControlID != kControlID_Invalid )
	{
		AListFrameViewFactory	listFrameViewFactory(GetObjectID(),inListViewFrameControlID,inListViewControlID,false,kControlID_Invalid,kControlID_Invalid);
		NVM_CreateView(inListViewFrameControlID,false,listFrameViewFactory);
		mListFrameViewControlIDArray.Add(inListViewFrameControlID);
	}
	
	//ListView生成
	AListViewFactory	listViewFactory(GetObjectID(),inListViewControlID);
	NVM_CreateView(inListViewControlID,false,listViewFactory);
	mListViewControlIDArray.Add(inListViewControlID);
	
	NVI_GetListView(inListViewControlID).NVI_SetScrollBarControlID(kControlID_Invalid,inScrollBarControlID);
}
*/

//#205
/**
ListView生成（フレーム、スクロールバーがリソース上に定義されている場合用）

@param inListViewControlID ListViewのControlID（Nibやrsrcのリソースファイルに定義済みのControlID）
*/
void	AWindow::NVI_CreateListViewFromResource( const AControlID inListViewControlID )
{
	AListViewFactory	listViewFactory(GetObjectID(),inListViewControlID);
	NVM_CreateView(inListViewControlID,false,listViewFactory);
	//#205 mListViewControlIDArray.Add(inListViewControlID);
}

//ListView生成（フレーム、スクロールバーはリソース上に定義されておらず、ListViewのみが定義されている場合用）
//inListViewControlID: ListViewのControlID
//inListViewFrameControlID: フレームのControlID（フレーム無しの場合はkControlID_Invalid)
//inScrollBarControlID: VScrollBarのControlID（VScrollBar無しの場合はkControlID_Invalid)
//win 080618 Frameにヘッダー機能とスクロールバー機能を追加。ListViewの外側にフレーム、ヘッダー、スクロールバーを構成する
/*#205
void	AWindow::NVI_CreateListView( const AControlID inListViewControlID, const ABool inFrame, const ABool inHeader, 
		const ABool inVScrollBar, const ABool inHScrollBar )
{
	AControlID	frameControlID = kControlID_Invalid;
	AControlID	vScrollbarControlID = kControlID_Invalid;
	AControlID	hScrollbarControlID = kControlID_Invalid;

#if USE_IMP_TABLEVIEW
#else
	if( inFrame == true )
	{
		AIndex	tabIndex = GetImp().GetControlEmbeddedTabIndex(inListViewControlID);
		frameControlID = NVM_AssignDynamicControlID();
		AWindowPoint	pt = {0,0};
		if( inVScrollBar == true )
		{
			vScrollbarControlID = NVM_AssignDynamicControlID();
			NVI_CreateScrollBar(vScrollbarControlID,pt,10,20,tabIndex);
		}
		if( inHScrollBar == true )
		{
			hScrollbarControlID = NVM_AssignDynamicControlID();
			NVI_CreateScrollBar(hScrollbarControlID,pt,20,10,tabIndex);
		}
		AListFrameViewFactory	listFrameViewFactory(this,*this,frameControlID,inListViewControlID,inHeader,vScrollbarControlID,hScrollbarControlID);
		NVM_CreateView(frameControlID,pt,10,10,false,listFrameViewFactory,tabIndex,false);
		mListFrameViewControlIDArray.Add(frameControlID);
		if( inVScrollBar == true )
		{
			NVI_EmbedControl(frameControlID,vScrollbarControlID);
		}
		if( inHScrollBar == true )
		{
			NVI_EmbedControl(frameControlID,hScrollbarControlID);
		}
	}
#endif
	
	//ListView生成
	AListViewFactory	listViewFactory(GetObjectID(),inListViewControlID);
	NVM_CreateView(inListViewControlID,false,listViewFactory);
	mListViewControlIDArray.Add(inListViewControlID);
	
	//フレームの中にListViewを入れる
	NVI_EmbedControl(frameControlID,inListViewControlID);
	
	//ListViewにフレームとスクロールバーのControlIDを設定
	NVI_GetListView(inListViewControlID).NVI_SetScrollBarControlID(hScrollbarControlID,vScrollbarControlID);
	NVI_GetListView(inListViewControlID).SPI_SetFrameControlID(frameControlID);
}
*/

AView_List&	AWindow::NVI_GetListView( const AControlID inListViewControlID )
{
	//#205 if( mListViewControlIDArray.Find(inListViewControlID) == kControlID_Invalid )   _ACThrow("no control ID",this);
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,inListViewControlID);//#199
	//#199 return dynamic_cast<AView_List&>(NVI_GetViewByControlID(inListViewControlID));
}
const AView_List&	AWindow::NVI_GetListViewConst( const AControlID inListViewControlID ) const
{
	//#205 if( mListViewControlIDArray.Find(inListViewControlID) == kControlID_Invalid )   _ACThrow("no control ID",this);
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,inListViewControlID);
}

/*#205
ListフレームはListViewからのみViewID経由でアクセスする
//Listフレームを取得
//win 080618
AView_ListFrame&	AWindow::NVI_GetListFrameView( const AControlID inListFrameViewControlID )
{
	if( mListFrameViewControlIDArray.Find(inListFrameViewControlID) == kControlID_Invalid )   _ACThrow("no control ID",this);
	return dynamic_cast<AView_ListFrame&>(NVI_GetViewByControlID(inListFrameViewControlID));
}
*/

#pragma mark ===========================

//win 080618

#pragma mark ---EditBoxView

/**
EditBox生成（リソース上に定義されている場合用）
*/
AViewID	AWindow::NVI_CreateEditBoxView( const AControlID inEditViewControlID, 
		const ABool inHasVScrollbar, const ABool inHasHScrollBar , const ABool inHasFrame, //#182
		const AEditBoxType inEditBoxType )//#725
{
	AEditBoxViewFactory	editViewFactory(GetObjectID(),inEditViewControlID);//#182
	AViewID	viewID = NVM_CreateView(inEditViewControlID,/*#688 false*/true,editViewFactory);
	NVI_GetEditBoxView(inEditViewControlID).SPI_SetEditBoxType(inEditBoxType);
	NVI_RegisterToFocusGroup(inEditViewControlID);
	if( inHasFrame == true )
	{
		//edit boxがfilter boxの場合は、左余白を多めにする（虫眼鏡アイコン表示用）
		ANumber	frameLeftMargin = 4;
		if( inEditBoxType == kEditBoxType_FilterBox )
		{
			frameLeftMargin += 18;
		}
		//frame view取得
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateFrameView(frameLeftMargin);
		//edit boxがfilter boxの場合は、フレームをfilter boxタイプに設定
		if( inEditBoxType == kEditBoxType_FilterBox )
		{
			NVI_GetViewByControlID(inEditViewControlID).NVI_GetFrameView().SPI_SetFrameViewType(kFrameViewType_FilterBox);
		}
		else
		{
			NVI_GetViewByControlID(inEditViewControlID).NVI_GetFrameView().SPI_SetFrameViewType(kFrameViewType_EditBox);
		}
	}
	if( inHasVScrollbar == true )
	{
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateVScrollBar(15,inHasHScrollBar);
	}
	if( inHasHScrollBar == true )
	{
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateHScrollBar(15,inHasVScrollbar);
	}
	return viewID;
}

//EditBox生成（リソース上に定義されていない場合用）
AViewID	AWindow::NVI_CreateEditBoxView( const AControlID inEditViewControlID, 
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight, 
		const AControlID inParentControlID,//#688
		const AIndex inTabControlIndex, const ABool inFront,
		const ABool inHasVScrollbar, const ABool inHasHScrollBar , const ABool inHasFrame, //#182
		const AEditBoxType inEditBoxType )//#725
{
	AEditBoxViewFactory	editViewFactory(/* #199 this,*this*/GetObjectID(),inEditViewControlID);
	AViewID	viewID = NVM_CreateView(inEditViewControlID,inPosition,inWidth,inHeight,inParentControlID,kControlID_Invalid,
			true,editViewFactory,inTabControlIndex,inFront);
	NVI_GetEditBoxView(inEditViewControlID).SPI_SetEditBoxType(inEditBoxType);
	NVI_RegisterToFocusGroup(inEditViewControlID);
	if( inHasFrame == true )
	{
		//edit boxがfilter boxの場合は、左余白を多めにする（虫眼鏡アイコン表示用）
		ANumber	frameLeftMargin = 4, frameRightMargin = 4;//#1316
		if( inEditBoxType == kEditBoxType_FilterBox )
		{
			frameLeftMargin += 22;//#1316 18;
			frameRightMargin += 8;//#1316
		}
		//frame view取得
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateFrameView(frameLeftMargin,frameRightMargin);//#1316
		//フレームの表示タイプを設定
		AFrameViewType	frameViewType = kFrameViewType_EditBox;
		switch(inEditBoxType)
		{
		  case kEditBoxType_ToolTip://#1316 kEditBoxType_ThinFrame:
			{
				frameViewType = kFrameViewType_ToolTip;//#1316 kFrameViewType_Normal;
				break;
			}
		  case kEditBoxType_FilterBox:
			{
				frameViewType = kFrameViewType_FilterBox;
				break;
			}
		  case kEditBoxType_NoFrameDraw:
			{
				frameViewType = kFrameViewType_NoFrameDraw;
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		NVI_GetViewByControlID(inEditViewControlID).NVI_GetFrameView().SPI_SetFrameViewType(frameViewType);
	}
	if( inHasVScrollbar == true )
	{
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateVScrollBar();
	}
	if( inHasHScrollBar == true )
	{
		NVI_GetViewByControlID(inEditViewControlID).NVI_CreateHScrollBar();
	}
	return viewID;
}

//EditBox削除
void	AWindow::NVI_DeleteEditBoxView( const AControlID inEditViewControlID )
{
	//win NVM_DeleteView(inEditViewControlID);
	NVI_DeleteControl(inEditViewControlID);//win NVI_DeleteView()はmTabDataArray_ControlIDsからの削除等を行わないのでNG
	/*#199 AIndex	index = mEditBoxViewControlIDArray.Find(inEditViewControlID);
	if( index == kIndex_Invalid )   {_ACError("",this);return;}
	mEditBoxViewControlIDArray.Delete1(index);*/
}

//EditBox取得
AView_EditBox&	AWindow::NVI_GetEditBoxView( const AControlID inEditViewControlID )
{
	//#199 if( mEditBoxViewControlIDArray.Find(inEditViewControlID) == kControlID_Invalid )   _ACThrow("no control ID",this);
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_EditBox,kViewType_EditBox,inEditViewControlID);
	/*#199
	return dynamic_cast<AView_EditBox&>(NVI_GetViewByControlID(inEditViewControlID));
	*/
}
const AView_EditBox&	AWindow::NVI_GetEditBoxViewConst( const AControlID inEditViewControlID ) const
{
	//#199 if( mEditBoxViewControlIDArray.Find(inEditViewControlID) == kControlID_Invalid )   _ACThrow("no control ID",this);
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_EditBox,kViewType_EditBox,inEditViewControlID);
	/*#199
	return dynamic_cast<const AView_EditBox&>(NVI_GetViewConstByControlID(inEditViewControlID));
	*/
}

#pragma mark ===========================

#pragma mark ---ButtonView
//#232

/**
Button生成
*/
AViewID	AWindow::NVI_CreateButtonView( const AControlID inControlID, 
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight,
		const AControlID inParentControlID )//#688
{
	AViewDefaultFactory<AView_Button>	buttonFactory(GetObjectID(),inControlID);
	return NVM_CreateView(inControlID,inPosition,inWidth,inHeight,inParentControlID,kControlID_Invalid,false,buttonFactory);
}

//#427
/**
Button生成(リソース上に定義されている場合用)
*/
AViewID	AWindow::NVI_CreateButtonView( const AControlID inControlID )
{
	AViewDefaultFactory<AView_Button>	buttonFactory(GetObjectID(),inControlID);
	return NVM_CreateView(inControlID,false,buttonFactory);
}


/**
Button取得
*/
AView_Button&	AWindow::NVI_GetButtonViewByControlID( const AControlID inButtonControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Button,kViewType_Button,inButtonControlID);
}

/**
最後にクリックしたButtonViewのControlIDを設定する
*/
void	AWindow::NVI_SetLastClickedButtonControlID( const AControlID inLastClickedButtonControlID )
{
	mLastClickedButtonControlID = inLastClickedButtonControlID;
}

/**
最後にクリックしたButtonViewのControlIDを取得する
*/
AControlID	AWindow::NVI_GetLastClickedButtonControlID() const
{
	return mLastClickedButtonControlID;
}

#pragma mark ===========================

#pragma mark ---PlainView
//#634

/**
PlainView生成
*/
AViewID	AWindow::NVI_CreatePlainView( const AControlID inControlID, const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight )
{
	AViewDefaultFactory<AView_Plain>	viewFactory(GetObjectID(),inControlID);
	return NVM_CreateView(inControlID,inPosition,inWidth,inHeight,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
}

/**
PlainView取得
*/
AView_Plain&	AWindow::NVI_GetPlainViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Plain,kViewType_Plain,inControlID);
}

/**
PlainView色設定
*/
void	AWindow::NVI_SetPlainViewColor( const AControlID inControlID, const AColor inColor, const AColor inColorDeactive, 
									   const ABool inLeftBottomRounded, const ABool inRightBottomRounded,
									   const ABool inForLeftSideBarSeparator, const ABool inForRightSideBarSeparator, const AColor inSeparatorLineColor )
{
	NVI_GetPlainViewByControlID(inControlID).SPI_SetColor(inColor,inColorDeactive,inLeftBottomRounded,inRightBottomRounded,inForLeftSideBarSeparator,inForRightSideBarSeparator,inSeparatorLineColor);
}

#pragma mark ===========================

#pragma mark ---Separator
//#291

/**
VSpeparator OverlayWindow
*/
AWindow_VSeparatorOverlay&	AWindow::NVI_GetVSeparatorWindow( const AWindowID inSeparatorOverlayWindow ) 
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_VSeparatorOverlay,kWindowType_VSeparatorOverlay,inSeparatorOverlayWindow);
}

/**
HSpeparator OverlayWindow
*/
AWindow_HSeparatorOverlay&	AWindow::NVI_GetHSeparatorWindow( const AWindowID inSeparatorOverlayWindow ) 
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_HSeparatorOverlay,kWindowType_HSeparatorOverlay,inSeparatorOverlayWindow);
}

/**
VSeparator生成
*/
void	AWindow::NVI_CreateVSeparatorView( const AControlID inControlID, const ASeparatorLinePosition inSeparatorLinePosition,
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight )
{
	AViewDefaultFactory<AView_VSeparator>	separatorFactory(GetObjectID(),inControlID);
	NVM_CreateView(inControlID,inPosition,inWidth,inHeight,kControlID_Invalid,kControlID_Invalid,false,separatorFactory);
	NVI_GetVSeparatorViewByControlID(inControlID).SPI_SetLinePosition(inSeparatorLinePosition);
}

/**
セパレータ移動したときにコールされる
*/
ANumber	AWindow::NVI_SeparatorMoved( const AWindowID inTargetWindowID, const AControlID inID, 
		const ANumber inDelta, const ABool inCompleted )//#409
{
	//派生クラスでの処理
	return AApplication::GetApplication().NVI_GetWindowByID(inTargetWindowID).
			NVIDO_SeparatorMoved(GetObjectID(),inID,inDelta,inCompleted);//#409
}

/**
セパレータダブルクリック時にコールされる
*/
void	AWindow::NVI_SeparatorDoubleClicked( const AWindowID inTargetWindowID, const AControlID inID )
{
	//派生クラスでの処理
	AApplication::GetApplication().NVI_GetWindowByID(inTargetWindowID).NVIDO_SeparatorDoubleClicked(GetObjectID(),inID);
}

/**
VSeparator取得
*/
AView_VSeparator&	AWindow::NVI_GetVSeparatorViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_VSeparator,kViewType_VSeparator,inControlID);
}

/**
HSeparator生成
*/
void	AWindow::NVI_CreateHSeparatorView( const AControlID inControlID, const ASeparatorLinePosition inSeparatorLinePosition,
		const AWindowPoint& inPosition, const ANumber inWidth, const ANumber inHeight )
{
	AViewDefaultFactory<AView_HSeparator>	separatorFactory(GetObjectID(),inControlID);
	NVM_CreateView(inControlID,inPosition,inWidth,inHeight,kControlID_Invalid,kControlID_Invalid,false,separatorFactory);
	NVI_GetHSeparatorViewByControlID(inControlID).SPI_SetLinePosition(inSeparatorLinePosition);
}

/**
HSeparator取得
*/
AView_HSeparator&	AWindow::NVI_GetHSeparatorViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_HSeparator,kViewType_HSeparator,inControlID);
}

#pragma mark ===========================

#pragma mark ---ButtonOverlayWindow
//#291

/**
AWindow_ButtonOverlay
*/
AWindow_ButtonOverlay&	AWindow::NVI_GetButtonWindow( const AWindowID inButtonOverlayWindow )
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ButtonOverlay,kWindowType_ButtonOverlay,inButtonOverlayWindow);
}

#pragma mark ===========================

#pragma mark ---modal処理
//#846

//モーダル中かどうかのフラグ #946
ABool	AWindow::sInModal = false;

/**
モーダルセッション開始
*/
void	AWindow::NVI_StartModalSession()
{
	GetImp().StartModalSession();
}

/**
モーダルセッション継続チェック
*/
ABool	AWindow::NVI_CheckContinueingModalSession()
{
	return GetImp().CheckContinueingModalSession();
}

/**
モーダルセッション終了
*/
void	AWindow::NVI_EndModalSession()
{
	GetImp().EndModalSession();
}

/**
モーダルセッション中止
*/
void	AWindow::NVI_StopModalSession()
{
	GetImp().StopModalSession();
}

/**
モーダルウインドウループ開始（ウインドウ内のイベント処理でStopModalSessionがコールされるまで終了しない）
*/
void	AWindow::NVI_RunModalWindow()
{
	try
	{
		//モーダル中フラグON #946
		sInModal = true;
		//モーダル実行（modal終了するまでブロック）
		GetImp().RunModalWindow();
		//モーダル中フラグOFF #946
		sInModal = false;
	}
	catch(...)
	{
		_ACError("",this);
	}
}

#pragma mark ===========================

#pragma mark ---staticデータ初期化
//#353

//#688 ABool	AWindow::sIsFocusGroup2ndPattern = false;//#353

/**
staticデータ初期化
*/
void	AWindow::STATIC_Init()
{
	//#688 sIsFocusGroup2ndPattern = CWindowImp::STATIC_CheckIsFocusGroup2ndPattern();
}

