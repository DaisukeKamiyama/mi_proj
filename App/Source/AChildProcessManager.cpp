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

AChildProcessManager

*/

#include "stdafx.h"

#include "AChildProcessManager.h"
#include "AApp.h"
//#include "AUtil.h"
#include "ADocument_IndexWindow.h"
#if IMPLEMENTATION_FOR_MACOSX
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#endif
#include "Cocoa.h"

#pragma mark ===========================
#pragma mark [クラス]AChildProcessPipe
#pragma mark ===========================
//子プロセスからのパイプ

#pragma mark ---コンストラクタ／デストラクタ

#if IMPLEMENTATION_FOR_MACOSX
AChildProcessPipe::AChildProcessPipe( const APipe inPipe, const APipe inPipeStderr, const APID inPID )  : AThread(&GetApp()), mPipe(inPipe), mPID(inPID), mNoPostInternalEvent(false), mPipeStderr(inPipeStderr)//#1231
{
	
}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
AChildProcessPipe::AChildProcessPipe( const HANDLE inHProcess, const HANDLE inHThread, 
			const HANDLE inHReadPipe )  
: AThread(&GetApp()), mHProcess(inHProcess), mHThread(inHThread), mHReadPipe(inHReadPipe)
{
}
#endif

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

//パイプ読み込み
void	AChildProcessPipe::NVIDO_ThreadMain()
{
#if IMPLEMENTATION_FOR_MACOSX
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AChildProcessPipe::NVIDO_ThreadMain started.",this);
	//fprintf(stderr,"thstart ");
	char line[4096];
	int len = 1;
	while( len != 0 )
	{
		//fprintf(stderr,"thloop ");
		len = read(mPipe,line,4096);
		if( len > 0 )
		{
			mText.AddFromTextPtr(line,len);
		}
		//fprintf(stderr,"%d ",len);
	}
	//標準エラー出力をパイプから取得 #1231
	len = 1;
	while( len != 0 )
	{
		len = read(mPipeStderr,line,4096);
		if( len > 0 )
		{
			mStderrText.AddFromTextPtr(line,len);
		}
		//AStCreateNSStringFromAText	str(mStderrText);
		//NSLog(str.GetNSString());
	}
	//
	close(mPipe);
	close(mPipeStderr);//#1231
	//
	if( waitpid(mPID,NULL,0) < 0 )
	{
		_ACError("waitpid error",NULL);
	}
	
	//fprintf(stderr,"thend ");
	//メインスレッドへイベント送信。イベントディスパッチからDeletePipe()がコールされる。
	if( mNoPostInternalEvent == false )//#1159
	{
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(GetObjectID());
		ABase::PostToMainInternalEventQueue(kInternalEvent_ChildProcessCompleted,GetObjectID(),0,mText,objectIDArray);
	}
	//B0314
	GetApp().NVI_WakeTrigger();
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AChildProcessPipe::NVIDO_ThreadMain ended.",this);
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	//パイプから結果読み出し
	char	buf[4096];
	while( true )
	{
		DWORD	readBytes = 0;
		if( ::ReadFile(mHReadPipe,buf,4096,&readBytes,NULL) == FALSE )   break;
		if( readBytes > 0 )
		{
			mText.AddFromTextPtr(buf,readBytes);
		}
	}
	::CloseHandle(mHReadPipe);
	//プロセス終了待ち
	::WaitForSingleObject(mHProcess,10*1000);//タイムアウトは10秒にしておく
	::CloseHandle(mHThread);
	::CloseHandle(mHProcess);
	//メインスレッドへイベント送信。イベントディスパッチからDeletePipe()がコールされる。
	AObjectIDArray	objectIDArray;
	objectIDArray.Add(GetObjectID());
	ABase::PostToMainInternalEventQueue(kInternalEvent_ChildProcessCompleted,GetObjectID(),0,mText,objectIDArray);
	//
	GetApp().NVI_WakeTrigger();
#endif
}


#pragma mark ===========================
#pragma mark [クラス]AChildProcessManager
#pragma mark ===========================
//子プロセス起動・管理

#pragma mark ---コンストラクタ／デストラクタ

AChildProcessManager::AChildProcessManager()
{
	//mPipeArray.EnableThreadSafe(false);
}
AChildProcessManager::~AChildProcessManager()
{
}

#pragma mark ===========================

#pragma mark ---子プロセス起動

AObjectID	AChildProcessManager::ExecuteChildProcess( const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath,
													   APID& outChildPID, const ABool inNoPostInternalEvent )//B0314 #1159
{
#if IMPLEMENTATION_FOR_MACOSX
	pid_t	pid;
	int	pfd[2];//pipe
	int pfd_stderr[2];//stderr受信用pipe #1231
	
	// 子→親のパイプを作成し、子でシェル実行、その標準出力を親で受け取る。
	// 子終了、または、タイマー満了で、親のブロック状態を解除する。
	if( pipe(pfd) < 0 )
	{
		return kObjectID_Invalid;
	}
	//stderr受信用pipe #1231
	if( pipe(pfd_stderr) < 0 )
	{
		return kObjectID_Invalid;
	}
	//
	pid = fork();
	if( pid > 0 ) //親プロセス
	{
		close(pfd[1]);//書き込み側クローズ
		close(pfd_stderr[1]);//書き込み側クローズ #1231
		
		AChildProcessPipeFactory	factory(pfd[0],pfd_stderr[0],pid);//#1231
		AIndex	index = mPipeArray.AddNewObject(factory);
		AObjectID	objectID = mPipeArray.GetObject(index).GetObjectID();
		mPipeArray.GetObject(index).SetNoInternalEvent(inNoPostInternalEvent);//#1159
		mPipeArray.GetObject(index).NVI_Run();
		
		//B0314
		outChildPID = pid;
		
		return objectID;
	}
	else if( pid == 0 ) //子プロセス
	{
		close(pfd[0]);//読み込み側クローズ
		close(pfd_stderr[0]);//書き込み側クローズ #1231
		//
		if( pfd[1] != STDOUT_FILENO )
		{
			if( dup2(pfd[1],STDOUT_FILENO) != STDOUT_FILENO )
			{
				_exit(0);
			}
			close(pfd[1]);
		}
		//#1231
		if( pfd_stderr[1] != STDERR_FILENO )
		{
			if( dup2(pfd_stderr[1],STDERR_FILENO) != STDERR_FILENO )
			{
				_exit(0);
			}
			close(pfd_stderr[1]);
		}
		//
		if( inChildDirectoryPath.GetItemCount() > 0 )//#1159
		{
			AStCreateCstringFromAText	dirPath(inChildDirectoryPath);
			chdir(dirPath.GetPtr());
		}
		AStCreateCstringFromAText	arg[256];
		ACstringPtr	argptr[256];
		AIndex argcount = 0;
		for( ; argcount < inArgArray.GetItemCount() && argcount < 255; argcount++ )
		{
			AText	text;
			inArgArray.Get(argcount,text);
			arg[argcount].Create(text);
			argptr[argcount] = const_cast<ACstringPtr>(arg[argcount].GetPtr());
		}
		argptr[argcount] = NULL;
		AStCreateCstringFromAText	command(inProcessPath);
		execv(command.GetPtr(),argptr);
		_exit(0);
	}
	else
	{
		_ACError("fork error",NULL);
		return kObjectID_Invalid;
	}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	//パイプ生成
	SECURITY_ATTRIBUTES	sa = {0};
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	HANDLE	hRead = 0;
	HANDLE	hWrite = 0;
	if( ::CreatePipe(&hRead,&hWrite,&sa,0) == false )   {_ACError("pipe create error",NULL);}
	//子側は書くだけ。hReadを継承させないために、hReadの継承属性を変更する。
	::DuplicateHandle(GetCurrentProcess(),hRead,GetCurrentProcess(),NULL,0,FALSE,DUPLICATE_SAME_ACCESS);//継承属性変更
	
	//パラメータ設定
	STARTUPINFOW	si = {0};
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.wShowWindow = SW_SHOWDEFAULT;
	si.hStdOutput = hWrite;
	si.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
	si.hStdError = ::GetStdHandle(STD_ERROR_HANDLE);
	PROCESS_INFORMATION	pi = {0};
	//コマンドライン生成
	AText	cmdline;
	cmdline.AddText(inProcessPath);
	cmdline.Add(kUChar_Space);
	for( AIndex i = 1; i < inArgArray.GetItemCount(); i++ )
	{
		AText	arg;
		arg.SetText(inArgArray.GetTextConst(i));
		//引数にスペースが含まれているか、空の場合は、""で囲む
		if( arg.IsCharContained(kUChar_Space) == true || arg.GetItemCount() == 0 )
		{
			arg.Insert1(0,'\"');
			arg.Add('\"');
		}
		//
		cmdline.AddText(arg);
		cmdline.Add(kUChar_Space);
	}
	//プロセス実行
	{
		AStCreateWcharStringFromAText	cmdlineptr(cmdline);
		AStCreateWcharStringFromAText	dirpathptr(inChildDirectoryPath);
		if( ::CreateProcessW(NULL,(LPWSTR)(cmdlineptr.GetPtr()),NULL,NULL,TRUE,//セキュリティ記述子、ハンドル継承
						CREATE_NO_WINDOW,//ウインドウ生成しない
						NULL,//環境ブロックは呼び出し側と同じ
						dirpathptr.GetPtr(),
				&si,&pi) == FALSE ) {DWORD errnum=GetLastError();_ACErrorNum("",errnum,NULL); return kObjectID_Invalid; }
	}
	//子プロセス起動したので、hWrite継承完了ということで、hWriteは閉じる。
	::CloseHandle(hWrite);
	//
	AChildProcessPipeFactory	factory(pi.hProcess,pi.hThread,hRead);
	AIndex	index = mPipeArray.AddNewObject(factory);
	AObjectID	objectID = mPipeArray.GetObject(index).GetObjectID();
	mPipeArray.GetObject(index).NVI_Run();
	//
	outChildPID = pi.dwProcessId;
	return objectID;
#endif
}

#pragma mark ===========================

#pragma mark ---

void	AChildProcessManager::DeletePipe( const AObjectID inPipeObjectID, const AText& inText )
{
	AIndex	index = mPipeArray.GetIndexByID(inPipeObjectID);
	if( index == kIndex_Invalid )   _ACThrow("",NULL);
	//標準エラー出力テキスト取得 #1231
	AText	stderrText;
	mPipeArray.GetObjectByID(inPipeObjectID).GetStderrText(stderrText);
	//
	mPipeArray.Delete1Object(index);
	if( mShellScript_PipeObjectID.Find(inPipeObjectID) != kIndex_Invalid )
	{
		DoPipeDeleted_ShellScript(inPipeObjectID,inText);
	}
	if( mTex_PipeObjectID.Find(inPipeObjectID) != kIndex_Invalid )
	{
		DoPipeDeleted_Tex(inPipeObjectID,inText);
	}
	//R0006
	if( mSCM_PipeObjectID.Find(inPipeObjectID) != kIndex_Invalid )
	{
		DoPipeDeleted_SCM(inPipeObjectID,inText);
	}
	//#361
	if( mAccessPlugin_PipeObjectID.Find(inPipeObjectID) != kIndex_Invalid )
	{
		DoPipeDeleted_AccessPlugin(inPipeObjectID,inText,stderrText);//#1231
	}
	//#455
	if( mGeneralSyncCommand_PipeObjectID.Find(inPipeObjectID) != kIndex_Invalid )
	{
		DoPipeDeleted_GeneralSyncCommand(inPipeObjectID,inText);
	}
	//#379
	if( mGeneralAsyncCommand_PipeObjectID.Find(inPipeObjectID) != kIndex_Invalid )
	{
		DoPipeDeleted_GeneralAsyncCommand(inPipeObjectID,inText);
	}
}

#pragma mark ===========================

#pragma mark ---シェルスクリプト

//
void	AChildProcessManager::ExecuteShellScript( const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath,
		const ADocumentID inTextDocumentID, const AIndex inInsertionIndex, const ANumber inTimerSecond, 
		const ABool inResultEncodingDirected, const ATextEncoding inResultEncoding, const ABool inDirty )//B0357
{
#if IMPLEMENTATION_FOR_MACOSX
	mShellScript_TextDocumentID.Add(inTextDocumentID);
	mShellScript_InsertionIndex.Add(inInsertionIndex);
	mShellScript_ResultEncodingDirected.Add(inResultEncodingDirected);
	mShellScript_ResultEncoding.Add(inResultEncoding);
	mShellScript_Dirty.Add(inDirty);
	APID	childPID;//B0314
	AObjectID	pipeObjectID = ExecuteChildProcess(inProcessPath,inArgArray,inChildDirectoryPath,childPID);
	mShellScript_PipeObjectID.Add(pipeObjectID);
	//B0314 シェルスクリプト終了、もしくは、タイマー経過までブロックさせる（ブロックしないと、ツールからシェルスクリプトコールされたときに、順番が従来と変わってしまう）
	if( GetApp().NVI_SleepWithTimer(inTimerSecond) == false )
	{
		//タイマー経過の場合は、子プロセスはkillする 検討必要：tcsh経由で無限ループ実行プロセスを起動した場合、tcshはkillできるが、その子プロセスをkillできない
		kill(childPID,SIGTERM);
	}
	//内部イベント実行（これにより、下のDoPipeDeleted_ShellScript()が直接コールされる）
	GetApp().EVT_DoInternalEvent();
#endif
}

//
void	AChildProcessManager::DoPipeDeleted_ShellScript( const AObjectID inPipeObjectID, const AText& inText )
{
	AIndex	index = mShellScript_PipeObjectID.Find(inPipeObjectID);
	ADocumentID	docID = mShellScript_TextDocumentID.Get(index);
	ATextIndex	insertionIndex = mShellScript_InsertionIndex.Get(index);
	ABool	resultEncodingDirected = mShellScript_ResultEncodingDirected.Get(index);
	ATextEncoding	resultEncoding = mShellScript_ResultEncoding.Get(index);
	ABool	dirty = mShellScript_Dirty.Get(index);//B0357
	//
	mShellScript_PipeObjectID.Delete1(index);
	mShellScript_TextDocumentID.Delete1(index);
	mShellScript_InsertionIndex.Delete1(index);
	mShellScript_ResultEncodingDirected.Delete1(index);
	mShellScript_ResultEncoding.Delete1(index);
	mShellScript_Dirty.Delete1(index);//B0357
	//
	if( insertionIndex != kIndex_Invalid )
	{
		AText	text;
		text.SetText(inText);
		if( resultEncodingDirected == true )
		{
			//==================テキストエンコーディング指定有りの場合==================
			//テキストエンコーディングをUTF8へ
			//テキストエンコーディング指定されているので、フォールバック有りにする
			ATextEncoding	rtec;
			AReturnCode	returnCode;
			ATextEncodingFallbackType	fallbackType;
			ABool	notMixed = true;//#995
			GetApp().SPI_ConvertToUTF8CR(text,resultEncoding,false,
										 true,//フォールバック
										 false,//SJISロスレスフォールバック
										 returnCode_LF,rtec,returnCode,fallbackType,notMixed);//B0400 #182 #307
		}
		else
		{
			//==================テキストエンコーディング指定無しの場合==================
			//改行コード変換
			text.ConvertReturnCodeToCR();
			//
			ATextEncoding	tec = ATextEncodingWrapper::GetSJISTextEncoding();
			text.GuessTextEncoding(tec);//B0390
			//テキストエンコーディングをUTF8へ
			//推測エンコーディングだが、この後、エンコーディング修正できないので、フォールバック有りにしておく
			ATextEncoding	rtec;
			AReturnCode	returnCode;
			ATextEncodingFallbackType	fallbackType;
			ABool	notMixed = true;//#995
			GetApp().SPI_ConvertToUTF8CR(text,tec,false,
										 true,//フォールバック
										 false,//SJISロスレスフォールバック
										 returnCode_LF,rtec,returnCode,fallbackType,notMixed);//B0400 #182 #307
		}
		GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(insertionIndex,text);
		GetApp().SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();//#92 SPI_SelectFirstWindow();
		GetApp().SPI_GetTextDocumentByID(docID).NVI_SetDirty(dirty);//B0357
		//B0314
		GetApp().SPI_GetTextDocumentByID(docID).SPI_SelectText(insertionIndex+text.GetItemCount(),0);
	}
}

#pragma mark ===========================

#pragma mark ---TeX

void	AChildProcessManager::CompileTex( const AFileAcc& inDocFile, const ABool inPreview )
{
	AFileAcc	folder;
	folder.SpecifyParent(inDocFile);
	AText	dirpath;
	folder.GetPath(dirpath);
	//%ParentFile判定
	//★対応検討
	//
	AText	command;
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kTexTypesetPath,command);
	//
	ATextArray	argArray;
	argArray.ExplodeFromText(command,kUChar_Space,true);
	AText	docpath;
	inDocFile.GetPath(docpath);
	argArray.Add(docpath);
	//
	APID	childPID;//B0314
	AObjectID	pipeObjectID = ExecuteChildProcess(argArray.GetTextConst(0),argArray,dirpath,childPID);
	mTex_PipeObjectID.Add(pipeObjectID);
	mTex_DocFile.GetObject(mTex_DocFile.AddNewObject()).CopyFrom(inDocFile);
	mTex_Preview.Add(inPreview);
}

void	AChildProcessManager::DoPipeDeleted_Tex( const AObjectID inPipeObjectID, const AText& inText )
{
	AIndex	index = mTex_PipeObjectID.Find(inPipeObjectID);
	AFileAcc	file;
	file.CopyFrom(mTex_DocFile.GetObject(index));
	ABool	preview = mTex_Preview.Get(index);
	//
	mTex_PipeObjectID.Delete1(index);
	mTex_DocFile.Delete1Object(index);
	mTex_Preview.Delete1(index);
	//
	DisplayTexCompileResult(file,preview);
}

void	AChildProcessManager::DisplayTexCompileResult( const AFileAcc& inDocFile, const ABool inPreview )
{
	//texドキュメントファイル読みだし
	AText	doctext;
	AModeIndex	modeIndex;
	AText	tecname;
	GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_TexCompileResult,inDocFile,doctext,modeIndex,tecname);//#723 ドキュメントからテキスト取得する必要が無いのでSPI_GetTextFromFileOrDocument()から変更
	//文法認識
	ATextInfo	textInfo(NULL);
	textInfo.SetMode(modeIndex);
	ABool	abortFlag = false;
	textInfo.CalcLineInfoAllWithoutView(doctext,abortFlag);//win
	//#695 AIndex	startLineIndex, endLineIndex;
	ABool	importChanged;
	AArray<AUniqID>	addedIdentifier;
	AArray<AIndex>		addedIdentifierLineIndex;
	//#695 AArray<AUniqID>	deletedIdentifier;//win
	AText	path;//#998
	inDocFile.GetPath(path);//#998
	textInfo.RecognizeSyntaxAll(doctext,path,//#695startLineIndex,endLineIndex, deletedIdentifier,
								addedIdentifier,addedIdentifierLineIndex,importChanged,abortFlag);//#698 #998
	
	//結果表示ウインドウ表示
	AText	wintitle;
	wintitle.SetLocalizedText("TexCompileResultWindowTitle");
	AText	grouptitle1, grouptitle2;
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kTexTypesetPath,grouptitle1);
	AText	datetime;
	ADateTimeWrapper::GetDateTimeText(datetime);
	grouptitle1.AddCstring("  (");
	grouptitle1.AddText(datetime);
	grouptitle1.AddCstring(")");
	ADocumentID	docID = GetApp().SPI_GetOrCreateFindResultWindowDocument();//#725
	AFileAcc	basefolder;//#465
	basefolder.SpecifyParent(inDocFile);//#465
	GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_InsertGroup(0,grouptitle1,grouptitle2,basefolder);//#465
	
	//
	AText	filename;
	inDocFile.GetFilename(filename);
	AText	docfilepath;
	inDocFile.GetPath(docfilepath);
	AText	origsuffix;
	filename.GetSuffix(origsuffix);
	AText	filenamewithoutsuffix;
	filenamewithoutsuffix.SetText(filename);
	filenamewithoutsuffix.Delete(filename.GetItemCount()-origsuffix.GetItemCount(),origsuffix.GetItemCount());
	AText	logfilename;
	logfilename.SetText(filenamewithoutsuffix);
	logfilename.AddCstring(".log");
	AFileAcc	logfile;
	logfile.SpecifySister(inDocFile,logfilename);
	AText	auxfilename;
	auxfilename.SetText(filenamewithoutsuffix);
	auxfilename.AddCstring(".aux");
	AFileAcc	auxfile;
	auxfile.SpecifySister(inDocFile,auxfilename);
	
	AText	auxfilepath;
	auxfile.GetPath(auxfilepath);
	GetApp().SPI_GetIndexWindowDocumentByID(docID).
			SPI_AddItem_TextPosition(0,auxfilepath,GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
			GetEmptyText(),//#465
			0,0,kIndex_Invalid,GetEmptyText());
	
	AText	logfilepath;
	logfile.GetPath(logfilepath);
	GetApp().SPI_GetIndexWindowDocumentByID(docID).
			SPI_AddItem_TextPosition(0,logfilepath,GetEmptyText(),GetEmptyText(),GetEmptyText(),GetEmptyText(),
			GetEmptyText(),//#465
			0,0,kIndex_Invalid,GetEmptyText());
	
	//
	ABool	errorExist = false;
	if( logfile.Exist() == true )
	{
		AText	logtext;
		logfile.ReadTo(logtext);
		logtext.ConvertLineEndToCR();
		AText	line;
		ABool	errorMode = false;
		AText	errStr;
		for( AIndex pos = 0; pos < logtext.GetItemCount(); )
		{
			logtext.GetLine(pos,line);
			if( line.GetItemCount() > 0 )
			{
				if( errorMode == true )
				{
					if( line.Get(0) == 'l' )
					{
						ANumber	lineNumber;
						AIndex	p = 2;
						line.ParseIntegerNumber(p,lineNumber);
						//
						AIndex	lineIndex = lineNumber-1;
						AIndex	spos = textInfo.GetLineStart(lineIndex);
						AItemCount	len = textInfo.GetLineLengthWithoutLineEnd(lineIndex);
						//
						AText	hit, prehit, posthit, positiontext;
						//段落Index等取得
						AIndex	paragraphIndex = textInfo.GetParagraphIndexByLineIndex(lineIndex);
						//場所情報取得
						//textInfo.GetPositionTextByLineIndex(lineIndex,positiontext);
						//
						GetApp().SPI_GetIndexWindowDocumentByID(docID).
								SPI_AddItem_TextPosition(0,docfilepath,hit,prehit,posthit,positiontext,
								GetEmptyText(),//#465
								spos,len,paragraphIndex,errStr);
						//
						errorExist = true;
						errorMode = false;
					}
					continue;
				}
				if( line.Get(0) == '!' )
				{
					errStr.SetText(line);
					errorMode = true;
					continue;
				}
				//
				if( line.CompareMin("Overfull") == true || line.CompareMin("Underful") == true )
				{
					errStr.SetText(line);
					ANumber	lineNumber = -1;
					for( AIndex p = 0; p < line.GetItemCount(); )
					{
						AText	token;
						line.GetToken(p,token);
						if( token.Compare("line") == true || token.Compare("lines") == true )
						{
							line.SkipTabSpace(p,line.GetItemCount());
							line.ParseIntegerNumber(p,lineNumber);
							break;
						}
					}
					AIndex	lineIndex = 0;
					AIndex	spos = 0;
					AItemCount	len = 0;
					if( lineNumber > 0 )
					{
						//
						/*B0000 AIndex	*/lineIndex = lineNumber-1;
						/*B0000 AIndex	*/spos = textInfo.GetLineStart(lineIndex);
						/*B0000 AItemCount	*/len = textInfo.GetLineLengthWithoutLineEnd(lineIndex);
					}
					//
					AText	hit, prehit, posthit, positiontext;
					//段落Index等取得
					AIndex	paragraphIndex = textInfo.GetParagraphIndexByLineIndex(lineIndex);
					//場所情報取得
					//textInfo.GetPositionTextByLineIndex(lineIndex,positiontext);
					//
					GetApp().SPI_GetIndexWindowDocumentByID(docID).
							SPI_AddItem_TextPosition(0,docfilepath,hit,prehit,posthit,positiontext,
							GetEmptyText(),//#465
							spos,len,paragraphIndex,errStr);
					continue;
				}
				if( line.CompareMin("LaTeX Warning:") == true )
				{
					if( line.GetLastChar() != '.' )
					{
						AText	line2;
						logtext.GetLine(pos,line2);
						line.AddText(line2);
					}
					errStr.SetText(line);
					ANumber	lineNumber = -1;
					for( AIndex p = 0; p < line.GetItemCount(); )
					{
						AText	token;
						line.GetToken(p,token);
						if( token.Compare("line") == true || token.Compare("lines") == true )
						{
							line.ParseIntegerNumber(p,lineNumber);
							break;
						}
					}
					AIndex	lineIndex = 0;
					AIndex	spos = 0;
					AItemCount	len = 0;
					if( lineNumber > 0 )
					{
						//
						/*B0000 AIndex	*/lineIndex = lineNumber-1;
						/*B0000 AIndex	*/spos = textInfo.GetLineStart(lineIndex);
						/*B0000 AItemCount	*/len = textInfo.GetLineLengthWithoutLineEnd(lineIndex);
					}
					//
					AText	hit, prehit, posthit, positiontext;
					//段落Index等取得
					AIndex	paragraphIndex = textInfo.GetParagraphIndexByLineIndex(lineIndex);
					//場所情報取得
					//textInfo.GetPositionTextByLineIndex(lineIndex,positiontext);
					//
					GetApp().SPI_GetIndexWindowDocumentByID(docID).
							SPI_AddItem_TextPosition(0,docfilepath,hit,prehit,posthit,positiontext,
							GetEmptyText(),//#465
							spos,len,paragraphIndex,errStr);
					errorExist = true;
				}
				if( line.CompareMin("No pages of output.") == true )
				{
					errStr.SetText(line);
					errorExist = true;
				}
			}
		}
	}
	if( errorExist == false && inPreview == true )
	{
		GetApp().SPI_TexPreview(inDocFile);
	}
	
}

#pragma mark ===========================

#pragma mark ---SCM
//R0006

//
void	AChildProcessManager::ExecuteSCMCommand( const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath,
		const ASCMCommandType inCommandType, const AFileAcc& inFolderOrFile, const ADocumentID inRequestorDocumentID )//#736
{
	APID	childPID;
	AObjectID	pipeObjectID = ExecuteChildProcess(inProcessPath,inArgArray,inChildDirectoryPath,childPID);
	AIndex	index = mSCM_FolderOrFile.AddNewObject();
	mSCM_FolderOrFile.GetObject(index).CopyFrom(inFolderOrFile);
	mSCM_PipeObjectID.Add(pipeObjectID);
	mSCM_CommandType.Add(inCommandType);
	mSCM_RequestorDocumentID.Add(inRequestorDocumentID);//#736
	//if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ExecuteSCMCommand() done.",NULL);
}

//
void	AChildProcessManager::DoPipeDeleted_SCM( const AObjectID inPipeObjectID, const AText& inText )
{
	AIndex	index = mSCM_PipeObjectID.Find(inPipeObjectID);
	AFileAcc	folderorfile;
	folderorfile.CopyFrom(mSCM_FolderOrFile.GetObjectConst(index));
	ASCMCommandType	commandType = mSCM_CommandType.Get(index);
	ADocumentID	requestorDocID = mSCM_RequestorDocumentID.Get(index);//#736
	//
	mSCM_PipeObjectID.Delete1(index);
	mSCM_FolderOrFile.Delete1Object(index);
	mSCM_CommandType.Delete1(index);
	mSCM_RequestorDocumentID.Delete1(index);//#736
	//
	switch(commandType)
	{
	  case kSCMCommandType_Status:
		{
			GetApp().SPI_GetSCMFolderManager().DoStatusCommandResult(folderorfile,inText);
			break;
		}
	  case kSCMCommandType_Compare:
		{
			//#736 GetApp().SPI_GetSCMFolderManager().DoCompareResult(folderorfile,inText);
			//要求元ドキュメントのSPI_DoSCMCompareResult()を実行する #736（前はSCMFolderManager経由だったが、#736で必ずしもdocumentのファイルとscmファイルが一致しなくなったので、変更）
			if( requestorDocID != kObjectID_Invalid )
			{
				if( GetApp().NVI_IsDocumentValid(requestorDocID) == true )
				{
					if( GetApp().NVI_GetDocumentTypeByID(requestorDocID) == kDocumentType_Text )
					{
						GetApp().SPI_GetTextDocumentByID(requestorDocID).SPI_DoSCMCompareResult(inText);//#379
					}
				}
			}
			break;
		}
		/*#455
	  case kSCMCommandType_Sync:
		{
			mSCMResultText.SetText(inText);
			break;
		}
		*/
	  default:
		{
			_ACError("",NULL);
			break;
		}
	}
	//if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("DoPipeDeleted_SCM() done.",NULL);
}

#pragma mark ===========================

#pragma mark ---コマンド実行（汎用・同期）
//#455

/**
コマンド実行（汎用・同期）
*/
void	AChildProcessManager::ExecuteGeneralSyncCommand( const AText& inProcessPath, const ATextArray& inArgArray, 
		const AText& inChildDirectoryPath, AText& outText, const ANumber inTimerSecond )
{
	APID	childPID;
    AObjectID	pipeObjectID = ExecuteChildProcess(inProcessPath,inArgArray,inChildDirectoryPath,childPID,true);//#1159 pipe切断時に内部イベント送信しないようにする
	mGeneralSyncCommand_PipeObjectID.Add(pipeObjectID);
	//#525 同期コマンド実行の複数同時実行時のバグ修正 mGeneralSyncCommandResultText.DeleteAll();
	mGeneralSyncCommand_ResultText.Add(GetEmptyText());//#525
	//pipeスレッドの終了待ち
	for( AIndex i = 0; i < inTimerSecond; i++ )
	{
		if( mPipeArray.GetObjectByID(pipeObjectID).NVI_IsThreadWorking() ==false )   break;
		GetApp().NVI_SleepWithTimer(1);//pipeスレッド終了したらwakeされる
	}
	//内部イベント実行（これにより、下のDoPipeDeleted_GeneralSyncCommand()が直接コールされる） #1159により処理削除（内部イベントは経由しないようにする）
	//#1159 GetApp().EVT_DoInternalEvent();
	//結果テキスト取得 #1159
	AText	resultText;
	mPipeArray.GetObjectByID(pipeObjectID).GetResultText(resultText);
	//内部イベントから実行されていた処理と同じことを実行 #1159
	DeletePipe(pipeObjectID,resultText);
	
	//#525 同期コマンド実行の複数同時実行時のバグ修正 outText.SetText(mGeneralSyncCommandResultText);
	//#525 同期コマンド実行の複数同時実行時のバグ修正 mGeneralSyncCommandResultText.DeleteAll();
	//#525 旧方式だと、mGeneralSyncCommandResultText設定後に、
	//EVT_DoInternalEvent()内から、ExecuteGeneralSyncCommand()がコールされて、mGeneralSyncCommandResultTextがクリアされてしまう
	
	//#525 結果テキストを配列から取得
	AIndex	index = mGeneralSyncCommand_PipeObjectID.Find(pipeObjectID);
	mGeneralSyncCommand_ResultText.Get(index,outText);
	
	//mGeneralSyncCommand配列を削除
	mGeneralSyncCommand_PipeObjectID.Delete1(index);
	mGeneralSyncCommand_ResultText.Delete1(index);
}

/**
パイプ切断時コールバック処理(InternalEvent経由)
*/
void	AChildProcessManager::DoPipeDeleted_GeneralSyncCommand( const AObjectID inPipeObjectID, const AText& inText )
{
	AIndex	index = mGeneralSyncCommand_PipeObjectID.Find(inPipeObjectID);
	//#525 同期コマンド実行の複数同時実行時のバグ修正 mGeneralSyncCommand_PipeObjectID.Delete1(index);
	//#525 同期コマンド実行の複数同時実行時のバグ修正 mGeneralSyncCommandResultText.SetText(inText);
	//#525 mGeneralSyncCommand配列はAChildProcessManager::ExecuteGeneralSyncCommand()内で削除。
	//#525 結果テキストを配列に設定
	mGeneralSyncCommand_ResultText.Set(index,inText);
}

#pragma mark ===========================

#pragma mark ---コマンド実行（汎用・非同期）
//#379

/**
コマンド実行（汎用・非同期）
*/
void	AChildProcessManager::ExecuteGeneralAsyncCommand( 
		const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath, 
		const ADocumentID inRetrunDocID, const AGeneralAsyncCommandType inCommandType )
{
	APID	childPID;
	AObjectID	pipeObjectID = ExecuteChildProcess(inProcessPath,inArgArray,inChildDirectoryPath,childPID);
	mGeneralAsyncCommand_PipeObjectID.Add(pipeObjectID);
	mGeneralAsyncCommand_CommandType.Add(inCommandType);
	mGeneralAsyncCommand_DocumentID.Add(inRetrunDocID);
}

/**
パイプ切断時コールバック処理(InternalEvent経由)
*/
void	AChildProcessManager::DoPipeDeleted_GeneralAsyncCommand( const AObjectID inPipeObjectID, const AText& inText )
{
	AIndex	index = mGeneralAsyncCommand_PipeObjectID.Find(inPipeObjectID);
	AGeneralAsyncCommandType	commandType = mGeneralAsyncCommand_CommandType.Get(index);
	ADocumentID	docID = mGeneralAsyncCommand_DocumentID.Get(index);
	//待ちデータ削除
	mGeneralAsyncCommand_PipeObjectID.Delete1(index);
	mGeneralAsyncCommand_CommandType.Delete1(index);
	mGeneralAsyncCommand_DocumentID.Delete1(index);
	//ドキュメントで結果実行
	if( AApplication::GetApplication().NVI_IsDocumentValid(docID) == true )//#0 pipe切断時（diff等終了時）にドキュメントが既に閉じられている場合があるので判定必要
	{
		AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_DoGeneralAsyncCommandResult(commandType,inText);
	}
}

#pragma mark ===========================

#pragma mark ---接続プラグイン
//#193

/**
接続プラグイン実行
*/
void	AChildProcessManager::ExecuteAccessPlugin( const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath,
		const AObjectID inRemoteConnectionObjectID )
{
	APID	childPID;
	AObjectID	pipeObjectID = ExecuteChildProcess(inProcessPath,inArgArray,inChildDirectoryPath,childPID);
	mAccessPlugin_PipeObjectID.Add(pipeObjectID);
	mAccessPlugin_RemoteConnectionObjectID.Add(inRemoteConnectionObjectID);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ExecuteAccessPlugin() done.",NULL);
}

//
void	AChildProcessManager::DoPipeDeleted_AccessPlugin( const AObjectID inPipeObjectID, const AText& inText, const AText& inStderrText )//#1231
{
	AIndex	index = mAccessPlugin_PipeObjectID.Find(inPipeObjectID);
	AObjectID	remoteConnectionObjectID = mAccessPlugin_RemoteConnectionObjectID.Get(index);
	//
	mAccessPlugin_PipeObjectID.Delete1(index);
	mAccessPlugin_RemoteConnectionObjectID.Delete1(index);
	//
	GetApp().SPI_DoAccessPluginResponseReceived(remoteConnectionObjectID,inText,inStderrText);//#1231
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("DoPipeDeleted_AccessPlugin() done.",NULL);
}
