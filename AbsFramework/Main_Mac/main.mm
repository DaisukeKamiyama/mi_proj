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
	//SIGPIPE�𖳎�
	::signal(SIGPIPE, SIG_IGN);
	
	//ABase�I�u�W�F�N�g�𐶐�����B
	ABase	base(true);
	CAppImp::Setup(argc,argv);//#263
	
	//#263 �R�}���h���C�����s���̏d���N���`�F�b�N
	ABool	commandline = false;
	if( argc > 1 )
	{
		//#495 �ʏ�N�����A������-psn_0_xxxxxxxx������̂ł��̂Ƃ��̓R�}���h���C�����s�ΏۊO�Ƃ��Ă���
		//�R�}���h���C�����s�t���OOn
		commandline = true;
		//-psn�Ŏn�܂�ꍇ�̓t���OOff�ɂ���
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
		//�d���N���`�F�b�N���A�d���N���Ȃ���������̃C���X�^���X�֑����ďI�� 
		
		//���v���Z�XPSN�擾
		ProcessSerialNumber	selfPSN = {0};
		::GetCurrentProcess(&selfPSN);
		//���p�X�擾
		AFileAcc	appFile;
		AFileWrapper::GetAppFile(appFile);
		AText	selfPath;
		appFile.GetPath(selfPath);
		//�����t�@�C���p�X������s�����C���X�^���X������
		ProcessSerialNumber	psn = {0};
		psn.lowLongOfPSN = kNoProcess;
		psn.highLongOfPSN = kNoProcess;
		ABool	found = false;
		while( found == false )
		{
			OSErr	status = ::GetNextProcess(&psn);
			//�����͏��O
			Boolean	same = FALSE;
			::SameProcess(&psn,&selfPSN,&same);
			if( same == TRUE )   continue;
			//
			ProcessInfoRec	info = {0};
			info.processInfoLength = sizeof(ProcessInfoRec);
			info.processName = NULL;
#ifdef __LP64__
			//64bit�p���� #1034
			//�i64bit�p�́AprocessAppSpec�̑���ɁAprocessAppRef�ɂȂ��Ă���j
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
			//32bit�p����
			FSSpec	spec = {0};
			info.processAppSpec = &spec;
			if( status == noErr ) 
			{
				status = ::GetProcessInformation(&psn,&info);
			}
			if( status == noErr && spec.name[0] > 0 ) //#495 ClassicOS�v���Z�X��spec�Ƀt�@�C��spec������Ȃ�
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
		//�d���N���Ȃ�AE���M���ďI��
		if( found == true )
		{
			//�����擾
			ATextArray	textArray;
			CAppImp::GetCmdLineArgTextArray(textArray);
			//CR��؂�e�L�X�g�֕ϊ�
			AText	text;
			//���݃f�B���N�g����ݒ�i���M��̃v���Z�X�̌��݃f�B���N�g���͂��̃v���Z�X�̌��݃f�B���N�g���Ƃ͈قȂ邽�߁A���M��ɓ`����j #1111
			AText	curdir;
			ACocoa::SetTextFromNSString([[NSFileManager defaultManager] currentDirectoryPath],curdir);
			text.AddCstring(":");
			text.AddText(curdir);
			text.Add(kUChar_CR);
			//������ݒ�
			for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
			{
				text.AddText(textArray.GetTextConst(i));
				text.Add(kUChar_CR);
			}
			//AE���M
			AAppAcc	appAcc(appFile);
			ALaunchWrapper::SendAppleEvent_Text(appAcc,'misc','args',text);
			//�I��
			if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[main exited (duplicated launch)]");
			return 0;
		}
	}
	
	//#688
	
	//Cocoa���C�����[�`�����R�[��
	//AApp�I�u�W�F�N�g��AppDelegate::awakeFromNib�Ő���
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[Before calling NSApplicationMain()]");
	return NSApplicationMain(argc,  (const char **) argv);
	//NSApplicationMain()�̂��Ƃ́A�I�����Ă�main()�ɂ͖߂��Ă��Ȃ��B
	//���LURL��NSApplication::terminate:�̍����Q�Ƃ̂���
	//https://developer.apple.com/library/mac/#documentation/Cocoa/Reference/ApplicationKit/Classes/NSApplication_Class/Reference/Reference.html
	//AApp�I�u�W�F�N�g�̉��������AppDelegate::applicationWillTerminate�ōs��
	
	/* #688 Cocoa��
	//�A�v���P�[�V�����I�u�W�F�N�g�𐶐�����B
	//�R���X�g���N�^���ŁAgApp��this���ݒ肳��A����ȍ~�AGetApp()���L���ƂȂ�B
	//�X�^�b�N�t���[����AApp������iAApp app�j�̂͂܂����悤���BOSADoScriptFile��-2706(stack error)�ƂȂ�
	try
	{
	AApp	*app = new AApp;
	
	//�A�v���P�[�V�������s
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

