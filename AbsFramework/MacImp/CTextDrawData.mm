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

CTextDrawData

*/

#include "CTextDrawData.h"
#include "../Frame.h"//#414

#pragma mark ===========================
#pragma mark [�N���X]CTextDrawData
#pragma mark ===========================
//TextDrawData�N���X
//�F���̏��t���`��p�f�[�^���i�[���AImp�N���X�ɓn�����߂̃N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
*/
CTextDrawData::CTextDrawData( const ABool inDisplayEachCanonicalDecompChar ) : /*#1034 mTextLayout(NULL), mATSUStyle(NULL),*/ 
		mCTLineRef(NULL),mCTTypesetterRef(NULL)//#1034
,mFontFallbackEnabled(true), /*#1034 mReuseATSUStyle(false),*/ mDisplayEachCanonicalDecompChar(inDisplayEachCanonicalDecompChar)
,selectionFrameAlpha(0.0), selectionBackgroundColor(kColor_White)//#1316
{
	//B0386 ���s�R�[�h�ʒu�ɕςȐF���\������邱�Ƃ�������C��
	selectionStart = 0;
	//B0000 �s�܂�Ԃ��v�Z������
	Init();
}
/**
�f�X�g���N�^
*/
CTextDrawData::~CTextDrawData()
{
	OSStatus	err = noErr;
	
	//CTLine��� #1034
	ClearCTLineRef();
	
	//CTTypesetter��� #1034
	ClearCTTypesetterRef();
	
	/*#1034
	//�������̂��߁A��x�v�Z����textLayout�̓I�u�W�F�N�g�폜�܂ŕێ�����B
	if( mTextLayout != NULL )
	{
		err = ::ATSUDisposeTextLayout(mTextLayout);
		if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,NULL);
		mTextLayout = NULL;
	}
	if( mATSUStyle != NULL )
	{
		err = ::ATSUDisposeStyle(mATSUStyle);
		if( err != noErr )   _ACErrorNum("ATSUDisposeStyle() returned error: ",err,NULL);
		mATSUStyle = NULL;
	}
	*/
}

//B0000 �s�܂�Ԃ��v�Z������
/**
�S�f�[�^������
*/
void	CTextDrawData::Init()
{
	OSStatus	err = noErr;
	
	//CTLine��� #1034
	ClearCTLineRef();
	
	//CTTypesetter��� #1034
	ClearCTTypesetterRef();
	
	/*#1034
	if( mTextLayout != NULL )
	{
		err = ::ATSUDisposeTextLayout(mTextLayout);
		if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,NULL);
		mTextLayout = NULL;
	}
	if( mATSUStyle != NULL && mReuseATSUStyle == false )//#695
	{
		err = ::ATSUDisposeStyle(mATSUStyle);
		if( err != noErr )   _ACErrorNum("ATSUDisposeStyle() returned error: ",err,NULL);
		mATSUStyle = NULL;
	}
	*/
	
	UTF16DrawText.DeleteAll();
	OriginalTextArray_UnicodeOffset.DeleteAll();
	UTF16DrawTextArray_OriginalTextIndex.DeleteAll();
	OriginalTextArray_TabLetters.DeleteAll();
	attrPos.DeleteAll();
	attrColor.DeleteAll();
	attrStyle.DeleteAll();
	additionalAttrPos.DeleteAll();//#1316
	additionalAttrLength.DeleteAll();//#1316
	additionalAttrColor.DeleteAll();//#1316
	additionalAttrStyle.DeleteAll();//#1316
	drawSelection = false;
	selectionStart = kIndex_Invalid;
	selectionEnd = kIndex_Invalid;
	selectionColor = kColor_Blue;
	selectionAlpha = 0.8;
	selectionFrameAlpha = 0.0;//#1316
	selectionBackgroundColor = kColor_White;//#1316
	misspellStartArray.DeleteAll();
	misspellEndArray.DeleteAll();
	startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex.DeleteAll();//#117
	controlCodeStartUTF8Index.DeleteAll();//#473
	controlCodeEndUTF8Index.DeleteAll();//#473
	hintTextStartUTF8Index.DeleteAll();
	hintTextEndUTF8Index.DeleteAll();
}

#pragma mark ===========================

#pragma mark ---TextLayout�ݒ�

#if 0 
/**
TextLayout�ݒ�
*/
void	CTextDrawData::SetTextLayout( ATSUTextLayout inTextLayout )
{
	OSStatus	err = noErr;
	
	if( mTextLayout != NULL )
	{
		err = ::ATSUDisposeTextLayout(mTextLayout);
		if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,NULL);
		mTextLayout = NULL;
	}
	mTextLayout = inTextLayout;
}

//#532
/**
TextLayout�L���b�V�����N���A
*/
void	CTextDrawData::ClearTextLayout()
{
	OSStatus	err = noErr;
	
	if( mTextLayout != NULL )
	{
		err = ::ATSUDisposeTextLayout(mTextLayout);
		if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,NULL);
		mTextLayout = NULL;
	}
}
#endif

//#1034
/**
CTLine�ݒ�
*/
void	CTextDrawData::SetCTLineRef( CTLineRef inCTLineRef )
{
	ClearCTLineRef();
	mCTLineRef = inCTLineRef;
}

//#1034
/**
CTLine���
*/
void	CTextDrawData::ClearCTLineRef()
{
	if( mCTLineRef != NULL )
	{
		::CFRelease(mCTLineRef);
		mCTLineRef = NULL;
	}
}

//#1034
/**
CTLine�ݒ�
*/
void	CTextDrawData::SetCTTypesetterRef( CTTypesetterRef inCTTypesetterRef )
{
	ClearCTTypesetterRef();
	mCTTypesetterRef = inCTTypesetterRef;
}

//#1034
/**
CTLine���
*/
void	CTextDrawData::ClearCTTypesetterRef()
{
	if( mCTTypesetterRef != NULL )
	{
		::CFRelease(mCTTypesetterRef);
		mCTTypesetterRef = NULL;
	}
}

#pragma mark ===========================

#pragma mark --- TextDrawData�쐬

//win AView_Edit�쐬�̂��߁AATextInfo����R�s�[���č쐬
/**
�e�L�X�g�`��p�f�[�^����

UTF16���A�^�u�̃X�y�[�X���A���䕶���̉��������s���B
CTextDrawData�N���X�̂����A���L��ݒ肷��B
UTF16DrawText: UTF16�ł̕`��p�e�L�X�g
UnicodeDrawTextIndexArray: �C���f�b�N�X�F�h�L�������g���ێ����Ă��錳�̃e�L�X�g�̃C���f�b�N�X�@�l�F�`��e�L�X�g��Unicode�����P�ʂł̃C���f�b�N�X
*/
void	CTextDrawData::MakeTextDrawDataWithoutStyle( const AText& inText, const ANumber inTabWidth, const AItemCount inLetterCountLimit, 
			const ABool inGetUTF16Text, const ABool inCountAs2Letters, const ABool inDisplayControlCode,
			const AIndex inStartIndex, const AItemCount inLength,
			const ABool inStartSpaceToIndent, const AItemCount inStartSpaceCount, const AItemCount inStartIndentWidth )//#117
{
	AArray<AIndex>	hintTextPosArray;
	ATextArray	hintTextArray;
	MakeTextDrawDataWithoutStyle(inText,inTabWidth,inLetterCountLimit,inGetUTF16Text,inCountAs2Letters,
								 inDisplayControlCode,inStartIndex,inLength,
								 hintTextPosArray, hintTextArray,
								 inStartSpaceToIndent,inStartSpaceCount,inStartIndentWidth,false
								 );
}
void	CTextDrawData::MakeTextDrawDataWithoutStyle( const AText& inText, 
													const ANumber inTabWidth, 
													const AItemCount inLetterCountLimit, 
													const ABool inGetUTF16Text, 
													const ABool inCountAs2Letters, 
													const ABool inDisplayControlCode,
													const AIndex inStartIndex, 
													const AItemCount inLength,
													const AArray<AIndex>& inHintTextPosArray,
													const ATextArray& inHintTextArray,
													const ABool inStartSpaceToIndent, 
													const AItemCount inStartSpaceCount, 
													const AItemCount inStartIndentWidth,
													const ABool inDisplayYenFor5C
													)//#117
{
	//B0000 �s�܂�Ԃ��v�Z������
	Init();
	
	//unicode data�擾
	AUnicodeData&	unicodeData = AApplication::GetApplication().NVI_GetUnicodeData();
	
	//�s���擾
	AIndex	lineInfo_Start = inStartIndex;
	//B0000 �s�܂�Ԃ��v�Z������
	AItemCount	lineInfo_LengthWithoutCR = inLength;
	
	//#733
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
	AIndex	originalTextIndex = 0;
	AIndex	nextOriginalTextIndex = 0;
	ABool	gyoutou = true;//#117
	AUCS4Char	nextUCS4Char = 0;//#863
	AItemCount	nextbc = 1;//#863
	ABool	nextUCS4CharLoaded = false;//#863
	for( ; originalTextIndex < lineInfo_LengthWithoutCR; originalTextIndex = nextOriginalTextIndex )
	{
		if( inLetterCountLimit > 0 )
		{
			if( tabletters >= inLetterCountLimit )   break;
		}
		
		//==================�����擾==================
		AItemCount	bc = 1;
		AUCS4Char	ucs4Char = 0;//#863
		if( nextUCS4CharLoaded == true )
		{
			//���̕������[�h�ς݂Ȃ炻����g�p
			ucs4Char = nextUCS4Char;
			bc = nextbc;
		}
		else
		{
			//���̕��������[�h�Ȃ�A�����ǂݍ���
			bc = AText::Convert1CharToUCS4(textp,textlen,lineInfo_Start+originalTextIndex,ucs4Char);//#733
		}
		//���̕������[�h�ς݃t���OOFF
		nextUCS4CharLoaded = false;
		//���̕��������[�h
		if( originalTextIndex+bc < lineInfo_LengthWithoutCR )
		{
			nextbc = AText::Convert1CharToUCS4(textp,textlen,lineInfo_Start+originalTextIndex+bc,nextUCS4Char);
			nextUCS4CharLoaded = true;
			//���̕�����decomp�����̏ꍇ�Acomp�����ɕϊ�����
			/*#1051 OS10.9�ł�decomp�ɂȂ��Ă��Ă�1�����ŕ\�������̂Ō�������K�v�����Ȃ��Ȃ������߁A���mDisplayEachCanonicalDecompChar==true�i2���������j�Ƃ��Ĉ���
			if( mDisplayEachCanonicalDecompChar == false && unicodeData.IsCanonicalDecomp2ndChar(nextUCS4Char) == true )
			{
				//------------------Canonical Comp�����i�����㕶���j���擾���āA���񏈗�����UCS4����(ucs4char)��u������------------------
				AUCS4Char	compchar = unicodeData.GetCanonicalCompChar(nextUCS4Char,ucs4Char);
				if( compchar != 0 )
				{
					//���񏈗�����UCS4������Comp�����ɒu������
					ucs4Char = compchar;
					//����̕�������decomp�����̌�܂łɂ���
					bc += nextbc;
					//���̕������[�h�ς݃t���OOFF
					nextUCS4CharLoaded = false;
				}
			}
			*/
		}
		//���̕����̈ʒu�ݒ�
		nextOriginalTextIndex = originalTextIndex + bc;
		//�X�y�[�X�E�^�u�ȊO��������s���t���O��false�ɂ���B
		if( ucs4Char != kUChar_Space && ucs4Char != kUChar_Tab )   gyoutou = false;//#117
		
		//�q���g�e�L�X�g�ʒu�ł���΃q���g��index���擾
		AIndex	hintTextArrayIndex = inHintTextPosArray.Find(originalTextIndex);
		
		//==================���s�R�[�h�̏ꍇ==================
		if( ucs4Char == kUChar_LineEnd && bc == 1 )//�C���[�K����UTF-8������c0 8d���̏ꍇ�AConvert1CharToUCS4()��0x0D�ƕϊ����Ă��܂��̂ŁA�����r�����邽��bc���`�F�b�N�B�i���s�R�[�h�͐������ǂ܂Ȃ��ƁA�s�܂�Ԃ��v�Z�Ŗ������[�v����j
		{
			break;
		}
		//==================�^�u�R�[�h�̏ꍇ==================
		else if( ucs4Char == kUChar_Tab && bc == 1)
		{
			//�^�u����
			
			//------------------�I���W�i���e�L�X�g�ɑΉ�����z��Ƀf�[�^�ݒ�------------------
			for( AIndex i = 0; i < bc; i++ )
			{
				//UTF16DrawText���̑Ή�����index��ݒ�i����unicode offset�j
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				//�^�u�p�����J�E���g����ݒ�
				OriginalTextArray_TabLetters.Add(tabletters);
			}
			
			//------------------Unicode�ꕶ���i�X�y�[�X�j�ǉ�------------------
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
			UTF16DrawText.AddUTF16Char(kUChar_Space);
			uniOffset++;
			
			//
			tabletters++;//B0154
			while( (tabletters%inTabWidth) != 0 )//B0154
			{
				//Unicode�ꕶ���ǉ�
				//�^�u�����̑O���^�㔼�ŁA�Ή����錳�̃e�L�X�g�̈ʒu��ς���B
				if( (tabletters%inTabWidth) > inTabWidth/2 )
				{
					UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex+1);
				}
				else
				{
					UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				}
				UTF16DrawText.AddUTF16Char(kUChar_Space);
				uniOffset++;
				
				//
				tabletters++;//B0154
			}
		}
		//==================�q���g�e�L�X�g==================
		else if( hintTextArrayIndex != kIndex_Invalid )
		{
			//------------------�I���W�i���e�L�X�g�ɑΉ�����z��Ƀf�[�^�ݒ�------------------
			for( AIndex i = 0; i < bc; i++ )
			{
				//UTF16DrawText���̑Ή�����index��ݒ�i����unicode offset�j
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				//�^�u�p�����J�E���g����ݒ�
				OriginalTextArray_TabLetters.Add(tabletters);
			}
			
			//�q���g�e�L�X�g�ǉ�
			AText	text;
			inHintTextArray.Get(hintTextArrayIndex,text);
			text.InsertCstring(0,"  ");
			text.AddCstring("  ");
			text.ConvertFromUTF8ToUTF16();
			UTF16DrawText.AddText(text);
			//
			for( AIndex i = 0; i < text.GetItemCount()/2; i++ )
			{
				UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				uniOffset++;
			}
			//�q���g�e�L�X�g�ʒu�L��
			hintTextStartUTF8Index.Add(originalTextIndex);
			hintTextEndUTF8Index.Add(originalTextIndex+1);
		}
		
		//==================���䕶���̏ꍇ==================
		else if( inDisplayControlCode == true && ucs4Char <= 0x1F && bc == 1)
		{
			//���䕶����HEX�\������
			
			//UTF-8�G���[�����̏ꍇ�AConvert1CharToUCS4()�ɂ��ucs4Char��0x00�ɂȂ��Ă���̂ŁA
			//���̂΂����Ɍ��̃R�[�h��\�����邽�߂ɁA�ēx���f�[�^��ǂݍ��ށB
			AUChar	ch = inText.Get(lineInfo_Start+originalTextIndex);
			
			//------------------�I���W�i���e�L�X�g�ɑΉ�����z��Ƀf�[�^�ݒ�------------------
			for( AIndex i = 0; i < bc; i++ )
			{
				//UTF16DrawText���̑Ή�����index��ݒ�i����unicode offset�j
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				//�^�u�p�����J�E���g����ݒ�
				OriginalTextArray_TabLetters.Add(tabletters);
			}
			
			//Unicode�ꕶ���ǉ�
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
			UTF16DrawText.AddUTF16Char(kUChar_Space);
			uniOffset++;
			//Unicode�ꕶ���ǉ�
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
			UTF16DrawText.AddUTF16Char(hex2asc((ch&0xF0)/0x10));
			uniOffset++;
			//Unicode�ꕶ���ǉ�
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
			UTF16DrawText.AddUTF16Char(hex2asc(ch&0xF));
			uniOffset++;
			//Unicode�ꕶ���ǉ�
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
			UTF16DrawText.AddUTF16Char(kUChar_Space);
			uniOffset++;
			
			//
			tabletters++;
			
			//#473
			controlCodeStartUTF8Index.Add(originalTextIndex);
			controlCodeEndUTF8Index.Add(originalTextIndex+bc);
		}
		/*#844
		//==================SJIS���X���X�t�H�[���o�b�N�����̏ꍇ==================
		//#473 SJIS���X���X�t�H�[���o�b�N 2�o�C�g�ڕ����Ȃ��G���[��ԕ\������
		else if( ucs4Char >= 0x105C00 && ucs4Char <= 0x105CFF )
		{
			//------------------�I���W�i���e�L�X�g�ɑΉ�����z��Ƀf�[�^�ݒ�------------------
			for( AIndex i = 0; i < bc; i++ )
			{
				//UTF16DrawText���̑Ή�����index��ݒ�i����unicode offset�j
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				//�^�u�p�����J�E���g����ݒ�
				OriginalTextArray_TabLetters.Add(tabletters);
			}
			
			//�G���[�����i1�o�C�g�ځj�擾
			AUChar	sjisch = (ucs4Char&0xFF);
			
			//�G���[�e�L�X�g�擾
			AText	t;
			t.SetLocalizedText("SJIS_Error_2ndByte");
			AText	text;
			text.Add(' ');
			text.Add(hex2asc((sjisch&0xF0)/0x10));
			text.Add(hex2asc(sjisch&0xF));
			text.Add(' ');
			text.AddText(t);
			text.ConvertFromUTF8ToUTF16();//#863
			//UTF16DrawText�ɃG���[�e�L�X�g�ǉ�
			UTF16DrawText.AddText(text);//#863
			//�G���[�e�L�X�g��ݒ�
			for( AIndex i = 0; i < text.GetItemCount()/2; i++ )
			{
				//UTF16DrawText�ɃG���[�e�L�X�g�ǉ��������̏���
				UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				uniOffset++;
			}
			
			//�^�u
			tabletters++;
			
			//���䕶���͈͐ݒ�
			controlCodeStartUTF8Index.Add(originalTextIndex);
			controlCodeEndUTF8Index.Add(originalTextIndex+bc);
		}
		*/
		//==================��L�ȊO�̕����̏ꍇ==================
		else
		{
			//#117
			if( gyoutou == true && inStartSpaceToIndent == true && inStartSpaceCount > 0 && inStartIndentWidth > 0 &&
						originalTextIndex+inStartSpaceCount <= lineInfo_LengthWithoutCR )
			{
				//���݂�originalTextIndex�ʒu����inStartSpaceCount��space���A�����Ă��邩�ǂ������`�F�b�N����
				ABool	nspace = true;
				for( AIndex i = originalTextIndex ; i < originalTextIndex+inStartSpaceCount; i++ )
				{
					AUChar	ch = (textp)[lineInfo_Start+i];//#733 inText.Get(lineInfo_Start+i);
					if( ch != kUChar_Space )
					{
						nspace = false;
						break;
					}
				}
				//n��space�����݂���Ȃ�΁AinStartIndentWidth��space�ɒu�������Ēǉ�
				if( nspace == true )
				{
					startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
					//
					for( AIndex i = 0; i < inStartSpaceCount; i++ )
					{
						//�����L�[�ɂ��L�����b�g�ړ��Ŕ��p�X�y�[�X���L�����b�g�ʒu������悤�ɂ���
						AIndex	uo = uniOffset + i;
						if( uo >= uniOffset+inStartIndentWidth )   uo = uniOffset+inStartIndentWidth-1;
						OriginalTextArray_UnicodeOffset.Add(uo);
						OriginalTextArray_TabLetters.Add(tabletters);
						tabletters++;
					}
					//Unicode n�����ǉ�
					for( AIndex i = 0; i < inStartIndentWidth; i++ )
					{
						//�N���b�N���A�������Ȃ�^���^�u�O�A�E�����Ȃ�^���^�u��ɂȂ�悤�ɂ���
						if( i > inStartIndentWidth/2 )
						{
							UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex+inStartSpaceCount);
						}
						else
						{
							UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
						}
						UTF16DrawText.AddUTF16Char(kUChar_Space);
						uniOffset++;
					}
					//����originalTextIndex�̈ʒu��ݒ�
					nextOriginalTextIndex = originalTextIndex + inStartSpaceCount;
					nextUCS4CharLoaded = false;
					continue;
				}
			}
			
			//==================����ȊO�̕��ʂ̕���==================
			
			//------------------�I���W�i���e�L�X�g�ɑΉ�����z��Ƀf�[�^�ݒ�------------------
			for( AIndex i = 0; i < bc; i++ )
			{
				//UTF16DrawText���̑Ή�����index��ݒ�i����unicode offset�j
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				//�^�u�p�����J�E���g����ݒ�
				OriginalTextArray_TabLetters.Add(tabletters);
			}
			
			/*
			//B0000���Ƃ���"�"�̂݋L�q���ꂽ�t�@�C����ǂݍ��݁A�G���R�[�f�B���O����F�������ꍇ���Abc�𑫂��ƃe�L�X�g�S�̂��I�[�o�[����\��������
			��AText::Convert1CharToUCS4()�ɂă`�F�b�N���Ă���̂ł��̃`�F�b�N�͂����K�v�Ȃ�
			if( lineInfo_Start+originalTextIndex+bc > textcount )
			{
				ucs4Char = 0x20;
				bc = 1;
			}
			*/
			//
			if( ucs4Char < 0x10000 )
			{
				//inDisplayYenFor5C��true�Ȃ�A�o�b�N�X���b�V���iU+005C�j�𔼊p���iU+00A5)�ŕ\������ #940
				if( inDisplayYenFor5C == true && ucs4Char == 0x005C )
				{
					ucs4Char = 0x00A5;
				}
				//Unicode�ꕶ���ǉ�
				UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				UTF16DrawText.AddUTF16Char(ucs4Char);
				uniOffset++;
			}
			else
			{
				//Unicode�T���Q�[�g�y�A�ǉ�
				UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				AUCS4Char	c = ucs4Char - 0x10000;
				AUCS4Char	high = (c&0x1FC00)>>10;
				UTF16DrawText.AddUTF16Char(0xD800+high);
				AUCS4Char	low = (c&0x3FF);
				UTF16DrawText.AddUTF16Char(0xDC00+low);
				uniOffset += 2;
			}
			/*
			//B0000 MacOSX10.4�ł�U+FFFF(EF BF BF)���g���Ƃ��������Ȃ�H�̂ŁAU+FFFF��U+3000(�S�p�X�y�[�X)�ɂ���
			//���K�v�H
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
			*/
			//
			if( unicodeData.IsWideWidth(ucs4Char) == false )//#414
			{
				tabletters++;
			}
			else
			{
				tabletters += tabinc0800;
			}
		}
	}
	UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
	//inText�̕�����+1�܂ŁAOriginalTextArray_xxx��ݒ肷�� #0 ���s�݂̂̃e�L�X�g�̏ꍇ�ɍŏI������̈ʒu��OriginalTextArray_UnicodeOffset�f�[�^���������̏C���̂��߁A1����s���烋�[�v���s�֕ύX
	//�E�E�E���悤�Ǝv�������A�Ȃ����������x���Ȃ�i�s�܂�Ԃ��v�Z�v���O���X�_�C�A���O���o��̂Ō��ɖ߂��B�j
	//while( OriginalTextArray_UnicodeOffset.GetItemCount() < lineInfo_LengthWithoutCR+1 )
	{
		OriginalTextArray_UnicodeOffset.Add(uniOffset);
		OriginalTextArray_TabLetters.Add(tabletters);
	}

	/*
	if( inGetUTF16Text == true )
	{
		//UTF8DrawText����UTF16DrawText�֕ϊ�
		UTF16DrawText.SetText(UTF8DrawText);
		UTF16DrawText.ConvertFromUTF8ToUTF16();
		//
		while( UTF16DrawTextArray_OriginalTextIndex.GetItemCount() <= UTF16DrawText.GetItemCount()/2 )
		{
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
		}
	}
	*/
}

//R0240
/**
�e�L�X�g�`��p�f�[�^����
*/
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
	AItemCount	tabletters = OriginalTextArray_TabLetters.Get(OriginalTextArray_TabLetters.GetItemCount()-1);
	
	//�Ō�̂P������
	UTF16DrawTextArray_OriginalTextIndex.Delete1(UTF16DrawTextArray_OriginalTextIndex.GetItemCount()-1);
	OriginalTextArray_UnicodeOffset.Delete1(OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	OriginalTextArray_TabLetters.Delete1(OriginalTextArray_TabLetters.GetItemCount()-1);
	
	//�F�ݒ�
	attrPos.Add(uniOffset);
	attrColor.Add(inColor);
	attrStyle.Add(kTextStyle_Normal);
	
	//UTF16�e�L�X�g�ݒ�
	UTF16DrawText.AddText(inUTF16Text);
	
	//UTF8-UTF16 �C���f�b�N�X�ϊ��z��ݒ�
	for( AIndex pos = 0; pos < inUTF8Text.GetItemCount(); )
	{
		AItemCount	bc = inUTF8Text.GetUTF8ByteCount(pos);
		for( AIndex i = 0; i < bc; i++ )
		{
			OriginalTextArray_UnicodeOffset.Add(uniOffset);
			OriginalTextArray_TabLetters.Add(tabletters);
		}
		pos+= bc;
		uniOffset++;
		UTF16DrawTextArray_OriginalTextIndex.Add(index);
		index++;
		tabletters++;
	}
	OriginalTextArray_UnicodeOffset.Add(uniOffset);
	UTF16DrawTextArray_OriginalTextIndex.Add(index);
	OriginalTextArray_TabLetters.Add(tabletters);
	//
	//#1034 ClearTextLayout();
	
	//CTLine��� #1034
	ClearCTLineRef();
	
	//CTTypesetter��� #1034
	ClearCTTypesetterRef();
	
}

//#1316
/**
Attribute���ォ��ǉ�����
*/
void	CTextDrawData::AddAttributeWithUTF8Offset( const AIndex inStart, const AIndex inEnd, const AColor& inColor )
{
	AIndex	spos = OriginalTextArray_UnicodeOffset.Get(inStart);
	AIndex	epos = OriginalTextArray_UnicodeOffset.Get(inEnd);
	additionalAttrPos.Add(spos);
	additionalAttrLength.Add(epos-spos);
	additionalAttrColor.Add(inColor);
	additionalAttrStyle.Add(kTextStyle_Normal);
	
	//CTLine��� #1034
	ClearCTLineRef();
	
	//CTTypesetter��� #1034
	ClearCTTypesetterRef();
}

/**
Attribute���ォ��ǉ�����
*/
void	CTextDrawData::AddAttributeWithUnicodeOffset( const AIndex inStart, const AIndex inEnd, const AColor& inColor )
{
	additionalAttrPos.Add(inStart);
	additionalAttrLength.Add(inEnd-inStart);
	additionalAttrColor.Add(inColor);
	additionalAttrStyle.Add(kTextStyle_Normal);
	
	//CTLine��� #1034
	ClearCTLineRef();
	
	//CTTypesetter��� #1034
	ClearCTTypesetterRef();
}

