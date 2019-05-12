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

ADocument

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]ADocument_Text
#pragma mark ===========================
//�h�L�������g�̃N���X
#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
/**
�R���X�g���N�^
*/
ADocument::ADocument( AObjectArrayItem* inParent, const AObjectID inDocImpID ) : AObjectArrayItem(inParent), mDirty(false), mReadOnly(false)
,mPrintImp(GetObjectID())//#182
,mRetainCount(0)//#379
,mAlwaysNotDirty(false)//#567
,mFileSpecifiedAndScreened(false)
,mDocImpID(inDocImpID)//#1034
{
	//#1034
	//CDocImp��ADocument��DocumentID��ݒ�
	if( mDocImpID != kObjectID_Invalid )
	{
		AApplication::GetApplication().GetImp().GetDocImpByID(mDocImpID).SetDocumentID(GetObjectID());
	}
}
/**
�f�X�g���N�^
*/
ADocument::~ADocument()
{
	//CDocImp��� #1078
	if( mDocImpID != kObjectID_Invalid )
	{
		AApplication::GetApplication().GetImp().DeleteDocImp(mDocImpID);
	}
}

//#92
/**
AObjectArrayItem��AObjectArray����폜��������ɃR�[�������B
�i�f�X�g���N�^��GC���Ȃ̂ŁA�^�C�~���O����ł��Ȃ��B����āA�e��폜������DoDeleted()�ōs���ׂ��B�j
DoDeleted()��NVI_DocumentWillBeDeleted()�ɕύX�BDoDeleted()��mDocumentArrayMutex�ɂ��r�����ōs����̂Ńf�b�h���b�N�l�����K�v�ɂȂ�A�댯�B
*/
void	ADocument::NVI_DocumentWillBeDeleted()
{
	NVIDO_DocumentWillBeDeleted();
}

//#379
/**
RetainCount���f�N�������g��0���AView�Ȃ��ɂȂ�����������
*/
void	ADocument::NVI_Release()
{
	//RetainCount�f�N�������g
	mRetainCount--;
	
	//#379
	//View�������Ȃ�A���ARetainCount��0�Ȃ�h�L�������g�폜
	if( mViewIDArray.GetItemCount() == 0 && mRetainCount <= 0 )
	{
		AApplication::GetApplication().NVI_DeleteDocument(GetObjectID());
	}
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�h�L�������g���ݒ�^�擾

/**
�h�L�������g�t�@�C����ݒ�i�X�N���[�j���O�����j
*/
void	ADocument::NVI_SpecifyFile( const AFileAcc& inFileAcc )
{
	mFileAcc = inFileAcc;
	//ReadOnly����
	NVI_SetReadOnly(mFileAcc.IsReadOnly());
	NVIDO_SpecifyFile(mFileAcc);
	
	//�X�N���[�j���O�����t���OON
	mFileSpecifiedAndScreened = true;
	
	/*#379
	for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
	{
		AApplication::GetApplication().NVI_GetWindowByID(mWindowIDArray.Get(i)).NVI_UpdateTitleBar();
	}
	*/
	AApplication::GetApplication().NVI_UpdateTitleBar();
	
	//#1422
	//�t�@�C���ݒ莞�����iFile Presenter�o�^�j
	if( mDocImpID != kObjectID_Invalid )
	{
		AApplication::GetApplication().GetImp().GetDocImpByID(mDocImpID).FileSpecified(inFileAcc);
	}
}

/**
�h�L�������g�t�@�C����ݒ�i���X�N���[�j���O�j
*/
void	ADocument::NVI_SpecifyFileUnscreened( const AFileAcc& inFileAcc )
{
	mFileAcc = inFileAcc;
}

/**
�h�L�������g�t�@�C�����擾
*/
ABool	ADocument::NVI_GetFile( AFileAcc& outFile ) const
{
	if( NVI_IsFileSpecified() == false )   return false;
	outFile = mFileAcc;
	return true;
}

//#723
/**
�h�L�������g�t�@�C���̃p�X���擾
*/
void	ADocument::NVI_GetFilePath( AText& outFilePath ) const
{
	outFilePath.DeleteAll();
	//�t�@�C���p�X�擾
	AFileAcc	file;
	if( NVI_GetFile(file) == true )
	{
		file.GetPath(outFilePath);
	}
}

//#465
/**
�e�t�H���_���擾
*/
ABool	ADocument::NVI_GetParentFolder( AFileAcc& outFolder ) const
{
	if( NVI_IsFileSpecified() == false )   return false;
	outFolder.SpecifyParent(mFileAcc);
	return true;
}

/**
Dirty������ݒ�
*/
void	ADocument::NVI_SetDirty( const ABool inDirty )
{
	//#81
	//�h���N���X����
	NVIDO_SetDirty(inDirty);
	
	//���Dirty�ɂ��Ȃ��ݒ�Ȃ牽�����Ȃ� #567
	if( mAlwaysNotDirty == true )   return;
	//
	ABool	svDirty = mDirty;
	if( inDirty )
	{
		if( NVI_IsReadOnly() == false )
		{
			mDirty = true;
		}
		//globals->mHookManager->TouchHook();
	}
	else
	{
		mDirty = false;
	}
	if( svDirty != mDirty )
	{
		/*#379
		for( AIndex i = 0; i < mWindowIDArray.GetItemCount(); i++ )
		{
			AApplication::GetApplication().NVI_GetWindowByID(mWindowIDArray.Get(i)).NVI_UpdateTitleBar();
		}
		*/
		AApplication::GetApplication().NVI_UpdateTitleBar();
	}
}

/**
Dirty�������擾
*/
ABool	ADocument::NVI_IsDirty() const
{
	return mDirty;
}

//#567
/**
���Dirty�ɂ��Ȃ��ݒ�
*/
void	ADocument::NVI_SetAlwaysNotDirty()
{
	mAlwaysNotDirty = true;
}

/**
�ǂݍ��ݐ�p������ݒ�
*/
void	ADocument::NVI_SetReadOnly( const ABool inReadOnly )
{
	mReadOnly = inReadOnly;
	//�^�C�g���o�[�X�V #0
	AApplication::GetApplication().NVI_UpdateTitleBar();
}

/*#699 NVIDO_�ɕύX
ABool	ADocument::NVI_IsReadOnly() const
{
	return mReadOnly;
}
*/

/**
�h�L�������g�t�@�C���ݒ�ς݂��ǂ������擾
*/
ABool	ADocument::NVI_IsFileSpecified() const
{
	return mFileAcc.IsSpecified();
}

/*#182
void	ADocument::NVI_UpdateProperty()
{
	NVIDO_UpdateProperty();
}
*/

#pragma mark ===========================

#pragma mark ---View

//#92
/**
ViewID�o�^
*/
void	ADocument::NVI_RegisterView( const AViewID inViewID )
{
	mViewIDArray.Add(inViewID);
	//#379
	NVIDO_ViewRegistered(inViewID);
}

/**
ViewID�o�^����
*/
void	ADocument::NVI_UnregisterView( const AViewID inViewID )
{
	AIndex	index = mViewIDArray.Find(inViewID);
	if( index == kIndex_Invalid )   { _ACError("viewID not found",this); return; }
	
	//#893
	//�h���N���X����
	NVIDO_ViewUnregistered(inViewID);
	
	//ViewID array����폜
	mViewIDArray.Delete1(index);
	
	//#379
	//View�������Ȃ�A���ARetainCount��0�Ȃ�h�L�������g�폜
	if( mViewIDArray.GetItemCount() == 0 && mRetainCount <= 0 )
	{
		AApplication::GetApplication().NVI_DeleteDocument(GetObjectID());
	}
}

/**
Document��\������View���őO�ʂɎ����Ă���
*/
void	ADocument::NVI_RevealDocumentWithAView() const
{
	if( mViewIDArray.GetItemCount() == 0 )   return;
	
	//Document�ɓo�^����Ă���ŏ���View��Reveal����
	AApplication::GetApplication().NVI_GetViewByID(mViewIDArray.Get(0)).NVI_RevealView(true,false);//#492
}

#pragma mark ===========================

#pragma mark ---���

//#182
/**
PageSetup�_�C�A���O��\��
*/
void	ADocument::NVI_PrintPageSetup( const AWindowID inWindowID )
{
	mPrintImp.PageSetup(AApplication::GetApplication().NVI_GetWindowByID(inWindowID).NVI_GetWindowRef());
}

//#182
/**
����_�C�A���O��\��
*/
void	ADocument::NVI_Print( const AWindowID inWindowID, const AText& inJobTitle )
{
	mPrintImp.Print(AApplication::GetApplication().NVI_GetWindowByID(inWindowID).NVI_GetWindowRef(),inJobTitle);
}



