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

AWindow_FusenEdit

*/

#include "stdafx.h"

#include "AWindow_FusenEdit.h"
#include "AApp.h"
#include "AView_Text.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_FusenEdit
#pragma mark ===========================
//インデックスウインドウのクラス

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ

付箋紙編集ウインドウは、Appオブジェクトが常に１つだけ所有する。
必要に応じて内容・位置・サイズを設定して表示する。
*/
AWindow_FusenEdit::AWindow_FusenEdit() :AWindow(), mProjectObjectID(kObjectID_Invalid), mTextViewID(kObjectID_Invalid), mEditBoxViewControlID(kControlID_Invalid)
{
	//キャレットブリンクのためタイマーイベントを有効にする
	NVI_AddToTimerActionWindowArray();
}
//デストラクタ
AWindow_FusenEdit::~AWindow_FusenEdit()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

//付箋を貼り付けたTextViewを取得
AView_Text&	AWindow_FusenEdit::GetTextView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_Text,kViewType_Text,mTextViewID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//ウインドウサイズ変更通知時のコールバック
void	AWindow_FusenEdit::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	if( NVI_IsWindowCreated() == false )   return;
	
	UpdateViewBounds();
}

//WindowDeactivated時のコールバック
void	AWindow_FusenEdit::EVTDO_WindowDeactivated()
{
	NVI_Hide();
}

//ウインドウの閉じるボタン
void	AWindow_FusenEdit::EVTDO_DoCloseButton()
{
	NVI_Hide();
}

//メニュー選択時のコールバック（処理を行ったらtrueを返す）
ABool	AWindow_FusenEdit::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	switch(inMenuItemID)
	{
	  case kMenuItemID_Close:
		{
			NVI_Hide();
			return true;
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成（AWindow::Show()からコールされる）
void	AWindow_FusenEdit::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ実体生成
	NVM_CreateWindow("main/FusenEdit");
	NVI_SetWindowTransparency(0.8);
	
	AWindowPoint	pt = {0,0};
	//EditBox生成
	mEditBoxViewControlID = NVM_AssignDynamicControlID();
	NVI_CreateEditBoxView(mEditBoxViewControlID,pt,10,10,kControlID_Invalid,kIndex_Invalid,true);
	//フォント・背景色色等設定
	AText	defaultfontname;
	AFontWrapper::GetAppDefaultFontName(defaultfontname);//#375
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetTextDrawProperty(defaultfontname,9.0,kColor_Gray20Percent,kColor_Gray20Percent);
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetBackgroundColor(kColor_LightYellow,kColor_Bisque,kColor_Bisque);
	NVI_SetCatchReturn(mEditBoxViewControlID,true);//#135
	NVI_SetCatchTab(mEditBoxViewControlID,true);//#135
}

void	AWindow_FusenEdit::UpdateViewBounds()
{
	//ウインドウbounds取得
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	
	//EditBox bounds設定
	AWindowPoint	pt;
	pt.x = 0;
	pt.y = 0;
	ANumber	viewwidth = windowBounds.right - windowBounds.left;
	ANumber	viewheight = windowBounds.bottom - windowBounds.top;
	NVI_GetViewByControlID(mEditBoxViewControlID).NVI_SetPosition(pt);
	NVI_GetViewByControlID(mEditBoxViewControlID).NVI_SetSize(viewwidth,viewheight);
}

/**
ウインドウをHideしたときに呼ばれる
*/
void	AWindow_FusenEdit::NVIDO_Hide()
{
	ApplyData();
	mProjectObjectID = kObjectID_Invalid;
	mTextViewID = kObjectID_Invalid;
}

/**
表示

@param inProjectObjectID 編集対象付箋データ主キー（プロジェクトのObjectID）
@param inRelativePath 編集対象付箋データ主キー（相対パス）
@param inModuleName 編集対象付箋データ主キー（モジュール名）
@param inOffset 編集対象付箋データ主キー（オフセット）
@param inBounds 表示Rect
*/
void	AWindow_FusenEdit::SPNVI_Show( const AObjectID inProjectObjectID, const AText& inRelativePath, 
		const AText& inModuleName, const ANumber inOffset,
		const ARect& inDisplayBounds, 
		const AViewID inTextViewID )
{
	//既に開いている場合は、一旦データを反映する。
	if( NVI_IsWindowVisible() == true )
	{
		ApplyData();
	}
	
	//対応TextView設定
	mTextViewID = inTextViewID;
	
	//編集対象付箋データ主キー保存
	mProjectObjectID = inProjectObjectID;
	mRelativePath.SetText(inRelativePath);
	mModuleName.SetText(inModuleName);
	mOffset = inOffset;
	
	//現在のコメントテキスト取得、EditBoxへ設定
	AText	text;
	GetApp().SPI_GetExternalCommentFromProjectDB(mProjectObjectID,mRelativePath,mModuleName,mOffset,text);
	NVI_SetControlText(mEditBoxViewControlID,text);
	
	//ウインドウbounds設定
	NVI_SetWindowBounds(inDisplayBounds);
	UpdateViewBounds();
	
	//ウインドウ表示
	NVI_Show();
	
	//フォーカス移動
	NVI_SwitchFocusTo(mEditBoxViewControlID);
	//テキストの最後を選択
	NVI_GetEditBoxView(mEditBoxViewControlID).SPI_SetSelect(text.GetItemCount(),text.GetItemCount());
}

/**
データをProjectDBへ保存する

付箋紙編集ウインドウをHideした場合などに呼ばれる
*/
void	AWindow_FusenEdit::ApplyData()
{
	//コメントテキストをEditBoxから取得し、ProjectDBへ設定
	AText	text;
	NVI_GetControlText(mEditBoxViewControlID,text);
	GetApp().SPI_SetExternalCommentToProjectDB(mProjectObjectID,mRelativePath,mModuleName,mOffset,text);
	//ウインドウ位置を取得し、TextViewとの相対位置から、ModuleName, Offsetを変更する
	if( mTextViewID != kObjectID_Invalid && GetApp().NVI_ExistView(mTextViewID) == true )
	{
		//位置を現在のウインドウ位置と、TextViewから取得し、ProjectDBへ設定
		APoint	pt;
		NVI_GetWindowPosition(pt);
		AText	newModuleName;
		ANumber	newOffset;
		GetTextView().SPI_GetFusenModuleAndOffset(pt,newModuleName,newOffset);
		GetApp().SPI_UpdateExternalCommentPosition(mProjectObjectID,mRelativePath,mModuleName,mOffset,newModuleName,newOffset);
		//内部データ更新（この後Hideするとは限らないかもしれないので、念のため主キー更新しておく）
		mModuleName.SetText(newModuleName);
		mOffset = newOffset;
	}
}

