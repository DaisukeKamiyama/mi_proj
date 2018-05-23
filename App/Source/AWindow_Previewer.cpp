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

AWindow_Previewer
#734

*/

#include "stdafx.h"

#include "AWindow_Previewer.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowSizeBox.h"

//WebView controlID
const AControlID	kControlID_WebView = 101;

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

//各ボタン
const AControlID	kControlID_LetterSize = 105;
const ANumber		kHeight_LetterSize = 15;


#pragma mark ===========================
#pragma mark [クラス]AWindow_Previewer
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_Previewer::AWindow_Previewer():AWindow()
{
}
/**
デストラクタ
*/
AWindow_Previewer::~AWindow_Previewer()
{
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_Previewer::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + 10;
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_Previewer::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
メニュー選択時処理
*/
ABool	AWindow_Previewer::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//クローズボタン
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
		//文字サイズパーセント取得
	  case kMenuItemID_Percent:
		{
			ANumber	percent = inDynamicMenuActionText.GetNumber();
			SPI_SetLetterSize(percent);
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
void	AWindow_Previewer::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_Previewer::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	return result;
}

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_Previewer::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

/**
ウインドウの閉じるボタン
*/
void	AWindow_Previewer::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}


#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_Previewer::NVIDO_Create( const ADocumentID inDocumentID )
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
		text.SetLocalizedText("SubWindowTitle_Previewer");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwPreview);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//Web view生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateWebView(kControlID_WebView,pt,100,600);//★サイズを10,10にすると表示されない。調査必要
		NVI_SetShowControl(kControlID_WebView,true);
		NVI_SetControlBindings(kControlID_WebView,true,true,true,true,false,false);
	}
	
	//文字サイズボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_LetterSize,pt,10,kHeight_LetterSize,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_LetterSize,false,true,false,false,false,true);
		NVI_GetButtonViewByControlID(kControlID_LetterSize).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	fontname;
		AFontWrapper::GetDialogDefaultFontName(fontname);
		AColor	color = GetApp().SPI_GetSubWindowTitlebarTextColor();
		NVI_GetButtonViewByControlID(kControlID_LetterSize).SPI_SetTextProperty(fontname,9.0,kJustification_Center,
					kTextStyle_Bold|kTextStyle_DropShadow,color,color);
		//
		NVI_GetButtonViewByControlID(kControlID_LetterSize).SPI_SetMenu(kContextMenuID_Percent,kMenuItemID_Invalid);
	}
	
	//SubWindow用セパレータ生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_WebView,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
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
void	AWindow_Previewer::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	NVI_UpdateProperty();
	//
	UpdateViewBounds();
}

/**
Hide()時等にウインドウのデータを保存する
*/
void	AWindow_Previewer::NVIDO_Hide()
{
}

/**
*/
void	AWindow_Previewer::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//文字サイズ拡大率設定
	AFloatNumber	mul = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kPreviewerMultiplier);
	mul /= 100.0;
	NVI_SetWebViewFontMultiplier(kControlID_WebView,mul);
	//文字サイズボタンに「文字サイズ：～%」を設定
	AText	text;
	text.SetLocalizedText("PreviewWindow_LetterSizeButton");
	text.ReplaceParamText('0',GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kPreviewerMultiplier));
	NVI_GetButtonViewByControlID(kControlID_LetterSize).NVI_SetText(text);
	//描画更新
	NVI_RefreshWindow();
}

/**
WebViewへのURL表示要求
*/
void	AWindow_Previewer::SPI_RequestLoadURL( const AText& inURL )
{
	//前回要求したURLと今回要求URLが同じ場合、何もせずリターン
	if( inURL.Compare(mRequestedURL) == true )
	{
		return;
	}
	
	//表示要求
	//AText	test("file:///Users/daisuke/kadai.pdf#page=3");
	NVI_LoadURLToWebView(kControlID_WebView,inURL);
	
	//要求URLを保存
	mRequestedURL.SetText(inURL);
	//現在表示中のURLを保存
	NVI_GetCurrentWebViewURL(kControlID_WebView,mDisplayedURLWhenLoaded);
}

/**
最後に要求されたURLをリロードする
@note ファイル保存の反映用
*/
void	AWindow_Previewer::SPI_RequestReload()
{
	//現在表示中のURL取得
	AText	currentURL;
	NVI_GetCurrentWebViewURL(kControlID_WebView,currentURL);
	
	if( currentURL.Compare(mDisplayedURLWhenLoaded) == true )
	{
		//現在表示中のURLが、要求時のURLと同じ場合、リロードする
		NVI_ReloadWebView(kControlID_WebView);
	}
	else
	{
		//現在表示中のURLが、要求時のURLと違う場合（＝web view上のユーザー操作によりページ移動した場合）、要求時のURLを再度要求する
		NVI_LoadURLToWebView(kControlID_WebView,mRequestedURL);
		//現在表示中のURLを保存
		NVI_GetCurrentWebViewURL(kControlID_WebView,mDisplayedURLWhenLoaded);
	}
}

/**
Web view内で指定JavaScriptを実行する
*/
void	AWindow_Previewer::SPI_ExecuteJavaScript( const AText& inText )
{
	NVI_ExecuteJavaScriptInWebView(kControlID_WebView,inText);
}

/**
文字サイズ設定
*/
void	AWindow_Previewer::SPI_SetLetterSize( const ANumber inPercent )
{
	GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kPreviewerMultiplier,inPercent);
	NVI_UpdateProperty();
	UpdateViewBounds();
}

/**
View配置更新
*/
void	AWindow_Previewer::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//タイルバー、セパレータ高さ取得
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	
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
	ABool	showSizeBox = false;
	//list viewサイズ
	AWindowPoint	listViewPoint = {leftMargin,0};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin;
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
			showSizeBox = true;
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
	
	//WebView配置
	{
		NVI_SetControlPosition(kControlID_WebView,listViewPoint);
		NVI_SetControlSize(kControlID_WebView,listViewWidth,listViewHeight);
		NVI_SetShowControl(kControlID_WebView,true);
	}
	
	//文字サイズボタン配置
	{
		NVI_GetButtonViewByControlID(kControlID_LetterSize).SPI_SetWidthToFitTextWidth();
		ANumber	w = NVI_GetButtonViewByControlID(kControlID_LetterSize).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right - windowBounds.left - w - 5,3};
		NVI_SetControlPosition(kControlID_LetterSize,pt);
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

