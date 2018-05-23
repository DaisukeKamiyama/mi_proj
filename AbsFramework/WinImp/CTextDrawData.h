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

CTextDrawData (Windows)

*/

#pragma once

#include "../AbsBase/ABase.h"

#pragma mark ===========================
#pragma mark [�N���X]CTextDrawData
/**
TextDrawData�N���X
�F���̏��t���`��p�f�[�^���i�[���AImp�N���X�ɓn�����߂̃N���X
*/
class CTextDrawData
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	CTextDrawData();
	~CTextDrawData();
	
	//B0000 �s�܂�Ԃ��v�Z������
  public:
	void	Init();
	
	//TextDrawData�쐬
  public:
	void	MakeTextDrawDataWithoutStyle( const AText& inText, const ANumber inTabWidth, const AItemCount inLetterCountLimit, 
			const ABool inGetUTF16Text, const ABool inCountAs2Letters, const ABool inDisplayControlCode,
			const AIndex inStartIndex, const AItemCount inLength,
			const ABool inStartSpaceToIndent = false, const AItemCount inStartSpaceCount = 0, const AItemCount inStartIndentWidth = 0 );//#117
	void	MakeTextDrawDataWithoutStyle( const AText& inText );//R0240
	void	AddText( const AText& inUTF8Text, const AText& inUTF16Text, const AColor& inColor );//#532
	
  public:
	//SPI_GetTextDrawDataWithoutStyle()�Őݒ肳��郁���o
	AText	UTF16DrawText;
	AText	UTF8DrawText;
	AArray<AIndex>	OriginalTextArray_UnicodeOffset;//�C���f�b�N�X�F�h�L�������g���ێ����Ă��錳�̃e�L�X�g�̃C���f�b�N�X�@�l�F�`��e�L�X�g��Unicode�����P�ʂł̃C���f�b�N�X
	AArray<AIndex>	UTF16DrawTextArray_OriginalTextIndex;//�C���f�b�N�X�F�`��e�L�X�g��Unicode�����P�ʂł̃C���f�b�N�X�@�l�F�h�L�������g���ێ����Ă��錳�̃e�L�X�g�̃C���f�b�N�X
	AArray<AItemCount>	TabLetters;
	//SPI_GetTextDrawDataWithStyle()�Őݒ肳��郁���o
	AArray<AIndex>	attrPos;
	AArray<AColor>	attrColor;
	AArray<ATextStyle>	attrStyle;
	AArray<AIndex>	controlCodeStartUTF8Index;//#473
	AArray<AIndex>	controlCodeEndUTF8Index;//#473
	ABool	drawSelection;
	AIndex	selectionStart;
	AIndex	selectionEnd;
	AColor	selectionColor;
	AFloatNumber	selectionAlpha;
	//R0199
	AArray<AIndex>	misspellStartArray;
	AArray<AIndex>	misspellEndArray;
	//#117
	AHashArray<AIndex>	startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex;
};

