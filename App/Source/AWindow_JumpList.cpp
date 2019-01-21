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

AWindow_JumpList

#291 タブ毎にListView,vScrollBar作成するように変更

*/

#include "stdafx.h"

#include "AWindow_JumpList.h"
#include "AApp.h"
#include "AView_Text.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowSizeBox.h"

/*#725
#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kButtonFontSize = 9.0;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AFloatNumber	kButtonFontSize = 8.0;
#endif
*/

//#798
//フィルタ
const AControlID	kControlID_Filter				= 201;
const ANumber		kHeight_FilterMargin			= 8;

//#725
//SubWindow用タイトルバーview
const AControlID	kControlID_TitleBar = 901;

//#725
//サイズボックス
const AControlID	kControlID_SizeBox = 903;
const ANumber	kWidth_SizeBox = 15;
const ANumber	kHeight_SizeBox = 15;

//#725
//背景色描画用view
const AControlID	kControlID_Background = 905;

//#725
//SubWindow用セパレータview
//const AControlID	kControlID_Separator = 902;

#pragma mark ===========================
#pragma mark [クラス]AWindow_JumpList
#pragma mark ===========================
//ジャンプリストウインドウのクラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_JumpList::AWindow_JumpList( const AWindowID inTextWindowID )
:AWindow(), mTextWindowID(inTextWindowID)
{
	//#725 タイマー対象にする
	NVI_AddToTimerActionWindowArray();
}
//デストラクタ
AWindow_JumpList::~AWindow_JumpList()
{
}

//TextWindow取得
AWindow_Text&	AWindow_JumpList::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_JumpList::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 20 + kHeight_FilterMargin + 10;
}

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_JumpList::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
メニュー選択時処理
*/
ABool	AWindow_JumpList::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			//フローティングの場合、テキストウインドウでshow/hideする
			if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
			{
				GetTextWindow().SPI_ShowHideJumpListWindow(false);
			}
			//フローティング以外の場合は、処理無し。（クローズボタンなし）
			result = true;
			break;
		}
	  case kMenuItemID_JumpList_CollapseAllForLevel:
		{
			//コンテキストメニュークリックしたリスト項目のindexを取得
			AIndex	contextMenuDBIndex = GetListView(NVI_GetCurrentTabIndex()).SPI_GetContextMenuSelectedRowDBIndex();
			if( contextMenuDBIndex != kIndex_Invalid )
			{
				AIndex	displayRowIndex = GetListView(NVI_GetCurrentTabIndex()).SPI_GetDisplayRowIndexFromDBIndex(contextMenuDBIndex);
				if( displayRowIndex != kIndex_Invalid )
				{
					//同じレベルの項目を全て折りたたみ・展開
					GetListView(NVI_GetCurrentTabIndex()).SPI_ExpandCollapseAllForSameLevel(displayRowIndex);
				}
			}
			break;
		}
	  case kMenuItemID_JumpList_ExpandAll:
		{
			GetListView(NVI_GetCurrentTabIndex()).SPI_ExpandAll();
			break;
		}
		//その他のmenu item IDの場合、このクラスで処理せず、次のコマンド対象で処理する
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

/**
メニュー更新
*/
void	AWindow_JumpList::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_JumpList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	//ListViewクリック
	AIndex	listViewTabIndex = mListViewControlIDArray.Find(inID);
	if( listViewTabIndex != kIndex_Invalid )
	{
		AIndex	index = GetListView(listViewTabIndex).SPI_GetSelectedDBIndex();
		if( index != kIndex_Invalid )
		{
			GetTextWindow().SPI_DoJumpListSelected(NVI_GetDocumentIDByTabIndex(listViewTabIndex),index,inModifierKeys);
			result = true;
		}
		return result;
	}
	//ソートボタンクリック
	AIndex	sortButtonTabIndex = mSortButtonControlIDArray.Find(inID);
	if( sortButtonTabIndex != kIndex_Invalid )
	{
		GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).
				SPI_SetJumpListSortMode(mListViewIDArray.Get(sortButtonTabIndex),
				NVI_GetButtonViewByControlID(mSortButtonControlIDArray.Get(sortButtonTabIndex)).SPI_GetToggleOn());
		result = true;
		return result;
	}
	return result;
}

//TableViewのコラム幅の変更やSort順の変更などの状態変更時のコールバック
void	AWindow_JumpList::EVTDO_TableViewStateChanged( const AControlID inID )
{
	if( NVI_IsWindowCreated() == false )   return;
}


//ウインドウサイズ変更通知時のコールバック
void	AWindow_JumpList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//ViewBounds更新
	UpdateViewBounds();
	
	//ウインドウ相対位置・サイズをDBに保存
	SaveWindowPosition();
}


//ウインドウの閉じるボタン
void	AWindow_JumpList::EVTDO_DoCloseButton()
{
	//フローティングの場合、テキストウインドウでshow/hideする
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		GetTextWindow().SPI_ShowHideJumpListWindow(false);
	}
	//フローティング以外の場合は、処理無し。（クローズボタンなし）
}

//#798
/**
コントロールでtext入力時処理
*/
void	AWindow_JumpList::EVTDO_TextInputted( const AControlID inID )
{
	//フィルタで入力時
	if( inID == kControlID_Filter )
	{
		//フィルタテキスト取得
		AText	filter;
		NVI_GetControlText(kControlID_Filter,filter);
		filter.ChangeCaseLowerFast();
		
		//list viewにフィルタ設定
		GetListView(NVI_GetCurrentTabIndex()).SPI_SetFilterText(0,filter);
		
		//選択を更新
		if( filter.GetItemCount() == 0 )
		{
			GetTextWindow().SPI_UpdateJumpListSelection(true);
		}
	}
}

//#798
/**
コントロールでリターンキー押下時処理
*/
ABool	AWindow_JumpList::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )
{
	//フィルタで入力時
	if( inControlID == kControlID_Filter )
	{
		//list viewでのクリック時と同じ処理を実行
		EVTDO_Clicked(mListViewControlIDArray.Get(NVI_GetCurrentTabIndex()),kModifierKeysMask_None);
		
		//フィルタテキストを空にする
		NVI_SetControlText(kControlID_Filter,GetEmptyText());
		
		//list viewにフィルタ設定
		GetListView(NVI_GetCurrentTabIndex()).SPI_SetFilterText(0,GetEmptyText());
		
		//選択を更新
		GetTextWindow().SPI_UpdateJumpListSelection(true);
		
	}
	return true;
}

//#798
/**
ESCキー押下時処理
*/
ABool	AWindow_JumpList::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )
{
	//フィルタで入力時
	if( inControlID == kControlID_Filter )
	{
		//フィルタテキストを空にする
		NVI_SetControlText(kControlID_Filter,GetEmptyText());
		
		//list viewにフィルタ設定
		GetListView(NVI_GetCurrentTabIndex()).SPI_SetFilterText(0,GetEmptyText());
		
		//選択を更新
		GetTextWindow().SPI_UpdateJumpListSelection(true);
		
		//テキストウインドウへフォーカス移動
		GetTextWindow().NVI_SelectWindow();
	}
	return true;
}

//#798
/**
コントロールで上キー押下時処理
*/
ABool	AWindow_JumpList::NVIDO_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//フィルタで入力時
	if( inControlID == kControlID_Filter )
	{
		//list viewで前を選択
		GetListView(NVI_GetCurrentTabIndex()).SPI_SetSelectPreviousDisplayRow(inModifers);
	}
	return true;
}

//#798
/**
コントロールで下キー押下時処理
*/
ABool	AWindow_JumpList::NVIDO_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//フィルタで入力時
	if( inControlID == kControlID_Filter )
	{
		//list viewで次を選択
		GetListView(NVI_GetCurrentTabIndex()).SPI_SetSelectNextDisplayRow(inModifers);
	}
	return true;
}

//#798
/**
タブキー押下時処理
*/
ABool	AWindow_JumpList::NVIDO_ViewTabKeyPressed( const AControlID inControlID )
{
	//タブキーによるフォーカス移動
	return true;
}

//#892
/**
リスト折りたたみ／展開時処理
*/
void	AWindow_JumpList::NVIDO_ListViewExpandedCollapsed( const AControlID inControlID )
{
	GetTextWindow().NVI_SelectWindow();
}

/**
リストのホバー更新時処理
*/
void	AWindow_JumpList::NVIDO_ListViewHoverUpdated( const AControlID inControlID, 
													 const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex )
{
	GetTextWindow().SPI_DoListViewHoverUpdated(inHoverStartDBIndex,inHoverEndDBIndex);
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_JumpList::NVIDO_Create( const ADocumentID inDocumentID )
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
			NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_None,false,false,false,false,false,true,false);
			break;
		}
	}
	
	//ウインドウサイズの設定
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListSizeX));
		NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListSizeY));
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
		text.SetLocalizedText("SubWindowTitle_JumpList");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwList);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//サブウインドウ用フォント取得
	AText	fontName;
	AFloatNumber	fontSize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontName,fontSize);
	
	//検索ボックスを生成 #798
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_Filter,pt,10,10,kControlID_Background,kIndex_Invalid,true,false,false,true,kEditBoxType_FilterBox);
		NVI_GetEditBoxView(kControlID_Filter).NVI_SetTextFont(fontName,fontSize);
		/*#1316
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetBackgroundColor(
					kColor_White,kColor_Gray70Percent,kColor_Gray70Percent);
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetTextColor(
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"),
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"));
					*/
		NVI_GetEditBoxView(kControlID_Filter).SPI_SetEnableFocusRing(false);
		NVI_SetAutomaticSelectBySwitchFocus(kControlID_Filter,true);
		NVI_SetControlBindings(kControlID_Filter,true,true,true,false,false,true);
	}
	
	//SizeBox生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
	
	//ViewBounds設定
	UpdateViewBounds();
}

/**
新規タブ生成
@return outInitialFocusControlID 生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_JumpList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {-10000,0};//win 描画範囲に新規コントロールを生成すると、小さくても生成時点で画面全体クリアが発生するため0→-10000
	
	//サブウインドウフォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
	//ListView生成
	AControlID	listViewControlID = NVM_AssignDynamicControlID();
	AListViewFactory	listViewFactory(GetObjectID(),listViewControlID);
	AViewID	listViewViewID = NVM_CreateView(listViewControlID,pt,10,10,kControlID_Background,kControlID_Invalid,false,listViewFactory);
	mListViewControlIDArray.Insert1(inTabIndex,listViewControlID);
	NVM_RegisterViewInTab(inTabIndex,listViewControlID);
	mListViewIDArray.Insert1(inTabIndex,listViewViewID);
	/*#725
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetListView(listViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetListView(listViewControlID).SPI_SetAlwaysActiveColors(true);
	}
	*/
	NVI_GetListView(listViewControlID).SPI_SetAlwaysActiveColors(true);
	NVI_GetListView(listViewControlID).SPI_SetBackgroundColor(
				GetApp().SPI_GetSubWindowBackgroundColor(true),
				GetApp().SPI_GetSubWindowBackgroundColor(false));
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(listViewControlID,true,true,true,true,false,false);
	//#328 HelpTag
	NVI_EnableHelpTagCallback(listViewControlID);
	//#442
	NVI_GetListView(listViewControlID).SPI_SetEnableContextMenu(true,kContextMenuID_JumpList);//#442
	NVI_GetListView(listViewControlID).SPI_SetTextDrawProperty(fontname,fontsize,GetApp().SPI_GetSubWindowLetterColor());//#725
	//#725
	//NVI_EmbedControl(kControlID_PaneFrame,listViewControlID);
	
	//Vスクロールバー生成
	AControlID	vScrollBarControlID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(vScrollBarControlID,pt,kVScrollBarWidth,kVScrollBarWidth*2);
	mVScrollBarControlIDArray.Insert1(inTabIndex,vScrollBarControlID);
	NVM_RegisterViewInTab(inTabIndex,vScrollBarControlID);
	//スクロールバー関連づけ
	NVI_GetViewByControlID(listViewControlID).NVI_SetScrollBarControlID(kControlID_Invalid,vScrollBarControlID);
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(vScrollBarControlID,false,true,true,true,true,false);
	
	//SubWindow用セパレータ生成
	{
		AControlID	separatorControlID = NVM_AssignDynamicControlID();
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),separatorControlID);
		NVM_CreateView(separatorControlID,pt,10,10,listViewControlID,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(separatorControlID,true);
		NVI_SetControlBindings(separatorControlID,true,false,true,true,false,true);
		mSeparatorControlIDArray.Insert1(inTabIndex,separatorControlID);
	}
	
	//
	NVI_UpdateProperty();
	
	//ViewBounds設定
	UpdateViewBounds();
	
	//DocumentにListViewを登録
	GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_RegisterJumpListView(listViewViewID);
	
	//#661
	/*
	{
		AText	text;
		text.SetLocalizedText("HelpTag_JumpList");
		NVI_SetHelpTag(jumpListButtonControlID,text);
	}
	 */
	//
	outInitialFocusControlID = listViewControlID;
}

/**
タブ削除時にコールされる

Tab内ViewはAWindow内で削除される
*/
void	AWindow_JumpList::NVIDO_DeleteTab( const AIndex inTabIndex )
{
	//DocumentからListViewを登録削除
	GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_UnregisterJumpListView(mListViewIDArray.Get(inTabIndex));
	//データ削除
	mListViewControlIDArray.Delete1(inTabIndex);
	mListViewIDArray.Delete1(inTabIndex);
	mVScrollBarControlIDArray.Delete1(inTabIndex);
	mSeparatorControlIDArray.Delete1(inTabIndex);//#725
	/*
	mSortButtonControlIDArray.Delete1(inTabIndex);
	mJumpListButtonControlIDArray.Delete1(inTabIndex);//#446
	*/
}

//Hide()時等にウインドウのデータを保存する
void	AWindow_JumpList::NVIDO_Hide()
{
	SaveWindowPosition();
}

//#798
/**
*/
void	AWindow_JumpList::NVIDO_Show()
{
}

/**
設定データ更新時処理
*/
void	AWindow_JumpList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	
	/*#725
	//透明度設定
	if( NVM_GetOverlayMode() == false )//#291
	{
		NVI_SetWindowTransparency((static_cast<AFloatNumber>(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListAlphaPercent)))/100);
	}
	else
	{
		//Overlay時は情報ペインOverlay透明度を適用（ただし、MacOSXでは実際には何もしない。View側で塗りつぶし時に透明度適用。）
		NVI_SetWindowTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
	}
	*/
	
	//サブウインドウフォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//フィルタにフォント設定
	NVI_GetEditBoxView(kControlID_Filter).NVI_SetTextFont(fontname,fontsize);
	//
	AText	filtertext;
	filtertext.SetLocalizedText("Filter");
	//
	AText	shortcuttext;
	GetApp().NVI_GetMenuManager().GetMenuItemShortcutDisplayText(kMenuItemID_SearchInHeaderList,shortcuttext);
	//
	NVI_GetEditBoxView(kControlID_Filter).SPI_SetTextForEmptyState(filtertext,shortcuttext);
	//リストviewにフォント設定
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		GetListView(tabIndex).SPI_SetTextDrawProperty(fontname,fontsize,GetApp().SPI_GetSubWindowLetterColor());
		//ホイールスクロール率設定
		GetListView(tabIndex).SPI_SetWheelScrollPercent(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent),
					GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd));
		//フローティングの場合の背景透過率設定
		if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
		{
			GetListView(tabIndex).SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
			NVI_GetEditBoxView(kControlID_Filter).SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
		}
		else
		{
			GetListView(tabIndex).SPI_SetTransparency(1.0);
			NVI_GetEditBoxView(kControlID_Filter).SPI_SetTransparency(1.0);
		}
	}
	/*#1316
	//フィルタの色設定
	NVI_GetEditBoxView(kControlID_Filter).SPI_SetBackgroundColorForEmptyState(
																			  GetApp().SPI_GetSubWindowBackgroundColor(true),
																			  GetApp().SPI_GetSubWindowBackgroundColor(false));
																			  */
	//view bounds更新
	UpdateViewBounds();
	//ウインドウ位置更新
	SPI_UpdatePosition();
}

/*#291
void	AWindow_JumpList::SPI_SetScrollPosition( const APoint& inPoint )
{
	if( NVI_IsWindowCreated() == false )   return;
	AImagePoint	pt;
	pt.x = inPoint.x;
	pt.y = inPoint.y;
	GetListView().NVI_ScrollTo(pt);
}

void	AWindow_JumpList::SPI_GetScrollPosition( APoint& outPoint ) const
{
	if( NVI_IsWindowCreated() == false )   return;
	AImagePoint	pt;
	GetListViewConst().NVI_GetOriginOfLocalFrame(pt);
	outPoint.x = pt.x;
	outPoint.y = pt.y;
}
*/

//#798
/**
検索ボックスをフォーカス
*/
void	AWindow_JumpList::SPI_SwitchFocusToSearchBox()
{
	NVI_SelectWindow();
	NVI_SwitchFocusTo(kControlID_Filter);
	NVI_GetEditBoxView(kControlID_Filter).NVI_SetSelectAll();
}

/**
ウインドウタイトル設定
*/
void	AWindow_JumpList::SPI_SetSubWindowTitle( const AText& inTitle )
{
	GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(inTitle,kImageID_iconSwList);
}

#pragma mark ===========================

#pragma mark ---表示更新

/**
ウインドウ位置更新
*/
void	AWindow_JumpList::SPI_UpdatePosition()
{
	if( NVI_IsWindowCreated() == false )   return;
	
	if( NVM_GetOverlayMode() == true )   return;//#291
	
	//
	ARect	textDocumentBounds = {0};
	GetTextWindow().NVI_GetWindowBounds(textDocumentBounds);
	
	//
	APoint	pt = {0};
	NVI_GetWindowPosition(pt);
	if( GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListPosition) == /*AAppPrefDB::kJumpListPosition_Left*/0 )
	{
		pt.x = textDocumentBounds.left + GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListOffsetX);
	}
	else
	{
		pt.x = textDocumentBounds.right + GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListOffsetX);
	}
	pt.y = textDocumentBounds.top + GetApp().GetAppPref().GetData_Number(AAppPrefDB::kJumpListOffsetY);
	NVI_SetWindowPosition(pt);
	//#680 これがあるとテキストウインドウ移動時に見出しウインドウの位置がずれてしまう NVI_ConstrainWindowPosition(true);
}

/**
ウインドウ相対位置・サイズをDBに保存
*/
void	AWindow_JumpList::SaveWindowPosition()
{
	//位置・サイズ保存
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		//
		ARect	textDocumentBounds = {0};
		GetTextWindow().NVI_GetWindowBounds(textDocumentBounds);
		
		//
		APoint	pt = {0};
		NVI_GetWindowPosition(pt);
		if( pt.x < (textDocumentBounds.left+textDocumentBounds.right)/2 )
		{
			//テキストウインドウの左上相対位置を保存
			GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListPosition,/*AAppPrefDB::kJumpListPosition_Left*/0);
			GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListOffsetX,pt.x-textDocumentBounds.left);
		}
		else
		{
			//テキストウインドウの右上相対位置を保存
			GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListPosition,/*AAppPrefDB::kJumpListPosition_Right*/1);
			GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListOffsetX,pt.x-textDocumentBounds.right);
		}
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListOffsetY,pt.y-textDocumentBounds.top);
		//ウインドウサイズの保存
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListSizeX,NVI_GetWindowWidth());
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kJumpListSizeY,NVI_GetWindowHeight());
	}
}

/**
ViewBounds更新
*/
void	AWindow_JumpList::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//タイルバー、セパレータ高さ取得
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	ANumber	height_Filter = NVI_GetEditBoxView(kControlID_Filter).SPI_GetLineHeightWithMargin() + kHeight_FilterMargin;
	ANumber	height_DropBox = 0;
	
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
	ABool	showSizeBox = false;
	//list viewサイズ
	AWindowPoint	listViewPoint = {leftMargin,0};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;
	//スクロールバー幅
	ANumber	vscrollbarWidth = kVScrollBarWidth;
	//
	ANumber	sizeboxHeight = 0;
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
			listViewPoint.y += height_Titlebar + height_Filter;
			listViewHeight -= height_Titlebar + height_Filter;// + height_Separator;
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
			listViewPoint.y += height_Titlebar + height_Filter;
			listViewHeight -= height_Titlebar + height_Filter;
			//
			height_DropBox = kDragBoxHeight;
			//
			showSizeBox = true;
			sizeboxHeight = kHeight_SizeBox;
			break;
		}
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
	{
		//背景描画view配置
		NVI_SetControlPosition(kControlID_Background,backgroundPt);
		NVI_SetControlSize(kControlID_Background,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top - backgroundPt.y);
		//背景描画view表示
		NVI_SetShowControl(kControlID_Background,true);
	}
	
	//フィルタ配置
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar};
		NVI_SetControlPosition(kControlID_Filter,pt);
		NVI_SetControlSize(kControlID_Filter,windowBounds.right-windowBounds.left- leftMargin - rightMargin,height_Filter);
	}
	
	
	//タブごと
	for( AIndex tabIndex = 0; tabIndex < mListViewControlIDArray.GetItemCount(); tabIndex++ )
	{
		AControlID	vscrollControlID = mVScrollBarControlIDArray.Get(tabIndex);
		//ListView bounds設定
		{
			AControlID	controlID = mListViewControlIDArray.Get(tabIndex);
			NVI_SetControlPosition(controlID,listViewPoint);
			//Vスクロールバー表示有無に合わせてlist viewのサイズを変える（選択描画が右端まで表示されないとかっこわるいため）
			ANumber	w = listViewWidth;
			if( vscrollbarWidth > 0 && NVI_IsControlEnable(vscrollControlID) == true )
			{
				w -= vscrollbarWidth;
			}
			NVI_SetControlSize(controlID,w,listViewHeight);
		}
		//Vスクロールバー配置
		if( vscrollbarWidth > 0 && NVI_IsControlEnable(vscrollControlID) == true && tabIndex == NVI_GetCurrentTabIndex() )
		{
			AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
			NVI_SetControlPosition(vscrollControlID,pt);
			NVI_SetControlSize(vscrollControlID,vscrollbarWidth,listViewHeight - sizeboxHeight);
			NVI_SetShowControl(vscrollControlID,true);
		}
		else
		{
			NVI_SetShowControl(vscrollControlID,false);
		}
		//セパレータ配置
		AControlID	separatorID = mSeparatorControlIDArray.Get(tabIndex);
		if( showSeparator == true && tabIndex == NVI_GetCurrentTabIndex() )
		{
			//セパレータ配置
			AWindowPoint	pt = {0,windowBounds.bottom-windowBounds.top-height_Separator};
			NVI_SetControlPosition(separatorID,pt);
			NVI_SetControlSize(separatorID,windowBounds.right-windowBounds.left,height_Separator);
			//セパレータ表示
			NVI_SetShowControl(separatorID,true);
		}
		else
		{
			//セパレータ非表示
			NVI_SetShowControl(separatorID,false);
		}
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

#if 0
//#725
//#291
/**
オーバーレイモード時の透明度設定
*/
void	AWindow_JumpList::SPI_UpdateOverlayWindowAlpha()
{
	if( NVI_GetOverlayMode() == true )
	{
		for( AIndex i = 0; i < mListViewControlIDArray.GetItemCount(); i++ )
		{
			AControlID listViewControlID = mListViewControlIDArray.Get(i);
			NVI_GetListView(listViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
			AControlID	sortButtonControlID = mSortButtonControlIDArray.Get(i);
			NVI_GetButtonViewByControlID(sortButtonControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
			//#446
			AControlID	jumpListButtonControlID = mJumpListButtonControlIDArray.Get(i);
			NVI_GetButtonViewByControlID(jumpListButtonControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		}
	}
}

//#634
/**
サイドバーモードの透過率・背景色に切り替え
*/
void	AWindow_JumpList::SPI_SetSideBarMode( const ABool inSideBarMode, const AColor inSideBarColor )
{
	if( NVI_GetOverlayMode() == true )
	{
		for( AIndex i = 0; i < mListViewControlIDArray.GetItemCount(); i++ )
		{
			AControlID listViewControlID = mListViewControlIDArray.Get(i);
			AControlID	sortButtonControlID = mSortButtonControlIDArray.Get(i);
			AControlID	jumpListButtonControlID = mJumpListButtonControlIDArray.Get(i);
			if( inSideBarMode == true )
			{
				//できる限り文字をきれいに表示させるために不透過率はできるだけ高めに設定する。（背景のグラデーションがうっすら透過する程度）
				NVI_GetListView(listViewControlID).SPI_SetBackgroundColor(inSideBarColor,kColor_Gray97Percent);
				NVI_GetListView(listViewControlID).SPI_SetTransparency(0.8);
				NVI_GetButtonViewByControlID(sortButtonControlID).SPI_SetTransparency(0.8);
				NVI_GetButtonViewByControlID(jumpListButtonControlID).SPI_SetTransparency(0.8);
			}
			else
			{
				NVI_GetListView(listViewControlID).SPI_SetBackgroundColor(kColor_White,kColor_White);
				NVI_GetListView(listViewControlID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
				NVI_GetButtonViewByControlID(sortButtonControlID).SPI_SetTransparency(1);
				NVI_GetButtonViewByControlID(jumpListButtonControlID).SPI_SetTransparency(1);
			}
		}
	}
}
#endif

//#695
/**
ソートボタンの表示状態を更新する
*/
void	AWindow_JumpList::SPI_UpdateSortButtonStatus( const ADocumentID inDocumentID, const ABool inSortOn )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	NVI_GetButtonViewByControlID(mSortButtonControlIDArray.Get(tabIndex)).SPI_SetToogleOn(inSortOn);
}

#pragma mark ===========================

#pragma mark --- ListView取得

/**
ListView取得
@param inTabIndex TabIndex
*/
AView_List&	AWindow_JumpList::GetListView( const AIndex inTabIndex )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,mListViewControlIDArray.Get(inTabIndex));
}
/**
ListView取得
@param inTabIndex TabIndex
*/
const AView_List&	AWindow_JumpList::GetListViewConst( const AIndex inTabIndex ) const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_List,kViewType_List,mListViewControlIDArray.Get(inTabIndex));
}

