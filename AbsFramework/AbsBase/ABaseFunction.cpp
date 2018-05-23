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

ABaseFunction

*/

#include "stdafx.h"

#include "ABase.h"

extern ABase*	gBase;

#pragma mark ===========================
#pragma mark [�N���X]ABaseFunction
#pragma mark ===========================
//AbsBase�̑S�̓I�ȋ@�\�ɑ΂��鐧����s���֐��Q

/**
���݃��C���X���b�h�����ǂ����̔���
*/
ABool	ABaseFunction::InMainThread()
{
	if( gBase != NULL )
	{
		return gBase->InMainThread();
	}
	else
	{
		//gBase���܂��p�ӂł��Ă��Ȃ��Ƃ������Ƃ́A�܂��X���b�h���N�����Ă��Ȃ��͂��B
		return true;
	}
}

#pragma mark ===========================

#pragma mark ---AObjectArrayItem�̍폜����

/**
ObjectArray���K�[�x�[�W�R���N�V�����Ώۂɂ���
*/
void	ABaseFunction::AddToObjectArrayItemTrash( AObjectArrayItem* inObjectPtr )
{
	if( gBase != NULL )
	{
		if( gBase->InMainThread() == true )
		{
			gBase->mObjectArrayItemTrashForMainThread.AddToObjectArrayItemTrash(inObjectPtr);
		}
		else
		{
			//���C���X���b�h�O�Ȃ瑦delete
			delete inObjectPtr;
		}
	}
}

/**
�K�[�x�[�W�R���N�V�������s
*/
void	ABaseFunction::DoObjectArrayItemTrashDelete()
{
	if( gBase != NULL )
	{
		if( gBase->InMainThread() == true )
		{
			gBase->mObjectArrayItemTrashForMainThread.DoObjectArrayItemTrashDelete();
		}
		else
		{
			//���C���X���b�h�O�Ȃ珈������
		}
	}
}

/*#344 �������[�v�[���@�\�폜�i���z���������ł͎g�p�����󋵂��܂����肦�Ȃ��̂Łj

#pragma mark ===========================
#pragma mark ---�������Ǘ�

//���z�����������ł͂��܂葶�݉��l���Ȃ��̂Ń������v�[�������͍폜���邩������Ȃ�
void	ABaseFunction::ShowLowMemoryAlertLevel1()
{
}

ABool	ABaseFunction::IsLevel1MemoryPoolReleased()
{
	if( gBase != NULL )
	{
		if( gBase->mLevel1MemoryPool != NULL )
		{
			return false;
		}
	}
	return true;
}

ABool	ABaseFunction::IsLevel2MemoryPoolReleased()
{
	if( gBase != NULL )
	{
		if( gBase->mLevel2MemoryPool != NULL )
		{
			return false;
		}
	}
	return true;
}

void	ABaseFunction::ReleaseMemoryPoolLevel1()
{
	if( gBase != NULL )
	{
		if( gBase->mLevel1MemoryPool != NULL )
		{
			AMemoryWrapper::Free(gBase->mLevel1MemoryPool);
			gBase->mLevel1MemoryPool = NULL;
		}
	}
}

void	ABaseFunction::ReleaseMemoryPoolLevel2()
{
	if( gBase != NULL )
	{
		if( gBase->mLevel2MemoryPool != NULL )
		{
			AMemoryWrapper::Free(gBase->mLevel2MemoryPool);
			gBase->mLevel2MemoryPool = NULL;
		}
	}
}
*/
#pragma mark ===========================
#pragma mark ---�G���[����������

extern void	BackupDataForCriticalError();

/**
�v���I�G���[
*/
void	ABaseFunction::CriticalError( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem )
{
	//�f�[�^�o�b�N�A�b�v
	BackupDataForCriticalError();
	
	//�f�o�b�K�N��
	ABaseFunction::ShowDebugger();
	
	if( gBase != NULL )
	{
		//�g���[�X���O
		ATrace::Log("[CError]",true,inFilename,inLineNumber,inString,true,inObjectArrayItem,true);//B0000 backtrace
	}
}

//#182
/**
�v���I�G���[�E�G���[�ԍ��t��
*/
void	ABaseFunction::CriticalError( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, 
		const ANumber inErrorNumber, const AObjectArrayItem* inObjectArrayItem )
{
	//�f�[�^�o�b�N�A�b�v
	BackupDataForCriticalError();
	
	//�f�o�b�K�N��
	ABaseFunction::ShowDebugger();
	
	//�G���[�e�L�X�g�쐬
	AText	text;
	text.SetCstring(inString);
	text.AddNumber(inErrorNumber);
	
	if( gBase != NULL )
	{
		AStCreateCstringFromAText	cstr(text);
		//�g���[�X���O
		ATrace::Log("[CError]",true,inFilename,inLineNumber,cstr.GetPtr(),true,inObjectArrayItem,true);//B0000 backtrace
	}
}

/**
�v���IThrow
*/
void	ABaseFunction::CriticalThrow( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem )
{
	//�f�[�^�o�b�N�A�b�v
	BackupDataForCriticalError();
	
	//�f�o�b�K�N��
	ABaseFunction::ShowDebugger();
	
	if( gBase != NULL )
	{
		//�g���[�X���O
		ATrace::Log("[CThrow]",true,inFilename,inLineNumber,inString,true,inObjectArrayItem,true);//B0000 backtrace
	}
	
	//throw
	throw inString;
}

/**
�G���[����
*/
void	ABaseFunction::Error( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem )
{
	//�f�o�b�K�N��
	ABaseFunction::ShowDebugger();
	
	if( gBase != NULL )
	{
		//�g���[�X���O
		ATrace::Log("[Error ]",true,inFilename,inLineNumber,inString,false,inObjectArrayItem,true);//B0000 backtrace
	}
}

/**
Throw����
*/
void	ABaseFunction::Throw( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem )
{
	//�f�o�b�K�N��
	ABaseFunction::ShowDebugger();
	
	if( gBase != NULL )
	{
		//�g���[�X���O
		ATrace::Log("[Throw ]",true,inFilename,inLineNumber,inString,false,inObjectArrayItem,true);//B0000 backtrace
	}
	
	//throw
	throw inString;
}

/**
�P�Ȃ��񃍃O�̂��߁i�G���[�ł͂Ȃ��j
*/
void	ABaseFunction::Info( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem )
{
	if( gBase != NULL )
	{
		//�g���[�X���O
		ATrace::Log("  [Info]",false,inFilename,inLineNumber,inString,false,inObjectArrayItem,false);//B0000 backtrace
	}
}

/**
�f�o�b�K�\��
*/
void	ABaseFunction::ShowDebugger()
{
#if IMPLEMENTATION_FOR_MACOSX
	//Xcode3.1�ł��܂����삵�Ȃ��HDebugger();
#endif
}

#pragma mark ===========================

#pragma mark ---���v�f�[�^
//#271

//AHashArray
AItemCount	gStatics_AHashArray_MakeHashCount = 0;
AItemCount	gStatics_AHashArray_FindCount = 0;
AItemCount	gStatics_AHashArray_FirstDataNull = 0;
AItemCount	gStatics_AHashArray_FirstDataHit = 0;
AItemCount	gStatics_AHashArray_CompareCount = 0;
//AHashObjectArray
AItemCount	gStatics_AHashObjectArray_MakeHashCount = 0;
//AObjectArrayItem
AItemCount	gStatics_AObjectArrayItem_MakeHashCount = 0;
AItemCount	gStatics_AObjectArrayItem_FindCount = 0;
AItemCount	gStatics_AObjectArrayItem_FirstDataNull = 0;
AItemCount	gStatics_AObjectArrayItem_FirstDataHit = 0;
AItemCount	gStatics_AObjectArrayItem_CompareCount = 0;

/**
�n�b�V�����v�\��
*/
void	ABaseFunction::HashStatics()
{
	fprintf(stderr,"\n");
	{
		fprintf(stderr,"[AHashArray]\n");
		fprintf(stderr,"   MakeHashCount:%ld\n",gStatics_AHashArray_MakeHashCount);
		fprintf(stderr,"   FindCount:%ld  (FirstDataNull:%ld  FirstDataHit:%ld  NotFirstData:%ld)\n",
				gStatics_AHashArray_FindCount,gStatics_AHashArray_FirstDataNull,gStatics_AHashArray_FirstDataHit,
				gStatics_AHashArray_FindCount-gStatics_AHashArray_FirstDataNull-gStatics_AHashArray_FirstDataHit);
		AFloatNumber	averagecompare = gStatics_AHashArray_CompareCount;
		averagecompare /= gStatics_AHashArray_FindCount;
		fprintf(stderr,"   AverageCompareCount:%g\n",averagecompare);
	}
	{
		fprintf(stderr,"[AHashObjectArray]\n");
		fprintf(stderr,"   MakeHashCount:%ld\n",gStatics_AHashObjectArray_MakeHashCount);
	}
	{
		AItemCount	hashsize, itemcount, nodata, deleted;
		ANumber	nextcandidate;
		AObjectArrayItem::GetStatics(hashsize,itemcount,nodata,deleted,nextcandidate);
		fprintf(stderr,"[AObjectArrayItem]\n");
		fprintf(stderr,"   NextCandidateID:%ld\n",nextcandidate);
		fprintf(stderr,"   MakeHashCount:%ld\n",gStatics_AObjectArrayItem_MakeHashCount);
		fprintf(stderr,"   HashSize:%ld  (ItemCount:%ld  NoData:%ld  Deleted:%ld)\n",hashsize,itemcount,nodata,deleted);
		fprintf(stderr,"   FindCount:%ld  (FirstDataNull:%ld  FirstDataHit:%ld  NotFirstData:%ld)\n",
				gStatics_AObjectArrayItem_FindCount,gStatics_AObjectArrayItem_FirstDataNull,gStatics_AObjectArrayItem_FirstDataHit,
				gStatics_AObjectArrayItem_FindCount-gStatics_AObjectArrayItem_FirstDataNull-gStatics_AObjectArrayItem_FirstDataHit);
		AFloatNumber	averagecompare = gStatics_AObjectArrayItem_CompareCount;
		averagecompare /= gStatics_AObjectArrayItem_FindCount;
		fprintf(stderr,"   AverageCompareCount:%g\n",averagecompare);
	}
}

