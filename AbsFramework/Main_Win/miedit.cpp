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
// miedit.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "miedit.h"
#include "../AbsBase/ABase.h"
#include "../../App/Source/AApp.h"

const wchar_t*	kIdForDropFromAnother = L"mieditdaisukekamiyamamessageonly57658352";
DWORD	kMemoryMappedFileSizeForDropFromAnother = 4096*sizeof(wchar_t);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//�d���N���`�F�b�N
	HWND	hwnd = ::FindWindowEx(NULL,NULL,kIdForDropFromAnother,kIdForDropFromAnother);
	if( hwnd != NULL )
	{
		HANDLE	hmap = ::CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,
				0,kMemoryMappedFileSizeForDropFromAnother,kIdForDropFromAnother);
		if( hmap == NULL )   return 0;
		wchar_t*	ptr = (wchar_t*)MapViewOfFile(hmap,FILE_MAP_ALL_ACCESS,0,0,0);
		if( ptr == NULL )    {::CloseHandle(hmap);return 0;}
		wcscpy_s(ptr,4096,lpCmdLine);
		::SendMessage(hwnd,WM_APP_DROPFROMANOTHER,0,0);
		::CloseHandle(hmap);
		return 0;
	}
	
	int result = 0;
	{
		//ABase�I�u�W�F�N�g�𐶐�����B
		ABase	base(true);
		CAppImp::Setup(hInstance,lpCmdLine,nCmdShow);
		
		//�A�v���P�[�V�����I�u�W�F�N�g�𐶐�����B
		//�R���X�g���N�^���ŁAgApp��this���ݒ肳��A����ȍ~�AGetApp()���L���ƂȂ�B
		AApp	*app = new AApp;
		
		//�A�v���P�[�V�������s
		result = app->NVI_Run();
		
		delete app;
		
		//if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[main exited]");
	}
	return result;
}

