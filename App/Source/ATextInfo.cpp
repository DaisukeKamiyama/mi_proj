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

ATextInfo

*/

#include "stdafx.h"

#include "ATextInfo.h"
#include "AApp.h"
#include "ASyntaxDefinition.h"
//#include "CTextDrawData.h"

#pragma mark ===========================
#pragma mark [クラス]ALineInfo
#pragma mark ===========================

/**
コンストラクタ
*/
ALineInfo::ALineInfo() : mIsPurged(false), mReallocateStep(kLineInfoAllocationStep),
		mLineInfoIDArray(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorage_UniqID(kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorage_Start(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorage_Length(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorage_ExistLineEnd(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorage_FirstGlobalIdentifierUniqID(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorage_ParagraphIndex(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorage_RecognizeFlags(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_StateIndex(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		//drop mLineInfoStorageP_CheckedDate(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_PushedStateIndex(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_FirstLocalIdentifierUniqID(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_LineColor(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_IndentCount(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_FirstBlockStartDataUniqID(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		//drop mLineInfoStorageP_Multiply(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_DirectiveLevel(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_DisabledDirectiveLevel(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_FoldingLevel(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_CharCount(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep),
		mLineInfoStorageP_WordCount(NULL,kLineInfoInitialAllocation,kLineInfoAllocationStep)
{
}

/**
デストラクタ
*/
ALineInfo::~ALineInfo()
{
}

#pragma mark ===========================

#pragma mark ---挿入・削除

/*
mLineInfoStorage_***: 行データを格納するテーブル。
mLineInfoStorage_UniqIDにてUniqIDを割り振り、対応するindexの各要素にデータを格納する。
途中挿入・途中削除は一切行わない。削除するときは、mLineInfoStorage_UniqIDに削除マークをつける。
（必ずしも行の順番には要素が並ばない。indexは行番号ではない）

mLineInfoIDArray: 上記テーブルのUniqIDを行番号順に並べた配列。
行追加・削除時は、こちらだけ途中挿入・途中削除を行う。
*/

/**
行挿入
*/
void	ALineInfo::InsertLines( const AIndex inLineIndex, const AItemCount inLineCount )
{
	//mLineInfoIDArrayの領域確保
	mLineInfoIDArray.Reserve(inLineIndex,inLineCount);
	//各行毎に、未使用storage要素の再利用 or 新規storage要素の追加 を行う
	for( AIndex lineIndex = inLineIndex; lineIndex < inLineIndex + inLineCount; lineIndex++ )
	{
		//Storage内の未使用要素を検索し、あればそこに割り当てる
		AIndex	storageIndex = mLineInfoStorage_UniqID.ReuseUnusedItem();
		if( storageIndex != kIndex_Invalid )
		{
			//未使用要素を再利用する。
			//初期値を設定
			mLineInfoStorage_Start.Set(storageIndex,0);
			mLineInfoStorage_Length.Set(storageIndex,0);
			mLineInfoStorage_ExistLineEnd.Set(storageIndex,true);
			mLineInfoStorage_FirstGlobalIdentifierUniqID.Set(storageIndex,kUniqID_Invalid);
			mLineInfoStorage_ParagraphIndex.Set(storageIndex,kIndex_Invalid);
			mLineInfoStorage_RecognizeFlags.Set(storageIndex,kLineInfoRecognizeFlagMask_None);
			mLineInfoStorageP_StateIndex.Set(storageIndex,0);
			mLineInfoStorageP_PushedStateIndex.Set(storageIndex,0);
			mLineInfoStorageP_FirstLocalIdentifierUniqID.Set(storageIndex,kUniqID_Invalid);
			mLineInfoStorageP_LineColor.Set(storageIndex,kColor_Black);
			mLineInfoStorageP_IndentCount.Set(storageIndex,0);
			mLineInfoStorageP_FirstBlockStartDataUniqID.Set(storageIndex,kUniqID_Invalid);
			//drop mLineInfoStorageP_Multiply.Set(storageIndex,100);
			mLineInfoStorageP_DirectiveLevel.Set(storageIndex,0);
			mLineInfoStorageP_DisabledDirectiveLevel.Set(storageIndex,kIndex_Invalid);
			mLineInfoStorageP_FoldingLevel.Set(storageIndex,kFoldingLevel_None);
			//drop mLineInfoStorageP_CheckedDate.Set(storageIndex,0);//#842
			mLineInfoStorageP_CharCount.Set(storageIndex,0);//#142
			mLineInfoStorageP_WordCount.Set(storageIndex,0);//#142
		}
		else
		{
			//新規要素を追加する。
			storageIndex = mLineInfoStorage_UniqID.AddItem();
			mLineInfoStorage_Start.Add(0);
			mLineInfoStorage_Length.Add(0);
			mLineInfoStorage_ExistLineEnd.Add(true);
			mLineInfoStorage_FirstGlobalIdentifierUniqID.Add(kUniqID_Invalid);
			mLineInfoStorage_ParagraphIndex.Add(kIndex_Invalid);
			mLineInfoStorage_RecognizeFlags.Add(kLineInfoRecognizeFlagMask_None);
			mLineInfoStorageP_StateIndex.Add(0);
			mLineInfoStorageP_PushedStateIndex.Add(0);
			mLineInfoStorageP_FirstLocalIdentifierUniqID.Add(kUniqID_Invalid);
			mLineInfoStorageP_LineColor.Add(kColor_Black);
			mLineInfoStorageP_IndentCount.Add(0);
			mLineInfoStorageP_FirstBlockStartDataUniqID.Add(kUniqID_Invalid);
			//drop mLineInfoStorageP_Multiply.Add(100);
			mLineInfoStorageP_DirectiveLevel.Add(0);
			mLineInfoStorageP_DisabledDirectiveLevel.Add(kIndex_Invalid);
			mLineInfoStorageP_FoldingLevel.Add(kFoldingLevel_None);
			//drop mLineInfoStorageP_CheckedDate.Add(0);//#842
			mLineInfoStorageP_CharCount.Add(0);//#142
			mLineInfoStorageP_WordCount.Add(0);//#142
		}
		//mLineInfoIDArrayの指定行位置に、追加したStorage要素のuniqIDを追加
		AUniqID	uniqID = mLineInfoStorage_UniqID.Get(storageIndex);
		mLineInfoIDArray.Set(lineIndex,uniqID);
	}
}

/**
行挿入（一行）
*/
void	ALineInfo::Insert1Line( const AIndex inLineIndex )
{
	InsertLines(inLineIndex,1);
}

/**
行挿入（最後に挿入）
*/
AIndex	ALineInfo::Add1Line()
{
	AIndex	lineIndex = mLineInfoIDArray.GetItemCount();
	Insert1Line(lineIndex);
	return lineIndex;
}

/**
行削除
*/
void	ALineInfo::DeleteLines( const AIndex inLineIndex, const AItemCount inDeleteCount )
{
	for( AIndex lineIndex = inLineIndex; lineIndex < inLineIndex + inDeleteCount; lineIndex++ )
	{
		//行のUniqIDを取得
		AUniqID	uniqID = mLineInfoIDArray.Get(lineIndex);
		//Storageの該当uniqID要素を未使用要素にする。
		AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
		mLineInfoStorage_UniqID.Unuse1Item(storageIndex);
		mLineInfoStorage_ParagraphIndex.Set(storageIndex,kIndex_Invalid);//GetLineIndexFromParagraphIndex()等のため、削除時にparagraphはkIndex_Invalidを設定
	}
	//mLineInfoIDArrayの指定行を削除
	mLineInfoIDArray.Delete(inLineIndex,inDeleteCount);
}

/**
行削除
*/
void	ALineInfo::Delete1Line( const AIndex inLineIndex )
{
	DeleteLines(inLineIndex,1);
}

/**
全削除し、Purge状態も解除する
*/
void	ALineInfo::DeleteAllAndCancelPurge()
{
	//行全削除
	mLineInfoIDArray.DeleteAll();
	//Storage全削除
	mLineInfoStorage_UniqID.DeleteAll();
	mLineInfoStorage_Start.DeleteAll();
	mLineInfoStorage_Length.DeleteAll();
	mLineInfoStorage_ExistLineEnd.DeleteAll();
	mLineInfoStorage_FirstGlobalIdentifierUniqID.DeleteAll();
	mLineInfoStorage_ParagraphIndex.DeleteAll();
	mLineInfoStorage_RecognizeFlags.DeleteAll();
	mLineInfoStorageP_StateIndex.DeleteAll();
	mLineInfoStorageP_PushedStateIndex.DeleteAll();
	mLineInfoStorageP_FirstLocalIdentifierUniqID.DeleteAll();
	mLineInfoStorageP_LineColor.DeleteAll();
	mLineInfoStorageP_IndentCount.DeleteAll();
	mLineInfoStorageP_FirstBlockStartDataUniqID.DeleteAll();
	//drop mLineInfoStorageP_Multiply.DeleteAll();
	mLineInfoStorageP_DirectiveLevel.DeleteAll();
	mLineInfoStorageP_DisabledDirectiveLevel.DeleteAll();
	mLineInfoStorageP_FoldingLevel.DeleteAll();
	//drop mLineInfoStorageP_CheckedDate.DeleteAll();//#842
	mLineInfoStorageP_CharCount.DeleteAll();//#142
	mLineInfoStorageP_WordCount.DeleteAll();//#142
	//Purge状態解除
	mIsPurged = false;
}

/**
行数拡大時の再割り当て増加量の設定
*/
void	ALineInfo::SetReallocateStep( const AItemCount inReallocateCount )
{
	//増加量設定（kLineInfoAllocationStepより小さくはしない）
	AItemCount	reallocationStep = inReallocateCount;
	if( reallocationStep < kLineInfoAllocationStep )
	{
		reallocationStep = kLineInfoAllocationStep;
	}
	//
	mLineInfoIDArray.SetReallocateStep(reallocationStep);
	//
	mLineInfoStorage_UniqID.SetReallocateStep(reallocationStep);
	mLineInfoStorage_Start.SetReallocateStep(reallocationStep);
	mLineInfoStorage_Length.SetReallocateStep(reallocationStep);
	mLineInfoStorage_ExistLineEnd.SetReallocateStep(reallocationStep);
	mLineInfoStorage_FirstGlobalIdentifierUniqID.SetReallocateStep(reallocationStep);
	mLineInfoStorage_ParagraphIndex.SetReallocateStep(reallocationStep);
	//
	mLineInfoStorage_RecognizeFlags.SetReallocateStep(reallocationStep);
	mLineInfoStorageP_StateIndex.SetReallocateStep(reallocationStep);
	mLineInfoStorageP_PushedStateIndex.SetReallocateStep(reallocationStep);
	mLineInfoStorageP_FirstLocalIdentifierUniqID.SetReallocateStep(reallocationStep);
	mLineInfoStorageP_LineColor.SetReallocateStep(reallocationStep);
	mLineInfoStorageP_IndentCount.SetReallocateStep(reallocationStep);
	mLineInfoStorageP_FirstBlockStartDataUniqID.SetReallocateStep(reallocationStep);
	//drop mLineInfoStorageP_Multiply.SetReallocateStep(reallocationStep);
	mLineInfoStorageP_DirectiveLevel.SetReallocateStep(reallocationStep);
	mLineInfoStorageP_DisabledDirectiveLevel.SetReallocateStep(reallocationStep);
	mLineInfoStorageP_FoldingLevel.SetReallocateStep(reallocationStep);
	//drop mLineInfoStorageP_CheckedDate.SetReallocateStep(reallocationStep);//#842
	mLineInfoStorageP_CharCount.SetReallocateStep(reallocationStep);//#142
	mLineInfoStorageP_WordCount.SetReallocateStep(reallocationStep);//#142
	//ReallocateStep記憶（AView_Textで参照するため）
	mReallocateStep = reallocationStep;
}

#pragma mark ===========================

#pragma mark ---Get/Set

/**
Start取得
*/
AIndex	ALineInfo::GetLineInfo_Start( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorage_Start.Get(storageIndex);
}

//#853
/**
LineUniqueIDからStart取得
*/
AIndex	ALineInfo::GetLineInfo_Start_FromLineUniqID( const AUniqID inLineUniqID ) const
{
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(inLineUniqID);
	return mLineInfoStorage_Start.Get(storageIndex);
}

/**
Start設定
*/
void	ALineInfo::SetLineInfo_Start( const AIndex inLineIndex, const AIndex inStart )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorage_Start.Set(storageIndex,inStart);
}

/**
Length取得
*/
AItemCount	ALineInfo::GetLineInfo_Length( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorage_Length.Get(storageIndex);
}

/**
Length設定
*/
void	ALineInfo::SetLineInfo_Length( const AIndex inLineIndex, const AItemCount inLength )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorage_Length.Set(storageIndex,inLength);
}

/**
ExistLineEnd取得
*/
ABool	ALineInfo::GetLineInfo_ExistLineEnd( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorage_ExistLineEnd.Get(storageIndex);
}

/**
ExistLineEnd設定
*/
void	ALineInfo::SetLineInfo_ExistLineEnd( const AIndex inLineIndex, const ABool inExistLineEnd )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorage_ExistLineEnd.Set(storageIndex,inExistLineEnd);
}

/**
FirstGlobalIdentifierUniqID取得
*/
AUniqID	ALineInfo::GetLineInfo_FirstGlobalIdentifierUniqID( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorage_FirstGlobalIdentifierUniqID.Get(storageIndex);
}

/**
FirstGlobalIdentifierUniqID設定
*/
void	ALineInfo::SetLineInfo_FirstGlobalIdentifierUniqID( const AIndex inLineIndex, const AUniqID inFirstGlobalIdentifierUniqID )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorage_FirstGlobalIdentifierUniqID.Set(storageIndex,inFirstGlobalIdentifierUniqID);
}

/**
ParagraphIndex取得
*/
AIndex	ALineInfo::GetLineInfo_ParagraphIndex( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorage_ParagraphIndex.Get(storageIndex);
}

/**
ParagraphIndex設定
*/
void	ALineInfo::SetLineInfo_ParagraphIndex( const AIndex inLineIndex, const AIndex inParagraphIndex )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorage_ParagraphIndex.Set(storageIndex,inParagraphIndex);
}

/**
Recognizedフラグ消去
*/
void	ALineInfo::ClearRecognizeFlags( const AIndex inLineIndex )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	//フラグ消去
	mLineInfoStorage_RecognizeFlags.Set(storageIndex,kLineInfoRecognizeFlagMask_None);
}

/**
Recognized取得
*/
ABool	ALineInfo::GetLineInfoP_Recognized( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_Recognized)!=0);
}

/**
Recognized設定
*/
void	ALineInfo::SetLineInfoP_Recognized( const AIndex inLineIndex, const ABool inRecognized )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	if( inRecognized == true )
	{
		mLineInfoStorage_RecognizeFlags.Set(storageIndex,
					((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) | kLineInfoRecognizeFlagMask_Recognized));
	}
	else
	{
		mLineInfoStorage_RecognizeFlags.Set(storageIndex,
					((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) & (~kLineInfoRecognizeFlagMask_Recognized)));
	}
}

/**
StateIndex取得
*/
short int	ALineInfo::GetLineInfoP_StateIndex( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_StateIndex.Get(storageIndex);
}

/**
StateIndex設定
*/
void	ALineInfo::SetLineInfoP_StateIndex( const AIndex inLineIndex, const short int inStateIndex )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_StateIndex.Set(storageIndex,inStateIndex);
}

/**
PushedStateIndex取得
*/
short int	ALineInfo::GetLineInfoP_PushedStateIndex( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_PushedStateIndex.Get(storageIndex);
}

/**
PushedStateIndex設定
*/
void	ALineInfo::SetLineInfoP_PushedStateIndex( const AIndex inLineIndex, const short int inPushedStateIndex )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_PushedStateIndex.Set(storageIndex,inPushedStateIndex);
}

/**
FirstLocalIdentifierUniqID取得
*/
AUniqID	ALineInfo::GetLineInfoP_FirstLocalIdentifierUniqID( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_FirstLocalIdentifierUniqID.Get(storageIndex);
}

/**
FirstLocalIdentifierUniqID設定
*/
void	ALineInfo::SetLineInfoP_FirstLocalIdentifierUniqID( const AIndex inLineIndex, const AUniqID inFirstLocalIdentifierUniqID )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_FirstLocalIdentifierUniqID.Set(storageIndex,inFirstLocalIdentifierUniqID);
}

/**
ColorizeLine取得
*/
ABool	ALineInfo::GetLineInfoP_ColorizeLine( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_ColorizeLine)!=0);
}

/**
ColorizeLine設定
*/
void	ALineInfo::SetLineInfoP_ColorizeLine( const AIndex inLineIndex, const ABool inColorizeLine )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	if( inColorizeLine == true )
	{
		mLineInfoStorage_RecognizeFlags.Set(storageIndex,
					((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) | kLineInfoRecognizeFlagMask_ColorizeLine));
	}
	else
	{
		mLineInfoStorage_RecognizeFlags.Set(storageIndex,
					((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) & (~kLineInfoRecognizeFlagMask_ColorizeLine)));
	}
}

/**
LineColor取得
*/
AColor	ALineInfo::GetLineInfoP_LineColor( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_LineColor.Get(storageIndex);
}

/**
LineColor設定
*/
void	ALineInfo::SetLineInfoP_LineColor( const AIndex inLineIndex, const AColor inLineColor )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_LineColor.Set(storageIndex,inLineColor);
}

/**
IndentCount取得
*/
short int	ALineInfo::GetLineInfoP_IndentCount( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_IndentCount.Get(storageIndex);
}

/**
IndentCount設定
*/
void	ALineInfo::SetLineInfoP_IndentCount( const AIndex inLineIndex, const short int inIndentCount )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_IndentCount.Set(storageIndex,inIndentCount);
}

/**
FirstBlockStartDataUniqID取得
*/
AUniqID	ALineInfo::GetLineInfoP_FirstBlockStartDataUniqID( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_FirstBlockStartDataUniqID.Get(storageIndex);
}

/**
FirstBlockStartDataUniqID設定
*/
void	ALineInfo::SetLineInfoP_FirstBlockStartDataUniqID( const AIndex inLineIndex, const AUniqID inBlockStartDataUniqID )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_FirstBlockStartDataUniqID.Set(storageIndex,inBlockStartDataUniqID);
}

/**
Multiply取得
*/
short int	ALineInfo::GetLineInfoP_Multiply( const AIndex inLineIndex ) const
{
	return 100;
	/*drop
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_Multiply.Get(storageIndex);
	*/
}

/**
Multiply設定
*/
void	ALineInfo::SetLineInfoP_Multiply( const AIndex inLineIndex, const short int inMultiply )
{
	/*drop
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_Multiply.Set(storageIndex,inMultiply);
	*/
}

/**
DirectiveLevel取得
*/
short int	ALineInfo::GetLineInfoP_DirectiveLevel( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_DirectiveLevel.Get(storageIndex);
}

/**
DirectiveLevel設定
*/
void	ALineInfo::SetLineInfoP_DirectiveLevel( const AIndex inLineIndex, const short int inDirectiveLevel )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_DirectiveLevel.Set(storageIndex,inDirectiveLevel);
}

/**
DisabledDirectiveLevel取得
*/
short int	ALineInfo::GetLineInfoP_DisabledDirectiveLevel( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_DisabledDirectiveLevel.Get(storageIndex);
}

/**
DisabledDirectiveLevel設定
*/
void	ALineInfo::SetLineInfoP_DisabledDirectiveLevel( const AIndex inLineIndex, const short int inDisabledDirectiveLevel )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_DisabledDirectiveLevel.Set(storageIndex,inDisabledDirectiveLevel);
}

/**
FoldingLevel取得
*/
AFoldingLevel	ALineInfo::GetLineInfoP_FoldingLevel( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_FoldingLevel.Get(storageIndex);
}

/**
FoldingLevel設定
*/
void	ALineInfo::SetLineInfoP_FoldingLevel( const AIndex inLineIndex, const AFoldingLevel inFoldingLevel )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_FoldingLevel.Set(storageIndex,inFoldingLevel);
}

/**
MenuIdentifierExist取得
*/
ABool	ALineInfo::GetLineInfoP_MenuIdentifierExist( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_MenuIdentifierExist)!=0);
}

/**
MenuIdentifierExist設定
*/
void	ALineInfo::SetLineInfoP_MenuIdentifierExist( const AIndex inLineIndex, const ABool inMenuIdentifierExist )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	if( inMenuIdentifierExist == true )
	{
		mLineInfoStorage_RecognizeFlags.Set(storageIndex,
					((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) | kLineInfoRecognizeFlagMask_MenuIdentifierExist));
	}
	else
	{
		mLineInfoStorage_RecognizeFlags.Set(storageIndex,
					((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) & (~kLineInfoRecognizeFlagMask_MenuIdentifierExist)));
	}
}

/**
Localデリミタ存在フラグ取得
*/
ABool	ALineInfo::GetLineInfoP_LocalDelimiterExist( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_LocalDelimiterExist)!=0);
}

/**
Localデリミタ存在フラグ設定
*/
void	ALineInfo::SetLineInfoP_LocalDelimiterExist( const AIndex inLineIndex, const ABool inLocalDelimiterExist )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	if( inLocalDelimiterExist == true )
	{
		mLineInfoStorage_RecognizeFlags.Set(storageIndex,
					((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) | kLineInfoRecognizeFlagMask_LocalDelimiterExist));
	}
	else
	{
		mLineInfoStorage_RecognizeFlags.Set(storageIndex,
					((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) & (~kLineInfoRecognizeFlagMask_LocalDelimiterExist)));
	}
}

/**
SyntaxWarning存在フラグ取得
*/
ABool	ALineInfo::GetLineInfoP_SyntaxWarningExist( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_SyntaxWarning)!=0);
}

/**
SyntaxWarning存在フラグ設定
*/
void	ALineInfo::SetLineInfoP_SyntaxWarningExist( const AIndex inLineIndex, const ABool inSyntaxWarning )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	if( inSyntaxWarning == true )
	{
		mLineInfoStorage_RecognizeFlags.
		Set(storageIndex,
			((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) | kLineInfoRecognizeFlagMask_SyntaxWarning));
	}
	else
	{
		mLineInfoStorage_RecognizeFlags.
		Set(storageIndex,
			((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) & (~kLineInfoRecognizeFlagMask_SyntaxWarning)));
	}
}

/**
SyntaxError存在フラグ取得
*/
ABool	ALineInfo::GetLineInfoP_SyntaxErrorExist( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_SyntaxError)!=0);
}

/**
SyntaxError存在フラグ設定
*/
void	ALineInfo::SetLineInfoP_SyntaxErrorExist( const AIndex inLineIndex, const ABool inSyntaxError )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	if( inSyntaxError == true )
	{
		mLineInfoStorage_RecognizeFlags.
		Set(storageIndex,
			((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) | kLineInfoRecognizeFlagMask_SyntaxError));
	}
	else
	{
		mLineInfoStorage_RecognizeFlags.
		Set(storageIndex,
			((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) & (~kLineInfoRecognizeFlagMask_SyntaxError)));
	}
}

/**
Anchor存在フラグ取得
*/
ABool	ALineInfo::GetLineInfoP_AnchorIdentifierExist( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_AnchorIdentifierExist)!=0);
}

/**
Anchor存在フラグ設定
*/
void	ALineInfo::SetLineInfoP_AnchorIdentifierExist( const AIndex inLineIndex, const ABool inAnchorExist )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	if( inAnchorExist == true )
	{
		mLineInfoStorage_RecognizeFlags.
		Set(storageIndex,
			((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) | kLineInfoRecognizeFlagMask_AnchorIdentifierExist));
	}
	else
	{
		mLineInfoStorage_RecognizeFlags.
		Set(storageIndex,
			((mLineInfoStorage_RecognizeFlags.Get(storageIndex)) & (~kLineInfoRecognizeFlagMask_AnchorIdentifierExist)));
	}
}

//#842
/**
行チェック日時取得
*/
ANumber	ALineInfo::GetLineInfoP_CheckedDate( const AIndex inLineIndex ) const
{
	return 0;
	/*drop
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_CheckedDate.Get(storageIndex);
	*/
}

//#842
/**
行チェック日時設定
*/
void	ALineInfo::SetLineInfoP_CheckedDate( const AIndex inLineIndex, const ANumber inDate )
{
	/*drop
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_CheckedDate.Set(storageIndex,inDate);
	*/
}

//#142
/**
行文字数取得
*/
AItemCount	ALineInfo::GetLineInfoP_CharCount( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_CharCount.Get(storageIndex);
}

//#142
/**
行文字数設定
*/
void	ALineInfo::SetLineInfoP_CharCount( const AIndex inLineIndex, const AItemCount inCount )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_CharCount.Set(storageIndex,inCount);
}

//#142
/**
行単語数取得
*/
AItemCount	ALineInfo::GetLineInfoP_WordCount( const AIndex inLineIndex ) const
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_WordCount.Get(storageIndex);
}

//#142
/**
行単語数設定
*/
void	ALineInfo::SetLineInfoP_WordCount( const AIndex inLineIndex, const AItemCount inCount )
{
	//行のUniqIDを取得
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_WordCount.Set(storageIndex,inCount);
}

/**
LineUniqID取得
*/
AUniqID	ALineInfo::GetLineUniqID( const AIndex inLineIndex ) const
{
	return mLineInfoIDArray.Get(inLineIndex);
}

/**
LineUniqIDから行index取得
@note 速度O(行数)
*/
AIndex	ALineInfo::FindLineIndexFromLineUniqID( const AUniqID inLineUniqID ) const
{
	return mLineInfoIDArray.Find(inLineUniqID);
}

/**
LineUniqIDからparagraph index取得
*/
AIndex	ALineInfo::GetParagraphIndexFromLineUniqID( const AUniqID inLineUniqID ) const
{
	//UniqIDに対応するStorage内データを取得
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(inLineUniqID);
	return mLineInfoStorage_ParagraphIndex.Get(storageIndex);
}

/**
Paragraph indexから行index取得
*/
AIndex	ALineInfo::GetLineIndexFromParagraphIndex( const AIndex inParagraphIndex ) const
{
	AItemCount	itemCount = mLineInfoStorage_ParagraphIndex.GetItemCount();
	AStArrayPtr<AIndex>	arrayptr(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p = arrayptr.GetPtr();
	//段落indexが一致する最小の行indexを検索する
	AIndex	minLineIndex = kNumber_MaxNumber;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( p[i] == inParagraphIndex )//削除時にparagraphはkIndex_Invalidを設定しているので、無効storage行には一致しない
		{
			//段落indexが一致したら、そのstorage行のuniqIDを取得し、uniqIDから行indexを取得
			AUniqID	uniqID = mLineInfoStorage_UniqID.Get(i);
			AIndex	lineIndex = mLineInfoIDArray.Find(uniqID);
			//最小の行indexを取得
			if( lineIndex < minLineIndex )
			{
				minLineIndex = lineIndex;
			}
		}
	}
	if( minLineIndex == kNumber_MaxNumber )
	{
		//kNumber_MaxNumberのままなら該当段落なし
		return kIndex_Invalid;
	}
	else
	{
		//該当段落の最小行indexを返す
		return minLineIndex;
	}
}

/**
行の最初から検索して最初に見つけた未認識行を返す
*/
AIndex	ALineInfo::FindFirstUnrecognizedLine() const
{
	//Recognizedフラグがfalseの最初の行のindexを取得する
	AItemCount	itemCount = mLineInfoStorage_RecognizeFlags.GetItemCount();
	AStArrayPtr<unsigned short int>	arrayptr_flags(mLineInfoStorage_RecognizeFlags,0,itemCount);
	unsigned short int*	p_flags = arrayptr_flags.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	//段落indexが最小となる、未認識行を検索する
	AIndex	minParagraphIndex = kNumber_MaxNumber;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//Recognizedがfalseかどうか
		if( ((p_flags[i]) & kLineInfoRecognizeFlagMask_Recognized) == 0 )
		{
			//未認識行なら、最小のparagraphを記憶
			AIndex	para = p_paragraph[i];
			if( para != kIndex_Invalid && para < minParagraphIndex )//削除時にparagraphはkIndex_Invalidを設定している
			{
				minParagraphIndex = para;
			}
		}
	}
	if( minParagraphIndex != kNumber_MaxNumber )
	{
		//未認識の最小段落に対応する行indexを返す
		return GetLineIndexFromParagraphIndex(minParagraphIndex);
	}
	else
	{
		//未認識なし
		return kIndex_Invalid;
	}
	//速度計測結果：3.5M項目、unrecognizedなしで約7～12ms
	//unrecognizedありは頻繁には発生しないので、
	//10M項目の平均は30ms程度か。
}

/**
指定inLineIndexに対応するmenu identifierを取得
*/
AUniqID	ALineInfo::FindCurrentMenuIdentifier( const AIndex inLineIndex ) const
{
	//現在段落を取得
	AIndex	paragraphIndex = GetLineInfo_ParagraphIndex(inLineIndex);
	//ポインタ取得
	AItemCount	itemCount = mLineInfoStorage_RecognizeFlags.GetItemCount();
	AStArrayPtr<unsigned short int>	arrayptr_flags(mLineInfoStorage_RecognizeFlags,0,itemCount);
	unsigned short int*	p_flags = arrayptr_flags.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	AStArrayPtr<AUniqID>	arrayptr_uniqID(mLineInfoStorage_FirstGlobalIdentifierUniqID,0,itemCount);
	AUniqID*	p_uniqID = arrayptr_uniqID.GetPtr();
	//現在段落への行indexオフセットが0以上かつ最小となるmenu identifierを検索
	AIndex	minOffset = kNumber_MaxNumber;
	AUniqID	uniqID = kUniqID_Invalid;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//MenuIdentifierExistがtrueかどうか
		if( ((p_flags[i]) & kLineInfoRecognizeFlagMask_MenuIdentifierExist) != 0 )
		{
			//現在段落へのオフセットが最小を記憶
			AIndex	para = p_paragraph[i];
			AIndex	offset = paragraphIndex - para;
			if( para != kIndex_Invalid && offset >= 0 && offset < minOffset )//削除時にparagraphはkIndex_Invalidを設定している
			{
				minOffset = offset;
				uniqID = p_uniqID[i];
			}
		}
	}
	//★さらにこのuniqIDにつながっているもののうち本当に対象となるオブジェクトを検索する必要有る
	return uniqID;
}

/**
指定inLineIndexに対応するローカル範囲開始identifierを取得
*/
AUniqID	ALineInfo::FindCurrentLocalStartIdentifier( const AIndex inLineIndex ) const
{
	//現在段落を取得
	AIndex	paragraphIndex = GetLineInfo_ParagraphIndex(inLineIndex);
	//ポインタ取得
	AItemCount	itemCount = mLineInfoStorage_RecognizeFlags.GetItemCount();
	AStArrayPtr<unsigned short int>	arrayptr_flags(mLineInfoStorage_RecognizeFlags,0,itemCount);
	unsigned short int*	p_flags = arrayptr_flags.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	AStArrayPtr<AUniqID>	arrayptr_uniqID(mLineInfoStorage_FirstGlobalIdentifierUniqID,0,itemCount);
	AUniqID*	p_uniqID = arrayptr_uniqID.GetPtr();
	//現在段落への行indexオフセットが0以上かつ最小となるmenu identifierを検索
	AIndex	minOffset = kNumber_MaxNumber;
	AUniqID	uniqID = kUniqID_Invalid;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//MenuIdentifierExistがtrueかどうか
		if( ((p_flags[i]) & kLineInfoRecognizeFlagMask_LocalDelimiterExist) != 0 )
		{
			//現在段落へのオフセットが最小を記憶
			AIndex	para = p_paragraph[i];
			AIndex	offset = paragraphIndex - para;
			if( para != kIndex_Invalid && offset >= 0 && offset < minOffset )//削除時にparagraphはkIndex_Invalidを設定している
			{
				minOffset = offset;
				uniqID = p_uniqID[i];
			}
		}
	}
	//★さらにこのuniqIDにつながっているもののうち本当に対象となるオブジェクトを検索する必要有る
	return uniqID;
}

/**
指定inLineIndexに対応するAnchor identifierを取得
*/
AUniqID	ALineInfo::FindAnchorIdentifier( const AIndex inLineIndex ) const
{
	//現在段落を取得
	AIndex	paragraphIndex = GetLineInfo_ParagraphIndex(inLineIndex);
	//ポインタ取得
	AItemCount	itemCount = mLineInfoStorage_RecognizeFlags.GetItemCount();
	AStArrayPtr<unsigned short int>	arrayptr_flags(mLineInfoStorage_RecognizeFlags,0,itemCount);
	unsigned short int*	p_flags = arrayptr_flags.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	AStArrayPtr<AUniqID>	arrayptr_uniqID(mLineInfoStorage_FirstGlobalIdentifierUniqID,0,itemCount);
	AUniqID*	p_uniqID = arrayptr_uniqID.GetPtr();
	//現在段落への行indexオフセットが0以上かつ最小となるanchor identifierを検索
	AIndex	minOffset = kNumber_MaxNumber;
	AUniqID	uniqID = kUniqID_Invalid;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//AnchorIdentifierExistがtrueかどうか
		if( ((p_flags[i]) & kLineInfoRecognizeFlagMask_AnchorIdentifierExist) != 0 )
		{
			//現在段落へのオフセットが最小を記憶
			AIndex	para = p_paragraph[i];
			AIndex	offset = paragraphIndex - para;
			if( para != kIndex_Invalid && offset >= 0 && offset < minOffset )//削除時にparagraphはkIndex_Invalidを設定している
			{
				minOffset = offset;
				uniqID = p_uniqID[i];
			}
		}
	}
	//★さらにこのuniqIDにつながっているもののうち本当に対象となるオブジェクトを検索する必要有る
	return uniqID;
}

//#695
/**
ローカル範囲取得
@note ローカルデリミタが全く存在しない場合は、開始・終了ともinvalidで返す
*/
void	ALineInfo::FindLocalRange( const AIndex inLineIndex, AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const
{
	//現在段落を取得
	AIndex	paragraphIndex = GetLineInfo_ParagraphIndex(inLineIndex);
	//ポインタ取得
	AItemCount	itemCount = mLineInfoStorage_RecognizeFlags.GetItemCount();
	AStArrayPtr<unsigned short int>	arrayptr_flags(mLineInfoStorage_RecognizeFlags,0,itemCount);
	unsigned short int*	p_flags = arrayptr_flags.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	//ローカルdelimiterが存在する行のうち、現在段落へのオフセットが0以上で最小のもの、負で最大（絶対値が最小）のものを記憶
	AIndex	minPlusOffset = kNumber_MaxNumber;
	AIndex	minPlusOffsetParagraph = kIndex_Invalid;
	AIndex	maxMinusOffset = kNumber_MinNumber;
	AIndex	maxMinusOffsetParagraph = kIndex_Invalid;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//LocalDelimiterExistがtrueかどうか
		if( ((p_flags[i]) & kLineInfoRecognizeFlagMask_LocalDelimiterExist) != 0 )
		{
			//現在段落へのオフセットが最小を記憶
			AIndex	para = p_paragraph[i];
			AIndex	offset = paragraphIndex - para;
			//オフセットが0以上で最小のものを記憶
			if( para != kIndex_Invalid && offset >= 0 && offset < minPlusOffset )//削除時にparagraphはkIndex_Invalidを設定している
			{
				minPlusOffset = offset;
				minPlusOffsetParagraph = para;
			}
			//オフセットが負で最大（絶対値が最小）のものを記憶
			if( para != kIndex_Invalid && offset < 0 && offset > maxMinusOffset )
			{
				maxMinusOffset = offset;
				maxMinusOffsetParagraph = para;
			}
		}
	}
	//ローカルデリミタが全く存在しない場合は、開始・終了ともinvalidで返す
	if( minPlusOffset == kNumber_MaxNumber && maxMinusOffset == kNumber_MinNumber )
	{
		outStartLineIndex = outEndLineIndex = kIndex_Invalid;
		return;
	}
	//ローカル範囲開始位置
	if( minPlusOffset != kNumber_MaxNumber )
	{
		outStartLineIndex = GetLineIndexFromParagraphIndex(minPlusOffsetParagraph);
	}
	else
	{
		outStartLineIndex = 0;
	}
	//ローカル範囲終了位置
	if( maxMinusOffset != kNumber_MinNumber )
	{
		outEndLineIndex = GetLineIndexFromParagraphIndex(maxMinusOffsetParagraph);
	}
	else
	{
		outEndLineIndex = GetLineCount();
	}
}

//#142
/**
単語数合計取得
*/
void	ALineInfo::GetTotalWordCount( AItemCount& outTotalCharCount, AItemCount& outTotalWordCount ) const
{
	//結果初期化
	outTotalCharCount = 0;
	outTotalWordCount = 0;
	//ポインタ取得
	AItemCount	itemCount = mLineInfoStorageP_CharCount.GetItemCount();
	AStArrayPtr<AItemCount>	arrayptr_charcount(mLineInfoStorageP_CharCount,0,itemCount);
	AIndex*	p_charcount = arrayptr_charcount.GetPtr();
	AStArrayPtr<AItemCount>	arrayptr_wordcount(mLineInfoStorageP_WordCount,0,itemCount);
	AIndex*	p_wordcount = arrayptr_wordcount.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	//削除行以外の文字数・単語数合計計算
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( p_paragraph[i] != kIndex_Invalid )
		{
			outTotalCharCount += p_charcount[i];
			outTotalWordCount += p_wordcount[i];
		}
	}
}

#pragma mark ===========================

#pragma mark ---Purge

/**
ImportFileData用として不要なデータを削除する（メモリ節約のため）
（インポートファイル用なのでグローバル識別子等は残す）
*/
void	ALineInfo::Purge()
{
	mLineInfoStorageP_StateIndex.DeleteAll();
	mLineInfoStorageP_PushedStateIndex.DeleteAll();
	mLineInfoStorageP_FirstLocalIdentifierUniqID.DeleteAll();
	mLineInfoStorageP_LineColor.DeleteAll();
	mLineInfoStorageP_IndentCount.DeleteAll();
	mLineInfoStorageP_FirstBlockStartDataUniqID.DeleteAll();
	//drop mLineInfoStorageP_Multiply.DeleteAll();
	mLineInfoStorageP_DirectiveLevel.DeleteAll();
	mLineInfoStorageP_DisabledDirectiveLevel.DeleteAll();
	mLineInfoStorageP_FoldingLevel.DeleteAll();
	//drop mLineInfoStorageP_CheckedDate.DeleteAll();//#842
	mLineInfoStorageP_CharCount.DeleteAll();//#142
	mLineInfoStorageP_WordCount.DeleteAll();//#142
	//Purge状態に設定
	mIsPurged = true;
}

#pragma mark ===========================

#pragma mark ---

/**
指定行移行のLineStartをずらす
*/
void	ALineInfo::ShiftLineStarts( const AIndex inStartLineIndex, const AItemCount inAddedTextLength )
{
	//inStartLineIndexが最終行より後なら何もせず終了
	if( inStartLineIndex >= GetLineCount() )
	{
		return;
	}
	//ずらし対象のtext indexを取得（これ以降のlineStartをずらす）
	AIndex	lineStartToBeShifted = GetLineInfo_Start(inStartLineIndex);
	//mLineInfoStorage_Startの各要素のうちlineStartToBeShifted移行のものを全てずらす
	AItemCount	itemCount = mLineInfoStorage_Start.GetItemCount();
	AStArrayPtr<ANumber>	arrayptr(mLineInfoStorage_Start,0,itemCount);
	ANumber*	p = arrayptr.GetPtr();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( p[i] >= lineStartToBeShifted )
		{
			p[i] += inAddedTextLength;
		}
	}
}

//#695
/**
指定行以降のparagraph indexをずらす
*/
void	ALineInfo::ShiftParagraphIndex( const AIndex inStartLineIndex, const AItemCount inParagraphCount )
{
	//inStartLineIndexが最終行より後なら何もせず終了
	if( inStartLineIndex >= GetLineCount() )
	{
		return;
	}
	
	//ずらし対象のparagraph indexを取得（これ以降のparagraph indexをずらす）
	//inStartLineIndexの行の段落は対象外とする。次の段落以降を対象とする。
	//inStartLineIndexの行の段落は段落途中の可能性があるため、下のブロックで処理。
	{
		AIndex	paragraphIndexToBeShifted = GetLineInfo_ParagraphIndex(inStartLineIndex) + 1;
		//mLineInfoStorage_Startの各要素のうちlineStartToBeShifted移行のものを全てずらす
		AItemCount	itemCount = mLineInfoStorage_ParagraphIndex.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mLineInfoStorage_ParagraphIndex,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			if( p[i] >= paragraphIndexToBeShifted )
			{
				p[i] += inParagraphCount;
			}
		}
	}
	
	//対象行以降で改行が見つかるまで、+inParagraphCountしていく
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = inStartLineIndex; lineIndex < lineCount; lineIndex++ )
	{
		SetLineInfo_ParagraphIndex(lineIndex,GetLineInfo_ParagraphIndex(lineIndex) + inParagraphCount);
		//fprintf(stderr,"ShiftParagraphIndex:line:%ld result:%ld\n",lineIndex,GetLineInfo_ParagraphIndex(lineIndex));
		if( GetLineInfo_ExistLineEnd(lineIndex) == true )
		{
			break;
		}
	}
}

/**
メインスレッド側の現在の行情報を、文法認識スレッド側のTextInfoへコピーする（文法認識スレッド実行前処理）
（文法認識スレッド側のTextInfoオブジェクトでコールする）
*/
void	ALineInfo::CopyLineInfoToSyntaxRecognizer( const ALineInfo& inSrcLineInfo, const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	//Dst側全データを削除
	DeleteAllAndCancelPurge();
	//Dst側メモリ割り当てステップ設定
	SetReallocateStep(inEndLineIndex-inStartLineIndex);
	//Dst側に行一括挿入
	InsertLines(0,inEndLineIndex-inStartLineIndex);
	
	//Dst側の行index
	AIndex	dstLineIndex = 0;
	
	//Src側の開始行のLineStartを取得（dst側の開始行のLineStartが0になるよう、LineStartをずらすために使う）
	AIndex	srcFirstLineStart = inSrcLineInfo.GetLineInfo_Start(inStartLineIndex);
	//行毎ループ
	for( AIndex srcLineIndex = inStartLineIndex; srcLineIndex < inEndLineIndex; srcLineIndex++ )
	{
		//Dst側
		AUniqID	dstUniqID = mLineInfoIDArray.Get(dstLineIndex);
		AIndex	dstStorageIndex = mLineInfoStorage_UniqID.Find(dstUniqID);
		
		//Src側
		AUniqID	srcUniqID = inSrcLineInfo.mLineInfoIDArray.Get(srcLineIndex);
		AIndex	srcStorageIndex = inSrcLineInfo.mLineInfoStorage_UniqID.Find(srcUniqID);
		
		//データをコピー
		mLineInfoStorage_Start.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorage_Start.Get(srcStorageIndex) - srcFirstLineStart );//dst側の開始行のLineStartが0になるよう、LineStartをずらす。
		mLineInfoStorage_Length.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorage_Length.Get(srcStorageIndex));
		mLineInfoStorage_ExistLineEnd.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorage_ExistLineEnd.Get(srcStorageIndex));
		mLineInfoStorage_ParagraphIndex.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorage_ParagraphIndex.Get(srcStorageIndex));
		mLineInfoStorage_RecognizeFlags.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorage_RecognizeFlags.Get(srcStorageIndex));
		mLineInfoStorageP_StateIndex.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_StateIndex.Get(srcStorageIndex));
		mLineInfoStorageP_PushedStateIndex.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_PushedStateIndex.Get(srcStorageIndex));
		mLineInfoStorageP_LineColor.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_LineColor.Get(srcStorageIndex));
		mLineInfoStorageP_IndentCount.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_IndentCount.Get(srcStorageIndex));
		/*drop mLineInfoStorageP_Multiply.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_Multiply.Get(srcStorageIndex));*/
		mLineInfoStorageP_DirectiveLevel.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_DirectiveLevel.Get(srcStorageIndex));
		mLineInfoStorageP_DisabledDirectiveLevel.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_DisabledDirectiveLevel.Get(srcStorageIndex));
		mLineInfoStorageP_FoldingLevel.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_FoldingLevel.Get(srcStorageIndex));
		/*drop mLineInfoStorageP_CheckedDate.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_CheckedDate.Get(srcStorageIndex));//#842*/
		//Dst側行indexを次の行へ
		dstLineIndex++;
	}
}

/**
文法認識スレッドの認識結果を、メインスレッドのTextInfoへコピーする
（メインスレッド側のオブジェクトでコールする）
*/
void	ALineInfo::CopyFromSyntaxRecognizerResult( const AIndex inDstStartLineIndex, const AIndex inDstEndLineIndex,
		const ALineInfo& inSrcLineInfo )
{
	//Src側の行index
	AIndex	srcLineIndex = 0;
	//Dst側の行indexでループ
	for( AIndex dstLineIndex = inDstStartLineIndex; dstLineIndex < inDstEndLineIndex; dstLineIndex++ )
	{
		//Dst側
		AUniqID	dstUniqID = mLineInfoIDArray.Get(dstLineIndex);
		AIndex	dstStorageIndex = mLineInfoStorage_UniqID.Find(dstUniqID);
		
		//Src側
		AUniqID	srcUniqID = inSrcLineInfo.mLineInfoIDArray.Get(srcLineIndex);
		AIndex	srcStorageIndex = inSrcLineInfo.mLineInfoStorage_UniqID.Find(srcUniqID);
		
		//認識結果をコピー（識別子はATextInfo::CopyFromSyntaxRecognizerResult()でコピーされる）
		mLineInfoStorage_RecognizeFlags.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorage_RecognizeFlags.Get(srcStorageIndex));
		mLineInfoStorageP_StateIndex.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_StateIndex.Get(srcStorageIndex));
		mLineInfoStorageP_PushedStateIndex.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_PushedStateIndex.Get(srcStorageIndex));
		mLineInfoStorageP_LineColor.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_LineColor.Get(srcStorageIndex));
		mLineInfoStorageP_IndentCount.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_IndentCount.Get(srcStorageIndex));
		/*drop mLineInfoStorageP_Multiply.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_Multiply.Get(srcStorageIndex));*/
		mLineInfoStorageP_DirectiveLevel.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_DirectiveLevel.Get(srcStorageIndex));
		mLineInfoStorageP_DisabledDirectiveLevel.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_DisabledDirectiveLevel.Get(srcStorageIndex));
		mLineInfoStorageP_FoldingLevel.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_FoldingLevel.Get(srcStorageIndex));
		/*drop mLineInfoStorageP_CheckedDate.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorageP_CheckedDate.Get(srcStorageIndex));//#842*/
		
		//Src側行indexを次の行へ
		srcLineIndex++;
	}
}

/**
デバッグ用行データ整合性チェック
*/
void	ALineInfo::CheckLineInfoDataForDebug()
{
	fprintf(stderr,"====================================\nCheckLineInfoDataForDebug\nLine count:%ld\n",GetLineCount());
	fprintf(stderr,"Paragraph count:%ld\n",GetLineInfo_ParagraphIndex(GetLineCount()-1)+1);
	fprintf(stderr,"Line storage count:%ld\n",mLineInfoStorage_Start.GetItemCount());
	//
	AItemCount	unrecognizedLineCount = 0;
	AIndex	paragraphIndex = 0;
	AIndex	textIndex = 0;
	for( AIndex lineIndex = 0; lineIndex < GetLineCount(); lineIndex++ )
	{
		//LineStart, lengthチェック、段落チェック
		AIndex	lineStart = GetLineInfo_Start(lineIndex);
		AItemCount	len = GetLineInfo_Length(lineIndex);
		if( textIndex != lineStart )
		{
			fprintf(stderr,"Line start error!!!:%ld\n",lineIndex);
			_ACError("",NULL);
		}
		if( len == 0 && lineIndex != GetLineCount()-1 )
		{
			fprintf(stderr,"Len==0 for non-last line!!!:%ld\n",lineIndex);
			_ACError("",NULL);
		}
		if( paragraphIndex != GetLineInfo_ParagraphIndex(lineIndex) )
		{
			fprintf(stderr,"Paragraph index error!!!:%ld\n",lineIndex);
			_ACError("",NULL);
		}
		//
		textIndex += len;
		if( GetLineInfo_ExistLineEnd(lineIndex) == true )
		{
			paragraphIndex++;
		}
		if( GetLineInfoP_Recognized(lineIndex) == false )
		{
			unrecognizedLineCount++;
		}
	}
	//
	fprintf(stderr,"Unrecognized line count:%ld\n",unrecognizedLineCount);
	fprintf(stderr,"====================================");
}


#pragma mark ===========================
#pragma mark [クラス]ATextInfo
#pragma mark ===========================
//テキストの行情報、識別子情報を作成、格納、管理するクラス
//ADocument_Text、および、AMultiFileFinderの検索メソッドに所有される
//（ADocument_Textが存在していなくても動作可能）

#pragma mark ---コンストラクタ／デストラクタ

//コンストラクタ
ATextInfo::ATextInfo( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent), 
		mGlobalIdentifierList(this), mLocalIdentifierList(this), mModeIndex(kStandardModeIndex)
		,mSystemHeaderMode(false)//#467
,mRegExpObjectPurged(false)//#693
,mRecognizeContext_CurrentTokenStackPosition(0)//#698
,mRecognizeContext_EndPossibleLineIndex(kIndex_Invalid)//#698
,mRecognizeContext_RecognizeStartLineIndex(kIndex_Invalid)//#698
,mRecognizeContext_CurrentStateIndex(kIndex_Invalid)//#698
,mRecognizeContext_PushedStateIndex(kIndex_Invalid)//#698
,mRecognizeContext_CurrentIndent(0)//#698
,mRecognizeContext_CurrentDirectiveLevel(0)//#698
,mRecognizeContext_NextDirectiveLevel(0)//#698
,mRecognizeContext_CurrentDisabledDirectiveLevel(kIndex_Invalid)//#698
,mRecognizeContext_NextDisabledDirectiveLevel(kIndex_Invalid)//#698
{
}
//デストラクタ
ATextInfo::~ATextInfo()
{
	//全データ削除 #890
	//別スレッドからアクセス中にいきなりオブジェクト削除しないようにするため。
	DeleteLineInfoAll();
}

//#693
/**
ImportFileData用として解析後に不要なデータを削除する
*/
void	ATextInfo::PurgeForImportFileData()
{
	PurgeRegExpObject();
	PurgeLineInfo();
}

//#693
/**
RegExpオブジェクトをpurgeする
AImportFileRecognizer::NVIDO_ThreadMain()でバックグラウンドで認識するとき、認識が終わったらメモリ節約のためRegExp内データを削除する
*/
void	ATextInfo::PurgeRegExpObject()
{
	mJumpSetupRegExp.DeleteAll();
	mIndentRegExp.DeleteAll();
	mSyntaxDefinitionRegExp.DeleteAll();
	mSyntaxDefinitionRegExp_State.DeleteAll();
	mSyntaxDefinitionRegExp_Index.DeleteAll();
	mKeywordRegExp.DeleteAll();
	mKeywordRegExp_CategoryIndex.DeleteAll();
	
	//Purgeフラグ設定（再度RecognizeSyntax()がコールされたら、そこから再度UpdateRegExp()がコールされる）
	mRegExpObjectPurged = true;
}

//#693
/**
LineInfoのうちImportFileData用としては不要なものを削除する（メモリ節約のため）
（インポートファイル用なのでグローバル識別子等は残す）
*/
void	ATextInfo::PurgeLineInfo()
{
	//LineInfoのうちImportFileData用としては不要なものを削除する
	//（インポートファイル用なのでグローバル識別子等は残す）
	mLineInfo.Purge();
	//
	{
		//#717
		//ローカル識別子排他制御
		AStMutexLocker	locker(mLocalIdentifierListMutex);
		//ローカル識別子全削除
		mLocalIdentifierList.DeleteAllIdentifiers();
	}
	//block start dataリスト全削除
	mBlockStartDataList.DeleteAll();

}

//#896
/**
全データ削除
*/
void	ATextInfo::DeleteAllInfo()
{
	//行情報全削除
	DeleteLineInfoAll();
	//正規表現オブジェクトデータ全削除
	PurgeRegExpObject();
	//ワードリストデータ全削除
	DeleteAllWordsList();
}

#pragma mark ===========================

#pragma mark ---

void	ATextInfo::SetMode( const AModeIndex inModeIndex, const ABool inLoadRegExp )
{
	mModeIndex = inModeIndex;
	
	//RegExpオブジェクトはpurgeする。必要時にロードする。
	//#693 UpdateRegExp();//#683
	PurgeRegExpObject();//#693
	//inLoadRegExpがtrueなら、ここでreg expオブジェクトをロード（falseの場合は、必要な時にロード）
	if( inLoadRegExp == true )
	{
		UpdateRegExp();
	}
}

//#683
/**
TextInfo内で持っている正規表現オブジェクト更新
*/
void	ATextInfo::UpdateRegExp() const
{
	GetApp().SPI_GetModePrefDB(mModeIndex).SetJumpSetupRegExp(mJumpSetupRegExp);
	GetApp().SPI_GetModePrefDB(mModeIndex).SetIndentRegExp(mIndentRegExp);
	GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().
			GetRegExpArray(mSyntaxDefinitionRegExp,mSyntaxDefinitionRegExp_State,mSyntaxDefinitionRegExp_Index);
	GetApp().SPI_GetModePrefDB(mModeIndex).GetKeywordRegExp(mKeywordRegExp,mKeywordRegExp_CategoryIndex);
	//#693 Purgeフラグoff
	mRegExpObjectPurged = false;
}

ABool	ATextInfo::CalcIndentByIndentRegExp( const AText& inText, const ATextIndex inStartTextIndex, const ATextIndex inEndTextIndex,
		AItemCount& ioCurrent, AItemCount& ioNext )//#441
{
	ABool	regExpFound = false;//#441
	//B0324
	AIndex	offset = 0;
	for( AIndex pos = inStartTextIndex; pos < inEndTextIndex; pos++, offset++ )
	{
		AUChar	ch = inText.Get(pos);
		if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
		else break;
	}
	
	//B0381
	/*B0381 AItemCount	currentTotal = 0;
	AItemCount	nextTotal = 0;*/
	AArray<AItemCount>	currentArray;
	AArray<AItemCount>	nextArray;
	AArray<AIndex>	eposArray;
	
	AItemCount	indentRegExpItemCount = mIndentRegExp.GetItemCount();
	for( AIndex index = 0; index < indentRegExpItemCount; index++ )
	{
		ARegExp&	regexp = mIndentRegExp.GetObject(index);
		regexp.InitGroup();
		ATextIndex	pos = inStartTextIndex+offset;//B0324
		if( regexp.Match(inText,pos,inEndTextIndex) == true )
		{
			AItemCount	current = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetCurrentLine,index);
			AItemCount	next = GetApp().SPI_GetModePrefDB(mModeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetNextLine,index);
			//B0381 正規表現一致終了位置が先であるものから順に並べてarrayに格納
			if( current != 0 || next != 0 )
			{
				AIndex	insertIndex = 0;
				for( ; insertIndex < eposArray.GetItemCount(); insertIndex++ )
				{
					if( pos < eposArray.Get(insertIndex) )
					{
						break;
					}
				}
				eposArray.Insert1(insertIndex,pos);
				currentArray.Insert1(insertIndex,current);
				nextArray.Insert1(insertIndex,next);
			}
			//#441
			regExpFound = true;
		}
	}
	
	//B0381 現在行インデントと次行インデントの和が0となる場合（<p>で次行+4, </p>で現在行-4の場合で<p></p>を入力した場合など）は現在行も次行も0とする
	/*B0381 if( currentTotal+nextTotal == 0 )
	{
		currentTotal = 0;
		nextTotal = 0;
	}
	ioCurrent += currentTotal;
	ioNext += nextTotal;*/
	//B0381 前のnextは後のcurrentと相殺する
	for( AIndex i = 0; i < nextArray.GetItemCount(); i++ )
	{
		AItemCount	mae_next = nextArray.Get(i);
		if( mae_next == 0 )   continue;
		for( AIndex j = i+1; j < currentArray.GetItemCount(); j++ )
		{
			AItemCount	ato_current = currentArray.Get(j);
			if( mae_next*ato_current < 0 )//先のnextと後のcurrentの+, -が逆である場合
			{
				//先のnextの絶対値の範囲内で相殺
				if( abs(ato_current) >= abs(mae_next) )
				{
					ato_current += mae_next;
					mae_next = 0;
				}
				else
				{
					mae_next += ato_current;
					ato_current = 0;
				}
				nextArray.Set(i,mae_next);
				currentArray.Set(j,ato_current);
			}
		}
	}
	for( AIndex i = 0; i < nextArray.GetItemCount(); i++ )
	{
		ioCurrent += currentArray.Get(i);
		ioNext += nextArray.Get(i);
	}
	return regExpFound;//#441
}

#pragma mark ===========================

#pragma mark ---行情報配列設定／取得

//行情報挿入
void	ATextInfo::InsertLineInfo( const AIndex inLineIndex )
{
	/*#695
	mLineInfo_Start.Insert1(inLineIndex,0);
	mLineInfo_Length.Insert1(inLineIndex,0);
	mLineInfo_ExistLineEnd.Insert1(inLineIndex,0);
	mLineInfoP_Recognized.Insert1(inLineIndex,false);
	mLineInfoP_StateIndex.Insert1(inLineIndex,0);
	mLineInfoP_PushedStateIndex.Insert1(inLineIndex,0);
	mLineInfo_FirstGlobalIdentifierUniqID.Insert1(inLineIndex,kUniqID_Invalid);
	mLineInfoP_FirstLocalIdentifierUniqID.Insert1(inLineIndex,kUniqID_Invalid);
	mLineInfoP_ColorizeLine.Insert1(inLineIndex,false);
	mLineInfoP_LineColor.Insert1(inLineIndex,kColor_Black);
	mLineInfoP_IndentCount.Insert1(inLineIndex,0);
	mLineInfoP_BlockStartDataObjectID.Insert1(inLineIndex,kObjectID_Invalid);
	mLineInfo_ParagraphIndex.Insert1(inLineIndex,0);//R0208
	mLineInfoP_Multiply.Insert1(inLineIndex,100);//#493
	mLineInfoP_DirectiveLevel.Insert1(inLineIndex,0);//#467
	mLineInfoP_DisabledDirectiveLevel.Insert1(inLineIndex,kIndex_Invalid);//#467
	mLineInfoP_RegExpGroupColor.Insert1(inLineIndex,kColor_Black);//#603
	mLineInfoP_RegExpGroupColor_StartIndex.Insert1(inLineIndex,kIndex_Invalid);//#603
	mLineInfoP_RegExpGroupColor_EndIndex.Insert1(inLineIndex,kIndex_Invalid);//#603
	mLineInfoP_FoldingLevel.Insert1(inLineIndex,kFoldingLevel_None);//#695
	*/
	//#695
	mLineInfo.Insert1Line(inLineIndex);
}

//行情報追加
void	ATextInfo::AddLineInfo()
{
	//#695 InsertLineInfo(mLineInfo_Start.GetItemCount());
	mLineInfo.Add1Line();
}

//#699
/**
行追加し、LineStart,Length, Paragraphを設定（Paragraphは自動計算して設定）
*/
void	ATextInfo::InsertLineInfo( const AIndex inLineIndex, const AIndex inStart, const AItemCount inLength, const ABool inExistLineEnd )
{
	//行追加
	mLineInfo.Insert1Line(inLineIndex);
	//LineStart, Lengthを設定
	mLineInfo.SetLineInfo_Start(inLineIndex,inStart);
	mLineInfo.SetLineInfo_Length(inLineIndex,inLength);
	mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,inExistLineEnd);
	//段落計算、設定
	AIndex	paragraphIndex = 0;
	if( inLineIndex > 0 )
	{
		paragraphIndex = mLineInfo.GetLineInfo_ParagraphIndex(inLineIndex-1);
		if( mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex-1) == true )
		{
			paragraphIndex++;
		}
	}
	mLineInfo.SetLineInfo_ParagraphIndex(inLineIndex,paragraphIndex);
}

//#699
/**
行追加し、LineStart,Length, Paragraphを設定（Paragraphは自動計算して設定）
*/
void	ATextInfo::AddLineInfo( const AIndex inStart, const AItemCount inLength, const ABool inExistLineEnd )
{
	InsertLineInfo(GetLineCount(),inStart,inLength,inExistLineEnd);
}

/**
行削除
*/
void	ATextInfo::DeleteLineInfos( const AIndex inLineIndex, const AItemCount inDeleteCount,
		AArray<AUniqID>& outDeletedIdentifiers, ABool& outImportFileIdentifierDeleted )
{
	//識別子削除（後ろの行から削除）
	for( AIndex lineIndex = inLineIndex + inDeleteCount - 1; lineIndex >= inLineIndex ; lineIndex-- )
	{
		DeleteLineIdentifiers(lineIndex,outDeletedIdentifiers,outImportFileIdentifierDeleted);
	}
	//行データ削除
	mLineInfo.DeleteLines(inLineIndex,inDeleteCount);
}

//行数取得
AItemCount	ATextInfo::GetLineCount() const
{
	//#695 return mLineInfo_Start.GetItemCount();
	return mLineInfo.GetLineCount();
}

//LineEndコードを含めない行レングス取得
AItemCount	ATextInfo::GetLineLengthWithoutLineEnd( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfo_Length(inLineIndex) - (mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex)?1:0);
}

//LineEndコードを含めた行レングス取得
AItemCount	ATextInfo::GetLineLengthWithLineEnd( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfo_Length(inLineIndex);
}

//行頭のTextIndexを取得
ATextIndex	ATextInfo::GetLineStart( const AIndex inLineIndex ) const 
{
	return mLineInfo.GetLineInfo_Start(inLineIndex);
}

//
AItemCount	ATextInfo::GetTextLength() const
{
	return mLineInfo.GetLineInfo_Start(GetLineCount()-1) + mLineInfo.GetLineInfo_Length(GetLineCount()-1);
}

//行にLineEndコードがあるかどうかを返す
ABool	ATextInfo::GetLineExistLineEnd( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex);
}

//
void	ATextInfoForDocument::GetLineStateIndex( const AIndex inLineIndex, AIndex& outStateIndex, AIndex& outPushedStateIndex ) const
{
	outStateIndex = mLineInfo.GetLineInfoP_StateIndex(inLineIndex);
	outPushedStateIndex = mLineInfo.GetLineInfoP_PushedStateIndex(inLineIndex);
}

//#467
/**
指定行のDirective Levelを取得
*/
AIndex	ATextInfoForDocument::GetDirectiveLevel( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_DirectiveLevel(inLineIndex);
}

//#467
/**
指定行がDirectiveによりdisableになったときのlevelを取得（disableでなければkIndex_Invalid）
*/
AIndex	ATextInfoForDocument::GetDisabledDirectiveLevel( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_DisabledDirectiveLevel(inLineIndex);
}

ABool	ATextInfoForDocument::GetLineRecognized( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_Recognized(inLineIndex);
}

/*
void	ATextInfoForDocument::SetLineRecognized( const AIndex inLineIndex, const ABool inRecognized )
{
	mLineInfo.SetLineInfoP_Recognized(inLineIndex,inRecognized);
}
*/

ABool	ATextInfoForDocument::GetLineColor( const AIndex inLineIndex, AColor& outColor ) const
{
	if( mLineInfo.GetLineInfoP_ColorizeLine(inLineIndex) == true )
	{
		outColor = mLineInfo.GetLineInfoP_LineColor(inLineIndex);
		return true;
	}
	else
	{
		return false;
	}
}

AItemCount	ATextInfoForDocument::GetIndentCount( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_IndentCount(inLineIndex);
}

//#603
/**
見出し正規表現グループ色づけデータ取得
*/
void	ATextInfoForDocument::GetLineRegExpGroupColor( const AIndex inLineIndex,
			AColor& outColor, AIndex& outStartIndex, AIndex& outEndIndex ) const
{
	//★#695
	outColor = kColor_Black;//#695 mLineInfoP_RegExpGroupColor.Get(inLineIndex);
	outStartIndex = kIndex_Invalid;//#695 mLineInfoP_RegExpGroupColor_StartIndex.Get(inLineIndex);
	outEndIndex = kIndex_Invalid;//#695 mLineInfoP_RegExpGroupColor_EndIndex.Get(inLineIndex);
}

//#695
/**
stateがstableな行かどうかを判定
*/
ABool	ATextInfoForDocument::IsStableStateLine( const AIndex inLineIndex ) const
{
	AIndex	stateIndex = mLineInfo.GetLineInfoP_StateIndex(inLineIndex);
	if( stateIndex == 0 )
	{
		return true;
	}
	else
	{
		return GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().
				GetSyntaxDefinitionState(stateIndex).IsStable();
	}
}

//#695
/**
stateがstableな行を、inLineIndexの前の行からさかのぼって検索
*/
AIndex	ATextInfoForDocument::GetPrevStableStateLineIndex( const AIndex inLineIndex ) const
{
	for( AIndex lineIndex = inLineIndex-1; lineIndex > 0; lineIndex-- )
	{
		if( IsStableStateLine(lineIndex) == true )
		{
			return lineIndex;
		}
	}
	return 0;
}

/**
SyntaxWarning取得
*/
ABool	ATextInfoForDocument::GetSyntaxWarning( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_SyntaxWarningExist(inLineIndex);
}

/**
SyntaxError取得
*/
ABool	ATextInfoForDocument::GetSyntaxError( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_SyntaxErrorExist(inLineIndex);
}

//#842
/**
行チェック日時設定
*/
void	ATextInfoForDocument::SetLineCheckedDate( const AIndex inLineIndex, const ANumber inDate )
{
	mLineInfo.SetLineInfoP_CheckedDate(inLineIndex,inDate);
}

//#842
/**
行チェック日時取得
*/
ANumber	ATextInfoForDocument::GetLineCheckedDate( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_CheckedDate(inLineIndex);
}

//#142
/**
合計行数取得
*/
void	ATextInfoForDocument::GetTotalWordCount( AItemCount& outTotalCharCount, AItemCount& outTotalWordCount ) const
{
	mLineInfo.GetTotalWordCount(outTotalCharCount,outTotalWordCount);
}

#pragma mark ===========================

#pragma mark ---TextPoint/TextIndex変換

//TextPointからIndex取得
AIndex	ATextInfo::GetTextIndexFromTextPoint( const ATextPoint& inPoint ) const
{
	return GetLineStart(inPoint.y)+inPoint.x;
}

//IndexからTextPoint取得
void	ATextInfo::GetTextPointFromTextIndex( const AIndex inTextPosition, ATextPoint& outPoint, const ABool inPreferNextLine ) const 
{
	//inIndexが不正の場合の処理
	if( inTextPosition > GetTextLength() || inTextPosition < 0 ) 
	{
		_ACError("index invalid",this);
		outPoint.y = GetLineCount()-1;
		outPoint.x = GetTextLength() - GetLineStart(GetLineCount()-1);
		return;
	}
	//2分法
	AIndex	startLineIndex = 0;
	AIndex	endLineIndex = GetLineCount();
	AIndex	lineIndex = (startLineIndex+endLineIndex)/2;
	AItemCount	lineCount = GetLineCount();
	for( AIndex i = 0; i < lineCount*2; i++ )//無限ループ防止（多くとも行数回のループで終わるはず）
	{
		AIndex	lineStart = GetLineStart(lineIndex);
		AIndex	lineLength = GetLineLengthWithoutLineEnd(lineIndex);
		if( inTextPosition >= lineStart && inTextPosition <= lineStart+lineLength ) 
		{
			outPoint.y = lineIndex;
			outPoint.x = inTextPosition-lineStart;
			if( inPreferNextLine == true && lineIndex+1 < GetLineCount() )
			{
				if( outPoint.x == GetLineLengthWithLineEnd(lineIndex) )
				{
					outPoint.x = 0;
					outPoint.y++;
				}
			}
			return;
		}
		if( inTextPosition < lineStart )
		{
			endLineIndex = lineIndex;
			lineIndex = (startLineIndex+endLineIndex)/2;
		}
		else if( inTextPosition > lineStart+lineLength )
		{
			startLineIndex = lineIndex+1;
			lineIndex = (startLineIndex+endLineIndex)/2;
		}
		if( startLineIndex+1 >= endLineIndex )
		{
			endLineIndex = startLineIndex+1;
			lineIndex = startLineIndex;
		}
	}
	_ACError("cannot find text index",this);
}

#pragma mark ===========================

#pragma mark ---描画用テキスト取得

//テキスト描画用データ取得
//UTF16化、タブのスペース化、制御文字の可視化等を行う。
//CTextDrawDataクラスのうち、下記を設定する。
//UTF16DrawText: UTF16での描画用テキスト
//UnicodeDrawTextIndexArray: インデックス：ドキュメントが保持している元のテキストのインデックス　値：描画テキストのUnicode文字単位でのインデックス
//inToLineEndCode: falseの場合は指定行のみ、trueの場合は行情報を無視して改行コードまで
void	ATextInfo::GetTextDrawDataWithoutStyle( const AText& inText, const ANumber inTabWidth, const AItemCount inIndentWidth, //#117
											   const AItemCount inLetterCountLimit, 
											   const AIndex inLineIndex, CTextDrawData& outTextDrawData, 
											   const ABool inGetUTF16Text, const ABool inCountAs2Letters,
											   const AIndex inToLineEndCode, const ABool inForDraw,
											   const ABool inDisplayYenFor5C ) const //B0000 行折り返し計算高速化 #695 #940
{
	//行情報取得
	AIndex	lineInfo_Start = GetLineStart(inLineIndex);
	//B0000 行折り返し計算高速化
	AItemCount	lineInfo_LengthWithoutCR = inText.GetItemCount()-lineInfo_Start;
	if( inToLineEndCode == false )
	{
		lineInfo_LengthWithoutCR = GetLineLengthWithoutLineEnd(inLineIndex);
	}
	//取得する文字数（バイト数）をkLimit_Max1LineDrawByteCountsに制限する（描画時のみ） #695
	//（kLimit_Max1LineDrawByteCounts以上となる最初の文字までで取得打ちきりとなる）
	if( inForDraw == true )
	{
		if( lineInfo_LengthWithoutCR > kLimit_Max1LineDrawByteCounts )
		{
			lineInfo_LengthWithoutCR = kLimit_Max1LineDrawByteCounts;
		}
	}
	//対象行のヒントテキストを取得
	ATextArray	hintTextArray;
	AArray<AIndex>	hintTextPosArray;
	{
		AItemCount	itemCount = mHintTextArray_TextIndex.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			ATextIndex	textIndex = mHintTextArray_TextIndex.Get(i);
			if( textIndex >= lineInfo_Start && textIndex < lineInfo_Start+lineInfo_LengthWithoutCR )
			{
				hintTextArray.Add(mHintTextArray_Text.GetTextConst(i));
				hintTextPosArray.Add(mHintTextArray_TextIndex.Get(i)-lineInfo_Start);
			}
		}
	}
	//作成
	outTextDrawData.MakeTextDrawDataWithoutStyle(inText,inTabWidth,inLetterCountLimit,inGetUTF16Text,inCountAs2Letters,
												 GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayControlCode),
												 lineInfo_Start,lineInfo_LengthWithoutCR,
												 hintTextPosArray,hintTextArray,
												 GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kStartSpaceToIndent),
												 inIndentWidth,inTabWidth,//#117
												 inDisplayYenFor5C);//#940
}

#pragma mark ===========================

#pragma mark ---行情報計算

/**
行全削除
*/
void	ATextInfo::DeleteLineInfoAll(/*#695 AArray<AUniqID>& outDeletedIdentifiers */)
{
	/*#695 高速化。全削除の場合、削除したidentifierのuniqIDを記憶する必要はない。（全削除のため）
	//行情報を全て消去
	ABool	importChanged = false;
	while( GetLineCount() > 0 )
	{
		DeleteLineInfo(GetLineCount()-1,outDeletedIdentifiers,importChanged);
	}
	//識別子情報の削除
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		DeleteLineIdentifiers(lineIndex,outDeletedIdentifiers,importChanged);
	}
	*/
	//識別子全削除
	{
		//#717
		//グローバル識別子リスト排他制御
		AStMutexLocker	locker(mGlobalIdentifierListMutex);
		//グローバル識別子リスト全削除
		mGlobalIdentifierList.DeleteAllIdentifiers();
	}
	{
		//#717
		//ローカル識別子リスト排他制御
		AStMutexLocker	locker(mLocalIdentifierListMutex);
		//ローカル識別子リスト全削除
		mLocalIdentifierList.DeleteAllIdentifiers();
	}
	mBlockStartDataList.DeleteAll();
	//行情報削除し、Purge状態解除
	mLineInfo.DeleteAllAndCancelPurge();
}

/**
行情報を計算する（旧情報は全て削除し、新規に生成）
*/
AItemCount	ATextInfo::CalcLineInfoAll( const AText& inText, 
									   const ABool inAddEmptyEOFLineIfNeeded, const AItemCount inLimitLineCount,//#699
									   const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
									   const AItemCount inTabWidth, const AItemCount inIndentWidth, 
									   const AWrapMode inWrapMode, const AItemCount inWrapLetterCount, //#117
									   const ANumber inViewWidth, const ABool inCountAs2Letters )
{
	//行情報全削除
	DeleteLineInfoAll(/*#695 outDeletedIdentifiers*/);
	
	//メモリ割り当てステップ設定
	if( inLimitLineCount == kIndex_Invalid )
	{
		//#695 テキスト内の行数分をLineInfoのメモリ割り当て増加ステップに設定
		//（ただしALineInfoにてkLineInfoAllocationStepより小さくは設定されない）
		AItemCount	lineEndCharCount = inText.GetCountOfChar(kUChar_LineEnd,0,inText.GetItemCount());
		mLineInfo.SetReallocateStep(lineEndCharCount);
	}
	else
	{
		//#699 制限値有りの場合はそれをステップに設定
		mLineInfo.SetReallocateStep(inLimitLineCount);
	}
	
	//==================各行行計算==================
	
	//B0000 行折り返し計算高速化
	CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	//#1034 textDrawData.mReuseATSUStyle = true;//#695
	
	AIndex	textDrawDataStartOffset = kIndex_Invalid;
	
	//メインスレッドでの実行ならモーダルセッション開始
	ABool	showModalSession = (ABaseFunction::InMainThread()==true);
	AStEditProgressModalSession	modalSession(kEditProgressType_CalcLineInfo,true,false,showModalSession);
	
	//行毎に行情報を追加、計算していく
	for( AIndex lineIndex = 0; ; lineIndex++ )
	{
		//モーダルセッション継続判定
		if( showModalSession == true )
		{
			AIndex	pos = 0;
			if( lineIndex > 0 )
			{
				pos = GetLineStart(lineIndex-1) + GetLineLengthWithLineEnd(lineIndex-1);
			}
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_CalcLineInfo,0,true,pos,inText.GetItemCount()) == false )
			{
				//キャンセル時
				break;
			}
		}
		
		//行追加
		AddLineInfo();
		//行計算
		CalcLineInfo(inText,lineIndex,inFontName,inFontSize,inAntiAliasing,inTabWidth,inIndentWidth,inWrapMode,inWrapLetterCount,inViewWidth,inCountAs2Letters,
					textDrawData,textDrawDataStartOffset);//B0000 行折り返し計算高速化
		//テキスト全ての行計算が完了したらbreak
		if( GetLineStart(lineIndex) + GetLineLengthWithLineEnd(lineIndex) >= inText.GetItemCount() )   break;
		//行数制限がある場合（最初の200行や、行計算スレッドからの実行）、行数判定#699
		//現在の行数が、制限値以上の場合は、この行までで終了
		//（この判定で終了する場合は、上の判定がfalseなので、テキストの途中。AddEmptyEOFLineIfNeeded()は不要。）
		if( inLimitLineCount != kIndex_Invalid )
		{
			if( lineIndex+1 >= inLimitLineCount )
			{
				//テキストの計算済み終了位置を返す
				return GetLineStart(lineIndex) + GetLineLengthWithLineEnd(lineIndex);
			}
		}
	}
	/*#699
	//最終行がLineEndを含むならもう１行、行情報を追加する
	if( GetLineExistLineEnd(GetLineCount()-1) == true )
	{
		AddLineInfo();
		CalcLineInfo(inText,GetLineCount()-1,inFontName,inFontSize,inAntiAliasing,inTabWidth,inIndentWidth,inWrapMode,inWrapLetterCount,inViewWidth,inCountAs2Letters,
					textDrawData,textDrawDataStartOffset);//B0000 行折り返し計算高速化
	}
	*/
	//必要ならばEOF空行を追加 #699
	if( inAddEmptyEOFLineIfNeeded == true )
	{
		AddEmptyEOFLineIfNeeded();
	}
	//テキストの計算済み終了位置（＝テキスト長さ）を返す
	return inText.GetItemCount();
}

const ANumber kWordWrapWindowLeftMargin = 2;//B0375 キャレットが見えなくなる問題の対策

//指定行のLineInfoを計算、更新する
void	ATextInfo::CalcLineInfo( const AText& inText, const AIndex inLineIndex, 
		/*win const AFont inFont*/const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
		const AItemCount inTabWidth, const AItemCount inIndentWidth, const AWrapMode inWrapMode, const AItemCount inWrapLetterCount, const ANumber inViewWidth, //#117
		const ABool inCountAs2Letters, 
		CTextDrawData& ioTextDrawData, AIndex& ioTextDrawDataStartOffset )//B0000 行折り返し計算高速化
{
	//lineStartを前の行の情報から算出
	AIndex	lineStart = 0;
	if( inLineIndex > 0 )
	{
		lineStart = GetLineStart(inLineIndex-1) + GetLineLengthWithLineEnd(inLineIndex-1);
	}
	mLineInfo.SetLineInfo_Start(inLineIndex,lineStart);
	mLineInfo.SetLineInfo_Length(inLineIndex,inText.GetItemCount()-lineStart);
	mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,false);
	//R0208
	AIndex	para = 0;
	if( inLineIndex > 0 )
	{
		para = mLineInfo.GetLineInfo_ParagraphIndex(inLineIndex-1);
		if( mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex-1) == true )
		{
			para++;
		}
	}
	mLineInfo.SetLineInfo_ParagraphIndex(inLineIndex,para);
	
	//#693 SetLineRecognized(inLineIndex,false);
	mLineInfo.ClearRecognizeFlags(inLineIndex);//#693
	if( inText.GetItemCount()-lineStart == 0 )   return;
	
	// 
	AItemCount	textlength = inText.GetItemCount();
	switch(inWrapMode)
	{
	  case kWrapMode_NoWrap:
		{
			for( AIndex pos = lineStart; pos < textlength; pos++ )
			{
				if( inText.Get(pos) == kUChar_LineEnd )
				{
					mLineInfo.SetLineInfo_Length(inLineIndex,pos+1-lineStart);
					mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,true);
					break;
				}
			}
			break;
		}
	  case kWrapMode_WordWrap:
	  case kWrapMode_LetterWrap://#1113
		{
			if( ioTextDrawDataStartOffset == kIndex_Invalid )
			{
				//次の改行コードまでのTextDrawDataを取得
				//GetTextDrawDataWithoutStyle(inText,inTabWidth,0,inLineIndex,ioTextDrawData,true,inCountAs2Letters);
				//ioTextDrawDataStartOffset = 0;
				AIndex	parastartlineindex = GetCurrentParagraphStartLineIndex(inLineIndex);
				//AIndex	svLen = mLineInfo.GetLineInfo_Length(startlineindex);
				//mLineInfo.SetLineInfo_Length(startlineindex,inText.GetItemCount()-mLineInfo.GetLineInfo_Start(startlineindex));
				GetTextDrawDataWithoutStyle(inText,inTabWidth,inIndentWidth,0,parastartlineindex,ioTextDrawData,true,inCountAs2Letters,true,false,false);
				ioTextDrawDataStartOffset = ioTextDrawData.OriginalTextArray_UnicodeOffset.Get(
							mLineInfo.GetLineInfo_Start(inLineIndex) - mLineInfo.GetLineInfo_Start(parastartlineindex));
				//mLineInfo.SetLineInfo_Length(startlineindex,svLen);
			}
			//------------------Line break計算------------------
			AItemCount	endOffset = 0;
			if( ioTextDrawData.UTF16DrawText.GetItemCount() > 0 )
			{
				endOffset = CWindowImp::GetLineBreak(ioTextDrawData,ioTextDrawDataStartOffset,
													 inFontName,inFontSize,inAntiAliasing,inViewWidth-kWordWrapWindowLeftMargin,//B0375
													 (inWrapMode==kWrapMode_LetterWrap));//#1113
			}
			//元のUTF-8文字の位置に対応するoffsetにする（途中でbreakした場合、その文字の最初まで戻る）
			//たとえば、制御コード表示<sp>00<sp><sp>00<sp>で、<sp>00<sp><sp>までが一行と判定された場合でも、<sp>00<sp>までを折り返しとするため
			endOffset = ioTextDrawData.OriginalTextArray_UnicodeOffset.Get(ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(endOffset));
			//
			AItemCount	length = ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(endOffset) 
						- ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(ioTextDrawDataStartOffset);
			//改行コードがあれば、そこまでを一行とし、フラグを付ける
			if( lineStart + length < inText.GetItemCount() )
			{
				if( inText.Get(lineStart+length) == kUChar_LineEnd )
				{
					length++;
					mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,true);
				}
			}
			//B0000無限ループ防止（行折り返し0文字だった場合、1文字分）
			if( length <= 0 && mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex) == false )
			{
				//UTF16DrawTextArray_OriginalTextIndexをioTextDrawDataStartOffsetから始めて、最初に値が変わるところを折り返し地点とする
				//制御コード表示<sp>00<sp>などのとき、UTF16DrawTextArray_OriginalTextIndexは、0,0,0,0,1などとなっている。単純にendOffset++ではNG
				for( endOffset = ioTextDrawDataStartOffset; endOffset < ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.GetItemCount(); endOffset++ )
				{
					length = ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(endOffset) 
							- ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(ioTextDrawDataStartOffset);
					if( length > 0 )   break;
				}
			}
			//不具合発生時の無限ループ防止処理
			if( length == 0 )
			{
				_ACError("",NULL);
				//次の改行コードまでを一行とする
				for( AIndex pos = lineStart; pos < textlength; pos++ )
				{
					if( inText.Get(pos) == kUChar_LineEnd )
					{
						mLineInfo.SetLineInfo_Length(inLineIndex,pos+1-lineStart);
						mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,true);
						break;
					}
				}
				//
				ioTextDrawDataStartOffset = kIndex_Invalid;
				break;
			}
			//length設定
			mLineInfo.SetLineInfo_Length(inLineIndex,length);
			//ioTextDrawDataStartOffset更新
			if( endOffset >= ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) )
			{
				//TextDrawDataのデータをすべて読み終わったときはInvalidにする→次回、新たにTextDrawDataを取得する
				ioTextDrawDataStartOffset = kIndex_Invalid;
			}
			else
			{
				ioTextDrawDataStartOffset = endOffset;
			}
			break;
		}
	  case kWrapMode_WordWrapByLetterCount:
	  case kWrapMode_LetterWrapByLetterCount://#1113
		{
			CTextDrawData	textDrawData(false);
			GetTextDrawDataWithoutStyle(inText,inTabWidth,inIndentWidth,inWrapLetterCount,inLineIndex,textDrawData,false,inCountAs2Letters,false,false,false);
			AItemCount	length = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(textDrawData.UTF16DrawTextArray_OriginalTextIndex.GetItemCount()-1);
			if( lineStart + length < inText.GetItemCount() )
			{
				if( inText.Get(lineStart+length) == kUChar_LineEnd )
				{
					length++;
					mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,true);
				}
			}
			mLineInfo.SetLineInfo_Length(inLineIndex,length);
			//ワードラップ処理
			if( inWrapMode == kWrapMode_WordWrapByLetterCount )//#1113
			{
				AIndex	pos = lineStart+length;
				if( mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex) == false && pos < inText.GetItemCount() && pos > 0 )
				{
					AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
					//
					AStTextPtr	textptr(inText,0,inText.GetItemCount());
					//この時点でposは次の行の開始文字
					AUCS4Char	curChar = 0;
					AText::Convert1CharToUCS4(textptr.GetPtr(),textptr.GetByteCount(),pos,curChar);
					AIndex	prevPos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(inText,pos);
					AUCS4Char	prevChar = 0;
					AText::Convert1CharToUCS4(textptr.GetPtr(),textptr.GetByteCount(),prevPos,prevChar);
					//計算した行末が単語の途中になってしまったかどうかのチェック
					if( unicodeData.IsAlphabetOrNumber(prevChar) == true && 
								unicodeData.IsAlphabetOrNumber(curChar) == true )
					{
						//１文字ずつ前に戻って、アルファベット以外になる箇所を探す
						AIndex	newpos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(inText,pos);//その行の最後の文字位置から
						for( ; newpos > lineStart; newpos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(inText,newpos) )
						{
							AUCS4Char	ch = 0;
							AText::Convert1CharToUCS4(textptr.GetPtr(),textptr.GetByteCount(),newpos,ch);
							if( unicodeData.IsAlphabetOrNumber(ch) == false )
							{
								//アルファベット以外の位置でbreak
								break;
							}
						}
						if( newpos > lineStart )
						{
							//posをアルファベット以外の文字の直後に設定
							pos = unicodeData.GetNextCharPosSkipCanonicalDecomp(inText,newpos);
						}
						mLineInfo.SetLineInfo_Length(inLineIndex,pos-lineStart);
					}
				}
			}
			Kinsoku(inText,inLineIndex);
			break;
		}
	}
	//#142
	//文字数単語数計算
	CalcWordCount(inText,inLineIndex);
#if 0
	Int16	winWidth = localFrame.right-localFrame.left-5;
	
	OSStatus	status;
	
	if( /*R-abs !mLCWrap*/GetWrapMode() != kWrapMode_WordWrapByLetterCount )//ウインドウ幅ラップ
	{
		//最終行で文字が1文字も無い場合はレングス０に設定して終了
		if( lineStart >= mLength )
		{
			(*(mLineInfos->mLineInfosH))[inLineNumber].length = 0;
			return;
		}
		//次のCRまでの文字をUnicodeに変換してlineBufferに入れる
		Int32	paraEndPos;
		Boolean	isLastLine = true;
		for( paraEndPos = lineStart; paraEndPos < mLength; paraEndPos++ )
		{
			if( (*(mText->mTextH))[paraEndPos] == 13 )
			{
				paraEndPos++;
				isLastLine = false;
				break;
			}
		}
		Int32	lineLen = paraEndPos-lineStart;
		if( !isLastLine )   lineLen--;
		//R0108
		if( mWrapLineBufferStart == lineStart )
		{
			//前回計算したmWrapLineBufferをそのまま使う
		}
		else
		{
			mWrapLineBuffer.DeleteAll();
			mText->Lock();
			UText::UTF8ToUnicodeTab2Space(&((*(mText->mTextH))[lineStart]),lineLen,/*R-abs mTabWidth*/GetTabWidth(),mWrapLineBuffer,/*R-abs (mPixelTabWidth==0)*/true);//R0008
			mText->Unlock();
			mWrapLineBufferStart = lineStart;
		}
		
		//ATSUBreakLineによる計算
		ATSUTextLayout	lineLayout = CreateTextLayout(mWrapLineBuffer);
		UniCharArrayOffset	endUniPos;
		status = ::ATSUBreakLine(lineLayout,kATSUFromTextBeginning,Long2Fix(winWidth),false,&endUniPos);
		if( mWrapLineBuffer.mItemCount == endUniPos )
		{
			(*(mLineInfos->mLineInfosH))[inLineNumber].length = paraEndPos-lineStart;
			if( !isLastLine )
			{
				(*(mLineInfos->mLineInfosH))[inLineNumber].CR = true;
			}
			mWrapLineBufferStart = -1;//R0108
		}
		else
		{
			Int32	tabletters = 0;//B0154
			UniCharCount	uniPos = 0;
			Int32	pos;
			for( pos = lineStart; pos < paraEndPos; )
			{
				pos += UText::UTF8ToUnicodeTab2SpaceVirtual((*(mText->mTextH))[pos],/*R-abs mTabWidth*/GetTabWidth(),uniPos,tabletters,/*R-abs (mPixelTabWidth==0)*/true);//R0008
				if( uniPos >= endUniPos )   break;
			}
			(*(mLineInfos->mLineInfosH))[inLineNumber].length = pos-lineStart;
			
			//R0108
			mWrapLineBuffer.Delete(0,endUniPos);
			mWrapLineBufferStart = pos;
		}
		
		::ATSUDisposeTextLayout(lineLayout);
	}
	else//文字数Wrap
	{
		short	lcinc0800 = 1;//B0154
		if( gApp->GetAppPref().GetData_Bool(AAppPrefDB::kCountAs2Letters) == true/*R-abs globals->mAppPref->mPref.countAs2Letters*/ )   lcinc0800 = 2;//B0154
		Int32	tabletters = 0;//B0154
		UniCharCount	uniLen = 0;
		Int32	pos;
		Int32	lc = 0;
		/*B0278 if( CModeData::GetModeDataByID(GetModeIndex())->mPref.mimikakiWrap )
		{
			lc = lcinc0800;
		}*/
		UniCharCount	uniPos = 0;
		mText->Lock();
		for( pos = lineStart; pos < mLength;  )
		{
			unsigned char ch = (*(mText->mTextH))[pos];
			if( ch == 13 )
			{
				pos++;
				(*(mLineInfos->mLineInfosH))[inLineNumber].CR = true;
				break;
			}
			UniCharCount	newPos = uniPos;
			short posinc = UText::UTF8ToUnicodeTab2SpaceVirtual(ch,/*R-abs mTabWidth*/GetTabWidth(),newPos,tabletters,/*R-abs (mPixelTabWidth==0)*/true);//R0008
			if( /*IsMultibyte(ch)B0154*/ ch >= 0xE0 )
			{
				lc += lcinc0800;
			}
			else
			{
				lc += newPos-uniPos;
			}
			if( lc > /*R-abs mLCWrapCount*/GetWrapLetterCount() )
			{
				break;
			}
			pos += posinc;
			uniPos = newPos;
		}
		
		//B0214暫定 本当はUCFindTextBreakとかを使うべき
		if( (*(mLineInfos->mLineInfosH))[inLineNumber].CR == false && pos < mLength-1 && pos > 0 )
		{
			unsigned char ch1 = (*(mText->mTextH))[pos-1];
			unsigned char ch2 = (*(mText->mTextH))[pos];
			if( ((ch1>='A'&&ch1<='Z')||(ch1>='a'&&ch1<='z')) && ((ch2>='A'&&ch2<='Z')||(ch2>='a'&&ch2<='z')) )
			{
				Int32 newpos = pos-1;
				for( ; newpos > lineStart; newpos-- )
				{
					unsigned char ch = (*(mText->mTextH))[newpos];
					if( not ((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')) )
					{
						break;
					}
				}
				if( newpos > lineStart )
				{
					pos = newpos+1;
				}
			}
		}
		
		mText->Unlock();
		(*(mLineInfos->mLineInfosH))[inLineNumber].length = pos-lineStart;
		Kinsoku(inLineNumber);
	}
#endif
}

//win
/**
行情報を計算する（旧情報は全て削除し、新規に生成）（Viewなし・NoWrap only）
*/
void	ATextInfo::CalcLineInfoAllWithoutView( const AText& inText, const ABool& inAbort )
{
	//行情報全削除
	//#695 AArray<AUniqID>	deletedIdentifiers;
	DeleteLineInfoAll(/*#695 deletedIdentifiers*/);
	
	//#695 テキスト内の行数分をLineInfoのメモリ割り当て増加ステップに設定
	//（ただしALineInfoにてkLineInfoAllocationStepより小さくは設定されない）
	AItemCount	lineEndCharCount = inText.GetCountOfChar(kUChar_LineEnd,0,inText.GetItemCount());
	mLineInfo.SetReallocateStep(lineEndCharCount);
	
	//行毎に行情報を追加、計算していく
	for( AIndex lineIndex = 0; ; lineIndex++ )
	{
		//AbortフラグONなら終了 #699
		if( inAbort == true )
		{
			return;
		}
		//
		AddLineInfo();
		CalcLineInfoWithoutView(inText,lineIndex);
		if( GetLineStart(lineIndex) + GetLineLengthWithLineEnd(lineIndex) >= inText.GetItemCount() )   break;
	}
	/*#699
	//最終行がLineEndを含むならもう１行、行情報を追加する
	if( GetLineExistLineEnd(GetLineCount()-1) == true )
	{
		AddLineInfo();
		CalcLineInfoWithoutView(inText,GetLineCount()-1);
	}
	*/
	//必要ならばEOF空行を追加 #699
	AddEmptyEOFLineIfNeeded();
}

//win
/**
指定行のLineInfoを計算、更新する（Viewなし・NoWrap only）
*/
void	ATextInfo::CalcLineInfoWithoutView( const AText& inText, const AIndex inLineIndex )
{
	//lineStartを前の行の情報から算出
	AIndex	lineStart = 0;
	if( inLineIndex > 0 )
	{
		lineStart = GetLineStart(inLineIndex-1) + GetLineLengthWithLineEnd(inLineIndex-1);
	}
	mLineInfo.SetLineInfo_Start(inLineIndex,lineStart);
	mLineInfo.SetLineInfo_Length(inLineIndex,inText.GetItemCount()-lineStart);
	mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,false);
	//R0208
	AIndex	para = 0;
	if( inLineIndex > 0 )
	{
		para = mLineInfo.GetLineInfo_ParagraphIndex(inLineIndex-1);
		if( mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex-1) == true )
		{
			para++;
		}
	}
	mLineInfo.SetLineInfo_ParagraphIndex(inLineIndex,para);
	
	//#693 SetLineRecognized(inLineIndex,false);
	mLineInfo.ClearRecognizeFlags(inLineIndex);//#693
	if( inText.GetItemCount()-lineStart == 0 )   return;
	
	// 
	AItemCount	textlength = inText.GetItemCount();
	for( AIndex pos = lineStart; pos < textlength; pos++ )
	{
		if( inText.Get(pos) == kUChar_LineEnd )
		{
			mLineInfo.SetLineInfo_Length(inLineIndex,pos+1-lineStart);
			mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,true);
			break;
		}
	}
}

// 禁則文字を付け加えていく
void	ATextInfo::Kinsoku( const AText& inText, const AIndex inLineIndex )
{
	if( mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex) == true )   return;
	AIndex	pos = mLineInfo.GetLineInfo_Start(inLineIndex) + mLineInfo.GetLineInfo_Length(inLineIndex);
	if( pos >= inText.GetItemCount() )   return;
	AUChar	ch = inText.Get(pos);
	if( ch == kUChar_Period || ch == kUChar_Comma )
	{
		pos++;
	}
	//禁則文字付け加え／削除
	for( AIndex i = 0; i < 2; i++ )
	{
		if( (pos < inText.GetItemCount() && pos >= 6 ) == false )   break;
		AIndex	nextcharpos = inText.GetNextCharPos(pos);
		AText	text;
		inText.GetText(pos,nextcharpos-pos,text);
		if( GetApp().GetAppPref().IsKinsokuLetter(text) == true )
		{
			//#844 if( GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kKinsokuBuraasge) == true )
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kKinsokuBuraasge) == true )
			{
				pos = inText.GetNextCharPos(pos);
			}
			else
			{
				pos = inText.GetPrevCharPos(pos);
			}
		}
		else
		{
			break;
		}
	}
	// 半角、全角スペース、半角タブ付け加え
	while( pos < inText.GetItemCount() )
	{
		AUTF16Char	uc;
		AItemCount	bc = inText.Convert1CharToUTF16(pos,uc);
		if( uc == 0x0020 || uc == /*#453 0x0008*/0x0009 || uc == 0x3000 )
		{
			pos += bc;
		}
		else
		{
			break;
		}
	}
	//lineinfo更新
	if( pos != mLineInfo.GetLineInfo_Start(inLineIndex) + mLineInfo.GetLineInfo_Length(inLineIndex) )
	{
		if( pos < inText.GetItemCount() )
		{
			if( inText.Get(pos) == kUChar_LineEnd )
			{
				pos++;
				mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,true);
			}
		}
		mLineInfo.SetLineInfo_Length(inLineIndex,pos-mLineInfo.GetLineInfo_Start(inLineIndex));
	}
}

//#699
/**
EOF行調整（最終行が改行を含むなら最後に空行を追加する）
*/
ABool	ATextInfo::AddEmptyEOFLineIfNeeded()
{
	//最終行が改行を含むなら最後に空行を追加する
	//最終行は常に改行無しの行になる。
	
	//行数0なら終了
	if( GetLineCount() == 0 )   return false;
	
	//最終行が改行を含むかどうかの判定
	AIndex	lineIndex = GetLineCount()-1;
	if( mLineInfo.GetLineInfo_ExistLineEnd(lineIndex) == true )
	{
		//最終行が改行を含むなら、もう一行追加
		AddLineInfo();
		lineIndex++;
		//行データ設定
		mLineInfo.SetLineInfo_Start(lineIndex,
					mLineInfo.GetLineInfo_Start(lineIndex-1)+mLineInfo.GetLineInfo_Length(lineIndex-1));
		mLineInfo.SetLineInfo_Length(lineIndex,0);
		mLineInfo.SetLineInfo_ExistLineEnd(lineIndex,false);
		mLineInfo.SetLineInfo_ParagraphIndex(lineIndex,
					mLineInfo.GetLineInfo_ParagraphIndex(lineIndex-1) + 1);
		mLineInfo.ClearRecognizeFlags(lineIndex);//#693
		return true;
	}
	else
	{
		return false;
	}
}

//行情報のlineStartをずらす
void	ATextInfo::ShiftLineStarts( const AIndex inStartLineIndex, const AItemCount inAddedTextLength )
{
	/*#695
	for( AIndex lineIndex = inStartLineIndex; lineIndex < GetLineCount(); lineIndex++ )
	{
		mLineInfo.SetLineInfo_Start(lineIndex,GetLineStart(lineIndex)+inAddedTextLength);
	}
	*/
	mLineInfo.ShiftLineStarts(inStartLineIndex,inAddedTextLength);
}

//#695
/**
指定行以降の段落をずらす
*/
void	ATextInfo::ShiftParagraphIndex( const AIndex inStartLineIndex, const AItemCount inParagraphCount )
{
	mLineInfo.ShiftParagraphIndex(inStartLineIndex,inParagraphCount);
}

/*#695
//R0208
void	ATextInfo::AdjustParagraphIndex()
{
	AIndex	para = 0;
	for( AIndex lineIndex = 0; lineIndex < GetLineCount(); lineIndex++ )
	{
		mLineInfo.SetLineInfo_ParagraphIndex(lineIndex,para);
		if( mLineInfo.GetLineInfo_ExistLineEnd(lineIndex) == true )
		{
			para++;
		}
	}
}
*/

//
AIndex	ATextInfo::GetCurrentParagraphStartLineIndex( const AIndex inLineIndex ) const
{
	for( AIndex y = inLineIndex; y > 0; y-- )
	{
		if( mLineInfo.GetLineInfo_ExistLineEnd(y-1) == true )   return y;
	}
	return 0;
}

//指定行より後の行で、段落が始まる行のインデックスを得る
AIndex	ATextInfo::GetNextParagraphStartLineIndex( const AIndex inLineIndex ) const
{
	AItemCount	lineCount = GetLineCount();//B0358 高速化のため変数に格納
	for( AIndex lineIndex = inLineIndex; lineIndex < lineCount; lineIndex++ )
	{
		if( mLineInfo.GetLineInfo_ExistLineEnd(lineIndex) == true )
		{
			return lineIndex+1;
		}
	}
	return lineCount;
}

//
ATextIndex	ATextInfo::GetParagraphStartTextIndex( const AIndex inParagraphIndex ) const
{
	AIndex	lineIndex = GetLineIndexByParagraphIndex(inParagraphIndex);
	if( lineIndex >= GetLineCount() )   return GetTextLength();
	return GetLineStart(lineIndex);
}

//行Indexから段落Indexを取得
AIndex	ATextInfo::GetParagraphIndexByLineIndex( const AIndex inLineIndex ) const
{
	//R0208
	return mLineInfo.GetLineInfo_ParagraphIndex(inLineIndex);
	/*R0208 AIndex	paragraphIndex = 0;
	for( AIndex index = 0; index < inLineIndex; index++ )
	{
		if( GetLineExistLineEnd(index) == true )
		{
			paragraphIndex++;
		}
	}
	return paragraphIndex;*/
}

//段落Indexから行Indexを取得
AIndex	ATextInfo::GetLineIndexByParagraphIndex( const AIndex inParagraphIndex ) const
{
	/*#695
	AIndex	paragraphIndex = 0;
	for( AIndex index = 0; index < GetLineCount(); index++ )
	{
		if( paragraphIndex == inParagraphIndex )
		{
			return index;
		}
		if( GetLineExistLineEnd(index) == true )
		{
			paragraphIndex++;
		}
	}
	return GetLineCount();
	*/
	AIndex	lineIndex = mLineInfo.GetLineIndexFromParagraphIndex(inParagraphIndex);
	if( lineIndex != kIndex_Invalid )
	{
		return lineIndex;
	}
	else
	{
		//結果を従来の関数の結果と合わせておく
		return GetLineCount();
	}
}

//
AUniqID	ATextInfo::GetFirstGlobalIdentifierUniqID( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(inLineIndex);
}

//
AUniqID	ATextInfoForDocument::GetFirstLocalIdentifierUniqID( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_FirstLocalIdentifierUniqID(inLineIndex);
}

//
ABool	ATextInfo::IsParagraphStartLine( const AIndex inLineIndex ) const
{
	if( inLineIndex == 0 )   return true;
	return mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex-1);
}

//#493
/**
*/
ANumber	ATextInfoForDocument::GetLineMultiply( const AIndex inLineIndex ) const
{
	return 100;
	//drop return mLineInfo.GetLineInfoP_Multiply(inLineIndex);
}

//#699
/**
行計算スレッド結果を、メインスレッドのTextInfoへコピーする
コピー先（メインスレッド側）のオブジェクトからコールされる
@param inLineIndex スレッド計算対象行のindex
*/
ABool	ATextInfo::CopyFromCalculatorResult( const AIndex inLineIndex, const ATextInfo& inSrcTextInfo )
{
	//行計算完了フラグ
	ABool	completed = false;
	//スレッド計算対象行のLineStart, Len, ExistLineEndを取得
	AIndex	originalLineStart = mLineInfo.GetLineInfo_Start(inLineIndex);
	AItemCount	originalLen = mLineInfo.GetLineInfo_Length(inLineIndex);
	ABool	originalExistLineEnd = mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex);
	//スレッド計算対象行の段落indexを取得
	AIndex	paragraphIndex = 0;
	if( inLineIndex > 0 )
	{
		paragraphIndex = mLineInfo.GetLineInfo_ParagraphIndex(inLineIndex-1);
		if( mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex-1) == true )
		{
			paragraphIndex++;
		}
	}
	//textIndex: コピー先現在行の現在の開始text index
	AIndex	textIndex = originalLineStart;
	//dstLineIndex: コピー先現在行index
	AIndex	dstLineIndex = inLineIndex;
	//srcLineCount: コピーする行数
	AItemCount	srcLineCount = inSrcTextInfo.GetLineCount();
	//コピー先に行を一括追加（スレッド計算対象行の後ろに追加する）
	mLineInfo.InsertLines(dstLineIndex+1,srcLineCount);
	//コピー元行ごとにループ
	for( AIndex srcLineIndex = 0; srcLineIndex < srcLineCount; srcLineIndex++ )
	{
		//Len, ExistLineEnd, paragraphを設定→次の行へ移動→LineStartを設定という処理順になっている
		//ループ終了時、dstLineIndexは、新たなスレッド計算対象行（または、行計算完了時は余分な一行）の位置になる。
		
		//Len, ExsitLineEndをコピー。paragraphを設定。
		AItemCount	len = inSrcTextInfo.mLineInfo.GetLineInfo_Length(srcLineIndex);
		mLineInfo.SetLineInfo_Length(dstLineIndex,len);
		ABool	existLineEnd = inSrcTextInfo.mLineInfo.GetLineInfo_ExistLineEnd(srcLineIndex);
		mLineInfo.SetLineInfo_ExistLineEnd(dstLineIndex,existLineEnd);
		mLineInfo.SetLineInfo_ParagraphIndex(dstLineIndex,paragraphIndex);
		//単語数・文字数コピー
		mLineInfo.SetLineInfoP_CharCount(dstLineIndex,inSrcTextInfo.mLineInfo.GetLineInfoP_CharCount(srcLineIndex));
		mLineInfo.SetLineInfoP_WordCount(dstLineIndex,inSrcTextInfo.mLineInfo.GetLineInfoP_WordCount(srcLineIndex));
		//fprintf(stderr,"srcline:%ld dstline:%ld para:%ld start:%ld len:%ld\n",srcLineIndex,dstLineIndex,paragraphIndex,
		//			mLineInfo.GetLineInfo_Start(dstLineIndex),len);
		//改行ありならparagraphを＋１
		if( existLineEnd == true )
		{
			paragraphIndex++;
		}
		
		//次の行の開始text indexを計算
		textIndex += len;
		//コピー先現在行を設定
		dstLineIndex++;
		//LineStartを設定
		mLineInfo.SetLineInfo_Start(dstLineIndex,textIndex);
	}
	//AdjustLineInfo()でEOF空行追加している場合、その行の段落番号は途中段階での段落番号になっているので、ここで更新する。
	//dstLineIndexの行はスレッド計算対象行なので、EOF空行が存在するとしたら、その次の行。
	if( dstLineIndex+1 < GetLineCount() )
	{
		mLineInfo.SetLineInfo_ParagraphIndex(dstLineIndex+1,paragraphIndex);
	}
	//
	//fprintf(stderr,"inLineIndex:%ld lineStart:%ld textIndex:%ld endTextIndex:%ld srcLineCount:%ld\n",
	//			inLineIndex,originalLineStart,textIndex,endTextIndex,srcLineCount);
	//次のスレッド計算対象行の計算
	//remainLenに残りのテキスト長さを計算
	AItemCount	remainLen = originalLen - (textIndex-originalLineStart);
	if( remainLen > 0 )
	{
		//未完了分がまだあるなら、スレッド計算対象行にデータ設定
		mLineInfo.SetLineInfo_Length(dstLineIndex,remainLen);
		mLineInfo.SetLineInfo_ExistLineEnd(dstLineIndex,originalExistLineEnd);
		mLineInfo.SetLineInfo_ParagraphIndex(dstLineIndex,paragraphIndex);
	}
	else if( remainLen == 0 )
	{
		//終了したのでスレッド計算対象行は削除
		mLineInfo.Delete1Line(dstLineIndex);
		//EOF行調整
		AddEmptyEOFLineIfNeeded();
		//終了フラグOn
		completed = true;
	}
	else
	{
		//remainLen<0となることはないので、エラー
		_ACError("",NULL);
		completed = true;
	}
	return completed;
}

//#142
/**
行内単語数・文字数計算
*/
void	ATextInfo::CalcWordCount( const AText& inText, const AIndex inLineIndex )
{
	//行情報取得
	AIndex	lineStart = GetLineStart(inLineIndex);
	AItemCount	len = GetLineLengthWithLineEnd(inLineIndex);
	//結果初期化
	AItemCount	charCount = 0;
	AItemCount	wordCount = 0;
	AItemCount	paragraphCount = 0;
	//テキスト範囲内単語数・文字数取得
	GetWordCount(inText,lineStart,len,charCount,wordCount,paragraphCount, true);//#1403
	//結果を行情報に設定
	mLineInfo.SetLineInfoP_CharCount(inLineIndex,charCount);
	mLineInfo.SetLineInfoP_WordCount(inLineIndex,wordCount);
}

//#142
/**
テキスト範囲内単語数・文字数計算
*/
void	ATextInfo::GetWordCount( const AText& inText, const AIndex inStartIndex, const AItemCount inLength,
		AItemCount& outCharCount, AItemCount& outWordCount, AItemCount& outParagraphCount, const ABool inAdjustForLineInfo ) const//#1403
{
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	const AUChar*	ptr = textptr.GetPtr();
	const AItemCount	textlen = textptr.GetByteCount();
	//結果初期化
	outCharCount = 0;
	outWordCount = 0;
	outParagraphCount = 0;
	//モードフラグ
	ABool	alphabetMode = false;
	ABool	charExistAfterLineEndChar = false;
	//各文字毎ループ
	AIndex	epos = inStartIndex+inLength;
	for( AIndex pos = inStartIndex; pos < epos; )
	{
		//文字数
		outCharCount++;
		//★OS関数使用検討
		//★"_"をアルファベットにすべき？しないべき？
		//文字取得
		AUCS4Char	ch = 0;
		AItemCount bc = AText::Convert1CharToUCS4(ptr,textlen,pos,ch);
		//段落カウント
		if( ch == kUChar_LineEnd )
		{
			//段落数
			outParagraphCount++;
			//改行後に文字が存在しているかどうかのフラグを初期化
			charExistAfterLineEndChar = false;
		}
		else
		{
			//改行後に文字が存在しているかどうかのフラグをON
			charExistAfterLineEndChar = true;
		}
		//単語判定
		//#1402 ABool	isAlphabet = unicodeData.IsAlphabetOrNumber(ch);
		//#1402 It's等が1単語にならない対策として途中のアポストロフィ、ハイフン、ピリオドでアルファベットモードOFFにしないような対策も検討したが、Word等では、単純に、基本的にスペース文字か否かで単語判定をしているようなので、そちらに合わせる。（たとえば、" test[1] "も1語扱い、" ( "も1語扱いとなっており、違和感もあるが、web検索した限り、Wordの単語数カウント機能はネイティブでも広く使われているようなので、そちらに合わせる。）Wordでは日本語全角の扱いは異なる（一文字一単語）ようだが、日本語での単語数カウントは普通しないし、一文字一単語もおかしいので、スペース以外の連続を一単語とする。記号の扱い等、定められた標準はないようで、微妙にツールによって異なっている。
		ABool	isSpace = (ch == 0x0020 || ch == 0x0009 || ch == 0x000A || ch == 0x000D);
		ABool	isAlphabet = (isSpace == false);
		//今回：アルファベット、その前：アルファベット以外
		if( isAlphabet == true && alphabetMode == false )
		{
			//単語数
			outWordCount++;
			//アルファベットモードON
			alphabetMode = true;
		}
		//今回：アルファベット以外、その前：アルファベット
		if( isAlphabet == false && alphabetMode == true )
		{
			//アルファベットモードOFF
			alphabetMode = false;
		}
		//次の文字
		pos += bc;
	}
	//#1403
	//LineInfoに格納する場合（テキスト情報の「全体」用）は、単語途中で折り返し発生している場合の調整を行う
	if( inAdjustForLineInfo == true )
	{
		if( epos < inText.GetItemCount() )
		{
			//行の最後の文字がアルファベット、かつ、次の文字もアルファベットの場合は、1単語としてカウントされるべきなので、
			//この行のカウントは-1しておく。
			AUCS4Char	ch = 0;
			inText.Convert1CharToUCS4(epos, ch);
			ABool	isSpace = (ch == 0x0020 || ch == 0x0009 || ch == 0x000A || ch == 0x000D);
			ABool	isAlphabet = (isSpace == false);
			if( isAlphabet == true && alphabetMode == true )
			{
				outWordCount--;
			}
		}
	}
	//改行コード後の後に文字が存在している状態で終了なら、段落数をもう１つ追加
	if( charExistAfterLineEndChar == true )
	{
		outParagraphCount++;
	}
}

#pragma mark ===========================

#pragma mark ---文法認識

const AItemCount	kTokenStackMaxCount = 256;

//#698 文法認識スレッド化のため、RecognizeSyntax()をPrepare/Executeに分割

/**
全行文法認識
*/
void	ATextInfo::RecognizeSyntaxAll( const AText& inText, const AText& inFileURL, 
									   AArray<AUniqID>& outAddedGlobalIdentifiers, AArray<AIndex>& outAddedGlobalIdentifiersLineIndex,
									   ABool& outImportChanged, const ABool& inAbort )//#998
{
	//認識コンテキスト生成
	PrepareRecognizeSyntaxContext(inText,0,inFileURL);//#998
	//認識実行
	AArray<AUniqID>	deletedIdentifiers;
	ExecuteRecognizeSyntax(inText,kIndex_Invalid,
				deletedIdentifiers,outAddedGlobalIdentifiers,outAddedGlobalIdentifiersLineIndex,outImportChanged,inAbort);
	//認識コンテキスト削除
	ClearRecognizeSyntaxContext();
}

/**
文法認識コンテキスト生成
*/
AIndex	ATextInfo::PrepareRecognizeSyntaxContext( const AText& inText, const AIndex inStartLineIndex, const AText& inFileURL )//#998
{
	//#693 RegExpオブジェクトがPurgeされていたら再度RegExpを更新
	if( mRegExpObjectPurged == true )
	{
		UpdateRegExp();
	}
	
	//
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(mModeIndex);
	ASyntaxDefinition&	syntaxDefinition = modePrefDB.GetSyntaxDefinition();
	
	//======================== Context変数設定 ========================
	
	//変数リスト初期化
	/*#698 メンバ変数化
	ATextArray	variable_ValueArray;
	AArray<ATextIndex>	variable_StartPos;
	AArray<AItemCount>	variable_Length;
	//AArray<AItemCount>	variable_Indent;
	*/
	mRecognizeContext_Variable_ValueArray.DeleteAll();//#698
	mRecognizeContext_Variable_StartPos.DeleteAll();//#698
	mRecognizeContext_Variable_Length.DeleteAll();//#698
	mRecognizeContext_Variable_BlockStartIndent.DeleteAll();//#695
	for( AIndex i = 0; i < syntaxDefinition.GetVariableCount(); i++ )
	{
		mRecognizeContext_Variable_ValueArray.Add(GetEmptyText());
		mRecognizeContext_Variable_StartPos.Add(0);
		mRecognizeContext_Variable_Length.Add(0);
		mRecognizeContext_Variable_BlockStartIndent.Add(0);//#695
		//variable_Indent.Add(0);
	}
	
	//TokenStack初期化
	//TokenStackの実体は、StartPos, Lengthのリングバッファ(kTokenStackMaxCount個)。テキスト内容は保存しない。
	//SetVarWithTokenStackにより、対応するTokenStackの位置の情報が変数にコピーされる。
	//mRecognizeContext_CurrentTokenStackPositionは、ClearTokenStackされるまで、ずっとインクリメントされる。配列アクセス時に%kTokenStackMaxCount演算する。
	//ClearTokenStackにより、mRecognizeContext_CurrentTokenStackPositionを0に戻す。
	/*#698 メンバ変数化
	AIndex	mRecognizeContext_CurrentTokenStackPosition = 0;
	AArray<ATextIndex>	tokenStack_StartPos;
	AArray<AItemCount>	tokenStack_Length;
	*/
	mRecognizeContext_CurrentTokenStackPosition = 0;//#698
	mRecognizeContext_TokenStack_StartPos.DeleteAll();//#698
	mRecognizeContext_TokenStack_Length.DeleteAll();//#698
	for( AIndex i = 0; i < kTokenStackMaxCount; i++ )
	{
		mRecognizeContext_TokenStack_StartPos.Add(0);
		mRecognizeContext_TokenStack_Length.Add(0);
	}
	
	//==================終了可能行決定==================
	//終了可能行は開始行の次の行とする（開始行よりも前にすると、スレッドによる文法認識が何度も同じところを繰り返す可能性がある。）
	mRecognizeContext_EndPossibleLineIndex = inStartLineIndex+1;
	
	//==================開始行決定==================
	//（inStartLineIndexまでの行（inStartLineIndexを含む）をさかのぼって、最初にidleもしくはstableな状態になる行から始める）
	//idleまで戻らないと、識別子認識が途中からになってしまい、関数名等が認識できない。
	//検討必要：#pragma mark行　の編集中の速度等
	//B0379 段落の最初の行を取得
	mRecognizeContext_RecognizeStartLineIndex = inStartLineIndex - kLimit_RecognizeSyntax_MinLineCountToGoBackRecognizeStart;//#905 最低8行戻る
	if( mRecognizeContext_RecognizeStartLineIndex < 0 )   mRecognizeContext_RecognizeStartLineIndex = 0;
	for( ; mRecognizeContext_RecognizeStartLineIndex > 0 ; mRecognizeContext_RecognizeStartLineIndex-- )
	{
		if( IsParagraphStartLine(mRecognizeContext_RecognizeStartLineIndex) == true )   break;
	}
	//開始段落の最初の行の認識フラグを落とす
	//mLineInfo.ClearRecognizeFlags(mRecognizeContext_RecognizeStartLineIndex);//#693
	//------------------最初にidleもしくはstableな状態になる行まで戻る------------------
	//#695 64行以上は戻らないようにする
	AIndex	startLimit = mRecognizeContext_RecognizeStartLineIndex - kLimit_RecognizeSyntax_MaxLineCountToGoBackRecognizeStart;
	if( startLimit < 0 )
	{
		startLimit = 0;
	}
	//どこまで戻って解析するかを決定し、mRecognizeContext_RecognizeStartLineIndexに格納
	for( ; mRecognizeContext_RecognizeStartLineIndex > /*#695 0*/startLimit; mRecognizeContext_RecognizeStartLineIndex-- )
	{
		AIndex	stateIndex = mLineInfo.GetLineInfoP_StateIndex(mRecognizeContext_RecognizeStartLineIndex);
		if( (stateIndex == 0 || syntaxDefinition.GetSyntaxDefinitionState(stateIndex).IsStable() == true) &&
					mLineInfo.GetLineInfoP_DirectiveLevel(mRecognizeContext_RecognizeStartLineIndex) == 0 )//Directiveレベルが0になる行
		{
			break;
		}
	}
	//fprintf(stderr,"s:%ld ",mRecognizeContext_RecognizeStartLineIndex);
	
	//======================== 各変数毎の開始時BlockStartData設定 ========================
	//#695
	for( AIndex i = 0; i < syntaxDefinition.GetVariableCount(); i++ )
	{
		if( syntaxDefinition.GetVariableBlockStartFlag(i) == true )
		{
			//認識開始行段階でのBlockStartインデントを取得し、コンテキストに設定する
			mRecognizeContext_Variable_BlockStartIndent.Set(i,
						FindLastBlockStartIndentCount(mRecognizeContext_RecognizeStartLineIndex-1,i));
		}
	}
	
	//======================== 状態Context設定 ========================
	
	//デフォルト文法パート取得
	AText	defaultSyntaxPart;
	modePrefDB.GetData_Text(AModePrefDB::kDefaultSyntaxPart,defaultSyntaxPart);
	
	//初期状態設定
	/*#698AIndex	*/mRecognizeContext_CurrentStateIndex = syntaxDefinition.GetInitialState(inFileURL,defaultSyntaxPart);//R0000 #998
	/*#698AIndex	*/mRecognizeContext_PushedStateIndex = mRecognizeContext_CurrentStateIndex;//R0000
	if( mRecognizeContext_RecognizeStartLineIndex > 0 )
	{
		//行Index>0の場合は、開始行の開始状態は設定済みのはず。
		mRecognizeContext_CurrentStateIndex = mLineInfo.GetLineInfoP_StateIndex(mRecognizeContext_RecognizeStartLineIndex);
		mRecognizeContext_PushedStateIndex = mLineInfo.GetLineInfoP_PushedStateIndex(mRecognizeContext_RecognizeStartLineIndex);
	}
	/*#698 AIndex	*/mRecognizeContext_CurrentIndent = mLineInfo.GetLineInfoP_IndentCount(mRecognizeContext_RecognizeStartLineIndex);
	
	//======================== Directive用Context設定 ========================
	
	//#467 Directive認識
	//directive認識用データ取得
	/*#698 AIndex	*/mRecognizeContext_CurrentDirectiveLevel = 0;
	mRecognizeContext_NextDirectiveLevel = 0;
	/*#698 AIndex	*/mRecognizeContext_CurrentDisabledDirectiveLevel = kIndex_Invalid;
	mRecognizeContext_NextDisabledDirectiveLevel = kIndex_Invalid;
	ABool	recognizeDirective = modePrefDB.GetData_Bool(AModePrefDB::kRecognizeConditionalCompileDirective);
	//現在行のdirective状態を取得
	if( recognizeDirective == true )
	{
		mRecognizeContext_CurrentDirectiveLevel = mLineInfo.GetLineInfoP_DirectiveLevel(mRecognizeContext_RecognizeStartLineIndex);
		mRecognizeContext_NextDirectiveLevel = mRecognizeContext_CurrentDirectiveLevel;
		mRecognizeContext_CurrentDisabledDirectiveLevel = mLineInfo.GetLineInfoP_DisabledDirectiveLevel(mRecognizeContext_RecognizeStartLineIndex);
		mRecognizeContext_NextDisabledDirectiveLevel = mRecognizeContext_CurrentDisabledDirectiveLevel;
	}
	
	return mRecognizeContext_RecognizeStartLineIndex;
}

/**
文法認識実行
*/
AIndex	ATextInfo::ExecuteRecognizeSyntax( const AText& inText, const AIndex inEndLineIndex,
		AArray<AUniqID>& outDeletedIdentifiers, 
		AArray<AUniqID>& outAddedGlobalIdentifiers, AArray<AIndex>& outAddedGlobalIdentifiersLineIndex,
		ABool& outImportChanged, const ABool& inAbort )
{
	//返り値初期化
	outDeletedIdentifiers.DeleteAll();
	outAddedGlobalIdentifiers.DeleteAll();
	outAddedGlobalIdentifiersLineIndex.DeleteAll();
	outImportChanged = false;
	
	//#693 RegExpオブジェクトがPurgeされていたら再度RegExpを更新
	if( mRegExpObjectPurged == true )
	{
		UpdateRegExp();
	}
	
	//======================== 各種データをローカル変数に納める ========================
	
	//
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(mModeIndex);
	ASyntaxDefinition&	syntaxDefinition = modePrefDB.GetSyntaxDefinition();
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();//#664
	
	//各state毎にコメント状態かどうかのarray
	AArray<ABool>	isCommentStateArray;
	for( AIndex i = 0; i < syntaxDefinition.GetStateCount(); i++ )
	{
		isCommentStateArray.Add(syntaxDefinition.GetSyntaxDefinitionState(i).IsCommentState());
	}
	
	//
	ABool	recognizeDirective = modePrefDB.GetData_Bool(AModePrefDB::kRecognizeConditionalCompileDirective);
	AColor	directiveDisabledColor = kColor_Gray;
	modePrefDB.GetData_Color(AModePrefDB::kConditionalCompileDisabledColor,directiveDisabledColor);
	
	//define値配列
	AHashTextArray	enabledDefineArray;
	ATextArray	enabledDefineValueArray;
	//"#if 1"をenable扱いにする
	AText	t;
	t.SetCstring("1");
	enabledDefineArray.Add(t);
	enabledDefineValueArray.Add(t);
	//define値配列に設定データをコピー（キャッシュ）する
	for( AIndex i = 0; i < GetApp().NVI_GetAppPrefDB().GetItemCount_Array(AAppPrefDB::kDirectiveEnabledDefine); i++ )
	{
		AText	text;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefine,i,text);
		enabledDefineArray.Add(text);
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,i,text);
		enabledDefineValueArray.Add(text);
	}
	/*#698 既にまともに動作していない処理なので削除
	//
	ATextIndex	recogStartTextIndex = GetTextIndexFromTextPoint(inStartTextPoint);
	AIndex	recogStartStateIndex = kIndex_Invalid;
	//B0339 2.1.8b2時点でまともに動作していなかったのでnomemoryは削除 ABool	noMemoryStateFlag = false;
	*/
	
	/*#698 既に、mRecognizeContext_RecognizeStartLineIndex以降全ての行でDeleteLineIdentifiers()実行されるようになったので、
	identifierClearedLineを利用した判定（識別子を今回削除した行かどうかの判定）は不要。mRecognizeContext_RecognizeStartLineIndexを利用した判定方法に変更
	identifierClearedLineは使用しない。メモリ量・処理速度がもったいないため。
	AHashArray<AIndex>	identifierClearedLine;
	*/
	//======================== 各行ループ開始 ========================
	AIndex	tokenspos;
	AText	token;
	AIndex	lineIndex = mRecognizeContext_RecognizeStartLineIndex;
	ABool	stateStart = true;//RC2
	AIndex	reservedNextLinePushState = kIndex_Invalid;
	//終了行を設定#698
	AIndex	endLineIndex = inEndLineIndex;
	if( endLineIndex == kIndex_Invalid )
	{
		endLineIndex = GetLineCount();
	}
	//#695 既に認識済みの行を再認識した数
	AItemCount	recognizedLineCount = 0;
	//
	for( ; lineIndex < /*#698 GetLineCount()*/endLineIndex; lineIndex++ )
	{
		//AbortフラグONなら終了 #699
		if( inAbort == true )
		{
			return lineIndex;
		}
		
		//前行の処理でPushStateFromNextLineアクションを実行したときは、reservedNextLinePushStateに今回の行でpush stateを実行する
		if( IsParagraphStartLine(lineIndex) == true )
		{
			if( reservedNextLinePushState != kIndex_Invalid )
			{
				mRecognizeContext_PushedStateIndex = mRecognizeContext_CurrentStateIndex;
				mRecognizeContext_CurrentStateIndex = reservedNextLinePushState;
				stateStart = true;
				reservedNextLinePushState = kIndex_Invalid;
			}
		}
		
		//#450
		//フォールディングレベル初期化
		AFoldingLevel	foldingLevel = kFoldingLevel_None;
		
		//#907
		//現在の文法パートindexを取得
		AIndex	lineStartSyntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(mRecognizeContext_CurrentStateIndex).GetStateSyntaxPartIndex();
		
		//fprintf(stderr,"recog:%ld ",lineIndex);
		ABool	recognizedLine = mLineInfo.GetLineInfoP_Recognized(lineIndex);
		
		//======================== 終了判定 ========================
		if( lineIndex >= /*B0379 inStartTextPoint.y*/mRecognizeContext_EndPossibleLineIndex )
		{
			//#695
			//既に認識済みの行を再認識した数をカウント。（トリガー位置以降のみカウント）
			//kLimit_ExecuteRecognizeSyntax_MaxLineCountAfterRecognizedFlagTrueよりも多くなったら認識終了となる。
			if( recognizedLine == true )
			{
				recognizedLineCount++;
			}
			
			//現在状態がidle、かつ、認識済み、かつ、状態／pushed状態が一致していれば、そこで認識終了。（認識途中の識別子はなく、また、これ以上認識しても全く同じ結果になる。）
			if( recognizedLine == true && mLineInfo.GetLineInfo_ExistLineEnd(lineIndex) == true )
			{
				const ASyntaxDefinitionState&	stateobj = syntaxDefinition.GetSyntaxDefinitionState(mRecognizeContext_CurrentStateIndex);
				if(	(/*速度向上のため「現在状態が0またはstable状態」条件をコメントアウト。関数前のヘッダコメントを編集時に関数見出しが毎回更新されるので。→やっぱり復活。関数が認識されないことがある*/
					((mRecognizeContext_CurrentStateIndex == 0)||(stateobj.IsStable() == true))  //現在状態が0またはstable状態
					&&((mLineInfo.GetLineInfoP_StateIndex(lineIndex) == mRecognizeContext_CurrentStateIndex  //行の状態・pushed状態が現在状態と一致
					&&mLineInfo.GetLineInfoP_PushedStateIndex(lineIndex) == mRecognizeContext_PushedStateIndex))
					&&mRecognizeContext_CurrentIndent == mLineInfo.GetLineInfoP_IndentCount(lineIndex)//#467 )//B0339インデント一致条件入れてみる
					&&mRecognizeContext_CurrentDirectiveLevel == mLineInfo.GetLineInfoP_DirectiveLevel(lineIndex)//#467 directive一致条件
					&&mRecognizeContext_CurrentDisabledDirectiveLevel == mLineInfo.GetLineInfoP_DisabledDirectiveLevel(lineIndex))//#467 directive一致条件
					||
					((stateobj.IsGlobalState() == false)
					&&(mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex) != /*#216 kIndex_Invalid*/kUniqID_Invalid )
					&&(mLineInfo.GetLineInfoP_StateIndex(lineIndex) == 0)&&(mLineInfo.GetLineInfoP_IndentCount(lineIndex) == 0)//#467)
					&&mRecognizeContext_CurrentDirectiveLevel == mLineInfo.GetLineInfoP_DirectiveLevel(lineIndex)//#467 directive一致条件
					&&mRecognizeContext_CurrentDisabledDirectiveLevel == mLineInfo.GetLineInfoP_DisabledDirectiveLevel(lineIndex))//#467 directive一致条件
					/*B0339 現在状態がglobalstate以外かつ行状態が0の場合は、GlobalIdentifierが存在する行で終了*/
					//#695 認識済み行が制限値より多くなったら上記条件を無視して認識完了とする
					|| (recognizedLineCount > kLimit_ExecuteRecognizeSyntax_MaxLineCountAfterRecognizedFlagTrue) )
				{
					//#698 outRecognizedStartLineIndex = mRecognizeContext_RecognizeStartLineIndex;
					//#698 outRecognizedEndLineIndex = lineIndex;
					AIndex	recognizeEndLineIndex = lineIndex;//#698
					
					//B0340 インデントが変わっている場合、インデントが0以下になるか、GlobalIdentifierが存在する行まで、インデントをずらしていく
					if( mRecognizeContext_CurrentIndent != mLineInfo.GetLineInfoP_IndentCount(lineIndex) )
					{
						AItemCount	indentOffset = mRecognizeContext_CurrentIndent - mLineInfo.GetLineInfoP_IndentCount(lineIndex);
						for( ; lineIndex < GetLineCount(); lineIndex++ )
						{
							if( mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex) !=  /*#216 kIndex_Invalid*/kUniqID_Invalid )   break;
							AItemCount	indent = mLineInfo.GetLineInfoP_IndentCount(lineIndex);
							if( indent <= 0 )   break;
							indent += indentOffset;
							mLineInfo.SetLineInfoP_IndentCount(lineIndex,indent);
						}
					}
					//認識最終行のcomment/block folding levelを設定する（ループ中は常に前行のfolding levelを設定しているため。）
					UpdateCommentBlockFoldingLevel(lineIndex-1,isCommentStateArray);
					
					return recognizeEndLineIndex;//#698
				}
			}
		}
		//B0339 対象段落のIdentifier削除、見出し認識は、認識開始行（stable行）から行うようにする。
		//（キャレット段落以降にすると、たとえば、関数について、{入力時に、関数名の行のIdentifier削除が実行されず、結果として関数名が識別子登録されない
		//{
		
		//stable仕様にしたのであまり戻らないはずなので右条件削除if( recognizedLine == false )
		//{
		
		//======================== 対象段落の識別子を削除 ========================
		
		//対象段落のIdentifierを削除 B0379対象行→対象段落（段落内の全ての行）
		if( IsParagraphStartLine(lineIndex) == true )
		{
			for( AIndex i = lineIndex; i < GetLineCount(); i++ )
			{
				ABool	importChanged = false;
				DeleteLineIdentifiers(i,outDeletedIdentifiers,importChanged);
				if( importChanged == true )
				{
					outImportChanged = true;
				}
				//認識フラグ消去
				mLineInfo.ClearRecognizeFlags(i);
				//#698 mRecognizeContext_RecognizeStartLineIndexを利用した判定方法に変更。identifierClearedLineは使用しない。 identifierClearedLine.Add(i);
				if( mLineInfo.GetLineInfo_ExistLineEnd(i) == true )   break;
			}
		}
		
		//==================行インデント取得==================
		
		AItemCount	indentCountAtLineStart = mRecognizeContext_CurrentIndent;
		
		//======================== Directive認識 ========================
		
		//Directive認識 #467
		if( IsParagraphStartLine(lineIndex) == true && recognizeDirective == true &&
					syntaxDefinition.GetSyntaxDefinitionState(mRecognizeContext_CurrentStateIndex).IsDisableDirective() == false )
		{
			//directiveでは行頭のみ認識
			//#695 高速化のため、ループ外のAText tokenを使う（毎行メモリ取得・解放発生しないように）。AText	token;
			AIndex	pos = GetLineStart(lineIndex);
			AIndex	epos = pos+GetLineLengthWithLineEnd(lineIndex);
			AIndex	tokenspos = epos;
			if( GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,pos,epos,token,tokenspos,lineStartSyntaxPartIndex) == true )//#664
			{
				//Directive開始トークン(#）かどうかを判定
				if( syntaxDefinition.IsDirectiveString(token) == true )
				{
					//#の次のトークンを取得
					if( GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,pos,epos,token,tokenspos,lineStartSyntaxPartIndex) == true )//#664
					{
						//#if
						if( syntaxDefinition.IsDirectiveIfString(token) == true )
						{
							//Directiveレベルインクリメント（次行から）
							mRecognizeContext_NextDirectiveLevel++;
							//無効行判定（現在有効の場合のみ判定）
							if( mRecognizeContext_CurrentDisabledDirectiveLevel == kIndex_Invalid )
							{
								//スペース系文字を省いてtextへ格納
								AText	text;
								inText.GetTextWithoutSpaces(pos,epos-pos,text);
								//enabled defineかつvalueが0以外なら無効defineと判断
								AIndex	enabledDefineArrayIndex = enabledDefineArray.Find(text);
								ABool	disabled = true;
								if( enabledDefineArrayIndex != kIndex_Invalid )
								{
									disabled = false;
									//値が空または"0"ならdisable
									const AText& value = enabledDefineValueArray.GetTextConst(enabledDefineArrayIndex);
									if( value.GetItemCount() == 0 || value.Compare("0") == true )
									{
										disabled = true;
									}
								}
								//システムヘッダの場合、"#if 0"以外は常にenableにする
								if( mSystemHeaderMode == true )
								{
									disabled = false;
								}
								if( text.Compare("0") == true )
								{
									disabled = true;
								}
								//
								if( disabled == true )
								{
									mRecognizeContext_NextDisabledDirectiveLevel = mRecognizeContext_CurrentDirectiveLevel;
								}
							}
							//#450
							//FoldingLevel設定(If)
							foldingLevel |= kFoldingLevel_IfStart;
						}
						//#else
						else if( syntaxDefinition.IsDirectiveElseString(token) == true )
						{
							//Directiveレベルデクリメント（現在行のみ）
							mRecognizeContext_CurrentDirectiveLevel--;
							//現在有効の場合は無効にする
							if( mRecognizeContext_CurrentDisabledDirectiveLevel == kIndex_Invalid )
							{
								mRecognizeContext_NextDisabledDirectiveLevel = mRecognizeContext_CurrentDirectiveLevel;
							}
							//現在無効の場合は有効にする
							else
							{
								mRecognizeContext_NextDisabledDirectiveLevel = kIndex_Invalid;
							}
						}
						//#elif
						else if( syntaxDefinition.IsDirectiveElifString(token) == true )
						{
							//Directiveレベルデクリメント（現在行のみ）
							mRecognizeContext_CurrentDirectiveLevel--;
							//無効行判定（現在有効の場合のみ判定）
							if( mRecognizeContext_CurrentDisabledDirectiveLevel == kIndex_Invalid )
							{
								//スペース系文字を省いてtextへ格納
								AText	text;
								inText.GetTextWithoutSpaces(pos,epos-pos,text);
								//enabled defineかつvalueが0以外なら無効defineと判断
								AIndex	enabledDefineArrayIndex = enabledDefineArray.Find(text);
								ABool	disabled = true;
								if( enabledDefineArrayIndex != kIndex_Invalid )
								{
									disabled = false;
									//値が空または"0"ならdisable
									const AText& value = enabledDefineValueArray.GetTextConst(enabledDefineArrayIndex);
									if( value.GetItemCount() == 0 || value.Compare("0") == true )
									{
										disabled = true;
									}
								}
								//システムヘッダの場合、"#if 0"以外は常にenableにする
								if( mSystemHeaderMode == true )
								{
									disabled = false;
								}
								if( text.Compare("0") == true )
								{
									disabled = true;
								}
								//
								if( disabled == true )
								{
									mRecognizeContext_NextDisabledDirectiveLevel = mRecognizeContext_CurrentDirectiveLevel;
								}
							}
						}
						//#ifdef
						else if( syntaxDefinition.IsDirectiveIfdefString(token) == true )
						{
							//Directiveレベルインクリメント（次行から）
							mRecognizeContext_NextDirectiveLevel++;
							//無効行判定（現在有効の場合のみ判定）
							if( mRecognizeContext_CurrentDisabledDirectiveLevel == kIndex_Invalid )
							{
								//スペース系文字を省いてtextへ格納
								AText	text;
								inText.GetTextWithoutSpaces(pos,epos-pos,text);
								//enabled define以外（かつシステムヘッダ以外）なら無効defineと判断
								if( enabledDefineArray.Find(text) == kIndex_Invalid && 
											mSystemHeaderMode == false )
								{
									mRecognizeContext_NextDisabledDirectiveLevel = mRecognizeContext_CurrentDirectiveLevel;
								}
							}
							//#450
							//FoldingLevel設定(If)
							foldingLevel |= kFoldingLevel_IfStart;
						}
						//#ifndef
						else if( syntaxDefinition.IsDirectiveIfndefString(token) == true )
						{
							//Directiveレベルインクリメント（次行から）
							mRecognizeContext_NextDirectiveLevel++;
							//無効行判定（現在有効の場合のみ判定）
							if( mRecognizeContext_CurrentDisabledDirectiveLevel == kIndex_Invalid )
							{
								//スペース系文字を省いてtextへ格納
								AText	text;
								inText.GetTextWithoutSpaces(pos,epos-pos,text);
								//enabled define（かつシステムヘッダ以外）なら無効defineと判断
								if( enabledDefineArray.Find(text) != kIndex_Invalid && 
											mSystemHeaderMode == false )
								{
									mRecognizeContext_NextDisabledDirectiveLevel = mRecognizeContext_CurrentDirectiveLevel;
								}
							}
							//#450
							//FoldingLevel設定(If)
							foldingLevel |= kFoldingLevel_IfStart;
						}
						//#endif
						else if( syntaxDefinition.IsDirectiveEndifString(token) == true )
						{
							//Directiveレベルデクリメント（次行から）
							if( mRecognizeContext_NextDirectiveLevel > 0 )
							{
								mRecognizeContext_NextDirectiveLevel--;
							}
							//デクリメント後のレベルが、disable時のレベル以下ならdisable状態解除
							if( mRecognizeContext_CurrentDisabledDirectiveLevel >= mRecognizeContext_NextDirectiveLevel )
							{
								mRecognizeContext_NextDisabledDirectiveLevel = kIndex_Invalid;
							}
							//#450
							foldingLevel |= kFoldingLevel_IfEnd;
							//FoldingLevel設定(EndIf)
						}
					}
				}
			}
		}
		
		//======================== 見出し認識 ========================
		
		mLineInfo.SetLineInfoP_ColorizeLine(lineIndex,false);
		//★#695 mLineInfoP_RegExpGroupColor_StartIndex.Set(lineIndex,kIndex_Invalid);//#603
		//★#695 mLineInfoP_RegExpGroupColor_EndIndex.Set(lineIndex,kIndex_Invalid);//#603
		if( IsParagraphStartLine(lineIndex) == true )
		{
			/*#130 drop ピリオドで開始する行を見出し
			//#130
			//
			if( false )
			{
				//行情報取得
				AIndex	spos = GetLineStart(lineIndex);
				AIndex	epos = spos + GetLineLengthWithoutLineEnd(lineIndex);
				AIndex	level = 0;
				//各文字毎ループ
				for( AIndex	pos = spos; pos < epos; pos++ )
				{
					//ピリオドで始まる場合、ピリオドの数だけレベル++
					if( inText.Get(pos) == '.' )
					{
						level++;
					}
					else
					{
						break;
					}
				}
				//アウトライン見出しを識別子として追加
				if( level > 0 )
				{
					AText	menuText;
					inText.GetText(spos,epos-spos,menuText);
					AUniqID	UniqID = AddLineGlobalIdentifier(kIdentifierType_OutlineHeader,
							lineIndex,0,epos-spos,0,GetEmptyText(),menuText,GetEmptyText(),
							GetEmptyText(),GetEmptyText(),GetEmptyText(),true,level-1);
					if( UniqID != kUniqID_Invalid && menuText.GetItemCount() > 0 )
					{
						outAddedGlobalIdentifiers.Add(UniqID);
						outAddedGlobalIdentifiersLineIndex.Add(lineIndex);
					}
				}
			}
			*/
			//段落の最初から正規表現一致するかどうか判定する
			//Identifierは各行に追加される
			
			AItemCount	jumpSetupItemCount = mJumpSetupRegExp.GetItemCount();
			for( AIndex jumpSetupIndex = 0; jumpSetupIndex < jumpSetupItemCount; jumpSetupIndex++ )
			{
				AIndex	pos = GetLineStart(lineIndex);//B0354+offset;//B0324
				AIndex	nextParagraphLineIndex = GetNextParagraphStartLineIndex(lineIndex);
				AIndex	endpos = GetLineStart(nextParagraphLineIndex-1)+GetLineLengthWithLineEnd(nextParagraphLineIndex-1);
				ARegExp&	regexp = mJumpSetupRegExp.GetObject(jumpSetupIndex);
				//正規表現が未設定なら何もしない
				if( regexp.IsREValid() == false )
				{
					continue;
				}
				//
				regexp.InitGroup();
				if( regexp.Match(inText,pos,endpos) == true )
				{
					//#493
					mLineInfo.SetLineInfoP_Multiply(lineIndex,modePrefDB.GetJumpSetup_DisplayMultiply(jumpSetupIndex));
					//
					if( modePrefDB.GetJumpSetup_ColorizeLine(jumpSetupIndex) == true ) 
					{
						mLineInfo.SetLineInfoP_ColorizeLine(lineIndex,true);
						AColor	color = modePrefDB.GetJumpSetup_LineColor(jumpSetupIndex);
						mLineInfo.SetLineInfoP_LineColor(lineIndex,color);
					}//B0316 if文の範囲変更（行の色づけ設定が無くても他の機能は各設定に従うべき）
					AText	anchorText;//R0212
					AText	menuText;
					if( modePrefDB.GetJumpSetup_RegisterToMenu(jumpSetupIndex) == true )
					{
						AText	origMenuText;
						modePrefDB.GetJumpSetup_MenuText(jumpSetupIndex,origMenuText);
						regexp.ChangeReplaceText(inText,origMenuText,menuText);
						//R0212
						/*ANumber	anchorGroup = modePrefDB.GetData_NumberArray(AModePrefDB::kJumpSetup_AnchorGroup,jumpSetupIndex);
						if( anchorGroup != 0 )
						{
						AText	groupText;
						groupText.SetFormattedCstring("$%ld",anchorGroup);
						regexp.ChangeReplaceText(inText,groupText,anchorText);
						}*/
					}
					AText	keyword;
					AIndex	categoryIndex = modePrefDB.GetJumpSetupCategoryIndex(jumpSetupIndex);
					if( modePrefDB.GetJumpSetup_RegisterAsKeyword(jumpSetupIndex) == true )
					{
						ANumber	keywordGroup = modePrefDB.GetJumpSetup_KeywordGroup(jumpSetupIndex);
						AText	keywordGroupText;
						keywordGroupText.SetFormattedCstring("$%ld",keywordGroup);
						regexp.ChangeReplaceText(inText,keywordGroupText,keyword);
					}
					AIndex	spos = GetLineStart(lineIndex);
					AIndex	epos = pos;
					/*#844
					ANumber	selectionGroup = modePrefDB.GetData_NumberArray(AModePrefDB::kJumpSetup_SelectionGroup,jumpSetupIndex);
					if( selectionGroup != 0 )
					{
						regexp.GetGroupRange(selectionGroup-1,spos,epos);//B0379
					}
					*/
					AIndex	outlineLevel = modePrefDB.GetJumpSetupOutlineLevel(jumpSetupIndex);
					AUniqID	UniqID = AddLineGlobalIdentifier(kIdentifierType_JumpSetupRegExp,
							/*B0379 GetLineStart(lineIndex),pos*/ /*#698 spos,epos*/ lineIndex,0,epos-spos,
							categoryIndex,keyword,menuText,anchorText,
															 GetEmptyText(),GetEmptyText(),GetEmptyText(),false,
															 outlineLevel + indentCountAtLineStart*10);//RC2 #593
					if( UniqID != kUniqID_Invalid && menuText.GetItemCount() > 0 )//#695
					{
						outAddedGlobalIdentifiers.Add(UniqID);
						outAddedGlobalIdentifiersLineIndex.Add(lineIndex);
						//折りたたみレベル設定（見出し）
						foldingLevel |= kFoldingLevel_Header;
					}
					//#603 見出し正規表現グループ色づけ
					if( modePrefDB.GetJumpSetup_ColorizeRegExpGroup(jumpSetupIndex) == true )
					{
						AIndex	lineStart = GetLineStart(lineIndex);
						AIndex	spos = lineStart;
						AIndex	epos = pos;
						ANumber	group = modePrefDB.GetJumpSetup_ColorizeRegExpGroup_Number(jumpSetupIndex);
						if( group != 0 )
						{
							regexp.GetGroupRange(group-1,spos,epos);
						}
						AColor	color = modePrefDB.GetJumpSetup_ColorizeRegExpGroup_Color(jumpSetupIndex);
						//★#695 mLineInfoP_RegExpGroupColor.Set(lineIndex,color);
						//★#695 mLineInfoP_RegExpGroupColor_StartIndex.Set(lineIndex,spos-lineStart);
						//★#695 mLineInfoP_RegExpGroupColor_EndIndex.Set(lineIndex,epos-lineStart);
					}
				}
			}
		}
		else
		{
			AIndex	paragraphStartLineIndex = GetCurrentParagraphStartLineIndex(lineIndex);
			if( mLineInfo.GetLineInfoP_ColorizeLine(paragraphStartLineIndex) == true )
			{
				mLineInfo.SetLineInfoP_ColorizeLine(lineIndex,true);
				mLineInfo.SetLineInfoP_LineColor(lineIndex,mLineInfo.GetLineInfoP_LineColor(paragraphStartLineIndex));
			}
		}
		//}
		
		//======================== 各情報設定 ========================
		
		//
		mLineInfo.SetLineInfoP_StateIndex(lineIndex,mRecognizeContext_CurrentStateIndex);
		mLineInfo.SetLineInfoP_PushedStateIndex(lineIndex,mRecognizeContext_PushedStateIndex);
		mLineInfo.SetLineInfoP_IndentCount(lineIndex,mRecognizeContext_CurrentIndent);
		//#693
		//フォールディング設定
		mLineInfo.SetLineInfoP_FoldingLevel(lineIndex,foldingLevel);
		//コメント/Block Folding判定
		if( lineIndex > 0 )
		{
			UpdateCommentBlockFoldingLevel(lineIndex-1,isCommentStateArray);
		}
		//
		mLineInfo.SetLineInfoP_Recognized(lineIndex,true);
		mLineInfo.SetLineInfoP_DirectiveLevel(lineIndex,mRecognizeContext_CurrentDirectiveLevel);//#467
		mLineInfo.SetLineInfoP_DisabledDirectiveLevel(lineIndex,mRecognizeContext_CurrentDisabledDirectiveLevel);//#467
		//#467 directive行情報更新
		ABool	disabledLine = ((mRecognizeContext_CurrentDisabledDirectiveLevel!=kIndex_Invalid)&&
				(mRecognizeContext_NextDisabledDirectiveLevel!=kIndex_Invalid));
		mRecognizeContext_CurrentDirectiveLevel = mRecognizeContext_NextDirectiveLevel;
		mRecognizeContext_CurrentDisabledDirectiveLevel = mRecognizeContext_NextDisabledDirectiveLevel;
		//fprintf(stderr,"L:%ld I:%ld  ",lineIndex,mRecognizeContext_CurrentIndent);
		//fprintf(stderr,"L:%ld DirectiveLevel:%ld DirectiveDisabled:%ld \n",lineIndex,mRecognizeContext_CurrentDirectiveLevel,disabledDirectiveLevel);
		//fprintf(stderr,"L:%ld %ld ",lineIndex,mRecognizeContext_CurrentDisabledDirectiveLevel);
		//#467 directiveによるdisable行なら行の色設定して、これ以降の文法認識はしない
		if( disabledLine == true )
		{
			mLineInfo.SetLineInfoP_ColorizeLine(lineIndex,true);
			mLineInfo.SetLineInfoP_LineColor(lineIndex,directiveDisabledColor);
			continue;
		}
		//}
		
		//======================== SyntaxDefinition 文法認識 ========================
		
		AIndex	pos = GetLineStart(lineIndex);
		AIndex	endpos = pos+GetLineLengthWithLineEnd(lineIndex);
		while( pos < endpos )
		{
			/*#698 既にまともに動作していない処理なので削除
			//
			if( pos >= recogStartTextIndex && recogStartStateIndex == kIndex_Invalid )
			{
				recogStartStateIndex = mRecognizeContext_CurrentStateIndex;
				*B0339 2.1.8b2時点でまともに動作していなかったのでnomemoryは削除
				if( syntaxDefinition.GetSyntaxDefinitionState(recogStartStateIndex).IsNoMemoryState() == true )
				{
					noMemoryStateFlag = true;
				}*
			}*/
			//
			ASyntaxDefinitionState&	state = syntaxDefinition.GetSyntaxDefinitionState(mRecognizeContext_CurrentStateIndex);
			const AArray<AIndex>&	conditionTextByteCountArrayIndexTable = state.GetConditionTextByteCountArrayIndexTable();//#693
			
			//現在の文法パートindexを取得
			AIndex	syntaxPartIndex = state.GetStateSyntaxPartIndex();
			
			//次のアクションシーケンスの取得
			//条件テキストにヒットするまで、トークン単位で読み進めていく。（条件テキストとの比較は、トークン開始位置からのみ行う。）
			AIndex	seqIndex = kIndex_Invalid;
			AIndex	spos = pos;
			AText	regExpGroup1;
			//RC2 状態開始時、一度だけ、startアクションを実行する
			if( stateStart == true )
			{
				seqIndex = state.GetStartActionSequenceIndex();
				stateStart = false;
			}
			if( seqIndex == kIndex_Invalid )//stateの最初で、startActionが存在する場合は、まずそれを実行する。（startAction実行時は次のwhileを実行せず、直接アクション実行へ）
			
			while( pos < endpos )
			{
				//タブ、スペース読み飛ばし
				if( inText.SkipTabSpace(pos,endpos) == false )   break;
				spos = pos;
				
				//==================正規表現一致検索==================
				for( AIndex i = 0; i < mSyntaxDefinitionRegExp.GetItemCount(); i++ )
				{
					if( mSyntaxDefinitionRegExp_State.Get(i) == mRecognizeContext_CurrentStateIndex )
					{
						AIndex	p = pos;
						if( mSyntaxDefinitionRegExp.GetObject(i).Match(inText,p,endpos) == true )
						{
							//正規表現グループ1を記憶
							AText	g1("$1");
							mSyntaxDefinitionRegExp.GetObject(i).ChangeReplaceText(inText,g1,regExpGroup1);
							//
							seqIndex = mSyntaxDefinitionRegExp_Index.Get(i);
							//読み進め
							AIndex	epos = p;
							while( pos < epos )
							{
								if( GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,pos,epos,token,tokenspos,syntaxPartIndex) == false )   break;//#664
								if( token.GetItemCount() == 0 )   break;
								if( token.Get(0) != kUChar_LineEnd )
								{
									mRecognizeContext_TokenStack_StartPos.Set(mRecognizeContext_CurrentTokenStackPosition%kTokenStackMaxCount,tokenspos);
									mRecognizeContext_TokenStack_Length.Set(mRecognizeContext_CurrentTokenStackPosition%kTokenStackMaxCount,pos-tokenspos);
									mRecognizeContext_CurrentTokenStackPosition++;
								}
							}
							break;
						}
					}
				}
				//正規表現一致したならここで条件一致検索ループ終了
				if( seqIndex != kIndex_Invalid )
				{
					break;
				}
				
				//==================条件テキスト検索==================
				//最初の1バイトを読み、可能性のある条件テキストのバイト数分のテキストについて、条件テキストとの一致検索を試みる
				AUChar	firstch = inText.Get(pos);
				//#693 const AArray<AItemCount>&	byteCountArray = state.GetConditionTextByteCountArray(firstch);
				AIndex	byteCountArrayIndex = conditionTextByteCountArrayIndexTable.Get(firstch);//#693
				if( byteCountArrayIndex != kIndex_Invalid )//#693
				{
					const AArray<AItemCount>&	byteCountArray = state.GetConditionTextByteCountArray(byteCountArrayIndex);//#693
					//
					for( AIndex i = 0; i < byteCountArray.GetItemCount(); i++ )
					{
						AItemCount	byteCount = byteCountArray.Get(i);
						if( pos+byteCount > inText.GetItemCount() )   continue;
						//win 080722 forxxxがforに一致してしまう問題対策
						//fprintf(stderr,"n ");
						if( pos+byteCount < inText.GetItemCount() )
						{
							//fprintf(stderr,"st:%c ",inText.Get(pos));
							//fprintf(stderr,"et:%c ",inText.Get(pos+byteCount));
							//アルファベットで始まる単語で、pos+byteCountの位置（一致部分の後ろの文字）にアルファベットがあれば、一致しないとみなす
							if( syntaxDefinition.IsStartAlphabet(inText.Get(pos),syntaxPartIndex) == true &&
										syntaxDefinition.IsContinuousAlphabet(inText.Get(pos+byteCount),syntaxPartIndex) == true )   continue;
						}
						
						seqIndex = state.FindActionSequenceIndex(inText,pos,byteCount);
						if( seqIndex != kIndex_Invalid )
						{
							//条件テキストと一致した場合
							AIndex	epos = pos + byteCount;
							while( pos < epos )
							{
								if( GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,pos,epos,token,tokenspos,syntaxPartIndex) == false )   break;//#664
								if( token.GetItemCount() == 0 )   break;
								if( token.Get(0) != kUChar_LineEnd )
								{
									mRecognizeContext_TokenStack_StartPos.Set(mRecognizeContext_CurrentTokenStackPosition%kTokenStackMaxCount,tokenspos);
									mRecognizeContext_TokenStack_Length.Set(mRecognizeContext_CurrentTokenStackPosition%kTokenStackMaxCount,pos-tokenspos);
									mRecognizeContext_CurrentTokenStackPosition++;
								}
							}
							//#includeeでも、"#include"に一致してしまう→上の「forxxxがforに一致してしまう問題対策」にて対策済み
							break;
						}
					}
				}
				if( seqIndex != kIndex_Invalid )
				{
					break;
				}
				else
				{
					//条件テキストと一致しなかった場合は、１トークン読み進める。
					if( GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,pos,endpos,token,tokenspos,syntaxPartIndex) == false )   break;//#664
					if( token.GetItemCount() == 0 )   break;
					if( token.Get(0) != kUChar_LineEnd )
					{
						mRecognizeContext_TokenStack_StartPos.Set(mRecognizeContext_CurrentTokenStackPosition%kTokenStackMaxCount,tokenspos);
						mRecognizeContext_TokenStack_Length.Set(mRecognizeContext_CurrentTokenStackPosition%kTokenStackMaxCount,pos-tokenspos);
						mRecognizeContext_CurrentTokenStackPosition++;
					}
					//------------------ヒアドキュメント終了テキスト一致判定------------------
					//終了テキストが"heredocumentid"という変数に入っているのでその変数を取得
					AText	hereDocumentIdVarName("heredocumentid");
					AIndex	varIndex = syntaxDefinition.FindVariableIndex(hereDocumentIdVarName);
					if( varIndex != kIndex_Invalid )
					{
						//ヒアドキュメント終了テキスト取得
						AText	hereDocumentId;
						mRecognizeContext_Variable_ValueArray.Get(varIndex,hereDocumentId);
						//tokenとの一致を判定
						if( token.Compare(hereDocumentId) == true )
						{
							seqIndex = state.GetHereDocumentEndSequenceIndex();
							if( seqIndex != kIndex_Invalid )
							{
								break;
							}
						}
					}
					//どの条件テキストとも一致しなかった場合、DefaultActionを検索する。
					seqIndex = state.GetDefaultActionSequenceIndex();
					if( seqIndex != kIndex_Invalid )
					{
						break;
					}
				}
			}
			//行末まで、どの条件テキストにもヒットせず、DefaultActionもなかった場合はbreak
			if( seqIndex == kIndex_Invalid )   break;
			
			/*
			//debug
			if( true )
			{
				AText	text;
				text.SetCstring("Token:");
				text.AddText(token);
				text.AddFormattedCstring(" SeqIndex:%ld\n",seqIndex);
				text.OutputToStderr();
			}
			*/
			
			//==================アクションシーケンスのアクションを順に実行する==================
			ASyntaxDefinitionActionSequence&	seq = state.GetActionSequence(seqIndex);
			for( AIndex actionIndex = 0; actionIndex < seq.GetActionItemCount(); actionIndex++ )
			{
				ASyntaxDefinitionActionType	actionType = seq.GetActionType(actionIndex);
				switch(actionType)
				{
					//SetVarWithTokenStack
				  case kSyntaxDefinitionActionType_SetVarWithTokenStack:
					{
						//fprintf(stderr,"SetVarWithTokenStack ");
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						ANumber	startnum = seq./*#693GetStartNumber*/GetParameter1(actionIndex);
						AIndex	startStackIndex;
						if( startnum == kSyntaxTokenStackIndex_First )
						{
							startStackIndex = 0;
						}
						else
						{
							startStackIndex = mRecognizeContext_CurrentTokenStackPosition-1 + startnum;
						}
						if( startStackIndex < 0 )   startStackIndex = 0;
						ANumber	endnum = seq./*#693GetEndNumber*/GetParameter2(actionIndex);
						AIndex	endStackIndex;
						if( endnum == kSyntaxTokenStackIndex_Next )
						{
							endStackIndex = startStackIndex;
						}
						else
						{
							endStackIndex = mRecognizeContext_CurrentTokenStackPosition-2 + endnum;
						}
						if( endStackIndex < 0 )   endStackIndex = 0;
						ATextIndex	startPos = mRecognizeContext_TokenStack_StartPos.Get(startStackIndex%kTokenStackMaxCount);
						ATextIndex	endPos = mRecognizeContext_TokenStack_StartPos.Get(endStackIndex%kTokenStackMaxCount) 
								+ mRecognizeContext_TokenStack_Length.Get(endStackIndex%kTokenStackMaxCount);
						AText	text;
						inText.GetText(startPos,endPos-startPos,text);
						mRecognizeContext_Variable_ValueArray.Set/*#693_SwapContent*/(varIndex,text);
						mRecognizeContext_Variable_StartPos.Set(varIndex,startPos);
						mRecognizeContext_Variable_Length.Set(varIndex,endPos-startPos);
						//text.OutputToStderr();
						break;
					}
					//SetVarStartFromNextChar
				  case kSyntaxDefinitionActionType_SetVarStartFromNextChar:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						mRecognizeContext_Variable_StartPos.Set(varIndex,pos);
						break;
					}
					//SetVarStart
				  case kSyntaxDefinitionActionType_SetVarStart:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						mRecognizeContext_Variable_StartPos.Set(varIndex,spos);
						break;
					}
					//SetVarStartOrContinue RC2
					//連続している場合は、startを更新しない（前回のstartをstartのままにする）
				  case kSyntaxDefinitionActionType_SetVarStartOrContinue:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						if( mRecognizeContext_Variable_StartPos.Get(varIndex) + mRecognizeContext_Variable_Length.Get(varIndex) != spos )
						{
							mRecognizeContext_Variable_StartPos.Set(varIndex,spos);
						}
						break;
					}
					//SetVarStartAtPrevToken
				  case kSyntaxDefinitionActionType_SetVarStartAtPrevToken:
					{
						AIndex	endStackIndex = mRecognizeContext_CurrentTokenStackPosition-2;
						if( endStackIndex < 0 )   endStackIndex = 0;
						AIndex	startpos = mRecognizeContext_TokenStack_StartPos.Get(endStackIndex%kTokenStackMaxCount);
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						mRecognizeContext_Variable_StartPos.Set(varIndex,startpos);
						break;
					}
					//SetVarEnd
				  case kSyntaxDefinitionActionType_SetVarEnd:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						mRecognizeContext_Variable_Length.Set(varIndex,spos-mRecognizeContext_Variable_StartPos.Get(varIndex));
						AText	text;
						inText.GetText(mRecognizeContext_Variable_StartPos.Get(varIndex),spos-mRecognizeContext_Variable_StartPos.Get(varIndex),text);
						mRecognizeContext_Variable_ValueArray.Set(varIndex,text);
						break;
					}
					//SetVarLastWord B0306
				  case kSyntaxDefinitionActionType_SetVarLastWord:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						AText	text;
						ATextIndex	startPos = kIndex_Invalid;
						ATextIndex	len = 0;
						for( AIndex stackIndex = mRecognizeContext_CurrentTokenStackPosition-2; stackIndex >= 0; stackIndex-- )
						{
							startPos = mRecognizeContext_TokenStack_StartPos.Get(stackIndex%kTokenStackMaxCount);
							len = mRecognizeContext_TokenStack_Length.Get(stackIndex%kTokenStackMaxCount);
							AUChar	ch = inText.Get(startPos);
							//win 080722 if( Macro_IsAlphabet(ch) == true || ch == '~' )
							if( syntaxDefinition.IsStartAlphabet(ch,syntaxPartIndex) == true )//win 080722
							{
								inText.GetText(startPos,len,text);
								break;
							}
						}
						if( startPos != kIndex_Invalid )
						{
							mRecognizeContext_Variable_StartPos.Set(varIndex,startPos);
							mRecognizeContext_Variable_Length.Set(varIndex,len);
							mRecognizeContext_Variable_ValueArray.Set(varIndex,text);
						}
						break;
					}
					//SetVarCurrentToken R0241
				  case kSyntaxDefinitionActionType_SetVarCurrentToken:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						mRecognizeContext_Variable_StartPos.Set(varIndex,tokenspos);
						mRecognizeContext_Variable_Length.Set(varIndex,pos-tokenspos);
						mRecognizeContext_Variable_ValueArray.Set(varIndex,token);
						break;
					}
					//SetVarNextToken
				  case kSyntaxDefinitionActionType_SetVarNextToken:
					{
						//次のトークンを先読み
						AIndex	nexttokenpos = pos;
						AText	nexttoken;
						AIndex	nexttokenspos = pos;
						if( GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,nexttokenpos,endpos,
														nexttoken,nexttokenspos,syntaxPartIndex) == true )
						{
							AIndex	varIndex = seq.GetVariableIndex(actionIndex);
							mRecognizeContext_Variable_StartPos.Set(varIndex,nexttokenspos);
							mRecognizeContext_Variable_Length.Set(varIndex,nexttokenpos-nexttokenspos);
							mRecognizeContext_Variable_ValueArray.Set(varIndex,nexttoken);
						}
						break;
					}
					//SetVarRegExpGroup1
				  case kSyntaxDefinitionActionType_SetVarRegExpGroup1:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						mRecognizeContext_Variable_StartPos.Set(varIndex,pos-regExpGroup1.GetItemCount());
						mRecognizeContext_Variable_Length.Set(varIndex,regExpGroup1.GetItemCount());
						mRecognizeContext_Variable_ValueArray.Set(varIndex,regExpGroup1);
						break;
					}
					//SetVarEndAtPrevToken
				  case kSyntaxDefinitionActionType_SetVarEndAtPrevToken:
					{
						AIndex	endStackIndex = mRecognizeContext_CurrentTokenStackPosition-3;
						if( endStackIndex < 0 )   endStackIndex = 0;
						AIndex	endpos = mRecognizeContext_TokenStack_StartPos.Get(endStackIndex%kTokenStackMaxCount) 
								+ mRecognizeContext_TokenStack_Length.Get(endStackIndex%kTokenStackMaxCount);
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						mRecognizeContext_Variable_Length.Set(varIndex,endpos-mRecognizeContext_Variable_StartPos.Get(varIndex));
						AText	text;
						AIndex	index = mRecognizeContext_Variable_StartPos.Get(varIndex);
						AItemCount	count = endpos-mRecognizeContext_Variable_StartPos.Get(varIndex);
						if( index >= 0 && index < inText.GetItemCount() && count > 0 && index+count <= inText.GetItemCount() )
						{
							inText.GetText(index,count,text);
						}
						mRecognizeContext_Variable_ValueArray.Set(varIndex,text);
						break;
					}
					//SetVarEndAtNextChar
				  case kSyntaxDefinitionActionType_SetVarEndAtNextChar:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						mRecognizeContext_Variable_Length.Set(varIndex,pos-mRecognizeContext_Variable_StartPos.Get(varIndex));
						AText	text;
						inText.GetText(mRecognizeContext_Variable_StartPos.Get(varIndex),pos-mRecognizeContext_Variable_StartPos.Get(varIndex),text);
						mRecognizeContext_Variable_ValueArray.Set(varIndex,text);
						break;
					}
					//CatVar
				  case kSyntaxDefinitionActionType_CatVar:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						AIndex	var1Index = seq./*#693 GetVariable1Index*/GetParameter1(actionIndex);
						AIndex	var2Index = seq./*#693 GetVariable2Index*/GetParameter2(actionIndex);
						mRecognizeContext_Variable_StartPos.Set(varIndex,mRecognizeContext_Variable_StartPos.Get(var1Index));
						mRecognizeContext_Variable_Length.Set(varIndex,mRecognizeContext_Variable_StartPos.Get(var2Index)+mRecognizeContext_Variable_Length.Get(var2Index)-mRecognizeContext_Variable_StartPos.Get(var1Index));
						AText	text1, text2;
						mRecognizeContext_Variable_ValueArray.Get(var1Index,text1);
						mRecognizeContext_Variable_ValueArray.Get(var2Index,text2);
						AText	text;
						text.SetText(text1);
						text.Add(kUChar_Space);
						text.AddText(text2);
						mRecognizeContext_Variable_ValueArray.Set(varIndex,text);
						break;
					}
					//SetVar RC2
				  case kSyntaxDefinitionActionType_SetVar:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						AIndex	var1Index = seq./*#693 GetVariable1Index*/GetParameter1(actionIndex);
						mRecognizeContext_Variable_StartPos.Set(varIndex,mRecognizeContext_Variable_StartPos.Get(var1Index));
						mRecognizeContext_Variable_Length.Set(varIndex,mRecognizeContext_Variable_Length.Get(var1Index));
						AText	text1;
						mRecognizeContext_Variable_ValueArray.Get(var1Index,text1);
						mRecognizeContext_Variable_ValueArray.Set(varIndex,text1);
						break;
					}
					//変数に固定テキストを設定
				  case kSyntaxDefinitionActionType_SetVarStaticText:
					{
						//変数index取得
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						//固定テキストを取得
						AText	text;
						AIndex	dummy = kIndex_Invalid;
						seq.GetKeyword(actionIndex,dummy,text);
						//変数に設定
						mRecognizeContext_Variable_ValueArray.Set(varIndex,text);
						break;
					}
					//変数に、今の内容を識別子名とする識別子のタイプを設定し直す
				  case kSyntaxDefinitionActionType_SetVarTypeOfVar:
					{
						//変数index取得
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						if( varIndex != kIndex_Invalid )
						{
							//変数の今の内容を取得
							AText	text;
							mRecognizeContext_Variable_ValueArray.Get(varIndex,text);
							//今の内容を識別子名とする識別子のタイプを取得
							//（グローバル識別子、システムヘッダ識別子、キーワードリストから検索）
							AText	typetext;
							if( GetGlobalIdentifierTypeTextByKeywordText(text,typetext) == false )
							{
								if( modePrefDB.GetSystemHeaderIdentifierLinkList().FindKeywordType(text,typetext) == false )
								{
									if( modePrefDB.FindKeywordTypeFromKeywordList(text,typetext) == false )
									{
										//空のテキストを設定する
										typetext.DeleteAll();
									}
								}
							}
							//変数に設定
							mRecognizeContext_Variable_ValueArray.Set(varIndex,typetext);
						}
						break;
					}
					//ClearVar RC2
				  case kSyntaxDefinitionActionType_ClearVar:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						mRecognizeContext_Variable_StartPos.Set(varIndex,spos);
						mRecognizeContext_Variable_Length.Set(varIndex,0);
						mRecognizeContext_Variable_ValueArray.Set(varIndex,GetEmptyText());
						break;
					}
					//
				  case kSyntaxDefinitionActionType_BlockStart:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						//variable_Indent.Set(varIndex,mRecognizeContext_CurrentIndent);
						AddBlockStartData(lineIndex,varIndex,mRecognizeContext_CurrentIndent);
						//コンテキストに該当変数のインデントを設定#695
						mRecognizeContext_Variable_BlockStartIndent.Set(varIndex,mRecognizeContext_CurrentIndent);
						break;
					}
					//
				  case kSyntaxDefinitionActionType_EscapeNextChar:
					{
						pos = inText.GetNextCharPos(pos);//B0318
						break;
					}
					//ClearTokenStack
				  case kSyntaxDefinitionActionType_ClearTokenStack:
					{
						mRecognizeContext_CurrentTokenStackPosition = 0;
						break;
					}
					//ChangeState
				  case kSyntaxDefinitionActionType_ChangeState:
				  case kSyntaxDefinitionActionType_ChangeStateFromNextChar://R0241
					{
						mRecognizeContext_CurrentStateIndex = seq.GetStateIndex(actionIndex);
						//fprintf(stderr,"changestate to %ld  ",mRecognizeContext_CurrentStateIndex);
						stateStart = true;//RC2
						break;
					}
					//
				  case kSyntaxDefinitionActionType_ChangeStateIfBlockEnd:
					{
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						//該当変数の現在のBlockStartインデント（コンテキストに格納されている）以下のインデントレベルなら状態遷移
						if( mRecognizeContext_CurrentIndent <= //#695 FindLastBlockStartIndentCount(lineIndex,varIndex) )
									mRecognizeContext_Variable_BlockStartIndent.Get(varIndex) )//#695
						{
							mRecognizeContext_CurrentStateIndex = seq.GetStateIndex(actionIndex);
							stateStart = true;//RC2
						}
						break;
					}
					//R0241
				  case kSyntaxDefinitionActionType_ChangeStateIfNotAfterAlphabet:
					{
						//前の文字がアルファベットかどうか調べる
						ABool	afterAlphabet = false;
						for( AIndex pos2 = inText.GetPrevCharPos(spos); pos2 > 0; pos2 = inText.GetPrevCharPos(pos2) )
						{
							AUChar	ch = inText.Get(pos2);
							if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
							if( syntaxDefinition.IsStartAlphabet(ch,syntaxPartIndex) == true || 
										syntaxDefinition.IsContinuousAlphabet(ch,syntaxPartIndex) == true )
							{
								afterAlphabet = true;
							}
							break;
						}
						//アルファベットの後でなければ状態遷移
						if( afterAlphabet == false )
						{
							mRecognizeContext_CurrentStateIndex = seq.GetStateIndex(actionIndex);
							stateStart = true;//RC2
						}
						break;
					}
					//R0241
				  case kSyntaxDefinitionActionType_PushStateIfNotAfterAlphabet:
					{
						//前の文字がアルファベットかどうか調べる
						ABool	afterAlphabet = false;
						for( AIndex pos2 = inText.GetPrevCharPos(spos); pos2 > 0; pos2 = inText.GetPrevCharPos(pos2) )
						{
							AUChar	ch = inText.Get(pos2);
							if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
							if( syntaxDefinition.IsStartAlphabet(ch,syntaxPartIndex) == true || 
										syntaxDefinition.IsContinuousAlphabet(ch,syntaxPartIndex) == true )
							{
								afterAlphabet = true;
							}
							break;
						}
						//アルファベットの後でなければ状態遷移
						if( afterAlphabet == false )
						{
							mRecognizeContext_PushedStateIndex = mRecognizeContext_CurrentStateIndex;
							mRecognizeContext_CurrentStateIndex = seq.GetStateIndex(actionIndex);
							stateStart = true;//RC2
						}
						break;
					}
					//R0241
				  case kSyntaxDefinitionActionType_ChangeStateIfTokenEqualVar:
					{
						//現在のtokenと変数が一致なら状態遷移
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						AText	text;
						mRecognizeContext_Variable_ValueArray.Get(varIndex,text);
						if( text.Compare(token) == true )
						{
							mRecognizeContext_CurrentStateIndex = seq.GetStateIndex(actionIndex);
							stateStart = true;//RC2
						}
						break;
					}
					//PushState
				  case kSyntaxDefinitionActionType_PushState:
					{
						mRecognizeContext_PushedStateIndex = mRecognizeContext_CurrentStateIndex;
						mRecognizeContext_CurrentStateIndex = seq.GetStateIndex(actionIndex);
						//fprintf(stderr,"PushState to %ld  ",mRecognizeContext_CurrentStateIndex);
						stateStart = true;//RC2
						break;
					}
					//PushStateFromNextLine
				  case kSyntaxDefinitionActionType_PushStateFromNextLine:
					{
						//次の行開始時にpush state実行する
						reservedNextLinePushState = seq.GetStateIndex(actionIndex);
						break;
					}
					//PopState
					//PopStateFromNextChar
				  case kSyntaxDefinitionActionType_PopState:
				  case kSyntaxDefinitionActionType_PopStateFromNextChar:
					{
						mRecognizeContext_CurrentStateIndex = mRecognizeContext_PushedStateIndex;
						//fprintf(stderr,"popstate to %ld  ",mRecognizeContext_CurrentStateIndex);
						//PopState時にはstart時のシーケンスは実行しないことにする。stateStart = true;//RC2
						break;
					}
				  case kSyntaxDefinitionActionType_IncIndentFromNext:
					{
						mRecognizeContext_CurrentIndent++;
						break;
					}
				  case kSyntaxDefinitionActionType_DecIndentFromCurrent:
					{
						if( mRecognizeContext_CurrentIndent > 0 )//#518
						{
							mRecognizeContext_CurrentIndent--;
						}
						break;
					}
				  case kSyntaxDefinitionActionType_DecIndentFromNext:
					{
						if( mRecognizeContext_CurrentIndent > 0 )//#518
						{
							mRecognizeContext_CurrentIndent--;
						}
						break;
					}
					//Import
				  case kSyntaxDefinitionActionType_Import:
					{
						AText	path;
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						if( varIndex != kIndex_Invalid )
						{
							mRecognizeContext_Variable_ValueArray.Get(varIndex,path);
						}
						if( /*#698 mRecognizeContext_RecognizeStartLineIndexを利用した判定方法に変更 identifierClearedLine.Find(lineIndex) != kIndex_Invalid*/
									lineIndex >= mRecognizeContext_RecognizeStartLineIndex && path.GetItemCount() > 0 )
						{
							AUniqID	UniqID = AddLineGlobalIdentifier(kIdentifierType_ImportFile,
									/*#698 GetLineStart(lineIndex),GetLineStart(lineIndex)*/ lineIndex,0,0,
								kIndex_Invalid,GetEmptyText(),GetEmptyText(),path,//sdfcheck
										GetEmptyText(),GetEmptyText(),GetEmptyText(),false,127);//RC2 #593
							//#695 outAddedGlobalIdentifiers.Add(UniqID);
							//#695 outAddedGlobalIdentifiersLineIndex.Add(lineIndex);
							outImportChanged = true;
						}
						break;
					}
					//SyntaxWarning
				  case kSyntaxDefinitionActionType_SyntaxWarning:
					{
						mLineInfo.SetLineInfoP_SyntaxWarningExist(lineIndex,true);
						break;
					}
					//SyntaxError
				  case kSyntaxDefinitionActionType_SyntaxError:
					{
						mLineInfo.SetLineInfoP_SyntaxErrorExist(lineIndex,true);
						break;
					}
					//AddGlobalId
				  case kSyntaxDefinitionActionType_AddGlobalId:
					{
						//fprintf(stderr," AddGlobalId ");
						AIndex	varIndex;
						AText	keyword;
						ATextIndex	spos = kIndex_Invalid;
						AItemCount	length = 0;
						seq.GetKeyword(actionIndex,varIndex,keyword);
						if( varIndex != kIndex_Invalid )
						{
							mRecognizeContext_Variable_ValueArray.Get(varIndex,keyword);
							spos = mRecognizeContext_Variable_StartPos.Get(varIndex);
							length = mRecognizeContext_Variable_Length.Get(varIndex);
						}
						else
						{
							spos = GetLineStart(lineIndex);
							length = GetLineLengthWithoutLineEnd(lineIndex);
						}
						if( keyword.GetItemCount() > 0 )
						{
							if( seq.CheckKeywordException(actionIndex,keyword) == false )   break;
						}
						AText	menutext;
						seq.GetMenuText(actionIndex,varIndex,menutext);
						if( varIndex != kIndex_Invalid )
						{
							mRecognizeContext_Variable_ValueArray.Get(varIndex,menutext);
						}
						AText	infotext;
						seq.GetInfoText(actionIndex,varIndex,infotext);
						if( varIndex != kIndex_Invalid )
						{
							mRecognizeContext_Variable_ValueArray.Get(varIndex,infotext);
						}
						//RC2
						AText	commenttext;
						seq.GetCommentText(actionIndex,varIndex,commenttext);
						if( varIndex != kIndex_Invalid )
						{
							mRecognizeContext_Variable_ValueArray.Get(varIndex,commenttext);
						}
						AText	parenttext;
						seq.GetParentKeywordText(actionIndex,varIndex,parenttext);
						if( varIndex != kIndex_Invalid )
						{
							mRecognizeContext_Variable_ValueArray.Get(varIndex,parenttext);
						}
						AText	typetext;
						seq.GetTypeText(actionIndex,varIndex,typetext);
						if( varIndex != kIndex_Invalid )
						{
							mRecognizeContext_Variable_ValueArray.Get(varIndex,typetext);
						}
						
						AIndex	categoryIndex = seq.GetCategoryIndex(actionIndex);
						ATextPoint	spt;
						GetTextPointFromTextIndex(spos,spt);
						AIndex	addedLineIndex = spt.y;
						if( /*#698 mRecognizeContext_RecognizeStartLineIndexを利用した判定方法に変更 identifierClearedLine.Find(addedLineIndex) != kIndex_Invalid*/
						   addedLineIndex >= mRecognizeContext_RecognizeStartLineIndex )
						{
							//識別子追加行のインデントを取得
							AIndex	indentCount = indentCountAtLineStart;
							if( addedLineIndex < lineIndex )
							{
								indentCount = mLineInfo.GetLineInfoP_IndentCount(addedLineIndex);
							}
							//global識別子追加
							AUniqID	UniqID =	AddLineGlobalIdentifier(kIdentifierType_GlobalIdentifier,
									/*#698 spos,spos+length*/ addedLineIndex,spt.x,spt.x+length,
									categoryIndex,keyword,menutext,infotext,
									commenttext,parenttext,typetext,(menutext.GetItemCount()>0),
																		seq.GetOutlineLevel(actionIndex) + indentCount*10);//RC2 #593 #875
							if( UniqID != kUniqID_Invalid && menutext.GetItemCount() > 0 )//#695
							{
								outAddedGlobalIdentifiers.Add(UniqID);
								outAddedGlobalIdentifiersLineIndex.Add(addedLineIndex);
								//メニュー登録する場合は、folding levelに見出しフラグ設定
								mLineInfo.SetLineInfoP_FoldingLevel(addedLineIndex,
											mLineInfo.GetLineInfoP_FoldingLevel(addedLineIndex)|kFoldingLevel_Header);
							}
							//keyword.OutputToStderr();
						}
						break;
					}
					//AddLocalId
				  case kSyntaxDefinitionActionType_AddLocalId:
					{
						AIndex	varIndex;
						AText	keyword;
						ATextIndex	spos = kIndex_Invalid;
						AItemCount	length = 0;
						seq.GetKeyword(actionIndex,varIndex,keyword);
						if( varIndex != kIndex_Invalid )
						{
							mRecognizeContext_Variable_ValueArray.Get(varIndex,keyword);
							spos = mRecognizeContext_Variable_StartPos.Get(varIndex);
							length = mRecognizeContext_Variable_Length.Get(varIndex);
						}
						else
						{
							spos = GetLineStart(lineIndex);
							length = GetLineLengthWithoutLineEnd(lineIndex);
						}
						if( seq.CheckKeywordException(actionIndex,keyword) == false )   break;
						AText	infotext;
						seq.GetInfoText(actionIndex,varIndex,infotext);
						if( varIndex != kIndex_Invalid )
						{
							mRecognizeContext_Variable_ValueArray.Get(varIndex,infotext);
						}
						//R0243
						AText	typetext;
						seq.GetTypeText(actionIndex,varIndex,typetext);
						if( varIndex != kIndex_Invalid )
						{
							mRecognizeContext_Variable_ValueArray.Get(varIndex,typetext);
						}
						
						AIndex	categoryIndex = seq.GetCategoryIndex(actionIndex);
						ATextPoint	spt;
						GetTextPointFromTextIndex(spos,spt);
						if( /*#698 mRecognizeContext_RecognizeStartLineIndexを利用した判定方法に変更 identifierClearedLine.Find(spt.y) != kIndex_Invalid*/
									spt.y >= mRecognizeContext_RecognizeStartLineIndex )
						{
							//B0000 AUniqID	UniqID = 
							AddLineLocalIdentifier(kIdentifierType_LocalIdentifier,
										/*#698 spos,spos+length*/ spt.y,spt.x,spt.x+length,categoryIndex,keyword,infotext,typetext);//R0243
							//keyword.OutputToStderr();
						}
						break;
					}
				}
			}
		}
	}
	//#698 outRecognizedStartLineIndex = mRecognizeContext_RecognizeStartLineIndex;
	//#698 outRecognizedEndLineIndex = lineIndex;
	return lineIndex;
}

//#450
/**
コメント／ブロックfoldingレベルを判定して行情報に設定する
*/
void	ATextInfo::UpdateCommentBlockFoldingLevel( const AIndex inLineIndex, const ABoolArray& isCommentStateArray)
{
	//==================コメントfolding判定==================
	//前行の開始位置がコメントかどうか、現在行の開始位置がコメントかどうかを取得
	ABool	prevLineIsComment = isCommentStateArray.Get(mLineInfo.GetLineInfoP_StateIndex(inLineIndex));
	ABool	currentLineIsComment = false;
	if( inLineIndex+1 < GetLineCount() )
	{
		currentLineIsComment = isCommentStateArray.Get(mLineInfo.GetLineInfoP_StateIndex(inLineIndex+1));
	}
	//コメント開始
	AFoldingLevel	prevLineFoldingLevel = mLineInfo.GetLineInfoP_FoldingLevel(inLineIndex);
	if( prevLineIsComment == false && currentLineIsComment == true )
	{
		mLineInfo.SetLineInfoP_FoldingLevel(inLineIndex,prevLineFoldingLevel|kFoldingLevel_CommentStart);
	}
	//コメント終了
	else if( prevLineIsComment == true && currentLineIsComment == false )
	{
		mLineInfo.SetLineInfoP_FoldingLevel(inLineIndex,prevLineFoldingLevel|kFoldingLevel_CommentEnd);
	}
	//==================ブロックfolding判定==================
	//ブロックFolding判定
	if( inLineIndex+1 < GetLineCount() )
	{
		AFoldingLevel	prevLineFoldingLevel = mLineInfo.GetLineInfoP_FoldingLevel(inLineIndex);
		//ブロック開始
		if( mLineInfo.GetLineInfoP_IndentCount(inLineIndex) < mLineInfo.GetLineInfoP_IndentCount(inLineIndex+1) )
		{
			mLineInfo.SetLineInfoP_FoldingLevel(inLineIndex,prevLineFoldingLevel|kFoldingLevel_BlockStart);
		}
		//ブロック終了
		if( mLineInfo.GetLineInfoP_IndentCount(inLineIndex) > mLineInfo.GetLineInfoP_IndentCount(inLineIndex+1) )
		{
			mLineInfo.SetLineInfoP_FoldingLevel(inLineIndex,prevLineFoldingLevel|kFoldingLevel_BlockEnd);
		}
	}
}

//#698
/**
文法認識コンテキスト全削除
*/
void	ATextInfo::ClearRecognizeSyntaxContext()
{
	mRecognizeContext_Variable_ValueArray.DeleteAll();
	mRecognizeContext_Variable_StartPos.DeleteAll();
	mRecognizeContext_Variable_Length.DeleteAll();
	mRecognizeContext_Variable_BlockStartIndent.DeleteAll();
	mRecognizeContext_TokenStack_StartPos.DeleteAll();
	mRecognizeContext_TokenStack_Length.DeleteAll();
}

//#698
/**
最初の行から検索して最初に未認識の行indexを返す
*/
AIndex	ATextInfo::FindFirstUnrecognizedLine() const
{
	return mLineInfo.FindFirstUnrecognizedLine();
}

//#698
/**
文法認識コンテキストを、文法認識スレッドへコピー
（コピー先オブジェクトでコールされる）
*/
void	ATextInfo::CopyRecognizeContext( const ATextInfo& inSrc, const AIndex inOffsetLineIndex )
{
	mRecognizeContext_RecognizeStartLineIndex	= inSrc.mRecognizeContext_RecognizeStartLineIndex - inOffsetLineIndex;
	mRecognizeContext_EndPossibleLineIndex		= inSrc.mRecognizeContext_EndPossibleLineIndex - inOffsetLineIndex;
	mRecognizeContext_CurrentStateIndex			= inSrc.mRecognizeContext_CurrentStateIndex;
	mRecognizeContext_PushedStateIndex			= inSrc.mRecognizeContext_PushedStateIndex;
	mRecognizeContext_CurrentIndent				= inSrc.mRecognizeContext_CurrentIndent;
	mRecognizeContext_CurrentDirectiveLevel		= inSrc.mRecognizeContext_CurrentDirectiveLevel;
	mRecognizeContext_NextDirectiveLevel		= inSrc.mRecognizeContext_NextDirectiveLevel;
	mRecognizeContext_CurrentDisabledDirectiveLevel= inSrc.mRecognizeContext_CurrentDisabledDirectiveLevel;
	mRecognizeContext_NextDisabledDirectiveLevel= inSrc.mRecognizeContext_NextDisabledDirectiveLevel;
	mRecognizeContext_Variable_ValueArray.SetFromTextArray(inSrc.mRecognizeContext_Variable_ValueArray);
	mRecognizeContext_Variable_StartPos.SetFromObject(inSrc.mRecognizeContext_Variable_StartPos);
	mRecognizeContext_Variable_Length.SetFromObject(inSrc.mRecognizeContext_Variable_Length);
	mRecognizeContext_Variable_BlockStartIndent.SetFromObject(inSrc.mRecognizeContext_Variable_BlockStartIndent);
	mRecognizeContext_CurrentTokenStackPosition	= inSrc.mRecognizeContext_CurrentTokenStackPosition;
	mRecognizeContext_TokenStack_StartPos.SetFromObject(inSrc.mRecognizeContext_TokenStack_StartPos);
	mRecognizeContext_TokenStack_Length.SetFromObject(inSrc.mRecognizeContext_TokenStack_Length);
}

//#698
/**
行情報を文法認識スレッドへコピー
（コピー先オブジェクトでコールされる）
*/
void	ATextInfo::CopyLineInfoToSyntaxRecognizer( const ATextInfo& inSrcTextInfo, const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	//識別子情報削除
	{
		//#717
		//グローバル識別子排他制御
		AStMutexLocker	locker(mGlobalIdentifierListMutex);
		//グローバル識別子全削除
		mGlobalIdentifierList.DeleteAllIdentifiers();
	}
	{
		//#717
		//ローカル識別子排他制御
		AStMutexLocker	locker(mLocalIdentifierListMutex);
		//ローカル識別子全削除
		mLocalIdentifierList.DeleteAllIdentifiers();
	}
	//BlockStartData全削除
	mBlockStartDataList.DeleteAll();
	//行情報コピー
	mLineInfo.CopyLineInfoToSyntaxRecognizer(inSrcTextInfo.mLineInfo,inStartLineIndex,inEndLineIndex);
}

//#698
/**
文法認識スレッドの結果を、メインスレッドのTextInfoへコピー
（コピー先オブジェクトでコールされる）
*/
void	ATextInfo::CopyFromSyntaxRecognizerResult( const AIndex inDstStartLineIndex, const AIndex inDstEndLineIndex,
		const ATextInfo& inSrcTextInfo,
		AArray<AUniqID>& outDeletedIdentifiers, 
		AArray<AUniqID>& outAddedGlobalIdentifiers, AArray<AIndex>& outAddedGlobalIdentifiersLineIndex,
		ABool& outImportChanged )
{
	//返り値初期化
	outDeletedIdentifiers.DeleteAll();
	outAddedGlobalIdentifiers.DeleteAll();
	outAddedGlobalIdentifiersLineIndex.DeleteAll();
	outImportChanged = false;
	
	//fprintf(stderr,"CopyFromSyntaxRecognizerResult():start:%ld:end:%ld\n",inDstStartLineIndex,inDstEndLineIndex);
	//コピー先の識別子を削除する
	for( AIndex dstLineIndex = inDstEndLineIndex-1; dstLineIndex >= inDstStartLineIndex; dstLineIndex-- )
	{
		ABool	importChanged = false;
		DeleteLineIdentifiers(dstLineIndex,outDeletedIdentifiers,importChanged);
		if( importChanged == true )
		{
			outImportChanged = true;
		}
	}
	
	//行情報コピー
	mLineInfo.CopyFromSyntaxRecognizerResult(inDstStartLineIndex,inDstEndLineIndex,inSrcTextInfo.mLineInfo);
	
	//グローバル識別子コピー
	{
		AIndex	srcLineIndex = 0;
		for( AIndex dstLineIndex = inDstStartLineIndex; dstLineIndex < inDstEndLineIndex; dstLineIndex++ )
		{
			//コピー元の行の最初の識別子を取得
			AUniqID	srcUniqID = inSrcTextInfo.mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(srcLineIndex);
			AItemCount	loopCount = 0;//無限ループ防止
			while( srcUniqID != kUniqID_Invalid )
			{
				//識別子データ取得
				AIdentifierType	identifierType = inSrcTextInfo.mGlobalIdentifierList.GetIdentifierType(srcUniqID);
				AIndex	xspos = inSrcTextInfo.mGlobalIdentifierList.GetStartIndex(srcUniqID);
				AIndex	xepos = inSrcTextInfo.mGlobalIdentifierList.GetEndIndex(srcUniqID);
				AIndex	categoryIndex = inSrcTextInfo.mGlobalIdentifierList.GetCategoryIndex(srcUniqID);
				AText	keyword;
				inSrcTextInfo.mGlobalIdentifierList.GetKeywordText(srcUniqID,keyword);
				AText	menutext;
				inSrcTextInfo.mGlobalIdentifierList.GetMenuText(srcUniqID,menutext);
				AText	infotext;
				inSrcTextInfo.mGlobalIdentifierList.GetInfoText(srcUniqID,infotext);
				AText	commenttext;
				inSrcTextInfo.mGlobalIdentifierList.GetCommentText(srcUniqID,commenttext);
				AText	parenttext;
				inSrcTextInfo.mGlobalIdentifierList.GetParentKeywordText(srcUniqID,parenttext);
				AText	typetext;
				inSrcTextInfo.mGlobalIdentifierList.GetTypeText(srcUniqID,typetext);
				AIndex	outlineLevel = inSrcTextInfo.mGlobalIdentifierList.GetOutlineLevel(srcUniqID);//#130
				ABool	isLocalDelimiter = inSrcTextInfo.mGlobalIdentifierList.IsLocalRangeDelimiter(srcUniqID);
				//識別子追加
				AUniqID	dstUniqID = AddLineGlobalIdentifier(identifierType,
						dstLineIndex,xspos,xepos,categoryIndex,keyword,menutext,infotext,
						commenttext,parenttext,typetext,isLocalDelimiter,outlineLevel);
				//importならフラグon
				if( identifierType == kIdentifierType_ImportFile )
				{
					outImportChanged = true;
				}
				//メニュー識別子ならoutAddedGlobalIdentifiersに追加
				if( dstUniqID != kUniqID_Invalid && menutext.GetItemCount() > 0 )
				{
					outAddedGlobalIdentifiers.Add(dstUniqID);
					outAddedGlobalIdentifiersLineIndex.Add(dstLineIndex);
				}
				//次の識別子を取得
				srcUniqID = inSrcTextInfo.mGlobalIdentifierList.GetNextUniqID(srcUniqID);
				//無限ループ防止
				loopCount++;
				if( loopCount > 128 )   { _ACError("Id UniqID loop",NULL); break;}
			}
			//次の行
			srcLineIndex++;
		}
	}
	//ローカル識別子コピー
	{
		AIndex	srcLineIndex = 0;
		for( AIndex dstLineIndex = inDstStartLineIndex; dstLineIndex < inDstEndLineIndex; dstLineIndex++ )
		{
			//コピー元の行の最初の識別子を取得
			AUniqID	srcUniqID = inSrcTextInfo.mLineInfo.GetLineInfoP_FirstLocalIdentifierUniqID(srcLineIndex);
			AItemCount	loopCount = 0;//無限ループ防止
			while( srcUniqID != kUniqID_Invalid )
			{
				//識別子データ取得
				AIdentifierType	identifierType = inSrcTextInfo.mLocalIdentifierList.GetIdentifierType(srcUniqID);
				AIndex	xspos = inSrcTextInfo.mLocalIdentifierList.GetStartIndex(srcUniqID);
				AIndex	xepos = inSrcTextInfo.mLocalIdentifierList.GetEndIndex(srcUniqID);
				AIndex	categoryIndex = inSrcTextInfo.mLocalIdentifierList.GetCategoryIndex(srcUniqID);
				AText	keyword;
				inSrcTextInfo.mLocalIdentifierList.GetKeywordText(srcUniqID,keyword);
				AText	infotext;
				inSrcTextInfo.mLocalIdentifierList.GetInfoText(srcUniqID,infotext);
				AText	typetext;
				inSrcTextInfo.mLocalIdentifierList.GetTypeText(srcUniqID,typetext);
				//識別子追加
				AddLineLocalIdentifier(identifierType,
							dstLineIndex,xspos,xepos,categoryIndex,keyword,infotext,typetext);
				//次の識別子を取得
				srcUniqID = inSrcTextInfo.mLocalIdentifierList.GetNextUniqID(srcUniqID);
				//無限ループ防止
				loopCount++;
				if( loopCount > 128 )   { _ACError("Id UniqID loop",NULL); break;}
			}
			//次の行
			srcLineIndex++;
		}
	}
	//BlockStartDataコピー
	{
		AIndex	srcLineIndex = 0;
		for( AIndex dstLineIndex = inDstStartLineIndex; dstLineIndex < inDstEndLineIndex; dstLineIndex++ )
		{
			//コピー元の行の最初のblockstartを取得
			AUniqID	srcUniqID = inSrcTextInfo.mLineInfo.GetLineInfoP_FirstBlockStartDataUniqID(srcLineIndex);
			AItemCount	loopCount = 0;//無限ループ防止
			while( srcUniqID != kUniqID_Invalid )
			{
				//blockstartデータ取得
				AIndex	variableIndex = inSrcTextInfo.mBlockStartDataList.GetVariableIndex(srcUniqID);
				AItemCount	indentCount = inSrcTextInfo.mBlockStartDataList.GetIndentCount(srcUniqID);
				//blockstart追加
				AddBlockStartData(dstLineIndex,variableIndex,indentCount);
				//次のblockstart
				srcUniqID = inSrcTextInfo.mBlockStartDataList.GetNextUniqID(srcUniqID);
				//無限ループ防止
				loopCount++;
				if( loopCount > 128 )   { _ACError("Id UniqID loop",NULL); break;}
			}
			//次の行
			srcLineIndex++;
		}
	}
}

//#723
/**
文法認識用Token取得
@note 文法定義専用token取得。SDFのアルファベット定義に従ってtoken取得。
文法認識時のtokenizeのアルファベット定義がSDFの前提とはずれると文法認識が正しくなくなるので、必ずSDFに定義したアルファベット定義を使うことにする。
*/
ABool	ATextInfo::GetTokenForRecongnizeSyntax( const ASyntaxDefinition& inSyntaxDefinition, AUnicodeData& inUnicodeData,//#664
		const AText& inText, AIndex& ioPos, const AIndex inEndPos, AText& outToken, AIndex& outTokenStartPos,
		const AIndex inSyntaxPartIndex ) const
{
	//#723 アルファベットで構成されるトークンかどうかの返しパラメータ追加
	ABool	isAlphabet = false;
	return GetTokenForRecongnizeSyntax(inSyntaxDefinition,inUnicodeData,
		inText,ioPos,inEndPos,outToken,outTokenStartPos,isAlphabet,inSyntaxPartIndex);
}
//
ABool	ATextInfo::GetTokenForRecongnizeSyntax( const ASyntaxDefinition& inSyntaxDefinition, AUnicodeData& inUnicodeData,//#664
		const AText& inText, AIndex& ioPos, const AIndex inEndPos, 
		AText& outToken, AIndex& outTokenStartPos, ABool& outIsAlphabet, const AIndex inSyntaxPartIndex ) const //#723
{
	//#723
	//tokenがアルファベットで構成されるどうかのフラグ初期化
	outIsAlphabet = false;
	//
	outToken.DeleteAll();
	
	if( inText.SkipTabSpace(ioPos,inEndPos) == false )   return false;
	
	outTokenStartPos = ioPos;
	
	//ポインタ取得
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	const AUChar*	ptr = textptr.GetPtr();
	const AItemCount	textlen = textptr.GetByteCount();
	
	//==================最初の文字の読み込み==================
	AUCS4Char	ucs4char = 0;
	AItemCount	bc = AText::Convert1CharToUCS4(ptr,textlen,ioPos,ucs4char);
	AUChar	ch = inText.Get(ioPos);
	//Unicode 1文字格納
	for( AIndex i = 0; i < bc; i++ )
	{
		if( ioPos >= inText.GetItemCount() )   break;//マルチファイル検索等でバイナリファイルを読んだ場合などの対策
		outToken.Add(inText.Get(ioPos));
		ioPos++;
	}
	//アルファベット判定
	AUnicodeCharCategory	category = inUnicodeData.GetUnicodeCharCategory(ucs4char);
	if( bc == 1 )
	{
		//アスキー内の場合：文法定義により判定→アルファベット以外ならここ（1文字）で終了
		if( inSyntaxDefinition.IsStartAlphabet(ch,inSyntaxPartIndex) == false )   return true;//win 080722
	}
	else
	{
		//アスキー外の場合：Unicodeデータにより判定→アルファベット以外ならここ（1文字）で終了
		//（v2.1ではLatinAndNumberカテゴリを独自に判定していたが、v3ではunicode定義に従う）
		if( inUnicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char) == false )   return true;
	}
	
	//#723
	//ここまで来た場合は、tokenはアルファベットで構成されると判断
	outIsAlphabet = true;
	
	//==================各文字毎のループ==================
	while( ioPos < inEndPos )
	{
		//読み込み
		bc = AText::Convert1CharToUCS4(ptr,textlen,ioPos,ucs4char);
		ch = inText.Get(ioPos);
		category = inUnicodeData.GetUnicodeCharCategory(ucs4char);
		//アルファベット判定
		if( bc == 1 )
		{
			//アスキー内の場合：文法定義により判定→アルファベット以外ならここ（アルファベット以外の文字の前）で終了
			if( inSyntaxDefinition.IsContinuousAlphabet(ch,inSyntaxPartIndex) == false )   return true;//win 080722
		}
		else
		{
			//アスキー外の場合：Unicodeデータにより判定→アルファベット以外ならここ（アルファベット以外の文字の前）で終了
			//（v2.1ではLatinAndNumberカテゴリを独自に判定していたが、v3ではunicode定義に従う）
			if( inUnicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char) == false )   return true;
		}
		//Unicode 1文字格納
		for( AIndex i = 0; i < bc; i++ )
		{
			if( ioPos >= inText.GetItemCount() )   break;//マルチファイル検索等でバイナリファイルを読んだ場合などの対策
			outToken.Add(inText.Get(ioPos));
			ioPos++;
		}
	}
	return true;
}

//#723 ADocument_Textから移動
/**
単語検索
@note 単語がアルファベットかマルチバイトならtrueを返す
*/
ABool	ATextInfo::FindWord( const AText& inText, const AIndex inModeIndex, const ABool inFindForwardOnly, 
		const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd, 
		const ABool inUseAlphabetForKeyword, const AIndex inSyntaxPartIndex ) 
{
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(inModeIndex);
	//#717
	//返り値（単語がアルファベットかマルチバイト）初期化
	ABool	wordIsAlphabetOrMultibyte = true;
	
	//開始位置補正（マルチバイトの途中から開始しないようにする）
	AIndex	startPos = inText.GetCurrentCharPos(inStartPos);
	if( startPos != inStartPos )
	{
		startPos = inText.GetNextCharPos(startPos);
	}
	
	//ポインタ取得
	AStTextPtr	sttextptr(inText,0,inText.GetItemCount());
	AUChar*	textptr = sttextptr.GetPtr();
	AByteCount	textlen = sttextptr.GetByteCount();
	
	//
	ABool	multibyteStart = false;
	ABool	alphabetStart = false;//#723
	AUnicodeCharCategory	category = kUnicodeCharCategory_Others;
	if( startPos == textlen )//B0125
	{
		//------------------開始位置がテキストの最後の場合------------------
		multibyteStart = false;
	}
	else
	{
		//------------------開始位置がテキストの最後ではない場合------------------
		
		//マルチバイト判定
		multibyteStart = inText.IsUTF8MultiByte(startPos);
		
		//B0437
		//Unicodeデータによるアルファベット判定
		//（v2.1ではLatinAndNumberカテゴリを独自に判定していたが、v3ではunicode定義に従う）
		AUCS4Char	ucs4char = 0;
		AText::Convert1CharToUCS4(textptr,textlen,startPos,ucs4char);
		category = unicodeData.GetUnicodeCharCategory(ucs4char);
		//#883 if( category == kUnicodeCharCategory_LatinAndNumber )
		if( unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char) == true )//#883
		{
			alphabetStart = true;
		}
	}
	
	if( multibyteStart == false || alphabetStart == true ) //その他<test
	{
		//==================開始文字が非マルチバイト、または、マルチバイトアルファベットの場合==================
		//（キーワード構成文字を使用する場合でも、非マルチバイトをこちらにしているので、開始文字についてキーワード構成文字を判定できる）
		
		//開始文字がアルファベットかどうかの判定
		
		//R0036 inUseAlphabetForKeywordがtrueのときは、モード設定に従った単語選択を行うよう、各所修正
		ABool	isAlphabet = false;
		if( startPos < textlen )
		{
			//開始文字読み込み
			AUCS4Char	ucs4char = 0;
			AItemCount	bc = AText::Convert1CharToUCS4(textptr,textlen,startPos,ucs4char);
			
			//アルファベット判定
			if( bc == 1 && inUseAlphabetForKeyword == true )
			{
				//------------------モード設定を使う場合------------------
				//非マルチバイト、かつ、キーワード構成文字設定を使用する場合、キーワード構成文字により判定
				isAlphabet = modePrefDB.IsAlphabet(textptr[startPos],inSyntaxPartIndex);
			}
			else
			{
				//------------------Unicodeデータのみを使う場合------------------
				//マルチバイト、または、キーワード構成文字を使用しない場合、Unicodeデータにより判定
				isAlphabet = unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char);
			}
		}
		//==================開始文字がアルファベット以外の場合（＝記号等）==================
		if( isAlphabet == false ) 
		{
			AUChar	ch = 0;
			if( startPos < textlen )
			{
				ch = textptr[startPos];
			}
			if( ch == kUChar_Space || ch == kUChar_Tab ) 
			{
				//前方向検索
				if( inFindForwardOnly == false )
				{
					AIndex	pos = startPos-1;
					for( ; pos >= 0; pos-- ) 
					{
						//
						AUChar	ch = textptr[pos];
						if( ch != kUChar_Space && ch != kUChar_Tab ) 
						{
							break;
						}
						//word文字数制限
						if( startPos - pos > kLimit_WordLength )
						{
							break;
						}
					}
					outStart = pos+1;
				}
				else
				{
					outStart = startPos;
				}
				//後方向検索
				{
					AIndex	pos = startPos+1;
					for( ; pos < textlen; pos++ ) 
					{
						//
						AUChar	ch = textptr[pos];
						if( ch != kUChar_Space && ch != kUChar_Tab ) 
						{
							break;
						}
						//word文字数制限
						if( pos - startPos > kLimit_WordLength )
						{
							break;
						}
					}
					outEnd = pos;
				}
			}
			else 
			{
				outStart = startPos;
				outEnd = startPos+1;
				if( outEnd > textlen )   outEnd = textlen;//B0125
			}
			//#717
			//返り値（単語がアルファベットかマルチバイト）をfalseにする
			wordIsAlphabetOrMultibyte = false;
		}
		//==================開始文字がアルファベットの場合==================
		else 
		{
			//前方向に検索
			if( inFindForwardOnly == false )
			{
				AIndex	pos = startPos;
				for( ; pos > 0; pos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(textptr,textlen,pos) )
				{
					//posの位置の文字読み込み
					AUCS4Char	ucs4char = 0;
					AItemCount	bc = AText::Convert1CharToUCS4(textptr,textlen,pos,ucs4char);
					
					//アルファベット判定
					if( bc == 1 && inUseAlphabetForKeyword == true )
					{
						//------------------モード設定を使う場合------------------
						//非マルチバイト、かつ、キーワード構成文字設定を使用する場合、キーワード構成文字により判定
						if( pos != startPos )
						{
							isAlphabet = ( (modePrefDB.IsAlphabet(textptr[pos],inSyntaxPartIndex)==true) && 
										(modePrefDB.IsTailAlphabet(textptr[pos],inSyntaxPartIndex)==false) );
							if( pos < textlen )
							{
								if( modePrefDB.IsContinuousAlphabet(textptr[pos+1],inSyntaxPartIndex) == false )
								{
									isAlphabet = false;
								}
							}
						}
						else isAlphabet = true;
					}
					else
					{
						//------------------Unicodeデータのみを使う場合------------------
						//マルチバイト、または、キーワード構成文字を使用しない場合、Unicodeデータにより判定
						isAlphabet = unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char);
					}
					
					//アルファベットでなければループ終了
					if( isAlphabet == false )
					{
						pos = AText::GetNextCharPos(textptr,textlen,pos);//B0437
						break;
					}
					//word文字数制限
					if( startPos - pos > kLimit_WordLength )
					{
						break;
					}
				}
				outStart = pos;
			}
			else
			{
				outStart = startPos;
			}
			//後方向に検索
			{
				AIndex	 pos = unicodeData.GetNextCharPosSkipCanonicalDecomp(textptr,textlen,startPos);
				for(; pos < textlen; pos = unicodeData.GetNextCharPosSkipCanonicalDecomp(textptr,textlen,pos) )
				{
					//posの位置の文字読み込み
					AUCS4Char	ucs4char = 0;
					AItemCount	bc = AText::Convert1CharToUCS4(textptr,textlen,pos,ucs4char);
					
					//アルファベット判定
					if( bc == 1 && inUseAlphabetForKeyword == true )
					{
						//------------------モード設定を使う場合------------------
						isAlphabet = ( (modePrefDB.IsContinuousAlphabet(textptr[pos],inSyntaxPartIndex)==true) &&
									(modePrefDB.IsTailAlphabet(textptr[pos-1],inSyntaxPartIndex)==false) );
					}
					else
					{
						//------------------Unicodeデータのみを使う場合------------------
						//マルチバイト、または、キーワード構成文字を使用しない場合、Unicodeデータにより判定
						isAlphabet = unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char);
					}
					
					//アルファベットでなければループ終了
					if( isAlphabet == false )   break;
					//word文字数制限
					if( pos - startPos > kLimit_WordLength )
					{
						break;
					}
				}
				outEnd = pos;
			}
		}
	}
	else 
	{
		//==================開始文字がマルチバイト、かつ、非アルファベットの場合==================
		
		ABool	bunsetsuSelect = GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kBunsetsuSelect);
		AUCS4Char	ucs4char = 0;
		AItemCount	bc = AText::Convert1CharToUCS4(textptr,textlen,startPos,ucs4char);
		//B0360
		AUnicodeCharCategory	firstCharCategory = unicodeData.GetUnicodeCharCategory(ucs4char);
		if( firstCharCategory == kUnicodeCharCategory_Others )
		{
			outStart = startPos;
			outEnd = startPos+bc;
		}
		else
		{
			//前方向検索
			if( inFindForwardOnly == false )
			{
				AIndex	pos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(textptr,textlen,startPos);
				for( ; pos > 0 ; pos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(textptr,textlen,pos) )
				{
					AItemCount	bc = AText::Convert1CharToUCS4(textptr,textlen,pos,ucs4char);
					AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(ucs4char);
					//カテゴリが変化するか、アルファベット文字になった場合、そこまでで単語終了（ただし、カテゴリによっては継続）
					if( firstCharCategory != category || unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char) == true )
					{
						if( (firstCharCategory==kUnicodeCharCategory_ZenkakuHiragana||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) &&
									category == kUnicodeCharCategory_ZenkakuOtohiki )
						{
							//継続
						}
						else if( firstCharCategory == kUnicodeCharCategory_ZenkakuOtohiki && 
									(category==kUnicodeCharCategory_ZenkakuHiragana||category==kUnicodeCharCategory_ZenkakuKatakana) )
						{
							firstCharCategory = category;
						}
						else if( bunsetsuSelect == true && (firstCharCategory == kUnicodeCharCategory_ZenkakuHiragana && 
										(category==kUnicodeCharCategory_Kanji||category==kUnicodeCharCategory_ZenkakuKatakana)) )
						{
							//（漢字またはカタカナ）←ひらがな　の場合
							firstCharCategory = category;
						}
						else
						{
							pos += bc;
							break;
						}
					}
					//word文字数制限
					if( startPos - pos > kLimit_WordLength )
					{
						break;
					}
				}
				if( pos < 0 )   pos = 0;
				outStart = pos;
			}
			else
			{
				outStart = startPos;
			}
			//後方向検索
			{
				AIndex	pos = startPos;
				for( ; pos < textlen; pos = unicodeData.GetNextCharPosSkipCanonicalDecomp(textptr,textlen,pos) )
				{
					bc = AText::Convert1CharToUCS4(textptr,textlen,pos,ucs4char);
					AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(ucs4char);
					//カテゴリが変化するか、アルファベット文字になった場合、そこまでで単語終了（ただし、カテゴリによっては継続）
					if( firstCharCategory != category || unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char) == true )
					{
						if( (firstCharCategory==kUnicodeCharCategory_ZenkakuHiragana||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) &&
									category == kUnicodeCharCategory_ZenkakuOtohiki )
						{
							//継続
						}
						else if( bunsetsuSelect == true && ((firstCharCategory==kUnicodeCharCategory_Kanji||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) && 
										category == kUnicodeCharCategory_ZenkakuHiragana) )
						{
							//（漢字またはカタカナ）→ひらがな　の場合
							firstCharCategory = category;
						}
						else
						{
							break;
						}
					}
					//word文字数制限
					if( pos - startPos > kLimit_WordLength )
					{
						break;
					}
				}
				outEnd = pos;
			}
			//
			if( outStart == outEnd )
			{
				outEnd = unicodeData.GetNextCharPosSkipCanonicalDecomp(textptr,textlen,outStart);
			}
		}
	}
	//結果補正（コール元で無限ループ等発生するのを防止するため）
	if( inStartPos < outStart )
	{
		outStart = inStartPos;
	}
	if( outEnd <= outStart )
	{
		//endはstartの一文字後にする
		outEnd = unicodeData.GetNextCharPosSkipCanonicalDecomp(textptr,textlen,outStart);
	}
	//
	return wordIsAlphabetOrMultibyte;
}

//#1003
/**
行頭がコード内かどうかを取得
*/
ABool	ATextInfoForDocument::GetLineStartIsCode( const AIndex inLineIndex ) const
{
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition();
	AIndex	stateIndex = mLineInfo.GetLineInfoP_StateIndex(inLineIndex);
	return (syntaxDefinition.GetSyntaxDefinitionState(stateIndex).IsNotCodeState()==false);
}

//
void	ATextInfoForDocument::GetIsCodeArray( const AText& inText, const AIndex inLineIndex, AArray<ABool>& outIsCodeArray ) const
{
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition();
	outIsCodeArray.DeleteAll();
	ATextIndex	lineStart = GetLineStart(inLineIndex);
	ATextIndex	endpos = lineStart + GetLineLengthWithLineEnd(inLineIndex);
	AIndex	stateIndex = mLineInfo.GetLineInfoP_StateIndex(inLineIndex);
	AIndex	pushedStateIndex = mLineInfo.GetLineInfoP_PushedStateIndex(inLineIndex);
	for( ATextIndex pos = lineStart; pos < endpos; )
	{
		//pos位置から1トークン解析
		AIndex	nextStateIndex;
		ABool	changeStateFromNextChar;
		ATextIndex	nextPos;
		ABool	seqFound;
		AText	token;
		ATextIndex	tokenspos;
		AItemCount	currentIndent = 0, nextIndent = 0;
		RecognizeStateAndIndent(inText,stateIndex,pushedStateIndex,
				pos,endpos,nextStateIndex,changeStateFromNextChar,nextPos,seqFound,token,tokenspos,0,0,currentIndent,nextIndent);
		
		//
		AIndex	s = stateIndex;
		if( stateIndex != nextStateIndex && changeStateFromNextChar == false )
		{
			s = nextStateIndex;
		}
		ABool	isCode = (syntaxDefinition.GetSyntaxDefinitionState(s).IsNotCodeState()==false);
		for( AIndex i = 0; i < nextPos-pos; i++ )
		{
			outIsCodeArray.Add(isCode);
		}
		
		//状態遷移
		stateIndex = nextStateIndex;
		
		//pos更新
		pos = nextPos;
	}
}

//
void	ATextInfoForDocument::GetIsCommentArray( const AText& inText, const AIndex inLineIndex, AArray<ABool>& outIsCommentArray ) const
{
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition();
	outIsCommentArray.DeleteAll();
	ATextIndex	lineStart = GetLineStart(inLineIndex);
	ATextIndex	endpos = lineStart + GetLineLengthWithLineEnd(inLineIndex);
	AIndex	stateIndex = mLineInfo.GetLineInfoP_StateIndex(inLineIndex);
	AIndex	pushedStateIndex = mLineInfo.GetLineInfoP_PushedStateIndex(inLineIndex);
	for( ATextIndex pos = lineStart; pos < endpos; )
	{
		//pos位置から1トークン解析
		AIndex	nextStateIndex;
		ABool	changeStateFromNextChar;
		ATextIndex	nextPos;
		ABool	seqFound;
		AText	token;
		ATextIndex	tokenspos;
		AItemCount	currentIndent = 0, nextIndent = 0;
		RecognizeStateAndIndent(inText,stateIndex,pushedStateIndex,
				pos,endpos,nextStateIndex,changeStateFromNextChar,nextPos,seqFound,token,tokenspos,0,0,currentIndent,nextIndent);
		
		//
		AIndex	s = stateIndex;
		ABool	isComment = (syntaxDefinition.GetSyntaxDefinitionState(s).IsCommentState()==true);
		for( AIndex i = 0; i < tokenspos-pos; i++ )
		{
			outIsCommentArray.Add(isComment);
		}
		if( stateIndex != nextStateIndex && changeStateFromNextChar == false )
		{
			s = nextStateIndex;
		}
		isComment = (syntaxDefinition.GetSyntaxDefinitionState(s).IsCommentState()==true);
		for( AIndex i = 0; i < nextPos-tokenspos; i++ )
		{
			outIsCommentArray.Add(isComment);
		}
		
		//状態遷移
		stateIndex = nextStateIndex;
		
		//pos更新
		pos = nextPos;
	}
}

/**
指定位置のstate取得
*/
void	ATextInfoForDocument::GetCurrentStateIndexAndName( const AText& inText, const ATextPoint inTextPoint, 
		AIndex& outStateIndex, AIndex& outPushedStateIndex,
		AText& outStateText, AText& outPushedStateText ) const
{
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition();
	outStateText.DeleteAll();
	outPushedStateText.DeleteAll();
	ATextIndex	lineStart = GetLineStart(inTextPoint.y);
	ATextIndex	endpos = lineStart + inTextPoint.x;
	AIndex	stateIndex = mLineInfo.GetLineInfoP_StateIndex(inTextPoint.y);
	AIndex	pushedStateIndex = mLineInfo.GetLineInfoP_PushedStateIndex(inTextPoint.y);
	for( ATextIndex pos = lineStart; pos < endpos; )
	{
		//pos位置から1トークン解析
		AIndex	nextStateIndex;
		ABool	changeStateFromNextChar;
		ATextIndex	nextPos;
		ABool	seqFound;
		AText	token;
		ATextIndex	tokenspos;
		AItemCount	currentIndent = 0, nextIndent = 0;
		RecognizeStateAndIndent(inText,stateIndex,pushedStateIndex,
								pos,endpos,nextStateIndex,changeStateFromNextChar,nextPos,seqFound,token,tokenspos,0,0,currentIndent,nextIndent);
		
		//状態遷移
		stateIndex = nextStateIndex;
		
		//pos更新
		pos = nextPos;
	}
	outStateText.SetText(syntaxDefinition.GetStateName(stateIndex));
	outPushedStateText.SetText(syntaxDefinition.GetStateName(pushedStateIndex));
	outStateIndex = stateIndex;
	outPushedStateIndex = pushedStateIndex;
}

//
ABool	ATextInfo::RecognizeStateAndIndent( const AText& inText, const AIndex inStateIndex, AIndex& ioPushedStateIndex, 
		const ATextIndex inPos, const ATextIndex inEndPos, 
		AIndex& outStateIndex, ABool& outChangeStateFromNextChar, ATextIndex& outPos, ABool& outSeqFound,
		AText& outToken, ATextIndex& outTokenStartPos, 
		const AItemCount inIndentWidth, const AItemCount inLabelIndentWidth, 
		AItemCount& ioCurrentLineIndentCount, AItemCount& ioNextLineIndentCount ) const
{
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition();
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();//#664
	outStateIndex = inStateIndex;
	outPos = inPos;
	outChangeStateFromNextChar = false;
	outSeqFound = false;
	outToken.DeleteAll();
	outTokenStartPos = inPos;
	
	//タブ、スペース読み飛ばし
	if( inText.SkipTabSpace(outPos,inEndPos) == false )   return false;
	outTokenStartPos = outPos;
	
	//改行位置で認識完了（行計算スレッド化により、スレッド計算対象行に改行込みの長いテキストが入るため、改行で打ち切りさせる）#699
	if( inText.Get(outPos) == kUChar_LineEnd )
	{
		outPos = inEndPos;
		return false;
	}
	
	AIndex	seqIndex = kIndex_Invalid;
	//正規表現一致検索
	for( AIndex i = 0; i < mSyntaxDefinitionRegExp.GetItemCount(); i++ )
	{
		if( mSyntaxDefinitionRegExp_State.Get(i) == inStateIndex )
		{
			AIndex	p = outPos;
			if( mSyntaxDefinitionRegExp.GetObject(i).Match(inText,p,inEndPos) == true )
			{
				seqIndex = mSyntaxDefinitionRegExp_Index.Get(i);
				//読み進め
				outPos = p;
				inText.GetText(outTokenStartPos,p-outTokenStartPos,outToken);
				break;
			}
		}
	}
	ASyntaxDefinitionState&	state = syntaxDefinition.GetSyntaxDefinitionState(inStateIndex);
	const AArray<AIndex>&	conditionTextByteCountArrayIndexTable = state.GetConditionTextByteCountArrayIndexTable();//#693
	//#907
	AIndex	syntaxPartIndex = state.GetStateSyntaxPartIndex();
	if( seqIndex == kIndex_Invalid )
	{
		//最初の1バイトを読み、可能性のある条件テキストのバイト数分のテキストについて、条件テキストとの一致検索を試みる
		AUChar	firstch = inText.Get(outPos);
		//byteCountArray: 一致の可能性のある条件テキストのバイト数
		//#693 const AArray<AItemCount>&	byteCountArray = state.GetConditionTextByteCountArray(firstch);
		AIndex	byteCountArrayIndex = conditionTextByteCountArrayIndexTable.Get(firstch);//#693
		if( byteCountArrayIndex != kIndex_Invalid )//#693
		{
			const AArray<AItemCount>&	byteCountArray = state.GetConditionTextByteCountArray(byteCountArrayIndex);//#693
			//
			AItemCount	byteCount = 1;
			for( AIndex i = 0; i < byteCountArray.GetItemCount(); i++ )
			{
				byteCount = byteCountArray.Get(i);
				if( outPos+byteCount > inText.GetItemCount() )   continue;
				//
				if( outPos+byteCount < inText.GetItemCount() )
				{
					//アルファベットで始まる単語で、pos+byteCountの位置（一致部分の後ろの文字）にアルファベットがあれば、一致しないとみなす
					if( syntaxDefinition.IsStartAlphabet(inText.Get(outPos),syntaxPartIndex) == true &&
					   syntaxDefinition.IsContinuousAlphabet(inText.Get(outPos+byteCount),syntaxPartIndex) == true )   continue;
				}
				//条件テキストからアクションシーケンス検索
				seqIndex = state.FindActionSequenceIndex(inText,outPos,byteCount);
				if( seqIndex != kIndex_Invalid )
				{
					outPos += byteCount;
					inText.GetText(outTokenStartPos,byteCount,outToken);
					break;
				}
			}
		}
	}
	//どの条件テキストとも一致しなかった場合、１トークン読み進めた後、DefaultActionを検索する。
	ABool	result = false;
	if( seqIndex == kIndex_Invalid )
	{
		result = GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,outPos,inEndPos,outToken,outTokenStartPos,syntaxPartIndex);//#664
		seqIndex = state.GetDefaultActionSequenceIndex();
	}
	if( seqIndex != kIndex_Invalid )
	{
		//条件テキストと一致した場合
		//アクションシーケンスのアクションを順に実行する
		ASyntaxDefinitionActionSequence&	seq = state.GetActionSequence(seqIndex);
		for( AIndex actionIndex = 0; actionIndex < seq.GetActionItemCount(); actionIndex++ )
		{
			ASyntaxDefinitionActionType	actionType = seq.GetActionType(actionIndex);
			switch(actionType)
			{
			  case kSyntaxDefinitionActionType_ChangeState:
			  case kSyntaxDefinitionActionType_PushState:
				{
					if( actionType == kSyntaxDefinitionActionType_PushState )
					{
						ioPushedStateIndex = inStateIndex;
					}
					outStateIndex = seq.GetStateIndex(actionIndex);
					break;
				}
				//R0241
			  case kSyntaxDefinitionActionType_ChangeStateFromNextChar:
				{
					outStateIndex = seq.GetStateIndex(actionIndex);
					outChangeStateFromNextChar = true;
					break;
				}
				//R0241
			  case kSyntaxDefinitionActionType_ChangeStateIfNotAfterAlphabet:
				{
					//前の文字がアルファベットかどうか調べる
					ABool	afterAlphabet = false;
					for( AIndex pos2 = inText.GetPrevCharPos(inPos); pos2 > 0; pos2 = inText.GetPrevCharPos(pos2) )
					{
						AUChar	ch = inText.Get(pos2);
						if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
						if( syntaxDefinition.IsStartAlphabet(ch,syntaxPartIndex) == true || 
									syntaxDefinition.IsContinuousAlphabet(ch,syntaxPartIndex) == true )
						{
							afterAlphabet = true;
						}
						break;
					}
					//アルファベットの後でなければ状態遷移
					if( afterAlphabet == false )
					{
						outStateIndex = seq.GetStateIndex(actionIndex);
					}
					break;
				}
				//R0241
			  case kSyntaxDefinitionActionType_PushStateIfNotAfterAlphabet:
				{
					//前の文字がアルファベットかどうか調べる
					ABool	afterAlphabet = false;
					for( AIndex pos2 = inText.GetPrevCharPos(inPos); pos2 > 0; pos2 = inText.GetPrevCharPos(pos2) )
					{
						AUChar	ch = inText.Get(pos2);
						if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
						if( syntaxDefinition.IsStartAlphabet(ch,syntaxPartIndex) == true || 
									syntaxDefinition.IsContinuousAlphabet(ch,syntaxPartIndex) == true )
						{
							afterAlphabet = true;
						}
						break;
					}
					//アルファベットの後でなければ状態遷移
					if( afterAlphabet == false )
					{
						ioPushedStateIndex = inStateIndex;
						outStateIndex = seq.GetStateIndex(actionIndex);
					}
					break;
				}
			  case kSyntaxDefinitionActionType_PopState:
				{
					outStateIndex = ioPushedStateIndex;
					break;
				}
			  case kSyntaxDefinitionActionType_PopStateFromNextChar:
				{
					outStateIndex = ioPushedStateIndex;
					outChangeStateFromNextChar = true;
					break;
				}
			  case kSyntaxDefinitionActionType_IncIndentFromNext:
				{
					ioNextLineIndentCount += inIndentWidth;
					break;
				}
			  case kSyntaxDefinitionActionType_DecIndentFromNext:
				{
					ioNextLineIndentCount -= inIndentWidth;
					break;
				}
			  case kSyntaxDefinitionActionType_DecIndentFromCurrent:
				{
					ioNextLineIndentCount -= inIndentWidth;
					//次行の増加分がある間は次行の増加分のみを減らす。
					//次行の増加分がなくなったら、次行のインデント量に、現在行のインデント量を合わせる
					//#1001 $(document).on('pageshow','#MainPage', function(event) {といった行の現在行のインデントが+1になる問題を修正
					//閉じ括弧で常に次行のインデント量に、現在行のインデント量を合わせる処理を行うと、増加済みの次行インデントを現在行に反映してしまうので、
					//「次行の増加分がなくなったら」の条件を追加。
					if( ioNextLineIndentCount <= ioCurrentLineIndentCount )
					{
						ioCurrentLineIndentCount = ioNextLineIndentCount;
					}
					break;
				}
			  case kSyntaxDefinitionActionType_DecIndentOnlyCurrent:
				{
					ioCurrentLineIndentCount -= inIndentWidth;
					break;
				}
			  case kSyntaxDefinitionActionType_SetZeroIndentOnlyCurrent:
				{
					ioCurrentLineIndentCount = 0;
					break;
				}
			  case kSyntaxDefinitionActionType_IndentLabel:
				{
					ioCurrentLineIndentCount += inLabelIndentWidth;
					break;
				}
				//
			  case kSyntaxDefinitionActionType_EscapeNextChar:
				{
					outPos = inText.GetNextCharPos(outPos);//B0318
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
		}
		outSeqFound = true;
		return true;
	}
	
	//１トークン読み
	return result;
}

//#695
/**
全ての行の認識状態を解除し、識別子情報を削除する
*/
void	ATextInfo::ClearSyntaxDataForAllLines()
{
	//識別子情報全削除
	{
		//#717
		//グローバル識別子排他制御
		AStMutexLocker	locker(mGlobalIdentifierListMutex);
		//グローバル識別子全削除
		mGlobalIdentifierList.DeleteAllIdentifiers();
	}
	{
		//#717
		//ローカル識別子排他制御
		AStMutexLocker	locker(mLocalIdentifierListMutex);
		//ローカル識別子全削除
		mLocalIdentifierList.DeleteAllIdentifiers();
	}
	mBlockStartDataList.DeleteAll();
	//全行の識別子uniqID消去、認識フラグoff
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		mLineInfo.SetLineInfo_FirstGlobalIdentifierUniqID(lineIndex,kUniqID_Invalid);
		mLineInfo.SetLineInfoP_FirstLocalIdentifierUniqID(lineIndex,kUniqID_Invalid);
		mLineInfo.SetLineInfoP_FirstBlockStartDataUniqID(lineIndex,kUniqID_Invalid);
		mLineInfo.ClearRecognizeFlags(lineIndex);//#693
		//状態消去
		mLineInfo.SetLineInfoP_StateIndex(lineIndex,0);
		mLineInfo.SetLineInfoP_PushedStateIndex(lineIndex,0);
	}
}

#pragma mark ===========================

#pragma mark ---識別子情報

//指定行のIdentifierを削除する
void	ATextInfo::DeleteLineIdentifiers( const AIndex inLineIndex, AArray<AUniqID>& outDeletedGlobalIdentifiers, ABool& outImportFileIdentifierDeleted )
{
	//#695 outImportFileIdentifierDeleted = false;
	
	//Global
	{
		AUniqID	firstGlobalIdentifierUniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(inLineIndex);
		//削除するIdentifierのUniqIDをoutDeletedIdentifiersに格納
		AUniqID	identifierUniqID = firstGlobalIdentifierUniqID;
		while( identifierUniqID != kUniqID_Invalid )
		{
			outDeletedGlobalIdentifiers.Add(identifierUniqID);
			if( GetGlobalIdentifierType(identifierUniqID) == kIdentifierType_ImportFile )
			{
				outImportFileIdentifierDeleted = true;
			}
			identifierUniqID = GetNextGlobalIdentifier(identifierUniqID);
		}
		//削除実行
		if( firstGlobalIdentifierUniqID != kUniqID_Invalid )
		{
			{
				//#717
				//グローバル識別子排他制御
				AStMutexLocker	locker(mGlobalIdentifierListMutex);
				//グローバル識別子削除
				mGlobalIdentifierList.DeleteIdentifiers(firstGlobalIdentifierUniqID);
			}
			mLineInfo.SetLineInfo_FirstGlobalIdentifierUniqID(inLineIndex,kUniqID_Invalid);
		}
	}
	
	//Local
	if( mLineInfo.IsPurged() == false )//#693
	{
		AUniqID	firstLocalIdentifierUniqID = mLineInfo.GetLineInfoP_FirstLocalIdentifierUniqID(inLineIndex);
		//削除実行
		if( firstLocalIdentifierUniqID != kUniqID_Invalid )
		{
			{
				//#717
				//ローカル識別子排他制御
				AStMutexLocker	locker(mLocalIdentifierListMutex);
				//ローカル識別子削除
				mLocalIdentifierList.DeleteIdentifiers(firstLocalIdentifierUniqID);
			}
			mLineInfo.SetLineInfoP_FirstLocalIdentifierUniqID(inLineIndex,kUniqID_Invalid);
		}
	}
	//BlockStartData
	if( mLineInfo.IsPurged() == false )//#693
	{
		AUniqID	firstUniqID = mLineInfo.GetLineInfoP_FirstBlockStartDataUniqID(inLineIndex);
		//削除実行
		if( firstUniqID != kUniqID_Invalid )
		{
			mBlockStartDataList.DeleteIdentifiers(firstUniqID);
			mLineInfo.SetLineInfoP_FirstBlockStartDataUniqID(inLineIndex,kUniqID_Invalid);
		}
	}
}

//指定行に、Identifierを追加する。
AUniqID	ATextInfo::AddLineGlobalIdentifier( const AIdentifierType inType, 
		/*#698 const ATextIndex inStartIndex, const ATextIndex inEndIndex*/
		const AIndex inLineIndex, const AIndex inXSpos, const AIndex inXEpos, //#698
		const AIndex inCategoryIndex, 
		const AText& inKeywordText, const AText& inMenuText, const AText& inInfoText,
		const AText& inCommentText, const AText& inParentKeywordText, const AText& inTypeText,
		const ABool inLocalRangeDelimiter, const AIndex inOutlineLevel )//RC2 #593 #130
{
	//識別子数制限#695
	if( mGlobalIdentifierList.GetItemCount() >= kLimit_MaxGlobalIdentifiers )   return kUniqID_Invalid;
	
	//IdentifierListに追加
	/*#698
	ATextPoint	spt, ept;
	GetTextPointFromTextIndex(inStartIndex,spt);
	GetTextPointFromTextIndex(inEndIndex,ept);
	*/
	AIndex	lineIndex = inLineIndex;//#698 spt.y;
	AIndex	spos = inXSpos;//#698 spt.x;
	AIndex	epos = inXEpos;//#698ept.x;
	//#698 if( lineIndex != ept.y )   epos = GetLineLengthWithoutLineEnd(lineIndex);
	if( epos > GetLineLengthWithoutLineEnd(lineIndex) )   epos = GetLineLengthWithoutLineEnd(lineIndex);//#698
	//lineIndexの最後のオブジェクトを検索
	AUniqID	last = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex);
	if( last != kUniqID_Invalid )
	{
		AUniqID	next = mGlobalIdentifierList.GetNextUniqID(last);
		while( next != kUniqID_Invalid )
		{
			last = next;
			next = mGlobalIdentifierList.GetNextUniqID(last);
			if( next == last )   {_ACError("object ID list error (loop)",this);break;}//#547
		}
	}
	//menu text取得
	AText	menuText;
	menuText.SetText(inMenuText);
	//menu textが~!anchorのときは、アンカーフラグを設定し、menu textは空にする
	if( menuText.Compare("~!anchor") == true )
	{
		mLineInfo.SetLineInfoP_AnchorIdentifierExist(lineIndex,true);
		menuText.DeleteAll();
	}
	//
	AUniqID	UniqID = kUniqID_Invalid;//#717
	{
		//#717
		//グローバル識別子排他制御
		AStMutexLocker	locker(mGlobalIdentifierListMutex);
		//グローバル識別子追加
		UniqID = mGlobalIdentifierList.AddIdentifier(mLineInfo.GetLineUniqID(lineIndex),//#695
													 last,inType,inCategoryIndex,inKeywordText,
													 menuText,inInfoText,spos,epos,false,
													 inCommentText,inParentKeywordText,inTypeText,
													 GetEmptyText(),GetEmptyText(),
													 inLocalRangeDelimiter,inOutlineLevel,kIndex_Invalid);//RC2 #593 #130
	}
	//previousがkUniqID_Invalidの場合は、行最初の識別子なので、mLineInfo_FirstGlobalIdentifierUniqIDにSet
	if( last == kUniqID_Invalid )
	{
		if( mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex) != kUniqID_Invalid )   _ACThrow("not the first of line identifier",this);
		mLineInfo.SetLineInfo_FirstGlobalIdentifierUniqID(lineIndex,UniqID);
	}
	//#493
	if( menuText.GetItemCount() > 0 && mLineInfo.GetLineInfoP_Multiply(lineIndex) == 100 )
	{
		mLineInfo.SetLineInfoP_Multiply(lineIndex,GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Number(AModePrefDB::kSDFHeaderDisplayMultiply));
	}
	//#693
	if( menuText.GetItemCount() > 0 )
	{
		mLineInfo.SetLineInfoP_MenuIdentifierExist(lineIndex,true);
	}
	//#695
	//ローカルデリミタの場合は、ローカルデリミタフラグをON
	if( inLocalRangeDelimiter == true )
	{
		mLineInfo.SetLineInfoP_LocalDelimiterExist(lineIndex,true);
	}
	return UniqID;
}

//指定行に、Identifierを追加する。
AUniqID	ATextInfo::AddLineLocalIdentifier( const AIdentifierType inType, 
		/*#698 const ATextIndex inStartIndex, const ATextIndex inEndIndex*/
		const AIndex inLineIndex, const AIndex inXSpos, const AIndex inXEpos, //#698
		const AIndex inCategoryIndex, 
			const AText& inKeywordText, const AText& inInfoText, const AText& inTypeText )//R0243
{
	//識別子数制限#695
	if( mLocalIdentifierList.GetItemCount() >= kLimit_MaxLocalIdentifiers )   return kUniqID_Invalid;
	
	//IdentifierListに追加
	/*#698
	ATextPoint	spt, ept;
	GetTextPointFromTextIndex(inStartIndex,spt);
	GetTextPointFromTextIndex(inEndIndex,ept);
	*/
	AIndex	lineIndex = inLineIndex;//#698 spt.y;
	AIndex	spos = inXSpos;//#698 spt.x;
	AIndex	epos = inXEpos;//#698ept.x;
	//#698 if( lineIndex != ept.y )   epos = GetLineLengthWithoutLineEnd(lineIndex);
	if( epos > GetLineLengthWithoutLineEnd(lineIndex) )   epos = GetLineLengthWithoutLineEnd(lineIndex);//#698
	//lineIndexの最後のオブジェクトを検索
	AUniqID	last = mLineInfo.GetLineInfoP_FirstLocalIdentifierUniqID(lineIndex);
	if( last != kUniqID_Invalid )
	{
		AUniqID	next = mLocalIdentifierList.GetNextUniqID(last);
		while( next != kUniqID_Invalid )
		{
			last = next;
			next = mLocalIdentifierList.GetNextUniqID(last);
			if( next == last )   {_ACError("object ID list error (loop)",this);break;}//#547
		}
	}
	AUniqID	UniqID = kUniqID_Invalid;//#717
	{
		//ローカル識別子排他制御
		AStMutexLocker	locker(mLocalIdentifierListMutex);
		//
		UniqID = mLocalIdentifierList.AddIdentifier(mLineInfo.GetLineUniqID(lineIndex),//#695
													last,inType,inCategoryIndex,inKeywordText,
													GetEmptyText(),inInfoText,spos,epos,false,
													GetEmptyText(),GetEmptyText(),inTypeText,
													GetEmptyText(),GetEmptyText(),
													false,0,kIndex_Invalid);//RC2, R0243 #593
	}
	//previousがkUniqID_Invalidの場合は、行最初の識別子なので、mLineInfoP_FirstLocalIdentifierUniqIDにSet
	if( last == kUniqID_Invalid )
	{
		if( mLineInfo.GetLineInfoP_FirstLocalIdentifierUniqID(lineIndex) != kUniqID_Invalid )   _ACThrow("not the first of line identifier",this);
		mLineInfo.SetLineInfoP_FirstLocalIdentifierUniqID(lineIndex,UniqID);
	}
	return UniqID;
}

/**
指定識別子が存在する行のIndexを返す
*/
AIndex	ATextInfo::GetLineIndexByGlobalIdentifier( const AUniqID inIdentifierUniqID ) const
{
	//IdentifierListに識別子ごとに格納しているlineUniqIDを取得
	AUniqID	lineUniqID = mGlobalIdentifierList.GetLineUniqID(inIdentifierUniqID);
	//lineUniqIDからlineIndexを取得
	return mLineInfo.FindLineIndexFromLineUniqID(lineUniqID);
	/*#695
	for( AIndex index = 0; index < GetLineCount(); index++ )
	{
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(index);
		while( UniqID != kUniqID_Invalid )
		{
			if( UniqID == inIdentifierUniqID )   return index;
			AUniqID	nextUniqID = mGlobalIdentifierList.GetNextUniqID(UniqID);//#547
			if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			UniqID = nextUniqID;//#547
		}
	}
	return kIndex_Invalid;
	*/
}

//#695
/**
指定識別子が存在する行のParagraph Indexを返す
*/
AIndex	ATextInfo::GetParagraphIndexByGlobalIdentifier( const AUniqID inIdentifierUniqID ) const
{
	//IdentifierListに識別子ごとに格納しているlineUniqIDを取得
	AUniqID	lineUniqID = mGlobalIdentifierList.GetLineUniqID(inIdentifierUniqID);
	//lineUniqIDからparagraph indexを取得
	return mLineInfo.GetParagraphIndexFromLineUniqID(lineUniqID);
}


/**
行が属するメニュー識別子を取得
*/
AUniqID	ATextInfo::GetMenuIdentifierByLineIndex( const AIndex inLineIndex ) const
{
	return mLineInfo.FindCurrentMenuIdentifier(inLineIndex);
	/*#695
	for( AIndex lineIndex = inLineIndex; lineIndex >= 0; lineIndex-- )
	{
		//B0000
		if( lineIndex >= GetLineCount() )//#695 mLineInfo_FirstGlobalIdentifierUniqID.GetItemCount() )
		{
			continue;
		}
		
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex);
		while( UniqID != kUniqID_Invalid )
		{
			if( IsMenuIdentifier(UniqID) == true )   return UniqID;
			AUniqID	nextUniqID = mGlobalIdentifierList.GetNextUniqID(UniqID);//#547
			if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			UniqID = nextUniqID;//#547
		}
	}
	return kUniqID_Invalid;
	*/
}

/*
//RC3
//inLineIndexから前に遡って最初に見つかった、メニュー登録される識別子のUniqIDと行番号を取得
AUniqID	ATextInfo::GetMenuIdentifierByLineIndex( const AIndex inLineIndex, AIndex& outIdnetifierLineIndex ) const
{
	outIdnetifierLineIndex = kIndex_Invalid;
	for( AIndex lineIndex = inLineIndex; lineIndex >= 0; lineIndex-- )
	{
		//B0000
		if( lineIndex >= GetLineCount() )//#695 mLineInfo_FirstGlobalIdentifierUniqID.GetItemCount() )
		{
			continue;
		}
		
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex);
		while( UniqID != kUniqID_Invalid )
		{
			if( IsMenuIdentifier(UniqID) == true )
			{
				outIdnetifierLineIndex = lineIndex;
				return UniqID;
			}
			AUniqID	nextUniqID = mGlobalIdentifierList.GetNextUniqID(UniqID);
			if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			UniqID = nextUniqID;//#547
		}
	}
	return kUniqID_Invalid;
}
*/
//#138
//メニュー識別子のメニューテキストから、それを定義している行の行番号を取得する
AIndex	ATextInfo::FindLineIndexByMenuText( const AText& inMenuName ) const
{
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		//各行に設定されたグローバル識別子片方向リストからMenuIdentifierを検索する
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex);
		while( UniqID != kUniqID_Invalid )
		{
			if( IsMenuIdentifier(UniqID) == true )
			{
				//メニューテキストを比較し、一致ならその行番号を返す
				AText	text;
				GetGlobalIdentifierMenuText(UniqID,text);
				if( text.Compare(inMenuName) == true )
				{
					return lineIndex;
				}
			}
			AUniqID	nextUniqID = mGlobalIdentifierList.GetNextUniqID(UniqID);//#547
			if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			UniqID = nextUniqID;//#547
		}
	}
	return kIndex_Invalid;
}

//#138
//メニュー識別子のキーワードテキストから、それを定義している行の行番号を取得する
AIndex	ATextInfo::FindLineIndexByMenuKeywordText( const AText& inMenuName ) const
{
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		//各行に設定されたグローバル識別子片方向リストからMenuIdentifierを検索する
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex);
		while( UniqID != kUniqID_Invalid )
		{
			if( IsMenuIdentifier(UniqID) == true )
			{
				//メニューのキーワードテキストを比較し、一致ならその行番号を返す
				AText	text;
				GetGlobalIdentifierKeywordText(UniqID,text);
				if( text.Compare(inMenuName) == true )
				{
					return lineIndex;
				}
			}
			AUniqID	nextUniqID = mGlobalIdentifierList.GetNextUniqID(UniqID);//#547
			if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			UniqID = nextUniqID;//#547
		}
	}
	return kIndex_Invalid;
}

//
void	ATextInfo::GetLocalRangeByLineIndex( const AIndex inLineIndex, AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const
{
	mLineInfo.FindLocalRange(inLineIndex,outStartLineIndex,outEndLineIndex);
	/*#695
	AItemCount	lineCount = GetLineCount();
	outStartLineIndex = 0;
	outEndLineIndex = lineCount;
	//
	for( AIndex lineIndex = inLineIndex; lineIndex >= 0; lineIndex-- )
	{
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex);
		ABool	found = false;
		while( UniqID != kUniqID_Invalid )
		{
			//#593 if( IsMenuIdentifier(UniqID) == true )
			if( mGlobalIdentifierList.IsLocalRangeDelimiter(UniqID) == true )//#593
			{
				found = true;
				break;
			}
			AUniqID	nextUniqID = mGlobalIdentifierList.GetNextUniqID(UniqID);//#547
			if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			UniqID = nextUniqID;//#547
		}
		if( found == true )
		{
			outStartLineIndex = lineIndex;
			break;
		}
	}
	//
	for( AIndex lineIndex = inLineIndex+1; lineIndex < lineCount; lineIndex++ )
	{
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex);
		ABool	found = false;
		while( UniqID != kUniqID_Invalid )
		{
			//#593 if( IsMenuIdentifier(UniqID) == true )
			if( mGlobalIdentifierList.IsLocalRangeDelimiter(UniqID) == true )//#593
			{
				found = true;
				break;
			}
			AUniqID	nextUniqID = mGlobalIdentifierList.GetNextUniqID(UniqID);//#547
			if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			UniqID = nextUniqID;//#547
		}
		if( found == true )
		{
			outEndLineIndex = lineIndex;
			break;
		}
	}
	*/
}

//#717
/**
行が属するLocal範囲の開始識別子を取得
*/
AUniqID	ATextInfo::GetLocalStartIdentifierByLineIndex( const AIndex inLineIndex ) const
{
	return mLineInfo.FindCurrentLocalStartIdentifier(inLineIndex);
}

//#717
/**
行が属するLocal範囲の開始識別子の親キーワードを取得（典型的には現在の関数のクラス）
*/
void	ATextInfo::GetCurrentLocalStartIdentifierParentKeyword( const AIndex inLineIndex ,AText& outParentKeyword ) const
{
	AUniqID	uniqID = GetLocalStartIdentifierByLineIndex(inLineIndex);
	if( uniqID !=kUniqID_Invalid )
	{
		mGlobalIdentifierList.GetParentKeywordText(uniqID,outParentKeyword);
	}
}

//
void	ATextInfoForDocument::GetLocalRangeIdentifier( const AIndex inStartLineIndex, const AIndex inEndLineIndex, 
		AObjectArrayItem* inParent, AHashObjectArray<ALocalIdentifierData,AText>& outArray ) const
{
	outArray.DeleteAll();
	//★速度改善検討 #695
	for( AIndex lineIndex = inStartLineIndex; lineIndex < inEndLineIndex; lineIndex++ )
	{
		AUniqID	UniqID = mLineInfo.GetLineInfoP_FirstLocalIdentifierUniqID(lineIndex);
		while( UniqID != kUniqID_Invalid )
		{
			//#348
			AText	keywordText;
			mLocalIdentifierList.GetKeywordText(UniqID,keywordText);
			AText	infoText;
			mLocalIdentifierList.GetInfoText(UniqID,infoText);
			AText	typeText;
			mLocalIdentifierList.GetTypeText(UniqID,typeText);
			//
			ALocalIdentifierDataFactory factory(inParent,
					//#348 mLocalIdentifierList.GetKeywordText(UniqID),
					keywordText,//#348
					mLocalIdentifierList.GetCategoryIndex(UniqID),
					//#348 mLocalIdentifierList.GetInfoText(UniqID),//RC2
					infoText,//#348
					mLocalIdentifierList.GetStartIndex(UniqID)+GetLineStart(lineIndex),//RC1
					mLocalIdentifierList.GetEndIndex(UniqID)+GetLineStart(lineIndex),//RC1
					//#348 mLocalIdentifierList.GetTypeText(UniqID));
					typeText);//#348
			outArray.AddNewObject(factory);
			AUniqID	nextUniqID = mLocalIdentifierList.GetNextUniqID(UniqID);//#547
			if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			UniqID = nextUniqID;//#547
		}
	}
}

/**
行が属するAnchor識別子を取得
*/
AUniqID	ATextInfo::GetAnchorIdentifierByLineIndex( const AIndex inLineIndex ) const
{
	return mLineInfo.FindAnchorIdentifier(inLineIndex);
}

#pragma mark ===========================

#pragma mark ---グローバル識別子情報取得

/**
Global識別子の定義位置（範囲）取得
@note 速度O(行数)
*/
void	ATextInfo::GetGlobalIdentifierRange( const AUniqID inIdentifierUniqID, ATextPoint& outStart, ATextPoint& outEnd ) const
{
	outStart.y = outEnd.y = GetLineIndexByGlobalIdentifier(inIdentifierUniqID);
	outStart.x = mGlobalIdentifierList.GetStartIndex(inIdentifierUniqID);
	outEnd.x = mGlobalIdentifierList.GetEndIndex(inIdentifierUniqID);
}

//キーワード文字列からグローバル識別子のUniqIDを取得
AUniqID	ATextInfo::GetGlobalIdentifierByKeywordText( const AText& inKeywordText ) const
{
	//R0000 カテゴリー可否 ★ 2番目の引数がtrueなのでtmpArrayは使われない
	ABoolArray	tmpArray;
	return mGlobalIdentifierList.GetIdentifierByKeywordText(inKeywordText,true,tmpArray);
}

//キーワード文字列からグローバル識別子のUniqIDのリストを取得 RC2
void	ATextInfo::GetGlobalIdentifierListByKeywordText( const AText& inKeywordText, AArray<AUniqID>& outUniqIDArray ) const
{
	return mGlobalIdentifierList.GetIdentifierListByKeyword(inKeywordText,outUniqIDArray);
}

AIndex	ATextInfo::GetGlobalIdentifierCategoryIndexByKeywordText( const AText& inKeywordText ) const
{
	//R0000 カテゴリー可否 ★
	ABoolArray	tmpArray;
	AIndex	colorSlot = kIndex_Invalid;
	return mGlobalIdentifierList.GetCategoryIndexByKeywordText(inKeywordText,true,tmpArray,colorSlot);
}

//R0243
ABool	ATextInfo::GetGlobalIdentifierTypeTextByKeywordText( const AText& inKeywordText, AText& outTypeText ) const
{
	ABoolArray	tmpArray;
	return mGlobalIdentifierList.GetTypeTextByKeywordText(inKeywordText,true,tmpArray,outTypeText);
}

ABool	ATextInfo::GetGlobalIdentifierKeywordText( const AUniqID inIdentifierID, AText& outText ) const
{
	return mGlobalIdentifierList.GetKeywordText(inIdentifierID,outText);
}

void	ATextInfo::GetGlobalIdentifierMenuText( const AUniqID inIdentifierID, AText& outText ) const
{
	mGlobalIdentifierList.GetMenuText(inIdentifierID,outText);
}

void	ATextInfo::GetGlobalIdentifierInfoText( const AUniqID inIdentifierID, AText& outText ) const
{
	mGlobalIdentifierList.GetInfoText(inIdentifierID,outText);
}

//RC2
void	ATextInfo::GetGlobalIdentifierCommentText( const AUniqID inIdentifierID, AText& outText ) const
{
	mGlobalIdentifierList.GetCommentText(inIdentifierID,outText);
}

//RC2
void	ATextInfo::GetGlobalIdentifierParentKeywordText( const AUniqID inIdentifierID, AText& outText ) const
{
	mGlobalIdentifierList.GetParentKeywordText(inIdentifierID,outText);
}

AIndex	ATextInfo::GetGlobalIdentifierCategoryIndex( const AUniqID inIdentifierID ) const
{
	return mGlobalIdentifierList.GetCategoryIndex(inIdentifierID);
}

//#130
/**
アウトラインレベル取得
*/
AIndex	ATextInfo::GetGlobalIdentifierOutlineLevel( const AUniqID inIdentifierID ) const
{
	return mGlobalIdentifierList.GetOutlineLevel(inIdentifierID);
}

ABool	ATextInfo::IsMenuIdentifier( const AUniqID inIdentifierID ) const
{
	return mGlobalIdentifierList.IsMenuIdentifier(inIdentifierID);
}

AUniqID	ATextInfo::GetNextGlobalIdentifier( const AUniqID inIdentifierID ) const
{
	return mGlobalIdentifierList.GetNextUniqID(inIdentifierID);
}

AIdentifierType	ATextInfo::GetGlobalIdentifierType( const AUniqID inIdentifierID ) const
{
	return mGlobalIdentifierList.GetIdentifierType(inIdentifierID);
}

/*#717
//省略入力候補取得（データは引数の配列へ追加される）（グローバル）
void	ATextInfo::GetAbbrevCandidateGlobal( const AText& inText,
		ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
		AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray ) const
{
	//#717
	AStMutexLocker	locker(mGlobalIdentifierListMutex);
	//
	mGlobalIdentifierList.GetAbbrevCandidate(inText,outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray);
	for( AIndex i = outScopeArray.GetItemCount(); i < outAbbrevCandidateArray.GetItemCount(); i++ )
	{
		outScopeArray.Add(kScopeType_Global);
	}
}

//省略入力候補取得（データは引数の配列へ追加される）（グローバル）R0243
void	ATextInfo::GetAbbrevCandidateGlobalByParentKeyword( const AText& inText,
		ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
		AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray ) const
{
	mGlobalIdentifierList.GetAbbrevCandidateByParentKeyword(inText,outAbbrevCandidateArray,outInfoTextArray,outCategoryIndexArray);
	for( AIndex i = outScopeArray.GetItemCount(); i < outAbbrevCandidateArray.GetItemCount(); i++ )
	{
		outScopeArray.Add(kScopeType_Global);
	}
}
*/
//#853
/**
各種キーワード検索スレッド用統合キーワード検索処理(Global識別子)
@note ハッシュを使わないので低速なため、スレッド使用が必須だが、いろいろな検索方法ができる。
*/
ABool	ATextInfo::SearchKeywordGlobal( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
									   const AIndex inCurrentStateIndex,
									   AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
									   ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
									   ATextArray& outCommentTextArray,
									   ATextArray& outCompletionTextArray, ATextArray& outURLArray,
									   AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
									   AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
									   AArray<AScopeType>& outScopeArray,
									   AArray<AItemCount>& outMatchedCountArray,
									   const ABool& inAbort ) const
{
	//グローバル識別子排他制御
	AStMutexLocker	locker(mGlobalIdentifierListMutex);
	//検索実行前の項目個数を記憶
	AIndex	startIndex = outKeywordArray.GetItemCount();
	//グローバル識別子からキーワード検索
	AHashArray<AUniqID>	limitLineUniqIDArray;
	AArray<AUniqID>	lineUniqIDArray;
	ABool	result = mGlobalIdentifierList.SearchKeyword(inWord,inParentWord,inOption,
														 mModeIndex,inCurrentStateIndex,
														 limitLineUniqIDArray,
														 outKeywordArray,outParentKeywordArray,
														 outTypeTextArray,outInfoTextArray,outCommentTextArray,
														 outCompletionTextArray,outURLArray,
														 outCategoryIndexArray,outColorSlotIndexArray,
														 outStartIndexArray,outEndIndexArray,outMatchedCountArray,lineUniqIDArray,
														 inAbort);
	//今回検索で追加した分についてspos, epos, scopeを設定
	for( AIndex i = startIndex; i < outKeywordArray.GetItemCount(); i++ )
	{
		//AIdentifierList::SearchKeyword()から取得したstartIndex/endIndexは行内のx位置なので、テキストindexに変換する。
		AIndex	lineStart = mLineInfo.GetLineInfo_Start_FromLineUniqID(lineUniqIDArray.Get(i-startIndex));
		outStartIndexArray.Set(i,outStartIndexArray.Get(i)+lineStart);
		outEndIndexArray.Set(i,outEndIndexArray.Get(i)+lineStart);
		//scope設定
		outScopeArray.Add(kScopeType_Global);
	}
	return result;
}

//#853
/**
各種キーワード検索スレッド用統合キーワード検索処理(Local識別子)
@note ハッシュを使わないので低速なため、スレッド使用が必須だが、いろいろな検索方法ができる。
*/
ABool	ATextInfo::SearchKeywordLocal( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
									  const AIndex inCurrentStateIndex,
									  const AIndex inStartLineIndex, const AIndex inEndLineIndex,
									  AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
									  ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
									  ATextArray& outCommentTextArray,
									  ATextArray& outCompletionTextArray, ATextArray& outURLArray,
									  AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
									  AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
									  AArray<AScopeType>& outScopeArray,
									  AArray<AItemCount>& outMatchedCountArray,
									  const ABool& inAbort ) const
{
	//ローカル識別子排他制御
	AStMutexLocker	locker(mLocalIdentifierListMutex);
	//検索実行前の項目個数を記憶
	AIndex	startIndex = outKeywordArray.GetItemCount();
	//ローカル範囲内のlineUniqIDをlimitLineUniqIDArrayに格納（検索対象行を限定）
	AHashArray<AUniqID>	limitLineUniqIDArray;
	for( AIndex lineIndex = inStartLineIndex; lineIndex < inEndLineIndex; lineIndex++ )
	{
		limitLineUniqIDArray.Add(mLineInfo.GetLineUniqID(lineIndex));
	}
	//ローカル識別子から検索
	AArray<AUniqID>	lineUniqIDArray;
	ABool	result = mLocalIdentifierList.SearchKeyword(inWord,inParentWord,inOption,
														mModeIndex,inCurrentStateIndex,
														limitLineUniqIDArray,
														outKeywordArray,outParentKeywordArray,
														outTypeTextArray,outInfoTextArray,outCommentTextArray,
														outCompletionTextArray,outURLArray,
														outCategoryIndexArray,outColorSlotIndexArray,
														outStartIndexArray,outEndIndexArray,outMatchedCountArray,lineUniqIDArray,
														inAbort);
	//今回検索で追加した分についてspos, epos, scopeを設定
	for( AIndex i = startIndex; i < outKeywordArray.GetItemCount(); i++ )
	{
		//AIdentifierList::SearchKeyword()から取得したstartIndex/endIndexは行内のx位置なので、テキストindexに変換する。
		AIndex	lineStart = mLineInfo.GetLineInfo_Start_FromLineUniqID(lineUniqIDArray.Get(i-startIndex));
		outStartIndexArray.Set(i,outStartIndexArray.Get(i)+lineStart);
		outEndIndexArray.Set(i,outEndIndexArray.Get(i)+lineStart);
		//scope設定
		outScopeArray.Add(kScopeType_Local);
	}
	return result;
}

//
void	ATextInfo::GetGlobalIdentifierListByKeyword( const AText& inText, AArray<AUniqID>& outIdentifierIDArray ) const
{
	mGlobalIdentifierList.GetIdentifierListByKeyword(inText,outIdentifierIDArray);
}

AItemCount	ATextInfo::GetGlobalIdentifierListCount() const
{
	return mGlobalIdentifierList.GetItemCount();
}

/*#348
//
const AText&	ATextInfo::GetKeywordTextByGlobalIdentifierIndex( const AIndex inIndex ) const
{
	return mGlobalIdentifierList.GetKeywordTextByIdentifierIndex(inIndex);
}
*/
void	ATextInfo::GetKeywordTextByGlobalIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mGlobalIdentifierList.GetKeywordTextByIdentifierIndex(inIndex,outText);
}

//#348
ABool	ATextInfo::CompareKeywordTextByGlobalIdentifierIndex( const AIndex inIndex, const AText& inText ) const
{
	return mGlobalIdentifierList.CompareKeywordTextByIdentifierIndex(inIndex,inText);
}

//
AIndex	ATextInfo::GetCategoryIndexByGlobalIdentifierIndex( const AIndex inIndex ) const
{
	return mGlobalIdentifierList.GetCategoryIndexByIdentifierIndex(inIndex);
}

/**
グローバル識別子のcolor slotを取得
*/
AIndex	ATextInfo::GetColorSlotIndexByGlobalIdentifierIndex( const AIndex inIndex ) const
{
	return mGlobalIdentifierList.GetColorSlotIndexByIdentifierIndex(inIndex);
}

/*#348
//
const AText&	ATextInfo::GetInfoTextByGlobalIdentifierIndex( const AIndex inIndex ) const
{
	return mGlobalIdentifierList.GetInfoTextByIdentifierIndex(inIndex);
}
*/
void	ATextInfo::GetInfoTextByGlobalIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mGlobalIdentifierList.GetInfoTextByIdentifierIndex(inIndex,outText);
}

AUniqID	ATextInfo::GetUniqIDByGlobalIdentifierIndex( const AIndex inIndex ) const
{
	return mGlobalIdentifierList.GetUniqIDByIdentifierIndex(inIndex);
}

/*#348
//RC2
const AText&	ATextInfo::GetCommentTextByGlobalIdentifierIndex( const AIndex inIndex ) const
{
	return mGlobalIdentifierList.GetCommentTextByIdentifierIndex(inIndex);
}
*/
void	ATextInfo::GetCommentTextByGlobalIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mGlobalIdentifierList.GetCommentTextByIdentifierIndex(inIndex,outText);
}

/*#348
//RC2
const AText&	ATextInfo::GetParentKeywordTextByGlobalIdentifierIndex( const AIndex inIndex ) const
{
	return mGlobalIdentifierList.GetParentKeywordTextByIdentifierIndex(inIndex);
}
*/
void	ATextInfo::GetParentKeywordTextByGlobalIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mGlobalIdentifierList.GetParentKeywordTextByIdentifierIndex(inIndex,outText);
}

/*#348
//R0243
const AText&	ATextInfo::GetTypeTextByGlobalIdentifierIndex( const AIndex inIndex ) const
{
	return mGlobalIdentifierList.GetTypeTextByIdentifierIndex(inIndex);
}
*/
void	ATextInfo::GetTypeTextByGlobalIdentifierIndex( const AIndex inIndex, AText& outText ) const
{
	mGlobalIdentifierList.GetTypeTextByIdentifierIndex(inIndex,outText);
}

//
/* void	ATextInfo::SetGlobalIdentifierStartEndIndex( const AUniqID inUniqID, const AIndex inStartIndex, const Aindex inEndIndex )
{
	mGlobalIdentifierList.SetStartEndIndex(inStartIndex,inEndIndex);
}*/




#pragma mark ===========================

#pragma mark ---ローカル識別子情報取得

//キーワード文字列からローカル識別子のUniqIDを取得
AUniqID	ATextInfo::GetLocalIdentifierByKeywordText( const AText& inKeywordText ) const
{
	//R0000 カテゴリー可否 ★
	ABoolArray	tmpArray;
	return mLocalIdentifierList.GetIdentifierByKeywordText(inKeywordText,true,tmpArray);
}

//キーワード文字列からローカル識別子のUniqIDのリストを取得
void	ATextInfo::GetLocalIdentifierListByKeywordText( const AText& inKeywordText, AArray<AUniqID>& outUniqIDArray ) const
{
	mLocalIdentifierList.GetIdentifierListByKeyword(inKeywordText,outUniqIDArray);
}

//
void	ATextInfo::GetLocalIdentifierInfoText( const AUniqID inIdentifierID, AText& outText ) const
{
	mLocalIdentifierList.GetInfoText(inIdentifierID,outText);
}

AIndex	ATextInfo::GetLocalIdentifierCategoryIndex( const AUniqID inIdentifierID ) const
{
	return mLocalIdentifierList.GetCategoryIndex(inIdentifierID);
}

/*#348
//R0243
const AText&	ATextInfo::GetLocalIdentifierTypeText( const AUniqID inIdentifierID ) const
{
	return mLocalIdentifierList.GetTypeText(inIdentifierID);
}
*/
void	ATextInfo::GetLocalIdentifierTypeText( const AUniqID inIdentifierID, AText& outText ) const
{
	mLocalIdentifierList.GetTypeText(inIdentifierID,outText);
}

AIndex	ATextInfo::GetLocalIdentifierCategoryIndexByKeywordText( const AText& inKeywordText ) const
{
	//R0000 カテゴリー可否 ★
	ABoolArray	tmpArray;
	AIndex	colorSlot = kIndex_Invalid;
	return mLocalIdentifierList.GetCategoryIndexByKeywordText(inKeywordText,true,tmpArray,colorSlot);
}

void	ATextInfoForDocument::GetLocalIdentifierRange( const AUniqID inIdentifierUniqID, ATextPoint& outStart, ATextPoint& outEnd ) const
{
	outStart.y = outEnd.y = GetLineIndexByLocalIdentifier(inIdentifierUniqID);
	outStart.x = mLocalIdentifierList.GetStartIndex(inIdentifierUniqID);
	outEnd.x = mLocalIdentifierList.GetEndIndex(inIdentifierUniqID);
}

//
void	ATextInfo::GetLocalIdentifierListByKeyword( const AText& inText, AArray<AUniqID>& outIdentifierIDArray ) const
{
	mLocalIdentifierList.GetIdentifierListByKeyword(inText,outIdentifierIDArray);
}

//指定識別子が存在する行のIndexを返す
AIndex	ATextInfoForDocument::GetLineIndexByLocalIdentifier( const AUniqID inIdentifierUniqID ) const
{
	//IdentifierListに識別子ごとに格納しているlineUniqIDを取得
	AUniqID	lineUniqID = mLocalIdentifierList.GetLineUniqID(inIdentifierUniqID);
	//lineUniqIDからlineIndexを取得
	return mLineInfo.FindLineIndexFromLineUniqID(lineUniqID);
	/* #695
	for( AIndex index = 0; index < GetLineCount(); index++ )
	{
		AUniqID	UniqID = mLineInfo.GetLineInfoP_FirstLocalIdentifierUniqID(index);
		while( UniqID != kUniqID_Invalid )
		{
			if( UniqID == inIdentifierUniqID )   return index;
			AUniqID	nextUniqID = mLocalIdentifierList.GetNextUniqID(UniqID);//#547
			if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			UniqID = nextUniqID;//#547
		}
	}
	return kIndex_Invalid;
	*/
}


//
void	ATextInfo::AddBlockStartData( const AIndex inLineIndex, const AIndex inVariableIndex, const AItemCount inIndentCount )
{
	//BlockStart数制限#695
	if( mBlockStartDataList.GetItemCount() >= kLimit_MaxBlockStartData )   return;
	
	//lineIndexの最後のオブジェクトを検索
	AUniqID	last = mLineInfo.GetLineInfoP_FirstBlockStartDataUniqID(inLineIndex);
	if( last != kUniqID_Invalid )
	{
		AUniqID	next = mBlockStartDataList.GetNextUniqID(last);
		while( next != kUniqID_Invalid )
		{
			last = next;
			next = mBlockStartDataList.GetNextUniqID(last);
			if( next == last )   {_ACError("object ID list error (loop)",this);break;}//#547
		}
	}
	AUniqID	uniqID = mBlockStartDataList.Add(last,inVariableIndex,inIndentCount);
	//previousがkObjectID_Invalidの場合は、行最初の識別子なので、mLineInfoP_BlockStartDataObjectIDにSet
	if( last == kUniqID_Invalid )
	{
		if( mLineInfo.GetLineInfoP_FirstBlockStartDataUniqID(inLineIndex) != kUniqID_Invalid )   _ACThrow("not the first of line identifier",this);
		mLineInfo.SetLineInfoP_FirstBlockStartDataUniqID(inLineIndex,uniqID);
	}
	//fprintf(stderr,"AddBlockStartData():%ld\n",mBlockStartDataList.GetItemCount());
}

//
AItemCount	ATextInfo::FindLastBlockStartIndentCount( const AIndex inStartLineIndex, const AIndex inVariableIndex ) const
{
	//★速度検討
	for( AIndex lineIndex = inStartLineIndex; lineIndex >= 0; lineIndex-- )
	{
		AUniqID	uniqID = mLineInfo.GetLineInfoP_FirstBlockStartDataUniqID(lineIndex);
		while( uniqID != kUniqID_Invalid )
		{
			if( inVariableIndex == mBlockStartDataList.GetVariableIndex(uniqID) )
			{
				return mBlockStartDataList.GetIndentCount(uniqID);
			}
			AUniqID	nextUniqID = mBlockStartDataList.GetNextUniqID(uniqID);//#547
			if( nextUniqID == uniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			uniqID = nextUniqID;//#547
		}
	}
	return 0;
}

#pragma mark ===========================

#pragma mark ---Importファイル

//Importファイル取得
void	ATextInfo::GetImportFileArray( const AFileAcc& inBaseFolder, AObjectArray<AFileAcc>& outFileArray )
{
	//#695 高速化。対象suffixを小文字化してローカル変数に格納。
	const AHashTextArray&	sa = GetApp().SPI_GetModePrefDB(mModeIndex).GetSuffixArray();
	ATextArray	suffixarray;
	for( AIndex i = 0; i < sa.GetItemCount(); i++ )
	{
		AText	suffix;
		sa.Get(i,suffix);
		suffix.ChangeCaseLower();
		suffixarray.Add(suffix);
	}
	
	//#271 追加済み判定用パス文字列配列を生成
	AHashTextArray	pathArray;
	for( AIndex i = 0; i < outFileArray.GetItemCount(); i++ )
	{
		AText	path;
		outFileArray.GetObjectConst(i).GetNormalizedPath(path);
		pathArray.Add(path);
	}
	//各行毎のループ（import file identifierを検索）
	for( AIndex index = 0; index < GetLineCount(); index++ )
	{
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(index);
		while( UniqID != kUniqID_Invalid )
		{
			if( GetGlobalIdentifierType(UniqID) == kIdentifierType_ImportFile )
			{
				//ImportFile識別子（#include等）があった場合、そのファイルをoutFileArrayに追加する。
				//ただし、既に追加済みの場合、ファイルが存在していない場合は、追加しない。
				//また、拡張子のみを変えたファイルが存在していればそれも追加（★要検討）
				
				//ファイル取得
				AText	text;
				GetGlobalIdentifierInfoText(UniqID,text);
				AFileAcc	file;
				file.SpecifyChild(inBaseFolder,text);//B0389 OK ,kFilePathType_1);kFilePathType_Defaultでも../.解釈できるので問題なし。
				//#695 高速化のため、下へ移動（Exist判定には時間がかかる） if( file.Exist() == true )
				//#695 {
				//outFileArrayにすでに追加済みかどうか調べる win 080714
				/*#271
				ABool	alreadyadded = false;
				for( AIndex j = 0; j < outFileArray.GetItemCount(); j++ )
				{
				if( outFileArray.GetObject(j).Compare(file) == true )
				{
				alreadyadded = true;
				break;
				}
				}
				*/
				AText	path;//#271
				file.GetNormalizedPath(path);//#271
				//追加済みでなければoutFileArrayへ追加
				if( /*#271alreadyadded == false*/pathArray.Find(path) == kIndex_Invalid )
				{
					//ファイルが存在していれば、outFileArrayに追加。
					if( file.Exist() == true )//#695 高速化のため上から移動
					{
						outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(file);
					}//#695
					//追加済みarrayに追加
					pathArray.Add(path);//#271
					//#695 }
					//#695 }
					//#695 }
					//win 080714
					
					//拡張子を変えたファイルも対象とする
					//★拡張子を変えたファイルも対象としてほしくない要望対応検討
					AText	pathwithoutsuffix;
					pathwithoutsuffix.SetText(text);
					AText	originalsuffix;
					pathwithoutsuffix.GetSuffix(originalsuffix);
					pathwithoutsuffix.DeleteAfter(pathwithoutsuffix.GetItemCount()-originalsuffix.GetItemCount());
					//モード設定の各拡張子毎に調べる
					//#693 const AHashTextArray&	suffixarray = GetApp().SPI_GetModePrefDB(mModeIndex).GetSuffixArray();
					for( AIndex i = 0; i < suffixarray.GetItemCount(); i++ )
					{
						//モード設定から拡張子を読み出す
						AText	suffix;
						suffixarray.Get(i,suffix);
						//#693 suffix.SetText(suffixarray.GetTextConst(i));
						if( suffix.Compare(originalsuffix) == true )   continue;
						//#693 suffix.ChangeCaseLower();
						//その拡張子に変えたファイル名のファイルが存在していればそのファイルもoutFileArrayに追加する
						AText	t;
						t.AddText(pathwithoutsuffix);
						t.AddText(suffix);
						AFileAcc	f;
						f.SpecifyChild(inBaseFolder,t);
						//#695 高速化のため、下へ移動 if( f.Exist() == true )
						//#695 {
						//outFileArrayにすでに追加済みかどうか調べる
						/*#271
						ABool	alreadyadded = false;
						for( AIndex j = 0; j < outFileArray.GetItemCount(); j++ )
						{
						if( outFileArray.GetObject(j).Compare(f) == true )
						{
						alreadyadded = true;
						break;
						}
						}
						*/
						AText	path;//#271
						f.GetNormalizedPath(path);//#271
						//追加済みでなければoutFileArrayへ追加
						if( /*#271alreadyadded == false*/pathArray.Find(path) == kIndex_Invalid )
						{
							//ファイルが存在していれば、outFileArrayに追加。
							if( f.Exist() == true )//#695 高速化のため上から移動
							{
								outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(f);
							}//#695
							//追加済みarrayに追加
							pathArray.Add(path);//#271
							//#695 }
						}
					}
				}
			}
			AUniqID	nextUniqID = mGlobalIdentifierList.GetNextUniqID(UniqID);//#547
			if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#547
			UniqID = nextUniqID;//#547
		}
	}
}

#pragma mark ===========================

#pragma mark ---

AIndex	ATextInfo::MatchKeywordRegExp( const AText& inKeyword ) const
{
	//#693 RegExpオブジェクトがPurgeされていたら再度RegExpを更新
	if( mRegExpObjectPurged == true )
	{
		UpdateRegExp();
	}
	
	AItemCount	keywordLen = inKeyword.GetItemCount();
	for( AIndex i = 0; i < mKeywordRegExp.GetItemCount(); i++ )
	{
		AIndex	pos = 0;
		if( mKeywordRegExp.GetObject(i).Match(inKeyword,pos,keywordLen) == true )
		{
			if( pos == keywordLen )
			{
				return mKeywordRegExp_CategoryIndex.Get(i);
			}
		}
	}
	return kIndex_Invalid;
}

#pragma mark ===========================

#pragma mark ---折りたたみ
//#450

/**
*/
AFoldingLevel	ATextInfoForDocument::GetFoldingLevel( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_FoldingLevel(inLineIndex);
}
/*#695
{
	AFoldingLevel	result = kFoldingLevel_None;
	
	//
	if( inLineIndex+1 < mLineInfoP_IndentCount.GetItemCount() )
	{
		if( mLineInfo.GetLineInfoP_IndentCount(inLineIndex) < mLineInfo.GetLineInfoP_IndentCount(inLineIndex+1) )
		{
			result = kFoldingLevel_Block;
		}
	}
	//
	AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(inLineIndex);
	while( UniqID != kUniqID_Invalid )
	{
		if( IsMenuIdentifier(UniqID) == true )
		{
			result = kFoldingLevel_Header;
			break;
		}
		AUniqID	nextUniqID = mGlobalIdentifierList.GetNextUniqID(UniqID);//#546
		if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#546 無限ループ防止
		UniqID = nextUniqID;//#546
	}
	//
	return result;
}
*/

#pragma mark ===========================

#pragma mark ---ワードリスト
//#723

/**
ワードリスト更新（生成）
@param inText ドキュメントのテキスト
@note thread-safe
*/
void	ATextInfo::UpdateWordsList( const AText& inText )
{
	//ワードリスト排他制御
	AStMutexLocker	locker(mWordsListMutex);
	//全削除
	mWordsList_Word.DeleteAll();
	mWordsList_WordStartTextIndex.DeleteAll();
	
	//参照取得（これらは、アプリ起動後、実体削除・移動等されることはない）
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(mModeIndex);
	ASyntaxDefinition&	syntaxDefinition = modePrefDB.GetSyntaxDefinition();
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
	
	//各行毎のループ
	AText	token;
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		//行情報取得
		AItemCount	startpos = GetLineStart(lineIndex);
		AItemCount	len = GetLineLengthWithoutLineEnd(lineIndex);
		AIndex	endpos = startpos + len;
		//文法パートindex取得（下のFindWord()の第７引数がtrueなので、文法パートによって単語定義が異なる。）
		AIndex	stateIndex = mLineInfo.GetLineInfoP_StateIndex(lineIndex);
		AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
		
		//文字毎ループ
		for( AIndex pos = startpos; pos < endpos;  )
		{
			//単語を取得
			AIndex	tokenspos = pos;
			ATextIndex	start = 0, end = 0;
			ABool	isAlphabet = FindWord(inText,mModeIndex,true,tokenspos,start,end,true,syntaxPartIndex);
			if( end > pos )
			{
				//posをendに進める
				pos = end;
				//アルファベットで構成されるtokenの場合のみワードリストに追加
				if( isAlphabet == true )
				{
					inText.GetText(tokenspos,end-tokenspos,token);
					//ワードリストに追加
					mWordsList_Word.Add(token);
					mWordsList_WordStartTextIndex.Add(tokenspos);
				}
			}
			else
			{
				//posを次の文字に進める
				pos = inText.GetNextCharPos(pos);
			}
		}
	}
}

/**
ワードリスト検索
@note thread-safe
*/
void	ATextInfo::FindWordsList( const AText& inWord, AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray ) const
{
	//ワードリスト排他制御
	AStMutexLocker	locker(mWordsListMutex);
	//ワードリストから検索
	AArray<AIndex>	indexArray;
	mWordsList_Word.FindAll(inWord,0,inWord.GetItemCount(),indexArray);
	//検索結果項目毎のループ
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		//結果格納
		AIndex	index = indexArray.Get(i);
		AIndex	spos = mWordsList_WordStartTextIndex.Get(index);
		outStartIndexArray.Add(spos);
		outEndIndexArray.Add(spos+inWord.GetItemCount());
	}
}

/**
ワードリストデータ全削除
*/
void	ATextInfo::DeleteAllWordsList()
{
	//ワードリスト排他制御
	AStMutexLocker	locker(mWordsListMutex);
	//
	mWordsList_Word.DeleteAll();
	mWordsList_WordStartTextIndex.DeleteAll();
}

/**
デバッグ用（TextInfoデータ整合性チェック）
*/
void	ATextInfo::CheckLineInfoDataForDebug()
{
	//
	mLineInfo.CheckLineInfoDataForDebug();
	//
	fprintf(stderr,"===========================\n");
	fprintf(stderr,"Global identifier count:%ld\n",mGlobalIdentifierList.GetItemCount());
	fprintf(stderr,"Local identifier count:%ld\n",mLocalIdentifierList.GetItemCount());
	fprintf(stderr,"===========================\n");
}

#pragma mark ===========================

#pragma mark ---ヒントテキスト
//

/**
ヒントテキスト追加
*/
AIndex	ATextInfo::AddHintText( const ATextIndex inTextIndex, const AText& inHintText )
{
	mHintTextArray_TextIndex.Add(inTextIndex);
	mHintTextArray_Text.Add(inHintText);
	return mHintTextArray_TextIndex.GetItemCount() -1;
}

/**
ヒントテキスト全削除
*/
void	ATextInfo::ClearHintText()
{
	mHintTextArray_TextIndex.DeleteAll();
	mHintTextArray_Text.DeleteAll();
}

/**
ヒントテキスト取得
*/
void	ATextInfo::GetHintText( const ATextIndex inTextIndex, AText& outHintText ) const
{
	outHintText.DeleteAll();
	AIndex	index = mHintTextArray_TextIndex.Find(inTextIndex);
	if( index != kIndex_Invalid )
	{
		mHintTextArray_Text.Get(index,outHintText);
	}
}

/**
ヒントテキスト更新（InsertText時）
*/
void	ATextInfo::UpdateHintTextByInsertText( const ATextIndex inTextIndex, const AIndex inTextCount )
{
	for( AIndex i = 0; i < mHintTextArray_TextIndex.GetItemCount(); i++ )
	{
		ATextIndex	hintTextIndex = mHintTextArray_TextIndex.Get(i);
		if( hintTextIndex >= inTextIndex )
		{
			hintTextIndex += inTextCount;
			mHintTextArray_TextIndex.Set(i,hintTextIndex);
		}
	}
}

/**
ヒントテキスト更新（DeleteText時）
*/
void	ATextInfo::UpdateHintTextByDeleteText( const ATextIndex inStart, const ATextIndex inEnd )
{
	for( AIndex i = 0; i < mHintTextArray_TextIndex.GetItemCount(); )
	{
		ATextIndex	hintTextIndex = mHintTextArray_TextIndex.Get(i);
		if( hintTextIndex >= inEnd )
		{
			hintTextIndex -= inEnd-inStart;
			mHintTextArray_TextIndex.Set(i,hintTextIndex);
			i++;
		}
		else if( hintTextIndex >= inStart && hintTextIndex < inEnd )
		{
			mHintTextArray_TextIndex.Delete1(i);
			mHintTextArray_Text.Delete1(i);
		}
		else
		{
			i++;
		}
	}
}

/**
ヒントテキストpos取得
*/
ATextIndex	ATextInfo::GetHintTextPos( const AIndex inHintTextIndex ) const
{
	return mHintTextArray_TextIndex.Get(inHintTextIndex);
}

/**
ヒントテキスト位置からindex取得
*/
AIndex	ATextInfo::FindHintIndexFromTextIndex( const ATextIndex inHintTextPos ) const
{
	return mHintTextArray_TextIndex.Find(inHintTextPos);
}

/*#695
#pragma mark ===========================
#pragma mark [クラス]ABlockStartDataList
#pragma mark ===========================

#pragma mark ---

AObjectID	ABlockStartDataList::Add( const AObjectID inPreviousID, const AIndex inVariableIndex, const AItemCount inIndentCount )
{
	//配列へ追加
	ABlockStartDataFactory	factory(this,inVariableIndex,inIndentCount);
	AIndex	index = mArray.AddNewObject(factory);
	AObjectID	addedObjectID = mArray.GetObjectConst(index).GetObjectID();
	
	//リスト接続
	//inPreviousIdentifierObjectIDがkObjectID_Invalidでない場合（＝リスト先頭以外）、前のNextに自オブジェクトを設定する。
	if( inPreviousID != kObjectID_Invalid )
	{
		//前のIdentifierのNextObjectIDに、追加したIdentifierのObjectIDを設定する。
		if( mArray.GetObjectByID(inPreviousID).GetNextObjectID() == kObjectID_Invalid )
		{
			mArray.GetObjectByID(inPreviousID).SetNextObjectID(addedObjectID);
		}
		else
		{
			//prevにObjectID設定済み（最後尾追加でない）場合はthrow
			_ACThrow("not last object",this);
		}
	}
	
	return addedObjectID;
}

//識別子削除
//リストに連なる識別子を全て削除する。
void	ABlockStartDataList::DeleteIdentifiers( const AObjectID inFirstObjectID )
{
	//まず次の識別子以降を削除（再帰）
	AObjectID	nextObjectID = mArray.GetObjectByID(inFirstObjectID).GetNextObjectID();
	if( nextObjectID == inFirstObjectID )   {_ACError("object ID list error (loop)",this);return;}//#547
	if( nextObjectID != kObjectID_Invalid )
	{
		DeleteIdentifiers(nextObjectID);
	}
	//自身を削除
	//indexはここで再度検索する必要がある。（再帰コールで次以降が削除済みなので、最初とはIndexが変わっている。）
	AIndex	index = mArray.GetIndexByID(inFirstObjectID);//検討必要（速度）
	mArray.Delete1Object(index);
}

AIndex	ABlockStartDataList::GetVariableIndex( const AObjectID inObjectID ) const
{
	return mArray.GetObjectConstByID(inObjectID).GetVariableIndex();
}

AItemCount	ABlockStartDataList::GetIndentCount( const AObjectID inObjectID ) const
{
	return mArray.GetObjectConstByID(inObjectID).GetIndentCount();
}

AObjectID	ABlockStartDataList::GetNextObjectID( const AObjectID inObjectID ) const
{
	return mArray.GetObjectConstByID(inObjectID).GetNextObjectID();
}
*/