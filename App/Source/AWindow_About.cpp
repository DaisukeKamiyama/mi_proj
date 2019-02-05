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

AWindow_About

*/

#include "stdafx.h"

#include "AApp.h"
#include "AWindow_About.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_About
#pragma mark ===========================
//Aboutウインドウ

//コンストラクタ
AWindow_About::AWindow_About():AWindow(/*#175 NULL*/)
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//コントロールのクリック時のコールバック
ABool	AWindow_About::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kButton_WebSite:
		{
			if( GetApp().NVI_GetLanguage() == kLanguage_Japanese )//#305
			{
				ALaunchWrapper::OpenURL("https://www.mimikaki.net/");
			}
			else
			{
				ALaunchWrapper::OpenURL("http://www.mimikaki.net/en/");
			}
			result = true;
			break;
		}
#ifdef IMPLEMENTATION_FOR_WINDOWS
		//diff.exeライセンス
	  case 11:
		{
			AFileAcc	appfolder;
			AFileWrapper::GetAppFolder(appfolder);
			AFileAcc	license;
			license.SpecifyChild(appfolder,"diff\\COPYING");
			GetApp().SPNVI_CreateDocumentFromLocalFile(license);
			break;
		}
#endif
		//#567
	  case 1002:
		{
			AFileAcc	helpFolder;
			AFileWrapper::GetHelpFolder("ja",helpFolder);
			AFileAcc	help;
			help.SpecifyChild(helpFolder,"copyright.htm");
			AText	url;
			help.GetURL(url);
			//url.AddCstring("#lua");
			ALaunchWrapper::OpenURLWithDefaultBrowser(url);
			break;
		}
	}
	return result;
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_About::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/About");
	
	AText	version;
	GetApp().NVI_GetVersion(version);
	//#1096
	AText	build;
	GetApp().NVI_GetBuildNumber(build);
	version.AddCstring(" (");
	version.AddText(build);
	version.AddCstring(")");
	AText	text;
	text.SetLocalizedText("Version");
	text.AddCstring(" ");
	text.AddText(version);
	//
	NVI_SetControlText(AWindow_About::kVersion,text);
	
	NVI_RegisterToFocusGroup(kButton_WebSite,true);//#353
	NVI_RegisterToFocusGroup(1001,true);//#353
	
	NVM_UpdateControlStatus();//#1384
}

//#1384
/**
コントロール更新
*/
void	AWindow_About::NVMDO_UpdateControlStatus()
{
	AText	useridText;
	useridText.SetLocalizedText("About_UserID");
	AText	userid;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kUserIDRegistration, userid);
	if( AWindow_UserIDRegistration::CheckUserID(userid) == true )
	{
		useridText.ReplaceParamText('0',userid);
	}
	else
	{
		AText	notRegistered;
		notRegistered.SetLocalizedText("About_UserIDNotRegistered");
		useridText.ReplaceParamText('0',notRegistered);
	}
	NVI_SetControlText(AWindow_About::kUserID,useridText);
}


