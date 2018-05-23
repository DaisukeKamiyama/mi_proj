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

AUnicodeData


*/

#include "stdafx.h"

#include "AUnicodeData.h"

#pragma mark ===========================
#pragma mark [�N���X]AUnicodeLinkData
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
AUnicodeLinkData::AUnicodeLinkData()
{
	for( short i = 0; i < kUnicodeLinkDataDecompositionSize; i++ )
	{
		mDecomposition[i] = 0;
	}
	mUpper = 0;
	mLower =0;
	mLink = NULL;
}

AUnicodeLinkData::~AUnicodeLinkData()
{
	if( mLink != NULL )
	{
		delete mLink;
	}
}

AUCS4Char	AUnicodeLinkData::GetCompatibilityDecomposition( const AUInt8 inNumber )
{
	if( inNumber >= kUnicodeLinkDataDecompositionSize )
	{
		if( mLink != NULL )
		{
			return mLink->GetCompatibilityDecomposition(inNumber-kUnicodeLinkDataDecompositionSize);
		}
		else
		{
			fprintf(stderr,"UnicodeData error.\n");
			return 0;
		}
	}
	else
	{
		return mDecomposition[inNumber];
	}
}

void AUnicodeLinkData::SetCompatibilityDecomposition( const AUInt8 inNumber, const AUCS4Char inChar )
{
	if( inNumber >= kUnicodeLinkDataDecompositionSize )
	{
		if( mLink == NULL )
		{
			mLink = new AUnicodeLinkData();
		}
		mLink->SetCompatibilityDecomposition(inNumber-kUnicodeLinkDataDecompositionSize,inChar);
	}
	else
	{
		mDecomposition[inNumber] = inChar;
	}
}

#pragma mark ===========================
#pragma mark [�N���X]AUnicodeData
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
AUnicodeData::AUnicodeData():mUnicodeData(NULL)
{
	//#852 mUnicodeData = (AUnicodeCharData*)AMemoryWrapper::Malloc(sizeof(AUnicodeCharData)*kUnicodeDataSize);//#407
}

//#852
/**
�f�[�^�t�@�C���ݒ�i�A�v���P�[�V�����N������AApplication����R�[�������j
*/
void	AUnicodeData::Init( const AFileAcc& inUnicodeDataFile, const AFileAcc& inEastAsianWidthFile )
{
	mUnicodeDataFile.CopyFrom(inUnicodeDataFile);
	mEastAsianWidthFile.CopyFrom(inEastAsianWidthFile);
	
	//#852
	//�������m��
	mUnicodeData = (AUnicodeCharData*)AMemoryWrapper::Malloc(sizeof(AUnicodeCharData)*kUnicodeDataSize);//#407
	
	//�T�C�Y�`�F�b�N
	AByteCount	bytecount = AMemoryWrapper::AllocatedSize(mUnicodeData);
	//#0 AByteCount	bytecount0 = sizeof(AUnicodeCharData);
	if( bytecount < sizeof(AUnicodeCharData)*kUnicodeDataSize )
	{
		_ACError("UnicodeData size Error",NULL);
	}
	//
	for( AUCS4Char i = 0; i < kUnicodeDataSize; i++ )
	{
		mUnicodeData[i].combiningClass = 0;
		mUnicodeData[i].decompositionLen = 0;
		mUnicodeData[i].linkData = NULL;
		mUnicodeData[i].category = kUnicodeCharCategory_Others;//B0360
		mUnicodeData[i].flags = 0;//#407
	}
	ReadUnicodeDataTxt(mUnicodeDataFile);
	ReadEastAsianWidthTxt(mEastAsianWidthFile);//#415
	//0-9 #416
	for( AUCS4Char i = 0x0030; i <= 0x0039; i++ )
	{
		mUnicodeData[i].flags |= kFlags_Number;
	}
	//B0360
	//���e���A����
	//0-9
	for( AUCS4Char i = 0x0030; i <= 0x0039; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	//A-Z
	for( AUCS4Char i = 0x0041; i <= 0x005A; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	//a-z
	for( AUCS4Char i = 0x0061; i <= 0x007A; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	//���e�������g��
	for( AUCS4Char i = 0x00C0; i <= 0x02AF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	//�E���������L���g���A���e�������g��
	for( AUCS4Char i = 0x1D00; i <= 0x1EFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	//�A���t�@�x�b�g�\���` 
	for( AUCS4Char i = 0xFB00; i <= 0xFB4F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_LatinAndNumber;
	}
	
	
	//�e��S�p�A���p 
	//�O-�X 
	for( AUCS4Char i = 0xFF10; i <= 0xFF19; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuLatinAndNumber;
	}
	//�`-�y 
	for( AUCS4Char i = 0xFF21; i <= 0xFF3A; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuLatinAndNumber;
	}
	//��-�� 
	for( AUCS4Char i = 0xFF41; i <= 0xFF5A; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuLatinAndNumber;
	}
	// 
	for( AUCS4Char i = 0x3041; i <= 0x309F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuHiragana;
	}
	// 
	for( AUCS4Char i = 0x30A1; i <= 0x30FA; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuKatakana;
	}
	// 
	for( AUCS4Char i = 0x31F0; i <= 0x31FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_ZenkakuKatakana;
	}
	//�[ 
	mUnicodeData[0x30FC].category = kUnicodeCharCategory_ZenkakuOtohiki;
	//�-� 
	for( AUCS4Char i = 0xFF66; i <= 0xFF9F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_HankakuKatakana;
	}
	
	//���� 
	//�X 
	mUnicodeData[0x3005].category = kUnicodeCharCategory_Kanji;
	//CJK���������g��A
	for( AUCS4Char i = 0x3400; i <= 0x4DBF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Kanji;
	}
	//CJK�������� 
	for( AUCS4Char i = 0x4E00; i <= 0x9FFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Kanji;
	}
	//CJK�݊����� 
	for( AUCS4Char i = 0xF900; i <= 0xFAFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Kanji;
	}
	//CJK���������g��B, CJK�݊������ǉ� 
	for( AUCS4Char i = 0x20000; i <= 0x2FFFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Kanji;
	}
	
	//�e������ 
	//�M���V������ 
	for( AUCS4Char i = 0x0370; i <= 0x03FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Greek;
	}
	for( AUCS4Char i = 0x1F00; i <= 0x1FFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Greek;
	}
	//�L�������� 
	for( AUCS4Char i = 0x0400; i <= 0x052F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Cyrillic;
	}
	//�A�����j�A���� 
	for( AUCS4Char i = 0x0530; i <= 0x058F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Armenian;
	}
	//�w�u���C���� 
	for( AUCS4Char i = 0x0590; i <= 0x05FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Hebrew;
	}
	//�A���r�A���� 
	for( AUCS4Char i = 0x0600; i <= 0x06FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Arabic;
	}
	for( AUCS4Char i = 0x0750; i <= 0x077F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Arabic;
	}
	//�A���r�A�����\���`A
	for( AUCS4Char i = 0xFB50; i <= 0xFDFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Arabic;
	}
	//�A���r�A�����\���`B
	for( AUCS4Char i = 0xFE70; i <= 0xFEFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Arabic;
	}
	//�V���A���� 
	for( AUCS4Char i = 0x0700; i <= 0x074F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Syriac;
	}
	//�^�[�i���� 
	for( AUCS4Char i = 0x0780; i <= 0x07FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Thaana;
	}
	//�f�[�o�i�[�K���[���� 
	for( AUCS4Char i = 0x0900; i <= 0x097F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Devanagari;
	}
	//�x���K������ 
	for( AUCS4Char i = 0x0980; i <= 0x09FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Bengali;
	}
	//�O�����L�[���� 
	for( AUCS4Char i = 0x0A00; i <= 0x0A7F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Gurmukhi;
	}
	//�O�W�����[�g���� 
	for( AUCS4Char i = 0x0A80; i <= 0x0AFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Gujarati;
	}
	//�I�����[���� 
	for( AUCS4Char i = 0x0B00; i <= 0x0B7F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Oriya;
	}
	//�^�~�[������ 
	for( AUCS4Char i = 0x0B80; i <= 0x0BFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Tamil;
	}
	//�e���O���� 
	for( AUCS4Char i = 0x0C00; i <= 0x0C7F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Telugu;
	}
	//�J���i�_���� 
	for( AUCS4Char i = 0x0C80; i <= 0x0CFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Kannada;
	}
	//�}�����[�������� 
	for( AUCS4Char i = 0x0D00; i <= 0x0D7F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Malayalam;
	}
	//�V���n������ 
	for( AUCS4Char i = 0x0D80; i <= 0x0DFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Sinhala;
	}
	//�^�C���� 
	for( AUCS4Char i = 0x0E00; i <= 0x0E7F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Thai;
	}
	//���I���� 
	for( AUCS4Char i = 0x0E80; i <= 0x0EFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Lao;
	}
	//�`�x�b�g���� 
	for( AUCS4Char i = 0x0F00; i <= 0x0FFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Tibetan;
	}
	//�~�����}�[���� 
	for( AUCS4Char i = 0x1000; i <= 0x109F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Myanmar;
	}
	//�O���W�A���� 
	for( AUCS4Char i = 0x10A0; i <= 0x10FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Georgian;
	}
	//�n���O������ 
	for( AUCS4Char i = 0x1100; i <= 0x11FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Hangul;
	}
	for( AUCS4Char i = 0x3130; i <= 0x318F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Hangul;
	}
	for( AUCS4Char i = 0xAC00; i <= 0xD7FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Hangul;
	}
	//�G�`�I�s�A���� 
	for( AUCS4Char i = 0x1200; i <= 0x139F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Ethiopic;
	}
	//�`�F���L�[���� 
	for( AUCS4Char i = 0x13A0; i <= 0x13FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Cherokee;
	}
	//�J�i�_��Z������  
	for( AUCS4Char i = 0x1400; i <= 0x167F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_CanadianSyllabics;
	}
	//�I�K������ 
	for( AUCS4Char i = 0x1680; i <= 0x169F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Ogham;
	}
	//���[������ 
	for( AUCS4Char i = 0x16A0; i <= 0x16FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Runic;
	}
	//�^�K���O���� 
	for( AUCS4Char i = 0x1700; i <= 0x171F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Tagalog;
	}
	//�n�k�m�I���� 
	for( AUCS4Char i = 0x1720; i <= 0x173F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Hanunoo;
	}
	//�u�q�b�h���� 
	for( AUCS4Char i = 0x1740; i <= 0x175F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Buhid;
	}
	//�^�O�o�k������ 
	for( AUCS4Char i = 0x1760; i <= 0x177F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Tagbanwa;
	}
	//�N���[������ 
	for( AUCS4Char i = 0x1780; i <= 0x17FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Khmer;
	}
	//�����S������ 
	for( AUCS4Char i = 0x1800; i <= 0x18FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Mongolian;
	}
	//�����u�[���� 
	for( AUCS4Char i = 0x1900; i <= 0x194F; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Limbu;
	}
	//�^�C�i���� 
	for( AUCS4Char i = 0x1950; i <= 0x19DF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_TaiLe;
	}
	//�_�� 
	for( AUCS4Char i = 0x2801; i <= 0x28FF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_BraillePattern;
	}
	//�C���� 
	for( AUCS4Char i = 0xA000; i <= 0xABFF; i++ )
	{
		mUnicodeData[i].category = kUnicodeCharCategory_Yi;
	}
}

AUnicodeData::~AUnicodeData()
{
	//�������ς݁i���������m�ۍς݁j�Ȃ烁�������
	if( mUnicodeData != NULL )
	{
		for( AUCS4Char i = 0; i < kUnicodeDataSize; i++ )
		{
			if( mUnicodeData[i].linkData != NULL )
			{
				delete mUnicodeData[i].linkData;
			}
		}
		//#407
		AMemoryWrapper::Free(mUnicodeData);
	}
}

//B0360
AUnicodeCharCategory	AUnicodeData::GetUnicodeCharCategory( const AUCS4Char inChar ) 
{
	if( inChar < kUnicodeDataSize )
	{
		return mUnicodeData[inChar].category;
	}
	else
	{
		return kUnicodeCharCategory_Others;
	}
}

/*#166
//
void	AUnicodeData::Cat( AArray<AUCS4Char>& ioText, const AUCS4Char inAddChar, const ABool inLower, const ABool inYenToBackslash ) const//#412
{
	AUCS4Char	addChar = inAddChar;
	if( addChar < kUnicodeDataSize )
	{
		if( inLower )
		{
			if( mUnicodeData[addChar].linkData != NULL )
			{
				if( mUnicodeData[addChar].linkData->mLower != 0 )
				{
					addChar = mUnicodeData[addChar].linkData->mLower;
				}
			}
		}
		if( inYenToBackslash )
		{
			if( addChar == 0x00A5 )
			{
				addChar = 0x005C;
			}
		}
	}
	ioText.Add(addChar);
}
*/

//ioText�ɁAinAddChar�ɑ΂���NFKD�e�L�X�g���ǉ�����B 
//#166 void	AUnicodeData::CatNFKD( AArray<AUCS4Char>& ioText, const AUCS4Char inAddChar, const ABool inLower, const ABool inYenToBackslash ) const//#412

//#166
/**
���K�����Ēǉ�
@return ���ʃo�b�t�@�iioTextPtr�j�I�[�o�[�̏ꍇ��false
*/
ABool	AUnicodeData::CatNormalizedText( 
										AUCS4Char*	ioTextPtr, AIndex& ioTextLen, const AItemCount inTextMaxLen,
										const AUCS4Char inAddChar, 
										const AUnicodeNormalizeType inNormalizeType, //#427
										const ABool inLower, const ABool inHiragana, const ABool inYenToBackslash ) 
{
	AHashArray<AUCS4Char>	from;
	AObjectArray< AArray<AUCS4Char> >	to;
	return CatNormalizedText(ioTextPtr,ioTextLen,inTextMaxLen,inAddChar,
		inNormalizeType,inLower,inHiragana,inYenToBackslash,false,from,to);
}
ABool	AUnicodeData::CatNormalizedText( 
										AUCS4Char*	ioTextPtr, AIndex& ioTextLen, const AItemCount inTextMaxLen,
										const AUCS4Char inAddChar, 
										const AUnicodeNormalizeType inNormalizeType, //#427
										const ABool inLower, const ABool inHiragana, const ABool inYenToBackslash,
										const ABool inUseNormalizeArray,
										const AHashArray<AUCS4Char>& inNormalizeArray_from, 
										const AObjectArray< AArray<AUCS4Char> >& inNormalizeArray_to ) 
{
	//�z��f�[�^�ɂ�鐳�K��
	if( inUseNormalizeArray == true )
	{
		//�܂����ׂĂЂ炪�Ȃɂ���i���K���t�@�C���ɂ͂Ђ炪�Ȃ̕������L�q����j
		AUCS4Char	fromch = inAddChar;
		if( fromch >= 0x30A1 && fromch <= 0x30F6 )
		{
			fromch -= (0x30A0-0x3040);
		}
		//�z��f�[�^�ɂ�鐳�K�����s�i���K���f�[�^�ɑ΂��Ă��ēx�ċA�I�ɐ��K�����s����B���̐��K�������s�����邽�߁B�j
		AIndex	normalizeArrayIndex = inNormalizeArray_from.Find(fromch);
		if( normalizeArrayIndex != kIndex_Invalid )
		{
			const AArray<AUCS4Char>&	addText = inNormalizeArray_to.GetObjectConst(normalizeArrayIndex);
			for( AIndex i = 0; i < addText.GetItemCount(); i++ )
			{
				if( CatNormalizedText(ioTextPtr,ioTextLen,inTextMaxLen,addText.Get(i),
									  inNormalizeType,inLower,inHiragana,inYenToBackslash,inUseNormalizeArray,//#427
									  inNormalizeArray_from,inNormalizeArray_to) == false )
				{
					return false;
				}
			}
			return true;
		}
	}
	//
	if( inAddChar < kUnicodeDataSize )
	{
		//
		ABool	needDecomp = false;
		if( inNormalizeType == kUnicodeNormalizeType_KD && mUnicodeData[inAddChar].decompositionLen > 0 )
		{
			needDecomp = true;
		}
		if( inNormalizeType == kUnicodeNormalizeType_D && mUnicodeData[inAddChar].decompositionLen > 0 &&
					((mUnicodeData[inAddChar].flags&kFlags_HasCompatible) == 0 ))
		{
			needDecomp = true;
		}
		//
		if( /*#427 inNFKD == true && mUnicodeData[inAddChar].decompositionLen > 0*/ needDecomp == true )//#166
		{
			//compatible/decomp�֕ϊ��idecomp:�������{���_�j
			for( AUInt8 i = 0; i < mUnicodeData[inAddChar].decompositionLen; i++ )
			{
				//#166 CatNFKD(ioText,mUnicodeData[inAddChar].linkData->GetCompatibilityDecomposition(i),inLower,inYenToBackslash);
				if( CatNormalizedText(ioTextPtr,ioTextLen,inTextMaxLen,
									  mUnicodeData[inAddChar].linkData->GetCompatibilityDecomposition(i),
									  inNormalizeType,inLower,inHiragana,inYenToBackslash,inUseNormalizeArray,//#427
									  inNormalizeArray_from,inNormalizeArray_to) == false )
				{
					return false;
				}
			}
		}
		else
		{
			AUCS4Char	addChar = inAddChar;
			//�������֐��K��
			if( inLower )
			{
				if( mUnicodeData[addChar].linkData != NULL )
				{
					if( mUnicodeData[addChar].linkData->mLower != 0 )
					{
						addChar = mUnicodeData[addChar].linkData->mLower;
					}
				}
			}
			//�Ђ炪�Ȃ֐��K�� #166
			if( inHiragana == true )
			{
				if( addChar >= 0x30A1 && addChar <= 0x30F6 )
				{
					addChar -= (0x30A0-0x3040);
				}
			}
			//���p�~�L������o�b�N�X���b�V���֐��K��
			if( inYenToBackslash )
			{
				if( addChar == 0x00A5 )
				{
					addChar = 0x005C;
				}
			}
			//���K���㕶���ǉ�
			//#733 ioText.Add(addChar);
			if( ioTextLen >= inTextMaxLen )
			{
				//�o�b�t�@�I�[�o�[
				//_ACThrow("",NULL);
				return false;
			}
			else
			{
				ioTextPtr[ioTextLen] = addChar;
				ioTextLen++;
			}
			
			//reorder algorithm
			if( /*#427 inNFKD == true*/ inNormalizeType == kUnicodeNormalizeType_KD || inNormalizeType == kUnicodeNormalizeType_D )//#166
			{
				AUInt8	addCharClass = mUnicodeData[addChar].combiningClass;
				if( addCharClass > 0 )
				{
					for( AIndex i = /*#733 ioText.GetItemCount()-1*/ioTextLen-1; i > 0; i-- )
					{
						AUInt8	prevCharClass = 0;//#407
						if( /*#733 ioText.Get(i-1)*/ioTextPtr[i-1] < kUnicodeDataSize )//#407
						{
							prevCharClass = mUnicodeData[/*#733 ioText.Get(i-1)*/ioTextPtr[i-1]].combiningClass;
						}
						if( prevCharClass <= addCharClass )
						{
							break;
						}
						//#733 ioText.Set(i,ioText.Get(i-1));
						ioTextPtr[i] = ioTextPtr[i-1];//#733
						//#733 ioText.Set(i-1,addChar);
						ioTextPtr[i-1] = addChar;//#733
					}
				}
			}
		}
	}
	else
	{
		//�e�[�u���O��Unicode�����̏ꍇ�͕ϊ������ɒP���ɒǉ�
		//#733 ioText.Add(inAddChar);
		if( ioTextLen >= inTextMaxLen )
		{
			//�o�b�t�@�I�[�o�[
			//_ACThrow("",NULL);
			return false;
		}
		else
		{
			ioTextPtr[ioTextLen] = inAddChar;
			ioTextLen++;
		}
	}
	return true;
}

void	AUnicodeData::ReadUnicodeDataTxt( const AFileAcc& inFile )
{
	try
	{
		AText	text;
		inFile.ReadTo(text);
		//
		AStTextPtr	textptr(text,0,text.GetItemCount());
		const AUChar*	ptr = textptr.GetPtr();
		const AItemCount	textlen = textptr.GetByteCount();
		//
		AIndex	pos = 0;
		while( pos < textlen )
		{
			AUCS4Char	unicode = 0;
			AText	str;
			AIndex	p = pos;
			
			//�s�̏I���ʒu�����ip�̘A���ł��ǂ����A�O�̂��߁A�ʒu�␳����B�j 
			while( pos < textlen )
			{
				unsigned char	ch = ptr[pos];
				pos++;
				if( ch == '\r' || ch == '\n' )
				{
					if( pos < textlen )
					{
						ch = ptr[pos];
						if( ch == '\r' || ch == '\n' )
						{
							pos++;
						}
					}
					break;
				}
			}
			
			//unicode�擾 
			AText::GetToken(ptr,textlen,p,str);
			unicode = str.GetIntegerFromHexText();
			if( unicode >= kUnicodeDataSize )
			{
				continue;
			}
			p++;
			
			//Name�ǂݔ�΂� 
			while( ptr[p] != ';' )   p++;
			p++;
			//General_Category
			AText	t;
			while( ptr[p] != ';' )
			{
				t.Add(ptr[p]);
				p++;
			}
			//�A���t�@�x�b�g����
			if( t.Compare("Lu") == true || t.Compare("Ll") == true || t.Compare("Lt") == true )
			{
				//http://www.unicode.org/reports/tr44/#General_Category_Values
				//Lu: an uppercase letter
				//Ll: a lowercase letter
				//Lt: a digraphic character, with first part uppercase
				//Lm: a modifier letter �i�����L�����͂����j
				//Lo: other letters, including syllables and ideographs �i�����E�Ђ炪�ȓ��͂����j
				mUnicodeData[unicode].flags |= kFlags_Alphabet;
			}
			p++;
			//Canonical_Combining_Class
			AText::GetToken(ptr,textlen,p,str);
			mUnicodeData[unicode].combiningClass = str.GetIntegerFromHexText();
			p++;
			//Bidi_Class�ǂݔ�΂� 
			while( ptr[p] != ';' )   p++;
			p++;
			//------------------Decomposition_Mapping------------------
			//Compatible Decomp���ǂ����̔���icompatible�̏ꍇ�A�ŏ���<>�ň͂܂ꂽ����������B�i<fraction>���j�j
			if( ptr[p] == '<' )
			{
				while( ptr[p] != '>' )   p++;
				p++;
				mUnicodeData[unicode].flags |= kFlags_HasCompatible;//#427
			}
			//decomp�������Ƃ̃��[�v�idecomp�������A�Ȃ��ċL�q����Ă���̂ł����ǂݍ��ށj
			AItemCount	decompositionLen = 0;
			AUCS4Char	firstDecompChar = 0;//#863 �ŏ���decomp�������L�� canonical decomp��comp�ϊ��p�̃f�[�^�Ɏg�p
			AUCS4Char	lastDecompChar = 0;//#863 �Ō��decomp�������L�� canonical decomp��comp�ϊ��p�̃f�[�^�Ɏg�p
			for( ; decompositionLen < 256; decompositionLen++)
			{
				if( ptr[p] == ';' )    break;
				AText::GetToken(ptr,textlen,p,str);
				if( mUnicodeData[unicode].linkData == NULL )
				{
					mUnicodeData[unicode].linkData = new AUnicodeLinkData();
				}
				//decomp�������擾 #863
				lastDecompChar = str.GetIntegerFromHexText();
				if( decompositionLen == 0 )
				{
					//�ŏ���decomp�������L��
					firstDecompChar = lastDecompChar;
				}
				//link data��decomp��ǉ�
				mUnicodeData[unicode].linkData->SetCompatibilityDecomposition(decompositionLen,lastDecompChar);
			}
			//decomp�����O�X�ݒ�
			mUnicodeData[unicode].decompositionLen = decompositionLen;
			//------------------canonical decomp��comp�ϊ��p�̃f�[�^���쐬����------------------
			if( (mUnicodeData[unicode].flags&kFlags_HasCompatible) == 0 && 
						decompositionLen >= 2 &&
						lastDecompChar < kUnicodeDataSize )
			{
				//canonical comp�̏ꍇ�A�Ō��decomp������kFlags_CanonicalDecomp2ndChar�t���O��ON�ɂ���
				mUnicodeData[lastDecompChar].flags |= kFlags_CanonicalDecomp2ndChar;
				//mCanonicalCompArray_Decomp2ndChar, mCanonicalCompArray_Decomp1stCharArray, mCanonicalCompArray_CompCharArray��
				//�Ō��decomp�������ɁA{decomp�ꕶ����,comp����}�̃��X�g���i�[����
				AIndex	index = mCanonicalCompArray_Decomp2ndChar.Find(lastDecompChar);
				if( index == kIndex_Invalid )
				{
					index = mCanonicalCompArray_Decomp2ndChar.Add(lastDecompChar);
					mCanonicalCompArray_Decomp1stCharArray.AddNewObject();
					mCanonicalCompArray_CompCharArray.AddNewObject();
				}
				mCanonicalCompArray_Decomp1stCharArray.GetObject(index).Add(firstDecompChar);
				mCanonicalCompArray_CompCharArray.GetObject(index).Add(unicode);
			}
			//decomposition��256�ȏ゠��ꍇ�̓G���[�i�K�i��A18�������Œ��j
			if( ptr[p] != ';' )
			{
				mUnicodeData[unicode].decompositionLen = 0;
				if( mUnicodeData[unicode].linkData != NULL )
				{
					delete mUnicodeData[unicode].linkData;
					mUnicodeData[unicode].linkData = NULL;
				}
				fprintf(stderr,"The length of the compatibility decomposition is more than %d characters for U+%06X.\n",256,unicode);
			}
			p++;
			//Numeric_Value�ǂݔ�΂� 
			while( ptr[p] != ';' )   p++;
			p++;
			//Numeric_Value�ǂݔ�΂� 
			while( ptr[p] != ';' )   p++;
			p++;
			//Numeric_Value�ǂݔ�΂� 
			while( ptr[p] != ';' )   p++;
			p++;
			//Bidi_Mirrored�ǂݔ�΂� 
			while( ptr[p] != ';' )   p++;
			p++;
			//Unicode_1_Name�ǂݔ�΂� 
			while( ptr[p] != ';' )   p++;
			p++;
			//ISO_Comment�ǂݔ�΂� 
			while( ptr[p] != ';' )   p++;
			p++;
			//Simple_Uppercase_Mapping
			if( ptr[p] != ';' )
			{
				AText::GetToken(ptr,textlen,p,str);
				if( mUnicodeData[unicode].linkData == NULL )
				{
					mUnicodeData[unicode].linkData = new AUnicodeLinkData();
				}
				mUnicodeData[unicode].linkData->mUpper = str.GetIntegerFromHexText();
			}
			p++;
			//Simple_Lowercase_Mapping
			if( ptr[p] != ';' )
			{
				AText::GetToken(ptr,textlen,p,str);
				if( mUnicodeData[unicode].linkData == NULL )
				{
					mUnicodeData[unicode].linkData = new AUnicodeLinkData();
				}
				mUnicodeData[unicode].linkData->mLower = str.GetIntegerFromHexText();
			}
			p++;
		}
	}
	catch(...)
	{
	}
}

//#415
void	AUnicodeData::ReadEastAsianWidthTxt( const AFileAcc& inFile )
{
	try
	{
		AText	text;
		inFile.ReadTo(text);
		//
		AStTextPtr	textptr(text,0,text.GetItemCount());
		const AUChar*	ptr = textptr.GetPtr();
		const AItemCount	textlen = textptr.GetByteCount();
		//
		AIndex	pos = 0;
		while( pos < textlen )
		{
			AUCS4Char	unicode_s = 0, unicode_e = 0;//#430
			AText	str;
			AIndex	p = pos;
			
			//�s�̏I���ʒu����
			while( pos < textlen )
			{
				unsigned char	ch = ptr[pos];
				pos++;
				if( ch == '\r' || ch == '\n' )
				{
					if( pos < textlen )
					{
						ch = ptr[pos];
						if( ch == '\r' || ch == '\n' )
						{
							pos++;
						}
					}
					break;
				}
			}
			
			//unicode�擾 
			AText::GetToken(ptr,textlen,p,str);
			if( str.Compare("#") == true )   continue;
			unicode_s = str.GetIntegerFromHexText();
			if( unicode_s >= kUnicodeDataSize )
			{
				continue;
			}
			//#430 p++;
			//#430 �͈͎w��Ή�
			AText::GetToken(ptr,textlen,p,str);
			if( str.Compare(".") == true )
			{
				AText::GetToken(ptr,textlen,p,str);//�����P��"."�ǂݍ���
				AText::GetToken(ptr,textlen,p,str);
				unicode_e = str.GetIntegerFromHexText();
				if( unicode_e >= kUnicodeDataSize )
				{
					unicode_e = kUnicodeDataSize-1;
				}
				AText::GetToken(ptr,textlen,p,str);//";"
			}
			else
			{
				unicode_e = unicode_s;
			}
			
			//F/H/W/Na/A�ǂݍ���
			AText::GetToken(ptr,textlen,p,str);
			if( str.GetItemCount() == 1 )
			{
				AUChar	ch = str.Get(0);
				if( ch == 'F' || ch == 'W' || ch == 'A' )//#561
				{
					for( AUCS4Char uni = unicode_s; uni <= unicode_e; uni++ )//#430
					{
						mUnicodeData[uni].flags |= kFlags_WideWidth;
					}
				}
			}
		}
	}
	catch(...)
	{
	}
}

//#863
/**
decomp��������comp�������擾�icanonical comp�̂ݑΉ��j
*/
AUCS4Char	AUnicodeData::GetCanonicalCompChar( const AUCS4Char in2ndChar, const AUCS4Char in1stChar ) const
{
	//2�Ԗ�decoomp��������A�e�[�u������
	AIndex	comparrayindex = mCanonicalCompArray_Decomp2ndChar.Find(in2ndChar);
	if( comparrayindex == kIndex_Invalid )
	{
		_ACError("",NULL);
		return 0;
	}
	
	//1�Ԗ�decomp����������
	AIndex	index = mCanonicalCompArray_Decomp1stCharArray.GetObjectConst(comparrayindex).Find(in1stChar);
	if( index == kIndex_Invalid )
	{
		return 0;
	}
	else
	{
		//�Ή�����comp������Ԃ�
		return mCanonicalCompArray_CompCharArray.GetObjectConst(comparrayindex).Get(index);
	}
}

//#427
/**
UTF-8�e�L�X�g���K��(NormalizeD)
*/
/*
//��
void	AUnicodeData::NormalizeD( const AText& inText, AText& outText ) const
{
	outText.DeleteAll();
	//inText����ꕶ�����ǂ��NormalizeD���Atext�֊i�[
	AArray<AUCS4Char>	text;
	AItemCount	len = inText.GetItemCount();
	for( AIndex pos = 0; pos < len; )
	{
		AUCS4Char	ch = 0;
		pos += inText.Convert1CharToUCS4(pos,ch);
		CatNormalizedText(text,ch,kUnicodeNormalizeType_D,false,false,false);
	}
	//UCS4->UTF8
	AItemCount	ucs4textlen = text.GetItemCount();
	for( AIndex i = 0; i  < ucs4textlen; i++ )
	{
		outText.AddFromUCS4Char(text.Get(i));
	}
}
*/

//#863
/**
�O�̕����ʒu���擾�icanonical decomp�����̓X�L�b�v�j
*/
AIndex	AUnicodeData::GetPrevCharPosSkipCanonicalDecomp( const AText& inText, const AIndex inPos, const ABool inReturnMinusWhen0 ) 
{
	AIndex	pos = inText.GetPrevCharPos(inPos,inReturnMinusWhen0);
	if( pos < inPos )
	{
		//inPos�̂P�O�̕������擾
		AUCS4Char	ucs4char = 0;
		inText.Convert1CharToUCS4(pos,ucs4char);
		//inPos�̂P�O�̕�����decomp2�����ڂ��ǂ����𔻒�
		if( IsCanonicalDecomp2ndChar(ucs4char) == true )
		{
			//------------------inPos�̂P�O�̕�����decomp2�����ڂ̏ꍇ------------------
			//1�O�̕����̈ʒu�i��decomp1�����ځj���擾
			AIndex	prevpos = inText.GetPrevCharPos(pos,inReturnMinusWhen0);
			if( prevpos < pos )
			{
				//decomp�̑O�̕������擾
				AUCS4Char	prevUcs4char = 0;
				inText.Convert1CharToUCS4(prevpos,prevUcs4char);
				//comp�������擾�ł��邩�ǂ���������
				AUCS4Char	compchar = GetCanonicalCompChar(ucs4char,prevUcs4char);
				if( compchar != 0 )
				{
					//comp�����L��Ȃ�Adecomp�P�����ڂ̈ʒu��Ԃ�l�ɂ���
					pos = prevpos;
				}
			}
		}
	}
	return pos;
}
AIndex	AUnicodeData::GetPrevCharPosSkipCanonicalDecomp( const AUChar* inTextPtr, const AItemCount inTextLen, const AIndex inPos, const ABool inReturnMinusWhen0 ) 
{
	AIndex	pos = AText::GetPrevCharPos(inTextPtr,inTextLen,inPos,inReturnMinusWhen0);
	if( pos < inPos )
	{
		//inPos�̂P�O�̕������擾
		AUCS4Char	ucs4char = 0;
		AText::Convert1CharToUCS4(inTextPtr,inTextLen,pos,ucs4char);
		//inPos�̂P�O�̕�����decomp2�����ڂ��ǂ����𔻒�
		if( IsCanonicalDecomp2ndChar(ucs4char) == true )
		{
			//------------------inPos�̂P�O�̕�����decomp2�����ڂ̏ꍇ------------------
			//1�O�̕����̈ʒu�i��decomp1�����ځj���擾
			AIndex	prevpos = AText::GetPrevCharPos(inTextPtr,inTextLen,pos,inReturnMinusWhen0);
			if( prevpos < pos )
			{
				//decomp�̑O�̕������擾
				AUCS4Char	prevUcs4char = 0;
				AText::Convert1CharToUCS4(inTextPtr,inTextLen,prevpos,prevUcs4char);
				//comp�������擾�ł��邩�ǂ���������
				AUCS4Char	compchar = GetCanonicalCompChar(ucs4char,prevUcs4char);
				if( compchar != 0 )
				{
					//comp�����L��Ȃ�Adecomp�P�����ڂ̈ʒu��Ԃ�l�ɂ���
					pos = prevpos;
				}
			}
		}
	}
	return pos;
}

//#863
/**
���̕����ʒu���擾�icanonical decomp�����̓X�L�b�v�j
*/
AIndex	AUnicodeData::GetNextCharPosSkipCanonicalDecomp( const AText& inText, const AIndex inPos ) 
{
	AIndex	pos = inText.GetNextCharPos(inPos);
	if( pos > inPos && pos < inText.GetItemCount() )
	{
		//pos�̈ʒu�̕����i��inPos�̈ʒu�̕����̎��̕����j���擾
		AUCS4Char	ucs4char = 0;
		inText.Convert1CharToUCS4(pos,ucs4char);
		//pos�̈ʒu�̕�����decomp2�����ڂ��ǂ����𔻒�
		if( IsCanonicalDecomp2ndChar(ucs4char) == true )
		{
			//inPos�̈ʒu�̕������擾
			AUCS4Char	currentUcs4char = 0;
			inText.Convert1CharToUCS4(inPos,currentUcs4char);
			//comp�������擾�ł��邩�ǂ���������
			AUCS4Char	compchar = GetCanonicalCompChar(ucs4char,currentUcs4char);
			if( compchar != 0 )
			{
				//comp�����L��Ȃ�Apos�̎��̈ʒu��Ԃ�l�ɂ���
				pos = inText.GetNextCharPos(pos);
			}
		}
	}
	return pos;
}
AIndex	AUnicodeData::GetNextCharPosSkipCanonicalDecomp( const AUChar* inTextPtr, const AItemCount inTextLen, const AIndex inPos ) 
{
	AIndex	pos = AText::GetNextCharPos(inTextPtr,inTextLen,inPos);
	if( pos > inPos && pos < inTextLen )
	{
		//pos�̈ʒu�̕����i��inPos�̈ʒu�̕����̎��̕����j���擾
		AUCS4Char	ucs4char = 0;
		AText::Convert1CharToUCS4(inTextPtr,inTextLen,pos,ucs4char);
		//pos�̈ʒu�̕�����decomp2�����ڂ��ǂ����𔻒�
		if( IsCanonicalDecomp2ndChar(ucs4char) == true )
		{
			//inPos�̈ʒu�̕������擾
			AUCS4Char	currentUcs4char = 0;
			AText::Convert1CharToUCS4(inTextPtr,inTextLen,inPos,currentUcs4char);
			//comp�������擾�ł��邩�ǂ���������
			AUCS4Char	compchar = GetCanonicalCompChar(ucs4char,currentUcs4char);
			if( compchar != 0 )
			{
				//comp�����L��Ȃ�Apos�̎��̈ʒu��Ԃ�l�ɂ���
				pos = AText::GetNextCharPos(inTextPtr,inTextLen,pos);
			}
		}
	}
	return pos;
}


