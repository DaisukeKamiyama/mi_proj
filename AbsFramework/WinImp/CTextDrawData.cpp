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

#include "stdafx.h"

#include "CTextDrawData.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]CTextDrawData
#pragma mark ===========================
//
#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
CTextDrawData::CTextDrawData() 
{
	//B0386 ���s�R�[�h�ʒu�ɕςȐF���\������邱�Ƃ�������C��
	selectionStart = 0;
	//B0000 �s�܂�Ԃ��v�Z������
	Init();
}

CTextDrawData::~CTextDrawData()
{
}

void	CTextDrawData::Init()
{
	UTF16DrawText.DeleteAll();
	UTF8DrawText.DeleteAll();
	OriginalTextArray_UnicodeOffset.DeleteAll();
	UTF16DrawTextArray_OriginalTextIndex.DeleteAll();
	TabLetters.DeleteAll();
	attrPos.DeleteAll();
	attrColor.DeleteAll();
	attrStyle.DeleteAll();
	drawSelection = false;
	selectionStart = kIndex_Invalid;
	selectionEnd = kIndex_Invalid;
	selectionColor = kColor_Blue;
	selectionAlpha = 0.8;
	misspellStartArray.DeleteAll();
	misspellEndArray.DeleteAll();
	startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex.DeleteAll();//#117
	controlCodeStartUTF8Index.DeleteAll();//#473
	controlCodeEndUTF8Index.DeleteAll();//#473
}

#pragma mark ===========================

#pragma mark --- TextDrawData�쐬

//win AView_Edit�쐬�̂��߁AATextInfo����R�s�[���č쐬
/**
�e�L�X�g�`��p�f�[�^����

UTF16���A�^�u�̃X�y�[�X���A���䕶���̉��������s���B
CTextDrawData�N���X�̂����A���L��ݒ肷��B
UTF16DrawText: UTF16�ł̕`��p�e�L�X�g
UTF8DrawText: UTF8�ł̕`��p�e�L�X�g
UnicodeDrawTextIndexArray: �C���f�b�N�X�F�h�L�������g���ێ����Ă��錳�̃e�L�X�g�̃C���f�b�N�X�@�l�F�`��e�L�X�g��Unicode�����P�ʂł̃C���f�b�N�X
*/
void	CTextDrawData::MakeTextDrawDataWithoutStyle( const AText& inText, const ANumber inTabWidth, const AItemCount inLetterCountLimit, 
			const ABool inGetUTF16Text, const ABool inCountAs2Letters, const ABool inDisplayControlCode,
			const AIndex inStartIndex, const AItemCount inLength,
			const ABool inStartSpaceToIndent, const AItemCount inStartSpaceCount, const AItemCount inStartIndentWidth )//#117
{
	//B0000 �s�܂�Ԃ��v�Z������
	Init();
	
	//�s���擾
	AIndex	lineInfo_Start = inStartIndex;
	//B0000 �s�܂�Ԃ��v�Z������
	AItemCount	lineInfo_LengthWithoutCR = inLength;
	
	AItemCount	textcount = inText.GetItemCount();//B0000
	
	//#733
	//��Convert1CharToUCS4�ȊO�̕����̍�����������
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	const AUChar*	textp = textptr.GetPtr();
	const AItemCount	textlen = textptr.GetByteCount();
	
	//inText���Q�Ƃ��āAUnicode�����P�������Ƀ^�u�X�y�[�X���A���䕶���������s���āAUTF8DrawText�֊i�[����
	//�܂��A�����Ɍ��̃e�L�X�g�ƁA�`��pUnicode�e�L�X�g�Ƃ̈ʒu�Ή��֌W��z��Ɋi�[����B
	//UTF16DrawTextArray_OriginalTextIndex: ���̃e�L�X�g�̃C���f�b�N�X(index)���l�ɓ���z��B�Y����Unicode�����̕�����(uniOffset)�B�}�E�X�N���b�N�ʒu����e�L�X�g�ʒu�ւ̕ϊ��Ɏg����B
	//OriginalTextArray_UnicodeOffset: Unicode�����̕�����(uniOffset)���l�ɓ���z��B�Y���͌��̃e�L�X�g�̃C���f�b�N�X(index)
	AIndex	uniOffset = 0;
	AIndex	tabletters = 0;//B0154
	AIndex	tabinc0800 = 1;//B0154
	if( inCountAs2Letters )   tabinc0800 = 2;//B0154
	AIndex	index = 0;
	ABool	gyoutou = true;//#117
	for( ; index < lineInfo_LengthWithoutCR;  )
	{
		if( inLetterCountLimit > 0 )
		{
			if( tabletters >= inLetterCountLimit )   break;
		}
		AUChar	ch = inText.Get(lineInfo_Start+index);
		if( ch != kUChar_Space && ch != kUChar_Tab )   gyoutou = false;//#117
		//#414
		AUCS4Char	ucs4Char = 0;
		inText.Convert1CharToUCS4(textp,textlen,lineInfo_Start+index,ucs4Char);//#733
		//
		AItemCount	bc = inText.GetUTF8ByteCount(ch);
		if( ch == kUChar_LineEnd )
		{
			break;
		}
		else if( ch == kUChar_Tab )
		{
			//�^�u����
			
			//
			OriginalTextArray_UnicodeOffset.Add(uniOffset);
			TabLetters.Add(tabletters);
			
			//Unicode�ꕶ���ǉ�
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			UTF8DrawText.Add(kUChar_Space);
			uniOffset++;
			
			//
			tabletters++;//B0154
			while( (tabletters%inTabWidth) != 0 )//B0154
			{
				//Unicode�ꕶ���ǉ�
				//�^�u�����̑O���^�㔼�ŁA�Ή����錳�̃e�L�X�g�̈ʒu��ς���B
				if( (tabletters%inTabWidth) > inTabWidth/2 )
				{
					UTF16DrawTextArray_OriginalTextIndex.Add(index+1);
				}
				else
				{
					UTF16DrawTextArray_OriginalTextIndex.Add(index);
				}
				UTF8DrawText.Add(kUChar_Space);
				uniOffset++;
				
				//
				tabletters++;//B0154
			}
			
			//
			index++;
		}
		else if( inDisplayControlCode == true && ch <= 0x1F )
		{
			//���䕶����HEX�\������
			
			//
			OriginalTextArray_UnicodeOffset.Add(uniOffset);
			TabLetters.Add(tabletters);
			
			//Unicode�ꕶ���ǉ�
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			UTF8DrawText.Add(kUChar_Space);
			uniOffset++;
			//Unicode�ꕶ���ǉ�
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			UTF8DrawText.Add(hex2asc((ch&0xF0)/0x10));
			uniOffset++;
			//Unicode�ꕶ���ǉ�
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			UTF8DrawText.Add(hex2asc(ch&0xF));
			uniOffset++;
			//Unicode�ꕶ���ǉ�
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			UTF8DrawText.Add(kUChar_Space);
			uniOffset++;
			
			//
			tabletters++;
			
			//#473
			controlCodeStartUTF8Index.Add(index);
			controlCodeEndUTF8Index.Add(index+bc);
			
			//
			index++;
		}
		//#473 SJIS���X���X�t�H�[���o�b�N 2�o�C�g�ڕ����Ȃ��G���[��ԕ\������
		else if( ucs4Char >= 0x105C00 && ucs4Char <= 0x105CFF )
		{
			//�G���[�����i1�o�C�g�ځj�擾
			AUChar	sjisch = (ucs4Char&0xFF);
			
			//�I���W�i���e�L�X�g�ɑΉ�����z��̂ق��Ƀf�[�^�ݒ�
			for( AIndex i = 0; i < bc; i++ )
			{
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				TabLetters.Add(tabletters);
			}
			
			//�G���[�e�L�X�g�擾
			AText	t;
			t.SetLocalizedText("SJIS_Error_2ndByte");
			AText	text;
			text.Add(' ');
			text.Add(hex2asc((sjisch&0xF0)/0x10));
			text.Add(hex2asc(sjisch&0xF));
			text.Add(' ');
			text.AddText(t);
			//�G���[�e�L�X�g��ݒ�
			for( AIndex i = 0; i < text.GetItemCount(); i++ )
			{
				AUChar	ch = text.Get(i);
				UTF8DrawText.Add(ch);
				if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )
				{
					//Unicode�ꕶ���ǉ�
					UTF16DrawTextArray_OriginalTextIndex.Add(index);
					uniOffset++;
				}
			}
			
			//�^�u
			tabletters++;
			
			//���䕶���͈͐ݒ�
			controlCodeStartUTF8Index.Add(index);
			controlCodeEndUTF8Index.Add(index+bc);
			
			//�����C���f�b�N�X�X�V
			index += bc;
		}
		else
		{
			//#117
			if( gyoutou == true && inStartSpaceToIndent == true && inStartSpaceCount > 0 && inStartIndentWidth > 0 &&
						index+inStartSpaceCount <= lineInfo_LengthWithoutCR )
			{
				//���݂�index�ʒu����inStartSpaceCount��space���A�����Ă��邩�ǂ������`�F�b�N����
				ABool	nspace = true;
				for( AIndex i = index ; i < index+inStartSpaceCount; i++ )
				{
					AUChar	ch = inText.Get(lineInfo_Start+i);
					if( ch != kUChar_Space )
					{
						nspace = false;
						break;
					}
				}
				//n��space�����݂���Ȃ�΁AinStartIndentWidth��space�ɒu�������Ēǉ�
				if( nspace == true )
				{
					startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex.Add(index);
					//
					for( AIndex i = 0; i < inStartSpaceCount; i++ )
					{
						//�����L�[�ɂ��L�����b�g�ړ��Ŕ��p�X�y�[�X���L�����b�g�ʒu������悤�ɂ���
						AIndex	uo = uniOffset + i;
						if( uo >= uniOffset+inStartIndentWidth )   uo = uniOffset+inStartIndentWidth-1;
						OriginalTextArray_UnicodeOffset.Add(uo);
						TabLetters.Add(tabletters);
						tabletters++;
					}
					//Unicode n�����ǉ�
					for( AIndex i = 0; i < inStartIndentWidth; i++ )
					{
						//�N���b�N���A�������Ȃ�^���^�u�O�A�E�����Ȃ�^���^�u��ɂȂ�悤�ɂ���
						if( i > inStartIndentWidth/2 )
						{
							UTF16DrawTextArray_OriginalTextIndex.Add(index+inStartSpaceCount);
						}
						else
						{
							UTF16DrawTextArray_OriginalTextIndex.Add(index);
						}
						UTF8DrawText.Add(kUChar_Space);
						uniOffset++;
					}
					//index�ړ�
					index += inStartSpaceCount;
					continue;
				}
			}
			
			//����ȊO�̕��ʂ̕���
			
			//B0000���Ƃ���"�"�̂݋L�q���ꂽ�t�@�C����ǂݍ��݁A�G���R�[�f�B���O����F�������ꍇ���Abc�𑫂��ƃe�L�X�g�S�̂��I�[�o�[����\��������
			if( lineInfo_Start+index+bc > textcount )
			{
				ch = 0x20;
				bc = 1;
			}
			
			//
			for( AIndex i = 0; i < bc; i++ )
			{
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				TabLetters.Add(tabletters);
			}
			
			//Unicode�ꕶ���ǉ�
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
			//#412
			if( ucs4Char >= 0x10000 )
			{
				UTF16DrawTextArray_OriginalTextIndex.Add(index);
				uniOffset++;
			}
			UTF8DrawText.Add(ch);
			index++;
			for( AIndex i = 1; i < bc; i++ )
			{
				UTF8DrawText.Add(inText.Get(lineInfo_Start+index));
				index++;
			}
			uniOffset++;
			//B0000 MacOSX10.4�ł�U+FFFF(EF BF BF)���g���Ƃ��������Ȃ�H�̂ŁAU+FFFF��U+3000(�S�p�X�y�[�X)�ɂ���
			if( ch == 0xEF )
			{
				if(	UTF8DrawText.Get(UTF8DrawText.GetItemCount()-2) == 0xBF &&
					UTF8DrawText.Get(UTF8DrawText.GetItemCount()-1) == 0xBF )
				{
					UTF8DrawText.Set(UTF8DrawText.GetItemCount()-3,0xE3);
					UTF8DrawText.Set(UTF8DrawText.GetItemCount()-2,0x80);
					UTF8DrawText.Set(UTF8DrawText.GetItemCount()-1,0x80);
				}
			}
			
			//
			//#414 if( bc <= 2 )
			if( AApplication::GetApplication().NVI_GetUnicodeData().IsWideWidth(ucs4Char) == false )//#414
			{
				tabletters++;
			}
			else
			{
				tabletters += tabinc0800;
			}
		}
	}
	UTF16DrawTextArray_OriginalTextIndex.Add(index);
	OriginalTextArray_UnicodeOffset.Add(uniOffset);
	TabLetters.Add(tabletters);
	
	if( inGetUTF16Text == true )
	{
		//UTF8DrawText����UTF16DrawText�֕ϊ�
		UTF16DrawText.SetText(UTF8DrawText);
		UTF16DrawText.ConvertFromUTF8ToUTF16();
		//
		while( UTF16DrawTextArray_OriginalTextIndex.GetItemCount() <= UTF16DrawText.GetItemCount()/2 )
		{
			UTF16DrawTextArray_OriginalTextIndex.Add(index);
		}
	}
}

//R0240
void	CTextDrawData::MakeTextDrawDataWithoutStyle( const AText& inText )
{
	MakeTextDrawDataWithoutStyle(inText,4,0,true,true,false,0,inText.GetItemCount());
}

//#532
/**
�ǉ�
*/
void	CTextDrawData::AddText( const AText& inUTF8Text, const AText& inUTF16Text, const AColor& inColor )
{
	//�Ō�̃f�[�^���擾
	AIndex	index = UTF16DrawTextArray_OriginalTextIndex.Get(UTF16DrawTextArray_OriginalTextIndex.GetItemCount()-1);
	AIndex	uniOffset = OriginalTextArray_UnicodeOffset.Get(OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	AItemCount	tabletters = TabLetters.Get(TabLetters.GetItemCount()-1);
	
	//�Ō�̂P������
	UTF16DrawTextArray_OriginalTextIndex.Delete1(UTF16DrawTextArray_OriginalTextIndex.GetItemCount()-1);
	OriginalTextArray_UnicodeOffset.Delete1(OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	TabLetters.Delete1(TabLetters.GetItemCount()-1);
	
	//�F�ݒ�
	attrPos.Add(uniOffset);
	attrColor.Add(inColor);
	attrStyle.Add(kTextStyle_Normal);
	
	//UTF8�e�L�X�g�ݒ�
	UTF8DrawText.AddText(inUTF8Text);
	//UTF16�e�L�X�g�ݒ�
	UTF16DrawText.AddText(inUTF16Text);
	
	//UTF8-UTF16 �C���f�b�N�X�ϊ��z��ݒ�
	for( AIndex pos = 0; pos < inUTF8Text.GetItemCount(); )
	{
		AItemCount	bc = inUTF8Text.GetUTF8ByteCount(pos);
		for( AIndex i = 0; i < bc; i++ )
		{
			OriginalTextArray_UnicodeOffset.Add(uniOffset);
			TabLetters.Add(tabletters);
		}
		pos+= bc;
		uniOffset++;
		UTF16DrawTextArray_OriginalTextIndex.Add(index);
		index++;
		tabletters++;
	}
	OriginalTextArray_UnicodeOffset.Add(uniOffset);
	UTF16DrawTextArray_OriginalTextIndex.Add(index);
	TabLetters.Add(tabletters);
}

