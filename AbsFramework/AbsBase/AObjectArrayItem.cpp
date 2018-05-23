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

#include "stdafx.h"

#include "ABase.h"

//#693 ObjectID�ɂ̓|�C���^���i�[����悤�ɕύX
#if 0
const /*#216 AObjectID*/ANumber	kObjectID_Min = 1;//#216
const /*#216 AObjectID*/ANumber	kObjectID_Max = 0x7FFFFFFF;//Mac��Win���A���z���������32bit�Ȃ̂ŁA���ꂾ������Ώ\���̂͂��i�����炪�{�g���l�b�N�ɂȂ邱�Ƃ͂Ȃ��j #216

/*#216 AObjectID*/ANumber	gNextObjectIDCandidate = kObjectID_Min;
AArray<AObjectArrayItem*>	gObjectIDHash;
AItemCount	gObjectIDHash_DeletedCount = 0;//#303 "Deleted"�̌���Rehash���f�Ɏg�p����
AItemCount	gObjectIDItemCount;
AThreadMutex	gObjectIDMutex;

AObjectArrayItem*	kObjectArrayItemHash_Deleted = (AObjectArrayItem*)0xFFFFFFFF;
AObjectArrayItem*	kObjectArrayItemHash_NoData = (AObjectArrayItem*)NULL;
#endif


#pragma mark ===========================
#pragma mark [�N���X]AObjectArrayItem
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

//�R���X�g���N�^
AObjectArrayItem::AObjectArrayItem( AObjectArrayItem* inParent ) : mObjectID(kObjectID_Invalid), mParentObjectArrayItem(NULL)
,mRetainCount(0)//#717
{
	/*#693 ObjectID�ɂ̓|�C���^���i�[����悤�ɕύX
	if( inParent != NULL )
	{
		mObjectID = RegisterObject(this);
		mParentObjectArrayItem = inParent;
	}*/
	mObjectID.val = this;//#693
	mParentObjectArrayItem = inParent;//#693
}

//�f�X�g���N�^
AObjectArrayItem::~AObjectArrayItem()
{
	/*#693 ObjectID�ɂ̓|�C���^���i�[����悤�ɕύX
	if( mObjectID != kObjectID_Invalid )
	{
		UnregisterObject(mObjectID);
	}
	*/
}

//#693 ObjectID�ɂ̓|�C���^���i�[����悤�ɕύX
#if 0
#pragma mark ===========================

#pragma mark ---ObjectID�Ǘ�(static)

/*

AObjectArray�o�R�Ő������ꂽAObjectArrayItem�́A�A�v�����ň�ӂ�ObjectID�����B
�܂��AObjectID�ƁA���̃I�u�W�F�N�g�ւ̃|�C���^�́AgObjectIDHash�ɂĊǗ�����A
ObjectID����|�C���^�������ł���B
��ӂ�ObjectID��������Ƃ��́A����gObjectIDHash��p����B

*/

//AObjectArrayItem�o�^
AObjectID	AObjectArrayItem::RegisterObject( AObjectArrayItem* inObject )
{
	AStMutexLocker	locker(gObjectIDMutex);
	//#216 AObjectID	objectID = gNextObjectIDCandidate;
	AObjectID	objectID;//#216
	objectID.val = gNextObjectIDCandidate;//#216
	while( FindHashIndexByObjectID(objectID) != kIndex_Invalid )
	{
		if( objectID.val < kObjectID_Max )//#216
		{
			objectID.val++;//#216
		}
		else
		{
			objectID.val = kObjectID_Min;//#216
		}
		//ObjectID�ɋ󂫂Ȃ��̏ꍇ
		if( objectID.val == gNextObjectIDCandidate )//#216
		{
			_ACThrow("ObjectID Full",NULL);
		}
	}
	RegisterToHash(objectID,inObject);
	gNextObjectIDCandidate = objectID.val+1;//#216
	//#303 ������l��Max�ȏ�ɂȂ�����Min��ݒ肷��
	if( gNextObjectIDCandidate >= kObjectID_Max )
	{
		gNextObjectIDCandidate = kObjectID_Min;
	}
	return objectID;
}

//AObjectArrayItem�o�^����
void	AObjectArrayItem::UnregisterObject( const AObjectID inObjectID )
{
	AStMutexLocker	locker(gObjectIDMutex);
	DeleteFromHash(inObjectID);
}

//����Hash�ʒu�擾
inline AIndex	AObjectArrayItem::GetNextHashPos( const AIndex inHashPosition )
{
	if( inHashPosition == 0 )
	{
		return gObjectIDHash.GetItemCount()-1;
	}
	else
	{
		return inHashPosition -1;
	}
}

//�n�b�V�����猟��
AIndex	AObjectArrayItem::FindHashIndexByObjectID( const AObjectID inObjectID )
{
	if( gObjectIDHash.GetItemCount() == 0 )   return kIndex_Invalid;
	//�n�b�V������
	unsigned long	hash = GetHashValue(inObjectID);
	AIndex	hashstartpos = hash%(gObjectIDHash.GetItemCount());
	
	//���v���L�^ #271
	gStatics_AObjectArrayItem_FindCount++;
	gStatics_AObjectArrayItem_CompareCount++;
	//�q�b�g���Ȃ��p�x�̂ق��������̂�
	AObjectArrayItem*	firstHitData = gObjectIDHash.Get(hashstartpos);
	if( firstHitData == kObjectArrayItemHash_NoData )
	{
		gStatics_AObjectArrayItem_FirstDataNull++;//#271 ���v���L�^
		return kIndex_Invalid;
	}
	//��x�Ńq�b�g����ꍇ�͂����Ń��^�[��#271
	else if( firstHitData != kObjectArrayItemHash_Deleted )
	{
		if( firstHitData->GetObjectID() == inObjectID )
		{
			gStatics_AObjectArrayItem_FirstDataHit++;//#271 ���v���L�^
			return hashstartpos;
		}
	}
	//�������̂��߁A�|�C���^�g�p�������񐔂͕��ϐ�����x�Ȃ̂Ŗ������ă|�C���^�g�p���Ȃ��ق��������͂�
	//#271 AStArrayPtr<AObjectArrayItem*>	arrayptr(gObjectIDHash,0,gObjectIDHash.GetItemCount());
	//#271 AObjectArrayItem**	p = arrayptr.GetPtr();
	AIndex	hashpos = hashstartpos;
	hashpos = GetNextHashPos(hashpos);//#271 �ŏ��̈ʒu�͏�L�Ō����ς݂Ȃ̂Ŏ��̈ʒu����n�߂�
	while( true )
	{
		gStatics_AObjectArrayItem_CompareCount++;//���v���#271
		AObjectArrayItem*	ptr = gObjectIDHash.Get(hashpos);//#271 p[hashpos];
		if( ptr == kObjectArrayItemHash_NoData )   break;
		if( ptr != kObjectArrayItemHash_Deleted )
		{
			if( ptr->GetObjectID() == inObjectID )
			{
				return hashpos;
			}
		}
		hashpos = GetNextHashPos(hashpos);
		if( hashpos == hashstartpos )   break;
	}
	return kIndex_Invalid;
	
}

//�n�b�V���֓o�^
void	AObjectArrayItem::RegisterToHash( const AObjectID inObjectID, AObjectArrayItem* inObject )
{
	//�n�b�V���T�C�Y���Ώۃf�[�^�z��̌��݂̃T�C�Y��3�{������������΁A�n�b�V���e�[�u�����č\������B
	if( (gObjectIDHash.GetItemCount() <= (gObjectIDItemCount+gObjectIDHash_DeletedCount)*3) || 
				(gObjectIDHash_DeletedCount>100000) )//#303 "Deleted"�̌���Rehash���f�Ɏg�p����
	{
		MakeHash();
	}
	//�n�b�V���ɓo�^
	WriteToHash(inObjectID,inObject);
	gObjectIDItemCount++;
}

//�n�b�V������폜
void	AObjectArrayItem::DeleteFromHash( const AObjectID inObjectID )
{
	/*win 080712 �폜�}�[�N���炯�ɂȂ��Ă��܂����΍�B���̃\�[�X�ɕύX�B���X�u�P�v�̏����͂���Ă�̂ŁA�Q�łǂꂾ�����ʂ����邩�l�q���B
	AIndex index = FindHashIndexByObjectID(inObjectID);
	if( index == kIndex_Invalid )   _ACError("",NULL);
	if( gObjectIDHash.Get(GetNextHashPos(index)) == kObjectArrayItemHash_NoData )
	{
		gObjectIDHash.Set(index,kObjectArrayItemHash_NoData);
	}
	else
	{
		gObjectIDHash.Set(index,kObjectArrayItemHash_Deleted);
	}*/
	//�n�b�V������
	unsigned long	hash = GetHashValue(inObjectID);
	AIndex	hashstartpos = hash%(gObjectIDHash.GetItemCount());
	{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
		//�������̂��߁A�|�C���^�g�p
		AStArrayPtr<AObjectArrayItem*>	arrayptr(gObjectIDHash,0,gObjectIDHash.GetItemCount());
		AObjectArrayItem**	p = arrayptr.GetPtr();
		AIndex	hashpos = hashstartpos;
		while( true )
		{
			AObjectArrayItem*	ptr = p[hashpos];
			if( ptr == kObjectArrayItemHash_NoData )   {_ACError("cannot delete(not found)",NULL);break;}
			if( ptr != kObjectArrayItemHash_Deleted )
			{
				if( ptr->GetObjectID() == inObjectID )
				{
					if( p[GetNextHashPos(hashpos)] == kObjectArrayItemHash_NoData )
					{
						//�P ���̃n�b�V���ʒu�i�����I�ɂ͑O�j�̃f�[�^��NoData�Ȃ�A�폜����ʒu��NoData�ɂ��Ă悢
						p[hashpos] = kObjectArrayItemHash_NoData;
						//�Q ����ɁA�O�̃n�b�V���ʒu�i�����I�ɂ͎��j�̃f�[�^��Deleted�������ꍇ�́ADeleted�ȊO��������܂�NoData�ɂ���
						//[hashpos-1]:NoData
						//[hashpos  ]:NoData�����񏑂�����
						//[hashpos+1]:Deleted������Deleted��NoData�ɂ���ihashpos�̈ʒu�̃f�[�^�ւ̃����N���Ȃ��邽�߂ɑ��݂��Ă���Deleted�Ȃ̂ŁB�j
						//[hashpos+2]:Deleted������Ɏ���Deleted�Ȃ炱����NoData
						AIndex	hp = hashpos+1;
						if( hp >= gObjectIDHash.GetItemCount() )   hp = 0;
						while( p[hp] == kObjectArrayItemHash_Deleted )
						{
							p[hp] = kObjectArrayItemHash_NoData;
							hp++;
							if( hp >= gObjectIDHash.GetItemCount() )   hp = 0;
							//"Deleted"��"NoData"�ɂ����̂�"Deleted"�����f�N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
							gObjectIDHash_DeletedCount--;
						}
					}
					else
					{
						//���̃n�b�V���ʒu�̃f�[�^��NoData�ȊO�Ȃ�ADeleted�ɂ��Ă����K�v������
						p[hashpos] = kObjectArrayItemHash_Deleted;
						//"Deleted"�ɂ����̂�"Deleted"�����C���N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
						gObjectIDHash_DeletedCount++;
					}
					break;
				}
			}
			hashpos = GetNextHashPos(hashpos);
			if( hashpos == hashstartpos )   {_ACError("cannot delete(not found)",NULL);break;}
		}
	}
	
	gObjectIDItemCount--;
}

//�n�b�V���č\���A�܂��́A�V�K�쐬
void	AObjectArrayItem::MakeHash()
{
	//�Ώ�Array�̃T�C�Y���傫��������return
	if( gObjectIDItemCount >= kItemCount_HashMaxSize/2 )
	{
		_ACError("cannot make hash because array is too big",NULL);
		return;
	}
	//�n�b�V������ꎞ�I�Ƀf�[�^���ڂ�
	AArray<AObjectArrayItem*>	tmpArray;
	for( AIndex i = 0; i < gObjectIDHash.GetItemCount(); i++ )
	{
		AObjectArrayItem*	ptr = gObjectIDHash.Get(i);
		if( ptr == kObjectArrayItemHash_NoData || ptr == kObjectArrayItemHash_Deleted )   continue;
		tmpArray.Add(ptr);
	}
	if( tmpArray.GetItemCount() != gObjectIDItemCount )   _ACError("",NULL);
	//�n�b�V���e�[�u���S�폜
	gObjectIDHash.DeleteAll();
	AItemCount	hashsize = gObjectIDItemCount*4 + 1000000;//#303 ��{�T�C�Y��65536��1000000�֕ύX +0x10000;
	//�n�b�V���T�C�Y���ő�n�b�V���T�C�Y�����傫����΁A�␳����B
	if( hashsize > kItemCount_HashMaxSize )   hashsize = kItemCount_HashMaxSize;
	//�n�b�V���e�[�u���m��
	gObjectIDHash.Reserve(0,hashsize);
	//�n�b�V���e�[�u��������
	for( AIndex i = 0; i < hashsize; i++ )
	{
		gObjectIDHash.Set(i,kObjectArrayItemHash_NoData);
	}
	//�Ώ�Array�̌��݂̃f�[�^��S�ēo�^
	for( AIndex i = 0; i < tmpArray.GetItemCount(); i++ )
	{
		AObjectArrayItem*	ptr = tmpArray.Get(i);
		WriteToHash(ptr->GetObjectID(),ptr);
	}
	//"Deleted"��������   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
	gObjectIDHash_DeletedCount = 0;
	//#298 ���v���L�^
	gStatics_AObjectArrayItem_MakeHashCount++;
}

//�n�b�V���֏�������
void	AObjectArrayItem::WriteToHash( const AObjectID inObjectID, AObjectArrayItem* inObject )
{
	//�n�b�V���l�v�Z
	unsigned long	hash = GetHashValue(inObjectID);
	AIndex	hashstartpos = hash%(gObjectIDHash.GetItemCount());
	
	//�o�^�ӏ�����
	AIndex	hashpos = hashstartpos;
	while( true )
	{
		AObjectArrayItem*	ptr = gObjectIDHash.Get(hashpos);
		if( ptr == kObjectArrayItemHash_NoData )   break;
		if( ptr == kObjectArrayItemHash_Deleted )
		{
			//"Deleted"�Ƀf�[�^�㏑������̂�"Deleted"�����f�N�������g   #303 "Deleted"�̌���Rehash���f�Ɏg�p����
			gObjectIDHash_DeletedCount--;
			break;
		}
		hashpos = GetNextHashPos(hashpos);
		if( hashpos == hashstartpos )
		{
			_ACError("no area in hash",NULL);
			return;
		}
	}
	
	//�o�^
	gObjectIDHash.Set(hashpos,inObject);
}

//�n�b�V���֐�
inline unsigned long	AObjectArrayItem::GetHashValue( const AObjectID inObjectID )
{
	return static_cast<unsigned long>(inObjectID.val*3);//#216
}
#endif
//#271
/**
ObjectArrayItem���v���擾
*/
void	AObjectArrayItem::GetStatics( AItemCount& outHashSize, AItemCount& outItemCount, AItemCount& outNoDataCount, AItemCount& outDeletedCount,
		ANumber& outNextCandidate )
{
	/*#693
	//
	outHashSize = gObjectIDHash.GetItemCount();
	outItemCount = gObjectIDItemCount;
	outNoDataCount = 0;
	outDeletedCount = 0;
	outNextCandidate = gNextObjectIDCandidate;
	//
	AStMutexLocker	locker(gObjectIDMutex);
	for( AIndex i = 0; i < gObjectIDHash.GetItemCount(); i++ )
	{
		if( gObjectIDHash.Get(i) == kObjectArrayItemHash_NoData )   outNoDataCount++;
		if( gObjectIDHash.Get(i) == kObjectArrayItemHash_Deleted )   outDeletedCount++;
	}
	//
	if( outDeletedCount != gObjectIDHash_DeletedCount )   _ACError("AObjectArrayItem Hash Deleted coutn is not correct",NULL);
	*/
}

//#271
AByteCount	AObjectArrayItem::GetAllocatedSize() const
{
	return AMemoryWrapper::AllocatedSize(this);
}

//
void	AObjectArrayItem::DumpObjectID( const AFileAcc& inDumpFile )
{
	/*�e�X�g
	�n�b�V�����e�����o��
	AText	text;
	for( AIndex i = 0; i < gObjectIDHash.GetItemCount(); i++ )
	{
		AObjectID	id = kObjectID_Invalid;
		if( gObjectIDHash.Get(i) != kObjectArrayItemHash_NoData &&
		gObjectIDHash.Get(i) != kObjectArrayItemHash_Deleted )
		{
			id = gObjectIDHash.Get(i)->GetObjectID();
		}
		text.AddFormattedCstring("%06d %08X %06d\n",i,gObjectIDHash.Get(i),id.val);
	}
	inDumpFile.WriteFile(text);
	*/
	/*#693
	ANumber	totalAllocatedSize = 0;
	ANumber	identifierListAllocatedSize = 0;//#348
	//
	AText	text;
	AObjectID	objectID;//#216
	objectID.val = kObjectID_Min;//#216
	AItemCount	itemCount = 0;
	while( itemCount < gObjectIDItemCount )
	{
		AObjectArrayItem*	ptr = NULL;
		while(true)
		{
			AIndex	index = FindHashIndexByObjectID(objectID);
			if( index == kIndex_Invalid )
			{
				objectID.val++;//#216
				continue;
			}
			else
			{
				ptr = gObjectIDHash.Get(index);
				break;
			}
		}
		text.AddFormattedCstring("ObjectID:%09d AllocatedSize:%011d ObjectPtr:%08X Class::%s\r",objectID,ptr->GetAllocatedSize(),ptr,ptr->GetClassName());
		totalAllocatedSize += ptr->GetAllocatedSize();
		//#348
		AText	className;
		className.SetCstring(ptr->GetClassName());
		if( className.Compare("AIdentifierList") == true )
		{
			identifierListAllocatedSize += ptr->GetAllocatedSize();
		}
		objectID.val++;//#216
		itemCount++;
	}
	text.Add(kUChar_LineEnd);
	text.AddFormattedCstring("TotalAllocatedSize:%d \n",totalAllocatedSize);
	text.AddFormattedCstring("Size of all AIdentifierList:%d \n",identifierListAllocatedSize);//#348
	inDumpFile.WriteFile(text);
	*/
	/*
	AText	text;
	AItemCount	itemCount = gObjectIDArrayForDebug.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AObjectArrayItem*	ptr = gObjectIDArrayForDebug.Get(i);
		AText	className;
		className.SetCstring(ptr->GetClassName());
		AText	dataSammary;
		ptr->GetDataSummary(dataSammary);
		text.AddFormattedCstring("ObjectID:%08x Class: ",ptr);
		text.AddText(className);
		text.AddCstring("\n");
		text.AddText(dataSammary);
		text.AddCstring("\n");
	}
	inDumpFile.WriteFile(text);
	 */
}

