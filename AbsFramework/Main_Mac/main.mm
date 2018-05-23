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

main.cpp

*/

#include "../../AbsFramework/Frame.h"
//#include "../../App/Source/AApp.h"
#import <Cocoa/Cocoa.h>
#include "../AbsBase/Cocoa.h"

int	main( int argc, const char **argv )
{
	//SIGPIPEを無視
	::signal(SIGPIPE, SIG_IGN);
	
	//ABaseオブジェクトを生成する。
	ABase	base(true);
	CAppImp::Setup(argc,argv);//#263
	
	//#263 コマンドライン実行時の重複起動チェック
	ABool	commandline = false;
	if( argc > 1 )
	{
		//#495 通常起動時、引数に-psn_0_xxxxxxxxが入るのでこのときはコマンドライン実行対象外としておく
		//コマンドライン実行フラグOn
		commandline = true;
		//-psnで始まる場合はフラグOffにする
		AText	argv1;
		argv1.AddCstring(argv[1]);
		if( argv1.GetItemCount() > 4 )
		{
			if( argv1.CompareMin("-psn") == true )
			{
				commandline = false;
			}
		}
	}
	if( commandline == true )
	{
		//重複起動チェックし、重複起動なら引数をそのインスタンスへ送って終了 
		
		//自プロセスPSN取得
		ProcessSerialNumber	selfPSN = {0};
		::GetCurrentProcess(&selfPSN);
		//自パス取得
		AFileAcc	appFile;
		AFileWrapper::GetAppFile(appFile);
		AText	selfPath;
		appFile.GetPath(selfPath);
		//同じファイルパスから実行したインスタンスを検索
		ProcessSerialNumber	psn = {0};
		psn.lowLongOfPSN = kNoProcess;
		psn.highLongOfPSN = kNoProcess;
		ABool	found = false;
		while( found == false )
		{
			OSErr	status = ::GetNextProcess(&psn);
			//自分は除外
			Boolean	same = FALSE;
			::SameProcess(&psn,&selfPSN,&same);
			if( same == TRUE )   continue;
			//
			ProcessInfoRec	info = {0};
			info.processInfoLength = sizeof(ProcessInfoRec);
			info.processName = NULL;
#ifdef __LP64__
			//64bit用処理 #1034
			//（64bit用は、processAppSpecの代わりに、processAppRefになっている）
			FSRef	fsref = {0};
			info.processAppRef = &fsref;
			if( status == noErr ) 
			{
				status = ::GetProcessInformation(&psn,&info);
			}
			if( status == noErr )
			{
				AFileAcc	file;
				file.SpecifyByFSRef(fsref);
				AText	path;
				file.GetPath(path);
				if( path.CompareMin(selfPath) == true )
				{
					found = true;
					::SetFrontProcess(&psn);
					break;
				}
			}
			else break;
#else
			//32bit用処理
			FSSpec	spec = {0};
			info.processAppSpec = &spec;
			if( status == noErr ) 
			{
				status = ::GetProcessInformation(&psn,&info);
			}
			if( status == noErr && spec.name[0] > 0 ) //#495 ClassicOSプロセスはspecにファイルspecが入らない
			{
				AFileAcc	file;
				file.SpecifyByFSSpec(spec);
				AText	path;
				file.GetPath(path);
				if( path.CompareMin(selfPath) == true )
				{
					found = true;
					::SetFrontProcess(&psn);
					break;
				}
			}
			else break;
#endif
		}
		//重複起動ならAE送信して終了
		if( found == true )
		{
			//引数取得
			ATextArray	textArray;
			CAppImp::GetCmdLineArgTextArray(textArray);
			//CR区切りテキストへ変換
			AText	text;
			//現在ディレクトリを設定（送信先のプロセスの現在ディレクトリはこのプロセスの現在ディレクトリとは異なるため、送信先に伝える） #1111
			AText	curdir;
			ACocoa::SetTextFromNSString([[NSFileManager defaultManager] currentDirectoryPath],curdir);
			text.AddCstring(":");
			text.AddText(curdir);
			text.Add(kUChar_CR);
			//引数を設定
			for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
			{
				text.AddText(textArray.GetTextConst(i));
				text.Add(kUChar_CR);
			}
			//AE送信
			AAppAcc	appAcc(appFile);
			ALaunchWrapper::SendAppleEvent_Text(appAcc,'misc','args',text);
			//終了
			if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[main exited (duplicated launch)]");
			return 0;
		}
	}
	
	//#688
	
	//Cocoaメインルーチンをコール
	//AAppオブジェクトはAppDelegate::awakeFromNibで生成
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[Before calling NSApplicationMain()]");
	return NSApplicationMain(argc,  (const char **) argv);
	//NSApplicationMain()のあとは、終了してもmain()には戻ってこない。
	//下記URLのNSApplication::terminate:の項を参照のこと
	//https://developer.apple.com/library/mac/#documentation/Cocoa/Reference/ApplicationKit/Classes/NSApplication_Class/Reference/Reference.html
	//AAppオブジェクトの解放処理はAppDelegate::applicationWillTerminateで行う
	
	/* #688 Cocoa化
	//アプリケーションオブジェクトを生成する。
	//コンストラクタ内で、gAppにthisが設定され、これ以降、GetApp()が有効となる。
	//スタックフレームにAAppをつくる（AApp app）のはまずいようだ。OSADoScriptFileで-2706(stack error)となる
	try
	{
	AApp	*app = new AApp;
	
	//アプリケーション実行
	app->NVI_Run();
	
	delete app;
	}
	catch(...)
	{
	fprintf(stderr,"main() catched throw");
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[main exited]");
	}
	return 0;
	*/
}

