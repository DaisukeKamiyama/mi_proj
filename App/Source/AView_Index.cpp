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

AView_Index

*/

#include "stdafx.h"

#include "AView_Index.h"
#include "ADocument_IndexWindow.h"
#include "AApp.h"
#include "AWindow_FindResult.h"

//列最小幅
const ANumber	kColumnMinWidth = 16;

#pragma mark ===========================
#pragma mark [クラス]AView_Index
#pragma mark ===========================
//インデックスウインドウのメインView

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_Index::AView_Index( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID, const AObjectID inIndexDocumentID ) 
	: AView(/*#199 inParent,inWindow*/inWindowID,inID), mIndexDocumentID(inIndexDocumentID), mSelectedRowIndex(kIndex_Invalid), /*#199mWindow(inWindow),*/ mFileColumnWidth(50)
	, mCursorRowDisplayIndex(kIndex_Invalid)//#92
	,mMouseTrackingMode(kMouseTrackingMode_None)//#688 , mMouseTrackByLoop(true)//win
	,mContextMenuSelectedRowIndex(kIndex_Invalid)//#465
,mOneColumnMode(false), mLineHeight(9)//#725
{
	NVMC_SetOffscreenMode(true);//win
	//#92 初期化はNVIDO_Init()へ移動
}

//デストラクタ
AView_Index::~AView_Index()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_Index::NVIDO_Init()
{
	/*#688
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
	SPI_UpdateTextDrawProperty();
	SPI_UpdateImageSize();
	//#92
	GetIndexDocument().NVI_RegisterView(GetObjectID());
	//#92
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時に必ずコールされる（デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと）
*/
void	AView_Index::NVIDO_DoDeleted()
{
	GetIndexDocument().NVI_UnregisterView(GetObjectID());
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

//IndexDocument取得
ADocument_IndexWindow&	AView_Index::GetIndexDocument()
{
	return GetApp().SPI_GetIndexWindowDocumentByID(mIndexDocumentID);
}
const ADocument_IndexWindow&	AView_Index::GetIndexDocumentConst() const
{
	return GetApp().SPI_GetIndexWindowDocumentConstByID(mIndexDocumentID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

const ANumber	kCellLeftRightPadding = 2;
const ANumber	kFoldingIconWidth = 16;

//一列表示モード時余白
const ANumber	kOneColumnMode_BoxLeftMargin = 3;
const ANumber	kOneColumnMode_BoxRightMargin = 3;
const ANumber	kOneColumnMode_BoxTopMargin = 2;
const ANumber	kOneColumnMode_BoxBottomMargin = 1;

//行間
const ANumber	kLineMargin = 1;
const ANumber	kOneColumnMode_LineMargin = 2;

//一列表示モード時パディング
const ANumber	kOneColumnMode_LeftPadding1stLine = 5;
const ANumber	kOneColumnMode_LeftPadding2ndLine = 8;

//描画要求時のコールバック(AView用)
void	AView_Index::EVTDO_DoDraw()
{
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	//=========================描画データ取得=========================
	//フォント取得
	//内容部分フォント
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//ヘッダ部分フォント
	AText	labelfontname;
	AFontWrapper::GetDialogDefaultFontName(labelfontname);
	
	//=========================背景情報取得（このviewでは背景は描画しない）=========================
	//背景情報取得
	AColor	backgroundColor = kColor_White;
	AFloatNumber	backgroundAlpha = 1.0;
	//サブウインドウの場合の背景色・α取得
	if( GetApp().SPI_IsSubWindow(NVM_GetWindow().GetObjectID()) == true )
	{
		backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
		if( NVM_GetWindow().NVI_GetOverlayMode() == false )
		{
			//フローティングの指定透過率で背景消去
			backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			//NVMC_PaintRect(localFrameRect,backgroundColor,backgroundAlpha);
		}
		else
		{
			//NVMC_PaintRect(localFrameRect,backgroundColor);
		}
	}
	
	//=========================各行描画=========================
	AColor	normalcolor = GetApp().SPI_GetSubWindowLetterColor();
	//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kIndexWinNormalColor,normalcolor);
	AColor	groupcolor = kColor_Red;
	//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kIndexWinGroupColor,groupcolor);
	//
	NVMC_SetDefaultTextProperty(fontname,fontsize,normalcolor,kTextStyle_Normal,true);
	
	AItemCount	rowCount = GetIndexDocument().SPI_GetDisplayRowCount();
	AIndex	startRow = imageFrameRect.top / GetRowHeight();
	AIndex	endRow = imageFrameRect.bottom / GetRowHeight();
	for( AIndex rowIndex = startRow; rowIndex <= endRow; rowIndex++ )
	{
		if( rowIndex >= rowCount )   break;
		AImageRect	rowImageRect;
		GetRowImageRect(rowIndex,rowIndex+1,rowImageRect);
		ALocalRect	rowLocalRect;
		NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
		if( NVMC_IsRectInDrawUpdateRegion(rowLocalRect) == false )   continue;
		
		//行データの取得
		AText	path, prehit, hit, posthit, position;
		ANumber	start, length, paragraph;
		ABool	isGroup;
		AIndex	groupIndex;
		AText	comment;
		GetIndexDocument().SPI_GetDisplayRowData(rowIndex,path,hit,prehit,posthit,position,start,length,paragraph,groupIndex,isGroup,comment);
		
		//色の設定
		AColor	color;
		if( isGroup == true )
		{
			color = groupcolor;
		}
		else
		{
			color = normalcolor;
		}
		
		if( mOneColumnMode == true )
		{
			//==================一列表示モード==================
			rowLocalRect.left 		+= kOneColumnMode_BoxLeftMargin;
			rowLocalRect.right		-= kOneColumnMode_BoxRightMargin;
			rowLocalRect.top		+= kOneColumnMode_BoxTopMargin;
			rowLocalRect.bottom		-= kOneColumnMode_BoxBottomMargin;
			
			//ボックス描画#643
			if( rowIndex == mSelectedRowIndex )
			{
				AColor	selectionColor = kColor_Blue;
				AColorWrapper::GetHighlightColor(selectionColor);//#1034 GetListHighlightColor()→GetHighlightColor()
				AColor	color2 = AColorWrapper::ChangeHSV(selectionColor,0,0.6,1.0);
				NVMC_PaintRoundedRect(rowLocalRect,color2,selectionColor,kGradientType_Vertical,backgroundAlpha,backgroundAlpha,
									  5,true,true,true,true);
				NVMC_FrameRoundedRect(rowLocalRect,kColor_Gray20Percent,1.0,5,true,true,true,true);
				
				//
				color = kColor_White;
			}
			else
			{
				NVMC_PaintRoundedRect(rowLocalRect,
									  kColor_White,kColor_Gray95Percent,kGradientType_Vertical,backgroundAlpha,backgroundAlpha,
									  5,true,true,true,true);
				NVMC_FrameRoundedRect(rowLocalRect,kColor_Gray20Percent,1.0,5,true,true,true,true);
			}
		}
		else
		{
			//==================二列表示モード==================
			//Selection描画
			if( rowIndex == mSelectedRowIndex )
			{
				/*
				AColor	selectionColor;
				if( NVM_GetWindow().GetObjectID() == GetApp().SPI_GetActiveSubWindowForItemSelector() )
				{
					AColorWrapper::GetHighlightColor(selectionColor);
				}
				else
				{
					selectionColor = kColor_Gray80Percent;
				}
				NVMC_PaintRect(rowLocalRect,selectionColor,0.8);
				*/
				
				//------------------選択行------------------
				
				//選択色取得
				AColor	selColor1 = kColor_Blue, selColor2 = kColor_Blue, selColor3 = kColor_Blue;
				if( NVM_GetWindow().NVI_IsWindowActive() == true ) 
				{
					//activated時
					selColor1 = AColorWrapper::GetColorByHTMLFormatColor("5c91d2");
					selColor2 = AColorWrapper::GetColorByHTMLFormatColor("71a2dc");
					selColor3 = AColorWrapper::GetColorByHTMLFormatColor("2966b9");
				}
				else
				{
					//deactivated時
					selColor1 = AColorWrapper::GetColorByHTMLFormatColor("a8a8a8");
					selColor2 = AColorWrapper::GetColorByHTMLFormatColor("c1c1c1");
					selColor3 = AColorWrapper::GetColorByHTMLFormatColor("9c9c9c");
				}
				NVMC_PaintRectWithVerticalGradient(rowLocalRect,selColor2,selColor3,1.0,1.0);
				//選択部分の最上部1pixelを描画
				ALocalRect	rect = rowLocalRect;
				rect.bottom = rect.top+1;
				NVMC_PaintRect(rect,selColor1,1.0);
				
				//文字色は白色にする
				color = kColor_White;
			}
			//#92
			else if( rowIndex == mCursorRowDisplayIndex )
			{
				/*
				AColor	selectionColor;
				AColorWrapper::GetListHighlightColor(selectionColor);
				NVMC_PaintRect(rowLocalRect,selectionColor,0.2);
				NVMC_FrameRect(rowLocalRect,selectionColor,1.0);
				*/
				//------------------ホバー行------------------
				ALocalRect	rect = rowLocalRect;
				rect.left		+= 2;
				rect.top		+= 2;
				rect.right		-= 2;
				rect.bottom		-= 2;
				NVMC_FrameRoundedRect(rect,kColor_Blue,0.3,3,true,true,true,true,true,true,true,true,1.0);
			}
		}
		
		//==================ファイル欄（ヘッダ）表示==================
		//
		if( mOneColumnMode == true )
		{
			ALocalRect	headerLocalRect = rowLocalRect;
			headerLocalRect.bottom = headerLocalRect.top + mLineHeight;
			//ヘッダ色取得
			AColor	headercolor = kColor_Blue;
			/*
			if( mClickedArray.Get(i) == true )
			{
			headercolor = kColor_Gray;
			}
			*/
			//ヘッダrect描画
			AColor	headercolor_start = AColorWrapper::ChangeHSV(headercolor,0,0.8,1.0);
			AColor	headercolor_end = AColorWrapper::ChangeHSV(headercolor,0,1.2,1.0);
			NVMC_PaintRoundedRect(headerLocalRect,headercolor_start,headercolor_end,kGradientType_Vertical,0.15,0.15,
								  5,true,true,true,true);
		}
		//
		ALocalRect	rect = rowLocalRect;
		if( mOneColumnMode == false )
		{
			rect.left += kCellLeftRightPadding;
			rect.right = mFileColumnWidth - kCellLeftRightPadding;
		}
		else
		{
			rect.left += kOneColumnMode_LeftPadding1stLine;
			rect.right -= kCellLeftRightPadding;
			rect.bottom = rect.top + mLineHeight;
		}
		//
		if( isGroup == true )
		{
			ALocalRect	iconrect;
			iconrect.left		= rect.left;
			iconrect.right		= rect.left + 16;
			iconrect.top		= (rect.top+rect.bottom)/2 -8;
			iconrect.bottom		= (rect.top+rect.bottom)/2 +8;
			//#688 AIconID	iconID = kIconID_Mi_ArrowDown;
			AImageID	iconID = kImageID_barSwCursorDown;
			if( GetIndexDocument().SPI_IsGroupExpanded(groupIndex) == false )
			{
				iconID = kImageID_barSwCursorRight;//#688 kIconID_Mi_ArrowRight;
			}
			//#688 NVMC_DrawIcon(iconrect,iconID,true,true);//win 080722
			ALocalPoint	pt = {0,0};
			pt.x = iconrect.left;
			pt.y = iconrect.top;
			NVMC_DrawImage(iconID,pt);
			//
			rect.left += kFoldingIconWidth;
			AText	text;
			text.SetText(path);
			//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Normal,true);
			NVMC_SetDefaultTextProperty(color,kTextStyle_Normal);//win
			NVMC_DrawText(rect,text);
		}
		else
		{
			AText	text;
			if( mOneColumnMode == false )
			{
				rect.left += kFoldingIconWidth;
			}
			if( path.GetItemCount() > 0 )//#220
			{
				if( path.Get(0) == kUChar_Tab )//#221
				{
					//#221 新規ドキュメントの場合は、ファイルパスにタブコード+DocumentIDを格納することにする
					ADocumentID	docID;
					AIndex	pos = 1;
					AUniqID	docUniqID;//#693
					path.ParseIntegerNumber(pos,docUniqID.val,false);
					docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
					if( GetApp().NVI_IsDocumentValid(docID) == true )
					{
						if( GetApp().NVI_GetDocumentTypeByID(docID) == kDocumentType_Text )
						{
							GetApp().NVI_GetDocumentByID(docID).NVI_GetTitle(text);
						}
					}
				}
				else
				{
					path.GetFilename(text);
				}
				//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Bold,true);
				NVMC_SetDefaultTextProperty(color,kTextStyle_Bold);//win
				ANumber	w = static_cast<ANumber>(NVMC_GetDrawTextWidth(text));
				NVMC_DrawText(rect,text);
				rect.left += w;
				
				text.SetCstring("   ");
				text.AddText(position);
				if( paragraph != kIndex_Invalid )
				{
					text.AddCstring("  ");
					AText	paratext;
					paratext.SetLocalizedText("FindResult_Paragraph");
					text.AddText(paratext);
					text.AddFormattedCstring("%d",paragraph+1);
				}
			}
			//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Normal,true);
			NVMC_SetDefaultTextProperty(color,kTextStyle_Normal);//win
			NVMC_DrawText(rect,text);
		}
		
		//==================内容欄表示==================
		rect = rowLocalRect;
		if( mOneColumnMode == false )
		{
			//------------------複数カラム表示モード------------------
			rect.left = mFileColumnWidth + kCellLeftRightPadding;
			if( comment.GetItemCount() == 0 )
			{
				if( prehit.GetItemCount() > 0 || posthit.GetItemCount() > 0 )
				{
					//
					prehit.ReplaceChar(kUChar_LineEnd,kUChar_Space);
					posthit.ReplaceChar(kUChar_LineEnd,kUChar_Space);
					//
					NVMC_SetDefaultTextProperty(kColor_Black,kTextStyle_Normal);//win
					ANumber	w1 = static_cast<ANumber>(NVMC_GetDrawTextWidth(prehit));
					NVMC_SetDefaultTextProperty(kColor_Black,kTextStyle_Bold);//win
					ANumber	w2 = static_cast<ANumber>(NVMC_GetDrawTextWidth(hit));
					ANumber	w = w1 + w2/2;
					ANumber	drawStartX = rect.left - (w-(rect.right-rect.left)/2);
					ALocalRect	drawRect;
					drawRect = rect;
					drawRect.left = drawStartX;
					//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Normal,true);
					NVMC_SetDefaultTextProperty(color,kTextStyle_Normal);//win
					NVMC_DrawText(drawRect,rect,prehit);
					drawRect.left += w1;
					//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Bold,true);
					NVMC_SetDefaultTextProperty(color,kTextStyle_Bold);//win
					NVMC_DrawText(drawRect,rect,hit);
					drawRect.left += w2;
					//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Normal,true);
					NVMC_SetDefaultTextProperty(color,kTextStyle_Normal);//win
					NVMC_DrawText(drawRect,rect,posthit);
				}
				//R0238
				else
				{
					NVMC_SetDefaultTextProperty(color,kTextStyle_Bold);
					NVMC_DrawText(rect,rect,hit);
				}
			}
			else
			{
				//win NVMC_SetDefaultTextProperty(font,fontsize,color,kTextStyle_Normal,true);
				NVMC_SetDefaultTextProperty(color,kTextStyle_Normal);//win
				NVMC_DrawText(rect,comment);
			}
			//区切り線表示
			ALocalPoint	spt, ept;
			spt.x = ept.x = mFileColumnWidth;
			spt.y = rect.top;
			ept.y = rect.bottom;
			NVMC_DrawLine(spt,ept,normalcolor,1.0,1.0);
		}
		else
		{
			//------------------１カラム表示モード------------------
			//
			rect.left += kOneColumnMode_LeftPadding2ndLine;
			rect.top = rect.top + mLineHeight;
			//
			//未実装
		}
		rect.right -= kCellLeftRightPadding;
	}
}

//マウスボタン押下時のコールバック(AView用)
ABool	AView_Index::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	AIndex	rowIndex = clickImagePoint.y/GetRowHeight();
	if( rowIndex >= GetIndexDocument().SPI_GetDisplayRowCount() )   return false;
	
	mSelectedRowIndex = rowIndex;
	NVMC_RefreshControl();
	
	AText	path, prehit, hit, posthit, position;
	ANumber	start, length, paragraph;
	ABool	isGroup;
	AIndex	groupIndex;
	AText	comment;
	GetIndexDocumentConst().SPI_GetDisplayRowData(rowIndex,path,hit,prehit,posthit,position,start,length,paragraph,groupIndex,isGroup,comment);
	
	if( isGroup == true && inLocalPoint.x < 16 )
	{
		GetIndexDocument().SPI_ExpandCollapse(groupIndex);
		return false;
	}
	
	ANumber	w = mFileColumnWidth;
	if( inLocalPoint.x >= w-kColumnWidthChangeAreaWidth && inLocalPoint.x <= w+kColumnWidthChangeAreaWidth )
	{
		DragChangeColumnWidth(inLocalPoint);//win
		return false;
	}
	
	//ダブルクリック
	//#92 if( inClickCount == 2 )
	{
		if( isGroup == true )
		{
			GetIndexDocument().SPI_ExpandCollapse(groupIndex);
		}
		else
		{
			SPI_OpenFromRow(rowIndex);
		}
	}
	return false;
}

void	AView_Index::DragChangeColumnWidth( const ALocalPoint inLocalPoint )//win
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	//ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	/*
	ALocalPoint	spt, ept;
	spt.x = inLocalPoint.x;
	ept.x = spt.x;
	spt.y = localFrameRect.top;
	ept.y = localFrameRect.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,false,1);
	*/
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	mMouseTrackStartPoint = inLocalPoint;
	mMouseTrackPoint = inLocalPoint;
	NVM_SetMouseTrackingMode(true);
	return;
	//#688 }
	/*#688
	//最初のマウス位置取得（この時点でマウスボタンが放されていたらリターン。TrackMouseDown()でマウスボタンリリースのイベントを取得したら、次はもう来ない。B0260）
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
	
	//Local座標に変換
	ALocalPoint	mouseLocalPoint;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
	//
	ALocalPoint	spt, ept;
	spt.x = mouseLocalPoint.x;
	ept.x = spt.x;
	spt.y = localFrameRect.top;
	ept.y = localFrameRect.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,1);
	
	//前回マウス位置の保存
	ALocalPoint	previousLocalPoint = mouseLocalPoint;
	//マウスボタンが放されるまでループ
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//Local座標に変換
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		//前回マウス位置と同じなら何もせずループ継続
		if( mouseLocalPoint.x == previousLocalPoint.x && mouseLocalPoint.y == previousLocalPoint.y )
		{
			continue;
		}
		//
		NVMC_DrawXorCaretLine(spt,ept,true,1);
		spt.x = mouseLocalPoint.x;
		ept.x = spt.x;
		spt.y = localFrameRect.top;
		ept.y = localFrameRect.bottom;
		NVMC_DrawXorCaretLine(spt,ept,true,1);
		
	}
	//
	NVMC_DrawXorCaretLine(spt,ept,true,1);
	
	//
	SPI_SetFileColumnWidth(spt.x-localFrameRect.left);
	*/
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_Index::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( inLocalPoint.x == mMouseTrackPoint.x && inLocalPoint.y == mMouseTrackPoint.y )   return;
	
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	/*
	ALocalPoint	spt, ept;
	spt.x = mMouseTrackPoint.x;
	ept.x = spt.x;
	spt.y = localFrameRect.top;
	ept.y = localFrameRect.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,false,1);
	*/
	mMouseTrackPoint = inLocalPoint;
	/*
	spt.x = mMouseTrackPoint.x;
	ept.x = spt.x;
	spt.y = localFrameRect.top;
	ept.y = localFrameRect.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,false,1);
	*/
	SPI_SetFileColumnWidth(inLocalPoint.x-localFrameRect.left);
}

//Mouse Tracking終了（マウスボタンUp）
void	AView_Index::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	/*
	ALocalPoint	spt, ept;
	spt.x = mMouseTrackPoint.x;
	ept.x = spt.x;
	spt.y = localFrameRect.top;
	ept.y = localFrameRect.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,false,1);
	*/
	//
	SPI_SetFileColumnWidth(inLocalPoint.x-localFrameRect.left);
	
	//TrackingMode解除
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}
//#688 #endif

//
void	AView_Index::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
		const ALocalPoint inLocalPoint )//win 080706
{
	//水平方向ホイール禁止なら常にdeltaXは0にする
	ANumber	deltaX = inDeltaX;
	/*
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisableHorizontalWheelScroll) == true )
	{
		deltaX = 0;
	}
	*/
	//Y変化量を設定（cmdキーなしなら常に１段階ずつ） win
	ANumber	yscroll = 0;
	ANumber	scrollPercent = 100;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true 
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd);
		yscroll = inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100;
	}
	else
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent);
		if( inDeltaY > 0 )
		{
			yscroll = NVI_GetVScrollBarUnit();
		}
		else if( inDeltaY < 0 )//（#956 inDeltaYが0のときはyscrollを0のままにする）
		{
			yscroll = -NVI_GetVScrollBarUnit();
		}
	}
	//スクロール実行
	NVI_Scroll(deltaX*NVI_GetHScrollBarUnit()*scrollPercent/100,yscroll);
	
	//#92
	UpdateCursorRow(inLocalPoint);
}

//カーソル
ACursorType	AView_Index::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ANumber	w = mFileColumnWidth;
	if( inLocalPoint.x >= w-kColumnWidthChangeAreaWidth && inLocalPoint.x <= w+kColumnWidthChangeAreaWidth )
	{
		return kCursorType_ResizeLeftRight;
	}
	return kCursorType_Arrow;
}

//
ABool	AView_Index::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	//#688
	outUpdateMenu = false;
	//
	ABool	result = false;
	if( inText.GetItemCount() == 1 )
	{
		if( mSelectedRowIndex != kIndex_Invalid )
		{
			AText	path, prehit, hit, posthit, position;
			ANumber	start, length, paragraph;
			ABool	isGroup;
			AIndex	groupIndex;
			AText	comment;
			GetIndexDocumentConst().SPI_GetDisplayRowData(mSelectedRowIndex,path,hit,prehit,posthit,position,start,length,paragraph,groupIndex,isGroup,comment);
			
			AUChar	ch = inText.Get(0);
			switch(ch)
			{
			  case kUChar_CR:
				{
					if( isGroup == true )
					{
						GetIndexDocument().SPI_ExpandCollapse(groupIndex);
					}
					else
					{
						SPI_OpenFromRow(mSelectedRowIndex);
					}
					result = true;
					break;
				}
			  case 0x1E://up
				{
					if( AKeyWrapper::IsCommandKeyPressed(/*#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent)*/inModifierKeys) == true )
					{
						SPI_SelectRow(0);
					}
					else
					{
						if( mSelectedRowIndex > 0 )
						{
							SPI_SelectRow(mSelectedRowIndex-1);
						}
					}
					result = true;
					break;
				}
			  case 0x1F://down
				{
					if( AKeyWrapper::IsCommandKeyPressed(/*#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent)*/inModifierKeys) == true )
					{
						SPI_SelectRow(GetIndexDocumentConst().SPI_GetDisplayRowCount()-1);
					}
					else
					{
						if( mSelectedRowIndex < GetIndexDocumentConst().SPI_GetDisplayRowCount()-1 )
						{
							SPI_SelectRow(mSelectedRowIndex+1);
						}
					}
					result = true;
					break;
				}
			}
		}
	}
	return result;
}

//#92
/**
マウス移動イベント
*/
ABool	AView_Index::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	UpdateCursorRow(inLocalPoint);
	return true;
}

//#92
/**
マウス領域外移動イベント
*/
void	AView_Index::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearCursorRow();
}

//#92
/**
カーソルHover行更新
*/
void	AView_Index::UpdateCursorRow( const ALocalPoint& inLocalPoint )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	
	AIndex newCursorIndex = clickImagePoint.y/GetRowHeight();
	if( newCursorIndex != mCursorRowDisplayIndex )
	{
		AIndex	svRowIndex = mCursorRowDisplayIndex;
		mCursorRowDisplayIndex = newCursorIndex;
		SPI_RefreshRow(svRowIndex);
		SPI_RefreshRow(mCursorRowDisplayIndex);
	}
}

//#92
/**
カーソルHover行クリア
*/
void	AView_Index::ClearCursorRow()
{
	if( mCursorRowDisplayIndex == kIndex_Invalid )   return;
	AIndex	svRowIndex = mCursorRowDisplayIndex;
	mCursorRowDisplayIndex = kIndex_Invalid;
	SPI_RefreshRow(svRowIndex);
}

//#92
/**
Viewがdeactivateされたときにコールされる
*/
void	AView_Index::NVMDO_DoViewDeactivated()
{
	ClearCursorRow();
}

/**
Viewのサイズ変更時にコールされる
*/
void	AView_Index::NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight )
{
	mFileColumnWidth = inWidth * GetApp().GetAppPref().GetData_Number(AAppPrefDB::kIndexWindowSeparateLine)/100;
	SPI_UpdateImageSize();//#291
}

//#465
//マウスボタン押下時のコールバック(AView用)
ABool	AView_Index::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	AIndex	rowIndex = clickImagePoint.y/GetRowHeight();
	if( rowIndex >= GetIndexDocument().SPI_GetDisplayRowCount() )   return false;
	
	mContextMenuSelectedRowIndex = rowIndex;
	
	AGlobalPoint	globalPoint = {0};
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_IndexView,
	//#688 globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	NVMC_ShowContextMenu(kContextMenuID_IndexView,globalPoint);//#688
	return true;
}

//#541
/**
ヘルプタグテキスト取得
*/
ABool	AView_Index::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	outTagSide = kHelpTagSide_Default;//#643
	//#688 NVM_GetWindowConst().NVI_HideHelpTag();
	
	//表示行index取得
	AImagePoint	clickImagePoint = {0,0};
	NVM_GetImagePointFromLocalPoint(inPoint,clickImagePoint);
	if( clickImagePoint.x > mFileColumnWidth )   return false;//ファイル欄のみ対応
	AIndex	rowIndex = clickImagePoint.y/GetRowHeight();
	if( rowIndex >= GetIndexDocumentConst().SPI_GetDisplayRowCount() )   return false;
	
	//行データ取得
	AText	path, prehit, hit, posthit, position;
	ANumber	start = 0, length = 0, paragraph = 0;
	ABool	isGroup = false;
	AIndex	groupIndex = kIndex_Invalid;
	AText	comment;
	GetIndexDocumentConst().SPI_GetDisplayRowData(rowIndex,path,hit,prehit,posthit,position,start,length,paragraph,groupIndex,isGroup,comment);
	
	//text設定
	outText1.SetText(path);
	outText1.Add(kUChar_LineEnd);
	outText1.AddText(position);
	outText2.SetText(outText1);
	//行rect取得
	AImageRect	imagerect = {0,0,0,0};
	GetRowImageRect(rowIndex,rowIndex+1,imagerect);
	NVM_GetLocalRectFromImageRect(imagerect,outRect);
	return true;
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

//
void	AView_Index::SPI_UpdateRows( const AIndex inStartRowIndex, const AIndex inEndRowIndex ) 
{
	//表示フレームをImage座標で取得
	AImageRect	frameImageRect;
	NVM_GetImageViewRect(frameImageRect);
	//
	AImageRect	imageRect;
	GetRowImageRect(inStartRowIndex,inEndRowIndex,imageRect);
	if( frameImageRect.top < imageRect.bottom || frameImageRect.bottom > imageRect.top )
	{
		ALocalRect	localRect;
		NVM_GetLocalRectFromImageRect(imageRect,localRect);
		NVMC_RefreshRect(localRect);
	}
}

//
void	AView_Index::SPI_UpdateImageSize()
{
	ANumber	width = NVI_GetViewWidth();
	ANumber	height = GetIndexDocumentConst().SPI_GetDisplayRowCount() * GetRowHeight();
	NVM_SetImageSize(width,height);
}

//
void	AView_Index::SPI_UpdateTextDrawProperty()
{
	AColor	color = kColor_Black;
	//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kIndexWinNormalColor,color);
	/*#844
	AText	fontname;//win
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kIndexWindowFontName,fontname);//win
	*/
	AFloatNumber	fontsize = GetApp().GetAppPref().GetData_FloatNumber(AAppPrefDB::kSubWindowsFontSize);//#844
	AText	fontname;
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kSubWindowsFontName,fontname);//#844
	NVMC_SetDefaultTextProperty(fontname,fontsize,color,kTextStyle_Normal,true);
	NVMC_GetMetricsForDefaultTextProperty(mLineHeight,mAscent);
	if( mOneColumnMode == false )
	{
		mLineHeight += kLineMargin;
	}
	else
	{
		mLineHeight += kOneColumnMode_LineMargin;
	}
	
	SPI_UpdateScrollBarUnit();
}

//
void	AView_Index::SPI_UpdateScrollBarUnit()
{
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(kHScrollBarUnit,GetRowHeight(),rect.right-rect.left-kHScrollBarUnit*5,rect.bottom-rect.top-GetRowHeight()*5);
}

//
ANumber	AView_Index::GetRowHeight() const
{
	if( mOneColumnMode == false )
	{
		return mLineHeight;
	}
	else
	{
		return mLineHeight*2 + kOneColumnMode_BoxTopMargin + kOneColumnMode_BoxBottomMargin;
	}
}

//
void	AView_Index::GetRowImageRect( const AIndex inStartRowIndex, const AIndex inEndRowIndex, AImageRect& outRowImageRect ) const
{
	outRowImageRect.left	= 0;
	outRowImageRect.right	= NVM_GetImageWidth();
	outRowImageRect.top		= inStartRowIndex*GetRowHeight();
	outRowImageRect.bottom	= inEndRowIndex*GetRowHeight();
}

void	AView_Index::SPI_OpenFromRow( const AIndex inRowIndex ) const
{
	//項目選択アクティブサブウインドウに設定
	GetApp().SPI_SetActiveSubWindowForItemSelector(NVM_GetWindowConst().GetObjectID());
	//
	GetApp().SPI_SleepForAWhileMultiFileFind();
	//行データの取得
	AText	path, prehit, hit, posthit, position;
	ANumber	start = 0, length = 0, paragraph = 0;
	ABool	isGroup = false;
	AIndex	groupIndex = 0;
	AText	comment;
	GetIndexDocumentConst().SPI_GetDisplayRowData(inRowIndex,path,hit,prehit,posthit,position,start,length,paragraph,groupIndex,isGroup,comment);
	if( path.GetItemCount() == 0 )   return;//#221
	if( isGroup == true )   return;//#960 グループ項目の場合はファイルオープン処理を行わない
	//現在の位置をNavigate登録する #146
	GetApp().SPI_RegisterNavigationPosition();
	//
	if( path.Get(0) == kUChar_Tab )//#221
	{
		//==================新規ドキュメントの場合==================
		//#221 新規ドキュメントの場合は、ファイルパスにタブコード+DocumentIDを格納することにする
		ADocumentID	docID;
		AIndex	pos = 1;
		AUniqID	docUniqID;//#693
		path.ParseIntegerNumber(pos,docUniqID.val,false);
		docID = GetApp().NVI_GetDocumentIDByUniqID(docUniqID);//#693
		if( GetApp().NVI_IsDocumentValid(docID) == true )
		{
			if( GetApp().NVI_GetDocumentTypeByID(docID) == kDocumentType_Text )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_SelectText(start,length,true);
			}
		}
	}
	else
	{
		//==================通常ファイルの場合==================
		/*#725
		AFileAcc	file;
		file.Specify(path);
		GetApp().SPI_SelectTextDocument(file,start,length,true);
		*/
		//ファイルの指定場所を開く
		GetApp().SPI_OpenOrRevealTextDocument(path,start,start+length,kObjectID_Invalid,false,kAdjustScrollType_Center);
	}
}

void	AView_Index::SPI_ScrollToHome()
{
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

void	AView_Index::SPI_SelectNextRow()
{
	AIndex	old = mSelectedRowIndex;
	if( mSelectedRowIndex == kIndex_Invalid )
	{
		mSelectedRowIndex = 0;
	}
	else
	{
		if( mSelectedRowIndex < GetIndexDocumentConst().SPI_GetDisplayRowCount()-1 )
		{
			mSelectedRowIndex++;
		}
		else return;
	}
	SPI_RefreshRow(old);
	SPI_RefreshRow(mSelectedRowIndex);
	SPI_OpenFromRow(mSelectedRowIndex);
	SPI_AdjustScroll();
}

void	AView_Index::SPI_SelectPreviousRow()
{
	AIndex	old = mSelectedRowIndex;
	if( mSelectedRowIndex == kIndex_Invalid )
	{
		mSelectedRowIndex = GetIndexDocumentConst().SPI_GetDisplayRowCount()-1;
	}
	else
	{
		if( mSelectedRowIndex > 0 )
		{
			mSelectedRowIndex--;
		}
		else return;
	}
	SPI_RefreshRow(old);
	SPI_RefreshRow(mSelectedRowIndex);
	SPI_OpenFromRow(mSelectedRowIndex);
	SPI_AdjustScroll();
}

void	AView_Index::SPI_SelectRow( const AIndex inDisplayRowIndex )
{
	AIndex	old = mSelectedRowIndex;
	mSelectedRowIndex = inDisplayRowIndex;
	SPI_RefreshRow(old);
	SPI_RefreshRow(mSelectedRowIndex);
	SPI_AdjustScroll();
}

void	AView_Index::SPI_AdjustScroll()
{
	if( mSelectedRowIndex == kIndex_Invalid )
	{
		AImagePoint	pt = {0,0};
		NVI_ScrollTo(pt);
		return;
	}
	AImageRect	rect;
	NVM_GetImageViewRect(rect);
	AIndex	startRow = rect.top / GetRowHeight();
	AIndex	endRow = rect.bottom / GetRowHeight();
	if( mSelectedRowIndex < startRow || mSelectedRowIndex >= endRow )
	{
		AImagePoint	pt;
		pt.x = 0;
		pt.y = (mSelectedRowIndex-(endRow-startRow)/2)*GetRowHeight();
		NVI_ScrollTo(pt);
	}
}

void	AView_Index::SPI_RefreshRow( const AIndex inDisplayRowIndex )
{
	if( inDisplayRowIndex == kIndex_Invalid )   return;
	AImageRect	rowImageRect;
	GetRowImageRect(inDisplayRowIndex,inDisplayRowIndex+1,rowImageRect);
	ALocalRect	rowLocalRect;
	NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
	NVMC_RefreshRect(rowLocalRect);
}

void	AView_Index::SPI_SetOnlyFileColumnWidth( const ANumber inNewWidth )
{
	mFileColumnWidth = inNewWidth;
}

void	AView_Index::SPI_SetInfoColumnWidth( const ANumber inNewWidth )
{
#if 0
	//カラム幅変更時にはウインドウサイズも変更する仕様だったが、不要（あまり良くない仕様）と思われるので、通常のインターフェイスにする
	if( NVI_IsBorrowerView() == false )//#92
	{
		ANumber	oldWidth = NVI_GetViewWidth()-mFileColumnWidth;
		ARect	rect;
		/*#199 mWindow*/NVM_GetWindowConst().NVI_GetWindowBounds(rect);
		ANumber	windowOldWidth = rect.right-rect.left;
		ANumber	windowNewWidth = windowOldWidth + inNewWidth - oldWidth;
		rect.right = rect.left + windowNewWidth;
		/*#199 mWindow*/NVM_GetWindow().NVI_SetWindowBounds(rect);
		//B0415 mWindow.EVT_WindowResizeCompleted();
		//
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowSeparateLine,mFileColumnWidth*100/windowNewWidth);
	}
	else
#endif
	{
		//#92
		mFileColumnWidth = NVI_GetViewWidth() - inNewWidth;
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowSeparateLine,mFileColumnWidth*100/NVI_GetViewWidth());
	}
	NVI_Refresh();
}

void	AView_Index::SPI_SetFileColumnWidth( const ANumber inNewWidth )
{
#if 0
	//カラム幅変更時にはウインドウサイズも変更する仕様だったが、不要（あまり良くない仕様）と思われるので、通常のインターフェイスにする
	if( NVI_IsBorrowerView() == false )//#92
	{
		ANumber	oldWidth = mFileColumnWidth;
		mFileColumnWidth = inNewWidth;
		ARect	rect;
		/*#199 mWindow*/NVM_GetWindowConst().NVI_GetWindowBounds(rect);
		ANumber	windowOldWidth = rect.right-rect.left;
		ANumber	windowNewWidth = windowOldWidth + inNewWidth - oldWidth;
		rect.right = rect.left + windowNewWidth;
		/*#199 mWindow*/NVM_GetWindow().NVI_SetWindowBounds(rect);
		//B0415 mWindow.EVT_WindowResizeCompleted();
		//
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowSeparateLine,inNewWidth*100/windowNewWidth);
	}
	else
#endif
	{
		//#92
		mFileColumnWidth = inNewWidth;
		//
		if( mFileColumnWidth < kColumnMinWidth )
		{
			mFileColumnWidth = kColumnMinWidth;
		}
		if( NVI_GetViewWidth()-mFileColumnWidth < kColumnMinWidth )
		{
			mFileColumnWidth = NVI_GetViewWidth() - kColumnMinWidth;
		}
		//
		GetApp().GetAppPref().SetData_Number(AAppPrefDB::kIndexWindowSeparateLine,mFileColumnWidth*100/NVI_GetViewWidth());
	}
	NVI_Refresh();
}

void	AView_Index::SPI_GetColumnWidth( ANumber& outFileColumnWidth, ANumber& outInfoColumnWidth ) const
{
	outFileColumnWidth = mFileColumnWidth;
	outInfoColumnWidth = NVI_GetViewWidth()-mFileColumnWidth;
}

//#92
/**
保存ダイアログをViewの親ウインドウ上に表示する
*/
void	AView_Index::SPI_ShowSaveWindow()
{
	if( GetApp().NVI_GetWindowByID(NVM_GetWindowID()).NVI_GetWindowType() == kWindowType_FindResult )
	{
		GetApp().SPI_GetFindResultWindow(NVM_GetWindowID()).SPI_ShowSaveWindow();
	}
}

//#465
AIndex	AView_Index::SPI_GetContextMenuSelectedRowDBIndex() const
{
	return mContextMenuSelectedRowIndex;
}

//#465
void	AView_Index::SPI_ExportToNewDocument( const AIndex inRowIndex ) const
{
	AText	path, prehit, hit, posthit, position;
	ANumber	start, length, paragraph;
	ABool	isGroup;
	AIndex	groupIndex;
	AText	comment;
	AText	paratext;
	GetIndexDocumentConst().SPI_GetDisplayRowData(inRowIndex,path,hit,prehit,posthit,position,
				start,length,paragraph,groupIndex,isGroup,comment);
	//グループデータ取得
	AFileAcc	baseFolder;
	GetIndexDocumentConst().SPI_GetGroupBaseFolder(groupIndex,baseFolder);
	//
	AText	text;
	//
	AText	title1, title2;
	GetIndexDocumentConst().SPI_GetGroupTitle(groupIndex,title1,title2);
	text.AddText(title1);
	text.Add(kUChar_LineEnd);
	text.AddText(title2);
	text.Add(kUChar_LineEnd);
	//
	for( AIndex index = 0; index < GetIndexDocumentConst().SPI_GetItemCountInGroup(groupIndex); index++ )
	{
		GetIndexDocumentConst().SPI_GetItemData(groupIndex,index,
					path,hit,prehit,posthit,position,paratext,
					start,length,paragraph,comment);
		//
		if( path.GetItemCount() > 0 )
		{
			AFileAcc	file;
			file.Specify(path);
			AText	relativePath;
			file.GetPartialPath(baseFolder,relativePath);
			text.AddText(relativePath);
			text.Add('(');
			text.AddNumber(paragraph+1);
			text.Add(')');
			//
			if( position.GetItemCount() > 0 )
			{
				text.Add('[');
				text.AddText(position);
				text.AddCstring("]: ");
			}
			else
			{
				text.AddCstring(": ");
			}
			//
			if( paratext.GetItemCount() > 0 )
			{
				if( paratext.Get(paratext.GetItemCount()-1) == kUChar_LineEnd )
				{
					//最後の改行コードは削除
					paratext.Delete1(paratext.GetItemCount()-1);
				}
			}
			text.AddText(paratext);
		}
		else
		{
			text.AddText(hit);
		}
		text.Add(kUChar_LineEnd);
	}
	//
	ADocumentID	docID = GetApp().SPNVI_CreateNewTextDocument();
	if( docID == kObjectID_Invalid )   return;//win
	GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
}


