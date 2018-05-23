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

AStArrayPtr

*/

#pragma once

#include "AThreadMutex.h"

/**
AArray����f�[�^�|�C���^���擾���邽�߂̃I�u�W�F�N�g
�i�X�^�b�N������p�I�u�W�F�N�g�j
*/
class AAbstructStArrayPtr
{
  public:
	AAbstructStArrayPtr() {}
	~AAbstructStArrayPtr() {}
	
	//�����|�C���^�̊Ǘ�
	//�I�u�W�F�N�g���̂̕�����ɏ��ł����ꍇ�A�I�u�W�F�N�g���̂̃f�X�g���N�^����DoArrayDeleted()���R�[������邱�Ƃɂ��A
	//mAlivePtr����|�C���^���폜�����B
  public:
	static void	DoArrayDeleted( const AArrayAbstract* inArray )
	{
	}
};

#pragma mark ===========================
#pragma mark [�N���X]AStArrayPtr
/**
AArray����f�[�^�|�C���^���擾���邽�߂̃I�u�W�F�N�g
�i�X�^�b�N������p�I�u�W�F�N�g�j
*/
template<class T> class AStArrayPtr : public AAbstructStArrayPtr
{
  public:
	AStArrayPtr( const AArray<T>& inArray, const AIndex inIndex, const AItemCount inCount ) :mAArrayPtr(&inArray), mIndex(inIndex), mCount(inCount)
	{
		if( inIndex < 0 || inIndex > inArray.GetItemCount() )   _ACThrow("AStArrayPtr inIndex arg error",NULL);
		if( inIndex+inCount < 0 || inIndex+inCount > inArray.GetItemCount() || inCount < 0 )   _ACThrow("AStArrayPtr inIndex arg error",NULL);
		inArray.LockByArrayPtr();
	}
	
	~AStArrayPtr()
	{
		mAArrayPtr->UnlockByArrayPtr();
	}
	
	T* GetPtr() const
	{
		//Lock���ł��邱�Ƃ��`�F�b�N����
		if( mAArrayPtr->IsLockedByArrayPtr() == false )
		{
			_ACError("ptr unlocked",NULL);
			return static_cast<T*>(AMemoryWrapper::Malloc(mCount*sizeof(T)));
		}
		//index, count�̃`�F�b�N
		if( mIndex < 0 || mIndex+mCount > mAArrayPtr->GetItemCount() )
		{
			_ACError("index or count error",NULL);
			return static_cast<T*>(AMemoryWrapper::Malloc(mCount*sizeof(T)));
		}
		return mAArrayPtr->GetDataPtr(mIndex);
	}
	
	AByteCount	GetByteCount() const
	{
		//Lock���ł��邱�Ƃ��`�F�b�N����
		if( mAArrayPtr->IsLockedByArrayPtr() == false )
		{
			_ACError("ptr unlocked",NULL);
			return 0;
		}
		//index, count�̃`�F�b�N
		if( mIndex < 0 || mIndex+mCount > mAArrayPtr->GetItemCount() )
		{
			_ACError("index or count error",NULL);
			return 0;
		}
		return mCount * sizeof(T);
	}
  private:
	const AArray<T>*	mAArrayPtr;
	const AIndex		mIndex;
	const AItemCount	mCount;
	
};

typedef AStArrayPtr<AUChar> AStTextPtr;
typedef AStArrayPtr<AUTF16Char>	AStUniTextPtr;

