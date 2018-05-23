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

AObjectArrayIDIndexed

*/

#pragma once

#include "ABaseFunction.h"
#include "AObjectArrayItem.h"
#include "AStackOnlyObject.h"
#include "AHashArray.h"

#pragma mark ===========================
#pragma mark [�N���X]AObjectArrayIDIndexed
/**
�I�u�W�F�N�g�̉ϒ��z��

���̃I�u�W�F�N�g�́A�����I�u�W�F�N�g��ێ�����B
���ۂɕێ�����̂̓I�u�W�F�N�g�ւ̃|�C���^�����AAObjectArray�I�u�W�F�N�g��Delete�n���\�b�h���R�[���A
�������́AAObjectArray�I�u�W�F�N�g���̂��폜����ƁA�ێ������e�I�u�W�F�N�g��delete�����B

#693 �ȑO��ObjectID�Ƃ��ẮAAObjectArrayItem()�ɂĊ��蓖�Ă�ꂽ�A�v�����ň�ӂ�ID��ݒ肵�Ă������A
�������팸�E�������̂��߁AID�ɂ̓|�C���^���̂��̂�ݒ肷��悤�ɕύX�B
�܂��AID����ObjectID���擾����K�v������ꍇ�́AAObjectArrayIDIndexed�I�u�W�F�N�g�̕����g�p����悤�ɕύX�B�iAArray��AHashArray�𓯎��Ɋi�[����̂̓�������H�����߁j
*/
template<class T> class AObjectArrayIDIndexed : public AObjectArrayItem
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AObjectArrayIDIndexed( AObjectArrayItem* inParent = NULL )
	: AObjectArrayItem(inParent), mMutexEnabled(true)
	{
		//OutOfBounds�̏ꍇ��throw����悤�ɐݒ�
		mObjectIDArray.SetThrowIfOutOfBounds();
	}
	~AObjectArrayIDIndexed()
	{
		DeleteAll();
	}
	
	//�z��f�[�^
  private:
	AHashArray<AObjectID>		mObjectIDArray;//#693
	
  public:
	//�V�K�I�u�W�F�N�g�V�K�}��
	void	InsertNew1Object( const AIndex inIndex, AAbstractFactoryForObjectArray<T>& inFactory )
	{
		//�I�u�W�F�N�g����
		T*	objectPtr = NULL;
		try
		{
			//AbstractFactory���g�p���Đ���
			objectPtr = inFactory.Create();
			if( objectPtr == NULL )   throw "";//���݂̃R���p�C���ł�new���s��throw����̂����ʂ����A�Â����̂ł�NULL��Ԃ��ꍇ������B
		}
		catch(...)
		{
			//�I�u�W�F�N�g�̐������s
			{
				//�������v�[�������̏ꍇ
				_ACThrow("cannot allocate memory (new NG) (critical)",this);
			}
		}
		//�I�u�W�F�N�g�̐���OK
		{
			AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArray�����o��Mutex���b�N #598
			//�z��f�[�^��ObjectID��o�^
			mObjectIDArray.Insert1(inIndex,objectPtr->GetObjectID());
		}
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
		AIndex	index = GetItemCount();
		InsertNew1Object(index,inFactory);
		return index;
	}
	//�V�K�I�u�W�F�N�g�V�K�ǉ��i�f�t�H���gAbstractFactory�g�p�j
	AIndex AddNewObject()
	{
		ADefaultAbstractFactoryForObjectArray<T>	defaultFactory(this);
		return AddNewObject(defaultFactory);
	}
	
	//�I�u�W�F�N�g�擾
	T& GetObject( const AIndex inIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArray�����o��Mutex���b�N #598
		//index�`�F�b�N
		if( inIndex < 0 || inIndex >= mObjectIDArray.GetItemCount() )
		{
			_ACThrow("index out of bounds",this);
		}
		//�I�u�W�F�N�g�擾
		return (T&)(*(mObjectIDArray.Get(inIndex).val));
	}
	//�I�u�W�F�N�g�擾(const)
	const T& GetObjectConst( const AIndex inIndex ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArray�����o��Mutex���b�N #598
		//index�`�F�b�N
		if( inIndex < 0 || inIndex >= mObjectIDArray.GetItemCount() )
		{
			_ACThrow("index out of bounds",this);
		}
		//�I�u�W�F�N�g�擾
		return (T&)(*(mObjectIDArray.Get(inIndex).val));
	}
	
	//�폜
	void	Delete1Object( const AIndex inIndex )
	{
		T*	objectPtr = &(GetObject(inIndex));//#693NULL;//#598
		//�I�u�W�F�N�g�֒ʒm
		objectPtr->DoDeleted();
		{
			AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArray�����o��Mutex���b�N #598
			//�z��f�[�^����ObjectID���폜
			mObjectIDArray.Delete1(inIndex);
		}
		//
		ABaseFunction::AddToObjectArrayItemTrash(objectPtr);
	}
	
	//AObjectArray<>�z����ł�Swap�i�|�C���^�̂݌����j
	void	SwapObject( const AIndex inA, const AIndex inB )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArray�����o��Mutex���b�N #598
		//�z��f�[�^Swap
		mObjectIDArray.Swap(inA,inB);
	}
	
	//Move
  public:
	void	MoveObject( const AIndex inOldIndex, const AIndex inNewIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArray�����o��Mutex���b�N #598
		//�z��f�[�^Move
		mObjectIDArray.Move(inOldIndex,inNewIndex);
	}
	
	//�S�v�f�폜
	void DeleteAll()
	{
		while( GetItemCount() > 0 )
		{
			Delete1Object(GetItemCount()-1);
		}
	}
	
	//ItemCount
	AItemCount GetItemCount() const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArray�����o��Mutex���b�N #598
		//�z��f�[�^�v�f���擾
		return mObjectIDArray.GetItemCount();
	}
	
	//ObjectID�Ǘ�
  public:
	/**
	ObjectID����Index�擾
	*/
	AIndex	GetIndexByID( const AObjectID inID ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArray�����o��Mutex���b�N #598
		//#693return mIDArray.Find(inID);
		//ObjectID����index���擾
		return mObjectIDArray.Find(inID);
	}
	
  public:
	//ObjectID����I�u�W�F�N�g�擾
	T& GetObjectByID( const AObjectID inObjectID )
	{
		//Object�����`�F�b�N
		if( ExistObject(inObjectID) == false )   _ACThrow("Object not found",this);
		//�I�u�W�F�N�g�擾�iObjectID���|�C���^���̂��̂Ȃ̂ŁA���̂܂ܕԂ��B�����`�F�b�N�̂��߂ɂ��̊֐���ʂ����Ƃ��K�v�j
		return (T&)(*(inObjectID.val));
	}
	//ObjectID����I�u�W�F�N�g�擾(const)
	T& GetObjectConstByID( const AObjectID inObjectID ) const
	{
		//Object�����`�F�b�N
		if( ExistObject(inObjectID) == false )   _ACThrow("Object not found",this);
		//�I�u�W�F�N�g�擾�iObjectID���|�C���^���̂��̂Ȃ̂ŁA���̂܂ܕԂ��B�����`�F�b�N�̂��߂ɂ��̊֐���ʂ����Ƃ��K�v�j
		return (T&)(*(inObjectID.val));
	}
	//#272
	/**
	Object�����݂��邩�ǂ�����Ԃ�
	*/
	ABool	ExistObject( const AObjectID inObjectID ) const
	{
		return (GetIndexByID(inObjectID)!=kIndex_Invalid);//#693
	}
	
	//#717
	/**
	Object�ւ̃|�C���^���擾���ARetainCount���C���N�������g����B
	�X���b�h����I�u�W�F�N�g�փA�N�Z�X����Ƃ��A�X���b�h�̏������s���ɂ��̃I�u�W�F�N�g���폜�����\��������ꍇ�A
	���̃��\�b�h���g���ăI�u�W�F�N�g�ւ̃|�C���^���擾����B
	�X���b�h����̃A�N�Z�X���I��������I�u�W�F�N�g��DecrementRetainCount()���R�[������RetainCount���f�N�������g����B
	RetainCount��1�ȏ�̊Ԃ̓I�u�W�F�N�g��delete����Ȃ��B�iAObjectArrayIDIndexed����폜����Ă��AAObjectArrayItemTrash�ɂȂ���āA
	����������͂���Ȃ��B�j
	@note �Ԃ�l��NULL�̏ꍇ������i���łɃI�u�W�F�N�g�폜�ς݂̏ꍇ�j�B�R�[������NULL�`�F�b�N�K�v�B
	*/
	T*	GetObjectAndRetain( const AObjectID inObjectID ) 
	{
		//���̃��\�b�h���g���ꍇ�́Amutex����͕K�{
		if( mMutexEnabled == false )
		{
			_ACError("",NULL);
			mMutexEnabled = true;
		}
		//mutex���b�N����
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//ObjectArray�����o��Mutex���b�N
		if( mObjectIDArray.Find(inObjectID) != kIndex_Invalid )
		{
			//�|�C���^�擾���ARetainCount���C���N�������g����
			T*	object = (T*)(inObjectID.val);
			object->IncrementRetainCount();
			return object;
		}
		else
		{
			//inObjectID�̃I�u�W�F�N�g�����݂��Ȃ��ꍇ��NULL��Ԃ��B
			//�R�[������NULL�`�F�b�N�K�v
			return NULL;
		}
	}
	
	//
  public:
	void	EnableThreadSafe( const ABool inEnable ) { mMutexEnabled = inEnable; }
  private:
	mutable AThreadMutex	mMutex;
	ABool	mMutexEnabled;
	
	//Object���擾
  public:
	virtual AConstCharPtr	GetClassName() const { return "AObjectArrayIDIndexed<>"; }
	
};

