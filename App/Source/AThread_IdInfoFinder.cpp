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

AThread_IdInfoFinder
#853

*/

#include "stdafx.h"

#include "AThread_IdInfoFinder.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AThread_IdInfoFinder
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AThread_IdInfoFinder::AThread_IdInfoFinder( AObjectArrayItem* inParent ) : AThread(inParent)
,mRequestData_TextDocumentID(kObjectID_Invalid),mRequestData_IdInfoWindowID(kObjectID_Invalid)
,mTextDocumentID(kObjectID_Invalid),mIdInfoWindowID(kObjectID_Invalid)
,mRequestData_TextWindowID(kObjectID_Invalid),mTextWindowID(kObjectID_Invalid)
,mRequestData_TextViewID(kObjectID_Invalid),mTextViewID(kObjectID_Invalid)
,mRequestData_ArgIndex(kIndex_Invalid),mArgIndex(kIndex_Invalid)
,mRequestData_CurrentStateIndex(kIndex_Invalid),mCurrentStateIndex(kIndex_Invalid)
,mAbortCurrentRequest(false)
{
}

/**
�f�X�g���N�^
*/
AThread_IdInfoFinder::~AThread_IdInfoFinder()
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/**
�X���b�h���C�����[�`��
�X���b�h�̓A�v�����쒆�A��ɓ��삵�Ă���B
�v���f�[�^��ݒ肵�āAunpause����ƁA���������s����B
*/
void	AThread_IdInfoFinder::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_IdInfoFinder::NVIDO_ThreadMain started.",this);
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
		AArray<AIndex>	colorSlotIndexArray;
		AArray<AIndex>	startIndexArray;
		AArray<AIndex>	endIndexArray;
		AArray<AScopeType>	scopeArray;
		ATextArray	filePathArray;
		AArray<AItemCount>	matchedCountArray;
		
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
						mResult_KeywordTextArray.SetFromTextArray(keywordArray);
						mResult_ParentKeywordTextArray.SetFromTextArray(parentKeywordArray);
						mResult_TypeTextArray.SetFromTextArray(typeTextArray);
						mResult_InfoTextArray.SetFromTextArray(infoTextArray);
						mResult_CommentTextArray.SetFromTextArray(commentTextArray);
						mResult_CategoryIndexArray.SetFromObject(categoryIndexArray);
						mResult_ColorSlotIndexArray.SetFromObject(colorSlotIndexArray);
						mResult_ScopeArray.SetFromObject(scopeArray);
						mResult_StartIndexArray.SetFromObject(startIndexArray);
						mResult_EndIndexArray.SetFromObject(endIndexArray);
						mResult_FilePathArray.SetFromTextArray(filePathArray);
					}
					//�X���b�h��pause��Ԃɂ���
					NVI_Pause();
					//�r�����f����Ă��Ȃ���΁A���C���X���b�h�֊����ʒm
					if( mAbortCurrentRequest == false )
					{
						AObjectIDArray	objectIDArray;
						objectIDArray.Add(mTextDocumentID);
						objectIDArray.Add(mIdInfoWindowID);
						objectIDArray.Add(mTextWindowID);
						ABase::PostToMainInternalEventQueue(kInternalEvent_IdInfoFinderPaused,GetObjectID(),0,GetEmptyText(),objectIDArray);
					}
					//unpause�҂���ԂցB
					continue;
				}
				//�v���f�[�^��ǂݎ���āA��ɂ���
				//�h�L�������gID
				mTextDocumentID = mRequestData_TextDocumentID;
				mRequestData_TextDocumentID = kObjectID_Invalid;
				//IdInfoWindow
				mIdInfoWindowID = mRequestData_IdInfoWindowID;
				mRequestData_IdInfoWindowID = kObjectID_Invalid;
				//TextWindowID
				mTextWindowID = mRequestData_TextWindowID;
				mRequestData_TextWindowID = kObjectID_Invalid;
				//�v����text view
				mTextViewID = mRequestData_TextViewID;
				mRequestData_TextViewID = kObjectID_Invalid;
				//����word
				mWord.SetText(mRequestData_Word);
				mRequestData_Word.DeleteAll();
				//����parent word
				mParentWord.SetFromTextArray(mRequestData_ParentWord);
				mRequestData_ParentWord.DeleteAll();
				//����index
				mArgIndex = mRequestData_ArgIndex;
				mRequestData_ArgIndex = kIndex_Invalid;
				//����state
				mCurrentStateIndex = mRequestData_CurrentStateIndex;
				mRequestData_CurrentStateIndex = kIndex_Invalid;
				//�R�[����FuncId
				mCallerFuncIdText.SetText(mRequestData_CallerFuncIdText);
				mRequestData_CallerFuncIdText.DeleteAll();
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
			colorSlotIndexArray.DeleteAll();
			startIndexArray.DeleteAll();
			endIndexArray.DeleteAll();
			scopeArray.DeleteAll();
			filePathArray.DeleteAll();
			matchedCountArray.DeleteAll();
			
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
				//�u���b�N���o���Ƃ���DecrementRetainCount()���R�[�����邱�Ƃ�ۏ؂��邽�߂�stack�N���X
				//���NVI_GetDocumentAndRetain()�ɑΉ�
				AStDecrementRetainCounter	releaser(doc);
				
				ADocument_Text&	textdoc = reinterpret_cast<ADocument_Text&>(*doc);
				AModeIndex	modeIndex = textdoc.SPI_GetModeIndex();
				
				//�h�L�������g�̃t�@�C���p�X���擾
				AText	docFilePath;
				textdoc.NVI_GetFilePath(docFilePath);
				//�g���q���폜
				AText	docFilePathWithoutSuffix;
				docFilePathWithoutSuffix.SetText(docFilePath);
				docFilePathWithoutSuffix.DeleteAfter(docFilePathWithoutSuffix.GetSuffixStartPos());
				
				//�����I�v�V�����ݒ�
				//IdInfoWindow���ݒ�i��text view��id�N���b�N���j�̏ꍇ�́A�����I�v�V�����Ȃ��i�L�[���[�h���S��v�̂݁j
				AKeywordSearchOption	option = kKeywordSearchOption_OnlyEnabledCategoryForCurrentState;
				if( mIdInfoWindowID != kObjectID_Invalid )
				{
					//IdInfoWindow�ݒ�iIdInfoWindow�̌����{�b�N�X�j�̏ꍇ�́A�����I�v�V����������v�E�啶�������������EInfoText��������
					option = kKeywordSearchOption_Partial | 
							kKeywordSearchOption_IgnoreCases | 
							kKeywordSearchOption_IncludeInfoText |
							kKeywordSearchOption_OnlyEnabledCategoryForCurrentState;
				}
				
				// =================== ���[�J�����ʎq�����⌟�� =================== 
				if( mTextViewID != kObjectID_Invalid )
				{
					textdoc.SPI_SearchKeywordLocal(mWord,mParentWord,option,mCurrentStateIndex,
												   keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
												   completionTextArray,urlArray,
												   categoryIndexArray,colorSlotIndexArray,
												   startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
												   mTextViewID,
												   mAbortCurrentRequest);
				}
				
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
				AIndex	modeKeywordStartIndex = keywordArray.GetItemCount();
				GetApp().SPI_GetModePrefDB(modeIndex).
						SPI_SearchKeywordKeywordList(mWord,mParentWord,option,mCurrentStateIndex,
													 keywordArray,parentKeywordArray,typeTextArray,infoTextArray,commentTextArray,
													 completionTextArray,urlArray,
													 categoryIndexArray,colorSlotIndexArray,
													 startIndexArray,endIndexArray,scopeArray,filePathArray,matchedCountArray,
													 mAbortCurrentRequest);
				
				//�L�[���[�h���ڂ̂���infotext����̂��͍̂폜����
				{
					for( AIndex index = modeKeywordStartIndex; index < keywordArray.GetItemCount();  )
					{
						if( scopeArray.Get(index) == kScopeType_ModeKeyword &&
									infoTextArray.GetTextLen(index) == 0 )
						{
							keywordArray.Delete1(index);
							parentKeywordArray.Delete1(index);
							typeTextArray.Delete1(index);
							infoTextArray.Delete1(index);
							commentTextArray.Delete1(index);
							completionTextArray.Delete1(index);
							urlArray.Delete1(index);
							categoryIndexArray.Delete1(index);
							colorSlotIndexArray.Delete1(index);
							startIndexArray.Delete1(index);
							endIndexArray.Delete1(index);
							scopeArray.Delete1(index);
							filePathArray.Delete1(index);
							matchedCountArray.Delete1(index);
						}
						else
						{
							index++;
						}
					}
				}
				
				//==================�D�揇�ʐݒ�==================
				//�����ύX�B�D�捀�ڂ��ɕ\������
				//�D�捀�ڂ�index
				AIndex	priorIndex = 0;
				//�S���ڃ��[�v
				AItemCount	itemCount = keywordArray.GetItemCount();
				for( AIndex index = 0; index < itemCount; index++ )
				{
					//�D�悷�邩�ǂ����̔��茋�ʊi�[�t���O
					ABool	isPrior = false;
					//parent keyword��v�ɂ��D��
					if( mParentWord.GetItemCount() > 0 )
					{
						//parent keyword�擾
						AText	parentKeyword;
						parentKeywordArray.Get(index,parentKeyword);
						//�v���f�[�^�Ŏw�肳�ꂽparent keyword�Ȃ�D�悷��
						if( mParentWord.Find(parentKeyword) != kIndex_Invalid )
						{
							isPrior = true;
						}
					}
					//parent keyword���w��Ȃ�A�g���q���������t�@�C�����������ꍇ�ɗD�悷��
					else
					{
						//�t�@�C���p�X���擾���A�g���q������
						AText	filepath;
						filePathArray.Get(index,filepath);
						filepath.DeleteAfter(filepath.GetSuffixStartPos());
						//�g���q���������t�@�C�����������Ȃ�D�悷��
						if( filepath.Compare(docFilePathWithoutSuffix) == true )
						{
							isPrior = true;
						}
					}
					//�D�捀�ڂȂ�A�ŏ��̂ق��Ɉړ�����
					if( isPrior == true )
					{
						keywordArray.MoveObject(index,priorIndex);
						parentKeywordArray.MoveObject(index,priorIndex);
						typeTextArray.MoveObject(index,priorIndex);
						infoTextArray.MoveObject(index,priorIndex);
						commentTextArray.MoveObject(index,priorIndex);
						completionTextArray.MoveObject(index,priorIndex);
						urlArray.MoveObject(index,priorIndex);
						categoryIndexArray.Move(index,priorIndex);
						colorSlotIndexArray.Move(index,priorIndex);
						startIndexArray.Move(index,priorIndex);
						endIndexArray.Move(index,priorIndex);
						scopeArray.Move(index,priorIndex);
						filePathArray.MoveObject(index,priorIndex);
						matchedCountArray.Move(index,priorIndex);
						priorIndex++;
					}
				}
			}
		}
	}
	catch(...)
	{
		_ACError("AThread_IdInfoFinder::NVIDO_ThreadMain() caught exception.",NULL);
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_IdInfoFinder::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---�v���f�[�^�ݒ�

/**
�v���f�[�^�ݒ�
@note inParentWord�Ɉ�v���鍀�ڂ��D��\�������B
*/
void	AThread_IdInfoFinder::SetRequestData( const ADocumentID inTextDocumentID, 
											 const AWindowID inIdInfoWindowID, 
											 const AWindowID inTextWindowID, const AViewID inTextViewID, 
											 const AText& inWord, const ATextArray& inParentWord,
											 const AIndex inArgIndex, const AText& inCallerFuncIdText,
											 const AIndex inCurrentStateIndex )
{
	//�v���f�[�^mutex lock
	AStMutexLocker	locker(mRequestDataMutex);
	//�h�L�������gID�AIdInfowWindowID, TextWindowID�ݒ�
	mRequestData_TextDocumentID = inTextDocumentID;
	mRequestData_IdInfoWindowID = inIdInfoWindowID;
	mRequestData_TextWindowID = inTextWindowID;
	mRequestData_TextViewID = inTextViewID;
	//����word, parent word�ݒ�
	mRequestData_Word.SetText(inWord);
	mRequestData_ParentWord.SetFromTextArray(inParentWord);
	//arg index
	mRequestData_ArgIndex = inArgIndex;
	//�R�[����FuncId
	mRequestData_CallerFuncIdText.SetText(inCallerFuncIdText);
	//����state
	mRequestData_CurrentStateIndex = inCurrentStateIndex;
	//Abort�t���OON
	mAbortCurrentRequest = true;
}

/**
���݂̌����v���𒆒f
*/
void	AThread_IdInfoFinder::AbortCurrentRequest()
{
	mRequestData_TextDocumentID = kObjectID_Invalid;
	mAbortCurrentRequest = true;
}

#pragma mark ===========================

#pragma mark ---���ʃf�[�^�擾

/**
���̌��ʃf�[�^�ɑ΂���v���f�[�^�iword, parent word�j�̎擾
*/
void	AThread_IdInfoFinder::GetWordForResult( AText& outWordForResult, ATextArray& outParentWordForResult ) const
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
void	AThread_IdInfoFinder::GetResult( ATextArray& outKeywordTextArray, ATextArray& outParentKeywordTextArray,
		ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, ATextArray& outCommentTextArray,
		AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray, AArray<AScopeType>& outScopeArray,
		AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
		ATextArray& outFilePathArray, AIndex& outArgIndex, AText& outCallerFuncIdText ) const
{
	//���ʃf�[�^ mutex lock
	AStMutexLocker	locker(mResultMutex);
	//
	//�f�[�^�擾
	outKeywordTextArray.AddFromTextArray(mResult_KeywordTextArray);
	outParentKeywordTextArray.AddFromTextArray(mResult_ParentKeywordTextArray);
	outTypeTextArray.AddFromTextArray(mResult_TypeTextArray);
	outInfoTextArray.AddFromTextArray(mResult_InfoTextArray);
	outCommentTextArray.AddFromTextArray(mResult_CommentTextArray);
	outCategoryIndexArray.AddFromObject(mResult_CategoryIndexArray);
	outColorSlotIndexArray.AddFromObject(mResult_ColorSlotIndexArray);
	outScopeArray.AddFromObject(mResult_ScopeArray);
	outStartIndexArray.AddFromObject(mResult_StartIndexArray);
	outEndIndexArray.AddFromObject(mResult_EndIndexArray);
	outFilePathArray.AddFromTextArray(mResult_FilePathArray);
	outArgIndex = mArgIndex;
	outCallerFuncIdText.SetText(mCallerFuncIdText);
}
