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

ADocument

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]ADocument_Text
#pragma mark ===========================
//ドキュメントのクラス
#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
ADocument::ADocument( AObjectArrayItem* inParent, const AObjectID inDocImpID ) : AObjectArrayItem(inParent), mDirty(false), mReadOnly(false)
,mPrintImp(GetObjectID())//#182
,mRetainCount(0)//#379
,mAlwaysNotDirty(false)//#567
,mFileSpecifiedAndScreened(false)
,mDocImpID(inDocImpID)//#1034
{
	//#1034
	//CDocImpへADocumentのDocumentIDを設定
	if( mDocImpID != kObjectID_Invalid )
	{
		AApplication::GetApplication().GetImp().GetDocImpByID(mDocImpID).SetDocumentID(GetObjectID());
	}
}
/**
デストラクタ
*/
ADocument::~ADocument()
{
	//CDocImp解放 #1078
	if( mDocImpID != kObjectID_Invalid )
	{
		AApplication::GetApplication().GetImp().DeleteDocImp(mDocImpID);
	}
}

//#92
/**
AObjectArrayItemをAObjectArrayから削除した直後にコールされる。
（デストラクタはGC時なので、タイミング制御できない。よって、各種削除処理はDoDeleted()で行うべき。）
DoDeleted()→NVI_DocumentWillBeDeleted()に変更。DoDeleted()はmDocumentArrayMutexによる排他内で行われるのでデッドロック考慮が必要になり、危険。
*/
void	ADocument::NVI_DocumentWillBeDeleted()
{
	NVIDO_DocumentWillBeDeleted();
}

//#379
/**
RetainCountをデクリメントし0かつ、Viewなしになったら解放する
*/
void	ADocument::NVI_Release()
{
	//RetainCountデクリメント
	mRetainCount--;
	
	//#379
	//Viewが無くなり、かつ、RetainCountも0ならドキュメント削除
	if( mViewIDArray.GetItemCount() == 0 && mRetainCount <= 0 )
	{
		AApplication::GetApplication().NVI_DeleteDocument(GetObjectID());
	}
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ドキュメント情報設定／取得

/**
ドキュメントファイルを設定（スクリーニング完了）
*/
void	ADocument::NVI_SpecifyFile( const AFileAcc& inFileAcc )
{
	mFileAcc = inFileAcc;
	//ReadOnly判定
	NVI_SetReadOnly(mFileAcc.IsReadOnly());
	NVIDO_SpecifyFile(mFileAcc);
	
	//スクリーニング完了フラグON
	mFileSpecifiedAndScreened = true;
	
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mWindowIDArray.Get(i)).NVI_UpdateTitleBar();
	}
	*/
	AApplication::GetApplication().NVI_UpdateTitleBar();
	
	//#1422
	//ファイル設定時処理（File Presenter登録）
	if( mDocImpID != kObjectID_Invalid )
	{
		AApplication::GetApplication().GetImp().GetDocImpByID(mDocImpID).FileSpecified(inFileAcc);
	}
}

/**
ドキュメントファイルを設定（未スクリーニング）
*/
void	ADocument::NVI_SpecifyFileUnscreened( const AFileAcc& inFileAcc )
{
	mFileAcc = inFileAcc;
}

/**
ドキュメントファイルを取得
*/
ABool	ADocument::NVI_GetFile( AFileAcc& outFile ) const
{
	if( NVI_IsFileSpecified() == false )   return false;
	outFile = mFileAcc;
	return true;
}

//#723
/**
ドキュメントファイルのパスを取得
*/
void	ADocument::NVI_GetFilePath( AText& outFilePath ) const
{
	outFilePath.DeleteAll();
	//ファイルパス取得
	AFileAcc	file;
	if( NVI_GetFile(file) == true )
	{
		file.GetPath(outFilePath);
	}
}

//#465
/**
親フォルダを取得
*/
ABool	ADocument::NVI_GetParentFolder( AFileAcc& outFolder ) const
{
	if( NVI_IsFileSpecified() == false )   return false;
	outFolder.SpecifyParent(mFileAcc);
	return true;
}

/**
Dirty属性を設定
*/
void	ADocument::NVI_SetDirty( const ABool inDirty )
{
	//#81
	//派生クラス処理
	NVIDO_SetDirty(inDirty);
	
	//常にDirtyにしない設定なら何もしない #567
	if( mAlwaysNotDirty == true )   return;
	//
	ABool	svDirty = mDirty;
	if( inDirty )
	{
		if( NVI_IsReadOnly() == false )
		{
			mDirty = true;
		}
		//globals->mHookManager->TouchHook();
	}
	else
	{
		mDirty = false;
	}
	if( svDirty != mDirty )
	{
		/*#379
		for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
		{
			AApplication::GetApplication().NVI_GetWindowByID(mWindowIDArray.Get(i)).NVI_UpdateTitleBar();
		}
		*/
		AApplication::GetApplication().NVI_UpdateTitleBar();
	}
}

/**
Dirty属性を取得
*/
ABool	ADocument::NVI_IsDirty() const
{
	return mDirty;
}

//#567
/**
常にDirtyにしない設定
*/
void	ADocument::NVI_SetAlwaysNotDirty()
{
	mAlwaysNotDirty = true;
}

/**
読み込み専用属性を設定
*/
void	ADocument::NVI_SetReadOnly( const ABool inReadOnly )
{
	mReadOnly = inReadOnly;
	//タイトルバー更新 #0
	AApplication::GetApplication().NVI_UpdateTitleBar();
}

/*#699 NVIDO_に変更
ABool	ADocument::NVI_IsReadOnly() const
{
	return mReadOnly;
}
*/

/**
ドキュメントファイル設定済みかどうかを取得
*/
ABool	ADocument::NVI_IsFileSpecified() const
{
	return mFileAcc.IsSpecified();
}

/*#182
void	ADocument::NVI_UpdateProperty()
{
	NVIDO_UpdateProperty();
}
*/

#pragma mark ===========================

#pragma mark ---View

//#92
/**
ViewID登録
*/
void	ADocument::NVI_RegisterView( const AViewID inViewID )
{
	mViewIDArray.Add(inViewID);
	//#379
	NVIDO_ViewRegistered(inViewID);
}

/**
ViewID登録解除
*/
void	ADocument::NVI_UnregisterView( const AViewID inViewID )
{
	AIndex	index = mViewIDArray.Find(inViewID);
	if( index == kIndex_Invalid )   { _ACError("viewID not found",this); return; }
	
	//#893
	//派生クラス処理
	NVIDO_ViewUnregistered(inViewID);
	
	//ViewID arrayから削除
	mViewIDArray.Delete1(index);
	
	//#379
	//Viewが無くなり、かつ、RetainCountも0ならドキュメント削除
	if( mViewIDArray.GetItemCount() == 0 && mRetainCount <= 0 )
	{
		AApplication::GetApplication().NVI_DeleteDocument(GetObjectID());
	}
}

/**
Documentを表示するViewを最前面に持ってくる
*/
void	ADocument::NVI_RevealDocumentWithAView() const
{
	if( mViewIDArray.GetItemCount() == 0 )   return;
	
	//Documentに登録されている最初のViewをRevealする
	AApplication::GetApplication().NVI_GetViewByID(mViewIDArray.Get(0)).NVI_RevealView(true,false);//#492
}

#pragma mark ===========================

#pragma mark ---印刷

//#182
/**
PageSetupダイアログを表示
*/
void	ADocument::NVI_PrintPageSetup( const AWindowID inWindowID )
{
	mPrintImp.PageSetup(AApplication::GetApplication().NVI_GetWindowByID(inWindowID).NVI_GetWindowRef());
}

//#182
/**
印刷ダイアログを表示
*/
void	ADocument::NVI_Print( const AWindowID inWindowID, const AText& inJobTitle )
{
	mPrintImp.Print(AApplication::GetApplication().NVI_GetWindowByID(inWindowID).NVI_GetWindowRef(),inJobTitle);
}



