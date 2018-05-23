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

AHashObjectArray
(C) 2008 Daisuke Kamiyama, All Rights Reserved.

*/

#pragma once

#include "ABaseTypes.h"
#include "AObjectArray.h"
#include "AObjectArrayIDIndexed.h"
#include "AObjectArrayItem.h"

//#693 �n�b�V���p��`
#ifdef __LP64__
#define	kObjectArrayItemHash_Deleted ((AObjectArrayItem*)0xFFFFFFFFFFFFFFFF)
#else
#define	kObjectArrayItemHash_Deleted ((AObjectArrayItem*)0xFFFFFFFF)
#endif
#define	kObjectArrayItemHash_NoData ((AObjectArrayItem*)NULL)

#pragma mark ===========================
#pragma mark [�N���X]AHashObjectArray
/**
�I�u�W�F�N�g�̉ϒ��z��i�n�b�V�������j�N���X
AObjectArray�I�u�W�F�N�g���R���|�W�V�����Ŏ����A���̃f�[�^���n�b�V�������ł���B

T�͔z��ɂ���I�u�W�F�N�g�AU�͌������ɃL�[�Ƃ��Ďg�p����I�u�W�F�N�g�������͊�{�^�ŁA���̃I�u�W�F�N�g���́A�������́A�I�u�W�F�N�g�����L���郁���o�[�̂����ꂩ
*/
template <class T, class U> class AHashObjectArray : public AObjectArrayItem
{
  public:
	//�R���X�g���N�^
	AHashObjectArray( AObjectArrayItem* inParent = NULL, const ANumber inHashSizeMultiple = 3 ) : AObjectArrayItem(inParent),
			mHashSizeMultiple(inHashSizeMultiple)//B0000 ������
	,mHash_DeletedCount(0)//#303
	,mMutexEnabled(true)//#598
	{
		//#598 ������̃N���X�Ŕr�����䂷��̂ŉ��ʂ�mObjectArray�ł͐���s�v
		mObjectArray.EnableThreadSafe(false);
		
		//
		MakeHash();
	}
	//�f�X�g���N�^
	virtual ~AHashObjectArray()
	{
	}
	
	//�V�K�I�u�W�F�N�g�V�K�}��
	void	InsertNew1Object( const AIndex inIndex, AAbstractFactoryForObjectArray<T>& inFactory )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		//�I�u�W�F�N�g�ǉ�
		mObjectArray.InsertNew1Object(inIndex,inFactory);
		
		//�n�b�V���X�V
		UpdateHash_DataAdded(inIndex,mObjectArray.GetObjectConst(inIndex).GetObjectID());
	}
	
	//�V�K�I�u�W�F�N�g�V�K�}���i�f�t�H���gAbstractFactory�g�p�j
	void	InsertNew1Object( const AIndex inIndex )
	{
		ADefaultAbstractFactoryForObjectArray<T>	defaultFactory(this);
		InsertNew1Object(inIndex,defaultFactory);
	}
	
	//�V�K�I�u�W�F�N�g�V�K�ǉ�
	AIndex	AddNewObject( AAbstractFactoryForObjectArray<T>& inFactory )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		//�I�u�W�F�N�g�ǉ�
		AIndex	index = mObjectArray.AddNewObject(inFactory);
		
		//�n�b�V���X�V
		UpdateHash_DataAdded(index,mObjectArray.GetObjectConst(index).GetObjectID());
		
		return index;
	}
	
	//�V�K�I�u�W�F�N�g�V�K�ǉ��i�f�t�H���gAbstractFactory�g�p�j
	AIndex AddNewObject()
	{
		ADefaultAbstractFactoryForObjectArray<T>	defaultFactory;
		return AddNewObject(defaultFactory);
	}
	
	//�I�u�W�F�N�g�擾
  public:
	T& GetObject( const AIndex inIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetObject(inIndex);
	}
	//�I�u�W�F�N�g�擾(const)
	const T& GetObjectConst( const AIndex inIndex ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetObjectConst(inIndex);
	}
  public:
	//ObjectID����I�u�W�F�N�g�擾
	T& GetObjectByID( const AObjectID inObjectID )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetObjectByID(inObjectID);
	}
	//ObjectID����I�u�W�F�N�g�擾(const)
	T& GetObjectConstByID( const AObjectID inObjectID ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetObjectConstByID(inObjectID);
	}
	
	//�I�u�W�F�N�g�X�V�ɑ΂��ăn�b�V���X�V
	void	ObjectUpdateStart( const AIndex inIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		AObjectID	objectID = mObjectArray.GetObjectConst(inIndex).GetObjectID();
		UpdateHash_DataDeleted(inIndex,objectID);
	}
	void	ObjectUpdateEnd( const AIndex inIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		AObjectID	objectID = mObjectArray.GetObjectConst(inIndex).GetObjectID();
		UpdateHash_DataAdded(inIndex,objectID);
	}
	
	//�폜
	void	Delete1Object( const AIndex inIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		//�폜���悤�Ƃ���I�u�W�F�N�g��ObjectID�擾
		AObjectID	objectID = mObjectArray.GetObjectConst(inIndex).GetObjectID();
	
		//�n�b�V���X�V
		UpdateHash_DataDeleted(inIndex,objectID);
		
		//�I�u�W�F�N�g�폜
		mObjectArray.Delete1Object(inIndex);
	}
	
	//�S�폜
	void	DeleteAll( const ABool inRetainSameHashSize = false )//win 080714 �������i���݂̃n�b�V���T�C�Y���ێ�����t���O�ǉ��j
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		/* B0000 ������ �P��������Hash�X�V�̎��Ԃ�������̂ŁA��C�ɏ����āAMakeHash()�����ق�������
		while( GetItemCount() > 0 )
		{
			Delete1Object(GetItemCount()-1);
		}*/
		mObjectArray.DeleteAll();
		MakeHash(inRetainSameHashSize);//win 080714
	}
	
	//ObjectID�Ǘ�
	ANumber	GetID( const AIndex inIndex ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetID(inIndex);
	}
	/**
	ObjectID����Index�擾�i�ᑬ�Ȃ̂ŋɗ͎g�p���Ȃ����Ɓj
	*/
	AIndex	GetIndexByID( const AObjectID inID ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetIndexByID(inID);
	}
	
	//�����iinTargetIndex, inTargetItemCount�́ATarget�̈ꕔ���ƈ�v����z��v�f���������邽�߂Ɏg�p����B�j
	//���iID->Index���x���I�j
	AIndex	Find( const U& inTargetData, const AIndex inTargetIndex = 0, const AItemCount inTargetItemCount = kIndex_Invalid ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		unsigned long	hash = GetHashValue(inTargetData,inTargetIndex,inTargetItemCount);
		AIndex	hashstartpos = hash%(mHash.GetItemCount());
		
		AIndex	hashpos = hashstartpos;
		while( true )
		{
			AObjectID	dataObjectID = mHash.Get(hashpos);
			if( dataObjectID.val == kObjectArrayItemHash_NoData )   break;//#216
			if( dataObjectID.val != kObjectArrayItemHash_Deleted )//#216
			{
				AIndex	dataIndex = mObjectArray.GetIndexByID(dataObjectID);//#598
				if( mObjectArray.GetObjectConst(dataIndex).Compare(inTargetData,inTargetIndex,inTargetItemCount) == true )
				{
					return dataIndex;
				}
			}
			hashpos = GetNextHashPos(hashpos);
			if( hashpos == hashstartpos )   break;
		}
		return kIndex_Invalid;
	}
	//�����iObjectID���擾�j
	AObjectID	FindObjectID( const U& inTargetData, const AIndex inTargetIndex = 0, const AItemCount inTargetItemCount = kIndex_Invalid ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		unsigned long	hash = GetHashValue(inTargetData,inTargetIndex,inTargetItemCount);
		AIndex	hashstartpos = hash%(mHash.GetItemCount());
		
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			//�������̂��߁A�|�C���^�g�p
			AStArrayPtr<AObjectID>	arrayptr(mHash,0,mHash.GetItemCount());
			AObjectID*	p = arrayptr.GetPtr();
			AIndex	hashpos = hashstartpos;
			while( true )
			{
				AObjectID	dataObjectID = p[hashpos];
				if( dataObjectID.val == kObjectArrayItemHash_NoData )   break;//#216
				if( dataObjectID.val != kObjectArrayItemHash_Deleted )
				{
					if( mObjectArray.GetObjectConstByID(dataObjectID).Compare(inTargetData,inTargetIndex,inTargetItemCount) == true )
					{
						return dataObjectID;
					}
				}
				hashpos = GetNextHashPos(hashpos);
				if( hashpos == hashstartpos )   break;
			}
		}
		return kObjectID_Invalid;
	}
	//�����iObjectID���擾�j
	void	FindObjectID( const U& inTargetData, const AIndex inTargetIndex/* = 0*/, const AItemCount inTargetItemCount/* = kIndex_Invalid*/,
			AArray<AObjectID> &outObjectIDArray ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		outObjectIDArray.DeleteAll();
		
		unsigned long	hash = GetHashValue(inTargetData,inTargetIndex,inTargetItemCount);
		AIndex	hashstartpos = hash%(mHash.GetItemCount());
		
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			//�������̂��߁A�|�C���^�g�p
			AStArrayPtr<AObjectID>	arrayptr(mHash,0,mHash.GetItemCount());
			AObjectID*	p = arrayptr.GetPtr();
			AIndex	hashpos = hashstartpos;
			while( true )
			{
				AObjectID	dataObjectID = p[hashpos];
				if( dataObjectID.val == kObjectArrayItemHash_NoData )   break;//#216
				if( dataObjectID.val != kObjectArrayItemHash_Deleted )//#216
				{
					if( mObjectArray.GetObjectConstByID(dataObjectID).Compare(inTargetData,inTargetIndex,inTargetItemCount) == true )
					{
						outObjectIDArray.Add(dataObjectID);
					}
				}
				hashpos = GetNextHashPos(hashpos);
				if( hashpos == hashstartpos )   break;
			}
		}
	}
	
	//ItemCount
	AItemCount	GetItemCount() const 
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetItemCount();
	}
	
	
  private:
	//Hash�l�擾
	//�eT���ɒ�`���邱�ƁB
	unsigned long	GetHashValue( const U& inData , const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const;
	
	//�n�b�V���e�[�u�������������A�Ώ�Array�̑S�f�[�^��o�^����B
	void	MakeHash( const ABool inRetainSameHashSize = false )//win 080714 �������i���݂̃n�b�V���T�C�Y���ێ�����t���O�ǉ��j
	{
		//�Ώ�Array�̃T�C�Y���傫��������return
		if( mObjectArray.GetItemCount() >= kItemCount_HashMaxSize/2 )
		{
			_ACError("cannot make hash because array is too big",this);
			return;
		}
		AItemCount	hashsize = mHash.GetItemCount();//win 080714 ������
		if( inRetainSameHashSize == false )//win 080714 ������
		{
			//�n�b�V���e�[�u���S�폜
			mHash.DeleteAll();
			hashsize = mObjectArray.GetItemCount()* /*B0000 ������ 3*/mHashSizeMultiple+128;
			//�n�b�V���T�C�Y���ő�n�b�V���T�C�Y�����傫����΁A�␳����B
			if( hashsize > kItemCount_HashMaxSize )   hashsize = kItemCount_HashMaxSize;
			//�n�b�V���e�[�u���m��
			mHash.Reserve(0,hashsize);
		}
		//�n�b�V���e�[�u��������
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			//�������̂��߁A�|�C���^�g�p
			AStArrayPtr<AObjectID>	arrayptr(mHash,0,mHash.GetItemCount());
			AObjectID*	p = arrayptr.GetPtr();
			for( AIndex i = 0; i < hashsize; i++ )
			{
				p[i].val = kObjectArrayItemHash_NoData;//#216
			}
		}
		//�Ώ�Array�̌��݂̃f�[�^��S�ēo�^
		AItemCount	datacount = mObjectArray.GetItemCount();
		for( AIndex i = 0; i < datacount; i++ )
		{
			RegisterToHash(i,mObjectArray.GetObjectConst(i).GetObjectID());
		}
		//fprintf(stderr,"AHashObjectArray::MakeHash() ");
		//"Deleted"��������   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
		mHash_DeletedCount = 0;
		//���v���#271
		gStatics_AHashObjectArray_MakeHashCount++;
	}
	
	//�n�b�V���X�V�i�ǉ��j
	void	UpdateHash_DataAdded( const AIndex inIndex, const AObjectID& inObjectID )
	{
		//�n�b�V���T�C�Y���Ώۃf�[�^�z��̌��݂̃T�C�Y��2�{������������΁A�n�b�V���e�[�u�����č\������B
		if( mHash.GetItemCount() < (mObjectArray.GetItemCount()+mHash_DeletedCount)* /*B0000 ������ 2*/(mHashSizeMultiple*2/3) )//#303 "Deleted"�̌���Rehash���f�Ɏg�p����
		{
			MakeHash();
		}
		//�����łȂ��ꍇ�͑ΏۃA�C�e�������ǉ��o�^����B
		else
		{
			//�n�b�V���ɓo�^
			RegisterToHash(inIndex,inObjectID);
		}
	}
	
	//�n�b�V���X�V�i�폜�j
	void UpdateHash_DataDeleted( const AIndex inIndex, const AObjectID inObjectID )
	{
		//�n�b�V���l�v�Z
		unsigned long	hash = GetHashValue(mObjectArray.GetObjectConst(inIndex).GetPrimaryKeyData(),0,kIndex_Invalid);
		AIndex	hashstartpos = hash%(mHash.GetItemCount());
		
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			//�������̂��߁A�|�C���^�g�p
			AStArrayPtr<AObjectID>	arrayptr(mHash,0,mHash.GetItemCount());
			AObjectID*	p = arrayptr.GetPtr();
			AIndex	hashpos = hashstartpos;
			while( true )
			{
				AObjectID	dataObjectID = p[hashpos];
				if( dataObjectID.val == kObjectArrayItemHash_NoData )   break;//#216
				if( dataObjectID.val != kObjectArrayItemHash_Deleted )//#216
				{
					if( dataObjectID.val == inObjectID.val )//#216
					{
						//�폜���s
						if( p[GetNextHashPos(hashpos)].val == kObjectArrayItemHash_NoData )
						{
							p[hashpos].val = kObjectArrayItemHash_NoData;//#216
							//win 080713 NoData���������񂾂Ƃ��A�O�̃n�b�V���ʒu�i�����I�ɂ͎��j��Deleted�Ȃ�NoData�ɏ�������
							//[hashpos-1]:NoData
							//[hashpos  ]:NoData�����񏑂�����
							//[hashpos+1]:Deleted������Deleted��NoData�ɂ���ihashpos�̈ʒu�̃f�[�^�ւ̃����N���Ȃ��邽�߂ɑ��݂��Ă���Deleted�Ȃ̂ŁB�j
							//[hashpos+2]:Deleted������Ɏ���Deleted�Ȃ炱����NoData
							AIndex	hp = hashpos+1;
							if( hp >= mHash.GetItemCount() )   hp = 0;
							while( p[hp].val == kObjectArrayItemHash_Deleted )//#216
							{
								//Deleted�Ȃ�NoData�ɏ��������Čp��
								p[hp].val = kObjectArrayItemHash_NoData;//#216
								hp++;
								if( hp >= mHash.GetItemCount() )   hp = 0;
								//"Deleted"��"NoData"�ɂ����̂�"Deleted"�����f�N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
								mHash_DeletedCount--;
							}
						}
						else
						{
							p[hashpos].val = kObjectArrayItemHash_Deleted;//#216
							//"Deleted"�ɂ����̂�"Deleted"�����C���N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
							mHash_DeletedCount++;
						}
						return;
					}
				}
				hashpos = GetNextHashPos(hashpos);
				if( hashpos == hashstartpos )   break;
			}
		}
	}
	
	//�n�b�V���e�[�u���ɁA�Ώ�Array�̎w��A�C�e����o�^����B
	//�f�[�^Index�ԍ��̈ړ��Ȃ�
	void RegisterToHash( const AIndex inIndex, const AObjectID& inObjectID )
	{
		//�n�b�V���l�v�Z
		unsigned long	hash = GetHashValue(mObjectArray.GetObjectConst(inIndex).GetPrimaryKeyData(),0,kIndex_Invalid);
		AIndex	hashstartpos = hash%(mHash.GetItemCount());
		
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			//�������̂��߁A�|�C���^�g�p
			//#695 AStArrayPtr<AObjectID>	arrayptr(mHash,0,mHash.GetItemCount());
			//#695 AObjectID*	p = arrayptr.GetPtr();
			//�������̂��߁A����AArray::mDataPtr���擾����B
			//AStArrayPtr<>�́A�����܂ŃR�[�f�B���O�~�X�h�~�Ȃ̂ŁA�ύX�����p�ɂɂ͔������Ȃ�AbsBase���R�[�h�ł́A
			//�K�������g���K�v�������B
			//AHashArray�ւ�10M���ڒǉ��ŁA9s��2s�̌��ʁB
			//rehash����MakeHash�ɂ��g���Ă���̂ŁA�����̍������͏d�v�B
			AObjectID*	p = mHash.GetDataPtrWithoutCheck();
			//�o�^�ӏ�����
			AIndex	hashpos = hashstartpos;
			while( true )
			{
				AObjectID	dataObjectID = p[hashpos];
				if( dataObjectID.val == kObjectArrayItemHash_NoData )   break;
				if( dataObjectID.val == kObjectArrayItemHash_Deleted )
				{
					//"Deleted"�Ƀf�[�^�㏑������̂�"Deleted"�����f�N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
					mHash_DeletedCount--;
					break;
				}
				hashpos = GetNextHashPos(hashpos);
				if( hashpos == hashstartpos )
				{
					_ACError("no area in hash",this);
					return;
				}
			}
			
			//�o�^
			p[hashpos] = inObjectID;
		}
	}
	
	//����Hash�ʒu�擾
	AIndex	GetNextHashPos( const AIndex& inHashPosition ) const
	{
		if( inHashPosition == 0 )
		{
			return mHash.GetItemCount()-1;
		}
		else
		{
			return inHashPosition -1;
		}
	}
	
	//�n�b�V��
	//�z��f�[�^��ObjectID���i�[����B
	AArray<AObjectID>	mHash;
	AItemCount	mHash_DeletedCount;//#303
	
	//�z��f�[�^
	AObjectArrayIDIndexed<T>	mObjectArray;//#693
	
	//�n�b�V���T�C�Y�{�� B0000 ������ HTML��name�������O���[�o�����ʎq�ɓ����悤�ɂ�����Aphp_manual_ja.html�ŕ`�摬�x���ɒ[�ɒx���Ȃ�������
	//AIdentifierList�̃����omIdentifierArray�̔{����10�ɐݒ肷��B����ȊO�͏]���ʂ�3�Ƃ���B080822
	ANumber	mHashSizeMultiple;
	
	//#598
	/**
	�X���b�h�Z�[�tEnable/Disable�ݒ�
	inEnable=true�̏ꍇ�AAHashObjectArray�̓����f�[�^�i������Array�v�f��Object�͊܂܂Ȃ��j�ɂ��Ă̓X���b�h�Z�[�t�ƂȂ�B
	�i�O���ɓn�����f�[�^�ɂ��Ă̓X���b�h�Z�[�t�ł͂Ȃ��i�����f�[�^�Ƃ̐������ۏ؂���Ȃ��j�̂Œ��ӁB
	���Ƃ���GetItemCount()�ō��ڐ����擾�����ꍇ�A���̒l�̓R�[������mutex�������Ȃ�����A
	�ۏ؂���Ȃ��B�i�ʃX���b�h��Insert/Delete�����\��������B�j
	�܂��AArray�v�f��Object���X���b�h�Z�[�t�ł͂Ȃ��̂ŁA�K�v�Ȃ�ʓrmutex�K�v�B
	�f�t�H���g��Enable�B
	*/
  public:
	void	EnableThreadSafe( const ABool inEnable ) { mMutexEnabled = inEnable; }
  private:
	mutable AThreadMutex	mMutex;
	ABool	mMutexEnabled;
	
	//Object���擾
  public:
	virtual AConstCharPtr	GetClassName() const { return "AHashObjectArray<>"; }
	
};


