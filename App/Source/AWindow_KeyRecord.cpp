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

AWindow_KeyRecord
#725

*/

#include "stdafx.h"

#include "AWindow_KeyRecord.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowHeader.h"
#include "AView_SubWindowSizeBox.h"

//テキストボックス
const AControlID	kControlID_Text = 101;

//スクロールバー
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kWidth_VScroll = 11;

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

//ヘッダview
const AControlID	kControlID_Header = 906;
const ANumber	kHeight_Header = 20;

//ヘッダ内各種コントロール
const AControlID	kControlID_RecordStart = 106;
const AControlID	kControlID_RecordStop = 107;
const AControlID	kControlID_Play = 108;
const AControlID	kControlID_AddMacro = 109;
const ANumber		kHeight_HeaderButton = 15;

#pragma mark ===========================
#pragma mark [クラス]AWindow_KeyRecord
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_KeyRecord::AWindow_KeyRecord():AWindow()
{
	//#725 タイマー対象ウインドウにする
	NVI_AddToTimerActionWindowArray();
}
/**
デストラクタ
*/
AWindow_KeyRecord::~AWindow_KeyRecord()
{
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_KeyRecord::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + kHeight_Header + 10;
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_KeyRecord::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
メニュー選択時処理
*/
ABool	AWindow_KeyRecord::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
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
void	AWindow_KeyRecord::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_KeyRecord::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	//エディットボックスのテキストをデータに反映
	ApplyTextToKeyRecord();
	//
	ABool	result = false;
	//
	switch(inID)
	{
		//
	  case kControlID_RecordStart:
		{
			//キー記録開始
			GetApp().SPI_StartRecord();
			//
			result = true;
			break;
		}
		//
	  case kControlID_RecordStop:
		{
			//キー記録停止
			GetApp().SPI_StopRecord();
			//
			result = true;
			break;
		}
		//
	  case kControlID_Play:
		{
			//キー再生
			AObjectID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(winID).SPI_GetCurrentFocusTextView().SPI_PlayKeyRecord();
			}
			//
			result = true;
			break;
		}
		//
	  case kControlID_AddMacro:
		{
			//マクロに追加
			AObjectID	winID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( winID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(winID).SPI_ShowAddToolButtonWindowFromKeyRecord();
			}
			//
			result = true;
			break;
		}
	}
	return result;
}

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_KeyRecord::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}


/**
テキスト入力時処理
*/
void	AWindow_KeyRecord::EVTDO_TextInputted( const AControlID inID )
{
}

/**
コントロールの値変更時処理
*/
ABool	AWindow_KeyRecord::EVTDO_ValueChanged( const AControlID inID )
{
	switch(inID)
	{
		//エディットボックスのテキストをデータに反映
	  case kControlID_Text:
		{
			ApplyTextToKeyRecord();
			break;
		}
	}
	return true;
}

/**
コントロールでリターンキー押下時処理
*/
ABool	AWindow_KeyRecord::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )
{
	//フィルタで入力時
	switch(inControlID)
	{
	}
	return true;
}

/**
コントロールでESCキー押下時処理
*/
ABool	AWindow_KeyRecord::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )
{
	//フィルタで入力時
	switch(inControlID)
	{
	}
	return true;
}

/**
ウインドウの閉じるボタン
*/
void	AWindow_KeyRecord::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}

/**
現在のエディットボックスのテキストをキー記録データに反映
*/
void	AWindow_KeyRecord::ApplyTextToKeyRecord()
{
	AText	text;
	NVI_GetEditBoxView(kControlID_Text).NVI_GetText(text);
	GetApp().SPI_SetKeyRecordedRawText(text);
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_KeyRecord::NVIDO_Create( const ADocumentID inDocumentID )
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
	
	//ヘッダview生成
	{
		AViewDefaultFactory<AView_SubWindowHeader>	viewFactory(GetObjectID(),kControlID_Header);
		AWindowPoint	pt = {0,0};
		NVM_CreateView(kControlID_Header,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Header,true);
		NVI_SetControlBindings(kControlID_Header,true,true,true,false,false,true);
	}
	
	//サブウインドウ用フォント取得
	AText	fontName;
	AFloatNumber	fontSize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontName,fontSize);
	
	//記録開始ボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_RecordStart,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_RecordStart,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_RecordStart).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("KeyRecordStart");
		NVI_GetButtonViewByControlID(kControlID_RecordStart).NVI_SetText(text);
	}
	//停止ボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_RecordStop,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_RecordStop,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_RecordStop).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("KeyRecordStop");
		NVI_GetButtonViewByControlID(kControlID_RecordStop).NVI_SetText(text);
	}
	//再生ボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_Play,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_Play,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_Play).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("KeyRecordPlay");
		NVI_GetButtonViewByControlID(kControlID_Play).NVI_SetText(text);
	}
	//マクロに追加ボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_AddMacro,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_AddMacro,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_AddMacro).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("KeyRecordAddMacro");
		NVI_GetButtonViewByControlID(kControlID_AddMacro).NVI_SetText(text);
	}
	
	//SubWindow用タイトルバー生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_KeyRecord");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconTbKeyRecord);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//テキストボックス生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_Text,pt,10,10,kControlID_Invalid,kIndex_Invalid,false,false,false,true,kEditBoxType_Normal);//#1316 kEditBoxType_NoFrameDraw);
		NVI_GetEditBoxView(kControlID_Text).SPI_SetTextForEmptyState(GetEmptyText(),GetEmptyText());
		NVI_GetEditBoxView(kControlID_Text).SPI_SetEnableFocusRing(false);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetCatchReturn(true);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetAutomaticSelectBySwitchFocus(false);
		//NVI_GetEditBoxView(kControlID_Text).SPI_SetReadOnly();
		NVI_SetControlBindings(kControlID_Text,true,true,true,true,false,false);
	}
	
	//SubWindow用セパレータ生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_Text,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//Vスクロールバー生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateScrollBar(kControlID_VScrollBar,pt,kWidth_VScroll,kWidth_VScroll*2);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
		NVI_SetShowControl(kControlID_VScrollBar,true);
		NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
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
void	AWindow_KeyRecord::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	NVI_UpdateProperty();
	//
	UpdateViewBounds();
}

/**
Hide()時等にウインドウのデータを保存する
*/
void	AWindow_KeyRecord::NVIDO_Hide()
{
}

/**
*/
void	AWindow_KeyRecord::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//サブウインドウ用フォント取得
	AText	fontName;
	AFloatNumber	fontSize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontName,fontSize);
	//ヘッダ用フォント取得
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
    //#1316 色はボタンクラス側で設定する AColor	headerlettercolor = GetApp().SPI_GetSubWindowHeaderLetterColor();
	//制御ボタン色設定
	NVI_GetButtonViewByControlID(kControlID_RecordStart).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_RecordStop).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_Play).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_AddMacro).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	//edit box色設定
	NVI_GetEditBoxView(kControlID_Text).NVI_SetTextFont(fontName,fontSize);
	NVI_GetEditBoxView(kControlID_Text).SPI_SetTextColor(GetApp().SPI_GetSubWindowLetterColor(),GetApp().SPI_GetSubWindowLetterColor());
	/*#1316 色はEditBoxクラス側で設定する
	NVI_GetEditBoxView(kControlID_Text).SPI_SetBackgroundColor(GetApp().SPI_GetSubWindowBackgroundColor(true),
															   kColor_Gray70Percent,kColor_Gray70Percent);
	NVI_GetEditBoxView(kControlID_Text).SPI_SetBackgroundColorForEmptyState(GetApp().SPI_GetSubWindowBackgroundColor(true),
																			GetApp().SPI_GetSubWindowBackgroundColor(false));
																			*/
	
	//view bounds更新
	UpdateViewBounds();
}

/**
コントロール状態（Enable/Disable等）を更新
NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()からコールされる
クリックやアクティベートでコールされるので、あまり重い処理をNVMDO_UpdateControlStatus()には入れないこと。
*/
void	AWindow_KeyRecord::NVMDO_UpdateControlStatus()
{
	NVI_SetControlEnable(kControlID_RecordStart,(GetApp().SPI_IsKeyRecording()==false));
	NVI_SetControlEnable(kControlID_RecordStop,(GetApp().SPI_IsKeyRecording()==true));
	NVI_SetControlEnable(kControlID_Play,((GetApp().SPI_IsKeyRecording()==false)&&(GetApp().SPI_ExistRecordedText()==true)));
	NVI_SetControlEnable(kControlID_AddMacro,((GetApp().SPI_IsKeyRecording()==false)&&(GetApp().SPI_ExistRecordedText()==true)));
}

/**
view bounds更新
*/
void	AWindow_KeyRecord::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//タイルバー、セパレータ高さ取得
	ANumber	height_Titlebar = GetApp().SPI_GetSubWindowTitleBarHeight();
	ANumber	height_Separator = GetApp().SPI_GetSubWindowSeparatorHeight();
	ANumber	height_Header = kHeight_Header;
	
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
			leftMargin = 3;//#1316 0;
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
	ANumber	vscrollbarWidth = kWidth_VScroll;
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
			listViewPoint.y += height_Titlebar + height_Header;
			listViewHeight -= height_Titlebar + height_Header;// + height_Separator;
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
			listViewPoint.y += height_Titlebar + height_Header;
			listViewHeight -= height_Titlebar + height_Header;
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
	
	//マクロに追加ボタン配置
	ANumber	w_buttons = 0;
	{
		NVI_GetButtonViewByControlID(kControlID_AddMacro).SPI_SetWidthToFitTextWidth();
		w_buttons += NVI_GetButtonViewByControlID(kControlID_AddMacro).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - w_buttons - 3,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_AddMacro,pt);
	}
	
	//グループ名変更ボタン配置
	{
		NVI_GetButtonViewByControlID(kControlID_Play).SPI_SetWidthToFitTextWidth();
		w_buttons += NVI_GetButtonViewByControlID(kControlID_Play).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - w_buttons - 3,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_Play,pt);
	}
	
	//削除ボタン配置
	{
		NVI_GetButtonViewByControlID(kControlID_RecordStop).SPI_SetWidthToFitTextWidth();
		w_buttons += NVI_GetButtonViewByControlID(kControlID_RecordStop).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - w_buttons - 3,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_RecordStop,pt);
	}
	
	//追加ボタン配置
	{
		NVI_GetButtonViewByControlID(kControlID_RecordStart).SPI_SetWidthToFitTextWidth();
		w_buttons += NVI_GetButtonViewByControlID(kControlID_RecordStart).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - w_buttons - 3,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_RecordStart,pt);
	}
	
	//text box配置
	{
		NVI_SetControlPosition(kControlID_Text,listViewPoint);
		NVI_SetControlSize(kControlID_Text,listViewWidth - vscrollbarWidth,listViewHeight-5);
		NVI_GetEditBoxView(kControlID_Text).SPI_AdjustScroll();
	}
	
	//Vスクロールバー配置
	if( vscrollbarWidth > 0 /*&& NVI_IsControlEnable(kControlID_VScrollBar) == true*/ )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(kControlID_VScrollBar,pt);
		NVI_SetControlSize(kControlID_VScrollBar,vscrollbarWidth,listViewHeight - sizeboxHeight);
		NVI_SetShowControl(kControlID_VScrollBar,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_VScrollBar,false);
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
UIにキー記録テキストを追加
*/
void	AWindow_KeyRecord::SPI_AddRecordedText( const AText& inText )
{
	NVI_GetEditBoxView(kControlID_Text).NVI_AddText(inText);
	NVI_GetEditBoxView(kControlID_Text).SPI_ArrowKeyEdge(kUChar_Down,false);
}

/**
UIにキー記録テキストを設定
*/
void	AWindow_KeyRecord::SPI_SetRecordedText( const AText& inText )
{
	NVI_GetEditBoxView(kControlID_Text).NVI_SetText(inText);
}


