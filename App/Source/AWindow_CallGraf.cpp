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

AWindow_CallGraf
#723

*/

#include "stdafx.h"

#include "AWindow_CallGraf.h"
#include "AView_CallGraf.h"
#include "AView_CallGrafHeader.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowStatusBar.h"
#include "AView_SubWindowSizeBox.h"

//コールグラフ
const AControlID	kControlID_CallGrafView = 101;

//スクロールバー
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kWidth_VScroll = 11;
//const AControlID	kControlID_HScrollBar = 106;
//const ANumber		kHeight_HScroll = 11;

//CallGrafヘッダ
const AControlID	kControlID_CallGrafHeaderView = 110;
const ANumber		kHeight_CallGrafHeader = AView_CallGrafHeader::kLineHeight*2;

//各ボタン
const AControlID	kControlID_ReturnHome = 105;
const ANumber		kHeight_HomeButton = 17;
const ANumber		kWidth_LevelButton = 72;
const ANumber		kHeight_HeaderButtons = 18;

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
//const AControlID	kControlID_ImportFileProgressStatusBar = 803;

//Findプログレス表示
const AControlID	kControlID_FindProgressIndicator = 811;

//Progress indicatorサイズ
const ANumber	kHeight_ProgressIndicator = 16;
const ANumber	kWidth_ProgressIndicator = 16;


#pragma mark ===========================
#pragma mark [クラス]AWindow_CallGraf
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_CallGraf::AWindow_CallGraf():AWindow(), mAllFilesSearched(false), mShowImportFileProgress(false)
{
}
/**
デストラクタ
*/
AWindow_CallGraf::~AWindow_CallGraf()
{
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_CallGraf::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + kHeight_CallGrafHeader + 10;
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

/**
TextView取得
*/
AView_CallGraf&	AWindow_CallGraf::SPI_GetCallGrafView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_CallGraf,kViewType_CallGraf,kControlID_CallGrafView);
}

/**
CallGrafビュー取得
*/
AView_CallGrafHeader&	AWindow_CallGraf::SPI_GetCallGrafHeaderView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_CallGrafHeader,kViewType_CallGrafHeader,kControlID_CallGrafHeaderView);
}

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_CallGraf::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
メニュー項目選択時処理
*/
ABool	AWindow_CallGraf::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
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
メニュー更新
*/
void	AWindow_CallGraf::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_CallGraf::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kControlID_ReturnHome:
		{
			SPI_GetCallGrafView().SPI_ReturnHome();
			break;
		}
	  case kControlID_LeftLevelButton:
		{
			SPI_GetCallGrafView().SPI_GoLeftLevel();
			break;
		}
	  case kControlID_RightLevelButton:
		{
			SPI_GetCallGrafView().SPI_GoRightLevel();
			break;
		}
	}
	return result;
}

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_CallGraf::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

/**
ウインドウの閉じるボタン
*/
void	AWindow_CallGraf::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}


#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_CallGraf::NVIDO_Create( const ADocumentID inDocumentID )
{
	//サブウインドウ用フォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//ヘッダ部分フォント
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	
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
	
	//SubWindow用タイトルバー生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_CallGraf");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwTree);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//==================CallGraf view生成==================
	
	//CallGraf view生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_CallGraf>	callGrafViewFactory(GetObjectID(),kControlID_CallGrafView);
		NVM_CreateView(kControlID_CallGrafView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,callGrafViewFactory);
		NVI_SetControlBindings(kControlID_CallGrafView,true,true,true,true,false,false);
	}
	
	//==================Import File Progress生成==================
	
	/*
	//Import File Progressテキストパネル生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowStatusBar>	viewFactory(GetObjectID(),kControlID_ImportFileProgressStatusBar);
		NVM_CreateView(kControlID_ImportFileProgressStatusBar,pt,10,10,kControlID_CallGrafView,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_ImportFileProgressStatusBar,true,true,true,false,false,true);
		//背景色等設定
		NVI_GetViewByControlID(kControlID_ImportFileProgressStatusBar).NVI_SetBackgroundColor(
					GetApp().SPI_GetSubWindowBackgroundColor(true),
					GetApp().SPI_GetSubWindowBackgroundColor(false));
	}
	*/
	
	//Import File Progressテキスト生成
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateButtonView(kControlID_ImportFileProgressText,pt,10,kHeight_ProgressIndicator,
							 kControlID_Background);//kControlID_ImportFileProgressStatusBar);
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).SPI_SetButtonViewType(kButtonViewType_NoFrame);
		NVI_GetButtonViewByControlID(kControlID_ImportFileProgressText).
				SPI_SetTextProperty(headerfontname,9.0,kJustification_Left,kTextStyle_Normal);//#1316
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
	
	//==================Find Progress生成==================
	
	//Find Progress Indicator生成
	{
		//
		AWindowPoint	pt = {0};
		NVI_CreateProgressIndicator(kControlID_FindProgressIndicator,pt,kWidth_ProgressIndicator,kHeight_ProgressIndicator);
		NVI_EmbedControl(kControlID_CallGrafView,kControlID_FindProgressIndicator);
	}
	
	//==================セパレータ生成==================
	
	//SubWindow用セパレータ生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_CallGrafView,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//==================CallGrafヘッダ生成==================
	
	//CallGrafHeader view生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_CallGrafHeader>	viewFactory(GetObjectID(),kControlID_CallGrafHeaderView);
		NVM_CreateView(kControlID_CallGrafHeaderView,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_CallGrafHeaderView,true,true,true,false,false,true);
	}
	
	//==================制御ボタン生成==================
	
	//ホームボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_ReturnHome,pt,10,kHeight_HomeButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_ReturnHome,false,true,false,false,false,true);
		NVI_GetButtonViewByControlID(kControlID_ReturnHome).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		NVI_GetButtonViewByControlID(kControlID_ReturnHome).
				SPI_SetOvalHoverColor(GetApp().SPI_GetSubWindowTitlebarButtonHoverColor());
		NVI_GetButtonViewByControlID(kControlID_ReturnHome).SPI_SetIcon(kImageID_iconSwHome);
		AText	text;
		text.SetLocalizedText("CallGrafHome");
		NVI_GetButtonViewByControlID(kControlID_ReturnHome).NVI_SetText(text);
	}
	
	/*
	//左レベルボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_LeftLevelButton,pt,10,10,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_LeftLevelButton,true,true,false,false,true,true);
	}
	
	//右レベルボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_RightLevelButton,pt,10,10,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_RightLevelButton,false,true,true,false,true,true);
	}
	*/
	
	//==================ウインドウ制御コントロール生成==================
	
	//Vスクロールバー生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateScrollBar(kControlID_VScrollBar,pt,kWidth_VScroll,kWidth_VScroll*2);
		SPI_GetCallGrafView().NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
		NVI_SetShowControl(kControlID_VScrollBar,true);
		NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	}
	
	/*
	//Hスクロールバー生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateScrollBar(kControlID_HScrollBar,pt,kHeight_HScroll*2,kHeight_HScroll);
		NVI_SetMinMax(kControlID_HScrollBar,0,100);
		NVI_SetShowControl(kControlID_HScrollBar,true);
		NVI_SetControlBindings(kControlID_HScrollBar,true,false,true,true,false,true);
	}
	*/
	//SizeBox生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
}

/**
新規タブ生成
引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_CallGraf::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	UpdateViewBounds();
}

/**
Hide()時等にウインドウのデータを保存する
*/
void	AWindow_CallGraf::NVIDO_Hide()
{
}

/**
*/
void	AWindow_CallGraf::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//検索プログレス表示・非表示制御
	NVI_SetShowControl(kControlID_FindProgressIndicator,SPI_GetCallGrafView().SPI_IsFinding());
	NVI_SetControlBool(kControlID_FindProgressIndicator,true);
	
	//ホームボタン色設定
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);
    //#1316 AColor	color = GetApp().SPI_GetSubWindowTitlebarTextColor();
	NVI_GetButtonViewByControlID(kControlID_ReturnHome).
			SPI_SetTextProperty(fontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	
	//view bounds更新
	UpdateViewBounds();
	//描画更新
	NVI_RefreshWindow();
}

/**
view bounds更新
*/
void	AWindow_CallGraf::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//タイルバー、セパレータ高さ取得
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	
	//サイドバー最後の項目かどうかを取得
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
	//
	AWindowPoint	headerViewPoint = {leftMargin,0};
	ANumber	headerViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
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
			listViewPoint.y += height_Titlebar /*+ kHeight_HeaderButtons*/ + kHeight_CallGrafHeader;
			listViewHeight -= height_Titlebar /*+ kHeight_HeaderButtons*/ + kHeight_CallGrafHeader;// + height_Separator;
			//
			headerViewPoint.y += height_Titlebar;// + kHeight_HeaderButtons;
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
			listViewPoint.y += height_Titlebar /*+ kHeight_HeaderButtons*/ + kHeight_CallGrafHeader;
			listViewHeight -= height_Titlebar /*+ kHeight_HeaderButtons*/ + kHeight_CallGrafHeader;
			//
			headerViewPoint.y += height_Titlebar;// + kHeight_HeaderButtons;
			//
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
	
	//プログレスバー配置計算
	ABool	showProgress = mShowImportFileProgress;
	AWindowPoint	progressPoint = {0};
	if( showProgress == true )
	{
		//list view高さをプログレスバー分減らす
		listViewHeight -= kHeight_ProgressArea;
		//プログレスバー位置計算
		progressPoint.x = leftMargin;
		progressPoint.y = listViewPoint.y + listViewHeight;
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
	
	//CallGrafHeader配置
	{
		NVI_SetControlPosition(kControlID_CallGrafHeaderView,headerViewPoint);
		NVI_SetControlSize(kControlID_CallGrafHeaderView,headerViewWidth,kHeight_CallGrafHeader);
	}
	
	//==================CallGraf配置==================
	{
		NVI_SetControlPosition(kControlID_CallGrafView,listViewPoint);
		NVI_SetControlSize(kControlID_CallGrafView,listViewWidth - vscrollbarWidth,listViewHeight);
	}
	
	//==================Import File Progress配置==================
	/*
	//Import File Progressテキストパネル配置・表示
	{
		NVI_SetControlPosition(kControlID_ImportFileProgressStatusBar,progressPoint);
		NVI_SetControlSize(kControlID_ImportFileProgressStatusBar,
		windowBounds.right - windowBounds.left - vscrollbarWidth,kHeight_ProgressArea);
		NVI_SetShowControl(kControlID_ImportFileProgressStatusBar,showProgress);
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
	//==================検索プログレス配置==================
	
	//Find Progress Indicator配置
	{
		AWindowPoint	pt = {0};
		pt.x = listViewPoint.x + 5;
		pt.y = listViewPoint.y + 2;
		NVI_SetControlPosition(kControlID_FindProgressIndicator,pt);
		NVI_SetShowControl(kControlID_FindProgressIndicator,showProgress);
		//
		NVI_SetShowControl(kControlID_FindProgressIndicator,SPI_GetCallGrafView().SPI_IsFinding());
		NVI_SetControlBool(kControlID_FindProgressIndicator,true);
	}
	
	//==================制御ボタン配置==================
	
	//ホームボタン配置
	{
		NVI_GetButtonViewByControlID(kControlID_ReturnHome).SPI_SetWidthToFitTextWidth();
		ANumber	w = NVI_GetButtonViewByControlID(kControlID_ReturnHome).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right - windowBounds.left - w - 5,1};
		NVI_SetControlPosition(kControlID_ReturnHome,pt);
	}
	
	/*
	//左レベルボタン配置
	{
		AWindowPoint	pt = {leftMargin,height_Titlebar};
		NVI_SetControlPosition(kControlID_LeftLevelButton,pt);
		NVI_SetControlSize(kControlID_LeftLevelButton,kWidth_LevelButton-leftMargin,kHeight_HeaderButtons);
	}
	
	//右レベルボタン配置
	{
		AWindowPoint	pt = {windowBounds.right - windowBounds.left - kWidth_LevelButton,height_Titlebar};
		NVI_SetControlPosition(kControlID_RightLevelButton,pt);
		NVI_SetControlSize(kControlID_RightLevelButton,kWidth_LevelButton-rightMargin,kHeight_HeaderButtons);
	}
	*/
	
	//==================ウインドウ制御コントロール配置==================
	
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
	/*
	//Hスクロールバー配置
	{
		AWindowPoint	pt = {0,listViewPoint.y + listViewHeight};
		NVI_SetControlPosition(kControlID_HScrollBar,pt);
		NVI_SetControlSize(kControlID_HScrollBar,listViewWidth,kHeight_HScroll);
		NVI_SetShowControl(kControlID_HScrollBar,true);
	}
	*/
	
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
Import File Progress表示・非表示
*/
void	AWindow_CallGraf::SPI_ShowHideImportFileProgress( const ABool inShow )
{
	//表示・非表示変更なしなら何もしない
	if( mShowImportFileProgress == inShow )
	{
		return;
	}
	
	//表示・非表示
	mShowImportFileProgress = inShow;
	UpdateViewBounds();
}



