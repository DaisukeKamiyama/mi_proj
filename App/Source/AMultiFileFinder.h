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

AMultiFileFinder
(C) 2008 Daisuke Kamiyama, All Rights Reserved.

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextFinder.h"

#pragma mark ===========================
#pragma mark [�N���X]AMultiFileFinder
//�}���`�t�@�C�������X���b�h�I�u�W�F�N�g
class AMultiFileFinder : public AThread
{
  public:
	AMultiFileFinder( AObjectArrayItem* inParent );
	void	SPNVI_Run( const AFindParameter& inFindParam, const ADocumentID inIndexDocumentID,
					  const ABool inForMultiFileReplace, const ABool inExtractText );//#725
	ABool	FindInFolder( const AFileAcc& inFolder, const AIndex inLevel );
	ABool	FindInFile( const AFileAcc& inFile );
	
  public:
	void	SPI_SleepForAWhile();
  private:
	void	NVIDO_ThreadMain();
	ABool	mSleepForAWhile;
	
	AFindParameter	mFindParam;
	ABool			mForMultiFileReplace;//#65
	ABool			mExtractMatchedText;//#937
	AFileAcc	mRootFolder;
	AObjectID	mIndexWindowDocumentID;
	ATextFinder	mTextFinder;
	AItemCount	mTotalLineCount;
	AItemCount	mHitCount;//R0238
	AItemCount	mHitFileCount;//#828
	AText		mExtractedText;//#937
	
	ABool			mFileFilterExist;
	ARegExp			mFileFilterRegExp;
	
	//#1378
	//�}���`�t�@�C�������I������́A�����I���C�x���g��M���ɂ܂��X���b�h�����삵�Ă���̂ŁA�X���b�h���쒆���ǂ����ł͂Ȃ��A�t���O�Q�Ƃɕς���B
  public:
	ABool	SPI_IsWorking() const { return mWorking; }
  private:
	ABool		mWorking;
	
	//#890
	//�����r�����f
  public:
	void			SPI_AbortRecognizeSyntax() { mAbortRecognizeSyntax = true; }
  private:
	ABool			mAbortRecognizeSyntax;
	void			NVIDO_AbortThread();//#890
};


#pragma mark ===========================
#pragma mark [�N���X]AMultiFileFinderFactory
//
class AMultiFileFinderFactory : public AThreadFactory
{
  public:
	AMultiFileFinderFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AMultiFileFinder*	Create() 
	{
		return new AMultiFileFinder(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};

#pragma mark ===========================
#pragma mark [�N���X]AMultiFileFindResult
//
class AMultiFileFindResult : public AObjectArrayItem
{
  public:
	AMultiFileFindResult( AObjectArrayItem* inParent = NULL ) : AObjectArrayItem(inParent){}
	
	void	SetIndexWindowDocumentID( const AObjectID inIndexWindowDocumentID )
	{
		mIndexWindowDocumentID = inIndexWindowDocumentID;
	}
	
	void	Add( AText& inFilePath, AText& inHitText, AText& inPreHitText, AText& inPostHitText, AText& inPositionText, 
			AText& inParagraphText,//#465
			const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph )
	{
		//#693 Add_SwapContent->Add�ɕύX�����̂ŁA���x�ቺ���Ă��邩������Ȃ��B�v�����B
		mFilePathArray.Add/*#693_SwapContent*/(inFilePath);
		mHitTextArray.Add/*#693_SwapContent*/(inHitText);
		mPreHitTextArray.Add/*#693_SwapContent*/(inPreHitText);
		mPostHitTextArray.Add/*#693_SwapContent*/(inPostHitText);
		mPositionTextArray.Add/*#693_SwapContent*/(inPositionText);
		mParagraphTextArray.Add/*#693_SwapContent*/(inParagraphText);//#465
		mStartPositionArray.Add(inStartPosition);
		mLengthArray.Add(inLength);
		mParagraphArray.Add(inParagraph);
	}
	
	AObjectID	GetIndexWindowDocumentID()
	{
		return mIndexWindowDocumentID;
	}
	
	const ATextArray&	GetFilePathArray() const {return mFilePathArray;}
	const ATextArray&	GetHitTextArray() const {return mHitTextArray;}
	const ATextArray&	GetPreHitTextArray() const {return mPreHitTextArray;}
	const ATextArray&	GetPostHitTextArray() const {return mPostHitTextArray;}
	const ATextArray&	GetPositionTextArray() const {return mPositionTextArray;}
	const ATextArray&	GetParagraphTextArray() const {return mParagraphTextArray;}//#465
	const ANumberArray&	GetStartPositionArray() const {return mStartPositionArray;}
	const ANumberArray&	GetLengthArray() const {return mLengthArray;}
	const ANumberArray&	GetParagraphArray() const {return mParagraphArray;}
	
	AItemCount	GetRecordItemCount()
	{
		return mFilePathArray.GetItemCount();
	}
	
  private:
	AObjectID	mIndexWindowDocumentID;
	ATextArray	mFilePathArray;
	ATextArray	mHitTextArray;
	ATextArray	mPreHitTextArray;
	ATextArray	mPostHitTextArray;
	ATextArray	mPositionTextArray;
	ATextArray	mParagraphTextArray;//#465
	ANumberArray	mStartPositionArray;
	ANumberArray	mLengthArray;
	ANumberArray	mParagraphArray;
	
	//Object���擾
  public:
	virtual AConstCharPtr	GetClassName() const { return "AMultiFileFindResult"; }
	
};


