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

	CAppImp (Windows)

*/

#include "stdafx.h"

#include "CAppImp.h"
#include "CWindowImp.h"
#include "CUserPane.h"
#include "Commdlg.h"
#include "../Frame.h"
#include "gdiplus.h"
using namespace Gdiplus;

HINSTANCE	CAppImp::sHInstance;
HINSTANCE	CAppImp::sHResourceInstance = NULL;//satDLL
LPTSTR		CAppImp::sCmdLine;
int			CAppImp::sCmdShow;
ULONG_PTR	CAppImp::sGdiToken = 0;
ABool		CAppImp::sDisableNextTranslateMessage = false;
ALanguage	CAppImp::sLanguage = kLanguage_English;

#pragma mark ===========================
#pragma mark [�N���X]CAppImp
#pragma mark ===========================

#pragma mark --- �R���X�g���N�^�^�f�X�g���N�^

CAppImp::CAppImp()
{
	//
	LANGID	langid = DetectLanguage();
	if( langid == 0x0411 )
	{
		sLanguage = kLanguage_Japanese;
	}
	//#645 �������̂���CAppImp::GetVersion()����ړ�
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	AText	appPath;
	appFile.GetPath(appPath);
	AStCreateWcharStringFromAText	appPathStr(appPath);
	DWORD	dummy = 0;
	DWORD	infoSize = ::GetFileVersionInfoSizeW(appPathStr.GetPtr(),&dummy);
	LPBYTE	lpVersionInfo = (LPBYTE)::malloc(infoSize);
	if( lpVersionInfo == NULL )   {_ACThrow("",NULL);return;}
	::GetFileVersionInfo(appPathStr.GetPtr(),0,infoSize,lpVersionInfo);
	UINT uLen = 0;
	VS_FIXEDFILEINFO *lpFfi;
	::VerQueryValueW(lpVersionInfo,L"\\",(LPVOID*)&lpFfi,&uLen);
	DWORD dwFileVersionMS = lpFfi->dwFileVersionMS;
	DWORD dwFileVersionLS = lpFfi->dwFileVersionLS;
	::free(lpVersionInfo);
	mApplicationVersionText.SetFormattedCstring("%d.%d.%d.%d",
				HIWORD(dwFileVersionMS),LOWORD(dwFileVersionMS),
				HIWORD(dwFileVersionLS),LOWORD(dwFileVersionLS));
}

CAppImp::~CAppImp()
{
}

/**
�R�}���h���C��������ݒ�
*/
void	CAppImp::Setup( HINSTANCE hInstance, LPTSTR inCmdLine, int nCmdShow )
{
	::OleInitialize(NULL);//CoInitializeEx�������ŌĂ΂��
	LoadLocalizedText();
	sHInstance = hInstance;
	sHResourceInstance = sHInstance;//satDLL
	sCmdLine = inCmdLine;
	sCmdShow = nCmdShow;
	CWindowImp::InitWindowClass();
	CUserPane::RegisterUserPaneWindow();
	WSADATA wsaData;
	if( ::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0 )
	{
		_ACError("",NULL);
	}
	//GDI+
	GdiplusStartupInput gdiSI;
	::GdiplusStartup(&sGdiToken, &gdiSI, NULL);
	
}

/*#263
void	CAppImp::GetCmdLine( AText& outText )
{
	outText.SetFromWcharString(sCmdLine);
}
*/

/**
�R�}���h���C��������TextArray���擾
*/
void	CAppImp::GetCmdLineArgTextArray( ATextArray& outTextArray )
{
	AText	cmdline;
	cmdline.SetFromWcharString(sCmdLine,4096);//�R�}���h���C������
	GetCmdLineArgTextArray(cmdline,outTextArray);
}
void	CAppImp::GetCmdLineArgTextArray( const AText& cmdline ,ATextArray& outTextArray )//#263
{
	for( AIndex pos = 0; pos < cmdline.GetItemCount(); pos++ )
	{
		//�X�y�[�X���΂�
		if( cmdline.SkipTabSpace(pos,cmdline.GetItemCount()) == false )   break;
		//Text�擾
		AText	text;
		AUChar	ch = cmdline.Get(pos);
		//"�͂��܂�Ȃ�A����ȍ~����"�̂܂��܂�
		if( ch == '\"' )
		{
			pos++;
			for( ; pos < cmdline.GetItemCount(); pos++ )
			{
				ch = cmdline.Get(pos);
				if( ch == '\"' )
				{
					pos++;
					break;
				}
				text.Add(ch);
			}
		}
		//"�͂��܂�łȂ��Ȃ�A�X�y�[�X�̂܂��܂�
		else
		{
			for( ; pos < cmdline.GetItemCount(); pos++ )
			{
				ch = cmdline.Get(pos);
				if( ch == ' ' )
				{
					//pos++;
					break;
				}
				text.Add(ch);
			}
		}
		outTextArray.Add(text);
	}
}

#pragma mark ===========================

#pragma mark --- Run/Quit

HWND	CAppImp::sMessageOnlyWindow;

int	CAppImp::Run()
{
	//���v���Z�X����̃��b�Z�[�W��M�p�E�C���h�E����
	//�����O��AApp����擾�ł���悤�ɂ��ׂ�
	//��10�����̓�������Ԃ̕����̂��߂Ɏg�p����Ă����炵���H
	sMessageOnlyWindow = ::CreateWindowW(L"mieditdaisukekamiyamamessageonly57658352",L"mieditdaisukekamiyamamessageonly57658352",
			WS_OVERLAPPEDWINDOW,0,0,0,0,NULL,NULL,CAppImp::GetHInstance(),NULL);
	
	//timer
	UINT_PTR	timer = ::SetTimer(NULL,0,kTickMsec,STATIC_APICB_DoTickTimerAction);
	
	//
	HACCEL	hAccelTable = ::LoadAccelerators(CAppImp::GetHResourceInstance(),MAKEINTRESOURCE(IDC_APPACCEL));//satDLL
	MSG	msg = {0};
	while( ::GetMessage(&msg,NULL,0,0) )//�L���[����擾
	{
		if( ::TranslateAccelerator(msg.hwnd,hAccelTable,&msg) == 0 )
		{
			//
			sDisableNextTranslateMessage = false;
			//
			::DispatchMessageW(&msg);
			//
			if( sDisableNextTranslateMessage == false )
			{
				::TranslateMessage(&msg);//�ϊ��チ�b�Z�[�W(WM_CHAR)���L���[�ɂ�
			}
		}
	}
	
	::KillTimer(NULL,timer);
	
	return (int)msg.wParam;
}

void	CAppImp::Quit()
{
	::PostQuitMessage(0);
	
	//GDI+
	::GdiplusShutdown(sGdiToken);
}

//TickTimer
VOID CALLBACK	CAppImp::STATIC_APICB_DoTickTimerAction( HWND inHWnd, UINT inMessage, UINT inTimerID, DWORD inTime )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(A-TTA)");
	//�i�e�R�[���o�b�N���ʏ��������j
	//�A�v����퓮�쒆�łȂ���Ή����������^�[��
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return;
	//�C�x���g�g�����U�N�V�����O�����^�㏈��
	AStEventTransactionPrePostProcess	oneevent(true);
	try
	{
		AApplication::GetApplication().EVT_DoTickTimerAction();
	}
	catch(...)
	{
		_ACError("CAppImp::STATIC_APICB_DoTickTimerAction() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[A-TTA]");
}

void	CAppImp::ShowFileOpenWindow( const ABool inShowInvisibleFile )
{
	//���b�� ���ؕK�v mHWnd�w�肵�Ȃ���OK?
	wchar_t	path[kPathMaxLength], title[kPathMaxLength];
	path[0] = 0;
	title[0] = 0;
	AText	filter("*.*");
	AStCreateWcharStringFromAText	textptr(filter);
	OPENFILENAMEW	ofn = {0};
	ofn.lStructSize				= sizeof(OPENFILENAMEW);
	ofn.hwndOwner				= NULL;//��
	ofn.hInstance				= NULL;
	ofn.lpstrFilter				= textptr.GetPtr();
	ofn.lpstrCustomFilter		= NULL;
	ofn.nMaxCustFilter			= 0;
	ofn.nFilterIndex			= 0;
	ofn.lpstrFile				= path;
	ofn.nMaxFile				= kPathMaxLength;
	ofn.lpstrFileTitle			= title;
	ofn.nMaxFileTitle			= kPathMaxLength;
	ofn.lpstrInitialDir			= NULL;
	ofn.lpstrTitle				= NULL;
	ofn.Flags					= OFN_CREATEPROMPT;
	ofn.nFileOffset				= 0;
	ofn.nFileExtension			= 0;
	ofn.lpstrDefExt				= NULL;
	ofn.lCustData				= 0L;
	ofn.lpfnHook				= NULL;
	ofn.lpTemplateName			= NULL;
	ofn.FlagsEx = 0;
	if( GetOpenFileNameW(&ofn) == TRUE )
	{
		AText	text;
		text.SetFromWcharString(path,sizeof(path)/sizeof(path[0]));
		AFileAcc	file;
		file.Specify(text);
		AApplication::GetApplication().NVI_OpenFile(file,NULL);//��
	}
}

void	CAppImp::GetVersion( AText& outText ) const
{
	/*#645 ������
	AFileAcc	appFile;
	AFileWrapper::GetAppFile(appFile);
	AText	appPath;
	appFile.GetPath(appPath);
	AStCreateWcharStringFromAText	appPathStr(appPath);
	DWORD	dummy = 0;
	DWORD	infoSize = ::GetFileVersionInfoSizeW(appPathStr.GetPtr(),&dummy);
	LPBYTE	lpVersionInfo = (LPBYTE)::malloc(infoSize);
	if( lpVersionInfo == NULL )   {_ACThrow("",NULL);return;}
	::GetFileVersionInfo(appPathStr.GetPtr(),0,infoSize,lpVersionInfo);
	UINT uLen = 0;
	VS_FIXEDFILEINFO *lpFfi;
	::VerQueryValueW(lpVersionInfo,L"\\",(LPVOID*)&lpFfi,&uLen);
	DWORD dwFileVersionMS = lpFfi->dwFileVersionMS;
	DWORD dwFileVersionLS = lpFfi->dwFileVersionLS;
	::free(lpVersionInfo);
	outText.SetFormattedCstring("%d.%d.%d.%d",
			HIWORD(dwFileVersionMS),LOWORD(dwFileVersionMS),
			HIWORD(dwFileVersionLS),LOWORD(dwFileVersionLS));
	*/
	outText.SetText(mApplicationVersionText);
}

#pragma mark ===========================

#pragma mark --- LocalizedText

AHashTextArray	CAppImp::sLocalizedTextArray_Key;
ATextArray	CAppImp::sLocalizedTextArray_Text;

void	CAppImp::GetLocalizedText( const AText& inKey, AText& outText ) 
{
	outText.DeleteAll();
	AIndex	index = sLocalizedTextArray_Key.Find(inKey);
	if( index == kIndex_Invalid )
	{
		_ACError("",NULL);
	}
	else
	{
		outText.SetText(sLocalizedTextArray_Text.GetTextConst(index));
	}
}

ABool	CAppImp::ExistLocalizedText( const AText& inKey )
{
	AIndex	index = sLocalizedTextArray_Key.Find(inKey);
	return (index!=kIndex_Invalid);
}

void	CAppImp::LoadLocalizedText()
{
	sLocalizedTextArray_Key.DeleteAll();
	sLocalizedTextArray_Text.DeleteAll();
	AFileAcc	appfolder;
	AFileWrapper::GetAppFolder(appfolder);
	AText	langText;
	GetLangText(langText);
	AFileAcc	localizedFolder;
	localizedFolder.SpecifyChild(appfolder,langText);
	AFileAcc	localfile;
	localfile.SpecifyChild(localizedFolder,"Strings.txt");
	AText	text;
	localfile.ReadTo(text);
	text.ConvertToUTF8FromUTF16();
	//parse
	for( AIndex pos = 0; pos < text.GetItemCount(); )
	{
		AText	line;
		text.GetLine(pos,line);
		if( line.GetItemCount() == 0 )   continue;
		if( line.Get(0) == '/' )   continue;
		AText	key;
		AIndex	p = 0;
		line.GetToken(p,key);
		AText	token;
		line.GetToken(p,token);//=
		line.GetToken(p,token);//"
		AText	localizedtext;
		line.ExtractUnescapedTextFromPrefText(p,localizedtext,'\"');
		sLocalizedTextArray_Key.Add(key);
		sLocalizedTextArray_Text.Add(localizedtext);
	}
}

void	CAppImp::GetLangText( AText& outText )
{
	switch(sLanguage)
	{
	  case kLanguage_Japanese:
		{
			outText.SetCstring("jp");
			break;
		}
	  default:
		{
			outText.SetCstring("en");
			break;
		}
	}
}

#pragma mark ===========================

#pragma mark --- �V�X�e�����\�[�X

/**
GDI�I�u�W�F�N�g�`�F�b�N
*/
ABool	CAppImp::SystemResourceCheck_GDIObject()
{
	DWORD	currentProcessID = ::GetCurrentProcessId();
	HANDLE	hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,currentProcessID);
	if( hProcess == NULL )   {_ACError("",NULL);return false;}
	DWORD	gdiObjects = ::GetGuiResources(hProcess,GR_GDIOBJECTS);
	::CloseHandle(hProcess);
	if( gdiObjects > 9000 )
	{
		AText	mes1, mes2;
		mes1.SetLocalizedText("SystemResource_GDIObject1");
		mes2.SetLocalizedText("SystemResource_GDIObject2");
		AApplication::GetApplication().NVI_ShowModalAlertWindow(mes1,mes2);
		return false;
	}
	return true;
}

#pragma mark ===========================

#pragma mark --- ����

/**
����T�e���C�gDLL�ǂݍ���
*/
void	CAppImp::SetupLanguageSateliteDLL( const ALanguage inLanguage )
{
	sLanguage = inLanguage;
	if( sLanguage != kLanguage_English )
	{
		AText	satDLLPath;
		satDLLPath.SetCstring("sat_");
		switch(sLanguage)
		{
		  case kLanguage_Japanese:
		  default:
			{
				satDLLPath.AddCstring("jp");
				break;
			}
		}
		satDLLPath.AddCstring(".dll");
		AFileAcc	appFile;
		AFileWrapper::GetAppFile(appFile);
		AFileAcc	satDLLFile;
		satDLLFile.SpecifySister(appFile,satDLLPath);
		AText	satDLLFullPath;
		satDLLFile.GetPath(satDLLFullPath);
		AStCreateWcharStringFromAText	str(satDLLFullPath);
		sHResourceInstance = ::LoadLibraryW(str.GetPtr());
		DWORD	err = GetLastError();
		if( sHResourceInstance == NULL )
		{
			_ACError("",NULL);
			sHResourceInstance = sHInstance;
		}
		if( sHResourceInstance != sHInstance )
		{
			CWindowImp::InitWindowClass();
		}
		LoadLocalizedText();
	}
}

//�ȉ�MS��satelite DLL�T���v���R�[�h����R�s�[

// �^��`
typedef struct LANGINFO_DEF {
	int		Count;
	LANGID	LangID;
} LANGINFO;
typedef LANGINFO *PLANGINFO;

// ���̊֐��ɂ́A�������[�U�[ �C���^�[�t�F�C�X
// ���\������錾������o����R�[�h��
// �܂܂�Ă��܂�

BOOL CALLBACK CAppImp::EnumLangProc(HANDLE, LPCTSTR, LPCTSTR,
						   WORD wIDLanguage, LONG_PTR lParam)
{
    PLANGINFO LangInfo;

    LangInfo = (PLANGINFO) lParam;
    LangInfo->Count++;
    LangInfo->LangID  = wIDLanguage;

    return (TRUE);        // �񋓂��p�����܂�
}

// Hongkong SAR �o�[�W�����̓���̏������s�� ntdll.dll 
// �̌�������o���܂�
LANGID CAppImp::GetNTDLLNativeLangID()
{

    LANGINFO LangInfo;
	LPCTSTR Type = (LPCTSTR) ((LPVOID)((WORD)16));
    LPCTSTR Name = (LPCTSTR) 1;

    ZeroMemory(&LangInfo,sizeof(LangInfo));
    
    // ntdll �� HModule ���擾���܂�
    HMODULE hMod = GetModuleHandle(_T("ntdll.dll"));
    if (hMod==NULL) {
        return(0);
    }

    BOOL result = EnumResourceLanguages(hMod, Type, Name, (ENUMRESLANGPROC)EnumLangProc, (LONG_PTR) &LangInfo);
    
    if (!result || (LangInfo.Count > 2) || (LangInfo.Count < 1) ) {
        return (0);
    }
    
    return (LangInfo.LangID);
}

// NT4 �V�X�e���� Hongkong SAR �o�[�W�����ł��邩�ǂ������`�F�b�N���܂�
BOOL CAppImp::IsHongKongVersion()
{
    HMODULE hMod;
    BOOL bRet=FALSE;
	typedef BOOL (WINAPI *IMMRELEASECONTEXT)(HWND,HIMC);
    IMMRELEASECONTEXT pImmReleaseContext;

    hMod = LoadLibrary(_T("imm32.dll"));
    if (hMod) {
        pImmReleaseContext = (IMMRELEASECONTEXT)GetProcAddress(hMod,"ImmReleaseContext");
        if (pImmReleaseContext) {
            bRet = pImmReleaseContext(NULL,NULL);
        }
        FreeLibrary(hMod);
    }
    return (bRet);
}

// ���̊֐��́A���ׂẴv���b�g�t�H�[�� (Win9x�AMe�ANT4�AWindows 2000�AWindows XP) 
// �ɓK�؂ȏ��� UI ��������o���܂�
LANGID CAppImp::DetectLanguage() {

#define MAX_KEY_BUFFER	80

	OSVERSIONINFO		VersionInfo;
	LANGID				uiLangID = 0;
	HKEY				hKey;
	DWORD				Type, BuffLen = MAX_KEY_BUFFER;
	TCHAR				LangKeyValue[MAX_KEY_BUFFER];


	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if( !GetVersionEx(&VersionInfo) )
		return(0);

	switch( VersionInfo.dwPlatformId ) {
		// Windows NT�AWindows 2000�A�܂��͂���ȏ�̏ꍇ
		case VER_PLATFORM_WIN32_NT:
			if( VersionInfo.dwMajorVersion >= 5)   // Windows 2000 �܂��͂���ȏ�
				uiLangID = GetUserDefaultUILanguage();
			else {   // ntdll.dll �̌���� NT4 �`�F�b�N�p
				uiLangID = GetNTDLLNativeLangID();   
				if (uiLangID == 1033) {		// NT 4 �� Honkong SAR �o�[�W�����̓��ʂȏ���
					if (IsHongKongVersion()) {
						uiLangID = 3076;
					}
				}
			}
			break;
		// Windows 98 �܂��� Windows Me �̏ꍇ
		case VER_PLATFORM_WIN32_WINDOWS:
			// UI ����̃��W�X�g�� �L�[���J���܂�
			if( RegOpenKeyEx(HKEY_CURRENT_USER,_T("Default\\Control Panel\\Desktop\\ResourceLocale"), 0,
				KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS ) {
				// ����̃L�[�̌^���擾���܂�
				if( RegQueryValueEx(hKey, NULL, NULL, &Type, NULL, NULL) == ERROR_SUCCESS 
					&& Type == REG_SZ ) {
					// �L�[�̒l��ǂݎ��܂�
					if( RegQueryValueEx(hKey, NULL, NULL, &Type, (LPBYTE)LangKeyValue, &BuffLen) 
						== ERROR_SUCCESS ) {
						uiLangID = (LANGID)_ttoi(LangKeyValue);
					}
				}
				RegCloseKey(hKey);
			}				
			break;
	}

    if (uiLangID == 0) {
        uiLangID = GetUserDefaultLangID();
    }
    // ������������ ID ��Ԃ��܂��B
    return (uiLangID);
}
