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

AView_Text

*/

#include "stdafx.h"

#include "AView_Text.h"
#include "ADocument_Text.h"
#include "AApp.h"

//AView_Textメソッドのうち、ツール関係のもの

//#50
AHashTextArray				AView_Text::sToolData_VarNameArray;
ATextArray					AView_Text::sToolData_VarValueArray;


#pragma mark ===========================

#pragma mark ---ツール

void	AView_Text::SPI_DoTool( const AFileAcc& inFile, const AModifierKeys inModifierKeys, const ABool inDontShowDropWarning,
		const ANumber inDepth )//B0405
{
	//inFileがフォルダの場合は開かない(#947)
	if( inFile.IsFolder() == true && inFile.IsBundleFolder() == false )//#1155
	{
		return;
	}
	//
	if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || AKeyWrapper::IsOptionKeyPressed() == true
#if IMPLEMENTATION_FOR_WINDOWS
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true
#endif
				)
	{
		//==================ツールをドキュメントとして開いて編集==================
		//B0326GetApp().SPNVI_CreateDocumentFromLocalFile(inFile);
		GetApp().GetAppPref().LaunchAppWithFile(inFile,inModifierKeys);
		//#265
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,inFile);
		if( docID != kObjectID_Invalid )
		{
			//開いたドキュメントのモード取得
			AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex();
			if( modeIndex == kStandardModeIndex )
			{
				//モードが標準モードならツールコマンドモードへ変更
				AText	toolcommandModeName("TOOLCOMMAND");
				modeIndex = GetApp().SPI_FindModeIndexByModeRawName(toolcommandModeName);
				if( modeIndex != kIndex_Invalid )
				{
					GetApp().SPI_GetTextDocumentByID(docID).SPI_SetMode(modeIndex);
				}
			}
			//#427 自動更新ツールは書き込み不可にする
			AText	path;
			inFile.GetPath(path);
			AIndex	pos = 0;
			if( path.FindCstring(0,"/autoupdate/",pos) == true )
			{
				GetApp().SPI_GetTextDocumentByID(docID).NVI_SetReadOnly(true);
			}
			GetApp().NVI_UpdateTitleBar();
		}
		return;
	}
	
	//==================ツール実行==================
	//ファイル名の"drop"文字列検索
	AText	filename;
	inFile.GetFilename(filename);
	AIndex	tmp;
	ABool	isDrop = filename.FindCstring(0,"drop",tmp);
	
	//エイリアス解決
	AFileAcc	file;
	file.CopyFrom(inFile);
	file.ResolveAlias();
	
	//
	AText	docfilename;
	file.GetFilename(docfilename);
	AText	suffix;
	docfilename.GetSuffix(suffix);
	
	//#904 JavaScriptファイル実行
	if( suffix.Compare(".js") == true )
	{
		//Undoアクションタグ記録
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
		//JavaScript実行
		AText	script;
		AIndex	modeIndex = kIndex_Invalid;
		AText	tecname;
		GetApp().SPI_LoadTextFromFileOrDocument(kLoadTextPurposeType_Tool,file,script,modeIndex,tecname);
		AFileAcc	folder;
		folder.SpecifyParent(file);
		GetApp().SPI_ExecuteJavaScript(script,folder);
		return;
	}
	/*#1282
	//#567 Luaファイル実行
	if( suffix.Compare(".lua") == true )
	{
		AText	errorText;
		if( GetApp().NVI_Lua_dofile(file,errorText) != 0 )
		{
			ADocumentID	docID = GetApp().SPI_CreateOrGetLuaConsoleDocumentID();
			GetApp().SPI_GetTextDocumentByID(docID).SPI_Lua_Output(errorText);
			GetApp().SPI_GetTextDocumentByID(docID).NVI_RevealDocumentWithAView();
		}
		ADocumentID	docID = GetApp().SPI_GetLuaConsoleDocumentID();
		if( docID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(docID).SPI_Lua_Prompt();
		}
		return;
	}
     */
	//
	AFileAttribute	attr;
	file.GetFileAttribute(attr);
#if IMPLEMENTATION_FOR_MACOSX
	if( /*B0000 attr.type == 'osas'*/file.IsAppleScriptFile() == true )
	{
		DoTool_AppleScript(file);
		return;
	}
	
	//
	if( attr.type == 'APPL' || suffix.Compare(".app") == true )
	{
		AAppAcc	app(file);
		AFileAcc	docfile;
		GetTextDocumentConst().NVI_GetFile(docfile);
		if( isDrop == true )
		{
			ALaunchWrapper::Drop(app,docfile);
		}
		else
		{
			ALaunchWrapper::Launch(app);
		}
		return;
	}
#endif
	
	AText	text;
	GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_Tool,file,text);
	
	//シェルスクリプト
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseShellScript) == true && text.GetItemCount() > 2 )
	{
		if( text.Get(0) == '#' && text.Get(1) == '!' )
		{
			DoTool_ShellScript(file,text,false);//#862
			return;
		}
	}
	/*#862 test
	if( suffix.Compare(".c") == true )
	{
		AFileAcc	tmpFolder;
		GetApp().SPI_GetUniqTempFolder(tmpFolder);
		//
		AFileAcc	tmpCopiedFile;
		tmpCopiedFile.SpecifyChild(tmpFolder,"tmp.c");
		//
		file.CopyFileTo(tmpCopiedFile,true);
		//
		AText	path;
		tmpCopiedFile.GetPath(path);
		//
		AFileAcc	execFile;
		execFile.SpecifyChild(tmpFolder,"test");
		AText	execFilePath;
		execFile.GetPath(execFilePath);
		//
		AText	command;
		command.SetCstring("gcc -o \"");
		command.AddText(execFilePath);
		command.AddCstring("\" \"");
		command.AddText(path);
		command.AddCstring("\"");
		//
		AStCreateCstringFromAText	cstr(command);
		system(cstr.GetPtr());
		//
		DoTool_ShellScript(execFile,text,true);
		return;
	}
	*/
	
	//通常ツール
	SPI_DoTool_Text(text,inDontShowDropWarning,inDepth);//B0405
}

#if IMPLEMENTATION_FOR_MACOSX
void	AView_Text::DoTool_AppleScript( const AFileAcc& inFile )
{
	GetApp().SPI_ExecuteAppleScript(inFile);
	//CCompiledAppleScript	appleScript(inFile);
	//appleScript.ReadAndExecute();
}
#endif

void	AView_Text::DoTool_ShellScript( const AFileAcc& inFile, const AText& inText, const ABool inBinaryExecute )//#862
{
	//パラメータ
	ANumber	timer = 5;
	ABool	newMode = true;
	ABool	replaceAllMode = false;
	ABool	replaceSelectedMode = false;
	ABool	windowMode = false;
	AText	windowTitle;
	ABool	sendSelectedMaccode = false;
	ATextEncoding	resultEncoding = ATextEncodingWrapper::GetUTF8TextEncoding();//R0157
	ABool	resultEncodingDirected = false;//R0157
	//ツール内容テキスト解析
	AIndex	pos = 0;
	AText	lineText;
	//最初の行（実行プロセスパス）を取得
	inText.GetLine(pos,lineText);
	AText	command;
	if( inBinaryExecute == false )//#862
	{
		command.SetText(lineText);
		command.Delete(0,2);
	}
	//#862
	else
	{
		AText	filepath;
		inFile.GetPath(command);
	}
	//
	ATextArray	argArray;
	argArray.Add(command);
	AText	path;
	inFile.GetPath(path,kFilePathType_Default);
	argArray.Add(path);
	//
	AFileAcc	docfile;
	GetTextDocumentConst().NVI_GetFile(docfile);
	AText	docpath;
	docfile.GetPath(docpath,kFilePathType_Default);
	argArray.Add(docpath);
	//２行目取得
	inText.GetLine(pos,lineText);
	if( lineText.GetItemCount() > 0 )
	{
		if( lineText.Get(0) == '#' )
		{
			AIndex	pos2 = 1;
			while( pos2 < lineText.GetItemCount() )
			{
				AText	token;
				lineText.GetToken(pos2,token);
				if( token.Compare("replace_all") == true )
				{
					replaceAllMode = true;
					replaceSelectedMode = false;
					newMode = false;
					windowMode = false;
				}
				else if( token.Compare("replace_selected") == true )
				{
					replaceSelectedMode = true;
					replaceAllMode = false;
					newMode = false;
					windowMode = false;
				}
				else if( token.Compare("none") == true )
				{
					replaceSelectedMode = false;
					replaceAllMode = false;
					newMode = false;
					windowMode = false;
				}
				else if( token.Compare("new") == true )
				{
					newMode = true;
					replaceSelectedMode = false;
					replaceAllMode = false;
					windowMode = false;
				}
				else if( token.Compare("window") == true )
				{
					lineText.GetTextToTabSpaceLineEnd(pos2,windowTitle);
					if( windowTitle.GetItemCount() > 0 )
					{
						newMode = false;
						replaceSelectedMode = false;
						replaceAllMode = false;
						windowMode = true;
					}
				}
				else if( token.Compare("timeout") == true )
				{
					lineText.GetToken(pos2,token);
					if( token.Compare("=") == true )
					{
						lineText.GetToken(pos2,token);
						timer = token.GetNumber();
						if( timer <= 0 )   timer = 1;
					}
				}
				else if( token.Compare("send_selected_maccode") == true )//B0088
				{
					sendSelectedMaccode = true;
				}
				else if( token.Compare("result_encoding") == true )//R0157 result文字列の自動認識では「表」「ウサギ」などが化ける
				{
					lineText.GetToken(pos2,token);
					if( token.Compare("=") == true )
					{
						lineText.GetToken(pos2,token);
						ATextEncodingWrapper::GetTextEncodingFromName(token,resultEncoding);
						resultEncodingDirected = true;
					}
				}
			}
		}
	}
	//CurrentDirectory取得（ドキュメントのフォルダ）
	AText	dirpath;
	AFileAcc	docfolder;
	docfolder.SpecifyParent(docfile);
	docfolder.GetPath(dirpath,kFilePathType_Default);
	//選択文字列
	AText	selectedText;
	GetSelectedText(selectedText);
	if( sendSelectedMaccode == true )
	{
		AText	text;
		if( selectedText.ConvertFromUTF8CR(ATextEncodingWrapper::GetSJISTextEncoding(),//#688 GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),
						returnCode_CR,text) == true )
		{
			selectedText.SetText(text);
		}
	}
	argArray.Add(selectedText);
	//結果表示ドキュメント
	ADocumentID	resultDocumentID = mTextDocumentRef.GetObjectID();//#695
	ATextIndex	insertionIndex = kIndex_Invalid;
	ABool	dirty = true;//B0357
	if( newMode == true )
	{
		AText	text;
		GetApp().GetAppPref().GetData_Text(AAppPrefDB::kStdOutModeName,text);
		AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(text);
		if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
		resultDocumentID = GetApp().SPNVI_CreateNewTextDocument(modeIndex);
		if( resultDocumentID == kObjectID_Invalid )   return;//win
		AText	title;
		GetTextDocumentConst().NVI_GetTitle(title);
		title.AddCstring(":stdout:");
		AText	datetime;
		ADateTimeWrapper::GetDateTimeText(datetime);
		title.AddText(datetime);
		GetApp().SPI_GetTextDocumentByID(resultDocumentID).SPI_SetTemporaryTitle(title);
		insertionIndex = 0;
		//Undoアクションタグ記録
		GetApp().SPI_GetTextDocumentByID(resultDocumentID).SPI_RecordUndoActionTag(undoTag_Tool);
		//B0357
		dirty = false;
	}
	else if( windowMode == true )
	{
		resultDocumentID = GetApp().NVI_GetDocumentIDByTitle(kDocumentType_Text,windowTitle);
		if( resultDocumentID == kObjectID_Invalid )
		{
			AText	text;
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kStdOutModeName,text);
			AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(text);
			if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
			resultDocumentID = GetApp().SPNVI_CreateNewTextDocument(modeIndex);
			if( resultDocumentID == kObjectID_Invalid )   return;//win
			GetApp().SPI_GetTextDocumentByID(resultDocumentID).SPI_SetTemporaryTitle(windowTitle);
			insertionIndex = 0;
		}
		else
		{
			insertionIndex = GetApp().SPI_GetTextDocumentByID(resultDocumentID).SPI_GetTextLength();//B0314
		}
		//Undoアクションタグ記録
		GetApp().SPI_GetTextDocumentByID(resultDocumentID).SPI_RecordUndoActionTag(undoTag_Tool);
	}
	else if( replaceSelectedMode == true )
	{
		if( GetTextDocument().NVI_IsReadOnly() == false )
		{
			//Undoアクションタグ記録
			GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
			//選択範囲削除
			ATextIndex	spos,epos;
			GetSelect(spos,epos);
			DeleteTextFromDocument(spos,epos);
			//
			insertionIndex = spos;
		}
	}
	else if( replaceAllMode == true )
	{
		if( GetTextDocument().NVI_IsReadOnly() == false )
		{
			//Undoアクションタグ記録
			GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
			//全削除
			ATextIndex	spos,epos;
			spos = 0;
			epos = GetTextDocumentConst().SPI_GetTextLength();
			DeleteTextFromDocument(spos,epos);
			//
			insertionIndex = 0;
		}
	}
	//
	GetApp().SPI_GetChildProcessManager().ExecuteShellScript(command,argArray,dirpath,
			resultDocumentID,insertionIndex,timer,resultEncodingDirected,resultEncoding,dirty);//B0357
}

void	AView_Text::SPI_DoTool_Text( const AText& inText, const ABool inDontShowDropWarning, const ANumber inDepth )//B0405
{
	mToolData_CloseFlag = false;
	mToolData_CaretReserved = false;
	GetSelectedText(mToolData_SelectedText);
	mToolData_CaretMode = mCaretMode;
	mToolData_CaretTextIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
	if( mCaretMode == true )
	{
		mToolData_SelectTextIndex = mToolData_CaretTextIndex;
	}
	else
	{
		mToolData_SelectTextIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetSelectTextPoint());
	}
	mToolData_UndoTagInserted = false;
	mToolData_DropWarningFlag = false;
	mToolData_DontShowDropWarning = inDontShowDropWarning;
	mToolData_IndentFlag = false;
	mToolData_IndentFlag_InputSpaces = false;//#249
	mToolData_IndentFlag_OnlyNewLine = false;//#249
	mToolData_ToolIndentMode = true;//R0000 #844 常に挿入ツールに対しインデントする
	InsertToolText(inText,inDepth);//B0405
	//<<<CARET>>>コマンドにより保存されたキャレット位置がある場合は、そこへ移動
	if( mToolData_CaretReserved == true )
	{
		ATextPoint	caretPoint, selectPoint;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(mToolData_CaretTextIndex,caretPoint);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(mToolData_SelectTextIndex,selectPoint);
		if( mToolData_CaretMode == true )
		{
			SetCaretTextPoint(caretPoint);
		}
		else
		{
			SetSelect(caretPoint,selectPoint);
		}
	}
	if( mTextWindowID != kObjectID_Invalid )
	{
		if( mToolData_CloseFlag == true )
		{
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_CloseTab(GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_GetCurrentTabIndex());
		}
	}
	if( mToolData_DropWarningFlag == true && mToolData_DontShowDropWarning == false )
	{
		AText	text;
		inText.GetText(mToolData_DropWarningNextStartPos,inText.GetItemCount()-mToolData_DropWarningNextStartPos,text);
		text.InsertCstring(0,"<<<");
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowDropWarningWindow(text);
	}
}

void	AView_Text::InsertToolText( const AText& inText, const ANumber inDepth )
{
	for( AIndex pos = 0; pos < inText.GetItemCount();  )
	{
		//"<<<"を検索
		AIndex	epos = inText.GetItemCount();
		ABool found = inText.FindCstring(pos,"<<<",epos);
		//"<<<"までをドキュメントにテキスト挿入
		if( epos-pos > 0 )
		{
			if( mToolData_UndoTagInserted == false )
			{
				GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
				mToolData_UndoTagInserted = true;
			}
			AText	text;
			inText.GetText(pos,epos-pos,text);
			ATextPoint	spt, ept;
			SPI_GetSelect(spt,ept);
			DeleteAndInsertTextToDocument(text);
			//R0000
			if( mToolData_ToolIndentMode == true )
			{
				//
				ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
				ept = GetCaretTextPoint();
				//<<<CARET>>>コマンドにより保存されたキャレット位置がある場合は、そこへキャレット移動
				//（インデント実行後に、インデントにより変化したキャレット位置を、再取得する。
				//つまり、ツール実行時は、キャレットは文字列挿入するたびに移動するので、CARETコマンドは最後に実行する必要があるが、
				//インデントにより、正しいキャレット位置は変化する。DoIndent()は、キャレット位置を適正な位置に補正するので、
				//それを利用して、CARETによるキャレット位置を補正する。）
				if( mToolData_CaretReserved == true )
				{
					//CARETコマンドによるキャレット位置指定があれば、その位置にキャレット設定。
					ATextPoint	caretPoint, selectPoint;
					GetTextDocumentConst().SPI_GetTextPointFromTextIndex(mToolData_CaretTextIndex,caretPoint);
					GetTextDocumentConst().SPI_GetTextPointFromTextIndex(mToolData_SelectTextIndex,selectPoint);
					if( mToolData_CaretMode == true )
					{
						SetCaretTextPoint(caretPoint);
					}
					else
					{
						SetSelect(caretPoint,selectPoint);
					}
				}
				DoIndent(/*#650 false,*/undoTag_NOP,spt,ept,inputSpaces,kIndentTriggerType_IndentFeature);//#249 #639 #650
				spt = ept;
				DoIndent(/*#650 false,*/undoTag_NOP,spt,ept,inputSpaces,kIndentTriggerType_IndentFeature);//#249 #639 #650
				//CARETコマンドによるキャレット位置指定があれば、DoIndent()により補正されたキャレット位置を取得
				if( mToolData_CaretReserved == true )
				{
					mToolData_CaretTextIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
				}
				//キャレットを元のキャレット位置（DoIndent()による補正もあり）に移動
				SetCaretTextPoint(ept);
			}
		}
		if( found == false )
		{
			//
			break;
		}
		else
		{
			//コマンドモードに移行
			mToolData_IndentFlag = false;
			mToolData_IndentFlag_InputSpaces = false;//#249
			mToolData_IndentFlag_OnlyNewLine = false;//#249
			pos = epos+3;
			for( ; pos < inText.GetItemCount();  )
			{
				//Tab, Spaceスキップ
				if( inText.SkipTabSpace(pos,inText.GetItemCount()) == false )   break;
				//コマンドモード終了判定
				AUChar	ch = inText.Get(pos);
				if( ch == '>' && pos+2 < inText.GetItemCount() )
				{
					if( inText.Get(pos+1) == '>' && inText.Get(pos+2) == '>' )
					{
						pos += 3;
						break;
					}
				}
				//R0033 コメント判定
				if( ch == '/' && pos+1 < inText.GetItemCount() )
				{
					if( inText.Get(pos+1) == '*' )
					{
						AIndex	foundpos = pos;
						if( inText.FindCstring(pos,"*/",foundpos) == true )
						{
							pos = foundpos+2;
						}
						else
						{
							pos = inText.GetItemCount();
						}
						continue;
					}
					else if( inText.Get(pos+1) == '/' )
					{
						for( ; pos < inText.GetItemCount(); pos++ )
						{
							if( inText.Get(pos) == kUChar_LineEnd )
							{
								pos++;
								break;
							}
						}
						continue;
					}
				}
				//トークン読み込み
				AIndex	svPos = pos;
				AText	token;
				inText.GetToken(pos,token);
				//ツールコマンド検索
				AToolCommandID	toolCommandID = GetApp().SPI_FindToolCommandID(token);
				if( toolCommandID != tc_NOP )
				{
					//Undoタグ
					if( toolCommandID != tc_REDO && toolCommandID != tc_UNDO && mToolData_UndoTagInserted == false )
					{
						GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
						mToolData_UndoTagInserted = true;
					}
					//ツールコマンド実行（ツールコマンド１つ分）
					AText	text;
					AText	hintText;
					if( InsertToolText_Command(inText,pos,inDepth,toolCommandID,text,hintText) == true )   continue;
					
					//dropwarning判定
					if( mToolData_DropWarningFlag == true && mToolData_DontShowDropWarning == false )
					{
						mToolData_DropWarningNextStartPos = svPos;
						return;
					}
					//結果文字列が存在するなら
					if( text.GetItemCount() > 0 )
					{
						//Undoタグ
						if( mToolData_UndoTagInserted == false )
						{
							GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
							mToolData_UndoTagInserted = true;
						}
						//ツールコマンド結果挿入
						DeleteAndInsertTextToDocument(text);
						//ヒントテキストを登録
						AIndex	hintTextIndex = kIndex_Invalid;
						if( hintText.GetItemCount() > 0 )
						{
							ATextIndex	insertIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint)
										- text.GetItemCount();
							hintTextIndex = GetTextDocument().SPI_AddHintText(insertIndex,hintText);
						}
						//インデント実行
						if( mToolData_IndentFlag == true )
						{
							ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
							if( mToolData_IndentFlag_InputSpaces == true )   inputSpaces = true;//#249
							if( mToolData_IndentFlag_OnlyNewLine == false )//#249
							{
								//B0381 改行挿入行のインデント実行
								/* R0000 SDFでも改行挿入行のインデント実行して問題ないはず＆インデント実行すべき。ツールコマンドで</p><<<INDENT-RETURNの場合など。
								ABool	useSyntaxDef = 
								(GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseSyntaxDefinitionIndent) == true &&
								GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseBuiltinSyntaxDefinitionFile) == true);*/
								if( (GetTextDocumentConst().SPI_GetIndentType() == kIndentType_SDFIndent || 
												GetTextDocumentConst().SPI_GetIndentType() == kIndentType_RegExpIndent ) && 
											GetApp().SPI_GetAutoIndentMode() == true )
								{
									if( GetCaretTextPoint().y > 0 ) 
									{
										ATextPoint	spt, ept;
										spt = GetCaretTextPoint();
										spt.y--;
										ept = spt;
										DoIndent(/*#650 false,*/undoTag_NOP,spt,ept,inputSpaces,
												 kIndentTriggerType_ReturnKey_CurrentLine);//#249 #639 #650
									}
								}
							}
							
							DoIndent(/*#650 false,*/undoTag_NOP,inputSpaces,kIndentTriggerType_ReturnKey_NewLine);//#249 #639 #650
							mToolData_IndentFlag = false;
							mToolData_IndentFlag_InputSpaces = false;//#249
							mToolData_IndentFlag_OnlyNewLine = false;//#249
						}
					}
				}
			}
		}
	}
}

ABool	AView_Text::InsertToolText_Command( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, 
										   const AToolCommandID inToolCommandID, AText& outText,
										   AText& outHintText )
{
	switch(inToolCommandID)
	{
	  case tc_CARET:
	  case tc_MOVECARET_HERE:
		{
			mToolData_CaretTextIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
			mToolData_CaretReserved = true;
			mToolData_CaretMode = true;
			break;
		}
	  case tc_SELECTED:
		{
			outText.AddText(mToolData_SelectedText);
			break;
		}
	  case tc_SELECTED_PLACEHOLDER://#110
		{
			AText	altText;
			ToolCommand_ReadArg1(inText,ioPos,altText);
			if( mToolData_SelectedText.GetItemCount() == 0 )
			{
				outText.AddText(altText);
			}
			else
			{
				outText.AddText(mToolData_SelectedText);
			}
			break;
		}
	  case tc_SWITCH_WINDOW:
		{
			GetApp().SPI_SwitchWindow(true);//#552
			break;
		}
	  case tc_SWITCH_PREVWINDOW://#552
		{
			GetApp().SPI_SwitchWindow(false);
			break;
		}
	  case tc_COMPILE_TEX:
		{
			if( GetTextDocumentConst().NVI_IsDirty() == true && mToolData_DontShowDropWarning == false )
			{
				mToolData_DropWarningFlag = true;
				return false;
			}
			
			AFileAcc	file;
			GetTextDocumentConst().NVI_GetFile(file);
			GetApp().SPI_GetChildProcessManager().CompileTex(file,false);
			break;
		}
	  case tc_COMPILEPREVIEW_TEX:
		{
			if( GetTextDocumentConst().NVI_IsDirty() == true && mToolData_DontShowDropWarning == false )
			{
				mToolData_DropWarningFlag = true;
				return false;
			}
			
			AFileAcc	file;
			GetTextDocumentConst().NVI_GetFile(file);
			GetApp().SPI_GetChildProcessManager().CompileTex(file,true);
			break;
		}
	  case tc_PREVIEW_TEX:
		{
			AFileAcc	file;
			GetTextDocumentConst().NVI_GetFile(file);
			GetApp().SPI_TexPreview(file);
			break;
		}
	  case tc_WAIT:
	  case tc_WAIT_ACTIVATE:
		{
			if( inDepth == 1 )
			{
				AText	text;
				inText.GetText(ioPos,inText.GetItemCount()-ioPos,text);
				text.InsertCstring(0,"<<<");
				GetApp().SPI_Register_ToolCommandWaitText(text,GetTextDocumentConst().GetObjectID());
				ioPos = inText.GetItemCount();
			}
			break;
		}
		break;
	  case tc_SWITCH_NEXTSPLITVIEW:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SwitchFocus_SplitView(true);
			}
			break;
		}
	  case tc_SWITCH_PREVSPLITVIEW:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SwitchFocus_SplitView(false);
			}
			break;
		}
	  case tc_SPLITVIEW:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SplitView(false);
			}
			break;
		}
	  case tc_CLOSE_SPLITVIEW:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ConcatSplitView();
			}
			break;
		}
	  case tc_SHOW_DEFINITION:
		{
			AText	word;
			GetSelectedText(word);
			GetTextDocument().SPI_UpdateCurrentIdentifierDefinitionList(word);
			if( GetTextDocument().SPI_GetIdentifierDefinitionListCount() > 0 )
			{
				ATextIndex	spos, epos;
				ABool	import;
				AFileAcc	file;
				GetTextDocument().SPI_GetCurrentIdentifierDefinitionRange(0,spos,epos,import,file);
				if( import == false )
				{
					if( mTextWindowID != kObjectID_Invalid )
					{
						GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SplitView(false);
					}
					SetSelect(spos,epos);
					AdjustScroll_Top(GetCaretTextPoint());
				}
				else
				{
					//#725 GetApp().SPI_SelectTextDocument(file,spos,epos-spos);
					AText	path;
					file.GetPath(path);
					GetApp().SPI_OpenOrRevealTextDocument(path,spos,epos,kObjectID_Invalid,false,kAdjustScrollType_Center);
				}
			}
			break;
		}
		//R0017
	  case tc_TRANSLITERATE_SELECTED:
		{
			//Transliterateの種類取得
			AText	arg1;
			ToolCommand_ReadArg1(inText,ioPos,arg1);
			ABool	changecaselower = false, changecaseupper = false;
			AText	ccltext, ccutext;
			ccltext.SetLocalizedText("Transliterate_ChangeCaseLower");
			ccutext.SetLocalizedText("Transliterate_ChangeCaseUpper");
			if( arg1.Compare(ccltext) == true )   changecaselower = true;
			if( arg1.Compare(ccutext) == true )   changecaseupper = true;
			//Transliterate実行
			ATextIndex	spos,epos;
			GetSelect(spos,epos);
			AText	origtext, text;
			GetSelectedText(origtext);
			if( changecaselower == true )
			{
				/*#1035
				text.SetText(origtext);
				text.ChangeCaseLower();
				TextInput(undoTag_Transliterate,text);
				SetSelect(spos,spos+text.GetItemCount());
				*/
				TransliterateToLower();
			}
			else if( changecaseupper == true )
			{
				/*#1035
				text.SetText(origtext);
				text.ChangeCaseUpper();
				TextInput(undoTag_Transliterate,text);
				SetSelect(spos,spos+text.GetItemCount());
				*/
				TransliterateToUpper();
			}
			/*#1035
			else if( GetApp().SPI_Transliterate(arg1,origtext,text) == true )
			{
				TextInput(undoTag_Transliterate,text);
				SetSelect(spos,spos+text.GetItemCount());
			}
			*/
			else
			{
				Transliterate(arg1);
			}
			break;
		}
		//R0017
	  case tc_TRANSLITERATE_REGEXP_SELECTED:
		{
			if( GetTextDocumentConst().NVI_IsReadOnly() == true )   break;
			//Undoアクションタグ記録
			GetTextDocument().SPI_RecordUndoActionTag(undoTag_Transliterate);
			
			//Transliterateの種類取得
			AText	arg1;
			ToolCommand_ReadArg1(inText,ioPos,arg1);
			ABool	changecaselower = false, changecaseupper = false;
			AText	ccltext, ccutext;
			ccltext.SetLocalizedText("Transliterate_ChangeCaseLower");
			ccutext.SetLocalizedText("Transliterate_ChangeCaseUpper");
			if( arg1.Compare(ccltext) == true )   changecaselower = true;
			if( arg1.Compare(ccutext) == true )   changecaseupper = true;
			//選択範囲取得
			ATextIndex	selspos, selepos;
			GetSelect(selspos,selepos);
			//正規検索パラメータ取得
			AFindParameter	param;
			ToolCommand_ReadFindArg1(inText,ioPos,param.findText);
			param.regExp = true;
			param.loop = false;
			//#846
			//モーダルセッション
			AStEditProgressModalSession	modalSession(kEditProgressType_Find,true,false,true);
			try
			{
				//
				for( AIndex pos = selspos; pos < selepos;  )
				{
					//検索実行
					AIndex	endPos = selepos;
					ATextIndex	replaceStart1, replaceEnd1;
					ABool	modalSessionAborted = false;
					if( GetTextDocument().SPI_Find(param,false,pos,endPos,replaceStart1,replaceEnd1,true,modalSessionAborted) == false )   break;
					//検索一致文字列取得
					AText	origtext, text;
					GetTextDocumentConst().SPI_GetText(replaceStart1,replaceEnd1,origtext);
					//Transliterate
					if( changecaselower == true )
					{
						text.SetText(origtext);
						text.ChangeCaseLower();
						GetTextDocument().SPI_DeleteText(replaceStart1,replaceEnd1);
						GetTextDocument().SPI_InsertText(replaceStart1,text);
					}
					else if( changecaseupper == true )
					{
						text.SetText(origtext);
						text.ChangeCaseUpper();
						GetTextDocument().SPI_DeleteText(replaceStart1,replaceEnd1);
						GetTextDocument().SPI_InsertText(replaceStart1,text);
					}
					else if( GetApp().SPI_Transliterate(arg1,origtext,text) == true )
					{
						GetTextDocument().SPI_DeleteText(replaceStart1,replaceEnd1);
						GetTextDocument().SPI_InsertText(replaceStart1,text);
					}
					else
					{
						//Transliterate無効
						break;
					}
					//位置移動
					pos = replaceStart1 + text.GetItemCount();
					if( replaceStart1 == replaceEnd1 )//無限ループ防止（挿入した番号文字列の次の文字の後へ位置移動）
					{
						pos = GetTextDocumentConst().SPI_GetNextCharTextIndex(pos);
					}
					AItemCount	offset = text.GetItemCount() - (replaceEnd1-replaceStart1);
					selepos += offset;
				}
			}
			catch(...)
			{
				_ACError("",this);
			}
			break;
		}
		//R0000
	  case tc_TOOLINDENT:
		{
			mToolData_ToolIndentMode = true;
			break;
		}
		//R0000
	  case tc_CONCATALL_INPROJECT:
		{
			AModeIndex	modeIndex = GetTextDocumentConst().SPI_GetModeIndex();
			AText	text;
			ATextArray	ta;
			GetApp().SPI_GetSameProjectPathnameArrayMatchMode(modeIndex,ta);
			for( AIndex i = 0; i < ta.GetItemCount(); i++ )
			{
				AFileAcc	file;
				file.Specify(ta.GetTextConst(i));
				AText	t;
				GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_ConcatAllTool,file,t);
				text.AddText(t);
			}
			ADocumentID docID = GetApp().SPNVI_CreateNewTextDocument(modeIndex);
			if( docID == kObjectID_Invalid )   break;//win
			GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
			break;
		}
		//R0006
	  case tc_COMPARE_WITH_LATEST:
		{
			//#379 GetTextDocument().SPI_SetDiffMode(kDiffTargetMode_RepositoryLatest);//#379
			DiffWithRepository();//#379
			break;
		}
	  case tc_MOVECARET_NEXTDIFF:
		{
			NextDiff();
			break;
		}
	  case tc_MOVECARET_PREVDIFF:
		{
			PrevDiff();
			break;
		}
		//RC1
	  case tc_ARROWTODEF:
		{
			GetTextDocument().SPI_SetDrawArrowToDef(!GetTextDocumentConst().SPI_GetDrawArrowToDef());
			NVM_GetWindow().NVI_RefreshWindow();
			break;
		}
		//RC2
	  case tc_IDINFO:
		{
			DisplayIdInfo(true,kIndex_Invalid,kIndex_Invalid);
			break;
		}
	  case tc_FIX_IDINFO:
		{
			//#725 GetApp().SPI_GetIdInfoWindow().SPI_FixDisplay();
			break;
		}
			/*#858
		//RC3
	  case tc_EDIT_MYCOMMENT:
		{
			ExternalComment();
			break;
		}*/
#if IMPLEMENTATION_FOR_MACOSX
		//R0231
	  case tc_DICT_POPUP:
		{
			//未選択の場合は単語選択
			AIndex	spos, epos;
			GetSelect(spos,epos);
			AText	text;
			if( spos == epos )
			{
				GetTextDocument().SPI_FindWord(spos,spos,epos);
				SetSelect(spos,epos);
			}
			//
			DictionaryPopup();
			break;
		}
	  case tc_DICT:
		{
			//未選択の場合は単語選択
			AIndex	spos, epos;
			GetSelect(spos,epos);
			AText	text;
			if( spos == epos )
			{
				GetTextDocument().SPI_FindWord(spos,spos,epos);
				SetSelect(spos,epos);
			}
			//
			Dictionary();
			break;
		}
#endif
		//R0185
	  case tc_SHOWHIDE_CROSSCARET:
		{
			SPI_ShowHideCrossCaret();
			break;
		}
		
		//For debug
	  case tc_DUMP_UNICODE_CHARCODE:
		{
			AText	text;
			GetSelectedText(text);
			text.ConvertFromUTF8(ATextEncodingWrapper::GetUTF16LETextEncoding());
			for( AIndex i = 0; i < text.GetItemCount(); i += 2 )
			{
				fprintf(stderr,"%02X%02X\n",text.Get(i+1),text.Get(i));
			}
			break;
		}
		
		//#146
	  case tc_NAV_PREV:
		{
			GetApp().SPI_NavigatePrev();
			break;
		}
	  case tc_NAV_NEXT:
		{
			GetApp().SPI_NavigateNext();
			break;
		}
		//#232
	  case tc_TOOLBARICON:
		{
			AText	arg;
			ToolCommand_ReadArg1(inText,ioPos,arg);
			//何もしない（AModePrefのほうで使用する）
			break;
		}
	  case tc_INDEXWINDOW_NEXT:
		{
			//#438 GetApp().SPI_GoNextIndexWindowItem();
			/*#725
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IndexWindowNext();
			}
			*/
			GetApp().SPI_SelectNextItemInSubWindow();
			break;
		}
	  case tc_INDEXWINDOW_PREV:
		{
			//#438 GetApp().SPI_GoPrevIndexWindowItem();
			/*#725
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IndexWindowPrev();
			}
			*/
			GetApp().SPI_SelectPreviousItemInSubWindow();
			break;
		}
		//#50
	  case tc_SETVAR_SELECTED:
		{
			AText	arg;
			ToolCommand_ReadArg1(inText,ioPos,arg);
			AIndex	index = sToolData_VarNameArray.Find(arg);
			if( index == kIndex_Invalid )
			{
				index = sToolData_VarNameArray.Add(arg);
				sToolData_VarValueArray.Add(GetEmptyText());
			}
			AText	selectedText;//#397
			GetSelectedText(selectedText);//#397
			sToolData_VarValueArray.Set(index,selectedText);//#397mToolData_SelectedText);
			break;
		}
	  case tc_GETVAR:
		{
			AText	arg;
			ToolCommand_ReadArg1(inText,ioPos,arg);
			AIndex	index = sToolData_VarNameArray.Find(arg);
			if( index != kIndex_Invalid )
			{
				outText.AddText(sToolData_VarValueArray.GetTextConst(index));
			}
			break;
		}
		//#212
	  case tc_SHOWHIDE_SUBPANECOLUMN:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowHideLeftSideBarColumn();
			}
			break;
		}
		//#291
	  case tc_SHOWHIDE_INFOPANECOLUMN:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowHideRightSideBarColumn();
			}
			break;
		}
		//#357
	  case tc_SWITCH_NEXTTAB:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SwitchTabNext();
			}
			break;
		}
		//#357
	  case tc_SWITCH_PREVTAB:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SwitchTabPrev();
			}
			break;
		}
		//#390
	  case tc_KEYRECORD_START:
		{
			GetApp().SPI_StartRecord();
			break;
		}
	  case tc_KEYRECORD_START_WITHWINDOW:
		{
			GetApp().SPI_ShowKeyRecordWindow();
			GetApp().SPI_StartRecord();
			break;
		}
	  case tc_KEYRECORD_STOP:
		{
			GetApp().SPI_StopRecord();
			break;
		}
	  case tc_KEYRECORD_PLAY:
		{
			//Undoアクションタグ記録
			GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
			//
			AText	text;
			GetApp().SPI_GetRecordedText(text);
			InsertToolText(text,1);
			break;
		}
		//#389
	  case tc_TABTOWINDOW:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DetachTab(GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_GetCurrentTabIndex());
			}
			break;
		}
	  case tc_WINDOWTOTAB:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_WindowToTab();
			}
			break;
		}
		//#359
	  case tc_SELECT_CORRESPONDENCE:
		{
			SelectBetweenCorrespondTexts();
			break;
		}
		//#425
	  case tc_ADDITIONAL_COPY_TEXT:
		{
			AText	token;
			inText.GetToken(ioPos,token);//(読み込み
			inText.GetToken(ioPos,token);//"読み込み
			ToolCommand_ReadQuotedText(inText,ioPos,token);
			AText	argText;
			argText.SetText(token);
			inText.GetToken(ioPos,token);//)読み込み
			//
			AText	text;
			AScrapWrapper::GetClipboardTextScrap(text);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);
			text.AddText(argText);
			AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp));//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win 
			break;
		}
		//win
	  case tc_SHOW_PRINTDIALOG:
		{
			/*#524
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_Print();
			}
			*/
			GetTextDocument().SPI_Print();
			break;
		}
		//#404
	  case tc_FULLSCREEN:
		{
			/*#476
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SwitchFullScreenMode();
			}
			*/
			GetApp().SPI_SwitchFullScreenMode();
			break;
		}
		//#606
	  case tc_HANDTOOL:
		{
			GetApp().SPI_SwitchHandToolMode();
			break;
		}
		//#455
	  case tc_SCM_COMMIT:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCommitWindow(mTextDocumentRef.GetObjectID());//#695
			}
			break;
		}
		//#379
	  case tc_DIFF_REPOSITORY:
		{
			DiffWithRepository();
			break;
		}
		//#594
	  case tc_SHOW_LINEDIALOG:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowMoveLineWindow();
			}
			break;
		} 
		//#638
	  case tc_AUTOINDENTMODE:
		{
			GetApp().SPI_SetAutoIndentMode(!GetApp().SPI_GetAutoIndentMode());
			break;
		}
		//#699
	  case tc_DEBUG_CHECKLINEINFO:
		{
			GetTextDocument().SPI_CheckLineInfoDataForDebug();
			//
			GetApp().UnitTest2();
			break;
		}
		//#734
	  case tc_PREVIEW_EXECJS_SELECTED:
		{
			AText	selectedText;
			GetSelectedText(selectedText);
			//drop GetApp().SPI_ExecuteJavaScriptInPreviewerWindow(mTextWindowID,selectedText);
			break;
		}
		//#913
	  case tc_YANK:
		{
			Yank();
			break;
		}
		//ヒントテキスト
	  case tc_HINT:
		{
			/*
			AText	token;
			inText.GetToken(ioPos,token);//(読み込み
			inText.GetToken(ioPos,token);//"読み込み
			ToolCommand_ReadQuotedText(inText,ioPos,token);
			AText	argText;
			argText.SetText(token);
			inText.GetToken(ioPos,token);//)読み込み
			*/
			//読み込み
			AText	argText;
			ToolCommand_ReadArg1(inText,ioPos,argText);
			//ヒントテキスト返り値設定
			outHintText.SetText(argText);
			//対応する文字を追加
			outText.Add(kHintTextChar);//スペースにするとインデント時に消えるので?にしている
			break;
		}
		
	  case tc_FIND_DIALOG:
	  case tc_MULTIFILEFIND_DIALOG:
	  case tc_SHOW_FINDDIALOG://win
	  case tc_SHOW_MULTIFINDDIALOG://win
	  case tc_FIND_REGEXP_NEXT:
	  case tc_FIND_REGEXP_PREV:
	  case tc_REPLACE_REGEXP_NEXT:
	  case tc_REPLACE_REGEXP_AFTERCARET:
	  case tc_REPLACE_REGEXP_ALL:
	  case tc_REPLACE_REGEXP_SELECTED:
	  case tc_FIND_SELECTED_NEXT:
	  case tc_FIND_SELECTED_PREV:
	  case tc_MULTIFILEFIND_SELECTED:
	  case tc_MULTIFILEFIND_SELECTED_INPROJECT:
	  case tc_FIND_REGEXP_FROMFIRST://R0075
	  case tc_NUMBER_REGEXP_SELECTED://R0189
	  case tc_HIGHLIGHT_SELECTED://#232
	  case tc_FIND_NEXT://win
	  case tc_FIND_PREV://win
	  case tc_SETFINDTEXT_SELECTED://win
	  case tc_FIND_TEXT_NEXT://#887
	  case tc_FIND_TEXT_PREV://#887
	  case tc_FIND_TEXT_FROMFIRST://#887
	  case tc_REPLACE_TEXT_NEXT://#887
	  case tc_REPLACE_TEXT_INSELECTION://#887
	  case tc_REPLACE_TEXT_AFTERCARET://#887
	  case tc_REPLACE_TEXT_ALL://#887
	  case tc_REPLACE_TEXT_SELECTEDTEXTONLY://#887
	  case tc_FIND_EXTRACT:
		{
			InsertToolText_Command_FIND(inText,ioPos,inDepth,inToolCommandID,outText);
			break;
		}
	  case tc_HIGHLIGHT_CLEAR://#232
		{
			GetTextDocument().SPI_ClearFindHighlight();
			break;
		}
	  case tc_DATE:
	  case tc_DATE_LONG:
	  case tc_DATE_SHORT:
	  case tc_DATE_6:
	  case tc_TIME:
	  case tc_YEAR:
	  case tc_YEAR_4:
	  case tc_YEAR_2:
	  case tc_MONTH:
	  case tc_MONTH_2:
	  case tc_DAY:
	  case tc_DAY_2:
	  case tc_HOUR://R0116
	  case tc_HOUR12://R0116
	  case tc_HOUR_2://R0116
	  case tc_HOUR12_2://R0116
	  case tc_AMPM://R0116
	  case tc_MINUTE://R0116
	  case tc_MINUTE_2://R0116
	  case tc_SECOND://R0116
	  case tc_SECOND_2://R0116
	  case tc_DAYOFWEEK://#537
	  case tc_DAYOFWEEK_LONG://#537
		{
			InsertToolText_Command_DATE(inText,ioPos,inDepth,inToolCommandID,outText);
			break;
		}
	  case tc_MOVECARET_UP:
	  case tc_MOVECARET_DOWN:
	  case tc_MOVECARET_LEFT:
	  case tc_MOVECARET_RIGHT:
	  case tc_MOVECARET_HOME:
	  case tc_MOVECARET_END:
	  case tc_MOVECARET_STARTOFLINE:
	  case tc_MOVECARET_ENDOFLINE:
	  case tc_MOVECARET_PREVWORD:
	  case tc_MOVECARET_NEXTWORD:
	  case tc_MOVECARET_PREVWORD_LINESTOP://R0052
	  case tc_MOVECARET_NEXTWORD_LINESTOP://R0052
	  case tc_MOVECARET_LINE:
	  case tc_MOVECARET_PARAGRAPH:
	  case tc_MOVECARET_COLUMN:
	  case tc_MOVECARET_STARTOFPARAGRAPH:
	  case tc_MOVECARET_ENDOFPARAGRAPH:
		{
			InsertToolText_Command_MOVECARET(inText,ioPos,inDepth,inToolCommandID,outText);
			break;
		}
	  case tc_SELECT_UP:
	  case tc_SELECT_DOWN:
	  case tc_SELECT_LEFT:
	  case tc_SELECT_RIGHT:
	  case tc_SELECT_HOME:
	  case tc_SELECT_END:
	  case tc_SELECT_STARTOFLINE:
	  case tc_SELECT_ENDOFLINE:
	  case tc_SELECT_PREVWORD:
	  case tc_SELECT_NEXTWORD:
	  case tc_SELECT_ALL:
      case tc_SELECT_WHOLE_PARAGRAPH:
      case tc_SELECT_WHOLE_PARAGRAPH_NOLINEEND://#223
	  case tc_SELECT_WHOLE_LINE:
	  case tc_SELECT_PREVWORD_LINESTOP://R0063
	  case tc_SELECT_NEXTWORD_LINESTOP://R0063
	  case tc_SELECT_NEXTHEADER://#150
	  case tc_SELECT_PREVHEADER://#150
	  case tc_SELECT_STARTOFPARAGRAPH://#913
	  case tc_SELECT_ENDOFPARAGRAPH://#913
	  case tc_SETMARK://#913
	  case tc_SWAPMARK://#913
	  case tc_SELECT_MARK://#913
	  case tc_SELECT_WORD://#913
		{
			InsertToolText_Command_SELECT(inText,ioPos,inDepth,inToolCommandID,outText);
			break;
		}
	  case tc_SCROLL_UP:
	  case tc_SCROLL_DOWN:
	  case tc_SCROLL_LEFT:
	  case tc_SCROLL_RIGHT:
	  case tc_SCROLL_HOME:
	  case tc_SCROLL_END:
	  case tc_SCROLL_PREVPAGE:
	  case tc_SCROLL_NEXTPAGE:
	  case tc_SCROLL_MOVECARET_PREVPAGE:
	  case tc_SCROLL_MOVECARET_NEXTPAGE:
		{
			InsertToolText_Command_SCROLL(inText,ioPos,inDepth,inToolCommandID,outText);
			break;
		}
	  case tc_CHANGEMODE:
	  case tc_CHANGEFONT:
	  case tc_CHANGESIZE:
	  case tc_CHANGEFONT_MODEDEFAULT://#354
	  case tc_REVERTFONT://#354
	  case tc_CHANGETO_SHIFTJIS:
	  case tc_CHANGETO_JIS:
	  case tc_CHANGETO_EUC:
	  case tc_CHANGETO_UCS2:
	  case tc_CHANGETO_UTF8:
	  case tc_CHANGETO_ISO88591:
	  case tc_CHANGETO_CR:
	  case tc_CHANGETO_LF:
	  case tc_CHANGETO_CRLF:
	  case tc_CHANGETO_NOWRAP:
	  case tc_CHANGETO_WORDWRAP:
	  case tc_CHANGETO_LETTERWRAP:
	  case tc_CHANGETAB:
	  case tc_CHANGEINDENT://#251
	  case tc_CHANGECREATOR:
	  case tc_CHANGETYPE:
	  case tc_CHANGETO:
	  case tc_MODIFYTO:
		{
			InsertToolText_Command_CHANGE(inText,ioPos,inDepth,inToolCommandID,outText);
			break;
		}
	  case tc_INDENT:
	  case tc_INDENT_BYSPACE://#249
	  case tc_AUTOTAB:
	  case tc_SHIFT_LEFT:
	  case tc_SHIFT_RIGHT:
	  case tc_ABBREV_PREV:
	  case tc_ABBREV_NEXT:
	  case tc_DELETE_NEXT:
	  case tc_DELETE_PREV:
	  case tc_DELETE_SELECTED:
	  case tc_DELETE_ENDOFLINE:
	  case tc_PASTE:
	  case tc_UNDO:
	  case tc_REDO:
	  case tc_CUT:
	  case tc_COPY:
	  case tc_CUT_ENDOFLINE:
	  case tc_CUT_STARTOFLINE:
	  case tc_SELECTMODE:
	  case tc_SELECT_BRACE:
	  case tc_RETURN:
	  case tc_INDENT_RETURN:
	  case tc_INDENT_BYSPACE_RETURN://#249
	  case tc_INDENT_ONLYNEWLINE_RETURN://#249
	  case tc_INDENT_ONLYNEWLINE_BYSPACE_RETURN://#249
	  case tc_TAB:
	  case tc_TAB_BYSPACE://#249
	  case tc_INDENT_TAB:
	  case tc_INDENT_BYSPACE_TAB://#249
	  case tc_INDENT_BYSPACE_TAB_BYSPACE://#249
	  case tc_SPACE:
	  case tc_CORRESPONDENCE:
	  case tc_RECENTER:
	  case tc_SAMELETTER_UP:
	  case tc_DUPLICATE_LINE:
	  case tc_INSERT_LINE:
	  case tc_GT3:
	  case tc_LT3:
	  case tc_GT1:
	  case tc_LT1:
	  case tc_SWAPLINE:
	  case tc_SWAPLETTER://#1154
	  case tc_COPY_WITHHTMLCOLOR://R0015
	  case tc_DELETE_ENDOFPARAGRAPH://R0213
	  case tc_DELETE_WHOLE_PARAGRAPH://#124
	  case tc_CUT_ENDOFPARAGRAPH://R0213
	  case tc_ADDITIONAL_CUT_ENDOFPARAGRAPH://R0213
	  case tc_ADDITIONAL_CUT://R0213
	  case tc_ADDITIONAL_COPY://R0213
	  case tc_DELETE_STARTOFLINE://#913
	  case tc_DELETE_STARTOFPARAGRAPH://#913
	  case tc_DELETE_MARK://#913
		{
			InsertToolText_Command_EDIT(inText,ioPos,inDepth,inToolCommandID,outText);
			break;
		}
	  case tc_SAVE:
	  case tc_SAVEAS:
	  case tc_NEWDOCUMENT:
	  case tc_NEWDOCUMENT_MOVETARGET://#56
	  case tc_NEWDOCUMENT_SAMEMODE://#56
	  case tc_NEWDOCUMENT_SAMEMODE_MOVETARGET://#56
	  case tc_OPEN_DIALOG:
	  case tc_CLOSE:
	  case tc_OPEN_URL:
	  case tc_OPEN_SELECTED://B0009
	  case tc_OPEN_FILE:
	  case tc_FILENAME:
	  case tc_FILENAME_NOSUFFIX:
	  case tc_DIRECTORY:
	  case tc_FULL_PATH: 
	  case tc_TOOL:
	  case tc_KEYTOOL:
	  case tc_BROWSE:
		{
			InsertToolText_Command_FILE(inText,ioPos,inDepth,inToolCommandID,outText);
			break;
		}
	  case tc_LAUNCH:
	  case tc_DROP:
	  case tc_CLOSEDROP:
	  case tc_CLOSE_AND_DROP:
	  case tc_SENDSELECTED:
	  case tc_SEND_SELECTED:
	  case tc_SENDSELECTEDR:
	  case tc_SEND_SELECTED_REPLACE:
	  case tc_SENDSELECTEDN:
	  case tc_SEND_SELECTED_NEWDOCUMENT:
		{
			InsertToolText_Command_LAUNCH(inText,ioPos,inDepth,inToolCommandID,outText);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return false;
}

void	AView_Text::InsertToolText_Command_FIND( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, const AToolCommandID inToolCommandID, AText& outText )
{
	switch(inToolCommandID)
	{
	  case tc_FIND_DIALOG:
		{
			GetApp().SPI_ShowFindWindow(mToolData_SelectedText);
			break;
		}
	  case tc_MULTIFILEFIND_DIALOG:
		{
			GetApp().SPI_ShowMultiFileFindWindow(mToolData_SelectedText);
			break;
		}
	  case tc_SHOW_FINDDIALOG://win
		{
			GetApp().SPI_ShowFindWindow(GetEmptyText());
			break;
		}
	  case tc_SHOW_MULTIFINDDIALOG://win
		{
			GetApp().SPI_ShowMultiFileFindWindow(GetEmptyText());
			break;
		}
	  case tc_FIND_NEXT://win
		{
			SPI_FindNext();
			break;
		}
	  case tc_FIND_PREV://win
		{
			SPI_FindPrev();
			break;
		}
	  case tc_SETFINDTEXT_SELECTED://win
		{
			SetFindText();
			break;
		}
	  case tc_FIND_REGEXP_NEXT:
		{
			//Findパラメータ設定
			AFindParameter	param;
			param.loop = false;//#5
			ParseFindOption(inText,ioPos,param);//#5
			ToolCommand_ReadFindArg1(inText,ioPos,param.findText);
			param.regExp = true;
			//#5 param.loop = false;
			//検索実行
			SPI_FindNext(param);
			break;
		}
	  case tc_FIND_REGEXP_PREV:
		{
			//Findパラメータ設定
			AFindParameter	param;
			param.loop = false;//#5
			ParseFindOption(inText,ioPos,param);//#5
			ToolCommand_ReadFindArg1(inText,ioPos,param.findText);
			param.regExp = true;
			//#5 param.loop = false;
			//検索実行
			SPI_FindPrevious(param);
			break;
		}
	  case tc_FIND_REGEXP_FROMFIRST://R0075
		{
			//Findパラメータ設定
			AFindParameter	param;
			param.loop = false;//#5
			ParseFindOption(inText,ioPos,param);//#5
			ToolCommand_ReadFindArg1(inText,ioPos,param.findText);
			param.regExp = true;
			//#5 param.loop = false;
			//検索実行
			SPI_FindFromFirst(param);
			break;
		}
	  case tc_REPLACE_REGEXP_NEXT:
		{
			//Findパラメータ設定
			AFindParameter	param;
			param.loop = false;//#5
			ParseFindOption(inText,ioPos,param);//#5
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.regExp = true;
			//#5 param.loop = false;
			//置換実行
			SPI_ReplaceNext(param);
			break;
		}
	  case tc_REPLACE_REGEXP_AFTERCARET:
		{
			//Findパラメータ設定
			AFindParameter	param;
			param.loop = false;//#5
			ParseFindOption(inText,ioPos,param);//#5
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.regExp = true;
			//#5 param.loop = false;
			//置換実行
			SPI_ReplaceAfterCaret(param);
			break;
		}
	  case tc_REPLACE_REGEXP_ALL:
		{
			//Findパラメータ設定
			AFindParameter	param;
			param.loop = false;//#5
			ParseFindOption(inText,ioPos,param);//#5
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.regExp = true;
			//#5 param.loop = false;
			//置換実行
			SPI_ReplaceAll(param);
			break;
		}
	  case tc_REPLACE_REGEXP_SELECTED:
		{
			//Findパラメータ設定
			AFindParameter	param;
			param.loop = false;//#5
			ParseFindOption(inText,ioPos,param);//#5
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.regExp = true;
			//#5 param.loop = false;
			//置換実行
			SPI_ReplaceInSelection(param);
			break;
		}
		//#887
	  case tc_FIND_TEXT_NEXT://#887
		{
			//Findパラメータ設定
			AFindParameter	param;
			ParseFindOption(inText,ioPos,param);
			ToolCommand_ReadFindArg1(inText,ioPos,param.findText);
			param.findText.ConvertToUnescapedText();
			//検索実行
			SPI_FindNext(param);
			break;
		}
	  case tc_FIND_TEXT_PREV://#887
		{
			//Findパラメータ設定
			AFindParameter	param;
			ParseFindOption(inText,ioPos,param);
			ToolCommand_ReadFindArg1(inText,ioPos,param.findText);
			param.findText.ConvertToUnescapedText();
			//検索実行
			SPI_FindPrevious(param);
			break;
		}
	  case tc_FIND_TEXT_FROMFIRST://#887
		{
			//Findパラメータ設定
			AFindParameter	param;
			ParseFindOption(inText,ioPos,param);
			ToolCommand_ReadFindArg1(inText,ioPos,param.findText);
			param.findText.ConvertToUnescapedText();
			//検索実行
			SPI_FindFromFirst(param);
			break;
		}
	  case tc_REPLACE_TEXT_NEXT://#887
		{
			//Findパラメータ設定
			AFindParameter	param;
			ParseFindOption(inText,ioPos,param);
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.findText.ConvertToUnescapedText();
			param.replaceText.ConvertToUnescapedText();
			//置換実行
			SPI_ReplaceNext(param);
			break;
		}
	  case tc_REPLACE_TEXT_INSELECTION://#887
		{
			//Findパラメータ設定
			AFindParameter	param;
			ParseFindOption(inText,ioPos,param);
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.findText.ConvertToUnescapedText();
			param.replaceText.ConvertToUnescapedText();
			//置換実行
			SPI_ReplaceInSelection(param);
			break;
		}
	  case tc_REPLACE_TEXT_AFTERCARET://#887
		{
			//Findパラメータ設定
			AFindParameter	param;
			ParseFindOption(inText,ioPos,param);
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.findText.ConvertToUnescapedText();
			param.replaceText.ConvertToUnescapedText();
			//置換実行
			SPI_ReplaceAfterCaret(param);
			break;
		}
	  case tc_REPLACE_TEXT_ALL://#887
		{
			//Findパラメータ設定
			AFindParameter	param;
			ParseFindOption(inText,ioPos,param);
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.findText.ConvertToUnescapedText();
			param.replaceText.ConvertToUnescapedText();
			//置換実行
			SPI_ReplaceAll(param);
			break;
		}
	  case tc_REPLACE_TEXT_SELECTEDTEXTONLY://#887
		{
			//Findパラメータ設定
			AFindParameter	param;
			ParseFindOption(inText,ioPos,param);
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.findText.ConvertToUnescapedText();
			param.replaceText.ConvertToUnescapedText();
			//置換実行
			SPI_ReplaceSelectedTextOnly(param);
			break;
		}
		//抽出 #887
	  case tc_FIND_EXTRACT:
		{
			//Findパラメータ設定
			AFindParameter	param;
			ParseFindOption(inText,ioPos,param);
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.findText.ConvertToUnescapedText();
			param.replaceText.ConvertToUnescapedText();
			//抽出実行
			SPI_FindList(param,true);
			break;
		}
		//
	  case tc_FIND_SELECTED_NEXT:
		{
			//#5
			AFindParameter	param;
			param.ignoreCase 	= true;
			param.wholeWord 	= false;
			param.fuzzy			= true;
			param.regExp 		= false;
			param.loop 			= false;
			param.ignoreBackslashYen = true;
			param.ignoreSpaces	= true;//#165
			ParseFindOption(inText,ioPos,param);
			
			if( mToolData_SelectedText.GetItemCount() > 0 )
			{
				//Findパラメータ設定
				//#5 AFindParameter	param;
				param.findText.SetText(mToolData_SelectedText);
				/*#5 param.ignoreCase 	= true;
				param.wholeWord 	= false;
				param.fuzzy			= true;
				param.regExp 		= false;
				param.loop 			= false;
				param.ignoreBackslashYen = true;*/
				//検索実行
				SPI_FindNext(param);
			}
			break;
		}
	  case tc_FIND_SELECTED_PREV:
		{
			//#5
			AFindParameter	param;
			param.ignoreCase 	= true;
			param.wholeWord 	= false;
			param.fuzzy			= true;
			param.regExp 		= false;
			param.loop 			= false;
			param.ignoreBackslashYen = true;
			param.ignoreSpaces	= true;//#165
			ParseFindOption(inText,ioPos,param);
			
			if( mToolData_SelectedText.GetItemCount() > 0 )
			{
				//Findパラメータ設定
				//#5 AFindParameter	param;
				param.findText.SetText(mToolData_SelectedText);
				/*#5 param.ignoreCase 	= true;
				param.wholeWord 	= false;
				param.fuzzy			= true;
				param.regExp 		= false;
				param.loop 			= false;
				param.ignoreBackslashYen = true;*/
				//検索実行
				SPI_FindPrevious(param);
			}
			break;
		}
		//#232
	  case tc_HIGHLIGHT_SELECTED:
		{
			AFindParameter	param;
			param.ignoreCase 	= true;
			param.wholeWord 	= false;
			param.fuzzy			= true;
			param.regExp 		= false;
			param.loop 			= false;
			param.ignoreBackslashYen = true;
			param.ignoreSpaces	= true;//#165
			ParseFindOption(inText,ioPos,param);
			
			if( mToolData_SelectedText.GetItemCount() > 0 )
			{
				//Findパラメータ設定
				param.findText.SetText(mToolData_SelectedText);
				//検索実行
				GetTextDocument().SPI_SetFindHighlight(param);
			}
			break;
		}
	  case tc_MULTIFILEFIND_SELECTED:
		{
			if( GetApp().SPI_IsMultiFileFindWorking() )   break;
			//#5
			AFindParameter	param;
			param.ignoreCase 	= true;//B0000
			param.wholeWord 	= false;
			param.fuzzy			= true;
			param.regExp 		= false;
			param.loop 			= false;
			param.ignoreBackslashYen = true;
			param.ignoreSpaces	= true;//#165
			param.recursive		= true;
			param.displayPosition = true;
			//param.modeIndex		= kIndex_Invalid;
			ParseFindOption(inText,ioPos,param);
			
			if( mToolData_SelectedText.GetItemCount() > 0 )
			{
				//フォルダ取得
				AFileAcc	file;
				GetTextDocumentConst().NVI_GetFile(file);
				AFileAcc	folder;
				folder.SpecifyParent(file);
				//Findパラメータ設定
				//#5 AFindParameter	param;
				param.findText.SetText(mToolData_SelectedText);
				if( param.filterText.GetItemCount() == 0 )//#273
				{
					param.filterText.SetCstring(".+");
				}
				/*#5 param.ignoreCase 	= true;//B0000
				param.wholeWord 	= false;
				param.fuzzy			= true;
				param.regExp 		= false;
				param.loop 			= false;
				param.ignoreBackslashYen = true;
				param.recursive		= true;
				param.displayPosition = true;
				param.modeIndex		= kIndex_Invalid;*/
				folder.GetPath(param.folderPath);
				//検索実行
				GetApp().SPI_StartMultiFileFind(param,GetApp().SPI_GetOrCreateFindResultWindowDocument(),false,false);//#725
				//マルチファイル検索ウインドウの「最近使った～」に追加
				GetApp().SPI_AddRecentlyUsed(mToolData_SelectedText,folder);
			}
			break;
		}
	  case tc_MULTIFILEFIND_SELECTED_INPROJECT:
		{
			if( GetApp().SPI_IsMultiFileFindWorking() )   break;
			//#5
			AFindParameter	param;
			param.ignoreCase 	= true;//B0000
			param.wholeWord 	= false;
			param.fuzzy			= true;
			param.regExp 		= false;
			param.loop 			= false;
			param.ignoreBackslashYen = true;
			param.ignoreSpaces	= true;//#165
			param.recursive		= true;
			param.displayPosition = true;
			//param.modeIndex		= kIndex_Invalid;
			ParseFindOption(inText,ioPos,param);
			
			if( mToolData_SelectedText.GetItemCount() > 0 )
			{
				//フォルダ取得
				AFileAcc	folder;
				GetTextDocumentConst().SPI_GetProjectFolder(folder);
				if( folder.IsSpecified() == false )
				{//B0000 プロジェクトフォルダ非存在の場合はtc_MULTIFILEFIND_SELECTEDと同じ動作にする
					AFileAcc	file;
					GetTextDocumentConst().NVI_GetFile(file);
					folder.SpecifyParent(file);
				}
				//Findパラメータ設定
				//#5 AFindParameter	param;
				param.findText.SetText(mToolData_SelectedText);
				if( param.filterText.GetItemCount() == 0 )//#273
				{
					param.filterText.SetCstring(".+");
				}
				/*#5 param.ignoreCase 	= true;//B0000
				param.wholeWord 	= false;
				param.fuzzy			= true;
				param.regExp 		= false;
				param.loop 			= false;
				param.ignoreBackslashYen = true;
				param.recursive		= true;
				param.displayPosition = true;
				param.modeIndex		= kIndex_Invalid;*/
				folder.GetPath(param.folderPath);
				//検索実行
				GetApp().SPI_StartMultiFileFind(param,GetApp().SPI_GetOrCreateFindResultWindowDocument(),false,false);//#725
				//マルチファイル検索ウインドウの「最近使った～」に追加
				GetApp().SPI_AddRecentlyUsed(mToolData_SelectedText,folder);
			}
			break;
		}
	  case tc_NUMBER_REGEXP_SELECTED://R0189
		{
			if( GetTextDocumentConst().NVI_IsReadOnly() == true )   break;
			//Undoアクションタグ記録
			GetTextDocument().SPI_RecordUndoActionTag(undoTag_Typing);
			
			//選択範囲取得
			ATextIndex	selspos, selepos;
			GetSelect(selspos,selepos);
			//正規検索パラメータ取得
			AFindParameter	param;
			param.loop = false;//#5
			ParseFindOption(inText,ioPos,param);//#5
			ToolCommand_ReadFindArg2(inText,ioPos,param.findText,param.replaceText);
			param.regExp = true;
			//#5 param.loop = false;
			//開始番号取得
			AText	startnumtext;
			ioPos--;
			ToolCommand_ReadFindArg1(inText,ioPos,startnumtext);
			
			//B0000 security
			//%(+数字)の後にd,i,o,x,X,u以外の文字が存在する場合、および、%が2回以上存在する場合はNGとする
			ABool	percentFound = false;
			for( AIndex pos = 0; pos < param.replaceText.GetItemCount(); pos++ )
			{
				AUChar	ch = param.replaceText.Get(pos);
				if( ch == '%')
				{
					if( percentFound == false )
					{
						pos++;
						for( ; pos < param.replaceText.GetItemCount(); pos++ )
						{
							ch = param.replaceText.Get(pos);
							if( (ch >= '0' && ch <= '9') )
							{
								continue;
							}
							else if( ch == 'd' || ch == 'i' || ch == 'o' || ch == 'x' || ch == 'X' || ch == 'u' )
							{
								break;
							}
							else
							{
								param.replaceText.DeleteAll();
								break;
							}
						}
						percentFound = true;
					}
					else
					{
						param.replaceText.DeleteAll();
					}
				}
			}
			if( percentFound == false || param.replaceText.GetItemCount() == 0 )   break;
			
			//#846
			//モーダルセッション
			AStEditProgressModalSession	modalSession(kEditProgressType_Find,true,false,true);
			try
			{
				//
				ANumber	num = startnumtext.GetNumber();
				for( AIndex pos = selspos; pos < selepos;  )
				{
					//検索実行
					AIndex	endPos = selepos;
					ATextIndex	replaceStart1, replaceEnd1;
					ABool	modalSessionAborted = false;
					if( GetTextDocument().SPI_Find(param,false,pos,endPos,replaceStart1,replaceEnd1,true,modalSessionAborted) == false )   break;
					//置換文字列変換
					AText	replaceText;
					GetTextDocumentConst().SPI_ChangeReplaceText(param.replaceText,replaceText);
					GetTextDocument().SPI_DeleteText(replaceStart1,replaceEnd1);
					AText	numtext;
					{
						//B0000 security さらに正規表現置換実行後のテキスト（replaceText）に対してもチェック実行
						//%(+数字)の後にd,i,o,x,X,u以外の文字が存在する場合、および、%が2回以上存在する場合はNGとする
						ABool	percentFound = false;
						for( AIndex pos = 0; pos < replaceText.GetItemCount(); pos++ )
						{
							AUChar	ch = replaceText.Get(pos);
							if( ch == '%')
							{
								if( percentFound == false )
								{
									pos++;
									for( ; pos < replaceText.GetItemCount(); pos++ )
									{
										ch = replaceText.Get(pos);
										if( (ch >= '0' && ch <= '9') )
										{
											continue;
										}
										else if( ch == 'd' || ch == 'i' || ch == 'o' || ch == 'x' || ch == 'X' || ch == 'u' )
										{
											break;
										}
										else
										{
											replaceText.DeleteAll();
											break;
										}
									}
									percentFound = true;
								}
								else
								{
									replaceText.DeleteAll();
								}
							}
						}
						if( percentFound == true && replaceText.GetItemCount() > 0 )
						{
							AStCreateCstringFromAText	replacecstr(replaceText);
							//番号文字列設定
							numtext.SetFormattedCstring(replacecstr.GetPtr(),num);
							GetTextDocument().SPI_InsertText(replaceStart1,numtext);
						}
					}
					//位置移動
					pos = replaceStart1 + numtext.GetItemCount();
					if( replaceStart1 == replaceEnd1 )//無限ループ防止（挿入した番号文字列の次の文字の後へ位置移動）
					{
						pos = GetTextDocumentConst().SPI_GetNextCharTextIndex(pos);
					}
					AItemCount	offset = numtext.GetItemCount() - (replaceEnd1-replaceStart1);
					selepos += offset;
					num++;
				}
			}
			catch(...)
			{
				_ACError("",this);
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

//#5
//検索オプション  MULTIFILEFIND-SELECTED-INPROJECT(ONLYVISIBLEFILE ONLYTEXTFILE)のような形で指定可能にする（括弧なしの形式も可能）
void	AView_Text::ParseFindOption( const AText& inText, ATextIndex& ioPos, AFindParameter& ioParameter )
{
	if( ioPos >= inText.GetItemCount() )   return;
	
	//デフォルトの値を設定
	ioParameter.onlyVisibleFile		= false;
	ioParameter.onlyTextFile		= false;
	ioParameter.ignoreCase 			= true;
	ioParameter.wholeWord 			= false;
	ioParameter.fuzzy				= true;
	ioParameter.loop 				= false;
	ioParameter.ignoreBackslashYen	= true;
	ioParameter.recursive			= true;
	ioParameter.displayPosition		= true;
	ioParameter.ignoreSpaces		= true;
	ioParameter.regExp				= false;
	
	//#273 AText	option;
	if( inText.Get(ioPos) == '(' )
	{
		//#273 ToolCommand_ReadArg1(inText,ioPos,option);
		ioPos++;//#273
		//括弧内トークンきりだし
		AText	token;
		//#273 for( AIndex pos = 0; pos < option.GetItemCount(); pos++ )
		for( ; ioPos < inText.GetItemCount(); )//#273
		{
			//#273 option.GetToken(pos,token);
			inText.GetToken(ioPos,token);//#273
			if( token.Compare("ONLYVISIBLEFILE") == true )
			{
				ioParameter.onlyVisibleFile = true;
			}
			else if( token.Compare("ONLYTEXTFILE") == true )
			{
				ioParameter.onlyTextFile = true;
			}
			else if( token.Compare("CASESENSITIVE") == true )
			{
				ioParameter.ignoreCase = false;
			}
			else if( token.Compare("WHOLEWORD") == true )
			{
				ioParameter.wholeWord = true;
			}
			else if( token.Compare("NOFUZZY") == true )
			{
				ioParameter.fuzzy = false;
			}
			else if( token.Compare("LOOP") == true )
			{
				ioParameter.loop = true;
			}
			else if( token.Compare("BACKSLASHYENSENSITIVE") == true )
			{
				ioParameter.ignoreBackslashYen = false;
			}
			else if( token.Compare("NORECURSIVE") == true )
			{
				ioParameter.recursive = false;
			}
			else if( token.Compare("NODISPLAYPOSITION") == true )
			{
				ioParameter.displayPosition = false;
			}
			//#273
			else if( token.Compare("FILTER") == true )
			{
				inText.GetToken(ioPos,token);//"読み込み
				ToolCommand_ReadQuotedText(inText,ioPos,token);
				ioParameter.filterText.SetText(token);
			}
			//#165
			else if( token.Compare("SPACESSENSITIVE") == true )
			{
				ioParameter.ignoreSpaces = false;
			}
			//#887
			else if( token.Compare("REGEXP") == true )
			{
				ioParameter.regExp = true;
			}
			//
			else if( token.Compare(")") == true )
			{
				break;
			}
		}
	}
}

void	AView_Text::InsertToolText_Command_DATE( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, const AToolCommandID inToolCommandID, AText& outText )
{
	switch(inToolCommandID)
	{
	  case tc_DATE:
	  case tc_DATE_LONG:
		{
			AText	text;
			ADateTimeWrapper::GetLongDateText(text);
			outText.AddText(text);
			break;
		}
	  case tc_DATE_SHORT:
		{
			AText	text;
			ADateTimeWrapper::GetShortDateText(text);
			outText.AddText(text);
			break;
		}
	  case tc_DATE_6:
		{
			AText	text;
			ADateTimeWrapper::Get6LettersDateText(text);
			outText.AddText(text);
			break;
		}
	  case tc_DAYOFWEEK://#537
		{
			AText	text;
			ADateTimeWrapper::GetDayOfWeek(text);
			outText.AddText(text);
			break;
		}
	  case tc_DAYOFWEEK_LONG://#537
		{
			AText	text;
			ADateTimeWrapper::GetDayOfWeekLong(text);
			outText.AddText(text);
			break;
		}
	  case tc_TIME:
		{
			AText	text;
			ADateTimeWrapper::GetTimeText(text);
			outText.AddText(text);
			break;
		}
	  case tc_YEAR:
	  case tc_YEAR_4:
		{
			AText	text;
			ADateTimeWrapper::GetYearText(text);
			outText.AddText(text);
			break;
		}
	  case tc_YEAR_2:
		{
			AText	text;
			ADateTimeWrapper::Get2LettersYearText(text);
			outText.AddText(text);
			break;
		}
	  case tc_MONTH:
		{
			AText	text;
			ADateTimeWrapper::GetMonthText(text);
			outText.AddText(text);
			break;
		}
	  case tc_MONTH_2:
		{
			AText	text;
			ADateTimeWrapper::Get2LettersMonthText(text);
			outText.AddText(text);
			break;
		}
	  case tc_DAY:
		{
			AText	text;
			ADateTimeWrapper::GetDayText(text);
			outText.AddText(text);
			break;
		}
	  case tc_DAY_2:
		{
			AText	text;
			ADateTimeWrapper::Get2LettersDayText(text);
			outText.AddText(text);
			break;
		}
		//R0116
	  case tc_HOUR:
		{
			AText	text;
			ADateTimeWrapper::GetHourText(text);
			outText.AddText(text);
			break;
		}
	  case tc_HOUR12:
		{
			AText	text;
			ADateTimeWrapper::GetHour12Text(text);
			outText.AddText(text);
			break;
		}
	  case tc_HOUR_2:
		{
			AText	text;
			ADateTimeWrapper::Get2LettersHourText(text);
			outText.AddText(text);
			break;
		}
	  case tc_HOUR12_2:
		{
			AText	text;
			ADateTimeWrapper::Get2LettersHour12Text(text);
			outText.AddText(text);
			break;
		}
	  case tc_AMPM:
		{
			AText	text;
			ADateTimeWrapper::GetAMPM(text);
			outText.AddText(text);
			break;
		}
	  case tc_MINUTE:
		{
			AText	text;
			ADateTimeWrapper::GetMinuteText(text);
			outText.AddText(text);
			break;
		}
	  case tc_MINUTE_2:
		{
			AText	text;
			ADateTimeWrapper::Get2LettersMinuteText(text);
			outText.AddText(text);
			break;
		}
	  case tc_SECOND:
		{
			AText	text;
			ADateTimeWrapper::GetSecondText(text);
			outText.AddText(text);
			break;
		}
	  case tc_SECOND_2:
		{
			AText	text;
			ADateTimeWrapper::Get2LettersSecondText(text);
			outText.AddText(text);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

void	AView_Text::InsertToolText_Command_MOVECARET( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, const AToolCommandID inToolCommandID, AText& outText )
{
	switch(inToolCommandID)
	{
	  case tc_MOVECARET_UP:
		{
			ArrowKey(keyAction_caretup);
			break;
		}
	  case tc_MOVECARET_DOWN:
		{
			ArrowKey(keyAction_caretdown);
			break;
		}
	  case tc_MOVECARET_LEFT:
		{
			ArrowKey(keyAction_caretleft);
			break;
		}
	  case tc_MOVECARET_RIGHT:
		{
			ArrowKey(keyAction_caretright);
			break;
		}
	  case tc_MOVECARET_HOME:
		{
			ArrowKeyEdge(keyAction_caretup,false,false);
			break;
		}
	  case tc_MOVECARET_END:
		{
			ArrowKeyEdge(keyAction_caretdown,false,false);
			break;
		}
	  case tc_MOVECARET_STARTOFLINE:
		{
			ArrowKeyEdge(keyAction_caretleft,false,false);
			break;
		}
	  case tc_MOVECARET_ENDOFLINE:
		{
			ArrowKeyEdge(keyAction_caretright,false,false);
			break;
		}
	  case tc_MOVECARET_STARTOFPARAGRAPH:
		{
			ArrowKeyEdge(keyAction_caretleft,false,true);
			break;
		}
	  case tc_MOVECARET_ENDOFPARAGRAPH:
		{
			ArrowKeyEdge(keyAction_caretright,false,true);
			break;
		}
	  case tc_MOVECARET_PREVWORD:
		{
			ArrowKeyWord(keyAction_caretleft,false,false);
			break;
		}
	  case tc_MOVECARET_NEXTWORD:
		{
			ArrowKeyWord(keyAction_caretright,false,false);
			break;
		}
	  case tc_MOVECARET_PREVWORD_LINESTOP://R0052
		{
			ArrowKeyWord(keyAction_caretleft,false,true);
			break;
		}
	  case tc_MOVECARET_NEXTWORD_LINESTOP://R0052
		{
			ArrowKeyWord(keyAction_caretright,false,true);
			break;
		}
	  case tc_MOVECARET_LINE:
		{
			AText	arg1;
			ToolCommand_ReadArg1(inText,ioPos,arg1);
			SPI_MoveToLine(arg1.GetNumber()-1,false);
			ArrowKey(keyAction_caretleft);
			break;
		}
	  case tc_MOVECARET_PARAGRAPH:
		{
			AText	arg1;
			ToolCommand_ReadArg1(inText,ioPos,arg1);
			SPI_MoveToLine(arg1.GetNumber()-1,true);
			ArrowKey(keyAction_caretleft);
			break;
		}
	  case tc_MOVECARET_COLUMN:
		{
			AText	arg1;
			ToolCommand_ReadArg1(inText,ioPos,arg1);
			ATextIndex	linestart = GetTextDocumentConst().SPI_GetLineStart(GetCaretTextPoint().y);
			ATextPoint	textpoint;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(GetTextDocumentConst().SPI_GetTextIndexByUTF8CharOffset(linestart,arg1.GetNumber()),textpoint);
			if( textpoint.y == GetCaretTextPoint().y )
			{
				SetCaretTextPoint(textpoint);
			}
			else
			{
				ArrowKeyEdge(keyAction_caretright,false,false);
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

//
void	AView_Text::InsertToolText_Command_SELECT( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, const AToolCommandID inToolCommandID, AText& outText )
{
	switch(inToolCommandID)
	{
	  case tc_SELECT_UP:
		{
			ArrowKeySelect(keyAction_caretup);
			break;
		}
	  case tc_SELECT_DOWN:
		{
			ArrowKeySelect(keyAction_caretdown);
			break;
		}
	  case tc_SELECT_LEFT:
		{
			ArrowKeySelect(keyAction_caretleft);
			break;
		}
	  case tc_SELECT_RIGHT:
		{
			ArrowKeySelect(keyAction_caretright);
			break;
		}
	  case tc_SELECT_HOME:
		{
			ArrowKeyEdge(keyAction_caretup,true,false);
			break;
		}
	  case tc_SELECT_END:
		{
			ArrowKeyEdge(keyAction_caretdown,true,false);
			break;
		}
	  case tc_SELECT_STARTOFLINE:
		{
			ArrowKeyEdge(keyAction_caretleft,true,false);
			break;
		}
	  case tc_SELECT_ENDOFLINE:
		{
			ArrowKeyEdge(keyAction_caretright,true,false);
			break;
		}
	  case tc_SELECT_STARTOFPARAGRAPH://#913
		{
			ArrowKeyEdge(keyAction_caretleft,true,true);
			break;
		}
	  case tc_SELECT_ENDOFPARAGRAPH://#913
		{
			ArrowKeyEdge(keyAction_caretright,true,true);
			break;
		}
	  case tc_SETMARK://#913
		{
			SetMark();
			break;
		}
	  case tc_SWAPMARK://#913
		{
			SwapMark();
			break;
		}
	  case tc_SELECT_MARK://#913
		{
			SelectToMark();
			break;
		}
	  case tc_SELECT_PREVWORD:
		{
			ArrowKeyWord(keyAction_caretleft,true,false);
			break;
		}
	  case tc_SELECT_NEXTWORD:
		{
			ArrowKeyWord(keyAction_caretright,true,false);
			break;
		}
	  case tc_SELECT_PREVWORD_LINESTOP://R0063
		{
			ArrowKeyWord(keyAction_caretleft,true,true);
			break;
		}
	  case tc_SELECT_NEXTWORD_LINESTOP://R0063
		{
			ArrowKeyWord(keyAction_caretright,true,true);
			break;
		}
	  case tc_SELECT_ALL:
		{
			SetSelect(0,GetTextDocumentConst().SPI_GetTextLength());
			break;
		}
	  case tc_SELECT_WHOLE_PARAGRAPH:
		{
			SelectWholeParagraph();
			break;
        }
        //#223
	  case tc_SELECT_WHOLE_PARAGRAPH_NOLINEEND:
		{
			ATextPoint	spt, ept;
			SPI_GetSelect(spt,ept);
			if( spt.x != ept.x || spt.y != ept.y )//B0327 条件追加（空行でキャレット状態の場合はept.y--しないようにする）
			{
				if( ept.x == 0 && ept.y > 0 )   ept.y--;
			}
			ATextIndex	spos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y));
            ATextIndex	epos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ept.y)+1);
            if( epos > 0 )
            {
                if( GetTextDocumentConst().SPI_GetTextChar(epos-1) == kUChar_LineEnd && epos-1 >= spos )
                {
                    epos--;
                }
            }
			SetSelect(spos,epos);
			break;
		}
	  case tc_SELECT_WHOLE_LINE:
		{
			SelectWholeLine();
			break;
		}
		//#150
	  case tc_SELECT_NEXTHEADER:
		{
			JumpNext();
			break;
		}
	  case tc_SELECT_PREVHEADER:
		{
			JumpPrev();
			break;
		}
		//#913
	  case tc_SELECT_WORD:
		{
			SelectWord();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//
void	AView_Text::InsertToolText_Command_SCROLL( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, const AToolCommandID inToolCommandID, AText& outText )
{
	switch(inToolCommandID)
	{
	  case tc_SCROLL_UP:
		{
			NVI_Scroll(0,-NVI_GetVScrollBarUnit());
			break;
		}
	  case tc_SCROLL_DOWN:
		{
			NVI_Scroll(0,NVI_GetVScrollBarUnit());
			break;
		}
	  case tc_SCROLL_LEFT:
		{
			NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
			break;
		}
	  case tc_SCROLL_RIGHT:
		{
			NVI_Scroll(NVI_GetHScrollBarUnit(),0);
			break;
		}
	  case tc_SCROLL_HOME:
		{
			HomeKey();
			break;
		}
	  case tc_SCROLL_END:
		{
			EndKey();
			break;
		}
	  case tc_SCROLL_PREVPAGE:
		{
			PageupKey(false);
			break;
		}
	  case tc_SCROLL_NEXTPAGE:
		{
			PagedownKey(false);
			break;
		}
	  case tc_SCROLL_MOVECARET_PREVPAGE:
		{
			PageupKey(true);
			break;
		}
	  case tc_SCROLL_MOVECARET_NEXTPAGE:
		{
			PagedownKey(true);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//
void	AView_Text::InsertToolText_Command_CHANGE( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, const AToolCommandID inToolCommandID, AText& outText )
{
	switch(inToolCommandID)
	{
	  case tc_CHANGETO:
		{
			AText	tecname;
			ToolCommand_ReadArg1(inText,ioPos,tecname);
			if( ATextEncodingWrapper::CheckTextEncodingAvailability(tecname) == false )   break;
			GetTextDocument().SPI_SetTextEncoding(tecname);
			break;
		}
		//tc_CHANGETO_SHIFTJISからtc_CHANGETO_ISO88591のツールコマンドは互換性のため残す
	  case tc_CHANGETO_SHIFTJIS:
		{
			AText	tecname("Shift_JIS");
			GetTextDocument().SPI_SetTextEncoding(tecname);//#934 ATextEncodingWrapper::GetSJISTextEncoding());
			break;
		}
	  case tc_CHANGETO_JIS:
		{
			AText	tecname("ISO-2022-JP");
			GetTextDocument().SPI_SetTextEncoding(tecname);//#934 ATextEncodingWrapper::GetJISTextEncoding());
			break;
		}
	  case tc_CHANGETO_EUC:
		{
			AText	tecname("EUC-JP");
			GetTextDocument().SPI_SetTextEncoding(tecname);//#934 ATextEncodingWrapper::GetEUCTextEncoding());
			break;
		}
	  case tc_CHANGETO_UCS2:
		{
			AText	tecname("UTF-16");
			GetTextDocument().SPI_SetTextEncoding(tecname);//#934 ATextEncodingWrapper::GetUTF16TextEncoding());
			break;
		}
	  case tc_CHANGETO_UTF8:
		{
			AText	tecname("UTF-8");
			GetTextDocument().SPI_SetTextEncoding(tecname);//#934 ATextEncodingWrapper::GetUTF8TextEncoding());
			break;
		}
	  case tc_CHANGETO_ISO88591:
		{
			AText	tecname;
			tecname.SetCstring("ISO-8859-1");
			if( ATextEncodingWrapper::CheckTextEncodingAvailability(tecname) == false )   break;
			GetTextDocument().SPI_SetTextEncoding(tecname);
			break;
		}
	  case tc_MODIFYTO:
		{
			if( GetTextDocument().NVI_IsDirty() == false )
			{
				AText	text;
				ToolCommand_ReadArg1(inText,ioPos,text);
				ATextEncodingFallbackType	resultFallbackType = kTextEncodingFallbackType_None;//#473
				GetTextDocument().SPI_ModifyTextEncoding(text,resultFallbackType);//#473
			}
			break;
		}
	  case tc_CHANGEMODE://未テスト
		{
			AText	text;
			ToolCommand_ReadArg1(inText,ioPos,text);
			AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(text);
			if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
			GetTextDocument().SPI_SetMode(modeIndex);
			break;
		}
	  case tc_CHANGEFONT:
		{
			AText	text;
			ToolCommand_ReadArg1(inText,ioPos,text);
			/*win AFont	font;
			if( AFontWrapper::GetFontByName(text,font) == true )
			{
				GetTextDocument().SPI_SetFont(font);
			}*/
			GetTextDocument().SPI_SetFontName(text);//win
			break;
		}
	  case tc_CHANGESIZE:
		{
			AText	text;
			ToolCommand_ReadArg1(inText,ioPos,text);
			AFloatNumber	fontsize = text.GetFloatNumber();
			if( fontsize >= 6.0 )//B0398 最小値変更
			{
				GetTextDocument().SPI_SetFontSize(fontsize);
			}
			break;
		}
		//#354
	  case tc_CHANGEFONT_MODEDEFAULT:
		{
			AText	fontname;
			GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).
					GetModeData_Text(AModePrefDB::kDefaultFontName,fontname);
			AFloatNumber	fontsize = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).
					GetModeData_FloatNumber(AModePrefDB::kDefaultFontSize);
			GetTextDocument().SPI_SetFontNameAndSize(fontname,fontsize);
			break;
		}
		//#354
	  case tc_REVERTFONT:
		{
			GetTextDocument().SPI_RevertFontNameAndSize();
			break;
		}
	  case tc_CHANGETO_CR:
		{
			GetTextDocument().SPI_SetReturnCode(returnCode_CR);
			break;
		}
	  case tc_CHANGETO_LF:
		{
			GetTextDocument().SPI_SetReturnCode(returnCode_LF);
			break;
		}
	  case tc_CHANGETO_CRLF:
		{
			GetTextDocument().SPI_SetReturnCode(returnCode_CRLF);
			break;
		}
	  case tc_CHANGETO_NOWRAP:
		{
			GetTextDocument().SPI_SetWrapMode(kWrapMode_NoWrap,0);
			break;
		}
	  case tc_CHANGETO_WORDWRAP:
		{
			GetTextDocument().SPI_SetWrapMode(kWrapMode_WordWrap,0);
			break;
		}
	  case tc_CHANGETO_LETTERWRAP:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowWrapLetterCountWindow(kWrapMode_LetterWrapByLetterCount);//#1113
			}
			break;
		}
	  case tc_CHANGETAB:
		{
			/*#844
			AText	text;
			ToolCommand_ReadArg1(inText,ioPos,text);
			GetTextDocument().SPI_SetTabWidth(text.GetNumber());
			*/
			break;
		}
	  case tc_CHANGEINDENT://#251
		{
			/*#844
			AText	text;
			ToolCommand_ReadArg1(inText,ioPos,text);
			GetTextDocument().SPI_SetIndentWidth(text.GetNumber());
			*/
			break;
		}
#if IMPLEMENTATION_FOR_MACOSX
	  case tc_CHANGECREATOR:
		{
			AText	text;
			ToolCommand_ReadArg1(inText,ioPos,text);
			GetTextDocument().SPI_SetFileCreator(text.GetOSTypeFromText());//AUtil::GetOSTypeFromAText(text));
			break;
		}
	  case tc_CHANGETYPE:
		{
			AText	text;
			ToolCommand_ReadArg1(inText,ioPos,text);
			GetTextDocument().SPI_SetFileType(text.GetOSTypeFromText());//AUtil::GetOSTypeFromAText(text));
			break;
		}
#endif
	  default:
		{
			//処理なし
			break;
		}
	}
}

//
void	AView_Text::InsertToolText_Command_EDIT( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, const AToolCommandID inToolCommandID, AText& outText )
{
	switch(inToolCommandID)
	{
	  case tc_INDENT:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			DoIndent(/*#650 false,*/undoTag_NOP,inputSpaces,kIndentTriggerType_IndentKey);//#249 #639 #650
			break;
		}
	  case tc_INDENT_BYSPACE://#249
		{
			DoIndent(/*#650 false,*/undoTag_NOP,true,kIndentTriggerType_IndentKey);//#639 #650
			break;
		}
	  case tc_AUTOTAB:
		{
			//B0381 IndentTab(true);
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			AutoTab(inputSpaces);//#249
			break;
		}
	  case tc_SHIFT_LEFT:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(false,inputSpaces);//#249
			break;
		}
	  case tc_SHIFT_RIGHT:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(true,inputSpaces);//#249
			break;
		}
	  case tc_ABBREV_PREV:
		{
			InputAbbrev(false);
			break;
		}
	  case tc_ABBREV_NEXT:
		{
			InputAbbrev(true);
			break;
		}
	  case tc_DELETE_NEXT:
		{
			DeleteNextChar();
			break;
		}
	  case tc_DELETE_PREV:
		{
			DeletePreviousChar();
			break;
		}
	  case tc_DELETE_SELECTED:
		{
			if( IsCaretMode() == true )   break;
			DeletePreviousChar();
			break;
		}
	  case tc_DELETE_ENDOFLINE:
		{
			DeleteToLineEnd();
			break;
		}
	  case tc_DELETE_ENDOFPARAGRAPH://R0213
		{
			DeleteToParagraphEnd();
			break;
		}
	  case tc_DELETE_WHOLE_PARAGRAPH://#124
		{
			DeleteCurrentParagraph();
			break;
		}
	  case tc_DELETE_STARTOFLINE://#913
		{
			DeleteToLineStart();
			break;
		}
	  case tc_DELETE_STARTOFPARAGRAPH://#913
		{
			DeleteToParagraphStart();
			break;
		}
	  case tc_DELETE_MARK://#913
		{
			DeleteToMark();
			break;
		}
	  case tc_DELETE_PREV_DECOMP://#913
		{
			DeletePreviousChar(false);
			break;
		}
	  case tc_PASTE:
		{
			Paste();
			break;
		}
	  case tc_UNDO:
		{
			UndoRedo(true);
			ioPos = inText.GetItemCount();//#252 Undo実行後にツールコマンド実行・文字入力するとredo出来なくなってしまうので、ここで終わらせる
			break;
		}
	  case tc_REDO:
		{
			UndoRedo(false);
			ioPos = inText.GetItemCount();//#252
			break;
		}
	  case tc_CUT:
		{
			Cut(false);
			break;
		}
	  case tc_COPY:
		{
			Copy(false);
			break;
		}
	  case tc_ADDITIONAL_CUT://R0213
		{
			Cut(true);
			break;
		}
	  case tc_ADDITIONAL_COPY://R0213
		{
			Copy(true);
			break;
		}
	  case tc_CUT_ENDOFLINE:
		{
			CutToLineEnd();
			break;
		}
	  case tc_CUT_STARTOFLINE:
		{
			CutToLineStart();
			break;
		}
	  case tc_CUT_ENDOFPARAGRAPH://R0213
		{
			CutToParagraphEnd(false);
			break;
		}
	  case tc_ADDITIONAL_CUT_ENDOFPARAGRAPH://R0213
		{
			CutToParagraphEnd(true);
			break;
		}
	  case tc_SELECTMODE:
		{
			mForceSelectMode = !mForceSelectMode;
			break;
		}
	  case tc_SELECT_BRACE:
		{
			SelectBrace();
			break;
		}
	  case tc_RETURN:
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			outText.AddText(text);
			break;
		}
	  case tc_INDENT_RETURN:
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			outText.AddText(text);
			mToolData_IndentFlag = true;
			mToolData_IndentFlag_InputSpaces = false;//#249
			mToolData_IndentFlag_OnlyNewLine = false;//#249
			break;
		}
	  case tc_INDENT_BYSPACE_RETURN://#249
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			outText.AddText(text);
			mToolData_IndentFlag = true;
			mToolData_IndentFlag_InputSpaces = true;
			mToolData_IndentFlag_OnlyNewLine = false;
			break;
		}
	  case tc_INDENT_ONLYNEWLINE_RETURN://#249
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			outText.AddText(text);
			mToolData_IndentFlag = true;
			mToolData_IndentFlag_InputSpaces = false;
			mToolData_IndentFlag_OnlyNewLine = true;
			break;
		}
	  case tc_INDENT_ONLYNEWLINE_BYSPACE_RETURN://#249
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			outText.AddText(text);
			mToolData_IndentFlag = true;
			mToolData_IndentFlag_InputSpaces = true;
			mToolData_IndentFlag_OnlyNewLine = true;
			break;
		}
	  case tc_TAB:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			InputTab(inputSpaces);//#249
			break;
		}
	  case tc_TAB_BYSPACE://#249
		{
			InputTab(true);
			break;
		}
	  case tc_INDENT_TAB:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			IndentTab(inputSpaces,inputSpaces);//#249
			break;
		}
	  case tc_INDENT_BYSPACE_TAB://#249
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);
			IndentTab(true,inputSpaces);
			break;
		}
	  case tc_INDENT_BYSPACE_TAB_BYSPACE://#249
		{
			IndentTab(true,true);
			break;
		}
	  case tc_SPACE:
		{
			AText	text;
			text.Add(kUChar_Space);
			outText.AddText(text);
			break;
		}
	  case tc_CORRESPONDENCE:
		{
			InputCorrespondence();
			break;
		}
	  case tc_RECENTER:
		{
			Recenter();
			break;
		}
	  case tc_SAMELETTER_UP:
		{
			SameLetterUp();
			break;
		}
	  case tc_DUPLICATE_LINE:
		{
			DuplicateLine();
			break;
		}
	  case tc_INSERT_LINE:
		{
			InsertEmptyLine();
			break;
		}
	  case tc_GT3://B0012
		{
			AText	text;
			text.AddCstring(">>>");
			outText.AddText(text);
			break;
		}
	  case tc_LT3://B0012
		{
			AText	text;
			text.AddCstring("<<<");
			outText.AddText(text);
			break;
		}
	  case tc_GT1:
		{
			AText	text;
			text.AddCstring(">");
			outText.AddText(text);
			break;
		}
	  case tc_LT1:
		{
			AText	text;
			text.AddCstring("<");
			outText.AddText(text);
			break;
		}
	  case tc_SWAPLINE:
		{
			AText	text;
			GetSelectedText(text);
			for( AIndex pos = 0; pos < text.GetItemCount(); )
			{
				AText	line1, line2;
				text.GetLine(pos,line1);
				text.GetLine(pos,line2);
				outText.AddText(line2);
				outText.Add(kUChar_LineEnd);
				outText.AddText(line1);
				outText.Add(kUChar_LineEnd);
			}
			break;
		}
	  case tc_SWAPLETTER://#1154
		{
			SwapLetter();
			break;
		}
	  case tc_COPY_WITHHTMLCOLOR://R0015
		{
			CopyWithHTMLColor();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//
void	AView_Text::InsertToolText_Command_FILE( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, const AToolCommandID inToolCommandID, AText& outText )
{
	switch(inToolCommandID)
	{
	  case tc_SAVE:
		{
			if( GetTextDocument().NVI_IsDirty() == true )
			{
				GetTextDocument().SPI_Save(false);
			}
			break;
		}
	  case tc_SAVEAS:
		{
			GetTextDocument().SPI_Save(true);
			break;
		}
	  case tc_NEWDOCUMENT:
		{
			GetApp().SPNVI_CreateNewTextDocument(kStandardModeIndex);
			break;
		}
	  case tc_NEWDOCUMENT_MOVETARGET://#56
		{
			//textに続きをコピーして、こちらのViewでの実行は終了する
			AText	text;
			inText.GetText(ioPos,inText.GetItemCount()-ioPos,text);
			text.InsertCstring(0,"<<<");
			ioPos = inText.GetItemCount();
			//新規ドキュメントを開いて、そちらで続きを続行
			ADocumentID	docID = GetApp().SPNVI_CreateNewTextDocument(kStandardModeIndex);
			if( docID == kObjectID_Invalid )   return;//win
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			GetApp().SPI_GetTextWindowByID(winID).SPI_DoTool_Text(docID,text,false);
			break;
		}
	  case tc_NEWDOCUMENT_SAMEMODE://#56
		{
			GetApp().SPNVI_CreateNewTextDocument(GetTextDocumentConst().SPI_GetModeIndex());
			break;
		}
	  case tc_NEWDOCUMENT_SAMEMODE_MOVETARGET://#56
		{
			//textに続きをコピーして、こちらのViewでの実行は終了する
			AText	text;
			inText.GetText(ioPos,inText.GetItemCount()-ioPos,text);
			text.InsertCstring(0,"<<<");
			ioPos = inText.GetItemCount();
			//新規ドキュメントを開いて、そちらで続きを続行
			ADocumentID	docID = GetApp().SPNVI_CreateNewTextDocument(GetTextDocumentConst().SPI_GetModeIndex());
			if( docID == kObjectID_Invalid )   return;//win
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			GetApp().SPI_GetTextWindowByID(winID).SPI_DoTool_Text(docID,text,false);
			break;
		}
	  case tc_OPEN_DIALOG:
		{
			GetApp().NVI_ShowFileOpenWindow(false);
			break;
		}
	  case tc_CLOSE:
		{
			mToolData_CloseFlag = true;
			break;
		}
	  case tc_OPEN_URL:
		{
			AText	text;
			ToolCommand_ReadFindArg1(inText,ioPos,text);
			ALaunchWrapper::OpenURL(text);
			break;
		}
	  case tc_OPEN_SELECTED://B0009
		{
			OpenSelected(0);
			break;
		}
	  case tc_OPEN_FILE:
		{
			AText	text;
			ToolCommand_ReadFindArg1(inText,ioPos,text);
			AFileAcc	file;
			file.SpecifyWithAnyFilePathType(text);//B0389 OK
			GetApp().SPNVI_CreateDocumentFromLocalFile(file);
			break;
		}
	  case tc_FILENAME:
		{
			AText	text;
			GetTextDocument().SPI_GetFilename(text);
			outText.AddText(text);
			break;
		}
	  case tc_FILENAME_NOSUFFIX:
		{
			AText	text;
			GetTextDocument().SPI_GetFilename(text);
			AText	suffix;
			text.GetSuffix(suffix);
			if( suffix.GetItemCount() > 0 )
			{
				text.DeleteAfter(text.GetItemCount()-suffix.GetItemCount());
			}
			outText.AddText(text);
			break;
		}
	  case tc_FULL_PATH: 
		{
			AText	text;
			if( GetTextDocument().SPI_IsRemoteFileMode() == true )
			{
				GetTextDocument().SPI_GetRemoteFileURL(text);
			}
			else if( GetTextDocument().NVI_IsFileSpecified() == false )
			{
				GetTextDocument().SPI_GetTemporaryTitle(text);
			}
			else
			{
				AFileAcc	file;
				GetTextDocument().NVI_GetFile(file);
				//B0389 OK 検討必要file.GetPath(text,kFilePathType_1);
				GetApp().GetAppPref().GetFilePathForDisplay(file,text);
			}
			outText.AddText(text);
			break;
		}
	  case tc_DIRECTORY:
		{
			AFileAcc	file;
			GetTextDocument().NVI_GetFile(file);
			AFileAcc	parent;
			parent.SpecifyParent(file);
			AText	text;
			//B0389 OK parent.GetPath(text,kFilePathType_1);
			GetApp().GetAppPref().GetFilePathForDisplay(parent,text);
			outText.AddText(text);
			break;
		}
	  case tc_TOOL:
		{
			AText	path;
			for( ioPos++ ; ioPos < inText.GetItemCount(); ioPos++ )
			{
				AUChar	ch = inText.Get(ioPos);
				if( ch == ')' )
				{
					break;
				}
				path.Add(ch);
			}
			ioPos++;
			
			if( path.CompareMin("tool") == true )
			{
				path.Delete(0,5);
				AIndex	pathpos = 0;
				for( ; pathpos < path.GetItemCount(); pathpos++ )
				{
					if( path.Get(pathpos) == '/' )   break;
				}
				if( pathpos < path.GetItemCount() )
				{
					path.InsertCstring(pathpos,"/tool");
				}
			}
			
			AFileAcc	modeRootFolder;
			GetApp().SPI_GetModeRootFolder(modeRootFolder);
			
			AFileAcc	toolfile;
			toolfile.SpecifyChild(modeRootFolder,path);
			toolfile.ResolveAlias();
			
#if IMPLEMENTATION_FOR_MACOSX
			AText	tooltext;
			GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_Tool,toolfile,tooltext);
			OSType	creator, type;
			toolfile.GetCreatorType(creator,type);
			ABool	unixshell = false;//B0189
			if( type == 'TEXT' )//★タイプ使用しているがNG
			{
				//B0024 TOOL()でシェルスクリプト起動できるようにする
				if( tooltext.Get(0) == '#' && tooltext.Get(1) == '!' )
				{
					unixshell = true;
				}
				if( unixshell == true )
				{
					DoTool_ShellScript(toolfile,tooltext,false);//#862
				}
				else
				{
					if( inDepth < 10 )
					{
						InsertToolText(tooltext,inDepth+1);
					}
				}
			}
			else
			{
				if( inDepth /*#230== 1*/ < 10 )//B0405
				{
					SPI_DoTool(toolfile,0,false,inDepth+1);//B0405
				}
			}
#endif
			break;
		}
	  case tc_KEYTOOL:
		{
			StartKeyToolMode();
			for( ioPos++ ; ioPos < inText.GetItemCount(); ioPos++ )
			{
				AUChar	ch = inText.Get(ioPos);
				if( ch == ')' )
				{
					break;
				}
				AText	t;
				t.Add(ch);
				KeyTool(t);
			}
			break;
		}
	  case tc_BROWSE:
		{
			if( GetTextDocumentConst().NVI_IsDirty() == true && mToolData_DontShowDropWarning == false )
			{
				mToolData_DropWarningFlag = true;
				return;
			}
			AText	text;
			ToolCommand_ReadArg1(inText,ioPos,text);
			if( text.GetItemCount() > 4 )
			{
				if( text.Get(0) == '-' && text.Get(1) == '-' && text.Get(2) == '-' && text.Get(3) == '-' )
				{
					text.Delete(0,4);
				}
			}
			if( GetTextDocumentConst().SPI_IsRemoteFileMode() == false )
			{
#if IMPLEMENTATION_FOR_MACOSX
				//#194
				if( GetTextDocumentConst().SPI_IsODBMode() == true )
				{
					AText	customPath;
					GetTextDocumentConst().SPI_GetODBCustomPath(customPath);
					if( customPath.GetItemCount() > 0 )
					{
						AText	httpurl;
						GetApp().GetAppPref().GetHTTPURLFromFTPURL(customPath,httpurl);
						AAppAcc	appAcc;
						appAcc.SetFromToolCommandText(text);
						ALaunchWrapper::OpenURLWithSpecifiedApp(appAcc,httpurl);
						break;
					}
				}
#endif				
				AAppAcc	appAcc;//win(NULL,NULL);
				appAcc.SetFromToolCommandText(text);
				AFileAcc	file;
				GetTextDocumentConst().NVI_GetFile(file);
				
				//R0212
				//file://のURLOpenで開けばSafari上で現在表示中のスクロール位置で更新表示されるので不要となってしまった
				/*AText	anchor;
				ATextPoint	spt,ept;
				SPI_GetSelect(spt,ept);
				GetTextDocumentConst().SPI_GetJumpMenuItemInfoTextByLineIndex(spt.y,anchor);
				if( anchor.GetItemCount() == 0 )
				{
					ALaunchWrapper::Drop(appAcc,file);
				}
				else
				{
					AText	url;
					file.GetPath(url);
					url.InsertCstring(0,"file://");
					AText	text;
					text.SetCstring("%20");
					url.ReplaceChar(' ',text);
					url.Add('#');
					url.AddText(anchor);
					ALaunchWrapper::OpenURL(url);
				}*/
				//ALaunchWrapper::OpenURL(file);B0000 これではアプリ指定の意味がない
				ALaunchWrapper::Drop(appAcc,file);
			}
			else
			{
				AText	ftpurl, httpurl;
				GetTextDocumentConst().SPI_GetRemoteFileURL(ftpurl);
				GetApp().GetAppPref().GetHTTPURLFromFTPURL(ftpurl,httpurl);
				//#214 ALaunchWrapper::OpenURL(httpurl);
				AAppAcc	appAcc;
				appAcc.SetFromToolCommandText(text);
				ALaunchWrapper::OpenURLWithSpecifiedApp(appAcc,httpurl);//#214
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

//
void	AView_Text::InsertToolText_Command_LAUNCH( const AText& inText, ATextIndex& ioPos, const ANumber inDepth, const AToolCommandID inToolCommandID, AText& outText )
{
	ABool	isDrop = (inToolCommandID==tc_DROP||inToolCommandID==tc_CLOSEDROP||inToolCommandID==tc_CLOSE_AND_DROP);
	if( isDrop == true && GetTextDocumentConst().NVI_IsDirty() == true && mToolData_DontShowDropWarning == false )//#308
	{
		mToolData_DropWarningFlag = true;
		return;
	}
	ABool	isSend = (inToolCommandID==tc_SENDSELECTED||inToolCommandID==tc_SEND_SELECTED);
	ABool	isSendReplace = (inToolCommandID==tc_SENDSELECTEDR||inToolCommandID==tc_SEND_SELECTED_REPLACE);
	ABool	isSendNew = (inToolCommandID==tc_SENDSELECTEDN||inToolCommandID==tc_SEND_SELECTED_NEWDOCUMENT);
	if( isSend == true || isSendReplace == true || isSendNew == true )
	{
		AText	text;
		ToolCommand_ReadArg1(inText,ioPos,text);
#if IMPLEMENTATION_FOR_MACOSX
		if( text.GetItemCount() == 12 )
		{
			OSType	creator, eventclass, eventid;
			AText	t;
			text.GetText(0,4,t);
			creator = t.GetOSTypeFromText();//AUtil::GetOSTypeFromAText(t);
			text.GetText(4,4,t);
			eventclass = t.GetOSTypeFromText();//AUtil::GetOSTypeFromAText(t);
			text.GetText(8,4,t);
			eventid = t.GetOSTypeFromText();//AUtil::GetOSTypeFromAText(t);
			//
			AAppAcc	app('APPL',creator);
			if( isSend == true )
			{
				ALaunchWrapper::Launch(app);
				ALaunchWrapper::SendAppleEvent_Text(app,eventclass,eventid,mToolData_SelectedText);
			}
			else if( isSendReplace == true || isSendNew == true )
			{
				AText	newtext;
				ALaunchWrapper::Launch(app);
				ALaunchWrapper::SendAppleEvent_Text(app,eventclass,eventid,mToolData_SelectedText,newtext);
				if( isSendReplace == true )
				{
					outText.AddText(newtext);
				}
				else
				{
					ADocumentID docID = GetApp().SPNVI_CreateNewTextDocument(GetTextDocumentConst().SPI_GetModeIndex());
					if( docID == kObjectID_Invalid )   return;//win
					GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,newtext);
				}
			}
		}
#endif
	}
	else
	{
		AFileAcc	file;
		GetTextDocumentConst().NVI_GetFile(file);
		AText	apptext;
		ToolCommand_ReadArg1(inText,ioPos,apptext);
		if( apptext.GetItemCount() > 4 )
		{
			if( apptext.Get(0) == '-' && apptext.Get(1) == '-' && apptext.Get(2) == '-' && apptext.Get(3) == '-' )
			{
				apptext.Delete(0,4);
			}
		}
		//
		AText	suffix;
		AAppAcc	appAcc;//win(NULL,NULL);
		if( appAcc.SetFromToolCommandText(apptext) == false )
		{
			if( apptext.GetItemCount() == 7 )
			{
				apptext.GetText(4,3,suffix);
				apptext.DeleteAfter(4);
				appAcc.SetFromToolCommandText(apptext);
			}
		}
		//
		if( ioPos < inText.GetItemCount() )
		{
			if( inText.Get(ioPos) == '(' )
			{
				ToolCommand_ReadArg1(inText,ioPos,suffix);
			}
		}
		//
		if( suffix.GetItemCount() > 0 )
			{
				suffix.Insert1(0,'.');
				AFileAcc	f;
				GetTextDocumentConst().NVI_GetFile(f);
				file.SpecifyChangeSuffix(f,suffix);
			}
		//
		if( inToolCommandID == tc_CLOSEDROP || inToolCommandID == tc_CLOSE_AND_DROP )
		{
			if( inDepth == 1 && GetTextDocumentConst().NVI_IsFileSpecified() == true )
			{
				if( GetTextDocument().NVI_IsDirty() == true )
				{
					GetTextDocument().SPI_Save(false);
				}
				AText	text;
				inText.GetText(ioPos,inText.GetItemCount()-ioPos,text);
				text.InsertCstring(0,"<<<");
				GetApp().SPI_Register_ToolCommandWaitOpen(file,text);
				ioPos = inText.GetItemCount();
				mToolData_CloseFlag = true;
			}
		}
		//
		if( isDrop == true )
		{
			ALaunchWrapper::Drop(appAcc,file);
		}
		else
		{
			ALaunchWrapper::Launch(appAcc);
		}
	}
}

//
void	AView_Text::ToolCommand_ReadArg1( const AText& inText, ATextIndex& ioPos, AText& outArg )
{
	outArg.DeleteAll();
	ioPos++;
	ANumber	level = 1;//#451
	for( ; ioPos < inText.GetItemCount(); ioPos++ ) 
	{
		AUChar ch = inText.Get(ioPos);
		if( ch == ')' ) 
		{
			level--;//#451
			if( level == 0 )//#451
			{
				ioPos++;
				return;
			}
		}
		//#451
		else if( ch == '(' )
		{
			level++;
		}
		//
		if( ch == '\\' && ioPos+1 < inText.GetItemCount() )
		{
			ioPos++;
			ch = inText.Get(ioPos);
		}
		outArg.Add(ch);
	}
}

//
void	AView_Text::ToolCommand_ReadFindArg1( const AText& inText, ATextIndex& ioPos, AText& outArg )
{
	outArg.DeleteAll();
	for( ; ioPos < inText.GetItemCount(); ioPos++ ) 
	{
		AUChar ch = inText.Get(ioPos);
		if( ch == kUChar_LineEnd )
		{
			ioPos++;
			break;
		}
	}
	for( ; ioPos < inText.GetItemCount(); ioPos++ ) 
	{
		AUChar ch = inText.Get(ioPos);
		if( ch == kUChar_LineEnd )
		{
			ioPos++;
			break;
		}
		outArg.Add(ch);
	}
}

void	AView_Text::ToolCommand_ReadFindArg2( const AText& inText, ATextIndex& ioPos, AText& outArg1, AText& outArg2 )
{
	outArg1.DeleteAll();
	outArg2.DeleteAll();
	for( ; ioPos < inText.GetItemCount(); ioPos++ ) 
	{
		AUChar ch = inText.Get(ioPos);
		if( ch == kUChar_LineEnd )
		{
			ioPos++;
			break;
		}
	}
	for( ; ioPos < inText.GetItemCount(); ioPos++ ) 
	{
		AUChar ch = inText.Get(ioPos);
		if( ch == kUChar_LineEnd )
		{
			ioPos++;
			break;
		}
		outArg1.Add(ch);
	}
	for( ; ioPos < inText.GetItemCount(); ioPos++ ) 
	{
		AUChar ch = inText.Get(ioPos);
		if( ch == kUChar_LineEnd )
		{
			ioPos++;
			break;
		}
		outArg2.Add(ch);
	}
}

//#273
/**
""等で囲まれたテキストを取得
*/
void	AView_Text::ToolCommand_ReadQuotedText( const AText& inText, ATextIndex& ioPos, AText& outText, const AUChar inTerminateChar )
{
	outText.DeleteAll();
	for( ; ioPos < inText.GetItemCount(); ioPos++ )
	{
		AUChar	ch = inText.Get(ioPos);
		if( ch == inTerminateChar )
		{
			ioPos++;
			break;
		}
		if( ch == '\\' )
		{
			ioPos++;
			ch = inText.Get(ioPos);
			if( ch == 'r' )
			{
				ch = '\r';
			}
		}
		outText.Add(ch);
	}
}

//
ABool	AView_Text::MakeDropText( const AFileAcc& inFile, const AText& inURL, const AText& inTitle, AText& outText, AIndex& outCaretOffset )//#91
{
	outText.DeleteAll();
	outCaretOffset = kIndex_Invalid;
	AText	filename;
	if( inFile.IsSpecified() == true )
	{
		inFile.GetFilename(filename);
	}
	else
	{
		inURL.GetFilename(filename);
	}
	AText	suffix;
	filename.GetSuffix(suffix);
	ANumber	imagewidth = 0, imageheight = 0;
	if( inFile.IsSpecified() == true )
	{
		AFileWrapper::GetImageInfo(inFile,imagewidth,imageheight);
	}
	AIndex	index = GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).Find_TextArray(AModePrefDB::kDragDrop_Suffix,suffix,true);
	if( index == kIndex_Invalid )
	{
		if( inURL.GetItemCount() > 0 )
		{
			suffix.SetCstring(".html");
			index = GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).Find_TextArray(AModePrefDB::kDragDrop_Suffix,suffix,true);
		}
	}
	if( index != kIndex_Invalid )
	{
		AText	droptext;
		GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_TextArray(AModePrefDB::kDragDrop_Text,index,droptext);
		for( AIndex pos = 0; pos < droptext.GetItemCount();  )
		{
			//"<<<"を検索
			AIndex	epos = droptext.GetItemCount();
			ABool found = droptext.FindCstring(pos,"<<<",epos);
			//
			if( epos-pos > 0 )
			{
				AText	text;
				droptext.GetText(pos,epos-pos,text);
				outText.AddText(text);
			}
			if( found == false )
			{
				break;
			}
			else
			{
				//コマンドモード
				pos = epos+3;
				for( ; pos < droptext.GetItemCount();  )
				{
					//Tab, Spaceスキップ
					if( droptext.SkipTabSpace(pos,droptext.GetItemCount()) == false )   break;
					//コマンドモード終了判定
					AUChar	ch = droptext.Get(pos);
					if( ch == '>' && pos+2 < droptext.GetItemCount() )
					{
						if( droptext.Get(pos+1) == '>' && droptext.Get(pos+2) == '>' )
						{
							pos += 3;
							break;
						}
					}
					//トークン読み込み
					AText	token;
					droptext.GetToken(pos,token);
					//ツールコマンド検索
					AToolCommandID	toolCommandID = GetApp().SPI_FindToolCommandID(token);
					switch(toolCommandID)
					{
					  case tc_PATH:
						{
							if( inFile.IsSpecified() == true )
							{
								AFileAcc	file;
								GetTextDocument().NVI_GetFile(file);
								AText	path;
								inFile.GetPartialPath(file,path);//B0389 OK ,kFilePathType_1);
								outText.AddText(path);
							}
							break;
						}
					  case tc_FILENAME:
						{
							outText.AddText(filename);
							break;
						}
					  case tc_FILENAME_NOSUFFIX:
						{
							AText	text;
							text.SetText(filename);
							text.Delete(text.GetItemCount()-suffix.GetItemCount(),suffix.GetItemCount());
							outText.AddText(text);
							break;
						}
					  case tc_DIRECTORY:
						{
							if( inFile.IsSpecified() == true )
							{
								AFileAcc	folder;
								folder.SpecifyParent(inFile);
								AText	name;
								folder.GetFilename(name);
								outText.AddText(name);
							}
							break;
						}
					  case tc_IMAGEWIDTH:
						{
							outText.AddFormattedCstring("%d",imagewidth);
							break;
						}
					  case tc_IMAGEHEIGHT:
						{
							outText.AddFormattedCstring("%d",imageheight);
							break;
						}
						//#1173
					  case tc_IMAGEHALFWIDTH:
						{
							outText.AddFormattedCstring("%d",imagewidth/2);
							break;
						}
					  case tc_IMAGEHALFHEIGHT:
						{
							outText.AddFormattedCstring("%d",imageheight/2);
							break;
						}
						//URL挿入（URLエンコード無し）従来の<<<URL>>>の処理内容をURL-NOURLENCODEに移す
					  case tc_URL_NOURLENCODE://#363
						{
							outText.AddText(inURL);
							break;
						}
						//#363 URLエンコード
					  case tc_URL:
						{
							AText	url(inURL);
							url.ConvertToURLEncode();
							outText.AddText(url);
							break;
						}
					  case tc_TITLE:
						{
							outText.AddText(inTitle);
							break;
						}
					  case tc_CARET:
					  case tc_MOVECARET_HERE:
						{
							outCaretOffset = outText.GetItemCount();
							break;
						}
					  default:
						{
							//処理なし
							break;
						}
					}
				}
			}
		}
		return true;//#91
	}
	else
	{
		inFile.GetPath(outText);//B0389 OK ,kFilePathType_1);
		return false;//#91
	}
}

