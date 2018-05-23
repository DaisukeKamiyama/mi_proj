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

ASCMFolderManager
R0006
*/

#include "stdafx.h"

#include "ASCMFolderManager.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [�N���X]ASCMFolderData
#pragma mark ===========================
//�e�t�H���_���̃f�[�^��ێ�����N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
ASCMFolderData::ASCMFolderData( AObjectArrayItem* inParent, const AFileAcc& inFolder, const ABool inIsProjectSubFolder ) : 
		AObjectArrayItem(NULL), mSCMFolderType(kSCMFolderType_NotSCMFolder), mNowUpdatingStatus(false)
,mIsProjectSubFolder(inIsProjectSubFolder)
{
	mFolder.CopyFrom(inFolder);
	
	//#961
	//SCM�t�H���_�^�C�v����
	FindSCMConfigFolderRecursive(inFolder);
	
	/*#961
	AFileAcc	cvsroot;
	cvsroot.SpecifyChild(inFolder,"CVS/Root");
	//R0247
	AFileAcc	svnroot;
	svnroot.SpecifyChild(inFolder,".svn/entries");
	//svn��entries�t�@�C�������݂�����Asvn�t�H���_�Ƃ݂Ȃ��isvn�D��j
	if( svnroot.Exist() == true )
	{
		AText	text;
		svnroot.ReadTo(text);
		if( text.GetItemCount() > 0 )
		{
			AText	line;
			AIndex	pos = 0;
			//5�s��
			text.GetLine(pos,line);
			text.GetLine(pos,line);
			text.GetLine(pos,line);
			text.GetLine(pos,line);
			text.GetLine(pos,mRepository);
			mSCMFolderType = kSCMFolderType_SVN;
		}
	}
	*/
	/*#526 CVS���Ή�
	else
	if( cvsroot.Exist() == true )
	{
		AText	text;
		cvsroot.ReadTo(text);
		if( text.GetItemCount() > 0 )
		{
			if( text.Get(0) != ':' )//�܂��̓��[�J���̂ݑΉ�����
			{
				AIndex	pos = 0;
				text.GetLine(pos,mRepository);
				mSCMFolderType = kSCMFolderType_CVS;
			}
		}
	}
	*/
	
	//#402 �R�[������RegisterFolder()/RegisterFolders()�Ŏ��s UpdateStatus();
}
//�f�X�g���N�^
ASCMFolderData::~ASCMFolderData()
{
}

//#589
/**
SCM�t�H���_�^�C�v�̔���
*/
void	ASCMFolderData::FindSCMConfigFolderRecursive( const AFileAcc& inFolder )
{
	AFileAcc	configfolder;
	//SVN�t�H���_���ǂ����𔻒�
	configfolder.SpecifyChild(inFolder,".svn");
	if( configfolder.Exist() == true )
	{
		mSCMFolderType = kSCMFolderType_SVN;
		//entries�t�H���_���烌�|�W�g���p�X�擾
		AFileAcc	svnroot;
		svnroot.SpecifyChild(inFolder,".svn/entries");
		if( svnroot.Exist() == true )
		{
			AText	text;
			svnroot.ReadTo(text);
			if( text.GetItemCount() > 0 )
			{
				AText	line;
				AIndex	pos = 0;
				//5�s��
				text.GetLine(pos,line);
				text.GetLine(pos,line);
				text.GetLine(pos,line);
				text.GetLine(pos,line);
				text.GetLine(pos,mRepository);
			}
		}
		return;
	}
	//Git�t�H���_���ǂ����𔻒�
	configfolder.SpecifyChild(inFolder,".git");
	if( configfolder.Exist() == true )
	{
		mSCMFolderType = kSCMFolderType_Git;
		mRootFolder.CopyFrom(inFolder);
		return;
	}
	
	//���݂��Ă��Ȃ���ΐe�t�H���_�Ō����i�ċA�j
	AFileAcc	parent;
	parent.SpecifyParent(inFolder);
	if( parent.IsRoot() == true )
	{
		return;
	}
	FindSCMConfigFolderRecursive(parent);
}

#pragma mark ===========================

#pragma mark --- UpdateStatus

//�X�V�icvs status���j
void	ASCMFolderData::UpdateStatus()
{
	if( mSCMFolderType == kSCMFolderType_NotSCMFolder )   return;
	
	mFileNameArray.DeleteAll();
	mFileStateArray.DeleteAll();
	mFileStateTextArray.DeleteAll();
	
	switch(mSCMFolderType)
	{
	  case kSCMFolderType_CVS:
		{
			AText	command;
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kCVSPath,command);
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			t.SetCstring("-d");
			argArray.Add(t);
			argArray.Add(mRepository);
			t.SetCstring("status");
			argArray.Add(t);
			t.SetCstring("-l");
			argArray.Add(t);
			AText	dirpath;
			mFolder.GetNormalizedPath(dirpath);
			GetApp().SPI_GetChildProcessManager().ExecuteSCMCommand(command,argArray,dirpath,kSCMCommandType_Status,mFolder,kObjectID_Invalid);
			mNowUpdatingStatus = true;
			break;
		}
		//R0247
	  case kSCMFolderType_SVN:
		{
			//�R�}���h�Ƃ��Ă�env�����s�B���{��t�@�C������svn�ɓn�����߁B #1087
			AText	command;
			command.SetCstring("/usr/bin/env");
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			t.SetCstring("LANG=UTF-8");
			argArray.Add(t);
			//svn�R�}���h�p�X
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kSVNPath,t);
			argArray.Add(t);
			//����
			t.SetCstring("status");
			argArray.Add(t);
			//�J�����g�f�B���N�g��
			AText	dirpath;
			mFolder.GetNormalizedPath(dirpath);
			//���s
			GetApp().SPI_GetChildProcessManager().ExecuteSCMCommand(command,argArray,dirpath,kSCMCommandType_Status,mFolder,kObjectID_Invalid);
			mNowUpdatingStatus = true;
			break;
		}
		//#589
	  case kSCMFolderType_Git:
		{
			//�e�X�g�����Ƃ���ASVN�Ɠ��l��#1087�Ή��͕K�v�Ȃ������B���{��t�@�C�����ł����Ȃ����삵�Ă���B141207
			//git�R�}���h�p�X
			AText	command;
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kGitPath,command);
			//����
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			t.SetCstring("status");
			argArray.Add(t);
			t.SetCstring("--short");
			argArray.Add(t);
			t.SetCstring("-z");//���s��NULL�ŏo��
			argArray.Add(t);
			//�J�����g�f�B���N�g��
			AText	dirpath;
			mFolder.GetNormalizedPath(dirpath);
			//���s
			GetApp().SPI_GetChildProcessManager().ExecuteSCMCommand(command,argArray,dirpath,kSCMCommandType_Status,mFolder,kObjectID_Invalid);
			mNowUpdatingStatus = true;
			break;
		}
	  default:
		{
			//��������
			break;
		}
	}
}

void	ASCMFolderData::DoStatusCommandResult( const AText& inText )
{
	/*
	AText	path;
	mFolder.GetPath(path);
	path.OutputToStderr();
	inText.OutputToStderr();
	AText	t("\r");
	t.OutputToStderr();
	*/
	AUChar	pathDelimiter = AFileAcc::GetPathDelimiter();//win
	mNowUpdatingStatus = false;
	switch(mSCMFolderType)
	{
	  case kSCMFolderType_CVS:
		{
			for( AIndex pos = 0; pos < inText.GetItemCount(); )
			{
				AText	line;
				inText.GetLine(pos,line);
				if( line.GetItemCount() > 5 )
				{
					if( line.CompareMin("File: no file") == true )   continue;
					if( line.CompareMin("File: ") == true )
					{
						AIndex	linepos = 6;
						AIndex	spos = linepos;
						for( ; linepos < line.GetItemCount(); linepos++ )
						{
							AUChar	ch = line.Get(linepos);
							if( ch == kUChar_Tab || ch == kUChar_Space )   break;
						}
						AText	filename;
						line.GetText(spos,linepos-spos,filename);
						if( line.FindCstring(linepos,"Status: ",linepos) == true )
						{
							linepos += 8;
							AText	statustext;
							line.GetText(linepos,line.GetItemCount()-linepos,statustext);
							filename.ConvertToCanonicalComp();
							//#402 ���ֈړ� mFileNameArray.Add(filename);
							//#402 ���ֈړ� mFileStateTextArray.Add(statustext);
							ASCMFileState	filestate = kSCMFileState_Others;
							if( statustext.CompareMin("Up-to-date") == true )
							{
								filestate = kSCMFileState_UpToDate;
							}
							else if( statustext.CompareMin("Locally Modified") == true )
							{
								filestate = kSCMFileState_Modified;
							}
							else if( statustext.CompareMin("Locally Added") == true )
							{
								filestate = kSCMFileState_Added;
							}
							//filename����'/'������Ƃ��́A�T�u�t�H���_����status�Ƃ݂Ȃ��āA�ʂ�SCMFolderData��set���� #402
							if( filename.IsCharContained(/*win kUChar_Slash*/pathDelimiter) == true )
							{
								AFileAcc	file;
								file.SpecifyChild(mFolder,filename);
								GetApp().SPI_GetSCMFolderManager().SetStatus(file,statustext,filestate);
							}
							//
							else
							{
								mFileNameArray.Add(filename);//#402
								mFileStateTextArray.Add(statustext);//#402
								mFileStateArray.Add(filestate);
							}
						}
					}
				}
			}
			GetApp().SPI_SCMFolderDataUpdated(mFolder);
			break;
		}
		//R0247
	  case kSCMFolderType_SVN:
		{
			for( AIndex pos = 0; pos < inText.GetItemCount(); )
			{
				//1�s�擾
				AText	line;
				inText.GetLine(pos,line);
				if( line.GetItemCount() > 1 )
				{
					//�t�@�C�����擾
					AIndex	linepos = 1;
					line.SkipTabSpace(linepos,line.GetItemCount());
					AText	filename, statustext;
					line.GetText(linepos,line.GetItemCount()-linepos,filename);
					filename.ConvertToCanonicalComp();//#1087 canonical comp�`���ŋL��
					//�ŏ���1�����ɂ���ăt�@�C����Ԏ擾
					ASCMFileState	filestate = kSCMFileState_Others;
					switch(line.Get(0))
					{
					  case 'M':
						{
							filestate = kSCMFileState_Modified;
							statustext.SetCstring("Modified");
							break;
						}
					  case 'A':
						{
							filestate = kSCMFileState_Added;
							statustext.SetCstring("Added");
							break;
						}
					  case 'D':
						{
							filestate = kSCMFileState_Others;
							statustext.SetCstring("Deleted");
							break;
						}
					  case 'C':
						{
							filestate = kSCMFileState_Others;
							statustext.SetCstring("Conflict");
							break;
						}
					  case '?':
						{
							filestate = kSCMFileState_NotEntried;
							statustext.SetCstring("Not Under Version Control");
							break;
						}
					}
					//filename����'/'������Ƃ��́A�T�u�t�H���_����status�Ƃ݂Ȃ��āA�ʂ�SCMFolderData��set���� #402
					if( filename.IsCharContained(/*win kUChar_Slash*/pathDelimiter) == true )
					{
						AFileAcc	file;
						file.SpecifyChild(mFolder,filename);
						GetApp().SPI_GetSCMFolderManager().SetStatus(file,statustext,filestate);
					}
					//
					else
					{
						mFileNameArray.Add(filename);
						mFileStateTextArray.Add(statustext);
						mFileStateArray.Add(filestate);
					}
				}
			}
			GetApp().SPI_SCMFolderDataUpdated(mFolder);
			break;
		}
		//#589
	  case kSCMFolderType_Git:
		{
			//NULL���������s�ɕϊ�
			AText	text(inText);
			text.ReplaceChar(0,kUChar_LineEnd);
			//
			for( AIndex pos = 0; pos < text.GetItemCount(); )
			{
				//1�s�擾
				AText	line;
				text.GetLine(pos,line);
				if( line.GetItemCount() > 1 )
				{
					//�t�@�C�����擾
					AIndex	linepos = 2;
					line.SkipTabSpace(linepos,line.GetItemCount());
					AText	filename, statustext;
					line.GetText(linepos,line.GetItemCount()-linepos,filename);
					filename.ConvertToCanonicalComp();//#1087 canonical comp�`���ŋL��
					//2�����ڂɂ���ăt�@�C����Ԏ擾
					ASCMFileState	filestate = kSCMFileState_Others;
					switch(line.Get(1))
					{
					  case 'M':
						{
							filestate = kSCMFileState_Modified;
							statustext.SetCstring("Modified");
							break;
						}
					  case 'A':
						{
							filestate = kSCMFileState_Added;
							statustext.SetCstring("Added");
							break;
						}
					  case 'D':
						{
							filestate = kSCMFileState_Others;
							statustext.SetCstring("Deleted");
							break;
						}
					  case 'R':
						{
							filestate = kSCMFileState_Others;
							statustext.SetCstring("Renamed");
							break;
						}
					  case 'C':
						{
							filestate = kSCMFileState_Others;
							statustext.SetCstring("Copied");
							break;
						}
					  case 'U':
						{
							filestate = kSCMFileState_Others;
							statustext.SetCstring("Updated but Unmerged");
							break;
						}
					  case '?':
						{
							filestate = kSCMFileState_NotEntried;
							statustext.SetCstring("Untracked");
							break;
						}
					}
					//-z�I�v�V����������Ƃ��́A���git���[�g����̃p�X�ɂȂ�
					AFileAcc	file;
					file.SpecifyChild(mRootFolder,filename);
					GetApp().SPI_GetSCMFolderManager().SetStatus(file,statustext,filestate);
				}
			}
			GetApp().SPI_SCMFolderDataUpdated(mFolder);
			break;
		}
	  default:
		{
			//��������
			break;
		}
	}
	//#379 �h�L�������g��Diff���[�h�X�V�̂���
	GetApp().SPI_DoSCMStatusUpdated(mFolder);
}

ASCMFileState	ASCMFolderData::GetFileState( const AText& inFilename ) const
{
	if( mSCMFolderType == kSCMFolderType_NotSCMFolder )   return kSCMFileState_NotSCMFolder;
	
	//#1087
	//canonical comp�`���ɕϊ��i�����I��canonical comp�ŋL�����Ă��邽�߁j
	AText	filename(inFilename);
	filename.ConvertToCanonicalComp();
	switch(mSCMFolderType)
	{
	  case kSCMFolderType_CVS:
		{
			AIndex	index = mFileNameArray.Find(filename);
			if( index == kIndex_Invalid )
			{
				return kSCMFileState_NotEntried;
			}
			else
			{
				return mFileStateArray.Get(index);
			}
			break;
		}
		//R0247
	  case kSCMFolderType_SVN:
		{
			AIndex	index = mFileNameArray.Find(filename);
			if( index == kIndex_Invalid )
			{
				return kSCMFileState_UpToDate;
			}
			else
			{
				return mFileStateArray.Get(index);
			}
			break;
		}
		//#589
	  case kSCMFolderType_Git:
		{
			AIndex	index = mFileNameArray.Find(filename);
			if( index == kIndex_Invalid )
			{
				return kSCMFileState_UpToDate;
			}
			else
			{
				return mFileStateArray.Get(index);
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
	return kSCMFileState_NotEntried;
}

void	ASCMFolderData::GetFileStateText( const AText& inFilename, AText& outText ) const
{
	outText.DeleteAll();
	if( mNowUpdatingStatus == true )   return;
	
	//#1087
	//canonical comp�`���ɕϊ��i�����I��canonical comp�ŋL�����Ă��邽�߁j
	AText	filename(inFilename);
	filename.ConvertToCanonicalComp();
	switch(mSCMFolderType)
	{
	  case kSCMFolderType_CVS:
		{
			AIndex	index = mFileNameArray.Find(filename);
			if( index != kIndex_Invalid )
			{
				outText.AddCstring("[CVS: ");
				outText.AddText(mFileStateTextArray.GetTextConst(index));
				outText.AddCstring("]");
			}
			else
			{
				outText.AddCstring("[CVS: ");
				outText.AddLocalizedText("SCMFileStateText_NotEntried");
				outText.AddCstring("]");
			}
			break;
		}
		//R0247
	  case kSCMFolderType_SVN:
		{
			AIndex	index = mFileNameArray.Find(filename);
			if( index != kIndex_Invalid )
			{
				if( mFileStateArray.Get(index ) == kSCMFileState_NotEntried )
				{
					outText.AddCstring("[SVN: ");
					outText.AddLocalizedText("SCMFileStateText_NotEntried");
					outText.AddCstring("]");
				}
				else
				{
					outText.AddCstring("[SVN: ");
					outText.AddText(mFileStateTextArray.GetTextConst(index));
					outText.AddCstring("]");
				}
			}
			else
			{
				outText.AddCstring("[SVN: Up-to-date]");
			}
			break;
		}
		//#589
	  case kSCMFolderType_Git:
		{
			AIndex	index = mFileNameArray.Find(filename);
			if( index != kIndex_Invalid )
			{
				if( mFileStateArray.Get(index ) == kSCMFileState_NotEntried )
				{
					outText.AddCstring("[git: ");
					outText.AddLocalizedText("SCMFileStateText_NotEntried");
					outText.AddCstring("]");
				}
				else
				{
					outText.AddCstring("[git: ");
					outText.AddText(mFileStateTextArray.GetTextConst(index));
					outText.AddCstring("]");
				}
			}
			else
			{
				outText.AddCstring("[git: Up-to-date]");
			}
			break;
		}
	  case kSCMFolderType_NotSCMFolder:
	  default:
		{
			//��������
			break;
		}
	}
}

/**
SCM��Ԃ��擾
���ӁF���x���l�����AGetFileState()���ƈ���āA�֐����Ńt�@�C������ConvertToCanonicalComp()���s���Ă��Ȃ��B�R�[�����鑤��ConvertToCanonicalComp()�ς݂̔z���n�����ƁB
*/
void	ASCMFolderData::GetFileStateArray( const ATextArray& inFilenameArray, AArray<ASCMFileState>& outFileStateArray ) const
{
	outFileStateArray.DeleteAll();
	if( mSCMFolderType == kSCMFolderType_NotSCMFolder )   return;
	
	switch(mSCMFolderType)
	{
	  case kSCMFolderType_CVS:
		{
			AItemCount	itemcount = inFilenameArray.GetItemCount();
			for( AIndex i = 0; i < itemcount; i++ )
			{
				AIndex	index = mFileNameArray.Find(inFilenameArray.GetTextConst(i));
				if( index == kIndex_Invalid )
				{
					outFileStateArray.Add(kSCMFileState_NotEntried);
				}
				else
				{
					outFileStateArray.Add(mFileStateArray.Get(index));
				}
			}
			break;
		}
		//R0247
	  case kSCMFolderType_SVN:
		{
			AItemCount	itemcount = inFilenameArray.GetItemCount();
			for( AIndex i = 0; i < itemcount; i++ )
			{
				AIndex	index = mFileNameArray.Find(inFilenameArray.GetTextConst(i));
				if( index == kIndex_Invalid )
				{
					outFileStateArray.Add(kSCMFileState_UpToDate);
				}
				else
				{
					outFileStateArray.Add(mFileStateArray.Get(index));
				}
			}
			break;
		}
		//#589
	  case kSCMFolderType_Git:
		{
			AItemCount	itemcount = inFilenameArray.GetItemCount();
			for( AIndex i = 0; i < itemcount; i++ )
			{
				AIndex	index = mFileNameArray.Find(inFilenameArray.GetTextConst(i));
				if( index == kIndex_Invalid )
				{
					outFileStateArray.Add(kSCMFileState_UpToDate);
				}
				else
				{
					outFileStateArray.Add(mFileStateArray.Get(index));
				}
			}
			break;
		}
	  default:
		{
			//�����Ȃ�
			break;
		}
	}
}

/**
�ŐV�łƂ̔�r���s�i�񓯊����s�j
*/
void	ASCMFolderData::CompareWithLatest( const AFileAcc& inFile, const ADocumentID& inDocumentID )//#736
{
	AText	filename;
	inFile.GetRawFilename(filename);
	switch(mSCMFolderType)
	{
	  case kSCMFolderType_CVS:
		{
			AText	command;
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kCVSPath,command);
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			t.SetCstring("-d");
			argArray.Add(t);
			argArray.Add(mRepository);
			t.SetCstring("diff");
			argArray.Add(t);
			AFileAcc::GetNormalizedPathString(filename);//#1087
			argArray.Add(filename);
			AText	dirpath;
			mFolder.GetNormalizedPath(dirpath);
			AText	resulttext;
			GetApp().SPI_GetChildProcessManager().ExecuteSCMCommand(command,argArray,dirpath,kSCMCommandType_Compare,inFile,inDocumentID);//#736
			break;
		}
		//R0247
	  case kSCMFolderType_SVN:
		{
			//�R�}���h�Ƃ��Ă�env�����s�B���{��t�@�C������svn�ɓn�����߁B #1087
			AText	command;
			command.SetCstring("/usr/bin/env");
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			t.SetCstring("LANG=UTF-8");
			argArray.Add(t);
			//svn�R�}���h�p�X
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kSVNPath,t);
			argArray.Add(t);
			//����
			t.SetCstring("diff");
			argArray.Add(t);
			/*#379 ����svn diff�̓��|�W�g���e�L�X�g�𐶐����邽�߂����ɂ����̂ŁA�O��diff���g���K�v�Ȃ�
			if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseExternalDiffForSubversion) == true )//#335
			{
				//�O��diff�w��
				t.SetCstring("--diff-cmd");
				argArray.Add(t);
				GetApp().GetAppPref().GetData_Text(AAppPrefDB::kDiffPath,t);
				argArray.Add(t);
				//diff�I�v�V�����w��
				t.SetCstring("-x");
				argArray.Add(t);
				GetApp().GetAppPref().GetData_Text(AAppPrefDB::kDiffOption,t);//�_�u���N�I�[�e�[�V�����̓V�F���o�R�ł͂Ȃ��̂ŕs�v
				if( t.GetItemCount() > 0 )
				{
					t.Add(' ');
				}
				t.AddCstring("--normal");
				argArray.Add(t);
			}
			*/
			//�t�@�C����
			AFileAcc::GetNormalizedPathString(filename);//#1087
			argArray.Add(filename);
			//�J�����g�f�B���N�g��
			AText	dirpath;
			mFolder.GetNormalizedPath(dirpath);
			//���s
			AText	resulttext;
			GetApp().SPI_GetChildProcessManager().ExecuteSCMCommand(command,argArray,dirpath,kSCMCommandType_Compare,inFile,inDocumentID);//#736
			break;
		}
		//#589
	  case kSCMFolderType_Git:
		{
			//�e�X�g�����Ƃ���ASVN�Ɠ��l��#1087�Ή��͕K�v�Ȃ������B���{��t�@�C�����ł����Ȃ����삵�Ă���B141207
			//svn�R�}���h�p�X
			AText	command;
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kGitPath,command);
			//����
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			t.SetCstring("diff");
			argArray.Add(t);
			//�t�@�C����
			AFileAcc::GetNormalizedPathString(filename);//#1087
			argArray.Add(filename);
			//�J�����g�f�B���N�g��
			AText	dirpath;
			mFolder.GetNormalizedPath(dirpath);
			//���s
			AText	resulttext;
			GetApp().SPI_GetChildProcessManager().ExecuteSCMCommand(command,argArray,dirpath,kSCMCommandType_Compare,inFile,inDocumentID);//#736
			break;
		}
	  default:
		{
			//��������
			break;
		}
	}
}

#if 0
/*
UnifiedDiff�`��
Index: AApp.cpp
===================================================================
--- AApp.cpp	(revision 2247)
+++ AApp.cpp	(working copy)
@@ -5632,7 +5649,7 @@
 	SPI_GetMultiFileFindWindow().NVI_SwitchFocusToFirst();
 }
 
-//�u�ŋߎg����?�v�ɒǉ�
+//�u�ŋߎg����?�v�ɒǉ�
 void	AApp::SPI_AddRecentlyUsed( const AText& inFindText, const AFileAcc& inFolder )
 {
 	SPI_GetMultiFileFindWindow().SPI_AddRecentlyUsedFindText(inFindText);

��diff�`��
Index: AView_EditBox.cpp
===================================================================
70a71
> 	GetTextDocument().NVI_RegisterView(GetObjectID());
141c142,143
< 	AApplication::GetApplication().NVI_DeleteDocument(mDocumentID);
---
> 	//#379 AApplication::GetApplication().NVI_DeleteDocument(mDocumentID);
> 	GetTextDocument().NVI_UnregisterView(GetObjectID());
*/

/**
SCM��r�R�}���h���s�㏈��
AChildProcessManager::DoPipeDeleted_SCM()����R�[�������B
*/
void	ASCMFolderData::DoCompareResult( const AFileAcc& inFile, const AText& inText )
{
	/*#379 SPI_ParseDiffText()�ֈړ�
	AArray<ADiffType>	diffTypeArray;
	AArray<AIndex>	startParagraphIndexArray;
	AArray<AIndex>	endParagraphIndexArray;
	AArray<AIndex>	oldStartParagraphIndexArray;//#379
	AArray<AIndex>	oldEndParagraphIndexArray;//#379
	ATextArray	deletedTextArray;//#379
	//#379 �����e�L�X�g���e�Ō`�����f����悤�ɕύXif( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseExternalDiffForSubversion) == false )//#335
	ABool	unifiedDiffFormat = false;
	AIndex	pos = 0;
	AText	line;
	AItemCount	lineCount = 0;
	for( ; pos < inText.GetItemCount(); )
	{
		inText.GetLine(pos,line);
		if( line.GetItemCount() == 0 )   continue;
		if( line.Get(0) == '@' )
		{
			//@�Ŏn�܂�s�������UnifiedDiff�`���Ƃ݂Ȃ�
			unifiedDiffFormat = true;
			break;
		}
		//�ŏ�����16�s�ڂ܂ł��`�F�b�N
		lineCount++;
		if( lineCount > 16 )   break;
	}
	//�`���ʂɔ�r���e�擾
	if( unifiedDiffFormat == true )
	{
		//Unified Diff�`�� #335
		
		//@�Ŏn�܂�ŏ��̍s������
		AIndex	pos = 0;
		AText	line;
		for( ; pos < inText.GetItemCount(); )
		{
			inText.GetLine(pos,line);
			if( line.GetItemCount() == 0 )   continue;
			if( line.Get(0) == '@' )   break;
		}
		//�e�ύX�_���Ƃ̃��[�v
		for( ; pos < inText.GetItemCount(); )
		{
			//�����ɂ����Ƃ��ɂ́A���Ȃ炸@@�s��line�ɓǂݍ��񂾎��_�i@@�s�̎��̍s��pos������j
			AIndex	new_startlineindex = 0;//�ύX�㑤�̊J�n�s
			AIndex	new_linecount = 0;//�ύX�㑤�̍s��
			AIndex	old_startlineindex = 0;//�ύX�O���̊J�n�s #379
			AIndex	old_linecount = 0;//�ύX�O���̍s�� #379
			//@@�s�̉��
			//��F@@ -5632,7 +5649,7 @@
			for( AIndex linepos = 1; linepos < line.GetItemCount(); linepos++ )
			{
				AUChar	ch = line.Get(linepos);
				//�ύX�O���J�n�s�E�s���擾 #379
				if( ch == '-' )
				{
					linepos++;
					if( line.ParseIntegerNumber(linepos,old_startlineindex,false) == false )   {_ACError("",NULL);return;}
					old_startlineindex--;//�s�ԍ���1�͂��܂�Ȃ̂ŁA0�͂��܂�ɂ���
					linepos++;
					if( line.ParseIntegerNumber(linepos,old_linecount,false) == false )   {_ACError("",NULL);return;}
				}
				//�ύX�㑤�J�n�s�E�s���擾
				if( ch == '+' )
				{
					linepos++;
					if( line.ParseIntegerNumber(linepos,new_startlineindex,false) == false )   {_ACError("",NULL);return;}
					new_startlineindex--;//�s�ԍ���1�͂��܂�Ȃ̂ŁA0�͂��܂�ɂ���
					linepos++;
					if( line.ParseIntegerNumber(linepos,new_linecount,false) == false )   {_ACError("",NULL);return;}
					break;
				}
			}
			//�ύX���e�����̉��
			//+�̃u���b�N�A-�̃u���b�N����ڍs����Ƃ��ɁA�����s�i�[�������s��
			AIndex	new_lineindex = new_startlineindex;//working���̌��ݍs���i�[
			AIndex	old_lineindex = old_startlineindex;//#379 repos���̌��ݍs���i�[
			AItemCount	pluscount = 0;
			AItemCount	minuscount = 0;
			AText	deletedText;//#379
			for( ; pos < inText.GetItemCount(); )
			{
				//1�s�擾
				inText.GetLine(pos,line);
				if( line.GetItemCount() == 0 )   {_ACError("",NULL);return;}
				AUChar	ch = line.Get(0);
				//@�s�Ȃ炱�̕ύX���e�������[�v�I��
				if( ch == '@' )   break;
				//+�u���b�N�I��
				if( ch != '+' && pluscount > 0 )
				{
					if( minuscount > 0 )
					{
						//-�u���b�N����A+�u���b�N�I���Ȃ̂ŁA�u�ύX�v
						diffTypeArray.Add(kDiffType_Changed);
						startParagraphIndexArray.Add(new_lineindex-pluscount);
						endParagraphIndexArray.Add(new_lineindex);
						//#379 ���|�W�g������Start,End�i���ݒ�
						oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
						oldEndParagraphIndexArray.Add(old_lineindex);
						//�폜�e�L�X�g�ǉ�
						deletedTextArray.Add(deletedText);//#379
					}
					else
					{
						//-�u���b�N�Ȃ��A+�u���b�N�I���Ȃ̂ŁA�u�ǉ��v
						diffTypeArray.Add(kDiffType_Added);
						startParagraphIndexArray.Add(new_lineindex-pluscount);
						endParagraphIndexArray.Add(new_lineindex);
						//#379 ���|�W�g������Start,End�i���ݒ�
						oldStartParagraphIndexArray.Add(old_lineindex);
						oldEndParagraphIndexArray.Add(old_lineindex+1);
						//�폜�e�L�X�g�ǉ�
						deletedTextArray.Add(GetEmptyText());//#379
					}
					pluscount = 0;
					minuscount = 0;
					deletedText.DeleteAll();
				}
				//-�u���b�N�I���i�������A����+�̏ꍇ�́Achanged�Ȃ̂ŉ������Ȃ��B
				if( ch == ' ' && minuscount > 0 )
				{
					//-�u���b�N�I�����ύX�Ȃ��u���b�N�Ȃ̂ŁA�u�폜�v
					diffTypeArray.Add(kDiffType_Deleted);
					startParagraphIndexArray.Add(new_lineindex);
					endParagraphIndexArray.Add(new_lineindex+1);
					//#379 ���|�W�g������Start,End�i���ݒ�
					oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
					oldEndParagraphIndexArray.Add(old_lineindex);
					//�폜�e�L�X�g�ǉ�
					deletedTextArray.Add(deletedText);//#379
					minuscount = 0;
					deletedText.DeleteAll();
				}
				//�폜�e�L�X�g�L�� #379
				if( ch == '-' )
				{
					AText	text;
					text.SetText(line);
					text.Delete(0,1);
					text.Add(kUChar_LineEnd);
					deletedText.AddText(text);
				}
				//+, -�J�E���g�A�s���C���N�������g
				switch(ch)
				{
				  case '+':
					{
						pluscount++;
						new_lineindex++;
						break;
					}
				  case '-':
					{
						minuscount++;
						old_lineindex++;//#379
						break;
					}
				  case ' ':
					{
						new_lineindex++;
						old_lineindex++;//#379
						break;
					}
				  default:
					{
						_ACError("",NULL);
						break;
					}
				}
			}
			//�ύX���e�������[�v�I������
			//+, -�̃J�E���g�c�肪����Ώ�������
			if( pluscount > 0 )
			{
				if( minuscount > 0 )
				{
					//-�u���b�N����A+�u���b�N�I���Ȃ̂ŁA�u�ύX�v
					diffTypeArray.Add(kDiffType_Changed);
					startParagraphIndexArray.Add(new_lineindex-pluscount);
					endParagraphIndexArray.Add(new_lineindex);
					//#379 ���|�W�g������Start,End�i���ݒ�
					oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
					oldEndParagraphIndexArray.Add(old_lineindex);
					//�폜�e�L�X�g�ǉ�
					deletedTextArray.Add(deletedText);//#379
				}
				else
				{
					//-�u���b�N�Ȃ��A+�u���b�N�I���Ȃ̂ŁA�u�ǉ��v
					diffTypeArray.Add(kDiffType_Added);
					startParagraphIndexArray.Add(new_lineindex-pluscount);
					endParagraphIndexArray.Add(new_lineindex);
					//#379 ���|�W�g������Start,End�i���ݒ�
					oldStartParagraphIndexArray.Add(old_lineindex);
					oldEndParagraphIndexArray.Add(old_lineindex+1);
					//�폜�e�L�X�g�ǉ�
					deletedTextArray.Add(GetEmptyText());//#379
				}
				pluscount = 0;
				minuscount = 0;
			}
			if( minuscount > 0 )
			{
				//-�u���b�N�Ȃ��I���Ȃ̂ŁA�u�폜�v
				diffTypeArray.Add(kDiffType_Deleted);
				startParagraphIndexArray.Add(new_lineindex);
				endParagraphIndexArray.Add(new_lineindex+1);
				//#379 ���|�W�g������Start,End�i���ݒ�
				oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
				oldEndParagraphIndexArray.Add(old_lineindex);
				//�폜�e�L�X�g�ǉ�
				deletedTextArray.Add(deletedText);//#379
				minuscount = 0;
			}
		}
	}
	else
	{
		//��diff�`��
		for( AIndex pos = 0; pos < inText.GetItemCount(); )
		{
			AText	line;
			inText.GetLine(pos,line);
			if( line.GetItemCount() == 0 )   continue;
			AUChar	ch = line.Get(0);
			if( ch >= '0' && ch <= '9' )
			{
				AIndex	linepos = 0;
				ANumber	oldsnum = 0, oldenum = 0;
				if( line.ParseIntegerNumber(linepos,oldsnum,false) == false )   continue;
				if( linepos >= line.GetItemCount() )   continue;
				ch = line.Get(linepos);
				linepos++;
				if( ch == ',' )
				{
					if( line.ParseIntegerNumber(linepos,oldenum,false) == false )   continue;
					if( linepos >= line.GetItemCount() )   continue;
					ch = line.Get(linepos);
					linepos++;
				}
				if( oldenum == 0 )   oldenum = oldsnum;//#379
				//
				switch(ch)
				{
				  case 'a':
					{
						ANumber	newsnum = 0, newenum = 0;
						if( line.ParseIntegerNumber(linepos,newsnum,false) == false )   continue;
						if( linepos < line.GetItemCount() )
						{
							if( line.Get(linepos) == ',' )
							{
								linepos++;
								line.ParseIntegerNumber(linepos,newenum,false);
							}
						}
						if( newenum == 0 )   newenum = newsnum;
						
						diffTypeArray.Add(kDiffType_Added);
						startParagraphIndexArray.Add(newsnum-1);
						endParagraphIndexArray.Add(newenum+1-1);
						//#379 ���|�W�g������Start,End�i���ݒ�
						oldStartParagraphIndexArray.Add(oldsnum-1+1);
						oldEndParagraphIndexArray.Add(oldenum+1-1+1);
						//#379 �ǉ��s�Ȃ̂ō폜�e�L�X�g�͂Ȃ�
						deletedTextArray.Add(GetEmptyText());
						break;
					}
				  case 'c':
					{
						ANumber	newsnum = 0, newenum = 0;
						if( line.ParseIntegerNumber(linepos,newsnum,false) == false )   continue;
						if( linepos < line.GetItemCount() )
						{
							if( line.Get(linepos) == ',' )
							{
								linepos++;
								line.ParseIntegerNumber(linepos,newenum,false);
							}
						}
						if( newenum == 0 )   newenum = newsnum;
						
						diffTypeArray.Add(kDiffType_Changed);
						startParagraphIndexArray.Add(newsnum-1);
						endParagraphIndexArray.Add(newenum+1-1);
						//#379 ���|�W�g������Start,End�i���ݒ�
						oldStartParagraphIndexArray.Add(oldsnum-1);
						oldEndParagraphIndexArray.Add(oldenum+1-1);
						//#379 �폜�e�L�X�g�擾
						AText	deletedText;
						for( AIndex p = pos; p < inText.GetItemCount(); )
						{
							AText	l;
							inText.GetLine(p,l);
							if( l.GetItemCount() < 2 )   continue;
							if( l.Get(0) != '<' )   break;
							l.Delete(0,2);
							deletedText.AddText(l);
							deletedText.Add(kUChar_LineEnd);
						}
						deletedTextArray.Add(deletedText);
						break;
					}
				  case 'd':
					{
						ANumber	newsnum = 0, newenum = 0;
						if( line.ParseIntegerNumber(linepos,newsnum,false) == false )   continue;
						if( linepos < line.GetItemCount() )
						{
							if( line.Get(linepos) == ',' )
							{
								linepos++;
								line.ParseIntegerNumber(linepos,newenum,false);
							}
						}
						if( newenum == 0 )   newenum = newsnum;
						
						diffTypeArray.Add(kDiffType_Deleted);
						//'d'�̏ꍇ�A�\�����ꂽ�i���ԍ��̌�ɍ폜�s�����邱�Ƃ��Ӗ�����B
						//newsnum==0�����肤��B'a'��'c'�Ɠ�����newsnum-1���i�[���Ă��܂��ƁA-1���i�[����āA�����̓s���������B
						//����āA�i�[���ꂽ�i���ԍ��̑O�ɍ폜�s�����邱�Ƃ��Ӗ�����悤�ɂ���B
						startParagraphIndexArray.Add(newsnum-1+1);
						endParagraphIndexArray.Add(newenum+1-1+1);
						//#379 ���|�W�g������Start,End�i���ݒ�
						oldStartParagraphIndexArray.Add(oldsnum-1);
						oldEndParagraphIndexArray.Add(oldenum+1-1);
						//#379 �폜�e�L�X�g�擾
						AText	deletedText;
						for( AIndex p = pos; p < inText.GetItemCount(); )
						{
							AText	l;
							inText.GetLine(p,l);
							if( l.GetItemCount() < 2 )   continue;
							if( l.Get(0) != '<' )   break;
							l.Delete(0,2);
							deletedText.AddText(l);
							deletedText.Add(kUChar_LineEnd);
						}
						deletedTextArray.Add(deletedText);
						break;
					}
				}
			}
		}
	}
	*/
	AObjectID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,inFile);
	if( docID != kObjectID_Invalid )
	{
		if( GetApp().NVI_GetDocumentTypeByID(docID) == kDocumentType_Text )
		{
			GetApp().SPI_GetTextDocumentByID(docID).SPI_DoSCMCompareResult(/*#379 diffTypeArray,
					startParagraphIndexArray,endParagraphIndexArray,
					oldStartParagraphIndexArray,oldEndParagraphIndexArray,deletedTextArray*/inText);//#379
		}
	}
}
#endif

//#402
/**
�t�@�C���ʂ�status��ݒ�
ASCMFolderData::DoStatusCommandResult()����SCMFolderManager�o�R�ŃR�[�������
*/
void	ASCMFolderData::SetStatus( const AFileAcc& inFile, const AText& inStatusText, const ASCMFileState inSCMFileState )
{
	AText	filename;
	inFile.GetFilename(filename);
	filename.ConvertToCanonicalComp();//#1087
	//���f�[�^���폜
	AIndex	index = mFileNameArray.Find(filename);
	if( index != kIndex_Invalid )
	{
		mFileNameArray.Delete1(index);
		mFileStateArray.Delete1(index);
		mFileStateTextArray.Delete1(index);
	}
	//�f�[�^�ǉ�
	mFileNameArray.Add(filename);
	mFileStateArray.Add(inSCMFileState);
	mFileStateTextArray.Add(inStatusText);
}

#pragma mark ===========================
#pragma mark [�N���X]ASCMFolderManager
#pragma mark ===========================
//ASCMFolderData�̃��X�g��ێ����A�Ǘ�����N���X

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
ASCMFolderManager::ASCMFolderManager() : AObjectArrayItem(NULL)
{
}
//�f�X�g���N�^
ASCMFolderManager::~ASCMFolderManager()
{
}

/*
AText	gSCMFolderPathCache;
AIndex	gSCMFolderPathCache_Index = kIndex_Invalid;
*/

/**
�t�@�C������SCM�t�H���_index������
*/
AIndex	ASCMFolderManager::GetSCMFolderDataIndexByFile( const AFileAcc& inFile ) const
{
	AText	path;
	inFile.GetPath(path);
	return GetSCMFolderDataIndexByFilePath(path);
}

/**
�t�H���_����SCM�t�H���_index������
*/
AIndex	ASCMFolderManager::GetSCMFolderDataIndexByFolder( const AFileAcc& inFolder ) const
{
	//�p�X�擾�i�Ō��'/'�͖����ɂ���j
	AText	path;
	inFolder.GetPath(path);//#530
	path.RemoveLastPathDelimiter();//#892
	/*
	if( gSCMFolderPathCache.Compare(path) == true )
	{
		return gSCMFolderPathCache_Index;
	}
	*/
	//
	AIndex	index = mSCMFolderDataArray_PathText.Find(path);
	/*
	if( index != kIndex_Invalid )
	{
		gSCMFolderPathCache.SetText(path);
		gSCMFolderPathCache_Index = index;
	}
	*/
	return index;
}

/**
�t�@�C���p�X����SCM�t�H���_index������
*/
AIndex	ASCMFolderManager::GetSCMFolderDataIndexByFilePath( const AText& inFilePath ) const
{
	//�t�@�C���p�X����̂Ƃ��̓��^�[��
	if( inFilePath.GetItemCount() == 0 )   return kIndex_Invalid;
	//pos�ɍŌ��'/'�̈ʒu���i�[����
	AIndex	pos = inFilePath.GetItemCount()-1;
	for( ; pos >= 0; pos-- )
	{
		if( inFilePath.Get(pos) == AFileAcc::GetPathDelimiter() )
		{
			break;
		}
	}
	//�t�@�C���p�X�̍Ō��'/'�̑O�܂ł̃e�L�X�g�i���t�H���_�j������
	return mSCMFolderDataArray_PathText.Find(inFilePath,0,pos);
}

#pragma mark ===========================

#pragma mark ---

//�t�H���_��update�����擾����
void	ASCMFolderManager::UpdateFolderStatus( const AFileAcc& inFolder )
{
	AIndex	index = GetSCMFolderDataIndexByFolder(inFolder);
	if( index == kIndex_Invalid )
	{
		RegisterFolder(inFolder);
	}
	else
	{
		mSCMFolderDataArray.GetObject(index).UpdateStatus();
	}
}
void	ASCMFolderManager::UpdateFolderStatusAll()
{
	for( AIndex i = 0; i < mSCMFolderDataArray.GetItemCount(); i++ )
	{
		mSCMFolderDataArray.GetObject(i).UpdateStatus();
	}
}

#pragma mark ===========================

#pragma mark ---�t�H���_�o�^

/*
�����v���W�F�N�g�^�����t�H���_�ǂݍ��݊�������RegisterFolder(s)����ɃR�[�������B
�t�H���_���o�^�ł���΁ASCMFolder��o�^����B
�t�H���_��SCM�̃t�H���_���ǂ����Ɋւ�炸�A���SCM�t�H���_�ɓo�^�����B
�o�^��A�eSCM�c�[���̃t�H���_���ǂ����𔻒肷��isvn info�Ȃǂɂ���Ĕ��ʁj
*/

/**
�t�H���_�o�^�i�P�̂݁j
�t�H���_�����o�^�Ȃ�o�^���A�t�H���_��status���X�V����
�t�H���_���o�^�ς݂Ȃ牽�����Ȃ�
*/
void	ASCMFolderManager::RegisterFolder( const AFileAcc& inFolder )
{
	//�ݒ�Off�Ȃ烊�^�[��
	//����ɂ��A�ݒ�Off�̏ꍇ�́AASCMFolderData���쐬����Ȃ��̂ŁAASCMFolderData�̃��\�b�h�͈�؎��s����Ȃ�
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == false  )   return;
	
	//ASCMFolderData����
	AIndex	index = GetSCMFolderDataIndexByFolder(inFolder);
	if( index == kIndex_Invalid )
	{
		ASCMFolderDataFactory	factory(this,inFolder,false);
		index = mSCMFolderDataArray.AddNewObject(factory);
		AText	path;
		inFolder.GetPath(path);//#530
		path.RemoveLastPathDelimiter();//#892
		mSCMFolderDataArray_PathText.Add(path);
		//#664}
		
		//#402 Status�X�V #664�o�^���̂�UpdateStatus()����悤��if�u���b�N���Ɉړ�
		mSCMFolderDataArray.GetObject(index).UpdateStatus();
	}
}

//#402
/**
�t�H���_�o�^�i�����ꊇ�j (kInternalEvent_ProjectFolderLoaderResult����R�[�������)
�t�H���_�����o�^�Ȃ�o�^���A�t�H���_��status���X�V����
�t�H���_���o�^�ς݂Ȃ牽�����Ȃ�
*/
void	ASCMFolderManager::RegisterFolders( const AObjectArray<AFileAcc>& inFolders, const AIndex inRootFolderIndex )
{
	//�ݒ�Off�Ȃ烊�^�[��
	//����ɂ��A�ݒ�Off�̏ꍇ�́AASCMFolderData���쐬����Ȃ��̂ŁAASCMFolderData�̃��\�b�h�͈�؎��s����Ȃ�
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == false  )   return;
	
	//ASCMFolderData����
	for( AIndex i = 0; i < inFolders.GetItemCount(); i++ )
	{
		AIndex	index = GetSCMFolderDataIndexByFolder(inFolders.GetObjectConst(i));
		if( index == kIndex_Invalid )
		{
			AText	path;
			inFolders.GetObjectConst(i).GetPath(path);//#530 GetNormalizedPath(path);
			//.svn�t�H���_�͑ΏۊO�Ƃ��� #0�������ߖ�
			AIndex	p = 0;
			AText	svnfolder;
			svnfolder.Add(AFileAcc::GetPathDelimiter());
			svnfolder.AddCstring(".svn");
			if( path.FindText(0,svnfolder,p) == true )
			{
				continue;
			}
			//�T�u�t�H���_���ǂ������擾
			ABool	isSubFolder = (i != inRootFolderIndex);
			//FolderData�𐶐�
			ASCMFolderDataFactory	factory(this,inFolders.GetObjectConst(i),isSubFolder);
			index = mSCMFolderDataArray.AddNewObject(factory);
			path.RemoveLastPathDelimiter();//#892
			mSCMFolderDataArray_PathText.Add(path);
		}
	}
	
	//Status�X�V
	if( inFolders.GetItemCount() > 0 )
	{
		AIndex	index = GetSCMFolderDataIndexByFolder(inFolders.GetObjectConst(inRootFolderIndex));
		mSCMFolderDataArray.GetObject(index).UpdateStatus();
	}
}

#pragma mark ===========================

#pragma mark ---Status����

//#402
/**
�t�@�C���ʂ�status��ݒ�
ASCMFolderData::DoStatusCommandResult()����R�[�������
*/
void	ASCMFolderManager::SetStatus( const AFileAcc& inFile, const AText& inStatusText, const ASCMFileState inSCMFileState )
{
	AFileAcc	parent;
	parent.SpecifyParent(inFile);
	AIndex	index = GetSCMFolderDataIndexByFolder(parent);
	if( index != kIndex_Invalid )
	{
		mSCMFolderDataArray.GetObject(index).SetStatus(inFile,inStatusText,inSCMFileState);
	}
}

#pragma mark ===========================

#pragma mark ---

void	ASCMFolderManager::DoStatusCommandResult( const AFileAcc& inFolder, const AText& inText )
{
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return;
	
	AIndex	index = GetSCMFolderDataIndexByFolder(inFolder);
	if( index != kIndex_Invalid )
	{
		mSCMFolderDataArray.GetObject(index).DoStatusCommandResult(inText);
	}
}

/**
�t�@�C����SCM��Ԃ��擾
@note AApp::SPI_GetProjectSCMStateArray()�Ȃǂ���R�[�������B�A�v���P�[�V����activate�����ɃR�[�������B
*/
ASCMFileState	ASCMFolderManager::GetFileState( const AFileAcc& inFile ) const
{
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return kSCMFileState_NotSCMFolder;
	
	AIndex	index = GetSCMFolderDataIndexByFile(inFile);
	if( index != kIndex_Invalid )
	{
		AText	path;//#530
		inFile.GetPath(path);//#530
		AText	filename;
		path.GetFilename(filename);//#530
		return mSCMFolderDataArray.GetObjectConst(index).GetFileState(filename);
	}
	return kSCMFileState_NotSCMFolder;
}
ASCMFileState	ASCMFolderManager::GetFileState( const AText& inFilePath ) const
{
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return kSCMFileState_NotSCMFolder;
	
	AIndex	index = GetSCMFolderDataIndexByFilePath(inFilePath);
	if( index != kIndex_Invalid )
	{
		AText	filename;
		inFilePath.GetFilename(filename);//#530
		return mSCMFolderDataArray.GetObjectConst(index).GetFileState(filename);
	}
	return kSCMFileState_NotSCMFolder;
}

void	ASCMFolderManager::GetFileStateText( const AFileAcc& inFile, AText& outText ) const
{
	outText.DeleteAll();
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return;
	
	AIndex	index = GetSCMFolderDataIndexByFile(inFile);
	if( index != kIndex_Invalid )
	{
		AText	filename;
		inFile.GetFilename(filename);
		mSCMFolderDataArray.GetObjectConst(index).GetFileStateText(filename,outText);
	}
}

void	ASCMFolderManager::GetFileStateArray( const AFileAcc& inFolder, const ATextArray& inFilenameArray, 
		AArray<ASCMFileState>& outFileStateArray ) const
{
	outFileStateArray.DeleteAll();
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return;
	
	AIndex	index = GetSCMFolderDataIndexByFolder(inFolder);
	if( index != kIndex_Invalid )
	{
		mSCMFolderDataArray.GetObjectConst(index).GetFileStateArray(inFilenameArray,outFileStateArray);
	}
}

void	ASCMFolderManager::CompareWithLatest( const AFileAcc& inFile, const ADocumentID& inDocumentID )//#736
{
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return;
	
	AIndex	index = GetSCMFolderDataIndexByFile(inFile);
	if( index != kIndex_Invalid )
	{
		mSCMFolderDataArray.GetObject(index).CompareWithLatest(inFile,inDocumentID);
	}
}

/*#736 
void	ASCMFolderManager::DoCompareResult( const AFileAcc& inFile, const AText& inText )
{
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return;
	
	AIndex	index = GetSCMFolderDataIndexByFile(inFile);
	if( index != kIndex_Invalid )
	{
		mSCMFolderDataArray.GetObject(index).DoCompareResult(inFile,inText);
	}
}
*/

ABool	ASCMFolderManager::IsSCMFolder( const AFileAcc& inFolder ) const
{
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return false;
	
	AIndex	index = GetSCMFolderDataIndexByFolder(inFolder);
	if( index != kIndex_Invalid )
	{
		if( mSCMFolderDataArray.GetObjectConst(index).GetSCMFolderType() != kSCMFolderType_NotSCMFolder )   return true;
	}
	return false;
}

//#455
/**
�t�@�C������SCM�t�H���_�^�C�v�擾
*/
ASCMFolderType	ASCMFolderManager::GetSCMFolderType( const AFileAcc& inFile ) const
{
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return kSCMFolderType_NotSCMFolder;
	
	AIndex	index = GetSCMFolderDataIndexByFile(inFile);
	if( index != kIndex_Invalid )
	{
		return mSCMFolderDataArray.GetObjectConst(index).GetSCMFolderType();
	}
	return kSCMFolderType_NotSCMFolder;
}

//#455
/**
�t�@�C������Repository�p�X�擾
*/
const AText&	ASCMFolderManager::GetRepositoryPath( const AFileAcc& inFile ) const
{
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return GetEmptyText();
	
	AIndex	index = GetSCMFolderDataIndexByFile(inFile);
	if( index != kIndex_Invalid )
	{
		return mSCMFolderDataArray.GetObjectConst(index).GetRepositoryPath();
	}
	return GetEmptyText();
}

//#455
/**
�t�@�C������Status�X�V
*/
void	ASCMFolderManager::UpdateStatus( const AFileAcc& inFile ) 
{
	if( mSCMFolderDataArray.GetItemCount() == 0 )   return;
	
	AIndex	index = GetSCMFolderDataIndexByFile(inFile);
	if( index != kIndex_Invalid )
	{
		mSCMFolderDataArray.GetObject(index).UpdateStatus();
	}
}

