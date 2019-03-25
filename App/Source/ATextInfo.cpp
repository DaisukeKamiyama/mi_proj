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
#pragma mark [�N���X]ALineInfo
#pragma mark ===========================

/**
�R���X�g���N�^
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
�f�X�g���N�^
*/
ALineInfo::~ALineInfo()
{
}

#pragma mark ===========================

#pragma mark ---�}���E�폜

/*
mLineInfoStorage_***: �s�f�[�^���i�[����e�[�u���B
mLineInfoStorage_UniqID�ɂ�UniqID������U��A�Ή�����index�̊e�v�f�Ƀf�[�^���i�[����B
�r���}���E�r���폜�͈�؍s��Ȃ��B�폜����Ƃ��́AmLineInfoStorage_UniqID�ɍ폜�}�[�N������B
�i�K�������s�̏��Ԃɂ͗v�f�����΂Ȃ��Bindex�͍s�ԍ��ł͂Ȃ��j

mLineInfoIDArray: ��L�e�[�u����UniqID���s�ԍ����ɕ��ׂ��z��B
�s�ǉ��E�폜���́A�����炾���r���}���E�r���폜���s���B
*/

/**
�s�}��
*/
void	ALineInfo::InsertLines( const AIndex inLineIndex, const AItemCount inLineCount )
{
	//mLineInfoIDArray�̗̈�m��
	mLineInfoIDArray.Reserve(inLineIndex,inLineCount);
	//�e�s���ɁA���g�pstorage�v�f�̍ė��p or �V�Kstorage�v�f�̒ǉ� ���s��
	for( AIndex lineIndex = inLineIndex; lineIndex < inLineIndex + inLineCount; lineIndex++ )
	{
		//Storage���̖��g�p�v�f���������A����΂����Ɋ��蓖�Ă�
		AIndex	storageIndex = mLineInfoStorage_UniqID.ReuseUnusedItem();
		if( storageIndex != kIndex_Invalid )
		{
			//���g�p�v�f���ė��p����B
			//�����l��ݒ�
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
			//�V�K�v�f��ǉ�����B
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
		//mLineInfoIDArray�̎w��s�ʒu�ɁA�ǉ�����Storage�v�f��uniqID��ǉ�
		AUniqID	uniqID = mLineInfoStorage_UniqID.Get(storageIndex);
		mLineInfoIDArray.Set(lineIndex,uniqID);
	}
}

/**
�s�}���i��s�j
*/
void	ALineInfo::Insert1Line( const AIndex inLineIndex )
{
	InsertLines(inLineIndex,1);
}

/**
�s�}���i�Ō�ɑ}���j
*/
AIndex	ALineInfo::Add1Line()
{
	AIndex	lineIndex = mLineInfoIDArray.GetItemCount();
	Insert1Line(lineIndex);
	return lineIndex;
}

/**
�s�폜
*/
void	ALineInfo::DeleteLines( const AIndex inLineIndex, const AItemCount inDeleteCount )
{
	for( AIndex lineIndex = inLineIndex; lineIndex < inLineIndex + inDeleteCount; lineIndex++ )
	{
		//�s��UniqID���擾
		AUniqID	uniqID = mLineInfoIDArray.Get(lineIndex);
		//Storage�̊Y��uniqID�v�f�𖢎g�p�v�f�ɂ���B
		AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
		mLineInfoStorage_UniqID.Unuse1Item(storageIndex);
		mLineInfoStorage_ParagraphIndex.Set(storageIndex,kIndex_Invalid);//GetLineIndexFromParagraphIndex()���̂��߁A�폜����paragraph��kIndex_Invalid��ݒ�
	}
	//mLineInfoIDArray�̎w��s���폜
	mLineInfoIDArray.Delete(inLineIndex,inDeleteCount);
}

/**
�s�폜
*/
void	ALineInfo::Delete1Line( const AIndex inLineIndex )
{
	DeleteLines(inLineIndex,1);
}

/**
�S�폜���APurge��Ԃ���������
*/
void	ALineInfo::DeleteAllAndCancelPurge()
{
	//�s�S�폜
	mLineInfoIDArray.DeleteAll();
	//Storage�S�폜
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
	//Purge��ԉ���
	mIsPurged = false;
}

/**
�s���g�厞�̍Ċ��蓖�đ����ʂ̐ݒ�
*/
void	ALineInfo::SetReallocateStep( const AItemCount inReallocateCount )
{
	//�����ʐݒ�ikLineInfoAllocationStep��菬�����͂��Ȃ��j
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
	//ReallocateStep�L���iAView_Text�ŎQ�Ƃ��邽�߁j
	mReallocateStep = reallocationStep;
}

#pragma mark ===========================

#pragma mark ---Get/Set

/**
Start�擾
*/
AIndex	ALineInfo::GetLineInfo_Start( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorage_Start.Get(storageIndex);
}

//#853
/**
LineUniqueID����Start�擾
*/
AIndex	ALineInfo::GetLineInfo_Start_FromLineUniqID( const AUniqID inLineUniqID ) const
{
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(inLineUniqID);
	return mLineInfoStorage_Start.Get(storageIndex);
}

/**
Start�ݒ�
*/
void	ALineInfo::SetLineInfo_Start( const AIndex inLineIndex, const AIndex inStart )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorage_Start.Set(storageIndex,inStart);
}

/**
Length�擾
*/
AItemCount	ALineInfo::GetLineInfo_Length( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorage_Length.Get(storageIndex);
}

/**
Length�ݒ�
*/
void	ALineInfo::SetLineInfo_Length( const AIndex inLineIndex, const AItemCount inLength )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorage_Length.Set(storageIndex,inLength);
}

/**
ExistLineEnd�擾
*/
ABool	ALineInfo::GetLineInfo_ExistLineEnd( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorage_ExistLineEnd.Get(storageIndex);
}

/**
ExistLineEnd�ݒ�
*/
void	ALineInfo::SetLineInfo_ExistLineEnd( const AIndex inLineIndex, const ABool inExistLineEnd )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorage_ExistLineEnd.Set(storageIndex,inExistLineEnd);
}

/**
FirstGlobalIdentifierUniqID�擾
*/
AUniqID	ALineInfo::GetLineInfo_FirstGlobalIdentifierUniqID( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorage_FirstGlobalIdentifierUniqID.Get(storageIndex);
}

/**
FirstGlobalIdentifierUniqID�ݒ�
*/
void	ALineInfo::SetLineInfo_FirstGlobalIdentifierUniqID( const AIndex inLineIndex, const AUniqID inFirstGlobalIdentifierUniqID )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorage_FirstGlobalIdentifierUniqID.Set(storageIndex,inFirstGlobalIdentifierUniqID);
}

/**
ParagraphIndex�擾
*/
AIndex	ALineInfo::GetLineInfo_ParagraphIndex( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorage_ParagraphIndex.Get(storageIndex);
}

/**
ParagraphIndex�ݒ�
*/
void	ALineInfo::SetLineInfo_ParagraphIndex( const AIndex inLineIndex, const AIndex inParagraphIndex )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorage_ParagraphIndex.Set(storageIndex,inParagraphIndex);
}

/**
Recognized�t���O����
*/
void	ALineInfo::ClearRecognizeFlags( const AIndex inLineIndex )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	//�t���O����
	mLineInfoStorage_RecognizeFlags.Set(storageIndex,kLineInfoRecognizeFlagMask_None);
}

/**
Recognized�擾
*/
ABool	ALineInfo::GetLineInfoP_Recognized( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_Recognized)!=0);
}

/**
Recognized�ݒ�
*/
void	ALineInfo::SetLineInfoP_Recognized( const AIndex inLineIndex, const ABool inRecognized )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
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
StateIndex�擾
*/
short int	ALineInfo::GetLineInfoP_StateIndex( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_StateIndex.Get(storageIndex);
}

/**
StateIndex�ݒ�
*/
void	ALineInfo::SetLineInfoP_StateIndex( const AIndex inLineIndex, const short int inStateIndex )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_StateIndex.Set(storageIndex,inStateIndex);
}

/**
PushedStateIndex�擾
*/
short int	ALineInfo::GetLineInfoP_PushedStateIndex( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_PushedStateIndex.Get(storageIndex);
}

/**
PushedStateIndex�ݒ�
*/
void	ALineInfo::SetLineInfoP_PushedStateIndex( const AIndex inLineIndex, const short int inPushedStateIndex )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_PushedStateIndex.Set(storageIndex,inPushedStateIndex);
}

/**
FirstLocalIdentifierUniqID�擾
*/
AUniqID	ALineInfo::GetLineInfoP_FirstLocalIdentifierUniqID( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_FirstLocalIdentifierUniqID.Get(storageIndex);
}

/**
FirstLocalIdentifierUniqID�ݒ�
*/
void	ALineInfo::SetLineInfoP_FirstLocalIdentifierUniqID( const AIndex inLineIndex, const AUniqID inFirstLocalIdentifierUniqID )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_FirstLocalIdentifierUniqID.Set(storageIndex,inFirstLocalIdentifierUniqID);
}

/**
ColorizeLine�擾
*/
ABool	ALineInfo::GetLineInfoP_ColorizeLine( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_ColorizeLine)!=0);
}

/**
ColorizeLine�ݒ�
*/
void	ALineInfo::SetLineInfoP_ColorizeLine( const AIndex inLineIndex, const ABool inColorizeLine )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
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
LineColor�擾
*/
AColor	ALineInfo::GetLineInfoP_LineColor( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_LineColor.Get(storageIndex);
}

/**
LineColor�ݒ�
*/
void	ALineInfo::SetLineInfoP_LineColor( const AIndex inLineIndex, const AColor inLineColor )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_LineColor.Set(storageIndex,inLineColor);
}

/**
IndentCount�擾
*/
short int	ALineInfo::GetLineInfoP_IndentCount( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_IndentCount.Get(storageIndex);
}

/**
IndentCount�ݒ�
*/
void	ALineInfo::SetLineInfoP_IndentCount( const AIndex inLineIndex, const short int inIndentCount )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_IndentCount.Set(storageIndex,inIndentCount);
}

/**
FirstBlockStartDataUniqID�擾
*/
AUniqID	ALineInfo::GetLineInfoP_FirstBlockStartDataUniqID( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_FirstBlockStartDataUniqID.Get(storageIndex);
}

/**
FirstBlockStartDataUniqID�ݒ�
*/
void	ALineInfo::SetLineInfoP_FirstBlockStartDataUniqID( const AIndex inLineIndex, const AUniqID inBlockStartDataUniqID )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_FirstBlockStartDataUniqID.Set(storageIndex,inBlockStartDataUniqID);
}

/**
Multiply�擾
*/
short int	ALineInfo::GetLineInfoP_Multiply( const AIndex inLineIndex ) const
{
	return 100;
	/*drop
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_Multiply.Get(storageIndex);
	*/
}

/**
Multiply�ݒ�
*/
void	ALineInfo::SetLineInfoP_Multiply( const AIndex inLineIndex, const short int inMultiply )
{
	/*drop
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_Multiply.Set(storageIndex,inMultiply);
	*/
}

/**
DirectiveLevel�擾
*/
short int	ALineInfo::GetLineInfoP_DirectiveLevel( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_DirectiveLevel.Get(storageIndex);
}

/**
DirectiveLevel�ݒ�
*/
void	ALineInfo::SetLineInfoP_DirectiveLevel( const AIndex inLineIndex, const short int inDirectiveLevel )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_DirectiveLevel.Set(storageIndex,inDirectiveLevel);
}

/**
DisabledDirectiveLevel�擾
*/
short int	ALineInfo::GetLineInfoP_DisabledDirectiveLevel( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_DisabledDirectiveLevel.Get(storageIndex);
}

/**
DisabledDirectiveLevel�ݒ�
*/
void	ALineInfo::SetLineInfoP_DisabledDirectiveLevel( const AIndex inLineIndex, const short int inDisabledDirectiveLevel )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_DisabledDirectiveLevel.Set(storageIndex,inDisabledDirectiveLevel);
}

/**
FoldingLevel�擾
*/
AFoldingLevel	ALineInfo::GetLineInfoP_FoldingLevel( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_FoldingLevel.Get(storageIndex);
}

/**
FoldingLevel�ݒ�
*/
void	ALineInfo::SetLineInfoP_FoldingLevel( const AIndex inLineIndex, const AFoldingLevel inFoldingLevel )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_FoldingLevel.Set(storageIndex,inFoldingLevel);
}

/**
MenuIdentifierExist�擾
*/
ABool	ALineInfo::GetLineInfoP_MenuIdentifierExist( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_MenuIdentifierExist)!=0);
}

/**
MenuIdentifierExist�ݒ�
*/
void	ALineInfo::SetLineInfoP_MenuIdentifierExist( const AIndex inLineIndex, const ABool inMenuIdentifierExist )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
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
Local�f���~�^���݃t���O�擾
*/
ABool	ALineInfo::GetLineInfoP_LocalDelimiterExist( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_LocalDelimiterExist)!=0);
}

/**
Local�f���~�^���݃t���O�ݒ�
*/
void	ALineInfo::SetLineInfoP_LocalDelimiterExist( const AIndex inLineIndex, const ABool inLocalDelimiterExist )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
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
SyntaxWarning���݃t���O�擾
*/
ABool	ALineInfo::GetLineInfoP_SyntaxWarningExist( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_SyntaxWarning)!=0);
}

/**
SyntaxWarning���݃t���O�ݒ�
*/
void	ALineInfo::SetLineInfoP_SyntaxWarningExist( const AIndex inLineIndex, const ABool inSyntaxWarning )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
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
SyntaxError���݃t���O�擾
*/
ABool	ALineInfo::GetLineInfoP_SyntaxErrorExist( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_SyntaxError)!=0);
}

/**
SyntaxError���݃t���O�ݒ�
*/
void	ALineInfo::SetLineInfoP_SyntaxErrorExist( const AIndex inLineIndex, const ABool inSyntaxError )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
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
Anchor���݃t���O�擾
*/
ABool	ALineInfo::GetLineInfoP_AnchorIdentifierExist( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return ((mLineInfoStorage_RecognizeFlags.Get(storageIndex)&kLineInfoRecognizeFlagMask_AnchorIdentifierExist)!=0);
}

/**
Anchor���݃t���O�ݒ�
*/
void	ALineInfo::SetLineInfoP_AnchorIdentifierExist( const AIndex inLineIndex, const ABool inAnchorExist )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
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
�s�`�F�b�N�����擾
*/
ANumber	ALineInfo::GetLineInfoP_CheckedDate( const AIndex inLineIndex ) const
{
	return 0;
	/*drop
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_CheckedDate.Get(storageIndex);
	*/
}

//#842
/**
�s�`�F�b�N�����ݒ�
*/
void	ALineInfo::SetLineInfoP_CheckedDate( const AIndex inLineIndex, const ANumber inDate )
{
	/*drop
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_CheckedDate.Set(storageIndex,inDate);
	*/
}

//#142
/**
�s�������擾
*/
AItemCount	ALineInfo::GetLineInfoP_CharCount( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_CharCount.Get(storageIndex);
}

//#142
/**
�s�������ݒ�
*/
void	ALineInfo::SetLineInfoP_CharCount( const AIndex inLineIndex, const AItemCount inCount )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_CharCount.Set(storageIndex,inCount);
}

//#142
/**
�s�P�ꐔ�擾
*/
AItemCount	ALineInfo::GetLineInfoP_WordCount( const AIndex inLineIndex ) const
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	return mLineInfoStorageP_WordCount.Get(storageIndex);
}

//#142
/**
�s�P�ꐔ�ݒ�
*/
void	ALineInfo::SetLineInfoP_WordCount( const AIndex inLineIndex, const AItemCount inCount )
{
	//�s��UniqID���擾
	AUniqID	uniqID = mLineInfoIDArray.Get(inLineIndex);
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(uniqID);
	mLineInfoStorageP_WordCount.Set(storageIndex,inCount);
}

/**
LineUniqID�擾
*/
AUniqID	ALineInfo::GetLineUniqID( const AIndex inLineIndex ) const
{
	return mLineInfoIDArray.Get(inLineIndex);
}

/**
LineUniqID����sindex�擾
@note ���xO(�s��)
*/
AIndex	ALineInfo::FindLineIndexFromLineUniqID( const AUniqID inLineUniqID ) const
{
	return mLineInfoIDArray.Find(inLineUniqID);
}

/**
LineUniqID����paragraph index�擾
*/
AIndex	ALineInfo::GetParagraphIndexFromLineUniqID( const AUniqID inLineUniqID ) const
{
	//UniqID�ɑΉ�����Storage���f�[�^���擾
	AIndex	storageIndex = mLineInfoStorage_UniqID.Find(inLineUniqID);
	return mLineInfoStorage_ParagraphIndex.Get(storageIndex);
}

/**
Paragraph index����sindex�擾
*/
AIndex	ALineInfo::GetLineIndexFromParagraphIndex( const AIndex inParagraphIndex ) const
{
	AItemCount	itemCount = mLineInfoStorage_ParagraphIndex.GetItemCount();
	AStArrayPtr<AIndex>	arrayptr(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p = arrayptr.GetPtr();
	//�i��index����v����ŏ��̍sindex����������
	AIndex	minLineIndex = kNumber_MaxNumber;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( p[i] == inParagraphIndex )//�폜����paragraph��kIndex_Invalid��ݒ肵�Ă���̂ŁA����storage�s�ɂ͈�v���Ȃ�
		{
			//�i��index����v������A����storage�s��uniqID���擾���AuniqID����sindex���擾
			AUniqID	uniqID = mLineInfoStorage_UniqID.Get(i);
			AIndex	lineIndex = mLineInfoIDArray.Find(uniqID);
			//�ŏ��̍sindex���擾
			if( lineIndex < minLineIndex )
			{
				minLineIndex = lineIndex;
			}
		}
	}
	if( minLineIndex == kNumber_MaxNumber )
	{
		//kNumber_MaxNumber�̂܂܂Ȃ�Y���i���Ȃ�
		return kIndex_Invalid;
	}
	else
	{
		//�Y���i���̍ŏ��sindex��Ԃ�
		return minLineIndex;
	}
}

/**
�s�̍ŏ����猟�����čŏ��Ɍ��������F���s��Ԃ�
*/
AIndex	ALineInfo::FindFirstUnrecognizedLine() const
{
	//Recognized�t���O��false�̍ŏ��̍s��index���擾����
	AItemCount	itemCount = mLineInfoStorage_RecognizeFlags.GetItemCount();
	AStArrayPtr<unsigned short int>	arrayptr_flags(mLineInfoStorage_RecognizeFlags,0,itemCount);
	unsigned short int*	p_flags = arrayptr_flags.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	//�i��index���ŏ��ƂȂ�A���F���s����������
	AIndex	minParagraphIndex = kNumber_MaxNumber;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//Recognized��false���ǂ���
		if( ((p_flags[i]) & kLineInfoRecognizeFlagMask_Recognized) == 0 )
		{
			//���F���s�Ȃ�A�ŏ���paragraph���L��
			AIndex	para = p_paragraph[i];
			if( para != kIndex_Invalid && para < minParagraphIndex )//�폜����paragraph��kIndex_Invalid��ݒ肵�Ă���
			{
				minParagraphIndex = para;
			}
		}
	}
	if( minParagraphIndex != kNumber_MaxNumber )
	{
		//���F���̍ŏ��i���ɑΉ�����sindex��Ԃ�
		return GetLineIndexFromParagraphIndex(minParagraphIndex);
	}
	else
	{
		//���F���Ȃ�
		return kIndex_Invalid;
	}
	//���x�v�����ʁF3.5M���ځAunrecognized�Ȃ��Ŗ�7�`12ms
	//unrecognized����͕p�ɂɂ͔������Ȃ��̂ŁA
	//10M���ڂ̕��ς�30ms���x���B
}

/**
�w��inLineIndex�ɑΉ�����menu identifier���擾
*/
AUniqID	ALineInfo::FindCurrentMenuIdentifier( const AIndex inLineIndex ) const
{
	//���ݒi�����擾
	AIndex	paragraphIndex = GetLineInfo_ParagraphIndex(inLineIndex);
	//�|�C���^�擾
	AItemCount	itemCount = mLineInfoStorage_RecognizeFlags.GetItemCount();
	AStArrayPtr<unsigned short int>	arrayptr_flags(mLineInfoStorage_RecognizeFlags,0,itemCount);
	unsigned short int*	p_flags = arrayptr_flags.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	AStArrayPtr<AUniqID>	arrayptr_uniqID(mLineInfoStorage_FirstGlobalIdentifierUniqID,0,itemCount);
	AUniqID*	p_uniqID = arrayptr_uniqID.GetPtr();
	//���ݒi���ւ̍sindex�I�t�Z�b�g��0�ȏォ�ŏ��ƂȂ�menu identifier������
	AIndex	minOffset = kNumber_MaxNumber;
	AUniqID	uniqID = kUniqID_Invalid;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//MenuIdentifierExist��true���ǂ���
		if( ((p_flags[i]) & kLineInfoRecognizeFlagMask_MenuIdentifierExist) != 0 )
		{
			//���ݒi���ւ̃I�t�Z�b�g���ŏ����L��
			AIndex	para = p_paragraph[i];
			AIndex	offset = paragraphIndex - para;
			if( para != kIndex_Invalid && offset >= 0 && offset < minOffset )//�폜����paragraph��kIndex_Invalid��ݒ肵�Ă���
			{
				minOffset = offset;
				uniqID = p_uniqID[i];
			}
		}
	}
	//������ɂ���uniqID�ɂȂ����Ă�����̂̂����{���ɑΏۂƂȂ�I�u�W�F�N�g����������K�v�L��
	return uniqID;
}

/**
�w��inLineIndex�ɑΉ����郍�[�J���͈͊J�nidentifier���擾
*/
AUniqID	ALineInfo::FindCurrentLocalStartIdentifier( const AIndex inLineIndex ) const
{
	//���ݒi�����擾
	AIndex	paragraphIndex = GetLineInfo_ParagraphIndex(inLineIndex);
	//�|�C���^�擾
	AItemCount	itemCount = mLineInfoStorage_RecognizeFlags.GetItemCount();
	AStArrayPtr<unsigned short int>	arrayptr_flags(mLineInfoStorage_RecognizeFlags,0,itemCount);
	unsigned short int*	p_flags = arrayptr_flags.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	AStArrayPtr<AUniqID>	arrayptr_uniqID(mLineInfoStorage_FirstGlobalIdentifierUniqID,0,itemCount);
	AUniqID*	p_uniqID = arrayptr_uniqID.GetPtr();
	//���ݒi���ւ̍sindex�I�t�Z�b�g��0�ȏォ�ŏ��ƂȂ�menu identifier������
	AIndex	minOffset = kNumber_MaxNumber;
	AUniqID	uniqID = kUniqID_Invalid;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//MenuIdentifierExist��true���ǂ���
		if( ((p_flags[i]) & kLineInfoRecognizeFlagMask_LocalDelimiterExist) != 0 )
		{
			//���ݒi���ւ̃I�t�Z�b�g���ŏ����L��
			AIndex	para = p_paragraph[i];
			AIndex	offset = paragraphIndex - para;
			if( para != kIndex_Invalid && offset >= 0 && offset < minOffset )//�폜����paragraph��kIndex_Invalid��ݒ肵�Ă���
			{
				minOffset = offset;
				uniqID = p_uniqID[i];
			}
		}
	}
	//������ɂ���uniqID�ɂȂ����Ă�����̂̂����{���ɑΏۂƂȂ�I�u�W�F�N�g����������K�v�L��
	return uniqID;
}

/**
�w��inLineIndex�ɑΉ�����Anchor identifier���擾
*/
AUniqID	ALineInfo::FindAnchorIdentifier( const AIndex inLineIndex ) const
{
	//���ݒi�����擾
	AIndex	paragraphIndex = GetLineInfo_ParagraphIndex(inLineIndex);
	//�|�C���^�擾
	AItemCount	itemCount = mLineInfoStorage_RecognizeFlags.GetItemCount();
	AStArrayPtr<unsigned short int>	arrayptr_flags(mLineInfoStorage_RecognizeFlags,0,itemCount);
	unsigned short int*	p_flags = arrayptr_flags.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	AStArrayPtr<AUniqID>	arrayptr_uniqID(mLineInfoStorage_FirstGlobalIdentifierUniqID,0,itemCount);
	AUniqID*	p_uniqID = arrayptr_uniqID.GetPtr();
	//���ݒi���ւ̍sindex�I�t�Z�b�g��0�ȏォ�ŏ��ƂȂ�anchor identifier������
	AIndex	minOffset = kNumber_MaxNumber;
	AUniqID	uniqID = kUniqID_Invalid;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//AnchorIdentifierExist��true���ǂ���
		if( ((p_flags[i]) & kLineInfoRecognizeFlagMask_AnchorIdentifierExist) != 0 )
		{
			//���ݒi���ւ̃I�t�Z�b�g���ŏ����L��
			AIndex	para = p_paragraph[i];
			AIndex	offset = paragraphIndex - para;
			if( para != kIndex_Invalid && offset >= 0 && offset < minOffset )//�폜����paragraph��kIndex_Invalid��ݒ肵�Ă���
			{
				minOffset = offset;
				uniqID = p_uniqID[i];
			}
		}
	}
	//������ɂ���uniqID�ɂȂ����Ă�����̂̂����{���ɑΏۂƂȂ�I�u�W�F�N�g����������K�v�L��
	return uniqID;
}

//#695
/**
���[�J���͈͎擾
@note ���[�J���f���~�^���S�����݂��Ȃ��ꍇ�́A�J�n�E�I���Ƃ�invalid�ŕԂ�
*/
void	ALineInfo::FindLocalRange( const AIndex inLineIndex, AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const
{
	//���ݒi�����擾
	AIndex	paragraphIndex = GetLineInfo_ParagraphIndex(inLineIndex);
	//�|�C���^�擾
	AItemCount	itemCount = mLineInfoStorage_RecognizeFlags.GetItemCount();
	AStArrayPtr<unsigned short int>	arrayptr_flags(mLineInfoStorage_RecognizeFlags,0,itemCount);
	unsigned short int*	p_flags = arrayptr_flags.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	//���[�J��delimiter�����݂���s�̂����A���ݒi���ւ̃I�t�Z�b�g��0�ȏ�ōŏ��̂��́A���ōő�i��Βl���ŏ��j�̂��̂��L��
	AIndex	minPlusOffset = kNumber_MaxNumber;
	AIndex	minPlusOffsetParagraph = kIndex_Invalid;
	AIndex	maxMinusOffset = kNumber_MinNumber;
	AIndex	maxMinusOffsetParagraph = kIndex_Invalid;
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//LocalDelimiterExist��true���ǂ���
		if( ((p_flags[i]) & kLineInfoRecognizeFlagMask_LocalDelimiterExist) != 0 )
		{
			//���ݒi���ւ̃I�t�Z�b�g���ŏ����L��
			AIndex	para = p_paragraph[i];
			AIndex	offset = paragraphIndex - para;
			//�I�t�Z�b�g��0�ȏ�ōŏ��̂��̂��L��
			if( para != kIndex_Invalid && offset >= 0 && offset < minPlusOffset )//�폜����paragraph��kIndex_Invalid��ݒ肵�Ă���
			{
				minPlusOffset = offset;
				minPlusOffsetParagraph = para;
			}
			//�I�t�Z�b�g�����ōő�i��Βl���ŏ��j�̂��̂��L��
			if( para != kIndex_Invalid && offset < 0 && offset > maxMinusOffset )
			{
				maxMinusOffset = offset;
				maxMinusOffsetParagraph = para;
			}
		}
	}
	//���[�J���f���~�^���S�����݂��Ȃ��ꍇ�́A�J�n�E�I���Ƃ�invalid�ŕԂ�
	if( minPlusOffset == kNumber_MaxNumber && maxMinusOffset == kNumber_MinNumber )
	{
		outStartLineIndex = outEndLineIndex = kIndex_Invalid;
		return;
	}
	//���[�J���͈͊J�n�ʒu
	if( minPlusOffset != kNumber_MaxNumber )
	{
		outStartLineIndex = GetLineIndexFromParagraphIndex(minPlusOffsetParagraph);
	}
	else
	{
		outStartLineIndex = 0;
	}
	//���[�J���͈͏I���ʒu
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
�P�ꐔ���v�擾
*/
void	ALineInfo::GetTotalWordCount( AItemCount& outTotalCharCount, AItemCount& outTotalWordCount ) const
{
	//���ʏ�����
	outTotalCharCount = 0;
	outTotalWordCount = 0;
	//�|�C���^�擾
	AItemCount	itemCount = mLineInfoStorageP_CharCount.GetItemCount();
	AStArrayPtr<AItemCount>	arrayptr_charcount(mLineInfoStorageP_CharCount,0,itemCount);
	AIndex*	p_charcount = arrayptr_charcount.GetPtr();
	AStArrayPtr<AItemCount>	arrayptr_wordcount(mLineInfoStorageP_WordCount,0,itemCount);
	AIndex*	p_wordcount = arrayptr_wordcount.GetPtr();
	AStArrayPtr<AIndex>	arrayptr_paragraph(mLineInfoStorage_ParagraphIndex,0,itemCount);
	AIndex*	p_paragraph = arrayptr_paragraph.GetPtr();
	//�폜�s�ȊO�̕������E�P�ꐔ���v�v�Z
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
ImportFileData�p�Ƃ��ĕs�v�ȃf�[�^���폜����i�������ߖ�̂��߁j
�i�C���|�[�g�t�@�C���p�Ȃ̂ŃO���[�o�����ʎq���͎c���j
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
	//Purge��Ԃɐݒ�
	mIsPurged = true;
}

#pragma mark ===========================

#pragma mark ---

/**
�w��s�ڍs��LineStart�����炷
*/
void	ALineInfo::ShiftLineStarts( const AIndex inStartLineIndex, const AItemCount inAddedTextLength )
{
	//inStartLineIndex���ŏI�s����Ȃ牽�������I��
	if( inStartLineIndex >= GetLineCount() )
	{
		return;
	}
	//���炵�Ώۂ�text index���擾�i����ȍ~��lineStart�����炷�j
	AIndex	lineStartToBeShifted = GetLineInfo_Start(inStartLineIndex);
	//mLineInfoStorage_Start�̊e�v�f�̂���lineStartToBeShifted�ڍs�̂��̂�S�Ă��炷
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
�w��s�ȍ~��paragraph index�����炷
*/
void	ALineInfo::ShiftParagraphIndex( const AIndex inStartLineIndex, const AItemCount inParagraphCount )
{
	//inStartLineIndex���ŏI�s����Ȃ牽�������I��
	if( inStartLineIndex >= GetLineCount() )
	{
		return;
	}
	
	//���炵�Ώۂ�paragraph index���擾�i����ȍ~��paragraph index�����炷�j
	//inStartLineIndex�̍s�̒i���͑ΏۊO�Ƃ���B���̒i���ȍ~��ΏۂƂ���B
	//inStartLineIndex�̍s�̒i���͒i���r���̉\�������邽�߁A���̃u���b�N�ŏ����B
	{
		AIndex	paragraphIndexToBeShifted = GetLineInfo_ParagraphIndex(inStartLineIndex) + 1;
		//mLineInfoStorage_Start�̊e�v�f�̂���lineStartToBeShifted�ڍs�̂��̂�S�Ă��炷
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
	
	//�Ώۍs�ȍ~�ŉ��s��������܂ŁA+inParagraphCount���Ă���
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
���C���X���b�h���̌��݂̍s�����A���@�F���X���b�h����TextInfo�փR�s�[����i���@�F���X���b�h���s�O�����j
�i���@�F���X���b�h����TextInfo�I�u�W�F�N�g�ŃR�[������j
*/
void	ALineInfo::CopyLineInfoToSyntaxRecognizer( const ALineInfo& inSrcLineInfo, const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	//Dst���S�f�[�^���폜
	DeleteAllAndCancelPurge();
	//Dst�����������蓖�ăX�e�b�v�ݒ�
	SetReallocateStep(inEndLineIndex-inStartLineIndex);
	//Dst���ɍs�ꊇ�}��
	InsertLines(0,inEndLineIndex-inStartLineIndex);
	
	//Dst���̍sindex
	AIndex	dstLineIndex = 0;
	
	//Src���̊J�n�s��LineStart���擾�idst���̊J�n�s��LineStart��0�ɂȂ�悤�ALineStart�����炷���߂Ɏg���j
	AIndex	srcFirstLineStart = inSrcLineInfo.GetLineInfo_Start(inStartLineIndex);
	//�s�����[�v
	for( AIndex srcLineIndex = inStartLineIndex; srcLineIndex < inEndLineIndex; srcLineIndex++ )
	{
		//Dst��
		AUniqID	dstUniqID = mLineInfoIDArray.Get(dstLineIndex);
		AIndex	dstStorageIndex = mLineInfoStorage_UniqID.Find(dstUniqID);
		
		//Src��
		AUniqID	srcUniqID = inSrcLineInfo.mLineInfoIDArray.Get(srcLineIndex);
		AIndex	srcStorageIndex = inSrcLineInfo.mLineInfoStorage_UniqID.Find(srcUniqID);
		
		//�f�[�^���R�s�[
		mLineInfoStorage_Start.Set(dstStorageIndex,
					inSrcLineInfo.mLineInfoStorage_Start.Get(srcStorageIndex) - srcFirstLineStart );//dst���̊J�n�s��LineStart��0�ɂȂ�悤�ALineStart�����炷�B
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
		//Dst���sindex�����̍s��
		dstLineIndex++;
	}
}

/**
���@�F���X���b�h�̔F�����ʂ��A���C���X���b�h��TextInfo�փR�s�[����
�i���C���X���b�h���̃I�u�W�F�N�g�ŃR�[������j
*/
void	ALineInfo::CopyFromSyntaxRecognizerResult( const AIndex inDstStartLineIndex, const AIndex inDstEndLineIndex,
		const ALineInfo& inSrcLineInfo )
{
	//Src���̍sindex
	AIndex	srcLineIndex = 0;
	//Dst���̍sindex�Ń��[�v
	for( AIndex dstLineIndex = inDstStartLineIndex; dstLineIndex < inDstEndLineIndex; dstLineIndex++ )
	{
		//Dst��
		AUniqID	dstUniqID = mLineInfoIDArray.Get(dstLineIndex);
		AIndex	dstStorageIndex = mLineInfoStorage_UniqID.Find(dstUniqID);
		
		//Src��
		AUniqID	srcUniqID = inSrcLineInfo.mLineInfoIDArray.Get(srcLineIndex);
		AIndex	srcStorageIndex = inSrcLineInfo.mLineInfoStorage_UniqID.Find(srcUniqID);
		
		//�F�����ʂ��R�s�[�i���ʎq��ATextInfo::CopyFromSyntaxRecognizerResult()�ŃR�s�[�����j
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
		
		//Src���sindex�����̍s��
		srcLineIndex++;
	}
}

/**
�f�o�b�O�p�s�f�[�^�������`�F�b�N
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
		//LineStart, length�`�F�b�N�A�i���`�F�b�N
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
#pragma mark [�N���X]ATextInfo
#pragma mark ===========================
//�e�L�X�g�̍s���A���ʎq�����쐬�A�i�[�A�Ǘ�����N���X
//ADocument_Text�A����сAAMultiFileFinder�̌������\�b�h�ɏ��L�����
//�iADocument_Text�����݂��Ă��Ȃ��Ă�����\�j

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

//�R���X�g���N�^
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
//�f�X�g���N�^
ATextInfo::~ATextInfo()
{
	//�S�f�[�^�폜 #890
	//�ʃX���b�h����A�N�Z�X���ɂ����Ȃ�I�u�W�F�N�g�폜���Ȃ��悤�ɂ��邽�߁B
	DeleteLineInfoAll();
}

//#693
/**
ImportFileData�p�Ƃ��ĉ�͌�ɕs�v�ȃf�[�^���폜����
*/
void	ATextInfo::PurgeForImportFileData()
{
	PurgeRegExpObject();
	PurgeLineInfo();
}

//#693
/**
RegExp�I�u�W�F�N�g��purge����
AImportFileRecognizer::NVIDO_ThreadMain()�Ńo�b�N�O���E���h�ŔF������Ƃ��A�F�����I������烁�����ߖ�̂���RegExp���f�[�^���폜����
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
	
	//Purge�t���O�ݒ�i�ēxRecognizeSyntax()���R�[�����ꂽ��A��������ēxUpdateRegExp()���R�[�������j
	mRegExpObjectPurged = true;
}

//#693
/**
LineInfo�̂���ImportFileData�p�Ƃ��Ă͕s�v�Ȃ��̂��폜����i�������ߖ�̂��߁j
�i�C���|�[�g�t�@�C���p�Ȃ̂ŃO���[�o�����ʎq���͎c���j
*/
void	ATextInfo::PurgeLineInfo()
{
	//LineInfo�̂���ImportFileData�p�Ƃ��Ă͕s�v�Ȃ��̂��폜����
	//�i�C���|�[�g�t�@�C���p�Ȃ̂ŃO���[�o�����ʎq���͎c���j
	mLineInfo.Purge();
	//
	{
		//#717
		//���[�J�����ʎq�r������
		AStMutexLocker	locker(mLocalIdentifierListMutex);
		//���[�J�����ʎq�S�폜
		mLocalIdentifierList.DeleteAllIdentifiers();
	}
	//block start data���X�g�S�폜
	mBlockStartDataList.DeleteAll();

}

//#896
/**
�S�f�[�^�폜
*/
void	ATextInfo::DeleteAllInfo()
{
	//�s���S�폜
	DeleteLineInfoAll();
	//���K�\���I�u�W�F�N�g�f�[�^�S�폜
	PurgeRegExpObject();
	//���[�h���X�g�f�[�^�S�폜
	DeleteAllWordsList();
}

#pragma mark ===========================

#pragma mark ---

void	ATextInfo::SetMode( const AModeIndex inModeIndex, const ABool inLoadRegExp )
{
	mModeIndex = inModeIndex;
	
	//RegExp�I�u�W�F�N�g��purge����B�K�v���Ƀ��[�h����B
	//#693 UpdateRegExp();//#683
	PurgeRegExpObject();//#693
	//inLoadRegExp��true�Ȃ�A������reg exp�I�u�W�F�N�g�����[�h�ifalse�̏ꍇ�́A�K�v�Ȏ��Ƀ��[�h�j
	if( inLoadRegExp == true )
	{
		UpdateRegExp();
	}
}

//#683
/**
TextInfo���Ŏ����Ă��鐳�K�\���I�u�W�F�N�g�X�V
*/
void	ATextInfo::UpdateRegExp() const
{
	GetApp().SPI_GetModePrefDB(mModeIndex).SetJumpSetupRegExp(mJumpSetupRegExp);
	GetApp().SPI_GetModePrefDB(mModeIndex).SetIndentRegExp(mIndentRegExp);
	GetApp().SPI_GetModePrefDB(mModeIndex).GetSyntaxDefinition().
			GetRegExpArray(mSyntaxDefinitionRegExp,mSyntaxDefinitionRegExp_State,mSyntaxDefinitionRegExp_Index);
	GetApp().SPI_GetModePrefDB(mModeIndex).GetKeywordRegExp(mKeywordRegExp,mKeywordRegExp_CategoryIndex);
	//#693 Purge�t���Ooff
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
			//B0381 ���K�\����v�I���ʒu����ł�����̂��珇�ɕ��ׂ�array�Ɋi�[
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
	
	//B0381 ���ݍs�C���f���g�Ǝ��s�C���f���g�̘a��0�ƂȂ�ꍇ�i<p>�Ŏ��s+4, </p>�Ō��ݍs-4�̏ꍇ��<p></p>����͂����ꍇ�Ȃǁj�͌��ݍs�����s��0�Ƃ���
	/*B0381 if( currentTotal+nextTotal == 0 )
	{
		currentTotal = 0;
		nextTotal = 0;
	}
	ioCurrent += currentTotal;
	ioNext += nextTotal;*/
	//B0381 �O��next�͌��current�Ƒ��E����
	for( AIndex i = 0; i < nextArray.GetItemCount(); i++ )
	{
		AItemCount	mae_next = nextArray.Get(i);
		if( mae_next == 0 )   continue;
		for( AIndex j = i+1; j < currentArray.GetItemCount(); j++ )
		{
			AItemCount	ato_current = currentArray.Get(j);
			if( mae_next*ato_current < 0 )//���next�ƌ��current��+, -���t�ł���ꍇ
			{
				//���next�̐�Βl�͈͓̔��ő��E
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

#pragma mark ---�s���z��ݒ�^�擾

//�s���}��
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

//�s���ǉ�
void	ATextInfo::AddLineInfo()
{
	//#695 InsertLineInfo(mLineInfo_Start.GetItemCount());
	mLineInfo.Add1Line();
}

//#699
/**
�s�ǉ����ALineStart,Length, Paragraph��ݒ�iParagraph�͎����v�Z���Đݒ�j
*/
void	ATextInfo::InsertLineInfo( const AIndex inLineIndex, const AIndex inStart, const AItemCount inLength, const ABool inExistLineEnd )
{
	//�s�ǉ�
	mLineInfo.Insert1Line(inLineIndex);
	//LineStart, Length��ݒ�
	mLineInfo.SetLineInfo_Start(inLineIndex,inStart);
	mLineInfo.SetLineInfo_Length(inLineIndex,inLength);
	mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,inExistLineEnd);
	//�i���v�Z�A�ݒ�
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
�s�ǉ����ALineStart,Length, Paragraph��ݒ�iParagraph�͎����v�Z���Đݒ�j
*/
void	ATextInfo::AddLineInfo( const AIndex inStart, const AItemCount inLength, const ABool inExistLineEnd )
{
	InsertLineInfo(GetLineCount(),inStart,inLength,inExistLineEnd);
}

/**
�s�폜
*/
void	ATextInfo::DeleteLineInfos( const AIndex inLineIndex, const AItemCount inDeleteCount,
		AArray<AUniqID>& outDeletedIdentifiers, ABool& outImportFileIdentifierDeleted )
{
	//���ʎq�폜�i���̍s����폜�j
	for( AIndex lineIndex = inLineIndex + inDeleteCount - 1; lineIndex >= inLineIndex ; lineIndex-- )
	{
		DeleteLineIdentifiers(lineIndex,outDeletedIdentifiers,outImportFileIdentifierDeleted);
	}
	//�s�f�[�^�폜
	mLineInfo.DeleteLines(inLineIndex,inDeleteCount);
}

//�s���擾
AItemCount	ATextInfo::GetLineCount() const
{
	//#695 return mLineInfo_Start.GetItemCount();
	return mLineInfo.GetLineCount();
}

//LineEnd�R�[�h���܂߂Ȃ��s�����O�X�擾
AItemCount	ATextInfo::GetLineLengthWithoutLineEnd( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfo_Length(inLineIndex) - (mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex)?1:0);
}

//LineEnd�R�[�h���܂߂��s�����O�X�擾
AItemCount	ATextInfo::GetLineLengthWithLineEnd( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfo_Length(inLineIndex);
}

//�s����TextIndex���擾
ATextIndex	ATextInfo::GetLineStart( const AIndex inLineIndex ) const 
{
	return mLineInfo.GetLineInfo_Start(inLineIndex);
}

//
AItemCount	ATextInfo::GetTextLength() const
{
	return mLineInfo.GetLineInfo_Start(GetLineCount()-1) + mLineInfo.GetLineInfo_Length(GetLineCount()-1);
}

//�s��LineEnd�R�[�h�����邩�ǂ�����Ԃ�
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
�w��s��Directive Level���擾
*/
AIndex	ATextInfoForDocument::GetDirectiveLevel( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_DirectiveLevel(inLineIndex);
}

//#467
/**
�w��s��Directive�ɂ��disable�ɂȂ����Ƃ���level���擾�idisable�łȂ����kIndex_Invalid�j
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
���o�����K�\���O���[�v�F�Â��f�[�^�擾
*/
void	ATextInfoForDocument::GetLineRegExpGroupColor( const AIndex inLineIndex,
			AColor& outColor, AIndex& outStartIndex, AIndex& outEndIndex ) const
{
	//��#695
	outColor = kColor_Black;//#695 mLineInfoP_RegExpGroupColor.Get(inLineIndex);
	outStartIndex = kIndex_Invalid;//#695 mLineInfoP_RegExpGroupColor_StartIndex.Get(inLineIndex);
	outEndIndex = kIndex_Invalid;//#695 mLineInfoP_RegExpGroupColor_EndIndex.Get(inLineIndex);
}

//#695
/**
state��stable�ȍs���ǂ����𔻒�
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
state��stable�ȍs���AinLineIndex�̑O�̍s���炳���̂ڂ��Č���
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
SyntaxWarning�擾
*/
ABool	ATextInfoForDocument::GetSyntaxWarning( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_SyntaxWarningExist(inLineIndex);
}

/**
SyntaxError�擾
*/
ABool	ATextInfoForDocument::GetSyntaxError( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_SyntaxErrorExist(inLineIndex);
}

//#842
/**
�s�`�F�b�N�����ݒ�
*/
void	ATextInfoForDocument::SetLineCheckedDate( const AIndex inLineIndex, const ANumber inDate )
{
	mLineInfo.SetLineInfoP_CheckedDate(inLineIndex,inDate);
}

//#842
/**
�s�`�F�b�N�����擾
*/
ANumber	ATextInfoForDocument::GetLineCheckedDate( const AIndex inLineIndex ) const
{
	return mLineInfo.GetLineInfoP_CheckedDate(inLineIndex);
}

//#142
/**
���v�s���擾
*/
void	ATextInfoForDocument::GetTotalWordCount( AItemCount& outTotalCharCount, AItemCount& outTotalWordCount ) const
{
	mLineInfo.GetTotalWordCount(outTotalCharCount,outTotalWordCount);
}

#pragma mark ===========================

#pragma mark ---TextPoint/TextIndex�ϊ�

//TextPoint����Index�擾
AIndex	ATextInfo::GetTextIndexFromTextPoint( const ATextPoint& inPoint ) const
{
	return GetLineStart(inPoint.y)+inPoint.x;
}

//Index����TextPoint�擾
void	ATextInfo::GetTextPointFromTextIndex( const AIndex inTextPosition, ATextPoint& outPoint, const ABool inPreferNextLine ) const 
{
	//inIndex���s���̏ꍇ�̏���
	if( inTextPosition > GetTextLength() || inTextPosition < 0 ) 
	{
		_ACError("index invalid",this);
		outPoint.y = GetLineCount()-1;
		outPoint.x = GetTextLength() - GetLineStart(GetLineCount()-1);
		return;
	}
	//2���@
	AIndex	startLineIndex = 0;
	AIndex	endLineIndex = GetLineCount();
	AIndex	lineIndex = (startLineIndex+endLineIndex)/2;
	AItemCount	lineCount = GetLineCount();
	for( AIndex i = 0; i < lineCount*2; i++ )//�������[�v�h�~�i�����Ƃ��s����̃��[�v�ŏI���͂��j
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

#pragma mark ---�`��p�e�L�X�g�擾

//�e�L�X�g�`��p�f�[�^�擾
//UTF16���A�^�u�̃X�y�[�X���A���䕶���̉��������s���B
//CTextDrawData�N���X�̂����A���L��ݒ肷��B
//UTF16DrawText: UTF16�ł̕`��p�e�L�X�g
//UnicodeDrawTextIndexArray: �C���f�b�N�X�F�h�L�������g���ێ����Ă��錳�̃e�L�X�g�̃C���f�b�N�X�@�l�F�`��e�L�X�g��Unicode�����P�ʂł̃C���f�b�N�X
//inToLineEndCode: false�̏ꍇ�͎w��s�̂݁Atrue�̏ꍇ�͍s���𖳎����ĉ��s�R�[�h�܂�
void	ATextInfo::GetTextDrawDataWithoutStyle( const AText& inText, const ANumber inTabWidth, const AItemCount inIndentWidth, //#117
											   const AItemCount inLetterCountLimit, 
											   const AIndex inLineIndex, CTextDrawData& outTextDrawData, 
											   const ABool inGetUTF16Text, const ABool inCountAs2Letters,
											   const AIndex inToLineEndCode, const ABool inForDraw,
											   const ABool inDisplayYenFor5C ) const //B0000 �s�܂�Ԃ��v�Z������ #695 #940
{
	//�s���擾
	AIndex	lineInfo_Start = GetLineStart(inLineIndex);
	//B0000 �s�܂�Ԃ��v�Z������
	AItemCount	lineInfo_LengthWithoutCR = inText.GetItemCount()-lineInfo_Start;
	if( inToLineEndCode == false )
	{
		lineInfo_LengthWithoutCR = GetLineLengthWithoutLineEnd(inLineIndex);
	}
	//�擾���镶�����i�o�C�g���j��kLimit_Max1LineDrawByteCounts�ɐ�������i�`�掞�̂݁j #695
	//�ikLimit_Max1LineDrawByteCounts�ȏ�ƂȂ�ŏ��̕����܂łŎ擾�ł�����ƂȂ�j
	if( inForDraw == true )
	{
		if( lineInfo_LengthWithoutCR > kLimit_Max1LineDrawByteCounts )
		{
			lineInfo_LengthWithoutCR = kLimit_Max1LineDrawByteCounts;
		}
	}
	//�Ώۍs�̃q���g�e�L�X�g���擾
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
	//�쐬
	outTextDrawData.MakeTextDrawDataWithoutStyle(inText,inTabWidth,inLetterCountLimit,inGetUTF16Text,inCountAs2Letters,
												 GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayControlCode),
												 lineInfo_Start,lineInfo_LengthWithoutCR,
												 hintTextPosArray,hintTextArray,
												 GetApp().SPI_GetModePrefDB(mModeIndex).GetData_Bool(AModePrefDB::kStartSpaceToIndent),
												 inIndentWidth,inTabWidth,//#117
												 inDisplayYenFor5C);//#940
}

#pragma mark ===========================

#pragma mark ---�s���v�Z

/**
�s�S�폜
*/
void	ATextInfo::DeleteLineInfoAll(/*#695 AArray<AUniqID>& outDeletedIdentifiers */)
{
	/*#695 �������B�S�폜�̏ꍇ�A�폜����identifier��uniqID���L������K�v�͂Ȃ��B�i�S�폜�̂��߁j
	//�s����S�ď���
	ABool	importChanged = false;
	while( GetLineCount() > 0 )
	{
		DeleteLineInfo(GetLineCount()-1,outDeletedIdentifiers,importChanged);
	}
	//���ʎq���̍폜
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		DeleteLineIdentifiers(lineIndex,outDeletedIdentifiers,importChanged);
	}
	*/
	//���ʎq�S�폜
	{
		//#717
		//�O���[�o�����ʎq���X�g�r������
		AStMutexLocker	locker(mGlobalIdentifierListMutex);
		//�O���[�o�����ʎq���X�g�S�폜
		mGlobalIdentifierList.DeleteAllIdentifiers();
	}
	{
		//#717
		//���[�J�����ʎq���X�g�r������
		AStMutexLocker	locker(mLocalIdentifierListMutex);
		//���[�J�����ʎq���X�g�S�폜
		mLocalIdentifierList.DeleteAllIdentifiers();
	}
	mBlockStartDataList.DeleteAll();
	//�s���폜���APurge��ԉ���
	mLineInfo.DeleteAllAndCancelPurge();
}

/**
�s�����v�Z����i�����͑S�č폜���A�V�K�ɐ����j
*/
AItemCount	ATextInfo::CalcLineInfoAll( const AText& inText, 
									   const ABool inAddEmptyEOFLineIfNeeded, const AItemCount inLimitLineCount,//#699
									   const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
									   const AItemCount inTabWidth, const AItemCount inIndentWidth, 
									   const AWrapMode inWrapMode, const AItemCount inWrapLetterCount, //#117
									   const ANumber inViewWidth, const ABool inCountAs2Letters )
{
	//�s���S�폜
	DeleteLineInfoAll(/*#695 outDeletedIdentifiers*/);
	
	//���������蓖�ăX�e�b�v�ݒ�
	if( inLimitLineCount == kIndex_Invalid )
	{
		//#695 �e�L�X�g���̍s������LineInfo�̃��������蓖�đ����X�e�b�v�ɐݒ�
		//�i������ALineInfo�ɂ�kLineInfoAllocationStep��菬�����͐ݒ肳��Ȃ��j
		AItemCount	lineEndCharCount = inText.GetCountOfChar(kUChar_LineEnd,0,inText.GetItemCount());
		mLineInfo.SetReallocateStep(lineEndCharCount);
	}
	else
	{
		//#699 �����l�L��̏ꍇ�͂�����X�e�b�v�ɐݒ�
		mLineInfo.SetReallocateStep(inLimitLineCount);
	}
	
	//==================�e�s�s�v�Z==================
	
	//B0000 �s�܂�Ԃ��v�Z������
	CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	//#1034 textDrawData.mReuseATSUStyle = true;//#695
	
	AIndex	textDrawDataStartOffset = kIndex_Invalid;
	
	//���C���X���b�h�ł̎��s�Ȃ烂�[�_���Z�b�V�����J�n
	ABool	showModalSession = (ABaseFunction::InMainThread()==true);
	AStEditProgressModalSession	modalSession(kEditProgressType_CalcLineInfo,true,false,showModalSession);
	
	//�s���ɍs����ǉ��A�v�Z���Ă���
	for( AIndex lineIndex = 0; ; lineIndex++ )
	{
		//���[�_���Z�b�V�����p������
		if( showModalSession == true )
		{
			AIndex	pos = 0;
			if( lineIndex > 0 )
			{
				pos = GetLineStart(lineIndex-1) + GetLineLengthWithLineEnd(lineIndex-1);
			}
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_CalcLineInfo,0,true,pos,inText.GetItemCount()) == false )
			{
				//�L�����Z����
				break;
			}
		}
		
		//�s�ǉ�
		AddLineInfo();
		//�s�v�Z
		CalcLineInfo(inText,lineIndex,inFontName,inFontSize,inAntiAliasing,inTabWidth,inIndentWidth,inWrapMode,inWrapLetterCount,inViewWidth,inCountAs2Letters,
					textDrawData,textDrawDataStartOffset);//B0000 �s�܂�Ԃ��v�Z������
		//�e�L�X�g�S�Ă̍s�v�Z������������break
		if( GetLineStart(lineIndex) + GetLineLengthWithLineEnd(lineIndex) >= inText.GetItemCount() )   break;
		//�s������������ꍇ�i�ŏ���200�s��A�s�v�Z�X���b�h����̎��s�j�A�s������#699
		//���݂̍s�����A�����l�ȏ�̏ꍇ�́A���̍s�܂łŏI��
		//�i���̔���ŏI������ꍇ�́A��̔��肪false�Ȃ̂ŁA�e�L�X�g�̓r���BAddEmptyEOFLineIfNeeded()�͕s�v�B�j
		if( inLimitLineCount != kIndex_Invalid )
		{
			if( lineIndex+1 >= inLimitLineCount )
			{
				//�e�L�X�g�̌v�Z�ςݏI���ʒu��Ԃ�
				return GetLineStart(lineIndex) + GetLineLengthWithLineEnd(lineIndex);
			}
		}
	}
	/*#699
	//�ŏI�s��LineEnd���܂ނȂ�����P�s�A�s����ǉ�����
	if( GetLineExistLineEnd(GetLineCount()-1) == true )
	{
		AddLineInfo();
		CalcLineInfo(inText,GetLineCount()-1,inFontName,inFontSize,inAntiAliasing,inTabWidth,inIndentWidth,inWrapMode,inWrapLetterCount,inViewWidth,inCountAs2Letters,
					textDrawData,textDrawDataStartOffset);//B0000 �s�܂�Ԃ��v�Z������
	}
	*/
	//�K�v�Ȃ��EOF��s��ǉ� #699
	if( inAddEmptyEOFLineIfNeeded == true )
	{
		AddEmptyEOFLineIfNeeded();
	}
	//�e�L�X�g�̌v�Z�ςݏI���ʒu�i���e�L�X�g�����j��Ԃ�
	return inText.GetItemCount();
}

const ANumber kWordWrapWindowLeftMargin = 2;//B0375 �L�����b�g�������Ȃ��Ȃ���̑΍�

//�w��s��LineInfo���v�Z�A�X�V����
void	ATextInfo::CalcLineInfo( const AText& inText, const AIndex inLineIndex, 
		/*win const AFont inFont*/const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
		const AItemCount inTabWidth, const AItemCount inIndentWidth, const AWrapMode inWrapMode, const AItemCount inWrapLetterCount, const ANumber inViewWidth, //#117
		const ABool inCountAs2Letters, 
		CTextDrawData& ioTextDrawData, AIndex& ioTextDrawDataStartOffset )//B0000 �s�܂�Ԃ��v�Z������
{
	//lineStart��O�̍s�̏�񂩂�Z�o
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
				//���̉��s�R�[�h�܂ł�TextDrawData���擾
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
			//------------------Line break�v�Z------------------
			AItemCount	endOffset = 0;
			if( ioTextDrawData.UTF16DrawText.GetItemCount() > 0 )
			{
				endOffset = CWindowImp::GetLineBreak(ioTextDrawData,ioTextDrawDataStartOffset,
													 inFontName,inFontSize,inAntiAliasing,inViewWidth-kWordWrapWindowLeftMargin,//B0375
													 (inWrapMode==kWrapMode_LetterWrap));//#1113
			}
			//����UTF-8�����̈ʒu�ɑΉ�����offset�ɂ���i�r����break�����ꍇ�A���̕����̍ŏ��܂Ŗ߂�j
			//���Ƃ��΁A����R�[�h�\��<sp>00<sp><sp>00<sp>�ŁA<sp>00<sp><sp>�܂ł���s�Ɣ��肳�ꂽ�ꍇ�ł��A<sp>00<sp>�܂ł�܂�Ԃ��Ƃ��邽��
			endOffset = ioTextDrawData.OriginalTextArray_UnicodeOffset.Get(ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(endOffset));
			//
			AItemCount	length = ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(endOffset) 
						- ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(ioTextDrawDataStartOffset);
			//���s�R�[�h������΁A�����܂ł���s�Ƃ��A�t���O��t����
			if( lineStart + length < inText.GetItemCount() )
			{
				if( inText.Get(lineStart+length) == kUChar_LineEnd )
				{
					length++;
					mLineInfo.SetLineInfo_ExistLineEnd(inLineIndex,true);
				}
			}
			//B0000�������[�v�h�~�i�s�܂�Ԃ�0�����������ꍇ�A1�������j
			if( length <= 0 && mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex) == false )
			{
				//UTF16DrawTextArray_OriginalTextIndex��ioTextDrawDataStartOffset����n�߂āA�ŏ��ɒl���ς��Ƃ����܂�Ԃ��n�_�Ƃ���
				//����R�[�h�\��<sp>00<sp>�Ȃǂ̂Ƃ��AUTF16DrawTextArray_OriginalTextIndex�́A0,0,0,0,1�ȂǂƂȂ��Ă���B�P����endOffset++�ł�NG
				for( endOffset = ioTextDrawDataStartOffset; endOffset < ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.GetItemCount(); endOffset++ )
				{
					length = ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(endOffset) 
							- ioTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(ioTextDrawDataStartOffset);
					if( length > 0 )   break;
				}
			}
			//�s��������̖������[�v�h�~����
			if( length == 0 )
			{
				_ACError("",NULL);
				//���̉��s�R�[�h�܂ł���s�Ƃ���
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
			//length�ݒ�
			mLineInfo.SetLineInfo_Length(inLineIndex,length);
			//ioTextDrawDataStartOffset�X�V
			if( endOffset >= ioTextDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) )
			{
				//TextDrawData�̃f�[�^�����ׂēǂݏI������Ƃ���Invalid�ɂ��遨����A�V����TextDrawData���擾����
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
			//���[�h���b�v����
			if( inWrapMode == kWrapMode_WordWrapByLetterCount )//#1113
			{
				AIndex	pos = lineStart+length;
				if( mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex) == false && pos < inText.GetItemCount() && pos > 0 )
				{
					AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
					//
					AStTextPtr	textptr(inText,0,inText.GetItemCount());
					//���̎��_��pos�͎��̍s�̊J�n����
					AUCS4Char	curChar = 0;
					AText::Convert1CharToUCS4(textptr.GetPtr(),textptr.GetByteCount(),pos,curChar);
					AIndex	prevPos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(inText,pos);
					AUCS4Char	prevChar = 0;
					AText::Convert1CharToUCS4(textptr.GetPtr(),textptr.GetByteCount(),prevPos,prevChar);
					//�v�Z�����s�����P��̓r���ɂȂ��Ă��܂������ǂ����̃`�F�b�N
					if( unicodeData.IsAlphabetOrNumber(prevChar) == true && 
								unicodeData.IsAlphabetOrNumber(curChar) == true )
					{
						//�P�������O�ɖ߂��āA�A���t�@�x�b�g�ȊO�ɂȂ�ӏ���T��
						AIndex	newpos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(inText,pos);//���̍s�̍Ō�̕����ʒu����
						for( ; newpos > lineStart; newpos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(inText,newpos) )
						{
							AUCS4Char	ch = 0;
							AText::Convert1CharToUCS4(textptr.GetPtr(),textptr.GetByteCount(),newpos,ch);
							if( unicodeData.IsAlphabetOrNumber(ch) == false )
							{
								//�A���t�@�x�b�g�ȊO�̈ʒu��break
								break;
							}
						}
						if( newpos > lineStart )
						{
							//pos���A���t�@�x�b�g�ȊO�̕����̒���ɐݒ�
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
	//�������P�ꐔ�v�Z
	CalcWordCount(inText,inLineIndex);
#if 0
	Int16	winWidth = localFrame.right-localFrame.left-5;
	
	OSStatus	status;
	
	if( /*R-abs !mLCWrap*/GetWrapMode() != kWrapMode_WordWrapByLetterCount )//�E�C���h�E�����b�v
	{
		//�ŏI�s�ŕ�����1�����������ꍇ�̓����O�X�O�ɐݒ肵�ďI��
		if( lineStart >= mLength )
		{
			(*(mLineInfos->mLineInfosH))[inLineNumber].length = 0;
			return;
		}
		//����CR�܂ł̕�����Unicode�ɕϊ�����lineBuffer�ɓ����
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
			//�O��v�Z����mWrapLineBuffer�����̂܂܎g��
		}
		else
		{
			mWrapLineBuffer.DeleteAll();
			mText->Lock();
			UText::UTF8ToUnicodeTab2Space(&((*(mText->mTextH))[lineStart]),lineLen,/*R-abs mTabWidth*/GetTabWidth(),mWrapLineBuffer,/*R-abs (mPixelTabWidth==0)*/true);//R0008
			mText->Unlock();
			mWrapLineBufferStart = lineStart;
		}
		
		//ATSUBreakLine�ɂ��v�Z
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
	else//������Wrap
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
		
		//B0214�b�� �{����UCFindTextBreak�Ƃ����g���ׂ�
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
�s�����v�Z����i�����͑S�č폜���A�V�K�ɐ����j�iView�Ȃ��ENoWrap only�j
*/
void	ATextInfo::CalcLineInfoAllWithoutView( const AText& inText, const ABool& inAbort )
{
	//�s���S�폜
	//#695 AArray<AUniqID>	deletedIdentifiers;
	DeleteLineInfoAll(/*#695 deletedIdentifiers*/);
	
	//#695 �e�L�X�g���̍s������LineInfo�̃��������蓖�đ����X�e�b�v�ɐݒ�
	//�i������ALineInfo�ɂ�kLineInfoAllocationStep��菬�����͐ݒ肳��Ȃ��j
	AItemCount	lineEndCharCount = inText.GetCountOfChar(kUChar_LineEnd,0,inText.GetItemCount());
	mLineInfo.SetReallocateStep(lineEndCharCount);
	
	//�s���ɍs����ǉ��A�v�Z���Ă���
	for( AIndex lineIndex = 0; ; lineIndex++ )
	{
		//Abort�t���OON�Ȃ�I�� #699
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
	//�ŏI�s��LineEnd���܂ނȂ�����P�s�A�s����ǉ�����
	if( GetLineExistLineEnd(GetLineCount()-1) == true )
	{
		AddLineInfo();
		CalcLineInfoWithoutView(inText,GetLineCount()-1);
	}
	*/
	//�K�v�Ȃ��EOF��s��ǉ� #699
	AddEmptyEOFLineIfNeeded();
}

//win
/**
�w��s��LineInfo���v�Z�A�X�V����iView�Ȃ��ENoWrap only�j
*/
void	ATextInfo::CalcLineInfoWithoutView( const AText& inText, const AIndex inLineIndex )
{
	//lineStart��O�̍s�̏�񂩂�Z�o
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

// �֑�������t�������Ă���
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
	//�֑������t�������^�폜
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
	// ���p�A�S�p�X�y�[�X�A���p�^�u�t������
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
	//lineinfo�X�V
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
EOF�s�����i�ŏI�s�����s���܂ނȂ�Ō�ɋ�s��ǉ�����j
*/
ABool	ATextInfo::AddEmptyEOFLineIfNeeded()
{
	//�ŏI�s�����s���܂ނȂ�Ō�ɋ�s��ǉ�����
	//�ŏI�s�͏�ɉ��s�����̍s�ɂȂ�B
	
	//�s��0�Ȃ�I��
	if( GetLineCount() == 0 )   return false;
	
	//�ŏI�s�����s���܂ނ��ǂ����̔���
	AIndex	lineIndex = GetLineCount()-1;
	if( mLineInfo.GetLineInfo_ExistLineEnd(lineIndex) == true )
	{
		//�ŏI�s�����s���܂ނȂ�A������s�ǉ�
		AddLineInfo();
		lineIndex++;
		//�s�f�[�^�ݒ�
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

//�s����lineStart�����炷
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
�w��s�ȍ~�̒i�������炷
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

//�w��s����̍s�ŁA�i�����n�܂�s�̃C���f�b�N�X�𓾂�
AIndex	ATextInfo::GetNextParagraphStartLineIndex( const AIndex inLineIndex ) const
{
	AItemCount	lineCount = GetLineCount();//B0358 �������̂��ߕϐ��Ɋi�[
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

//�sIndex����i��Index���擾
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

//�i��Index����sIndex���擾
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
		//���ʂ��]���̊֐��̌��ʂƍ��킹�Ă���
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
�s�v�Z�X���b�h���ʂ��A���C���X���b�h��TextInfo�փR�s�[����
�R�s�[��i���C���X���b�h���j�̃I�u�W�F�N�g����R�[�������
@param inLineIndex �X���b�h�v�Z�Ώۍs��index
*/
ABool	ATextInfo::CopyFromCalculatorResult( const AIndex inLineIndex, const ATextInfo& inSrcTextInfo )
{
	//�s�v�Z�����t���O
	ABool	completed = false;
	//�X���b�h�v�Z�Ώۍs��LineStart, Len, ExistLineEnd���擾
	AIndex	originalLineStart = mLineInfo.GetLineInfo_Start(inLineIndex);
	AItemCount	originalLen = mLineInfo.GetLineInfo_Length(inLineIndex);
	ABool	originalExistLineEnd = mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex);
	//�X���b�h�v�Z�Ώۍs�̒i��index���擾
	AIndex	paragraphIndex = 0;
	if( inLineIndex > 0 )
	{
		paragraphIndex = mLineInfo.GetLineInfo_ParagraphIndex(inLineIndex-1);
		if( mLineInfo.GetLineInfo_ExistLineEnd(inLineIndex-1) == true )
		{
			paragraphIndex++;
		}
	}
	//textIndex: �R�s�[�挻�ݍs�̌��݂̊J�ntext index
	AIndex	textIndex = originalLineStart;
	//dstLineIndex: �R�s�[�挻�ݍsindex
	AIndex	dstLineIndex = inLineIndex;
	//srcLineCount: �R�s�[����s��
	AItemCount	srcLineCount = inSrcTextInfo.GetLineCount();
	//�R�s�[��ɍs���ꊇ�ǉ��i�X���b�h�v�Z�Ώۍs�̌��ɒǉ�����j
	mLineInfo.InsertLines(dstLineIndex+1,srcLineCount);
	//�R�s�[���s���ƂɃ��[�v
	for( AIndex srcLineIndex = 0; srcLineIndex < srcLineCount; srcLineIndex++ )
	{
		//Len, ExistLineEnd, paragraph��ݒ聨���̍s�ֈړ���LineStart��ݒ�Ƃ����������ɂȂ��Ă���
		//���[�v�I�����AdstLineIndex�́A�V���ȃX���b�h�v�Z�Ώۍs�i�܂��́A�s�v�Z�������͗]���Ȉ�s�j�̈ʒu�ɂȂ�B
		
		//Len, ExsitLineEnd���R�s�[�Bparagraph��ݒ�B
		AItemCount	len = inSrcTextInfo.mLineInfo.GetLineInfo_Length(srcLineIndex);
		mLineInfo.SetLineInfo_Length(dstLineIndex,len);
		ABool	existLineEnd = inSrcTextInfo.mLineInfo.GetLineInfo_ExistLineEnd(srcLineIndex);
		mLineInfo.SetLineInfo_ExistLineEnd(dstLineIndex,existLineEnd);
		mLineInfo.SetLineInfo_ParagraphIndex(dstLineIndex,paragraphIndex);
		//�P�ꐔ�E�������R�s�[
		mLineInfo.SetLineInfoP_CharCount(dstLineIndex,inSrcTextInfo.mLineInfo.GetLineInfoP_CharCount(srcLineIndex));
		mLineInfo.SetLineInfoP_WordCount(dstLineIndex,inSrcTextInfo.mLineInfo.GetLineInfoP_WordCount(srcLineIndex));
		//fprintf(stderr,"srcline:%ld dstline:%ld para:%ld start:%ld len:%ld\n",srcLineIndex,dstLineIndex,paragraphIndex,
		//			mLineInfo.GetLineInfo_Start(dstLineIndex),len);
		//���s����Ȃ�paragraph���{�P
		if( existLineEnd == true )
		{
			paragraphIndex++;
		}
		
		//���̍s�̊J�ntext index���v�Z
		textIndex += len;
		//�R�s�[�挻�ݍs��ݒ�
		dstLineIndex++;
		//LineStart��ݒ�
		mLineInfo.SetLineInfo_Start(dstLineIndex,textIndex);
	}
	//AdjustLineInfo()��EOF��s�ǉ����Ă���ꍇ�A���̍s�̒i���ԍ��͓r���i�K�ł̒i���ԍ��ɂȂ��Ă���̂ŁA�����ōX�V����B
	//dstLineIndex�̍s�̓X���b�h�v�Z�Ώۍs�Ȃ̂ŁAEOF��s�����݂���Ƃ�����A���̎��̍s�B
	if( dstLineIndex+1 < GetLineCount() )
	{
		mLineInfo.SetLineInfo_ParagraphIndex(dstLineIndex+1,paragraphIndex);
	}
	//
	//fprintf(stderr,"inLineIndex:%ld lineStart:%ld textIndex:%ld endTextIndex:%ld srcLineCount:%ld\n",
	//			inLineIndex,originalLineStart,textIndex,endTextIndex,srcLineCount);
	//���̃X���b�h�v�Z�Ώۍs�̌v�Z
	//remainLen�Ɏc��̃e�L�X�g�������v�Z
	AItemCount	remainLen = originalLen - (textIndex-originalLineStart);
	if( remainLen > 0 )
	{
		//�����������܂�����Ȃ�A�X���b�h�v�Z�Ώۍs�Ƀf�[�^�ݒ�
		mLineInfo.SetLineInfo_Length(dstLineIndex,remainLen);
		mLineInfo.SetLineInfo_ExistLineEnd(dstLineIndex,originalExistLineEnd);
		mLineInfo.SetLineInfo_ParagraphIndex(dstLineIndex,paragraphIndex);
	}
	else if( remainLen == 0 )
	{
		//�I�������̂ŃX���b�h�v�Z�Ώۍs�͍폜
		mLineInfo.Delete1Line(dstLineIndex);
		//EOF�s����
		AddEmptyEOFLineIfNeeded();
		//�I���t���OOn
		completed = true;
	}
	else
	{
		//remainLen<0�ƂȂ邱�Ƃ͂Ȃ��̂ŁA�G���[
		_ACError("",NULL);
		completed = true;
	}
	return completed;
}

//#142
/**
�s���P�ꐔ�E�������v�Z
*/
void	ATextInfo::CalcWordCount( const AText& inText, const AIndex inLineIndex )
{
	//�s���擾
	AIndex	lineStart = GetLineStart(inLineIndex);
	AItemCount	len = GetLineLengthWithLineEnd(inLineIndex);
	//���ʏ�����
	AItemCount	charCount = 0;
	AItemCount	wordCount = 0;
	AItemCount	paragraphCount = 0;
	//�e�L�X�g�͈͓��P�ꐔ�E�������擾
	GetWordCount(inText,lineStart,len,charCount,wordCount,paragraphCount, true);//#1403
	//���ʂ��s���ɐݒ�
	mLineInfo.SetLineInfoP_CharCount(inLineIndex,charCount);
	mLineInfo.SetLineInfoP_WordCount(inLineIndex,wordCount);
}

//#142
/**
�e�L�X�g�͈͓��P�ꐔ�E�������v�Z
*/
void	ATextInfo::GetWordCount( const AText& inText, const AIndex inStartIndex, const AItemCount inLength,
		AItemCount& outCharCount, AItemCount& outWordCount, AItemCount& outParagraphCount, const ABool inAdjustForLineInfo ) const//#1403
{
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	const AUChar*	ptr = textptr.GetPtr();
	const AItemCount	textlen = textptr.GetByteCount();
	//���ʏ�����
	outCharCount = 0;
	outWordCount = 0;
	outParagraphCount = 0;
	//���[�h�t���O
	ABool	alphabetMode = false;
	ABool	charExistAfterLineEndChar = false;
	//�e���������[�v
	AIndex	epos = inStartIndex+inLength;
	for( AIndex pos = inStartIndex; pos < epos; )
	{
		//������
		outCharCount++;
		//��OS�֐��g�p����
		//��"_"���A���t�@�x�b�g�ɂ��ׂ��H���Ȃ��ׂ��H
		//�����擾
		AUCS4Char	ch = 0;
		AItemCount bc = AText::Convert1CharToUCS4(ptr,textlen,pos,ch);
		//�i���J�E���g
		if( ch == kUChar_LineEnd )
		{
			//�i����
			outParagraphCount++;
			//���s��ɕ��������݂��Ă��邩�ǂ����̃t���O��������
			charExistAfterLineEndChar = false;
		}
		else
		{
			//���s��ɕ��������݂��Ă��邩�ǂ����̃t���O��ON
			charExistAfterLineEndChar = true;
		}
		//�P�ꔻ��
		//#1402 ABool	isAlphabet = unicodeData.IsAlphabetOrNumber(ch);
		//#1402 It's����1�P��ɂȂ�Ȃ��΍�Ƃ��ēr���̃A�|�X�g���t�B�A�n�C�t���A�s���I�h�ŃA���t�@�x�b�g���[�hOFF�ɂ��Ȃ��悤�ȑ΍�������������AWord���ł́A�P���ɁA��{�I�ɃX�y�[�X�������ۂ��ŒP�ꔻ������Ă���悤�Ȃ̂ŁA������ɍ��킹��B�i���Ƃ��΁A" test[1] "��1�ꈵ���A" ( "��1�ꈵ���ƂȂ��Ă���A��a�������邪�Aweb������������AWord�̒P�ꐔ�J�E���g�@�\�̓l�C�e�B�u�ł��L���g���Ă���悤�Ȃ̂ŁA������ɍ��킹��B�jWord�ł͓��{��S�p�̈����͈قȂ�i�ꕶ����P��j�悤�����A���{��ł̒P�ꐔ�J�E���g�͕��ʂ��Ȃ����A�ꕶ����P������������̂ŁA�X�y�[�X�ȊO�̘A������P��Ƃ���B�L���̈������A��߂�ꂽ�W���͂Ȃ��悤�ŁA�����Ƀc�[���ɂ���ĈقȂ��Ă���B
		ABool	isSpace = (ch == 0x0020 || ch == 0x0009 || ch == 0x000A || ch == 0x000D);
		ABool	isAlphabet = (isSpace == false);
		//����F�A���t�@�x�b�g�A���̑O�F�A���t�@�x�b�g�ȊO
		if( isAlphabet == true && alphabetMode == false )
		{
			//�P�ꐔ
			outWordCount++;
			//�A���t�@�x�b�g���[�hON
			alphabetMode = true;
		}
		//����F�A���t�@�x�b�g�ȊO�A���̑O�F�A���t�@�x�b�g
		if( isAlphabet == false && alphabetMode == true )
		{
			//�A���t�@�x�b�g���[�hOFF
			alphabetMode = false;
		}
		//���̕���
		pos += bc;
	}
	//#1403
	//LineInfo�Ɋi�[����ꍇ�i�e�L�X�g���́u�S�́v�p�j�́A�P��r���Ő܂�Ԃ��������Ă���ꍇ�̒������s��
	if( inAdjustForLineInfo == true )
	{
		if( epos < inText.GetItemCount() )
		{
			//�s�̍Ō�̕������A���t�@�x�b�g�A���A���̕������A���t�@�x�b�g�̏ꍇ�́A1�P��Ƃ��ăJ�E���g�����ׂ��Ȃ̂ŁA
			//���̍s�̃J�E���g��-1���Ă����B
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
	//���s�R�[�h��̌�ɕ��������݂��Ă����ԂŏI���Ȃ�A�i�����������P�ǉ�
	if( charExistAfterLineEndChar == true )
	{
		outParagraphCount++;
	}
}

#pragma mark ===========================

#pragma mark ---���@�F��

const AItemCount	kTokenStackMaxCount = 256;

//#698 ���@�F���X���b�h���̂��߁ARecognizeSyntax()��Prepare/Execute�ɕ���

/**
�S�s���@�F��
*/
void	ATextInfo::RecognizeSyntaxAll( const AText& inText, const AText& inFileURL, 
									   AArray<AUniqID>& outAddedGlobalIdentifiers, AArray<AIndex>& outAddedGlobalIdentifiersLineIndex,
									   ABool& outImportChanged, const ABool& inAbort )//#998
{
	//�F���R���e�L�X�g����
	PrepareRecognizeSyntaxContext(inText,0,inFileURL);//#998
	//�F�����s
	AArray<AUniqID>	deletedIdentifiers;
	ExecuteRecognizeSyntax(inText,kIndex_Invalid,
				deletedIdentifiers,outAddedGlobalIdentifiers,outAddedGlobalIdentifiersLineIndex,outImportChanged,inAbort);
	//�F���R���e�L�X�g�폜
	ClearRecognizeSyntaxContext();
}

/**
���@�F���R���e�L�X�g����
*/
AIndex	ATextInfo::PrepareRecognizeSyntaxContext( const AText& inText, const AIndex inStartLineIndex, const AText& inFileURL )//#998
{
	//#693 RegExp�I�u�W�F�N�g��Purge����Ă�����ēxRegExp���X�V
	if( mRegExpObjectPurged == true )
	{
		UpdateRegExp();
	}
	
	//
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(mModeIndex);
	ASyntaxDefinition&	syntaxDefinition = modePrefDB.GetSyntaxDefinition();
	
	//======================== Context�ϐ��ݒ� ========================
	
	//�ϐ����X�g������
	/*#698 �����o�ϐ���
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
	
	//TokenStack������
	//TokenStack�̎��̂́AStartPos, Length�̃����O�o�b�t�@(kTokenStackMaxCount��)�B�e�L�X�g���e�͕ۑ����Ȃ��B
	//SetVarWithTokenStack�ɂ��A�Ή�����TokenStack�̈ʒu�̏�񂪕ϐ��ɃR�s�[�����B
	//mRecognizeContext_CurrentTokenStackPosition�́AClearTokenStack�����܂ŁA�����ƃC���N�������g�����B�z��A�N�Z�X����%kTokenStackMaxCount���Z����B
	//ClearTokenStack�ɂ��AmRecognizeContext_CurrentTokenStackPosition��0�ɖ߂��B
	/*#698 �����o�ϐ���
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
	
	//==================�I���\�s����==================
	//�I���\�s�͊J�n�s�̎��̍s�Ƃ���i�J�n�s�����O�ɂ���ƁA�X���b�h�ɂ�镶�@�F�������x�������Ƃ�����J��Ԃ��\��������B�j
	mRecognizeContext_EndPossibleLineIndex = inStartLineIndex+1;
	
	//==================�J�n�s����==================
	//�iinStartLineIndex�܂ł̍s�iinStartLineIndex���܂ށj�������̂ڂ��āA�ŏ���idle��������stable�ȏ�ԂɂȂ�s����n�߂�j
	//idle�܂Ŗ߂�Ȃ��ƁA���ʎq�F�����r������ɂȂ��Ă��܂��A�֐��������F���ł��Ȃ��B
	//�����K�v�F#pragma mark�s�@�̕ҏW���̑��x��
	//B0379 �i���̍ŏ��̍s���擾
	mRecognizeContext_RecognizeStartLineIndex = inStartLineIndex - kLimit_RecognizeSyntax_MinLineCountToGoBackRecognizeStart;//#905 �Œ�8�s�߂�
	if( mRecognizeContext_RecognizeStartLineIndex < 0 )   mRecognizeContext_RecognizeStartLineIndex = 0;
	for( ; mRecognizeContext_RecognizeStartLineIndex > 0 ; mRecognizeContext_RecognizeStartLineIndex-- )
	{
		if( IsParagraphStartLine(mRecognizeContext_RecognizeStartLineIndex) == true )   break;
	}
	//�J�n�i���̍ŏ��̍s�̔F���t���O�𗎂Ƃ�
	//mLineInfo.ClearRecognizeFlags(mRecognizeContext_RecognizeStartLineIndex);//#693
	//------------------�ŏ���idle��������stable�ȏ�ԂɂȂ�s�܂Ŗ߂�------------------
	//#695 64�s�ȏ�͖߂�Ȃ��悤�ɂ���
	AIndex	startLimit = mRecognizeContext_RecognizeStartLineIndex - kLimit_RecognizeSyntax_MaxLineCountToGoBackRecognizeStart;
	if( startLimit < 0 )
	{
		startLimit = 0;
	}
	//�ǂ��܂Ŗ߂��ĉ�͂��邩�����肵�AmRecognizeContext_RecognizeStartLineIndex�Ɋi�[
	for( ; mRecognizeContext_RecognizeStartLineIndex > /*#695 0*/startLimit; mRecognizeContext_RecognizeStartLineIndex-- )
	{
		AIndex	stateIndex = mLineInfo.GetLineInfoP_StateIndex(mRecognizeContext_RecognizeStartLineIndex);
		if( (stateIndex == 0 || syntaxDefinition.GetSyntaxDefinitionState(stateIndex).IsStable() == true) &&
					mLineInfo.GetLineInfoP_DirectiveLevel(mRecognizeContext_RecognizeStartLineIndex) == 0 )//Directive���x����0�ɂȂ�s
		{
			break;
		}
	}
	//fprintf(stderr,"s:%ld ",mRecognizeContext_RecognizeStartLineIndex);
	
	//======================== �e�ϐ����̊J�n��BlockStartData�ݒ� ========================
	//#695
	for( AIndex i = 0; i < syntaxDefinition.GetVariableCount(); i++ )
	{
		if( syntaxDefinition.GetVariableBlockStartFlag(i) == true )
		{
			//�F���J�n�s�i�K�ł�BlockStart�C���f���g���擾���A�R���e�L�X�g�ɐݒ肷��
			mRecognizeContext_Variable_BlockStartIndent.Set(i,
						FindLastBlockStartIndentCount(mRecognizeContext_RecognizeStartLineIndex-1,i));
		}
	}
	
	//======================== ���Context�ݒ� ========================
	
	//�f�t�H���g���@�p�[�g�擾
	AText	defaultSyntaxPart;
	modePrefDB.GetData_Text(AModePrefDB::kDefaultSyntaxPart,defaultSyntaxPart);
	
	//������Ԑݒ�
	/*#698AIndex	*/mRecognizeContext_CurrentStateIndex = syntaxDefinition.GetInitialState(inFileURL,defaultSyntaxPart);//R0000 #998
	/*#698AIndex	*/mRecognizeContext_PushedStateIndex = mRecognizeContext_CurrentStateIndex;//R0000
	if( mRecognizeContext_RecognizeStartLineIndex > 0 )
	{
		//�sIndex>0�̏ꍇ�́A�J�n�s�̊J�n��Ԃ͐ݒ�ς݂̂͂��B
		mRecognizeContext_CurrentStateIndex = mLineInfo.GetLineInfoP_StateIndex(mRecognizeContext_RecognizeStartLineIndex);
		mRecognizeContext_PushedStateIndex = mLineInfo.GetLineInfoP_PushedStateIndex(mRecognizeContext_RecognizeStartLineIndex);
	}
	/*#698 AIndex	*/mRecognizeContext_CurrentIndent = mLineInfo.GetLineInfoP_IndentCount(mRecognizeContext_RecognizeStartLineIndex);
	
	//======================== Directive�pContext�ݒ� ========================
	
	//#467 Directive�F��
	//directive�F���p�f�[�^�擾
	/*#698 AIndex	*/mRecognizeContext_CurrentDirectiveLevel = 0;
	mRecognizeContext_NextDirectiveLevel = 0;
	/*#698 AIndex	*/mRecognizeContext_CurrentDisabledDirectiveLevel = kIndex_Invalid;
	mRecognizeContext_NextDisabledDirectiveLevel = kIndex_Invalid;
	ABool	recognizeDirective = modePrefDB.GetData_Bool(AModePrefDB::kRecognizeConditionalCompileDirective);
	//���ݍs��directive��Ԃ��擾
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
���@�F�����s
*/
AIndex	ATextInfo::ExecuteRecognizeSyntax( const AText& inText, const AIndex inEndLineIndex,
		AArray<AUniqID>& outDeletedIdentifiers, 
		AArray<AUniqID>& outAddedGlobalIdentifiers, AArray<AIndex>& outAddedGlobalIdentifiersLineIndex,
		ABool& outImportChanged, const ABool& inAbort )
{
	//�Ԃ�l������
	outDeletedIdentifiers.DeleteAll();
	outAddedGlobalIdentifiers.DeleteAll();
	outAddedGlobalIdentifiersLineIndex.DeleteAll();
	outImportChanged = false;
	
	//#693 RegExp�I�u�W�F�N�g��Purge����Ă�����ēxRegExp���X�V
	if( mRegExpObjectPurged == true )
	{
		UpdateRegExp();
	}
	
	//======================== �e��f�[�^�����[�J���ϐ��ɔ[�߂� ========================
	
	//
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(mModeIndex);
	ASyntaxDefinition&	syntaxDefinition = modePrefDB.GetSyntaxDefinition();
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();//#664
	
	//�estate���ɃR�����g��Ԃ��ǂ�����array
	AArray<ABool>	isCommentStateArray;
	for( AIndex i = 0; i < syntaxDefinition.GetStateCount(); i++ )
	{
		isCommentStateArray.Add(syntaxDefinition.GetSyntaxDefinitionState(i).IsCommentState());
	}
	
	//
	ABool	recognizeDirective = modePrefDB.GetData_Bool(AModePrefDB::kRecognizeConditionalCompileDirective);
	AColor	directiveDisabledColor = kColor_Gray;
	modePrefDB.GetData_Color(AModePrefDB::kConditionalCompileDisabledColor,directiveDisabledColor);
	
	//define�l�z��
	AHashTextArray	enabledDefineArray;
	ATextArray	enabledDefineValueArray;
	//"#if 1"��enable�����ɂ���
	AText	t;
	t.SetCstring("1");
	enabledDefineArray.Add(t);
	enabledDefineValueArray.Add(t);
	//define�l�z��ɐݒ�f�[�^���R�s�[�i�L���b�V���j����
	for( AIndex i = 0; i < GetApp().NVI_GetAppPrefDB().GetItemCount_Array(AAppPrefDB::kDirectiveEnabledDefine); i++ )
	{
		AText	text;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefine,i,text);
		enabledDefineArray.Add(text);
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kDirectiveEnabledDefineValue,i,text);
		enabledDefineValueArray.Add(text);
	}
	/*#698 ���ɂ܂Ƃ��ɓ��삵�Ă��Ȃ������Ȃ̂ō폜
	//
	ATextIndex	recogStartTextIndex = GetTextIndexFromTextPoint(inStartTextPoint);
	AIndex	recogStartStateIndex = kIndex_Invalid;
	//B0339 2.1.8b2���_�ł܂Ƃ��ɓ��삵�Ă��Ȃ������̂�nomemory�͍폜 ABool	noMemoryStateFlag = false;
	*/
	
	/*#698 ���ɁAmRecognizeContext_RecognizeStartLineIndex�ȍ~�S�Ă̍s��DeleteLineIdentifiers()���s�����悤�ɂȂ����̂ŁA
	identifierClearedLine�𗘗p��������i���ʎq������폜�����s���ǂ����̔���j�͕s�v�BmRecognizeContext_RecognizeStartLineIndex�𗘗p����������@�ɕύX
	identifierClearedLine�͎g�p���Ȃ��B�������ʁE�������x�����������Ȃ����߁B
	AHashArray<AIndex>	identifierClearedLine;
	*/
	//======================== �e�s���[�v�J�n ========================
	AIndex	tokenspos;
	AText	token;
	AIndex	lineIndex = mRecognizeContext_RecognizeStartLineIndex;
	ABool	stateStart = true;//RC2
	AIndex	reservedNextLinePushState = kIndex_Invalid;
	//�I���s��ݒ�#698
	AIndex	endLineIndex = inEndLineIndex;
	if( endLineIndex == kIndex_Invalid )
	{
		endLineIndex = GetLineCount();
	}
	//#695 ���ɔF���ς݂̍s���ĔF��������
	AItemCount	recognizedLineCount = 0;
	//
	for( ; lineIndex < /*#698 GetLineCount()*/endLineIndex; lineIndex++ )
	{
		//Abort�t���OON�Ȃ�I�� #699
		if( inAbort == true )
		{
			return lineIndex;
		}
		
		//�O�s�̏�����PushStateFromNextLine�A�N�V���������s�����Ƃ��́AreservedNextLinePushState�ɍ���̍s��push state�����s����
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
		//�t�H�[���f�B���O���x��������
		AFoldingLevel	foldingLevel = kFoldingLevel_None;
		
		//#907
		//���݂̕��@�p�[�gindex���擾
		AIndex	lineStartSyntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(mRecognizeContext_CurrentStateIndex).GetStateSyntaxPartIndex();
		
		//fprintf(stderr,"recog:%ld ",lineIndex);
		ABool	recognizedLine = mLineInfo.GetLineInfoP_Recognized(lineIndex);
		
		//======================== �I������ ========================
		if( lineIndex >= /*B0379 inStartTextPoint.y*/mRecognizeContext_EndPossibleLineIndex )
		{
			//#695
			//���ɔF���ς݂̍s���ĔF�����������J�E���g�B�i�g���K�[�ʒu�ȍ~�̂݃J�E���g�j
			//kLimit_ExecuteRecognizeSyntax_MaxLineCountAfterRecognizedFlagTrue���������Ȃ�����F���I���ƂȂ�B
			if( recognizedLine == true )
			{
				recognizedLineCount++;
			}
			
			//���ݏ�Ԃ�idle�A���A�F���ς݁A���A��ԁ^pushed��Ԃ���v���Ă���΁A�����ŔF���I���B�i�F���r���̎��ʎq�͂Ȃ��A�܂��A����ȏ�F�����Ă��S���������ʂɂȂ�B�j
			if( recognizedLine == true && mLineInfo.GetLineInfo_ExistLineEnd(lineIndex) == true )
			{
				const ASyntaxDefinitionState&	stateobj = syntaxDefinition.GetSyntaxDefinitionState(mRecognizeContext_CurrentStateIndex);
				if(	(/*���x����̂��߁u���ݏ�Ԃ�0�܂���stable��ԁv�������R�����g�A�E�g�B�֐��O�̃w�b�_�R�����g��ҏW���Ɋ֐����o��������X�V�����̂ŁB������ς蕜���B�֐����F������Ȃ����Ƃ�����*/
					((mRecognizeContext_CurrentStateIndex == 0)||(stateobj.IsStable() == true))  //���ݏ�Ԃ�0�܂���stable���
					&&((mLineInfo.GetLineInfoP_StateIndex(lineIndex) == mRecognizeContext_CurrentStateIndex  //�s�̏�ԁEpushed��Ԃ����ݏ�Ԃƈ�v
					&&mLineInfo.GetLineInfoP_PushedStateIndex(lineIndex) == mRecognizeContext_PushedStateIndex))
					&&mRecognizeContext_CurrentIndent == mLineInfo.GetLineInfoP_IndentCount(lineIndex)//#467 )//B0339�C���f���g��v��������Ă݂�
					&&mRecognizeContext_CurrentDirectiveLevel == mLineInfo.GetLineInfoP_DirectiveLevel(lineIndex)//#467 directive��v����
					&&mRecognizeContext_CurrentDisabledDirectiveLevel == mLineInfo.GetLineInfoP_DisabledDirectiveLevel(lineIndex))//#467 directive��v����
					||
					((stateobj.IsGlobalState() == false)
					&&(mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex) != /*#216 kIndex_Invalid*/kUniqID_Invalid )
					&&(mLineInfo.GetLineInfoP_StateIndex(lineIndex) == 0)&&(mLineInfo.GetLineInfoP_IndentCount(lineIndex) == 0)//#467)
					&&mRecognizeContext_CurrentDirectiveLevel == mLineInfo.GetLineInfoP_DirectiveLevel(lineIndex)//#467 directive��v����
					&&mRecognizeContext_CurrentDisabledDirectiveLevel == mLineInfo.GetLineInfoP_DisabledDirectiveLevel(lineIndex))//#467 directive��v����
					/*B0339 ���ݏ�Ԃ�globalstate�ȊO���s��Ԃ�0�̏ꍇ�́AGlobalIdentifier�����݂���s�ŏI��*/
					//#695 �F���ςݍs�������l��葽���Ȃ������L�����𖳎����ĔF�������Ƃ���
					|| (recognizedLineCount > kLimit_ExecuteRecognizeSyntax_MaxLineCountAfterRecognizedFlagTrue) )
				{
					//#698 outRecognizedStartLineIndex = mRecognizeContext_RecognizeStartLineIndex;
					//#698 outRecognizedEndLineIndex = lineIndex;
					AIndex	recognizeEndLineIndex = lineIndex;//#698
					
					//B0340 �C���f���g���ς���Ă���ꍇ�A�C���f���g��0�ȉ��ɂȂ邩�AGlobalIdentifier�����݂���s�܂ŁA�C���f���g�����炵�Ă���
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
					//�F���ŏI�s��comment/block folding level��ݒ肷��i���[�v���͏�ɑO�s��folding level��ݒ肵�Ă��邽�߁B�j
					UpdateCommentBlockFoldingLevel(lineIndex-1,isCommentStateArray);
					
					return recognizeEndLineIndex;//#698
				}
			}
		}
		//B0339 �Ώےi����Identifier�폜�A���o���F���́A�F���J�n�s�istable�s�j����s���悤�ɂ���B
		//�i�L�����b�g�i���ȍ~�ɂ���ƁA���Ƃ��΁A�֐��ɂ��āA{���͎��ɁA�֐����̍s��Identifier�폜�����s���ꂸ�A���ʂƂ��Ċ֐��������ʎq�o�^����Ȃ�
		//{
		
		//stable�d�l�ɂ����̂ł��܂�߂�Ȃ��͂��Ȃ̂ŉE�����폜if( recognizedLine == false )
		//{
		
		//======================== �Ώےi���̎��ʎq���폜 ========================
		
		//�Ώےi����Identifier���폜 B0379�Ώۍs���Ώےi���i�i�����̑S�Ă̍s�j
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
				//�F���t���O����
				mLineInfo.ClearRecognizeFlags(i);
				//#698 mRecognizeContext_RecognizeStartLineIndex�𗘗p����������@�ɕύX�BidentifierClearedLine�͎g�p���Ȃ��B identifierClearedLine.Add(i);
				if( mLineInfo.GetLineInfo_ExistLineEnd(i) == true )   break;
			}
		}
		
		//==================�s�C���f���g�擾==================
		
		AItemCount	indentCountAtLineStart = mRecognizeContext_CurrentIndent;
		
		//======================== Directive�F�� ========================
		
		//Directive�F�� #467
		if( IsParagraphStartLine(lineIndex) == true && recognizeDirective == true &&
					syntaxDefinition.GetSyntaxDefinitionState(mRecognizeContext_CurrentStateIndex).IsDisableDirective() == false )
		{
			//directive�ł͍s���̂ݔF��
			//#695 �������̂��߁A���[�v�O��AText token���g���i���s�������擾�E����������Ȃ��悤�Ɂj�BAText	token;
			AIndex	pos = GetLineStart(lineIndex);
			AIndex	epos = pos+GetLineLengthWithLineEnd(lineIndex);
			AIndex	tokenspos = epos;
			if( GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,pos,epos,token,tokenspos,lineStartSyntaxPartIndex) == true )//#664
			{
				//Directive�J�n�g�[�N��(#�j���ǂ����𔻒�
				if( syntaxDefinition.IsDirectiveString(token) == true )
				{
					//#�̎��̃g�[�N�����擾
					if( GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,pos,epos,token,tokenspos,lineStartSyntaxPartIndex) == true )//#664
					{
						//#if
						if( syntaxDefinition.IsDirectiveIfString(token) == true )
						{
							//Directive���x���C���N�������g�i���s����j
							mRecognizeContext_NextDirectiveLevel++;
							//�����s����i���ݗL���̏ꍇ�̂ݔ���j
							if( mRecognizeContext_CurrentDisabledDirectiveLevel == kIndex_Invalid )
							{
								//�X�y�[�X�n�������Ȃ���text�֊i�[
								AText	text;
								inText.GetTextWithoutSpaces(pos,epos-pos,text);
								//enabled define����value��0�ȊO�Ȃ疳��define�Ɣ��f
								AIndex	enabledDefineArrayIndex = enabledDefineArray.Find(text);
								ABool	disabled = true;
								if( enabledDefineArrayIndex != kIndex_Invalid )
								{
									disabled = false;
									//�l����܂���"0"�Ȃ�disable
									const AText& value = enabledDefineValueArray.GetTextConst(enabledDefineArrayIndex);
									if( value.GetItemCount() == 0 || value.Compare("0") == true )
									{
										disabled = true;
									}
								}
								//�V�X�e���w�b�_�̏ꍇ�A"#if 0"�ȊO�͏��enable�ɂ���
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
							//FoldingLevel�ݒ�(If)
							foldingLevel |= kFoldingLevel_IfStart;
						}
						//#else
						else if( syntaxDefinition.IsDirectiveElseString(token) == true )
						{
							//Directive���x���f�N�������g�i���ݍs�̂݁j
							mRecognizeContext_CurrentDirectiveLevel--;
							//���ݗL���̏ꍇ�͖����ɂ���
							if( mRecognizeContext_CurrentDisabledDirectiveLevel == kIndex_Invalid )
							{
								mRecognizeContext_NextDisabledDirectiveLevel = mRecognizeContext_CurrentDirectiveLevel;
							}
							//���ݖ����̏ꍇ�͗L���ɂ���
							else
							{
								mRecognizeContext_NextDisabledDirectiveLevel = kIndex_Invalid;
							}
						}
						//#elif
						else if( syntaxDefinition.IsDirectiveElifString(token) == true )
						{
							//Directive���x���f�N�������g�i���ݍs�̂݁j
							mRecognizeContext_CurrentDirectiveLevel--;
							//�����s����i���ݗL���̏ꍇ�̂ݔ���j
							if( mRecognizeContext_CurrentDisabledDirectiveLevel == kIndex_Invalid )
							{
								//�X�y�[�X�n�������Ȃ���text�֊i�[
								AText	text;
								inText.GetTextWithoutSpaces(pos,epos-pos,text);
								//enabled define����value��0�ȊO�Ȃ疳��define�Ɣ��f
								AIndex	enabledDefineArrayIndex = enabledDefineArray.Find(text);
								ABool	disabled = true;
								if( enabledDefineArrayIndex != kIndex_Invalid )
								{
									disabled = false;
									//�l����܂���"0"�Ȃ�disable
									const AText& value = enabledDefineValueArray.GetTextConst(enabledDefineArrayIndex);
									if( value.GetItemCount() == 0 || value.Compare("0") == true )
									{
										disabled = true;
									}
								}
								//�V�X�e���w�b�_�̏ꍇ�A"#if 0"�ȊO�͏��enable�ɂ���
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
							//Directive���x���C���N�������g�i���s����j
							mRecognizeContext_NextDirectiveLevel++;
							//�����s����i���ݗL���̏ꍇ�̂ݔ���j
							if( mRecognizeContext_CurrentDisabledDirectiveLevel == kIndex_Invalid )
							{
								//�X�y�[�X�n�������Ȃ���text�֊i�[
								AText	text;
								inText.GetTextWithoutSpaces(pos,epos-pos,text);
								//enabled define�ȊO�i���V�X�e���w�b�_�ȊO�j�Ȃ疳��define�Ɣ��f
								if( enabledDefineArray.Find(text) == kIndex_Invalid && 
											mSystemHeaderMode == false )
								{
									mRecognizeContext_NextDisabledDirectiveLevel = mRecognizeContext_CurrentDirectiveLevel;
								}
							}
							//#450
							//FoldingLevel�ݒ�(If)
							foldingLevel |= kFoldingLevel_IfStart;
						}
						//#ifndef
						else if( syntaxDefinition.IsDirectiveIfndefString(token) == true )
						{
							//Directive���x���C���N�������g�i���s����j
							mRecognizeContext_NextDirectiveLevel++;
							//�����s����i���ݗL���̏ꍇ�̂ݔ���j
							if( mRecognizeContext_CurrentDisabledDirectiveLevel == kIndex_Invalid )
							{
								//�X�y�[�X�n�������Ȃ���text�֊i�[
								AText	text;
								inText.GetTextWithoutSpaces(pos,epos-pos,text);
								//enabled define�i���V�X�e���w�b�_�ȊO�j�Ȃ疳��define�Ɣ��f
								if( enabledDefineArray.Find(text) != kIndex_Invalid && 
											mSystemHeaderMode == false )
								{
									mRecognizeContext_NextDisabledDirectiveLevel = mRecognizeContext_CurrentDirectiveLevel;
								}
							}
							//#450
							//FoldingLevel�ݒ�(If)
							foldingLevel |= kFoldingLevel_IfStart;
						}
						//#endif
						else if( syntaxDefinition.IsDirectiveEndifString(token) == true )
						{
							//Directive���x���f�N�������g�i���s����j
							if( mRecognizeContext_NextDirectiveLevel > 0 )
							{
								mRecognizeContext_NextDirectiveLevel--;
							}
							//�f�N�������g��̃��x�����Adisable���̃��x���ȉ��Ȃ�disable��ԉ���
							if( mRecognizeContext_CurrentDisabledDirectiveLevel >= mRecognizeContext_NextDirectiveLevel )
							{
								mRecognizeContext_NextDisabledDirectiveLevel = kIndex_Invalid;
							}
							//#450
							foldingLevel |= kFoldingLevel_IfEnd;
							//FoldingLevel�ݒ�(EndIf)
						}
					}
				}
			}
		}
		
		//======================== ���o���F�� ========================
		
		mLineInfo.SetLineInfoP_ColorizeLine(lineIndex,false);
		//��#695 mLineInfoP_RegExpGroupColor_StartIndex.Set(lineIndex,kIndex_Invalid);//#603
		//��#695 mLineInfoP_RegExpGroupColor_EndIndex.Set(lineIndex,kIndex_Invalid);//#603
		if( IsParagraphStartLine(lineIndex) == true )
		{
			/*#130 drop �s���I�h�ŊJ�n����s�����o��
			//#130
			//
			if( false )
			{
				//�s���擾
				AIndex	spos = GetLineStart(lineIndex);
				AIndex	epos = spos + GetLineLengthWithoutLineEnd(lineIndex);
				AIndex	level = 0;
				//�e���������[�v
				for( AIndex	pos = spos; pos < epos; pos++ )
				{
					//�s���I�h�Ŏn�܂�ꍇ�A�s���I�h�̐��������x��++
					if( inText.Get(pos) == '.' )
					{
						level++;
					}
					else
					{
						break;
					}
				}
				//�A�E�g���C�����o�������ʎq�Ƃ��Ēǉ�
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
			//�i���̍ŏ����琳�K�\����v���邩�ǂ������肷��
			//Identifier�͊e�s�ɒǉ������
			
			AItemCount	jumpSetupItemCount = mJumpSetupRegExp.GetItemCount();
			for( AIndex jumpSetupIndex = 0; jumpSetupIndex < jumpSetupItemCount; jumpSetupIndex++ )
			{
				AIndex	pos = GetLineStart(lineIndex);//B0354+offset;//B0324
				AIndex	nextParagraphLineIndex = GetNextParagraphStartLineIndex(lineIndex);
				AIndex	endpos = GetLineStart(nextParagraphLineIndex-1)+GetLineLengthWithLineEnd(nextParagraphLineIndex-1);
				ARegExp&	regexp = mJumpSetupRegExp.GetObject(jumpSetupIndex);
				//���K�\�������ݒ�Ȃ牽�����Ȃ�
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
					}//B0316 if���͈͕̔ύX�i�s�̐F�Â��ݒ肪�����Ă����̋@�\�͊e�ݒ�ɏ]���ׂ��j
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
						//�܂肽���݃��x���ݒ�i���o���j
						foldingLevel |= kFoldingLevel_Header;
					}
					//#603 ���o�����K�\���O���[�v�F�Â�
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
						//��#695 mLineInfoP_RegExpGroupColor.Set(lineIndex,color);
						//��#695 mLineInfoP_RegExpGroupColor_StartIndex.Set(lineIndex,spos-lineStart);
						//��#695 mLineInfoP_RegExpGroupColor_EndIndex.Set(lineIndex,epos-lineStart);
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
		
		//======================== �e���ݒ� ========================
		
		//
		mLineInfo.SetLineInfoP_StateIndex(lineIndex,mRecognizeContext_CurrentStateIndex);
		mLineInfo.SetLineInfoP_PushedStateIndex(lineIndex,mRecognizeContext_PushedStateIndex);
		mLineInfo.SetLineInfoP_IndentCount(lineIndex,mRecognizeContext_CurrentIndent);
		//#693
		//�t�H�[���f�B���O�ݒ�
		mLineInfo.SetLineInfoP_FoldingLevel(lineIndex,foldingLevel);
		//�R�����g/Block Folding����
		if( lineIndex > 0 )
		{
			UpdateCommentBlockFoldingLevel(lineIndex-1,isCommentStateArray);
		}
		//
		mLineInfo.SetLineInfoP_Recognized(lineIndex,true);
		mLineInfo.SetLineInfoP_DirectiveLevel(lineIndex,mRecognizeContext_CurrentDirectiveLevel);//#467
		mLineInfo.SetLineInfoP_DisabledDirectiveLevel(lineIndex,mRecognizeContext_CurrentDisabledDirectiveLevel);//#467
		//#467 directive�s���X�V
		ABool	disabledLine = ((mRecognizeContext_CurrentDisabledDirectiveLevel!=kIndex_Invalid)&&
				(mRecognizeContext_NextDisabledDirectiveLevel!=kIndex_Invalid));
		mRecognizeContext_CurrentDirectiveLevel = mRecognizeContext_NextDirectiveLevel;
		mRecognizeContext_CurrentDisabledDirectiveLevel = mRecognizeContext_NextDisabledDirectiveLevel;
		//fprintf(stderr,"L:%ld I:%ld  ",lineIndex,mRecognizeContext_CurrentIndent);
		//fprintf(stderr,"L:%ld DirectiveLevel:%ld DirectiveDisabled:%ld \n",lineIndex,mRecognizeContext_CurrentDirectiveLevel,disabledDirectiveLevel);
		//fprintf(stderr,"L:%ld %ld ",lineIndex,mRecognizeContext_CurrentDisabledDirectiveLevel);
		//#467 directive�ɂ��disable�s�Ȃ�s�̐F�ݒ肵�āA����ȍ~�̕��@�F���͂��Ȃ�
		if( disabledLine == true )
		{
			mLineInfo.SetLineInfoP_ColorizeLine(lineIndex,true);
			mLineInfo.SetLineInfoP_LineColor(lineIndex,directiveDisabledColor);
			continue;
		}
		//}
		
		//======================== SyntaxDefinition ���@�F�� ========================
		
		AIndex	pos = GetLineStart(lineIndex);
		AIndex	endpos = pos+GetLineLengthWithLineEnd(lineIndex);
		while( pos < endpos )
		{
			/*#698 ���ɂ܂Ƃ��ɓ��삵�Ă��Ȃ������Ȃ̂ō폜
			//
			if( pos >= recogStartTextIndex && recogStartStateIndex == kIndex_Invalid )
			{
				recogStartStateIndex = mRecognizeContext_CurrentStateIndex;
				*B0339 2.1.8b2���_�ł܂Ƃ��ɓ��삵�Ă��Ȃ������̂�nomemory�͍폜
				if( syntaxDefinition.GetSyntaxDefinitionState(recogStartStateIndex).IsNoMemoryState() == true )
				{
					noMemoryStateFlag = true;
				}*
			}*/
			//
			ASyntaxDefinitionState&	state = syntaxDefinition.GetSyntaxDefinitionState(mRecognizeContext_CurrentStateIndex);
			const AArray<AIndex>&	conditionTextByteCountArrayIndexTable = state.GetConditionTextByteCountArrayIndexTable();//#693
			
			//���݂̕��@�p�[�gindex���擾
			AIndex	syntaxPartIndex = state.GetStateSyntaxPartIndex();
			
			//���̃A�N�V�����V�[�P���X�̎擾
			//�����e�L�X�g�Ƀq�b�g����܂ŁA�g�[�N���P�ʂœǂݐi�߂Ă����B�i�����e�L�X�g�Ƃ̔�r�́A�g�[�N���J�n�ʒu����̂ݍs���B�j
			AIndex	seqIndex = kIndex_Invalid;
			AIndex	spos = pos;
			AText	regExpGroup1;
			//RC2 ��ԊJ�n���A��x�����Astart�A�N�V���������s����
			if( stateStart == true )
			{
				seqIndex = state.GetStartActionSequenceIndex();
				stateStart = false;
			}
			if( seqIndex == kIndex_Invalid )//state�̍ŏ��ŁAstartAction�����݂���ꍇ�́A�܂���������s����B�istartAction���s���͎���while�����s�����A���ڃA�N�V�������s�ցj
			
			while( pos < endpos )
			{
				//�^�u�A�X�y�[�X�ǂݔ�΂�
				if( inText.SkipTabSpace(pos,endpos) == false )   break;
				spos = pos;
				
				//==================���K�\����v����==================
				for( AIndex i = 0; i < mSyntaxDefinitionRegExp.GetItemCount(); i++ )
				{
					if( mSyntaxDefinitionRegExp_State.Get(i) == mRecognizeContext_CurrentStateIndex )
					{
						AIndex	p = pos;
						if( mSyntaxDefinitionRegExp.GetObject(i).Match(inText,p,endpos) == true )
						{
							//���K�\���O���[�v1���L��
							AText	g1("$1");
							mSyntaxDefinitionRegExp.GetObject(i).ChangeReplaceText(inText,g1,regExpGroup1);
							//
							seqIndex = mSyntaxDefinitionRegExp_Index.Get(i);
							//�ǂݐi��
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
				//���K�\����v�����Ȃ炱���ŏ�����v�������[�v�I��
				if( seqIndex != kIndex_Invalid )
				{
					break;
				}
				
				//==================�����e�L�X�g����==================
				//�ŏ���1�o�C�g��ǂ݁A�\���̂�������e�L�X�g�̃o�C�g�����̃e�L�X�g�ɂ��āA�����e�L�X�g�Ƃ̈�v���������݂�
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
						//win 080722 forxxx��for�Ɉ�v���Ă��܂����΍�
						//fprintf(stderr,"n ");
						if( pos+byteCount < inText.GetItemCount() )
						{
							//fprintf(stderr,"st:%c ",inText.Get(pos));
							//fprintf(stderr,"et:%c ",inText.Get(pos+byteCount));
							//�A���t�@�x�b�g�Ŏn�܂�P��ŁApos+byteCount�̈ʒu�i��v�����̌��̕����j�ɃA���t�@�x�b�g������΁A��v���Ȃ��Ƃ݂Ȃ�
							if( syntaxDefinition.IsStartAlphabet(inText.Get(pos),syntaxPartIndex) == true &&
										syntaxDefinition.IsContinuousAlphabet(inText.Get(pos+byteCount),syntaxPartIndex) == true )   continue;
						}
						
						seqIndex = state.FindActionSequenceIndex(inText,pos,byteCount);
						if( seqIndex != kIndex_Invalid )
						{
							//�����e�L�X�g�ƈ�v�����ꍇ
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
							//#includee�ł��A"#include"�Ɉ�v���Ă��܂�����́uforxxx��for�Ɉ�v���Ă��܂����΍�v�ɂđ΍�ς�
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
					//�����e�L�X�g�ƈ�v���Ȃ������ꍇ�́A�P�g�[�N���ǂݐi�߂�B
					if( GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,pos,endpos,token,tokenspos,syntaxPartIndex) == false )   break;//#664
					if( token.GetItemCount() == 0 )   break;
					if( token.Get(0) != kUChar_LineEnd )
					{
						mRecognizeContext_TokenStack_StartPos.Set(mRecognizeContext_CurrentTokenStackPosition%kTokenStackMaxCount,tokenspos);
						mRecognizeContext_TokenStack_Length.Set(mRecognizeContext_CurrentTokenStackPosition%kTokenStackMaxCount,pos-tokenspos);
						mRecognizeContext_CurrentTokenStackPosition++;
					}
					//------------------�q�A�h�L�������g�I���e�L�X�g��v����------------------
					//�I���e�L�X�g��"heredocumentid"�Ƃ����ϐ��ɓ����Ă���̂ł��̕ϐ����擾
					AText	hereDocumentIdVarName("heredocumentid");
					AIndex	varIndex = syntaxDefinition.FindVariableIndex(hereDocumentIdVarName);
					if( varIndex != kIndex_Invalid )
					{
						//�q�A�h�L�������g�I���e�L�X�g�擾
						AText	hereDocumentId;
						mRecognizeContext_Variable_ValueArray.Get(varIndex,hereDocumentId);
						//token�Ƃ̈�v�𔻒�
						if( token.Compare(hereDocumentId) == true )
						{
							seqIndex = state.GetHereDocumentEndSequenceIndex();
							if( seqIndex != kIndex_Invalid )
							{
								break;
							}
						}
					}
					//�ǂ̏����e�L�X�g�Ƃ���v���Ȃ������ꍇ�ADefaultAction����������B
					seqIndex = state.GetDefaultActionSequenceIndex();
					if( seqIndex != kIndex_Invalid )
					{
						break;
					}
				}
			}
			//�s���܂ŁA�ǂ̏����e�L�X�g�ɂ��q�b�g�����ADefaultAction���Ȃ������ꍇ��break
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
			
			//==================�A�N�V�����V�[�P���X�̃A�N�V���������Ɏ��s����==================
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
					//�A�����Ă���ꍇ�́Astart���X�V���Ȃ��i�O���start��start�̂܂܂ɂ���j
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
						//���̃g�[�N�����ǂ�
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
					//�ϐ��ɌŒ�e�L�X�g��ݒ�
				  case kSyntaxDefinitionActionType_SetVarStaticText:
					{
						//�ϐ�index�擾
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						//�Œ�e�L�X�g���擾
						AText	text;
						AIndex	dummy = kIndex_Invalid;
						seq.GetKeyword(actionIndex,dummy,text);
						//�ϐ��ɐݒ�
						mRecognizeContext_Variable_ValueArray.Set(varIndex,text);
						break;
					}
					//�ϐ��ɁA���̓��e�����ʎq���Ƃ��鎯�ʎq�̃^�C�v��ݒ肵����
				  case kSyntaxDefinitionActionType_SetVarTypeOfVar:
					{
						//�ϐ�index�擾
						AIndex	varIndex = seq.GetVariableIndex(actionIndex);
						if( varIndex != kIndex_Invalid )
						{
							//�ϐ��̍��̓��e���擾
							AText	text;
							mRecognizeContext_Variable_ValueArray.Get(varIndex,text);
							//���̓��e�����ʎq���Ƃ��鎯�ʎq�̃^�C�v���擾
							//�i�O���[�o�����ʎq�A�V�X�e���w�b�_���ʎq�A�L�[���[�h���X�g���猟���j
							AText	typetext;
							if( GetGlobalIdentifierTypeTextByKeywordText(text,typetext) == false )
							{
								if( modePrefDB.GetSystemHeaderIdentifierLinkList().FindKeywordType(text,typetext) == false )
								{
									if( modePrefDB.FindKeywordTypeFromKeywordList(text,typetext) == false )
									{
										//��̃e�L�X�g��ݒ肷��
										typetext.DeleteAll();
									}
								}
							}
							//�ϐ��ɐݒ�
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
						//�R���e�L�X�g�ɊY���ϐ��̃C���f���g��ݒ�#695
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
						//�Y���ϐ��̌��݂�BlockStart�C���f���g�i�R���e�L�X�g�Ɋi�[����Ă���j�ȉ��̃C���f���g���x���Ȃ��ԑJ��
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
						//�O�̕������A���t�@�x�b�g���ǂ������ׂ�
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
						//�A���t�@�x�b�g�̌�łȂ���Ώ�ԑJ��
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
						//�O�̕������A���t�@�x�b�g���ǂ������ׂ�
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
						//�A���t�@�x�b�g�̌�łȂ���Ώ�ԑJ��
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
						//���݂�token�ƕϐ�����v�Ȃ��ԑJ��
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
						//���̍s�J�n����push state���s����
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
						//PopState���ɂ�start���̃V�[�P���X�͎��s���Ȃ����Ƃɂ���BstateStart = true;//RC2
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
						if( /*#698 mRecognizeContext_RecognizeStartLineIndex�𗘗p����������@�ɕύX identifierClearedLine.Find(lineIndex) != kIndex_Invalid*/
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
						if( /*#698 mRecognizeContext_RecognizeStartLineIndex�𗘗p����������@�ɕύX identifierClearedLine.Find(addedLineIndex) != kIndex_Invalid*/
						   addedLineIndex >= mRecognizeContext_RecognizeStartLineIndex )
						{
							//���ʎq�ǉ��s�̃C���f���g���擾
							AIndex	indentCount = indentCountAtLineStart;
							if( addedLineIndex < lineIndex )
							{
								indentCount = mLineInfo.GetLineInfoP_IndentCount(addedLineIndex);
							}
							//global���ʎq�ǉ�
							AUniqID	UniqID =	AddLineGlobalIdentifier(kIdentifierType_GlobalIdentifier,
									/*#698 spos,spos+length*/ addedLineIndex,spt.x,spt.x+length,
									categoryIndex,keyword,menutext,infotext,
									commenttext,parenttext,typetext,(menutext.GetItemCount()>0),
																		seq.GetOutlineLevel(actionIndex) + indentCount*10);//RC2 #593 #875
							if( UniqID != kUniqID_Invalid && menutext.GetItemCount() > 0 )//#695
							{
								outAddedGlobalIdentifiers.Add(UniqID);
								outAddedGlobalIdentifiersLineIndex.Add(addedLineIndex);
								//���j���[�o�^����ꍇ�́Afolding level�Ɍ��o���t���O�ݒ�
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
						if( /*#698 mRecognizeContext_RecognizeStartLineIndex�𗘗p����������@�ɕύX identifierClearedLine.Find(spt.y) != kIndex_Invalid*/
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
�R�����g�^�u���b�Nfolding���x���𔻒肵�čs���ɐݒ肷��
*/
void	ATextInfo::UpdateCommentBlockFoldingLevel( const AIndex inLineIndex, const ABoolArray& isCommentStateArray)
{
	//==================�R�����gfolding����==================
	//�O�s�̊J�n�ʒu���R�����g���ǂ����A���ݍs�̊J�n�ʒu���R�����g���ǂ������擾
	ABool	prevLineIsComment = isCommentStateArray.Get(mLineInfo.GetLineInfoP_StateIndex(inLineIndex));
	ABool	currentLineIsComment = false;
	if( inLineIndex+1 < GetLineCount() )
	{
		currentLineIsComment = isCommentStateArray.Get(mLineInfo.GetLineInfoP_StateIndex(inLineIndex+1));
	}
	//�R�����g�J�n
	AFoldingLevel	prevLineFoldingLevel = mLineInfo.GetLineInfoP_FoldingLevel(inLineIndex);
	if( prevLineIsComment == false && currentLineIsComment == true )
	{
		mLineInfo.SetLineInfoP_FoldingLevel(inLineIndex,prevLineFoldingLevel|kFoldingLevel_CommentStart);
	}
	//�R�����g�I��
	else if( prevLineIsComment == true && currentLineIsComment == false )
	{
		mLineInfo.SetLineInfoP_FoldingLevel(inLineIndex,prevLineFoldingLevel|kFoldingLevel_CommentEnd);
	}
	//==================�u���b�Nfolding����==================
	//�u���b�NFolding����
	if( inLineIndex+1 < GetLineCount() )
	{
		AFoldingLevel	prevLineFoldingLevel = mLineInfo.GetLineInfoP_FoldingLevel(inLineIndex);
		//�u���b�N�J�n
		if( mLineInfo.GetLineInfoP_IndentCount(inLineIndex) < mLineInfo.GetLineInfoP_IndentCount(inLineIndex+1) )
		{
			mLineInfo.SetLineInfoP_FoldingLevel(inLineIndex,prevLineFoldingLevel|kFoldingLevel_BlockStart);
		}
		//�u���b�N�I��
		if( mLineInfo.GetLineInfoP_IndentCount(inLineIndex) > mLineInfo.GetLineInfoP_IndentCount(inLineIndex+1) )
		{
			mLineInfo.SetLineInfoP_FoldingLevel(inLineIndex,prevLineFoldingLevel|kFoldingLevel_BlockEnd);
		}
	}
}

//#698
/**
���@�F���R���e�L�X�g�S�폜
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
�ŏ��̍s���猟�����čŏ��ɖ��F���̍sindex��Ԃ�
*/
AIndex	ATextInfo::FindFirstUnrecognizedLine() const
{
	return mLineInfo.FindFirstUnrecognizedLine();
}

//#698
/**
���@�F���R���e�L�X�g���A���@�F���X���b�h�փR�s�[
�i�R�s�[��I�u�W�F�N�g�ŃR�[�������j
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
�s���𕶖@�F���X���b�h�փR�s�[
�i�R�s�[��I�u�W�F�N�g�ŃR�[�������j
*/
void	ATextInfo::CopyLineInfoToSyntaxRecognizer( const ATextInfo& inSrcTextInfo, const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	//���ʎq���폜
	{
		//#717
		//�O���[�o�����ʎq�r������
		AStMutexLocker	locker(mGlobalIdentifierListMutex);
		//�O���[�o�����ʎq�S�폜
		mGlobalIdentifierList.DeleteAllIdentifiers();
	}
	{
		//#717
		//���[�J�����ʎq�r������
		AStMutexLocker	locker(mLocalIdentifierListMutex);
		//���[�J�����ʎq�S�폜
		mLocalIdentifierList.DeleteAllIdentifiers();
	}
	//BlockStartData�S�폜
	mBlockStartDataList.DeleteAll();
	//�s���R�s�[
	mLineInfo.CopyLineInfoToSyntaxRecognizer(inSrcTextInfo.mLineInfo,inStartLineIndex,inEndLineIndex);
}

//#698
/**
���@�F���X���b�h�̌��ʂ��A���C���X���b�h��TextInfo�փR�s�[
�i�R�s�[��I�u�W�F�N�g�ŃR�[�������j
*/
void	ATextInfo::CopyFromSyntaxRecognizerResult( const AIndex inDstStartLineIndex, const AIndex inDstEndLineIndex,
		const ATextInfo& inSrcTextInfo,
		AArray<AUniqID>& outDeletedIdentifiers, 
		AArray<AUniqID>& outAddedGlobalIdentifiers, AArray<AIndex>& outAddedGlobalIdentifiersLineIndex,
		ABool& outImportChanged )
{
	//�Ԃ�l������
	outDeletedIdentifiers.DeleteAll();
	outAddedGlobalIdentifiers.DeleteAll();
	outAddedGlobalIdentifiersLineIndex.DeleteAll();
	outImportChanged = false;
	
	//fprintf(stderr,"CopyFromSyntaxRecognizerResult():start:%ld:end:%ld\n",inDstStartLineIndex,inDstEndLineIndex);
	//�R�s�[��̎��ʎq���폜����
	for( AIndex dstLineIndex = inDstEndLineIndex-1; dstLineIndex >= inDstStartLineIndex; dstLineIndex-- )
	{
		ABool	importChanged = false;
		DeleteLineIdentifiers(dstLineIndex,outDeletedIdentifiers,importChanged);
		if( importChanged == true )
		{
			outImportChanged = true;
		}
	}
	
	//�s���R�s�[
	mLineInfo.CopyFromSyntaxRecognizerResult(inDstStartLineIndex,inDstEndLineIndex,inSrcTextInfo.mLineInfo);
	
	//�O���[�o�����ʎq�R�s�[
	{
		AIndex	srcLineIndex = 0;
		for( AIndex dstLineIndex = inDstStartLineIndex; dstLineIndex < inDstEndLineIndex; dstLineIndex++ )
		{
			//�R�s�[���̍s�̍ŏ��̎��ʎq���擾
			AUniqID	srcUniqID = inSrcTextInfo.mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(srcLineIndex);
			AItemCount	loopCount = 0;//�������[�v�h�~
			while( srcUniqID != kUniqID_Invalid )
			{
				//���ʎq�f�[�^�擾
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
				//���ʎq�ǉ�
				AUniqID	dstUniqID = AddLineGlobalIdentifier(identifierType,
						dstLineIndex,xspos,xepos,categoryIndex,keyword,menutext,infotext,
						commenttext,parenttext,typetext,isLocalDelimiter,outlineLevel);
				//import�Ȃ�t���Oon
				if( identifierType == kIdentifierType_ImportFile )
				{
					outImportChanged = true;
				}
				//���j���[���ʎq�Ȃ�outAddedGlobalIdentifiers�ɒǉ�
				if( dstUniqID != kUniqID_Invalid && menutext.GetItemCount() > 0 )
				{
					outAddedGlobalIdentifiers.Add(dstUniqID);
					outAddedGlobalIdentifiersLineIndex.Add(dstLineIndex);
				}
				//���̎��ʎq���擾
				srcUniqID = inSrcTextInfo.mGlobalIdentifierList.GetNextUniqID(srcUniqID);
				//�������[�v�h�~
				loopCount++;
				if( loopCount > 128 )   { _ACError("Id UniqID loop",NULL); break;}
			}
			//���̍s
			srcLineIndex++;
		}
	}
	//���[�J�����ʎq�R�s�[
	{
		AIndex	srcLineIndex = 0;
		for( AIndex dstLineIndex = inDstStartLineIndex; dstLineIndex < inDstEndLineIndex; dstLineIndex++ )
		{
			//�R�s�[���̍s�̍ŏ��̎��ʎq���擾
			AUniqID	srcUniqID = inSrcTextInfo.mLineInfo.GetLineInfoP_FirstLocalIdentifierUniqID(srcLineIndex);
			AItemCount	loopCount = 0;//�������[�v�h�~
			while( srcUniqID != kUniqID_Invalid )
			{
				//���ʎq�f�[�^�擾
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
				//���ʎq�ǉ�
				AddLineLocalIdentifier(identifierType,
							dstLineIndex,xspos,xepos,categoryIndex,keyword,infotext,typetext);
				//���̎��ʎq���擾
				srcUniqID = inSrcTextInfo.mLocalIdentifierList.GetNextUniqID(srcUniqID);
				//�������[�v�h�~
				loopCount++;
				if( loopCount > 128 )   { _ACError("Id UniqID loop",NULL); break;}
			}
			//���̍s
			srcLineIndex++;
		}
	}
	//BlockStartData�R�s�[
	{
		AIndex	srcLineIndex = 0;
		for( AIndex dstLineIndex = inDstStartLineIndex; dstLineIndex < inDstEndLineIndex; dstLineIndex++ )
		{
			//�R�s�[���̍s�̍ŏ���blockstart���擾
			AUniqID	srcUniqID = inSrcTextInfo.mLineInfo.GetLineInfoP_FirstBlockStartDataUniqID(srcLineIndex);
			AItemCount	loopCount = 0;//�������[�v�h�~
			while( srcUniqID != kUniqID_Invalid )
			{
				//blockstart�f�[�^�擾
				AIndex	variableIndex = inSrcTextInfo.mBlockStartDataList.GetVariableIndex(srcUniqID);
				AItemCount	indentCount = inSrcTextInfo.mBlockStartDataList.GetIndentCount(srcUniqID);
				//blockstart�ǉ�
				AddBlockStartData(dstLineIndex,variableIndex,indentCount);
				//����blockstart
				srcUniqID = inSrcTextInfo.mBlockStartDataList.GetNextUniqID(srcUniqID);
				//�������[�v�h�~
				loopCount++;
				if( loopCount > 128 )   { _ACError("Id UniqID loop",NULL); break;}
			}
			//���̍s
			srcLineIndex++;
		}
	}
}

//#723
/**
���@�F���pToken�擾
@note ���@��`��ptoken�擾�BSDF�̃A���t�@�x�b�g��`�ɏ]����token�擾�B
���@�F������tokenize�̃A���t�@�x�b�g��`��SDF�̑O��Ƃ͂����ƕ��@�F�����������Ȃ��Ȃ�̂ŁA�K��SDF�ɒ�`�����A���t�@�x�b�g��`���g�����Ƃɂ���B
*/
ABool	ATextInfo::GetTokenForRecongnizeSyntax( const ASyntaxDefinition& inSyntaxDefinition, AUnicodeData& inUnicodeData,//#664
		const AText& inText, AIndex& ioPos, const AIndex inEndPos, AText& outToken, AIndex& outTokenStartPos,
		const AIndex inSyntaxPartIndex ) const
{
	//#723 �A���t�@�x�b�g�ō\�������g�[�N�����ǂ����̕Ԃ��p�����[�^�ǉ�
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
	//token���A���t�@�x�b�g�ō\�������ǂ����̃t���O������
	outIsAlphabet = false;
	//
	outToken.DeleteAll();
	
	if( inText.SkipTabSpace(ioPos,inEndPos) == false )   return false;
	
	outTokenStartPos = ioPos;
	
	//�|�C���^�擾
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	const AUChar*	ptr = textptr.GetPtr();
	const AItemCount	textlen = textptr.GetByteCount();
	
	//==================�ŏ��̕����̓ǂݍ���==================
	AUCS4Char	ucs4char = 0;
	AItemCount	bc = AText::Convert1CharToUCS4(ptr,textlen,ioPos,ucs4char);
	AUChar	ch = inText.Get(ioPos);
	//Unicode 1�����i�[
	for( AIndex i = 0; i < bc; i++ )
	{
		if( ioPos >= inText.GetItemCount() )   break;//�}���`�t�@�C���������Ńo�C�i���t�@�C����ǂ񂾏ꍇ�Ȃǂ̑΍�
		outToken.Add(inText.Get(ioPos));
		ioPos++;
	}
	//�A���t�@�x�b�g����
	AUnicodeCharCategory	category = inUnicodeData.GetUnicodeCharCategory(ucs4char);
	if( bc == 1 )
	{
		//�A�X�L�[���̏ꍇ�F���@��`�ɂ�蔻�聨�A���t�@�x�b�g�ȊO�Ȃ炱���i1�����j�ŏI��
		if( inSyntaxDefinition.IsStartAlphabet(ch,inSyntaxPartIndex) == false )   return true;//win 080722
	}
	else
	{
		//�A�X�L�[�O�̏ꍇ�FUnicode�f�[�^�ɂ�蔻�聨�A���t�@�x�b�g�ȊO�Ȃ炱���i1�����j�ŏI��
		//�iv2.1�ł�LatinAndNumber�J�e�S����Ǝ��ɔ��肵�Ă������Av3�ł�unicode��`�ɏ]���j
		if( inUnicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char) == false )   return true;
	}
	
	//#723
	//�����܂ŗ����ꍇ�́Atoken�̓A���t�@�x�b�g�ō\�������Ɣ��f
	outIsAlphabet = true;
	
	//==================�e�������̃��[�v==================
	while( ioPos < inEndPos )
	{
		//�ǂݍ���
		bc = AText::Convert1CharToUCS4(ptr,textlen,ioPos,ucs4char);
		ch = inText.Get(ioPos);
		category = inUnicodeData.GetUnicodeCharCategory(ucs4char);
		//�A���t�@�x�b�g����
		if( bc == 1 )
		{
			//�A�X�L�[���̏ꍇ�F���@��`�ɂ�蔻�聨�A���t�@�x�b�g�ȊO�Ȃ炱���i�A���t�@�x�b�g�ȊO�̕����̑O�j�ŏI��
			if( inSyntaxDefinition.IsContinuousAlphabet(ch,inSyntaxPartIndex) == false )   return true;//win 080722
		}
		else
		{
			//�A�X�L�[�O�̏ꍇ�FUnicode�f�[�^�ɂ�蔻�聨�A���t�@�x�b�g�ȊO�Ȃ炱���i�A���t�@�x�b�g�ȊO�̕����̑O�j�ŏI��
			//�iv2.1�ł�LatinAndNumber�J�e�S����Ǝ��ɔ��肵�Ă������Av3�ł�unicode��`�ɏ]���j
			if( inUnicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char) == false )   return true;
		}
		//Unicode 1�����i�[
		for( AIndex i = 0; i < bc; i++ )
		{
			if( ioPos >= inText.GetItemCount() )   break;//�}���`�t�@�C���������Ńo�C�i���t�@�C����ǂ񂾏ꍇ�Ȃǂ̑΍�
			outToken.Add(inText.Get(ioPos));
			ioPos++;
		}
	}
	return true;
}

//#723 ADocument_Text����ړ�
/**
�P�ꌟ��
@note �P�ꂪ�A���t�@�x�b�g���}���`�o�C�g�Ȃ�true��Ԃ�
*/
ABool	ATextInfo::FindWord( const AText& inText, const AIndex inModeIndex, const ABool inFindForwardOnly, 
		const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd, 
		const ABool inUseAlphabetForKeyword, const AIndex inSyntaxPartIndex ) 
{
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(inModeIndex);
	//#717
	//�Ԃ�l�i�P�ꂪ�A���t�@�x�b�g���}���`�o�C�g�j������
	ABool	wordIsAlphabetOrMultibyte = true;
	
	//�J�n�ʒu�␳�i�}���`�o�C�g�̓r������J�n���Ȃ��悤�ɂ���j
	AIndex	startPos = inText.GetCurrentCharPos(inStartPos);
	if( startPos != inStartPos )
	{
		startPos = inText.GetNextCharPos(startPos);
	}
	
	//�|�C���^�擾
	AStTextPtr	sttextptr(inText,0,inText.GetItemCount());
	AUChar*	textptr = sttextptr.GetPtr();
	AByteCount	textlen = sttextptr.GetByteCount();
	
	//
	ABool	multibyteStart = false;
	ABool	alphabetStart = false;//#723
	AUnicodeCharCategory	category = kUnicodeCharCategory_Others;
	if( startPos == textlen )//B0125
	{
		//------------------�J�n�ʒu���e�L�X�g�̍Ō�̏ꍇ------------------
		multibyteStart = false;
	}
	else
	{
		//------------------�J�n�ʒu���e�L�X�g�̍Ō�ł͂Ȃ��ꍇ------------------
		
		//�}���`�o�C�g����
		multibyteStart = inText.IsUTF8MultiByte(startPos);
		
		//B0437
		//Unicode�f�[�^�ɂ��A���t�@�x�b�g����
		//�iv2.1�ł�LatinAndNumber�J�e�S����Ǝ��ɔ��肵�Ă������Av3�ł�unicode��`�ɏ]���j
		AUCS4Char	ucs4char = 0;
		AText::Convert1CharToUCS4(textptr,textlen,startPos,ucs4char);
		category = unicodeData.GetUnicodeCharCategory(ucs4char);
		//#883 if( category == kUnicodeCharCategory_LatinAndNumber )
		if( unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char) == true )//#883
		{
			alphabetStart = true;
		}
	}
	
	if( multibyteStart == false || alphabetStart == true ) //���̑�<test
	{
		//==================�J�n��������}���`�o�C�g�A�܂��́A�}���`�o�C�g�A���t�@�x�b�g�̏ꍇ==================
		//�i�L�[���[�h�\���������g�p����ꍇ�ł��A��}���`�o�C�g��������ɂ��Ă���̂ŁA�J�n�����ɂ��ăL�[���[�h�\�������𔻒�ł���j
		
		//�J�n�������A���t�@�x�b�g���ǂ����̔���
		
		//R0036 inUseAlphabetForKeyword��true�̂Ƃ��́A���[�h�ݒ�ɏ]�����P��I�����s���悤�A�e���C��
		ABool	isAlphabet = false;
		if( startPos < textlen )
		{
			//�J�n�����ǂݍ���
			AUCS4Char	ucs4char = 0;
			AItemCount	bc = AText::Convert1CharToUCS4(textptr,textlen,startPos,ucs4char);
			
			//�A���t�@�x�b�g����
			if( bc == 1 && inUseAlphabetForKeyword == true )
			{
				//------------------���[�h�ݒ���g���ꍇ------------------
				//��}���`�o�C�g�A���A�L�[���[�h�\�������ݒ���g�p����ꍇ�A�L�[���[�h�\�������ɂ�蔻��
				isAlphabet = modePrefDB.IsAlphabet(textptr[startPos],inSyntaxPartIndex);
			}
			else
			{
				//------------------Unicode�f�[�^�݂̂��g���ꍇ------------------
				//�}���`�o�C�g�A�܂��́A�L�[���[�h�\���������g�p���Ȃ��ꍇ�AUnicode�f�[�^�ɂ�蔻��
				isAlphabet = unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char);
			}
		}
		//==================�J�n�������A���t�@�x�b�g�ȊO�̏ꍇ�i���L�����j==================
		if( isAlphabet == false ) 
		{
			AUChar	ch = 0;
			if( startPos < textlen )
			{
				ch = textptr[startPos];
			}
			if( ch == kUChar_Space || ch == kUChar_Tab ) 
			{
				//�O��������
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
						//word����������
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
				//���������
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
						//word����������
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
			//�Ԃ�l�i�P�ꂪ�A���t�@�x�b�g���}���`�o�C�g�j��false�ɂ���
			wordIsAlphabetOrMultibyte = false;
		}
		//==================�J�n�������A���t�@�x�b�g�̏ꍇ==================
		else 
		{
			//�O�����Ɍ���
			if( inFindForwardOnly == false )
			{
				AIndex	pos = startPos;
				for( ; pos > 0; pos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(textptr,textlen,pos) )
				{
					//pos�̈ʒu�̕����ǂݍ���
					AUCS4Char	ucs4char = 0;
					AItemCount	bc = AText::Convert1CharToUCS4(textptr,textlen,pos,ucs4char);
					
					//�A���t�@�x�b�g����
					if( bc == 1 && inUseAlphabetForKeyword == true )
					{
						//------------------���[�h�ݒ���g���ꍇ------------------
						//��}���`�o�C�g�A���A�L�[���[�h�\�������ݒ���g�p����ꍇ�A�L�[���[�h�\�������ɂ�蔻��
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
						//------------------Unicode�f�[�^�݂̂��g���ꍇ------------------
						//�}���`�o�C�g�A�܂��́A�L�[���[�h�\���������g�p���Ȃ��ꍇ�AUnicode�f�[�^�ɂ�蔻��
						isAlphabet = unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char);
					}
					
					//�A���t�@�x�b�g�łȂ���΃��[�v�I��
					if( isAlphabet == false )
					{
						pos = AText::GetNextCharPos(textptr,textlen,pos);//B0437
						break;
					}
					//word����������
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
			//������Ɍ���
			{
				AIndex	 pos = unicodeData.GetNextCharPosSkipCanonicalDecomp(textptr,textlen,startPos);
				for(; pos < textlen; pos = unicodeData.GetNextCharPosSkipCanonicalDecomp(textptr,textlen,pos) )
				{
					//pos�̈ʒu�̕����ǂݍ���
					AUCS4Char	ucs4char = 0;
					AItemCount	bc = AText::Convert1CharToUCS4(textptr,textlen,pos,ucs4char);
					
					//�A���t�@�x�b�g����
					if( bc == 1 && inUseAlphabetForKeyword == true )
					{
						//------------------���[�h�ݒ���g���ꍇ------------------
						isAlphabet = ( (modePrefDB.IsContinuousAlphabet(textptr[pos],inSyntaxPartIndex)==true) &&
									(modePrefDB.IsTailAlphabet(textptr[pos-1],inSyntaxPartIndex)==false) );
					}
					else
					{
						//------------------Unicode�f�[�^�݂̂��g���ꍇ------------------
						//�}���`�o�C�g�A�܂��́A�L�[���[�h�\���������g�p���Ȃ��ꍇ�AUnicode�f�[�^�ɂ�蔻��
						isAlphabet = unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char);
					}
					
					//�A���t�@�x�b�g�łȂ���΃��[�v�I��
					if( isAlphabet == false )   break;
					//word����������
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
		//==================�J�n�������}���`�o�C�g�A���A��A���t�@�x�b�g�̏ꍇ==================
		
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
			//�O��������
			if( inFindForwardOnly == false )
			{
				AIndex	pos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(textptr,textlen,startPos);
				for( ; pos > 0 ; pos = unicodeData.GetPrevCharPosSkipCanonicalDecomp(textptr,textlen,pos) )
				{
					AItemCount	bc = AText::Convert1CharToUCS4(textptr,textlen,pos,ucs4char);
					AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(ucs4char);
					//�J�e�S�����ω����邩�A�A���t�@�x�b�g�����ɂȂ����ꍇ�A�����܂łŒP��I���i�������A�J�e�S���ɂ���Ă͌p���j
					if( firstCharCategory != category || unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char) == true )
					{
						if( (firstCharCategory==kUnicodeCharCategory_ZenkakuHiragana||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) &&
									category == kUnicodeCharCategory_ZenkakuOtohiki )
						{
							//�p��
						}
						else if( firstCharCategory == kUnicodeCharCategory_ZenkakuOtohiki && 
									(category==kUnicodeCharCategory_ZenkakuHiragana||category==kUnicodeCharCategory_ZenkakuKatakana) )
						{
							firstCharCategory = category;
						}
						else if( bunsetsuSelect == true && (firstCharCategory == kUnicodeCharCategory_ZenkakuHiragana && 
										(category==kUnicodeCharCategory_Kanji||category==kUnicodeCharCategory_ZenkakuKatakana)) )
						{
							//�i�����܂��̓J�^�J�i�j���Ђ炪�ȁ@�̏ꍇ
							firstCharCategory = category;
						}
						else
						{
							pos += bc;
							break;
						}
					}
					//word����������
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
			//���������
			{
				AIndex	pos = startPos;
				for( ; pos < textlen; pos = unicodeData.GetNextCharPosSkipCanonicalDecomp(textptr,textlen,pos) )
				{
					bc = AText::Convert1CharToUCS4(textptr,textlen,pos,ucs4char);
					AUnicodeCharCategory	category = unicodeData.GetUnicodeCharCategory(ucs4char);
					//�J�e�S�����ω����邩�A�A���t�@�x�b�g�����ɂȂ����ꍇ�A�����܂łŒP��I���i�������A�J�e�S���ɂ���Ă͌p���j
					if( firstCharCategory != category || unicodeData.IsAlphabetOrNumberOrUnderbar(ucs4char) == true )
					{
						if( (firstCharCategory==kUnicodeCharCategory_ZenkakuHiragana||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) &&
									category == kUnicodeCharCategory_ZenkakuOtohiki )
						{
							//�p��
						}
						else if( bunsetsuSelect == true && ((firstCharCategory==kUnicodeCharCategory_Kanji||firstCharCategory==kUnicodeCharCategory_ZenkakuKatakana) && 
										category == kUnicodeCharCategory_ZenkakuHiragana) )
						{
							//�i�����܂��̓J�^�J�i�j���Ђ炪�ȁ@�̏ꍇ
							firstCharCategory = category;
						}
						else
						{
							break;
						}
					}
					//word����������
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
	//���ʕ␳�i�R�[�����Ŗ������[�v����������̂�h�~���邽�߁j
	if( inStartPos < outStart )
	{
		outStart = inStartPos;
	}
	if( outEnd <= outStart )
	{
		//end��start�̈ꕶ����ɂ���
		outEnd = unicodeData.GetNextCharPosSkipCanonicalDecomp(textptr,textlen,outStart);
	}
	//
	return wordIsAlphabetOrMultibyte;
}

//#1003
/**
�s�����R�[�h�����ǂ������擾
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
		//pos�ʒu����1�g�[�N�����
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
		
		//��ԑJ��
		stateIndex = nextStateIndex;
		
		//pos�X�V
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
		//pos�ʒu����1�g�[�N�����
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
		
		//��ԑJ��
		stateIndex = nextStateIndex;
		
		//pos�X�V
		pos = nextPos;
	}
}

/**
�w��ʒu��state�擾
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
		//pos�ʒu����1�g�[�N�����
		AIndex	nextStateIndex;
		ABool	changeStateFromNextChar;
		ATextIndex	nextPos;
		ABool	seqFound;
		AText	token;
		ATextIndex	tokenspos;
		AItemCount	currentIndent = 0, nextIndent = 0;
		RecognizeStateAndIndent(inText,stateIndex,pushedStateIndex,
								pos,endpos,nextStateIndex,changeStateFromNextChar,nextPos,seqFound,token,tokenspos,0,0,currentIndent,nextIndent);
		
		//��ԑJ��
		stateIndex = nextStateIndex;
		
		//pos�X�V
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
	
	//�^�u�A�X�y�[�X�ǂݔ�΂�
	if( inText.SkipTabSpace(outPos,inEndPos) == false )   return false;
	outTokenStartPos = outPos;
	
	//���s�ʒu�ŔF�������i�s�v�Z�X���b�h���ɂ��A�X���b�h�v�Z�Ώۍs�ɉ��s���݂̒����e�L�X�g�����邽�߁A���s�őł��؂肳����j#699
	if( inText.Get(outPos) == kUChar_LineEnd )
	{
		outPos = inEndPos;
		return false;
	}
	
	AIndex	seqIndex = kIndex_Invalid;
	//���K�\����v����
	for( AIndex i = 0; i < mSyntaxDefinitionRegExp.GetItemCount(); i++ )
	{
		if( mSyntaxDefinitionRegExp_State.Get(i) == inStateIndex )
		{
			AIndex	p = outPos;
			if( mSyntaxDefinitionRegExp.GetObject(i).Match(inText,p,inEndPos) == true )
			{
				seqIndex = mSyntaxDefinitionRegExp_Index.Get(i);
				//�ǂݐi��
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
		//�ŏ���1�o�C�g��ǂ݁A�\���̂�������e�L�X�g�̃o�C�g�����̃e�L�X�g�ɂ��āA�����e�L�X�g�Ƃ̈�v���������݂�
		AUChar	firstch = inText.Get(outPos);
		//byteCountArray: ��v�̉\���̂�������e�L�X�g�̃o�C�g��
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
					//�A���t�@�x�b�g�Ŏn�܂�P��ŁApos+byteCount�̈ʒu�i��v�����̌��̕����j�ɃA���t�@�x�b�g������΁A��v���Ȃ��Ƃ݂Ȃ�
					if( syntaxDefinition.IsStartAlphabet(inText.Get(outPos),syntaxPartIndex) == true &&
					   syntaxDefinition.IsContinuousAlphabet(inText.Get(outPos+byteCount),syntaxPartIndex) == true )   continue;
				}
				//�����e�L�X�g����A�N�V�����V�[�P���X����
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
	//�ǂ̏����e�L�X�g�Ƃ���v���Ȃ������ꍇ�A�P�g�[�N���ǂݐi�߂���ADefaultAction����������B
	ABool	result = false;
	if( seqIndex == kIndex_Invalid )
	{
		result = GetTokenForRecongnizeSyntax(syntaxDefinition,unicodeData,inText,outPos,inEndPos,outToken,outTokenStartPos,syntaxPartIndex);//#664
		seqIndex = state.GetDefaultActionSequenceIndex();
	}
	if( seqIndex != kIndex_Invalid )
	{
		//�����e�L�X�g�ƈ�v�����ꍇ
		//�A�N�V�����V�[�P���X�̃A�N�V���������Ɏ��s����
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
					//�O�̕������A���t�@�x�b�g���ǂ������ׂ�
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
					//�A���t�@�x�b�g�̌�łȂ���Ώ�ԑJ��
					if( afterAlphabet == false )
					{
						outStateIndex = seq.GetStateIndex(actionIndex);
					}
					break;
				}
				//R0241
			  case kSyntaxDefinitionActionType_PushStateIfNotAfterAlphabet:
				{
					//�O�̕������A���t�@�x�b�g���ǂ������ׂ�
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
					//�A���t�@�x�b�g�̌�łȂ���Ώ�ԑJ��
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
					//���s�̑�����������Ԃ͎��s�̑������݂̂����炷�B
					//���s�̑��������Ȃ��Ȃ�����A���s�̃C���f���g�ʂɁA���ݍs�̃C���f���g�ʂ����킹��
					//#1001 $(document).on('pageshow','#MainPage', function(event) {�Ƃ������s�̌��ݍs�̃C���f���g��+1�ɂȂ�����C��
					//�����ʂŏ�Ɏ��s�̃C���f���g�ʂɁA���ݍs�̃C���f���g�ʂ����킹�鏈�����s���ƁA�����ς݂̎��s�C���f���g�����ݍs�ɔ��f���Ă��܂��̂ŁA
					//�u���s�̑��������Ȃ��Ȃ�����v�̏�����ǉ��B
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
					//�����Ȃ�
					break;
				}
			}
		}
		outSeqFound = true;
		return true;
	}
	
	//�P�g�[�N���ǂ�
	return result;
}

//#695
/**
�S�Ă̍s�̔F����Ԃ��������A���ʎq�����폜����
*/
void	ATextInfo::ClearSyntaxDataForAllLines()
{
	//���ʎq���S�폜
	{
		//#717
		//�O���[�o�����ʎq�r������
		AStMutexLocker	locker(mGlobalIdentifierListMutex);
		//�O���[�o�����ʎq�S�폜
		mGlobalIdentifierList.DeleteAllIdentifiers();
	}
	{
		//#717
		//���[�J�����ʎq�r������
		AStMutexLocker	locker(mLocalIdentifierListMutex);
		//���[�J�����ʎq�S�폜
		mLocalIdentifierList.DeleteAllIdentifiers();
	}
	mBlockStartDataList.DeleteAll();
	//�S�s�̎��ʎquniqID�����A�F���t���Ooff
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		mLineInfo.SetLineInfo_FirstGlobalIdentifierUniqID(lineIndex,kUniqID_Invalid);
		mLineInfo.SetLineInfoP_FirstLocalIdentifierUniqID(lineIndex,kUniqID_Invalid);
		mLineInfo.SetLineInfoP_FirstBlockStartDataUniqID(lineIndex,kUniqID_Invalid);
		mLineInfo.ClearRecognizeFlags(lineIndex);//#693
		//��ԏ���
		mLineInfo.SetLineInfoP_StateIndex(lineIndex,0);
		mLineInfo.SetLineInfoP_PushedStateIndex(lineIndex,0);
	}
}

#pragma mark ===========================

#pragma mark ---���ʎq���

//�w��s��Identifier���폜����
void	ATextInfo::DeleteLineIdentifiers( const AIndex inLineIndex, AArray<AUniqID>& outDeletedGlobalIdentifiers, ABool& outImportFileIdentifierDeleted )
{
	//#695 outImportFileIdentifierDeleted = false;
	
	//Global
	{
		AUniqID	firstGlobalIdentifierUniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(inLineIndex);
		//�폜����Identifier��UniqID��outDeletedIdentifiers�Ɋi�[
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
		//�폜���s
		if( firstGlobalIdentifierUniqID != kUniqID_Invalid )
		{
			{
				//#717
				//�O���[�o�����ʎq�r������
				AStMutexLocker	locker(mGlobalIdentifierListMutex);
				//�O���[�o�����ʎq�폜
				mGlobalIdentifierList.DeleteIdentifiers(firstGlobalIdentifierUniqID);
			}
			mLineInfo.SetLineInfo_FirstGlobalIdentifierUniqID(inLineIndex,kUniqID_Invalid);
		}
	}
	
	//Local
	if( mLineInfo.IsPurged() == false )//#693
	{
		AUniqID	firstLocalIdentifierUniqID = mLineInfo.GetLineInfoP_FirstLocalIdentifierUniqID(inLineIndex);
		//�폜���s
		if( firstLocalIdentifierUniqID != kUniqID_Invalid )
		{
			{
				//#717
				//���[�J�����ʎq�r������
				AStMutexLocker	locker(mLocalIdentifierListMutex);
				//���[�J�����ʎq�폜
				mLocalIdentifierList.DeleteIdentifiers(firstLocalIdentifierUniqID);
			}
			mLineInfo.SetLineInfoP_FirstLocalIdentifierUniqID(inLineIndex,kUniqID_Invalid);
		}
	}
	//BlockStartData
	if( mLineInfo.IsPurged() == false )//#693
	{
		AUniqID	firstUniqID = mLineInfo.GetLineInfoP_FirstBlockStartDataUniqID(inLineIndex);
		//�폜���s
		if( firstUniqID != kUniqID_Invalid )
		{
			mBlockStartDataList.DeleteIdentifiers(firstUniqID);
			mLineInfo.SetLineInfoP_FirstBlockStartDataUniqID(inLineIndex,kUniqID_Invalid);
		}
	}
}

//�w��s�ɁAIdentifier��ǉ�����B
AUniqID	ATextInfo::AddLineGlobalIdentifier( const AIdentifierType inType, 
		/*#698 const ATextIndex inStartIndex, const ATextIndex inEndIndex*/
		const AIndex inLineIndex, const AIndex inXSpos, const AIndex inXEpos, //#698
		const AIndex inCategoryIndex, 
		const AText& inKeywordText, const AText& inMenuText, const AText& inInfoText,
		const AText& inCommentText, const AText& inParentKeywordText, const AText& inTypeText,
		const ABool inLocalRangeDelimiter, const AIndex inOutlineLevel )//RC2 #593 #130
{
	//���ʎq������#695
	if( mGlobalIdentifierList.GetItemCount() >= kLimit_MaxGlobalIdentifiers )   return kUniqID_Invalid;
	
	//IdentifierList�ɒǉ�
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
	//lineIndex�̍Ō�̃I�u�W�F�N�g������
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
	//menu text�擾
	AText	menuText;
	menuText.SetText(inMenuText);
	//menu text��~!anchor�̂Ƃ��́A�A���J�[�t���O��ݒ肵�Amenu text�͋�ɂ���
	if( menuText.Compare("~!anchor") == true )
	{
		mLineInfo.SetLineInfoP_AnchorIdentifierExist(lineIndex,true);
		menuText.DeleteAll();
	}
	//
	AUniqID	UniqID = kUniqID_Invalid;//#717
	{
		//#717
		//�O���[�o�����ʎq�r������
		AStMutexLocker	locker(mGlobalIdentifierListMutex);
		//�O���[�o�����ʎq�ǉ�
		UniqID = mGlobalIdentifierList.AddIdentifier(mLineInfo.GetLineUniqID(lineIndex),//#695
													 last,inType,inCategoryIndex,inKeywordText,
													 menuText,inInfoText,spos,epos,false,
													 inCommentText,inParentKeywordText,inTypeText,
													 GetEmptyText(),GetEmptyText(),
													 inLocalRangeDelimiter,inOutlineLevel,kIndex_Invalid);//RC2 #593 #130
	}
	//previous��kUniqID_Invalid�̏ꍇ�́A�s�ŏ��̎��ʎq�Ȃ̂ŁAmLineInfo_FirstGlobalIdentifierUniqID��Set
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
	//���[�J���f���~�^�̏ꍇ�́A���[�J���f���~�^�t���O��ON
	if( inLocalRangeDelimiter == true )
	{
		mLineInfo.SetLineInfoP_LocalDelimiterExist(lineIndex,true);
	}
	return UniqID;
}

//�w��s�ɁAIdentifier��ǉ�����B
AUniqID	ATextInfo::AddLineLocalIdentifier( const AIdentifierType inType, 
		/*#698 const ATextIndex inStartIndex, const ATextIndex inEndIndex*/
		const AIndex inLineIndex, const AIndex inXSpos, const AIndex inXEpos, //#698
		const AIndex inCategoryIndex, 
			const AText& inKeywordText, const AText& inInfoText, const AText& inTypeText )//R0243
{
	//���ʎq������#695
	if( mLocalIdentifierList.GetItemCount() >= kLimit_MaxLocalIdentifiers )   return kUniqID_Invalid;
	
	//IdentifierList�ɒǉ�
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
	//lineIndex�̍Ō�̃I�u�W�F�N�g������
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
		//���[�J�����ʎq�r������
		AStMutexLocker	locker(mLocalIdentifierListMutex);
		//
		UniqID = mLocalIdentifierList.AddIdentifier(mLineInfo.GetLineUniqID(lineIndex),//#695
													last,inType,inCategoryIndex,inKeywordText,
													GetEmptyText(),inInfoText,spos,epos,false,
													GetEmptyText(),GetEmptyText(),inTypeText,
													GetEmptyText(),GetEmptyText(),
													false,0,kIndex_Invalid);//RC2, R0243 #593
	}
	//previous��kUniqID_Invalid�̏ꍇ�́A�s�ŏ��̎��ʎq�Ȃ̂ŁAmLineInfoP_FirstLocalIdentifierUniqID��Set
	if( last == kUniqID_Invalid )
	{
		if( mLineInfo.GetLineInfoP_FirstLocalIdentifierUniqID(lineIndex) != kUniqID_Invalid )   _ACThrow("not the first of line identifier",this);
		mLineInfo.SetLineInfoP_FirstLocalIdentifierUniqID(lineIndex,UniqID);
	}
	return UniqID;
}

/**
�w�莯�ʎq�����݂���s��Index��Ԃ�
*/
AIndex	ATextInfo::GetLineIndexByGlobalIdentifier( const AUniqID inIdentifierUniqID ) const
{
	//IdentifierList�Ɏ��ʎq���ƂɊi�[���Ă���lineUniqID���擾
	AUniqID	lineUniqID = mGlobalIdentifierList.GetLineUniqID(inIdentifierUniqID);
	//lineUniqID����lineIndex���擾
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
�w�莯�ʎq�����݂���s��Paragraph Index��Ԃ�
*/
AIndex	ATextInfo::GetParagraphIndexByGlobalIdentifier( const AUniqID inIdentifierUniqID ) const
{
	//IdentifierList�Ɏ��ʎq���ƂɊi�[���Ă���lineUniqID���擾
	AUniqID	lineUniqID = mGlobalIdentifierList.GetLineUniqID(inIdentifierUniqID);
	//lineUniqID����paragraph index���擾
	return mLineInfo.GetParagraphIndexFromLineUniqID(lineUniqID);
}


/**
�s�������郁�j���[���ʎq���擾
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
//inLineIndex����O�ɑk���čŏ��Ɍ��������A���j���[�o�^����鎯�ʎq��UniqID�ƍs�ԍ����擾
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
//���j���[���ʎq�̃��j���[�e�L�X�g����A������`���Ă���s�̍s�ԍ����擾����
AIndex	ATextInfo::FindLineIndexByMenuText( const AText& inMenuName ) const
{
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		//�e�s�ɐݒ肳�ꂽ�O���[�o�����ʎq�Е������X�g����MenuIdentifier����������
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex);
		while( UniqID != kUniqID_Invalid )
		{
			if( IsMenuIdentifier(UniqID) == true )
			{
				//���j���[�e�L�X�g���r���A��v�Ȃ炻�̍s�ԍ���Ԃ�
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
//���j���[���ʎq�̃L�[���[�h�e�L�X�g����A������`���Ă���s�̍s�ԍ����擾����
AIndex	ATextInfo::FindLineIndexByMenuKeywordText( const AText& inMenuName ) const
{
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		//�e�s�ɐݒ肳�ꂽ�O���[�o�����ʎq�Е������X�g����MenuIdentifier����������
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(lineIndex);
		while( UniqID != kUniqID_Invalid )
		{
			if( IsMenuIdentifier(UniqID) == true )
			{
				//���j���[�̃L�[���[�h�e�L�X�g���r���A��v�Ȃ炻�̍s�ԍ���Ԃ�
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
�s��������Local�͈͂̊J�n���ʎq���擾
*/
AUniqID	ATextInfo::GetLocalStartIdentifierByLineIndex( const AIndex inLineIndex ) const
{
	return mLineInfo.FindCurrentLocalStartIdentifier(inLineIndex);
}

//#717
/**
�s��������Local�͈͂̊J�n���ʎq�̐e�L�[���[�h���擾�i�T�^�I�ɂ͌��݂̊֐��̃N���X�j
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
	//�����x���P���� #695
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
�s��������Anchor���ʎq���擾
*/
AUniqID	ATextInfo::GetAnchorIdentifierByLineIndex( const AIndex inLineIndex ) const
{
	return mLineInfo.FindAnchorIdentifier(inLineIndex);
}

#pragma mark ===========================

#pragma mark ---�O���[�o�����ʎq���擾

/**
Global���ʎq�̒�`�ʒu�i�͈́j�擾
@note ���xO(�s��)
*/
void	ATextInfo::GetGlobalIdentifierRange( const AUniqID inIdentifierUniqID, ATextPoint& outStart, ATextPoint& outEnd ) const
{
	outStart.y = outEnd.y = GetLineIndexByGlobalIdentifier(inIdentifierUniqID);
	outStart.x = mGlobalIdentifierList.GetStartIndex(inIdentifierUniqID);
	outEnd.x = mGlobalIdentifierList.GetEndIndex(inIdentifierUniqID);
}

//�L�[���[�h�����񂩂�O���[�o�����ʎq��UniqID���擾
AUniqID	ATextInfo::GetGlobalIdentifierByKeywordText( const AText& inKeywordText ) const
{
	//R0000 �J�e�S���[�� �� 2�Ԗڂ̈�����true�Ȃ̂�tmpArray�͎g���Ȃ�
	ABoolArray	tmpArray;
	return mGlobalIdentifierList.GetIdentifierByKeywordText(inKeywordText,true,tmpArray);
}

//�L�[���[�h�����񂩂�O���[�o�����ʎq��UniqID�̃��X�g���擾 RC2
void	ATextInfo::GetGlobalIdentifierListByKeywordText( const AText& inKeywordText, AArray<AUniqID>& outUniqIDArray ) const
{
	return mGlobalIdentifierList.GetIdentifierListByKeyword(inKeywordText,outUniqIDArray);
}

AIndex	ATextInfo::GetGlobalIdentifierCategoryIndexByKeywordText( const AText& inKeywordText ) const
{
	//R0000 �J�e�S���[�� ��
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
�A�E�g���C�����x���擾
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
//�ȗ����͌��擾�i�f�[�^�͈����̔z��֒ǉ������j�i�O���[�o���j
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

//�ȗ����͌��擾�i�f�[�^�͈����̔z��֒ǉ������j�i�O���[�o���jR0243
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
�e��L�[���[�h�����X���b�h�p�����L�[���[�h��������(Global���ʎq)
@note �n�b�V�����g��Ȃ��̂Œᑬ�Ȃ��߁A�X���b�h�g�p���K�{�����A���낢��Ȍ������@���ł���B
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
	//�O���[�o�����ʎq�r������
	AStMutexLocker	locker(mGlobalIdentifierListMutex);
	//�������s�O�̍��ڌ����L��
	AIndex	startIndex = outKeywordArray.GetItemCount();
	//�O���[�o�����ʎq����L�[���[�h����
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
	//���񌟍��Œǉ��������ɂ���spos, epos, scope��ݒ�
	for( AIndex i = startIndex; i < outKeywordArray.GetItemCount(); i++ )
	{
		//AIdentifierList::SearchKeyword()����擾����startIndex/endIndex�͍s����x�ʒu�Ȃ̂ŁA�e�L�X�gindex�ɕϊ�����B
		AIndex	lineStart = mLineInfo.GetLineInfo_Start_FromLineUniqID(lineUniqIDArray.Get(i-startIndex));
		outStartIndexArray.Set(i,outStartIndexArray.Get(i)+lineStart);
		outEndIndexArray.Set(i,outEndIndexArray.Get(i)+lineStart);
		//scope�ݒ�
		outScopeArray.Add(kScopeType_Global);
	}
	return result;
}

//#853
/**
�e��L�[���[�h�����X���b�h�p�����L�[���[�h��������(Local���ʎq)
@note �n�b�V�����g��Ȃ��̂Œᑬ�Ȃ��߁A�X���b�h�g�p���K�{�����A���낢��Ȍ������@���ł���B
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
	//���[�J�����ʎq�r������
	AStMutexLocker	locker(mLocalIdentifierListMutex);
	//�������s�O�̍��ڌ����L��
	AIndex	startIndex = outKeywordArray.GetItemCount();
	//���[�J���͈͓���lineUniqID��limitLineUniqIDArray�Ɋi�[�i�����Ώۍs������j
	AHashArray<AUniqID>	limitLineUniqIDArray;
	for( AIndex lineIndex = inStartLineIndex; lineIndex < inEndLineIndex; lineIndex++ )
	{
		limitLineUniqIDArray.Add(mLineInfo.GetLineUniqID(lineIndex));
	}
	//���[�J�����ʎq���猟��
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
	//���񌟍��Œǉ��������ɂ���spos, epos, scope��ݒ�
	for( AIndex i = startIndex; i < outKeywordArray.GetItemCount(); i++ )
	{
		//AIdentifierList::SearchKeyword()����擾����startIndex/endIndex�͍s����x�ʒu�Ȃ̂ŁA�e�L�X�gindex�ɕϊ�����B
		AIndex	lineStart = mLineInfo.GetLineInfo_Start_FromLineUniqID(lineUniqIDArray.Get(i-startIndex));
		outStartIndexArray.Set(i,outStartIndexArray.Get(i)+lineStart);
		outEndIndexArray.Set(i,outEndIndexArray.Get(i)+lineStart);
		//scope�ݒ�
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
�O���[�o�����ʎq��color slot���擾
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

#pragma mark ---���[�J�����ʎq���擾

//�L�[���[�h�����񂩂烍�[�J�����ʎq��UniqID���擾
AUniqID	ATextInfo::GetLocalIdentifierByKeywordText( const AText& inKeywordText ) const
{
	//R0000 �J�e�S���[�� ��
	ABoolArray	tmpArray;
	return mLocalIdentifierList.GetIdentifierByKeywordText(inKeywordText,true,tmpArray);
}

//�L�[���[�h�����񂩂烍�[�J�����ʎq��UniqID�̃��X�g���擾
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
	//R0000 �J�e�S���[�� ��
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

//�w�莯�ʎq�����݂���s��Index��Ԃ�
AIndex	ATextInfoForDocument::GetLineIndexByLocalIdentifier( const AUniqID inIdentifierUniqID ) const
{
	//IdentifierList�Ɏ��ʎq���ƂɊi�[���Ă���lineUniqID���擾
	AUniqID	lineUniqID = mLocalIdentifierList.GetLineUniqID(inIdentifierUniqID);
	//lineUniqID����lineIndex���擾
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
	//BlockStart������#695
	if( mBlockStartDataList.GetItemCount() >= kLimit_MaxBlockStartData )   return;
	
	//lineIndex�̍Ō�̃I�u�W�F�N�g������
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
	//previous��kObjectID_Invalid�̏ꍇ�́A�s�ŏ��̎��ʎq�Ȃ̂ŁAmLineInfoP_BlockStartDataObjectID��Set
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
	//�����x����
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

#pragma mark ---Import�t�@�C��

//Import�t�@�C���擾
void	ATextInfo::GetImportFileArray( const AFileAcc& inBaseFolder, AObjectArray<AFileAcc>& outFileArray )
{
	//#695 �������B�Ώ�suffix�������������ă��[�J���ϐ��Ɋi�[�B
	const AHashTextArray&	sa = GetApp().SPI_GetModePrefDB(mModeIndex).GetSuffixArray();
	ATextArray	suffixarray;
	for( AIndex i = 0; i < sa.GetItemCount(); i++ )
	{
		AText	suffix;
		sa.Get(i,suffix);
		suffix.ChangeCaseLower();
		suffixarray.Add(suffix);
	}
	
	//#271 �ǉ��ςݔ���p�p�X������z��𐶐�
	AHashTextArray	pathArray;
	for( AIndex i = 0; i < outFileArray.GetItemCount(); i++ )
	{
		AText	path;
		outFileArray.GetObjectConst(i).GetNormalizedPath(path);
		pathArray.Add(path);
	}
	//�e�s���̃��[�v�iimport file identifier�������j
	for( AIndex index = 0; index < GetLineCount(); index++ )
	{
		AUniqID	UniqID = mLineInfo.GetLineInfo_FirstGlobalIdentifierUniqID(index);
		while( UniqID != kUniqID_Invalid )
		{
			if( GetGlobalIdentifierType(UniqID) == kIdentifierType_ImportFile )
			{
				//ImportFile���ʎq�i#include���j���������ꍇ�A���̃t�@�C����outFileArray�ɒǉ�����B
				//�������A���ɒǉ��ς݂̏ꍇ�A�t�@�C�������݂��Ă��Ȃ��ꍇ�́A�ǉ����Ȃ��B
				//�܂��A�g���q�݂̂�ς����t�@�C�������݂��Ă���΂�����ǉ��i���v�����j
				
				//�t�@�C���擾
				AText	text;
				GetGlobalIdentifierInfoText(UniqID,text);
				AFileAcc	file;
				file.SpecifyChild(inBaseFolder,text);//B0389 OK ,kFilePathType_1);kFilePathType_Default�ł�../.���߂ł���̂Ŗ��Ȃ��B
				//#695 �������̂��߁A���ֈړ��iExist����ɂ͎��Ԃ�������j if( file.Exist() == true )
				//#695 {
				//outFileArray�ɂ��łɒǉ��ς݂��ǂ������ׂ� win 080714
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
				//�ǉ��ς݂łȂ����outFileArray�֒ǉ�
				if( /*#271alreadyadded == false*/pathArray.Find(path) == kIndex_Invalid )
				{
					//�t�@�C�������݂��Ă���΁AoutFileArray�ɒǉ��B
					if( file.Exist() == true )//#695 �������̂��ߏォ��ړ�
					{
						outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(file);
					}//#695
					//�ǉ��ς�array�ɒǉ�
					pathArray.Add(path);//#271
					//#695 }
					//#695 }
					//#695 }
					//win 080714
					
					//�g���q��ς����t�@�C�����ΏۂƂ���
					//���g���q��ς����t�@�C�����ΏۂƂ��Ăق����Ȃ��v�]�Ή�����
					AText	pathwithoutsuffix;
					pathwithoutsuffix.SetText(text);
					AText	originalsuffix;
					pathwithoutsuffix.GetSuffix(originalsuffix);
					pathwithoutsuffix.DeleteAfter(pathwithoutsuffix.GetItemCount()-originalsuffix.GetItemCount());
					//���[�h�ݒ�̊e�g���q���ɒ��ׂ�
					//#693 const AHashTextArray&	suffixarray = GetApp().SPI_GetModePrefDB(mModeIndex).GetSuffixArray();
					for( AIndex i = 0; i < suffixarray.GetItemCount(); i++ )
					{
						//���[�h�ݒ肩��g���q��ǂݏo��
						AText	suffix;
						suffixarray.Get(i,suffix);
						//#693 suffix.SetText(suffixarray.GetTextConst(i));
						if( suffix.Compare(originalsuffix) == true )   continue;
						//#693 suffix.ChangeCaseLower();
						//���̊g���q�ɕς����t�@�C�����̃t�@�C�������݂��Ă���΂��̃t�@�C����outFileArray�ɒǉ�����
						AText	t;
						t.AddText(pathwithoutsuffix);
						t.AddText(suffix);
						AFileAcc	f;
						f.SpecifyChild(inBaseFolder,t);
						//#695 �������̂��߁A���ֈړ� if( f.Exist() == true )
						//#695 {
						//outFileArray�ɂ��łɒǉ��ς݂��ǂ������ׂ�
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
						//�ǉ��ς݂łȂ����outFileArray�֒ǉ�
						if( /*#271alreadyadded == false*/pathArray.Find(path) == kIndex_Invalid )
						{
							//�t�@�C�������݂��Ă���΁AoutFileArray�ɒǉ��B
							if( f.Exist() == true )//#695 �������̂��ߏォ��ړ�
							{
								outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(f);
							}//#695
							//�ǉ��ς�array�ɒǉ�
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
	//#693 RegExp�I�u�W�F�N�g��Purge����Ă�����ēxRegExp���X�V
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

#pragma mark ---�܂肽����
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
		if( nextUniqID == UniqID )   {_ACError("object ID list error (loop)",this);break;}//#546 �������[�v�h�~
		UniqID = nextUniqID;//#546
	}
	//
	return result;
}
*/

#pragma mark ===========================

#pragma mark ---���[�h���X�g
//#723

/**
���[�h���X�g�X�V�i�����j
@param inText �h�L�������g�̃e�L�X�g
@note thread-safe
*/
void	ATextInfo::UpdateWordsList( const AText& inText )
{
	//���[�h���X�g�r������
	AStMutexLocker	locker(mWordsListMutex);
	//�S�폜
	mWordsList_Word.DeleteAll();
	mWordsList_WordStartTextIndex.DeleteAll();
	
	//�Q�Ǝ擾�i�����́A�A�v���N����A���̍폜�E�ړ�������邱�Ƃ͂Ȃ��j
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(mModeIndex);
	ASyntaxDefinition&	syntaxDefinition = modePrefDB.GetSyntaxDefinition();
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
	
	//�e�s���̃��[�v
	AText	token;
	AItemCount	lineCount = GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		//�s���擾
		AItemCount	startpos = GetLineStart(lineIndex);
		AItemCount	len = GetLineLengthWithoutLineEnd(lineIndex);
		AIndex	endpos = startpos + len;
		//���@�p�[�gindex�擾�i����FindWord()�̑�V������true�Ȃ̂ŁA���@�p�[�g�ɂ���ĒP���`���قȂ�B�j
		AIndex	stateIndex = mLineInfo.GetLineInfoP_StateIndex(lineIndex);
		AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
		
		//���������[�v
		for( AIndex pos = startpos; pos < endpos;  )
		{
			//�P����擾
			AIndex	tokenspos = pos;
			ATextIndex	start = 0, end = 0;
			ABool	isAlphabet = FindWord(inText,mModeIndex,true,tokenspos,start,end,true,syntaxPartIndex);
			if( end > pos )
			{
				//pos��end�ɐi�߂�
				pos = end;
				//�A���t�@�x�b�g�ō\�������token�̏ꍇ�̂݃��[�h���X�g�ɒǉ�
				if( isAlphabet == true )
				{
					inText.GetText(tokenspos,end-tokenspos,token);
					//���[�h���X�g�ɒǉ�
					mWordsList_Word.Add(token);
					mWordsList_WordStartTextIndex.Add(tokenspos);
				}
			}
			else
			{
				//pos�����̕����ɐi�߂�
				pos = inText.GetNextCharPos(pos);
			}
		}
	}
}

/**
���[�h���X�g����
@note thread-safe
*/
void	ATextInfo::FindWordsList( const AText& inWord, AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray ) const
{
	//���[�h���X�g�r������
	AStMutexLocker	locker(mWordsListMutex);
	//���[�h���X�g���猟��
	AArray<AIndex>	indexArray;
	mWordsList_Word.FindAll(inWord,0,inWord.GetItemCount(),indexArray);
	//�������ʍ��ږ��̃��[�v
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		//���ʊi�[
		AIndex	index = indexArray.Get(i);
		AIndex	spos = mWordsList_WordStartTextIndex.Get(index);
		outStartIndexArray.Add(spos);
		outEndIndexArray.Add(spos+inWord.GetItemCount());
	}
}

/**
���[�h���X�g�f�[�^�S�폜
*/
void	ATextInfo::DeleteAllWordsList()
{
	//���[�h���X�g�r������
	AStMutexLocker	locker(mWordsListMutex);
	//
	mWordsList_Word.DeleteAll();
	mWordsList_WordStartTextIndex.DeleteAll();
}

/**
�f�o�b�O�p�iTextInfo�f�[�^�������`�F�b�N�j
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

#pragma mark ---�q���g�e�L�X�g
//

/**
�q���g�e�L�X�g�ǉ�
*/
AIndex	ATextInfo::AddHintText( const ATextIndex inTextIndex, const AText& inHintText )
{
	mHintTextArray_TextIndex.Add(inTextIndex);
	mHintTextArray_Text.Add(inHintText);
	return mHintTextArray_TextIndex.GetItemCount() -1;
}

/**
�q���g�e�L�X�g�S�폜
*/
void	ATextInfo::ClearHintText()
{
	mHintTextArray_TextIndex.DeleteAll();
	mHintTextArray_Text.DeleteAll();
}

/**
�q���g�e�L�X�g�擾
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
�q���g�e�L�X�g�X�V�iInsertText���j
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
�q���g�e�L�X�g�X�V�iDeleteText���j
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
�q���g�e�L�X�gpos�擾
*/
ATextIndex	ATextInfo::GetHintTextPos( const AIndex inHintTextIndex ) const
{
	return mHintTextArray_TextIndex.Get(inHintTextIndex);
}

/**
�q���g�e�L�X�g�ʒu����index�擾
*/
AIndex	ATextInfo::FindHintIndexFromTextIndex( const ATextIndex inHintTextPos ) const
{
	return mHintTextArray_TextIndex.Find(inHintTextPos);
}

/*#695
#pragma mark ===========================
#pragma mark [�N���X]ABlockStartDataList
#pragma mark ===========================

#pragma mark ---

AObjectID	ABlockStartDataList::Add( const AObjectID inPreviousID, const AIndex inVariableIndex, const AItemCount inIndentCount )
{
	//�z��֒ǉ�
	ABlockStartDataFactory	factory(this,inVariableIndex,inIndentCount);
	AIndex	index = mArray.AddNewObject(factory);
	AObjectID	addedObjectID = mArray.GetObjectConst(index).GetObjectID();
	
	//���X�g�ڑ�
	//inPreviousIdentifierObjectID��kObjectID_Invalid�łȂ��ꍇ�i�����X�g�擪�ȊO�j�A�O��Next�Ɏ��I�u�W�F�N�g��ݒ肷��B
	if( inPreviousID != kObjectID_Invalid )
	{
		//�O��Identifier��NextObjectID�ɁA�ǉ�����Identifier��ObjectID��ݒ肷��B
		if( mArray.GetObjectByID(inPreviousID).GetNextObjectID() == kObjectID_Invalid )
		{
			mArray.GetObjectByID(inPreviousID).SetNextObjectID(addedObjectID);
		}
		else
		{
			//prev��ObjectID�ݒ�ς݁i�Ō���ǉ��łȂ��j�ꍇ��throw
			_ACThrow("not last object",this);
		}
	}
	
	return addedObjectID;
}

//���ʎq�폜
//���X�g�ɘA�Ȃ鎯�ʎq��S�č폜����B
void	ABlockStartDataList::DeleteIdentifiers( const AObjectID inFirstObjectID )
{
	//�܂����̎��ʎq�ȍ~���폜�i�ċA�j
	AObjectID	nextObjectID = mArray.GetObjectByID(inFirstObjectID).GetNextObjectID();
	if( nextObjectID == inFirstObjectID )   {_ACError("object ID list error (loop)",this);return;}//#547
	if( nextObjectID != kObjectID_Invalid )
	{
		DeleteIdentifiers(nextObjectID);
	}
	//���g���폜
	//index�͂����ōēx��������K�v������B�i�ċA�R�[���Ŏ��ȍ~���폜�ς݂Ȃ̂ŁA�ŏ��Ƃ�Index���ς���Ă���B�j
	AIndex	index = mArray.GetIndexByID(inFirstObjectID);//�����K�v�i���x�j
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