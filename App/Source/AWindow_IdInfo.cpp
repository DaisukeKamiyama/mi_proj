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

AWindow_IdInfo

*/

#include "stdafx.h"

#include "AWindow_IdInfo.h"
#include "AView_IdInfo.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowSizeBox.h"

//const AControlID	kDisplayCommentButtonID = 100;
//#725 const AControlID	kFixDisplayButtonID = 101;
//#725 const AControlID	kHistoryButtonID = 102;//#238

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

//#725
//検索ボックス
const AControlID	kControlID_SearchBox = 101;
const ANumber		kHeight_SearchBoxMargin			= 8;

//#725
//ポップアップ設定ボタン
//const AControlID	kControlID_PopupSetupButton = 102;
//const ANumber		kWidth_PopupSetupButton = 80;
//ポップアップクローズボタン
const AControlID	kControlID_PopupCloseButton = 103;
const ANumber		kWidth_PopupCloseButton = 80;
//ポップアップクローズボタン高さ
const ANumber		kHeight_PopupButtons = 20;

/*#725
//const ANumber	kDisplayCommentButtonWidth = 80;
const ANumber	kDisplayCommentButtonHeight = 19;
//#238
const ANumber	kFixDisplayButtonWidth = 80;

#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kButtonFontSize = 9.0;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AFloatNumber	kButtonFontSize = 8.0;
#endif
*/

#pragma mark ===========================
#pragma mark [クラス]AWindow_IdInfo
#pragma mark ===========================
//情報ウインドウのクラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_IdInfo::AWindow_IdInfo():AWindow(/*#175NULL*/)//#238, mFixDisplay(false)
,mInfoViewControlID(kControlID_Invalid)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kIdInfoWindowPosition);
	//#138 デフォルト範囲(10000～)を使う限り設定不要 NVM_SetDynamicControlIDRange(kDynamicControlIDStart,kDynamicControlIDEnd);
	//#725 タイマー対象ウインドウにする
	NVI_AddToTimerActionWindowArray();
}
//デストラクタ
AWindow_IdInfo::~AWindow_IdInfo()
{
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_IdInfo::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 20 + kHeight_SearchBoxMargin + 10;
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

//TextView取得
AView_IdInfo&	AWindow_IdInfo::GetIdInfoView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_IdInfo,kViewType_IdInfo,mInfoViewControlID);
	/*#199
	if( NVI_GetViewByControlID(mInfoViewControlID).NVI_GetViewType() != kViewType_IdInfo )   _ACThrow("",this);
	return dynamic_cast<AView_IdInfo&>(NVI_GetViewByControlID(mInfoViewControlID));
	*/
}
const AView_IdInfo&	AWindow_IdInfo::GetIdInfoViewConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_IdInfo,kViewType_IdInfo,mInfoViewControlID);
	/*#199
	if( NVI_GetViewConstByControlID(mInfoViewControlID).NVI_GetViewType() != kViewType_IdInfo )   _ACThrow("",this);
	return dynamic_cast<const AView_IdInfo&>(NVI_GetViewConstByControlID(mInfoViewControlID));
	*/
}

//#238
/**
IdInfoView取得
*/
AView_IdInfo&	AWindow_IdInfo::SPI_GetIdInfoView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_IdInfo,kViewType_IdInfo,mInfoViewControlID);
}

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_IdInfo::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
メニュー選択時処理
*/
ABool	AWindow_IdInfo::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			//フローティングウインドウの場合のみウインドウクローズする
			if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
			{
				GetApp().SPI_CloseSubWindow(GetObjectID());
			}
			result = true;
			break;
		}
		//サブウインドウを折りたたむ #1380
	  case kMenuItemID_CollapseThisSubwindow:
		{
			AWindowID	textWindowID = NVI_GetOverlayParentWindowID();
			if( textWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(textWindowID).SPI_ExpandCollapseSubWindow(GetObjectID());
			}
			break;
		}
		//サブウインドウを閉じる #1380
	  case kMenuItemID_CloseThisSubwindow:
		{
			AWindowID	textWindowID = NVI_GetOverlayParentWindowID();
			if( textWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(textWindowID).SPI_CloseOverlaySubWindow(GetObjectID());
			}
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
void	AWindow_IdInfo::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

//コントロールのクリック時のコールバック
ABool	AWindow_IdInfo::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_PopupCloseButton:
		{
			NVI_Hide();
			break;
		}
		/*#725 表示固定ボタン廃止
	  case kFixDisplayButtonID:
		{
			//#238 mFixDisplay = NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_GetToggleOn();//#232NVI_GetControlBool(kFixDisplayButtonID);
			SPI_GetIdInfoView().SPI_SetFixDisplay(!SPI_GetIdInfoView().SPI_GetFixDisplay());//#238
			result = true;
			break;
		}
		*/
	}
	return result;
}

//ウインドウサイズ変更通知時のコールバック
void	AWindow_IdInfo::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

/*#725
//#291
void	AWindow_IdInfo::EVTDO_DoCloseButton()
{
	GetApp().SPI_ShowHideIdInfoWindow(false);
}
*/

/**
コントロールでtext入力時処理
*/
void	AWindow_IdInfo::EVTDO_TextInputted( const AControlID inID )
{
	//サーチボックスで入力時
	if( inID == kControlID_SearchBox )
	{
		//サーチボックスのテキスト取得
		AText	text;
		NVI_GetControlText(kControlID_SearchBox,text);
		
		//最前面のテキストドキュメントを取得
		ADocumentID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
		//IdInfo検索スレッドに要求設定
		GetApp().SPI_GetIdInfoFinderThread().SetRequestData(docID,
															GetObjectID(),kObjectID_Invalid,kObjectID_Invalid,
															text,GetEmptyTextArray(),kIndex_Invalid,GetEmptyText(),
															kIndex_Invalid);
		//スレッドpause解除
		GetApp().SPI_GetIdInfoFinderThread().NVI_UnpauseIfPaused();
	}
}

//#853
/**
コントロールでリターンキー押下時処理
*/
ABool	AWindow_IdInfo::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )
{
	//サーチボックスでリターンキー押下時、項目の定義箇所を開く
	if( inControlID == kControlID_SearchBox )
	{
		GetIdInfoView().SPI_OpenItem(GetIdInfoView().SPI_GetSelectedIndex(),kModifierKeysMask_None);
	}
	return true;
}

//#798
/**
コントロールでESCキー押下時処理
*/
ABool	AWindow_IdInfo::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )
{
	//フィルタで入力時
	if( inControlID == kControlID_SearchBox )
	{
		//フィルタテキストを空にする
		NVI_SetControlText(kControlID_SearchBox,GetEmptyText());
		
		//テキストウインドウへフォーカス移動
		if( NVM_GetOverlayMode() == true )
		{
			GetApp().NVI_GetWindowByID(NVI_GetOverlayParentWindowID()).NVI_SelectWindow();
		}
	}
	return true;
}

//#853
/**
コントロールで上キー押下時処理
*/
ABool	AWindow_IdInfo::NVIDO_ViewArrowUpKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//サーチボックスで上キー押下時、前の項目を選択
	if( inControlID == kControlID_SearchBox )
	{
		GetIdInfoView().SPI_SelectPrev();
	}
	return true;
}

//#853
/**
コントロールで下キー押下時処理
*/
ABool	AWindow_IdInfo::NVIDO_ViewArrowDownKeyPressed( const AControlID inControlID, const AModifierKeys inModifers )
{
	//サーチボックスで上キー押下時、次の項目を選択
	if( inControlID == kControlID_SearchBox )
	{
		GetIdInfoView().SPI_SelectNext();
	}
	return true;
}

/**
ウインドウの閉じるボタン
*/
void	AWindow_IdInfo::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}

/**
tickタイマー処理
*/
void	AWindow_IdInfo::EVTDO_DoTickTimerAction()
{
	if( NVI_IsWindowVisible() == true )
	{
		GetIdInfoView().SPI_DoTickTimer();
	}
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_IdInfo::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ生成
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//ポップアップ
	  case kSubWindowLocationType_Popup:
		{
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
			break;
		}
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
	
	//★サイズ設定必要？
	/*
	if( NVM_GetOverlayMode() == false )//#291
	{
		//ウインドウ幅の設定
		NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIdInfoWindowWidth));
		NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIdInfoWindowHeight));
	}
	*/
	
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
		text.SetLocalizedText("SubWindowTitle_IdInfo");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwKeyWord);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//サブウインドウ用フォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
	//検索ボックスを生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_SearchBox,pt,10,10,kControlID_Invalid,kIndex_Invalid,true,false,false,true,kEditBoxType_FilterBox);
		NVI_SetControlBindings(kControlID_SearchBox,true,true,true,false,false,true);
		//フォント設定
		NVI_GetEditBoxView(kControlID_SearchBox).NVI_SetTextFont(fontname,fontsize);
		/*#1316 色はEditBox側で設定
		NVI_GetEditBoxView(kControlID_SearchBox).SPI_SetBackgroundColor(
					kColor_White,kColor_Gray70Percent,kColor_Gray70Percent);
		NVI_GetEditBoxView(kControlID_SearchBox).SPI_SetTextColor(
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"),
					AColorWrapper::GetColorByHTMLFormatColor("1574cf"));
					*/
		NVI_SetAutomaticSelectBySwitchFocus(kControlID_SearchBox,true);
		NVI_GetEditBoxView(kControlID_SearchBox).SPI_SetEnableFocusRing(false);
	}
	
	/*
	//ポップアップ設定ボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_PopupSetupButton,pt,10,10,kControlID_Invalid);
		NVI_GetButtonViewByControlID(kControlID_PopupSetupButton).SPI_SetTextProperty(fontname,fontsize,kJustification_Center,kTextStyle_Normal);
		NVI_GetButtonViewByControlID(kControlID_PopupSetupButton).SPI_SetButtonViewType(kButtonViewType_NoFrame);
		NVI_GetButtonViewByControlID(kControlID_PopupSetupButton).SPI_SetIcon(kImageID_iconPnOption,kWidth_PopupSetupButton-16,5);
		AText	text("config");
		//NVI_GetButtonViewByControlID(kControlID_PopupSetupButton).NVI_SetText(text);
	}
	*/
	//ポップアップクローズボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_PopupCloseButton,pt,10,10,kControlID_Invalid);
		NVI_GetButtonViewByControlID(kControlID_PopupCloseButton).SPI_SetTextProperty(fontname,14.0,kJustification_Left,kTextStyle_Normal);
		NVI_GetButtonViewByControlID(kControlID_PopupCloseButton).SPI_SetButtonViewType(kButtonViewType_NoFrameWithTextHover);
		AText	text;
		text.SetLocalizedText("PopupClose");
		NVI_GetButtonViewByControlID(kControlID_PopupCloseButton).NVI_SetText(text);
	}
	//SizeBox生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
}

//新規タブ生成
//引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
void	AWindow_IdInfo::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//サブウインドウ用フォント取得
	AText	fontName;
	AFloatNumber	fontSize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontName,fontSize);
	
	AWindowPoint	pt = {0,0};
	//IdInfoView生成
	mInfoViewControlID = NVM_AssignDynamicControlID();
	AViewDefaultFactory<AView_IdInfo>	infoViewFactory(GetObjectID(),mInfoViewControlID);//#199
	NVM_CreateView(mInfoViewControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,infoViewFactory);
	/*#725
	if( NVM_GetOverlayMode() == true )//#291
	{
		GetIdInfoView().SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
	}
	*/
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(mInfoViewControlID,true,true,true,true,false,false);
	//#442
	//#1380 GetIdInfoView().SPI_SetContextMenuItemID(kContextMenuID_GeneralSubWindow);//#442 #1380
	//#507
	//#1370 NVI_EnableHelpTagCallback(mInfoViewControlID,false);
	
	//SubWindow用セパレータ生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,mInfoViewControlID,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//Vスクロールバー生成
	mVScrollBarControlID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(mVScrollBarControlID,pt,kVScrollBarWidth,kVScrollBarWidth*2);
	GetIdInfoView().NVI_SetScrollBarControlID(kControlID_Invalid,mVScrollBarControlID);
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(mVScrollBarControlID,false,true,true,true,true,false);
	
	
	/*#725
	//表示固定ボタン生成
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);//#375
	NVI_CreateButtonView(kFixDisplayButtonID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);
	AText	t;
	t.SetLocalizedText("IdInfo_FixDisplayButton");
	NVI_GetButtonViewByControlID(kFixDisplayButtonID).NVI_SetText(t);
	NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetToggleMode(true);
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetAlwaysActiveColors(true);
	}
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(kFixDisplayButtonID,false,true,true,false,true,true);
	
	//#238
	NVI_CreateButtonView(kHistoryButtonID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);//#446
	NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetMenu(kContextMenuID_IdInfoHistory,kMenuItemID_IdInfoHistory);
	if( NVM_GetOverlayMode() == true )//#291
	{
		NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetAlwaysActiveColors(true);
	}
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(kHistoryButtonID,true,true,true,false,false,true);
	//#446
	t.SetLocalizedText("IdInfo");
	NVI_GetButtonViewByControlID(kHistoryButtonID).NVI_SetText(t);
	
	//#661
	{
		AText	text;
		text.SetLocalizedText("HelpTag_IdInfo");
		NVI_SetHelpTag(kHistoryButtonID,text);
		text.SetLocalizedText("HelpTag_IdInfoFixDisplay");
		NVI_SetHelpTag(kFixDisplayButtonID,text);
	}
	//#634 背景色設定
	SPI_SetSideBarMode(false,kColor_Gray97Percent);
	*/
	//
	UpdateViewBounds();
}

//Hide()時等にウインドウのデータを保存する
void	AWindow_IdInfo::NVIDO_Hide()
{
	//★検討
	/*
	if( NVM_GetOverlayMode() == false )//#291 Overlayモード時には保存しない
	{
		//ウインドウ幅の保存
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIdInfoWindowWidth,NVI_GetWindowWidth());
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIdInfoWindowHeight,NVI_GetWindowHeight());
	}
	*/
}

//
void	AWindow_IdInfo::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	/*#725
	if( NVM_GetOverlayMode() == false )//#291
	{
		NVI_SetWindowTransparency((static_cast<AFloatNumber>(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIdInfoWindowAlphaPercent)))/100);
	}
	else
	{
		//Overlay時は不透明（ただし、MacOSXでは実際には何もしない。View側で塗りつぶし時に透明度適用。）
		NVI_SetWindowTransparency(0.5);
	}
	*/
	//サブウインドウ用フォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//テキスト未入力時テキスト取得
	AText	filtertext;
	filtertext.SetLocalizedText("SearchKeyword");
	//キーワード情報検索用のショートカットキーを取得
	AText	shortcuttext;
	GetApp().NVI_GetMenuManager().GetMenuItemShortcutDisplayText(kMenuItemID_SearchInKeywordList,shortcuttext);
	//テキスト未入力時テキスト設定
	NVI_GetEditBoxView(kControlID_SearchBox).SPI_SetTextForEmptyState(filtertext,shortcuttext);
	//検索ボックスにフォント設定
	NVI_GetEditBoxView(kControlID_SearchBox).NVI_SetTextFont(fontname,fontsize);
	/*#1316 色はEditBox側で設定
	NVI_GetEditBoxView(kControlID_SearchBox).SPI_SetBackgroundColorForEmptyState(
																				 GetApp().SPI_GetSubWindowBackgroundColor(true),
																				 GetApp().SPI_GetSubWindowBackgroundColor(false));
																				 */
	//#1380
	//ウインドウタイプに応じてコンテキストメニュー設定
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
	  case kSubWindowLocationType_RightSideBar:
		{
			NVI_GetViewByControlID(mInfoViewControlID).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow_RightSideBar);
			NVI_GetViewByControlID(kControlID_TitleBar).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow_RightSideBar);
			break;
		}
	  case kSubWindowLocationType_LeftSideBar:
		{
			NVI_GetViewByControlID(mInfoViewControlID).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow_LeftSideBar);
			NVI_GetViewByControlID(kControlID_TitleBar).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow_LeftSideBar);
			break;
		}
	  default:
		{
			NVI_GetViewByControlID(mInfoViewControlID).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow);
			NVI_GetViewByControlID(kControlID_TitleBar).NVI_SetEnableContextMenu(kContextMenuID_GeneralSubWindow);
			break;
		}
	}
	//view bounds更新
	UpdateViewBounds();
	//
	GetIdInfoView().SPI_UpdateDisplayComment();
	NVI_RefreshWindow();
}

/*#238
//
void	AWindow_IdInfo::SPI_SetInfo( const AText& inIdText, const AModeIndex inModeIndex, 
		const ATextArray& inInfoTextArray, const AObjectArray<AFileAcc>& inFileArray, 
		const ATextArray& inCommentTextArray, const AArray<AIndex>& inCategoryIndexArray,
		const AArray<AScopeType>& inScopeTypeArray,
		const AArray<AIndex>& inStartIndexArray, const AArray<AIndex>& inEndIndexArray,
		const ATextArray& inParentKeywordArray )
{
	if( mFixDisplay == true )   return;
	
	GetIdInfoView().SPI_SetInfo(inIdText,inModeIndex,inInfoTextArray,inFileArray,inCommentTextArray,inCategoryIndexArray,inScopeTypeArray,
			inStartIndexArray,inEndIndexArray,inParentKeywordArray);
}
*/

//#798
/**
検索ボックスへフォーカス移動
*/
void	AWindow_IdInfo::SPI_SwitchFocusToSearchBox()
{
	NVI_SelectWindow();
	NVI_SwitchFocusTo(kControlID_SearchBox);
	NVI_GetEditBoxView(kControlID_SearchBox).NVI_SetSelectAll();
}

/*:
ウインドウの最大高さ取得（ポップアップ時のウインドウ高さ調整のため）
*/
ANumber	AWindow_IdInfo::SPI_GetMaxWindowHeight() const
{
	return GetIdInfoViewConst().NVI_GetImageHeight() + kHeight_PopupButtons;
}

/**
view bounds更新
*/
void	AWindow_IdInfo::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//タイルバー、セパレータ高さ取得
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	ANumber	height_Filter = NVI_GetEditBoxView(kControlID_SearchBox).SPI_GetLineHeightWithMargin() + kHeight_SearchBoxMargin;
	
	//サイドバーの最後の項目かどうかを取得
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
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - listViewPoint.x - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;
	//スクロールバー幅
	ANumber	vscrollbarWidth = kVScrollBarWidth;
	//右サイドバーの最終項目の場合、かつ、ステータスバー未表示時以外はサイズボックスの対象は自ウインドウ
	NVI_SetSizeBoxTargetWindowID(GetObjectID());
	//レイアウト計算
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//ポップアップの場合
	  case kSubWindowLocationType_Popup:
		{
			listViewPoint.y += kHeight_PopupButtons;
			listViewHeight -= kHeight_PopupButtons;
			vscrollbarWidth = 0;
			break;
		}
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
			listViewPoint.y += height_Titlebar + height_Filter;
			listViewHeight -= height_Titlebar + height_Filter;
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
	
	//検索ボックス配置
	if( showTitleBar == true )
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar};
		NVI_SetControlPosition(kControlID_SearchBox,pt);
		NVI_SetControlSize(kControlID_SearchBox,windowBounds.right-windowBounds.left- leftMargin - rightMargin,height_Filter);
		NVI_SetShowControl(kControlID_SearchBox,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_SearchBox,false);
	}
	
	//ListView配置・表示
	{
		NVI_SetControlPosition(mInfoViewControlID,listViewPoint);
		NVI_SetControlSize(mInfoViewControlID,listViewWidth - vscrollbarWidth,listViewHeight);
		NVI_SetShowControl(mInfoViewControlID,true);
	}
	
	//Vスクロールバー配置
	if( vscrollbarWidth > 0 )//#1370 && NVI_IsControlEnable(mVScrollBarControlID) == true )
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
	
	/*
	//ポップアップ設定ボタン配置
	{
		AWindowPoint	pt = {0};
		pt.x = windowBounds.right - windowBounds.left - kWidth_PopupSetupButton;
		pt.y = 0;
		NVI_SetControlPosition(kControlID_PopupSetupButton,pt);
		NVI_SetControlSize(kControlID_PopupSetupButton,kWidth_PopupSetupButton,kHeight_PopupButtons);
		NVI_SetShowControl(kControlID_PopupSetupButton,false);
	}
	*/
	
	//ポップアップクローズボタン配置
	{
		AWindowPoint	pt = {0};
		pt.x = 0;
		pt.y = 0;
		NVI_SetControlPosition(kControlID_PopupCloseButton,pt);
		NVI_SetControlSize(kControlID_PopupCloseButton,kWidth_PopupCloseButton,kHeight_PopupButtons);
		NVI_SetShowControl(kControlID_PopupCloseButton,false);
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
	
	//描画データ更新
	GetIdInfoView().SPI_UpdateDrawProperty();
}

/*#725
//#238
void	AWindow_IdInfo::SPI_UpdateHistoryButton()
{
	NVI_GetButtonViewByControlID(kHistoryButtonID).NVI_SetText(SPI_GetIdInfoView().SPI_GetCurrentIdText());
}
*/

/*#238
//
void	AWindow_IdInfo::SPI_SetArgIndex( const AIndex inIndex )
{
	if( mFixDisplay == true )   return;
	
	GetIdInfoView().SPI_SetArgIndex(inIndex);
}
*/

/*#725
//表示固定
void	AWindow_IdInfo::SPI_FixDisplay()
{
	//#238 mFixDisplay = !mFixDisplay;
	SPI_GetIdInfoView().SPI_SetFixDisplay(!SPI_GetIdInfoView().SPI_GetFixDisplay());//#238
	//#232 NVI_SetControlBool(kFixDisplayButtonID,mFixDisplay);
	NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetToogleOn(SPI_GetIdInfoView().SPI_GetFixDisplay());
}
*/

/*#238
//現在の識別子テキストを取得
const AText&	AWindow_IdInfo::SPI_GetIdText() const
{
	return GetIdInfoViewConst().SPI_GetIdText();
}
*/

#if 0
//#725

//#291
/**
オーバーレイモード時の透明度設定
*/
void	AWindow_IdInfo::SPI_UpdateOverlayWindowAlpha()
{
	if( NVI_GetOverlayMode() == true )
	{
		GetIdInfoView().SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		//#725 NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		//#725 NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
	}
}

//#634
/**
サイドバーモードの透過率・背景色に切り替え
*/
void	AWindow_IdInfo::SPI_SetSideBarMode( const ABool inSideBarMode, const AColor inSideBarColor )
{
	if( NVI_GetOverlayMode() == true )
	{
		if( inSideBarMode == true )
		{
			//できる限り文字をきれいに表示させるために不透過率はできるだけ高めに設定する。（背景のグラデーションがうっすら透過する程度）
			GetIdInfoView().SPI_SetBackgroundColor(inSideBarColor,kColor_Gray97Percent);
			GetIdInfoView().SPI_SetTransparency(0.8);
			NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTransparency(0.8);
			NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTransparency(0.8);
		}
		else
		{
			GetIdInfoView().SPI_SetBackgroundColor(kColor_White,kColor_White);
			GetIdInfoView().SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
			NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTransparency(1);
			NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTransparency(1);
		}
	}
	else
	{
		//フローティングウインドウ時の背景色等設定
		GetIdInfoView().SPI_SetBackgroundColor(kColor_Gray97Percent,kColor_Gray97Percent);
		GetIdInfoView().SPI_SetTransparency(GetApp().SPI_GetOverlayWindowsAlpha());
		NVI_GetButtonViewByControlID(kFixDisplayButtonID).SPI_SetTransparency(1);
		NVI_GetButtonViewByControlID(kHistoryButtonID).SPI_SetTransparency(1);
	}
}
#endif

/**
ポップアップウインドウのマウスenter/leave時処理
*/
ABool	AWindow_IdInfo::EVTDO_DoMouseMoved( const AWindowPoint& inWindowPoint, const AModifierKeys inModifierKeys )
{
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Popup )
	{
		//NVI_SetShowControl(kControlID_PopupSetupButton,true);
		NVI_SetShowControl(kControlID_PopupCloseButton,true);
		//
		GetIdInfoView().SPI_SetDisableAutoHide();
	}
	/*
	fprintf(stderr,"moved ");
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Popup )
	{
		GetApp().SPI_SetSubWindowProperty(GetObjectID(),kSubWindowLocationType_Floating,kIndex_Invalid);
		//
		UpdateViewBounds();
		NVI_RefreshWindow();
	}
	*/
	return true;
}


