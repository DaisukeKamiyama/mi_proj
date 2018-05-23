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

ADocument_IndexWindow

*/

#include "stdafx.h"

#include "ADocument_IndexWindow.h"
#include "AApp.h"
//#include "AUtil.h"
#include "AView_Index.h"

#pragma mark ===========================
#pragma mark [�N���X]AIndexGroup
#pragma mark ===========================
//�C���f�b�N�X�E�C���h�E
#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AIndexGroup::AIndexGroup( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent)
{
}
//�f�X�g���N�^
AIndexGroup::~AIndexGroup()
{
}

#pragma mark ===========================

void	AIndexGroup::SetGroupTitleText( const AText& inTitle1, const AText& inTitle2 )
{
	mTitle1.SetText(inTitle1);
	mTitle2.SetText(inTitle2);
}

void	AIndexGroup::GetGroupTitleText( AText& outTitle1, AText& outTitle2 ) const
{
	outTitle1.SetText(mTitle1);
	outTitle2.SetText(mTitle2);
}

//#465
/**
*/
void	AIndexGroup::SetGroupBaseFolder( const AFileAcc& inBaseFolder )
{
	mBaseFolder.CopyFrom(inBaseFolder);
}

//#465
/**
*/
void	AIndexGroup::GetGroupBaseFolder( AFileAcc& outBaseFolder ) const
{
	outBaseFolder.CopyFrom(mBaseFolder);
}

void	AIndexGroup::InsertItems_TextPosition(  const AIndex inItemIndex, const ATextArray& inFilePath, 
		const ATextArray& inHitText, const ATextArray& inPreHitText, const ATextArray& inPostHitText, const ATextArray& inPositionText, 
		const ATextArray& inParagraphText,//#465
		const ANumberArray& inStartPosition, const ANumberArray& inLength, const ANumberArray& inParagraph, const AText& inComment )
{
	//#895
	//�Ō�ɒǉ��������ǂ������擾
	ABool	addedAtLast = (inItemIndex == mArray_Type.GetItemCount());
	//
	//���Ή������F���x���P
	//#693 Insert1_SwapContent->Insert1�ւ̕ύX�ɂ�葬�x�x���Ȃ��Ă��邩������Ȃ��B�v�����B
	AItemCount	itemCount = inFilePath.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		mArray_Type.Insert1(inItemIndex+i,kIndexRowType_TextPosition);
		AText	text;
		inHitText.Get(i,text);
		mArray_HitText.Insert1/*#693_SwapContent*/(inItemIndex+i,text);
		inPreHitText.Get(i,text);
		mArray_PreHitText.Insert1/*#693_SwapContent*/(inItemIndex+i,text);
		inPostHitText.Get(i,text);
		mArray_PostHitText.Insert1/*#693_SwapContent*/(inItemIndex+i,text);
		inFilePath.Get(i,text);
		mArray_FilePath.Insert1/*#693_SwapContent*/(inItemIndex+i,text);
		//#895
		//�e�t�@�C���ɑΉ����鍀��index�e�[�u���ɒǉ�
		InsertFileToItemIndexArray(text,inItemIndex+i,addedAtLast);
		//
		inPositionText.Get(i,text);
		mArray_PositionText.Insert1/*#693_SwapContent*/(inItemIndex+i,text);
		inParagraphText.Get(i,text);//#465
		mArray_ParagraphText.Insert1/*#693_SwapContent*/(inItemIndex+i,text);//#465
		mArray_StartPosition.Insert1(inItemIndex+i,inStartPosition.Get(i));
		mArray_Length.Insert1(inItemIndex+i,inLength.Get(i));
		mArray_Paragraph.Insert1(inItemIndex+i,inParagraph.Get(i));
		mArray_CommentText.Insert1(inItemIndex+i,inComment);
	}
}

void	AIndexGroup::InsertItem_TextPosition( const AIndex inItemIndex, const AText& inFilePath,
		const AText& inHitText, const AText& inPreHitText, const AText& inPostHitText, const AText& inPositionText,
		const AText& inParagraphText,//#465
		const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph, const AText& inComment )
{
	//#895
	//�Ō�ɒǉ��������ǂ������擾
	ABool	addedAtLast = (inItemIndex == mArray_Type.GetItemCount());
	//
	mArray_Type.Insert1(inItemIndex,kIndexRowType_TextPosition);
	mArray_HitText.Insert1(inItemIndex,inHitText);
	mArray_PreHitText.Insert1(inItemIndex,inPreHitText);
	mArray_PostHitText.Insert1(inItemIndex,inPostHitText);
	mArray_FilePath.Insert1(inItemIndex,inFilePath);
	mArray_PositionText.Insert1(inItemIndex,inPositionText);
	mArray_ParagraphText.Insert1(inItemIndex,inParagraphText);//#465
	mArray_StartPosition.Insert1(inItemIndex,inStartPosition);
	mArray_Length.Insert1(inItemIndex,inLength);
	mArray_Paragraph.Insert1(inItemIndex,inParagraph);
	mArray_CommentText.Insert1(inItemIndex,inComment);
	//#895
	//�e�t�@�C���ɑΉ����鍀��index�e�[�u���ɒǉ�
	InsertFileToItemIndexArray(inFilePath,inItemIndex,addedAtLast);
}

//#895
/**
�e�t�@�C���ɑΉ����鍀��index�e�[�u���ɒǉ�
*/
void	AIndexGroup::InsertFileToItemIndexArray( const AText& inFilePath, const AIndex inItemIndex, const ABool inAddedAtLast )
{
	//�Ō�ւ̒ǉ��ȊO�̎��́A�r���̍��ڂ�index�����炷�B
	if( inAddedAtLast == false )
	{
		//�e�[�u���e�s���Ƃ̃��[�v
		AItemCount	fileToItemItemCount = mFileToItemIndexArray_IndexArray.GetItemCount();
		for( AIndex i = 0; i < fileToItemItemCount; i++ )
		{
			AArray<AIndex>&	indexArray = mFileToItemIndexArray_IndexArray.GetObject(i);
			//�}��index�ȍ~�̑S�Ă�index�����炷
			AStArrayPtr<AIndex>	arrayptr(indexArray,0,indexArray.GetItemCount());
			AIndex*	p = arrayptr.GetPtr();
			AItemCount	itemCount = indexArray.GetItemCount();
			for( AIndex j = 0; j < itemCount; j++ )
			{
				if( p[j] >= inItemIndex )
				{
					p[j]++;
				}
			}
		}
	}
	//�t�@�C���p�X����e�[�u����index������
	AIndex	filestartindexIndex = mFileToItemIndexArray_Path.Find(inFilePath);
	if( filestartindexIndex == kIndex_Invalid )
	{
		//�t�@�C���ɑΉ�����e�[�u���s�������Ȃ�A�s����
		filestartindexIndex = mFileToItemIndexArray_Path.Add(inFilePath);
		mFileToItemIndexArray_IndexArray.AddNewObject();
	}
	//����index���t�@�C���ɑΉ�����e�[�u���s�ɒǉ�
	mFileToItemIndexArray_IndexArray.GetObject(filestartindexIndex).Add(inItemIndex);
}

void	AIndexGroup::SetItem_TextPosition( const AIndex inItemIndex, const AText& inFilePath,
		const AText& inHitText, const AText& inPreHitText, const AText& inPostHitText, const AText& inPositionText,
		const AText& inParagraphText,//#465
		const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph, const AText& inComment )
{
	mArray_Type.Set(inItemIndex,kIndexRowType_TextPosition);
	mArray_HitText.Set(inItemIndex,inHitText);
	mArray_PreHitText.Set(inItemIndex,inPreHitText);
	mArray_PostHitText.Set(inItemIndex,inPostHitText);
	mArray_FilePath.Set(inItemIndex,inFilePath);
	mArray_PositionText.Set(inItemIndex,inPositionText);
	mArray_ParagraphText.Set(inItemIndex,inParagraphText);//#465
	mArray_StartPosition.Set(inItemIndex,inStartPosition);
	mArray_Length.Set(inItemIndex,inLength);
	mArray_Paragraph.Set(inItemIndex,inParagraph);
	mArray_CommentText.Set(inItemIndex,inComment);
	//#895
	//��mFileStartIndexArray_Path�̍X�V�K�v
}

void	AIndexGroup::DeleteItem( const AIndex inItemIndex )
{
	mArray_Type.Delete1(inItemIndex);
	mArray_HitText.Delete1(inItemIndex);
	mArray_PreHitText.Delete1(inItemIndex);
	mArray_PostHitText.Delete1(inItemIndex);
	mArray_FilePath.Delete1(inItemIndex);
	mArray_PositionText.Delete1(inItemIndex);
	mArray_StartPosition.Delete1(inItemIndex);
	mArray_Length.Delete1(inItemIndex);
	mArray_Paragraph.Delete1(inItemIndex);
	mArray_CommentText.Delete1(inItemIndex);
	//#895
	//��mFileStartIndexArray_Path�̍X�V�K�v
}

AItemCount	AIndexGroup::GetItemCount() const
{
	return mArray_Type.GetItemCount();
}

void	AIndexGroup::Get( const AIndex inItemIndex, 
		AText& outFilePath, AText& outHitText, AText& outPreHitText, AText& outPostHitText, AText& outPositionText, 
		ANumber& outStartPosition, ANumber& outLength, ANumber& outParagraph, AText& outComment ) const
{
	if( inItemIndex == kIndex_Invalid )
	{
		//�O���[�v�^�C�g���擾
		outFilePath.SetText(mTitle1);
		outHitText.DeleteAll();
		outPreHitText.DeleteAll();
		outPostHitText.DeleteAll();
		outPositionText.DeleteAll();
		outComment.SetText(mTitle2);
		outStartPosition = 0;
		outLength = 0;
		outParagraph = 0;
	}
	else
	{
		//�O���[�v�����ڃf�[�^�擾
		mArray_FilePath.Get(inItemIndex,outFilePath);
		mArray_HitText.Get(inItemIndex,outHitText);
		mArray_PreHitText.Get(inItemIndex,outPreHitText);
		mArray_PostHitText.Get(inItemIndex,outPostHitText);
		mArray_PositionText.Get(inItemIndex,outPositionText);
		mArray_CommentText.Get(inItemIndex,outComment);
		outStartPosition = mArray_StartPosition.Get(inItemIndex);
		outLength = mArray_Length.Get(inItemIndex);
		outParagraph = mArray_Paragraph.Get(inItemIndex);
	}
}

//#465
/**
*/
void	AIndexGroup::GetParagraphText( const AIndex inItemIndex, AText& outParagraphText ) const
{
	outParagraphText.DeleteAll();
	if( inItemIndex != kIndex_Invalid )
	{
		mArray_ParagraphText.Get(inItemIndex,outParagraphText);
	}
}

void	AIndexGroup::Update_TextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	AText	filepath;
	inFile.GetPath(filepath);
	/*#895
	for( AIndex i = 0; i < mArray_Type.GetItemCount(); i++ )
	{
		AText	path;
		mArray_FilePath.Get(i,path);
		if( filepath.Compare(path) == true )
		{
			ATextIndex	spos = mArray_StartPosition.Get(i);
			if( spos > inTextIndex )
			{
				mArray_StartPosition.Set(i,spos+inInsertedCount);
			}
		}
	}
	*/
	//�e�t�@�C���ɑΉ����鍀��index�e�[�u���̍sindex������
	AIndex	filestartindexIndex = mFileToItemIndexArray_Path.Find(filepath);
	if( filestartindexIndex != kIndex_Invalid )
	{
		//�Ώۃt�@�C����index array���擾
		AArray<AIndex>&	indexArray = mFileToItemIndexArray_IndexArray.GetObject(filestartindexIndex);
		//�|�C���^�擾
		AStArrayPtr<AIndex>	indexarrayptr(indexArray,0,indexArray.GetItemCount());
		AIndex*	p_indexarray = indexarrayptr.GetPtr();
		AStArrayPtr<ANumber>	sposarrayptr(mArray_StartPosition,0,mArray_StartPosition.GetItemCount());
		ANumber*	p_sposarray = sposarrayptr.GetPtr();
		AItemCount	itemCount = indexArray.GetItemCount();
		AItemCount	sposArrayItemCount = mArray_StartPosition.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			//�e���ڂɂ��āA�}���ʒu�ȍ~��������A�}�������񒷕��������炷
			AIndex	index = p_indexarray[i];
			if( index >= 0 && index < sposArrayItemCount )//�|�C���^�͈͊O���A�N�Z�X���Ȃ��悤�Ƀ`�F�b�N
			{
				if( p_sposarray[index] > inTextIndex )
				{
					p_sposarray[index] += inInsertedCount;
				}
			}
		}
	}
}

#pragma mark ===========================
#pragma mark [�N���X]ADocument_IndexWindow
#pragma mark ===========================
//�C���f�b�N�X�E�C���h�E
#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^

//�R���X�g���N�^
//�h�L�������g�̃R���X�g���N�^���ł̓E�C���h�E�\���͍s��Ȃ����ƁB�A�v���P�[�V�������̃h�L�������gArray�ւ̓o�^���܂��Ȃ̂ŁA�\�����AView����̃h�L�������g�擾�����܂������Ȃ��B
ADocument_IndexWindow::ADocument_IndexWindow( AObjectArrayItem* inParent, const AObjectID inDocImpID )
		: ADocument(inParent, inDocImpID), //#1034
/*#1034
#if IMPLEMENTATION_FOR_MACOSX
		mASImp(&(GetApp().SPI_GetASImp()),GetObjectID()), 
#endif
*/
		mAskSavingMode(false)
{
}
//�f�X�g���N�^
ADocument_IndexWindow::~ADocument_IndexWindow()
{
}

#pragma mark ===========================

#pragma mark ---�����ݒ�

//�����ݒ�i�t�@�C������j
void	ADocument_IndexWindow::Load( const AFileAcc& inFile )//#92 Init->Load
{
	//�t�@�C���ݒ�
	NVI_SpecifyFile(inFile);
	//
	AText	filename;
	inFile.GetFilename(filename);
	mTitle.SetText(filename);
	/*#92
	//
	AIndexWindowFactory	factory(GetObjectID());
	mWindowID = GetApp().NVI_CreateWindow(factory);
	GetApp().NVI_GetWindowByID(mWindowID).NVI_Create(GetObjectID());
	*/
	
	//
	AFileAcc	folder;
	folder.SpecifyParent(inFile);
	
	//
	AText	text;
	inFile.ReadTo(text);
	AItemCount	count = text.GetItemCount();
	AIndex pos = 0;
	for( ; pos < count; )
	{
		if( text.Get(pos) == kUChar_LineEnd )   break;
	}
	pos++;
	if( pos >= count )   return;
	
	AText	returntext;
	returntext.SetLocalizedText("ReturnCodeText");
	for( ; pos < count; )
	{
		AText	line;
		text.GetLine(pos,line);
		AText	lowercase;
		lowercase.SetText(line);
		lowercase.ChangeCaseLower();
		if( lowercase.CompareMin("file") == true )
		{
			AFileAcc	basefolder;//#465�i�����̓x�[�X�t�H���_���Ή��j
			if( SPI_GetGroupCount() == 0 )   SPI_AddGroup(GetEmptyText(),GetEmptyText(),basefolder);//#465
			line.Delete(0,7);
			AFileAcc	file;
			if( line.GetItemCount() > 0 )
			{
				if( line.Get(0) == ':' )
				{
					file.Specify(line,kFilePathType_2);
				}
				else
				{
					file.SpecifyChild(folder,line,kFilePathType_2);
				}
			}
			AText	filepath;
			file.GetPath(filepath);
			text.GetLine(pos,line);
			if( line.CompareMin("***") == true )
			{
				AIndex	p = 3;
				AText	token;
				line.GetToken(p,token);
				AIndex	paraIndex = token.GetNumber();
				line.GetToken(p,token);
				AIndex	spos = token.GetNumber();
				line.GetToken(p,token);
				AIndex	epos = token.GetNumber();
				line.GetToken(p,token);
				AIndex	hitspos = token.GetNumber();
				line.GetToken(p,token);
				p++;//#220
				AIndex	hitepos = token.GetNumber();
				AText	prehit, hit, posthit;
				line.GetText(p,hitspos,prehit);
				line.GetText(p+hitspos,hitepos-hitspos,hit);
				line.GetText(p+hitepos,line.GetItemCount()-(p+hitepos),posthit);
				prehit.ReplaceChar(0x1B,returntext);
				hit.ReplaceChar(0x1B,returntext);
				posthit.ReplaceChar(0x1B,returntext);
				SPI_AddItem_TextPosition(SPI_GetGroupCount()-1,filepath,hit,prehit,posthit,GetEmptyText(),
						GetEmptyText(),//#465
						spos,epos-spos,paraIndex,GetEmptyText());
			}
			else
			{
				SPI_AddItem_TextPosition(SPI_GetGroupCount()-1,filepath,GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
						GetEmptyText(),//#465
						0,0,0,line);
			}
		}
		else if( lowercase.CompareMin("http") == true || lowercase.CompareMin("comm") )
		{
			text.GetLine(pos,line);//#220
			AFileAcc	basefolder;//#465�i�����̓x�[�X�t�H���_���Ή��j
			if( SPI_GetGroupCount() == 0 )   SPI_AddGroup(GetEmptyText(),GetEmptyText(),basefolder);//#465
			SPI_AddItem_TextPosition(SPI_GetGroupCount()-1,GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
					GetEmptyText(),//#465
						0,0,0,line);
		}
		else if( lowercase.CompareMin("grou") == true )
		{
			ABool	expand = false;
			if( line.GetItemCount() > 5 )
			{
				if( line.Get(5) == 'E' )   expand = true;
			}
			text.GetLine(pos,line);
			//#220 �^�C�g���Q���A�ۑ��t�@�C�����畜���ł��Ȃ������C��
			AText	title1, title2;
			AIndex	title2pos = 0;
			for( ; title2pos < line.GetItemCount(); title2pos++ )
			{
				if( line.Get(title2pos) == kUChar_Tab )
				{
					title2pos++;
					break;
				}
			}
			line.GetText(0,title2pos-0,title1);
			line.GetText(title2pos,line.GetItemCount()-title2pos,title2);
			AFileAcc	basefolder;//#465�i�����̓x�[�X�t�H���_���Ή��j
			SPI_AddGroup(title1,title2,basefolder);//#220line,GetEmptyText()); #465
			
			if( expand == false )
			{
				SPI_Collapse(SPI_GetGroupCount()-1);
			}
		}
	}
	/*#92
	//
	GetApp().NVI_GetWindowByID(mWindowID).NVI_Show();
	//�E�C���h�E���j���[�X�V
	GetApp().SPI_UpdateWindowMenu();
	*/
}

void	ADocument_IndexWindow::SPI_SetTitle( const AText& inTitle )
{
	mTitle.SetText(inTitle);
	//#92 GetWindow().NVI_UpdateTitleBar();
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).NVI_UpdateWindowTitleBar();
	}
	//�E�C���h�E���j���[�X�V
	//#922 GetApp().SPI_UpdateWindowMenu();
}


#if IMPLEMENTATION_FOR_MACOSX
extern void	WritePrefVersion( short inRRefNum );
#endif

//
void	ADocument_IndexWindow::SPI_Save( const ABool inSaveAs )
{
	//File������A�������́ASaveAs�Ȃ�A�t�@�C���ۑ��_�C�A���O��\�����ďI��
	if( NVI_IsFileSpecified() == false || inSaveAs == true )
	{
		//#92 GetApp().SPI_GetIndexWindowByID(mWindowID).SPI_ShowSaveWindow();
		if( mViewIDArray.GetItemCount() > 0 )
		{
			AView_Index::GetIndexViewByViewID(mViewIDArray.Get(0)).SPI_ShowSaveWindow();
		}
		//��U�I���B�t�@�C���ۑ��_�C�A���O����OK�̏ꍇ�AFile�����A��������ēxSPI_Save(false)���R�[�������B
		return;
	}
	
	AText	text;
	text.Add(kUChar_LineEnd);
	for( AIndex groupIndex = 0; groupIndex < SPI_GetGroupCount(); groupIndex++ )
	{
		text.AddCstring("Group");
		if( mIndexGroup_Expanded.Get(groupIndex) == true )
		{
			text.Add('E');
		}
		text.Add(kUChar_LineEnd);
		AText	title1, title2;
		mIndexGroup.GetObject(groupIndex).GetGroupTitleText(title1,title2);
		text.AddText(title1);
		text.Add(kUChar_Tab);//#220 �^�C�g���Q���A�ۑ��t�@�C�����畜���ł��Ȃ������C��
		text.AddText(title2);
		text.Add(kUChar_LineEnd);
		for( AIndex itemIndex = 0; itemIndex < mIndexGroup.GetObject(groupIndex).GetItemCount(); itemIndex++ )
		{
			AText	filepath, hit, prehit, posthit, positiontext;
			ANumber	spos, len, para;
			AText	comment;
			mIndexGroup.GetObject(groupIndex).Get(itemIndex,filepath,hit,prehit,posthit,positiontext,spos,len,para,comment);
			//#220 comment����Ȃ�preHit, Hit, postHit�������������̂ɂ���
			if( comment.GetItemCount() == 0 )
			{
				comment.SetText(prehit);
				comment.AddText(hit);
				comment.AddText(posthit);
			}
			
			AFileAcc	file;
			file.Specify(filepath);
			if( file.Exist() == true )
			{
				AText	path;
				file.GetPath(path,kFilePathType_2);
				text.AddCstring("file://");
				text.AddText(path);
				text.Add(kUChar_LineEnd);
				text.AddFormattedCstring("***%d %d %d %d %d ",para,spos,spos+len,prehit.GetItemCount(),prehit.GetItemCount()+hit.GetItemCount());
				AText	h;
				h.SetText(prehit);
				h.AddText(hit);
				h.AddText(posthit);
				h.ReplaceChar(kUChar_LineEnd,0x1B);
				text.AddText(h);
				text.Add(kUChar_LineEnd);
			}
			else
			{
				text.AddCstring("Comment\r");
				text.AddText(comment);
				text.Add(kUChar_LineEnd);
			}
		}
	}
	AFileAcc	file;
	NVI_GetFile(file);
	file.WriteFile(text);
	NVI_SetDirty(false);
	
	/*#1034
#if IMPLEMENTATION_FOR_MACOSX
	//Ver2.1.8a7�ȑO�ŊJ����悤�ɂ��邽��
	FSRef	ref;
	file.GetFSRef(ref);
	{
		HFSUniStr255	forkName;
		::FSGetResourceForkName(&forkName);
		::FSCreateResourceFork(&ref,forkName.length,forkName.unicode,0);
	}
	short	rRefNum = ::FSOpenResFile(&ref,fsWrPerm);
	if( rRefNum != -1 )
	{
		WritePrefVersion(rRefNum);
		::CloseResFile(rRefNum);
	}
#endif
	*/
}

#pragma mark ===========================

#pragma mark <�C���^�[�t�F�C�X>

#pragma mark ===========================

#pragma mark ---�\���p�f�[�^�擾

//
AItemCount	ADocument_IndexWindow::SPI_GetDisplayRowCount() const
{
	AItemCount	rowCount = 0;
	for( AIndex i = 0; i < mIndexGroup.GetItemCount(); i++ )
	{
		rowCount++;
		if( mIndexGroup_Expanded.Get(i) == true )
		{
			rowCount += mIndexGroup.GetObjectConst(i).GetItemCount();
		}
	}
	return rowCount;
}

//
void	ADocument_IndexWindow::SPI_GetDisplayRowData( const AIndex inDisplayRowIndex, 
		AText& outFilePath, AText& outHitText, AText& outPreHitText, AText& outPostHitText, AText& outPositionText, 
		ANumber& outStartPosition, ANumber& outLength, ANumber& outParagraph, AIndex& outGroupIndex, ABool& outIsGroup, AText& outComment ) const
{
	AIndex	itemIndex;
	GetGroupItemIndexFromDisplayRow(inDisplayRowIndex,outGroupIndex,itemIndex);
	mIndexGroup.GetObjectConst(outGroupIndex).
			Get(itemIndex,outFilePath,outHitText,outPreHitText,outPostHitText,outPositionText,outStartPosition,outLength,outParagraph,outComment);
	outIsGroup = (itemIndex == kIndex_Invalid);
}

//�\���s�C���f�b�N�X����A�O���[�vIndex, ����Index�𓾂�B
void	ADocument_IndexWindow::GetGroupItemIndexFromDisplayRow( const AIndex inDisplayRowIndex, AIndex& outGroupIndex, AIndex& outItemIndex ) const
{
	outGroupIndex = kIndex_Invalid;
	outItemIndex = kIndex_Invalid;
	AItemCount	rowCount = 0;
	for( AIndex i = 0; i < mIndexGroup.GetItemCount(); i++ )
	{
		if( rowCount == inDisplayRowIndex )
		{
			//�O���[�v�^�C�g���s
			outGroupIndex = i;
			outItemIndex = kIndex_Invalid;
			return;
		}
		rowCount++;
		if( mIndexGroup_Expanded.Get(i) == true )
		{
			rowCount += mIndexGroup.GetObjectConst(i).GetItemCount();
		}
		if( inDisplayRowIndex < rowCount )
		{
			//�O���[�v������
			outGroupIndex = i;
			outItemIndex = inDisplayRowIndex - (rowCount-mIndexGroup.GetObjectConst(i).GetItemCount());
			return;
		}
	}
}

//�O���[�vIndex, ����Index����A�\���s�C���f�b�N�X�𓾂�B
void	ADocument_IndexWindow::GetDisplayRowIndexFromGroupItemIndex( const AIndex inGroupIndex, const AIndex inItemIndex, AIndex& outDisplayRowIndex ) const
{
	AItemCount	rowCount = 0;
	for( AIndex i = 0; i < inGroupIndex; i++ )
	{
		rowCount++;
		if( mIndexGroup_Expanded.Get(i) == true )
		{
			rowCount += mIndexGroup.GetObjectConst(i).GetItemCount();
		}
	}
	if( inItemIndex == kIndex_Invalid )
	{
		outDisplayRowIndex = rowCount;
	}
	else
	{
		outDisplayRowIndex = rowCount+1+inItemIndex;
	}
}

#pragma mark ===========================

#pragma mark ---Document�f�[�^�ݒ�

//�O���[�v�ǉ��i�}���j
void	ADocument_IndexWindow::SPI_InsertGroup( const AIndex inIndex, const AText& inGroupTitleText1, const AText& inGroupTitleText2,
		const AFileAcc& inBaseFolder )//#465
{
	//#895
	//�O���[�v�������B16��葽���O���[�v���͍폜����B�i�h�L�������g�ҏW���̈ʒu�X�VUpdate_TextDocumentEdited()�̑��x��x�����Ȃ��悤�ɂ��邽�߁j
	if( mIndexGroup.GetItemCount() >= kLimit_IndexGroupMax )
	{
		SPI_DeleteGroup(mIndexGroup.GetItemCount()-1);
	}
	
	//
	mIndexGroup.InsertNew1Object(inIndex);
	mIndexGroup_Expanded.Insert1(inIndex,true);
	mIndexGroup.GetObject(inIndex).SetGroupTitleText(inGroupTitleText1,inGroupTitleText2);
	mIndexGroup.GetObject(inIndex).SetGroupBaseFolder(inBaseFolder);//#465
	
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.SPI_Update_RowsInserted->view.SPI_UpdateImageSize()+view.SPI_UpdateRows()
	//#92 GetWindow().SPI_Update_RowsInserted(inIndex,1);
	//#92 GetWindow().SPI_ScrollToHome();
	
	//IndexView�̕\���X�V #92
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateRows(inIndex,SPI_GetDisplayRowCount());
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_ScrollToHome();
	}
	
	if( mAskSavingMode == true )
	{
		NVI_SetDirty(true);
	}
}
//�O���[�v�ǉ�
void	ADocument_IndexWindow::SPI_AddGroup( const AText& inGroupTitleText1, const AText& inGroupTitleText2, 
		const AFileAcc& inBaseFolder )//#465
{
	SPI_InsertGroup(mIndexGroup.GetItemCount(),inGroupTitleText1,inGroupTitleText2,inBaseFolder);//#465
}

//�O���[�v�����ڒǉ�
void	ADocument_IndexWindow::SPI_InsertItems_TextPosition( const AIndex inGroupIndex, const AIndex inItemIndex, const ATextArray& inFilePath, 
		const ATextArray& inHitText, const ATextArray& inPreHitText, const ATextArray& inPostHitText, const ATextArray& inPositionText, 
		const ATextArray& inParagraphText,//#465
		const ANumberArray& inStartPosition, const ANumberArray& inLength, const ANumberArray& inParagraph, const AText& inComment )
{
	mIndexGroup.GetObject(inGroupIndex).
			InsertItems_TextPosition(inItemIndex,inFilePath,inHitText,inPreHitText,inPostHitText,inPositionText,
			inParagraphText,//#465
			inStartPosition,inLength,inParagraph,inComment);
	AIndex	rowIndex;
	GetDisplayRowIndexFromGroupItemIndex(inGroupIndex,inItemIndex,rowIndex);
	
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.SPI_Update_RowsInserted->view.SPI_UpdateImageSize()+view.SPI_UpdateRows()
	//#92 GetWindow().SPI_Update_RowsInserted(rowIndex,inFilePath.GetItemCount());
	
	//IndexView�̕\���X�V #92
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateRows(rowIndex,SPI_GetDisplayRowCount());
	}
	
	if( mAskSavingMode == true )
	{
		NVI_SetDirty(true);
	}
}

void	ADocument_IndexWindow::SPI_InsertItem_TextPosition( const AIndex inGroupIndex, const AIndex inItemIndex, const AText& inFilePath,
		const AText& inHitText, const AText& inPreHitText, const AText& inPostHitText, const AText& inPositionText,
		const AText& inParagraphText,//#465
		const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph, const AText& inComment )
{
	mIndexGroup.GetObject(inGroupIndex).
			InsertItem_TextPosition(inItemIndex,inFilePath,inHitText,inPreHitText,inPostHitText,inPositionText,
			inParagraphText,//#465
			inStartPosition,inLength,inParagraph,inComment);
	AIndex	rowIndex;
	GetDisplayRowIndexFromGroupItemIndex(inGroupIndex,inItemIndex,rowIndex);
	
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.SPI_Update_RowsInserted->view.SPI_UpdateImageSize()+view.SPI_UpdateRows()
	//#92 GetWindow().SPI_Update_RowsInserted(rowIndex,1);
	
	//IndexView�̕\���X�V #92
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateRows(rowIndex,SPI_GetDisplayRowCount());
	}
	
	if( mAskSavingMode == true )
	{
		NVI_SetDirty(true);
	}
}

void	ADocument_IndexWindow::SPI_SetItem_TextPosition( const AIndex inGroupIndex, const AIndex inItemIndex, const AText& inFilePath,
		const AText& inHitText, const AText& inPreHitText, const AText& inPostHitText, const AText& inPositionText,
		const AText& inParagraphText,//#465
		const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph, const AText& inComment )
{
	mIndexGroup.GetObject(inGroupIndex).
			SetItem_TextPosition(inItemIndex,inFilePath,inHitText,inPreHitText,inPostHitText,inPositionText,
			inParagraphText,//#465
			inStartPosition,inLength,inParagraph,inComment);
	AIndex	rowIndex;
	GetDisplayRowIndexFromGroupItemIndex(inGroupIndex,inItemIndex,rowIndex);
	
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.SPI_RefreshRow->view.SPI_UpdateRows
	//#92 GetWindow().SPI_RefreshRow(rowIndex);
	
	//IndexView�̕\���X�V #92
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateRows(rowIndex,rowIndex+1);
	}
	
	if( mAskSavingMode == true )
	{
		NVI_SetDirty(true);
	}
}

void	ADocument_IndexWindow::SPI_AddItems_ToFirstGroup_TextPosition( const ATextArray& inFilePath, 
		const ATextArray& inHitText, const ATextArray& inPreHitText, const ATextArray& inPostHitText, const ATextArray& inPositionText, 
		const ATextArray& inParagraphText,//#465
		const ANumberArray& inStartPosition, const ANumberArray& inLength, const ANumberArray& inParagraph, const AText& inComment )
{
	SPI_InsertItems_TextPosition(0,mIndexGroup.GetObject(0).GetItemCount(),
			inFilePath,inHitText,inPreHitText,inPostHitText,inPositionText,
			inParagraphText,//#465
			inStartPosition,inLength,inParagraph,inComment);
}

void	ADocument_IndexWindow::SPI_AddItem_TextPosition( const AIndex inGroupIndex, const AText& inFilePath,
		const AText& inHitText, const AText& inPreHitText, const AText& inPostHitText, const AText& inPositionText,
		const AText& inParagraphText,//#465
		const ANumber inStartPosition, const ANumber inLength, const ANumber inParagraph, const AText& inComment )
{
	AIndex	itemIndex = mIndexGroup.GetObject(inGroupIndex).GetItemCount();
	mIndexGroup.GetObject(inGroupIndex).
			InsertItem_TextPosition(itemIndex,
			inFilePath,inHitText,inPreHitText,inPostHitText,inPositionText,
			inParagraphText,//#465
			inStartPosition,inLength,inParagraph,inComment);
	AIndex	rowIndex;
	GetDisplayRowIndexFromGroupItemIndex(inGroupIndex,itemIndex,rowIndex);
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.SPI_Update_RowsInserted->view.SPI_UpdateImageSize()+view.SPI_UpdateRows()
	//#92 GetWindow().SPI_Update_RowsInserted(rowIndex,1);
	//IndexView�̕\���X�V #92
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateRows(rowIndex,SPI_GetDisplayRowCount());
	}
}

void	ADocument_IndexWindow::SPI_ExpandCollapse( const AIndex inGroupIndex )
{
	mIndexGroup_Expanded.Set(inGroupIndex,mIndexGroup_Expanded.Get(inGroupIndex)==false);
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.NVI_UpdateProperty->view.SPI_UpdateImageSize+view.SPI_UpdateTextDrawProperty
	//#92 GetWindow().NVI_UpdateProperty();
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateTextDrawProperty();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).NVI_Refresh();
	}
}

void	ADocument_IndexWindow::SPI_Expand( const AIndex inGroupIndex )
{
	mIndexGroup_Expanded.Set(inGroupIndex,true);
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.NVI_UpdateProperty->view.SPI_UpdateImageSize+view.SPI_UpdateTextDrawProperty
	//#92 GetWindow().NVI_UpdateProperty();
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateTextDrawProperty();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).NVI_Refresh();
	}
}

void	ADocument_IndexWindow::SPI_Collapse( const AIndex inGroupIndex )
{
	mIndexGroup_Expanded.Set(inGroupIndex,false);
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.NVI_UpdateProperty->view.SPI_UpdateImageSize+view.SPI_UpdateTextDrawProperty
	//#92 GetWindow().NVI_UpdateProperty();
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateTextDrawProperty();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).NVI_Refresh();
	}
}

/**
�ŏ��̃O���[�v�����肽���ށi�}���`�t�@�C�������J�n���ɑO�񌟍��������I�ɂ��肽���ނ��߁j
*/
void	ADocument_IndexWindow::SPI_CollapseFirstGroup()
{
	if( SPI_GetGroupCount() > 0 )
	{
		SPI_Collapse(0);
	}
}

ABool	ADocument_IndexWindow::SPI_IsGroupExpanded( const AIndex inIndex ) const
{
	return mIndexGroup_Expanded.Get(inIndex);
}

void	ADocument_IndexWindow::SPI_SelectGroup( const AIndex inGroupIndex )
{
	AIndex	displayRowIndex;
	GetDisplayRowIndexFromGroupItemIndex(inGroupIndex,kIndex_Invalid,displayRowIndex);
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.SPI_SelectRow->view.SPI_SelectRow
	//#92 GetWindow().SPI_SelectRow(displayRowIndex);
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_SelectRow(displayRowIndex);
	}
}

void	ADocument_IndexWindow::SPI_Select( const AIndex inGroupIndex, const AIndex inItemIndex )
{
	AIndex	displayRowIndex;
	GetDisplayRowIndexFromGroupItemIndex(inGroupIndex,inItemIndex,displayRowIndex);
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.SPI_SelectRow->view.SPI_SelectRow
	//#92 GetWindow().SPI_SelectRow(displayRowIndex);
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_SelectRow(displayRowIndex);
	}
}

void	ADocument_IndexWindow::NVIDO_GetTitle( AText& outTitle ) const
{
	outTitle.SetText(mTitle);
}

//���j���[�R�}���h����
ABool	ADocument_IndexWindow::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	
	return result;
}

void	ADocument_IndexWindow::EVTDO_UpdateMenu()
{
}

/*#92
void	ADocument_IndexWindow::SPI_SelectFirstWindow()
{
	GetWindow().NVI_SelectWindow();
}
*/

void	ADocument_IndexWindow::SPI_DeleteItem( const AIndex inGroupIndex, const AIndex inItemIndex )
{
	mIndexGroup.GetObject(inGroupIndex).DeleteItem(inItemIndex);
}

void	ADocument_IndexWindow::SPI_DeleteAllItemsInGroup( const AIndex inGroupIndex )
{
	while( mIndexGroup.GetObject(inGroupIndex).GetItemCount() > 0 )
	{
		mIndexGroup.GetObject(inGroupIndex).DeleteItem(mIndexGroup.GetObject(inGroupIndex).GetItemCount()-1);
	}
}

void	ADocument_IndexWindow::SPI_DeleteGroup( const AIndex inGroupIndex )
{
	mIndexGroup.Delete1Object(inGroupIndex);
	mIndexGroup_Expanded.Delete1(inGroupIndex);
	
	//IndexView�̕\���X�V #725
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateImageSize();
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateRows(0,SPI_GetDisplayRowCount());
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).NVI_Refresh();
	}
	
}

//
void	ADocument_IndexWindow::SPI_DeleteAllGroup()
{
	while( mIndexGroup.GetItemCount() > 0 )
	{
		SPI_DeleteGroup(mIndexGroup.GetItemCount()-1);
	}
}

void	ADocument_IndexWindow::SPI_GetGroupTitle( const AIndex inGroupIndex, AText& outTitle1, AText& outTitle2 ) const
{
	mIndexGroup.GetObjectConst(inGroupIndex).GetGroupTitleText(outTitle1,outTitle2);
}

void	ADocument_IndexWindow::SPI_SetGroupTitle( const AIndex inGroupIndex, const AText& inGroupTitleText1, const AText& inGroupTitleText2 )
{
	mIndexGroup.GetObject(inGroupIndex).SetGroupTitleText(inGroupTitleText1,inGroupTitleText2);
	SPI_RefreshRow(inGroupIndex,kIndex_Invalid);
	if( mAskSavingMode == true )
	{
		NVI_SetDirty(true);
	}
}

//#465
/**
*/
void	ADocument_IndexWindow::SPI_GetGroupBaseFolder( const AIndex inGroupIndex, AFileAcc& outBaseFolder ) const
{
	mIndexGroup.GetObjectConst(inGroupIndex).GetGroupBaseFolder(outBaseFolder);
}

void	ADocument_IndexWindow::SPI_SetItemComment( const AIndex inGroupIndex, const AIndex inItemIndex, const AText& inComment )
{
	mIndexGroup.GetObject(inGroupIndex).SetComment(inItemIndex,inComment);
	SPI_RefreshRow(inGroupIndex,inItemIndex);
	if( mAskSavingMode == true )
	{
		NVI_SetDirty(true);
	}
}

void	ADocument_IndexWindow::SPI_SetItemFile( const AIndex inGroupIndex, const AIndex inItemIndex, const AFileAcc& inFile )
{
	AText	path;
	inFile.GetPath(path);
	mIndexGroup.GetObject(inGroupIndex).SetFilePath(inItemIndex,path);
	SPI_RefreshRow(inGroupIndex,inItemIndex);
	if( mAskSavingMode == true )
	{
		NVI_SetDirty(true);
	}
}

#pragma mark ===========================

#pragma mark ---�e�L�X�g�ʒu�␳

void	ADocument_IndexWindow::SPI_Update_TextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	for( AIndex i = 0; i < mIndexGroup.GetItemCount(); i++ )
	{
		mIndexGroup.GetObject(i).Update_TextDocumentEdited(inFile,inTextIndex,inInsertedCount);
	}
}

#pragma mark ===========================

#pragma mark ---�O���[�v�f�[�^�擾
//#465

/**
*/
void	ADocument_IndexWindow::SPI_GetItemData( const AIndex inGroupIndex, const AIndex inItemIndex, 
		AText& outFilePath, AText& outHitText, 
		AText& outPreHitText, AText& outPostHitText, AText& outPositionText, 
		AText& outParagraphText,
		ANumber& outStartPosition, ANumber& outLength, ANumber& outParagraph, AText& outComment ) const
{
	mIndexGroup.GetObjectConst(inGroupIndex).
			Get(inItemIndex,outFilePath,outHitText,outPreHitText,outPostHitText,outPositionText,outStartPosition,outLength,outParagraph,outComment);
	mIndexGroup.GetObjectConst(inGroupIndex).
			GetParagraphText(inItemIndex,outParagraphText);
}

#pragma mark ===========================

#pragma mark ---

void	ADocument_IndexWindow::SPI_RefreshRow( const AIndex inGroupIndex, const AIndex inItemIndex )
{
	AIndex	displayRowIndex;
	GetDisplayRowIndexFromGroupItemIndex(inGroupIndex,inItemIndex,displayRowIndex);
	
	//#92 �E�C���h�E�o�R�����ɁAView�֒��ڎw������Bwindow.SPI_RefreshRow->view.SPI_UpdateRows
	//#92 GetWindow().SPI_RefreshRow(displayRowIndex);
	
	//IndexView�̕\���X�V #92
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_UpdateRows(displayRowIndex,displayRowIndex+1);
	}
	
}

/**
�E�C���h�E��TryClose����

������View�Ή�����Ƃ��͌����K�v
*/
void	ADocument_IndexWindow::SPI_TryClose()
{
	//#92 GetWindow().SPI_TryClose();
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).NVI_TryCloseWindow();
	}
}

/**
�E�C���h�E��ExecuteClose����

������View�Ή�����Ƃ��͌����K�v
*/
void	ADocument_IndexWindow::SPI_ExecuteClose()
{
	//#92 GetWindow().SPI_ExecuteClose();
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).NVI_ExecuteCloseWindow();
	}
}

void	ADocument_IndexWindow::SPI_InhibitClose( const ABool inInhibit )
{
	//#92 GetWindow().SPI_InhibitClose(inInhibit);
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).NVI_SetInhibitCloseWindow(inInhibit);
	}
}

#pragma mark ===========================

#pragma mark ---AppleScript�p�h�L�������g�f�[�^�擾

//
//inASRecordIndex: �O���[�v�^�C�g���ȊO�̃��R�[�h�̒ʂ��ԍ��i0�`�j
ABool	ADocument_IndexWindow::SPI_ASGetRecord( const AIndex inASRecordIndex, 
		AFileAcc& outFile, ATextIndex& outStartPos, AItemCount& outLength, ANumber& outparagraph, AText& outComment ) const
{
	AIndex	groupIndex, indexInGroup;
	if( SPI_GetIndexFromASRecordIndex(inASRecordIndex,groupIndex,indexInGroup) == false )   return false;
	
	AText	path, hittext, prehit, posthit, positiontext;
	mIndexGroup.GetObjectConst(groupIndex).Get(indexInGroup,path,hittext,prehit,posthit,positiontext,outStartPos,outLength,outparagraph,outComment);
	outFile.Specify(path);
	return true;
}

//
ABool	ADocument_IndexWindow::SPI_GetIndexFromASRecordIndex( const AIndex inASRecordIndex, AIndex& outGroupIndex, AIndex& outIndexInGroup ) const
{
	AItemCount	count = 0;
	for( AIndex groupIndex = 0; groupIndex < mIndexGroup.GetItemCount(); groupIndex++ )
	{
		if( inASRecordIndex-count < mIndexGroup.GetObjectConst(groupIndex).GetItemCount() )
		{
			outGroupIndex = groupIndex;
			outIndexInGroup = inASRecordIndex-count;
			return true;
		}
		count += mIndexGroup.GetObjectConst(groupIndex).GetItemCount();
	}
	return false;
}

//
ABool	ADocument_IndexWindow::SPI_GetASRecordIndex( const AIndex inGroupIndex, const AIndex inItemIndex, AIndex& outASRecordIndex ) const
{
	if( inGroupIndex < 0 || inGroupIndex >= mIndexGroup.GetItemCount() )   return false;
	if( inItemIndex < 0 || inItemIndex >= mIndexGroup.GetObjectConst(inGroupIndex).GetItemCount() )   return false;
	AItemCount	count = 0;
	for( AIndex groupIndex = 0; groupIndex < inGroupIndex; groupIndex++ )
	{
		count += mIndexGroup.GetObjectConst(groupIndex).GetItemCount();
	}
	count += inItemIndex;
	outASRecordIndex = count;
	return true;
}

#pragma mark ===========================

#pragma mark ---AppleScript�p
//#92

/**
Column���擾(AS�p)

*/
void	ADocument_IndexWindow::SPI_ASGetColumnWidth( ANumber& outFileColumnWidth, ANumber& outInfoColumnWidth ) const
{
	outFileColumnWidth = 0;
	outInfoColumnWidth = 0;
	if( mViewIDArray.GetItemCount() == 0 )   return;
	
	AView_Index::GetIndexViewConstByViewID(mViewIDArray.Get(0)).SPI_GetColumnWidth(outFileColumnWidth,outInfoColumnWidth);
}

/**
Column���ݒ�(AS�p)

*/
void	ADocument_IndexWindow::SPI_ASSetFileColumnWidth( const ANumber inNewWidth )
{
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_SetFileColumnWidth(inNewWidth);
	}
}

/**
Column���ݒ�(AS�p)

*/
void	ADocument_IndexWindow::SPI_ASSetInfoColumnWidth( const ANumber inNewWidth )
{
	for( AIndex i = 0; i < mViewIDArray.GetItemCount(); i++ )
	{
		AView_Index::GetIndexViewByViewID(mViewIDArray.Get(i)).SPI_SetInfoColumnWidth(inNewWidth);
	}
}

/**
WindowID�擾(AS�p)

*/
AWindowID	ADocument_IndexWindow::SPI_ASGetWindowID() const
{
	if( mViewIDArray.GetItemCount() == 0 )   return kObjectID_Invalid;
	
	return AView_Index::GetIndexViewConstByViewID(mViewIDArray.Get(0)).NVI_GetWindowID();
}


