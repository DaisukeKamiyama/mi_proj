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

AWindow_FindResult
#725

*/

#include "stdafx.h"

#include "AWindow_FindResult.h"
#include "ADocument_IndexWindow.h"
#include "AView_Index.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowSizeBox.h"

//#725
//SubWindow用タイトルバーview
const AControlID	kControlID_TitleBar = 901;

//#725
//SubWindow用セパレータview
const AControlID	kControlID_Separator = 902;

//#725
//サイズボックス
const AControlID	kControlID_SizeBox = 903;
const ANumber	kWidth_SizeBox = 15;
const ANumber	kHeight_SizeBox = 15;

//#725
//背景色描画用view
const AControlID	kControlID_Background = 905;

//スクロールバー
const ANumber		kWidth_VScroll = 11;

#pragma mark ===========================
#pragma mark [クラス]AWindow_FindResult
#pragma mark ===========================
//インデックスウインドウのクラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_FindResult::AWindow_FindResult( const ADocumentID inDocumentID )
	:AWindow(), mDocumentID(inDocumentID), mQuitting(false), mAllClosing(false)//drop , mAskingSaveChanges(false)
	,mIndexViewID(kObjectID_Invalid)//#92
{
}
//デストラクタ
AWindow_FindResult::~AWindow_FindResult()
{
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_FindResult::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 10;
}

ADocument_IndexWindow&	AWindow_FindResult::GetDocument()
{
	return GetApp().SPI_GetIndexWindowDocumentByID(mDocumentID);
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_FindResult::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//
ABool	AWindow_FindResult::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			NVI_TryClose();//#92
			result = true;
			break;
		}
		/*drop
		//Save
	  case kMenuItemID_Save:
		{
			GetDocument().SPI_Save(false);
			break;
		}
		//SaveAs
	  case kMenuItemID_SaveAs:
		{
			GetDocument().SPI_Save(true);
			break;
		}
		*/
		//#465
	  case kMenuItemID_CM_IndexView:
		{
			AIndex	rowIndex = AView_Index::GetIndexViewByViewID(mIndexViewID).SPI_GetContextMenuSelectedRowDBIndex();
			AView_Index::GetIndexViewByViewID(mIndexViewID).SPI_ExportToNewDocument(rowIndex);
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
void	AWindow_FindResult::EVTDO_UpdateMenu()
{
	/*drop
	//Save
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Save,true);
	//SaveAs
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SaveAs,true);
	*/
}

//ウインドウの閉じるボタン
void	AWindow_FindResult::EVTDO_DoCloseButton()
{
	//#92 TryClose();
	NVI_TryClose();//#92
}

//
void	AWindow_FindResult::EVTDO_WindowResizeCompleted( const ABool inResized )
{
	//Viewの位置、サイズ更新
	UpdateViewBounds();
	NVI_RefreshWindow();
}

/*drop
//AskSaveChangesウインドウの応答受信処理
void	AWindow_FindResult::EVTDO_AskSaveChangesReply( const AObjectID inDocumentID, const AAskSaveChanges inAskSaveChangesReply )
{
	switch(inAskSaveChangesReply)
	{
		//キャンセル
	  case kAskSaveChanges_Cancel:
		{
			mQuitting = false;
			mAllClosing = false;
			break;
		}
		//保存しない
	  case kAskSaveChanges_DontSave:
		{
			//#92 ExecuteClose();
			NVI_ExecuteClose();//#92
			break;
		}
		//保存
	  case kAskSaveChanges_Save:
		{
			GetDocument().SPI_Save(false);
			//#92 ExecuteClose();
			NVI_ExecuteClose();//#92
			break;
		}
	}
	mAskingSaveChanges = false;
	if( mQuitting == true )
	{
		mQuitting = false;
		mAllClosing = false;
		GetApp().NVI_Quit(true);//#433
	}
	else if( mAllClosing == true )
	{
		mQuitting = false;
		mAllClosing = false;
		GetApp().NVI_CloseAllWindow(false);
	}
}

//保存ダイアログ応答受信処理
void	AWindow_FindResult::EVTDO_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText )
{
	AText	filename;
	inFile.GetFilename(filename);
	GetDocument().SPI_SetTitle(filename);
	GetDocument().NVI_SpecifyFile(inFile);
	GetDocument().SPI_Save(false);
}
*/
//メニューから「閉じる」選択、もしくは、ウインドウの閉じるボタンクリック時、CloseをTryする。（DirtyならAskSaveChangesウインドウを開く）
//#92 void	AWindow_FindResult::TryClose()
void	AWindow_FindResult::NVIDO_TryClose( const AIndex inTabIndex )
{
	//#92 NVI_ExecuteClose()内で実行済み if( mInhibitClose == true )   return;
	//AskSaveChangesウインドウ表示中なら何もせずリターン
	//drop if( mAskingSaveChanges == true )    return;
	
	//現在のドキュメントがDirtyかどうか判断
	/*drop
	if( GetDocument().NVI_IsDirty() == true )
	{
		//AskSaveChangesウインドウを開いて、保存するかどうかを尋ねる
		AText	filename;
		GetDocument().NVI_GetTitle(filename);
		NVI_ShowAskSaveChangesWindow(filename,GetDocument().GetObjectID());
		mAskingSaveChanges = true;
		return;
	}
	else
	*/
	{
		//Close実行
		//#92 ExecuteClose();
		//#1336 NVI_ExecuteClose();//#92
		//#1336 閉じずにhideする
		NVI_Hide();
	}
}

//Close実行
//#92 void	AWindow_FindResult::ExecuteClose()
void	AWindow_FindResult::NVIDO_ExecuteClose()
{
	//#92 NVI_ExecuteClose()内で実行済み if( mInhibitClose == true )   return;
	//タブ＋内包ビュー削除
	NVI_DeleteTabAndView(0);
	//タブが0個になったらウインドウ削除
	if( NVI_GetTabCount() == 0 )
	{
		NVI_Close();
		GetApp().SPI_CloseSubWindow(GetObjectID());
	}
	
	//ドキュメントへウインドウ（タブ）が閉じられたことを通知する
	//#379 AView_Index::NVI_UnregisterView()によりドキュメント解放するためここは不要。 GetDocument().OWICB_DoWindowClosed(GetObjectID());
	
	//ウインドウメニュー更新
	//#922 GetApp().SPI_UpdateWindowMenu();
	
}

//ウインドウサイズ変更通知時のコールバック
void	AWindow_FindResult::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_FindResult::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ生成
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//サイドバー
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Bottom,false,false,false,false,false,false,false);
			break;
		}
		//フローティング
	  default:
		{
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,false,false,false,false);
			break;
		}
	}
	
	//背景色描画用view生成
	{
		AViewDefaultFactory<AView_SubWindowBackground>	viewFactory(GetObjectID(),kControlID_Background);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Background,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Background,true);
		NVI_SetControlBindings(kControlID_Background,true,true,true,true,false,false);
	}
	
	//SubWindow用タイトルバー生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_FindResult");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwSearchList);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//サブウインドウ用フォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
	//SizeBox生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
	
	//IndexView生成
	AControlID	indexViewControlID = kIndex_Invalid;
	{
		AWindowPoint	pt = {0,0};
		indexViewControlID = NVM_AssignDynamicControlID();//#271
		AIndexViewFactory	indexViewFactory(GetObjectID(),indexViewControlID,mDocumentID);//#92
		mIndexViewID = NVI_CreateView(indexViewControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,true,indexViewFactory);//#92
		NVI_SetControlBindings(indexViewControlID,true,true,true,true,false,false);
	}
	
	//Vスクロールバー生成
	{
		AWindowPoint	pt = {0,0};
		mVScrollBarControlID = NVM_AssignDynamicControlID();
		NVI_CreateScrollBar(mVScrollBarControlID,pt,kWidth_VScroll,kWidth_VScroll*2);
		NVI_SetControlBindings(mVScrollBarControlID,false,true,true,true,true,false);
	}
	
	//SubWindow用セパレータ生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,indexViewControlID,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//
	AView_Index::GetIndexViewByViewID(mIndexViewID).NVI_SetScrollBarControlID(kControlID_Invalid,mVScrollBarControlID);
}

void	AWindow_FindResult::NVIDO_Hide()
{
	//フローティングの場合は位置をdbに設定
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		APoint	pt = {0};
		NVI_GetWindowPosition(pt);
		GetApp().GetAppPref().SetData_Point(AAppPrefDB::kIndexWindowPosition,pt);
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowWidth,NVI_GetWindowWidth());
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowHeight,NVI_GetWindowHeight());
	}
}

//ウインドウ全体の表示更新
void	AWindow_FindResult::NVIDO_UpdateProperty()
{
	//
	AView_Index::GetIndexViewByViewID(mIndexViewID).SPI_UpdateImageSize();
	AView_Index::GetIndexViewByViewID(mIndexViewID).SPI_UpdateTextDrawProperty();
	//view bounds更新
	UpdateViewBounds();
	//
	NVI_RefreshWindow();
}

//コントロール状態（Enable/Disable等）を更新
//NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()からコールされる
//クリックやアクティベートでコールされるので、あまり重い処理をNVMDO_UpdateControlStatus()には入れないこと。
void	AWindow_FindResult::NVMDO_UpdateControlStatus()
{
}

//新規タブ生成
//引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
void	AWindow_FindResult::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//view bounds更新
	UpdateViewBounds();
	//ドキュメントIDをタブデータに設定（NVI_GetDocumentIDByTabIndex()からmDocumentIDを取得できるようにするため）#688
	NVM_SetTabDocumentID(inTabIndex,mDocumentID);
	//
	outInitialFocusControlID = AView_Index::GetIndexViewByViewID(mIndexViewID).NVI_GetControlID();
}

void	AWindow_FindResult::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//タイルバー、セパレータ高さ取得
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	
	//
	ABool	isSideBarBottom = GetApp().SPI_IsSubWindowSideBarBottom(GetObjectID());
	
	//背景色描画view調整
	ANumber	leftMargin = 1;
	ANumber	rightMargin = 1;
	ANumber	bottomMargin = 1;
	AWindowPoint	backgroundPt = {0,height_Titlebar};
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			leftMargin = 0;
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			rightMargin = 0;
			break;
		}
	  default:
		{
			leftMargin = 0;
			rightMargin = 0;
			bottomMargin = 0;
			break;
		}
	}
	if( isSideBarBottom == true )
	{
		bottomMargin = 0;
	}
	
	//タイトルバー、セパレータ表示フラグ
	ABool	showTitleBar = false;
	ABool	showSeparator = false;
	ABool	showBackground = false;
	ABool	showSizeBox = false;
	//list viewサイズ
	AWindowPoint	listViewPoint = {leftMargin,0};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;
	//スクロールバー幅
	ANumber	vscrollbarWidth = kWidth_VScroll;
	//右サイドバーの最終項目の場合、かつ、ステータスバー未表示時以外はサイズボックスの対象は自ウインドウ
	NVI_SetSizeBoxTargetWindowID(GetObjectID());
	//レイアウト計算
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//サイドバーの場合
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//タイトルバー、セパレータ表示フラグ
			showTitleBar = true;
			showSeparator = true;
			//list viewサイズ
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;// + height_Separator;
			//
			showBackground = true;
			/*
			if( isSideBarBottom == true )
			{
				showSeparator = false;
				//右サイドバーの最終項目の場合、かつ、ステータスバー未表示時はサイズボックスを表示する
				if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_RightSideBar &&
							GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
				{
					showSizeBox = true;
					//サイズボックス対象をテキストウインドウにする
					NVI_SetSizeBoxTargetWindowID(NVI_GetOverlayParentWindowID());
				}
			}
			*/
			break;
		}
		//フローティングの場合
	  default:
		{
			//タイトルバー、セパレータ表示フラグ
			showTitleBar = true;
			//list viewサイズ
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;
			//
			showBackground = true;
			showSizeBox = true;
			break;
		}
	}
	
	//Vスクロールバー設定
	ANumber	vscrollbarHeight = listViewHeight;
	if( showSizeBox == true )
	{
		vscrollbarHeight -= kHeight_SizeBox;
	}
	
	//タイトルバー配置
	if( showTitleBar == true )
	{
		//タイトルバー配置
		AWindowPoint	pt = {-1+leftMargin,0};
		NVI_SetControlPosition(kControlID_TitleBar,pt);
		NVI_SetControlSize(kControlID_TitleBar,windowBounds.right - windowBounds.left +2-leftMargin-rightMargin,height_Titlebar);
		//タイトルバー表示
		NVI_SetShowControl(kControlID_TitleBar,true);
	}
	else
	{
		//タイトルバー非表示
		NVI_SetShowControl(kControlID_TitleBar,false);
	}
	
	//背景描画view配置
	if( showBackground == true )
	{
		//背景描画view配置
		NVI_SetControlPosition(kControlID_Background,backgroundPt);
		NVI_SetControlSize(kControlID_Background,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top - backgroundPt.y);
		//背景描画view表示
		NVI_SetShowControl(kControlID_Background,true);
	}
	else
	{
		//タイトルバー非表示
		NVI_SetShowControl(kControlID_Background,false);
	}
	
	//セパレータ配置
	if( showSeparator == true )
	{
		//セパレータ配置
		AWindowPoint	pt = {0,windowBounds.bottom-windowBounds.top-height_Separator};
		NVI_SetControlPosition(kControlID_Separator,pt);
		NVI_SetControlSize(kControlID_Separator,windowBounds.right-windowBounds.left,height_Separator);
		//セパレータ表示
		NVI_SetShowControl(kControlID_Separator,true);
	}
	else
	{
		//セパレータ非表示
		NVI_SetShowControl(kControlID_Separator,false);
	}
	
	//ListView配置・表示
	{
		AView_Index::GetIndexViewByViewID(mIndexViewID).NVI_SetPosition(listViewPoint);
		AView_Index::GetIndexViewByViewID(mIndexViewID).NVI_SetSize(listViewWidth - vscrollbarWidth,listViewHeight);
		AView_Index::GetIndexViewByViewID(mIndexViewID).SPI_UpdateImageSize();
	}
	
	//Vスクロールバー配置
	if( vscrollbarWidth > 0 /*&& NVI_IsControlEnable(mVScrollBarControlID) == true*/)
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(mVScrollBarControlID,pt);
		NVI_SetControlSize(mVScrollBarControlID,vscrollbarWidth,vscrollbarHeight);
		NVI_SetShowControl(mVScrollBarControlID,true);
	}
	else
	{
		NVI_SetShowControl(mVScrollBarControlID,false);
	}
	
	//サイズボックス配置
	if( showSizeBox == true )
	{
		AWindowPoint	pt = {windowBounds.right-windowBounds.left-kWidth_SizeBox - rightMargin,
		windowBounds.bottom-windowBounds.top-kHeight_SizeBox - bottomMargin};
		NVI_SetControlPosition(kControlID_SizeBox,pt);
		NVI_SetControlSize(kControlID_SizeBox,kWidth_SizeBox,kHeight_SizeBox);
		NVI_SetShowControl(kControlID_SizeBox,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_SizeBox,false);
	}
}

#pragma mark ===========================

#pragma mark ---

void	AWindow_FindResult::SPI_ShowSaveWindow()
{
/*drop
	AText	filename;
	GetDocument().NVI_GetTitle(filename);
	AFileAttribute	attr;
	//win attr.creator = 'MMKE';
	//win attr.type = 'INDX';
	GetApp().SPI_GetIndexFileAttribute(attr);
	AFileAcc	defaultfolder;
	if( GetDocument().NVI_IsFileSpecified() == true )
	{
		AFileAcc	file;
		GetDocument().NVI_GetFile(file);
		defaultfolder.SpecifyParent(file);
	}
	else
	{
		defaultfolder.SpecifyParent(GetApp().SPI_GetLastOpenedTextFile());
	}
	AText	filter;//win 080709
	GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
	NVI_ShowSaveWindow(filename,GetDocument().GetObjectID(),attr,filter,GetEmptyText(),defaultfolder);
*/
}
