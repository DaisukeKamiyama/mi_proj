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

AView_List

*/

#include "stdafx.h"

#include "../Frame.h"

const AControlID	kEditBoxControlID = 9999;
const ANumber	kEditBoxBottomMargin = 8;

#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kListDefaultFontSize = 12.0;
const AFloatNumber	kListFrameDefaultFontSize = 10.0;
#else
const AFloatNumber	kListDefaultFontSize = 9.0;
const AFloatNumber	kListFrameDefaultFontSize = 9.0;
#endif

//リスト項目アイコンの幅、マージン
const ANumber	kImageIconWidth = 16;
const ANumber	kImageIconLeftMargin = 4;
const ANumber	kImageIconTopMargin = 1;//#1316 0;

//アウトライン１レベル辺りの幅
const AFloatNumber	kOutlineWidthPerLevel = 0.5;

//ヘッダ高さ
const ANumber				kHeaderHeight = 18;

#pragma mark ===========================
#pragma mark [クラス]AView_List
#pragma mark ===========================
//インデックスウインドウのメインView

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_List::AView_List( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) 
: AView(/*#199 inParent,inWindow*/inWindowID,inID), mSelectedRowDisplayIndex(kIndex_Invalid), 
		mCursorRowStartDisplayIndex(kIndex_Invalid),mCursorRowEndDisplayIndex(kIndex_Invalid), mDragDrawn(false), 
		mEnableCursorRow(false), mEnableFileIcon(false), mEnableDragDrop(false), mRowMargin(kDefaultRowMargin), /*#1012 mEnableIcon(false),*/
		mEnableContextMenu(false), mWheelScrollPercent(100), mWheelScrollPercentHighSpeed(100),//R0186, R0000
		mEditMode(false), /*#205 mFrameControlID(kControlID_Invalid),*///win 080618
		mFrameViewID(kObjectID_Invalid),//#205
		/*#688 mMouseTrackByLoop(true),*/ mMouseTrackingMode(kMouseTrackingMode_None)//win 080728
		,mRowHeight(9)
		,mInhibit0Length(true)//#205
//#725,mBackGroundImageIndex(kIndex_Invalid), mBackGroundImageAlpha(0.5)//#291
		,mTransparency(1.0), mAlwaysActiveColors(false)//#291
		,mMultiSelectEnalbed(false)//#237
,mBackgroundColor(kColor_White),mBackgroundColorDeactivate(kColor_White)//#634
,mFontSize(9.0)
,mDisplayRowOrderMode(kDisplayRowOrderMode_Normal)//#871
,mSortMode(false), mSortAscendant(false)//#871
,mCursorRowHoverFolding(false)//#130
,mContextMenuSelectedRowDisplayIndex(kIndex_Invalid)//#892
,mOutlineLevelDataExist(false)//#130
,mVScrollBarControlID(kControlID_Invalid),mHScrollBarControlID(kControlID_Invalid)
,mFilterColumnID(kIndex_Invalid),mSortColumnID(kIndex_Invalid)
,mEnableImageIcon(false)
,mEllipsisFirstCharacters(false)
,mCursorRowLeftMargin(0)
{
	//win SPI_SetTextDrawProperty(AFontWrapper::GetDefaultFont(),12.0,kColor_Black);
	/*win 080707 Initへ移動
	AText	fontname;
	AFontWrapper::GetDefaultFontName(fontname);
	SPI_SetTextDrawProperty(fontname,12.0,kColor_Black);
	mDataBase.CreateTableStart();
	mDataBase.CreateData_TextArray(kFilePathText,"","");
	mDataBase.CreateData_NumberArray(kIconID,"",0,-1,9999999,false);
	mIconDataBase.CreateTableStart();//win 080618
	*/
	NVMC_SetOffscreenMode(true);
}

//デストラクタ
AView_List::~AView_List()
{
}

//Init 080707
void	AView_List::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);//#375
	SPI_SetTextDrawProperty(fontname,/*win 080707 12.0*/kListDefaultFontSize,kColor_Black);
	mDataBase.CreateTableStart();
	//属性列。これらの列は描画されない（mColumnIDArrayに追加されないので描画されない）
	mDataBase.CreateData_TextArray(kFilePathText,"","");
	mDataBase.CreateData_NumberArray(kIconID,"",0,-1,9999999);//#695 ,false);
	mDataBase.CreateData_BoolArray(kEnabled,"",true);//#232
	mDataBase.CreateData_NumberArray(kTextStyle,"",static_cast<ANumber>(kTextStyle_Normal),0,0xFFFF);//#695
	mDataBase.CreateData_ColorArray(kColor,"",kColor_Black);//#695
	mDataBase.CreateData_ColorArray(kBackgroundColor,"",mBackgroundColor);//#695
	mDataBase.CreateData_BoolArray(kBackgroundColorEnabled,"",false);//#695
	mDataBase.CreateData_BoolArray(kRowEditable,"",true);//#695
	mDataBase.CreateData_NumberArray(kOutlineFoldingLevel,"",0,0,9999);//#130
	mDataBase.CreateData_BoolArray(kOutlineFoldingCollapsed,"",false);//#130
	mDataBase.CreateData_NumberArray(kImageIconID,"",kImageID_Invalid,-1,9999999);//#725
	//
	//#1012 mIconDataBase.CreateTableStart();//win 080618
	//#138
	NVIDO_ListInit();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_List::NVIDO_DoDeleted()
{
	SPI_EndEditMode();
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

/**
ListフレームView取得
*/
AView_ListFrame&	AView_List::GetFrameView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_ListFrame,kViewType_ListFrame,mFrameViewID);
}

#pragma mark ===========================

void	AView_List::SPI_SetEnableCursorRow()
{
	mEnableCursorRow = true;
}

void	AView_List::SPI_SetEnableFileIcon( const ABool inEnable )
{
	mEnableFileIcon = inEnable;
}

/*#1012
void	AView_List::SPI_SetEnableIcon( const ABool inEnable )
{
	mEnableIcon = inEnable;
}
*/

/**
項目アイコンEnable/disable設定
*/
void	AView_List::SPI_SetEnableImageIcon( const ABool inEnable )
{
	mEnableImageIcon = inEnable; 
}

void	AView_List::SPI_SetEnableDragDrop( const ABool inEnable )//B0319
{
	mEnableDragDrop = inEnable;
	//#236
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_ListViewItem);
	scrapTypeArray.Add(kScrapType_File);//win
	NVMC_EnableDrop(scrapTypeArray);
}

void	AView_List::SPI_SetRowMargin( const ANumber inRowMargin )
{
	mRowMargin = inRowMargin;
	AText	fontname;//win
	fontname.SetText(mFontName);//win
	SPI_SetTextDrawProperty(/*win mFont*/fontname,mFontSize,mColor);
}

//R0186
void	AView_List::SPI_SetEnableContextMenu( const ABool inEnable, const AMenuItemID inMenuItemID )
{
	mEnableContextMenu = inEnable;
	mContextMenuItemID = inMenuItemID;
}

//R0000
void	AView_List::SPI_SetWheelScrollPercent( const ANumber inNormal, const ANumber inHighSpeed )
{
	mWheelScrollPercent = inNormal;
	mWheelScrollPercentHighSpeed = inHighSpeed;
}

/*#205
//win 080618
void	AView_List::SPI_SetFrameControlID( const AControlID inFrameControlID )
{
	mFrameControlID = inFrameControlID;
}
*/

//#205
/**
ListフレームのViewIDを設定する

本クラス外でListフレームを生成した場合用。
AWindowのTableView機構では、AWindowでListフレームを生成している。
*/
void	AView_List::SPI_SetFrameViewID( const AViewID inViewID )
{
	mFrameViewID = inViewID;
}

//#205
/**
Listフレームを生成する

本ListViewの周りに、Listフレームを生成する。
本ListViewは指定Margin分だけ小さくなる。
*/
void	AView_List::SPI_CreateFrame( const ABool inCreateHeader, const ABool inCreateVScrollBar, const ABool inCreateHScrollBar,
		const ANumber inLeftMargin, const ANumber inTopMargin, const ANumber inRightMargin, const ANumber inBottomMargin )
{
	//ListViewの元の位置・サイズを取得し、ListViewのサイズは縮小する。
	AWindowPoint	origPoint = NVI_GetPosition();
	ANumber	origWidth = NVI_GetWidth();
	ANumber	origHeight = NVI_GetHeight();
	AWindowPoint	insetPoint;
	insetPoint.x = origPoint.x + inLeftMargin;
	insetPoint.y = origPoint.y;
	ANumber	insetWidth = origWidth - inLeftMargin - inRightMargin;
	ANumber	insetHeight = origHeight - inBottomMargin;
	//フレーム分だけ、自分のサイズを小さくする
	if( inCreateHeader == true )
	{
		insetPoint.y += kHeaderHeight;
		insetHeight -= kHeaderHeight;
	}
	else
	{
		insetPoint.y += inTopMargin;
		insetHeight -= inTopMargin;
	}
	NVM_GetWindow().NVI_SetControlPosition(NVI_GetControlID(),insetPoint);
	//
	ANumber	listviewWidth = insetWidth;
	ANumber	listviewHeight = insetHeight;
	//ListViewが存在するTabControlのIndexを取得
	AIndex	tabControlIndex = NVM_GetWindow().NVI_GetControlEmbeddedTabControlIndex(NVI_GetControlID());
	//Frame生成
	AControlID	frameControlID = NVM_GetWindow().NVM_AssignDynamicControlID();
	AListFrameViewFactory	frameViewFactory(NVM_GetWindowID(),frameControlID,NVI_GetControlID(),inCreateHeader,kControlID_Invalid,kControlID_Invalid);
	mFrameViewID = NVM_GetWindow().NVI_CreateView(frameControlID,origPoint,origWidth,origHeight,kControlID_Invalid,
				NVI_GetControlID(),false,frameViewFactory,tabControlIndex,false);
	//フレーム内にembedする win
	NVM_GetWindow().NVI_EmbedControl(frameControlID,NVI_GetControlID());
	//引数によってスクロールバーを生成する
	if( inCreateVScrollBar == true )
	{
		//ListViewの元の位置・サイズを取得し、ListViewのサイズは縮小する。
		AWindowPoint	pt;
		pt.x = insetPoint.x + insetWidth - kVScrollBarWidth;
		pt.y = insetPoint.y;
		ANumber	width = kVScrollBarWidth;
		ANumber	height = insetHeight;
		if( inCreateHScrollBar == true )
		{
			height -= kHScrollBarHeight;
		}
		mVScrollBarControlID = NVM_GetWindow().NVM_AssignDynamicControlID();
		//Vスクロールバー生成
		NVM_GetWindow().NVI_CreateScrollBar(mVScrollBarControlID,pt,width,height,tabControlIndex);
		//Vスクロールバー分だけlistviewのサイズを縮める
		listviewWidth -= kVScrollBarWidth;
		//フレーム内にembedする win
		NVM_GetWindow().NVI_EmbedControl(frameControlID,mVScrollBarControlID);
	}
	if( inCreateHScrollBar == true )
	{
		//ListViewの元の位置・サイズを取得し、ListViewのサイズは縮小する。
		AWindowPoint	pt;
		pt.x = insetPoint.x;
		pt.y = insetPoint.y + insetHeight - kHScrollBarHeight;
		ANumber	width = insetWidth;
		if( inCreateVScrollBar == true )
		{
			width -= kVScrollBarWidth;
		}
		ANumber	height = kHScrollBarHeight;
		mHScrollBarControlID = NVM_GetWindow().NVM_AssignDynamicControlID();
		//Hスクロールバー生成
		NVM_GetWindow().NVI_CreateScrollBar(mHScrollBarControlID,pt,width,height,tabControlIndex);
		//Hスクロールバー分だけlistviewのサイズを縮める
		listviewHeight -= kHScrollBarHeight;
		//フレーム内にembedする win
		NVM_GetWindow().NVI_EmbedControl(frameControlID,mHScrollBarControlID);
	}
	//
	NVI_SetSize(listviewWidth,listviewHeight);
	//スクロールバーへのリンクを設定
	NVI_SetScrollBarControlID(mHScrollBarControlID,mVScrollBarControlID);
}

/**
bindings設定
*/
void	AView_List::SPI_SetControlBindings( const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
		const ABool inWidthFixed, const ABool inHeightFixed )
{
	if( mFrameViewID != kObjectID_Invalid )
	{
		//------------------フレーム有りの場合------------------
		//Frame viewの方にBind設定する
		NVM_GetWindow().NVI_SetControlBindings(GetFrameView().NVI_GetControlID(),
					inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
		//Hスクロールバーのbindings設定
		if( mHScrollBarControlID != kControlID_Invalid )
		{
			NVM_GetWindow().NVI_SetControlBindings(mHScrollBarControlID,true,false,true,true,false,true);
		}
		//Vスクロールバーのbindings設定
		if( mVScrollBarControlID != kControlID_Invalid )
		{
			NVM_GetWindow().NVI_SetControlBindings(mVScrollBarControlID,false,true,true,true,true,false);
		}
		//自身はフレームビューの左右上下にbindingする。（サイズはフレキシブル）
		NVMC_SetControlBindings(true,true,true,true,false,false);
	}
	else
	{
		//------------------フレーム無しの場合------------------
		//そのまま設定
		NVMC_SetControlBindings(inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
	}
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//描画要求時のコールバック(AView用)
void	AView_List::EVTDO_DoDraw()
{
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	ALocalRect	localFrame;
	NVM_GetLocalViewRect(localFrame);
	
	/*#1316 AView_Listでは背景描画しない。（Frame有りの場合はFrameで背景描画、無しの場合は透明）
	//==================背景データ取得（このviewでは背景描画しない）==================
	
	//背景色取得
	AColor	backgroundColor = mBackgroundColor;
	if( NVM_GetWindow().NVI_IsWindowActive() == false )
	{
		backgroundColor = mBackgroundColorDeactivate;
	}
	*/
	//背景描画
	//NVMC_EraseRect(localFrame);//Windows用（透明色描画）
	//NVMC_PaintRect(localFrame,backgroundColor,mTransparency);//#291
	
	
	//==================ホバー描画==================
	if( mEnableCursorRow == true &&
	   mCursorRowStartDisplayIndex != kIndex_Invalid && mCursorRowEndDisplayIndex != kIndex_Invalid )
	{
		AImageRect	hoverImageRect = {0};
		GetRowImageRect(mCursorRowStartDisplayIndex,mCursorRowEndDisplayIndex+1,hoverImageRect);
		ALocalRect	hoverLocalRect = {0};
		NVM_GetLocalRectFromImageRect(hoverImageRect,hoverLocalRect);
		hoverLocalRect.left		+= 2 + mCursorRowLeftMargin;
		hoverLocalRect.top		+= 2;
		hoverLocalRect.right	-= 2;
		hoverLocalRect.bottom	-= 2;
		NVMC_FrameRoundedRect(hoverLocalRect,AColorWrapper::GetControlAccentColor(),0.7,
							  3,true,true,true,true,true,true,true,true,1.0);
	}
	
	//==================各行描画==================
	
	AImageRect	rect;
	NVM_GetImageViewRect(rect);
	AIndex	startRow = rect.top / GetRowHeight();
	AIndex	endRow = rect.bottom / GetRowHeight();
	
	//#232 ABool	enabled =  ((NVM_GetWindow().IsWindowActive() == true) || (NVM_GetWindow().NVI_IsFloatingWindow() == true))&&(NVMC_IsControlEnabled());
	AItemCount	rowDisplayCount = SPI_GetDisplayRowCount();//#798 GetRowCount();
	for( AIndex rowDisplayIndex = startRow; rowDisplayIndex <= endRow; rowDisplayIndex++ )
	{
		AImageRect	rowImageRect;
		GetRowImageRect(rowDisplayIndex,rowDisplayIndex+1,rowImageRect);
		ALocalRect	rowLocalRect;
		NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
		if( NVMC_IsRectInDrawUpdateRegion(rowLocalRect) == false )   continue;
		
		//行表示クリア
		//NVMC_PaintRect(rowLocalRect,kColor_White,1.0);
		
		if( rowDisplayIndex >= rowDisplayCount )   break;
		
		AIndex	DBRowIndex = mRowOrderDisplayToDBArray.Get(rowDisplayIndex);
		
		//#232 行のEnable/Disable
		ABool	enabled =  ((NVM_GetWindow().NVI_IsWindowActive() == true) || 
				(NVM_GetWindow().NVI_IsFloating() == true))&&(NVMC_IsControlEnabled());
		if( mAlwaysActiveColors == true )//#291
		{
			enabled = true;
		}
		if( enabled == true )
		{
			enabled = mDataBase.GetData_BoolArray(kEnabled,DBRowIndex);;
		}
		
		//#237
		//選択行かどうかの判定
		ABool	selected = (rowDisplayIndex==mSelectedRowDisplayIndex);
		if( mSelectedRowDisplayIndexArray.GetItemCount() > 0 )
		{
			if( mSelectedRowDisplayIndexArray.Find(rowDisplayIndex) != kIndex_Invalid )
			{
				selected = true;
			}
		}
		
		//==================Selection描画==================
		if( selected == true )//#237
		{
			AFloatNumber	alpha = 0.7;
			if( AApplication::GetApplication().NVI_IsDarkMode() == true )
			{
				alpha = 0.4;
			}
			NVMC_PaintRect(rowLocalRect,AColorWrapper::GetControlAccentColor(),alpha);
			/*#1316
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
			NVMC_PaintRect(rect,selColor,1.0);
			*/
		}
		
		/*
		//背景色 #427
		if( selected == false )
		{
			if( mDataBase.GetData_BoolArray(kBackgroundColorEnabled,DBRowIndex) == true )
			{
				AColor	c = mBackgroundColor;
				mDataBase.GetData_ColorArray(kBackgroundColor,DBRowIndex,c);
				NVMC_PaintRect(rowLocalRect,c,1.0);
			}
		}
		*/
		
		ANumber	imagex = kRowLeftMargin;
		
		//#130
		//==================foldingアイコン描画有無およびアウトラインレベル余白領域取得==================
		ALocalRect	foldingRect = {0};
		ABool	hasFolding = GetOutlineFoldingAndOutlineMarginRect(rowDisplayIndex,foldingRect);
		
		//==================Folding三角アイコン描画==================
		if( hasFolding == true )
		{
			//表示位置取得
			ALocalPoint	pt = {0};
			pt.x = foldingRect.right - 18;
			pt.y = (foldingRect.top+foldingRect.bottom)/2 - 6;
			//折りたたみ状態による分岐
			if( mDataBase.GetData_BoolArray(kOutlineFoldingCollapsed,DBRowIndex) == true )
			{
				//折りたたみ中
				if( rowDisplayIndex >= mCursorRowStartDisplayIndex && rowDisplayIndex <= mCursorRowEndDisplayIndex &&
				   mCursorRowHoverFolding == true )
				{
					//ホバー
					NVMC_DrawImage(kImageID_barSwCursorRight_h,pt);
				}
				else
				{
					//通常
					NVMC_DrawImage(kImageID_barSwCursorRight,pt);
				}
			}
			else
			{
				//折りたたみ解除中
				if( rowDisplayIndex >= mCursorRowStartDisplayIndex && rowDisplayIndex <= mCursorRowEndDisplayIndex &&
				   mCursorRowHoverFolding == true )
				{
					//ホバー
					NVMC_DrawImage(kImageID_barSwCursorDown_h,pt);
				}
				else
				{
					//通常
					NVMC_DrawImage(kImageID_barSwCursorDown,pt);
				}
			}
		}
		//X位置をfoldingアイコンの右へ
		imagex += foldingRect.right - foldingRect.left - kCellLeftMargin - kCellRightMargin;
		//==================ファイルアイコン表示==================
		if( mEnableFileIcon == true )
		{
			AText	path;
			mDataBase.GetData_TextArray(kFilePathText,DBRowIndex,path);
			AFileAcc	file;
			file.Specify(path);
			AImageRect	cellImageRect = rowImageRect;
			cellImageRect.left		= imagex;
			cellImageRect.right		= imagex + GetRowHeight() +kCellLeftMargin +kCellRightMargin;
			ALocalRect	drawRect = {0};
			NVM_GetLocalRectFromImageRect(cellImageRect,drawRect);
			drawRect.top += mRowMargin/2 -1;//#232
			drawRect.left += kCellLeftMargin;
			drawRect.right = drawRect.left + (drawRect.bottom-drawRect.top);
			if( file.Exist() == true )//#1316
			{
				NVMC_DrawIconFromFile(drawRect,file,enabled);
			}
			imagex = cellImageRect.right;
		}
		/*#1012
		//==================レガシーアイコン表示==================
		if( mEnableIcon == true )
		{
			AIconID	iconID = mDataBase.GetData_NumberArray(kIconID,DBRowIndex);
			AImageRect	cellImageRect = rowImageRect;
			cellImageRect.left		= imagex;
			cellImageRect.right		= imagex + GetRowHeight() +kCellLeftMargin +kCellRightMargin;
			ALocalRect	drawRect = {0};
			NVM_GetLocalRectFromImageRect(cellImageRect,drawRect);
			drawRect.top += mRowMargin/2 -1;//#232
			drawRect.left += kCellLeftMargin;
			drawRect.right = drawRect.left + (drawRect.bottom-drawRect.top);
			NVMC_DrawIcon(drawRect,iconID,enabled);
			imagex = cellImageRect.right;
		}
		*/
		//#725
		//==================項目アイコン表示==================
		if( mEnableImageIcon == true )
		{
			AImageID	imageIconID = mDataBase.GetData_NumberArray(kImageIconID,DBRowIndex);
			if( imageIconID != kImageID_Invalid )
			{
				AImagePoint	imagept = {imagex+kImageIconLeftMargin,rowImageRect.top+kImageIconTopMargin};
				ALocalPoint	pt = {0};
				NVM_GetLocalPointFromImagePoint(imagept,pt);
				NVMC_DrawImage(imageIconID,pt);
				imagex = imagept.x + kImageIconWidth;
			}
		}
		//==================各列ごとのループ==================
		for( AIndex colIndex = 0; colIndex < mColumnIDArray.GetItemCount(); colIndex++ )
		{
			//
			APrefID	colID = mColumnIDArray.Get(colIndex);
			AImageRect	cellImageRect = rowImageRect;
			cellImageRect.left		= imagex;
			cellImageRect.right		= imagex + mColumnWidthArray.Get(colIndex);
			//最後の列は右端まで
			if( colIndex == mColumnIDArray.GetItemCount() - 1 )
			{
				cellImageRect.right = rowImageRect.right;
			}
			//描画領域取得
			ALocalRect	drawRect = {0};
			NVM_GetLocalRectFromImageRect(cellImageRect,drawRect);
			drawRect.top += mRowMargin/2 + 0.1;//#1316 -1;
			drawRect.left += kCellLeftMargin;
			drawRect.right -= kCellRightMargin;
			drawRect.bottom += 32;
			//
			switch(mDataBase.GetDataType(colID))
			{
				//==================テキスト列==================
			  case kDataType_TextArray:
				{
					//------------------アイコン取得------------------
					/*#1012
					if( mIconDataBase.GetItemCount_NumberArray(colID) > 0 )
					{
						AIconID	iconID = mIconDataBase.GetData_NumberArray(colID,DBRowIndex);
						if( iconID != kIndex_Invalid )
						{
							//
							ALocalRect	iconDrawRect = {0};
							NVM_GetLocalRectFromImageRect(cellImageRect,iconDrawRect);
							iconDrawRect.top	+= mRowMargin/2 -1;//#232
							iconDrawRect.left	+= kCellLeftMargin;
							iconDrawRect.right	= drawRect.left + (iconDrawRect.bottom-iconDrawRect.top);
							NVMC_DrawIcon(iconDrawRect,iconID,true,true);//#232
							//
						}
						drawRect.left += GetRowHeight();
					}
					*/
					
					//------------------テキスト取得------------------
					//テキスト取得
					AText	text;
					mDataBase.GetData_TextArray(colID,DBRowIndex,text);
					//Canonical compに変換
					text.ConvertToCanonicalComp();
					/*
					AIndex	outlineLevel = mDataBase.GetData_NumberArray(kOutlineFoldingLevel,DBRowIndex);
					AText	t;
					t.SetNumber(outlineLevel);
					text.InsertText(0,t);
					*/
					//色取得
					AColor	color = kColor_List_Normal;//#1316 kColor_Black;
					mDataBase.GetData_ColorArray(kColor,DBRowIndex,color);//#695
					//#1316
					//kColor_List_カラーをダークモードかどうかに従って、実際の色に変換
					if( AColorWrapper::CompareColor(color,kColor_List_Normal) == true )
					{
						if( selected == true )
						{
							color = kColor_White;
						}
						else
						{
							color = AColorWrapper::GetColorByHTMLFormatColor("303030");
							if( AApplication::GetApplication().NVI_IsDarkMode() == true )
							{
								color = AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
							}
						}
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Blue) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("0000FF");
						if( AApplication::GetApplication().NVI_IsDarkMode() == true )
						{
							color = AColorWrapper::GetColorByHTMLFormatColor("00C0FF");
						}
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Red) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("FF0000");
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Pink) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("FF00C3");
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Gray) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("757575");
					}
					
					//スタイル取得
					ATextStyle	style = static_cast<ATextStyle>(mDataBase.GetData_NumberArray(kTextStyle,DBRowIndex));//#695
					//選択行はボールドに #1316
					if( selected == true )
					{
						style |= kTextStyle_Bold;
					}
					/*#1316
					if( selected == true )
					{
						//------------------選択行の場合------------------
						//文字色が黒の場合は、白にする
						if( AColorWrapper::CompareColor(color,kColor_Black) == true )//R0206
						{
							color = kColor_White;
						}
						//文字色が黒以外の場合は、Sを下げ、Vをあげる
						else
						{
							color = AColorWrapper::ChangeHSV(color,0,0.05,100000);
						}
						//ドロップシャドウ色設定
						NVMC_SetDropShadowColor(kColor_Gray30Percent);
						//boldに設定
						//#1089 style |= kTextStyle_Bold;
					}
					*/
					//#1428
					//設定データの文字列長が0のときは代替テキストを表示する
					if( text.GetItemCount() == 0 )
					{
						text.SetText(m0LengthText);
						color = AColorWrapper::GetColorByHTMLFormatColor("808080");
						style = kTextStyle_Italic;
					}
					//テキストプロパティ設定
					NVMC_SetDefaultTextProperty(mFontName,mFontSize,color,style,true);
					//ellipsisテキスト取得
					AText	ellipsisText;
					if( mEllipsisFirstCharacters == true )
					{
						NVI_GetEllipsisTextWithFixedFirstCharacters(text,drawRect.right-drawRect.left,3,ellipsisText);
					}
					else
					{
						NVI_GetEllipsisTextWithFixedLastCharacters(text,drawRect.right-drawRect.left,3,ellipsisText,true);
					}
					//テキスト描画
					NVMC_DrawText(drawRect,ellipsisText,color,style);
					break;
				}
				//==================番号列==================
			  case kDataType_NumberArray:
				{
					//テキスト取得
					AText	text;
					text.SetNumber(mDataBase.GetData_NumberArray(colID,DBRowIndex));
					//色取得
					AColor	color = kColor_Black;
					mDataBase.GetData_ColorArray(kColor,DBRowIndex,color);//#695
					//#1316
					//kColor_List_カラーをダークモードかどうかに従って、実際の色に変換
					if( AColorWrapper::CompareColor(color,kColor_List_Normal) == true )
					{
						if( selected == true )
						{
							color = kColor_White;
						}
						else
						{
							color = AColorWrapper::GetColorByHTMLFormatColor("303030");
							if( AApplication::GetApplication().NVI_IsDarkMode() == true )
							{
								color = AColorWrapper::GetColorByHTMLFormatColor("C0C0C0");
							}
						}
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Blue) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("0000FF");
						if( AApplication::GetApplication().NVI_IsDarkMode() == true )
						{
							color = AColorWrapper::GetColorByHTMLFormatColor("00C0FF");
						}
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Red) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("FF0000");
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Pink) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("FF00C3");
					}
					else if( AColorWrapper::CompareColor(color,kColor_List_Gray) == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("757575");
					}
					
					//スタイル取得
					ATextStyle	style = static_cast<ATextStyle>(mDataBase.GetData_NumberArray(kTextStyle,DBRowIndex));//#695
					//選択行はボールドに #1316
					if( selected == true )
					{
						style |= kTextStyle_Bold;
					}
					/*#1316
					if( selected == true )//#237
					{
						//------------------選択行の場合------------------
						//文字色が黒の場合は、白にする
						if( AColorWrapper::CompareColor(color,kColor_Black) == true )//R0206
						{
							color = kColor_White;
						}
						//文字色が黒以外の場合は、Sを下げ、Vをあげる
						else
						{
							color = AColorWrapper::ChangeHSV(color,0,0.05,100000);
						}
						//ドロップシャドウ色設定
						NVMC_SetDropShadowColor(kColor_Gray30Percent);
						//boldに設定
						style |= kTextStyle_Bold;
					}
					*/
					//テキスト描画
					NVMC_DrawText(drawRect,text,color,style);
					break;
				}
			  case kDataType_BoolArray:
				{
					//#1012 AIconID	iconID = kIconID_CheckBoxOn;
					AImageID	imageID = kImageID_iconChecked;//#1012
					if( mDataBase.GetData_BoolArray(colID,DBRowIndex) == false )
					{
						//#1012 iconID = kIconID_CheckBoxOff;
						imageID = kImageID_iconUnchecked;//#1012
					}
					ALocalRect	drawRect = {0};
					NVM_GetLocalRectFromImageRect(cellImageRect,drawRect);
					drawRect.top += (drawRect.bottom-drawRect.top)/2 - 8;//#1316 mRowMargin/2 -1;
					drawRect.left += kCellLeftMargin;
					drawRect.right = drawRect.left + (drawRect.bottom-drawRect.top);
					drawRect.bottom = drawRect.top + 16;//#1316
					//#1012 NVMC_DrawIcon(drawRect,iconID,enabled,true);//#232
					ALocalPoint	pt = {0};
					pt.x = drawRect.left;
					pt.y = drawRect.top;
					NVMC_DrawImage(imageID,pt);
					break;
				}
			  default:
				{
					_ACError("",NULL);
					break;
				}
			}
			imagex += mColumnWidthArray.Get(colIndex);
		}
	}
	//#871
	//==================Drop線描画==================
	if( mDragDrawn == true && mDragCaretRowDisplayIndex != mSelectedRowDisplayIndex && mDragCaretRowDisplayIndex != mSelectedRowDisplayIndex+1 )
	{
		//色取得
		AColor	dragColor = AColorWrapper::GetColorByHTMLFormatColor("526cdc");
		//線描画
		AImageRect	imageframe = {0};
		NVM_GetImageViewRect(imageframe);
		AImagePoint	caretImageStart, caretImageEnd;
		caretImageStart.y = caretImageEnd.y = mDragCaretRowDisplayIndex*GetRowHeight();
		caretImageStart.x = imageframe.left + 9;
		caretImageEnd.x = imageframe.right - 2;
		ALocalPoint	caretLocalStart = {0}, caretLocalEnd = {0};
		NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
		NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
		NVMC_DrawLine(caretLocalStart,caretLocalEnd,dragColor,1.0,0,2.0);
		//丸描画
		ALocalRect	circleRect = {0};
		circleRect.left		= caretLocalStart.x -7;
		circleRect.top		= caretLocalStart.y - 3;
		circleRect.right	= caretLocalStart.x;
		circleRect.bottom	= caretLocalStart.y + 4;
		NVMC_DrawCircle(circleRect,dragColor,2);
	}
}

//#130
/**
foldingアイコン描画有無およびアウトラインレベル余白領域取得
*/
ABool	AView_List::GetOutlineFoldingAndOutlineMarginRect( const AIndex inDisplayRowIndex, ALocalRect& outRect ) const
{
	//行rect取得
	AImageRect	rowImageRect = {0};
	GetRowImageRect(inDisplayRowIndex,inDisplayRowIndex+1,rowImageRect);
	
	//アウトラインデータ無しのときは、左余白なし
	if( mOutlineLevelDataExist == false )
	{
		AImageRect	foldingImageRect = rowImageRect;
		foldingImageRect.right = kRowLeftMargin;
		NVM_GetLocalRectFromImageRect(foldingImageRect,outRect);
		return false;
	}
	
	//アウトラインレベル取得
	AIndex	DBRowIndex = mRowOrderDisplayToDBArray.Get(inDisplayRowIndex);
	AIndex	outlineLevel = mDataBase.GetData_NumberArray(kOutlineFoldingLevel,DBRowIndex);
	//アウトラインレベル余白領域取得
	AImageRect	foldingImageRect = rowImageRect;
	foldingImageRect.right = kRowLeftMargin + 11 + outlineLevel*kOutlineWidthPerLevel;
	NVM_GetLocalRectFromImageRect(foldingImageRect,outRect);
	//foldingアイコン描画有無判定
	if( HasFoldingChild(DBRowIndex) == true )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//マウスボタン押下時のコールバック(AView用)
ABool	AView_List::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//コントロールがdisableなら何もせず終了
	if( NVMC_IsControlEnabled() == false )
	{
		return false;
	}
	
	//自ViewがActiveでない場合は、Windowへフォーカスの移動を依頼する
	if( NVI_IsFocusActive() == false )
	{
		NVM_GetWindow().NVI_SwitchFocusTo(NVI_GetControlID());
	}
	
	//==================クリック位置取得==================
	AImagePoint	clickImagePoint = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	//クリック行取得
	AIndex	clickRowDisplayIndex = clickImagePoint.y/GetRowHeight();
	if( clickRowDisplayIndex >= SPI_GetDisplayRowCount() )
	{
		//行の最後より後をクリックした場合は、選択解除
		SPI_SetSelect(kIndex_Invalid);
		//AWindowのテーブル選択状態を解除するためにEVT_Clicked()をコールする
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
		return true;
	}
	
	//#130
	//==================Foldingアイコンクリック==================
	ALocalRect	foldingRect = {0};
	if( clickRowDisplayIndex >= 0 && clickRowDisplayIndex < SPI_GetDisplayRowCount() )
	{
		ABool	hasFolding = GetOutlineFoldingAndOutlineMarginRect(clickRowDisplayIndex,foldingRect);
		if( hasFolding == true )
		{
			if( IsPointInRect(inLocalPoint,foldingRect) == true )
			{
				//------------------Foldingクリック時処理------------------
				if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
				{
					//同じレベルを全て折りたたみ・展開
					SPI_ExpandCollapseAllForSameLevel(clickRowDisplayIndex);
				}
				else
				{
					//折りたたみ・展開
					SPI_ExpandCollapse(clickRowDisplayIndex,false);
				}
				//ホバー更新
				UpdateHover(inLocalPoint,inModifierKeys,true);
				//
				NVM_GetWindow().OWICB_ListViewExpandedCollapsed(NVI_GetControlID());
				return true;
			}
		}
	}
	
	//==================選択==================
	//------------------描画更新用に、現在の選択項目を記憶する。------------------
	//B0303のついで（リスト選択処理高速化）
	AIndex	svSelectedRowIndex = mSelectedRowDisplayIndex;
	AIndex	svCursorStartRowIndex = mCursorRowStartDisplayIndex;
	AIndex	svCursorEndRowIndex = mCursorRowEndDisplayIndex;
	//#237
	AArray<AIndex>	svSelectedRowArray;
	for( AIndex i = 0; i < mSelectedRowDisplayIndexArray.GetItemCount(); i++ )
	{
		svSelectedRowArray.Add(mSelectedRowDisplayIndexArray.Get(i));
	}
	//------------------選択項目を更新（単一項目）------------------
	if( NVM_IsImagePointInViewRect(clickImagePoint) == true )
	{
		mSelectedRowDisplayIndex = clickRowDisplayIndex;
		if( mSelectedRowDisplayIndex < 0 || mSelectedRowDisplayIndex >= GetRowCount() )
		{
			mSelectedRowDisplayIndex = kIndex_Invalid;
		}
		//#232 行がdisabledなら選択解除する
		if( mSelectedRowDisplayIndex != kIndex_Invalid )
		{
			if( mDataBase.GetData_BoolArray(kEnabled,mSelectedRowDisplayIndex) == false )
			{
				mSelectedRowDisplayIndex = kIndex_Invalid;
			}
		}
	}
	else
	{
		mSelectedRowDisplayIndex = kIndex_Invalid;
	}
	mCursorRowStartDisplayIndex = kIndex_Invalid;
	mCursorRowEndDisplayIndex = kIndex_Invalid;
	mCursorRowLeftMargin = 0;
	
	//------------------選択項目を更新（複数項目）------------------
	//#237 複数選択
	if( mMultiSelectEnalbed == true )
	{
		//Shiftキーによる複数選択
		if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true )
		{
			if( mSelectedRowDisplayIndexArray.GetItemCount() == 0 )
			{
				mSelectedRowDisplayIndexArray.Add(mSelectedRowDisplayIndex);
			}
			else
			{
				AIndex	first = mSelectedRowDisplayIndexArray.Get(0);
				AIndex	cur = mSelectedRowDisplayIndex;
				mSelectedRowDisplayIndexArray.DeleteAll();
				if( first <= cur )
				{
					for( AIndex i = first; i <= cur; i++ )
					{
						mSelectedRowDisplayIndexArray.Add(i);
					}
				}
				else
				{
					for( AIndex i = first; i >= cur; i-- )
					{
						mSelectedRowDisplayIndexArray.Add(i);
					}
				}
			}
		}
		//Ctrlキーによる複数選択
		else if( AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
		{
			if( mSelectedRowDisplayIndexArray.Find(mSelectedRowDisplayIndex) == kIndex_Invalid )
			{
				mSelectedRowDisplayIndexArray.Add(mSelectedRowDisplayIndex);
			}
		}
		//複数選択解除（１つ選択）
		else
		{
			mSelectedRowDisplayIndexArray.DeleteAll();
			mSelectedRowDisplayIndexArray.Add(mSelectedRowDisplayIndex);
		}
	}
	
	//win 080618
	//クリックしたらそれまでの編集モードは解除
	if( mEditMode == true )
	{
		SPI_EndEditMode();
	}
	
	//==================描画更新==================
	//B0303のついで（リスト選択処理高速化）NVI_Refresh();
	//B0303のついで（リスト選択処理高速化）
	//
	SPI_RefreshRow(svSelectedRowIndex);
	SPI_RefreshRow(svCursorStartRowIndex);
	SPI_RefreshRow(svCursorEndRowIndex);
	if( svSelectedRowArray.GetItemCount() > 0 )//#237
	{
		for( AIndex i = 0; i < svSelectedRowArray.GetItemCount(); i++ )
		{
			SPI_RefreshRow(svSelectedRowArray.Get(i));
		}
	}
	//
	SPI_RefreshRow(mSelectedRowDisplayIndex);
	if( mSelectedRowDisplayIndexArray.GetItemCount() > 0 )//#237
	{
		for( AIndex i = 0; i < mSelectedRowDisplayIndexArray.GetItemCount(); i++ )
		{
			SPI_RefreshRow(mSelectedRowDisplayIndexArray.Get(i));
		}
	}
	
	//------------------AWindowでの処理------------------
	if( inClickCount == 1 )
	{
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),inModifierKeys);
	}
	else if( inClickCount == 2 )
	{
		NVM_GetWindow().EVT_DoubleClicked(NVI_GetControlID());
	}
	
	//------------------選択解除時処理------------------
	if( mSelectedRowDisplayIndex == kIndex_Invalid )
	{
		EVTDO_ListClicked(kIndex_Invalid,kIndex_Invalid,inClickCount);//#199
		return false;
	}
	
	//win 080618
	//クリックされた列のindexを取得
	AIndex	DBRowIndex = mRowOrderDisplayToDBArray.Get(mSelectedRowDisplayIndex);
	
	EVTDO_ListClicked(mSelectedRowDisplayIndex,DBRowIndex,inClickCount);//#199
	//
	AIndex colIndex = 0;
	ANumber	w = 0;
	for( ; colIndex < mColumnIDArray.GetItemCount()-1; colIndex++ )
	{
		w += mColumnWidthArray.Get(colIndex);
		if( clickImagePoint.x < w )
		{
			break;
		}
	}
	//Edit
	//win 080618
	mLastClickedColumnID = mColumnIDArray.Get(colIndex);
	if( mColumnEditable.Get(colIndex) == true )
	{
		//行編集可能判定 #427
		/*#695
		ABool	rowEditable = true;
		if( mSelectedRowDisplayIndex < mRowEditableArray.GetItemCount() )
		{
			if( mRowEditableArray.Get(mSelectedRowDisplayIndex) == false )
			{
				rowEditable = false;
			}
		}
		*/
		ABool	rowEditable = mDataBase.GetData_BoolArray(kRowEditable,mSelectedRowDisplayIndex);//#695
		if( rowEditable == true )
		{
			switch(mDataBase.GetDataType(mLastClickedColumnID))
			{
			  case kDataType_BoolArray:
				{
					//On/Offをトグルする
					mDataBase.SetData_BoolArray(mLastClickedColumnID,DBRowIndex,(mDataBase.GetData_BoolArray(mLastClickedColumnID,DBRowIndex)==false));
					NVM_GetWindow().EVT_ValueChanged(NVI_GetControlID());
					SPI_RefreshRow(mSelectedRowDisplayIndex);
					break;
				}
			  case kDataType_TextArray:
			  case kDataType_NumberArray://#868
				{
					//編集モードへ移行
					if( inClickCount == 2 )
					{
						SPI_StartEditMode(mColumnIDArray.Get(colIndex),mSelectedRowDisplayIndex);
					}
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
		}
	}
	
	if( mEnableDragDrop == true )
	{
		//ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
		//TrackingMode設定
		mMouseTrackingMode = kMouseTrackingMode_SingleClick;
		mMouseTrackStartImagePoint = clickImagePoint;
		mMouseTrackResultIsDrag = false;
		NVM_SetMouseTrackingMode(true);
		return true;
	}
	return true;
}

void	AView_List::StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mMouseTrackResultIsDrag == true )
	{
		//Drag領域設定
		AArray<ALocalRect>	dragRect;
		AImageRect	rowImageRect;
		GetRowImageRect(mSelectedRowDisplayIndex,mSelectedRowDisplayIndex+1,rowImageRect);
		ALocalRect	rowLocalRect;
		NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
		dragRect.Add(rowLocalRect);
		//Scrap設定
		AArray<AScrapType>	scrapType;
		ATextArray	data;
		scrapType.Add(kScrapType_ListViewItem);
		AText	text;
		data.Add(text);
		//Drag実行
		NVMC_DragText(inLocalPoint,dragRect,scrapType,data);
		//
		NVI_Refresh();
	}
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_List::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	switch(mMouseTrackingMode)
	{
	  case kMouseTrackingMode_SingleClick:
		{
			//#871
			//row orderモードが通常、かつ、複数選択ではない場合のみ（ソートやフィルター以外）、ドラッグ開始
			if( mDisplayRowOrderMode == kDisplayRowOrderMode_Normal && mSelectedRowDisplayIndexArray.GetItemCount() <= 1 )
			{
				//
				if( mouseImagePoint.x != mMouseTrackStartImagePoint.x || mouseImagePoint.y != mMouseTrackStartImagePoint.y )
				{
					mMouseTrackResultIsDrag = true;
					//
					StartDrag(inLocalPoint,inModifierKeys);
					//TrackingMode解除
					mMouseTrackingMode = kMouseTrackingMode_None;
					NVM_SetMouseTrackingMode(false);
				}
			}
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
}

//Mouse Tracking終了（マウスボタンUp）
void	AView_List::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	//TrackingMode解除
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}
//#688 #endif

//R0186
//マウスボタン押下時のコールバック(AView用)
ABool	AView_List::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( mEnableContextMenu == true )
	{
		AImagePoint	clickImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		if( NVM_IsImagePointInViewRect(clickImagePoint) == true )
		{
			mContextMenuSelectedRowDisplayIndex = clickImagePoint.y/GetRowHeight();
			AGlobalPoint	globalPoint;
			NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
			//#&88 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(mContextMenuItemID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());//win 080712
			NVMC_ShowContextMenu(mContextMenuItemID,globalPoint);//#688
		}
		return true;
	}
	else
	{
		return false;
	}
}

//R0186
AIndex	AView_List::SPI_GetContextMenuSelectedRowDBIndex() const
{
	return mRowOrderDisplayToDBArray.Get(mContextMenuSelectedRowDisplayIndex);
}

//
void	AView_List::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
									   const ALocalPoint inLocalPoint )//win 080706
{
	//R0000
	ANumber	scrollPercent;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win 080702
	{
		scrollPercent = mWheelScrollPercentHighSpeed;
	}
	else
	{
		scrollPercent = mWheelScrollPercent;
	}
	//スクロール実行
	NVI_Scroll(inDeltaX*NVI_GetHScrollBarUnit()*scrollPercent/100,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100);//R0000
	
	//（マウス位置が現在のホバー範囲外になるときは）ホバー更新
	AImagePoint	clickImagePoint = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	AIndex newCursorDisplayIndex = clickImagePoint.y/GetRowHeight();
	if( newCursorDisplayIndex < mCursorRowStartDisplayIndex  || newCursorDisplayIndex > mCursorRowEndDisplayIndex )
	{
		UpdateHover(inLocalPoint,inModifierKeys,false);
	}
}

/**
マウスホバー処理
*/
ABool	AView_List::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//ホバー更新
	UpdateHover(inLocalPoint,inModifierKeys,false);
	//カーソル設定
	ACursorWrapper::SetCursor(kCursorType_Arrow);
	return true;
}

/**
ホバー更新
*/
void	AView_List::UpdateHover( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ABool inUpdateAlways )
{
	if( mEnableCursorRow == true )
	{
		AImagePoint	clickImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		
		//==================ホバー位置取得==================
		AIndex newCursorDisplayIndex = clickImagePoint.y/GetRowHeight();
		
		//==================ホバー位置が行範囲外ならホバーを消して終了==================
		if( newCursorDisplayIndex < 0 || newCursorDisplayIndex >= SPI_GetDisplayRowCount() )
		{
			ClearCursorRow();
			return;
		}
		
		//==================foldingアイコン上かどうかの判定==================
		ABool	newCursorRowHoverFolding = false;
		ALocalRect	foldingRect = {0};
		if( newCursorDisplayIndex >= 0 && newCursorDisplayIndex < SPI_GetDisplayRowCount() )
		{
			GetOutlineFoldingAndOutlineMarginRect(newCursorDisplayIndex,foldingRect);
			if( IsPointInRect(inLocalPoint,foldingRect) == true )
			{
				newCursorRowHoverFolding = true;
			}
		}
		
		//==================ホバー更新==================
		//ホバー位置が変化したか、foldingアイコン上かどうかが変化したか、inUpdateAlwaysがtrueのときはホバー更新
		if( newCursorDisplayIndex != mCursorRowStartDisplayIndex ||
		   newCursorRowHoverFolding != mCursorRowHoverFolding ||
		   inUpdateAlways == true )
		{
			//変更前のホバー範囲を記憶
			AIndex	svStartRowDisplayIndex = mCursorRowStartDisplayIndex;
			AIndex	svEndRowDisplayIndex = mCursorRowEndDisplayIndex;
			//折りたたみアイコンをホバー中かどうかをメンバー変数に記憶
			mCursorRowHoverFolding = newCursorRowHoverFolding;
			if( newCursorRowHoverFolding == true )
			{
				//==================折りたたみ範囲をホバー設定==================
				//folding開始位置db index取得
				AIndex	foldingStartRowDBIndex = mRowOrderDisplayToDBArray.Get(newCursorDisplayIndex);
				AIndex	foldingEndRowDBIndex = foldingStartRowDBIndex;
				//この項目の折りたたみレベルを取得
				AIndex	collapseLevel = mDataBase.GetData_NumberArray(kOutlineFoldingLevel,foldingStartRowDBIndex);
				//次の項目がcollapseLevel以下となる項目までとばす
				for( ; foldingEndRowDBIndex+1 < GetRowCount(); foldingEndRowDBIndex++ )
				{
					if( mDataBase.GetData_NumberArray(kOutlineFoldingLevel,foldingEndRowDBIndex+1) <= collapseLevel )
					{
						break;
					}
				}
				//folding範囲表示index取得
				mCursorRowStartDisplayIndex = newCursorDisplayIndex;
				mCursorRowEndDisplayIndex = SPI_GetDisplayRowIndexFromDBIndex(foldingEndRowDBIndex);
				if( mCursorRowEndDisplayIndex == kIndex_Invalid )
				{
					mCursorRowEndDisplayIndex = newCursorDisplayIndex;
				}
				mCursorRowLeftMargin = foldingRect.right - 20;
				if( mCursorRowLeftMargin < 0 )   mCursorRowLeftMargin = 0;
			}
			else
			{
				//==================現在のマウス行をホバー設定==================
				mCursorRowStartDisplayIndex = mCursorRowEndDisplayIndex = newCursorDisplayIndex;
				mCursorRowLeftMargin = 0;
			}
			//描画更新
			SPI_RefreshRows(svStartRowDisplayIndex,svEndRowDisplayIndex);
			SPI_RefreshRows(mCursorRowStartDisplayIndex,mCursorRowEndDisplayIndex);
			//ホバー更新を通知
			AIndex	hoverStartDBIndex = kIndex_Invalid;
			AIndex	hoverEndDBIndex = kIndex_Invalid;
			if( mCursorRowStartDisplayIndex != kIndex_Invalid )
			{
				hoverStartDBIndex = mRowOrderDisplayToDBArray.Get(mCursorRowStartDisplayIndex);
			}
			if( mCursorRowEndDisplayIndex != kIndex_Invalid )
			{
				hoverEndDBIndex = mRowOrderDisplayToDBArray.Get(mCursorRowEndDisplayIndex);
			}
			NVM_GetWindow().OWICB_ListViewHoverUpdated(NVI_GetControlID(),hoverStartDBIndex,hoverEndDBIndex);
		}
	}
}

//win 080712
void	AView_List::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearCursorRow();
}

void	AView_List::EVTDO_DoControlBoundsChanged()
{
	SetImageSize();
}

//Drag&Drop処理（Drag中）
void	AView_List::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
{
	if( mEnableDragDrop == false )   return;
	AIndex	newDragCaretDisplayRowIndex = CalcDragCaretDisplayRowIndex(inLocalPoint);
	if( newDragCaretDisplayRowIndex != mDragCaretRowDisplayIndex || mDragDrawn == false )
	{
		if( mDragDrawn == true )
		{
			//Erase
			mDragDrawn = false;
			XorDragCaret();
		}
		mDragCaretRowDisplayIndex = newDragCaretDisplayRowIndex;
		mDragDrawn = true;
		XorDragCaret();
	}
}

//Drag&Drop処理（DragがViewに入った）
void	AView_List::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mEnableDragDrop == false )   return;
	mDragCaretRowDisplayIndex = CalcDragCaretDisplayRowIndex(inLocalPoint);
	mDragDrawn = true;
	XorDragCaret();
}

//Drag&Drop処理（DragがViewから出た）
void	AView_List::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mEnableDragDrop == false )   return;
	if( mDragDrawn == true )
	{
		//Erase
		mDragDrawn = false;
		XorDragCaret();
	}
	//#871
	mDragCaretRowDisplayIndex = kIndex_Invalid;
}

//Drag&Drop処理（Drop）
void	AView_List::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mEnableDragDrop == false )   return;
	if( mDragDrawn == true )
	{
		//Erase
		mDragDrawn = false;
		XorDragCaret();
	}
	mDragCaretRowDisplayIndex = CalcDragCaretDisplayRowIndex(inLocalPoint);
	
	//==================ファイルDrop==================
	AFileAcc	file;
	if( AScrapWrapper::GetFileDragData(inDragRef,file) == true )
	{
		NVM_GetWindow().OWICB_ListViewFileDropped(NVI_GetControlID(),mDragCaretRowDisplayIndex,file);
		return;
	}
	
	//==================ListView項目Drop==================
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_ListViewItem) == true )
	{
		if( mDragCaretRowDisplayIndex == mSelectedRowDisplayIndex || mDragCaretRowDisplayIndex == mSelectedRowDisplayIndex+1 )   return;
		
		//行index取得
		AIndex	newRowIndex = mDragCaretRowDisplayIndex;
		if( newRowIndex > mSelectedRowDisplayIndex )
		{
			newRowIndex--;
		}
		//行移動
		//（複数項目移動、子移動には未対応。
		//複数項目Drag、ソート／フィルター時のDragはDrag開始を禁止している。）
		MoveRow(mRowOrderDisplayToDBArray.Get(mSelectedRowDisplayIndex),mRowOrderDisplayToDBArray.Get(newRowIndex));
		//drop後の行を選択
		SPI_SetSelect(newRowIndex);
		//AWindowでの選択時処理
		NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),0);
	}
}

//
AIndex	AView_List::CalcDragCaretDisplayRowIndex( const ALocalPoint& inLocalPoint )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	AIndex	caretRowIndex = clickImagePoint.y/GetRowHeight();
	if( caretRowIndex < 0 )   caretRowIndex = 0;
	if( caretRowIndex >= GetRowCount() )   caretRowIndex = GetRowCount();
	return caretRowIndex;
}

void	AView_List::XorDragCaret()
{
	AImageRect	imageframe;
	NVM_GetImageViewRect(imageframe);
	AImagePoint	caretImageStart, caretImageEnd;
	caretImageStart.y = caretImageEnd.y = mDragCaretRowDisplayIndex*GetRowHeight();
	caretImageStart.x = imageframe.left;
	caretImageEnd.x = imageframe.right;
	ALocalPoint	caretLocalStart, caretLocalEnd;
	NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
	NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
	//#871 NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,false,2);
	ALocalRect	refreshRect = {0};
	refreshRect.left	= caretLocalStart.x;
	refreshRect.top		= caretLocalStart.y - 5;
	refreshRect.right	= caretLocalEnd.x;
	refreshRect.bottom	= caretLocalStart.y + 5;
	NVMC_RefreshRect(refreshRect);
}

/**
行移動
*/
void 	AView_List::MoveRow( const AIndex inDeleteRowDBIndex, const AIndex inNewRowDBIndex )
{
	//#695 SPI_BeginSetTable()はテーブルを全て削除するようになった＆mTextStyleArray等はテーブルに組み込んだので、この処理は削除。 SPI_BeginSetTable();
	//まず選択解除（OWICB_ListViewRowMoved()は選択解除中に処理すること前提）
	SPI_SetSelect(kIndex_Invalid);
	NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),0);
	//DBのデータ移動
	mDataBase.MoveRow_Table(kFilePathText,inDeleteRowDBIndex,inNewRowDBIndex);
	//#695 SPI_EndSetTable();
	//AWindowでの行移動
	NVM_GetWindow().OWICB_ListViewRowMoved(NVI_GetControlID(),inDeleteRowDBIndex,inNewRowDBIndex);
}

//#328
/**
ヘルプタグ
*/
ABool	AView_List::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	outTagSide = kHelpTagSide_Right;//#644
	//#688 NVM_GetWindowConst().NVI_HideHelpTag();
	
	//現在の表示行範囲取得
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	AIndex	startRow = imageFrameRect.top / GetRowHeight();
	AIndex	endRow = imageFrameRect.bottom / GetRowHeight();
	//localviewrect取得
	ALocalRect	viewRect;
	NVM_GetLocalViewRect(viewRect);
	
	//表示中の各行毎に判定
	AItemCount	rowCount = SPI_GetDisplayRowCount();
	for( AIndex rowIndex = startRow; rowIndex <= endRow; rowIndex++ )
	{
		if( rowIndex >= rowCount )   break;
		
		//行のimageRectを取得
		AImageRect	imageRowRect = imageFrameRect;
		imageRowRect.top	= rowIndex*GetRowHeight();
		imageRowRect.bottom	= (rowIndex+1)*GetRowHeight();
		//localRectに変換
		ALocalRect	rowRect;
		NVM_GetLocalRectFromImageRect(imageRowRect,rowRect);
		//マウス位置判定
		if( inPoint.x > rowRect.left && inPoint.x < rowRect.right &&
					inPoint.y > rowRect.top && inPoint.y < rowRect.bottom )
		{
			//この行にマウス位置が存在するなら、列の最初から、textarrayが存在する列を検索
			for( AIndex colIndex = 0; colIndex < mColumnIDArray.GetItemCount(); colIndex++ )
			{
				APrefID	colID = mColumnIDArray.Get(colIndex);
				switch(mDataBase.GetDataType(colID))
				{
				  case kDataType_TextArray:
					{
						//この行のDBIndex取得
						AIndex	DBRowIndex = mRowOrderDisplayToDBArray.Get(rowIndex);
						//Text取得
						AText	text;
						mDataBase.GetData_TextArray(colID,DBRowIndex,text);
						//表示幅取得
						ANumber	rowWidth = viewRect.right-viewRect.left;
						ALocalRect	foldingRect = {0};
						ABool	hasFolding = GetOutlineFoldingAndOutlineMarginRect(rowIndex,foldingRect);
						if( hasFolding == true )
						{
							rowWidth -= foldingRect.right-foldingRect.left;
						}
						//タグ表示文字列設定
						if( NVMC_GetDrawTextWidth(text) >= rowWidth-40 )
						{
							outText1.SetText(text);
						}
						outText2.SetText(text);
						//タグ表示位置設定（多段の場合は一番下）
						outRect = rowRect;
						outRect.left += kRowLeftMargin+kCellLeftMargin+kCellRightMargin;//win
						return true;
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
			}
		}
	}
	return false;
}

//win
ABool	AView_List::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	//#688
	outUpdateMenu = false;
	
	//
	if( inText.GetItemCount() == 1 )
	{
		AModifierKeys	modifiers = inModifierKeys;//#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent);
		//#0 ABool	shift = AKeyWrapper::IsShiftKeyPressed(modifiers);
		//#0 ABool	option = AKeyWrapper::IsOptionKeyPressed(modifiers);
		//#0 ABool	cmd = AKeyWrapper::IsCommandKeyPressed(modifiers);
		AUChar	ch = inText.Get(0);
		switch(ch)
		{
		  case kUChar_Tab:
			{
				NVM_GetWindow().OWICB_ViewTabKeyPressed(NVI_GetControlID(),modifiers);
				break;
			}
		  case kUChar_Escape:
			{
				NVM_GetWindow().OWICB_ViewEscapeKeyPressed(NVI_GetControlID(),modifiers);
				break;
			}
		  case kUChar_Up:
			{
				AIndex	sel = mSelectedRowDisplayIndex;
				if( sel == kIndex_Invalid )
				{
					sel = SPI_GetRowCount()-1;
				}
				else if( sel > 0 )
				{
					sel--;
				}
				SPI_SetSelect(sel);
				//
				NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),0);
				break;
			}
		  case kUChar_Down:
			{
				AIndex	sel = mSelectedRowDisplayIndex;
				if( sel == kIndex_Invalid )
				{
					sel = 0;
				}
				else if( sel < SPI_GetRowCount()-1 )
				{
					sel++;
				}
				SPI_SetSelect(sel);
				//
				NVM_GetWindow().EVT_Clicked(NVI_GetControlID(),0);
				break;
			}
			//#353
		  case kUChar_LineEnd:
		  case kUChar_Space:
			{
				if( mSelectedRowDisplayIndex != kIndex_Invalid )
				{
					NVM_GetWindow().EVT_DoubleClicked(NVI_GetControlID());
				}
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	return true;
}

#pragma mark ===========================

#pragma mark ---フォーカス
//#137

void	AView_List::EVTDO_DoViewFocusActivated()
{
	if( mFrameViewID != kObjectID_Invalid )
	{
		GetFrameView().SPI_SetFocused(true);
	}
}

void	AView_List::EVTDO_DoViewFocusDeactivated()
{
	if( mFrameViewID != kObjectID_Invalid )
	{
		GetFrameView().SPI_SetFocused(false);
	}
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

void	AView_List::SPI_SetSelect( const AIndex inSelect )
{
	//現在の選択を記憶
	AIndex	oldSelectedRowIndex = mSelectedRowDisplayIndex;
	//選択更新
	mSelectedRowDisplayIndex = inSelect;
	mSelectedRowDisplayIndexArray.DeleteAll();
	//
	if( inSelect != kIndex_Invalid )
	{
		//#237
		if( mMultiSelectEnalbed == true )
		{
			mSelectedRowDisplayIndexArray.Add(inSelect);
		}
		//
		SPI_AdjustScroll();
	}
	SPI_RefreshRow(oldSelectedRowIndex);
	SPI_RefreshRow(mSelectedRowDisplayIndex);
}

//win
/**
*/
void	AView_List::NVIDO_SetNumber( const ANumber inNumber )
{
	SPI_SetSelect(inNumber);
}

//#798
/**
次の項目を選択
*/
void	AView_List::SPI_SetSelectNextDisplayRow( const AModifierKeys inModifers )
{
	//cmdキーを押しながらのときは最後の項目を選択
	if( AKeyWrapper::IsCommandKeyPressed(inModifers) == true )
	{
		if( SPI_GetDisplayRowCount() > 0 )
		{
			SPI_SetSelectByDisplayRowIndex(SPI_GetDisplayRowCount()-1);
		}
		return;
	}
	//現在未選択なら最初の項目を選択
	if( mSelectedRowDisplayIndex == kIndex_Invalid )
	{
		if( SPI_GetDisplayRowCount() > 0 )
		{
			SPI_SetSelectByDisplayRowIndex(0);
		}
	}
	//次を選択
	else
	{
		if( mSelectedRowDisplayIndex+1 < SPI_GetDisplayRowCount() )
		{
			SPI_SetSelectByDisplayRowIndex(mSelectedRowDisplayIndex+1);
		}
		else
		{
			SPI_SetSelectByDisplayRowIndex(0);
		}
	}
}

//#798
/**
前の項目を選択
*/
void	AView_List::SPI_SetSelectPreviousDisplayRow( const AModifierKeys inModifers )
{
	//cmdキーを押しながらのときは最初の項目を選択
	if( AKeyWrapper::IsCommandKeyPressed(inModifers) == true )
	{
		if( SPI_GetDisplayRowCount() > 0 )
		{
			SPI_SetSelectByDisplayRowIndex(0);
		}
		return;
	}
	//現在未選択なら最後の項目を選択
	if( mSelectedRowDisplayIndex == kIndex_Invalid )
	{
		if( SPI_GetDisplayRowCount() > 0 )
		{
			SPI_SetSelectByDisplayRowIndex(SPI_GetDisplayRowCount()-1);
		}
	}
	//前を選択
	else
	{
		if( mSelectedRowDisplayIndex-1 >= 0 )
		{
			SPI_SetSelectByDisplayRowIndex(mSelectedRowDisplayIndex-1);
		}
		else
		{
			SPI_SetSelectByDisplayRowIndex(SPI_GetDisplayRowCount()-1);
		}
	}
}

void	AView_List::SPI_SetSelectByDBIndex( const AIndex inSelectDBIndex, const ABool inAdjustScroll )//#698
{
	AIndex	oldSelectedRowIndex = mSelectedRowDisplayIndex;
	mSelectedRowDisplayIndex = mRowOrderDisplayToDBArray.Find(inSelectDBIndex);
	//現在表示されていない項目をselectしたときは、DBIndexをさかのぼって、最初に表示されている項目をselectする
	if( mSelectedRowDisplayIndex == kIndex_Invalid && mDisplayRowOrderMode == kDisplayRowOrderMode_Normal )
	{
		for( AIndex dbindex = inSelectDBIndex-1; dbindex > 0; dbindex-- )
		{
			mSelectedRowDisplayIndex = mRowOrderDisplayToDBArray.Find(dbindex);
			if( mSelectedRowDisplayIndex != kIndex_Invalid )
			{
				break;
			}
		}
	}
	//#237
	if( mMultiSelectEnalbed == true )
	{
		mSelectedRowDisplayIndexArray.DeleteAll();
		mSelectedRowDisplayIndexArray.Add(mSelectedRowDisplayIndex);
	}
	//
	if( inAdjustScroll == true )//#698
	{
		SPI_AdjustScroll();
	}
	//
	SPI_RefreshRow(oldSelectedRowIndex);
	SPI_RefreshRow(mSelectedRowDisplayIndex);
}

//#237
/**
複数選択された項目のDBIndexを取得する

※グレーの項目は入れない
*/
void	AView_List::SPI_GetSelectedDBIndexArray( AArray<AIndex>& outDBIndexArray ) const
{
	outDBIndexArray.DeleteAll();
	if( mSelectedRowDisplayIndexArray.GetItemCount() > 0 )
	{
		for( AIndex i = 0; i < mSelectedRowDisplayIndexArray.GetItemCount(); i++ )
		{
			AIndex	selectedRowIndex = mSelectedRowDisplayIndexArray.Get(i);
			if( selectedRowIndex != kIndex_Invalid )
			{
				AIndex	dbIndex = mRowOrderDisplayToDBArray.Get(selectedRowIndex);
				if( mDataBase.GetData_BoolArray(kEnabled,dbIndex) == true )
				{
					outDBIndexArray.Add(dbIndex);
				}
			}
		}
	}
	else
	{
		if( mSelectedRowDisplayIndex != kIndex_Invalid )
		{
			AIndex	dbIndex = mRowOrderDisplayToDBArray.Get(mSelectedRowDisplayIndex);
			if( mDataBase.GetData_BoolArray(kEnabled,dbIndex) == true )
			{
				outDBIndexArray.Add(dbIndex);
			}
		}
	}
}

void	AView_List::SPI_AdjustScroll()
{
	SPI_AdjustScroll(mSelectedRowDisplayIndex);
}
void	AView_List::SPI_AdjustScroll( const AIndex inRowDisplayIndex )
{
	if( inRowDisplayIndex == kIndex_Invalid )
	{
		AImagePoint	pt = {0,0};
		NVI_ScrollTo(pt);
		return;
	}
	AImageRect	rect;
	NVM_GetImageViewRect(rect);
	AIndex	startRow = rect.top / GetRowHeight();
	AIndex	endRow = rect.bottom / GetRowHeight();
	if( inRowDisplayIndex < startRow || inRowDisplayIndex >= endRow )//B0321
	{
		AImagePoint	pt;
		pt.x = 0;
		pt.y = (inRowDisplayIndex-(endRow-startRow)/2)*GetRowHeight();
		NVI_ScrollTo(pt);
	}
}

void	AView_List::SPI_BeginSetTable()
{
	//R0108高速化→やっぱやめ。AWindow_JumpList::SPI_UpdateTableのほうで対応することにする。
	/*NVI_GetOriginOfLocalFrame(mSavedOrigin);
	mSelectedRowDisplayIndex = kIndex_Invalid;
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);*/
	//R0006
	/*#695
	mTextStyleArray.DeleteAll();
	mColorArray.DeleteAll();
	mBackgroundColorArray.DeleteAll();//#427
	mRowEditableArray.DeleteAll();//#427
	*/
	mDataBase.DeleteAllRows_Table(kFilePathText);
}

void	AView_List::SPI_EndSetTable()
{
	while( mDataBase.GetItemCount_Array(kFilePathText) < GetRowCount() )
	{
		mDataBase.Add_TextArray(kFilePathText,GetEmptyText());
	}
	/*#695
	while( mDataBase.GetItemCount_Array(kIconID) < GetRowCount() )
	{
		mDataBase.Add_NumberArray(kIconID,kIconID_NoIcon);
	}
	//#232 行のEnable/Disable
	while( mDataBase.GetItemCount_Array(kEnabled) < GetRowCount() )
	{
		mDataBase.Add_BoolArray(kEnabled,true);
	}
	*/
	mDataBase.CorrectTable();//#695
	//
	/*#871
	mRowOrderDisplayToDBArray.DeleteAll();
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		mRowOrderDisplayToDBArray.Add(i);
	}
	*/
	//行表示順更新
	UpdateRowOrderDisplay(false);
	/*UpdateRowOrderDisplay()で実行
	//win 080618
	if( mSelectedRowDisplayIndex >= SPI_GetRowCount() )
	{
		mSelectedRowDisplayIndex = kIndex_Invalid;
	}
	*/
	//
	SetImageSize();
	
	//R0108高速化→やっぱやめ。AWindow_JumpList::SPI_UpdateTableのほうで対応することにする。
	//NVI_ScrollTo(mSavedOrigin);
	
	NVI_Refresh();
}

#pragma mark ===========================

#pragma mark ---テーブル全体

/**
表示している項目の数を取得
*/
AItemCount	AView_List::SPI_GetDisplayRowCount() const
{
	return mRowOrderDisplayToDBArray.GetItemCount();
}

#pragma mark ===========================

#pragma mark ---列データの登録・設定・取得

//列の登録(Text)
void	AView_List::SPI_RegisterColumn_Text( const APrefID inColumnID, const ANumber inWidth, const AConstCharPtr inTitle, const ABool inEditable )
{
	mDataBase.CreateData_TextArray(inColumnID,"","");
	mColumnIDArray.Add(inColumnID);
	mColumnWidthArray.Add(inWidth);
	mColumnEditable.Add(inEditable);
	//win 080618
	//フレームの幅、タイトルを設定
	if( mFrameViewID != kObjectID_Invalid )
	{
		AText	title;
		if( inTitle[0] != 0 )
		{
			title.SetLocalizedText(inTitle);
		}
		GetFrameView().SPI_RegisterColumn(inColumnID,inWidth,title);
	}
	//アイコン
	//#1012 mIconDataBase.CreateData_NumberArray(inColumnID,"",0,-1,kNumber_MaxNumber);//#695 ,false);
}

//列の登録(Bool) win 080618
void	AView_List::SPI_RegisterColumn_Bool( const APrefID inColumnID, const ANumber inWidth, const AConstCharPtr inTitle, const ABool inEditable )
{
	mDataBase.CreateData_BoolArray(inColumnID,"",false);
	mColumnIDArray.Add(inColumnID);
	mColumnWidthArray.Add(inWidth);
	mColumnEditable.Add(inEditable);
	//フレームの幅、タイトルを設定
	if( mFrameViewID != kObjectID_Invalid )
	{
		AText	title;
		if( inTitle[0] != 0 )
		{
			title.SetLocalizedText(inTitle);
		}
		GetFrameView().SPI_RegisterColumn(inColumnID,inWidth,title);
	}
}

//列の登録(Number) win 080618
void	AView_List::SPI_RegisterColumn_Number( const APrefID inColumnID, const ANumber inWidth, const AConstCharPtr inTitle, const ABool inEditable )
{
	mDataBase.CreateData_NumberArray(inColumnID,"",0,kNumber_MinNumber,kNumber_MaxNumber);//#695 ,false);
	mColumnIDArray.Add(inColumnID);
	mColumnWidthArray.Add(inWidth);
	mColumnEditable.Add(inEditable);
	//フレームの幅、タイトルを設定
	if( mFrameViewID != kObjectID_Invalid )
	{
		AText	title;
		if( inTitle[0] != 0 )
		{
			title.SetLocalizedText(inTitle);
		}
		GetFrameView().SPI_RegisterColumn(inColumnID,inWidth,title);
	}
}

//列データを設定(Text)
void	AView_List::SPI_SetColumn_Text( const APrefID inColumnID, const ATextArray& inTextArray )
{
	mDataBase.SetData_TextArray(inColumnID,inTextArray);
}

//列データを設定(Bool)
void	AView_List::SPI_SetColumn_Bool( const APrefID inColumnID, const ABoolArray& inBoolArray )
{
	mDataBase.SetData_BoolArray(inColumnID,inBoolArray);
}

//列データを設定(Number)
void	AView_List::SPI_SetColumn_Number( const APrefID inColumnID, const ANumberArray& inNumberArray )
{
	mDataBase.SetData_NumberArray(inColumnID,inNumberArray);
}

//列データを設定(Color)
void	AView_List::SPI_SetColumn_Color( const APrefID inColumnID, const AColorArray& inColorArray )
{
	mDataBase.SetData_ColorArray(inColumnID,inColorArray);
}

//列データを取得 
//R0108 高速化
const ATextArray&	AView_List::SPI_GetColumn_Text( const APrefID inColumnID ) const
{
	return mDataBase.GetData_ConstTextArrayRef(inColumnID);
}

//列データを取得 win 080618
const ANumberArray&	AView_List::SPI_GetColumn_Number( const APrefID inColumnID ) const
{
	return mDataBase.GetData_ConstNumberArrayRef(inColumnID);
}

//列データを取得 win 080618
const ABoolArray&	AView_List::SPI_GetColumn_Bool( const APrefID inColumnID ) const
{
	return mDataBase.GetData_ConstBoolArrayRef(inColumnID);
}

//列データを取得 win 080618
const AColorArray&	AView_List::SPI_GetColumn_Color( const APrefID inColumnID ) const
{
	return mDataBase.GetData_ConstColorArrayRef(inColumnID);
}

#pragma mark ===========================

#pragma mark ---

void	AView_List::SPI_SetTable_File( const ATextArray& inTextArray )
{
	mDataBase.SetData_TextArray(kFilePathText,inTextArray);
}

void	AView_List::SPI_SetTable_TextStyle( const AArray<ATextStyle>& inTextStyleArray )
{
	mDataBase.DeleteAll_NumberArray(kTextStyle);
	AItemCount	rowCount = inTextStyleArray.GetItemCount();
	for( AIndex i = 0; i < rowCount; i++ )
	{
		mDataBase.Add_NumberArray(kTextStyle,static_cast<ANumber>(inTextStyleArray.Get(i)));
	}
	/*#695
	mTextStyleArray.DeleteAll();
	AItemCount	count = inTextStyleArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mTextStyleArray.Add(inTextStyleArray.Get(i));
	}
	*/
}

//R0006
void	AView_List::SPI_SetTable_Color( const AArray<AColor>& inColorArray )
{
	mDataBase.SetData_ColorArray(kColor,inColorArray);
	/*#695
	mColorArray.DeleteAll();
	AItemCount	count = inColorArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mColorArray.Add(inColorArray.Get(i));
	}
	*/
}

//#427
/**
背景色設定
*/
void	AView_List::SPI_SetTable_BackgroundColor( const AArray<AColor>& inColorArray )
{
	mDataBase.SetData_ColorArray(kBackgroundColor,inColorArray);
	//
	mDataBase.DeleteAll_BoolArray(kBackgroundColorEnabled);
	AItemCount	itemCount = inColorArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		mDataBase.Add_BoolArray(kBackgroundColorEnabled,true);
	}
	/*#695
	mBackgroundColorArray.DeleteAll();
	AItemCount	count = inColorArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mBackgroundColorArray.Add(inColorArray.Get(i));
	}
	*/
}

void	AView_List::SPI_SetTable_Icon( const AArray<AIconID>& inIconArray )
{
	mDataBase.DeleteAll_NumberArray(kIconID);
	AItemCount	count = inIconArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mDataBase.Add_NumberArray(kIconID,inIconArray.Get(i));
	}
}

//#725
/**
ImageIconIDを設定
*/
void	AView_List::SPI_SetTable_ImageIconID( const AArray<AImageID>& inImageIconArray )
{
	mDataBase.DeleteAll_NumberArray(kImageIconID);
	AItemCount	count = inImageIconArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mDataBase.Add_NumberArray(kImageIconID,inImageIconArray.Get(i));
	}
}

//#427
/**
編集可否設定
*/
void	AView_List::SPI_SetTable_RowEditable( const ABoolArray& inEditableArray )
{
	mDataBase.SetData_BoolArray(kRowEditable,inEditableArray);
	//#695 mRowEditableArray.SetFromObject(inEditableArray);
}

//#130
/**
Outlineレベル設定
*/
void	AView_List::SPI_SetTable_OutlineFoldingLevel( const AArray<AIndex>& inOutlineFoldinLevelArray )
{
	mDataBase.SetData_NumberArray(kOutlineFoldingLevel,inOutlineFoldinLevelArray);
	mOutlineLevelDataExist = true;
}

//#232
/**
行のEnable/Disable設定
*/
void	AView_List::SPI_SetTable_Enabled( const ABoolArray& inEnabledArray )
{
	mDataBase.DeleteAll_BoolArray(kEnabled);
	AItemCount	count = inEnabledArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		mDataBase.Add_BoolArray(kEnabled,inEnabledArray.Get(i));
	}
}

//win 080618
void	AView_List::SPI_SetColumnWidth( const ADataID inColumnID, const ANumber inWidth )
{
	AIndex	index = mColumnIDArray.Find(inColumnID);
	if( index == kIndex_Invalid )   {_ACError("",this);return;}
	mColumnWidthArray.Set(index,inWidth);
	//win 080618
	//フレームの幅、タイトルを設定
	if( /*#205 mFrameControlID != kControlID_Invalid*/mFrameViewID != kObjectID_Invalid )
	{
		/*#205 NVM_GetWindow().NVI_GetListFrameView(mFrameControlID)*/GetFrameView().SPI_SetColumnWidth(index,inWidth);
	}
}

//win 080618
ANumber	AView_List::SPI_GetColumnWidth( const ADataID inColumnID ) const
{
	AIndex	index = mColumnIDArray.Find(inColumnID);
	if( index == kIndex_Invalid )   {_ACError("",this);return 0;}
	return mColumnWidthArray.Get(index);
}

/* win 080618 未使用（高速化のため使用されなくなった）のため削除
void	AView_List::SPI_GetTable_Text( const APrefID inColumnID, ATextArray& outTextArray ) const
{
	outTextArray.Set(mDataBase.GetData_ConstTextArrayRef(inColumnID));
}
*/

void	AView_List::SPI_GetTable_File( ATextArray& outTextArray ) const
{
	outTextArray.SetFromTextArray(mDataBase.GetData_ConstTextArrayRef(kFilePathText));
}

/*#695 未使用のため削除
const AArray<ATextStyle>&	AView_List::SPI_GetTable_TextStyle() const
{
	return mTextStyleArray;
}
*/

#pragma mark ===========================

#pragma mark ---行毎Set

//R0108 高速化
void	AView_List::SPI_SetTableRow_Text( const APrefID inColumnID, 
		const AIndex inDBIndex, const AText& inText, const ATextStyle inTextStyle )
{
	mDataBase.SetData_TextArray(inColumnID,inDBIndex,inText);
	//#695 mTextStyleArray.Set(inDBIndex,inTextStyle);
	mDataBase.SetData_NumberArray(kTextStyle,inDBIndex,static_cast<ANumber>(inTextStyle));//#695
	//#695 通常ArrayのFind()なので遅い（コール元でまとめてrefreshするようにする） SPI_RefreshRow(mRowOrderDisplayToDBArray.Find(inDBIndex));
}

//#130
/**
アウトラインレベル設定
*/
void	AView_List::SPI_SetTableRow_OutlineFoldingLevel( const AIndex inDBIndex, const AIndex inOutlineFoldingLevel )
{
	mDataBase.SetData_NumberArray(kOutlineFoldingLevel,inDBIndex,inOutlineFoldingLevel);
	mOutlineLevelDataExist = true;
}

#pragma mark ===========================

#pragma mark ---行毎Insert/Delete

//#695
/**
行挿入（inCount個の行を挿入）
*/
void	AView_List::SPI_InsertTableRows( const AIndex inDBIndex, const AItemCount inCount,
		const ATextStyle inTextStyle, const AColor inColor, 
		const ABool inEnableBackgroundColor, const AColor inBackgroundColor, 
		const ABool inRowEditable )
{
	//display row indexを取得
	//（挿入DBIndex以降で最初に表示されている項目のdisplay row indexを取得。なければ最後。）
	AIndex	insertDisplayRowIndex = mRowOrderDisplayToDBArray.GetItemCount();
	for( AIndex dbIndex = inDBIndex; dbIndex < mDataBase.GetItemCount_Array(kFilePathText); dbIndex++ )
	{
		AIndex	displayIndex = mRowOrderDisplayToDBArray.Find(dbIndex);
		if( displayIndex != kIndex_Invalid )
		{
			insertDisplayRowIndex = displayIndex;
			break;
		}
	}
	//==================DBにデータ挿入==================
	mDataBase.ReserveRow_Table(kFilePathText,inDBIndex,inCount);
	for( AIndex index = inDBIndex; index < inDBIndex + inCount; index++ )
	{
		mDataBase.SetData_NumberArray(kIconID,index,0);
		mDataBase.SetData_BoolArray(kEnabled,index,true);
		mDataBase.SetData_NumberArray(kTextStyle,index,inTextStyle);
		mDataBase.SetData_ColorArray(kColor,index,inColor);
		mDataBase.SetData_ColorArray(kBackgroundColor,index,inBackgroundColor);
		mDataBase.SetData_BoolArray(kBackgroundColorEnabled,index,inEnableBackgroundColor);
		mDataBase.SetData_BoolArray(kRowEditable,index,inRowEditable);
		mDataBase.SetData_NumberArray(kImageIconID,index,kImageID_Invalid);//#725
	}
	/*#871
	//ソートデータ項目挿入
	mRowOrderDisplayToDBArray.Reserve(inDBIndex,inCount);
	//ソート状態を解除（行の表示indexとdbのindexを全て同じにする）//★速度検討
	{
		AItemCount	itemCount = mRowOrderDisplayToDBArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mRowOrderDisplayToDBArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			p[i] = i;
		}
	}
	*/
	//==================表示行arrayの更新==================
	//mRowOrderDisplayToDBArray内の、挿入DBIndex以降のDBIndexをずらす
	{
		AItemCount	itemCount = mRowOrderDisplayToDBArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mRowOrderDisplayToDBArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			if( p[i] >= inDBIndex )
			{
				p[i] += inCount;
			}
		}
	}
	
	//mRowOrderDisplayToDBArrayに今回追加分の項目を挿入
	//つまり、フィルタ中であっても、追加項目は必ず表示される。（表示位置は挿入DBIndex以降で最初に表示されている項目の前）
	mRowOrderDisplayToDBArray.Reserve(insertDisplayRowIndex,inCount);
	{
		AItemCount	itemCount = mRowOrderDisplayToDBArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mRowOrderDisplayToDBArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < inCount; i++ )
		{
			p[insertDisplayRowIndex+i] = inDBIndex+i;
		}
	}
	//選択解除
	mSelectedRowDisplayIndex = kIndex_Invalid;
	mSelectedRowDisplayIndexArray.DeleteAll();
	//Image size更新
	SetImageSize();
}

//#695
/**
行削除（一行削除）
*/
void	AView_List::SPI_DeleteTableRow( const AIndex inDBIndex )
{
	//==================DBの行削除==================
	mDataBase.DeleteRow_Table(kFilePathText,inDBIndex);
	
	//==================表示行arrayの更新==================
	AIndex	deleteDisplayRowIndex = mRowOrderDisplayToDBArray.Find(inDBIndex);
	if( deleteDisplayRowIndex != kIndex_Invalid )
	{
		mRowOrderDisplayToDBArray.Delete1(deleteDisplayRowIndex);
	}
	//mRowOrderDisplayToDBArray内の、削除DBIndex以降のDBIndexをずらす
	{
		AItemCount	itemCount = mRowOrderDisplayToDBArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mRowOrderDisplayToDBArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			if( p[i] >= inDBIndex )
			{
				p[i] -= 1;
			}
		}
	}
	/*
	//ソートデータ項目削除
	mRowOrderDisplayToDBArray.Delete1(0);
	//ソート状態を解除（行の表示indexとdbのindexを全て同じにする）//★速度検討
	{
		AItemCount	itemCount = mRowOrderDisplayToDBArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mRowOrderDisplayToDBArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			p[i] = i;
		}
	}
	*/
	//選択解除
	mSelectedRowDisplayIndex = kIndex_Invalid;
	mSelectedRowDisplayIndexArray.DeleteAll();
	//Image size更新
	SetImageSize();
}

#pragma mark ===========================

#pragma mark ---表示行制御

/**
表示行更新
*/
void	AView_List::UpdateRowOrderDisplay( const ABool inRedraw )
{
	//表示行データ全削除
	mRowOrderDisplayToDBArray.DeleteAll();
	//
	if( mSortMode == true )
	{
		//==================ソートモード==================
		
		for( AIndex i = 0; i < GetRowCount(); i++ )
		{
			mRowOrderDisplayToDBArray.Add(i);
		}
		//クイックソート
		if( mDataBase.GetItemCount_Array(mSortColumnID) >= 2 )
		{
			QuickSort(mSortColumnID,0,mDataBase.GetItemCount_Array(mSortColumnID)-1,mSortAscendant);
		}
		//ソートモードに設定
		mDisplayRowOrderMode = kDisplayRowOrderMode_Sorted;
	}
	else if( mFilterText.GetItemCount() > 0 )
	{
		//==================フィルタモード==================
		
		const ATextArray&	textArray = SPI_GetColumn_Text(mFilterColumnID);
		//部分一致するものを追加していく
		for( AIndex i = 0; i < GetRowCount(); i++ )
		{
			AText	text;
			textArray.Get(i,text);
			text.ChangeCaseLowerFast();
			AIndex	pos = 0;
			if( text.FindText(0,mFilterText,pos) == true )
			{
				mRowOrderDisplayToDBArray.Add(i);
			}
		}
		//フィルタモードに設定
		mDisplayRowOrderMode = kDisplayRowOrderMode_Filtered;
	}
	else
	{
		//==================通常モード（折りたたみ適用）==================
		
		//各行データごとのループ
		for( AIndex i = 0; i < GetRowCount(); i++ )
		{
			//この項目をmRowOrderDisplayToDBArrayに追加（＝表示する）
			mRowOrderDisplayToDBArray.Add(i);
			//この項目の折りたたみ状態を取得
			ABool	collapsed = mDataBase.GetData_BoolArray(kOutlineFoldingCollapsed,i);
			if( collapsed == true )
			{
				//------------------折りたたみの場合------------------
				//この項目の折りたたみレベルを取得
				AIndex	collapseLevel = mDataBase.GetData_NumberArray(kOutlineFoldingLevel,i);
				//次の項目がcollapseLevel以下となる項目までとばす（＝非表示にする）。
				for( ; i+1 < GetRowCount(); i++ )
				{
					if( mDataBase.GetData_NumberArray(kOutlineFoldingLevel,i+1) <= collapseLevel )
					{
						break;
					}
				}
			}
		}
		//通常モードに設定
		mDisplayRowOrderMode = kDisplayRowOrderMode_Normal;
	}
	
	//現在選択行が表示行の数をオーバーしたら、選択解除する
	if( mSelectedRowDisplayIndex >= mRowOrderDisplayToDBArray.GetItemCount() )
	{
		mSelectedRowDisplayIndex = kIndex_Invalid;
	}
	//複数選択は無条件解除
	mSelectedRowDisplayIndexArray.DeleteAll();
	//イメージサイズ更新
	SetImageSize();
	
	if( inRedraw == true )
	{
		//最初にスクロール
		NVI_ScrollTo(kImagePoint_00);
		//描画更新
		NVI_Refresh();
	}
}

#pragma mark ===========================

#pragma mark ---フィルタ

/**
フィルタテキスト設定
*/
void	AView_List::SPI_SetFilterText( const ADataID inColumnID, const AText& inFilterText )
{
	//現在と同じなら何もしない
	if( mFilterText.Compare(inFilterText) == true )
	{
		return;
	}
	//フィルタ設定
	mFilterText.SetText(inFilterText);
	mFilterColumnID = inColumnID;
	//表示行更新
	UpdateRowOrderDisplay();
}

#pragma mark ===========================

#pragma mark ---フォールディング

/**
折りたたまれているかどうかを取得
*/
ABool	AView_List::SPI_IsCollapsed( const AIndex inDisplayRowIndex ) const
{
	AIndex	DBIndex = mRowOrderDisplayToDBArray.Get(inDisplayRowIndex);
	return mDataBase.GetData_BoolArray(kOutlineFoldingCollapsed,DBIndex);
}

/**
折りたたみ・展開
*/
void	AView_List::SPI_ExpandCollapse( const AIndex inDisplayRowIndex, const ABool inSelectRow )
{
	//DB更新
	AIndex	DBIndex = mRowOrderDisplayToDBArray.Get(inDisplayRowIndex);
	mDataBase.SetData_BoolArray(kOutlineFoldingCollapsed,DBIndex,
				!mDataBase.GetData_BoolArray(kOutlineFoldingCollapsed,DBIndex));
	
	if( inSelectRow == true )
	{
		//==================行選択更新する場合==================
		//表示行更新
		UpdateRowOrderDisplay(false);
		//行選択
		SPI_SetSelectByDBIndex(DBIndex,true);
		//描画更新
		NVI_Refresh();
	}
	else
	{
		//==================行選択更新しない場合==================
		//元々の選択行取得
		AIndex	origSelectDBIndex = kIndex_Invalid;
		if( mSelectedRowDisplayIndex != kIndex_Invalid )
		{
			origSelectDBIndex = mRowOrderDisplayToDBArray.Get(mSelectedRowDisplayIndex);
		}
		AImagePoint	origin = {0};
		NVI_GetOriginOfLocalFrame(origin);
		//表示行更新
		UpdateRowOrderDisplay(false);
		//選択行復元
		SPI_SetSelectByDBIndex(origSelectDBIndex,false);
		NVI_ScrollTo(origin,false);
		//描画更新
		NVI_Refresh();
	}
}

/**
折りたたみ可能かどうか（子が存在か）を取得
*/
ABool	AView_List::HasFoldingChild( const AIndex inDBRowIndex ) const
{
	//次の項目のアウトラインレベルが深ければtrueを返す
	AIndex	collapseLevel = mDataBase.GetData_NumberArray(kOutlineFoldingLevel,inDBRowIndex);
	if( inDBRowIndex+1 < GetRowCount() )
	{
		if( mDataBase.GetData_NumberArray(kOutlineFoldingLevel,inDBRowIndex+1) > collapseLevel )
		{
			return true;
		}
	}
	return false;
}

/**
全て展開
*/
void	AView_List::SPI_ExpandAll()
{
	//全てcollapse解除
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		mDataBase.SetData_BoolArray(kOutlineFoldingCollapsed,i,false);
	}
	mDisplayRowOrderMode = kDisplayRowOrderMode_Normal;
	//表示行更新
	UpdateRowOrderDisplay();
	//スクロール調整
	SPI_AdjustScroll();
}

/**
同じレベルの全てを折りたたむ・展開し、スクロール位置を調整（指定行が同じ場所に表示されるようにする）
*/
void	AView_List::SPI_ExpandCollapseAllForSameLevel( const AIndex inDisplayRowIndex )
{
	//指定行のrectを記憶
	AImageRect	rowImageRect = {0};
	GetRowImageRect(inDisplayRowIndex,inDisplayRowIndex+1,rowImageRect);
	ALocalRect	rowLocalRect = {0};
	NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
	//折りたたみ、展開
	AIndex	DBRowIndex = mRowOrderDisplayToDBArray.Get(inDisplayRowIndex);
	AIndex	outlineLevel = SPI_GetOutlineFoldingLevel(DBRowIndex);
	if( SPI_IsCollapsed(inDisplayRowIndex) == false )
	{
		SPI_CollapseAllForLevel(outlineLevel);
	}
	else
	{
		SPI_ExpandAllForLevel(outlineLevel);
	}
	//指定行inDisplayRowIndexがexpand/collapse前と同じ位置に表示されるようにスクロール位置を調整する
	AIndex	rowDisplayIndex = mRowOrderDisplayToDBArray.Find(DBRowIndex);
	AImageRect	newRowImageRect = {0};
	GetRowImageRect(rowDisplayIndex,rowDisplayIndex+1,newRowImageRect);
	AImagePoint	imagept = {0};
	imagept.y = newRowImageRect.top - rowLocalRect.top;
	NVI_ScrollTo(imagept);
}

/**
同じレベルの全てを折りたたむ
*/
void	AView_List::SPI_CollapseAllForLevel( const ANumber inLevel )
{
	//同じアウトラインレベルを全て折りたたむ
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		if( mDataBase.GetData_NumberArray(kOutlineFoldingLevel,i) == inLevel )
		{
			mDataBase.SetData_BoolArray(kOutlineFoldingCollapsed,i,true);
		}
	}
	mDisplayRowOrderMode = kDisplayRowOrderMode_Normal;
	//表示行更新
	UpdateRowOrderDisplay();
}

/**
同じレベルの全てを展開
*/
void	AView_List::SPI_ExpandAllForLevel( const ANumber inLevel )
{
	//同じアウトラインレベルを全て展開
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		if( mDataBase.GetData_NumberArray(kOutlineFoldingLevel,i) == inLevel )
		{
			mDataBase.SetData_BoolArray(kOutlineFoldingCollapsed,i,false);
		}
	}
	mDisplayRowOrderMode = kDisplayRowOrderMode_Normal;
	//表示行更新
	UpdateRowOrderDisplay();
}

/**
アウトラインレベル取得
*/
ANumber	AView_List::SPI_GetOutlineFoldingLevel( const AIndex inDBRowIndex ) const
{
	return mDataBase.GetData_NumberArray(kOutlineFoldingLevel,inDBRowIndex);
}

/**
折りたたみ行のテキストリストを取得
*/
void	AView_List::SPI_GetCollapseRowsTextArray( const ADataID inColumnID, ATextArray& outTextArray ) const
{
	outTextArray.DeleteAll();
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		if( mDataBase.GetData_BoolArray(kOutlineFoldingCollapsed,i) == true )
		{
			AText	text;
			mDataBase.GetData_TextArray(inColumnID,i,text);
			outTextArray.Add(text);
		}
	}
}

/**
テキストリストに一致する行のみ折りたたみ
*/
void	AView_List::SPI_ApplyCollapseRowsByTextArray( const ADataID inColumnID, const ATextArray& inTextArray ) 
{
	//全て展開
	if( mDataBase.GetItemCount_BoolArray(kOutlineFoldingCollapsed) == 0 )
	{
		for( AIndex i = 0; i < GetRowCount(); i++ )
		{
			mDataBase.Add_BoolArray(kOutlineFoldingCollapsed,false);
		}
	}
	//hash text arrayへコピー
	AHashTextArray	hashTextArray;
	hashTextArray.SetFromTextArray(inTextArray);
	//テキストが一致する項目を折りたたみ
	for( AIndex i = 0; i < GetRowCount(); i++ )
	{
		AText	text;
		mDataBase.GetData_TextArray(inColumnID,i,text);
		if( hashTextArray.Find(text) != kIndex_Invalid )
		{
			mDataBase.SetData_BoolArray(kOutlineFoldingCollapsed,i,true);
		}
	}
}

#pragma mark ===========================

#pragma mark ---

/*#1012
//Icon設定 win 080618
void	AView_List::SPI_SetColumnIcon( const ADataID inColumnID, const ANumberArray& inIconIDArray )
{
	mIconDataBase.SetData_NumberArray(inColumnID,inIconIDArray);
}
*/

//
void	AView_List::UpdateScrollBarUnit()
{
	const ANumber				kHScrollBarUnit = 30;
	
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(kHScrollBarUnit,GetRowHeight(),rect.right-rect.left-kHScrollBarUnit,rect.bottom-rect.top-GetRowHeight()*5);
}

//フォント等のText描画プロパティが変更された場合の処理
/*win void	AView_List::SPI_SetTextDrawProperty( const AFont inFont, const AFloatNumber inFontSize, const AColor& inColor )
{
	mFont = inFont;
	mFontSize = inFontSize;
	mColor = inColor;
	NVMC_SetDefaultTextProperty(mFont,mFontSize,mColor,kTextStyle_Normal,true);
	NVMC_GetMetricsForDefaultTextProperty(mRowHeight,mAscent);
	mRowHeight += mRowMargin;
	UpdateScrollBarUnit();
	//LineHeightが変更されたのでImageSizeも変更必要
	SetImageSize();
}*/
void	AView_List::SPI_SetTextDrawProperty( const AText& inFontName, const AFloatNumber inFontSize, const AColor& inColor )
{
	mFontName.SetText(inFontName);
	mFontSize = inFontSize;
	mColor = inColor;
	NVMC_SetDefaultTextProperty(mFontName,mFontSize,mColor,kTextStyle_Normal,true);
	NVMC_GetMetricsForDefaultTextProperty(mRowHeight,mAscent);
	mRowHeight += mRowMargin;
	UpdateScrollBarUnit();
	//LineHeightが変更されたのでImageSizeも変更必要
	SetImageSize();
}

void	AView_List::SPI_SetFontSize( const AFloatNumber inFontSize )
{
	AText	fontname(mFontName);
	SPI_SetTextDrawProperty(fontname,inFontSize,mColor);
}

//
ANumber	AView_List::GetRowHeight() const
{
	return mRowHeight;
}

AItemCount	AView_List::GetRowCount() const
{
	if( mColumnIDArray.GetItemCount() == 0 )   return 0;
	else
	{
		APrefID	colID = mColumnIDArray.Get(0);
		return mDataBase.GetItemCount_Array(colID);
	}
}

void	AView_List::SetImageSize()
{
	//win 080618
	//widthを、全ての列の幅の和か、Viewの幅のどちらか大きい方に設定
	ANumber	width = 0;
	for( AIndex i = 0; i < mColumnWidthArray.GetItemCount(); i++ )
	{
		width += mColumnWidthArray.Get(i);
	}
	if( width < NVI_GetViewWidth() )
	{
		width = NVI_GetViewWidth();
	}
	
	ANumber	height = /*#789 GetRowCount()*/SPI_GetDisplayRowCount() * GetRowHeight() + 8;
	NVM_SetImageSize(width,height);
	
	//
	AImagePoint	origin = {0};
	NVI_GetOriginOfLocalFrame(origin);
	NVI_ScrollTo(origin);
}

//
void	AView_List::GetRowImageRect( const AIndex inStartRowIndex, const AIndex inEndRowIndex, AImageRect& outRowImageRect ) const
{
	outRowImageRect.left	= 0;
	outRowImageRect.right	= NVM_GetImageWidth();
	outRowImageRect.top		= inStartRowIndex*GetRowHeight();
	outRowImageRect.bottom	= inEndRowIndex*GetRowHeight();
}

//win 080618
//セルのImageRectを取得
void	AView_List::GetCellImageRect( const APrefID inColumnID, const AIndex inRowIndex, AImageRect& outRowImageRect ) const
{
	GetRowImageRect(inRowIndex,inRowIndex+1,outRowImageRect);
	AIndex	colIndex = mColumnIDArray.Find(inColumnID);
	if( colIndex == kIndex_Invalid )   {_ACError("",this);return;}
	for( AIndex i = 0; i < colIndex; i++ )
	{
		outRowImageRect.left += mColumnWidthArray.Get(i);
	}
	if( colIndex != mColumnIDArray.GetItemCount() - 1 )
	{
		outRowImageRect.right = outRowImageRect.left + mColumnWidthArray.Get(colIndex);
	}
}


void	AView_List::ClearCursorRow()
{
	if( mCursorRowStartDisplayIndex == kIndex_Invalid )   return;
	//描画更新
	AIndex	svStartRowDisplayIndex = mCursorRowStartDisplayIndex;
	AIndex	svEndRowDisplayIndex = mCursorRowEndDisplayIndex;
	mCursorRowStartDisplayIndex = mCursorRowEndDisplayIndex = kIndex_Invalid;
	mCursorRowLeftMargin = 0;
	SPI_RefreshRows(svStartRowDisplayIndex,svEndRowDisplayIndex);
}

void	AView_List::SPI_SortOff()
{
	mSortMode = false;
	UpdateRowOrderDisplay();
}

void	AView_List::SPI_Sort( const APrefID inColumnID, const ABool inAscendant )
{
	mSortMode = true;
	mSortColumnID = inColumnID;
	mSortAscendant = inAscendant;
	UpdateRowOrderDisplay();
}

void	AView_List::QuickSort( const ADataID inColumnID, AIndex first, AIndex last, const ABool inAscendant )
{
	switch(mDataBase.GetDataType(inColumnID))
	{
	  case kDataType_TextArray:
		{
			AText	axis;
			mDataBase.GetData_TextArray(inColumnID,mRowOrderDisplayToDBArray.Get((first+last)/2),axis);
			AIndex	i = first, j = last;
			for( ; ; )
			{
				for( ; ; )
				{
					AText	data;
					mDataBase.GetData_TextArray(inColumnID,mRowOrderDisplayToDBArray.Get(i),data);
					if( inAscendant == true )
					{
						if( data.IsLessThan(axis) == true )
						{
							i++;
							continue;
						}
					}
					else
					{
						if( axis.IsLessThan(data) == true )
						{
							i++;
							continue;
						}
					}
					break;
				}
				for( ; ; )
				{
					AText	data;
					mDataBase.GetData_TextArray(inColumnID,mRowOrderDisplayToDBArray.Get(j),data);
					if( inAscendant == true )
					{
						if( axis.IsLessThan(data) == true )
						{
							j--;
							continue;
						}
					}
					else
					{
						if( data.IsLessThan(axis) == true )
						{
							j--;
							continue;
						}
					}
					break;
				}
				if( i >= j )   break;
				AIndex	tmp = mRowOrderDisplayToDBArray.Get(i);
				mRowOrderDisplayToDBArray.Set(i,mRowOrderDisplayToDBArray.Get(j));
				mRowOrderDisplayToDBArray.Set(j,tmp);
				i++;
				j--;
			}
			if( first < i-1 )   QuickSort(inColumnID,first,i-1,inAscendant);
			if( j+1 < last  )   QuickSort(inColumnID,j+1,last,inAscendant);
			break;
		}
		//#138
	  case kDataType_NumberArray:
		{
			ANumber	axis = mDataBase.GetData_NumberArray(inColumnID,mRowOrderDisplayToDBArray.Get((first+last)/2));
			AIndex	i = first, j = last;
			for( ; ; )
			{
				for( ; ; )
				{
					ANumber	data = mDataBase.GetData_NumberArray(inColumnID,mRowOrderDisplayToDBArray.Get(i));
					if( inAscendant == true )
					{
						if( data < axis )
						{
							i++;
							continue;
						}
					}
					else
					{
						if( axis < data )
						{
							i++;
							continue;
						}
					}
					break;
				}
				for( ; ; )
				{
					ANumber	data = mDataBase.GetData_NumberArray(inColumnID,mRowOrderDisplayToDBArray.Get(j));
					if( inAscendant == true )
					{
						if( axis < data )
						{
							j--;
							continue;
						}
					}
					else
					{
						if( data < axis )
						{
							j--;
							continue;
						}
					}
					break;
				}
				if( i >= j )   break;
				AIndex	tmp = mRowOrderDisplayToDBArray.Get(i);
				mRowOrderDisplayToDBArray.Set(i,mRowOrderDisplayToDBArray.Get(j));
				mRowOrderDisplayToDBArray.Set(j,tmp);
				i++;
				j--;
			}
			if( first < i-1 )   QuickSort(inColumnID,first,i-1,inAscendant);
			if( j+1 < last  )   QuickSort(inColumnID,j+1,last,inAscendant);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

void	AView_List::SPI_RefreshRow( const AIndex inDisplayRowIndex )
{
	if( inDisplayRowIndex == kIndex_Invalid )   return;
	AImageRect	rowImageRect;
	GetRowImageRect(inDisplayRowIndex,inDisplayRowIndex+1,rowImageRect);
	ALocalRect	rowLocalRect;
	NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
	NVMC_RefreshRect(rowLocalRect);
}

/**
描画更新
*/
void	AView_List::SPI_RefreshRows( const AIndex inDisplayRowStartIndex, const AIndex inDisplayRowEndIndex )
{
	AImageRect	rowImageRect;
	GetRowImageRect(inDisplayRowStartIndex,inDisplayRowEndIndex+1,rowImageRect);
	ALocalRect	rowLocalRect;
	NVM_GetLocalRectFromImageRect(rowImageRect,rowLocalRect);
	NVMC_RefreshRect(rowLocalRect);
}

//win 080618
//スクロール後処理
void	AView_List::NVIDO_ScrollPostProcess( const ANumber inDeltaX, const ANumber inDeltaY,
											 const ABool inRedraw, const ABool inConstrainToImageSize,
											 const AScrollTrigger inScrollTrigger )//#138 #1031
{
	//フレームのスクロール位置を合わせる
	if( /*#205 mFrameControlID != kControlID_Invalid*/mFrameViewID != kObjectID_Invalid )
	{
		AImagePoint	pt;
		NVI_GetOriginOfLocalFrame(pt);
		/*#205 NVM_GetWindow().NVI_GetListFrameView(mFrameControlID)*/GetFrameView().SPI_SetHeaderOffset(pt.x);
	}
	//描画更新
	NVI_Refresh();
}

//#205
/**
テーブルへのレングス0のテキスト入力を禁止する
*/
void	AView_List::SPI_SetInhibit0Length( const ABool inInhibit0Length )
{
	SPI_SetInhibit0Length(inInhibit0Length, GetEmptyText());
}
void	AView_List::SPI_SetInhibit0Length( const ABool inInhibit0Length, const AText& in0LengthText )//#1428
{
	mInhibit0Length = inInhibit0Length;
	m0LengthText.SetText(in0LengthText);//#1428
}

#pragma mark ===========================

#pragma mark ---Edit

//編集モード開始
//inColumnID: 列ID
//inRowIndex: 行インデックス（kIndex_Invalidなら選択行）
void	AView_List::SPI_StartEditMode( const APrefID inColumnID, const AIndex inRowIndex )
{
	//既に編集モードなら編集モードを終了させる
	if( mEditMode == true )
	{
		SPI_EndEditMode();
	}
	
	//列インデックス取得
	AIndex	colIndex = mColumnIDArray.Find(inColumnID);
	if( colIndex == kIndex_Invalid )   {_ACError("",this);return;}
	
	//行インデックス取得
	AIndex	rowIndex = inRowIndex;
	if( rowIndex == kIndex_Invalid )
	{
		rowIndex = mSelectedRowDisplayIndex;
	}
	if( rowIndex == kIndex_Invalid )   return;
	
	//EditBoxコントロールが存在していたら、EditBoxコントロールを削除する
	if( NVM_GetWindow().NVI_ExistView(kEditBoxControlID) == true )
	{
		NVM_GetWindow().NVI_DeleteEditBoxView(kEditBoxControlID);
	}
	
	//EditBoxコントロール生成、初期設定
	AIndex	tabIndex = NVM_GetWindow().NVI_GetControlEmbeddedTabControlIndex(NVI_GetControlID());
	AImageRect	imagerect;
	GetCellImageRect(inColumnID,rowIndex,imagerect);
	AImagePoint	imagept;
	imagept.x = imagerect.left;
	imagept.y = imagerect.top;
	ALocalPoint	localpt;
	NVM_GetLocalPointFromImagePoint(imagept,localpt);
	AWindowPoint	windowpt;
	NVM_GetWindow().NVI_GetWindowPointFromControlLocalPoint(NVI_GetControlID(),localpt,windowpt);
	AWindowPoint	pt;
	pt.x = windowpt.x;
	pt.y = windowpt.y;
	NVM_GetWindow().NVI_CreateEditBoxView(kEditBoxControlID,pt,imagerect.right-imagerect.left,imagerect.bottom-imagerect.top+kEditBoxBottomMargin,
				kControlID_Invalid,
				tabIndex,true);
	NVM_GetWindow().NVI_EmbedControl(NVI_GetControlID(),kEditBoxControlID);
	AText	text;
	switch(mDataBase.GetDataType(inColumnID))
	{
	  case kDataType_TextArray:
		{
			mDataBase.GetData_TextArray(inColumnID,rowIndex,text);
			break;
		}
	  case kDataType_NumberArray://#868
		{
			text.SetNumber(mDataBase.GetData_NumberArray(inColumnID,rowIndex));
			break;
		}
	  default:
		{
			_ACError("",NULL);
			break;
		}
	}
	NVM_GetWindow().NVI_GetEditBoxView(kEditBoxControlID).SPI_SetListViewID(NVI_GetControlID());
	NVM_GetWindow().NVI_SetControlText(kEditBoxControlID,text);
	NVM_GetWindow().NVI_GetEditBoxView(kEditBoxControlID).NVI_SetSelectAll();
	NVM_GetWindow().NVI_SwitchFocusTo(kEditBoxControlID);
	
	mEditMode = true;
	mEditModeColumnID = inColumnID;
	mEditModeRowIndex = rowIndex;
}

//編集モード終了
void	AView_List::SPI_EndEditMode()
{
	if( mEditMode == false )   return;
	if( NVM_GetWindow().NVI_ExistView(kEditBoxControlID) == false )
	{
		//他のListViewでEditBoxを開いた場合
		mEditMode = false;
		return;
	}
	//EditBoxコントロール削除
	NVM_GetWindow().NVI_DeleteEditBoxView(kEditBoxControlID);
}

//EditBoxで文字入力されたときにコールされる
//改行入力で、ListViewの編集モードを終了する
ABool	AView_List::SPI_EditBoxInput( const AUChar inChar )
{
	if( inChar == '\r' )
	{
		SPI_EndEditMode();
		return true;
	}
	return false;
}

//EditBoxが削除されたときに、EditBoxのデストラクタからコールされる。
void	AView_List::SPI_DoEditBoxDeleted( const AText& inText )
{
	//#205
	ABool	ok = true;
	if( mInhibit0Length == true )
	{
		if( inText.GetItemCount() == 0 )   ok = false;
	}
	if( ok == true )
	{
		switch(mDataBase.GetDataType(mEditModeColumnID))
		{
		  case kDataType_TextArray:
			{
				mDataBase.SetData_TextArray(mEditModeColumnID,mEditModeRowIndex,inText);
				break;
			}
		  case kDataType_NumberArray://#868
			{
				mDataBase.SetData_NumberArray(mEditModeColumnID,mEditModeRowIndex,inText.GetNumber());
				break;
			}
		  default:
			{
				_ACError("",NULL);
				break;
			}
		}
	}
	
	mEditMode = false;
	NVM_GetWindow().EVT_ValueChanged(NVI_GetControlID());
}

#pragma mark ===========================
#pragma mark [クラス]AView_ListFrame
#pragma mark ===========================
//

#if IMPLEMENTATION_FOR_WINDOWS
const AColor	kListFrameColor = kColor_Gray;
#endif
#if IMPLEMENTATION_FOR_MACOSX
const AColor	kListFrameColor = kColor_LightGray;
#endif

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_ListFrame::AView_ListFrame( const AWindowID inWindowID, const AControlID inID, const AControlID inListViewControlID,
			const ABool inHeader, const AControlID inVScrollBarControlID, const AControlID inHScrollBarControlID ) 
: AView(inWindowID,inID), 
		mHeaderEnable(inHeader), mVScrollBarControlID(inVScrollBarControlID), mHScrollBarControlID(inHScrollBarControlID)
		,mFocused(false), mListViewControlID(inListViewControlID)
,mMouseTrackingPreviousLocalPoint(kLocalPoint_00),mMouseTrackingColumnIndex(kIndex_Invalid), mHeaderOffset(0)
{
	/*win 080707 Initへ移動
	AText	fontname;
	AFontWrapper::GetDefaultFontName(fontname);
	NVMC_SetDefaultTextProperty(fontname,10.5,kColor_Black,kTextStyle_Normal,true);
	//
	SPI_UpdateBounds();
	*/
}
//デストラクタ
AView_ListFrame::~AView_ListFrame()
{
}

//Init 080707
void	AView_ListFrame::NVIDO_Init()
{
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);//#375
	NVMC_SetDefaultTextProperty(fontname,kListFrameDefaultFontSize,kColor_Black,kTextStyle_Normal,true);
	//win
	NVM_GetWindow().NVI_DisableEventCatch(NVI_GetControlID());
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_ListFrame::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//描画要求時のコールバック(AView用)
void	AView_ListFrame::EVTDO_DoDraw()
{
	//#1316
	AColor	frameColor = AColorWrapper::GetColorByHTMLFormatColor("D0D0D0");
	AColor	fillColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
	AColor	letterColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
	if( AApplication::GetApplication().NVI_IsDarkMode() == true )
	{
		frameColor = AColorWrapper::GetColorByHTMLFormatColor("505050");
		fillColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
		letterColor = AColorWrapper::GetColorByHTMLFormatColor("909090");
	}
	//
	ALocalRect	localFrame;
	NVM_GetLocalViewRect(localFrame);
	NVMC_PaintRect(localFrame,fillColor,1.0);
	//ヘッダ描画
	if( mHeaderEnable == true )
	{
		ALocalRect	headerRect = localFrame;
		headerRect.bottom = headerRect.top + kHeaderHeight;
		/*#1316
		//Mac OSのリストヘッダに似せてgradient描画
		ALocalRect	headerRect1 = headerRect;
		headerRect1.bottom = headerRect.top + kHeaderHeight/2;
		NVMC_PaintRectWithVerticalGradient(headerRect1,kColor_White,kColor_Gray90Percent,1.0,1.0);
		ALocalRect	headerRect2 = headerRect;
		headerRect2.top = headerRect.top + kHeaderHeight/2;
		NVMC_PaintRectWithVerticalGradient(headerRect2,kColor_Gray90Percent,kColor_White,1.0,1.0);
		*/
		AColor	backgroundStartColor = AColorWrapper::GetColorByHTMLFormatColor("ECECEC");//D9D9D9");
		AColor	backgroundEndColor = AColorWrapper::GetColorByHTMLFormatColor("ECECEC");//F7F7F7");
		if( AApplication::GetApplication().NVI_IsDarkMode() == true )
		{
			backgroundStartColor = AColorWrapper::GetColorByHTMLFormatColor("383838");//282828");
			backgroundEndColor = AColorWrapper::GetColorByHTMLFormatColor("303030");//"181818");
		}
		NVMC_PaintRectWithVerticalGradient(headerRect,backgroundStartColor,backgroundEndColor,1.0,1.0);
		//フレーム描画
		NVMC_FrameRect(headerRect,frameColor);//#1316 kListFrameColor,1.0);
		//
		ANumber	x = 0;
		for( AIndex i = 0; i < mColumnWidth.GetItemCount(); i++ )
		{
			//タイトル
			AText	title;
			mColumnTitle.Get(i,title);
			ALocalRect	drawRect;
			drawRect.left		= x + 7 - mHeaderOffset;
			drawRect.right		= x + mColumnWidth.Get(i) - mHeaderOffset;
			drawRect.top		= headerRect.top + 3;
			drawRect.bottom		= headerRect.bottom -1;
			//最後の列は右端まで
			if( i == mColumnWidth.GetItemCount() - 1 )
			{
				drawRect.right = headerRect.right-1;
			}
			//タイトル描画
			/*#1316
			AColor	color = kColor_Black;
			if( NVM_GetWindow().NVI_IsWindowActive() == false )
			{
				color = kColor_Gray;
			}
			*/
			NVMC_DrawText(drawRect,title,letterColor,kTextStyle_Normal);
			
			//
			x += mColumnWidth.Get(i);
			//区切り線
			if( i < mColumnWidth.GetItemCount() - 1 )
			{
				ALocalPoint	spt, ept;
				spt.x = x - mHeaderOffset;
				spt.y = headerRect.top;
				ept.x = x - mHeaderOffset;
				ept.y = headerRect.bottom-1;//win 080618
				NVMC_DrawLine(spt,ept,frameColor);//#1316 kListFrameColor);
			}
		}
	}
	//
	AColor	color = frameColor;//#1316 kListFrameColor;
	/*#1316
	if( NVM_GetWindow().NVI_IsWindowActive() == false )
	{
		color = kColor_Gray80Percent;
	}
	*/
	if( mFocused == true )
	{
		AColorWrapper::GetHighlightColor(color);
		NVMC_FrameRect(localFrame,color,1.0);
		//NVMC_DrawFocusFrame(localFrame);
	}
	else
	{
		NVMC_FrameRect(localFrame,color,1.0);
	}
}

/**
マウスボタンダウン時処理
*/
ABool	AView_ListFrame::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//前回マウス位置の保存
	mMouseTrackingPreviousLocalPoint = inLocalPoint;
	mMouseTrackingColumnIndex = kIndex_Invalid;
	ANumber	x = -mHeaderOffset;
	for( AIndex i = 0; i < mColumnWidth.GetItemCount()-1; i++ )
	{
		x += mColumnWidth.Get(i);
		if( inLocalPoint.x > x-5 && inLocalPoint.x < x+5 )
		{
			mMouseTrackingColumnIndex = i;
			break;
		}
	}
	if( mMouseTrackingColumnIndex == kIndex_Invalid )
	{
		return false;
	}
	
	//Trackingモード設定
	NVM_SetMouseTrackingMode(true);
	NVI_Refresh();
	return true;
}

/**
マウスボタン押下中TrackingのOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_ListFrame::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//前回マウス位置と同じなら何もせずループ継続
	if( inLocalPoint.x == mMouseTrackingPreviousLocalPoint.x && inLocalPoint.y == mMouseTrackingPreviousLocalPoint.y )
	{
		return;
	}
	//カラム幅変更
	ANumber	w = mColumnWidth.Get(mMouseTrackingColumnIndex);
	w += inLocalPoint.x - mMouseTrackingPreviousLocalPoint.x;
	mColumnWidth.Set(mMouseTrackingColumnIndex,w);
	NVM_GetWindow().NVI_GetListView(mListViewControlID).SPI_SetColumnWidth(mColumnID.Get(mMouseTrackingColumnIndex),w);
	//描画更新
	NVI_Refresh();
	NVM_GetWindow().NVI_GetListView(mListViewControlID).NVI_Refresh();
	//
	mMouseTrackingPreviousLocalPoint = inLocalPoint;
}

/**
マウスボタン押下中Tracking終了時のOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_ListFrame::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	NVM_SetMouseTrackingMode(false);
	mMouseTrackingColumnIndex = kIndex_Invalid;
}

/**
カーソルタイプ取得
*/
ACursorType	AView_ListFrame::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ABool	resize = false;
	ANumber	x = -mHeaderOffset;
	for( AIndex i = 0; i < mColumnWidth.GetItemCount()-1; i++ )
	{
		x += mColumnWidth.Get(i);
		if( inLocalPoint.x > x-5 && inLocalPoint.x < x+5 )
		{
			resize = true;
			break;
		}
	}
	if( resize == true )
	{
		return kCursorType_ResizeLeftRight;
	}
	else
	{
		return kCursorType_Arrow;
	}
}

//win
/**
フォーカス設定
*/
void	AView_ListFrame::SPI_SetFocused( const ABool inFocused )
{
	mFocused = inFocused;
	NVI_Refresh();
}

//列登録
void	AView_ListFrame::SPI_RegisterColumn( const ADataID inColumnID, const ANumber inWidth, const AText& inTitle )
{
	mColumnID.Add(inColumnID);
	mColumnWidth.Add(inWidth);
	mColumnTitle.Add(inTitle);
	NVI_Refresh();
}

//列の幅設定
void	AView_ListFrame::SPI_SetColumnWidth( const AIndex inIndex, const ANumber inWidth )
{
	mColumnWidth.Set(inIndex,inWidth);
	NVI_Refresh();
}

//ヘッダのスクロール位置設定
void	AView_ListFrame::SPI_SetHeaderOffset( const ANumber inHeaderOffset )
{
	mHeaderOffset = inHeaderOffset;
	NVI_Refresh();
}

