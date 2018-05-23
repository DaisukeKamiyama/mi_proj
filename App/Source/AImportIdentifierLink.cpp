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

AImportIdentifierLinkList

*/

#include "stdafx.h"

#include "AImportIdentifierLink.h"
#include "AApp.h"
#include "AImportFileManager.h"
#include "ATextInfo.h"

/*
�n�b�V���T�C�Y��10�{�Ƃ���B
Import�̏ꍇ�A�������O�̃L�[���[�h���d�����邱�Ƃ������A�n�b�V�����c�q�ɂȂ�₷��
���̂��߁A�n�b�V���T�C�Y��傫�߂ɂ��āA�c�q�ɂԂ���\����Ⴍ����B
��AApp.cpp�ł̗၄
�n�b�V���T�C�Y�F158K(10�{)
FindCount: 21544
CompareCount: 14124
HitCount: 3069
MaxContinuous: 976
�����x��FindCount�̏ꍇ�ɁA
�n�b�V���T�C�Y5�{�ŁACompareCount: 98114
�n�b�V���T�C�Y4�{�ŁACompareCount: 306280

*/
const AItemCount	kImportIdentifierLinkListHashMultiple = 10;

//�������̂��߁AAImportIdentifierLink�N���X�͍폜���܂����B
//�iAImportIdentifierLinkList���ɔz��Ƃ��ăf�[�^��ێ�����悤�ɕύX�j

#pragma mark ===========================
#pragma mark [�N���X]AImportIdentifierLinkList
#pragma mark ===========================
//�C���|�[�g���ʎq�����N���X�g
//�e�e�L�X�g�h�L�������g����������I�u�W�F�N�g�B
//�C���|�[�g�t�@�C���̎��ʎq���ւ̃����N����ێ�����B
//���L�̂Q��private�����o�z��Ƀ����N�f�[�^���ۑ������B
//�����N�f�[�^�i�n�b�V���j
//ImportFileData�ւ̃|�C���^
//AArray<AImportFileData*>	mHash_ImportFileData;
//ImportFileData���̃C���f�b�N�X
//AArray<AIndex>				mHash_IdentifierIndex;


#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AImportIdentifierLinkList::AImportIdentifierLinkList() : AObjectArrayItem(NULL)
{
}
//�f�X�g���N�^
AImportIdentifierLinkList::~AImportIdentifierLinkList()
{
}

#pragma mark ===========================

#pragma mark ---�ǉ��^�폜

//�����z��Ɋi�[���ꂽObjectID��ImportFileData�i�t�@�C�����ɑ��݂���I�u�W�F�N�g�j�ւ̃����N��S�Ēǉ�����
void	AImportIdentifierLinkList::UpdateIdentifierLinkList( const AHashArray<AObjectID>& inImportFileDataObjectIDArray )//#423
{
	//
	AItemCount	totalIdentifierCount = 0;
	for( AIndex i = 0; i < inImportFileDataObjectIDArray.GetItemCount(); i++ )
	{
		AObjectID	importFileDataObjectID = inImportFileDataObjectIDArray.Get(i);
		totalIdentifierCount += GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetTextInfoConst().
				GetGlobalIdentifierListCount();
		/*
		//
		AText	filename;
		GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetFile().GetFilename(filename);
		AText	t;
		t.SetFormattedCstring("Arg:%d:",importFileDataObjectID.val);
		t.AddText(filename);
		t.Add(13);
		t.OutputToStderr();
		*/
	}
	//#423
	if( mHash_ImportFileDataID.GetItemCount() <= 
				totalIdentifierCount*(kImportIdentifierLinkListHashMultiple/2) )
	{
		//==================�n�b�V���č쐬==================
		//���݂̃n�b�V���T�C�Y��identifier�̐���5�{�����Ȃ�n�b�V����V�K�쐬����
		
		//
		InitHash(totalIdentifierCount);
		//�n�b�V���͐V�K�쐬����A����ImportFileData��ǉ�����̂ŁArevision data���S�폜
		mRevisionData_ImportFileDataID.DeleteAll();
		mRevisionData_RevisonNumber.DeleteAll();
		//Hash�ɓo�^
		//�eImportFileData���Ƃ̃��[�v
		for( AIndex i = 0; i < inImportFileDataObjectIDArray.GetItemCount(); i++ )
		{
			AObjectID	importFileDataObjectID = inImportFileDataObjectIDArray.Get(i);
			//#423 AImportFileData*	importFileData = &(GetApp().SPI_GetImportFileManager().GetImportFileDataByID(importFileDataObjectID));
			//#423 AItemCount	identifierCount = importFileData->GetTextInfoConst().GetGlobalIdentifierListCount();
			const AImportFileData&	importFileData = GetApp().SPI_GetImportFileManager().
									GetImportFileDataConstByID(importFileDataObjectID);
			AItemCount	identifierCount = importFileData.GetTextInfoConst().GetGlobalIdentifierListCount();//#423
			//ImportFileData���̑S�Ă�identifier��o�^
			for( AIndex j = 0; j < identifierCount; j++ )
			{
				RegisterHash(/*#423 importFileData*/importFileDataObjectID,j);
			}
			//RevisionData�o�^
			mRevisionData_ImportFileDataID.Add(importFileDataObjectID);
			mRevisionData_RevisonNumber.Add(importFileData.GetRevisionNumber());
			/*
			//
			AText	filename;
			GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetFile().GetFilename(filename);
			AText	t;
			t.SetFormattedCstring("NewReg:%d:",importFileDataObjectID.val);
			t.AddText(filename);
			t.Add(13);
			t.OutputToStderr();
			*/
		}
	}
	else
	{
		//==================�n�b�V���ɒǉ��o�^==================
		//���݂̃n�b�V���T�C�Y��identifier�̐���5�{�ȏ�Ȃ�ARevision���������͍̂X�V���Ȃ�
		
		//�V�KimportFileData�ɂ��āA
		//1. mRevisionData_ImportFileDataID�ɂȂ���΁A���̂܂܃n�b�V���o�^���A
		//revisionData��o�^
		//2. mRevisionData_ImportFileDataID�ɂ����āARevision�������Ȃ�A�������Ȃ�
		//3. mRevisionData_ImportFileDataID�ɂ����āARevision���Ⴄ�Ȃ�A
		//�n�b�V�����炻��importFileDataID�̃f�[�^���폜��A�n�b�V���o�^���A
		//revisionData���X�V
		for( AIndex i = 0; i < inImportFileDataObjectIDArray.GetItemCount(); i++ )
		{
			AObjectID	importFileDataObjectID = inImportFileDataObjectIDArray.Get(i);
			const AImportFileData&	importFileData = GetApp().SPI_GetImportFileManager().
									GetImportFileDataConstByID(importFileDataObjectID);
			AItemCount	identifierCount = importFileData.GetTextInfoConst().GetGlobalIdentifierListCount();//#423
			AIndex	revisionDataIndex = mRevisionData_ImportFileDataID.Find(importFileDataObjectID);
			if( revisionDataIndex == kIndex_Invalid )
			{
				//------------------�V�KImportFileData�o�^�̏ꍇ------------------
				
				//�n�b�V���o�^
				for( AIndex j = 0; j < identifierCount; j++ )
				{
					RegisterHash(importFileDataObjectID,j);
				}
				//RevisionData�o�^
				mRevisionData_ImportFileDataID.Add(importFileDataObjectID);
				mRevisionData_RevisonNumber.Add(importFileData.GetRevisionNumber());
				/*
				//
				AText	filename;
				GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetFile().GetFilename(filename);
				AText	t;
				t.SetFormattedCstring("NewReg:%d:",importFileDataObjectID.val);
				t.AddText(filename);
				t.Add(13);
				t.OutputToStderr();
				*/
			}
			else
			{
				if( importFileData.GetRevisionNumber() == mRevisionData_RevisonNumber.Get(revisionDataIndex) )
				{
					//------------------�o�^�ς�ImportFileData�̏ꍇ�irevision�ɕω������j------------------
					
					//�������Ȃ�
				}
				else
				{
					//------------------�o�^�ς�ImportFileData�̏ꍇ�irevision�ɕω�����j------------------
					
					//�n�b�V������폜
					DeleteFromHash(importFileDataObjectID);
					//�n�b�V���o�^
					for( AIndex j = 0; j < identifierCount; j++ )
					{
						RegisterHash(importFileDataObjectID,j);
					}
					//RevisionData�X�V
					mRevisionData_RevisonNumber.Set(revisionDataIndex,importFileData.GetRevisionNumber());
					/*
					//
					AText	filename;
					GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetFile().GetFilename(filename);
					AText	t;
					t.SetFormattedCstring("Update:%d:",importFileDataObjectID.val);
					t.AddText(filename);
					t.Add(13);
					t.OutputToStderr();
					*/
				}
			}
		}
	}
	//==================�����inImportFileDataObjectIDArray�Ɋ܂܂�Ȃ�Import File Data���폜==================
	//inImportFileDataObjectIDArray�ɂȂ����Arevisiondata�ɂ���f�[�^�́A�n�b�V���ErevisionData����폜����B
	for( AIndex i = 0; i < mRevisionData_ImportFileDataID.GetItemCount(); )
	{
		AObjectID	importFileDataObjectID = mRevisionData_ImportFileDataID.Get(i);
		if( inImportFileDataObjectIDArray.Find(importFileDataObjectID) == kIndex_Invalid )
		{
			//����̐V�K�f�[�^�ɂ͊܂܂�Ȃ��̂ŁA�n�b�V���ErevisionData����폜����
			
			//�n�b�V������폜
			DeleteFromHash(importFileDataObjectID);
			//RevisionData�폜
			mRevisionData_ImportFileDataID.Delete1(i);
			mRevisionData_RevisonNumber.Delete1(i);
			/*
			//
			AText	filename;
			GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataObjectID).GetFile().GetFilename(filename);
			AText	t;
			t.SetFormattedCstring("Del:%d:",importFileDataObjectID.val);
			t.AddText(filename);
			t.Add(13);
			t.OutputToStderr();
			*/
		}
		else
		{
			//����
			i++;
		}
	}
}

//�����N�f�[�^�S�č폜
void	AImportIdentifierLinkList::DeleteAll()
{
	//#423 mHash_ImportFileData.DeleteAll();
	mHash_ImportFileDataID.DeleteAll();//#423
	mHash_IdentifierIndex.DeleteAll();
}

//�w��ObjectID��ImportFileData���̃f�[�^�ւ̃����N��S�č폜
ABool	AImportIdentifierLinkList::Delete( const AObjectID inImportFileDataObjectID )
{
	//==================�����N���X�g���ɑ��݂���Import File Data���ǂ����`�F�b�N==================
	AIndex	index = mRevisionData_ImportFileDataID.Find(inImportFileDataObjectID);
	//mRevisionData_ImportFileDataID��import file data��������΁A�n�b�V���ɂ����݂��Ȃ��̂ŁA���������I�� #0 ������
	if( index == kIndex_Invalid )
	{
		return false;
	}
	
	//==================�폜==================
	//���g�p�̂��߃R�����g�A�E�g ABool	deleted = false;
	//#423 AImportFileData*	importFileData = &(GetApp().SPI_GetImportFileManager().GetImportFileDataByID(inImportFileDataObjectID));
	ABool	result = DeleteFromHash(/*#423 importFileData*/inImportFileDataObjectID);
	//RevisionData�폜 #423
	if( index != kIndex_Invalid )
	{
		mRevisionData_ImportFileDataID.Delete1(index);
		mRevisionData_RevisonNumber.Delete1(index);
	}
	return result;
}

#pragma mark ===========================

#pragma mark ---����

//�L�[���[�h�i�ŏ��Ɉ�v�������́j�̃J�e�S���[�擾
ABool	AImportIdentifierLinkList::FindKeyword( const AText& inKeywordText, AIndex& outCategoryIndex ) const
{
	AIndex	index = Find(inKeywordText);
	if( index == kIndex_Invalid )   return false;
	outCategoryIndex = //#423 mHash_ImportFileData.Get(index)->GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index));
			GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(mHash_ImportFileDataID.Get(index)).
			GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index));
	return true;
}

//�L�[���[�h�i�ŏ��Ɉ�v�������́j�̃J�e�S���[��InfoText�擾
ABool	AImportIdentifierLinkList::FindKeyword( const AText& inKeywordText, AFileAcc& outFile, AIndex& outCategoryIndex, AText& outInfoText ) const
{
	AIndex	index = Find(inKeywordText);
	if( index == kIndex_Invalid )   return false;
	outCategoryIndex = //#423 mHash_ImportFileData.Get(index)->GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index));
			GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(mHash_ImportFileDataID.Get(index)).
			GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index));
	//#348 outInfoText.SetText(mHash_ImportFileData.Get(index)->GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index)));
	//#423 mHash_ImportFileData.Get(index)->GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index),outInfoText);//#348
	GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(mHash_ImportFileDataID.Get(index)).
			GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index),outInfoText);//#348
	//#423 outFile.CopyFrom(mHash_ImportFileData.Get(index)->GetFile());
	outFile.CopyFrom(
			GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(mHash_ImportFileDataID.Get(index)).GetFile());
	return true;
}

//R0243
//�L�[���[�h�i�ŏ��Ɉ�v�������́j�̃^�C�v�擾
ABool	AImportIdentifierLinkList::FindKeywordType( const AText& inKeywordText, AText& outTypeText ) const
{
	AIndex	index = Find(inKeywordText);
	if( index == kIndex_Invalid )   return false;
	//#348 outTypeText.SetText(mHash_ImportFileData.Get(index)->GetTextInfoConst().GetTypeTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index)));
	//#423 mHash_ImportFileData.Get(index)->GetTextInfoConst().GetTypeTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index),outTypeText);//#348
	GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(mHash_ImportFileDataID.Get(index)).
			GetTextInfoConst().GetTypeTextByGlobalIdentifierIndex(mHash_IdentifierIndex.Get(index),outTypeText);//#348
	return true;
}

//#853 #717
/**
�e��L�[���[�h�����X���b�h�p�����L�[���[�h��������
@return false�̏ꍇ�͍��ڐ���limit over
@note �n�b�V�����g��Ȃ��̂Œᑬ�Ȃ��߁A�X���b�h�g�p���K�{�����A���낢��Ȍ������@���ł���B
*/
ABool	AImportIdentifierLinkList::SearchKeywordImport( const AText& inWord, const AHashTextArray& inParentWord, 
													   const AKeywordSearchOption inOption,
													   const AIndex inCurrentStateIndex,
													   AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
													   ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
													   ATextArray& outCommentTextArray,
													   ATextArray& outCompletionTextArray, ATextArray& outURLArray,
													   AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
													   AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
													   AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
													   AArray<AItemCount>& outMatchedCountArray,
													   const ABool& inAbort  ) const
{
	//���ʁi�������ڐ������j�t���O
	ABool	result = true;
	//�����I�v�V�����������ꍇ�́A�����������̂��߁A�n�b�V�����猟������B�i�⊮��⌟���̏ꍇ�j
	if( inOption == kKeywordSearchOption_None )
	{
		//��������������
		
		//�L�[���[�h�Ɉ�v����f�[�^�̃C���f�b�N�X���擾
		AArray<AIndex>	indexArray;
		Find(inWord,indexArray);//�n�b�V������
		//���ꂼ��̃f�[�^�ɂ��āA�o�͔z��ւ̃f�[�^�ǉ�
		for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
		{
			//Abort�t���OON�Ȃ�I��
			if( inAbort == true )
			{
				break;
			}
			//Import File Data�ƁAdataIndex�擾
			AIndex	index = indexArray.Get(i);
			AIndex	dataIndex = mHash_IdentifierIndex.Get(index);
			AObjectID	importFileDataID = mHash_ImportFileDataID.Get(index);
			//Import File Data�|�C���^�擾
			AImportFileData*	importFileDataPtr = 
								GetApp().SPI_GetImportFileManager().GetImportFileDataAndRetain(importFileDataID);
			if( importFileDataPtr != NULL )//����ImportFileData���폜�ς݂̏ꍇ�́ANULL���Ԃ�
			{
				//��L�̃|�C���^retain���m���ɉ�����邽�߂̃X�^�b�N�N���X
				AStDecrementRetainCounter	releaser(importFileDataPtr);
				
				AImportFileData&	importFileData = *(importFileDataPtr);
				//#723
				//ImportFileData�̍X�V���s���ԁAwords list finder��ImportFileData(�̒���TextInfo)�ɃA�N�Z�X���Ȃ��悤�ɔr������
				AStImportFileDataLocker	locker(importFileData);
				//
				//�L�[���[�h�擾
				AText	keywordText;
				importFileData.GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex,keywordText);
				//�e�L�[���[�h�擾
				AText	parentKeywordText;
				importFileData.GetTextInfoConst().GetParentKeywordTextByGlobalIdentifierIndex(dataIndex,parentKeywordText);
				//Type�擾
				AText	typeText;
				importFileData.GetTextInfoConst().GetTypeTextByGlobalIdentifierIndex(dataIndex,typeText);
				//InfoText�擾
				AText	infoText;
				importFileData.GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex,infoText);
				//�R�����g�e�L�X�g�擾
				AText	commentText;
				importFileData.GetTextInfoConst().GetCommentTextByGlobalIdentifierIndex(dataIndex,commentText);
				//�J�e�S��index�擾
				AIndex	categoryIndex = importFileData.GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(dataIndex);
				//�J���[�X���b�g
				AIndex	colorSlotIndex = importFileData.GetTextInfoConst().GetColorSlotIndexByGlobalIdentifierIndex(dataIndex);
				//�ʒu�擾
				AUniqID	objectUniqID = importFileData.GetTextInfoConst().GetUniqIDByGlobalIdentifierIndex(dataIndex);
				ATextPoint	spt = {0}, ept = {0};
				importFileData.GetTextInfoConst().GetGlobalIdentifierRange(objectUniqID,spt,ept);
				//�t�@�C���p�X�擾
				AText	filepath;
				importFileData.GetFile().GetPath(filepath);
				/*
				//
				if( compareParentKeyword == true )
				{
				fprintf(stderr,"  list:");
				parentKeywordText.OutputToStderr();
				fprintf(stderr,"::");
				keywordText.OutputToStderr();
				fprintf(stderr," ");
				if( parentKeywordText.Compare(inParentWord) == false )
				{
				continue;
				}
				}
				*/
				//
				outKeywordArray.Add(keywordText);
				outParentKeywordArray.Add(parentKeywordText);
				outTypeTextArray.Add(typeText);
				outInfoTextArray.Add(infoText);
				outCommentTextArray.Add(commentText);
				outCompletionTextArray.Add(GetEmptyText());
				outURLArray.Add(GetEmptyText());
				outCategoryIndexArray.Add(categoryIndex);
				outColorSlotIndexArray.Add(colorSlotIndex);
				outStartIndexArray.Add(importFileData.GetTextInfoConst().GetTextIndexFromTextPoint(spt));
				outEndIndexArray.Add(importFileData.GetTextInfoConst().GetTextIndexFromTextPoint(ept));
				outScopeArray.Add(kScopeType_Import);
				outFilePathArray.Add(filepath);
				outMatchedCountArray.Add(1);
				//�������I�[�o�[�Ȃ�I��
				if( outKeywordArray.GetItemCount() >= kLimit_KeywordSearchResultCount )
				{
					return false;
				}
			}
		}
	}
	else
	{
		//�����I�v�V�����L�莞�����i�������Ԃ�������j
		
		//�n�b�V������AImportFileDataID��array�𐶐�
		AHashArray<AObjectID>	importFileDataIDArray;
		//�n�b�V�����̑S�Ẵf�[�^����AimportFileDataID���擾���āAimportFileDataIDArray�ցi�d�����Ȃ��悤�Ɂj�i�[�B
		{
			AItemCount	hashCount = mHash_ImportFileDataID.GetItemCount();
			for( AIndex i = 0; i < hashCount; i++ )
			{
				AIndex	dataIndex = mHash_IdentifierIndex.Get(i);
				AObjectID	importFileDataID = mHash_ImportFileDataID.Get(i);
				if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
				if( importFileDataID == kObjectID_Invalid )   { _ACError("",NULL); continue; }
				//�d�����Ă��Ȃ���΁AimportFileDataIDArray�֒ǉ�
				if( importFileDataIDArray.Find(importFileDataID) == kIndex_Invalid )
				{
					importFileDataIDArray.Add(importFileDataID);
				}
			}
		}
		//�������s
		//�iAImportIdentifierLinkList�̃n�b�V���͎g�킸�A����TextInfo����GlobalIdentifiierList���猟������B�j
		//�eImportFileData���̃��[�v
		AItemCount	itemCount = importFileDataIDArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AIndex	startIndex = outKeywordArray.GetItemCount();
			//Import File Data�|�C���^�擾
			AImportFileData*	importFileDataPtr = 
								GetApp().SPI_GetImportFileManager().GetImportFileDataAndRetain(importFileDataIDArray.Get(i));
			if( importFileDataPtr != NULL )//����ImportFileData���폜�ς݂̏ꍇ�́ANULL���Ԃ�
			{
				//��L�̃|�C���^retain���m���ɉ�����邽�߂̃X�^�b�N�N���X
				AStDecrementRetainCounter	releaser(importFileDataPtr);
				
				AImportFileData&	importFileData = *(importFileDataPtr);
				//#723
				//ImportFileData�̍X�V���s���ԁAwords list finder��ImportFileData(�̒���TextInfo)�ɃA�N�Z�X���Ȃ��悤�ɔr������
				AStImportFileDataLocker	locker(importFileData);
				//
				AArray<AScopeType>	scopeArray;
				if( importFileData.GetTextInfoConst().SearchKeywordGlobal(inWord,inParentWord,inOption,
																		  inCurrentStateIndex,
																		  outKeywordArray,outParentKeywordArray,
																		  outTypeTextArray,outInfoTextArray,outCommentTextArray,
																		  outCompletionTextArray,outURLArray,
																		  outCategoryIndexArray,outColorSlotIndexArray,
																		  outStartIndexArray,outEndIndexArray,scopeArray,
																		  outMatchedCountArray,inAbort) == false )
				{
					result = false;
				}
				//�t�@�C���p�X��scope��ǉ��B
				AText	path;
				importFileData.GetFile().GetPath(path);
				for( AIndex j = startIndex; j < outKeywordArray.GetItemCount(); j++ )
				{
					outScopeArray.Add(kScopeType_Import);
					outFilePathArray.Add(path);
				}
			}
		}
	}
	
	return result;
}

#if 0
//RC2
//�⊮���͌���S�Ď擾�i�o�͐�z��ɂ̓f�[�^���ǉ�����A�폜�͂���Ȃ��j
void	AImportIdentifierLinkList::GetAbbrevCandidate( const AText& inText, const AFileAcc& inDocFile, 
		ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
		AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray, AObjectArray<AFileAcc>& outFileArray ) const
{
	AText	docfilename;
	inDocFile.GetFilename(docfilename);
	docfilename.DeleteAfter(docfilename.GetSuffixStartPos());
	//
	AIndex	startIndex = outAbbrevCandidateArray.GetItemCount();
	AIndex	otherFileStartIndex = startIndex;
	//�f�[�^����S�Č���
	AItemCount	itemCount = /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AIndex	dataIndex = mHash_IdentifierIndex.Get(i);
		//#423 AImportFileData*	importFileData = mHash_ImportFileData.Get(i);
		AObjectID	importFileDataID = mHash_ImportFileDataID.Get(i);//#423
		if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
		if( /*#423 importFileData == NULL*/importFileDataID == kObjectID_Invalid )   _ACThrow("",NULL);
		
		//#423
		const AImportFileData&	importFileData = 
				GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataID);
		//�����N�f�[�^�̍ŏ��̕����ƁA����inText����v���邩�ǂ������ׂ�
		//#348 const AText&	keywordText = importFileData->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex);
		AText	keywordText;//#348
		importFileData/*#423->*/.GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex,keywordText);//#348
		if( keywordText.GetItemCount() >= inText.GetItemCount() )
		{
			if( keywordText.CompareMin(inText) == true )
			{
				//if( outAbbrevCandidateArray.Find(keywordText) == kIndex_Invalid )�d���`�F�b�N����߂�
				{
					AIndex	index = otherFileStartIndex;
					AIndex	end = outAbbrevCandidateArray.GetItemCount();
					AScopeType	scopeType = kScopeType_Import;
					//�g���q�݂̂��Ⴄ�t�@�C���̏ꍇ�́AstartIndex����otherFileStartIndex�̊Ԃɑ}������i�����ȗ����͂ɕ\���悤�Ɂj
					//����ȊO�̏ꍇ��otherFileStartIndex����Ō�܂ł̊Ԃɑ}������
					AText	filename;
					importFileData/*#423->*/.GetFile().GetFilename(filename);
					filename.DeleteAfter(filename.GetSuffixStartPos());
					if( docfilename.Compare(filename) == true )
					{
						index = startIndex;
						end = otherFileStartIndex;
						scopeType = kScopeType_Import_Near;
						otherFileStartIndex++;
					}
					//�}���ӏ�����
					//3:A
					//4:B
					//5:D
					//6:E
					//C��}����3:C>A, 4:C>B, 5:C<D ��5�̈ʒu�ɑ}��
					for( ; index < end; index++ )
					{
						if( keywordText.IsLessThan(outAbbrevCandidateArray.GetTextConst(index)) == true )
						{
							break;
						}
					}
					//�o�͔z��փf�[�^�ǉ�
					outAbbrevCandidateArray.Insert1(index,keywordText);
					//#348 outInfoTextArray.Insert1(index,importFileData->GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex));
					AText	infoText;//#348
					importFileData/*#423->*/.GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex,infoText);//#348
					outInfoTextArray.Insert1(index,infoText);//#348
					outCategoryIndexArray.Insert1(index,importFileData/*#423->*/.GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(dataIndex));
					outScopeArray.Insert1(index,scopeType);
					outFileArray.InsertNew1Object(index);
					outFileArray.GetObject(index).CopyFrom(importFileData/*#423->*/.GetFile());
				}
			}
		}
	}
}

//R0243
//ParentKeyword����⊮���͌���S�Ď擾�i�o�͐�z��ɂ̓f�[�^���ǉ�����A�폜�͂���Ȃ��j
void	AImportIdentifierLinkList::GetAbbrevCandidateByParentKeyword( const AText& inParentKeyword, 
		ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
		AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray, AObjectArray<AFileAcc>& outFileArray ) const
{
	//
	AIndex	startIndex = outAbbrevCandidateArray.GetItemCount();
	//�f�[�^����S�Č���
	AItemCount	itemCount = /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AIndex	dataIndex = mHash_IdentifierIndex.Get(i);
		//#423 AImportFileData*	importFileData = mHash_ImportFileData.Get(i);
		AObjectID	importFileDataID = mHash_ImportFileDataID.Get(i);//#423
		if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
		if( /*#423 importFileData == NULL*/importFileDataID == kObjectID_Invalid )   _ACThrow("",NULL);
		
		//#423
		const AImportFileData&	importFileData = 
				GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataID);
		//ParentKeyword����v���邩�ǂ������ׂ�
		//#348 if( importFileData->GetTextInfoConst().GetParentKeywordTextByGlobalIdentifierIndex(dataIndex).Compare(inParentKeyword) == true )
		AText	parentKeywordText;//#348
		importFileData/*#423->*/.GetTextInfoConst().GetParentKeywordTextByGlobalIdentifierIndex(dataIndex,parentKeywordText);//#348
		if( parentKeywordText.Compare(inParentKeyword) == true )//#348
		{
			//
			//#348 const AText&	keywordText = importFileData->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex);
			AText	keywordText;//#348
			importFileData/*#423->*/.GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex,keywordText);//#348
			AIndex	index = startIndex;
			AIndex	end = outAbbrevCandidateArray.GetItemCount();
			//�}���ӏ�����
			//3:A
			//4:B
			//5:D
			//6:E
			//C��}����3:C>A, 4:C>B, 5:C<D ��5�̈ʒu�ɑ}��
			for( ; index < end; index++ )
			{
				if( keywordText.IsLessThan(outAbbrevCandidateArray.GetTextConst(index)) == true )
				{
					break;
				}
			}
			//�o�͔z��փf�[�^�ǉ�
			outAbbrevCandidateArray.Insert1(index,keywordText);
			//#348 outInfoTextArray.Insert1(index,importFileData->GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex));
			AText	infoText;//#348
			importFileData/*#423->*/.GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex,infoText);//#348
			outInfoTextArray.Insert1(index,infoText);//#348
			outCategoryIndexArray.Insert1(index,importFileData/*#423->*/.GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(dataIndex));
			outScopeArray.Insert1(index,kScopeType_Import);
			outFileArray.InsertNew1Object(index);
			outFileArray.GetObject(index).CopyFrom(importFileData/*#423->*/.GetFile());
		}
	}
}
#endif

//�L�[���[�h���环�ʎq�̃��X�g���擾
void	AImportIdentifierLinkList::GetIdentifierListByKeyword( const AText& inText, 
		AArray<AIndex>& outCategoryIndexArray, ATextArray& outInfoText, AObjectArray<AFileAcc>& outFileArray,
		AArray<ATextIndex>& outStartArray, AArray<ATextIndex>& outEndArray,
		ATextArray& outCommentTextArray, ATextArray& outParentKeywordTextArray ) const//RC2
{
	//�L�[���[�h�Ɉ�v����f�[�^�̃C���f�b�N�X���擾
	AArray<AIndex>	indexArray;
	Find(inText,indexArray);
	//���ꂼ��̃f�[�^�ɂ��āA�o�͔z��ւ̃f�[�^�ǉ�
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		AIndex	index = indexArray.Get(i);
		AIndex	dataIndex = mHash_IdentifierIndex.Get(index);
		//#423 AImportFileData*	importFileData = mHash_ImportFileData.Get(index);
		AObjectID	importFileDataID = mHash_ImportFileDataID.Get(index);//#423
		const AImportFileData&	importFileData = 
				GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(importFileDataID);//#423
		
		//
		outCategoryIndexArray.Add(importFileData/*#423->*/.GetTextInfoConst().GetCategoryIndexByGlobalIdentifierIndex(dataIndex));
		//
		//#348 outInfoText.Add(importFileData->GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex));
		AText	infoText;//#348
		importFileData/*#423->*/.GetTextInfoConst().GetInfoTextByGlobalIdentifierIndex(dataIndex,infoText);//#348
		outInfoText.Add(infoText);//#348
		//
		AFileAcc	file;
		outFileArray.GetObject(outFileArray.AddNewObject()).CopyFrom(importFileData/*#423->*/.GetFile());
		//
		AUniqID	objectID = importFileData/*#423->*/.GetTextInfoConst().GetUniqIDByGlobalIdentifierIndex(dataIndex);
		ATextPoint	spt, ept;
		importFileData/*#423->*/.GetTextInfoConst().GetGlobalIdentifierRange(objectID,spt,ept);
		outStartArray.Add(importFileData/*#423->*/.GetTextInfoConst().GetTextIndexFromTextPoint(spt));
		outEndArray.Add(importFileData/*#423->*/.GetTextInfoConst().GetTextIndexFromTextPoint(ept));
		//
		//#348 outCommentTextArray.Add(importFileData->GetTextInfoConst().GetCommentTextByGlobalIdentifierIndex(dataIndex));
		AText	commentText;//#348
		importFileData/*#423->*/.GetTextInfoConst().GetCommentTextByGlobalIdentifierIndex(dataIndex,commentText);//#348
		outCommentTextArray.Add(commentText);//#348
		//
		//#348 outParentKeywordTextArray.Add(importFileData->GetTextInfoConst().GetParentKeywordTextByGlobalIdentifierIndex(dataIndex));
		AText	parentKeywordText;//#348
		importFileData/*#423->*/.GetTextInfoConst().GetParentKeywordTextByGlobalIdentifierIndex(dataIndex,parentKeywordText);//#348
		outParentKeywordTextArray.Add(parentKeywordText);//#348
	}
}

#pragma mark ===========================

#pragma mark ---�n�b�V������

//�n�b�V��������
void	AImportIdentifierLinkList::InitHash( const AItemCount inItemCount )
{
	//�T�C�Y���傫��������return
	if( inItemCount >= kItemCount_HashMaxSize/2 )
	{
		_ACError("cannot make hash because array is too big",this);
		return;
	}
	AItemCount	hashsize = inItemCount*kImportIdentifierLinkListHashMultiple;//*3;
	//�n�b�V���e�[�u���S�폜
	//#423 mHash_ImportFileData.DeleteAll();
	mHash_ImportFileDataID.DeleteAll();//#423
	mHash_IdentifierIndex.DeleteAll();
	//�n�b�V���T�C�Y���ő�n�b�V���T�C�Y�����傫����΁A�␳����B
	if( hashsize > kItemCount_HashMaxSize )   hashsize = kItemCount_HashMaxSize;
	//�n�b�V���e�[�u���m��
	//#423 mHash_ImportFileData.Reserve(0,hashsize);
	mHash_ImportFileDataID.Reserve(0,hashsize);//#423
	mHash_IdentifierIndex.Reserve(0,hashsize);
	//�n�b�V���e�[�u��������
	//�������̂��߁A�|�C���^�g�p
	//#423 AStArrayPtr<AImportFileData*>	arrayptr1(mHash_ImportFileData,0,mHash_ImportFileData.GetItemCount());
	AStArrayPtr<AObjectID>			arrayptr1(mHash_ImportFileDataID,0,mHash_ImportFileDataID.GetItemCount());//#423
	AStArrayPtr<AIndex>				arrayptr2(mHash_IdentifierIndex,0,mHash_IdentifierIndex.GetItemCount());
	//#423 AImportFileData**	p1 = arrayptr1.GetPtr();
	AObjectID*			p1 = arrayptr1.GetPtr();//#423
	AIndex*				p2 = arrayptr2.GetPtr();
	for( AIndex i = 0; i < hashsize; i++ )
	{
		//#423 p1[i] = NULL;
		p1[i] = kObjectID_Invalid;//#423
		p2[i] = kIndex_HashNoData;
	}
}

//�n�b�V���֓o�^
void	AImportIdentifierLinkList::RegisterHash( /*#423 AImportFileData* inImportFileData*/
			const AObjectID inImportFileDataID , const AIndex inIndex )
{
	if( /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount() == 0 )   _ACThrow("",NULL);
	
	//
	//#348 const AText& keywordText = inImportFileData->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(inIndex);
	AText	keywordText;//#348
	//#423 inImportFileData->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(inIndex,keywordText);//#348
	GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(inImportFileDataID).
	GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(inIndex,keywordText);
	
	//�n�b�V���l�v�Z
	unsigned long	hash = GetHashValue(keywordText);
	AIndex	hashstartpos = hash%(/*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount());
	AIndex	hashpos = hashstartpos;//#598
	{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
		//�������̂��߁A�|�C���^�g�p
		AStArrayPtr</*#216 AObjectID*/AIndex>	arrayptr(mHash_IdentifierIndex,0,mHash_IdentifierIndex.GetItemCount());
		AIndex*	p = arrayptr.GetPtr();
		//�o�^�ӏ�����
		//p1[]: ImportFileData�ւ̃|�C���^  p2[]: ImportFileData����Index�A����сAHash����f�[�^
		//#598 ��ֈړ�AIndex	hashpos = hashstartpos;
		while( true )
		{
			AIndex	dataIndex = p[hashpos];
			if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   break;
			hashpos = GetNextHashPos(hashpos);
			if( hashpos == hashstartpos )
			{
				_ACError("no area in hash",this);
				return;
			}
		}
	}
	//�o�^
	mHash_IdentifierIndex.Set(hashpos,inIndex);
	//#423 mHash_ImportFileData.Set(hashpos,inImportFileData);
	mHash_ImportFileDataID.Set(hashpos,inImportFileDataID);//#423
}

//�n�b�V������w��ImportFileData�폜
ABool	AImportIdentifierLinkList::DeleteFromHash( /*#423 AImportFileData* inImportFileData*/
			const AObjectID inImportFileDataID )
{
	ABool	deleted = false;
	//�������̂��߁A�|�C���^�g�p
	//#423 AStArrayPtr<AImportFileData*>	arrayptr1(mHash_ImportFileData,0,mHash_ImportFileData.GetItemCount());
	AStArrayPtr<AObjectID>			arrayptr1(mHash_ImportFileDataID,0,mHash_ImportFileDataID.GetItemCount());//#423
	AStArrayPtr<AIndex>				arrayptr2(mHash_IdentifierIndex,0,mHash_IdentifierIndex.GetItemCount());
	//#423 AImportFileData**	p1 = arrayptr1.GetPtr();
	AObjectID*			p1 = arrayptr1.GetPtr();//#423
	AIndex*				p2 = arrayptr2.GetPtr();
	AItemCount	hashsize = /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount();
	for( AIndex hashpos = 0; hashpos < hashsize; hashpos++ )
	{
		if( p1[hashpos] == /*#423 inImportFileData*/inImportFileDataID )
		{
			//�폜���s
			if( p2[GetNextHashPos(hashpos)] == kIndex_HashNoData )
			{
				//���̃n�b�V���ʒu��NoData�Ȃ�ANoData������
				p1[hashpos] = kObjectID_Invalid;//#423 NULL;
				p2[hashpos] = kIndex_HashNoData;
				//NoData���������񂾂Ƃ��A�O�̃n�b�V���ʒu�i�����I�ɂ͎��j��Deleted�Ȃ�NoData�ɏ�������
				//[hashpos-1]:NoData
				//[hashpos  ]:NoData�����񏑂�����
				//[hashpos+1]:Deleted������Deleted��NoData�ɂ���ihashpos�̈ʒu�̃f�[�^�ւ̃����N���Ȃ��邽�߂ɑ��݂��Ă���Deleted�Ȃ̂ŁB�j
				//[hashpos+2]:Deleted������Ɏ���Deleted�Ȃ炱����NoData
				AIndex	hp = hashpos+1;
				if( hp >= /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount() )   hp = 0;
				while( p2[hp] == kIndex_HashDeleted )
				{
					//Deleted�Ȃ�NoData�ɏ��������Čp��
					p1[hp] = kObjectID_Invalid;//#423 NULL;
					p2[hp] = kIndex_HashNoData;
					hp++;
					if( hp >= /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount() )   hp = 0;
				}
			}
			else
			{
				//���̃n�b�V���ʒu��NoData�ȊO�ADeleted������
				p1[hashpos] = kObjectID_Invalid;//#423 NULL;
				p2[hashpos] = kIndex_HashDeleted;
			}
			deleted = true;
		}
	}
	return deleted;
}

//R-PFM �n�b�V���p�t�H�[�}���X����p
AItemCount	gImportIdentifierLinkListFind_FindCount = 0;
AItemCount	gImportIdentifierLinkListFind_CompareCount = 0;
AItemCount	gImportIdentifierLinkListFind_HitCount = 0;
AItemCount	gImportIdentifierLinkListFind_MaxContinuous = 0;

//�ŏ��ɃL�[���[�h����v�������ڂ̃C���f�b�N�X���擾
AIndex	AImportIdentifierLinkList::Find( const AText& inKeywordText ) const
{
	if( /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount() == 0 )   return kIndex_Invalid;
	
	//#0 ������
	//Import File Data�I�u�W�F�N�garray�擾
	AStMutexLocker	mutexlocker(GetApp().SPI_GetImportFileManager().GetImportFileDataArrayMutex());
	const AObjectArrayIDIndexed< AImportFileData >&	importFileDataArray = GetApp().SPI_GetImportFileManager().GetImportFileDataArrayConst();
	
	//R-PFM
	gImportIdentifierLinkListFind_FindCount++;
	AItemCount	continuous = 0;
	
	//�n�b�V���l�v�Z
	unsigned long	hash = GetHashValue(inKeywordText);
	AIndex	hashstartpos = hash%(/*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount());
	
	//�������̂��߁A�|�C���^�g�p
	//#423 AStArrayPtr<AImportFileData*>	arrayptr1(mHash_ImportFileData,0,mHash_ImportFileData.GetItemCount());
	AStArrayPtr<AObjectID>			arrayptr1(mHash_ImportFileDataID,0,mHash_ImportFileDataID.GetItemCount());//#423
	AStArrayPtr<AIndex>				arrayptr2(mHash_IdentifierIndex,0,mHash_IdentifierIndex.GetItemCount());
	//#423 AImportFileData**	p1 = arrayptr1.GetPtr();
	AObjectID*			p1 = arrayptr1.GetPtr();
	AIndex*				p2 = arrayptr2.GetPtr();
	//�o�^�ӏ�����
	//p1[]: ImportFileData�ւ̃|�C���^  p2[]: ImportFileData����Index�A����сAHash����f�[�^
	AIndex	hashpos = hashstartpos;
	while( true )
	{
		AIndex	dataIndex = p2[hashpos];
		if( dataIndex == kIndex_HashNoData )   break;
		if( dataIndex != kIndex_HashDeleted )
		{
			//R-PFM
			gImportIdentifierLinkListFind_CompareCount++;
			continuous++;
			if( continuous > gImportIdentifierLinkListFind_MaxContinuous )
			{
				gImportIdentifierLinkListFind_MaxContinuous = continuous;
			}
			//����NULL�i�s��j�Ȃ�throw
			if( p1[hashpos] == /*#423 NULL*/kObjectID_Invalid )   _ACThrow("",NULL);
			//�L�[���[�h�ƈ�v����Ȃ�n�b�V���f�[�^�̃C���f�b�N�X��Ԃ�
			//#348 if( p1[hashpos]->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex).Compare(inKeywordText) == true )
			//#423 if( p1[hashpos]->GetTextInfoConst().CompareKeywordTextByGlobalIdentifierIndex(dataIndex,inKeywordText) == true )//#348
			//#717if( GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(p1[hashpos]).GetTextInfoConst().
			if( importFileDataArray.GetObjectConstByID(p1[hashpos]).GetTextInfoConst().//#0 ������
						CompareKeywordTextByGlobalIdentifierIndex(dataIndex,inKeywordText) == true )
			{
				//R-PFM
				gImportIdentifierLinkListFind_HitCount++;
				//
				return hashpos;
			}
		}
		hashpos = GetNextHashPos(hashpos);
		if( hashpos == hashstartpos )   break;
	}
	return kIndex_Invalid;
}

//��v���鍀�ڂ̑S�ẴC���f�b�N�X���擾
void	AImportIdentifierLinkList::Find( const AText& inKeywordText, AArray<AIndex>& outIndexArray ) const
{
	outIndexArray.DeleteAll();
	
	if( /*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount() == 0 )   return;
	
	//�n�b�V���l�v�Z
	unsigned long	hash = GetHashValue(inKeywordText);
	AIndex	hashstartpos = hash%(/*#423 mHash_ImportFileData*/mHash_ImportFileDataID.GetItemCount());
	
	//�������̂��߁A�|�C���^�g�p
	//#423 AStArrayPtr<AImportFileData*>	arrayptr1(mHash_ImportFileData,0,mHash_ImportFileData.GetItemCount());
	AStArrayPtr<AObjectID>			arrayptr1(mHash_ImportFileDataID,0,mHash_ImportFileDataID.GetItemCount());//#423
	AStArrayPtr<AIndex>				arrayptr2(mHash_IdentifierIndex,0,mHash_IdentifierIndex.GetItemCount());
	//#423 AImportFileData**	p1 = arrayptr1.GetPtr();
	AObjectID*			p1 = arrayptr1.GetPtr();
	AIndex*				p2 = arrayptr2.GetPtr();
	//�o�^�ӏ�����
	//p1[]: ImportFileData�ւ̃|�C���^  p2[]: ImportFileData����Index�A����сAHash����f�[�^
	AIndex	hashpos = hashstartpos;
	while( true )
	{
		AIndex	dataIndex = p2[hashpos];
		if( dataIndex == kIndex_HashNoData )   break;
		if( dataIndex != kIndex_HashDeleted )
		{
			//����NULL�i�s��j�Ȃ�throw
			if( p1[hashpos] == /*#423 NULL*/kObjectID_Invalid )   _ACThrow("",NULL);
			//�L�[���[�h�ƈ�v����Ȃ�n�b�V���f�[�^�̃C���f�b�N�X��ǉ�����
			//#348 if( p1[hashpos]->GetTextInfoConst().GetKeywordTextByGlobalIdentifierIndex(dataIndex).Compare(inKeywordText) == true )
			//#423 if( p1[hashpos]->GetTextInfoConst().CompareKeywordTextByGlobalIdentifierIndex(dataIndex,inKeywordText) == true )
			if( GetApp().SPI_GetImportFileManager().GetImportFileDataConstByID(p1[hashpos]).GetTextInfoConst().
						CompareKeywordTextByGlobalIdentifierIndex(dataIndex,inKeywordText) == true )
			{
				outIndexArray.Add(hashpos);
			}
		}
		hashpos = GetNextHashPos(hashpos);
		if( hashpos == hashstartpos )   break;
	}
}


