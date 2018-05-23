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

AWindow_Fusen

*/

#include "stdafx.h"

#include "AWindow_Fusen.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_Fusen
#pragma mark ===========================
//付箋紙表示ウインドウ

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ

@param inTextView 対応するTextViewのViewID
@param inProjectObjectID 表示付箋データ主キー（プロジェクトのObjectID）
@param inRelativePath 表示付箋データ主キー（相対パス）
@param inModuleName 表示付箋データ主キー（モジュール名）
@param inOffset 表示付箋データ主キー（オフセット）
*/
AWindow_Fusen::AWindow_Fusen( const AViewID inTextViewID,
			const AObjectID inProjectObjectID, const AText& inRelativePath, 
			const AText& inModuleName, const ANumber inOffset ) 
			:AWindow(), mTextViewID(inTextViewID), mEditBoxViewControlID(kControlID_Invalid)
{
	//編集対象付箋データ主キー保存
	mProjectObjectID = inProjectObjectID;
	mRelativePath.SetText(inRelativePath);
	mModuleName.SetText(inModuleName);
	mOffset = inOffset;
}
/**
デストラクタ
*/
AWindow_Fusen::~AWindow_Fusen()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

//付箋に対応するTextViewを取得
AView_Text&	AWindow_Fusen::GetTextView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_Text,kViewType_Text,mTextViewID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_Fusen::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	if( inID == mEditBoxViewControlID )
	{
		SPI_EditFusen();
		result = true;
	}
	return result;
}

//ウインドウサイズ変更通知時のコールバック
void	AWindow_Fusen::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	//ウインドウ内の各Viewの位置・サイズを更新
	UpdateViewBounds();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成（AWindow::Show()からコールされる）
void	AWindow_Fusen::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ実体生成
	NVM_CreateWindow(kWindowClass_Toolbar);
	NVI_SetWindowTransparency(0.8);
	
	AWindowPoint	pt = {0,0};
	//EditBox生成
	mEditBoxViewControlID = NVM_AssignDynamicControlID();
	NVI_CreateEditBoxView(mEditBoxViewControlID,pt,10,10,kControlID_Invalid,kIndex_Invalid,true);
	//EditBox設定
	AText	defaultfontname;
	AFontWrapper::GetAppDefaultFontName(defaultfontname);//#375
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetTextDrawProperty(defaultfontname,9.0,kColor_Gray20Percent,kColor_Gray20Percent);
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetBackgroundColor(kColor_LightYellow,kColor_Bisque,kColor_Bisque);
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetEnableEdiit(false);
	//現在のコメントテキスト取得、EditBoxへ設定
	AText	text;
	GetApp().SPI_GetExternalCommentFromProjectDB(mProjectObjectID,mRelativePath,mModuleName,mOffset,text);
	NVI_SetControlText(mEditBoxViewControlID,text);
	//ウインドウのサイズ変更
	ANumber	width = NVI_GetEditBoxView(mEditBoxViewControlID).SPI_GetMaxDisplayWidth() 
			+ NVI_GetEditBoxView(mEditBoxViewControlID).NVI_GetFrameLeftMargin()
			+ NVI_GetEditBoxView(mEditBoxViewControlID).NVI_GetFrameRightMargin();
	if( width < kMinWidth )   width = kMinWidth;
	ANumber	height = NVI_GetEditBoxView(mEditBoxViewControlID).NVI_GetImageHeight()
			+ NVI_GetEditBoxView(mEditBoxViewControlID).NVI_GetFrameTopMargin()
			+ NVI_GetEditBoxView(mEditBoxViewControlID).NVI_GetFrameBottomMargin();
	if( height < kMinHeight )   height = kMinHeight;
	NVI_SetWindowSize(width,height);
	UpdateViewBounds();
	//MouseWheelRedirect
	NVI_GetEditBoxView(mEditBoxViewControlID).NVI_SetMouseWheelEventRedirectViewID(mTextViewID);
	/*win
	//フローティング設定
	NVI_SetFloating();
	*/
}

/**
ウインドウ内の各Viewの位置・サイズをウインドウサイズに合わせて更新する
*/
void	AWindow_Fusen::UpdateViewBounds()
{
	//ウインドウ位置・サイズ取得
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	
	//FileListView bounds設定
	AWindowPoint	pt;
	pt.x = 0;
	pt.y = 0;
	ANumber	viewwidth = windowBounds.right - windowBounds.left;
	ANumber	viewheight = windowBounds.bottom - windowBounds.top;
	//EditBox bounds設定
	NVI_GetViewByControlID(mEditBoxViewControlID).NVI_SetPosition(pt);
	NVI_GetViewByControlID(mEditBoxViewControlID).NVI_SetSize(viewwidth,viewheight);
}

/**
付箋紙編集ウインドウを表示し、表示ウインドウをHide
*/
void	AWindow_Fusen::SPI_EditFusen()
{
	//自ウインドウよりもちょっと大きめのbounds取得
	ARect	bounds;
	NVI_GetWindowBounds(bounds);
	ARect	rect = bounds;
	rect.right += kEditWindowWidthDelta;
	rect.bottom += kEditWindowHeightDelta;
	//付箋紙編集ウインドウ表示
	GetApp().SPI_GetFusenEditWindow().SPNVI_Show(mProjectObjectID,mRelativePath,mModuleName,mOffset,rect,mTextViewID);
	//自ウインドウは隠す（TextView上の管理データは残ったままになる。次回のTextViewからの更新時（再描画等）に、再表示される。）
	NVI_Hide();
}
