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

AThread_CandidateFinder
#717

*/

#include "stdafx.h"

#include "AThread_CandidateFinder.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AThread_CandidateFinder
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AThread_CandidateFinder::AThread_CandidateFinder( AObjectArrayItem* inParent ) : AThread(inParent)
,mRequestData_TextDocumentID(kObjectID_Invalid),mRequestData_TextViewID(kObjectID_Invalid)
,mTextDocumentID(kObjectID_Invalid),mTextViewID(kObjectID_Invalid)
,mRequestData_CurrentStateIndex(kIndex_Invalid),mCurrentStateIndex(kIndex_Invalid)
,mAbortCurrentRequest(false)
{
}

/**
�f�X�g���N�^
*/
AThread_CandidateFinder::~AThread_CandidateFinder()
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/**
�X���b�h���C�����[�`��
�X���b�h�̓A�v�����쒆�A��ɓ��삵�Ă���B
�v���f�[�^��ݒ肵�āAunpause����ƁA���������s����B
*/
void	AThread_CandidateFinder::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_CandidateFinder::NVIDO_ThreadMain started.",this);
	try
	{
		// =================== ���ʊi�[�p���[�J���ϐ� =================== 
		//���ʎ擾�p���[�J���ϐ�
		//�imResult_KeywordArray���ɂ͒��ڊi�[���Ȃ��B
		//���C���X���b�h����́i�O��́j���ʎ擾�Ƃ̔r�����䂪�K�v�ł��邽�߁B
		//�r�����䎞�Ԃ�Z�����邽�߂ɁASPI_GetImportIdentifierAbbrevCandidate()���s���͔r�������A
		//�Ō��mResult_KeywordArray���փR�s�[����Ƃ������A�r�����䂷��悤�ɂ��Ă���B�j
		AHashTextArray	keywordArray;
		ATextArray	parentKeywordArray;
		ATextArray	typeTextArray;
		ATextArray	infoTextArray;
		ATextArray	commentTextArray;
		ATextArray	completionTextArray, urlArray;
		AArray<AIndex>	categoryIndexArray;
		AArray<AIndex>	startIndexArray;
		AArray<AIndex>	endIndexArray;
		AArray<AScopeType>	scopeArray;
		ATextArray	filePathArray;
		AArray<AItemCount>	matchedCountArray;
		AArray<ACandidatePriority>	priorityArray;
		AArray<AIndex>	colorSlotIndexArray;
		//�\�[�g�ς݂̌��ʊi�[�p���[�J���ϐ�
		ATextArray	orderedKeywordArray;
		ATextArray	orderedInfoTextArray;
		ATextArray	orderedComletionTextArray;
		AArray<AIndex>	orderedCategoryIndexArray;
		AArray<AScopeType>	orderedScopeArray;
		ATextArray	orderedFilePathArray;
		AArray<AItemCount>	orderedMatchedCountArray;
		AArray<ACandidatePriority>	orderedPriorityArray;
		ATextArray	orderedParentKeywordArray;
		
		//�A�v�����쒆�͂����ƃ��[�v
		while( NVM_IsAborted() == false )//NVI_AbortThread()���R�[������Ă�����X���b�h�I��
		{
			//pause�����܂ő҂�
			NVM_WaitForUnpaused();
			//Abort����Ă�����I���i�A�v���I�����A���C���X���b�h����NVI_AbortThread()+Unpause()�ɂ��I�������B�j
			if( NVM_IsAborted() == true )
			{
				break;
			}
			
			// =================== �v���f�[�^�ǂݎ�� =================== 
			{
				//�v���f�[�^mutex lock
				AStMutexLocker	locker(mRequestDataMutex);
				//�v���f�[�^����Ȃ��U�I��(pause)
				//�i�v���f�[�^���󁁑O�񃋁[�v�ł̌������s��A�V���ȗv���f�[�^�������Ă��Ă��Ȃ��B�j
				if( mRequestData_TextDocumentID == kObjectID_Invalid )
				{
					// =================== �i�v���f�[�^�������ꍇ�j������������ =================== 
					//���ʂ��i�[
					{
						//����mutex lock
						AStMutexLocker	locker(mResultMutex);
						mResult_WordForResult.SetText(mWord);
						mResult_ParentWordForResult.SetFromTextArray(mParentWord);
						mResult_KeywordArray.SetFromTextArray(orderedKeywordArray);
						mResult_InfoTextArray.SetFromTextArray(orderedInfoTextArray);
						mResult_CompletionTextArray.SetFromTextArray(orderedComletionTextArray);
						mResult_CategoryIndexArray.SetFromObject(orderedCategoryIndexArray);
						mResult_ScopeArray.SetFromObject(orderedScopeArray);
						mResult_FilePathArray.SetFromTextArray(orderedFilePathArray);
						mResult_PriorityArray.SetFromObject(orderedPriorityArray);
						mResult_ParentKeywordArray.SetFromTextArray(orderedParentKeywordArray);
						mResult_MatchedCountArray.SetFromObject(orderedMatchedCountArray);
					}
					//�X���b�h��pause��Ԃɂ���
					NVI_Pause();
					//���C���X���b�h�֊����ʒm
					AObjectIDArray	objectIDArray;
					objectIDArray.Add(mTextViewID);
					ABase::PostToMainInternalEventQueue(kInternalEvent_CandidateFinderPaused,GetObjectID(),0,GetEmptyText(),objectIDArray);
					//unpause�҂���ԂցB
					continue;
				}
				//�v���f�[�^��ǂݎ���āA��ɂ���
				//�h�L�������gID
				mTextDocumentID = mRequestData_TextDocumentID;
				mRequestData_TextDocumentID = kObjectID_Invalid;
				//TextViewID
				mTextViewID = mRequestData_TextViewID;
				mRequestData_TextViewID = kObjectID_Invalid;
				//����word
				mWord.SetText(mRequestData_Word);
				mRequestData_Word.DeleteAll();
				//����parent word
				mParentWord.SetFromTextArray(mRequestData_ParentWord);
				mRequestData_ParentWord.DeleteAll();
				//����state
				mCurrentStateIndex = mRequestData_CurrentStateIndex;
				mRequestData_CurrentStateIndex = kIndex_Invalid;
				//Abort�t���O��OFF�ɂ���i���s���ɗv���f�[�^�ݒ肳�ꂽ��t���OON�ɂȂ�j
				mAbortCurrentRequest = false;
			}
			
			// =================== ���ʊi�[�惍�[�J���ϐ������� =================== 
			keywordArray.DeleteAll();
			parentKeywordArray.DeleteAll();
			typeTextArray.DeleteAll();
			infoTextArray.DeleteAll();
			commentTextArray.DeleteAll();
			completionTextArray.DeleteAll();
			urlArray.DeleteAll();
			categoryIndexArray.DeleteAll();
			startIndexArray.DeleteAll();
			endIndexArray.DeleteAll();
			scopeArray.DeleteAll();
			filePathArray.DeleteAll();
			matchedCountArray.DeleteAll();
			priorityArray.DeleteAll();
			colorSlotIndexArray.DeleteAll();
			
			// =================== ��⌟�����s =================== 
			
			//�v���f�[�^��word, parentword��������̂Ƃ��i���S�f�[�^���ΏۂɂȂ�j�͏������Ȃ�
			if( mWord.GetItemCount() == 0 && mParentWord.GetItemCount() == 0 )
			{
				continue;
			}
			
			//Text Document�I�u�W�F�N�g���擾�B�܂��A���C���X���b�h��Delete1Object()����Ă��I�u�W�F�N�g�������Ȃ��悤��retain����B
			ADocument*	doc = GetApp().NVI_GetDocumentAndRetain(mTextDocumentID);
			if( doc != NULL )
			{
				//AStDecrementRetainCounter�͂��̃u���b�N���甲����Ƃ��ɕK��DecrementRetainCount()���R�[�����邱�Ƃ�ۏ؂��邽�߂�stack�I�u�W�F�N�g
				//�i�f�X�g���N�^��DecrementRetainCount()���Ă΂��BNVI_GetDocumentAndRetain()�ŃR�[�����ꂽIncrementRetainCount()�ɑΉ�
				AStDecrementRetainCounter	releaser(doc);
				
				ADocument_Text&	textdoc = reinterpret_cast<ADocument_Text&>(*doc);
				AModeIndex	modeIndex = textdoc.SPI_GetModeIndex();
				
				//�h�L�������g�̃t�@�C���p�X���擾
				AText	docFilePath;
				textdoc.NVI_GetFilePath(docFilePath);
				//�g���q���폜����
				AText	docFilePathWithoutSuffix;
				docFilePathWithoutSuffix.SetText(docFilePath);
				docFilePathWithoutSuffix.DeleteAfter(docFilePathWithoutSuffix.GetSuffixStartPos());
				
				//�����I�v�V����
				AKeywordSearchOption	option = kKeywordSearchOption_Leading | kKeywordSearchOption_IgnoreCases |
										kKeywordSearchOption_InhibitDuplicatedKeyword |
										kKeywordSearchOption_OnlyEnabledCategoryForCurrentState;
				if( GetApp().SPI_IsCandidatePartialSearchMode() == true )
				{
					option |= kKeywordSearchOption_Partial;
				}
				//�����Ώ�word����ŁAparent word�����݂���ꍇ�́Aparent word�Ɉ�v������݂̂̂�Ώۂɂ���
				//�i�����Ώ�word����ł͂Ȃ��ꍇ�́Aparent word�Ɉ�v������̂�"�D��"�ɂ���̂݁j
				if( mWord.GetItemCount() == 0 && mParentWord.GetItemCount() > 0 )
				{
					option |= kKeywordSearchOption_CompareParentKeyword;
				}
				
				// =================== ���[�J�����ʎq�����⌟�� =================== 
				textdoc.SPI_SearchKeywordLocal(mWord,mParentWord,option,mCurrentStateIndex,
											   keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
											   completionTextArray,urlArray,
											   categoryIndexArray,colorSlotIndexArray,
											   startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
											   mTextViewID,
											   mAbortCurrentRequest);
				
				// =================== �O���[�o�����ʎq�����⌟�� =================== 
				textdoc.SPI_SearchKeywordGlobal(mWord,mParentWord,option,mCurrentStateIndex,
												keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
												completionTextArray,urlArray,
												categoryIndexArray,colorSlotIndexArray,
												startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
												mAbortCurrentRequest);
				
				// =================== Import Identifier�����⌟�� =================== 
				textdoc.SPI_SearchKeywordImport(mWord,mParentWord,option,mCurrentStateIndex,
												keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
												completionTextArray,urlArray,
												categoryIndexArray,colorSlotIndexArray,
												startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
												mAbortCurrentRequest);
				
				// =================== System Header �����⌟�� =================== 
				GetApp().SPI_GetModePrefDB(modeIndex).
						SPI_SearchKeywordSystemHeader(mWord,mParentWord,option,mCurrentStateIndex,
													  keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
													  completionTextArray,urlArray,
													  categoryIndexArray,colorSlotIndexArray,
													  startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
													  mAbortCurrentRequest);
				
				// =================== ���[�h�̃L�[���[�h �����⌟�� =================== 
				GetApp().SPI_GetModePrefDB(modeIndex).
						SPI_SearchKeywordKeywordList(mWord,mParentWord,option,mCurrentStateIndex,
													 keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
													 completionTextArray,urlArray,
													 categoryIndexArray,colorSlotIndexArray,
													 startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
													 mAbortCurrentRequest);
				
				//==================�D�揇�ʐݒ�==================
				//�����ݒ�
				AItemCount	itemCount = keywordArray.GetItemCount();
				for( AIndex index = 0; index < itemCount; index++ )
				{
					priorityArray.Add(kCandidatePriority_None);
				}
				//�D��x���Ⴂ������ݒ肷��
				//------------------�P�D�����ɂ��D��------------------
				AHashTextArray	recentCompletionWordArray;
				GetApp().SPI_GetRecentCompletionWordArray(recentCompletionWordArray);
				//�S���ڃ��[�v
				for( AIndex index = 0; index < itemCount; index++ )
				{
					//�L�[���[�h�擾
					AText	keyword;
					keywordArray.Get(index,keyword);
					//�����ƈ�v������A������v�D���ݒ�
					if( recentCompletionWordArray.Find(keyword) != kIndex_Invalid )
					{
						//������v�D���ݒ�
						priorityArray.Set(index,kCandidatePriority_History);
					}
				}
				//------------------�Q�Dparent keyword��v�ɂ��D��------------------
				//�S���ڃ��[�v
				for( AIndex index = 0; index < itemCount; index++ )
				{
					//�L�[���[�h�擾
					AText	keyword;
					keywordArray.Get(index,keyword);
					//parent keyword��v�ɂ��D��
					if( mParentWord.GetItemCount() > 0 )
					{
						//parent keyword�擾
						AText	parentKeyword;
						parentKeywordArray.Get(index,parentKeyword);
						//fprintf(stderr,"PARENT:");
						//parentKeyword.OutputToStderr();
						//�v���f�[�^�ɓ���parent keyword�����݂���ΗD�悷��
						if( mParentWord.Find(parentKeyword) != kIndex_Invalid )
						{
							//�����ƈ�v���邩�ǂ����𔻒�
							if( recentCompletionWordArray.Find(keyword) != kIndex_Invalid )
							{
								//parent keyword��v��������v�D���ݒ�
								priorityArray.Set(index,kCandidatePriority_ParentKeywordWithHistory);
							}
							else
							{
								//parent keyword��v�D���ݒ�
								priorityArray.Set(index,kCandidatePriority_ParentKeyword);
							}
						}
					}
				}
				//------------------�R�Dlocal��D��------------------
				//�S���ڃ��[�v
				for( AIndex index = 0; index < itemCount; index++ )
				{
					//�X�R�[�v�����[�J���Ȃ烍�[�J���D���ݒ�
					if( scopeArray.Get(index) == kScopeType_Local )
					{
						priorityArray.Set(index,kCandidatePriority_Local);
					}
				}
				
				//------------------�D�揇�ʏ���ordered�`�ϐ��Ɋi�[------------------
				//�D�揇�ʂ����Ă���ordered�z��Ɋi�[���Ă����i�P�̍��ڂɕ����̗D�揇�ʂ����蓾��̂ŁA��̏����ň�ԗD�悷��D�揇�ʂ��Ō�ɐݒ肵�Ă���j
				orderedKeywordArray.DeleteAll();
				orderedInfoTextArray.DeleteAll();
				orderedComletionTextArray.DeleteAll();
				orderedCategoryIndexArray.DeleteAll();
				orderedScopeArray.DeleteAll();
				orderedFilePathArray.DeleteAll();
				orderedMatchedCountArray.DeleteAll();
				orderedPriorityArray.DeleteAll();
				orderedParentKeywordArray.DeleteAll();
				//�D��x���������烋�[�v
				for( AIndex priority = kCandidatePriority_Highest; priority >= kCandidatePriority_None; priority-- )
				{
					//�S���ڃ��[�v
					for( AIndex index = 0; index < itemCount; index++ )
					{
						//�\���������i200���ځj
						//��������������v���ڐ������Ă��邪�A�������1024���ځB�D�揇�ʂ��������ڂ���Ɍ�������邱�Ƃ����肤��̂ŁA
						//�������ڐ����͑��߂ɂ��Ă���B
						if( orderedKeywordArray.GetItemCount() >= kLimit_CandidateListItemCount )
						{
							//�������ȏ�͒ǉ����Ȃ�
							break;
						}
						//�D�揇�ʈ�v������̂�ǉ�
						if( priorityArray.Get(index) == priority )
						{
							orderedKeywordArray.Add(keywordArray.GetTextConst(index));
							orderedInfoTextArray.Add(infoTextArray.GetTextConst(index));
							orderedComletionTextArray.Add(completionTextArray.GetTextConst(index));
							orderedCategoryIndexArray.Add(categoryIndexArray.Get(index));
							orderedScopeArray.Add(scopeArray.Get(index));
							orderedFilePathArray.Add(filePathArray.GetTextConst(index));
							orderedMatchedCountArray.Add(matchedCountArray.Get(index));
							orderedPriorityArray.Add(static_cast<ACandidatePriority>(priority));
							orderedParentKeywordArray.Add(parentKeywordArray.GetTextConst(index));
						}
					}
				}
			}
		}
	}
	catch(...)
	{
		_ACError("AThread_CandidateFinder::NVIDO_ThreadMain() caught exception.",NULL);
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_CandidateFinder::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---�v���f�[�^�ݒ�

/**
�v���f�[�^�ݒ�
*/
void	AThread_CandidateFinder::SetRequestData( const ADocumentID inTextDocumentID, const AViewID inTextViewID,
												const AText& inWord, const ATextArray& inParentWord,
												const AIndex inCurrentStateIndex )
{
	//�v���f�[�^mutex lock
	AStMutexLocker	locker(mRequestDataMutex);
	//�h�L�������gID�ATextViewID�ݒ�
	mRequestData_TextDocumentID = inTextDocumentID;
	mRequestData_TextViewID = inTextViewID;
	//����word, parent word�ݒ�
	mRequestData_Word.SetText(inWord);
	mRequestData_ParentWord.SetFromTextArray(inParentWord);
	//�v��state
	mRequestData_CurrentStateIndex = inCurrentStateIndex;
	//Abort�t���O��ON�ɂ���B�isearch�������f�����j
	mAbortCurrentRequest = true;
}

#pragma mark ===========================

#pragma mark ---���ʃf�[�^�擾

/**
���̌��ʃf�[�^�ɑ΂���v���f�[�^�iword, parent word�j�̎擾
*/
void	AThread_CandidateFinder::GetWordForResult( AText& outWordForResult, ATextArray& outParentWordForResult ) const
{
	//���ʃf�[�^ mutex lock
	AStMutexLocker	locker(mResultMutex);
	//�f�[�^�擾
	outWordForResult.SetText(mResult_WordForResult);
	outParentWordForResult.SetFromTextArray(mResult_ParentWordForResult);
}

/**
���ʃf�[�^�擾
*/
void	AThread_CandidateFinder::GetResult( ATextArray& outKeywordArray, ATextArray& outInfoTextArray,
										   AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray,
										   ATextArray& outFilePathArray, AArray<ACandidatePriority>& outPriorityArray,
										   ATextArray& outParentKeywordArray, ATextArray& outCompletionTextArray,
										   AArray<AItemCount>& outMatchedItemCountArray ) const
{
	//���ʃf�[�^ mutex lock
	AStMutexLocker	locker(mResultMutex);
	//
	//�f�[�^�擾
	outKeywordArray.AddFromTextArray(mResult_KeywordArray);
	outInfoTextArray.AddFromTextArray(mResult_InfoTextArray);
	outCompletionTextArray.AddFromTextArray(mResult_CompletionTextArray);
	outCategoryIndexArray.AddFromObject(mResult_CategoryIndexArray);
	outScopeArray.AddFromObject(mResult_ScopeArray);
	outFilePathArray.AddFromTextArray(mResult_FilePathArray);
	outPriorityArray.AddFromObject(mResult_PriorityArray);
	outMatchedItemCountArray.AddFromObject(mResult_MatchedCountArray);
	outParentKeywordArray.AddFromTextArray(mResult_ParentKeywordArray);
}
