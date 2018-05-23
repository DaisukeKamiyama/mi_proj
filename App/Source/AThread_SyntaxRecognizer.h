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

AThread_SyntaxRecognizer
#698

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextInfo.h"

#pragma mark ===========================
#pragma mark [�N���X]AThread_SyntaxRecognizer
/**
AThread_SyntaxRecognizer�X���b�h�I�u�W�F�N�g
*/
class AThread_SyntaxRecognizer : public AThread
{
  public:
	AThread_SyntaxRecognizer( AObjectArrayItem* inParent );
	
	//�X���b�h���C�����[�`��
  private:
	void	NVIDO_ThreadMain();
	
	//�X���b�h�ݒ�E�N��
  public:
	void	SPI_Init( const ADocumentID inTextDocumentID );
	void	SPI_SetContext( const AModeIndex inModeIndex, const ATextInfo& inSrcTextInfo, const AIndex inStartLineIndex );
	void	SPI_SetText( const AText& inText, const AIndex inIndex, const AItemCount inCount );
	void	SPI_SetTextInfo( const ATextInfo& inSrcTextInfo, 
			const AIndex inStartLineIndex, const AIndex inEndLineIndex );
	void	SPI_PurgeTextinfoRegExp();
	
	//���ʎ擾
  public:
	ABool	SPI_RecognizerJudgedAsEnd() const { return mRecognizerJudgedAsEnd; }
	AIndex	SPI_GetRecognizeEndLineIndex() const { return mRecognizeEndLineIndex; }
	ATextInfo&	SPI_GetTextInfo() { return mTextInfo; }
	
	//�X���b�h�I���㏈��
  public:
	void	ClearData();
	
	//�f�[�^
  private:
	ADocumentID					mTextDocumentID;
	AIndex						mModeIndex;
	AText						mText;
	ATextInfo					mTextInfo;
	AThreadMutex				mTextInfoMutex;
	ABool						mRecognizerJudgedAsEnd;
	AIndex						mRecognizeEndLineIndex;
	
};


#pragma mark ===========================
#pragma mark [�N���X]AThread_SyntaxRecognizerFactory
//
class AThread_SyntaxRecognizerFactory : public AThreadFactory
{
  public:
	AThread_SyntaxRecognizerFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AThread_SyntaxRecognizer*	Create() 
	{
		return new AThread_SyntaxRecognizer(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};
