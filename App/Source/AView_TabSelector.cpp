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

AView_TabSelector

*/

#include "stdafx.h"

#include "AView_TabSelector.h"
#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AView_TabSelector
#pragma mark ===========================
//タブ選択ビュー

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_TabSelector::AView_TabSelector( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) 
: AView(/*#199 inParent,inWindow*/inWindowID,inID), mTabRowHeight(21), mTabColumnWidth(160), mFontSize(10.5), mHoverIndex(kIndex_Invalid)//R0177, B0000 080810
//#164 ,mCurrentCloseButtonIndex(kIndex_Invalid), mEnableTabCloseButton(false)//R0228
,mMouseTrackResultIsDrag(false),mMouseDown(false)//#688 mMouseTrackByLoop(true)//#364
//#850 ,mDragMode(false),mDragCurrentIndex(kIndex_Invalid),mDragOriginalDisplayIndex(kIndex_Invalid)
,mMouseTrackStartLocalPoint(kLocalPoint_00)//#364
,mHoverCloseButton(false)//#164
,mDragging(false),mDraggingCurrentDisplayIndex(kIndex_Invalid),mDraggingTabShouldHide(false)//#575 #850
,mMouseClickCount(0)
{
	NVMC_SetOffscreenMode(true);//win
	//#92 初期化はNVIDO_Init()へ移動
	
	//viewを不透明に設定 #1090
	NVMC_SetOpaque(true);
}

//デストラクタ
AView_TabSelector::~AView_TabSelector()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_TabSelector::NVIDO_Init()
{
	SPI_UpdateProperty();//R0177
	//B0000 080810
	NVMC_EnableMouseLeaveEvent();
	/*#164
	//R0228
	AWindowPoint	pt = {0,0};
	NVM_GetWindow().NVI_CreateButtonView(kTabCloseButtonControlID,pt,16,16);
	NVM_GetWindow().NVI_GetButtonViewByControlID(kTabCloseButtonControlID).SPI_SetIcon(kIconID_SplitView);//#182
	NVM_GetWindow().NVI_SetShowControl(kTabCloseButtonControlID,false);//#284
	*/
	//win
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_TabSelector);
	scrapTypeArray.Add(kScrapType_File);//#575
	NVMC_EnableDrop(scrapTypeArray);
	/*#688
	//#364
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_TabSelector::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//マウスボタン押下時のコールバック(AView用)
ABool	AView_TabSelector::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//
	mMouseDown = true;//#364
	mMouseTrackStartLocalPoint = inLocalPoint;//#364
	
	AIndex	col = inLocalPoint.x / mTabColumnWidth;
	AIndex	row = inLocalPoint.y / mTabRowHeight;
	//#498 AIndex	index = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	AIndex	displayIndex = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	
	//R0225 タブダブルクリックで新規ドキュメント
	if( displayIndex >= /*#498 NVM_GetWindow().NVI_GetTabCount()*/mDisplayOrderArray.GetItemCount() || 
				col >= GetTabCountInOneRow(NVI_GetViewWidth()) ) 
	{
		if( inClickCount == 2 )
		{
			AText	name;
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kCmdNModeName,name);
			AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(name);
			if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
			GetApp().SPNVI_CreateNewTextDocument(modeIndex);
		}
		return false;
	}
	
	//#364 タブ順序入れ替え
	//ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
	//#688 if( mMouseTrackByLoop == false )
	//#688{
	//TrackingMode設定
	mMouseTrackResultIsDrag = false;
	mMouseClickCount = inClickCount;
	NVM_SetMouseTrackingMode(true);
	NVI_Refresh();
	return true;
	//#688}
	/*#688
	//Drag開始判定処理
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	mMouseTrackResultIsDrag = false;
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//タブ範囲取得
		ALocalRect	tabrect = {0,0,0,0};
		GetTabRect(mMouseTrackStartLocalPoint,tabrect);
		//Drag開始判定処理
		ALocalPoint	mouseLocalPoint;
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		if(	mouseLocalPoint.x >= tabrect.left && mouseLocalPoint.x <= tabrect.right &&
			mouseLocalPoint.y >= tabrect.top && mouseLocalPoint.y <= tabrect.bottom )
		{
			//タブ内のまま
			mMouseDown = true;
		}
		else
		{
			//タブ外へ移動
			mMouseDown = false;
			//Drag開始
			mMouseTrackResultIsDrag = true;
			NVI_Refresh();
			break;
		}
		//#662 NVI_Refresh();
	}
	if( mMouseTrackResultIsDrag == true )
	{
		//Drag開始
		StartDrag(inLocalPoint,inModifierKeys);
		return true;
	}
	MouseClick(inLocalPoint);//#164
	return true;//#164 false->true（trueにしないとEVTDO_Click()がこの後実行される。Tabクローズ後に実行すべきではない）
	*/
}

//#164
/**
マウスクリック（マウスダウン→マウスアップ）時処理
*/
void	AView_TabSelector::MouseClick( const ALocalPoint& inLocalPoint )
{
	AIndex	col = inLocalPoint.x / mTabColumnWidth;
	AIndex	row = inLocalPoint.y / mTabRowHeight;
	AIndex	displayIndex = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	if( displayIndex < 0 || displayIndex >= mDisplayOrderArray.GetItemCount() )
	{
		return;
	}
	AIndex	index = mDisplayOrderArray.Get(displayIndex);
	//
	ALocalRect	closeButtonRect = {0,0,0,0};
	GetCloseButtonRect(displayIndex,closeButtonRect);
	if( //#844 GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowTabCloseButton) == true &&
				mMouseTrackStartLocalPoint.x > closeButtonRect.left && mMouseTrackStartLocalPoint.x < closeButtonRect.right &&
				mMouseTrackStartLocalPoint.y > closeButtonRect.top && mMouseTrackStartLocalPoint.y < closeButtonRect.bottom &&
				inLocalPoint.x > closeButtonRect.left && inLocalPoint.x < closeButtonRect.right &&
				inLocalPoint.y > closeButtonRect.top && inLocalPoint.y < closeButtonRect.bottom )
	{
		//マウスボタンダウン・アップ箇所がクローズボタン内のとき、タブを閉じる
		NVM_GetWindow().NVI_TryClose(index);
	}
	else
	{
		//Tab選択処理
		NVM_GetWindow().NVI_SelectTab(index);
		//ダブルクリック時はフラグを付ける #832
		if( mMouseClickCount == 2 )
		{
			ADocumentID	tabDocID = NVM_GetWindow().NVI_GetDocumentIDByTabIndex(index);
			GetApp().SPI_GetTextDocumentByID(tabDocID).
					SPI_SetDocumentFlag(!GetApp().SPI_GetTextDocumentByID(tabDocID).SPI_GetDocumentFlag());
		}
	}
}

//#364
/**
Drag開始
*/
void	AView_TabSelector::StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mMouseTrackResultIsDrag == true )
	{
		//表示Index取得
		AIndex	col = inLocalPoint.x / mTabColumnWidth;
		AIndex	row = inLocalPoint.y / mTabRowHeight;
		//#850 mDragOriginalDisplayIndex = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
		mDraggingCurrentDisplayIndex = col + row*GetTabCountInOneRow(NVI_GetViewWidth());//#850
		AIndex	tabIndex = mDisplayOrderArray.Get(mDraggingCurrentDisplayIndex);
		
		//Tab範囲取得
		ALocalRect	rect = {0,0,0,0};
		GetTabRect(inLocalPoint,rect);
		
		//Drag領域設定
		AArray<ALocalRect>	dragRect;
		dragRect.Add(rect);
		//Scrap設定（表示Indexを渡す）
		AArray<AScrapType>	scrapType;
		ATextArray	data;
		scrapType.Add(kScrapType_TabSelector);
		//#859 data.Add(GetEmptyText());
		//drag データにはドキュメントIDを設定
		AText	tabIndexText;
		ADocumentID	docID = NVM_GetWindow().NVI_GetDocumentIDByTabIndex(tabIndex);
		tabIndexText.SetNumber(GetApp().NVI_GetDocumentUniqID(docID).val);//#850 mDragOriginalDisplayIndex);
		data.Add(tabIndexText);
		//Drag実行
		mDragging = true;//#575
		mDraggingTabShouldHide = false;//#850
		//
		AWindowRect	wrect = {0};
		NVM_GetWindow().NVI_GetWindowRectFromControlLocalRect(NVI_GetControlID(),rect,wrect);
		if( MVMC_DragTextWithWindowImage(inLocalPoint,scrapType,data,wrect,wrect.right-wrect.left,wrect.bottom-wrect.top) == false )
		{
			//マウス位置取得
			AGlobalPoint	mouseGlobalPoint = {0};
			AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
			APoint	pt = {mouseGlobalPoint.x,mouseGlobalPoint.y};
			//別viewにdropされなかったらウインドウ化する//★drag中にウインドウ表示に切り替わるようにしたい
			AWindowID	thisWinID = NVM_GetWindow().GetObjectID();
			GetApp().SPI_GetTextWindowByID(thisWinID).SPI_TabToWindow(tabIndex,true,pt);
		}
		mDragging = false;//#575
		mDraggingCurrentDisplayIndex = kIndex_Invalid;//#850
		mDraggingTabShouldHide = false;//#850
	}
}

//#364
/**
タブ範囲取得
*/
void	AView_TabSelector::GetTabRect( const ALocalPoint& inLocalPoint, ALocalRect& outTabRect )
{
	//行／列取得
	AIndex	col = inLocalPoint.x / mTabColumnWidth;
	AIndex	row = inLocalPoint.y / mTabRowHeight;
	//範囲計算
	outTabRect.left = col*mTabColumnWidth;
	outTabRect.right = outTabRect.left + mTabColumnWidth;
	outTabRect.top = row*mTabRowHeight;
	outTabRect.bottom = outTabRect.top + mTabRowHeight;
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
/**
マウスボタン押下中TrackingのOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_TabSelector::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//Drag開始判定
	/*#850
	ALocalRect	tabrect = {0,0,0,0};
	GetTabRect(mMouseTrackStartLocalPoint,tabrect);
	if( inLocalPoint.x >= tabrect.left && inLocalPoint.x <= tabrect.right &&
	inLocalPoint.y >= tabrect.top && inLocalPoint.y <= tabrect.bottom )
	*/
	if( IsDragStart(mMouseTrackStartLocalPoint,inLocalPoint) == false )
	{
		//タブ内
		mMouseDown = true;
	}
	else
	{
		//タブ外へ移動
		mMouseDown = false;
		//
		mMouseTrackResultIsDrag = true;
		//ドラッグ開始
		StartDrag(mMouseTrackStartLocalPoint,inModifierKeys);
		//TrackingMode解除
		NVM_SetMouseTrackingMode(false);
	}
	NVI_Refresh();
}

/**
マウスボタン押下中Tracking終了時のOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_TabSelector::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	NVM_SetMouseTrackingMode(false);
	//#379
	if( mMouseTrackResultIsDrag == false )
	{
		MouseClick(inLocalPoint);//#164
	}
	//クローズボタンクリック時にホバー描画を更新するため、moved処理する。
	EVTDO_DoMouseMoved(inLocalPoint,inModifierKeys);
}
//#688 #endif

//マウスボタン押下時のコールバック(AView用)
ABool	AView_TabSelector::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	AIndex	col = inLocalPoint.x / mTabColumnWidth;
	AIndex	row = inLocalPoint.y / mTabRowHeight;
	//#498 AIndex	index = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	AIndex	displayIndex = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	if( displayIndex >= mDisplayOrderArray.GetItemCount() )   return true;
	AIndex	index = mDisplayOrderArray.Get(displayIndex);
	if( index >= NVM_GetWindow().NVI_GetTabCount() )   return true;
	mContextMenuSelectedTabIndex = index;
	//#688 GetApp().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_Tab,globalPoint,NVM_GetWindow().NVI_GetWindowRef());//win 080712
	NVMC_ShowContextMenu(kContextMenuID_Tab,globalPoint);//#688
	return true;
}

//ボタン上余白
const ANumber	kButtonTopMargin = 0;

//描画要求時のコールバック(AView用)
void	AView_TabSelector::EVTDO_DoDraw()
{
	//NSLog(@"AView_TabSelector::EVTDO_DoDraw");
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	
	//×ボタン用文字列
	AText	closetext;
	closetext.SetLocalizedText("PopupClose");
	//×ボタン用文字フォント設定
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);
	NVMC_SetDefaultTextProperty(defaultfontname,16.0,kColor_Black,kTextStyle_Normal,true);
	//×ボタン用表示幅取得
	ANumber	closeTextWidth = NVMC_GetDrawTextWidth(closetext);
	
	//アクティブ時色
	//背景
	AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("D8D8D8");
	//枠線
	AColor	tabFrameColor = AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
	//1番目タブ
	AColor	firstTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
	AColor	firstTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
	AColor	firstTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
	//2番目タブ
	AColor	secondTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
	AColor	secondTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
	AColor	secondTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
	//3番目以降タブ
	AColor	thirdTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("E8E8E8");
	AColor	thirdTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("333333");
	AColor	thirdTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("EDEDED");
	//タブ領域境界線
	AColor	boaderColor = AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
	//クローズボタン
	AColor	closeButtonColor = kColor_Black;
	//非アクティブ時色
	if( NVI_GetWindow().NVI_IsWindowActive() == false )
	{
		//背景
		backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
		//枠線
		tabFrameColor = AColorWrapper::GetColorByHTMLFormatColor("E6E6E6");
		//1番目タブ
		firstTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
		firstTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("B2B2B2");
		//2番目タブ
		secondTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
		secondTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("B2B2B2");
		//3番目タブ
		thirdTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
		thirdTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("B2B2B2");
		//タブ領域境界線
		boaderColor = AColorWrapper::GetColorByHTMLFormatColor("E6E6E6");
	}
	//ダークモード #1316
	if( AApplication::GetApplication().NVI_IsDarkMode() == true )
	{
		//背景
		backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("222222");
		//枠線
		tabFrameColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
		//1番目タブ
		firstTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
		firstTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
		firstTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("383838");
		//２番目タブ
		secondTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
		secondTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("909090");
		secondTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("383838");
		//3番目タブ
		thirdTabPaintColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
		thirdTabLetterColor = AColorWrapper::GetColorByHTMLFormatColor("707070");
		thirdTabPaintColor_Hover = AColorWrapper::GetColorByHTMLFormatColor("383838");
		//タブ領域境界線
		boaderColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
		//クローズボタン
		closeButtonColor = kColor_White;
	}
	
	//=====================タブエリア背景描画=====================
	ALocalRect	r = viewRect;
	r.top -= 1;
	NVMC_PaintRect(r,backgroundColor);
	
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(NVI_GetViewWidth());
	//１つのタブ毎の表示
	for( AIndex index = 0; index < NVM_GetWindow().NVI_GetTabCount(); index++ )
	{
		//=====================各タブ描画=====================
		ADocumentID	tabDocID = NVM_GetWindow().NVI_GetDocumentIDByTabIndex(index);
		
		//表示index, 行、列取得
		AIndex	displayIndex = mDisplayOrderArray.Find(index);
		if( displayIndex == kIndex_Invalid )   continue;
		
		//#850
		//ドラッグ中のタブはボタン非表示にする
		if( mDragging == true && mDraggingTabShouldHide == true && displayIndex == mDraggingCurrentDisplayIndex )
		{
			continue;
		}
		
		//
		AIndex	column = displayIndex%tabCountInOneRow;
		AIndex	row = displayIndex/tabCountInOneRow;
		
		//表示rect取得
		ALocalRect	rect = viewRect;
		rect.left = column*mTabColumnWidth;
		rect.right = rect.left + mTabColumnWidth;
		rect.top = row*mTabRowHeight;
		rect.bottom = rect.top + mTabRowHeight;
		
		//フラット塗りつぶし用rect #1079
		ALocalRect	r = rect;
		r.top -= 1;
		if( column != 0 )
		{
			r.left -= 1;
		}
		//×ボタン用文字フォント設定
		AText	defaultfontname;
		AFontWrapper::GetDialogDefaultFontName(defaultfontname);
		NVMC_SetDefaultTextProperty(defaultfontname,16.0,closeButtonColor,kTextStyle_Normal,true);
		//×ボタン用rect ★ topをマイナスにしないと位置が合わない・・・
		ALocalRect	closetextrect = rect;
		closetextrect.top -= 2;
		closetextrect.left += 5;
		closetextrect.bottom -= 2;
		
		//×ボタン囲みrect
		ALocalRect	closeHoverRect = rect;
		closeHoverRect.left += 3;
		closeHoverRect.top += 3;
		closeHoverRect.bottom -= 3;
		closeHoverRect.right = closeHoverRect.left + closeTextWidth + 6;
		
		//=====================各タブボタンイメージ描画=====================
		//
		AColor	tabPaintColor = thirdTabPaintColor;
		AColor	letterColor = thirdTabLetterColor;
		AColor	tabPaintColor_Hover = thirdTabPaintColor_Hover;
		//
		if( NVM_GetWindow().NVI_GetCurrentTabIndex() == index )
		{
			tabPaintColor = firstTabPaintColor;
			letterColor = firstTabLetterColor;
			tabPaintColor_Hover = firstTabPaintColor_Hover;
		}
		else if( NVM_GetWindow().NVI_GetTabCount() >= 2 )
		{
			if( NVM_GetWindow().NVI_GetTabIndexByZOrderIndex(1) == index )
			{
				tabPaintColor = secondTabPaintColor;
				letterColor = secondTabLetterColor;
				tabPaintColor_Hover = secondTabPaintColor_Hover;
			}
		}
		// -------------------- タブimage描画 --------------------
		ALocalPoint	pt = {rect.left,rect.top+kButtonTopMargin};
		if( mHoverIndex != kIndex_Invalid && index == mHoverIndex )
		{
			//ホバー
			NVMC_PaintRect(r,tabPaintColor_Hover);
			//×ボタン囲みrounded rect描画
			if( mHoverCloseButton == true )
			{
				NVMC_PaintRoundedRect(closeHoverRect,closeButtonColor,closeButtonColor,kGradientType_None,
									  0.1,0.1,2.0,true,true,true,true);
			}
			//×ボタン描画
			NVMC_DrawText(closetextrect,closetext);
		}
		else
		{
			//通常
			NVMC_PaintRect(r,tabPaintColor);
		}
		//タブ枠線描画
		NVMC_FrameRect(r,tabFrameColor);
		
		//=====================テキスト描画=====================
		ATextStyle	style = 0;
		//文字色取得
		NVMC_SetDefaultTextProperty(mFontName,mFontSize,letterColor,style,true);
		ALocalRect	textRect = rect;
		textRect.left += kTextLeftMargin;
		textRect.right -= kTextRightMargin;
		textRect.bottom -= kTextBottomMargin;
		textRect.bottom += 1;
		textRect.top = textRect.bottom-mFontHeight;
		AText	title;
		NVM_GetWindow().NVI_GetTitle(index,title);
		//親フォルダ名も表示 #1334
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kTabShowParentFolder) == true )
		{
			AText	fileurl;
			GetApp().SPI_GetTextDocumentByID(tabDocID).SPI_GetURL(fileurl);
			ATextArray	fileurlArray;
			fileurl.Explode('/', fileurlArray);
			if( fileurlArray.GetItemCount() >= 2 )
			{
				AText	folderName;
				fileurlArray.Get(fileurlArray.GetItemCount()-2, folderName);
				title.InsertCstring(0,"/");
				title.InsertText(0,folderName);
			}
		}
		//
		AText	ellipsisTitle;
		NVI_GetEllipsisTextWithFixedLastCharacters(title,textRect.right-textRect.left-8,5,ellipsisTitle);
		//
		NVMC_DrawText(textRect,ellipsisTitle,letterColor,style,kJustification_Center);
		
		if( GetApp().NVI_GetDocumentByID(tabDocID).NVI_IsDirty() == true )
		{
			//
			ANumber	textWidth = NVMC_GetDrawTextWidth(ellipsisTitle);
			//変更有りドット表示
			ALocalPoint	pt = {(textRect.left+textRect.right)/2 + textWidth/2 + 3, textRect.top};
			NVMC_DrawImage(kImageID_tabDot,pt);
		}
		
		//フラグ有り表示 #832
		if( GetApp().SPI_GetTextDocumentByID(tabDocID).SPI_GetDocumentFlag() == true )
		{
			//テキスト表示幅取得
			ANumber	textWidth = NVMC_GetDrawTextWidth(ellipsisTitle);
			//フラグアイコン表示
			ALocalPoint	pt = {(textRect.left+textRect.right)/2 - textWidth/2 - 15, textRect.top};
			NVMC_DrawImage(kImageID_iconSwlFlag,pt);
		}
	}
	//分割線
	ALocalRect	borderRect = viewRect;
	borderRect.top = borderRect.bottom -1;
	NVMC_FrameRect(borderRect,boaderColor);
}

//B0000 080810
//カーソル
ABool	AView_TabSelector::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(NVI_GetViewWidth());
	for( AIndex index = 0; index < NVM_GetWindow().NVI_GetTabCount(); index++ )
	{
		/*#498
		AIndex	column = index%tabCountInOneRow;
		AIndex	row = index/tabCountInOneRow;
		*/
		AIndex	displayIndex = mDisplayOrderArray.Find(index);
		if( displayIndex == kIndex_Invalid )   continue;
		AIndex	column = displayIndex%tabCountInOneRow;
		AIndex	row = displayIndex/tabCountInOneRow;
		//
		ALocalRect	rect = viewRect;
		rect.left = column*mTabColumnWidth;
		rect.right = rect.left + mTabColumnWidth;
		rect.top = row*mTabRowHeight;
		rect.bottom = rect.top + mTabRowHeight;
		//
		if( inLocalPoint.x > rect.left && inLocalPoint.x < rect.right &&
					inLocalPoint.y > rect.top && inLocalPoint.y < rect.bottom )
		{
			/*#164
			//R0228
			if( mEnableTabCloseButton == true )
			{
				ALocalPoint	lpt;
				lpt.x = rect.right - mTabRowHeight;
				lpt.y = rect.top + 2;
				AWindowPoint	wpt;
				NVM_GetWindow().NVI_GetWindowPointFromControlLocalPoint(NVI_GetControlID(),lpt,wpt);
				NVM_GetWindow().NVI_SetControlPosition(kTabCloseButtonControlID,wpt);
				NVM_GetWindow().NVI_SetControlSize(kTabCloseButtonControlID,mTabRowHeight-4,mTabRowHeight-4);
				NVM_GetWindow().NVI_SetShowControl(kTabCloseButtonControlID,true);
				mCurrentCloseButtonIndex = index;
			}
			*/
			//
			if( mHoverIndex != index )
			{
				mHoverIndex = index;
				NVI_Refresh();
			}
			//#164 クローズボタンのHover判定
			//#844 if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowTabCloseButton) == true )
			{
				ALocalRect	closeButtonRect = {0,0,0,0};
				GetCloseButtonRect(displayIndex,closeButtonRect);
				if( inLocalPoint.x >= closeButtonRect.left && inLocalPoint.x <= closeButtonRect.right &&
							inLocalPoint.y >= closeButtonRect.top && inLocalPoint.y <= closeButtonRect.bottom )//#688 =をつける（ゆっくり動かすとenter/exit時にちょうど境界と同じ値になるので）
				{
					if( mHoverCloseButton == false )
					{
						mHoverCloseButton = true;
						NVI_Refresh();
					}
				}
				else
				{
					if( mHoverCloseButton == true )
					{
						mHoverCloseButton = false;
						NVI_Refresh();
					}
				}
			}
			return true;
		}
	}
	/*#164
	//R0228
	if( mEnableTabCloseButton == true )
	{
		NVM_GetWindow().NVI_SetShowControl(kTabCloseButtonControlID,false);
		mCurrentCloseButtonIndex = kIndex_Invalid;
	}
	*/
	//#164 クローズボタンのHover解除
	if( mHoverCloseButton == true )
	{
		mHoverCloseButton = false;
		NVI_Refresh();
	}
	//
	ClearHover();
	return true;
}

//B0000 080810
void	AView_TabSelector::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	/*#164
	//R0228
	if( mEnableTabCloseButton == true )
	{
		ALocalRect	viewRect;
		NVM_GetLocalViewRect(viewRect);
		if( (inLocalPoint.x > viewRect.left && inLocalPoint.x < viewRect.right &&
						inLocalPoint.y > viewRect.top && inLocalPoint.y < viewRect.bottom) == false )
		{
			NVM_GetWindow().NVI_SetShowControl(kTabCloseButtonControlID,false);
			mCurrentCloseButtonIndex = kIndex_Invalid;
		}
	}
	*/
	//#1644 クローズボタンのHover解除
	if( mHoverCloseButton == true )
	{
		mHoverCloseButton = false;
		NVI_Refresh();
	}
	//
	ClearHover();
}

//B0000 080810
void	AView_TabSelector::ClearHover()
{
	mHoverIndex = kIndex_Invalid;
	NVI_Refresh();
}

//R0240
ABool	AView_TabSelector::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	outTagSide = kHelpTagSide_Default;//#643
	//#688 NVM_GetWindowConst().NVI_HideHelpTag();
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(NVI_GetViewWidth());
	for( AIndex index = 0; index < NVM_GetWindowConst().NVI_GetTabCount(); index++ )
	{
		/*#498
		AIndex	column = index%tabCountInOneRow;
		AIndex	row = index/tabCountInOneRow;
		*/
		AIndex	displayIndex = mDisplayOrderArray.Find(index);
		if( displayIndex == kIndex_Invalid )   continue;
		AIndex	column = displayIndex%tabCountInOneRow;
		AIndex	row = displayIndex/tabCountInOneRow;
		//
		ALocalRect	rect = viewRect;
		rect.left = column*mTabColumnWidth;
		rect.right = rect.left + mTabColumnWidth;
		rect.top = row*mTabRowHeight;
		rect.bottom = rect.top + mTabRowHeight;
		//
		if( inPoint.x > rect.left && inPoint.x < rect.right &&
					inPoint.y > rect.top && inPoint.y < rect.bottom )
		{
			ADocumentID	docID = NVM_GetWindowConst().NVI_GetDocumentIDByTabIndex(index);
			//ウインドウタイトルバーのテキストを取得
			AText	text;
			GetApp().SPI_GetTextWindowByID(NVM_GetWindowConst().GetObjectID()).SPI_GetTitleText(docID,text);
			//パス設定
			outText1.SetText(text);
			outText2.SetText(text);
			//タグ表示位置設定（多段の場合は一番下）
			outRect = rect;
			outRect.bottom = viewRect.bottom;
			return true;
		}
	}
	return false;
}

/**
Drag&Drop処理（Drag中）
*/
void	AView_TabSelector::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation )
{
	//#575
	if( mDragging == false )
	{
		//==================このタブからドラッグ中ではない場合==================
		//
		ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
	}
	else
	{
		//==================このタブからドラッグ中の場合==================
		//Drag中のタブ表示位置更新
		UpdateDragging(inLocalPoint);
		//
		ACursorWrapper::SetCursor(kCursorType_Arrow);
		//表示更新
		NVI_Refresh();
	}
}

/**
Drag&Drop処理（DragがViewに入った）
*/
void	AView_TabSelector::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mDragging == true )
	{
		//==================このタブからドラッグ中の場合==================
		//Drag中のタブ表示位置更新
		UpdateDragging(inLocalPoint);
		//表示更新
		NVI_Refresh();
	}
}

/**
Drag&Drop処理（DragがViewから出た）
*/
void	AView_TabSelector::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//表示更新
	NVI_Refresh();
}

/**
Drag中のタブ表示位置更新
*/
void	AView_TabSelector::UpdateDragging( const ALocalPoint& inLocalPoint )
{
	//Drop位置取得
	AIndex	dropDisplayIndex = FindDragIndex(inLocalPoint);
	if( mDraggingCurrentDisplayIndex != dropDisplayIndex )
	{
		//表示位置移動
		mDisplayOrderArray.Move(mDraggingCurrentDisplayIndex,dropDisplayIndex);
		//現在表示位置更新
		mDraggingCurrentDisplayIndex = dropDisplayIndex;
	}
	//ドラッグ位置のタブを非表示にする
	mDraggingTabShouldHide = true;
}

/**
Drag&Drop処理（Drop）
*/
void	AView_TabSelector::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//==================ファイルDrop受信処理==================
	AObjectArray<AFileAcc>	fileArray;
	AScrapWrapper::GetFileDragData(inDragRef,fileArray);
	for( AIndex i = 0; i < fileArray.GetItemCount(); i++ )
	{
		GetApp().SPNVI_CreateDocumentFromLocalFile(fileArray.GetObjectConst(i));
	}
	//表示更新
	NVI_Refresh();
}

//#364
/**
LocalPointから、Dropインデックスを取得
*/
AIndex	AView_TabSelector::FindDragIndex( const ALocalPoint& inLocalPoint ) const
{
	AIndex	col = (inLocalPoint.x/*#850 +(mTabColumnWidth/2)*/) / mTabColumnWidth;
	AIndex	row = inLocalPoint.y / mTabRowHeight;
	AIndex	index = col + row*GetTabCountInOneRow(NVI_GetViewWidth());
	if( index < 0 )   index = 0;
	if( index >= mDisplayOrderArray.GetItemCount() )   index = mDisplayOrderArray.GetItemCount()-1;
	return index;
}

//#164
/**
タブを閉じるボタンの表示領域取得
*/
void	AView_TabSelector::GetCloseButtonRect( const AIndex inTabDisplayIndex, ALocalRect& outRect ) const
{
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(NVI_GetViewWidth());
	AIndex	column = inTabDisplayIndex%tabCountInOneRow;
	AIndex	row = inTabDisplayIndex/tabCountInOneRow;
	
	outRect.left 	= (column)*mTabColumnWidth + 2;//(column+1)*mTabColumnWidth - 18;
	outRect.right 	= (column)*mTabColumnWidth +18;//(column+1)*mTabColumnWidth - 2;
	outRect.top 	= row*mTabRowHeight + 1;;
	ANumber	margin = (/*#844 GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kTextWindowTabHeight)*/mTabRowHeight -16) /2 +1;
	if( margin < 0 )    margin = 0;
	outRect.top += margin;
	outRect.bottom 	= outRect.top + 16;
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

//
ANumber	AView_TabSelector::SPI_GetHeight( const ANumber inViewWidth ) const
{
	return GetTabRowCount(inViewWidth)*mTabRowHeight;
}

//
AItemCount	AView_TabSelector::GetTabRowCount( const ANumber inViewWidth ) const
{
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(inViewWidth);
	if( tabCountInOneRow == 0 )   return 1;
	//#498 return (NVM_GetWindowConst().NVI_GetTabCount()+tabCountInOneRow-1)/tabCountInOneRow;
	return (mDisplayOrderArray.GetItemCount()+tabCountInOneRow-1)/tabCountInOneRow;//#498
}

//
AItemCount	AView_TabSelector::GetTabCountInOneRow( const ANumber inViewWidth ) const
{
	/*#844
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kMultipleRowTab) == false )
	{
		return 99999;
	}
	else
	*/
	{
		//B0000 0除算対策 常に1行のtab数は1以上にする return inViewWidth/mTabColumnWidth;
		AItemCount	result = inViewWidth/mTabColumnWidth;
		if( result <= 0 )   result = 1;
		return result;
	}
}

//R0177
void	AView_TabSelector::SPI_UpdateProperty()
{
	/*#844
	mTabRowHeight = static_cast<ANumber>(GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kTextWindowTabHeight));
	mTabRowHeight = 21;
	mTabColumnWidth = static_cast<ANumber>(GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kTextWindowTabWidth));
	mTabColumnWidth = 140;
	mFontSize = GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kTextWindowTabFontSize);
#if IMPLEMENTATION_FOR_WINDOWS
	mFontSize *= 0.8;
	if( mFontSize < 7.8 )   mFontSize = 7.8;
#endif
	*/
	//タブ幅設定 #1349
	mTabColumnWidth = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kTabWidth);
	//
	AFontWrapper::GetDialogDefaultFontName(mFontName);//#375
	NVMC_SetDefaultTextProperty(mFontName,mFontSize,kColor_Black,/*#732 kTextStyle_Normal*/kTextStyle_DropShadow,true);
	NVMC_GetMetricsForDefaultTextProperty(mFontHeight,mFontAscent);
	/*#164
	//R0228
	if( false )
	{
		ALocalRect	viewRect;
		NVM_GetLocalViewRect(viewRect);
		ALocalPoint	lpt;
		lpt.x = viewRect.right - mTabRowHeight +2;
		lpt.y = viewRect.top + 2;
		AWindowPoint	wpt;
		NVM_GetWindow().NVI_GetWindowPointFromControlLocalPoint(NVI_GetControlID(),lpt,wpt);
		NVM_GetWindow().NVI_SetControlPosition(kTabCloseButtonControlID,wpt);
		NVM_GetWindow().NVI_SetControlSize(kTabCloseButtonControlID,mTabRowHeight-4,mTabRowHeight-4);
		NVM_GetWindow().NVI_SetShowControl(kTabCloseButtonControlID,true);
	}
	*/
	//
	NVI_Refresh();
}

/*#164
//R0228
AIndex	AView_TabSelector::SPI_GetCurrentCloseButtonIndex() const
{
	if( mEnableTabCloseButton == true )
	{
		return mCurrentCloseButtonIndex;
	}
	else
	{
		return NVM_GetWindowConst().NVI_GetCurrentTabIndex();
	}
}
*/

//#498
/**
Tab追加時処理
*/
void	AView_TabSelector::SPI_DoTabCreated( const AIndex inTabIndex )
{
	//削除タブより後のインデックスをずらす
	for( AIndex i = 0; i < mDisplayOrderArray.GetItemCount(); i++ )
	{
		if( mDisplayOrderArray.Get(i) >= inTabIndex )
		{
			mDisplayOrderArray.Set(i,mDisplayOrderArray.Get(i)+1);
		}
	}
	//追加（選択可能タブの場合のみ）
	if( NVM_GetWindow().NVI_GetTabSelectable(inTabIndex) == true )
	{
		mDisplayOrderArray.Add(inTabIndex);
	}
	//描画更新
	NVI_Refresh();
	//#688 MouseTrackingRect更新→最終的には使用せず
	//UpdateMouseTrackingRect();
}

//#498
/**
Tab削除時処理
*/
void	AView_TabSelector::SPI_DoTabDeleted( const AIndex inTabIndex )
{
	//削除
	AIndex	index = mDisplayOrderArray.Find(inTabIndex);
	if( index != kIndex_Invalid )
	{
		mDisplayOrderArray.Delete1(index);
		
	}
	//削除タブより後のインデックスをずらす
	for( AIndex i = 0; i < mDisplayOrderArray.GetItemCount(); i++ )
	{
		if( mDisplayOrderArray.Get(i) > inTabIndex )
		{
			mDisplayOrderArray.Set(i,mDisplayOrderArray.Get(i)-1);
		}
	}
	//描画更新
	NVI_Refresh();
	//#688 MouseTrackingRect更新→最終的には使用せず
	//UpdateMouseTrackingRect();
}

//#850
/**
タブ表示index設定
*/
void	AView_TabSelector::SPI_SetDisplayTabIndex( const AIndex inTabIndex, const AIndex inDisplayTabIndex )
{
	AIndex	oldIndex = mDisplayOrderArray.Find(inTabIndex);
	mDisplayOrderArray.Move(oldIndex,inDisplayTabIndex);
	NVI_Refresh();
}

//#1062
/**
タブ表示順を設定する
*/
void	AView_TabSelector::SPI_SetDisplayOrderArray( const AArray<AIndex>& inTabIndexArray )
{
	for( AIndex i = 0; i < inTabIndexArray.GetItemCount(); i++ )
	{
		mDisplayOrderArray.Set(i,inTabIndexArray.Get(i));
	}
	NVI_Refresh();
}

/**
MouseTrackingRect更新→最終的には使用せず（設計変更）
*/
/*
void	AView_TabSelector::UpdateMouseTrackingRect()
{
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	
	//
	AArray<ALocalRect>	rectarray;
	//
	AItemCount	tabCountInOneRow = GetTabCountInOneRow(NVI_GetViewWidth());
	for( AIndex index = 0; index < mDisplayOrderArray.GetItemCount(); index++ )
	{
		//
		AIndex	column = index%tabCountInOneRow;
		AIndex	row = index/tabCountInOneRow;
		
		//
		ALocalRect	rect = viewRect;
		rect.left 		= column*mTabColumnWidth;
		rect.right 		= rect.left + mTabColumnWidth;
		rect.top 		= row*mTabRowHeight;
		rect.bottom 	= rect.top + mTabRowHeight;	
		
		//
		rectarray.Add(rect);
		
		//
		ALocalRect	closeButtonRect = {0};
		GetCloseButtonRect(index,closeButtonRect);
		rectarray.Add(closeButtonRect);
	}
	//
	NVM_GetWindow().NVI_SetMouseTrackingRect(NVI_GetControlID(),rectarray);
}
*/
//★control bounds変更時のUpdateMouseTrackingRect()

