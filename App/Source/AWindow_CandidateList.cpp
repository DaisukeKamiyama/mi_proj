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

AWindow_CandidateList

*/

#include "stdafx.h"

#include "AWindow_CandidateList.h"
#include "AView_CandidateList.h"
#include "AApp.h"
#include "AView_SubWindowSeparator.h"
#include "AView_SubWindowStatusBar.h"
#include "AView_SubWindowSizeBox.h"

//#725 const AFloatNumber	kFontSize = 9.0;

//#725
//List view, scrollbar
const AControlID	kControlID_ListView = 101;
const AControlID	kControlID_VScrollBar = 102;
const ANumber		kVScrollBarWidth = 11;

//下部StatusBar
const AControlID	kControlID_StatusBar = 103;

//検索モード選択メニューボタン
const AControlID	kControlID_SelectSearchMode = 104;
const ANumber		kHeight_SelectSearchModeButton = 15;

//#725
//サイズボックス
const AControlID	kControlID_SizeBox = 903;
const ANumber	kWidth_SizeBox = 15;
const ANumber	kHeight_SizeBox = 15;

//#725
//SubWindow用タイトルバーview
const AControlID	kControlID_TitleBar = 901;

//#725
//SubWindow用セパレータview
const AControlID	kControlID_Separator = 902;

//#725
//背景色描画用view
const AControlID	kControlID_Background = 905;

//list viewの高さ方向余白
const AControlID	kCandidateListHeightMargin = 4;

#pragma mark ===========================
#pragma mark [クラス]AWindow_CandidateList
#pragma mark ===========================
//情報ウインドウのクラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_CandidateList::AWindow_CandidateList():AWindow(/*#175NULL*/), mCurrentModeIndexForAbbrevKeyText(kIndex_Invalid)
{
	//#138 デフォルト範囲(10000～)を使う限り設定不要 NVM_SetDynamicControlIDRange(kDynamicControlIDStart,kDynamicControlIDEnd);
}
//デストラクタ
AWindow_CandidateList::~AWindow_CandidateList()
{
}

/**
ウインドウの通常時（collapseしていない時）の最小高さ取得
*/
ANumber	AWindow_CandidateList::SPI_GetSubWindowMinHeight() const
{
	return GetApp().SPI_GetSubWindowTitleBarHeight() + kCandidateListHeightMargin;
}

//#725
/**
SubWindow用タイトルバー取得
*/
AView_SubWindowTitlebar&	AWindow_CandidateList::GetTitlebarViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_SubWindowTitlebar,kViewType_SubWindowsTitlebar,inControlID);
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

//TextView取得
AView_CandidateList&	AWindow_CandidateList::GetCandidateListView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_CandidateList,kViewType_CandidateList,kControlID_ListView);
	/*#199 
	if( NVI_GetViewByControlID(kControlID_ListView).NVI_GetViewType() != kViewType_CandidateList )   _ACThrow("",this);
	return dynamic_cast<AView_CandidateList&>(NVI_GetViewByControlID(kControlID_ListView));
	*/
}
const AView_CandidateList&	AWindow_CandidateList::GetCandidateListViewConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_CandidateList,kViewType_CandidateList,kControlID_ListView);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//コントロールのクリック時のコールバック
ABool	AWindow_CandidateList::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	return result;
}

//ウインドウサイズ変更通知時のコールバック
void	AWindow_CandidateList::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

/**
ウインドウの閉じるボタン
*/
void	AWindow_CandidateList::EVTDO_DoCloseButton()
{
	GetApp().SPI_CloseSubWindow(GetObjectID());
}


#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

/**
メニュー選択時処理
*/
ABool	AWindow_CandidateList::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//クローズボタン
	  case kMenuItemID_Close:
		{
			GetApp().SPI_CloseSubWindow(GetObjectID());
			result = true;
			break;
		}
		//検索モード 前方一致
	  case kMenuItemID_CandidateSearchMode_Normal:
		{
			GetApp().SPI_SetCandidatePartialSearchMode(false);
			NVI_UpdateProperty();
			break;
		}
		//検索モード 部分一致
	  case kMenuItemID_CandidateSearchMode_Partial:
		{
			GetApp().SPI_SetCandidatePartialSearchMode(true);
			NVI_UpdateProperty();
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
void	AWindow_CandidateList::EVTDO_UpdateMenu()
{
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating));
}

//ウインドウ生成
void	AWindow_CandidateList::NVIDO_Create( const ADocumentID inDocumentID )
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
		//ポップアップ
	  case kSubWindowLocationType_Popup:
		{
			//ウインドウ生成
			//※シャドウはfalse。シャドウを入れると、背景色アルファが薄い場合に、文字にシャドウがついて汚くなる。
			NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
			//サイズ設定
			NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kCandidateWindowWidth));
			NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kCandidateWindowHeight));
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
		text.SetLocalizedText("SubWindowTitle_CandidateList");
		GetTitlebarViewByControlID(kControlID_TitleBar).SPI_SetTitleTextAndIconImageID(text,kImageID_iconSwList);
		NVI_SetShowControl(kControlID_TitleBar,true);
		NVI_SetControlBindings(kControlID_TitleBar,true,true,true,false,false,true);
	}
	
	//下部テキストパネル生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowStatusBar>	viewFactory(GetObjectID(),kControlID_StatusBar);
		NVM_CreateView(kControlID_StatusBar,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_StatusBar,true,false,true,true,false,true);
	}
	
	//検索モード切替ボタン生成
	{
		AWindowPoint	pt = {0,0};
		NVI_CreateButtonView(kControlID_SelectSearchMode,pt,10,kHeight_SelectSearchModeButton,kControlID_StatusBar);
		NVI_SetControlBindings(kControlID_SelectSearchMode,false,false,true,true,true,true);
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).SPI_SetEllipsisMode(kEllipsisMode_FixedFirstCharacters,10);
		//
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).SPI_SetMenu(kContextMenuID_CandidateSearchMode,kMenuItemID_Invalid);
	}
	
	//SubWindow用セパレータ生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSeparator>	viewFactory(GetObjectID(),kControlID_Separator);
		NVM_CreateView(kControlID_Separator,pt,10,10,kControlID_StatusBar,kControlID_Invalid,false,viewFactory);
		NVI_SetShowControl(kControlID_Separator,true);
		NVI_SetControlBindings(kControlID_Separator,true,false,true,true,false,true);
	}
	
	//SizeBox生成
	{
		AWindowPoint	pt = {0,0};
		AViewDefaultFactory<AView_SubWindowSizeBox>	viewFactory(GetObjectID(),kControlID_SizeBox);
		NVM_CreateView(kControlID_SizeBox,pt,10,10,kControlID_StatusBar,kControlID_Invalid,false,viewFactory);
		NVI_SetControlBindings(kControlID_SizeBox,false,false,true,true,true,true);
	}
}

//新規タブ生成
//引数：生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
void	AWindow_CandidateList::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//listview生成
	{
		AWindowPoint	pt = {0,0}; 
		AViewDefaultFactory<AView_CandidateList>	candidateListViewFactory(GetObjectID(),kControlID_ListView);//#199
		NVM_CreateView(kControlID_ListView,pt,500,500,kControlID_Invalid,kControlID_Invalid,false,candidateListViewFactory);
		NVI_SetControlBindings(kControlID_ListView,true,true,true,true,false,false);
	}
	
	//Vスクロールバー生成
	{
		AWindowPoint	pt = {0,0}; 
		NVI_CreateScrollBar(kControlID_VScrollBar,pt,kVScrollBarWidth,kVScrollBarWidth*2);
		GetCandidateListView().NVI_SetScrollBarControlID(kControlID_Invalid,kControlID_VScrollBar);
		NVI_SetControlBindings(kControlID_VScrollBar,false,true,true,true,true,false);
	}
	//
	UpdateViewBounds();
	//windows用
	NVI_RegisterToFocusGroup(kControlID_ListView);
	NVI_SwitchFocusTo(kControlID_ListView);
}

//Hide()時等にウインドウのデータを保存する
void	AWindow_CandidateList::NVIDO_Hide()
{
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Popup )
	{
		//ウインドウ幅の保存
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kCandidateWindowWidth,NVI_GetWindowWidth());
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kCandidateWindowHeight,NVI_GetWindowHeight());
	}
}

//
void	AWindow_CandidateList::NVIDO_UpdateProperty()
{
	if( NVI_IsWindowCreated() == false )   return;
	//#725 NVI_SetWindowTransparency(/*(static_cast<AFloatNumber>(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIdInfoWindowAlphaPercent)))/100*/
	//#725		0.9);
	
	//サブウインドウ用背景色
	AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(true);
	//下部テキストパネル色等設定
	NVI_GetViewByControlID(kControlID_StatusBar).NVI_SetBackgroundColor(
				GetApp().SPI_GetSubWindowBackgroundColor(true),
				GetApp().SPI_GetSubWindowBackgroundColor(false));
	NVI_GetViewByControlID(kControlID_StatusBar).NVI_SetTextProperty(kColor_Black,kTextStyle_Normal,kJustification_Left);
	//サイズボックス色設定
	NVI_GetViewByControlID(kControlID_SizeBox).NVI_SetColor(backgroundColor);
	//list viewと下部テキストパネルにα値設定
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Floating )
	{
		GetCandidateListView().SPI_SetTransparency(GetApp().SPI_GetFloatingWindowAlpha());
		NVI_GetViewByControlID(kControlID_StatusBar).NVI_SetBackgroundTransparency(GetApp().SPI_GetFloatingWindowAlpha());
	}
	if( GetApp().SPI_GetSubWindowLocationType(GetObjectID()) == kSubWindowLocationType_Popup )
	{
		GetCandidateListView().SPI_SetTransparency(GetApp().SPI_GetPopupWindowAlpha());
		NVI_GetViewByControlID(kControlID_StatusBar).NVI_SetBackgroundTransparency(GetApp().SPI_GetPopupWindowAlpha());
	}
	//下部テキストパネルのテキスト設定
	SPI_UpdateStatusBar();
	//検索モードメニューボタンのテキスト設定
	{
		AText	text;
		if( GetApp().SPI_IsCandidatePartialSearchMode() == false )
		{
			text.SetLocalizedText("CandidateList_SearchMode_Normal");
		}
		else
		{
			text.SetLocalizedText("CandidateList_SearchMode_Partial");
		}
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).NVI_SetText(text);
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).SPI_SetWidthToFitTextWidth();
		
		//サブウインドウ用フォント取得
		AText	fontname;
		AFloatNumber	fontsize = 9.0;
		GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
		//描画色設定
		AColor	letterColor = kColor_Black;
		AColor	dropShadowColor = kColor_White;
		AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
		GetApp().SPI_GetSubWindowBoxColor(GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
		NVI_GetButtonViewByControlID(kControlID_SelectSearchMode).SPI_SetTextProperty(fontname,9.0,kJustification_Left,
																					  kTextStyle_Normal,letterColor,letterColor);
	}
	//view bounds更新
	UpdateViewBounds();
	//描画更新
	NVI_RefreshWindow();
}

/**
下部ステータスバーの表示更新
*/
void	AWindow_CandidateList::SPI_UpdateStatusBar()
{
	//候補数
	AText	text;
	text.SetLocalizedText("CandidateList_Count");
	text.ReplaceParamText('0',GetCandidateListView().SPI_GetCandidateCount());
	if( GetCandidateListView().SPI_GetCandidateCount() >= kLimit_CandidateListItemCount )
	{
		AText	t;
		t.SetLocalizedText("CandidateList_MaxItems");
		text.AddText(t);
	}
	text.AddCstring("   ");
	//補完キー
	if( GetCandidateListView().SPI_GetIndex() == kIndex_Invalid )
	{
		//------------------補完入力キー入力前------------------
		AText	t;
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAbbrevInputByArrowKeyAfterPopup) == true )
		{
			//------------------上下キーで補完入力可能の場合------------------
			//「^1キーまたは↑↓キーで補完入力」
			t.SetLocalizedText("CandidateList_Key_BeforeAbbrevMode_ArrowKeyAvailable");
		}
		else
		{
			//------------------補完キーのみで補完入力可能の場合------------------
			//「^1キーで補完入力」
			t.SetLocalizedText("CandidateList_Key_BeforeAbbrevMode");
		}
		t.ReplaceParamText('1',mAbbrevKeyText);
		text.AddText(t);
	}
	else
	{
		//------------------補完入力キー入力後------------------
		//「"↑↓キーで候補選択、return キーで確定、esc キーでキャンセル」
		AText	t;
		t.SetLocalizedText("CandidateList_Key_AfterAbbrevMode");
		text.AddText(t);
	}
	NVI_GetViewByControlID(kControlID_StatusBar).NVI_SetText(text);
}

//
void	AWindow_CandidateList::SPI_SetInfo( const AModeIndex inModeIndex,
		const AWindowID inTextWindowID, const AControlID inTextViewControlID, 
		//#717 const AGlobalPoint inPoint, const ANumber inLineHeight, 
		const ATextArray& inIdTextArray, const ATextArray& inInfoTextArray,
		const AArray<AIndex>& inCategoryIndexArray, const AArray<AScopeType>& inScopeTypeArray, //#717 const AObjectArray<AFileAcc>& inFileArray )
		const ATextArray& inFilePathArray, const AArray<ACandidatePriority>& inPriorityArray,
		const ATextArray& inParentKeywordArray,
		const AArray<AItemCount>& inMatchedCountArray )
{
	/*#717 ウインドウ位置は、コール元のほうで設定する
	ANumber	h = NVI_GetWindowHeight();
	AGlobalRect	screenbounds;
	AWindow::NVI_GetMainDeviceScreenBounds(screenbounds);
	APoint	pt;
	pt.x = inPoint.x;
	pt.y = inPoint.y + inLineHeight + 32;//タイトルバー分だけ多め
	if( pt.y + h > screenbounds.bottom )
	{
		pt.y = inPoint.y - 16- h;
	}
	NVI_SetWindowPosition(pt);
	*/
	GetCandidateListView().SPI_SetInfo(inModeIndex,inTextWindowID,inTextViewControlID,
				inIdTextArray,inInfoTextArray,inCategoryIndexArray,inScopeTypeArray,inFilePathArray,inPriorityArray,
				inParentKeywordArray,
				inMatchedCountArray);//#717 inFileArray);
	//
	NVI_UpdateProperty();
	/*#1160
	//候補キーのキーバインドテキスト取得
	switch(GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kAbbrevNextKey))
	{
	  case kAbbrevKeyType_ShiftSpace:
		{
			mAbbrevKeyText.SetLocalizedText("AbbrevKeyType1");
			break;
		}
	  case kAbbrevKeyType_ShiftTab:
		{
			mAbbrevKeyText.SetLocalizedText("AbbrevKeyType2");
			break;
		}
	  case kAbbrevKeyType_Tab:
		{
			mAbbrevKeyText.SetLocalizedText("AbbrevKeyType3");
			break;
		}
	  case kAbbrevKeyType_Escape:
		{
			mAbbrevKeyText.SetLocalizedText("AbbrevKeyType4");
			break;
		}
	  case kAbbrevKeyType_ShiftEscape:
		{
			mAbbrevKeyText.SetLocalizedText("AbbrevKeyType5");
			break;
		}
	  case kAbbrevKeyType_KeyBind:
		{
			//モードが変換した場合のみ、補完キー取得（GetKeyTextFromKeybindAction()はけっこう時間がかかる）
			if( mCurrentModeIndexForAbbrevKeyText != inModeIndex && inModeIndex != kIndex_Invalid )
			{
				GetApp().SPI_GetModePrefDB(inModeIndex).GetKeyTextFromKeybindAction(keyAction_abbrevnext,mAbbrevKeyText);
				//キーバインド割当が無いときは?と表示（未割当を明確にするため）
				if( mAbbrevKeyText.GetItemCount() == 0 )
				{
					mAbbrevKeyText.SetCstring("?");
				}
			}
			break;
		}
		mCurrentModeIndexForAbbrevKeyText = inModeIndex;
	}
	*/
}

//
void	AWindow_CandidateList::SPI_SetIndex( const AIndex inIndex )
{
	GetCandidateListView().SPI_SetIndex(inIndex);
}

/*:
ウインドウの最大高さ取得（ポップアップ時のウインドウ高さ調整のため）
*/
ANumber	AWindow_CandidateList::SPI_GetMaxWindowHeight() const
{
	return NVI_GetViewConstByControlID(kControlID_ListView).NVI_GetImageHeight() + kHeight_CandidateListStatusBar + kCandidateListHeightMargin;
}

//
void	AWindow_CandidateList::UpdateViewBounds()
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
	ANumber	topMargin = 0;
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
	  case kSubWindowLocationType_Floating:
		{
			leftMargin = 0;
			rightMargin = 0;
			bottomMargin = 0;
			break;
		}
	  default:
		{
			//ポップアップの場合
			leftMargin = 0;
			rightMargin = 0;
			bottomMargin = 0;
			backgroundPt.y = 0;
			topMargin = 1;
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
	ABool	showBackground = false;
	//list viewサイズ
	AWindowPoint	listViewPoint = {leftMargin,topMargin};
	ANumber	listViewWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	ANumber	listViewHeight = windowBounds.bottom - windowBounds.top -bottomMargin -topMargin - kHeight_CandidateListStatusBar;
	//vscrollbar幅
	ANumber	vscrollbarWidth = kVScrollBarWidth;
	//右サイドバーの最終項目の場合、かつ、ステータスバー未表示時以外はサイズボックスの対象は自ウインドウ
	NVI_SetSizeBoxTargetWindowID(GetObjectID());
	//レイアウト計算
	switch(GetApp().SPI_GetSubWindowLocationType(GetObjectID()))
	{
		//ポップアップの場合
	  case kSubWindowLocationType_Popup:
		{
			//background viewにrounded rectを表示するので、その内部におさまるように、list viewを少し小さくする
			showBackground = true;
			listViewPoint.y += 1;//#1079 3;
			listViewPoint.x += 1;
			listViewWidth -= 1;//#1079 2;
			listViewHeight -= 1;//#1079 2;
			//scroll barは表示しない
			vscrollbarWidth = 0;
			//サイズボックスは表示する
			showSizeBox = true;
			break;
		}
		//サイドバーの場合
	  case kSubWindowLocationType_LeftSideBar:
	  case kSubWindowLocationType_RightSideBar:
		{
			//タイトルバー、セパレータ表示フラグ
			showTitleBar = true;
			showSeparator = true;
			showBackground = true;
			//list viewサイズ
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;// + height_Separator;
			/*
			if( isSideBarBottom == true )
			{
				showSeparator = false;
				//右サイドバーの最終項目の場合、かつ、ステータスバー未表示時はテキストウインドウサイズ変更用のサイズボックスを表示する
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
			showBackground = true;
			//list viewサイズ
			listViewPoint.y += height_Titlebar;
			listViewHeight -= height_Titlebar;
			//
			showSizeBox = true;
			break;
		}
	}
	//
	if( listViewHeight < 0 )   listViewHeight = 0;
	AWindowPoint	footerPoint = {leftMargin,listViewPoint.y+listViewHeight};
	ANumber	footerWidth = windowBounds.right - windowBounds.left - leftMargin - rightMargin;
	
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
		NVI_SetControlPosition(kControlID_ListView,listViewPoint);
		NVI_SetControlSize(kControlID_ListView,listViewWidth - vscrollbarWidth,listViewHeight-3);
		NVI_SetShowControl(kControlID_ListView,true);
	}
	
	//Vスクロールバー配置
	if( vscrollbarWidth > 0 )
	{
		AWindowPoint	pt = {listViewPoint.x + listViewWidth - vscrollbarWidth,listViewPoint.y};
		NVI_SetControlPosition(kControlID_VScrollBar,pt);
		NVI_SetControlSize(kControlID_VScrollBar,vscrollbarWidth,listViewHeight);
		NVI_SetShowControl(kControlID_VScrollBar,true);
	}
	else
	{
		NVI_SetShowControl(kControlID_VScrollBar,false);
	}
	
	//テキストパネル配置・表示
	{
		NVI_SetControlPosition(kControlID_StatusBar,footerPoint);
		NVI_SetControlSize(kControlID_StatusBar,footerWidth,kHeight_CandidateListStatusBar);
		NVI_SetShowControl(kControlID_StatusBar,true);
	}
	
	//検索モードボタン
	{
		AWindowPoint	pt = {0};
		pt.x = windowBounds.right-windowBounds.left - NVI_GetControlWidth(kControlID_SelectSearchMode) - kWidth_SizeBox - 3;
		pt.y = windowBounds.bottom-windowBounds.top - kHeight_SelectSearchModeButton - 1;
		NVI_SetControlPosition(kControlID_SelectSearchMode,pt);
		NVI_SetShowControl(kControlID_SelectSearchMode,true);
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
	GetCandidateListView().SPI_UpdateDrawProperty();
}

