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

ABlockStartDataList
#695

*/

#include "stdafx.h"

#include "ABlockStartDataList.h"

/**
BlockStartData�ǉ�
*/
AUniqID	ABlockStartDataList::Add( const AUniqID inPreviousID, const AIndex inVariableIndex, const AItemCount inIndentCount )
{
	//�z��֒ǉ�
	AIndex	index = mUniqIDArray.AddItem();
	AUniqID	addedUniqID = mUniqIDArray.Get(index);
	mVariableIndexArray.Add(inVariableIndex);
	mIndentCountArray.Add(inIndentCount);
	mNextUniqIDArray.Add(kUniqID_Invalid);
	
	//���X�g�ڑ�
	//inPreviousID��kUniqID_Invalid�łȂ��ꍇ�i�����X�g�擪�ȊO�j�A�O��Next�Ɏ��I�u�W�F�N�g��ݒ肷��B
	if( inPreviousID != kUniqID_Invalid )
	{
		//�O��Identifier��NextUniqID�ɁA�ǉ�����Identifier��UniqID��ݒ肷��B
		AIndex	prevIndex = mUniqIDArray.Find(inPreviousID);
		if( mNextUniqIDArray.Get(prevIndex) == kUniqID_Invalid )
		{
			mNextUniqIDArray.Set(prevIndex,addedUniqID);
		}
		else
		{
			//prev��UniqID�ݒ�ς݁i�Ō���ǉ��łȂ��j�ꍇ��throw
			_ACThrow("not last object",this);
		}
	}
	return addedUniqID;
}

/**
BlockStartData�폜
���X�g�ɘA�Ȃ鎯�ʎq��S�č폜����B
*/
void	ABlockStartDataList::DeleteIdentifiers( const AUniqID inFirstUniqID )
{
	//�܂����̎��ʎq�ȍ~���폜�i�ċA�j
	AIndex	index0 = mUniqIDArray.Find(inFirstUniqID);
	AUniqID	nextUniqID = mNextUniqIDArray.Get(index0);
	if( nextUniqID == inFirstUniqID )   {_ACError("uniq id error (loop)",NULL);return;}
	if( nextUniqID != kUniqID_Invalid )
	{
		DeleteIdentifiers(nextUniqID);
	}
	//���g���폜
	//index�͂����ōēx��������K�v������B�i�ċA�R�[���Ŏ��ȍ~���폜�ς݂Ȃ̂ŁA�ŏ��Ƃ�Index���ς���Ă���B�j
	AIndex	index = mUniqIDArray.Find(inFirstUniqID);
	mUniqIDArray.Delete1Item(index);
	mVariableIndexArray.Delete1(index);
	mIndentCountArray.Delete1(index);
	mNextUniqIDArray.Delete1(index);
}

/**
�S�폜
*/
void	ABlockStartDataList::DeleteAll()
{
	mUniqIDArray.DeleteAll();
	mVariableIndexArray.DeleteAll();
	mIndentCountArray.DeleteAll();
	mNextUniqIDArray.DeleteAll();
}

/**
VariableIndex�擾
*/
AIndex	ABlockStartDataList::GetVariableIndex( const AUniqID inUniqID ) const
{
	AIndex	index = mUniqIDArray.Find(inUniqID);
	return mVariableIndexArray.Get(index);
}

/**
IndentCount�擾
*/
AItemCount	ABlockStartDataList::GetIndentCount( const AUniqID inUniqID ) const
{
	AIndex	index = mUniqIDArray.Find(inUniqID);
	return mIndentCountArray.Get(index);
}

/**
NextUniqID�擾
*/
AUniqID	ABlockStartDataList::GetNextUniqID( const AUniqID inUniqID ) const
{
	AIndex	index = mUniqIDArray.Find(inUniqID);
	return mNextUniqIDArray.Get(index);
}

