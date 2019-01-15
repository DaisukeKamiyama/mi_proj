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

AWindow_WordsList
#723

*/

#include "stdafx.h"

#include "AWindow_WordsList.h"
#include "AView_WordsList.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowHeader.h"
#include "AView_SubWindowStatusBar.h"
#include "AView_SubWindowSizeBox.h"

//ワードリストビュー
const AControlID	kControlID_WordsListView = 101;

//スクロールバー
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kWidth_VScroll = 11;
const AControlID	kControlID_HScrollBar = 106;
const ANumber		kHeight_HScroll = 11;

//ロックボタン
const AControlID	kControlID_LockDisplay = 107;
const ANumber		kHeight_LockDisplay = 17;

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

//Import Fileプログレス表示
const AControlID	kControlID_ImportFileProgressIndicator = 801;
const AControlID	kControlID_ImportFileProgressText = 802;
const ANumber	kHeight_ProgressArea = 20;
//const AControlID	kControlID_ImportFileProgressTextPanel = 803;

//Findプログレス表示
const AControlID	kControlID_FindProgressIndicator = 811;

//プログレス表示のサイズ
const ANumber	kHeight_ProgressIndicator = 16;
const ANumber	kWidth_ProgressIndicator = 16;

/*
const AControlID	kControlID_ProjectFolderIsNotSetText = 812;
const AControlID	kControlID_ProjectFolderIsNotSetPanel = 813;
const ANumber	kHeight_ProjectFolderIsNotSetPanel = 40;
*/

//ヘッダview
const AControlID	kControlID_Header = 906;
const ANumber	kHeight_Header = 36;

//ヘッダ上のテキスト
const AControlID	kControlID_FindTitle = 701;
const AControlID	kControlID_FindPath = 702;
const ANumber		kHeight_HeaderButton = 15;
const ANumber		kLeftMargin_HeaderButton = 5;
const ANumber		kRightMargin_HeaderButton = 5;

#pragma mark ===========================
#pragma mark [クラス]AWindow_WordsList
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_WordsList::AWindow_WordsList():AWindow(),  mShowImportFileProgress(false)
{
}
/**
デストラクタ
*/
AWindow_WordsList::~AWindow_WordsList()
{
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_WordsList::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + kHeight_Header + 10;
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

/**
ワードリストビュー取得
*/
AView_WordsList&	AWindow_WordsList::SPI_GetWordsListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_WordsList,kViewType_WordsList,kControlID_WordsListView);
}

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_WordsList::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
メニュー項目選択時処理
*/
ABool	AWindow_WordsList::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
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
メニュー項目update
*/
void	AWindow_WordsList::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_WordsList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_LockDisplay:
		{
			SPI_GetWordsListView().SPI_SetLockedMode(NVI_GetControlBool(kControlID_LockDisplay));
			break;
		}
	}
	return result;
}

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_WordsList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
	//
	if( (inCurrentBounds.right-inCurrentBounds.left) != (inPrevBounds.right-inPrevBounds.left) ||
		(inCurrentBounds.bottom-inCurrentBounds.top) != (inPrevBounds.bottom-inPrevBounds.top) )
	{
		SPI_GetWordsListView().SPI_UpdateDrawProperty();
	}
}

/**
ウインドウの閉じるボタン
*/
void	AWindow_WordsList::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}


#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_WordsList::NVIDO_Create( const ADocumentID inDocumentID )
{
	//サブウインドウ用フォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
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
	
	//背景色描画用view生成
	{
		AViewDefaultFactory<AView_SubWindowBackground>	viewFactory(GetObjectID(),kControlID_Background);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Background,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Background,true);
		NVI_SetControlBindings(kControlID_Background,true,true,true,true,false,false);
	}
	
	//ヘッダview生成
	{
		AViewDefaultFactory<AView_SubWindowHeader>	viewFactory(GetObjectID(),kControlID_Header);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Header,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Header,true);
		NVI_SetControlBindings(kControlID_Header,true,true,true,false,false,true);
	}
	
	//検索タイトルボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_FindTitle,pt,10,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_FindTitle,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_FindTitle).SPI_SetButtonViewType(kButtonViewType_TextWithNoHoverWithFixedWidth);
		NVI_GetButtonViewByControlID(kControlID_FindTitle).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,10);
	}
	
	//検索パスボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_FindPath,pt,10,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_FindPath,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_FindPath).SPI_SetButtonViewType(kButtonViewType_TextWithNoHoverWithFixedWidth);
		NVI_GetButtonViewByControlID(kControlID_FindPath).SPI_SetEllipsisMode(kEllipsisMode_FixedFirstCharacters,20);
	}
	
	//SubWindow用タイトルバー生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_WordsList");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwWordReferrer);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//WordsList view生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_WordsList>	viewFactory(GetObjectID(),kControlID_WordsListView);
		NVM_CreateView(kControlID_WordsListView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_WordsListView,true,true,true,true,false,false);
	}
	
	/*
	//Import File Progressテキストパネル生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowStatusBar>	viewFactory(GetObjectID(),kControlID_ImportFileProgressTextPanel);
		NVM_CreateView(kControlID_ImportFileProgressTextPanel,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_ImportFileProgressTextPanel,true,true,true,false,false,true);
		//背景色等設定
		NVI_GetViewByControlID(kControlID_ImportFileProgressTextPanel).NVI_SetBackgroundColor(
					GetApp().SPI_GetSubWindowBackgroundColor(true),
					GetApp().SPI_GetSubWindowBackgroundColor(false));
	}
	*/
	
	//ヘッダ部分フォント
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	//Import File Progressテキスト生成
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateButtonView(kControlID_ImportFileProgressText,pt,10,kHeight_ProgressIndicator,
							 kControlID_Background);//kControlID_ImportFileProgressTextPanel);
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).SPI_SetButtonViewType(kButtonViewType_NoFrame);
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,kTextStyle_Normal);//#1316
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).SPI_SetDropShadowColor(kColor_Gray80Percent);
		//
		AText	text;
		text.SetLocalizedText("Progress_ImportFile");
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).NVI_SetText(text);
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).SPI_SetWidthToFitTextWidth();
	}
	
	//Import File Progress Indicator生成
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateProgressIndicator(kControlID_ImportFileProgressIndicator,pt,kWidth_ProgressIndicator,kHeight_ProgressIndicator);
	}
	
	//Find Progress Indicator生成
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateProgressIndicator(kControlID_FindProgressIndicator,pt,kWidth_ProgressIndicator,kHeight_ProgressIndicator);
		NVI_EmbedControl(kControlID_WordsListView,kControlID_FindProgressIndicator);
	}
	
	/*
	
	//Project Folder未設定テキストパネル生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_TextPanel>	viewFactory(GetObjectID(),kControlID_ProjectFolderIsNotSetPanel);
		NVM_CreateView(kControlID_ProjectFolderIsNotSetPanel,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_ProjectFolderIsNotSetPanel,true,true,true,false,false,true);
		//背景色等設定
		NVI_GetViewByControlID(kControlID_ProjectFolderIsNotSetPanel).NVI_SetBackgroundColor(
					GetApp().SPI_GetSubWindowBackgroundColor(true),
					GetApp().SPI_GetSubWindowBackgroundColor(false));
	}
	
	//Import File Progressテキスト生成
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateButtonView(kControlID_ProjectFolderIsNotSetText,pt,10,kHeight_ProgressIndicator,kControlID_ProjectFolderIsNotSetPanel);
		NVI_GetButtonViewByControlID(kControlID_ProjectFolderIsNotSetText).SPI_SetButtonViewType(kButtonViewType_NoFrame);
		NVI_GetButtonViewByControlID(kControlID_ProjectFolderIsNotSetText).SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,
					kTextStyle_DropShadow,kColor_Black,kColor_Black);
		NVI_GetButtonViewByControlID(kControlID_ProjectFolderIsNotSetText).SPI_SetDropShadowColor(kColor_Gray80Percent);
		//
		AText	text;
		text.SetLocalizedText("Progress_ImportFile");
		NVI_GetButtonViewByControlID(kControlID_ProjectFolderIsNotSetText).NVI_SetText(text);
		NVI_GetButtonViewByControlID(kControlID_ProjectFolderIsNotSetText).SPI_SetWidthToFitTextWidth();
	}
	*/
	//==================
	
	//SubWindow用セパレータ生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_WordsListView,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//Vスクロールバー生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateScrollBar(kControlID_VScrollBar,pt,kWidth_VScroll,kWidth_VScroll*2);
		SPI_GetWordsListView().NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
		NVI_SetShowControl(kControlID_VScrollBar,true);
		NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	}
	
	//Lockボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_LockDisplay,pt,10,kHeight_LockDisplay,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_LockDisplay,false,true,false,false,false,true);
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).
				SPI_SetOvalHoverColor(GetApp().SPI_GetSubWindowTitlebarButtonHoverColor());
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).SPI_SetIconImageID(kImageID_iconSwUnlock,
																				kImageID_Invalid,kImageID_iconSwLock);
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).SPI_SetToggleMode(true);
		AText	text;
		text.SetLocalizedText("WordsListLock");
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).NVI_SetText(text);
	}
	
	//SizeBox生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
	
	//ヘッダテキスト初期テキスト設定
	SPI_SetHeaderText(GetEmptyText(),GetEmptyText(),kIndex_Invalid);
}

/**
新規タブ生成
引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_WordsList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	UpdateViewBounds();
}

/**
Hide()時等にウインドウのデータを保存する
*/
void	AWindow_WordsList::NVIDO_Hide()
{
}

/**
ボタン等プロパティ更新
*/
void	AWindow_WordsList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//検索中プログレス更新
	NVI_SetShowControl(kControlID_FindProgressIndicator,SPI_GetWordsListView().SPI_IsFinding());
	NVI_SetControlBool(kControlID_FindProgressIndicator,true);
	
	//ヘッダ部分フォント、色
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
    //#1316 AColor	headerlettercolor = GetApp().SPI_GetSubWindowHeaderLetterColor();
	NVI_GetButtonViewByControlID(kControlID_FindTitle).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_FindPath).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,kTextStyle_Bold);//#1316
	
	//ロックボタン色設定
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);
    //#1316 AColor	color = GetApp().SPI_GetSubWindowTitlebarTextColor();
	NVI_GetButtonViewByControlID(kControlID_LockDisplay).
			SPI_SetTextProperty(fontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	
	//view bounds更新
	UpdateViewBounds();
	NVI_RefreshWindow();
}

/**
view bounds更新
*/
void	AWindow_WordsList::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//タイルバー、セパレータ高さ取得
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	ANumber	height_Header = kHeight_Header;
	
	//サイドバーの一番下の項目かどうかを取得
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
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - listViewPoint.x - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;// - kHeight_HScroll;
	//ヘッダー部分表示位置・サイズ
	AWindowPoint	headerViewPoint = {leftMargin,0};
	//ANumber	headerViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
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
			listViewPoint.y += height_Titlebar /*+ kHeight_HeaderButtons*/ + height_Header;
			listViewHeight -= height_Titlebar /*+ kHeight_HeaderButtons*/ + height_Header;// + height_Separator;
			//ヘッダー部分表示位置
			headerViewPoint.y += height_Titlebar;// + kHeight_HeaderButtons;
			/*
			//右サイドバーの最終項目の場合、分割線非表示
			if( isSideBarBottom == true )
			{
				//セパレータ非表示
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
			listViewPoint.y += height_Titlebar /*+ kHeight_HeaderButtons*/ + height_Header;
			listViewHeight -= height_Titlebar /*+ kHeight_HeaderButtons*/ + height_Header;
			//ヘッダー部分表示位置
			headerViewPoint.y += height_Titlebar;// + kHeight_HeaderButtons;
			//サイズボックス表示
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
	
	/*
	//プロジェクトフォルダ未設定表示 配置計算
	ABool	showProjectFolderIsNotSet = false;
	ANumber	showProjectFolderIsNotSetHeight = 0;
	AWindowPoint	projectFolderIsNotSetPoint = {0};
	if( SPI_GetWordsListView().SPI_GetSearchInProjectFolder() == false )
	{
		//
		showProjectFolderIsNotSet = true;
		showProjectFolderIsNotSetHeight = kHeight_ProjectFolderIsNotSetPanel;
		//
		//list view高さをプログレスバー分減らす
		listViewHeight -= showProjectFolderIsNotSetHeight;
		//プログレスバー位置計算
		projectFolderIsNotSetPoint.x = listViewPoint.x;
		projectFolderIsNotSetPoint.y = listViewPoint.y + listViewHeight;
	}
	*/
	
	//プログレスバー配置計算
	ABool	showProgress = mShowImportFileProgress;
	if( listViewHeight < kHeight_ProgressArea )   showProgress = false;
	AWindowPoint	progressPoint = {0};
	if( showProgress == true )
	{
		//list view高さをプログレスバー分減らす
		listViewHeight -= kHeight_ProgressArea;
		//プログレスバー位置計算
		progressPoint.x = leftMargin;
		progressPoint.y = listViewPoint.y + listViewHeight;// + showProjectFolderIsNotSetHeight;
		//
		//projectFolderIsNotSetPoint.y -= kHeight_ProgressArea;
	}
	
	//タイトルバー配置
	{
		//タイトルバー配置
		AWindowPoint	pt = {-1+leftMargin,0};
		NVI_SetControlPosition(kControlID_TitleBar,pt);
		NVI_SetControlSize(kControlID_TitleBar,windowBounds.right - windowBounds.left +2-leftMargin-rightMargin,height_Titlebar);
		//タイトルバー表示
		NVI_SetShowControl(kControlID_TitleBar,true);
	}
	
	//背景描画view配置
	{
		//背景描画view配置
		NVI_SetControlPosition(kControlID_Background,backgroundPt);
		NVI_SetControlSize(kControlID_Background,windowBounds.right - windowBounds.left,windowBounds.bottom - windowBounds.top - backgroundPt.y);
		//背景描画view表示
		NVI_SetShowControl(kControlID_Background,true);
	}
	
	//==================Words List view配置==================
	{
		NVI_SetControlPosition(kControlID_WordsListView,listViewPoint);
		NVI_SetControlSize(kControlID_WordsListView,listViewWidth - vscrollbarWidth,listViewHeight);
	}
	
	/*
	//プロジェクトフォルダ未設定テキストパネル配置
	{
		NVI_SetControlPosition(kControlID_ProjectFolderIsNotSetPanel,projectFolderIsNotSetPoint);
		NVI_SetControlSize(kControlID_ProjectFolderIsNotSetPanel,windowBounds.right - windowBounds.left - vscrollbarWidth,kHeight_ProjectFolderIsNotSetPanel);
		NVI_SetShowControl(kControlID_ProjectFolderIsNotSetPanel,showProjectFolderIsNotSet);
	}
	*/
	//==================ImportFileProgress配置==================
	
	/*
	//Import File Progressテキストパネル配置・表示
	{
		NVI_SetControlPosition(kControlID_ImportFileProgressTextPanel,progressPoint);
		NVI_SetControlSize(kControlID_ImportFileProgressTextPanel,windowBounds.right - windowBounds.left - vscrollbarWidth,kHeight_ProgressArea);
		NVI_SetShowControl(kControlID_ImportFileProgressTextPanel,showProgress);
	}
	*/
	//Import File Progress Indicator配置
	{
		AWindowPoint	pt = {0};
		pt.x = progressPoint.x + 5;
		pt.y = progressPoint.y + 2;
		NVI_SetControlPosition(kControlID_ImportFileProgressIndicator,pt);
		NVI_SetShowControl(kControlID_ImportFileProgressIndicator,showProgress);
		//
		NVI_SetControlBool(kControlID_ImportFileProgressIndicator,true);
	}
	//Import File Progress Text配置
	{
		AWindowPoint	pt = {0};
		pt.x = progressPoint.x + 23;
		pt.y = progressPoint.y + 3;
		NVI_SetControlPosition(kControlID_ImportFileProgressText,pt);
		NVI_SetShowControl(kControlID_ImportFileProgressText,showProgress);
	}
	
	//Find Progress Indicator配置
	{
		AWindowPoint	pt = {0};
		pt.x = listViewPoint.x + 5;
		pt.y = listViewPoint.y + 2;
		NVI_SetControlPosition(kControlID_FindProgressIndicator,pt);
		NVI_SetShowControl(kControlID_FindProgressIndicator,showProgress);
		//
		NVI_SetShowControl(kControlID_FindProgressIndicator,SPI_GetWordsListView().SPI_IsFinding());
		NVI_SetControlBool(kControlID_FindProgressIndicator,true);
	}
	//Vスクロールバー配置
	if( vscrollbarWidth > 0 && NVI_IsControlEnable(kControlID_VScrollBar) == true )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(kControlID_VScrollBar,pt);
		NVI_SetControlSize(kControlID_VScrollBar,vscrollbarWidth,vscrollbarHeight);
		NVI_SetShowControl(kControlID_VScrollBar,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_VScrollBar,false);
	}
	
	//ヘッダ配置
	if( height_Header > 0 )
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar};
		NVI_SetControlPosition(kControlID_Header,pt);
		NVI_SetControlSize(kControlID_Header,windowBounds.right-windowBounds.left - leftMargin - rightMargin,height_Header);
		NVI_SetShowControl(kControlID_Header,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_Header,false);
	}
	
	//検索タイトルボタン配置
	{
		//NVI_GetButtonViewByControlID(kControlID_FindTitle).SPI_SetWidthToFitTextWidth();
		//ANumber	w = NVI_GetButtonViewByControlID(kControlID_FindTitle).NVI_GetViewWidth();
		AWindowPoint	pt = {kLeftMargin_HeaderButton,height_Titlebar + 2};
		NVI_SetControlPosition(kControlID_FindTitle,pt);
		NVI_SetControlSize(kControlID_FindTitle,windowBounds.right-windowBounds.left-kLeftMargin_HeaderButton-kRightMargin_HeaderButton,kHeight_HeaderButton);
	}
	
	//検索パスボタン配置
	{
		//NVI_GetButtonViewByControlID(kControlID_FindPath).SPI_SetWidthToFitTextWidth();
		//ANumber	w = NVI_GetButtonViewByControlID(kControlID_FindPath).NVI_GetViewWidth();
		AWindowPoint	pt = {kLeftMargin_HeaderButton,height_Titlebar + 19};
		NVI_SetControlPosition(kControlID_FindPath,pt);
		NVI_SetControlSize(kControlID_FindPath,windowBounds.right-windowBounds.left-kLeftMargin_HeaderButton-kRightMargin_HeaderButton,kHeight_HeaderButton);
	}
	
	//Lockボタン配置
	{
		NVI_GetButtonViewByControlID(kControlID_LockDisplay).SPI_SetWidthToFitTextWidth();
		ANumber	w = NVI_GetButtonViewByControlID(kControlID_LockDisplay).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right - windowBounds.left - w - 5,1};
		NVI_SetControlPosition(kControlID_LockDisplay,pt);
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

/**
Import File Progressの表示／非表示
*/
void	AWindow_WordsList::SPI_ShowHideImportFileProgress( const ABool inShow )
{
	if( mShowImportFileProgress == inShow )
	{
		return;
	}
	
	//表示／非表示変更
	mShowImportFileProgress = inShow;
	UpdateViewBounds();
}

/**
ヘッダ部分のテキスト更新
*/
void	AWindow_WordsList::SPI_SetHeaderText( const AText& inWord, const AText& inProjectPath, const AModeIndex inModeIndex )
{
	//
	AText	headertext;
	headertext.SetLocalizedText("WordsListHeader");
	headertext.ReplaceParamText('0',inWord);
	AText	modename;
	if( inModeIndex != kIndex_Invalid )
	{
		GetApp().SPI_GetModePrefDB(inModeIndex).GetModeDisplayName(modename);
	}
	//
	AText	headertext2;
	headertext2.SetLocalizedText("WordsListHeader2");
	headertext2.ReplaceParamText('0',inProjectPath);
	headertext2.ReplaceParamText('1',modename);
	//
	NVI_GetViewByControlID(kControlID_FindTitle).NVI_SetText(headertext);
	NVI_GetViewByControlID(kControlID_FindPath).NVI_SetText(headertext2);
	//UpdateViewBounds();
}

/**
ロックモード設定
*/
void	AWindow_WordsList::SPI_SetLockedMode( const ABool inLocked )
{
	NVI_SetControlBool(kControlID_LockDisplay,inLocked);
	SPI_GetWordsListView().SPI_SetLockedMode(inLocked);
}

