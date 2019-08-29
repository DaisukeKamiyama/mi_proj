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

AText

*/

#include "stdafx.h"

#include "ABase.h"
#include "../Imp.h"
#include "Cocoa.h"//#1034

#if IMPLEMENTATION_FOR_WINDOWS
#include <math.h>
#endif

#pragma mark ===========================
#pragma mark [�N���X]AText
#pragma mark ===========================

//�N���X�������̃��������蓖�ăT�C�Y
const AItemCount kTextInitialAllocateCountDefault = 32;//B0373
//���������蓖�Ēǉ����̒ǉ��T�C�Y
const AItemCount kTextReallocateStepCount = 32;//B0373

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

/**
�R���X�g���N�^
*/
AText::AText( AObjectArrayItem* inParent ) : AArray<AUChar>(inParent,kTextInitialAllocateCountDefault,kTextReallocateStepCount)//B0373
{
}

//B0373
/**
�R���X�g���N�^
*/
AText::AText( AObjectArrayItem* inParent, const AItemCount inInitialAllocateCount, const AItemCount inReallocateStepCount )
: AArray<AUChar>(inParent,inInitialAllocateCount,inReallocateStepCount)
{
}

/**
�R���X�g���N�^
*/
AText::AText( const AText& inText ) : AArray<AUChar>(NULL,kTextInitialAllocateCountDefault,kTextReallocateStepCount)//B0373
{
	SetText(inText);
}

/**
�R���X�g���N�^
*/
AText::AText( const AConstCharPtr inString ) :AArray<AUChar>(NULL,kTextInitialAllocateCountDefault,kTextReallocateStepCount)//B0373
{
	AddCstring(inString);
}

/**
�f�X�g���N�^
*/
AText::~AText()
{
}

#pragma mark ===========================

#pragma mark ---������Z�q

/**
������Z�q
*/
AText& AText::operator = ( const AText& inText ) 
{
	SetText(inText);
	return (*this);
}


#pragma mark ===========================

#pragma mark --- �e�L�X�g�|�C���^����}���^�ǉ��^�ݒ�

/*
�e�L�X�g�|�C���^����}��(unsigned char�p)
*/
void	AText::InsertFromTextPtr( const AIndex inIndex, const AConstUCharPtr inSrcPtr, const AByteCount inByteCount )
{
	//NULL�|�C���^�`�F�b�N
	if( inSrcPtr == NULL )
	{
		_ACError("",this);
		return;
	}
	//
	AItemCount	itemCount = static_cast<AItemCount>(inByteCount/sizeof(AUChar));
	if( itemCount == 0 )   return;
	//�̈�m��
	Reserve(inIndex,itemCount);
	//�R�s�[��|�C���^�擾
	AStTextPtr	textptr(*this,inIndex,itemCount);
	//�R�s�[���s
	AMemoryWrapper::Memmove(textptr.GetPtr(),inSrcPtr,textptr.GetByteCount());
}

//Insert (char�p)
void	AText::InsertFromTextPtr( const AIndex inIndex, const AConstCharPtr inSrcPtr, const AByteCount inByteCount )
{
	InsertFromTextPtr(inIndex,reinterpret_cast<AConstUCharPtr>(inSrcPtr),inByteCount);
}

//Add (unsigned char�p)
void	AText::AddFromTextPtr( const AConstUCharPtr inSrcPtr, const AByteCount inByteCount )
{
	InsertFromTextPtr(GetItemCount(),inSrcPtr,inByteCount);
}

//Add (char�p)
void	AText::AddFromTextPtr( const AConstCharPtr inSrcPtr, const AByteCount inByteCount )
{
	InsertFromTextPtr(GetItemCount(),inSrcPtr,inByteCount);
}

//Set (unsigned char�p)
void	AText::SetFromTextPtr( const AConstUCharPtr inSrcPtr, const AByteCount inByteCount )
{
	DeleteAll();
	InsertFromTextPtr(0,inSrcPtr,inByteCount);
}

//Set (char�p)
void	AText::SetFromTextPtr( const AConstCharPtr inSrcPtr, const AByteCount inByteCount )
{
	DeleteAll();
	InsertFromTextPtr(0,inSrcPtr,inByteCount);
}

void	AText::InsertFromUTF16TextPtr( const AIndex inIndex, const AUTF16CharPtr inSrcPtr, const AByteCount inByteCount )
{
	//B0000 �ǉ���S�̂ɑ΂���ConvertToUTF8FromUTF16()�������Ă��������C��
	AText	text;
	text.InsertFromTextPtr(0,reinterpret_cast<AConstUCharPtr>(inSrcPtr),inByteCount);
	text.ConvertToUTF8FromUTF16();
	InsertText(inIndex,text);
}

#pragma mark --- �e�L�X�g�|�C���^�փR�s�[

//�e�L�X�g�|�C���^�փR�s�[
//inMaxByteCount�ɃR�s�[��o�b�t�@�̍ő�T�C�Y���w�肷��K�v�L�B
void	AText::CopyToTextPtr( AUCharPtr outDstPtr, const AByteCount inMaxByteCount, const AIndex inIndex, const AItemCount inItemCount ) const
{
	AItemCount	itemCount = inItemCount;
	//�R�s�[�T�C�Y�̃`�F�b�N
	if( inMaxByteCount < itemCount*sizeof(AUChar) )
	{
		//�R�s�[�T�C�Y(inItemCount)���R�s�[��̍ő�G���A�����傫���ꍇ�A����Ɏ��܂�悤��inItemCount�𒲐�����B�iUTF-8�̕�����؂�ʒu�␳�����{�j
		AIndex	lastCharPos = inIndex+inMaxByteCount/sizeof(AUChar);
		lastCharPos = GetPrevCharPos(lastCharPos);
		itemCount = lastCharPos-inIndex;
	}
	//�R�s�[���|�C���^�擾
	AStTextPtr	textptr(*this,inIndex,itemCount);
	//�R�s�[���s
	AMemoryWrapper::Memmove(outDstPtr,textptr.GetPtr(),itemCount*sizeof(AUChar));
}

#pragma mark ===========================

#pragma mark --- C�����񂩂�}���^�ǉ��^�ݒ�

//Insert
void	AText::InsertCstring( const AIndex inIndex, const AConstCstringPtr inString )
{
	//NULL�|�C���^�`�F�b�N
	if( inString == NULL )
	{
		_ACError("",this);
		return;
	}
	//
	InsertFromTextPtr(inIndex,inString,ACstringWrapper::GetLength(inString));
}

//Add
void	AText::AddCstring( const AConstCstringPtr inString )
{
	InsertCstring(GetItemCount(),inString);
}

//Set
void AText::SetCstring( const AConstCstringPtr inString )
{
	DeleteAll();
	AddCstring(inString);
}

#pragma mark --- printf�����̑}���^�ǉ��^�ݒ�

/*#1034
void	AText::AddFormattedCstring_( const AConstCstringPtr inFormat, va_list inArgList )
{
	AByteCount len = ACstringWrapper::GetFormattedCstringLength(inFormat,inArgList);
	AIndex	insertIndex = GetItemCount();
	Reserve(insertIndex,static_cast<AItemCount>(len+1));
	{
		AStTextPtr	textptr(*this,insertIndex,len+1);
		ACstringWrapper::Sprintf(reinterpret_cast<char*>(textptr.GetPtr()),textptr.GetByteCount(),inFormat,inArgList);
	}
	//�Ō��NULL�������폜
	Delete1(GetItemCount()-1);
}
*/

void	AText::AddFormattedCstring( const AConstCstringPtr inFormat, ... )
{
	//#1034 64bit�Ή� va_list��s�x�ݒ�
	
	//�������X�g
	va_list	args;
	
	//�T�C�Y�擾
	va_start(args,inFormat);
	char	dummy;
	AByteCount len = vsnprintf(&dummy,0,inFormat,args);//�T�C�Y���������������Ƃ����ꍇ�̃T�C�Y���Ԃ�i�Ō��NULL�����͊܂܂Ȃ��j
	va_end(args);
	
	//vsnprintf������擾
	va_start(args,inFormat);
	AIndex	insertIndex = GetItemCount();
	Reserve(insertIndex,static_cast<AItemCount>(len+1));
	{
		AStTextPtr	textptr(*this,insertIndex,len+1);
		vsnprintf(reinterpret_cast<char*>(textptr.GetPtr()),textptr.GetByteCount(),inFormat,args);
	}
	//�Ō��NULL�������폜
	Delete1(GetItemCount()-1);
	va_end(args);
}

/*#1034
void	AText::SetFormattedCstring_( const AConstCstringPtr inFormat, va_list inArgList )
{
	DeleteAll();
	AddFormattedCstring_(inFormat,inArgList);
}
*/

void	AText::SetFormattedCstring( const AConstCstringPtr inFormat, ... )
{
	//�S�폜
	DeleteAll();
	
	//�ȉ��AAddFormattedCstring()�Ɠ�������
	//#1034 64bit�Ή� va_list��s�x�ݒ�
	
	//�������X�g
	va_list	args;
	
	//�T�C�Y�擾
	va_start(args,inFormat);
	char	dummy;
	AByteCount len = vsnprintf(&dummy,0,inFormat,args);//�T�C�Y���������������Ƃ����ꍇ�̃T�C�Y���Ԃ�i�Ō��NULL�����͊܂܂Ȃ��j
	va_end(args);
	
	//vsnprintf������擾
	va_start(args,inFormat);
	AIndex	insertIndex = GetItemCount();
	Reserve(insertIndex,static_cast<AItemCount>(len+1));
	{
		AStTextPtr	textptr(*this,insertIndex,len+1);
		vsnprintf(reinterpret_cast<char*>(textptr.GetPtr()),textptr.GetByteCount(),inFormat,args);
	}
	//�Ō��NULL�������폜
	Delete1(GetItemCount()-1);
	va_end(args);
}

#pragma mark --- C������+AText+C������̑g�ݍ��킹

void	AText::SetCstringAndText( const AConstCstringPtr inString1, const AText& inText, const AConstCstringPtr inString2 )
{
	SetCstring(inString1);
	AddText(inText);
	AddCstring(inString2);
}

#if IMPLEMENTATION_FOR_WINDOWS
void	AText::SetFromWcharString( const wchar_t* inString, AItemCount inMaxLength )
{
	DeleteAll();
	//security AItemCount	len = ::wcslen(inString);
	AItemCount	len = 0;
	for( ; len < inMaxLength; len++ )
	{
		if( inString[len] == 0x0000 )
		{
			break;
		}
	}
	if( len == 0 )   return;
	AddFromTextPtr(reinterpret_cast<const char*>(inString),len*sizeof(wchar_t));
	ConvertToUTF8FromAPIUnicode();
}
#endif

#pragma mark ===========================

#pragma mark --- AText����}���^�ǉ��^�ݒ�

void AText::InsertText( const AIndex inDstIndex, const AText& inSrcText, const AIndex inSrcIndex, const AItemCount inSrcItemCount )
{
	AStTextPtr	textptr(inSrcText,inSrcIndex,inSrcItemCount);
	InsertFromTextPtr(inDstIndex,textptr.GetPtr(),textptr.GetByteCount());
}

void	AText::InsertText( const AIndex inDstIndex, const AText& inSrcText )
{
	InsertText(inDstIndex,inSrcText,0,inSrcText.GetItemCount());
}

void AText::AddText( const AText& inText )
{
	InsertText(GetItemCount(),inText);
}

void AText::SetText( const AText& inText )
{
	// �R�s�[���ƁA���̃I�u�W�F�N�g�������ꍇ�͉������Ȃ� #1231 �i���������ꍇ�ɁA���̉������s����ƁADeleteAll()�őS�폜����Ă��܂��j
	if( GetDataPtrWithoutCheck() == inText.GetDataPtrWithoutCheck() )
	{
		return;
	}
	//
	DeleteAll();
	AddText(inText);
}

#pragma mark ---AText�փR�s�[
//�w��C���f�b�N�X(inIndex)����A�w�蕶����(inCount)���AoutText��Set����B
void	AText::GetText( const AIndex& inIndex, const AItemCount& inCount, AText& outText ) const
{
	outText.DeleteAll();
	outText.InsertText(0,*this,inIndex,inCount);
}

//#467
/**
�e�L�X�g���擾�i�X�y�[�X�n�����͍폜�j
*/
void	AText::GetTextWithoutSpaces( const AIndex inIndex, const AItemCount inCount, AText& outText ) const
{
	outText.DeleteAll();
	for( AIndex i = inIndex; i < inIndex+inCount; i++ )
	{
		AUChar	ch = Get(i);
		if( ch != kUChar_Space && ch != kUChar_Tab && ch != kUChar_LineEnd )
		{
			outText.Add(ch);
		}
	}
}

//#467
/**
�e�L�X�g���擾�i�X�y�[�X�n�����͍폜�j
*/
void	AText::GetTextWithoutSpaces( AText& outText ) const
{
	GetTextWithoutSpaces(0,GetItemCount(),outText);
}

#pragma mark ===========================

#pragma mark ---�폜

/*AArray�Ɉړ�
void AText::DeleteAll()
{
	Delete(0,GetItemCount());
}

void AText::DeleteAfter( const AIndex inIndex )
{
	Delete(inIndex,GetItemCount()-inIndex);
}
*/
//UTF16�̃k�������ȍ~���폜 win
void	AText::DeleteAfterUTF16NullChar()
{
	for( AIndex i = 0; i < GetItemCount(); i += sizeof(AUTF16Char) )
	{
		if( Get(i) == 0 && Get(i+1) == 0 )
		{
			DeleteAfter(i);
			return;
		}
	}
}

#pragma mark ===========================

#pragma mark ---��r

ABool	AText::Compare( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	/*#695
	AItemCount	targetItemCount = inTargetItemCount;
	if( targetItemCount == kIndex_Invalid )   targetItemCount = inTargetText.GetItemCount();
	if( GetLength() != targetItemCount )   return false;
	AStTextPtr	ptr1(*this,0,GetItemCount());
	AStTextPtr	ptr2(inTargetText,inTargetIndex,targetItemCount);
	return AMemoryWrapper::Memcmp(ptr1.GetPtr(),ptr2.GetPtr(),ptr1.GetByteCount());
	*/
	return Compare(0,GetItemCount(),inTargetText,inTargetIndex,inTargetItemCount);
}

/**
��r
#695
*/
ABool	AText::Compare( const AIndex inIndex, const AItemCount inItemCount,
		const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	//ItemCount�擾�ikIndex_Invalid�̏ꍇ��GetItemCount()���擾
	AItemCount	itemCount = inItemCount;
	if( itemCount == kIndex_Invalid )   itemCount = GetItemCount();
	AItemCount	targetItemCount = inTargetItemCount;
	if( targetItemCount == kIndex_Invalid )   targetItemCount = inTargetText.GetItemCount();
	//ItemCount���Ⴄ�ꍇ�͂�����return
	if( itemCount != targetItemCount )   return false;
	//��������r
	AStTextPtr	ptr1(*this,inIndex,itemCount);
	AStTextPtr	ptr2(inTargetText,inTargetIndex,targetItemCount);
	return AMemoryWrapper::Memcmp(ptr1.GetPtr(),ptr2.GetPtr(),ptr1.GetByteCount());
}

ABool	AText::Compare( const AConstCstringPtr inString ) const
{
	if( GetLength() != ACstringWrapper::GetLength(inString) )   return false;
	AStTextPtr	ptr(*this,0,GetItemCount());
	return AMemoryWrapper::Memcmp(ptr.GetPtr(),inString,ptr.GetByteCount());
}

ABool	AText::CompareMin( const AText& inText ) const
{
	AItemCount	count = inText.GetLength();
	if( GetLength() < count )
	{
		count = GetLength();
	}
	AStTextPtr	ptr1(*this,0,count);
	AStTextPtr	ptr2(inText,0,count);
	return AMemoryWrapper::Memcmp(ptr1.GetPtr(),ptr2.GetPtr(),ptr1.GetByteCount());
}

ABool	AText::CompareMin( const AConstCstringPtr inString ) const
{
	AItemCount	count = ACstringWrapper::GetLength(inString);
	if( GetLength() < count )
	{
		count = GetLength();
	}
	AStTextPtr	ptr1(*this,0,count);
	return AMemoryWrapper::Memcmp(ptr1.GetPtr(),inString,ptr1.GetByteCount());
}

ABool	AText::CompareIgnoreCase( const AText& inText ) const
{
	AText	text1, text2;
	text1.SetText(*this);
	text1.ChangeCaseLower();
	text2.SetText(inText);
	text2.ChangeCaseLower();
	return text1.Compare(text2);
}

ABool	AText::IsLessThan( const AText& inText ) const
{
#if IMPLEMENTATION_FOR_MACOSX
	AStCreateCFStringFromAText	cfstr1(*this);
	AStCreateCFStringFromAText	cfstr2(inText);
	return (::CFStringCompare(cfstr1.GetRef(),cfstr2.GetRef(),kCFCompareCaseInsensitive) == kCFCompareLessThan);
#elif IMPLEMENTATION_FOR_WINDOWS
	AStCreateWcharStringFromAText	str1(*this);
	AStCreateWcharStringFromAText	str2(inText);
	return (::lstrcmp(str1.GetPtr(),str2.GetPtr())<0);//������CompareString�̂ق����悢�H�������x�����i�ϊ����x���B�Ⴆ��Get�j
#else
	not implemented
#endif
}

#pragma mark ===========================
#pragma mark ---�u��
//�e�L�X�g���̎w�蕶��(inFromChar)���A�w�蕶��(inToChar)�Œu������B
void	AText::ReplaceChar( const AUChar inFromChar, const AUChar inToChar )
{
	for( AIndex pos = 0; pos < GetItemCount(); pos = GetNextCharPos(pos) )
	{
		AUChar	ch = Get(pos);
		if( ch == inFromChar )
		{
			Set(pos,inToChar);
		}
	}
}

//�e�L�X�g���̎w�蕶��(inFromChar)���AinToText�Œu������B
void	AText::ReplaceChar( const AUChar inFromChar, AText& inToText )
{
	for( AIndex pos = 0; pos < GetItemCount(); )
	{
		if( Get(pos) == inFromChar )
		{
			Delete(pos,1);
			InsertText(pos,inToText);
			pos += inToText.GetItemCount();
		}
		else
		{
			pos = GetNextCharPos(pos);
		}
	}
}

//�e�L�X�g���̎w��e�L�X�g(inFromText)���AinToText�Œu������
void	AText::ReplaceText( const AText& inFromText, const AText& inToText )
{
	for( AIndex pos = 0; pos < GetItemCount(); )
	{
		AIndex	foundPos;
		if( FindText(pos,inFromText,foundPos) == true )
		{
			Delete(foundPos,inFromText.GetItemCount());
			InsertText(foundPos,inToText);
			pos = foundPos+inToText.GetItemCount();//B0000 ���̌����͒u��������̌�납��n�߂�ׂ�
		}
		else
		{
			break;
		}
	}
}

//�e�L�X�g���̎w��e�L�X�g(inFromCstring)���AinToCstring�Œu������
void	AText::ReplaceCstring( const AConstCstringPtr inFromCstring, const AConstCstringPtr inToCstring )
{
	AText	fromText, toText;
	fromText.SetCstring(inFromCstring);
	toText.SetCstring(inToCstring);
	ReplaceText(fromText,toText);
}

/**
�e�L�X�g����"^x"�ix��inChar�Ŏw��j�������AinText�Œu������B
*/
void	AText::ReplaceParamText( const AUChar inChar, const AText& inText )
{
	for( AIndex pos = 0; pos < GetItemCount(); pos++ )
	{
		if( Get(pos) == '^' )
		{
			if( pos+1 < GetItemCount() )
			{
				if( Get(pos+1) == inChar )
				{
					Delete(pos,2);
					InsertText(pos,inText);
					//�ǉ������e�L�X�g����^X��u������Ɩ������[�v����̂ŁA�ǉ������e�L�X�g���Apos��i�߂�ifor��++����̂ŁA-1�j
					pos += inText.GetItemCount()-1;
				}
			}
		}
	}
}
/**
�e�L�X�g����"^x"�ix��inChar�Ŏw��j�������AinNumber�Œu������B
*/
void	AText::ReplaceParamText( const AUChar inChar, const ANumber inNumber )
{
	AText	text;
	text.SetNumber(inNumber);
	ReplaceParamText(inChar,text);
}

//#450
/**
�X�y�[�X�E�^�u���폜
*/
void	AText::RemoveSpaceTab()
{
	AIndex	dstpos = 0;
	{
		AStTextPtr	textptr(*this,0,GetItemCount());
		AUChar*	p = textptr.GetPtr();
		AItemCount	len = textptr.GetByteCount();
		//
		for( AIndex pos = 0; pos < len; pos++ )
		{
			AUChar	ch = p[pos];
			if( ch == kUChar_Space || ch == kUChar_Tab )
			{
				//���������i�e�L�X�g�ɂ͓���Ȃ��j
			}
			else
			{
				//�e�L�X�g�ɒǉ�
				p[dstpos] = ch;
				dstpos++;
			}
		}
	}
	DeleteAfter(dstpos);
}

//�e�L�X�g���̑啶����S�ď������ɕϊ�����
void	AText::ChangeCaseLower()
{
#if IMPLEMENTATION_FOR_MACOSX
	AStCreateCFMutableStringFromAText	strref(*this);
	::CFStringLowercase(strref.GetRef(),NULL);
	SetFromCFString(strref.GetRef());
#elif IMPLEMENTATION_FOR_WINDOWS
	AStCreateWcharStringFromAText	strref(*this);
	CharLowerW(strref.GetWritablePtr());
	SetFromWcharString(strref.GetPtr(),strref.GetWcharCount());
#else
	not implemented
#endif
}

//B0368
ABool	AText::ChangeCaseLowerFast()
{
	ABool	changed = false;//B0000
	AItemCount	itemcount = GetItemCount();//B0000
	for( AIndex pos = 0; pos < itemcount; pos++ )//B0000
	{
		AUChar	ch = Get(pos);
		if( ch >= 'A' && ch <= 'Z' )
		{
			ch += 'a'-'A';
			Set(pos,ch);
			changed = true;
		}
	}
	return changed;
}



//�e�L�X�g���̏�������S�đ啶���ɕϊ�����
void	AText::ChangeCaseUpper()
{
#if IMPLEMENTATION_FOR_MACOSX
	AStCreateCFMutableStringFromAText	strref(*this);
	::CFStringUppercase(strref.GetRef(),NULL);
	SetFromCFString(strref.GetRef());
	return;
#elif IMPLEMENTATION_FOR_WINDOWS
	AStCreateWcharStringFromAText	strref(*this);
	CharUpperW(strref.GetWritablePtr());
	SetFromWcharString(strref.GetPtr(),strref.GetWcharCount());
#else
	not implemented
#endif
	
}

/**
�w�蕶�����w����J��Ԃ����e�L�X�g��ݒ�
*/
void	AText::SetRepeatedCstring( const AConstCstringPtr inCstring, const AItemCount inCount )
{
	DeleteAll();
	for( AIndex i = 0; i < inCount; i++ )
	{
		AddCstring(inCstring);
	}
}

//#914
/**
modifier�L�[��\���e�L�X�g��ݒ�
*/
void	AText::AddModifiersKeyText( const AModifierKeys inModifierKeys )
{
	//
	if( (inModifierKeys&kModifierKeysMask_Shift) != 0 )
	{
		AddLocalizedText("ShiftKey");
	}
	if( (inModifierKeys&kModifierKeysMask_Control) != 0 )
	{
		AddLocalizedText("ControlKey");
	}
	if( (inModifierKeys&kModifierKeysMask_Option) != 0 )
	{
		AddLocalizedText("OptionKey");
	}
	if( (inModifierKeys&kModifierKeysMask_Command) != 0 )
	{
		AddLocalizedText("CommandKey");
	}
}

#pragma mark ===========================
#pragma mark ---�e�L�X�g���
//���s�ʒu���ǂ����𔻒肵�A���s�ʒu�Ȃ玟�̍s�̍ŏ��ʒu��outPosition�Ɋi�[���Atrue��Ԃ��B���s�ʒu�łȂ��Ȃ�AoutPosition�ύX�����Afalse��Ԃ��B
ABool	AText::IsLineEnd( const AIndex inPosition, AIndex& outPosition ) const
{
	AUChar	ch = Get(inPosition);
	if( ch == kUChar_CR )
	{
		if( inPosition+1 < GetItemCount() )
		{
			ch = Get(inPosition+1);
			if( ch == kUChar_LF )
			{
				outPosition = inPosition+2;
				return true;
			}
		}
		outPosition = inPosition+1;
		return true;
	}
	if( ch == kUChar_LF )
	{
		outPosition = inPosition+1;
		return true;
	}
	return false;
}

/**
�P�s��outText�֓ǂݍ��݁AioPosition�����̍s���ֈړ�����
�Ԃ�AText�ɂ͉��s�R�[�h�͊܂܂Ȃ�
*/
void	AText::GetLine( AIndex& ioPosition, AText& outText ) const
{
	AIndex	pos = ioPosition;
	for( ; pos < GetItemCount(); pos++ )
	{
		AIndex	lineEndPos;
		if( IsLineEnd(pos,lineEndPos) == true )
		{
			//�Ԃ�AText�ɂ͉��s�R�[�h�͊܂܂Ȃ�
			GetText(ioPosition,pos-ioPosition,outText);
			//���̍s���ֈړ�
			ioPosition = lineEndPos;
			return;
		}
	}
	//���s�R�[�h�Ȃ��̏ꍇ
	GetText(ioPosition,pos-ioPosition,outText);
	ioPosition = pos;
}

//ioPosition�����̍s���ֈړ�����
void	AText::GoForwardToNextLineStart( AIndex& ioPosition ) const
{
	for( ; ioPosition < GetItemCount(); ioPosition++ )
	{
		AIndex	lineEndPos;
		if( IsLineEnd(ioPosition,lineEndPos) == true )
		{
			ioPosition = lineEndPos;
			return;
		}
	}
}

/**
�w�蕶��(inChar)��������܂ŁAioPosition��߂��B
������Ȃ��ꍇ�AioPosition��-1�ɂȂ�
*/
ABool	AText::GoBackToChar( AIndex& ioPosition, const AUChar inChar ) const //#427
{
	ABool	result = false;//#427
	for( ; ioPosition >= 0 ; ioPosition-- )//#427 �����������ǂ�����Ԃ����߂ɁA>��>=�ɕύX�B
	{
		if( Get(ioPosition) == inChar )
		{
			result = true;//#427
			break;
		}
	}
	return result;//#427
}

/**
�w�蕶��(inChar)��������܂ŁAioPosition��i�߂�
������Ȃ��ꍇ�AioPosition��itemcount-1�ɂȂ�
*/
ABool	AText::GoForwardToChar( AIndex& ioPosition, const AUChar inChar ) const //#427
{
	ABool	result = false;//#427
	for( ; ioPosition < GetItemCount(); ioPosition++ )
	{
		if( Get(ioPosition) == inChar )
		{
			result = true;//#427
			break;
		}
	}
	return result;//#427
}

//�w��f���~�^(inDelimiter)�ŕ��������؂�AoutTextArray�ɏ��Ɋi�[����B
//�f���~�^��2�����A������ꍇ�́A��AText���i�[�����
//�Ō�̕������f���~�^�̏ꍇ�́A�Ō�ɋ�AText���i�[�����
void	AText::Explode( const AUChar inDelimiter, ATextArray& outTextArray ) const
{
	/*#693 ATextArray::ExplodeFromText()�ƃR�[�h�d���̂��߁A������R�[������悤�ɕύX
	outTextArray.DeleteAll();
	AIndex	spos = 0;
	AIndex pos = 0;
	for( ; pos < GetItemCount(); pos++ )
	{
		AUChar	ch = Get(pos);
		if( ch == inDelimiter )
		{
			AText	text;
			GetText(spos,pos-spos,text);
			outTextArray.Add(text);
			spos = pos+1;
		}
	}
	AText	text;
	GetText(spos,pos-spos,text);
	outTextArray.Add(text);
	*/
	outTextArray.ExplodeFromText(*this,inDelimiter);
}

#pragma mark ===========================
#pragma mark ---UTF-8�����񑀍�

/**
UTF-8��1������P�ʂƂ��āA1�����O�̈ʒu�𓾂�
@note ����inReturnMinusWhen0�ɂ���āAinPos=0�̏ꍇ��-1��Ԃ����A0��Ԃ�����I���\�B�i�f�t�H���g�Ffalse(0��Ԃ�)�j
*/
AIndex	AText::GetPrevCharPos( const AIndex inPos, const ABool inReturnMinusWhen0 ) const
{
	//#789
	//inReturnMinusWhen0��true�̏ꍇ�AinPos=0�̏ꍇ��-1��Ԃ��B�iinReturnMinusWhen0��false�̏ꍇ�́A0��Ԃ��j
	if( inReturnMinusWhen0 == true )
	{
		if( inPos <= 0 )
		{
			return inPos-1;
		}
	}
	//inPos��0�Ȃ�A0��Ԃ�
	if( inPos <= 0 )   return inPos;
	//
	AIndex	pos = inPos-1;
	for( ; pos > 0; pos--)
	{
		AUChar	ch =  Get(pos);
		if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )   break;
	}
	return pos;
}
AIndex	AText::GetPrevCharPos( const AUChar* inTextPtr, const AItemCount inTextLen,
		const AIndex inPos, const ABool inReturnMinusWhen0 )
{
	//#789
	//inReturnMinusWhen0��true�̏ꍇ�AinPos=0�̏ꍇ��-1��Ԃ��B�iinReturnMinusWhen0��false�̏ꍇ�́A0��Ԃ��j
	if( inReturnMinusWhen0 == true )
	{
		if( inPos <= 0 )
		{
			return inPos-1;
		}
	}
	//inPos��0�Ȃ�A0��Ԃ�
	if( inPos <= 0 )   return inPos;
	//
	AIndex	pos = inPos-1;
	for( ; pos > 0; pos--)
	{
		AUChar	ch =  inTextPtr[pos];
		if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )   break;
	}
	return pos;
}

//UTF-8��1������P�ʂƂ��āA1������̈ʒu�𓾂�
AIndex	AText::GetNextCharPos( const AIndex inPos ) const
{
	if( inPos == GetItemCount() )   return inPos;
	AIndex	pos = inPos+1;
	for( ; pos < GetItemCount(); pos++ )
	{
		AUChar	ch =  Get(pos);
		if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )   break;
	}
	return pos;
}
AIndex	AText::GetNextCharPos( const AUChar* inTextPtr, const AItemCount inTextLen, const AIndex inPos )
{
	if( inPos == inTextLen )   return inPos;
	AIndex	pos = inPos+1;
	for( ; pos < inTextLen; pos++ )
	{
		AUChar	ch =  inTextPtr[pos];
		if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )   break;
	}
	return pos;
}

//UTF-8��1������P�ʂƂ��āA���݂̕����̍ŏ��̃o�C�g�ʒu�𓾂�
AIndex	AText::GetCurrentCharPos( const AIndex inPos ) const
{
	if( inPos == 0 )   return inPos;
	if( inPos == GetItemCount() )   return inPos;
	AIndex	pos = inPos;
	for( ; pos > 0; pos--)
	{
		AUChar	ch =  Get(pos);
		if( ( IsUTF8Multibyte(ch) == false ) || ( IsUTF8Multibyte1(ch) == true ) )   break;
	}
	return pos;
}

ABool	AText::IsUTF8FirstByteInMultiByte( const AIndex inPos ) const
{
	return IsUTF8Multibyte1(Get(inPos));
}
ABool	AText::IsUTF8FirstByteInMultiByte( const AUChar inChar ) const
{
	return IsUTF8Multibyte1(inChar);
}

ABool	AText::IsUTF8MultiByte( const AIndex inPos ) const
{
	return IsUTF8Multibyte(Get(inPos));
}
ABool	AText::IsUTF8MultiByte( const AUChar inChar ) const
{
	return IsUTF8Multibyte(inChar);
}

//Unicode�ꕶ������UTF8�}���`�o�C�g����Ԃ�
AItemCount	AText::GetUTF8ByteCount( const AIndex inPos ) const
{
	return GetUTF8ByteCount(Get(inPos));
}
AItemCount	AText::GetUTF8ByteCount( const AUChar inChar ) const
{
	//#412 GetUTF8ByteCount()�̐��ۂ�Ԃ����[�`�����쐬
	AItemCount	byteCount = 1;
	GetUTF8ByteCount(inChar,byteCount);
	return byteCount;
}
ABool	AText::GetUTF8ByteCount( const AUChar inChar, AItemCount& outByteCount ) const//#412
{
	ABool	result = true;
	if( (inChar&0x80) == 0 )   outByteCount = 1;
	else if( (inChar&0xE0) == 0xC0 )   outByteCount = 2;
	else if( (inChar&0xF0) == 0xE0 )   outByteCount = 3;
	else if( (inChar&0xF8) == 0xF0 )   outByteCount = 4;
	//else if( (inChar&0xFC) == 0xF8 )   return 5; Unicode��U+10FFFF�܂łȂ̂ŁAUTF-8��4�o�C�g�܂�
	//else if( (inChar&0xFE) == 0xFC )   return 6;
	else
	{
		//UTF-8��2�o�C�g�ڈȍ~�Ȃ�
		outByteCount = 1;//#412
		result = false;
	}
	return result;
}

void	AText::Get1UTF8Char( AIndex& ioIndex, AText& outChar ) const
{
	AItemCount	count = GetUTF8ByteCount(ioIndex);
	GetText(ioIndex,count,outChar);
	ioIndex += count;
}

AItemCount	AText::Convert1CharToUTF16( const AIndex inIndex, AUTF16Char& outUniChar ) const
{
	unsigned char	ch1,ch2,ch3;
	ch1 = Get(inIndex);
	if( (ch1&0x80) == 0 )
	{
		outUniChar = ch1;
		return 1;
	}
	else if( (ch1&0xE0) == 0xC0 )
	{
		//B0000
		if( inIndex+1 >= GetItemCount() )
		{
			outUniChar = 0;
			return GetItemCount()-inIndex;
		}
		
		ch2 = Get(inIndex+1);
		outUniChar = ((ch1&0x1C)>>2)*0x100 + ((ch1&0x03)<<6)+((ch2&0x3F));
		return 2;
	}
	else if( (ch1&0xF0) == 0xE0 )
	{
		//B0000
		if( inIndex+2 >= GetItemCount() )
		{
			outUniChar = 0;
			return GetItemCount()-inIndex;
		}
		
		ch2 = Get(inIndex+1);
		ch3 = Get(inIndex+2);
		outUniChar = (((ch1&0x0F)<<4)+((ch2&0x3C)>>2))*0x100 + ((ch2&0x03)<<6)+((ch3&0x3F));
		return 3;
	}
	else
	{
		outUniChar = 0;
		return 4;
	}
}

//#412
/**
�w��index�ȍ~��Unicode�ꕶ����UCS4�Ŏ擾
*/
AItemCount	AText::Convert1CharToUCS4( const AIndex inIndex, AUCS4Char& outUniChar ) const
{
	AStTextPtr	textptr(*this,0,GetItemCount());
	return AText::Convert1CharToUCS4(textptr.GetPtr(),textptr.GetByteCount(),inIndex,outUniChar);
}
/**
�w��index�ȍ~��Unicode�ꕶ����UCS4�Ŏ擾�i�e�L�X�g�|�C���^�w��Estatic�j
#733 �����������̂��߃e�L�X�g�|�C���^�w��ł��쐬
*/
AItemCount	AText::Convert1CharToUCS4( const AUChar* inTextPtr, const AItemCount inTextLen,
			const AIndex inIndex, AUCS4Char& outUniChar ) //#733
{
	unsigned char	ch1,ch2,ch3,ch4;
	ch1 = inTextPtr[inIndex];//#733 Get(inIndex);
	if( (ch1&0x80) == 0 )
	{
		outUniChar = ch1;
		return 1;
	}
	else if( (ch1&0xE0) == 0xC0 )
	{
		if( inIndex+1 >= /*#733 GetItemCount()*/inTextLen )
		{
			outUniChar = 0;
			return /*#733 GetItemCount()*/inTextLen -inIndex;
		}
		
		ch2 = inTextPtr[inIndex+1];//#733 Get(inIndex+1);
		outUniChar = ((ch1&0x1C)>>2)*0x100 + ((ch1&0x03)<<6)+((ch2&0x3F));
		return 2;
	}
	else if( (ch1&0xF0) == 0xE0 )
	{
		if( inIndex+2 >= /*#733 GetItemCount()*/inTextLen )
		{
			outUniChar = 0;
			return /*#733 GetItemCount()*/inTextLen -inIndex;
		}
		
		ch2 = inTextPtr[inIndex+1];//#733 Get(inIndex+1);
		ch3 = inTextPtr[inIndex+2];//#733 Get(inIndex+2);
		outUniChar = (((ch1&0x0F)<<4)+((ch2&0x3C)>>2))*0x100 + ((ch2&0x03)<<6)+((ch3&0x3F));
		return 3;
	}
	else if( (ch1&0xF8) == 0xF0 )
	{
		if( inIndex+3 >= /*#733 GetItemCount()*/inTextLen )
		{
			outUniChar = 0;
			return /*#733 GetItemCount()*/inTextLen -inIndex;
		}
		
		ch2 = inTextPtr[inIndex+1];//#733 Get(inIndex+1);
		ch3 = inTextPtr[inIndex+2];//#733 Get(inIndex+2);
		ch4 = inTextPtr[inIndex+3];//#733 Get(inIndex+3);
		outUniChar = (((ch1&0x07)<<2)+((ch2&0x30)>>4))*0x10000 +
				(((ch2&0x0F)<<4)+((ch3&0x3C)>>2))*0x100 + ((ch3&0x03)<<6)+((ch4&0x3F));
		return 4;
	}
	else
	{
		outUniChar = 0;
		return 1;
	}
}

//#427
/**
UCS4������}��
*/
void	AText::InsertFromUCS4Char( const AIndex inIndex, const AUCS4Char inUniChar )
{
	if( inUniChar < 0x0080 )
	{
		Insert1(inIndex,inUniChar);
	}
	else if( inUniChar < 0x0800 )
	{
		Insert1(inIndex  ,0xC0|((inUniChar&0x07C0)>>6));
		Insert1(inIndex+1,0x80|((inUniChar&0x003F)));
	}
	else if( inUniChar < 0x10000 )
	{
		Insert1(inIndex  ,0xE0|((inUniChar&0xF000)>>12));
		Insert1(inIndex+1,0x80|((inUniChar&0x0FC0)>>6));
		Insert1(inIndex+2,0x80|((inUniChar&0x003F)));
	}
	else if( inUniChar < 0x200000 )
	{
		Insert1(inIndex  ,0xF0|((inUniChar&0x1C0000)>>18));
		Insert1(inIndex+1,0x80|((inUniChar&0x03F000)>>12));
		Insert1(inIndex+2,0x80|((inUniChar&0x000FC0)>>6));
		Insert1(inIndex+3,0x80|((inUniChar&0x00003F)));
	}
	else
	{
		_ACError("invalid UCS4 character",this);
	}
}

/**
UCS4������ǉ�
*/
void	AText::AddFromUCS4Char( const AUCS4Char inUniChar )
{
	InsertFromUCS4Char(GetItemCount(),inUniChar);
}

#pragma mark ===========================
//�w��f���~�^(inDelimiter)�܂ŁA�������́A���s�܂ł̕������outText�Ɏ擾���AioPosition��i�߂�B
/*R-abs void AText::GetTokenBeforeLineEnd( AIndex& ioPosition, AText& outText, const AUChar inDelimiter ) const
{
	outText.DeleteAll();
	
	while( inPosition < GetItemCount() )
	{
		unsigned char	ch = Get(inPosition);
		if( ch == A_CHAR_CR || ch == A_CHAR_LF )
		{
			break;
		}
		inPosition++;
		if( ch == inDelimiter )                                                                                                                                                             
		{
			break;
		}
		outText.Add(ch);
	}
}*/

//�g���q�擾�i
void AText::GetSuffix( AText& outText ) const
{
	outText.DeleteAll();
	if( GetLength() == 0 )   return;
	AIndex	pos = GetItemCount()-1;
	if( GoBackToChar(pos,A_CHAR_PERIOD) == false )   return;//#427 .��������Ȃ��ꍇ��return
	if( pos >= GetItemCount() )   return;
	if( Get(pos) != '.' )   return;
	GetText(pos,GetItemCount()-pos,outText);
}
AIndex	AText::GetSuffixStartPos() const
{
	if( GetLength() == 0 )   return 0;
	for( AIndex	pos = GetItemCount()-1; pos >= 0; pos-- )
	{
		if( Get(pos) == kUChar_Period )   return pos;
	}
	return GetItemCount();
}
//R0137
AIndex	AText::GetFirstSuffixStartPos() const
{
	for( AIndex	pos = 0; pos < GetItemCount(); pos++ )
	{
		if( Get(pos) == kUChar_Period )   return pos;
	}
	return GetItemCount();
}

/**
token�擾
*/
void AText::GetToken( AIndex& ioPosition, AText& outText ) const
{
	AStTextPtr	textptr(*this,0,GetItemCount());
	GetToken(textptr.GetPtr(),textptr.GetByteCount(),ioPosition,outText);
}
void AText::GetToken( const AUChar* inTextPtr, const AItemCount inTextLen,
					 AIndex& ioPosition, AText& outText ) 
{
	outText.DeleteAll();
	
	if( ioPosition >= inTextLen )   return;
	AUChar	ch = inTextPtr[ioPosition];
	ioPosition++;
	if( ch == kUChar_Space || ch == kUChar_Tab )
	{
		while( ioPosition < inTextLen )
		{
			ch = inTextPtr[ioPosition];
			ioPosition++;
			if( ch == kUChar_Space || ch == kUChar_Tab )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	outText.Add(ch);
	if( (((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_'||ch=='-')) == false )
	{
		return;
	}
	while( ioPosition < inTextLen )
	{
		AUChar	ch = inTextPtr[ioPosition];
		if( (((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_'||ch=='-')) == false )
		{
			break;
		}
		outText.Add(ch);
		ioPosition++;
	}
}

void	AText::GetTextToTabSpaceLineEnd( AIndex& ioPos, AText& outText ) const
{
	outText.DeleteAll();
	//
	if( ioPos >= GetItemCount() )   return;
	//���݈ʒu�̃X�y�[�X�E�^�u�E���s�͂Ƃ΂�
	AUChar	ch = Get(ioPos);
	if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_CR || ch == kUChar_LF )
	{
		ioPos++;
		for( ; ioPos < GetItemCount(); ioPos++ )
		{
			ch = Get(ioPos);
			if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_CR || ch == kUChar_LF )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	if( ioPos >= GetItemCount() )   return;
	//�J�n�ʒu����
	AIndex	spos = ioPos;
	//
	ioPos++;
	for( ; ioPos < GetItemCount(); ioPos++ )
	{
		ch = Get(ioPos);
		if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_CR || ch == kUChar_LF )
		{
			break;
		}
		else
		{
			continue;
		}
	}
	//
	GetText(spos,ioPos-spos,outText);
}

void AText::GoBackGetToken( AIndex& inPosition, AText& outText ) const
{
	outText.DeleteAll();
	
	inPosition--;
	if( inPosition < 0 )   return;
	unsigned char	ch = Get(inPosition);
	if( ch == A_CHAR_SPACE || ch == A_CHAR_TAB )
	{
		while( inPosition > 0 )
		{
			inPosition--;
			ch = Get(inPosition);
			if( ch == A_CHAR_SPACE || ch == A_CHAR_TAB )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	outText.Add(ch);
	if( (((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_'||ch=='-')) == false )
	{
		return;
	}
	while( inPosition > 0 )
	{
		inPosition--;
		unsigned char	ch = Get(inPosition);
		if( (((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_'||ch=='-')) == false )
		{
			inPosition++;
			break;
		}
		outText.Insert1(0,ch);
	}
}

/**
inStartPos�ȍ~����inLength�o�C�g���A�X�y�[�X�����ȊO�̕������擾����
*/
AIndex	AText::GetCharactersOtherThanSpaces( const AUChar* inTextPtr, const AItemCount inTextLen,
											const AIndex inStartPos, const AItemCount inLength, AText& outText ) 
{
	//���ʏ�����
	outText.DeleteAll();
	//�������̃��[�v
	AIndex pos = inStartPos;
	for( ; pos < inTextLen; pos++ )
	{
		//�����擾
		AUChar	ch = inTextPtr[pos];
		if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
		{
			//�X�y�[�X�����Ȃ牽���������[�v�p��
			continue;
		}
		else
		{
			//���ʂɕ����ǉ�
			outText.Add(ch);
			//�w�蕶�����擾������I��
			if( outText.GetItemCount() == inLength )
			{
				pos++;
				break;
			}
		}
	}
	return pos;
}

#pragma mark ===========================

ANumber	AText::GetIntegerByHexChar( const AIndex inIndex ) const
{
	AUChar	ch = Get(inIndex);
	if( ch >= '0' && ch <= '9' )
	{
		ch -= '0';
	}
	else if( ch >= 'A' && ch <= 'F' )
	{
		ch = ch-'A'+10;
	}
	else if( ch >= 'a' && ch <= 'f' )
	{
		ch = ch-'a'+10;
	}
	else 
	{
		return 0;
	}
	return ch;
}

AUInt32	AText::GetIntegerFromHexText() const
{
	AUInt32	result = 0;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		result = result * 0x10;
		result += GetIntegerByHexChar(i);
	}
	return result;
}

#pragma mark ���K��

void AText::Normalize()
{
	NormalizeLineEnd();
	//
}

void AText::NormalizeLineEnd()
{
	ConvertLineEndTo(A_CHAR_LineEnd);
}


//#200 ���\�b�h���ύX AddTextFromUTF16CharPtr��SetTextFromUTF16CharPtr  �Ō��ConvertToUTF8FromUTF16()�����Ă��邩��AAdd�Ƃ��Ă͋@�\���Ȃ�
/**
UTF16Char����Text��ݒ肷��
*/
void AText::SetTextFromUTF16CharPtr( AUTF16Char* inPtr, unsigned long inLength )
{
	DeleteAll();//#200 ���\�b�h���ύXAddTextFromUTF16CharPtr��SetTextFromUTF16CharPtr�ɔ��������ǉ�
	InsertFromTextPtr(GetItemCount(),reinterpret_cast<AConstUCharPtr>(inPtr),inLength);
	//Insert(GetItemCount(),reinterpret_cast<const unsigned char*>(inPtr),inLength);
	ConvertToUTF8FromUTF16();
}

void	AText::AddNumber( long inNumber )
{
	char	str[256];
	sprintf(str,"%ld",inNumber);
	AddCstring(str);
}

void	AText::SetNumber( ANumber inNumber )
{
	DeleteAll();
	AddNumber(inNumber);
}

//#427
/**
64bit���l���e�L�X�g�ɕϊ����Ēǉ�
*/
void	AText::AddNumber64bit( const long long inNumber )
{
	char	str[256];
	sprintf(str,"%lld",inNumber);
	AddCstring(str);
}

//#427
/**
64bit���l���e�L�X�g�ɕϊ����Đݒ�
*/
void	AText::SetNumber64bit( const ANumber64bit inNumber )
{
	DeleteAll();
	AddNumber64bit(inNumber);
}

AUChar	AText::GetLastChar() const
{
	if( GetItemCount() == 0 )   return 0;
	else return Get(GetItemCount()-1);
}
#pragma mark �Q��
/*B0000 ������
AIndex AText::GetLength() const
{
	return GetItemCount();
}
*/
//�擾


#pragma mark ---������e�푀��

void	AText::LimitLength( const AItemCount inMin, const AItemCount inMax )
{
	while( GetLength() < inMin )
	{
		Add(' ');
	}
	if( GetLength() > inMax )
	{
		AIndex	pos = GetCurrentCharPos(inMax);
		DeleteAfter(pos);
	}
}

void	AText::LimitLengthAsSJIS( const AItemCount& inMin, const AItemCount& inMax )
{
	while( GetLength() < inMin )
	{
		Add(' ');
	}
	if( GetLength() > inMax )
	{
		AIndex pos = 0;
		for( ; pos < GetItemCount(); )
		{
			AUChar	c = Get(pos);
			if( ((c>=0x81&&c<=0x9F)||(c>=0xE0&&c<=0xFC)) )
			{
				if( pos+2 >= inMax )   break;
				pos += 2;
			}
			else
			{
				if( pos+1 >= inMax )   break;
				pos++;
			}
		}
		DeleteAfter(pos);
	}
}

//#723
/**
�������������i�����Ƃ���ellipsis�ŏȗ��j
*/
void	AText::LimitLengthWithEllipsis( const AItemCount inMax )
{
	if( GetItemCount() > inMax )
	{
		AIndex	pos = GetCurrentCharPos(inMax-3);
		DeleteAfter(pos);
		AddCstring("...");
	}
}

ABool	AText::IsCharContained( unsigned char inChar )
{
	AIndex pos = 0;
	while( pos < GetItemCount() )
	{
		unsigned char	ch = Get(pos);
		if( ch == inChar )
		{
			return true;
		}
		pos++;
	}
	return false;
}

//R0208
ABool	AText::IsCharContained( const AUChar inChar, const AIndex inStartIndex, const AIndex inEndIndex ) const
{
	for( AIndex pos = inStartIndex; pos < inEndIndex; pos++ )
	{
		AUChar	ch = Get(pos);
		if( ch == inChar )
		{
			return true;
		}
		//B0399 pos++;
	}
	return false;
}

//#695
/**
�w��͈͓��Ɋ܂܂��inChar�̐����J�E���g����
*/
AItemCount	AText::GetCountOfChar( const AUChar inChar ) const
{
	return GetCountOfChar(inChar,0,GetItemCount());
}
AItemCount	AText::GetCountOfChar( const AUChar inChar, const AIndex inIndex, const AItemCount inCount ) const
{
	AItemCount	count = 0;
	AStArrayPtr<AUChar>	arrayptr(*this,inIndex,inCount);
	AUChar*	p = arrayptr.GetPtr();
	for( AIndex i = 0; i < inCount; i++ )
	{
		if( p[i] == inChar )
		{
			count++;
		}
	}
	return count;
}

//#1050
/**
�e�L�X�g���Ɏw�蕶���񂪊܂܂�邩�ǂ�����Ԃ�
*/
ABool	AText::Contains( const AConstCstringPtr inStringPtr ) const
{
	AIndex	pos = 0;
	if( FindCstring(0,inStringPtr,pos) == true )
	{
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark ���s�R�[�h����

/**
���s�R�[�h�ϊ�
*/
void AText::ConvertLineEndTo( const AUChar inChar )
{
	//�ǂݎ��ʒu�A�������݈ʒu������
	AIndex srcpos = 0, dstpos = 0;
	{
		//�|�C���^�擾
		AStTextPtr	textptr(*this,0,GetItemCount());
		AUChar*	p = textptr.GetPtr();
		AItemCount	origlen = GetItemCount();
		
		//�o�C�g���̃��[�v
		while( srcpos < origlen )
		{
			//�ǂݎ��
			AUChar	ch = p[srcpos];
			srcpos++;
			//�����ɂ���ĕ���
			switch(ch)
			{
				//CR�̏ꍇ
			  case kUChar_CR:
				{
					//�w����s�R�[�h����������
					p[dstpos] = inChar;
					dstpos++;
					//���̕�����ǂݎ���āALF�Ȃ炻�̕����͓ǂݔ�΂�
					if( srcpos < origlen )
					{
						if( p[srcpos] == kUChar_LF )
						{
							srcpos++;
						}
					}
					break;
				}
				//LF�̏ꍇ
			  case kUChar_LF:
				{
					//�w����s�R�[�h����������
					p[dstpos] = inChar;
					dstpos++;
					break;
				}
				//���̑��̕����̏ꍇ
			  default:
				{
					//�ǂݎ�����o�C�g�����̂܂܏�������
					p[dstpos] = ch;
					dstpos++;
					break;
				}
			}
		}
	}
	//�������񂾍Ō�ȍ~�͍폜
	DeleteAfter(dstpos);
}

void AText::ConvertLineEndToLF()
{
	ConvertLineEndTo(kUChar_LF);
}

void AText::ConvertLineEndToCR()
{
	ConvertLineEndTo(kUChar_CR);
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�G���R�[�f�B���O�^���s�R�[�h����

/*�s�܂�Ԃ��͐��������A�h�L�������g�ݒ肪����΂��̐ݒ�A������΃��[�h�ݒ�Őݒ肵���l���g���B���s�R�[�h��ConvertReturnCodeToCR()�ŔF���B
void	AText::GuessReturnCodeAndWrap( AReturnCode& outReturnCode, ABool& outWrap ) const
{
	AIndex	end = 5*1024;//�ŏ���5KB
	if( end > GetItemCount() )   end = GetItemCount();
	ANumber	oneline = 0;
	ANumber	over100 = 0;
	ANumber	cr = 0, crnl = 0, nl = 0;
	AUChar ch, ch2;
	for( AIndex pos = 0; pos < end; pos++ ) 
	{
		oneline++;
		ch = Get(pos);
		if( pos+1 < end )
		{
			ch2= Get(pos+1);
		}
		else
		{
			ch2 = 0x20;
		}
		if( ch == kUChar_CR && ch2 == kUChar_LF ) 
		{
			crnl++;
			pos++;
			if( oneline > 100 ) 
			{
				over100 += oneline;
			}
			oneline = 0;
		}
		else if( ch == kUChar_CR ) 
		{
			cr++;
			if( oneline > 100 ) 
			{
				over100 += oneline;
			}
			oneline = 0;
		}
		else if( ch == 10 ) 
		{
			nl++;
			if( oneline > 100 ) 
			{
				over100 += oneline;
			}
			oneline = 0;
		}
	}
	if( over100 > GetItemCount()/10 ) 
	{
		outWrap = true;
	}
	else 
	{
		outWrap = false;
	}
	if( nl >= 1 )   outReturnCode = returnCode_LF;
	else if( crnl >= 1 )   outReturnCode = returnCode_CRLF;
	else outReturnCode = returnCode_CR;
}
*/

//B0390 ioTextEncoding�̓R�[�����ŏ����l��ݒ肳��Ă��Ȃ���΂Ȃ�Ȃ�
ABool	AText::GuessTextEncoding( ATextEncoding& ioTextEncoding ) const
{
	//B0390 outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
	//�@BOM
	ATextEncoding	tec = ioTextEncoding;//B0390
	//#764 if( inUseBOM == true )
	{
		if( GuessTextEncodingFromBOM(tec) == true )//B0390
		{
			ioTextEncoding = tec;//B0390
			return true;
		}
	}
	//�Acharset
	//#764 if( inUseCharset == true )
	{
		if( GuessTextEncodingFromCharset(tec) == true )//B0390
		{
			ioTextEncoding = tec;//B0390
			return true;
		}
	}
	//�B�]������
	//#764 if( inUseLegacy == true )
	{
		if( GuessTextEncodingByAnalysis(tec) == true )//B0390
		{
			ioTextEncoding = tec;//B0390
			return true;
		}
	}
	/*#764
	//B0166
	//�S��ascii�R�[�h���Ȃ�ioTextEncoding�͏����l�̂܂܂ŁA���A���ʂ�true
	ABool	onlyAscii = true;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		if( Get(i) >= 0x80 )
		{
			onlyAscii = false;
			break;
		}
	}
	if( onlyAscii == true )
	{
		//B0390 outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
		return true;
	}
	//B0390
	if( inSetDefaultWhenNotDecided == false )
	{
		ioTextEncoding = tec;
	}
	*/
	return false;
}

//R0045 
ABool	AText::GuessTextEncodingFromCharset( ATextEncoding &outTextEncoding ) const
{
	ABool	found = false;
	AText	tecname;
	AItemCount	lineCount = 0;
	for( AIndex pos = 0; pos < GetItemCount();  )
	{
		AText	line;
		GetLine(pos,line);
		lineCount++;
		
		//B0139 20�s�ڂ܂łɌ��肷��
		if( lineCount > 20 )
		{
			break;
		}
		
		/*
		HTML4
		<META http-equiv="Content-Type" content="text/html;charset=UTF-8">
		HTML5
		<meta charset="UTF-8">
		XML
		<?xml version="1.0" encoding="UTF-8"?>
		CSS
		@charset "UTF-8";
		*/
		
		AIndex	p = 0;
		//------------------@charset "UTF-8";------------------
		if( line.FindCstring(0,"@charset",p) == true )
		{
			p += 8;
			//p�ȍ~��"UTF-8"
			AIndex	spos = p;
			if( line.FindCstring(p,"\"",spos) == true )
			{
				spos++;
				//spos�ȍ~��UTF-8"
				AIndex	epos = spos;
				if( line.FindCstring(spos,"\"",epos) == true )
				{
					//spos�`epos�i"�̑O�܂Łj�̃e�L�X�g���擾
					line.GetText(spos,epos-spos,tecname);
					if( ATextEncodingWrapper::GetTextEncodingFromName(tecname,outTextEncoding) == true )
					{
						found = true;
						break;
					}
				}
			}
		}
		//------------------charset=UTF-8"�A�܂��́Acharset="UTF-8"------------------
		else if( line.FindCstring(0,"charset",p) == true )
		{
			p += 7;
			AText	token;
			line.GetToken(p,token);
			if( token.Compare("=") == true )//=�̃`�F�b�N
			{
				//spos��=�̒���
				AIndex	spos = p;
				line.GetToken(p,token);
				if( token.Compare("\"") == true )
				{
					//�J�n"���L��ꍇ�iHTML5�j�Aspos��"�̒���ɂ���
					spos = p;
				}
				//spos�ȍ~��UTF-8"
				AIndex	epos = spos;
				if( line.FindCstring(spos,"\"",epos) == true )
				{
					//spos�`epos�i"�̑O�܂Łj�̃e�L�X�g���擾
					line.GetText(spos,epos-spos,tecname);
					if( ATextEncodingWrapper::GetTextEncodingFromName(tecname,outTextEncoding) == true )
					{
						found = true;
						break;
					}
				}
			}
		}
		//Ruby
		//------------------encoding: UTF-8------------------
		else if( line.FindCstring(0,"encoding:",p,true,true) == true )
		{
			AText	tecname;
			line.GetTextToTabSpaceLineEnd(p,tecname);
			if( tecname.GetItemCount() > 0 )
			{
				if( ATextEncodingWrapper::GetTextEncodingFromName(tecname,outTextEncoding) == true )
				{
					found = true;
					break;
				}
			}
		}
		//------------------encoding="UTF-8"------------------
		else if( line.FindCstring(0,"encoding",p) == true )
		{
			p += 8;
			AText	token;
			line.GetToken(p,token);
			if( token.Compare("=") == true )//=�̃`�F�b�N
			{
				//spos��=�̒���
				line.GetToken(p,token);
				if( token.Compare("\"") == true )
				{
					//spos��"�̒���ɂ���
					AIndex	spos = p;
					//spos�ȍ~��UTF-8"
					AIndex	epos = spos;
					if( line.FindCstring(spos,"\"",epos) == true )
					{
						//spos�`epos�i"�̑O�܂Łj�̃e�L�X�g���擾
						line.GetText(spos,epos-spos,tecname);
						if( ATextEncodingWrapper::GetTextEncodingFromName(tecname,outTextEncoding) == true )
						{
							found = true;
							break;
						}
					}
				}
			}
		}
	}
	if( found == true )
	{
		return true;
	}
	return false;
}

//
ABool	AText::GuessTextEncodingFromBOM( ATextEncoding &outTextEncoding ) const
{
	ABool	result = false;
	if( GetItemCount() >= 3 )
	{
		if( Get(0) == 0xEF && Get(1) == 0xBB && Get(2) == 0xBF )
		{
			outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
			result = true;
		}
	}
	if( GetItemCount() >= 2 )
	{
		if( (Get(0) == 0xFE && Get(1) == 0xFF) || (Get(0) == 0xFF && Get(1) == 0xFE) ) 
		{
			outTextEncoding = ATextEncodingWrapper::GetUTF16TextEncoding();
			result = true;
		}
	}
	return result;
}

//#764
/**
�G���[���ɂ�鐄��
*/
ABool	AText::GuessTextEncodingByAnalysis( ATextEncoding &outTextEncoding ) const
{
	ABool	result = false;
	
	//==================JIS, UTF16����==================
	//BOM�����݂���ꍇ��BOM�ɂ�鐄����UTF16�Ɣ���ς݁B
	{
		//�|�C���^�擾
		AStTextPtr	textptr(*this,0,GetItemCount());
		AUChar*	p = textptr.GetPtr();
		//�ŏ���1MB������
		AItemCount	len = textptr.GetByteCount();
		if( len > 1024*1024 )
		{
			len = 1024*1024;
		}
		
		AItemCount	jisCount = 0, utf16count = 0;
		for( AIndex pos = 0; pos < len-1; pos++ )
		{
			AUChar	ch1 = p[pos];
			AUChar	ch2 = p[pos+1];
			//JIS����
			if( ch1 == 0x1B && (ch2 == '$' || ch2 == 'K') )
			{
				jisCount++;
			}
			//UTF16����
			if( (ch1 == 0x0A && ch2 == 0x00) || (ch1 == 0x0D && ch2 == 0x00) )
			{
				utf16count++;
			}
			//
			if( jisCount >= 1 )
			{
				//JIS�m��
				outTextEncoding = ATextEncodingWrapper::GetJISTextEncoding();
				return true;
			}
			if( utf16count >= 1 )
			{
				//UTF16�m��
				//��UTF16LE?
				outTextEncoding = ATextEncodingWrapper::GetUTF16TextEncoding();
				return true;
			}
		}
	}
	
	//�G���[��
	AFloatNumber	errorRate = 0.0;
	
	//==================UTF-8����==================
	if( CalcUTF8ErrorRate(1024*1024,errorRate) == true )
	{
		//�G���[��10%�����Ȃ�UTF-8�Ɛ���
		if( errorRate < 0.1 )
		{
			outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
			return true;
		}
	}
	
	//==================SJIS����==================
	if( CalcSJISErrorRate(1024*1024,errorRate) == true )
	{
		//�G���[��1%�����Ȃ�SJIS�Ɛ���
		if( errorRate < 0.01 )
		{
			outTextEncoding = ATextEncodingWrapper::GetSJISTextEncoding();
			return true;
		}
	}
	
	//==================EUC����================== #1040 ���ԓ���ւ��iSJIS�D��ɂ���B����EUC�g�����Ƃ͏��Ȃ��ł��傤�B�j
	if( CalcEUCErrorRate(1024*1024,errorRate) == true )
	{
		//�G���[��1%�����Ȃ�EUC�Ɛ���
		if( errorRate < 0.01 )
		{
			outTextEncoding = ATextEncodingWrapper::GetEUCTextEncoding();
			return true;
		}
	}
	
	return false;
}

/**
UTF-8�ł̃G���[���v�Z
@return ascii�O�����������true
*/
ABool	AText::CalcUTF8ErrorRate( const AItemCount inLimit, AFloatNumber& outErrorRate ) const
{
	//���ʏ�����
	outErrorRate = 0.0;
	//�|�C���^�擾
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	p = textptr.GetPtr();
	//�ŏ���inLimit�o�C�g������
	AItemCount	len = textptr.GetByteCount();
	if( len > inLimit )
	{
		len = inLimit;
	}
	
	//�e�������Ƃ̃��[�v
	AItemCount	asciiCount = 0;
	AItemCount	utf8OKCount = 0;
	AItemCount	utf8NGCount = 0;
	for( AIndex pos = 0; pos < len;  )
	{
		AItemCount	bc = 1;
		if( GetUTF8ByteCount(p[pos],bc) == false )
		{
			//�ŏ��̃o�C�g�̃G���[
			utf8NGCount++;
			bc = 1;
		}
		else
		{
			if( bc == 1 )
			{
				//�A�X�L�[����
				asciiCount++;
			}
			else
			{
				//OK
				utf8OKCount++;
				//�r��������UTF-8�`���`�F�b�N
				for( AIndex pos2 = 1; pos2 < bc; pos2++ )
				{
					if( pos+pos2 < len )
					{
						AUChar	ch = p[pos+pos2];
						if( (ch&0xC0)!=0x80 )
						{
							utf8OKCount--;
							utf8NGCount++;
							break;
						}
					}
				}
			}
		}
		pos += bc;
	}
	
	//�G���[���v�Z
	if( utf8OKCount+utf8NGCount > 0 )
	{
		outErrorRate = utf8NGCount;
		outErrorRate = outErrorRate/(utf8OKCount+utf8NGCount);
		return true;
	}
	else
	{
		return false;
	}
}

/**
EUC�ł̃G���[���v�Z
@return ascii�O�����������true
*/
ABool	AText::CalcEUCErrorRate( const AItemCount inLimit, AFloatNumber& outErrorRate ) const
{
	//���ʏ�����
	outErrorRate = 0.0;
	//�|�C���^�擾
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	p = textptr.GetPtr();
	//�ŏ���inLimit�o�C�g������
	AItemCount	len = textptr.GetByteCount();
	if( len > inLimit )
	{
		len = inLimit;
	}
	
	//�e�������Ƃ̃��[�v
	AItemCount	asciiCount = 0;
	AItemCount	eucOKCount = 0;
	AItemCount	eucNGCount = 0;
	for( AIndex pos = 0; pos < len-1; pos++ )
	{
		AUChar	ch = p[pos];
		AUChar	ch2 = p[pos+1];
		//
		if( ch >= 0x80 )
		{
			if( ch >= 0x80 && ch <= 0xA0 && ch != 0x8E && ch!= 0x8F )
			{
				//1�o�C�g��NG
				eucNGCount++;
			}
			else
			{
				pos++;
				//2�o�C�g�ڔ���
				if( ch2 >= 0xA1 )
				{
					eucOKCount++;
				}
				else
				{
					eucNGCount++;
				}
			}
		}
	}
	//�G���[���v�Z
	if( eucOKCount+eucNGCount > 0 )
	{
		outErrorRate = eucNGCount;
		outErrorRate = outErrorRate/(eucOKCount+eucNGCount);
		return true;
	}
	else
	{
		return false;
	}
}

/**
SJIS�ł̃G���[���v�Z
@return ascii�O�����������true
*/
ABool	AText::CalcSJISErrorRate( const AItemCount inLimit, AFloatNumber& outErrorRate ) const
{
	//���ʏ�����
	outErrorRate = 0.0;
	//�|�C���^�擾
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	p = textptr.GetPtr();
	//�ŏ���inLimit�o�C�g������
	AItemCount	len = textptr.GetByteCount();
	if( len > inLimit )
	{
		len = inLimit;
	}
	
	//�e�������Ƃ̃��[�v
	AItemCount	asciiCount = 0;
	AItemCount	sjisOKCount = 0;
	AItemCount	sjisNGCount = 0;
	for( AIndex pos = 0; pos < len-1; pos++ )
	{
		AUChar	ch = p[pos];
		AUChar	ch2 = p[pos+1];
		//
		if( ch <= 0x7F )
		{
			asciiCount++;
		}
		else if( ((ch>=0x81&&ch<=0x9F)||(ch>=0xE0&&ch<=0xFC)) )
		{
			if( ((ch2>=0x40&&ch2<=0x7E)||(ch2>=0x80&&ch2<=0xFC)) )
			{
				pos++;
				//
				sjisOKCount++;
			}
			else
			{
				//
				sjisNGCount++;
			}
		}
		/*
		else
		{
			//���p�J�^�J�i��
			sjisOKCount++;
		}
		*/
	}
	
	//�G���[���v�Z
	if( sjisOKCount+sjisNGCount > 0 )
	{
		outErrorRate = sjisNGCount;
		outErrorRate = outErrorRate/(sjisOKCount+sjisNGCount);
		return true;
	}
	else
	{
		return false;
	}
}

ABool	AText::GuessTextEncodingLegacy( ATextEncoding &outTextEncoding ) const
{
	//MacOS�ɂ�鐄���̃e�X�g�����x�Ⴂ
#if 0
	OSStatus status = noErr;
	ItemCount actualCount = 0;
	
	ItemCount	encodingsCount = 0;
	status = ::TECCountAvailableTextEncodings(&encodingsCount);
	if( status != noErr )
	{
		return false;
	}
	
	TextEncoding*	encodingArray = (TextEncoding*)::malloc(sizeof(TextEncoding) * encodingsCount);
	
	status = ::TECGetAvailableTextEncodings(encodingArray,encodingsCount,&actualCount);
	if( status != noErr )
	{
		return false;
	}
	/*
	for( AIndex i = 0; i < actualCount; i++ )
	{
		fprintf(stderr,"%8X ",encodingArray[i]);
		AText	name;
		ATextEncodingWrapper::GetTextEncodingName(encodingArray[i],name);
		name.OutputToStderr();
		fprintf(stderr,"\n");
	}
	*/
	TECSnifferObjectRef	sniffer;
	
	TextEncoding	encs[5];
	encs[0] = ATextEncodingWrapper::GetSJISTextEncoding();
	encs[1] = ATextEncodingWrapper::GetJISTextEncoding();
	encs[2] = ATextEncodingWrapper::GetEUCTextEncoding();
	encs[3] = ATextEncodingWrapper::GetUTF8TextEncoding();
	encs[4] = ATextEncodingWrapper::GetUTF16LETextEncoding();
	status = ::TECCreateSniffer(&sniffer,encs,5);
	
	
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	p = textptr.GetPtr();
	AByteCount	len = textptr.GetByteCount();
	if( len > 1024 )   len = 1024;
	
	/*
	TextEncoding*	encodings = (TextEncoding*)::malloc(sizeof(TextEncoding) * actualCount);
	::memcpy(encodings, encodingArray, sizeof(TextEncoding) * actualCount);
	
	ItemCount*	errors = (ItemCount*)malloc(sizeof(ItemCount) * actualCount);
	ItemCount*	features = (ItemCount*)malloc(sizeof(ItemCount) * actualCount);
	*/
	
	TextEncoding	encodings[5];
	encodings[0] = 1;//ATextEncodingWrapper::GetSJISTextEncoding();
	encodings[1] = 0x820;//ATextEncodingWrapper::GetJISTextEncoding();
	encodings[2] = 0x920;//ATextEncodingWrapper::GetEUCTextEncoding();
	encodings[3] = 0x8000103;//ATextEncodingWrapper::GetUTF8TextEncoding();
	encodings[4] = 0x103;//ATextEncodingWrapper::GetUTF16LETextEncoding();
	ItemCount	errors[5];
	ItemCount	features[5];
	
	status = ::TECSniffTextEncoding(sniffer,p,len,encodings,5,errors,5,features,5);
	
	AText	tecname;
	ATextEncodingWrapper::GetTextEncodingName(encodings[0],tecname);
	tecname.OutputToStderr();
	
	/*
	free(encodings);
	free(errors);
	free(features);
	*/
	
	::TECDisposeSniffer(sniffer);
	free(encodingArray);
	return false;
	#endif
	
	ABool	outKakutei = false;//R0045
	outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
	
	ABool	kanjiExist = false;
	AIndex	end = 1024*1024;//B0015 ����Ɏg�p����e�L�X�g�F�ŏ���5KB��1MB
	ANumber	jesc = 0, s8182 = 0, eA1A4 = 0;
	ANumber	OutOfEUC = 0;//B0044
	ANumber	uni000D = 0, uni000A = 0, uniE3 = 0;
	AUChar ch, ch2;
	if( end > GetItemCount() )   end = GetItemCount();
	ABool	kanjiMode = false;
	for( AIndex pos = 0; pos < end; pos++ ) 
	{
		ch = Get(pos);
		if( pos+1 < end )
		{
			ch2= Get(pos+1);
		}
		else
		{
			ch2 = 0x20;
		}
		if( kanjiMode )
		{
			kanjiMode = false;
			continue;
		}
		if( ch >= 0x80 )   kanjiExist = true;
		if( ch >= 0x80 && ch <= 0xA0 && ch != 0x8E && ch!= 0x8F )//B0044
		{
			OutOfEUC++;
		}
		if( ch == 0x1B ) 
		{
			if( ch2 == '$' || ch2 == 'K' )   jesc++;
			kanjiExist = true;
		}
		else if( ch == 0x81 || ch == 0x82 ) {
			s8182++;
			kanjiExist = true;
		}
		else if( ch == 0xA1 || ch == 0xA4 || ch == 0xA5 ) {//B0044 A5��ǉ��i�J�^�J�i�p�j
			eA1A4++;
			kanjiExist = true;
		}
		else if( ch == 0 && ch2 == 13 ) 
		{
			uni000D++;
			kanjiExist = true;
		}
		else if( ch == 0 && ch2 == 10 ) 
		{
			uni000A++;
			kanjiExist = true;
		}
		else if( ch == 0xE3 ) 
		{
			uniE3++;
			kanjiExist = true;
		}
		if( ch >= 0x80 ) 
		{
			kanjiMode = true;
		}
	}
	if( jesc >= 1 )
	{
		outTextEncoding = ATextEncodingWrapper::GetJISTextEncoding();
		outKakutei = true;//R0045  ESC+$/K�������JIS�͊m��
	}
	else if( ( uni000D >= 1 || uni000A >= 1 ) )
	{
		outTextEncoding = ATextEncodingWrapper::GetUTF16TextEncoding();
		outKakutei = true;//R0045  000D/000A�������UTF-16�m��
	}
	else if( ((uniE3 >= 2 && uniE3 > s8182-1 && uniE3 > eA1A4-1 && uniE3 > uni000D && uniE3 > uni000A) 
			/*|| uniE3 > 10  B0007*/ ) )
	{
		outTextEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();
		if( uniE3 > /*B0274 10*/5 )//R0045  E3��10�ȏ゠���UTF-8�m��
		{
			outKakutei = true;
		}
	}
//	else if( s8182 >= 2 && s8182 > eA1A4 && s8182 > uni000D && s8182 > uni000A )   mKanjiCode = kanjiCode_MacCode;B0044
	else if( eA1A4 >= 2 && OutOfEUC == 0 )
	{
		outTextEncoding = ATextEncodingWrapper::GetEUCTextEncoding();
		if( eA1A4 > /*B0274 10*/5 )//R0045 A1/A4��10�ȏ゠���EUC-JP�m��
		{
			outKakutei = true;
		}
	}
	else //if(  s8182 > 2 )
	{
		outTextEncoding = ATextEncodingWrapper::GetSJISTextEncoding();
		if(  s8182 > /*B0274 10*/5 )//81/82��10�ȏ゠���ShiftJIS�m��
		{
			outKakutei = true;
		}
	}
	return outKakutei;
	/*B0044
	EUC: ��P�o�C�g�@0xA1-0xFE, 0x8E, 0x8F
	��Q�o�C�g�@0xA1-0xFE
	SJIS: ��P�o�C�g�@0x81-0x9F, 0xE0-0xFC�i�c��͔��p�J�i�j
	��Q�o�C�g�@�S�R�[�h�H
	
	�b��΍�
	OutOfEUC�J�E���^�i8E,8F�ȊO��80-A0�j�����݂�����EUC�ɂ͂��Ȃ�
	
	���{�΍�āF
	JIS:ESC+$/K��1�ȏ�
	EUC:�ŏ������L���[���Ŗ������Ȃ����ǂ����𒲂ב����A�����������_��EUC�t���O���Ƃ�
	UCS2:00��2�ȏ�
	UTF8:E3��2�ȏ�A����81/82��������
	SJIS:����ȊO
	*/
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�G���R�[�f�B���O�^���s�R�[�h�ϊ�

//�e��G���R�[�f�B���O�^���s�R�[�h����G���R�[�f�B���O�FUTF8, ���s�R�[�h�FCR�֕ϊ�
//�ϊ��G���[���̃e�L�X�g���e�͔j�󂳂��
ABool	AText::ConvertToUTF8CR( const ATextEncoding inSrcTextEncoding, const ABool inUseFallback, ABool& outFallbackUsed )
{
	ABool	result = ConvertToUTF8(inSrcTextEncoding,inUseFallback,outFallbackUsed);
	ConvertReturnCodeToCR();
	return result;
}

//UTF8, CR����e��G���R�[�f�B���O�A���s�R�[�h�֕ϊ�
//�e�L�X�g���e�͐���^�G���[�ǂ���̏ꍇ�����̂܂ܕێ������
ABool	AText::ConvertFromUTF8CR( const ATextEncoding inDstTextEncoding, const AReturnCode inDstReturnCode, AText& outText,
								  const ABool inConvertToCanonical ) const //#1044
{
	ConvertReturnCodeFromCR(inDstReturnCode,outText);
	return outText.ConvertFromUTF8(inDstTextEncoding,inConvertToCanonical);//#1044
}

//���s�R�[�h��CR�֕ϊ�
AReturnCode	AText::ConvertReturnCodeToCR( const AReturnCode inDefaultReturnCode )//#307
{
	ABool	notMixed = true;
	return ConvertReturnCodeToCR(inDefaultReturnCode,notMixed);
}
AReturnCode	AText::ConvertReturnCodeToCR( const AReturnCode inDefaultReturnCode, ABool& outNotMixed ) //#995
{
	outNotMixed = true;//#995
	/*B0313 Delete()����Ƒ傫���t�@�C�����J�����Ƃ��ɕ��ׂ��傫��
	AIndex pos = 0;
	while( pos < GetItemCount() )
	{
		AUChar	ch = Get(pos);
		pos++;
		if( ch == kUChar_CR )
		{ 
			if( pos < GetItemCount() )
			{
				if( Get(pos) == kUChar_LF )
				{
					Delete(pos,1);
				}
			}
		}
		else if( ch == kUChar_LF )
		{
			Set(pos-1,kUChar_CR);
		}
	}*/
	AItemCount	crCount = 0, crlfCount = 0, lfCount = 0;//#995
	AItemCount	u0085Count = 0, u2028Count = 0, u2029Count = 0;//#1472
	AReturnCode	resultReturnCode = inDefaultReturnCode;//returnCode_CR;//#307
	AItemCount	count = GetItemCount();
	AText	buffer;
	buffer.Reserve(0,count);
	AIndex	dstpos = 0;
	{
		AStTextPtr	dsttextptr(buffer,0,buffer.GetItemCount());
		AUChar*	dsttextp = dsttextptr.GetPtr();
		AStTextPtr	srctextptr(*this,0,GetItemCount());
		AUChar*	srctextp = srctextptr.GetPtr();
		for( AIndex pos = 0; pos < count;  )
		{
			AUChar	ch = srctextp[pos];
			pos++;
			if( ch == kUChar_CR )
			{
				if( pos < count )
				{
					if( srctextp[pos] == kUChar_LF )
					{
						pos++;
						resultReturnCode = returnCode_CRLF;//#307
						crlfCount++;//#995
					}
					else
					{
						resultReturnCode = returnCode_CR;//#995
						crCount++;//#995
					}
				}
				else
				{
					resultReturnCode = returnCode_CR;//#995
					crCount++;//#995
				}
			}
			else if( ch == kUChar_LF )
			{
				ch = kUChar_CR;
				resultReturnCode = returnCode_LF;//#307
				lfCount++;//#995
			}
			//#1472
			//U+2028 (UTF-8: 0xE2 0x80 0xA8) LINE SEPARATOR
			//U+2029 (UTF-8: 0xE2 0x80 0xA9) PARAGRAPH SEPARATOR
			//http://unicode.org/versions/Unicode5.2.0/ch05.pdf
			else if( ch == 0xE2 )
			{
				if( pos+1 < count )
				{
					if( srctextp[pos] == 0x80 && srctextp[pos+1] == 0xA8 )
					{
						pos += 2;
						ch = kUChar_CR;
						u2028Count++;
					}
					else if( srctextp[pos] == 0x80 && srctextp[pos+1] == 0xA9 )
					{
						pos += 2;
						ch = kUChar_CR;
						u2029Count++;
					}
				}
			}
			//#1472
			//U+0085 (UTF-8: 0xC2 0x85) NEXT LINE
			else if( ch == 0xC2 )
			{
				if( pos < count )
				{
					if( srctextp[pos] == 0x85 )
					{
						pos++;
						ch = kUChar_CR;
						u0085Count++;
					}
				}
			}
			dsttextp[dstpos] = ch;
			dstpos++;
		}
	}
	buffer.DeleteAfter(dstpos);
	//#695 ������ SetText(buffer);
	SwapContent(buffer);//#695
	//#1472
	AItemCount	totalCount = crCount + crlfCount + lfCount + u0085Count + u2028Count + u2029Count;
	//#995
	//���肵�����s�R�[�h�ƈႤ���s�R�[�h�J�E���^��1�ȏ�Ȃ�G���[�ioutNotMixed��false�ɂ���j
	switch(resultReturnCode)
	{
	  case returnCode_CR:
		{
			if( crCount != totalCount )//#1472
			{
				outNotMixed = false;
			}
			break;
		}
	  case returnCode_CRLF:
		{
			if( crlfCount != totalCount )//#1472
			{
				outNotMixed = false;
			}
			break;
		}
	  case returnCode_LF:
		{
			if( lfCount != totalCount )//#1472
			{
				outNotMixed = false;
			}
			break;
		}
	}
	//#1472
	//U+0085/U+2028/U+2029�ɂ��ẮA���ׂẲ��s�R�[�h������ɂȂ��Ă���ꍇ�̂݁AresultReturnCode�ɐݒ肷��B
	//U+0085
	if( u0085Count > 0 && u0085Count == totalCount )
	{
		resultReturnCode = returnCode_U0085;
	}
	//U+2028
	if( u2028Count > 0 && u2028Count == totalCount )
	{
		resultReturnCode = returnCode_U2028;
	}
	//U+2029
	if( u2029Count > 0 && u2029Count == totalCount )
	{
		resultReturnCode = returnCode_U2029;
	}
	return resultReturnCode;//#307
}

//���s�R�[�h��CR����inDstReturnCode�֕ϊ����āAoutText�֊i�[
void	AText::ConvertReturnCodeFromCR( const AReturnCode inDstReturnCode, AText& outText ) const
{
	outText.DeleteAll();
	if( inDstReturnCode == returnCode_CR )
	{
		outText.SetText(*this);
	}
	else
	{
		//�����x���P����
		AStTextPtr	textptr(*this,0,GetItemCount());
		AByteCount	bytecount = textptr.GetByteCount();//#598
		AUChar*	ptr = textptr.GetPtr();
		for( AIndex pos = 0; pos < /*#598 GetItemCount()*/bytecount; pos++ )
		{
			unsigned char	ch = (ptr)[pos];
			if( ch == kUChar_CR )
			{
				switch(inDstReturnCode)
				{
				  case returnCode_LF:
					{
						outText.Add(kUChar_LF);
						break;
					}
				  case returnCode_CR:
					{
						outText.Add(kUChar_CR);
						break;
					}
				  case returnCode_CRLF:
					{
						outText.Add(kUChar_CR);
						outText.Add(kUChar_LF);
						break;
					}
					//#1472
				  case returnCode_U0085:
					{
						outText.Add(0xC2);
						outText.Add(0x85);
						break;
					}
					//#1472
				  case returnCode_U2028:
					{
						outText.Add(0xE2);
						outText.Add(0x80);
						outText.Add(0xA8);
						break;
					}
					//#1472
				  case returnCode_U2029:
					{
						outText.Add(0xE2);
						outText.Add(0x80);
						outText.Add(0xA9);
						break;
					}
				}
			}
			else
			{
				outText.Add(ch);
			}
		}
	}
}

//�e��G���R�[�f�B���O����UTF8�֕ϊ�
//�ϊ��G���[���̃e�L�X�g���e�͔j�󂳂��
ABool	AText::ConvertToUTF8( const ATextEncoding inSrcTextEncoding, const ABool inUseFallback, ABool& outFallbackUsed, const ABool inFromExternalData )//B0400
{
	return ATextEncodingWrapper::ConvertToUTF8(*this,inSrcTextEncoding,inUseFallback,outFallbackUsed,inFromExternalData);//B0400
}
ABool	AText::ConvertToUTF8( const ATextEncoding inSrcTextEncoding )
{
	ABool	fallback = false;
	return ConvertToUTF8(inSrcTextEncoding,true,fallback);
}

//UTF8����e��G���R�[�f�B���O�֕ϊ�
//�ϊ��G���[���̃e�L�X�g���e�͔j�󂳂��
ABool	AText::ConvertFromUTF8( const ATextEncoding inDstTextEncoding, const ABool inConvertToCanonical )//#1044
{
	return ATextEncodingWrapper::ConvertFromUTF8(*this,inDstTextEncoding,inConvertToCanonical);//#1044
}

//UTF16����UTF8�֕ϊ�
void	AText::ConvertToUTF8FromUTF16()
{
	ABool	fallback;
	if( ATextEncodingWrapper::ConvertToUTF8(*this,ATextEncodingWrapper::GetUTF16TextEncoding(),true,fallback,false) == false )   _ACError("",this);
}

//UTF8����UTF16�֕ϊ�
ABool	AText::ConvertFromUTF8ToUTF16()
{
	if( ATextEncodingWrapper::ConvertFromUTF8(*this,ATextEncodingWrapper::GetUTF16TextEncoding()) == false )
	{
		//������CUserPane::DrawText()������R�[������邪�A�����������Ă���Ƃ��ɗ�����̂ŁA_ACError�ɂ͂��Ȃ��B
		//CTextDrawData::MakeTextDrawDataWithoutStyle()����R�[�����ꂽ�Ƃ��Ɏ��s����ƁA�������[�v�Ɋׂ�̂ŁA
		//���L�̂ނ���ϊ����{���Bwin
		AText	text;
		for( AIndex i = 0; i < GetItemCount(); i++ )
		{
			if( ATextEncodingWrapper::UTF16IsLE() == true )
			{
				text.Add(Get(i));
				text.Add(0);
			}
			else
			{
				text.Add(0);
				text.Add(Get(i));
			}
		}
		SetText(text);
		return false;
	}
	return true;
}

//API�pUnicode����UTF8�֕ϊ�
void	AText::ConvertToUTF8FromAPIUnicode()
{
	ABool	fallback;
	if( ATextEncodingWrapper::ConvertToUTF8(*this,ATextEncodingWrapper::GetUTF16TextEncoding(),true,fallback,false) == false )   _ACError("",this);
}

//UTF8����API�pUnicode�֕ϊ�
void	AText::ConvertFromUTF8ToAPIUnicode()
{
	if( ATextEncodingWrapper::ConvertFromUTF8(*this,ATextEncodingWrapper::GetUTF16TextEncoding()) == false )   
			{/*_ACError("");������CUserPane::DrawText()����R�[������邪�A�����������Ă���Ƃ��ɗ�����̂ŁA_ACError�ɂ͂��Ȃ��B�����K�v*/}
}

//API�pLegacy�e�L�X�g�G���R�[�f�B���O����UTF8�֕ϊ�
void	AText::ConvertToUTF8FromAPILegacy()
{
	ABool	fallback;
	if( ATextEncodingWrapper::ConvertToUTF8(*this,ATextEncodingWrapper::GetSJISTextEncoding(),true,fallback,false) == false )   _ACError("",this);
	//��SJIS�Ƃ͌���Ȃ��̂ő΍��K�v
}

//UTF8����API�pLegacy�e�L�X�g�G���R�[�f�B���O�֕ϊ�
void	AText::ConvertFromUTF8ToAPILegacy()
{
	if( ATextEncodingWrapper::ConvertFromUTF8(*this,ATextEncodingWrapper::GetSJISTextEncoding()) == false )   
			{/*_ACError("");������CUserPane::DrawText()����R�[������邪�A�����������Ă���Ƃ��ɗ�����̂ŁA_ACError�ɂ͂��Ȃ��B�����K�v*/}
	//��SJIS�Ƃ͌���Ȃ��̂ő΍��K�v
}

//AText�Ɋi�[����UTF-8������ɑ΂��āAUTF-16������֕ϊ������ꍇ�́AUTF-8�o�C�g�ʒu��UTF-16�����ʒu�̑Ή����𓾂�
//outUTF16PosIndexArray: UTF-8������ł̃o�C�g�ʒu���C���f�b�N�X�Ƃ��āA�Ή�����UTF-16������ł̈ʒu���i�[�����z��
//outUTF8PosIndexArray: UTF-16������ł̈ʒu���C���f�b�N�X�Ƃ��āA�Ή�����UTF-8������ł̍ŏ��̃o�C�g�̈ʒu���i�[�����z��
void	AText::GetUTF16TextIndexInfo( AArray<AIndex>& outUTF16PosIndexArray, AArray<AIndex>& outUTF8PosIndexArray ) const
{
	outUTF16PosIndexArray.DeleteAll();
	outUTF8PosIndexArray.DeleteAll();
	AItemCount	utf8textlength = GetItemCount();
	AIndex	utf16pos = 0;
	for( AIndex utf8pos = 0; utf8pos < utf8textlength; )
	{
		//Unicode�ꕶ�����ɏ���
		//UTF-8��Unicode�ꕶ�����ɁA���݂�UTF-16���ʒu���i�[
		AItemCount	bc = GetUTF8ByteCount(Get(utf8pos));
		for( AIndex i = 0; i < bc; i++ )
		{
			outUTF16PosIndexArray.Add(utf16pos);
		}
		//UTF-16�ʒu���X�V���AUTF-8��Unicode�ꕶ�����ɁA���݂�UTF-8���ʒu���i�[
		utf16pos++;
		outUTF8PosIndexArray.Add(utf8pos);
		//#560 �T���Q�[�g�y�A�̏ꍇ�AUTF-16��2�����ŁAUnicode�ꕶ��(U+010000-U+10FFFF)
		if( bc >= 4 )
		{
			utf16pos++;
			outUTF8PosIndexArray.Add(utf8pos);
		}
		//
		utf8pos += bc;
	}
	outUTF8PosIndexArray.Add(utf8textlength);
	outUTF16PosIndexArray.Add(utf16pos);
}


//UTF8�Ƃ��Đ��������`�F�b�N����
ABool	AText::CheckUTF8Text( const AItemCount inLimitLength ) const
{
	AItemCount	limitlength = inLimitLength;
	if( limitlength > GetItemCount() )
	{
		limitlength = GetItemCount();
	}
	//�������̂��߃|�C���^�g�p
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	ptr = textptr.GetPtr();
	AItemCount	count = textptr.GetByteCount();
	for( AIndex pos = 0; pos < limitlength; )
	{
		AItemCount	bc = 1;
		if( GetUTF8ByteCount((ptr)[pos],bc) == false )//#412
		//#412 if( bc == 5 )
		{
			return false;
		}
		pos += bc;
		if( pos > count )
		{
			return false;//B0000
		}
	}
	return true;
}

void	AText::GetUTF16Text( AUniText& outText ) const
{
	outText.DeleteAll();
	AText	text;
	text.SetText(*this);
	text.ConvertFromUTF8ToUTF16();
	AStTextPtr	textptr(text,0,text.GetItemCount());
	outText.InsertFromPtr(0,reinterpret_cast<AUTF16Char*>(textptr.GetPtr()),textptr.GetByteCount()/sizeof(AUTF16Char));
}

/*
void AText::NormalizeTextEncodingFromUTF16()
{
	//�b��
#if IMPLEMENTATION_FOR_MACOSX
	CText	text;
	{
		AStTextPtr	textptr(*this,0,GetItemCount());
		text.AddTextPtr(textptr.GetPtr(),textptr.GetByteCount());
	}
	text.ConvertToUTF8();
	DeleteAll();
	text.Lock();
	AddFromTextPtr(*(text.mTextH),text.mLength);
	text.Unlock();
#endif
}

void AText::GetUTF16Text( AUniText& outText ) const
{
	outText.DeleteAll();
	//�b��
#if IMPLEMENTATION_FOR_MACOSX
	AText	tmp;
	tmp.SetText(*this);
	AStTextPtr	textptr(tmp,0,tmp.GetItemCount());
	CText	text;
	text.AddTextPtr(textptr.GetPtr(),textptr.GetByteCount());
	CText	utf16text;
	text.ConvertFromUTF8(utf16text);
	utf16text.Lock();
	outText.InsertFromPtr(0,reinterpret_cast<AUTF16Char*>(*(utf16text.mTextH)),utf16text.mLength/2);
	utf16text.Unlock();
#endif
#if A_Windows
	wchar_t	str[4096];
	AStTextPtr	textptr(this);
	int unilen = MultiByteToWideChar(65001,0,reinterpret_cast<LPCSTR>(textptr.Get()),
			GetLength(),str,2048);
	outText.Insert(0,str,unilen);
#endif
}

void AText::GetUTF16TextForDisplay( AItemCount inTabWidth, bool inCountAs2Letters0800, bool inDisplayControlCode, AUniText& outText )
{
	GetUTF16Text(outText);
	AItemCount	tabletters = 0;
	AItemCount	tabinc0800 = 1;
	if( inCountAs2Letters0800 )   tabinc0800 = 2;
	for( AIndex pos = 0; pos < outText.GetItemCount(); pos++ )
	{
		AUTF16Char	ch = outText.Get(pos);
		if( ch == 0x0009 )
		{
			outText.Set(pos,0x0020);
			tabletters++;
			AItemCount	spaces = 0;
			while((tabletters%inTabWidth)!=0)
			{
				spaces++;
				tabletters++;
			}
			if( spaces > 0 )
			{
				outText.Reserve(pos,spaces);
				for( AItemCount i = 0; i < spaces; i++ )
				{
					outText.Set(pos+i,0x0020);
				}
			}
			pos += spaces;
		}
		else if( inDisplayControlCode && ch <= 0x001F )
		{
			//�b��
			//�X�^�u
		}
		else 
		{
			if( ch < 0x0800 )
			{
				tabletters++;
			}
			else
			{
				tabletters += tabinc0800;
			}
		}
	}
}

void AText::GetUTF16TextForDisplayVirtual( AIndex& ioPosition, AItemCount inTabWidth, 
		bool inCountAs2Letters0800, bool inDisplayControlCode, AIndex& ioUniPos, AItemCount& ioTabLetters )
{
	unsigned char	ch = Get(ioPosition);
	AItemCount	bc = GetUTF8ByteCount(ioPosition);
	AItemCount	tabinc0800 = 1;
	if( inCountAs2Letters0800 )   tabinc0800 = 2;
	if( bc < 3 )
	{
		ioUniPos++;
		ioTabLetters++;
	}
	else if( bc < 4 )
	{
		ioUniPos++;
		ioTabLetters += tabinc0800;
	}
	else
	{
		ioUniPos += 2;
		ioTabLetters += tabinc0800;
	}
	if( ch == A_CHAR_TAB )
	{
		while((ioTabLetters%inTabWidth)!=0)
		{
			ioUniPos++;
			ioTabLetters++;
		}
	}
	else if( inDisplayControlCode && ch < 0x1F )
	{
		ioUniPos += 3;
	}
	ioPosition += bc;
}

//�b��
void	AText::ConvertToCanonicalComp()
{
	CText	text, unitext;
	AUtil::GetCTextFromAText(*this,text);
	text.ConvertFromUTF8(unitext);
	unitext.ConvertToCanonicalComp();
	unitext.ConvertToUTF8();
	AUtil::GetATextFromCText(unitext,*this);
}

//�b��
void	AText::ConvertToHFSPlusDecomp()
{
	CText	text, unitext;
	AUtil::GetCTextFromAText(*this,text);
	text.ConvertFromUTF8(unitext);
	unitext.ConvertToHFSPlusDecomp();
	unitext.ConvertToUTF8();
	AUtil::GetATextFromCText(unitext,*this);
}
*/

/*void AText::GetLine( const AIndex& inPosition, AText& outText )
{
	AIndex	pos = inPosition;
	while( pos < GetItemCount() )
	{
		if( mDataPtr[pos] == A_CHAR_LineEnd )
		{
			break;
		}
		pos++;
	}
	GetText(inPosition,pos-inPosition,outText);
}*/

//�^�u�A�X�y�[�X��ǂݔ�΂��āA����ȊO�̕����܂�ioPos��i�߂�B
ABool	AText::SkipTabSpace( AIndex& ioPos, const AIndex inEndPos ) const
{
	AStTextPtr	textptr(*this,0,GetItemCount());
	return SkipTabSpace(textptr.GetPtr(),textptr.GetByteCount(),ioPos,inEndPos);
}
ABool	AText::SkipTabSpace( const AUChar* inTextPtr, const AItemCount inTextLen,
							AIndex& ioPos, const AIndex inEndPos ) 
{
	//
	if( inEndPos > inTextLen )
	{
		_ACError("",NULL);
		return false;
	}
	//
	if( ioPos >= inEndPos )   return false;
	AUChar	ch = inTextPtr[ioPos];
	if( ch == kUChar_Space || ch == kUChar_Tab )
	{
		ioPos++;
		for( ;ioPos < inEndPos; ioPos++ )
		{
			ch = inTextPtr[ioPos];
			if( ch == kUChar_Space || ch == kUChar_Tab )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	if( ioPos >= inEndPos )   return false;
	else return true;
}

//�^�u�A�X�y�[�X��ǂݔ�΂��āA����ȊO�̕����܂�ioPos��i�߂�B
ABool	AText::SkipTabSpaceLineEnd( AIndex& ioPos, const AIndex inEndPos ) const
{
	if( ioPos >= inEndPos )   return false;
	AUChar	ch = Get(ioPos);
	if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
	{
		ioPos++;
		for( ;ioPos < inEndPos; ioPos++ )
		{
			ch = Get(ioPos);
			if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	if( ioPos >= inEndPos )   return false;
	else return true;
}

//#318
/**
�^�u�E�X�y�[�X�i�S�p�X�y�[�X�܂ށj���X�L�b�v����
*/
ABool	AText::SkipTabSpaceIncludingZenkakuSpace( AIndex& ioPos, const AIndex inEndPos ) const
{
	if( ioPos >= inEndPos )   return false;
	//�S�p�X�y�[�X�A���p�X�y�[�X�A�^�u
	AText	zenkakuSpaceText;
	zenkakuSpaceText.SetZenkakuSpace();
	AText	spaceText(" ");
	AText	tabText("\t");
	//ioPos�ȍ~�Ŏ��ɃX�y�[�X�E�^�u�ł͂Ȃ��ӏ���ioPos�ɐݒ肷��
	AText	text;
	GetText(ioPos,GetUTF8ByteCount(ioPos),text);
	if( text.Compare(spaceText) == true || text.Compare(tabText) == true || text.Compare(zenkakuSpaceText) == true )
	{
		ioPos += text.GetItemCount();
		for( ;ioPos < inEndPos; ioPos += text.GetItemCount() )
		{
			GetText(ioPos,GetUTF8ByteCount(ioPos),text);
			if( text.Compare(spaceText) == true || text.Compare(tabText) == true || text.Compare(zenkakuSpaceText) == true )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
	if( ioPos >= inEndPos )   return false;
	else return true;
}

//#318
/**
�S�p�X�y�[�X��ݒ肷��
*/
void	AText::SetZenkakuSpace()
{
	DeleteAll();
	Add(0xE3);
	Add(0x80);
	Add(0x80);
}

//#942
/**
�e�L�X�g���S�p�X�y�[�X���ǂ����𔻒�
*/
ABool	AText::CompareZenkakuSpace() const
{
	if( GetItemCount() != 3 )
	{
		return false;
	}
	else
	{
		return ( Get(0)==0xE3 && Get(1)==0x80 && Get(2)==0x80 );
	}
}

void	AText::ConvertToCanonicalComp()
{
	/*#1040
	ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(*this);
	ConvertToUTF8FromUTF16();
	*/
	ATextEncodingWrapper::ConvertToCanonicalComp(*this);
}

//UTF16Text����1������UTF16Char���擾 R0199
AUTF16Char	AText::GetUTF16CharFromUTF16Text( const AIndex inIndex ) const
{
	if( ATextEncodingWrapper::UTF16IsLE() == true )
	{
		return Get(inIndex) + Get(inIndex+1)*0x100;
	}
	else
	{
		return Get(inIndex)*0x100 + Get(inIndex+1);
	}
}

//win
/**
UTF-16������}���iLE/BE���f���A�K�؂ȏ��Ԃő}���j
*/
void	AText::InsertUTF16Char( const AIndex inIndex, AUTF16Char inChar )
{
	if( ATextEncodingWrapper::UTF16IsLE() == true )
	{
		Insert1(inIndex,(inChar&0xFF));
		Insert1(inIndex+1,(inChar&0xFF00)/0x100);
	}
	else
	{
		Insert1(inIndex,(inChar&0xFF00)/0x100);
		Insert1(inIndex+1,(inChar&0xFF));
	}
}

//#764
/**
�o�C�i���t�@�C�����ǂ����𐄑�����
*/
ABool	AText::SuspectBinaryData() const
{
	//==================UTF8�������`�F�b�N==================
	//�ŏ���10000�o�C�g���`�F�b�N���AUTF8�Ƃ��Ă���������΃o�C�i���t�@�C���Ƃ݂Ȃ�
	if( CheckUTF8Text(10000) == false )
	{
		return true;
	}
	
	//==================NULL�������`�F�b�N==================
	//�ŏ���10000�o�C�g���`�F�b�N���ANULL������10������葽������΁A�o�C�i���t�@�C���Ƃ݂Ȃ�
	AItemCount	checklength = 10000;
	if( checklength > GetItemCount() )
	{
		checklength = GetItemCount();
	}
	//NULL������
	AItemCount	nullCharCount = 0;
	//�|�C���^�擾
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	p = textptr.GetPtr();
	for( AIndex i = 0; i < checklength; i++ )
	{
		AUChar	ch = p[i];
		if( ch == 0x00 )
		{
			nullCharCount++;
		}
		if( nullCharCount > 10 )
		{
			return true;
		}
	}
	
	return false;
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g���

//�����l��Parse
ABool	AText::ParseIntegerNumber( AIndex& ioPos, ANumber& outNumber, const ABool inAllowMinus ) const
{
	AIndex	startPos = ioPos;
	ABool	result = false;
	ANumber	number = 0;
	ANumber	mul = 1;
	AIndex	digits = 9;//9���܂�
	for( ; ioPos < GetItemCount(); ioPos++ )
	{
		AUChar	ch = Get(ioPos);
		if( ch == kUChar_Minus && inAllowMinus == true && ioPos == startPos )
		{
			mul = -1;
		}
		else if( ch >= '0' && ch <= '9' )
		{
			number = number*10;
			number += ch-'0';
			result = true;
			digits--;
			if( digits == 0 )   break;
		}
		else break;
	}
	if( result == true )
	{
		outNumber = number*mul;
	}
	return result;
}
ABool	AText::ParseIntegerNumber( ANumber& outNumber, const ABool inAllowMinus ) const
{
	AIndex	pos = 0;
	return ParseIntegerNumber(pos,outNumber,inAllowMinus);
}

//#427
/**
64bit�����l��Parse
*/
ABool	AText::ParseIntegerNumber64bit( AIndex& ioPos, ANumber64bit& outNumber, const ABool inAllowMinus ) const
{
	ABool	result = false;
	ANumber64bit	number = 0;
	ANumber64bit	mul = 1;
	AIndex	digits = 18;//18���܂�
	for( ; ioPos < GetItemCount(); ioPos++ )
	{
		AUChar	ch = Get(ioPos);
		if( ch == kUChar_Minus && inAllowMinus == true )
		{
			mul = -1;
		}
		else if( ch >= '0' && ch <= '9' )
		{
			number = number*10;
			number += ch-'0';
			result = true;
			digits--;
			if( digits == 0 )   break;
		}
		else break;
	}
	if( result == true )
	{
		outNumber = number*mul;
	}
	return result;
}
ABool	AText::ParseIntegerNumber64bit( ANumber64bit& outNumber, const ABool inAllowMinus ) const
{
	AIndex	pos = 0;
	return ParseIntegerNumber64bit(pos,outNumber,inAllowMinus);
}

//�����l��Parse
ABool	AText::ParseFloatNumber( AIndex& ioPos, AFloatNumber& outNumber, const ABool inAllowMinus ) const
{
	if( ioPos >= GetItemCount() )   return false;
	//��������Parse
	ANumber	integer = 0;
	ABool	result = ParseIntegerNumber(ioPos,integer,inAllowMinus);
	outNumber = integer;
	//�s���I�h�������ꍇ�ȊO�͂����Ń��^�[��
	if( ioPos >= GetItemCount() )   return result;
	if( Get(ioPos) != kUChar_Period )   return result;
	//�����_�ȉ�Parse
	ioPos++;
	AIndex	prePos = ioPos;
	if( ParseIntegerNumber(ioPos,integer,false) == false )   return true;
	AItemCount	digits = ioPos - prePos;
	AFloatNumber	num = integer;
	num /= pow(10.0,digits);
	outNumber += num;
	return true;
}
ABool	AText::ParseFloatNumber( AFloatNumber& outNumber, const ABool inAllowMinus ) const
{
	AIndex	pos = 0;
	return ParseFloatNumber(pos,outNumber,inAllowMinus);
}

//�e�L�X�g���琮���l���擾�i�e�L�X�g�̍ŏ��ɕK�������l�����݂��邱�Ƃ��킩���Ă���ꍇ�p�j
ANumber	AText::GetNumber() const
{
	ANumber	result = 0;
	ParseIntegerNumber(result);
	return result;
}

//�e�L�X�g��������l���擾�i�e�L�X�g�̍ŏ��ɕK�������l�����݂��邱�Ƃ��킩���Ă���ꍇ�p�j
AFloatNumber	AText::GetFloatNumber() const
{
	AFloatNumber	result = 0.0;
	ParseFloatNumber(result);
	return result;
}

//#427
/**
�e�L�X�g����64bit�����l���擾
*/
ANumber64bit	AText::GetNumber64bit() const
{
	ANumber64bit	result = 0;
	ParseIntegerNumber64bit(result);
	return result;
}

//�e�L�X�g�����i�P���������炵�Ȃ����v����j
//outPosition: ���̈ʒu�ȍ~�Ɉ�v�e�L�X�g
ABool	AText::FindText( AIndex inPosition, const AText& inText, AIndex& outPosition, const AIndex inEndPos,
						const ABool inIgnoreSpace ) const
{
	AItemCount	targetLen = inText.GetLength();
	if( targetLen == 0 )   return false;
	//
	AIndex	endPos = GetItemCount();
	if( inEndPos != kIndex_Invalid )
	{
		endPos = inEndPos;
	}
	//this���̃e�L�X�g�̃|�C���^���擾
	AStTextPtr	textptr(*this,0,GetItemCount());
	AUChar*	ptr = textptr.GetPtr();
	//��r�Ώۃe�L�X�g�̃|�C���^���擾
	AStTextPtr	targettextptr(inText,0,inText.GetItemCount());
	AUChar*	targetptr = targettextptr.GetPtr();
	
	//this���e�L�X�g�̕������ƃ��[�v
	AIndex	pos = inPosition;
	while( pos <= endPos-targetLen )
	{
		ABool	same = true;
		//pos�ȍ~�̕�������ƍ�����
		AIndex	targetpos = 0;
		for( AIndex p = pos; p < endPos; p++ )
		{
			//this�������擾
			AUChar	ch = ptr[p];
			//inIgnoreSpace��true�̏ꍇ�A�X�y�[�X�����Ȃ�A�����������[�v�p��
			if( inIgnoreSpace == true )
			{
				if( ch == kUChar_Space || ch == kUChar_Tab || ch == kUChar_LineEnd )
				{
					continue;
				}
			}
			//��r�Ώۃe�L�X�g�̌��݈ʒu�̕����Ɣ�r���āA����Ă����烋�[�v�I���B�i�s��v�j
			if( ch != targetptr[targetpos] )
			{
				same = false;
				break;
			}
			//��r�Ώۃe�L�X�g�̌��݈ʒu�����̕����Ɉړ��B��r�Ώۃe�L�X�g��S���ǂ񂾂烋�[�v�I���B�i��v�j
			targetpos++;
			if( targetpos >= inText.GetItemCount() )
			{
				break;
			}
		}
		if( same == true ) 
		{
			outPosition = pos;
			return true;
		}
		pos++;
	}
	return false;
}

//#1231
/**
C�����񌟍�
*/
ABool	AText::FindCstring( const AConstCstringPtr inString ) const
{
	AIndex pos = 0;
	return FindCstring(pos,inString,pos);
}

ABool	AText::FindCstring( AIndex inPosition, const AConstCstringPtr inString, AIndex& outPosition, 
						   const ABool inIgnoreSpace, const ABool inProceedPositionAfterMatchedText ) const
{
	AText	target;
	target.SetCstring(inString);
	ABool	result = FindText(inPosition,target,outPosition,kIndex_Invalid,inIgnoreSpace);
	if( result == true && inProceedPositionAfterMatchedText == true )
	{
		outPosition += target.GetItemCount();
	}
	return result;
}

/**
�i���v�Z
*/
void	AText::CalcParagraphBreaks( AArray<AIndex>& outStartPosArray, AArray<AItemCount>& outLengthArray ) const
{
	//���ʏ�����
	outStartPosArray.DeleteAll();
	outLengthArray.DeleteAll();
	//�ŏ��̒i���J�n�ʒu
	outStartPosArray.Add(0);
	//���������[�v
	AIndex	lineStart = 0;
	for( AIndex p = 0; p < GetItemCount(); p++ )
	{
		if( Get(p) == kUChar_LineEnd )
		{
			//�i������
			outLengthArray.Add(p+1-lineStart);
			//���̒i���J�n�ʒu
			lineStart = p+1;
			//�i���J�n�ʒu
			outStartPosArray.Add(lineStart);
		}
	}
	//�Ō�̒i������
	outLengthArray.Add(GetItemCount()-lineStart);
}

//#193
/**
���K�\���̃O���[�v�Ɉ�v����e�L�X�g�𒊏o
*/
ABool	AText::ExtractUsingRegExp( const AConstCharPtr inRegExpString, const AIndex inExtractGroup, AText& outText ) const
{
	AText	re;
	re.SetCstring(inRegExpString);
	ARegExp	regexp;
	regexp.SetRE(re,false,false);
	AIndex	pos = 0;
	if( regexp.Match(*this,pos,GetItemCount()) == true )
	{
		if( inExtractGroup == 0 )
		{
			GetText(0,pos,outText);
		}
		else
		{
			AIndex	groupspos = 0, groupepos = 0;
			regexp.GetGroupRange(inExtractGroup-1,groupspos,groupepos);
			GetText(groupspos,groupepos-groupspos,outText);
		}
		return true;
	}
	else
	{
		return false;
	}
}

#pragma mark --- Localized Text

//��AbsFramework�p�̋��ʕ�����𕪂���K�v�L��

void	AText::SetLocalizedText( const AText& inKey )
{
#if IMPLEMENTATION_FOR_MACOSX
	AStCreateCFStringFromAText	keystrref(inKey);
	CFStringRef	strref = ::CFCopyLocalizedString(keystrref.GetRef(),"");
	if( strref == NULL )   return;
	SetFromCFString(strref);
	::CFRelease(strref);
#elif IMPLEMENTATION_FOR_WINDOWS
	CAppImp::GetLocalizedText(inKey,*this);
#else
	not implemented
#endif
}

void	AText::SetLocalizedText( AConstCharPtr inKey )
{
	AText	keyText;
	keyText.SetCstring(inKey);
	SetLocalizedText(keyText);
}

void	AText::SetLocalizedText( AConstCharPtr inKey, const ANumber inNumber )
{
	AText	keyText;
	keyText.SetCstring(inKey);
	keyText.AddFormattedCstring("%d",inNumber);
	SetLocalizedText(keyText);
}

void	AText::AddLocalizedText( AConstCharPtr inKey )
{
	AText	text;
	text.SetLocalizedText(inKey);
	AddText(text);
}

#pragma mark ---URL

void	AText::SetFTPURL( const AText& inConnectionType, 
						  const AText& inServer, const AText& inUser, const AText& inPath, const APortNumber inPortNumber )//#193
{
	//#193
	//�R�l�N�V�����^�C�v����"("�ȍ~���폜�����e�L�X�g��://�̑O�ɕt����
	SetText(inConnectionType);
	AIndex	foundpos = 0;
	if( FindCstring(0,"(",foundpos) == true )
	{
		DeleteAfter(foundpos);
	}
	ChangeCaseLower();
	//"://"
	AddCstring("://");
	//���[�U�[��
	AddText(inUser);
	//�T�[�o�[��
	AddCstring("@");
	AddText(inServer);
	//�|�[�g�ԍ� #193
	if( inPortNumber > 0 )
	{
		AddCstring(":");
		AddNumber(inPortNumber);
	}
	//�p�X
	AddText(inPath);
}

//#193
/**
URL�����񂩂�v���g�R���𒊏o
*/
void	AText::GetFTPURLProtocol( AText& outProtocol ) const
{
	outProtocol.DeleteAll();
	// "://"�̑O�܂ł𔲂��o��
	AIndex	foundpos;
	if( FindCstring(0,"://",foundpos) == false )   return;
	//
	GetText(0,foundpos,outProtocol);
}

void	AText::GetFTPURLServer( AText& outServer ) const
{
	outServer.DeleteAll();
	// "://"�ȍ~����A�ŏ���"/"�̑O�܂ł𔲂��o���B���̒���@�������@�ȍ~�A�Ȃ���ΑS�̂�Ԃ��B
	AIndex	foundpos;
	if( FindCstring(0,"://",foundpos) == false )   return;
	AIndex	spos = foundpos+3;
	foundpos = GetLength();
	FindCstring(spos,"/",foundpos);
	AText	text;
	AIndex	epos = foundpos;
	GetText(spos,epos-spos,text);
	if( text.FindCstring(0,"@",foundpos) == true )
	{
		text.FindCstring(foundpos+1,"@",foundpos);//���[�U�[����@���܂܂��ꍇ�̑΍�
		text.GetText(foundpos+1,text.GetLength()-(foundpos+1),outServer);
	}
	else
	{
		outServer.SetText(text);
	}
	// :�ȍ~�i�|�[�g�ԍ��j���폜����
	if( outServer.FindCstring(0,":",foundpos) == true )
	{
		outServer.DeleteAfter(foundpos);
	}
}

void	AText::GetFTPURLUser( AText& outUser ) const
{
	outUser.DeleteAll();
	// "://"�ȍ~����A�ŏ���"/"�̑O�܂ł𔲂��o���B���̒���@�������@���O�A�Ȃ����empty��Ԃ��B
	AIndex	foundpos;
	if( FindCstring(0,"://",foundpos) == false )   return;
	AIndex	spos = foundpos+3;
	foundpos = GetLength();
	FindCstring(spos,"/",foundpos);
	AText	text;
	AIndex	epos = foundpos;
	GetText(spos,epos-spos,text);
	if( text.FindCstring(0,"@",foundpos) == true )
	{
		text.FindCstring(foundpos+1,"@",foundpos);//���[�U�[����@���܂܂��ꍇ�̑΍�
		text.GetText(0,foundpos,outUser);
	}
}

void	AText::GetFTPURLPath( AText& outPath ) const
{
	outPath.DeleteAll();
	//"://"�ȍ~�A�ŏ���"/"�ȍ~��Ԃ��B
	AIndex	foundpos;
	if( FindCstring(0,"://",foundpos) == false )   return;
	AIndex	spos = foundpos+3;
	if( FindCstring(spos,"/",foundpos) == false )   return;
	GetText(foundpos,GetLength()-foundpos,outPath);
}

//#193
/**
URL�����񂩂�|�[�g�ԍ��𒊏o
*/
APortNumber	AText::GetFTPURLPortNumber() const
{
	AText	server;
	// "://"�ȍ~����A�ŏ���"/"�̑O�܂ł𔲂��o���B���̒���@�������@�ȍ~�A�Ȃ���ΑS�̂�Ԃ��B
	AIndex	foundpos;
	if( FindCstring(0,"://",foundpos) == false )   return 0;
	AIndex	spos = foundpos+3;
	foundpos = GetLength();
	FindCstring(spos,"/",foundpos);
	AText	text;
	AIndex	epos = foundpos;
	GetText(spos,epos-spos,text);
	if( text.FindCstring(0,"@",foundpos) == true )
	{
		text.FindCstring(foundpos+1,"@",foundpos);//���[�U�[����@���܂܂��ꍇ�̑΍�
		text.GetText(foundpos+1,text.GetLength()-(foundpos+1),server);
	}
	else
	{
		server.SetText(text);
	}
	// :������擾���A���l��
	if( server.FindCstring(0,":",foundpos) == true )
	{
		server.Delete(0,foundpos+1);
		return server.GetNumber();
	}
	else
	{
		return 0;
	}
}

void	AText::AddPathDelimiter( const AUChar inDelimiter )//win 080713
{
	ABool	exist = false;
	if( GetLength() > 0 )
	{
		if( Get(GetLength()-1) == /*'/' win 080713*/inDelimiter )   exist = true;
	}
	if( exist == false )
	{
		Add(/*'/' win 080713*/inDelimiter);
	}
}

void	AText::CatFTPURLPathText( const AText& inRelativePath )
{
	AddPathDelimiter(kUChar_Slash);//win 080713
	if( inRelativePath.GetLength() == 0 )   return;
	if( inRelativePath.Get(0) == '/' )
	{
		AIndex	pos = 0;
		//"://"������΁A���̌�܂Ői�߂�
		AIndex	foundpos;
		if( FindCstring(pos,"://",foundpos) == true )
		{
			pos = foundpos+3;
		}
		//�ŏ��ɂ���'/'�܂Ői�߂�
		FindCstring(pos,"/",foundpos);
		pos = foundpos;
		//����ȍ~���폜���āAinRelativePath����������
		DeleteAfter(pos);
		AddText(inRelativePath);
		return;
	}
	else
	{
		//inRelativePath����������ʒu��catpos�ɋ��߂�B�ŏ��͍Ō������n�߂�B
		AIndex	catpos = GetLength();
		//�܂�catpos���P�O��/�̒���ɂ���B
		{
			catpos -= 2;
			GoBackToChar(catpos,'/');
			catpos++;
		}
		//inRelativePath�̍ŏ�����"./"��"../"��T���Ă���
		AIndex	pos = 0;
		AItemCount	len = inRelativePath.GetLength();
		while( pos < len )
		{
			//"./"�������牽�������ɐi�߂�
			if( pos+1 < len )
			{
				if( inRelativePath.Get(pos) == '.' && inRelativePath.Get(pos+1) == '/' )
				{
					pos += 2;
					continue;
				}
			}
			//"../"��������catpos���P�O��/�̒���ɂ���B
			if( pos+2 < len )
			{
				if( inRelativePath.Get(pos) == '.' && inRelativePath.Get(pos+1) == '.' && inRelativePath.Get(pos+2) == '/' )
				{
					pos += 3;
					catpos -= 2;
					GoBackToChar(catpos,'/');
					catpos++;
					continue;
				}
			}
			//"./"��"../"�ȊO��������catpos�ȍ~���폜���āAinRelativePath�̌��݈ʒu�ȍ~����������
			DeleteAfter(catpos);
			AText	text = inRelativePath;
			text.Delete(0,pos);
			AddText(text);
			return;
		}
	}
}

//�t�@�C�����E�t�H���_���擾
//�Ō�̕����ȊO�ŁA�Ō��'/'���o������ӏ�������A�t�@�C�����E�t�H���_���Ƃ݂Ȃ��B�������A�Ōオ/�Ȃ炻��͍폜
void	AText::GetFilename( AText& outFilename ) const
{
	//
	AIndex pos = GetItemCount()-2;
	if( pos < 0 )   pos = 0;
	for( ; pos > 0; pos -- )
	{
		if( Get(pos) == /*win 080703 kUChar_Slash*/AFileAcc::GetPathDelimiter() )
		{
			pos++;
			break;
		}
	}
	GetText(pos,GetItemCount()-pos,outFilename);
	if( GetLastChar() == /*win 080703 kUChar_Slash*/AFileAcc::GetPathDelimiter() )   outFilename.Delete(outFilename.GetItemCount()-1,1);
}

//#892
/**
�Ō�̃p�X��؂蕶�����폜
*/
void	AText::RemoveLastPathDelimiter()
{
	if( GetItemCount() > 0 )
	{
		if( GetLastChar() == AFileAcc::GetPathDelimiter() )   Delete(GetItemCount()-1,1);
	}
}

//#892
/**
�Ō�̃p�X��؂蕶���łȂ���΁A�p�X��؂蕶����ǉ�
*/
void	AText::AddLastPathDelimiter()
{
	if( GetItemCount() > 0 )
	{
		if( GetLastChar() != AFileAcc::GetPathDelimiter() )
		{
			Add(AFileAcc::GetPathDelimiter());
		}
	}
	else
	{
		Add(AFileAcc::GetPathDelimiter());
	}
}

#pragma mark ===========================

#pragma mark ---URL

//#427
/**
�qURL��ݒ�
*/
void	AText::SetChildURL( const AText& inParentURL, const AText& inChildName )
{
	//�eURL�ݒ�
	SetText(inParentURL);
	//�Ō�̕�����/�łȂ����/��ǉ�
	if( GetItemCount() > 0 )
	{
		if( Get(GetItemCount()-1) != '/' )   Add('/');
	}
	//�qname�ǉ�
	AddText(inChildName);
}

//#427
/**
�qURL��ݒ�
*/
void	AText::SetChildURL( const AText& inParentURL, const AConstCstringPtr inChildName )
{
	AText	childname(inChildName);
	SetChildURL(inParentURL,childname);
}

//#361
/**
URL�p�X�ɂ��āA�e�t�H���_�̃p�X���擾
*/
ABool	AText::GetParentURL( AText& outParentURL ) const
{
	outParentURL.DeleteAll();
	for( AIndex pos = GetItemCount()-2; pos > 0; pos-- )
	{
		if( Get(pos) == '/' )
		{
			GetText(0,pos+1,outParentURL);
			return true;
		}
	}
	return false;
}

//#363
/**
URL�G���R�[�h���s
RFC3986
*/
void	AText::ConvertToURLEncode()
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AUChar	ch = Get(i);
		switch(ch)
		{
			//reserved    = gen-delims / sub-delims
			//gen-delims  = ":" / "/" / "?" / "#" / "[" / "]" / "@"
			// sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"/ "*" / "+" / "," / ";" / "="
		  case ':':
		  case '/':
		  case '?':
		  case '#':
		  case '[':
		  case ']':
		  case '@':
		  case '!':
		  case '$':
		  case '&':
		  case '\'':
		  case '(':
		  case ')':
		  case '*':
		  case '+':
		  case ',':
		  case ';':
		  case '=':
			//unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
		  case '-':
		  case '.':
		  case '_':
		  case '~':
			//%
		  case '%':
			{
				t.Add(ch);
				break;
			}
		  default:
			//ALPHA / DIGIT
			if( (ch>='A'&&ch<='Z') || (ch>='a'&&ch<='z') || (ch>='0'&&ch<='9') )
			{
				t.Add(ch);
				break;
			}
			//%-escape
			else
			{
				t.Add('%');
				t.AddFormattedCstring("%02X",ch);
				break;
			}
		}
	}
	SetText(t);
}

//R0000
void	AText::ConvertToURLEscape()
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		t.Add('%');
		t.AddFormattedCstring("%02X",Get(i));
	}
	SetText(t);
}

//#358
/**
*/
void	AText::ConvertFromURLEscape()
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		if( Get(i) == '%' && i+2 < GetItemCount() )
		{
			AUChar	ch = (GetIntegerByHexChar(i+1))*0x10 + GetIntegerByHexChar(i+2);
			t.Add(ch);
			i += 2;
		}
		else
		{
			t.Add(Get(i));
		}
	}
	SetText(t);
}

#pragma mark --- Pref Text Escape

void	AText::ConvertToPrefTextEscape()
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AUChar	ch = Get(i);
		if( ch == '\r' )
		{
			t.Add('\\');
			t.Add('r');
		}
		else if( ch == '\"' )
		{
			t.Add('\\');
			t.Add('\"');
		}
		else if( ch == '\\' )
		{
			t.Add('\\');
			t.Add('\\');
		}
		else
		{
			t.Add(ch);
		}
	}
	SetText(t);
}

void	AText::ExtractUnescapedTextFromPrefText( AIndex& ioPos, AText& outText, const AUChar inTerminateChar ) const
{
	outText.DeleteAll();
	for( ; ioPos < GetItemCount(); ioPos++ )
	{
		AUChar	ch = Get(ioPos);
		if( ch == inTerminateChar )   break;
		if( ch == '\\' )
		{
			ioPos++;
			ch = Get(ioPos);
			if( ch == 'r' )
			{
				ch = '\r';
			}
		}
		outText.Add(ch);
	}
}

#pragma mark --- �ėpescaped text

/**
���s�R�[�h�^�_�u���N�I�[�e�[�V�������G�X�P�[�v
*/
void	AText::ConvertToEscapedText( const ABool inEscapeLineEnd, const ABool inEscapeDoubleQuotation )
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AUChar	ch = Get(i);
		if( ch == '\r' && inEscapeLineEnd == true )
		{
			t.Add('\\');
			t.Add('r');
		}
		else if( ch == '\"' && inEscapeDoubleQuotation == true )
		{
			t.Add('\\');
			t.Add('\"');
		}
		else if( ch == '\\' )
		{
			t.Add('\\');
			t.Add('\\');
		}
		else
		{
			t.Add(ch);
		}
	}
	SetText(t);
}

/**
�G�X�P�[�v���ꂽ��������G�X�P�[�v����
*/
void	AText::ConvertToUnescapedText() 
{
	AText	t;
	for( AIndex pos = 0; pos < GetItemCount(); pos++ )
	{
		AUChar	ch = Get(pos);
		if( ch == '\\' )
		{
			pos++;
			ch = Get(pos);
			if( ch == 'r' )
			{
				ch = '\r';
			}
		}
		t.Add(ch);
	}
	SetText(t);
}


#pragma mark --- ���K�\���pescaped text

/**
�ʏ�̃e�L�X�g�𐳋K�\���G�X�P�[�v�e�L�X�g�ɕϊ�
*/
void	AText::ConvertToRegExpEscapedText()
{
	AText	t;
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AUChar	ch = Get(i);
		switch(ch)
		{
		  case '(':
			{
				t.AddCstring("\\(");
				break;
			}
		  case ')':
			{
				t.AddCstring("\\)");
				break;
			}
		  case '[':
			{
				t.AddCstring("\\[");
				break;
			}
		  case ']':
			{
				t.AddCstring("\\]");
				break;
			}
		  case '{':
			{
				t.AddCstring("\\{");
				break;
			}
		  case '}':
			{
				t.AddCstring("\\}");
				break;
			}
		  case '\\':
			{
				t.AddCstring("\\\\");
				break;
			}
		  case '*':
			{
				t.AddCstring("\\*");
				break;
			}
		  case '+':
			{
				t.AddCstring("\\+");
				break;
			}
		  case '?':
			{
				t.AddCstring("\\?");
				break;
			}
		  case '|':
			{
				t.AddCstring("\\|");
				break;
			}
		  case '^':
			{
				t.AddCstring("\\^");
				break;
			}
		  case '$':
			{
				t.AddCstring("\\$");
				break;
			}
		  case '.':
			{
				t.AddCstring("\\.");
				break;
			}
		  default:
			{
				t.Add(ch);
				break;
			}
		}
	}
	SetText(t);
}

#pragma mark --- Cocoa

//#1034
/**
AText����NSString�𐶐�����
*/
NSString*	AText::CreateNSString( const ABool inAutorelease ) const
{
	AStCreateNSStringFromAText	str(*this);
	NSString*	string = str.GetNSString();
	[string retain];
	if( inAutorelease == true )
	{
		[string autorelease];
	}
	return string;
}

//#1034
/**
NSString����AText�փe�L�X�g�ݒ肷��
*/
void	AText::SetFromNSString( const NSString* inNSString )
{
	ACocoa::SetTextFromNSString(inNSString,*this);
}

#pragma mark �f�o�b�O

void AText::OutputToStderr() const
{
#if IMPLEMENTATION_FOR_MACOSX
	AStCreateCstringFromAText	cstr(*this);
	fprintf(stderr,"%s",cstr.GetPtr());
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	AStCreateWcharStringFromAText	wcstr(*this);
	OutputDebugStringW(wcstr.GetPtr());
#endif
}

void	AText::OutputToStderrHex() const
{
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		fprintf(stderr,"%02X ",Get(i));
	}
	fprintf(stderr,"\n");
}

#pragma mark ===========================
#pragma mark [�N���X]AStCreateCstringFromAText
#pragma mark ===========================
//AText����C������|�C���^���擾���邽�߂̃N���X
//�i�X�^�b�N������p�N���X�j

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//��������AText���m��ł���ꍇ�p
AStCreateCstringFromAText::AStCreateCstringFromAText( const AText& inText )
{
	Create(inText);
}

//��������AText���m��ł��Ȃ��ꍇ�p
AStCreateCstringFromAText::AStCreateCstringFromAText()
{
}

void AStCreateCstringFromAText::Create( const AText& inText )
{
	//mCstringText�ցA����AText+NULL�������i�[����B
	mCstringText.SetText(inText);
	mCstringText.Add(0);
	//���b�N
	mCstringText.LockByArrayPtr();
}

//�f�X�g���N�^
AStCreateCstringFromAText::~AStCreateCstringFromAText()
{
	//�A�����b�N
	mCstringText.UnlockByArrayPtr();//�����K�v�@mCstringText�̐���
}

#pragma mark ---�|�C���^�擾
//C������̃|�C���^�Ƃ��Ď擾
ACstringPtr AStCreateCstringFromAText::GetPtr()
{
	return reinterpret_cast<ACstringPtr>(mCstringText.GetDataPtr(0));
}

#if IMPLEMENTATION_FOR_WINDOWS
#pragma mark ===========================
#pragma mark [�N���X]AStCreateWcharStringFromAText
#pragma mark ===========================
//AText����wchar_t������|�C���^���擾���邽�߂̃N���X
//�i�X�^�b�N������p�N���X�j

//��������AText���m��ł���ꍇ�p
AStCreateWcharStringFromAText::AStCreateWcharStringFromAText( const AText& inText )
{
	Create(inText);
}

//��������AText���m��ł��Ȃ��ꍇ�p
AStCreateWcharStringFromAText::AStCreateWcharStringFromAText()
{
}

void AStCreateWcharStringFromAText::Create( const AText& inText )
{
	mWcharText.SetText(inText);
	mWcharText.Add(0);
	mWcharText.ConvertFromUTF8ToAPIUnicode();
	//���b�N
	mWcharText.LockByArrayPtr();
}

//�f�X�g���N�^
AStCreateWcharStringFromAText::~AStCreateWcharStringFromAText()
{
	//�A�����b�N
	mWcharText.UnlockByArrayPtr();//�����K�v�@mCstringText�̐���
}

//Wide������̃|�C���^�Ƃ��Ď擾
const wchar_t* AStCreateWcharStringFromAText::GetPtr()
{
	return reinterpret_cast<wchar_t*>(mWcharText.GetDataPtr(0));
}

//Wide������̃|�C���^�Ƃ��Ď擾
wchar_t* AStCreateWcharStringFromAText::GetWritablePtr()
{
	return reinterpret_cast<wchar_t*>(mWcharText.GetDataPtr(0));
}

//Wide������̕��������擾
int	AStCreateWcharStringFromAText::GetWcharCount() const
{
	return mWcharText.GetItemCount()/sizeof(wchar_t);
}

#endif

#pragma mark ===========================
#pragma mark [�N���X]AUniText
#pragma mark ===========================

void	AUniText::InsertFromPtr( AIndex inIndex, AUTF16CharPtr inSrcPtr, AItemCount inItemCount )
{
	//�̈�m��
	try
	{
		Reserve(inIndex,inItemCount);
	}
	catch(...)
	{
		throw 0;
	}
	//�R�s�[���|�C���^�擾
	AStUniTextPtr	textptr(*this,inIndex,inItemCount);
	//�R�s�[���s
	AMemoryWrapper::Memmove(textptr.GetPtr(),inSrcPtr,textptr.GetByteCount());
}






