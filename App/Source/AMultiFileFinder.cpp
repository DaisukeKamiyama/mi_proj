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
#pragma mark [�N���X]AMultiFileFinder
#pragma mark ===========================
//�}���`�t�@�C���������s

#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
//�R���X�g���N�^
AMultiFileFinder::AMultiFileFinder( AObjectArrayItem* inParent ) : AThread(inParent), mTotalLineCount(0), mSleepForAWhile(false)
,mHitCount(0),mHitFileCount(0),mAbortRecognizeSyntax(false)//R0238 #828
,mForMultiFileReplace(false)//#65
,mExtractMatchedText(false)//#937
{
}

#pragma mark ===========================

#pragma mark ---�X���b�h���C�����[�`��

void	AMultiFileFinder::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AMultiFileFinder::NVIDO_ThreadMain started.",this);
	try
	{
		//�������s
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
	//B0359 ����
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
		//#828 �t�@�C�����\��
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
	//B0313 try������O�ֈړ��ithrow�����Ƃ��Ƀv���O���X�E�C���h�E�������Ȃ��j
	//�v���O���X�E�C���h�EHide��
	AArray<AObjectID>	objectIDArray;
	objectIDArray.Add(mIndexWindowDocumentID);
	ABase::PostToMainInternalEventQueue(kInternalEvent_MultiFileFind_Completed,GetObjectID(),0,mExtractedText,objectIDArray);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AMultiFileFinder::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---����

//�������s�i�X���b�h�N���j
void	AMultiFileFinder::SPNVI_Run( const AFindParameter& findParam, const ADocumentID inIndexDocumentID,
									const ABool inForMultiFileReplace, const ABool inExtractText )//#725
{
	//�����p�����[�^�ۑ�
	mFindParam.Set(findParam);
	//�����e�L�X�g������
	mFindParam.findText.LimitLength(0,kLimit_FindTextLength);
	//�}���`�t�@�C���u�����ǂ������L��
	mForMultiFileReplace = inForMultiFileReplace;
	//���o���邩�ǂ������L���A���o�e�L�X�g������
	mExtractMatchedText = inExtractText;
	mExtractedText.DeleteAll();
	//�����Ώۃ��[�g�t�H���_�L��
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
	
	//�v���O���X�o�[�\���i�������}���`�t�@�C���u���̏ꍇ�̓_�C�A���O���ɕ\������̂Ńv���O���X�E�C���h�E�\�����Ȃ��j
	if( GetApp().SPI_GetMultiFileFindWindow().SPI_ExecutingFindForMultiFileReplace() == false )
	{
		GetApp().SPI_GetMultiFileFindProgress().NVI_Show();
		GetApp().SPI_GetMultiFileFindProgress().SPI_SetProgress(0);
		GetApp().SPI_GetMultiFileFindProgress().SPI_SetProgressText(GetEmptyText());
	}
	
	//���ʕ\���E�C���h�E�\��
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
	
	//ImportFileRecognizer���ꎞ�X���[�v�i��������d���j
	GetApp().SPI_GetImportFileRecognizer().NVI_SetSleepFlag(true);
	//�X���b�h�N��
	NVI_Run();
	//�}���`�t�@�C�������E�C���h�E�̌����{�^����Disable�ɂ��邽��
	GetApp().SPI_UpdateMultiFileFindWindow();
}

//�t�H���_�̒��������i�ċA�j
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
		//#827 ���݌������̃t�@�C���t�H���_��\������B�i#828��inLevel==0�̏�����������O�ֈړ��j
		ABase::PostToMainInternalEventQueue(kInternalEvent_MultiFileFindProgress_SetProgressText,GetObjectID(),0,pathtext,GetEmptyObjectIDArray());
		//
		AFileAcc	child = children.GetObject(i);
		if( child.IsFolder() == true )
		{
			if( child.IsLink() == false )//#0 �V���{���b�N�����N�E�G�C���A�X�ɂ�閳�����[�v�h�~
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
				mFileFilterRegExp.InitGroup();//�����K�v�@�O���[�v�������݂̂�OK?
				AIndex	pos = 0;
				if( mFileFilterRegExp.Match(filename,pos,filename.GetItemCount()) == false )
				{
					continue;
				}
			}
			//B0313
			if( mFindParam.onlyTextFile == true )
			{
				//��e�L�X�g�t�@�C���Ȃ�ΏۊO�Ƃ���
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
			
			//B0313 try, catch throw�͂����ŕ߂܂��Ă��Ƃ̃t�@�C�������s������
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

//�e�t�@�C��������
ABool	AMultiFileFinder::FindInFile( const AFileAcc& inFile )
{
	//�e�L�X�g���[�h�ړI�擾
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
	
	//findParam�擾
	AFindParameter	findParam;
	findParam.Set(mFindParam);
	
	//JIS�n�e�L�X�g�G���R�[�f�B���O�̂Ƃ��́A��Ƀo�b�N�X���b�V���^���𖳎�����B
	ATextEncoding	tec = ATextEncodingWrapper::GetTextEncodingFromName(tecname);
	if( tec == ATextEncodingWrapper::GetSJISTextEncoding() ||
	   tec == ATextEncodingWrapper::GetJISTextEncoding() ||
	   tec == ATextEncodingWrapper::GetEUCTextEncoding()  )
	{
		findParam.ignoreBackslashYen = true;
	}
	
	//���@�F��
	ATextInfo	textInfo(NULL);
	textInfo.SetMode(modeIndex);
	ABool	abortFlag = false;
	textInfo.CalcLineInfoAllWithoutView(text,abortFlag);//win
	ABool	syntaxRecognized = false;//#730 �������B�������ʂ�����Ƃ��������@�F������
	mTotalLineCount += textInfo.GetLineCount();
	
	//���ʎ�M�I�u�W�F�N�g����
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
		//���f����
		if( NVM_IsAborted() == true )   return false;
		
		//���΂炭�x�e�i���C���X���b�h�ɓ����Ă��炤�j
		if( mSleepForAWhile == true )
		{
			NVM_SleepWithTimer(2);
			mSleepForAWhile = false;
		}
		
		//�������s
		ATextIndex	resultspos, resultepos;
		ABool	modalSessionAborted = false;//#695
		if( mTextFinder.ExecuteFind(text,findParam,false,spos,epos,resultspos,resultepos,mAborted,false,modalSessionAborted) == false )   break;//B0359
		
		//��v�ӏ����o
		if( mExtractMatchedText == true )
		{
			AText	t;
			text.GetText(resultspos,resultepos-resultspos,t);
			mExtractedText.AddText(t);
		}
		
		//�������ʂ��P�ȏ゠��Ƃ��̂݁A�ŏ��̈�񕶖@�F������悤�ɂ��� #730
		if( syntaxRecognized == false && mAbortRecognizeSyntax == false )//#890
		{
			if( findParam.displayPosition == true && text.SuspectBinaryData() == false )//#898 utf8ok == true && text.GetItemCount() < 1024*1024 )//B0313 1MB�ȏ��������UTF8�e�L�X�g�Ƃ��Ă܂��Ƃ��łȂ��Ƃ��̓o�C�i���t�@�C���̉\���������̂�RecognizeSyntax()���Ȃ� #395
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
		
		//���ʃf�[�^�쐬
		inFile.GetPath(filepath);
		text.GetText(resultspos,resultepos-resultspos,hit);
		
		//���Ӄe�L�X�g�擾
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
		
		//�i��Index���擾
		ATextPoint	textpoint;
		textInfo.GetTextPointFromTextIndex(resultspos,textpoint);
		AIndex	paragraphIndex = textInfo.GetParagraphIndexByLineIndex(textpoint.y);
		/*
		//#911 �O�i���ƁA���i���܂ł����Ӄe�L�X�g�Ƃ��Ď擾���邱�Ƃɂ��遨����ς��߂��B�}���`�t�@�C���������ʂ͏]���ʂ�̕\���Ƃ���B
		ATextPoint	spt = {0,textpoint.y-1};
		if( spt.y < 0 )   spt.y = 0;
		ATextIndex	sp = textInfo.GetTextIndexFromTextPoint(spt);
		ATextPoint	ept = {0,textpoint.y+2};
		if( ept.y >= textInfo.GetLineCount() )   ept.y = textInfo.GetLineCount()-1;
		ATextIndex	ep = textInfo.GetTextIndexFromTextPoint(ept);
		text.GetText(sp,resultspos-sp,prehit);
		text.GetText(ep,ep-resultepos,posthit);
		*/
		//�ꏊ���擾
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
		//���ʃf�[�^�i�[
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
	
	//���ʂ����C���X���b�h�֑��M
	if( found == true )
	{
		AObjectIDArray	objectIDArray;
		objectIDArray.Add(receiverID);
		ABase::PostToMainInternalEventQueue(kInternalEvent_MultiFileFindResult,GetObjectID(),0,tecname,objectIDArray);
		
		//#828 �t�@�C����+1
		mHitFileCount++;
	}
	else
	{
		//������Ȃ���Ό��ʑ��M���Ȃ�
		GetApp().SPI_Delete1ThreadDataReceiver_MultiFileFindResult(receiverID);
	}
	return true;
}

#pragma mark ===========================

#pragma mark ---���f

//#890
/**
�����r�����f
*/
void	AMultiFileFinder::NVIDO_AbortThread()
{
	SPI_AbortRecognizeSyntax();
}

void	AMultiFileFinder::SPI_SleepForAWhile()
{
	mSleepForAWhile = true;
}

