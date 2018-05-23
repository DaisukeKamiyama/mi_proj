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

AProjectFolderLoader
#402

*/

#include "stdafx.h"

#include "AProjectFolderLoader.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]AProjectFolderLoader
#pragma mark ===========================
//�}���`�t�@�C���������s

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AProjectFolderLoader::AProjectFolderLoader( AObjectArrayItem* inParent ) : AThread(inParent), mDontDisplayInvisible(false)
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

/**
�X���b�h���C�����[�`��
*/
void	AProjectFolderLoader::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AProjectFolderLoader::NVIDO_ThreadMain started.",this);
	try
	{
		//3�b�ԃE�G�C�g
		GetApp().NVI_SleepWithTimer(3);
		//���ʑS�폜
		mResult.DeleteAll();
		mFolders.DeleteAll();
		//�t�H���_���[�h���ċA�I�Ɏ��s
		for( AIndex i = 0; i < mRequestArray_ProjectFolder.GetItemCount(); i++ )
		{
			mResult.AddNewObject();
			AFileAcc	folder;
			folder.Specify(mRequestArray_ProjectFolder.GetTextConst(i));
			LoadFolderRecursive(i,folder,0);
		}
	}
	catch(...)
	{
		_ACError("AProjectFolderLoader::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	AObjectIDArray	objectIDArray;
	objectIDArray.Add(GetObjectID());
	ABase::PostToMainInternalEventQueue(kInternalEvent_ProjectFolderLoaderResult,GetObjectID(),0,GetEmptyText(),objectIDArray);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AProjectFolderLoader::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---�X���b�h�N��

/**
�X���b�h�N���i�v���W�F�N�g�P�j
*/
void	AProjectFolderLoader::SPNVI_Run( const AFileAcc& inProjectFolder, const ABool inDontDisplayInvisible )
{
	if( NVI_IsThreadWorking() == true )   {_ACError("",this);return;}
	
	//�v���f�[�^���R�s�[
	mRequestArray_ProjectFolder.DeleteAll();
	AText	path;
	inProjectFolder.GetPath(path);
	mRequestArray_ProjectFolder.Add(path);
	mDontDisplayInvisible = inDontDisplayInvisible;
	
	//�X���b�h�N��
	NVI_Run();
}

/**
�X���b�h�N���i�v���W�F�N�g�����j
*/
void	AProjectFolderLoader::SPNVI_Run( const AObjectArray<AFileAcc>& inProjectFolderArray, const ABool inDontDisplayInvisible )
{
	if( NVI_IsThreadWorking() == true )   {_ACError("",this);return;}
	
	//�v���f�[�^���R�s�[
	mRequestArray_ProjectFolder.DeleteAll();
	for( AIndex i = 0; i < inProjectFolderArray.GetItemCount(); i++ )
	{
		AText	projectFolderPath;
		inProjectFolderArray.GetObjectConst(i).GetPath(projectFolderPath);
		mRequestArray_ProjectFolder.Add(projectFolderPath);
	}
	mDontDisplayInvisible = inDontDisplayInvisible;
	
	//�X���b�h�N��
	NVI_Run();
}

/**
�t�H���_�ǂݍ���
mSameProject_Pathname�̓��e�쐬�i�v���W�F�N�g�t�H���_���ċA�I�Ɍ������đS�t�@�C���o�^�j
mSameProject_Pathname�̎w��Index�̔z��͂��̊֐����ł͍폜����Ȃ��i�폜�����ɒǉ����Ă����j�̂ŁA�֐��R�[�����ō폜�K�v
*/
void	AProjectFolderLoader::LoadFolderRecursive( const AIndex inRequestIndex, const AFileAcc& inFolder, const AIndex inDepth )
{
	//���[�h�����t�H���_���L�^�iSCM�p�j
	mFolders.GetObject(mFolders.AddNewObject()).CopyFrom(inFolder);
	
	//#892 �t�@�C�����X�g�K�w��
	//���ʂɃt�H���_�̃p�X��ǉ�
	if( inDepth > 0 )
	{
		AText	folderpath;
		inFolder.GetPath(folderpath);
		folderpath.AddLastPathDelimiter();
		folderpath.ConvertToCanonicalComp();//#1087
		mResult.GetObject(inRequestIndex).Add(folderpath);
	}
	
	//�t�H���_���t�@�C���擾
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	for( AIndex index = 0; index < children.GetItemCount(); index++ )
	{
		//�X���b�habort���ꂽ��I��
		if( NVM_IsAborted() == true )
		{
			break;
		}
		
		//
		AFileAcc	child;
		child.CopyFrom(children.GetObjectConst(index));
		//�s���t�@�C����\�����Ȃ��ݒ�Ȃ�mSameProject_Pathname�ɒǉ����Ȃ�
		//#533 �s���t�@�C���͏�ɔ�\���ɂ���i�����ɕ��ׂ����邽�߁j if( mDontDisplayInvisible == true )
		{
			if( child.IsInvisible() == true )   continue;
		}
		//
		if( child.IsFolder() == true && child.IsLink() == false )//#0 �V���{���b�N�����N�E�G�C���A�X�ɂ�閳�����[�v�h�~
		{
			LoadFolderRecursive(inRequestIndex,child,inDepth+1);
		}
		else
		{
			AText	path;
			child.GetPath(path);
			path.ConvertToCanonicalComp();//#1087
			mResult.GetObject(inRequestIndex).Add(path);
		}
	}
}

#pragma mark ===========================

#pragma mark ---���ʎ擾

/**
�v���f�[�^�擾
*/
void	AProjectFolderLoader::SPI_GetRequestedData( ATextArray& outProjectFolderArray ) const
{
	outProjectFolderArray.SetFromTextArray(mRequestArray_ProjectFolder);
}

/**
���ʎ擾
*/
void	AProjectFolderLoader::SPI_GetResult( const AIndex inRequestIndex, ATextArray& outResult ) const
{
	outResult.SetFromTextArray(mResult.GetObjectConst(inRequestIndex));
}

/**
���[�h�����t�H���_���X�g�擾
*/
void	AProjectFolderLoader::SPI_GetLoadedFolders( AObjectArray<AFileAcc>& outFolders ) const
{
	for( AIndex i = 0; i < mFolders.GetItemCount(); i++ )
	{
		outFolders.GetObject(outFolders.AddNewObject()).CopyFrom(mFolders.GetObjectConst(i));
	}
}



