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

AProjectFolderLoader
#402

*/

#include "stdafx.h"

#include "AProjectFolderLoader.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AProjectFolderLoader
#pragma mark ===========================
//マルチファイル検索実行

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AProjectFolderLoader::AProjectFolderLoader( AObjectArrayItem* inParent ) : AThread(inParent), mDontDisplayInvisible(false)
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/**
スレッドメインルーチン
*/
void	AProjectFolderLoader::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AProjectFolderLoader::NVIDO_ThreadMain started.",this);
	try
	{
		//3秒間ウエイト
		GetApp().NVI_SleepWithTimer(3);
		//結果全削除
		mResult.DeleteAll();
		mFolders.DeleteAll();
		//フォルダロードを再帰的に実行
		for( AIndex i = 0; i < mRequestArray_ProjectFolder.GetItemCount(); i++ )
		{
			mResult.AddNewObject();
			AFileAcc	folder;
			folder.Specify(mRequestArray_ProjectFolder.GetTextConst(i));
			LoadFolderRecursive(i,folder,0);
		}
	}
	catch(...)
	{
		_ACError("AProjectFolderLoader::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	AObjectIDArray	objectIDArray;
	objectIDArray.Add(GetObjectID());
	ABase::PostToMainInternalEventQueue(kInternalEvent_ProjectFolderLoaderResult,GetObjectID(),0,GetEmptyText(),objectIDArray);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AProjectFolderLoader::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---スレッド起動

/**
スレッド起動（プロジェクト１つ）
*/
void	AProjectFolderLoader::SPNVI_Run( const AFileAcc& inProjectFolder, const ABool inDontDisplayInvisible )
{
	if( NVI_IsThreadWorking() == true )   {_ACError("",this);return;}
	
	//要求データをコピー
	mRequestArray_ProjectFolder.DeleteAll();
	AText	path;
	inProjectFolder.GetPath(path);
	mRequestArray_ProjectFolder.Add(path);
	mDontDisplayInvisible = inDontDisplayInvisible;
	
	//スレッド起動
	NVI_Run();
}

/**
スレッド起動（プロジェクト複数）
*/
void	AProjectFolderLoader::SPNVI_Run( const AObjectArray<AFileAcc>& inProjectFolderArray, const ABool inDontDisplayInvisible )
{
	if( NVI_IsThreadWorking() == true )   {_ACError("",this);return;}
	
	//要求データをコピー
	mRequestArray_ProjectFolder.DeleteAll();
	for( AIndex i = 0; i < inProjectFolderArray.GetItemCount(); i++ )
	{
		AText	projectFolderPath;
		inProjectFolderArray.GetObjectConst(i).GetPath(projectFolderPath);
		mRequestArray_ProjectFolder.Add(projectFolderPath);
	}
	mDontDisplayInvisible = inDontDisplayInvisible;
	
	//スレッド起動
	NVI_Run();
}

/**
フォルダ読み込み
mSameProject_Pathnameの内容作成（プロジェクトフォルダを再帰的に検索して全ファイル登録）
mSameProject_Pathnameの指定Indexの配列はこの関数内では削除されない（削除せずに追加していく）ので、関数コール元で削除必要
*/
void	AProjectFolderLoader::LoadFolderRecursive( const AIndex inRequestIndex, const AFileAcc& inFolder, const AIndex inDepth )
{
	//ロードしたフォルダを記録（SCM用）
	mFolders.GetObject(mFolders.AddNewObject()).CopyFrom(inFolder);
	
	//#892 ファイルリスト階層化
	//結果にフォルダのパスを追加
	if( inDepth > 0 )
	{
		AText	folderpath;
		inFolder.GetPath(folderpath);
		folderpath.AddLastPathDelimiter();
		folderpath.ConvertToCanonicalComp();//#1087
		mResult.GetObject(inRequestIndex).Add(folderpath);
	}
	
	//フォルダ内ファイル取得
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		//スレッドabortされたら終了
		if( NVM_IsAborted() == true )
		{
			break;
		}
		
		//
		AFileAcc	child;
		child.CopyFrom(children.GetObjectConst(index));
		//不可視ファイルを表示しない設定ならmSameProject_Pathnameに追加しない
		//#533 不可視ファイルは常に非表示にする（処理に負荷かかるため） if( mDontDisplayInvisible == true )
		{
			if( child.IsInvisible() == true )   continue;
		}
		//
		if( child.IsFolder() == true && child.IsLink() == false )//#0 シンボリックリンク・エイリアスによる無限ループ防止
		{
			LoadFolderRecursive(inRequestIndex,child,inDepth+1);
		}
		else
		{
			AText	path;
			child.GetPath(path);
			path.ConvertToCanonicalComp();//#1087
			mResult.GetObject(inRequestIndex).Add(path);
		}
	}
}

#pragma mark ===========================

#pragma mark ---結果取得

/**
要求データ取得
*/
void	AProjectFolderLoader::SPI_GetRequestedData( ATextArray& outProjectFolderArray ) const
{
	outProjectFolderArray.SetFromTextArray(mRequestArray_ProjectFolder);
}

/**
結果取得
*/
void	AProjectFolderLoader::SPI_GetResult( const AIndex inRequestIndex, ATextArray& outResult ) const
{
	outResult.SetFromTextArray(mResult.GetObjectConst(inRequestIndex));
}

/**
ロードしたフォルダリスト取得
*/
void	AProjectFolderLoader::SPI_GetLoadedFolders( AObjectArray<AFileAcc>& outFolders ) const
{
	for( AIndex i = 0; i < mFolders.GetItemCount(); i++ )
	{
		outFolders.GetObject(outFolders.AddNewObject()).CopyFrom(mFolders.GetObjectConst(i));
	}
}



