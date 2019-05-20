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

AThread_WrapCalculator
#699

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextInfo.h"

#pragma mark ===========================
#pragma mark [�N���X]AThread_WrapCalculator
/**
AThread_WrapCalculator�X���b�h�I�u�W�F�N�g
*/
class AThread_WrapCalculator : public AThread
{
  public:
	AThread_WrapCalculator( AObjectArrayItem* inParent );
	
	//�X���b�h���C�����[�`��
  private:
	void	NVIDO_ThreadMain();
	
	//�X���b�h�ݒ�E�N��
  public:
	void	SPI_Init( const ADocumentID inTextDocumentID );
	void	SPI_SetContext( const ATextInfo& inSrcTextInfo, const AIndex inStartLineIndex );
	void	SPI_SetText( const AText& inText, const AIndex inIndex, const AItemCount inCount );
	void	SPI_SetWrapParameters( 
			const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
			const AItemCount inTabWidth, const AItemCount inIndentWidth, 
			const AWrapMode inWrapMode, const AItemCount inWrapLetterCount, 
			const ANumber inViewWidth, const ABool inCountAs2Letters, const AArray<AIndex>& inTabPositions );//#1421
	
	//�X���b�h�I���㏈��
  public:
	void	ClearData();
	
	//�f�[�^
  public:
	ATextInfo&	SPI_GetTextInfo() { return mTextInfo; }
  private:
	ADocumentID					mTextDocumentID;
	AText						mText;
	ATextInfo					mTextInfo;
	AText						mFontName;
	AFloatNumber				mFontSize;
	ABool						mAntiAliasing;
	AItemCount					mTabWidth;
	AItemCount					mIndentWidth;
	AWrapMode					mWrapMode;
	AItemCount					mWrapLetterCount;
	ANumber						mViewWidth;
	ABool						mCountAs2Letters;
	AArray<AIndex>				mTabPositions;//#1421
	
};


#pragma mark ===========================
#pragma mark [�N���X]AThread_WrapCalculatorFactory
//
class AThread_WrapCalculatorFactory : public AThreadFactory
{
  public:
	AThread_WrapCalculatorFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AThread_WrapCalculator*	Create() 
	{
		return new AThread_WrapCalculator(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};
