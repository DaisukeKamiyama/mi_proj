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

AArray

*/

#pragma once

#include "ABaseTypes.h"
#include "../AbsBase_Imp/ABaseWrapper.h"
#include "AObjectArrayItem.h"
#include "ABaseFunction.h"
#include "AThreadMutex.h"
#include "APurgeManager.h"

const AItemCount kInitialAllocateCountDefault = 16;//B0373
const AItemCount kReallocateStepCount = 32;//B0373

#pragma mark ===========================
#pragma mark [�N���X]AArrayAbstract

/**
AArray<>�̊��N���X
*/
class AArrayAbstract: public AObjectArrayItem
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	/**
	�R���X�g���N�^
	*/
	AArrayAbstract( AObjectArrayItem* inParent );
	/**
	�f�X�g���N�^
	*/
	~AArrayAbstract();
	
	//�v�f��
  public:
	/**
	�v�f���擾
	*/
	AItemCount	GetItemCount() const {return mItemCount;}
  protected:
	AItemCount	mItemCount;
	
	//debug B0373
  public:
	static AByteCount	GetTotalAllocatedByteCount() { return sTotalAllocatedByteCount; }
  protected:
	static AByteCount	sTotalAllocatedByteCount;
	
};

#pragma mark ===========================
#pragma mark [�N���X]AArrayCore<>

template<class T> class AStArrayPtr;
class AStCreateCstringFromAText;

//��{�^�̉ϒ��z��̃R�A����
//mDataPtr�𒼐ڃA�N�Z�X����
//mDataPtr�A�N�Z�X�Ɏg�p���������͑S�ă`�F�b�N�����B
//AStArrayPtr�o�R�Ŏ擾�����|�C���^������Ďg�p�����ꍇ�ȊO�A
//AArray�Ŗ��m�ۃ������G���A���Q�Ƃ��邱�Ƃ��������Ƃ��_���I�ɕۏ؂����B
/**
�ϒ��z��R�A�����̃N���X
*/
template<class T> class AArrayCore : public AArrayAbstract
{
	//mDataPtr��private��`�ł��邪�AOS API��C������Ƃ̂����̂��߂ɂ́A�f�[�^�ւ̃|�C���^���g�p�����������x�ʂŗL���Ȃ̂ŁA
	//��O�I�ɁA���L�N���X�o�R��GetDataPtr()���Ăяo���AmDataPtr���Q�ƁA�|�C���^���擾����B
	friend class AStArrayPtr<T>;
	friend class AStCreateCstringFromAText;
#if IMPLEMENTATION_FOR_WINDOWS
	friend class AStCreateWcharStringFromAText;
#endif
	
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	/**
	�R���X�g���N�^
	*/
	AArrayCore( AObjectArrayItem* inParent, const AItemCount inInitialAllocateCount, const AItemCount inReallocateStepCount ) 
		: AArrayAbstract(inParent), mLockCount(0), mDataPtr(NULL), mReallocateStepCount(inReallocateStepCount),
	mAllocatedSize(/*#693inInitialAllocateCount*sizeof(T)*/0), mThrowIfOutOfBounds(false)//B0000
	,mMutexEnabled(false)//#598
	,mInitialAllocateCount(inInitialAllocateCount)//#693
	,mPurged(false)//#693
	{
	}
  private:
	void	FirstMalloc() const
	{
		//#693
		mAllocatedSize = mInitialAllocateCount*sizeof(T);
		//����������
		mDataPtr = (T*)AMemoryWrapper::Malloc(mAllocatedSize);
		if( mDataPtr == NULL )
		{
			_ACThrow("cannot allocate memory (malloc NG) (critical)",this);
		}
		//debug
		sTotalAllocatedByteCount += mAllocatedSize;
	}
  public:
	/**
	�f�X�g���N�^
	*/
	virtual ~AArrayCore()
	{
		//#693 �t�@�C���ւ�Purge���Ȃ�Purge�t�@�C���폜 �����e�X�g
		if( mPurged == true )
		{
			APurgeManager::GetPurgeManager().DeletePurge(this);
			mPurged = false;
		}
		//
		if( mDataPtr != NULL )
		{
			//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
			AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
			
			if( IsLockedByArrayPtr() == true )
			{
				//ArrayPtr�ɂ�郍�b�N���̓f�[�^�|�C���^��������Ȃ��i���������[�N�ƂȂ邪�A�����I�����܂��j
				//mMutexEnabled=true�̂Ƃ��́A�����ɗ��邱�Ƃ͂Ȃ��B
				//�iArrayPtr�ɂ�郍�b�N����mMutex�͕K�����b�N����Ă��邽�߁AArrayPtr���b�N���͂���Ă��炱���ɗ���B�j
				//������mMutexEnabled=false�̂Ƃ��p�̏����B
				_ACError("AArray deleted while Locked",this);
			}
			else
			{
				AMemoryWrapper::Free(mDataPtr);
				mDataPtr = NULL;
				//debug
				sTotalAllocatedByteCount -= mAllocatedSize;
			}
		}
	}
  public:
	/**
	Allocate���ꂽ�T�C�Y���擾
	*/
	AByteCount	GetAllocatedSize() const { return mAllocatedSize; }
  private:
	//�f�[�^�|�C���^
	mutable T*	mDataPtr;
	//#693
	AItemCount	mInitialAllocateCount;
	//�T�C�Y�ύX�X�e�b�v
	AItemCount	mReallocateStepCount;
	//�����������T�C�Y
	mutable AByteCount	mAllocatedSize;
	
	//B0000
	/**
	�w��Index���͈͊O�̏ꍇThrow����悤�ɐݒ肷��
	*/
  public:
	void	SetThrowIfOutOfBounds() { mThrowIfOutOfBounds = true; }
  private:
	ABool	mThrowIfOutOfBounds;
	
	/**
	�|�C���^�̎擾
	*/
  private:
	T*	GetDataPtr( const AIndex inIndex ) const
	{
		if( inIndex < 0 || inIndex > mItemCount )
		{
			_ACThrow("index error",this);
		}
		//#693 �t�@�C���ւ�Purge���Ȃ�Purge�t�@�C�����畜�� �����e�X�g
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		//#693 �����������蓖�ĂȂ犄�蓖��
		if( mDataPtr == NULL )
		{
			FirstMalloc();
		}
		//
		return &(mDataPtr[inIndex]);
	}
	/**
	�|�C���^�̒��ڎ擾�i���x�d���̂��߂ǂ����Ă��K�v�ȏꍇ�p�B�`�F�b�N��S�����Ȃ��̂Œ��ӕK�v�j
	*/
  public:
	T* GetDataPtrWithoutCheck() const
	{
		return mDataPtr;
	}
	
	//�}���^�폜�R�A
  public:
	/**
	�������̈�}���@�Y��inIndex�ȍ~��inCount���̗̈��}������
	*/
	void	Reserve( const AIndex inIndex, const AItemCount inCount )
	{
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//���b�N���͑}���s��
		if( IsLockedByArrayPtr() == true )
		{
			//mMutexEnabled=true�̂Ƃ��́A�����ɗ��邱�Ƃ͂Ȃ��B
			//�iArrayPtr�ɂ�郍�b�N����mMutex�͕K�����b�N����Ă��邽�߁AArrayPtr���b�N���͂���Ă��炱���ɗ���B�j
			//������mMutexEnabled=false�̂Ƃ��p�̏����B
			_ACThrow("AArray size is to be changed while Locked.",this);
		}
		//�����`�F�b�N
		if( inCount <= 0 )   return;
		if( inIndex < 0 )
		{
			_ACThrow("index out of bounds (<0)",this);
		}
		if( inIndex > mItemCount )
		{
			_ACThrow("index out of bounds (>mItemCount)",this);
		}
		
		//#693 �t�@�C���ւ�Purge���Ȃ�Purge�t�@�C�����畜�� �����e�X�g
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		//#693 �����������蓖�ĂȂ犄�蓖��
		if( mDataPtr == NULL )
		{
			FirstMalloc();
		}
		
		//�������̈�g��
		//�V�����K�v�ɂȂ�T�C�Y���A���݂�storage�T�C�Y-3���ȏ�ł���΁A�������̈���g�傷��B
		//"-3"����̂́A������Astorage�̂�����ƌ��̃f�[�^���A�N�Z�X�����Ƃ��ł������I�������Ȃ����߂̈��S��
		if( (mItemCount+inCount)*sizeof(T) >= mAllocatedSize - 3*sizeof(T) )
		{
			AByteCount	newSize = (mItemCount+inCount)*sizeof(T);
			newSize += mReallocateStepCount*sizeof(T);//mReallocateStepCount���A�]���Ƀ������m�ۂ���B
			T*	newptr = (T*)AMemoryWrapper::Realloc(mDataPtr,newSize);//��200MB�y�[�X�g���G���[
			if( newptr == NULL )
			{
				_ACThrow("cannot allocate memory (realloc NG) (critical)",this);
			}
			if( newptr != mDataPtr )
			{
				//�|�C���^�ւ̒l����́A�|�C���^�l���ς�����Ƃ������ɂ��Ă���B
				//�X���b�h�Z�[�t�ݒ�ɂ��Ă��Ȃ��ꍇ�ɁA����X���b�h�ԂŃA�N�Z�X���Ă��܂����Ƃ��ɁA
				//�|�C���^�G���[�ɂȂ郊�X�N��Ⴍ���邽�߁B
				mDataPtr = newptr;
			}
			//debug
			sTotalAllocatedByteCount += newSize-mAllocatedSize;
			//realloc OK
			mAllocatedSize = newSize;
		}
		//�̈�m�ۃ`�F�b�N�͂��Ȃ�
		//�irealloc���G���[�Ȃ��ŕԂ��Ă��Ă���̂ŁA�̈�m�ۂ͐M���ł���B�`�F�b�N�̓I�[�o�[�w�b�h�ɂȂ�B�j
		//���e�̈ړ�
		if( inIndex != mItemCount )
		{
			AMemoryWrapper::Memmove(&(mDataPtr[inIndex+inCount]),&(mDataPtr[inIndex]),(mItemCount-inIndex)*sizeof(T));
		}
		//mItemCount�X�V
		mItemCount += inCount;
	}
	/**
	�폜
	*/
	void	Delete( const AIndex inIndex, const AItemCount inCount )
	{
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//���b�N���͍폜�s��
		if( IsLockedByArrayPtr() == true )
		{
			//mMutexEnabled=true�̂Ƃ��́A�����ɗ��邱�Ƃ͂Ȃ��B
			//�iArrayPtr�ɂ�郍�b�N����mMutex�͕K�����b�N����Ă��邽�߁AArrayPtr���b�N���͂���Ă��炱���ɗ���B�j
			//������mMutexEnabled=false�̂Ƃ��p�̏����B
			_ACThrow("AArray size is to be changed while Locked.",this);
		}
		
		//�����`�F�b�N
		if( inCount <= 0 )   return;
		if( inCount > mItemCount-inIndex )
		{
			_ACThrow("count out of bounds (>mItemCount-index)",this);
		}
		if( inIndex < 0 )
		{
			_ACThrow("index out of bounds (<0)",this);
		}
		if( inIndex > mItemCount )
		{
			_ACThrow("index out of bounds (>mItemCount)",this);
		}
		
		//#693 �t�@�C���ւ�Purge���Ȃ�Purge�t�@�C�����畜�� �����e�X�g
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		
		//�S�폜���ǂ����̔��� #1275
		if( inIndex == 0 && inCount == mItemCount )
		{
			//�S�폜�̏ꍇ #1275
			
			//mItemCount�X�V
			mItemCount = 0;
			
			//���������
			if( mDataPtr != NULL )
			{
				//���������
				AMemoryWrapper::Free(mDataPtr);
				mDataPtr = NULL;
				//debug
				sTotalAllocatedByteCount -= mAllocatedSize;
				//
				mAllocatedSize = 0;
			}
		}
		else
		{
			//�S�폜�ȊO�̏ꍇ
			
			//���e�̈ړ�
			if( inIndex != mItemCount-inCount )
			{
				AMemoryWrapper::Memmove(&(mDataPtr[inIndex]),&(mDataPtr[inIndex+inCount]),(mItemCount-inIndex-inCount)*sizeof(T));
			}
			//�V�����K�v�ɂȂ�T�C�Y���A���݂�storage�T�C�Y-(mReallocateStepCount+10)�������ł���΁A�������̈���k������B
			if( mAllocatedSize > mReallocateStepCount*sizeof(T) && //�ŏ��T�C�Y�imReallocateStepCount�j��菬�������Ȃ����߂̏���
			(mItemCount-inCount)*sizeof(T) < mAllocatedSize - (mReallocateStepCount+10)*sizeof(T) ) 
			{
				AByteCount	newSize = (mItemCount-inCount)*sizeof(T);
				newSize += mReallocateStepCount*sizeof(T);
				T*	newptr = (T*)AMemoryWrapper::Realloc(mDataPtr,newSize);
				if( newptr == NULL )
				{
					//realloc NG�̏ꍇ�̏���
					//Reserve()�̏ꍇ�ƈႢ�A������͏k���Ȃ̂ŁA�ӂ��G���[�ɂȂ�͂��������A�܂��A�������v�[��������Ă����������Ȃ��B
					_ACThrow("cannot allocate memory (realloc NG) (critical)",this);
				}
				//debug
				sTotalAllocatedByteCount += newSize-mAllocatedSize;
				//realloc OK
				mAllocatedSize = newSize;
				if( newptr != mDataPtr )
				{
					mDataPtr = newptr;
				}
			}
			//mItemCount�X�V
			mItemCount -= inCount;
		}
		/* #1275 �폜��̍��ڐ���0�̏ꍇ�A���Ń�����������Ă������A��ōs�Ȃ��Ă����������̈�̏k�����̏��������ʂɂȂ�̂ŁA�폜��̍��ڐ���0�̏ꍇ�̓����������������悤�ɂ���i��������Ɉړ��j�B
		//#693
		//�폜��̍��ڐ���0�Ȃ烁�����������i�ēxReserve()���R�[�����ꂽ�炻���ōēxFirstMalloc()���R�[�������j
		if( mItemCount == 0 && mDataPtr != NULL )
		{
			//���������
			AMemoryWrapper::Free(mDataPtr);
			mDataPtr = NULL;
			//debug
			sTotalAllocatedByteCount -= mAllocatedSize;
			//
			mAllocatedSize = 0;
		}
		*/
	}
	//#695
	/**
	�������g�厞�̍Ċ��蓖�đ����ʂ̐ݒ�
	*/
  public:
	void	SetReallocateStep( const AItemCount inReallocateCount )
	{
		mReallocateStepCount = inReallocateCount;
	}
	/**
	���݂̊��蓖�đ����ʐݒ�̎擾
	*/
	AItemCount	GetReallocateStep() const
	{
		return mReallocateStepCount;
	}
	
  public:
	//#695
	/**
	����AArray�z�񂩂�R�s�[���āA�w��inIndex�֑}��
	*/
	void	Insert( const AIndex inIndex, const AArrayCore<T>& inArray )
	{
		//�R�s�[���z��̌����擾���āA�{�z��̗̈�m��
		AItemCount	itemCount = inArray.GetItemCount();
		Reserve(inIndex,itemCount);
		//�f�[�^�R�s�[
		for( AIndex i = 0; i < itemCount; i++ )
		{
			mDataPtr[inIndex+i] = inArray.mDataPtr[i];
		}
	}
	
	//�f�[�^�擾�^�ݒ�R�A
  public:
	/**
	�擾
	*/
	T Get( const AIndex inIndex ) const
	{
		//�����`�F�b�N
		if( inIndex < 0 )
		{
			//B0000 Get/Set�Ɋւ��ẮA�f�t�H���g�ł�throw���Ȃ��ق������S
			//Get/Set��̏����͎��s�����Ă��������[�v����������m���͒Ⴂ�B�i�z����͐���n�ł��ς��Ȃ��̂Łj
			//Get/Set��throw�����Ă��܂��ƁA�Q�̂ق����傫���i�v���I�ł͂Ȃ��̂ɂ����ŏ������X�g�b�v���Ă��܂����Ƃ�����j
			//�����A�|�C���^�̔z��Ɋւ��ẮAthrow�����ق������S�Ȃ̂�mThrowIfOutOfBounds�ɂ���Đ؂�ւ���
			if( mThrowIfOutOfBounds == false )
			{
				_ACError("index out of bounds (<0)",this);
			}
			else
			{
				_ACThrow("index out of bounds (<0)",this);
			}
			T	tmp;
			AMemoryWrapper::Fill0(&tmp,sizeof(T));
			return tmp;
		}
		if( inIndex >= mItemCount )
		{
			//B0000 Get/Set�Ɋւ��ẮA�f�t�H���g�ł�throw���Ȃ��ق������S
			//Get/Set��̏����͎��s�����Ă��������[�v����������m���͒Ⴂ�B�i�z����͐���n�ł��ς��Ȃ��̂Łj
			//Get/Set��throw�����Ă��܂��ƁA�Q�̂ق����傫��
			//�����A�|�C���^�̔z��Ɋւ��ẮAthrow�����ق������S�Ȃ̂�mThrowIfOutOfBounds�ɂ���Đ؂�ւ���
			if( mThrowIfOutOfBounds == false )
			{
				_ACError("index out of bounds (>=mItemCount)",this);
			}
			else
			{
				_ACThrow("index out of bounds (>=mItemCount)",this);
			}
			T	tmp;
			AMemoryWrapper::Fill0(&tmp,sizeof(T));
			return tmp;
		}
		
		//#693 �t�@�C���ւ�Purge���Ȃ�Purge�t�@�C�����畜�� �����e�X�g
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		
		if( mMutexEnabled == true )//AStMutexLocker�̃I�u�W�F�N�g�����ɂ����Ԃ�������̂ō������̂��ߐ�ɔ��� #598
		{
			//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
			AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
			//�擾
			return mDataPtr[inIndex];
		}
		else
		{
			//�擾
			return mDataPtr[inIndex];
		}
	}
	/**
	�ݒ�
	*/
	void Set( const AIndex inIndex, const T inData )
	{
		//�����`�F�b�N
		if( inIndex < 0 )
		{
			//B0000 Get/Set�Ɋւ��ẮA�f�t�H���g�ł�throw���Ȃ��ق������S
			//Get/Set��̏����͎��s�����Ă��������[�v����������m���͒Ⴂ�B�i�z����͐���n�ł��ς��Ȃ��̂Łj
			//Get/Set��throw�����Ă��܂��ƁA�Q�̂ق����傫��
			//�����A�|�C���^�̔z��Ɋւ��ẮAthrow�����ق������S�Ȃ̂�mThrowIfOutOfBounds�ɂ���Đ؂�ւ���
			if( mThrowIfOutOfBounds == false )
			{
				_ACError("index out of bounds (<0)",this);
			}
			else
			{
				_ACThrow("index out of bounds (<0)",this);
			}
			return;
		}
		if( inIndex >= mItemCount )
		{
			//B0000 Get/Set�Ɋւ��ẮA�f�t�H���g�ł�throw���Ȃ��ق������S
			//Get/Set��̏����͎��s�����Ă��������[�v����������m���͒Ⴂ�B�i�z����͐���n�ł��ς��Ȃ��̂Łj
			//Get/Set��throw�����Ă��܂��ƁA�Q�̂ق����傫��
			//�����A�|�C���^�̔z��Ɋւ��ẮAthrow�����ق������S�Ȃ̂�mThrowIfOutOfBounds�ɂ���Đ؂�ւ���
			if( mThrowIfOutOfBounds == false )
			{
				_ACError("index out of bounds (>=mItemCount)",this);
			}
			else
			{
				_ACThrow("index out of bounds (>=mItemCount)",this);
			}
			return;
		}
		
		//#693 �t�@�C���ւ�Purge���Ȃ�Purge�t�@�C�����畜�� �����e�X�g
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		
		if( mMutexEnabled == true )//AStMutexLocker�̃I�u�W�F�N�g�����ɂ����Ԃ�������̂ō������̂��ߐ�ɔ��� #598
		{
			//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
			AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
			//�ݒ�
			mDataPtr[inIndex] = inData;
		}
		else
		{
			//�ݒ�
			mDataPtr[inIndex] = inData;
		}
	}
	
	//�f�[�^����
  public:
	/**
	inTarget�Ɉ�v����f�[�^��index��Ԃ��i�ŏ��Ɍ����������ڂ�index��Ԃ��j
	*/
	AIndex	Find( const T inTarget ) const
	{
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		//����
		for( AIndex i = 0; i < mItemCount; i++ )
		{
			if( mDataPtr[i] == inTarget )   return i;
		}
		return kIndex_Invalid;
	}
	
  public:
	/**
	���e����ւ�
	*/
	void	SwapContent( AArrayCore<T>& ioArrayObject )
	{
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//#693 �t�@�C���ւ�Purge���Ȃ�Purge�t�@�C�����畜�� �����e�X�g
		if( mPurged == true )
		{
			mDataPtr = (T*)(APurgeManager::GetPurgeManager().RestoreFromPurge(this));
			mPurged = false;
		}
		//#693 �����������蓖�ĂȂ犄�蓖��
		if( mDataPtr == NULL )
		{
			FirstMalloc();
		}
		
		//
		AItemCount	itemCount 			= mItemCount;
		T*	dataPtr						= mDataPtr;
		AItemCount	reallocateStepCount	= mReallocateStepCount;
		AByteCount	allocatedSize		= mAllocatedSize;
		
		mItemCount 						= ioArrayObject.mItemCount;
		mDataPtr						= ioArrayObject.mDataPtr;
		mReallocateStepCount			= ioArrayObject.mReallocateStepCount;
		mAllocatedSize					= ioArrayObject.mAllocatedSize;
		
		ioArrayObject.mItemCount		= itemCount;
		ioArrayObject.mDataPtr			= dataPtr;
		ioArrayObject.mReallocateStepCount = reallocateStepCount;
		ioArrayObject.mAllocatedSize	= allocatedSize;
	}
	
	//AStArrayPtr�ɂ��ptr�擾����Insert/Delete�h�~
	//�i�R�[�f�B���O�~�X�ɂ��Insert/Delete��ptr�擾���ɂ��Ă��܂��̂�h�~����B
	//�ʃX���b�h�����Insert/Delete�h�~�́AmMutexEnabled=true�ɂ��āA�S�̂�r�����䂷��K�v������B
	//�܂�A���L�����ł͕ʃX���b�h�Ƃ̔r������͂ł��Ȃ��B�i�s��������������̂́Aptr�擾�������ł͂Ȃ����߁j�j
  private:
	/**
	Lock�����ǂ������擾
	*/
	ABool	IsLockedByArrayPtr() const { return (mLockCount>0);}
	/**
	Lock����
	*/
	void	LockByArrayPtr() const 
	{
		//���ڐ�0�̂Ƃ���mDataPtr��NULL�̏ꍇ�����邪�A
		//���ڐ�0�Ȃ̂ŁA���ۂ̃A�N�Z�X���������邱�Ƃ��Ȃ��B
		//�ip[]�̊��ʓ��ɂ��Ă͂�����ɂ��Ă��g�p���鑤�œY���I�[�o�[���Ȃ��悤���ӂ��K�v�B�j
		AStMutexLocker	locker(mLockCountMutex);
		//ptr�擾���́Aptr�ȊO����̃A�N�Z�X�͋֎~ #598
		if( mLockCount == 0 && mMutexEnabled == true )
		{
			mMutex.Lock();
		}
		mLockCount++;
	}
	/**
	Unlock����
	*/
	void	UnlockByArrayPtr() const 
	{
		AStMutexLocker	locker(mLockCountMutex);
		mLockCount--;
		//Mutex����
		if( mLockCount == 0 && mMutexEnabled == true )
		{
			mMutex.Unlock();
		}
		//�i�O�̂��߁jLockCount�}�C�i�X�Ȃ�0�ɂ���
		if( mLockCount < 0 )
		{
			_ACError("",this);
			mLockCount = 0;
		}
	}
	mutable AItemCount	mLockCount;
	mutable AThreadMutex	mLockCountMutex;
	
	//#693
	//�t�@�C���ւ�Purge �����e�X�g
  public:
	void	Purge()
	{
		//
		if( mDataPtr == NULL )   return;
		
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		if( IsLockedByArrayPtr() == true )
		{
			//ArrayPtr�ɂ�郍�b�N����Purge���Ȃ�
			_ACError("AArray purge while Locked",this);
		}
		else
		{
			//
			APurgeManager::GetPurgeManager().Purge(this,mDataPtr,mAllocatedSize);
			mPurged = true;
			mDataPtr = NULL;
			//mAllocatedSize�͂��̂܂܂ɂ���
			//debug
			sTotalAllocatedByteCount -= mAllocatedSize;
		}
	}
  private:
	mutable ABool mPurged;
	
	/**
	�X���b�h�Z�[�tEnable/Disable�ݒ�
	inEnable=true�̏ꍇ�AAArray�̓����f�[�^�ɂ��Ă̓X���b�h�Z�[�t�ƂȂ�B
	�i�O���ɓn�����f�[�^�ɂ��Ă̓X���b�h�Z�[�t�ł͂Ȃ��i�����f�[�^�Ƃ̐������ۏ؂���Ȃ��j�̂Œ��ӁB
	���Ƃ���GetItemCount()�ō��ڐ����擾�����ꍇ�A���̒l�̓R�[������mutex�������Ȃ�����A
	�ۏ؂���Ȃ��B�i�ʃX���b�h��Insert/Delete�����\��������B�j
	�f�t�H���g��Disable�B�iGet/Set��mutex��������̂Œx���Ȃ�B
	mi�̎����e�X�g���s�������Ԕ�r�F�S��disable:3�����S��enable:5���j
	*/
  public:
	void	EnableThreadSafe( const ABool inEnable ) { mMutexEnabled = inEnable; }
  private:
	//Array�ւ̃A�N�Z�X�ɑ΂���Mutex
  private:
	ABool	mMutexEnabled;
	mutable AThreadMutex	mMutex;
};

//�ϒ��z��N���X
//��{�^�̉ϒ��z��
//mDataPtr�͒��ڃA�N�Z�X���Ȃ��i�ł��Ȃ��j
//AArrayCore��Reserve(), Delete(), Get(), Set()���R�[�����邱�Ƃő��삷��
/**
�ϒ��z��N���X
*/
template<class T> class AArray : public AArrayCore<T>
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	/**
	�R���X�g���N�^
	*/
	AArray( AObjectArrayItem* inParent = NULL, const AItemCount inInitialAllocateCount = kInitialAllocateCountDefault, const AItemCount inReallocateStepCount = kReallocateStepCount ) //B0373
		: AArrayCore<T>(inParent,inInitialAllocateCount,inReallocateStepCount)
	{
	}
	virtual ~AArray()
	{
	}
	
	//�}���^�ǉ�
  public:
	/**
	�P�̃f�[�^��}��
	*/
	void	Insert1( const AIndex inIndex, const T& inItem )
	{
		AArrayCore<T>::Reserve(inIndex,1);
		AArrayCore<T>::Set(inIndex,inItem);
	}
	/**
	�P�̃f�[�^���Ō�ɒǉ�
	*/
	AIndex	Add( const T& inItem )
	{
		AIndex	index = AArrayCore<T>::GetItemCount();
		Insert1(index,inItem);
		return index;
	}
	
	//�폜
  public:
	/**
	�P�̃f�[�^���폜
	*/
	void	Delete1( const AIndex inIndex )
	{
		AArrayCore<T>::Delete(inIndex,1);
	}
	/**
	�S���̃f�[�^���폜
	*/
	void DeleteAll()
	{
		AArrayCore<T>::Delete(0,AArrayCore<T>::GetItemCount());
	}
	/**
	�w��index�ȍ~���폜
	*/
	void DeleteAfter( const AIndex inIndex )
	{
		AArrayCore<T>::Delete(inIndex,AArrayCore<T>::GetItemCount()-inIndex);
	}
	/**
	��v���鍀�ڂ��폜
	*/
	void	FindAndDelete( const T& inItem )
	{
		AIndex	index = AArrayCore<T>::Find(inItem);//#1012
		if( index != kIndex_Invalid )
		{
			Delete1(index);
		}
	}
	
  public:
	/**
	�I�u�W�F�N�g�̑S�f�[�^��ǉ�
	@note AddFromObject()�P�̂ł͖��e�X�g
	*/
	void	AddFromObject( const AArray<T>& inSrc )//#717
	{
		/*#853 ������
		for( AIndex index = 0; index < inSrc.GetItemCount(); index++ )
		{
			Add(inSrc.Get(index));
		}
		*/
		//�ǉ��O��dst���i*this)�̍��ڐ����L��
		AItemCount	origDstItemCount = AArrayCore<T>::GetItemCount();
		//dst���̃������m��
		AArrayCore<T>::Reserve(origDstItemCount,inSrc.GetItemCount());
		//dst���|�C���^
		AStArrayPtr<T>	dstarrayptr(*this,0,AArrayCore<T>::GetItemCount());
		T*	dstp = dstarrayptr.GetPtr();
		//src���|�C���^
		AStArrayPtr<T>	srcarrayptr(inSrc,0,inSrc.GetItemCount());
		T*	srcp = srcarrayptr.GetPtr();
		//�R�s�[
		AItemCount	srcItemCount = inSrc.GetItemCount();
		for( AIndex index = 0; index < srcItemCount; index++ )
		{
			dstp[origDstItemCount + index] = srcp[index];
		}
	}
	/**
	�I�u�W�F�N�g�̑S�f�[�^��ݒ�
	*/
	void	SetFromObject( const AArray<T>& inSrc )
	{
		DeleteAll();
		AddFromObject(inSrc);
	}
	
	//Swap
  public:
	/**
	�Y��inIndexA��inIndexB�̃f�[�^���e�����ւ���
	*/
	void	Swap( const AIndex inIndexA, const AIndex inIndexB )
	{
		T	tmp = AArrayCore<T>::Get(inIndexA);
		AArrayCore<T>::Set(inIndexA,AArrayCore<T>::Get(inIndexB));
		AArrayCore<T>::Set(inIndexB,tmp);
	}
	
	//Move
  public:
	/**
	�ړ�
	*/
	void	Move( const AIndex inOldIndex, const AIndex inNewIndex )
	{
		T	tmp = AArrayCore<T>::Get(inOldIndex);
		Delete1(inOldIndex);
		Insert1(inNewIndex,tmp);
	}
	
	/*#695 �������̂���AArrayCore::Find()�ɂāA���ڃ|�C���^���g�p���Č�������悤�ɕύX
	//�f�[�^����
  public:
	*
	inTarget�Ɉ�v����f�[�^��index��Ԃ�
	*
	AIndex	Find( const T inTarget ) const
	{
		for( AIndex i = 0; i < AArrayCore<T>::GetItemCount(); i++ )
		{
			if( AArrayCore<T>::Get(i) == inTarget )   return i;
		}
		return kIndex_Invalid;
	}
	*/
	
	//Sort
  public:
	/**
	Sort
	@param inAscendant true�Ȃ珸��
	*/
	void	Sort( const ABool inAscendant )
	{
		if( AArrayCore<T>::GetItemCount() <= 1 )   return;
		QuickSort(0,AArrayCore<T>::GetItemCount()-1,inAscendant);
	}
  private:
	/**
	Sort (QuickSort)
	@param inFirst  Start index
	@param inLast   End index (The item at inLast is included in sort items.)
	*/
	void	QuickSort( AIndex inFirst, AIndex inLast, const ABool inAscendant )
	{
		//Get pivot value
		T	pivot = AArrayCore<T>::Get((inFirst+inLast)/2);
		//Divide 
		AIndex	i = inFirst, j = inLast;
		for( ; ; )
		{
			for( ; ; )
			{
				T	data = AArrayCore<T>::Get(i);
				if( inAscendant == true )
				{
					if( data < pivot )
					{
						i++;
						continue;
					}
				}
				else
				{
					if( data > pivot )
					{
						i++;
						continue;
					}
				}
				break;
			}
			for( ; ; )
			{
				T	data = AArrayCore<T>::Get(j);
				if( inAscendant == true )
				{
					if( pivot < data )
					{
						j--;
						continue;
					}
				}
				else
				{
					if( pivot > data )
					{
						j--;
						continue;
					}
				}
				break;
			}
			if( i >= j )   break;
			Swap(i,j);
			i++;
			j--;
		}
		if( inFirst < i-1 )   QuickSort(inFirst,i-1,inAscendant);
		if( j+1 < inLast  )   QuickSort(j+1,inLast,inAscendant);
	}
	
};

//�e���{�f�[�^�^�z��̒�`
typedef AArray<ABool> ABoolArray;
typedef AArray<ANumber> ANumberArray;
typedef AArray<AColor> AColorArray;
typedef AArray<AObjectID>	AObjectIDArray;




