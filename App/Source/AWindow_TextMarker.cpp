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

AWindow_TextMarker
#750

*/

#include "stdafx.h"

#include "AWindow_TextMarker.h"
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
const ANumber	kHeight_Header = 40;
const ANumber	kHeight_Header1Line = 20;

//ヘッダ内各種コントロール
const AControlID	kControlID_SelectGroupButton = 105;
const AControlID	kControlID_AddGroupButton = 106;
const AControlID	kControlID_DeleteGroupButton = 107;
const AControlID	kControlID_ChangeGroupNameButton = 108;
const AControlID	kControlID_DeleteGroupButtonOK = 109;
const AControlID	kControlID_NameEditBox = 110;
const ANumber		kHeight_HeaderButton = 15;

#pragma mark ===========================
#pragma mark [クラス]AWindow_TextMarker
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_TextMarker::AWindow_TextMarker():AWindow(),mGroupNameEditMode(false)
{
	//#725 タイマー対象ウインドウにする
	NVI_AddToTimerActionWindowArray();
}
/**
デストラクタ
*/
AWindow_TextMarker::~AWindow_TextMarker()
{
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_TextMarker::SPI_GetSubWindowMinHeight() const
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
AView_SubWindowTitlebar&	AWindow_TextMarker::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
メニュー選択時処理
*/
ABool	AWindow_TextMarker::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
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
		//グループ選択
	  case kMenuItemID_TextMarkerSelectGroup:
		{
			//現在のテキスト内容を適用
			ApplyMarkerTextToDocument();
			//グループ変更
			AIndex	index = inDynamicMenuActionText.GetNumber();
			GetApp().SPI_SetCurrentTextMarkerGroupIndex(index);
			result = true;
			//描画プロパティ更新
			NVI_UpdateProperty();
			//view bouns更新
			UpdateViewBounds();
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
void	AWindow_TextMarker::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_TextMarker::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	//
	if( inID != kControlID_NameEditBox )
	{
		ClearGroupNameEditMode(true);
	}
	//
	switch(inID)
	{
		//グループ追加ボタン
	  case kControlID_AddGroupButton:
		{
			//現在のテキスト内容を適用
			ApplyMarkerTextToDocument();
			//新規テキストマーカーグループ追加
			GetApp().SPI_CreateNewTextMarkerGroup();
			//描画プロパティ更新
			NVI_UpdateProperty();
			//view bounds更新
			UpdateViewBounds();
			//
			result = true;
			break;
		}
		//グループ削除ボタン
	  case kControlID_DeleteGroupButton:
		{
			//if( GetApp().SPI_GetCurrentTextMarkerGroupIndex() != 0 )
			if( GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kTextMarkerArray_Text) > 1 )
			{
				//削除確認ダイアログ表示
				AText	mes1, mes2, mes3;
				mes1.SetLocalizedText("TextMarker_DeleteButton1");
				mes2.SetLocalizedText("TextMarker_DeleteButton2");
				mes3.SetLocalizedText("TextMarker_DeleteButton3");
				NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kControlID_DeleteGroupButtonOK);
			}
			//
			result = true;
			break;
		}
		//削除確認ダイアログでOKクリック
	  case kControlID_DeleteGroupButtonOK:
		{
			//現在のテキストマーカーグループ削除
			GetApp().SPI_DeleteCurrentTextMarkerGroup();
			//描画プロパティ更新
			NVI_UpdateProperty();
			//view bounds更新
			UpdateViewBounds();
			//
			result = true;
			break;
		}
		//グループ名変更ボタン
	  case kControlID_ChangeGroupNameButton:
		{
			//現在のテキスト内容を適用
			ApplyMarkerTextToDocument();
			//グループ名変更モードON
			mGroupNameEditMode = true;
			//描画プロパティ更新
			NVI_UpdateProperty();
			//view bounds更新
			UpdateViewBounds();
			//グループタイトル取得、edit boxに設定
			AText	text, title;
			GetApp().SPI_GetCurrentTextMarkerData(title,text);
			NVI_GetEditBoxView(kControlID_NameEditBox).NVI_SetText(title);
			//edit boxにフォーカス移動
			NVI_SwitchFocusTo(kControlID_NameEditBox);
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
void	AWindow_TextMarker::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}


/**
テキスト入力時処理
*/
void	AWindow_TextMarker::EVTDO_TextInputted( const AControlID inID )
{
}

/**
コントロールの値変更時処理
*/
ABool	AWindow_TextMarker::EVTDO_ValueChanged( const AControlID inID )
{
	switch(inID)
	{
		//マーカーテキスト変更時処理
	  case kControlID_Text:
		{
			//各ドキュメントにマーカー適用
			ApplyMarkerTextToDocument();
			break;
		}
	}
	return true;
}

/**
コントロールでリターンキー押下時処理
*/
ABool	AWindow_TextMarker::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )
{
	//フィルタで入力時
	switch(inControlID)
	{
	  case kControlID_Text:
		{
			//list viewでのクリック時と同じ処理を実行
			ApplyMarkerTextToDocument();
			break;
		}
	  case kControlID_NameEditBox:
		{
			//グループ名編集モード解除
			ClearGroupNameEditMode(true);
			break;
		}
	}
	return true;
}

/**
コントロールでESCキー押下時処理
*/
ABool	AWindow_TextMarker::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )
{
	//フィルタで入力時
	switch(inControlID)
	{
	  case kControlID_NameEditBox:
		{
			ClearGroupNameEditMode(false);
			break;
		}
	}
	return true;
}

/**
グループ名編集モード解除
*/
void	AWindow_TextMarker::ClearGroupNameEditMode( const ABool inApply )
{
	if( mGroupNameEditMode == true )
	{
		if( inApply == true )
		{
			//グループ名変更適用
			AText	title;
			NVI_GetEditBoxView(kControlID_NameEditBox).NVI_GetText(title);
			GetApp().SPI_SetCurrentTextMarkerData_Title(title);
		}
		//グループ名編集モード解除
		mGroupNameEditMode = false;
		//描画プロパティ更新
		NVI_UpdateProperty();
		//view bounds更新
		UpdateViewBounds();
	}
	//現在のテキスト内容を適用
	ApplyMarkerTextToDocument();
}

/**
Markerをドキュメントに適用する
*/
void	AWindow_TextMarker::ApplyMarkerTextToDocument()
{
	//テキストボックスからテキスト取得
	AText	text;
	NVI_GetControlText(kControlID_Text,text);
	//マーカーテキスト適用
	GetApp().SPI_SetCurrentTextMarkerData_Text(text);
}

/**
ウインドウの閉じるボタン
*/
void	AWindow_TextMarker::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_TextMarker::NVIDO_Create( const ADocumentID inDocumentID )
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
	
	//選択ボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_SelectGroupButton,pt,11,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_SelectGroupButton,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
	}
	
	//追加ボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_AddGroupButton,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_AddGroupButton,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_AddGroupButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("TextMarkerAdd");
		NVI_GetButtonViewByControlID(kControlID_AddGroupButton).NVI_SetText(text);
	}
	//追加ボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_DeleteGroupButton,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_DeleteGroupButton,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_DeleteGroupButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("TextMarkerRemove");
		NVI_GetButtonViewByControlID(kControlID_DeleteGroupButton).NVI_SetText(text);
	}
	//グループ名変更ボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_ChangeGroupNameButton,pt,12,kHeight_HeaderButton,kControlID_Invalid);
		NVI_SetControlBindings(kControlID_ChangeGroupNameButton,false,true,true,false,true,true);
		NVI_GetButtonViewByControlID(kControlID_ChangeGroupNameButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	text;
		text.SetLocalizedText("TextMarkerEdit");
		NVI_GetButtonViewByControlID(kControlID_ChangeGroupNameButton).NVI_SetText(text);
	}
	
	//SubWindow用タイトルバー生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowTitlebar>	viewFactory(GetObjectID(),kControlID_TitleBar);
		NVM_CreateView(kControlID_TitleBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		AText	text;
		text.SetLocalizedText("SubWindowTitle_TextMarker");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwMarkerPen);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//テキストボックス生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_Text,pt,10,10,kControlID_Invalid,kIndex_Invalid,false,false,false,true,
							  kEditBoxType_Normal);//#1316 kEditBoxType_NoFrameDraw);
		AText	filtertext;
		filtertext.SetLocalizedText("TextMarkerEmptyText");
		NVI_GetEditBoxView(kControlID_Text).SPI_SetTextForEmptyState(filtertext,GetEmptyText());
		NVI_GetEditBoxView(kControlID_Text).SPI_SetEnableFocusRing(false);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetCatchReturn(true);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetAutomaticSelectBySwitchFocus(false);
		NVI_SetControlBindings(kControlID_Text,true,true,true,true,false,false);
	}
	
	//グループ名変更テキストボックス生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_NameEditBox,pt,10,10,kControlID_Invalid,kIndex_Invalid,false,false,false,true,kEditBoxType_Normal);
		NVI_GetEditBoxView(kControlID_NameEditBox).NVI_SetTextFont(fontName,fontSize);
		/*#1316 色はEditBoxクラス側で設定する
		NVI_GetEditBoxView(kControlID_NameEditBox).SPI_SetBackgroundColor(
					kColor_White,kColor_Gray70Percent,kColor_Gray70Percent);
					*/
		NVI_GetEditBoxView(kControlID_NameEditBox).NVI_SetAutomaticSelectBySwitchFocus(false);
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
void	AWindow_TextMarker::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//
	NVI_UpdateProperty();
	//
	UpdateViewBounds();
}

/**
Hide()時等にウインドウのデータを保存する
*/
void	AWindow_TextMarker::NVIDO_Hide()
{
}

/**
*/
void	AWindow_TextMarker::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//view bounds更新
	UpdateViewBounds();
	//サブウインドウ用フォント取得
	AText	fontName;
	AFloatNumber	fontSize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontName,fontSize);
	//マーカーテキストedit boxにα値適用
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		NVI_GetEditBoxView(kControlID_Text).SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
	}
	//現在のグループのマーカーテキスト取得し、edit boxに設定
	AText	title, text;
	GetApp().SPI_GetCurrentTextMarkerData(title,text);
	NVI_GetEditBoxView(kControlID_Text).NVI_SetText(text);
	//edit boxにフォント設定
	NVI_GetEditBoxView(kControlID_Text).NVI_SetTextFont(fontName,fontSize);
	
	//マーカーグループ選択ボタンの設定
	ATextArray	groupTextArray;
	groupTextArray.SetFromTextArray(GetApp().NVI_GetAppPrefDB().GetData_TextArrayRef(AAppPrefDB::kTextMarkerArray_Title));
	ATextArray	actionTextArray;
	for( AIndex i = 0;i < groupTextArray.GetItemCount(); i++ )
	{
		AText	actionText;
		actionText.SetNumber(i);
		actionTextArray.Add(actionText);
	}
	NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).
			SPI_SetMenuTextArray(groupTextArray,actionTextArray,kMenuItemID_TextMarkerSelectGroup);
	NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).NVI_SetText(title);
	
	//削除ボタンのenable/disable（最初のグループは削除不可）
	//if( GetApp().SPI_GetCurrentTextMarkerGroupIndex() == 0 )
	if( GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kTextMarkerArray_Text) <= 1 )
	{
		NVI_SetControlEnable(kControlID_DeleteGroupButton,false);
	}
	else
	{
		NVI_SetControlEnable(kControlID_DeleteGroupButton,true);
	}
	
	//ヘッダ色取得
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	//#1316 色はボタンクラス側で設定する AColor	headerlettercolor = GetApp().SPI_GetSubWindowHeaderLetterColor();
	
	//制御ボタン色設定
	NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_AddGroupButton).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_DeleteGroupButton).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	NVI_GetButtonViewByControlID(kControlID_ChangeGroupNameButton).
			SPI_SetTextProperty(headerfontname,9.0,kJustification_Center,kTextStyle_Bold);//#1316
	//edit box色設定
	NVI_GetEditBoxView(kControlID_Text).NVI_SetTextFont(fontName,fontSize);
	
	/*#1316 色はEditBoxクラス側で設定する
	NVI_GetEditBoxView(kControlID_Text).SPI_SetTextColor(GetApp().SPI_GetSubWindowLetterColor(),GetApp().SPI_GetSubWindowLetterColor());
	NVI_GetEditBoxView(kControlID_Text).SPI_SetBackgroundColor(GetApp().SPI_GetSubWindowBackgroundColor(true),
															   kColor_Gray70Percent,kColor_Gray70Percent);
	NVI_GetEditBoxView(kControlID_Text).SPI_SetBackgroundColorForEmptyState(GetApp().SPI_GetSubWindowBackgroundColor(true),
																			GetApp().SPI_GetSubWindowBackgroundColor(false));
																			*/
	
	//描画更新
	NVI_RefreshWindow();
}

/**
view bounds更新
*/
void	AWindow_TextMarker::UpdateViewBounds()
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
	
	//グループ選択ボタン配置
	{
		NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).SPI_SetWidthToFitTextWidth();
		//ANumber	w = NVI_GetButtonViewByControlID(kControlID_SelectGroupButton).NVI_GetViewWidth();
		AWindowPoint	pt = {2,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_SelectGroupButton,pt);
	}
	
	//グループ名変更text box配置
	if( mGroupNameEditMode == true )
	{
		AWindowPoint	pt = {2,height_Titlebar + 3};
		NVI_SetControlPosition(kControlID_NameEditBox,pt);
		NVI_SetControlSize(kControlID_NameEditBox,listViewWidth - 32,24);
		//
		NVI_SetShowControl(kControlID_NameEditBox,true);
	}
	else
	{
		//
		NVI_SetShowControl(kControlID_NameEditBox,false);
	}
	
	//グループ名変更ボタン配置
	ANumber	wgroupname = 0;
	{
		NVI_GetButtonViewByControlID(kControlID_ChangeGroupNameButton).SPI_SetWidthToFitTextWidth();
		wgroupname = NVI_GetButtonViewByControlID(kControlID_ChangeGroupNameButton).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - wgroupname - 3,height_Titlebar + kHeight_Header1Line + 3};
		NVI_SetControlPosition(kControlID_ChangeGroupNameButton,pt);
	}
	
	//削除ボタン配置
	ANumber	wminus = 0;
	{
		NVI_GetButtonViewByControlID(kControlID_DeleteGroupButton).SPI_SetWidthToFitTextWidth();
		wminus = NVI_GetButtonViewByControlID(kControlID_DeleteGroupButton).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - wminus - wgroupname - 3,height_Titlebar + kHeight_Header1Line + 3};
		NVI_SetControlPosition(kControlID_DeleteGroupButton,pt);
	}
	
	//追加ボタン配置
	ANumber	wplus = 0;
	{
		NVI_GetButtonViewByControlID(kControlID_AddGroupButton).SPI_SetWidthToFitTextWidth();
		wplus = NVI_GetButtonViewByControlID(kControlID_AddGroupButton).NVI_GetViewWidth();
		AWindowPoint	pt = {windowBounds.right-windowBounds.left - wplus - wminus - wgroupname - 3,height_Titlebar + kHeight_Header1Line + 3};
		NVI_SetControlPosition(kControlID_AddGroupButton,pt);
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

