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

#pragma once

#include "../../AbsFramework/Frame.h"

//#361 ABaseTypes.hへ移動 typedef int APipe;
/*ABaseTypes.hへ移動
#if IMPLEMENTATION_FOR_MACOSX
typedef pid_t	APID;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
typedef DWORD APID;
#endif
*/

#pragma mark ===========================
#pragma mark [クラス]AChildProcessPipe
//
#if IMPLEMENTATION_FOR_MACOSX
class AChildProcessPipe : public AThread
{
  public:
	AChildProcessPipe( const APipe inPipe, const APipe inPipeStderr, const APID inPID );//#1231
	
	//#1159
	//内部イベント送信要否
	void	SetNoInternalEvent( const ABool inNoPostInternalEvent ) { mNoPostInternalEvent = inNoPostInternalEvent; }
	//結果テキスト取得
	void	GetResultText( AText& outText ) { outText.SetText(mText); }
	void	GetStderrText( AText& outText ) { outText.SetText(mStderrText); }//#1231
	//
  private:
	void			NVIDO_ThreadMain();
	
	//
  private:
	APipe						mPipe;
	APipe						mPipeStderr;//#1231
	APID						mPID;
	AText						mText;
	AText						mStderrText;//#1231
	ABool						mNoPostInternalEvent;
};
#endif
#if IMPLEMENTATION_FOR_WINDOWS
class AChildProcessPipe : public AThread
{
  public:
	AChildProcessPipe( const HANDLE inHProcess, const HANDLE inHThread, const HANDLE inHReadPipe );
	
	//
  private:
	void			NVIDO_ThreadMain();
	
	//
  private:
	HANDLE						mHProcess;
	HANDLE						mHThread;
	HANDLE						mHReadPipe;
	AText						mText;
};
#endif

#pragma mark ===========================
#pragma mark [クラス]AChildProcessPipeFactory
//
#if IMPLEMENTATION_FOR_MACOSX
class AChildProcessPipeFactory : public AAbstractFactoryForObjectArray<AChildProcessPipe>
{
  public:
	AChildProcessPipeFactory( const APipe inPipe, const APipe inPipeStderr, const APID inPID )
	: mPipe(inPipe), mPipeStderr(inPipeStderr), mPID(inPID)//#1231
	{
	}
	AChildProcessPipe*	Create() 
	{
		return new AChildProcessPipe(mPipe,mPipeStderr,mPID);//#1231
	}
  private:
	APipe			mPipe;
	APipe			mPipeStderr;//#1231
	APID			mPID;
};
#endif
#if IMPLEMENTATION_FOR_WINDOWS
class AChildProcessPipeFactory : public AAbstractFactoryForObjectArray<AChildProcessPipe>
{
  public:
	AChildProcessPipeFactory( const HANDLE inHProcess, const HANDLE inHThread, 
			const HANDLE inHReadPipe )  
	: mHProcess(inHProcess), mHThread(inHThread), mHReadPipe(inHReadPipe)
	{
	}
	AChildProcessPipe*	Create() 
	{
		return new AChildProcessPipe(mHProcess,mHThread,mHReadPipe);
	}
  private:
	HANDLE			mHProcess;
	HANDLE			mHThread;
	HANDLE			mHReadPipe;
};
#endif

#pragma mark ===========================
#pragma mark [クラス]AChildProcessManager

enum ASCMCommandType
{
	kSCMCommandType_Status = 0,
	kSCMCommandType_Compare 
	/*#455,
	kSCMCommandType_Sync
	*/
};

//#379
const AGeneralAsyncCommandType	kGeneralAsyncCommandType_Diff = 0;

//
class AChildProcessManager
{
	//コンストラクタ／デストラクタ
  public:
	AChildProcessManager();
	~AChildProcessManager();
	
	//子プロセス起動
  public:
	AObjectID		ExecuteChildProcess( const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath,
										 APID& outChildPID, const ABool inNoPostInternalEvent = false );//B0314
	
	//
  public:
	void			DeletePipe( const AObjectID inPipeObjectID, const AText& inText );
	
	//シェルスクリプト
  public:
	void			ExecuteShellScript( const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath,
			const ADocumentID inTextDocumentID, const AIndex inInsertionIndex, const ANumber inTimerSecond, 
			const ABool inResultEncodingDirected, const ATextEncoding inResultEncoding, const ABool inDirty );//B0357
	void			DoPipeDeleted_ShellScript( const AObjectID inPipeObjectID, const AText& inText );
  private:
	AArray<AObjectID>					mShellScript_PipeObjectID;
	AArray<ADocumentID>					mShellScript_TextDocumentID;
	AArray<ATextIndex>					mShellScript_InsertionIndex;
	AArray<ABool>						mShellScript_ResultEncodingDirected;
	AArray<ATextEncoding>				mShellScript_ResultEncoding;
	AArray<ABool>						mShellScript_Dirty;//B0357
	
	//TeX
  public:
	void			CompileTex( const AFileAcc& inDocFile, const ABool inPreview );
	void			DoPipeDeleted_Tex( const AObjectID inPipeObjectID, const AText& inText );
  private:
	void			DisplayTexCompileResult( const AFileAcc& inDocFile, const ABool inPreview );
	AArray<AObjectID>					mTex_PipeObjectID;
	AObjectArray<AFileAcc>				mTex_DocFile;
	AArray<ABool>						mTex_Preview;
	
	//SCM R0006
  public:
	void			ExecuteSCMCommand( const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath,
					const ASCMCommandType inCommandType, const AFileAcc& inFolder, const ADocumentID inRequestorDocumentID );//#736
	/*#455 void			ExecuteSCMCommandSync( const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath,
					const AFileAcc& inFolder, AText& outText );*/
	void			DoPipeDeleted_SCM( const AObjectID inPipeObjectID, const AText& inText );
  private:
	AArray<AObjectID>					mSCM_PipeObjectID;
	AObjectArray<AFileAcc>				mSCM_FolderOrFile;
	AArray<ADocumentID>					mSCM_RequestorDocumentID;//#736
	AArray<ASCMCommandType>				mSCM_CommandType;
	//#455 AText								mSCMResultText;
	
	//コマンド実行（汎用・同期） #455
  public:
	void			ExecuteGeneralSyncCommand( const AText& inProcessPath, const ATextArray& inArgArray, 
					const AText& inChildDirectoryPath, AText& outText, const ANumber inTimerSecond = 20 );
  private:
	void			DoPipeDeleted_GeneralSyncCommand( const AObjectID inPipeObjectID, const AText& inText );
	AArray<AObjectID>					mGeneralSyncCommand_PipeObjectID;
	ATextArray							mGeneralSyncCommand_ResultText;//#525 同期コマンド実行の複数同時実行時のバグ修正
	
	//コマンド実行（汎用・非同期） #379
  public:
	void			ExecuteGeneralAsyncCommand( 
					const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath, 
					const ADocumentID inRetrunDocID, const AGeneralAsyncCommandType inCommandType );
  private:
	void			DoPipeDeleted_GeneralAsyncCommand( const AObjectID inPipeObjectID, const AText& inText );
	AArray<AObjectID>					mGeneralAsyncCommand_PipeObjectID;
	AArray<AGeneralAsyncCommandType>	mGeneralAsyncCommand_CommandType;
	AArray<ADocumentID>					mGeneralAsyncCommand_DocumentID;
	
	//アクセスプラグイン #361
  public:
	void			ExecuteAccessPlugin( const AText& inProcessPath, const ATextArray& inArgArray, const AText& inChildDirectoryPath,
					const AObjectID inRemoteConnectionObjectID );
	void			DoPipeDeleted_AccessPlugin( const AObjectID inPipeObjectID, const AText& inText, const AText& inStderrText );//#1231
  private:
	AArray<AObjectID>					mAccessPlugin_PipeObjectID;
	AArray<AObjectID>					mAccessPlugin_RemoteConnectionObjectID;
	
  private:
	AObjectArrayIDIndexed<AChildProcessPipe>		mPipeArray;//#693
};






