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
#pragma mark [クラス]ASCMFolderData
#pragma mark ===========================
//各フォルダ毎のデータを保持するクラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
ASCMFolderData::ASCMFolderData( AObjectArrayItem* inParent, const AFileAcc& inFolder, const ABool inIsProjectSubFolder ) : 
		AObjectArrayItem(NULL), mSCMFolderType(kSCMFolderType_NotSCMFolder), mNowUpdatingStatus(false)
,mIsProjectSubFolder(inIsProjectSubFolder)
{
	mFolder.CopyFrom(inFolder);
	
	//#961
	//SCMフォルダタイプ判定
	FindSCMConfigFolderRecursive(inFolder);
	
	/*#961
	AFileAcc	cvsroot;
	cvsroot.SpecifyChild(inFolder,"CVS/Root");
	//R0247
	AFileAcc	svnroot;
	svnroot.SpecifyChild(inFolder,".svn/entries");
	//svnのentriesファイルが存在したら、svnフォルダとみなす（svn優先）
	if( svnroot.Exist() == true )
	{
		AText	text;
		svnroot.ReadTo(text);
		if( text.GetItemCount() > 0 )
		{
			AText	line;
			AIndex	pos = 0;
			//5行目
			text.GetLine(pos,line);
			text.GetLine(pos,line);
			text.GetLine(pos,line);
			text.GetLine(pos,line);
			text.GetLine(pos,mRepository);
			mSCMFolderType = kSCMFolderType_SVN;
		}
	}
	*/
	/*#526 CVS未対応
	else
	if( cvsroot.Exist() == true )
	{
		AText	text;
		cvsroot.ReadTo(text);
		if( text.GetItemCount() > 0 )
		{
			if( text.Get(0) != ':' )//まずはローカルのみ対応する
			{
				AIndex	pos = 0;
				text.GetLine(pos,mRepository);
				mSCMFolderType = kSCMFolderType_CVS;
			}
		}
	}
	*/
	
	//#402 コール元のRegisterFolder()/RegisterFolders()で実行 UpdateStatus();
}
//デストラクタ
ASCMFolderData::~ASCMFolderData()
{
}

//#589
/**
SCMフォルダタイプの判定
*/
void	ASCMFolderData::FindSCMConfigFolderRecursive( const AFileAcc& inFolder )
{
	AFileAcc	configfolder;
	//SVNフォルダかどうかを判定
	configfolder.SpecifyChild(inFolder,".svn");
	if( configfolder.Exist() == true )
	{
		mSCMFolderType = kSCMFolderType_SVN;
		//entriesフォルダからレポジトリパス取得
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
				//5行目
				text.GetLine(pos,line);
				text.GetLine(pos,line);
				text.GetLine(pos,line);
				text.GetLine(pos,line);
				text.GetLine(pos,mRepository);
			}
		}
		return;
	}
	//Gitフォルダかどうかを判定
	configfolder.SpecifyChild(inFolder,".git");
	if( configfolder.Exist() == true )
	{
		mSCMFolderType = kSCMFolderType_Git;
		mRootFolder.CopyFrom(inFolder);
		return;
	}
	
	//存在していなければ親フォルダで検索（再帰）
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

//更新（cvs status等）
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
			//コマンドとしてはenvを実行。日本語ファイル名をsvnに渡すため。 #1087
			AText	command;
			command.SetCstring("/usr/bin/env");
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			t.SetCstring("LANG=UTF-8");
			argArray.Add(t);
			//svnコマンドパス
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kSVNPath,t);
			argArray.Add(t);
			//引数
			t.SetCstring("status");
			argArray.Add(t);
			//カレントディレクトリ
			AText	dirpath;
			mFolder.GetNormalizedPath(dirpath);
			//実行
			GetApp().SPI_GetChildProcessManager().ExecuteSCMCommand(command,argArray,dirpath,kSCMCommandType_Status,mFolder,kObjectID_Invalid);
			mNowUpdatingStatus = true;
			break;
		}
		//#589
	  case kSCMFolderType_Git:
		{
			//テストしたところ、SVNと同様の#1087対応は必要なさそう。日本語ファイル名でも問題なく動作している。141207
			//gitコマンドパス
			AText	command;
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kGitPath,command);
			//引数
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			t.SetCstring("status");
			argArray.Add(t);
			t.SetCstring("--short");
			argArray.Add(t);
			t.SetCstring("-z");//改行をNULLで出力
			argArray.Add(t);
			//カレントディレクトリ
			AText	dirpath;
			mFolder.GetNormalizedPath(dirpath);
			//実行
			GetApp().SPI_GetChildProcessManager().ExecuteSCMCommand(command,argArray,dirpath,kSCMCommandType_Status,mFolder,kObjectID_Invalid);
			mNowUpdatingStatus = true;
			break;
		}
	  default:
		{
			//処理無し
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
							//#402 下へ移動 mFileNameArray.Add(filename);
							//#402 下へ移動 mFileStateTextArray.Add(statustext);
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
							//filename内に'/'があるときは、サブフォルダ内のstatusとみなして、別のSCMFolderDataへsetする #402
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
				//1行取得
				AText	line;
				inText.GetLine(pos,line);
				if( line.GetItemCount() > 1 )
				{
					//ファイル名取得
					AIndex	linepos = 1;
					line.SkipTabSpace(linepos,line.GetItemCount());
					AText	filename, statustext;
					line.GetText(linepos,line.GetItemCount()-linepos,filename);
					filename.ConvertToCanonicalComp();//#1087 canonical comp形式で記憶
					//最初の1文字によってファイル状態取得
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
					//filename内に'/'があるときは、サブフォルダ内のstatusとみなして、別のSCMFolderDataへsetする #402
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
			//NULL文字を改行に変換
			AText	text(inText);
			text.ReplaceChar(0,kUChar_LineEnd);
			//
			for( AIndex pos = 0; pos < text.GetItemCount(); )
			{
				//1行取得
				AText	line;
				text.GetLine(pos,line);
				if( line.GetItemCount() > 1 )
				{
					//ファイル名取得
					AIndex	linepos = 2;
					line.SkipTabSpace(linepos,line.GetItemCount());
					AText	filename, statustext;
					line.GetText(linepos,line.GetItemCount()-linepos,filename);
					filename.ConvertToCanonicalComp();//#1087 canonical comp形式で記憶
					//2文字目によってファイル状態取得
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
					//-zオプションがあるときは、常にgitルートからのパスになる
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
			//処理無し
			break;
		}
	}
	//#379 ドキュメントのDiffモード更新のため
	GetApp().SPI_DoSCMStatusUpdated(mFolder);
}

ASCMFileState	ASCMFolderData::GetFileState( const AText& inFilename ) const
{
	if( mSCMFolderType == kSCMFolderType_NotSCMFolder )   return kSCMFileState_NotSCMFolder;
	
	//#1087
	//canonical comp形式に変換（内部的にcanonical compで記憶しているため）
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
			//処理なし
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
	//canonical comp形式に変換（内部的にcanonical compで記憶しているため）
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
			//処理無し
			break;
		}
	}
}

/**
SCM状態を取得
注意：速度を考慮し、GetFileState()等と違って、関数内でファイル名のConvertToCanonicalComp()を行っていない。コールする側でConvertToCanonicalComp()済みの配列を渡すこと。
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
			//処理なし
			break;
		}
	}
}

/**
最新版との比較実行（非同期実行）
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
			//コマンドとしてはenvを実行。日本語ファイル名をsvnに渡すため。 #1087
			AText	command;
			command.SetCstring("/usr/bin/env");
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			t.SetCstring("LANG=UTF-8");
			argArray.Add(t);
			//svnコマンドパス
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kSVNPath,t);
			argArray.Add(t);
			//引数
			t.SetCstring("diff");
			argArray.Add(t);
			/*#379 このsvn diffはレポジトリテキストを生成するためだけにしたので、外部diffを使う必要ない
			if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseExternalDiffForSubversion) == true )//#335
			{
				//外部diff指定
				t.SetCstring("--diff-cmd");
				argArray.Add(t);
				GetApp().GetAppPref().GetData_Text(AAppPrefDB::kDiffPath,t);
				argArray.Add(t);
				//diffオプション指定
				t.SetCstring("-x");
				argArray.Add(t);
				GetApp().GetAppPref().GetData_Text(AAppPrefDB::kDiffOption,t);//ダブルクオーテーションはシェル経由ではないので不要
				if( t.GetItemCount() > 0 )
				{
					t.Add(' ');
				}
				t.AddCstring("--normal");
				argArray.Add(t);
			}
			*/
			//ファイル名
			AFileAcc::GetNormalizedPathString(filename);//#1087
			argArray.Add(filename);
			//カレントディレクトリ
			AText	dirpath;
			mFolder.GetNormalizedPath(dirpath);
			//実行
			AText	resulttext;
			GetApp().SPI_GetChildProcessManager().ExecuteSCMCommand(command,argArray,dirpath,kSCMCommandType_Compare,inFile,inDocumentID);//#736
			break;
		}
		//#589
	  case kSCMFolderType_Git:
		{
			//テストしたところ、SVNと同様の#1087対応は必要なさそう。日本語ファイル名でも問題なく動作している。141207
			//svnコマンドパス
			AText	command;
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kGitPath,command);
			//引数
			ATextArray	argArray;
			argArray.Add(command);
			AText	t;
			t.SetCstring("diff");
			argArray.Add(t);
			//ファイル名
			AFileAcc::GetNormalizedPathString(filename);//#1087
			argArray.Add(filename);
			//カレントディレクトリ
			AText	dirpath;
			mFolder.GetNormalizedPath(dirpath);
			//実行
			AText	resulttext;
			GetApp().SPI_GetChildProcessManager().ExecuteSCMCommand(command,argArray,dirpath,kSCMCommandType_Compare,inFile,inDocumentID);//#736
			break;
		}
	  default:
		{
			//処理無し
			break;
		}
	}
}

#if 0
/*
UnifiedDiff形式
Index: AApp.cpp
===================================================================
--- AApp.cpp	(revision 2247)
+++ AApp.cpp	(working copy)
@@ -5632,7 +5649,7 @@
 	SPI_GetMultiFileFindWindow().NVI_SwitchFocusToFirst();
 }
 
-//「最近使った?」に追加
+//「最近使った?」に追加
 void	AApp::SPI_AddRecentlyUsed( const AText& inFindText, const AFileAcc& inFolder )
 {
 	SPI_GetMultiFileFindWindow().SPI_AddRecentlyUsedFindText(inFindText);

旧diff形式
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
SCM比較コマンド実行後処理
AChildProcessManager::DoPipeDeleted_SCM()からコールされる。
*/
void	ASCMFolderData::DoCompareResult( const AFileAcc& inFile, const AText& inText )
{
	/*#379 SPI_ParseDiffText()へ移動
	AArray<ADiffType>	diffTypeArray;
	AArray<AIndex>	startParagraphIndexArray;
	AArray<AIndex>	endParagraphIndexArray;
	AArray<AIndex>	oldStartParagraphIndexArray;//#379
	AArray<AIndex>	oldEndParagraphIndexArray;//#379
	ATextArray	deletedTextArray;//#379
	//#379 差分テキスト内容で形式判断するように変更if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseExternalDiffForSubversion) == false )//#335
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
			//@で始まる行があればUnifiedDiff形式とみなす
			unifiedDiffFormat = true;
			break;
		}
		//最初から16行目までをチェック
		lineCount++;
		if( lineCount > 16 )   break;
	}
	//形式別に比較内容取得
	if( unifiedDiffFormat == true )
	{
		//Unified Diff形式 #335
		
		//@で始まる最初の行を検索
		AIndex	pos = 0;
		AText	line;
		for( ; pos < inText.GetItemCount(); )
		{
			inText.GetLine(pos,line);
			if( line.GetItemCount() == 0 )   continue;
			if( line.Get(0) == '@' )   break;
		}
		//各変更点ごとのループ
		for( ; pos < inText.GetItemCount(); )
		{
			//ここにきたときには、かならず@@行をlineに読み込んだ時点（@@行の次の行にposがある）
			AIndex	new_startlineindex = 0;//変更後側の開始行
			AIndex	new_linecount = 0;//変更後側の行数
			AIndex	old_startlineindex = 0;//変更前側の開始行 #379
			AIndex	old_linecount = 0;//変更前側の行数 #379
			//@@行の解析
			//例：@@ -5632,7 +5649,7 @@
			for( AIndex linepos = 1; linepos < line.GetItemCount(); linepos++ )
			{
				AUChar	ch = line.Get(linepos);
				//変更前側開始行・行数取得 #379
				if( ch == '-' )
				{
					linepos++;
					if( line.ParseIntegerNumber(linepos,old_startlineindex,false) == false )   {_ACError("",NULL);return;}
					old_startlineindex--;//行番号は1はじまりなので、0はじまりにする
					linepos++;
					if( line.ParseIntegerNumber(linepos,old_linecount,false) == false )   {_ACError("",NULL);return;}
				}
				//変更後側開始行・行数取得
				if( ch == '+' )
				{
					linepos++;
					if( line.ParseIntegerNumber(linepos,new_startlineindex,false) == false )   {_ACError("",NULL);return;}
					new_startlineindex--;//行番号は1はじまりなので、0はじまりにする
					linepos++;
					if( line.ParseIntegerNumber(linepos,new_linecount,false) == false )   {_ACError("",NULL);return;}
					break;
				}
			}
			//変更内容部分の解析
			//+のブロック、-のブロックから移行するときに、差分行格納処理を行う
			AIndex	new_lineindex = new_startlineindex;//working側の現在行を格納
			AIndex	old_lineindex = old_startlineindex;//#379 repos側の現在行を格納
			AItemCount	pluscount = 0;
			AItemCount	minuscount = 0;
			AText	deletedText;//#379
			for( ; pos < inText.GetItemCount(); )
			{
				//1行取得
				inText.GetLine(pos,line);
				if( line.GetItemCount() == 0 )   {_ACError("",NULL);return;}
				AUChar	ch = line.Get(0);
				//@行ならこの変更内容部分ループ終了
				if( ch == '@' )   break;
				//+ブロック終了
				if( ch != '+' && pluscount > 0 )
				{
					if( minuscount > 0 )
					{
						//-ブロックあり、+ブロック終了なので、「変更」
						diffTypeArray.Add(kDiffType_Changed);
						startParagraphIndexArray.Add(new_lineindex-pluscount);
						endParagraphIndexArray.Add(new_lineindex);
						//#379 レポジトリ側のStart,End段落設定
						oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
						oldEndParagraphIndexArray.Add(old_lineindex);
						//削除テキスト追加
						deletedTextArray.Add(deletedText);//#379
					}
					else
					{
						//-ブロックなし、+ブロック終了なので、「追加」
						diffTypeArray.Add(kDiffType_Added);
						startParagraphIndexArray.Add(new_lineindex-pluscount);
						endParagraphIndexArray.Add(new_lineindex);
						//#379 レポジトリ側のStart,End段落設定
						oldStartParagraphIndexArray.Add(old_lineindex);
						oldEndParagraphIndexArray.Add(old_lineindex+1);
						//削除テキスト追加
						deletedTextArray.Add(GetEmptyText());//#379
					}
					pluscount = 0;
					minuscount = 0;
					deletedText.DeleteAll();
				}
				//-ブロック終了（ただし、次が+の場合は、changedなので何もしない。
				if( ch == ' ' && minuscount > 0 )
				{
					//-ブロック終了→変更なしブロックなので、「削除」
					diffTypeArray.Add(kDiffType_Deleted);
					startParagraphIndexArray.Add(new_lineindex);
					endParagraphIndexArray.Add(new_lineindex+1);
					//#379 レポジトリ側のStart,End段落設定
					oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
					oldEndParagraphIndexArray.Add(old_lineindex);
					//削除テキスト追加
					deletedTextArray.Add(deletedText);//#379
					minuscount = 0;
					deletedText.DeleteAll();
				}
				//削除テキスト記憶 #379
				if( ch == '-' )
				{
					AText	text;
					text.SetText(line);
					text.Delete(0,1);
					text.Add(kUChar_LineEnd);
					deletedText.AddText(text);
				}
				//+, -カウント、行数インクリメント
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
			//変更内容部分ループ終了処理
			//+, -のカウント残りがあれば処理する
			if( pluscount > 0 )
			{
				if( minuscount > 0 )
				{
					//-ブロックあり、+ブロック終了なので、「変更」
					diffTypeArray.Add(kDiffType_Changed);
					startParagraphIndexArray.Add(new_lineindex-pluscount);
					endParagraphIndexArray.Add(new_lineindex);
					//#379 レポジトリ側のStart,End段落設定
					oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
					oldEndParagraphIndexArray.Add(old_lineindex);
					//削除テキスト追加
					deletedTextArray.Add(deletedText);//#379
				}
				else
				{
					//-ブロックなし、+ブロック終了なので、「追加」
					diffTypeArray.Add(kDiffType_Added);
					startParagraphIndexArray.Add(new_lineindex-pluscount);
					endParagraphIndexArray.Add(new_lineindex);
					//#379 レポジトリ側のStart,End段落設定
					oldStartParagraphIndexArray.Add(old_lineindex);
					oldEndParagraphIndexArray.Add(old_lineindex+1);
					//削除テキスト追加
					deletedTextArray.Add(GetEmptyText());//#379
				}
				pluscount = 0;
				minuscount = 0;
			}
			if( minuscount > 0 )
			{
				//-ブロックなし終了なので、「削除」
				diffTypeArray.Add(kDiffType_Deleted);
				startParagraphIndexArray.Add(new_lineindex);
				endParagraphIndexArray.Add(new_lineindex+1);
				//#379 レポジトリ側のStart,End段落設定
				oldStartParagraphIndexArray.Add(old_lineindex-minuscount);
				oldEndParagraphIndexArray.Add(old_lineindex);
				//削除テキスト追加
				deletedTextArray.Add(deletedText);//#379
				minuscount = 0;
			}
		}
	}
	else
	{
		//旧diff形式
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
						//#379 レポジトリ側のStart,End段落設定
						oldStartParagraphIndexArray.Add(oldsnum-1+1);
						oldEndParagraphIndexArray.Add(oldenum+1-1+1);
						//#379 追加行なので削除テキストはない
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
						//#379 レポジトリ側のStart,End段落設定
						oldStartParagraphIndexArray.Add(oldsnum-1);
						oldEndParagraphIndexArray.Add(oldenum+1-1);
						//#379 削除テキスト取得
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
						//'d'の場合、表示された段落番号の後に削除行があることを意味する。
						//newsnum==0もありうる。'a'や'c'と同じくnewsnum-1を格納してしまうと、-1が格納されて、処理の都合が悪い。
						//よって、格納された段落番号の前に削除行があることを意味するようにする。
						startParagraphIndexArray.Add(newsnum-1+1);
						endParagraphIndexArray.Add(newenum+1-1+1);
						//#379 レポジトリ側のStart,End段落設定
						oldStartParagraphIndexArray.Add(oldsnum-1);
						oldEndParagraphIndexArray.Add(oldenum+1-1);
						//#379 削除テキスト取得
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
ファイル個別にstatusを設定
ASCMFolderData::DoStatusCommandResult()からSCMFolderManager経由でコールされる
*/
void	ASCMFolderData::SetStatus( const AFileAcc& inFile, const AText& inStatusText, const ASCMFileState inSCMFileState )
{
	AText	filename;
	inFile.GetFilename(filename);
	filename.ConvertToCanonicalComp();//#1087
	//旧データを削除
	AIndex	index = mFileNameArray.Find(filename);
	if( index != kIndex_Invalid )
	{
		mFileNameArray.Delete1(index);
		mFileStateArray.Delete1(index);
		mFileStateTextArray.Delete1(index);
	}
	//データ追加
	mFileNameArray.Add(filename);
	mFileStateArray.Add(inSCMFileState);
	mFileStateTextArray.Add(inStatusText);
}

#pragma mark ===========================
#pragma mark [クラス]ASCMFolderManager
#pragma mark ===========================
//ASCMFolderDataのリストを保持し、管理するクラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
ASCMFolderManager::ASCMFolderManager() : AObjectArrayItem(NULL)
{
}
//デストラクタ
ASCMFolderManager::~ASCMFolderManager()
{
}

/*
AText	gSCMFolderPathCache;
AIndex	gSCMFolderPathCache_Index = kIndex_Invalid;
*/

/**
ファイルからSCMフォルダindexを検索
*/
AIndex	ASCMFolderManager::GetSCMFolderDataIndexByFile( const AFileAcc& inFile ) const
{
	AText	path;
	inFile.GetPath(path);
	return GetSCMFolderDataIndexByFilePath(path);
}

/**
フォルダからSCMフォルダindexを検索
*/
AIndex	ASCMFolderManager::GetSCMFolderDataIndexByFolder( const AFileAcc& inFolder ) const
{
	//パス取得（最後の'/'は無しにする）
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
ファイルパスからSCMフォルダindexを検索
*/
AIndex	ASCMFolderManager::GetSCMFolderDataIndexByFilePath( const AText& inFilePath ) const
{
	//ファイルパスが空のときはリターン
	if( inFilePath.GetItemCount() == 0 )   return kIndex_Invalid;
	//posに最後の'/'の位置を格納する
	AIndex	pos = inFilePath.GetItemCount()-1;
	for( ; pos >= 0; pos-- )
	{
		if( inFilePath.Get(pos) == AFileAcc::GetPathDelimiter() )
		{
			break;
		}
	}
	//ファイルパスの最後の'/'の前までのテキスト（＝フォルダ）を検索
	return mSCMFolderDataArray_PathText.Find(inFilePath,0,pos);
}

#pragma mark ===========================

#pragma mark ---

//フォルダのupdate情報を取得する
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

#pragma mark ---フォルダ登録

/*
同じプロジェクト／同じフォルダ読み込み完了時にRegisterFolder(s)が常にコールされる。
フォルダ未登録であれば、SCMFolderを登録する。
フォルダがSCMのフォルダかどうかに関わらず、常にSCMフォルダに登録される。
登録後、各SCMツールのフォルダかどうかを判定する（svn infoなどによって判別）
*/

/**
フォルダ登録（１つのみ）
フォルダが未登録なら登録し、フォルダのstatusを更新する
フォルダが登録済みなら何もしない
*/
void	ASCMFolderManager::RegisterFolder( const AFileAcc& inFolder )
{
	//設定Offならリターン
	//これにより、設定Offの場合は、ASCMFolderDataが作成されないので、ASCMFolderDataのメソッドは一切実行されない
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == false  )   return;
	
	//ASCMFolderData生成
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
		
		//#402 Status更新 #664登録時のみUpdateStatus()するようにifブロック内に移動
		mSCMFolderDataArray.GetObject(index).UpdateStatus();
	}
}

//#402
/**
フォルダ登録（複数一括） (kInternalEvent_ProjectFolderLoaderResultからコールされる)
フォルダが未登録なら登録し、フォルダのstatusを更新する
フォルダが登録済みなら何もしない
*/
void	ASCMFolderManager::RegisterFolders( const AObjectArray<AFileAcc>& inFolders, const AIndex inRootFolderIndex )
{
	//設定Offならリターン
	//これにより、設定Offの場合は、ASCMFolderDataが作成されないので、ASCMFolderDataのメソッドは一切実行されない
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == false  )   return;
	
	//ASCMFolderData生成
	for( AIndex i = 0; i < inFolders.GetItemCount(); i++ )
	{
		AIndex	index = GetSCMFolderDataIndexByFolder(inFolders.GetObjectConst(i));
		if( index == kIndex_Invalid )
		{
			AText	path;
			inFolders.GetObjectConst(i).GetPath(path);//#530 GetNormalizedPath(path);
			//.svnフォルダは対象外とする #0メモリ節約
			AIndex	p = 0;
			AText	svnfolder;
			svnfolder.Add(AFileAcc::GetPathDelimiter());
			svnfolder.AddCstring(".svn");
			if( path.FindText(0,svnfolder,p) == true )
			{
				continue;
			}
			//サブフォルダかどうかを取得
			ABool	isSubFolder = (i != inRootFolderIndex);
			//FolderDataを生成
			ASCMFolderDataFactory	factory(this,inFolders.GetObjectConst(i),isSubFolder);
			index = mSCMFolderDataArray.AddNewObject(factory);
			path.RemoveLastPathDelimiter();//#892
			mSCMFolderDataArray_PathText.Add(path);
		}
	}
	
	//Status更新
	if( inFolders.GetItemCount() > 0 )
	{
		AIndex	index = GetSCMFolderDataIndexByFolder(inFolders.GetObjectConst(inRootFolderIndex));
		mSCMFolderDataArray.GetObject(index).UpdateStatus();
	}
}

#pragma mark ===========================

#pragma mark ---Status制御

//#402
/**
ファイル個別にstatusを設定
ASCMFolderData::DoStatusCommandResult()からコールされる
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
ファイルのSCM状態を取得
@note AApp::SPI_GetProjectSCMStateArray()などからコールされる。アプリケーションactivate時等にコールされる。
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
ファイルからSCMフォルダタイプ取得
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
ファイルからRepositoryパス取得
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
ファイルからStatus更新
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

