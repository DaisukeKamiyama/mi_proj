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

ADocument_EditBox

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "AEditBoxUndoer.h"
class CTextDrawData;

#pragma mark ===========================
#pragma mark [�N���X]ADocument_EditBox
/**
AView_EditBox�p�e�L�X�g�h�L�������g�̃N���X
*/
class ADocument_EditBox : public ADocument
{
	//�R���X�g���N�^�A�f�X�g���N�^
  public:
	ADocument_EditBox( AObjectArrayItem* inParent );
	~ADocument_EditBox();
	
	//<�C���^�[�t�F�C�X>
	//�e�L�X�g�ҏW
  public:
	void					SPI_InsertText( const ATextPoint& inInsertPoint, const AText& inText );
	void					SPI_InsertText( const ATextIndex inInsertIndex, const AText& inText );
	void					SPI_DeleteText( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint );
	void					SPI_DeleteText( const ATextIndex inStart, const ATextIndex inEnd );
	void					SPI_RecordUndoActionTag( const AEditBoxUndoTag inTag );
  private:
	AText								mText;
	
	//�e�L�X�g�ɑ΂���e�푀��
  public:
	void					SPI_Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength );
	void					SPI_Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength );
	ABool					SPI_GetUndoText( AText& outText ) const;
	ABool					SPI_GetRedoText( AText& outText ) const;
	void					SPI_Save( const ABool inSaveAs );
	void					SPI_ClearUndoInfo();//#135
  private:
	AEditBoxUndoer						mUndoer;
	
	//�e�L�X�g�擾
  public:
	void					SPI_GetText( AText& outText ) const;
	void					SPI_GetText( const ATextIndex inStart, const ATextIndex inEnd, AText& outText ) const;
	void					SPI_GetText( const ATextPoint& inStart, const ATextPoint& inEnd, AText& outText ) const;
	void					SPI_GetTextDrawData( const AIndex inLineIndex, CTextDrawData& outTextDrawData ) const;
	AItemCount				SPI_GetTextLength() const { return mText.GetItemCount(); }
	AIndex					SPI_GetLineLengthWithoutLineEnd( const AIndex inLineIndex ) const { return GetLineLengthWithoutLineEnd(inLineIndex); }
	AIndex					SPI_GetPrevCharTextIndex( const ATextIndex inIndex ) const;
	AIndex					SPI_GetNextCharTextIndex( const ATextIndex inIndex ) const;
	AIndex					SPI_GetCurrentCharTextIndex( const ATextIndex inIndex ) const;
	void					SPI_FindWord( const ATextIndex inStartPos, ATextIndex& outStart, ATextIndex& outEnd ) const;
	ABool					SPI_IsAsciiAlphabet( const ATextIndex inPos ) const;
	ABool					SPI_IsUTF8Multibyte( const ATextIndex inIndex ) const;
	AUChar					SPI_GetTextChar( const ATextIndex inIndex ) const;
	
	//TextPoint/TextIndex�ϊ�
  public:
	AIndex					SPI_GetTextIndexFromTextPoint( const ATextPoint& inPoint ) const;
	void					SPI_GetTextPointFromTextIndex( const AIndex inTextPosition, ATextPoint& outPoint, const ABool inPreferNextLine = false ) const;
	
	//�s�v�Z
  public:
	AItemCount				SPI_GetLineCount() const;
	ABool					SPI_ExistLineEnd( const AIndex inLineIndex ) const;
  private:
	void					CalcLineStarts();
	AItemCount				GetTextLength() const;
	AItemCount				GetLineCount() const;
	AItemCount				GetLineLengthWithoutLineEnd( const AIndex inLineIndex ) const;
	AItemCount				GetLineLengthWithLineEnd( const AIndex inLineIndex ) const;
	AIndex					GetLineStart( const AIndex inLineIndex ) const;
	AArray<AIndex>						mLineStartArray;
	AArray<AItemCount>					mLineLengthArray;
	
  private:
	ADocumentType			NVIDO_GetDocumentType() const {return kDocumentType_EditBox;}
};

#pragma mark ===========================
#pragma mark [�N���X]AEditBoxDocumentFactory
typedef AAbstractFactoryForObjectArray<ADocument>	ADocumentFactory;
/**
ADocument_EditBox�����pFactory�N���X
*/
class AEditBoxDocumentFactory : public ADocumentFactory
{
  public:
	AEditBoxDocumentFactory( AObjectArrayItem*	inParent ) : mParent(inParent)
	{
	}
	ADocument*	Create()
	{
		return new ADocument_EditBox(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};



