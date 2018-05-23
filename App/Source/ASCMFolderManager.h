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

ASCMFolderManager
R0006
*/

#pragma once

#include "../../AbsFramework/Frame.h"

enum ASCMFolderType
{
	kSCMFolderType_NotSCMFolder = 0,
	kSCMFolderType_CVS,
	//R0247
	kSCMFolderType_SVN,
	kSCMFolderType_Git,//#589
};

enum ASCMFileState
{
	kSCMFileState_NotSCMFolder = 0,
	kSCMFileState_UpToDate,
	kSCMFileState_Modified,
	kSCMFileState_Added,
	kSCMFileState_Others,
	kSCMFileState_NotEntried
};

#pragma mark ===========================
#pragma mark [クラス]ASCMFolderData
//
class ASCMFolderData : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	ASCMFolderData( AObjectArrayItem* inParent, const AFileAcc& inFolder, const ABool inIsProjectSubFolder );
	~ASCMFolderData();
  private:
	void				FindSCMConfigFolderRecursive( const AFileAcc& inFolder );
	
  public:
	void				UpdateStatus();
	void				DoStatusCommandResult( const AText& inText );
	
	ASCMFileState		GetFileState( const AText& inFilename ) const;
	void				GetFileStateText( const AText& inFilename, AText& outText ) const;
	void				GetFileStateArray( const ATextArray& inFilenameArray, AArray<ASCMFileState>& outFileStateArray ) const;
	void				CompareWithLatest( const AFileAcc& inFile, const ADocumentID& inDocumentID );//#736
	//#736 void				DoCompareResult( const AFileAcc& inFile, const AText& inText );
	ASCMFolderType		GetSCMFolderType() const { return mSCMFolderType; }
	const AText&		GetRepositoryPath() const { return mRepository; }//#455
  private:
	ASCMFolderType					mSCMFolderType;
	AFileAcc						mFolder;
	AText							mRepository;
	ABool							mIsProjectSubFolder;//#961
	AFileAcc						mRootFolder;//#589
	
	//status制御
  public:
	void				SetStatus( const AFileAcc& inFile, const AText& inStatusText, const ASCMFileState inSCMFileState );
	
  private:
	AHashTextArray					mFileNameArray;
	AArray<ASCMFileState>			mFileStateArray;
	ATextArray						mFileStateTextArray;
	
	ABool							mNowUpdatingStatus;
};

#pragma mark ===========================
#pragma mark [クラス]ASCMFolderDataFactory
//
class ASCMFolderDataFactory : public AAbstractFactoryForObjectArray<ASCMFolderData>
{
  public:
	ASCMFolderDataFactory( AObjectArrayItem* inParent, const AFileAcc& inFolder, const ABool inIsProjectSubFolder )
	: mParent(inParent), mIsProjectSubFolder(inIsProjectSubFolder)
	{
		mFolder.CopyFrom(inFolder);
	}
	ASCMFolderData*	Create() 
	{
		return new ASCMFolderData(mParent,mFolder,mIsProjectSubFolder);
	}
  private:
	AObjectArrayItem*	mParent;
	AFileAcc			mFolder;
	ABool				mIsProjectSubFolder;//#961
};

#pragma mark ===========================
#pragma mark [クラス]ASCMFolderManager
//
class ASCMFolderManager : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	ASCMFolderManager();
	~ASCMFolderManager();
	
	//
  private:
	AIndex				GetSCMFolderDataIndexByFile( const AFileAcc& inFile ) const;
	AIndex				GetSCMFolderDataIndexByFolder( const AFileAcc& inFolder ) const;
	AIndex				GetSCMFolderDataIndexByFilePath( const AText& inFilePath ) const;
	
	//
  public:
	void				UpdateFolderStatus( const AFileAcc& inFolder );
	void				UpdateFolderStatusAll();
	
	//フォルダ登録
  public:
	void				RegisterFolder( const AFileAcc& inFolder );
	void				RegisterFolders( const AObjectArray<AFileAcc>& inFolders, const AIndex inRootFolderIndex );//#402
	
	//status制御
  public:
	void				SetStatus( const AFileAcc& inFile, const AText& inStatusText, const ASCMFileState inSCMFileState );
	
  public:
	void				DoStatusCommandResult( const AFileAcc& inFolder, const AText& inText );
	ASCMFileState		GetFileState( const AFileAcc& inFile ) const;
	ASCMFileState		GetFileState( const AText& inFilePath ) const;
	void				GetFileStateText( const AFileAcc& inFile, AText& outText ) const;
	void				GetFileStateArray( const AFileAcc& inFolder, const ATextArray& inFilenameArray, 
						AArray<ASCMFileState>& outFileStateArray ) const;
	void				CompareWithLatest( const AFileAcc& inFile, const ADocumentID& inDocumentID );//#736
	//#736 void				DoCompareResult( const AFileAcc& inFile, const AText& inText );
	ABool				IsSCMFolder( const AFileAcc& inFolder ) const;
	ASCMFolderType		GetSCMFolderType( const AFileAcc& inFile ) const;//#455
	const AText&		GetRepositoryPath( const AFileAcc& inFile ) const;//#455
	void				UpdateStatus( const AFileAcc& inFile );//#455
	//
  private:
	
  private:
	AObjectArray<ASCMFolderData>	mSCMFolderDataArray;
	AHashTextArray					mSCMFolderDataArray_PathText;
};






