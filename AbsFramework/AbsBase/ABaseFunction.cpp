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

ABaseFunction

*/

#include "stdafx.h"

#include "ABase.h"

extern ABase*	gBase;

#pragma mark ===========================
#pragma mark [クラス]ABaseFunction
#pragma mark ===========================
//AbsBaseの全体的な機構に対する制御を行う関数群

/**
現在メインスレッド内かどうかの判定
*/
ABool	ABaseFunction::InMainThread()
{
	if( gBase != NULL )
	{
		return gBase->InMainThread();
	}
	else
	{
		//gBaseがまだ用意できていないということは、まだスレッドも起動していないはず。
		return true;
	}
}

#pragma mark ===========================

#pragma mark ---AObjectArrayItemの削除制御

/**
ObjectArrayをガーベージコレクション対象にする
*/
void	ABaseFunction::AddToObjectArrayItemTrash( AObjectArrayItem* inObjectPtr )
{
	if( gBase != NULL )
	{
		if( gBase->InMainThread() == true )
		{
			gBase->mObjectArrayItemTrashForMainThread.AddToObjectArrayItemTrash(inObjectPtr);
		}
		else
		{
			//メインスレッド外なら即delete
			delete inObjectPtr;
		}
	}
}

/**
ガーベージコレクション実行
*/
void	ABaseFunction::DoObjectArrayItemTrashDelete()
{
	if( gBase != NULL )
	{
		if( gBase->InMainThread() == true )
		{
			gBase->mObjectArrayItemTrashForMainThread.DoObjectArrayItemTrashDelete();
		}
		else
		{
			//メインスレッド外なら処理無し
		}
	}
}

/*#344 メモリープール機構削除（仮想メモリ環境では使用される状況がまずありえないので）

#pragma mark ===========================
#pragma mark ---メモリ管理

//仮想メモリ環境下ではあまり存在価値がないのでメモリプール処理は削除するかもしれない
void	ABaseFunction::ShowLowMemoryAlertLevel1()
{
}

ABool	ABaseFunction::IsLevel1MemoryPoolReleased()
{
	if( gBase != NULL )
	{
		if( gBase->mLevel1MemoryPool != NULL )
		{
			return false;
		}
	}
	return true;
}

ABool	ABaseFunction::IsLevel2MemoryPoolReleased()
{
	if( gBase != NULL )
	{
		if( gBase->mLevel2MemoryPool != NULL )
		{
			return false;
		}
	}
	return true;
}

void	ABaseFunction::ReleaseMemoryPoolLevel1()
{
	if( gBase != NULL )
	{
		if( gBase->mLevel1MemoryPool != NULL )
		{
			AMemoryWrapper::Free(gBase->mLevel1MemoryPool);
			gBase->mLevel1MemoryPool = NULL;
		}
	}
}

void	ABaseFunction::ReleaseMemoryPoolLevel2()
{
	if( gBase != NULL )
	{
		if( gBase->mLevel2MemoryPool != NULL )
		{
			AMemoryWrapper::Free(gBase->mLevel2MemoryPool);
			gBase->mLevel2MemoryPool = NULL;
		}
	}
}
*/
#pragma mark ===========================
#pragma mark ---エラー発生時処理

extern void	BackupDataForCriticalError();

/**
致命的エラー
*/
void	ABaseFunction::CriticalError( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem )
{
	//データバックアップ
	BackupDataForCriticalError();
	
	//デバッガ起動
	ABaseFunction::ShowDebugger();
	
	if( gBase != NULL )
	{
		//トレースログ
		ATrace::Log("[CError]",true,inFilename,inLineNumber,inString,true,inObjectArrayItem,true);//B0000 backtrace
	}
}

//#182
/**
致命的エラー・エラー番号付き
*/
void	ABaseFunction::CriticalError( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, 
		const ANumber inErrorNumber, const AObjectArrayItem* inObjectArrayItem )
{
	//データバックアップ
	BackupDataForCriticalError();
	
	//デバッガ起動
	ABaseFunction::ShowDebugger();
	
	//エラーテキスト作成
	AText	text;
	text.SetCstring(inString);
	text.AddNumber(inErrorNumber);
	
	if( gBase != NULL )
	{
		AStCreateCstringFromAText	cstr(text);
		//トレースログ
		ATrace::Log("[CError]",true,inFilename,inLineNumber,cstr.GetPtr(),true,inObjectArrayItem,true);//B0000 backtrace
	}
}

/**
致命的Throw
*/
void	ABaseFunction::CriticalThrow( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem )
{
	//データバックアップ
	BackupDataForCriticalError();
	
	//デバッガ起動
	ABaseFunction::ShowDebugger();
	
	if( gBase != NULL )
	{
		//トレースログ
		ATrace::Log("[CThrow]",true,inFilename,inLineNumber,inString,true,inObjectArrayItem,true);//B0000 backtrace
	}
	
	//throw
	throw inString;
}

/**
エラー発生
*/
void	ABaseFunction::Error( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem )
{
	//デバッガ起動
	ABaseFunction::ShowDebugger();
	
	if( gBase != NULL )
	{
		//トレースログ
		ATrace::Log("[Error ]",true,inFilename,inLineNumber,inString,false,inObjectArrayItem,true);//B0000 backtrace
	}
}

/**
Throw発生
*/
void	ABaseFunction::Throw( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem )
{
	//デバッガ起動
	ABaseFunction::ShowDebugger();
	
	if( gBase != NULL )
	{
		//トレースログ
		ATrace::Log("[Throw ]",true,inFilename,inLineNumber,inString,false,inObjectArrayItem,true);//B0000 backtrace
	}
	
	//throw
	throw inString;
}

/**
単なる情報ログのため（エラーではない）
*/
void	ABaseFunction::Info( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem )
{
	if( gBase != NULL )
	{
		//トレースログ
		ATrace::Log("  [Info]",false,inFilename,inLineNumber,inString,false,inObjectArrayItem,false);//B0000 backtrace
	}
}

/**
デバッガ表示
*/
void	ABaseFunction::ShowDebugger()
{
#if IMPLEMENTATION_FOR_MACOSX
	//Xcode3.1でうまく動作しない？Debugger();
#endif
}

#pragma mark ===========================

#pragma mark ---統計データ
//#271

//AHashArray
AItemCount	gStatics_AHashArray_MakeHashCount = 0;
AItemCount	gStatics_AHashArray_FindCount = 0;
AItemCount	gStatics_AHashArray_FirstDataNull = 0;
AItemCount	gStatics_AHashArray_FirstDataHit = 0;
AItemCount	gStatics_AHashArray_CompareCount = 0;
//AHashObjectArray
AItemCount	gStatics_AHashObjectArray_MakeHashCount = 0;
//AObjectArrayItem
AItemCount	gStatics_AObjectArrayItem_MakeHashCount = 0;
AItemCount	gStatics_AObjectArrayItem_FindCount = 0;
AItemCount	gStatics_AObjectArrayItem_FirstDataNull = 0;
AItemCount	gStatics_AObjectArrayItem_FirstDataHit = 0;
AItemCount	gStatics_AObjectArrayItem_CompareCount = 0;

/**
ハッシュ統計表示
*/
void	ABaseFunction::HashStatics()
{
	fprintf(stderr,"\n");
	{
		fprintf(stderr,"[AHashArray]\n");
		fprintf(stderr,"   MakeHashCount:%ld\n",gStatics_AHashArray_MakeHashCount);
		fprintf(stderr,"   FindCount:%ld  (FirstDataNull:%ld  FirstDataHit:%ld  NotFirstData:%ld)\n",
				gStatics_AHashArray_FindCount,gStatics_AHashArray_FirstDataNull,gStatics_AHashArray_FirstDataHit,
				gStatics_AHashArray_FindCount-gStatics_AHashArray_FirstDataNull-gStatics_AHashArray_FirstDataHit);
		AFloatNumber	averagecompare = gStatics_AHashArray_CompareCount;
		averagecompare /= gStatics_AHashArray_FindCount;
		fprintf(stderr,"   AverageCompareCount:%g\n",averagecompare);
	}
	{
		fprintf(stderr,"[AHashObjectArray]\n");
		fprintf(stderr,"   MakeHashCount:%ld\n",gStatics_AHashObjectArray_MakeHashCount);
	}
	{
		AItemCount	hashsize, itemcount, nodata, deleted;
		ANumber	nextcandidate;
		AObjectArrayItem::GetStatics(hashsize,itemcount,nodata,deleted,nextcandidate);
		fprintf(stderr,"[AObjectArrayItem]\n");
		fprintf(stderr,"   NextCandidateID:%ld\n",nextcandidate);
		fprintf(stderr,"   MakeHashCount:%ld\n",gStatics_AObjectArrayItem_MakeHashCount);
		fprintf(stderr,"   HashSize:%ld  (ItemCount:%ld  NoData:%ld  Deleted:%ld)\n",hashsize,itemcount,nodata,deleted);
		fprintf(stderr,"   FindCount:%ld  (FirstDataNull:%ld  FirstDataHit:%ld  NotFirstData:%ld)\n",
				gStatics_AObjectArrayItem_FindCount,gStatics_AObjectArrayItem_FirstDataNull,gStatics_AObjectArrayItem_FirstDataHit,
				gStatics_AObjectArrayItem_FindCount-gStatics_AObjectArrayItem_FirstDataNull-gStatics_AObjectArrayItem_FirstDataHit);
		AFloatNumber	averagecompare = gStatics_AObjectArrayItem_CompareCount;
		averagecompare /= gStatics_AObjectArrayItem_FindCount;
		fprintf(stderr,"   AverageCompareCount:%g\n",averagecompare);
	}
}

