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

AFileAcc
�t�@�C���i�t�H���_�j�A�N�Z�X�N���X

*/

#include "stdafx.h"

//#include "../miedit.h"
#include "../AbsBase/ABase.h"
#if IMPLEMENTATION_FOR_MACOSX
#include <sys/xattr.h>
#include <sys/stat.h>
#endif
#include "../MacWrapper/AFileWrapper.h"
#include "../AbsBase/Cocoa.h"
#import <Cocoa/Cocoa.h>

#if IMPLEMENTATION_FOR_WINDOWS
#include "io.h"
#include <stdio.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>
#include <direct.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <Shlwapi.h>
#endif

#pragma mark ===========================
#pragma mark [�N���X]AFileAcc
#pragma mark ===========================
//�t�@�C���A�N�Z�X�̂��߂̃N���X
//�u�t�@�C�����̂ɑ΂���Q�Ɓv�I�ȃN���X�ł���A�t�@�C���A�N�Z�X�̓��\�b�h���Ŋ�������B�t�@�C���̏�Ԃ͕ێ����Ȃ��B

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

AFileAcc::AFileAcc( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent), mSpecified(false)//, mFSRefSpecified(false)
{
}

AFileAcc::AFileAcc( const AText& inPath ) : AObjectArrayItem(NULL), mSpecified(false)//, mFSRefSpecified(false)
{
	Specify(inPath);
}

AFileAcc::AFileAcc( const AFileAcc& inFile ) : AObjectArrayItem(NULL), mSpecified(false)//, mFSRefSpecified(false)
{
	CopyFrom(inFile);
}

AFileAcc& AFileAcc::operator = ( const AFileAcc& inFile )
{
	CopyFrom(inFile);
	return (*this);
}

void AFileAcc::CopyFrom( const AFileAcc& inSrcFile )
{
	mPath.DeleteAll();
	mPath.AddText(inSrcFile.mPath);
	mSpecified = inSrcFile.IsSpecified();
	//mFSRefSpecified = inSrcFile.mFSRefSpecified;
	//mFSRef = inSrcFile.mFSRef;
}


#pragma mark ===========================

#pragma mark --- �t�@�C���ʒu�w��

/*
�t�@�C���p�X�ɂ���

�yMacOSX�̏ꍇ�z
�@mPath���ێ�����p�X��UNIX�p�X�`���A�G���R�[�h��UTF8(CanonicalComp)�Ƃ���B
�������ASpecify����UTF8(CanonicalComp)�ł��邱�Ƃ̃`�F�b�N��ϊ��͂����A�󂯎�����܂܂̃R�[�h�ŕۑ�����B
�܂��A�t�@�C�����̃X���b�V���^�R�����́A�R�����ŕ\������B
GetPath()/Specify()�̃f�t�H���g(kFilePathType_Default)�͂��̌`�����g�p����B
OS API�֐��R�[�����ɁAHFSPlusDecomp(OS��)��CanonicalComp(AFileAcc��)��ϊ�����B
�AkFilePathType_1
GetPath()/Specify()�Ŏg�p�ł���p�X�`���BGetPath()/Specify()���Ň@�̌`���Ƒ��ݕϊ�����B
/volumename/folder1/folder2/test.txt
�̌`���ŁA�G���R�[�h��UTF8(CanonicalComp)
�t�@�C�����̃X���b�V���^�R�����́A�R�����ŕ\������B
�BkFilePathType_2
GetPath()/Specify()�Ŏg�p�ł���p�X�`���BGetPath()/Specify()���Ň@�̌`���Ƒ��ݕϊ�����B
:volumename:folder1:folder2:test.txt
�̌`���ŁA�G���R�[�h��UTF8(CanonicalComp)
�t�@�C�����̃X���b�V���^�R�����́A�X���b�V���ŕ\������B

*/

//�t�@�C���̏ꏊ���w�肷��
// �������ASpecify�ł́A�p�X������̐ݒ�݂̂��s���A�t�@�C�������݂��邩�ǂ����̃`�F�b�N���͍s��Ȃ��B
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::Specify( const AText& inPath, const AFilePathType inFilePathType )
{
	mPath.DeleteAll();
	switch(inFilePathType)
	{
		//UNIX�p�X�`��
	  case kFilePathType_Default:
		{
			//mPath�ւ��̂܂ܐݒ�
			//mPath��CannonicalComp��z�肵�Ă��邪�A����decomp�ł������Ƃ��Ă��AAFileAcc���ł͓��ɖ��Ȃ��B
			//�X���b�V���^�R�����́AinPath���Ŋ��ɃR�����ŕ\������Ă���ׂ��B�i���̃��\�b�h���ł͕ϊ��̂��悤���Ȃ��j
			mPath.AddText(inPath);
			break;
		}
		//  /volumename/folder1/folder2/test.txt�̌`��(kFilePathType_1)
		//  :volumename:folder1:folder2:test.txt�̌`��(kFilePathType_2)
	  case kFilePathType_1:
	  case kFilePathType_2:
		{
			//�p�X��؂蕶���̐ݒ�
			AUChar	delimiter = GetPathDelimiter(inFilePathType);
			/*win kUChar_Slash;
			if( inFilePathType == kFilePathType_2 )
			{
				delimiter = kUChar_Colon;
			}*/
			//�X���b�V�����f���~�^�Ƃ��ăe�L�X�g�z��ɕ���
			ATextArray	pathArray;
			inPath.Explode(delimiter,pathArray);
			
			//�����e�L�X�g��2��菬�����Ƃ����X���b�V�����݂��Ȃ��Ƃ��͉����������^�[��
			if( pathArray.GetItemCount() < 2 )   return;
			
			//�X���b�V���Ŏn�܂�̂ŁA�ŏ���text�͕K����̂͂�
			AText	text;
			pathArray.Get(0,text);
			if( text.GetItemCount() != 0 )   return;
			
			//�{�����[������
			FSRef	volumeFSRef;
			AText	volumeNameText;
			pathArray.Get(1,volumeNameText);
			ABool	volumeFound = false;
			FSVolumeRefNum	vRefNum;
			for( ItemCount i = 1; ; i++ )
			{
				HFSUniStr255	volName;
				OSStatus	status = ::FSGetVolumeInfo(kFSInvalidVolumeRefNum,i,&vRefNum,kFSVolInfoNone,NULL,&volName,&volumeFSRef);
				if( status != noErr )   break;
				AText	text;
				SetTextByHFSUniStr255(volName,text);
				if( text.Compare(volumeNameText) == true )
				{
					volumeFound = true;
					break;
				}
			}
			//�{�����[����������Ȃ��Ƃ��͉����������^�[��
			if( volumeFound == false )   return;
			
			//�t�H���_�K�w�����֌���
			FSRef	currentFSRef = volumeFSRef;
			for( AIndex i = 2; i < pathArray.GetItemCount(); i++ )
			{
				AText	text;
				pathArray.Get(i,text);
				//..�Ȃ�e�t�H���_�ֈړ�
				if( text.Compare("..") == true )
				{
					if( ::FSCompareFSRefs(&currentFSRef,&volumeFSRef) == noErr )   return;
					//�e�t�H���_��FSRef�擾
					::FSGetCatalogInfo(&currentFSRef,kFSCatInfoNone,NULL,NULL,NULL,&currentFSRef);
				}
				//.�͉������Ȃ�
				else if( text.Compare(".") == true )
				{
					//��������
				}
				//�q�t�H���_�^�t�@�C��
				else
				{
					//B0445
					if( inFilePathType == kFilePathType_1 )
					{
						text.ReplaceChar(':','/');
					}
					//�q�̖��O�e�L�X�g���AUTF16(HFSPlusDecomp)�֕ϊ����AFSRef���擾
					text.ConvertFromUTF8ToUTF16();
					ATextEncodingWrapper::ConvertUTF16ToHFSPlusDecomp(text);
					FSRef	newFSRef;
					{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
						AStTextPtr	textptr(text,0,text.GetItemCount());
						OSStatus status = ::FSMakeFSRefUnicode(&currentFSRef,textptr.GetByteCount()/sizeof(UniChar),reinterpret_cast<const UniChar*>(textptr.GetPtr()),kTextEncodingUnknown,&newFSRef);
						if( status != noErr )   return;
					}
					//�G�C���A�X����
					Boolean	isFolder,isAlias;
					::FSResolveAliasFile(&newFSRef,true,&isFolder,&isAlias);
					currentFSRef = newFSRef;
				}
			}
			
			//FSRef��Specify
			SpecifyByFSRef(currentFSRef);
			break;
		}
	}
	mSpecified = true;
	//mFSRefSpecified = false;
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::Specify( const AText& inPath, const AFilePathType inFilePathType )
{
	mPath.DeleteAll();
	mPath.AddText(inPath);
	mSpecified = true;
}
#endif

#if IMPLEMENTATION_FOR_MACOSX
//B0389 OK �����ꂩ��FilePathType�Ńt�@�C����������܂Ŏ���
void	AFileAcc::SpecifyWithAnyFilePathType( const AText& inPath )
{
	Specify(inPath,kFilePathType_Default);
	if( Exist() == true )   return;
	Specify(inPath,kFilePathType_1);
	if( Exist() == true )   return;
	AText	path;
	path.SetCstring(":");
	path.AddText(inPath);
	Specify(path,kFilePathType_2);
	if( Exist() == true )   return;
	Specify(inPath,kFilePathType_2);
	if( Exist() == true )   return;
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::SpecifyWithAnyFilePathType( const AText& inPath )
{
	Specify(inPath);
}
#endif

/**
����t�@�C���̐e�t�H���_���w�肷��
*/
ABool	AFileAcc::SpecifyParent( const AFileAcc& inBaseFile ) //#427
{
	ABool	result = true;
	//�x�[�X�t�@�C���I�u�W�F�N�g�̃p�X���R�s�[����
	inBaseFile.GetPath(mPath);
	if( mPath.GetLength() == 0 )   return false;//#427
	
	//1��̃t�H���_�������悤�ɕ����񑀍�
	AIndex pos = mPath.GetLength();
	if( pos >= 1 )   pos--;//�Ō��/�͖����������̂�-2����
	if( pos >= 1 )   pos--;
	if( mPath.GoBackToChar(pos,GetPathDelimiter()) == false )
	{
		//#427 /��������Ȃ������ꍇ�̓G���[�ɂ���
		result = false;
	}
	pos++;//�����/�̒���ɗ���
	mPath.DeleteAfter(pos);
	mSpecified = true;
	//mFSRefSpecified = false;
	return result;//#427
}

//����t�@�C���̓��t�H���_�̕ʂ̃t�@�C���i�܂��̓t�H���_�₻�̉��̃t�@�C���j���w�肷��
void	AFileAcc::SpecifySister( const AFileAcc& inBaseFile, AConstCharPtr inPath, const AFilePathType inFilePathType )
{
	AFileAcc	parent;
	parent.SpecifyParent(inBaseFile);
	SpecifyChild(parent,inPath,inFilePathType);
}
void	AFileAcc::SpecifySister( const AFileAcc& inBaseFile, const AText& inPath, const AFilePathType inFilePathType )
{
	AFileAcc	parent;
	parent.SpecifyParent(inBaseFile);
	SpecifyChild(parent,inPath,inFilePathType);
}

//����t�H���_�̎q�t�@�C���i�܂��̓t�H���_�₻�̉��̃t�@�C���j���w�肷��
void	AFileAcc::SpecifyChild( const AFileAcc& inBaseFile, AConstCharPtr inPath, const AFilePathType inFilePathType )
{
	AText	path;
	path.AddCstring(inPath);
	SpecifyChild(inBaseFile,path,inFilePathType);
}
/*#695
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::SpecifyChild( const AFileAcc& inBaseFile, const AText& inPath, const AFilePathType inFilePathType )
{
	//path�ɁAinFilePathType�̌`���Ńt�@�C���p�X���č\������
	AText	path;
	
	//�x�[�X�t�@�C���I�u�W�F�N�g�̃p�X���擾
	inBaseFile.GetPath(path,inFilePathType);
	//�p�X�̍Ō�̕�����/�łȂ���΁A/��t������
	if( path.GetLastChar() != GetPathDelimiter(inFilePathType) )
	{
		path.Add(GetPathDelimiter(inFilePathType));
	}
	//inPath��t��
	path.AddText(inPath);
	
	//
	Specify(path,inFilePathType);
	
	mSpecified = true;
	//mFSRefSpecified = false;
}
//�Ή��K�v child��../�������Ă���ꍇ
#endif
#if IMPLEMENTATION_FOR_WINDOWS
*/
void	AFileAcc::SpecifyChild( const AFileAcc& inBaseFile, const AText& inPath, const AFilePathType inFilePathType )
{
	//path�ɁAinFilePathType�̌`���Ńt�@�C���p�X���č\������
	AText	path;
	
	//�x�[�X�t�@�C���I�u�W�F�N�g�̃p�X���擾
	inBaseFile.GetPath(path,inFilePathType);
	//�p�X�̍Ō�̕�����/�łȂ���΁A/��t������
	if( path.GetLastChar() != GetPathDelimiter(inFilePathType) )
	{
		path.Add(GetPathDelimiter(inFilePathType));
	}
	
	//path��inPath����������
	
	//�f���~�^�擾
	AUChar	delimiter = GetPathDelimiter(inFilePathType);
	//���΃p�X�擾 
	AText	relativePath;
	relativePath.SetText(inPath);
	//�p�X��؂��delimiter�œ���
	//�iWindows�̏ꍇ�ɁA�X���b�V�����o�b�N�X���b�V���ɕϊ�����j
	if( delimiter != kUChar_Slash )
	{
		relativePath.ReplaceChar(kUChar_Slash,delimiter);//#695 kUChar_Backslash);
	}
	
	//relativePath����������ʒu��catpos�ɋ��߂�B�ŏ��͍Ō������n�߂�B
	AIndex	catpos = path.GetLength();
	//relativePath�̍ŏ�����"./"��"../"��T���Ă���
	AIndex	pos = 0;
	AItemCount	len = relativePath.GetLength();
	//�ŏ��̕������o�b�N�X���b�V����������폜����i��̂ق��Ńo�b�N�X���b�V�������Ă���̂ŁA�d�����Ȃ��悤�ɂ��邽�߁j
	if( len > 0 )
	{
		if( relativePath.Get(0) == /*#695 kUChar_Backslash*/ delimiter )
		{
			relativePath.Delete1(0);
		}
	}
	//
	while( pos < len )
	{
		//".\"�������牽�������ɐi�߂�
		if( pos+1 < len )
		{
			if( relativePath.Get(pos) == kUChar_Period && relativePath.Get(pos+1) == /*#695 kUChar_Backslash*/ delimiter )
			{
				pos += 2;
				continue;
			}
		}
		//"..\"��������catpos���P�O��/�̒���ɂ���B
		if( pos+2 < len )
		{
			if( relativePath.Get(pos) == kUChar_Period && relativePath.Get(pos+1) == kUChar_Period && 
						relativePath.Get(pos+2) == /*#695 kUChar_Backslash*/ delimiter )
			{
				pos += 3;
				catpos -= 2;
				path.GoBackToChar(catpos,/*#695 kUChar_Backslash*/ delimiter);
				catpos++;
				continue;
			}
		}
		//".\"��"..\"�ȊO��������catpos�ȍ~���폜���āAinRelativePath�̌��݈ʒu�ȍ~����������
		path.DeleteAfter(catpos);
		relativePath.Delete(0,pos);
		path.AddText(relativePath);
		break;
	}
	
	//
	Specify(path,inFilePathType);
	
	mSpecified = true;
}
//#695 #endif

//����t�H���_�̎q�t�@�C�����w�肷��
void	AFileAcc::SpecifyChildFile( const AFileAcc& inBaseFile, const AText& inFilename ) 
{
	AText	filename;
	filename.SetText(inFilename);
#if IMPLEMENTATION_FOR_MACOSX
	//�t�@�C�������̃X���b�V���͑S�ăR�����ɕϊ�����
	filename.ReplaceChar(kUChar_Slash,kUChar_Colon);
#endif
	SpecifyChild(inBaseFile,filename,kFilePathType_Default);
}

//inFolder���ŁA���̃t�@�C���̃t�@�C�����Əd�����Ȃ��t�@�C�����w�肷��B
void	AFileAcc::SpecifyUniqChildFile( const AFileAcc& inFolder, AConstCharPtr inFilename )
{
	AText	filename;
	filename.AddCstring(inFilename);
	SpecifyUniqChildFile(inFolder,filename);
}
void	AFileAcc::SpecifyUniqChildFile( const AFileAcc& inFolder, const AText& inFilename )
{
	SpecifyChildFile(inFolder,inFilename);
	
	if( Exist() == false )
	{
		return;
	}
	AText	originalPath;
	GetPath(originalPath);
	for( ANumber i = 2; i < 1000000; i++ )
	{
		AText	number;
		number.SetFormattedCstring("-%d",i);//#0 �X�y�[�X����n�C�t�H���֕ύX
		AText	path;
		path.SetText(originalPath);
		//R0137 path.AddText(number);
		path.InsertText(path.GetFirstSuffixStartPos(),number);
		
		Specify(path);
		if( Exist() == false )
		{
			return;
		}
	}
	_ACThrow("",this);
}

void	AFileAcc::SpecifyChangeSuffix( const AFileAcc& inBaseFile, const AText& inSuffix )
{
	AText	path;
	inBaseFile.GetPath(path);
	for( AIndex i = path.GetItemCount()-1; i >= 0; i-- )
	{
		AUChar	ch = path.Get(i);
		if( ch == kUChar_Period )
		{
			path.DeleteAfter(i);
			path.AddText(inSuffix);
			break;
		}
	}
	Specify(path);
}

void	AFileAcc::Unspecify()
{
	mPath.DeleteAll();
	mSpecified = false;
	//mFSRefSpecified = false;
}

#pragma mark ===========================

#pragma mark --- �p�X�擾

#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::GetPath( AText& outPath, const AFilePathType inFilePathType ) const
{
	outPath.DeleteAll();
	switch(inFilePathType)
	{
		//UNIX�p�X�`��
	  case kFilePathType_Default:
		{
			outPath.AddText(mPath);
			//#530 GetPath()�̏o�͌`���𓝈�
			//�Ō�̃X���b�V���͎�菜��
			if( outPath.GetItemCount() > 0 )
			{
				if( outPath.Get(outPath.GetItemCount()-1) == kUChar_Slash )
				{
					outPath.DeleteAfter(outPath.GetItemCount()-1);
				}
			}
			break;
		}
		//  /volumename/folder1/folder2/test.txt�̌`��(kFilePathType_1)
		//  :volumename:folder1:folder2:test.txt�̌`��(kFilePathType_2)
	  case kFilePathType_1:
	  case kFilePathType_2:
		{
			FSRef	fsref;
			if( GetFSRef(fsref) == false )   return;
			//�p�X��؂蕶���̐ݒ�
			AUChar	pathdelimiter = GetPathDelimiter(inFilePathType);
			/*win kUChar_Slash;
			if( inFilePathType == kFilePathType_2 )
			{
				pathdelimiter = kUChar_Colon;
			}*/
			//�t�H���_�K�w����֌���
			HFSUniStr255	name;
			FSCatalogInfo	catalogInfo;
			FSRef	parent;
			while(true)
			{
				//�t�@�C���^�t�H���_���ƁA�e�t�H���_���擾
				::FSGetCatalogInfo(&fsref,kFSCatInfoParentDirID,&catalogInfo,&name,NULL,&parent);
				//�p�X������ɒǉ�
				AText	nametext;
				SetTextByHFSUniStr255(name,nametext);
				//B0445
				if( inFilePathType == kFilePathType_1 )
				{
					nametext.ReplaceChar('/',':');
				}
				
				outPath.InsertText(0,nametext);
				outPath.Insert1(0,pathdelimiter);
				//���[�g�i�{�����[���j�ɓ��B������I��
				if( catalogInfo.parentDirID == fsRtParID )   break;
				fsref = parent;
			}
			break;
		}
	}
}

//#1087
/**
�t�@�C�������̕�������t�@�C���V�X�e���p�̕�����ɕϊ�
*/
void	AFileAcc::GetNormalizedPathString( AText& ioPath ) 
{
	@try 
	{
		if( ioPath.GetItemCount() > 0 )
		{
			//�t�@�C���V�X�e���p������擾
			AStCreateNSStringFromAText	str(ioPath);
			ioPath.SetCstring([str.GetNSString() fileSystemRepresentation]);
		}
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::GetNormalizedPathString() @catch",NULL);
	}
}

//B0000
void	AFileAcc::GetNormalizedPath( AText& outPath ) const
{
	//HFS+ decomp�`���ɕϊ��imPath�ɂ�decomp�`���Acomp�`�������ݒ肳�ꂤ��̂ŁB�Ō�̃X���b�V����GetPath()�o�͎��_�ō폜�ς݁B�j
	outPath.DeleteAll();
	//�p�X�擾
	GetPath(outPath);
	//�t�@�C���V�X�e���p������ɕϊ�
	AFileAcc::GetNormalizedPathString(outPath);
	
	/*
	outPath.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToHFSPlusDecomp(outPath);
	outPath.ConvertToUTF8FromUTF16();
	*/
	/*#695
	outPath.DeleteAll();
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return;
	UInt8	path[4096];
	path[0] = 0;
	::FSRefMakePath(&fsref,path,4096);
	outPath.SetCstring(reinterpret_cast<char*>(path));
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::GetPath( AText& outPath, const AFilePathType inFilePathType ) const
{
	GetNormalizedPath(outPath);
}
void	AFileAcc::GetNormalizedPath( AText& outPath ) const
{
	outPath.SetText(mPath);
	//�Ō�̃o�b�N�X���b�V���͎�菜��
	if( outPath.GetItemCount() > 0 )
	{
		if( outPath.Get(outPath.GetItemCount()-1) == kUChar_Backslash )
		{
			outPath.DeleteAfter(outPath.GetItemCount()-1);
		}
	}
	//���m�F
	//GetFullPathName?
}
#endif

//���΃p�X�擾
void	AFileAcc::GetPartialPath( const AFileAcc& inBaseFile, AText& outPath, const AFilePathType inFilePathType ) const
{
	outPath.DeleteAll();
	if( inBaseFile.IsSpecified() == false )
	{
		GetNormalizedPath(outPath);
		return;
	}
	//�p�X��؂蕶���̐ݒ�
	AUChar	pathdelimiter = GetPathDelimiter(inFilePathType);
	//�^�[�Q�b�g�̃p�X�擾
	AText	target;
	GetNormalizedPath(target);
	//#614 �t�H���_����̑��΃p�X���������擾�ł��Ȃ����΍�
	if( IsFolder() == true )
	{
		target.AddPathDelimiter(pathdelimiter);
	}
	//�x�[�X�̃p�X�擾
	AText	base;
	inBaseFile.GetNormalizedPath(base);
	//#614 �t�H���_����̑��΃p�X���������擾�ł��Ȃ����΍�
	if( inBaseFile.IsFolder() == true )
	{
		base.AddPathDelimiter(pathdelimiter);
	}
	
	/*win kUChar_Slash;
	if( inFilePathType == kFilePathType_2 )
	{
		pathdelimiter = kUChar_Colon;
	}*/
	
	//���΃p�X�v�Z
	//�P�D�^�[�Q�b�g�ƃx�[�X�̕������ŏ��ɈقȂ�ʒu���擾
	AIndex	pos = 0;
	for( ; pos < target.GetItemCount(); pos++ )
	{
		if( pos >= base.GetItemCount() )   break;
		if( target.Get(pos) != base.Get(pos) )   break;
	}
	//�Q�D�P�Ŏ擾�����ʒu����߂��čŏ��̃p�X�f���~�^�̈ʒu���擾
	for( pos--; pos > 0; pos-- )
	{
		if( target.Get(pos) == pathdelimiter )   break;
	}
	pos++;
	//�R�D�x�[�X����Q�̃p�X�f���~�^����̃p�X�f���~�^�̐����擾
	AItemCount	basedepth = 0;
	for( AIndex basepos = pos; basepos < base.GetItemCount(); basepos++ )
	{
		if( base.Get(basepos) == pathdelimiter )
		{
			basedepth++;
		}
	}
	//�S�D�R�̐�����../��ǉ�
	for( AIndex i = 0; i < basedepth; i++ )
	{
		outPath.AddCstring("..");
		outPath.Add(pathdelimiter);
	}
	//�T�D�^�[�Q�b�g����Q�̃p�X�f���~�^����̕������ǉ�
	AText	text;
	target.GetText(pos,target.GetItemCount()-pos,text);
	outPath.AddText(text);
}

//#539
/**
URL�擾
*/
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::GetURL( AText& outURL ) const
{
	//#1425
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	NSURL*	url = [NSURL fileURLWithPath:pathstr.GetNSString()];
	if( url != nil )
	{
		outURL.SetFromNSString(url.path);
		return true;
	}
	else
	{
		return false;
	}
	
	/*#1425
	FSRef	ref;
	if( GetFSRef(ref) == false )   return false;
	
	ABool	result = false;
	CFURLRef	url = ::CFURLCreateFromFSRef(NULL,&ref);
	if( url != NULL )
	{
		CFStringRef	str = ::CFURLGetString(url);
		if( str != NULL )
		{
			outURL.SetFromCFString(str);
			result = true;
			::CFRelease(str);
		}
		//#1012 ���ꂪ�����SIGABRT��������̂ŃR�����g�A�E�g::CFRelease(url);
	}
	return result;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::GetURL( AText& outURL ) const
{
	AText	path;
	GetNormalizedPath(path);
	AStCreateWcharStringFromAText	pathstr(path);
	wchar_t	url[4096];
	DWORD	len = 4096;
	HRESULT	res = ::UrlCreateFromPath(pathstr.GetPtr(),url,&len,NULL);
	outURL.SetFromWcharString(url,len);
	return (res==S_OK);
}
#endif

#pragma mark ===========================

#pragma mark --- �t�@�C�����擾

//�t�@�C�������擾
void	AFileAcc::GetFilename( AText& outName, const ABool inConvertForDisplay ) const
{
	outName.DeleteAll();
	/*B0000 ������
	ATextArray	textArray;
	mPath.Explode(GetPathDelimiter(),textArray);
	if( textArray.GetItemCount() == 0 )   return;
	textArray.Get(textArray.GetItemCount()-1,outName);
	if( outName.GetItemCount() == 0 )
	{
		//folder
		if( textArray.GetItemCount() < 2 )   return;
		textArray.Get(textArray.GetItemCount()-2,outName);
	}
	*/
	mPath.GetFilename(outName);
	
	/*#1040
	outName.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(outName);
	outName.ConvertToUTF8FromUTF16();
	*/
	outName.ConvertToCanonicalComp();
	
	if( inConvertForDisplay == true )
	{
#if IMPLEMENTATION_FOR_MACOSX
		//UnixPath�ł�/��:�ɕϊ�����Ă���BMacOX��API�ł�/���g���̂ŁA:��/�ɕϊ��B
		outName.ReplaceChar(kUChar_Colon,kUChar_Slash);
#endif
	}
}

//�t�@�C�������擾 R0006
//UNIX�R�}���h�ɓn���Đ��������삷��`���ŕԂ�
void	AFileAcc::GetRawFilename( AText& outName ) const
{
	outName.DeleteAll();
	AText	path;
	GetNormalizedPath(path);
	path.GetFilename(outName);
}

#pragma mark ===========================

#pragma mark --- ��r

//�t�@�C����r
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::Compare( const AFileAcc& inFileAcc ) const
{
	/*#695
	//�t�@�C�������݂���ꍇ�͓����t�@�C�����ǂ������r
	FSRef	fsref1, fsref2;
	ABool	exist = false;
	exist = GetFSRef(fsref1);
	if( exist == true )
	{
		exist = inFileAcc.GetFSRef(fsref2);
	}
	if( exist == true )
	{
		return (::FSCompareFSRefs(&fsref1,&fsref2) == noErr);
	}
	*/
	//�t�@�C�������݂��Ȃ��ꍇ�̓p�X�e�L�X�g���r
	AText	path1, path2;
	//�p�X�擾
	inFileAcc.GetPath(path1);
	//�Ō�̃p�X�f���~�^���폜
	if( path1.GetLength() > 0 )
	{
		if( path1.Get(path1.GetLength()-1) == GetPathDelimiter() )
		{
			path1.Delete(path1.GetLength()-1,1);
		}
	}
	/*#1040
	path1.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(path1);
	*/
	path1.ConvertToCanonicalComp();
	
	//�p�X�擾
	GetPath(path2);
	//�Ō�̃p�X�f���~�^���폜
	if( path2.GetLength() > 0 )
	{
		if( path2.Get(path2.GetLength()-1) == GetPathDelimiter() )
		{
			path2.Delete(path2.GetLength()-1,1);
		}
	}
	/*#1040
	path2.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(path2);
	*/
	path2.ConvertToCanonicalComp();
	return path1.Compare(path2);
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::Compare( const AFileAcc& inFileAcc ) const
{
	//�p�X�e�L�X�g���r
	AText	path1, path2;
	//�p�X�擾
	inFileAcc.GetPath(path1);
	//�Ō�̃p�X�f���~�^���폜
	if( path1.GetLength() > 0 )
	{
		if( path1.Get(path1.GetLength()-1) == GetPathDelimiter() )
		{
			path1.Delete(path1.GetLength()-1,1);
		}
	}
	path1.ChangeCaseLowerFast();
	
	//�p�X�擾
	GetPath(path2);
	//�Ō�̃p�X�f���~�^���폜
	if( path2.GetLength() > 0 )
	{
		if( path2.Get(path2.GetLength()-1) == GetPathDelimiter() )
		{
			path2.Delete(path2.GetLength()-1,1);
		}
	}
	path2.ChangeCaseLowerFast();
	
	return path1.Compare(path2);
}
#endif

//�t�@�C������r
ABool	AFileAcc::CompareFilename( const AText& inFilename ) const
{
	AText	filename1;
	GetFilename(filename1);
	/*#1040
	filename1.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(filename1);
	*/
	filename1.ConvertToCanonicalComp();
	
	AText	filename2;
	filename2.SetText(inFilename);
	/*#1040
	filename2.ConvertFromUTF8ToUTF16();
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(filename2);
	*/
	filename2.ConvertToCanonicalComp();
	
	return filename1.Compare(filename2);
}

//�g���q��r
ABool	AFileAcc::CompareSuffix( const AText& inSuffix ) const
{
	AText	filename;
	GetFilename(filename);
	AText	suffix;
	filename.GetSuffix(suffix);
	return suffix.Compare(inSuffix);
}

#pragma mark ===========================

#pragma mark --- �t�@�C�����擾

//�t�@�C�����݃`�F�b�N
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::Exist() const
{
	ABool	result = false;
	@try 
	{
		//�p�X�擾
		AText	path;
		GetPath(path);//#1090 �e�X�g�����Ƃ���AfileExistsAtPath��comp/decomp�֌W�Ȃ����삷��悤�Ȃ̂ŁA�������̂���normalize���Ȃ��B
		AStCreateNSStringFromAText	pathstr(path);
		//�t�@�C�����݃`�F�b�N
		result = ([[NSFileManager defaultManager] fileExistsAtPath:pathstr.GetNSString()]==YES);
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::Exist() @catch",this);
	}
	return result;
	/*#1034
	FSRef	ref;
	return GetFSRef(ref);
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::Exist() const
{
/*	if( mPath.GetItemCount() < 2 )   return false;
	if( mPath.Get(1) != ':' )   return false;
	//�h���C�u���݃`�F�b�N�i���e�X�g�E�h���C�u�񑶍݃_�C�A���O���\�������s�����Č��B�Č�������e�X�g�j
	DWORD	bitmask = 0;
	AUChar	ch = mPath.Get(0);
	if( ch >= 'A' && ch <= 'Z' )
	{
		bitmask = (1<<(ch-'A'));
	}
	else if( ch >= 'a' && ch <= 'z' )
	{
		bitmask = (1<<(ch-'a'));
	}
	else return false;
	if( (::GetLogicalDrives()&bitmask) == 0 )   return false;*/
	
	if( mPath.GetItemCount() == 0 )   return false;
	
	//�Ō��\������Ɛ���ɔ��肳��Ȃ�
	AText	path(mPath);
	if( path.Get(path.GetItemCount()-1) == GetPathDelimiter() )
	{
		path.DeleteAfter(path.GetItemCount()-1);
	}
	
	//���݃`�F�b�N
	AStCreateWcharStringFromAText	pathstr(path);
	return (_waccess_s(pathstr.GetPtr(),0)==0);
}
#endif

//�t�H���_���H
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::IsFolder() const
{
	ABool	result = false;
	@try 
	{
		//�p�X�擾
		AText	path;
		GetPath(path);//#1090 �e�X�g�����Ƃ���AfileExistsAtPath��comp/decomp�֌W�Ȃ����삷��悤�Ȃ̂ŁA�������̂���normalize���Ȃ��B
		AStCreateNSStringFromAText	pathstr(path);
		//�t�@�C�����݃`�F�b�N���t�H���_���ǂ�������
		BOOL	isDir = NO;
		if( [[NSFileManager defaultManager] fileExistsAtPath:pathstr.GetNSString() isDirectory:&isDir] == YES )
		{
			result = (isDir==YES);
		}
		else
		{
			//�t�@�C���񑶍݂Ȃ�false
			result = false;
		}
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::IsFolder() @catch",this);
	}
	return result;
	/*#1034
	FSCatalogInfo	catalog;
	FSRefParam	param;
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	param.ref = &fsref;
	param.whichInfo = kFSCatInfoNodeFlags;
	param.catInfo = &catalog;
	param.spec = NULL;
	param.parentRef = NULL;
	param.outName = NULL;
	::PBGetCatalogInfoSync(&param);
	return ((param.catInfo->nodeFlags&kFSNodeIsDirectoryMask)!=0);
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::IsFolder() const
{
	if( mPath.GetItemCount() == 0 )   return false;
	
	//�Ō��\������Ɛ���ɔ��肳��Ȃ�
	AText	path(mPath);
	if( path.Get(path.GetItemCount()-1) == GetPathDelimiter() )
	{
		return true;
	}
	
	//�t�H���_���ǂ����̔���
	AStCreateWcharStringFromAText	pathstr(path);
	struct __stat64	st = {0};
	if( _wstat64(pathstr.GetPtr(),&st) == 0 )//�yOS�݊����zWin95/98�ł͎g���Ȃ��BWinNT�ȍ~
	{
		return ((st.st_mode&_S_IFDIR)!=0);
	}
	else
	{
		return false;
	}
}
#endif

//�ǂݍ��ݐ�p�t�@�C�����H
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::IsReadOnly() const
{
	/*readonly�̃t�@�C���ł�0�ɂȂ��Ă��܂� FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	FSCatalogInfo	catinfo;
  ::FSGetCatalogInfo(&fsref,kFSCatInfoNodeFlags,&catinfo,NULL,NULL,NULL);
	return ((catinfo.nodeFlags&kFSNodeLockedMask)!=0);*/
	
	//�ǂݍ��ݐ�p���̔���
	@try
	{
		//�p�X�擾
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		
		//�������݉ہi�p�[�~�V�����j�`�F�b�N
		if( [[NSFileManager defaultManager] isWritableFileAtPath:pathstr.GetNSString()] == NO )
		{
			//�������݃p�[�~�V�����Ȃ����true��Ԃ�
			return true;
		}
		
		/*fileHandleForWritingAtPath��O_SHLOCK���ꂽ�t�@�C���ł����Ă�file handle��Ԃ��̂Ŏg���Ȃ��B
		NSFileHandle* fh = [NSFileHandle fileHandleForWritingAtPath:pathstr.GetNSString()];
		if( fh == nil )
		{
			return true;
		}
		*/
		
		/*#1243
		//�r�����b�N���Ƃ��Ă݂�
		AStCreateCstringFromAText	str(path);
		int	fd = ::open((const char*)(str.GetPtr()),O_NONBLOCK|O_EXLOCK);
		if( fd != -1 )
		{
			::close(fd);
		}
		else
		{
			//�r�����b�N�Ƃ�Ȃ����true��Ԃ�
			return true;
		}
		*/
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::IsReadOnly() @catch",this);
	}
	return false;
	
	/*#1034
	FSSpec	fsspec;
	if( GetFSSpec(fsspec) == false )   return false;
	HFileParam		fp;
	fp.ioNamePtr = fsspec.name;
	fp.ioVRefNum = fsspec.vRefNum;
	fp.ioFDirIndex = 0;
	fp.ioDirID = fsspec.parID;
	::PBHGetFInfoSync((HParmBlkPtr)&fp);
	if( (fp.ioFlAttrib&1) != 0 )   return true;
	
	ABool	result = false;
	//write permission check
	short	fRefNum;
	//OSErr	err = UFSSpec::OpenFileWriteLock(mFileSpec,&fRefNum);
	OSErr err = ::FSpOpenDF(&fsspec,fsWrPerm,&fRefNum);
	if( err != noErr || fRefNum == 0 )
	{
		result = true;
	}
	else 
	{ 
	  ::FSClose(fRefNum);
	}
	//B0019 ���\�[�X�t�H�[�N���������݉\���ǂ��������ׂ�悤�ɕύX
	short rRefNum = ::FSpOpenResFile(&fsspec,fsRdPerm);//�܂��ǂݎ��p�[�~�V�����ŊJ��
	if( rRefNum != -1 )//���\�[�X�t�H�[�N�������Ȃ�-1
	{
	  ::CloseResFile(rRefNum);
		
		rRefNum = ::FSpOpenResFile(&fsspec,fsWrPerm);
		if( rRefNum == -1 )
		{
			result =true;
		}
		else
		{
		  ::CloseResFile(rRefNum);
		}
	}
	return result;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::IsReadOnly() const
{
	if( mPath.GetItemCount() == 0 )   return false;
	
	//�Ō��\������Ɛ���ɔ��肳��Ȃ�
	AText	path(mPath);
	if( path.Get(path.GetItemCount()-1) == GetPathDelimiter() )
	{
		path.DeleteAfter(path.GetItemCount()-1);
	}
	
	//�������݌����`�F�b�N
	AStCreateWcharStringFromAText	pathstr(path);
	return (_waccess_s(pathstr.GetPtr(),2)!=0);//2:�������݌����`�F�b�N
}
#endif

//Root���H
ABool	AFileAcc::IsRoot() const
{
	return (mPath.GetItemCount() <= 1);
}

//B0313
//Invisible�t�@�C���E�t�H���_���H
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::IsInvisible() const
{
	AText	filename;
	GetFilename(filename);
	if( filename.GetItemCount() > 0 )
	{
		if( filename.Get(0) == '.' )   return true;
	}
	return false;
	/*#1034 #1090
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	GetFSRef(fsref);
	::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
	return (((((FileInfo*)(&(catinfo.finderInfo[0])))->finderFlags)&kIsInvisible)!=0);
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::IsInvisible() const
{
	AText	filename;
	GetFilename(filename);
	if( filename.GetItemCount() > 0 )
	{
		if( filename.Get(0) == '.' )   return true;
	}
	//���m�F ���̑���invisible��������
	return false;
}
#endif

//R0000 security
//��3.0.0b6���_�łǂ�������g�p����Ă��Ȃ����ߖ��e�X�g
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::IsExecutable() const
{
	@try
	{
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		
		if( [[NSFileManager defaultManager] isExecutableFileAtPath:pathstr.GetNSString()] == NO )
		{
			return true;
		}
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::IsExecutable() @catch",this);
	}
	return false;
	
	/*#1034
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	GetFSRef(fsref);
	::FSGetCatalogInfo(&fsref,kFSCatInfoPermissions,&catinfo,NULL,NULL,NULL);
	UInt16	mode = ((FSPermissionInfo*)(&(catinfo.permissions[0])))->mode;
	return (((mode&S_IXUSR)!=0)||((mode&S_IXGRP)!=0)||((mode&S_IXOTH)!=0));
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::IsExecutable() const
{
	if( mPath.GetItemCount() == 0 )   return false;
	
	//�Ō��\������Ɛ���ɔ��肳��Ȃ�
	AText	path(mPath);
	if( path.Get(path.GetItemCount()-1) == GetPathDelimiter() )
	{
		path.DeleteAfter(path.GetItemCount()-1);
	}
	
	//�t�H���_���ǂ����̔���
	AStCreateWcharStringFromAText	pathstr(path);
	struct __stat64	st = {0};
	if( _wstat64(pathstr.GetPtr(),&st) == 0 )//�yOS�݊����zWin95/98�ł͎g���Ȃ��BWinNT�ȍ~
	{
		return ((st.st_mode&_S_IEXEC)!=0);
	}
	else
	{
		return false;
	}
	//�����e�X�g
}
#endif

#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::GetFileAttribute( AFileAttribute& outFileAttribute ) const
{
	//#1425
	//�p�X�擾
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	//attribute�擾
	NSError *anError = nil;
	NSDictionary*	dic = [[NSFileManager defaultManager] attributesOfItemAtPath:pathstr.GetNSString() error:&anError];
	if( dic != nil )
	{
		//creator/type�擾�icreator/type���ݒ�Ȃ�0���擾�����j
		outFileAttribute.creator = [dic fileHFSCreatorCode];
		outFileAttribute.type = [dic fileHFSTypeCode];
		return true;
	}
	return false;
	
	/*#1425
	outFileAttribute.creator = 0;
	outFileAttribute.type = 0;
	//�N���G�[�^�^�^�C�v
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
	outFileAttribute.creator = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileCreator;
	outFileAttribute.type = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileType;
	return true;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::GetFileAttribute( AFileAttribute& outFileAttribute ) const
{
	//���ʁA�_�~�[�����̂�
	outFileAttribute.tmp = 0;
	return true;
}
#endif

#if IMPLEMENTATION_FOR_MACOSX
//win
ABool	AFileAcc::SetFileAttribute( const AFileAttribute& inFileAttribute ) const
{
	//#1425
	//�p�X�擾
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	//creator/type��NSDictionary�쐬
	NSDictionary*	dic = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithUnsignedLong:inFileAttribute.creator], NSFileHFSCreatorCode,
		[NSNumber numberWithUnsignedLong:inFileAttribute.type], NSFileHFSTypeCode,
		nil
	];
	//attribute�ݒ�
	NSError *anError = nil;
	if( [[NSFileManager defaultManager] setAttributes:dic ofItemAtPath:pathstr.GetNSString() error:&anError] == YES )
	{
		return true;
	}
	else
	{
		return false;
	}
	
	/*#1425
	//�N���G�[�^�^�^�C�v��������
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
	((FileInfo*)(&(catinfo.finderInfo[0])))->fileCreator = inFileAttribute.creator;
	((FileInfo*)(&(catinfo.finderInfo[0])))->fileType = inFileAttribute.type;
	::FSSetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo);
	return true;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::SetFileAttribute( const AFileAttribute& inFileAttribute ) const
{
	//���ʁA�����Ȃ�
	return true;
}
#endif

//R0208
//�t�@�C���T�C�Y�擾
#if IMPLEMENTATION_FOR_MACOSX
AItemCount	AFileAcc::GetFileSize() const
{
	return AFileWrapper::GetFileSize(mPath);
	/*
	SInt64	forksize = 0;
	SInt16	forkref = 0;
	try
	{
		FSRef	ref;
		if( GetFSRef(ref) == false )   throw 0;
		HFSUniStr255	forkName;
		::FSGetDataForkName(&forkName);
		OSErr	err;
		err = ::FSOpenFork(&ref,forkName.length,forkName.unicode,fsRdPerm,&forkref);
		if( err != noErr )   throw 0;
		err = ::FSGetForkSize(forkref,&forksize);
		::FSCloseFork(forkref);
	}
	catch(...)
	{
		if( forkref != 0 )   ::FSCloseFork(forkref);
	}
	return forksize;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
AItemCount	AFileAcc::GetFileSize() const
{
	AItemCount	len = 0;
	FILE*	fp = NULL;
	try
	{
		AStCreateWcharStringFromAText	pathstr(mPath);
		if( _wfopen_s(&fp,pathstr.GetPtr(),L"rb") != 0 )   throw 0;//_wfopen_s��Win95����Ή�
		len = _filelength(_fileno(fp));
		fclose(fp);
	}
	catch(...)
	{
		if( fp != NULL )   fclose(fp);
	}
	return len;
	//�����e�X�g
}
#endif

#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::GetLastModifiedDataTime( ADateTime& outDateTime ) const
{
	//#1425
	//�p�X�擾
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	//attribute�擾
	NSError *anError = nil;
	NSDictionary*	dic = [[NSFileManager defaultManager] attributesOfItemAtPath:pathstr.GetNSString() error:&anError];
	if( dic != nil )
	{
		//�t�@�C���ύX�����擾
		NSDate*	modificationDate = [dic fileModificationDate];
		if( modificationDate != nil )
		{
			//ADateTime(CFAbsoluteTime)�֕ϊ�
			outDateTime = ::CFDateGetAbsoluteTime((CFDateRef)modificationDate);
			return true;
		}
	}
	return false;
	
	/*#1425
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	OSErr	err = ::FSGetCatalogInfo(&fsref,kFSCatInfoContentMod,&catinfo,NULL,NULL,NULL);
	if( err != noErr )   return false;//#653
	*#380
	LocalDateTime	local;
	::ConvertUTCToLocalDateTime(&(catinfo.contentModDate),&local);
	outDateTime = local.lowSeconds;
	*
	::UCConvertUTCDateTimeToCFAbsoluteTime(&(catinfo.contentModDate),&outDateTime);//#380
	return true;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::GetLastModifiedDataTime( ADateTime& outDateTime ) const
{
	if( mPath.GetItemCount() == 0 )   return false;
	
	//�Ō��\������Ɛ���ɔ��肳��Ȃ�
	AText	path(mPath);
	if( path.Get(path.GetItemCount()-1) == GetPathDelimiter() )
	{
		path.DeleteAfter(path.GetItemCount()-1);
	}
	
	//
	AStCreateWcharStringFromAText	pathstr(path);
	struct __stat64	st = {0};
	int err = _wstat64(pathstr.GetPtr(),&st);//�yOS�݊����zWin95/98�ł͎g���Ȃ��BWinNT�ȍ~
	outDateTime = st.st_mtime;
	return (err==0);
}
#endif

//R0137
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::IsBundleFolder() const
{
	if( IsFolder() == false )   return false;
	
	ABool	result = false;
	@try 
	{
		//�p�X�擾
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		//�p�b�P�[�W���ǂ����̔���
		result = ([[NSWorkspace sharedWorkspace] isFilePackageAtPath:pathstr.GetNSString()]==YES);
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::IsBundleFolder() @catch",this);
	}
	return result;
	/*#1034
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	
	LSItemInfoRecord	info;
	::LSCopyItemInfoForRef(&fsref,kLSRequestAllFlags,&info);
	if( (info.flags & kLSItemInfoIsPackage ) != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::IsBundleFolder() const
{
	return false;
}
#endif

/**
�w��t�H���_�̎q�t�@�C�����ǂ������擾
*/
ABool	AFileAcc::IsChildOfFolder( const AFileAcc& inFolder ) const
{
	//�p�X�擾
	AText	path;
	GetPath(path);
	//�t�H���_�p�X�擾
	AText	folderPath;
	inFolder.GetPath(folderPath);
	//�t�H���_�z���̃t�@�C�����ǂ����𔻒�
	if( folderPath.GetItemCount() < path.GetItemCount() )
	{
		if( folderPath.Compare(path,0,folderPath.GetItemCount()) == true )
		{
			return true;
		}
	}
	return false;
}

#pragma mark ===========================

#pragma mark --- �t�H���_���t�@�C���擾

//�t�H���_���t�@�C���擾
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::GetChildren( AObjectArray<AFileAcc>& outChilds ) const
{
	outChilds.DeleteAll();
	//#1034
	@try 
	{
		//�p�X�擾
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		//�q�t�@�C���E�t�H���_���X�g�擾
		NSArray*	contents_raw = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:pathstr.GetNSString() error:nil];//������K�v�H
		if( contents_raw != nil )
		{
			//���O���Ƀ\�[�g #1245
			NSArray*  contents = [contents_raw sortedArrayUsingComparator:^NSComparisonResult(id obj1, id obj2) {
					NSString*	str1 = (NSString*)obj1;
					NSString*	str2 = (NSString*)obj2;
					return [str1 compare:str2];
			}];//������K�v�H
			
			//�q�t�@�C���E�t�H���_���Ƃ̃��[�v
			AItemCount	count = [contents count];
			for( AIndex i = 0; i < count; i++ )
			{
				//�q�̃t�@�C�����擾
				NSString*	str = [contents objectAtIndex:i];
				AText	childText;
				ACocoa::SetTextFromNSString(str,childText);
				//���ʊi�[
				AIndex	newIndex = outChilds.AddNewObject();
				outChilds.GetObject(newIndex).SpecifyChild(path,childText);
			}
		}
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::GetChildren() @catch",this);
	}
	/*#1034
	OSErr	err;
	try
	{
		FSRef	ref;
		if( GetFSRef(ref) == false )   throw 0;
		FSCatalogBulkParam	param;
		err = ::FSOpenIterator(&ref,kFSIterateFlat,&(param.iterator));
		if( err != noErr )   throw 0;
		param.container = &ref;
		param.catalogInfo = NULL;
		param.maximumItems = 1;
		param.whichInfo = kFSCatInfoNone;
		FSRef	childfsref;
		param.refs = &childfsref;
		param.names = NULL;
		param.specs = NULL;
		while(true)
		{
			err = ::PBGetCatalogInfoBulkSync(&param);
			if( err == errFSNoMoreItems )   break;
			if( err != noErr )   throw 0;
			AIndex	newIndex = outChilds.AddNewObject();
			outChilds.GetObject(newIndex).SpecifyByFSRef(childfsref);
		}
		::FSCloseIterator(param.iterator);
	}
	catch(...)
	{
	}
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::GetChildren( AObjectArray<AFileAcc>& outChilds ) const
{
	outChilds.DeleteAll();
	AText	p;
	p.SetText(mPath);
	if( p.GetLastChar() != '\\' )   p.Add('\\');
	p.AddCstring("*.*");
	AStCreateWcharStringFromAText	pathstr(p);
	struct _wfinddata_t finddata;
	long	h = ::_wfindfirst(pathstr.GetPtr(),&finddata);///�yOS�݊����zWin95/98�ł͎g���Ȃ��BWinNT�ȍ~
	if( h != -1 )
	{
		while( true )
		{
			AText	filename;
			filename.SetFromWcharString(finddata.name,sizeof(finddata.name)/sizeof(finddata.name[0]));
			if( filename.Compare(".") == false && filename.Compare("..") == false )
			{
				AText	path;
				path.SetText(mPath);
				if( path.GetLastChar() != '\\' )   path.Add('\\');
				path.AddText(filename);
				AIndex	newIndex = outChilds.AddNewObject();
				outChilds.GetObject(newIndex).Specify(path);
			}
			if( ::_wfindnext(h,&finddata) != 0 )   break;
		}
		::_findclose(h);
	}
}
#endif

#pragma mark ===========================

#pragma mark --- �t�@�C���ǂݏ���

//�ǂݍ���
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::ReadTo( AText &outText/*#1034, ABool inResourceFork*/ ) const
{
	__block	ABool	result = false;//#1422
	outText.DeleteAll();
	
	//File Coordinator���� #1422
	NSFileCoordinator*	fileCoordinator = [[NSFileCoordinator alloc] initWithFilePresenter:nil];
	//File Coordinator�ɂăt�@�C���ǂݍ��� #1422
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	NSURL*	url = [NSURL fileURLWithPath:pathstr.GetNSString() isDirectory:NO];
	NSError *anError = nil;
	[fileCoordinator coordinateReadingItemAtURL:url options:0 error:&anError byAccessor:^(NSURL *newURL)
		{
			//
			NSFileHandle* fh = nil;
			@try 
			{
				//�p�X�擾
				NSString*	newPath = newURL.path;//#1422 File Coordinator�Ŏ擾����path�֏������ނ悤�ɂ���B
				//�t�@�C���n���h���擾
				fh = [NSFileHandle fileHandleForReadingAtPath:newPath];//#1422
				if( fh != nil )
				{
					//NSData�ɓǂݍ���
					NSData* data = [fh readDataToEndOfFile];
					if( data != nil )
					{
						//outText�֊i�[
						AByteCount	len = [data length];
						if( len > 0 )
						{
							outText.Reserve(0,len);
							outText.SetFromTextPtr((const AConstUCharPtr)[data bytes],len);
							result = true;
						}
					}
				}
			}
			@catch(NSException *theException)
			{
				_ACError("AFileAcc::ReadTo() @catch",this);
			}
			//�t�@�C���n���h���N���[�Y
			if( fh != nil )
			{
				[fh closeFile];
			}
	}];
	//File Coordinator��� #1422
	[fileCoordinator release];
	return result;
	
	/*#1034
	SInt16	forkref = 0;
	try
	{
		FSRef	ref;
		if( GetFSRef(ref) == false )   throw 0;
		HFSUniStr255	forkName;
		if( not inResourceFork )
		{
			::FSGetDataForkName(&forkName);
		}
		else
		{
			::FSGetResourceForkName(&forkName);
		}
		OSErr	err;
		err = ::FSOpenFork(&ref,forkName.length,forkName.unicode,fsRdPerm,&forkref);
		if( err != noErr )   throw 0;
		SInt64	forksize = 0;
		err = ::FSGetForkSize(forkref,&forksize);
		outText.Reserve(0,forksize);
		ByteCount	actualCount = 0;
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			AStTextPtr	textptr(outText,0,outText.GetItemCount());
			err = ::FSReadFork(forkref,fsFromStart,0,textptr.GetByteCount(),textptr.GetPtr(),&actualCount);
		}
		if( actualCount < outText.GetLength() )
		{
			_ACError("file read actual count is less than file size",this);
			outText.Delete(actualCount,outText.GetLength()-actualCount);//�� �������̎��ۂ����������ꍇ�Alock���itextptr�g�p���j�Ȃ̂�Delete�͕s��
		}
		if( err != noErr )   throw 0;
		::FSCloseFork(forkref);
	}
	catch(...)
	{
		outText.DeleteAll();
		if( forkref != 0 )   ::FSCloseFork(forkref);
		result = false;
	}
	*/
	return result;
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::ReadTo( AText &outText, ABool inResourceFork ) const
{
	ABool	result = true;
	outText.DeleteAll();
	
	FILE*	fp = NULL;
	try
	{
		AStCreateWcharStringFromAText	pathstr(mPath);
		if( _wfopen_s(&fp,pathstr.GetPtr(),L"rb") != 0 )   throw 0;//_wfopen_s��Win95����Ή�
		AItemCount	len = _filelength(_fileno(fp));
		outText.Reserve(0,len);
		size_t	readsize = 0;
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			AStTextPtr	textptr(outText,0,outText.GetItemCount());
			readsize = fread(textptr.GetPtr(),sizeof(char),textptr.GetByteCount(),fp);
		}
		if( readsize < outText.GetItemCount() )
		{
			_ACError("file read actual count is less than file size",this);
			outText.Delete(readsize,outText.GetItemCount()-readsize);
		}
		fclose(fp);
	}
	catch(...)
	{
		outText.DeleteAll();
		if( fp != NULL )   fclose(fp);
		result = false;
	}
	return result;
}
#endif

#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::WriteFile( const AText& inText, AFileError& outError ) const
{
	__block ABool	result = false;//#1422
	outError = kFileError_General;
	
	//File Coordinator���� #1422
	NSFileCoordinator*	fileCoordinator = [[NSFileCoordinator alloc] initWithFilePresenter:nil];
	//File Coordinator�ɂăt�@�C���������� #1422
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	NSURL*	url = [NSURL fileURLWithPath:pathstr.GetNSString() isDirectory:NO];
	NSError *anError = nil;
	[fileCoordinator coordinateWritingItemAtURL:url options:NSFileCoordinatorWritingForReplacing error:&anError byAccessor:^(NSURL *newURL)
		{
			//
			NSFileHandle* fh;
			@try 
			{
				//�p�X�擾
				NSString*	newPath = newURL.path;//#1422 File Coordinator�Ŏ擾����path�֏������ނ悤�ɂ���B
				//�t�@�C���n���h���擾
				fh = [NSFileHandle fileHandleForWritingAtPath:newPath];//#1422
				if( fh != nil )
				{
					//NSData�쐬
					AStTextPtr	textptr(inText,0,inText.GetItemCount());
					NSData*	data = [NSData dataWithBytes:textptr.GetPtr() length:textptr.GetByteCount()];
					if( data != nil )
					{
						//�t�@�C���ɏ�������
						[fh writeData:data];
						//�T�C�Y�ݒ�
						[fh truncateFileAtOffset:textptr.GetByteCount()];
						//����OK
						result = true;
					}
				}
			}
			@catch(NSException *theException)
			{
				//_AError("AFileAcc::WriteFile() @catch",this);
				outError = kFileError_DiskFull;
			}
			//�t�@�C���n���h���N���[�Y
			if( fh != nil )
			{
				[fh closeFile];
			}
	}];
	//File Coordinator��� #1422
	[fileCoordinator release];
	return result;
	
	
	/*#1304
	SInt16	forkref = 0;
	try
	{
		FSRef	ref;
		if( GetFSRef(ref) == false )   throw 0;
		HFSUniStr255	forkName;
		::FSGetDataForkName(&forkName);
		outError = ::FSOpenFork(&ref,forkName.length,forkName.unicode,fsRdWrPerm,&forkref);
		if( outError != noErr )   throw 0;
		ByteCount	actualCount = 0;
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			outError = ::FSWriteFork(forkref,fsFromStart,0,textptr.GetByteCount(),textptr.GetPtr(),&actualCount);
		}
		if( actualCount < inText.GetLength() )
		{
			_ACError("file write actual count is less than file size",this);
			result = false;
		}
		//if( outError != noErr )   throw 0;
		::FSSetForkSize(forkref,fsFromStart,actualCount);
		::FSCloseFork(forkref);
	}
	catch(...)
	{
		if( forkref != 0 )   ::FSCloseFork(forkref);
		result = false;
	}
	return result;
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::WriteFile( const AText& inText, AFileError& outError ) const
{
	ABool	result = true;
	FILE*	fp = NULL;
	try
	{
		AStCreateWcharStringFromAText	pathstr(mPath);
		if( _wfopen_s(&fp,pathstr.GetPtr(),L"wb") != 0 )   throw 0;//_wfopen_s��Win95����Ή�
		size_t	count = 0;
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			count = fwrite(textptr.GetPtr(),sizeof(char),textptr.GetByteCount(),fp);
		}
		if( count < inText.GetLength() )
		{
			_ACError("file write actual count is less than file size",this);
			result = false;
		}
		fclose(fp);
	}
	catch(...)
	{
		if( fp != NULL )   fclose(fp);
		result = false;
	}
	return result;
}
#endif

#pragma mark ===========================

#pragma mark --- �t�@�C������

//�t�@�C���쐬
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::CreateFile() const
{
	ABool	result = false;
	//���Ƀt�@�C�����݂Ȃ牽���������^�[��
	//�icreateFileAtPath�̓t�@�C���㏑�����Ă��܂��̂ŁA���łɑ��݂��Ă���Ȃ炱���Ń��^�[���K�v�B�j
	if( Exist() == true )
	{
		return true;
	}
	@try 
	{
		//��f�[�^�쐬
		NSData*	data = [NSData data];
		//�p�X�擾
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		//�t�@�C������
		result = ([[NSFileManager defaultManager] createFileAtPath:pathstr.GetNSString() 
														  contents:data 
														attributes:nil] == YES);
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::CreateFile() @catch",this);
	}
	return result;
	/*#1034
	AFileAcc	parent;
	parent.SpecifyParent(*this);
	FSRef	parentref;
	if( parent.GetFSRef(parentref) == false )   return false;
	AText	filename;
	GetFilename(filename);
	AUniText	filenameUTF16;
	filename.GetUTF16Text(filenameUTF16);
	AStUniTextPtr	textptr(filenameUTF16,0,filenameUTF16.GetItemCount());
	OSErr	err = ::FSCreateFileUnicode(&parentref,filenameUTF16.GetItemCount(),textptr.GetPtr(),kFSCatInfoNone,NULL,NULL,NULL);
	return (err==noErr);
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::CreateFile() const
{
	AStCreateWcharStringFromAText	pathstr(mPath);
	int	fh = NULL;
	if( _wsopen_s(&fh,pathstr.GetPtr(),_O_CREAT,_SH_DENYRW,_S_IREAD|_S_IWRITE) != 0 )   return false;
	_close(fh);
	return true;
}
#endif

//�t�H���_�쐬
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::CreateFolder()
{
	if( Exist() )
	{
		if( IsFolder() )   return true;
		else return false;
	}
	ABool	result = false;
	@try 
	{
		//�p�X�擾
		AText	path;
		GetNormalizedPath(path);
		AStCreateNSStringFromAText	pathstr(path);
		//�t�H���_����
		result = ([[NSFileManager defaultManager] createDirectoryAtPath:pathstr.GetNSString() 
											withIntermediateDirectories:NO attributes:nil error:nil] == YES);
	}
	@catch(NSException *theException)
	{
		_ACError("AFileAcc::CreateFolder() @catch",this);
	}
	return result;
	/*#1034
	AFileAcc	parent;
	parent.SpecifyParent(*this);
	FSRef	parentref;
	if( parent.GetFSRef(parentref) == false )   return false;
	AText	filename;
	GetFilename(filename);
	AUniText	filenameUTF16;
	filename.GetUTF16Text(filenameUTF16);
	AStUniTextPtr	textptr(filenameUTF16,0,filenameUTF16.GetItemCount());
	OSErr	err = ::FSCreateDirectoryUnicode(&parentref,filenameUTF16.GetItemCount(),textptr.GetPtr(),kFSCatInfoNone,NULL,NULL,NULL,NULL);
	return (err==noErr);
	*/
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::CreateFolder()
{
	if( Exist() == true )
	{
		if( IsFolder() == true )   return true;
		else return false;
	}
	//
	AStCreateWcharStringFromAText	pathstr(mPath);
	if( _wmkdir(pathstr.GetPtr()) != 0 )   return false;//�yOS�݊����z_wmkdir��Win95/98���Ή�
	return true;
}
#endif

//#427
/**
�t�H���_�쐬�i�ċA�I�j
*/
ABool	AFileAcc::CreateFolderRecursive()
{
	AFileAcc	parent;
	if( parent.SpecifyParent(*this) == false )   return false;//�p�X�f���~�^�����݂��Ȃ��ꍇ
	if( parent.Exist() == false )
	{
		//�e�t�H���_�����݂��Ă��Ȃ���΁A�e�t�H���_���ċA�I�ɍ쐬
		if( parent.CreateFolderRecursive() == false )   return false;
	}
	return CreateFolder();
}

/**
���̕ύX
@note AFileAcc���̖̂��̂��ύX
*/
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::Rename( const AText& inNewName )
{
	//#1425
	//�p�X�擾
	AText	oldPath;
	GetNormalizedPath(oldPath);
	AStCreateNSStringFromAText	oldPathstr(oldPath);
	//�e�t�H���_�擾
	AFileAcc	parent;
	if( parent.SpecifyParent(*this) == false )   return false;
	//���̕ύX��p�X�擾
	AFileAcc	newFile;
	newFile.SpecifyChild(parent, inNewName);
	AText	newPath;
	newFile.GetNormalizedPath(newPath);
	AStCreateNSStringFromAText	newPathstr(newPath);
	
	//�t�@�C�����ύX
	NSError *anError = nil;
  if( [[NSFileManager defaultManager] moveItemAtPath:oldPathstr.GetNSString() toPath:newPathstr.GetNSString() error:&anError] == NO )
	{
		return false;
	}
	
	/*#1425
	FSRef	ref;
	if( GetFSRef(ref) == false )   return false;
	
	AFileAcc	parent;
	parent.SpecifyParent(*this);
	AUniText	filenameUTF16;
	inNewName.GetUTF16Text(filenameUTF16);
	{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
		AStUniTextPtr	textptr(filenameUTF16,0,filenameUTF16.GetItemCount());
		if( ::FSRenameUnicode(&ref,filenameUTF16.GetItemCount(),textptr.GetPtr(),kTextEncodingUnknown,NULL) != noErr )   return false;
	}
	*/
	
	//B0417 SpecifyChild(parent,inNewName);
	SpecifyChildFile(parent,inNewName);
	return true;//B0000
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::Rename( const AText& inNewName )
{
	AStCreateWcharStringFromAText	oldpathstr(mPath);
	AFileAcc	newFileAcc;
	newFileAcc.SpecifySister(*this,inNewName);
	AText	path;
	newFileAcc.GetPath(path);
	AStCreateWcharStringFromAText	newnamestr(path);
	if( _wrename(oldpathstr.GetPtr(),newnamestr.GetPtr()) != 0 )   return false;//�yOS�݊����z_wrename��Win95/98���Ή�
	
	//���I�u�W�F�N�g�̃f�[�^���X�V
	CopyFrom(newFileAcc);
	return true;
}
#endif

//�t�@�C���폜
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::DeleteFile()
{
	/*#1404 �s��̌����͕s������FSDeleteObject()��deprecated�Ȃ̂ŐVAPI�ɍ����ւ�
	FSRef	ref;
	if( GetFSRef(ref) == false )   return;
	::FSDeleteObject(&ref);
	*/
	DeleteFileOrFolder();
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::DeleteFile()
{
	AStCreateWcharStringFromAText	pathstr(mPath);
	_wremove(pathstr.GetPtr());
}
#endif

//#427
/**
�t�H���_�폜
*/
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::DeleteFolder()
{
	/*#1404 �s��̌����͕s������FSDeleteObject()��deprecated�Ȃ̂ŐVAPI�ɍ����ւ�
	FSRef	ref;
	if( GetFSRef(ref) == false )   return;
	::FSDeleteObject(&ref);
	*/
	DeleteFileOrFolder();
}

//#1404
/**
�t�@�C���E�t�H���_�폜
*/
void	AFileAcc::DeleteFileOrFolder()
{
	//�p�X�擾
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	if( [pathstr.GetNSString() length] == 0 ) { _ACError("",this); return; }//pathstr�̕�������0�̏ꍇ�́A�����������^�[���i�O�ׁ̈j
	
	//�t�H���_����łȂ��ꍇ�͉����������^�[���iremoveItemAtPath�͏��recursive�폜�Ȃ̂ŁA�]����FSDeleteObject()�̓���ɍ��킹��B����ɂ��A����R�[�����Ɂi�t�@�C���폜�̂���ňႤ�t�H���_���w�肵�Ă��܂��Ă���A�Ȃǂ́j�s��������Ă����e�̂���t�H���_���폜���邱�Ƃ͂Ȃ��B�j
	NSArray*	contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:pathstr.GetNSString() error:nil];
	if( contents != nil )
	{
		if( [contents count] > 0 )
		{
			return;
		}
	}
	//�폜
	[[NSFileManager defaultManager] removeItemAtPath:pathstr.GetNSString() error:nil];
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::DeleteFolder()
{
	AStCreateWcharStringFromAText	pathstr(mPath);
	_wrmdir(pathstr.GetPtr());
}
#endif

//#427
/**
�t�@�C���E�t�H���_�폜�i�ċA�I�j
*/
void	AFileAcc::DeleteFileOrFolderRecursive()
{
	if( IsFolder() == true )
	{
		//�q�����ׂčċA�I�ɍ폜
		AObjectArray<AFileAcc>	children;
		GetChildren(children);
		for( AIndex i = 0; i < children.GetItemCount(); i++ )
		{
			children.GetObject(i).DeleteFileOrFolderRecursive();
		}
		//�t�H���_�폜
		DeleteFolder();
	}
	else
	{
		//�t�@�C���폜
		DeleteFile();
	}
}

//�t�@�C����inDst�փR�s�[
#if IMPLEMENTATION_FOR_MACOSX
ABool	AFileAcc::CopyFileTo( AFileAcc& inDst, const ABool inInhibitOverwrite ) const//#427
{
	//�R�s�[���ƃR�s�[�悪�����t�@�C���̂Ƃ��̓G���[
	if( Compare(inDst) == true )
	{
		_ACError("",this);
		return false;
	}
	
	//
	if( not Exist() )   return false;
	
	//#427 �������ݐ�t�@�C�����݃`�F�b�N
	if( inDst.Exist() == true && inInhibitOverwrite == true )
	{
		return false;
	}
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
	{
		/*#1425
		//R0000 MacOSX10.4�ȍ~�͂����API��copy���ł���
		FSRef	srcFSRef;
		if( GetFSRef(srcFSRef) == false )   return false;//#427
		*/
		//�R�s�[��e�t�H���_�擾
		AFileAcc	dstParent;
		dstParent.SpecifyParent(inDst);
		/*#1425
		FSRef	dstParentFSRef;
		if( dstParent.GetFSRef(dstParentFSRef) == false )   return false;//#427
		*/
		//�R�s�[��t�@�C�����擾
		AText	dstfilename;
		inDst.GetFilename(dstfilename);
		
		//------------------�R�s�[��̃t�@�C�������݂��Ă���ꍇ�̓��l�[������------------------
		AFileAcc	renamedDstFile;
		if( inDst.Exist() == true )
		{
			//���j�[�N�t�@�C�������擾
			AFileAcc	uniqChildDstFile;
			uniqChildDstFile.SpecifyUniqChildFile(dstParent,dstfilename);
			AText	renamedDstFileName;
			uniqChildDstFile.GetFilename(renamedDstFileName);
			//�����̃R�s�[��t�@�C�������j�[�N�t�@�C�����Ƀ��l�[������
			renamedDstFile = inDst;
			renamedDstFile.Rename(renamedDstFileName);
		}
		//------------------�R�s�[���s------------------
		{
			//#1425
			//�R�s�[���p�X�擾
			AText	srcPath;
			GetNormalizedPath(srcPath);
			AStCreateNSStringFromAText	srcPathstr(srcPath);
			//�R�s�[��p�X�擾
			AText	dstPath;
			inDst.GetNormalizedPath(dstPath);
			AStCreateNSStringFromAText	dstPathstr(dstPath);
			
			//�t�@�C���R�s�[
			NSError *anError = nil;
			if( [[NSFileManager defaultManager] copyItemAtPath:srcPathstr.GetNSString() toPath:dstPathstr.GetNSString() error:&anError] == NO )
			{
				//�����Ȃ��i�R�s�[��t�@�C���L���ŃR�s�[�����𔻒f���Ă��邽�߁j
			}
			
			/*#1425
			AStCreateCFStringFromAText	destName(dstfilename);
			::FSCopyObjectSync(&srcFSRef,&dstParentFSRef,destName.GetRef(),NULL,kFSFileOperationDefaultOptions);
			*/
		}
		//------------------���l�[���t�@�C�����쐬�����ꍇ�́A�R�s�[�������F���l�[���t�@�C�����폜�A�R�s�[���s���F���l�[���t�@�C�������̖��O�ɖ߂�------------------
		if( renamedDstFile.IsSpecified() == true )
		{
			if( inDst.Exist() == true )
			{
				//�R�s�[�������F���l�[���t�@�C�����폜
				renamedDstFile.DeleteFile();
			}
			else
			{
				//�R�s�[���s���F���l�[���t�@�C�������̖��O�ɖ߂�
				renamedDstFile.Rename(dstfilename);
			}
		}
	}
	else
	{
		//#1034 Mac OS X 10.5�����͖��Ή��Ƃ���
		_ACError("",NULL);
#if 0
		//
		inDst.DeleteFile();//#427
		//
		if( inDst.CreateFile() == false )   return false;//#427
		AText	buffer;
		//DF
		ReadTo(buffer);
		inDst.WriteFile(buffer);
		/*#1034
		//RF
		ReadTo(buffer,true);
		inDst.WriteResourceFork(buffer);
		*/
		//creator/type
		FSSpec	srcFSSpec, dstFSSpec;
		GetFSSpec(srcFSSpec);
		inDst.GetFSSpec(dstFSSpec);
		FInfo	src_fi;//B0000, dst_fi;
		::FSpGetFInfo(&srcFSSpec,&src_fi);
		//B0000::FSpGetFInfo(&dstFSSpec,&dst_fi);
		//B0000dst_fi.fdCreator = src_fi.fdCreator;
		//B0000dst_fi.fdType = src_fi.fdType;
		::FSpSetFInfo(&dstFSSpec,&src_fi);
#endif
	}
	return true;//#427
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
ABool	AFileAcc::CopyFileTo( AFileAcc& inDst, const ABool inInhibitOverwrite ) const//#427
{
	//�R�s�[���ƃR�s�[�悪�����t�@�C���̂Ƃ��̓G���[
	if( Compare(inDst) == true )
	{
		_ACError("",this);
		return false;
	}
	
	//
	if( Exist() == false )   return false;
	
	//#427 �������ݐ�t�@�C�����݃`�F�b�N
	if( inDst.Exist() == true && inInhibitOverwrite == true )
	{
		return false;
	}
	
	inDst.CreateFile();
	if( inDst.Exist() == false )   return false;
	AText	buffer;
	ReadTo(buffer);
	inDst.WriteFile(buffer);
	
	return true;//#427
	//��SHFileOperation���g�p���������悢?
}
#endif

/*#427
//�t�H���_��inDst�փR�s�[
#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::CopyFolderTo( AFileAcc& inDst ) const
{
	if( not Exist() )   return;
	if( AEnvWrapper::GetOSVersion() >= 0x1040 )
	{
		//R0000 MacOSX10.4�ȍ~�͂����API��copy���ł���
		FSRef	srcFSRef;
		if( GetFSRef(srcFSRef) == false )   return;
		AFileAcc	dstParent;
		dstParent.SpecifyParent(inDst);
		FSRef	dstParentFSRef;
		if( dstParent.GetFSRef(dstParentFSRef) == false )   return;
		AText	dstfilename;
		inDst.GetFilename(dstfilename);
		{
			AStCreateCFStringFromAText	destName(dstfilename);
			::FSCopyObjectSync(&srcFSRef,&dstParentFSRef,destName.GetRef(),NULL,kFSFileOperationDefaultOptions);
		}
	}
	else
	{
	if( inDst.CreateFolder() == false )   return;
	AObjectArray<AFileAcc>	children;
	GetChildren(children);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		AText	filename;
		children.GetObject(i).GetFilename(filename,false);
		AFileAcc	dstChild;
		dstChild.SpecifyChildFile(inDst,filename);
		if( children.GetObject(i).IsFolder() && children.GetObject(i).IsLink() == false )//#0 �V���{���b�N�����N�E�G�C���A�X�ɂ�閳�����[�v�h�~
		{
			children.GetObject(i).CopyFolderTo(dstChild);
		}
		else
		{
			children.GetObject(i).CopyFileTo(dstChild);
		}
	}
	}
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
*/

//#427
/**
*/
void	AFileAcc::CopyFolderTo( AFileAcc& inDst, const ABool inInhibitOverwrite, const ABool inOnlyWhenNoDstFolder ) const //#427
{
	if( Exist() == false )   return;
	
	//#427
	if( inOnlyWhenNoDstFolder == true )
	{
		if( inDst.Exist() == true )   return;
	}
	//
	inDst.CreateFolderRecursive();
	//
	AObjectArray<AFileAcc>	children;
	GetChildren(children);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		AText	filename;
		children.GetObject(i).GetFilename(filename,false);
		AFileAcc	dstChild;
		dstChild.SpecifyChildFile(inDst,filename);
		if( children.GetObject(i).IsFolder() && children.GetObject(i).IsLink() == false )//#0 �V���{���b�N�����N�E�G�C���A�X�ɂ�閳�����[�v�h�~
		{
			children.GetObject(i).CopyFolderTo(dstChild,inInhibitOverwrite,inOnlyWhenNoDstFolder);//#427
		}
		else
		{
			children.GetObject(i).CopyFileTo(dstChild,inInhibitOverwrite);//#427
		}
	}
}
//#427 #endif

//#427
/**
�t�@�C���܂��̓t�H���_���w���փR�s�[
*/
void	AFileAcc::CopyFileOrFolderTo( AFileAcc& inDst, const ABool inInhibitOverwrite, const ABool inOnlyWhenNoDstFolder )
{
	if( IsFolder() == true && IsLink() == false )
	{
		//==================�t�H���_���w���փR�s�[==================
		CopyFolderTo(inDst,inInhibitOverwrite,inOnlyWhenNoDstFolder);
	}
	else
	{
		//==================�t�@�C�����w���փR�s�[==================
		//�R�s�[��̐e�t�H���_���������Ȃ�t�H���_����
		AFileAcc	parentFolder;
		parentFolder.SpecifyParent(inDst);
		parentFolder.CreateFolderRecursive();
		//�t�@�C�����R�s�[
		CopyFileTo(inDst,inInhibitOverwrite);
	}
}

#pragma mark ===========================

#pragma mark --- �����N�^�G�C���A�X

#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::ResolveAnyLink( const AFileAcc& inFileMayLink )
{
	//#1425
	//�܂�inFileMayLink�����g�ɃR�s�[
	CopyFrom(inFileMayLink);
	//�G�C���A�X����
	ResolveAlias();
	
	/*#1425
	FSRef	fsref;
	if( inFileMayLink.GetFSRef(fsref) == false )   return;
	Boolean isFolder,isAlias;
	::FSResolveAliasFile(&fsref,true,&isFolder,&isAlias);
	SpecifyByFSRef(fsref);
	*/
}
ABool	AFileAcc::ResolveAlias()
{
	//URL�擾
	AText	path;
	GetNormalizedPath(path);
	AStCreateNSStringFromAText	pathstr(path);
	NSURL*	url = [NSURL fileURLWithPath:pathstr.GetNSString()];
	//URL����bookmark�𐶐��iURL���G�C���A�X�t�@�C���łȂ���΁Abookmark��NULL�ƂȂ�j
	CFErrorRef	error = nil;
	CFDataRef	bookmark = ::CFURLCreateBookmarkDataFromFile(NULL, (CFURLRef)url, &error);
	if( bookmark != NULL )
	{
		//bookmark����G�C���A�X�����ς�URL�𐶐�
		ABool	isStale = false;
		NSURL*	resolvedURL = (NSURL*)::CFURLCreateByResolvingBookmarkData(NULL, bookmark, kCFBookmarkResolutionWithoutUIMask, NULL, NULL, &isStale, &error);
		if( resolvedURL != nil )
		{
			//bookmark���琶������URL�̓t�@�C���Q��URL�ɂȂ��Ă���̂ŁA���ʂ̃t�@�C���p�XURL�֕ϊ�����B
			NSURL*	filePathURL = resolvedURL.filePathURL;
			if( filePathURL != nil )
			{
				//���g�ɉ����ς݃p�X��ݒ�
				AText	resolvedPath;
				resolvedPath.SetFromNSString(filePathURL.path);
				Specify(resolvedPath);
				return true;
			}
		}
	}
	return false;
	
	/*#1425
	//�G�C���A�X����
	FSRef	fsref;
	if( GetFSRef(fsref) == true )
	{
		Boolean	isFolder,isAlias;
		::FSResolveAliasFile(&fsref,true,&isFolder,&isAlias);
		SpecifyByFSRef(fsref);
	}
	*/
}
//#0
/**
�V���{���b�N�����N�E�G�C���A�X���ǂ�����Ԃ�
*/
ABool	AFileAcc::IsLink()
{
	//�G�C���A�X���ǂ������擾
	//#1425
	AFileAcc	file;
	file.CopyFrom(*this);
	if( file.ResolveAlias() == true )
	{
		return true;
	}
	/*#1425
	FSRef	fsref;
	if( GetFSRef(fsref) == true )
	{
		Boolean	isFolder,isAlias;
		::FSResolveAliasFile(&fsref,true,&isFolder,&isAlias);
		if( isAlias == true )   return true;
	}
	*/
	//�V���{���b�N�����N���ǂ������擾
	AText	path;
	GetPath(path);
	AStCreateCstringFromAText	pathstr(path);
	struct stat	sta;
	::lstat(pathstr.GetPtr(),&sta);
	if( S_ISLNK(sta.st_mode) == true )   return true;
	return false;
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::ResolveAnyLink( const AFileAcc& inFileMayLink )
{
	CopyFrom(inFileMayLink);
	ResolveAlias();
}
void	AFileAcc::ResolveAlias()
{
	if( Exist() == false )   return;
	if( IsLink() == false )   return;
	
	// Get a pointer to the IShellLink interface. 
	IShellLink*	psl = NULL;
	HRESULT	hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
	if( hres != S_OK )   {_ACError("",NULL);return;}
	
	// Get a pointer to the IShellLink interface. 
	IPersistFile* ppf = NULL;
	hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if( hres != S_OK )   {psl->Release();_ACError("",NULL);return;}
	
	// Load the shortcut. 
	AStCreateWcharStringFromAText	pathstr(mPath);
	hres = ppf->Load(pathstr.GetPtr(), STGM_READ); 
	if( hres != S_OK )   {ppf->Release();psl->Release();_ACError("",NULL);return;}
	
	// Resolve the link. 
	hres = psl->Resolve(NULL, 0); 
	if( hres != S_OK )   {ppf->Release();psl->Release();_ACError("",NULL);return;}
	
	// Get the path to the link target. 
	wchar_t	path[MAX_PATH];
	WIN32_FIND_DATA wfd = {0};
	hres = psl->GetPath(path,MAX_PATH,(WIN32_FIND_DATA*)&wfd,SLGP_UNCPRIORITY);
	if( hres == S_OK )
	{
		mPath.SetFromWcharString(path,sizeof(path)/sizeof(path[0]));
	}
	//Release
	ppf->Release();
	psl->Release();
}
//#0
/**
�V���{���b�N�����N�E�G�C���A�X���ǂ�����Ԃ�
*/
ABool	AFileAcc::IsLink()//#0
{
	AText	suffix;
	mPath.GetSuffix(suffix);
	return (suffix.Compare(".lnk")==true);
}

//�Q�l�Fhttp://msdn.microsoft.com/en-us/library/bb776891(VS.85).aspx
ABool	AFileAcc::CreateShellLink( const AFileAcc& inLinkFile ) const
{
	if( Exist() == false )   return false;
	
	// Get a pointer to the IShellLink interface. 
	IShellLink*	psl = NULL;
	HRESULT	hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
	if( hres != S_OK )   {_ACError("",NULL);return false;}
	// Set the path to the shortcut target and add the description. 
	AStCreateWcharStringFromAText	pathstr(mPath);
	psl->SetPath(pathstr.GetPtr());
	psl->SetDescription(L"");
	// Query IShellLink for the IPersistFile interface for saving the 
	// shortcut in persistent storage. 
	IPersistFile*	ppf = NULL;
	psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
	if( hres != S_OK )   {psl->Release();_ACError("",NULL);return false;}
	// Save the link by calling IPersistFile::Save. 
	AText	linkpath;
	inLinkFile.GetPath(linkpath);
	AStCreateWcharStringFromAText	linkpathstr(linkpath);
	hres = ppf->Save(linkpathstr.GetPtr(), TRUE); 
	ppf->Release(); 
	psl->Release();
	return (hres==S_OK);
}
#endif

#pragma mark ===========================

#pragma mark --- ���̑�

#if IMPLEMENTATION_FOR_MACOSX
void	AFileAcc::SpecifyAsUnitTestFolder()
{
	AFileAcc	docfolder;
	FSRef	ref;
	::FSFindFolder(kUserDomain,kDocumentsFolderType,true,&ref);
	docfolder.SpecifyByFSRef(ref);
	SpecifyChild(docfolder,"UnitTest");
	CreateFolder();
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
void	AFileAcc::SpecifyAsUnitTestFolder()
{
	//�}�C�h�L�������g����UnitTest�t�H���_
	wchar_t	buf[MAX_PATH];
	::SHGetFolderPathW(NULL,CSIDL_PERSONAL,NULL,0,buf);
	AText	path;
	path.SetFromWcharString(buf,sizeof(buf)/sizeof(buf[0]));
	AFileAcc	mydocfolder;
	mydocfolder.Specify(path);
	SpecifyChild(mydocfolder,"UnitTest");
}
#endif

#pragma mark ===========================

#pragma mark --- MacOS��p

#if IMPLEMENTATION_FOR_MACOSX

//���\�[�X�t�H�[�N�pMutex
AThreadMutex	AFileAcc::sResourceForkMutex;//#92

//MacOS����
ABool	AFileAcc::ExistResource( const ResType& inResType, const short& inResNum ) const
{
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	
	//���\�[�X�A�N�Z�X���b�N
	//#92 �����X���b�h���烊�\�[�X�t�@�C�����J���ƃN���b�V������炵���̂�
	AStMutexLocker	mutexlocker(sResourceForkMutex);
	
	/*#1034 short*/ResFileRefNum	rRefNum = -1;
	HFSUniStr255	forkName;
	::FSGetResourceForkName(&forkName);
	::FSOpenResourceFile(&fsref,forkName.length,forkName.unicode,fsRdPerm,&rRefNum);
	if( rRefNum != -1 )
	{
		::CloseResFile(rRefNum);
		return true;
	}
	return false;
}

//MacOS����
ABool	AFileAcc::ReadResouceTo( AText &outText, ResType inResType, short inResNum ) const
{
	ABool	result = false;
	/*#1034 short*/ResFileRefNum	rRefNum = -1;
	try
	{
		FSRef	fsref;
		if( GetFSRef(fsref) == false )   _AThrow("cannot find file",this);
		
		//���\�[�X�A�N�Z�X���b�N
		//#92 �����X���b�h���烊�\�[�X�t�@�C�����J���ƃN���b�V������炵���̂�
		AStMutexLocker	mutexlocker(sResourceForkMutex);
		
		HFSUniStr255	forkName;
		::FSGetResourceForkName(&forkName);
		::FSOpenResourceFile(&fsref,forkName.length,forkName.unicode,fsRdPerm,&rRefNum);
		if( rRefNum == -1 )   _AThrow("cannot open resource file",this);
		
		Handle	res = ::Get1Resource(inResType,inResNum);
		if( res != NULL )
		{
			::HLock(res);
			outText.SetFromTextPtr(*res,::GetHandleSize(res));
			::HUnlock(res);
			::ReleaseResource(res);
			result = true;
		}
		::CloseResFile(rRefNum);
	}
	catch(...)
	{
		outText.DeleteAll();
		if( rRefNum != -1 )   ::CloseResFile(rRefNum);
	}
	return result;
}

//#1034
#if 0
ABool	AFileAcc::WriteResouceFile( const AText &inText, const ResType inResType, const short inResNum ) const
{
	ABool	result = true;
	short	rRefNum = -1;
	Handle	res = NULL;
	try
	{
		FSRef	fsref;
		if( GetFSRef(fsref) == false )   _AThrow("cannot find file",this);
		
		FSSpec	fsspec;
		GetFSSpec(fsspec);
		//B0000  ::FSpCreateResFile(&fsspec,NULL,NULL,0);
		//B0000 ���\�[�X�t�H�[�N�����݂��Ȃ��t�@�C����creator,type�ݒ聨�V�K�Ƀ��\�[�X�t�H�[�N�쐬�A��creator, type����������C��
		AFileAttribute	attr;
		GetFileAttribute(attr);
		::FSpCreateResFile(&fsspec,attr.creator,attr.type,0);
		
		//���\�[�X�A�N�Z�X���b�N
		//#92 �����X���b�h���烊�\�[�X�t�@�C�����J���ƃN���b�V������炵���̂�
		AStMutexLocker	mutexlocker(sResourceForkMutex);
		
		HFSUniStr255	forkName;
		::FSGetResourceForkName(&forkName);
		/*B0000 OSErr err =*/ ::FSOpenResourceFile(&fsref,forkName.length,forkName.unicode,fsWrPerm,&rRefNum);
		//short	rRefNum = ::FSOpenResFile(&fsref,fsRdPerm);���������݌������Ȃ��Ă������Ă��܂�
		if( rRefNum == -1 )   _AThrow("cannot open resource file",this);
		
		res = ::Get1Resource(inResType,inResNum);
		if( res != NULL )
		{
			::RemoveResource(res);
			::DisposeHandle(res);
		}
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			res = ::NewHandle(textptr.GetByteCount());
			if( res == NULL )   _ACThrow("cannot create newhandle",this);
			::HLock(res);
			AMemoryWrapper::Memmove(*res,textptr.GetPtr(),textptr.GetByteCount());
			::HUnlock(res);
		}
		::AddResource(res,inResType,inResNum,"\p");
		::WriteResource(res);
		::ReleaseResource(res);
		::CloseResFile(rRefNum);
	}
	catch(...)
	{
		if( res != NULL )   ::DisposeHandle(res);
		if( rRefNum != -1 )   ::CloseResFile(rRefNum);
		result = false;
	}
	return result;
}
#endif

//MacOSX�ŗL�̏���
ABool	AFileAcc::GetFSRef( FSRef& outFSRef ) const
{
	/* if( mFSRefSpecified == true )
	{
		outFSRef = mFSRef;
		return true;
	}*/
	AText	path;
	GetPath(path);
	path.ConvertFromUTF8ToUTF16();//�����x�΍􌟓�
	ATextEncodingWrapper::ConvertUTF16ToHFSPlusDecomp(path);
	path.ConvertToUTF8FromUTF16();
	AStCreateCstringFromAText	cstr(path);
	OSStatus err = ::FSPathMakeRef((UInt8*)(cstr.GetPtr()),&outFSRef,NULL);
	if( err == noErr )
	{
		/* mFSRef = outFSRef;
		mFSRefSpecified = true;*/
		return true;
	}
	else
	{
		return false;
	}
}

ABool	AFileAcc::GetFSSpec( FSSpec& outFSSpec ) const
{
	FSRef	ref;
	if( GetFSRef(ref) == false )   return false;
	::FSGetCatalogInfo(&ref,kFSCatInfoNone,NULL,NULL,&outFSSpec,NULL);
	return true;
}

/*#1034
ABool	AFileAcc::WriteResourceFork( const AText& inText ) const
{
	bool	result = true;
	SInt16	forkref = 0;
	try
	{
		FSRef	ref;
		if( GetFSRef(ref) == false )   throw 0;
		
		//���\�[�X�A�N�Z�X���b�N
		//#92 �����X���b�h���烊�\�[�X�t�@�C�����J���ƃN���b�V������炵���̂�
		AStMutexLocker	mutexlocker(sResourceForkMutex);
		
		HFSUniStr255	forkName;
		::FSGetResourceForkName(&forkName);
		OSErr err = ::FSOpenFork(&ref,forkName.length,forkName.unicode,fsRdWrPerm,&forkref);
		if( err != noErr )   throw 0;
		ByteCount	actualCount = 0;
		{//#598 arrayptr�̗L���͈͂��ŏ��ɂ���
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			err = ::FSWriteFork(forkref,fsFromStart,0,textptr.GetByteCount(),textptr.GetPtr(),&actualCount);
		}
		if( actualCount < inText.GetLength() )
		{
			_ACError("file write actual count is less than file size",this);
		}
		if( err != noErr )   throw 0;
		::FSSetForkSize(forkref,fsFromStart,actualCount);
		::FSCloseFork(forkref);
	}
	catch(...)
	{
		if( forkref != 0 )   ::FSCloseFork(forkref);
		result = false;
	}
	return result;
}
*/

/*#1425
void	AFileAcc::SetCreatorType( OSType inCreator, OSType inType )
{
	*#1034
	FSSpec	fsspec;
	GetFSSpec(fsspec);
	FInfo	fi;
	::FSpGetFInfo(&fsspec,&fi);
	fi.fdCreator = inCreator;
	fi.fdType = inType;
	::FSpSetFInfo(&fsspec,&fi);
	*
	FSRef	fsref;
	GetFSRef(fsref);
	FSCatalogInfo catInfo;
	::FSGetCatalogInfo(&fsref, kFSCatInfoFinderInfo, &catInfo, NULL, NULL, NULL);
	FileInfo *info = (FileInfo*)catInfo.finderInfo;
	info->fileCreator = inCreator;
	info->fileType = inType;
	::FSSetCatalogInfo(&fsref, kFSCatInfoFinderInfo, &catInfo);
}
*/

void	AFileAcc::GetCreatorType( OSType& outCreator, OSType& outType ) const
{
	//#1425
	AFileAttribute	attribute = {0};
	if( GetFileAttribute(attribute) == true )
	{
		outCreator = attribute.creator;
		outType = attribute.type;
	}
	/*#1425
	//�N���G�[�^�^�^�C�v�ǂݍ���
	FSCatalogInfo	catinfo;
	FSRef	fsref;
	GetFSRef(fsref);
	::FSGetCatalogInfo(&fsref,kFSCatInfoFinderInfo,&catinfo,NULL,NULL,NULL);
	outCreator = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileCreator;
	outType = ((FileInfo*)(&(catinfo.finderInfo[0])))->fileType;
	*/
}

void	AFileAcc::SpecifyByFSRef( const FSRef& inFSRef )
{
	char	buf[4096];
	buf[0] = 0;
	::FSRefMakePath(&inFSRef,(UInt8*)buf,4096);
	AText	text;
	text.AddCstring(buf);
	Specify(text);
	/* mFSRef = inFSRef;
	mFSRefSpecified = true;*/
}

#ifndef __LP64__
void	AFileAcc::SpecifyByFSSpec( const FSSpec& inFSSpec )
{
	FSRef	ref;
	::FSpMakeFSRef(&inFSSpec,&ref);
	SpecifyByFSRef(ref);
}
#endif

//OS�����M����UTF16(HFSPlusDecomp variant)��UTF8(CanonicalComp)�ɕϊ�����AText�Ɋi�[����
//HFSPlusDecomp�ł͑��_������������Ă���
void	AFileAcc::SetTextByHFSUniStr255( const HFSUniStr255 inHFSUniString, AText& outText ) const
{
	outText.DeleteAll();
	outText.InsertFromTextPtr(0,reinterpret_cast<AConstUCharPtr>(inHFSUniString.unicode),inHFSUniString.length*sizeof(UniChar));
	/*#1040
	ATextEncodingWrapper::ConvertUTF16ToCanonicalComp(outText);
	outText.ConvertToUTF8FromUTF16();
	*/
	outText.ConvertToUTF8FromUTF16();
	outText.ConvertToCanonicalComp();
}

//B0000
ABool	AFileAcc::IsAppleScriptFile()
{
	AText	filename;
	GetFilename(filename);
	AText	suffix;
	filename.GetSuffix(suffix);
	if( suffix.Compare(".scpt") == true )   return true;
	if( suffix.Compare(".scptd") == true )   return true;//#206 �X�N���v�g�o���h�������s�ł��Ȃ������C��
	AFileAttribute	attr;
	GetFileAttribute(attr);
	if( attr.type == 'osas' )   return true;
	//R0137
	if( GetBundleFolderAttribute(attr) == true )
	{
		if( attr.type == 'osas' )   return true;
	}
	return false;
}

//R0230
//�����͂��̊֐��͎g�p���Ă��Ȃ��B����A�g�p����ꍇ�́AGetTextEncodingIANAName()�����l�����邱�ƁB
ABool	AFileAcc::GetTextEncodingByXattr( AText& outTextEncodingName ) const
{
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_4 )   return false;
#if !BUILD_FOR_MACOSX_10_3
	AText	path;
	GetPath(path);
	AStCreateCstringFromAText	pathstr(path);
	ssize_t	len = ::getxattr(pathstr.GetPtr(),"com.apple.TextEncoding",NULL,0,0,0);
	if( len > 0 )
	{
		AText	value;
		value.Reserve(0,len);
		{//#598
			AStTextPtr	textptr(value,0,value.GetItemCount());
			::getxattr(pathstr.GetPtr(),"com.apple.TextEncoding",textptr.GetPtr(),textptr.GetByteCount(),0,0);
		}
		ATextArray	v;
		value.Explode(';',v);
		if( v.GetItemCount() == 2 )
		{
			/*
			http://developer.apple.com/releasenotes/Cocoa/Foundation.html
			�t�H�[�}�b�g��F
			MACINTOSH;0
			UTF-8;134217984
			UTF-8;
			;3071
			*/
			if( v.GetTextConst(0).GetItemCount() > 0 )
			{
				if( ATextEncodingWrapper::CheckTextEncodingAvailability(v.GetTextConst(0)) == true )
				{
					outTextEncodingName.SetText(v.GetTextConst(0));
					return true;
				}
			}
			if( v.GetTextConst(1).GetItemCount() > 0 )
			{
				ATextEncoding	tecnum = v.GetTextConst(1).GetNumber();
				return ATextEncodingWrapper::GetTextEncodingName(tecnum,outTextEncodingName);
			}
		}
		return false;
	}
#endif
	return false;
}

//R0230
void	AFileAcc::SetTextEncodingByXattr( const AText& inTextEncodingName )
{
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_4 )   return;
#if !BUILD_FOR_MACOSX_10_3
	AText	path;
	GetPath(path);
	AStCreateCstringFromAText	pathstr(path);
	if( inTextEncodingName.GetItemCount() > 0 )
	{
		AText	value;
		value.SetText(inTextEncodingName);
		value.Add(';');
		ATextEncoding	tec;
		if( ATextEncodingWrapper::GetTextEncodingFromName(inTextEncodingName,tec) == true )
		{
			value.AddNumber(tec);
		}
		{//#598
			AStTextPtr	textptr(value,0,value.GetItemCount());
			::setxattr(pathstr.GetPtr(),"com.apple.TextEncoding",textptr.GetPtr(),textptr.GetByteCount(),0,0);
		}
	}
	else
	{
		::removexattr(pathstr.GetPtr(),"com.apple.TextEncoding",0);
	}
#endif
}

//R0137
ABool	AFileAcc::GetBundleFolderAttribute( AFileAttribute& outFileAttribute ) const
{
	outFileAttribute.creator = 0;
	outFileAttribute.type = 0;
	
	if( IsBundleFolder() == false )   return false;
	
	FSRef	fsref;
	if( GetFSRef(fsref) == false )   return false;
	
	CFURLRef	urlref = ::CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	if( urlref == NULL )   return false;
	CFBundleRef	bundleref = ::CFBundleCreate(kCFAllocatorDefault,urlref);
	::CFRelease(urlref);
	if( bundleref == NULL )   return false;
	::CFBundleGetPackageInfo(bundleref,&outFileAttribute.type,&outFileAttribute.creator);
	::CFRelease(bundleref);
	return true;
}

#endif

#pragma mark ===========================

#pragma mark --- path delimiter

#if IMPLEMENTATION_FOR_MACOSX
//AFileAcc�����Ŏg�p����i��mPath�֊i�[����Ƃ��́j�p�X��؂蕶����Ԃ�
AUChar	AFileAcc::GetPathDelimiter( const AFilePathType inFilePathType )
{
	switch(inFilePathType)
	{
		//UNIX�p�X�`��
	  case kFilePathType_Default:
		{
			return kUChar_Slash;
		}
		//  /volumename/folder1/folder2/test.txt�̌`��(kFilePathType_1)
	  case kFilePathType_1:
		{
			return kUChar_Slash;
		}
		//  :volumename:folder1:folder2:test.txt�̌`��(kFilePathType_2)
	  case kFilePathType_2:
		{
			return kUChar_Colon;
		}
	  default:
		{
			_ACError("",NULL);
			return kUChar_Slash;
		}
	}
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
AUChar	AFileAcc::GetPathDelimiter( const AFilePathType inFilePathType )
{
	return kUChar_Backslash;
}
#endif



