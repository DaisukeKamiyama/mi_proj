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

AThread_WordsListFinder
#723

*/

#include "stdafx.h"

#include "AThread_WordsListFinder.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AThread_WordsListFinder
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
AThread_WordsListFinder::AThread_WordsListFinder( AObjectArrayItem* inParent ) : AThread(inParent)
,mRequestData_WindowID(kObjectID_Invalid)
,mRequestData_ModeIndex(kIndex_Invalid)
,mRequestData_RequestLevelIndex(kIndex_Invalid)
,mWindowID(kObjectID_Invalid)
,mModeIndex(kIndex_Invalid)
,mRequestLevelIndex(kIndex_Invalid)
,mResult_RequestLevelIndex(kIndex_Invalid)
,mResult_WindowID(kObjectID_Invalid)
,mResult_AllFilesSearched(false)
,mWordsListFinderType(kWordsListFinderType_ForCallGraf)
,mAbortCurrentRequest(false)
{
}

/**
�f�X�g���N�^
*/
AThread_WordsListFinder::~AThread_WordsListFinder()
{
}

/**
���[�h���X�g�����^�C�v�ݒ�
*/
void	AThread_WordsListFinder::SetWordsListFinderType( const AWordsListFinderType inType )
{
	mWordsListFinderType = inType;
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/**
�X���b�h���C�����[�`��
�X���b�h�̓A�v�����쒆�A��ɓ��삵�Ă���B
�v���f�[�^��ݒ肵�āAunpause����ƁA���������s����B
*/
void	AThread_WordsListFinder::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_WordsListFinder::NVIDO_ThreadMain started.",this);
	try
	{
		// =================== ���ʊi�[�p���[�J���ϐ� =================== 
		//���ʎ擾�p���[�J���ϐ�
		ATextArray	filePathArray;
		ATextArray	classNameArray;
		ATextArray	functionNameArray;
		AArray<AIndex>	startArray;
		AArray<AIndex>	endArray;
		ATextArray	contentTextArray;
		AArray<AIndex>	contentTextStartIndexArray;
		AArray<AColor>	headerColorArray;
		ABool	allFilesSearched = true;
		
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
				if( mRequestData_WordText.GetItemCount() == 0 )
				{
					// =================== �i�v���f�[�^�������ꍇ�j������������ =================== 
					//���ʂ��i�[
					{
						//����mutex lock
						AStMutexLocker	locker(mResultMutex);
						mResult_RequestLevelIndex = mRequestLevelIndex;
						mResult_WindowID = mWindowID;
						mResult_WordText.SetText(mWordText);
						mResult_FilePathArray.SetFromTextArray(filePathArray);
						mResult_ClassNameArray.SetFromTextArray(classNameArray);
						mResult_FunctionNameArray.SetFromTextArray(functionNameArray);
						mResult_StartArray.SetFromObject(startArray);
						mResult_EndArray.SetFromObject(endArray);
						mResult_ContentTextArray.SetFromTextArray(contentTextArray);
						mResult_ContentTextStartIndexArray.SetFromObject(contentTextStartIndexArray);
						mResult_HeaderColorArray.SetFromObject(headerColorArray);
						mResult_AllFilesSearched = allFilesSearched;
					}
					//�X���b�h��pause��Ԃɂ���
					NVI_Pause();
					//�r�����f����Ă��Ȃ���΁A���C���X���b�h�֊����ʒm
					if( mAbortCurrentRequest == false )
					{
						AObjectIDArray	objectIDArray;
						objectIDArray.Add(mResult_WindowID);
						switch(mWordsListFinderType)
						{
						  case kWordsListFinderType_ForCallGraf:
							{
								ABase::PostToMainInternalEventQueue(kInternalEvent_WordsListFinderPausedForCallGraf,GetObjectID(),0,GetEmptyText(),objectIDArray);
								break;
							}
						  case kWordsListFinderType_ForWordsList:
							{
								ABase::PostToMainInternalEventQueue(kInternalEvent_WordsListFinderPausedForWordsList,GetObjectID(),0,GetEmptyText(),objectIDArray);
								break;
							}
						}
					}
					//unpause�҂���ԂցB
					continue;
				}
				//�v���f�[�^��ǂݎ���āA��ɂ���
				//�h�L�������gID
				mProjectFolder = mRequestData_ProjectFolder;
				mRequestData_ProjectFolder.Unspecify();
				//mode index
				mModeIndex = mRequestData_ModeIndex;
				mRequestData_ModeIndex = kIndex_Invalid;
				//�E�C���h�EID
				mWindowID = mRequestData_WindowID;
				mRequestData_WindowID = kObjectID_Invalid;
				//����word
				mWordText.SetText(mRequestData_WordText);
				mRequestData_WordText.DeleteAll();
				//callgraf��level
				mRequestLevelIndex = mRequestData_RequestLevelIndex;
				mRequestData_RequestLevelIndex = kIndex_Invalid;
				//Abort�t���O��OFF�ɂ���i���s���ɗv���f�[�^�ݒ肳�ꂽ��t���OON�ɂȂ�j
				mAbortCurrentRequest = false;
			}
			
			// =================== ���ʊi�[�惍�[�J���ϐ������� =================== 
			filePathArray.DeleteAll();
			classNameArray.DeleteAll();
			functionNameArray.DeleteAll();
			startArray.DeleteAll();
			endArray.DeleteAll();
			contentTextArray.DeleteAll();
			contentTextStartIndexArray.DeleteAll();
			headerColorArray.DeleteAll();
			allFilesSearched = true;
			
			// =================== ���[�h���X�g�������s =================== 
			
			//�v��project folder, mode index�ɑ�����t�@�C����importFileData�I�u�W�F�N�g���擾
			//�iimportFileData�͖��F���̉\��������B�j
			AArray<AObjectID>	importFileDataIDArray;
			GetApp().SPI_GetImportFileDataIDArrayForWordsList(mProjectFolder,mModeIndex,importFileDataIDArray,mAborted);
			//ImportFileData�I�u�W�F�N�g���̃��[�v
			for( AIndex i = 0; i < importFileDataIDArray.GetItemCount(); i++ )
			{
				//���f�v���������break
				if( mAbortCurrentRequest == true || NVM_IsAborted() == true )
				{
					allFilesSearched = false;
					break;
				}
				//�������ʂ��������ȏ�ɂȂ�����break
				if( filePathArray.GetItemCount() >= kLimit_WordsListResultCount )
				{
					allFilesSearched = false;
					break;
				}
				//------------------Import File Data�I�u�W�F�N�g�擾------------------
				//ImportFileData�폜���A���̒���TextInfo�̃f�[�^���폜�����̂�ImportFileData�̃f�X�g���N�^���Ȃ̂ŁA
				//AObjectArrayIDIndexed��getandretain�@�\���g���΁ADecrementRetainCount()����܂ł́ATextInfo�̃f�[�^���ۏ؂����B
				AImportFileData*	importFileDataPtr = 
									GetApp().SPI_GetImportFileManager().GetImportFileDataAndRetain(importFileDataIDArray.Get(i));
				if( importFileDataPtr != NULL )//����ImportFileData���폜�ς݂̏ꍇ�́ANULL���Ԃ�
				{
					//���̃u���b�N�𔲂���Ƃ���DecrementRetainCount()��K���R�[�����邽�߂�stack�I�u�W�F�N�g
					//�i���GetImportFileDataAndRetain()��IncrementRetainCount()���Ă���̂ɑΉ����Ă���j
					AStDecrementRetainCounter	releaser(importFileDataPtr);
					
					const AImportFileData&	importFileData = *(importFileDataPtr);
					//�r�����Ȃ��ƁA�����r����ImportFileData��ImportFileRecognizer�ɂčĔF�������ɓ���\��������B
					AStImportFileDataLocker	locker(importFileData);
					if( importFileData.IsRecognized() == false )
					{
						//ImportFileData���F���̏ꍇ
						
						//�������s���Ȃ��B
						
						//�t���O��ݒ�B
						allFilesSearched = false;
					}
					else
					{
						//ImportFileData�F���ς݂̏ꍇ
						
						//------------------���[�h���X�g�擾------------------
						//ImportFileData�̃t�@�C���p�X�擾
						AText	filepath;
						importFileData.GetFile().GetPath(filepath);
						//words list���猟���i�X���b�h�Z�[�t�j
						AArray<AIndex>	sposArray, eposArray;
						importFileData.GetTextInfoConst().FindWordsList(mWordText,sposArray,eposArray);
						if( sposArray.GetItemCount() > 0 )
						{
							//------------------�t�@�C���̃e�L�X�g�ǂݍ���------------------
							//���[�h���X�g�p�̏ꍇ�̂݃e�L�X�g��ǂݍ��ށi���Ӄe�L�X�g�\���̂��߁j
							AText	importFileText;
							if( mWordsListFinderType == kWordsListFinderType_ForWordsList )
							{
								AModeIndex	modeIndex = 0;
								AText	tecname;
								GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_WordsListFinder,
															  importFileData.GetFile(),importFileText,modeIndex,tecname);
							}
							//------------------���̃t�@�C���̃��[�h���X�g�̊e���ږ��̃��[�v------------------
							for( AIndex j = 0; j < sposArray.GetItemCount(); j++ )
							{
								//���f�v���������break
								if( mAbortCurrentRequest == true || NVM_IsAborted() == true )
								{
									allFilesSearched = false;
									break;
								}
								//�������ʂ��������ȏ�ɂȂ�����break
								if( filePathArray.GetItemCount() >= kLimit_WordsListResultCount )
								{
									allFilesSearched = false;
									break;
								}
								//�t�@�C���p�X
								filePathArray.Add(filepath);
								//------------------�J�n�E�I��text index�擾------------------
								AIndex	spos = sposArray.Get(j);
								AIndex	epos = eposArray.Get(j);
								startArray.Add(spos);
								endArray.Add(epos);
								//------------------���o���f�[�^�擾------------------
								//���̈ʒu�ɑΉ�����local start identifier���擾���A�L�[���[�h�i�֐����j�A�N���X���A�F���擾
								ATextPoint	textpt = {0,0};
								importFileData.GetTextInfoConst().GetTextPointFromTextIndex(spos,textpt);
								AText	functionName, className;
								AColor	headercolor = kColor_Blue;
								AUniqID	identifierUniqID = importFileData.GetTextInfoConst().GetLocalStartIdentifierByLineIndex(textpt.y);
								if( identifierUniqID != kUniqID_Invalid )
								{
									//local start identifier�̃L�[���[�h�e�L�X�g�܂��̓��j���[�e�L�X�g���擾
									if( importFileData.GetTextInfoConst().GetGlobalIdentifierKeywordText(identifierUniqID,functionName) == false )
									{
										importFileData.GetTextInfoConst().GetGlobalIdentifierMenuText(identifierUniqID,functionName);
									}
									//local start identifier�̕����F���擾
									AIndex	categoryIndex = importFileData.GetTextInfoConst().GetGlobalIdentifierCategoryIndex(identifierUniqID);
									if( categoryIndex != kIndex_Invalid )
									{
										GetApp().SPI_GetModePrefDB(importFileData.GetModeIndex()).GetCategoryColor(categoryIndex,headercolor);
									}
									//local start identifier�̃N���X�����擾
									importFileData.GetTextInfoConst().GetGlobalIdentifierParentKeywordText(identifierUniqID,className);
								}
								classNameArray.Add(className);
								functionNameArray.Add(functionName);
								headerColorArray.Add(headercolor);
								//------------------���Ӄe�L�X�g�^���Ӄe�L�X�g�J�n�e�L�X�g�C���f�b�N�X���擾------------------
								AText	contentText;
								AIndex	contentTextStartIndex = kIndex_Invalid;
								if( mWordsListFinderType == kWordsListFinderType_ForWordsList )
								{
									//wordslist�̏ꍇ�̂݁AcontentText�Ɏ��Ӄe�L�X�g�i�O��1�s���j���i�[����
									
									//�O���s���ƂȂ�J�n�E�I���|�C���g���擾
									ATextPoint	spt = textpt;
									ATextPoint	ept = textpt;
									//�O�s�̍ŏ����J�n�ʒu�ɂ���
									spt.x = 0;
									spt.y -= 1;
									if( spt.y < 0 )
									{
										spt.y = 0;
									}
									//���̎��̍s�̍ŏ����J�n�ʒu�ɂ���
									ept.x = 0;
									ept.y += 2;
									if( ept.y >= importFileData.GetTextInfoConst().GetLineCount() )
									{
										ept.y = importFileData.GetTextInfoConst().GetLineCount()-1;
									}
									//�J�n�I��text index�擾
									AIndex	start = importFileData.GetTextInfoConst().GetTextIndexFromTextPoint(spt);
									AIndex	end = importFileData.GetTextInfoConst().GetTextIndexFromTextPoint(ept);
									//�o�C�g����1000�ȏ�ƂȂ�ꍇ�́A�O��400�o�C�g�ɂ���
									if( end-start >= 1000 )
									{
										//400�o�C�g�O
										start = spos-400;
										if( start < 0 )
										{
											start = 0;
										}
										start = importFileText.GetCurrentCharPos(start);
										//400�o�C�g��
										end = epos+400;
										if( end > importFileText.GetItemCount() )
										{
											end = importFileText.GetItemCount();
										}
										end = importFileText.GetCurrentCharPos(end);
									}
									//text index�␳
									if( start < 0 )   start = 0;
									if( start >= importFileText.GetItemCount() )   start = importFileText.GetItemCount();
									if( end < 0 )   end = 0;
									if( end >= importFileText.GetItemCount() )   end = importFileText.GetItemCount();
									//���Ӄe�L�X�g�擾
									importFileText.GetText(start,end-start,contentText);
									//���Ӄe�L�X�g�J�n�ʒu�ݒ�
									contentTextStartIndex = start;
								}
								//���Ӄe�L�X�g�ݒ�
								contentTextArray.Add(contentText);
								contentTextStartIndexArray.Add(contentTextStartIndex);
							}
						}
					}
				}
			}
		}
	}
	catch(...)
	{
		_ACError("AThread_WordsListFinder::NVIDO_ThreadMain() caught exception.",NULL);
	}
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_WordsListFinder::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---�v���f�[�^�ݒ�

/**
�v���f�[�^�ݒ�i�R�[���O���t�p�j
*/
void	AThread_WordsListFinder::SetRequestDataForCallGraf( const AFileAcc& inProjectFolder, const AIndex inModeIndex,
		const AWindowID inWindowID, const AText& inWordText, const AIndex inRequestLevelIndex )
{
	//�v���f�[�^mutex lock
	AStMutexLocker	locker(mRequestDataMutex);
	//Project folder, mode index, WindowID�ݒ�
	mRequestData_ProjectFolder = inProjectFolder;
	mRequestData_ModeIndex = inModeIndex;
	mRequestData_WindowID = inWindowID;
	//����word�ݒ�
	mRequestData_WordText.SetText(inWordText);
	//callgraf��level
	mRequestData_RequestLevelIndex = inRequestLevelIndex;
	//Abort�t���O��ON�ɂ���B�i���ݎ��s���̂�search�����͒��f����A����̗v���ǂݍ��ݏ����ֈڍs����j
	mAbortCurrentRequest = true;
}

/**
�v���f�[�^�ݒ�i���[�h���X�g�p�j
*/
void	AThread_WordsListFinder::SetRequestDataForWordsList( const AFileAcc& inProjectFolder, const AIndex inModeIndex,
		const AWindowID inWindowID, const AText& inWordText )
{
	//�v���f�[�^mutex lock
	AStMutexLocker	locker(mRequestDataMutex);
	//Project folder, mode index, WindowID�ݒ�
	mRequestData_ProjectFolder = inProjectFolder;
	mRequestData_ModeIndex = inModeIndex;
	mRequestData_WindowID = inWindowID;
	//����word�ݒ�
	mRequestData_WordText.SetText(inWordText);
	//callgraf��level
	mRequestData_RequestLevelIndex = kIndex_Invalid;
	//Abort�t���O��ON�ɂ���B�i���ݎ��s���̂�search�����͒��f����A����̗v���ǂݍ��ݏ����ֈڍs����j
	mAbortCurrentRequest = true;
}

/**
���݂̌����v���𒆒f
*/
void	AThread_WordsListFinder::AbortCurrentRequest()
{
	mRequestData_WordText.DeleteAll();
	mAbortCurrentRequest = true;
}

#pragma mark ===========================

#pragma mark ---���ʃf�[�^�擾

/**
���ʃf�[�^�擾�i�R�[���O���t�p�j
*/
ABool	AThread_WordsListFinder::GetResultForCallGraf( AWindowID& outWindowID, AIndex& outRequestLevelIndex, 
		ATextArray& outFilePathArray, ATextArray& outClassNameArray, ATextArray& outFunctionNameArray,
		AArray<AIndex>& outStartArray, AArray<AIndex>& outEndArray ) const
{
	//���ʃf�[�^ mutex lock
	AStMutexLocker	locker(mResultMutex);
	//WindowID�擾
	outWindowID = mResult_WindowID;
	//call graf leve�擾
	outRequestLevelIndex = mResult_RequestLevelIndex;
	//�������ʎ擾
	outFilePathArray.SetFromTextArray(mResult_FilePathArray);
	outClassNameArray.SetFromTextArray(mResult_ClassNameArray);
	outFunctionNameArray.SetFromTextArray(mResult_FunctionNameArray);
	outStartArray.SetFromObject(mResult_StartArray);
	outEndArray.SetFromObject(mResult_EndArray);
	return mResult_AllFilesSearched;
}

/**
���ʃf�[�^�擾�i���[�h���X�g�p�j
*/
ABool	AThread_WordsListFinder::GetResultForWordsList( 
		ATextArray& outFilePathArray, ATextArray& outClassNameArray, ATextArray& outFunctionNameArray,
		AArray<AIndex>& outStartArray, AArray<AIndex>& outEndArray,
		AArray<AColor>& outHeaderColorArray,
		ATextArray& outContentTextArray, AArray<AIndex>& outContentTextIndexArray ) const
{
	//���ʃf�[�^ mutex lock
	AStMutexLocker	locker(mResultMutex);
	//�������ʎ擾
	outFilePathArray.SetFromTextArray(mResult_FilePathArray);
	outClassNameArray.SetFromTextArray(mResult_ClassNameArray);
	outFunctionNameArray.SetFromTextArray(mResult_FunctionNameArray);
	outStartArray.SetFromObject(mResult_StartArray);
	outEndArray.SetFromObject(mResult_EndArray);
	outHeaderColorArray.SetFromObject(mResult_HeaderColorArray);
	outContentTextArray.SetFromTextArray(mResult_ContentTextArray);
	outContentTextIndexArray.SetFromObject(mResult_ContentTextStartIndexArray);
	return mResult_AllFilesSearched;
}
