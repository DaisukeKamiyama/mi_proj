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

AObjectArrayItem

*/

#pragma once

#include "ABaseTypes.h"

class AFileAcc;
class AObjectArrayItem;

//#693 ABaseTypes.h����ړ����Aval���|�C���^�ɕύX
/**
ObjectID��`
*/
struct AObjectID
{
	AObjectArrayItem*	val;
	//�|�C���^����ObjectID���쐬 #904
	static AObjectID	GetObjectIDFromPtr( AObjectArrayItem* inPtr )
	{ AObjectID	objID = {inPtr}; return objID; }
	//operator
	bool operator==(AObjectID inObjectID) const { return (val==inObjectID.val); }
	bool operator!=(AObjectID inObjectID) const { return (val!=inObjectID.val); }
};
const AObjectID	kObjectID_Invalid = {NULL};


#pragma mark ===========================
#pragma mark [�N���X]AObjectArrayItem
/**
AObjectArray<>�ɕێ������N���X�̊��N���X
ObjectID���Ǘ�����
*/
class AObjectArrayItem
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AObjectArrayItem( AObjectArrayItem* inParent );
	virtual ~AObjectArrayItem();
	virtual void	DoDeleted() {}//AObjectArray����̍폜����ɃR�[�������֐�
	
	//debug
  public:
	virtual AByteCount	GetAllocatedSize() const;//#271 { return 0; }
	
	//ObjectID�ݒ�^�擾
  public:
	//#693 inline void	SetObjectID( const AObjectID inObjectID ) { mObjectID = inObjectID; }
	inline AObjectID	GetObjectID() const { return mObjectID; }
  private:
	AObjectID	mObjectID;
	AObjectArrayItem*	mParentObjectArrayItem;
	
	//RetainCount���� #717
	//RetainCount > 0 �̊Ԃ�AObjectArrayItemTrash�Ń������������Ȃ�
	//AObjectArrayIDIndexed::GetObjectAndRetain()��increment�����B
  public:
	void					IncrementRetainCount() { mRetainCount++; }
	void					DecrementRetainCount() { mRetainCount--; }
	AItemCount				GetRetainCount() const { return mRetainCount; }
  private:
	AItemCount							mRetainCount;
	
	//Object���擾
  public:
	virtual AConstCharPtr	GetClassName() const { return "AObjectArrayItem"; }
	const AObjectArrayItem*	GetParent() const { return mParentObjectArrayItem; }
	
	/*#693
	//ObjectID�Ǘ�(static)
  private:
	static AObjectID	RegisterObject( AObjectArrayItem* inObject );
	static void	UnregisterObject( const AObjectID inObjectID );
	static AIndex	GetNextHashPos( const AIndex inHashPosition );
	static AIndex	FindHashIndexByObjectID( const AObjectID inObjectID );
	static void	RegisterToHash( const AObjectID inObjectID, AObjectArrayItem* inObject );
	static void	DeleteFromHash( const AObjectID inObjectID );
	static void	MakeHash();
	static void	WriteToHash( const AObjectID inObjectID, AObjectArrayItem* inObject );
	static unsigned long	GetHashValue( const AObjectID inObjecID );
	*/
	//�f�o�b�O
  public:
	static void	GetStatics( AItemCount& outHashSize, AItemCount& outItemCount, AItemCount& outNoDataCount, AItemCount& outDeletedCount,
			ANumber& outNextCandidate );
	static void	DumpObjectID( const AFileAcc& inDumpFile );
	
	//�R�s�[�����q�^������Z�q�̓T�u�N���X�Ŗ����I�ɒ�`����Ȃ�����A�g�p�s�B
  protected:
	AObjectArrayItem( const AObjectArrayItem& );//�R�s�[�����q
	AObjectArrayItem& operator=( const AObjectArrayItem& );//������Z�q
};

/**
�u���b�N�𔲂���Ƃ���AObjectArrayItem��DecrementRetainCount()���R�[�����邱�Ƃ�ۏ؂��邽�߂�stack�N���X
*/
class AStDecrementRetainCounter
{
  public:
	AStDecrementRetainCounter( AObjectArrayItem* inPtr ) : mPtr(inPtr) {}
	~AStDecrementRetainCounter()
	{ mPtr->DecrementRetainCount(); }
	//�iretain����Ă���̂ŁA�|�C���^�̓f�X�g���N�^�܂ŗL���B�j
  private:
	AObjectArrayItem*	mPtr;
};


