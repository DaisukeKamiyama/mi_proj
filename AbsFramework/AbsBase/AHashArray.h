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

AHashArray

*/

#pragma once

#include "ABaseTypes.h"
#include "AObjectArrayItem.h"
#include "AArray.h"
#include "AStArrayPtr.h"

#pragma mark ===========================
#pragma mark ---[�N���X]AHashArray<>
/**
�ϒ��z��i�n�b�V�������j�N���X
AArray�I�u�W�F�N�g���R���|�W�V�����Ŏ����A���̃f�[�^���n�b�V�������ł���B
*/
template <class T> class AHashArray : public AObjectArrayItem
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	//�R���X�g���N�^
	AHashArray( AObjectArrayItem* inParent = NULL, const AItemCount inInitialAllocateCount = 64, const AItemCount inReallocateStepCount = 64 ) 
			: AObjectArrayItem(inParent), mData(NULL,inInitialAllocateCount,inReallocateStepCount), mEnableHash(true)
	,mHash_DeletedCount(0)//#303
	,mMutexEnabled(false)//#598
	{
		//#693 �������ߖ�̂��ߗv�f���ǉ������܂ł̓n�b�V���������Ȃ� MakeHash();
	}
	//�f�X�g���N�^
	virtual ~AHashArray()
	{
	}
	
	//Hash�����Enable/Disable��ݒ肷�� #695
	//���ʂ�Insert/Delete���s���Ƃ��́A���L�̎菇�̂ق��������ꍇ������B
	//1. SetEnableHash(false);
	//2. ���ʂ�Insert/Delete���s��
	//3. SetEnableHash(true);
	//4. MakeHash();
  public:
	void	SetEnableHash( const ABool inEnableHash )
	{
		mEnableHash = inEnableHash;
	}
  private:
	ABool	mEnableHash;
	
  public:
	//�P�̃f�[�^��}��
	void	Insert1( const AIndex inIndex, const T& inItem )
	{
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//�f�[�^�z��ɒǉ�
		mData.Insert1(inIndex,inItem);
		
		//�n�b�V���X�V
		if( mEnableHash == true )
		{
			UpdateHash_DataAdded(inIndex,1);
		}
	}
	//Array���畡���̃f�[�^��}��
	void	Insert( const AIndex inIndex, const AArray<T>& inArray )
	{
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//�f�[�^�z��ɒǉ�
		AItemCount	itemcount = inArray.GetItemCount();
		for( AIndex i = 0; i < itemcount; i++ )
		{
			mData.Insert1(inIndex+i,inArray.Get(i));
		}
		//�n�b�V���X�V
		if( mEnableHash == true )
		{
			UpdateHash_DataAdded(inIndex,itemcount);
		}
	}
	//�P�̃f�[�^��ǉ�
	AIndex	Add( const T& inItem )
	{
		AIndex	index = GetItemCount();
		Insert1(index,inItem);
		return index;
	}
	
	//�P�̃f�[�^���폜
	void	Delete1( const AIndex inIndex )
	{
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		//�f�[�^�z�񂩂�폜
		mData.Delete1(inIndex);
		
		//�n�b�V���X�V
		if( mEnableHash == true )
		{
			UpdateHash_DataDeleted(inIndex);
		}
	}
	
	//�S���̃f�[�^���폜
	void DeleteAll()
	{
		/*#695 ������
		while(GetItemCount()>0)
		{
			Delete1(GetItemCount()-1);
		}
		*/
		mData.DeleteAll();
		mHash.DeleteAll();
		mHash_DeletedCount = 0;
	}
	
	//�f�[�^�擾
	T Get( const AIndex inIndex ) const
	{
		if( mMutexEnabled == true )//AStMutexLocker�̃I�u�W�F�N�g�����ɂ����Ԃ�������̂ō������̂��ߐ�ɔ��� #598
		{
			//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
			AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
			
			return mData.Get(inIndex);
		}
		else
		{
			return mData.Get(inIndex);
		}
	}
	
	//�f�[�^�ݒ�
	void Set( const AIndex inIndex, const T& inData )
	{
		if( mMutexEnabled == true )//AStMutexLocker�̃I�u�W�F�N�g�����ɂ����Ԃ�������̂ō������̂��ߐ�ɔ��� #598
		{
			//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
			AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
			
			//�ݒ�O�̃f�[�^�l���擾 #695
			T	oldData = mData.Get(inIndex);
			
			//�f�[�^�z��ɐݒ�
			mData.Set(inIndex,inData);
			
			//�n�b�V���X�V
			if( mEnableHash == true )
			{
				UpdateHash_DataChanged(inIndex,oldData);//#695
			}
		}
		else
		{
			//�ݒ�O�̃f�[�^�l���擾 #695
			T	oldData = mData.Get(inIndex);
			
			//�f�[�^�z��ɐݒ�
			mData.Set(inIndex,inData);
			
			//�n�b�V���X�V
			if( mEnableHash == true )
			{
				UpdateHash_DataChanged(inIndex,oldData);//#695
			}
		}
	}
	
	//Swap
	void	Swap( const AIndex inIndexA, const AIndex inIndexB )
	{
		T	tmp = Get(inIndexA);
		Set(inIndexA,Get(inIndexB));
		Set(inIndexB,tmp);
	}
	
	//Move #693
	void	Move( const AIndex inOldIndex, const AIndex inNewIndex )
	{
		T	tmp = Get(inOldIndex);
		Delete1(inOldIndex);
		Insert1(inNewIndex,tmp);
	}
	
	//#676
	/**
	�z��S�̂𑼂�AHashArray����R�s�[����
	*/
	void	SetFromArray( const AHashArray<T>& inSrcArray )
	{
		//�S�폜
		DeleteAll();
		
		//�P�v�f���R�s�[�ǉ�
		AItemCount	itemCount = inSrcArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			Add(inSrcArray.Get(i));
		}
	}
	
	//����
	AIndex	Find( const T& inTargetData ) const
	{
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		if( mEnableHash == true )
		{
			//#693 �v�f0�̏ꍇ��mHash�������̏ꍇ������̂ŁA���̏�����0���Z�����Ȃ��悤�A�����Ŕ��f
			if( mData.GetItemCount() == 0 )
			{
				return kIndex_Invalid;
			}
			
			//���v���#271
			gStatics_AHashArray_FindCount++;
			gStatics_AHashArray_CompareCount++;
			
			//�n�b�V���T�C�Y�擾
			AItemCount	hashsize = mHash.GetItemCount();
			unsigned long	hash = GetHashValue(inTargetData);
			AIndex	hashstartpos = hash%hashsize;
			
			//�ꔭ�����ł���Ȃ�|�C���^�g�p���Ȃ�
			AIndex	dI = mHash.Get(hashstartpos);
			if( dI == kIndex_HashNoData )
			{
				//���v���#271
				gStatics_AHashArray_FirstDataNull++;
				//
				return kIndex_Invalid;
			}
			else if( dI != kIndex_HashDeleted )
			{
				if( mData.Get(dI) == inTargetData )
				{
					//���v���#271
					gStatics_AHashArray_FirstDataHit++;
					//
					return dI;
				}
			}
			{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
				//�������̂��߁A�|�C���^�g�p
				AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
				AIndex*	p = arrayptr.GetPtr();
				//�n�b�V������
				AIndex	hashpos = hashstartpos;
				hashpos = ((hashpos==0)?hashsize-1:hashpos-1);//#271 �ŏ��̈ʒu�͏�L�Ō����ς݂Ȃ̂Ŏ��̈ʒu����n�߂�
				while( true )
				{
					gStatics_AHashArray_CompareCount++;//���v���#271
					AIndex	dataIndex = p[hashpos];
					if( dataIndex == kIndex_HashNoData )   break;
					if( dataIndex != kIndex_HashDeleted )
					{
						if( mData.Get(dataIndex) == inTargetData )
						{
							return dataIndex;
						}
					}
					//���̃n�b�V���ʒu��
					hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
					if( hashpos == hashstartpos )   break;
				}
			}
			return kIndex_Invalid;
		}
		else
		{
			return mData.Find(inTargetData);
		}
	}
	void	FindAll( const T& inTargetData, AArray<AIndex>& outFoundIndex ) const
	{
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		outFoundIndex.DeleteAll();
		if( mEnableHash == true )
		{
			//#693 �v�f0�̏ꍇ��mHash�������̏ꍇ������̂ŁA���̏�����0���Z�����Ȃ��悤�A�����Ŕ��f
			if( mData.GetItemCount() == 0 )
			{
				return;
			}
			
			//�n�b�V���T�C�Y�擾
			AItemCount	hashsize = mHash.GetItemCount();
			unsigned long	hash = GetHashValue(inTargetData);
			AIndex	hashstartpos = hash%hashsize;
			
			{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
				//�������̂��߁A�|�C���^�g�p
				AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
				AIndex*	p = arrayptr.GetPtr();
				//�n�b�V������
				AIndex	hashpos = hashstartpos;
				while( true )
				{
					AIndex	dataIndex = p[hashpos];
					if( dataIndex == kIndex_HashNoData )   break;
					if( dataIndex != kIndex_HashDeleted )
					{
						if( mData.Get(dataIndex) == inTargetData )
						{
							outFoundIndex.Add(dataIndex);
						}
					}
					//���̃n�b�V���ʒu��
					hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
					if( hashpos == hashstartpos )   break;
				}
			}
			return;
		}
		else
		{
			_ACError("not yet supported",this);
		}
	}
	
	//ItemCount
	AItemCount	GetItemCount() const 
	{
		//Mutex�擾�҂��imMutexEnabled=true�̏ꍇ�Ɍ���j
		AStMutexLocker	locker(mMutex,mMutexEnabled);//#598
		
		return mData.GetItemCount();
	}
	
  public:
	//#695
	/**
	�������g�厞�̍Ċ��蓖�đ����ʂ̐ݒ�
	*/
	void	SetReallocateStep( const AItemCount inReallocateCount )
	{
		mData.SetReallocateStep(inReallocateCount);
	}
	
  private:
	//Hash�l�擾
	//�eT���ɒ�`���邱�ƁB
	unsigned long	GetHashValue( const T inData ) const;
	
  public:
	//�n�b�V���e�[�u�������������A�Ώ�Array�̑S�f�[�^��o�^����B
	void	MakeHash( const AItemCount inMinimumHashSize = kIndex_Invalid )//#693 �g�p������n�b�V���T�C�Y���w�肵��MakeHash�ł���悤�ɂ����B
	{
		//�Ώ�Array�̃T�C�Y���傫��������return
		if( mData.GetItemCount() >= kItemCount_HashMaxSize/2 )
		{
			_ACError("cannot make hash because array is too big",this);
			return;
		}
		//�n�b�V���e�[�u���S�폜
		mHash.DeleteAll();
		AItemCount	hashsize = (mData.GetItemCount()+mData.GetReallocateStep())*3+128;
		//inMinimumHashSize���w�肳��Ă���A���A��L�Őݒ肵���l�����傫���Ƃ��́A�n�b�V���T�C�Y��inMinimumHashSize�ɂ���
		if( inMinimumHashSize != kIndex_Invalid )
		{
			if( hashsize < inMinimumHashSize )
			{
				hashsize = inMinimumHashSize;
			}
		}
		//�n�b�V���T�C�Y���ő�n�b�V���T�C�Y�����傫����΁A�␳����B
		if( hashsize > kItemCount_HashMaxSize )   hashsize = kItemCount_HashMaxSize;
		//�n�b�V���e�[�u���m��
		mHash.Reserve(0,hashsize);
		//�n�b�V���e�[�u��������
		{
			AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
			AIndex*	p = arrayptr.GetPtr();
			for( AIndex i = 0; i < hashsize; i++ )
			{
				//#695 mHash.Set(i,kIndex_HashNoData);
				p[i] = kIndex_HashNoData;
			}
		}
		//�Ώ�Array�̌��݂̃f�[�^��S�ēo�^
		AItemCount	datacount = mData.GetItemCount();
		for( AIndex i = 0; i < datacount; i++ )
		{
			RegisterToHash(i);
		}
		//"Deleted"��������   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
		mHash_DeletedCount = 0;
		//���v���#271
		gStatics_AHashArray_MakeHashCount++;
	}
		
	//�n�b�V���X�V�i�ǉ��j
	void	UpdateHash_DataAdded( const AIndex inDataIndex, const AItemCount inAddedDataCount )
	{
		//���̊֐���mData�ւ̒ǉ���ɃR�[�������i���ł�mData�̗v�f���͌��̐��{inAddedDataCount�ɂȂ��Ă���j�B
		//inDataIndex�͒ǉ��J�n�ʒu�B
		
		//�n�b�V���T�C�Y���Ώۃf�[�^�z��̌��݂̃T�C�Y��2�{������������΁A�n�b�V���e�[�u�����č\������B
		if( mHash.GetItemCount() < (mData.GetItemCount()+mHash_DeletedCount)*2+32 )//#303 "Deleted"�̌���Rehash���f�Ɏg�p����
		{
			MakeHash();
		}
		//�����łȂ��ꍇ�͑ΏۃA�C�e�������ǉ��o�^����B
		else
		{
			//���̔z��̍Ō�ɒǉ������ꍇ�ȊO�́A�ǉ��f�[�^�̃C���f�b�N�X����̃C���f�b�N�X��+1����B
			if( inDataIndex < mData.GetItemCount() - inAddedDataCount )
			{
				AItemCount	hashsize = mHash.GetItemCount();
				AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
				AIndex*	p = arrayptr.GetPtr();
				for( AIndex hashpos = 0; hashpos < hashsize; hashpos++ )
				{
					AIndex	dataIndex = p[hashpos];
					if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
					if( dataIndex >= inDataIndex )
					{
						p[hashpos] = dataIndex + inAddedDataCount;
					}
				}
			}
			/*#695������
			//�n�b�V���T�C�Y�擾
			AItemCount	hashsize = mHash.GetItemCount();
			AItemCount	itemCount = mData.GetItemCount();
			//�������̂��߁A�|�C���^�g�p
			{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
				AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
				AIndex*	p = arrayptr.GetPtr();
				//�ǉ��f�[�^��index����̃f�[�^��S�Č������A�n�b�V���ɓo�^���Ă���index��+inAddedDataCount����B
				for( AIndex index = inDataIndex+inAddedDataCount; index < itemCount; index++ )
				{
					//�n�b�V���l�v�Z
					unsigned long	hash = GetHashValue(mData.Get(index));
					AIndex	hashstartpos = hash%hashsize;
					
					//����
					AIndex	hashpos = hashstartpos;
					while( true )
					{
						AIndex	dataIndex = p[hashpos];
						if( dataIndex == kIndex_HashNoData )   {_ACThrow("",this);break;}
						if( dataIndex+inAddedDataCount == index )
						{
							//�o�^Index��+inAddedDataCount
							p[hashpos] = dataIndex+inAddedDataCount;
							break;
						}
						//���̃n�b�V���ʒu��
						hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
						if( hashpos == hashstartpos )   {_ACThrow("",this);break;}
					}
				}
			}
			*/
			//�n�b�V���ɓo�^
			RegisterToHash(inDataIndex);
		}
	}
	
	//�n�b�V���X�V�i�f�[�^�̒��g���������ɂ��Hash�l�̕ύX�j
	void UpdateHash_DataChanged( const AIndex inDataIndex, const T inOldData )//#695
	{
		//�n�b�V���T�C�Y�擾
		AItemCount	hashsize = mHash.GetItemCount();
		//�n�b�V���l�擾 #695
		unsigned long	hash = GetHashValue(inOldData);
		AIndex	hashstartpos = hash%hashsize;
		//
		ABool	found = false;
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			//�������̂��߁A�|�C���^�g�p
			AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
			AIndex*	p = arrayptr.GetPtr();
			//�n�b�V������
			//#695 for( AIndex hashpos = 0; hashpos < hashsize; hashpos++ )
			AIndex	hashpos = hashstartpos;//#695
			while( true )//#695
			{
				AIndex	dataIndex = p[hashpos];
				//#695 if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
				if( dataIndex == kIndex_HashNoData )   break;//#695
				if( dataIndex == inDataIndex )
				{
					if( p[(hashpos==0)?hashsize-1:hashpos-1] == kIndex_HashNoData )
					{
						p[hashpos] = kIndex_HashNoData;
						//win 080713 NoData���������񂾂Ƃ��A�O�̃n�b�V���ʒu�i�����I�ɂ͎��j��Deleted�Ȃ�NoData�ɏ�������
						//[hashpos-1]:NoData
						//[hashpos  ]:NoData�����񏑂�����
						//[hashpos+1]:Deleted������Deleted��NoData�ɂ���ihashpos�̈ʒu�̃f�[�^�ւ̃����N���Ȃ��邽�߂ɑ��݂��Ă���Deleted�Ȃ̂ŁB�j
						//[hashpos+2]:Deleted������Ɏ���Deleted�Ȃ炱����NoData
						AIndex	hp = hashpos+1;
						if( hp >= hashsize )   hp = 0;
						while( p[hp] == kIndex_HashDeleted )
						{
							//Deleted�Ȃ�NoData�ɏ��������Čp��
							p[hp] = kIndex_HashNoData;
							hp++;
							if( hp >= hashsize )   hp = 0;
							//"Deleted"��"NoData"�ɂ����̂�"Deleted"�����f�N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
							mHash_DeletedCount--;
						}
					}
					else
					{
						p[hashpos] = kIndex_HashDeleted;
						//"Deleted"�ɂ����̂�"Deleted"�����C���N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
						mHash_DeletedCount++;
					}
					////#598 ���s�ʒu�����ֈړ� RegisterToHash(inDataIndex);
					found = true;
					break;//win 080713 ����index�������o�^����Ă��邱�Ƃ͂Ȃ��i���A������break���Ȃ��ƐV�K�o�^�����f�[�^�ɂЂ�������̂ł́H�j
				}
				//#695
				//���̃n�b�V���ʒu��
				hashpos = ((hashpos==0)?hashsize-1:hashpos-1);
				if( hashpos == hashstartpos )   break;
			}
		}
		//#598
		if( found == true )
		{
			RegisterToHash(inDataIndex);
		}
	}
	
	//�n�b�V���X�V�i�폜�j
	void UpdateHash_DataDeleted( const AIndex inDataIndex )
	{
		//�n�b�V���T�C�Y�擾
		AItemCount	hashsize = mHash.GetItemCount();
		//�������̂��߁A�|�C���^�g�p #695
		AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
		AIndex*	p = arrayptr.GetPtr();
		//
		for( AIndex hashpos = 0; hashpos < hashsize; hashpos++ )
		{
			AIndex	dataIndex = p[hashpos];//#695 mHash.Get(hashpos);
			if( dataIndex == kIndex_HashNoData || dataIndex == kIndex_HashDeleted )   continue;
			if( dataIndex == inDataIndex )
			{
				if( /*#695 mHash.Get*/p[((hashpos==0)?hashsize-1:hashpos-1)] == kIndex_HashNoData )
				{
					//#695 mHash.Set(hashpos,kIndex_HashNoData);
					p[hashpos] = kIndex_HashNoData;//#695
					//win 080713 NoData���������񂾂Ƃ��A�O�̃n�b�V���ʒu�i�����I�ɂ͎��j��Deleted�Ȃ�NoData�ɏ�������
					//[hashpos-1]:NoData
					//[hashpos  ]:NoData�����񏑂�����
					//[hashpos+1]:Deleted������Deleted��NoData�ɂ���ihashpos�̈ʒu�̃f�[�^�ւ̃����N���Ȃ��邽�߂ɑ��݂��Ă���Deleted�Ȃ̂ŁB�j
					//[hashpos+2]:Deleted������Ɏ���Deleted�Ȃ炱����NoData
					AIndex	hp = hashpos+1;
					if( hp >= hashsize )   hp = 0;
					while( /*#695 mHash.Get(hp)*/p[hp] == kIndex_HashDeleted )
					{
						//Deleted�Ȃ�NoData�ɏ��������Čp��
						//#695 mHash.Set(hp,kIndex_HashNoData);
						p[hp] = kIndex_HashNoData;//#695
						hp++;
						if( hp >= hashsize )   hp = 0;
						//"Deleted"��"NoData"�ɂ����̂�"Deleted"�����f�N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
						mHash_DeletedCount--;
					}
				}
				else
				{
					//#695 mHash.Set(hashpos,kIndex_HashDeleted);
					p[hashpos] = kIndex_HashDeleted;//#695
					//"Deleted"�ɂ����̂�"Deleted"�����C���N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
					mHash_DeletedCount++;
				}
			}
			//�ǉ��f�[�^�̃C���f�b�N�X����̃C���f�b�N�X��-1����B
			else if( dataIndex > inDataIndex )
			{
				//#695 mHash.Set(hashpos,dataIndex-1);
				p[hashpos] = dataIndex-1;//#695
			}
		}
	}
	
	//�n�b�V���e�[�u���ɁA�Ώ�Array�̎w��A�C�e����o�^����B
	//�f�[�^Index�ԍ��̈ړ��Ȃ�
	void RegisterToHash( const AIndex inDataIndex )
	{
		//�n�b�V���T�C�Y�擾
		AItemCount	hashsize = mHash.GetItemCount();
		
		//�n�b�V���l�v�Z
		unsigned long	hash = GetHashValue(mData.Get(inDataIndex));
		AIndex	hashstartpos = hash%hashsize;
		
		//�o�^�ӏ�����
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			//�������̂��߁A�|�C���^�g�p
			//#695 AStArrayPtr<AIndex>	arrayptr(mHash,0,hashsize);
			//#695 AIndex*	p = arrayptr.GetPtr();
			//�������̂��߁A����AArray::mDataPtr���擾����B
			//AStArrayPtr<>�́A�����܂ŃR�[�f�B���O�~�X�h�~�Ȃ̂ŁA�ύX�����p�ɂɂ͔������Ȃ�AbsBase���R�[�h�ł́A
			//�K�������g���K�v�������B
			//AHashArray�ւ�10M���ڒǉ��ŁA9s��2s�̌��ʁB
			//rehash����MakeHash�ɂ��g���Ă���̂ŁA�����̍������͏d�v�B
			AIndex*	p = mHash.GetDataPtrWithoutCheck();
			//�n�b�V������
			AIndex	hashpos = hashstartpos;
			while( true )
			{
				AIndex	dataIndex = p[hashpos];
				if( dataIndex == kIndex_HashNoData )   break;
				if( dataIndex == kIndex_HashDeleted )
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
			p[hashpos] = inDataIndex;
		}
	}
	
	//�f�[�^
  public:
	AItemCount	GetHashSize() const { return mHash.GetItemCount(); }
  private:
	//�n�b�V��
	//�z��f�[�^�̃C���f�b�N�X���i�[����B
	AArray<AIndex>	mHash;
	AItemCount	mHash_DeletedCount;//#303
	//�z��f�[�^
	AArray<T>	mData;
	
	//OutOfBounds�̏ꍇ��throw����ݒ�
  public:
	void	SetThrowIfOutOfBounds() { mData.SetThrowIfOutOfBounds(); }
	
	//#598
	/**
	�X���b�h�Z�[�tEnable/Disable�ݒ�
	inEnable=true�̏ꍇ�AAHashArray�̓����f�[�^�ɂ��Ă̓X���b�h�Z�[�t�ƂȂ�B
	�i�O���ɓn�����f�[�^�ɂ��Ă̓X���b�h�Z�[�t�ł͂Ȃ��i�����f�[�^�Ƃ̐������ۏ؂���Ȃ��j�̂Œ��ӁB
	���Ƃ���GetItemCount()�ō��ڐ����擾�����ꍇ�A���̒l�̓R�[������mutex�������Ȃ�����A
	�ۏ؂���Ȃ��B�i�ʃX���b�h��Insert/Delete�����\��������B�j
	�f�t�H���g��Disable�B
	*/
  public:
	void	EnableThreadSafe( const ABool inEnable ) { mMutexEnabled = inEnable; }
  private:
	//HashArray�����f�[�^�ւ̃A�N�Z�X�ɑ΂���Mutex
  private:
	ABool	mMutexEnabled;
	mutable AThreadMutex	mMutex;
	
	//Object���擾
  public:
	virtual AConstCharPtr	GetClassName() const { return "AHashArray<>"; }
	
};

