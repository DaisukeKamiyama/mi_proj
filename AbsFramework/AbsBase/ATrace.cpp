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

ATrace
����́^�f�o�b�O�̂��߁A�g���[�X���O���̎悷��B
Thread-Safe

*/

#include "stdafx.h"

#include "../AbsBase/ABase.h"
#if IMPLEMENTATION_FOR_MACOSX
#include <execinfo.h>
#endif

extern void GetApplicationName( AText& outName );

//For debug
ABool	gTraceAlwaysOutputToStderr = false;

//B0000
ABool	gStartPostInternalEvent = false;

#pragma mark ===========================
#pragma mark [���O���]ATrace
#pragma mark ===========================
//�g���[�X���O���̎悷��

void	ATrace::Log( const AConstCharPtr inTraceType, const ABool inStderr, const AConstCharPtr inFilename, const ANumber inLineNumber, 
		const AConstCharPtr inString, const ABool inShowLogWindow, const AObjectArrayItem* inObjectArrayItem, const ABool inBacktrace )//B0000
{
	//���O�e�L�X�g�쐬
	AText	datetime;
	ADateTimeWrapper::GetDateTimeText(datetime);
	AText	log;
	log.SetCstringAndText(inTraceType,datetime,"   Reason:\"");
	log.AddCstring(inString);
	log.AddFormattedCstring("\"   File:\"%s\" Line:%d\r",inFilename,inLineNumber);
	//
	AText	objectInfoText;
	const AObjectArrayItem*	obj = inObjectArrayItem;
	AText	spaces;
	while( obj != NULL )
	{
		objectInfoText.InsertText(0,spaces);
		AText	text;
		text.SetFormattedCstring("   Class:%s \r",obj->GetClassName());
		objectInfoText.InsertText(0,text);
		obj = obj->GetParent();
		spaces.Add(kUChar_Space);
	}
	log.AddText(objectInfoText);
	if( inBacktrace == true )
	{
#if !BUILD_FOR_MACOSX_10_3
#if IMPLEMENTATION_FOR_MACOSX
		//backtrace
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
		{
			//���Ȃ��Ƃ�Xcode3.1�ł�Fix&Continue��Enable�ɂ��Ȃ��ƃV���{���\������Ȃ����L��
			void*	callstack[128];
			int	frames = backtrace(callstack,128);
			char**	strs = backtrace_symbols(callstack,frames);
			for( AIndex i = 0; i < frames; i++ )
			{
				log.AddFormattedCstring("   %s\r",strs[i]);
			}
			free(strs);
		}
#endif
#endif
	}
	//����ɏ��ǉ��H
	//�����ǉ�����Ȃ�AMutex���b�N�����������Ȋ֐��R�[�������Ȃ��悤���ӂ���B
	//Mutex���b�N����_ACThrow�������Ă����ɗ��čēx����Mutex�����b�N����댯��������B
	//
	log.AddCstring("\r");
	
	if( gStartPostInternalEvent == true )
	{
	//���O�ǉ��C�x���g�����C�������L���[�ɐς�
	//�i�G���[�����������̂��T�u�X���b�h�������ꍇ�́A���ځA���O�E�C���h�E���𐧌䂷�邱�Ƃ��o���Ȃ����߁j
	ABase::PostToMainInternalEventQueue(kInternalEvent_TraceLog,kObjectID_Invalid,0,log,GetEmptyObjectIDArray());
	//���O�E�C���h�E��\������C�x���g�𑗏o
	if( inShowLogWindow == true )
	{
		ABase::PostToMainInternalEventQueue(kInternalEvent_ShowTraceLog,kObjectID_Invalid,0,GetEmptyText(),GetEmptyObjectIDArray());
	}
	}
	
	//�W���G���[�o�͂ɏo��
	if( inStderr == true || gTraceAlwaysOutputToStderr == true )
	{
		AText	name;
		GetApplicationName(name);
		name.OutputToStderr();
		fprintf(stderr,":");
		log.OutputToStderr();
	}
}

//For debug
void	ATrace::SetTraceAlwaysOutputToStderr()
{
	gTraceAlwaysOutputToStderr = true;
}

//B0000
void	ATrace::StartPostInternalEvent()
{
	gStartPostInternalEvent = true;
}

