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

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextInfo.h"

/**
���[�h���X�g�X���b�h�����^�C�v
*/
enum AWordsListFinderType
{
	kWordsListFinderType_ForCallGraf = 1,
	kWordsListFinderType_ForWordsList,
};

#pragma mark ===========================
#pragma mark [�N���X]AThread_WordsListFinder
/**
AThread_WordsListFinder�X���b�h�I�u�W�F�N�g
*/
class AThread_WordsListFinder : public AThread
{
	//�R���X�g���N�^
  public:
	AThread_WordsListFinder( AObjectArrayItem* inParent );
	~AThread_WordsListFinder();
	
	//�X���b�h���C�����[�`��
  private:
	void	NVIDO_ThreadMain();
	ABool						mAbortCurrentRequest;
	
	//
  public:
	void						SetWordsListFinderType( const AWordsListFinderType inType );
  private:
	AWordsListFinderType		mWordsListFinderType;
	
	//�v���f�[�^�ݒ�
  public:
	void						SetRequestDataForCallGraf( const AFileAcc& inProjectFolder, const AIndex inModeIndex, 
								const AWindowID inWindowID, 
								const AText& inWordText, const AIndex inRequestLevelIndex );
	void						SetRequestDataForWordsList( const AFileAcc& inProjectFolder, const AIndex inModeIndex,
								const AWindowID inWindowID, const AText& inWordText );
	void						AbortCurrentRequest();
	//�v���f�[�^
	//mRequestedDataMutex�ɂ��r������B
	//�X���b�h���s�J�n���ɁAmTextDocumentID���փR�s�[�����B
  private:
	AFileAcc					mRequestData_ProjectFolder;
	AWindowID					mRequestData_WindowID;
	AText						mRequestData_WordText;
	AIndex						mRequestData_ModeIndex;
	AIndex						mRequestData_RequestLevelIndex;
	mutable AThreadMutex		mRequestDataMutex;
	
	//���s�p�f�[�^
  private:
	AFileAcc					mProjectFolder;
	AWindowID					mWindowID;
	AText						mWordText;
	AIndex						mModeIndex;
	AIndex						mRequestLevelIndex;
	
	//���ʃf�[�^�擾
  public:
	ABool						GetResultForCallGraf( AWindowID& outWindowID, AIndex& outRequestLevelIndex,
								ATextArray& outFilePathArray, ATextArray& outClassNameArray, ATextArray& outFunctionNameArray,
								AArray<AIndex>& outStartArray, AArray<AIndex>& outEndArray ) const;
	ABool						GetResultForWordsList( 
								ATextArray& outFilePathArray, ATextArray& outClassNameArray, ATextArray& outFunctionNameArray,
								AArray<AIndex>& outStartArray, AArray<AIndex>& outEndArray,
								AArray<AColor>& outHeaderColorArray,
								ATextArray& outContentTextArray, AArray<AIndex>& outContentTextIndexArray ) const;
  private:
	//���ʃf�[�^
	//mResultMutex�ɂ��r������B
	//�X���b�h���Ō������s��A���[�J���ϐ�����R�s�[�����B
	AIndex						mResult_RequestLevelIndex;
	AWindowID					mResult_WindowID;
	AText						mResult_WordText;
	ATextArray					mResult_FilePathArray;
	ATextArray					mResult_ClassNameArray;
	ATextArray					mResult_FunctionNameArray;
	AArray<AIndex>				mResult_StartArray;
	AArray<AIndex>				mResult_EndArray;
	ATextArray					mResult_ContentTextArray;
	AArray<AIndex>				mResult_ContentTextStartIndexArray;
	AArray<AColor>				mResult_HeaderColorArray;
	ABool						mResult_AllFilesSearched;
	mutable AThreadMutex		mResultMutex;
};


#pragma mark ===========================
#pragma mark [�N���X]AThread_WordsListFinderFactory
//����Factory
class AThread_WordsListFinderFactory : public AThreadFactory
{
  public:
	AThread_WordsListFinderFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AThread_WordsListFinder*	Create() 
	{
		return new AThread_WordsListFinder(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};
