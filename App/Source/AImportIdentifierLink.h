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

AImportIdentifierLink

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

//#423 class AImportFileData;

#pragma mark ===========================
#pragma mark [クラス]AImportIdentifierLinkList
//
class AImportIdentifierLinkList : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	AImportIdentifierLinkList();
	~AImportIdentifierLinkList();
	
	//追加／削除
  public:
	void			UpdateIdentifierLinkList( const AHashArray<AObjectID>& inImportFileDataObjectIDArray );
	void			DeleteAll();
	ABool			Delete( const AObjectID inImportFileDataObjectID );
	
	//検索
  public:
	ABool			FindKeyword( const AText& inKeywordText, AIndex& outCategoryIndex ) const;
	ABool			FindKeyword( const AText& inKeywordText, AFileAcc& outFile, AIndex& outCategoryIndex, AText& outInfoText ) const;
	ABool			FindKeywordType( const AText& inKeywordText, AText& outTypeText ) const;//R0243
	/*#717
	void			GetAbbrevCandidate( const AText& inText, const AFileAcc& inDocFile, 
					ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
					AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray, AObjectArray<AFileAcc>& outFileArray ) const;//RC2
	void			GetAbbrevCandidateByParentKeyword( const AText& inParentKeyword, 
					ATextArray& outAbbrevCandidateArray, ATextArray& outInfoTextArray,
	AArray<AIndex>& outCategoryIndexArray, AArray<AScopeType>& outScopeArray, AObjectArray<AFileAcc>& outFileArray ) const;//R0243
	*/
	void			GetIdentifierListByKeyword( const AText& inText, AArray<AObjectID>& outIdentifierIDArray ) const;
	void			GetIdentifierListByKeyword( const AText& inText, 
					AArray<AIndex>& outCategoryIndexArray, ATextArray& outInfoText, AObjectArray<AFileAcc>& outFileArray,
					AArray<ATextIndex>& outStartArray, AArray<ATextIndex>& outEndArray,
											   ATextArray& outCommentTextArray, ATextArray& outParentKeywordTextArray ) const;//RC2
	ABool			SearchKeywordImport( const AText& inWord, const AHashTextArray& inParentWord, const AKeywordSearchOption inOption,
										const AIndex inCurrentStateIndex,
										AHashTextArray& outKeywordArray, ATextArray& outParentKeywordArray,
										ATextArray& outTypeTextArray, ATextArray& outInfoTextArray, 
										ATextArray& outCommentTextArray,
										ATextArray& outCompletionTextArray, ATextArray& outURLArray,
										AArray<AIndex>& outCategoryIndexArray, AArray<AIndex>& outColorSlotIndexArray,
										AArray<AIndex>& outStartIndexArray, AArray<AIndex>& outEndIndexArray,
										AArray<AScopeType>& outScopeArray, ATextArray& outFilePathArray,
										AArray<AItemCount>& outMatchedCountArray,
										const ABool& inAbort  ) const;//#853
	//ハッシュ
  private:
	void			InitHash( const AItemCount inItemCount );
	void			RegisterHash( /*#423 AImportFileData* inImportFileData*/
					const AObjectID inImportFileDataID , const AIndex inIndex );//#423
	ABool			DeleteFromHash( /*#423 AImportFileData* inImportFileData*/
					const AObjectID inImportFileDataID );//#423
	AIndex			Find( const AText& inKeywordText ) const;
	void			Find( const AText& inKeywordText, AArray<AIndex>& outIndexArray ) const;
	AIndex			GetNextHashPos( const AIndex inHashPosition ) const
	{
		if( inHashPosition == 0 )
		{
			return mHash_IdentifierIndex.GetItemCount()-1;
		}
		else
		{
			return inHashPosition -1;
		}
	}
	unsigned long	GetHashValue( const AText& inData ) const
	{
		AItemCount	targetItemCount = inData.GetItemCount();
		if( targetItemCount == 0 )
		{
			return 1234567;
		}
		else if( targetItemCount == 1 )
		{
			return inData.Get(0)*67890123;
		}
		else if( targetItemCount == 2 )
		{
			return inData.Get(0)*78901234 + inData.Get(1)*891012345;
		}
		else 
		{
			return inData.Get(0)*8901234 + inData.Get(1)*9012345 
			+ inData.Get(targetItemCount-2)*1234567 + inData.Get(targetItemCount-1) + targetItemCount*4567;
		}
	}
	
	//Revision #423
  private:
	AHashArray<AObjectID>		mRevisionData_ImportFileDataID;
	AArray<ANumber>				mRevisionData_RevisonNumber;
	
	//リンクデータ（ハッシュ）
	//ImportFileDataへのポインタ
	//#423 AArray<AImportFileData*>	mHash_ImportFileData;
	AArray<AObjectID>			mHash_ImportFileDataID;//#423
	//ImportFileData内のインデックス
	AArray<AIndex>				mHash_IdentifierIndex;
};

