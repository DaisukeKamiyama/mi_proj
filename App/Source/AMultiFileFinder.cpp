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

AMultiFileFinder

*/

#include "stdafx.h"

#include "AMultiFileFinder.h"
#include "AApp.h"
//#include "AUtil.h"
#include "ADocument_IndexWindow.h"

#pragma mark ===========================
#pragma mark [クラス]AMultiFileFinder
#pragma mark ===========================
//マルチファイル検索実行

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AMultiFileFinder::AMultiFileFinder( AObjectArrayItem* inParent ) : AThread(inParent), mTotalLineCount(0), mSleepForAWhile(false)
,mHitCount(0),mHitFileCount(0),mAbortRecognizeSyntax(false)//R0238 #828
,mForMultiFileReplace(false)//#65
,mExtractMatchedText(false)//#937
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

void	AMultiFileFinder::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AMultiFileFinder::NVIDO_ThreadMain started.",this);
	try
	{
		//検索実行
		mTotalLineCount = 0;
		mHitCount = 0;//R0238
		mHitFileCount = 0;//#828
		mAbortRecognizeSyntax = false;//#890
		FindInFolder(mRootFolder,0);
		//
		//fprintf(stderr,"Total:%d lines.\n",mTotalLineCount);
	}
	catch(...)
	{
		_ACError("AMultiFileFinder::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	//B0359 ついで
	if( NVM_IsAborted() == true )
	{
		AObjectID	receiverID = GetApp().SPI_MakeNewThreadDataReceiver_MultiFileFindResult();
		GetApp().SPI_GetThreadDataReceiver_MultiFileFindResult(receiverID).SetIndexWindowDocumentID(mIndexWindowDocumentID);
		AText	text;
		text.SetLocalizedText("MultiFileFind_Aborted");
		AText	filepath,prehit,posthit,position;
		AText	paratext;//#465
		GetApp().SPI_GetThreadDataReceiver_MultiFileFindResult(receiverID).
				Add(filepath,text,prehit,posthit,position,
				paratext,//#465
				0,0,-1);
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(receiverID);
		ABase::PostToMainInternalEventQueue(kInternalEvent_MultiFileFindResult,GetObjectID(),0,GetEmptyText(),objectIDArray);
	}
	//R0238
	else
	{
		AObjectID	receiverID = GetApp().SPI_MakeNewThreadDataReceiver_MultiFileFindResult();
		GetApp().SPI_GetThreadDataReceiver_MultiFileFindResult(receiverID).SetIndexWindowDocumentID(mIndexWindowDocumentID);
		AText	text;
		text.SetLocalizedText("MultiFileFind_ResultCount");
		AText	numtext;
		numtext.AddNumber(mHitCount);
		text.ReplaceParamText('0',numtext);
		//#828 ファイル数表示
		numtext.SetNumber(mHitFileCount);
		text.ReplaceParamText('1',numtext);
		//
		AText	filepath,prehit,posthit,position;
		AText	paratext;//#465
		GetApp().SPI_GetThreadDataReceiver_MultiFileFindResult(receiverID).
				Add(filepath,text,prehit,posthit,position,
				paratext,//#465
				0,0,-1);
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(receiverID);
		ABase::PostToMainInternalEventQueue(kInternalEvent_MultiFileFindResult,GetObjectID(),0,GetEmptyText(),objectIDArray);
	}
	//B0313 try内から外へ移動（throwしたときにプログレスウインドウが消えない）
	//プログレスウインドウHide等
	AArray<AObjectID>	objectIDArray;
	objectIDArray.Add(mIndexWindowDocumentID);
	ABase::PostToMainInternalEventQueue(kInternalEvent_MultiFileFind_Completed,GetObjectID(),0,mExtractedText,objectIDArray);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AMultiFileFinder::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---検索

//検索実行（スレッド起動）
void	AMultiFileFinder::SPNVI_Run( const AFindParameter& findParam, const ADocumentID inIndexDocumentID,
									const ABool inForMultiFileReplace, const ABool inExtractText )//#725
{
	//検索パラメータ保存
	mFindParam.Set(findParam);
	//検索テキスト長制限
	mFindParam.findText.LimitLength(0,kLimit_FindTextLength);
	//マルチファイル置換かどうかを記憶
	mForMultiFileReplace = inForMultiFileReplace;
	//抽出するかどうかを記憶、抽出テキスト初期化
	mExtractMatchedText = inExtractText;
	mExtractedText.DeleteAll();
	//検索対象ルートフォルダ記憶
	mRootFolder.Specify(findParam.folderPath);
	if( findParam.filterText.GetItemCount() > 0 && findParam.filterText.Compare(".+") == false )
	{
		mFileFilterExist = true;
		mFileFilterRegExp.SetRE(findParam.filterText,true);
	}
	else
	{
		mFileFilterExist = false;
	}
	
	//
	mSleepForAWhile = false;
	
	//プログレスバー表示（ただしマルチファイル置換の場合はダイアログ内に表示するのでプログレスウインドウ表示しない）
	if( GetApp().SPI_GetMultiFileFindWindow().SPI_ExecutingFindForMultiFileReplace() == false )
	{
		GetApp().SPI_GetMultiFileFindProgress().NVI_Show();
		GetApp().SPI_GetMultiFileFindProgress().SPI_SetProgress(0);
		GetApp().SPI_GetMultiFileFindProgress().SPI_SetProgressText(GetEmptyText());
	}
	
	//結果表示ウインドウ表示
	//#92 AText	wintitle;
	//#92 wintitle.SetLocalizedText("IndexWindow_Title_FindResult");
	AText	grouptitle1, grouptitle2;
	grouptitle1.SetLocalizedText("FindResult_MultiFileFindResult1");
	AText	datetime;
	ADateTimeWrapper::GetDateTimeText(datetime);
	grouptitle1.ReplaceParamText('0',datetime);
	grouptitle2.SetLocalizedText("FindResult_MultiFileFindResult2");
	grouptitle2.ReplaceParamText('0',mFindParam.findText);
	AText	regexp;
	if( mFindParam.regExp == true )
	{
		regexp.SetLocalizedText("FindResult_RegExp");
	}
	grouptitle2.ReplaceParamText('1',regexp);
	grouptitle2.ReplaceParamText('2',findParam.folderPath);
	if( mFileFilterExist == true )
	{
		AText	text;
		text.SetLocalizedText("FindResult_MultiFileFindResult3");
		text.ReplaceParamText('1',findParam.filterText);
		grouptitle2.AddText(text);
	}
	mIndexWindowDocumentID = inIndexDocumentID;//#92 SPNVI_SelectOrCreateIndexWindowDocument(wintitle); #725
	GetApp().SPI_GetIndexWindowDocumentByID(mIndexWindowDocumentID).SPI_InsertGroup(0,grouptitle1,grouptitle2,mRootFolder);//#465
	GetApp().SPI_GetIndexWindowDocumentByID(mIndexWindowDocumentID).SPI_InhibitClose(true);
	
	//ImportFileRecognizerを一時スリープ（同時動作重い）
	GetApp().SPI_GetImportFileRecognizer().NVI_SetSleepFlag(true);
	//スレッド起動
	NVI_Run();
	//マルチファイル検索ウインドウの検索ボタンをDisableにするため
	GetApp().SPI_UpdateMultiFileFindWindow();
}

//フォルダの中を検索（再帰）
ABool	AMultiFileFinder::FindInFolder( const AFileAcc& inFolder, const AIndex inLevel )
{
	AObjectArray<AFileAcc>	children;
	inFolder.GetChildren(children);
	AText	pathtext;
	//B0389 OK inFolder.GetPath(pathtext,kFilePathType_1);
	//GetApp().GetAppPref().GetFilePathForDisplay(inFolder,pathtext);//B0389
	inFolder.GetPath(pathtext);
	
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		if( inLevel == 0 )
		{
			short	percent = i*100/children.GetItemCount();
			ABase::PostToMainInternalEventQueue(kInternalEvent_MultiFileFindProgress_SetProgress,GetObjectID(),percent,GetEmptyText(),GetEmptyObjectIDArray());
		}
		//#827 現在検索中のファイルフォルダを表示する。（#828でinLevel==0の条件文内から外へ移動）
		ABase::PostToMainInternalEventQueue(kInternalEvent_MultiFileFindProgress_SetProgressText,GetObjectID(),0,pathtext,GetEmptyObjectIDArray());
		//
		AFileAcc	child = children.GetObject(i);
		if( child.IsFolder() == true )
		{
			if( child.IsLink() == false )//#0 シンボリックリンク・エイリアスによる無限ループ防止
			{
				//R0170
				if( mFindParam.onlyVisibleFile == true )
				{
					if( child.IsInvisible() == true )
					{
						continue;
					}
				}
				
				if( mFindParam.recursive == true )
				{
					if( FindInFolder(child,inLevel+1) == false )   return false;
				}
			}
		}
		else
		{
			AText	filename;
			child.GetFilename(filename);
			if( mFileFilterExist == true )
			{
				mFileFilterRegExp.InitGroup();//検討必要　グループ初期化のみでOK?
				AIndex	pos = 0;
				if( mFileFilterRegExp.Match(filename,pos,filename.GetItemCount()) == false )
				{
					continue;
				}
			}
			//B0313
			if( mFindParam.onlyTextFile == true )
			{
				//非テキストファイルなら対象外とする
				if( GetApp().GetAppPref().IsBinaryFile(child) == true )
				{
					continue;
				}
			}
			//B0313
			if( mFindParam.onlyVisibleFile == true )
			{
				if( child.IsInvisible() == true )
				{
					continue;
				}
			}
			
			//B0313 try, catch throwはここで捕まえてあとのファイルを実行させる
			try
			{
				if( FindInFile(child) == false )   return false;
			}
			catch(...)
			{
				_ACError("AMultiFileFinder::FindInFolder() caught exception.",NULL);//#199
			}
		}
	}
	return true;
}

const AItemCount	kPreHitTextLength = 100;
const AItemCount	kPostHitTextLength = 100;

//各ファイルを検索
ABool	AMultiFileFinder::FindInFile( const AFileAcc& inFile )
{
	//テキストロード目的取得
	ALoadTextPurposeType	purposeType = kLoadTextPurposeType_MultiFileFinder_Find;
	if( mForMultiFileReplace == true )
	{
		purposeType = kLoadTextPurposeType_MultiFileFinder_Replace;
	}
	//
	AText	text;
	AModeIndex	modeIndex;
	AText	tecname;
	GetApp().SPI_LoadTextFromFileOrDocument(purposeType,inFile,text,modeIndex,tecname);
	
	//findParam取得
	AFindParameter	findParam;
	findParam.Set(mFindParam);
	
	//JIS系テキストエンコーディングのときは、常にバックスラッシュ／￥を無視する。
	ATextEncoding	tec = ATextEncodingWrapper::GetTextEncodingFromName(tecname);
	if( tec == ATextEncodingWrapper::GetSJISTextEncoding() ||
	   tec == ATextEncodingWrapper::GetJISTextEncoding() ||
	   tec == ATextEncodingWrapper::GetEUCTextEncoding()  )
	{
		findParam.ignoreBackslashYen = true;
	}
	
	//文法認識
	ATextInfo	textInfo(NULL);
	textInfo.SetMode(modeIndex);
	ABool	abortFlag = false;
	textInfo.CalcLineInfoAllWithoutView(text,abortFlag);//win
	ABool	syntaxRecognized = false;//#730 高速化。検索結果があるときだけ文法認識する
	mTotalLineCount += textInfo.GetLineCount();
	
	//結果受信オブジェクト生成
	AObjectID	receiverID = GetApp().SPI_MakeNewThreadDataReceiver_MultiFileFindResult();
	GetApp().SPI_GetThreadDataReceiver_MultiFileFindResult(receiverID).SetIndexWindowDocumentID(mIndexWindowDocumentID);
	
	AText	returntext;
	returntext.SetLocalizedText("ReturnCodeText");
	ATextIndex	spos = 0, epos = text.GetItemCount();
	AText	filepath;
	AText	hit, prehit, posthit;
	ABool	found = false;
	while(true)
	{
		//中断判定
		if( NVM_IsAborted() == true )   return false;
		
		//しばらく休憩（メインスレッドに働いてもらう）
		if( mSleepForAWhile == true )
		{
			NVM_SleepWithTimer(2);
			mSleepForAWhile = false;
		}
		
		//検索実行
		ATextIndex	resultspos, resultepos;
		ABool	modalSessionAborted = false;//#695
		if( mTextFinder.ExecuteFind(text,findParam,false,spos,epos,resultspos,resultepos,mAborted,false,modalSessionAborted) == false )   break;//B0359
		
		//一致箇所抽出
		if( mExtractMatchedText == true )
		{
			AText	t;
			text.GetText(resultspos,resultepos-resultspos,t);
			mExtractedText.AddText(t);
		}
		
		//検索結果が１つ以上あるときのみ、最初の一回文法認識するようにする #730
		if( syntaxRecognized == false && mAbortRecognizeSyntax == false )//#890
		{
			if( findParam.displayPosition == true && text.SuspectBinaryData() == false )//#898 utf8ok == true && text.GetItemCount() < 1024*1024 )//B0313 1MB以上もしくはUTF8テキストとしてまっとうでないときはバイナリファイルの可能性が高いのでRecognizeSyntax()しない #395
			{
				//#695 AIndex	startLineIndex, endLineIndex;
				ABool	importChanged = false;
				AArray<AUniqID>	addedIdentifier;
				AArray<AIndex>		addedIdentifierLineIndex;
				//#695 AArray<AUniqID>	deletedIdentifier;//win
				AText	path;//#998
				inFile.GetPath(path);//#998
				textInfo.RecognizeSyntaxAll(text,path,//#695 startLineIndex,endLineIndex,deletedIdentifier, #998
							addedIdentifier,addedIdentifierLineIndex,importChanged,mAbortRecognizeSyntax);//R0000 #698 #890
				if( NVM_IsAborted() == true )   return false;
			}
			syntaxRecognized = true;
		}
		
		//結果データ作成
		inFile.GetPath(filepath);
		text.GetText(resultspos,resultepos-resultspos,hit);
		
		//周辺テキスト取得
		ATextIndex	pos = resultspos - kPreHitTextLength;
		if( pos < 0 )   pos = 0;
		pos = text.GetCurrentCharPos(pos);
		text.GetText(pos,resultspos-pos,prehit);
		pos = resultepos + kPostHitTextLength;
		if( pos >= text.GetItemCount() )   pos = text.GetItemCount();
		pos = text.GetCurrentCharPos(pos);
		text.GetText(resultepos,pos-resultepos,posthit);
		hit.ReplaceChar(kUChar_CR,returntext);
		prehit.ReplaceChar(kUChar_CR,returntext);
		posthit.ReplaceChar(kUChar_CR,returntext);
		
		//段落Index等取得
		ATextPoint	textpoint;
		textInfo.GetTextPointFromTextIndex(resultspos,textpoint);
		AIndex	paragraphIndex = textInfo.GetParagraphIndexByLineIndex(textpoint.y);
		/*
		//#911 前段落と、次段落までを周辺テキストとして取得することにする→やっぱりやめた。マルチファイル検索結果は従来通りの表示とする。
		ATextPoint	spt = {0,textpoint.y-1};
		if( spt.y < 0 )   spt.y = 0;
		ATextIndex	sp = textInfo.GetTextIndexFromTextPoint(spt);
		ATextPoint	ept = {0,textpoint.y+2};
		if( ept.y >= textInfo.GetLineCount() )   ept.y = textInfo.GetLineCount()-1;
		ATextIndex	ep = textInfo.GetTextIndexFromTextPoint(ept);
		text.GetText(sp,resultspos-sp,prehit);
		text.GetText(ep,ep-resultepos,posthit);
		*/
		//場所情報取得
		AText	position;//#395
		if( findParam.displayPosition == true )//#395
		{
			AUniqID	identifierID = textInfo.GetMenuIdentifierByLineIndex(textpoint.y);
			if( identifierID != kUniqID_Invalid )
			{
				textInfo.GetGlobalIdentifierMenuText(identifierID,position);
			}
		}
		//#465
		ATextIndex	parastart = textInfo.GetParagraphStartTextIndex(paragraphIndex);
		ATextIndex	paraend = parastart;
		for( ; paraend < text.GetItemCount(); paraend++ )
		{
			if( text.Get(paraend) == kUChar_LineEnd )
			{
				paraend++;
				break;
			}
		}
		AText	paratext;
		text.GetText(parastart,paraend-parastart,paratext);
		//結果データ格納
		GetApp().SPI_GetThreadDataReceiver_MultiFileFindResult(receiverID).
				Add(filepath,hit,prehit,posthit,position,
				paratext,//#465
				resultspos,resultepos-resultspos,paragraphIndex);
		found = true;
		mHitCount++;//R0238
		
		if( spos == resultepos )
		{
			spos = text.GetNextCharPos(spos);
		}
		else
		{
			spos = resultepos;
		}
	}
	
	//結果をメインスレッドへ送信
	if( found == true )
	{
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(receiverID);
		ABase::PostToMainInternalEventQueue(kInternalEvent_MultiFileFindResult,GetObjectID(),0,tecname,objectIDArray);
		
		//#828 ファイル数+1
		mHitFileCount++;
	}
	else
	{
		//見つからなければ結果送信しない
		GetApp().SPI_Delete1ThreadDataReceiver_MultiFileFindResult(receiverID);
	}
	return true;
}

#pragma mark ===========================

#pragma mark ---中断

//#890
/**
検索途中中断
*/
void	AMultiFileFinder::NVIDO_AbortThread()
{
	SPI_AbortRecognizeSyntax();
}

void	AMultiFileFinder::SPI_SleepForAWhile()
{
	mSleepForAWhile = true;
}

