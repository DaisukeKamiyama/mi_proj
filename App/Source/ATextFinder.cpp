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

ATextFinder

*/

#include "stdafx.h"

#include "ATextFinder.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]ATextFinder
#pragma mark ===========================
//�e�L�X�g����

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
ATextFinder::ATextFinder( AObjectArrayItem* inParent )//#750
: AObjectArrayItem(inParent),mCurrentParameter_IgnoreCase(true),mCurrentParameter_IgnoreBackslashYen(true),
mCurrentParameter_Fuzzy_NFKD(true),mCurrentParameter_Fuzzy_KanaType(true),
mCurrentParameter_Fuzzy_Onbiki(true),mCurrentParameter_Fuzzy_KanaYuragi(true)
,mFindTextLen(0)//#733
,mCurrentParameter_Reverse(false)//#789
,mInited(false)
{
}

/**
�������iBM�e�[�u���̏������j
*/
void	ATextFinder::Init()
{
	//BMTable������
	mBMTable.DeleteAll();
	mBMTable.Reserve(0,kBMTableCount);//#852
	for( AIndex i = 0; i < kBMTableCount; i++ )
	{
		//#852 mBMTable.Add(0);
		mBMTable.Set(i,0);//#852
	}
	//�������t���O�ݒ�
	mInited = true;
}

#pragma mark ===========================

#pragma mark ---�������s

//�������s
ABool	ATextFinder::ExecuteFind( const AText& inText, const AFindParameter& inParam, const ABool inReverse,//#789
		const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos,
		const ABool& inAbortFlag, //B0359
		const ABool inUseModalSession, ABool& outModalSessionAborted )
{
	//���������Ȃ珉�������s
	if( mInited == false )
	{
		Init();
	}
	//
	if( inParam.regExp == false )
	{
		return BMFind(inText,inParam,inReverse,inSpos,inEpos,outSpos,outEpos,inAbortFlag,inUseModalSession,outModalSessionAborted);
	}
	else
	{
		return RegExpFind(inText,inParam,inReverse,inSpos,inEpos,outSpos,outEpos,inAbortFlag,inUseModalSession,outModalSessionAborted);
	}
}

//�O���������p�o�b�t�@ #789
AUCS4Char	gFindTextBufferForReverse[kBufferSize_TextFinder_UCS4FindText];

//BM�@�ɂ��ʏ팟��
ABool	ATextFinder::BMFind( const AText& inText, const AFindParameter& inParam, const ABool inReverse,//#789
		const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos,
		const ABool& inAbortFlag, //B0359
		const ABool inUseModalSession, ABool& outModalSessionAborted )
{
	//���f���ꂽ�t���O������
	outModalSessionAborted = false;
	
	//#789 ����������A�����Ώۃe�L�X�g�̒�����0�Ȃ炱���Ń��^�[��
	if( inParam.findText.GetItemCount() <= 0 )
	{
		_ACError("",NULL);
		return false;
	}
	if( inText.GetItemCount() <= 0 )
	{
		return false;
	}
	
	ABool	result = false;
	
	//#733 ������
	//�Ώۃe�L�X�g�ւ̃|�C���^�擾
	AStTextPtr	targetStTextPtr(inText,0,inText.GetItemCount());
	const AUChar*	targettextptr = targetStTextPtr.GetPtr();
	const AItemCount	targettextlen = targetStTextPtr.GetByteCount();
	//����������ւ̃|�C���^�擾
	AStTextPtr	findStTextPtr(inParam.findText,0,inParam.findText.GetItemCount());
	const AUChar*	findtextptr = findStTextPtr.GetPtr();
	const AItemCount	findtextlen = findStTextPtr.GetByteCount();
	
	//#166 �B�������@�\����
	ABool	fuzzy_NFKD = false;
	ABool	fuzzy_KanaType = false;
	ABool	fuzzy_Onbiki = false;
	ABool	fuzzy_KanaYuragi = false;
	if( inParam.fuzzy == true )
	{
		fuzzy_NFKD 			= GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFuzzyFind_NFKD);
		fuzzy_KanaType 		= GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFuzzyFind_KanaType);
		fuzzy_Onbiki		= GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFuzzyFind_Onbiki);
		fuzzy_KanaYuragi 	= GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFuzzyFind_KanaYuragi);
	}
	
	//#709
	//�X�y�[�X�E�^�u�E���s�A�����������̏ꍇ�ɁA
	//�S�Ă̌����Ώە����������Ώە����̏ꍇ�́A�X�y�[�X�E�^�u�E���s�A�������������Ȃ��悤�ɂ���
	//���Ƃ��΁u�[�v��������������ꍇ�B�����Ώۂ��u�[�v�Ȃ�B������������K�v�͖����͂��B
	//�imFindText����ɂȂ�̂�h���B�j
	ABool	ignoreSpace = inParam.ignoreSpaces;
	ABool	ignoreOnbiki = fuzzy_Onbiki;
	ABool	targetCharExists = false;//�������Ȃ������L��t���O
	//�����������������
	for( AIndex pos = 0; pos < inParam.findText.GetItemCount(); )
	{
		//UCS4�����ɕϊ�
		AUCS4Char	uc = 0;
		pos += AText::Convert1CharToUCS4(findtextptr,findtextlen,pos,uc);//#733
		
		//�����Ώە����łȂ���΃t���Oon�ɂ���break
		if( IsIgnoreChar(uc,ignoreSpace,ignoreOnbiki) == false )
		{
			targetCharExists = true;
			break;
		}
	}
	//�S�Ă̕����������Ώە����̏ꍇ�A�X�y�[�X�E�^�u�E���s�A�������������Ȃ��悤�ɂ���
	if( targetCharExists == false )
	{
		ignoreSpace = false;
		ignoreOnbiki = false;
	}
	
	//#733
	//�������̂��߁A�Q�Ƃ����[�v�O�Ŏ擾���L��
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
	const AHashArray<AUCS4Char>&	fuzzySearchNormalizeArray_from = GetApp().SPI_GetFuzzySearchNormalizeArray_from();
	const AObjectArray< AArray<AUCS4Char> >&	fuzzySearchNormalizeArray_to = GetApp().SPI_GetFuzzySearchNormalizeArray_to();
	
	//=======================================================
	//�@1. �܂��AmFindText��UTF16�Ō�����������i�[���ABMTable�\������
	//=======================================================
	//�O�񌟍������Ƃ��̃p�����[�^�Ɣ�r���A�S�������ł���΁AmFindText, mBMTable�͂��̂܂܎g�p����
	if( 	(inParam.findText.Compare(mCurrentParameter_FindText) == false) ||
			(inParam.ignoreCase != mCurrentParameter_IgnoreCase) ||
	   (inParam.ignoreBackslashYen != mCurrentParameter_IgnoreBackslashYen) ||
	   (inParam.ignoreSpaces != mCurrentParameter_IgnoreSpaces ) ||//#936
			//#166 (inParam.fuzzy != mCurrentParameter_Fuzzy) )
			//#166
			(fuzzy_NFKD != mCurrentParameter_Fuzzy_NFKD) ||
			(fuzzy_KanaType != mCurrentParameter_Fuzzy_KanaType) ||
			(fuzzy_Onbiki != mCurrentParameter_Fuzzy_Onbiki) ||
			(fuzzy_KanaYuragi != mCurrentParameter_Fuzzy_KanaYuragi) ||
			(inReverse != mCurrentParameter_Reverse) )//#789
	{
		//���񌟍��̃p�����[�^��ۑ�
		mCurrentParameter_FindText.SetText(inParam.findText);
		mCurrentParameter_IgnoreCase = inParam.ignoreCase;
		mCurrentParameter_IgnoreBackslashYen = inParam.ignoreBackslashYen;
		mCurrentParameter_IgnoreSpaces = inParam.ignoreSpaces;//#936
		//#166 mCurrentParameter_Fuzzy = inParam.fuzzy;
		//#166
		mCurrentParameter_Fuzzy_NFKD = fuzzy_NFKD;
		mCurrentParameter_Fuzzy_KanaType = fuzzy_KanaType;
		mCurrentParameter_Fuzzy_Onbiki = fuzzy_Onbiki;
		mCurrentParameter_Fuzzy_KanaYuragi = fuzzy_KanaYuragi;
		//#789 �t�����t���O
		mCurrentParameter_Reverse = inReverse;
		
		//===========================================================
		//0�`GetItemCount()�̊Ԃ̕�����UCS4�ɕϊ����āAmFindText�Ɋi�[����B
		//inReverse��true�̏ꍇ�́A��납��mFindText�Ɋi�[����B
		//===========================================================
		
		//#733 mFindText.DeleteAll();
		mFindTextLen = 0;//#733
		//findText�̊J�n�ʒu�̎擾 #789
		//�����������̏ꍇ�́A�ŏ���UTF-8�����̊J�n�ʒu���A�J�n�ʒu�ɂ���
		AIndex	pos = 0;
		if( inReverse == true )
		{
			//�t���������̏ꍇ�́A�Ō��UTF-8�����̊J�n�ʒu���A�J�n�ʒu�ɂ���B
			pos = inParam.findText.GetCurrentCharPos(inParam.findText.GetItemCount()-1);
		}
		//
		//#789 for( AIndex pos = 0; pos < inParam.findText.GetItemCount(); )
		while( pos >= 0 && pos < inParam.findText.GetItemCount() )//#789 pos��findText�̗L���͈͓��̊ԁA���[�v����
		{
			//pos�̈ʒu��Unicode�������擾
			AUCS4Char	uc = 0;//#412
			if( inReverse == false )
			{
				//�����������̏ꍇ�́Apos�̈ʒu��Unicode�������擾���A����UTF-8�����J�n�ʒu�ֈړ��B
				pos += AText::Convert1CharToUCS4(findtextptr,findtextlen,pos,uc);//#412 #733
			}
			else//#789
			{
				//�t���������̏ꍇ�́Apos�̈ʒu��Unicode�������擾
				AText::Convert1CharToUCS4(findtextptr,findtextlen,pos,uc);
				//pos��O��UTF-8�����J�n�ʒu�ֈړ��i�������Apos=0�Ȃ�pos=-1�ɂȂ�j
				pos = inParam.findText.GetPrevCharPos(pos,true);
			}
			
			//#166
			//�B�������@�X�y�[�X�����A����������
			if( IsIgnoreChar(uc,ignoreSpace,ignoreOnbiki) == true )
			{
				continue;
			}
			
			//���K�����Ēǉ� #166
			//#427
			AUnicodeNormalizeType	normalizeType = kUnicodeNormalizeType_D;//#863 kUnicodeNormalizeType_None;
			//
			if( fuzzy_NFKD == true )
			{
				normalizeType = kUnicodeNormalizeType_KD;
			}
			//mFindText�ցAuc�𐳋K������Unicode������ǉ�
			if( inReverse == false )
			{
				//------------------�����������̏ꍇ------------------
				if( unicodeData.CatNormalizedText(
												  mFindText,mFindTextLen,kBufferSize_TextFinder_UCS4FindText,uc,
												  normalizeType,inParam.ignoreCase,fuzzy_KanaType,inParam.ignoreBackslashYen,fuzzy_KanaYuragi,
												  fuzzySearchNormalizeArray_from,fuzzySearchNormalizeArray_to) == false )
				{
					_ACError("",this);
					return false;
				}
			}
			else
			{
				//------------------�O���������̏ꍇ------------------
				//#789
				AItemCount	len = 0;
				if( unicodeData.CatNormalizedText(
												  gFindTextBufferForReverse,len,kBufferSize_TextFinder_UCS4FindText,uc,
												  normalizeType,inParam.ignoreCase,fuzzy_KanaType,inParam.ignoreBackslashYen,fuzzy_KanaYuragi,
												  fuzzySearchNormalizeArray_from,fuzzySearchNormalizeArray_to) == false )
				{
					_ACError("",this);
					return false;
				}
				//�O���������̏ꍇ�A���]�����e�L�X�g��mFindText�Ɋi�[
				for( AIndex i = len-1; i >= 0; i-- )
				{
					mFindText[mFindTextLen] = gFindTextBufferForReverse[i];
					mFindTextLen++;
				}
			}
		}
		//BMTable������
		for( AIndex i = 0; i < kBMTableCount; i++ )
		{
			mBMTable.Set(i,0);
		}
		//BMTable�쐬
		for( AIndex pos = 0; pos < /*#733 mFindText.GetItemCount()-1*/mFindTextLen-1; pos++ )
		{
			AUCS4Char	uc = mFindText[pos];//#733 .Get(pos);
			//#166 BM�e�[�u���͈͓��̏ꍇ�݈̂ʒu�f�[�^�i�[����i�͈͊O�f�[�^�͍Ō�̃X���C�h���ɁABMTable�Q�Ƃ�����+1����j
			if( uc < kBMTableCount )
			{
				//BMTable�ɁA�e�����ɂ��āAfindText�̒��ň�ԍŌ�Ɍ����ʒu+1���i�[
				mBMTable.Set(uc,pos+1);
			}
		}
	}
	
	//=======================================================
	//�@�@2. �������C�����[�v
	//=======================================================
	//srcText�̊e�����ɑΉ����錳�e�L�X�g(inText)�̈ʒu�f�[�^
	//#733 srcText�֘A(srcText,srcTextPos,srcTextPosDecomp)��length��srcTextLen���g���B
	AItemCount	srcTextLen = 0;//#733
	//#733 AArray<AIndex>	srcTextPos;
	AIndex	srcTextPos[kBufferSize_TextFinder_UCS4FindText];//#733
	//���̃e�L�X�g(inText)�̈ꕶ�����AsrcText�����̕����ɂȂ邱�Ƃ�����̂ŁA�ꕶ�����̃I�t�Z�b�g���L��
	//#733 AArray<AIndex>	srcTextPosDecomp;
	AIndex	srcTextPosDecomp[kBufferSize_TextFinder_UCS4FindText];//#733
	//�����^�[�Q�b�g�e�L�X�g�i���̃e�L�X�g(inText)���琳�K�����Ȃ�����o���j
	//#733 AArray<AUCS4Char>	srcText;//#412
	AUCS4Char	srcText[kBufferSize_TextFinder_UCS4FindText];//#733
	AIndex	srcSposDecompPos = 0;
	//#789 for( AIndex srcSpos = inSpos; srcSpos < inEpos; )
	//�����Ώۃe�L�X�g�̊J�n�ʒu�̎擾 #789
	//�����������̏ꍇ�́AinSpos����J�n
	AIndex	srcSpos = inSpos;
	if( inReverse == true )
	{
		//�t���������̏ꍇ�́AinEpos�̈ʒu��1�O��UTF-8��������J�n
		srcSpos = inText.GetCurrentCharPos(inEpos-1);
	}
	while( srcSpos >= inSpos && srcSpos < inEpos )//#789 inText��inSpos�`inEpos�̊Ԃ�srcSpos������ԁA���[�v
	{
		//B0359
		if( inAbortFlag == true )
		{
			break;
		}
		//���[�_���Z�b�V�����p���`�F�b�N
		if( inUseModalSession == true )
		{
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Find,0,false,srcSpos-inSpos,inEpos-inSpos) == false )
			{
				outModalSessionAborted = true;
				break;
			}
		}
		
		//#733 srcTextPos.DeleteAll();
		//#733 srcTextPosDecomp.DeleteAll();
		//#733 srcText�֘A��length��srcTextLen���g���BsrcTextLen�͂��̂�������0�ɏ������B
		
		ABool	comparecheck = true;
		//=======================================================
		//�@2.1. ��������e�L�X�g��Unicode�ɕϊ����Ȃ���findTextLen�������AsrcText�Ɋi�[
		//=======================================================
		//#733 srcText.DeleteAll();
		srcTextLen = 0;//#733
		AIndex	srcLastCharPos = srcSpos;
		AIndex	decompDelete = srcSposDecompPos;
		AItemCount	findTextItemCount = mFindTextLen;//#733 mFindText.GetItemCount();
		for( ; srcTextLen < findTextItemCount;  )//srcText�̒�����mFindText�̒����ɂȂ�܂Ń��[�v
		{
			//�r����inEpos/inSpos�ɓ��B�����ꍇ�͎c���0xFFFF�Ŗ��߂�
			if( srcLastCharPos >= inEpos || srcLastCharPos < inSpos )
			{
				if( srcTextLen >= kBufferSize_TextFinder_UCS4FindText )
				{
					_ACThrow("",NULL);
				}
				else
				{
					//srcText��0xFFFF��ǉ�
					srcTextPos[srcTextLen] = srcLastCharPos;
					srcTextPosDecomp[srcTextLen] = 0;
					srcText[srcTextLen] = 0xFFFF;
					srcTextLen++;
				}
				continue;
			}
			
			//uc��pos�̈ʒu��UCS4�������擾
			AUCS4Char	uc = 0;//#412
			AItemCount	bc = AText::Convert1CharToUCS4(targettextptr,targettextlen,srcLastCharPos,uc);//#412 #733
			//cursrctextpos�ɍ���uc�̑Ώۃe�L�X�g���ʒu���L�� #789
			AIndex	cursrctextpos = srcLastCharPos;
			//srcLastCharPos���ړ�
			if( inReverse == false )
			{
				//�����������Ȃ�A���̕����̊J�n�ʒu�ֈړ�
				srcLastCharPos += bc;
			}
			else//#789
			{
				//�t���������Ȃ�A�O�̕����̊J�n�ʒu�ֈړ�
				srcLastCharPos = inText.GetPrevCharPos(srcLastCharPos,true);
			}
			
			//#166
			//�B�������@�X�y�[�X�����A�����������isrcText�֊i�[���Ȃ��j
			while( IsIgnoreChar(uc,ignoreSpace,ignoreOnbiki) == true )
			{
				bc = 0;//#477
				//�r����inEpos/inSpos�ɓ��B�����ꍇ�͂����ŏI��
				if( srcLastCharPos >= inEpos || srcLastCharPos < inSpos )   break;
				
				//���̕����ǂݍ���
				bc = AText::Convert1CharToUCS4(targettextptr,targettextlen,srcLastCharPos,uc);//#733
				//cursrctextpos�ɍ���uc�̑Ώۃe�L�X�g���ʒu���L�� #789
				cursrctextpos = srcLastCharPos;
				//
				if( inReverse == false )
				{
					//�����������Ȃ�A���̕����̊J�n�ʒu�ֈړ�
					srcLastCharPos += bc;
				}
				else//#789
				{
					//�t���������Ȃ�A�O�̕����̊J�n�ʒu�ֈړ�
					srcLastCharPos = inText.GetPrevCharPos(srcLastCharPos,true);
				}
			}
			//�X�y�[�X��������ǂݔ�΂��r����inEpos�ɓ��B�����ꍇ�͂����ŏI���iFFFF���߁��K����v���Ȃ��j
			if( bc == 0 && (srcLastCharPos >= inEpos || srcLastCharPos < inSpos) )   continue;//#477 #789
			
			//�eUnicode�����ɑΉ�����text pos��cursrctextpos�Ɋi�[���� #166 �ォ��ړ��A���AsrcLastCharPos�ł͂Ȃ�-bc�ɂ���i�Ώە����̑O�j
			if( srcTextLen >= kBufferSize_TextFinder_UCS4FindText )
			{
				_ACThrow("",NULL);
			}
			else
			{
				//�Ώۃe�L�X�g�̊Y���ʒu���L������B
				//��srcTextLen�̕ύX�͉���CatNormalizedText()�Ŏ��s�����
				//���Ώۃe�L�X�g�̈ꕶ�����A�����̕����ɂȂ���srcText�ɕۑ�����邱�Ƃ����邪�A����for���Ŋi�[�����B
				srcTextPos[srcTextLen] = cursrctextpos;//#789 srcLastCharPos-bc;
				srcTextPosDecomp[srcTextLen] = 0;//decomp��0�����ڂ��Ӗ�����
			}
			//srcText�Ɋi�[����(decomp��)�ŏ��̕����̈ʒu���L��
			AIndex	firstDecompPos = srcTextLen;//#733 srcText.GetItemCount();
			
			//���K�����Ēǉ� #166
			//#427
			AUnicodeNormalizeType	normalizeType = kUnicodeNormalizeType_D;//#863 kUnicodeNormalizeType_None;
			//
			if( fuzzy_NFKD == true )
			{
				normalizeType = kUnicodeNormalizeType_KD;
			}
			//
			if( inReverse == false )
			{
				//------------------�����������̏ꍇ------------------
				if( unicodeData.CatNormalizedText(
												  srcText,srcTextLen,kBufferSize_TextFinder_UCS4FindText,uc,
												  normalizeType,inParam.ignoreCase,fuzzy_KanaType,inParam.ignoreBackslashYen,fuzzy_KanaYuragi,
												  fuzzySearchNormalizeArray_from,fuzzySearchNormalizeArray_to) == false )
				{
					_ACError("",this);
					return false;
				}
			}
			else
			{
				//------------------�O���������̏ꍇ------------------
				//#789
				AItemCount	len = 0;
				if( unicodeData.CatNormalizedText(
												  gFindTextBufferForReverse,len,kBufferSize_TextFinder_UCS4FindText,uc,
												  normalizeType,inParam.ignoreCase,fuzzy_KanaType,inParam.ignoreBackslashYen,fuzzy_KanaYuragi,
												  fuzzySearchNormalizeArray_from,fuzzySearchNormalizeArray_to) == false )
				{
					_ACError("",this);
					return false;
				}
				//�O���������̏ꍇ�A���]�����e�L�X�g��srcText�Ɋi�[
				for( AIndex i = len-1; i >= 0; i-- )
				{
					srcText[srcTextLen] = gFindTextBufferForReverse[i];
					srcTextLen++;
				}
			}
			
			// �B�������̏ꍇ�͌��̃e�L�X�g�̈ꕶ���������̕����ɂȂ邱�Ƃ�����̂ŁAsrcTextPos, srcTextPosDecomp��ǉ�����B
			for( AIndex i = firstDecompPos+1; i < srcTextLen; i++ )
			{
				srcTextPos[i] = srcTextPos[firstDecompPos];//�ŏ���decomp�����Ɠ����Ώۃe�L�X�g���ʒu���i�[//#733 .Get(firstDecompPos);
				srcTextPosDecomp[i] = i-firstDecompPos;//decomp������index(1,2,...)���i�[
			}
			//�B�������̏ꍇ�͂��Ƃ̃e�L�X�g�̈ꕶ���̓r������J�n���邱�Ƃ�����̂ŁA�ŏ��̐��������폜
			if( decompDelete > 0 )
			{
				//decompDelete���������炵�āAlen�����炷
				for( AIndex dstPos = 0; dstPos < srcTextLen-decompDelete; dstPos++ )
				{
					srcText[dstPos] = srcText[dstPos+decompDelete];
					srcTextPos[dstPos] = srcTextPos[dstPos+decompDelete];
					srcTextPosDecomp[dstPos] = srcTextPosDecomp[dstPos+decompDelete];
				}
				srcTextLen -= decompDelete;
				//
				decompDelete = 0;
			}
		}
		//#733 srcText.Add(0);
		//#733 srcTextPos.Add(srcLastCharPos);
		//#733 srcTextPosDecomp.Add(0);
		//�Ō��0��ǉ�
		if( srcTextLen >= kBufferSize_TextFinder_UCS4FindText )
		{
			_ACThrow("",NULL);
		}
		else
		{
			srcText[srcTextLen] = 0;
			srcTextPos[srcTextLen] = srcLastCharPos;
			srcTextPosDecomp[srcTextLen] = 0;
			srcTextLen++;
		}
		
		//=======================================================
		//�@2.2. ��r
		//=======================================================
		for( AIndex i = 0; i < findTextItemCount; i++ )
		{
			if( mFindText[i] != srcText[i] )//#733 mFindText.Get(i) != srcText.Get(i) )
			{
				comparecheck = false;
				break;
			}
		}
		
		if( comparecheck == true )
		{
			ABool	wholecheck = true;
			if( inParam.wholeWord )
			{
				//��v������̍ŏ��̕����ƁA���̂P�O�̕����������p�����Ȃ�WholeWord�`�F�b�NNG
				//#789 if( srcSpos-1 >= 0 )
				//#789 {
				//#789 AUChar	ch1 = inText.Get(srcSpos-1);
				//#789 AUChar	ch2 = inText.Get(srcSpos);
				{
					AIndex	p = -1;
					if( inReverse == false )
					{
						//�������̏ꍇ�́AsrcSpos�̑O�̈ʒu���擾
						p = inText.GetPrevCharPos(srcSpos,true);
					}
					else
					{
						//�t�����̏ꍇ�́AsrcSpos�̎��̈ʒu���擾
						p = inText.GetNextCharPos(srcSpos);
					}
					//srcSpos, p�����Ƃ�inText�͈͓̔��̏ꍇ�̂݁Awhole word�`�F�b�N
					if( srcSpos >= 0 && srcSpos < inText.GetItemCount() &&
								p >= 0 && p < inText.GetItemCount() )
					{
						//UCS4�����擾
						AUCS4Char	ch1 = 0, ch2 = 0;
						AText::Convert1CharToUCS4(targettextptr,targettextlen,srcSpos,ch1);
						AText::Convert1CharToUCS4(targettextptr,targettextlen,p,ch2);
						//�����Ƃ��A���t�@�x�b�g�Ȃ�whole word�łȂ��Ɣ��f
						//#416 if( Macro_IsAlphabet(ch1) == true && Macro_IsAlphabet(ch2) == true )
						if( unicodeData.IsAlphabetOrNumberOrUnderbar(ch1) == true && unicodeData.IsAlphabetOrNumberOrUnderbar(ch2) == true )//#416
						{
							wholecheck = false;
						}
					}
					/*#416 ���܂łƎd�l���ς���Ă��܂��̂łƂ肠�����ۗ�
					AUCS4Char	ch1 = 0, ch2 = 0;
					inText.Convert1CharToUCS4(inText.GetPrevCharPos(srcSpos),ch1);
					inText.Convert1CharToUCS4(srcSpos,ch2);
					if( GetApp().NVI_GetUnicodeData().IsAlphabetOrNumber(ch1) == true &&
					GetApp().NVI_GetUnicodeData().IsAlphabetOrNumber(ch2) == true )
					{
					wholecheck = false;
					}
					*/
				}
				//��v������̍Ō�̕����ƁA���̂P���Ƃ̕����������p�����Ȃ�WholeWord�`�F�b�NNG
				//#789 if( srcLastCharPos < inText.GetItemCount() )
				//#789 {
				//#789 AUChar	ch1 = inText.Get(srcLastCharPos-1);
				//#789 AUChar	ch2 = inText.Get(srcLastCharPos);
				{
					AIndex	p = -1;
					if( inReverse == false )
					{
						//�������̏ꍇ�́AsrcLastCharPos�̑O�̈ʒu���擾
						p = inText.GetPrevCharPos(srcLastCharPos,true);
					}
					else
					{
						//�t�����̏ꍇ�́AsrcLastCharPos�̎��̈ʒu���擾
						p = inText.GetNextCharPos(srcLastCharPos);
					}
					//srcLastCharPos, p�����Ƃ�inText�͈͓̔��̏ꍇ�̂݁Awhole word�`�F�b�N
					if( srcLastCharPos >= 0 && srcLastCharPos < inText.GetItemCount() &&
								p >= 0 && p < inText.GetItemCount() )
					{
						//UCS4�����擾
						AUCS4Char	ch1 = 0, ch2 = 0;
						AText::Convert1CharToUCS4(targettextptr,targettextlen,srcLastCharPos,ch1);
						AText::Convert1CharToUCS4(targettextptr,targettextlen,p,ch2);
						//�����Ƃ��A���t�@�x�b�g�Ȃ�whole word�łȂ��Ɣ��f
						//#416 if( Macro_IsAlphabet(ch1) == true && Macro_IsAlphabet(ch2) == true )
						if( unicodeData.IsAlphabetOrNumberOrUnderbar(ch1) == true && unicodeData.IsAlphabetOrNumberOrUnderbar(ch2) == true )//#416
						{
							wholecheck = false;
						}
					}
					/*#416 ���܂łƎd�l���ς���Ă��܂��̂łƂ肠�����ۗ�
					AUCS4Char	ch1 = 0, ch2 = 0;
					inText.Convert1CharToUCS4(inText.GetPrevCharPos(srcLastCharPos),ch1);
					inText.Convert1CharToUCS4(srcLastCharPos,ch2);
					if( GetApp().NVI_GetUnicodeData().IsAlphabetOrNumber(ch1) == true &&
					GetApp().NVI_GetUnicodeData().IsAlphabetOrNumber(ch2) == true )
					{
					wholecheck = false;
					}
					*/
				}
			}
			if( wholecheck )
			{
				//��v���āAwholeWord�`�F�b�N��OK�Ȃ炻���ŏI��
				outSpos = srcSpos;
				outEpos = srcLastCharPos;
				//#166 ���������ȊO�̕���������J�n�ʒu�Ƃ��邽��
				if( /*#733 srcTextPos.GetItemCount()*/srcTextLen > 0 )
				{
					outSpos = srcTextPos[0];//#733.Get(0);
				}
				//#789
				//�t�����̏ꍇ�́AsrcSpos:�Ō�̕����̊J�n�ʒu�AsrcLastCharPos:�ŏ��̕����̑O�̕����̊J�n�ʒu�ƂȂ��Ă���̂ŁA
				//�␳����B
				if( inReverse == true )
				{
					AIndex	tmp = inText.GetNextCharPos(outSpos);
					outSpos = inText.GetNextCharPos(outEpos);
					outEpos = tmp;
				}
				//�����I��
				result = true;
				break;
			}
		}
		//=======================================================
		//�@2.3. BM�@�ɂ�莟�̃}�b�`���O�J�n�ʒu�����肷��
		//=======================================================
		//1.srcText�̍Ō�̕����ɑ�������BMTable�̒l���擾�iBMTable�̒l��0�`findTextLen-1�B���̕������Ō�Ɍ����ʒu�i�1�j�j
		//2.BMTable�̒l��0�̏ꍇ�FsrcLastCharPos�ifindTextLen�ɑ������镶���S�Ă�ǂݏI������ʒu�j�ֈړ�
		//�@BMTable�̒l��1�`findTextLen-1�̏ꍇ�FfindTextLen-bmValue�i�X���C�h���镶�����j�ɑ�������ʒu�ֈړ�
		AUCS4Char	bmChar = srcText[findTextItemCount-1];//srcText�̍Ō�̕��� #733 srcText.Get(findTextItemCount-1);
		if( bmChar < kBMTableCount )//#166
		{
			//BMTable�̒l���擾
			AIndex	bmValue = mBMTable.Get(bmChar);
			if( bmValue == 0 )//��r�Ώې�e�L�X�g�̍Ō�̕�����BMTable��0�A���Ȃ킿�A������������ɂ��̕������Ȃ�
			{
				//srcText�̍Ō�̕������A������������ɂȂ��ꍇ�A����srcText�͈͓̔��ň�v����\���͂Ȃ��̂ŁA
				//srcText�̎��̕�������A���̌������s���B
				srcSpos = srcTextPos[findTextItemCount];//#733.Get(findTextItemCount);
				srcSposDecompPos = srcTextPosDecomp[findTextItemCount];//#733 .Get(findTextItemCount);
			}
			else
			{
				//���Ɉ�v����\��������ʒu�A���Ȃ킿�AsrcText��(findTextLen-bmValue)�����ڂ̈ʒu�ֈړ�����B
				AIndex	nextSrcSpos = srcTextPos[findTextItemCount-bmValue];//#733 .Get(findTextItemCount-bmValue);
				AIndex	nextSrcSposDecompPos = srcTextPosDecomp[findTextItemCount-bmValue];//#733 .Get(findTextItemCount-bmValue);
				if( !((nextSrcSpos == srcSpos)&&(nextSrcSposDecompPos == srcSposDecompPos)) )
				{
					srcSpos = nextSrcSpos;
					srcSposDecompPos = nextSrcSposDecompPos;
				}
				else
				{
					//����A���̊J�n�ʒu���A����̊J�n�ʒu�Ɠ����������ꍇ�A�������[�v���Ȃ��悤�ɁA���^�O�̕����ֈړ�
					if( inReverse == false )
					{
						//�������̏ꍇ�́A���̕����ֈړ�
						srcSpos = inText.GetNextCharPos(srcSpos);
					}
					else//#789
					{
						//�t�����̏ꍇ�́A�O�̕����ֈړ�
						srcSpos = inText.GetPrevCharPos(srcSpos,true);
					}
					srcSposDecompPos = 0;
					_ACError("Find BMTable Error?",NULL);
				}
			}
		}
		else
		{
			//bmChar��BMTable�͈͊O�̏ꍇ�A+1�����X���C�h������ #166
			if( inReverse == false )
			{
				//�������̏ꍇ�́A���̕����ֈړ�
				srcSpos = inText.GetNextCharPos(srcSpos);
			}
			else//#789
			{
				//�t�����̏ꍇ�́A�O�̕����ֈړ�
				srcSpos = inText.GetPrevCharPos(srcSpos,true);
			}
			srcSposDecompPos = 0;
		}
	}
	//#709
	//�������ʗL��A���A�����������A���A�����q�b�g�����̏I���ʒu���e�L�X�g�̍Ō�łȂ��ꍇ�A
	//�����q�b�g�����̏I���ʒu����̉������͌������ʔ͈͂Ɋ܂߂�
	if( result == true && ignoreOnbiki == true && outEpos < inText.GetItemCount() )
	{
		//�����q�b�g�����̏I���ʒu�����UCS4�������擾
		AUCS4Char	uc = 0;
		AItemCount	len = AText::Convert1CharToUCS4(targettextptr,targettextlen,outEpos,uc);//#733
		if( IsIgnoreChar(uc,false,true) == true )
		{
			//�����������Ȃ�A���̕����������A�I���ʒu�����炷�B
			outEpos += len;
		}
	}
	//
	return result;
	
}

//���K�\������
ABool	ATextFinder::RegExpFind( const AText& inText, const AFindParameter& inParam, const ABool inReverse,//#789
		const AIndex inSpos, const AIndex inEpos, 
		AIndex& outSpos, AIndex& outEpos, const ABool& inAbortFlag, //B0359
		const ABool inUseModalSession, ABool& outModalSessionAborted )
{
	//���f�����t���O��������
	outModalSessionAborted = false;
	
	//
	AText	findText;
	findText.SetText(inParam.findText);
	/*#501 ARegExp::SetRE()���ŏ�������悤�ɕύX
	AText	yenText;
	yenText.SetLocalizedText("Text_Yen");
	AText	backslashText;
	backslashText.Add(kUChar_Backslash);
	findText.ReplaceText(yenText,backslashText);
	*/
	try//B0317����try,catch�͍폜���Ă������Ƃ͎v��
	{
		mRegExp.SetRE(findText,inParam.ignoreCase);//#930
	}
	catch(...)
	{
		ASoundWrapper::Beep();
		return false;
	}
	
	//B0056 �s�S�̂�I�������ꍇ�ɁA���̍s�̍s����^�Ɉ�v���Ă��܂����΍�
	/*if( inEnd.h == 0 && inEnd.v > 0 )
	{
		if( mText->GetChar(inepos-1) == '\r' )   inepos--;
	}*///���΍�ԈႢ�@�s�S�̂�I�����āA���̍s�̍Ō�̃��^�[���R�[�h��\r�Ɉ�v���Ȃ�(B0083)
	//B0056��URegExp::MatchRecursive�ɂčďC��
	
	ABool	result = false;
	//#789 for( AIndex spos = inSpos; spos <= inEpos; spos = inText.GetNextCharPos(spos) ) //B0047 spos<inepos��<=�ɕύX
	//�J�n�ʒu�擾
	//�����������̏ꍇ�AinSpos�̈ʒu����J�n
	AIndex	spos = inSpos;
	if( inReverse == true )
	{
		//�t���������̏ꍇ�AinEpos�̈ʒu����J�n
		spos = inEpos;
	}
	//pos��inSpos�`inEpos�̊ԁA���[�v����
	while( spos >= inSpos && spos <= inEpos )
	{
		//B0359
		if( inAbortFlag == true )
		{
			break;
		}
		//���[�_���Z�b�V�����p���`�F�b�N
		if( inUseModalSession == true )
		{
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Find,0,false,spos-inSpos,inEpos-inSpos) == false )
			{
				outModalSessionAborted = true;
				break;
			}
		}
		
		AIndex	textpos = spos;
		mRegExp.InitGroup();
		if( mRegExp.Match(inText,textpos,inEpos) )//B0050
		{
			outSpos = spos;//B0192
			outEpos = textpos;//B0192
			result = true;
			break;
		}
		
		//#789
		if( inReverse == false )
		{
			//�����������̏ꍇ�A���̕����ֈړ�
			spos = inText.GetNextCharPos(spos);
		}
		else
		{
			//�t���������̏ꍇ�A�O�̕����ֈړ�
			spos = inText.GetPrevCharPos(spos,true);
		}
		
		//spos��inText�͈̔͊O�ɂȂ�����I��
		if( spos >= inText.GetItemCount() )   break;
		if( spos < 0 )   break;
	}
	return result;
}

void	ATextFinder::ChangeReplaceText( const AText& inTargetText, const AText& inReplaceText, AText& outText ) const
{
	mRegExp.ChangeReplaceText(inTargetText,inReplaceText,outText);
}

//#166
/**
������������
*/
ABool	ATextFinder::IsIgnoreChar( const AUCS4Char inUCS4Char, const ABool isIgnoreSpaces, const ABool isIgnoreOnbiki ) const
{
	ABool	result = false;
	//�X�y�[�X����
	if( isIgnoreSpaces == true )
	{
		switch(inUCS4Char)
		{
		  case 0x0020://space
		  case 0x0009://tab
		  case 0x000D://return
		  case 0x3000://�S�pspace
		  case 0x00A0://no-break-space (normal)
		  case 0x202F://no-break-space (narrow)
		  case 0x2060://no-break-space (zero width)
			{
				result = true;
				break;
			}
		}
	}
	//������
	if( isIgnoreOnbiki == true )
	{
		switch(inUCS4Char)
		{
		  case 0x30FC://�S�p������
		  case 0xFF70://���p������
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

void	AFindParameter::Set( const AFindParameter& inSrc )
{
	findText.					SetText(inSrc.findText);
	replaceText.				SetText(inSrc.replaceText);
	ignoreCase 					= inSrc.ignoreCase;
	wholeWord 					= inSrc.wholeWord;
	fuzzy 						= inSrc.fuzzy;
	regExp 						= inSrc.regExp;
	loop 						= inSrc.loop;
	ignoreBackslashYen 			= inSrc.ignoreBackslashYen;
	filterText.					SetText(inSrc.filterText);
	recursive 					= inSrc.recursive;
	displayPosition 			= inSrc.displayPosition;
	folderPath.					SetText(inSrc.folderPath);
	//modeIndex 					= inSrc.modeIndex;
	ignoreSpaces				= inSrc.ignoreSpaces;//#165
	//B0313
	onlyVisibleFile				= inSrc.onlyVisibleFile;
	onlyTextFile				= inSrc.onlyTextFile;
}

