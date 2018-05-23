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

ATextArray

*/

#include "stdafx.h"

#include "ATextArray.h"
#if 0
#pragma mark ===========================
#pragma mark [�N���X]ATextArray
#pragma mark ===========================
//AText�̔z��

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
ATextArray::ATextArray( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent)
{
}
//�f�X�g���N�^
ATextArray::~ATextArray()
{
}

void	ATextArray::Insert1( const AIndex inIndex, const AText& inText )
{
	mTextArray.InsertNew1Object(inIndex);
	Set(inIndex,inText);
}

void	ATextArray::Insert1_SwapContent( const AIndex inIndex, AText& inText )
{
	mTextArray.InsertNew1Object(inIndex);
	Set_SwapContent(inIndex,inText);
}

void	ATextArray::Delete1( const AIndex inIndex )
{
	mTextArray.Delete1Object(inIndex);
}

void	ATextArray::Set( const AIndex inIndex, const AText& inText )
{
	mTextArray.GetObject(inIndex).SetText(inText);
}

void	ATextArray::Set_SwapContent( const AIndex inIndex, AText& ioText )
{
	mTextArray.GetObject(inIndex).SwapContent(ioText);
}

void	ATextArray::Get( const AIndex inIndex, AText& outText ) const
{
	outText.SetText(mTextArray.GetObjectConst(inIndex));
}

const AText&	ATextArray::GetTextConst( const AIndex inIndex ) const
{
	return mTextArray.GetObjectConst(inIndex);
}

void	ATextArray::Add( const ATextArray& inTextArray )
{
	AItemCount	count = inTextArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AText	text;
		inTextArray.Get(i,text);
		Add(text);
	}
}

void	ATextArray::Set( const ATextArray& inTextArray )
{
	DeleteAll();
	Add(inTextArray);
}

void	ATextArray::Add( const AText& inText )
{
	Insert1(GetItemCount(),inText);
}

void	ATextArray::Add_SwapContent( AText& inText )
{
	Insert1_SwapContent(GetItemCount(),inText);
}

void	ATextArray::Add( AConstCharPtr inText )
{
	AText	text(inText);
	Add(text);
}

void	ATextArray::MoveObject( const AIndex inOldIndex, const AIndex inNewIndex )
{
	mTextArray.MoveObject(inOldIndex,inNewIndex);
}

void	ATextArray::DeleteAll()
{
	while( GetItemCount() > 0 )
	{
		Delete1(GetItemCount()-1);
	}
}

void	ATextArray::Implode( const AChar& inChar, AText& outText ) const
{
	outText.DeleteAll();
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AText	text;
		Get(i,text);
		outText.AddText(text);
		outText.Add(inChar);
	}
}

//�w��f���~�^(inDelimiter)�ŕ��������؂�ATextArray�ɏ��Ɋi�[����B
//�f���~�^��2�����A������ꍇ�́A��AText���i�[�����
//�Ō�̕������f���~�^�̏ꍇ�́A�Ō�ɋ�AText���i�[�����
void	ATextArray::ExplodeFromText( const AText& inText, const AUChar inDelimiter )
{
	//�ŏ���TextArray�S����
	DeleteAll();
	//�f���~�^�������閈��TextArray�ɒǉ�
	AIndex	spos = 0;
	AIndex pos = 0;
	for( ; pos < inText.GetItemCount(); pos++ )
	{
		AUChar	ch = inText.Get(pos);
		if( ch == inDelimiter )
		{
			AText	text;
			inText.GetText(spos,pos-spos,text);
			Add(text);
			spos = pos+1;
		}
	}
	//�Ō�̃f���~�^�ȍ~�̕�����ǉ�
	AText	text;
	inText.GetText(spos,pos-spos,text);
	Add(text);
}

//
void	ATextArray::ExplodeLineFromText( const AText& inText )
{
	//CR�֕ϊ�
	AText	text;
	text.SetText(inText);
	text.ConvertReturnCodeToCR();
	//CR��Explode
	ExplodeFromText(text,kUChar_CR);
}

AIndex	ATextArray::Find( const AText& inText ) const
{
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AText	text;
		Get(i,text);
		if( text.Compare(inText) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

AIndex	ATextArray::FindIgnoreCase( const AText& inText ) const
{
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AText	text;
		Get(i,text);
		if( text.CompareIgnoreCase(inText) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

AItemCount	ATextArray::GetTextLength( const AIndex inIndex ) const
{
	return mTextArray.GetObjectConst(inIndex).GetItemCount();
}

void	ATextArray::Sort( const ABool inAscendant )
{
	if( GetItemCount() < 2 )   return;
	QuickSort(0,GetItemCount()-1,inAscendant);
}

void	ATextArray::QuickSort( AIndex first, AIndex last, const ABool inAscendant )
{
	AText	axis;
	Get((first+last)/2,axis);
	AIndex	i = first, j = last;
	for( ; ; )
	{
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( GetTextConst(i).IsLessThan(axis) == true )
				{
					i++;
					continue;
				}
			}
			else
			{
				if( axis.IsLessThan(GetTextConst(i)) == true )
				{
					i++;
					continue;
				}
			}
			break;
		}
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( axis.IsLessThan(GetTextConst(j)) == true )
				{
					j--;
					continue;
				}
			}
			else
			{
				if( GetTextConst(j).IsLessThan(axis) == true )
				{
					j--;
					continue;
				}
			}
			break;
		}
		if( i >= j )   break;
		mTextArray.SwapObject(i,j);
		i++;
		j--;
	}
	if( first < i-1 )   QuickSort(first,i-1,inAscendant);
	if( j+1 < last  )   QuickSort(j+1,last,inAscendant);
}

#pragma mark ===========================
#pragma mark [�N���X]AHashTextArray
#pragma mark ===========================
//AText�̔z��i�n�b�V������j

AHashTextArray::AHashTextArray( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent)
{
}

AHashTextArray::~AHashTextArray()
{
}

void	AHashTextArray::Insert1( const AIndex inIndex, const AText& inText )
{
	mTextArray.InsertNew1Object(inIndex);
	mTextArray.ObjectUpdateStart(inIndex);
	mTextArray.GetObject(inIndex).SetText(inText);
	mTextArray.ObjectUpdateEnd(inIndex);
}

AIndex	AHashTextArray::Add( const AText& inText )
{
	AIndex	index = GetItemCount();
	AHashTextArray::Insert1(index,inText);
	return index;
}

void	AHashTextArray::Delete1( const AIndex inIndex )
{
	mTextArray.Delete1Object(inIndex);
}

void	AHashTextArray::DeleteAll()
{
	while( GetItemCount() > 0 )
	{
		Delete1(GetItemCount()-1);
	}
}

void	AHashTextArray::Set( const AIndex inIndex, const AText& inText )
{
	mTextArray.ObjectUpdateStart(inIndex);
	mTextArray.GetObject(inIndex).SetText(inText);
	mTextArray.ObjectUpdateEnd(inIndex);
}

void	AHashTextArray::Get( const AIndex inIndex, AText& outText ) const
{
	outText.SetText(mTextArray.GetObjectConst(inIndex));
}

const AText&	AHashTextArray::GetTextConst( const AIndex inIndex ) const
{
	return mTextArray.GetObjectConst(inIndex);
}

//#348
/**
�e�L�X�g��r
*/
ABool	AHashTextArray::Compare( const AIndex inIndex, const AText& inText ) const
{
	return GetTextConst(inIndex).Compare(inText);
}

AIndex	AHashTextArray::Find( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	return mTextArray.Find(inTargetText,inTargetIndex,inTargetItemCount);
}

//#0
/**
��v����e�L�X�g��ObjectID�̃��X�g���擾
*/
void	AHashTextArray::FindObjectID( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount,
		AArray<AObjectID> &outObjectIDArray ) const
{
	mTextArray.FindObjectID(inTargetText,inTargetIndex,inTargetItemCount,outObjectIDArray);
}

//#348
AObjectID	AHashTextArray::FindObjectID( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	return mTextArray.FindObjectID(inTargetText,inTargetIndex,inTargetItemCount);
}

AObjectID	AHashTextArray::GetObjectIDOfText( const AIndex inIndex ) const
{
	return mTextArray.GetObjectConst(inIndex).GetObjectID();
}

AIndex	AHashTextArray::GetIndexByID( const AObjectID inObjectID ) const
{
	return mTextArray.GetIndexByID(inObjectID);
}
#endif

#pragma mark ===========================
#pragma mark [�N���X]ATextArray
#pragma mark ===========================
//#348

/**
�R���X�g���N�^
*/
ATextArray::ATextArray( AObjectArrayItem* inParent ) : AObjectArrayItem(NULL)
{
}

/**
�f�X�g���N�^
*/
ATextArray::~ATextArray()
{
}

#pragma mark ===========================

#pragma mark ---�ǉ��E�폜

/**
�ǉ�
*/
void	ATextArray::Insert1( const AIndex inIndex, const AText& inText )
{
	//�}���ʒu�擾
	AIndex	spos;
	if( inIndex >= mSposArray.GetItemCount() )
	{
		spos = mTextData.GetItemCount();
	}
	else
	{
		spos = mSposArray.Get(inIndex);
	}
	//�e�L�X�g�f�[�^�}��
	mTextData.InsertText(spos,inText);
	//�ʒu�f�[�^�}��
	mSposArray.Insert1(inIndex,spos);
	//�ǉ��ʒu�ȍ~�����炷
	AItemCount	len = inText.GetItemCount();
	/*#695 ������
	for( AIndex i = inIndex+1; i < mSposArray.GetItemCount(); i++ )
	{
		mSposArray.Set(i,mSposArray.Get(i)+len);
	}
	*/
	{
		AItemCount	itemCount = mSposArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mSposArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = inIndex+1; i < itemCount; i++ )
		{
			p[i] += len;
		}
	}
}

/**
�폜
*/
void	ATextArray::Delete1( const AIndex inIndex )
{
	//�폜�͈͎擾
	AIndex	spos, epos;
	GetRange(inIndex,spos,epos);
	//�e�L�X�g�f�[�^�폜
	mTextData.Delete(spos,epos-spos);
	//�ʒu�f�[�^�폜
	mSposArray.Delete1(inIndex);
	//���炷
	AItemCount	len = epos-spos;
	/*#695 ������
	for( AIndex i = inIndex; i < mSposArray.GetItemCount(); i++ )
	{
		mSposArray.Set(i,mSposArray.Get(i)-len);
	}
	*/
	{
		AItemCount	itemCount = mSposArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mSposArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = inIndex; i < itemCount; i++ )
		{
			p[i] -= len;
		}
	}
}

/**
�ǉ�
*/
AIndex	ATextArray::Add( const AText& inText )
{
	Insert1(mSposArray.GetItemCount(),inText);
	return mSposArray.GetItemCount()-1;
}

/**
�S�폜
*/
void	ATextArray::DeleteAll()
{
	/*#695
	while( mSposArray.GetItemCount() > 0 )
	{
		Delete1(mSposArray.GetItemCount()-1);
	}
	*/
	mTextData.DeleteAll();
	mSposArray.DeleteAll();
}

/**
�w��index�ȍ~���폜
*/
void	ATextArray::DeleteAfter( const AIndex inIndex )
{
	//���ڐ��ȍ~���폜�̏ꍇ�́A�������Ȃ��ŏI���iAArray::DeleteAfter()�����s���Ă���薳�����AmSposArray.Get()�ŃG���[�ɂȂ�̂ŁB�j
	if( inIndex == GetItemCount() )
	{
		return;
	}
	
	//mTextData�̎w��index�̊J�n�ʒu�ȍ~���폜����
	AIndex	spos = mSposArray.Get(inIndex);
	mTextData.DeleteAfter(spos);
	//mSposArray�̎w��index�ȍ~���폜����
	mSposArray.DeleteAfter(inIndex);
}


//#695
/**
�̈�m�ہi��e�L�X�g���w��inIndex����inCount�ݒ肳���j
*/
void	ATextArray::Reserve( const AIndex inIndex, const AItemCount inCount )
{
	//�}�����悤�Ƃ��Ă���index�́AmTextData��pos���擾
	AIndex	pos = mTextData.GetItemCount();
	if( inIndex < mSposArray.GetItemCount() )
	{
		pos = mSposArray.Get(inIndex);
	}
	//spos array�̗̈�m��
	mSposArray.Reserve(inIndex,inCount);
	//�m�ۂ���spos array�̍��ڂɁA��Ŏ擾����pos��ݒ�i�S�Ă̍��ڂɓ���pos�j
	AStArrayPtr<AIndex>	arrayptr(mSposArray,0,mSposArray.GetItemCount());
	AIndex*	p = arrayptr.GetPtr();
	for( AIndex i = inIndex; i < inIndex + inCount; i++ )
	{
		p[i] = pos;
	}
}

#pragma mark ===========================

#pragma mark ---�ݒ�E�擾

/**
�e�L�X�g�ݒ�
*/
void	ATextArray::Set( const AIndex inIndex, const AText& inText )
{
	//�폜�͈͎擾
	AIndex	spos, epos;
	GetRange(inIndex,spos,epos);
	//�e�L�X�g�f�[�^�폜
	mTextData.Delete(spos,epos-spos);
	//�e�L�X�g�f�[�^�}��
	mTextData.InsertText(spos,inText);
	//���炷
	AIndex	offset = inText.GetItemCount()-(epos-spos);
	/*#695 ������
	for( AIndex i = inIndex+1; i < mSposArray.GetItemCount(); i++ )
	{
		mSposArray.Set(i,mSposArray.Get(i)+offset);
	}
	*/
	{
		AItemCount	itemCount = mSposArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mSposArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = inIndex+1; i < itemCount; i++ )
		{
			p[i] += offset;
		}
	}
}

/**
�e�L�X�g�擾
*/
void	ATextArray::Get( const AIndex inIndex, AText& outText ) const
{
	//�͈͎擾
	AIndex	spos, epos;
	GetRange(inIndex,spos,epos);
	//�e�L�X�g�f�[�^�擾
	mTextData.GetText(spos,epos-spos,outText);
}

//#693 ATextArray��AText�I�u�W�F�N�g�̔z�񂾂����Ƃ��́A����AText�ւ̎Q�Ƃ�Ԃ��Ă������A
//AText�I�u�W�F�N�g�̔z��ł͂Ȃ��Ȃ����̂ŁA�I�u�W�F�N�g�̃e���|�����R�s�[��Ԃ��悤�ɕύX�B
/**
const Text���擾
�����x�����K�v
*/
const AText	ATextArray::GetTextConst( const AIndex inIndex ) const
{
	AText	result;
	Get(inIndex,result);
	return result;
}

/**
�e�L�X�g��r
*/
ABool	ATextArray::Compare( const AIndex inIndex, const AText& inText ) const
{
	//�͈͎擾
	AIndex	spos, epos;
	GetRange(inIndex,spos,epos);
	//�e�L�X�g�f�[�^��r
	return inText.Compare(mTextData,spos,epos-spos);
}

/**
�e�L�X�g��r #695
*/
ABool	ATextArray::Compare( const AIndex inIndex, 
		const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	//�͈͎擾
	AIndex	spos, epos;
	GetRange(inIndex,spos,epos);
	//�e�L�X�g�f�[�^��r
	return inTargetText.Compare(inTargetIndex,inTargetItemCount,mTextData,spos,epos-spos);
}

/**
�w��index�̃e�L�X�g�̒������擾
*/
AItemCount	ATextArray::GetTextLen( const AIndex inIndex ) const
{
	AIndex	spos = 0, epos = 0;
	GetRange(inIndex,spos,epos);
	return epos-spos;
}

/**
�͈͎擾
*/
void	ATextArray::GetRange( const AIndex inIndex, AIndex& spos, AIndex& epos ) const
{
	spos = mSposArray.Get(inIndex);
	if( inIndex+1 >= mSposArray.GetItemCount() )
	{
		epos = mTextData.GetItemCount();
	}
	else
	{
		epos = mSposArray.Get(inIndex+1);
	}
}

/**
�ړ�
*/
void	ATextArray::MoveObject( const AIndex inOldIndex, const AIndex inNewIndex )
{
	AText	text;
	Get(inOldIndex,text);
	Delete1(inOldIndex);
	Insert1(inNewIndex,text);
}

/**
Swap
*/
void	ATextArray::SwapObject( const AIndex inA, const AIndex inB )
{
	AText	textA, textB;
	Get(inA,textA);
	Get(inB,textB);
	Set(inA,textB);
	Set(inB,textA);
}

/**
ATextArray����ǉ�
*/
void	ATextArray::AddFromTextArray( const ATextArray& inTextArray )
{
	AItemCount	count = inTextArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AText	text;
		inTextArray.Get(i,text);
		Add(text);
	}
}

/**
ATextArray����ݒ�
*/
void	ATextArray::SetFromTextArray( const ATextArray& inTextArray )
{
	mTextData.SetText(inTextArray.mTextData);
	mSposArray.SetFromObject(inTextArray.mSposArray);
}

/**
AHashTextArray����ǉ�
*/
void	ATextArray::AddFromTextArray( const AHashTextArray& inTextArray )
{
	AItemCount	count = inTextArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AText	text;
		inTextArray.Get(i,text);
		Add(text);
	}
}

/**
ATextArray����ݒ�
*/
void	ATextArray::SetFromTextArray( const AHashTextArray& inTextArray )
{
	DeleteAll();
	AddFromTextArray(inTextArray);
}

#pragma mark ===========================

#pragma mark ---����

/**
����
*/
AIndex	ATextArray::Find( const AText& inTargetText, const AIndex inTargetIndex, 
		const AItemCount inTargetItemCount ) const
{
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		/*#723
		AText	text;
		Get(i,text);
		if( text.Compare(inTargetText,inTargetIndex,inTargetItemCount) == true )
		{
			return i;
		}
		*/
		//������ #723
		if( Compare(i,inTargetText,inTargetIndex,inTargetItemCount) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

//#723
/**
�����iIndex�擾�E��v������̑S�āj
*/
void	ATextArray::FindAll( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount, 
		AArray<AIndex>& outFoundIndex ) const
{
	//���ʃN���A
	outFoundIndex.DeleteAll();
	//�e���ڂƔ�r
	AItemCount	itemCount = GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( Compare(i,inTargetText,inTargetIndex,inTargetItemCount) == true )
		{
			outFoundIndex.Add(i);
		}
	}
}

/**
�����i�啶�������������j
*/
AIndex	ATextArray::FindIgnoreCase( const AText& inText ) const
{
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AText	text;
		Get(i,text);
		if( text.CompareIgnoreCase(inText) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

/**
TextArray�̊e�v�f���w��f���~�^�ŋ�؂���Text�𐶐�����B
�Ō�ɂ��f���~�^���t��
*/
void	ATextArray::Implode( const AChar& inChar, AText& outText ) const
{
	outText.DeleteAll();
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		AText	text;
		Get(i,text);
		outText.AddText(text);
		outText.Add(inChar);
	}
}

//�w��f���~�^(inDelimiter)�ŕ��������؂�ATextArray�ɏ��Ɋi�[����B
//�f���~�^��2�����A������ꍇ�́A��AText���i�[�����
//�Ō�̕������f���~�^�̏ꍇ�́A�Ō�ɋ�AText���i�[�����
void	ATextArray::ExplodeFromText( const AText& inText, const AUChar inDelimiter, const ABool inRemoveEmptyItem )
{
	//�ŏ���TextArray�S����
	DeleteAll();
	//�f���~�^�������閈��TextArray�ɒǉ�
	AIndex	spos = 0;
	AIndex pos = 0;
	for( ; pos < inText.GetItemCount(); pos++ )
	{
		AUChar	ch = inText.Get(pos);
		if( ch == inDelimiter )
		{
			AText	text;
			inText.GetText(spos,pos-spos,text);
			if( inRemoveEmptyItem == false || text.GetItemCount() > 0 )
			{
				Add(text);
			}
			spos = pos+1;
		}
	}
	//�Ō�̃f���~�^�ȍ~�̕�����ǉ�
	AText	text;
	inText.GetText(spos,pos-spos,text);
	if( inRemoveEmptyItem == false || text.GetItemCount() > 0 )
	{
		Add(text);
	}
}

//
void	ATextArray::ExplodeLineFromText( const AText& inText )
{
	//CR�֕ϊ�
	AText	text;
	text.SetText(inText);
	text.ConvertReturnCodeToCR();
	//CR��Explode
	ExplodeFromText(text,kUChar_CR);
}

/**
�\�[�g
*/
void	ATextArray::Sort( const ABool inAscendant )
{
	if( GetItemCount() < 2 )   return;
	QuickSort(0,GetItemCount()-1,inAscendant);
}

void	ATextArray::QuickSort( AIndex first, AIndex last, const ABool inAscendant )
{
	AText	axis;
	Get((first+last)/2,axis);
	AIndex	i = first, j = last;
	for( ; ; )
	{
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( GetTextConst(i).IsLessThan(axis) == true )
				{
					i++;
					continue;
				}
			}
			else
			{
				if( axis.IsLessThan(GetTextConst(i)) == true )
				{
					i++;
					continue;
				}
			}
			break;
		}
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( axis.IsLessThan(GetTextConst(j)) == true )
				{
					j--;
					continue;
				}
			}
			else
			{
				if( GetTextConst(j).IsLessThan(axis) == true )
				{
					j--;
					continue;
				}
			}
			break;
		}
		if( i >= j )   break;
		SwapObject(i,j);
		i++;
		j--;
	}
	if( first < i-1 )   QuickSort(first,i-1,inAscendant);
	if( j+1 < last  )   QuickSort(j+1,last,inAscendant);
}

//#717
/**
�\�[�g
*/
/*���
void	ATextArray::SortUsingOrderArray( const ABool inAscendant, AArray<AIndex>& outOrderArray )
{
	if( GetItemCount() < 2 )   return;
	//
	outOrderArray.DeleteAll();
	for( AIndex i = 0; i < GetItemCount(); i++ )
	{
		outOrderArray.Add(i);
	}
	//
	QuickSortUsingOrderArray(0,GetItemCount()-1,inAscendant,outOrderArray);
}

void	ATextArray::QuickSortUsingOrderArray( const AIndex first, const AIndex last, const ABool inAscendant, AArray<AIndex>& ioOrderArray )
{
	AText	axis;
	Get((first+last)/2,axis);
	AIndex	i = first, j = last;
	for( ; ; )
	{
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( GetTextConst(i).IsLessThan(axis) == true )
				{
					i++;
					continue;
				}
			}
			else
			{
				if( axis.IsLessThan(GetTextConst(i)) == true )
				{
					i++;
					continue;
				}
			}
			break;
		}
		for( ; ; )
		{
			if( inAscendant == true )
			{
				if( axis.IsLessThan(GetTextConst(j)) == true )
				{
					j--;
					continue;
				}
			}
			else
			{
				if( GetTextConst(j).IsLessThan(axis) == true )
				{
					j--;
					continue;
				}
			}
			break;
		}
		if( i >= j )   break;
		SwapObject(i,j);
		i++;
		j--;
	}
	if( first < i-1 )   QuickSortUsingOrderArray(first,i-1,inAscendant);
	if( j+1 < last  )   QuickSortUsingOrderArray(j+1,last,inAscendant);
}
*/

#pragma mark ===========================
#pragma mark [�N���X]AHashTextArray
#pragma mark ===========================
//#348

//#693 �n�b�V���p��` -1��kUniqID_Invalid, -2��kUniqID_Unused�Ŏg�p���Ă���̂ŁA-3, -4���g�p����B
#define	kUniqIDValHash_Deleted (-3)
#define	kUniqIDValHash_NoData (-4)

/**
�R���X�g���N�^
*/
AHashTextArray::AHashTextArray( AObjectArrayItem* inParent ) : mHash_DeletedCount(0)//#693 : mNextObjectIDCandidate(kObjectID_Min)
{
	//#693 �������ߖ�̂��ߕK�v�ɂȂ�܂�Hash�������Ȃ� MakeHash();
}

/**
�f�X�g���N�^
*/
AHashTextArray::~AHashTextArray()
{
}

#pragma mark ===========================

#pragma mark ---�ǉ��E�폜

/**
�ǉ�
*/
void	AHashTextArray::Insert1( const AIndex inIndex, const AText& inText )
{
	//���N���X�̃��\�b�h���R�[��
	ATextArray::Insert1(inIndex,inText);
	
	//UniqID�����EinIndex�̗v�f�Ɍ��т� #695
	mUniqIDArray.Insert1Item(inIndex);
	
	//�n�b�V���X�V
	UpdateHash_DataAdded(GetUniqIDByIndex(inIndex),inText);
	
}

//#695
/**
�ǉ�
*/
AIndex	AHashTextArray::Add( const AText& inText )
{
	AIndex	index = GetItemCount();
	Insert1(index,inText);
	return index;
}

/**
�폜
*/
void	AHashTextArray::Delete1( const AIndex inIndex )
{
	//�n�b�V���X�V�i�폜�j #695
	AText	deletingText;
	Get(inIndex,deletingText);
	AUniqID	uniqID = GetUniqIDByIndex(inIndex);
	UpdateHash_DataDeleting(uniqID,deletingText);
	
	//���N���X�̃��\�b�h���R�[��
	ATextArray::Delete1(inIndex);
	
	//UniqID�폜
	mUniqIDArray.Delete1Item(inIndex);
}

//#695
/**
�S�폜
*/
void	AHashTextArray::DeleteAll()
{
	ATextArray::DeleteAll();
	mUniqIDArray.DeleteAll();
	mHash.DeleteAll();
	mHash_DeletedCount = 0;
}

//#65
/**
�w��text�Ɉ�v���鍀�ڂ�S�č폜
*/
void	AHashTextArray::DeleteItemsOfText( const AText& inText )
{
	for( AIndex index = 0; index < GetItemCount(); )
	{
		AText	text;
		Get(index,text);
		if( text.Compare(inText) == true )
		{
			Delete1(index);
		}
		else
		{
			index++;
		}
	}
}

#pragma mark ===========================

#pragma mark ---�ݒ�E�擾

/**
�e�L�X�g�ݒ�
*/
void	AHashTextArray::Set( const AIndex inIndex, const AText& inText )
{
	//�n�b�V���X�V�i�폜�j #695
	AText	deletingText;
	Get(inIndex,deletingText);
	AUniqID	uniqID = GetUniqIDByIndex(inIndex);
	UpdateHash_DataDeleting(uniqID,deletingText);
	
	//���N���X�̃��\�b�h���R�[��
	ATextArray::Set(inIndex,inText);
	
	//�n�b�V���X�V
	RegisterToHash(GetUniqIDByIndex(inIndex),inText,0,inText.GetItemCount());
}

/**
�ړ�
*/
void	AHashTextArray::MoveObject( const AIndex inOldIndex, const AIndex inNewIndex )
{
	AText	text;
	Get(inOldIndex,text);
	Delete1(inOldIndex);
	Insert1(inNewIndex,text);
}

/**
Swap
*/
void	AHashTextArray::SwapObject( const AIndex inA, const AIndex inB )
{
	AText	textA, textB;
	Get(inA,textA);
	Get(inB,textB);
	Set(inA,textB);
	Set(inB,textA);
}

//#853
/**
AHashTextArray����ǉ�
*/
void	AHashTextArray::AddFromTextArray( const ATextArray& inTextArray )
{
	AItemCount	count = inTextArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AText	text;
		inTextArray.Get(i,text);
		Add(text);
	}
}

//#853
/**
AHashTextArray����ݒ�
*/
void	AHashTextArray::SetFromTextArray( const ATextArray& inTextArray )
{
	DeleteAll();
	AddFromTextArray(inTextArray);
}

//#717
/**
AHashTextArray����ǉ�
*/
void	AHashTextArray::AddFromTextArray( const AHashTextArray& inTextArray )
{
	AItemCount	count = inTextArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AText	text;
		inTextArray.Get(i,text);
		Add(text);
	}
}

//#717
/**
ATextArray����ݒ�
*/
void	AHashTextArray::SetFromTextArray( const AHashTextArray& inTextArray )
{
	DeleteAll();
	AddFromTextArray(inTextArray);
}

#pragma mark ===========================

#pragma mark ---UniqID
//#693

/**
Index����UniqID�擾
*/
AUniqID	AHashTextArray::GetUniqIDByIndex( const AIndex inIndex ) const
{
	return mUniqIDArray.Get(inIndex);
}

/**
UniqID����Index�擾
*/
AIndex	AHashTextArray::GetIndexByUniqID( const AUniqID inUniqID ) const
{
	return mUniqIDArray.Find(inUniqID);
}

#pragma mark ===========================

#pragma mark ---����

/**
�����iUniqID�擾�E�ŏ��Ɉ�v�������́j
*/
AUniqID	AHashTextArray::FindUniqID( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	//#693 �v�f��0�̂Ƃ���hash�������Ȃ��ihash�������̏ꍇ������̂�0���Z�΍�j
	if( GetItemCount() == 0 )
	{
		return kUniqID_Invalid;//#695 kIndex_Invalid;
	}
	
	//�n�b�V���T�C�Y�擾
	AItemCount	hashsize = mHash.GetItemCount();
	
	//�n�b�V���v�Z
	unsigned long	hash = GetHashValue(inTargetText,inTargetIndex,inTargetItemCount);
	AIndex	hashstartpos = hash%hashsize;
	
	//�ꔭ�����ł���Ȃ�|�C���^�g�p���Ȃ�
	{
		AUniqID	uniqID = mHash.Get(hashstartpos);
		if( uniqID.val == kUniqIDValHash_NoData )
		{
			return kUniqID_Invalid;//#695 kIndex_Invalid;
		}
		else if( uniqID.val != kUniqIDValHash_Deleted )
		{
			if( Compare(GetIndexByUniqID(uniqID),inTargetText,inTargetIndex,inTargetItemCount) == true )
			{
				return uniqID;
			}
		}
	}
	//�������̂��߁A�|�C���^�g�p
	AStArrayPtr<AUniqID>	arrayptr(mHash,0,hashsize);
	AUniqID*	p = arrayptr.GetPtr();
	//�n�b�V������
	AIndex	hashpos = hashstartpos;
	hashpos = ((hashpos==0)?hashsize-1:hashpos-1);//#271 �ŏ��̈ʒu�͏�L�Ō����ς݂Ȃ̂Ŏ��̈ʒu����n�߂�
	while( true )
	{
		AUniqID	uniqID = p[hashpos];
		if( uniqID.val == kUniqIDValHash_NoData )   break;
		if( uniqID.val != kUniqIDValHash_Deleted )
		{
			if( Compare(GetIndexByUniqID(uniqID),inTargetText,inTargetIndex,inTargetItemCount) == true )
			{
				return uniqID;//#695
			}
		}
		//���̃n�b�V���ʒu��
		hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
		if( hashpos == hashstartpos )   break;
	}
	return kUniqID_Invalid;//#695 kIndex_Invalid;
}

/**
�����iIndex�擾�E��v������̑S�āj
*/
void	AHashTextArray::FindUniqIDs( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount,
		AArray<AUniqID> &outUniqIDArray ) const
{
	outUniqIDArray.DeleteAll();
	
	//#693 �v�f��0�̂Ƃ���hash�������Ȃ��ihash�������̏ꍇ������̂�0���Z�΍�j
	if( GetItemCount() == 0 )
	{
		return;
	}
	
	//�n�b�V���T�C�Y�擾
	AItemCount	hashsize = mHash.GetItemCount();
	
	//�n�b�V���v�Z
	unsigned long	hash = GetHashValue(inTargetText,inTargetIndex,inTargetItemCount);
	AIndex	hashstartpos = hash%hashsize;
	
	//�������̂��߁A�|�C���^�g�p
	AStArrayPtr<AUniqID>	arrayptr(mHash,0,hashsize);
	AUniqID*	p = arrayptr.GetPtr();
	//�n�b�V������
	AIndex	hashpos = hashstartpos;
	while( true )
	{
		AUniqID	uniqID = p[hashpos];
		if( uniqID.val == kUniqIDValHash_NoData )   break;
		if( uniqID.val != kUniqIDValHash_Deleted )
		{
			if( Compare(GetIndexByUniqID(uniqID),inTargetText,inTargetIndex,inTargetItemCount) == true )
			{
				outUniqIDArray.Add(uniqID);
			}
		}
		//���̃n�b�V���ʒu��
		hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
		if( hashpos == hashstartpos )   break;
	}
}

/**
�����iIndex�擾�E�ŏ��Ɉ�v�������́j
*/
AIndex	AHashTextArray::Find( const AText& inTargetText, const AIndex inTargetIndex, 
		const AItemCount inTargetItemCount ) const
{
	AUniqID	uniqID = FindUniqID(inTargetText,inTargetIndex,inTargetItemCount);
	if( uniqID == kUniqID_Invalid )   return kIndex_Invalid;
	return GetIndexByUniqID(uniqID);
}

/**
�����iIndex�擾�E��v������̑S�āj
*/
void	AHashTextArray::FindAll( const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount, 
		AArray<AIndex>& outFoundIndex ) const
{
	//
	outFoundIndex.DeleteAll();
	//
	AArray<AUniqID>	uniqIDArray;
	FindUniqIDs(inTargetText,inTargetIndex,inTargetItemCount,uniqIDArray);
	for( AIndex i = 0; i < uniqIDArray.GetItemCount(); i++ )
	{
		outFoundIndex.Add(GetIndexByUniqID(uniqIDArray.Get(i)));
	}
}

/**
�n�b�V���e�[�u�������������A�Ώ�Array�̑S�f�[�^��o�^����B
*/
void	AHashTextArray::MakeHash()
{
	//�Ώ�Array�̃T�C�Y���傫��������return
	if( GetItemCount() >= kItemCount_HashMaxSize/2 )
	{
		_ACError("cannot make hash because array is too big",this);
		return;
	}
	//�n�b�V���e�[�u���S�폜
	mHash.DeleteAll();
	AItemCount	hashsize = GetItemCount()*3+128;
	//�n�b�V���T�C�Y���ő�n�b�V���T�C�Y�����傫����΁A�␳����B
	if( hashsize > kItemCount_HashMaxSize )   hashsize = kItemCount_HashMaxSize;
	//�n�b�V���e�[�u���m��
	mHash.Reserve(0,hashsize);
	//�n�b�V���e�[�u��������
	{
		AUniqID	nodataUniqID = {kUniqIDValHash_NoData};//#695
		AStArrayPtr<AUniqID>	arrayptr(mHash,0,hashsize);//#695
		AUniqID*	p = arrayptr.GetPtr();//#695
		for( AIndex i = 0; i < hashsize; i++ )
		{
			//#695 mHash.Set(i,kIndex_HashNoData);
			p[i] = nodataUniqID;//#695
		}
	}
	//�Ώ�Array�̌��݂̃f�[�^��S�ēo�^
	AItemCount	datacount = GetItemCount();
	for( AIndex i = 0; i < datacount; i++ )
	{
		AIndex	spos, epos;
		GetRange(i,spos,epos);
		RegisterToHash(GetUniqIDByIndex(i),mTextData,spos,epos-spos);//#695
	}
	//"Deleted"��������   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
	mHash_DeletedCount = 0;
}

/**
�n�b�V���X�V�i�ǉ��j
*/
void	AHashTextArray::UpdateHash_DataAdded( const AUniqID inUniqID, const AText& inText )//#695
{
	//�n�b�V���T�C�Y���Ώۃf�[�^�z��̌��݂̃T�C�Y��2�{������������΁A�n�b�V���e�[�u�����č\������B
	if( mHash.GetItemCount() < (GetItemCount()+mHash_DeletedCount)*2+32 )//#303 "Deleted"�̌���Rehash���f�Ɏg�p���� #693 hash�������̏ꍇ�ɐ�������悤��"+32"��ǉ�
	{
		MakeHash();
	}
	//�����łȂ��ꍇ�͑ΏۃA�C�e�������ǉ��o�^����B
	else
	{
		//�n�b�V���ɓo�^
		RegisterToHash(inUniqID,inText,0,inText.GetItemCount());//#695
	}
}

/**
�n�b�V���X�V�i�폜�j
*/
void	AHashTextArray::UpdateHash_DataDeleting( const AUniqID inUniqID, const AText& inDeletingText )
{
	//�n�b�V���T�C�Y�擾
	AItemCount	hashsize = mHash.GetItemCount();
	
	//�n�b�V���l�v�Z
	unsigned long	hash = GetHashValue(inDeletingText,0,inDeletingText.GetItemCount());
	AIndex	hashstartpos = hash%hashsize;
	
	{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
		//�������̂��߁A�|�C���^�g�p
		AStArrayPtr<AUniqID>	arrayptr(mHash,0,hashsize);
		AUniqID*	p = arrayptr.GetPtr();
		AIndex	hashpos = hashstartpos;
		while( true )
		{
			AUniqID	dataUniqID = p[hashpos];
			if( dataUniqID.val == kUniqIDValHash_NoData )   break;//#216
			if( dataUniqID.val != kUniqIDValHash_Deleted )//#216
			{
				if( dataUniqID.val == inUniqID.val )//#216
				{
					//�폜���s
					if( p[((hashpos==0)?hashsize-1:hashpos-1)].val == kUniqIDValHash_NoData )
					{
						p[hashpos].val = kUniqIDValHash_NoData;//#216
						//win 080713 NoData���������񂾂Ƃ��A�O�̃n�b�V���ʒu�i�����I�ɂ͎��j��Deleted�Ȃ�NoData�ɏ�������
						//[hashpos-1]:NoData
						//[hashpos  ]:NoData�����񏑂�����
						//[hashpos+1]:Deleted������Deleted��NoData�ɂ���ihashpos�̈ʒu�̃f�[�^�ւ̃����N���Ȃ��邽�߂ɑ��݂��Ă���Deleted�Ȃ̂ŁB�j
						//[hashpos+2]:Deleted������Ɏ���Deleted�Ȃ炱����NoData
						AIndex	hp = hashpos+1;
						if( hp >= hashsize )   hp = 0;
						while( p[hp].val == kUniqIDValHash_Deleted )//#216
						{
							//Deleted�Ȃ�NoData�ɏ��������Čp��
							p[hp].val = kUniqIDValHash_NoData;//#216
							hp++;
							if( hp >= hashsize )   hp = 0;
							//"Deleted"��"NoData"�ɂ����̂�"Deleted"�����f�N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
							mHash_DeletedCount--;
						}
					}
					else
					{
						p[hashpos].val = kUniqIDValHash_Deleted;//#216
						//"Deleted"�ɂ����̂�"Deleted"�����C���N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
						mHash_DeletedCount++;
					}
					return;
				}
			}
			//���̃n�b�V���ʒu��
			hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
			if( hashpos == hashstartpos )   break;
		}
	}
}

/**
�n�b�V���o�^

�n�b�V���e�[�u���ɁA�Ώ�Array�̎w��A�C�e����o�^����B
�f�[�^Index�ԍ��̈ړ��Ȃ�
*/
void	AHashTextArray::RegisterToHash( const AUniqID inUniqID, 
		const AText& inText, const AIndex inTextIndex, const AItemCount inTextItemCount )
{
	//�n�b�V���T�C�Y�擾
	AItemCount	hashsize = mHash.GetItemCount();
	
	//�n�b�V���l�v�Z
	unsigned long	hash = GetHashValue(inText,inTextIndex,inTextItemCount);
	AIndex	hashstartpos = hash%hashsize;
	
	{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
		//�������̂��߁A�|�C���^�g�p
		//#695 AStArrayPtr<AUniqID>	arrayptr(mHash,0,hashsize);
		//#695 AUniqID*	p = arrayptr.GetPtr();
		//�������̂��߁A����AArray::mDataPtr���擾����B
		//AStArrayPtr<>�́A�����܂ŃR�[�f�B���O�~�X�h�~�Ȃ̂ŁA�ύX�����p�ɂɂ͔������Ȃ�AbsBase���R�[�h�ł́A
		//�K�������g���K�v�������B
		//AHashArray�ւ�10M���ڒǉ��ŁA9s��2s�̌��ʁB
		//rehash����MakeHash�ɂ��g���Ă���̂ŁA�����̍������͏d�v�B
		AUniqID*	p = mHash.GetDataPtrWithoutCheck();
		//�o�^�ӏ�����
		AIndex	hashpos = hashstartpos;
		while( true )
		{
			AUniqID	dataUniqID = p[hashpos];
			if( dataUniqID.val == kUniqIDValHash_NoData )   break;
			if( dataUniqID.val == kUniqIDValHash_Deleted )
			{
				//"Deleted"�Ƀf�[�^�㏑������̂�"Deleted"�����f�N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
				mHash_DeletedCount--;
				break;
			}
			//���̃n�b�V���ʒu��
			hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
			if( hashpos == hashstartpos )
			{
				_ACError("no area in hash",this);
				return;
			}
		}
		
		//�o�^
		p[hashpos] = inUniqID;
	}
}

/**
�n�b�V���l�v�Z
*/
unsigned long	AHashTextArray::GetHashValue( const AText& inData, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const
{
	AItemCount	targetItemCount = inTargetItemCount;
	if( targetItemCount == kIndex_Invalid )
	{
		targetItemCount = inData.GetItemCount();
	}
	if( targetItemCount == 0 )
	{
		return 1234567;
	}
	else if( targetItemCount == 1 )
	{
		return inData.Get(inTargetIndex)*67890123;
	}
	else if( targetItemCount == 2 )
	{
		return inData.Get(inTargetIndex)*78901234 + inData.Get(inTargetIndex+1)*891012345;
	}
	else 
	{
		return inData.Get(inTargetIndex)*8901234 + inData.Get(inTargetIndex+1)*9012345 
			+ inData.Get(inTargetIndex+targetItemCount-2)*1234567 + inData.Get(inTargetIndex+targetItemCount-1) + targetItemCount*4567;
	}
}


