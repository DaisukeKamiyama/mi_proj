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

ABase

*/

#include "stdafx.h"

#include "ABase.h"
#include "../Wrapper.h"
#include "APurgeManager.h"

ABase*	gBase = NULL;

#pragma mark ===========================
#pragma mark [�N���X]ABase
#pragma mark ===========================
//�K���ŏ��ɐ��������ׂ��N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
ABase::ABase( const ABool& inUseTraceLog ) 
	: mUseTraceLog(inUseTraceLog)/*#344 �������[�v�[���@�\�폜�i���z���������ł͎g�p�����󋵂��܂����肦�Ȃ��̂Łj, 
	mLevel1MemoryPool(AMemoryWrapper::Malloc(kByteCount_MemoryPoolLevel1Size)), 
	mLevel2MemoryPool(AMemoryWrapper::Malloc(kByteCount_MemoryPoolLevel2Size))*/
{
	//gBase�ݒ�
	gBase = this;
	
	//�e�L�X�g�G���R�[�f�B���O���b�p�[������
	ATextEncodingWrapper::Init();
	
	//#693
	//#941 �K�v���ɐ�������悤�ɂ��� new APurgeManager();
	
	//�t�H���g���b�p�[������ win
	//AApplication::AApplication()�ֈړ��iLocalizedText���g�p���邽��) AFontWrapper::Init();
	
	mMainThreadID = AThreadWrapper::GetCurrentThreadID();
	
	_AInfo("ABase init completed",NULL);
}

/**
�f�X�g���N�^
*/
ABase::~ABase()
{
	gBase = NULL;
}

/**
����MainThread�����ǂ����𔻒肷��
*/
ABool	ABase::InMainThread() const
{
	return (mMainThreadID == AThreadWrapper::GetCurrentThreadID());
}

#pragma mark ===========================

#pragma mark ---���C�������C�x���g�L���[Post/Dequeue

/**
�L���[����C�x���g���o��
*/
ABool	ABase::DequeueFromMainInternalEventQueue( AInternalEventType& outType, AObjectID& outSenderObjectID, 
		ANumber& outNumber, AText& outText, AObjectIDArray& outObjectIDArray )
{
	if( gBase != NULL )
	{
		return gBase->mMainInternalEventQueue.Dequeue(outType,outSenderObjectID,outNumber,outText,outObjectIDArray);
	}
	return false;
}

/**
�L���[���̃C�x���g���ڐ��擾
*/
AItemCount	ABase::GetMainInternalEventQueueCount()
{
	if( gBase != NULL )
	{
		return gBase->mMainInternalEventQueue.GetQueueCount();
	}
	return false;
}

/**
�C�x���g���M
*/
void	ABase::PostToMainInternalEventQueue( const AInternalEventType& inType, const AObjectID inSenderObjectID,
		const ANumber& inNumber, const AText& inText, const AObjectIDArray& inObjectIDArray )
{
	if( gBase != NULL )
	{
		gBase->mMainInternalEventQueue.Post(inType,inSenderObjectID,inNumber,inText,inObjectIDArray);
	}
}

//#698
/**
�w��C�x���g���L���[����폜
*/
void	ABase::RemoveEventFromMainInternalEventQueue( const AInternalEventType inType, const AObjectID inSenderObjectID )
{
	if( gBase != NULL )
	{
		gBase->mMainInternalEventQueue.RemoveEvent(inType,inSenderObjectID);
	}
}

//#1056
/**
�L���[���Ɏw��^�C�v�̃C�x���g�����݂��邩�ǂ�����Ԃ�
*/
ABool	ABase::ExistEventInInternalEventQueue( const AInternalEventType inType )
{
	if( gBase != NULL )
	{
		return gBase->mMainInternalEventQueue.ExistEventInInternalEventQueue(inType);
	}
	return false;
}


#pragma mark ===========================

#pragma mark ---��Array�̎擾

const AText				kEmptyText;
const ATextArray		kEmptyTextArray;
const ABoolArray		kEmptyBoolArray;
const ANumberArray		kEmptyNumberArray;
const AColorArray		kEmptyColorArray;
const AObjectIDArray	kEmptyObjectIDArray;
const AHashNumberArray	kEmptyHashNumberArray;

/**
��Text�擾
*/
const AText&	GetEmptyText()
{
	return kEmptyText;
}

/**
��TextArray�擾
*/
const ATextArray&	GetEmptyTextArray()
{
	return kEmptyTextArray;
}

/**
��BoolArray�擾
*/
const ABoolArray&	GetEmptyBoolArray()
{
	return kEmptyBoolArray;
}

/**
��NumberArray�擾
*/
const ANumberArray&	GetEmptyNumberArray()
{
	return kEmptyNumberArray;
}

/**
��ColorArray�擾
*/
const AColorArray&	GetEmptyColorArray()
{
	return kEmptyColorArray;
}

/**
��ObjectIDArray�擾
*/
const AObjectIDArray&	GetEmptyObjectIDArray()
{
	return kEmptyObjectIDArray;
}

/**
��HashNumberArray�擾
*/
const AHashNumberArray&	GetEmptyHashNumberArray()
{
	return kEmptyHashNumberArray;
}

#pragma mark ===========================

#pragma mark ---ANumber���[�e�B���e�B
//#688

/**
�ŏ��l���擾
*/
ANumber	GetMinValue( const ANumber inNumber1, const ANumber inNumber2 )
{
	if( inNumber1 < inNumber2 )
	{
		return inNumber1;
	}
	else
	{
		return inNumber2;
	}
}

/**
�ő�l���擾
*/
ANumber	GetMaxValue( const ANumber inNumber1, const ANumber inNumber2 )
{
	if( inNumber1 > inNumber2 )
	{
		return inNumber1;
	}
	else
	{
		return inNumber2;
	}
}


#pragma mark ===========================

#pragma mark ---APoint���[�e�B���e�B

/**
Point������r
*/
void	OrderPoint( const APoint& inSelectTextPoint1, const APoint& inSelectTextPoint2, APoint& outStartTextPoint, APoint& outEndTextPoint ) 
{
	if( inSelectTextPoint1.y < inSelectTextPoint2.y ) 
	{
		outStartTextPoint = inSelectTextPoint1;
		outEndTextPoint = inSelectTextPoint2;
	}
	else if( inSelectTextPoint1.y == inSelectTextPoint2.y )
	{
		if( inSelectTextPoint1.x < inSelectTextPoint2.x )
		{
			outStartTextPoint = inSelectTextPoint1;
			outEndTextPoint = inSelectTextPoint2;
		}
		else
		{
			outStartTextPoint = inSelectTextPoint2;
			outEndTextPoint = inSelectTextPoint1;
		}
	}
	else
	{
		outStartTextPoint = inSelectTextPoint2;
		outEndTextPoint = inSelectTextPoint1;
	}
}

//R0108
/**
AImagePoint������r
*/
void	OrderImagePoint( const AImagePoint& inSelectTextPoint1, const AImagePoint& inSelectTextPoint2, AImagePoint& outStartTextPoint, AImagePoint& outEndTextPoint ) 
{
	if( inSelectTextPoint1.y < inSelectTextPoint2.y ) 
	{
		outStartTextPoint = inSelectTextPoint1;
		outEndTextPoint = inSelectTextPoint2;
	}
	else if( inSelectTextPoint1.y == inSelectTextPoint2.y )
	{
		if( inSelectTextPoint1.x < inSelectTextPoint2.x )
		{
			outStartTextPoint = inSelectTextPoint1;
			outEndTextPoint = inSelectTextPoint2;
		}
		else
		{
			outStartTextPoint = inSelectTextPoint2;
			outEndTextPoint = inSelectTextPoint1;
		}
	}
	else
	{
		outStartTextPoint = inSelectTextPoint2;
		outEndTextPoint = inSelectTextPoint1;
	}
}

//win 080618
/**
ATextPoint������r
*/
void	OrderTextPoint( const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2, 
		ATextPoint& outStartTextPoint, ATextPoint& outEndTextPoint ) 
{
	if( inSelectTextPoint1.y < inSelectTextPoint2.y ) 
	{
		outStartTextPoint = inSelectTextPoint1;
		outEndTextPoint = inSelectTextPoint2;
	}
	else if( inSelectTextPoint1.y == inSelectTextPoint2.y )
	{
		if( inSelectTextPoint1.x < inSelectTextPoint2.x )
		{
			outStartTextPoint = inSelectTextPoint1;
			outEndTextPoint = inSelectTextPoint2;
		}
		else
		{
			outStartTextPoint = inSelectTextPoint2;
			outEndTextPoint = inSelectTextPoint1;
		}
	}
	else
	{
		outStartTextPoint = inSelectTextPoint2;
		outEndTextPoint = inSelectTextPoint1;
	}
}

#pragma mark ===========================

#pragma mark ---ALocalRect���[�e�B���e�B
//#688

/**
�Q��LocalRect�̏d�Ȃ蕔�����擾
*/
ABool	GetIntersectRect( const ALocalRect& inRect1, const ALocalRect& inRect2, ALocalRect& outIntersectRect )
{
	outIntersectRect.left		= 0;
	outIntersectRect.top		= 0;
	outIntersectRect.right		= 0;
	outIntersectRect.bottom		= 0;
	//X�����̏d�Ȃ蕔�����v�Z
	ANumber	maxLeft = GetMaxValue(inRect1.left,inRect2.left);
	ANumber	minRight = GetMinValue(inRect1.right,inRect2.right);
	if( minRight <= maxLeft )
	{
		//�d�Ȃ��Ă��Ȃ�
		return false;
	}
	//Y�����̏d�Ȃ蕔�����v�Z
	ANumber	maxTop = GetMaxValue(inRect1.top,inRect2.top);
	ANumber	minBottom = GetMinValue(inRect1.bottom,inRect2.bottom);
	if( minBottom <= maxTop )
	{
		//�d�Ȃ��Ă��Ȃ�
		return false;
	}
	//����
	outIntersectRect.left		= maxLeft;
	outIntersectRect.top		= maxTop;
	outIntersectRect.right		= minRight;
	outIntersectRect.bottom		= minBottom;
	return true;
}

//#688
/**
LocalPoint��LoaclRect���ɂ��邩�ǂ����𔻒�
*/
ABool	IsPointInRect( const ALocalPoint inPoint, const ALocalRect& inRect )
{
	if( inPoint.x >= inRect.left && inPoint.x <= inRect.right && 
				inPoint.y >= inRect.top && inPoint.y <= inRect.bottom )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//#850
/**
�h���b�O�J�n�𔻒�i�ŏ��̃N���b�N���̃|�C���g����̍������傫���Ȃ�����true��Ԃ��j
*/
ABool	IsDragStart( const ALocalPoint& inCurrentPoint, const ALocalPoint& inOriginalPoint )
{
	if( abs(inCurrentPoint.x - inOriginalPoint.x) > 8 || abs(inCurrentPoint.y - inOriginalPoint.y) > 8 )
	{
		return true;
	}
	else
	{
		return false;
	}
}


